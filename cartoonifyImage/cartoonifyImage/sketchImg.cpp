#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv ;

Mat drawFaceStickFigure(Mat image)
{
	Size size ;
	size.width = image.cols ;
	size.height = image.rows ;

	Mat faceOutline = cv::Mat::zeros(size , CV_8UC3) ;
	Scalar color = CV_RGB(255,0,0) ;
	
	int thickness = 4 ;

	// use 70% of the screen height as the face height
	int sw = size.width ;
	int sh = size.height ;
	int faceH = (sh/2) * (70.0/100 );		// faceH is the radius of the ellipses.

	// scale the width to be the same shape for any screen width .
	int faceW = faceH*72.0/100 ;
	// draw the face outline. ellipse()的第一个参数是要绘制椭圆的图片，在图片上
	// 绘制椭圆；参数2位椭圆的圆心；参数3为椭圆的长，短轴的长度，Size的宽为椭圆的长轴
	// 高为椭圆的短轴；参数4描述椭圆是否旋转，围绕圆心；参数5，6为椭圆曲线的起始和终止角度
	// ellipse函数可以绘制完整的，或部分椭圆曲线；参数7为颜色；参数8表示线宽；参数9表示是否"抗锯齿"
	ellipse(faceOutline , Point(sw/2 , sh/2) , Size(faceW , faceH) ,
		0,0,360 , color , thickness , CV_AA)  ;
	
	// draw the eye outlines , as 2 halp ellipses
	int eyeW = faceW * 23.0/100 ;
	int eyeH = faceH *11.0 / 100 ;
	int eyeX = faceW * 48.0 /100 ;
	int eyeY = faceH * 13.0 /100 ;

	// set the angle and shift for the eye halp ellipses
	int eyeA = 15 ;	// angle in degrees
	int eyeYshift = 11 ;

	// draw the top ellipse of the right eye
	ellipse(faceOutline, Point(sw/2 - eyeX , sh/2 - eyeH),Size(eyeW , eyeH) , 0 , 180 + eyeA , 360 - eyeA , color , thickness , CV_AA) ;
	ellipse(faceOutline, Point(sw/2 - eyeX, sh/2 - eyeY - eyeYshift), Size(eyeW, eyeH), 0, 0+eyeA, 180-eyeA, color, thickness, CV_AA);
    // Draw the top of the left eye.
    ellipse(faceOutline, Point(sw/2 + eyeX, sh/2 - eyeY), Size(eyeW, eyeH), 0, 180+eyeA, 360-eyeA, color, thickness, CV_AA);
    // Draw the bottom of the left eye.
    ellipse(faceOutline, Point(sw/2 + eyeX, sh/2 - eyeY - eyeYshift), Size(eyeW, eyeH), 0, 0+eyeA, 180-eyeA, color, thickness, CV_AA);

	// Draw the bottom lip of the mouth.
    int mouthY = faceH * 53/100;
    int mouthW = faceW * 45/100; 
    int mouthH = faceH * 6/100;
    ellipse(faceOutline, Point(sw/2, sh/2 + mouthY), Size(mouthW, mouthH), 0, 0, 180, color, thickness, CV_AA);

	// draw anti-aliased text.
	int fontFace = FONT_HERSHEY_COMPLEX ;
	float fontScale = 1.0;
	int fontThickness = 2 ;

	putText(faceOutline , "Put your face here" ,
		Point(sw * 23.0/100 , sh * 10.0/100) ,
		fontFace , fontScale , color , fontThickness , CV_AA) ; 

	
	addWeighted(image , 1.0,faceOutline , 0.7 , 0 , image , CV_8UC3) ;

	return image ; 

}

void changeFacialSkinColor(Mat smallImgBGR , Mat bigEdges , int debugType)
{
	Size size ;
	size.width = smallImgBGR.cols ;
	size.height = smallImgBGR.rows ;


	Mat yuv = Mat(size , CV_8UC3) ;
	cvtColor(smallImgBGR , yuv , CV_BGR2YCrCb);

	int sw = smallImgBGR.cols ;
	int sh = smallImgBGR.rows ;

	Mat maskPlusBorder = Mat::zeros(sh +2 , sw +2 , CV_8U) ;
	Mat mask = maskPlusBorder(Rect(1 , 1 , sw , sh)) ;
	resize(bigEdges , mask , smallImgBGR.size()) ;
	
	threshold(mask , mask , 80 , 255 , THRESH_BINARY) ;
	dilate(mask , mask , Mat()) ;
	erode(mask ,  mask , Mat()) ;

	int const NUM_SKIN_POINTS = 6 ;
	Point skinPts[NUM_SKIN_POINTS] ;

	skinPts[0] = Point(sw/2 , sh/2 - sh/6) ;
	skinPts[1] = Point(sw/2 - sw/11 , sh/2 - sh/6) ;
	skinPts[0] = Point(sw/2 + sw/11 , sh/2 - sh/6) ;
	skinPts[0] = Point(sw/2 , sh/2 + sh/16) ;
	skinPts[0] = Point(sw/2 - sw/9 , sh/2 + sh/16) ;
	skinPts[0] = Point(sw/2 + sw/9 , sh/2 + sh/16) ;

	const int LOWER_Y = 60 ;
	const int UPPER_Y = 80 ;
	const int LOWER_Cr = 25 ;
	const int UPPER_Cr = 15 ;
	const int LOWER_Cb = 20 ;
	const int UPPER_Cb = 15 ;

	Scalar lowerDiff = Scalar(LOWER_Y,LOWER_Cr , LOWER_Cb) ;
	Scalar upperDiff = Scalar(UPPER_Y,UPPER_Cr , UPPER_Cb) ;

	Mat edgeMask = mask.clone() ;
        for (int i=0; i<NUM_SKIN_POINTS; i++) {
            // Use the floodFill() mode that stores to an external mask, instead of the input image.
            const int flags = 4 | FLOODFILL_FIXED_RANGE | FLOODFILL_MASK_ONLY;
            floodFill(yuv, maskPlusBorder, skinPts[i], Scalar(), NULL, lowerDiff, upperDiff, flags);
            if (debugType >= 1)
                circle(smallImgBGR, skinPts[i], 5, CV_RGB(0, 0, 255), 1, CV_AA);
        }
        if (debugType >= 2)
            imshow("flood mask", mask*120); // Draw the edges as white and the skin region as grey.

        // After the flood fill, "mask" contains both edges and skin pixels, whereas
        // "edgeMask" just contains edges. So to get just the skin pixels, we can remove the edges from it.
        mask -= edgeMask;
        // "mask" now just contains 1's in the skin pixels and 0's for non-skin pixels.

        // Change the color of the skin pixels in the given BGR image.
        int Red = 0;
        int Green = 70;
        int Blue = 0;

        add(smallImgBGR, Scalar(Blue, Green, Red), smallImgBGR, mask);
}