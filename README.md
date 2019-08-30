# shawzin-song-converter
A song converter for the Shawzin in Warframe. 

## Usage
### Windows
Download the executable from the releases section (or compile it yourself).

In a command prompt, type
```
> shawzinConverter mySong.txt
```
### macOS/Linux
Download the source code and compile it.
```
$ gcc shawzinConverter.c -o shawzinConverter
$ ./shawzinConverter mySong.txt
```
The converter outputs a song string for use ingame.

## Formatting the input file
The input file is expected to follow a specific format.
You can experiment with `test.txt` in the repository.

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
The default tempo used when recording a song ingame is 120 BPM.

---

Each line after the second should be in the format `<note> <duration>`.

### Notes

For scales that contain the same note in different octaves, an input of a note by itself is interpreted as the lowest octave of that note in the scale.  
Use `^` to raise the note by an octave.  
For example, `F`, `F^` and `F^^` are all valid notes for the pentatonic major scale, where `F` represents the lowest octave in the scale.

If a note has enharmonic equivalents, write it using a flat (b).  
For example, write F# as `Gb`.

### Duration

Express the duration as a fraction of a beat, using either an integer or a fraction.  
For example, `1/4`, `2/3` and `1` are all valid inputs.

## Issues
Not much testing has been done. There are probably still bugs with the converter.

Currently the converter does not check the length of the song.
The duration of an ingame recording must be at least 4 beats (which is 2 seconds at 120 BPM).
