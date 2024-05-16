#include <synthple_mixer.hpp>
#include <spdlog/sinks/basic_file_sink.h>

using namespace synthple::mixer;


// Mixer //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
Mixer::Mixer()
:_logger(spdlog::basic_logger_mt("Mixer", "synthple.log"))
{
    _logger->set_level(spdlog::level::debug);
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Mixer::setSong( filedata::SongFileData *_sfd )
{
    
    _timeInSong_s = 0;
    _timeInSection_s = 0;

    _tempo_bpm = _sfd->bpm;
    _loaded_song_name = _sfd->id;

    int _midireader_id = 0;
    int _oscillator_id = 0;

    for (int __section_index = 0; __section_index < _sfd->sections.size(); __section_index++){
        Section __section = {
            .repeat = _sfd->sections[ __section_index ].repeat,
            .length_bars = _sfd->sections[ __section_index ].length_bars,
        };

        for (int __midi_ass_ind = 0; __midi_ass_ind < _sfd->sections[__section_index].tracks.size(); __midi_ass_ind++){
            filedata::TrackFileData &__tfd = _sfd->sections[__section_index].tracks[__midi_ass_ind];
            __section._midiFiles_perTrack.push_back( 
                midi::MonophonicMidiFileReader( 
                    _midireader_id++, 
                    __tfd.midi_file_path,
                    _tempo_bpm,
                    _input_period_in_samplerates*_dt_s,
                    __section.length_bars,
                    _sfd->beats_per_bar )
            );
        }

        _sections.push_back(__section);
    }

    for (int __voice_i = 0; __voice_i < _sfd->voices.size(); __voice_i++){
        filedata::VoiceFileData &__vfd = _sfd->voices[__voice_i];

        _tracks.push_back({
            .gain = __vfd.gain,
            .oscillator = oscillator::Oscillator( _oscillator_id, __vfd.type, __vfd.n_samples )
        });
    }

    setSection(0);

    _logger->debug("Mixer setup complete, loaded {} tracks, {} sections", _tracks.size(), _sections.size());
    _logger->flush();
}


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Mixer::setSection(int sectionindex){

    _loaded_section_index = sectionindex;
    Section &__loaded_sect = _sections[_loaded_section_index];
    midi::MonophonicMidiFileReader *__first_mfw = __loaded_sect._midiFiles_perTrack.data();

    // assign midi to tracks
    for (int i = 0; i < __loaded_sect._midiFiles_perTrack.size(); i ++ ){
        _tracks[i].midi_fw_ptr = &__first_mfw[i];
        // input is processed in intervals of "_input_period_in_samplerates" clicks.
        // _tracks[i].midi_fw_ptr->resetToTicks(0);
    }

    _is_silent = false;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Mixer::setSilence(){
    _is_silent = true;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Mixer::produceData( float *requestedsamples_vector, int requestedsamples_len ){

    if ( _is_silent ){
        // nothing is loaded, just produce silence.

        for (int i = 0; i < requestedsamples_len; i++){
            requestedsamples_vector[i] = 0;
        }
    
    } else {
        
        Section &__loaded_sect = _sections[_loaded_section_index];

        for (int i = 0; i < requestedsamples_len; i++){

            if (_midi_click_counter++ % _input_period_in_samplerates == 0){

                // time to process midi and set track statuses
                for (int j = 0; j < __loaded_sect._midiFiles_perTrack.size(); j ++){

                    Track &__track = _tracks[j];

                    // midi::MidiNote __next_active_note = __track.midi_fw_ptr->getNextActiveNote();
                    midi::MidiNote *_curr_note = __track.midi_fw_ptr->getStateAt_Time_s( _timeInSection_s );

                    if (_curr_note != __track.last_played_note_ptr){

                        // Change oscillator state
                        float __tgt_freq = _note_frequency_map.noteFreqMap[ _curr_note->note_value ];

                        assert(__tgt_freq > 0);
                        
                        __track.oscillator.setFrequency( _note_frequency_map.noteFreqMap[ _curr_note->note_value ] );

                        __track.last_played_note_ptr = _curr_note;
                        __track.is_silent = false;

                        __track.last_played_note_ptr = _curr_note;

                     } else if ( _curr_note == nullptr ){

                        // set to silence.
                        __track.is_silent = true;
                     }
                }
            }
            // end event processing

            // mixdown for each track
            float __mixed_values = 0;
            for (int j = 0; j < _tracks.size(); j++){
                Track &__track = _tracks[j];

                __mixed_values += __track.is_silent ? 0 : __track.gain * __track.oscillator.getValueAt(_timeInSection_s);

            }

            assert(__mixed_values < 1.0);
            requestedsamples_vector[i] = __mixed_values;

            float __section_t_spillover = ( (_sections[_loaded_section_index].length_bars * 4 * 60 )/_tempo_bpm) - _timeInSection_s;

            if (__section_t_spillover > 0 ){
                _timeInSection_s += _dt_s;
            } else {
                _timeInSection_s = __section_t_spillover;
            }
        }
    }
}