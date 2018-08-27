#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

const int
INT16T_MAX = 32767,
AMPLITUDE = INT16T_MAX,
sampleSize = 2; // number of samples

int16_t sampleBuffer = 0;

int main(){
	while(fread(&sampleBuffer, 1, sampleSize, stdin)){
		float sample = (float)sampleBuffer/(float)AMPLITUDE;
		printf("%.3f, ", sample);
	}
	return EXIT_SUCCESS;
}

//gcc raw_audio_to_array.c  && cat audio.raw | ./a.out > audio.txt
