










#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <capstone/capstone.h>

#define nullptr NULL

char *filename="assets/capstone.dll";
int position=0;
int fontsize=10;
double zoom=1;

int selection_start=3;
int selection_end=500;
int selection_active=0;
int selection_begin=0;
int cursor=10;
int cursor_byte=0;

int LE_Shirshina=40;

int active_panel=0;

SDL_Surface *glyph_cache[256];
SDL_Texture *glyphs[256];




  csh handle;
  cs_insn *x86d_insn;
  size_t x86d_count;
 


typedef struct{

double x;
double y;
double ox;
double oy;
double size;

}SPARKLE;

SPARKLE sparkles[128];
int sparkles_pos=0;

// draw string on screen
int draw_string(SDL_Renderer *ren, char *str, int size, int x, int y){
int q;

SDL_Rect Message_rect; //create a rect
Message_rect.x = x;  //controls the rect's x coordinate 
Message_rect.y = y; // controls the rect's y coordinte
Message_rect.w = size; // controls the width of the rect
Message_rect.h = size*2; // controls the height of the rect


for(q=0;q<strlen(str);q++){

SDL_RenderCopy(ren, glyphs[str[q]], NULL, &Message_rect);
Message_rect.x+=size;

}
return(1);
}


Uint32 my_callbackfunc(Uint32 interval, void *param)
{
    SDL_Event event;
    SDL_UserEvent userevent;

    /* In this example, our callback pushes an SDL_USEREVENT event
    into the queue, and causes our callback to be called again at the
    same interval: */

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return(interval);
}




int main(int argc, char **argv)
{
if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
    return 4;
}

//open a window
SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 300, 800, 480, SDL_WINDOW_SHOWN);
if (win == nullptr){
    return 3;
}

//renderer
SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0 | SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
if (ren == nullptr){
    return 2;
}

//load bitmap
SDL_Surface *bmp = IMG_Load("assets/helloworld.jpg");
if (bmp == nullptr){
    return 6;
}

SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
if (tex == nullptr){
    return 5;
}

SDL_FreeSurface(bmp);

if (TTF_Init())
    printf("%s\n", TTF_GetError());

SDL_Rect Message_rect; //create a rect
Message_rect.x = 0;  //controls the rect's x coordinate 
Message_rect.y = 0; // controls the rect's y coordinte
Message_rect.w = 100; // controls the width of the rect
Message_rect.h = 100; // controls the height of the rect

SDL_Rect rect; //create a rect
Message_rect.x = 0;  //controls the rect's x coordinate 
Message_rect.y = 100; // controls the rect's y coordinte
Message_rect.w = 800; // controls the width of the rect
Message_rect.h = 480; // controls the height of the rect


TTF_Font* Sans = TTF_OpenFont("assets/sans.ttf", 24); //this opens a font style and sets a size

if(!Sans){printf("Fuck! Here no SANS!!!!!!!!!\n");}

SDL_Color White = {10, 255, 255};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

SDL_Texture *atlas_texture=SDL_CreateTexture(ren,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING|SDL_TEXTUREACCESS_TARGET,2560,20);
if(atlas_texture){printf("atlas done\n");}
SDL_Texture *window_texture=SDL_GetRenderTarget(ren);
SDL_SetRenderTarget(ren,atlas_texture);

// Font pre-rendering
int ch;
for(ch=0; ch<256; ++ch){
printf("rendering %d\n",ch);
    glyph_cache[ch]=TTF_RenderGlyph_Solid(Sans,ch,White);
glyphs[ch]=SDL_CreateTextureFromSurface(ren, glyph_cache[ch]);
Message_rect.x=ch*10;
Message_rect.y=10;
Message_rect.w=10;
Message_rect.h=20;
SDL_RenderCopy(ren, glyphs[ch], NULL, &Message_rect);
}

SDL_SetRenderTarget(ren,window_texture);

int atlas_pitch;
uint8_t *atlas;
SDL_LockTexture(atlas_texture,
                    NULL,
                    (void **)&atlas,
                    &atlas_pitch);


SDL_Texture *bukovki=SDL_CreateTexture(ren,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,800,480);
uint8_t *pixels;


SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, filename, White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
if(!surfaceMessage){printf("Fuck! Here no surface!!!!!!!!!\n");}

SDL_Texture* Message = SDL_CreateTextureFromSurface(ren, surfaceMessage); //now you can convert it into a texture
if(!Message){printf("Fuck! Here no texture!!!!!!!!!\n");}


//Mind you that (0,0) is on the top left of the window/screen, think a rect as the text's box, that way it would be very simple to understance

//Now since it's a texture, you have to put RenderCopy in your game loop area, the area where the whole code executes


int ret,q;
FILE *ffile=fopen(filename,"rb");
if(!ffile){
fprintf(stderr,"Can't open %s\n",filename);
}

unsigned char data[4096];

ret=fread(data,1,4096,ffile);
fprintf(stderr,"Read %d bytes done\n",ret);

  if (cs_open(CS_ARCH_X86, CS_MODE_16, &handle) != CS_ERR_OK){
      return -1;
  }
  x86d_count = cs_disasm(handle, data, ret, 0, 0, &x86d_insn);
fprintf(stderr,"decoded %d instructions\n",x86d_count);

unsigned char str[4096];
int line;
        SDL_Event e;


memset(sparkles,0,sizeof(sparkles));




Uint32 delay = (33 / 10) * 20;  /* To round it down to the nearest 10 ms */
SDL_TimerID my_timer_id = SDL_AddTimer(delay, my_callbackfunc, 0);




 while (1) {
// SDL_PumpEvents();

        if (SDL_WaitEvent(&e)) {
            if (e.type == SDL_QUIT) {
                break;
            }

if(e.type==SDL_MOUSEWHEEL){
position-=e.wheel.y*LE_Shirshina;
if(position<0){position=0;}
}

if(e.type==SDL_MOUSEBUTTONDOWN){
int pos_x=e.button.x;
int pos_y=e.button.y;
if(pos_y>=100 && pos_x>=fontsize*10){
if(pos_x<fontsize*(LE_Shirshina*3+10)){
selection_begin=selection_start=selection_end=cursor=position+(pos_y-100)/fontsize/2*LE_Shirshina+(pos_x-fontsize*10)/fontsize/3;
selection_active=1;
}
if(pos_x>fontsize*(LE_Shirshina*3+10)){
selection_begin=selection_start=selection_end=cursor=position+(pos_y-100)/fontsize/2*LE_Shirshina+(pos_x-fontsize*(10+LE_Shirshina*3)-20)/fontsize;
selection_active=1;
}
}
}

if(e.type==SDL_MOUSEBUTTONUP){
selection_active=0;
}


if(e.type==SDL_MOUSEMOTION){
int pos_x=e.motion.x;
int pos_y=e.motion.y;

sparkles[sparkles_pos].x=pos_x;
sparkles[sparkles_pos].y=pos_y;
sparkles[sparkles_pos].ox=((double)rand()/RAND_MAX*10)-5;
sparkles[sparkles_pos].oy=((double)rand()/RAND_MAX*10)-5;
sparkles[sparkles_pos].size=(int)((double)rand()/RAND_MAX*10)+3;
sparkles_pos++;
if(sparkles_pos>=sizeof(sparkles)/sizeof(*sparkles)){
sparkles_pos=0;
}

if(selection_active==1){
if(pos_x>fontsize*(LE_Shirshina*3+10)){
cursor=position+(pos_y-100)/fontsize/2*LE_Shirshina+(pos_x-fontsize*(10+LE_Shirshina*3)-20)/fontsize;
}
if(pos_x<fontsize*(LE_Shirshina*3+10)){
cursor=position+(pos_y-100)/fontsize/2*LE_Shirshina+(pos_x-fontsize*10)/fontsize/3;
}

if(cursor<selection_begin){
selection_end=selection_begin+1;
selection_start=cursor;
} else {
selection_start=selection_begin;
selection_end=cursor;

}
}
}


if(e.type==SDL_KEYDOWN){

switch(e.key.keysym.scancode){

case SDL_SCANCODE_KP_DIVIDE:
LE_Shirshina--;
if(LE_Shirshina<1){LE_Shirshina=1;}
break;

case SDL_SCANCODE_KP_MULTIPLY:
LE_Shirshina++;
break;

case SDL_SCANCODE_HOME:
position=0;
break;

case SDL_SCANCODE_PAGEUP:
position-=(480-100)/fontsize*LE_Shirshina;
if(position<0){position=0;}
break;

case SDL_SCANCODE_PAGEDOWN:
position+=(480-100)/fontsize*LE_Shirshina;
break;

case SDL_SCANCODE_UP:
position-=LE_Shirshina;
if(position<0){position=0;}
break;

case SDL_SCANCODE_DOWN:
position+=LE_Shirshina;
break;

case SDL_SCANCODE_TAB:
active_panel=(active_panel+1)%3;
break;


case SDL_SCANCODE_KP_MINUS:
zoom/=1.1;
break;

case SDL_SCANCODE_KP_PLUS:
zoom*=1.1;
break;

default:
break;
}
fontsize=(int)((double)10.0*zoom);

        }

if(e.type!=SDL_USEREVENT){continue;}


}


SDL_RenderClear(ren);
SDL_RenderCopy(ren, tex, NULL, NULL);
SDL_RenderCopy(ren, Message, NULL, &Message_rect); //you put the renderer's name first, the Message, the crop size(you can ignore this if you don't want to dabble with cropping), and the rect which is the size and coordinate of your texture

// draw sparkles



SDL_SetRenderDrawColor(ren,255,255,255,255);

for(q=0;q<sizeof(sparkles)/sizeof(*sparkles);q++){
if(sparkles[q].size>0){
Message_rect.x=sparkles[q].x;
Message_rect.y=sparkles[q].y;
Message_rect.w=Message_rect.h=sparkles[q].size;
SDL_RenderFillRect(ren,&Message_rect);
sparkles[q].x+=sparkles[q].ox;
sparkles[q].y+=sparkles[q].oy;
sparkles[q].size-=.5;
}
}



/* Here draw bytes */


fseek(ffile,position,0);
line=0;

sprintf(str,"position at %x (%d)",position,position);
draw_string(ren, str, 10, 0, 80);

int draw_y;
int i;
int x86d_pos=0;

char forasm[4096];

int screen_size=(480-100)/fontsize*2*LE_Shirshina;
int position_end=position+(480-100)/fontsize*2;

/*if(11 || selection_start<position_end && selection_end>=position){

// top line
int screen_start=selection_start-position;
if(screen_start<0){screen_start=0;}
int screen_end=selection_end-position_end;
if(screen_end>screen_size){screen_end=screen_size;}

int sel_y=screen_start/LE_Shirshina;
int sel_x=screen_start%LE_Shirshina;

Message_rect.x = (sel_x*3+10)*fontsize;
Message_rect.y = sel_y*fontsize*2+100;
Message_rect.w = (LE_Shirshina-sel_x+1)*fontsize*3;
Message_rect.h=fontsize*2;
SDL_SetRenderDrawColor(ren,0,200,50,128);
SDL_RenderFillRect(ren,&Message_rect);


// body

// bottom line



/*
Message_rect.x = q*fontsize*3+10*fontsize;  //controls the rect's x coordinate 
Message_rect.w = fontsize*3;

SDL_RenderFillRect(ren,&Message_rect);

Message_rect.w = fontsize;
Message_rect.x=(10+LE_Shirshina*3)*fontsize+20+q*fontsize;
SDL_RenderFillRect(ren,&Message_rect);
*

}

*/
/*
int pitch;
SDL_LockTexture(bukovki,
                    NULL,
                    (void **)&pixels,
                    &pitch);


while(1){
memset(str,0,4096);
memset(forasm,0,4096);

sprintf(str,"%.8x: ",position+line*LE_Shirshina);

draw_y=100+line*2*fontsize;
//draw_string(ren, str, fontsize, 0, draw_y);



	ret=fread(data,1,LE_Shirshina,ffile);
	if(ret<0){break;}
	for(q=0;q<ret;q++){
int byte_pos=position+line*LE_Shirshina+q;

sprintf(str,"%.2x",data[q]);

Message_rect.y = draw_y; // controls the rect's y coordinte
Message_rect.w = fontsize; // controls the width of the rect
Message_rect.h = fontsize*2; // controls the height of the rect




if(byte_pos==cursor){
Message_rect.x = q*fontsize*3+10*fontsize;  //controls the rect's x coordinate 
Message_rect.w = fontsize*3;
SDL_SetRenderDrawColor(ren,250,50,20,255);
SDL_RenderFillRect(ren,&Message_rect);

Message_rect.w = fontsize;
Message_rect.x=(10+LE_Shirshina*3)*fontsize+20+q*fontsize;
SDL_RenderFillRect(ren,&Message_rect);

}

Message_rect.x = q*fontsize*3+10*fontsize;  //controls the rect's x coordinate 



for(i=0;i<2;i++){

/*
{
int f,g;
for(g=0;g<20;g++){
for(f=0;f<40;f++){
if(Message_rect.y+g>=480){continue;}
pixels[Message_rect.x*4+f+(Message_rect.y+g)*pitch]=atlas[str[i]*10*4+f+g*atlas_pitch];
}
}

}
/
//SDL_RenderCopy(ren, glyphs[str[i]], NULL, &Message_rect);
Message_rect.x+=fontsize;
}


Message_rect.x=(10+LE_Shirshina*3)*fontsize+20+q*fontsize;
//SDL_RenderCopy(ren, glyphs[data[q]], NULL, &Message_rect);


// draw QUADRATICK
/*
Message_rect.x=(10+LE_Shirshina+LE_Shirshina*3)*fontsize+20+20+q*fontsize*2;
Message_rect.w=fontsize*2;
SDL_SetRenderDrawColor(ren,data[q],data[q],data[q],255);
SDL_RenderFillRect(ren,&Message_rect);
*/

// disasm
//printf("ins: %d/%d %d\n",x86d_pos,x86d_count,(int)x86d_insn[x86d_pos].address);
/*
for(;x86d_insn[x86d_pos].address<byte_pos;x86d_pos++);

  if (x86d_count>0 && x86d_pos<x86d_count && x86d_insn[x86d_pos].address==byte_pos){
          sprintf(forasm+strlen(forasm),"%s %s; ", x86d_insn[x86d_pos].mnemonic, x86d_insn[x86d_pos].op_str);
x86d_pos++;
  }
/


}
 // out disasm to screen
/*
Message_rect.x=(10+LE_Shirshina+LE_Shirshina*3)*fontsize+20+20;
draw_string(ren, forasm, fontsize, Message_rect.x, draw_y);
/

	line++;
	if(line*fontsize*2+100>480){break;}

}

*/

/*
memcpy(pixels,atlas,2560*20*4);

SDL_UnlockTexture(bukovki);
*/
//SDL_RenderCopy(ren, atlas_texture, NULL, NULL);



//printf("Present!\n");

SDL_RenderPresent(ren);
//usleep(100000);

    }
/*
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);

    SDL_Quit();

*/


return 0;
}



/*



int main(void)
11 {
19  if (count > 0) {
20      size_t j;
21      for (j = 0; j < count; j++) {
22          printf("0x%"PRIx64":\t%s\t\t%s\n", insn[j].address, insn[j].mnemonic,
23                  insn[j].op_str);
24      }
25 
26      cs_free(insn, count);
27  } else
28      printf("ERROR: Failed to disassemble given code!\n");
29 
30  cs_close(&handle);
31 
32     return 0;
33 }

*/



