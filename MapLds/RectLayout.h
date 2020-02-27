#pragma once
#include "ArrayList.h"
#include "HashTable.h"

struct LAYOUT_COORD2D{
	double x,y;
	LAYOUT_COORD2D(double x=0,double y=0){this->x=x;this->y=y;}
	void Set(double x=0,double y=0){this->x=x;this->y=y;}
	operator double*(){return &x;};			//����һ��˫����ָ��
};

struct SIMPLE_SCOPE{
public:
	double fMinX, fMaxX;
	double fMinY, fMaxY;
	double fMinZ, fMaxZ;
public:
	SIMPLE_SCOPE(){ClearScope();}
	double wide(){return max(0,fMaxX-fMinX);}
	double high(){return max(0,fMaxY-fMinY);}
	double depth(){return max(0,fMaxZ-fMinZ);}
	void ClearScope(){
		fMinX = fMinY = fMinZ =  1.7976931348623158E308;
		fMaxX = fMaxY = fMaxZ = -fMinX;
	}
	void VerifyVertex(const double* pt,int len){
		if(len>=1)
		{
			if(pt[0]<fMinX)
				fMinX = pt[0];
			if(pt[0]>fMaxX)
				fMaxX = pt[0];
		}
		if(len>=2)
		{
			if(pt[1]<fMinY)
				fMinY = pt[1];
			if(pt[1]>fMaxY)
				fMaxY = pt[1];
		}
		if(len>=3)
		{
			if(pt[2]<fMinZ)
				fMinZ = pt[2];
			if(pt[2]>fMaxZ)
				fMaxZ = pt[2];
		}
	}
};

struct DRAWRECT
{
	friend class CDrawingRectLayout;
	//friend class CDrawingRectArray;
	BOOL m_bLayout;					//�ڲ��Ż�����ʱ���ڱ�ʶ�Ƿ��Ѿ�����λ��
	LAYOUT_COORD2D m_PrevTopLeft;	//�ڲ��Ż�����ʱ���ڴ洢ǰһ���ַ��������Ͻ�λ��,�Ա��ڻع�����
public:
	int m_iPageIndex;			//0Ϊ��ʼҳ������
	LAYOUT_COORD2D topLeft;		//���ݾ������²��ֺ�����Ͻ�����λ��
	LAYOUT_COORD2D initTopLeft;	//�����²���ǰ��ͼ���ݾ��εĳ�ʼ���Ͻ����꣬���������²��ֺ�topLeft����ֵһ��ȷ�����ݾ��ε�ƫ����
	LAYOUT_COORD2D LocatePoint(){return LAYOUT_COORD2D(topLeft.x-initTopLeft.x,topLeft.y-initTopLeft.y);}
	double width,height;		//��ͼ���ƾ��εĿ�Ⱥ͸߶�
	//ָ�������CSectionViewDrawing����
	//�޹�������ʱ��ʾ�þ���Ϊ�̶�����
	void *m_pDrawing;	
	DRAWRECT(){m_pDrawing=NULL;m_iPageIndex=0;m_bLayout=FALSE;}
};
//struct SEGREC{SEGI iSeg;int nPageCount;};
class CRectLayout
{
protected:
	long m_idType;
	double m_fFrameWidth;	//�߿���
	double m_fFrameHeight;	//�߿�߶�
	double m_fBlkZoomcoef;	//ԭʼͼԪ�ߴ�/����ͼԪ�ߴ�
	bool IsDefiniteNotEnoughSpace(double width,double height,bool canExtendByX=false,bool canExtendByY=false);
	bool GetBlankRectTopLeft(double width,double height,LAYOUT_COORD2D *topLeft,double marginSpace,bool bSearchHori=true);
	CArray<double,double&>m_arrRowHeight;	//
public:
	static const long TYPE_BASIC = 1;
	PRESET_ARRAY4<LAYOUT_COORD2D> arrOrgOfSubpages;
	bool m_bAutoZoomFixRect;//�涯�������Ź̶�λ�þ���
	bool m_bPlaceFixRect;	//�̶������ȸ߶Ƚ�����ռλ����ҳʱ���ڵ�������ͼ����һ�� wht 18-08-03
	double m_fFixRectWidth;	//�̶�λ�þ��ο�
	double m_fFixRectHeight;//�̶�λ�þ��θ�
	LAYOUT_COORD2D m_FixRectTopLeft;
	static const int FIX_ALIGN_TOP_LEFT		= 1;	//���Ͻ�
	static const int FIX_ALIGN_TOP_RIGHT	= 2;	//���Ͻ�
	static const int FIX_ALIGN_BTM_LEFT		= 3;	//���½�
	static const int FIX_ALIGN_BTM_RIGHT	= 4;	//���½�
	int fixRectAlign;	//�̶�λ�þ��εĶ�λ��ʽDIMALIGN_TYPE
	CRectLayout();
	virtual ~CRectLayout(void);
	long get_idType(){return m_idType;}
	__declspec(property(get=get_idType)) long idType;
	ARRAY_LIST<DRAWRECT> drawRectArr;
	virtual bool Relayout(double h_to_w,double marginSpace,double limitWidth=0,double maxHeight=0);
	double GetFrameWidth(){return m_fFrameWidth;}
	double GetFrameHeight(){return m_fFrameHeight;}
	double GetRealDrawingHeight();
	//����ʵ�ʻ�ͼ�����С�������û�ͼ���������λ��
	void ResetDrawingRectPosition();
};
