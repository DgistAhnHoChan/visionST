#include "Main.h"
//#include "CameraParameter.h"

// ī�޶� �����ϱ� ���� �Լ� // ����, �Ķ��Ÿ �ʱ�ȭ
int OnCameraConnect()						
{
	void* m_hwnd = NULL;					// ������ �ڵ鷯 ��
	int ret = 0;							// return ��

	isColor = true;							// Bayer = 0, Color = 1;
	ret = vstpleoraInitialize(m_hwnd);		// ī�޶� ����

	wholeImg = Mat(IMAGE_HEIGHT, IMAGE_WIDTH*BAYER_OVERHEADER, CV_8UC1);			// ��ü�� ���Ź��� ���� �ʱ�ȭ
	
	for(int idx = 0; idx < 4; idx++)
	{
		imgColor[idx] = Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3);		
		cameraImg[idx] = Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
		cameraBuf[idx] = cameraImg[idx].data;
	}

	// �Ķ��Ÿ �� ����
	SetExposure(EXPOSURE_INIT_VALUE);		// SetExposure, Gain �� ���� (���)
	SetGlobalGain(GLOBAL_GAIN_INIT_VALUE);
	SetRedGain(RED_GAIN_INIT_VALUE);
	SetGreenGain(GREEN_GAIN_INIT_VALUE);
	SetBlueGain(BLUE_GAIN_INIT_VALUE);

	vstpleoraGetImageSize(&size);											// �̹��� ������
	receiveImage = vstpleoraGetImage(wholeImg.data, size);					// �̹��� �޾ƿ���
	videoWrite.open("windows1.avi" , CV_FOURCC('X', 'V', 'I', 'D'), 30, Size(IMAGE_WIDTH, IMAGE_HEIGHT), true); // ������ �����ϴ� lib
	return ret;								// ī�޶� ���� �� ���� (����Ǿ����� 1, �ƴϸ� 0)
}

// �Ķ��Ÿ 
void SetExposure(int pos)
{	
	cis_mt9m025_control(MT9M025_SLAVE_ADDR, CIS_EXPOSURE_ADDR, 0xFFFF & (pos>>0));
	return;
}

void SetGlobalGain(int pos)
{	
	cis_mt9m025_control(MT9M025_SLAVE_ADDR, CIS_GAIN_ADDR, 0xFFFF & (pos>>0));
	return;
}

void SetRedGain(int pos)
{	
	cis_mt9m025_control(MT9M025_SLAVE_ADDR, CIS_RED_ADDR, 0xFFFF & (pos>>0));
	return;
}

void SetGreenGain(int pos)
{
	cis_mt9m025_control(MT9M025_SLAVE_ADDR, CIS_GREEN1_ADDR, 0xFFFF & (pos>>0));
	cis_mt9m025_control(MT9M025_SLAVE_ADDR, CIS_GREEN2_ADDR, 0xFFFF & (pos>>0));
	return;
}

void SetBlueGain(int pos)
{	
	cis_mt9m025_control(MT9M025_SLAVE_ADDR, CIS_BLUE_ADDR, 0xFFFF & (pos>>0));
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

	int xrow = 0;
	int singleStep = 0;
	int wholeStep = 0;
	
	for(int y=0; y<height; y++)		// �̹��� ���� ��ŭ �ݺ�
	{
		singleStep = y * width;		// �̹����� ���� 
		wholeStep = y * 7680;		// ���� ������ ��ü ��
		for(int xx=0; xx<width; xx++)	// �̹��� �� ��ŭ �ݺ�
		{		
			xrow=xx*6;			// ī�޶� �� �ε���
			
			cameraBuf[0][singleStep + xx] = buf[wholeStep + (xrow+0)];			// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
			cameraBuf[1][singleStep + xx] = buf[wholeStep + (xrow+2)];			// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
			cameraBuf[2][singleStep + xx] = buf[wholeStep + (xrow+3)];			// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
			cameraBuf[3][singleStep + xx] = buf[wholeStep + (xrow+5)];			// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
		}	// end of for(int xx=0; xx<width; xx++)
	}	// end of for(int y=0; y<height; y++)
	
} // end of void SeparationBuffer

void getImage()
{
	vstpleoraGetImageSize(&size);											// �̹��� ������
	receiveImage = vstpleoraGetImage(wholeImg.data, size);					// �̹��� �޾ƿ���
	if( receiveImage != 1 ) return;
		
	SeparationBuffer(wholeImg.data, cameraBuf, width, height);	// ���� ������
	if( isColor ) 
	{
		for(int idx = 0; idx < 4; idx++)
			cvtColor( cameraImg[idx], imgColor[idx], CV_BayerGB2BGR );	// Opencv�� �÷� ���� ��ȯ�� ���
	}
}

// ���� �Լ�
int main(void)
{
	char chKey = 0;
	int ret = 0;										// ī�޶� ������ ���� ��
	puts("Bayer : B , Color : C\n");
	puts("Connecting... ");
	ret = OnCameraConnect();							// ī�޶� ����, �Ķ��Ÿ ����, ���� ä���
	
	if(ret != 1) 
	{
		puts("Connect fail");
		return -1;									// ���� ���� ��
	}
	
	puts("Camera Connect\n");
	puts("Press 'Esc' to exit ");
	stop_signal = false;							// ������ ���� ��ȣ
	
	while(1)
	{
		system_clock::time_point startTime = system_clock::now();
		getImage();
		system_clock::time_point endTime = system_clock::now();
		milliseconds milli2 = duration_cast<milliseconds>(endTime-startTime);
		std::cout << "Processing Time : " << milli2.count()  << " ms " << endl;

		if( !isColor ) 
		{
			imshow("Camera1", cameraImg[0]);
			imshow("Camera2", cameraImg[1]);
			imshow("Camera3", cameraImg[2]);
			imshow("Camera4", cameraImg[3]);
		}
		else
		{
			imshow("Camera1", imgColor[0]);
			imshow("Camera2", imgColor[1]);
			imshow("Camera3", imgColor[2]);
			imshow("Camera4", imgColor[3]);
		}
		chKey = waitKey(8);
		if( chKey == 27 ) break;
		else if( chKey == 'c' )		isColor = true;
		else if( chKey == 'b' )		isColor = false;
	}
	
	if(videoWrite.isOpened()) videoWrite.release();
	vstpleoraDestroy();								// ī�޶� ���� ����
	return 0;
}

//	�ð� ���ϱ�, Main.h ���Ͽ� �ּ�ó�� �Ǿ� ����
//	startTime = system_clock::now();
//	endTime = system_clock::now();
//	if(!flag)
//	{
//		nanoseconds milli2 = duration_cast<nanoseconds>(endTime2-startTime2);
//		std::cout << "�̹��� �޾ƿ��� �ð� : " << milli2.count() << endl;
//		flag = true;
//	}