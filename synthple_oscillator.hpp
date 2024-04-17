#pragma once

#include <string>
#include <vector>
#include <spdlog/spdlog.h>
#include <synthple_config.hpp>

namespace synthple::oscillator {
    
    class WaveTable {

        std::shared_ptr<spdlog::logger >_logger;
        config::WaveTableConfig _config;

        std::vector<float> _data;

        void _generateSinWaveform();
        void _generateSquareWaveform();

        public:
            WaveTable( config::WaveTableConfig cnf);
            float lookupValueAt( short index );
    };

    class Oscillator {

        std::shared_ptr<spdlog::logger >_logger;
        const config::OscillatorConfig _config;
        
        WaveTable _waveTable;

        float _outputFreq = 1;
        float _outputPeriod = 1;
        float _outputAmp = 1;
        bool _outputSilence = false;

        bool _isSilenceRequested = false;
        short _lastIndex = 0;

        public:
            Oscillator( config::OscillatorConfig cnf );

            // OUTPUT
            float getValueAt(float t_s);
            
            // controls
            void setFrequency( float newfreq );
            void setAmp( float newamp );
            void requestSilence();
    };
}