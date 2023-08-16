
#include <synthple_globals.hpp>
#include <midi/synthple_midi.hpp>
#include <synthple_bus.hpp>
#include <synthple.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// #include <iostream>

int main(int argc, char *argv[]){
    
    auto _logger(spdlog::basic_logger_mt("MAIN", "synthple.log"));
    _logger->info(LOG_DASH_SEPARATOR);
    _logger->info("Starting Run\n");
    _logger->flush();

    synthple::midi::MidiFileWrapper mfw( "/Users/capitaof/THE_CODES/git/synthple/lowC_to_hiC.mid" ); 
    
    _logger->debug("Constructed mfw");
    _logger->flush();
     
    synthple::bus::AudioData_Queue iat_q;
    synthple::bus::Commands_Queue fat_q;
    _logger->debug("got queues");
    _logger->flush();
    
    synthple::Synthple synthple( mfw, &iat_q, &fat_q );
    synthple.run();


    // synthple.run();
    _logger->info(LOG_DASH_SEPARATOR);
    _logger->info("Exiting Run\n\n\n");
    _logger->flush();

    //     smf::MidiFile midifile( file );

    // std::cout << "TPQ: " << mfw.getTicksPerQuarterNote() << std::endl;
    // std::cout << "TRACKS: " << midifile.getTrackCount() << std::endl;
    //    midifile.joinTracks();
    //    // midifile.getTrackCount() will now return "1", but original
    //    // track assignments can be seen in .track field of MidiEvent.
    //    std::cout << "TICK    DELTA   TRACK   MIDI MESSAGE\n";
    //    std::cout << "____________________________________\n";
    
    //    smf::MidiEvent* mev;
    //    int deltatick;
    //    for (int event=0; event < midifile[0].size(); event++) {
    //       mev = &midifile[0][event];
    //       if (event == 0) deltatick = mev->tick;
    //       else deltatick = mev->tick - midifile[0][event-1].tick;
    //       std::cout << std::dec << mev->tick;
    //       std::cout << '\t' << deltatick;
    //       std::cout << '\t' << mev->track;
    //       std::cout << '\t' << std::hex;
    //       for (int i=0; i < mev->size(); i++)
    //          std::cout << (int)(*mev)[i] << ' ';
    //       std::cout << std::endl;
    //    }
    //    return 0;

    //     printf("done\n");
}