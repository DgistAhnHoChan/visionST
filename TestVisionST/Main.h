#include <chrono>				// �ð� ����
#include <thread>				// ������
#include <iostream>				// ���

#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>

#include "../VstPleoraLib/VstPleoraInterface.h"

using namespace cv;
using namespace std;
using namespace chrono;

#define IMAGE_WIDTH 1280				// �̹��� ������
#define IMAGE_HEIGHT 720
#define BAYER_OVERHEADER 6				// Bayer
#define CAMERA_NUMBER 4					// ī�޶� ����

#define EXPOSURE_INIT_VALUE 480			// 480
#define GLOBAL_GAIN_INIT_VALUE 64		// 64
#define RED_GAIN_INIT_VALUE 64			// 64
#define GREEN_GAIN_INIT_VALUE (64*0.8)	// (64*0.8)
#define BLUE_GAIN_INIT_VALUE 64			// 64

Mat wholeImg;						// ��ü ����
Mat cameraImg[4];					// 
uchar *cameraBuf[4];				//
Mat imgColor[4];					//

int size;							// �̹��� ũ�� �޾ƿ��� �Լ�
bool isColor;						// ���α׷� ���� �� Bayer or Color �������ִ� �Լ� ( 0�̸� ���, 1�̸� �÷�)
int receiveImage;					// �޾ƿ� �̹����� ��� ��

int OnCameraConnect();				
void SeparationBuffer(uchar *buf, uchar **cameraBuf, int width, int height);
void getImage(void);

// �Ķ��Ÿ ����
void cis_mt9m025_control(char slave_addr, int sub_addr, int data);
int uart_cmd_3byte(uchar c, uchar addr, uchar data0, uchar data1);
void SetParam(int posExposure, int posGlobalGain, int posRedGain, int posGreenGain, int posBlueGain);