#include <spdlog/sinks/basic_file_sink.h>
#include <midi/synthple_midi.hpp>

#include <algorithm>

using namespace synthple::midi;

// MidiNote ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
std::string MidiNote::toString(){
    std::string retval = "";

    switch (note)
    {
        case NoteKey::C:
            retval += "C";
            break;
        case NoteKey::Cs:
            retval += "Cs";
            break;
        case NoteKey::D:
            retval += "D";
            break;
        case NoteKey::Ds:
            retval += "Ds";
            break;
        case NoteKey::E:
            retval += "E";
            break;
        case NoteKey::F:
            retval += "F";
            break;
        case NoteKey::Fs:
            retval += "Fs";
            break;
        case NoteKey::G:
            retval += "G";
            break;
        case NoteKey::Gs:
            retval += "Gs";
            break;
        case NoteKey::A:
            retval += "A";
            break;
        case NoteKey::As:
            retval += "As";
            break;
        case NoteKey::B:
            retval += "B";
            break;
        case NoteKey::NOT_A_NOTE:
            retval += "NOT_A_NOTE";
            break;
        default:
            throw std::runtime_error("Invalid Note detected.");
    }

    retval += std::to_string(octave);

    return retval;
}


// MidiEventWrapper////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
MidiEventWrapper::MidiEventWrapper( smf::MidiEvent* mev ){
    
    ticks = mev->tick;
    type = _getMidiEventTypeFromCode( (int)(*mev)[0] );

    // continue parsing only for "allowed" events
    if (type == MidiEventType::NOTE_OFF || type == MidiEventType::NOTE_ON ){
        note = _getMidiEventNoteFromCode( (int)(*mev)[1] );
    }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
MidiEventWrapper::MidiEventWrapper( MidiEventType _t, MidiNote _n, int _ticks )
:type( _t ), note( _n ), ticks( _ticks )
{}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
MidiNote MidiEventWrapper::_getMidiEventNoteFromCode( int noteCode ){
    // return {
    //     .note = _intToNote( noteCode % DIATONIC_SCALE_SIZE),
    //     .octave = (ushort)(noteCode / DIATONIC_SCALE_SIZE)
    // };
    return MidiNote( _intToNote( noteCode % DIATONIC_SCALE_SIZE),  (ushort)(noteCode / DIATONIC_SCALE_SIZE) );
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
synthple::NoteKey MidiEventWrapper::_intToNote(int noteNumber){
    switch (noteNumber) {
        case 0:
            return NoteKey::C;
        case 1:
            return NoteKey::Cs;
        case 2:
            return NoteKey::D;
        case 3:
            return NoteKey::Ds;
        case 4:
            return NoteKey::E;
        case 5:
            return NoteKey::F;
        case 6:
            return NoteKey::Fs;
        case 7:
            return NoteKey::G;
        case 8:
            return NoteKey::Gs;
        case 9:
            return NoteKey::A;
        case 10:
            return NoteKey::As;
        case 11:
            return NoteKey::B;
        default:
            throw std::runtime_error("Invalid Note detected, aborting.\n"); 
    }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
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


// MonophonicMidiFileReader ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
MonophonicMidiFileReader::MonophonicMidiFileReader(
    int readerid,
    std::string fp,
    float tempobpm,
    float dts,
    short lbars,
    short beatsperbar
):
// _id(readerid),
_file_path(fp),
_logger(spdlog::basic_logger_mt("MidiReader " + std::to_string(readerid), "synthple.log")),
_length_bars(lbars),
_beats_per_bar(beatsperbar),
_tempo_bpm(tempobpm)
{   
    _logger->set_level(spdlog::level::debug);
    _logger->info("path = \"{}\", ENTER constructor", fp);

    smf::MidiFile midifile( fp );
    midifile.joinTracks();

    assert(midifile.getEventCount(0) > 0);

    _beat_duration_s = 60.0 / _tempo_bpm;
    _ticks_per_beat = midifile.getTicksPerQuarterNote();
    _tick_duration_s = _beat_duration_s / _ticks_per_beat;
    
    _populateMidiEvents(midifile);
    _curr_evt = _midi_events.begin();
    _next_evt_aux = std::next(_curr_evt, 1);
    
    // _end_of_sequence__tick = _midi_events.back().ticks;
    _last_time_aux_s = 0.0;
    _duration_s = _tick_duration_s * _ticks_per_beat * (float)_beats_per_bar * (float)_length_bars;
    _total_ticks = _ticks_per_beat * _beats_per_bar * _length_bars;

    // create a "fake" midinote
    // to use to return silence.
    _silence.note = NoteKey::NOT_A_NOTE;
    _silence.note_value = "NOT_A_NOTE";
    _silence.octave = 0;

    _logger->debug(
        "Midi File time params:\n" 
        "TOTAL DURATION OF PLAY:{}s - {} ticks\n"
        "SINGLE TICK DURATION: \'{}\'s\n"
        "END_OF_SEQUENCE TICK: {}\n", 
        _duration_s, _total_ticks, _tick_duration_s, _end_of_sequence__tick );

    _logger->flush();
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void MonophonicMidiFileReader::_populateMidiEvents(smf::MidiFile &file){
    
    _logger->debug("populating. Getting {} events.", file[0].size());
    smf::MidiEvent* mev;

    for (int event=0; event < file[0].size(); event++) {

        mev = &file[0][event];

        _midi_events.push_back({
            MidiEventWrapper(mev)
        });

        // fill out _end_of_sequence__tick
        if (_midi_events.back().type == MidiEventType::END_OF_SEQUENCE){
            _end_of_sequence__tick = _midi_events.back().ticks;
        }
    }

    std::string __events_string = "file events:\n";
    for (int i=0; i < _midi_events.size(); i++) {

        __events_string += 
            "Note: \"" 
            + _midi_events[i].note.note_value 
            + " *" + (_midi_events[i].type == MidiEventType::NOTE_ON ? "ON" : "OFF" ) + "* "
            + "\" at \"" 
            + std::to_string( _midi_events[i].ticks ) + " ticks\"\n";
    }
    _logger->debug(__events_string);
}


MidiEventWrapper *MonophonicMidiFileReader::getActiveMidiEventAt_Time_s(float t_s) {

    // when loop loops:
    if ( t_s < _last_time_aux_s){
        _curr_evt = _midi_events.begin();

        // what if there is no next event?
        _next_evt_aux = std::next(_curr_evt, 1);

        _logger->debug("when loop loops");
    }
    
    int _curr_tick = _t_secs__toTicks(t_s);
    
    if (_curr_tick < _total_ticks ){

        _last_time_aux_s = t_s;

        if (_curr_tick > _end_of_sequence__tick){
            _logger->debug("reached end of tick. returning nullptr");
            return nullptr;
        }
        // TODO : give uuid to MidiEvent at birth to compare every event...
        if ( _next_evt_aux->ticks <= _curr_tick ){

            _curr_evt = std::next(_curr_evt, 1);
            _next_evt_aux = std::next(_curr_evt, 1);

            std::string _logginginAux = ((_curr_evt->type == MidiEventType::NOTE_ON) ? "NOTE_ON" : "NOTE_OFF");
            
            _logger->debug("Transitioning into new event at ticks={}\n"
                 "{}\n"
                "NOTE={}",
                _curr_tick, _logginginAux, _curr_evt->note.note_value );
        }

        if (_curr_evt->type == MidiEventType::NOTE_ON){
            return &(*_curr_evt);
        }
    }

    return nullptr;
}
///////////////////////////////////////////////////////////////////////
// This one should be pretty simple, 
// for a given time value in seconds we return
// whatever note is active (following NOTE_ON event)
///////////////////////////////////////////////////////////////////////
MidiNote *MonophonicMidiFileReader::getStateAt_Time_s(float t_s) {
    
    int _curr_tick = _t_secs__toTicks(t_s);
    MidiEventWrapper *_curr_event, *_next_event;

    for (int i = 0; i < _midi_events.size()-1; i++){

        _curr_event = &_midi_events[i];
        _next_event = &_midi_events[i+1];

        if (_curr_tick >= _curr_event->ticks && _curr_tick < _next_event->ticks &&  _curr_event->type == MidiEventType::NOTE_ON)
            return &(_curr_event->note);

    }

    return &_silence;
}

int MonophonicMidiFileReader::_t_secs__toTicks( float tsecs){
    float _t_in_cycle = fmod( tsecs, _duration_s );
    assert(_tick_duration_s > 0 && _t_in_cycle < _duration_s );
    
    int ticks = (int)floor( _t_in_cycle / _tick_duration_s );
    
    return ticks;
}
