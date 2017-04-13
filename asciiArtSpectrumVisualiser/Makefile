all:
	$(MAKE) clean
	$(MAKE) build
	$(MAKE) test
	
build:
	gcc ascii_art_visualiser.c -lm -I/usr/local/include -L/usr/local/lib -lfftw3

test:
	ffmpeg -i sound.mp3  -f s16le -acodec pcm_s16le sound.raw && ./a.out sound.raw

clean:
	rm -f a.out
	rm -f sound.raw