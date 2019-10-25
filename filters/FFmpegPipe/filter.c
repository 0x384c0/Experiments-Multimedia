#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(){

	int
	width = 256,
	heigh = 192,
	pixelsPerFrame = width * heigh,
	RGBframeBufferSize = pixelsPerFrame * 3;

	// allocate variables and buffer
	uint8_t 
	*pixels = malloc(RGBframeBufferSize), //frame buffer
	*pix; // pointer to current pixel

	while(1){
		// read frame
		if(fread(pixels, 1, RGBframeBufferSize, stdin) <= 0){
			break;
		}
		// process image
		pix=pixels;
		for(int q = 0; q < pixelsPerFrame; q++){
				*pix++ =* pix * 2; // multiple red to 2
				*pix++ =* pix + 120; // shift green channel
				*pix++ =* pix + q/10; // lines in blue channel
		}
		// write frame back
		fwrite(pixels, 1, RGBframeBufferSize, stdout);
	}
	return EXIT_SUCCESS;
}