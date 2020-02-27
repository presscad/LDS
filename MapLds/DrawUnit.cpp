//<LOCALE_TRANSLATE/>
// DrawUnit.cpp: implementation of the CDrawUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdarx.h"
#include "ptr_list.h"
#include "RxTools.h"
#include "DrawUnit.h"
#include "SysPara.h"
#include "dbxrecrd.h"
#include "Tower.h"
#include "AdsTools.h"
#include "DragEntSet.h"
#include "PlankDrawing.h"
#include "Query.h"
#include "DimJgZhun.h"
#include "GlobalFunc.h"
#include "DrawTaMap.h"
#include "SortFunc.h"
#include "PartGroupDrawing.h"
#include "DrawDim.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


long PAGE_INDEX::get_liPage()
{
	return _liPage;
}
void PAGE_INDEX::IncrementSerial(){
	ciPagesOfSerial=1;
	ciIndexInSerial=0;
	wiSerial++;
	_liPage++;
}
void PAGE_INDEX::IncrementLiPage()
{
	_liPage++;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int compareDrawingRect(const CDrawingRect &drawRect1,const CDrawingRect &drawRect2)
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
BOOL IsOrthoRectInters(f3dPoint topLeft1,double width1,double height1,f3dPoint topLeft2,double width2,double height2)
{
	if((width1==0&&height1==0)||(width2==0&&height2==0))
		return false;
	if(topLeft2.x>topLeft1.x+width1)
		return FALSE;	//�ڶ��������ڵ�һ�����ε��Ҳ�
	else if(topLeft2.y<topLeft1.y-height1)
		return FALSE;	//�ڶ��������ڵ�һ�����ε��²�
	else if(topLeft2.x+width2<topLeft1.x)
		return FALSE;	//�ڶ��������ڵ�һ�����ε����
	else if(topLeft2.y-height2>topLeft1.y)
		return FALSE;	//�ڶ��������ڵ�һ�����ε��ϲ�
	else
		return TRUE;
}
bool CDrawingRectLayout::IsDefiniteNotEnoughSpace(double width,double height,bool canExtendByX/*=false*/,bool canExtendByY/*=false*/)
{
	if(!canExtendByX&&width>m_fFrameWidth)
		return true;
	if(!canExtendByY&&height>m_fFrameHeight)
		return true;
	return false;
}
BOOL CDrawingRectLayout::GetBlankRectTopLeft(double width,double height,f3dPoint *topLeft,double marginSpace,bool bSearchHori/*=true*/)
{
	int i,j,k,nLayoutNum=0;
	int maxY=0;
	//��֤stepX��stepY��Ϊ0 wht 11-06-16
	double stepX=max(0.5,m_fFrameWidth/100),stepY=max(0.5,m_fFrameHeight/100);
	double zoomCoefOfFixRect=1;//m_bAutoZoomFixRect?1.0:m_fBlkZoomcoef;
	f3dPoint origin;
	double fFixRectWidth=m_bPlaceFixRect?0:m_fFixRectWidth;
	double fFixRectHeight=m_bPlaceFixRect?0:m_fFixRectHeight;
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
			BOOL bHasInters=IsOrthoRectInters(m_FixRectTopLeft,fFixRectWidth*zoomCoefOfFixRect,fFixRectHeight*zoomCoefOfFixRect,origin,width,height);
			if(!bHasInters)
			{
				for(k=0;k<drawRectArr.GetSize();k++)
				{
					if(!drawRectArr[k].m_bLayout)
						continue;	//δ����
					nLayoutNum++;	//�Ѳ��ù��ľ��θ���
					if(IsOrthoRectInters(drawRectArr[k].topLeft,drawRectArr[k].width,drawRectArr[k].height,origin,width,height))
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

BOOL CDrawingRectLayout::GetBlankRectTopLeftTemp(CArray<double,double&>&startXArray,double width,double height,f3dPoint *topLeft)
{
	int i,k,nLayoutNum=0;
	int maxY=0;
	f3dPoint origin;
	for(i=0;;i++)		//����������
	{
		if(i>0)
		{
			if(startXArray.GetSize()>=i)
				origin.x = startXArray[i-1]+0.1;
			else
				break;
		}
		else
			origin.x = 0;
		if(origin.x+width>m_fFrameWidth)
			break;
		origin.y=0;
		while(origin.y-height>-m_fFrameHeight)
		{
			BOOL bHasInters=FALSE;
			int num = nLayoutNum;
			nLayoutNum = 0;
			for(k=0;k<drawRectArr.GetSize();k++)
			{
				if(!drawRectArr[k].m_bLayout)
					continue;	//δ����
				nLayoutNum++;	//�Ѳ��ù��ľ��θ���
				if (drawRectArr[k].topLeft.x+drawRectArr[k].width<origin.x)
					continue;
				if (origin.y-height>drawRectArr[k].topLeft.y)
					continue;
				if(IsOrthoRectInters(drawRectArr[k].topLeft,drawRectArr[k].width,drawRectArr[k].height,origin,width,height))
				{
					bHasInters=TRUE;
					origin.y-=drawRectArr[k].height+0.1;
					break;
				}
			}
			if(!bHasInters)
			{
				if(topLeft)
					*topLeft=origin;
				double widthTemp = topLeft->x+width;
				if (startXArray.GetSize()==0)
					startXArray.Add(widthTemp);
				else if(widthTemp>startXArray[startXArray.GetSize()-1])
					startXArray.Add(widthTemp);
				else if (startXArray.GetSize()==1)
					startXArray.InsertAt(0,widthTemp);
				else if(fabs(widthTemp-startXArray[startXArray.GetSize()-1])>EPS)
				{
					int j=0;
					for (j=startXArray.GetSize()-1;j>0;j--)
					{
						if (widthTemp<startXArray[j]&&widthTemp>startXArray[j-1]&&j>=1)
						{
							startXArray.InsertAt(j,widthTemp);
							break;
						}
					}
					if (j==0)
						startXArray.InsertAt(0,widthTemp);
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}

void DisplayProcess(int percent,const char *sTitle);
#include "LogFile.h"
BOOL CDrawingRectLayout::RelayoutTemp(double hight/*=1000*/,double width/*=10000000000000000*/)
{
	int nNum = drawRectArr.GetSize();
	if(nNum<=0)
		return FALSE;
	//����ͼ���ƾ��ΰ��ճ�/���ɴ�С���������Ա���һ���Զ��Ż�����
	CHeapSort<CDrawingRect>::HeapSort(drawRectArr.GetData(),drawRectArr.GetSize(),compareDrawingRect);
	m_fFrameWidth=width;		//���Ʊ߿��ʼ��
	m_fFrameHeight=hight;		//���Ʊ߿��ʼ��
	//�Բ�ͼ���ƾ��εĲ��÷��������Ż�(��ʼ״̬�ǰ�һ�ж��в���)
	m_arrRowHeight.RemoveAll();
	int i = 0;
	for(i=0;i<drawRectArr.GetSize();i++)
	{
		if(drawRectArr[i].m_pDrawing)
			drawRectArr[i].m_bLayout=FALSE;
	}
	CArray<double,double&> startXArray;
	for(i=0;i<drawRectArr.GetSize();i++)
	{
		if(drawRectArr[i].height>hight)
		{
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef __COMMON_PART_INC_
			if(drawRectArr[i].m_pDrawing!=NULL)
				logerr.Log("the minimum width of %s model plate envelope rectangular is larger than the specified width,fail to arrange automatically!",(char*)((CPlankDrawing*)drawRectArr[i].m_pDrawing)->GetPlate()->GetPartNo());
			else
#endif
				MessageBox(NULL,"Part of the plate lack of drawing data!","ERROR",MB_OK);
#else
#ifdef __COMMON_PART_INC_
			if(drawRectArr[i].m_pDrawing!=NULL)
				logerr.Log("%s�Ÿְ���С������ο�ȴ���ָ����ͼ��ȣ�δ���Զ��Ű�!",(char*)((CPlankDrawing*)drawRectArr[i].m_pDrawing)->GetPlate()->GetPartNo());
			else
#endif
				MessageBox(NULL,"���ְָ�ȱ��ͼ�λ�������!","ERROR",MB_OK);
#endif
			continue;
		}
		if(drawRectArr[i].m_bLayout)
			continue;
		int j=0;
		for(j=0;j<drawRectArr.GetSize();j++)
		{
			if(drawRectArr[j].m_bLayout)
				continue;
			if(GetBlankRectTopLeftTemp(startXArray,drawRectArr[j].width,drawRectArr[j].height,&drawRectArr[j].topLeft))
			{
				drawRectArr[j].m_bLayout=TRUE;
				break;
			}
		}
		if(j==drawRectArr.GetSize())
			break;	//�ռ��Ѿ�������,�ع�����һ���ַ���
	}
	//
	BOOL bLayout=FALSE;
	for(i=0;i<drawRectArr.GetSize();i++)
	{
		if(drawRectArr[i].m_bLayout)
			bLayout=TRUE;
	}
	return bLayout;	
}
//����ϵͳ�������������������Ų�
BOOL CDrawingRectLayout::RelayoutPlateDraw(double minDistance,DYN_ARRAY<SEGREC>& segarr)
{
	int nNum = drawRectArr.GetSize();
	if(nNum<=0)
		return FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"the process of arrange material automatically");
#else
	DisplayProcess(0,"�ְ��Ų�����");
#endif
	int iOrder=0,iPreRow=0,iPreCol=0;
	double fMaxHigh=0,fMaxWidth=0;
	for(UINT iSeg=0;iSeg<segarr.Size();iSeg++)
	{
		SEGREC xCurrSegInfo=segarr[iSeg];
		if(iOrder>0)
		{	//��η��������һ��DWGͼ��ʱ��Ҫ���в���հ�ռλ���տ�λ
			if(iOrder%sys.general_set.nPartRowsN>0)	//����ռλ
				iOrder+=(sys.general_set.nPartRowsN-iOrder%sys.general_set.nPartRowsN);
		}
		if(sys.general_set.bCardLayoutBaseRow)
		{	//����ͼ����Ϊ��׼��������
			ATOM_LIST<CDrawingRect*> drawGroupRectList;
			double fWidth=0,fSingMaxHigh=0;
			for(int j=0;j<drawRectArr.GetSize();j++)
			{
				CPlankDrawing *pPlateDraw=(CPlankDrawing *)drawRectArr[j].m_pDrawing;	
				if(xCurrSegInfo.iSeg!=-1&&pPlateDraw->GetPlate()->iSeg!=xCurrSegInfo.iSeg)
					continue;
#ifdef AFX_TARG_ENU_ENGLISH
				DisplayProcess(ftoi(100*iOrder/nNum),"the process of arrange material automatically");
#else
				DisplayProcess(ftoi(100*iOrder/nNum),"�ְ��Ų�����");
#endif
				int iCurRow=iOrder/sys.general_set.nPartRowsN;
				if(iCurRow!=iPreRow)
				{
					fMaxHigh+=(fSingMaxHigh+minDistance);
					for(int n=0;n<drawGroupRectList.GetNodeNum();n++)
					{
						CDrawingRect* pDrawRect=drawGroupRectList[n];
						pDrawRect->topLeft.Set(fWidth,-fMaxHigh+pDrawRect->height,0);
						fWidth+=pDrawRect->width+minDistance;
					}
					//
					drawGroupRectList.Empty();
					iPreRow=iCurRow;
					fSingMaxHigh=0;
					fWidth=0;
				}
				drawGroupRectList.append(&drawRectArr[j]);
				if(fSingMaxHigh<drawRectArr[j].height)
					fSingMaxHigh=drawRectArr[j].height;
				iOrder++;
			}
			//�������һ��
			fMaxHigh+=(fSingMaxHigh+minDistance);
			for(int n=0;n<drawGroupRectList.GetNodeNum();n++)
			{
				CDrawingRect* pDrawRect=drawGroupRectList[n];
				pDrawRect->topLeft.Set(fWidth,-fMaxHigh+pDrawRect->height,0);
				fWidth+=pDrawRect->width+minDistance;
			}
		}
		else
		{	//����ͼ����Ϊ��׼��������
			double fHigh=0,fSingMaxWidth=0;
			for(int j=0;j<drawRectArr.GetSize();j++)
			{
				CPlankDrawing *pPlateDraw=(CPlankDrawing *)drawRectArr[j].m_pDrawing;	
				if(xCurrSegInfo.iSeg!=-1)
				{
					if(j==drawRectArr.GetSize()-1)
						fMaxWidth+=fSingMaxWidth+minDistance;
					if(pPlateDraw->GetPlate()->iSeg!=xCurrSegInfo.iSeg)
						continue;
				}
#ifdef AFX_TARG_ENU_ENGLISH
				DisplayProcess(ftoi(100*iOrder/nNum),"the process of arrange material automatically");
#else
				DisplayProcess(ftoi(100*iOrder/nNum),"�ְ��Ų�����");
#endif
				int iCurCol=iOrder/sys.general_set.nPartRowsN;
				if(iCurCol!=iPreCol)
				{
					iPreCol=iCurCol;
					fMaxWidth+=fSingMaxWidth+minDistance;
					fSingMaxWidth=0;
					fHigh=0;
				}
				else if(iOrder>0)
					fHigh+=minDistance;
				drawRectArr[j].topLeft.Set(fMaxWidth,-fHigh,0);
				//
				fHigh+=drawRectArr[j].height;
				if(fSingMaxWidth<drawRectArr[j].width)
					fSingMaxWidth=drawRectArr[j].width;
				iOrder++;
			}
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"the process of arrange material automatically");
#else
	DisplayProcess(100,"�ְ��Ų����");
#endif
	return TRUE;
}

BOOL CDrawingRectLayout::RelayoutDraw(double minDistance)
{
	int nNum = drawRectArr.GetSize();
	if(nNum<=0)
		return FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"the process of arrange material automatically");
#else
	DisplayProcess(0,"ͼ�β��ֽ���");
#endif
	int iOrder=0,iPreRow=0,iPreCol=0;
	double fMaxHigh=0,fMaxWidth=0;
	if(sys.general_set.bCardLayoutBaseRow)
	{	//����ͼ����Ϊ��׼��������
		ATOM_LIST<CDrawingRect*> drawGroupRectList;
		double fWidth=0,fSingMaxHigh=0;
		for(int j=0;j<drawRectArr.GetSize();j++)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess(ftoi(100*iOrder/nNum),"the process of arrange material automatically");
#else
			DisplayProcess(ftoi(100*iOrder/nNum),"ͼ�β��ֽ���");
#endif
			int iCurRow=iOrder/sys.general_set.nPartRowsN;
			if(iCurRow!=iPreRow)
			{
				fMaxHigh+=(fSingMaxHigh+minDistance);
				for(int n=0;n<drawGroupRectList.GetNodeNum();n++)
				{
					CDrawingRect* pDrawRect=drawGroupRectList[n];
					pDrawRect->topLeft.Set(fWidth,-fMaxHigh+pDrawRect->height,0);
					fWidth+=pDrawRect->width+minDistance;
				}
				//
				drawGroupRectList.Empty();
				iPreRow=iCurRow;
				fSingMaxHigh=0;
				fWidth=0;
			}
			drawGroupRectList.append(&drawRectArr[j]);
			if(fSingMaxHigh<drawRectArr[j].height)
				fSingMaxHigh=drawRectArr[j].height;
			iOrder++;
		}
		//�������һ��
		fMaxHigh+=(fSingMaxHigh+minDistance);
		for(int n=0;n<drawGroupRectList.GetNodeNum();n++)
		{
			CDrawingRect* pDrawRect=drawGroupRectList[n];
			pDrawRect->topLeft.Set(fWidth,-fMaxHigh+pDrawRect->height,0);
			fWidth+=pDrawRect->width+minDistance;
		}
	}
	else
	{	//����ͼ����Ϊ��׼��������
		double fHigh=0,fSingMaxWidth=0;
		for(int j=0;j<drawRectArr.GetSize();j++)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess(ftoi(100*iOrder/nNum),"the process of arrange material automatically");
#else
			DisplayProcess(ftoi(100*iOrder/nNum),"ͼ�β��ֽ���");
#endif
			int iCurCol=iOrder/sys.general_set.nPartRowsN;
			if(iCurCol!=iPreCol)
			{
				iPreCol=iCurCol;
				fMaxWidth+=fSingMaxWidth+minDistance;
				fSingMaxWidth=0;
				fHigh=0;
			}
			else if(iOrder>0)
				fHigh+=minDistance;
			drawRectArr[j].topLeft.Set(fMaxWidth,-fHigh,0);
			//
			fHigh+=drawRectArr[j].height;
			if(fSingMaxWidth<drawRectArr[j].width)
				fSingMaxWidth=drawRectArr[j].width;
			iOrder++;
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"the process of arrange material automatically");
#else
	DisplayProcess(100,"ͼ�β��ֽ���");
#endif
	return TRUE;
}

void CDrawingRectLayout::Relayout(double h_to_w,double marginSpace)
{
	if(drawRectArr.GetSize()==0)
		return;
	int i;
	//����ͼ���ƾ��ΰ��ճ�/���ɴ�С���������Ա���һ���Զ��Ż�����
	CHeapSort<CDrawingRect>::HeapSort(drawRectArr.GetData(),drawRectArr.GetSize(),compareDrawingRect);
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
	if(stepH<=EPS||stepW<=EPS)
		return;
	double minStepLimitW=m_fFrameWidth/100;
	
	while(1)
	{
		m_arrRowHeight.RemoveAll();
		for(i=0;i<drawRectArr.GetSize();i++)
		{
			if(drawRectArr[i].m_pDrawing)
				drawRectArr[i].m_bLayout=FALSE;
		}
		if(fixRectAlign==TopLeft)
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
}
#ifdef __COMMON_PART_INC_
//<DEVELOP_PROCESS_MARK nameId="CDrawingRectLayout::RelayoutSphereGroup">
int compareDrawingRect2(const CDrawingRect &drawRect1,const CDrawingRect &drawRect2)
{
	CSectionViewDrawing* pViewDrawing1=(CSectionViewDrawing*)drawRect1.m_pDrawing;
	CSectionViewDrawing* pViewDrawing2=(CSectionViewDrawing*)drawRect2.m_pDrawing;
	return strcmp(pViewDrawing1->pSectionView->Symbol,pViewDrawing2->pSectionView->Symbol);
}
void InitDrawingRectTopfLeft(CArray<CDrawingRect,CDrawingRect&>& drawRectArr,void* pDrawing,f3dPoint topLeft)
{
	for(int i=0;i<drawRectArr.GetSize();i++)
	{
		if(drawRectArr[i].m_pDrawing==pDrawing)
			drawRectArr[i].topLeft=topLeft;
	}
}
void CDrawingRectLayout::RelayoutSphereGroup(double h_to_w,double marginSpace)
{
	//1��������ͼ��Ŷ���ڵ��麸����ͼ���������
	CHeapSort<CDrawingRect>::HeapSort(drawRectArr.GetData(),drawRectArr.GetSize(),compareDrawingRect2);
	//2������ڵ���ͼ����з��飺��Ҫ��ͼ�����Ҫ��ͼ��
	CArray<CDrawingRect,CDrawingRect&> drawMainRectArr;
	CArray<CDrawingRect,CDrawingRect&> drawSubRectArr;
	for(int i=0;i<drawRectArr.GetSize();i++)
	{
		drawRectArr[i].m_bLayout=FALSE;
		CSectionViewDrawing* pViewDrawing=(CSectionViewDrawing*)drawRectArr[i].m_pDrawing;
		if(pViewDrawing==NULL)
			continue;
		if(pViewDrawing->GetSectionView()->m_iViewType!=VIEW_FL)
			drawMainRectArr.Add(drawRectArr[i]);
		else
		{
			CSlaveSectionView* pSlaveView=pViewDrawing->GetSectionView()->pSlaveView;
			if(pSlaveView&&pSlaveView->symbol[1]>'1')
				drawSubRectArr.Add(drawRectArr[i]);
			else
				drawMainRectArr.Add(drawRectArr[i]);
		}
	}
	//3��������Ҫ��ͼ�����ͼ����Ŀ�Ⱥ͸߶�
	int nCount=0;
	double fMaxFrameHigh=0,fMaxFrameWidth=0;
	double fMainRectMaxHigh=0,fSonRectMaxHigh=0;
	m_fFrameWidth=m_fFixRectWidth+0.5*marginSpace;
	m_fFrameHeight=m_fFixRectHeight;
	for(int i=0;i<drawMainRectArr.GetSize();i++)
	{
		CSectionViewDrawing* pViewDrawing=(CSectionViewDrawing*)drawMainRectArr[i].m_pDrawing;
		if(pViewDrawing->GetSectionView()->m_iViewType==VIEW_MAIN || pViewDrawing->GetSectionView()->m_iViewType==VIEW_FRONT)
		{
			//���
			m_fFrameWidth+=drawMainRectArr[i].width+marginSpace;
			fMainRectMaxHigh=max(fMainRectMaxHigh,drawMainRectArr[i].height);
			//�߶�
			m_fFrameHeight=max(m_fFrameHeight,fMaxFrameHigh);
			fMaxFrameHigh=drawMainRectArr[i].height+marginSpace;
			nCount++;
		}
		else
		{
			fMaxFrameHigh+=drawMainRectArr[i].height;
			fSonRectMaxHigh=max(fSonRectMaxHigh,drawMainRectArr[i].height);
		}
		if(i==drawMainRectArr.GetSize()-1)
			m_fFrameHeight=max(m_fFrameHeight,fMaxFrameHigh);
	}
	//4�����ձ�����������ͼ����,����ʣ��ռ�
	fMaxFrameHigh=m_fFrameHeight;
	fMaxFrameWidth=m_fFrameWidth;
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
	//5��������ͼ��ͼ���ƾ��εĲ��÷��������Ż�
	BOOL bMainView=FALSE;
	double fDist=0,fWidth=0;
	f3dPoint origin,mainPt,sonPt;
	mainPt.x=0.5*marginSpace;
	for(int i=0;i<drawMainRectArr.GetSize();i++)
	{
		CSectionViewDrawing* pViewDrawing=(CSectionViewDrawing*)drawMainRectArr[i].m_pDrawing;
		if(pViewDrawing->GetSectionView()->m_iViewType==VIEW_MAIN || pViewDrawing->GetSectionView()->m_iViewType==VIEW_FRONT)
		{	
			fWidth=drawMainRectArr[i].width;
			mainPt.x+=fDist;
			mainPt.y=drawMainRectArr[i].height-fMainRectMaxHigh;
			origin=mainPt;
			bMainView=TRUE;
		}
		else
		{
			sonPt.x=mainPt.x+(fWidth-drawMainRectArr[i].width)*0.5;
			sonPt.y=-(fMainRectMaxHigh+0.5*marginSpace+(fSonRectMaxHigh-drawMainRectArr[i].height));
			origin=sonPt;
			bMainView=FALSE;
		}
		drawMainRectArr[i].topLeft=origin;
		drawMainRectArr[i].m_bLayout=TRUE;
		if(bMainView)
			fDist=drawMainRectArr[i].width+marginSpace;
	}
	//7���Դ���ͼ��ͼ���ƾ��εĲ��÷��������Ż�
	drawRectArr.RemoveAll();
	for(int i=0;i<drawMainRectArr.GetSize();i++)
		drawRectArr.Add(drawMainRectArr[i]);
	for(int i=0;i<drawSubRectArr.GetSize();i++)
		drawRectArr.Add(drawSubRectArr[i]);

	double stepH=drawRectArr[drawMainRectArr.GetSize()-1].height;
	double stepW=stepH/h_to_w;
	if(stepH<=EPS||stepW<=EPS)
		return;
	double minStepLimitW=m_fFrameWidth/100;
	while(1)
	{
		for(i=0;i<drawRectArr.GetSize();i++)
		{
			if(drawRectArr[i].m_bLayout==TRUE)
				continue;
			bool bSearchHori=true;
			if(drawRectArr[i].width/drawRectArr[i].height>3)
				bSearchHori=false;
			if(!GetBlankRectTopLeft(drawRectArr[i].width,drawRectArr[i].height,&drawRectArr[i].topLeft,marginSpace,bSearchHori))
				break;	//�ռ��Ѿ�������,�ع�����һ���ַ���
			drawRectArr[i].m_bLayout=TRUE;
		}
		if(i==drawRectArr.GetSize())
			break;
		else//���Ҳ������µĸ��Ż����ַ���,Ӧ�ع���ǰһ��Ч����
		{
			for(i=0;i<drawRectArr.GetSize();i++)
				drawRectArr[i].topLeft=drawRectArr[i].m_PrevTopLeft;
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
	ResetDrawingRectPosition();
}
//</DEVELOP_PROCESS_MARK>
#endif
//////////////////////////////////////////////////////////////////////////
/*
��ͨ�ֹ��躸ͼ���ַ���
	1�����ȣ�ȷ����������ͼ��������ͼλ��
	2��Ȼ��������ͼ��������ȷ���Ըֹܷ���չ����������ͼ(����ͼ��)�Ķ�λ����֤������ͼ��ֱ�������
	3�����������ͼ����ȷ������ͼ��λ�ã�
		��������ͼ��������ͼ��λ�ã�ȷ������ͼ��ˮƽλ�ã����ʹ����ͼ���ھ��ο�����������ͼ��λ�ñ�����ֱ�������
		���������������ͼ��������������ʵ���ˮƽ�������е���;
		����������õ�����ͼ��ˮƽ���볬��������ͼ���ο�Ŀ��,��ѡ��հ״����в��á�
*/
//////////////////////////////////////////////////////////////////////////
struct  CSlaveDrawingRect
{
	double fDistX;
	CDrawingRect* pSlaveRect;
	CSlaveDrawingRect(){fDistX=0.0;pSlaveRect=NULL;}
};
int compareDrawingRect3(const CSlaveDrawingRect &drawRect1,const CSlaveDrawingRect &drawRect2)
{
	if(drawRect1.fDistX>drawRect2.fDistX)
		return 1;
	else if(drawRect1.fDistX<drawRect2.fDistX)
		return -1;
	else
		return 0;
}
bool CDrawingRectLayout::RelayoutTubeCommonPartGroup(double h_to_w,double marginSpace,double limitWidth/*=0*/,double *pfPageCount/*=NULL*/,bool bZoomRelayout/*=false*/)
{
	//1������ͼ���з��ࣺ����ͼ������ͼ
	CArray<CDrawingRect,CDrawingRect&> drawMainRectArr;
	CArray<CDrawingRect,CDrawingRect&> drawSubRectArr;
	CSectionViewDrawing* pViewDrawing=NULL,*pMainViewDrawing=NULL;
	CSectionView* pView=NULL;
	double fMainViewRectW=0;
	int i=0;
	for(i=0;i<drawRectArr.GetSize();i++)
	{
		drawRectArr[i].m_bLayout=FALSE;
		pViewDrawing=(CSectionViewDrawing*)drawRectArr[i].m_pDrawing;
		pView=pViewDrawing->GetSectionView();
		if(pView->IsSlaveView()&&strlen(pView->Symbol)>0)
			drawSubRectArr.Add(drawRectArr[i]);		//����ͼ
		else	//������ͼ
		{
			drawMainRectArr.Add(drawRectArr[i]);
			if( pMainViewDrawing==NULL&&
				(pView->m_iViewType==VIEW_MAIN||pView->m_iViewType==VIEW_JOINTTUBE||
				 pView->m_iViewType==VIEW_FRONT))
			{
				pMainViewDrawing=pViewDrawing;
				fMainViewRectW=drawRectArr[i].width;
			}
		}
	}
	//2��������ͼ����ȡ����ͼ��λ����Ϣ,����������
	CTextOnlyDrawing *pDim=NULL;
	CHashStrList<long> idHashByStrKey;
	CArray<CSlaveDrawingRect,CSlaveDrawingRect&> slaveDrawRectArr;
	if(pMainViewDrawing)
	{
		for(pDim=pMainViewDrawing->GetPureTextDimList()->GetFirst();pDim;pDim=pMainViewDrawing->GetPureTextDimList()->GetNext())
		{
			if(pDim->iOrder!=3)	
				continue;	//��������ͼ��
			if(idHashByStrKey.GetValue(pDim->dimText))
				continue;
			idHashByStrKey.Add(pDim->dimText);	//�����ظ�������ͼ����ע
			CDrawingRect* pRect=NULL;
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
				CSlaveDrawingRect slaveRect;
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
	CHeapSort<CSlaveDrawingRect>::HeapSort(slaveDrawRectArr.GetData(),slaveDrawRectArr.GetSize(),compareDrawingRect3);
	//3��������ͼ������ͼ����Ŀ�Ⱥ͸߶�
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
	double fSubRectMaxUpHigh=0,fSubRectMaxDownHigh=0;
	for(i=0;i<drawSubRectArr.GetSize();i++)
	{
		if(fSubRectMaxUpHigh<drawSubRectArr[i].topLeft.y)
			fSubRectMaxUpHigh=drawSubRectArr[i].topLeft.y;
		if(fSubRectMaxDownHigh<drawSubRectArr[i].height-drawSubRectArr[i].topLeft.y)
			fSubRectMaxDownHigh=drawSubRectArr[i].height-drawSubRectArr[i].topLeft.y;
		fSumWidth+=drawRectArr[i].width;
	}
	double fSubRectMaxHigh=fSubRectMaxUpHigh+fSubRectMaxDownHigh;
	double fZoomScale=floor(fSumWidth/m_fFrameWidth)+1;
	if(fZoomScale>1&&pfPageCount)
		fSumHigh=fSubRectMaxHigh*fZoomScale;	//����ͼ̫����Ҫ�ֶ�����ʾ
	else
		fSumHigh=fSubRectMaxUpHigh;
	m_fFrameHeight+=fSumHigh;
	//4�����ձ�����������ͼ����,����ʣ��ռ�
	bool bCompulsoryHoriLayout=false;	//���߿�ȹ���ʱǿ�Ƹ�Ϊ������������ͼ
	m_fFrameHeight=max(m_fFrameHeight,m_fFixRectHeight);
	if(pfPageCount==NULL&&m_fFrameHeight/m_fFrameWidth>1&&!bZoomRelayout)
	{
		bCompulsoryHoriLayout=true;
		m_fFrameHeight-=fSumHigh;
	}
	if(bZoomRelayout&&m_fFrameWidth<limitWidth)
		m_fFrameWidth=limitWidth;
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
	if(pfPageCount)	//����pfPageCountʱ��ֻ�����������ҳ�� wht 18-08-02
		return true;
	//5��������ͼ���ض�����ͼ�������Ƚ��в���
	double fDist=0,fWidth=0;
	f3dPoint origin;
	origin.x=marginSpace;
	for(i=0;i<drawMainRectArr.GetSize();i++)
	{
		if(i==0)
			origin.y=bCompulsoryHoriLayout? - marginSpace : -(fSubRectMaxHigh+marginSpace);
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
		if(bCompulsoryHoriLayout)
			break;
		double fUseX=marginSpace,fOffsetX=0,fOffsetY=0;
		for(i=0;i<slaveDrawRectArr.GetSize();i++)
			slaveDrawRectArr[i].pSlaveRect->m_bLayout=FALSE;
		CSlaveDrawingRect* pRightestRect=NULL;
		for(i=0;i<slaveDrawRectArr.GetSize();i++)
		{
			//slaveDrawRectArr[i].fDistXΪ�������λ��
			//fDistRealXΪ��ʵ���к���λ��
			double fDistRealX=fUseX+slaveDrawRectArr[i].pSlaveRect->width*0.5;
			if(fDistRealX>fMainViewRectW&&!bExclusiveEndConn)
			{
				bExclusiveEndConn=true;
				break;
			}
			else if(fDistRealX>fMainViewRectW)
				continue;
			CSectionViewDrawing* pDrawing=(CSectionViewDrawing*)slaveDrawRectArr[i].pSlaveRect->m_pDrawing;
			if( pDrawing&&bExclusiveEndConn&&(
				pDrawing->GetSectionView()->m_iViewType==VIEW_FL||
				pDrawing->GetSectionView()->m_iViewType==VIEW_INSERTPLATE||
				pDrawing->GetSectionView()->m_iViewType==VIEW_FOOTNAILSLOT||
				pDrawing->GetSectionView()->m_iViewType==VIEW_FOOTNAILANGLE))
				continue;
			fOffsetY=-fSubRectMaxUpHigh+slaveDrawRectArr[i].pSlaveRect->initTopLeft.y;//height-fSubRectMaxHigh;
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
				if(fDistRealX>fMainViewRectW)
					break;
				CSectionViewDrawing* pDrawing=(CSectionViewDrawing*)slaveDrawRectArr[i].pSlaveRect->m_pDrawing;
				if( pDrawing&&(
					pDrawing->GetSectionView()->m_iViewType==VIEW_FL||
					pDrawing->GetSectionView()->m_iViewType==VIEW_INSERTPLATE||
					pDrawing->GetSectionView()->m_iViewType==VIEW_FOOTNAILSLOT||
					pDrawing->GetSectionView()->m_iViewType==VIEW_FOOTNAILANGLE))
					continue;
				fOffsetY=-fSubRectMaxUpHigh+slaveDrawRectArr[i].pSlaveRect->initTopLeft.y;//height-fSubRectMaxHigh;
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
				CSlaveDrawingRect* pCurrRect=pRightestRect;
				while(startindex>0&&moveleft>EPS)
				{
					CSlaveDrawingRect* pPrevRect=&slaveDrawRectArr[startindex-1];
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
	//6����ʣ����ͼ���ν��в���
	drawRectArr.RemoveAll();
	for(i=0;i<drawMainRectArr.GetSize();i++)
		drawRectArr.Add(drawMainRectArr[i]);
	for(i=0;i<slaveDrawRectArr.GetSize();i++)
		drawRectArr.Add(*(slaveDrawRectArr[i].pSlaveRect));
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
void CDrawingRectLayout::AdjustTubeOneFrontViewPartGroupLayout(double marginSpace,double limitWidth/*=0*/,double maxHeight/*=0*/)
{	
	if(limitWidth==0||maxHeight==0)
		return;
	//1.����ƫ�Ʋ���
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
	double up_offset_y=(maxHeight-fabs(fMinY))*0.4;
	double down_offset_y=(maxHeight-fabs(fMinY))*0.6;
	if(up_offset_y<=0||down_offset_y<=0)
		return;
	//2.��ͼ�������� 
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
	//3.����ͼ������ͼ�ص�ʱ����ͼ����
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
	double offset_y=maxHeight-fabs(fMinY);
	//4.����ͼ����ϸ���ص�ʱ������ͼ��������
	if(m_fFixRectWidth<=0||m_fFixRectHeight<=0)
		return;
	//������Ҫ������
	double offset_left=0,offset_down=0;
	if(fSalveViewMaxX>(limitWidth-m_fFixRectWidth)&&fabs(fSalveViewMaxY)<m_fFixRectHeight)
	{
		offset_down=m_fFixRectHeight-fabs(fSalveViewMaxY)+marginSpace;
		offset_left=fSalveViewMaxX-(limitWidth-m_fFixRectWidth)+marginSpace;
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
}
//
void CDrawingRectLayout::Relayout3ViewsPartGroup(double h_to_w,double marginSpace)
{
	CDrawingRect *pFrontViewRect=NULL,*pSideViewRect=NULL,*pTopViewRect=NULL;
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
		return;

	m_fFrameWidth=pFrontViewRect->width+pSideViewRect->width+m_fFixRectWidth;	//���Ʊ߿��ʼ��
	m_fFrameHeight=pFrontViewRect->height+pTopViewRect->height;					//���Ʊ߿��ʼ��
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
	pFrontViewRect->topLeft.Set(0,0);
	pSideViewRect->topLeft.Set(pFrontViewRect->width+marginSpace,0);
	pTopViewRect->topLeft.Set(0,-1*(pFrontViewRect->height+marginSpace));
	//����ʵ�ʻ�ͼ�����С���²��ֻ�ͼ���������λ��,�ⲿ�ִ���������д��������δ���ǵ�������ϸ��
	ResetDrawingRectPosition();	
}
void CDrawingRectLayout::RelayoutPlateCommonPartGroup(double h_to_w,double marginSpace)
{
	CArray<CDrawingRect,CDrawingRect&> drawKaoMeRectArr;
	CDrawingRect xFrontRect,xSideRect,xPlanRect;
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
		return Relayout(h_to_w,marginSpace);
	//�����������Ŀ�Ⱥ͸߶�
	m_fFrameWidth=m_fFixRectWidth+max(fKaoMeWidth,fFrameWidth);
	m_fFrameWidth+=max(m_fFixRectHeight,fFrameHigh);
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
	drawRectArr.RemoveAll();
	f3dPoint origin(0.5*marginSpace,-0.5*marginSpace);
	xFrontRect.topLeft=origin;
	xFrontRect.m_bLayout=TRUE;
	drawRectArr.Add(xFrontRect);
	//����ͼ
	if(xSideRect.m_pDrawing)
	{
		origin=xFrontRect.topLeft;
		origin.x+=xFrontRect.width+0.5*marginSpace;
		origin.y-=0.5*xFrontRect.height-0.5*xSideRect.height;
		xSideRect.topLeft=origin;
		xSideRect.m_bLayout=TRUE;
		drawRectArr.Add(xSideRect);
	}
	//����ͼ
	if(xPlanRect.m_pDrawing)
	{
		origin=xFrontRect.topLeft;
		origin.x+=0.5*xFrontRect.width-0.5*xPlanRect.width;
		origin.y-=xFrontRect.height+0.5*marginSpace;
		xPlanRect.topLeft=origin;
		xPlanRect.m_bLayout=TRUE;
		drawRectArr.Add(xPlanRect);
		origin.y-=xPlanRect.height+marginSpace;
	}
	//��ģ�Ǹ�
	origin.x=xFrontRect.topLeft.x;
	for(int i=0;i<drawKaoMeRectArr.GetSize();i++)
	{
		drawKaoMeRectArr[i].topLeft=origin;
		drawKaoMeRectArr[i].m_bLayout=TRUE;
		origin.x+=drawKaoMeRectArr[i].width+0.5*marginSpace;
		drawRectArr.Add(drawKaoMeRectArr[i]);
	}
	//����ʵ�ʻ�ͼ�����С���²��ֻ�ͼ���������λ��,�ⲿ�ִ���������д��������δ���ǵ�������ϸ��
	ResetDrawingRectPosition();	
}

CDrawingRectLayout::CDrawingRectLayout()
{
	m_fFixRectWidth=m_fFixRectHeight=m_fFrameWidth=m_fFrameHeight=0;fixRectAlign=TopRight;
	m_fBlkZoomcoef=1.0;
	m_bAutoZoomFixRect=true;
	m_bPlaceFixRect=false;
}
//����ʵ�ʻ�ͼ�����С���²��ֻ�ͼ���������λ��
void CDrawingRectLayout::ResetDrawingRectPosition()
{
	int i=0;
	SCOPE_STRU scope;
	for(i=0;i<drawRectArr.GetSize();i++)
	{
		if(i<0||i>drawRectArr.GetUpperBound())
			continue;
		f3dPoint topLeft=drawRectArr[i].topLeft;
		f3dPoint bottomRight(topLeft.x+drawRectArr[i].width,topLeft.y-drawRectArr[i].height);
		scope.VerifyVertex(topLeft);
		scope.VerifyVertex(bottomRight);
	}
	double fScopeHeight=scope.high();	//��ͼ������ʵ�߶�
	double fScopeWidth=scope.wide();	//��ͼ�������ӿ��
	if(fScopeHeight>m_fFrameHeight||fScopeWidth>m_fFrameWidth)
		return;	
	f3dPoint offset_pt(0.5*(m_fFrameWidth-fScopeWidth),
		-0.5*(m_fFrameHeight-fScopeHeight));//ƫ�Ƶ�
	if(	m_fFixRectWidth!=0)
		offset_pt.x=0;

	//������ο�λ��
	for(i=0;i<drawRectArr.GetSize();i++)
	{
		if(i<0||i>drawRectArr.GetUpperBound())
			continue;
		drawRectArr[i].topLeft+=offset_pt;
	}
}

double CDrawingRectLayout::GetRealDrawingHeight()
{
	if(drawRectArr.GetSize()<=0)
		return 0;
	CMinDouble minY;
	CMaxDouble maxY;
	for(int i=0;i<drawRectArr.GetSize();i++)
	{
		minY.Update(drawRectArr[i].topLeft.y);
		maxY.Update(drawRectArr[i].topLeft.y);
		minY.Update(drawRectArr[i].topLeft.y-drawRectArr[i].height);
		maxY.Update(drawRectArr[i].topLeft.y-drawRectArr[i].height);
	}
	return (maxY.number-minY.number);
}

//////////////////////////////////////////////////////////////////////////
//CDrawingRectArray
CDrawingRectArray::CDrawingRectArray()
{
	m_fFrameWidth=0;	//�߿���
	m_fFrameHeight=0;	//�߿�߶�
	m_fMarginSpace=0;	//���
}

CDrawingRectArray& CDrawingRectArray::operator = (CDrawingRectArray &drawRectArr)
{
	CopyProperty(&drawRectArr);
	return *this;
}

void CDrawingRectArray::CopyProperty(CDrawingRectArray *pDrawingRectArray)
{
	if(pDrawingRectArray==NULL)
		return;
	m_fFrameHeight=pDrawingRectArray->m_fFrameHeight;
	m_fFrameWidth=pDrawingRectArray->m_fFrameWidth;
	m_fMarginSpace=pDrawingRectArray->m_fMarginSpace;
	drawRectArr.RemoveAll();
	for(int i=0;i<pDrawingRectArray->GetDrawRectArrSize();i++)
	{
		CDrawingRect rect=GetDrawRectAt(i);
		drawRectArr.Add(rect);
	}
}

//
void CDrawingRectArray::InitDrawingRectArray(double width,double height,double marginSpace)
{
	m_fFrameWidth=width;	//�߿���
	m_fFrameHeight=height;	//�߿�߶�
	m_fMarginSpace=marginSpace;	//���
	drawRectArr.RemoveAll();
}
//���һ����������
int CDrawingRectArray::AddUsedDrawingRect(f2dPoint topLeft,double width,double height)
{
	CDrawingRect rect;
	rect.topLeft.Set(topLeft.x-m_fMarginSpace,topLeft.y+m_fMarginSpace);
	rect.width=width+m_fMarginSpace*2;
	rect.height=height+m_fMarginSpace*2;
	rect.m_bLayout=TRUE;	
	return drawRectArr.Add(rect);
}
//�õ�һ�����þ�������
BOOL CDrawingRectArray::GetBlankRectTopLeft(double width,double height,f3dPoint *topLeft,BOOL bSearchHori/*=TRUE*/)
{ 
	int i,j,k,nLayoutNum=0;
	int maxY=0;
	//��֤stepX��stepY��Ϊ0 wht 11-06-16
	double stepX=max(0.5,m_fFrameWidth/100),stepY=max(0.5,m_fFrameHeight/100);
	f3dPoint origin;
	for(i=0;;i++)	//����������
	{
		origin.y=-i*stepY;
		if(origin.y-m_fMarginSpace-height<-m_fFrameHeight)
			break;
		for(j=0;;j++)
		{
			origin.x=j*stepX;
			if(origin.x+m_fMarginSpace+width>m_fFrameWidth)
				break;
			BOOL bHasInters=FALSE;
			for(k=0;k<drawRectArr.GetSize();k++)
			{
				if(IsOrthoRectInters(drawRectArr[k].topLeft,drawRectArr[k].width,drawRectArr[k].height,origin,width,height))
				{
					bHasInters=TRUE;
					break;
				}
			}
			if(!bHasInters)
			{
				if(topLeft)
				{
					*topLeft=origin;
					//���Ǽ�϶ֵ
					topLeft->x+=m_fMarginSpace;
					topLeft->y-=m_fMarginSpace;
				}
				return TRUE;
			}
			if(!bSearchHori)
				break;	//�����к�������
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifndef __TSA_FILE_
LINEPARTSET OUTSIDEJGSETDRAW;
LINEPARTSET INSIDEJGSETDRAW;
void DrawJgStru(AcDbBlockTableRecord *pBlockTableRecord,CDrawPartInfo& drawPartInfo,CDrawCoordTrans dcs,
				BOOL bSetUpDrawing,BOOL bOnlyDrawJg=FALSE,AcDbObjectId *pOutEntId=NULL);
void DimJgZhun(AcDbBlockTableRecord *pBlockTableRecord,CDrawCoordTrans dcs,
			   CDimJgZhun &dim_jg_zhun, const JGZJ &jgzj,int now_g,BOOL bSpecialZhun=FALSE);
void DrawAngleToMap(LINEPARTSET &partset,BOOL bInside,AcDbBlockTableRecord *pBlockTableRecord,
					CDrawCoordTrans dcs,BOOL bSetupDrawing,AcDbObjectId *pOutEntId/*=NULL*/)
{
	CLDSLinePart *pPart=NULL;
	LINEPARTSET tmpartset;
	if(bInside)
	{
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			tmpartset.append(pPart);
	}
	else
	{
		for(pPart=partset.GetTail();pPart;pPart=partset.GetPrev())
			tmpartset.append(pPart);
	}
	for(pPart=tmpartset.GetFirst();pPart;pPart=tmpartset.GetNext())
	{
		if(pPart)
			DrawJgStru(pBlockTableRecord,CDrawPartInfo(pPart),dcs,bSetupDrawing,FALSE,pOutEntId);
		else 
			continue;
	}
}
CLsListDraw LSDRAW;
CLsDrawUnit *CLsListDraw::AppendLsRef(CLDSBolt *pBolt, BOOL bOriginal,f3dPoint view_norm,
									  f3dLine *pSpreadLine/*=NULL*/,double rotate_angle/*=0*/)
{
	if(pBolt==NULL)
		return NULL;
	CLsDrawUnit *pLsDraw=NULL;
	for(pLsDraw=GetFirst();pLsDraw;pLsDraw=GetNext())
	{
		if(pLsDraw->pLs==pBolt&&!pLsDraw->bOriginal&&bOriginal)
		{
			pLsDraw->bOriginal=TRUE;
			if(fabs(pLsDraw->rotate_angle)<=EPS&&pSpreadLine)
			{	//��Ҫ��ת����˨>��ͨ��˨ (������ӵ���Ҫ��ת����˨Ϊ׼) wht 10-12-10
				pLsDraw->spreadLine.startPt=pSpreadLine->startPt;	//չ����ʼ��
				pLsDraw->spreadLine.endPt=pSpreadLine->endPt;		//չ�����ն�
				pLsDraw->rotate_angle=rotate_angle;	//��ת�Ƕ�
				//�����ж���˨�����滹�ǲ������
				if(fabs(view_norm*pBolt->ucs.axis_z)>EPS_COS2)
					pLsDraw->front0_side1=0;
				else
					pLsDraw->front0_side1=1;
			}
			return pLsDraw;
		}
		else if(pLsDraw->pLs==pBolt)
		{
			if(fabs(pLsDraw->rotate_angle)<=EPS&&pSpreadLine)
			{	//��Ҫ��ת����˨>��ͨ��˨ (������ӵ���Ҫ��ת����˨Ϊ׼) wht 10-12-10
				pLsDraw->spreadLine.startPt=pSpreadLine->startPt;	//չ����ʼ��
				pLsDraw->spreadLine.endPt=pSpreadLine->endPt;		//չ�����ն�
				pLsDraw->rotate_angle=rotate_angle;	//��ת�Ƕ�
				//�����ж���˨�����滹�ǲ������
				if(fabs(view_norm*pBolt->ucs.axis_z)>EPS_COS2)
					pLsDraw->front0_side1=0;
				else
					pLsDraw->front0_side1=1;
			}
			return pLsDraw;
		}
	}
	pLsDraw=drawls.append();
	pLsDraw->bOriginal=bOriginal;
	normalize(view_norm);
	pLsDraw->pLs=pBolt;
	pLsDraw->work_norm=view_norm;
	if(fabs(view_norm*pBolt->ucs.axis_z)>EPS_COS2)
		pLsDraw->front0_side1=0;
	else
		pLsDraw->front0_side1=1;
	//�˴������ٴο�����˨����ƫ��������Ϊ������˨λ��ucs.originʱ�ѿ��ǹ�����ƫ���� wht 10-07-13
	pLsDraw->pos=pBolt->ucs.origin;//+pBolt->ucs.axis_z*pBolt->des_base_pos.norm_offset.Thick(pBolt->SuperiorTower());
	if(pSpreadLine)
	{
		pLsDraw->spreadLine.startPt=pSpreadLine->startPt;	//չ����ʼ��
		pLsDraw->spreadLine.endPt=pSpreadLine->endPt;		//չ�����ն�
		pLsDraw->rotate_angle=rotate_angle;	//��ת�Ƕ�
	}
	return pLsDraw;
}
BOOL CLsListDraw::DelLsRef(long handle)
{
	for(CLsDrawUnit *pLsDraw=GetFirst();pLsDraw;pLsDraw=GetNext())
	{
		if(pLsDraw->pLs->handle==handle)
		{
			drawls.DeleteCursor();
			return TRUE;
		}
	}
	return FALSE;
}
void CLsListDraw::EmptyDrawList()
{
	drawls.Empty();
}
CLsDrawUnit* CLsListDraw::GetFirst()
{
	return drawls.GetFirst();
}
CLsDrawUnit* CLsListDraw::GetNext()
{
	return drawls.GetNext();
}
void CLsListDraw::PushStack()
{
	drawls.push_stack();
}
void CLsListDraw::PopStack()
{
	drawls.pop_stack();
}

void CLsListDraw::DrawBolt(AcDbBlockTableRecord *pBlockTableRecord,CLsDrawUnit *pLsDraw,
						   CDrawCoordTrans dcs,AcDbObjectId *pOutEntId/*=NULL*/)
{
	f3dPoint f_pos;
	AcGePoint3d acad_pos;
	char blkname[50];
	double angle=0;	//���ڼ�¼�Ŷ���ͼ����ת����
	if(pLsDraw==NULL)
		return;
	if(pLsDraw->pLs==NULL)
		return;
	if(pLsDraw->pLs->IsFootNail())
	{
		CLDSLineSlot *pLineSlot=(CLDSLineSlot*)Ta.FromPartHandle(pLsDraw->pLs->des_base_pos.hPart,CLS_LINESLOT);
		if(pLineSlot&&pLineSlot->GetFirstLsRef()->GetLsPtr()==pLsDraw->pLs)
			return;	//�������Ŷ������ĽŶ������л��� 
	}
	if(pLsDraw->front0_side1==1)	//�������˨�Ŷ�
	{
		f3dPoint work_norm(pLsDraw->pLs->get_norm());
		if(pLsDraw->pLs->IsFootNail())
			strcpy(blkname,"NAIL_SIDE");
		else
			strcpy(blkname,"LS_SIDE");
		work_norm=dcs.TransVToDraw(work_norm);//vector_trans(work_norm,ucs,FALSE);work_norm.z = 0;
		if(work_norm.mod()<EPS)	//������˨���ò�����
			return;
		if(pLsDraw->bDrawSideLine)
		{	//��ֱ�ߵķ�ʽ����ƽ�Ų��õ���˨
			f3dPoint start=pLsDraw->pLs->ucs.origin;
			start=dcs.TransToDraw(start);
			double capthick=pLsDraw->pLs->get_d()*0.6;
			f3dPoint end=start+((pLsDraw->pLs->get_L0()+capthick)/dcs.m_fDrawScale)*work_norm;
			start-=(capthick/dcs.m_fDrawScale)*work_norm;
			CreateAcadPolyLine(pBlockTableRecord,start,end,0.1,pLsDraw->pLs->handle);
			return;
		}
		double dd = work_norm.y/work_norm.mod();
		angle = asin(dd);
		if(work_norm.x<0)
			angle = Pi-angle;
	}
	else	//�������˨�Ŷ�
	{
		if(pLsDraw->pLs->IsFootNail())	//�Ŷ�
			strcpy(blkname,"NAIL_FRONT");
		else if(!pLsDraw->pLs->IsTwoCap())	//��˨
			sprintf(blkname,"M%dX%.0f",pLsDraw->pLs->get_d(),pLsDraw->pLs->get_L());
		else //˫ñ��˨
			sprintf(blkname,"DM%dX%.0f",pLsDraw->pLs->get_d(),pLsDraw->pLs->get_L());
	}
	BOOL bRet=FALSE;
	//������˨��λ��
	f_pos=pLsDraw->pos;
	if(fabs(pLsDraw->rotate_angle)>EPS)	//��ָ������ת wht 10-12-10
	{
		dcs.m_fSpreadAngle=pLsDraw->rotate_angle;
		dcs.m_xSpreadLine=pLsDraw->spreadLine;
	}
	f_pos=dcs.TransToDraw(f_pos);
	//������˨����Բ�׻��� wht 11-07-22
	if((sys.general_set.StruMapScale.iBoltSymbolStyle>=1||pLsDraw->bOriginal)&&(pLsDraw->front0_side1==1||!pLsDraw->pLs->m_bVirtualPart)) 
	{		//�׻���ͼ��/������˨/������˨
		double tmscale;
		if(sys.general_set.StruMapScale.iBoltSymbolStyle==2)	//���Ʊ���ͼ��ʱ,��1:20Ϊ��׼���� wjh-2011.10.8
			tmscale=sys.general_set.StruMapScale.fBoltSymbolFactor/dcs.m_fDrawScale;
		else if(sys.general_set.StruMapScale.iBoltSymbolStyle==3)	//���Ʊ���ֱ��ͼ��
		{
			double holed=pLsDraw->pLs->get_d()+pLsDraw->pLs->hole_d_increment;//sys.general_set.fHoleAugmentInNC;
			tmscale=holed*sys.general_set.StruMapScale.fBoltDiaFactor/sys.general_set.StruMapScale.fRealToDraw;
			if(pLsDraw->pLs->get_d()==24)
				tmscale/=1.2;	//ĿǰM24��˨ͼ�����ձ�Ĭ��Ϊ2.4mm����ͳһת��Ϊ2mm�����������ֱ��ͼ�� wjh-2015.10.08
		}
		else
			tmscale=1.0;	//����ͼ��ʱӦԭ��С���롡wjh-2011.8.23
		//��ʱ����ĳЩԭ��(�κŲ�ƥ��),���·�������˨ʱ,Ҳ��Ҫ��ע��Ȧ��Ϣ wjh-2012.6.17
		//if(pLsDraw->bOriginal)
			InsertPadPoint(pBlockTableRecord,pLsDraw,f_pos,pOutEntId);
		AcDbObjectId outEntId=0;
		if(pLsDraw->pLs->IsFootNail()||	//����˨Ϊ�Ŷ�ʱ��Ҫ������Ӧ��������չ��     wht 11-04-07
			(pLsDraw->front0_side1==0&&pLsDraw->bOriginal))	//������������������˨��Ҫ������Ӧ��������չ�� wjh-2011.10.08
			bRet=InsertBlock(pBlockTableRecord,f_pos,blkname,tmscale,angle,pLsDraw->pLs->handle,&outEntId);
		else if(pLsDraw->front0_side1!=0||		//������˨���������˨���,������˨����Ҫ�����
			sys.general_set.StruMapScale.iBoltSymbolStyle>=1)//�����������˨ѡ����˨ͼ��ʱ������Ӧ�Ŀ飬�����������˨����� wjh-2011.10.08
			bRet=InsertBlock(pBlockTableRecord,f_pos,blkname,tmscale,angle,0,&outEntId);
		//��������˨������CADʵ�����Ϊ�գ���Ҫ�ڴ˴���ֵ�������ע��˨��Ϣʱ���ܻ�©�� wht 12-03-21
		if(pOutEntId&&pOutEntId->isNull())
			*pOutEntId=outEntId;
	}
	//ֻҪbRet==FALSE����Ҫִ�����´���  wht 11-07-20
	if(!bRet)//sys.general_set.iLsApertureStyle==0&&	
	{	//������˨��Բ�״���
		double hole_radius=(pLsDraw->pLs->get_d()+pLsDraw->pLs->hole_d_increment)*0.5/dcs.m_fDrawScale;
		double draw_radius=max(hole_radius,1);
		AcDbObjectId entId=CreateAcadCircle(pBlockTableRecord,f_pos,draw_radius,pLsDraw->pLs->handle);
		if(pOutEntId)
			*pOutEntId=entId;
		//ԭ������ע���߿׵Ŀ׾�������Ӧ����������˨ֱ���ı�ע��wjh-2014.5.16
		//if(pLsDraw->pLs->m_bVirtualPart&&pLsDraw->bDimHoleDiameter)
		{
			CXhChar16 sText;
			sText.Printf("%%%%c%f",pLsDraw->pLs->get_d()+pLsDraw->pLs->hole_d_increment);
			SimplifiedNumString(sText);
			f3dPoint vec(draw_radius*0.707,draw_radius*0.707);
			f3dPoint chordPos(f_pos.x-vec.x,f_pos.y-vec.y),farChordPos(f_pos.x+vec.x,f_pos.y+vec.y);
			double leaderLength=2;
			DimDiameter(pBlockTableRecord,chordPos,farChordPos,leaderLength,sText,DimStyleTable::dimRadiusStyle.dimStyleId);
		}
	}
}

void CLsListDraw::DrawToMap(AcDbBlockTableRecord *pBlockTableRecord,UCS_STRU &ucs,double scale)
{
	for(CLsDrawUnit *pLsDraw=drawls.GetFirst();pLsDraw!=NULL;pLsDraw=drawls.GetNext())
		DrawBolt(pBlockTableRecord,pLsDraw,CDrawCoordTrans(ucs,scale));
}
void CLsListDraw::InsertPadPoint(AcDbBlockTableRecord *pBlockTableRecord,
					  CLsDrawUnit *pLsDraw,f3dPoint pos,AcDbObjectId *pOutEntId/*=NULL*/)
{
	AcDbXrecord *pXrec;
	AcDbDictionary *pDict;
	AcDbObjectId PadId,dictObjId,xrecObjId;
	AcGePoint3d acad_insert_pos;
	Cpy_Pnt(acad_insert_pos,pos);
	if(pLsDraw!=NULL&&pLsDraw->pLs!=NULL)
	{
		AcDbPoint *pPadPoint;
		pPadPoint = new AcDbPoint(acad_insert_pos);
		if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,PadId,pPadPoint))//��ʵ��д�����¼
		{
			for(CLsSpaceDimUnit *pDim=LSSPACEDIM.GetFirst();pDim;pDim=LSSPACEDIM.GetNext())
			{
				if(pDim->hStartLs==pLsDraw->pLs->handle)
					pDim->StartLsId=PadId;
				else if(pDim->hEndLs==pLsDraw->pLs->handle)
					pDim->EndLsId=PadId;
			}
			if(pOutEntId)	//��˨��Ӧ��ʵ��ID wht 11-06-20
				*pOutEntId=PadId;
			if(pLsDraw->pLs->DianQuan.N<=0)
			{
				pPadPoint->close();//���ùرն���ĳ�Ա����
				return;	//�޵�Ȧ
			}
			//
			pPadPoint->createExtensionDictionary();
			dictObjId = pPadPoint->extensionDictionary();
			pPadPoint->close();//���ùرն���ĳ�Ա����
		}
		else
		{
			pPadPoint->close();
			return;
		}
		acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
		pXrec = new AcDbXrecord;
#ifdef _ARX_2007
		pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
		wchar_t sPad[24];
		swprintf(sPad,L"-%dX%d",pLsDraw->pLs->DianQuan.thick,pLsDraw->pLs->DianQuan.N);
#else
		pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
		char sPad[24]="";
		sprintf(sPad,"-%dX%d",pLsDraw->pLs->DianQuan.thick,pLsDraw->pLs->DianQuan.N);
#endif
		pDict->close();
	
		struct resbuf *head;
		head = acutBuildList(AcDb::kDxfInt32,pLsDraw->pLs->handle,
			AcDb::kDxfInt32,STEELTYPE_BOLT,AcDb::kDxfText, sPad,0);
		pXrec->setFromRbChain(*head);
		acutRelRb(head);
		pXrec->close();
	}
}

CLDSLinePart* CLsSpaceDimUnit::GetDatumRod(BYTE dimvec0_plate1/*=0*/)
{
	CLDSPlate *pPlate=(CLDSPlate*)Ta.FromPartHandle(hFatherPlate,CLS_PLATE);
	CLDSLinePart *pDatumRod=(pPlate!=NULL)?(CLDSLinePart*)Ta.Parts.FromHandle(pPlate->designInfo.m_hBasePart,CLS_LINEANGLE):NULL;
	if(dimvec0_plate1==1)
		return pDatumRod;
	CLDSBolt *pStartBolt=(CLDSBolt*)Ta.Parts.FromHandle(hStartLs,CLS_BOLT);
	CLDSBolt *pEndBolt=(CLDSBolt*)Ta.Parts.FromHandle(hEndLs,CLS_BOLT);
	if(pPlate==NULL||pStartBolt==NULL||pEndBolt==NULL)
		return NULL;
	CLDSLinePart *pStartRod=(CLDSLinePart*)Ta.Parts.FromHandle(pStartBolt->des_base_pos.hPart,CLS_LINEANGLE);
	CLDSLinePart *pEndRod=(CLDSLinePart*)Ta.Parts.FromHandle(pEndBolt->des_base_pos.hPart,CLS_LINEANGLE);
	if(pStartRod==NULL||pEndRod==NULL)
		return NULL;
	GEPOINT verfiy_vec=pStartBolt->ucs.origin-pEndBolt->ucs.origin;
	normalize(verfiy_vec);
	double dd_start=fabs(verfiy_vec*pStartRod->ucs.axis_z);
	double dd_end=fabs(verfiy_vec*pEndRod->ucs.axis_z);
	double dd_max=dd_end;
	CLDSLinePart *pRod=pEndRod;
	if(dd_start>dd_end)
	{
		pRod=pStartRod;
		dd_max=dd_start;
	}
	if(pDatumRod!=NULL)
	{	
		double dd_datum=fabs(verfiy_vec*pDatumRod->ucs.axis_z);
		if(dd_datum>dd_max)
			pRod=pDatumRod;
	}
	return pRod;
}

ATOM_LIST<CLsSpaceDimUnit> LSSPACEDIM;
void DimSingleLsSpace(AcDbBlockTableRecord *pBlockTableRecord,
					  CLsSpaceDimUnit *pSpaceDim,double fDimDist=10,
					  double fSpace=3,BOOL bInside=TRUE)
{	//��ע������˨���
	if(pSpaceDim==NULL)
		return;
	f3dPoint f_pos;
	AcGePoint3d acad_pos;
	ROTATEDIM_STRU align_dim;
	AcDbEntity *pStartEnt=NULL,*pEndEnt=NULL;
	acdbOpenObject(pStartEnt,pSpaceDim->StartLsId,AcDb::kForRead);
	acdbOpenObject(pEndEnt,pSpaceDim->EndLsId,AcDb::kForRead);
	//��ʱ�ر�CADʵ�����Ԥ���������
	if(pStartEnt)
		pStartEnt->close();
	if(pEndEnt)
		pEndEnt->close();
	if(pStartEnt==NULL||pEndEnt==NULL)
		return;
	AcGePoint3d acad_pnt;
	if (pStartEnt->isKindOf(AcDbCircle::desc())&&pEndEnt->isKindOf(AcDbCircle::desc()))
	{
		acad_pnt=((AcDbCircle*)pStartEnt)->center();
		Cpy_Pnt(align_dim.startPos,acad_pnt);
		acad_pnt=((AcDbCircle*)pEndEnt)->center();
		Cpy_Pnt(align_dim.endPos,acad_pnt);
	}
	else if (pStartEnt->isKindOf(AcDbPoint::desc())&&pEndEnt->isKindOf(AcDbPoint::desc()))
	{
		acad_pnt=((AcDbPoint*)pStartEnt)->position();
		Cpy_Pnt(align_dim.startPos,acad_pnt);
		acad_pnt=((AcDbPoint*)pEndEnt)->position();
		Cpy_Pnt(align_dim.endPos,acad_pnt);
	}
	else
		return;
	double angle=pSpaceDim->dim_angle;
	if(pSpaceDim->bCalDimAngle)
		angle=Cal2dLineAng(align_dim.startPos.x,align_dim.startPos.y,align_dim.endPos.x,align_dim.endPos.y);
	if(angle>Pi/2.0&&angle<=1.5*Pi)
	{
		angle-=Pi;
		align_dim.dimPos=align_dim.endPos+(align_dim.startPos-align_dim.endPos)*0.5;
	}
	else
		align_dim.dimPos=align_dim.startPos+(align_dim.endPos-align_dim.startPos)*0.5;
	align_dim.angle=angle;
	align_dim.dist=pSpaceDim->dist;
	int nFlag=1;
	if(!bInside)	//�������Ʊ�ע���� wht 11-06-30
		nFlag=-1;
	align_dim.startPos=GetPtInPolar(align_dim.startPos,angle+nFlag*Pi/2.0,fSpace);
	align_dim.endPos=GetPtInPolar(align_dim.endPos,angle+nFlag*Pi/2.0,fSpace);
	align_dim.dimPos=GetPtInPolar(align_dim.dimPos,angle+nFlag*Pi/2.0,fDimDist);
	char sDimText[20];
	if (pSpaceDim->nLsSpaceNum>1)
	{
		if (pSpaceDim->bMergeDimLsSpace)
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sDimText,"%.0f��%d",align_dim.dist,pSpaceDim->nLsSpaceNum);
#else
			sprintf(sDimText,"%.0f��%d",align_dim.dist,pSpaceDim->nLsSpaceNum);
#endif
		else
		{
			ROTATEDIM_STRU alignDim;
			f3dPoint pos=(align_dim.endPos-align_dim.startPos)/pSpaceDim->nLsSpaceNum;
			for (int i=0;i<pSpaceDim->nLsSpaceNum-1;i++)
			{
				alignDim.startPos=align_dim.startPos+pos*i;
				alignDim.endPos=align_dim.startPos+pos*(i+1);
				if(angle>Pi/2.0&&angle<=1.5*Pi)
				{
					angle-=Pi;
					alignDim.dimPos=alignDim.endPos+(alignDim.startPos-alignDim.endPos)*0.5;
				}
				else
					alignDim.dimPos=alignDim.startPos+(alignDim.endPos-alignDim.startPos)*0.5;
				alignDim.angle=angle;
				alignDim.dist=pSpaceDim->dist;
				sprintf(sDimText,"%.0f",alignDim.dist);
				int nFlag=1;
				if(!bInside)	//�������Ʊ�ע���� wht 11-06-30
					nFlag=-1;
				alignDim.startPos=GetPtInPolar(alignDim.startPos,angle+nFlag*Pi/2.0,fSpace);
				alignDim.endPos=GetPtInPolar(alignDim.endPos,angle+nFlag*Pi/2.0,fSpace);
				alignDim.dimPos=GetPtInPolar(alignDim.dimPos,angle+nFlag*Pi/2.0,fDimDist);
				AcDbObjectId dimId=DimAngleSize(pBlockTableRecord,
					alignDim.startPos,alignDim.endPos,
					alignDim.dimPos,sDimText,DimStyleTable::dimStyle.dimStyleId,alignDim.angle);
				DRAGSET.Add(dimId);//��ʵ��д�����¼
			}
			align_dim.startPos=align_dim.endPos-pos;
		}
	}
	else
		sprintf(sDimText,"%.0f",align_dim.dist);
	AcDbObjectId dimId=DimAngleSize(pBlockTableRecord,
		align_dim.startPos,align_dim.endPos,
		align_dim.dimPos,sDimText,DimStyleTable::dimStyle.dimStyleId,align_dim.angle);
	DRAGSET.Add(dimId);//��ʵ��д�����¼
}
void DimLsSpaceToMap(AcDbBlockTableRecord *pBlockTableRecord,UCS_STRU &ucs)
{
	DimStyleTable::dimStyle.InitDimStyle(sys.dim_map.fDimTextSize);	//��ע�ָ�
	GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
	for(CLsSpaceDimUnit *pSpaceDim=LSSPACEDIM.GetFirst();pSpaceDim!=NULL;pSpaceDim=LSSPACEDIM.GetNext())
		DimSingleLsSpace(pBlockTableRecord,pSpaceDim);
}
#endif
//ȫ�ֱ���
CDimPartSpecList dimpartspec;
void CDimPartSpecList::SetBackMirStyle(int AXISX0_AXISY1)
{
	back_mir_style=AXISX0_AXISY1;
}
int CDimPartSpecList::BackMirStyle()
{
	return back_mir_style;
}
CDimPartSpec *CDimPartSpecList::AppendPartSpec(CLDSDbObject *pDbObj,f3dPoint dim_pos,BOOL bDimByAnalysisMode,
											   BOOL bDrawRefLine/*=FALSE*/,double angle/*=0*/)
{
	if(pDbObj==NULL)
		return NULL;
	else if(pDbObj->IsPart())
	{
		CLDSPart *pPart=(CLDSPart*)pDbObj;
		if(pPart->IsLinePart())
			return AppendLinePartSpec((CLDSLinePart*)pPart,dim_pos,bDimByAnalysisMode,bDrawRefLine,angle);
#ifndef __TSA_FILE_
		else if(pPart->GetClassTypeId()==CLS_PLATE&&!bDimByAnalysisMode)
			return AppendPlatePartSpec((CLDSPlate*)pPart,dim_pos,bDrawRefLine);
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&!bDimByAnalysisMode)
			return AppendParamPlatePartSpec((CLDSParamPlate*)pPart,dim_pos,bDrawRefLine);
#endif
		else
			return NULL;
	}
	else if(pDbObj->IsNode())
		return AppendNodeSpec((CLDSNode*)pDbObj,dim_pos,bDimByAnalysisMode,bDrawRefLine);
	else
		return NULL;
}

CDimPartSpec *CDimPartSpecList::AppendNodeSpec(CLDSNode *pNode,f3dPoint dim_pos,
											   BOOL bDimByAnalysisMode,BOOL bDrawRefLine)
{
	if(pNode==NULL)
		return NULL;
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)||defined(__TSA_FILE_)
	if(bDimByAnalysisMode)
	{
		CDimPartSpec *pNewPartSpec=dimpart.append();
		pNewPartSpec->cls_typeid=CLS_NODE;
		pNewPartSpec->hPart=pNode->handle;
		pNewPartSpec->base_pos=dim_pos;
		pNewPartSpec->iNo=pNode->point_i;
		sprintf(pNewPartSpec->guige,"%d",pNode->point_i);
		pNewPartSpec->bDrawRefLine=bDrawRefLine;
		return pNewPartSpec;
	}
	else
#endif
		return NULL;
}

CDimPartSpec *CDimPartSpecList::AppendLinePartSpec(CLDSLinePart *pLinePart,f3dPoint dim_pos,BOOL bDimByAnalysisMode,
												   BOOL bDrawRefLine,double angle)
{
	if(pLinePart==NULL)
		return NULL;
	CSuperSmartPtr<CLDSLinePart>pDimPart=pLinePart;	//����������ת������
	CDimPartSpec *pNewPartSpec=NULL;
	char guige[50]="";
	if(bDimByAnalysisMode)
	{
		CXhChar16 sPara1,sPara2;
		sPara1.Printf("%f",pLinePart->size_wide);
		sPara2.Printf("%f",pLinePart->size_thick);
		SimplifiedNumString(sPara1);
		SimplifiedNumString(sPara2);
		//����������ʱ�������Ϊ'S',Ĭ�ϲ���'S'��ӵ�����ַ����� wht 10-10-20
		if(pLinePart->size_idClassType==CLS_LINEANGLE)
			sprintf(guige,"L%sX%s%C",&sPara1,&sPara2,pLinePart->cMaterial);
		else if(pLinePart->size_idClassType==CLS_GROUPLINEANGLE)
			sprintf(guige,"%XL%sX%s%C",pLinePart->size_cSubClassType,&sPara1,&sPara2,pLinePart->cMaterial);
		else if(pLinePart->size_idClassType==CLS_LINETUBE)
			sprintf(guige,"��%sX%s%C",&sPara1,&sPara2,pLinePart->cMaterial);
		else if(pLinePart->size_idClassType==CLS_LINEFLAT)
			sprintf(guige,"-%sX%s%C",&sPara1,&sPara2,pLinePart->cMaterial);
		else
			strcpy(guige," ");
		if(pLinePart->cMaterial=='S')
			guige[strlen(guige)-1]='\0';
		pNewPartSpec=dimpart.append();
	}
#ifndef __TSA_FILE_
	else
	{
		for(CDimPartSpec *pPartSpec=GetFirst();pPartSpec;pPartSpec=GetNext())
		{
			if(stricmp(pPartSpec->sPartNo,pLinePart->GetPartNo())==0)
			{
				pNewPartSpec=dimpart.append();
				pNewPartSpec->bDimPartNoOnly=TRUE;
				if(m_bMergeNearSampePartNoDim&&DISTANCE(pNewPartSpec->base_pos,dim_pos)<30)
				{
					pNewPartSpec->m_bDimRefLineOnly=TRUE;
					pNewPartSpec->dim_pos=pPartSpec->dim_pos;
				}
				break;
			}
			else if(pPartSpec->hPart==pLinePart->handle)
				return pPartSpec;
		}
		if(pNewPartSpec==NULL)
		{
			pNewPartSpec=dimpart.append();
			pNewPartSpec->bDimPartNoOnly=FALSE;
		}
		//���
		char steelmark[20]="";
		if(pLinePart->cMaterial!='S')
			QuerySteelMatMark(pLinePart->cMaterial,steelmark);
		if(pDimPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			sprintf(guige,"%sL%.0fX%.0f",steelmark,
				pDimPart.LineAnglePointer()->GetWidth(),pDimPart.LineAnglePointer()->GetThick());
		}
		else if(pDimPart->GetClassTypeId()==CLS_LINETUBE)
		{
			sprintf(guige,"%s%%%%c%.0fX%.0f",steelmark,
				pDimPart.LineTubePointer()->GetDiameter(),pDimPart.LineTubePointer()->GetThick());
		}
		//���
		pNewPartSpec->sPartNo.Copy(pLinePart->GetPartNo());
		if(back_mir_style==0)	//X��Գ�
		{
			CLDSLinePart *pMirPart=(CLDSLinePart*)pDimPart->GetMirPart(MIRMSG(1));
			//ǰ�󹹼���ͬʱҲ��Ҫ��ע�Գƹ����ı�� wht 11-04-08 
			//ǰ�󹹼���ͬʱ����Ҫ�ԳƱ�ע(�������) wht 11-07-18
			if(pMirPart&&stricmp(pMirPart->GetPartNo(),pDimPart->GetPartNo())!=0)
			{
				strcpy(pNewPartSpec->sBackPartNo,pMirPart->GetPartNo());
				pNewPartSpec->hMirPart=pMirPart->handle;
			}
			else
				strcpy(pNewPartSpec->sBackPartNo,"");
		}
		else if(back_mir_style==1)	//Y��Գ�
		{
			CLDSLinePart *pMirPart=(CLDSLinePart*)pDimPart->GetMirPart(MIRMSG(2));
			//ǰ�󹹼���ͬʱҲ��Ҫ��ע�Գƹ����ı�� wht 11-04-08
			if(pMirPart&&stricmp(pMirPart->GetPartNo(),pDimPart->GetPartNo())!=0)
			{
				strcpy(pNewPartSpec->sBackPartNo,pMirPart->GetPartNo());
				pNewPartSpec->hMirPart=pMirPart->handle;
			}
			else
				strcpy(pNewPartSpec->sBackPartNo,"");
		}
		else
			strcpy(pNewPartSpec->sBackPartNo,"");
		//��������
		if(pLinePart->huoqu_handle>0x20)
		{
			CSuperSmartPtr<CLDSPart> pPart;
			double length=0;
			for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
			{
				if(pPart->IsLinePart()&&pPart.LinePartPointer()->huoqu_handle==pLinePart->huoqu_handle)
					length+=pPart.LinePartPointer()->GetLength();
			}
			sprintf(pNewPartSpec->sLength,"%.0f",length);
		}
		else
			sprintf(pNewPartSpec->sLength,"%.0f",pDimPart->GetLength());
		/*else
		{
			if(pAngle->sub_type&TA_JG_HUOQUSEG&&pAngle->huoqu_handle>0x20)
			{
				CLDSPolyAngle *pHuoQuJg=(CLDSPolyAngle*)Ta.FromPartHandle(pAngle->huoqu_handle,CLS_POLYANGLE);
				sprintf(pNewPartSpec->sLength,"%.0f",pHuoQuJg->GetLength());
			}
		}*/
	}
#endif
	strcpy(pNewPartSpec->guige,guige);
	pNewPartSpec->text_angle = angle;
	pNewPartSpec->hPart=pLinePart->handle;
	pNewPartSpec->cls_typeid=pLinePart->GetClassTypeId();
	pNewPartSpec->base_pos=dim_pos;
	pNewPartSpec->bDrawRefLine=bDrawRefLine;
	return pNewPartSpec;
}

#ifndef __TSA_FILE_
CDimPartSpec *CDimPartSpecList::AppendPlatePartSpec(CLDSPlate *pPlate,f3dPoint dim_pos,BOOL bDrawRefLine)
{
	CDimPartSpec *pNewPartSpec=NULL;
	for(CDimPartSpec *pPartSpec=GetFirst();pPartSpec;pPartSpec=GetNext())
	{
		if(stricmp(pPartSpec->sPartNo,pPlate->GetPartNo())==0)
		{
			pNewPartSpec=dimpart.append();
			pNewPartSpec->bDimPartNoOnly=TRUE;
			if(m_bMergeNearSampePartNoDim&&DISTANCE(pNewPartSpec->base_pos,dim_pos)<30)
			{
				pNewPartSpec->m_bDimRefLineOnly=TRUE;
				pNewPartSpec->dim_pos=pPartSpec->dim_pos;
			}
			break;
		}
		else if(pPartSpec->hPart==pPlate->handle)
			return pPartSpec;
	}
	if(pNewPartSpec==NULL)
	{
		pNewPartSpec=dimpart.append();
		pNewPartSpec->bDimPartNoOnly=FALSE;
	}
	pNewPartSpec->sPartNo.Copy(pPlate->GetPartNo());
	pNewPartSpec->hPart=pPlate->handle;
	pNewPartSpec->cls_typeid=CLS_PLATE;
	char steelmark[20]="";
	if(pPlate->cMaterial!='S')
		QuerySteelMatMark(pPlate->cMaterial,steelmark);
	sprintf(pNewPartSpec->guige,"%s-%.0f",steelmark,pPlate->GetThick());
	pNewPartSpec->base_pos=dim_pos;
	//�ԳƱ�ע�ְ��� wht 11-07-18
	if(back_mir_style==0)	//X��Գ�
	{	//��ȡ�ԳƸְ�ʱ���ñ�֤����һ�� wht 11-07-22
		CLDSPart *pMirPart=pPlate->GetMirPart(MIRMSG(1),NULL,FALSE);
		if(pMirPart&&stricmp(pMirPart->GetPartNo(),pPlate->GetPartNo())!=0)
		{
			strcpy(pNewPartSpec->sBackPartNo,pMirPart->GetPartNo());
			pNewPartSpec->hMirPart=pMirPart->handle;
		}
		else
			strcpy(pNewPartSpec->sBackPartNo,"");
	}
	else if(back_mir_style==1)	//Y��Գ�
	{
		CLDSPart *pMirPart=pPlate->GetMirPart(MIRMSG(2),NULL,FALSE);
		if(pMirPart&&stricmp(pMirPart->GetPartNo(),pPlate->GetPartNo())!=0)
		{
			strcpy(pNewPartSpec->sBackPartNo,pMirPart->GetPartNo());
			pNewPartSpec->hMirPart=pMirPart->handle;
		}
		else
			strcpy(pNewPartSpec->sBackPartNo,"");
	}
	else
		strcpy(pNewPartSpec->sBackPartNo,"");
	pNewPartSpec->bDrawRefLine=bDrawRefLine;
	return pNewPartSpec;
}

CDimPartSpec *CDimPartSpecList::AppendParamPlatePartSpec(CLDSParamPlate *pPlate,f3dPoint dim_pos,BOOL bDrawRefLine)
{
	CDimPartSpec *pNewPartSpec=NULL;
	for(CDimPartSpec *pPartSpec=GetFirst();pPartSpec;pPartSpec=GetNext())
	{
		if(stricmp(pPartSpec->sPartNo,pPlate->GetPartNo())==0)
		{
			pNewPartSpec=dimpart.append();
			pNewPartSpec->bDimPartNoOnly=TRUE;
			if(m_bMergeNearSampePartNoDim&&DISTANCE(pNewPartSpec->base_pos,dim_pos)<30)
			{
				pNewPartSpec->m_bDimRefLineOnly=TRUE;
				pNewPartSpec->dim_pos=pPartSpec->dim_pos;
			}
			break;
		}
		else if(pPartSpec->hPart==pPlate->handle)
			return pPartSpec;
	}
	if(pNewPartSpec==NULL)
	{
		pNewPartSpec=dimpart.append();
		pNewPartSpec->bDimPartNoOnly=FALSE;
	}
	//��������׼����岻��Ҫ��ע��񼰺�� wht 12-03-22
	if(pPlate->IsFL()||(pPlate->m_bStdPart&&pPlate->IsInsertPlate()))
		pNewPartSpec->bDimPartNoOnly=TRUE;
	strcpy(pNewPartSpec->sPartNo,pPlate->GetPartNo());
	pNewPartSpec->hPart=pPlate->handle;
	pNewPartSpec->cls_typeid=CLS_PARAMPLATE;
	char steelmark[20]="";
	if(pPlate->cMaterial!='S')
		QuerySteelMatMark(pPlate->cMaterial,steelmark);
	sprintf(pNewPartSpec->guige,"%s-%d",steelmark,pPlate->thick);
	pNewPartSpec->base_pos=dim_pos;
	//�ԳƱ�ע�ְ��� wht 11-07-18
	if(back_mir_style==0)	//X��Գ�
	{
		CLDSPart *pMirPart=pPlate->GetMirPart(MIRMSG(1));
		if(pMirPart&&stricmp(pMirPart->GetPartNo(),pPlate->GetPartNo())!=0)
		{
			strcpy(pNewPartSpec->sBackPartNo,pMirPart->GetPartNo());
			pNewPartSpec->hMirPart=pMirPart->handle;
		}
		else
			strcpy(pNewPartSpec->sBackPartNo,"");
	}
	else if(back_mir_style==1)	//Y��Գ�
	{
		CLDSPart *pMirPart=pPlate->GetMirPart(MIRMSG(2));
		if(pMirPart&&stricmp(pMirPart->GetPartNo(),pPlate->GetPartNo())!=0)
		{
			strcpy(pNewPartSpec->sBackPartNo,pMirPart->GetPartNo());
			pNewPartSpec->hMirPart=pMirPart->handle;
		}
		else
			strcpy(pNewPartSpec->sBackPartNo,"");
	}
	else
		strcpy(pNewPartSpec->sBackPartNo,"");
	pNewPartSpec->bDrawRefLine=bDrawRefLine;
	return pNewPartSpec;
}
#endif

BOOL CDimPartSpecList::DelPartSpecDim(long handle)
{
	for(CDimPartSpec *pPartSpec=GetFirst();pPartSpec;pPartSpec=GetNext())
	{
		if(pPartSpec->hPart==handle)
		{
			dimpart.DeleteCursor();
			return TRUE;
		}
	}
	return FALSE;
}

void CDimPartSpecList::EmptyPartSpecDim()
{
	dimpart.Empty();
}

CDimPartSpec* CDimPartSpecList::GetFirst()
{
	return dimpart.GetFirst();
}

CDimPartSpec* CDimPartSpecList::GetNext()
{
	return dimpart.GetNext();
}

void CDimPartSpecList::PushStack()
{
	dimpart.push_stack();
}

void CDimPartSpecList::PopStack()
{
	dimpart.pop_stack();
}

//����ָ���Ļ�׼�㡢�Ƕȡ����ȡ������ʼ�˻��ն˵õ���������
static void GetRegion(f3dPoint *rgn_vert,AcDb::TextHorzMode &horzMode,f3dPoint base_pos,
					  double angle,double len,double height,BOOL bStart)
{
	const double margin=0.3;
	if(bStart)
	{	//ʼ��
		if(angle>Pi/2.0&&angle<=1.5*Pi)
		{
			rgn_vert[0]=GetPtInPolar(base_pos,angle+Pi/2,height/2+margin);
			rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,len);
			rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle-Pi/2,height+2*margin);
			rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle-Pi/2,height+2*margin);
			horzMode=AcDb::kTextRight;
		}
		else
		{
			rgn_vert[0]=GetPtInPolar(base_pos,angle-Pi/2,height/2+margin);
			rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,len);
			rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle+Pi/2,height+2*margin);
			rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle+Pi/2,height+2*margin);
			horzMode=AcDb::kTextLeft;
		}
	}
	else
	{	//�ն�
		if(angle>Pi/2.0&&angle<=1.5*Pi)
		{
			rgn_vert[0]=GetPtInPolar(base_pos,angle-Pi/2,height/2+margin);
			rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,-len);
			rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle+Pi/2,height+2*margin);
			rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle+Pi/2,height+2*margin);
			horzMode=AcDb::kTextLeft;
		}
		else
		{
			rgn_vert[0]=GetPtInPolar(base_pos,angle+Pi/2,height/2+margin);
			rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,-len);
			rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle-Pi/2,height+2*margin);
			rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle-Pi/2,height+2*margin);
			horzMode=AcDb::kTextRight;
		}
	}
}

#ifdef __SMART_DRAW_INC_
void CDimPartSpecList::AutoDimPartSpec(AcDbBlockTableRecord *pBlockTableRecord,CDimPartSpec *pPartSpec,
									   BOOL bDimByAnalysisMode,BOOL bSelectPos/*=FALSE*/,bool bDimStartPartSpec)
{
	if(pPartSpec==NULL)
		return;
	DRAGSET.ClearEntSet();
	f3dPoint f_pos;
	AcGePoint3d acad_pos;
	CXhChar16 dimText,dimText2;
	f3dPoint rgn_vert[4];
	//
	double angle=0;	//���ڼ�¼���ֱ�ע��ת����
	dimText.Copy(pPartSpec->sPartNo);	//
	f3dPoint refpoint=pPartSpec->base_pos;
	refpoint.x+=pPartSpec->fBasePosOffsetX;	
	refpoint.y+=pPartSpec->fBasePosOffsetY;
	f3dLine refline;
	refline.startPt=refpoint;
	CLDSDbObject *pDbObj=(CLDSDbObject*)Ta.FromHandle(pPartSpec->hPart,TRUE);
	if(pDbObj==NULL)
		return;
	if(pPartSpec->m_bDimRefLineOnly)
	{	//����������ָ���ѱ�ע�Ĺ�����ż���
		refline.endPt=pPartSpec->dim_pos;
		CreateAcadLine(pBlockTableRecord,refline.startPt,refline.endPt);
		return;
	}
	BOOL bIsLinePart=FALSE;
	if(pDbObj->IsPart()&&((CLDSPart*)pDbObj)->IsLinePart())
		bIsLinePart=TRUE;
	if(pDbObj->IsNode()||bIsLinePart)	//�˼����ע
	{
		double height=0,length=0,radius=0;
		angle=pPartSpec->text_angle;
		height=sys.dim_map.PartNo.fPartNoTextSize;
		int nRetCode=0;
		if(bDimByAnalysisMode)
		{	//��עTSA�˼����߽ڵ���Ϣ
			AcDbObjectId layerId=LayerTable::DimTextLayer.layerId;
			if(bIsLinePart)
			{
				char guige[50];
				CLDSLinePart *pPole=(CLDSLinePart*)pDbObj;
				height=sys.dim_map.fPartGuigeTextSize;
				int i=0;
				for(SPEC_MARK *pMark=SpecList.GetFirst();pMark;pMark=SpecList.GetNext())
				{
					if(stricmp(pPartSpec->guige,pMark->spec)==0)
					{
						sprintf(pPartSpec->guige,"%C",pMark->cMark);
						break;
					}
					/*�˴������жϼ򻯹����,���ɹ���б�ʱ�Ѵ��� wht 11-06-14
					i++;
					if(i>=sys.nBriefGuigeN)
						break;
					*/
				}
				strcpy(guige,pPartSpec->guige);
				if(pPole->size_idClassType==CLS_LINETUBE)
				{
					CXhChar50 connStartInfo,connEndInfo;
					if(pPole->connectStart.m_iConnectType==0)
						connStartInfo.Copy(pPole->connectStart.flNo);
					//��ʱ�����������˨ʱӦ�����˨��
					//else
					//	connStartInfo.Printf("M%dX%d",pPole->connectStart.d,pPole->connectStart.N);
					if(pPole->connectEnd.m_iConnectType==0)
						connEndInfo.Copy(pPole->connectEnd.flNo);
					//else
					//	connEndInfo.Printf("M%dX%d",pPole->connectEnd.d,pPole->connectEnd.N);
					if(connStartInfo.GetLength()>0&&connStartInfo.Equal(connEndInfo))
						sprintf(pPartSpec->guige,"%s %s",&connStartInfo,guige);
					/*if(pPartSpec->bReverseLsInfo)
						sprintf(pPartSpec->guige,"%s~%s%s",&connEndInfo,&connStartInfo,guige);
					else
						sprintf(pPartSpec->guige,"%s~%s%s",&connStartInfo,&connEndInfo,guige);*/
				}
				else
				{	//ʼ�ն���˨���������˨ֱ�����ʱ����˨��Ϣ��˼����һ���ע wht 12-06-01
					if(pPole->connectStart.wnSummBoltN>0&&pPole->connectStart.wnSummBoltN==pPole->connectEnd.wnSummBoltN&&pPole->connectStart.d==pPole->connectEnd.d)
						sprintf(pPartSpec->guige,"M%dx%d %s",pPole->connectStart.d,pPole->connectStart.wnSummBoltN,guige);
					/*
					if(pPole->connectStart.N==1&&pPole->connectEnd.N==1)
						sprintf(pPartSpec->guige,"%s",guige);
					else if(pPartSpec->bReverseLsInfo)
						sprintf(pPartSpec->guige,"%d~%d%s",pPole->connectEnd.N,pPole->connectStart.N,guige);
					else
						sprintf(pPartSpec->guige,"%d~%d%s",pPole->connectStart.N,pPole->connectEnd.N,guige);
					*/
				}
			}
			else	//�ڵ��� �ڵ���Ҳʹ�ù�����������С wht 10-07-29
			{
				layerId=LayerTable::CommonLayer.layerId;
				height=sys.dim_map.PartNo.fPartNoTextSize;	//fOtherTextSize;	//fNodeNoTextSize;
			}
			length=strlen(pPartSpec->guige)*sys.dim_map.fPartGuigeTextSize*0.7+0.5;

			//����ƫ����������עλ��
			pPartSpec->base_pos.x+=pPartSpec->fDimPosOffsetX;
			pPartSpec->base_pos.y+=pPartSpec->fDimPosOffsetY;

			f3dPoint start,end;
			f3dPoint centre=pPartSpec->base_pos;
			centre=GetPtInPolar(centre,angle+Pi/2.0,height*0.5+0.5);
			//����������
			if(pPartSpec->bDrawRefLine)
			{
				refline.endPt=centre;
				f3dPoint vec=refline.endPt-refline.startPt;
				normalize(vec);
				refline.endPt=refline.endPt-vec*height;
				CreateAcadLine(pBlockTableRecord,refline.startPt,refline.endPt);
			}
			DimText(pBlockTableRecord,centre,pPartSpec->guige,TextStyleTable::hzfs.textStyleId,height,angle,AcDb::kTextCenter,AcDb::kTextVertMid,layerId);
		}
		else 
		{	//��עLTMA�˼���Ϣ
			double rMain=sys.dim_map.PartNo.fMainPoleNoOuterCirD*0.5;
			double rMainInner = sys.dim_map.PartNo.fMainPoleNoInnerCirD*0.5;
			double rVice=sys.dim_map.PartNo.fPartNoCirD*0.5;
			if(rMain<=0)
				rMain = 7;
			if(rVice<=0)
				rVice=4;
			if(rMainInner<=0)
				rMainInner = rMain-1;
			BOOL bIsMainPole=FALSE;	//�Ƿ�Ϊ����
			CLDSLinePart *pLinePart=(CLDSLinePart*)pDbObj;
			//�̽Ǹֲ�ʹ��˫Ȧ��ע wht 11-03-28
			if(toupper(pDbObj->layer(1))=='Z'&&pLinePart->pStart&&pLinePart->pEnd)	//����
			{
				length=radius=rMain;
				bIsMainPole=TRUE;
			}
			else
				length=radius=rVice;
			length+=strlen(pPartSpec->guige)*sys.dim_map.fPartGuigeTextSize*0.7+0.2;
			//����ƫ����������עλ��
			pPartSpec->base_pos.x+=pPartSpec->fDimPosOffsetX;
			pPartSpec->base_pos.y+=pPartSpec->fDimPosOffsetY;
			//
			f3dPoint centre=pPartSpec->base_pos;
			centre=GetPtInPolar(centre,angle+Pi/2.0,radius);
			CreateAcadCircle(pBlockTableRecord,centre,radius);
			//����������
			if(!bSelectPos&&pPartSpec->bDrawRefLine)
			{
				refline.endPt=centre;
				f3dPoint vec=refline.endPt-refline.startPt;
				normalize(vec);
				refline.endPt=refline.endPt-vec*radius;
				CreateAcadLine(pBlockTableRecord,refline.startPt,refline.endPt);
			}
			//�̽Ǹֲ�ʹ��˫Ȧ��ע wht 11-03-28
			if(toupper(pDbObj->layer(1))=='Z'&&pLinePart->pStart&&pLinePart->pEnd)	//���ı��˫ԲȦ
				CreateAcadCircle(pBlockTableRecord,centre,rMainInner);
			f3dPoint start,end;
			if(strlen(pPartSpec->sBackPartNo)>0)
			{
				double fPartNoTextSize=sys.dim_map.PartNo.fPartNoTextSize;
				if(!bIsMainPole)	//�ԳƱ�ע�����Ĺ������ʱ,Ϊ��Ӧ���Ȧ��С,Ĭ�ϵ�����ע�ָ�Ϊ2mm�������ָ��еĽ�Сֵ wht 11-08-13
				{
					fPartNoTextSize=min(2,sys.dim_map.PartNo.fPartNoTextSize);
					start.Set(centre.x-radius,centre.y);
					end.Set(centre.x+radius,centre.y);
				}
				else 
				{
					start.Set(centre.x-rMainInner,centre.y);
					end.Set(centre.x+rMainInner,centre.y);
				}
				CreateAcadLine(pBlockTableRecord,start,end);
				if(pPartSpec->iMirMsg==1)
				{
					if(pPartSpec->bLeftPart)
#ifdef AFX_TARG_ENU_ENGLISH
						dimText.Printf("left %s",(char*)pPartSpec->sPartNo);
					else 
						dimText.Printf("right %s",(char*)pPartSpec->sPartNo);
				}
				else 
					dimText.Printf("front %s",(char*)pPartSpec->sPartNo);
#else
						dimText.Printf("��%s",(char*)pPartSpec->sPartNo);
					else 
						dimText.Printf("��%s",(char*)pPartSpec->sPartNo);
				}
				else 
					dimText.Printf("ǰ%s",(char*)pPartSpec->sPartNo);
#endif
				f_pos.Set(centre.x,centre.y+0.5);
				DimText(pBlockTableRecord,f_pos,dimText,TextStyleTable::hzfs.textStyleId,
						fPartNoTextSize,0,AcDb::kTextCenter,AcDb::kTextBottom);
				if(pPartSpec->iMirMsg==1)
				{
					if(pPartSpec->bLeftPart)
#ifdef AFX_TARG_ENU_ENGLISH
						dimText.Printf("right %s",(char*)pPartSpec->sBackPartNo);
					else 
						dimText.Printf("light %s",(char*)pPartSpec->sBackPartNo);
				}
				else 
					dimText.Printf("behind %s",(char*)pPartSpec->sBackPartNo);
#else
						dimText.Printf("��%s",(char*)pPartSpec->sBackPartNo);
					else 
						dimText.Printf("��%s",(char*)pPartSpec->sBackPartNo);
				}
				else 
					dimText.Printf("��%s",(char*)pPartSpec->sBackPartNo);
#endif
				f_pos.Set(centre.x,centre.y-0.5);
				DimText(pBlockTableRecord,f_pos,dimText,TextStyleTable::hzfs.textStyleId,
						fPartNoTextSize,0,AcDb::kTextCenter,AcDb::kTextTop);
				if(!pPartSpec->bDimPartNoOnly)
				{
					start=GetPtInPolar(centre,angle,radius+0.2);
					end = GetPtInPolar(start,angle,length-radius);
					if(pPartSpec->bDimJgLength)	//��Ҫ��ע�Ǹֳ���
						CreateAcadLine(pBlockTableRecord,start,end);
					f3dPoint mid_pos=(start+end)*0.5;
					f_pos=GetPtInPolar(mid_pos,angle+Pi/2,0.5);
					if(pPartSpec->bDimJgLength)
					{		//��Ҫ��ע�Ǹֳ���
						DimText(pBlockTableRecord,f_pos,pPartSpec->guige,TextStyleTable::hzfs.textStyleId,
								sys.dim_map.fPartGuigeTextSize,angle,AcDb::kTextCenter,AcDb::kTextBottom);
						f_pos=GetPtInPolar(mid_pos,angle-Pi/2,0.5);
						DimText(pBlockTableRecord,f_pos,pPartSpec->sLength,TextStyleTable::hzfs.textStyleId,
								sys.dim_map.fPartGuigeTextSize,angle,AcDb::kTextCenter,AcDb::kTextTop);
					}
					else	//����Ҫ��ע�Ǹֳ���
						DimText(pBlockTableRecord,f_pos,pPartSpec->guige,TextStyleTable::hzfs.textStyleId,
								sys.dim_map.fPartGuigeTextSize,angle,AcDb::kTextCenter,AcDb::kTextVertMid);
				}
			}
			else
			{
				DimText(pBlockTableRecord,centre,pPartSpec->sPartNo,TextStyleTable::hzfs.textStyleId,
						sys.dim_map.PartNo.fPartNoTextSize,0,AcDb::kTextCenter,AcDb::kTextVertMid);
				if(!pPartSpec->bDimPartNoOnly)
				{
					start=GetPtInPolar(centre,angle,radius+0.2);
					end = GetPtInPolar(start,angle,length-radius);
					if(pPartSpec->bDimJgLength)	//��Ҫ��ע�Ǹֳ���
						CreateAcadLine(pBlockTableRecord,start,end);
					f3dPoint mid_pos=(start+end)*0.5;
					f_pos=GetPtInPolar(mid_pos,angle+Pi/2,0.5);
					if(pPartSpec->bDimJgLength)
					{		//��Ҫ��ע�Ǹֳ���
						DimText(pBlockTableRecord,f_pos,pPartSpec->guige,TextStyleTable::hzfs.textStyleId,
								sys.dim_map.fPartGuigeTextSize,angle,AcDb::kTextCenter,AcDb::kTextBottom);
						f_pos=GetPtInPolar(mid_pos,angle-Pi/2,0.5);
						DimText(pBlockTableRecord,f_pos,pPartSpec->sLength,TextStyleTable::hzfs.textStyleId,
								sys.dim_map.fPartGuigeTextSize,angle,AcDb::kTextCenter,AcDb::kTextTop);
					}
					else	//����Ҫ��ע�Ǹֳ���
						DimText(pBlockTableRecord,f_pos,pPartSpec->guige,TextStyleTable::hzfs.textStyleId,
								sys.dim_map.fPartGuigeTextSize,angle,AcDb::kTextCenter,AcDb::kTextVertMid);
				}
			}
		}
		if(bSelectPos)
		{
			f3dPoint box_vertex[4];
			ads_point L_T,R_B;
			SCOPE_STRU scope;
			box_vertex[0]=GetPtInPolar(pPartSpec->base_pos,angle+Pi,length+20);
			box_vertex[1]=GetPtInPolar(pPartSpec->base_pos,angle,length+20);
			box_vertex[2]=GetPtInPolar(rgn_vert[1],angle+Pi/2,height+20);
			box_vertex[3]=GetPtInPolar(rgn_vert[0],angle+Pi/2,height+20);
			for(int jj=0;jj<4;jj++)
				scope.VerifyVertex(box_vertex[jj]);
			L_T[X]=scope.fMinX;
			L_T[Y]=scope.fMaxY;
			L_T[Z]=0;
			R_B[X]=scope.fMaxX;
			R_B[Y]=scope.fMinY;
			R_B[Z]=0;
		#ifdef _ARX_2007
			acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,RTNONE);
		#else
			acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,RTNONE);
		#endif
#ifdef AFX_TARG_ENU_ENGLISH
			DragEntSet(pPartSpec->base_pos,"\nPlease select a proper mark position:\n");
#else
			DragEntSet(pPartSpec->base_pos,"\n��ѡ��һ�����ʵı�עλ��:\n");
#endif
			//����ָ�ԭ�е���ͼ״̬����ȻacedSSGet��������ʧ��
		#ifdef _ARX_2007
			acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
		#else
			acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
		#endif
			if(pPartSpec->bDrawRefLine)
			{	//ѡȡ����λ�ú�ҲӦ������������ wht 10-12-05
				f3dPoint centre=pPartSpec->base_pos;
				centre=GetPtInPolar(centre,angle+Pi/2.0,radius);
				refline.endPt=centre;
				f3dPoint vec=refline.endPt-refline.startPt;
				normalize(vec);
				refline.endPt=refline.endPt-vec*radius;
				CreateAcadLine(pBlockTableRecord,refline.startPt,refline.endPt);
			}
			//��¼���λ������ڳ�ʼλ�õ�ƫ���� wht 10-12-04
			pPartSpec->fDimPosOffsetX=pPartSpec->base_pos.x-refpoint.x;
			pPartSpec->fDimPosOffsetY=pPartSpec->base_pos.y-refpoint.y;
		}
	}
#ifndef __TSA_FILE_
	else if((pPartSpec->cls_typeid==CLS_PLATE||pPartSpec->cls_typeid==CLS_PARAMPLATE))	//�����ע
	{	//��ע�ְ���Ϣ
		double radius = sys.dim_map.PartNo.fPartNoCirD*0.5;//�û�ָ�����Ȧ�뾶  wht 10-01-10
		if(radius<=0)
			radius=max(sys.dim_map.PartNo.fPartNoTextSize,sys.dim_map.fPartGuigeTextSize*strlen(dimText)*0.7*0.5);
		CTextFrame frameText;
		frameText.InitTextFrame(sys.dim_map.PartNo.fPartNoTextSize,radius*2);
		//������ע�ı��ı߿�
		CLDSLineTube *pDatumTube=NULL;
		int iPartNoFrameStyle=sys.dim_map.PartNo.iPartNoFrameStyle;
		if(pDbObj->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate *pPlate=(CLDSParamPlate*)pDbObj;
			if(pPlate->m_bStdPart&&pPlate->IsInsertPlate())
				iPartNoFrameStyle=sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle;	//��׼��嵥��ָ����ſ����� wht 12-03-19
			else if (pPlate->IsFL())
				iPartNoFrameStyle=sys.dim_map.PartNo.iFlPartNoFrameStyle;				//��������ָ����ſ����� wht 12-03-19
			//���㷨�����׼��常�˼� wht 12-03-22
			if(pPlate->IsFL()||(pPlate->m_bStdPart&&pPlate->IsInsertPlate()))
				pDatumTube=(CLDSLineTube*)Ta.FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
		}
		frameText.BuildFrame(dimText,dimText2,0,iPartNoFrameStyle);
		//����ƫ����������עλ��
		pPartSpec->base_pos.x+=pPartSpec->fDimPosOffsetX;
		pPartSpec->base_pos.y+=pPartSpec->fDimPosOffsetY;
		if(pDatumTube&&fabs(pPartSpec->rod_angle)>0)
		{	//��ָ�����ű�ע�Ƕ� wht 12-03-22
			AcDb::TextHorzMode horzMode;
			double len=frameText.Width(),height=frameText.Height();
			GetRegion(rgn_vert,horzMode,pPartSpec->base_pos,pPartSpec->rod_angle,len,height,bDimStartPartSpec);
			if(iPartNoFrameStyle==1||iPartNoFrameStyle==2)
				frameText.textAlignHori=(ACDB::TextHorzMode)horzMode;
		}
		else 
		{
			rgn_vert[0]=pPartSpec->base_pos+f3dPoint(-0.5*frameText.Width(),-0.5*frameText.Height());
			rgn_vert[1]=pPartSpec->base_pos+f3dPoint( 0.5*frameText.Width(),-0.5*frameText.Height());
			rgn_vert[2]=pPartSpec->base_pos+f3dPoint( 0.5*frameText.Width(), 0.5*frameText.Height());
			rgn_vert[3]=pPartSpec->base_pos+f3dPoint(-0.5*frameText.Width(), 0.5*frameText.Height());
		}
		//����������
		if(!bSelectPos&&pPartSpec->bDrawRefLine)
		{
			refline.endPt=pPartSpec->base_pos;
			f3dPoint vec=refline.endPt-refline.startPt;
			normalize(vec);
			refline.endPt=refline.endPt-vec*radius;
			CreateAcadLine(pBlockTableRecord,refline.startPt,refline.endPt);
		}
		if(bSelectPos)
		{
			ads_point L_T,R_B;
			L_T[X]=rgn_vert[3].x-20;
			L_T[Y]=rgn_vert[3].y+20;
			L_T[Z]=0;
			R_B[X]=rgn_vert[1].x+20;
			R_B[Y]=rgn_vert[1].y-20;
			R_B[Z]=0;
		#ifdef _ARX_2007
			acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
		#else
			acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
		#endif
			DRAGSET.ClearEntSet();
			DimPartSpec(pBlockTableRecord,pPartSpec,&frameText);
#ifdef AFX_TARG_ENU_ENGLISH
			DragEntSet(pPartSpec->base_pos,"\nPlease select a proper mark position:\n");
#else
			DragEntSet(pPartSpec->base_pos,"\n��ѡ��һ�����ʵı�עλ��:\n");
#endif
			//����ָ�ԭ�е���ͼ״̬����ȻacedSSGet��������ʧ��
		#ifdef _ARX_2007
			acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
		#else
			acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
		#endif
			if(pPartSpec->bDrawRefLine)
			{	//ѡȡ����λ�ú�ҲӦ������������ wht 10-12-05
				refline.endPt=pPartSpec->base_pos;
				f3dPoint vec=refline.endPt-refline.startPt;
				normalize(vec);
				refline.endPt=refline.endPt-vec*radius;
				CreateAcadLine(pBlockTableRecord,refline.startPt,refline.endPt);
			}
			//��¼���λ������ڳ�ʼλ�õ�ƫ���� wht 10-12-04
			pPartSpec->fDimPosOffsetX=pPartSpec->base_pos.x-refpoint.x;
			pPartSpec->fDimPosOffsetY=pPartSpec->base_pos.y-refpoint.y;
		}
		else
			DimPartSpec(pBlockTableRecord,pPartSpec,&frameText);
		if(!pPartSpec->bDimPartNoOnly)
		{	//����ƫ������������עλ��(���ƫ������������ű�עλ�õ�ƫ����) wht 11-07-18
			f_pos.x=pPartSpec->base_pos.x+pPartSpec->fGuigeOffsetX;
			f_pos.y=pPartSpec->base_pos.y+pPartSpec->fGuigeOffsetY;
			if(bSelectPos)
			{
				DRAGSET.ClearEntSet();
				DimText(pBlockTableRecord,f_pos,pPartSpec->guige,TextStyleTable::hzfs.textStyleId,
					sys.dim_map.fPlateThickTextSize,0,AcDb::kTextCenter,AcDb::kTextVertMid);
				f3dPoint box_vertex[4];
				ads_point L_T,R_B;
				SCOPE_STRU scope;
				box_vertex[0].Set(f_pos.x-20,f_pos.y-40);
				box_vertex[1].Set(f_pos.x+20,f_pos.y-40);
				box_vertex[2].Set(f_pos.x+20,f_pos.y+40);
				box_vertex[3].Set(f_pos.x-20,f_pos.y+40);
				for(int jj=0;jj<4;jj++)
					scope.VerifyVertex(box_vertex[jj]);
				L_T[X]=scope.fMinX;
				L_T[Y]=scope.fMaxY;
				L_T[Z]=0;
				R_B[X]=scope.fMaxX;
				R_B[Y]=scope.fMinY;
				R_B[Z]=0;
			#ifdef _ARX_2007
				acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
			#else
				acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
			#endif
#ifdef AFX_TARG_ENU_ENGLISH
				DragEntSet(f_pos,"\nPlease select a proper mark position:\n");
#else
				DragEntSet(f_pos,"\n��ѡ��һ�����ʵı�עλ��:\n");
#endif
				//��¼�ְ���λ������ڱ��λ�õ�ƫ����,�����ƶ����ʱ���λ����֮�䶯 wht 11-07-18
				pPartSpec->fGuigeOffsetX=f_pos.x-pPartSpec->base_pos.x;
				pPartSpec->fGuigeOffsetY=f_pos.y-pPartSpec->base_pos.y;
				//����ָ�ԭ�е���ͼ״̬����ȻacedSSGet��������ʧ��
			#ifdef _ARX_2007
				acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
			#else
				acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
			#endif
			}
			else
				DimText(pBlockTableRecord,f_pos,pPartSpec->guige,TextStyleTable::hzfs.textStyleId,
				sys.dim_map.fPlateThickTextSize,0,AcDb::kTextCenter,AcDb::kTextVertMid);
		}
	}
#endif
	pPartSpec->cls_typeid*=-1;	//��ʾ�Ѿ���ע�Ĺ���
}
#endif

//
void CDimPartSpecList::AnalyseAndDimPartSpec(AcDbBlockTableRecord *pBlockTableRecord,CDimPartSpec *pPartSpec,
											 BOOL bDimByAnalysisMode,bool bDimStartPartSpec)
{
	if(pPartSpec==NULL)
		return;
	f3dPoint f_pos;
	AcGePoint3d acad_pos;
	CXhChar16 dimText,dimText2;
	f3dPoint rgn_vert[4];
	//
	double angle=0;	//���ڼ�¼���ֱ�ע��ת����
	dimText.Copy(pPartSpec->sPartNo);	//
	f3dPoint refpoint=pPartSpec->base_pos;
	f3dLine refline;
	refline.startPt=pPartSpec->base_pos;
	CLDSDbObject *pDbObj=(CLDSDbObject*)Ta.FromHandle(pPartSpec->hPart,TRUE);
	if(pDbObj==NULL)
		return;
	if(pPartSpec->m_bDimRefLineOnly)
	{	//����������ָ���ѱ�ע�Ĺ�����ż���
		refline.endPt=pPartSpec->dim_pos;
		CreateAcadLine(pBlockTableRecord,refline.startPt,refline.endPt);
		return;
	}
	BOOL bIsLinePart=FALSE;
	if(pDbObj->IsPart()&&((CLDSPart*)pDbObj)->IsLinePart())
		bIsLinePart=TRUE;
	if(pDbObj->IsNode()||bIsLinePart)	//�˼����ע
	{
		double height=0,length=0,radius=0;
		height=sys.dim_map.PartNo.fPartNoTextSize;
		angle=pPartSpec->text_angle;
		int nRetCode=0;
		DRAGSET.ClearEntSet();
		if(bDimByAnalysisMode)
		{	//��עTSA�˼����߽ڵ���Ϣ
			AcDbObjectId layerId=LayerTable::DimTextLayer.layerId;
			if(bIsLinePart)
			{
				char guige[50];
				CLDSLinePart *pPole=(CLDSLinePart*)pDbObj;
				height=sys.dim_map.fPartGuigeTextSize;
				int i=0;
				for(SPEC_MARK *pMark=SpecList.GetFirst();pMark;pMark=SpecList.GetNext())
				{
					if(stricmp(pPartSpec->guige,pMark->spec)==0)
					{
						sprintf(pPartSpec->guige,"%C",pMark->cMark);
						break;
					}
					/*�˴������жϼ򻯹����,���ɹ���б�ʱ�Ѵ��� wht 11-06-14
					i++;
					if(i>=sys.nBriefGuigeN)
						break;
					*/
				}
				strcpy(guige,pPartSpec->guige);
				if(pPole->size_idClassType==CLS_LINETUBE)
				{
					CXhChar50 connStartInfo,connEndInfo;
					if(pPole->connectStart.m_iConnectType==0)
						connStartInfo.Copy(pPole->connectStart.flNo);
					//��ʱ�����������˨ʱӦ�����˨��
					//else
					//	connStartInfo.Printf("M%dX%d",pPole->connectStart.d,pPole->connectStart.N);
					if(pPole->connectEnd.m_iConnectType==0)
						connEndInfo.Copy(pPole->connectEnd.flNo);
					//else
					//	connEndInfo.Printf("M%dX%d",pPole->connectEnd.d,pPole->connectEnd.N);
					if(connStartInfo.GetLength()>0&&connStartInfo.Equal(connEndInfo))
						sprintf(pPartSpec->guige,"%s %s",(char*)connStartInfo,guige);
					/*if(pPartSpec->bReverseLsInfo)
						sprintf(pPartSpec->guige,"%s~%s%s",&connEndInfo,&connStartInfo,guige);
					else
						sprintf(pPartSpec->guige,"%s~%s%s",&connStartInfo,&connEndInfo,guige);*/
				}
				else
				{	//ʼ�ն���˨���������˨ֱ�����ʱ����˨��Ϣ��˼����һ���ע wht 12-06-01
					if(pPole->connectStart.wnSummBoltN>0&&pPole->connectStart.wnSummBoltN==pPole->connectEnd.wnSummBoltN&&pPole->connectStart.d==pPole->connectEnd.d)
					{
						if(strlen(guige)>1&&pPole->connectStart.wnSummBoltN==1)
							sprintf(pPartSpec->guige,"%sM%d",guige,pPole->connectStart.d);
						else if(strlen(guige)>1)
							sprintf(pPartSpec->guige,"%sM%dx%d",guige,pPole->connectStart.d,pPole->connectStart.wnSummBoltN);
					}
					/*if(pPole->connectStart.N==1&&pPole->connectEnd.N==1)
						sprintf(pPartSpec->guige,"%s",guige);
					else if(pPartSpec->bReverseLsInfo)
						sprintf(pPartSpec->guige,"%d~%d%s",pPole->connectEnd.N,pPole->connectStart.N,guige);
					else
						sprintf(pPartSpec->guige,"%d~%d%s",pPole->connectStart.N,pPole->connectEnd.N,guige);*/
				}
			}
			else	//�ڵ��� �ڵ���Ҳʹ�ù�����������С wht 10-07-29
			{
				layerId=LayerTable::CommonLayer.layerId;
				height=sys.dim_map.PartNo.fPartNoTextSize;	//fOtherTextSize;	//fNodeNoTextSize;
			}
			
			length=strlen(pPartSpec->guige)*sys.dim_map.fPartGuigeTextSize*0.7+0.5;
			rgn_vert[0]=GetPtInPolar(pPartSpec->base_pos,angle+Pi/2,0.5);
			rgn_vert[0]=GetPtInPolar(rgn_vert[0],angle,-length*0.5);
			rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,length);
			rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle+Pi/2,height);
			rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle+Pi/2,height);
			//���Ҳ���ײ�ı�עλ��
			nRetCode=SearchNoneIntersPos(&pPartSpec->base_pos,rgn_vert,NULL,NULL,3*height,1);
			//����ʱ������ʾ��ײ���Ҿ��ο�
			/*GetCurDwg()->setClayer(LayerTable::AuxLineLayer.layerId);
			CreateAcadLine(pBlockTableRecord,rgn_vert[0],rgn_vert[1]);//�±߿���
			GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
			CreateAcadLine(pBlockTableRecord,rgn_vert[2],rgn_vert[1]);//�ұ߿���
			GetCurDwg()->setClayer(LayerTable::BoltLineLayer.layerId);
			CreateAcadLine(pBlockTableRecord,rgn_vert[3],rgn_vert[2]);//�ϱ߿���
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			CreateAcadLine(pBlockTableRecord,rgn_vert[3],rgn_vert[0]);//��߿���*/
			//��¼���λ������ڳ�ʼλ�õ�ƫ���� wht 11-06-15
			pPartSpec->fDimPosOffsetX=pPartSpec->base_pos.x-refpoint.x;
			pPartSpec->fDimPosOffsetY=pPartSpec->base_pos.y-refpoint.y;

			f3dPoint start,end;
			f3dPoint centre=pPartSpec->base_pos;
			centre=GetPtInPolar(centre,angle+Pi/2.0,height*0.5+0.5);
			//����������
			if(pPartSpec->bDrawRefLine)
			{
				refline.endPt=centre;
				f3dPoint vec=refline.endPt-refline.startPt;
				normalize(vec);
				refline.endPt=refline.endPt-vec*height;
				CreateAcadLine(pBlockTableRecord,refline.startPt,refline.endPt);
			}
			DimText(pBlockTableRecord,centre,pPartSpec->guige,TextStyleTable::hzfs.textStyleId,
					height,angle,AcDb::kTextCenter,AcDb::kTextVertMid,layerId);
		}
		else 
		{	//��עLTMA�˼���Ϣ
			double rMain=sys.dim_map.PartNo.fMainPoleNoOuterCirD*0.5;
			double rMainInner = sys.dim_map.PartNo.fMainPoleNoInnerCirD*0.5;
			double rVice=sys.dim_map.PartNo.fPartNoCirD*0.5;
			if(rMain<=0)
				rMain = 7;
			if(rVice<=0)
				rVice=4;
			if(rMainInner<=0)
				rMainInner = rMain-1;
			BOOL bIsMainPole=FALSE;	//�Ƿ�Ϊ����
			CLDSLinePart *pLinePart=(CLDSLinePart*)pDbObj; 
			//�̽Ǹֲ�ʹ��˫Ȧ��ע wht 11-03-28
			if(toupper(pDbObj->layer(1))=='Z'&&pLinePart->pStart&&pLinePart->pEnd)	//����
			{
				length=radius=rMain;
				bIsMainPole=TRUE;
			}
			else
				length=radius=rVice;

			length+=strlen(pPartSpec->guige)*sys.dim_map.fPartGuigeTextSize*0.7+0.2;
			rgn_vert[0]=GetPtInPolar(pPartSpec->base_pos,angle+Pi,radius);
			rgn_vert[1]=GetPtInPolar(pPartSpec->base_pos,angle,length);
			rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle+Pi/2,radius*2);
			rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle+Pi/2,radius*2);
			//���Ҳ���ײ�ı�עλ��
			nRetCode=SearchNoneIntersPos(&pPartSpec->base_pos,rgn_vert,NULL,NULL,0,0,&pPartSpec->searchDirect);
			//��¼���λ������ڳ�ʼλ�õ�ƫ���� wht 10-12-04
			pPartSpec->fDimPosOffsetX=pPartSpec->base_pos.x-refpoint.x;
			pPartSpec->fDimPosOffsetY=pPartSpec->base_pos.y-refpoint.y;
			//
			f3dPoint centre=pPartSpec->base_pos;
			centre=GetPtInPolar(centre,angle+Pi/2.0,radius);
			CreateAcadCircle(pBlockTableRecord,centre,radius);
			//����������
			if(pPartSpec->bDrawRefLine)
			{
				refline.endPt=centre;
				f3dPoint vec=refline.endPt-refline.startPt;
				normalize(vec);
				refline.endPt=refline.endPt-vec*radius;
				CreateAcadLine(pBlockTableRecord,refline.startPt,refline.endPt);
			}
			//�̽Ǹֲ�ʹ��˫Ȧ��ע wht 11-03-28
			if(toupper(pDbObj->layer(1))=='Z'&&pLinePart->pStart&&pLinePart->pEnd)	//���ı��˫ԲȦ
				CreateAcadCircle(pBlockTableRecord,centre,rMainInner);
			f3dPoint start,end;
			if(strlen(pPartSpec->sBackPartNo)>0)
			{
				double fPartNoTextSize=sys.dim_map.PartNo.fPartNoTextSize;
				if(!bIsMainPole)	//�ԳƱ�ע�����Ĺ������ʱ,Ϊ��Ӧ���Ȧ��С,Ĭ�ϵ�����ע�ָ�Ϊ2mm�������ָ��еĽ�Сֵ wht 11-08-13
				{
					fPartNoTextSize=min(2,sys.dim_map.PartNo.fPartNoTextSize);
					start.Set(centre.x-radius,centre.y);	
					end.Set(centre.x+radius,centre.y);
				}
				else 
				{
					start.Set(centre.x-rMainInner,centre.y);	
					end.Set(centre.x+rMainInner,centre.y);
				}
				CreateAcadLine(pBlockTableRecord,start,end);
				if(pPartSpec->iMirMsg==1)
				{
					if(pPartSpec->bLeftPart)
#ifdef AFX_TARG_ENU_ENGLISH
						dimText.Printf("left %s",(char*)pPartSpec->sPartNo);
					else 
						dimText.Printf("right %s",(char*)pPartSpec->sPartNo);
				}
				else 
					dimText.Printf("front %s",(char*)pPartSpec->sPartNo);
#else
						dimText.Printf("��%s",(char*)pPartSpec->sPartNo);
					else 
						dimText.Printf("��%s",(char*)pPartSpec->sPartNo);
				}
				else 
					dimText.Printf("ǰ%s",(char*)pPartSpec->sPartNo);
#endif
				f_pos.Set(centre.x,centre.y+0.5);
				DimText(pBlockTableRecord,f_pos,dimText,TextStyleTable::hzfs.textStyleId,
						fPartNoTextSize,0,AcDb::kTextCenter,AcDb::kTextBottom);
				if(pPartSpec->iMirMsg==1)
				{
					if(pPartSpec->bLeftPart)
#ifdef AFX_TARG_ENU_ENGLISH
						dimText.Printf("right %s",(char*)pPartSpec->sBackPartNo);
					else 
						dimText.Printf("left %s",(char*)pPartSpec->sBackPartNo);
				}
				else 
					dimText.Printf("behind %s",(char*)pPartSpec->sBackPartNo);
#else
						dimText.Printf("��%s",(char*)pPartSpec->sBackPartNo);
					else 
						dimText.Printf("��%s",(char*)pPartSpec->sBackPartNo);
				}
				else 
					dimText.Printf("��%s",(char*)pPartSpec->sBackPartNo);
#endif
				f_pos.Set(centre.x,centre.y-0.5);
				DimText(pBlockTableRecord,f_pos,dimText,TextStyleTable::hzfs.textStyleId,
						fPartNoTextSize,0,AcDb::kTextCenter,AcDb::kTextTop);
				if(!pPartSpec->bDimPartNoOnly)
				{
					start=GetPtInPolar(centre,angle,radius+0.2);
					end = GetPtInPolar(start,angle,length-radius);
					if(sys.stru_map.bDimStruJgLen)	//��Ҫ��ע�Ǹֳ���
						CreateAcadLine(pBlockTableRecord,start,end);
					f3dPoint mid_pos=(start+end)*0.5;
					f_pos=GetPtInPolar(mid_pos,angle+Pi/2,0.5);
					if(sys.stru_map.bDimStruJgLen)
					{	//��Ҫ��ע�Ǹֳ���
						DimText(pBlockTableRecord,f_pos,pPartSpec->guige,TextStyleTable::hzfs.textStyleId,
								sys.dim_map.fPartGuigeTextSize,angle,AcDb::kTextCenter,AcDb::kTextBottom);
						f_pos=GetPtInPolar(mid_pos,angle-Pi/2,0.5);
						DimText(pBlockTableRecord,f_pos,pPartSpec->sLength,TextStyleTable::hzfs.textStyleId,
								sys.dim_map.fPartGuigeTextSize,angle,AcDb::kTextCenter,AcDb::kTextTop);
					}
					else//����Ҫ��ע�Ǹֳ���
						DimText(pBlockTableRecord,f_pos,pPartSpec->guige,TextStyleTable::hzfs.textStyleId,
								sys.dim_map.fPartGuigeTextSize,angle,AcDb::kTextCenter,AcDb::kTextVertMid);
				}
			}
			else
			{
				DimText(pBlockTableRecord,centre,pPartSpec->sPartNo,TextStyleTable::hzfs.textStyleId,
						sys.dim_map.PartNo.fPartNoTextSize,0,AcDb::kTextCenter,AcDb::kTextVertMid);
				if(!pPartSpec->bDimPartNoOnly)
				{
					start=GetPtInPolar(centre,angle,radius+0.2);
					end = GetPtInPolar(start,angle,length-radius);
					if(sys.stru_map.bDimStruJgLen)	//��Ҫ��ע�Ǹֳ���
						CreateAcadLine(pBlockTableRecord,start,end);
					f3dPoint mid_pos=(start+end)*0.5;
					f_pos=GetPtInPolar(mid_pos,angle+Pi/2,0.5);
					if(sys.stru_map.bDimStruJgLen)
					{		//��Ҫ��ע�Ǹֳ���
						DimText(pBlockTableRecord,f_pos,pPartSpec->guige,TextStyleTable::hzfs.textStyleId,
								sys.dim_map.fPartGuigeTextSize,angle,AcDb::kTextCenter,AcDb::kTextBottom);
						f_pos=GetPtInPolar(mid_pos,angle-Pi/2,0.5);
						DimText(pBlockTableRecord,f_pos,pPartSpec->sLength,TextStyleTable::hzfs.textStyleId,
								sys.dim_map.fPartGuigeTextSize,angle,AcDb::kTextCenter,AcDb::kTextTop);
					}
					else	//����Ҫ��ע�Ǹֳ���
						DimText(pBlockTableRecord,f_pos,pPartSpec->guige,TextStyleTable::hzfs.textStyleId,
								sys.dim_map.fPartGuigeTextSize,angle,AcDb::kTextCenter,AcDb::kTextVertMid);
				}
			}
		}
		//δ�ҵ�����ķ���λ�����û�ָ��
		/* ����˾��ͼ��������ʾ�û����ѡ������ײ��ע��λ�ã���ʵ��������(Ч�ʵͲ��׺��ڸ�)����˾��ͼ������һ���̶ȵ���ײ wjh-2013.12.15
		if(nRetCode!=0)
		{
			f3dPoint box_vertex[4];
			ads_point L_T,R_B;
			SCOPE_STRU scope;
			box_vertex[0]=GetPtInPolar(pPartSpec->base_pos,angle+Pi,length+20);
			box_vertex[1]=GetPtInPolar(pPartSpec->base_pos,angle,length+20);
			box_vertex[2]=GetPtInPolar(rgn_vert[1],angle+Pi/2,height+20);
			box_vertex[3]=GetPtInPolar(rgn_vert[0],angle+Pi/2,height+20);
			for(int jj=0;jj<4;jj++)
				scope.VerifyVertex(box_vertex[jj]);
			L_T[X]=scope.fMinX;
			L_T[Y]=scope.fMaxY;
			L_T[Z]=0;
			R_B[X]=scope.fMaxX;
			R_B[Y]=scope.fMinY;
			R_B[Z]=0;
			acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,RTNONE);
#ifdef AFX_TARG_ENU_ENGLISH
			DragEntSet(pPartSpec->base_pos,"\nPlease select a proper mark position:\n");
#else
			DragEntSet(pPartSpec->base_pos,"\n��ѡ��һ�����ʵı�עλ��:\n");
#endif
			//����ָ�ԭ�е���ͼ״̬����ȻacedSSGet��������ʧ��
			acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
			if(pPartSpec->bDrawRefLine)
			{	//ѡȡ����λ�ú�ҲӦ������������ wht 10-12-05
				f3dPoint centre=pPartSpec->base_pos;
				centre=GetPtInPolar(centre,angle+Pi/2.0,radius);
				refline.endPt=centre;
				f3dPoint vec=refline.endPt-refline.startPt;
				normalize(vec);
				refline.endPt=refline.endPt-vec*radius;
				CreateAcadLine(pBlockTableRecord,refline.startPt,refline.endPt);
			}
			//��¼���λ������ڳ�ʼλ�õ�ƫ���� wht 10-12-04
			pPartSpec->fDimPosOffsetX=pPartSpec->base_pos.x-refpoint.x;
			pPartSpec->fDimPosOffsetY=pPartSpec->base_pos.y-refpoint.y;
		}*/
	}
#ifndef __TSA_FILE_
	else if((pPartSpec->cls_typeid==CLS_PLATE||pPartSpec->cls_typeid==CLS_PARAMPLATE))	//�����ע
	{	//��ע�ְ���Ϣ
		double radius = sys.dim_map.PartNo.fPartNoCirD*0.5;//�û�ָ�����Ȧ�뾶  wht 10-01-10
		if(radius<=0)
			radius=max(sys.dim_map.PartNo.fPartNoTextSize,sys.dim_map.fPartGuigeTextSize*strlen(dimText)*0.7*0.5);
		CTextFrame frameText;
		frameText.InitTextFrame(sys.dim_map.PartNo.fPartNoTextSize,radius*2);
		//������ע�ı��ı߿�
		CLDSLineTube *pDatumTube=NULL;
		int iPartNoFrameStyle=sys.dim_map.PartNo.iPartNoFrameStyle;
		if(pDbObj->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate *pPlate=(CLDSParamPlate*)pDbObj;
			if(pPlate->m_bStdPart&&pPlate->IsInsertPlate())
				iPartNoFrameStyle=sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle;	//��׼��嵥��ָ����ſ����� wht 12-03-19
			else if (pPlate->IsFL())
				iPartNoFrameStyle=sys.dim_map.PartNo.iFlPartNoFrameStyle;				//��������ָ����ſ����� wht 12-03-19
			//���㷨�����׼��常�˼� wht 12-03-22
			if(pPlate->IsFL()||(pPlate->m_bStdPart&&pPlate->IsInsertPlate()))
				pDatumTube=(CLDSLineTube*)Ta.FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
		}
		frameText.BuildFrame(dimText,dimText2,0,iPartNoFrameStyle);
		
		int nRet=0;
		if(pDatumTube&&fabs(pPartSpec->rod_angle)>0)
		{	//��ָ�����ű�ע�Ƕ� wht 12-03-22
			AcDb::TextHorzMode horzMode;
			double len=frameText.Width(),height=frameText.Height();
			GetRegion(rgn_vert,horzMode,pPartSpec->base_pos,pPartSpec->rod_angle,len,height,bDimStartPartSpec);
			int quad_order[4]={1,4,0,0};
			nRet=SearchNoneIntersPos(&pPartSpec->base_pos,rgn_vert,NULL,quad_order);
			if(iPartNoFrameStyle==1||iPartNoFrameStyle==2)
				frameText.textAlignHori=(ACDB::TextHorzMode)horzMode;
		}
		else 
		{
			rgn_vert[0]=pPartSpec->base_pos+f3dPoint(-0.5*frameText.Width(),-0.5*frameText.Height());
			rgn_vert[1]=pPartSpec->base_pos+f3dPoint( 0.5*frameText.Width(),-0.5*frameText.Height());
			rgn_vert[2]=pPartSpec->base_pos+f3dPoint( 0.5*frameText.Width(), 0.5*frameText.Height());
			rgn_vert[3]=pPartSpec->base_pos+f3dPoint(-0.5*frameText.Width(), 0.5*frameText.Height());
			nRet=SearchNoneIntersPos(&pPartSpec->base_pos,rgn_vert,NULL,NULL);
		}
		if(nRet!=0)
		{	//δ�ҵ�����λ��
			ads_point L_T,R_B;
			L_T[X]=rgn_vert[3].x-20;
			L_T[Y]=rgn_vert[3].y+20;
			L_T[Z]=0;
			R_B[X]=rgn_vert[1].x+20;
			R_B[Y]=rgn_vert[1].y-20;
			R_B[Z]=0;
		#ifdef _ARX_2007
			acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
		#else
			acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
		#endif
			DRAGSET.ClearEntSet();
			DimPartSpec(pBlockTableRecord,pPartSpec,&frameText);
#ifdef AFX_TARG_ENU_ENGLISH
			DragEntSet(pPartSpec->base_pos,"\nPlease select a proper mark position:\n");
#else
			DragEntSet(pPartSpec->base_pos,"\n��ѡ��һ�����ʵı�עλ��:\n");
#endif
			//����ָ�ԭ�е���ͼ״̬����ȻacedSSGet��������ʧ��
		#ifdef _ARX_2007
			acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
		#else
			acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
		#endif
			if(pPartSpec->bDrawRefLine)
			{	//ѡȡ����λ�ú�ҲӦ������������ wht 10-12-05
				refline.endPt=pPartSpec->base_pos;
				f3dPoint vec=refline.endPt-refline.startPt;
				normalize(vec);
				if(pDatumTube==NULL||fabs(pPartSpec->rod_angle)<eps)
					refline.endPt=refline.endPt-vec*radius;
				CreateAcadLine(pBlockTableRecord,refline.startPt,refline.endPt);
			}
		}
		else
		{
			if(pPartSpec->bDrawRefLine)
			{	//������Ӧ���ڲ��ҵ�����λ�ú��ٽ��л��� wht 10-12-05
				refline.endPt=pPartSpec->base_pos;
				f3dPoint vec=refline.endPt-refline.startPt;
				normalize(vec);
				if(pDatumTube==NULL||fabs(pPartSpec->rod_angle)<eps)
					refline.endPt=refline.endPt-vec*radius;
				CreateAcadLine(pBlockTableRecord,refline.startPt,refline.endPt);
			}
			DimPartSpec(pBlockTableRecord,pPartSpec,&frameText);
		}
		//��¼���λ������ڳ�ʼλ�õ�ƫ���� wht 10-12-04
		pPartSpec->fDimPosOffsetX=pPartSpec->base_pos.x-refpoint.x;
		pPartSpec->fDimPosOffsetY=pPartSpec->base_pos.y-refpoint.y;
		if(!pPartSpec->bDimPartNoOnly)
		{	//��ע�ְ���
			double length=strlen(pPartSpec->guige)*sys.dim_map.fPlateThickTextSize*0.7;
			f_pos=pPartSpec->base_pos;
			f_pos+=f3dPoint(0.8*radius,0.8*radius);
			rgn_vert[0].Set(f_pos.x-length/2,f_pos.y-sys.dim_map.fPlateThickTextSize/2);
			rgn_vert[1].Set(f_pos.x+length/2,f_pos.y-sys.dim_map.fPlateThickTextSize/2);
			rgn_vert[2].Set(f_pos.x+length/2,f_pos.y+sys.dim_map.fPlateThickTextSize/2);
			rgn_vert[3].Set(f_pos.x-length/2,f_pos.y+sys.dim_map.fPlateThickTextSize/2);
			//int quad_order[4]={1,2,4,3};
			//if(strlen(pPartSpec->guige)<6)
			//if(SearchNoneIntersPos(&f_pos,rgn_vert,NULL,quad_order)!=0)
			//TODO:�Ժ�Ӧ����Ϊ�����ϰ������ wjh-2017.8.30
			/*if(SearchNoneIntersPos(&f_pos,rgn_vert,NULL,NULL)!=0)
			{
				DRAGSET.ClearEntSet();
				DimText(pBlockTableRecord,f_pos,pPartSpec->guige,TextStyleTable::hzfs.textStyleId,
					sys.fonts.segment.fPlateThickTextSize,0,AcDb::kTextCenter,AcDb::kTextVertMid);
				f3dPoint box_vertex[4];
				ads_point L_T,R_B;
				SCOPE_STRU scope;
				box_vertex[0].Set(f_pos.x-20,f_pos.y-40);
				box_vertex[1].Set(f_pos.x+20,f_pos.y-40);
				box_vertex[2].Set(f_pos.x+20,f_pos.y+40);
				box_vertex[3].Set(f_pos.x-20,f_pos.y+40);
				for(int jj=0;jj<4;jj++)
					scope.VerifyVertex(box_vertex[jj]);
				L_T[X]=scope.fMinX;
				L_T[Y]=scope.fMaxY;
				L_T[Z]=0;
				R_B[X]=scope.fMaxX;
				R_B[Y]=scope.fMinY;
				R_B[Z]=0;
			#ifdef _ARX_2007
				acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
			#else
				acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
			#endif
#ifdef AFX_TARG_ENU_ENGLISH
				DragEntSet(f_pos,"\nPlease select a proper mark position:\n");//DragEntSet(pPartSpec->base_pos,"\nPlease select a proper mark position:\n");
#else
				DragEntSet(f_pos,"\n��ѡ��һ�����ʵı�עλ��:\n");//DragEntSet(pPartSpec->base_pos,"\n��ѡ��һ�����ʵı�עλ��:\n");
#endif
				//����ָ�ԭ�е���ͼ״̬����ȻacedSSGet��������ʧ��
			#ifdef _ARX_2007
				acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
			#else
				acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
			#endif
			}
			else	//sys.dim_map.fSimPolyTextSize�滻Ϊsys.dim_map.fPlateThickTextSize wht 11-03-29
				DimText(pBlockTableRecord,f_pos,pPartSpec->guige,TextStyleTable::hzfs.textStyleId,
						sys.fonts.segment.fPlateThickTextSize,0,AcDb::kTextCenter,AcDb::kTextVertMid);*/
			//�ְ���Ĭ�ϱ�ע�ڼ���λ�����Ϸ�45�����췽���� wht 17-08-14
			f3dPoint ray_vec(1,1,0);
			normalize(ray_vec);
			f_pos=pPartSpec->base_pos+ray_vec*(radius+1);
			DimText(pBlockTableRecord,f_pos,pPartSpec->guige,TextStyleTable::hzfs.textStyleId,
					sys.dim_map.fPlateThickTextSize,0,AcDb::kTextLeft,AcDb::kTextBottom);
			//��¼�ְ���λ������ڱ��λ�õ�ƫ����,�����ƶ����ʱ���λ����֮�䶯 wht 11-07-18
			pPartSpec->fGuigeOffsetX=f_pos.x-pPartSpec->base_pos.x;
			pPartSpec->fGuigeOffsetY=f_pos.y-pPartSpec->base_pos.y;
		}
	}
#endif
	pPartSpec->cls_typeid*=-1;	//��ʾ�Ѿ���ע�Ĺ���
}

void CDimPartSpecList::DimToMap(AcDbBlockTableRecord *pBlockTableRecord,BOOL bDimByAnalysisMode)
{
	for(CDimPartSpec *pPartSpec=GetFirst();pPartSpec!=NULL;pPartSpec=GetNext())
		AnalyseAndDimPartSpec(pBlockTableRecord,pPartSpec,bDimByAnalysisMode);	
}

void CDimPartSpecList::DimPartSpec(AcDbBlockTableRecord *pBlockTableRecord,CDimPartSpec* pPartSpec, CTextFrame* pTextFrame)
{
	f3dPoint f_pos;
	CXhChar16 dimText,dimText2;
	double fFrameWidth = pTextFrame->Width();
	double fFrameHeight = pTextFrame->Height();
	double radius = fFrameWidth*0.5;
	double height = sys.dim_map.PartNo.fPartNoTextSize;
	//������Ƕȱ�עʱ��ŷ��򼰱�ע�Ƕ� 12-03-23
	double dim_angle=0;
	f3dPoint waist_vec,norm_vec;
	f3dPoint dim_pos=pPartSpec->base_pos;	//��ʼ��עλ��
	if(pTextFrame->Style()!=0)
	{
		dim_angle=pPartSpec->rod_angle;
		if(pPartSpec->rod_angle>Pi/2.0&&pPartSpec->rod_angle<=1.5*Pi)
			dim_angle=pPartSpec->rod_angle-Pi;
		waist_vec=pPartSpec->rod_vec;
		normalize(waist_vec);
		if(waist_vec.x<0)	//��֤��Բ�׷�����X�᷽����ͬ
			waist_vec*=-1.0;
		norm_vec.Set(-waist_vec.y,waist_vec.x);	//norm_vec��ֱ����Բ����
		//���ݶ��뷽ʽ������עλ��
		if(pTextFrame->textAlignHori==AcDb::kTextLeft)
			dim_pos+=waist_vec*(0.5*fFrameWidth);
		else if(pTextFrame->textAlignHori==AcDb::kTextRight)
			dim_pos-=waist_vec*(0.5*fFrameWidth);
	}
	if(pPartSpec->sBackPartNo.GetLength()>0)
	{	//�ú���Ŀǰ��������ע�Ǹ˼���ţ��ʲ����жϹ����Ƿ�Ϊ������
		//�ԳƱ�ע�����Ĺ������ʱ,Ϊ��Ӧ���Ȧ��С,Ĭ�ϵ�����ע�ָ�Ϊ2mm�������ָ��еĽ�Сֵ wht 11-08-13
		height=min(2,sys.dim_map.PartNo.fPartNoTextSize);
#ifdef AFX_TARG_ENU_ENGLISH
		dimText.Printf("front %s",(char*)pPartSpec->sPartNo);
		dimText2.Printf("behind %s",(char*)pPartSpec->sBackPartNo);
#else
		dimText.Printf("ǰ%s",(char*)pPartSpec->sPartNo);
		dimText2.Printf("��%s",(char*)pPartSpec->sBackPartNo);
#endif
		f3dPoint start,end,up_dim_pos,down_dim_pos;
		if(pTextFrame->Style()==0)
		{
			start.Set(dim_pos.x-radius,dim_pos.y);
			end.Set(dim_pos.x+radius,dim_pos.y);
			up_dim_pos.Set(dim_pos.x,dim_pos.y+0.5);
			down_dim_pos.Set(dim_pos.x,dim_pos.y-0.5);
		}
		else 
		{
			start=dim_pos-waist_vec*radius;
			end=dim_pos+waist_vec*radius;
			up_dim_pos=dim_pos+norm_vec*0.5;
			down_dim_pos=dim_pos-norm_vec*0.5;
		}
		CreateAcadLine(pBlockTableRecord,start,end);
		DimText(pBlockTableRecord,up_dim_pos,dimText,TextStyleTable::hzfs.textStyleId,
				height,dim_angle,AcDb::kTextCenter,AcDb::kTextBottom);
		DimText(pBlockTableRecord,down_dim_pos,dimText2,TextStyleTable::hzfs.textStyleId,
				height,dim_angle,AcDb::kTextCenter,AcDb::kTextTop);
	}
	else
	{
		dimText=pPartSpec->sPartNo;
		dimText2.Empty();
		DimText(pBlockTableRecord,dim_pos,pPartSpec->sPartNo,TextStyleTable::hzfs.textStyleId,
			height,dim_angle,AcDb::kTextCenter,AcDb::kTextVertMid);
	}
	
	//�����ı���Ŀ���
	if(pTextFrame->Style()==0)
		CreateAcadCircle(pBlockTableRecord,dim_pos,radius);
	else 
	{	
		f3dPoint topLeft=dim_pos-waist_vec*0.5*fFrameWidth+norm_vec*0.5*fFrameHeight;
		if(pTextFrame->Style()==1)	//��Բ���ο�
			CreateAcadRect(pBlockTableRecord,topLeft,fFrameWidth,fFrameHeight,TRUE,NULL,waist_vec);
		else	//ֱ�Ǿ��ο�
			CreateAcadRect(pBlockTableRecord,topLeft,fFrameWidth,fFrameHeight,FALSE,NULL,waist_vec);
	}
	
	/*if(pTextFrame->Style()==0)
		CreateAcadCircle(pBlockTableRecord,pPartSpec->base_pos,radius);
	else
	{	//���ο�
		f3dPoint rgn_vert[4];
		rgn_vert[0]=pPartSpec->base_pos+f3dPoint(-0.5*pTextFrame->Width(),-0.5*pTextFrame->Height());
		rgn_vert[1]=pPartSpec->base_pos+f3dPoint( 0.5*pTextFrame->Width(),-0.5*pTextFrame->Height());
		rgn_vert[2]=pPartSpec->base_pos+f3dPoint( 0.5*pTextFrame->Width(), 0.5*pTextFrame->Height());
		rgn_vert[3]=pPartSpec->base_pos+f3dPoint(-0.5*pTextFrame->Width(), 0.5*pTextFrame->Height());
		CreateAcadLine(pBlockTableRecord,rgn_vert[3],rgn_vert[2]);//�ϱ߿���
		CreateAcadLine(pBlockTableRecord,rgn_vert[0],rgn_vert[1]);//�±߿���
		CreateAcadLine(pBlockTableRecord,rgn_vert[3],rgn_vert[0]);//��߿���
		CreateAcadLine(pBlockTableRecord,rgn_vert[2],rgn_vert[1]);//�ұ߿���
	}*/
}

//////////////////////////////////////////////////////////////////////////
#ifndef __TSA_FILE_
//��˨���ע��Ԫ
static int CompareHash2Key(const HASH2KEY& item1,const HASH2KEY& item2)
{
	if(item1.key1>item2.key1)
		return 1;
	else if(item1.key1<item2.key1)
		return -1;
	else if(item1.key2>item2.key2)
		return 1;
	else if(item1.key2<item2.key2)
		return -1;
	else
		return 0;
}
void CLsGroupDimUnit::GetDimText(char *sDimText)
{
	if(L==0)
	{
		ARRAY_LIST<HASH2KEY> keyList;
		CMapList<long> mapBoltInfoList;
		for(BOLTINFO *pBoltInfo=hashBoltsByH.GetFirst();pBoltInfo;pBoltInfo=hashBoltsByH.GetNext())
		{
			long d=pBoltInfo->GetBoltD();
			long L=pBoltInfo->GetBoltL();
			long *pCount=mapBoltInfoList.GetValue(d,L);
			if(pCount==NULL)
			{
				pCount=mapBoltInfoList.Add(d,L);
				keyList.append(HASH2KEY(d,L));
				*pCount=0;
			}
			(*pCount)++;
		}
		CHeapSort<HASH2KEY>::HeapSort(keyList.m_pData,keyList.GetSize(),CompareHash2Key);
		CXhChar200 sText;
		for(HASH2KEY *pKey=keyList.GetFirst();pKey;pKey=keyList.GetNext())
		{
			long *pCount=mapBoltInfoList.GetValue(pKey->key1,pKey->key2);
			if(sText.GetLength()>0)
				sText.Append("\n");
			sText.Append(CXhChar16("%dM%dX%d",*pCount,pKey->key1,pKey->key2));
		}
		strcpy(sDimText,sText);
	}
	else
		sprintf(sDimText,"%dM%dX%d",hashBoltsByH.GetNodeNum(),d,L);
}

void CLsGroupDimUnit::FromBuffer(CBuffer &buffer, long version/*=NULL*/)
{
	buffer.ReadInteger(&hDatumRod);
	//pDatumRod=(CLDSLinePart*)Ta.FromPartHandle(hDatumBolt,CLS_LINEPART);
	buffer.ReadInteger(&hDatumBolt);
	//pDatumBolt=(CLDSBolt*)Ta.FromPartHandle(hDatumBolt,CLS_BOLT);
	buffer.ReadInteger(&L);
	buffer.ReadInteger(&d);
	hashBoltsByH.Empty();
	int nBolt=0;
	buffer.ReadInteger(&nBolt);
	for(int i=0;i<nBolt;i++)
	{
		long hBolt=0;
		buffer.ReadInteger(&hBolt);
		if(d==0)
		{
			CLDSBolt *pBolt=(CLDSBolt*)Ta.FromPartHandle(hBolt,CLS_BOLT);
			d=pBolt->get_d();
			L=ftoi(pBolt->get_L());
		}
		hashBoltsByH.SetValue(hBolt,BOLTINFO(hBolt));
	}
}

void CLsGroupDimUnit::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(hDatumRod);
	buffer.WriteInteger(hDatumBolt);
	buffer.WriteInteger(L);
	buffer.WriteInteger(d);
	int nBolt=hashBoltsByH.GetNodeNum();
	buffer.WriteInteger(nBolt);
	for(BOLTINFO *pBoltInfo=hashBoltsByH.GetFirst();pBoltInfo;pBoltInfo=hashBoltsByH.GetNext())
		buffer.WriteInteger(pBoltInfo->hBolt);
}
//��ȡ��˨��ע�����׼�˼�
CLDSLinePart* CLsGroupDimUnit::GetDimVecDatumRod(CLDSBolt *pBolt)
{
	CLDSLinePart *pRod=NULL;
	CLDSPart *pPart=Ta.FromPartHandle(pBolt->des_base_pos.hPart);
	if(pPart&&pPart->IsLinePart())
		pRod=(CLDSLinePart*)pPart;
	else if(pPart&&pPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
		if(pParamPlate->m_iParamType==TYPE_XEND||pParamPlate->m_iParamType==TYPE_UEND
			||pParamPlate->m_iParamType==TYPE_ROLLEND)
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)Ta.FromPartHandle(pParamPlate->m_hPartWeldParent,CLS_LINEPART);
			if(pLinePart)
				pRod=pLinePart;
		}
	}
	return pRod;
}
//��ʼ����˨���׼�˼�
void CLsGroupDimUnit::InitDatumRod(CLDSBolt *pBolt)
{
	if(pDatumRod!=NULL)
		return;	//�Ѵ��ڻ�׼�˼����������¼���
	//��ȡ��˨��ע�����׼�˼�
	pDatumRod=GetDimVecDatumRod(pBolt);
	if(pDatumRod)
	{
		hDatumRod=pDatumRod->handle;
		ucs.axis_x=pDatumRod->End()-pDatumRod->Start();
		//����˼���������˨X-Yƽ���ϵ�ͶӰ����
		project_vector(ucs.axis_x,ucs.axis_z);
		normalize(ucs.axis_x);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		//m_bInitAxisXByRod=TRUE;
	}
}
//��ʼ����˨����Ϣ
void CLsGroupDimUnit::Init(CLDSBolt *pBolt)
{
	hashBoltsByH.Empty();
	hashBoltsByH.SetValue(pBolt->handle,BOLTINFO(pBolt));
	hDatumBolt=pBolt->handle;
	pDatumBolt=pBolt;
	d=pBolt->get_d();
	L=ftoi(pBolt->get_L());
	group_scope.VerifyVertex(f3dPoint());
	//��ʼ����˨������ϵʱ��Ӧʹ��GetUCS()�������ϵ��������ֱ��ʹ��ucs wht 12-05-29
	ucs=pBolt->GetUCS();
	//��ʼ����׼�˼�
	InitDatumRod(pBolt);
}
bool CLsGroupDimUnit::IsPlateLsGroup()
{
	return L==0;
}
//��ʼ����˨����Ϣ
bool CLsGroupDimUnit::Init(CLDSPlate *pPlate,BOLTSET &boltSet)
{
	if(pPlate==NULL||boltSet.GetNodeNum()<=0)
		return false;
	CLDSLinePart *pRod=(CLDSLinePart*)pPlate->BelongModel()->FromRodHandle(pPlate->designInfo.m_hBasePart);
	if(pRod==NULL)
		return false;
	L=0;	//�Ըְ�Ϊ��Ԫ����˨��
	d=pPlate->handle;
	ucs=pPlate->ucs;
	pDatumRod=pRod;
	hDatumRod=pRod->handle;
	hashBoltsByH.Empty();
	for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		CLDSBolt *pBolt=pLsRef->GetLsPtr();
		if(boltSet.FromHandle(pBolt->handle)==NULL)
			continue;
		hashBoltsByH.SetValue(pBolt->handle,BOLTINFO(pBolt));	
		if(pDatumBolt==NULL&&pBolt->des_base_pos.hPart==pRod->handle)
		{
			hDatumBolt=pBolt->handle;
			pDatumBolt=pBolt;
		}
	}
	if(pDatumBolt==NULL||hashBoltsByH.GetNodeNum()<=0)
		return false;
	else
		return true;
}
f3dPoint CLsGroupDimUnit::TransToLocalCoord(f3dPoint pos)
{
	coord_trans(pos,ucs,FALSE,TRUE);
	return pos;
}

//�����˨����˨��	
void CLsGroupDimUnit::AddBolt(BOLTINFO boltInfo)
{
	hashBoltsByH.SetValue(boltInfo.hBolt,boltInfo);
	//��ʼ����׼�˼�
	InitDatumRod(boltInfo.GetBolt());
	group_scope.VerifyVertex(f3dPoint(boltInfo.pos2d.x,boltInfo.pos2d.y));
}
//�Ƴ�ָ����˨,�������Ƿ��Ƴ��ɹ�
BOOL CLsGroupDimUnit::RemoveBolt(CLDSBolt *pBolt)	
{
	if(pBolt==NULL||hashBoltsByH.GetValue(pBolt->handle)==NULL)
		return FALSE;	//��ǰ����ָ����˨
	hashBoltsByH.DeleteNode(pBolt->handle);
	if(pBolt->handle==hDatumBolt)
	{	//ɾ����˨Ϊ��׼��˨�����»�׼��˨
		BOLTINFO *pBoltInfo=hashBoltsByH.GetFirst();
		if(pBoltInfo)
		{
			hDatumBolt=pBoltInfo->hBolt;
			pDatumBolt=pBoltInfo->GetBolt();
		}
	}
	return TRUE;
}
//�ж���˨�Ƿ����ڵ�ǰ��
double CLsGroupDimUnit::fLsSpaceVerticalThreshold=80;		//��ֱ������˨�����ֵ
double CLsGroupDimUnit::fLsSpaceHorizontalThreshold=100;	//ˮƽ������˨�����ֵ
BOOL CLsGroupDimUnit::IsIncludeBolt(CLsGroupDimUnit::BOLTINFO boltInfo)
{
	if(boltInfo.GetBolt()==NULL)
		return FALSE;
	// 1.�ж���˨����Ƿ���ͬ
	if(d!=boltInfo.GetBoltD()||L!=ftoi(boltInfo.GetBoltL()))
		return FALSE;	//��˨���ͬ
	double thresholdV2=fLsSpaceVerticalThreshold*fLsSpaceVerticalThreshold;
	double thresholdH2=fLsSpaceHorizontalThreshold*fLsSpaceHorizontalThreshold;
	// 2.�ж���˨�Ƿ��ڵ�ǰ��
	f3dPoint pos=TransToLocalCoord(boltInfo.GetBolt()->ucs.origin);	//����˨λ��ת�������������ϵ�£������ж�
	if(pDatumRod)
	{
		CLDSLinePart *pOtherDatumRod=GetDimVecDatumRod(boltInfo.GetBolt());
		if(pOtherDatumRod&&pOtherDatumRod->handle!=pDatumRod->handle)
			return FALSE;	//��˨��׼�˼���ͬ�����ɷ���ͬһ�� wht 12-03-23
		double fThresholdX=fLsSpaceHorizontalThreshold;
		double fThresholdY=fLsSpaceVerticalThreshold;
		if( pos.x>=group_scope.fMinX-fThresholdX&&pos.x<=group_scope.fMaxX+fThresholdX&&
			pos.y>=group_scope.fMinY-fThresholdY&&pos.y<=group_scope.fMaxY+fThresholdY)
			return TRUE;
		else
			return FALSE;
	}
	else 
	{	//����������ϵ������˨���е���˨������бȶԣ�ֻҪ��һ������ֵ��Χ�ڼ���Ϊ���ڵ�ǰ��
		for(BOLTINFO *pBoltInfo=hashBoltsByH.GetFirst();pBoltInfo;pBoltInfo=hashBoltsByH.GetNext())
		{
			if((pos.x-pBoltInfo->pos2d.x)*(pos.x-pBoltInfo->pos2d.x)+(pos.y-pBoltInfo->pos2d.y)*(pos.y-pBoltInfo->pos2d.y)<thresholdH2)
				return TRUE;
		}
	}
	return FALSE;
}
void CLsGroupDimUnit::CopyProperty(CLsGroupDimUnit *pLsGroupDim)
{
	CLDSBolt *pBolt=pLsGroupDim->GetDatumBolt();
	if(pBolt)
	{
		pDatumBolt=pBolt;
		hDatumBolt=pBolt->handle;
	}
	CLDSLinePart *pLinePart=pLsGroupDim->GetDatumRod();
	if(pLinePart)
	{
		pDatumRod=pLinePart;
		hDatumRod=pLinePart->handle;
	}
	L=pLsGroupDim->GetBoltL();
	d=pLsGroupDim->GetBoltD();
	hashBoltsByH.Empty();
	for(BOLTINFO *pInfo=pLsGroupDim->GetFirstBoltInfo();pInfo;pInfo=pLsGroupDim->GetNxetBoltInfo())
	{
		hashBoltsByH.SetValue(pInfo->hBolt,*pInfo);
	}
}
CLDSBolt* CLsGroupDimUnit::GetDatumBolt()
{
	if(pDatumBolt==NULL)
		pDatumBolt=(CLDSBolt*)Ta.FromPartHandle(hDatumBolt,CLS_BOLT);
	return pDatumBolt;
}
CLDSLinePart* CLsGroupDimUnit::GetDatumRod()
{
	if(pDatumRod==NULL)
		pDatumRod=(CLDSLinePart*)Ta.FromPartHandle(hDatumRod,CLS_LINEPART);
	return pDatumRod;
}
void CLsGroupDimUnit::SetDatumRod(CLDSLinePart *pRod)
{
	if(pRod)
	{
		pDatumRod=pRod;
		hDatumRod=pRod->handle;
	}
}
//�ж���˨���ڻ�׼�˼�ʼ�ˡ��ն˻����м�,�Զ�����λ��ʱ�����жϲ��ҷ���
LSGROUP_POS CLsGroupDimUnit::GetLsGroupPos(f3dPoint base_pos,AcDbLine *pDatumLine,double fScale)
{
	LSGROUP_POS groupPos=GROUP_MID;
	/*if(pDatumBolt&&pDatumRod&&pDatumRod->IsAngle())
	{
		if(pDatumBolt->des_base_pos.datumPoint.datum_pos_style==1)
		{
			if(pDatumBolt->des_base_pos.datumPoint.des_para.JGEND.direction==0)
				groupPos=GROUP_START;
			else 
				groupPos=GROUP_END;
		}
	}*/
	if(pDatumLine)
	{	//������˨���׼λ����˼�ʼ�ն�λ���жϣ���˨��λ�� wht 12-05-31
		double threshold_dist=500*fScale;
		double fStartDist=DISTANCE(base_pos,pDatumLine->startPoint());
		double fEndDist=DISTANCE(base_pos,pDatumLine->endPoint());
		if(fStartDist<threshold_dist)
			groupPos=GROUP_START;
		else if(fEndDist<threshold_dist)
			groupPos=GROUP_END;
		else 
			groupPos=GROUP_MID;
	}
	return groupPos;
}

CXhChar500 CLsGroupDimUnit::GetBoltIdKey()
{
	ARRAY_LIST<long> idArr;
	for(BOLTINFO *pInfo=hashBoltsByH.GetFirst();pInfo;pInfo=hashBoltsByH.GetNext())
		idArr.append(pInfo->hBolt);
	CHeapSort<long>::HeapSort(idArr.m_pData,idArr.GetSize(),compare_long);
	CXhChar500 sKey;
	for(long *pId=idArr.GetFirst();pId;pId=idArr.GetNext())
		sKey.Append((char*)CXhChar16("0x%X",*pId),'#');
	return sKey;
}

//<DEVELOP_PROCESS_MARK nameId="SPHERE_COORD">
//////////////////////////////////////////////////////////////////////////
//
BOOL SPHERE_COORD::IsEqual(SPHERE_COORD sphereCoord)
{
	if( fabs(latitude-sphereCoord.latitude)>EPS||
		fabs(longitude-sphereCoord.longitude)>EPS)
		return false;
	else 
		return true;
}
//�Ƕ����ַ���
CXhChar200 SPHERE_COORD::ToDegreeString()
{
#ifdef AFX_TARG_ENU_ENGLISH
	CXhChar16 sLatitude("North latitude");
	if(latitude<0)
		sLatitude.Copy("South latitude");
#else
	CXhChar16 sLatitude("��γ");
	if(latitude<0)
		sLatitude.Copy("��γ");
#endif
	CXhChar200 sDegree("(%s%.1f��,%.1f��)",(char*)sLatitude,fabs(latitude)*DEGTORAD_COEF,
		fabs(longitude)*DEGTORAD_COEF);
	return sDegree;
}
//�������ַ���
CXhChar200 SPHERE_COORD::ToRadianString()
{
#ifdef AFX_TARG_ENU_ENGLISH
	CXhChar16 sLatitude("North latitude");
	if(latitude<0)
		sLatitude.Copy("South latitude");
#else
	CXhChar16 sLatitude("��γ");
	if(latitude<0)
		sLatitude.Copy("��γ");
#endif
	return CXhChar200("(%s%.6f��,%.6f��)",(char*)sLatitude,fabs(latitude),fabs(longitude));
}
//1.��XYƽ��Ϊ�����
//	Z>0��ʾ��γ��γ�ȷ�ΧΪ(0~90)
//	Z<0��ʾ��γ��γ�ȷ�ΧΪ(0~-90)
//2.��XZƽ��X+Ϊ���������߼�����Ϊ0
//	Y+����Ϊ����(0~180)
//	Y-����Ϊ����(0~180)
SPHERE_COORD SPHERE_COORD::TransToSphereCoord(f3dPoint pt)
{	
	SPHERE_COORD sphereCoord;
	if(!pt.IsZero())
	{
		//1.����pt��equatorPt����ֱ�߷������γ��
		f3dPoint  equatorPt(pt.x,pt.y,0);	//���ڳ����(XY��)�ϵ�ͶӰ��
		sphereCoord.latitude=cal_angle_of_2vec(pt,equatorPt);
		if(sphereCoord.latitude>0.5*Pi)
			sphereCoord.latitude-=0.5*Pi;
		if(pt.z<0)	//ZС��0Ϊ��γ���ø�����ʾ
			sphereCoord.latitude*=-1;
		//2.����pt�ڳ�����ϵ�ͶӰ���뱾��������֮��ļнǣ�������
		sphereCoord.longitude=Cal2dLineAng(0,0,equatorPt.x,equatorPt.y);
	}
	return sphereCoord;
}

//��γ������ת��Ϊֱ������
f3dPoint SPHERE_COORD::TransToRectangularCoord(SPHERE_COORD sphereCoord,double R)
{
	f3dPoint initPt(R,0,0);	//����Ϊ0�ĳ�ʼ��
	if(R>0)
	{	//1.����ʼ����ת��ָ���Ƕȵľ�������(�õ�λ�ڳ������)
		rotate_point_around_axis(initPt,sphereCoord.longitude,f3dPoint(0,0,0),f3dPoint(0,0,R));
		//2.����һ������ת���,��ת��ָ���Ƕȵ�γ������
		f3dPoint rotate_axis(initPt.y,-initPt.x);	//˳ʱ����ת90��õ���תγ��ʱ��Ҫ����ת��
		rotate_point_around_axis(initPt,sphereCoord.latitude,f3dPoint(0,0,0),rotate_axis);
	}
	return initPt;
}
//</DEVELOP_PROCESS_MARK>

//<DEVELOP_PROCESS_MARK nameId="CDrawUnit::Int2dls">
/*ֱ�������󽻵�
  return value:
	0:ֱ�������޽���
	1:�������غ�Ϊһ���е�
	2:�ҵ�������Ч����
	-1:ֱ�߻���������
*/
int Int2dls(f3dPoint &inters1,f3dPoint &inters2,f3dLine line,double sphere_r)
{	//1.ֱ�߻���������
	if(sphere_r<=0||DISTANCE(line.startPt,line.endPt)<EPS)
		return -1;
	int nRetCode=0;
	//2.������ֱ��ͶӰȷ����ֱ���ཻԲ����ƽ��
	f3dPoint perp;
	double fDist=0;
	SnapPerp(&perp,line,f3dPoint(0,0,0),&fDist);
	//2.1ֱ����������
	if(fDist>sphere_r)
		return 0;	//Բ�ĵ�ֱ�ߵ�ͶӰ���������뾶,ֱ�������޽���
	//2.2ֱ���������У�����һ���е�
	if(fabs(fDist-sphere_r)<EPS)
	{
		inters1=perp;
		return 1;	//ֱ����������
	}
	//2.3ֱ�������ཻ
	//2.3.1 ������ֱ���ཻ��Բ���ڵ�����ϵ
	UCS_STRU cir_cs;
	cir_cs.origin.Set();
	f3dPoint line_vec=line.endPt-line.startPt;
	normalize(line_vec);
	f3dPoint perp_vec=perp;
	cir_cs.axis_y=line_vec;
	if(perp_vec.IsZero())	//ֱ�߹�Բ��
		cir_cs.axis_x=inters_vec(cir_cs.axis_y);
	else					//ֱ��δ��Բ��
		cir_cs.axis_x=-perp_vec;
	cir_cs.axis_z=cir_cs.axis_x^cir_cs.axis_y;
	cir_cs.axis_y=cir_cs.axis_z^cir_cs.axis_x;
	normalize(cir_cs.axis_x);
	normalize(cir_cs.axis_y);
	normalize(cir_cs.axis_z);
	//2.3.2 ֱ����Բ�󽻵�
	//��ֱ��ת����Բ����ϵ��
	coord_trans(line.startPt,cir_cs,FALSE);
	coord_trans(line.endPt,cir_cs,FALSE);
	f2dLine line2d;
	line2d.startPt.Set(line.startPt.x,line.startPt.y);
	line2d.endPt.Set(line.endPt.x,line.endPt.y);
	nRetCode=Int2dlc(line2d,f2dCircle(sphere_r,0,0),inters1.x,inters1.y,inters2.x,inters2.y);
	//����õĽ���ת����������ϵ��
	if(nRetCode>0)
		coord_trans(inters1,cir_cs,TRUE);
	if(nRetCode>1)
	{
		coord_trans(inters2,cir_cs,TRUE);
		f3dPoint inters_vec=inters2-inters1;
		normalize(inters_vec);
		if(inters_vec*line_vec<0)
		{
			f3dPoint temp=inters1;
			inters1=inters2;
			inters2=temp;
		}
	}
	return nRetCode;
}
//</DEVELOP_PROCESS_MARK>

#endif
//////////////////////////////////////////////////////////////////////////