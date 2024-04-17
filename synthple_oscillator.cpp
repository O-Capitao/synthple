#include <synthple_oscillator.hpp>

#include <math.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace synthple::oscillator;



// Wavetable
WaveTable::WaveTable( config::WaveTableConfig cnf )
:_logger(spdlog::basic_logger_mt("WAVETABLE", "synthple.log")),
_config( cnf ),
_data( cnf.n_samples )
{

    switch( _config.type ){
        case config::WaveTableType::SINE:
            _generateSinWaveform();
            break;
        case config::WaveTableType::SQUARE:
            _generateSquareWaveform();
            break;
        default:
            throw std::runtime_error("invalid WaveTableType.");
    }
    _logger->set_level(spdlog::level::debug);
    _logger->debug("constructed.");
}

float WaveTable::lookupValueAt(short i){
    assert(i < _config.n_samples);
    return _data[i];
}

void WaveTable::_generateSinWaveform(){
    float angle_step = (2 * M_PI) / (float)_config.n_samples;

    for (int i = 0; i < _config.n_samples; i++){
        _data[i] = sin( i * angle_step );
    }
}

void WaveTable::_generateSquareWaveform(){

    for (int i = 0; i < _config.n_samples; i++){
        _data[i] = i <= _config.n_samples / 2 ? 0 : 1;
    }

    // give it some countour
    for (int i = 3; i < _config.n_samples; i++){    
        _data[i] = (_data[i] + _data[i-1] + sqrtf(_data[i-2]))/3;
    }
}


// Oscillator
Oscillator::Oscillator( config::OscillatorConfig cnf )
:_logger(spdlog::basic_logger_mt("OSCILLATOR", "synthple.log")),
_config(cnf),
_waveTable(cnf.waveTableConfig)
{}

float Oscillator::getValueAt(float t_ms){

    float retval = 0;

    if (!_outputSilence){

        float _t_in_cycle = fmod( (t_ms), _outputPeriod );
        short _index = (short)floor(((float)_config.waveTableConfig.n_samples * _t_in_cycle) / (_outputPeriod));

        assert(_index < _config.waveTableConfig.n_samples);

        if (_isSilenceRequested && (_lastIndex > _index)){
            _isSilenceRequested = false;
            _outputSilence = true;
            
            retval = 0;
        } else {
            
            retval = _outputAmp * _waveTable.lookupValueAt( _index );
        }

        _lastIndex = _index;
    }

    return retval;
}

void Oscillator::requestSilence(){
    _isSilenceRequested = true;
}

void Oscillator::setFrequency(float newf){
    _outputFreq = newf;
    _outputPeriod = 1 / newf;
    _outputSilence = false;
    _isSilenceRequested = false;
}

void Oscillator::setAmp(float newamp){
    _outputAmp = newamp;
}