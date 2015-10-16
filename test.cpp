// test.cpp : �������̨Ӧ�ó������ڵ㡣
//
//

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv ;


#ifdef Debug
int main()
{
	VideoCapture cap ;
	Mat frame , segm;
	cap.open(0) ;

	if(!cap.isOpened())
	{
		std::cerr<<"can't open camera";

		return -1 ;
	}

	namedWindow("FG" , WINDOW_NORMAL);

	for(;;)
	{
		cap >> frame ;

		if(frame.empty())
			break ;

		frame.copyTo(segm) ;

		imshow("FG" , segm) ;
	
		int c = waitKey(30) ;
	
		if(c == 'q' || c == 'Q' || (c & 255) == 27)
			break ;
	}
}
#else 
int main( int argc, char** argv )  
{  
  //����IplImageָ��  
  IplImage* pFrame = NULL;  
  
 //��ȡ����ͷ  
  CvCapture* pCapture = cvCreateCameraCapture(0);  
   
  //��������  
  cvNamedWindow("video", 1);  
   
  //��ʾ����  
  while(1)  
  {  
      pFrame=cvQueryFrame( pCapture );  
      if(!pFrame)break;  
      cvShowImage("video",pFrame);  
      char c=cvWaitKey(33);  
      if(c=='q')
		  break;  
  }  
  cvReleaseCapture(&pCapture);  
  cvDestroyWindow("video");  
} 

#endif

#ifdef Debug

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv ;
using namespace std ;

    void update(void *param)  
    {  
        VideoCapture cap;  
        cap.open("test.avi");  
        if ( !cap.isOpened() )  
        {  
            return ;  
        }  
      
        for(;;)  
        {  
            cap>>frame;  
            //cvNamedWindow("tmp");//�����������ʹ��waitkey��û������  
            //waitKey(100);//ʹ��waitkey����ض�������Ҫ����Ϊwaitkey�ǶԴ��ڻ��������õģ���cvNamedWindow�����Ĵ��ڣ���û�в����Ĵ��ڣ���waitkey�൱��δִ�У�����Ƶ֡�ܿ�ı����꣬frameΪ�գ���Ȼ�ͳ�������  
      
            Sleep(100);//��ʹ��Sleep��������������  
        }     
    }  
      
      
    void main()  
    {  
        _beginthread(update,0,NULL);  
        Sleep(1000);  
        cvNamedWindow("test");  
      
        for(;;)  
        {  
            //�п�(ʡ��)          
            imshow("test",*frame);  
            waitKey(100);  
        }  
        waitKey(0);  
    }  
#endif 


#ifdef Debug

int main()
{
   //����Ƶ�ļ�����ʵ���ǽ���һ��VideoCapture�ṹ  
    VideoCapture capture("D:/videos/PetsD2TeC2.avi");  
    //����Ƿ�������:�ɹ���ʱ��isOpened����ture  
    if(!capture.isOpened())  
        cout<<"fail to open!"<<endl;  
    //��ȡ����֡��  
    long totalFrameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);  
    cout<<"������Ƶ��"<<totalFrameNumber<<"֡"<<endl;  
  
  
    //���ÿ�ʼ֡()  
    long frameToStart = 300;  
    capture.set( CV_CAP_PROP_POS_FRAMES,frameToStart);  
    cout<<"�ӵ�"<<frameToStart<<"֡��ʼ��"<<endl;  
  
  
    //���ý���֡  
    int frameToStop = 400;  
  
    if(frameToStop < frameToStart)  
    {  
        cout<<"����֡С�ڿ�ʼ֡��������󣬼����˳���"<<endl;  
        return -1;  
    }  
    else  
    {  
        cout<<"����֡Ϊ����"<<frameToStop<<"֡"<<endl;  
    }  
  
  
    //��ȡ֡��  
    double rate = capture.get(CV_CAP_PROP_FPS);  
    cout<<"֡��Ϊ:"<<rate<<endl;  
  
  
  
    //����һ���������ƶ�ȡ��Ƶѭ�������ı���  
    bool stop = false;  
    //����ÿһ֡��ͼ��  
    Mat frame;  
    //��ʾÿһ֡�Ĵ���  
    namedWindow("Extracted frame");  
    //��֡��ļ��ʱ��:  
    //int delay = 1000/rate;  
    int delay = 1000/rate;  
  
  
    //����whileѭ����ȡ֡  
    //currentFrame����ѭ�����п��ƶ�ȡ��ָ����֡��ѭ�������ı���  
    long currentFrame = frameToStart;  
  
  
    //�˲����ĺ�  
    int kernel_size = 3;  
    Mat kernel = Mat::ones(kernel_size,kernel_size,CV_32F)/(float)(kernel_size*kernel_size);  
  
    while(!stop)  
    {  
        //��ȡ��һ֡  
        if(!capture.read(frame))  
        {  
            cout<<"��ȡ��Ƶʧ��"<<endl;  
            waitKey(0) ;    
        }  
          
        //������˲�����  
        imshow("Extracted frame",frame);  
        filter2D(frame,frame,-1,kernel);  
  
        imshow("after filter",frame);  
        cout<<"���ڶ�ȡ��"<<currentFrame<<"֡"<<endl;  
        //waitKey(int delay=0)��delay �� 0ʱ����Զ�ȴ�����delay>0ʱ��ȴ�delay����  
        //��ʱ�����ǰû�а�������ʱ������ֵΪ-1�����򷵻ذ���  
  
  
        int c = waitKey(delay);  
        //����ESC���ߵ���ָ���Ľ���֡���˳���ȡ��Ƶ  
        if((char) c == 27 || currentFrame > frameToStop)  
        {  
            stop = true;  
        }  
        //���°������ͣ���ڵ�ǰ֡���ȴ���һ�ΰ���  
        if( c >= 0)  
        {  
            waitKey(0);  
        }  
        currentFrame++;  
      
    }  
    //�ر���Ƶ�ļ�  
    capture.release();  
    waitKey(0);  
    return 0;  
}
#endif
