
////////////////////////////////////////////////////////////////////
#ifndef IMAGEMGMT_H
#define IMAGEMGMT_H
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
#define IMAGEMGMTDLL	__declspec(dllimport)
#define IMAGEMGMT_LIBNAME "../ImageMgmtLib/ImageMgmt.lib"
#pragma comment(lib, IMAGEMGMT_LIBNAME)
////////////////////////////////////////////////////////////////////
/*typedef struct tagRGBBYTE{
	BYTE b;
	BYTE g;
	BYTE r;
} RGBBYTE; */
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
class ImageMgmt
{
protected:
	//HGLOBAL m_hImage;
	//HGLOBAL m_hUndoImage;
	int m_nWidth;
	int m_nHeight;
	int m_nRWidth;
	int m_nBitCount;
	//CPalette *m_pPal;
public:
	IMAGEMGMTDLL ImageMgmt();
	IMAGEMGMTDLL ~ImageMgmt();
	//IMAGEMGMTDLL void Draw(HDC hDC, int dx, int dy, int dw, int dh, DWORD dwRop = SRCCOPY);
	//IMAGEMGMTDLL BOOL CreateRGBImage(int nWidth, int nHeight, BYTE** pImg, BOOL dib=TRUE);
	//IMAGEMGMTDLL BOOL CreateRGBImage(int nWidth, int nHeight, BYTE* pImg, BOOL dib=TRUE);	
	//IMAGEMGMTDLL BOOL WriteBitmapFile(LPCTSTR lpszFileName);
	//IMAGEMGMTDLL unsigned char *ReadBitmapFileTo1D(LPCTSTR lpszFileName, int *wdith, int *height, int *band);
	IMAGEMGMTDLL void ReadBitmapFileFree(unsigned char *raw);
	//IMAGEMGMTDLL unsigned char **ReadBitmapFileTo2D(LPCTSTR lpszFileName, int *wdith, int *height, int *band);
	IMAGEMGMTDLL void ReadBitmapFileFree(unsigned char **raw, int height);
	//IMAGEMGMTDLL BOOL WriteBitmapFile(LPCTSTR lpszFileName, unsigned char *raw, int width, int height, int band);
	//IMAGEMGMTDLL BOOL WriteBitmapFile(LPCTSTR lpszFileName, unsigned char **raw, int width, int height, int band);
};

#endif //IMAGEMGMT_H
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
