#include "ARDrawingContext.hpp"
#include "ARPipeline.hpp"
#include "DebugHelpers.hpp"

#include <opencv2/opencv.hpp>
#include <gl/gl.h>
#include <gl/GLU.h>

// 对某视频文件进行处理或是通过相机获取的时实图片进行处理
void processVideo(const cv::Mat & patternImage ,
	CameraCalibration& calibration ,
	cv::VideoCapture& capture);

// 对网络摄像头捕获的一帧图片进行处理
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

	// 本例需要从命令行中，读取模板文件的路径，模板文件的路径为命令行的第二个参数
	cv::Mat patternImage = cv::imread(argv[1]) ;
	if(patternImage.empty())
	{
		std::cout << "input image cannot be read" <<std::endl;
		return 2 ;
	}

	// 本例命令行参数至少为1个，当仅有一个命令行参数时，表示的模板文件
	// 当命令行参数为1个时，表示从摄像头中读取帧
	if(argc ==2)
	{
		processVideo(patternImage , calibration , cv::VideoCapture());
	}
	else if(argc ==3)
	{
		// 当命令行参数为两个时，第一个为模板文件，第二个为视频文件路径
		// 当命令行参数为两个时，表示从视频文件中读取帧。
		std::string input = argv[2] ;
		cv::Mat testImage = cv::imread(input);
		if(!testImage.empty())
		{
			processSingleImage(patternImage , calibration , testImage);
		}
		else
		{
			cv::VideoCapture cap ;
			// videocapture的open方法除了可以打开摄像头外，还可从视频文件中获取帧
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
	// 捕获第一帧，并获取帧的尺寸
	cv::Mat currentFrame ;
	capture >> currentFrame ;

	// 检测是否捕获到了图像
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
			continue;	// 但执行continue后，将执行while()中的判断
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

	// 调用processFrame()从拍摄的图片中找都某个特征，然后，更新特征检测状态
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


