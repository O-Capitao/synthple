#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#include <synthple_audio.hpp>
#include <synthple_globals.hpp>

using namespace synthple::audio;

AudioThread::AudioThread(
    bus::AudioDataBus *in_bus
)
:_logger(spdlog::basic_logger_mt("AUDIO THREAD", "synthple.log"))
{
    _data = new InternalAudioData();
    _data->inputBus = in_bus;
    
    _logger->set_level(spdlog::level::info);
    _logger->info("Constructed.");
}

AudioThread::~AudioThread(){
    delete _data;
}

void AudioThread::start()
{
    _logger->debug("start()");

    if ( Pa_Initialize() != paNoError){
        _logger->error("run() : Error initing the AudioEngine");
        throw std::runtime_error("Error initing the AudioEngine");
    }

    if ( _data == NULL ){
        Pa_Terminate();
        _logger->error("run() : No data to play, shutting down.");
        throw std::runtime_error("No data to play, shutting down.");
    }

    _openStream();
    _startStream();

}

void AudioThread::stop()
{
    _logger->debug("stop()");
    _stopStream();
    _closeStream();
}

/*
    PortAudio provides samples in raw PCM format. 
    That means each sample is an amplitude to be given to the DAC in your sound card. 
    For paInt16, this is a value from -32768 to 32767. 
    For paFloat32, this is a floating-point value from -1.0 to 1.0. 
    The sound card converts this values to a proportional voltage that then drives your audio equipment.
*/
void AudioThread::_openStream()
{
    _logger->debug("AudioEngine::_openStream()");

    PaError e = Pa_OpenDefaultStream(
        &_stream,
        0,
        N_CHANNELS,
        paFloat32,
        FRAMERATE,
        BUFFER_SIZE,
        paStreamCallback,
        _data
    );

    if (e != paNoError){
        std::string msg = Pa_GetErrorText( e );
        _logger->error( "Error opening stream. msg; {}", msg );
        throw std::runtime_error("Could not Open stream.");
    }
}

void AudioThread::_startStream()
{
    _logger->debug("AudioEngine::_startStream()");
    PaError e = Pa_StartStream(_stream);

    if (e != paNoError){
        std::string msg = Pa_GetErrorText( e );
        _logger->error( "Error starting stream. msg; {}", msg );
        throw std::runtime_error("Could not Start stream.");
    }
}

void AudioThread::_stopStream()
{
    _logger->debug("AudioEngine::_stopStream()");
    PaError e = Pa_StopStream(_stream);

    if (e != paNoError){
        std::string msg = Pa_GetErrorText( e );
        _logger->error( "Error stopping stream. msg; {}", msg );
        throw std::runtime_error("Could not Stop stream.");
    }
}

void AudioThread::_closeStream()
{
    _logger->debug("AudioEngine::_closeStream()");
    PaError e = Pa_CloseStream(_stream);

    _stream = NULL; 

    if (e != paNoError){
        std::string msg = Pa_GetErrorText( e );
        _logger->error( "Error closing stream. msg; {}", msg );
        throw std::runtime_error("Could not close stream.");
    }

}


int AudioThread::paStreamCallback(
    const void *inputBuffer,
    void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData
){
    float *out;
    out = (float*)outputBuffer;
    InternalAudioData *p_data = (InternalAudioData*)userData;
    
    int fill_buffer_counter = 0;
    float value;
    
    while ( 
        p_data->inputBus->queue.pop(value) 
        && (fill_buffer_counter < N_CHANNELS * FRAMES_IN_BUFFER)
    ){
        out[fill_buffer_counter++] = value;
        // fill_buffer_counter++;
    }

    return 0;
}

