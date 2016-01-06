#include "stdlib.h"
#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cvaux.h>//必须引此头文件
#include <iostream>
#include <fstream>
#include "SwaveletFilter.h"
#include "FrGrdExtract.h"
#include "ColorFilter.h"
#include "constants.h"

using namespace std;
using namespace cv;

class InvFireDetector
{
public:
	bool      isPause;
	IplImage* pSrc;
	IplImage* pFeedBack;
	IplImage* pEnergy;
	CvRect    pRec[MAX_OF_BLOCK];
	int       blockNum;
	int       Mark[MAX_OF_BLOCK];

	int       frameNum;
	bool      invAlarm;
	bool      fireAlarm;
	int       fireBlockNum;

	int       countFrame;
	int       countFireBlock;
	InvFireDetector();
	void init(IplImage* pFrame,const char*wavename="haar");
	void update(IplImage* pFrame);
	void show();
	void release();
	void keyMenu();

	int  blockRowNum;
	int  blockColumnNum;
	CvRect* blockSeq;
	int  *blockMark;

private:
  CvVideoWriter* resultWriter;
	CvVideoWriter* energyWriter;
	CvVideoWriter* foreWriter;
	CvVideoWriter* backWriter;

	FrGrdExtract *mFrGrdExtract;
	ColorFilter *mColorFilter;
	SwaveletFilter *mSwaveletFilter;
    
	CvMat* pBackGround;
	CvMat* pBackMat;
	const char* wname;

	int count;
};