#pragma once

#include <vector>
#include <spdlog/spdlog.h>

#include <string>

#include <synthple_globals.hpp>
#include <synthple_bus.hpp>
#include <synthple_audio.hpp>
#include <synthple_config.hpp>
#include <synthple_mixer.hpp>

namespace synthple {
       
    class Synthple {

        bool                            _MAIN_QUIT = false;
        bool                            _QUIT_REQUESTED = false;

        std::shared_ptr<spdlog::logger >_logger;

        bus::AudioDataBus               _audioDataBus;
        bus::CommandBus                 _commandBus;
        audio::AudioThread              _audioThread;
        filedata::SynthpleFileData      _filedata;
        mixer::Mixer                    _mixer;
        std::string                     _activeSong_id,
                                        _activeSongSection_id;


        // audio runtime lifecycle
        void _init();
        void _close();
        void _run();

        public:
            Synthple( std::string path_to_config );

            // accepted commands
            void setSong( const std::string &song_to_set );
            void setSongSection( const std::string &_sectionid );
            void play();
            void pause();
            void stop();
    };
}