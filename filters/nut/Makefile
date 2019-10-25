CC=gcc
OUT_FILE=filter
C_FILES=filter.c utils/*.c nut/*.c

.ONESHELL:

all:
	$(MAKE) clean
	$(MAKE) build
	$(MAKE) test
	
build:
	$(CC) -O3 -o $(OUT_FILE) $(C_FILES)

debug:
	$(CC) -g -o $(OUT_FILE) $(C_FILES)
	lldb ./filter

test:
	> "logs.log"
	ffmpeg  -i ~/Movies/test.mp4 -c:v rawvideo -pix_fmt rgb24 -an -t 10 -f nut  - | ./$(OUT_FILE) | ffmpeg -y -i - out.mp4
	# ffplay out.mp4

clean:
	rm -f $(OUT_FILE).exe
	rm -f $(OUT_FILE)
	rm -f "out.mp4"

test_nut:
	$(MAKE) build
	> "logs.log"
	ffmpeg -loglevel panic -i ~/Movies/test_red.mp4 -c:v rawvideo -pix_fmt rgb24 -an -t 10  -f nut  - | ./$(OUT_FILE) > out_exe.nut
	# ffmpeg  -i input.mp4 -c:v rawvideo -pix_fmt rgb24 -c:a pcm_u16le -s 10x10 -f nut  - > out.nut
	# ffmpeg  -i input.mp4 -c:v rawvideo -pix_fmt rgb24 -c:a pcm_u16le -s 4x4 -f nut  - > out4.nut
	# ffmpeg  -i input.mp4 -c:v rawvideo -pix_fmt rgb24 -c:a pcm_u16le -s 5x5 -f nut  - > out5.nut
	# ffmpeg  -i input.mp4 -c:v rawvideo -pix_fmt rgb24 -c:a pcm_u16le -s 300x300 -f nut  - > out300.nut
	# ffmpeg  -i input.mp4 -c:v rawvideo -pix_fmt rgb24 -c:a pcm_u16le -s 5x300 -f nut  - > out5x300.nut
	# ffmpeg  -i input.mp4 -c:v rawvideo -pix_fmt rgb24 -c:a pcm_u16le -ar 48001 -s 10x10 -f nut  - > out_a_rate.nut