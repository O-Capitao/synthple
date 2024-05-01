#pragma once

#include <vector>
#include <string>

#include <boost/thread/thread.hpp>
#include <spdlog/spdlog.h>

#include <synthple_globals.hpp>
#include <synthple_bus.hpp>
#include <synthple_audio.hpp>
#include <synthple_config.hpp>
#include <synthple_mixer.hpp>

namespace synthple {
       
    class Synthple {

        std::shared_ptr<spdlog::logger >_logger;

        bus::AudioDataBus               _audioDataBus;
        bus::CommandBus                 _commandBus;
        audio::AudioThread              _audioThread;
        filedata::SynthpleFileData      _filedata;
        mixer::Mixer                    _mixer;
        std::string                     _activeSong_id;
        int                             _active_song_section_index;

        boost::thread _playThread;

        void _run();
        void _pushToAudioDataBus( float *topush_data_arr, int topush_length  );

        public:
            Synthple( std::string path_to_config );
            
            void init();
            void close();
            
            // accepted commands
            void setSong( const std::string &song_to_set );
            void setSongSection( int sectionindex );
            void stop();
    };
}