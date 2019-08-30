#include <stdio.h>
#include <string.h>

#define MAX_STRING 2048
#define SCALE_SIZE 12
#define NOTE_LEN 4
#define DEFAULT_BPM 120
#define DEFAULT_CHARS_PER_BEAT 8

static const char scalePentMin[SCALE_SIZE][NOTE_LEN + 1] = {"C","Eb","F","G","Bb","C^","Eb^","F^","G^","Bb^","Eb^^","F^^"};

static const char scalePentMaj[SCALE_SIZE][NOTE_LEN + 1] = {"C","D","E","A","B","C^","D^","E^","A^","B^","C^^","D^^"};

static const char scaleChromatic[SCALE_SIZE][NOTE_LEN + 1] = {"C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B"};

static const char scaleHexatonic[SCALE_SIZE][NOTE_LEN + 1] = {"C","Eb","F","Gb","G","Bb","C","Eb","F","Gb","G","Bb"};

static const char scaleMajor[SCALE_SIZE][NOTE_LEN + 1] = {"C","D","E","F","G","A","B","C^","D^","E^","F^","G^"};

static const char scaleMinor[SCALE_SIZE][NOTE_LEN + 1] = {"C","D","Eb","F","G","Ab","Bb","C^","D^","Eb^","F^","G^"};

static const char scaleHirajoshi[SCALE_SIZE][NOTE_LEN + 1] = {"C","E","Gb","G","B","C","E","Gb^","G^","B^","C^","E^"};

static const char scalePhrygian[SCALE_SIZE][NOTE_LEN + 1] = {"C","Db","E","F","G","Ab","Bb","C^","Db^","E^","F^","G^"};

static const char scaleOutput[SCALE_SIZE] = {'B','C','E','J','K','M','R','S','U','h','i','k'};

char toBase64(int x) {
    if (0 <= x && x <= 25) {
        return x + 'A';
    } else if (26 <= x && x <= 51) {
        return (x - 26) + 'a';
    } else if (52 <= x && x <= 61) {
        return (x - 52) + '0';
    } else if (x == 62) {
        return '+';
    } else if (x == 63) {
        return '/';
    } else {
        puts("error");
        return EOF;
    }
}

int indexOf(char* x, char* arr, int len) {
    int ret = -1;
    for (int i = 0; i < len; i++) {
        if (strcmp((arr + (NOTE_LEN + 1)*i), x) == 0) {
            ret = i;
        }
    }
    return ret;
}

int main(int argc, char** argv) {
    FILE* fp = fopen(argv[1], "r");

    char out[MAX_STRING] = {'\0'};
    int scale;
    double bpm;
    char* scaleNotes;

    fscanf(fp, "%i", &scale);

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

    fscanf(fp, "%lf", &bpm);
    if (bpm <= 0) {
        puts("Invalid BPM - must be positive.");
        return 1;
    }

    double bps = bpm/60.0;
    double charsPerBeat = DEFAULT_CHARS_PER_BEAT/(bpm/DEFAULT_BPM);

    char note[NOTE_LEN + 1] = {'\0'};
    char bar = 'A';
    double time = 0;
    int duration1 = 0;
    int duration2 = 1;
    int lineNo = 0;

    while (1) {
        lineNo++;
        int res = fscanf(fp, "%s %i/%i\n", note, &duration1, &duration2);
        if (res == EOF) {
            break;
        } else if (res < 3) {
            printf("Invalid line (line %i)\n", lineNo);
            return 1;
        }
        int i = indexOf(note, scaleNotes, SCALE_SIZE);
        if (i == -1) {
            printf("Invalid note %s\n", note);
            return 1;
        }

        out[strlen(out)] = scaleOutput[i];
        out[strlen(out)] = bar;
        out[strlen(out)] = toBase64((int) time);

        time += ((double) duration1/(double) duration2) * charsPerBeat;
        if (time > 63) {
            time -= 63;
            bar++;
        }

        memset(note, 0, NOTE_LEN);
        duration1 = 0;
        duration2 = 1;
    }
    printf("Song string: \n%s\n", out);
    return 0;
}
