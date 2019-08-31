# shawzin-song-converter
A song converter for the Shawzin in Warframe. 

## Usage
The converter outputs a song string for use ingame.
### Windows
Download the executable from the [releases section](https://github.com/PKBeam/shawzin-song-converter/releases) (or compile it yourself).

In a cmd prompt, navigate to the directory with the executable and type
```
> shawzinConverter mySong.txt
```
(If using PowerShell you will need to type `.\shawzinConverter mySong.txt` instead)
### macOS/Linux
Download the source code and compile it.
```
$ gcc shawzinConverter.c -o shawzinConverter
$ ./shawzinConverter mySong.txt
```
### Formatting the output file
Use `-f` to format the output to be more human readable.
```
> shawzinConverter mySong.txt -f
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

You can comment out a line with a `#` and it will be ignored by the converter (for lines after the second).
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

### Duration

Express the duration as a fraction of a beat, using either an integer or a fraction.  
For example, `1/4`, `2/3` and `1` are all valid inputs.

## Issues
Not much testing has been done. There are probably many bugs with the converter.

Currently the converter does not check the length of the song.
The duration of an ingame recording must be at least 4 beats (which is 2 seconds at 120 BPM).
The recording must also contain at least 6 notes.

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

Fret\Notes | 1 | 2 | 1+2 | 3 | 1+3 | 2+3 | 1+2+3
--- | --- | --- | --- | --- | --- | --- | --- |
**none** | B | C | D | E | F | G | H |
**left** | J | K | L | M | N | O | P |
**middle** | R | S | T | U | V | W | X |
**left + mid** | Z | a | b | c | d | e | f |
**right** | h | i | j | k | l | m | n |
**left + right** | p | q | r | s | t | u | v |
**mid + right** | x | y | z | 0 | 1 | 2 | 3 |
**left + mid + right**  | 5 | 6 | 7 | 8 | 9 | 0 | / |

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
Any two or more notes can be specified to play at the same time. This is not very easy to do using ingame recording, but the converter makes it quite simple.
