#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

double modifyFreq(double currentValue);
double get_random(double min, double max) ;

const int 
SAMPLING_FREQENCY = 44100,
INT16T_MAX = 32767,
DURATION = 5,
BUFFER_SIZE = BUFSIZ * 10,
AMPLITUDE = INT16T_MAX;
const double
BASE_FREQ = 0.02;

int16_t         pcm[BUFFER_SIZE];

int main() {
    srand(time(NULL));
    int 
    sizeLeftTotal = SAMPLING_FREQENCY * DURATION;
    double 
    currenfFreq = BASE_FREQ;
    while (sizeLeftTotal > 0) {
        for (int y = 0; y  < BUFFER_SIZE; y ++) {
            pcm[y] = (int16_t)(sin(y * M_PI * (currenfFreq)) * AMPLITUDE);
        }
        currenfFreq = modifyFreq(currenfFreq);
        sizeLeftTotal -= fwrite(pcm, sizeof(int16_t), BUFFER_SIZE, stdout); //pipe in to ffmpeg
    }
    return EXIT_SUCCESS;
}

/* change freq value  between minRand and maxRand*/
double modifyFreq(double currentValue){
        double 
        minRand = -0.03,
        maxRand = 0.03,
        freqRandomizer = get_random(minRand, maxRand),
        result = currentValue + freqRandomizer;
        if (result > 1) {
            result = 1 + minRand;
        }
        if (result < 0) {
            result = 0 + maxRand;
        }
    return result;
}
/* Returns a random double between min and max */
double get_random(double min, double max) {
  return (max - min) * ( (double)rand() / (double)RAND_MAX ) + min;
}