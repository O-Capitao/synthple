#pragma once

#include <string>

#include <portaudio.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <boost/lockfree/spsc_queue.hpp>

#include <synthple_synth.hpp>

namespace synthple {

    struct InternalAudioData {

    };

    class Synthple{
        const std::string _config_file_path;

        public:

            Synthple( const std::string &config_file_path );
            ~Synthple();


    };
}