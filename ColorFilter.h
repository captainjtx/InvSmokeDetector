#include "stdlib.h"
#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cvaux.h>//��������ͷ�ļ�
#include <iostream>
#include <math.h>

class ColorFilter
{
	IplImage* curImg;
	IplImage* firImg;
	IplImage* showImg;
	int threshold_color;
public:
	//Ϊ�˷���������ʣ�һ�±�����Ϊ����InvFireDetector�ڲ���
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