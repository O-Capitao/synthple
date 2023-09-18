
#include <synthple_globals.hpp>
#include <midi/synthple_midi.hpp>
#include <synthple_bus.hpp>
#include <synthple.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <iostream>

int main(int argc, char *argv[]){

    if (argc < 2){
        // error -> need a bpm and midi file to play
        std::cerr << "Too little args, please supply a midi file path\n";
        return 1;
    }

    // get bpm
    // int bpm = atoi(argv[1]);
    std::string midifilepath( argv[2] );

    std::cout << "Playing " << midifilepath.c_str() << std::endl;

    auto _logger(spdlog::basic_logger_mt("MAIN", "synthple.log"));

    _logger->info(LOG_DASH_SEPARATOR);
    _logger->info("Starting Run\n");
    _logger->flush();
    
    synthple::bus::AudioDataBus _audioDataBus;    
    synthple::midi::MidiFileWrapper midiFile( midifilepath );

    synthple::Synthple synthple( &_audioDataBus, &midiFile );
    synthple.run();
}