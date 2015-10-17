#include "ARDrawingContext.hpp"
#include "ARPipeline.hpp"
#include "DebugHelpers.hpp"

#include <opencv2/opencv.hpp>
#include <gl/gl.h>
#include <gl/GLU.h>

// ��ĳ��Ƶ�ļ����д������ͨ�������ȡ��ʱʵͼƬ���д���
void processVideo(const cv::Mat & patternImage ,
	CameraCalibration& calibration ,
	cv::VideoCapture& capture);

// ����������ͷ�����һ֡ͼƬ���д���
void processSingleImage(const cv::Mat& patternImage,
	CameraCalibration& calibration ,
	const cv::Mat& image);

int main(int argc , const char * argv[])
{
	CameraCalibration calibration(526.58037684199849f,
		, 524.65577209994706f, 318.41744018680112f, 202.96659047014398f);

	if(argc <2)
	{
		std::cout << "input image not specified"<<std::endl;
		std::cout << "Usage: markerless_ar_demo"<<std::endl;
		return 1 ;
	}

	// ������Ҫ���������У���ȡģ���ļ���·����ģ���ļ���·��Ϊ�����еĵڶ�������
	cv::Mat patternImage = cv::imread(argv[1]) ;
	if(patternImage.empty())
	{
		std::cout << "input image cannot be read" <<std::endl;
		return 2 ;
	}

	// ���������в�������Ϊ1����������һ�������в���ʱ����ʾ��ģ���ļ�
	// �������в���Ϊ1��ʱ����ʾ������ͷ�ж�ȡ֡
	if(argc ==2)
	{
		processVideo(patternImage , calibration , cv::VideoCapture());
	}
	else if(argc ==3)
	{
		// �������в���Ϊ����ʱ����һ��Ϊģ���ļ����ڶ���Ϊ��Ƶ�ļ�·��
		// �������в���Ϊ����ʱ����ʾ����Ƶ�ļ��ж�ȡ֡��
		std::string input = argv[2] ;
		cv::Mat testImage = cv::imread(input);
		if(!testImage.empty())
		{
			processSingleImage(patternImage , calibration , testImage);
		}
		else
		{
			cv::VideoCapture cap ;
			// videocapture��open�������˿��Դ�����ͷ�⣬���ɴ���Ƶ�ļ��л�ȡ֡
			if(cap.open((input)))
			{
				processVideo(patternImage, patternImage , cap);
			}
		}
	}
	else
	{
		std::cerr <<"invalid number of arguments passed" <<std::endl;
		return 1 ;
	}

	return 0 ;
}

void processVideo(const cv::Mat& patternImage ,
	CameraCalibration& calibration ,
	cv::VideoCapture& capture)
{
	// �����һ֡������ȡ֡�ĳߴ�
	cv::Mat currentFrame ;
	capture >> currentFrame ;

	// ����Ƿ񲶻���ͼ��
	if(currentFrame.empty())
	{
		std::cout << "cannot open video capture device" <<endl;
		return ;
	}

	cv::Size frameSize(currentFrame.cols , currentFrame.rows);

	ARPipeline pipeline(patternImage , calibration);
	ARDrawingContext drawingCtx("Markerless AR" , frameSize , calibration);

	bool shouldQuit = false ;
	do
	{
		capture >> currentFrame ;

		if(currentFrame.empty())
		{
			shouldQuit = true ;
			continue;	// ��ִ��continue�󣬽�ִ��while()�е��ж�
		}

		shouldQuit = processFrame();
	}while(!shouldQuit);
}

bool processFrame(const cv::Mat& cameraFrame , ARPipeline& pipeline , ARDrawingContext& drawingCtx)
{
	cv::Mat img = cameraFrame.clone();

	if(pipeline.m_patternDetector.enableHomographyRefinement)
		cv::putText(img , "Pose refinement: On ('h' to switch off)" , cv::Point(10,15) , CV_FONT_HERSHEY_PLAIN , 1 , CV_RGB(0,200,0);
	else
		cv::putText(img , "Pose refinement: Off ('h' to switch on)" , cv::Point(10,15) , CV_FONT_HERSHEY_PLAIN , 1 , CV_RGB(0,200,0);

	cv::putText(img , "RANSAC threshold: " + ToString(pipeline.m_patternDetector.homographyReprojectionThreshold) + "(Use '-'/'+' to adjust)" , cv::Point(10,30) , CV_FONT_HERSHEY_PLAIN , 1, CV_RGB(0,200,0)) ;

	drawingCtx.updateBackground(img) ;

	// ����processFrame()�������ͼƬ���Ҷ�ĳ��������Ȼ�󣬸����������״̬
	drawingCtx.isPatternPresent = pipeline.processFrame(cameraFrame) ;

	drawingCtx.patternPose = pipeline.getPatternLocation();

	drawingCtx.updateWindow();

	int keyCode = cv::waitKey(5);

	bool shouldQuit = false ;

	if(keyCode == '+' || keyCode == '-')
	{
		pipeline.m_patternDetector.homographyReprojectionThreshold += 0.2f;
		pipeline.m_patternDetector.homographyReprojectionThreshold = std::min(10.0f , pipeline.m_patternDetector.homographyReprojectionThreshold);
	}
	else if (keyCode == '-')
	{
		pipeline.m_patternDetector.homographyReprojectionThreshold -= 0.2f;
		pipeline.m_patternDetector.homographyReprojectionThreshold = std::min(0.0f, pipeline.m_patternDetector.homographyReprojectionThreshold);
	}
	else if (keyCode == 'h')
	{
		pipeline.m_patternDetector.enableHomographyRefinement = !pipeline.m_patternDetector.enableHomographyRefinement;
	}
	else if (keyCode == 27 || keyCode == 'q')
	{
		shouldQuit = true;
	}

	return shouldQuit ;
}


