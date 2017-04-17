#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


uint8_t *thumb=NULL;
uint8_t *thumb_prev=NULL;
uint8_t *thumb_tmp=NULL;


void detector_init(){
thumb=malloc(32*32);
thumb_prev=malloc(32*32);
thumb_tmp=malloc(32*32);
}



int detector_frame(uint8_t *src, int width, int height);
int detector_frame(uint8_t *src, int width, int height){

int q,w,e;
int pos;

int halfs[3];
int halfs_sums[3];



int diffmax=0,differr=0;

if(thumb==NULL){detector_init();}
int bytes=width*height;
// make treshold
int a,s,bw,bh;
int half3=128;
memset(thumb,0,32*32);
bh=height/32;
for(s=0;s<32;s++){
bw=width/32;
for(a=0;a<32;a++){
for(w=0;w<bh;w++){
for(q=0;q<bw;q++){
pos=(a*bw+q+(w+s*bh)*width);
if(pos>bytes){continue;}
if(src[pos]>half3){thumb[a+s*32]=255;goto nextblock;}
}
}
nextblock:;
}
}

// find difference and offsets for global motion compensation
int radius=5;
int mindiff=0xFFFFFFF;
int diff_len,diff_cool;
int min_x,min_y;
int diff;
for(s=-radius;s<=radius;s++){
for(a=-radius;a<=radius;a++){
// draw difference to tmp buffer
memset(thumb_tmp,0,32*32);
for(w=0;w<32;w++){
for(q=0;q<32;q++){
pos=(a+q+(w+s)*32);
if(pos<0 || pos>=32*32 || a+q<0 || w+s<0 || a+q>=32 || w+s>=32){continue;}
if(thumb_prev[pos]!=thumb[q+w*32]){thumb_tmp[q+w*32]=255;}
}
}
// filter this buffer
for(w=0;w<32;w++){
for(q=1;q<31;q++){
pos=q+w*32;
if(thumb_tmp[pos-1]==0 && thumb_tmp[pos+1]==0){thumb_tmp[pos]=0;}
}
}
for(w=0;w<32;w++){
for(q=1;q<31;q++){
pos=q*32+w;
if(thumb_tmp[pos-32]==0 && thumb_tmp[pos+32]==0){thumb_tmp[pos]=0;}
}
}
// sum pixels
diff=0;
diff_cool=0;
diff_len=0;
for(w=0;w<32;w++){
for(q=0;q<32;q++){
pos=q+w*32;
if(thumb_tmp[pos]){diff++;diff_len++;} else {diff_cool+=pow(diff_len,2);diff_len=0;}
}
diff_cool+=pow(diff_len,2);diff_len=0;
}

for(q=0;q<32;q++){
for(w=0;w<32;w++){
pos=q+w*32;
if(thumb_tmp[pos]){diff++;diff_len++;} else {diff_cool+=pow(diff_len,2);diff_len=0;}
}
diff_cool+=pow(diff_len,2);diff_len=0;
}

if(diff_cool<mindiff){mindiff=diff_cool;min_x=a;min_y=a;}
}
}

memcpy(thumb_prev,thumb,32*32);

return(mindiff);
}

