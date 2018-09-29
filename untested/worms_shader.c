





#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

static uint32_t pixels[480][640];
static uint8_t mask_current[480][640];
static uint32_t mask_target[480][640];
static uint32_t texture[480][640];
static uint8_t pixels2[640*480*3];

//#define setPixel(x,y,color) pixels[(int)(y)][(int)(x)]=color

void setPixel(int x, int y, uint32_t color){
if(x<0||y<0||x>=640||y>=480){return;}
pixels[y][x]=color;
}

#define WORMCOUNT 1000

typedef struct{
float x;
float y;
float angle;
uint32_t color;
} PIX;

FILE *out;

void flip_frame(){
//return;
uint8_t *p=pixels2;
int q,w;
for(w=0;w<480;w++){
for(q=0;q<640;q++){
*p++=pixels[w][q];
*p++=pixels[w][q]>>8;
*p++=pixels[w][q]>>16;
}
}
fwrite(pixels2,640*480*3,1,out);
fflush(stdout);
usleep(20000);
}


float random_range[WORMCOUNT*2][10][4];

void random_init(){
int q,w;
for(w=0;w<WORMCOUNT*2;w++){
for(q=0;q<10;q++){
random_range[w][q][0]=drand48();
random_range[w][q][1]=drand48();
random_range[w][q][2]=0;
random_range[w][q][3]=w<WORMCOUNT?drand48()*10.0+3:1000000.0;
}
}
}

float getRandomNoise(int id, float p){
int q;
float sum=0;
for(q=0;q<10;q++){
random_range[id][q][2]+=((float)(1<<q)/random_range[id][q][3]);
if(random_range[id][q][2]>1.0){
random_range[id][q][0]=random_range[id][q][1];
random_range[id][q][1]=drand48();
random_range[id][q][2]-=1.0;
}
//fprintf(stderr,"debuh %f\n",random_range[q][2]);
p=.5+cos(random_range[id][q][2]*M_PI)/2;
sum+=(random_range[id][q][0]*p+random_range[id][q][1]*(1-p))/(2<<q);
}
// this must never happen, but need
if(sum<0){sum=0;}
if(sum>=1.0){sum=0.99999;}
return(sum);
}

PIX history[WORMCOUNT][640];


int main(int argc, char **argv){
srand48(time(NULL));
random_init();

out=fopen("video","wb");

FILE *in=fopen("mask","rb");
if(!in){fprintf(stderr,"Can't open file \"mask\", dyatel!\n");abort();}
fread(mask_target,1,640*480*4,in);
fclose(in);


in=fopen("texture","rb");
if(!in){fprintf(stderr,"Can't open file \"texture\", dyatel!\n");abort();}
fread(texture,1,640*480*4,in);
fclose(in);


memset(mask_current,0xFF,640*480);



int q,w;


for(w=0;w<480;w++){
for(q=0;q<640;q++){
mask_target[w][q]&=0xFF;
}
}


for(w=0;w<WORMCOUNT;w++){
for(q=0;q<640;q++){
history[w][q].x=320;
history[w][q].y=240;
history[w][q].color=getRandomNoise(w+WORMCOUNT,1)*0xFFFFFF;
history[w][q].angle=drand48()*2*M_PI;
}
}


static uint8_t mask_tmp[480][640];

while(1){

//memset(pixels,0,640*480*4);
//memcpy(pixels,mask_current,640*480*4);
float m;
int a,d,tb,tt,tm,r,g,b;
for(w=1;w<480;w++){
for(q=0;q<640;q++){
m=mask_current[w][q]/255.0;

tt=mask_tmp[w-1][q];
tm=tb=mask_current[w][q];
d=tt-tb;

a=-d*1.2-(255-tb)/5.0-3;
if(tt>tm){
tm=tt;
if(tm<0){tm=0;}
}


r=((texture[w][q]>>16)&0xFF);
g=((texture[w][q]>>8)&0xFF);
b=((texture[w][q]>>0)&0xFF);

r+=a;
g+=a;
b+=a;

if(r<0){r=0;}
if(r>255){r=255;}
if(g<0){g=0;}
if(g>255){g=255;}
if(b<0){b=0;}
if(b>255){b=255;}

mask_tmp[w][q]=tm;

pixels[w][q]=(r)|(g<<8)|(b<<16);
}
}


for(w=0;w<WORMCOUNT;w++){
for(q=615;q<640;q++){
setPixel(history[w][q].x,history[w][q].y,history[w][639].color);
}
}

flip_frame();

float theta;
int t;
for(w=0;w<WORMCOUNT;w++){
for(q=0;q<5;q++){
theta=getRandomNoise(w,q/640.0)*2*M_PI+history[w][639].angle;
memmove(history[w],history[w]+1,(640-1)*sizeof(PIX));
history[w][639].x=history[w][638].x+sin(theta);
history[w][639].y=history[w][638].y+cos(theta);
history[w][639].angle=history[w][638].angle;
if(history[w][639].x<0){history[w][639].x=0;history[w][639].angle=drand48()*2*M_PI;}
if(history[w][639].x>639){history[w][639].x=639;history[w][639].angle=drand48()*2*M_PI;}
if(history[w][639].y<0){history[w][639].y=0;history[w][639].angle=drand48()*2*M_PI;}
if(history[w][639].y>480){history[w][639].y=480;history[w][639].angle=drand48()*2*M_PI;}
history[w][639].color=getRandomNoise(w+WORMCOUNT,1)*0xFFFFFF;
int ox,oy;
ox=(int)(history[w][639].x);
oy=(int)(history[w][639].y);

t=(int)mask_current[oy][ox]-5;
//fprintf(stderr,"%d\n",t);
if(t<(int)mask_target[oy][ox]){t=mask_target[oy][ox];}

mask_current[oy][ox]=t;
}
}
}

return(EXIT_SUCCESS);
}








