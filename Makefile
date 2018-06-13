CFLAGS = -I. -Wall -g
LDFLAGS = -lportaudio -lsndfile -lpthread

all: sine check_file play_file

%: examples/%.c audio_utsl.c pa_ringbuffer.c audio_utsl.h
	echo =================================================================
	gcc $(CFLAGS) -o $@ $< audio_utsl.c pa_ringbuffer.c $(LDFLAGS)

