#pragma once

#include <vector>
#include <spdlog/spdlog.h>

#include <synthple_globals.hpp>
#include <synthple_bus.hpp>
#include <synthple_audio.hpp>
#include <midi/synthple_midi.hpp>
#include <synthple_waves.hpp>

namespace synthple {
       
    class Synthple{
        
        bool _MAIN_QUIT = false;
        
        std::shared_ptr<spdlog::logger >_logger;
        audio::AudioThread _audioThread;
        bus::AudioDataBus *_audioDataBus_ptr;
        midi::MidiFileWrapper *_midi_file_ptr;
        int _bpm;
        bool _isNotePressed;
        float _totalTime_s;
        
        NoteFrequency _noteFrequency;
        midi::MidiNote _pressedNote;
        waves::WaveGenerator _generator;

        public:
            // Synthple( bus::AudioDataBus *audioDataBus, int bpm );
            Synthple( bus::AudioDataBus *audioDataBus, midi::MidiFileWrapper *file );
            ~Synthple();
            void run();
            void quit();
    };
}