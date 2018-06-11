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

/* Private types ========================================================== */

typedef struct AU_Output {
    /* PortAudio */
    PaStream *pa_stream;
    void *pa_callback_userdata;
    PaStreamCallback *pa_callback;

    /* libsndfile */
} AU_Output, *PAU;

/** For convenience - map from the opaque HAU provided by the caller to
 * the visible AU_Output * that we use.
 */
#define POW_FAST \
    PAU pau = (PAU)handle;

#define POW \
    if(!AuInitialized_) return FALSE; \
    POW_FAST

/* Globals ================================================================ */
BOOL AuInitialized_ = FALSE;

/* PortAudio callbacks ==================================================== */

/** Main callback for all PortAudio streams.
 * The callback is a thunk to the actual callback, stored in pau.
 */
int PACallback_(const void *input, void *output,
    unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags, void *handle )
{
    POW_FAST
    return pau->pa_callback(input, output, frameCount, timeInfo,
            statusFlags, pau->pa_callback_userdata);
}

int PAEmptyCallback_(const void *input, void *output,
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
} //Au_Startup

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
} //Au_Shutdowh

/** Create a new output.
 * @param handle {HAU} The output to shut down
 * @return non-NULL on success; NULL on failure
 */
HAU Au_New(AU_SampleFormat format, double sample_rate, void *user_data)
{
    PAU pau;
    PaError pa_err;
    PaSampleFormat pa_format;

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

        pau->pa_callback = PAEmptyCallback_;
        pau->pa_callback_userdata = user_data;

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
    if(!pau) return FALSE;
    free(pau);
    return TRUE;
}

/* High-level functions * ================================================= */

BOOL Au_HL_Sine(HAU handle, int secs)
{
    POW
    if(!pau) return FALSE;
    Pa_Sleep(secs*1000);
    return TRUE;
}

/* vi: set ts=4 sts=4 sw=4 et ai tw=72: */
