/////////////////////////////////////////////////
#ifndef VSTPLEORA_INTERFACE_H
#define VSTPLEORA_INTERFACE_H
/////////////////////////////////////////////////
#define MT9M025_SLAVE_ADDR		0x30				// 이미지 센서 슬레이브 주소
#define CIS_EXPOSURE_ADDR		0x3012				// 이미지 센서 노출값 설정 주소
#define CIS_EXPOSURE_RANGE		1920				// 이미지 센서 노출 최대값
#define CIS_EXPOSURE_INIT_VALUE	480					// 이미지 센서 노추 초기값
#define CIS_GAIN_ADDR			0x305E				// 이미지 센서 글로벌 게인 설정 주소
#define CIS_GAIN_RANGE			255					// 이미지 센서 글로벌 게인 최대값
#define CIS_GAIN_INIT_VALUE		64					// 이미지 센서 글로벌 게인 초기값
#define CIS_RED_ADDR			0x305A				// 이미지센서 레드 게인 설정 주소
#define CIS_RED_RANGE			255					// 이미지센서 레드 게인 최대값
#define CIS_RED_INIT_VALUE		64					// 이미지센서 레드 게인 초기값
#define CIS_GREEN1_ADDR			0x3056				// 이미지센서 그린 게인1 설정 주소
#define CIS_GREEN2_ADDR			0x305C				// 이미지센서 그린 게인2 설정 주소
#define CIS_GREEN_RANGE			255					// 이미지센서 그린 게인 최대값
#define CIS_GREEN_INIT_VALUE	(64*0.8)			// 이미지센서 그린 게인 초기값
#define CIS_BLUE_ADDR			0x3058				// 이미지센서 블루 게인 설정 주소
#define CIS_BLUE_RANGE			255					// 이미지센서 그린 게인 최대값
#define CIS_BLUE_INIT_VALUE		64					// 이미지센서 그린 게인 초기값
/////////////////////////////////////////////////
#define VSTPLEORADLL	extern "C" __declspec(dllimport)		// 라이브러리 함수를 임폴트
/////////////////////////////////////////////////
#define VSTPLEORA_LIBNAME "../VstPleoraLib/VstPleora.lib"		// 정적 링크 라이브럴리 파일 지정

#pragma comment(lib, VSTPLEORA_LIBNAME)							// 라이브러리 추가
/////////////////////////////////////////////////
//VSTPLEORADLL char *vstpleoraGetLastMessage(void);						// (에러 메시지)마지막 에러 메시지 가져옴
VSTPLEORADLL int vstpleoraInitialize(void *hWnd);						// (1-정상)카메라 초기화(윈도우 핸들러)
VSTPLEORADLL int vstpleoraDestroy(void);								// (1-정상)카메라 연결 해제
VSTPLEORADLL int vstpleoraGetImageSize(int *size);						// (의미없음)출력되는 영상크기확인(버퍼크기)
VSTPLEORADLL int vstpleoraGetImage(unsigned char *buf, int size);		// (1-정상)영상을 획득(할당된 메모리 공간 주소, 크기)
VSTPLEORADLL double vstpleoraGetOutputFramerate(void);					// (출력 프레임레이드)CL-U3 출력 프레임 레이트를 확인(없음)
VSTPLEORADLL int vstpleoraSendSerialPort(unsigned char *cmd, int size);	// (보낸크기)시리얼포트에 명령어 송신(명령어조합, 크기)
/////////////////////////////////////////////////
#endif//VSTUSB_INTERFACE_H
/////////////////////////////////////////////////