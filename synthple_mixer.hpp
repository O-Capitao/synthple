#pragma once

#include <string>
#include <vector>
#include <synthple_config.hpp>
#include <synthple_oscillator.hpp>
#include <midi/synthple_midi.hpp>
#include <spdlog/spdlog.h>

namespace synthple::mixer {

    struct Track {
        oscillator::Oscillator oscillator;
        midi::MonophonicMidiFileReader *midi_fw_ptr = nullptr;
        midi::MidiNote *last_played_note_ptr = nullptr; 
        midi::MidiNote *curr_note_ptr = nullptr;

        float gain;
        bool is_silent = true;
    };

    struct Section {
        short repeat;
        short length_bars;
        float length_s;
        // std::unordered_map<std::string, midi::MidiFileWrapper>  _midiFiles_perTrack_map;
        std::vector<midi::MonophonicMidiFileReader> _midiFiles_perTrack;
    };

    class Mixer {

        std::shared_ptr<spdlog::logger >_logger;

        std::vector<Track> _tracks;
        std::vector<Section> _sections;

        std::string _loaded_song_name = "";

        filedata::SongFileData *_sfd_ptr;
        int _loaded_section_index = 0;

        float _timeInSong_s, 
            _timeInSection_s,
            _loadedSectionDuration_s;
        
        const float _dt_s = 1.0f / (float)FRAMERATE;
        const int _input_period_in_samplerates = 100;
        int _midi_click_counter = 0;

        short _sectionRepeat_count = 0;
        float _tempo_bpm = 0;
        bool _is_silent = true;

        NoteFrequency _note_frequency_map;

        public:  
            // constructs a silent mixer.
            Mixer();
            
            void setSong( filedata::SongFileData *_sfd );
            void setSection( int sectionindex );
            void setSilence();

            const std::string &getCurrentSectionName();
            void produceData( float *requestedsamples_vector, int requestedsamples_len );
    };

}