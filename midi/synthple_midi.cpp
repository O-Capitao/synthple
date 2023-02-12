#include <midi/synthple_midi.hpp>

using namespace synthple::midi;

/*************************************************************************
 *  MIDI EVENT WRAPPER Class
*************************************************************************/
MidiEventWrapper::MidiEventWrapper( smf::MidiEvent* mev ){
    
    ticks = mev->tick;
    type = _getMidiEventTypeFromCode( (int)(*mev)[0] );

    // continue parsing only for "allowed" events
    if (type == MidiEventType::NOTE_OFF || type == MidiEventType::NOTE_ON ){
        note = _getMidiEventNoteFromCode( (int)(*mev)[1] );
    }
}

MidiEventType MidiEventWrapper::_getMidiEventTypeFromCode( int eventCode ){
    switch (eventCode) {
        case 128:
            return MidiEventType::NOTE_ON;
        case 144:
            return MidiEventType::NOTE_OFF;
        case 255:
            return MidiEventType::END_OF_SEQUENCE;
        default:
            return MidiEventType::OTHER;
    }
}

MidiEventNote MidiEventWrapper::_getMidiEventNoteFromCode( int noteCode ){
    return {
        .note = _intToNote( noteCode % Globals::DIATONIC_SCALE_SIZE),
        .octave = (ushort)(noteCode / Globals::DIATONIC_SCALE_SIZE)
    };
}

synthple::Note MidiEventWrapper::_intToNote(int noteNumber){
    switch (noteNumber) {
        case 0:
            return Note::C;
        case 1:
            return Note::Cs;
        case 2:
            return Note::D;
        case 3:
            return Note::Ds;
        case 4:
            return Note::E;
        case 5:
            return Note::F;
        case 6:
            return Note::Fs;
        case 7:
            return Note::G;
        case 8:
            return Note::Gs;
        case 9:
            return Note::A;
        case 10:
            return Note::As;
        case 11:
            return Note::B;
        default:
            throw std::runtime_error("Invalid Note detected, aborting.\n"); 
    }
}



/*************************************************************************
 *  MIDI FILE WRAPPER Class
*************************************************************************/
MidiFileWrapper::MidiFileWrapper(
    const std::string &fp 
){
    smf::MidiFile midifile( fp );
    midifile.joinTracks();
    
    this->_ticks_per_quarter_note = midifile.getTicksPerQuarterNote();

    smf::MidiEvent* mev;

    for (int event=0; event < midifile[0].size(); event++) {

        mev = &midifile[0][event];

        _midi_events.push_back({
            MidiEventWrapper(mev)
        });
    }
}

MidiFileWrapper::~MidiFileWrapper(){}
