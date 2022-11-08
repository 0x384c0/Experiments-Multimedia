//Copyright © 2016 Radio Anonminous All rights reserved.


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <fftw3.h>
#include <math.h>

#define WINDOW 1024
#define WIDTH 320
#define HEIGHT 240

int main(int argc, char **argv){

double *ffin=(double *)fftw_malloc(sizeof(double)*WINDOW);
fftw_complex *ffout=(fftw_complex *)fftw_malloc(sizeof(fftw_complex)*WINDOW);
fftw_plan p=fftw_plan_dft_r2c_1d(WINDOW, ffin, ffout, FFTW_ESTIMATE);

int fftsize=WINDOW/2+1;
double fps=25.0;
int samplerate=44100;
int framesize=WIDTH*HEIGHT*3;
int w;
FILE *file_in=fopen(argv[1],"rb");

uint8_t *video=malloc(framesize*256);
uint8_t *pixels=malloc(framesize);
int16_t *samples=malloc(WINDOW*2);
int32_t *amp=malloc(fftsize*4);
double *amp_pre=malloc(fftsize*sizeof(double));
double *amp_prev=malloc(fftsize*sizeof(double)*10);
memset(amp_prev,0,fftsize*4*10);

int size;
int q;
int diff;
double diff_double;
int audio_pos=0;
int current_frame=0;
int done_frame=0;
int current_light=0;
int max=0;
double amp_max=0;

fread(video,1,framesize*255,stdin);

while(!feof(file_in)){

memset(amp,0,fftsize*4);

while(current_frame==done_frame){
size=fread(samples,2,WINDOW,file_in);
if(size!=WINDOW){break;}
current_frame=(int)((double)audio_pos*fps/samplerate);
audio_pos+=size;

for(q=0;q<WINDOW;q++){
ffin[q]=(double)samples[q]/32768.0;
}
fftw_execute(p);
for(q=0;q<fftsize;q++){
amp_pre[q]=30.0*sqrt(ffout[q][0]*ffout[q][0]+ffout[q][1]*ffout[q][1]);
}

}

double rms=0;
for(q=0;q<WINDOW;q++){
rms+=pow(samples[q],2);
}

rms=sqrt(rms/WINDOW);

done_frame=current_frame;
diff_double=0;
amp_max=0;
for(q=0;q<fftsize;q++){
if(amp_max<amp_pre[q]){amp_max=amp_pre[q];}
}
amp_max/=10;
if(amp_max==0){amp_max=1;}
for(q=0;q<fftsize;q++){
amp_pre[q]/=amp_max;
}

double *prev_row;
for(w=0;w<5;w++){
prev_row=amp_prev+w*fftsize;
for(q=10;q<250;q++){
diff_double+=pow(amp_pre[q]-prev_row[q],2);
}
}

//diff_double=rms*255/32000;

fprintf(stderr,"frame:%d, diff:%f, ampmax:%d\n",current_frame,(float)diff_double,(int)(amp_max));
int new_light=diff_double;//*255/32000;

/*
for(q=20;q<250;q++){
if(max<amp[q]){max=amp[q];}
}
current_light=max;
*/


if(new_light>255){new_light=255;}

if(current_light<new_light){current_light=new_light;}

if(current_light<0){current_light=0;}



//memset(pixels,current_light,framesize);
memcpy(pixels,video+framesize*current_light,framesize);
fwrite(pixels,1,framesize,stdout);

current_light-=5;
if(current_light<0){current_light=0;}

memmove(amp_prev+fftsize,amp_prev,fftsize*sizeof(double)*5);
memcpy(amp_prev,amp_pre,fftsize*sizeof(double));

memmove(video+framesize,video,framesize*255);
fread(video,1,framesize,stdin);

}
fclose(file_in);


return 0;
}



