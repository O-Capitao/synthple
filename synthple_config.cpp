#include <synthple_config.hpp>

#include <filesystem>
#include <spdlog/sinks/basic_file_sink.h>

using namespace synthple::filedata;

SynthpleFileData::SynthpleFileData(std::string path_to_data_dir)
:_logger(spdlog::basic_logger_mt("Config_reader", "synthple.log"))
{
    _logger->set_level(spdlog::level::debug);
    _logger->debug("Awake");


    std::string songspath = path_to_data_dir + "/songs/";
    std::vector<std::string> song_files;
    
    

    for (const auto & entry : std::filesystem::directory_iterator(songspath))
        song_files.push_back( entry.path() );
    
    _logger->debug("Found {} Songs in {}", song_files.size(), songspath);

    for (auto &songfilepath : song_files ){

        YAML::Node song = YAML::LoadFile( songfilepath );
        
        SongFileData sfd = {
            .bpm = song["bpm"].as<short>(),
            .voices = std::vector<VoiceFileData>()
        };

        YAML::Node voicesNode = song["voices"];
        _logger->debug("Found {} voices config.", voicesNode.size());

        for (std::size_t i=0 ; i < voicesNode.size(); i++) {
            auto currvoicedata = voicesNode[i];
            sfd.voices.push_back({
                .id = currvoicedata["id"].as<std::string>(),
                .type = currvoicedata["type"].as<std::string>(),
                .n_samples = currvoicedata["n_samples"].as<short>(),
                .offset = currvoicedata["offset"].as<float>(),
                .gain = currvoicedata["gain"].as<float>()
            });
            _logger->debug("Parsed voice: {}.", currvoicedata["id"].as<std::string>());
        }
    }

    _logger->debug("Done reading");
    _logger->flush();
}