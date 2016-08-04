#include <chrono>				// 시간 측정
#include <thread>				// 스레드
#include <iostream>				// 출력

#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>

#include "../VstPleoraLib/VstPleoraInterface.h"

using namespace cv;
using namespace std;
using namespace chrono;

#define IMAGE_WIDTH 1280				// 이미지 사이즈
#define IMAGE_HEIGHT 720
#define BAYER_OVERHEADER 6				// Bayer
#define CAMERA_NUMBER 4					// 카메라 갯수

#define EXPOSURE_INIT_VALUE 480			// 480
#define GLOBAL_GAIN_INIT_VALUE 64		// 64
#define RED_GAIN_INIT_VALUE 64			// 64
#define GREEN_GAIN_INIT_VALUE (64*0.8)	// (64*0.8)
#define BLUE_GAIN_INIT_VALUE 64			// 64

Mat wholeImg;						// 전체 버퍼
Mat cameraImg[4];					// 
uchar *cameraBuf[4];				//
Mat imgColor[4];					//

int size;							// 이미지 크기 받아오는 함수
bool isColor;						// 프로그램 실행 시 Bayer or Color 설정해주는 함수 ( 0이면 흑백, 1이면 컬러)
int receiveImage;					// 받아온 이미지의 출력 값

int OnCameraConnect();				
void SeparationBuffer(uchar *buf, uchar **cameraBuf, int width, int height);
void getImage(void);

// 파라메타 설정
void cis_mt9m025_control(char slave_addr, int sub_addr, int data);
int uart_cmd_3byte(uchar c, uchar addr, uchar data0, uchar data1);
void SetParam(int posExposure, int posGlobalGain, int posRedGain, int posGreenGain, int posBlueGain);