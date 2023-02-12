#pragma once

#include <synthple_config.hpp>

namespace synthple {
    namespace synth {

        class Oscilator {

            config::Wave_Shape _shape;

            public:

                float produceValue();

        };

        class Synth {

            public:
                config::SynthConfig synth_config;


        };
    }
}