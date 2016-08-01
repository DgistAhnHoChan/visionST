#include "Main.h"
//#include "CameraParameter.h"

// 카메라 연결하기 위한 함수 // 버퍼, 파라메타 초기화
int OnCameraConnect()						
{
	void* m_hwnd = NULL;					// 윈도우 핸들러 값
	int ret = 0;							// return 값

	isColor = true;							// Bayer = 0, Color = 1;
	ret = vstpleoraInitialize(m_hwnd);		// 카메라 연결

	wholeImg = Mat(IMAGE_HEIGHT, IMAGE_WIDTH*BAYER_OVERHEADER, CV_8UC1);			// 전체를 수신받을 버퍼 초기화
	
	for(int idx = 0; idx < 4; idx++)
	{
		imgColor[idx] = Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3);		
		cameraImg[idx] = Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
		cameraBuf[idx] = cameraImg[idx].data;
	}

	// 파라메타 값 설정
	SetExposure(EXPOSURE_INIT_VALUE);		// SetExposure, Gain 값 설정 (밝기)
	SetGlobalGain(GLOBAL_GAIN_INIT_VALUE);
	SetRedGain(RED_GAIN_INIT_VALUE);
	SetGreenGain(GREEN_GAIN_INIT_VALUE);
	SetBlueGain(BLUE_GAIN_INIT_VALUE);

	vstpleoraGetImageSize(&size);											// 이미지 사이즈
	receiveImage = vstpleoraGetImage(wholeImg.data, size);					// 이미지 받아오기
	videoWrite.open("windows1.avi" , CV_FOURCC('X', 'V', 'I', 'D'), 30, Size(IMAGE_WIDTH, IMAGE_HEIGHT), true); // 동영상 저장하는 lib
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

// 버퍼 나누기
void SeparationBuffer(uchar *buf, uchar **cameraBuf, int width, int height)
{
	// 폭 인덱스

	int xrow = 0;
	int singleStep = 0;
	int wholeStep = 0;
	
	for(int y=0; y<height; y++)		// 이미지 높이 만큼 반복
	{
		singleStep = y * width;		// 이미지의 행은 
		wholeStep = y * 7680;		// 수신 버퍼의 전체 폭
		for(int xx=0; xx<width; xx++)	// 이미지 폭 만큼 반복
		{		
			xrow=xx*6;			// 카메라별 폭 인덱스
			
			cameraBuf[0][singleStep + xx] = buf[wholeStep + (xrow+0)];			// 3채널 버퍼이므로 같은 값을 3곳에 넣음
			cameraBuf[1][singleStep + xx] = buf[wholeStep + (xrow+2)];			// 3채널 버퍼이므로 같은 값을 3곳에 넣음
			cameraBuf[2][singleStep + xx] = buf[wholeStep + (xrow+3)];			// 3채널 버퍼이므로 같은 값을 3곳에 넣음
			cameraBuf[3][singleStep + xx] = buf[wholeStep + (xrow+5)];			// 3채널 버퍼이므로 같은 값을 3곳에 넣음
		}	// end of for(int xx=0; xx<width; xx++)
	}	// end of for(int y=0; y<height; y++)
	
} // end of void SeparationBuffer

void getImage()
{
	vstpleoraGetImageSize(&size);											// 이미지 사이즈
	receiveImage = vstpleoraGetImage(wholeImg.data, size);					// 이미지 받아오기
	if( receiveImage != 1 ) return;
		
	SeparationBuffer(wholeImg.data, cameraBuf, width, height);	// 버퍼 나누기
	if( isColor ) 
	{
		for(int idx = 0; idx < 4; idx++)
			cvtColor( cameraImg[idx], imgColor[idx], CV_BayerGB2BGR );	// Opencv의 컬러 공간 변환을 사용
	}
}

// 메인 함수
int main(void)
{
	char chKey = 0;
	int ret = 0;										// 카메라 연결의 리턴 값
	puts("Bayer : B , Color : C\n");
	puts("Connecting... ");
	ret = OnCameraConnect();							// 카메라 연결, 파라메타 설정, 버퍼 채우기
	
	if(ret != 1) 
	{
		puts("Connect fail");
		return -1;									// 연결 실패 시
	}
	
	puts("Camera Connect\n");
	puts("Press 'Esc' to exit ");
	stop_signal = false;							// 스레드 멈춤 신호
	
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
	vstpleoraDestroy();								// 카메라 연결 해제
	return 0;
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