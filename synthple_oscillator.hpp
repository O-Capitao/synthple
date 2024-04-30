#pragma once

#include <string>
#include <vector>
#include <spdlog/spdlog.h>
#include <synthple_config.hpp>

namespace synthple::oscillator {
    
    enum WaveTableType {
        SINE,
        SQUARE,
        SAW,
        TRIANGLE,
        CUSTOM
    };

    class WaveTable {

        std::shared_ptr<spdlog::logger >_logger;

        int _n_samples;
        WaveTableType _wt_type;

        std::vector<float> _data;

        void _generateSinWaveform();
        void _generateSquareWaveform();

        public:
            WaveTable( int nsamples, std::string wavetabletype_str, std::shared_ptr<spdlog::logger > logger );
            float lookupValueAt( short index );
            static WaveTableType mapStringToWaveTableType( std::string wtt_str );
            int get__n_samples(){ return _n_samples; }
    };

    class Oscillator {

        std::shared_ptr<spdlog::logger >_logger;
        // const config::OscillatorConfig _config;
        
        WaveTable _waveTable;

        float _outputFreq = 1;
        float _outputPeriod = 1;
        // float _outputAmp = 1;
        bool _outputSilence = false;

        bool _isSilenceRequested = false;
        short _lastIndex = 0;

        public:
            Oscillator(
                std::string wavetabletype_str,
                int wavetable_nsamples,
                std::shared_ptr<spdlog::logger > logger
            );

            // OUTPUT
            float getValueAt(float t_s);
            
            // controls
            void setFrequency( float newfreq );
            void setAmp( float newamp );
            void requestSilence();
    };
}