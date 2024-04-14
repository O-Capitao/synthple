#pragma once

namespace synthple::config {

    enum WaveTableType {
        SINE,
        SQUARE,
        SAW,
        TRIANGLE,
        CUSTOM
    };

    struct WaveTableConfig {
        WaveTableType type;
        short n_samples;
    };

    struct OscillatorConfig{

        WaveTableConfig waveTableConfig;
        
        float freq;
        float amp;

    };
}