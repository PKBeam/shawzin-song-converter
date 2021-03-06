# shawzin-song-converter
A song converter for the Shawzin in Warframe. 

## Usage
The converter outputs a song string for use ingame.
### Windows
Download the executable from the [releases section](https://github.com/PKBeam/shawzin-song-converter/releases) (or compile it yourself).

In a cmd prompt, navigate to the directory with the executable and run it with
```
shawzinConverter mySong.txt
```
(If using PowerShell you will need to type `.\shawzinConverter mySong.txt` instead)
### macOS/Linux
Download the source code and compile it.
```
gcc shawzinConverter.c -o shawzinConverter
./shawzinConverter mySong.txt
```
### Formatting the output file
Use `-f` to format the output to be more human readable.
```
shawzinConverter mySong.txt -f
```
## Input
The input file is expected to follow a specific format.
Some example files are provided in the repository (under the `Examples/` folder).

### Scale
The first line of the input file should consist of an integer from 1-8 inclusive representing the scale to use.

Number | Scale |
--- | --- |
1 | Pentatonic Major |
2 | Pentatonic Minor |
3 | Chromatic |
4 | Hexatonic |
5 | Major |
6 | Minor |
7 | Hirajoshi |
8 | Phrygian |

### Tempo
The second line should consist of a positive number. This is the tempo of the song in BPM (beats per minute).

---

Each line after the second should be in the format `<note> <note> ... <duration>`.  

### Comments

You can comment out a line with a `#` and it will be ignored by the converter (this only works *after the second line*).
```
5
120
# This is my song
A 1/2
B 1/2
C 1/2
```

### Notes and Chords

For scales that contain the same note in different octaves, an input of a note by itself is interpreted as the lowest octave of that note in the scale.  
Use `^` to raise the note by an octave.  
For example, `F`, `F^` and `F^^` are all valid notes for the pentatonic major scale, where `F` represents the lowest octave in the scale.

You can specify chords, by adding more notes as such:  
```
D F# A 1/2
C E G Bb 1/2
```

Note that the format no longer supports the simultaneous playing of two or more notes, so chords will be arpeggiated - that is, the notes will be played in ascending order as quickly as possible. The order of input determines the order in which notes are played.

For example, an input of `C E G Bb 1/2` will sound more like
```
C 1/64
E 1/64
G 1/64
Bb 29/64
```
when played ingame.

### Duration

Express the duration as a fraction of a beat, using either an integer or a fraction.  
For example, `1/4`, `2/3` and `1` are all valid inputs.

## Issues
Not much testing has been done. There are probably some bugs with the converter. (Please feel free to open an issue if you find one!)

Currently the converter does not check the length of the song.
* The duration of an ingame recording must be at least 4 beats (which is 2 seconds at 120 BPM).
* The recording must also contain at least 6 notes.
(There are most likely other requirements too.)

The converter does not use the ingame format's chord specification. This may be slightly inefficient with respect to size.
For example, if the notes 1 and 3 are simultaneously played with no fret (see below table), the converter will represent this as `BAA FAB` instead of just `GAA`.

## More on the ingame song string format

The song string format used by the game looks something like this:  
`1BAACAIEAQJAYKAgMAo`

We can break it up into chunks to make it easier to analyse.  
`1 BAA CAI EAQ JAY KAg MAo`

### Scale
The first character is a number from 1-8 representing the scale (much like the converter's input file format).

---

We then have sequences of three-character strings in the form `<pitch> <time>`.

### Pitch

The first character represents the pitch.

Fret\String | 1 | 2 | 1+2 | 3 | 1+3 | 2+3 | 1+2+3
--- | --- | --- | --- | --- | --- | --- | --- |
**none** | B | C | D | E | F | G | H |
**left** | J | K | L | M | N | O | P |
**down** | R | S | T | U | V | W | X |
**left + down** | Z | a | b | c | d | e | f |
**right** | h | i | j | k | l | m | n |
**left + right** | p | q | r | s | t | u | v |
**down + right** | x | y | z | 0 | 1 | 2 | 3 |
**left + down + right**  | 5 | 6 | 7 | 8 | 9 | 0 | / |

### Time

The last two characters represent the time at which the notes occur.

The former character is an uppercase alphebetic character representing a rough time offset for the note.  
It starts at `A` (where the time is 0 seconds) and increments every 4 seconds.

The latter character is a finer offset encoded in base-64, with 64 values ranging from `A-Z`, `a-z`, `0-9`, `+`, `/`.  
A value difference of 16 represents one second, and so the entire 64-value range spans 4 seconds (filling in the gaps between the rough offsets).

Put together, these two characters tell you when the note is played.

For example, `BCf` represents the note with lowest pitch and no fret (equivalent ingame to pressing the "1" key without clicking).  
The time of the note is 8 seconds from the `C`, plus (`31/64 * 4`) seconds from the `f`, as `f` is `31` in base-64, and 64 characters represents 4 seconds.

## Miscellaneous notes
The metronome for recording ingame operates at 120 BPM (or 2 beats per second).  
The encoding system's limited precision for timing means that most tempos (e.g. 67 BPM) cannot be timed perfectly. (But they'll probably still sound good.)  
