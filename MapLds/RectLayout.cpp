#include "stdafx.h"
#include "RectLayout.h"
#include "SortFunc.h"
#include "NumberToolKits.h"

CRectLayout::CRectLayout(){
	m_idType=1;
	m_bAutoZoomFixRect=false;
	m_bPlaceFixRect=false;
	fixRectAlign=FIX_ALIGN_TOP_RIGHT;
	m_fFixRectWidth=m_fFixRectHeight=m_fFrameWidth=m_fFrameHeight=0;
	m_fBlkZoomcoef=0;
}

CRectLayout::~CRectLayout()
{

}
static BOOL IsOrthoRectInters(const double* pxTopLeft1,double width1,double height1,const double* pxTopLeft2,double width2,double height2)
{
	if((width1==0&&height1==0)||(width2==0&&height2==0))
		return false;
	if(pxTopLeft2[0]>pxTopLeft1[0]+width1)
		return FALSE;	//�ڶ��������ڵ�һ�����ε��Ҳ�
	else if(pxTopLeft2[1]<pxTopLeft1[1]-height1)
		return FALSE;	//�ڶ��������ڵ�һ�����ε��²�
	else if(pxTopLeft2[0]+width2<pxTopLeft1[0])
		return FALSE;	//�ڶ��������ڵ�һ�����ε����
	else if(pxTopLeft2[1]-height2>pxTopLeft1[1])
		return FALSE;	//�ڶ��������ڵ�һ�����ε��ϲ�
	else
		return TRUE;
}
bool CRectLayout::IsDefiniteNotEnoughSpace(double width,double height,bool canExtendByX/*=false*/,bool canExtendByY/*=false*/)
{
	if(!canExtendByX&&width>m_fFrameWidth)
		return true;
	if(!canExtendByY&&height>m_fFrameHeight)
		return true;
	return false;
}
bool CRectLayout::GetBlankRectTopLeft(double width,double height,LAYOUT_COORD2D *topLeft,double marginSpace,bool bSearchHori/*=true*/)
{
	int i,j,k,nLayoutNum=0;
	int maxY=0;
	//��֤stepX��stepY��Ϊ0 wht 11-06-16
	double stepX=max(0.5,m_fFrameWidth/100),stepY=max(0.5,m_fFrameHeight/100);
	double zoomCoefOfFixRect=m_bAutoZoomFixRect?1.0:m_fBlkZoomcoef;
	LAYOUT_COORD2D origin;
	for(j=0;;j++)
	{
		origin.y=-j*stepY;
		if(origin.y-marginSpace-height<-m_fFrameHeight)
			break;
		for(i=0;;i++)	//����������
		{
			origin.x=i*stepX;
			if(origin.x+marginSpace+width>m_fFrameWidth)
				break;
			BOOL bHasInters=IsOrthoRectInters(m_FixRectTopLeft,m_fFixRectWidth*zoomCoefOfFixRect,m_fFixRectHeight*zoomCoefOfFixRect,origin,width,height);
			if(!bHasInters)
			{
				for(k=0;k<drawRectArr.GetSize();k++)
				{
					if(!drawRectArr[k].m_bLayout)
						continue;	//δ����
					nLayoutNum++;	//�Ѳ��ù��ľ��θ���
					LAYOUT_COORD2D xCornerPoint(drawRectArr[k].topLeft.x,drawRectArr[k].topLeft.y);
					if(IsOrthoRectInters(xCornerPoint,drawRectArr[k].width,drawRectArr[k].height,origin,width,height))
					{
						bHasInters=TRUE;
						break;
					}
				}
			}
			if(!bHasInters)
			{
				if(topLeft)
					*topLeft=origin;
				return TRUE;
			}
		}
		if(!bSearchHori)
			break;	//�����к�������
	}
	return FALSE;
}
static int compareDrawingRect(const DRAWRECT &drawRect1,const DRAWRECT &drawRect2)
{
	if(drawRect1.width>drawRect2.width)
		return -1;
	else if(drawRect1.width<drawRect2.width)
		return 1;
	else if(drawRect1.height>drawRect2.height)
		return -1;
	else if(drawRect1.height<drawRect2.height)
		return 1;
	else
		return 0;
}
bool CRectLayout::Relayout(double h_to_w,double marginSpace,double limitWidth/*=0*/,double maxHeight/*=0*/)
{
	if(drawRectArr.GetSize()==0)
		return false;
	int i;
	//����ͼ���ƾ��ΰ��ճ�/���ɴ�С���������Ա���һ���Զ��Ż�����
	CHeapSort<DRAWRECT>::HeapSort(drawRectArr.m_pData,drawRectArr.GetSize(),compareDrawingRect);
	m_fFrameWidth=drawRectArr[0].width+m_fFixRectWidth;	//���Ʊ߿��ʼ��
	m_fFrameHeight=drawRectArr[0].height;				//���Ʊ߿��ʼ��
	for(i=1;i<drawRectArr.GetSize();i++)
		m_fFrameHeight+=drawRectArr[i].height;
	//ȡ������ͼ���Ӹ߶���̶����Ϳ�߶��ؽϴ�����Ϊ���Ʊ߿��ʼ�߶�
	if(m_fFixRectHeight>m_fFrameHeight)
		m_fFrameHeight=m_fFixRectHeight;
	if(m_fFrameHeight/m_fFrameWidth>h_to_w)	//���߿�ĸ߿�ȼ���,�߿���Ҫ�ӿ�
	{
		m_fFrameHeight+=2*marginSpace;
		m_fFrameWidth=m_fFrameHeight/h_to_w;
	}
	else
	{
		m_fFrameWidth+=2*marginSpace;
		m_fFrameHeight=m_fFrameWidth*h_to_w;	//���߿�ĸ߿�ȼ���,�߿���Ҫ�Ӹ�
	}
	//�Բ�ͼ���ƾ��εĲ��÷��������Ż�(��ʼ״̬�ǰ�һ�ж��в���)
	double stepH=drawRectArr[drawRectArr.GetSize()-1].height;
	double stepW=stepH/h_to_w;
	#define    STEP_EPS              1e-9
	if(stepH<=STEP_EPS||stepW<=STEP_EPS)
		return false;
	double minStepLimitW=m_fFrameWidth/100;

	while(1)
	{
		m_arrRowHeight.RemoveAll();
		for(i=0;i<drawRectArr.GetSize();i++)
		{
			if(drawRectArr[i].m_pDrawing)
				drawRectArr[i].m_bLayout=FALSE;
		}
		if(fixRectAlign==FIX_ALIGN_TOP_LEFT)
			m_FixRectTopLeft.Set(0,0);
		else //if(fixRectAlign==TopRight)
			m_FixRectTopLeft.Set(m_fFrameWidth-m_fFixRectWidth*m_fBlkZoomcoef,0);
		for(i=0;i<drawRectArr.GetSize();i++)
		{
			bool bSearchHori=true;
			if(drawRectArr[i].width/drawRectArr[i].height>3)
				bSearchHori=false;
			if(!GetBlankRectTopLeft(drawRectArr[i].width,drawRectArr[i].height,&drawRectArr[i].topLeft,marginSpace,bSearchHori))
				break;	//�ռ��Ѿ�������,�ع�����һ���ַ���
			drawRectArr[i].m_bLayout=TRUE;
		}
		if(i==drawRectArr.GetSize())
		{	//���ҵ��µĸ��Ų��ַ���,���м����Ż����ֵĿ���
			m_fFrameWidth -=stepW;
			m_fFrameHeight-=stepH;
			for(i=0;i<drawRectArr.GetSize();i++)
				drawRectArr[i].m_PrevTopLeft=drawRectArr[i].topLeft;
		}
		else//���Ҳ������µĸ��Ż����ַ���,Ӧ�ع���ǰһ��Ч����
		{
			for(i=0;i<drawRectArr.GetSize();i++)
			{
				drawRectArr[i].topLeft=drawRectArr[i].m_PrevTopLeft;
			}
			if(stepW>minStepLimitW)
			{
				stepW*=0.5;
				stepH*=0.5;
				m_fFrameWidth +=stepW;
				m_fFrameHeight+=stepH;
			}
			else
			{
				m_fFrameWidth +=stepW;
				m_fFrameHeight+=stepH;
				break;
			}
		}
	}
	//����ʵ�ʻ�ͼ�����С���²��ֻ�ͼ���������λ��,�ⲿ�ִ���������д��������δ���ǵ�������ϸ��
	ResetDrawingRectPosition();	
	return true;
}

double CRectLayout::GetRealDrawingHeight()
{
	if(drawRectArr.GetSize()<=0)
		return 0;
	CMinD minY;
	CMaxD maxY;
	for(int i=0;i<drawRectArr.GetSize();i++)
	{
		minY.Update(drawRectArr[i].topLeft.y);
		maxY.Update(drawRectArr[i].topLeft.y);
		minY.Update(drawRectArr[i].topLeft.y-drawRectArr[i].height);
		maxY.Update(drawRectArr[i].topLeft.y-drawRectArr[i].height);
	}
	return (maxY.number-minY.number);
}
//����ʵ�ʻ�ͼ�����С���²��ֻ�ͼ���������λ��
void CRectLayout::ResetDrawingRectPosition()
{
	int i=0;
	SIMPLE_SCOPE scope;
	for(i=0;i<drawRectArr.GetSize();i++)
	{
		LAYOUT_COORD2D topLeft=drawRectArr[i].topLeft;
		LAYOUT_COORD2D bottomRight(topLeft.x+drawRectArr[i].width,topLeft.y-drawRectArr[i].height);
		scope.VerifyVertex(topLeft,2);
		scope.VerifyVertex(bottomRight,2);
	}
	double fScopeHeight=scope.high();	//��ͼ������ʵ�߶�
	double fScopeWidth=scope.wide();	//��ͼ�������ӿ��
	if(fScopeHeight>m_fFrameHeight||fScopeWidth>m_fFrameWidth)
		return;	
	LAYOUT_COORD2D offset_pt(0.5*(m_fFrameWidth-fScopeWidth),-0.5*(m_fFrameHeight-fScopeHeight));//ƫ�Ƶ�
	if(!m_bPlaceFixRect||m_fFixRectWidth==0)
		offset_pt.x=0;

	//������ο�λ��
	for(i=0;i<drawRectArr.GetSize();i++)
	{
		drawRectArr[i].topLeft.x+=offset_pt.x;
		drawRectArr[i].topLeft.y+=offset_pt.y;
	}
}