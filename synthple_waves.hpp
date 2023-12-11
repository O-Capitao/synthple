#pragma once

#include <vector>
#include <spdlog/spdlog.h>

#include <synthple_globals.hpp>

namespace synthple::waves {

    // contains the numeric definition of a wave
    // mean to be extended
    // 
    // NORMALIZED WAVE FORM - between -1 and 1
    class Wave {

        protected:
            float _freq_hz, _freq_rad_s, _period;

        public:
            Wave( float freqHz );
            virtual float getValue( float t ) = 0;
    };

    class SineWave : public Wave {
        public:
            SineWave( float freqHz );
            float getValue( float t );
    };

    class WaveGenerator
    {
        protected:
    
            std::shared_ptr<spdlog::logger >_logger;
            // each time there is a frequency change we need to change
            // the wave
            // we should only change when slopes are equal
            SineWave _wave = NULL; 
            SineWave _new_wave = NULL;
            
            bool _is_silent = true;
            bool _is_transient = false;
            bool _is_fadingIn = false;
            bool _is_fadingOut = false;

            // the generator's own inner clock, starting at zero to avoid pops
            // reset every time the wave changes
            float _int_t = 0;
            
            // generator's value
            float _value = 0;
            float _gain = 0;

            // amp threshold for note change
            // (normalized amplitude)
            float _change_amp_thresh = 0.99999;

            // transition from/ into silence

            // fades work by making
            // the _fade_factor vary between 0 and 1
            // in the time interval _fade_legth
            // resulting in _fade_step_n increments/ decrements
            // of value _fade_increment_val;
            const float _fade_length = 0.01;
            const float _fade_step_n = 1 / _fade_length;
            const float _fade_increment_val = 1 / _fade_step_n;

            float _fade_factor = 0.0;
            float _t_fade = 0.0;


        public:
            WaveGenerator( float gain );

            float getValue(){ 
                return _value;
            };
            void step( float dt );
            void requestFreqChange( float new_freq_hz );
            void requestSilence();
    };
}