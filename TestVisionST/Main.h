
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "../VstPleoraLib/VstPleoraInterface.h"

#include <chrono>	// 시간 측정
#include <thread>	// 스레드
#include <iostream> // 출력
#include <conio.h>	// 윈도우 키 입력 헤더

#include <opencv\cv.h>			// OPENCV
#include <opencv\highgui.h>		// OPENCV

using namespace cv;
using namespace std;
using namespace chrono;

#pragma warning(disable: 4996)	// fopen과 fopen_s와 같은 보안 경고
#pragma warning(disable: 4244)	// double -> int와 같은 데이터 손실 경고
#pragma warning(disable: 4090)	// 디버깅 시 편집과 동시에 하기 위한 경고 무시
#pragma warning(disable: 4819)	// 한글로 주석을 달 때 생길 수 있는 경고
#pragma warning(disable: 4819)	// 한글로 주석을 달 때 생길 수 있는 경고

#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 720
#define BAYER_OVERHEADER 6
#define CAMERA_NUMBER 2

#define EXPOSURE_INIT_VALUE 480 // 480
#define GLOBAL_GAIN_INIT_VALUE 64 // 64
#define RED_GAIN_INIT_VALUE 64 // 64
#define GREEN_GAIN_INIT_VALUE (64*0.8) // (64*0.8)
#define BLUE_GAIN_INIT_VALUE 64 // 64

Mat wholeImg;		// 전체 버퍼
Mat cameraImg[4];
uchar *cameraBuf[4];
Mat imgColor[4];		

#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 720
#define TIMER_AUTO_RECV 0x02 //영상 수신을 위한 Timer ID


int size;							// 이미지 크기 받아오는 함수
bool isColor;						// 프로그램 실행 시 Bayer or Color 설정해주는 함수 ( 0이면 흑백, 1이면 컬러)

int width =IMAGE_WIDTH;				// 이미지 가로
int height=IMAGE_HEIGHT;			// 이미지 세로

int receiveImage;					// 이미지 받아오고 if 문 설정해주기 위한 변수
bool stop_signal = false;			// 쓰레드의 while 문을 stop 시키위 위해 필요한 bool 변수


VideoWriter videoWrite;				// 녹화 라이브러리 변수
VideoWriter videoWrite2;

// 카메라 연결, 버퍼 나누기, 이미지 받아오는 함수, 연결 해제
int initCamera(void);		
void SeparationBuffer(uchar *buf, uchar *buf1, uchar *buf2, uchar *buf3, uchar *buf4, int width, int height);
void getImage(void);
void releaseBuf(void);

// 버퍼 쓰레드
void bufThread(void);
void bufThread2(void);

// 파라메타 설정
void cis_mt9m025_control(char slave_addr, int sub_addr, int data);
int uart_cmd_3byte(uchar c, uchar addr, uchar data0, uchar data1);
void SetExposure(int pos);
void SetGlobalGain(int pos);
void SetRedGain(int pos);
void SetGreenGain(int pos);
void SetBlueGain(int pos);







void SeparationBuffer2(uchar *buf, unsigned char *buf5, unsigned char *buf6, unsigned char *buf7, unsigned char *buf8, int width, int height);
void Bayer2BgrFree(unsigned char *buf);
unsigned char *Bayer2Bgr(unsigned char *src, int width, int height);