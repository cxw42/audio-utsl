all: sine

sine: examples/sine.c audio_utsl.c audio_utsl.h
	echo =================================================================
	gcc -o sine -I. -Wall examples/sine.c audio_utsl.c -lportaudio
