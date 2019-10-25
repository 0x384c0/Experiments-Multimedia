#include <stdlib.h>
#include <stdio.h>

#include "utils/CameraWindow.hpp"

IplImage* image = 0;
IplImage* src = 0;

int main(int argc, char* argv[])
{
        

        CameraWindow cameraWindow;
        // получаем картинку
        image = cameraWindow.getImage();

        // задаём точку для вывода текста
        CvPoint pt = cvPoint( image->height/4, image->width/2 );
        // инициализация шрифта
        CvFont font;
        cvInitFont( &font, CV_FONT_HERSHEY_COMPLEX,1.0, 1.0, 0, 1, CV_AA);
        // используя шрифт выводим на картинку текст
        cvPutText(image, "Hello world", pt, &font, CV_RGB(200, 200, 200) );


        // показываем картинку
        cameraWindow.drawImage(image);

        // выводим в консоль информацию о картинке
        printf( "[i] channels:  %d\n",        image->nChannels );
        printf( "[i] pixel depth: %d bits\n",   image->depth );
        printf( "[i] width:       %d pixels\n", image->width );
        printf( "[i] height:      %d pixels\n", image->height );
        printf( "[i] image size:  %d bytes\n",  image->imageSize );
        printf( "[i] width step:  %d bytes\n",  image->widthStep );

        // ждём нажатия клавиши
        cvWaitKey(0);
        return 0;
}