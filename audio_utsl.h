/* audio_utsl.h: Simple audio library you can build from source.
 * Copyright (c) 2018 Chris White (cxw/Incline).
 * License TODO.
 */

#ifndef _AUDIO_UTSL_H_

/* Headers. -------------------------------------------------------------- */

#include <stdlib.h>
#include <pthread.h>
    /* Sorry - no portability yet.  PRs welcome! :) . */

#include <libsndfile.h>
#include <portaudio.h>

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

/* Data structures ------------------------------------------------------- */

/** A running instance of audio-utsl ("AU").  AU is re-entrant, so you can
 * have multiple instances running at a time.  The limit is the number of
 * physical audio devices available on the system --- you can run as many
 * AU instances as you want if they are all outputting to files.
 */
typedef struct AU_Instance {
    void *placeholder;
} AU_Instance, *HAU;

/* Prototypes ------------------------------------------------------------ */

/** Initialize an instance.
 * @return A new AU_Instance on success; NULL on failure
 */
HAU Au_Init();

/** Shutdown an instance.
 * @param handle {HAU} The instance to shut down
 * @return TRUE on success; FALSE on failure
 */
BOOL Au_Shutdown(HAU handle);

#define _AUDIO_UTSL_H_
#endif /* _AUDIO_UTSL_H_ */

/* vi: set ts=4 sts=4 sw=4 et ai: */
