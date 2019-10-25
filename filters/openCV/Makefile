.PHONY: all clean test

CC=clang
CFLAGS=`pkg-config --cflags --libs opencv` -std=c++11 -lc++

SOURCES=utils/CameraWindow.cpp utils/ImageUtils.cpp utils/ObjectDetector.cpp
OBJECTS=$(SOURCES:.cpp=.o)

all:
	$(MAKE) clean
	$(MAKE) obj_detect
	$(MAKE) test

test:
	./a.out

clean:
	rm -rf *.out *.out.dSYM


#utils
#TODO ObjectDetector is dependent of ImageUtils. represent it here.
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

# targets
	
hello: $(OBJECTS)
	$(CC) $(CFLAGS) $^ hello.cpp

obj_detect: $(OBJECTS)
	$(CC) $(CFLAGS) $^ obj_detect.cpp

obj_inpaint: $(OBJECTS)
	$(CC) $(CFLAGS) $^ obj_inpaint.cpp

obj_blur: $(OBJECTS)
	$(CC) $(CFLAGS) $^ obj_blur.cpp

video_obj_detect: $(OBJECTS)
	$(CC) $(CFLAGS) $^ video_obj_detect.cpp

#others
video_test:
	ffmpeg -i "in.mp4" -s 256x192 -f rawvideo -pix_fmt rgb24 - | ./a.out | ffmpeg -s 256x192 -f rawvideo -pix_fmt rgb24 -i - -y "out.mp4"
