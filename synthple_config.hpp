#pragma once

namespace synthple {
    namespace config {
        
        struct ADSR_Envelope {
            int attack;
            int decay;
            int sustain;
            int release;
        };

        enum Wave_Shape {
            SQUARE,
            SINE,
            TRIANGLE
        };

        class SynthConfig {
            public:
                ADSR_Envelope envelope;
                Wave_Shape wave_shape;
        };

        class PlayerConfig {
            int framerate;

        };

    }
}