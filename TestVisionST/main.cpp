#include "Main.h"
//#include "CameraParameter.h"

// 카메라 연결하기 위한 함수 // 버퍼, 파라메타 초기화
int OnCameraConnect()						
{
	void* m_hwnd = NULL;					// 윈도우 핸들러 값
	int ret = 0;							// return 값

	ViewColor = 0;							// Bayer = 0, Color = 1;
	ret = vstpleoraInitialize(m_hwnd);		// 카메라 연결

	size = height*width*6;					// 이미지 사이즈 초기화

	Buf = (uchar *)malloc(size);			// 전체를 수신받을 버퍼 초기화
	Buf1 = (uchar *)malloc(height*width);	// 카메라 1번 버퍼 초기화
	Buf2 = (uchar *)malloc(height*width);	// 카메라 2번 버퍼 초기화
	Buf3 = (uchar *)malloc(height*width);	// 카메라 3번 버퍼 초기화
	Buf4 = (uchar *)malloc(height*width);	// 카메라 4번 버퍼 초기화

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

// 버퍼 나누기
void SeparationBuffer(unsigned char *buf, unsigned char *buf1, unsigned char *buf2, unsigned char *buf3, unsigned char *buf4, int width, int height)
{
	// 수신 버퍼의 전체 폭
	int step = 7680;

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
	unsigned char tmp;			// 임시 픽셀 변수
	for(y=0; y<height; y++)		// 이미지 높이 만큼 반복
	{
		row = y * width;	// 이미지의 행은 
		for(x=0; x<width; x++)	// 이미지 폭 만큼 반복
		{		
			xrow=x*6;			// 카메라별 폭 인덱스
			xstep=x;			// 화소별 폭 인덱스
			buf1[row + xstep+0] = buf[y*step + (xrow+cam1)];			// 3채널 버퍼이므로 같은 값을 3곳에 넣음
			buf2[row + xstep+0] = tmp=buf[y*step + (xrow+cam2)];		// 3채널 버퍼이므로 같은 값을 3곳에 넣음
			buf3[row + xstep+0] = tmp = buf[y*step + (xrow+cam3)];		// 3채널 버퍼이므로 같은 값을 3곳에 넣음
			buf4[row + xstep+0] = buf[y*step + (xrow+cam4)];			// 3채널 버퍼이므로 같은 값을 3곳에 넣음
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

	Mat imgColor(height, width, CV_8UC3);		
	bool flag2 = false;
	while( stop_signal == false )									// 멈춤 신호가 오기까지 반복
	{
		if( receiveImage == 1 )										// 이미지를 정상적으로 받아오면
		{
			Mat camera1(height, width, CV_8UC1, Buf1);		
			if( ViewColor==0 )										// 흑백 처리
			{
				
				cv::imshow("windows1", camera1);					// 화면에 보이기
				onImageCheck = true;								// 이미지 출력했음을 알림
			}
			else if( ViewColor==1 )									// 컬러 처리
			{
				cvtColor( camera1, imgColor, CV_BayerGB2BGR );	// Opencv의 컬러 공간 변환을 사용
				cv::imshow("windows1", imgColor);					// 화면에 보이기
				onImageCheck = true;								// 이미지 출력했음을 알림


				videoWrite << imgColor;								// 녹화 기능
				if( flag2 == false )								// 녹화 기능은 함수를 한번만 실행해주면 처리 됨
				{   
					videoWrite.open("windows1.avi" , CV_FOURCC('X', 'V', 'I', 'D'), 30, Size(width, height), true); // 동영상 저장하는 lib
					flag2 = true;
				}
			}
		}
		else														// 이미지를 정상적으로 받아오지 못하면
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			puts("Not read data");								
		}

		char chKey = waitKey(5);

		if( chKey == 'q' )				// q 입력
		{
			if(ViewColor==1)
				videoWrite.release();
			break;
		}
		else if( chKey == 'c' )			// c 입력
			ViewColor=1;
		else if( chKey == 'b' )			// b 입력
		{
			videoWrite.release();		// 동영상 저장 종료
			ViewColor=0;
		}
	}
}

// 메인 함수
int main(void)
{
	int ret = 0;										// 카메라 연결의 리턴 값
	stop_signal = false;								// 스레드 멈춤 신호

	puts("Bayer : B , Color : C ");
	puts("");

	puts("Connecting... ");
	ret = OnCameraConnect();							// 카메라 연결, 파라메타 설정, 버퍼 채우기
	
	if(ret == 1)										// 연결 성공시 작업 진행
	{
		puts("Camera Connect");
		puts("");

		getImage();										// 초기 이미지 값 받기

		thread threadCamera(bufThread);					// 첫번째 카메라에 대한 쓰레드

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

		//Join
		threadCamera.join();									// 스레드가 완전히 종료될 때 까지 대기
		free(Buf);										// 버퍼 비우기
		free(Buf1);
		free(Buf2);
		free(Buf3);
		free(Buf4);

		vstpleoraDestroy();								// 카메라 연결 해제
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