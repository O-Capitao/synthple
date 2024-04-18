#pragma once

#include <vector>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <string>

#include <synthple_globals.hpp>
#include <synthple_bus.hpp>
#include <synthple_audio.hpp>   
#include <synthple_config.hpp>
#include <synthple_oscillator.hpp>
#include <synthple_config.hpp>

namespace synthple {
       
    class Synthple {

        bool _MAIN_QUIT = false;
        bool _QUIT_REQUESTED = false;

        std::shared_ptr<spdlog::logger >_logger;
        
        bus::AudioDataBus           _audioDataBus;
        audio::AudioThread          _audioThread;
        filedata::SynthpleFileData  _filedata;

        std::vector<oscillator::Oscillator>         _oscillators;
        std::unordered_map<std::string,song::Song>  _songs;
        std::string _activeSong_id, 
            _activePart_id;



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