#pragma once
#include "f_ent.h"

#if defined(__TMA_)||defined(__LMA_)
class CSamplePixelLine
{
public:
	int m_nValidBlackSeg;	//�������ڼ�⵽��ɫ�߶ε�����
	//[i][0]��ʾ��i�ε���ʼ��λ��[i][1]��ʾ��i�ε���ֹ��λ
	int shiftSegArr[5][2];
	int m_iPosIndex;
public:
	CSamplePixelLine();
	~CSamplePixelLine();
	//pixels��true��ʾ�ڵ�;false��ʾ�׵�
	bool AnalyzeLinePixels(bool* pixels,int nNum);
	bool FindBlackSegInSpecScope(int scope_start,int scope_end,int* seg_start, int* seg_end);
};
class CImageTransform  
{
	LPBITMAPINFOHEADER m_lpBMIH;	//ͼ��ı�ͷ��Ϣ������ɫ����������
	BYTE *m_lpBits;					//��ɨ����ʽ��λͼ�ֽ�����
	BYTE *m_lpOrgBits;				//��ɨ����ʽ��λͼ�ֽ�����
	BYTE *m_lpPixelGrayThreshold;	//��¼ÿ�����غڰ׷ֽ�Ҷ�ֵ
	BYTE *m_lpGrayBitsMap;			//���ֽڻҶ��͵�ͼ���������ݱ���Ҫ����ͼ����
	BYTE *m_lpDesBitstMap;			//��ȡ������Ŀ������
	int m_nWidth,m_nHeight;			//ͼƬ�Ŀ�Ⱥ͸߶�(���ص�λ)
	int m_nDrawingRgnWidth,m_nDrawingRgnHeight;//ͼƬ�Ŀ�Ⱥ͸߶�(���ص�λ)
	double kesai[4];				//��[4]�ı�������οռ�任�Ĳ���
	int m_nEffByteWidth;			//ͼƬ�ж�����ʵ���ֽ�����4 byte���룩
	UINT m_uBitcount;				//ÿ����ʾ���ص���ɫ��ʾλ����1Ϊ��ɫ;4Ϊ16ɫ;8Ϊ256ɫ;16��24��32Ϊ���ɫ
	POINT m_leftUp,m_leftDown,m_rightUp,m_rightDown;	//���տ��߿���ĸ���
	BYTE m_ubGreynessThreshold;		//�ڽ��ж�ֵ��(�ڰ׶�ɫ)����ʱ����ֵ���Ҷȴ��ڴ�ֵΪ�׵㣬С�ڴ�ֵΪ�ڵ�
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
	bool ProcessImage();					//����һ���Ҷ�ͼ��
	BYTE CalGreynessThreshold();			//����Ҷ���ֵ
	bool CalDrawingRgnRect();				//������Ч����ľ���
	void LocateDrawingRgnRect();			//��λ��Ч���򣨹��տ�ͼ�򣩵ľ���
	void LocateDrawingRgnRect(CPoint leftU,CPoint leftD,CPoint rightU,CPoint rightD);
	BYTE GetPixelGrayness(int x,int y);		//ָ�����ص�ĻҶ�ֵ
	BYTE GetPixelGraynessThresold(int x,int y);	//ָ�����ص�ڰ���ֵ
	bool IsBlackPixel(int x,int y);				//ָ�����ص��Ƿ�Ϊ�ڵ�
	bool ReadImageFile(const char* file_path);	//��ȡͼƬ
	bool WriteImageFile(const char* file_path);	//����ͼƬ
	bool GapDetect(int start_x,int start_y,int maxGap,bool bScanByRow,int *gaplen);	//���ֶϵ���Ƿ��������ж�
	bool RetrieveSpecifyRegion( double pos_x_coef,double pos_y_coef,double width_coef,double height_coef, 
		BYTE** outBitsMap,long *outWidth,long *effWidth,long *outHeight);	//ʶ��ָ������
	POINT TransformPointToOriginalField(POINT p);	//��ָ�������ת����ԭʼͼ����
	int StretchBlt(HDC hDC,CRect imageShowRect,DWORD rop);
};
#endif
