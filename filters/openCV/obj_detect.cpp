#include "opencv2/objdetect.hpp"

#include "utils/CameraWindow.hpp"
#include "utils/ImageUtils.hpp"
#include "utils/ObjectDetector.hpp"

using namespace cv;
using namespace std;

// others
bool escPressed(){
        printf("Press eny key to update image or Esc to exit\n");
        char c = cvWaitKey(0);
        return c == 27;// нажата ESC
}

//haar cascade
ObjectDetector objectDetector("data/haarcascades/haarcascade_frontalface_alt.xml");

//MAIN
int main(int argc, char* argv[]){
        IplImage* image;
        CvCapture* capture;
        CameraWindow cameraWindow;


        while(true){

                image = cameraWindow.getImage();
                vector<Rect> objectRects = objectDetector.getObjects(image);

                for (Rect &rect : objectRects){
                        ImageUtils::fillRectInImage(rect,image);
                }

                cameraWindow.drawImage(image);

                if (escPressed()){  break; }
        }

        return 0;
}