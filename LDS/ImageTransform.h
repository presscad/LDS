#pragma once
#include "f_ent.h"

#if defined(__TMA_)||defined(__LMA_)
class CSamplePixelLine
{
public:
	int m_nValidBlackSeg;	//象素线内检测到黑色线段的数量
	//[i][0]表示第i段的起始点位，[i][1]表示第i段的终止点位
	int shiftSegArr[5][2];
	int m_iPosIndex;
public:
	CSamplePixelLine();
	~CSamplePixelLine();
	//pixels中true表示黑点;false表示白点
	bool AnalyzeLinePixels(bool* pixels,int nNum);
	bool FindBlackSegInSpecScope(int scope_start,int scope_end,int* seg_start, int* seg_end);
};
class CImageTransform  
{
	LPBITMAPINFOHEADER m_lpBMIH;	//图像的表头信息（含调色板数据区）
	BYTE *m_lpBits;					//行扫描形式的位图字节数据
	BYTE *m_lpOrgBits;				//行扫描形式的位图字节数据
	BYTE *m_lpPixelGrayThreshold;	//记录每个像素黑白分界灰度值
	BYTE *m_lpGrayBitsMap;			//单字节灰度型的图像像素数据表，主要用于图像处理
	BYTE *m_lpDesBitstMap;			//截取出来的目标区域
	int m_nWidth,m_nHeight;			//图片的宽度和高度(像素单位)
	int m_nDrawingRgnWidth,m_nDrawingRgnHeight;//图片的宽度和高度(像素单位)
	double kesai[4];				//ψ[4]四边形与矩形空间变换的参数
	int m_nEffByteWidth;			//图片行对齐后的实际字节数（4 byte对齐）
	UINT m_uBitcount;				//每个显示象素的颜色显示位数：1为二色;4为16色;8为256色;16、24、32为真彩色
	POINT m_leftUp,m_leftDown,m_rightUp,m_rightDown;	//工艺卡边框的四个点
	BYTE m_ubGreynessThreshold;		//在进行二值化(黑白二色)处理时的阈值，灰度大于此值为白点，小于此值为黑点
private:
	bool ReadBmpFile(FILE* fp);
	bool WriteBmpFile(FILE* fp);
	bool ReadJpegFile(FILE* fp);
	bool WriteJpegFile(FILE* fp);
	bool ReadPngFile(FILE* fp);
	bool WritePngFile(FILE* fp);
	void InitPixelGrayThreshold();
	void CalRectPixelGrayThreshold(int iRow,int iCol,int RECT_WIDTH,int RECT_HEIGHT);
public:
	int m_nRectColNum,m_nRectRowNum;
	int m_nRectWidth,m_nRectHeight;
	int GetRectPixelGrayThreshold(int iRow,int iCol);
	void SetRectPixelGrayThreshold(int iRow,int iCol,int threshold);
public:
	CImageTransform();
	virtual ~CImageTransform();
	//
	LPBITMAPINFOHEADER GetBitmapInfoHeader(){return m_lpBMIH;}
	BYTE *GetBits(){return m_lpBits;}
	BYTE *GetOrgBits(){return m_lpOrgBits;}
	BYTE *GetDesBitstMap(){return m_lpDesBitstMap;}
	UINT GetBitCount(){return m_uBitcount;}
	int GetWidth(){return m_nWidth;}
	int GetEffWidth(){return m_nEffByteWidth;}
	int GetHeight(){return m_nHeight;}
	int GetDrawingWidth(){return m_nDrawingRgnWidth;}
	int GetDrawingHeight(){return m_nDrawingRgnHeight;}
	//
	bool ProcessImage();					//保存一个灰度图像
	BYTE CalGreynessThreshold();			//计算灰度阈值
	bool CalDrawingRgnRect();				//计算有效区域的矩形
	void LocateDrawingRgnRect();			//定位有效区域（工艺卡图框）的矩形
	void LocateDrawingRgnRect(CPoint leftU,CPoint leftD,CPoint rightU,CPoint rightD);
	BYTE GetPixelGrayness(int x,int y);		//指定象素点的灰度值
	BYTE GetPixelGraynessThresold(int x,int y);	//指定像素点黑白阈值
	bool IsBlackPixel(int x,int y);				//指定象素点是否为黑点
	bool ReadImageFile(const char* file_path);	//读取图片
	bool WriteImageFile(const char* file_path);	//保存图片
	bool GapDetect(int start_x,int start_y,int maxGap,bool bScanByRow,int *gaplen);	//出现断点后是否延续的判断
	bool RetrieveSpecifyRegion( double pos_x_coef,double pos_y_coef,double width_coef,double height_coef, 
		BYTE** outBitsMap,long *outWidth,long *effWidth,long *outHeight);	//识别指定区域
	POINT TransformPointToOriginalField(POINT p);	//将指定区域点转换到原始图像中
	int StretchBlt(HDC hDC,CRect imageShowRect,DWORD rop);
};
#endif
