#include <synthple.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <iostream>

int main(int argc, char *argv[]){

    if (argc < 2){
        // error -> need a bpm and midi file to play
        std::cerr << "Too little args, please supply a config dir path\n";
        return 1;
    }

    std::string configfilepath( argv[1] );
    synthple::Synthple synthple( configfilepath );

    synthple.init();
    synthple.setSong("song1");

    std::cout << "\nStarting Synthple.\n";

    while (1){
        sleep(10);
    }

    // // while(true){

    // // }

    std::cout << "Stopping Synthple.\n";
    synthple.stop();
    synthple.close();
}