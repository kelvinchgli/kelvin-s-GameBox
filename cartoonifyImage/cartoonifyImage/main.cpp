#include <opencv2\opencv.hpp>
#include <iostream>
#include <Windows.h>

using namespace cv ;

extern Mat drawFaceStickFigure(Mat image) ;
extern void changeFacialSkinColor(Mat smallImgBGR , Mat bigEdges , int debugType);

int main()
{
	VideoCapture cap ;
	Mat frame , segm ;

	system("color 2F") ;

	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE) , 
	//	BACKGROUND_INTENSITY | BACKGROUND_GREEN ) ;

	//
	//printf("kelvin's demo") ;

	//int i = 0 ;
	//scanf("%d" , &i) ; 

	cap.open(0) ;

	if(cap.isOpened() == false)
	{
		std::cerr<< "camera open failed"<<std::endl ;
		return -1 ;
	}

	namedWindow("cartoonifiyImage",WINDOW_AUTOSIZE) ;

	for(;;)
	{
		cap >> frame ;

		if(frame.empty())
			break ;
			
		Size size;
		size.width = frame.cols ;
		size.height = frame.rows ;

		Mat edges = Mat(size, CV_8U);

		frame.copyTo(segm) ;
		segm = drawFaceStickFigure(segm) ;

		Size smallSize;
		smallSize.width = size.width/2;
		smallSize.height = size.height/2;
		Mat smallImg = Mat(smallSize, CV_8UC3);
	    resize(segm, smallImg, smallSize, 0,0, INTER_LINEAR);

		changeFacialSkinColor(smallImg, edges , 2) ;

		// Go back to the original scale.
		resize(smallImg, segm, size, 0,0, INTER_LINEAR);

		//// Clear the output image to black, so that the cartoon line drawings will be black (ie: not drawn).
		//memset((char*)dst.data, 0, dst.step * dst.rows);

		//// Use the blurry cartoon image, except for the strong edges that we will leave black.
		//srcColor.copyTo(dst, mask);

		imshow("cartoonifyImage" , segm) ;
		int i = waitKey(33) ;
		if(i == 'q')
			break ;
	}


}