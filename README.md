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

## Message struct:

80/90 -> note off/ on    |    4 bit for velocity


## Notes vs Freq
https://pages.mtu.edu/~suits/NoteFreqCalcs.html

fn = f0 * (a)n 

where

*f0* = the frequency of one fixed note which must be defined. A common choice is setting the A above middle C (A4) at f0 = 440 Hz.
*n* = the number of half steps away from the fixed note you are. If you are at a higher note, n is positive. If you are on a lower note, n is negative.
*fn* = the frequency of the note n half steps away.
*a* = (2)1/12 = the twelth root of 2 = the number which when multiplied by itself 12 times equals 2 = 1.059463094359... 

# Notes     F       Wavelen
-----------------------------
C0	        16.35 	2109.89
C#0/Db0  	17.32 	1991.47
D0	        18.35 	1879.69
D#0/Eb0  	19.45 	1774.20
E0	        20.60 	1674.62
F0	        21.83 	1580.63
F#0/Gb0    	23.12 	1491.91
G0	        24.50 	1408.18
G#0/Ab0  	25.96 	1329.14
A0	        27.50 	1254.55
A#0/Bb0  	29.14 	1184.13
B0	        30.87 	1117.67


A2 = 3 * 27.5








# The Runtime


1 Thread is running AudioThread 
- Calling Callbakc
- Reading Audio Bus

main thread is hgandling the midi reading / mixer



MIDI FILE IS READ BY MAIN THREAD
    (INSTRUMENT config is also read)
PARSED - Vec of EventMidiWrapper is passed to main


> Mixer gets constructed,
knowing AT START all of the event midi wrapper data needed for the entire track


## run() ->

> 
a loop is started that runs until all event midi wrappers were handled

>
timer keeps the current *playtime*
handles **NOTE_ON** and **NOTE_OFF** commands
instructs each instrument to start or stop playing








