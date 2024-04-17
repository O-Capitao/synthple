# Song/ Loop management

# first iteration:
> single config file at data/songs/{songname}.yml

```yaml
id: intro
bpm: 120
voices:
  - id: &square1 square_1
    type: SQUARE # one of SQUARE, TRIANGLE, SAW or CUSTOM (TBD)
    offset: 0  #centered
    n_samples: 64
    gain: 0.7
  - id: &square2 square_2
    type: SQUARE
    offset: -0.3
    n_samples: 64
    gain: 0.3
parts:
  - id: intro
    repeat: 0 # 0 = infinite loop
    tracks:
      - voice: *square1
        src: midi/gtr_intro.mid
      - voice: *square2
        src: midi/gtr_intro_2.mid
```

arranjed into something like:

```

Synthple
    |
    |_ audioDataBus
    |_ audioThread
    |_ FileData (from yaml)
    |_ Songs
        []
        |_Song#1
            |_ id
            |_ bpm
            |_ voices -> oscillators WHO OWNS THE OSCILLATORS?
            |    []
            |    |_ id
            |    |_ type
            |    |_ offset
            |    |_ n_samples
            |    |_ gain
            |
            |_ parts
                []
                |_ id
                |_ repeat
                |_ tracks
                    []
                    |_ voice
                    |_ src

```

Synthple-> play song

@ init
1. Synthple.constructor loads data files into FileData obj and builds Songs vector/map

@ play song

- build oscillators
- init time_counter



by default start by playing **Part #1**

- load midi files into each used voice.
