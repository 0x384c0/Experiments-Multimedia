all:
	$(MAKE) clean
	$(MAKE) build
	$(MAKE) test
	
build:
	gcc audioToMidiConverter.c -lm -I/usr/local/include -L/usr/local/lib -lfftw3

test:
	ffmpeg -i sound.mp3  -f s16le -acodec pcm_s16le sound.raw && cp sound.raw sampleraw && ./a.out sound.raw

clean:
	rm -f a.out
	rm -f sound.raw
	rm -f sampleraw