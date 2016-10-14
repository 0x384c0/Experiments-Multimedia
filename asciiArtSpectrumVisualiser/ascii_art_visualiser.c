#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <fftw3.h>
#include <math.h>


#define MAXWANT 155
#define WINDOW 2048

uint8_t ascii_luma[]={32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,96,96,96,96,96,45,45,45,45,45,45,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,94,94,94,94,94,94,94,94,94,94,94,92,92,92,92,92,92,126,126,126,126,126,124,124,124,124,124,124,124,124,124,124,124,124,62,62,62,62,62,62,62,62,62,62,62,125,125,125,125,125,125,125,125,125,125,125,122,122,122,122,122,122,114,114,114,114,114,114,111,111,111,111,111,117,117,117,117,117,117,119,119,119,119,119,119,55,55,55,55,55,121,121,121,121,121,121,53,53,53,53,53,53,109,109,109,109,109,83,83,83,83,83,83,113,113,113,113,113,113,104,104,104,104,104,88,88,88,88,88,88,90,90,90,90,90,90,85,85,85,85,85,85,85,85,85,85,85,87,87,87,87,87,87,87,87,87,87,87,82,82,82,82,82,82,81,81,81,81,81,81,35,35,35,35,35,48,48,48,48,48,48,48,48,48,48,48,48,78};

int16_t *samples; // buffer for raw audio samples
uint8_t *pixels;

#define math_min(a,b) (a>b?b:a)
#define math_max(a,b) (a>b?a:b)

double *ffin;
fftw_complex *ffout1;
fftw_complex *ffout2;
fftw_plan p;
fftw_plan p2;

int *spect1;
int *spect2;
int **history;

void samples_to_FFT(int16_t *samples, double *arr, fftw_plan p)
{
  int q;
  for (q = 0; q < WINDOW; q++)
    {
      arr[q] = samples[q] / 32768.0;
    }
  fftw_execute (p);
}


void downscale(int *spect, int *history){
int q;
for(q=0;q<MAXWANT;q++){
history[q]=(history[q]+spect[q])/2;
}

for(q=0;q<MAXWANT;q+=2){
spect[q/2]=(history[q]+spect[q+1])/2;
}

}


void
calcPowerSpectrum (fftw_complex * arr, int *res)
{
  int q, len;//=WINDOW/2+1;
len=MAXWANT;
  for (q=0;q<len;q++)
    {
      res[q] = 30.0*sqrt (arr[q][0] * arr[q][0] + arr[q][1] * arr[q][1]);
    }
}




void calcTimeStats(int16_t *samples, int *_min, int *_max, int *_rms){
int min=samples[0];
int max=samples[0];
int64_t rms=0;
int q;
for(q=0;q<WINDOW;q++){
min=math_min(min,samples[q]);
max=math_max(max,samples[q]);
rms+=samples[q]*samples[q];
}
*_min=abs(min);
*_max=abs(max);
*_rms=(int)(sqrt(rms/WINDOW));
}

void multSamples(int16_t *samples, double mult){
int q,s;
for(q=0;q<WINDOW;q++){
s=(double)samples[q]*mult;
if(s>32767){s=32767;}
if(s<-32767){s=-32767;}
samples[q]=s;
}
}


void compare(int16_t *samples1, int16_t *samples2, int *history1, int *history2, int packet, char *res, int8_t *pix){

int samples_max1,samples_min1,samples_rms1;

calcTimeStats(samples1,&samples_min1,&samples_max1,&samples_rms1);
samples_to_FFT(samples1,ffin,p);
calcPowerSpectrum(ffout1,spect1);
downscale(spect1,history1);


int q;
// draw wave
//drawLine(pix,192,0+25-samples_max1/1500,25+samples_min1/1500);
//drawLine(pix,255,0+25-samples_rms1/1500,25+samples_rms1/1500);

// draw spectrum
for(q=0;q<75;q++){
printf("%c",ascii_luma[math_min(255,spect1[q+10])]);
}
printf("\n");
}


int main(int argc, char** argv){

int win=WINDOW;
samples=malloc(win*2);

FILE *in1=fopen(argv[1],"rb");
if(in1){
fseek(in1,0x66,0);
} else {
in1=stdin;
}
int complex_size=win/2+1;
int channels=1;

int ypos=0;
int q;

ffin= (double *) fftw_malloc (sizeof (double) * win);
ffout1=(fftw_complex *) fftw_malloc (sizeof (fftw_complex) * win);
p =fftw_plan_dft_r2c_1d (win, ffin, ffout1, FFTW_ESTIMATE);

spect1=malloc(sizeof(int)*win);

history=malloc(sizeof(int*)*channels*2);
for(q=0;q<channels*2;q++){
history[q]=malloc(sizeof(int)*win);
memset(history[q],0,sizeof(int)*win);
}

int blocksize=WINDOW*2*channels;
char *padding=malloc(1000000);
int16_t *datablock1=malloc(blocksize);
int16_t *samples1=malloc(blocksize);

int c,off;
char out[1280];
int outpos=0;

while(1){
int s1=fread(datablock1,1,blocksize,in1);
if(s1!=blocksize){break;}

outpos=0;
for(c=0;c<channels;c++){
for(q=0;q<WINDOW;q++){
off=q*channels+c;
samples1[q]=datablock1[off];
}

compare(samples1,0,history[c*2],history[c*2+1],0,0,0);
}
}


fclose(in1);

return 0;
}


