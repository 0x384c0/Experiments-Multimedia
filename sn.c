#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

const int 
SAMPLING_FREQENCY = 44100,
INT16T_MAX = 32767,
DURATION = 2,
BUFFER_SIZE = BUFSIZ;

int16_t         pcm[BUFFER_SIZE];


int main() {
    int 
    size = SAMPLING_FREQENCY * DURATION,
    len;
    srand(time(NULL));

    while (size > 0) {
        len = BUFFER_SIZE;

        double 
        amplitude = INT16T_MAX,
        periodDivider =  0.2,
        periodRandomizer = (rand()%3)-1;

        for (int y = 0; y  < len; y ++) {
            pcm[y] = (int16_t)(sin(y * M_PI * (periodDivider)) * amplitude);
        }

        size -= fwrite(pcm, sizeof(int16_t), len, stdout); //pipe in to ffmpeg
    }
    return EXIT_SUCCESS;
}
