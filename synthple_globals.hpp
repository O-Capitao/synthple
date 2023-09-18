#pragma once

#define FRAMES_IN_BUFFER 512                        // Frame - data necessary to render a timestep
#define N_CHANNELS 1                                // Channels
#define FRAMERATE 22000                             // value for the mega drive
#define SAMPLE_DEPTH 16
#define BUFFER_SIZE (FRAMES_IN_BUFFER * N_CHANNELS) // total of values inside the queue
#define AUDIO_QUEUE_SIZE (BUFFER_SIZE * 4)          // number of frames in the buffer
#define DIATONIC_SCALE_SIZE 12
#define TIME_FOR_BUFFER (FRAMES_IN_BUFFER / FRAMERATE)

#define AUDIO_TH_WAIT_TIME 33

#define LOG_DASH_SEPARATOR "------------------------------------------"

#include <portaudio.h>
#include <map>
#include <string>

namespace synthple {

    // 0 through 255 decimal
    // using MY_SAMPLE_DEF = float;
    
    enum NoteKey { C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B };

    enum Signal {
        PLAYBACK_FINISHED
    };

    // print out defined numbers
    std::string globalsToString();

    struct NoteFrequency {
        std::map<std::string, float> noteFreqMap;
        NoteFrequency();
    };
}