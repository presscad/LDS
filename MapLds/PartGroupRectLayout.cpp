#include "stdafx.h"
#include "PartGroupRectLayout.h"
#include "PartGroupDrawing.h"
#include "SortFunc.h"

//////////////////////////////////////////////////////////////////////////
// CPartGroupRectLayout
CPartGroupRectLayout::CPartGroupRectLayout()
{
	m_idType=TYPE_DEFAULT;
	m_pPartGroupDrawing=NULL;
	draw_rc_width=draw_rc_height=0;
}
CLDSPartGroup *CPartGroupRectLayout::GetPartGroup()
{
	return m_pPartGroupDrawing?m_pPartGroupDrawing->m_pPartGroup:NULL;
}
CLDSPart *CPartGroupRectLayout::GetRootPart()
{
	return m_pPartGroupDrawing?m_pPartGroupDrawing->m_pRootPart:NULL;
}
void CPartGroupRectLayout::SetPartGroupDrawing(CPartGroupDrawing *pPartGroupDrawing)
{
	if(pPartGroupDrawing)
	{
		m_pPartGroupDrawing=pPartGroupDrawing;
		draw_rc_width=m_pPartGroupDrawing->m_fMaxDrawingLength;
		draw_rc_height=m_pPartGroupDrawing->m_fMaxDrawingHeight;
	}
	else
	{
		m_pPartGroupDrawing=NULL;
		draw_rc_width=draw_rc_height=0;
	}
}
bool CPartGroupRectLayout::InitDrawingRectLayout(int iPageIndex/*=-1*/)
{
	if(m_pPartGroupDrawing==NULL)
		return false;
	drawRectArr.Empty();
	//1.1 ��ӹ̶�����(���ڻ����麸����ϸ��)����ڵ��麸���Ľ�����Ϣ��
	//1.2 ���������ͼ
	double table_width=0,table_height=0;
	CSectionViewDrawing* pViewDrawing=NULL;
	CPartGroupTblDrawing* pTblDrawing=NULL;
	bool bPlaceFixRect=false;
	for(pViewDrawing=(CSectionViewDrawing*)m_pPartGroupDrawing->EnumFirstDrawingComponent();pViewDrawing;
		pViewDrawing=(CSectionViewDrawing*)m_pPartGroupDrawing->EnumNextDrawingComponent())
	{
		if(pViewDrawing->GetComponentType()==CSectionViewDrawing::CLS_BOM_VIEW)
		{	//ÿҳ�ж�Ӧ������ϸ����
			pTblDrawing=(CPartGroupTblDrawing*)pViewDrawing;
			pTblDrawing->CreatePartGroupTable(1);
			f2dRect table_rect=pTblDrawing->GetDrawRegion();
			table_width=table_rect.Width();
			table_height+=table_rect.Height();
			bPlaceFixRect=(iPageIndex!=-1&&pViewDrawing->drawingRect.m_iPageIndex!=iPageIndex);
		}
		else
		{
			if(iPageIndex!=-1&&pViewDrawing->drawingRect.m_iPageIndex!=iPageIndex)
				continue;
			f2dRect view_draw_rect;
			view_draw_rect=pViewDrawing->GetDrawRegion(true);
			DRAWRECT drawingRect;
			drawingRect.m_pDrawing=pViewDrawing;
			drawingRect.height=view_draw_rect.Height();
			drawingRect.width=view_draw_rect.Width();
			drawingRect.initTopLeft.Set(view_draw_rect.topLeft.x,view_draw_rect.topLeft.y);
			drawingRect.topLeft=drawingRect.initTopLeft;
			drawRectArr.append(drawingRect);
		}
	}
	m_bPlaceFixRect=bPlaceFixRect;
	m_fFixRectWidth=table_width;
	m_fFixRectHeight=table_height;
	fixRectAlign=CRectLayout::FIX_ALIGN_TOP_RIGHT;
	return true;
}
bool CPartGroupRectLayout::Relayout(double h_to_w,double marginSpace,double limitWidth/*=0*/,double maxHeight/*=0*/)
{
	return CRectLayout::Relayout(h_to_w,marginSpace,limitWidth,maxHeight);
}
double _LocalRoundingDrawScale(double scaleOfModel2Draw)
{
	if(scaleOfModel2Draw<=2.02)
		scaleOfModel2Draw=2;
	//else if(scaleOfModel2Draw<=2.53)
	//	scaleOfModel2Draw=2.5;
	else if(scaleOfModel2Draw<=3.03)
		scaleOfModel2Draw=3;
	else if(scaleOfModel2Draw<=4.04)
		scaleOfModel2Draw=4;
	else if(scaleOfModel2Draw<=5.05)
		scaleOfModel2Draw=5;
	else if(scaleOfModel2Draw<=6.06)
		scaleOfModel2Draw=6;
	else if(scaleOfModel2Draw<=8.08)
		scaleOfModel2Draw=8;
	else if(scaleOfModel2Draw<=10.1)
		scaleOfModel2Draw=10;
	else if(scaleOfModel2Draw<=15.15)
		scaleOfModel2Draw=15;
	else if(scaleOfModel2Draw<=20.2)
		scaleOfModel2Draw=20;
	else
		scaleOfModel2Draw=30;
	return scaleOfModel2Draw;
}

int CPartGroupRectLayout::Relayout(ARRAY_LIST<double> &zoomdraw_coef_arr)
{
	//2.�Զ��Ż�����
	double h_to_w=draw_rc_height/draw_rc_width;
	double dfAverageScaleOfModel2Draw=0;
	bool blScaleConverge=true;
	do{
		//1.��ʼ����ͼ���ֲ���()
		InitDrawingRectLayout();
		Relayout(h_to_w,CPartGroupDrawingLayoutOper::MARGIN_SPACE,draw_rc_width,draw_rc_height);
		double zoomdraw_coef=draw_rc_width/this->m_fFrameWidth;
		if(!sys.general_set.bProcessCardTbl)
			zoomdraw_coef=1.0;	//�޹��տ�ͼ�����ƿ�ֻ���ֲ����� wjh-2020.1.3
		//3.��ʼ����ͼ��ͼ����
		blScaleConverge=true;
		dfAverageScaleOfModel2Draw=0;
		for(int ii=0;ii<drawRectArr.GetSize();ii++)
		{
			DRAWRECT *pDrawingRect=&drawRectArr[ii];
			CSectionViewDrawing *pViewDrawing=(CSectionViewDrawing*)pDrawingRect->m_pDrawing;
			pViewDrawing->drawingRect=*pDrawingRect;
			if(pViewDrawing->m_iComponentType== CSectionViewDrawing::CLS_ANGLE_SPREAD_VIEW)
				continue;	//�Ǹ�չ��ͼ���ݹ��տ������Զ��������ű���
			if(!sys.general_set.bProcessCardTbl)
				continue;
			double dfActualScaleOfModel2Draw=pViewDrawing->dfScaleOfModel2Draw/zoomdraw_coef;
			double dfRoundScale=_LocalRoundingDrawScale(dfActualScaleOfModel2Draw);
			dfAverageScaleOfModel2Draw+=pViewDrawing->dfScaleOfModel2Draw/dfRoundScale;
			double dfZoomCoef=pViewDrawing->dfScaleOfModel2Draw/dfRoundScale;
			blScaleConverge=blScaleConverge&&fabs(dfZoomCoef-1)<0.1;
			pViewDrawing->ZoomDrawing(dfZoomCoef);
		}
	}while(!blScaleConverge);
	//�麸��ϸ������
	dfAverageScaleOfModel2Draw/=drawRectArr.GetSize();
	for(CSectionViewDrawing* pBomDrawing=(CSectionViewDrawing*)this->m_pPartGroupDrawing->EnumFirstDrawingComponent();pBomDrawing;
		pBomDrawing=(CSectionViewDrawing*)this->m_pPartGroupDrawing->EnumNextDrawingComponent())
	{
		if(pBomDrawing->m_iComponentType==CSectionViewDrawing::CLS_BOM_VIEW)
			pBomDrawing->ZoomDrawing(dfAverageScaleOfModel2Draw);
	}
	zoomdraw_coef_arr.append(1.0);
	return 1;
}
//////////////////////////////////////////////////////////////////////////
//
C3ViewsDrawingRectLayout::C3ViewsDrawingRectLayout()
{
	m_idType=TYPE_3VIEW;
}
bool C3ViewsDrawingRectLayout::Relayout(double h_to_w,double marginSpace,double limitWidth/*=0*/,double maxHeight/*=0*/)
{
	DRAWRECT *pFrontViewRect=NULL,*pSideViewRect=NULL,*pTopViewRect=NULL;
	int i;
	for(i=0;i<drawRectArr.GetSize();i++)
	{
		CSectionViewDrawing *pDrawing=(CSectionViewDrawing*)drawRectArr[i].m_pDrawing;
		CSectionView *pView=pDrawing->GetSectionView();
		BYTE cSubType=0;
		if(pView)
			cSubType=pView->GetViewSubType();
		if(cSubType=='F')
			pFrontViewRect=&drawRectArr[i];
		else if(cSubType=='S')
			pSideViewRect=&drawRectArr[i];
		else if(cSubType=='D')
			pTopViewRect=&drawRectArr[i];
	}
	if(pFrontViewRect==NULL||pSideViewRect==NULL||pTopViewRect==NULL)
		return false;

	double dfLockedRectWidth=this->m_bPlaceFixRect?m_fFixRectWidth:0;
	double dfLockedRectHeight=this->m_bPlaceFixRect?m_fFixRectHeight:0;
	m_fFrameWidth=pFrontViewRect->width+pSideViewRect->width+dfLockedRectWidth;	//���Ʊ߿��ʼ��
	m_fFrameHeight=pFrontViewRect->height+pTopViewRect->height;					//���Ʊ߿��ʼ��
	//ȡ������ͼ���Ӹ߶���̶����Ϳ�߶��ؽϴ�����Ϊ���Ʊ߿��ʼ�߶�
	if(dfLockedRectHeight>m_fFrameHeight)
		m_fFrameHeight=dfLockedRectHeight;
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
	pFrontViewRect->topLeft.Set(0,0);
	pSideViewRect->topLeft.Set(pFrontViewRect->width+marginSpace,0);
	pTopViewRect->topLeft.Set(0,-1*(pFrontViewRect->height+marginSpace));
	//����ʵ�ʻ�ͼ�����С���²��ֻ�ͼ���������λ��,�ⲿ�ִ���������д��������δ���ǵ�������ϸ��
	ResetDrawingRectPosition();
	return true;
}

//////////////////////////////////////////////////////////////////////////
CCommonPlateDrawingRectLayout::CCommonPlateDrawingRectLayout()
{
	m_idType=TYPE_COMMON_PLATE;
}
bool CCommonPlateDrawingRectLayout::Relayout(double h_to_w,double marginSpace,double limitWidth/*=0*/,double maxHeight/*=0*/)
{
	CArray<DRAWRECT,DRAWRECT&> drawKaoMeRectArr;
	DRAWRECT xFrontRect,xSideRect,xPlanRect;
	double fFrameHigh=0,fFrameWidth=0,fKaoMeWidth=0;
	for(int i=0;i<drawRectArr.GetSize();i++)
	{
		drawRectArr[i].m_bLayout=FALSE;
		CSectionViewDrawing* pViewDrawing=(CSectionViewDrawing*)drawRectArr[i].m_pDrawing;
		CSectionView* pView=pViewDrawing->GetSectionView();
		if(strcmp(pView->sName,"����ͼ")==0)
		{
			xFrontRect=drawRectArr[i];
			fFrameHigh+=drawRectArr[i].height;
			fFrameWidth+=drawRectArr[i].width;
		}
		else if(strcmp(pView->sName,"����ͼ")==0)
		{
			xSideRect=drawRectArr[i];
			fFrameWidth+=drawRectArr[i].width;
		}
		else if(strcmp(pView->sName,"����ͼ")==0)
		{
			xPlanRect=drawRectArr[i];
			fFrameHigh+=drawRectArr[i].height;
		}
		else if(strstr(pView->sName,"��ģ�Ǹ�"))
		{
			fFrameHigh+=drawRectArr[i].height;
			fKaoMeWidth+=drawRectArr[i].width;
			drawKaoMeRectArr.Add(drawRectArr[i]);
		}
	}
	if(xFrontRect.m_pDrawing==NULL || 
		(xSideRect.m_pDrawing==NULL&&xPlanRect.m_pDrawing==NULL))
		return CRectLayout::Relayout(h_to_w,marginSpace);
	//�����������Ŀ�Ⱥ͸߶�
	m_fFrameWidth=m_fFixRectWidth+max(fKaoMeWidth,fFrameWidth);
	//m_fFrameWidth+=max(m_fFixRectHeight,fFrameHigh);	//֮ǰ�����ǲ��Ǵ��ˣ� wjh-2019.5.13
	m_fFrameHeight=max(m_fFixRectHeight,fFrameHigh);
	if(m_fFrameHeight/m_fFrameWidth>h_to_w)	
	{	//���߿�ĸ߿�ȼ���,�߿���Ҫ�ӿ�
		m_fFrameHeight+=2*marginSpace;
		m_fFrameWidth=m_fFrameHeight/h_to_w;
	}
	else
	{	//���߿�ĸ߿�ȼ���,�߿���Ҫ�Ӹ�
		m_fFrameWidth+=2*marginSpace;
		m_fFrameHeight=m_fFrameWidth*h_to_w;
	}
	//����ͼ
	drawRectArr.Empty();
	LAYOUT_COORD2D origin(0.5*marginSpace,-0.5*marginSpace);
	xFrontRect.topLeft=origin;
	xFrontRect.m_bLayout=TRUE;
	drawRectArr.append(xFrontRect);
	//����ͼ
	if(xSideRect.m_pDrawing)
	{
		origin=xFrontRect.topLeft;
		origin.x+=xFrontRect.width+0.5*marginSpace;
		origin.y-=0.5*xFrontRect.height-0.5*xSideRect.height;
		xSideRect.topLeft=origin;
		xSideRect.m_bLayout=TRUE;
		drawRectArr.append(xSideRect);
	}
	//����ͼ
	if(xPlanRect.m_pDrawing)
	{
		origin=xFrontRect.topLeft;
		origin.x+=0.5*xFrontRect.width-0.5*xPlanRect.width;
		origin.y-=xFrontRect.height+0.5*marginSpace;
		xPlanRect.topLeft=origin;
		xPlanRect.m_bLayout=TRUE;
		drawRectArr.append(xPlanRect);
		origin.y-=xPlanRect.height+marginSpace;
	}
	//��ģ�Ǹ�
	origin.x=xFrontRect.topLeft.x;
	for(int i=0;i<drawKaoMeRectArr.GetSize();i++)
	{
		drawKaoMeRectArr[i].topLeft=origin;
		drawKaoMeRectArr[i].m_bLayout=TRUE;
		origin.x+=drawKaoMeRectArr[i].width+0.5*marginSpace;
		drawRectArr.append(drawKaoMeRectArr[i]);
	}
	//����ʵ�ʻ�ͼ�����С���²��ֻ�ͼ���������λ��,�ⲿ�ִ���������д��������δ���ǵ�������ϸ��
	ResetDrawingRectPosition();	
	return true;
}
//////////////////////////////////////////////////////////////////////////
CCommonTubeDrawingRectLayout::CCommonTubeDrawingRectLayout(CPartGroupDrawing *pPartGroupDrawing/*=NULL*/)
{
	m_idType=TYPE_COMMON_TUBE;
	m_fDatumZoomCoef=0;
	m_iDatumPage=-1;
	m_bZoomRelayout=false;
	m_bCompulsoryHoriLayout=false;
	m_fSubRectMaxHigh=0;
	m_fSubRectMaxUpHigh=m_fSubRectMaxDownHigh=0;
	if(pPartGroupDrawing)
	{
		SetPartGroupDrawing(pPartGroupDrawing);
		InitDrawingRectLayout();
	}
}
static int compareDrawingRect3(const SLAVE_DRAWING_RECT &drawRect1,const SLAVE_DRAWING_RECT &drawRect2)
{
	if(drawRect1.fDistX>drawRect2.fDistX)
		return 1;
	else if(drawRect1.fDistX<drawRect2.fDistX)
		return -1;
	else
		return 0;
}
bool CCommonTubeDrawingRectLayout::IsSlaveViewException()
{
	if(drawMainRectArr.GetSize()==0&&drawSubRectArr.GetSize()==0)
		return (Init()==false);
	else
		return (slaveDrawRectArr.GetSize()!=drawSubRectArr.GetSize());
}
bool CCommonTubeDrawingRectLayout::Init()
{	//1.����ͼ���з��ࣺ����ͼ������ͼ
	drawMainRectArr.RemoveAll();
	drawSubRectArr.RemoveAll();
	m_pMainViewDrawing=NULL;
	CSectionView* pView=NULL;
	CSectionViewDrawing*pViewDrawing=NULL;
	for(int i=0;i<drawRectArr.GetSize();i++)
	{
		drawRectArr[i].m_bLayout=FALSE;
		pViewDrawing=(CSectionViewDrawing*)drawRectArr[i].m_pDrawing;
		pView=pViewDrawing->GetSectionView();
		if(pView->IsSlaveView()&&strlen(pView->Symbol)>0)
			drawSubRectArr.Add(drawRectArr[i]);		//����ͼ
		else	//������ͼ
		{
			drawMainRectArr.Add(drawRectArr[i]);
			if( m_pMainViewDrawing==NULL&&
				(pView->m_iViewType==VIEW_MAIN||pView->m_iViewType==VIEW_JOINTTUBE||
				pView->m_iViewType==VIEW_FRONT))
			{
				m_pMainViewDrawing=pViewDrawing;
				m_fMainViewRectW=drawRectArr[i].width;
			}
		}
	}
	//2��������ͼ����ȡ����ͼ��λ����Ϣ,����������
	CTextOnlyDrawing *pDim=NULL;
	CHashStrList<long> idHashByStrKey;
	slaveDrawRectArr.RemoveAll();
	if(m_pMainViewDrawing)
	{
		for(pDim=m_pMainViewDrawing->GetPureTextDimList()->GetFirst();pDim;pDim=m_pMainViewDrawing->GetPureTextDimList()->GetNext())
		{
			if(pDim->iOrder!=3)	
				continue;	//��������ͼ��
			if(idHashByStrKey.GetValue(pDim->dimText))
				continue;
			idHashByStrKey.Add(pDim->dimText);	//�����ظ�������ͼ����ע
			DRAWRECT* pRect=NULL;
			for(i=0;i<drawSubRectArr.GetSize();i++)
			{
				pViewDrawing=(CSectionViewDrawing*)drawSubRectArr[i].m_pDrawing;
				pView=pViewDrawing->GetSectionView();
				if(strnicmp(pView->Symbol,pDim->dimText,50)==0)
				{
					pRect=&drawSubRectArr[i];
					break;
				}
			}
			if(pRect)
			{
				SLAVE_DRAWING_RECT slaveRect;
				slaveRect.fDistX=pDim->origin.x;
				slaveRect.pSlaveRect=pRect;
				slaveDrawRectArr.Add(slaveRect);
			}
		}
	}
	if(slaveDrawRectArr.GetSize()!=drawSubRectArr.GetSize())
	{
		logerr.Log("����ͼ���������仯��");
		return false;
	}
	CHeapSort<SLAVE_DRAWING_RECT>::HeapSort(slaveDrawRectArr.GetData(),slaveDrawRectArr.GetSize(),compareDrawingRect3);
	return true;
}

void CCommonTubeDrawingRectLayout::CalFrameSize(double h_to_w,double marginSpace,double limitWidth/*=0*/,double maxHeight/*=0*/,
											    bool bZoomRelayout/*=false*/,double *pfPageCount/*=NULL*/)
{
	if(drawMainRectArr.GetSize()==0&&slaveDrawRectArr.GetSize()==0)
		Init();
	//
	int i=0;
	m_fFrameHeight=m_fFrameWidth=0;
	for(i=0;i<drawMainRectArr.GetSize();i++)
	{
		if(m_fFrameWidth<drawMainRectArr[i].width)
			m_fFrameWidth=drawMainRectArr[i].width;
		m_fFrameHeight+=drawMainRectArr[i].height;
	}
	//����������ͼ�ܸ߶�+��ϸ��߳��������,��ʱ�Ա��ش�ʩ wjh-2018.8.2
	double FRAME_HEIGHT=limitWidth*h_to_w;
	if(m_fFrameHeight>(FRAME_HEIGHT-m_fFixRectHeight))
		m_fFrameWidth+=m_fFixRectWidth;
	double fSumWidth=0,fSumHigh=0;
	m_fSubRectMaxUpHigh=m_fSubRectMaxDownHigh=0;
	for(i=0;i<drawSubRectArr.GetSize();i++)
	{
		if(m_fSubRectMaxUpHigh<drawSubRectArr[i].topLeft.y)
			m_fSubRectMaxUpHigh=drawSubRectArr[i].topLeft.y;
		if(m_fSubRectMaxDownHigh<drawSubRectArr[i].height-drawSubRectArr[i].topLeft.y)
			m_fSubRectMaxDownHigh=drawSubRectArr[i].height-drawSubRectArr[i].topLeft.y;
		fSumWidth+=drawRectArr[i].width;
	}
	m_fSubRectMaxHigh=m_fSubRectMaxUpHigh+m_fSubRectMaxDownHigh;
	double fZoomScale=floor(fSumWidth/m_fFrameWidth)+1;
	if(fZoomScale>1&&pfPageCount)
		fSumHigh=m_fSubRectMaxHigh*fZoomScale;	//����ͼ̫����Ҫ�ֶ�����ʾ
	else
		fSumHigh=m_fSubRectMaxUpHigh;
	m_fFrameHeight+=fSumHigh;
	if(bZoomRelayout)//&&m_fFrameWidth<limitWidth)
		m_fFrameWidth=limitWidth;
	else 
		m_fFrameHeight=max(m_fFrameHeight,m_fFixRectHeight);
	//4�����ձ�����������ͼ����,����ʣ��ռ�
	m_bCompulsoryHoriLayout=false;	//���߿�ȹ���ʱǿ�Ƹ�Ϊ������������ͼ
	if(pfPageCount==NULL&&m_fFrameHeight/m_fFrameWidth>1&&!bZoomRelayout)
	{
		m_bCompulsoryHoriLayout=true;
		m_fFrameHeight-=fSumHigh;
	}
	if(m_fFrameHeight/m_fFrameWidth>h_to_w)	
	{	//���߿�ĸ߿�ȼ���,�߿���Ҫ�ӿ�
		if(pfPageCount)
			*pfPageCount=(m_fFrameHeight/m_fFrameWidth)/h_to_w;
		m_fFrameHeight+=2*marginSpace;
		m_fFrameWidth=m_fFrameHeight/h_to_w;
	}
	else
	{	//���߿�ĸ߿�ȼ���,�߿���Ҫ�Ӹ�
		m_fFrameWidth+=2*marginSpace;
		m_fFrameHeight=m_fFrameWidth*h_to_w;
		if(pfPageCount)
			*pfPageCount=1;
	}
}

double CCommonTubeDrawingRectLayout::CalPageCount(double h_to_w,double marginSpace,double limitWidth/*=0*/)
{
	Init();
	double fPageCount=1;
	CalFrameSize(h_to_w,marginSpace,limitWidth,0,false,&fPageCount);
	return fPageCount;
}

bool CCommonTubeDrawingRectLayout::Relayout(double h_to_w,double marginSpace,double limitWidth/*=0*/,double maxHeight/*=0*/)
{
	double *pfPageCount=NULL;
	//1������ͼ���࣬��������ͼ����ȡ����ͼ��λ����Ϣ����
	Init();
	//2��������ͼ������ͼ����Ŀ�Ⱥ͸߶�
	CalFrameSize(h_to_w,marginSpace,limitWidth,maxHeight,m_bZoomRelayout,NULL);
	//3��������ͼ���ض�����ͼ�������Ƚ��в���
	double fDist=0,fWidth=0;
	LAYOUT_COORD2D origin;
	origin.x=marginSpace;
	int i=0;
	for(i=0;i<drawMainRectArr.GetSize();i++)
	{
		if(i==0)
			origin.y=m_bCompulsoryHoriLayout? - marginSpace : -(m_fSubRectMaxHigh+marginSpace);
		else
			origin.y-=drawMainRectArr[i-1].height;
		drawMainRectArr[i].topLeft=origin;
		drawMainRectArr[i].m_bLayout=TRUE;
	}
	m_fBlkZoomcoef=limitWidth<EPS ? 1.0 : m_fFrameWidth/limitWidth;
	double zoomCoefOfFixRect=1;//m_bAutoZoomFixRect?1.0:m_fBlkZoomcoef;
	if(fixRectAlign==TopLeft)
		m_FixRectTopLeft.Set(0,0);
	else //fixRectAlign==TopRight)
		m_FixRectTopLeft.Set(m_fFrameWidth-m_fFixRectWidth*zoomCoefOfFixRect,0);
	bool bExclusiveEndConn=false;//�ų�
	do{
		if(m_bCompulsoryHoriLayout)
			break;
		double fUseX=marginSpace,fOffsetX=0,fOffsetY=0;
		for(i=0;i<slaveDrawRectArr.GetSize();i++)
			slaveDrawRectArr[i].pSlaveRect->m_bLayout=FALSE;
		SLAVE_DRAWING_RECT* pRightestRect=NULL;
		for(i=0;i<slaveDrawRectArr.GetSize();i++)
		{
			//slaveDrawRectArr[i].fDistXΪ�������λ��
			//fDistRealXΪ��ʵ���к���λ��
			double fDistRealX=fUseX+slaveDrawRectArr[i].pSlaveRect->width*0.5;
			if(fDistRealX>m_fMainViewRectW&&!bExclusiveEndConn)
			{
				bExclusiveEndConn=true;
				break;
			}
			else if(fDistRealX>m_fMainViewRectW)
				continue;
			CSectionViewDrawing* pDrawing=(CSectionViewDrawing*)slaveDrawRectArr[i].pSlaveRect->m_pDrawing;
			if( pDrawing&&bExclusiveEndConn&&(
				pDrawing->GetSectionView()->m_iViewType==VIEW_FL||
				pDrawing->GetSectionView()->m_iViewType==VIEW_INSERTPLATE||
				pDrawing->GetSectionView()->m_iViewType==VIEW_FOOTNAILSLOT||
				pDrawing->GetSectionView()->m_iViewType==VIEW_FOOTNAILANGLE))
				continue;
			fOffsetY=-m_fSubRectMaxUpHigh+slaveDrawRectArr[i].pSlaveRect->initTopLeft.y;//height-fSubRectMaxHigh;
			//if(fabs(fOffsetY)<EPS)
			fOffsetY-=marginSpace;
			if(fDistRealX>slaveDrawRectArr[i].fDistX)
				fOffsetX=fUseX;
			else
				fOffsetX=slaveDrawRectArr[i].fDistX-slaveDrawRectArr[i].pSlaveRect->width*0.5;
			origin.x=fOffsetX;
			origin.y=fOffsetY;
			slaveDrawRectArr[i].pSlaveRect->topLeft=origin;
			slaveDrawRectArr[i].pSlaveRect->m_bLayout=TRUE;
			//
			fUseX=origin.x+slaveDrawRectArr[i].pSlaveRect->width+marginSpace;
			pRightestRect=&slaveDrawRectArr[i];
		}
		if(bExclusiveEndConn)
		{	//����ͼ����ʱ�������Ұ������ã����ٶ�������ͼ�� wht 18-08-03
			fUseX=marginSpace;
			for(i=0;i<slaveDrawRectArr.GetSize();i++)
				slaveDrawRectArr[i].pSlaveRect->m_bLayout=FALSE;
			for(i=0;i<slaveDrawRectArr.GetSize();i++)
			{
				//slaveDrawRectArr[i].fDistXΪ�������λ��
				//fDistRealXΪ��ʵ���к���λ��
				double fDistRealX=fUseX+slaveDrawRectArr[i].pSlaveRect->width*0.5;
				if(fDistRealX>m_fMainViewRectW)
					break;
				CSectionViewDrawing* pDrawing=(CSectionViewDrawing*)slaveDrawRectArr[i].pSlaveRect->m_pDrawing;
				if( pDrawing&&(
					pDrawing->GetSectionView()->m_iViewType==VIEW_FL||
					pDrawing->GetSectionView()->m_iViewType==VIEW_INSERTPLATE||
					pDrawing->GetSectionView()->m_iViewType==VIEW_FOOTNAILSLOT||
					pDrawing->GetSectionView()->m_iViewType==VIEW_FOOTNAILANGLE))
					continue;
				fOffsetY=-m_fSubRectMaxUpHigh+slaveDrawRectArr[i].pSlaveRect->initTopLeft.y;//height-fSubRectMaxHigh;
				//if(fabs(fOffsetY)<EPS)
				fOffsetY-=marginSpace;
				origin.x=fUseX;
				origin.y=fOffsetY;
				slaveDrawRectArr[i].pSlaveRect->topLeft=origin;
				slaveDrawRectArr[i].pSlaveRect->m_bLayout=TRUE;
				//
				fUseX=origin.x+slaveDrawRectArr[i].pSlaveRect->width+marginSpace;
				pRightestRect=&slaveDrawRectArr[i];
			}
			break;
		}
		else
		{
			double moveleft=0;
			if(pRightestRect&&(moveleft=pRightestRect->pSlaveRect->topLeft.x+pRightestRect->pSlaveRect->width-this->m_FixRectTopLeft.x)>0)
			{
				moveleft+=5;	//����ϸ�����5mm��϶
				//	����һ���ܵļ�϶ֵ�����Ƿ���
				int startindex=slaveDrawRectArr.GetSize()-1;
				SLAVE_DRAWING_RECT* pCurrRect=pRightestRect;
				while(startindex>0&&moveleft>EPS)
				{
					SLAVE_DRAWING_RECT* pPrevRect=&slaveDrawRectArr[startindex-1];
					double canmoveleft=pCurrRect->pSlaveRect->topLeft.x-(pPrevRect->pSlaveRect->topLeft.x+pPrevRect->pSlaveRect->width);
					double move=min(canmoveleft,moveleft);
					for(i=startindex;move>0&&i<slaveDrawRectArr.GetSize();i++)
					{
						pCurrRect=&slaveDrawRectArr[i];
						pCurrRect->pSlaveRect->topLeft.x-=move;
					}
					moveleft-=move;
					startindex--;
					pCurrRect=pPrevRect;
				}
			}
		}
	}while(i<slaveDrawRectArr.GetSize());
	//4����ʣ����ͼ���ν��в���
	drawRectArr.Empty();
	for(i=0;i<drawMainRectArr.GetSize();i++)
		drawRectArr.append(drawMainRectArr[i]);
	for(i=0;i<slaveDrawRectArr.GetSize();i++)
		drawRectArr.append(*(slaveDrawRectArr[i].pSlaveRect));
	double stepH=drawRectArr[0].height;
	double stepW=stepH/h_to_w;
	double minStepLimitW=m_fFrameWidth/100;

	bool success=true;
	while(1)
	{
		for(i=0;i<drawRectArr.GetSize();i++)
		{
			if(drawRectArr[i].m_bLayout==TRUE)
				continue;
			bool bSearchHori=true;
			if(drawRectArr[i].width/drawRectArr[i].height>3)
				bSearchHori=false;
			BOOL layouted=TRUE;
			while(!GetBlankRectTopLeft(drawRectArr[i].width,drawRectArr[i].height,&drawRectArr[i].topLeft,marginSpace,bSearchHori))
			{
				if(IsDefiniteNotEnoughSpace(drawRectArr[i].width,drawRectArr[i].height,false,true))
				{
					layouted=FALSE;
					break;	//������ͼ�����¿϶����ܷŵ��£������˳�
				}
				m_fFrameHeight+=100;	//�ռ䲻����ʱ��ʱ���²����Ӹ߶�
				success=false;
			}
			drawRectArr[i].m_bLayout=layouted;
		}
		break;
	}
	return success;
}
bool CCommonTubeDrawingRectLayout::InternalRelayout(double &zoomdraw_coef,int iPageIndex/*=-1*/)
{	
	//1.��ʼ����ͼ���ֲ���()
	InitDrawingRectLayout(iPageIndex);
	//2.�Զ��Ż�����
	int j=0,n=0;
	zoomdraw_coef=1.0;
	double zoomdraw_v_coef=1.0;
	double h_to_w=draw_rc_height/draw_rc_width;
	bool relayout_successed=false;
	int hits=0;
	//��ҳ����ʱΪ��֤����ҳ����һ�£���zoom_coef�ӽ�1������ظ����ִ�������Ϊ2 wht 18-08-28
	int MAX_HITS=(m_idType==TYPE_PAGEING_TUBE)?2:1;
	CSectionViewDrawing* pViewDrawing=NULL;
	do{
		m_bZoomRelayout=hits>0;
		bool bSuccess=Relayout(h_to_w,CPartGroupDrawingLayoutOper::MARGIN_SPACE,draw_rc_width,draw_rc_height);
		if(sys.general_set.bProcessCardTbl)
		{
			zoomdraw_coef=draw_rc_width/GetFrameWidth();
			double height=GetRealDrawingHeight();
			if(height>0&&((iPageIndex>=0&&m_fDatumZoomCoef==0)||(iPageIndex==-1&&!bSuccess)))
			{
				zoomdraw_v_coef=draw_rc_height/height;
				zoomdraw_coef=min(zoomdraw_coef,zoomdraw_v_coef);
			}
		}
		if((zoomdraw_coef<0.95||m_fDatumZoomCoef>0)&&hits<MAX_HITS)
		{	//
			if(iPageIndex>=0)
			{
				if(m_iDatumPage==-1&&m_fDatumZoomCoef==0)
				{
					m_fDatumZoomCoef=zoomdraw_coef;
					m_iDatumPage=iPageIndex;
				}
				else if(m_iDatumPage==iPageIndex)
					m_fDatumZoomCoef*=zoomdraw_coef;
				else if(m_iDatumPage!=iPageIndex&&hits==0&&m_fDatumZoomCoef>0)	//�Ի�׼���ű���Ϊ׼��������ҳ
					zoomdraw_coef=m_fDatumZoomCoef;
			}
			for(int ii=0;ii<drawRectArr.GetSize();ii++)
			{
				DRAWRECT *pDrawingRect=&drawRectArr[ii];
				pViewDrawing=(CSectionViewDrawing*)pDrawingRect->m_pDrawing;
				pViewDrawing->ZoomDrawing(zoomdraw_coef);
				f2dRect view_draw_rect=pViewDrawing->GetDrawRegion(true);
				pDrawingRect->height=view_draw_rect.Height();
				pDrawingRect->width=view_draw_rect.Width();
				pDrawingRect->initTopLeft.Set(view_draw_rect.topLeft.x,view_draw_rect.topLeft.y);
				pDrawingRect->topLeft=pDrawingRect->initTopLeft;
			}
			hits++;
		}
		else
			break;
	}while(hits<MAX_HITS+1);
	for(int ii=0;ii<drawRectArr.GetSize();ii++)
	{
		DRAWRECT *pDrawingRect=&drawRectArr[ii];
		pViewDrawing=(CSectionViewDrawing*)pDrawingRect->m_pDrawing;
		pViewDrawing->drawingRect=*pDrawingRect;
		if(iPageIndex>=0)
			pViewDrawing->drawingRect.m_iPageIndex=iPageIndex;
	}
	return true;
}
int CCommonTubeDrawingRectLayout::Relayout(ARRAY_LIST<double> &zoomdraw_coef_arr)
{
	double zoomdraw_coef=1.0;
	InternalRelayout(zoomdraw_coef);
	zoomdraw_coef_arr.append(zoomdraw_coef);
	return 1;
}
//////////////////////////////////////////////////////////////////////////
// CSimpleTubeDrawingRectLayout
CSimpleTubeDrawingRectLayout::CSimpleTubeDrawingRectLayout()
{
	m_idType=TYPE_SIMPLE_TUBE;
}
bool CSimpleTubeDrawingRectLayout::Relayout(double h_to_w,double marginSpace,double limitWidth/*=0*/,double maxHeight/*=0*/)
{	
	if(limitWidth==0||maxHeight==0)
		return false;
	//1.�麸ͼ����
	CCommonTubeDrawingRectLayout::Relayout(h_to_w,marginSpace,limitWidth,maxHeight);
	//2.��֤��ͼ����
	double zoomdraw_coef=draw_rc_width/GetFrameWidth();
	double rela_limit_width=limitWidth/zoomdraw_coef,rela_max_height=maxHeight/zoomdraw_coef;
	//2.1����ƫ�Ʋ���
	int iMaxXSalveView=-1;
	double fMinY=0,fSlaveViewMinY=0,fSalveViewMaxY=-10000,fSalveViewMaxX=0;
	for(int i=0;i<drawRectArr.GetSize();i++)
	{
		double fCurY=drawRectArr[i].topLeft.y-drawRectArr[i].height;
		if(fMinY>fCurY)
			fMinY=fCurY;
		CSectionViewDrawing* pViewDrawing=(CSectionViewDrawing*)drawRectArr[i].m_pDrawing;
		CSectionView* pView=pViewDrawing?pViewDrawing->GetSectionView():NULL;
		if(pView&&pView->IsSlaveView())
		{
			if(fSlaveViewMinY>fCurY)
				fSlaveViewMinY=fCurY;
			if(fSalveViewMaxY<drawRectArr[i].topLeft.y)
			{
				fSalveViewMaxY=drawRectArr[i].topLeft.y;
				if(fSalveViewMaxX<(drawRectArr[i].topLeft.x+drawRectArr[i].width))
				{
					fSalveViewMaxX=(drawRectArr[i].topLeft.x+drawRectArr[i].width);
					iMaxXSalveView=i;
				}
			}
		}
	}
	//����ͼ������ͼ�м�Ԥ��һ����϶������ʱ���ܷŴ���ƣ������ص�
	double up_offset_y=(rela_max_height-fabs(fMinY))*0.4;
	double down_offset_y=(rela_max_height-fabs(fMinY))*0.6;
	if(up_offset_y<=0||down_offset_y<=0)
		return false;
	//2.2 ��ͼ�������� 
	for(i=0;i<drawRectArr.GetSize();i++)
	{
		CSectionViewDrawing* pViewDrawing=(CSectionViewDrawing*)drawRectArr[i].m_pDrawing;
		CSectionView* pView=pViewDrawing?pViewDrawing->GetSectionView():NULL;
		if(pView==NULL)
			continue;
		if(pView->IsMainView())
			drawRectArr[i].topLeft.y-=down_offset_y;
		else
			drawRectArr[i].topLeft.y-=up_offset_y;
	}
	fSlaveViewMinY-=down_offset_y;
	//2.3 ����ͼ������ͼ�ص�ʱ����ͼ����
	fMinY=0;
	for(i=0;i<drawRectArr.GetSize();i++)
	{
		CSectionViewDrawing* pViewDrawing=(CSectionViewDrawing*)drawRectArr[i].m_pDrawing;
		CSectionView* pView=pViewDrawing?pViewDrawing->GetSectionView():NULL;
		if(pView==NULL||!pView->IsMainView())
			continue;
		double fOverlapYScope=drawRectArr[i].topLeft.y-fSlaveViewMinY;	
		if(fOverlapYScope>0)
			drawRectArr[i].topLeft.y-=(fOverlapYScope+marginSpace);	//����ͼ�������Ʊ����ص�
		fMinY=drawRectArr[i].topLeft.y-drawRectArr[i].height;
	}
	double offset_y=rela_max_height-fabs(fMinY);
	//2.4 ����ͼ����ϸ���ص�ʱ������ͼ��������
	if(m_fFixRectWidth<=0||m_fFixRectHeight<=0)
		return false;
	//������Ҫ������
	double offset_left=0,offset_down=0;
	if(fSalveViewMaxX>(rela_limit_width-m_fFixRectWidth)&&fabs(fSalveViewMaxY)<m_fFixRectHeight)
	{
		offset_down=m_fFixRectHeight-fabs(fSalveViewMaxY)+marginSpace;
		offset_left=fSalveViewMaxX-(rela_limit_width-m_fFixRectWidth)+marginSpace;
		if(offset_down<offset_y)
		{	//������ͼ����
			for(i=0;i<drawRectArr.GetSize();i++)
				drawRectArr[i].topLeft.y-=offset_down;
		}
		else if(iMaxXSalveView>=0)
		{	//���Ҳ�����ͼ����
			drawRectArr[iMaxXSalveView].topLeft.x-=offset_left;
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
//
CPagingTubeDrawingRectLayout::CPagingTubeDrawingRectLayout()
{
	m_idType=TYPE_PAGEING_TUBE;	
}
int CPagingTubeDrawingRectLayout::Prepaging()
{
	CLDSPartGroup *pPartGroup=GetPartGroup();
	CLDSPart *pRootPart=GetRootPart();
	if(pPartGroup==NULL||pRootPart==NULL)
		return 0;
	if(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_COMMON&&pRootPart!=NULL&&pRootPart->IsTube())
	{
		CSectionView *pView=NULL;
		int nMainTubeView=0,nSlaveViews=0;
		int nSectViews=0,nFlViews=0;
		for(CSectionView *pView=pPartGroup->SonViews.GetFirst();pView;pView=pPartGroup->SonViews.GetNext())
		{
			if(pView->m_iViewType==VIEW_MAIN||pView->m_iViewType==VIEW_FRONT)
				nMainTubeView++;
			else if(pView->IsSlaveView())
			{
				nSlaveViews++;
				if(pView->IsTubeSectionView())
					nSectViews++;
			}
		}
		if(nMainTubeView>2)
		{	//��Ҫ��ҳ��ʾ,��ʱ��֧�ַ���ҳ��ʾ
			for(CSectionViewDrawing* pViewDrawing=(CSectionViewDrawing*)m_pPartGroupDrawing->EnumFirstDrawingComponent();
				pViewDrawing;pViewDrawing=(CSectionViewDrawing*)m_pPartGroupDrawing->EnumNextDrawingComponent())
			{
				CSectionView *pView=pViewDrawing->GetSectionView();
				if(pViewDrawing->GetComponentType()==CSectionViewDrawing::CLS_BOM_VIEW)
				{	//����ͼ����5��ʱ����ڵ�2ҳ��ʾ
					pViewDrawing->drawingRect.m_iPageIndex=(nSlaveViews>5)?1:0;
				}
				else
				{
					if(pView->m_iViewType==VIEW_MAIN||pView->IsSlaveView())
						pViewDrawing->drawingRect.m_iPageIndex=0;
					else
						pViewDrawing->drawingRect.m_iPageIndex=1;
				}
			}
			return 2;
		}
		else
			return 1;
	}
	else
		return 1;
}
int CPagingTubeDrawingRectLayout::Relayout(ARRAY_LIST<double> &zoomdraw_coef_arr)
{	//�Ժ�Ӧͨ��������ȷ����˭Ϊ��׼�������ű������˴���ʱ�ڴ����й̶�����ǰֻ��һ�ַ�ҳģʽ wht 18-08-03
	int nPageCount=Prepaging();
	zoomdraw_coef_arr.SetSize(nPageCount);
	double datum_zoom_coef=0;
	for(int i=nPageCount-1;i>=0;i--)
	{	
		double zoomdraw_coef=1.0;
		InternalRelayout(zoomdraw_coef,nPageCount>1?i:-1);
		if(datum_zoom_coef==0)
			datum_zoom_coef=zoomdraw_coef;
		else
			zoomdraw_coef=datum_zoom_coef;
		zoomdraw_coef_arr[i]=zoomdraw_coef;
	}
	return nPageCount;
}

//////////////////////////////////////////////////////////////////////////
// CPartGroupDrawingLayoutOper
CPartGroupDrawingLayoutOper::CPartGroupDrawingLayoutOper(CPartGroupDrawing *pPartGroupDrawing)
{
	m_pPartGroupDrawing=pPartGroupDrawing;
	int iType=AnalysisPartGroupDrawingLayoutType();
	m_pRectLayout=CreateRectLayoutInstance(iType);
}

CPartGroupDrawingLayoutOper::~CPartGroupDrawingLayoutOper()
{
	DestroyRectLayoutInstance();
	m_pPartGroupDrawing=NULL;
}

CPartGroupRectLayout* CPartGroupDrawingLayoutOper::CreateRectLayoutInstance(int idLayoutType)
{
	CPartGroupRectLayout *pRectLayout=NULL;
	switch(idLayoutType)
	{
	case CPartGroupRectLayout::TYPE_3VIEW:
		pRectLayout=new C3ViewsDrawingRectLayout();
		break;
	case CPartGroupRectLayout::TYPE_COMMON_PLATE:
		pRectLayout=new CCommonPlateDrawingRectLayout();
		break;
	case CPartGroupRectLayout::TYPE_COMMON_TUBE:
		pRectLayout=new CCommonTubeDrawingRectLayout();
		break;
	case CPartGroupRectLayout::TYPE_SIMPLE_TUBE:
		pRectLayout=new CSimpleTubeDrawingRectLayout();
		break;
	case CPartGroupRectLayout::TYPE_PAGEING_TUBE:
		pRectLayout=new CPagingTubeDrawingRectLayout();
		break;
	case CPartGroupRectLayout::TYPE_DEFAULT:
		pRectLayout=new CPartGroupRectLayout();
		break;
	default:
		break;
	}
	if(pRectLayout)
	{
		m_pRectLayout=pRectLayout;
		pRectLayout->SetPartGroupDrawing(m_pPartGroupDrawing);
		return pRectLayout;
	}
	else
		return NULL;
}

bool CPartGroupDrawingLayoutOper::DestroyRectLayoutInstance()
{
	if(m_pRectLayout)
	{
		switch(m_pRectLayout->idType)
		{
		case CPartGroupRectLayout::TYPE_3VIEW:
			delete (C3ViewsDrawingRectLayout*)m_pRectLayout;
			break;
		case CPartGroupRectLayout::TYPE_COMMON_PLATE:
			delete (CCommonPlateDrawingRectLayout*)m_pRectLayout;
			break;
		case CPartGroupRectLayout::TYPE_COMMON_TUBE:
			delete (CCommonTubeDrawingRectLayout*)m_pRectLayout;
			break;
		case CPartGroupRectLayout::TYPE_SIMPLE_TUBE:
			delete (CSimpleTubeDrawingRectLayout*)m_pRectLayout;
			break;
		case CPartGroupRectLayout::TYPE_PAGEING_TUBE:
			delete (CPagingTubeDrawingRectLayout*)m_pRectLayout;
			break;
		case CPartGroupRectLayout::TYPE_DEFAULT:
			delete m_pRectLayout;
			break;
		default:
			return FALSE;
		}
		m_pRectLayout=NULL;
	}
	return TRUE;
}

int CPartGroupDrawingLayoutOper::AnalysisCommonTubeDrawingLayoutType()
{
	if(m_pPartGroupDrawing==NULL||m_pPartGroupDrawing->m_pPartGroup==NULL||m_pPartGroupDrawing->m_pRootPart==NULL)
		return 0;
	if(m_pPartGroupDrawing->m_pPartGroup->m_iPartGroupType!=CLDSPartGroup::GROUP_COMMON||!m_pPartGroupDrawing->m_pRootPart->IsTube())
		return 0;
	//1.ͳ������ͼ������ͼ����
	CSectionView *pView=NULL;
	int nMainTubeView=0,nSlaveViews=0;
	int nSectViews=0,nFlViews=0;
	double draw_rc_height=m_pPartGroupDrawing->m_fMaxDrawingHeight;
	double draw_rc_width=m_pPartGroupDrawing->m_fMaxDrawingLength;
	for(CSectionView *pView=m_pPartGroupDrawing->m_pPartGroup->SonViews.GetFirst();pView;pView=m_pPartGroupDrawing->m_pPartGroup->SonViews.GetNext())
	{
		if(pView->m_iViewType==VIEW_MAIN||pView->m_iViewType==VIEW_FRONT)
			nMainTubeView++;
		else if(pView->IsSlaveView())
		{
			nSlaveViews++;
			if(pView->IsTubeSectionView())
				nSectViews++;
		}
	}
	//2.������ͼ���ٴ�ϸ����ͼ����
	CCommonTubeDrawingRectLayout rectLayout(m_pPartGroupDrawing);
	if(nMainTubeView==1&&nSlaveViews<=4)
	{	//����ͼ���������ʾ
		if(!rectLayout.IsSlaveViewException())
			return CPartGroupRectLayout::TYPE_SIMPLE_TUBE;
		else
			return CPartGroupRectLayout::TYPE_COMMON_TUBE;
	}
	else if(nMainTubeView>2)
	{	//����ͼ����2��ʱ,������Ҫ��ҳ��ʾ
		double h_to_w=draw_rc_height/draw_rc_width;
		if(rectLayout.CalPageCount(h_to_w,MARGIN_SPACE,draw_rc_width)>1.5)
			return CPartGroupRectLayout::TYPE_PAGEING_TUBE;
		else
			return CPartGroupRectLayout::TYPE_COMMON_TUBE;
	}
	else
		return CPartGroupRectLayout::TYPE_COMMON_TUBE;
}

int CPartGroupDrawingLayoutOper::AnalysisPartGroupDrawingLayoutType()
{
	CLDSPartGroup *pPartGroup=m_pPartGroupDrawing?m_pPartGroupDrawing->m_pPartGroup:NULL;
	CLDSPart *pRootPart=m_pPartGroupDrawing?m_pPartGroupDrawing->m_pRootPart:NULL;
	if(m_pPartGroupDrawing==NULL||pPartGroup==NULL||pRootPart==NULL)
		return 0;
	if( pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_STDX_PLATES||
		pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_ANGLEFOOT)
		return CPartGroupRectLayout::TYPE_3VIEW;
	else if(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_COMMON&&pRootPart->IsTube())
		return AnalysisCommonTubeDrawingLayoutType();
	else if(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_COMMON&&pRootPart->IsPlate())
		return CPartGroupRectLayout::TYPE_COMMON_PLATE;
	else
		return CPartGroupRectLayout::TYPE_DEFAULT;
}
int CPartGroupDrawingLayoutOper::Relayout(ARRAY_LIST<double> &zoomdraw_coef_arr)
{	
	return m_pRectLayout->Relayout(zoomdraw_coef_arr);
}