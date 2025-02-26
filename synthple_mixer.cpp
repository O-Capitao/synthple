#include <synthple_mixer.hpp>
#include <spdlog/sinks/basic_file_sink.h>

using namespace synthple::mixer;

// Track
Track::Track( filedata::VoiceFileData &_vfd ){

    gain = _vfd.gain;

    if (_vfd.type == "SQUARE"){
        oscillator_ptr = std::make_shared<oscillator::SquareOscillator>( _vfd.inflection );
    } else if (_vfd.type == "TRIANGLE"){
        oscillator_ptr = std::make_shared<oscillator::TriangleOscillator>( _vfd.inflection );
    }else if(_vfd.type == "SIN"){
        oscillator_ptr = std::make_shared<oscillator::SinOscillator>( _vfd.n_samples );
    }else{
        throw std::runtime_error("Bad Oscilator type defined");
    }
}

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
void Mixer::init( bus::CommandBus *cmd_bus  )
{
    _commandBus = cmd_bus;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Mixer::setSong( filedata::SongFileData *_sfd )
{
    _sfd_ptr = _sfd;
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
        _tracks.push_back( Track( __vfd ) );
    }

    setSection(0);

    _logger->debug("Mixer setup complete, loaded {} tracks, {} sections", _tracks.size(), _sections.size());
    _logger->flush();
}


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Mixer::setSection(int sectionindex){

    _logger->debug("setSection={}", sectionindex);
    _logger->flush();

    _loaded_section_index = sectionindex;
    Section &__loaded_sect = _sections[_loaded_section_index];
    midi::MonophonicMidiFileReader *__first_mfw = __loaded_sect._midiFiles_perTrack.data();

    _loadedSectionDuration_s = __loaded_sect.length_bars * _sfd_ptr->beats_per_bar * 60.0 / _sfd_ptr->bpm;

    // assign midi to tracks
    for (int i = 0; i < __loaded_sect._midiFiles_perTrack.size(); i ++ ){
        _tracks[i].midi_fw_ptr = &__first_mfw[i];
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

    // _logger->debug("produce Data Enter");

    if ( _is_silent ){
        // nothing is loaded, just produce silence.

        for (int i = 0; i < requestedsamples_len; i++){
            requestedsamples_vector[i] = 0;
        }

    } else {

        Section &__loaded_sect = _sections[_loaded_section_index];
        
        float __aux_freq = 0, __tgt_freq = 0;

        for (int i = 0; i < requestedsamples_len; i++){
            
            if (_midi_click_counter++ % _input_period_in_samplerates == 0){
                
                // time to process midi and set track statuses
                for (int j = 0; j < __loaded_sect._midiFiles_perTrack.size(); j ++){

                    Track &__track = _tracks[j];
                    __track.curr_note_ptr = __track.midi_fw_ptr->getStateAt_Time_s( _timeInSection_s );

                    if ( !__track.is_silent && __track.curr_note_ptr->note == NoteKey::NOT_A_NOTE ){

                        // do nothing
                        __track.is_silent = true;
                        __track.last_played_note_ptr = __track.curr_note_ptr;
                        _logger->debug("Requesting Silence for Oscillator \"{}\" at T=\"{}\"s", j, _timeInSection_s);
                        _logger->flush();

                    } else if (
                        __track.last_played_note_ptr == nullptr 
                        ||  (__track.last_played_note_ptr != nullptr && __track.curr_note_ptr->note != __track.last_played_note_ptr->note)
                    ){
                                                
                        __tgt_freq = _note_frequency_map.noteFreqMap[ __track.curr_note_ptr->note_value ];
                        assert(__tgt_freq > 0);
                        
                        __aux_freq = _note_frequency_map.noteFreqMap[ __track.curr_note_ptr ->note_value ];

                        _logger->debug("Requesting new Freq for Oscillator \"{}\" at T=\"{}\"s", j, _timeInSection_s);
                        _logger->flush();

                        __track.oscillator_ptr->setFrequency(__aux_freq );
                        __track.last_played_note_ptr = __track.curr_note_ptr ;
                        __track.is_silent = false;
                    }
                }
            }
            // end event processing


            // mixdown for each track
            float __mixed_values = 0;
            for (int j = 0; j < _tracks.size(); j++){
                Track &__track = _tracks[j];

                __mixed_values += __track.is_silent ? 0 : __track.gain * __track.oscillator_ptr->getValueAt(_timeInSection_s);

            }

            requestedsamples_vector[i] = __mixed_values;
            assert(__mixed_values < 1.0);
            float __section_t_spillover = _timeInSection_s - _loadedSectionDuration_s;

            if (__section_t_spillover < 0 ){
                _timeInSection_s += _dt_s;
            } else {

                Section &_loaded_section = _sections[_loaded_section_index];
                // special case for:
                if (_loaded_section.repeat != 0){
                    if ( _loaded_section_repeat_count == _loaded_section.repeat ){
                        // time to go to next section

                        // is there a next section
                        if ( _loaded_section_index <= _sfd_ptr->sections.size() ){

                            _commandBus->queue.push({
                                .cmdType= bus::CommandType::SET_SECTION,
                                .arg= std::to_string( _loaded_section_index + 1 )
                            });

                        } else {
                            // song has finished, stop playback or output silence
                            _commandBus->queue.push({
                                .cmdType= bus::CommandType::STOP,
                                .arg= ""
                            });
                        }
                    }
                }

                _logger->debug("spillover of {} at {}", __section_t_spillover, _timeInSection_s );
                
                _loaded_section_repeat_count++;
                _timeInSection_s = __section_t_spillover;
            }
        }
    }
}