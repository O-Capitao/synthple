#pragma once

#include <portaudio.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include  <synthple_bus.hpp>

namespace synthple::audio {
    
    struct InternalAudioData {
        bus::AudioDataBus *inputBus;
    };

    class AudioThread { // TODO: rename to AudioEngine

        std::shared_ptr<spdlog::logger >_logger;
        InternalAudioData *_data = NULL;
        PaStream *_stream = NULL;

        void _openStream();
        void _closeStream();
        void _startStream();
        void _stopStream();

        public:

            AudioThread();
            ~AudioThread();

            void init(bus::AudioDataBus *in_bus);
            void start();
            void stop();

            static int paStreamCallback( 
                const void *inputBuffer,
                void *outputBuffer,
                unsigned long framesPerBuffer,
                const PaStreamCallbackTimeInfo* timeInfo,
                PaStreamCallbackFlags statusFlags,
                void *userData );
    };
}