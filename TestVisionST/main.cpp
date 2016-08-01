#include "Main.h"

// 카메라 연결하기 위한 함수 // 버퍼, 파라메타 초기화
int OnCameraConnect()						
{
	size = IMAGE_HEIGHT * IMAGE_WIDTH * 6;
	void* m_hwnd = NULL;					// 윈도우 핸들러 값
	int ret = 0;							// return 값

	isColor = true;							// Bayer = 0, Color = 1;
	ret = vstpleoraInitialize(m_hwnd);		// 카메라 연결

	wholeImg = Mat(IMAGE_HEIGHT, IMAGE_WIDTH*BAYER_OVERHEADER, CV_8UC1);			// 전체를 수신받을 버퍼 초기화
	
	for(int idx = 0; idx < CAMERA_NUMBER; idx++)
	{
		imgColor[idx] = Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3);		
		cameraImg[idx] = Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
		cameraBuf[idx] = cameraImg[idx].data;
	}

	// 파라메타 값 설정
	SetParam(EXPOSURE_INIT_VALUE, GLOBAL_GAIN_INIT_VALUE, RED_GAIN_INIT_VALUE, GREEN_GAIN_INIT_VALUE, BLUE_GAIN_INIT_VALUE);		// SetExposure, Gain 값 설정 (밝기)
	vstpleoraGetImageSize(&size);											// 이미지 사이즈
	receiveImage = vstpleoraGetImage(wholeImg.data, size);					// 이미지 받아오기
	return ret;								// 카메라 연결 값 리턴 (연결되었으면 1, 아니면 0)
}

// 파라메타 
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

// 버퍼 나누기
void SeparationBuffer(uchar *buf, uchar **cameraBuf, int width, int height)
{
	// 폭 인덱스
	int xrow = 0;
	int singleStep = 0;
	int wholeStep = 0;
	
	for(int yy=0; yy<height; yy++)		// 이미지 높이 만큼 반복
	{
		singleStep = yy * width;		// 이미지의 행은 
		wholeStep = yy * 7680;		// 수신 버퍼의 전체 폭
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
	waitKey(5);
	if( receiveImage != 1 ) return;
	SeparationBuffer(wholeImg.data, cameraBuf, IMAGE_WIDTH, IMAGE_HEIGHT);	// 버퍼 나누기
	if( isColor ) 
	{
		for(int idx = 0; idx < CAMERA_NUMBER; idx++)
			cvtColor( cameraImg[idx], imgColor[idx], CV_BayerGB2BGR );	// Opencv의 컬러 공간 변환을 사용
	}
}

// 메인 함수
int main(void)
{
	char chKey = 0;
	int ret = 0;										// 카메라 연결의 리턴 값
	puts("Bayer : B , Color : C\n Connecting... ");
	
	ret = OnCameraConnect();							// 카메라 연결, 파라메타 설정, 버퍼 채우기
	if(ret != 1) 
	{
		puts("Connect fail");
		return -1;									// 연결 실패 시
	}
	
	puts("Camera Connect\nPress 'Esc' to exit");
	
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
		chKey = waitKey(10);
		if( chKey == 27 ) break;
		else if( chKey == 'c' )		isColor = true;
		else if( chKey == 'b' )		isColor = false;
	}
	vstpleoraDestroy();								// 카메라 연결 해제
	return 0;
}