#include "stdlib.h"
#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cvaux.h>//必须引此头文件
#include <iostream>
#include <math.h>

class ColorFilter
{
	IplImage* curImg;
	IplImage* firImg;
	IplImage* showImg;
	int threshold_color;
public:
	//为了方便参数访问，一下变量均为主类InvFireDetector内参量
	int blockRowNum,blockColumnNum;
  int *blockMark;
  CvRect* blockSeq;
	//***********************************************************
	CvRect* rectSeq;
	int seqNum;
	int* Mark;
	int fireBlockNum;

	ColorFilter();
	void init(IplImage *pFrame);
	void show();
	void setRectSeq(CvRect* rectseq,int Num,int* rectMark);
	void setBlockVar(CvRect* blockseq,int* blockmark,int blockrownum,int blockcolumnnum);
	void nextImage(IplImage *pFrame);
	void filter();
	void blockFilter();
	void release();
};