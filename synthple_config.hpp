#pragma once

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>
#include <unordered_map>

namespace synthple::filedata {

    struct VoiceFileData {
        std::string id;
        std::string type;
        
        short n_samples;
        float offset;
        float gain;
    };

    struct TrackFileData {
        std::string voice_id;
        std::string midi_file_path;
    };

    struct SectionFileData {
        std::string id;
        short repeat;
        short length_bars;
        std::vector<TrackFileData> tracks;
    };

    struct SongFileData {
        short bpm;
        std::string id;
        std::vector<VoiceFileData> voices;
        std::vector<SectionFileData> sections;
    };

    class SynthpleFileData{

        std::shared_ptr<spdlog::logger >_logger;
        std::unordered_map<std::string,filedata::SongFileData>  _songs;

        public:
            SynthpleFileData(std::string path_to_data_dir);
            filedata::SongFileData *getSongByName( const std::string &songname );
    };
}