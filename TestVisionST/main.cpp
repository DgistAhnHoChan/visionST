#include "Main.h"
//#include "CameraParameter.h"

// ī�޶� �����ϱ� ���� �Լ� // ����, �Ķ��Ÿ �ʱ�ȭ
int OnCameraConnect()						
{
	void* m_hwnd = NULL;					// ������ �ڵ鷯 ��
	int ret = 0;							// return ��

	ViewColor = 0;							// Bayer = 0, Color = 1;
	ret = vstpleoraInitialize(m_hwnd);		// ī�޶� ����

	size = height*width*6;					// �̹��� ������ �ʱ�ȭ

	Buf = (uchar *)malloc(size);			// ��ü�� ���Ź��� ���� �ʱ�ȭ
	Buf1 = (uchar *)malloc(height*width);	// ī�޶� 1�� ���� �ʱ�ȭ
	Buf2 = (uchar *)malloc(height*width);	// ī�޶� 2�� ���� �ʱ�ȭ
	Buf3 = (uchar *)malloc(height*width);	// ī�޶� 3�� ���� �ʱ�ȭ
	Buf4 = (uchar *)malloc(height*width);	// ī�޶� 4�� ���� �ʱ�ȭ

	// �Ķ��Ÿ �� ����
	SetExposure(EXPOSURE_INIT_VALUE);		// SetExposure, Gain �� ���� (���)
	SetGlobalGain(GLOBAL_GAIN_INIT_VALUE);
	SetRedGain(RED_GAIN_INIT_VALUE);
	SetGreenGain(GREEN_GAIN_INIT_VALUE);
	SetBlueGain(BLUE_GAIN_INIT_VALUE);

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
	//Sleep(20);
	uart_cmd_3byte('m', 0x00, (data>>8)&0xFF, (data>>0)&0xFF);
	//Sleep(20);
} 

int uart_cmd_3byte(unsigned char c, unsigned char addr, unsigned char data0, unsigned char data1)
{
	unsigned char cmd[9];
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
	if(ret<9){	
	}

	//Sleep(20);
	return 0;
}

// ���� ������
void SeparationBuffer(unsigned char *buf, unsigned char *buf1, unsigned char *buf2, unsigned char *buf3, unsigned char *buf4, int width, int height)
{
	// ���� ������ ��ü ��
	int step = 7680;

	// xy��ǥ
	int y, x;

	// �� �ε���
	int row=0;
	int xrow=0;
	int xstep=0;

	// ī�޶�ID �� �ε���
	int cam1=0;	
	int cam2=2;
	int cam3=3;
	int cam4=5;
	unsigned char tmp;			// �ӽ� �ȼ� ����
	for(y=0; y<height; y++)		// �̹��� ���� ��ŭ �ݺ�
	{
		row = y * width;	// �̹����� ���� 
		for(x=0; x<width; x++)	// �̹��� �� ��ŭ �ݺ�
		{		
			xrow=x*6;			// ī�޶� �� �ε���
			xstep=x;			// ȭ�Һ� �� �ε���
			buf1[row + xstep+0] = buf[y*step + (xrow+cam1)];			// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
			buf2[row + xstep+0] = tmp=buf[y*step + (xrow+cam2)];		// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
			buf3[row + xstep+0] = tmp = buf[y*step + (xrow+cam3)];		// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
			buf4[row + xstep+0] = buf[y*step + (xrow+cam4)];			// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
		}
	}
}

void getImage()
{
	vstpleoraGetImageSize(&size);											// �̹��� ������
	receiveImage = vstpleoraGetImage(Buf, size);							// �̹��� �޾ƿ���

	if( onImageCheck == true )												// �����尡 �̹����� ��� ó���ϸ� true
	{
		if( receiveImage == 1 )												// �̹����� �޾ƿ��� ���� ������
		{
			SeparationBuffer(Buf, Buf1, Buf2, Buf3, Buf4, width, height);	// ���� ������
		}
		else																// �̹����� �޾ƿ��� ������ �� ���� �ʱ�ȭ
		{
			memset(Buf1, 0x00, height*width); 
			memset(Buf2, 0x00, height*width);
			memset(Buf3, 0x00, height*width);
			memset(Buf4, 0x00, height*width);
		}
		onImageCheck = false;												// �ٽ� false�� �����ؼ� �̹��� ��� ó�� �� �� ���� ���
	}
	else																	// �����尡 �̹��� ����� ó������ ���¶�� ���
	{
		this_thread::sleep_for(milliseconds(1));							// ������� ���ϸ� ���
	}
}

// ���� ������
void bufThread()
{

	Mat imgColor(height, width, CV_8UC3);		
	bool flag2 = false;
	while( stop_signal == false )									// ���� ��ȣ�� ������� �ݺ�
	{
		if( receiveImage == 1 )										// �̹����� ���������� �޾ƿ���
		{
			Mat camera1(height, width, CV_8UC1, Buf1);		
			if( ViewColor==0 )										// ��� ó��
			{
				
				cv::imshow("windows1", camera1);					// ȭ�鿡 ���̱�
				onImageCheck = true;								// �̹��� ��������� �˸�
			}
			else if( ViewColor==1 )									// �÷� ó��
			{
				cvtColor( camera1, imgColor, CV_BayerGB2BGR );	// Opencv�� �÷� ���� ��ȯ�� ���
				cv::imshow("windows1", imgColor);					// ȭ�鿡 ���̱�
				onImageCheck = true;								// �̹��� ��������� �˸�


				videoWrite << imgColor;								// ��ȭ ���
				if( flag2 == false )								// ��ȭ ����� �Լ��� �ѹ��� �������ָ� ó�� ��
				{   
					videoWrite.open("windows1.avi" , CV_FOURCC('X', 'V', 'I', 'D'), 30, Size(width, height), true); // ������ �����ϴ� lib
					flag2 = true;
				}
			}
		}
		else														// �̹����� ���������� �޾ƿ��� ���ϸ�
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			puts("Not read data");								
		}

		char chKey = waitKey(5);

		if( chKey == 'q' )				// q �Է�
		{
			if(ViewColor==1)
				videoWrite.release();
			break;
		}
		else if( chKey == 'c' )			// c �Է�
			ViewColor=1;
		else if( chKey == 'b' )			// b �Է�
		{
			videoWrite.release();		// ������ ���� ����
			ViewColor=0;
		}
	}
}

// ���� �Լ�
int main(void)
{
	int ret = 0;										// ī�޶� ������ ���� ��
	stop_signal = false;								// ������ ���� ��ȣ

	puts("Bayer : B , Color : C ");
	puts("");

	puts("Connecting... ");
	ret = OnCameraConnect();							// ī�޶� ����, �Ķ��Ÿ ����, ���� ä���
	
	if(ret == 1)										// ���� ������ �۾� ����
	{
		puts("Camera Connect");
		puts("");

		getImage();										// �ʱ� �̹��� �� �ޱ�

		thread threadCamera(bufThread);					// ù��° ī�޶� ���� ������

		puts("Press 'q' to exit ");

		while(1)
		{
			getImage();

			if( _kbhit() )								// Ű���� q ������ �ݺ��� ����
			{
				if(getch() == 'q')
					break;
			}
		}
	
		stop_signal = true;								// Thread ����

		//Join
		threadCamera.join();									// �����尡 ������ ����� �� ���� ���
		free(Buf);										// ���� ����
		free(Buf1);
		free(Buf2);
		free(Buf3);
		free(Buf4);

		vstpleoraDestroy();								// ī�޶� ���� ����
		puts("");
		puts("Camera disconnect");
		
		return 0;
	}
	else												// ���� ���� ��
	{
		puts("Connect fail");
		return 0;
	}
	
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