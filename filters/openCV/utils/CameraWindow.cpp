#include "CameraWindow.hpp"

CameraWindow::CameraWindow(float scale){
	capture = cvCreateCameraCapture(CV_CAP_ANY);

	if (scale != 1){
		double 
		width = cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH),
		height = cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT);

		cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, width * scale);
		cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, height * scale );
	}
	
	assert( capture );
	cvNamedWindow("capture", CV_WINDOW_AUTOSIZE);
}
CameraWindow::~CameraWindow(){
	cvReleaseCapture( &capture );
	cvDestroyWindow("capture");
}

IplImage* CameraWindow::getImage(){
	return cvQueryFrame( capture );
}

void CameraWindow::drawImage(IplImage* image){
	cvShowImage("capture", image);
}