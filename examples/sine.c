/* examples/sine.c: Sine-wave example for audio-utsl.
 * Copyright (c) 2018 Chris White (cxw/Incline).
 * License TODO.
 */

#define AU_HIGH_LEVEL
    /* to pull in Au_HL_*() */

#include "audio_utsl.h"

int main(void)
{
    HAU hau;    /* the audio output */

    if(!Au_Startup()) return 1;
    if(!(hau=Au_New(AUSF_F32, 44100, NULL))) return 2;   /* create an output */

    Au_HL_Sine(hau, 2);     /* 2-sec. sine wave */

    if(!Au_Delete(hau)) return 3;
    if(!Au_Shutdown()) return 4;

    return 0;
}

/* vi: set ts=4 sts=4 sw=4 et ai: */
