/* audio_utsl.h: Simple audio library you can build from source.
 * Copyright (c) 2018 Chris White (cxw/Incline).
 * License TODO.
 */

#ifndef _AUDIO_UTSL_H_

/* Headers. -------------------------------------------------------------- */

#include <stdlib.h>
#include <pthread.h>
    /* Sorry - no portability yet.  PRs welcome! :) . */

/* A few niceties. ------------------------------------------------------- */
#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef NULL
#define NULL ((void *)(0))
#endif

/* Data structures, constants, and enums --------------------------------- */

/** A running instance of audio-utsl ("AU"), connected to a particular
 * output.  AU is re-entrant, so you can have multiple instances running
 * at a time.  The limit is the number of physical audio devices
 * available on the system --- you can run as many AU instances as you
 * want if they are all outputting to files.  There is a 1-1
 * relationship between AU instances and outputs.
 */
typedef void *HAU;

typedef enum Au_SampleFormat { AUSF_F32, AUSF_I32, AUSF_I24, AUSF_I16, AUSF_I8,
    AUSF_UI8, AUSF_CUSTOM } Au_SampleFormat;

/* Initialization and termination functions ------------------------------ */

/** Initialize AU.  Must be called before any other functions.
 * @return TRUE on success; FALSE on failure.
 */
extern BOOL Au_Startup();

/** Shutdown AU.  Call this after closing any outputs you have open.
 * @return TRUE on success; FALSE on failure
 */
extern BOOL Au_Shutdown();

/** Create a new output.
 * @param format The output format
 * @param sample_rate The sample rate, in Hz
 * @param channels How many channels
 * @param user_data Currently unused
 * @return non-NULL on success; NULL on failure
 */
extern HAU Au_New(Au_SampleFormat format, int sample_rate,
        int channels, void *user_data);

/** Close an output.  If this succeeds, any memory associated witht that
 * output has been freed.
 * @param handle {HAU} The output to shut down
 * @return TRUE on success; FALSE on failure
 */
extern BOOL Au_Delete(HAU handle);

/* File-reading and -playback functions ---------------------------------- */

/** Get the sample rate and format from a file.
 * @param filename The file
 * @samplerate Will be filled in with the sample rate on success
 * @channels Will be filled in with the channel count on success
 * @format Will be filled in with the sample format on success.
 *          If the format is unknown to audio-utsl, the return value
 *          will be AUSF_CUSTOM.
 * @return TRUE on success; FALSE on failure. */
BOOL Au_InspectFile(const char *filename, int *samplerate, int *channels,
        Au_SampleFormat *format);

/** Play audio file #filename on output #handle. */
BOOL Au_Play(HAU handle, const char *filename);

/** Stop a playback that was started with Au_Play.
 * @return FALSE on invalid #hau; otherwise TRUE.
 */
BOOL Au_Stop(HAU hau);

/* Utility functions ----------------------------------------------------- */

/** Sleep for approximately #ms milliseconds.
 * Actually just wraps Pa_Sleep().  */
void Au_msleep(long ms);

/* High-level test functions --------------------------------------------- */

#ifdef AU_HIGH_LEVEL

/** Play a sine wave for #secs seconds at #freq_Hz.
 * Only supports the AUSF_F32 format, 2 channels.
 * @return FALSE if an error occurs; otherwise, TRUE.
 */
extern BOOL Au_HL_Sine(HAU handle, double freq_Hz, int secs);
#endif /* AU_HIGH_LEVEL */

#define _AUDIO_UTSL_H_
#endif /* _AUDIO_UTSL_H_ */

/* vi: set ts=4 sts=4 sw=4 et ai tw=72: */
