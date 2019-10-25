

#include "ImageUtils.hpp"
#include <opencv2/photo/photo.hpp>
#include <opencv2/xphoto.hpp>

IplImage ImageUtils::inpaintRectToImage(Rect rect, IplImage* image){
	Mat frame = cvarrToMat(image,true);
	Point pt1( rect.x , rect.y  );
	Point pt2( rect.x + rect.width, rect.y + rect.height );

	Mat frame_inpainted;
	Mat mask = Mat::zeros(frame.size(), CV_8UC1);
	rectangle(mask, pt1, pt2, Scalar( 255, 255, 255 ), CV_FILLED);
	inpaint(frame, mask, frame_inpainted, 2, INPAINT_TELEA);

	return IplImage(frame_inpainted);
}

IplImage ImageUtils::xphotoInpaintRectToImage(Rect rect, IplImage* image){
	Mat frame = cvarrToMat(image,true);
	Point pt1( rect.x , rect.y  );
	Point pt2( rect.x + rect.width, rect.y + rect.height );

	Mat frame_inpainted;
	Mat mask = Mat(frame.size(), CV_8UC1,Scalar( 255, 255, 255 ));
	rectangle(mask, pt1, pt2, Scalar( 0, 0, 0 ), CV_FILLED);
	cv::xphoto::inpaint(frame, mask, frame_inpainted, cv::xphoto::INPAINT_SHIFTMAP);

	return IplImage(frame_inpainted);
}

IplImage ImageUtils::blurRectToImage(Rect rect, IplImage *image){
	Mat frame = cvarrToMat(image,true);
	Mat frame_blured = frame.clone();
	uint radius = (rect.width)/10;
	GaussianBlur(frame(rect), frame_blured(rect), Size(0, 0),radius,radius);
	return IplImage(frame_blured);
}

void ImageUtils::drawRectToImage(Rect rect,IplImage* image){
	Mat frame = cvarrToMat(image);
	Point pt1( rect.x , rect.y  );
	Point pt2( rect.x + rect.width, rect.y + rect.height );
	rectangle( frame, pt1, pt2, Scalar( 255, 0, 255 ));
}

void ImageUtils::fillRectInImage(Rect rect,IplImage* image){
	for (int y = 0; y < image->height; y++){
		for (int x = 0; x < image->width; x++){
			if (
				x >= rect.x && x <= rect.x + rect.width &&
				y >= rect.y && y <= rect.y + rect.height 
				){
				auto pointer = (image->width * y + x) * 3;
				image->imageData[pointer] *= 2;
				image->imageData[pointer + 1] += 120;
				image->imageData[pointer + 2] += pointer/10;
			}
		}
	}
}

Mat ImageUtils::getGrayScaledFrame(IplImage* image,bool scale){
	Mat frame = cvarrToMat(image);

	int maxHeight = 100;
	if (image->height > maxHeight && scale){
		int width = (int)((float)image->width * ((float) maxHeight / (float)image->height));
		Size size(width,maxHeight);//the dst image size,e.g.100x100
		Mat frame_resized;
		resize(frame,frame_resized,size);//resize image
		frame = frame_resized;
	}

	Mat frame_gray;
	cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );
	return frame_gray;
}

