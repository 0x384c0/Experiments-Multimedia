all:
	$(MAKE) clean
	$(MAKE) build
	$(MAKE) test
	
build:
	gcc "filter.c"

test:
	ffmpeg -i "in.mp4" -s 256x192 -f rawvideo -pix_fmt rgb24 - | ./a.out | ffmpeg -s 256x192 -f rawvideo -pix_fmt rgb24 -i - -y "out.mp4"

clean:
	rm -f a.out
	rm -f "out.mp4"