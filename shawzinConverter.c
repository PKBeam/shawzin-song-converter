#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_STRING 2048
#define SCALE_SIZE 12
#define NOTE_LEN 4
#define CHORD_LEN 64
#define DEFAULT_BPM 120
#define DEFAULT_CHARS_PER_BEAT 8

#define FORMAT(x) if (formatted) { out[strlen(out)] = x; }

static const char scalePentMin[SCALE_SIZE][NOTE_LEN + 1] = {"C","Eb","F","G","Bb","C^","Eb^","F^","G^","Bb^","Eb^^","F^^"};

static const char scalePentMaj[SCALE_SIZE][NOTE_LEN + 1] = {"C","D","E","A","B","C^","D^","E^","A^","B^","C^^","D^^"};

static const char scaleChromatic[SCALE_SIZE][NOTE_LEN + 1] = {"C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B"};

static const char scaleHexatonic[SCALE_SIZE][NOTE_LEN + 1] = {"C","Eb","F","Gb","G","Bb","C","Eb","F","Gb","G","Bb"};

static const char scaleMajor[SCALE_SIZE][NOTE_LEN + 1] = {"C","D","E","F","G","A","B","C^","D^","E^","F^","G^"};

static const char scaleMinor[SCALE_SIZE][NOTE_LEN + 1] = {"C","D","Eb","F","G","Ab","Bb","C^","D^","Eb^","F^","G^"};

static const char scaleHirajoshi[SCALE_SIZE][NOTE_LEN + 1] = {"C","E","Gb","G","B","C^","E^","Gb^","G^","B^","C^^","E^^"};

static const char scalePhrygian[SCALE_SIZE][NOTE_LEN + 1] = {"C","Db","E","F","G","Ab","Bb","C^","Db^","E^","F^","G^"};

static const char scaleOutput[SCALE_SIZE] = {'B','C','E','J','K','M','R','S','U','h','i','k'};

// convert base-10 to base-64
char toBase64(int x) {
    char c = EOF;
    if (0 <= x && x <= 25) {
        c = x + 'A';
    } else if (26 <= x && x <= 51) {
        c = (x - 26) + 'a';
    } else if (52 <= x && x <= 61) {
        c = (x - 52) + '0';
    } else if (x == 62) {
        c = '+';
    } else if (x == 63) {
        c = '/';
    } else {
        puts("base-64 conversion failed");
    }
    return c;
}

// convert sharps to enharmonic equivalent
// e.g. G# -> Ab
void toEnharmonicFlat(char* note) {
    if (note[1] == '#') {
        note[1] = 'b';
        char c = note[0];
        if ('A' <= c && c < 'G') {
            note[0] = c + 1;
        } else if (c == 'G') {
            note[0] = 'A';
        }
    }
}

int indexOf(char* x, char* arr, int len) {
    int ret = -1;
    for (int i = 0; i < len; i++) {
        // interpret arr as array of strings
        if (strcmp((arr + (NOTE_LEN + 1)*i), x) == 0) {
            ret = i;
        }
    }
    return ret;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        puts("Usage: shawzinconverter <input file> [-f]");
        return 1;
    }
    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("File \"%s\" not found.\n", argv[1]);
        return 1;
    }

    int formatted = (argc > 2) ? !strcmp(argv[2], "-f") : 0;
    int lineNo = 0;
    char out[MAX_STRING] = {'\0'};
    int scale;
    char* scaleNotes;
    double bpm;

    // get scale
    fscanf(fp, "%i", &scale);
    lineNo++;
    switch (scale) {
    case 1: scaleNotes = (char*) scalePentMin;   break;
    case 2: scaleNotes = (char*) scalePentMaj;   break;
    case 3: scaleNotes = (char*) scaleChromatic; break;
    case 4: scaleNotes = (char*) scaleHexatonic; break;
    case 5: scaleNotes = (char*) scaleMajor;     break;
    case 6: scaleNotes = (char*) scaleMinor;     break;
    case 7: scaleNotes = (char*) scaleHirajoshi; break;
    case 8: scaleNotes = (char*) scalePhrygian;  break;
    default: puts("Invalid scale - must be an integer from 1 to 8"); return 1;
    }
    out[0] = '0' + scale;
    FORMAT('\n');

    // get bpm
    fscanf(fp, "%lf", &bpm);
    lineNo++;
    if (bpm <= 0) {
        puts("Invalid BPM - must be positive.");
        return 1;
    }

    // calculate chars per beat
    double charsPerBeat = DEFAULT_CHARS_PER_BEAT * (DEFAULT_BPM/bpm);

    char note[NOTE_LEN + 1] = {'\0'};
    char bar = 'A';
    double time = 0;
    int duration1 = 0;
    int duration2 = 1;

    // iterate over each line of file
    while (!feof(fp)) {
        
        lineNo++;

        // scan in all notes of a chord
        int counter = 0;
        do {
            // read a note
            if (fscanf(fp, "%s", note) < 1) {
                // nothing read
                break;
            }

            // find comments
            if (note[0] == '#') {
                // discard the rest of the line, check for EOF
                while (fgetc(fp) != '\n' && !feof(fp));
                break;
            }

            // find out which note is being read
            toEnharmonicFlat(note);
            int i = indexOf(note, scaleNotes, SCALE_SIZE);
            if (i == -1) {
                printf("Invalid note %s (line %i)\n", note, lineNo);
                return 1;
            }

            // generate converted song string
            out[strlen(out)] = scaleOutput[i];
            out[strlen(out)] = bar;
            // arpeggiate the chord with the counter
            out[strlen(out)] = toBase64(round(time) + counter++);

            // check if finished reading notes
            // if no more notes, integers are read
            // so res != 0 and loop ends
        } while (fscanf(fp, "%i/%i", &duration1, &duration2) == 0);

        FORMAT('\n');

        // increment time offset
        time += ((double) duration1/(double) duration2) * charsPerBeat;
        if (time >= 64) {
            time -= 64;
            bar++;
        }

        duration1 = 0;
        duration2 = 1;

    }

    fclose(fp);

    printf("%s\n", out);

    return 0;
}
