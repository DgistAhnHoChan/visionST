#include "Main.h"
//#include "CameraParameter.h"

// ī�޶� �����ϱ� ���� �Լ� // ����, �Ķ��Ÿ �ʱ�ȭ
int OnCameraConnect()						
{
	void* m_hwnd = NULL;					// ������ �ڵ鷯 ��
	int ret = 0;							// return ��

	ViewColor = 0;							// Bayer = 0, Color = 1;
	ret = vstpleoraInitialize(m_hwnd);		// ī�޶� ����

	size = height*width*4*2;				// �̹��� ������ �ʱ�ȭ

	Buf = (uchar *)malloc(size);			// ��ü�� ���Ź��� ���� �ʱ�ȭ

	Buf1 = (uchar *)malloc(height*width*3);	// ī�޶� 1�� ���� �ʱ�ȭ
	Buf2 = (uchar *)malloc(height*width*3);	// ī�޶� 2�� ���� �ʱ�ȭ
	Buf3 = (uchar *)malloc(height*width*3);	// ī�޶� 3�� ���� �ʱ�ȭ
	Buf4 = (uchar *)malloc(height*width*3);	// ī�޶� 4�� ���� �ʱ�ȭ

	Buf11 = (uchar *)malloc(height*width);	// �÷���ȯ �� �ӽ÷� ����� ���� �ʱ�ȭ
	Buf12 = (uchar *)malloc(height*width);

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
		row = y * width * 3;	// �̹����� ���� 
		for(x=0; x<width; x++)	// �̹��� �� ��ŭ �ݺ�
		{		
			xrow=x*6;			// ī�޶� �� �ε���
			xstep=x*3;			// ȭ�Һ� �� �ε���
			tmp = buf[y*step + (xrow+cam1)];// ī�޶� 1���� �ش� ȭ�� ��������
			buf1[row + xstep+0] = tmp;		// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
			buf1[row + xstep+1] = tmp;
			buf1[row + xstep+2] = tmp;
			tmp=buf[y*step + (xrow+cam2)];	// ī�޶� 2���� �ش� ȭ�� ��������
			buf2[row + xstep+0] = tmp;		// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
			buf2[row + xstep+1] = tmp;
			buf2[row + xstep+2] = tmp;
			tmp = buf[y*step + (xrow+cam3)];// ī�޶� 3���� �ش� ȭ�� ��������
			buf3[row + xstep+0] = tmp;		// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
			buf3[row + xstep+1] = tmp;
			buf3[row + xstep+2] = tmp;
			tmp = buf[y*step + (xrow+cam4)];// ī�޶� 4���� �ش� ȭ�� ��������
			buf4[row + xstep+0] = tmp;		// 3ä�� �����̹Ƿ� ���� ���� 3���� ����
			buf4[row + xstep+1] = tmp;
			buf4[row + xstep+2] = tmp;			
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

unsigned char *Bayer2Bgr2(unsigned char *src, int width, int height)
{
	unsigned char *dst = (unsigned char *)malloc(height*width*3);	// ������ �޸� ������ �����Ҵ�
	int y,x;

	// 3ä���� ���� �޸� ������ 1ä���� ������ ����
	for(y=0; y<height; y++)
	{
		for(x=0; x<width*3; x+=3)
		{
			Buf12[y*width + x/3] = src[y*width*3 + x];
		}
	}

	Mat color( height, width, CV_8UC3);				// �÷� �����͸� ������ �̹��� ���� ����
	Mat original( height, width, CV_8UC1, Buf12 );	// ���� �����͸� ������ �̹��� ���� ����
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

void getImage()
{
	int band = 3;

	vstpleoraGetImageSize(&size);											// �̹��� ������
	receiveImage = vstpleoraGetImage(Buf, size);							// �̹��� �޾ƿ���

	if( onImageCheck == true )												// �����尡 �̹����� ��� ó���ϸ� true
	{
		if( receiveImage == 1 )												// �̹����� �޾ƿ��� ���� ������
		{
			//OnViewFramerate();											// fps ���
			SeparationBuffer(Buf, Buf1, Buf2, Buf3, Buf4, width, height);	// ���� ������
		}
		else																// �̹����� �޾ƿ��� ������ �� ���� �ʱ�ȭ
		{
			memset(Buf1, 0x00, height*width*band); 
			memset(Buf2, 0x00, height*width*band);
			memset(Buf3, 0x00, height*width*band);
			memset(Buf4, 0x00, height*width*band);
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
	uchar *color1=0;
	Mat camera1;

	bool flag2 = false;

	while( stop_signal == false )									// ���� ��ȣ�� ������� �ݺ�
	{
		if( receiveImage == 1 )										// �̹����� ���������� �޾ƿ���
		{
			if( ViewColor==0 )										// ��� ó��
			{
				Mat camera1(height, width, CV_8UC3, Buf1);		
				cv::imshow("windows1", camera1);					// ȭ�鿡 ���̱�
				onImageCheck = true;								// �̹��� ��������� �˸�
			}
			else if( ViewColor==1 )									// �÷� ó��
			{

				color1 = Bayer2Bgr(Buf1, width, height);			// ������

				Mat camera1(height, width, CV_8UC3, color1);
				cv::imshow("windows1", camera1);					// ȭ�鿡 ���̱�
				onImageCheck = true;								// �̹��� ��������� �˸�


				videoWrite << camera1;								// ��ȭ ���
				if( flag2 == false )								// ��ȭ ����� �Լ��� �ѹ��� �������ָ� ó�� ��
				{   
					videoWrite.open("windows1.avi" , CV_FOURCC('X', 'V', 'I', 'D'), 30, Size(width, height), true); // ������ �����ϴ� lib
					flag2 = true;
				}

				Bayer2BgrFree(color1);								// �÷� ���� ����
				//Bayer2BgrFree(color2);
				//Bayer2BgrFree(color3);
				//Bayer2BgrFree(color4);
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

void bufThread2()
{
	unsigned char *color2=0;

	bool flag = false;
	Mat camera2;

	while( stop_signal == false )
	{
		if(receiveImage == 1)
		{
			if( ViewColor==0 )
			{
				Mat camera2(height, width, CV_8UC3, Buf2);
				cv::imshow("windows2", camera2);

				onImageCheck = true;
			}
			else if( ViewColor==1 )
			{
				color2 = Bayer2Bgr2(Buf2, width, height);

				Mat camera2(height, width, CV_8UC3, color2);
				cv::imshow("windows2", camera2);

				onImageCheck = true;

				videoWrite2 << camera2;
				if( flag == false )
				{
					videoWrite2.open("windows2.avi" , CV_FOURCC('X', 'V', 'I', 'D'), 30, Size(width, height), true); // ������ �����ϴ� lib
					flag = true;
				}
				Bayer2BgrFree(color2);
			}
		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

		char chKey = cvWaitKey(5);

		if(chKey == 'q') 
		{
			if(ViewColor==1)
				videoWrite2.release();
			break;
		}
		else if(chKey == 'c')
			ViewColor=1;
		else if(chKey == 'b') 
		{
			videoWrite2.release(); // ������ ���� ����
			ViewColor=0;
		}
	}
}

// ������ üũ
void OnViewFramerate()
{
	double outputframerate = vstpleoraGetOutputFramerate();	// ��� ����Ʈ üũ
	printf("output rate : %.02f fps \n", outputframerate);
	return;
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

		thread thread1(bufThread);							// ù��° ī�޶� ���� ������
		thread thread2(bufThread2);							// �ι�° ī�޶� ���� ������

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
		thread1.join();									// �����尡 ������ ����� �� ���� ���
		thread2.join(); 

		free(Buf);										// ���� ����
		free(Buf1);
		free(Buf2);
		free(Buf3);
		free(Buf4);

		free(Buf11);
		free(Buf12);

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