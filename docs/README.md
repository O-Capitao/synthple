# Synthple revision: 2023 - 08 - 14


===
## Main Thread

* create **Synthple**
* create **Bus**
* agarra argumentos do comando:
    - bpm
    - midifilename 

* Main loop
    - handle events from *User Input*




## Synthple
* create and start **AudioThread** - container for paCallback
* start **Generator**
* flag MAIN_QUIT passed from main to stop playback/ cleanup
* antes de correr - organiza uma sequência de notas em lista de **MidiEventWrapper**


### Synthple.run()
de dt em dt

> dentro do ciclo:
buffer vazio? -> enche

> no fim de cada step 
espera T_BUFFER / 2 para verificar de novo


## AudioThread
* Handle all Portaudio stuff - start, stop, openStream, closeStream
* contains PaCallback -> called by Portaudio, runs in separate thread, passed to *Pa_OpenDefaultStream* on **_openStream()**


## Bus
* one ptr to a sample queue -> for passing samples into the callback
* sample format should be float


## Generators
### Base Generator:

### Sine Generator:
* generate a sine wave of frquency f for t seconds using Math.sin()

### Square Generator:
* generate Square wave of frquency f for t seconds










===
### THEORY: Frq Stuff
1 rad/s = 1 / 2π Hz 

freq_rad_s = freq_hz / 2π

### THEORY: About MIDI messages:

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

### Ticks:
In the MIDI specification, and in most DAW software, the smallest unit of metric timing is expressed in "ticks", which is to say, partial units of a beat.
Commonly the number of ticks used is 480 or 960 parts per quarter note.

### Message struct:
80/90 -> note off/ on    |    4 bit for velocity

### Notes vs Freq
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


### THEORY : Clocks:

> Naive clock:

```c
/* clock example: frequency of primes */
#include <stdio.h>      /* printf */
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */
#include <math.h>       /* sqrt */

int frequency_of_primes (int n) {
  int i,j;
  int freq=n-1;
  for (i=2; i<=n; ++i) for (j=sqrt(i);j>1;--j) if (i%j==0) {--freq; break;}
  return freq;
}

int main ()
{
  clock_t t;
  int f;
  t = clock();
  printf ("Calculating...\n");
  f = frequency_of_primes (99999);
  printf ("The number of primes lower than 100,000 is: %d\n",f);
  t = clock() - t;
  printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
  return 0;
}
```

#

# MAKEFILEs

sempre:

```Makefile

target: dependencies
    action

```



## Avliação da coisa Oct 2024


proposta para nova config

- Configuração do synthple é feita baseada em ficheiros song,
localizados debaixo de {datadir}/songs

```yaml
# song file
id: songname
bpm: 230
signature: 3/4, 4/4 etc etc # TODO
swing: 0

midifiles:
    - id: bass1 
      path: midi/file1.mid
    - id: horn2
      path: midi/hornsolo.mid

tracks:
    - id: basstrack
      gain: 0.7
      wave:
        type: SAW
        inflection: 0.5
      envelope:
        attack: 1
        sustain: 1
        decay: 1
        release: 1
      effects:
        - type: reverb1
          gain-wet: 1
          gain-dry: 1
          parameter: 1
        - type: compression
          gain-wet: 1
          gain-dry: 1
          parameter: 23
        
parts:
    - id: intro
      repeat: 2 #0 for infinite repeat
      length-bars: 4 # for a single repeat
      midi-to-track:
        - file_id: bass1
          track_id: basstrack
          length_bars: 4
          repeat: 0
          priority: 3 # in case of overlap highest priority wins

          offset: # where is the midi file put into
            bars-start: 2
            beats-start: 0
          
          trim:
            bars-start: 0
            beats-start: 0
            bars-end: 0
            beats-end: 0



```