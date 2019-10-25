#include <highgui.h>
#include <opencv2/imgproc.hpp>

using namespace cv;

class ImageUtils{
public:
	static IplImage inpaintRectToImage(Rect rect,IplImage* image);
	static IplImage xphotoInpaintRectToImage(Rect rect, IplImage* image);
	static IplImage blurRectToImage(Rect rect,IplImage* image);
	static void drawRectToImage(Rect rect,IplImage* image);
	static void fillRectInImage(Rect rect,IplImage* image);
	static Mat getGrayScaledFrame(IplImage* image,bool scale = true); 
};