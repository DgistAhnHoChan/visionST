
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
#define CAMERA_NUMBER 2

#define EXPOSURE_INIT_VALUE 480 // 480
#define GLOBAL_GAIN_INIT_VALUE 64 // 64
#define RED_GAIN_INIT_VALUE 64 // 64
#define GREEN_GAIN_INIT_VALUE (64*0.8) // (64*0.8)
#define BLUE_GAIN_INIT_VALUE 64 // 64

unsigned char *Buf;		// ��ü ����
unsigned char *Buf1;	// ä�� 1�� ����
unsigned char *Buf2;	// ä�� 2�� ����
unsigned char *Buf3;	// ä�� 3�� ����
unsigned char *Buf4;	// ä�� 4�� ����
unsigned char *Buf11;	// �� ä���� �÷� ��ȯ(Bayer -> RGB)�� ���� ����
unsigned char *Buf12;
unsigned char *bufTemp; // �����忡 �ֱ� ���� �ӽ� ����


#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 720
#define TIMER_AUTO_RECV 0x02 //���� ������ ���� Timer ID


int size;							// �̹��� ũ�� �޾ƿ��� �Լ�
int ViewColor;						// ���α׷� ���� �� Bayer or Color �������ִ� �Լ� ( 0�̸� ���, 1�̸� �÷�)

int width =IMAGE_WIDTH;				// �̹��� ����
int height=IMAGE_HEIGHT;			// �̹��� ����

int receiveImage;					// �̹��� �޾ƿ��� if �� �������ֱ� ���� ����
bool onImageCheck = false;			// ������� getImage ���� ���� ������ ���� �ʿ��� bool ����
bool stop_signal = false;			// �������� while ���� stop ��Ű�� ���� �ʿ��� bool ����


VideoWriter videoWrite;				// ��ȭ ���̺귯�� ����
VideoWriter videoWrite2;

// ī�޶� ����, ���� ������, �̹��� �޾ƿ��� �Լ�
int OnCameraConnect(void);	
void SeparationBuffer(unsigned char *buf, unsigned char *buf1, unsigned char *buf2, unsigned char *buf3, unsigned char *buf4, int width, int height);
void getImage(void);

// ���� ������
void bufThread(void);
void bufThread2(void);

// Bayer ó��, ���� ����
unsigned char *Bayer2Bgr(unsigned char *src, int width, int height);
unsigned char *Bayer2Bgr2(unsigned char *src, int width, int height);
void Bayer2BgrFree(unsigned char *buf);

// ������ üũ
void OnViewFramerate(void);

// �Ķ��Ÿ ����
void cis_mt9m025_control(char slave_addr, int sub_addr, int data);
int uart_cmd_3byte(unsigned char c, unsigned char addr, unsigned char data0, unsigned char data1);
void SetExposure(int pos);
void SetGlobalGain(int pos);
void SetRedGain(int pos);
void SetGreenGain(int pos);
void SetBlueGain(int pos);