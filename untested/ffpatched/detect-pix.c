#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define WIDTH 960
#define HEIGHT 424

#define WIDTH 320
#define HEIGHT 180

#define BYTES (WIDTH*HEIGHT*3)

int main(){

uint8_t *src=malloc(BYTES);
int histogram[3][256];
int histogram2[3][256];
int histogram_prev[3][256];
uint8_t *thumb=malloc(32*32);
uint8_t *thumb_prev=malloc(32*32);
uint8_t *thumb_tmp=malloc(32*32);
uint8_t *thumb_view=malloc(32*32);

int graph[3][290];
int graph2[3][290];
memset(graph,0,sizeof(graph));
memset(graph2,0,sizeof(graph2));

int frame_num=0;

int q,w,e;
int pos;

int halfs[3];
int halfs_sums[3];



int diffmax=0,differr=0;

while(!feof(stdin)){
if(fread(src,1,BYTES,stdin)<0){break;}
memset(histogram,0,sizeof(histogram));
uint8_t *pix=src;
int halfs[3];
for(w=0;w<HEIGHT;w++){
for(q=0;q<WIDTH;q++){
for(e=0;e<3;e++){
histogram[e][*pix]++;
pix++;
}
}
}

int pixcount=WIDTH*HEIGHT;

// smooth
memcpy(histogram2,histogram,sizeof(histogram));
for(q=1;q<256;q++){
for(e=0;e<3;e++){
histogram[e][q]=(histogram2[e][q]+histogram2[e][q-1])/2;
}
}

// normalize upto 1000
for(q=0;q<256;q++){
for(e=0;e<3;e++){
histogram[e][q]=histogram[e][q]*3000/pixcount;
}
}

// can I use histograms at all?
int usable_color=0;
int low_values=0;
for(e=0;e<3;e++){
low_values=0;
for(q=0;q<256;q++){
if(histogram[e][q]>5){low_values++;}
//if(histogram[e][q]>80){usable_color=1;break;}
}
if(low_values>100){usable_color=1;}
}



/*
// find halfs
memset(halfs_sums,0,sizeof(halfs));
for(e=0;e<3;e++){
for(q=0;q<256;q++){
halfs_sums[e]+=histogram[e][q];
if(halfs_sums[e]>1500){halfs[e]=q;break;}
}
}
*/

// make treshold
int a,s,bw,bh;
int half3=(halfs[0]+halfs[1]+halfs[2])*0+128*3;
memset(thumb,0,32*32);
bh=HEIGHT/32;
for(s=0;s<32;s++){
//if(s*32+32>HEIGHT){bh=HEIGHT-s*32;}
bw=WIDTH/32;
for(a=0;a<32;a++){
//if(a*32+32>WIDTH){bw=WIDTH-a*32;}

for(w=0;w<bh;w++){
for(q=0;q<bw;q++){
pos=(a*bw+q+(w+s*bh)*WIDTH)*3;
if(pos>BYTES){continue;}
if(((int)src[pos]+(int)src[pos+1]+(int)src[pos+2])>half3){thumb[a+s*32]=255;goto nextblock;}
src[pos]=src[pos+1]=src[pos+2]=127;
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

if(diff_cool<mindiff){mindiff=diff_cool;min_x=a;min_y=a;memcpy(thumb_view,thumb_tmp,32*32);}
}
}


// draw thumbs
int pos2;
for(s=0;s<32;s++){
for(a=0;a<32;a++){
pos=(100+a+(s+5)*WIDTH)*3;
src[pos]=src[pos+1]=src[pos+2]=thumb[a+s*32];
pos-=50*3;
src[pos]=src[pos+1]=src[pos+2]=thumb_prev[a+s*32];
pos+=100*3;
pos2=(min_x+a+(min_y+s)*32);
if(pos2<0|| pos2>=32*32){continue;}
src[pos]=src[pos+1]=src[pos+2]=abs(thumb[a+s*32]-thumb_prev[pos2]);
pos+=50*3;
src[pos]=src[pos+1]=src[pos+2]=thumb_view[a+s*32];
}
}

if(usable_color){
// draw histogramm
for(e=0;e<3;e++){
for(q=0;q<256;q++){
for(w=0;w<histogram[e][q];w++){
pos=(q+10+(50-w+e*50+10)*WIDTH)*3+e;
if(pos<0 || pos>BYTES){continue;}
src[pos]=255;
}
}
}
}

int diffs[3];
memset(diffs,0,sizeof(diffs));
for(e=0;e<3;e++){
for(q=0;q<256;q++){
diffs[e]+=pow(histogram_prev[e][q]-histogram[e][q],2);
}
}

int diffmax=0;
for(e=0;e<3;e++){
diffs[e]/=2560;
if(diffmax<diffs[e]){diffmax=diffs[e];}
}


// draw bar at right
for(e=0;e<3;e++){
for(q=0;q<10;q++){
for(w=0;w<diffs[e];w++){
pos=(q+300+(50-w+e*50+10)*WIDTH)*3+e;
if(pos<0 || pos>BYTES){continue;}
src[pos]=255;
src[pos+1]=255;
src[pos+2]=255;
}
}
}

// push to graph
for(e=0;e<3;e++){
memmove(&graph[e][1],&graph[e][0],sizeof(graph[1])-sizeof(int));
graph[e][0]=diffs[e];
}

for(e=0;e<3;e++){
memmove(&graph2[e][1],&graph2[e][0],sizeof(graph[1])-sizeof(int));
}

graph[0][0]=mindiff/20;
graph[1][0]=min_x;
graph[2][0]=min_y;

if(diffmax-differr*2>10){
// yes, new scene

graph2[0][0]=100;

} else {
differr=diffmax;
graph2[0][0]=0;
}

graph2[1][0]=diffmax;
graph2[2][0]=differr;

// draw graph
int prev_val=0,current_val=0;
for(e=0;e<3;e++){
for(q=0;q<sizeof(graph[1])/sizeof(int);q++){
prev_val=q>0?graph[e][q-1]:graph[e][q];
current_val=graph[e][q];
if(prev_val>current_val){
prev_val^=current_val;
current_val^=prev_val;
prev_val^=current_val;
}
for(w=prev_val;w<=current_val;w++){
pos=(295-q+(50-w+e*50+10)*WIDTH)*3;
if(pos<0 || pos>BYTES){continue;}
src[pos]=255;
src[pos+1]=255;
src[pos+2]=255;
}
}
}



// draw graph
//int prev_val=0,current_val=0;
for(e=0;e<3;e++){
for(q=0;q<sizeof(graph[1])/sizeof(int);q++){
prev_val=q>0?graph2[e][q-1]:graph2[e][q];
current_val=graph2[e][q];
if(prev_val>current_val){
prev_val^=current_val;
current_val^=prev_val;
prev_val^=current_val;
}
for(w=prev_val;w<=current_val;w++){
pos=(298-q+(53-w+e*50+10)*WIDTH)*3+e;
if(pos<0 || pos>BYTES){continue;}
src[pos]=255;
}
}
}


fprintf(stderr,"%d: difference: RGB: %d,%d,%d; scene: %d, signal: %d, err: %d\n",frame_num++,
diffs[0],diffs[1],diffs[2],
graph2[0][0],graph2[1][0],graph2[2][0]
);

for(q=0;q<256;q++){
fprintf(stderr,"%d,",histogram[1][q]);
}
fprintf(stderr,"\n");

memcpy(histogram_prev,histogram,sizeof(histogram));
memcpy(thumb_prev,thumb,32*32);

fwrite(src,1,BYTES,stdout);
}



return 0;
}

