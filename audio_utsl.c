/* audio_utsl.c: Simple audio library you can build from source.
 * Copyright (c) 2018 Chris White (cxw/Incline).
 * License TODO.
 */

/* Headers ================================================================ */

#include "audio_utsl.h"

/* Implementation headers */
#include <sndfile.h>
#include <portaudio.h>

#include <string.h>

#define _USE_MATH_DEFINES
    /* Or you don't get M_PI from math.h on my system */
#include <math.h>

#define UNUSED(x) ((void)(x))

/* Private types ========================================================== */

typedef struct AU_Output *PAU;

typedef struct AU_Userdata {
    PAU pau;
    void *data;
} AU_Userdata, *PAU_Userdata;

typedef struct AU_Output {
    /** A copy of the AU sample format */
    AU_SampleFormat format;

    /* PortAudio */

    /** The PortAudio stream */
    PaStream *pa_stream;

    /** The callback that does the work.  PACallback_() dispatches to
     * this function. */
    PaStreamCallback *pa_callback;

    /* Userdata for the pa_callback */
    void *pa_callback_userdata;

    /* libsndfile TODO */
} AU_Output;

/** For convenience - map from the opaque HAU provided by the caller to
 * the visible AU_Output * that we use.
 */
#define POW_FAST \
    PAU pau = (PAU)handle;

#define POW \
    if(!AuInitialized_) return FALSE; \
    POW_FAST \
    if(!pau) return FALSE;


/* Globals ================================================================ */
BOOL AuInitialized_ = FALSE;

/* PortAudio callbacks ==================================================== */

/** Main callback for all PortAudio streams.
 * The callback is a thunk to the actual callback, stored in pau.
 */
static int PACallback_(const void *input, void *output,
    unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags, void *handle )
{
    POW_FAST
    return pau->pa_callback(input, output, frameCount, timeInfo,
            statusFlags, pau->pa_callback_userdata);
}

static int PAEmptyCallback_(const void *input, void *output,
    unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags, void *handle )
{
    return paComplete;      /* no audio */
}


/* Init/termination ======================================================= */

/** Initialize AU.  Must be called before any other functions.
 * @return TRUE on success; FALSE on failure.
 */
BOOL Au_Startup()
{
    if(AuInitialized_) return TRUE;     /* idempotent */

    /* Initialize PortAudio */
    PaError err = Pa_Initialize();
    if( err != paNoError ) return FALSE;
        /* TODO figure out error reporting - Pa_GetErrorText(err) */

    AuInitialized_ = TRUE;
    return TRUE;
} /* Au_Startup */

/** Shutdown AU.  Call this after closing any outputs you have open.
 * @return TRUE on success; FALSE on failure
 */
BOOL Au_Shutdown()
{
    if(!AuInitialized_) return TRUE;    /* idempotent */

    /* Shutdown PortAudio */
    PaError err = Pa_Terminate();
    if( err != paNoError ) return FALSE;
        /* TODO figure out error reporting - Pa_GetErrorText(err) */

    AuInitialized_ = FALSE;
    return TRUE;
} /* Au_Shutdown */

/** Create a new output.
 * @param handle {HAU} The output to shut down
 * @return non-NULL on success; NULL on failure
 */
HAU Au_New(AU_SampleFormat format, double sample_rate, void *user_data)
{
    PAU pau;
    PaError pa_err;
    PaSampleFormat pa_format;

    (void)user_data;    /* not yet used */

    if(!AuInitialized_) return FALSE;
    /* Map the format, since we don't directly expose the implementation
     * types to the caller.*/
    switch(format) {
        case AUSF_F32: pa_format = paFloat32; break;
        case AUSF_I32: pa_format = paInt32; break;
        case AUSF_I24: pa_format = paInt24; break;
        case AUSF_I16: pa_format = paInt16; break;
        case AUSF_I8: pa_format = paInt8; break;
        case AUSF_UI8: pa_format = paUInt8; break;
        case AUSF_CUSTOM: pa_format = paCustomFormat; break;
        default: return NULL;
    }

    if(sample_rate < 1.0) return NULL;

    do {    /* init with rollback */
        pau = (PAU)malloc(sizeof(AU_Output));
        if(!pau) break;
        memset(pau, 0, sizeof(AU_Output));

        pau->format = format;

        pau->pa_callback = PAEmptyCallback_;
        pau->pa_callback_userdata = NULL;

        pa_err = Pa_OpenDefaultStream(
            &pau->pa_stream,
            0,              /* no input channels */
            2,              /* stereo output */
            pa_format,
            sample_rate,
            256,            /* frames per buffer, i.e. the number
                            of sample frames that PortAudio will
                            request from the callback. Many apps
                            may want to use
                            paFramesPerBufferUnspecified, which
                            tells PortAudio to pick the best,
                            possibly changing, buffer size.*/
            PACallback_,    /* dispatches to pau->pa_callback */
            pau );      /*This is a pointer that will be passed to
                            your callback*/
        if(pa_err != paNoError) break;

        return (HAU)pau;    /* Success exit */
    } while(0);

    /* If we got here, rollback whatever was done. */
    if(pau) {
        if(pau->pa_stream) Pa_CloseStream(pau->pa_stream);
        free(pau);
    }

    return NULL;
} /* Au_New */

/** Close an output.  If this succeeds, any memory associated witht that
 * output has been freed.
 * @param handle {HAU} The output to shut down
 * @return TRUE on success; FALSE on failure
 */
BOOL Au_Delete(HAU handle)
{
    POW
    free(pau);
    return TRUE;
}

/* High-level functions * ================================================= */

/** Callback to make a sine wave.  Currently only supports paFloat32
 * format.
 */
static int PA_HL_Sine_Callback_(const void *input, void *output,
    unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags, void *handle )
{
    double freq_rad = *((double *)handle);
    float *out = (float*)output;
    unsigned int i;
    double d;
    PaTime t = timeInfo->outputBufferDacTime;   /* time for sample 0 */
    double time_step = 1.0/44100.0; /* for now, assume 44.1kHz */

    UNUSED(input);
    UNUSED(statusFlags);

    for( i=0; i<frameCount; i++ )
    {
        d = sin(freq_rad * t);
        *out++ = d; /* left */
        *out++ = d; /* right */
        t += time_step;
    }
    return paContinue;
} /* PA_HL_Sine_Callback_ */

BOOL Au_HL_Sine(HAU handle, double freq_Hz, int secs)
{
    double freq_rad = 2.0 * M_PI * freq_Hz;
    PaError pa_err;
    void *old_userdata;
    PaStreamCallback *old_pacallback;

    POW
    if(pau->format != AUSF_F32) return FALSE;

    Pa_StopStream(pau->pa_stream);      /* just in case */

    const PaStream *strinfo;
    if(!(strinfo=Pa_GetStreamInfo(pau->pa_stream))) return FALSE;
    /* TODO pass the stream params to the callback */

    old_userdata = pau->pa_callback_userdata;
    pau->pa_callback_userdata = (void *)&freq_rad;
    old_pacallback = pau->pa_callback;
    pau->pa_callback = PA_HL_Sine_Callback_;

    pa_err = Pa_StartStream(pau->pa_stream);
    if(pa_err != paNoError) return FALSE;

    Pa_Sleep(secs*1000);

    Pa_StopStream(pau->pa_stream);

    pau->pa_callback_userdata = old_userdata;
    pau->pa_callback = old_pacallback;

    return TRUE;
}

/* vi: set ts=4 sts=4 sw=4 et ai tw=72: */
