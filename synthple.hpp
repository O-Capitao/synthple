#pragma once

#include <vector>
#include <spdlog/spdlog.h>

#include <synthple_globals.hpp>
#include <synthple_bus.hpp>
#include <synthple_audio.hpp>

namespace synthple {

    class Generator
    {
        protected:
            std::shared_ptr<spdlog::logger >_logger;
            float _amp;
            float _freq_hz;
            float _freq_rad_s;
            float _period;

        public:
            Generator( float amp, float freq );
            ~Generator();
            virtual float getValueAtTime( float t ) = 0;
    };

    class SineGenerator: public Generator
    {
        public:
            SineGenerator( float amp, float freq );
            ~SineGenerator();
            float getValueAtTime( float t );
    };

    class SquareGenerator: public Generator
    {
        public:
            SquareGenerator( float amp, float freq );
            ~SquareGenerator();
            float getValueAtTime( float t );
    };
    


    class Synthple{
        
        bool _MAIN_QUIT = false;
        
        std::shared_ptr<spdlog::logger >_logger;
        audio::AudioThread _audioThread;
        bus::AudioDataBus *_audioDataBus_ptr;
        SineGenerator _generator;

        float _totalTime_s;

        public:
            Synthple( bus::AudioDataBus *audioDataBus );
            ~Synthple();
            void run();
            void quit();
    };
}