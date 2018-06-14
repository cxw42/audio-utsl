/* examples/play_file.c: File-playing example for audio-utsl.
 * Copyright (c) 2018 Chris White (cxw/Incline).
 * License TODO.
 */

#include <stdio.h>
#include "audio_utsl.h"

int main(int argc, char **argv)
{
    HAU hau;
    int samplerate, channels;
    Au_SampleFormat format;
    long int len = -1;

    int idx;
    int maxidx;
    double time;

    if(argc<2) return 1;
    if(!Au_Startup()) return 2;

    if(!Au_InspectFile(argv[1], &samplerate, &channels, &format,
                            &len)) return 3;
    printf("File %s: %d channels @ %d Hz, len %ld, format ", argv[1],
            channels, samplerate, len);
    switch(format) {
        case AUSF_F32: printf("float32\n"); break;
        case AUSF_I32: printf("int32\n"); break;
        case AUSF_I24: printf("int24\n"); break;
        case AUSF_I16: printf("int16\n"); break;
        case AUSF_I8: printf("int8\n"); break;
        case AUSF_UI8: printf("uint8\n"); break;
        default: printf("unknown\n"); return 4; break;
    }

    /* create an output */
    if(!(hau=Au_New(format, samplerate, channels, NULL))) return 5;

    if(!Au_Play(hau, argv[1])) return 6;

    if(len < 0.0) {     /* if we don't know how long it is, play for ~7 sec. */
        maxidx = 15;
    } else {
        maxidx = 2*(len/samplerate+1);
    }

    for(idx=0; idx < maxidx; ++idx) {
        time = Au_GetTimeInPlayback(hau);
        printf("Time %f\n", time);
        Au_msleep(500);
    }

    Au_Stop(hau);

    if(!Au_Delete(hau)) return 7;
    if(!Au_Shutdown()) return 8;

    return 0;
}

/* vi: set ts=4 sts=4 sw=4 et ai: */
