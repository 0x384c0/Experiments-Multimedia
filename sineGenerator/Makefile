all:
	$(MAKE) clean
	$(MAKE) build
	$(MAKE) test
	
build:
	gcc sn.c

test:
	./a.out | ffmpeg -f s16le -ar 44100 -ac 1 -i - -y sound.mp3 && ffplay sound.mp3

clean:
	rm -f a.out
	rm -f sound.mp3