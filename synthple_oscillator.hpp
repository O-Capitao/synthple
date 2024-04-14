#pragma once

#include <string>
#include <spdlog/spdlog.h>
#include <synthple_config.hpp>

namespace synthple::oscillator {
    
    class WaveTable {

        std::shared_ptr<spdlog::logger >_logger;
        config::WaveTableConfig _config;

        float *_data;


        void _generateSinWaveform();
        void _generateSquareWaveform();

        public:
            WaveTable( config::WaveTableConfig cnf);
            ~WaveTable();

            float lookupValueAt( short index );

    };

    class Oscillator {

        std::shared_ptr<spdlog::logger >_logger;
        const config::OscillatorConfig _config;
        
        WaveTable _waveTable;
        float _outputFreq = 1;
        float _outputPeriod = 1;
        float _outputAmp = 1;

        public:
            Oscillator( config::OscillatorConfig cnf );

            float getValueAt(float t_ms);
            
            void setFrequency( float newfreq ){ _outputFreq = newfreq; }
            void setAmp( float newamp ){ _outputAmp = newamp; }
    };
}