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

// Ϊ�˱���Ѱ���������򣬻��ڿ�ʼ���е�����֡����ʾһ�����������������������ʾ��ǰ40֡��
const int NUM_STICK_FIGURE_ITERATION = 40 ;
const char * windowName = "Cartoonifier" ;

// �˱���Ϊ�棬��ϣ�������ǵ�����ͼƬʱ
bool m_sketchMode = false ;
// ��ϣ���������ǡ������ˡ�Ч��ʱ�����˱�������Ϊ��
bool m_alienMode = false ;
// ��ϣ���������ǡ��ֲ��ġ�Ч��ʱ�����˱�������Ϊ��
bool m_evilMode = false ;

bool m_debugMode = false ;

// Draws a stick figure outline for where the user's face should be. 
// ����������ÿ��ͼƬ�У��������ֵ��������һ�������� 
int m_stickFigureIterations = 40 ;

#if !defined VK_ESCAPE
	#define VK_ESCAPE 0x1B
#endif 

// get access to the webcam 
// �����������ͷ�ķ���
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

		// ����Ҫ��ʾ�������ˡ�ģʽʱ����Ҫ��ǰ����֡����ʾһ����������
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

	// �������ϰ�װ�ж������ͷʱ��ͨ���˱���ѡ����Ҫ�򿪵�����ͷ
	int cameraNumber = 0 ;
	if(argc > 1)
	{
		cameraNumber = atoi(argv[1]);
	}

	VideoCapture camera ;
	initWebcam(camera , cameraNumber);

	// ������ͷ���Ϳ���ָ�����ÿһ֡ͼƬ�Ŀ�͸�
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

		// ��һ��ʱ���ڣ������ץȡ��֡�У��������������߿�
		if(m_stickFigureIterations > 0)
		{
			drawFaceStickFigure(displayedFrame) ;
			m_stickFigureIterations -- ;
		}

		imshow(windowName , displayedFrame);

		// waitKey()����ʱ20ms���൱�ڵ�����sleep(20)��ͬʱ������ʱʱ�䵽ʱ�����ѯ�Ƿ��а�������
		// ��ˣ�waitKey()�ķ���ֵ�ǰ����ļ�ֵ�����ް������£��򷵻�-1������ʱ�����У����а������£���ʱ���ᱻ���
		// waitKey()����ǰ����
		char keypress = waitKey(20);
		if(keypress == VK_ESCAPE)
			break ;

		if(keypress > 0)
			onKeyPress(keypress);
	}

	return EXIT_SUCCESS ;
}