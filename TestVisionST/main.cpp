#include "Main.h"

// ī�޶� �����ϱ� ���� �Լ� // ����, �Ķ��Ÿ �ʱ�ȭ
int OnCameraConnect()						
{
	size = IMAGE_HEIGHT * IMAGE_WIDTH * 6;	// �̹��� ������ 1280 * 720 * 6
	void* m_hwnd = NULL;					// vstpleoraInitialize()�� ������ �ڵ鷯 ��
	int ret = 0;							// vstpleoraInitialize()�� ���� ��

	isColor = true;							// Bayer = 0, Color = 1;
	ret = vstpleoraInitialize(m_hwnd);		// ī�޶� ���� (������ �����̸� 1, �����ϸ� 0 ���)
											// MFC ȯ���� �Լ�, m_hwnd ������ NULL ���� ���Ƿ� �Է�

	wholeImg = Mat(IMAGE_HEIGHT, IMAGE_WIDTH*BAYER_OVERHEADER, CV_8UC1);	// ��ü�� ���Ź��� ���� �ʱ�ȭ
																			// 720, 1280*6, 8 bit unsigned 1ä�� ���
																			// ��, �̹��� ������ ��(1280*6)*��(720)
	
	for(int idx = 0; idx < CAMERA_NUMBER; idx++)							// Bayer, color ���� �ʱ�ȭ
	{
		imgColor[idx] = Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3);			// Color ī�޶� ���� �ʱ�ȭ
		cameraImg[idx] = Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);			// Bayer ī�޶� ���� �ʱ�ȭ
		cameraBuf[idx] = cameraImg[idx].data;								// ����� ������
	}

	// �Ķ��Ÿ �� ����
	SetParam(EXPOSURE_INIT_VALUE, GLOBAL_GAIN_INIT_VALUE, RED_GAIN_INIT_VALUE, GREEN_GAIN_INIT_VALUE, BLUE_GAIN_INIT_VALUE);	// SetExposure, Gain �� ���� (���)
																			// �Ķ��Ÿ �⺻ ���� ���� 480, 64, 64, (64*0.8), 64 �� �������
	vstpleoraGetImageSize(&size);											// �̹��� ������ �޾ƿ���
	receiveImage = vstpleoraGetImage(wholeImg.data, size);					// �̹��� �޾ƿ��� (lib �Լ�)
	return ret;																// ī�޶� ���� �� ���� (����Ǿ����� 1, �ƴϸ� 0)
}

// �Ķ��Ÿ
void SetParam(int posExposure, int posGlobalGain, int posRedGain, int posGreenGain, int posBlueGain)
{	
	cis_mt9m025_control(MT9M025_SLAVE_ADDR, CIS_EXPOSURE_ADDR, 0xFFFF & (posExposure>>0));	// SetExposure
	cis_mt9m025_control(MT9M025_SLAVE_ADDR, CIS_GAIN_ADDR, 0xFFFF & (posGlobalGain>>0));	// SetGlobalGain
	cis_mt9m025_control(MT9M025_SLAVE_ADDR, CIS_RED_ADDR, 0xFFFF & (posRedGain>>0));		// SetRedGain
	cis_mt9m025_control(MT9M025_SLAVE_ADDR, CIS_GREEN1_ADDR, 0xFFFF & (posGreenGain>>0));	// SetGreenGain
	cis_mt9m025_control(MT9M025_SLAVE_ADDR, CIS_GREEN2_ADDR, 0xFFFF & (posGreenGain>>0));
	cis_mt9m025_control(MT9M025_SLAVE_ADDR, CIS_BLUE_ADDR, 0xFFFF & (posBlueGain>>0));		// SetBlueGain
	return;
}

void cis_mt9m025_control(char slave_addr, int sub_addr, int data)
{
	uart_cmd_3byte('s', 0x00, 0x00, slave_addr);
	uart_cmd_3byte('l', 0x00, (sub_addr>>8)&0xFF, (sub_addr>>0)&0xFF);
	uart_cmd_3byte('m', 0x00, (data>>8)&0xFF, (data>>0)&0xFF);
} 

int uart_cmd_3byte(uchar c, uchar addr, uchar data0, uchar data1)
{
	uchar cmd[9];
	cmd[0]= 0x81;
	cmd[1]= c;
	cmd[2]= 0x0F & (addr>>4);
	cmd[3]= 0x0F & (addr>>0);
	cmd[4]= 0x0F & (data0>>4);
	cmd[5]= 0x0F & (data0>>0);
	cmd[6]= 0x0F & (data1>>4);
	cmd[7]= 0x0F & (data1>>0);
	cmd[8]= 0x82;
	int ret = vstpleoraSendSerialPort(cmd, 9);
	return 0;
}

// ���� ������
void SeparationBuffer(uchar *buf, uchar **cameraBuf, int width, int height)
{
	// �� �ε���
	int xrow = 0;						// �̹��� ���� �ε���
	int singleStep = 0;					// �̹����� ��
	int wholeStep = 0;					// ���� ������ ��ü ��
	
	for(int yy=0; yy<height; yy++)		// �̹��� ���� ��ŭ �ݺ�
	{
		singleStep = yy * width;		// �̹����� ��
		wholeStep = yy * 7680;			// ���� ������ ��ü ��
		for(int xx=0; xx<width; xx++)	// �̹��� �� ��ŭ �ݺ�
		{		 
			xrow=xx*6;					// ī�޶� �� �ε���
			cameraBuf[0][singleStep + xx] = buf[wholeStep + (xrow+0)];		// ī�޶� 1���� �̹����� ī�޶� 1�� ���ۿ� ����
			cameraBuf[1][singleStep + xx] = buf[wholeStep + (xrow+2)];		// ī�޶� 2���� �̹����� ī�޶� 2�� ���ۿ� ����	
			cameraBuf[2][singleStep + xx] = buf[wholeStep + (xrow+3)];		// ī�޶� 3���� �̹����� ī�޶� 3�� ���ۿ� ����	
			cameraBuf[3][singleStep + xx] = buf[wholeStep + (xrow+5)];		// ī�޶� 4���� �̹����� ī�޶� 4�� ���ۿ� ����	
		}	// end of for(int xx=0; xx<width; xx++)
	}	// end of for(int y=0; y<height; y++)
	
} // end of void SeparationBuffer

// �̹��� �޾ƿ��� �Լ�
void getImage()
{
	vstpleoraGetImageSize(&size);											// �̹��� ������
	receiveImage = vstpleoraGetImage(wholeImg.data, size);					// �̹��� �޾ƿ���
	waitKey(5);																// �̹��� ������ ���� -> ���ð��� �������� ������ �̹��� ���� ���� �߻�
	if( receiveImage != 1 ) return;											// �̹��� ���� ���ϸ� ����
	SeparationBuffer(wholeImg.data, cameraBuf, IMAGE_WIDTH, IMAGE_HEIGHT);	// ���� ������
	if( isColor )															// �÷� �̹��� �����̸�
	{
		for(int idx = 0; idx < CAMERA_NUMBER; idx++)						// 4���� ī�޶� ������ŭ �ݺ�
			cvtColor( cameraImg[idx], imgColor[idx], CV_BayerGB2BGR );		// Opencv�� �÷� ���� ��ȯ�� ���
																			// bayer ������ ������, color ���۷� �ű��, Bayer to Color ó��
	}
}

// ���� �Լ�
int main(void)
{
	char chKey = 0;	
	int ret = 0;										// ī�޶� ������ ���� ��
	puts("Bayer : B , Color : C\n Connecting... ");		// B �Է�: Bayer ��, C �Է�: Color �� 
	
	ret = OnCameraConnect();							// ī�޶� ����, �Ķ��Ÿ ����, ���� ä���
	if(ret != 1)										// ���� ���� �� (1 �� �ƴ� ��)
	{
		puts("Connect fail");							// Connect fail ���
		return -1;										
	}

	puts("Camera Connect\nPress 'Esc' to exit");
	
	while(1)
	{																			// Processing Time üũ
		system_clock::time_point startTime = system_clock::now();				// ���� �ð� üũ
		getImage();																// �̹��� �޾ƿ��� �Լ�
		system_clock::time_point endTime = system_clock::now();					// ���� �ð� üũ
		milliseconds milli2 = duration_cast<milliseconds>(endTime-startTime);	// ���� �ð� - ���� �ð�
		std::cout << "Processing Time : " << milli2.count()  << " ms " << endl; // ���μ��� �ð� üũ (ms)

		if( !isColor )									// Bayer ó��
		{
			imshow("Camera1", cameraImg[0]);			// 1�� ī�޶� ȭ�鿡 ��� (bayer)
			imshow("Camera2", cameraImg[1]);			// 2�� ī�޶� ȭ�鿡 ��� (bayer)
			imshow("Camera3", cameraImg[2]);			// 3�� ī�޶� ȭ�鿡 ��� (bayer)
			imshow("Camera4", cameraImg[3]);			// 4�� ī�޶� ȭ�鿡 ��� (bayer)
		}
		else											// Color ó��
		{
			imshow("Camera1", imgColor[0]);				// 1�� ī�޶� ȭ�鿡 ��� (color)
			imshow("Camera2", imgColor[1]);				// 2�� ī�޶� ȭ�鿡 ��� (color)
			imshow("Camera3", imgColor[2]);				// 3�� ī�޶� ȭ�鿡 ��� (color)
			imshow("Camera4", imgColor[3]);				// 4�� ī�޶� ȭ�鿡 ��� (color)	
		}
		chKey = waitKey(10);							// 10ms ���
		if( chKey == 27 ) break;						// Esc ������ break
		else if( chKey == 'c' )		isColor = true;		// c ������ color
		else if( chKey == 'b' )		isColor = false;	// b ������ bayer
	}
	vstpleoraDestroy();									// ī�޶� ���� ����
	return 0;
}