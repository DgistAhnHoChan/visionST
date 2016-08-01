#include "Main.h"
//#include "CameraParameter.h"

// ī�޶� �����ϱ� ���� �Լ� // ����, �Ķ��Ÿ �ʱ�ȭ
int initCamera()						
{
	void* m_hwnd = NULL;					// ������ �ڵ鷯 ��
	int ret = 0;							// return ��

	ViewColor = 0;							// Bayer = 0, Color = 1;
	ret = vstpleoraInitialize(m_hwnd);		// ī�޶� ����

	/*

	�̹��� ����� 1280 * 720 * 6�� ������ �̹����� �޾ƿ� �� �ϳ��� �ȼ��� buf[6]�� �޾ƿ��� ������ �̷������. 
	�� �������� buf[0]�� Camera1, buf[2]�� Camera2, buf[3]�� Camera3, buf[5]�� Camera4�� ������ �ȴ�. 
	�̷��� �� �ȼ��� ���۸� 6���� �Ҵ� �޾� ���Ǳ� ������ 1280 * 720�� �̹����� 1280 * 720 * 6�� ���� ������ �ʿ��ϰ� �Ǵ� ���̴�.
	
	*/

	size = height*width*6;					// �̹��� ������ �ʱ�ȭ

	Buf = (uchar *)malloc(size);			// ��ü�� ���Ź��� ���� �ʱ�ȭ
	Buf1 = (uchar *)malloc(height*width);	// ī�޶� 1�� ���� �ʱ�ȭ
	Buf2 = (uchar *)malloc(height*width);	// ī�޶� 2�� ���� �ʱ�ȭ
	Buf3 = (uchar *)malloc(height*width);	// ī�޶� 3�� ���� �ʱ�ȭ
	Buf4 = (uchar *)malloc(height*width);	// ī�޶� 4�� ���� �ʱ�ȭ

	Buff = (uchar *)malloc(height*width*4*2);			// ��ü�� ���Ź��� ���� �ʱ�ȭ
	Buf5 = (uchar *)malloc(height*width*3);	// ī�޶� 1�� ���� �ʱ�ȭ
	Buf6 = (uchar *)malloc(height*width*3);	// ī�޶� 2�� ���� �ʱ�ȭ
	Buf7 = (uchar *)malloc(height*width*3);	// ī�޶� 3�� ���� �ʱ�ȭ
	Buf8 = (uchar *)malloc(height*width*3);	// ī�޶� 4�� ���� �ʱ�ȭ


	Buf11 = (uchar *)malloc(height*width);	// ī�޶� 4�� ���� �ʱ�ȭ

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
	if(ret<9){	
	}

	//Sleep(20);
	return 0;
}

// ���� ������
void SeparationBuffer(uchar *buf,  uchar *buf1, uchar *buf2, uchar *buf3, uchar *buf4, int width, int height)
{
	// ���� ������ ��ü ��
	int step = 7680;		// 1280 * 6

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
	uchar tmp;						// �ӽ� �ȼ� ����
	for(y=0; y<height; y++)			// �̹��� ���� ��ŭ �ݺ�
	{
		row = y * width;			// �̹����� ���� 
		for(x=0; x<width; x++)		// �̹��� �� ��ŭ �ݺ�
		{		
			xrow=x*6;				// ī�޶� �� �ε���
			xstep=x;				// ȭ�Һ� �� �ε���
			buf1[row + xstep+0] = buf[y*step + (xrow+cam1)];				// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
			buf2[row + xstep+0] = tmp=buf[y*step + (xrow+cam2)];		
			buf3[row + xstep+0] = tmp = buf[y*step + (xrow+cam3)];		
			buf4[row + xstep+0] = buf[y*step + (xrow+cam4)];			
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
			SeparationBuffer2(Buf, Buf5, Buf6, Buf7, Buf8, width, height);	// ���� ������
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
	int count = 0;
	char fileName[200] = "";

	Mat imgColor1(height, width, CV_8UC3);
	Mat imgColor2(height, width, CV_8UC3);
	//Mat imgColor3(height, width, CV_8UC3);
	//Mat imgColor4(height, width, CV_8UC3);

	bool flag = false;
	while( stop_signal == false )									// ���� ��ȣ�� ������� �ݺ�
	{
		if( receiveImage == 1 )										// �̹����� ���������� �޾ƿ���
		{
			Mat camera1(height, width, CV_8UC1, Buf1);
			//Mat camera2(height, width, CV_8UC1, Buf2);
			//Mat camera3(height, width, CV_8UC1, Buf3);
			//Mat camera4(height, width, CV_8UC1, Buf4);

			if( ViewColor==0 )										// ��� ó��
			{
				cv::imshow("Windows", camera1);					// ȭ�鿡 ���̱�
				//cv::imshow("windows23", camera2);
				//cv::imshow("windows3", camera3);
				//cv::imshow("windows4", camera4);

				onImageCheck = true;								// �̹��� ��������� �˸�
			}
			else if( ViewColor==1 )									// �÷� ó��
			{
				cvtColor( camera1, imgColor1, CV_BayerGB2BGR );		// Opencv�� �÷� ���� ��ȯ�� ���
				//cvtColor( camera2, imgColor2, CV_BayerGB2BGR );		// Opencv�� �÷� ���� ��ȯ�� ���
				//cvtColor( camera3, imgColor3, CV_BayerGB2BGR );		// Opencv�� �÷� ���� ��ȯ�� ���
				//cvtColor( camera4, imgColor4, CV_BayerGB2BGR );		// Opencv�� �÷� ���� ��ȯ�� ���
				
				cv::imshow("Windows", imgColor1);					// ȭ�鿡 ���̱�
				//cv::imshow("windows23", imgColor2);	
				//cv::imshow("windows3", imgColor3);	
				//cv::imshow("windows4", imgColor4);	

				onImageCheck = true;								// �̹��� ��������� �˸�

				/*
				videoWrite << imgColor;								// ��ȭ ���
				if( flag == false )								// ��ȭ ����� �Լ��� �ѹ��� �������ָ� ó�� ��
				{   
					videoWrite.open("windows1.avi" , CV_FOURCC('X', 'V', 'I', 'D'), 30, Size(width, height), true); // ������ �����ϴ� lib
					flag = true;
				}*/

			
			imwrite("Windows.jpg", imgColor1);
			count++;

			}

		}
		else														// �̹����� ���������� �޾ƿ��� ���ϸ�
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			puts("Not read data");								
		}

		char chKey = waitKey(10);		// 10ms�� �����̸� �߻�

		if( chKey == 'q' )				// q �Է�
		{
			//if(ViewColor==1)
			//	videoWrite.release();
			break;
		}
		else if( chKey == 'c' )			// c �Է�
			ViewColor=1;
		else if( chKey == 'b' )			// b �Է�
		{
			//videoWrite.release();		// ������ ���� ����
			ViewColor=0;
		}
	}
}

void releaseBuf()
{
		free(Buf);										// ���� ����
		free(Buf1);
		free(Buf2);
		free(Buf3);
		free(Buf4);

		free(Buf5);
		free(Buf6);
		free(Buf7);
		free(Buf8);

		vstpleoraDestroy();								// ī�޶� ���� ����
}

void bufThread2()
{

		int count1 = 0;
	char fileName[200] = "";
	unsigned char *color2=0;

	bool flag = false;
	Mat camera2;

	while( stop_signal == false )
	{
		if( receiveImage == 1 )
		{
			if( ViewColor==0 )
			{
				Mat camera2(height, width, CV_8UC3, Buf5);
				cv::imshow("windows2", camera2);
			}
			else if( ViewColor==1 )
			{
				color2 = Bayer2Bgr(Buf5, width, height);

				Mat camera2(height, width, CV_8UC3, color2);
				cv::imshow("windows2", camera2);

				imwrite("Windows2.jpg", camera2);
				count1++;

				Bayer2BgrFree(color2);


			}
		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

		char chKey = cvWaitKey(10);

		if(chKey == 'q') 
		{
			break;
		}
		else if(chKey == 'c')
			ViewColor=1;
		else if(chKey == 'b') 
		{
			ViewColor=0;
		}

	}
}


void SeparationBuffer2(unsigned char *buff, unsigned char *buf5, unsigned char *buf6, unsigned char *buf7, unsigned char *buf8, int width, int height)
{
	// ���� ������ ��ü ��
	int step1 = 7680;	// ��ü ������ x�� ������ ũ��

	// xy��ǥ
	int y1, x1;

	// �� �ε���
	int row1=0;
	int xrow1=0;
	int xstep1=0;

	// ī�޶�ID �� �ε���
	int cam5=0;	
	int cam6=2;
	int cam7=3;
	int cam8=5;
	unsigned char tmp1;			// �ӽ� �ȼ� ����
	for(y1=0; y1<height; y1++)		// �̹��� ���� ��ŭ �ݺ�
	{
		row1 = y1 * width * 3;	// �̹����� ���� 
		for(x1=0; x1<width; x1++)	// �̹��� �� ��ŭ �ݺ�
		{		
			xrow1=x1*6;			// ī�޶� �� �ε���
			xstep1=x1*3;			// ȭ�Һ� �� �ε���
			tmp1 = buff[y1*step1 + (xrow1+cam5)];// ī�޶� 1���� �ش� ȭ�� ��������
			buf5[row1 + xstep1+0] = tmp1;		// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
			buf5[row1 + xstep1+1] = tmp1;
			buf5[row1 + xstep1+2] = tmp1;
			tmp1=buff[y1*step1 + (xrow1+cam6)];	// ī�޶� 2���� �ش� ȭ�� ��������
			buf6[row1 + xstep1+0] = tmp1;		// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
			buf6[row1 + xstep1+1] = tmp1;
			buf6[row1 + xstep1+2] = tmp1;
			tmp1 = buff[y1*step1 + (xrow1+cam7)];// ī�޶� 3���� �ش� ȭ�� ��������
			buf7[row1 + xstep1+0] = tmp1;		// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
			buf7[row1+ xstep1+1] = tmp1;
			buf7[row1 + xstep1+2] = tmp1;
			tmp1 = buff[y1*step1 + (xrow1+cam8)];// ī�޶� 4���� �ش� ȭ�� ��������
			buf8[row1 + xstep1+0] = tmp1;		// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
			buf8[row1 + xstep1+1] = tmp1;
			buf8[row1+ xstep1+2] = tmp1;			
		}
	}
}

// Bayer ������ RGB �������� ����
unsigned char *Bayer2Bgr(unsigned char *src, int width, int height)
{
	unsigned char *dst = (unsigned char *)malloc(height*width*3);	// ������ �޸� ������ �����Ҵ�
	int y,x;

	// 3ä���� ���� �޸� ������ 1ä���� ������ ����
	for(y=0; y<height; y++)
	{
		for(x=0; x<width*3; x+=3)
		{
			Buf11[y*width + x/3] = src[y*width*3 + x];
		}
	}

	Mat color( height, width, CV_8UC3);				// �÷� �����͸� ������ �̹��� ���� ����
	Mat original( height, width, CV_8UC1, Buf11 );	// ���� �����͸� ������ �̹��� ���� ����
	cvtColor( original, color, CV_BayerGB2BGR );	// Opencv�� �÷� ���� ��ȯ�� ���

	// OpenCV�� �����͸� �ٽ� char�� ��ȯ
	for(y=0; y<height; y++)
	{
		for(x=0; x<width*3; x++)
		{
			dst[y*width*3 + x] = color.data[y*width*3 + x];			
		}
	}
	return dst;
}

void Bayer2BgrFree(unsigned char *buf)
{
	if(buf != NULL)
	{ 
		free(buf);
		buf=NULL;
	}
}

// ���� �Լ�
int main(void)
{
	int ret = 0;										// ī�޶� ������ ���� ��
	stop_signal = false;								// ������ ���� ��ȣ

	puts("Bayer : B , Color : C ");
	puts("");

	puts("Connecting...");
	ret = initCamera();							// ī�޶� ����, �Ķ��Ÿ ����, ���� ä���
	
	if(ret == 1)										// ���� ������ �۾� ����
	{
		puts("Camera Connect");
		puts("");

		getImage();										// �ʱ� �̹��� �� �ޱ�

		thread threadCamera(bufThread);					// ù��° ī�޶� ���� ������
		thread threadCamera2(bufThread2);

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

		threadCamera.join();							// �����尡 ������ ����� �� ���� ���
		threadCamera2.join();	
		releaseBuf();									// ���� ����, ���� ����

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