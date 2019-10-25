#include "opencv2/objdetect.hpp"

#include "utils/CameraWindow.hpp"
#include "utils/ImageUtils.hpp"
#include "utils/ObjectDetector.hpp"

using namespace cv;
using namespace std;

// others
bool escPressed(){
        char c = cvWaitKey(0);
        return c == 27;// нажата ESC
}

//haar cascade
ObjectDetector objectDetector("data/haarcascades/haarcascade_frontalface_alt.xml");

//MAIN
int main(int argc, char* argv[]){
        IplImage* image;
        CvCapture* capture;
        CameraWindow cameraWindow(0.1);

        while(true){

                IplImage drawImage;
                image = cameraWindow.getImage();
                vector<Rect> objectRects = objectDetector.getObjects(image);

                for (Rect &rect : objectRects){
                        drawImage = ImageUtils::blurRectToImage(rect,cameraWindow.getImage());
                        image = &drawImage;
                }

                cameraWindow.drawImage(image);

                if (escPressed()){  break; }
        }

        return 0;
}