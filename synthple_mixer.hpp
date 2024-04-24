#pragma once

#include <string>
#include <vector>
#include <synthple_config.hpp>
#include <synthple_oscillator.hpp>
#include <midi/synthple_midi.hpp>

namespace synthple::mixer {

    struct Track {

        oscillator::Oscillator _oscillator;
        float _gain;

        Track();
        // float getValueAt( float timeinloop_s );
    };

    struct Section {
        short repeat;
        std::unordered_map<std::string, midi::MidiFileWrapper>  _midiFiles_perTrack_map;
    };

    class Mixer {

        std::vector<Track> _tracks;
        std::vector<Section> _sections;

        Section *_activeSection_ptr;

        float _timeInSong_s, _timeInSection_s;
        short _sectionRepeat_count;

        public:
            // Mixer();
            void loadSong( filedata::SongFileData *_sfd );

            // override "natural" mixer state,
            // e.g. forcibly change part
            void setSection( std::string sectionid );

            const std::string &getCurrentSectionName();
            void produceData( std::vector<float> *requestedsamples_vector, int requestedsamples_len );
            
    };

}