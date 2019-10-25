#include <highgui.h>
#include "opencv2/objdetect.hpp"

using namespace cv;
using namespace std;

class ObjectDetector{
private:
	CascadeClassifier face_cascade;

public:
	ObjectDetector(const String& filename);
	vector<Rect> getObjects(IplImage* image);
};