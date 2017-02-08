











#include <stdio.h>
#include <stdint.h>
#include <windows.h>


BITMAPINFO	bmInfo;
uint8_t *pixels;
HBITMAP myBitmap;
HDC mdc;
FILE *out;

int pos=0;



void processPixels(uint8_t *pixels, int width, int height, int charcode){
int q,w,len,pos;

// step 1 - here is half-tone?
len=width*height*4;
for(w=0;w<len;w++){
if(pixels[w]>0 && pixels[w]<255){
return;
}
}

// step 2 - calc real bounding box

int min_x=0xFFFF,min_y=0xFFFF,max_x=0,max_y=0;

for(w=0;w<height;w++){
for(q=0;q<width;q++){
pos=(q+w*width)*4;
if(pixels[pos]){
if(q<min_x){min_x=q;}
if(w<min_y){min_y=w;}
if(q>max_x){max_x=q;}
if(w>max_y){max_y=w;}
}
}
}

// step 3 - store ???
if(min_x<0xFFFF){
fwrite(&charcode,1,4,out);
fwrite(&min_x,1,4,out);
fwrite(&max_x,1,4,out);
fwrite(&min_y,1,4,out);
fwrite(&max_y,1,4,out);
fwrite(pixels,1,4*50*50,out);
}

}



BOOL CALLBACK EnumFamCallBack(LPLOGFONT lplf, LPNEWTEXTMETRIC lpntm, DWORD FontType, LPVOID aFontCount) { 
 
    // Record the number of raster, TrueType, and vector  
    // fonts in the font-count array.  



printf("Got font %s ",lplf->lfFaceName);
 
    if (FontType & RASTER_FONTTYPE) 
        printf("is raster"); 
    else if (FontType & TRUETYPE_FONTTYPE) 
        printf("is truetype"); 
    else 
        printf("is hueta"); 
printf("\n");

int widths[256];
if(FontType & RASTER_FONTTYPE){
HDC hdc=mdc;
HDC ddc=GetDC(0);

RECT r;
r.top=0;
r.left=0;
r.right=50;
r.bottom=50;
HBRUSH brush=GetStockObject(BLACK_BRUSH);
FillRect(hdc,&r,brush);
SetBkColor(hdc,0);
SetTextColor(hdc,0xFFFFFF);
SetTextColor(ddc,0xFF);
int q;
for(q=1;q<12;q++){
lplf->lfHeight=-q;
lplf->lfCharSet=RUSSIAN_CHARSET;
lplf->lfQuality=PROOF_QUALITY;
HFONT font=CreateFontIndirect(lplf);

GetCharWidth32(hdc,0,127,widths);

HFONT prev=SelectObject(hdc,font);
SelectObject(ddc,font);
if(prev==NULL){printf("ALARM! Null !\n");}

int w;
printf("for %s size %d\n",lplf->lfFaceName,q);
for(w=0;w<127;w++){
printf("%d=%d\n",w,widths[w]);
FillRect(hdc,&r,brush);

TextOut(mdc,10,10,&w, 1);
TextOut(ddc,10+w*10,150+q*10,&w, 1);

GetDIBits(mdc, myBitmap, 0, 50, pixels, &bmInfo, DIB_RGB_COLORS );

processPixels(pixels,50,50,w);
}

SelectObject(hdc,prev);

}
pos+=10;
}
 
    UNREFERENCED_PARAMETER( lplf ); 
    UNREFERENCED_PARAMETER( lpntm ); 
return(1);
} 




int main(int argc, char **argv){

		ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
			bmInfo.bmiHeader.biBitCount = 32;
			bmInfo.bmiHeader.biWidth = 50;
			bmInfo.bmiHeader.biHeight = -50;
			bmInfo.bmiHeader.biPlanes = 1;
			bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmInfo.bmiHeader.biCompression = BI_RGB;

HDC hdc=GetDC(0);


    UINT uAlignPrev; 
    char szCount[8];
        HRESULT hr;
        size_t * pcch; 
out=fopen("out.bin","wb");
pixels = malloc(50*50*4);
			myBitmap = CreateCompatibleBitmap(hdc,50,50);
		mdc = CreateCompatibleDC(hdc);
			SelectObject(mdc, myBitmap);

 
    EnumFontFamilies(GetDC(NULL), (LPCTSTR) NULL, 
        (FONTENUMPROC) EnumFamCallBack, NULL); 

fclose(out);
/* 
    uAlignPrev = SetTextAlign(hdc, TA_UPDATECP); 
 
    MoveToEx(hdc, 10, 50, (LPPOINT)NULL); 
    TextOut(hdc, 0, 0, "Number of raster fonts: ", 24); 
    itoa(aFontCount[0], szCount, 10); 
        
        hr = StringCchLength(szCount, 9, pcch);
        if (FAILED(hr))
        {
        // TODO: write error handler 
        }
    TextOut(hdc, 0, 0, szCount, *pcch); 
 
    MoveToEx(hdc, 10, 75, (LPPOINT)NULL); 
    TextOut(hdc, 0, 0, "Number of vector fonts: ", 24); 
    itoa(aFontCount[1], szCount, 10);
        hr = StringCchLength(szCount, 9, pcch);
        if (FAILED(hr))
        {
        // TODO: write error handler 
        } 
    TextOut(hdc, 0, 0, szCount, *pcch); 
 
    MoveToEx(hdc, 10, 100, (LPPOINT)NULL); 
    TextOut(hdc, 0, 0, "Number of TrueType fonts: ", 26); 
    itoa(aFontCount[2], szCount, 10);
        hr = StringCchLength(szCount, 9, pcch);
        if (FAILED(hr))
        {
        // TODO: write error handler 
        }
    TextOut(hdc, 0, 0, szCount, *pcch); 
 
    SetTextAlign(hdc, uAlignPrev); 
  */


return 0;
}



