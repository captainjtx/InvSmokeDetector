#include"FrGrdExtract.h"

FrGrdExtract::FrGrdExtract()
{
	curImg = NULL;
	pFrImg = NULL; 
	pBackImg=NULL;
	showImg = NULL;

	rectSeq = NULL;
	seqNum = 0;
	Mark = NULL;

	blockMark=NULL;
	blockSeq=NULL;

	CvGaussBGModel* bg_model=NULL; //初始化高斯混合模型参数
}

void FrGrdExtract::init(IplImage* pFrame)
{
	pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U,1); 
	pBackImg= cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U,3);

	bg_model = (CvGaussBGModel*)cvCreateGaussianBGModel(pFrame, 0);
  
}

void FrGrdExtract::show()
{
	showImg = cvCloneImage(curImg);
	for(int i=0;i<seqNum;i++)
		if(Mark[i] == 1)
		{
			cvRectangle(showImg,cvPoint(rectSeq[i].x,rectSeq[i].y),cvPoint(rectSeq[i].x+rectSeq[i].width,rectSeq[i].y+rectSeq[i].height),cvScalar(0,255,0),4);
		}
	cvShowImage("FrGrdExtract",showImg);
	cvShowImage("Background",bg_model->background);
	cvReleaseImage(&showImg);
}

void FrGrdExtract::release()
{
	cvReleaseBGStatModel((CvBGStatModel**)&bg_model);
	cvReleaseImage(&pFrImg);
	cvReleaseImage(&pBackImg);
}

void FrGrdExtract::nextImage(IplImage* pFrame)
{
	curImg = pFrame;
	//更新高斯模型
	cvUpdateBGStatModel(curImg, (CvBGStatModel*)bg_model );
	updateBackground((CvGaussBGModel*)bg_model);  
	//pFrImg为前景图像，只能为单通道
	cvCopy(bg_model->foreground,pFrImg,0);
	cvCopy(bg_model->background,pBackImg,0);
	filter(pFrImg);
	/*segmentation(pFrImg);*/
}

void FrGrdExtract::setRectSeq(CvRect* rectseq,int *rectMark)
{
	rectSeq = rectseq;
	Mark = rectMark;
}

void FrGrdExtract::setBlockVar(CvRect* blockseq,int* blockmark,int blockrownum,int blockcolumnnum)
{
	blockMark=blockmark;
	blockSeq=blockseq;
	blockRowNum=blockrownum;
	blockColumnNum=blockcolumnnum;
}

void FrGrdExtract::filter(IplImage* pFrImg)
{
	CvSeq* contour = NULL;
	CvMemStorage* storage = cvCreateMemStorage(0);
	seqNum = 0;
	double minarea = pFrImg->width*pFrImg->height*BLOCK_SIZE;
	double maxarea = pFrImg->width*pFrImg->height/2;
	double tmparea = 0.0;
       
    IplImage* img_src = cvCloneImage(pFrImg);   
       
    //搜索二值图中的轮廓，并从轮廓树中删除面积小于某个阈值minarea的轮廓   
    CvContourScanner scanner = NULL;   
	scanner = cvStartFindContours(img_src,storage,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE,cvPoint(0,0));   
    //开始遍历轮廓树   
    CvRect rect;   
    while (contour=cvFindNextContour(scanner))   
    {   
        tmparea = fabs(cvContourArea(contour));   
            rect = cvBoundingRect(contour,0);      
        if (tmparea > minarea && tmparea< maxarea)   
        {   
            rectSeq[seqNum] = rect;
			Mark[seqNum] = 1;
			seqNum++;
			//cvRectangle(pFrImg,cvPoint(rect.x,rect.y),cvPoint(rect.x+rect.width,rect.y+rect.height),cvScalar(255,255,255),4);
		}
	}

	cvReleaseImage(&img_src);
	cvReleaseMemStorage(&storage);
}

void FrGrdExtract::segmentation(IplImage* pFrImg)
{
  seqNum=0;
  double ratio=0.1;
  
  int step       = pFrImg->widthStep;
  uchar* data    = (uchar *)pFrImg->imageData;

  for(int i=0;i<blockColumnNum*blockRowNum;++i)
  {
  	int x=blockSeq[i].x;
  	int y=blockSeq[i].y;
  	int width=blockSeq[i].width;
  	int height=blockSeq[i].height;
    int count=0;
     for(int m=y;m<y+height;++m)
     {
     	for(int n=x;n<x+width;++n)
     	{
     		count+=(data[m*step+n]>0?1:0);
     	}
     }
     blockMark[i]=(float)count/(float)(width*height)>ratio?1:0;
	 if(blockMark[i])
		 ++seqNum;
  }
}