// https://www.youtube.com/embed/s6CzZ7L9tcc
// gcc 1.c -l MagickWand -I /usr/include/ImageMagick/ && a.out








#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <wand/magick_wand.h>

#define TEXTURESIZE 512
#define TEXTUREMASK 0x1ff
#define TEXTURESHIFT 9

int atkinson_matrix[][2]={
{1,0},
{2,0},
{-1,1},
{0,1},
{1,1},
{0,2}
};


int main(int ac, char **av){

uint32_t *texture=malloc(TEXTURESIZE*TEXTURESIZE*4);

MagickWandGenesis();
MagickWand *wand=NewMagickWand();
MagickReadImage(wand,"fire.png");
MagickExportImagePixels(wand,0,0,512,512,"ARGB",CharPixel,texture);
DestroyMagickWand(wand);

FILE *out_pipe=fopen("/dev/shm/zoom/video","wb");

// don't forget to change for 128x64 pixels per 1 bit
uint8_t *screen=malloc(128*64);
uint8_t *screen_pipe=malloc(640*480*3);
uint32_t *back_screen=malloc(640*480*3*4);

int32_t *errors=malloc(128*67*4);

memset(screen_pipe,0,640*480*3);

int q,w,ox,oy;
int out_pos;
int frame_num=time(NULL);
int rot;

int fps,fps_show,fps_stamp;
int si_int,co_int,zoom_int;

for(q=0;q<128*67;q++){
errors[q]=rand()%32;
}


float zoom;

uint32_t v,l=640*480*3;
int interlace_state=0;

uint8_t *tex_luma=malloc(TEXTURESIZE*TEXTURESIZE);
uint8_t *tex_alpha=malloc(TEXTURESIZE*TEXTURESIZE);

for(q=0;q<TEXTURESIZE*TEXTURESIZE;q++){
tex_alpha[q]=(texture[q]&0xFF);
v=(((texture[q]>>8)&0xFF)*0.2989+((texture[q]>>16)&0xFF)*0.5870+((texture[q]>>24)&0xFF)*0.1140);
if(v>255){v=255;}
tex_luma[q]=v;
}


while(1){//main loop

for(w=0;w<64;w+=1){
for(q=((interlace_state+w))&1;q<128;q+=2){
out_pos=(q+w*128);
screen[out_pos+0]=0;
//screen[out_pos+1]=0;
//screen[out_pos+2]=0;
}
}


int g;

int alpha_pixel_int;
int alpha_int;
int tex_pos;
uint32_t tex_pixel;
for(g=0;g<12;g++){

alpha_int=(sin(frame_num/12.3+g*123)*.5+.5)*255.0;
if(alpha_int>255){alpha_int=255;}
if(alpha_int<0){alpha_int=0;}

si_int=(int)((double)sin(frame_num/1000.0+g*123123)*127.0);
co_int=(int)((double)cos(frame_num/1000.0+g*123123)*127.0);

//si_int=0;
//co_int=255;

zoom=(double)alpha_int/150.0*2+.01;
//zoom_int=1;


for(w=0;w<64;w+=1){
for(q=((interlace_state+w))&1;q<128;q+=2){
out_pos=(q+w*128);

/*co,si
-si,co*/

ox=(int)(q*co_int/zoom+w*si_int+(frame_num<<7)*7)>>5;
oy=(int)(q*(-si_int)+w*co_int/zoom+(frame_num<<7)*3)>>5;
tex_pos=(ox&TEXTUREMASK)|((oy&TEXTUREMASK)<<TEXTURESHIFT);

alpha_pixel_int=tex_alpha[tex_pos]*alpha_int;
v=((tex_luma[tex_pos]*alpha_pixel_int)>>16);
v+=screen[out_pos+0];
if(v>255){v=255;}
screen[out_pos+0]=v;
}
}


}

int v,l=128*64*3;


// dither here
int err,nv,e;
for(w=0;w<64;w++){
for(q=0;q<128;q++){
v=screen[q+w*128]+errors[q+w*128];
errors[q+w*128]>>=1;
nv=v<128?0:255;
screen[q+w*128]=nv;
err=(v-nv)/8;
for(e=0;e<6;e++){
errors[q+atkinson_matrix[e][0]+(w+atkinson_matrix[e][1])*128]+=err;
}

screen[q+w*128]=screen[q+w*128]<128?0:255;
}
}





for(w=0;w<64;w++){
for(q=0;q<128;q++){
screen_pipe[(q+w*640)*3+0]=screen[q+w*128]<128?0:255;
screen_pipe[(q+w*640)*3+1]=screen_pipe[(q+w*640)*3+0];
screen_pipe[(q+w*640)*3+2]=screen_pipe[(q+w*640)*3+0];
}
}



fwrite(screen_pipe,1,640*480*3,out_pipe);
frame_num++;
interlace_state++;

fps++;
int current_time=time(NULL);
if(current_time!=fps_stamp){
fps_stamp=current_time;
fps_show=fps;
fps=0;
fprintf(stderr,"Current FPS: %d\n",fps_show);
}

}



return EXIT_SUCCESS;
}















