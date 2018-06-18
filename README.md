# audio-utsl

A C audio player library you can build from source!
Integrates libsndfile with portaudio.

This project is intended to fill the same niche that
[BASS](https://www.un4seen.com/bass.html) does.  BASS is
awesome, but I couldn't get it to run recently, possibly because of
a false positive on the AV scan.  This project doesn't require any binary
blobs.

## Installation

 - Install libsndfile, portaudio, and pthreads.  E.g., on cygwin, those are
   packages you can install from setup.exe.
 - `make`.  This will build the three examples.

## Usage

To use audio-utsl in your own projects, just copy everything in `src` to
some directory in your project and make sure that directory is listed as `-I`.
See the example `CFLAGS` and `LDFLAGS` in [Makefile](Makefile).

## Internals

 - A producer thread to read from the source using libsndfile
 - A portaudio callback consumer (which probably runs in portaudio's own thread)
   to pass the data to portaudio
 - [PortAudio's ring buffer](https://app.assembla.com/spaces/portaudio/git/source/master/src/common/pa_ringbuffer.h)
   to pass ownership of blocks of data between the producer and the consumer

## Links

 - <http://www.rossbencina.com/code/lockfree>
 - <https://www.cs.cmu.edu/~410-s05/lectures/L31_LockFree.pdf>

## License

Copyright 2018 Christopher White.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License
[here](https://www.apache.org/licenses/LICENSE-2.0).  See the accompanying
[NOTICE](./NOTICE) for required attribution statements.

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an **"AS IS" basis,
without warranties or conditions of any kind,** either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
