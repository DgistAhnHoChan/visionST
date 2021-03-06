#include "Main.h"

// 카메라 연결하기 위한 함수 // 버퍼, 파라메타 초기화
int OnCameraConnect()						
{
	size = IMAGE_HEIGHT * IMAGE_WIDTH * 6;	// 이미지 사이즈 1280 * 720 * 6
	void* m_hwnd = NULL;					// vstpleoraInitialize()의 윈도우 핸들러 값
	int ret = 0;							// vstpleoraInitialize()의 리턴 값

	isColor = false;							// Bayer = 0, Color = 1;
	ret = vstpleoraInitialize(m_hwnd);		// 카메라 연결 (연결이 정상이면 1, 실패하면 0 출력)
	// MFC 환경의 함수, m_hwnd 변수의 NULL 값을 임의로 입력

	wholeImg = Mat(IMAGE_HEIGHT, IMAGE_WIDTH*BAYER_OVERHEADER, CV_8UC1);	// 전체를 수신받을 버퍼 초기화
	// 720, 1280*6, 8 bit unsigned 1채널 행렬
	// 즉, 이미지 사이즈 행(1280*6)*열(720)

	cameraImg = Mat(IMAGE_HEIGHT*2, IMAGE_WIDTH*2, CV_8UC1, Scalar(0));	// Bayer 카메라 버퍼 초기화

	// 파라메타 값 설정
	SetParam(EXPOSURE_INIT_VALUE, GLOBAL_GAIN_INIT_VALUE, RED_GAIN_INIT_VALUE, GREEN_GAIN_INIT_VALUE, BLUE_GAIN_INIT_VALUE);	// SetExposure, Gain 값 설정 (밝기)
	// 파라메타 기본 설정 값은 480, 64, 64, (64*0.8), 64 → 순서대로
	vstpleoraGetImageSize(&size);											// 이미지 사이즈 받아오기
	receiveImage = vstpleoraGetImage(wholeImg.data, size);					// 이미지 받아오기 (lib 함수)
	return ret;																// 카메라 연결 값 리턴 (연결되었으면 1, 아니면 0)
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
void SeparationBuffer(uchar *buf, uchar *cameraBuf2, int width, int height)
{
	// 폭 인덱스
	int xrow = 0;						// 이미지 버퍼 인덱스
	int singleStep = 0;					// 이미지의 행
	int wholeStep = 0;					// 수신 버퍼의 전체 폭

	for(int plusY = 0 ; plusY < height ; plusY++)
	{
		wholeStep = plusY * 7680;

		for(int plusX = 0 ; plusX < width ; plusX++)
		{
			xrow=plusX * 6;
			cameraBuf2[plusY*(width*2) + plusX] = buf[wholeStep + (xrow+0)];
			cameraBuf2[plusY*(width*2) + plusX + width] = buf[wholeStep + (xrow+2)];
			cameraBuf2[(plusY+height)*(width*2) + plusX ] = buf[wholeStep + (xrow+3)];
			cameraBuf2[(plusY+height)*(width*2) + plusX + width] = buf[wholeStep + (xrow+5)];	
		}
	}

	
} // end of void SeparationBuffer

// 이미지 받아오는 함수
void getImage()
{
	vstpleoraGetImageSize(&size);											// 이미지 사이즈
	receiveImage = vstpleoraGetImage(wholeImg.data, size);					// 이미지 받아오기
	waitKey(5);																// 이미지 깨짐을 예방 -> 대기시간을 설정하지 않으면 이미지 깨짐 현상 발생
	if( receiveImage != 1 ) return;											// 이미지 받지 못하면 종료
	SeparationBuffer(wholeImg.data, cameraImg.data, IMAGE_WIDTH, IMAGE_HEIGHT);	// 버퍼 나누기
	/*if( isColor )															// 컬러 이미지 설정이면
	{
		for(int idx = 0; idx < CAMERA_NUMBER; idx++)						// 4대의 카메라 갯수만큼 반복
			cvtColor( cameraImg[idx], imgColor[idx], CV_BayerGB2BGR );		// Opencv의 컬러 공간 변환을 사용
		// bayer 버퍼의 내용을, color 버퍼로 옮기고, Bayer to Color 처리
	} */

}

// 메인 함수
int main(void)
{
	char chKey = 0;
	bool flag = 0;

	int ret = 0;										// 카메라 연결의 리턴 값
	puts("Bayer : B , Color : C\n Connecting... ");		// B 입력: Bayer 값, C 입력: Color 값 

	ret = OnCameraConnect();							// 카메라 연결, 파라메타 설정, 버퍼 채우기
	if(ret != 1)										// 연결 실패 시 (1 이 아닐 떄)
	{
		puts("Connect fail");							// Connect fail 출력
		return -1;										
	}

	puts("Camera Connect\nPress 'Esc' to exit");

	while(1)
	{																		
		getImage();																// 이미지 받아오는 함수

		if( !isColor )									// Bayer 처리 
		{

			namedWindow("Camera1", WINDOW_NORMAL);
			//resizeWindow("Camera1", 1280,720);
			imshow("Camera1", cameraImg);			// 1번 카메라 화면에 출력 (bayer)

			//imshow("Camera2", cameraImg[1]);			// 2번 카메라 화면에 출력 (bayer)
			//imshow("Camera3", cameraImg[2]);			// 3번 카메라 화면에 출력 (bayer)
			//imshow("Camera4", cameraImg[3]);			// 4번 카메라 화면에 출력 (bayer)

		}
		else											// Color 처리
		{
			imshow("Camera1", imgColor[0]);				// 1번 카메라 화면에 출력 (color)
			imshow("Camera2", imgColor[1]);				// 2번 카메라 화면에 출력 (color)
			imshow("Camera3", imgColor[2]);				// 3번 카메라 화면에 출력 (color)
			imshow("Camera4", imgColor[3]);				// 4번 카메라 화면에 출력 (color)	

			/*
			system_clock::time_point startTime = system_clock::now();

			videoWrite1 << imgColor[0];					// 녹화 기능
			videoWrite2 << imgColor[1];					// 녹화 기능
			videoWrite3 << imgColor[2];					// 녹화 기능
			videoWrite4 << imgColor[3];					// 녹화 기능

			if( flag == false )								// 녹화 기능은 함수를 한번만 실행해주면 처리 됨
			{   
				videoWrite1.open("Camera1.avi" , CV_FOURCC('X', 'V', 'I', 'D'), 30, Size(IMAGE_WIDTH, IMAGE_HEIGHT), true); // 동영상 저장하는 lib
				videoWrite2.open("Camera2.avi" , CV_FOURCC('X', 'V', 'I', 'D'), 30, Size(IMAGE_WIDTH, IMAGE_HEIGHT), true); 
				videoWrite3.open("Camera3.avi" , CV_FOURCC('X', 'V', 'I', 'D'), 30, Size(IMAGE_WIDTH, IMAGE_HEIGHT), true); 
				videoWrite4.open("Camera4.avi" , CV_FOURCC('X', 'V', 'I', 'D'), 30, Size(IMAGE_WIDTH, IMAGE_HEIGHT), true); 

				flag = true;
			}

			system_clock::time_point endTime = system_clock::now();
			milliseconds milli2 = duration_cast<milliseconds>(endTime-startTime);	// 종료 시간 - 시작 시간
			std::cout << "Recording Time : " << milli2.count()  << " ms " << endl; // 프로세싱 시간 체크 (ms)

			*/

		}
		chKey = waitKey(10);							// 10ms 대기
		if( chKey == 27 ) 
		{
			//videoWrite1.release(); // 동영상 저장 종료
			//videoWrite2.release(); // 동영상 저장 종료
			//videoWrite3.release(); // 동영상 저장 종료
			//videoWrite4.release(); // 동영상 저장 종료
			break;						// Esc 누르면 break
		}

		else if( chKey == 'c' )		isColor = true;		// c 누르면 color
		else if( chKey == 'b' )		isColor = false;	// b 누르면 bayer
	}
	vstpleoraDestroy();									// 카메라 연결 해제
	return 0;
}