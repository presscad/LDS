// Drawing.cpp: implementation of the CDrawing class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdarx.h"
#include "Drawing.h"
#include "DragEntSet.h"
#include "AdsTools.h"
#include "DrawTaMap.h"
#include "InputDrawingUcsDlg.h"
#include "GlobalFunc.h"
#include "RxTools.h"
#include "ScrTools.h"
#include "SortFunc.h"
#include "DimPlankLs.h"
#include "DimJgZhun.h"
#include "LogFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef __SMART_DRAW_INC_
CDrawing* CreateDrawing(int idClsType,DWORD key,void* pContext)
{
	return new CDrawing();
}
BOOL DeleteDrawing(CDrawing *pDrawing)
{
	if(pDrawing)
	{
		delete pDrawing;
		return TRUE;
	}
	return FALSE;
}
//ͼֽ
CSuperHashList<CDrawing> drawingList;
//
static IDrawingComponent *NewDrawingComponent(int CLSTYPE_ID,DWORD key,void* pContext)
{
	CDrawingComponent *pDrawingCom=NULL;
	IDrawing* pParentDrawing=NULL;
	if(pContext!=NULL)
		pParentDrawing=((DrawingComponentContext*)pContext)->pParentDrawing;
	switch(CLSTYPE_ID)
	{
	case DRAW_COM_LINEVIEW:
		pDrawingCom = new CLineView(key,pParentDrawing);
		break;
	case DRAW_COM_STRUVIEW:
		pDrawingCom = new CStruView(key,pParentDrawing);
		break;
	case DRAW_COM_STDVIEW:
		pDrawingCom = new CStdView(key,pParentDrawing);
		break;
	case DRAW_COM_BOM:
		pDrawingCom = new CDrawingBOM(key,pParentDrawing);
		break;
	case DRAW_COM_PARTVIEW:
		pDrawingCom = new CPartDrawing(key,pParentDrawing);
		break;
	default:
		pDrawingCom = new CDrawingComponent(key,pParentDrawing);
	}
	return pDrawingCom;
}

static BOOL NativeDeleteDrawingComponent(IDrawingComponent *pCom)
{
	switch(((CDrawingComponent*)pCom)->m_eClassType)
	{
	case DRAW_COM_LINEVIEW:
		delete (CLineView*)pCom;
		break;
	case DRAW_COM_STRUVIEW:
		delete (CStruView*)pCom;
		break;
	case DRAW_COM_STDVIEW:
		delete (CStdView*)pCom;
		break;
	case DRAW_COM_BOM:
		delete (CDrawingBOM*)pCom;
		break;
	case DRAW_COM_PARTVIEW:
		delete (CPartDrawing*)pCom;
		break;
	default:
		delete pCom;
	}
	return TRUE;
}

static int GetDrawingComType(CDrawingComponent *pCom)
{
	return (int)pCom->m_eClassType;
}
//
void GetTaUCS(UCS_STRU &ucs,BOOL is_frontface);

CDrawing::CDrawing(int id/*=0*/) : CXeroDrawing(id)
{
	m_nDrawingNumber=1;		
	m_bSpecifyMapFrmPos=TRUE;
	m_pActiveModule=NULL;
	m_hActiveModule=0;
	m_bAutoLayoutComponent=FALSE;
	m_fComponentSpace=20;
	//����HashList�ص�����
	listComponents.CreateNewAtom=NewDrawingComponent;
	listComponents.DeleteAtom=NativeDeleteDrawingComponent;
	LoadDefaultUCS(&dcs);
	feature=0;
}

CDrawing::~CDrawing()
{

}

// ����ָ������ͼ��ͼֽ�����ɶ�Ӧ����ͼ
// pView ָ����ͼ
// pModule ����˾��ͼ����ͼʱ�ñ�����Ч,����ָ�����߷���
// bFlag   �ṹͼ:��ʾ���ɵ���ͼ���ǽṹͼ ˾��ͼ\��ͼ:��ʾ������ͼ������ͼ
CDrawingComponent *CDrawing::AddComponentByView(CDisplayView *pView,CLDSModule *pModule,BOOL bFlag/*=FALSE*/)
{
	if(m_nSegI.iSeg==-1||m_nSegI.iSeg==-2)
	{	//˾��ͼ����ͼ
		CLineView *pLineView=NULL;
		if(pView->m_iViewType==0)
		{	//ͼ����ͼ
			if(pModule==NULL)
				return NULL;
			if(pModule->handle==m_hActiveModule)	//����+������߷���
				pLineView=(CLineView*)listComponents.Add(pView->handle-abs(m_nSegI.iSeg),DRAW_COM_LINEVIEW);
			else	//�Ǽ������
				pLineView=(CLineView*)listComponents.Add(pView->handle+pModule->handle,DRAW_COM_LINEVIEW);
			//pLineView->m_pDrawing=this;
			pLineView->m_bNeedAdjust=FALSE;//(m_nSegI==-1);	//��ͼ����Ҫ����
			pLineView->m_bAnalysisMode=(m_nSegI.iSeg==-1);	//��ѧ����ģʽ��˾��ͼ
			pLineView->SetDCS(pView->ucs);
			pLineView->m_eClassType=DRAW_COM_LINEVIEW;
			pLineView->m_hAttachView=pView->handle;
			pLineView->m_pAttachView=pView;
			pLineView->m_sDrawingLayer.Copy(pView->filter.ThirdCharSet);	//ͼ�㷶Χ
			pLineView->m_hModule=pModule->handle;
			pLineView->m_pModule=pModule;
			if(pModule->handle==m_hActiveModule)	//����+������߷���
			{
				if(!bFlag)	//����ͼ
					pLineView->m_sName.Printf("%s����ͼ",pModule->description);
				else		//����ͼ
					pLineView->m_sName.Printf("%s����ͼ",pModule->description);
			}
			else	//�Ǽ�����߷���
				pLineView->m_sName.Printf("%s����ͼ",pModule->description);
			if(m_nSegI.iSeg==-1)
				pLineView->m_fScale=1.0/sys.general_set.ForceLineScale;
			pLineView->CreateCmdList();
			return pLineView;
		}
		else if(pView->m_iViewType==1)
		{	//������ͼ
			pLineView=(CLineView*)listComponents.Add(pView->handle,DRAW_COM_LINEVIEW);			
			//pLineView->m_pDrawing=this;
			pLineView->m_bNeedAdjust=FALSE;//(m_nSegI==-1);	//��ͼ����Ҫ����
			pLineView->m_bAnalysisMode=(m_nSegI.iSeg==-1);	//��ѧ����ģʽ��˾��ͼ
			pLineView->SetDCS(pView->ucs);
			pLineView->m_eClassType=DRAW_COM_LINEVIEW;
			pLineView->m_hAttachView=pView->handle;
			pLineView->m_pAttachView=pView;
			pLineView->m_sName.Printf("%s",pView->name);
			pLineView->CreateCmdList();
			if(m_nSegI.iSeg==-1)
				pLineView->m_fScale=1.0/sys.general_set.ForceLineScale;
			return pLineView;
		}
		else 
			return NULL;
	}
	else 
	{	//�ṹͼ
		if(pView->m_iViewType==1&&!(pView->GetSegI().iSeg==m_nSegI.iSeg||(SEGI(pView->folderName)).iSeg==m_nSegI.iSeg))
			return NULL;
		if(bFlag)
		{	//ָ����ͼΪ����ͼ
			CLineView *pLineView=(CLineView*)listComponents.Add((pView->handle+m_nSegI.iSeg)%16+16,DRAW_COM_LINEVIEW);
			//pLineView->m_pDrawing=this;
			pLineView->m_bNeedAdjust=FALSE;	//����Ҫ���е���
			pLineView->m_bAnalysisMode=(m_nSegI.iSeg==-1);//��ѧ����ģʽ��˾��ͼ
			pLineView->SetDCS(pView->ucs);
			pLineView->m_eClassType=DRAW_COM_LINEVIEW;
			pLineView->m_hAttachView=pView->handle;
			pLineView->m_pAttachView=pView;
			pLineView->m_sDrawingLayer.Copy(pView->filter.ThirdCharSet);	//ͼ�㷶Χ
			pLineView->m_sName.Printf("����ͼ 1:100");
			pLineView->m_fScale=0.01;	//��ͼ���� 1:100
			pLineView->CreateCmdList();
			return pLineView;
		}
		else
		{
			CStruView *pStruView=NULL;
			if(pView->m_iViewType==0)		//ͼ����ͼ
			{
				pStruView=(CStruView*)listComponents.Add(pView->handle+m_nSegI.iSeg,DRAW_COM_STRUVIEW);	
				pStruView->m_sDrawingLayer.Copy(pView->filter.ThirdCharSet);	//ͼ�㷶Χ
			}
			else if(pView->m_iViewType==1)	//������ͼ
				pStruView=(CStruView*)listComponents.Add(pView->handle,DRAW_COM_STRUVIEW);			
			else 
				return NULL;
			//��ͼ��������
			//pStruView->m_pDrawing=this;
			pStruView->SetDCS(pView->ucs);
			pStruView->m_bNeedAdjust=FALSE;	//����Ҫ���е���
			pStruView->m_eClassType=DRAW_COM_STRUVIEW;
			pStruView->m_hAttachView=pView->handle;
			pStruView->m_pAttachView=pView;
			pStruView->m_sName.Printf("%s",pView->name);
			int name_len=strlen(pView->name);
			char cSectionViewCode=pView->name[name_len-1];
			if(isalpha(cSectionViewCode)||isdigit(cSectionViewCode))	//���ֻ���ĸ
				pStruView->m_sSectionViewCode.Printf("%c",cSectionViewCode);//Ĭ��ȡ����ͼ���Ƶ����һ���ַ�
			else 
				strcpy(pStruView->m_sSectionViewCode,"");
			pStruView->CreateCmdList();
			return pStruView;
		}
	}
	return NULL;
}

void CDrawing::InitPartDrawing(CStruView *pStruView)
{
	if(pStruView==NULL)
		return;
	CLDSPart *pValue=NULL,*pPart=NULL;
	CPartDrawing *pPartDrawing=NULL;
	CStringKeyHashTable<CLDSPart*> hashTable;
	hashTable.CreateHashTable(pStruView->GetPartCount());
	for(pPart=pStruView->GetFirstPart();pPart;pPart=pStruView->GetNextPart())
	{
		int nStyle=-1;
		if(hashTable.GetValueAt(pPart->GetPartNo(),pValue))
			continue;
		//0.��ͷ
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{	//1.���Ҷ̽Ǹ�
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPart;
			if(pLineAngle->bHuoQuStart||pLineAngle->bHuoQuEnd
				||pLineAngle->pStart||pLineAngle->pEnd)
				continue;
			nStyle=1;
		}
		else if(pPart->GetClassTypeId()==CLS_PLATE)
		{	//2.��˨���
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			if(pPlate->jdb_style!=4)
				continue;
			nStyle=2;
		}
		//3.�����塢�߰��Լ���Խ����
		else 
			continue;
		hashTable.SetValueAt(pPart->GetPartNo(),pPart);
		pPartDrawing=(CPartDrawing*)listComponents.Add(pPart->handle,DRAW_COM_PARTVIEW);
		if(nStyle==1)
			pPartDrawing->m_fScale=0.1;	//�̽Ǹֻ�ͼ����Ĭ��Ϊ1::10
		else if(nStyle==2)
			pPartDrawing->m_fScale=0.2;	//��˨���Ĭ�ϻ�ͼ����Ϊ1:5
		//pPartDrawing->m_pDrawing=this;
		pPartDrawing->m_hPart=pPart->handle;
		pPartDrawing->m_pPart=pPart;
		char sGuige[100]="";
		pPart->GetSpecification(sGuige,TRUE);
		pPartDrawing->m_sName.Printf("%s %s",(char*)pPart->GetPartNo(),sGuige);
		pPartDrawing->m_bNeedAdjust=FALSE;
		pPartDrawing->CreateCmdList();
	}
	//�����Ǹ�
	/*
	CLDSPolyPart *pPolyPart=NULL;
	for(pPolyPart=pStruView->GetFirstPolyPart();pPolyPart;pPolyPart=pStruView->GetNextPolyPart())
	{
		if(hashTable.GetValueAt(pPolyPart->sPartNo,pValue))
			continue;
		hashTable.SetValueAt(pPolyPart->sPartNo,pPart);
		pPartDrawing=(CPartDrawing*)listComponents.Add(pPolyPart->handle,DRAW_COM_PARTVIEW);
		pPartDrawing->m_pDrawing=this;
		pPartDrawing->m_hAttachView=pStruView->m_hAttachView;
		pPartDrawing->m_pAttachView=pStruView->m_pAttachView;
		pPartDrawing->dcs=pStruView->dcs;
		pPartDrawing->dcs.origin=pPolyPart->ucs.origin;
		pPartDrawing->m_bPolyPart=TRUE;
		pPartDrawing->polyPart.CopyProperty(pPolyPart);
		CLDSLinePart *pLinePart=NULL;
		pPartDrawing->sonPoleHandleList.Empty();
		for(pLinePart=pPolyPart->segset.GetFirst();pLinePart;pLinePart=pPolyPart->segset.GetNext())
			pPartDrawing->sonPoleHandleList.append(pLinePart->handle);
		pPartDrawing->m_sName.Printf("%s�����Ǹ�",pPolyPart->sPartNo);
		pPartDrawing->m_bNeedAdjust=FALSE;
		pPartDrawing->CreateCmdList();
	}
	*/
}

void CDrawing::InitDrawing(SEGI nSegI)
{	//��ʼ����ͼ�б�
	sectionViewSymbols.Empty();
	listComponents.Empty();
	m_nSegI=nSegI;
	CDrawingBOM *pDrawingBOM=NULL;
	CDrawingComponent *pComponent=NULL;
	CDisplayView *pView=NULL,*pFrontView=NULL,*pSideView=NULL;
	if(nSegI.iSeg==-1||nSegI.iSeg==-2)
	{	//˾��ͼ����ͼ
		if(nSegI.iSeg==-1)
		{	//��ע�������ʱ��Ҫ���ɼ򻯹����ñ��Ӧ���ʼ����
			pDrawingBOM=(CDrawingBOM*)listComponents.Add(BOM_GUIGE,DRAW_COM_BOM);
			//pDrawingBOM->m_pDrawing=this;
			pDrawingBOM->m_bNeedAdjust=FALSE;	//�����Ҫ���е���
			pDrawingBOM->m_eClassType=DRAW_COM_BOM;
			pDrawingBOM->m_eBOMType=BOM_GUIGE;
			pDrawingBOM->m_sName.Copy("�򻯹���");
			pDrawingBOM->CreateCmdList();
		}
		for(pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
		{
			if(pView->m_iViewType==0&&stricmp(pView->defaultLayer,"SPQ")==0&&pView->filter.ThirdCharSet[0]!='?')
				pFrontView=pView;
			else if(pView->m_iViewType==0&&stricmp(pView->defaultLayer,"SPY")==0)
				pSideView=pView;
			if(pFrontView&&pSideView)
				break;
		}
		if(pFrontView)
		{
			if(nSegI.iSeg==-2)//��ͼ
			{	//���ϻ��ܱ�
				pDrawingBOM=(CDrawingBOM*)listComponents.Add(BOM_SUMPART,DRAW_COM_BOM);
				//pDrawingBOM->m_pDrawing=this;
				pDrawingBOM->m_bSpecifyComPos=FALSE;//���ϻ��ܱ�Ĭ�Ϸ��������Ͻǲ���Ҫ�Ϸ�
				pDrawingBOM->m_bNeedAdjust=FALSE;	//�����Ҫ���е���
				pDrawingBOM->m_eClassType=DRAW_COM_BOM;
				pDrawingBOM->m_eBOMType=BOM_SUMPART;
				pDrawingBOM->m_sName.Copy("���ϻ��ܱ�");
				pDrawingBOM->CreateCmdList();
				//����������
				pDrawingBOM=(CDrawingBOM*)listComponents.Add(BOM_GENKAI,DRAW_COM_BOM);
				//pDrawingBOM->m_pDrawing=this;
				pDrawingBOM->m_bSpecifyComPos=TRUE;
				pDrawingBOM->m_bNeedAdjust=FALSE;
				pDrawingBOM->m_eClassType=DRAW_COM_BOM;
				pDrawingBOM->m_eBOMType=BOM_GENKAI;
				pDrawingBOM->m_sName.Copy("����������");
				pDrawingBOM->CreateCmdList();
			}
			
			CLDSModule *pModule=NULL;
			CLDSModule *pActiveModule=Ta.Module.FromHandle(m_hActiveModule);
			if(pActiveModule)
			{	//����ͼ
				pComponent=AddComponentByView(pFrontView,pActiveModule);
				pComponent->m_bSpecifyComPos=FALSE;//����ͼĬ�Ϸ��������Ͻǲ���Ҫ�Ϸ�
				//������ͼ(��ע�ڵ�ż��˼����) wht 11-06-15
				if(nSegI.iSeg==-1&&pSideView)
					AddComponentByView(pSideView,pActiveModule,TRUE);
			}
			for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
			{
				if(pModule->handle!=m_hActiveModule)
					AddComponentByView(pFrontView,pModule);
			}
			if(nSegI.iSeg==-1)//˾��ͼ
			{	//������ͼ
				for(pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
				{
					if(pView->m_iViewType!=1)
						continue;
					AddComponentByView(pView,NULL);	//������ͼ
				}
			}
		}
	}
	else 
	{	//�ṹͼ
		//�ֶι�����ϸ��
		pDrawingBOM=(CDrawingBOM*)listComponents.Add(BOM_SEGPART,DRAW_COM_BOM);
		//pDrawingBOM->m_pDrawing=this;
		pDrawingBOM->m_bNeedAdjust=FALSE;	//�����Ҫ���е���
		pDrawingBOM->m_eClassType=DRAW_COM_BOM;
		pDrawingBOM->m_eBOMType=BOM_SEGPART;
		pDrawingBOM->m_sName.Copy("������ϸ��");
		pDrawingBOM->CreateCmdList();
		//�ֶ���˨��ϸ��
		pDrawingBOM=(CDrawingBOM*)listComponents.Add(BOM_SEGBOLT,DRAW_COM_BOM);
		//pDrawingBOM->m_pDrawing=this;
		pDrawingBOM->m_bNeedAdjust=FALSE;	//�����Ҫ���е���
		pDrawingBOM->m_eClassType=DRAW_COM_BOM;
		pDrawingBOM->m_eBOMType=BOM_SEGBOLT;
		pDrawingBOM->m_sName.Copy("��˨��ϸ��");
		pDrawingBOM->CreateCmdList();
		//
		int nCount=0;
		CDrawingComponent *ComponentPtrArr[4]={NULL};
		for(pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
		{
			if(pView->m_iViewType==0&&stricmp(pView->defaultLayer,"SPQ")==0&&pView->filter.ThirdCharSet[0]!='?')
			{
				AddComponentByView(pView,NULL,TRUE);	//ǰ��ͼ����ͼ
				pComponent=AddComponentByView(pView,NULL,FALSE);	//ǰ��ͼ�ṹͼ
				if(pComponent&&pComponent->m_eClassType==DRAW_COM_STRUVIEW)
				{
					if(nCount<3)
					{
						ComponentPtrArr[nCount]=pComponent;
						nCount++;
					}
				}
			}
			else if(pView->m_iViewType==1&&((SEGI(pView->folderName)).iSeg==nSegI.iSeg||pView->GetSegI().iSeg==nSegI.iSeg))
				AddComponentByView(pView,NULL,FALSE);	//������ͼ�ṹͼ
		}
		//������ͼ,�����ڶ�����ͼ֮�� wht 11-07-26
		for(int i=0;i<nCount;i++)
			InitPartDrawing((CStruView*)ComponentPtrArr[i]);
	}
}

void CDrawing::InsertMapFrm()		//����ͼ��
{
	InitDrawingEnv();
	//��õ�ǰͼ�εĿ��ָ��
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	DRAGSET.ClearEntSet();
	BOOL bSucceed=FALSE;
	bSucceed=InsertBlock(pBlockTableRecord,dcs.origin,m_xMapFrm.sMapFrameName,1.0,0,0);
	if(!bSucceed)//�Զ���ͼ��
	{
		int i=1,nUnitSize=100;
		f3dPoint topLeft=GetMapFrmInsideTopLeft();
		//�ڲ�ͼ��
		CreateAcadRect(pBlockTableRecord,topLeft,m_xMapFrm.nLength-m_xMapFrm.nLeftMargin-m_xMapFrm.nRightMargin
			,m_xMapFrm.nWidth-m_xMapFrm.nTopMargin-m_xMapFrm.nBottomMargin
			,FALSE,NULL,f3dPoint(0,0,0),sys.stru_map.fPolyLineWidth);
		double txt_height=min(m_xMapFrm.nTopMargin,m_xMapFrm.nBottomMargin)*0.5;
		for(i=1;i<=m_xMapFrm.GetInsideLen()/nUnitSize;i++)
		{	//�ϲ�̶���
			f3dPoint start(topLeft.x+i*nUnitSize,topLeft.y);
			f3dPoint end(start.x,topLeft.y+m_xMapFrm.nTopMargin);
			f3dPoint dim_pos(start.x-0.5*nUnitSize,start.y+0.5*txt_height);
			CreateAcadLine(pBlockTableRecord,start,end);
			CXhChar16 dim_txt("%d",i);
			DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
					txt_height,0,AcDb::kTextCenter,AcDb::kTextBottom);
			//�²�̶���
			start.y-=(m_xMapFrm.GetInsideWidth()+m_xMapFrm.nBottomMargin);
			end.y=start.y+m_xMapFrm.nBottomMargin;
			dim_pos.y=start.y+0.5*txt_height;
			CreateAcadLine(pBlockTableRecord,start,end);
			DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
					txt_height,0,AcDb::kTextCenter,AcDb::kTextBottom);
		}
		txt_height=min(m_xMapFrm.nLeftMargin,m_xMapFrm.nRightMargin)*0.5;
		for(i=1;i<=m_xMapFrm.GetInsideWidth()/nUnitSize;i++)
		{	//���̶���
			f3dPoint start(topLeft.x,topLeft.y-i*nUnitSize);
			f3dPoint end(start.x-m_xMapFrm.nLeftMargin,start.y);
			f3dPoint dim_pos(start.x-0.5*txt_height,start.y+0.5*nUnitSize);
			CreateAcadLine(pBlockTableRecord,start,end);
			CXhChar16 dim_txt("%c",(int)('A'+i-1));
			DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
				txt_height,0,AcDb::kTextRight,AcDb::kTextVertMid);
			//�Ҳ�̶���
			start.x+=m_xMapFrm.GetInsideLen();
			end.x=start.x+m_xMapFrm.nRightMargin;
			dim_pos.x=start.x+0.5*txt_height;
			CreateAcadLine(pBlockTableRecord,start,end);
			DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
				txt_height,0,AcDb::kTextLeft,AcDb::kTextVertMid);
		}
		//���ͼ��
		topLeft.x-=m_xMapFrm.nLeftMargin;
		topLeft.y+=m_xMapFrm.nTopMargin;
		CreateAcadRect(pBlockTableRecord,topLeft,m_xMapFrm.nLength,m_xMapFrm.nWidth
			,FALSE,NULL,f3dPoint(0,0,0),sys.stru_map.fPolyLineWidth);
	}
	pBlockTableRecord->close();
	if(m_bSpecifyMapFrmPos)
	{
		f3dPoint basepnt;
		DragEntSet(basepnt,"��������");
		dcs.origin=basepnt;
		m_bSpecifyMapFrmPos=FALSE;
		//����ͼԪ����ϵ
		CDrawingComponent *pComponent=NULL;
		for(pComponent=GetFirstDrawingComponent();pComponent;pComponent=GetNextDrawingComponent())
		{
			if(pComponent->m_eClassType==DRAW_COM_LINEVIEW||pComponent->m_eClassType==DRAW_COM_STRUVIEW)
				pComponent->CalInitAssemblyPosition(); //�����ʼװ��λ��
		}
	}
	//��ʼ����������
	drawingRectArr.InitDrawingRectArray(m_xMapFrm.nLength,m_xMapFrm.nWidth,m_fComponentSpace);
	ads_command(RTSTR,"RE",RTNONE);
	acedCommand(RTSTR,"zoom",RTSTR,"a",RTNONE);
}

//����ָ��ͼֽ�ж�����ͼ������ͼ����
static void DimSpecifyDrawingSectionViewSymbol(CDrawing *pDrawing,CStruView *pMainView)
{
	if(pDrawing==NULL||pMainView==NULL)
		return;
	CDrawingComponent *pComponent=NULL;
	for(pComponent=pDrawing->GetFirstDrawingComponent();pComponent;pComponent=pDrawing->GetNextDrawingComponent())
	{
		if(pComponent->m_eClassType!=DRAW_COM_STRUVIEW)
			continue;
		if(pComponent->m_pAttachView==NULL||pComponent->m_pAttachView->m_iViewType!=1)
			continue;	//һ�������ֻ�ж�����ͼ����Ҫ��ע����ͼ��
		//�Զ����ŵ�ָ���������������ʾ����ͼ�еĹ���������ͼ�е�λ��
		SCOPE_STRU scope;
		scope.ClearScope();
		scope.fMinX = scope.fMinY = scope.fMinZ = 100000.0;
		scope.fMaxX = scope.fMaxY = scope.fMaxZ =-100000.0;
		AcDbObjectId *pEntId=NULL;
		for(pEntId=pComponent->keyEntIdList.GetFirst();pEntId;pEntId=pComponent->keyEntIdList.GetNext())
		{
			AcDbEntity *pEnt=NULL;
			long h=pComponent->keyEntIdList.GetCursorKey();
			CLDSLinePart *pLinePart=(CLDSLinePart*)Ta.FromPartHandle(h,CLS_LINEPART);
			if(pLinePart&&pComponent->m_pAttachView)
			{
				f3dPoint line_vec=pLinePart->End()-pLinePart->Start();
				normalize(line_vec);
				if(fabs(line_vec*pComponent->m_pAttachView->ucs.axis_z)>0.70710) //<45��
					continue;
			}
			pEnt=pMainView->FindCADEntByHandle(pComponent->keyEntIdList.GetCursorKey());
			if(pEnt==NULL)
				continue;
			if(pEnt->isKindOf(AcDbLine::desc()))
			{
				AcDbLine *pLine=(AcDbLine*)pEnt;
				scope.VerifyVertex(f3dPoint(pLine->startPoint().x,pLine->startPoint().y,0));
				scope.VerifyVertex(f3dPoint(pLine->endPoint().x,pLine->endPoint().y,0));
			}
			else if(pEnt->isKindOf(AcDbPoint::desc()))
			{
				AcDbPoint *pPoint=(AcDbPoint*)pEnt;
				scope.VerifyVertex(f3dPoint(pPoint->position().x,pPoint->position().y,0));
			}
		}
		double width=scope.fMaxX-scope.fMinX;
		double height=scope.fMaxY-scope.fMinY;
		if(fabs(fabs(width)-200000)>EPS||fabs(fabs(width)-200000)>EPS)
		{
			ZoomAcadView(scope,100);
			actrTransactionManager->flushGraphics();
			acedUpdateDisplay();
		}
		//��������ͼ��ʼ���Լ������
		CSectionViewSymbol::AddSectionViewSymbol(pMainView,(CStruView*)pComponent);
	}
}

//��ע����ͼ��
void CDrawing::DimSectionViewSymbol()
{
	CDrawingComponent *pComponent=NULL;
	CStruView *pMainView=NULL;
	for(pComponent=GetFirstDrawingComponent();pComponent;pComponent=GetNextDrawingComponent())
	{
		if(pComponent->m_eClassType==DRAW_COM_STRUVIEW&&pComponent->m_pAttachView
			&&pComponent->m_pAttachView->m_iViewType==0)
		{
			pMainView=(CStruView*)pComponent;
			break;
		}
	}
	if(pMainView==NULL||sectionViewSymbols.GetNodeNum()>0)
		return;
	DimSpecifyDrawingSectionViewSymbol(this,pMainView);
	//�ڴ˴���ע��ͼֽ������ͼ������ͼ���������ײ�����ʱע�͵� wht 11-08-04
	/*
	//�����뵱ǰͼֽ�κ�һ����������ͼ(ͼ����ͼ)��ͼֽ
	CDrawing *pSonDrawing=NULL;
	for(pSonDrawing=drawingList.GetFirst();pSonDrawing;pSonDrawing=drawingList.GetNext())
	{
		if(pSonDrawing==this)
			continue;
		if(pSonDrawing->m_nSegI!=m_nSegI)
			continue;	//�뵱ǰͼֽ�κŲ�һ��
		pComponent=NULL;
		for(pComponent=pSonDrawing->GetFirstDrawingComponent();pComponent;pComponent=pSonDrawing->GetNextDrawingComponent())
		{
			if(pComponent->m_eClassType!=DRAW_COM_STRUVIEW)
				continue;
			if(pComponent->m_pAttachView&&pComponent->m_pAttachView->m_iViewType==0)
				break;	//��������ͼ
		}
		if(pComponent)
			continue;
		DimSpecifyDrawingSectionViewSymbol(pSonDrawing,pMainView);
	}*/
	//��ʾ����ͼֽ
	f2dRect rect;
	rect.topLeft.Set(dcs.origin.x,dcs.origin.y+m_xMapFrm.nWidth);
	rect.bottomRight.Set(dcs.origin.x+m_xMapFrm.nLength,dcs.origin.y);
	ZoomAcadView(rect,100);
}

//�õ���׼�㼴ͼ���ڲ�������ϽǶ���
f3dPoint CDrawing::GetMapFrmInsideTopLeft()
{
	f3dPoint datum_pos;
	datum_pos=dcs.origin;
	datum_pos.y+=m_xMapFrm.nWidth;
	datum_pos.x+=m_xMapFrm.nLeftMargin;
	datum_pos.y-=m_xMapFrm.nTopMargin;
	return datum_pos;
}

void CDrawing::CreateComponents()
{
	//��չ������Ƽ���ע״̬
	ClearDrawnStates(CDrawingComponent::TYPE_JOINT);
	ClearDrawnStates(CDrawingComponent::TYPE_ROD);
	ClearDrawnStates(CDrawingComponent::TYPE_NODE);
	CDrawingComponent *pComponent=NULL;
	for(pComponent=GetFirstDrawingComponent();pComponent;pComponent=GetNextDrawingComponent())
		pComponent->Create();
}
//ɾ��������CADʵ��
void CDrawing::EraseRelaCadEnt()
{
	CDrawingComponent *pComponent=NULL;
	for(pComponent=GetFirstDrawingComponent();pComponent;pComponent=GetNextDrawingComponent())
		pComponent->EraseRelaCadEnt();
}
/*
void CDrawing::CopyProperty(CDrawing *pDrawing)
{
	if(pDrawing==NULL)
		return;
	m_nSegI=pDrawing->m_nSegI;						//�����κ� -1��ʾ˾��ͼ,-2��ʾ��ͼ
	dcs=pDrawing->dcs;								//����ϵ
	m_nDrawingNumber=pDrawing->m_nDrawingNumber;	//ͼֽ����
	m_xMapFrm=pDrawing->m_xMapFrm;					//ͼ��
	m_sDrawingName.Copy(pDrawing->m_sDrawingName);	//ͼֽ����
	m_sDrawingNo.Copy(pDrawing->m_sDrawingNo);		//ͼ��
	m_bSpecifyMapFrmPos=pDrawing->m_bSpecifyMapFrmPos;	//ָ��ͼ��λ��
	m_hActiveModule=pDrawing->m_hActiveModule;		//��ǰ������߾��
	m_pActiveModule=pDrawing->m_pActiveModule;		//��ǰ�������
	m_bAutoLayoutComponent=pDrawing->m_bAutoLayoutComponent;//�Զ�����
	m_fComponentSpace=pDrawing->m_fComponentSpace;	//������ͼ֮��Ļ�ͼ��϶
	//��ͼ�б�
	listComponents.Empty();
	CDrawingComponent *pComponent=NULL;
	for(pComponent=pDrawing->GetFirstDrawingComponent();pComponent;pComponent=pDrawing->GetNextDrawingComponent())
	{
		long key=pDrawing->listComponents.GetCursorKey();
		CDrawingComponent *pNewCom=AddComponent(key,pComponent->m_eClassType);
		pNewCom->CopyProperty(pComponent);
		//pNewCom->m_pDrawing=this;
	}
	//����ͼ���б�
	sectionViewSymbols.Empty();
	CSectionViewSymbol *pSymbol=NULL;
	for(pSymbol=pDrawing->sectionViewSymbols.GetFirst();pSymbol;pSymbol=pDrawing->sectionViewSymbols.GetNext())
	{
		CSectionViewSymbol *pNewSymbol=sectionViewSymbols.append();
		pNewSymbol->CopyProperty(pSymbol);
	}
	//�Ѳ��þ�������
	drawingRectArr=pDrawing->drawingRectArr;
}
*/
CDrawing& CDrawing::operator = (const CDrawing &drawing)
{
	//CopyProperty(&drawing);
	m_nSegI=drawing.m_nSegI;						//�����κ� -1��ʾ˾��ͼ,-2��ʾ��ͼ
	dcs=drawing.dcs;								//����ϵ
	m_nDrawingNumber=drawing.m_nDrawingNumber;	//ͼֽ����
	m_xMapFrm=drawing.m_xMapFrm;					//ͼ��
	m_sDrawingName.Copy(drawing.m_sDrawingName);	//ͼֽ����
	m_sDrawingNo.Copy(drawing.m_sDrawingNo);		//ͼ��
	m_bSpecifyMapFrmPos=drawing.m_bSpecifyMapFrmPos;	//ָ��ͼ��λ��
	m_hActiveModule=drawing.m_hActiveModule;		//��ǰ������߾��
	m_pActiveModule=drawing.m_pActiveModule;		//��ǰ�������
	m_bAutoLayoutComponent=drawing.m_bAutoLayoutComponent;//�Զ�����
	m_fComponentSpace=drawing.m_fComponentSpace;	//������ͼ֮��Ļ�ͼ��϶
	return *this;
}

void CDrawing::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(m_nSegI);	//�����κ� -1��ʾ˾��ͼ,-2��ʾ��ͼ
	buffer.WritePoint(dcs.origin);	//����ϵԭ��
	buffer.WritePoint(dcs.axis_x);	//����ϵX��
	buffer.WritePoint(dcs.axis_y);	//����ϵY��
	buffer.WritePoint(dcs.axis_z);	//����ϵZ��
	buffer.WriteInteger(m_nDrawingNumber);		//ͼֽ����
	buffer.WriteByte(m_xMapFrm.style);			//Ĭ��ͼ������0-9
	buffer.WriteString(m_xMapFrm.sMapFrameName);//ͼ������ A0 A0-2 A1 A1-2 A2 A2-2 A3 A3-2 A4 A4-2
	buffer.WriteInteger(m_xMapFrm.nLength);		//ͼ����
	buffer.WriteInteger(m_xMapFrm.nWidth);		//ͼ�򳤶�
	buffer.WriteWord(m_xMapFrm.nLeftMargin);	//����϶
	buffer.WriteWord(m_xMapFrm.nRightMargin);	//�Ҳ��϶
	buffer.WriteWord(m_xMapFrm.nTopMargin);		//�ϲ��϶
	buffer.WriteWord(m_xMapFrm.nBottomMargin);	//�²��϶
	buffer.WriteWord(m_xMapFrm.nBottomRightFixLen);		//���½ǹ̶�����
	buffer.WriteWord(m_xMapFrm.nBottomRightFixWidth);	//���½ǹ̶����
	buffer.WriteString(m_sDrawingName);			//ͼֽ����
	buffer.WriteString(m_sDrawingNo);			//ͼ��
	buffer.WriteInteger(m_bSpecifyMapFrmPos);	//ָ��ͼ��λ��
	buffer.WriteInteger(m_hActiveModule);		//��ǰ����ģ����
	buffer.WriteInteger(m_bAutoLayoutComponent);//�Զ�����
	buffer.WriteDouble(m_fComponentSpace);		//������ͼ���
	//��ͼ����
	CDrawingComponent *pComponent=NULL;
	buffer.WriteInteger(listComponents.GetNodeNum());
	for(pComponent=GetFirstDrawingComponent();pComponent;pComponent=GetNextDrawingComponent())
	{
		long key=listComponents.GetCursorKey();
		buffer.WriteInteger(key);
		buffer.WriteInteger(pComponent->m_eClassType);
		pComponent->ToBuffer(buffer);
	}
	//����ͼ����
	CSectionViewSymbol *pViewSymbol=NULL;
	buffer.WriteInteger(sectionViewSymbols.GetNodeNum());
	for(pViewSymbol=sectionViewSymbols.GetFirst();pViewSymbol;pViewSymbol=sectionViewSymbols.GetNext())
		pViewSymbol->ToBuffer(buffer);
}
void CDrawing::FromBuffer(CBuffer &buffer, long version/*=NULL*/)
{
	buffer.ReadInteger(&m_nSegI.iSeg);	//�����κ� -1��ʾ˾��ͼ,-2��ʾ��ͼ
	buffer.ReadPoint(dcs.origin);	//����ϵԭ��
	buffer.ReadPoint(dcs.axis_x);	//����ϵX��
	buffer.ReadPoint(dcs.axis_y);	//����ϵY��
	buffer.ReadPoint(dcs.axis_z);	//����ϵZ��
	buffer.ReadInteger(&m_nDrawingNumber);	//ͼֽ����
	buffer.ReadByte(&m_xMapFrm.style);			//Ĭ��ͼ������0-9
	if (version>=1000021)
		buffer.ReadString(m_xMapFrm.sMapFrameName,100);//ͼ������ A0 A0-2 A1 A1-2 A2 A2-2 A3 A3-2 A4 A4-2
	buffer.ReadInteger(&m_xMapFrm.nLength);		//ͼ����
	buffer.ReadInteger(&m_xMapFrm.nWidth);		//ͼ�򳤶�
	buffer.ReadWord(&m_xMapFrm.nLeftMargin);	//����϶
	buffer.ReadWord(&m_xMapFrm.nRightMargin);	//�Ҳ��϶
	buffer.ReadWord(&m_xMapFrm.nTopMargin);		//�ϲ��϶
	buffer.ReadWord(&m_xMapFrm.nBottomMargin);	//�²��϶
	buffer.ReadWord(&m_xMapFrm.nBottomRightFixLen);		//���½ǹ̶�����
	buffer.ReadWord(&m_xMapFrm.nBottomRightFixWidth);	//���½ǹ̶����
	buffer.ReadString(m_sDrawingName,50);		//ͼֽ����
	buffer.ReadString(m_sDrawingNo,50);			//ͼ��
	buffer.ReadInteger(&m_bSpecifyMapFrmPos);	//ָ��ͼ��λ��
	buffer.ReadInteger(&m_hActiveModule);		//��ǰ����ģ����
	buffer.ReadInteger(&m_bAutoLayoutComponent);//�Զ�����
	buffer.ReadDouble(&m_fComponentSpace);		//������ͼ���
	//��ͼ����
	listComponents.Empty();
	long key=0,i=0,nCount=0,nType=0,buffer_len=buffer.GetLength();
	buffer.ReadInteger(&nCount);
	for(i=0;i<nCount;i++)
	{
		if(Ta.DisplayProcess)
			Ta.DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),"��ͼ�ű���ȡ����");
		buffer.ReadInteger(&key);		//��ֵ
		buffer.ReadInteger(&nType);		//��ͼ����
		CDrawingComponent* pComponent=(CDrawingComponent*)listComponents.Add(key,nType);
		//pComponent->m_pDrawing=this;
		pComponent->FromBuffer(buffer,version);
	}
	//����ͼ����
	sectionViewSymbols.Empty();
	long nSymbolNum=0;
	buffer.ReadInteger(&nSymbolNum);
	for(i=0;i<nSymbolNum;i++)
	{
		CSectionViewSymbol *pViewSymbol=sectionViewSymbols.append();
		pViewSymbol->FromBuffer(buffer,version);
		if(version<1000006)
		{	//��ǰ�汾��¼������ͼλ��Ϊ�����ͼ�����Ͻǵ�λ�ã��ڴ˴�ת��Ϊ�������� wht 11-07-25
			f3dPoint datum_pos=GetMapFrmInsideTopLeft();
			pViewSymbol->startPt+=datum_pos;
			pViewSymbol->endPt+=datum_pos;
		}
	}
}
void CDrawing::ClearDrawnStates(int stateType/*=TYPE_ROD*/)
{
	for(CDrawingComponent *pComponent=(CDrawingComponent*)listComponents.GetFirst();pComponent;pComponent=(CDrawingComponent*)listComponents.GetNext())
		pComponent->ClearDrawnStates(stateType);
}
bool CDrawing::IsItemDrawn(int stateType,long idItem)	//����ָ����Ŀ�Ƿ��ڵ�ǰͼֽ�л��ƹ�
{
	for(CDrawingComponent *pComponent=(CDrawingComponent*)listComponents.GetFirst();pComponent;pComponent=(CDrawingComponent*)listComponents.GetNext())
	{
		if(pComponent->IsItemDrawn(stateType,idItem))
			return true;
	}
	return false;
}
bool CDrawing::IsItemDimmed(int stateType,long idItem)	//����ָ����Ŀ�Ƿ��ڵ�ǰͼֽ�б�ע��
{
	for(CDrawingComponent *pComponent=(CDrawingComponent*)listComponents.GetFirst();pComponent;pComponent=(CDrawingComponent*)listComponents.GetNext())
	{
		if(pComponent->IsItemDimmed(stateType,idItem))
			return true;
	}
	return false;
}
void CDrawing::MarkItemDrawnState(CDrawingComponent *pComponent,int stateType,long idItem,bool drawn/*=true*/)
{
	if(pComponent)
		pComponent->MarkItemDrawnState(stateType,idItem,drawn);
}
void CDrawing::MarkItemDimSpecState(CDrawingComponent *pComponent,int stateType,long idItem,bool dimmed/*=true*/)
{
	if(pComponent)
		pComponent->MarkItemDimSpecState(stateType,idItem,dimmed);
}

//���������ɱ�����¼��㵱ǰ����ʼλ��
void CDrawing::UpdateBOMTableInitPos(CDrawingBOM *pDrawingBOM)
{
	CDrawingComponent *pComponent=NULL;
	f2dRect rect;
	double fMaxHeight=m_xMapFrm.GetInsideWidth();
	CDrawingBOM *pPrevBOM=NULL;
	for(pComponent=GetFirstDrawingComponent();pComponent;pComponent=GetNextDrawingComponent())
	{
		if(pComponent->m_eClassType!=DRAW_COM_BOM)
			continue;
		CDrawingBOM *pBOM=(CDrawingBOM*)pComponent;
		if(pPrevBOM&&pDrawingBOM==pBOM)
		{
			CDrawingCommand *pPrevFirstCmd=pPrevBOM->cmdList.GetFirst();
			if(pPrevFirstCmd==NULL||pPrevFirstCmd->m_eCmdType!=CMD_DRAW_TABLE)
				continue;
			double fTableWidth=pPrevFirstCmd->m_paraBOMTable.fTableWidth;
			double fTableHeight=pPrevFirstCmd->m_paraBOMTable.fTableHeight;
			f3dPoint table_pos(pPrevFirstCmd->m_paraBOMTable.fDimPosX,pPrevFirstCmd->m_paraBOMTable.fDimPosY);
			if(pPrevFirstCmd->m_paraBOMTable.nLocatePos==0)		//����
			{
				rect.topLeft=table_pos;
				rect.bottomRight.Set(table_pos.x+fTableWidth,table_pos.y-fTableHeight);
			}
			else if(pPrevFirstCmd->m_paraBOMTable.nLocatePos==1)//����
			{
				rect.topLeft.Set(table_pos.x-fTableWidth,table_pos.y);
				rect.bottomRight.Set(table_pos.x,table_pos.y-fTableHeight);
			}
			else if(pPrevFirstCmd->m_paraBOMTable.nLocatePos==2)//����
			{
				rect.topLeft.Set(table_pos.x,table_pos.y+fTableHeight);
				rect.bottomRight.Set(table_pos.x+fTableWidth,table_pos.y);
			}
			else if(pPrevFirstCmd->m_paraBOMTable.nLocatePos==3)//����
			{
				rect.topLeft.Set(table_pos.x-fTableWidth,table_pos.x+fTableHeight);
				rect.bottomRight=table_pos;
			}
			else 
				continue;
			//��ǰ���λ��
			CDrawingCommand *pFirstCmd=pBOM->cmdList.GetFirst();
			if(pFirstCmd==NULL||pFirstCmd->m_eCmdType!=CMD_DRAW_TABLE)
				continue;
			f3dPoint topRight;
			if(fMaxHeight+rect.bottomRight.y>pFirstCmd->m_paraBOMTable.fTableHeight)
				topRight.Set(rect.bottomRight.x,rect.bottomRight.y);
			else 
				topRight.Set(rect.topLeft.x,rect.topLeft.y);
			if(pFirstCmd->m_paraBOMTable.nLocatePos==0)		//����
			{
				pFirstCmd->m_paraBOMTable.fDimPosX=topRight.x-pFirstCmd->m_paraBOMTable.fTableWidth;
				pFirstCmd->m_paraBOMTable.fDimPosY=topRight.y;
			}
			else if(pFirstCmd->m_paraBOMTable.nLocatePos==1)//����
			{
				pFirstCmd->m_paraBOMTable.fDimPosX=topRight.x;
				pFirstCmd->m_paraBOMTable.fDimPosY=topRight.y;
			}
			else if(pFirstCmd->m_paraBOMTable.nLocatePos==2)//����
			{
				pFirstCmd->m_paraBOMTable.fDimPosX=topRight.x-pFirstCmd->m_paraBOMTable.fTableWidth;
				pFirstCmd->m_paraBOMTable.fDimPosY=topRight.y-pFirstCmd->m_paraBOMTable.fTableHeight;
			}
			else if(pFirstCmd->m_paraBOMTable.nLocatePos==3)//����
			{
				pFirstCmd->m_paraBOMTable.fDimPosX=topRight.x;
				pFirstCmd->m_paraBOMTable.fDimPosY=topRight.y-pFirstCmd->m_paraBOMTable.fTableHeight;
			}
			break;
		}
		else if(pDrawingBOM==pBOM)
			break;
		pPrevBOM=pBOM;
	}
}


CString CDrawing::MakeComponentSetStr()
{
	CString strComponent="";
	CDrawingComponent *pComponent=NULL;
	int i=0,nCount=listComponents.GetNodeNum();
	for(pComponent=GetFirstDrawingComponent();pComponent;pComponent=GetNextDrawingComponent())
	{
		CString ss="";
		ss.Format("%d.",i);
		strComponent+=ss;
		strComponent+=pComponent->m_sName;
		if(i<nCount-1)
			strComponent+='|';
		i++;
	}
	return strComponent;
}

void PreparePartSpecList(ATOM_LIST<SPEC_MARK> &specList);
void CDrawing::UpdateSimpleSpecList()
{
	CDrawingBOM* pDrawingBom=NULL;
	for(CDrawingComponent* pComponent=GetFirstDrawingComponent();pComponent;pComponent=GetNextDrawingComponent())
	{
		if(pComponent->m_eClassType==DRAW_COM_BOM&&((CDrawingBOM*)pComponent)->m_eBOMType==BOM_GUIGE)
		{
			pDrawingBom=(CDrawingBOM*)pComponent;
			break;
		}
	}
	if(pDrawingBom&&pDrawingBom->SpecList.GetNodeNum()<=0)
		PreparePartSpecList(pDrawingBom->SpecList);		//�򻯵Ĺ���б�(˾��ͼ��ע�˼����ʱʹ��)
	dimpartspec.SpecList.Empty();
	if(pDrawingBom)
	{	//�����ܻ�ͼ�����еļ򻯹���ͬ����ȫ�ֹ�����ϸ��ע����
		for(SPEC_MARK *pMark=pDrawingBom->SpecList.GetFirst();pMark;pMark=pDrawingBom->SpecList.GetNext())
		{
			SPEC_MARK *pTmpMark = dimpartspec.SpecList.append();
			pTmpMark->N=pMark->N;
			strcpy(pTmpMark->spec,pMark->spec);
			pTmpMark->wide = pMark->wide;
			pTmpMark->thick = pMark->thick;
			pTmpMark->cMark = pMark->cMark;
		}
	}
}
void CDrawing::UpdatePtrByHandle()
{
	CDrawingComponent *pComponent=NULL;
	for(pComponent=GetFirstDrawingComponent();pComponent;pComponent=GetNextDrawingComponent())
		pComponent->UpdatePtrByHandle();
}

//����ͼֽ������ͼԪ�������б� wht 11-07-31
void CDrawing::UpdateCmdList()
{
	CDrawingComponent *pDrawCom=NULL;
	if(m_nSegI.iSeg==-1||m_nSegI.iSeg>=0)
	{	//˾��ͼ��ṹͼ�����ݵ�ǰ���ж�����ͼ�б����ͼֽ�ж�����ͼ
		CHashList<CDrawingComponent*> hashViewList;
		for(pDrawCom=GetFirstDrawingComponent();pDrawCom;pDrawCom=GetNextDrawingComponent())
		{
			if(pDrawCom->m_eClassType!=DRAW_COM_STRUVIEW&&pDrawCom->m_eClassType!=DRAW_COM_LINEVIEW)
				continue;	//�ǵ���ͼ�ͽṹͼ
			if(pDrawCom->m_pAttachView==NULL)
				pDrawCom->m_pAttachView=Ta.View.FromHandle(pDrawCom->m_hAttachView);
			if(pDrawCom->m_pAttachView==NULL)
				listComponents.DeleteCursor();	//ɾ���޶�Ӧ��ͼ��ͼԪ
			else if(pDrawCom->m_pAttachView->m_iViewType==1)
				hashViewList.SetValue(pDrawCom->m_hAttachView,pDrawCom);
		}
		CDisplayView *pView=NULL;
		for(pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
		{
			if(pView->m_iViewType!=1)
				continue;
			if(m_nSegI.iSeg>=0)
			{	//�ṹͼ���ж��Ƿ�Ϊ��ǰ�ζ�����ͼ
				if(pView->GetSegI().iSeg!=m_nSegI.iSeg&&SEGI(pView->folderName).iSeg!=m_nSegI.iSeg)
					continue;	//�ǵ�ǰ�ζ�����ͼ
				if(!hashViewList.GetValue(pView->handle))	//�ö�����ͼ�޶�ӦͼԪ��Ҫ���
				{
					pDrawCom=AddComponentByView(pView,NULL,FALSE);	
					pDrawCom->feature=-1;	//������ͼԪ
				}
			}
			else if(m_nSegI.iSeg==-1&&!hashViewList.GetValue(pView->handle))
			{
				pDrawCom=AddComponentByView(pView,NULL,TRUE);		//����˾��ͼ������ͼ
				pDrawCom->feature=-1;	//������ͼԪ
			}
		}
	}
	for(pDrawCom=GetFirstDrawingComponent();pDrawCom;pDrawCom=GetNextDrawingComponent())
	{
		if(pDrawCom->m_eClassType!=DRAW_COM_STRUVIEW&&pDrawCom->m_eClassType!=DRAW_COM_LINEVIEW)
			continue;	//�ǵ���ͼ�ͽṹͼ
		if(pDrawCom->feature!=-1)
			pDrawCom->UpdateCmdList();	//����ͼԪ��Ӧ�������б�
	}
}

//////////////////////////////////////////////////////////////////////////
//CDrawingComponent
CDrawingComponent::CDrawingComponent(int id/*=0*/,IDrawing* pDrawing/*=NULL*/) : CXeroDrawingComponent(id,pDrawing)
{
	//m_pDrawing=NULL;
	m_hAttachView=0;
	m_pAttachView=NULL;
	m_hModule=0;
	m_pModule=NULL;
	m_eClassType=DRAW_COM_OTHER;
	m_nAnchorType=TOP_LEFT;
	drawing_offset.Set();
	m_bSpecifyComPos=TRUE;
	m_bNeedAdjust=TRUE;
	m_bFirstCreate=TRUE;
	m_bDimScale=TRUE;
	m_bDimName=TRUE;
	m_fScale=1;
	m_fNamePosSpace=15;
	LoadDefaultUCS(&dcs);
	feature=0;
}

CDrawingComponent::~CDrawingComponent()
{

}

//�ڼ�����ͼ��ʵ��ͼ����ʱͨ��bOnlyIncludePartEnt���ƽ����㹹����������(��ȡ���Ʊ�עλ��ʱʹ��)
f2dRect CDrawingComponent::GetDrawRegion(BOOL bRealRect/*=TRUE*/,BOOL bOnlyIncludePartEnt/*=FALSE*/)
{
	f2dRect rect;
	SCOPE_STRU scope;
	scope.ClearScope();
	scope.fMinX = scope.fMinY = scope.fMinZ = 100000.0;
	scope.fMaxX = scope.fMaxY = scope.fMaxZ =-100000.0;
	if(bRealRect)
	{	//���ݻ��Ƴ���CADʵ�������ʵ�Ļ�ͼ����
		AcDbObjectId *pEntId=NULL;
		CDrawingCommand *pCmd=NULL;
		for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
		{
			if(bOnlyIncludePartEnt&&pCmd->m_eCmdType!=CMD_DRAW_LINEPART&&pCmd->m_eCmdType!=CMD_DRAW_PART)
				continue;	//�����㹹����ͼ����
			for(pEntId=pCmd->entIdList.GetFirst();pEntId;pEntId=pCmd->entIdList.GetNext())
				VerifyVertexByCADEntId(scope,*pEntId);
		}
		//����ʵ�弯(�ϽӶκź���ͼ���ƶ�Ӧ��ʵ��)
		CAD_OBJECT_ID *pObjId=NULL;
		for(pObjId=entIdList.GetFirst();pObjId;pObjId=entIdList.GetNext())
			VerifyVertexByCADEntId(scope,pObjId->entId);
		if(ParentDrawing())
		{	//�����ڵ�ǰͼԪ������ͼ���Ŷ�Ӧ��ʵ�弯 
			CSectionViewSymbol *pSymbol=NULL;
			long key=ParentDrawing()->listComponents.GetKey(this);
			for(pSymbol=ParentDrawing()->sectionViewSymbols.GetFirst();pSymbol;pSymbol=ParentDrawing()->sectionViewSymbols.GetNext())
			{
				if(pSymbol->hAttachViewKey==key)
				{
					for(pEntId=pSymbol->startEntIds.GetFirst();pEntId;pEntId=pSymbol->startEntIds.GetNext())
						VerifyVertexByCADEntId(scope,*pEntId);
					for(pEntId=pSymbol->endEntIds.GetFirst();pEntId;pEntId=pSymbol->endEntIds.GetNext())
						VerifyVertexByCADEntId(scope,*pEntId);
				}
			}
		}
		rect.topLeft.Set(scope.fMinX,scope.fMaxY);
		rect.bottomRight.Set(scope.fMaxX,scope.fMinY);
	}
	else 
	{	//���ݹ��������Լ���ͼ���������ŵĻ�ͼ����
		if(nodeSet.GetNodeNum()<=0||partSet.GetNodeNum()<=0)
			GetDrawingObjectSet(FALSE);	//�����б�Ϊ��ʱ���³�ʼ�������б�
		CLDSNode *pNode=NULL;
		for(pNode=nodeSet.GetFirst();pNode!=NULL;pNode=nodeSet.GetNext())
		{
			f3dPoint pos=pNode->Position(false);
			coord_trans(pos,dcs,FALSE);
			scope.fMaxX = __max(scope.fMaxX, pos.x);
			scope.fMaxY = __max(scope.fMaxY, pos.y);
			scope.fMinX = __min(scope.fMinX, pos.x);
			scope.fMinY = __min(scope.fMinY, pos.y);
		}
		if(ParentDrawing()->m_nSegI.iSeg>0)
		{
			CLDSPart *pPart=NULL;
			for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
			{
				if(pPart->IsLinePart())
				{
					CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
					f3dPoint pos=pLinePart->Start();
					coord_trans(pos,dcs,FALSE);
					scope.fMaxX = __max(scope.fMaxX, pos.x);
					scope.fMaxY = __max(scope.fMaxY, pos.y);
					scope.fMinX = __min(scope.fMinX, pos.x);
					scope.fMinY = __min(scope.fMinY, pos.y);
					pos=pLinePart->End();
					coord_trans(pos,dcs,FALSE);
					scope.fMaxX = __max(scope.fMaxX, pos.x);
					scope.fMaxY = __max(scope.fMaxY, pos.y);
					scope.fMinX = __min(scope.fMinX, pos.x);
					scope.fMinY = __min(scope.fMinY, pos.y);
				}
			}
		}
		if(m_eClassType==DRAW_COM_PARTVIEW)
		{	//����ͼ
			CPartDrawing *pPartDrawing=(CPartDrawing*)this;
			f3dPoint origin;
			double fWidth=0,fLength=0;
			if(pPartDrawing->m_bPolyPart&&pPartDrawing->polyPart.segset.GetNodeNum()>0)
			{
				origin=pPartDrawing->polyPart.segset.GetFirst()->ucs.origin;
				fWidth=pPartDrawing->polyPart.GetWidth()*m_fScale;
				fLength=pPartDrawing->polyPart.GetLength()*m_fScale;
			}
			else if(pPartDrawing->m_pPart) 
			{
				origin=pPartDrawing->m_pPart->ucs.origin;
				fWidth=pPartDrawing->m_pPart->GetWidth()*m_fScale;
				fLength=pPartDrawing->m_pPart->GetLength()*m_fScale;
			}
			coord_trans(origin,dcs,FALSE);
			origin*=m_fScale;
			rect.topLeft.Set(origin.x,origin.y);
			rect.bottomRight.Set(origin.x+fWidth,origin.y-fLength);
			scope.fMinX=rect.topLeft.x;
			scope.fMaxY=rect.topLeft.y;
			scope.fMaxX=rect.bottomRight.x;
			scope.fMinY=rect.bottomRight.y;
		}
		else
		{
			rect.topLeft.Set(scope.fMinX*m_fScale,scope.fMaxY*m_fScale);
			rect.bottomRight.Set(scope.fMaxX*m_fScale,scope.fMinY*m_fScale);
			//����ģ�ͳߴ��ע����������ͼ����
			if(m_eClassType==DRAW_COM_LINEVIEW)
			{
				double fMaxLeftSpace=0,fMaxTopSpace=0;
				CDrawingCommand *pCmd=NULL;
				for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
				{
					if(pCmd->m_eCmdType!=CMD_DIM_MODELSIZE)
						continue;
					if(pCmd->m_paraDimModelSize.iDimType>=4)
					{	//ˮƽ��ע
						double fTopSpace=pCmd->m_paraDimModelSize.fSpace+pCmd->m_paraDimModelSize.fDimDistance;
						if(fTopSpace>fMaxTopSpace)
							fMaxTopSpace=fTopSpace;
					}
					else
					{	//��ֱ��ע
						double fLeftSpace=pCmd->m_paraDimModelSize.fSpace+pCmd->m_paraDimModelSize.fDimDistance;
						if(fLeftSpace>fMaxLeftSpace)
							fMaxLeftSpace=fLeftSpace;
					}
				}
				rect.topLeft.x-=(fMaxLeftSpace*m_fScale);
				rect.topLeft.y+=(fMaxTopSpace*m_fScale);
			}
		}
	}
	double width=scope.fMaxX-scope.fMinX;
	if(fabs(fabs(width)-200000)<EPS)	//������
		rect.topLeft=rect.bottomRight=f3dPoint(0,0);
	draw_rect=rect;
	return rect;
}

//
f2dRect CDrawingComponent::GetComponentRect()
{
	if(draw_rect.Width()<EPS||draw_rect.Height()<EPS)
		GetDrawRegion(FALSE);	//��ʼ����ͼ����
	if(draw_rect.Width()<EPS||draw_rect.Height()<EPS)
		GetDrawRegion(TRUE);	//��ʵ�Ļ�ͼ����
	return draw_rect;
}  
//������ͼ����Ҫ���»�ͼ����
int CDrawingComponent::UpdateDrawingRectArray()
{
	f3dPoint datum_pos=GetMapFrmInsideTopLeft();
	f2dRect real_rect=GetDrawRegion(TRUE);
	f3dPoint topLeft(real_rect.topLeft.x-datum_pos.x,real_rect.topLeft.y-datum_pos.y);
	return ParentDrawing()->drawingRectArr.AddUsedDrawingRect(topLeft,real_rect.Width(),real_rect.Height());
}
//������ͼ
void CDrawingComponent::Create()
{
	if(ParentDrawing()==NULL)
		return;
}
//ɾ����ǰ��ͼ��������
void CDrawingComponent::EraseRelaCadEnt()
{
	//ɾ������ʵ��
	CAD_OBJECT_ID *pObjId=NULL;
	for(pObjId=entIdList.GetFirst();pObjId;pObjId=entIdList.GetNext())
	{	//ɾ���и��������ɵ�ʵ��
		AcDbEntity *pEnt=NULL;
		if(acdbOpenObject(pEnt,pObjId->entId,AcDb::kForWrite)!=Acad::eOk)
			continue;
		if(pEnt==NULL)
			continue;
		pEnt->erase();
		pEnt->close();
	}
	entIdList.Empty();	//��ո���ʵ���б�
	//ɾ��������ص�ʵ��
	CDrawingCommand *pCommand=NULL;
	for(pCommand=cmdList.GetFirst();pCommand;pCommand=cmdList.GetNext())
		pCommand->EmptyEntList();
	//ɾ���ؼ�ʵ�弯(��Ҫɾ����չ���ݵ�)
	AcDbObjectId *pEntId=NULL;
	for(pEntId=keyEntIdList.GetFirst();pEntId;pEntId=keyEntIdList.GetNext())
	{	
		AcDbEntity *pEnt=NULL;
		if(acdbOpenObject(pEnt,*pEntId,AcDb::kForWrite)!=Acad::eOk)
			continue;
		if(pEnt==NULL)
			continue;
		pEnt->erase();
		pEnt->close();
	}
	keyEntIdList.Empty();	//��ո������������ʵ���б�
	//ɾ�������ڵ�ǰͼԪ������ͼ����Ӧ��CADʵ�� wht 11-08-01
	if(ParentDrawing())
	{
		CSectionViewSymbol *pSymbol=NULL;
		long key=ParentDrawing()->listComponents.GetKey(this);
		for(pSymbol=ParentDrawing()->sectionViewSymbols.GetFirst();pSymbol;pSymbol=ParentDrawing()->sectionViewSymbols.GetNext())
		{
			if(pSymbol->hAttachViewKey==key)
				pSymbol->EraseRelativeAcadEntity();
		}
	}
	keyEntIdList.Empty();
	entIdList.Empty();
	//ˢ�½���
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
}
//ɾ����ǰ��ͼ����������������
void CDrawingComponent::ReCreate()
{	//��չ������Ƽ���ע״̬
	hashJointDrawnStates.Empty();
	hashNodesDrawn.Empty();
	hashJointDrawnStates.Empty();
	//ɾ����ǰ��ͼ��������
	EraseRelaCadEnt();
	//��������
	Create();
	//ˢ�½���
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
	//����ѡ��������Ŵ�
	f2dRect rect=GetDrawRegion(TRUE);
	ZoomAcadView(rect,50);
}
//�õ���׼�㼴ͼ���ڲ�������ϽǶ���
f3dPoint CDrawingComponent::GetMapFrmInsideTopLeft()
{
	f3dPoint datum_pos;
	if(ParentDrawing())
		datum_pos=ParentDrawing()->GetMapFrmInsideTopLeft();
	return datum_pos;
}
//��Ҫ�ƶ���ͼ�ڷ�λ��
BOOL CDrawingComponent::IsNeedSpecifyComPos()
{
	if(ParentDrawing()->m_bAutoLayoutComponent)
		return FALSE;
	else 
		return m_bSpecifyComPos;
}
//���ݹ�������õ�ָ����CADʵ��
AcDbEntity *CDrawingComponent::FindCADEntByHandle(long hObj)
{
	AcDbEntity *pEnt=NULL;
	AcDbObjectId *pEntId=NULL;
	pEntId=keyEntIdList.GetValue(hObj);
	if(pEntId)
		acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
	if(pEnt)
		pEnt->close();
	return pEnt;
}

void CDrawingComponent::CopyProperty(CDrawingComponent* pCom)
{
	if(pCom->m_eClassType!=m_eClassType)
		return;
	//m_pDrawing=pCom->m_pDrawing;		//����ͼֽ������ֵ����
	m_hAttachView=pCom->m_hAttachView;	//������ͼ���
	m_pAttachView=pCom->m_pAttachView;	//������ͼ
	m_hModule=pCom->m_hModule;			//�������߾��
	m_pModule=pCom->m_pModule;			//˾��ͼ����ͼ�л��Ʒǵ�ǰ�������ȵ���ͼ
	m_sDrawingLayer.Copy(pCom->m_sDrawingLayer);
	m_fNamePosSpace=pCom->m_fNamePosSpace;
	
	drawing_offset=pCom->drawing_offset;//ƫ����
	draw_rect=pCom->draw_rect;
	dcs=pCom->dcs;						//��ͼ����ϵ		
	m_sName.Copy(pCom->m_sName);		//��ͼ����
	m_eClassType=pCom->m_eClassType;	//��ͼ���� 
	m_nAnchorType=pCom->m_nAnchorType;	//��λ���� 0.���� 1.���� 2.���� 3.����
	m_bSpecifyComPos=pCom->m_bSpecifyComPos;//ָ����ͼλ��
	m_bNeedAdjust=pCom->m_bNeedAdjust;	//����ͼ��Ҫ����
	m_bFirstCreate=pCom->m_bFirstCreate;
	m_fScale=pCom->m_fScale;			//���Ʊ���
	//��ͼ��������
	cmdList.Empty();
	CDrawingCommand *pCommand=NULL;
	for(pCommand=pCom->cmdList.GetFirst();pCommand;pCommand=pCom->cmdList.GetNext())
	{
		CDrawingCommand *pNew=cmdList.append();
		pNew->CopyProperty(pCommand);
		pNew->m_pDrawingCom=this;	//�����������������ͼ		
	}
	//��Ӧ��CAD��ʵ��ID,��ע������ϸʱʹ��
	keyEntIdList.Empty();
	AcDbObjectId *pObjID=NULL;
	for(pObjID=pCom->keyEntIdList.GetFirst();pObjID;pObjID=pCom->keyEntIdList.GetNext())
	{
		long key=pCom->keyEntIdList.GetKey(pObjID);
		keyEntIdList.SetValue(key,*pObjID);
	}
	//�����б�
	partSet.Empty();
	partHandleList.Empty();
	CLDSPart *pPart=NULL;
	for(pPart=pCom->GetFirstPart();pPart;pPart=pCom->GetNextPart())
	{
		partSet.append(pPart);
		partHandleList.append(pPart->handle);
	}
	//�ڵ��б�
	nodeSet.Empty();
	nodeHandleList.Empty();
	CLDSNode *pNode=NULL;
	for(pNode=pCom->GetFirstNode();pNode;pNode=pCom->GetNextNode())
	{
		nodeSet.append(pNode);
		nodeHandleList.append(pNode->handle);
	}
}

//�޸Ķ�λ�����λ��
void CDrawingComponent::ModifyAnchorPointRelativePos(double *pOffsetX,double *pOffsetY)
{
	double fOffsetX=0,fOffsetY=0;
	if(pOffsetX)
		fOffsetX=*pOffsetX;
	if(pOffsetY)
		fOffsetY=*pOffsetY;
	if(pOffsetX==NULL&&pOffsetY==NULL)
		return;
	drawing_offset.x+=fOffsetX;
	drawing_offset.y+=fOffsetY;
	//�����������
	CDrawingCommand *pCommand=NULL;
	for(pCommand=cmdList.GetFirst();pCommand;pCommand=cmdList.GetNext())
		pCommand->UpdateCommandPara(fOffsetX,fOffsetY);
	if(m_eClassType==DRAW_COM_STRUVIEW)
	{	//��������ͼ��λ��
		long key=ParentDrawing()->listComponents.GetKey(this);
		CSectionViewSymbol *pViewSymbol=NULL;
		for(pViewSymbol=ParentDrawing()->sectionViewSymbols.GetFirst();pViewSymbol;pViewSymbol=ParentDrawing()->sectionViewSymbols.GetNext())
		{
			if(pViewSymbol->hAttachViewKey==key)//���µ�ǰ��ͼ������ͼ��λ�ò���
				pViewSymbol->UpdatePosition(fOffsetX,fOffsetY);
		}
	}
}
//���ö�λ�������ͼ�����Ͻǵ����λ��
f3dPoint CDrawingComponent::GetAnchorPointRelativePos()
{
	f3dPoint anchor_point;
	f2dRect rect=GetDrawRegion();
	if(rect.Width()<EPS||rect.Height()<EPS)
		return anchor_point;
	//���㶨λ�������ͼ�����Ͻǵ�����
	if(m_nAnchorType==TOP_LEFT)
		anchor_point.Set(rect.topLeft.x,rect.topLeft.y,0);
	else if(m_nAnchorType==TOP_RIGHT)
		anchor_point.Set(rect.bottomRight.x,rect.topLeft.y,0);
	else if(m_nAnchorType==BOTTOM_LEFT)
		anchor_point.Set(rect.topLeft.x,rect.bottomRight.y,0);
	else //if(m_nAnchorType==BOTTOM_RIGHT)
		anchor_point.Set(rect.bottomRight.x,rect.bottomRight.y,0);
	//
	f3dPoint datum_pos=GetMapFrmInsideTopLeft();
	anchor_point-=datum_pos;
	return anchor_point;
}

/*
//ͨ�����λ�ü���װ��λ��
void CDrawingComponent::CalAssemblyPosByRelativePos()
{
	//ʹ�����λ�����¼�������ϵԭ��λ��
	f3dPoint datum_pos=GetMapFrmInsideTopLeft();
	dcs.origin=datum_pos+m_ptLocation;	
}
*/
//������ͼ��ʼװ��λ�� 
//����ͼ���Լ���λ���͸ı����Ҫ���¼����ʼװ��λ��
void CDrawingComponent::CalInitAssemblyPosition()
{
	f2dRect rect;
	rect=GetDrawRegion(FALSE);
	double fRectHeight=rect.Height();
	double fRectWidth=rect.Width();
	double fMinX=rect.topLeft.x;	
	double fMaxY=rect.topLeft.y;
	f3dPoint datum_pos=GetMapFrmInsideTopLeft();
	//�ڻ�ͼ���ο������в���һ�����õĿհ�����
	BOOL bSideView=FALSE;
	if(m_pAttachView&&stricmp(m_pAttachView->defaultLayer,"SPY")==0)
		bSideView=TRUE;
	if(m_hModule==0||bSideView||(m_hModule>0x20&&m_hModule!=ParentDrawing()->m_hActiveModule))
	{
		f3dPoint topLeft;
		ParentDrawing()->drawingRectArr.GetBlankRectTopLeft(fRectWidth,fRectHeight,&topLeft);
		rect.topLeft.Set(topLeft.x,topLeft.y);
		rect.topLeft.x+=datum_pos.x;
		rect.topLeft.y+=datum_pos.y;
	}
	else
	{
		rect.topLeft=datum_pos;
		rect.topLeft.x+=ParentDrawing()->m_fComponentSpace;
		rect.topLeft.y-=ParentDrawing()->m_fComponentSpace;
	}
	rect.topLeft.x-=fMinX;
	rect.topLeft.y-=fMaxY;
	rect.bottomRight.x=rect.topLeft.x+fRectWidth;
	rect.bottomRight.y=rect.topLeft.y-fRectHeight;
	f3dPoint offset_pos;
	if(m_nAnchorType==TOP_LEFT)
		offset_pos.Set(rect.topLeft.x,rect.topLeft.y,0);
	else if(m_nAnchorType==TOP_RIGHT)
		offset_pos.Set(rect.bottomRight.x,rect.topLeft.y,0);
	else if(m_nAnchorType==BOTTOM_LEFT)
		offset_pos.Set(rect.topLeft.x,rect.bottomRight.y,0);
	else //if(m_nAnchorType==BOTTOM_RIGHT)
		offset_pos.Set(rect.bottomRight.x,rect.bottomRight.y,0);
	//��ͼĬ�ϻ��㵽Ŀ������ƫ��ʸ��
	drawing_offset=offset_pos;	
}
//ָ����ͼλ��
f3dPoint CDrawingComponent::SpecifyAssemblyPosition()
{
	f3dPoint offset;
	if(!IsNeedSpecifyComPos())
	{
		if(m_bSpecifyComPos)
			m_bSpecifyComPos=FALSE;
		//����ƫ��λ��ƽ��ʵ�弯
		/*f3dPoint datum_pos=GetMapFrmInsideTopLeft();
		f3dPoint cur_pos=datum_pos+m_ptLocation;
		if(cur_pos!=dcs.origin)
		{
			TranslationEntSet(dcs.origin,cur_pos,DRAGSET.drag_ent_set);
			dcs.origin=cur_pos;	//��������ϵԭ��λ�� wht 11-06-25
		}*/
		return offset;
	}
	//����ָ��λ�ø�����ͼλ���Լ���ͼ�����ͼ�����Ͻǵ�λ��
	f3dPoint basepnt;
	//��һ��������ͼ��Ҫָ�������
	f2dRect rect=GetDrawRegion();
	if(m_nAnchorType==TOP_LEFT)
		basepnt.Set(rect.topLeft.x,rect.topLeft.y,0);
	else if(m_nAnchorType==TOP_RIGHT)
		basepnt.Set(rect.bottomRight.x,rect.topLeft.y,0);
	else if(m_nAnchorType==BOTTOM_LEFT)
		basepnt.Set(rect.topLeft.x,rect.bottomRight.y,0);
	else //if(m_nAnchorType==BOTTOM_RIGHT)
		basepnt.Set(rect.bottomRight.x,rect.bottomRight.y,0);
	//
	f3dPoint old_pos=basepnt;
	f3dPoint datum_pos=GetMapFrmInsideTopLeft();
	acedCommand(RTSTR,"zoom",RTSTR,"e",RTNONE);
	CXhChar100 sPrompt("������[%s]�����\n",&m_sName);
	int nRetCode=DragEntSet(basepnt,sPrompt);
	if(nRetCode==RTNORM)
	{	//��ͼλ�ñ䶯�����Ҫ��������ϵλ��
		offset.x=basepnt.x-old_pos.x;
		offset.y=basepnt.y-old_pos.y;
		//�Ϸ���ͼλ�ú�ͬʱ����ƫ��ʸ�� wht 11-06-28
		drawing_offset.x+=offset.x;
		drawing_offset.y+=offset.y;
	}
	m_bSpecifyComPos=FALSE;
	return offset;
}

//������ͼ����
void CDrawingComponent::InsertDrawingName()
{
	if(entIdList.GetNodeNum()>0)
	{	//ɾ��������Ƽ��ϽӶκŶ�Ӧ��ʵ�� wht 11-07-31
		CAD_OBJECT_ID *pCADObjId=NULL;
		for(pCADObjId=entIdList.GetFirst();pCADObjId;pCADObjId=entIdList.GetNext())
		{	
			AcDbEntity *pEnt=NULL;
			if(acdbOpenObject(pEnt,pCADObjId->entId,AcDb::kForWrite)!=Acad::eOk)
				continue;
			if(pEnt==NULL)
				continue;
			pEnt->erase();
			pEnt->close();
		}
		entIdList.Empty();
		//���½���
		actrTransactionManager->flushGraphics(); //�رմ򿪵�ʵ���ſɳɹ�ˢ�»���
		acedUpdateDisplay();
	}
	entIdList.Empty();
	//1.�����עλ��
	f3dPoint dim_pos;
	//��ʼ����עλ��
	f2dRect rect=GetDrawRegion(TRUE,TRUE);
	if(rect.Width()<EPS||rect.Height()<EPS)
		return;	//��ͼ����ΪNULL �����ע��ͼ����
	dim_pos.Set((rect.topLeft.x+rect.bottomRight.x)*0.5,rect.bottomRight.y-m_fNamePosSpace);
	double fOutspreadLen=0.5;	//�»������������
	CXhChar50 sComponentName(m_sName);
	if(sComponentName.Length()==0&&m_pAttachView&&m_pAttachView->m_iViewType==1)
		sComponentName.Copy(m_pAttachView->name);
	double nLineLen=sComponentName.Length()*sys.dim_map.fCutawayViewSymbolTextSize*0.7+2*fOutspreadLen;
	f3dLine line;
	double nSpace=0.5;		//���»���֮��ļ�϶
	line.startPt.Set(dim_pos.x-0.5*nLineLen,dim_pos.y-sys.dim_map.fCutawayViewSymbolTextSize-nSpace);
	line.endPt.Set(dim_pos.x+0.5*nLineLen,line.startPt.y);
	//2.������ͼ���Ʊ�ע
	AcDbObjectId entId=0;
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	if(m_eClassType==DRAW_COM_PARTVIEW)
	{	
		CPartDrawing *pPartDrawing=(CPartDrawing*)this;
		if(pPartDrawing->m_pPart==NULL)
			pPartDrawing->m_pPart=Ta.FromPartHandle(pPartDrawing->m_hPart);
		/*if(pPartDrawing->m_pPart)
		{	//����ͼ��ע����ʱ������Ȧ
			CString sName=m_sName;
			CString sPartNo=pPartDrawing->m_pPart->sPartNo;
			CString sSumName=sName.Left(sPartNo.GetLength());
			if(sPartNo.CompareNoCase(sSumName)==0)
			{
				f3dPoint center=dim_pos;
				CreateAcadCircle(pBlockTableRecord,center,5);
			}
		}*/
	}
	if (m_bDimName)
	{
		entId=DimText(pBlockTableRecord,dim_pos,sComponentName,TextStyleTable::hzfs.textStyleId,
			sys.dim_map.fCutawayViewSymbolTextSize,0,AcDb::kTextCenter,AcDb::kTextTop);
		entIdList.append(CAD_OBJECT_ID(1,entId));
		entId=CreateAcadPolyLine(pBlockTableRecord,line.startPt,line.endPt,sys.dim_map.fCutawayViewPolyLineWidth);
		entIdList.append(CAD_OBJECT_ID(1,entId));
		line.startPt.y-=nSpace;
		line.endPt.y-=nSpace;
		entId=CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
		entIdList.append(CAD_OBJECT_ID(1,entId));
	}
	//3.��ע��ͼ����
	if (m_bDimScale)
	{
		f3dPoint scale_dim_pos=dim_pos;
		scale_dim_pos.y=line.endPt.y-nSpace;
		CXhChar50 sDrawScale("1:%d",ftoi(1.0/m_fScale));
		entId=DimText(pBlockTableRecord,scale_dim_pos,sDrawScale,TextStyleTable::hzfs.textStyleId,
			sys.dim_map.fCutawayViewSymbolTextSize,0,AcDb::kTextCenter,AcDb::kTextTop);
		entIdList.append(CAD_OBJECT_ID(1,entId));

	}
	//4.��ע�ϽӶκ�
	if(m_eClassType==DRAW_COM_LINEVIEW	//����ͼ
		&&((m_pModule&&m_pModule->handle!=ParentDrawing()->m_hActiveModule)||ParentDrawing()->m_nSegI.iSeg>0))	//�Ǽ�����߻�ֶε���ͼ
	{
		//������߽ڵ���Xֵ��С���Ľڵ�
		CLDSNode *pNode=NULL,*pHighestNode[2]={NULL};
		pHighestNode[0]=pHighestNode[1]=nodeSet.GetFirst();
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{
			if(pNode->Position('Z',false)<pHighestNode[0]->Position('Z',false))
				pHighestNode[0]=pHighestNode[1]=pNode;
			if(fabs(pNode->Position('Z',false)-pHighestNode[0]->Position('Z',false))<EPS)
			{	//Z�������
				if(pNode->Position('X',false)>pHighestNode[1]->Position('X',false))
					pHighestNode[1]=pNode;
				if(pNode->Position('X',false)<pHighestNode[0]->Position('X',false))
					pHighestNode[0]=pNode;
			}
		}
		SEGI nUpSegI;
		NODESET sum_nodeset;
		LINEPARTSET sum_partset;
		CLDSModule *pModule=m_pModule;
		if(pModule==NULL&&ParentDrawing()->m_nSegI.iSeg>0)
		{	//���ҵ�ǰ���������߷���
			CLDSPart *pPart=partSet.GetFirst();
			for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
			{
				if(pModule->IsSonPart(pPart))
					break;
			}
		}
		if(pModule&&pHighestNode[0]&&pHighestNode[1])
		{
			pModule->RefreshModule(sum_nodeset,sum_partset);
			for(CLDSLinePart *pLinePart=sum_partset.GetFirst();pLinePart;pLinePart=sum_partset.GetNext())
			{
				//if(!pLinePart->cfgword.IsEqual(CFGWORD(pModule->m_iBodyLegNo)))
				//	continue;	
				if(pLinePart->pStart==pHighestNode[0]||pLinePart->pStart==pHighestNode[1]
					||pLinePart->pEnd==pHighestNode[0]||pLinePart->pEnd==pHighestNode[1])
				{
					if(pLinePart->iSeg.iSeg==ParentDrawing()->m_nSegI.iSeg)
						continue;	//��ǰ�ι���
					nUpSegI=pLinePart->iSeg;
					break;
				}
			}
			if(nUpSegI.iSeg!=-1)
			{
				dim_pos.Set(0.5*(pHighestNode[0]->Position('X',false)+pHighestNode[1]->Position('X',false)),pHighestNode[0]->Position(false).y,pHighestNode[0]->Position('Z',false));
				coord_trans(dim_pos,dcs,FALSE);
				dim_pos.z=0;
				dim_pos*=m_fScale;
				dim_pos+=drawing_offset;
				dim_pos.y+=m_fNamePosSpace;	//��ע��϶
				CXhChar16 sUpSegI;
				sUpSegI.Printf("�Ͻ�  %s  ��",(char*)nUpSegI.ToString());
				entId=DimText(pBlockTableRecord,dim_pos,sUpSegI,TextStyleTable::hzfs.textStyleId,
					sys.dim_map.fCutawayViewSymbolTextSize,0,AcDb::kTextCenter,AcDb::kTextBottom);
				entIdList.append(CAD_OBJECT_ID(2,entId));
				f3dPoint center=dim_pos;
				center.x+=(sys.dim_map.fCutawayViewSymbolTextSize*0.7*0.5);
				center.y+=0.5*sys.dim_map.fCutawayViewSymbolTextSize;
				double radius=0.5*sys.dim_map.fCutawayViewSymbolTextSize+2;
				entId=CreateAcadCircle(pBlockTableRecord,center,radius);
				entIdList.append(CAD_OBJECT_ID(2,entId));
			}
		}
	}
	pBlockTableRecord->close();
}

//���������
void CDrawingComponent::ModifyComponentName(char *com_name)
{
	if(com_name==NULL)
		return;
	CXhChar50 sOldName("%s",&m_sName);
	m_sName.Copy(com_name);
	//
	CAD_OBJECT_ID *pObjId=NULL;
	for(pObjId=entIdList.GetFirst();pObjId;pObjId=entIdList.GetNext())
	{
		if(pObjId->entType!=1)
			continue;
		AcDbEntity *pEnt=NULL;
		if(acdbOpenObject(pEnt,pObjId->entId,AcDb::kForWrite)!=Acad::eOk)
			continue;
		if(pEnt==NULL)
			continue;
		if(pEnt->isKindOf(AcDbText::desc()))
		{
			AcDbText *pText=(AcDbText*)pEnt;
		#ifdef _ARX_2007
			if(stricmp(_bstr_t(pText->textString()),sOldName)==0)
				pText->setTextString(_bstr_t(com_name));
		#else
			if(stricmp(pText->textString(),sOldName)==0)
				pText->setTextString(com_name);
		#endif
		}
		pEnt->close();
	}
	//���½���
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
}

//��ȡ����ʵ�弯
void CDrawingComponent::GetAttachEntSet(ads_name &drag_ent_set,int name1_seg2_all0/*=0*/,
										BOOL bSelectEnts/*=FALSE*/,SCOPE_STRU *pScope/*=NULL*/)
{
	CAD_OBJECT_ID *pObjId=NULL;
	for(pObjId=entIdList.GetFirst();pObjId;pObjId=entIdList.GetNext())
	{
		if(name1_seg2_all0>0&&name1_seg2_all0!=pObjId->entType)
			continue;
		ads_name ent_name;
		if(acdbGetAdsName(ent_name,pObjId->entId)!=Acad::eOk)
			continue;
		if(bSelectEnts)
			ads_redraw(ent_name,3);//������ʾ
		if(pScope)
			VerifyVertexByCADEntId(*pScope,pObjId->entId);
		if(acedSSAdd(ent_name,drag_ent_set,drag_ent_set)!=RTNORM)
			continue;
		struct resbuf *pData=acdbEntGet(ent_name);
		acdbEntMod(pData);
		acdbEntUpd(ent_name);
	}
}

//�����ͼ��Ӧ��ʵ�弯��
void CDrawingComponent::GetEntSet(ads_name &drag_ent_set,BOOL bSelectEnts/*=FALSE*/)
{
	//��ȡ�������ʵ�弯
	CDrawingCommand *pCommand=NULL;
	for(pCommand=cmdList.GetFirst();pCommand;pCommand=cmdList.GetNext())
		pCommand->GetEntSet(drag_ent_set,bSelectEnts);
	//��ø���ʵ�弯
	GetAttachEntSet(drag_ent_set,0,bSelectEnts);
}
//
void CDrawingComponent::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(m_hAttachView);	//������ͼ���
	buffer.WriteInteger(m_hModule);		//�������߾��
	buffer.WriteString(m_sDrawingLayer);//ͼ�㷶Χ
	buffer.WriteDouble(m_fNamePosSpace);//��ͼ��������ͼ�ײ��ļ�϶ֵ
	buffer.WritePoint(dcs.origin);		//��ͼ����ϵԭ��
	buffer.WritePoint(dcs.axis_x);		//��ͼ����ϵX��
	buffer.WritePoint(dcs.axis_y);		//��ͼ����ϵY��
	buffer.WritePoint(dcs.axis_z);		//��ͼ����ϵZ��
	buffer.WriteString(m_sName);		//��ͼ����
	buffer.WriteString(m_sSectionViewCode);	//����ͼ����
	buffer.WriteInteger(m_eClassType);	//��ͼ���� 
	buffer.WriteInteger(m_nAnchorType);	//��λ���� 0.���� 1.���� 2.���� 3.����
	buffer.WritePoint(drawing_offset);	//ƫ����
	buffer.WriteInteger(m_bSpecifyComPos);//ָ����ͼλ��
	buffer.WriteInteger(m_bNeedAdjust);	//����ͼ��Ҫ����
	buffer.WriteInteger(m_bFirstCreate);//��һ������
	buffer.WriteDouble(m_fScale);		//���Ʊ���
	buffer.WriteInteger(m_bDimName);	//��ע��ͼ����
	buffer.WriteInteger(m_bDimScale);	//��ע���Ʊ���
	//��ͼ��������
	buffer.WriteInteger(cmdList.GetNodeNum());
	CDrawingCommand *pCommand=NULL;
	for(pCommand=cmdList.GetFirst();pCommand;pCommand=cmdList.GetNext())
		pCommand->ToBuffer(buffer);
}

void CDrawingComponent::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.ReadInteger(&m_hAttachView);	//������ͼ���
	m_pAttachView=(CDisplayView*)Ta.View.FromHandle(m_hAttachView);
	buffer.ReadInteger(&m_hModule);		//�������߾��
	m_pModule=(CLDSModule*)Ta.Module.FromHandle(m_hModule);
	buffer.ReadString(m_sDrawingLayer,16);//ͼ�㷶Χ
	buffer.ReadDouble(&m_fNamePosSpace);//��ͼ��������ͼ�ײ��ļ�϶ֵ
	buffer.ReadPoint(dcs.origin);		//��ͼ����ϵԭ��
	buffer.ReadPoint(dcs.axis_x);		//��ͼ����ϵX��
	buffer.ReadPoint(dcs.axis_y);		//��ͼ����ϵY��
	buffer.ReadPoint(dcs.axis_z);		//��ͼ����ϵZ��
	buffer.ReadString(m_sName,50);		//��ͼ����
	buffer.ReadString(m_sSectionViewCode,16);	//����ͼ����
	long nType=0;
	buffer.ReadInteger(&nType);			//��ͼ���� 
	m_eClassType=(DRAW_COMPONENT_TYPE)nType;
	buffer.ReadInteger(&nType);			//��λ���� 0.���� 1.���� 2.���� 3.����
	m_nAnchorType=(TBL_LOCATE_STYLE)nType;
	if(version<1000004)
	{
		f3dPoint pt;
		buffer.ReadPoint(pt);		//����ϵԭ�����λ��
		if(version>1000001)
			buffer.ReadPoint(pt);	//��λ�����λ��
	}
	else 
		buffer.ReadPoint(drawing_offset);
	buffer.ReadInteger(&m_bSpecifyComPos);//ָ����ͼλ��
	buffer.ReadInteger(&m_bNeedAdjust);	//����ͼ��Ҫ����
	buffer.ReadInteger(&m_bFirstCreate);//��һ������
	buffer.ReadDouble(&m_fScale);		//���Ʊ���
	if (version>=1000020)
	{
		buffer.ReadInteger(&m_bDimName);
		buffer.ReadInteger(&m_bDimScale);
	}
	//��ͼ��������
	CDrawingCommand *pCommand=NULL;
	cmdList.Empty();
	long i=0,nCount=0,buffer_len=buffer.GetLength();
	buffer.ReadInteger(&nCount);
	CHashList<f3dLine>hashOddLine;
	for(i=0;i<nCount;i++)
	{
		if(Ta.DisplayProcess)
			Ta.DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),"��ͼ�ű���ȡ����");
		pCommand=cmdList.append();
		pCommand->m_pDrawingCom=this;
		pCommand->FromBuffer(buffer,version);
		if(version<1000013)
		{	//С��V1000013�İ汾��,����ͷ��עλ���Ǿ�������ҪתΪ������� wjh-2011.9.25
			if(pCommand->m_eCmdType==CMD_DRAW_PART)
			{
				if(pCommand->m_paraPart.pPart&&pCommand->m_paraPart.pPart->IsLinePart())
				{
					CDrawCoordTrans ts;
					pCommand->GetDCS(ts);
					f3dLine *pBaseLine=hashOddLine.Add(pCommand->m_paraPart.hPart);
					pBaseLine->startPt=((CLDSLinePart*)pCommand->m_paraPart.pPart)->Start();
					pBaseLine->endPt=((CLDSLinePart*)pCommand->m_paraPart.pPart)->End();
					pBaseLine->startPt=ts.TransToDraw(pBaseLine->startPt);
					pBaseLine->endPt=ts.TransToDraw(pBaseLine->endPt);
				}
			}
			else if(pCommand->m_eCmdType==CMD_DIM_ODDMENT)
			{	//�����������ж��������ɻ��Ƹ˼�����,Ȼ������ɱ�ע����ͷ����,�ʿ�����ͬһ��ѭ����
				f3dLine *pBaseLine=hashOddLine.GetValue(pCommand->m_paraOddment.hLinePart);
				if(pBaseLine)
				{	//��ԭ�еľ�������ת��������ڱ�ע��������ƫ��������
					pCommand->m_paraOddment.fStartOddPosOffsetX-=pBaseLine->startPt.x;
					pCommand->m_paraOddment.fStartOddPosOffsetY-=pBaseLine->startPt.y;
					pCommand->m_paraOddment.fEndOddPosOffsetX-=pBaseLine->endPt.x;
					pCommand->m_paraOddment.fEndOddPosOffsetY-=pBaseLine->endPt.y;
				}
			}
		}
	}
}

void CDrawingComponent::UpdatePtrByHandle()
{
	m_pAttachView=(CDisplayView*)Ta.View.FromHandle(m_hAttachView);
	m_pModule=(CLDSModule*)Ta.Module.FromHandle(m_hModule);
	nodeSet.Empty();
	long *pH=NULL;
	for(pH=nodeHandleList.GetFirst();pH;pH=nodeHandleList.GetNext())
	{
		CLDSNode *pNode=Ta.FromNodeHandle(*pH);
		if(pNode)
			nodeSet.append(pNode);
	}
	partSet.Empty();
	for(pH=partHandleList.GetFirst();pH;pH=partHandleList.GetNext())
	{
		CLDSPart *pPart=Ta.FromPartHandle(*pH);
		if(pPart)
			partSet.append(pPart);
	}
	CDrawingCommand *pCommand=NULL;
	for(pCommand=cmdList.GetFirst();pCommand;pCommand=cmdList.GetNext())
		pCommand->UpdatePtrByHandle();
}

//��ʼ����������Ӧ��feature wht 11-07-22
void CDrawingComponent::InitCmdFeature(int nFeature)	
{
	CDrawingCommand *pCmd=NULL;
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
		pCmd->feature=nFeature;
}
//����״̬����ע״̬��غ���
void CDrawingComponent::ClearDrawnStates(int stateType/*=TYPE_ROD*/)
{
	if(stateType==TYPE_ROD)
		hashRodDrawnStates.Empty();
	else if(stateType==TYPE_JOINT)
		hashJointDrawnStates.Empty();
}
bool CDrawingComponent::IsItemDrawn(int stateType,long idItem)	//����ָ����Ŀ�Ƿ��ڵ�ǰͼֽ�л��ƹ�
{
	DRAWN_STATE* pState=NULL;
	if(stateType==TYPE_ROD)
		pState=hashRodDrawnStates.GetValue(idItem);
	else if(stateType==TYPE_JOINT)
		pState=hashJointDrawnStates.GetValue(idItem);
	else if(stateType==TYPE_NODE)
		pState=hashNodesDrawn.GetValue(idItem);
	if(pState)
		return pState->drawn;
	else
		return false;
}
bool CDrawingComponent::IsItemDimmed(int stateType,long idItem)	//����ָ����Ŀ�Ƿ��ڵ�ǰͼֽ�б�ע��
{
	DRAWN_STATE* pState=NULL;
	if(stateType==TYPE_ROD)
		pState=hashRodDrawnStates.GetValue(idItem);
	else if(stateType==TYPE_JOINT)
		pState=hashJointDrawnStates.GetValue(idItem);
	else if(stateType==TYPE_NODE)
		pState=hashNodesDrawn.GetValue(idItem);
	if(pState)
		return pState->dimspec;
	else
		return false;
}
void CDrawingComponent::MarkItemDrawnState(int stateType,long idItem,bool drawn/*=true*/)
{
	DRAWN_STATE* pState=NULL;
	if(stateType==TYPE_ROD)
		pState=hashRodDrawnStates.Add(idItem);
	else if(stateType==TYPE_JOINT)
		pState=hashJointDrawnStates.Add(idItem);
	else if(stateType==TYPE_NODE)
		pState=hashNodesDrawn.GetValue(idItem);
	if(pState)
		pState->drawn=drawn;
}
void CDrawingComponent::MarkItemDimSpecState(int stateType,long idItem,bool dimmed/*=true*/)
{
	DRAWN_STATE* pState=NULL;
	if(stateType==TYPE_ROD)
		pState=hashRodDrawnStates.Add(idItem);
	else if(stateType==TYPE_JOINT)
		pState=hashJointDrawnStates.Add(idItem);
	else if(stateType==TYPE_NODE)
		pState=hashNodesDrawn.GetValue(idItem);
	if(pState)
		pState->dimspec=dimmed;
}
//////////////////////////////////////////////////////////////////////////
//CDrawingBOM
CDrawingBOM::CDrawingBOM(int id/*=0*/,IDrawing* pDrawing/*=NULL*/) : CDrawingComponent(id,pDrawing)
{
	m_eClassType=DRAW_COM_BOM;	//��ͼ����
	m_eBOMType=(DRAW_BOM_TYPE)0;//�������
	m_nAnchorType=TOP_RIGHT;
	m_bAutoNewTable=TRUE;	//������ʱ�Զ������µı��
	m_bHasTableHead=TRUE;	//�±���Ƿ����ͷ
	m_nBriefGuigeN=min(sys.nBriefGuigeN,26);//����ܳ���26
}

CDrawingBOM::~CDrawingBOM()
{
	
}
//������ͼ����Ҫ���»�ͼ����
int CDrawingBOM::UpdateDrawingRectArray()
{
	int iRect=-1;
	CDrawingCommand *pCmd=NULL;
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DRAW_TABLE)
			continue;
		f3dPoint datum_pos(pCmd->m_paraBOMTable.fDimPosX,pCmd->m_paraBOMTable.fDimPosY);
		f2dPoint topLeft;
		if(pCmd->m_paraBOMTable.nLocatePos==0)
			topLeft.Set(datum_pos.x,datum_pos.y);
		else if(pCmd->m_paraBOMTable.nLocatePos==1)
			topLeft.Set(datum_pos.x-pCmd->m_paraBOMTable.fTableWidth,datum_pos.y);
		else if(pCmd->m_paraBOMTable.nLocatePos==2)
			topLeft.Set(datum_pos.x,datum_pos.y+pCmd->m_paraBOMTable.fTableHeight);
		else if(pCmd->m_paraBOMTable.nLocatePos==3)
			topLeft.Set(datum_pos.x-pCmd->m_paraBOMTable.fTableWidth,datum_pos.y+pCmd->m_paraBOMTable.fTableHeight);
		iRect=ParentDrawing()->drawingRectArr.AddUsedDrawingRect(topLeft,pCmd->m_paraBOMTable.fTableWidth,pCmd->m_paraBOMTable.fTableHeight);
	}
	return iRect;
}
//������ͼ
void CDrawingBOM::Create()
{
	DRAGSET.ClearEntSet();
	CDrawingCommand *pFirstCmd=cmdList.GetFirst();
	CDrawingCommand *pSecCmd=cmdList.GetNext();
	if(pFirstCmd==NULL)
		return;
	if(pFirstCmd->m_eCmdType!=CMD_DRAW_TABLE)
		return;
	f3dPoint location_pt=drawing_offset-GetMapFrmInsideTopLeft();
	pFirstCmd->m_paraBOMTable.bSelectPos=m_bSpecifyComPos;
	pFirstCmd->m_paraBOMTable.nLocatePos=m_nAnchorType;
	pFirstCmd->m_paraBOMTable.fDimPosX=location_pt.x;
	pFirstCmd->m_paraBOMTable.fDimPosY=location_pt.y;
	pFirstCmd->Run();
	if(IsNeedSpecifyComPos())
	{	//��һ��������ͼ��Ҫָ�������
		location_pt.x=pFirstCmd->m_paraBOMTable.fDimPosX;
		location_pt.y=pFirstCmd->m_paraBOMTable.fDimPosY;
		drawing_offset=GetMapFrmInsideTopLeft()+location_pt;
	}
	if(pSecCmd)
	{
		pSecCmd->m_paraBOMTable.bSelectPos=FALSE;
		pSecCmd->m_paraBOMTable.nLocatePos=m_nAnchorType;
		pSecCmd->m_paraBOMTable.fDimPosX=location_pt.x-xFormatTbl1.GetTableWide();
		pSecCmd->m_paraBOMTable.fDimPosY=location_pt.y;
		pSecCmd->Run();
	}
	m_bSpecifyComPos=FALSE;
	//����ͼԪ��ͼԪ����������ӵ���ͼ���ο�������
	UpdateDrawingRectArray();
	acedCommand(RTSTR,"zoom",RTSTR,"e",RTNONE);
	ads_command(RTSTR,"RE",RTNONE);
}

//���±���ʼλ��
void CDrawingBOM::UpdateTableInitPos()
{
	CDrawingCommand *pFirstCmd=cmdList.GetFirst();
	CDrawingCommand *pSecCmd=cmdList.GetNext();
	if(pFirstCmd==NULL)
		return;
	if(pFirstCmd->m_eCmdType!=CMD_DRAW_TABLE)
		return;
	//��ʼ�����λ��
	if(m_nAnchorType==TOP_RIGHT)
	{
		drawing_offset=ParentDrawing()->GetMapFrmInsideTopLeft();
		drawing_offset.x+=ParentDrawing()->m_xMapFrm.GetInsideLen();
		pFirstCmd->m_paraBOMTable.fDimPosX=ParentDrawing()->m_xMapFrm.GetInsideLen();
		pFirstCmd->m_paraBOMTable.fDimPosY=0;
		if(pSecCmd&&xFormatTbl2.GetRowsCount()>0)
		{	//�ڶ��ű������ڵ�һ�ű�����
			pFirstCmd->m_paraBOMTable.fDimPosX=ParentDrawing()->m_xMapFrm.GetInsideLen()-xFormatTbl1.GetTableWide();
			pFirstCmd->m_paraBOMTable.fDimPosY=0;
		}
	}
}
double FindGenKaiDistance(CLDSModule* pModule,CDisplayView* pFrontView)
{
	//1,��ȡ��ǰ����ǰ��ͼ�Ľڵ㼯��
	NODESET sumNodeSet,nodeSet;
	LINEPARTSET linePartSet;
	pModule->RefreshModule(sumNodeSet,linePartSet);
	for(CLDSNode* pNode=sumNodeSet.GetFirst();pNode;pNode=sumNodeSet.GetNext())
	{	
		if(pFrontView->filter.IsValidLayer(pNode->layer()))
			nodeSet.append(pNode);
	}
	//2,��ȡ����¿��������ڵ�
	double dist=0;
	CLDSNode *pLowestNode = nodeSet.GetFirst();
	for(CLDSNode *pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
	{	
		if(pNode->m_cPosCalType==4||pNode->m_cPosCalType==5)
			continue;
		if(pNode->Position('Z',false)>pLowestNode->Position('Z',false))
			pLowestNode = pNode;
		else if(pNode->Position('Z',false)==pLowestNode->Position('Z',false))
		{
			if(pNode->Position('X',false)<pLowestNode->Position('X',false))
				pLowestNode = pNode;
		}
	}
	CLDSNode *pLowestSecNode;
	for(CLDSNode *pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
	{
		if(pNode->m_cPosCalType==4||pNode->m_cPosCalType==5)
			continue;
		if(pNode->Position('Z',false)==pLowestNode->Position('Z',false)&&pNode!=pLowestNode)
		{
			pLowestSecNode=pNode;
			break;
		}
	}
	if(pLowestNode && pLowestSecNode)
	{
		f3dPoint start=pLowestNode->Position(true);
		f3dPoint end=pLowestSecNode->Position(true);
		dist=fabs(start.x-end.x);
	}
	return dist;
}
//From SegMatTable.cpp
BOOL PrepareSegPartTable(CFormatTbl &xPartTbl,SEGI nSegI=-1);	//Ԥ��׼���ֶι�����ϸ������
void PrepareSegBoltTable(CFormatTbl &xPartTbl,SEGI nSegI=-1);	//Ԥ��׼���ֶ���˨��ϸ������
//From SumMap.cpp
void PrepareSumMatTable(CFormatTbl &xPartTbl);		//Ԥ��׼�����ϻ��ܱ�����
//From TsaFunc.cpp
void PreparePartSpecList(ATOM_LIST<SPEC_MARK> &specList);//Ԥ��׼���򻯵Ĺ���б�(˾��ͼ��ע�˼����ʱʹ��)
void PreparePartSpecBriefTbl(ATOM_LIST<SPEC_MARK> &specList,CFormatTbl &xSpecTbl,int nBriefGuigeN);//Ԥ��׼���˼����򻯱���ʽ
void PrepareGenKaiTable(CFormatTbl &xPartTbl)		//Ԥ��׼����������������
{
	int nBodyN=Ta.Module.GetNodeNum();
	int i=0,j=0;
	xPartTbl.Create(3+nBodyN,7);
	for(i=0;i<7;i++)
	{
		xPartTbl.GetGridAt(0,i)->textsize = sys.dim_map.fHeaderTextSize;
		xPartTbl.GetGridAt(1,i)->textsize = sys.dim_map.fCommonTextSize;
		xPartTbl.GetGridAt(2,i)->textsize = sys.dim_map.fCommonTextSize;
	}
	for(i=1;i<3;i++)
	{
		xPartTbl.GetGridAt(i,0)->textsize = sys.dim_map.fCommonTextSize;
		xPartTbl.GetGridAt(i,1)->textsize = sys.dim_map.fCommonTextSize;
	}
	//���ñ����
	xPartTbl.SetRowHigh(0,16);		//������
	xPartTbl.SetRowHigh(1,8);		//��������,����������
	xPartTbl.SetRowHigh(2,8);		//���桪������
	//for(i=3;i<nRowsN-1;i++)			//������
	//	xPartTbl.SetRowHigh(i,6);
	for(i=0;i<5;i++)
		xPartTbl.SetColumnWide(i,20);	
	xPartTbl.SetColumnWide(5,30);	//�׽���˨����
	xPartTbl.SetColumnWide(6,40);	//�׽���˨ֱ��������
	//���������
	for(i=1;i<7;i++)//ˮƽ�ϲ���һ���е����б��Ԫ
		xPartTbl.GetGridAt(0,i)->m_bHGridVirtual = TRUE;
	xPartTbl.GetGridAt(0,0)->data.SetGridString("����������");
	xPartTbl.GetGridAt(0,0)->feature = 1;	//����Գ�MC
	//����--���߱��Ԫ
	xPartTbl.GetGridAt(2,0)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,0)->data.SetGridString("����");
	xPartTbl.GetGridAt(1,0)->feature = 1;	//����Գ�MC
	//����--�����������Ԫ
	xPartTbl.GetGridAt(1,2)->m_bHGridVirtual = TRUE;//ˮƽ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,1)->feature = 1;
	xPartTbl.GetGridAt(1,1)->data.SetGridString("�������� 2Lt");
	//����--�����������Ԫ
	xPartTbl.GetGridAt(1,4)->m_bHGridVirtual = TRUE;//ˮƽ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,3)->feature = 1;
	xPartTbl.GetGridAt(1,3)->data.SetGridString("�������� 2Lj");
	//����--�׽���˨�������Ԫ
	xPartTbl.GetGridAt(2,5)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,5)->feature = 1;
	xPartTbl.GetGridAt(1,5)->data.SetGridString("�׽���˨����\\PLD");
	//����--�׽���˨ֱ�����������Ԫ
	xPartTbl.GetGridAt(2,6)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	xPartTbl.GetGridAt(1,6)->feature = 1;
	xPartTbl.GetGridAt(1,6)->data.SetGridString("�׽���˨ֱ��������");
	//����--��-������Ԫ
	xPartTbl.GetGridAt(2,1)->data.SetGridString("����");
	xPartTbl.GetGridAt(2,1)->feature=1;
	xPartTbl.GetGridAt(2,2)->data.SetGridString("����");
	xPartTbl.GetGridAt(2,2)->feature=1;
	xPartTbl.GetGridAt(2,3)->data.SetGridString("����");
	xPartTbl.GetGridAt(2,3)->feature=1;
	xPartTbl.GetGridAt(2,4)->data.SetGridString("����");
	xPartTbl.GetGridAt(2,4)->feature=1;
	for(i=4;i<3+nBodyN;i++)
	{
		xPartTbl.GetGridAt(i,5)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
		xPartTbl.GetGridAt(i,6)->m_bVGridVirtual = TRUE;
	}
	xPartTbl.GetGridAt(3,5)->feature = 1;
	xPartTbl.GetGridAt(3,5)->data.SetGridString("");
	xPartTbl.GetGridAt(3,6)->feature = 1;
	xPartTbl.GetGridAt(3,6)->data.SetGridString("");
	CDisplayView *pFrontView=NULL;
	for(CDisplayView *pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
	{
		if(pView->m_iViewType==0&&stricmp(pView->defaultLayer,"SPQ")==0&&pView->filter.ThirdCharSet[0]!='?')
			pFrontView=pView;
	}
	i=3;
	for(CLDSModule *pModule=Ta.Module.GetFirst();pModule&&i<3+nBodyN;pModule=Ta.Module.GetNext(),i++)
	{
		xPartTbl.GetGridAt(i,0)->feature = 1;
		xPartTbl.GetGridAt(i,0)->data.SetGridString(pModule->description);
		double nGenKaiDis=FindGenKaiDistance(pModule,pFrontView);
		char sValue[20]="";
		sprintf(sValue,"%.0f",nGenKaiDis);
		for(j=1;j<5;j++)
		{
			xPartTbl.GetGridAt(i,j)->feature = 1;
			xPartTbl.GetGridAt(i,j)->data.SetGridString(sValue);
		}
	}
}
//׼���������
BOOL CDrawingBOM::PrepareTable()
{
	SEGI nSegI;
	if(ParentDrawing())
		nSegI=ParentDrawing()->m_nSegI;
	else
		return FALSE;
	xFormatTbl1.Destroy();
	xFormatTbl2.Destroy();
	if(m_eBOMType==BOM_SUMPART)
		PrepareSumMatTable(xFormatTbl1);	//���ϻ��ܱ�
	else if(m_eBOMType==BOM_GENKAI)
		PrepareGenKaiTable(xFormatTbl1);	//����������
	else if(m_eBOMType==BOM_SEGPART)
	{	
		if(m_bAutoNewTable)
		{
			CFormatTbl xFormatTbl;
			if(!PrepareSegPartTable(xFormatTbl,nSegI))//�ֶι�����ϸ��
				return FALSE;
			double fMaxHeight=ParentDrawing()->m_xMapFrm.GetTableValidWidth();
			int i,nRow=xFormatTbl.GetRowsCount();
			double height=0;
			for(i=0;i<nRow;i++)
			{
				double h=0;
				xFormatTbl.GetRowHigh(i,&h);
				height+=h;
				if(height>fMaxHeight)
					break;
			}
			if(i<nRow)
			{	//�ֶι�����ϸ����Ҫ��ҳ����	
				xFormatTbl1.CopyFrom(&xFormatTbl,i-1,nRow-1);
				if(m_bHasTableHead)	//��ͷ��3��
					xFormatTbl2.CopyFrom(&xFormatTbl,3,i-2);
				else 
					xFormatTbl2.CopyFrom(&xFormatTbl,0,i-2);
			}
			else 
				xFormatTbl1.CopyFrom(&xFormatTbl);
		}
		else if(!PrepareSegPartTable(xFormatTbl1,nSegI))	//�ֶι�����ϸ��
			return FALSE;
	}
	else if(m_eBOMType==BOM_SEGBOLT)
		PrepareSegBoltTable(xFormatTbl1,nSegI);		//�ֶ���˨��ϸ��
	else if(m_eBOMType==BOM_GUIGE)
	{
		if (SpecList.GetNodeNum()<=0)
			PreparePartSpecList(SpecList);		//�򻯵Ĺ���б�(˾��ͼ��ע�˼����ʱʹ��)
		PreparePartSpecBriefTbl(SpecList,xFormatTbl1,m_nBriefGuigeN);	//�˼����򻯱���ʽ
	}

	else 
		return FALSE;
	return TRUE;
}

//������������
BOOL CDrawingBOM::CreateCmdList(BOOL bInit/*=TRUE*/)
{
	cmdList.Empty();
	
	if(m_eBOMType!=BOM_GUIGE)
	{	//��ʼ��ʱ�ݲ�׼���򻯹�������,��������һ������˾��ͼ�ظ������öԻ��� wjh-2013.2.27
		if(!PrepareTable())
			return FALSE;
	}
	CDrawingCommand *pCommand=cmdList.append();
	pCommand->m_pDrawingCom=this;
	pCommand->m_eCmdType=CMD_DRAW_TABLE;
	pCommand->InitCommandPara();
	pCommand->m_paraBOMTable.nLocatePos=m_nAnchorType;
	pCommand->m_paraBOMTable.iTableType=(int)m_eBOMType;
	pCommand->m_paraBOMTable.bDisplayDlg=FALSE;
	pCommand->m_paraBOMTable.bSelectPos=TRUE;
	pCommand->m_paraBOMTable.iFormatTbl=1;
	if(xFormatTbl2.GetRowsCount()>0)
	{	//��Ҫ��ҳ��ʾ���ʱ�ĵڶ�ҳ
		pCommand=cmdList.append();
		pCommand->m_pDrawingCom=this;
		pCommand->m_eCmdType=CMD_DRAW_TABLE;
		pCommand->InitCommandPara();
		pCommand->m_paraBOMTable.nLocatePos=m_nAnchorType;
		pCommand->m_paraBOMTable.iTableType=(int)m_eBOMType;
		pCommand->m_paraBOMTable.bDisplayDlg=FALSE;
		pCommand->m_paraBOMTable.bSelectPos=FALSE;
		pCommand->m_paraBOMTable.iFormatTbl=2;
	}
	//���±���ʼλ��
	if(bInit)
		UpdateTableInitPos();
	return TRUE;
}

void CDrawingBOM::CopyProperty(CDrawingComponent* pCom)
{
	CDrawingComponent::CopyProperty(pCom);
	if(pCom->m_eClassType==m_eClassType)
	{
		CDrawingBOM *pDrawingBOM=(CDrawingBOM*)pCom;
		m_eBOMType=pDrawingBOM->m_eBOMType;
		m_bAutoNewTable=pDrawingBOM->m_bAutoNewTable;	//������ʱ�Զ������µı��
		m_bHasTableHead=pDrawingBOM->m_bHasTableHead;	//�±���Ƿ����ͷ
		xFormatTbl1.CopyFrom(&pDrawingBOM->xFormatTbl1);
		xFormatTbl2.CopyFrom(&pDrawingBOM->xFormatTbl2);
		m_nBriefGuigeN=pDrawingBOM->m_nBriefGuigeN;		//�򻯹�������˼��򻯱���ʹ�� wht 11-06-15
	}
}

//��ͼ��ͼ����
f2dRect CDrawingBOM::GetComponentRect()
{
	f2dRect rect;
	CDrawingCommand *pFirstCmd=cmdList.GetFirst();
	if(pFirstCmd&&pFirstCmd->m_eCmdType!=CMD_DRAW_TABLE)
	{
		rect.topLeft.Set(0,0);
		rect.bottomRight.Set(pFirstCmd->m_paraBOMTable.fTableWidth,-pFirstCmd->m_paraBOMTable.fTableHeight);
	}
	return rect;
}

void CDrawingBOM::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	CDrawingComponent::FromBuffer(buffer,version);
	long nType=0;
	buffer.ReadInteger(&nType);
	m_eBOMType=(DRAW_BOM_TYPE)nType;
	if(version>=1000017&&m_eBOMType==BOM_GUIGE)	//v1000017
	{	
		long n=0;
		buffer.ReadInteger(&n);
		SPEC_MARK *pMark=NULL;
		CDrawingBOM::SpecList.Empty();
		for(int i=0;i<n;i++)
		{
			pMark=CDrawingBOM::SpecList.append();
			buffer.ReadInteger(&pMark->N);
			buffer.ReadString(pMark->spec,49);
			buffer.ReadDouble(&pMark->wide);
			buffer.ReadDouble(&pMark->thick);
			if(version>=1000018)
				buffer.ReadByte(&pMark->cMark);
			else 
				pMark->cMark='A'+i;
		}
	}
	buffer.ReadInteger(&m_bAutoNewTable);
	buffer.ReadInteger(&m_bHasTableHead);
	if(version>1000002)
		buffer.ReadInteger(&m_nBriefGuigeN);	//1000003 wht 11-06-15
	
	CDrawingCommand *pFirstCmd=cmdList.GetFirst(); 
	CDrawingCommand *pSecCmd=cmdList.GetNext();
	if(pFirstCmd&&pFirstCmd->m_eCmdType==CMD_DRAW_TABLE)
		pFirstCmd->m_paraBOMTable.iFormatTbl=1;
	if(pSecCmd&&pSecCmd->m_eCmdType==CMD_DRAW_TABLE)
		pSecCmd->m_paraBOMTable.iFormatTbl=2;
}

void CDrawingBOM::ToBuffer(CBuffer &buffer)
{
	CDrawingComponent::ToBuffer(buffer);
	buffer.WriteInteger(m_eBOMType);
	if (m_eBOMType==BOM_GUIGE)
	{
		buffer.WriteInteger(CDrawingBOM::SpecList.GetNodeNum());
		for(SPEC_MARK *pMark=CDrawingBOM::SpecList.GetFirst();pMark;pMark=CDrawingBOM::SpecList.GetNext())
		{
			buffer.WriteInteger(pMark->N);
			buffer.WriteString(pMark->spec);
			buffer.WriteDouble(pMark->wide);
			buffer.WriteDouble(pMark->thick);
			buffer.WriteByte(pMark->cMark);
		}
	}
	buffer.WriteInteger(m_bAutoNewTable);
	buffer.WriteInteger(m_bHasTableHead);
	buffer.WriteInteger(m_nBriefGuigeN);		//1000003 wht 11-06-15
}

//////////////////////////////////////////////////////////////////////////
//CLineView
BOOL CLineView::m_bUsePolyLineSumMap=FALSE;	//ʹ�ö���߻�����ͼ
BOOL CLineView::m_bUsePolyLineChiefMap=FALSE;//ʹ�ö���߻���˾��ͼ
CLineView::CLineView(int id/*=0*/,IDrawing* pDrawing/*=NULL*/) : CDrawingComponent(id,pDrawing)
{
	m_eClassType=DRAW_COM_LINEVIEW;	//��ͼ����
	m_fScale=1.0/sys.general_set.LineMapScale;
	m_bAnalysisMode=FALSE;
	m_bDimNodeNo=TRUE;
	m_bDimPoleGuige=TRUE;
}

CLineView::~CLineView()
{
	
}

//��ȡ��Ҫ���ƵĶ��󼯺�
void CLineView::GetDrawingObjectSet(int bInitDCS)
{
	if(ParentDrawing()==NULL||m_pAttachView==NULL)
		return;
	partSet.Empty();
	nodeSet.Empty();
	SEGI nSegI=ParentDrawing()->m_nSegI;
	LINEPARTSET sum_partset;
	NODESET sum_nodeset;
	if(m_pModule==NULL&&(nSegI.iSeg>=0||nSegI.iSeg==-1))
	{	//˾��ͼ���ṹͼ��Ҫ���ɶ�����ͼ wht 11-06-16
		if(m_pAttachView->m_iViewType==0)
		{
			for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
			{
				if(m_pAttachView->filter.IsValidLayer(pPart->layer())&&pPart->IsLinePart()&&(pPart->iSeg.iSeg==nSegI.iSeg||nSegI.iSeg<0))
					partSet.append(pPart);	//nSegI.iSeg<0��ʾ˾��ͼ(-1)����ͼ(-2)����ʱ����Ҫ���жκŹ��� wjh-2014.6.17
			}
			for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{
				if(m_pAttachView->filter.IsValidLayer(pNode->layer())&&(pNode->iSeg.iSeg==nSegI.iSeg||nSegI.iSeg<0))
					nodeSet.append(pNode);
			}
		}
		else if(m_pAttachView->m_iViewType==1)
		{	//������ͼ���ý��жκ��ж� wht 10-11-23
			m_pAttachView->InitHashTable();
			for(CViewPart *pViewPart=m_pAttachView->GetFirstViewPart(); pViewPart; pViewPart=m_pAttachView->GetNextViewPart())
			{
				CLDSNode *pNode = Ta.FromNodeHandle(pViewPart->handle);
				if(pNode)
					nodeSet.append(pNode);
				CLDSLinePart *pLinePart = (CLDSLinePart*)Ta.FromPartHandle(pViewPart->handle,CLS_LINEPART);
				if(pLinePart)	
					partSet.append(pLinePart);
			}
		}
	}
	else if(m_pModule)
	{	//��ͼ��˾��ͼ
		m_pModule->RefreshModule(sum_nodeset,sum_partset);
		for(CLDSNode *pNode=sum_nodeset.GetFirst();pNode;pNode=sum_nodeset.GetNext())
		{
			if(m_pModule->handle!=ParentDrawing()->m_hActiveModule
				&&!pNode->cfgword.IsHasByteOnly(m_pModule->GetBodyNo()))//ѡ������ڸ�ģ�͵Ľڵ�
				continue;
			if(nSegI>0&&nSegI.iSeg!=pNode->iSeg.iSeg)
				continue;
			if(m_pAttachView->m_iViewType==1||(m_pAttachView->m_iViewType==0&&m_pAttachView->filter.IsValidLayer(pNode->layer())))
				nodeSet.append(pNode);
		}
		CLDSLinePart *pLowestLeg=NULL;
		double max_z=0;
		for(CLDSLinePart *pLinePart=sum_partset.GetFirst();pLinePart;pLinePart=sum_partset.GetNext())
		{
			if (pLinePart->layer(0)!='L'||pLinePart->layer(1)!='Z'||pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
				continue;
			if(pLowestLeg==NULL)
			{
				pLowestLeg=pLinePart;
				max_z=max(pLinePart->pStart->Position('Z',false),pLinePart->pEnd->Position('Z',false));
			}
			else if(max(pLinePart->pStart->Position('Z',false),pLinePart->pEnd->Position('Z',false))>max_z)
			{
				pLowestLeg=pLinePart;
				max_z=max(pLinePart->pStart->Position('Z',false),pLinePart->pEnd->Position('Z',false));
			}
		}
		BYTE flagByte=m_pModule->m_dwLegCfgWord.flag.bytes[m_pModule->GetBodyNo()-1];
		int nLegCount = StatTrueBitsCount(flagByte);
		for(pLinePart=sum_partset.GetFirst();pLinePart;pLinePart=sum_partset.GetNext())
		{
			if(nLegCount>1)
			{
				char cPos=CalPoleQuad(pLinePart->Start(),pLinePart->End());
				if (pLinePart->layer(0)=='L'&&pLinePart->layer(1)=='F'&&cPos=='1')
					continue;	//��������ͼ˾��ͼ�ĵ�һ�������ȵĸ���
				if (pLinePart->layer(0)=='L'&&pLinePart->layer(1)=='Z'&&cPos=='1'&&pLowestLeg!=NULL&&!pLinePart->cfgword.IsEqual(pLowestLeg->cfgword))
					continue;	//ֻ������ͼ˾��ͼ�ĵ�һ�������ȵ������
				if (pLinePart->layer(0)=='L'&&cPos!='1'&&pLowestLeg!=NULL&&!pLinePart->cfgword.IsEqual(pLowestLeg->cfgword))
					continue;	//��ͼ˾��ͼʱֻ���Ƶ�ǰ����ȼ���Ӧ����
				if(m_pModule->handle!=ParentDrawing()->m_hActiveModule&&pLinePart->layer(0)!='L')
					//&&!pLinePart->cfgword.IsEqual(CFGWORD(m_pModule->m_iBodyLegNo)))	//ѡ������ڸ�ģ�͵ĸ˼�
					continue;
			}
			else if(m_pModule->handle!=ParentDrawing()->m_hActiveModule
				&&!pLinePart->cfgword.IsHasByteOnly(m_pModule->GetBodyNo()))	
				continue;	//nLegCount==1ʱͨ����ĺ�ѡ������ڵ�ǰģ�͵ĸ˼� wht 13-07-23
			if(nSegI>0&&nSegI.iSeg!=pLinePart->iSeg.iSeg)
				continue;
			if(m_pAttachView->m_iViewType==1||(m_pAttachView->m_iViewType==0
				&&IsMatch(pLinePart->layer(2),m_pAttachView->filter.ThirdCharSet)))
				partSet.append(pLinePart);
		}
	}
	//���ƽڵ㼯��
	for(CLDSPart* pPart = partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->m_bVirtualPart||!pPart->IsLinePart())
			continue;
		CLDSLinePart* pLinePart = (CLDSLinePart*)pPart;
		if(pLinePart->pStart)
			pLinePart->pStart->feature=0;
		if(pLinePart->pEnd)
			pLinePart->pEnd->feature=0;
	}
	for(CLDSNode *pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		pNode->feature=1;
	for(pPart = partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->m_bVirtualPart||!pPart->IsLinePart())
			continue;
		CLDSLinePart* pLinePart = (CLDSLinePart*)pPart;
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;	//�����Ӿ��Ǹֻ����˴���ʼ�ն˽ڵ�,�˴��ų��Ӿ��Ǹ� wht 11-06-16
		if(pLinePart->pStart&&pLinePart->pStart->feature==0)
		{	//�˼���ʼ�˽ڵ�δ�ڵ�ǰ��ʾ������, ����ж������
			nodeSet.append(pLinePart->pStart);
			pLinePart->pStart->feature=1;
		}
		if(pLinePart->pEnd&&pLinePart->pEnd->feature==0)
		{	//�˼����ն˽ڵ�δ�ڵ�ǰ��ʾ������, ����ж������
			nodeSet.append(pLinePart->pEnd);
			pLinePart->pEnd->feature=1;
		}
	}
	//����ڵ������������������,����ű��ٴδ��ļ���ʹ�þ��������½ڵ�����͹������� wht 11-07-22
	nodeHandleList.Empty();
	for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		nodeHandleList.append(pNode->handle);
	partHandleList.Empty();
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
		partHandleList.append(pPart->handle);
}
void CLineView::EraseRelaCadEnt()
{
	AcDbObjectId *pEntId=NULL;
	for(pEntId=jointEntIdList.GetFirst();pEntId;pEntId=jointEntIdList.GetNext())
	{	
		AcDbEntity *pEnt=NULL;
		if(acdbOpenObject(pEnt,*pEntId,AcDb::kForWrite)!=Acad::eOk)
			continue;
		if(pEnt==NULL)
			continue;
		pEnt->erase();
		pEnt->close();
	}
	CDrawingComponent::EraseRelaCadEnt();
}
//������ͼ
void CLineView::Create()
{ 
	if(ParentDrawing()==NULL)
		return;
	//�����б�Ϊ��ʱ���³�ʼ�������б�
	if(nodeSet.GetNodeNum()<=0||partSet.GetNodeNum()<=0)
		GetDrawingObjectSet(FALSE);
	DRAGSET.ClearEntSet();
	//��չ������Ƽ���ע״̬
	ClearDrawnStates(TYPE_JOINT);
	ClearDrawnStates(TYPE_ROD);
	ClearDrawnStates(TYPE_NODE);
	CHashList<f2dLine>hashRodLine;	//Ϊ��ע�˼���˨��Ϣ������ϢԤ�洢�˼����˻���Ļ���λ�� wht 2012-06-01
	if(m_bFirstCreate)
	{	//��һ�����ɣ���Ҫ�Զ�������ͼװ��λ��
		CalInitAssemblyPosition();
		m_bFirstCreate=FALSE;
	}
	//ʹ�����λ�����¼�������ϵԭ��λ��
	//CalAssemblyPosByRelativePos();
	CDrawingCommand *pCmd=NULL;
	//��ʼ���ڵ�is_visible����,������ͼ�н�����ӽڵ��Ӧ����չ���ݵ� wht 11-07-27
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DRAW_LINEPART)
			continue;
		if(pCmd->m_paraLinePart.pLinePart==NULL)
			pCmd->m_paraLinePart.pLinePart=(CLDSLinePart*)Ta.FromPartHandle(pCmd->m_paraLinePart.hLinePart,CLS_LINEPART);
		if(pCmd->m_paraLinePart.pLinePart==NULL)
			continue;
		if(pCmd->m_paraLinePart.pLinePart->pStart)
			pCmd->m_paraLinePart.pLinePart->pStart->is_visible=FALSE;
		if(pCmd->m_paraLinePart.pLinePart->pEnd)
			pCmd->m_paraLinePart.pLinePart->pEnd->is_visible=FALSE;
	}
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{	//���ƹ���
		pCmd->feature=1;
		if(pCmd->m_eCmdType!=CMD_DRAW_LINEPART)
			continue;
		pCmd->feature=0;
		pCmd->Run();
		if(pCmd->m_paraLinePart.pLinePart)
		{
			f2dLine *pBaseLine=hashRodLine.Add(pCmd->m_paraLinePart.hLinePart);
			pBaseLine->startPt=pCmd->m_paraLinePart.drawStartBasePos;
			pBaseLine->endPt=pCmd->m_paraLinePart.drawEndBasePos;
		}
	}
	//ָ����ͼװ��λ��
	f2dPoint offset=SpecifyAssemblyPosition();
	for(f2dLine *pRodLine=hashRodLine.GetFirst();pRodLine;pRodLine=hashRodLine.GetNext())
	{
		pRodLine->startPt.x+=offset.x;
		pRodLine->startPt.y+=offset.y;
		pRodLine->endPt.x+=offset.x;
		pRodLine->endPt.y+=offset.y;
	}
	//��ע��ͼ�����Լ��ϽӶκ�
	InsertDrawingName();
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{	//���г����ƹ������κű�ע����ע��˨������Ϣ����ע�Ǹֽ�ͷ���������
		if(pCmd->feature==0||pCmd->m_eCmdType==CMD_DIM_SEGI||
			pCmd->m_eCmdType==CMD_DIM_BOLTORFL_INFO||pCmd->m_eCmdType==CMD_DIM_ANGLEJOINT)
			continue;
		pCmd->Run();
		pCmd->feature=0;
	}
	//��ע��˨������Ϣ
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_BOLTORFL_INFO) 
			continue;
		f2dLine *pBaseLine=hashRodLine.GetValue(pCmd->m_paraBoltOrFlInfo.hLinePart);
		if(pBaseLine)
		{
			pCmd->m_paraBoltOrFlInfo.fStartOddBasePosX=pBaseLine->startPt.x;
			pCmd->m_paraBoltOrFlInfo.fStartOddBasePosY=pBaseLine->startPt.y;
			pCmd->m_paraBoltOrFlInfo.fEndOddBasePosX=pBaseLine->endPt.x;
			pCmd->m_paraBoltOrFlInfo.fEndOddBasePosY=pBaseLine->endPt.y;
		}
		pCmd->Run();
		pCmd->feature=0;
	}
	//��ע�Ǹֽ�ͷ
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{	
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_ANGLEJOINT)
			continue;
		pCmd->Run();
		pCmd->feature=0;
	}
	//��ע�κ�
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0) 
			continue;
		pCmd->Run(); //if(pCmd->m_eCmdType==CMD_DIM_SEGI)
		pCmd->feature=0;
	}
	//����ͼԪ��ͼԪ����������ӵ���ͼ���ο�������
	UpdateDrawingRectArray();
	ads_command(RTSTR,"RE",RTNONE);
}
//���ر�עʱX����
//iType: 0.Ϊ�ڼ䴹�� 1.�ᵣ���ߵ�䴹ֱ��� 2.�ֶμ�� 
//		 3.���� 4.ˮƽ��ע(�����ڳߴ�) 5.��ͷ(�����ߵ�)ˮƽ����ע 
struct DIM_NODE_COUPLE{CLDSNodePtr pStart,pEnd;};
double CLineView::ChooseFeatPt(NODESET &nodesetSeleted,int iType,char cDrawAxisX/*='X'*/)
{
	double sizePtX = 0; //��ע��X����λ��
	if(nodeSet.GetNodeNum()<=0)
		return 0;
	CLDSNode* pNode,*pDimNode;
	CLDSNode *pLowestNode = nodeSet.GetFirst();
	CLDSNode *pHighestNode = nodeSet.GetFirst();
	if(iType==CLineView::DIM_SECTION_Z||iType==CLineView::DIM_GUAXIAN_SPACE_Z)
	{	//�ڼ䴹��
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{
			if(pNode->Position('Z',false)>pLowestNode->Position('Z',false))
				pLowestNode = pNode;
			if(iType==CLineView::DIM_SECTION_Z&&!pNode->IsUdfCoordZ())
				continue;	//�������û����������Ľڵ�
			else if(iType==CLineView::DIM_GUAXIAN_SPACE_Z&&(pNode->m_cPosCalType!=0||toupper(pNode->layer(2))!='Q'))
				continue;	//����Ϊ�ᵣ���ߵ㶼��ǰ����ƽڵ�
			for(pDimNode=nodesetSeleted.GetFirst();pDimNode;pDimNode=nodesetSeleted.GetNext())
			{
				if (pNode->Position('Z',false) == pDimNode->Position('Z',false))
				{
					if (pNode->Position('X',false)<pDimNode->Position('X',false))
						pDimNode=nodesetSeleted.SetCursorValue(pNode);
					break;
				}
			}
			if (pDimNode==NULL)
				pDimNode=nodesetSeleted.append(pNode);
			if (pDimNode->Position('X',false)<sizePtX)
				sizePtX = pDimNode->Position('X',false);
		}
		if(iType==CLineView::DIM_GUAXIAN_SPACE_Z)
			nodesetSeleted.append(pLowestNode);
		return sizePtX;
	}
	else if(iType==CLineView::DIM_SEG_SPACE_Z)
	{	//��ͷ������ֶμ��
		CHashSet<CLDSNodePtr> hashSegNodes;
		double min_shank_z=0;
		bool init_shank_z=false;
		CLDSNode *pShankTopNode=NULL;
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{	//���������߽ڵ�
			if(pNode->Position('Z',false)>pLowestNode->Position('Z',false))
				pLowestNode = pNode;
			if(pNode->Position('Z',false)<pHighestNode->Position('Z',false))
				pHighestNode = pNode;
			if(isdigit(pNode->layer(2))!=0&&toupper(pNode->layer(0))!='T')
			{
				if(pShankTopNode==NULL)
					pShankTopNode=pNode;
				else if(pNode->Position('Z',false)<pShankTopNode->Position('Z',false))
					pShankTopNode=pNode;
			}
			if(pNode->Position('X',false)<sizePtX)
				sizePtX = pNode->Position('X',false);
		}
		for(CSuperSmartPtr<CLDSPart> pPart=partSet.GetFirst();pPart.IsHasPtr();pPart=partSet.GetNext())
		{
			if(pPart->layer(0)=='T' || !pPart->IsLinePart() || pPart.LinePartPointer()->pStart==NULL ||pPart.LinePartPointer()->pEnd==NULL)
				continue;
			pNode=NULL;
			if( isdigit(pPart.LinePartPointer()->pStart->layer(2))!=0&&toupper(pPart.LinePartPointer()->pStart->layer(0))!='T')
				pNode=pPart.LinePartPointer()->pStart;
			if((isdigit(pPart.LinePartPointer()->pEnd->layer(2))!=0&&toupper(pPart.LinePartPointer()->pEnd->layer(0))!='T')&&
				(pNode==NULL||(pPart.LinePartPointer()->pEnd&&pNode->Position('Z',false)<pPart.LinePartPointer()->pEnd->Position('Z',false))))
					pNode=pPart.LinePartPointer()->pEnd;
			if(pNode==NULL)
				continue;
			for(pDimNode=hashSegNodes.GetFirst();pDimNode;pDimNode=hashSegNodes.GetNext())
			{	
				if(pPart->iSeg.iSeg == hashSegNodes.GetCursorKey())
				{	//δ���ǽڵ�ֻ�������¶���֮һ��������»�ȡ�ڵ㲻�Ƿֽ�㡡
					if(pNode->Position('Z',false)>pDimNode->Position('Z',false)||
						(pNode->Position('Z',false)==pDimNode->Position('Z',false)&&pNode->Position('X',false)<pDimNode->Position('X',false)))
						pDimNode=hashSegNodes.SetValue(pPart->iSeg.iSeg,pNode);
					break;
				}
			}

			if (pDimNode==NULL)
			{
				if(!init_shank_z&&pNode->layer(0)!='T')
					min_shank_z=pNode->Position('Z',false);
				else if(init_shank_z&&pNode->layer(0)!='T'&&min_shank_z>pNode->Position('Z',false))
				{
					min_shank_z=pNode->Position('Z',false);
					init_shank_z=true;
				}
				pDimNode=hashSegNodes.SetValue(pPart->iSeg.iSeg,pNode);
			}
		}
		//TODO:pShankTopNode��ΪNULL�������������ʱδ����ã��Ժ�Ҫϸ�� wjh-2014.11.17
		if(pShankTopNode==NULL)
			return 0;
		if(pShankTopNode->Position('Z',false)>pHighestNode->Position('Z',false))
			nodesetSeleted.append(pHighestNode);
		if(min_shank_z>pShankTopNode->Position('Z',false))
			nodesetSeleted.append(pShankTopNode);
		for(pDimNode=hashSegNodes.GetFirst();pDimNode;pDimNode=hashSegNodes.GetNext())
			nodesetSeleted.append(pDimNode);
		return sizePtX;
	}
	else if(iType==CLineView::DIM_TOWER_HEIGHT)
	{	//����(��ͷ+����)
		double min_shank_z=0;
		CLDSNode *pShankTopNode=NULL;
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{
			//���������߽ڵ�
			if(pNode->Position('Z',false)>pLowestNode->Position('Z',false))
				pLowestNode = pNode;
			if(pNode->Position('Z',false)<pHighestNode->Position('Z',false))
				pHighestNode = pNode;
			if(pNode->Position(cDrawAxisX,false)<sizePtX)
				sizePtX = pNode->Position(cDrawAxisX,false);
			if (!isdigit(pNode->layer(2))||toupper(pNode->layer(0))!='S')
				continue;
			if(pShankTopNode==NULL)
			{
				min_shank_z=pNode->Position('Z',false);
				pShankTopNode=pNode;
			}
			else if(min_shank_z>pNode->Position('Z',false))
			{
				min_shank_z=pNode->Position('Z',false);
				pShankTopNode=pNode;
			}
		}
		if(min_shank_z>pHighestNode->Position('Z',false))
			nodesetSeleted.append(pHighestNode);
		nodesetSeleted.append(pShankTopNode);
		nodesetSeleted.append(pLowestNode);
		return sizePtX;
	}
	else if(iType==DIM_SHANK_SPACE_XY)
	{	//�����¶ο��ڳߴ�
		ATOM_LIST<DIM_NODE_COUPLE> listSlopeOpen;
		double max_shank_z=0;
		DIM_NODE_COUPLE dimBtmCouple, *pCouple;
		dimBtmCouple.pStart=dimBtmCouple.pEnd=NULL;
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{
			if(pNode->Position('Z',false)>pLowestNode->Position('Z',false))
				pLowestNode = pNode;
			if(pNode->Position(cDrawAxisX,false)<sizePtX)
				sizePtX = pNode->Position(cDrawAxisX,false);
			if (pNode->Position(cDrawAxisX,false)<sizePtX)
				sizePtX = pNode->Position('X',false);
			if(dimBtmCouple.pStart==NULL)
			{
				dimBtmCouple.pStart=dimBtmCouple.pEnd=pNode;
			}
			else if(dimBtmCouple.pStart->Position('Z',false)<pNode->Position('Z',false))
			{
				dimBtmCouple.pStart=dimBtmCouple.pEnd=pNode;
			}
			else if(dimBtmCouple.pStart->Position('Z',false)==pNode->Position('Z',false))
			{
				if(pNode->Position(cDrawAxisX,false)<dimBtmCouple.pStart->Position(cDrawAxisX,false))
					dimBtmCouple.pStart=pNode;
				if(pNode->Position(cDrawAxisX,false)>dimBtmCouple.pEnd->Position(cDrawAxisX,false))
					dimBtmCouple.pEnd=pNode;
			}
			if (pNode->m_cPosCalType!=0||!isdigit(pNode->layer(2))||toupper(pNode->layer(0))=='T')
				continue;	//������¿ڽڵ����Ϊ���ƽڵ㣬
			for(pCouple=listSlopeOpen.GetFirst();pCouple;pCouple=listSlopeOpen.GetNext())
			{	
				if(pNode->Position('Z',false)==pCouple->pStart->Position('Z',false))
				{
					if(pNode->Position(cDrawAxisX,false)<pCouple->pStart->Position(cDrawAxisX,false))
						pCouple->pStart=pNode;
					if(pNode->Position(cDrawAxisX,false)>pCouple->pEnd->Position(cDrawAxisX,false))
						pCouple->pEnd=pNode;
					break;
				}
			}
			if(pCouple==NULL)
			{
				if(listSlopeOpen.GetNodeNum()==0)
					max_shank_z=pNode->Position('Z',false);
				else if(max_shank_z<pNode->Position('Z',false))
					max_shank_z=pNode->Position('Z',false);
				pCouple=listSlopeOpen.append();
				pCouple->pStart=pCouple->pEnd=pNode;
			}
		}
		for(pCouple=listSlopeOpen.GetFirst();pCouple;pCouple=listSlopeOpen.GetNext())
		{
			if(pCouple->pStart!=pCouple->pEnd)
			{
				nodesetSeleted.append(pCouple->pStart);
				nodesetSeleted.append(pCouple->pEnd);
			}
		}
		if(max_shank_z<dimBtmCouple.pStart->Position('Z',false))
		{
			nodesetSeleted.append(dimBtmCouple.pStart);
			nodesetSeleted.append(dimBtmCouple.pEnd);
		}
		return sizePtX;
	}
	else if(iType==DIM_GUAXIAN_SPACE_XY)
	{	//��ͷ(�����ߵ�)ˮƽ����ע
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{
			if(pNode->Position(cDrawAxisX,false)<EPS)
				continue;	//�������ϵĽڵ�
			if (toupper(pNode->layer(0))=='T'&&
				((cDrawAxisX=='X'&&pNode->IsUdfCoordX())||(cDrawAxisX=='Y'&&pNode->IsUdfCoordY())))
				nodesetSeleted.append(pNode);	//��ͷ(�����ߵ�)ˮƽ����ע
		}
		return 0;
	}
	else
		return 0;
}
//���ɹ�����ϸ��ע
void CLineView::CreateDimPartSpecCmdList()
{
	dimpartspec.EmptyPartSpecDim();	//��չ�����ϸ��ע�б�
	CDrawingCommand *pCmd=NULL;
	//ParentDrawing()->hashProcessedParts
	CHashList<CDrawingCommand*>hashDimRodCmds;
	CLDSPart *pPart=NULL;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(!pPart->IsLinePart())
			continue;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;	//����עʼ�ն˽ڵ�ǿյĸ˼� wht 11-06-14
		long  hFatherRod=0;
		RELATIVE_OBJECT relaObj;
		if(!pLinePart->GetFatherMirObj(&relaObj))
			hFatherRod=pLinePart->handle;
		else
			hFatherRod=relaObj.hObj;
		if(hashDimRodCmds.GetValue(hFatherRod)==NULL)
		{
			pCmd=cmdList.append();
			pCmd->m_pDrawingCom=this;
			pCmd->m_eCmdType=CMD_DIM_PARTSPEC;
			pCmd->InitCommandPara();
			pCmd->m_paraPartSpec.pObj=pPart;
			pCmd->m_paraPartSpec.hObj=pPart->handle;
			hashDimRodCmds.SetValue(hFatherRod,pCmd);
		}
#ifdef __PART_DESIGN_INC_
		else
		{
			CDrawingCommand* pLegacyCmd=hashDimRodCmds[hFatherRod];
			CLDSLinePart* pLegacyRod=(CLDSLinePart*)pLegacyCmd->m_paraPartSpec.pObj;
			long legacy_si=min(pLegacyRod->pStart->point_i,pLegacyRod->pEnd->point_i);
			long legacy_ei=max(pLegacyRod->pStart->point_i,pLegacyRod->pEnd->point_i);
			long si=min(pLinePart->pStart->point_i,pLinePart->pEnd->point_i);
			long ei=max(pLinePart->pStart->point_i,pLinePart->pEnd->point_i);
			if(si<legacy_si||(si==legacy_si&&ei<legacy_ei))
			{
				pLegacyCmd->m_paraPartSpec.hObj=pLinePart->handle;
				pLegacyCmd->m_paraPartSpec.pObj=pLinePart;
			}
		}
#endif
	}
	CLDSNode *pNode=NULL;
	for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
	{
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DIM_PARTSPEC;
		pCmd->InitCommandPara();
		pCmd->m_paraPartSpec.pObj=pNode;
		pCmd->m_paraPartSpec.hObj=pNode->handle;
	}
}
//������˨��ϸ��ע�����б�	
void CLineView::CreateDimBoltOrFlInfoCmdList()
{
	CLDSPart *pPart=NULL;
	CDrawingCommand *pCmd=NULL;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(!pPart->IsLinePart())
			continue;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;	//����עʼ�ն˽ڵ�ǿյĸ˼� wht 11-06-14
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DIM_BOLTORFL_INFO;
		pCmd->InitCommandPara();
		pCmd->m_paraBoltOrFlInfo.hLinePart=pPart->handle;
		pCmd->m_paraBoltOrFlInfo.pLinePart=pLinePart;
	}
}
//���ɽǸֽ�ͷ��ע�����б�	
void CLineView::CreateAngleJointCmdList()
{
	CLDSPart *pPart=NULL;
	CDrawingCommand *pCmd=NULL;
	CHashList<long> hashJointIdById;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(!pPart->IsAngle())
			continue;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
		int idStartJoint=pLinePart->connectStart.idAngleJoint;
		int idEndJoint=pLinePart->connectEnd.idAngleJoint;
		if(idStartJoint>0&&!hashJointIdById.GetValue(idStartJoint))
		{
			pCmd=cmdList.append();
			pCmd->m_pDrawingCom=this;
			pCmd->m_eCmdType=CMD_DIM_ANGLEJOINT;
			pCmd->InitCommandPara();
			pCmd->m_paraAngleJoint.idAngleJoint=idStartJoint;
			pCmd->m_paraAngleJoint.pJoint=Ta.hashAngleJoint.GetValue(idStartJoint);
			hashJointIdById.SetValue(idStartJoint,idStartJoint);
		}
		if(idEndJoint>0&&!hashJointIdById.GetValue(idEndJoint))
		{
			pCmd=cmdList.append();
			pCmd->m_pDrawingCom=this;
			pCmd->m_eCmdType=CMD_DIM_ANGLEJOINT;
			pCmd->InitCommandPara();
			pCmd->m_paraAngleJoint.idAngleJoint=idEndJoint;
			pCmd->m_paraAngleJoint.pJoint=Ta.hashAngleJoint.GetValue(idEndJoint);
			hashJointIdById.SetValue(idEndJoint,idEndJoint);
		}
	}
}
//������������
BOOL CLineView::CreateCmdList(BOOL bInit/*=TRUE*/)
{
	//1.��ȡ��Ҫ���ƵĹ���
	GetDrawingObjectSet(bInit);
	//2.������Ӧ�����б�
	cmdList.Empty();
	CLDSPart *pPart=NULL;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(!pPart->IsLinePart())
			continue;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;
		pLinePart->pStart->is_visible=FALSE;//���ñ�־λ����ڵ��ظ�����
		pLinePart->pEnd->is_visible=FALSE;		
		CDrawingCommand *pCommand=cmdList.append();
		pCommand->m_pDrawingCom=this;
		pCommand->m_eCmdType=CMD_DRAW_LINEPART;
		pCommand->InitCommandPara();
		pCommand->m_paraLinePart.hLinePart=pPart->handle;
		pCommand->m_paraLinePart.pLinePart=pLinePart;
		if(ParentDrawing()->m_nSegI.iSeg<0)
			pCommand->m_paraLinePart.bSumMap=TRUE;
		else //�ֶε���ͼ,���Ǹ����߻��� wht 11-07-27
			pCommand->m_paraLinePart.bSumMap=FALSE;	
		pCommand->m_paraLinePart.bDimByAnalysisMode=m_bAnalysisMode;
	}
	if(m_bAnalysisMode)	//��ѧ����ģʽ��˾��ͼ
	{	//3.������ϸ��ע
		CreateDimPartSpecCmdList();
		//4.�˼���˨��Ϣ������Ϣ
		CreateDimBoltOrFlInfoCmdList();
		//  �Ǹֽ�ͷ��ע��Ϣ
		CreateAngleJointCmdList();
	}
	//4.�ߴ��ע
	int i=0;
	BOOL bSideView=FALSE;
	if(m_pAttachView&&stricmp(m_pAttachView->defaultLayer,"SPY")==0)
		bSideView=TRUE;
	if(m_pAttachView&&m_pAttachView->m_iViewType==0)
	{	//Ŀǰ��֧��ͼ����ͼ��עģ�ͳߴ� wht 11-06-16
		int nVerticalDimCount=0;
		for(i=0;i<6;i++)
		{	
			if(bSideView&&i!=4)
				continue;	//������ͼ����ע�¶ο��ڳߴ� wht 11-06-15
			char cDrawAxisX='X';
			if(bSideView)
				cDrawAxisX='Y';
			NODESET nodeSetTemp;
			double fMinX = ChooseFeatPt(nodeSetTemp,i,cDrawAxisX);
			if(nodeSetTemp.GetNodeNum()==0)
				continue;
			CLDSNode* pNode;
			DYN_ARRAY<CLDSNodePtr>arrDimNodes(nodeSetTemp.GetNodeNum());
			UINT j=0;
			for(pNode=nodeSetTemp.GetFirst();pNode;pNode=nodeSetTemp.GetNext(),j++)
				arrDimNodes[j]=pNode;
			if(nodeSetTemp.GetNodeNum()<=2&&i<3)
				continue;
			CDrawingCommand *pCommand=cmdList.append();
			pCommand->m_pDrawingCom=this;
			pCommand->m_eCmdType=CMD_DIM_MODELSIZE;
			pCommand->InitCommandPara();
			pCommand->m_paraDimModelSize.iDimType = i;
			pCommand->m_paraDimModelSize.fSpace = 100;
			if(i==DIM_SHANK_SPACE_XY)//>3)
			{	//�¶ο��ڰ�nodeSetTemp�ɶ����ɳߴ��ע
				pCommand->m_paraDimModelSize.fDimDistance = 500;
				for(j=0;j+1<arrDimNodes.Size();j+=2)
				{
					pCommand->m_paraDimModelSize.nodeSet.append(arrDimNodes[j]);
					pCommand->m_paraDimModelSize.nodeHandleList.append(arrDimNodes[j]->handle);
					pCommand->m_paraDimModelSize.bUpDim = FALSE;
					if(arrDimNodes[j]->feature==2)
						pCommand->m_paraDimModelSize.bUpDim = TRUE;
					pCommand->m_paraDimModelSize.nodeSet.append(arrDimNodes[j+1]);
					pCommand->m_paraDimModelSize.nodeHandleList.append(arrDimNodes[j+1]->handle);
					if(j+3<arrDimNodes.Size())
					{
						pCommand=cmdList.append();
						pCommand->m_pDrawingCom=this;
						pCommand->m_eCmdType=CMD_DIM_MODELSIZE;
						pCommand->InitCommandPara();
						pCommand->m_paraDimModelSize.iDimType = i;
						pCommand->m_paraDimModelSize.fSpace = 100;
						pCommand->m_paraDimModelSize.fDimDistance = 500;
					}
				}
				pCommand->m_paraDimModelSize.fDimDistance = 2*sys.dim_map.fDimTextSize*sys.general_set.LineMapScale;
			}
			else
			{
				for(CLDSNode *pNode = nodeSetTemp.GetFirst();pNode;pNode = nodeSetTemp.GetNext())
				{
					pCommand->m_paraDimModelSize.nodeSet.append(pNode);
					pCommand->m_paraDimModelSize.nodeHandleList.append(pNode->handle);
				}
				if(i==DIM_GUAXIAN_SPACE_XY)
					pCommand->m_paraDimModelSize.fDimDistance = 100*(nVerticalDimCount+1);	
				else
					pCommand->m_paraDimModelSize.fDimDistance = 1000*(nVerticalDimCount+1);	
				pCommand->m_paraDimModelSize.fMinX = fMinX;
				nVerticalDimCount++;
			}
		}
	}
	//˾��ͼҲ��Ҫ��ע�κ���Ϣ wht 11-07-15
	//if(!m_bAnalysisMode)
	{	//5.�κű�ע
		if(m_pModule)
		{	//��ͼ��˾��ͼ�κű�ע
			int nSegNum=0;
			ATOM_LIST<SEGI> segNoList;
			if(m_pModule->handle==ParentDrawing()->m_hActiveModule)
				nSegNum=Ta.GetSegNoList(segNoList,m_pModule);
			else 
				nSegNum=Ta.GetSegNoList(segNoList,m_pModule,TRUE);
			if(nSegNum>0)
			{
				for(SEGI *pSegI=segNoList.GetFirst();pSegI;pSegI=segNoList.GetNext())
				{
					if(*pSegI<=0)
						continue;
					CDrawingCommand *pCommand=cmdList.append();
					pCommand->m_pDrawingCom=this;
					pCommand->m_eCmdType=CMD_DIM_SEGI;
					pCommand->InitCommandPara();
					pCommand->m_paraDimSegI.nSegI=*pSegI;
					pCommand->m_paraDimSegI.bSelectPos=TRUE;
				}
			}
		}
		else if(ParentDrawing()->m_nSegI.iSeg>0&&m_pAttachView&&m_pAttachView->m_iViewType==0)
		{	//�ֶε���ͼ�κű�ע
			CDrawingCommand *pCommand=cmdList.append();
			pCommand->m_pDrawingCom=this;
			pCommand->m_eCmdType=CMD_DIM_SEGI;
			pCommand->InitCommandPara();
			pCommand->m_paraDimSegI.nSegI=ParentDrawing()->m_nSegI;
			pCommand->m_paraDimSegI.bSelectPos=TRUE;
		}
	}
	if(bInit)	//6.������ͼ��ʼװ��λ��
		CalInitAssemblyPosition();
	return TRUE;
}
//���������б�
void CLineView::UpdateCmdList()
{
	CHashList<CDrawingCommand> hashDrawPoleList;		//key:pPart->handle
	CHashList<CDrawingCommand> hashPartSpecList;		//key:pPart->handle
	CHashList<CDrawingCommand> hashDimSegIList;			//key:pPart->handle
	CDrawingCommand *pCmd=NULL;
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		CDrawingCommand *pNewCmd=NULL;
		if(pCmd->m_eCmdType==CMD_DRAW_LINEPART)		//���Ƹ˼�
			pNewCmd=hashDrawPoleList.Add(pCmd->m_paraLinePart.hLinePart);
		else if(pCmd->m_eCmdType==CMD_DIM_PARTSPEC)	//��ע������ϸ
			pNewCmd=hashPartSpecList.Add(pCmd->m_paraPartSpec.hObj);
		else if(pCmd->m_eCmdType==CMD_DIM_SEGI)		//��ע�κ�
			pNewCmd=hashDimSegIList.Add(pCmd->m_paraDimSegI.nSegI);
		else
			continue;
		pNewCmd->CopyProperty(pCmd);
	}
	//���»�������б�
	CreateCmdList(FALSE);
	//�Ա������ɵ������б���ԭ�������б�,��˳���ƾɵ��������ֵ��Ӧ��������
	CDrawingCommand *pOldCmd=NULL;
	// ���Ƹ˼�����
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DRAW_LINEPART)
			continue;
		pOldCmd=hashDrawPoleList.GetValue(pCmd->m_paraLinePart.hLinePart);
		if(pOldCmd)
			pCmd->CopyProperty(pOldCmd);
	}
	// ��ע������ϸ����
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_PARTSPEC)
			continue;
		pOldCmd=hashPartSpecList.GetValue(pCmd->m_paraPartSpec.hObj);
		if(pOldCmd)
			pCmd->CopyProperty(pOldCmd);
	}
	// ��עģ�ͳߴ����� ֱ��ʹ�������ɵ������
	// ��ע�κ�����
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_SEGI)
			continue;
		pOldCmd=hashDimSegIList.GetValue(pCmd->m_paraDimSegI.nSegI);
		if(pOldCmd)
			pCmd->CopyProperty(pOldCmd);
	}
}

void CLineView::FromBuffer(CBuffer &buffer, long version/*=NULL*/)
{
	CDrawingComponent::FromBuffer(buffer,version);
	buffer.ReadInteger(&m_bAnalysisMode);
	if(version>1000002)
	{
		buffer.ReadInteger(&m_bDimNodeNo);		//1000003 wht 11-06-19
		buffer.ReadInteger(&m_bDimPoleGuige);	//1000003 
	}
}

void CLineView::ToBuffer(CBuffer &buffer)
{
	CDrawingComponent::ToBuffer(buffer);
	buffer.WriteInteger(m_bAnalysisMode);
	buffer.WriteInteger(m_bDimNodeNo);		//1000003 wht 11-06-19
	buffer.WriteInteger(m_bDimPoleGuige);	//1000003 wht 
}

//////////////////////////////////////////////////////////////////////////
//CStruView
BOOL CStruView::m_bDimPartSpec=TRUE;
BOOL CStruView::m_bDimPoleOddment=TRUE;
BOOL CStruView::m_bDimFootNailSpace=TRUE;
BOOL CStruView::m_bDimFillPlateSpace=TRUE;
BOOL CStruView::m_bDimLsSpace=TRUE;
BOOL CStruView::m_bDimJgZhunJu=TRUE;
BOOL CStruView::m_bDimBoltInfo=TRUE;
BOOL CStruView::m_bDimBoltPad=TRUE;
BOOL CStruView::m_bDimSimPoly=TRUE;
CStruView::CStruView(int id/*=0*/,IDrawing* pDrawing/*=NULL*/) : CDrawingComponent(id,pDrawing)
{
	m_eClassType=DRAW_COM_STRUVIEW;	//��ͼ����
	m_fScale=1.0/sys.general_set.StruMapScale.fRealToDraw;
}

CStruView::~CStruView()
{
	
}

//�ֶνṹͼ:��ȡ��Ҫ���ƵĶ��󼯺�
void CStruView::GetDrawingObjectSet(BOOL bInitDCS)
{	
	if(ParentDrawing()==NULL||m_pAttachView==NULL)
		return;
	partSet.Empty();
	nodeSet.Empty();
	CLDSNode *pNode=NULL;
	CLDSPart *pPart=NULL;
	if(m_pAttachView->m_iViewType==0)
	{	//ͼ����ͼ
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(m_pModule&&!m_pModule->IsSonPart(pPart))
				continue;	//�ǵ�ǰ���߹��������л���
			if(m_pAttachView->filter.IsValidLayer(pNode->layer())&&pNode->iSeg.iSeg==ParentDrawing()->m_nSegI.iSeg)
				nodeSet.append(pNode);
		}
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(m_pModule&&!m_pModule->IsSonPart(pPart))
				continue;	//�ǵ�ǰ���߹��������л���
			pPart->feature=0;
			pPart->is_visible=TRUE;
			if(m_pAttachView->filter.IsValidLayer(pPart->layer())&&pPart->iSeg.iSeg==ParentDrawing()->m_nSegI.iSeg)
				partSet.append((CLDSLinePart*)pPart);
		}
		//�����˼�
		polyPartlist.Empty();
		Ta.RetrievePolyPartList(polyPartlist);
		for(CLDSPolyPart *pPolyPart=polyPartlist.GetFirst();pPolyPart;pPolyPart=polyPartlist.GetNext())
		{
			if(m_pModule&&!m_pModule->IsSonPart(pPolyPart))
				continue;	//�ǵ�ǰ���߹��������л���
			if(!(m_pAttachView->filter.IsValidLayer(pPolyPart->layer())&&pPolyPart->iSeg.iSeg==ParentDrawing()->m_nSegI.iSeg))
				polyPartlist.DeleteCursor();
		}
		polyPartlist.Clean();
		for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
		{
			BOOL bPush=partSet.push_stack();
			for(CLDSPart *pAfterPart=partSet.GetNext();pAfterPart;pAfterPart=partSet.GetNext())
			{
				if(pPart==pAfterPart)	//ɾ��������Ĳ�©�������µ��ظ���������
					partSet.DeleteCursor();
			}
			if(bPush)
				partSet.pop_stack();
		}
	}
	else if(m_pAttachView->m_iViewType==1)
	{	//������ͼ
		for(CViewPart *pViewPart=m_pAttachView->GetFirstViewPart();pViewPart;pViewPart=m_pAttachView->GetNextViewPart())
		{
			pViewPart->pDisplayView=m_pAttachView;
			pPart = Ta.FromPartHandle(pViewPart->handle);
			if(pPart)
			{
				if(m_pModule&&!m_pModule->IsSonPart(pPart))
					continue;	//�ǵ�ǰ���߹��������л���
				pPart->feature=(long)pViewPart;	//������ص�CViewPartָ�� wht 10-12-10
				partSet.append(pPart);
				pPart->is_visible=TRUE;
			}
		}
	}
	//������˨�б�(������˨���ڱ��ε���˨) wht 11-07-22
	BOLTSET boltSet;
	for(pPart = partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->m_bVirtualPart)
			continue;
		CLsRefList *pLsRefList=pPart->GetLsRefList();
		if(pLsRefList==NULL)
			continue;
		if(CalPartDrawType(pPart,dcs,m_pAttachView)==1)
			continue;	//�����ǽ�����ƹ����ϵ���˨
		CLsRef *pLsRef=NULL;
		for(pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(pBolt==NULL)
				continue;
			if(pBolt->m_bVirtualPart)
			{
				pBolt->is_visible=FALSE;	//������˨
				boltSet.append(pBolt);	
			}
			else if(pBolt->iSeg.iSeg!=ParentDrawing()->m_nSegI.iSeg)	//&&m_pAttachView->m_iViewType==0
			{
				pBolt->is_visible=FALSE;	//�Ǳ�����˨
				boltSet.append(pBolt);	
			}
		}
	}
	for(pPart=boltSet.GetFirst();pPart;pPart=boltSet.GetNext())
		partSet.append(pPart);
	//���ƽڵ㼯��
	for(pPart = partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->m_bVirtualPart||!pPart->IsLinePart())
			continue;
		CLDSLinePart* pLinePart = (CLDSLinePart*)pPart;
		if(pLinePart->pStart)
			pLinePart->pStart->feature=0;
		if(pLinePart->pEnd)
			pLinePart->pEnd->feature=0;
	}
	for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		pNode->feature=1;
	for(pPart = partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->m_bVirtualPart||!pPart->IsLinePart())
			continue;
		CLDSLinePart* pLinePart = (CLDSLinePart*)pPart;
		CViewPart *pViewPart=(CViewPart*)pLinePart->feature;
		if(pViewPart&&pViewPart->cFlag=='Z')
			continue;	//ָ������Z����ĽǸ�
		if(pLinePart->pStart&&pLinePart->pStart->feature==0)
		{
			nodeSet.append(pLinePart->pStart);
			pLinePart->pStart->feature=1;
		}
		if(pLinePart->pEnd&&pLinePart->pEnd->feature==0)
		{
			nodeSet.append(pLinePart->pEnd);
			pLinePart->pEnd->feature=1;
		}
	}
	if(bInitDCS)	//2.�����ͼ����ϵ
		dcs=CalDrawingUcs(partSet,m_pAttachView,FALSE);
	
	//�����Ʋ�����ʾ�ķ����ϵ���˨ wht 10-03-04
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			if(sys.stru_map.bOnlyDrawHorizontalPlate)
			{	//�����Ƶ�ǰ��ͼ��ˮƽ��ʾ�ĸְ� wht 10-06-15
				if(fabs(dcs.axis_z*pPlate->ucs.axis_z)<0.707&&
					fabs(pPlate->ucs.axis_z*f3dPoint(0,0,1))<EPS_COS2)	//���Ű��ţ��嶼Ӧ�û���
				{
					CViewPart *pViewPart=(CViewPart*)pPlate->feature;
					if(pViewPart&&pViewPart->cFlag=='S')
						continue;	//����Ҫ�Ƴ�չ�����Ƶĸְ� wht 10-12-11
					partSet.DeleteNode(pPlate->handle);
					continue; //��ֱ����
				}
			}
			if(fabs(pPlate->m_fInnerRadius)<EPS2)
				continue;
			CLsRef *pLsRef=NULL;
			for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				partSet.DeleteNode(pLsRef->GetLsPtr()->handle);
		}
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
			if(!pParamPlate->IsFL()
				&&pParamPlate->m_iParamType!=TYPE_WATERSHIELDEDPLATE)	//��ˮ��
				continue;
			if(fabs(dcs.axis_z*pParamPlate->ucs.axis_z)>0.707)
				continue; //ˮƽ����
			CLsRef *pLsRef=NULL;
			for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				partSet.DeleteNode(pLsRef->GetLsPtr()->handle);
		}
	}
	//����ڵ������������������,����ű��ٴδ��ļ���ʹ�þ��������½ڵ�����͹������� wht 11-07-22
	nodeHandleList.Empty();
	for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		nodeHandleList.append(pNode->handle);
	partHandleList.Empty();
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
		partHandleList.append(pPart->handle);
}

typedef CDrawingCommand* DRWINGCMD_PTR;	
static int CompareCmdDrawingSerailNo(const DRWINGCMD_PTR &pCmd1,const DRWINGCMD_PTR &pCmd2)
{
	if(pCmd1->GetDrawingSerialNo()==0&&pCmd2->GetDrawingSerialNo()>0)
		return 1;
	else if(pCmd1->GetDrawingSerialNo()>0&&pCmd2->GetDrawingSerialNo()==0)
		return -1;	
	else if(pCmd1->GetDrawingSerialNo()>pCmd2->GetDrawingSerialNo())
		return 1;
	else if(pCmd1->GetDrawingSerialNo()<pCmd2->GetDrawingSerialNo())
		return -1;
	return 0;
}
//������ͼ
void CStruView::Create()
{	
	if(ParentDrawing()==NULL)
		return;
	//�����б�Ϊ��ʱ���³�ʼ�������б�
	if(nodeSet.GetNodeNum()<=0||partSet.GetNodeNum()<=0)
		GetDrawingObjectSet(FALSE);
	DRAGSET.ClearEntSet();
	if(m_bFirstCreate)
	{	//��һ�����ɣ���Ҫ�Զ�������ͼװ��λ��
		CalInitAssemblyPosition();
		m_bFirstCreate=FALSE;
	}
	//ʹ�����λ�����¼�������ϵԭ��λ��
	//CalAssemblyPosByRelativePos();
	CDrawingCommand *pCmd=NULL;
	CHashList<f2dLine>hashOddLine;	//Ϊ��ע����ͷԤ�洢�˼����˻���Ļ���λ�� wjh-2011.9.25
	CStringKeyHashList<CRodDimPos> rodDimPosHashPartNo;	//��¼��Ӧ������Ҫ��ע�˼���񳤶ȵĸ˼���� wht 12-05-22
	//ͳ����Ҫ������ƵĹ�������
	int i=0,nDrawingPartCount=0;
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{	//
		pCmd->feature=1;
		if(pCmd->m_eCmdType!=CMD_DRAW_PART)
			continue;
		if(pCmd->GetDrawingSerialNo()>0)
			nDrawingPartCount++;
	}
	if(nDrawingPartCount>0)
	{	//������˳����������
		DRWINGCMD_PTR *cmd_arr = new DRWINGCMD_PTR[nDrawingPartCount];
		for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
		{	//
			if(pCmd->m_eCmdType!=CMD_DRAW_PART)
				continue;
			if(pCmd->GetDrawingSerialNo()>0)
			{
				cmd_arr[i]=pCmd;
				i++;
			}	
		}
		//����������˳������
		CQuickSort<DRWINGCMD_PTR>::QuickSort(cmd_arr,nDrawingPartCount,CompareCmdDrawingSerailNo);
		for(i=0;i<nDrawingPartCount;i++)
		{
			pCmd=cmd_arr[i];
			if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DRAW_PART)
				continue;
			pCmd->Run();
			pCmd->feature=0;
			if(pCmd->m_paraPart.pPart&&pCmd->m_paraPart.pPart->IsLinePart())
			{
				f2dLine *pBaseLine=hashOddLine.Add(pCmd->m_paraPart.hPart);
				pBaseLine->startPt=pCmd->m_paraPart.drawStartBasePos;
				pBaseLine->endPt=pCmd->m_paraPart.drawEndBasePos;
				//
				char *sPartNo=pCmd->m_paraPart.pPart->GetPartNo();
				CRodDimPos rodDimPos(pCmd->m_paraPart.hPart,pBaseLine->startPt,pBaseLine->endPt);
				CRodDimPos *pRodDimPos=rodDimPosHashPartNo.GetValue(sPartNo);
				if(pRodDimPos)
				{
					if(pRodDimPos->GetDimPos().x<rodDimPos.GetDimPos().x)
						rodDimPosHashPartNo.SetValue(sPartNo,rodDimPos);
				}
				else 
					rodDimPosHashPartNo.SetValue(sPartNo,rodDimPos);
			}
		}
		delete []cmd_arr;
	}
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{	//���ƹ���
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DRAW_PART)
			continue;
		pCmd->Run();
		pCmd->feature=0;
		if(pCmd->m_paraPart.pPart&&pCmd->m_paraPart.pPart->IsLinePart())
		{
			f2dLine *pBaseLine=hashOddLine.Add(pCmd->m_paraPart.hPart);
			pBaseLine->startPt=pCmd->m_paraPart.drawStartBasePos;
			pBaseLine->endPt=pCmd->m_paraPart.drawEndBasePos;
			//
			char *sPartNo=pCmd->m_paraPart.pPart->GetPartNo();
			CRodDimPos rodDimPos(pCmd->m_paraPart.hPart,pBaseLine->startPt,pBaseLine->endPt);
			CRodDimPos *pRodDimPos=rodDimPosHashPartNo.GetValue(sPartNo);
			if(pRodDimPos)
			{
				if(pRodDimPos->GetDimPos().x<rodDimPos.GetDimPos().x)
					rodDimPosHashPartNo.SetValue(sPartNo,rodDimPos);
			}
			else 
				rodDimPosHashPartNo.SetValue(sPartNo,rodDimPos);
		}
	}
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{	//���Ƹ�����
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DRAW_ASSIST_LINE)
			continue;
		pCmd->Run();
		pCmd->feature=0;
	}
	//ָ����ͼװ��λ��
	f3dPoint offset=SpecifyAssemblyPosition();
	for(f2dLine *pOddLine=hashOddLine.GetFirst();pOddLine;pOddLine=hashOddLine.GetNext())
	{
		pOddLine->startPt.x+=offset.x;
		pOddLine->startPt.y+=offset.y;
		pOddLine->endPt.x+=offset.x;
		pOddLine->endPt.y+=offset.y;
	}
	//��ע��ͼ�����Լ��ϽӶκ�
	InsertDrawingName();
	//��ע��˨������
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_SPECIAL_LSSPACE)
			continue;
		if(CStruView::m_bDimLsSpace)
			pCmd->Run();
		pCmd->feature=0;
	}
	//��ע�Ǹ�����׼��
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_SPECIAL_JGZHUN)
			continue;
		if(CStruView::m_bDimJgZhunJu)
			pCmd->Run();
		pCmd->feature=0;
	}
	//��ע����ͷ
	actrTransactionManager->flushGraphics();	
	acedUpdateDisplay();	//ˢ�»�����½���,����acedSSGet()��ȡʵ�弯ʱ����ʧЧ wht 11-06-25
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_ODDMENT)
			continue;
		if(CStruView::m_bDimPoleOddment)
		{
			f2dLine *pBaseLine=hashOddLine.GetValue(pCmd->m_paraOddment.hLinePart);
			if(pBaseLine)
			{
				pCmd->m_paraOddment.fStartOddBasePosX=pBaseLine->startPt.x;
				pCmd->m_paraOddment.fStartOddBasePosY=pBaseLine->startPt.y;
				pCmd->m_paraOddment.fEndOddBasePosX=pBaseLine->endPt.x;
				pCmd->m_paraOddment.fEndOddBasePosY=pBaseLine->endPt.y;
			}
			pCmd->Run();
		}
		pCmd->feature=0;
	}
	//��ע�Ŷ����
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_FOOTNAIL_SPACE)
			continue;
		if(CStruView::m_bDimFootNailSpace)
			pCmd->Run();
		pCmd->feature=0;
	}
	//��ע�����
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_FILLPLATE_SPACE)
			continue;
		if(CStruView::m_bDimFillPlateSpace)
			pCmd->Run();
		pCmd->feature=0;
	}
	//��ע������ϸ
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_PARTSPEC)
			continue;
		CLDSDbObject *pObj=pCmd->m_paraPartSpec.pObj;
		if(pObj&&pObj->IsPart()&&((CLDSPart*)pObj)->IsLinePart())
		{	//�˼���񼰳��Ⱦ�����ע���Ҳ�˼��� wht 12-05-22
			CLDSLinePart *pLinePart=(CLDSLinePart*)pObj;
			CRodDimPos *pRodDimPos=rodDimPosHashPartNo.GetValue(pLinePart->GetPartNo());
			if(pRodDimPos&&pRodDimPos->GetRodHandle()==pLinePart->handle)
				pCmd->m_paraPartSpec.bOnlyDimPartNo=FALSE;	
			else
				pCmd->m_paraPartSpec.bOnlyDimPartNo=TRUE;
		}
		if(CStruView::m_bDimPartSpec)
			pCmd->Run();
		pCmd->feature=0;
	}
	//��ע������
	BOOL bNeedSetLayer=FALSE;	//�Ƿ���Ҫ����ͼ��
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{	//Ԥ���ж��Ƿ����ͨ��ʰȡ���ע��������
		if(pCmd->m_paraSimPoly.nSimPolyCmdType==2&&pCmd->m_bExecute)
		{
			bNeedSetLayer=TRUE;
			break;
		}
	}
	if(bNeedSetLayer)
	{	//����ͨ��ʰȡ���ע�������Σ���Ҫ�������ú�ͼ��
		acedCommand(RTSTR,"undo",RTSTR,"be",RTNONE);
		acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
		acedCommand(RTSTR,"layer",RTSTR,"F",RTSTR,"*",RTSTR,"",RTNONE);
		CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
		acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,&sLayers,RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
		acedCommand(RTSTR,"undo",RTSTR,"e",RTNONE);
	}
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_SIMPOLY)
			continue;
		if(CStruView::m_bDimSimPoly&&pCmd->m_paraSimPoly.nSimPolyCmdType!=1)
			pCmd->Run();
		pCmd->feature=0;
	}
	if(bNeedSetLayer)
	{	//����ͨ��ʰȡ���ע�������Σ�ִ�����ע�����������ظ�ԭ��ͼ��״̬
		acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"",RTNONE);
		acedCommand(RTSTR,"layer",RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
	}
	//��ע��˨��Ϣ
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_BOLTINFO)
			continue;
		if(CStruView::m_bDimBoltInfo)
			pCmd->Run();
		pCmd->feature=0;
	}
	//��ע��Ȧ��Ϣ
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_BOLTPAD)
			continue;
		if(CStruView::m_bDimBoltPad)
			pCmd->Run();
		pCmd->feature=0;
	}
	//��ע�����
	//��ע����
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==1)
			pCmd->Run();
	}
	//��ע����ͼ����
	long key=ParentDrawing()->listComponents.GetKey(this);
	CSectionViewSymbol *pViewSymbol=NULL;
	f3dPoint datum_pos=GetMapFrmInsideTopLeft();
	for(pViewSymbol=ParentDrawing()->sectionViewSymbols.GetFirst();pViewSymbol;pViewSymbol=ParentDrawing()->sectionViewSymbols.GetNext())
	{
		if(pViewSymbol->hAttachViewKey!=key)
			continue;	//�����ƴ����ڵ�ǰ��ͼ������ͼ��
		pViewSymbol->Draw();
	}
	//����ͼԪ��ͼԪ����������ӵ���ͼ���ο�������
	UpdateDrawingRectArray();
	ads_command(RTSTR,"REGEN",RTNONE);
}

typedef CLDSPart* PARTPTR;
//��������������,ԭ������:
//���ĽǸ�0;���Ű�2,��������1
static int GetTaAtomTypeGrade(CLDSPart *pPart)
{
	if(pPart->IsLinePart())
	{
		char layertag;
		int grade;
		layertag=pPart->layer(1);
		if(layertag=='Z')
			grade = 0;
		else if(layertag=='X')
			grade = 10;
		else if(layertag=='H')
			grade = 20;
		else
			grade = 30;
		if(pPart->m_hPartWeldParent>0)
			grade--;
		if(pPart->GetClassTypeId()==CLS_LINETUBE)
		{
			CLDSLineTube *pTube=(CLDSLineTube*)pPart;
			if(pTube->m_tJointStart.type==1||pTube->m_tJointEnd.type==1)
				grade--;	//��Բ���������
		}
		return grade;
	}
	else if(pPart->GetClassTypeId()==CLS_PLATE)
		return 40;
	else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		return 50;
	else if(pPart->GetClassTypeId()==CLS_FOOT)
		return 60;
	else
		return 70;
}

//���ȼ��ߵĹ��������,������ֵ��(������Ϊ0)
//-1��ʾ��һ��������� 0��ʾ��ͬ 1��ʾ�ڶ����������
static int CompareTaAtomType(const PARTPTR &pPart1,const PARTPTR &pPart2)
{
	int grade1,grade2;
	grade1 = GetTaAtomTypeGrade(pPart1);
	grade2 = GetTaAtomTypeGrade(pPart2);
	
	if(grade1<grade2)
		return 1;
	else if(grade1>grade2)
		return -1;
	else if(pPart1->IsLinePart()&&!pPart2->IsLinePart())
		return 1;
	else if(!pPart1->IsLinePart()&&pPart2->IsLinePart())
		return -1;
	else
		return 0;
}
//From DrawTaMap.cpp
BOOL ProcessJgStru(CLDSLineAngle *pJg, UCS_STRU &ucs);
void ProcessPlateStu(CLDSPlate *pPlate,UCS_STRU ucs,PARTSET& OutSidePlateSet,PARTSET& InSidePlateSet);
void CStruView::CreateDrawingPartCmdList()
{
	LSDRAW.EmptyDrawList();
	INSIDEJGSETDRAW.Empty();
	OUTSIDEJGSETDRAW.Empty();
	//���ݹ��������ͽ�������
	int nPart = partSet.GetNodeNum();
	PARTPTR *partPtrArr = new PARTPTR[nPart];
	int i=0;
	CLDSPart *pPart=NULL;
	for(pPart=partSet.GetFirst();pPart!=NULL;pPart=partSet.GetNext())
	{
		partPtrArr[i] = pPart;
		i++;
	}
	CHeapSort<PARTPTR>::HeapSort(partPtrArr,nPart,CompareTaAtomType);
	partSet.Empty();
	for(i=0;i<nPart;i++)
		partSet.append(partPtrArr[i]);
	delete []partPtrArr;
	
	BOLTSET BoltSet;
	CDrawingCommand *pCmd=NULL;
	CHashList<CLDSPart*>hashDrawParts;
	int nDrawingSerialNo=1;	//��������˳���
	LINEPARTSET OtherAngleSet,InsideShortJgSet,OutSideShortJgSet;
	PARTSET InSidePlateSet,OutSidePlateSet,TubeSet,LineSlotSet,InsideParamPlateSet,OutsideParamPlateSet,LineFlatSet;
	for(pPart=partSet.GetFirst();pPart!=NULL;pPart=partSet.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_BOLT&&pPart->m_bVirtualPart)	
			continue;	//��ͼʱ�����Ƴ���˨�������������
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSLineAngle *pJg=(CLDSLineAngle*)pPart;
			f3dPoint cur_jg_vec=pJg->End()-pJg->Start();
			normalize(cur_jg_vec);
			if((pJg->pStart==NULL||pJg->pEnd==NULL)
				&&(pJg->desStartPos.datumPoint.datum_pos_style==7||pJg->desEndPos.datumPoint.datum_pos_style==7))
			{	//�̽Ǹ�ʼ��/�ն˻�׼�㶨λ��ʽΪ:7.�ؽǸ���֫����ƫ�Ƶİ������ wht 11-03-02
				char norm_offset[50]="";
				if(pJg->desStartPos.datumPoint.datum_pos_style==7)
					strcpy(norm_offset,pJg->desStartPos.datumPoint.des_para.RODEND.norm_offset);
				else if(pJg->desEndPos.datumPoint.datum_pos_style==7)
					strcpy(norm_offset,pJg->desEndPos.datumPoint.des_para.RODEND.norm_offset);
				CDesignThickPara thick(norm_offset);
				double norm_offset_dist=thick.Thick(&Ta);
				if(norm_offset_dist>0)
					OutSideShortJgSet.append(pJg);
				else 
					InsideShortJgSet.append(pJg);
			}
			//֧�ֻ��ƽǸֽ��� wht 10-06-17
			if(pJg->m_bFootNail||fabs(cur_jg_vec*dcs.axis_z)>0.70710)	//�ǸֽŶ�����Ҫ���ƽǸֽ���ĽǸ�
				OtherAngleSet.append(pJg);
			else
			{
				if(!ProcessJgStru(pJg,dcs))
					OtherAngleSet.append(pJg);
			}
		}
		else if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//��ӻ�����ϽǸֵ�����
			pCmd=cmdList.append();
			pCmd->m_pDrawingCom=this;
			pCmd->m_eCmdType=CMD_DRAW_PART;
			pCmd->InitCommandPara();
			pCmd->m_paraPart.pPart=pPart;
			pCmd->m_paraPart.hPart=pPart->handle;
			pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		}
		else if(pPart->GetClassTypeId()==CLS_LINETUBE)
			TubeSet.append(pPart);
		else if(pPart->GetClassTypeId()==CLS_PLATE)		//������ͨ�ְ�ṹͼ
			ProcessPlateStu((CLDSPlate*)pPart,dcs,OutSidePlateSet,InSidePlateSet);
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)//���Ʋ������壨��˰�ȣ��ṹͼ
		{
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
			normalize(dcs.axis_z);
			if(pParamPlate->m_iParamType==TYPE_ROLLEND&&(pParamPlate->ucs.axis_z*dcs.axis_z>EPS_COS2))
				InsideParamPlateSet.append(pParamPlate);
			else
				OutsideParamPlateSet.append(pParamPlate);
		}
		else if(pPart->GetClassTypeId()==CLS_BOLT)		//������˨
			LSDRAW.AppendLsRef((CLDSBolt*)pPart,TRUE,dcs.axis_z);
		else if(pPart->GetClassTypeId()==CLS_LINESLOT)	//���Ʋ۸�(����ֻ�нŶ����õ��۸�)
			LineSlotSet.append(pPart);
		else if(pPart->GetClassTypeId()==CLS_LINEFLAT)	//���Ʊ���
			LineFlatSet.append(pPart);
	}
	PARTSET HorizontalPlateSet;	//ˮƽ���Ƶĸְ弯��
	//�������Ǹַֿ��������Խ���һ���̶��ϵ�����	
	CLDSLinePart *pLinePart=NULL;
	for(pLinePart=OtherAngleSet.GetFirst();pLinePart;pLinePart=OtherAngleSet.GetNext())
	{	//��Ҫ���ƽ���ĽǸֻ��������޷��жϵĽǸ�
		if(pLinePart->GetClassTypeId()!=CLS_LINEANGLE)
			continue;
		hashDrawParts.SetValue(pLinePart->handle,pLinePart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pLinePart;
		pCmd->m_paraPart.hPart=pLinePart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//���㹹�����Ʒ�ʽ 
		CDrawLineAngleTagInfo *pInfo=hashAngleDrawInfo.Add(pLinePart->handle);
		int h0v1e2=CalPartDrawType(pLinePart,dcs,m_pAttachView,pInfo);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}
	for(pLinePart=InsideShortJgSet.GetFirst();pLinePart;pLinePart=InsideShortJgSet.GetNext())
	{	//�ڲ�̽Ǹ�
		if(hashDrawParts.GetValue(pLinePart->handle)!=NULL)
			continue;	//�ѻ���
		hashDrawParts.SetValue(pLinePart->handle,pLinePart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pLinePart;
		pCmd->m_paraPart.hPart=pLinePart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//���㹹�����Ʒ�ʽ 
		int h0v1e2=CalPartDrawType(pLinePart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}
	//���������̽Ǹֹ�ϣ����Ϊ�����̽Ǹ�һ��������࣬�����ƣ�������ͨ��������Ӧ���Ȱ��������ƣ��������ߣ� wjh-2011.8.17
	CHashTable<CLDSLinePart*>hashShortJg;
	hashShortJg.CreateHashTable(OutSideShortJgSet.GetNodeNum());
	for(pLinePart=OutSideShortJgSet.GetFirst();pLinePart;pLinePart=OutSideShortJgSet.GetNext())
		hashShortJg.SetValueAt(pLinePart->handle,pLinePart);
	for(pLinePart=INSIDEJGSETDRAW.GetFirst();pLinePart;pLinePart=INSIDEJGSETDRAW.GetNext())
	{	//�����Ǹ�
		if(hashDrawParts.GetValue(pLinePart->handle)!=NULL)
			continue;	//�ѻ���
		if(hashShortJg.ContainsKey(pLinePart->handle))
			continue;	//�����̽Ǹ�
		hashDrawParts.SetValue(pLinePart->handle,pLinePart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pLinePart;
		pCmd->m_paraPart.hPart=pLinePart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//���㹹�����Ʒ�ʽ 
		int h0v1e2=CalPartDrawType(pLinePart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
		//
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLinePart;
		CLDSGroupLineAngle *pGroupJg=NULL;
		if(pLineAngle->group_father_jg_h>0x20)
			pGroupJg=(CLDSGroupLineAngle*)Ta.FromPartHandle(pLineAngle->group_father_jg_h,CLS_GROUPLINEANGLE);
		//��������������ϽǸ��е����� wht 11-08-03
		if(pGroupJg&&pGroupJg->group_style==2)
			pCmd->m_bExecute=FALSE;
	}
	for(pPart=InsideParamPlateSet.GetFirst();pPart;pPart=InsideParamPlateSet.GetNext())
	{
		if(hashDrawParts.GetValue(pPart->handle)!=NULL)
			continue;	//�ѻ���
		hashDrawParts.SetValue(pPart->handle,pPart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pPart;
		pCmd->m_paraPart.hPart=pPart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//���㹹�����Ʒ�ʽ 
		int h0v1e2=CalPartDrawType(pPart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}
	for(pPart=InSidePlateSet.GetFirst();pPart;pPart=InSidePlateSet.GetNext())
	{
		if(hashDrawParts.GetValue(pPart->handle)!=NULL)
			continue;	//�ѻ���
		hashDrawParts.SetValue(pPart->handle,pPart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pPart;
		pCmd->m_paraPart.hPart=pPart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//���㹹�����Ʒ�ʽ 
		int h0v1e2=CalPartDrawType(pPart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
		if(h0v1e2==0)
			HorizontalPlateSet.append(pPart);
	}
	for(pPart=TubeSet.GetFirst();pPart;pPart=TubeSet.GetNext())
	{
		if(hashDrawParts.GetValue(pPart->handle)!=NULL)
			continue;	//�ѻ���
		hashDrawParts.SetValue(pPart->handle,pPart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pPart;
		pCmd->m_paraPart.hPart=pPart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//���㹹�����Ʒ�ʽ 
		int h0v1e2=CalPartDrawType(pPart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}
	for(pPart=OutSidePlateSet.GetFirst();pPart;pPart=OutSidePlateSet.GetNext())
	{
		if(hashDrawParts.GetValue(pPart->handle)!=NULL)
			continue;	//�ѻ���
		hashDrawParts.SetValue(pPart->handle,pPart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pPart;
		pCmd->m_paraPart.hPart=pPart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//���㹹�����Ʒ�ʽ 
		int h0v1e2=CalPartDrawType(pPart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
		if(h0v1e2==0)
			HorizontalPlateSet.append(pPart);
	}
	for(pPart=OutsideParamPlateSet.GetFirst();pPart;pPart=OutsideParamPlateSet.GetNext())
	{
		if(hashDrawParts.GetValue(pPart->handle)!=NULL)
			continue;	//�ѻ���
		hashDrawParts.SetValue(pPart->handle,pPart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pPart;
		pCmd->m_paraPart.hPart=pPart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//���㹹�����Ʒ�ʽ 
		int h0v1e2=CalPartDrawType(pPart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}	
	for(pPart=LineSlotSet.GetFirst();pPart;pPart=LineSlotSet.GetNext())
	{
		if(hashDrawParts.GetValue(pPart->handle)!=NULL)
			continue;	//�ѻ���
		hashDrawParts.SetValue(pPart->handle,pPart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pPart;
		pCmd->m_paraPart.hPart=pPart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//���㹹�����Ʒ�ʽ 
		int h0v1e2=CalPartDrawType(pPart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}
	for(pPart=LineFlatSet.GetFirst();pPart;pPart=LineFlatSet.GetNext())
	{
		if(hashDrawParts.GetValue(pPart->handle)!=NULL)
			continue;
		hashDrawParts.SetValue(pPart->handle,pPart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->m_paraPart.pPart=pPart;
		pCmd->m_paraPart.hPart=pPart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//���㹹�����Ʒ�ʽ
		int h0v1e2=CalPartDrawType(pPart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}
	for(pLinePart=OUTSIDEJGSETDRAW.GetFirst();pLinePart;pLinePart=OUTSIDEJGSETDRAW.GetNext())
	{	//�����Ǹ�
		if(hashDrawParts.GetValue(pLinePart->handle)!=NULL)
			continue;	//�ѻ���
		if(hashShortJg.ContainsKey(pLinePart->handle))
			continue;	//�����̽Ǹ�
		hashDrawParts.SetValue(pLinePart->handle,pLinePart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pLinePart;
		pCmd->m_paraPart.hPart=pLinePart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//���㹹�����Ʒ�ʽ 
		int h0v1e2=CalPartDrawType(pLinePart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}
	for(pLinePart=OutSideShortJgSet.GetFirst();pLinePart;pLinePart=OutSideShortJgSet.GetNext())
	{	//���̽Ǹ�
		if(hashDrawParts.GetValue(pLinePart->handle)!=NULL)
			continue;	//�ѻ���
		hashDrawParts.SetValue(pLinePart->handle,pLinePart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pLinePart;
		pCmd->m_paraPart.hPart=pLinePart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//���㹹�����Ʒ�ʽ 
		int h0v1e2=CalPartDrawType(pLinePart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}
	//ˮƽ���Ƶĸְ��ϵ���˨����Ҫ������� wht 11-07-26
	for(pPart=HorizontalPlateSet.GetFirst();pPart;pPart=HorizontalPlateSet.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_PLATE)
			continue;
		CLDSPlate *pPlate=(CLDSPlate*)pPart;
		CDrawCoordTrans dct(dcs,1.0/m_fScale);
		CDrawingAssistant *pAssist=m_pAttachView->GetDrawingAssistByHandle(pPlate->handle,ParentDrawing()->m_nSegI);
		if(pAssist)
			pAssist->GetRotateAxisAndAngle(dcs.axis_z,dct.m_xSpreadLine,dct.m_fSpreadAngle);
		DrawLsRefList(pPlate->GetLsRefList(),dct,TRUE);//��ʾ������˨
	}
	//������˨
	CLsDrawUnit *pLsDraw=NULL;
	for(pLsDraw=LSDRAW.GetFirst();pLsDraw;pLsDraw=LSDRAW.GetNext())
	{
		//�˴�һ���й��˵����еĲ�����˨���ף�Ŀǰ�ݸ�Ϊ�ɻ�ͼ��Ա����ͼ��ȥ��
		//if(m_pAttachView&&m_pAttachView->m_iViewType==1&&pLsDraw->front0_side1==1)
		//	continue;	//������ͼ����ʾ������˨����ʱ�ڴ˴����˲�����˨
		if(pLsDraw->pLs==NULL)
			continue;
		if(pLsDraw->pLs->m_bVirtualPart&&pLsDraw->front0_side1==1)
			continue;	//������ʾ��������˨����Ҫ���� wht 11-08-05
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pLsDraw->pLs;
		pCmd->m_paraPart.hPart=pLsDraw->pLs->handle;
		pCmd->m_paraPart.para.bolt.bOriginal=pLsDraw->bOriginal;
		if(pLsDraw->pLs->is_visible==FALSE||pLsDraw->pLs->m_bVirtualPart)	//������˨ wht 11-07-25
			pCmd->m_paraPart.para.bolt.bOriginal=FALSE;
		pCmd->m_paraPart.para.bolt.front0_side1=pLsDraw->front0_side1;
	}
}

//ͼ�淨�߷����仯֮����Ҫ���¹������Ʒ�ʽ wht 11-07-31
void CStruView::UpdatePartDrawTypeAndSpecialDim()
{
	LSDRAW.EmptyDrawList();
	PARTSET HorizontalPlateSet;	//ˮƽ���Ƶĸְ弯��
	CDrawingCommand *pCmd=NULL;
	CHashList<CDrawingCommand*> hashBoltList;
	//���¹������Ʒ�ʽ
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DRAW_PART)
			continue;
		if(pCmd->m_paraPart.pPart==NULL)
			pCmd->m_paraPart.pPart=Ta.FromPartHandle(pCmd->m_paraPart.hPart);
		if(pCmd->m_paraPart.pPart==NULL)
			continue;
		if(pCmd->m_paraPart.pPart->GetClassTypeId()==CLS_BOLT)
		{
			LSDRAW.AppendLsRef((CLDSBolt*)pCmd->m_paraPart.pPart,TRUE,dcs.axis_z);
			hashBoltList.SetValue(pCmd->m_paraPart.hPart,pCmd);
		}
		else 
		{	//���¼��㹹�����Ʒ�ʽ
			int h0v1e2=CalPartDrawType(pCmd->m_paraPart.pPart,dcs,m_pAttachView);
			if(h0v1e2>=0)
				pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
			if(h0v1e2==0&&pCmd->m_paraPart.pPart->GetClassTypeId()==CLS_PLATE)
				HorizontalPlateSet.append(pCmd->m_paraPart.pPart);
		}
	}
	//
	//ˮƽ���Ƶĸְ��ϵ���˨����Ҫ������� wht 11-07-26
	CLDSPart *pPart=NULL;
	for(pPart=HorizontalPlateSet.GetFirst();pPart;pPart=HorizontalPlateSet.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_PLATE)
			continue;
		CLDSPlate *pPlate=(CLDSPlate*)pPart;
		DrawLsRefList(pPlate->GetLsRefList(),CDrawCoordTrans(dcs,m_fScale),TRUE);//��ʾ������˨
	}
	//������˨���Ʋ���
	CLsDrawUnit *pLsDraw=NULL;
	for(pLsDraw=LSDRAW.GetFirst();pLsDraw;pLsDraw=LSDRAW.GetNext())
	{
		CDrawingCommand **ppCmd=hashBoltList.GetValue(pLsDraw->pLs->handle);
		if(ppCmd==NULL)
			continue;
		pCmd=*ppCmd;
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pLsDraw->pLs;
		pCmd->m_paraPart.hPart=pLsDraw->pLs->handle;
		pCmd->m_paraPart.para.bolt.bOriginal=pLsDraw->bOriginal;
		if(pLsDraw->pLs->is_visible==FALSE||pLsDraw->pLs->m_bVirtualPart)	//������˨ wht 11-07-25
			pCmd->m_paraPart.para.bolt.bOriginal=FALSE;
		pCmd->m_paraPart.para.bolt.front0_side1=pLsDraw->front0_side1;
	}
	//ɾ��ԭ�нǸ������༰��˨�������������
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN||pCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
			cmdList.DeleteCursor();
	}
	cmdList.Clean();
	//����������˨�����༰�Ǹ��������ע
	CreateDimSpecialLsSpaceCmdList();
	CreateDimSpecialJgZhunJuCmdList();
}

//���ɹ�����ϸ��ע
void CStruView::CreateDimPartSpecCmdList()
{
	dimpartspec.EmptyPartSpecDim();	//��չ�����ϸ��ע�б�
	CDrawingCommand *pCmd=NULL;
	CLDSPart *pPart=NULL;
	CHashList<CLDSLinePart*> polyPartList;
	CStringKeyHashList<long>hashPartNo;
	CStringKeyHashList<long>hashSlotFootNailPlate;	//�Ŷ���
	BOOL bNeedMirDimPartSpec=TRUE;
	if(m_pAttachView&&m_pAttachView->m_iViewType==1)
		bNeedMirDimPartSpec=FALSE;	//������ͼ����Ҫ�ԳƱ�ע������ϸ wht 11-07-26
	//ʮ�ֲ��������ӹ����б�
	CHashList<long> attachPlateHash;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		CLDSParamPlate *pPlate=(CLDSParamPlate*)pPart;
		if(!pPlate->IsFL()&&pPlate->m_iParamType!=TYPE_XEND)
			continue;
		for(ATTACHPART_ITEM *pItem=pPlate->GetFirstAttachPart();pItem;pItem=pPlate->GetNextAttachPart())
		{
			if(!attachPlateHash.GetValue(pItem->h))
				attachPlateHash.SetValue(pItem->h,pItem->h);
		}
	}
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			continue;	//���ñ�ע��ϽǸ���ϸ wht 11-07-29
		if(attachPlateHash.GetValue(pPart->handle))
			continue;	//����עʮ�ֲ����߷������ӹ�����ϸ wht 12-03-22
		if(pPart->IsLinePart()||pPart->IsPlate())
		{	
			BOOL bDimPartNoOnly=FALSE;
			if(pPart->IsLinePart())
			{
				CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
				if(pLinePart->bHuoQuStart||pLinePart->bHuoQuEnd)
				{	//����������������,������������עһ�α�ż��� wht 11-06-21
					if(polyPartList.GetValue(pLinePart->huoqu_handle)==NULL)
						polyPartList.SetValue(pLinePart->huoqu_handle,pLinePart);
					continue;
				}
				CLDSLinePart *pMirLinePart=NULL;
				if(sys.dim_map.PartNo.iMirMsg==0)	//X��Գ�
					pMirLinePart=(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(1));
				else if(sys.dim_map.PartNo.iMirMsg==1)	//Y��Գ�
					pMirLinePart=(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(2));
				if(hashPartNo.GetValue(pLinePart->GetPartNo()))	//�Ѵ���ͬ������Ż�ԳƱ�עʱ�Ѵ��ڶԳƹ������
					//||(pMirLinePart&&hashPartNo.GetValue(pMirLinePart->sPartNo)))	
					bDimPartNoOnly=TRUE;
				else 
					hashPartNo.Add(pLinePart->GetPartNo());
				//�Ŷ���
				if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
				{
					CLDSLineSlot *pLineSlot=(CLDSLineSlot*)pLinePart;
					if(pLineSlot->IsFootNailPlate())
					{
						long *pFatherPart=hashSlotFootNailPlate.GetValue(pLineSlot->GetPartNo());
						if(pFatherPart&&*pFatherPart==pLineSlot->m_hPartWeldParent)
							continue;	//ͬһ���ֹ���ͬһ��ŵĽŶ�����Ҫ��עһ�� wht 11-07-21
						else 
							hashSlotFootNailPlate.SetValue(pLineSlot->GetPartNo(),pLineSlot->m_hPartWeldParent);
					}
				}
			}
			pCmd=cmdList.append();
			pCmd->m_pDrawingCom=this;
			pCmd->m_eCmdType=CMD_DIM_PARTSPEC;//��ע������ϸ
			pCmd->InitCommandPara();
			pCmd->m_paraPartSpec.pObj=pPart;
			pCmd->m_paraPartSpec.hObj=pPart->handle;
			if(!bNeedMirDimPartSpec)	//������ͼ����Ҫ�ԳƱ�ע������ϸ 
				pCmd->m_paraPartSpec.iMirMsg=2;
			if(bDimPartNoOnly)	//�ڶ��γ��ֵ�ͬ��Ÿ˼�������ע��� wht 11-07-21
				pCmd->m_paraPartSpec.bOnlyDimPartNo=bDimPartNoOnly;
			if(pPart->IsPlate())
			{
				BOOL bStdFl=FALSE,bDrawRefLine=TRUE;
				if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				{
					CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
					bStdFl=pParamPlate->m_bStdPart&&pParamPlate->IsFL();//��׼��������Ҫ����������
					//��������׼����岻��Ҫ��ע��񼰺�� wht 12-03-22
					if(pParamPlate->IsFL()||(pParamPlate->m_bStdPart&&pParamPlate->IsInsertPlate()))
					{
						pCmd->m_paraPartSpec.bOnlyDimPartNo=TRUE;
						bDrawRefLine=FALSE;
					}
				}
				if(!bStdFl&&fabs(dcs.axis_z*pPart->ucs.axis_z)<0.707)
					pCmd->m_paraPartSpec.bDrawRefLine=TRUE;	//��ֱ���Ƶĸְ���Ҫ���������� wht 11-07-21
				else if(pPart->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pPart)->jdb_style==4)
					pCmd->m_paraPartSpec.bDrawRefLine=TRUE;	//��ע��˨���ʱĬ�϶����������� wht 11-07-18
				if(bDrawRefLine==FALSE)
					pCmd->m_paraPartSpec.bDrawRefLine=FALSE;//������������ wht 12-03-23
			}
			else if(pPart->GetClassTypeId()==CLS_LINESLOT&&((CLDSLineSlot*)pPart)->IsFootNailPlate())
			{	//��ע�Ŷ�����ʱ��Ҫ����������,����Ҫ��ע���ȼ����
				pCmd->m_paraPartSpec.bDrawRefLine=TRUE;		
				pCmd->m_paraPartSpec.bOnlyDimPartNo=TRUE;
			}
		}
	}
	//������������עһ�α��
	CLDSLinePart **ppLinePart=NULL;
	for(ppLinePart=polyPartList.GetFirst();ppLinePart;ppLinePart=polyPartList.GetNext())
	{
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DIM_PARTSPEC;//��ע������ϸ
		pCmd->InitCommandPara();
		if(!bNeedMirDimPartSpec)	//������ͼ����Ҫ�ԳƱ�ע������ϸ 
			pCmd->m_paraPartSpec.iMirMsg=2;
		pCmd->m_paraPartSpec.pObj=*ppLinePart;
		pCmd->m_paraPartSpec.hObj=(*ppLinePart)->handle;
	}
}
//��������ͷ��ע����
void CStruView::CreateDimOddmentCmdList()
{
	CLDSPart *pPart=NULL;
	CDrawingCommand *pCmd=NULL;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(!pPart->IsLinePart())
			continue;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;	//�̽Ǹ֡��Ŷ��岻�ñ�ע����ͷ
		if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			continue;	//����ע��ϽǸ�����ͷ 
		double fStartOdd=pLinePart->startOdd();
		double fEndOdd=pLinePart->endOdd();
		int start_std_odd=0,end_std_odd=0;
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSLineAngle *pAngle=(CLDSLineAngle*)pLinePart;
			CDrawLineAngleTagInfo *pInfo=hashAngleDrawInfo.GetValue(pLinePart->handle);
			char cDrawStyle;
			if(pInfo)
				cDrawStyle=toupper(pInfo->m_cDrawStyle);
			else
			{
				CDrawPartTagInfo info;
				CalPartDrawType(pAngle,dcs,m_pAttachView,&info);
				cDrawStyle=toupper(info.m_cDrawStyle);
			}
			if(cDrawStyle=='X')
			{
				fStartOdd+=pAngle->GetStartExtraOdd(0);
				fEndOdd+=pAngle->GetEndExtraOdd(0);
			}
			else if(cDrawStyle=='Y')
			{
				fStartOdd+=pAngle->GetStartExtraOdd(1);
				fEndOdd+=pAngle->GetEndExtraOdd(1);
			}
			else
				continue;	//������ֱ����ĸ˼�����ע����ͷ
			//double start_ls_len,end_ls_len;
			int start_ls_d,end_ls_d;
			if(pAngle->GetStartEndBoltD(&start_ls_d,&end_ls_d))
			{
				LSSPACE_STRU LsSpace;
				GetLsSpace(LsSpace,start_ls_d);
				start_std_odd=LsSpace.EndSpace;
				GetLsSpace(LsSpace,end_ls_d);
				end_std_odd=LsSpace.EndSpace;
			}
			if(pInfo)
			{
				pInfo->start_std_odd=start_std_odd;
				pInfo->end_std_odd=end_std_odd;
			}
		}
		if((ftoi(fStartOdd)==0||ftoi(fStartOdd)==start_std_odd)&&(ftoi(fEndOdd)==0||ftoi(fEndOdd)==end_std_odd))
			continue;	//������ͷ
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DIM_ODDMENT;//��ע�˼�����ͷ
		pCmd->InitCommandPara();
		pCmd->m_paraOddment.pLinePart=pLinePart;
		pCmd->m_paraOddment.hLinePart=pPart->handle;
	}
}
//���ɽŶ�����ע����
void CStruView::CreateDimFootNailSpaceCmdList()
{
	CLDSPart *pPart=NULL;
	CDrawingCommand *pCmd=NULL;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(!pPart->IsLinePart())
			continue;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;	//��ͳ�ƶ̽Ǹ�
		if(pLinePart->IsTube())
		{
			CLDSLineTube *pLineTube=(CLDSLineTube*)pLinePart;
			if(pLineTube->GetFootNailCount()<=0)
				continue;
		}
		else if(pLinePart->IsAngle())
		{
			CLsRef *pLsRef=NULL;
			int nFootNailCount=0;
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLinePart;
			for(pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
			{
				if(pLsRef->GetLsPtr()->IsFootNail())
					nFootNailCount++;
			}
			//�˼��ϵĽŶ���������1����Ϊ��Ҫ��ע�Ŷ����,�����ע��������˨Ϊ�Ŷ������ wht 11-07-25
			if(nFootNailCount<=1)	
				continue;
		}
		else 
			continue;
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DIM_FOOTNAIL_SPACE;//��ע�˼��Ŷ����
		pCmd->InitCommandPara();
		pCmd->m_paraDimFootNailSpace.pLinePart=pLinePart;
		pCmd->m_paraDimFootNailSpace.hLinePart=pPart->handle;
		pCmd->m_paraDimFootNailSpace.fSpace=5;
		pCmd->m_paraDimFootNailSpace.fDimDistance=10;
	}
}
//����������ע����
void CStruView::CreateDimFillPlateSpaceCmdList()
{
	CLDSPart *pPart=NULL;
	CDrawingCommand *pCmd=NULL;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
			continue;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;	//��ͳ�ƶ̽Ǹ�
		int nFillPlateCount=0;
		for (CLDSPlate *pFillPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pFillPlate;pFillPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
		{
			if (pFillPlate->designInfo.origin.des_para.SCALE_NODE.hLinePart!=pLinePart->handle)
				continue;
			if (pFillPlate->designInfo.origin.datum_pos_style!=10)	
				continue;
			nFillPlateCount++;
		}
		//��ϽǸ��ϵ�����������1����Ϊ��Ҫ��ע�����,�����ע�����Ӱ�Ϊ�������
		if (nFillPlateCount<=1)
			continue;
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DIM_FILLPLATE_SPACE;//��ע��ϽǸ������
		pCmd->InitCommandPara();
		pCmd->m_paraDimFillPlateSpace.pLinePart=pLinePart;
		pCmd->m_paraDimFillPlateSpace.hLinePart=pPart->handle;
		pCmd->m_paraDimFillPlateSpace.fSpace=5;
		pCmd->m_paraDimFillPlateSpace.fDimDistance=10;
	}
}
//������˨�������ע����
void CStruView::CreateDimSpecialLsSpaceCmdList()
{	//ͳ����˨���
	CLDSPart *pPart=NULL;
	LSSPACEDIM.Empty();
	BOOL bDimSpecialLsSpace=sys.stru_map.bDimSpecialLsSpace;
	sys.stru_map.bDimSpecialLsSpace=TRUE;
	for(pPart=partSet.GetFirst();pPart!=NULL;pPart=partSet.GetNext())
	{
		if(!pPart->IsPlate())
			continue;
		CViewPart *pViewPart=NULL;
		if(m_pAttachView&&m_pAttachView->m_iViewType==1)
			pViewPart=m_pAttachView->FindViewPart(pPart->handle);
		if(pViewPart&&pViewPart->cFlag=='X'		//ƽ�̻���
			||fabs(dcs.axis_z*pPart->ucs.axis_z)>0.707)
		{
			CDimPlankLs dimls;
			if(pPart->GetClassTypeId()==CLS_PLATE)
				dimls.Create((CLDSPlate*)pPart);
			else 
				dimls.Create((CLDSParamPlate*)pPart);
		}
	}
	sys.stru_map.bDimSpecialLsSpace=bDimSpecialLsSpace;
	//
	CLsSpaceDimUnit *pSpaceDim=NULL;
	for(pSpaceDim=LSSPACEDIM.GetFirst();pSpaceDim;pSpaceDim=LSSPACEDIM.GetNext())
	{
		CDrawingCommand *pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DIM_SPECIAL_LSSPACE;
		pCmd->InitCommandPara();
		pCmd->m_paraDimLsSpace.spaceDim=*pSpaceDim;
		pCmd->m_paraDimLsSpace.fDimDistance=6;
		pCmd->m_paraDimLsSpace.fSpace=0;
		pCmd->m_paraDimLsSpace.hPlate=pSpaceDim->hFatherPlate;
		pCmd->m_paraDimLsSpace.pPlate=Ta.FromPartHandle(pSpaceDim->hFatherPlate);
	}
}
//���ɽǸ�����׼�ݱ�ע����
void CStruView::CreateDimSpecialJgZhunJuCmdList()
{
	CLDSPart *pPart=NULL;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_LINEANGLE)
			continue;
		CLDSLineAngle *pJg=(CLDSLineAngle*)pPart;
		if(pJg->pStart==NULL||pJg->pEnd==NULL)
			continue;	//�̽Ǹ�
		if(pJg->m_bFootNail)
			continue;	//�ǸֽŶ�
		pJg->getUCS(pJg->ucs);
		f3dPoint cur_jg_vec=pJg->End()-pJg->Start();
		normalize(cur_jg_vec);
		//
		CViewPart *pViewPart=NULL;
		if(m_pAttachView&&m_pAttachView->m_iViewType==1)
			pViewPart=m_pAttachView->FindViewPart(pJg->handle);
		if((pViewPart&&pViewPart->cFlag=='Z')||fabs(cur_jg_vec*dcs.axis_z)>0.70710)
			continue;
		int x_wing0_y_wing1=0;
		if(pViewPart&&(pViewPart->cFlag=='X'||pViewPart->cFlag=='S'))
			x_wing0_y_wing1=0;	//X֫Ϊ��ǰ����֫
		else if(pViewPart&&(pViewPart->cFlag=='Y'||pViewPart->cFlag=='Q'))
			x_wing0_y_wing1=1;	//Y֫Ϊ��ǰ����֫
		else
			IsInsideJg(pJg,dcs.axis_z,&x_wing0_y_wing1);
		f3dPoint work_wing_norm;
		JGZJ jgzj,jgzj2;
		getjgzj(jgzj,pJg->GetWidth());
		if(x_wing0_y_wing1==0)
		{
			work_wing_norm = pJg->get_norm_x_wing();
			if(pJg->m_bEnableTeG)
				jgzj2 = pJg->xWingXZhunJu;
		}
		else 
		{
			work_wing_norm = pJg->get_norm_y_wing();
			if(pJg->m_bEnableTeG)
				jgzj2 = pJg->xWingYZhunJu;
		}
		
		CDimJgZhun dim_jg_zhun(pJg);	//��������Ǹ��ϵ���˨׼����
		for(CLsRef *pLsRef=pJg->GetFirstLsRef();pLsRef!=NULL;pLsRef=pJg->GetNextLsRef())
		{
			if((*pLsRef)->IsFootNail())
				continue;
			if(fabs(dot_prod((*pLsRef)->get_norm(),work_wing_norm))<0.707)
				continue;	//���ǵ�ǰ����˨������
			dim_jg_zhun.DimLsZhun(pLsRef);	//���������϶���˨׼����
		}
		dim_jg_zhun.EndDim();
		BOOL bHasSameDim=FALSE;
		int i=0,n=dim_jg_zhun.GetZhunDimCount();
		for(i=0;i<n;i++)
		{
			CZhunXianTextDim zhun_dim;
			dim_jg_zhun.GetZhunDimAt(i,&zhun_dim);
			if(fabs(zhun_dim.dist)<EPS)
				continue;
			if(pJg->m_bEnableTeG&&fabs(jgzj2.g-zhun_dim.dist)<1)
			{	//��������׼������˨����Ϊ��ǰ�Ǹ�׼������׼��ʱ����Ҫ��עһ�� wht 11-06-21
				if(bHasSameDim)
					zhun_dim.bDimZhunJu=false;
				else
					bHasSameDim=TRUE;
			}
			else if(fabs(jgzj.g-zhun_dim.dist)<1||fabs(jgzj.g1-zhun_dim.dist)<1
				||fabs(jgzj.g2-zhun_dim.dist)<1||fabs(jgzj.g1-zhun_dim.dist)<1)
				zhun_dim.bDimZhunJu=false;	//δ��������׼��,��˨������׼��Ϊ��׼׼��,����ӱ�ע����׼������(���������˨��ʱ�ٽ������) wht 11-06-30
			CDrawingCommand *pCmd=cmdList.append();
			pCmd->m_pDrawingCom=this;
			pCmd->m_eCmdType=CMD_DIM_SPECIAL_JGZHUN;
			pCmd->InitCommandPara();
			pCmd->m_paraDimJgZhun.hLineAngle=pJg->handle;
			pCmd->m_paraDimJgZhun.pLineAngle=pJg;
			pCmd->m_paraDimJgZhun.zhun_dim=zhun_dim;
		}
	}
}
//from DrawUnit.cpp
BOOL IsOrthoRectInters(f3dPoint topLeft1,double width1,double height1,f3dPoint topLeft2,double width2,double height2);
//������˨��Ϣ��ע�����б�
void CStruView::CreateDimBoltInfoCmdList()
{
	// 1.ͳ������������Ƶ���˨
	BOLTSET boltSet;
	CHashList<long> boltHandleHash;
	CDrawingCommand *pCmd=NULL;
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DRAW_PART)
			continue;
		if(pCmd->m_paraPart.pPart==NULL)
			pCmd->m_paraPart.pPart=Ta.FromPartHandle(pCmd->m_paraPart.hPart);
		if(pCmd->m_paraPart.pPart==NULL)
			continue;
		if(pCmd->m_paraPart.pPart->GetClassTypeId()!=CLS_BOLT)
			continue;	//����˨
		if(pCmd->m_paraPart.para.bolt.front0_side1!=0)
			continue;	//�����������˨
		CLDSBolt *pBolt=(CLDSBolt*)pCmd->m_paraPart.pPart;
		if(pBolt->IsFootNail())
			continue;	//��ע��˨��Ϣʱ����ע�Ŷ���Ϣ
		if(!boltHandleHash.GetValue(pBolt->handle))
		{
			boltSet.append(pBolt);
			boltHandleHash.Add(pBolt->handle);
		}
	}
	// 2.����˨����
	CLDSBolt *pBolt=NULL;
	ATOM_LIST<CLsGroupDimUnit> lsGroupDimList;
	CLsGroupDimUnit *pLsGroupDim=NULL;
	for(pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
	{
		for (pLsGroupDim=lsGroupDimList.GetFirst();pLsGroupDim;pLsGroupDim=lsGroupDimList.GetNext())
		{
			CLsGroupDimUnit::BOLTINFO boltInfo(pBolt);
			boltInfo.pos2d=pLsGroupDim->TransToLocalCoord(pBolt->ucs.origin);
			if(pLsGroupDim->IsIncludeBolt(boltInfo))
			{
				pLsGroupDim->AddBolt(boltInfo);
				break;
			}
		}
		if(pLsGroupDim==NULL)
		{	//����µ���˨��
			pLsGroupDim=lsGroupDimList.append();
			pLsGroupDim->Init(pBolt);
		}
	}
	// 3.�ϲ�����Ͻ��ķ���
	for(pLsGroupDim=lsGroupDimList.GetFirst();pLsGroupDim;pLsGroupDim=lsGroupDimList.GetNext())
	{
		int pushed=lsGroupDimList.push_stack();
		pLsGroupDim->group_scope.fMinZ=pLsGroupDim->group_scope.fMinZ=0;
		for(CLsGroupDimUnit* pAfterGroup=lsGroupDimList.GetNext();pAfterGroup;pAfterGroup=lsGroupDimList.GetNext())
		{
			if(pLsGroupDim->GetBoltD()!=pAfterGroup->GetBoltD()||pLsGroupDim->GetBoltL()!=pAfterGroup->GetBoltL())
				continue;
			if(pLsGroupDim->ucs.axis_x*pAfterGroup->ucs.axis_x<=EPS_COS2)	//��ע������ƹ���
				continue;	//��ע����ͬ���ܺϲ�
			f3dPoint org=pAfterGroup->ucs.origin;
			coord_trans(org,pLsGroupDim->ucs,FALSE,TRUE);
			SCOPE_STRU scope=pAfterGroup->group_scope;
			scope.fMinX+=org.x;
			scope.fMaxX+=org.x;
			scope.fMinY+=org.y;
			scope.fMaxY+=org.y;
			scope.fMinZ=scope.fMaxZ=0;
			//����˨���������϶��ֵ����������Χ wht 12-03-23
			scope.fMinX-=CLsGroupDimUnit::fLsSpaceVerticalThreshold;
			scope.fMaxX+=CLsGroupDimUnit::fLsSpaceVerticalThreshold;
			scope.fMinY-=CLsGroupDimUnit::fLsSpaceHorizontalThreshold;
			scope.fMaxY+=CLsGroupDimUnit::fLsSpaceHorizontalThreshold;
			if(pLsGroupDim->group_scope.IsIntersWith(scope))
			{
				for(CLsGroupDimUnit::BOLTINFO *pBoltInfo=pAfterGroup->GetFirstBoltInfo();pBoltInfo;pBoltInfo=pAfterGroup->GetNxetBoltInfo())
				{
					if(pBoltInfo->GetBolt()==NULL)
						continue;
					CLsGroupDimUnit::BOLTINFO boltInfo(pBoltInfo->GetBolt());
					boltInfo.pos2d=pLsGroupDim->TransToLocalCoord(pBoltInfo->GetBolt()->ucs.origin);
					pLsGroupDim->AddBolt(boltInfo);
				}
				lsGroupDimList.DeleteCursor();
			}
		}
		lsGroupDimList.pop_stack(pushed);
	}
	// 4.�����˨��Ϣ��ע����
	for(pLsGroupDim=lsGroupDimList.GetFirst();pLsGroupDim;pLsGroupDim=lsGroupDimList.GetNext())
	{
		CDrawingCommand *pBoltInfoCmd=cmdList.append();
		pBoltInfoCmd->m_pDrawingCom=this;
		pBoltInfoCmd->m_eCmdType=CMD_DIM_BOLTINFO;
		pBoltInfoCmd->InitCommandPara();
		pBoltInfoCmd->m_paraBoltGroupInfo.lsGroupDim.CopyProperty(pLsGroupDim);
	}
}
//������˨��Ȧ��ע�����б�
void CStruView::CreateDimBoltPadCmdList()
{
	BOLTSET boltSet;
	CDrawingCommand *pCmd=NULL;
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DRAW_PART)
			continue;
		if(pCmd->m_paraPart.pPart==NULL)
			pCmd->m_paraPart.pPart=Ta.FromPartHandle(pCmd->m_paraPart.hPart);
		if(pCmd->m_paraPart.pPart==NULL)
			continue;
		if(pCmd->m_paraPart.pPart->GetClassTypeId()!=CLS_BOLT)
			continue;	//����˨
		if(pCmd->m_paraPart.para.bolt.front0_side1!=0)
			continue;	//�����������˨
		CLDSBolt *pBolt=(CLDSBolt*)pCmd->m_paraPart.pPart;
		if(pBolt->DianQuan.N<=0||pBolt->DianQuan.thick<=0)
			continue;	//�޵�Ȧ
		boltSet.append(pBolt);
	}
	//��ӵ�Ȧ��ע����
	CLDSBolt *pBolt=NULL;
	for(pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
	{
		CDrawingCommand *pBoltPadCmd=cmdList.append();
		pBoltPadCmd->m_pDrawingCom=this;
		pBoltPadCmd->m_eCmdType=CMD_DIM_BOLTPAD;
		pBoltPadCmd->InitCommandPara();
		pBoltPadCmd->m_paraBoltPad.hBolt=pBolt->handle;
		pBoltPadCmd->m_paraBoltPad.pBolt=pBolt;
	}
}

//���Ƹ����������б�
void CStruView::CreateDrawAssistLineCmdList()
{
	if(m_pAttachView==NULL)//||m_pAttachView->m_iViewType!=1)
		return;
	CDrawingAssistant *pAssist=NULL;
	for(pAssist=m_pAttachView->listAssist.GetFirst();pAssist;pAssist=m_pAttachView->listAssist.GetNext())
	{
		if(m_pAttachView->m_iViewType==0&&pAssist->m_iSeg.iSeg!=ParentDrawing()->m_nSegI.iSeg)
			continue;
		if(pAssist->m_nStyle!=1&&pAssist->m_nStyle!=2)
			continue;
		CDrawingCommand *pAssistCmd=cmdList.append();
		pAssistCmd->m_pDrawingCom=this;
		pAssistCmd->m_eCmdType=CMD_DRAW_ASSIST_LINE;
		pAssistCmd->InitCommandPara();
		pAssistCmd->m_paraDrawAssistLine.iNo=pAssist->iNo;
		pAssistCmd->m_paraDrawAssistLine.nStyle=pAssist->m_nStyle;
	}
}

//������������
BOOL CStruView::CreateCmdList(BOOL bInit/*=TRUE*/)
{
	hashAngleDrawInfo.Empty();
	//1.��ȡ��Ҫ���ƵĹ���
	GetDrawingObjectSet(bInit);
	//2.������Ӧ�����б�
	cmdList.Empty();
	//2.1 ���ƹ���
	CreateDrawingPartCmdList();
	//2.2 ���Ƹ�����
	CreateDrawAssistLineCmdList();
	//2.3 ��ע����ͷ
	CreateDimOddmentCmdList();
	//2.4 ��ע������ϸ
	CreateDimPartSpecCmdList();
	//2.5 ��ע��˨������
	CreateDimSpecialLsSpaceCmdList();
	//2.6 ��ע�Ǹ�����׼��
	CreateDimSpecialJgZhunJuCmdList();
	//2.7 ��ע��˨��Ϣ
	CreateDimBoltInfoCmdList();
	//2.8 ��ע��˨��Ȧ��Ϣ
	CreateDimBoltPadCmdList();
	//2.9 ��ע������
	//��������ʱbInitΪFALSE,Ҳ��Ҫ�������������,�����޷���ע������ wht 12-05-31
	//bInit==FALSE,����Ҫ���³�ʼ�������α�ע�������������ͼԪʱ���ٴ���ʾ��ע�����Σ�
	//�����ע�����ο��ֶ������ӽ��б�ע wht 13-04-19
	if(bInit)	
	{	//��ʼ�������б�ʱ����Ҫ�������������
		if(m_pAttachView&&m_pAttachView->m_iViewType<2)
		{	
			CDrawingCommand *pCmd=NULL;
			pCmd=cmdList.append();
			pCmd->m_pDrawingCom=this;
			pCmd->m_eCmdType=CMD_DIM_SIMPOLY;
			pCmd->InitCommandPara();
			pCmd->m_paraSimPoly.nSimPolyCmdType=1;	//�����������������������β���
			pCmd=cmdList.append();
			pCmd->m_pDrawingCom=this;
			pCmd->m_eCmdType=CMD_DIM_SIMPOLY;
			pCmd->InitCommandPara();
			pCmd->m_paraSimPoly.nSimPolyCmdType=0;	//�����������������ע����������
		}
	}
	//2.10 ��ע�Ŷ��������
	if(m_pAttachView&&m_pAttachView->m_iViewType==0)
	{
		CreateDimFootNailSpaceCmdList();	//������ͼ����ͼ�б�ע�Ŷ����
		CreateDimFillPlateSpaceCmdList();	//������ͼ����ͼ�б�ע�����
	}
	if(bInit)	//3.������ͼ��ʼװ��λ��
		CalInitAssemblyPosition();
	return TRUE;
}

//���������б�
void CStruView::UpdateCmdList()
{
	CHashList<CDrawingCommand> hashDrawPartList;		//key:pPart->handle
	CHashList<CDrawingCommand> hashDimOddmentList;		//key:pPart->handle
	CHashList<CDrawingCommand> hashPartSpecList;		//key:pPart->handle
	CStringKeyHashList<CDrawingCommand> hashLsSpaceList;//key:"startLs->handle endLs->handle"
	CStringKeyHashList<CDrawingCommand> hashJgZhunJuList;//key:"pJg->handle hRelaBolt"
	CHashList<CDrawingCommand> hashBoltPadList;			//key:pPart->handle ��ע��˨��Ȧ��Ϣ
	ATOM_LIST<CDrawingCommand> simPolyList;
	CHashList<CDrawingCommand> hashFootNailSpaceList;	//key:pPart->handle
	CHashList<CDrawingCommand> hashFillPlateSpaceList;	//key:pPart->handle
	CDrawingCommand *pCmd=NULL;
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		CDrawingCommand *pNewCmd=NULL;
		if(pCmd->m_eCmdType==CMD_DRAW_PART)
			pNewCmd=hashDrawPartList.Add(pCmd->m_paraPart.hPart);
		else if(pCmd->m_eCmdType==CMD_DIM_PARTSPEC)
			pNewCmd=hashPartSpecList.Add(pCmd->m_paraPartSpec.hObj);
		else if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
		{
			if(pCmd->m_paraDimJgZhun.zhun_dim.hRelaBolt<0x20)
				continue;	//�޹�����˨Ϊ�ɰ汾�ĽǸ�׼��
			char sKey[100]="";
			sprintf(sKey,"0X%X 0X%X",pCmd->m_paraDimJgZhun.hLineAngle,pCmd->m_paraDimJgZhun.zhun_dim.hRelaBolt);
			pNewCmd=hashJgZhunJuList.Add(sKey);
		}	
		else if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
		{
			char sKey[100]="";
			sprintf(sKey,"0X%X 0X%X",pCmd->m_paraDimLsSpace.spaceDim.hStartLs,pCmd->m_paraDimLsSpace.spaceDim.hEndLs);
			pNewCmd=hashLsSpaceList.Add(sKey);
		}
		else if(pCmd->m_eCmdType==CMD_DIM_BOLTPAD)
			pNewCmd=hashBoltPadList.Add(pCmd->m_paraBoltPad.hBolt);
		else if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY)	
			pNewCmd=simPolyList.append();
		else if(pCmd->m_eCmdType==CMD_DIM_ODDMENT)
			pNewCmd=hashDimOddmentList.Add(pCmd->m_paraOddment.hLinePart);
		else if(pCmd->m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
			pNewCmd=hashFootNailSpaceList.Add(pCmd->m_paraDimFootNailSpace.hLinePart);
		else if(pCmd->m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
			pNewCmd=hashFillPlateSpaceList.Add(pCmd->m_paraDimFillPlateSpace.hLinePart);
		else 
			continue;
		pNewCmd->CopyProperty(pCmd);
	}
	//���»�������б�
	CreateCmdList(FALSE);
	//�Ա������ɵ������б���ԭ�������б�,��˳���ƾɵ��������ֵ��Ӧ��������
	CDrawingCommand *pOldCmd=NULL;
	// ������������
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DRAW_PART)
			continue;
		pOldCmd=hashDrawPartList.GetValue(pCmd->m_paraPart.hPart);
		if(pOldCmd)
		{
			if(pCmd->m_paraPart.pPart==NULL)
				pCmd->m_paraPart.pPart=Ta.FromPartHandle(pCmd->m_paraPart.hPart);
			if(pCmd->m_paraPart.pPart&&pCmd->m_paraPart.pPart->GetClassTypeId()==CLS_BOLT)
			{
				pCmd->m_bExecute=pOldCmd->m_bExecute;
				continue;	//������˨�����Ҫ����ԭ���������(������˨λ�ò���ʵʱ����)
			}
			char cDrawFlag=pCmd->m_paraPart.cDrawFlag;	//�������Ʒ�ʽ
			pCmd->CopyProperty(pOldCmd);
			pCmd->m_paraPart.cDrawFlag=cDrawFlag;		//�������й���,���Ʒ�ʽ
		}
	}
	// ����ͷ��ע����
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_ODDMENT)
			continue;
		pOldCmd=hashDimOddmentList.GetValue(pCmd->m_paraOddment.hLinePart);
		if(pOldCmd)
		{
			pCmd->CopyProperty(pOldCmd);
			//��Ϊ�����Ա��״α�ע����ͷʱ����ʶ���������ͷ��ע����wjh-2011.9.17
			pCmd->m_paraOddment.fPoleAngle=-1.57;
		}
	}
	// ��ע������ϸ����
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_PARTSPEC)
			continue;
		pOldCmd=hashPartSpecList.GetValue(pCmd->m_paraPartSpec.hObj);
		if(pOldCmd)
			pCmd->CopyProperty(pOldCmd);
	}
	//	�Ǹ�����׼��
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_SPECIAL_JGZHUN)
			continue;
		char sKey[100]="";
		sprintf(sKey,"0X%X 0X%X",pCmd->m_paraDimJgZhun.hLineAngle,pCmd->m_paraDimJgZhun.zhun_dim.hRelaBolt);
		pOldCmd=hashJgZhunJuList.GetValue(sKey);
		if(pOldCmd)
		{	//�������û����޸õı���,������zhun_dim,��Ϊ��˨λ�ÿ��ܱ仯��������Ҫʹ���������� wht 11-08-10
			pCmd->m_paraDimJgZhun.bInside=pOldCmd->m_paraDimJgZhun.bInside;
			pCmd->m_paraDimJgZhun.fDimDistance=pOldCmd->m_paraDimJgZhun.fDimDistance;
			pCmd->m_paraDimJgZhun.fSpace=pOldCmd->m_paraDimJgZhun.fSpace;
		}
	}
	//  ��˨������
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_SPECIAL_LSSPACE)
			continue;
		char sKey[100]="";
		sprintf(sKey,"0X%X 0X%X",pCmd->m_paraDimLsSpace.spaceDim.hStartLs,pCmd->m_paraDimLsSpace.spaceDim.hEndLs);
		pOldCmd=hashLsSpaceList.GetValue(sKey);
		if(pOldCmd)
			pCmd->CopyProperty(pOldCmd);
	}	
	// ��ע��˨��Ϣ����ע��˨��Ȧ������ʱ��δ���
	// ��Ȧ��ע����
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_BOLTPAD)
			continue;
		pOldCmd=hashBoltPadList.GetValue(pCmd->m_paraBoltPad.hBolt);
		if(pOldCmd)
			pCmd->CopyProperty(pOldCmd);
	}
	// �Ŷ�����ע����
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_FOOTNAIL_SPACE)
			continue;
		pOldCmd=hashFootNailSpaceList.GetValue(pCmd->m_paraDimFootNailSpace.hLinePart);
		if(pOldCmd)
			pCmd->CopyProperty(pOldCmd);
	}
	// ������ע����
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_FILLPLATE_SPACE)
			continue;
		pOldCmd=hashFillPlateSpaceList.GetValue(pCmd->m_paraDimFillPlateSpace.hLinePart);
		if(pOldCmd)
			pCmd->CopyProperty(pOldCmd);
	}
	// ��ע����������ֱ�Ӹ���ԭ���������б���
	CDrawingCommand *pSimPolyCmd=NULL;
	for(pSimPolyCmd=simPolyList.GetFirst();pSimPolyCmd;pSimPolyCmd=simPolyList.GetNext())
	{
		pCmd=cmdList.append();
		pCmd->CopyProperty(pSimPolyCmd);
	}
}

//�õ��������ʵ�弯�� wht 12-03-22
void CStruView::GetDimBoltInfoCmdAttachEntSet(CDrawingCommand *pMainCmd, ATOM_LIST<AcDbObjectId> &entIdList)
{
	if(pMainCmd->m_eCmdType!=CMD_DIM_BOLTINFO)
		return;
	CDrawingCommand *pCmd=NULL;
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DRAW_PART)
			continue;
		if(pCmd->m_paraPart.pPart==NULL)
			continue;
		if(pCmd->m_paraPart.pPart->GetClassTypeId()!=CLS_BOLT)
			continue;
		if(!pMainCmd->m_paraBoltGroupInfo.lsGroupDim.ContainsBolt(pCmd->m_paraPart.hPart))
			continue;
		AcDbObjectId *pEntId=NULL;
		for(pEntId=pCmd->entIdList.GetFirst();pEntId;pEntId=pCmd->entIdList.GetNext())
			entIdList.append(*pEntId);
	}
}
//���ݹ�������õ���Ӧ�Ļ������� wht 12-03-22
CDrawingCommand* CStruView::GetDrawingPartCmdFromHandle(long hPart)
{
	CDrawingCommand *pCmd=NULL;
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DRAW_PART)
			continue;
		if(pCmd->m_paraPart.hPart==hPart)
			return pCmd;
	}
	return NULL;
}

//��(����ǰ�������)��˨��ע�������Ƴ�ָ����˨�������ع������� 
void CStruView::RemovBoltFromDimBoltInfoCmd(CDrawingCommand *pCurCmd,BOLTSET &boltSet,
											CHashList<CDrawingCommand*> &cmdHashList)
{
	CDrawingCommand *pCmd=NULL;
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_BOLTINFO)
			continue;
		if(pCmd==pCurCmd)
			continue;
		CLDSBolt *pBolt=NULL;
		for(pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
		{
			BOOL bRet=pCmd->m_paraBoltGroupInfo.lsGroupDim.RemoveBolt(pBolt);
			if(bRet&&cmdHashList.GetValue((long)pCmd)==NULL)
				cmdHashList.SetValue((long)pCmd,pCmd);
		}
	}
}

CString CStruView::MakeSpreadFaceSetStr()
{
	CString strSpreadFace="0.��׼��|";
	if(m_pAttachView&&m_pAttachView->m_iViewType!=1)
	{
		CDrawingAssistant *pAssist=NULL;
		for(pAssist=m_pAttachView->listAssist.GetFirst();pAssist;pAssist=m_pAttachView->listAssist.GetNext())
		{
			if(pAssist->m_nStyle!=0)
				continue;
			CString ss="";
			ss.Format("%d.%s|",pAssist->iNo,pAssist->name);
			strSpreadFace+=ss;
		}
	}
	return strSpreadFace;
}

CDrawingAssistant *CStruView::GetDrawingAssistantByNo(int iNo)
{
	CDrawingAssistant *pAssist=NULL;
	if(m_pAttachView&&m_pAttachView->m_iViewType==1)
		pAssist=m_pAttachView->GetDrawingAssistByNo(iNo);
	return pAssist;
}

CDrawingAssistant *CStruView::GetDrawingAssistantByHandle(long hPart)
{
	CDrawingAssistant *pAssist=NULL;
	if(m_pAttachView&&m_pAttachView->m_iViewType==1)
		pAssist=m_pAttachView->GetDrawingAssistByHandle(hPart);
	return pAssist;
}

void CStruView::FromBuffer(CBuffer &buffer, long version/*=NULL*/)
{
	CDrawingComponent::FromBuffer(buffer,version);
	if(version<1000008)
	{	//չ�����б�
		CDrawingSpreadLine spreadLine;
		long i=0,nLineNum=0;
		buffer.ReadInteger(&nLineNum);
		for(i=0;i<nLineNum;i++)
			spreadLine.FromBuffer(buffer,version);
	}
}

void CStruView::ToBuffer(CBuffer &buffer)
{
	CDrawingComponent::ToBuffer(buffer);
}
CSectionViewSymbol *CStruView::GetSectionViewSymbol()
{
	if(ParentDrawing()==NULL)
		return NULL;
	long key=ParentDrawing()->listComponents.GetKey(this);
	CSectionViewSymbol *pViewSymbol=NULL;
	for(pViewSymbol=ParentDrawing()->sectionViewSymbols.GetFirst();pViewSymbol;pViewSymbol=ParentDrawing()->sectionViewSymbols.GetNext())
	{
		if(pViewSymbol->hViewKey==key)
			break;
	}
	return pViewSymbol;
}

//�����ͼ��Ӧ��ʵ�弯��
void CStruView::GetEntSet(ads_name &drag_ent_set,BOOL bSelectEnts/*=FALSE*/)
{
	//��ȡ��ͼ���ʵ�弯
	CDrawingComponent::GetEntSet(drag_ent_set,bSelectEnts);
	//��ȡ���ڸ���ͼ������ͼ��ʵ�弯
	CSectionViewSymbol *pViewSymbol=NULL;
	long key=ParentDrawing()->listComponents.GetKey(this);
	for(pViewSymbol=ParentDrawing()->sectionViewSymbols.GetFirst();pViewSymbol;pViewSymbol=ParentDrawing()->sectionViewSymbols.GetNext())
	{
		if(pViewSymbol->hAttachViewKey!=key)
			continue;
		pViewSymbol->GetEntSet(drag_ent_set,2,bSelectEnts);
	}	
}

//��ָ�������ƶ���Ŀ������֮��
BOOL CStruView::MoveSpecCmdAfterDestCmd(CDrawingCommand *pSpecCmd,CDrawingCommand *pDestCmd)
{
	if(pSpecCmd==NULL||pDestCmd==NULL||
		pSpecCmd->GetDrawingSerialNo()<=0||
		pDestCmd->GetDrawingSerialNo()<=0||
		pSpecCmd->GetDrawingSerialNo()==pDestCmd->GetDrawingSerialNo())
		return FALSE;
	return MoveSpecCmdSerialNoAfterDestCmdSerialNo(pSpecCmd->GetDrawingSerialNo(),pDestCmd->GetDrawingSerialNo());
}
//��ָ�������ƶ���Ŀ������֮ǰ
BOOL CStruView::MoveSpecCmdBeforeDestCmd(CDrawingCommand *pSpecCmd,CDrawingCommand *pDestCmd)
{
	if(pSpecCmd==NULL||pDestCmd==NULL||
		pSpecCmd->GetDrawingSerialNo()<=0||
		pDestCmd->GetDrawingSerialNo()<=0||
		pSpecCmd->GetDrawingSerialNo()==pDestCmd->GetDrawingSerialNo())
		return FALSE;
	return MoveSpecCmdSerialNoAfterDestCmdSerialNo(pSpecCmd->GetDrawingSerialNo(),pDestCmd->GetDrawingSerialNo()-1);
}

//��ָ����Ŷ�Ӧ�����������Ŀ����Ŷ�Ӧ������֮��
BOOL CStruView::MoveSpecCmdSerialNoAfterDestCmdSerialNo(int specSerialNo,int destSerialNo)
{
	CDrawingCommand *pCmd=NULL,*pSpecCmd=NULL;
	if(specSerialNo>destSerialNo)
	{	//ָ����Ŵ���Ŀ����ţ���>specSerialNo&&<destSerialNo�����+1����specSerialNo����ΪdestSerialNo+1
		for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
		{
			if(pCmd->m_eCmdType!=CMD_DRAW_PART)
				continue;
			int serialNo=pCmd->GetDrawingSerialNo();
			if(serialNo==specSerialNo)
				pSpecCmd=pCmd;
			else if(serialNo>specSerialNo&&serialNo<destSerialNo)
				pCmd->SetDrawingSerialNo(serialNo+1);
		}
		if(pSpecCmd)
			pSpecCmd->SetDrawingSerialNo(destSerialNo+1);
	}
	else if(specSerialNo<destSerialNo)
	{	//Ŀ����Ŵ���ָ�����,��>specSerialNo&&<=destSerialNo�����-1����specserialNo����ΪdestSerialNo
		for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
		{
			if(pCmd->m_eCmdType!=CMD_DRAW_PART)
				continue;
			int serialNo=pCmd->GetDrawingSerialNo();
			if(serialNo==specSerialNo)
				pSpecCmd=pCmd;
			else if(serialNo>specSerialNo&&serialNo<=destSerialNo)
				pCmd->SetDrawingSerialNo(serialNo-1);
		}
		if(pSpecCmd)
			pSpecCmd->SetDrawingSerialNo(destSerialNo);
	}
	if(pSpecCmd==NULL)
		return FALSE;
	else 
		return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//CPartDrawing
CPartDrawing::CPartDrawing(int id/*=0*/,IDrawing* pDrawing/*=NULL*/) : CDrawingComponent(id,pDrawing)
{
	m_hPart=0;
	m_pPart=NULL;
	m_bPolyPart=FALSE;
	m_fScale=1.0/sys.general_set.PartMapScale.fRealToDraw;
	m_eClassType=DRAW_COM_PARTVIEW;
}
CPartDrawing::~CPartDrawing()
{

}

//������ͼ
void CPartDrawing::Create()
{
	DRAGSET.ClearEntSet();
	CDrawingCommand *pFirstCmd=cmdList.GetFirst();
	if(pFirstCmd==NULL)
		return;
	if(pFirstCmd->m_eCmdType!=CMD_DRAW_PART)
		return;
	if(m_bFirstCreate)
	{	//��һ�����ɣ���Ҫ�Զ�������ͼװ��λ��
		CalInitAssemblyPosition();
		m_bFirstCreate=FALSE;
	}
	//ʹ�����λ�����¼�������ϵԭ��λ��
	//CalAssemblyPosByRelativePos();
	//������ͼ����Ӧһ������
	pFirstCmd->Run();
	//ָ����ͼװ��λ��
	SpecifyAssemblyPosition();
	//��ע��ͼ����
	InsertDrawingName();
	//����ͼԪ��ͼԪ����������ӵ���ͼ���ο�������
	UpdateDrawingRectArray();
	ads_command(RTSTR,"RE",RTNONE);	
}

//���ɻ������ͼ����
BOOL CPartDrawing::CreateCmdList(BOOL bInit/*=TRUE*/)
{
	if(!m_bPolyPart)
	{
		if(m_pPart==NULL)
			m_pPart=Ta.FromPartHandle(m_hPart);
		if(m_pPart==NULL)
			return FALSE;
	}
	cmdList.Empty();
	CDrawingCommand *pCommand=cmdList.append();
	pCommand->m_pDrawingCom=this;
	pCommand->m_eCmdType=CMD_DRAW_PART;
	pCommand->InitCommandPara();
	pCommand->m_paraPart.hPart=m_hPart;
	if(m_bPolyPart)
		pCommand->m_paraPart.pPart=&polyPart;
	else
		pCommand->m_paraPart.pPart=m_pPart;
	//�������ͼ��ʼλ��
	return TRUE;
}

//��������
void CPartDrawing::CopyProperty(CDrawingComponent* pCom)
{
	if(pCom==NULL)
		return;
	if(pCom->m_eClassType!=m_eClassType)
		return;
	CPartDrawing *pPartDrawing=(CPartDrawing*)pCom;
	m_hPart=pPartDrawing->m_hPart;
	m_bPolyPart=pPartDrawing->m_bPolyPart;
	polyPart.CopyProperty(&pPartDrawing->polyPart);
	CDrawingComponent::CopyProperty(pCom);
}
//
void CPartDrawing::FromBuffer(CBuffer &buffer, long version/*=NULL*/)
{
	CDrawingComponent::FromBuffer(buffer,version);
	buffer.ReadInteger(&m_hPart);
	buffer.ReadInteger(&m_bPolyPart);
	if(m_bPolyPart)
	{
		long hPart=0,i=0,nPartNum=0;
		buffer.ReadInteger(&nPartNum);
		sonPoleHandleList.Empty();
		polyPart.segset.Empty();
		for(i=0;i<nPartNum;i++)
		{
			buffer.ReadInteger(&hPart);
			sonPoleHandleList.append(hPart);
			CLDSLinePart *pLinePart=(CLDSLinePart*)Ta.FromPartHandle(hPart,CLS_LINEPART);
			if(pLinePart)
				polyPart.segset.append(pLinePart);
		}
	}
}
	
void CPartDrawing::ToBuffer(CBuffer &buffer)
{
	CDrawingComponent::ToBuffer(buffer);
	buffer.WriteInteger(m_hPart);
	buffer.WriteInteger(m_bPolyPart);
	if(m_bPolyPart)
	{
		long *pH=NULL;
		buffer.WriteInteger(sonPoleHandleList.GetNodeNum());
		for(pH=sonPoleHandleList.GetFirst();pH;pH=sonPoleHandleList.GetNext())
			buffer.WriteInteger(*pH);
	}
}

//////////////////////////////////////////////////////////////////////////
//CStdView
CStdView::CStdView(int id/*=0*/,IDrawing* pDrawing/*=NULL*/) : CDrawingComponent(id,pDrawing)
{
	m_eClassType=DRAW_COM_STDVIEW;	//��ͼ����
	m_fScale=1;
}

CStdView::~CStdView()
{
	
}

//������ͼ
void CStdView::Create()
{
	
}
//������������
BOOL CStdView::CreateCmdList(BOOL bInit/*=TRUE*/)
{
	cmdList.Empty();
	return TRUE;
}

void CStdView::CopyProperty(CDrawingComponent* pCom)
{
	CDrawingComponent::CopyProperty(pCom);
	if(pCom->m_eClassType==m_eClassType)
		m_eStdViewType=((CStdView*)pCom)->m_eStdViewType;
}

void CStdView::FromBuffer(CBuffer &buffer, long version/*=NULL*/)
{
	CDrawingComponent::FromBuffer(buffer,version);
	int nType=0;
	buffer.ReadInteger(&nType);
	m_eStdViewType=(STD_VIEW_TYPE)nType;
}

void CStdView::ToBuffer(CBuffer &buffer)
{
	CDrawingComponent::ToBuffer(buffer);
	buffer.WriteInteger((int)m_eStdViewType);
}
//////////////////////////////////////////////////////////////////////////
//����ͼ����
CSectionViewSymbol::CSectionViewSymbol()
{
	bUpturned=FALSE;
	viewName.Empty();
	startEntIds.Empty();
	endEntIds.Empty();
	hAttachViewKey=hViewKey=0;
	fLenght=sys.dim_map.fCutawayViewSymbolLen;
	fPolyLineWidth=sys.dim_map.fCutawayViewPolyLineWidth;
	fTextSize=sys.dim_map.fCutawayViewSymbolTextSize;
}
CSectionViewSymbol::CSectionViewSymbol(f3dPoint start,f3dPoint end,CXhChar16 name)
{
	startPt=start;
	endPt=end;
	bUpturned=FALSE;	//Ĭ�ϳ���
	viewName.Copy(name);
	startEntIds.Empty();
	endEntIds.Empty();
	hAttachViewKey=hViewKey=0;
	fLenght=sys.dim_map.fCutawayViewSymbolLen;
	fPolyLineWidth=sys.dim_map.fCutawayViewPolyLineWidth;
	fTextSize=sys.dim_map.fCutawayViewSymbolTextSize;
};	

//�޸�����ͼ����
void CSectionViewSymbol::ModifySymbolName(char *symbol_name)
{
	if(symbol_name==NULL)
		return;
	strcpy(viewName,symbol_name);
	//
	AcDbEntity *pEnt=NULL;
	AcDbObjectId *pEntId=NULL;
	for(pEntId=startEntIds.GetFirst();pEntId;pEntId=startEntIds.GetNext())
	{	
		if(acdbOpenObject(pEnt,*pEntId,AcDb::kForWrite)!=Acad::eOk)
			continue;
		if(pEnt==NULL)
			continue;
		if(pEnt->isKindOf(AcDbText::desc()))
		{
			AcDbText *pText=(AcDbText*)pEnt;
		#ifdef _ARX_2007
			pText->setTextString(_bstr_t(symbol_name));
		#else
			pText->setTextString(symbol_name);
		#endif
		}
		pEnt->close();
	}
	for(pEntId=endEntIds.GetFirst();pEntId;pEntId=endEntIds.GetNext())
	{	
		if(acdbOpenObject(pEnt,*pEntId,AcDb::kForWrite)!=Acad::eOk)
			continue;
		if(pEnt==NULL)
			continue;
		if(pEnt->isKindOf(AcDbText::desc()))
		{
			AcDbText *pText=(AcDbText*)pEnt;
		#ifdef _ARX_2007
			pText->setTextString(_bstr_t(symbol_name));
		#else
			pText->setTextString(symbol_name);
		#endif
		}
		pEnt->close();
	}
	//���½���
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
}

//ɾ����ص�CADʵ��
void CSectionViewSymbol::EraseRelativeAcadEntity()
{
	AcDbEntity *pEnt=NULL;
	AcDbObjectId *pEntId=NULL;
	for(pEntId=startEntIds.GetFirst();pEntId;pEntId=startEntIds.GetNext())
	{	
		if(acdbOpenObject(pEnt,*pEntId,AcDb::kForWrite)!=Acad::eOk)
			continue;
		if(pEnt==NULL)
			continue;
		pEnt->erase();
		pEnt->close();
	}
	for(pEntId=endEntIds.GetFirst();pEntId;pEntId=endEntIds.GetNext())
	{	
		if(acdbOpenObject(pEnt,*pEntId,AcDb::kForWrite)!=Acad::eOk)
			continue;
		if(pEnt==NULL)
			continue;
		pEnt->erase();
		pEnt->close();
	}
	startEntIds.Empty();
	endEntIds.Empty();
	//���½���
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
}
//��������ͼ��
void CSectionViewSymbol::Draw()
{
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	if(pBlockTableRecord==NULL)
		return;
	startEntIds.Empty();
	endEntIds.Empty();
	f3dPoint start_pt=startPt;
	f3dPoint end_pt=endPt;
	//��������ͼ����
	f3dPoint line_vec,line_norm;
	line_vec=end_pt-start_pt;
	normalize(line_vec);	
	line_norm.Set(-line_vec.y,line_vec.x);
	if(!bUpturned)	//����
		line_norm*=-1.0;
	GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
	double symbol_len=fLenght;	//����ͼ���Ŵ�С
	double text_rot_angle=Cal2dLineAng(f2dLine(f2dPoint(0,0),f2dPoint(line_norm.x,line_norm.y)))-0.5*Pi;
	if(text_rot_angle>=0.5*Pi)	//��֤���峯��
		text_rot_angle-=Pi;
	AcDbObjectId entId=0;
	//��������ͼ��
	entId=CreateAcadPolyLine(pBlockTableRecord,start_pt,start_pt+symbol_len*line_vec,fPolyLineWidth);
	startEntIds.append(entId);	//ʼ��
	entId=CreateAcadLine(pBlockTableRecord,start_pt,start_pt+symbol_len*line_norm);
	startEntIds.append(entId);	//ʼ��
	entId=CreateAcadPolyLine(pBlockTableRecord,end_pt,end_pt-symbol_len*line_vec,fPolyLineWidth);
	endEntIds.append(entId);	//�ն�
	entId=CreateAcadLine(pBlockTableRecord,end_pt,end_pt+symbol_len*line_norm);
	endEntIds.append(entId);	//�ն�
	//��ע����ͼ����
	f3dPoint dimPos=start_pt+line_vec*0.5*symbol_len+line_norm*0.5*symbol_len;
	entId=DimText(pBlockTableRecord,dimPos,viewName,TextStyleTable::hzfs.textStyleId,
		fTextSize,text_rot_angle,AcDb::kTextCenter,AcDb::kTextVertMid);
	startEntIds.append(entId);	//ʼ�˱�ע����
	dimPos=end_pt-line_vec*0.5*symbol_len+line_norm*0.5*symbol_len;
	entId=DimText(pBlockTableRecord,dimPos,viewName,TextStyleTable::hzfs.textStyleId,
		fTextSize,text_rot_angle,AcDb::kTextCenter,AcDb::kTextVertMid);
	endEntIds.append(entId);	//�ն˱�ע����
	//
	pBlockTableRecord->close();
	//���½���
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
}

//��������ͼ��λ��
void CSectionViewSymbol::UpdatePosition(double fOffsetX,double fOffsetY)
{
	startPt.x+=fOffsetX;
	startPt.y+=fOffsetY;
	endPt.x+=fOffsetX;
	endPt.y+=fOffsetY;
}

//�õ���ͼ����
f2dRect CSectionViewSymbol::GetDrawRegion()
{
	f2dRect rect;
	SCOPE_STRU scope;
	scope.ClearScope();
	scope.fMinX = scope.fMinY = scope.fMinZ = 100000.0;
	scope.fMaxX = scope.fMaxY = scope.fMaxZ =-100000.0;
	AcDbObjectId *pEntId=NULL;
	for(pEntId=startEntIds.GetFirst();pEntId;pEntId=startEntIds.GetNext())
		VerifyVertexByCADEntId(scope,*pEntId);
	for(pEntId=endEntIds.GetFirst();pEntId;pEntId=endEntIds.GetNext())
		VerifyVertexByCADEntId(scope,*pEntId);
	rect.topLeft.Set(scope.fMinX,scope.fMaxY);
	rect.bottomRight.Set(scope.fMaxX,scope.fMinY);
	double width=scope.fMaxX-scope.fMinX;
	if(fabs(fabs(width)-200000)<EPS)	//������
		rect.topLeft=rect.bottomRight=f3dPoint(0,0);
	return rect;	
}

SCOPE_STRU CSectionViewSymbol::GetEntScope(int start0_end1_all2)
{
	SCOPE_STRU scope;
	if(start0_end1_all2==0||start0_end1_all2==2)
	{
		scope.VerifyVertex(f3dPoint(startPt.x-fLenght,startPt.y-fLenght));
		scope.VerifyVertex(f3dPoint(startPt.x+fLenght,startPt.y-fLenght));
		scope.VerifyVertex(f3dPoint(startPt.x-fLenght,startPt.y+fLenght));
		scope.VerifyVertex(f3dPoint(startPt.x+fLenght,startPt.y+fLenght));
	}
	if(start0_end1_all2==1||start0_end1_all2==2)
	{
		scope.VerifyVertex(f3dPoint(endPt.x-fLenght,endPt.y-fLenght));
		scope.VerifyVertex(f3dPoint(endPt.x+fLenght,endPt.y-fLenght));
		scope.VerifyVertex(f3dPoint(endPt.x-fLenght,endPt.y+fLenght));
		scope.VerifyVertex(f3dPoint(endPt.x+fLenght,endPt.y+fLenght));
	}
	return scope;
}

//�������ͼ����Ӧ��ʵ�弯��
void CSectionViewSymbol::GetEntSet(ads_name &drag_ent_set,
								   int start0_end1_all2/*=2*/,
								   BOOL bSelectEnts/*=FALSE*/)
{
	AcDbObjectId *pEntId=NULL;
	if(start0_end1_all2==0||start0_end1_all2==2)
	{
		for(pEntId=startEntIds.GetFirst();pEntId;pEntId=startEntIds.GetNext())
		{
			ads_name ent_name;
			if(acdbGetAdsName(ent_name,*pEntId)!=Acad::eOk)
				continue;
			if(bSelectEnts)
				ads_redraw(ent_name,3);//������ʾ
			if(acedSSAdd(ent_name,drag_ent_set,drag_ent_set)!=RTNORM)
				continue;
			struct resbuf *pData=acdbEntGet(ent_name);
			acdbEntMod(pData);
			acdbEntUpd(ent_name);
		}
	}
	if(start0_end1_all2==1||start0_end1_all2==2)
	{
		for(pEntId=endEntIds.GetFirst();pEntId;pEntId=endEntIds.GetNext())
		{
			ads_name ent_name;
			if(acdbGetAdsName(ent_name,*pEntId)!=Acad::eOk)
				continue;
			if(bSelectEnts)
				ads_redraw(ent_name,3);//������ʾ
			if(acedSSAdd(ent_name,drag_ent_set,drag_ent_set)!=RTNORM)
				continue;
			struct resbuf *pData=acdbEntGet(ent_name);
			acdbEntMod(pData);
			acdbEntUpd(ent_name);
		}
	}
}	

void CSectionViewSymbol::CopyProperty(CSectionViewSymbol *pSymbol)
{
	if(pSymbol==NULL)
		return;
	startPt=pSymbol->startPt;
	endPt=pSymbol->endPt;
	bUpturned=pSymbol->bUpturned;
	viewName.Copy(pSymbol->viewName);
	hAttachViewKey=pSymbol->hAttachViewKey;
	hViewKey=pSymbol->hViewKey;
}

void CSectionViewSymbol::FromBuffer(CBuffer &buffer, long version/*=NULL*/)
{
	buffer.ReadPoint(startPt);
	buffer.ReadPoint(endPt);
	buffer.ReadInteger(&bUpturned);
	buffer.ReadString(viewName,16);
	buffer.ReadInteger(&hAttachViewKey);
	buffer.ReadInteger(&hViewKey);
}

void CSectionViewSymbol::ToBuffer(CBuffer &buffer)
{
	buffer.WritePoint(startPt);
	buffer.WritePoint(endPt);
	buffer.WriteInteger(bUpturned);
	buffer.WriteString(viewName);
	buffer.WriteInteger(hAttachViewKey);
	buffer.WriteInteger(hViewKey);
}

//�������ͼ��
CSectionViewSymbol* CSectionViewSymbol::AddSectionViewSymbol(CStruView *pMainView,CStruView *pStruView)
{
	if(pStruView==NULL||pStruView->ParentDrawing()==NULL)	//||pMainView==NULL
		return FALSE;
	//��������ͼ��ʼ���Լ������
	ads_point start_pt,end_pt;
	f3dPoint startPt,endPt;
	CXhChar100 sPrompt("\n��ָ��[%s]����ͼ���ŵ���ʼ��,<Enter�˳�>:",&pStruView->m_sName);
	while(1)
	{
#ifdef _ARX_2007
		if(acedGetPoint(NULL,_bstr_t(sPrompt),start_pt)==RTNORM)
#else
		if(acedGetPoint(NULL,sPrompt,start_pt)==RTNORM)
#endif
		{
			startPt.Set(start_pt[X],start_pt[Y],start_pt[Z]);
			break;		
		}
		else
			return FALSE;
	}
	sPrompt.Printf("\n��ָ��[%s]����ͼ���ŵ���ֹ��,<Enter�˳�>:",(char*)pStruView->m_sName);
	while(1)
	{
#ifdef _ARX_2007
		if(acedGetPoint(start_pt,_bstr_t(sPrompt),end_pt)==RTNORM)
#else
		if(acedGetPoint(start_pt,sPrompt,end_pt)==RTNORM)
#endif
		{
			endPt.Set(end_pt[X],end_pt[Y],end_pt[Z]);
			break;		
		}
		else
			return FALSE;
	}
	if(pStruView->m_sSectionViewCode.Length()<=0)
	{
		while(1)
		{
#ifdef _ARX_2007
			if(acedGetString(0,L"\n������һ���ַ�,<Enter�˳�>:\n",_bstr_t(pStruView->m_sSectionViewCode))==RTNORM)
#else
			if(acedGetString(0,"\n������һ���ַ�,<Enter�˳�>:\n",pStruView->m_sSectionViewCode)==RTNORM)
#endif
				break;
		}
	}
	//������ͼ�������浽����ͼ��
	CSectionViewSymbol view_symbol(startPt,endPt,pStruView->m_sSectionViewCode);
	CSectionViewSymbol *pViewSymbol=pStruView->ParentDrawing()->sectionViewSymbols.append(view_symbol);
	if(pMainView)	//��������ͼ������ͼkeyֵ,������ͼʱ��Ҫ��ע��ɺ��ֶ�����
		pViewSymbol->hAttachViewKey=pStruView->ParentDrawing()->listComponents.GetKey(pMainView);	
	pViewSymbol->hViewKey=pStruView->ParentDrawing()->listComponents.GetKey(pStruView);
	pViewSymbol->Draw();	//��������ͼ����
	return pViewSymbol;
}

//////////////////////////////////////////////////////////////////////////
//��ʼ��ͼֽ�б�
void InitDrawingList()
{
	if(drawingList.GetNodeNum()<=0)
	{
		CDrawing *pDrawing=NULL;
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)||defined(__TSA_FILE_)
		//˾��ͼ
		PutMessage(NULL,"���ڳ�ʼ��˾��ͼ......");
		pDrawing=drawingList.Add(0);
		pDrawing->m_bAutoLayoutComponent=TRUE;	//�Զ�����
		pDrawing->m_hActiveModule=Ta.m_hActiveModule;
		pDrawing->InitDrawing(SEGI(-1));	//˾��ͼ
		strcpy(pDrawing->m_sDrawingName,"˾��ͼ");
#endif
		//��ͼ
		PutMessage(NULL,"���ڳ�ʼ����ͼ.........");
		pDrawing=drawingList.Add(0);
		pDrawing->m_bAutoLayoutComponent=TRUE;	//�Զ�����
		pDrawing->m_hActiveModule=Ta.m_hActiveModule;
		pDrawing->InitDrawing(SEGI(-2));	//��ͼ
		strcpy(pDrawing->m_sDrawingName,"��  ͼ");
		//�ֶνṹͼ
		ATOM_LIST<SEGI> segNoList;
		int nSegNum=Ta.GetSegNoList(segNoList);
		for(SEGI *pSegI=segNoList.GetFirst();pSegI;pSegI=segNoList.GetNext())
		{	
			pDrawing=drawingList.Add(0);
			PutMessage(NULL,"���ڳ�ʼ��%s�νṹͼ...",(char*)pSegI->ToString());
			pDrawing->InitDrawing(*pSegI);
			pDrawing->m_sDrawingName.Printf("%s��",(char*)pSegI->ToString());
		}
		PutMessage(NULL,NULL);
	}
}

//���޸Ĺ����κ�,��������ٷֶ�ʱӦ���·ֶνṹͼ�����б�
void UpdateStruDrawingList()
{
	CDrawing *pDrawing=NULL;
	CHashList<SEGI> segHashList;
	Ta.GetSegNoHashList(segHashList);
	for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
	{
		if(pDrawing->m_nSegI.iSeg<0)
			continue;
		SEGI *pSeg=segHashList.GetValue(pDrawing->m_nSegI);
		if(pSeg)	//���ڶ�Ӧ�Σ�����������б�
		{
			pDrawing->UpdateCmdList();
			//�����Ѵ��ڶ�Ӧͼֽ�Ķ�hashֵΪ���Ķκ�
			if(*pSeg>0)
				segHashList.SetValue(pDrawing->m_nSegI,(*pSeg)*-1); 
			else 
				segHashList.SetValue(pDrawing->m_nSegI,-1); 
		}
		else		//�Ѳ����ڶ�Ӧ�Σ���ɾ����Ӧ�ķֶνṹͼ
			drawingList.DeleteCursor();
	}
	SEGI *pSeg=NULL;
	for(pSeg=segHashList.GetFirst();pSeg;pSeg=segHashList.GetNext())
	{
		if(*pSeg<0)
			continue;
		//�����ڶ�Ӧ�ֶνṹͼ�������κ�,Ӧ�ڴ˴���Ӷ�Ӧ�ķֶνṹͼ
		pDrawing=drawingList.Add(0);
		pDrawing->InitDrawing(*pSeg);
		pDrawing->m_sDrawingName.Printf("%s��",(char*)pSeg->ToString());
	}
}
#endif
