#include <synthple_mixer.hpp>

using namespace synthple::mixer;

// Track //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////




// Mixer //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Mixer::loadSong( filedata::SongFileData *_sfd ){
    
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

            std::pair<std::string, midi::MidiFileWrapper> __pair(__tfd.voice_id,  midi::MidiFileWrapper( __tfd.midi_file_path));
            // __section._midiFiles_perTrack_map[ __tfd.voice_id ] = midi::MidiFileWrapper( __tfd.midi_file_path );
            __section._midiFiles_perTrack_map.insert( __pair );

        }

        for (int __voice_i = 0; __voice_i < _sfd->voices.size(); __voice_i++){
            filedata::VoiceFileData &__vfd = _sfd->voices[__voice_i];

            _tracks.push_back({
                .gain = __vfd.gain ,
                .oscillator = oscillator::Oscillator( __vfd.type, __vfd.n_samples
                    // .waveTableConfig = {
                    //     .type = config::mapStringToWaveTableType( __vfd.type ),
                    //     .n_samples = __vfd.n_samples,
                    // },
                    // .freq = 440,
                    // .amp = 0.5
                )
            });

        }

    }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Mixer::produceData( float *requestedsamples_vector, int requestedsamples_len ){
    for (int i = 0; i < requestedsamples_len; i++){
        requestedsamples_vector[i] = 0;
    }
}