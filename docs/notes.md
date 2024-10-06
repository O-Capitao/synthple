## CP #1
> toca um sinal sinosoidal de 440Hz durante 1 segundo.


# CP #2
> toca o somke on the water hardcoded, em ondas sinusoidais, com bpm variável, passado como argumento:
-bpm [BPM_VAL]


# CP #3
- ler config com x vozes monofónicas e tocar a partir de ficheiro midi
- ondas quadradas só.
- tempo hardcoded
- start() stop() hardcoded em main paraa simular um cliente ext.

### CP#4: ondas quadradas, triangulares, sawtooth, seno.







# 2024 - 08 - 24: sequencing

```yaml
id: intro
bpm: 120
beats-per-bar: 4
# (...)
sections:
  - id: intro
    # TODO: enable repeat
    repeat: 1 # 0 = infinite loop
    length-bars: 2
    tracks:
        # midi assignments
  - id: afterintro
    repeat: 0 # 0 = infinite loop
    length-bars: 2
    tracks:
        # midi asssignments
```

# Class SynthpleClock

```cpp
struct SynthpleCounters {

    int beat__counter;
    int bar__counter;

    int section__counter;


}
```


## O problema neste momento:
Porque é que o sequenciamento é gerido a meias entre `Synthple` e `Mixer`?


Duas maneiras de transitar entre Secção

1) Naturalmente por via da config e sequencia
> controlado por Mixer.

2) Por fora, chamado de algum cliente. (Forçar)
> controlado por Synthple.






## FUTURE:
### NEXT: Drum Tracks / One Shot FX
### NEXT: Portamento / Glissando / *Slide*

- detectar notas sobrepostas
- novo valor de config `slide`

```yaml
    slide_time: 1 #(valor em segundos): tempo para que, uma vez detectada a sobreposição de eventos NOTE_ON, seja feito o slide completo.
    slide_type: FULL # também pode ser NOTE, OFF. default = OFF.
```

- `slide_type=OFF` -> versão sem slide deve ser capaz de lidar com sobreposiçao de notas. e.g.: Mini Moog dá prioridade à nota baixa.



