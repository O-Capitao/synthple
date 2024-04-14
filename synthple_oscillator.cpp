#include <synthple_oscillator.hpp>

#include <math.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace synthple::oscillator;



// Wavetable
WaveTable::WaveTable( config::WaveTableConfig cnf )
:_logger(spdlog::basic_logger_mt("WAVETABLE", "synthple.log")),
_config( cnf )
{
    // TODO : init c style array from pointer?
    // float _data[ _config.n_samples ];

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
        
}

WaveTable::~WaveTable(){
    delete _data;
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
        _data[i] = i > _config.n_samples / 2 ? 0 : 1;
    }
}


// Oscillator
Oscillator::Oscillator( config::OscillatorConfig cnf )
:_logger(spdlog::basic_logger_mt("OSCILLATOR", "synthple.log")),
_config(cnf),
_waveTable(cnf.waveTableConfig)
{}

float Oscillator::getValueAt(float t_ms){

    float _t_in_cycle = fmod( (t_ms / 1000.0), _outputPeriod );
    short _index = (short)floor(((float)_config.waveTableConfig.n_samples * _t_in_cycle) / (_outputPeriod));

    assert(_index < _config.waveTableConfig.n_samples);

    return _waveTable.lookupValueAt( _index );
}