#include"InvFireDetector.h"
InvFireDetector::InvFireDetector()
{
	pSrc=NULL;pFeedBack=NULL;mFrGrdExtract=NULL;mColorFilter=NULL;mSwaveletFilter=NULL;wname=NULL;
	isPause=true;fireAlarm=false;invAlarm=false;
}
void InvFireDetector::init(IplImage* pFrame,const char* wavename)
{
	countFrame=0;
	countFireBlock=0;
	memset(this->pRec,0,sizeof(pRec));
	pSrc=cvCreateImage(cvSize(ADJUST_WIDTH,ADJUST_HEIGHT),IPL_DEPTH_8U,3);
	/*pFeedBack=cvCreateImage(cvSize(430,240),IPL_DEPTH_8U,3);*/
	cvResize(pFrame,pSrc);

	pFeedBack=cvCreateImage(cvSize(ADJUST_WIDTH,ADJUST_HEIGHT),IPL_DEPTH_8U,3);
	pEnergy=cvCreateImage(cvSize(ADJUST_WIDTH,ADJUST_HEIGHT),IPL_DEPTH_8U,1);

	frameNum=0;
	fireBlockNum=0;
    blockNum=0;
	wname=wavename;
	mFrGrdExtract=new FrGrdExtract;
	mColorFilter=new ColorFilter;
	mSwaveletFilter=new SwaveletFilter;
	 blockRowNum=15;
  blockColumnNum=15;
  blockMark=new int[blockColumnNum*blockRowNum];
  blockSeq=new CvRect[blockColumnNum*blockRowNum];

  int blockWidth=pSrc->width/blockColumnNum;
  int blockHeight=pSrc->height/blockRowNum;

  for(int i=0;i<blockRowNum;++i)
  {
  	for(int j=0;j<blockColumnNum;++j)
  	{
  		//此处按左上角的rectangle是（0,0）算的
  		blockSeq[i*blockColumnNum+j].x=j*blockWidth;
  		blockSeq[i*blockColumnNum+j].y=i*blockHeight;
  		blockSeq[i*blockColumnNum+j].width=i<(blockRowNum-1)?blockWidth:pSrc->width-(blockColumnNum-1)*blockWidth;
  		blockSeq[i*blockColumnNum+j].height=j<(blockColumnNum-1)?blockHeight:pSrc->height-(blockRowNum-1)*blockHeight;
  		blockMark[i*blockColumnNum+j]=0;
  	}
  }

	mFrGrdExtract->init(pSrc);
	mFrGrdExtract->setRectSeq(pRec,Mark);
	mFrGrdExtract->setBlockVar(blockSeq,blockMark,blockRowNum,blockColumnNum);

	mColorFilter->init(pSrc);
	mColorFilter->setRectSeq(this->pRec,blockNum,Mark);
	mColorFilter->setBlockVar(blockSeq,blockMark,blockRowNum,blockColumnNum);

	mSwaveletFilter->init(pSrc,wavename,1);
	mSwaveletFilter->setRectSeq(this->pRec,blockNum,this->Mark);
	mSwaveletFilter->setBlockVar(blockSeq,blockMark,blockRowNum,blockColumnNum);


	pBackGround=cvCreateMat(pSrc->height,pSrc->width,CV_32FC1);
	pBackMat=cvCreateMat(pSrc->height,pSrc->width,CV_32FC3);
	count=0;

	resultWriter=cvCreateVideoWriter(RESULT_NAME, CV_FOURCC('D', 'I', 'V', 'X'), OUTPUT_FPS, cvSize(ADJUST_WIDTH, ADJUST_HEIGHT),1);
	//energyWriter=cvCreateVideoWriter(ENERGY_NAME, CV_FOURCC('D', 'I', 'V', 'X'), OUTPUT_FPS, cvSize(ADJUST_WIDTH, ADJUST_HEIGHT),0);
	//foreWriter=cvCreateVideoWriter(FORE_NAME, CV_FOURCC('D', 'I', 'V', 'X'), OUTPUT_FPS, cvSize(ADJUST_WIDTH, ADJUST_HEIGHT),0);
	//backWriter=cvCreateVideoWriter(BACK_NAME, CV_FOURCC('D', 'I', 'V', 'X'), OUTPUT_FPS, cvSize(ADJUST_WIDTH, ADJUST_HEIGHT),1);
 
}

void InvFireDetector::update(IplImage* pFrame)
{
	cvResize(pFrame,pSrc);
	++frameNum;
	if(frameNum%FRAME_CAL==0)
	{
		mFrGrdExtract->nextImage(pSrc);
		blockNum=mFrGrdExtract->seqNum;
		if(blockNum>0)
		{invAlarm=true;}
		else
		{invAlarm=false;}

		mSwaveletFilter->seqNum=blockNum;
		mSwaveletFilter->setRectSeq(mFrGrdExtract->rectSeq,blockNum,mFrGrdExtract->Mark);//
		mSwaveletFilter->nextImage(pSrc);

		mColorFilter->seqNum=blockNum;
		mColorFilter->setRectSeq(mSwaveletFilter->rectSeq,blockNum,mSwaveletFilter->Mark);//
		mColorFilter->nextImage(pSrc);

		fireBlockNum=mColorFilter->fireBlockNum;

		if(fireBlockNum>0)
			fireAlarm=true;
		else
			fireAlarm=false;

		
		if(++count>BK_NUM)
		{
			count=0;
			double scale = (double)1/(double)255;
		    cvConvertScale(mFrGrdExtract->pBackImg,pBackMat,scale,0);
			cvCvtColor(pBackMat,pBackGround,CV_BGR2GRAY);
			mSwaveletFilter->WaveletAnalysis(pBackGround,mSwaveletFilter->pBackground);
		}
	}
}

void InvFireDetector::show()
{
    /*cvShowImage("source",pSrc);*/
    cvShowImage("foreground",mFrGrdExtract->pFrImg);
	cvShowImage("energy",mSwaveletFilter->pEnergyImg);
	cvShowImage("background",mFrGrdExtract->pBackImg);

	cvCopy(pSrc,pFeedBack);
	for(int i=0;i<blockNum;++i)
	{
		CvScalar color;

		switch(Mark[i])
		{
		case 1:
			color=cvScalar(0,255,0);
			break;
		case 2:
			if(FIRE_DEC_ENABLE)
			    color=cvScalar(0,0,255);
			else
				color=cvScalar(0,255,0);

		}
		cvRectangle(pFeedBack,cvPoint(pRec[i].x,pRec[i].y),cvPoint(pRec[i].x+pRec[i].width,pRec[i].y+pRec[i].height),color,4);
	}

 // for(int i=0;i<blockRowNum*blockColumnNum;++i)
	//{
	//	if(blockMark[i]!=0)
	//	{
	//	CvScalar color;

	//	switch(blockMark[i])
	//	{
	//	case 1:
	//		color=cvScalar(0,255,0);
	//		break;
	//	case 2:
	//		if(FIRE_DEC_ENABLE)
	//		    color=cvScalar(0,0,255);
	//		else
	//			color=cvScalar(0,255,0);

	//	}
	//	cvRectangle(pFeedBack,cvPoint(blockSeq[i].x,blockSeq[i].y),cvPoint(blockSeq[i].x+blockSeq[i].width,blockSeq[i].y+blockSeq[i].height),color,4);
	//	}
	//}
	cvShowImage("Result",pFeedBack);
	/*pFeedBack->origin=1;*/
	cvWriteFrame(resultWriter,pFeedBack);
}


void InvFireDetector::keyMenu()
{
	int key=cvWaitKey(WAIT_TIME);
	if(key==32)
		isPause=!isPause;
	
	while(isPause)
	{
		key=cvWaitKey(WAIT_TIME);
		string temp;
		switch(key)
		{
		case 32:
			isPause=!isPause;
			break;
		case 's':
			char num[100];
			temp="experiment//src";
			itoa(frameNum,num,10);
			temp+=num;
			temp+=".bmp";
			cvSaveImage(temp.c_str(),pSrc);

			temp="experiment//result";
			itoa(frameNum,num,10);
			temp+=num;
			temp+=".bmp";
			cvSaveImage(temp.c_str(),pFeedBack);

			cvConvertScale(mSwaveletFilter->pEnergyImg,pEnergy,255,0);
			temp="experiment//energy";
			itoa(frameNum,num,10);
			temp+=num;
			temp+=".bmp";
			cvSaveImage(temp.c_str(),pEnergy);

			temp="experiment//foreground";
			itoa(frameNum,num,10);
			temp+=num;
			temp+=".bmp";
			cvSaveImage(temp.c_str(),mFrGrdExtract->pFrImg);

			temp="experiment//background";
			itoa(frameNum,num,10);
			temp+=num;
			temp+=".bmp";
			cvSaveImage(temp.c_str(),mFrGrdExtract->pBackImg);
			break;
		}
	}
}

void InvFireDetector::release()
{
	    mSwaveletFilter->release();
		mFrGrdExtract->release();
		mColorFilter->release();
		//delete[] pRec;
		//delete[] Mark;
		cvReleaseImage(&pSrc);
		cvReleaseMat(&pBackGround);
		cvReleaseMat(&pBackMat);

		cvReleaseVideoWriter(&resultWriter);
		cvReleaseImage(&pFeedBack);
		//cvReleaseVideoWriter(&energyWriter);
		//cvReleaseVideoWriter(&foreWriter);
		//cvReleaseVideoWriter(&backWriter);
}