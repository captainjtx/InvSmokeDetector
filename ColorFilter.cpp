#include "ColorFilter.h"

ColorFilter::ColorFilter()
{
	rectSeq = NULL;
	seqNum = 0;
	Mark = NULL;

	curImg = NULL;
	firImg = NULL;
	showImg = NULL;
	blockSeq=NULL;
	blockMark=NULL;
	threshold_color = 40;
}

void ColorFilter::release()
{
	cvReleaseImage(&firImg);
}

void ColorFilter::init(IplImage *pFrame)
{
	firImg = cvCloneImage(pFrame);
}

void ColorFilter::setRectSeq(CvRect* rectseq,int Num,int* rectMark)
{
	rectSeq = rectseq;
	seqNum = Num;
	Mark = rectMark;
}
void ColorFilter::setBlockVar(CvRect* blockseq,int* blockmark,int blockrownum,int blockcolumnnum)
{
	blockMark=blockmark;
	blockSeq=blockseq;
	blockRowNum=blockrownum;
	blockColumnNum=blockcolumnnum;
}
void ColorFilter::show()
{
	showImg = cvCloneImage(curImg);
	for(int i=0;i<seqNum;i++)
		if(Mark[i] == 2)
		{
			cvRectangle(showImg,cvPoint(rectSeq[i].x,rectSeq[i].y),cvPoint(rectSeq[i].x+rectSeq[i].width,rectSeq[i].y+rectSeq[i].height),cvScalar(0,255,0),4);
		}
	cvShowImage("ColorFilter",showImg);
	cvReleaseImage(&showImg);
}

void ColorFilter::nextImage(IplImage* pFrame)
{
	fireBlockNum=0;
	curImg = pFrame; 
	filter();
	/*blockFilter();*/
}

void ColorFilter::filter()
{
	int i = 0,j = 0,k = 0;
	int width = 0,height = 0,rectX = 0,rectY = 0;
	int widthStep = 2;
	//uchar* imgData = (uchar*)curImg->imageData;
	for(int i=0;i<seqNum;i++)
		if(Mark[i] ==2)
		{
			int markNum = 0,tNum = 0;
			width = rectSeq[i].width;
			height = rectSeq[i].height;
			rectX = rectSeq[i].x;
			rectY = rectSeq[i].y;
			for(j = rectX;j<rectX+width-2;j=j+widthStep)
				for(k = rectY;k<rectY+height-2;k=k+widthStep)
				{
					CvScalar color;
					color = cvGet2D(curImg,k,j);
					float minI = (color.val[0]<color.val[1])?color.val[0]:color.val[1];
					minI = (minI<color.val[2])?minI:color.val[2];
					float sumI = (color.val[0]+color.val[1]+color.val[2])/3;
					//if((color.val[0]>color.val[1]) && (color.val[1]>color.val[2]) && color.val[2] > 50
					//	&& s >((255-color.val[2])*0.8/sumI))
					if((abs(color.val[0]-color.val[1])<=threshold_color) && 
						(abs(color.val[0]-color.val[2])<=threshold_color) &&
						(abs(color.val[1]-color.val[2])<=threshold_color) &&
						((sumI <=150 && sumI >=80) || (sumI >=150 && sumI <=220)))
					{
						markNum++;
					}
					tNum++;
				}
				if(markNum*2 <tNum)
				{
					Mark[i] = 1;
				}
				else
				{
					++fireBlockNum;
				}
		}
}
void ColorFilter::blockFilter()
{
	int i = 0,j = 0,k = 0;
	int width = 0,height = 0,rectX = 0,rectY = 0;
	int widthStep = 2;
	//uchar* imgData = (uchar*)curImg->imageData;
	for(int i=0;i<blockRowNum*blockColumnNum;i++)
		if(blockMark[i] ==2)
		{
			int markNum = 0,tNum = 0;
			width = blockSeq[i].width;
			height = blockSeq[i].height;
			rectX = blockSeq[i].x;
			rectY = blockSeq[i].y;
			for(j = rectX;j<rectX+width-2;j=j+widthStep)
				for(k = rectY;k<rectY+height-2;k=k+widthStep)
				{
					CvScalar color;
					color = cvGet2D(curImg,k,j);
					float minI = (color.val[0]<color.val[1])?color.val[0]:color.val[1];
					minI = (minI<color.val[2])?minI:color.val[2];
					float sumI = (color.val[0]+color.val[1]+color.val[2])/3;
					//if((color.val[0]>color.val[1]) && (color.val[1]>color.val[2]) && color.val[2] > 50
					//	&& s >((255-color.val[2])*0.8/sumI))
					if((abs(color.val[0]-color.val[1])<=threshold_color) && 
						(abs(color.val[0]-color.val[2])<=threshold_color) &&
						(abs(color.val[1]-color.val[2])<=threshold_color) &&
						((sumI <=150 && sumI >=80) || (sumI >=150 && sumI <=220)))
					{
						markNum++;
					}
					tNum++;
				}
				if(markNum*2 <tNum)
				{
					blockMark[i] = 1;
				}
				else
				{
					fireBlockNum++;
				}
		}
}
