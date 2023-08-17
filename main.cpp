
#include <synthple_globals.hpp>
#include <midi/synthple_midi.hpp>
#include <synthple_bus.hpp>
#include <synthple.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

int main(int argc, char *argv[]){
    
    auto _logger(spdlog::basic_logger_mt("MAIN", "synthple.log"));

    _logger->info(LOG_DASH_SEPARATOR);
    _logger->info("Starting Run\n");
    _logger->flush();
    
    synthple::bus::AudioDataBus _audioDataBus;
    synthple::Synthple synthple( &_audioDataBus);
    synthple.run();

}