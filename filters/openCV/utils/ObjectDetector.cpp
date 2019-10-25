#include "ObjectDetector.hpp"
#include "ImageUtils.hpp"

ObjectDetector::ObjectDetector(const String& filename){
	face_cascade.load( filename );
}

vector<Rect> ObjectDetector::getObjects(IplImage* image){
	vector<Rect> objects;
	Mat frame_gray = ImageUtils::getGrayScaledFrame(image,true);
	face_cascade.detectMultiScale( frame_gray, objects);

	float scaleFactor = image->height/(float)frame_gray.rows;
	if (scaleFactor != 1){
		for (Rect &rect : objects){
			rect.x *= scaleFactor;
			rect.y *= scaleFactor;
			rect.width *= scaleFactor;
			rect.height *= scaleFactor;
		}
	}

	return objects;
}