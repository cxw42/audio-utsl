/* examples/play_file.c: File-playing example for audio-utsl.
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
#include <limits.h>
#include "audio_utsl.h"

extern unsigned int AU_PAPC_Count;
extern unsigned int AU_SFFR_Count;

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

    /* If given a second parameter, play until the file is done. */
    if(argc>2) maxidx = INT_MAX;

    for(idx=0; idx < maxidx; ++idx) {
        time = Au_GetTimeInPlayback(hau);
        printf("Time %f\tpapc %d\tsffr %d\n", time, AU_PAPC_Count, AU_SFFR_Count);
        if(time>0 && !Au_IsPlaying(hau)) break;
            /* Check time>0 because IsPlaying is not necessarily true
             * just after an Au_Play() call, at which point time=0.*/

        Au_msleep(500);
    }

    Au_Stop(hau);

    if(!Au_Delete(hau)) return 7;
    if(!Au_Shutdown()) return 8;

    return 0;
}

/* vi: set ts=4 sts=4 sw=4 et ai: */
