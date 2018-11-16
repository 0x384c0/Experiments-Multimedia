

//ffmpeg -v 0 -i anonfm.aac -f s16le -ac 1 -ar 44100 - | ./a.out | ffmpeg -v 0 -r 25 -f rawvideo -pix_fmt yuv420p -s 640x480 -i - -vcodec libx264 -t 90 -y test.mp4

//Рядом должна быть директория out с 55 картинками




#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <wand/magick_wand.h>


#define SAMPLERATE 44100
#define FPS 25
#define WIDTH 640
#define HEIGHT 480

uint8_t pixels_oscillo[HEIGHT][WIDTH];
uint8_t pixels_wave[HEIGHT][WIDTH];
uint8_t pixels[HEIGHT][WIDTH];
uint8_t *color_dummy=NULL;
uint8_t *panorama=NULL;
int frame_num=0;

typedef struct{
int rrr;
int max;
} HISTORY_ITEM;

HISTORY_ITEM history[WIDTH];

void flush_screen(){
// yuv420p

fwrite(pixels,WIDTH,HEIGHT,stdout);

int dummy_size=WIDTH*HEIGHT/4;
if(!color_dummy){
color_dummy=malloc(dummy_size);
}
memset(color_dummy,(sin((double)frame_num/55.0)+1)*127.5,dummy_size);
memset(color_dummy+366*640/4,(sin((double)frame_num/755.0+8723.5)+1)*127.5,640*110/4);
fwrite(color_dummy,1,dummy_size,stdout);
memset(color_dummy,(sin((double)frame_num/233.3+123.0)+1)*127.5,dummy_size);
memset(color_dummy+366*640/4,(sin((double)frame_num/527.5+4785.5)+1)*127.5,640*110/4);
fwrite(color_dummy,1,dummy_size,stdout);

frame_num++;

}



int main(int argc, char **argv){

int samples_per_frame=SAMPLERATE/FPS;

int16_t *samples=malloc(samples_per_frame*2);
int q,w,e,v,ox,oy;

panorama=malloc(20000*480);


int panorama_size=0;
int panorama_pos=0;
uint8_t *tmp=malloc(5000*480*4);

MagickWandGenesis();
srand(time(NULL));
int iw,ih;
for(e=0;e<5;e++){
char filename[100];
sprintf(filename,"out/%d.jpg",rand()%55);
MagickWand *wand=NewMagickWand();
MagickReadImage(wand,filename);
iw=MagickGetImageWidth(wand);
ih=MagickGetImageHeight(wand);
MagickExportImagePixels(wand,0,0,iw,ih,"RGB",CharPixel,tmp);
DestroyMagickWand(wand);

for(w=0;w<ih;w++){
for(q=0;q<iw;q++){
v=(q+w*iw)*3;
panorama[q+panorama_pos+w*20000]=(tmp[v]+tmp[v+1]+tmp[v+2])/3;
}
}

panorama_pos+=iw;

fprintf(stderr,"Loading %s complete\n",filename);
}

panorama_size=panorama_pos;
panorama_pos=0;


srand48(time(NULL));

while(1){
fprintf(stderr,"reading sound frames...\n");
fread(samples,1,samples_per_frame*2,stdin);

for(w=HEIGHT-1;w>1;w--){
for(q=0;q<WIDTH;q++){
if(pixels_oscillo[w][q]){
ox=(int)((double)(q-320)/10.0)+q;
oy=(int)((double)(478-w)/25.0)+w+1;
if(ox<0){ox=0;}
if(ox>639){ox=639;}
if(oy<0){oy=0;}
if(oy<HEIGHT){
v=(pixels_oscillo[w][q]+pixels_oscillo[oy][ox])*10/12;
if(v>255){v=255;}
pixels_oscillo[oy][ox]=v;
}
pixels_oscillo[w][q]=0;
}
}
}

int max=0;
int64_t rms=0;
int ab;

for(q=0;q<samples_per_frame;q++){
pixels_oscillo[samples[q]*150/32768+240][q*640/samples_per_frame]=255;
ab=abs(samples[q]);
if(max<ab){max=ab;}
rms+=samples[q]*samples[q];
}
memmove(history,history+1,sizeof(HISTORY_ITEM)*639);
history[639].rrr=sqrt(rms/samples_per_frame);
history[639].max=max;

memcpy(pixels,pixels_oscillo,WIDTH*HEIGHT);
//memset(&pixels[370][0],0,WIDTH*100);

for(w=0;w<480;w++){
for(q=0;q<640;q++){
v=pixels[w][q]+panorama[panorama_pos+q+w*20000]/2;
if(v>255){v=255;}
pixels[w][q]=v;
}
}


for(q=0;q<640;q++){
for(w=history[q].max*50/32768;w>=0;w--){
pixels[420+w][q]=120;
pixels[420-w][q]=120;
}
for(w=history[q].rrr*50/32768;w>=0;w--){
pixels[420+w][q]=255;
pixels[420-w][q]=255;
}

}

panorama_pos+=3;
fprintf(stderr,"writing frame %d\n",frame_num);
flush_screen();

}



return EXIT_SUCCESS;
}









