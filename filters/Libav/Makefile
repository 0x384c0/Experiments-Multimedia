CC=clang
C_FILES=filter.c
OUT_FILE=filter
FFMPEG_LIBS=-lavformat -lavcodec -lavfilter -lavdevice -lavutil -lswresample -lswscale

.ONESHELL:

clean:
	find . -name "output.*" -delete

build:
	$(CC) -O3 -o $(OUT_FILE) $(C_FILES) -I. -I.. -I/usr/local/include/ -L/usr/local/lib/ $(FFMPEG_LIBS)

test:
	$(MAKE) clean
	ffmpeg -i ~/Movies/test.mp4 -t 00:05 -f nut - | ./filter "pipe:" "output.mp4"

test_remuxing:
	$(CC) -o remuxing remuxing.c -I. -I.. -I/usr/local/include/ -L/usr/local/lib/ $(FFMPEG_LIBS)
	./remuxing ~/Movies/test.mp4 output.mkv

test_transcoding:
	$(CC) -o transcoding transcoding.c -I. -I.. -I/usr/local/include/ -L/usr/local/lib/ $(FFMPEG_LIBS)
	./transcoding ~/Movies/test.mp4 output.mkv