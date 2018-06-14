CFLAGS = -Isrc -Wall -g
LDFLAGS = -lportaudio -lsndfile -lpthread

all: sine check_file play_file

%: examples/%.c src/audio_utsl.c src/pa_ringbuffer.c src/audio_utsl.h
	echo =================================================================
	gcc $(CFLAGS) -o $@ $< src/audio_utsl.c src/pa_ringbuffer.c $(LDFLAGS)

