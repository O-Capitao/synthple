# SYNTHPLE
## What it is

A very small api for:

- Creating synth sounds
    - 4 voices
    - 8 - 16 bit depth
    - programable parameters

- Using said synth in other apps, including
    - infinite loops
    - audio cues
    - encapsulation of low level stuff

## Inputs

### run in standalone:
- synth.yaml - parameters for synth
- song.midi

### as a component in an app:
- config.yaml - more configs relative to 
    - event cues
    - track order

## Synth
### Parameters
* wave: `SQUARE`, `SINE`, `TRIANGLE`
* ADSR env


## Midi Files
based on
https://github.com/craigsapp/midifile.git


# About MIDI messages:

e.g.:

```
    TPQ: 960
    TRACKS: 1
    
    TICK    DELTA   TRACK   MIDI MESSAGE
    ____________________________________
    0       0       0       90 0 7f            -> NOTE ON  -- LOW C
    480     480     0       80 0 0             -> NOTE OFF -- LOW C
    480     0       0       90 7f 7f           -> NOTE ON  -- HI C
    960     480     0       80 7f 0            -> NOTE OFF -- HI C
    960     0       0       ff 2f 0  
                                        -> and this one?
```

7f -> hexadecimal for 127 - 128 notes in total in the MIDI spec

Message struct:

80/90 -> note off/ on    |    4 bit for velocity