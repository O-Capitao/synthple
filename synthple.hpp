#pragma once

#include <vector>
#include <spdlog/spdlog.h>

#include <synthple_globals.hpp>
#include <synthple_bus.hpp>
#include <synthple_audio.hpp>

namespace synthple {

    class Generator{
        float _amp;
        float _freq_hz;
        float _freq_rad_s;
        float _period;

        public:
            Generator( float amp, float freq );
            ~Generator();
            float getValueAtTime( float t );
    };
    
    class Synthple{
        
        bool _MAIN_QUIT = false;
        
        audio::AudioThread _audioThread;
        bus::AudioDataBus *_audioDataBus_ptr;
        Generator _generator;

        float _totalTime_s;

        public:
            Synthple( bus::AudioDataBus *audioDataBus );
            ~Synthple();
            void run();
            void quit();
    };
}