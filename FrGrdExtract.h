#include "stdlib.h"
#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cvaux.h>//必须引此头文件
#include <iostream>
#include <math.h>
#include "myGMMbackground.h"
#include "constants.h"

class FrGrdExtract
{
private:
	IplImage* curImg;
	IplImage* showImg;
	CvGaussBGModel* bg_model; //初始化高斯混合模型参数

public:
	//为了方便参数访问，一下变量均为主类InvFireDetector内参量
	int blockRowNum,blockColumnNum;
  int *blockMark;
  CvRect* blockSeq;
	//***********************************************************
	CvRect* rectSeq;
	int seqNum;
	int *Mark;
	IplImage* pFrImg; 
	IplImage* pBackImg;


	FrGrdExtract();
	void init(IplImage* pFrame);
	void show();
	void nextImage(IplImage* pFrame);
	void setRectSeq(CvRect* rectseq,int* rectMark);
  void setBlockVar(CvRect* blockseq,int* blockmark,int blockrownum,int blockcolumnnum);
	void filter(IplImage* pFrImg);
	void release();
	void segmentation(IplImage* pFrImg);
};