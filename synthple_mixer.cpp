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

    for (int __section_index = 0; __section_index < _sfd->sections.size(); __section_index++){
        Section __section = {
            .repeat = _sfd->sections[ __section_index ].repeat
        };

        for (int __midi_ass_ind = 0; __midi_ass_ind < _sfd->sections[__section_index].tracks.size(); __midi_ass_ind++){
            filedata::TrackFileData &__tfd = _sfd->sections[__section_index].tracks[__midi_ass_ind];
            __section._midiFiles_perTrack.push_back( midi::MidiFileWrapper( __tfd.midi_file_path, _logger ));
        }

        _sections.push_back(__section);
    }

    for (int __voice_i = 0; __voice_i < _sfd->voices.size(); __voice_i++){
        filedata::VoiceFileData &__vfd = _sfd->voices[__voice_i];

        _tracks.push_back({
            .gain = __vfd.gain,
            .oscillator = oscillator::Oscillator( __vfd.type, __vfd.n_samples, _logger )
        });
    }

    _logger->debug("Mixer setup complete, loaded {} tracks, {} sections", _tracks.size(), _sections.size());
    _logger->flush();
}


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Mixer::setSection(int sectionindex){

    _loaded_section_index = sectionindex;
    Section &__loaded_sect = _sections[sectionindex];
    midi::MidiFileWrapper *__first_mfw = __loaded_sect._midiFiles_perTrack.data();

    // assign midi to tracks
    for (int i = 0; i < __loaded_sect._midiFiles_perTrack.size(); i ++ ){
        _tracks[i].midi_fw_ptr = &__first_mfw[i];

        // input is processed in intervals of "_input_period_in_samplerates" clicks.
        _tracks[i].midi_fw_ptr->initSequentialRead(_input_period_in_samplerates * _dt_s);
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

    if ( !_is_silent ){
        // nothing is loaded, just produce silence.

        for (int i = 0; i < requestedsamples_len; i++){
            requestedsamples_vector[i] = 0;
        }
    
    } else {
        std::runtime_error("asd");

        

    }
}