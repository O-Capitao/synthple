#include <synthple.hpp>
#include <synthple_audio.hpp>

#include <math.h>
#include <time.h> 

#include <spdlog/sinks/basic_file_sink.h>

#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp> 

using namespace synthple;

Synthple::Synthple( bus::AudioDataBus *audioDataBus, midi::MidiFileWrapper *midifile )
:
_logger(spdlog::basic_logger_mt("SYNTHPLE", "synthple.log")),
_audioThread( audio::AudioThread( audioDataBus ) ),
_audioDataBus_ptr( audioDataBus ),
_midi_file_ptr( midifile ),
_bpm(midifile->getTempoBpm()),
_totalTime_s( midifile->getDuration() ),
_generator(0.25)
{
    _logger->set_level(spdlog::level::debug);
    _logger->debug("Constructed Synthple obj");
}

Synthple::~Synthple()
{}

void Synthple::run()
{   
    
    // debug stuff
    clock_t __ticks_start = clock();
    int __writes_to_buffer = 0;
    int __predicted_sample_count = _totalTime_s * FRAMERATE;
    int __buffer_writes = __predicted_sample_count / FRAMES_IN_BUFFER;

    _logger->info(
        "Starting run. Predicted sample count to be handled: {}", 
        __predicted_sample_count);

    _logger->debug(
        "Size of buffer: {}. - Writes to Buffer (Sample Count / Buffer Size) = {}", 
        FRAMES_IN_BUFFER, __buffer_writes
    );

    // should be the same as the target sample rate
    float __logical_dt_s = 1 / (float)FRAMERATE;
    float __logical_current_time_s = 0;
    float __cycle_advance_dt_s = 00.5 * (float)FRAMES_IN_BUFFER / (float)FRAMERATE;

    _logger->debug("Starting Run - Cycle Time = {}", __cycle_advance_dt_s);
    
    _midi_file_ptr->initSequentialRead( __logical_dt_s );

    std::vector<midi::MidiNote> &__requested_notes = _midi_file_ptr->getActiveNotesVec();
    midi::MidiNote __next_note;
    midi::MidiNote __current_note;
    NoteFrequency __note_frequency;

    // // end in silence
    // int __end_counter = 0;
    // int __end_counterlimit = 100;

    _audioThread.start();

    while ( !_MAIN_QUIT )
    {
        if ( _QUIT_REQUESTED )
        {
            _MAIN_QUIT = true;
            break;
        }
        
        int __space_in_queue = _audioDataBus_ptr->queue.write_available();

        if ( __space_in_queue > 0 && !_QUIT_REQUESTED )
        {
            if ( _QUIT_REQUESTED )
            {
                for (int i = 0; i < __space_in_queue; i++ )
                {
                    _audioDataBus_ptr->queue.push( 0.0 );
                    // __end_counter ++;
                }

            } else 
            {
                // advance time as we push values into the queue
                for (int i = 0; i < __space_in_queue; i++ )
                {
                    // PROCESS MIDI
                    __requested_notes = _midi_file_ptr->getActiveNotesVec();
                    __next_note = __requested_notes[0];

                    if (__next_note.note == NoteKey::NOT_A_NOTE )
                    {
                        _generator.requestSilence();

                    } else {
                    
                        if (__current_note.note_value != __next_note.note_value){

                            _generator.requestFreqChange( 
                                __note_frequency.noteFreqMap[__next_note.note_value]
                            );
                        }
                    }

                    __current_note = __next_note;
                    
                    _audioDataBus_ptr->queue.push(  _generator.getValue() );
                    __logical_current_time_s += __logical_dt_s;

                    if (__logical_current_time_s >= _totalTime_s)
                    {
                        _QUIT_REQUESTED = true;
                    }

                    _generator.step( __logical_dt_s );
                    _midi_file_ptr->step();
                }
            }
            __writes_to_buffer++;   
        }
        boost::this_thread::sleep_for(boost::chrono::milliseconds((int)floor(__cycle_advance_dt_s * 1000)));
    }

    _audioThread.stop();

    // Time at end of run
    clock_t _length_ticks = clock() - __ticks_start;
    float _length_secs = (float)_length_ticks/ (float)CLOCKS_PER_SEC;

    _logger->info("run() Finished - took {} s and wrote {} times to the buffer" , 
        _length_secs, 
        __writes_to_buffer);
    
}

void Synthple::quit()
{
    _MAIN_QUIT = true;
}