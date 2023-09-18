#pragma once

#include <vector>
#include <spdlog/spdlog.h>

#include <synthple_globals.hpp>
#include <synthple_bus.hpp>
#include <synthple_audio.hpp>
#include <midi/synthple_midi.hpp>

namespace synthple {

    class Generator
    {
        protected:
            std::shared_ptr<spdlog::logger >_logger;
            float _amp;
            float _freq_hz;
            float _freq_rad_s;
            float _period;

        public:
            Generator( float amp, float freq );
            ~Generator();
            virtual float getValueAtTime( float t ) = 0;
            void setNewAmpAndFreq( float amp, float freq );
    };

    class SineGenerator: public Generator
    {
        public:
            SineGenerator( float amp, float freq );
            ~SineGenerator();
            float getValueAtTime( float t );
    };

    // class SquareGenerator: public Generator
    // {
    //     public:
    //         SquareGenerator( float amp, float freq );
    //         ~SquareGenerator();
    //         float getValueAtTime( float t );
    // };
    


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
        // std::vector<midi::MidiNote> _pressedNotes;
        midi::MidiNote _pressedNote;
        SineGenerator _generator;

        public:
            // Synthple( bus::AudioDataBus *audioDataBus, int bpm );
            Synthple( bus::AudioDataBus *audioDataBus, midi::MidiFileWrapper *file );
            ~Synthple();
            void run();
            void quit();
    };
}