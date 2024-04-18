#include <synthple.hpp>
#include <synthple_audio.hpp>

#include <math.h>
#include <time.h> 

#include <spdlog/sinks/basic_file_sink.h>

#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp> 

using namespace synthple;

Synthple::Synthple( std::string path_to_config )
:_logger(spdlog::basic_logger_mt("SYNTHPLE", "synthple.log")),
_audioDataBus(),
_audioThread( &_audioDataBus ),
_filedata( path_to_config )
{
    _logger->set_level(spdlog::level::debug);
    _logger->debug("Constructed Synthple obj");
    _logger->flush();
}

// Synthple::~Synthple()
// {}

// void Synthple::run()
// {   
    
//     // debug stuff
//     clock_t __ticks_start = clock();
//     int __writes_to_buffer = 0;
//     int __predicted_sample_count = _totalTime_s * FRAMERATE;
//     int __buffer_writes = __predicted_sample_count / FRAMES_IN_BUFFER;
//     int __space_in_queue = 0;

//     _logger->info(
//         "Starting run. Predicted sample count to be handled: {}", 
//         __predicted_sample_count);

//     _logger->debug(
//         "Size of buffer: {}. - Writes to Buffer (Sample Count / Buffer Size) = {}", 
//         FRAMES_IN_BUFFER, __buffer_writes
//     );

//     // should be the same as the target sample rate
//     float __logical_dt_s = 1 / (float)FRAMERATE;
//     float __logical_current_time_s = 0;
//     float __cycle_advance_dt_s = (float)FRAMES_IN_BUFFER / (float)FRAMERATE;

//     _logger->debug("Starting Run - Cycle Time = {}", __cycle_advance_dt_s);
//     _logger->flush();
    
//     _midi_file_ptr->initSequentialRead( __logical_dt_s );

//     std::vector<midi::MidiNote> &__requested_notes = _midi_file_ptr->getActiveNotesVec();
//     midi::MidiNote __next_note;
//     midi::MidiNote __current_note;
//     NoteFrequency __note_frequency;

//     _audioThread.start();

//     while ( !_MAIN_QUIT )
//     {
//         __space_in_queue = _audioDataBus_ptr->queue.write_available();

//         if ( __space_in_queue > FRAMES_IN_BUFFER / 4 ) {
        
//             if ( _QUIT_REQUESTED )
//             {
//                 _logger->debug("main loop: _QUIT_REQUESTED");
//                 _MAIN_QUIT = true;
//                 break;
//             } else 
//             {
//                 _logger->debug("MAIN LOOP: pushing {} samples into queue. t={} s", __space_in_queue, __logical_current_time_s);
//                 // advance time as we push values into the queue
//                 for (int i = 0; i < __space_in_queue; i++ )
//                 {
                    
//                     // PROCESS MIDI
//                     __requested_notes = _midi_file_ptr->getActiveNotesVec();
//                     __next_note = __requested_notes[0];

//                     if (__next_note.note == NoteKey::NOT_A_NOTE )
//                     {
//                         _oscillator.requestSilence();

//                     } else {
                    
//                         if (__current_note.note_value != __next_note.note_value){
                            
//                             // TODO
//                             // _generator.requestFreqChange( 
//                             //     __note_frequency.noteFreqMap[__next_note.note_value]
//                             // );
//                             _oscillator.setFrequency(__note_frequency.noteFreqMap[__next_note.note_value]);
//                         }
//                     }

//                     __current_note = __next_note;
                    
//                     _audioDataBus_ptr->queue.push(  _oscillator.getValueAt( __logical_current_time_s ));                    
//                     __logical_current_time_s += __logical_dt_s;

//                     if (__logical_current_time_s >= _totalTime_s)
//                     {
//                         _QUIT_REQUESTED = true;
//                     }
//                     // TODO
//                     // _generator.step( __logical_dt_s );
//                     _midi_file_ptr->step();
//                 }
//             }
//             __writes_to_buffer++;   
//         }


//         // boost::this_thread::sleep_for(boost::chrono::milliseconds((int)floor(__cycle_advance_dt_s * 1000)));
//     }

//     _audioThread.stop();

//     // Time at end of run
//     clock_t _length_ticks = clock() - __ticks_start;
//     float _length_secs = (float)_length_ticks/ (float)CLOCKS_PER_SEC;

//     _logger->info("run() Finished - took {} s and wrote {} times to the buffer" , 
//         _length_secs, 
//         __writes_to_buffer);
//     _logger->flush();
    
// }

// void Synthple::quit()
// {
//     _MAIN_QUIT = true;
// }

