#include <highgui.h>

class CameraWindow
{
private:
	CvCapture* capture;

public:
	CameraWindow(float scale = 1);
	~CameraWindow();
	IplImage* getImage();
	void drawImage(IplImage* image);
};