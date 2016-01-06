#ifndef CONSTANTS_H
#define CONStANTS_H

#define MOVING_NUM                30
#define REC_RATIO                 10                           
#define MAX_OF_BLOCK              30
#define WAVE_NAME                 "haar"

#define MAX_OF_INT               2000000000

//火灾检测算法使能
#define FIRE_DEC_ENABLE           1

//背景细节的更新时间间隔
#define BK_NUM                   10

//自适应减采样帧画面的大小
#define ADJUST_WIDTH             440
#define ADJUST_HEIGHT            320

//播放效果的时间间隔
#define WAIT_TIME                10

//视频结果输出

#define OUTPUT_FPS               10

#define RESULT_NAME              "experiment\\result.avi"
#define ENERGY_NAME              "experiment\\energy.avi"
#define FORE_NAME                "experiment\\fore.avi"
#define BACK_NAME                "experiment\\back.avi"
#define DATA_NAME                "experiment\\data.txt"

#define Q_LOW                    0.2             
#define Q_HIGH                   0.5

#define FRAME_CUM                1
#define FIRE_BLOCK_CUM           1       
#define FRAME_CAL                1        //period of calculation of frames

#define VIDEO_NAME                "sample\\sMoky.avi "

#define BLOCK_SIZE               1/1000

//sBtFence2.avi
//sBehindtheFence.avi
//CarLights1.avi
//CarLights2.avi
//ISYAM_Smoke_Output.avi
//sParkingLot.avi
//sWasteBasket.avi
//sWindow.avi
//person_smoke.avi
//sMoky.avi
//cross.avi
#endif