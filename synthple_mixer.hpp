#pragma once

#include <string>
#include <vector>
#include <synthple_config.hpp>
#include <synthple_oscillator.hpp>
#include <midi/synthple_midi.hpp>

namespace synthple::mixer {

    class Track {

        oscillator::Oscillator _oscillator;
        float _gain;

        public:
            Track();
            float getValueAt( float timeinloop_s );
    };

    struct Part {
        short repeat;
        midi::MidiFileWrapper _mfw;
        // Part *nextPart;
    };

    class Mixer {

        Mixer();

        std::vector<Track> _tracks;
        
        int _timeInSong;

        public:
            void loadSong( filedata::SongFileData _sfd );
            void setPart( std::string partid );
            

    };

}