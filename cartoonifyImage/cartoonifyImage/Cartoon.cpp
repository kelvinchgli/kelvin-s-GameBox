#include "cartoon.h"
#include "ImageUtils.h"		// 作者自己编写的一些，用于调试opencv的函数
#include <ctime>
#include <Windows.h>

void cartoonifyImage(Mat srcColor , Mat dst , 
	bool sketchMode , bool alienMode , 
	bool evilMode , bool debugType)
{
	// 算法时间统计方法1，时间精确到秒
	//time_t start = 0 , end = 0 ;
	//time(&start);

	Mat srcGray ;
	cvtColor(srcColor , srcGray , CV_BGR2GRAY);
	imshow("Cartoonifier" , srcGray);
	MessageBox(NULL , "Gray" , "debug" , MB_OK);

	// remove the pixel noise with a good Median filter, before we start detecting edges ;
	// 首先使用中值滤波器对图片进行滤波
	medianBlur(srcGray , srcGray , 7);		// 对参数1进行滤波，处理后结果保存在参数2中，使用7x7的核进行滤波
	imshow("Cartoonifier" , srcGray);
	MessageBox(NULL , "median Blur", "debug" , MB_OK );

	Size size(srcColor.cols , srcColor.rows);
	Mat mask = Mat(size , CV_8UC3);
	Mat edges = Mat(size , CV_8UC3);

	if(!evilMode)
	{
		// 使用拉普拉斯算子进行边沿检测
		Laplacian(srcGray , edges , CV_8U , 5);
		imshow("Cartoonifier" , edges);
		MessageBox(NULL , "Laplacian", "debug" , MB_OK);

		threshold(edges , mask , 80 , 255 , THRESH_BINARY_INV);
		imshow("Cartoonifier" , mask);
		MessageBox(NULL , "threshold", "debug" , MB_OK);

		removePepperNoise(mask);
		imshow("Cartoonifier" , mask);
		MessageBox(NULL , "remove pepper noise", "debug" , MB_OK);
	}
	else
	{
		Mat edges2 ;
		Scharr(srcGray , edges , CV_8U , 1 , 0);
		imshow("Cartoonifier" , edges);
		MessageBox(NULL , "scharr 1", "debug" , MB_OK);

		Scharr(srcGray , edges2 , CV_8U , 1 , 0 ,-1);
		imshow("Cartoonifier" , edges2);
		MessageBox(NULL , "scharr -1", "debug" , MB_OK);

		edges += edges2 ;			// 为什么要这样？？？
		threshold(edges , mask , 12 , 255 , THRESH_BINARY_INV);
		imshow("Cartoonifier" , mask);
		MessageBox(NULL , "threshod after scharr -1", "debug" , MB_OK);

		medianBlur(mask , mask , 3);		// 使用3x3的核进行滤波
		imshow("Cartoonifier" , mask);
		MessageBox(NULL , "median after scharr -1 and threshold", "debug" , MB_OK);
	}

	//if(sketchMode)
	{
		// cvtColor将gray图转换为bgr图时，b，g，r通道的值相同，均为灰度图的值。
		cvtColor(mask , dst , CV_GRAY2BGR);
		imshow("Cartoonifier" , mask);
		MessageBox(NULL , "sketch" , "debug" , MB_OK);
		//return ;
	}

	Size smallSize ;
	smallSize.width = size.width/2 ;
	smallSize.height = size.height /2 ;

	Mat smallImg = Mat(smallSize , CV_8UC3);
	// resize()在对图片进行缩放时，缩放后的结果图片的size可以直接由参数3来指定，当参数3为0时，
	// 则可由参数4,5来计算，参数4,5表示缩放系数，相对于原图的尺寸缩放时的缩放系数。当两个都指定时
	// 默认由参数3来决定。
	resize(srcColor , smallImg , smallSize , 0,0,INTER_LINEAR);
	imshow("Cartoonifier" , srcColor);
	MessageBox(NULL , "原图" , "Debug" , MB_OK);
	Mat tmp = Mat(smallSize , CV_8UC3);
	int repetitions = 7 ;

	// 使用双边滤波对原图进行7x2=14此滤波，以显示出有话的效果
	for(int i = 0 ; i < repetitions ; i ++)
	{
		int size = 9 ;
		double sigmaColor = 9 ;
		double sigmaSpace = 7 ;

		bilateralFilter(smallImg , tmp , size , sigmaColor , sigmaSpace);
		bilateralFilter(tmp , smallImg , size , sigmaColor , sigmaSpace);
	}

	if(alienMode)
	{
		changeFacialSkinColor(smallImg , edges , debugType);
	}

	resize(smallImg , srcColor , size , 0 , 0 , INTER_LINEAR) ;
	//memset((char *)dst.data , 0 , dst.step*dst.rows);
	dst.setTo(0);

	srcColor.copyTo(dst , mask);

	//time(&end);

	//string mode = "";
	//if(sketchMode)
	//	mode = "sketch";
	//else
	//	mode = "paint";

	//if(alienMode)
	//	mode = "alien";
	//else
	//	mode = "human";

	//if(evilMode)
	//	mode = "evil";
	//else
	//	mode = "good";
	
	//cout<<mode << "time"<< ":" <<(end-start)<<endl;
}


//void removePepperNoise(Mat &mask)
//{
//	// For simplicity, ignore the top & bottom row border.
//	for (int y=2; y<mask.rows-2; y++) 
//	{
//		// Get access to each of the 5 rows near this pixel.
//		uchar *pThis = mask.ptr(y);
//		uchar *pUp1 = mask.ptr(y-1);
//		uchar *pUp2 = mask.ptr(y-2);
//		uchar *pDown1 = mask.ptr(y+1);
//		uchar *pDown2 = mask.ptr(y+2);
//
//		// For simplicity, ignore the left & right row border.
//		pThis += 2;
//		pUp1 += 2;
//		pUp2 += 2;
//		pDown1 += 2;
//		pDown2 += 2;
//		for (int x=2; x<mask.cols-2; x++) 
//		{
//			uchar v = *pThis;   // Get the current pixel value (either 0 or 255).
//			// If the current pixel is black, but all the pixels on the 2-pixel-radius-border are white
//			// (ie: it is a small island of black pixels, surrounded by white), then delete that island.
//			if (v == 0) 
//			{
//				bool allAbove = *(pUp2 - 2) && *(pUp2 - 1) && *(pUp2) && *(pUp2 + 1) && *(pUp2 + 2);
//				bool allLeft = *(pUp1 - 2) && *(pThis - 2) && *(pDown1 - 2);
//				bool allBelow = *(pDown2 - 2) && *(pDown2 - 1) && *(pDown2) && *(pDown2 + 1) && *(pDown2 + 2);
//				bool allRight = *(pUp1 + 2) && *(pThis + 2) && *(pDown1 + 2);
//				bool surroundings = allAbove && allLeft && allBelow && allRight;
//				if (surroundings == true) 
//				{
//					// Fill the whole 5x5 block as white. Since we know the 5x5 borders
//					// are already white, just need to fill the 3x3 inner region.
//					*(pUp1 - 1) = 255;
//					*(pUp1 + 0) = 255;
//					*(pUp1 + 1) = 255;
//					*(pThis - 1) = 255;
//					*(pThis + 0) = 255;
//					*(pThis + 1) = 255;
//					*(pDown1 - 1) = 255;
//					*(pDown1 + 0) = 255;
//					*(pDown1 + 1) = 255;
//				}
//				// Since we just covered the whole 5x5 block with white, we know the next 2 pixels
//				// won't be black, so skip the next 2 pixels on the right.
//				pThis += 2;
//				pUp1 += 2;
//				pUp2 += 2;
//				pDown1 += 2;
//				pDown2 += 2;
//			}
//			// Move to the next pixel.
//			pThis++;
//			pUp1++;
//			pUp2++;
//			pDown1++;
//			pDown2++;
//		}
//	}
//}

void removePepperNoise(Mat &mask)
{
	for(int y = 2 ; y < mask.rows-2 ; y ++)
	{
		// mask.ptr[y]返回mask图像数据中，第y行的数据的指针
		// 搜索每个像素的上下两行，包括像素所在行，共5行
		uchar *pThis = mask.ptr(y);
		uchar *pUp1 = mask.ptr(y-1);
		uchar * pUp2 = mask.ptr(y-2);
		uchar * pDown1 = mask.ptr(y+1);
		uchar * pDown2 = mask.ptr(y+2);
	
		pThis += 2 ;
		pUp1 += 2 ;
		pUp2 += 2 ;
		pDown1 += 2 ;
		pDown2 += 2 ;

		for(int x = 2 ; x < mask.cols -2 ; x ++)
		{
			uchar v = *pThis ;

			if(v == 0)
			{
				bool allAbove = *(pUp2-2) && *(pUp2-1)&& *(pUp2) && *(pUp2+1) && *(pUp2+2);
				bool allLeft = *(pUp1-2) && *(pThis-2)&& *(pDown1-2);
				bool allBelow = *(pDown2-2) && *(pDown2 - 1) && *(pDown2) && *(pDown2 +1) && *(pDown2+2);
				bool allRight = *(pUp1 +2) && *(pThis + 2) && *(pDown1 +2);
				bool surroundings = allAbove && allBelow && allRight && allLeft;
			
				if(surroundings == true)
				{
					*(pUp1 - 1) = 255 ;
					*(pUp1 + 0) = 255 ;
					*(pUp1 + 1) = 255 ;
					*(pThis -1) = 255 ;
					*(pThis + 0) = 255 ;
					*(pThis + 1) = 255 ;
					*(pDown1 - 1)= 255 ;
					*(pDown1 + 0)= 255 ;
					*(pDown1 + 1) = 255 ;
				}

				pThis += 2 ;
				pUp1 += 2 ;
				pUp2 += 2 ;
				pDown1 += 2 ;
				pDown2 += 2;
			}
			
			pThis ++ ;
			pUp1 ++ ;
			pUp2 ++ ;
			pDown1 ++ ;
			pDown1 ++ ;
		}
	}
}

void drawFaceStickFigure(Mat dst)
{
	Size size = dst.size();
	int sw = size.width;
	int sh = size.height;

	// Draw the face onto a color image with black background.
	Mat faceOutline = Mat::zeros(size, CV_8UC3);
	Scalar color = CV_RGB(255,255,0);   // Yellow
	int thickness = 4;
	// Use 70% of the screen height as the face height.
	int faceH = sh/2 * 70/100;  // "faceH" is actually half the face height (ie: radius of the ellipse).
	// Scale the width to be the same nice shape for any screen width (based on screen height).
	int faceW = faceH * 72/100; // Use a face with an aspect ratio of 0.72
	// Draw the face outline.
	ellipse(faceOutline, Point(sw/2, sh/2), Size(faceW, faceH), 0, 0, 360, color, thickness, CV_AA);
	// Draw the eye outlines, as 2 half ellipses.
	int eyeW = faceW * 23/100;
	int eyeH = faceH * 11/100;
	int eyeX = faceW * 48/100;
	int eyeY = faceH * 13/100;
	// Set the angle and shift for the eye half ellipses.
	int eyeA = 15; // angle in degrees.
	int eyeYshift = 11;
	// Draw the top of the right eye.
	ellipse(faceOutline, Point(sw/2 - eyeX, sh/2 - eyeY), Size(eyeW, eyeH), 0, 180+eyeA, 360-eyeA, color, thickness, CV_AA);
	// Draw the bottom of the right eye.
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

	// Draw anti-aliased text.
	int fontFace = FONT_HERSHEY_COMPLEX;
	float fontScale = 1.0f;
	int fontThickness = 2;
	putText(faceOutline, "Put your face here", Point(sw * 23/100, sh * 10/100), fontFace, fontScale, color, fontThickness, CV_AA);
	//imshow("faceOutline", faceOutline);

	// Overlay the outline with alpha blending.
	addWeighted(dst, 1.0, faceOutline, 0.7, 0, dst, CV_8UC3);
}
