#include"InvFireDetector.h"

using namespace std;


/*
此处注释部分为接口函数供主程序采用,也就是对我们自己程序main函数的改写

参数说明：int width,int height,int nChannel,uchar* pData
		  图片宽高，通道数，数据指针（希望是BGR通道数据，否则需要通知我们进行修改）；

		  bool &invAlarm,bool &fireAlarm
		  分别是入侵检测和火灾检测的信号，如若有其中一类的事件发生，则此信号为true；

另外： 下面这两个外部变量
		InvFireDetector test；
		IplImage* pFrame = NULL;
		放在你们上一层的函数中定义比较好；

		后续还有一个资源回收函数，要在图片处理全部结束之后执行：
		test.release(); 

要调用这个接口函数需要将这个工程的*.cpp和*.h全部包括在项目里，在需要调用的CPP中包含#include"InvFireDetector.h"；
然后需要进行正确的OpenCV配置才可以运行；

InvFireDetector test;
IplImage* pFrame = NULL;
void InvFireDetect(int width,int height,int nChannel,uchar* pData,bool &invAlarm,bool &fireAlarm)
{
	static int counter=0;
	counter++;
	if(counter ==1)
	{
		pFrame = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,nChannel);
		pFrame->imageData = (char*)pData;
		test.init(pFrame,WAVE_NAME);
	}
	else
	{
		pFrame->imageData = (char*)pData;
		test.update(pFrame);
		test.show();
		cvWaitKey(10);
	}

	invAlarm = test.invAlarm;
	fireAlarm = test.fireAlarm;
}

*/

int main( int argc, char** argv )
{
/*statistical data*/
	int invTotleNum=0;
	int fireTotleNum=0;
	//**************

	IplImage* pFrame = NULL;
	CvCapture* pCapture = NULL;   
	if( !(pCapture = cvCreateFileCapture(VIDEO_NAME)))     
	{   
		fprintf(stderr, "Can not open video file %s\n", argv[1]);   
		return -2;     
	}

	InvFireDetector test;

	if(pFrame=cvQueryFrame(pCapture))
	{
		test.init(pFrame,WAVE_NAME);
		cvWaitKey(WAIT_TIME);
	}
//***************************************************************************
	while(pFrame = cvQueryFrame( pCapture ))   
	{
		test.update(pFrame);
		//test.show();
		//test.keyMenu();
		invTotleNum+=test.blockNum;
		fireTotleNum+=test.fireBlockNum;
		int posFrames   = (int) cvGetCaptureProperty(pCapture, CV_CAP_PROP_POS_FRAMES);
		int posMsecs=(double) cvGetCaptureProperty(pCapture,CV_CAP_PROP_POS_MSEC);
		cout<<posFrames<<"     "<<posMsecs<<"      "<<"fireAlarm:"<<test.fireAlarm<<"  invAlarm:"<<test.invAlarm<<"  invTotle:"<<\
			invTotleNum<<"  fireTotle"<<fireTotleNum<<endl;
	}
//***************************************************************************
	//释放占用内存   
	test.release(); 
	int temp;
	cin>>temp;
	return 0;
}