#include <spdlog/sinks/basic_file_sink.h>
#include <midi/synthple_midi.hpp>

using namespace synthple::midi;

/*************************************************************************
 * MIDI NOTE
*************************************************************************/
std::string MidiNote::toString(){
    std::string retval = "";

    switch (note)
    {
        case Note::C:
            retval += "C";
            break;
        case Note::Cs:
            retval += "C#";
            break;
        case Note::D:
            retval += "D";
            break;
        case Note::Ds:
            retval += "D#";
            break;
        case Note::E:
            retval += "E";
            break;
        case Note::F:
            retval += "F";
            break;
        case Note::Fs:
            retval += "F#";
            break;
        case Note::G:
            retval += "G";
            break;
        case Note::Gs:
            retval += "G#";
            break;
        case Note::A:
            retval += "A";
            break;
        case Note::As:
            retval += "A#";
            break;
        case Note::B:
            retval += "B";
            break;
        default:
            throw std::runtime_error("Invalid Note detected.");
    }

    retval += std::to_string(octave);

    return retval;
}

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
            return MidiEventType::NOTE_OFF;
        case 144:
            return MidiEventType::NOTE_ON;
        case 255:
            return MidiEventType::END_OF_SEQUENCE;
        default:
            return MidiEventType::OTHER;
    }
}

MidiNote MidiEventWrapper::_getMidiEventNoteFromCode( int noteCode ){
    return {
        .note = _intToNote( noteCode % DIATONIC_SCALE_SIZE),
        .octave = (ushort)(noteCode / DIATONIC_SCALE_SIZE)
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

std::string MidiEventWrapper::toString(){
    std::string retval = "\nEvent @:" + std::to_string(ticks) + "\n";

    switch (type){
        case MidiEventType::NOTE_ON:
            retval += "  ON ";
            break;
        case MidiEventType::NOTE_OFF:
            retval += "  OFF ";
            break;
        case MidiEventType::END_OF_SEQUENCE:
            return retval + " END OF SEQUECE";
        default:
            return retval + "  OTHER";
    }

    return retval + note.toString();
}



/*************************************************************************
 *  MIDI FILE WRAPPER Class
*************************************************************************/
MidiFileWrapper::MidiFileWrapper(
    const std::string &fp 
):_logger(spdlog::basic_logger_mt("MIDI", "synthple.log"))
{
    _logger->info("ENTER constructor");

    smf::MidiFile midifile( fp );

    midifile.joinTracks();
    
    _ticks_per_quarter_note = midifile.getTicksPerQuarterNote();
    _tick_duration = midifile.getTimeInSeconds(1);
    
    smf::MidiEvent* mev;

    for (int event=0; event < midifile[0].size(); event++) {

        mev = &midifile[0][event];

        _midi_events.push_back({
            MidiEventWrapper(mev)
        });
    }

    // get track duration from
    // last event's ticks
    long _last_event_ticks = _midi_events.back().ticks;
    _duration_ms = _last_event_ticks * _tick_duration;
    _tempo_bpm = 60 / (_tick_duration * _ticks_per_quarter_note);

    _logger->info(
        "Parsed MIDI file:\n  total duration = {}s\n  tiks_per_qnote = {}\n   bpm: {}\n   tick duration: {}", 
        _duration_ms, 
        _ticks_per_quarter_note,
        _tempo_bpm,
        _tick_duration);

    // for (auto e : _midi_events){
    //     _logger->info(e.toString());
    // }

    _logger->info("EXIT constructor");
    _logger->flush();
}

MidiFileWrapper::~MidiFileWrapper(){}

int MidiFileWrapper::_ticksToMilliseconds(int t){

}
