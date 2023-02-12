#include<midi/synthple_midi.hpp>

#include <iostream>

int main(int argc, char *argv[]){
    
    synthple::midi::MidiFileWrapper mfw( "/Users/capitaof/THE_CODES/git/synthple/lowC_to_hiC.mid" );    
    

//     smf::MidiFile midifile( file );

//     std::cout << "TPQ: " << midifile.getTicksPerQuarterNote() << std::endl;
//    std::cout << "TRACKS: " << midifile.getTrackCount() << std::endl;
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