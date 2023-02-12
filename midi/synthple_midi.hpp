#pragma once

#include <string>
#include <vector>

#include <midifile/MidiFile.h>

#include <synthple_enums.hpp>

namespace synthple::midi {
    
    enum MidiEventType{
        NOTE_ON,
        NOTE_OFF,
        END_OF_SEQUENCE,
        OTHER
    };

    struct MidiEventNote {
        Note note;
        ushort octave; // 0 to 9
    };

    struct MidiEventWrapper {

        MidiEventWrapper( smf::MidiEvent* mev );

        MidiEventType type;
        MidiEventNote note;
        int ticks;

        private:

            MidiEventType _getMidiEventTypeFromCode( int eventCode );
            MidiEventNote _getMidiEventNoteFromCode( int noteCode );
            Note _intToNote(int noteNumber);
    };

    class MidiFileWrapper {

        std::string _file_path;
        int _ticks_per_quarter_note;
        std::vector<MidiEventWrapper> _midi_events;

        public:
            // When creating the MFWrapper we want to parse and
            // store the entire track in a single instance 
            // and get rid of the file itself 
            MidiFileWrapper( const std::string &filePath );
            ~MidiFileWrapper();

            void printMidiEvents();
            // void getEventAt(int ms);
    
    };
}