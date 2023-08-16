#pragma once

#include <portaudio.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include  <synthple_bus.hpp>

namespace synthple::audio {
    
    struct InternalAudioData {
        bus::AudioData_Queue *input_queue;
    };

    class AudioThread {

        std::shared_ptr<spdlog::logger >_logger;
        InternalAudioData *_data = NULL;
        PaStream *_stream = NULL;
        bus::AudioData_Queue *_inputQueue;
        bus::Commands_Queue *_outputQueue;

        void _openStream();
        void _closeStream();
        void _startStream();
        void _stopStream();

        public:

            AudioThread( 
                bus::AudioData_Queue *inqueue,
                bus::Commands_Queue *outqueue
            );
            
            ~AudioThread();

            void run();

            static int paStreamCallback( 
                const void *inputBuffer,
                void *outputBuffer,
                unsigned long framesPerBuffer,
                const PaStreamCallbackTimeInfo* timeInfo,
                PaStreamCallbackFlags statusFlags,
                void *userData );
    };
}