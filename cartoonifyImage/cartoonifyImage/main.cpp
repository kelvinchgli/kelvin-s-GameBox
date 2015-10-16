#include<stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include<Windows.h>

#include "cartoon.h"
#include "ImageUtils.h"

using namespace cv ;
using namespace std ;

const int DESIRED_CAMERA_WIDTH = 640 ;
const int DESIRED_CAMERA_HEIGHT = 480 ;

// 为了便于寻找人脸区域，会在开始运行的若干帧中显示一个脸型轮廓，这个轮廓仅显示在前40帧中
const int NUM_STICK_FIGURE_ITERATION = 40 ;
const char * windowName = "Cartoonifier" ;

// 此变量为真，当希望看到是的素描图片时
bool m_sketchMode = false ;
// 当希望看到的是“外星人”效果时，将此变量设置为真
bool m_alienMode = false ;
// 当希望看到的是“恐怖的”效果时，将此变量设置为真
bool m_evilMode = false ;

bool m_debugMode = false ;

// Draws a stick figure outline for where the user's face should be. 
// 在相机拍摄的每张图片中，人脸出现的区域绘制一个轮廓线 
int m_stickFigureIterations = 40 ;

#if !defined VK_ESCAPE
	#define VK_ESCAPE 0x1B
#endif 

// get access to the webcam 
// 获得网络摄像头的访问
void initWebcam(VideoCapture &videoCapture , int cameraNumber)
{
	// get access to the default camera
	try
	{
		videoCapture.open(cameraNumber) ;
	}
	catch(cv::Exception &e)
	{
	}

	if(false == videoCapture.isOpened())
	{
		std::cerr << "Error:can't open camera" <<endl;
		exit(1);
	}

	cout<<"loaded camera"<<cameraNumber <<"." <<endl;
}

// keypress event handler
void onKeyPress(char key)
{
	switch(key)
	{
	case 's':
		m_sketchMode = !m_sketchMode ;
		cout <<"Sketch / Paint mode:" << m_sketchMode <<endl ;
		break ;
	case 'a':
		m_alienMode = !m_alienMode ;
		cout <<"Alien / Human mode:" << m_alienMode<<endl ;
		break ;

		// 当需要显示“外星人”模式时，需要在前部分帧中显示一个脸型轮廓
		if(m_alienMode)
		{
			m_stickFigureIterations = NUM_STICK_FIGURE_ITERATION ;
		}
	case 'e':
		m_evilMode = !m_evilMode ;
		cout<<"Evil / Good mode:"<<m_evilMode<<endl ;
		break ;
	case 'd':
		m_debugMode = !m_debugMode ;
		cout<<"Debug:mode"<<m_debugMode<<endl;
		break ;
	}
}

int main(int argc , const char * argv[])
{
	system("color 2F");
	cout << "Cartoonifier"<<endl;
	cout <<"converts real-life images to cartoon-like images."<<endl ;
	cout <<"Opencv version "<< CV_VERSION << endl ;
	cout <<endl;

	cout <<"Keyboard commands:"<<endl;
	cout <<"	Escape:	Quit the program."<<endl;
	cout <<"	s:		change Sketch / Paint mode"<<endl ;
	cout <<"	a:		change Alien / Human mode"<<endl ;
	cout <<"	e:		change Evil / Good character mode."<<endl;
	cout <<"	d:		change debug mode."<<endl ;
	cout << endl ;

	// 当电脑上安装有多个摄像头时，通过此变量选择需要打开的摄像头
	int cameraNumber = 0 ;
	if(argc > 1)
	{
		cameraNumber = atoi(argv[1]);
	}

	VideoCapture camera ;
	initWebcam(camera , cameraNumber);

	// 相摄像头发送控制指令，设置每一帧图片的宽和高
	camera.set(CV_CAP_PROP_FRAME_WIDTH , DESIRED_CAMERA_WIDTH);
	camera.set(CV_CAP_PROP_FRAME_HEIGHT , DESIRED_CAMERA_HEIGHT);

	namedWindow(windowName);

	while(true)
	{
		Mat cameraFrame ;
		camera >> cameraFrame ;
		if(cameraFrame.empty())
		{
			cerr<<"Error:can't grab the next camera frame."<<endl;
			exit(1);
		}

		Size size(cameraFrame.cols , cameraFrame.rows) ;		
		Mat displayedFrame = Mat(size , CV_8UC3);

		int debugType = 0 ;
		if(m_debugMode)
			debugType = 2 ;

		cartoonifyImage(cameraFrame , displayedFrame , m_sketchMode , m_alienMode , m_evilMode , debugType);

		// 在一段时间内，在相机抓取的帧中，叠加脸型轮廓线框
		if(m_stickFigureIterations > 0)
		{
			drawFaceStickFigure(displayedFrame) ;
			m_stickFigureIterations -- ;
		}

		imshow(windowName , displayedFrame);

		// waitKey()将延时20ms，相当于调用了sleep(20)，同时，当延时时间到时，会查询是否有按键按下
		// 因此，waitKey()的返回值是按键的键值，若无按键按下，则返回-1。当延时过程中，若有按键按下，延时将会被打断
		// waitKey()将提前返回
		char keypress = waitKey(20);
		if(keypress == VK_ESCAPE)
			break ;

		if(keypress > 0)
			onKeyPress(keypress);
	}

	return EXIT_SUCCESS ;
}