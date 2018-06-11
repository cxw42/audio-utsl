all: sine

sine: examples/sine.c audio_utsl.c audio_utsl.h
	gcc -o sine -I. examples/sine.c audio_utsl.c -lportaudio
