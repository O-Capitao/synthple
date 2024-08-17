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

### NEXT: ondas quadradas, triangulares, sawtooth, seno.


### NEXT: Portamento / Glissando / *Slide*

- detectar notas sobrepostas
- novo valor de config `slide`

```yaml
    slide_time: 1 #(valor em segundos): tempo para que, uma vez detectada a sobreposição de eventos NOTE_ON, seja feito o slide completo.
    slide_type: FULL # também pode ser NOTE, OFF. default = OFF.
```

- `slide_type=OFF` -> versão sem slide deve ser capaz de lidar com sobreposiçao de notas. e.g.: Mini Moog dá prioridade à nota baixa.




