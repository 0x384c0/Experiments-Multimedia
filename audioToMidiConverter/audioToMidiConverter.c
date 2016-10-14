#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <fftw3.h>
#include <math.h>

#define WINDOW 4096
#define HANDS 5
#define math_min(a,b) (a>b?b:a)
#define math_max(a,b) (a>b?a:b)

int16_t *samples; // buffer for raw audio samples


double *ffin;
fftw_complex *ffout1;
fftw_plan p;

int *spect1;
int *spect2;
int **history;
int notes[500000][HANDS];


void samples_to_FFT(int16_t *samples, double *arr, fftw_plan p){
  int q;
  for (q = 0; q < WINDOW; q++){
      arr[q] = samples[q] / 32768.0;
    }
  fftw_execute (p);
}



void calcPowerSpectrum (fftw_complex * arr, int *res)
{
  int q, len=WINDOW/2+1;
  for (q=0;q<len;q++)
    {
      res[q] = 30.0*sqrt (arr[q][0] * arr[q][0] + arr[q][1] * arr[q][1]);
    }
}




int **huitas;
int *huitas_peak;

int main(int argc, char** argv){

  int win=WINDOW;
  samples=malloc(win*2);

  huitas=malloc(1000*sizeof(int*));
  huitas_peak=malloc(1000*sizeof(int));

  FILE *in1=fopen(argv[1],"rb");
  int complex_size=win/2+1;

  ffin= (double *) fftw_malloc (sizeof (double) * win);
  ffout1=(fftw_complex *) fftw_malloc (sizeof (fftw_complex) * win);
  p=fftw_plan_dft_r2c_1d (win, ffin, ffout1, FFTW_ESTIMATE);

  spect1=malloc(sizeof(int)*win);

  int blocksize=WINDOW*2;
  char *padding=malloc(1000000);
  int16_t *datablock1=malloc(blocksize);
  int16_t *samples1=malloc(blocksize);

  int peak=0,ampmax=0;
  int q,w,e;
  int c,off;
  char out[1280];
  int outpos=0;

  int sec;
  int s1;
  for(sec=0;sec<128;sec++){
  fseek(in1,44100*2*sec+2000,0);

  s1=fread(samples1,1,blocksize,in1);
  if(s1!=blocksize){break;}

  samples_to_FFT(samples1,ffin,p);
  calcPowerSpectrum(ffout1,spect1);

  // for(c=0;c<complex_size;c++){printf("%d,",spect1[c]);}printf("\n\n");
  huitas[sec]=malloc(complex_size*sizeof(int));
  memcpy(huitas[sec],spect1,complex_size*sizeof(int));
  peak=0;ampmax=0;
  for(c=0;c<complex_size;c++){if(spect1[c]>ampmax){ampmax=spect1[c];peak=c;}}
  huitas_peak[sec]=peak;
  // printf("Fingerprint of note %d, peak %d\n",sec,peak);
  }

  fclose(in1);
  FILE *in2=fopen("sampleraw","r");

  int t;
  int pos=0;

  for(q=0;q<2000000;q++){
  s1=fread(samples1,1,blocksize,in2);
  if(s1!=blocksize){break;}
  samples_to_FFT(samples1,ffin,p);
  calcPowerSpectrum(ffout1,spect1);
  

  for(t=0;t<HANDS;t++){
  peak=0;ampmax=0;
  for(c=0;c<complex_size;c++){if(spect1[c]>ampmax){ampmax=spect1[c];peak=c;}}
  int64_t winner=0,errmin=0xFFFFFFFF;
  for(w=0;w<127;w++){int64_t err=pow(huitas_peak[w]-peak,2);if(err<errmin){errmin=err;winner=w;}}
  notes[pos][t]=(int)winner;
  // printf("win %d at %d, tr %d\n",(int)winner,pos,t);
  int around=10;
  int govninamin=peak-around;
  int govninamax=peak+around;
  int localpeak=peak;
  while(govninamin>0 && spect1[govninamin]<=localpeak){
  localpeak=spect1[govninamin];
  govninamin--;
  }
  localpeak=peak;
  while(govninamax<complex_size && spect1[govninamax]<=localpeak){
  localpeak=spect1[govninamax];
  govninamax++;
  }

  for(c=govninamin;c<=govninamax;c++){
  if(c>=0 && c<complex_size){spect1[c]=0;}
  }

  }
  pos++;
  }

  for(w=0;w<HANDS;w++){
  for(c=0;c<pos;c++){
  printf("%d,",notes[c][w]);
  }
  printf("\n");
  }






  return 0;
}

