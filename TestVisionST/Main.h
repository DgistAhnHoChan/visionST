
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "../VstPleoraLib/VstPleoraInterface.h"

#include <chrono>	// �ð� ����
#include <thread>	// ������
#include <iostream> // ���
#include <conio.h>	// ������ Ű �Է� ���

#include <opencv\cv.h>			// OPENCV
#include <opencv\highgui.h>		// OPENCV

using namespace cv;
using namespace std;
using namespace chrono;

#pragma warning(disable: 4996)	// fopen�� fopen_s�� ���� ���� ���
#pragma warning(disable: 4244)	// double -> int�� ���� ������ �ս� ���
#pragma warning(disable: 4090)	// ����� �� ������ ���ÿ� �ϱ� ���� ��� ����
#pragma warning(disable: 4819)	// �ѱ۷� �ּ��� �� �� ���� �� �ִ� ���
#pragma warning(disable: 4819)	// �ѱ۷� �ּ��� �� �� ���� �� �ִ� ���

#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 720
#define BAYER_OVERHEADER 6
#define CAMERA_NUMBER 2

#define EXPOSURE_INIT_VALUE 480 // 480
#define GLOBAL_GAIN_INIT_VALUE 64 // 64
#define RED_GAIN_INIT_VALUE 64 // 64
#define GREEN_GAIN_INIT_VALUE (64*0.8) // (64*0.8)
#define BLUE_GAIN_INIT_VALUE 64 // 64

Mat wholeImg;		// ��ü ����
Mat cameraImg[4];
uchar *cameraBuf[4];
Mat imgColor[4];		

#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 720
#define TIMER_AUTO_RECV 0x02 //���� ������ ���� Timer ID


int size;							// �̹��� ũ�� �޾ƿ��� �Լ�
bool isColor;						// ���α׷� ���� �� Bayer or Color �������ִ� �Լ� ( 0�̸� ���, 1�̸� �÷�)

int width =IMAGE_WIDTH;				// �̹��� ����
int height=IMAGE_HEIGHT;			// �̹��� ����

int receiveImage;					// �̹��� �޾ƿ��� if �� �������ֱ� ���� ����
bool stop_signal = false;			// �������� while ���� stop ��Ű�� ���� �ʿ��� bool ����


VideoWriter videoWrite;				// ��ȭ ���̺귯�� ����
VideoWriter videoWrite2;

// ī�޶� ����, ���� ������, �̹��� �޾ƿ��� �Լ�, ���� ����
int initCamera(void);		
void SeparationBuffer(uchar *buf, uchar *buf1, uchar *buf2, uchar *buf3, uchar *buf4, int width, int height);
void getImage(void);
void releaseBuf(void);

// ���� ������
void bufThread(void);
void bufThread2(void);

// �Ķ��Ÿ ����
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