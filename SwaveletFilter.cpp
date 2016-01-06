#include "SwaveletFilter.h"
using namespace std;

SwaveletFilter::SwaveletFilter()
{
	pCurFrame=0;
	pFrameGray = NULL;
	pSwaveletMat=NULL;
	pBackground=NULL;
	showImg = NULL;

	rectSeq=NULL;
	Mark = NULL;

	SCALE = 0.1;

	hh=0;
	ll=0;
	hl=0;
	lh=0;
  
  blockSeq=NULL;
	blockMark=NULL;

	framecount=0;
}


void SwaveletFilter::init(IplImage* pFrame,const char* wavename,int layer)
{
	fout.open(DATA_NAME);
	pFrameGray = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
	pFrameMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC3);
	pBackground =cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
	pSwaveletMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
	pEnergyImg=cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);

	double scale = (double)1/(double)255;
	cvConvertScale(pFrame,pFrameMat,scale,0);
	cvCvtColor(pFrameMat,pFrameGray,CV_BGR2GRAY);
	getFilter(wavename);
	level=layer;

	WaveletAnalysis(pFrameGray,pBackground);
}

void SwaveletFilter::show()
{
	showImg = cvCloneImage(pCurFrame);
	for(int i=0;i<seqNum;i++)
		if(Mark[i] == 2)
		{
			cvRectangle(showImg,cvPoint(rectSeq[i].x,rectSeq[i].y),cvPoint(rectSeq[i].x+rectSeq[i].width,rectSeq[i].y+rectSeq[i].height),cvScalar(0,255,0),4);
		}
	cvShowImage("SwaveletFilter",showImg);
	cvReleaseImage(&showImg);
}

void SwaveletFilter::release()
{
	cvReleaseMat(&pFrameGray);
	cvReleaseMat(&pFrameMat);
	cvReleaseMat(&pBackground);
	cvReleaseMat(&pSwaveletMat);
	cvReleaseMat(&pEnergyImg);
	cvReleaseMat(&hh);
	cvReleaseMat(&lh);
	cvReleaseMat(&hl);
	cvReleaseMat(&ll);
}

void SwaveletFilter::nextImage(IplImage *pFrame)
{

	pCurFrame=pFrame;
	fireBlockNum=0;
	int count=0;
	//for(int i=0;i<blockRowNum*blockColumnNum;++i)
	//{
	//	if(blockMark[i] > 0)
	//	{count++;}		
	//}
	
	for(int i=0;i<seqNum;++i)
	{
		if(Mark[i]==1)
			count++;
	}
	if(count>0)

	{
		double scale = (double)1/(double)255;
		cvConvertScale(pFrame,pFrameMat,scale,0);
		cvCvtColor(pFrameMat,pFrameGray,CV_BGR2GRAY); 
		cvZero(pSwaveletMat);
		WaveletAnalysis(pFrameGray,pSwaveletMat);
		filter(pFrameGray,pBackground,rectSeq,seqNum);
		/*blockFilter(pFrameGray);*/
		double  min_val[100];
		double  max_val[100];
		cvMinMaxLoc(pSwaveletMat,min_val,max_val);
		cvConvertScale(pSwaveletMat,pEnergyImg,255/(max_val[0]-min_val[0]),-255/((max_val[0]-min_val[0]))*min_val[0]);

	}
	else
	{
		/*pBackground=this->pSwaveletMat;*/
	}
}

void SwaveletFilter::setRectSeq(CvRect* rectseq, int Num,int* rectMark)
{
	rectSeq = rectseq;
	seqNum = Num;
	Mark = rectMark;
}

void SwaveletFilter::setBlockVar(CvRect* blockseq,int* blockmark,int blockrownum,int blockcolumnnum)
{
	blockMark=blockmark;
	blockSeq=blockseq;
	blockRowNum=blockrownum;
	blockColumnNum=blockcolumnnum;
}

void SwaveletFilter::filter(CvMat *pFrameCur,CvMat *pBackground,CvRect *rectSeq,int rectNum)
{
			int threshold_wavelet = 10;
			
			float energycur;
			float energyfir;

			float energyCur;
			float energyThd;
			float energyVar;
			int step = 1;
			int width = 0, height = 0;
			int rectX = 0, rectY = 0;

			float maxratio=0;
			float minratio=10000000000;
			float temp;

			for(int count=0;count<rectNum;count++)
			{
				if(Mark[count] == 1)
				{
					rectX = rectSeq[count].x;
					rectY = rectSeq[count].y;
					width = rectSeq[count].width;
					height = rectSeq[count].height;

					energyCur=0;
					energyVar = 0;
					energyThd = 0;
					for(int i=rectY;i<=rectY+height-2;i+=step)
					{
						for(int j=rectX;j<=rectX+width-2;j+=step)
						{
							energycur = cvmGet(pSwaveletMat,i,j);
							energyfir = cvmGet(pBackground,i,j);
							energyThd += energyfir*energyfir;
							energyVar += (energycur-energyfir)*(energycur-energyfir);
							energyCur+=energycur*energycur;
						}

					}
					energyVar/=(width*height);
					energyThd/=(width*height);
					energyCur/=(width*height);
					temp=(energyVar/energyThd);
					maxratio=temp>maxratio?temp:maxratio;
					minratio=temp<minratio?temp:minratio;

					fout<<energyCur<<endl<<energyThd<<endl;
					if(energyVar > Q_HIGH*(energyThd) || energyVar < Q_LOW*(energyThd))
					{
						Mark[count] = 1;	
					}
					else
					{
						Mark[count] = 2;
						fireBlockNum++;
					}
				}
		}
			/*fout<<maxratio<<endl<<minratio<<endl;*/
}
void SwaveletFilter::blockFilter(CvMat *pFrameCur)
{
	// ¿ÕÓòÐ¡²¨·ÖÎö
			int threshold_wavelet = 10;
			
			float energycur;
			float energyfir;
			float energyThd;
			float energyVar;
			int step = 1;
			int width = 0, height = 0;
			int rectX = 0, rectY = 0;

			for(int count=0;count<blockRowNum*blockColumnNum;count++)
			{
				if(blockMark[count] == 1)
				{
					rectX = blockSeq[count].x;
					rectY = blockSeq[count].y;
					width = blockSeq[count].width;
					height = blockSeq[count].height;

					energyVar = 0;
					energyThd = 0;
					for(int i=rectY;i<=rectY+height-2;i+=step)
					{
						for(int j=rectX;j<=rectX+width-2;j+=step)
						{
							energycur = cvmGet(pSwaveletMat,i,j);
							energyfir = cvmGet(pBackground,i,j);
							energyThd += abs(energyfir)*abs(energyfir);
							energyVar += abs(energycur-energyfir)*abs(energycur-energyfir);
						}

					}
					fout<<energyVar<<endl<<energyThd<<endl;
					blockMark[count]=(energyVar > Q_HIGH*energyThd || energyVar < Q_LOW*energyThd)?1:2;
				}
		}
}
void SwaveletFilter::makeCore(float* hd,float* ld,int num)
{
	cvReleaseMat(&hh);
	cvReleaseMat(&ll);
	cvReleaseMat(&hl);
	cvReleaseMat(&lh);

	hh=cvCreateMat(num,num,CV_32FC1);
	ll=cvCreateMat(num,num,CV_32FC1);
	hl=cvCreateMat(num,num,CV_32FC1);
	lh=cvCreateMat(num,num,CV_32FC1);

	float* data_hh=hh->data.fl;
	float* data_ll=ll->data.fl;
	float* data_hl=hl->data.fl;
	float* data_lh=lh->data.fl;

	int step=hh->step/sizeof(float);
	for(int i=0;i<num;++i)
	{
		for(int j=0;j<num;++j)
		{
			data_hh[i*step+j]=hd[num-1-i]*hd[num-1-j];
			data_ll[i*step+j]=ld[num-1-i]*ld[num-1-i];
			data_hl[i*step+j]=ld[num-1-i]*hd[num-1-i];
			data_lh[i*step+j]=hd[num-1-i]*ld[num-1-i];
		}
	}
}

void SwaveletFilter::getFilter(const char* wavename)
{
	if(wavename=="haar")
	{
		float phd[2]={-0.707106781186548,0.707106781186548};
		float pld[2]={0.707106781186548,0.707106781186548};
		float phr[2]={0.707106781186548,-0.707106781186548};
		float plr[2]={0.707106781186548,0.707106781186548};
		makeCore(phd,pld,2);
	}
	else if(wavename=="db4")
	{
		float phd[8]={   -0.2304  ,  0.7148  , -0.6309 ,  -0.0280  ,  0.1870   , 0.0308  , -0.0329 ,  -0.0106};
		float pld[8]={   -0.0106  ,  0.0329 ,   0.0308  , -0.1870  , -0.0280   , 0.6309   , 0.7148,   0.2304};
		float phr[8]={   -0.0106  , -0.0329   , 0.0308 ,   0.1870  , -0.0280 ,  -0.6309   , 0.7148,   -0.2304};
		float plr[8]={    0.2304   , 0.7148  ,  0.6309  , -0.0280   ,-0.1870 ,   0.0308   , 0.0329,   -0.0106};
		makeCore(phd,pld,8);
	}
}
CvMat* SwaveletFilter::zeroPadding(CvMat* img,int left,int right,int top,int bottom)
{
	CvMat* dst=cvCreateMat(img->rows+top+bottom,img->cols+left+right,CV_32FC1);
	float* dst_data=dst->data.fl;
	float* src_data=img->data.fl;
	cvZero(dst);
	int dst_step=dst->step/sizeof(float);
	int src_step=img->step/sizeof(float);
	for(int i=0;i<img->rows;++i)
	{
		for(int j=0;j<img->cols;++j)
		{
			dst_data[(i+top)*dst_step+j+left]=src_data[i*src_step+j];
		}
	}
	return dst;
}
void SwaveletFilter::WaveletAnalysis(CvMat *pImage, CvMat* pMatenergy)
{
	int numShort=hh->cols;
	CvMat* cA=zeroPadding(pImage,numShort-1,numShort-1,numShort-1,numShort-1);
	

	int step=cA->step/sizeof(float);
	float* pdata=cA->data.fl;
	int row=0;
	int col=0;

	int coreStep=hh->step/sizeof(float);
	float* phh=hh->data.fl;
	float* phl=hl->data.fl;
	float* plh=lh->data.fl;

	CvMat* detail=cvCreateMat((cA->rows-numShort+2)/2,(cA->cols-numShort+2)/2,CV_32FC1);
	float* pdetail=detail->data.fl;
	int detailStep=detail->step/sizeof(float);
    
	float totleEnergy=0;

	for(int i=0;i<cA->height;++i)
	{
		for(int j=0;j<cA->width;++j)
		{
			totleEnergy+=pdata[i*step+j]*pdata[i*step+j];
		}
	}

	float cD=0,cV=0,cH=0;
	for(int i=0;i<cA->rows-numShort+1;i+=2)
	{
		++row;
		for(int j=0;j<cA->cols-numShort+1;j+=2)
		{
			++col;
			cD=0;
			cV=0;
			cH=0;
			for(int m=0;m<numShort;++m)
			{
				for(int n=0;n<numShort;++n)
				{
					cD+=phh[m*coreStep+n]*pdata[(i+m)*step+j+n];
					cV+=phl[m*coreStep+n]*pdata[(i+m)*step+j+n];
					cH+=plh[m*coreStep+n]*pdata[(i+m)*step+j+n];
				}
			}
			pdetail[(row-1)*detailStep+col-1]=(cD*cD+cV*cV+cH*cH);//totleEnergy;
		}
		col=0;
	}
    
	cvResize(detail,pMatenergy,CV_INTER_LINEAR);

	cvReleaseMat(&detail);
	cvReleaseMat(&cA);
}