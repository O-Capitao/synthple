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

    //
    //  1 |_  _  _ .
    //    |      . |  .
    //    |    .   |     .
    //    |  .     |        .
    //    |.       |           .
    //    | _ _ _ _ _ _ _ _ _ _ _  1  
    //      -------  _offset
    //
    // _offset = 0 or 1 -> sawtooth

    // class TriangleOscillator: public BaseOscillator {
    //     float _offset;

    //     public:
    //         TriangleOscillator( int osc_id, float offset);
        
    //         float getValueAt(float t_s);
    // };

    class BaseOscillator {

        protected:
            // std::shared_ptr<spdlog::logger >_logger;
            // std::string _label;

            float _outputFreq = 1;
            float _outputPeriod = 1;

            bool _outputSilence = false;
            bool _isSilenceRequested = false;
            short _lastIndex = 0;

        public:
            // BaseOscillator(std::string l);

            virtual float getValueAt(float t_s) = 0;
            
            void setFrequency( float newfreq );
            void requestSilence();

    };

    class SquareOscillator: public BaseOscillator {
        float _offset;

        public:
            SquareOscillator( float offset = 0.85 );
            float getValueAt(float t_s);
    };

}