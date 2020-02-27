#pragma once
#include "list.h"
#include "ArrayList.h"

/////////////////////////////////////////////////////////////////////////
static const BYTE DETECT_LEFT	= 0x01;	//X��С����
static const BYTE DETECT_RIGHT	= 0x02;	//X���ӷ���
static const BYTE DETECT_UP		= 0x04;	//Y��С����
static const BYTE DETECT_DOWN	= 0x08;	//Y���ӷ���
static const BYTE DETECT_X0		= 0x10;	//X���ֲ���
static const BYTE DETECT_Y0		= 0x20;	//Y���ֲ���
struct PRESET_OBJS1600{static const int COUNT=1600;};
template <class TYPE> class PRESET_ARRAY1600 : public PRESET_ARRAY<TYPE,PRESET_OBJS1600>
{
};
struct PIXEL_RELAOBJ{
	union{
		long hRod;
		long hNode;
	};
	char ciObjType;			//0.�ڵ㣻1.�˼�
	bool bTipPixel;			//�Ƿ�Ϊ��ͷ�㣬false��ʾΪ�˼��м�����ص�
	bool bReverseDirection;	//���������Ƿ���˼�ʼ->�շ����෴
	float zDepth;			//zDepthԽСԽ��ǰ��Խ���ʾԽ����
	PIXEL_RELAOBJ(long hObj=0,float _zDepth=0.0,bool tippoint=false){hRod=hObj;zDepth=_zDepth;bTipPixel=tippoint;bReverseDirection=false;ciObjType=1;}
};
class CMonoImage{
protected:
	bool m_bBlackPixelIsTrue;	//���ر���λΪTrue��ʾ�ڵ�
	bool m_bExternalData;
	long m_nWidth,m_nHeight,m_nEffWidth;
	BYTE* m_lpBitMap;
	BYTE m_ciBitCount;
	PIXEL_RELAOBJ* m_parrRelaObjs;
public:
	CMonoImage(BYTE* lpBitMap=NULL,int width=0,int height=0,bool black_is_true=true,BYTE ciBitCount=1);
	virtual ~CMonoImage();
	virtual void SetRelaObjMap(PIXEL_RELAOBJ* pxRelaObjMap){m_parrRelaObjs=pxRelaObjMap;}
	virtual BYTE BitCount(){return m_ciBitCount;}
	virtual void InitBitImage(BYTE* lpBitMap=NULL,int width=0,int height=0,bool black_is_true=true);
	virtual void MirrorByAxisY();
	//virtual void MirrorRelaObjsByAxisY();
	virtual bool BlackIsTrue(){return m_bBlackPixelIsTrue;}
	virtual bool BlackBitIsTrue(){return m_bBlackPixelIsTrue;}
	virtual bool IsBlackPixel(int i,int j);
	virtual bool SetPixelState(int i,int j,bool black=true);
	virtual bool SetPixelRelaObj(int i,int j,PIXEL_RELAOBJ& relaObj);
	virtual PIXEL_RELAOBJ* GetPixelRelaObj(int i,int j);
	virtual PIXEL_RELAOBJ* GetPixelRelaObjEx(int& i,int& j,BYTE ciRadiusOfSearchNearFrontObj=1,int* piExcludeXI=NULL,int* pjExcludeYJ=NULL);
	virtual BYTE* GetImageMap(){return m_lpBitMap;}
	virtual int GetImageWidth(){return m_nWidth;}		//��ȡͼ��ÿ�е���Ч������
	virtual int GetImageHeight(){return m_nHeight;}		//��ȡͼ��ÿ�е���Ч������
	__declspec(property(get=GetImageWidth)) int Width;
	__declspec(property(get=GetImageHeight)) int Height;
	void DrawLine(const double* start,const double* end,long hRelaRod=0);
	void DrawPoint(const double* point,long hRelaNode=0);
	virtual int WriteMonoBmpFile(const char* fileName);
	static int WriteMonoBmpFile(const char* fileName, unsigned int width,unsigned effic_width, 
							   unsigned int height, unsigned char* image);
};
class CByteMonoImage : public CMonoImage
{	//����̽���ڲ��µ��Ĵ���
public:
	CByteMonoImage(BYTE* lpBitMap=NULL,int width=0,int height=0,bool black_is_true=true);
	virtual ~CByteMonoImage();
	virtual BYTE BitCount(){return 8;}
	struct ISLAND{
		//bool boundary;
		WORD maxy,miny;
		double x,y,count;
		ISLAND(){Clear();}
		void Clear(){x=y=count=maxy=miny=0;}//boundary=false;}
	};	//�ڲ��հ׹µ���������
	struct PIXEL{
		BYTE *pcbPixel;
		PIXEL(BYTE* _pcbImgPixel=0){pcbPixel=_pcbImgPixel;}
		bool get_Black();
		bool set_Black(bool black);
		char set_Connected(char connected);		//�������Ƿ�Ϊ��ͨ���������
		char get_Connected();		//�������Ƿ�Ϊ��ͨ���������
		const static char NONEVISIT	=0;	//��δ���ʵ�������
		const static char CONNECTED	=1;	//��߽���ͨ������
		const static char UNDECIDED	=2;	//�Ѽ�⵽�����أ���״̬δ�ж�
		__declspec(property(put=set_Black,get=get_Black)) bool Black;
		__declspec(property(put=set_Connected,get=get_Connected)) char Connected;
	};
	virtual int DetectIslands(CXhSimpleList<ISLAND>* listIslands);
protected:
	virtual bool IsDetect(int i,int j);
	virtual bool SetDetect(int xi,int yi,bool detect=true);
	virtual void ClearDetectMark();
	virtual bool StatNearWhitePixels(int i,int j,ISLAND* pIsland,PRESET_ARRAY1600<PIXEL>* listStatPixels,BYTE cDetectFlag=0x0f);
};
