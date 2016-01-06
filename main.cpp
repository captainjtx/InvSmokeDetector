#include"InvFireDetector.h"

using namespace std;


/*
�˴�ע�Ͳ���Ϊ�ӿں��������������,Ҳ���Ƕ������Լ�����main�����ĸ�д

����˵����int width,int height,int nChannel,uchar* pData
		  ͼƬ��ߣ�ͨ����������ָ�루ϣ����BGRͨ�����ݣ�������Ҫ֪ͨ���ǽ����޸ģ���

		  bool &invAlarm,bool &fireAlarm
		  �ֱ������ּ��ͻ��ּ����źţ�����������һ����¼�����������ź�Ϊtrue��

���⣺ �����������ⲿ����
		InvFireDetector test��
		IplImage* pFrame = NULL;
		����������һ��ĺ����ж���ȽϺã�

		��������һ����Դ���պ�����Ҫ��ͼƬ����ȫ������֮��ִ�У�
		test.release(); 

Ҫ��������ӿں�����Ҫ��������̵�*.cpp��*.hȫ����������Ŀ�����Ҫ���õ�CPP�а���#include"InvFireDetector.h"��
Ȼ����Ҫ������ȷ��OpenCV���òſ������У�

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
	//�ͷ�ռ���ڴ�   
	test.release(); 
	int temp;
	cin>>temp;
	return 0;
}