#ifndef MY_GMM_BACKGROUND_H
#define MY_GMM_BACKGROUND_H

#include<stdio.h>  
#include<cv.h>  
#include<cxcore.h>  
#include<highgui.h>  
#include<cvaux.h>  

typedef struct MyCvGaussBGValues  
{  
    float match_sum;  
    float weight;  
    float mean[3];  
    float variance[3];  
}  
MyCvGaussBGValues;  
  
static void updateBackground(CvGaussBGModel* bg_model){  
    int K = bg_model->params.n_gauss;  
    int nchannels = bg_model->background->nChannels;  
    int height = bg_model->background->height;  
    int width = bg_model->background->width;  
    MyCvGaussBGValues *g_point = (MyCvGaussBGValues *) ((CvMat*)(bg_model->g_point))->data.ptr;  
    MyCvGaussBGValues *mptr = g_point;  
  
    for(int y=0; y<height; y++){  
        for (int x=0; x<width; x++, mptr+=K){  
            int pos = bg_model->background->widthStep*y + x*nchannels;  
            float mean[3] = {0.0, 0.0, 0.0};  
  
            for(int k=0; k<K; k++){  
                for(int m=0; m<nchannels; m++){  
                    mean[m] += mptr[k].weight * mptr[k].mean[m];  
                }  
            }  
  
            for(int m=0; m<nchannels; m++){  
                bg_model->background->imageData[pos+m] = (uchar) (mean[m]+0.5);  
            }  
        }  
    }  
}  

#endif