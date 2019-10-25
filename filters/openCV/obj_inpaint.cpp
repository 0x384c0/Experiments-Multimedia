#include "opencv2/objdetect.hpp"

#include "utils/CameraWindow.hpp"
#include "utils/ImageUtils.hpp"
#include "utils/ObjectDetector.hpp"

using namespace cv;
using namespace std;

// others
bool inpaint_telea = true;
bool escPressed(){
        char c = cvWaitKey(0);

        if (c == 120){
                inpaint_telea = !inpaint_telea;
        }

        return c == 27;// нажата ESC
}

//haar cascade
ObjectDetector objectDetector("data/haarcascades/haarcascade_frontalface_alt.xml");

//MAIN
int main(int argc, char* argv[]){
        IplImage* image;
        CvCapture* capture;
        CameraWindow cameraWindow(0.5);

        printf("Press esc to exit, x - to switch inpaint method between INPAINT_TELEA and INPAINT_SHIFTMAP, any button to update image");

        while(true){
                image = cameraWindow.getImage();
                vector<Rect> objectRects = objectDetector.getObjects(image);
                IplImage drawImage;

                if (inpaint_telea){
                        for (Rect &rect : objectRects){
                                drawImage = ImageUtils::inpaintRectToImage(rect,image);
                                image = &drawImage;
                                break;
                        }
                } else {
                        for (Rect &rect : objectRects){
                                drawImage = ImageUtils::xphotoInpaintRectToImage(rect,image);
                                image = &drawImage;
                                break;
                        }
                }

                cameraWindow.drawImage(image);
                if (escPressed()){  break; }
        }

        return 0;
}