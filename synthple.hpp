#pragma once

#include <vector>
#include <spdlog/spdlog.h>

#include <midi/synthple_midi.hpp>
#include <synthple_globals.hpp>
#include <synthple_bus.hpp>

namespace synthple {

    class Synthple_Clock {
        unsigned long _ticks_start;
        
        public:
            Synthple_Clock();
            void resetClock();
            unsigned long getTicks();            
    };
    
    class Oscillator {
        
        float _amp = 0;
        float _t = 0;
        float _freq = 0;
        float _internal_value;

        public:
            float produceVal();
            void setFrequency( float newFreq );
            void setAmp( float newAmp );
            void reset();
    };
    
    class Synthple {

        // both queues used for
        // communication between main and audio threads
        bus::AudioData_Queue *_sample_queue_ptr = nullptr;
        bus::Commands_Queue *_cmd_queue_ptr = nullptr;

        std::shared_ptr<spdlog::logger> _logger;
        midi::MidiFileWrapper _midiFile;
        std::vector<Oscillator> _oscilators;
        ushort _n_oscilators;

        bool _playing = false;
        
        const double _dt_s = 1 / FRAMERATE;
        const double _buffer_duration = FRAMES_IN_BUFFER / FRAMERATE;

        public:

            void run();

            Synthple(
                midi::MidiFileWrapper &midiFile,
                bus::AudioData_Queue *sample_queue_ptr,
                bus::Commands_Queue *cmd_queue_ptr
            );

            ~Synthple();

    };
}