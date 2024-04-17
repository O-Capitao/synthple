#pragma once

#include <vector>
#include <spdlog/spdlog.h>
#include <string>

#include <synthple_globals.hpp>
#include <synthple_bus.hpp>
#include <synthple_audio.hpp>
// #include <midi/synthple_midi.hpp>
#include <synthple_config.hpp>
// #include <synthple_oscillator.hpp>
#include <synthple_config.hpp>

namespace synthple {
       
    class Synthple {

        bool _MAIN_QUIT = false;
        bool _QUIT_REQUESTED = false;

        // config::OscillatorConfig _oscillatorConfig;
        // oscillator::Oscillator _oscillator;

        std::shared_ptr<spdlog::logger >_logger;
        
        bus::AudioDataBus _audioDataBus;
        audio::AudioThread _audioThread;

        filedata::SynthpleFileData _filedata;
        // midi::MidiFileWrapper _midi_file;

        // int _bpm;
        // float _totalTime_s;

        public:
            Synthple(std::string path_to_config);
            // ~Synthple();

            // void run();
            // void quit();

            void init();
            void setSong(std::string &song_to_set);
            void setSongPart(std::string &part_to_set);
            void pause();
            void stop();
            void close();
    };  

}