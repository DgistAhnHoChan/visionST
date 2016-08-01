#include "Main.h"
//#include "CameraParameter.h"

// 카메라 연결하기 위한 함수 // 버퍼, 파라메타 초기화
int initCamera()						
{
	void* m_hwnd = NULL;					// 윈도우 핸들러 값
	int ret = 0;							// return 값

	ViewColor = 0;							// Bayer = 0, Color = 1;
	ret = vstpleoraInitialize(m_hwnd);		// 카메라 연결

	/*

	이미지 사이즈가 1280 * 720 * 6인 이유는 이미지를 받아올 때 하나의 픽셀에 buf[6]로 받아오는 구조로 이루어진다. 
	이 구조에서 buf[0]에 Camera1, buf[2]에 Camera2, buf[3]에 Camera3, buf[5]에 Camera4가 들어오게 된다. 
	이렇게 한 픽셀이 버퍼를 6개를 할당 받아 사용되기 때문에 1280 * 720의 이미지를 1280 * 720 * 6의 버퍼 공간이 필요하게 되는 것이다.
	
	*/

	size = height*width*6;					// 이미지 사이즈 초기화

	Buf = (uchar *)malloc(size);			// 전체를 수신받을 버퍼 초기화
	Buf1 = (uchar *)malloc(height*width);	// 카메라 1번 버퍼 초기화
	Buf2 = (uchar *)malloc(height*width);	// 카메라 2번 버퍼 초기화
	Buf3 = (uchar *)malloc(height*width);	// 카메라 3번 버퍼 초기화
	Buf4 = (uchar *)malloc(height*width);	// 카메라 4번 버퍼 초기화

	Buff = (uchar *)malloc(height*width*4*2);			// 전체를 수신받을 버퍼 초기화
	Buf5 = (uchar *)malloc(height*width*3);	// 카메라 1번 버퍼 초기화
	Buf6 = (uchar *)malloc(height*width*3);	// 카메라 2번 버퍼 초기화
	Buf7 = (uchar *)malloc(height*width*3);	// 카메라 3번 버퍼 초기화
	Buf8 = (uchar *)malloc(height*width*3);	// 카메라 4번 버퍼 초기화


	Buf11 = (uchar *)malloc(height*width);	// 카메라 4번 버퍼 초기화

	// 파라메타 값 설정
	SetExposure(EXPOSURE_INIT_VALUE);		// SetExposure, Gain 값 설정 (밝기)
	SetGlobalGain(GLOBAL_GAIN_INIT_VALUE);
	SetRedGain(RED_GAIN_INIT_VALUE);
	SetGreenGain(GREEN_GAIN_INIT_VALUE);
	SetBlueGain(BLUE_GAIN_INIT_VALUE);

	return ret;								// 카메라 연결 값 리턴 (연결되었으면 1, 아니면 0)
}

// 파라메타 
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

// 버퍼 나누기
void SeparationBuffer(uchar *buf,  uchar *buf1, uchar *buf2, uchar *buf3, uchar *buf4, int width, int height)
{
	// 수신 버퍼의 전체 폭
	int step = 7680;		// 1280 * 6

	// xy좌표
	int y, x;

	// 폭 인덱스
	int row=0;
	int xrow=0;
	int xstep=0;

	// 카메라ID 별 인덱스
	int cam1=0;	
	int cam2=2;
	int cam3=3;
	int cam4=5;
	uchar tmp;						// 임시 픽셀 변수
	for(y=0; y<height; y++)			// 이미지 높이 만큼 반복
	{
		row = y * width;			// 이미지의 행은 
		for(x=0; x<width; x++)		// 이미지 폭 만큼 반복
		{		
			xrow=x*6;				// 카메라별 폭 인덱스
			xstep=x;				// 화소별 폭 인덱스
			buf1[row + xstep+0] = buf[y*step + (xrow+cam1)];				// 3채널 버퍼이므로 같은 값을 3곳에 넣음
			buf2[row + xstep+0] = tmp=buf[y*step + (xrow+cam2)];		
			buf3[row + xstep+0] = tmp = buf[y*step + (xrow+cam3)];		
			buf4[row + xstep+0] = buf[y*step + (xrow+cam4)];			
		}
	}
}

void getImage()
{
	vstpleoraGetImageSize(&size);											// 이미지 사이즈
	receiveImage = vstpleoraGetImage(Buf, size);							// 이미지 받아오기

	if( onImageCheck == true )												// 스레드가 이미지를 출력 처리하면 true
	{
		if( receiveImage == 1 )												// 이미지를 받아오면 버퍼 나누기
		{
			SeparationBuffer(Buf, Buf1, Buf2, Buf3, Buf4, width, height);	// 버퍼 나누기
			SeparationBuffer2(Buf, Buf5, Buf6, Buf7, Buf8, width, height);	// 버퍼 나누기
		}
		else																// 이미지를 받아오지 못했을 시 버퍼 초기화
		{
			memset(Buf1, 0x00, height*width); 
			memset(Buf2, 0x00, height*width);
			memset(Buf3, 0x00, height*width);
			memset(Buf4, 0x00, height*width);
		}

		onImageCheck = false;												// 다시 false로 변경해서 이미지 출력 처리 할 때 까지 대기
	}
	else																	// 스레드가 이미지 출력을 처리중인 상태라면 대기
	{
		this_thread::sleep_for(milliseconds(1));							// 출력하지 못하면 대기
	}
}

// 버퍼 쓰레드
void bufThread()
{
	int count = 0;
	char fileName[200] = "";

	Mat imgColor1(height, width, CV_8UC3);
	Mat imgColor2(height, width, CV_8UC3);
	//Mat imgColor3(height, width, CV_8UC3);
	//Mat imgColor4(height, width, CV_8UC3);

	bool flag = false;
	while( stop_signal == false )									// 멈춤 신호가 오기까지 반복
	{
		if( receiveImage == 1 )										// 이미지를 정상적으로 받아오면
		{
			Mat camera1(height, width, CV_8UC1, Buf1);
			//Mat camera2(height, width, CV_8UC1, Buf2);
			//Mat camera3(height, width, CV_8UC1, Buf3);
			//Mat camera4(height, width, CV_8UC1, Buf4);

			if( ViewColor==0 )										// 흑백 처리
			{
				cv::imshow("Windows", camera1);					// 화면에 보이기
				//cv::imshow("windows23", camera2);
				//cv::imshow("windows3", camera3);
				//cv::imshow("windows4", camera4);

				onImageCheck = true;								// 이미지 출력했음을 알림
			}
			else if( ViewColor==1 )									// 컬러 처리
			{
				cvtColor( camera1, imgColor1, CV_BayerGB2BGR );		// Opencv의 컬러 공간 변환을 사용
				//cvtColor( camera2, imgColor2, CV_BayerGB2BGR );		// Opencv의 컬러 공간 변환을 사용
				//cvtColor( camera3, imgColor3, CV_BayerGB2BGR );		// Opencv의 컬러 공간 변환을 사용
				//cvtColor( camera4, imgColor4, CV_BayerGB2BGR );		// Opencv의 컬러 공간 변환을 사용
				
				cv::imshow("Windows", imgColor1);					// 화면에 보이기
				//cv::imshow("windows23", imgColor2);	
				//cv::imshow("windows3", imgColor3);	
				//cv::imshow("windows4", imgColor4);	

				onImageCheck = true;								// 이미지 출력했음을 알림

				/*
				videoWrite << imgColor;								// 녹화 기능
				if( flag == false )								// 녹화 기능은 함수를 한번만 실행해주면 처리 됨
				{   
					videoWrite.open("windows1.avi" , CV_FOURCC('X', 'V', 'I', 'D'), 30, Size(width, height), true); // 동영상 저장하는 lib
					flag = true;
				}*/

			
			imwrite("Windows.jpg", imgColor1);
			count++;

			}

		}
		else														// 이미지를 정상적으로 받아오지 못하면
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			puts("Not read data");								
		}

		char chKey = waitKey(10);		// 10ms의 딜레이를 발생

		if( chKey == 'q' )				// q 입력
		{
			//if(ViewColor==1)
			//	videoWrite.release();
			break;
		}
		else if( chKey == 'c' )			// c 입력
			ViewColor=1;
		else if( chKey == 'b' )			// b 입력
		{
			//videoWrite.release();		// 동영상 저장 종료
			ViewColor=0;
		}
	}
}

void releaseBuf()
{
		free(Buf);										// 버퍼 비우기
		free(Buf1);
		free(Buf2);
		free(Buf3);
		free(Buf4);

		free(Buf5);
		free(Buf6);
		free(Buf7);
		free(Buf8);

		vstpleoraDestroy();								// 카메라 연결 해제
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
	// 수신 버퍼의 전체 폭
	int step1 = 7680;	// 전체 버퍼의 x축 한줄의 크기

	// xy좌표
	int y1, x1;

	// 폭 인덱스
	int row1=0;
	int xrow1=0;
	int xstep1=0;

	// 카메라ID 별 인덱스
	int cam5=0;	
	int cam6=2;
	int cam7=3;
	int cam8=5;
	unsigned char tmp1;			// 임시 픽셀 변수
	for(y1=0; y1<height; y1++)		// 이미지 높이 만큼 반복
	{
		row1 = y1 * width * 3;	// 이미지의 행은 
		for(x1=0; x1<width; x1++)	// 이미지 폭 만큼 반복
		{		
			xrow1=x1*6;			// 카메라별 폭 인덱스
			xstep1=x1*3;			// 화소별 폭 인덱스
			tmp1 = buff[y1*step1 + (xrow1+cam5)];// 카메라 1번의 해당 화소 가져오기
			buf5[row1 + xstep1+0] = tmp1;		// 3채널 버퍼이므로 같은 값을 3곳에 넣음
			buf5[row1 + xstep1+1] = tmp1;
			buf5[row1 + xstep1+2] = tmp1;
			tmp1=buff[y1*step1 + (xrow1+cam6)];	// 카메라 2번의 해당 화소 가져오기
			buf6[row1 + xstep1+0] = tmp1;		// 3채널 버퍼이므로 같은 값을 3곳에 넣음
			buf6[row1 + xstep1+1] = tmp1;
			buf6[row1 + xstep1+2] = tmp1;
			tmp1 = buff[y1*step1 + (xrow1+cam7)];// 카메라 3번의 해당 화소 가져오기
			buf7[row1 + xstep1+0] = tmp1;		// 3채널 버퍼이므로 같은 값을 3곳에 넣음
			buf7[row1+ xstep1+1] = tmp1;
			buf7[row1 + xstep1+2] = tmp1;
			tmp1 = buff[y1*step1 + (xrow1+cam8)];// 카메라 4번의 해당 화소 가져오기
			buf8[row1 + xstep1+0] = tmp1;		// 3채널 버퍼이므로 같은 값을 3곳에 넣음
			buf8[row1 + xstep1+1] = tmp1;
			buf8[row1+ xstep1+2] = tmp1;			
		}
	}
}

// Bayer 패턴을 RGB 패턴으로 변경
unsigned char *Bayer2Bgr(unsigned char *src, int width, int height)
{
	unsigned char *dst = (unsigned char *)malloc(height*width*3);	// 저장할 메모리 공간에 동적할당
	int y,x;

	// 3채널인 기존 메모리 공간을 1채널인 공간에 복사
	for(y=0; y<height; y++)
	{
		for(x=0; x<width*3; x+=3)
		{
			Buf11[y*width + x/3] = src[y*width*3 + x];
		}
	}

	Mat color( height, width, CV_8UC3);				// 컬러 데이터를 저장할 이미지 변수 생성
	Mat original( height, width, CV_8UC1, Buf11 );	// 원본 데이터를 저장할 이미지 변수 생성
	cvtColor( original, color, CV_BayerGB2BGR );	// Opencv의 컬러 공간 변환을 사용

	// OpenCV의 데이터를 다시 char로 변환
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

// 메인 함수
int main(void)
{
	int ret = 0;										// 카메라 연결의 리턴 값
	stop_signal = false;								// 스레드 멈춤 신호

	puts("Bayer : B , Color : C ");
	puts("");

	puts("Connecting...");
	ret = initCamera();							// 카메라 연결, 파라메타 설정, 버퍼 채우기
	
	if(ret == 1)										// 연결 성공시 작업 진행
	{
		puts("Camera Connect");
		puts("");

		getImage();										// 초기 이미지 값 받기

		thread threadCamera(bufThread);					// 첫번째 카메라에 대한 쓰레드
		thread threadCamera2(bufThread2);

		puts("Press 'q' to exit ");

		while(1)
		{
			getImage();

			if( _kbhit() )								// 키보드 q 누르면 반복문 종료
			{
				if(getch() == 'q')
					break;
			}
		}
	
		stop_signal = true;								// Thread 종료

		threadCamera.join();							// 스레드가 완전히 종료될 때 까지 대기
		threadCamera2.join();	
		releaseBuf();									// 버퍼 비우기, 접속 종료

		puts("");
		puts("Camera disconnect");
		
		return 0;
	}
	else												// 연결 실패 시
	{
		puts("Connect fail");
		return 0;
	}
	
}
















//	시간 구하기, Main.h 파일에 주석처리 되어 있음
//	startTime = system_clock::now();
//	endTime = system_clock::now();
//	if(!flag)
//	{
//		nanoseconds milli2 = duration_cast<nanoseconds>(endTime2-startTime2);
//		std::cout << "이미지 받아오는 시간 : " << milli2.count() << endl;
//		flag = true;
//	}