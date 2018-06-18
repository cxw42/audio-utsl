/* examples/sine.c: Sine-wave example for audio-utsl.
 * Copyright (c) 2018 Chris White (cxw/Incline).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * https://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>

#define AU_HIGH_LEVEL
    /* to pull in Au_HL_*() */
#include "audio_utsl.h"

extern unsigned long Sine_Most_Recent_frameCount;   /* from audio_utsl.c */

int main(void)
{
    HAU hau;    /* the audio output */

    if(!Au_Startup()) return 1;
    if(!(hau=Au_New(AUSF_F32, 44100, 2, NULL))) return 2;
        /* create an output. */

    Au_HL_Sine(hau, 440.0, 2);      /* 2-sec. A4 sine wave */
    printf("Last callback frame count was %lu\n", Sine_Most_Recent_frameCount);

    if(!Au_Delete(hau)) return 3;
    if(!Au_Shutdown()) return 4;

    return 0;
}

/* vi: set ts=4 sts=4 sw=4 et ai: */
