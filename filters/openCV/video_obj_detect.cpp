#include "opencv2/objdetect.hpp"
#include "utils/ImageUtils.hpp"

using namespace cv;
using namespace std;

//haar cascade
CascadeClassifier face_cascade;
void loadCascade(){
		face_cascade.load( "data/haarcascades/haarcascade_frontalface_alt.xml" );
}
vector<Rect> getObjects(IplImage* image){
		vector<Rect> objects;
		Mat frame_gray = ImageUtils::getGrayScaledFrame(image,false);
		face_cascade.detectMultiScale( frame_gray, objects);
		return objects;
}

int main(){
	int
	width = 256,
	heigh = 192,
	pixelsPerFrame = width * heigh,
	RGBframeBufferSize = pixelsPerFrame * 3;

	// allocate variables and buffer
	uint8_t pixels[RGBframeBufferSize];//frame buffer
	loadCascade();

	while(fread(&pixels, sizeof(pixels), 1, stdin)){
		Mat mat(heigh, width, CV_8UC3, &pixels);
		IplImage image(mat);

		vector<Rect> objectRects = getObjects(&image);
		for (auto rect : objectRects){
			ImageUtils::fillRectInImage(rect,&image);
		}


		fwrite(image.imageData, sizeof(pixels), 1, stdout);
	}

}