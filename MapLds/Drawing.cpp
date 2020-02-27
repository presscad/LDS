//<LOCALE_TRANSLATE BY hxr /> 2015-04-27
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
#include "ArrayList.h"
#include "StackAcadVar.h"
#ifdef __SMART_DRAW_INC_
#include "../IntelliAgent/IntelliCore/IPreview.h"
#ifndef __PUBLIC_RELEASE_COMPILE_
#include "../IntelliAgent/IntelliAgent.h"
#include "../IntelliAgent/IntelliCore/IntelliCore.h"
#else
#include "../IntelliAgent/IntelliAgent.h"
#include "../IntelliAgent/IntelliCore/IntelliCoreObject.h"
#endif
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef __SMART_DRAW_INC_
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BYTE ANGLEWING_OFFSET::get_ciStyleG()
{
	BYTE _ciStyleG=(BYTE)(wiData>>13);
	return _ciStyleG&0x07;
}
BYTE ANGLEWING_OFFSET::set_ciStyleG(BYTE _ciStyleG)
{
	WORD wiStyleG=_ciStyleG&0x07;
	wiStyleG<<=13;
	wiData&=0x1fff;
	wiData|=wiStyleG;
	return _ciStyleG&0x07;
}
BYTE ANGLEWING_OFFSET::get_ciSectStyle()
{
	BYTE _ciSectStyle=(BYTE)(wiData>>10);
	return _ciSectStyle&0x07;
}
BYTE ANGLEWING_OFFSET::set_ciSectStyle(BYTE _ciSectStyle)
{
	WORD wiSectStyle=_ciSectStyle&0x07;
	wiSectStyle<<=10;
	wiData&=0xe3ff;
	wiData|=wiSectStyle;
	return _ciSectStyle&0x07;
}
short ANGLEWING_OFFSET::get_siOffsetDist()
{
	short siDistG=wiData&0x01ff;
	if(wiData&0x0200)
		return -siDistG;
	else
		return siDistG;
}
short ANGLEWING_OFFSET::set_siOffsetDist(short gDist)
{
	short siDistG=gDist;
	WORD wiDistG=abs(gDist);
	siDistG=wiDistG=wiDistG&0x01ff;
	wiData=wiDistG;
	if(gDist<0)	//负心距
		wiData|=0x0200;
	return gDist>0?siDistG:-siDistG;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
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
//图纸
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
	m_nAdjoinSegI.iSeg=0;
	//设置HashList回调函数
	listComponents.CreateNewAtom=NewDrawingComponent;
	listComponents.DeleteAtom=NativeDeleteDrawingComponent;
	LoadDefaultUCS(&dcs);
	feature=0;
}

CDrawing::~CDrawing()
{

}

// 根据指定的视图在图纸中生成对应的视图
// pView 指定视图
// pModule 生成司令图或总图时该变量有效,用来指定呼高分组
// bFlag   结构图:表示生成单线图还是结构图 司令图\总图:表示正面视图侧面视图
CDrawingComponent *CDrawing::AddComponentByView(CDisplayView *pView,CLDSModule *pModule,BOOL bFlag/*=FALSE*/)
{
	if(this->liDrwType==DARWING_MECHANICS||this->liDrwType==DARWING_SUMMARY)
	{	//司令图、总图
		CLineView *pLineView=NULL;
		if(pView->m_iViewType==0)
		{	//图层视图
			if(pModule==NULL)
				return NULL;
			if(pModule->handle==m_hActiveModule)	//主体+激活呼高分组
				pLineView=(CLineView*)listComponents.Add(pView->handle-abs(m_nSegI.iSeg),DRAW_COM_LINEVIEW);
			else	//非激活呼高
				pLineView=(CLineView*)listComponents.Add(pView->handle+pModule->handle,DRAW_COM_LINEVIEW);
			//pLineView->m_pDrawing=this;
			pLineView->m_bNeedAdjust=FALSE;//(m_nSegI==-1);	//总图不需要调整
			pLineView->m_bAnalysisMode=(m_nSegI.iSeg==-1);	//力学分析模式即司令图
			pLineView->SetDCS(pView->ucs);
			pLineView->m_eClassType=DRAW_COM_LINEVIEW;
			pLineView->m_hAttachView=pView->handle;
			pLineView->m_pAttachView=pView;
			pLineView->m_sDrawingLayer.Copy(pView->filter.ThirdCharSet);	//图层范围
			pLineView->m_hModule=pModule->handle;
			pLineView->m_pModule=pModule;
			if(pModule->handle==m_hActiveModule)	//主体+激活呼高分组
			{
				if(!bFlag)	//主体图
#ifdef AFX_TARG_ENU_ENGLISH
					pLineView->m_sName.Printf("%s Main View",pModule->description);
				else		//侧面图
					pLineView->m_sName.Printf("%s Flank View",pModule->description);
			}
			else	//非激活呼高分组
				pLineView->m_sName.Printf("%s Module View",pModule->description);
#else
					pLineView->m_sName.Printf("%s主体图",pModule->description);
				else		//侧面图
					pLineView->m_sName.Printf("%s侧面图",pModule->description);
			}
			else	//非激活呼高分组
				pLineView->m_sName.Printf("%s呼高图",pModule->description);
#endif
			if(this->liDrwType==DARWING_MECHANICS)
				pLineView->m_fScale=1.0/sys.general_set.ForceLineScale;
			pLineView->CreateCmdList();
			return pLineView;
		}
		else if(pView->m_iViewType==1)
		{	//定制视图
			pLineView=(CLineView*)listComponents.Add(pView->handle,DRAW_COM_LINEVIEW);			
			//pLineView->m_pDrawing=this;
			pLineView->m_bNeedAdjust=FALSE;//(m_nSegI==-1);	//总图不需要调整
			pLineView->m_bAnalysisMode=(m_nSegI.iSeg==-1);	//力学分析模式即司令图
			pLineView->SetDCS(pView->ucs);
			pLineView->m_eClassType=DRAW_COM_LINEVIEW;
			pLineView->m_hAttachView=pView->handle;
			pLineView->m_pAttachView=pView;
			pLineView->m_sName.Printf("%s",pView->name);
			pLineView->CreateCmdList();
			if(this->liDrwType==DARWING_MECHANICS)
				pLineView->m_fScale=1.0/sys.general_set.ForceLineScale;
			return pLineView;
		}
		else 
			return NULL;
	}
	else 
	{	//结构图
		if(pView->m_iViewType==1&&!(pView->GetSegI().iSeg==m_nSegI.iSeg||(SEGI(pView->folderName)).iSeg==m_nSegI.iSeg))
			return NULL;
		if(bFlag)
		{	//指定视图为单线图
			CLineView *pLineView=(CLineView*)listComponents.Add((pView->handle+m_nSegI.iSeg)%16+16,DRAW_COM_LINEVIEW);
			//pLineView->m_pDrawing=this;
			pLineView->m_bNeedAdjust=FALSE;	//不需要进行调整
			pLineView->m_bAnalysisMode=(m_nSegI.iSeg==-1);//力学分析模式即司令图
			pLineView->SetDCS(pView->ucs);
			pLineView->m_eClassType=DRAW_COM_LINEVIEW;
			pLineView->m_hAttachView=pView->handle;
			pLineView->m_pAttachView=pView;
			pLineView->m_sDrawingLayer.Copy(pView->filter.ThirdCharSet);	//图层范围
#ifdef AFX_TARG_ENU_ENGLISH
			pLineView->m_sName.Printf("Line View 1:100");
#else
			pLineView->m_sName.Printf("单线图 1:100");
#endif
			pLineView->m_fScale=0.01;	//绘图比例 1:100
			pLineView->CreateCmdList();
			return pLineView;
		}
		else
		{
			CStruView *pStruView=NULL;
			if(pView->m_iViewType==0)		//图层视图
			{
				pStruView=(CStruView*)listComponents.Add(pView->handle+m_nSegI.iSeg,DRAW_COM_STRUVIEW);	
				pStruView->m_sDrawingLayer.Copy(pView->filter.ThirdCharSet);	//图层范围
			}
			else if(pView->m_iViewType==1)	//定制视图
				pStruView=(CStruView*)listComponents.Add(pView->handle,DRAW_COM_STRUVIEW);			
			else 
				return NULL;
			//视图基本属性
			//pStruView->m_pDrawing=this;
			pStruView->SetDCS(pView->ucs);
			pStruView->m_bNeedAdjust=FALSE;	//不需要进行调整
			pStruView->m_eClassType=DRAW_COM_STRUVIEW;
			pStruView->m_hAttachView=pView->handle;
			pStruView->m_pAttachView=pView;
			pStruView->m_sName.Printf("%s",pView->name);
			size_t name_len=strlen(pView->name);
			char cSectionViewCode=pView->name[name_len-1];
			if(isalpha(cSectionViewCode)||isdigit(cSectionViewCode))	//数字或字母
				pStruView->m_sSectionViewCode.Printf("%c",cSectionViewCode);//默认取剖视图名称的最后一个字符
			else 
				strcpy(pStruView->m_sSectionViewCode,"");
			pStruView->CreateCmdList();
			return pStruView;
		}
	}
	return NULL;
}

void CDrawing::InitPartDrawing(CStruView *pStruView,CHashStrList<CLDSPart*>& partHashList)
{
	if(pStruView==NULL)
		return;
	CPartDrawing *pPartDrawing=NULL;
	CHashSet<CLDSPart*> hashWrapAngles;
	for(CLDSPart* pPart=pStruView->GetFirstPart();pPart;pPart=pStruView->GetNextPart())
	{
		int nStyle=-1;
		if(partHashList.GetValue(pPart->GetPartNo()))
		//if(hashTable.GetValueAt(pPart->GetPartNo(),pValue))
			continue;
		CSuperSmartPtr<CLDSPart> pSmartPart=pPart;
		//0.接头
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{	//1.查找短角钢
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPart;
			if(pLineAngle->bHuoQuStart||pLineAngle->bHuoQuEnd
				||pLineAngle->pStart||pLineAngle->pEnd)
				continue;
			hashWrapAngles.SetValue(pLineAngle->handle,pLineAngle);
			nStyle=1;
		}
		//else if(pPart->GetClassTypeId()==CLS_PLATE)
		else if(pSmartPart->IsPlate()&&pSmartPart.pBasicPlate->wiType==CLDSGeneralPlate::WASHER_PLATE)
			nStyle=2;	//2.螺栓垫板
		else if(pSmartPart->IsPlate()&&pSmartPart.pBasicPlate->wiType==CLDSGeneralPlate::WRAPPER_PLATE)	//3.接头包钢板
			nStyle=3;
		//3.环向封板、肋板以及跨越连板
		else 
			continue;
		partHashList.SetValue(pPart->GetPartNo(),pPart);
		if(nStyle==1)
		{	//添加角钢接头俯视图
			long key=pPart->handle+5000;	//
			CStdView *pJgJointView=(CStdView*)listComponents.Add(key,DRAW_COM_STDVIEW);
			pJgJointView->m_eStdViewType=STD_VIEW_JOINT;	//接头视图
			pJgJointView->m_fScale=0.1;			//短角钢及接头包钢板绘图比例默认为1::10
			pJgJointView->m_pPart=pPart;
			pJgJointView->m_sName.Printf("接头视图%s#",(char*)pPart->GetPartNo());
			pJgJointView->m_bNeedAdjust=FALSE;
			pJgJointView->CreateCmdList();
		}
		//添加构件视图
		pPartDrawing=(CPartDrawing*)listComponents.Add(pPart->handle,DRAW_COM_PARTVIEW);
		if(nStyle==1||nStyle==3)
			pPartDrawing->m_fScale=0.1;	//短角钢及接头包钢板绘图比例默认为1::10
		else if(nStyle==2)
			pPartDrawing->m_fScale=0.2;	//螺栓垫板默认绘图比例为1:5
		//pPartDrawing->m_pDrawing=this;
		pPartDrawing->m_hPart=pPart->handle;
		pPartDrawing->sPartNo.Copy(pPart->GetPartNo());
		pPartDrawing->m_pPart=pPart;
		char sGuige[100]="";
		pPart->GetSpecification(sGuige,TRUE);
		pPartDrawing->m_sName.Printf("%s %s",(char*)pPart->GetPartNo(),sGuige);
		pPartDrawing->m_bNeedAdjust=FALSE;
		pPartDrawing->CreateCmdList();
	}
	for(CDrawingComponent* pSonDrawing=(CDrawingComponent*)listComponents.GetFirst();pSonDrawing;pSonDrawing=(CDrawingComponent*)listComponents.GetNext())
	{	//将接头中的垫板比例设置为与接头其余构件一致的1:10
		if(pSonDrawing->m_eClassType!=DRAW_COM_PARTVIEW)
			continue;
		pPartDrawing=(CPartDrawing*)pSonDrawing;
		if(!pPartDrawing->m_pPart->IsPlate()||((CLDSGeneralPlate*)pPartDrawing->m_pPart)->wiType!=CLDSGeneralPlate::WASHER_PLATE)
			continue;	//非垫板零件图
		CLsRef* pFirstLsRef=((CLDSGeneralPlate*)pPartDrawing->m_pPart)->GetFirstLsRef();
		CLDSBolt* pBolt=(pFirstLsRef==NULL)?NULL:pFirstLsRef->GetLsPtr();
		if(pBolt==NULL)
			continue;
		for(THICK_ITEM item=pBolt->GetFirstL0ThickItem();!item.IsFinalKey();item=pBolt->GetNextL0ThickItem())
		{
			if(item.IsHandle()&&hashWrapAngles.GetValue(abs(item.val))!=NULL)
			{	//接头中的垫板比例设置为与接头其余构件一致的1:10
				pPartDrawing->m_fScale=0.1;
				break;
			}
		}
	}
	//火曲角钢
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
		pPartDrawing->m_sName.Printf("%s火曲角钢",pPolyPart->sPartNo);
		pPartDrawing->m_bNeedAdjust=FALSE;
		pPartDrawing->CreateCmdList();
	}
	*/
}

void CDrawing::InitDrawing(long liDrwTypeOrSegI)
{	//初始化视图列表
	sectionViewSymbols.Empty();
	listComponents.Empty();
	m_nSegI=liDrwTypeOrSegI;
	CDrawingBOM *pDrawingBOM=NULL;
	CDrawingComponent *pComponent=NULL;
	CDisplayView *pView=NULL,*pFrontView=NULL,*pSideView=NULL;
	if(liDrwTypeOrSegI==DARWING_MECHANICS||liDrwTypeOrSegI==DARWING_SUMMARY)
	{	//司令图、总图
		if(liDrwTypeOrSegI==DARWING_MECHANICS)
		{	//标注构件规格时需要生成简化规格表，该表格应在最开始生成
			pDrawingBOM=(CDrawingBOM*)listComponents.Add(BOM_GUIGE,DRAW_COM_BOM);
			//pDrawingBOM->m_pDrawing=this;
			pDrawingBOM->m_bNeedAdjust=FALSE;	//表格不需要进行调整
			pDrawingBOM->m_eClassType=DRAW_COM_BOM;
			pDrawingBOM->m_eBOMType=BOM_GUIGE;
#ifdef AFX_TARG_ENU_ENGLISH
			pDrawingBOM->m_sName.Copy("Simplify Spec BOM");
#else
			pDrawingBOM->m_sName.Copy("简化规格表");
#endif
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
			if(liDrwTypeOrSegI==DARWING_SUMMARY)//总图
			{	//材料汇总表
				pDrawingBOM=(CDrawingBOM*)listComponents.Add(BOM_SUMPART,DRAW_COM_BOM);
				//pDrawingBOM->m_pDrawing=this;
				pDrawingBOM->m_bSpecifyComPos=FALSE;//材料汇总表默认放置在右上角不需要拖放
				pDrawingBOM->m_bNeedAdjust=FALSE;	//表格不需要进行调整
				pDrawingBOM->m_eClassType=DRAW_COM_BOM;
				pDrawingBOM->m_eBOMType=BOM_SUMPART;
#ifdef AFX_TARG_ENU_ENGLISH
				pDrawingBOM->m_sName.Copy("Sum Part BOM");
#else
				pDrawingBOM->m_sName.Copy("材料汇总表");
#endif
				pDrawingBOM->CreateCmdList();
				//铁塔根开表
				pDrawingBOM=(CDrawingBOM*)listComponents.Add(BOM_GENKAI,DRAW_COM_BOM);
				//pDrawingBOM->m_pDrawing=this;
				pDrawingBOM->m_bSpecifyComPos=TRUE;
				pDrawingBOM->m_bNeedAdjust=FALSE;
				pDrawingBOM->m_eClassType=DRAW_COM_BOM;
				pDrawingBOM->m_eBOMType=BOM_GENKAI;
#ifdef AFX_TARG_ENU_ENGLISH
				pDrawingBOM->m_sName.Copy("Tower Foot Distance BOM");
#else
				pDrawingBOM->m_sName.Copy("铁塔根开表");
#endif
				pDrawingBOM->CreateCmdList();
			}
			
			CLDSModule *pModule=NULL;
			CLDSModule *pActiveModule=Ta.Module.FromHandle(m_hActiveModule);
			if(pActiveModule)
			{	//正面图
				pComponent=AddComponentByView(pFrontView,pActiveModule);
				pComponent->m_bSpecifyComPos=FALSE;//主体图默认放置在左上角不需要拖放
				//侧面视图(标注节点号及杆件规格) wht 11-06-15
				if(liDrwTypeOrSegI==DARWING_MECHANICS&&pSideView)
					AddComponentByView(pSideView,pActiveModule,TRUE);
			}
			for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
			{
				if(pModule->handle!=m_hActiveModule)
					AddComponentByView(pFrontView,pModule);
			}
			if(liDrwTypeOrSegI==DARWING_MECHANICS)//司令图
			{	//定制视图
				for(pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
				{
					if(pView->m_iViewType!=1)
						continue;
					AddComponentByView(pView,NULL);	//定制视图
				}
			}
		}
	}
	else 
	{	//结构图
		//分段构件明细表
		pDrawingBOM=(CDrawingBOM*)listComponents.Add(BOM_SEGPART,DRAW_COM_BOM);
		//pDrawingBOM->m_pDrawing=this;
		pDrawingBOM->m_bNeedAdjust=FALSE;	//表格不需要进行调整
		pDrawingBOM->m_eClassType=DRAW_COM_BOM;
		pDrawingBOM->m_eBOMType=BOM_SEGPART;
#ifdef AFX_TARG_ENU_ENGLISH
		pDrawingBOM->m_sName.Copy("Parts BOM");
#else
		pDrawingBOM->m_sName.Copy("构件明细表");
#endif
		pDrawingBOM->CreateCmdList();
		//分段螺栓明细表
		pDrawingBOM=(CDrawingBOM*)listComponents.Add(BOM_SEGBOLT,DRAW_COM_BOM);
		//pDrawingBOM->m_pDrawing=this;
		pDrawingBOM->m_bNeedAdjust=FALSE;	//表格不需要进行调整
		pDrawingBOM->m_eClassType=DRAW_COM_BOM;
		pDrawingBOM->m_eBOMType=BOM_SEGBOLT;
#ifdef AFX_TARG_ENU_ENGLISH
		pDrawingBOM->m_sName.Copy("Bolts BOM");
#else
		pDrawingBOM->m_sName.Copy("螺栓明细表");
#endif
		pDrawingBOM->CreateCmdList();
		//
		int nCount=0;
		CDrawingComponent *ComponentPtrArr[4]={NULL};
		for(pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
		{
			if( pView->m_iViewType==0&&
				((stricmp(pView->defaultLayer,"SPQ")==0&&pView->filter.ThirdCharSet[0]!='?')||
				  stricmp(pView->defaultLayer,"SPY")==0))
			{
				if(stricmp(pView->defaultLayer,"SPY")!=0)
					AddComponentByView(pView,NULL,TRUE);	//前面图单线图
				pComponent=AddComponentByView(pView,NULL,FALSE);	//前面图结构图
				if(pComponent&&pComponent->m_eClassType==DRAW_COM_STRUVIEW)
				{
					if(nCount<3)
					{
						ComponentPtrArr[nCount]=pComponent;
						nCount++;
					}
				}
			}
			else if(pView->m_iViewType==1&&((SEGI(pView->folderName)).iSeg==m_nSegI.iSeg||pView->GetSegI().iSeg==m_nSegI.iSeg))
				AddComponentByView(pView,NULL,FALSE);	//定制视图结构图
		}
		//添加零件图,放置在定制视图之后 wht 11-07-26
		CHashStrList<CLDSPart*> partHashList;	//避免添加重复构件
		for(int i=0;i<nCount;i++)
			InitPartDrawing((CStruView*)ComponentPtrArr[i],partHashList);
	}
}

void CDrawing::InsertMapFrm()		//插入图框
{
	InitDrawingEnv();
	//获得当前图形的块表指针
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	DRAGSET.ClearEntSet();
	BOOL bSucceed=FALSE;
	bSucceed=InsertBlock(pBlockTableRecord,dcs.origin,m_xMapFrm.sMapFrameName,1.0,0,0);
	if(!bSucceed)//自定义图框
	{
		int i=1,nUnitSize=100;
		f3dPoint topLeft=GetMapFrmInsideTopLeft();
		//内侧图框
		CreateAcadRect(pBlockTableRecord,topLeft,m_xMapFrm.nLength-m_xMapFrm.nLeftMargin-m_xMapFrm.nRightMargin
			,m_xMapFrm.nWidth-m_xMapFrm.nTopMargin-m_xMapFrm.nBottomMargin
			,FALSE,NULL,f3dPoint(0,0,0),sys.stru_map.fPolyLineWidth);
		double txt_height=min(m_xMapFrm.nTopMargin,m_xMapFrm.nBottomMargin)*0.5;
		for(i=1;i<=m_xMapFrm.GetInsideLen()/nUnitSize;i++)
		{	//上侧刻度线
			f3dPoint start(topLeft.x+i*nUnitSize,topLeft.y);
			f3dPoint end(start.x,topLeft.y+m_xMapFrm.nTopMargin);
			f3dPoint dim_pos(start.x-0.5*nUnitSize,start.y+0.5*txt_height);
			CreateAcadLine(pBlockTableRecord,start,end);
			CXhChar16 dim_txt("%d",i);
			DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
					txt_height,0,AcDb::kTextCenter,AcDb::kTextBottom);
			//下侧刻度线
			start.y-=(m_xMapFrm.GetInsideWidth()+m_xMapFrm.nBottomMargin);
			end.y=start.y+m_xMapFrm.nBottomMargin;
			dim_pos.y=start.y+0.5*txt_height;
			CreateAcadLine(pBlockTableRecord,start,end);
			DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
					txt_height,0,AcDb::kTextCenter,AcDb::kTextBottom);
		}
		txt_height=min(m_xMapFrm.nLeftMargin,m_xMapFrm.nRightMargin)*0.5;
		for(i=1;i<=m_xMapFrm.GetInsideWidth()/nUnitSize;i++)
		{	//左侧刻度线
			f3dPoint start(topLeft.x,topLeft.y-i*nUnitSize);
			f3dPoint end(start.x-m_xMapFrm.nLeftMargin,start.y);
			f3dPoint dim_pos(start.x-0.5*txt_height,start.y+0.5*nUnitSize);
			CreateAcadLine(pBlockTableRecord,start,end);
			CXhChar16 dim_txt("%c",(int)('A'+i-1));
			DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
				txt_height,0,AcDb::kTextRight,AcDb::kTextVertMid);
			//右侧刻度线
			start.x+=m_xMapFrm.GetInsideLen();
			end.x=start.x+m_xMapFrm.nRightMargin;
			dim_pos.x=start.x+0.5*txt_height;
			CreateAcadLine(pBlockTableRecord,start,end);
			DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
				txt_height,0,AcDb::kTextLeft,AcDb::kTextVertMid);
		}
		//外侧图框
		topLeft.x-=m_xMapFrm.nLeftMargin;
		topLeft.y+=m_xMapFrm.nTopMargin;
		CreateAcadRect(pBlockTableRecord,topLeft,m_xMapFrm.nLength,m_xMapFrm.nWidth
			,FALSE,NULL,f3dPoint(0,0,0),sys.stru_map.fPolyLineWidth);
	}
	pBlockTableRecord->close();
	if(m_bSpecifyMapFrmPos)
	{
		f3dPoint basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
		DragEntSet(basepnt,"Input interting point");
#else
		DragEntSet(basepnt,"输入插入点");
#endif
		dcs.origin=basepnt;
		m_bSpecifyMapFrmPos=FALSE;
		//更新图元坐标系
		CDrawingComponent *pComponent=NULL;
		for(pComponent=GetFirstDrawingComponent();pComponent;pComponent=GetNextDrawingComponent())
		{
			if(pComponent->m_eClassType==DRAW_COM_LINEVIEW||pComponent->m_eClassType==DRAW_COM_STRUVIEW)
				pComponent->CalInitAssemblyPosition(); //计算初始装配位置
		}
	}
	//初始化矩形数组
	drawingRectArr.InitDrawingRectArray(m_xMapFrm.nLength,m_xMapFrm.nWidth,m_fComponentSpace);
	CStackAcadVar stackCmdEcho("CMDECHO",0);
#ifdef _ARX_2007
	ads_command(RTSTR,L"RE",RTNONE);
	acedCommand(RTSTR,L"zoom",RTSTR,L"a",RTNONE);
#else
	ads_command(RTSTR,"RE",RTNONE);
	acedCommand(RTSTR,"zoom",RTSTR,"a",RTNONE);
#endif
}

//绘制指定图纸中定制视图的剖视图符号
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
			continue;	//一般情况下只有定制视图才需要标注剖视图符
		//自动缩放到指定构件区域高亮显示剖视图中的构件在主视图中的位置
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
				if(fabs(line_vec*pComponent->m_pAttachView->ucs.axis_z)>0.70710) //<45°
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
		//输入剖视图起始点以及方向点
		CSectionViewSymbol::AddSectionViewSymbol(pMainView,(CStruView*)pComponent);
	}
}

//标注剖视图符
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
	//在此处标注子图纸定制视图的剖视图符，不容易查找暂时注释掉 wht 11-08-04
	/*
	//查找与当前图纸段号一致且无主视图(图层视图)的图纸
	CDrawing *pSonDrawing=NULL;
	for(pSonDrawing=drawingList.GetFirst();pSonDrawing;pSonDrawing=drawingList.GetNext())
	{
		if(pSonDrawing==this)
			continue;
		if(pSonDrawing->m_nSegI!=m_nSegI)
			continue;	//与当前图纸段号不一致
		pComponent=NULL;
		for(pComponent=pSonDrawing->GetFirstDrawingComponent();pComponent;pComponent=pSonDrawing->GetNextDrawingComponent())
		{
			if(pComponent->m_eClassType!=DRAW_COM_STRUVIEW)
				continue;
			if(pComponent->m_pAttachView&&pComponent->m_pAttachView->m_iViewType==0)
				break;	//存在主视图
		}
		if(pComponent)
			continue;
		DimSpecifyDrawingSectionViewSymbol(pSonDrawing,pMainView);
	}*/
	//显示整张图纸
	f2dRect rect;
	rect.topLeft.Set(dcs.origin.x,dcs.origin.y+m_xMapFrm.nWidth);
	rect.bottomRight.Set(dcs.origin.x+m_xMapFrm.nLength,dcs.origin.y);
	ZoomAcadView(rect,100);
}

//得到基准点即图框内侧矩形左上角顶点
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
	//清空构件绘制及标注状态
	ClearDrawnStates(CDrawingComponent::TYPE_JOINT);
	ClearDrawnStates(CDrawingComponent::TYPE_ROD);
	ClearDrawnStates(CDrawingComponent::TYPE_NODE);
	CDrawingComponent *pComponent=NULL;
	for(pComponent=GetFirstDrawingComponent();pComponent;pComponent=GetNextDrawingComponent())
		pComponent->Create();
}
//删除关联的CAD实体
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
	m_nSegI=pDrawing->m_nSegI;						//关联段号 -1表示司令图,-2表示总图
	dcs=pDrawing->dcs;								//坐标系
	m_nDrawingNumber=pDrawing->m_nDrawingNumber;	//图纸数量
	m_xMapFrm=pDrawing->m_xMapFrm;					//图框
	m_sDrawingName.Copy(pDrawing->m_sDrawingName);	//图纸名称
	m_sDrawingNo.Copy(pDrawing->m_sDrawingNo);		//图号
	m_bSpecifyMapFrmPos=pDrawing->m_bSpecifyMapFrmPos;	//指定图框位置
	m_hActiveModule=pDrawing->m_hActiveModule;		//当前激活呼高句柄
	m_pActiveModule=pDrawing->m_pActiveModule;		//当前激活呼高
	m_bAutoLayoutComponent=pDrawing->m_bAutoLayoutComponent;//自动布局
	m_fComponentSpace=pDrawing->m_fComponentSpace;	//相邻视图之间的绘图间隙
	//视图列表
	listComponents.Empty();
	CDrawingComponent *pComponent=NULL;
	for(pComponent=pDrawing->GetFirstDrawingComponent();pComponent;pComponent=pDrawing->GetNextDrawingComponent())
	{
		long key=pDrawing->listComponents.GetCursorKey();
		CDrawingComponent *pNewCom=AddComponent(key,pComponent->m_eClassType);
		pNewCom->CopyProperty(pComponent);
		//pNewCom->m_pDrawing=this;
	}
	//剖视图符列表
	sectionViewSymbols.Empty();
	CSectionViewSymbol *pSymbol=NULL;
	for(pSymbol=pDrawing->sectionViewSymbols.GetFirst();pSymbol;pSymbol=pDrawing->sectionViewSymbols.GetNext())
	{
		CSectionViewSymbol *pNewSymbol=sectionViewSymbols.append();
		pNewSymbol->CopyProperty(pSymbol);
	}
	//已布置矩形数组
	drawingRectArr=pDrawing->drawingRectArr;
}
*/
CDrawing& CDrawing::operator = (const CDrawing &drawing)
{
	//CopyProperty(&drawing);
	m_nSegI=drawing.m_nSegI;						//关联段号 -1表示司令图,-2表示总图
	dcs=drawing.dcs;								//坐标系
	m_nDrawingNumber=drawing.m_nDrawingNumber;	//图纸数量
	m_xMapFrm=drawing.m_xMapFrm;					//图框
	m_sDrawingName.Copy(drawing.m_sDrawingName);	//图纸名称
	m_sDrawingNo.Copy(drawing.m_sDrawingNo);		//图号
	m_bSpecifyMapFrmPos=drawing.m_bSpecifyMapFrmPos;	//指定图框位置
	m_hActiveModule=drawing.m_hActiveModule;		//当前激活呼高句柄
	m_pActiveModule=drawing.m_pActiveModule;		//当前激活呼高
	m_bAutoLayoutComponent=drawing.m_bAutoLayoutComponent;//自动布局
	m_fComponentSpace=drawing.m_fComponentSpace;	//相邻视图之间的绘图间隙
	return *this;
}

void CDrawing::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(m_nSegI);	//关联段号 -1表示司令图,-2表示总图
	buffer.WritePoint(dcs.origin);	//坐标系原点
	buffer.WritePoint(dcs.axis_x);	//坐标系X轴
	buffer.WritePoint(dcs.axis_y);	//坐标系Y轴
	buffer.WritePoint(dcs.axis_z);	//坐标系Z轴
	buffer.WriteInteger(m_nDrawingNumber);		//图纸数量
	buffer.WriteByte(m_xMapFrm.style);			//默认图框类型0-9
	buffer.WriteString(m_xMapFrm.sMapFrameName);//图框名称 A0 A0-2 A1 A1-2 A2 A2-2 A3 A3-2 A4 A4-2
	buffer.WriteInteger(m_xMapFrm.nLength);		//图框宽度
	buffer.WriteInteger(m_xMapFrm.nWidth);		//图框长度
	buffer.WriteWord(m_xMapFrm.nLeftMargin);	//左侧间隙
	buffer.WriteWord(m_xMapFrm.nRightMargin);	//右侧间隙
	buffer.WriteWord(m_xMapFrm.nTopMargin);		//上侧间隙
	buffer.WriteWord(m_xMapFrm.nBottomMargin);	//下侧间隙
	buffer.WriteWord(m_xMapFrm.nBottomRightFixLen);		//右下角固定长度
	buffer.WriteWord(m_xMapFrm.nBottomRightFixWidth);	//右下角固定宽度
	buffer.WriteString(m_sDrawingName);			//图纸名称
	buffer.WriteString(m_sDrawingNo);			//图号
	buffer.WriteInteger(m_bSpecifyMapFrmPos);	//指定图框位置
	buffer.WriteInteger(m_hActiveModule);		//当前激活模型组
	buffer.WriteInteger(m_bAutoLayoutComponent);//自动布局
	buffer.WriteDouble(m_fComponentSpace);		//相邻视图间距
	//视图链表
	CDrawingComponent *pComponent=NULL;
	buffer.WriteInteger(listComponents.GetNodeNum());
	for(pComponent=GetFirstDrawingComponent();pComponent;pComponent=GetNextDrawingComponent())
	{
		long key=listComponents.GetCursorKey();
		buffer.WriteInteger(key);
		buffer.WriteInteger(pComponent->m_eClassType);
		pComponent->ToBuffer(buffer);
	}
	//剖视图符号
	CSectionViewSymbol *pViewSymbol=NULL;
	buffer.WriteInteger(sectionViewSymbols.GetNodeNum());
	for(pViewSymbol=sectionViewSymbols.GetFirst();pViewSymbol;pViewSymbol=sectionViewSymbols.GetNext())
		pViewSymbol->ToBuffer(buffer);
}
void CDrawing::FromBuffer(CBuffer &buffer, long version/*=NULL*/)
{
	buffer.ReadInteger(&m_nSegI.iSeg);	//关联段号 -1表示司令图,-2表示总图
	buffer.ReadPoint(dcs.origin);	//坐标系原点
	buffer.ReadPoint(dcs.axis_x);	//坐标系X轴
	buffer.ReadPoint(dcs.axis_y);	//坐标系Y轴
	buffer.ReadPoint(dcs.axis_z);	//坐标系Z轴
	buffer.ReadInteger(&m_nDrawingNumber);	//图纸数量
	buffer.ReadByte(&m_xMapFrm.style);			//默认图框类型0-9
	if (version>=1000021)
		buffer.ReadString(m_xMapFrm.sMapFrameName,100);//图框名称 A0 A0-2 A1 A1-2 A2 A2-2 A3 A3-2 A4 A4-2
	buffer.ReadInteger(&m_xMapFrm.nLength);		//图框宽度
	buffer.ReadInteger(&m_xMapFrm.nWidth);		//图框长度
	buffer.ReadWord(&m_xMapFrm.nLeftMargin);	//左侧间隙
	buffer.ReadWord(&m_xMapFrm.nRightMargin);	//右侧间隙
	buffer.ReadWord(&m_xMapFrm.nTopMargin);		//上侧间隙
	buffer.ReadWord(&m_xMapFrm.nBottomMargin);	//下侧间隙
	buffer.ReadWord(&m_xMapFrm.nBottomRightFixLen);		//右下角固定长度
	buffer.ReadWord(&m_xMapFrm.nBottomRightFixWidth);	//右下角固定宽度
	buffer.ReadString(m_sDrawingName,50);		//图纸名称
	buffer.ReadString(m_sDrawingNo,50);			//图号
	buffer.ReadInteger(&m_bSpecifyMapFrmPos);	//指定图框位置
	buffer.ReadInteger(&m_hActiveModule);		//当前激活模型组
	buffer.ReadInteger(&m_bAutoLayoutComponent);//自动布局
	buffer.ReadDouble(&m_fComponentSpace);		//相邻视图间距
	//视图链表
	listComponents.Empty();
	long key=0,i=0,nCount=0,nType=0,buffer_len=buffer.GetLength();
	buffer.ReadInteger(&nCount);
	for(i=0;i<nCount;i++)
	{
		if(Ta.DisplayProcess)
#ifdef AFX_TARG_ENU_ENGLISH
			Ta.DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),"Drawing script reading progress");
#else
			Ta.DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),"绘图脚本读取进度");
#endif
		buffer.ReadInteger(&key);		//键值
		buffer.ReadInteger(&nType);		//视图类型
		CDrawingComponent* pComponent=(CDrawingComponent*)listComponents.Add(key,nType);
		//pComponent->m_pDrawing=this;
		pComponent->FromBuffer(buffer,version);
	}
	//剖视图符号
	sectionViewSymbols.Empty();
	long nSymbolNum=0;
	buffer.ReadInteger(&nSymbolNum);
	for(i=0;i<nSymbolNum;i++)
	{
		CSectionViewSymbol *pViewSymbol=sectionViewSymbols.append();
		pViewSymbol->FromBuffer(buffer,version);
		if(version<1000006)
		{	//以前版本记录的剖视图位置为相对与图框左上角的位置，在此处转换为绝对坐标 wht 11-07-25
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
bool CDrawing::IsItemDrawn(int stateType,long idItem)	//返回指定项目是否在当前图纸中绘制过
{
	for(CDrawingComponent *pComponent=(CDrawingComponent*)listComponents.GetFirst();pComponent;pComponent=(CDrawingComponent*)listComponents.GetNext())
	{
		if(pComponent->IsItemDrawn(stateType,idItem))
			return true;
	}
	return false;
}
bool CDrawing::IsItemDimmed(int stateType,long idItem)	//返回指定项目是否在当前图纸中标注过
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

//根据已生成表格重新计算当前表格初始位置
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
			if(pPrevFirstCmd->m_paraBOMTable.nLocatePos==0)		//左上
			{
				rect.topLeft=table_pos;
				rect.bottomRight.Set(table_pos.x+fTableWidth,table_pos.y-fTableHeight);
			}
			else if(pPrevFirstCmd->m_paraBOMTable.nLocatePos==1)//右上
			{
				rect.topLeft.Set(table_pos.x-fTableWidth,table_pos.y);
				rect.bottomRight.Set(table_pos.x,table_pos.y-fTableHeight);
			}
			else if(pPrevFirstCmd->m_paraBOMTable.nLocatePos==2)//左下
			{
				rect.topLeft.Set(table_pos.x,table_pos.y+fTableHeight);
				rect.bottomRight.Set(table_pos.x+fTableWidth,table_pos.y);
			}
			else if(pPrevFirstCmd->m_paraBOMTable.nLocatePos==3)//右下
			{
				rect.topLeft.Set(table_pos.x-fTableWidth,table_pos.x+fTableHeight);
				rect.bottomRight=table_pos;
			}
			else 
				continue;
			//当前表格定位点
			CDrawingCommand *pFirstCmd=pBOM->cmdList.GetFirst();
			if(pFirstCmd==NULL||pFirstCmd->m_eCmdType!=CMD_DRAW_TABLE)
				continue;
			f3dPoint topRight;
			if(fMaxHeight+rect.bottomRight.y>pFirstCmd->m_paraBOMTable.fTableHeight)
				topRight.Set(rect.bottomRight.x,rect.bottomRight.y);
			else 
				topRight.Set(rect.topLeft.x,rect.topLeft.y);
			if(pFirstCmd->m_paraBOMTable.nLocatePos==0)		//左上
			{
				pFirstCmd->m_paraBOMTable.fDimPosX=topRight.x-pFirstCmd->m_paraBOMTable.fTableWidth;
				pFirstCmd->m_paraBOMTable.fDimPosY=topRight.y;
			}
			else if(pFirstCmd->m_paraBOMTable.nLocatePos==1)//右上
			{
				pFirstCmd->m_paraBOMTable.fDimPosX=topRight.x;
				pFirstCmd->m_paraBOMTable.fDimPosY=topRight.y;
			}
			else if(pFirstCmd->m_paraBOMTable.nLocatePos==2)//左下
			{
				pFirstCmd->m_paraBOMTable.fDimPosX=topRight.x-pFirstCmd->m_paraBOMTable.fTableWidth;
				pFirstCmd->m_paraBOMTable.fDimPosY=topRight.y-pFirstCmd->m_paraBOMTable.fTableHeight;
			}
			else if(pFirstCmd->m_paraBOMTable.nLocatePos==3)//右下
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
		PreparePartSpecList(pDrawingBom->SpecList);		//简化的规格列表(司令图标注杆件规格时使用)
	dimpartspec.SpecList.Empty();
	if(pDrawingBom)
	{	//将智能绘图数据中的简化规格表同步到全局构件明细标注表中
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

//更新图纸内所有图元的命令列表 wht 11-07-31
void CDrawing::UpdateCmdList()
{
	CDrawingComponent *pDrawCom=NULL;
	if(m_nSegI.iSeg==-1||m_nSegI.iSeg>=0)
	{	//司令图或结构图，根据当前塔中定制视图列表更新图纸中定制视图
		CHashList<CDrawingComponent*> hashViewList;
		for(pDrawCom=GetFirstDrawingComponent();pDrawCom;pDrawCom=GetNextDrawingComponent())
		{
			if(pDrawCom->m_eClassType!=DRAW_COM_STRUVIEW&&pDrawCom->m_eClassType!=DRAW_COM_LINEVIEW)
				continue;	//非单线图和结构图
			if(pDrawCom->m_pAttachView==NULL)
				pDrawCom->m_pAttachView=Ta.View.FromHandle(pDrawCom->m_hAttachView);
			if(pDrawCom->m_pAttachView==NULL)
				listComponents.DeleteCursor();	//删除无对应视图的图元
			else if(pDrawCom->m_pAttachView->m_iViewType==1)
				hashViewList.SetValue(pDrawCom->m_hAttachView,pDrawCom);
		}
		CDisplayView *pView=NULL;
		for(pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
		{
			if(pView->m_iViewType!=1)
				continue;
			if(m_nSegI.iSeg>=0)
			{	//结构图需判断是否为当前段定制视图
				if(pView->GetSegI().iSeg!=m_nSegI.iSeg&&SEGI(pView->folderName).iSeg!=m_nSegI.iSeg)
					continue;	//非当前段定制视图
				if(!hashViewList.GetValue(pView->handle))	//该定制视图无对应图元需要添加
				{
					pDrawCom=AddComponentByView(pView,NULL,FALSE);	
					pDrawCom->feature=-1;	//新增的图元
				}
			}
			else if(m_nSegI.iSeg==-1&&!hashViewList.GetValue(pView->handle))
			{
				pDrawCom=AddComponentByView(pView,NULL,TRUE);		//新增司令图定制视图
				pDrawCom->feature=-1;	//新增的图元
			}
		}
	}
	for(pDrawCom=GetFirstDrawingComponent();pDrawCom;pDrawCom=GetNextDrawingComponent())
	{
		if(pDrawCom->m_eClassType!=DRAW_COM_STRUVIEW&&pDrawCom->m_eClassType!=DRAW_COM_LINEVIEW)
			continue;	//非单线图和结构图
		if(pDrawCom->feature!=-1)
			pDrawCom->UpdateCmdList();	//更新图元对应的命令列表
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
	m_cCurrSegType=0;
}

CDrawingComponent::~CDrawingComponent()
{

}

//在计算视图真实绘图区域时通过bOnlyIncludePartEnt控制仅计算构件绘制区域(获取名称标注位置时使用)
f2dRect CDrawingComponent::GetDrawRegion(BOOL bRealRect/*=TRUE*/,BOOL bOnlyIncludePartEnt/*=FALSE*/)
{
	f2dRect rect;
	SCOPE_STRU scope;
	scope.ClearScope();
	scope.fMinX = scope.fMinY = scope.fMinZ = 100000.0;
	scope.fMaxX = scope.fMaxY = scope.fMaxZ =-100000.0;
	//由于文本绘制区域估值不准在获取BOM表区域时，需要过滤掉非直线型图元 wjh-2019.5.26
	bool blFilterOnlyBomTblLine=(this->m_eClassType==DRAW_COM_BOM);
	if(bRealRect)
	{	//根据绘制出的CAD实体计算真实的绘图区域
		AcDbObjectId *pEntId=NULL;
		CDrawingCommand *pCmd=NULL;
		for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
		{
			if(bOnlyIncludePartEnt&&pCmd->m_eCmdType!=CMD_DRAW_LINEPART&&pCmd->m_eCmdType!=CMD_DRAW_PART&&pCmd->m_eCmdType!=CMD_DRAW_JG_JOINT)
				continue;	//仅计算构件绘图区域
			for(pEntId=pCmd->entIdList.GetFirst();pEntId;pEntId=pCmd->entIdList.GetNext())
				VerifyVertexByCADEntId(scope,*pEntId,blFilterOnlyBomTblLine);
		}
		//附加实体集(上接段号和视图名称对应的实体)
		CAD_OBJECT_ID *pObjId=NULL;
		for(pObjId=entIdList.GetFirst();pObjId;pObjId=entIdList.GetNext())
			VerifyVertexByCADEntId(scope,pObjId->entId,blFilterOnlyBomTblLine);
		if(ParentDrawing())
		{	//从属于当前图元的剖视图符号对应的实体集 
			CSectionViewSymbol *pSymbol=NULL;
			long key=ParentDrawing()->listComponents.GetKey(this);
			for(pSymbol=ParentDrawing()->sectionViewSymbols.GetFirst();pSymbol;pSymbol=ParentDrawing()->sectionViewSymbols.GetNext())
			{
				if(pSymbol->hAttachViewKey==key)
				{
					for(pEntId=pSymbol->startEntIds.GetFirst();pEntId;pEntId=pSymbol->startEntIds.GetNext())
						VerifyVertexByCADEntId(scope,*pEntId,blFilterOnlyBomTblLine);
					for(pEntId=pSymbol->endEntIds.GetFirst();pEntId;pEntId=pSymbol->endEntIds.GetNext())
						VerifyVertexByCADEntId(scope,*pEntId,blFilterOnlyBomTblLine);
				}
			}
		}
		rect.topLeft.Set(scope.fMinX,scope.fMaxY);
		rect.bottomRight.Set(scope.fMaxX,scope.fMinY);
	}
	else 
	{	//根据构件链表以及绘图比例计算大概的绘图区域
		if(nodeSet.GetNodeNum()<=0||partSet.GetNodeNum()<=0)
			GetDrawingObjectSet(FALSE);	//构件列表为空时重新初始化构件列表
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
		{	//构件图
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
		else if(m_eClassType==DRAW_COM_STDVIEW)
		{
			CStdView *pJgJointView=(CStdView*)this;
			double fWidth=pJgJointView->m_pPart->GetWidth()*m_fScale;
			double fLength=fWidth*4;	
			f3dPoint origin=pJgJointView->m_pPart->ucs.origin;
			coord_trans(origin,dcs,FALSE);
			origin*=m_fScale;
			rect.topLeft.Set(origin.x,origin.y);
			rect.bottomRight.Set(origin.x+fLength,origin.y-fWidth);
			scope.fMinX=rect.topLeft.x;
			scope.fMaxY=rect.topLeft.y;
			scope.fMaxX=rect.bottomRight.x;
			scope.fMinY=rect.bottomRight.y;
		}
		else
		{
			rect.topLeft.Set(scope.fMinX*m_fScale,scope.fMaxY*m_fScale);
			rect.bottomRight.Set(scope.fMaxX*m_fScale,scope.fMinY*m_fScale);
			//根据模型尺寸标注参数计算视图区域
			if(m_eClassType==DRAW_COM_LINEVIEW)
			{
				double fMaxLeftSpace=0,fMaxTopSpace=0;
				CDrawingCommand *pCmd=NULL;
				for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
				{
					if(pCmd->m_eCmdType!=CMD_DIM_MODELSIZE)
						continue;
					if(pCmd->m_paraDimModelSize.iDimType>=4)
					{	//水平标注
						double fTopSpace=pCmd->m_paraDimModelSize.fSpace+pCmd->m_paraDimModelSize.fDimDistance;
						if(fTopSpace>fMaxTopSpace)
							fMaxTopSpace=fTopSpace;
					}
					else
					{	//垂直标注
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
	if(fabs(fabs(width)-200000)<EPS)	//空区域
		rect.topLeft=rect.bottomRight=f3dPoint(0,0);
	draw_rect=rect;
	return rect;
}

//
f2dRect CDrawingComponent::GetComponentRect()
{
	if(draw_rect.Width()<EPS||draw_rect.Height()<EPS)
		GetDrawRegion(FALSE);	//初始化绘图区域
	if(draw_rect.Width()<EPS||draw_rect.Height()<EPS)
		GetDrawRegion(TRUE);	//真实的绘图区域
	return draw_rect;
}  
//生成视图后需要更新绘图区域
int CDrawingComponent::UpdateDrawingRectArray()
{
	f3dPoint datum_pos=GetMapFrmInsideTopLeft();
	f2dRect real_rect=GetDrawRegion(TRUE);
	f3dPoint topLeft(real_rect.topLeft.x-datum_pos.x,real_rect.topLeft.y-datum_pos.y);
	return ParentDrawing()->drawingRectArr.AddUsedDrawingRect(topLeft,real_rect.Width(),real_rect.Height());
}
//生成视图
void CDrawingComponent::Create()
{
	if(ParentDrawing()==NULL)
		return;
}
//删除当前视图关联数据
void CDrawingComponent::EraseRelaCadEnt()
{
	//删除附加实体
	CAD_OBJECT_ID *pObjId=NULL;
	for(pObjId=entIdList.GetFirst();pObjId;pObjId=entIdList.GetNext())
	{	//删除有改命令生成的实体
		AcDbEntity *pEnt=NULL;
		if(acdbOpenObject(pEnt,pObjId->entId,AcDb::kForWrite)!=Acad::eOk)
			continue;
		if(pEnt==NULL)
			continue;
		pEnt->erase();
		pEnt->close();
	}
	entIdList.Empty();	//清空附加实体列表
	//删除命令相关的实体
	CDrawingCommand *pCommand=NULL;
	for(pCommand=cmdList.GetFirst();pCommand;pCommand=cmdList.GetNext())
		pCommand->EmptyEntList();
	//删除关键实体集(主要删除扩展数据点)
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
	keyEntIdList.Empty();	//清空附带构件句柄的实体列表
	//删除从属于当前图元的剖视图符对应的CAD实体 wht 11-08-01
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
	//刷新界面
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
}
//删除当前视图关联数据重新生成
void CDrawingComponent::ReCreate()
{	//清空构件绘制及标注状态
	hashJointDrawnStates.Empty();
	hashNodesDrawn.Empty();
	hashJointDrawnStates.Empty();
	//删除当前视图关联数据
	EraseRelaCadEnt();
	//重新生成
	Create();
	//刷新界面
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
	//将所选组件开窗放大
	f2dRect rect=GetDrawRegion(TRUE);
	ZoomAcadView(rect,50);
}
//得到基准点即图框内侧矩形左上角顶点
f3dPoint CDrawingComponent::GetMapFrmInsideTopLeft()
{
	f3dPoint datum_pos;
	if(ParentDrawing())
		datum_pos=ParentDrawing()->GetMapFrmInsideTopLeft();
	return datum_pos;
}
//需要制定视图摆放位置
BOOL CDrawingComponent::IsNeedSpecifyComPos()
{
	if(ParentDrawing()->m_bAutoLayoutComponent)
		return FALSE;
	else 
		return m_bSpecifyComPos;
}
//根据构件句柄得到指定的CAD实体
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
	//m_pDrawing=pCom->m_pDrawing;		//归属图纸不允许赋值更改
	m_hAttachView=pCom->m_hAttachView;	//关联视图句柄
	m_pAttachView=pCom->m_pAttachView;	//关联视图
	m_hModule=pCom->m_hModule;			//关联呼高句柄
	m_pModule=pCom->m_pModule;			//司令图或总图中绘制非当前呼高塔腿单线图
	m_sDrawingLayer.Copy(pCom->m_sDrawingLayer);
	m_fNamePosSpace=pCom->m_fNamePosSpace;
	
	drawing_offset=pCom->drawing_offset;//偏移量
	draw_rect=pCom->draw_rect;
	dcs=pCom->dcs;						//绘图坐标系		
	m_sName.Copy(pCom->m_sName);		//视图名称
	m_eClassType=pCom->m_eClassType;	//视图类型 
	m_nAnchorType=pCom->m_nAnchorType;	//定位类型 0.左上 1.右上 2.左下 3.右下
	m_bSpecifyComPos=pCom->m_bSpecifyComPos;//指定视图位置
	m_bNeedAdjust=pCom->m_bNeedAdjust;	//该视图需要调整
	m_bFirstCreate=pCom->m_bFirstCreate;
	m_fScale=pCom->m_fScale;			//绘制比例
	//绘图命令链表
	cmdList.Empty();
	CDrawingCommand *pCommand=NULL;
	for(pCommand=pCom->cmdList.GetFirst();pCommand;pCommand=pCom->cmdList.GetNext())
	{
		CDrawingCommand *pNew=cmdList.append();
		pNew->CopyProperty(pCommand);
		pNew->m_pDrawingCom=this;	//重新设置命令关联视图		
	}
	//对应的CAD中实体ID,标注构件明细时使用
	keyEntIdList.Empty();
	AcDbObjectId *pObjID=NULL;
	for(pObjID=pCom->keyEntIdList.GetFirst();pObjID;pObjID=pCom->keyEntIdList.GetNext())
	{
		long key=pCom->keyEntIdList.GetKey(pObjID);
		keyEntIdList.SetValue(key,*pObjID);
	}
	//构件列表
	partSet.Empty();
	partHandleList.Empty();
	CLDSPart *pPart=NULL;
	for(pPart=pCom->GetFirstPart();pPart;pPart=pCom->GetNextPart())
	{
		partSet.append(pPart);
		partHandleList.append(pPart->handle);
	}
	//节点列表
	nodeSet.Empty();
	nodeHandleList.Empty();
	CLDSNode *pNode=NULL;
	for(pNode=pCom->GetFirstNode();pNode;pNode=pCom->GetNextNode())
	{
		nodeSet.append(pNode);
		nodeHandleList.append(pNode->handle);
	}
}

//修改定位点相对位置
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
	//更新命令参数
	CDrawingCommand *pCommand=NULL;
	for(pCommand=cmdList.GetFirst();pCommand;pCommand=cmdList.GetNext())
		pCommand->UpdateCommandPara(fOffsetX,fOffsetY);
	if(m_eClassType==DRAW_COM_STRUVIEW)
	{	//更新剖视图符位置
		long key=ParentDrawing()->listComponents.GetKey(this);
		CSectionViewSymbol *pViewSymbol=NULL;
		for(pViewSymbol=ParentDrawing()->sectionViewSymbols.GetFirst();pViewSymbol;pViewSymbol=ParentDrawing()->sectionViewSymbols.GetNext())
		{
			if(pViewSymbol->hAttachViewKey==key)//更新当前视图中剖视图的位置参数
				pViewSymbol->UpdatePosition(fOffsetX,fOffsetY);
		}
	}
}
//设置定位点相对于图框左上角的相对位置
f3dPoint CDrawingComponent::GetAnchorPointRelativePos()
{
	f3dPoint anchor_point;
	f2dRect rect=GetDrawRegion();
	if(rect.Width()<EPS||rect.Height()<EPS)
		return anchor_point;
	//计算定位点相对与图框左上角的坐标
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
//通过相对位置计算装配位置
void CDrawingComponent::CalAssemblyPosByRelativePos()
{
	//使用相对位置重新计算坐标系原点位置
	f3dPoint datum_pos=GetMapFrmInsideTopLeft();
	dcs.origin=datum_pos+m_ptLocation;	
}
*/
//计算视图初始装配位置 
//插入图框以及定位类型改变后都需要重新计算初始装配位置
void CDrawingComponent::CalInitAssemblyPosition()
{
	f2dRect rect;
	rect=GetDrawRegion(FALSE);
	double fRectHeight=rect.Height();
	double fRectWidth=rect.Width();
	double fMinX=rect.topLeft.x;	
	double fMaxY=rect.topLeft.y;
	f3dPoint datum_pos=GetMapFrmInsideTopLeft();
	//在绘图矩形框数组中查找一个可用的空白区域
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
	//绘图默认基点到目标基点的偏移矢量
	drawing_offset=offset_pos;	
}
//指定视图位置
f3dPoint CDrawingComponent::SpecifyAssemblyPosition()
{
	f3dPoint offset;
	if(!IsNeedSpecifyComPos())
	{
		if(m_bSpecifyComPos)
			m_bSpecifyComPos=FALSE;
		//根据偏移位置平移实体集
		/*f3dPoint datum_pos=GetMapFrmInsideTopLeft();
		f3dPoint cur_pos=datum_pos+m_ptLocation;
		if(cur_pos!=dcs.origin)
		{
			TranslationEntSet(dcs.origin,cur_pos,DRAGSET.drag_ent_set);
			dcs.origin=cur_pos;	//更新坐标系原点位置 wht 11-06-25
		}*/
		return offset;
	}
	//根据指定位置更新视图位置以及视图相对于图框左上角的位置
	f3dPoint basepnt;
	//第一次生成视图需要指定插入点
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
#ifdef _ARX_2007
	acedCommand(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
#else
	acedCommand(RTSTR,"zoom",RTSTR,"e",RTNONE);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	CXhChar100 sPrompt("Please input [%s] inserting point\n",&m_sName);
#else
	CXhChar100 sPrompt("请输入[%s]插入点\n",&m_sName);
#endif
	int nRetCode=DragEntSet(basepnt,sPrompt);
	if(nRetCode==RTNORM)
	{	//视图位置变动后才需要更新坐标系位置
		offset.x=basepnt.x-old_pos.x;
		offset.y=basepnt.y-old_pos.y;
		//拖放视图位置后同时更新偏移矢量 wht 11-06-28
		drawing_offset.x+=offset.x;
		drawing_offset.y+=offset.y;
	}
	m_bSpecifyComPos=FALSE;
	return offset;
}

//插入视图名称
void CDrawingComponent::InsertDrawingName()
{
	if(entIdList.GetNodeNum()>0)
	{	//删除组件名称及上接段号对应的实体 wht 11-07-31
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
		//更新界面
		actrTransactionManager->flushGraphics(); //关闭打开的实体后才可成功刷新缓存
		acedUpdateDisplay();
	}
	entIdList.Empty();
	//1.计算标注位置
	f3dPoint dim_pos;
	//初始化标注位置
	f2dRect rect=GetDrawRegion(TRUE,TRUE);
	if(rect.Width()<EPS||rect.Height()<EPS)
		return;	//绘图区域为NULL 无需标注视图名称
	dim_pos.Set((rect.topLeft.x+rect.bottomRight.x)*0.5,rect.bottomRight.y-m_fNamePosSpace);
	double fOutspreadLen=0.5;	//下划线外侧延伸量
	CXhChar50 sComponentName(m_sName);
	if(sComponentName.GetLength()==0&&m_pAttachView&&m_pAttachView->m_iViewType==1)
		sComponentName.Copy(m_pAttachView->name);
	double nLineLen=sComponentName.GetLength()*sys.dim_map.fCutawayViewSymbolTextSize*0.7+2*fOutspreadLen;
	f3dLine line;
	double nSpace=0.5;		//两下划线之间的间隙
	line.startPt.Set(dim_pos.x-0.5*nLineLen,dim_pos.y-sys.dim_map.fCutawayViewSymbolTextSize-nSpace);
	line.endPt.Set(dim_pos.x+0.5*nLineLen,line.startPt.y);
	//2.生成视图名称标注
	AcDbObjectId entId=0;
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	if(m_eClassType==DRAW_COM_PARTVIEW)
	{	
		CPartDrawing *pPartDrawing=(CPartDrawing*)this;
		if(pPartDrawing->m_pPart==NULL)
			pPartDrawing->m_pPart=Ta.FromPartHandle(pPartDrawing->m_hPart);
		/*if(pPartDrawing->m_pPart)
		{	//构件图标注名称时编号需加圈
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
	//3.标注绘图比例
	if (m_bDimScale)
	{
		f3dPoint scale_dim_pos=dim_pos;
		scale_dim_pos.y=line.endPt.y-nSpace;
		CXhChar50 sDrawScale("1:%d",ftoi(1.0/m_fScale));
		entId=DimText(pBlockTableRecord,scale_dim_pos,sDrawScale,TextStyleTable::hzfs.textStyleId,
			sys.dim_map.fCutawayViewSymbolTextSize,0,AcDb::kTextCenter,AcDb::kTextTop);
		entIdList.append(CAD_OBJECT_ID(1,entId));

	}
	//4.标注上接段号
	if(m_eClassType==DRAW_COM_LINEVIEW	//单线图
		&&((m_pModule&&m_pModule->handle!=ParentDrawing()->m_hActiveModule)||ParentDrawing()->m_nSegI.iSeg>0))	//非激活呼高或分段单线图
	{
		//查找最高节点中X值最小最大的节点
		CLDSNode *pNode=NULL,*pHighestNode[2]={NULL};
		pHighestNode[0]=pHighestNode[1]=nodeSet.GetFirst();
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{
			if(pNode->xOriginalPos.z<pHighestNode[0]->xOriginalPos.z)
				pHighestNode[0]=pHighestNode[1]=pNode;
			if(fabs(pNode->xOriginalPos.z-pHighestNode[0]->xOriginalPos.z)<EPS)
			{	//Z坐标相等
				if(pNode->xOriginalPos.x>pHighestNode[1]->xOriginalPos.x)
					pHighestNode[1]=pNode;
				if(pNode->xOriginalPos.x<pHighestNode[0]->xOriginalPos.x)
					pHighestNode[0]=pNode;
			}
		}
		SEGI nUpSegI=m_cCurrSegType!='T'?this->ParentDrawing()->m_nAdjoinSegI:0;
		if(nUpSegI.iSeg!=-1&&nUpSegI.iSeg!=0)
		{
			dim_pos.Set(0.5*(pHighestNode[0]->Position('X',false)+pHighestNode[1]->Position('X',false)),pHighestNode[0]->Position(false).y,pHighestNode[0]->Position('Z',false));
			coord_trans(dim_pos,dcs,FALSE);
			dim_pos.z=0;
			dim_pos*=m_fScale;
			dim_pos+=drawing_offset;
			dim_pos.y+=m_fNamePosSpace;	//标注间隙
			CXhChar16 sUpSegI;
#ifdef AFX_TARG_ENU_ENGLISH
			sUpSegI.Printf("Up  %s  Segment",(char*)nUpSegI.ToString());
#else
			sUpSegI.Printf("上接  %s  段",(char*)nUpSegI.ToString());
#endif
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
	pBlockTableRecord->close();
}

//修组件名称
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
				pText->setTextString((ACHAR*)_bstr_t(com_name));
		#else
			if(stricmp(pText->textString(),sOldName)==0)
				pText->setTextString(com_name);
		#endif
		}
		pEnt->close();
	}
	//更新界面
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
}

//获取附属实体集
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
			ads_redraw(ent_name,3);//高亮显示
		if(pScope)
			VerifyVertexByCADEntId(*pScope,pObjId->entId);
		if(acedSSAdd(ent_name,drag_ent_set,drag_ent_set)!=RTNORM)
			continue;
		struct resbuf *pData=acdbEntGet(ent_name);
		acdbEntMod(pData);
		acdbEntUpd(ent_name);
	}
}

//获得视图对应的实体集合
void CDrawingComponent::GetEntSet(ads_name &drag_ent_set,BOOL bSelectEnts/*=FALSE*/)
{
	//获取命令相关实体集
	CDrawingCommand *pCommand=NULL;
	for(pCommand=cmdList.GetFirst();pCommand;pCommand=cmdList.GetNext())
		pCommand->GetEntSet(drag_ent_set,bSelectEnts);
	//获得附属实体集
	GetAttachEntSet(drag_ent_set,0,bSelectEnts);
}
//
void CDrawingComponent::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(m_hAttachView);	//关联视图句柄
	buffer.WriteInteger(m_hModule);		//关联呼高句柄
	buffer.WriteString(m_sDrawingLayer);//图层范围
	buffer.WriteDouble(m_fNamePosSpace);//视图名称与视图底部的间隙值
	buffer.WritePoint(dcs.origin);		//绘图坐标系原点
	buffer.WritePoint(dcs.axis_x);		//绘图坐标系X轴
	buffer.WritePoint(dcs.axis_y);		//绘图坐标系Y轴
	buffer.WritePoint(dcs.axis_z);		//绘图坐标系Z轴
	buffer.WriteString(m_sName);		//视图名称
	buffer.WriteString(m_sSectionViewCode);	//剖视图代号
	buffer.WriteInteger(m_eClassType);	//视图类型 
	buffer.WriteInteger(m_nAnchorType);	//定位类型 0.左上 1.右上 2.左下 3.右下
	buffer.WritePoint(drawing_offset);	//偏移量
	buffer.WriteInteger(m_bSpecifyComPos);//指定视图位置
	buffer.WriteInteger(m_bNeedAdjust);	//该视图需要调整
	buffer.WriteInteger(m_bFirstCreate);//第一次生成
	buffer.WriteDouble(m_fScale);		//绘制比例
	buffer.WriteInteger(m_bDimName);	//标注视图名称
	buffer.WriteInteger(m_bDimScale);	//标注绘制比例
	//绘图命令链表
	buffer.WriteInteger(cmdList.GetNodeNum());
	CDrawingCommand *pCommand=NULL;
	for(pCommand=cmdList.GetFirst();pCommand;pCommand=cmdList.GetNext())
		pCommand->ToBuffer(buffer);
}

void CDrawingComponent::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.ReadInteger(&m_hAttachView);	//关联视图句柄
	m_pAttachView=(CDisplayView*)Ta.View.FromHandle(m_hAttachView);
	buffer.ReadInteger(&m_hModule);		//关联呼高句柄
	m_pModule=(CLDSModule*)Ta.Module.FromHandle(m_hModule);
	buffer.ReadString(m_sDrawingLayer,16);//图层范围
	buffer.ReadDouble(&m_fNamePosSpace);//视图名称与视图底部的间隙值
	buffer.ReadPoint(dcs.origin);		//绘图坐标系原点
	buffer.ReadPoint(dcs.axis_x);		//绘图坐标系X轴
	buffer.ReadPoint(dcs.axis_y);		//绘图坐标系Y轴
	buffer.ReadPoint(dcs.axis_z);		//绘图坐标系Z轴
	buffer.ReadString(m_sName,50);		//视图名称
	buffer.ReadString(m_sSectionViewCode,16);	//剖视图代号
	long nType=0;
	buffer.ReadInteger(&nType);			//视图类型 
	m_eClassType=(DRAW_COMPONENT_TYPE)nType;
	buffer.ReadInteger(&nType);			//定位类型 0.左上 1.右上 2.左下 3.右下
	m_nAnchorType=(TBL_LOCATE_STYLE)nType;
	if(version<1000004)
	{
		f3dPoint pt;
		buffer.ReadPoint(pt);		//坐标系原点相对位置
		if(version>1000001)
			buffer.ReadPoint(pt);	//定位点相对位置
	}
	else 
		buffer.ReadPoint(drawing_offset);
	buffer.ReadInteger(&m_bSpecifyComPos);//指定视图位置
	buffer.ReadInteger(&m_bNeedAdjust);	//该视图需要调整
	buffer.ReadInteger(&m_bFirstCreate);//第一次生成
	buffer.ReadDouble(&m_fScale);		//绘制比例
	if (version>=1000020)
	{
		buffer.ReadInteger(&m_bDimName);
		buffer.ReadInteger(&m_bDimScale);
	}
	//绘图命令链表
	CDrawingCommand *pCommand=NULL;
	cmdList.Empty();
	long i=0,nCount=0,buffer_len=buffer.GetLength();
	buffer.ReadInteger(&nCount);
	CHashList<f3dLine>hashOddLine;
	for(i=0;i<nCount;i++)
	{
		if(Ta.DisplayProcess)
#ifdef AFX_TARG_ENU_ENGLISH
			Ta.DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),"Drawing script reading progress");
#else
			Ta.DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),"绘图脚本读取进度");
#endif
		pCommand=cmdList.append();
		pCommand->m_pDrawingCom=this;
		pCommand->FromBuffer(buffer,version);
		if(version<1000013)
		{	//小于V1000013的版本内,正负头标注位置是绝对坐标要转为相对坐标 wjh-2011.9.25
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
			{	//因以往数据中都是先生成绘制杆件命令,然后才生成标注正负头命令,故可以在同一个循环里
				f3dLine *pBaseLine=hashOddLine.GetValue(pCommand->m_paraOddment.hLinePart);
				if(pBaseLine)
				{	//由原有的绝对坐标转换到相对于标注基点的相对偏移量坐标
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

//初始化组件命令对应的feature wht 11-07-22
void CDrawingComponent::InitCmdFeature(int nFeature)	
{
	CDrawingCommand *pCmd=NULL;
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
		pCmd->feature=nFeature;
}
//绘制状态、标注状态相关函数
void CDrawingComponent::ClearDrawnStates(int stateType/*=TYPE_ROD*/)
{
	if(stateType==TYPE_ROD)
		hashRodDrawnStates.Empty();
	else if(stateType==TYPE_JOINT)
		hashJointDrawnStates.Empty();
}
bool CDrawingComponent::IsItemDrawn(int stateType,long idItem)	//返回指定项目是否在当前图纸中绘制过
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
bool CDrawingComponent::IsItemDimmed(int stateType,long idItem)	//返回指定项目是否在当前图纸中标注过
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
	m_eClassType=DRAW_COM_BOM;	//视图类型
	m_eBOMType=(DRAW_BOM_TYPE)0;//表格类型
	m_nAnchorType=TOP_RIGHT;
	m_bAutoNewTable=TRUE;	//表格过长时自动生成新的表格
	m_bHasTableHead=TRUE;	//新表格是否带表头
	m_nBriefGuigeN=min(sys.nBriefGuigeN,26);//最大不能超过26
}

CDrawingBOM::~CDrawingBOM()
{
	
}
//生成视图后需要更新绘图区域
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
//生成视图
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
	{	//第一次生成视图需要指定插入点
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
	//插入图元后将图元矩形区域添加到绘图矩形框数组中
	UpdateDrawingRectArray();
	CStackAcadVar stackCmdEcho("CMDECHO",0);
#ifdef _ARX_2007
	acedCommand(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
	ads_command(RTSTR,L"RE",RTNONE);
#else
	acedCommand(RTSTR,"zoom",RTSTR,"e",RTNONE);
	ads_command(RTSTR,"RE",RTNONE);
#endif
}

//更新表格初始位置
void CDrawingBOM::UpdateTableInitPos()
{
	CDrawingCommand *pFirstCmd=cmdList.GetFirst();
	CDrawingCommand *pSecCmd=cmdList.GetNext();
	if(pFirstCmd==NULL)
		return;
	if(pFirstCmd->m_eCmdType!=CMD_DRAW_TABLE)
		return;
	//初始化表格位置
	if(m_nAnchorType==TOP_RIGHT)
	{
		drawing_offset=ParentDrawing()->GetMapFrmInsideTopLeft();
		drawing_offset.x+=ParentDrawing()->m_xMapFrm.GetInsideLen();
		pFirstCmd->m_paraBOMTable.fDimPosX=ParentDrawing()->m_xMapFrm.GetInsideLen();
		pFirstCmd->m_paraBOMTable.fDimPosY=0;
		if(pSecCmd&&xFormatTbl2.GetRowsCount()>0)
		{	//第二张表格放置在第一张表格左侧
			pFirstCmd->m_paraBOMTable.fDimPosX=ParentDrawing()->m_xMapFrm.GetInsideLen()-xFormatTbl1.GetTableWide();
			pFirstCmd->m_paraBOMTable.fDimPosY=0;
		}
	}
}
double FindGenKaiDistance(CLDSModule* pModule,CDisplayView* pFrontView)
{
	//1,获取当前呼高前视图的节点集合
	NODESET sumNodeSet,nodeSet;
	LINEPARTSET linePartSet;
	pModule->RefreshModule(sumNodeSet,linePartSet);
	for(CLDSNode* pNode=sumNodeSet.GetFirst();pNode;pNode=sumNodeSet.GetNext())
	{	
		if(pFrontView->filter.IsValidLayer(pNode->layer()))
			nodeSet.append(pNode);
	}
	//2,获取最低坡口线两个节点
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
BOOL PrepareSegPartTable(CFormatTbl &xPartTbl,SEGI nSegI=-1);	//预先准备分段构件明细表数据
void PrepareSegBoltTable(CFormatTbl &xPartTbl,SEGI nSegI=-1);	//预先准备分段螺栓明细表数据
//From SumMap.cpp
void PrepareSumMatTable(CFormatTbl &xPartTbl);		//预先准备材料汇总表数据
//From TsaFunc.cpp
void PreparePartSpecList(ATOM_LIST<SPEC_MARK> &specList);//预先准备简化的规格列表(司令图标注杆件规格时使用)
void PreparePartSpecBriefTbl(ATOM_LIST<SPEC_MARK> &specList,CFormatTbl &xSpecTbl,int nBriefGuigeN);//预先准备杆件规格简化表样式
void PrepareGenKaiTable(CFormatTbl &xPartTbl)		//预先准备铁塔根开表数据
{
	int nBodyN=Ta.Module.GetNodeNum();
	int i=0,j=0;
	CGrid::PREFER_TEXT_SIZE=sys.dim_map.fNumericTextSize;	//初始化表格单元字体高度
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
	//设置表格宽度
	xPartTbl.SetRowHigh(0,16);		//标题行
	xPartTbl.SetRowHigh(1,8);		//铁塔根开,基础根开行
	xPartTbl.SetRowHigh(2,8);		//正面—侧面行
	//for(i=3;i<nRowsN-1;i++)			//数据行
	//	xPartTbl.SetRowHigh(i,6);
	for(i=0;i<5;i++)
		xPartTbl.SetColumnWide(i,20);	
	xPartTbl.SetColumnWide(5,30);	//底脚螺栓根开
	xPartTbl.SetColumnWide(6,40);	//底脚螺栓直径及个数
	//定义标题行
	for(i=1;i<7;i++)//水平合并第一行中的所有表格单元
		xPartTbl.GetGridAt(0,i)->m_bHGridVirtual = TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(0,0)->data.SetGridString("Tower Foot Distance BOM");
#else
	xPartTbl.GetGridAt(0,0)->data.SetGridString("铁塔根开表");
#endif
	xPartTbl.GetGridAt(0,0)->feature = 1;	//中央对称MC
	//定义--呼高表格单元
	xPartTbl.GetGridAt(2,0)->m_bVGridVirtual = TRUE;//坚直合并单元格
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,0)->data.SetGridString("Module");
#else
	xPartTbl.GetGridAt(1,0)->data.SetGridString("呼高");
#endif
	xPartTbl.GetGridAt(1,0)->feature = 1;	//中央对称MC
	//定义--铁塔根开表格单元
	xPartTbl.GetGridAt(1,2)->m_bHGridVirtual = TRUE;//水平合并单元格
	xPartTbl.GetGridAt(1,1)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,1)->data.SetGridString("Tower Foot Distance 2Lt");
#else
	xPartTbl.GetGridAt(1,1)->data.SetGridString("铁塔根开 2Lt");
#endif
	//定义--基础根开表格单元
	xPartTbl.GetGridAt(1,4)->m_bHGridVirtual = TRUE;//水平合并单元格
	xPartTbl.GetGridAt(1,3)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,3)->data.SetGridString("Base Foot Distance 2Lj");
#else
	xPartTbl.GetGridAt(1,3)->data.SetGridString("基础根开 2Lj");
#endif
	//定义--底脚螺栓根开表格单元
	xPartTbl.GetGridAt(2,5)->m_bVGridVirtual = TRUE;//竖直合并单元格
	xPartTbl.GetGridAt(1,5)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,5)->data.SetGridString("Foot Distance Of Foot Bolt\\PLD");
#else
	xPartTbl.GetGridAt(1,5)->data.SetGridString("底脚螺栓根开\\PLD");
#endif
	//定义--底脚螺栓直径及个数表格单元
	xPartTbl.GetGridAt(2,6)->m_bVGridVirtual = TRUE;//竖直合并单元格
	xPartTbl.GetGridAt(1,6)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	xPartTbl.GetGridAt(1,6)->data.SetGridString("Foot Bolt Diameter And Count");
	//定义--正-侧面表格单元
	xPartTbl.GetGridAt(2,1)->data.SetGridString("Front");
	xPartTbl.GetGridAt(2,1)->feature=1;
	xPartTbl.GetGridAt(2,2)->data.SetGridString("Flank");
	xPartTbl.GetGridAt(2,2)->feature=1;
	xPartTbl.GetGridAt(2,3)->data.SetGridString("Front");
	xPartTbl.GetGridAt(2,3)->feature=1;
	xPartTbl.GetGridAt(2,4)->data.SetGridString("Flank");
#else
	xPartTbl.GetGridAt(1,6)->data.SetGridString("底脚螺栓直径及个数");
	//定义--正-侧面表格单元
	xPartTbl.GetGridAt(2,1)->data.SetGridString("正面");
	xPartTbl.GetGridAt(2,1)->feature=1;
	xPartTbl.GetGridAt(2,2)->data.SetGridString("侧面");
	xPartTbl.GetGridAt(2,2)->feature=1;
	xPartTbl.GetGridAt(2,3)->data.SetGridString("正面");
	xPartTbl.GetGridAt(2,3)->feature=1;
	xPartTbl.GetGridAt(2,4)->data.SetGridString("侧面");
#endif
	xPartTbl.GetGridAt(2,4)->feature=1;
	for(i=4;i<3+nBodyN;i++)
	{
		xPartTbl.GetGridAt(i,5)->m_bVGridVirtual = TRUE;//竖直合并单元格
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
//准备表格数据
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
		PrepareSumMatTable(xFormatTbl1);	//材料汇总表
	else if(m_eBOMType==BOM_GENKAI)
		PrepareGenKaiTable(xFormatTbl1);	//铁塔根开表
	else if(m_eBOMType==BOM_SEGPART)
	{	
		if(m_bAutoNewTable)
		{
			CFormatTbl xFormatTbl;
			if(!PrepareSegPartTable(xFormatTbl,nSegI))//分段构件明细表
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
			{	//分段构件明细表需要分页绘制	
				xFormatTbl1.CopyFrom(&xFormatTbl,i-1,nRow-1);
				if(m_bHasTableHead)	//表头共3行
					xFormatTbl2.CopyFrom(&xFormatTbl,3,i-2);
				else 
					xFormatTbl2.CopyFrom(&xFormatTbl,0,i-2);
			}
			else 
				xFormatTbl1.CopyFrom(&xFormatTbl);
		}
		else if(!PrepareSegPartTable(xFormatTbl1,nSegI))	//分段构件明细表
			return FALSE;
	}
	else if(m_eBOMType==BOM_SEGBOLT)
		PrepareSegBoltTable(xFormatTbl1,nSegI);		//分段螺栓明细表
	else if(m_eBOMType==BOM_GUIGE)
	{
		if (SpecList.GetNodeNum()<=0)
			PreparePartSpecList(SpecList);		//简化的规格列表(司令图标注杆件规格时使用)
		PreparePartSpecBriefTbl(SpecList,xFormatTbl1,m_nBriefGuigeN);	//杆件规格简化表样式
	}

	else 
		return FALSE;
	return TRUE;
}

//生成命令链表
BOOL CDrawingBOM::CreateCmdList(BOOL bInit/*=TRUE*/)
{
	cmdList.Empty();
	
	if(m_eBOMType!=BOM_GUIGE)
	{	//初始化时暂不准备简化规格表数据,否则会与第一次生成司令图重复弹出该对话框 wjh-2013.2.27
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
	{	//需要分页显示表格时的第二页
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
	//更新表格初始位置
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
		m_bAutoNewTable=pDrawingBOM->m_bAutoNewTable;	//表格过长时自动生成新的表格
		m_bHasTableHead=pDrawingBOM->m_bHasTableHead;	//新表格是否带表头
		xFormatTbl1.CopyFrom(&pDrawingBOM->xFormatTbl1);
		xFormatTbl2.CopyFrom(&pDrawingBOM->xFormatTbl2);
		m_nBriefGuigeN=pDrawingBOM->m_nBriefGuigeN;		//简化规格数，杆件简化表格表使用 wht 11-06-15
	}
}

//视图绘图区域
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
BOOL CLineView::m_bUsePolyLineSumMap=FALSE;	//使用多段线绘制总图
BOOL CLineView::m_bUsePolyLineChiefMap=FALSE;//使用多段线绘制司令图
CLineView::CLineView(int id/*=0*/,IDrawing* pDrawing/*=NULL*/) : CDrawingComponent(id,pDrawing)
{
	m_eClassType=DRAW_COM_LINEVIEW;	//视图类型
	m_fScale=1.0/sys.general_set.LineMapScale;
	m_bAnalysisMode=FALSE;
	m_bDimNodeNo=TRUE;
	m_bDimPoleGuige=TRUE;
	m_cCurrSegType=0;
	TOPEST_Z=TOPEST_TRUNK_Z=LOWEST_WIREPOINT_z=0;
}

CLineView::~CLineView()
{
	
}

void CLineView::GetAjointSegInfo()
{
	CLDSLinePart* pRod;
	m_cCurrSegType=0;
	SEGI nSegI=ParentDrawing()->m_nSegI;
	if(m_pModule&&m_pModule->handle!=ParentDrawing()->m_hActiveModule&&nSegI.iSeg<=0)
	{	//非激活呼高或分段单线图
		//查找最高节点中X值最小最大的节点
		CMinDouble highestZ;
		for(CLDSNode *pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
			highestZ.Update(pNode->xOriginalPos.z,pNode);
		for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->Layer(0)=='T'||!m_pModule->IsSonPart(pRod))
				continue;
			double minZ=min(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z);
			double maxZ =max(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z);
			if(fabs(maxZ-highestZ.number)<1&&maxZ-minZ>100)
			{	//找到的第一根低端点位于当前呼高专属本体顶部Z平面，且高点至少比低点高100mm（避免横隔面分段）
				ParentDrawing()->m_nAdjoinSegI=pRod->iSeg;
				break;
			}
		}
		return;
	}

	char cPrevRodPosLayer=0;
	int nRodHits=1,nPrevRodHits=0;
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->iSeg!=nSegI)
			continue;
		if(m_cCurrSegType==0)
			m_cCurrSegType=pRod->Layer(0);
		else if(m_cCurrSegType==pRod->Layer(0))
			nRodHits++;
		else if(nRodHits>nPrevRodHits)
		{
			cPrevRodPosLayer=m_cCurrSegType;
			nPrevRodHits=nRodHits;
			nRodHits=1;
			m_cCurrSegType=pRod->Layer(0);
		}
		if(nRodHits>10)
			break;
	}
	if(nRodHits<nPrevRodHits)
		m_cCurrSegType=cPrevRodPosLayer;	//cCurrRodPosLayer此时代表了当前段类型'T','S','L'
	CUnifyModelLayer::SLOPE *pTopSlope=trunk.listSlopes.GetTail();
	if(pTopSlope==NULL)
	{	
		if(trunk.RecognizeSlopes())
			pTopSlope=trunk.listSlopes.GetTail();
	}
	if(pTopSlope==NULL)
		return ;
	GEPOINT xSlopeTopTip=pTopSlope->top;
	GEPOINT xSlopeBtmTip=pTopSlope->bottom;
	CPtInLineCheck check(xSlopeTopTip,xSlopeBtmTip);
	CMaxDouble maxPosZ;
	CMinDouble minPosZ;
	SEGI adjoinArmSegI;
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->iSeg!=nSegI)
			continue;
		if(m_cCurrSegType=='T')
		{	//当前为塔头段（包括横担）
			if(pRod->pStart&&pRod->pStart->Layer(0)=='S')
			{
				bool bOnLine=false;
				GEPOINT quad1pos=pRod->pStart->xOriginalPos;
				quad1pos.x=fabs(quad1pos.x);
				quad1pos.y=fabs(quad1pos.y);
				double lenpos_coef=check.CheckPointEx(quad1pos,&bOnLine,1);
				if(bOnLine)
				{
					maxPosZ.Update(quad1pos.z);
					minPosZ.Update(quad1pos.z);
				}
			}
			else if(pRod->pStart&&pRod->pStart->iSeg!=nSegI)
				adjoinArmSegI=pRod->pStart->iSeg;
			if(pRod->pEnd&&pRod->pEnd->Layer(0)=='S')
			{
				bool bOnLine=false;
				GEPOINT quad1pos=pRod->pEnd->xOriginalPos;
				quad1pos.x=fabs(quad1pos.x);
				quad1pos.y=fabs(quad1pos.y);
				double lenpos_coef=check.CheckPointEx(quad1pos,&bOnLine,1);
				if(bOnLine)
				{
					maxPosZ.Update(quad1pos.z);
					minPosZ.Update(quad1pos.z);
				}
			}
			else if(pRod->pEnd&&pRod->pEnd->iSeg!=nSegI)
				adjoinArmSegI=pRod->pEnd->iSeg;
		}
		else	//塔身或接腿段，需要查找上接塔身段号
		{
			if(pRod->pStart&&pRod->pStart->Layer(0)!='T')
			{
				maxPosZ.Update(pRod->pStart->xOriginalPos.z,pRod->pStart);
				minPosZ.Update(pRod->pStart->xOriginalPos.z,pRod->pStart);
			}
			if(pRod->pEnd&&pRod->pEnd->Layer(0)!='T')
			{
				maxPosZ.Update(pRod->pEnd->xOriginalPos.z,pRod->pEnd);
				minPosZ.Update(pRod->pEnd->xOriginalPos.z,pRod->pEnd);
			}
		}
	}
	if(m_cCurrSegType!='T')
	{	//塔身或接腿段
		CLDSLinePart* pSegPickRod=(CLDSLinePart*)minPosZ.m_pRelaObj;	//当前段中随机拾取的一根顶部杆件
		for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			if(pRod->iSeg==nSegI||pRod->Layer(0)=='T')
				continue;
			else if(pRod->pStart==NULL||pRod->pEnd==NULL)
				continue;
			double maxz=max(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z);
			if(fabs(maxz-minPosZ)>1)
				continue;	//与当前段没有几何位置连接关系
			if(!pRod->ModuleTogetherWith(pSegPickRod))
				continue;	//不共存于同一个塔高实例中
			ParentDrawing()->m_nAdjoinSegI=pRod->iSeg;
			break;
		}
	}
	else if(!maxPosZ.IsInited())
		this->ParentDrawing()->m_nAdjoinSegI=adjoinArmSegI;
	else if(fabs(minPosZ.number-maxPosZ.number)<1)
		this->ParentDrawing()->m_nAdjoinSegI.iSeg=0;	//塔头(如酒杯，猫头)
	else
	{
		for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			if(pRod->pStart==NULL||pRod->pEnd==NULL)
				continue;
			if(pRod->Layer(0)!='S')
				continue;
			double minz=min(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z);
			double maxz=max(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z);
			if(maxz<minPosZ.number+100||minz>maxPosZ.number-100)
				continue;//至少能保证与塔身段有100mm重叠区，才能认为与该当身段毗邻
			this->ParentDrawing()->m_nAdjoinSegI.iSeg=pRod->iSeg;	//塔头(如酒杯，猫头)
			break;
		}
	}
}
//获取需要绘制的对象集合
void CLineView::GetDrawingObjectSet(int bInitDCS)
{
	if(ParentDrawing()==NULL||m_pAttachView==NULL)
		return;
	partSet.Empty();
	nodeSet.Empty();
	SEGI nSegI=ParentDrawing()->m_nSegI;
	GetAjointSegInfo();
	SEGI adjoinSegI=ParentDrawing()->m_nAdjoinSegI;
	LINEPARTSET sum_partset;
	NODESET sum_nodeset;
	if(m_pModule==NULL&&(nSegI.iSeg>=0||nSegI.iSeg==-1))
	{	//司令图、结构图需要生成定制视图 wht 11-06-16
		if(m_pAttachView->m_iViewType==0)
		{
			for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
			{
				if(m_pAttachView->filter.IsValidLayer(pPart->layer())&&pPart->IsLinePart())
				{
					if( (pPart->iSeg.iSeg==nSegI.iSeg||nSegI.iSeg<0)||
						(nSegI.iSeg>0&&pPart->iSeg.iSeg==adjoinSegI.iSeg&&adjoinSegI!=0&&m_cCurrSegType=='T'))
						partSet.append(pPart);	//nSegI.iSeg<0表示司令图(-1)或总图(-2)，此时不需要进行段号过滤 wjh-2014.6.17
				}
			}
			for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{
				if(m_pAttachView->filter.IsValidLayer(pNode->layer())&&(pNode->iSeg.iSeg==nSegI.iSeg||nSegI.iSeg<0))
					nodeSet.append(pNode);
			}
		}
		else if(m_pAttachView->m_iViewType==1)
		{	//定制视图不用进行段号判断 wht 10-11-23
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
	{	//总图、司令图
		m_pModule->RefreshModule(sum_nodeset,sum_partset);
		for(CLDSNode *pNode=sum_nodeset.GetFirst();pNode;pNode=sum_nodeset.GetNext())
		{
			if(m_pModule->handle!=ParentDrawing()->m_hActiveModule
				&&!pNode->cfgword.IsHasBodyNoOnly(m_pModule->GetBodyNo()))//选择仅属于该模型的节点
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
			{	//应优先使用杆件单线计算构件归属象限，使用实体位置容易误判 wht 16-05-04
				f3dPoint lineStart=pLinePart->pStart?pLinePart->pStart->Position(false):pLinePart->Start();
				f3dPoint lineEnd=pLinePart->pEnd?pLinePart->pEnd->Position(false):pLinePart->End();
				char cPos=CalPoleQuad(lineStart,lineEnd);
				if (pLinePart->layer(0)=='L'&&pLinePart->layer(1)=='F'&&cPos=='1')
					continue;	//不绘制总图司令图的第一象限塔腿的辅材
				if (pLinePart->layer(0)=='L'&&pLinePart->layer(1)=='Z'&&cPos=='1'&&pLowestLeg!=NULL&&!pLinePart->cfgword.IsEqual(pLowestLeg->cfgword))
					continue;	//只绘制总图司令图的第一象限塔腿的最长主材
				if (pLinePart->layer(0)=='L'&&cPos!='1'&&pLowestLeg!=NULL&&!pLinePart->cfgword.IsEqual(pLowestLeg->cfgword))
					continue;	//总图司令图时只绘制当前最长塔腿及对应辅材
				if(m_pModule->handle!=ParentDrawing()->m_hActiveModule&&pLinePart->layer(0)!='L')
					//&&!pLinePart->cfgword.IsEqual(CFGWORD(m_pModule->m_iBodyLegNo)))	//选择仅属于该模型的杆件
					continue;
			}
			else if(m_pModule->handle!=ParentDrawing()->m_hActiveModule
				&&!pLinePart->cfgword.IsHasBodyNoOnly(m_pModule->GetBodyNo()))	
				continue;	//nLegCount==1时通过配材号选择仅属于当前模型的杆件 wht 13-07-23
			if(nSegI>0&&nSegI.iSeg!=pLinePart->iSeg.iSeg)
				continue;
			if(m_pAttachView->m_iViewType==1||(m_pAttachView->m_iViewType==0
				&&IsMatch(pLinePart->layer(2),m_pAttachView->filter.ThirdCharSet)))
				partSet.append(pLinePart);
		}
	}
	//完善节点集合
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
			continue;	//火曲加劲角钢火曲端存在始终端节点,此处排除加劲角钢 wht 11-06-16
		if(pLinePart->pStart&&pLinePart->pStart->feature==0)
		{	//杆件的始端节点未在当前显示集合中, 需进行额外添加
			nodeSet.append(pLinePart->pStart);
			pLinePart->pStart->feature=1;
		}
		if(pLinePart->pEnd&&pLinePart->pEnd->feature==0)
		{	//杆件的终端节点未在当前显示集合中, 需进行额外添加
			nodeSet.append(pLinePart->pEnd);
			pLinePart->pEnd->feature=1;
		}
	}
	//保存节点句柄链表及构件句柄链表,保存脚本再次打开文件后使用句柄链表更新节点链表和构件链表 wht 11-07-22
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
//生成视图
void CLineView::Create()
{ 
	if(ParentDrawing()==NULL)
		return;
	//构件列表为空时重新初始化构件列表
	if(nodeSet.GetNodeNum()<=0||partSet.GetNodeNum()<=0)
		GetDrawingObjectSet(FALSE);
	DRAGSET.ClearEntSet();
	//清空构件绘制及标注状态
	ClearDrawnStates(TYPE_JOINT);
	ClearDrawnStates(TYPE_ROD);
	ClearDrawnStates(TYPE_NODE);
	CHashList<f2dLine>hashRodLine;	//为标注杆件螺栓信息或法兰信息预存储杆件两端基点的绘制位置 wht 2012-06-01
	if(m_bFirstCreate)
	{	//第一次生成，需要自动计算视图装配位置
		CalInitAssemblyPosition();
		m_bFirstCreate=FALSE;
	}
	//使用相对位置重新计算坐标系原点位置
	//CalAssemblyPosByRelativePos();
	CDrawingCommand *pCmd=NULL;
	//初始化节点is_visible属性,否则单线图中将不添加节点对应的扩展数据点 wht 11-07-27
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
	{	//绘制构件
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
	//指定视图装配位置
	f2dPoint offset=SpecifyAssemblyPosition();
	for(f2dLine *pRodLine=hashRodLine.GetFirst();pRodLine;pRodLine=hashRodLine.GetNext())
	{
		pRodLine->startPt.x+=offset.x;
		pRodLine->startPt.y+=offset.y;
		pRodLine->endPt.x+=offset.x;
		pRodLine->endPt.y+=offset.y;
	}
	//标注视图名称以及上接段号
	InsertDrawingName();
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{	//运行除绘制构件、段号标注、标注螺栓或法兰信息及标注角钢接头以外的命令
		if(pCmd->feature==0||pCmd->m_eCmdType==CMD_DIM_SEGI||
			pCmd->m_eCmdType==CMD_DIM_BOLTORFL_INFO||pCmd->m_eCmdType==CMD_DIM_ANGLEJOINT)
			continue;
		pCmd->Run();
		pCmd->feature=0;
	}
	//标注螺栓或法兰信息
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
	//标注角钢接头
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{	
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_ANGLEJOINT)
			continue;
		pCmd->Run();
		pCmd->feature=0;
	}
	//标注段号
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0) 
			continue;
		pCmd->Run(); //if(pCmd->m_eCmdType==CMD_DIM_SEGI)
		pCmd->feature=0;
	}
	//插入图元后将图元矩形区域添加到绘图矩形框数组中
	UpdateDrawingRectArray();
	CStackAcadVar stackCmdEcho("CMDECHO",0);
#ifdef _ARX_2007
	ads_command(RTSTR,L"RE",RTNONE);
#else
	ads_command(RTSTR,"RE",RTNONE);
#endif
}
//返回标注时X坐标
//iType: 0.为节间垂距 1.横担挂线点间垂直间距 2.分段间距 
//		 3.塔高 4.水平标注(塔身开口尺寸) 5.塔头(含挂线点)水平间距标注 
struct DIM_NODE_COUPLE{CLDSNodePtr pStart,pEnd;};
double CLineView::ChooseFeatPt(NODESET &nodesetSeleted,int iType,char cDrawAxisX/*='X'*/)
{
	double sizePtX = 0; //标注的X坐标位置
	if(nodeSet.GetNodeNum()<=0)
		return 0;
	CLDSNode* pNode,*pDimNode;
	CLDSNode *pLowestNode = nodeSet.GetFirst();
	CLDSNode *pHighestNode = nodeSet.GetFirst();
	if(iType==CLineView::DIM_SECTION_Z||iType==CLineView::DIM_GUAXIAN_SPACE_Z)
	{	//节间垂距
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{
			if(pNode->Position('Z',false)>pLowestNode->Position('Z',false))
				pLowestNode = pNode;
			if(iType==CLineView::DIM_SECTION_Z&&!pNode->IsUdfCoordZ())
				continue;	//不是由用户定义Ｚ坐标的节点
			else if(iType==CLineView::DIM_GUAXIAN_SPACE_Z&&(pNode->m_cPosCalType!=0||toupper(pNode->layer(2))!='Q'))
				continue;	//暂认为横担挂线点都是前面控制节点
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
	{	//塔头及塔身分段间距
		CHashSet<CLDSNodePtr> hashSegNodes;
		double min_shank_z=0;
		bool init_shank_z=false;
		CLDSNode *pShankTopNode=NULL;
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{	//查找最低最高节点
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
				{	//未考虑节点只属于上下段中之一情况，导致获取节点不是分界点　
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
		//TODO:pShankTopNode有为NULL的情况，这里暂时未处理好，以后要细查 wjh-2014.11.17
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
	{	//塔高(塔头+塔身)
		double min_shank_z=0;
		CLDSNode *pShankTopNode=NULL;
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{
			//查找最低最高节点
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
	{	//塔身坡段开口尺寸
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
				continue;	//塔身变坡口节点必须为控制节点，
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
	{	//塔头(含挂线点)水平间距标注
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{
			if(pNode->Position(cDrawAxisX,false)<EPS)
				continue;	//中轴线上的节点
			if (toupper(pNode->layer(0))=='T'&&
				((cDrawAxisX=='X'&&pNode->IsUdfCoordX())||(cDrawAxisX=='Y'&&pNode->IsUdfCoordY())))
				nodesetSeleted.append(pNode);	//塔头(含挂线点)水平间距标注
		}
		return 0;
	}
	else
		return 0;
}
//生成构件明细标注
void CLineView::CreateDimPartSpecCmdList()
{
	dimpartspec.EmptyPartSpecDim();	//清空构件明细标注列表
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
			continue;	//仅标注始终端节点非空的杆件 wht 11-06-14
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
//生成螺栓明细标注命令列表	
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
			continue;	//仅标注始终端节点非空的杆件 wht 11-06-14
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DIM_BOLTORFL_INFO;
		pCmd->InitCommandPara();
		pCmd->m_paraBoltOrFlInfo.hLinePart=pPart->handle;
		pCmd->m_paraBoltOrFlInfo.pLinePart=pLinePart;
	}
}
//生成角钢接头标注命令列表	
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
//生成命令链表
struct WIRE_LEVEL{
	DWORD iSeg;
	bool bTrunkSegment;
	struct POSZ : public ICompareClass{
		bool bWireNode;
		long ziPos;	//挂点整型化Z坐标
		long xiPos;	//挂点距离塔心轴线水平投影距离（左侧为负，右侧为正）
		CLDSNode* pZcoordNode;
		POSZ(){bWireNode=false;xiPos=ziPos=0;}
		virtual int Compare(const ICompareClass *pCompareObj)const
		{
			if(ziPos>((POSZ*)pCompareObj)->ziPos)
				return 1;
			else if(ziPos<((POSZ*)pCompareObj)->ziPos)
				return -1;
			else if(abs(xiPos)>abs(((POSZ*)pCompareObj)->xiPos))
				return 1;
			else if(abs(xiPos)<abs(((POSZ*)pCompareObj)->xiPos))
				return -1;
			else if(xiPos>((POSZ*)pCompareObj)->xiPos)
				return 1;
			else if(xiPos<((POSZ*)pCompareObj)->xiPos)
				return -1;
			else
				return 0;
		};
	};
	PRESET_ARRAY8<POSZ> places;
	WIRE_LEVEL(){bTrunkSegment=false;}
	void SetKey(DWORD key){iSeg=key;}
	void AppendPosZ(const POSZ& posz)
	{
		for(UINT i=0;i<places.Count;i++)
		{
			POSZ _posz=places.At(i);
			if(_posz.ziPos>posz.ziPos)
			{
				places.Insert(i,posz);
				return;
			}
		}
		places.Append(posz);
	}
};
void CLineView::CreateVertDimensions(double BASE_ZERO_Z/*=0*/)	//生成垂直尺寸标注
{
	UINT i=0;
	CHashListEx<WIRE_LEVEL> hashWireLevels;	//挂点水平线
	SmartPartPtr pPart;
	CHashSet<CLDSNode*> hashZNodes;
	CHashListEx<WIRE_LEVEL>hashSegments;
	WIRE_LEVEL* pSegment;
	double MIN_X_OF_BOUNDARY=0;
	for(pPart=partSet.GetFirst();pPart.IsHasPtr();pPart=partSet.GetNext())
	{
		if(!pPart->IsLinePart()||pPart.pRod->pStart==NULL||pPart.pRod->pEnd==NULL)
			continue;
		if(pPart->iSeg==0)
			continue;
		if( pPart.pRod->pStart->m_cPosCalType==CLDSNode::COORD_INDEPENDENT||
			pPart.pRod->pStart->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED||
			pPart.pRod->pStart->m_cPosCalType==CLDSNode::PLANE_YZ_SPECIFIED||pPart.pRod->pStart->m_cPosCalType==CLDSNode::PLANE_XZ_SPECIFIED)
			hashZNodes.SetValue(pPart.pRod->pStart->handle,pPart.pRod->pStart);
		if( pPart.pRod->pEnd->m_cPosCalType==CLDSNode::COORD_INDEPENDENT||
			pPart.pRod->pEnd->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED||
			pPart.pRod->pEnd->m_cPosCalType==CLDSNode::PLANE_YZ_SPECIFIED||pPart.pRod->pEnd->m_cPosCalType==CLDSNode::PLANE_XZ_SPECIFIED)
			hashZNodes.SetValue(pPart.pRod->pEnd->handle,pPart.pRod->pEnd);
		pSegment=hashSegments.Add(pPart->iSeg);
		if(pSegment->places[0].ziPos==pSegment->places[1].ziPos)
		{
			pSegment->bTrunkSegment=pPart->Layer(0)=='S';	//第一根非水平杆件
			pSegment->places[0].ziPos=ftoi(min(pPart.pRod->pStart->xPreliftPos.z,pPart.pRod->pEnd->xPreliftPos.z));
			pSegment->places[1].ziPos=ftoi(max(pPart.pRod->pStart->xPreliftPos.z,pPart.pRod->pEnd->xPreliftPos.z));
		}
		else
		{
			pSegment->places[0].ziPos=min(pSegment->places[0].ziPos,ftoi(pPart.pRod->pStart->xPreliftPos.z));
			pSegment->places[0].ziPos=min(pSegment->places[0].ziPos,ftoi(pPart.pRod->pEnd->xPreliftPos.z));
			pSegment->places[1].ziPos=max(pSegment->places[1].ziPos,ftoi(pPart.pRod->pStart->xPreliftPos.z));
			pSegment->places[1].ziPos=max(pSegment->places[1].ziPos,ftoi(pPart.pRod->pEnd->xPreliftPos.z));
		}
		double fxPos=pPart.pRod->pStart->xPreliftPos.x*dcs.axis_x.x+pPart.pRod->pStart->xPreliftPos.y*dcs.axis_x.y;
		if(fxPos<MIN_X_OF_BOUNDARY)
			MIN_X_OF_BOUNDARY=fxPos;
		fxPos=pPart.pRod->pEnd->xPreliftPos.x*dcs.axis_x.x+pPart.pRod->pEnd->xPreliftPos.y*dcs.axis_x.y;
		if(fxPos<MIN_X_OF_BOUNDARY)
			MIN_X_OF_BOUNDARY=fxPos;
	}
	
	WIRE_LEVEL* pLevel;
	WIRE_LEVEL::POSZ posz;
	for(CLDSNode* pNode=hashZNodes.GetFirst();pNode;pNode=hashZNodes.GetNext())
	{
		pLevel=hashWireLevels.Add(pNode->iSeg);
		posz.pZcoordNode=pNode;
		posz.ziPos=ftoi(pNode->xPreliftPos.z);
		posz.xiPos=ftoi(pNode->xPreliftPos.x*dcs.axis_x.x+pNode->xPreliftPos.y*dcs.axis_x.y);
		if(pNode->Layer(0)!='T')
			posz.bWireNode=false;
		else
		{
			int levelrod_count=0;
			CLDSLinePart* pRod;
			for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
			{
				if(pRod->pStart==pNode&&pRod->pEnd&&fabs(pRod->pEnd->xPreliftPos.z-pRod->pStart->xPreliftPos.z)<EPS)
					levelrod_count++;
				if(pRod->pEnd==pNode&&pRod->pStart&&fabs(pRod->pEnd->xPreliftPos.z-pRod->pStart->xPreliftPos.z)<EPS)
					levelrod_count++;
				if(levelrod_count>=2)
					break;
			}
			posz.bWireNode=levelrod_count>=2;	//挂点一定有至少两根水平杆件穿过
		}
		pLevel->AppendPosZ(posz);
	}
	WIRE_LEVEL::POSZ *pPosZ,*pPrevPosZ,*pBtmestPosZ;
	ARRAY_LIST<WIRE_LEVEL::POSZ>arrayLeftVertIntervals(0,10);
	for(pLevel=hashWireLevels.GetFirst();pLevel;pLevel=hashWireLevels.GetNext())
	{
		pPrevPosZ=NULL;
		for(i=0;i<pLevel->places.Count;i++)
		{
			pPosZ=&pLevel->places.At(i);
			if(pPosZ->xiPos>0)
				continue;	//右侧节点
			if(pPrevPosZ==NULL||pPrevPosZ->ziPos!=pPosZ->ziPos)
				pPrevPosZ=arrayLeftVertIntervals.append(*pPosZ);
			else if(pPrevPosZ->ziPos==pPosZ->ziPos&&!pPrevPosZ->bWireNode&&pPosZ->bWireNode)
				*pPrevPosZ=*pPosZ;
			else if(pPrevPosZ->ziPos==pPosZ->ziPos)
			{
				if(pPosZ->xiPos<pPrevPosZ->xiPos)
					*pPrevPosZ=*pPosZ;	//找更靠左侧的节点作为尺寸线标定点
				else
					continue;
			}
			else
				continue;
		}
	}
	CQuickSort<WIRE_LEVEL::POSZ>::QuickSortClassic(arrayLeftVertIntervals.m_pData,arrayLeftVertIntervals.Size());
	CXhSimpleList<WIRE_LEVEL::POSZ> listVertIntervalNodes;
	CXhSimpleList<WIRE_LEVEL::POSZ> listVertSegmentNodes;
	CXhSimpleList<WIRE_LEVEL::POSZ> listVertWireNodes;
	//1.提取Z向节间垂高尺寸标注点
	double SPACE=2*sys.fonts.summary.fDimTextSize/m_fScale;//200;
	CDrawingCommand *pCommand=cmdList.append();
	pCommand->m_pDrawingCom=this;
	pCommand->m_eCmdType=CMD_DIM_MODELSIZE;
	pCommand->InitCommandPara();
	pCommand->m_paraDimModelSize.iDimType = DIM_SECTION_Z;
	pCommand->m_paraDimModelSize.fMinX=MIN_X_OF_BOUNDARY;
	pCommand->m_paraDimModelSize.fSpace=SPACE;
	pCommand->m_paraDimModelSize.fDimDistance=1*SPACE;
	pPrevPosZ=NULL;
	for(i=0;i<arrayLeftVertIntervals.Size();i++)
	{
		pPosZ=&arrayLeftVertIntervals[i];
		if(pPosZ->ziPos<BASE_ZERO_Z)
			continue;
		if(pPrevPosZ==NULL)
			pPrevPosZ=listVertIntervalNodes.Append(*pPosZ);
		else if(pPrevPosZ->ziPos<pPosZ->ziPos)
			pPrevPosZ=listVertIntervalNodes.Append(*pPosZ);
		else if(pPrevPosZ->xiPos>pPosZ->xiPos)
			*pPrevPosZ=*pPosZ;
		else
			continue;
		//标注Z向节间垂高尺寸标注
		pCommand->m_paraDimModelSize.AppendDimNode(pPrevPosZ->pZcoordNode);
	}
	//提取Z向分段垂高尺寸标注点
	for(i=0;i<arrayLeftVertIntervals.Size();i++)
	{
		pPosZ=&arrayLeftVertIntervals[i];
		if(pPosZ->ziPos<BASE_ZERO_Z)
		{
			if(pPrevPosZ==NULL)
				pPrevPosZ=listVertSegmentNodes.Append(*pPosZ);
			else if(pPrevPosZ->ziPos<pPosZ->ziPos)
				pPrevPosZ=listVertSegmentNodes.Append(*pPosZ);
			else if(pPrevPosZ->xiPos>pPosZ->xiPos)
				*pPrevPosZ=*pPosZ;
			continue;
		}
		else
			break;
	}
	long ziCurrScalar=ftoi(BASE_ZERO_Z);
	long ziCurrScalarIndex=i;
	while(true)
	{
		//根据之前找到的段间分界点查找标注节点
		pPrevPosZ=NULL;
		for(i=ziCurrScalarIndex;i<arrayLeftVertIntervals.Size();i++)
		{
			pPosZ=&arrayLeftVertIntervals[i];
			if(pPosZ->ziPos==ziCurrScalar&&pPrevPosZ==NULL)
				pPrevPosZ=listVertSegmentNodes.Append(*pPosZ);
			else if(pPosZ->ziPos==ziCurrScalar&&pPosZ->xiPos<pPrevPosZ->xiPos)
				*pPrevPosZ=*pPosZ;
			else if(pPosZ->ziPos>ziCurrScalar)
				break;
		}
		ziCurrScalarIndex=i;
		long ziBtmOfNextSegment=ziCurrScalar;
		for(pSegment=hashSegments.GetFirst();pSegment;pSegment=hashSegments.GetNext())
		{
			if(pSegment->places[0].ziPos<=ziCurrScalar&&pSegment->places[1].ziPos>ziBtmOfNextSegment)
				ziBtmOfNextSegment=pSegment->places[1].ziPos;
		}
		if(ziBtmOfNextSegment!=ziCurrScalar)
			ziCurrScalar=ziBtmOfNextSegment;
		else
			break;
	}
	//标注Z向分段垂高尺寸标注
	pCommand=cmdList.append();
	pCommand->m_pDrawingCom=this;
	pCommand->m_eCmdType=CMD_DIM_MODELSIZE;
	pCommand->InitCommandPara();
	pCommand->m_paraDimModelSize.iDimType = DIM_SEG_SPACE_Z;
	pCommand->m_paraDimModelSize.fMinX=MIN_X_OF_BOUNDARY;
	pCommand->m_paraDimModelSize.fSpace=100;
	pCommand->m_paraDimModelSize.fDimDistance=2*SPACE;
	for(pPosZ=listVertSegmentNodes.EnumObjectFirst();pPosZ;pPosZ=listVertSegmentNodes.EnumObjectNext())
	{
		pCommand->m_paraDimModelSize.AppendDimNode(pPosZ->pZcoordNode);
		//pCommand->m_paraDimModelSize.fDimDistance = 1000*(nVerticalDimCount+1);	
		if(pPosZ->xiPos<pCommand->m_paraDimModelSize.fMinX)
			pCommand->m_paraDimModelSize.fMinX = pPosZ->xiPos;
		pCommand->m_paraDimModelSize.fSpace = 100;
	}
	//提取Z向挂点间垂高尺寸标注点
	pPrevPosZ=NULL;
	for(i=0;i<arrayLeftVertIntervals.Size();i++)
	{
		pPosZ=&arrayLeftVertIntervals[i];
		if(!pPosZ->bWireNode)
			continue;
		if(pPrevPosZ==NULL)
			pPrevPosZ=listVertWireNodes.Append(*pPosZ);
		else if(pPrevPosZ->pZcoordNode->iSeg==pPosZ->pZcoordNode->iSeg&&pPosZ->ziPos-pPrevPosZ->ziPos<2000)
			*pPrevPosZ=*pPosZ;	//同一段内出现两个以上挂点时，如果垂高差在2m之内，则只标低点
		else if(pPosZ->ziPos-pPrevPosZ->ziPos>2000)
			pPrevPosZ=listVertWireNodes.Append(*pPosZ);
	}
	if(arrayLeftVertIntervals.Size()>0)
	{
		pBtmestPosZ=&arrayLeftVertIntervals[arrayLeftVertIntervals.Size()-1];
		listVertWireNodes.Append(*pBtmestPosZ);
		pCommand=cmdList.append();
		pCommand->m_pDrawingCom=this;
		pCommand->m_eCmdType=CMD_DIM_MODELSIZE;
		pCommand->InitCommandPara();
		pCommand->m_paraDimModelSize.fMinX=MIN_X_OF_BOUNDARY;
		pCommand->m_paraDimModelSize.iDimType = DIM_GUAXIAN_SPACE_Z;
		pCommand->m_paraDimModelSize.fSpace=100;
		pCommand->m_paraDimModelSize.fDimDistance=3*SPACE;
		for(pPosZ=listVertWireNodes.EnumObjectFirst();pPosZ;pPosZ=listVertWireNodes.EnumObjectNext())
		{
			pCommand->m_paraDimModelSize.AppendDimNode(pPosZ->pZcoordNode);
			//pCommand->m_paraDimModelSize.fDimDistance = 1000*(nVerticalDimCount+1);	
			if(pPosZ->xiPos<pCommand->m_paraDimModelSize.fMinX)
				pCommand->m_paraDimModelSize.fMinX = pPosZ->xiPos;
			pCommand->m_paraDimModelSize.fSpace = 100;
		}
		//提取全高尺寸标注点
		pCommand=cmdList.append();
		pCommand->m_pDrawingCom=this;
		pCommand->m_eCmdType=CMD_DIM_MODELSIZE;
		pCommand->InitCommandPara();
		pCommand->m_paraDimModelSize.fMinX=MIN_X_OF_BOUNDARY;
		pCommand->m_paraDimModelSize.iDimType = DIM_TOWER_HEIGHT;
		pCommand->m_paraDimModelSize.fSpace=100;
		pCommand->m_paraDimModelSize.fDimDistance=4*SPACE;
		WIRE_LEVEL::POSZ* pTopPosZ=&arrayLeftVertIntervals[0];
		pCommand->m_paraDimModelSize.AppendDimNode(pTopPosZ->pZcoordNode);
		pCommand->m_paraDimModelSize.AppendDimNode(pBtmestPosZ->pZcoordNode);
	}
}
void CLineView::CreateHorzDimensions()	//生成水平尺寸标注
{
	//1.添加塔身开口尺寸标注
	CUnifyModelLayer::SLOPE *pSlope,*pTopSlope=trunk.listSlopes.GetTail();
	//添加塔身顶部开口尺寸标注
	GEPOINT xSlopeTopTip=pTopSlope->top;
	CLDSNode* pNode;
	WIRE_LEVEL::POSZ posz,*pPosZ;
	ARRAY_LIST<WIRE_LEVEL::POSZ> keySectNodes(0,8);	//塔身关键控制开口节点
	for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
	{
		if(pNode->Layer(0)=='T')
			continue;	//不处理塔头节点
		if(fabs(fabs(pNode->xOriginalPos.x)-pTopSlope->top.x)<EPS&&fabs(fabs(pNode->xOriginalPos.y)-pTopSlope->top.y)<EPS)
		{	//该节点是顶部坡身上开口节点 wjh-2019.12.13
			posz.ziPos=ftoi(pNode->xOriginalPos.z);
			posz.xiPos=ftoi(pNode->xOriginalPos.x*dcs.axis_x.x+pNode->xOriginalPos.y*dcs.axis_x.y);
			posz.pZcoordNode=pNode;
			keySectNodes.append(posz);
			continue;
		}
		for(pSlope=trunk.listSlopes.GetFirst();pSlope;pSlope=trunk.listSlopes.GetNext())
		{
			if(fabs(fabs(pNode->xOriginalPos.x)-pSlope->bottom.x)<EPS&&fabs(fabs(pNode->xOriginalPos.y)-pSlope->bottom.y)<EPS)
			{
				posz.ziPos=ftoi(pNode->xOriginalPos.z);
				posz.xiPos=ftoi(pNode->xOriginalPos.x*dcs.axis_x.x+pNode->xOriginalPos.y*dcs.axis_x.y);
				posz.pZcoordNode=pNode;
				keySectNodes.append(posz);
			}
		}
	}
	CQuickSort<WIRE_LEVEL::POSZ>::QuickSortClassic(keySectNodes.m_pData,keySectNodes.Size());
	CDrawingCommand *pCommand;
	double flowDirect[2]={1.0,0};
	double SPACE=2*sys.fonts.summary.fDimTextSize/m_fScale;//200;
	char ciHoriAxis=fabs(dcs.axis_x.x)>fabs(dcs.axis_x.y)?'X':'Y';
	long i,count=keySectNodes.Size();	//TODO:存在问题 wjh-2018.6.10
	for(i=0;i<count-1;i++)
	{
		if(keySectNodes[i].ziPos!=keySectNodes[i+1].ziPos||keySectNodes[i].xiPos>=keySectNodes[i+1].xiPos)
			continue;
		DIMSIZE_NODE* pDimSizeNode=this->hashNodeDimensions.GetValue(keySectNodes[i].pZcoordNode->handle);
		if(pDimSizeNode&&(ciHoriAxis=='X'&&!pDimSizeNode->blNeedAddDimCoordX)||(ciHoriAxis=='Y'&&!pDimSizeNode->blNeedAddDimCoordY))
			continue;	//前端智能标注已经处理了该节点的尺寸标注项
		//合法配对塔身开口节点
		pCommand=cmdList.append();
		pCommand->m_pDrawingCom=this;
		pCommand->InitCommandPara(CMD_DIM_MODELSIZE);
		pCommand->m_paraDimModelSize.iDimType = DIM_SHANK_SPACE_XY;
		pCommand->m_paraDimModelSize.fDimDistance=4*SPACE;
		GEPOINT dimpos=(keySectNodes[i].pZcoordNode->xOriginalPos+keySectNodes[i+1].pZcoordNode->xOriginalPos)*0.5;
		GEPOINT dimpoint=preview->TransPToOcs(dimpos);
		long dist=keySectNodes[i+1].xiPos-keySectNodes[i].xiPos;
		IPreview::RGN_RECT dimrc;
		dimrc.h=ftoi(ceil(sys.fonts.summary.fDimTextSize))+1;
		dimrc.w=dist>10000?ftoi(dimrc.h*3.5):ftoi(dimrc.h*2.8);	//3.5=5*0.7,2.8=4*0.7
		dimrc.ox=ftoi(dimpoint.x)-dimrc.w/2;
		dimrc.oy=ftoi(dimpoint.y);
		IPreview::SEARCH search(flowDirect,IPreview::SEARCH::DIRECT_Y|IPreview::SEARCH::DIRECT_Y_INVERSE,0,dimrc.h*5,0,1);
		preview->SearchBlankRegion(&dimrc,&search);
		if(dcs.axis_y.z>0)
		{
			pCommand->m_paraDimModelSize.fSpace=SPACE*0.2;	//与节点间隙设为0.2个字高
			pCommand->m_paraDimModelSize.fDimDistance= (dimrc.oy-dimpoint.y)/preview->ScaleOfView2Model();
		}
		else
		{
			pCommand->m_paraDimModelSize.fSpace=-SPACE*0.2;	//与节点间隙设为0.2个字高
			pCommand->m_paraDimModelSize.fDimDistance=-(dimrc.oy-dimpoint.y)/preview->ScaleOfView2Model();
		}
		pCommand->m_paraDimModelSize.AppendDimNode(keySectNodes[i].pZcoordNode);
		pCommand->m_paraDimModelSize.AppendDimNode(keySectNodes[i+1].pZcoordNode);
	}
	//2.添加横担挂点或水平节间尺寸标注
	WIRE_LEVEL* pLevel;
	CHashListEx<WIRE_LEVEL> hashArmWireNodes;
	for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
	{
		if( pNode->Layer(0)!='T')
			continue;
		bool bNeedHorzDimNode=false;
		if( pNode->m_cPosCalType==0||
		   (ciHoriAxis=='X'&&pNode->IsUdfCoordX())||(ciHoriAxis=='Y'&&pNode->IsUdfCoordY()))
			bNeedHorzDimNode=true;	//横担上水平横向位置标定点
		if(!bNeedHorzDimNode)
			continue;
		pLevel=hashArmWireNodes.Add(pNode->iSeg);
		int levelrods=0,linkrods=0;
		for(CLDSLinePart* pRod=Ta.EnumRodFirst();levelrods<2&&pRod;pRod=Ta.EnumRodNext())
		{
			if(pRod->pStart==NULL||pRod->pEnd==NULL)
				continue;
			if(pRod->pStart!=pNode&&pRod->pEnd!=pNode)
				continue;
			linkrods++;
			if(fabs(pRod->pEnd->xPreliftPos.z-pRod->pStart->xPreliftPos.z)<EPS)
				levelrods++;
		}
		if(linkrods==0)
			continue;
		posz.bWireNode=levelrods>=2;
		posz.ziPos=ftoi(pNode->xPreliftPos.z);
		posz.xiPos=ftoi(pNode->xPreliftPos.x*dcs.axis_x.x+pNode->xPreliftPos.y*dcs.axis_x.y);
		posz.pZcoordNode=pNode;
		bool processed=false;
		for(i=0;i<(long)pLevel->places.Count;i++)
		{
			if(pLevel->places.At(i).xiPos==posz.xiPos&&abs(pLevel->places.At(i).ziPos-posz.ziPos)<=5000)
			{	//同一分段内的相同横向位置，上下节点垂直偏差在2000之内时，取下不取上
				processed=true;
				WIRE_LEVEL::POSZ* pLegacyPosZ=&pLevel->places.At(i);
				if( (posz.bWireNode&&!pLegacyPosZ->bWireNode)||
					(posz.bWireNode==pLegacyPosZ->bWireNode&&posz.ziPos>pLevel->places.At(i).ziPos))
				{
					pLevel->places.Set(i,posz);
					break;
				}
				else
					continue;
			}
		}
		if(!processed)
			pLevel->places.Append(posz);
	}
	CXhSimpleList<WIRE_LEVEL::POSZ> listHorzNodes;
	for(pLevel=hashArmWireNodes.GetFirst();pLevel;pLevel=hashArmWireNodes.GetNext())
	{
		for(i=0;i<(long)pLevel->places.Count;i++)
			listHorzNodes.Append(pLevel->places.At(i));
	}
	SIMPLE_STACK<PVOID> stack;
	listHorzNodes.AttachStack(&stack);
	ARRAY_LIST<WIRE_LEVEL::POSZ> arrayLevelPlaces(0,8);
	for(WIRE_LEVEL::POSZ* pCurrPosZ=listHorzNodes.EnumObjectFirst();pCurrPosZ;pCurrPosZ=listHorzNodes.EnumObjectNext())
	{
		arrayLevelPlaces.SetSize(0);
		arrayLevelPlaces.append(*pCurrPosZ);
		BOOL pushed=listHorzNodes.push_stack();
		for(pPosZ=listHorzNodes.EnumObjectNext();pPosZ;pPosZ=listHorzNodes.EnumObjectNext())
		{
			if(pPosZ->ziPos==pCurrPosZ->ziPos&&(pCurrPosZ->xiPos*pPosZ->xiPos>0))
			{
				arrayLevelPlaces.append(*pPosZ);
				listHorzNodes.DeleteCursor(FALSE);
			}
		}
		listHorzNodes.pop_stack(pushed);
		CQuickSort<WIRE_LEVEL::POSZ>::QuickSortClassic(arrayLevelPlaces.m_pData,arrayLevelPlaces.Size());
		pCommand=cmdList.append();
		pCommand->m_pDrawingCom=this;
		pCommand->m_eCmdType=CMD_DIM_MODELSIZE;
		pCommand->InitCommandPara();
		pCommand->m_paraDimModelSize.iDimType = DIM_GUAXIAN_SPACE_XY;
		pCommand->m_paraDimModelSize.fSpace=SPACE*0.2;	//与节点间隙设为0.2个字高
		int xiMinPos=ftoi(arrayLevelPlaces[0].xiPos*preview->ScaleOfView2Model());
		int xiMaxPos=abs(xiMinPos);
		int ziHighPos=ftoi(arrayLevelPlaces[0].ziPos*preview->ScaleOfView2Model());
		int ziLowPos=ftoi(arrayLevelPlaces[arrayLevelPlaces.Size()-1].ziPos*preview->ScaleOfView2Model());
		if(dcs.axis_y.z<0)
		{
			ziLowPos*=-1;
			ziHighPos*=-1;
		}
		for(UINT j=0;j<arrayLevelPlaces.Size();j++)
		{
			pCommand->m_paraDimModelSize.AppendDimNode(arrayLevelPlaces[j].pZcoordNode);
			int xiCurrPos=ftoi(abs(arrayLevelPlaces[j].xiPos)*preview->ScaleOfView2Model());
			if(xiMaxPos<xiCurrPos)
				xiMaxPos=xiCurrPos;
		}
		if(xiMaxPos==0)
		{
			cmdList.DeleteCursor();
			continue;
		}
		//查找空白的标注区域
		IPreview::RGN_RECT dimrc;
		dimrc.h=ftoi(ceil(sys.fonts.summary.fDimTextSize))+1;
		dimrc.w=max(dimrc.h*5,xiMaxPos-abs(xiMinPos));
		dimrc.ox=xiMinPos<0?-xiMaxPos:xiMinPos-dimrc.h*5;
		int ziInitPos=dimrc.oy=ziLowPos;
		bool searched=false;
		IPreview::SEARCH search(flowDirect,IPreview::SEARCH::DIRECT_Y_INVERSE,0,dimrc.h*5,0,1);
		if(ziLowPos==ziHighPos)
			search.cbDualDirectMode|=IPreview::SEARCH::DIRECT_Y;
		if(!(searched=preview->SearchBlankRegion(&dimrc,&search))&&ziLowPos!=ziHighPos)
		{
			search.cbDualDirectMode=IPreview::SEARCH::DIRECT_Y;
			ziInitPos=dimrc.oy=ziHighPos;
			searched=preview->SearchBlankRegion(&dimrc,&search);
		}
		if(searched)
		{
			if(dcs.axis_y.z>0)
				pCommand->m_paraDimModelSize.fDimDistance= (dimrc.oy-ziInitPos)/preview->ScaleOfView2Model();
			else
				pCommand->m_paraDimModelSize.fDimDistance=-(dimrc.oy-ziInitPos)/preview->ScaleOfView2Model();
			if(pCommand->m_paraDimModelSize.fDimDistance<0)
				pCommand->m_paraDimModelSize.fSpace=-SPACE*0.2;	//与节点间隙设为0.2个字高
		}
	}
}
CLineView::DIMSIZE_NODE::DIMSIZE_NODE()
{
	hNode=0;
	cbDimSizeFlag=0;//一览图/总图/司令图中需要进行的尺寸标注项，0x01:pos.x;0x02;pos.y;0x04:pos.z;
	hDimCmdCoordX=hDimCmdCoordY=hDimCmdCoordZ=0;
}
bool CLineView::DIMSIZE_NODE::get_blNeedAddDimCoordX()
{
	return (cbDimSizeFlag&0x01)==0||hDimCmdCoordX>0;
}
bool CLineView::DIMSIZE_NODE::get_blNeedAddDimCoordY()
{
	return (cbDimSizeFlag&0x02)==0||hDimCmdCoordY>0;
}
bool CLineView::DIMSIZE_NODE::get_blNeedAddDimCoordZ()
{
	return (cbDimSizeFlag&0x04)==0||hDimCmdCoordZ>0;
}
void CLineView::IntelliCreateDimensions()		//根据塔头/横担类型识别的结果智能识别尺寸标注
{
	//横担一般都在最顶部坡段，所以先查找到最顶部坡段
	CUnifyModelLayer::SLOPE *pUpperSlope=trunk.listSlopes.GetFirst();
	if(pUpperSlope==NULL)
		return;
	CIntelliAgent experts(&Ta,300);
	for(CUnifyModelLayer::SLOPE* pSlope=trunk.listSlopes.GetNext();pSlope;pSlope=trunk.listSlopes.GetNext())
	{
		if(pSlope->top.z<pUpperSlope->top.z)
			pUpperSlope=pSlope;
	}
	//if(m_pIntelliAgent->GetRodCount()==0)
	//	m_pIntelliAgent->SyncModel(&Ta);
	CXhSimpleList<STDTEMPL> listArms;
	COLINEAR_TOLERANCE xTolerance(5);
	int niRecogStdTmplCount=experts.RecognizeAllStdTempl(pUpperSlope->top,pUpperSlope->bottom,&listArms,&xTolerance);
	//添加描述基准平面的节点Y向坐标标注（司令图需要，总图中不一定需要）
	//PRESET_ARRAY8<long> arrArmTempPlanes;
	CDrawingCommand *pCommand;
	double flowDirect[2]={1.0,0};
	double SPACE=2*sys.fonts.summary.fDimTextSize/m_fScale;//200;
	for(STDTEMPL* pLiveArm=listArms.EnumObjectFirst();pLiveArm;pLiveArm=listArms.EnumObjectNext())
	{
		for(STDTEMPL::DIMSIZE* pDimension=pLiveArm->listPreferDimensions.EnumObjectFirst();pDimension;
			pDimension=pLiveArm->listPreferDimensions.EnumObjectNext())
		{
			STDTEMPL::MATCHNODE* pDimStartParaNode=pLiveArm->hashMatchNodes.GetValue(pDimension->widDimStartNode);
			STDTEMPL::MATCHNODE* pDimEndParaNode=pLiveArm->hashMatchNodes.GetValue(pDimension->widDimEndNode);
			long hDimStartNode=pDimStartParaNode!=NULL?pDimStartParaNode->hRelaModelNode:0;
			long hDimEndNode=pDimEndParaNode!=NULL?pDimEndParaNode->hRelaModelNode:0;
			if(hDimStartNode==0&&hDimEndNode==0)
				continue;
			CLDSNode* pDimFromNode=hDimStartNode>0?Ta.Node.FromHandle(hDimStartNode):NULL;
			CLDSNode* pDimToNode=hDimEndNode>0?Ta.Node.FromHandle(hDimEndNode):NULL;
			if(pDimFromNode==NULL&&pDimToNode==NULL)
				continue;
			DIMSIZE_NODE* pDimSizeNode;
			if((pDimension->cbDimEffectFlag&0x01)>0&&hDimStartNode>0)
			{
				pDimSizeNode=this->hashNodeDimensions.Add(hDimStartNode);
				//ciAlongDirection: 尺寸心线起始节点，0表示：X/Y时塔身中轴线;Z时表示Z向标注基准平面
				if(pDimension->ciAlongDirection=='X')
					pDimSizeNode->cbDimSizeFlag|=0x01;
				if(pDimension->ciAlongDirection=='Y')
					pDimSizeNode->cbDimSizeFlag|=0x02;
				if(pDimension->ciAlongDirection=='Z')
					pDimSizeNode->cbDimSizeFlag|=0x04;
			}
			if((pDimension->cbDimEffectFlag&0x02)>0&&hDimEndNode>0)
			{
				pDimSizeNode=this->hashNodeDimensions.Add(hDimEndNode);
				//尺寸心线起始节点，0表示：X/Y时塔身中轴线;Z时表示Z向标注基准平面
				if(pDimension->ciAlongDirection=='X')
					pDimSizeNode->cbDimSizeFlag|=0x01;
				if(pDimension->ciAlongDirection=='Y')
					pDimSizeNode->cbDimSizeFlag|=0x02;
				if(pDimension->ciAlongDirection=='Z')
					pDimSizeNode->cbDimSizeFlag|=0x04;
			}
			//'T'：向上标注Δx;'B'：向下标注Δx;'L':向左标注Δz;'R':向右标注Δz;'t'：侧视图向上标注Δy
			//pDimension->ciDimType;
			//实际添加标注项
			pCommand=cmdList.append();
			pCommand->m_pDrawingCom=this;
			pCommand->InitCommandPara(CMD_DIM_MODELSIZE);
			if(pDimension->ciDimType=='T'||pDimension->ciDimType=='B')
				pCommand->m_paraDimModelSize.iDimType = DIM_SHANK_SPACE_XY;
			//pDimension->ciDimLevelLayer;	//尺寸线所在分层序号，0:文本标注于左侧紧邻位置不整体对齐;>0：文本标注于左侧;<0文本标注于右侧
			pCommand->m_paraDimModelSize.fDimDistance=pDimension->ciDimLevelLayer*SPACE;
			GEPOINT dimPosFrom,dimPosTo;
			dimPosFrom=pDimFromNode!=NULL?pDimFromNode->xOriginalPos:pDimToNode->xOriginalPos;
			dimPosTo=pDimToNode!=NULL?pDimToNode->xOriginalPos:pDimFromNode->xOriginalPos;
			if(pDimFromNode==NULL)
				dimPosFrom.x=dimPosFrom.y=0;
			if(pDimToNode==NULL)
				dimPosTo.x=dimPosTo.y=0;
			GEPOINT dimpos=(dimPosFrom+dimPosTo)*0.5;
			GEPOINT dimpoint=preview->TransPToOcs(dimpos);
			//long dist=keySectNodes[i+1].xiPos-keySectNodes[i].xiPos;
			long dist;
			if(pDimension->ciAlongDirection=='X')
				dist=(long)fabs(dimPosFrom.x-dimPosTo.x);
			else if(pDimension->ciAlongDirection=='Y')
				dist=(long)fabs(dimPosFrom.y-dimPosTo.y);
			else //if(pDimension->ciAlongDirection=='Z')
				dist=(long)fabs(dimPosFrom.z-dimPosTo.z);
			IPreview::RGN_RECT dimrc;
			dimrc.h=ftoi(ceil(sys.fonts.summary.fDimTextSize))+1;
			dimrc.w=dist>10000?ftoi(dimrc.h*3.5):ftoi(dimrc.h*2.8);	//3.5=5*0.7,2.8=4*0.7
			dimrc.ox=ftoi(dimpoint.x)-dimrc.w/2;
			dimrc.oy=ftoi(dimpoint.y);
			IPreview::SEARCH search(flowDirect,IPreview::SEARCH::DIRECT_Y|IPreview::SEARCH::DIRECT_Y_INVERSE,0,dimrc.h*5,0,1);
			preview->SearchBlankRegion(&dimrc,&search);
			if(dcs.axis_y.z>0)
			{
				pCommand->m_paraDimModelSize.fSpace=SPACE*0.2;	//与节点间隙设为0.2个字高
				pCommand->m_paraDimModelSize.fDimDistance= (dimrc.oy-dimpoint.y)/preview->ScaleOfView2Model();
			}
			else
			{
				pCommand->m_paraDimModelSize.fSpace=-SPACE*0.2;	//与节点间隙设为0.2个字高
				pCommand->m_paraDimModelSize.fDimDistance=-(dimrc.oy-dimpoint.y)/preview->ScaleOfView2Model();
			}
			pCommand->m_paraDimModelSize.AppendDimNode(pDimFromNode);
			pCommand->m_paraDimModelSize.AppendDimNode(pDimToNode);
		}
	}
}
void CLineView::CreateDimensions()
{
	CUnifyModelLayer::SLOPE* pTopSlope=trunk.listSlopes.GetTail();
	bool bSummaryDrawing=(m_pAttachView&&m_pAttachView->m_iViewType==0&&ParentDrawing()->m_nSegI.iSeg<0);
	//1.识别Z向垂高标注的Z基点(塔身最高点或塔头、横担中水平导地线挂点
	double BASE_ZERO_Z=TOPEST_TRUNK_Z;
	if(TOPEST_TRUNK_Z-TOPEST_Z>2000)
		BASE_ZERO_Z=TOPEST_Z;
	//IntelliCreateDimensions();
	CreateVertDimensions(BASE_ZERO_Z);
	CreateHorzDimensions();
}
void CLineView::CreateSegmentLabelDimensions(SEGI *pOnlyDrawSegI/*=NULL*/,SEGI *pAdjoinTrunkSegI/*=NULL*/)
{	//总图、司令图段号标注
	int nSegNum=0;
	ATOM_LIST<SEGI> segNoList;
	if(pOnlyDrawSegI)
	{
		segNoList.append(*pOnlyDrawSegI);
		nSegNum=1;
	}
	else if(m_pModule->handle==ParentDrawing()->m_hActiveModule)
		nSegNum=Ta.GetSegNoList(segNoList,m_pModule);
	else 
		nSegNum=Ta.GetSegNoList(segNoList,m_pModule,TRUE);
	double flowDirect[2]={1.0,0.0};
	if(nSegNum>0)
	{
		for(SEGI *pSegI=segNoList.GetFirst();pSegI;pSegI=segNoList.GetNext())
		{
			if(*pSegI<=0)
				continue;
			SmartPartPtr pPart;
			char ciSegType='S';	//塔身
			bool bLeftSideArm=true;;
			SCOPE_STRU scope;
			for(CLDSNode* pNode=nodeSet.GetFirst();pNode!=NULL;pNode=nodeSet.GetNext())
			{
				if(pNode->iSeg!=*pSegI)
					continue;
				if(pNode->Layer(0)=='T')
				{
					bool bRightSide=(pNode->xOriginalPos.x*dcs.axis_x.x+pNode->xOriginalPos.y*dcs.axis_x.y)>0;
					if(ciSegType=='S')
						ciSegType=bRightSide?'R':'L';
					else if((ciSegType=='R'&&!bRightSide)||(ciSegType=='L'&&bRightSide))
						ciSegType='T';	//同时存在左侧和右侧的节点，则只能视为头部结构
					else if(ciSegType=='T')
						break;
				}
				GEPOINT point=preview->TransPToOcs(pNode->xPreliftPos);
				scope.VerifyVertex(point);
			}
			CDrawingCommand *pCommand=cmdList.append();
			pCommand->m_pDrawingCom=this;
			pCommand->m_eCmdType=CMD_DIM_SEGI;
			pCommand->m_paraDimSegI.nSegI=*pSegI;
			//pCommand->InitCommandPara();
			pCommand->m_paraDimSegI.bSelectPos=2;
			GEPOINT basepnt((scope.fMinX+scope.fMaxX)*0.5,(scope.fMinY+scope.fMaxY)*0.5);
			IPreview::RGN_CIR dimcir;
			dimcir.r=ftoi(sys.dim_map.fSegICirD/2);
			dimcir.cx=(short)ftoi(basepnt.x);
			dimcir.cy=(short)ftoi(basepnt.y);
			bool searched=false;
			IPreview::SEARCH search;
			double width =scope.wide();
			double height=scope.high();
			if(ciSegType=='S')
				height/=2;
			if(ciSegType!='S')
				search=IPreview::SEARCH(flowDirect,IPreview::SEARCH::DIRECT_Y|IPreview::SEARCH::DIRECT_Y_INVERSE,0,height,1,1);
			else
				search=IPreview::SEARCH(flowDirect,IPreview::SEARCH::DIRECT_X|IPreview::SEARCH::DIRECT_X_INVERSE,width,0,1,1);
			if(!(searched=preview->SearchBlankRegion(&dimcir,&search)))
			{
				if(ciSegType!='S')
					search=IPreview::SEARCH(flowDirect,IPreview::SEARCH::DIRECT_X|IPreview::SEARCH::DIRECT_X_INVERSE,width,0,1,1);
				else
					search=IPreview::SEARCH(flowDirect,IPreview::SEARCH::DIRECT_Y|IPreview::SEARCH::DIRECT_Y_INVERSE,0,height,1,1);
				searched=preview->SearchBlankRegion(&dimcir,&search);
			}
			if(searched)
			{
				pCommand->m_paraDimSegI.fDimPosX=(dimcir.cx-basepnt.x)*(m_fScale/preview->ScaleOfView2Model());
				pCommand->m_paraDimSegI.fDimPosY=(dimcir.cy-basepnt.y)*(m_fScale/preview->ScaleOfView2Model());
			}
		}
	}
}
BOOL CLineView::CreateCmdList(BOOL bInit/*=TRUE*/)
{
	trunk.RecognizeSlopes();	//提取塔身坡段
	//1.获取需要绘制的构件
	GetDrawingObjectSet(bInit);
	GECS ocs(dcs.origin,dcs.axis_x,dcs.axis_y,dcs.axis_z);
	preview->SetOCS(ocs,m_fScale);
	//2.生成相应命令列表
	cmdList.Empty();
	CLDSPart *pPart=NULL;
	CMinDouble topest_z,topest_trunk_z;
	//CXhSimpleList<RODLINE> listBriefLines;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(!pPart->IsLinePart())
			continue;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;
		topest_z.Update(min(pLinePart->pStart->xPreliftPos.z,pLinePart->pEnd->xPreliftPos.z));
		if(pLinePart->Layer('0')!='T')
			topest_trunk_z.Update(min(pLinePart->pStart->xPreliftPos.z,pLinePart->pEnd->xPreliftPos.z));
		preview->AppendLine(pLinePart->pStart->xPreliftPos,pLinePart->pEnd->xPreliftPos,pLinePart->handle);
		//listBriefLines.Append(RODLINE(pLinePart->pStart->xPreliftPos,pLinePart->pEnd->xPreliftPos,pLinePart->handle));
		pLinePart->pStart->is_visible=FALSE;//设置标志位避免节点重复绘制
		pLinePart->pEnd->is_visible=FALSE;		
		CDrawingCommand *pCommand=cmdList.append();
		pCommand->m_pDrawingCom=this;
		pCommand->m_eCmdType=CMD_DRAW_LINEPART;
		pCommand->InitCommandPara();
		pCommand->m_paraLinePart.hLinePart=pPart->handle;
		pCommand->m_paraLinePart.pLinePart=pLinePart;
		pCommand->m_paraLinePart.bAdjoinRod=(pLinePart->iSeg.iSeg>0&&pLinePart->iSeg==ParentDrawing()->m_nAdjoinSegI);
		if(ParentDrawing()->m_nSegI.iSeg<0)
			pCommand->m_paraLinePart.bSumMap=TRUE;
		else //分段单线图,按角钢心线绘制 wht 11-07-27
			pCommand->m_paraLinePart.bSumMap=FALSE;	
		pCommand->m_paraLinePart.bDimByAnalysisMode=m_bAnalysisMode;
	}
	preview->FlushPreview();
	TOPEST_Z=topest_z.number;
	TOPEST_TRUNK_Z=topest_trunk_z.number;

	if(m_bAnalysisMode)	//力学分析模式即司令图
	{	//3.构件明细标注
		CreateDimPartSpecCmdList();
		//4.杆件螺栓信息或法兰信息
		CreateDimBoltOrFlInfoCmdList();
		//  角钢接头标注信息
		CreateAngleJointCmdList();
	}
	//4.尺寸标注
	int i=0;
	BOOL bSideView=FALSE;
	if(m_pAttachView&&stricmp(m_pAttachView->defaultLayer,"SPY")==0)
		bSideView=TRUE;

	if(m_pAttachView&&m_pAttachView->m_iViewType==0)
	{	//目前仅支持图层视图标注模型尺寸 wht 11-06-16
		if(ParentDrawing()->liDrwType<0)	//总图
			CreateDimensions();
		else
		{
			int nVerticalDimCount=0;
			for(i=0;i<6;i++)
			{	
				if(bSideView&&i!=4)
					continue;	//侧面视图仅标注坡段开口尺寸 wht 11-06-15
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
				{	//坡段开口按nodeSetTemp成对生成尺寸标注
					pCommand->m_paraDimModelSize.fDimDistance = 500;
					for(j=0;j+1<arrDimNodes.Size();j+=2)
					{
						pCommand->m_paraDimModelSize.AppendDimNode(arrDimNodes[j]);
						pCommand->m_paraDimModelSize.bUpDim = FALSE;
						if(arrDimNodes[j]->feature==2)
							pCommand->m_paraDimModelSize.bUpDim = TRUE;
						pCommand->m_paraDimModelSize.AppendDimNode(arrDimNodes[j+1]);
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
						pCommand->m_paraDimModelSize.AppendDimNode(pNode);
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
	}

	//5.段号标注(司令图也需要标注段号信息 wht 11-07-15)
	if(m_pModule)
		CreateSegmentLabelDimensions();
	else if(ParentDrawing()->m_nSegI.iSeg>0&&m_pAttachView&&m_pAttachView->m_iViewType==0)
		CreateSegmentLabelDimensions(&ParentDrawing()->m_nSegI);	//分段单线图段号标注
	/*{
		CDrawingCommand *pCommand=cmdList.append();
		pCommand->m_pDrawingCom=this;
		pCommand->m_eCmdType=CMD_DIM_SEGI;
		pCommand->InitCommandPara();
		pCommand->m_paraDimSegI.nSegI=ParentDrawing()->m_nSegI;
		pCommand->m_paraDimSegI.bSelectPos=TRUE;
	}*/

	if(bInit)	//6.计算视图初始装配位置
		CalInitAssemblyPosition();
	return TRUE;
}
//更新命令列表
void CLineView::UpdateCmdList(BOOL bUpdateViewDcs/*=FALSE*/)
{
	if(bUpdateViewDcs&&m_pAttachView)
		SetDCS(m_pAttachView->ucs);
	CHashList<CDrawingCommand> hashDrawPoleList;		//key:pPart->handle
	CHashList<CDrawingCommand> hashPartSpecList;		//key:pPart->handle
	CHashList<CDrawingCommand> hashDimSegIList;			//key:pPart->handle
	CDrawingCommand *pCmd=NULL;
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		CDrawingCommand *pNewCmd=NULL;
		if(pCmd->m_eCmdType==CMD_DRAW_LINEPART)		//绘制杆件
			pNewCmd=hashDrawPoleList.Add(pCmd->m_paraLinePart.hLinePart);
		else if(pCmd->m_eCmdType==CMD_DIM_PARTSPEC)	//标注构件明细
			pNewCmd=hashPartSpecList.Add(pCmd->m_paraPartSpec.hObj);
		else if(pCmd->m_eCmdType==CMD_DIM_SEGI)		//标注段号
			pNewCmd=hashDimSegIList.Add(pCmd->m_paraDimSegI.nSegI);
		else
			continue;
		pNewCmd->CopyProperty(pCmd);
	}
	//重新获得命令列表
	CreateCmdList(FALSE);
	//对比新生成的命令列表与原有命令列表,按顺序复制旧的命令参数值对应的命令中
	CDrawingCommand *pOldCmd=NULL;
	// 绘制杆件命令
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DRAW_LINEPART)
			continue;
		pOldCmd=hashDrawPoleList.GetValue(pCmd->m_paraLinePart.hLinePart);
		if(pOldCmd)
			pCmd->CopyProperty(pOldCmd);
	}
	// 标注构件明细命令
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_PARTSPEC)
			continue;
		pOldCmd=hashPartSpecList.GetValue(pCmd->m_paraPartSpec.hObj);
		if(pOldCmd)
			pCmd->CopyProperty(pOldCmd);
	}
	// 标注模型尺寸命令 直接使用新生成的命令即可
	// 标注段号命令
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
	m_eClassType=DRAW_COM_STRUVIEW;	//视图类型
	m_fScale=1.0/sys.general_set.StruMapScale.fRealToDraw;
	m_bDrawRefLine=FALSE;//sys.dim_map.PartNo.bDrawRefLine;
	m_iMirMsg=2;//sys.dim_map.PartNo.iMirMsg;
}

CStruView::~CStruView()
{
	
}

//分段结构图:获取需要绘制的对象集合
void CStruView::GetDrawingObjectSet(BOOL bInitDCS)
{	
	if(ParentDrawing()==NULL||m_pAttachView==NULL)
		return;
	partSet.Empty();
	nodeSet.Empty();
	CLDSNode *pNode=NULL;
	CLDSPart *pPart=NULL;
	if(m_pAttachView->m_iViewType==0)
	{	//图层视图
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(m_pModule&&!m_pModule->IsSonPart(pPart))
				continue;	//非当前呼高构件不进行绘制
			if(m_pAttachView->filter.IsValidLayer(pNode->layer())&&pNode->iSeg.iSeg==ParentDrawing()->m_nSegI.iSeg)
				nodeSet.append(pNode);
		}
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(m_pModule&&!m_pModule->IsSonPart(pPart))
				continue;	//非当前呼高构件不进行绘制
			pPart->feature=0;
			pPart->is_visible=TRUE;
			if(m_pAttachView->filter.IsValidLayer(pPart->layer())&&pPart->iSeg.iSeg==ParentDrawing()->m_nSegI.iSeg)
				partSet.append((CLDSLinePart*)pPart);
		}
		//火曲杆件
		polyPartlist.Empty();
		Ta.RetrievePolyPartList(polyPartlist);
		for(CLDSPolyPart *pPolyPart=polyPartlist.GetFirst();pPolyPart;pPolyPart=polyPartlist.GetNext())
		{
			if(m_pModule&&!m_pModule->IsSonPart(pPolyPart))
				continue;	//非当前呼高构件不进行绘制
			if(!(m_pAttachView->filter.IsValidLayer(pPolyPart->layer())&&pPolyPart->iSeg.iSeg==ParentDrawing()->m_nSegI.iSeg))
				polyPartlist.DeleteCursor();
		}
		polyPartlist.Clean();
		for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
		{
			BOOL bPush=partSet.push_stack();
			for(CLDSPart *pAfterPart=partSet.GetNext();pAfterPart;pAfterPart=partSet.GetNext())
			{
				if(pPart==pAfterPart)	//删除因上面的补漏操作导致的重复构件问题
					partSet.DeleteCursor();
			}
			if(bPush)
				partSet.pop_stack();
		}
	}
	else if(m_pAttachView->m_iViewType==1)
	{	//定制视图
		for(CViewPart *pViewPart=m_pAttachView->GetFirstViewPart();pViewPart;pViewPart=m_pAttachView->GetNextViewPart())
		{
			pViewPart->pDisplayView=m_pAttachView;
			pPart = Ta.FromPartHandle(pViewPart->handle);
			if(pPart)
			{
				if(m_pModule&&!m_pModule->IsSonPart(pPart))
					continue;	//非当前呼高构件不进行绘制
				pPart->feature=(long)pViewPart;	//保存相关的CViewPart指定 wht 10-12-10
				partSet.append(pPart);
				pPart->is_visible=TRUE;
			}
		}
	}
	//完善螺栓列表(虚拟螺栓或不在本段的螺栓) wht 11-07-22
	BOLTSET boltSet;
	for(pPart = partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->m_bVirtualPart)
			continue;
		CLsRefList *pLsRefList=pPart->GetLsRefList();
		if(pLsRefList==NULL)
			continue;
		if(CalPartDrawType(pPart,dcs,m_pAttachView)==1)
			continue;	//不考虑截面绘制构件上的螺栓
		CLsRef *pLsRef=NULL;
		for(pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(pBolt==NULL)
				continue;
			if(pBolt->m_bVirtualPart)
			{
				pBolt->is_visible=FALSE;	//虚拟螺栓
				boltSet.append(pBolt);	
			}
			else if(pBolt->iSeg.iSeg!=ParentDrawing()->m_nSegI.iSeg)	//&&m_pAttachView->m_iViewType==0
			{
				pBolt->is_visible=FALSE;	//非本段螺栓
				boltSet.append(pBolt);	
			}
		}
	}
	for(pPart=boltSet.GetFirst();pPart;pPart=boltSet.GetNext())
		partSet.append(pPart);
	//完善节点集合
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
			continue;	//指定绘制Z截面的角钢
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
	if(bInitDCS)	//2.计算绘图坐标系
		dcs=CalDrawingUcs(partSet,m_pAttachView,FALSE);
	
	//不绘制侧面显示的法兰上的螺栓 wht 10-03-04
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			if(sys.stru_map.bOnlyDrawHorizontalPlate)
			{	//仅绘制当前视图中水平显示的钢板 wht 10-06-15
				if(fabs(dcs.axis_z*pPlate->ucs.axis_z)<0.707&&
					fabs(pPlate->ucs.axis_z*f3dPoint(0,0,1))<EPS_COS2)	//塔脚板或牛蹄板都应该绘制
				{
					CViewPart *pViewPart=(CViewPart*)pPlate->feature;
					if(pViewPart&&pViewPart->cFlag=='S')
						continue;	//不需要移除展开绘制的钢板 wht 10-12-11
					partSet.DeleteNode(pPlate->handle);
					continue; //竖直绘制
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
				&&pParamPlate->m_iParamType!=TYPE_WATERSHIELDEDPLATE)	//遮水板
				continue;
			if(fabs(dcs.axis_z*pParamPlate->ucs.axis_z)>0.707)
				continue; //水平绘制
			CLsRef *pLsRef=NULL;
			for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				partSet.DeleteNode(pLsRef->GetLsPtr()->handle);
		}
	}
	//保存节点句柄链表及构件句柄链表,保存脚本再次打开文件后使用句柄链表更新节点链表和构件链表 wht 11-07-22
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
//生成视图
void CStruView::Create()
{	
	if(ParentDrawing()==NULL)
		return;
	//构件列表为空时重新初始化构件列表
	if(nodeSet.GetNodeNum()<=0||partSet.GetNodeNum()<=0)
		GetDrawingObjectSet(FALSE);
	DRAGSET.ClearEntSet();
	if(m_bFirstCreate)
	{	//第一次生成，需要自动计算视图装配位置
		CalInitAssemblyPosition();
		m_bFirstCreate=FALSE;
	}
	//使用相对位置重新计算坐标系原点位置
	//CalAssemblyPosByRelativePos();
	CDrawingCommand *pCmd=NULL;
	CHashList<f2dLine>hashOddLine;	//为标注正负头预存储杆件两端基点的绘制位置 wjh-2011.9.25
	CStringKeyHashList<CRodDimPos> rodDimPosHashPartNo;	//记录对应件号需要标注杆件规格长度的杆件句柄 wht 12-05-22
	//统计需要排序绘制的构件数量
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
	{	//按绘制顺序号排序绘制
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
		//按构件绘制顺序排序
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
				const char *sPartNo=pCmd->m_paraPart.pPart->GetPartNo();
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
	{	//绘制构件
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
			const char *sPartNo=pCmd->m_paraPart.pPart->GetPartNo();
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
	{	//绘制辅助线
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DRAW_ASSIST_LINE)
			continue;
		pCmd->Run();
		pCmd->feature=0;
	}
	//缩放视图，否则相似性标注不出来 wht 17-08-15
	f2dRect rect=GetDrawRegion(TRUE);
	ZoomAcadView(rect,50);
	actrTransactionManager->flushGraphics();	
	acedUpdateDisplay();	//刷新缓存更新界面,否则acedSSGet()获取实体集时可能失效 wht 11-06-25
	//指定视图装配位置
	f3dPoint offset=SpecifyAssemblyPosition();
	for(f2dLine *pOddLine=hashOddLine.GetFirst();pOddLine;pOddLine=hashOddLine.GetNext())
	{
		pOddLine->startPt.x+=offset.x;
		pOddLine->startPt.y+=offset.y;
		pOddLine->endPt.x+=offset.x;
		pOddLine->endPt.y+=offset.y;
	}
	//标注视图名称以及上接段号
	InsertDrawingName();
	//标注螺栓特殊间距
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_SPECIAL_LSSPACE)
			continue;
		if(CStruView::m_bDimLsSpace)
			pCmd->Run();
		pCmd->feature=0;
	}
	//标注角钢特殊准据
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_SPECIAL_JGZHUN)
			continue;
		if(CStruView::m_bDimJgZhunJu)
			pCmd->Run();
		pCmd->feature=0;
	}
	//标注正负头
	actrTransactionManager->flushGraphics();	
	acedUpdateDisplay();	//刷新缓存更新界面,否则acedSSGet()获取实体集时可能失效 wht 11-06-25
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
	//标注脚钉间距
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_FOOTNAIL_SPACE)
			continue;
		if(CStruView::m_bDimFootNailSpace)
			pCmd->Run();
		pCmd->feature=0;
	}
	//标注填板间距
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_FILLPLATE_SPACE)
			continue;
		if(CStruView::m_bDimFillPlateSpace)
			pCmd->Run();
		pCmd->feature=0;
	}
	//标注构件明细
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_PARTSPEC)
			continue;
		CLDSDbObject *pObj=pCmd->m_paraPartSpec.pObj;
		if(pObj&&pObj->IsPart()&&((CLDSPart*)pObj)->IsLinePart())
		{	//杆件规格及长度尽量标注在右侧杆件上 wht 12-05-22
			CLDSLinePart *pLinePart=(CLDSLinePart*)pObj;
			if(pCmd->m_paraPartSpec.bOnlyDimPartNo==FALSE)
			{	//标注规格长度时才需要进行以下判断，否则手动指定不标注规格仍会标注规格 wht 17-08-14
				CRodDimPos *pRodDimPos=rodDimPosHashPartNo.GetValue(pLinePart->GetPartNo());
				if(pRodDimPos&&pRodDimPos->GetRodHandle()==pLinePart->handle)
					pCmd->m_paraPartSpec.bOnlyDimPartNo=FALSE;	
				else
					pCmd->m_paraPartSpec.bOnlyDimPartNo=TRUE;
			}
		}
		if(CStruView::m_bDimPartSpec)
			pCmd->Run();
		pCmd->feature=0;
	}
	//标注相似形
	BOOL bNeedSetLayer=FALSE;	//是否需要设置图层
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{	//预先判断是否存在通过拾取点标注的相似形
		if(pCmd->m_paraSimPoly.nSimPolyCmdType==2&&pCmd->m_bExecute)
		{
			bNeedSetLayer=TRUE;
			break;
		}
	}
	CStackAcadVar stackEcho("CMDECHO",0);
	if(bNeedSetLayer)
	{	//存在通过拾取点标注的相似形，需要事先设置好图层
	#ifdef _ARX_2007
		acedCommand(RTSTR,L"undo",RTSTR,L"be",RTNONE);
		acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,L"*",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
		acedCommand(RTSTR,L"layer",RTSTR,L"F",RTSTR,L"*",RTSTR,L"",RTNONE);
		CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
		acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,(ACHAR*)_bstr_t((char*)sLayers),RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
		acedCommand(RTSTR,L"undo",RTSTR,L"e",RTNONE);
	#else
		acedCommand(RTSTR,"undo",RTSTR,"be",RTNONE);
		acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
		acedCommand(RTSTR,"layer",RTSTR,"F",RTSTR,"*",RTSTR,"",RTNONE);
		CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
		acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,&sLayers,RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
		acedCommand(RTSTR,"undo",RTSTR,"e",RTNONE);
#endif
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
	{	//存在通过拾取点标注的相似形，执行完标注相似形命令后回复原有图层状态
	#ifdef _ARX_2007
		acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,L"*",RTSTR,L"",RTNONE);
		acedCommand(RTSTR,L"layer",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
	#else
		acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"",RTNONE);
		acedCommand(RTSTR,"layer",RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
	#endif
	}
	//标注螺栓信息
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_BOLTINFO)
			continue;
		if(CStruView::m_bDimBoltInfo)
			pCmd->Run();
		pCmd->feature=0;
	}
	//标注垫圈信息
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==0||pCmd->m_eCmdType!=CMD_DIM_BOLTPAD)
			continue;
		if(CStruView::m_bDimBoltPad)
			pCmd->Run();
		pCmd->feature=0;
	}
	//标注填板间距
	//标注其他
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->feature==1)
			pCmd->Run();
	}
	//标注剖视图符号
	long key=ParentDrawing()->listComponents.GetKey(this);
	CSectionViewSymbol *pViewSymbol=NULL;
	f3dPoint datum_pos=GetMapFrmInsideTopLeft();
	for(pViewSymbol=ParentDrawing()->sectionViewSymbols.GetFirst();pViewSymbol;pViewSymbol=ParentDrawing()->sectionViewSymbols.GetNext())
	{
		if(pViewSymbol->hAttachViewKey!=key)
			continue;	//仅绘制从属于当前视图的剖视图符
		pViewSymbol->Draw();
	}
	//插入图元后将图元矩形区域添加到绘图矩形框数组中
	UpdateDrawingRectArray();
#ifdef _ARX_2007
	ads_command(RTSTR,L"REGEN",RTNONE);
#else
	ads_command(RTSTR,"REGEN",RTNONE);
#endif
}

typedef CLDSPart* PARTPTR;
//塔构件类型排序,原则如下:
//主材角钢0;塔脚板2,其它构件1
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
				grade--;	//有圆柱面相贯线
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

//优先级高的构件级别高,但特征值低(如主材为0)
//-1表示第一构件级别高 0表示相同 1表示第二构件级别高
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
	//根据构件的类型进行排序
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
	int nDrawingSerialNo=1;	//构件绘制顺序号
	LINEPARTSET OtherAngleSet,InsideShortJgSet,OutSideShortJgSet;
	PARTSET InSidePlateSet,OutSidePlateSet,TubeSet,LineSlotSet,InsideParamPlateSet,OutsideParamPlateSet,LineFlatSet;
	for(pPart=partSet.GetFirst();pPart!=NULL;pPart=partSet.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_BOLT&&pPart->m_bVirtualPart)	
			continue;	//成图时不绘制除螺栓以外的隐含构件
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSLineAngle *pJg=(CLDSLineAngle*)pPart;
			f3dPoint cur_jg_vec=pJg->End()-pJg->Start();
			normalize(cur_jg_vec);
			if((pJg->pStart==NULL||pJg->pEnd==NULL)
				&&(pJg->desStartPos.datumPoint.datum_pos_style==7||pJg->desEndPos.datumPoint.datum_pos_style==7))
			{	//短角钢始端/终端基准点定位方式为:7.沿角钢两肢法向偏移的包铁楞点 wht 11-03-02
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
			//支持绘制角钢截面 wht 10-06-17
			if(pJg->m_bFootNail||fabs(cur_jg_vec*dcs.axis_z)>0.70710)	//角钢脚钉或需要绘制角钢截面的角钢
				OtherAngleSet.append(pJg);
			else
			{
				if(!ProcessJgStru(pJg,dcs))
					OtherAngleSet.append(pJg);
			}
		}
		else if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//添加绘制组合角钢的命令
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
		else if(pPart->GetClassTypeId()==CLS_PLATE)		//绘制普通钢板结构图
			ProcessPlateStu((CLDSPlate*)pPart,dcs,OutSidePlateSet,InSidePlateSet);
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)//绘制参数化板（如端板等）结构图
		{
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
			normalize(dcs.axis_z);
			if(pParamPlate->m_iParamType==TYPE_ROLLEND&&(pParamPlate->ucs.axis_z*dcs.axis_z>EPS_COS2))
				InsideParamPlateSet.append(pParamPlate);
			else
				OutsideParamPlateSet.append(pParamPlate);
		}
		else if(pPart->GetClassTypeId()==CLS_BOLT)		//绘制螺栓
			LSDRAW.AppendLsRef((CLDSBolt*)pPart,TRUE,dcs.axis_z);
		else if(pPart->GetClassTypeId()==CLS_LINESLOT)	//绘制槽钢(现在只有脚钉板用到槽钢)
			LineSlotSet.append(pPart);
		else if(pPart->GetClassTypeId()==CLS_LINEFLAT)	//绘制扁铁
			LineFlatSet.append(pPart);
	}
	PARTSET HorizontalPlateSet;	//水平绘制的钢板集合
	//里外铁角钢分开绘制用以进行一定程度上的消隐	
	CLDSLinePart *pLinePart=NULL;
	for(pLinePart=OtherAngleSet.GetFirst();pLinePart;pLinePart=OtherAngleSet.GetNext())
	{	//需要绘制截面的角钢或里外铁无法判断的角钢
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
		//计算构件绘制方式 
		CDrawLineAngleTagInfo *pInfo=hashAngleDrawInfo.Add(pLinePart->handle);
		int h0v1e2=CalPartDrawType(pLinePart,dcs,m_pAttachView,pInfo);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}
	for(pLinePart=InsideShortJgSet.GetFirst();pLinePart;pLinePart=InsideShortJgSet.GetNext())
	{	//内侧短角钢
		if(hashDrawParts.GetValue(pLinePart->handle)!=NULL)
			continue;	//已绘制
		hashDrawParts.SetValue(pLinePart->handle,pLinePart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pLinePart;
		pCmd->m_paraPart.hPart=pLinePart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//计算构件绘制方式 
		int h0v1e2=CalPartDrawType(pLinePart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}
	//建立外铁短角钢哈希表。因为外铁短角钢一般在最外侧，最后绘制，虽是普通外铁，但应优先按外铁绘制（不绘心线） wjh-2011.8.17
	CHashTable<CLDSLinePart*>hashShortJg;
	hashShortJg.CreateHashTable(OutSideShortJgSet.GetNodeNum());
	for(pLinePart=OutSideShortJgSet.GetFirst();pLinePart;pLinePart=OutSideShortJgSet.GetNext())
		hashShortJg.SetValueAt(pLinePart->handle,pLinePart);
	for(pLinePart=INSIDEJGSETDRAW.GetFirst();pLinePart;pLinePart=INSIDEJGSETDRAW.GetNext())
	{	//里铁角钢
		if(hashDrawParts.GetValue(pLinePart->handle)!=NULL)
			continue;	//已绘制
		if(hashShortJg.ContainsKey(pLinePart->handle))
			continue;	//外铁短角钢
		hashDrawParts.SetValue(pLinePart->handle,pLinePart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pLinePart;
		pCmd->m_paraPart.hPart=pLinePart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//计算构件绘制方式 
		int h0v1e2=CalPartDrawType(pLinePart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
		//
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLinePart;
		CLDSGroupLineAngle *pGroupJg=NULL;
		if(pLineAngle->group_father_jg_h>0x20)
			pGroupJg=(CLDSGroupLineAngle*)Ta.FromPartHandle(pLineAngle->group_father_jg_h,CLS_GROUPLINEANGLE);
		//不绘制四主材组合角钢中的里铁 wht 11-08-03
		if(pGroupJg&&pGroupJg->group_style==2)
			pCmd->m_bExecute=FALSE;
	}
	for(pPart=InsideParamPlateSet.GetFirst();pPart;pPart=InsideParamPlateSet.GetNext())
	{
		if(hashDrawParts.GetValue(pPart->handle)!=NULL)
			continue;	//已绘制
		hashDrawParts.SetValue(pPart->handle,pPart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pPart;
		pCmd->m_paraPart.hPart=pPart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//计算构件绘制方式 
		int h0v1e2=CalPartDrawType(pPart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}
	for(pPart=InSidePlateSet.GetFirst();pPart;pPart=InSidePlateSet.GetNext())
	{
		if(hashDrawParts.GetValue(pPart->handle)!=NULL)
			continue;	//已绘制
		hashDrawParts.SetValue(pPart->handle,pPart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pPart;
		pCmd->m_paraPart.hPart=pPart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//计算构件绘制方式 
		int h0v1e2=CalPartDrawType(pPart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
		if(h0v1e2==0)
			HorizontalPlateSet.append(pPart);
	}
	for(pPart=TubeSet.GetFirst();pPart;pPart=TubeSet.GetNext())
	{
		if(hashDrawParts.GetValue(pPart->handle)!=NULL)
			continue;	//已绘制
		hashDrawParts.SetValue(pPart->handle,pPart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pPart;
		pCmd->m_paraPart.hPart=pPart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//计算构件绘制方式 
		int h0v1e2=CalPartDrawType(pPart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}
	for(pPart=OutSidePlateSet.GetFirst();pPart;pPart=OutSidePlateSet.GetNext())
	{
		if(hashDrawParts.GetValue(pPart->handle)!=NULL)
			continue;	//已绘制
		hashDrawParts.SetValue(pPart->handle,pPart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pPart;
		pCmd->m_paraPart.hPart=pPart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//计算构件绘制方式 
		int h0v1e2=CalPartDrawType(pPart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
		if(h0v1e2==0)
			HorizontalPlateSet.append(pPart);
	}
	for(pPart=OutsideParamPlateSet.GetFirst();pPart;pPart=OutsideParamPlateSet.GetNext())
	{
		if(hashDrawParts.GetValue(pPart->handle)!=NULL)
			continue;	//已绘制
		hashDrawParts.SetValue(pPart->handle,pPart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pPart;
		pCmd->m_paraPart.hPart=pPart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//计算构件绘制方式 
		int h0v1e2=CalPartDrawType(pPart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}	
	for(pPart=LineSlotSet.GetFirst();pPart;pPart=LineSlotSet.GetNext())
	{
		if(hashDrawParts.GetValue(pPart->handle)!=NULL)
			continue;	//已绘制
		hashDrawParts.SetValue(pPart->handle,pPart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pPart;
		pCmd->m_paraPart.hPart=pPart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//计算构件绘制方式 
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
		//计算构件绘制方式
		int h0v1e2=CalPartDrawType(pPart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}
	for(pLinePart=OUTSIDEJGSETDRAW.GetFirst();pLinePart;pLinePart=OUTSIDEJGSETDRAW.GetNext())
	{	//外铁角钢
		if(hashDrawParts.GetValue(pLinePart->handle)!=NULL)
			continue;	//已绘制
		if(hashShortJg.ContainsKey(pLinePart->handle))
			continue;	//外铁短角钢
		hashDrawParts.SetValue(pLinePart->handle,pLinePart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pLinePart;
		pCmd->m_paraPart.hPart=pLinePart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//计算构件绘制方式 
		int h0v1e2=CalPartDrawType(pLinePart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}
	for(pLinePart=OutSideShortJgSet.GetFirst();pLinePart;pLinePart=OutSideShortJgSet.GetNext())
	{	//外侧短角钢
		if(hashDrawParts.GetValue(pLinePart->handle)!=NULL)
			continue;	//已绘制
		hashDrawParts.SetValue(pLinePart->handle,pLinePart);
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pLinePart;
		pCmd->m_paraPart.hPart=pLinePart->handle;
		pCmd->SetDrawingSerialNo(nDrawingSerialNo++);
		//计算构件绘制方式 
		int h0v1e2=CalPartDrawType(pLinePart,dcs,m_pAttachView);
		if(h0v1e2>=0)
			pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
	}
	//水平绘制的钢板上的螺栓都需要正面绘制 wht 11-07-26
	for(pPart=HorizontalPlateSet.GetFirst();pPart;pPart=HorizontalPlateSet.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_PLATE)
			continue;
		CLDSPlate *pPlate=(CLDSPlate*)pPart;
		CDrawCoordTrans dct(dcs,1.0/m_fScale);
		CDrawingAssistant *pAssist=m_pAttachView->GetDrawingAssistByHandle(pPlate->handle,ParentDrawing()->m_nSegI);
		if(pAssist)
			pAssist->GetRotateAxisAndAngle(dcs.axis_z,dct.m_xSpreadLine,dct.m_fSpreadAngle);
		DrawLsRefList(pPlate->GetLsRefList(),dct,TRUE);//显示所有螺栓
	}
	//绘制螺栓
	CLsDrawUnit *pLsDraw=NULL;
	for(pLsDraw=LSDRAW.GetFirst();pLsDraw;pLsDraw=LSDRAW.GetNext())
	{
		//此处一刀切过滤掉所有的侧面螺栓不妥，目前暂改为由绘图人员从视图里去掉
		//if(m_pAttachView&&m_pAttachView->m_iViewType==1&&pLsDraw->front0_side1==1)
		//	continue;	//定制视图不显示侧面螺栓，暂时在此处过滤侧面螺栓
		if(pLsDraw->pLs==NULL)
			continue;
		if(pLsDraw->pLs->m_bVirtualPart&&pLsDraw->front0_side1==1)
			continue;	//侧面显示的虚拟螺栓不需要绘制 wht 11-08-05
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DRAW_PART;
		pCmd->InitCommandPara();
		pCmd->m_paraPart.pPart=pLsDraw->pLs;
		pCmd->m_paraPart.hPart=pLsDraw->pLs->handle;
		pCmd->m_paraPart.para.bolt.bOriginal=pLsDraw->bOriginal;
		if(pLsDraw->pLs->is_visible==FALSE||pLsDraw->pLs->m_bVirtualPart)	//副本螺栓 wht 11-07-25
			pCmd->m_paraPart.para.bolt.bOriginal=FALSE;
		pCmd->m_paraPart.para.bolt.front0_side1=pLsDraw->front0_side1;
	}
}

//图面法线发生变化之后需要更新构件绘制方式 wht 11-07-31
void CStruView::UpdatePartDrawTypeAndSpecialDim()
{
	LSDRAW.EmptyDrawList();
	PARTSET HorizontalPlateSet;	//水平绘制的钢板集合
	CDrawingCommand *pCmd=NULL;
	CHashList<CDrawingCommand*> hashBoltList;
	//更新构件绘制方式
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
		{	//重新计算构件绘制方式
			int h0v1e2=CalPartDrawType(pCmd->m_paraPart.pPart,dcs,m_pAttachView);
			if(h0v1e2>=0)
				pCmd->m_paraPart.cDrawFlag='0'+h0v1e2;
			if(h0v1e2==0&&pCmd->m_paraPart.pPart->GetClassTypeId()==CLS_PLATE)
				HorizontalPlateSet.append(pCmd->m_paraPart.pPart);
		}
	}
	//
	//水平绘制的钢板上的螺栓都需要正面绘制 wht 11-07-26
	CLDSPart *pPart=NULL;
	for(pPart=HorizontalPlateSet.GetFirst();pPart;pPart=HorizontalPlateSet.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_PLATE)
			continue;
		CLDSPlate *pPlate=(CLDSPlate*)pPart;
		DrawLsRefList(pPlate->GetLsRefList(),CDrawCoordTrans(dcs,m_fScale),TRUE);//显示所有螺栓
	}
	//更新螺栓绘制参数
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
		if(pLsDraw->pLs->is_visible==FALSE||pLsDraw->pLs->m_bVirtualPart)	//副本螺栓 wht 11-07-25
			pCmd->m_paraPart.para.bolt.bOriginal=FALSE;
		pCmd->m_paraPart.para.bolt.front0_side1=pLsDraw->front0_side1;
	}
	//删除原有角钢特殊间距及螺栓特殊间距相关命令
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN||pCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
			cmdList.DeleteCursor();
	}
	cmdList.Clean();
	//重新生成螺栓特殊间距及角钢特殊间距标注
	CreateDimSpecialLsSpaceCmdList();
	CreateDimSpecialJgZhunJuCmdList();
}

//生成构件明细标注
void CStruView::CreateDimPartSpecCmdList()
{
	dimpartspec.EmptyPartSpecDim();	//清空构件明细标注列表
	CDrawingCommand *pCmd=NULL;
	CLDSPart *pPart=NULL;
	CHashList<CLDSLinePart*> polyPartList;
	CStringKeyHashList<long>hashPartNo;
	CStringKeyHashList<long>hashSlotFootNailPlate;	//脚钉板
	m_iMirMsg=2;	//定制视图不需要对称标注构件明细 wht 11-07-26
	if(m_pAttachView&&m_pAttachView->m_iViewType==0)
	{
		if(stricmp(m_pAttachView->defaultLayer,"SPQ")==0||stricmp(m_pAttachView->defaultLayer,"SPH")==0)
			m_iMirMsg=0;
		else if(stricmp(m_pAttachView->defaultLayer,"SPZ")==0||stricmp(m_pAttachView->defaultLayer,"SPY")==0)
			m_iMirMsg=1;
	}
	//十字插板或法兰附加构件列表
	CHashList<long> attachPlateHash;
	CHashSet<CLDSPart*> hashDrawParts;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		hashDrawParts.SetValue(pPart->handle,pPart);
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
			continue;	//不用标注组合角钢明细 wht 11-07-29
		if(attachPlateHash.GetValue(pPart->handle))
			continue;	//不标注十字插板或者法兰附加构件明细 wht 12-03-22
		if(pPart->IsLinePart()||pPart->IsPlate())
		{	
			BOOL bDimPartNoOnly=FALSE;
			bool disableDimBackMirLabel=false;
			if(pPart->IsLinePart())
			{
				CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
				if(pLinePart->bHuoQuStart||pLinePart->bHuoQuEnd)
				{	//火曲构件单独处理,火曲构件仅标注一次编号即可 wht 11-06-21
					if(polyPartList.GetValue(pLinePart->huoqu_handle)==NULL)
						polyPartList.SetValue(pLinePart->huoqu_handle,pLinePart);
					continue;
				}
				CLDSLinePart *pMirLinePart=NULL;
				MIRMSG mirmsg(m_iMirMsg+1);
				if(m_iMirMsg==0)	//X轴对称
					pMirLinePart=(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(1));
				else if(m_iMirMsg==1)	//Y轴对称
					pMirLinePart=(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(2));
				if((m_iMirMsg==0||m_iMirMsg==1)&&pMirLinePart)
				{
					CLDSPart *pMirZPart=pMirLinePart->GetMirPart(MIRMSG(3));
					if(pMirZPart!=NULL&&hashDrawParts.GetValue(pMirZPart->handle)&&stricmp(pMirZPart->Label,pMirLinePart->Label)==0)
						disableDimBackMirLabel=true;	//Z轴对称杆件已在当前视图中绘制，且编号相同
				}
				if(hashPartNo.GetValue(pLinePart->GetPartNo()))	//已存在同构件编号或对称标注时已存在对称构件编号
					//||(pMirLinePart&&hashPartNo.GetValue(pMirLinePart->sPartNo)))	
					bDimPartNoOnly=TRUE;
				else 
					hashPartNo.Add(pLinePart->GetPartNo());
				//脚钉板
				if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
				{
					CLDSLineSlot *pLineSlot=(CLDSLineSlot*)pLinePart;
					if(pLineSlot->IsBaseSlot())
					{
						long *pFatherPart=hashSlotFootNailPlate.GetValue(pLineSlot->GetPartNo());
						if(pFatherPart&&*pFatherPart==pLineSlot->m_hPartWeldParent)
							continue;	//同一根钢管上同一编号的脚钉仅需要标注一次 wht 11-07-21
						else 
							hashSlotFootNailPlate.SetValue(pLineSlot->GetPartNo(),pLineSlot->m_hPartWeldParent);
					}
				}
			}
			pCmd=cmdList.append();
			pCmd->m_pDrawingCom=this;
			pCmd->m_eCmdType=CMD_DIM_PARTSPEC;//标注构件明细
			pCmd->InitCommandPara();
			pCmd->m_paraPartSpec.pObj=pPart;
			pCmd->m_paraPartSpec.hObj=pPart->handle;
			pCmd->m_paraPartSpec.iMirMsg=disableDimBackMirLabel?-1:m_iMirMsg;
			if(bDimPartNoOnly)	//第二次出现的同编号杆件，仅标注编号 wht 11-07-21
				pCmd->m_paraPartSpec.bOnlyDimPartNo=bDimPartNoOnly;
			if(pPart->IsPlate())
			{
				BOOL bStdFl=FALSE,bDrawRefLine=TRUE;
				if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				{
					CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
					bStdFl=pParamPlate->m_bStdPart&&pParamPlate->IsFL();//标准法兰不需要绘制引出线
					//法兰及标准件插板不需要标注规格及厚度 wht 12-03-22
					if(pParamPlate->IsFL()||(pParamPlate->m_bStdPart&&pParamPlate->IsInsertPlate()))
					{
						pCmd->m_paraPartSpec.bOnlyDimPartNo=TRUE;
						bDrawRefLine=FALSE;
					}
				}
				if(!bStdFl&&fabs(dcs.axis_z*pPart->ucs.axis_z)<0.707)
					pCmd->m_paraPartSpec.bDrawRefLine=TRUE;	//竖直绘制的钢板需要绘制引出线 wht 11-07-21
				else if(pPart->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pPart)->jdb_style==4)
					pCmd->m_paraPartSpec.bDrawRefLine=TRUE;	//标注螺栓垫板时默认都绘制引出线 wht 11-07-18
				if(bDrawRefLine==FALSE)
					pCmd->m_paraPartSpec.bDrawRefLine=FALSE;//不绘制引出线 wht 12-03-23
			}
			else if(pPart->GetClassTypeId()==CLS_LINESLOT&&((CLDSLineSlot*)pPart)->IsBaseSlot())
			{	//标注脚钉板编号时需要绘制引出线,不需要标注长度及规格
				pCmd->m_paraPartSpec.bDrawRefLine=TRUE;		
				pCmd->m_paraPartSpec.bOnlyDimPartNo=TRUE;
			}
		}
	}
	//火曲构件仅标注一次标号
	CLDSLinePart **ppLinePart=NULL;
	for(ppLinePart=polyPartList.GetFirst();ppLinePart;ppLinePart=polyPartList.GetNext())
	{
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DIM_PARTSPEC;//标注构件明细
		pCmd->InitCommandPara();
		pCmd->m_paraPartSpec.iMirMsg=m_iMirMsg;
		pCmd->m_paraPartSpec.pObj=*ppLinePart;
		pCmd->m_paraPartSpec.hObj=(*ppLinePart)->handle;
	}
}
//生成正负头标注命令
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
			continue;	//短角钢、脚钉板不用标注正负头
		if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			continue;	//不标注组合角钢正负头 
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
				continue;	//绘制竖直截面的杆件不标注正负头
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
			continue;	//无正负头
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DIM_ODDMENT;//标注杆件正负头
		pCmd->InitCommandPara();
		pCmd->m_paraOddment.pLinePart=pLinePart;
		pCmd->m_paraOddment.hLinePart=pPart->handle;
	}
}
//生成脚钉间距标注命令
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
			continue;	//不统计短角钢
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
			//杆件上的脚钉个数大于1才认为需要标注脚钉间距,避免标注端连接螺栓为脚钉的情况 wht 11-07-25
			if(nFootNailCount<=1)	
				continue;
		}
		else 
			continue;
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DIM_FOOTNAIL_SPACE;//标注杆件脚钉间距
		pCmd->InitCommandPara();
		pCmd->m_paraDimFootNailSpace.pLinePart=pLinePart;
		pCmd->m_paraDimFootNailSpace.hLinePart=pPart->handle;
		pCmd->m_paraDimFootNailSpace.fSpace=5;
		pCmd->m_paraDimFootNailSpace.fDimDistance=10;
	}
}
//生成填板间距标注命令
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
			continue;	//不统计短角钢
		int nFillPlateCount=0;
		for (CLDSPlate *pFillPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pFillPlate;pFillPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
		{
			if (pFillPlate->designInfo.origin.des_para.SCALE_NODE.hLinePart!=pLinePart->handle)
				continue;
			if (pFillPlate->designInfo.origin.datum_pos_style!=10)	
				continue;
			nFillPlateCount++;
		}
		//组合角钢上的填板个数大于1才认为需要标注填板间距,避免标注端连接板为填板的情况
		if (nFillPlateCount<=1)
			continue;
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DIM_FILLPLATE_SPACE;//标注组合角钢填板间距
		pCmd->InitCommandPara();
		pCmd->m_paraDimFillPlateSpace.pLinePart=pLinePart;
		pCmd->m_paraDimFillPlateSpace.hLinePart=pPart->handle;
		pCmd->m_paraDimFillPlateSpace.fSpace=5;
		pCmd->m_paraDimFillPlateSpace.fDimDistance=10;
	}
}
//生成螺栓特殊间距标注命令
void CStruView::CreateDimSpecialLsSpaceCmdList()
{	//统计螺栓间距
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
		if(pViewPart&&pViewPart->cFlag=='X'		//平铺绘制
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
		//TODO:暂时在此处屏蔽螺栓定位基准不同的螺栓之间的特殊间距，后续应改进螺栓间距提取算法 wht 17-08-08
		CLDSBolt *pStartBolt=(CLDSBolt*)Ta.Parts.FromHandle(pSpaceDim->hStartLs,CLS_BOLT);
		CLDSBolt *pEndBolt=(CLDSBolt*)Ta.Parts.FromHandle(pSpaceDim->hEndLs,CLS_BOLT);
		CLDSLinePart *pRod=pSpaceDim->GetDatumRod();
		CLDSLinePart *pDatumRod=pSpaceDim->GetDatumRod(1);
		if( pRod&&pStartBolt&&pEndBolt&&pRod!=pDatumRod&&
			pStartBolt->des_base_pos.hPart!=pEndBolt->des_base_pos.hPart)
		{
			double oddment=pRod->endOdd();
			if(DISTANCE(pStartBolt->ucs.origin,pRod->Start())<DISTANCE(pStartBolt->ucs.origin,pRod->End()))
				oddment=pRod->startOdd();	
			if(oddment<0)
				pCmd->m_bExecute=FALSE;
			else
				pCmd->m_bExecute=TRUE;
		}
	}
}
//生成角钢特殊准据标注命令
void CStruView::CreateDimSpecialJgZhunJuCmdList()
{
	CLDSPart *pPart=NULL;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_LINEANGLE)
			continue;
		CLDSLineAngle *pJg=(CLDSLineAngle*)pPart;
		if(pJg->pStart==NULL||pJg->pEnd==NULL)
			continue;	//短角钢
		if(pJg->m_bFootNail)
			continue;	//角钢脚钉
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
			x_wing0_y_wing1=0;	//X肢为当前绘制肢
		else if(pViewPart&&(pViewPart->cFlag=='Y'||pViewPart->cFlag=='Q'))
			x_wing0_y_wing1=1;	//Y肢为当前绘制肢
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
		
		CDimJgZhun dim_jg_zhun(pJg);	//分析计算角钢上的螺栓准距线
		for(CLsRef *pLsRef=pJg->GetFirstLsRef();pLsRef!=NULL;pLsRef=pJg->GetNextLsRef())
		{
			if((*pLsRef)->IsFootNail())
				continue;
			if(fabs(dot_prod((*pLsRef)->get_norm(),work_wing_norm))<0.707)
				continue;	//不是当前面螺栓不绘制
			dim_jg_zhun.DimLsZhun(pLsRef);	//分析计算上端螺栓准距线
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
			{	//启用特殊准据且螺栓心线为当前角钢准线所在准据时，需要标注一次 wht 11-06-21
				if(bHasSameDim)
					zhun_dim.bDimZhunJu=false;
				else
					bHasSameDim=TRUE;
			}
			else if(fabs(jgzj.g-zhun_dim.dist)<1||fabs(jgzj.g1-zhun_dim.dist)<1
				||fabs(jgzj.g2-zhun_dim.dist)<1||fabs(jgzj.g1-zhun_dim.dist)<1)
				zhun_dim.bDimZhunJu=false;	//未启用特殊准据,螺栓线所在准据为标准准据,不添加标注特殊准据命令(如需绘制螺栓线时再进行添加) wht 11-06-30
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
//生成螺栓信息标注命令列表
void CStruView::CreateDimBoltInfoCmdList()
{
	// 1.统计所有正面绘制的螺栓
	BOLTSET boltSet;
	PLATESET plateSet;
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
		if(pCmd->m_paraPart.pPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate *pPlate=(CLDSPlate*)pCmd->m_paraPart.pPart;
			if(pCmd->m_paraPart.cDrawFlag=='0'&&pPlate->wiType!=CLDSPlate::WASHER_PLATE)
				plateSet.append((CLDSPlate*)pCmd->m_paraPart.pPart);
		}
		if(pCmd->m_paraPart.pPart->GetClassTypeId()!=CLS_BOLT)
			continue;	//非螺栓
		if(pCmd->m_paraPart.para.bolt.front0_side1!=0)
			continue;	//非正面绘制螺栓
		CLDSBolt *pBolt=(CLDSBolt*)pCmd->m_paraPart.pPart;
		if(pBolt->IsFootNail()&&pBolt->get_oldL()<EPS)
			continue;	//标注螺栓信息时不标注非代孔脚钉信息
		if(!boltHandleHash.GetValue(pBolt->handle))
		{
			boltSet.append(pBolt);
			boltHandleHash.Add(pBolt->handle);
		}
	}
	// 2.将螺栓分组
	CLDSBolt *pBolt=NULL;
	ATOM_LIST<CLsGroupDimUnit> lsGroupDimList;
	CLsGroupDimUnit *pLsGroupDim=NULL;
	CLsGroupDimUnit lsDim;
	for(CLDSPlate *pPlate=plateSet.GetFirst();pPlate;pPlate=plateSet.GetNext())
	{
		if(!lsDim.Init(pPlate,boltSet))
			continue;
		pLsGroupDim=lsGroupDimList.append();
		pLsGroupDim->Init(pPlate,boltSet);
		for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
			boltSet.DeleteNode((*pLsRef)->handle);
	}
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
		{	//添加新的螺栓组
			pLsGroupDim=lsGroupDimList.append();
			pLsGroupDim->Init(pBolt);
		}
	}
	// 3.合并距离较近的分组
	for(pLsGroupDim=lsGroupDimList.GetFirst();pLsGroupDim;pLsGroupDim=lsGroupDimList.GetNext())
	{
		int pushed=lsGroupDimList.push_stack();
		pLsGroupDim->group_scope.fMinZ=pLsGroupDim->group_scope.fMinZ=0;
		for(CLsGroupDimUnit* pAfterGroup=lsGroupDimList.GetNext();pAfterGroup;pAfterGroup=lsGroupDimList.GetNext())
		{
			if(pLsGroupDim->GetBoltD()!=pAfterGroup->GetBoltD()||pLsGroupDim->GetBoltL()!=pAfterGroup->GetBoltL())
				continue;
			if(pLsGroupDim->ucs.axis_x*pAfterGroup->ucs.axis_x<=EPS_COS2)	//标注方向近似共线
				continue;	//标注方向不同不能合并
			f3dPoint org=pAfterGroup->ucs.origin;
			coord_trans(org,pLsGroupDim->ucs,FALSE,TRUE);
			SCOPE_STRU scope=pAfterGroup->group_scope;
			scope.fMinX+=org.x;
			scope.fMaxX+=org.x;
			scope.fMinY+=org.y;
			scope.fMaxY+=org.y;
			scope.fMinZ=scope.fMaxZ=0;
			//按螺栓横向及纵向间隙阈值，扩大区域范围 wht 12-03-23
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
	// 4.添加螺栓信息标注命令
	for(pLsGroupDim=lsGroupDimList.GetFirst();pLsGroupDim;pLsGroupDim=lsGroupDimList.GetNext())
	{
		CDrawingCommand *pBoltInfoCmd=cmdList.append();
		pBoltInfoCmd->m_pDrawingCom=this;
		pBoltInfoCmd->m_eCmdType=CMD_DIM_BOLTINFO;
		pBoltInfoCmd->InitCommandPara();
		pBoltInfoCmd->m_paraBoltGroupInfo.lsGroupDim.CopyProperty(pLsGroupDim);
	}
}
static bool IsInternalInters(GEPOINT& v1s2s,GEPOINT& v1e2s,GEPOINT& v1s2e,GEPOINT& v1e2e)
{
	normalize(v1s2s);
	normalize(v1e2s);
	normalize(v1s2e);
	normalize(v1e2e);
	GEPOINT vn1,vn2;
	if(fabs(v1s2s*v1e2s)<EPS_COS)	//2号线始点不与1号线共线
		vn1=v1s2s^v1e2s;
	if(fabs(v1s2e*v1e2e)<EPS_COS)	//2号线终点不与1号线共线
		vn2=v1s2e^v1e2e;
	if(vn1*vn2<0)	//两线有内交点
		return true;
	else
		return false;
}
  
int CStruView::CalDualRodIntersPoint(CLDSLinePart* pPrevRod,CLDSLinePart* pNextRod,SIMPOLY_VERTEX* pBaseVertex,SIMPOLY_VERTEX* pInsertVertex)
{
	SmartRodPtr pRod1=pPrevRod,pRod2=pNextRod,pMidRod=NULL;
	CLDSNode *pPrevTipNode=NULL, *pNextTipNode=NULL;
	bool online=false;
	bool mayEndOffset=false;	//是否可能存在端点结构偏移情况
	//一杆件的端节点父杆件正好是另一杆件
	if(pPrevRod->pStart!=NULL&&pPrevRod->hStartDatumRod==pNextRod->handle)
		pPrevTipNode=pPrevRod->pStart;
	else if(pPrevRod->pEnd!=NULL&&pPrevRod->hEndDatumRod==pNextRod->handle)
		pPrevTipNode=pPrevRod->pEnd;
	else if(pNextRod->pStart!=NULL&&pNextRod->hStartDatumRod==pPrevRod->handle)
		pNextTipNode=pNextRod->pStart;
	else if(pNextRod->pEnd!=NULL&&pNextRod->hEndDatumRod==pPrevRod->handle)
		pNextTipNode=pNextRod->pEnd;
	else if(pPrevRod->IsAngle()&&pNextRod->IsAngle())
		mayEndOffset=true;
	if(mayEndOffset)
	{
		long hShareDependRod=0;//pPrevTipNode->hFatherPart;
		GEPOINT v1s2s=pRod1->pStart->xOriginalPos-pRod2->pStart->xOriginalPos,v1e2s=pRod1->pEnd->xOriginalPos-pRod2->pStart->xOriginalPos;
		GEPOINT v1s2e=pRod1->pStart->xOriginalPos-pRod2->pEnd->xOriginalPos,v1e2e=pRod1->pEnd->xOriginalPos-pRod2->pEnd->xOriginalPos;
		if(IsInternalInters(v1s2s,v1e2s,v1s2e,v1e2e))
			mayEndOffset=false;
		bool bS2S=false,bS2E=false,bE2S=false,bE2E=false;
		double distance=0,distOfS2S=0,distOfS2E=0,distOfE2S=0,distOfE2E=0;
		if(pRod1->pStart==pRod2->pStart)
			bS2S=true;
		else if(pRod1->pStart==pRod2->pEnd)
			bS2E=true;
		else if(pRod1->pEnd==pRod2->pStart)
			bE2S=true;
		else if(pRod1->pEnd==pRod2->pEnd)
			bE2E=true;
		else
		{
			distOfS2S=DISTANCE(pRod1->pStart->xOriginalPos,pRod2->pStart->xOriginalPos);
			distOfS2E=DISTANCE(pRod1->pStart->xOriginalPos,pRod2->pEnd->xOriginalPos);
			distOfE2S=DISTANCE(pRod1->pEnd->xOriginalPos,pRod2->pStart->xOriginalPos);
			distOfE2E=DISTANCE(pRod1->pEnd->xOriginalPos,pRod2->pEnd->xOriginalPos);
			if(pRod1.pAngle->hStartDatumRod==pRod2.pAngle->hStartDatumRod&&distOfS2S<distOfS2E&&distOfS2S<distOfE2S&&distOfS2S<distOfE2E)
				bS2S=true;
			else if(pRod1.pAngle->hStartDatumRod==pRod2.pAngle->hEndDatumRod&&distOfS2E<distOfS2S&&distOfS2E<distOfE2S&&distOfS2E<distOfE2E)
				bS2E=true;
			else if(pRod1.pAngle->hEndDatumRod==pRod2.pAngle->hStartDatumRod&&distOfE2S<distOfS2S&&distOfE2S<distOfS2E&&distOfE2S<distOfE2E)
				bE2S=true;
			else if(pRod1.pAngle->hEndDatumRod==pRod2.pAngle->hEndDatumRod&&distOfE2E<distOfS2S&&distOfE2E<distOfS2E&&distOfE2E<distOfE2S)
				bE2E=true;
		}
		if(bS2S)
		{
			distance=(pRod1->pStart!=pRod2->pStart)?distOfS2S:fabs(pRod1.pAngle->desStartPos.len_offset_dist-pRod2.pAngle->desStartPos.len_offset_dist);
			if(distance>0.5)
				hShareDependRod=pRod1.pAngle->hStartDatumRod;
		}
		else if(bS2E)
		{
			distance=(pRod1->pStart!=pRod2->pEnd)?distOfS2E:fabs(pRod1.pAngle->desStartPos.len_offset_dist-pRod2.pAngle->desEndPos.len_offset_dist);
			if(distance>0.5)
				hShareDependRod=pRod1.pAngle->hStartDatumRod;
		}
		else if(bE2S)
		{
			distance=(pRod1->pEnd!=pRod2->pStart)?distOfE2S:fabs(pRod1.pAngle->desEndPos.len_offset_dist-pRod2.pAngle->desStartPos.len_offset_dist);
			if(distance>0.5)
				hShareDependRod=pRod1.pAngle->hEndDatumRod;
		}
		else if(bE2E)
		{
			distance=(pRod1->pEnd!=pRod2->pEnd)?distOfE2S:fabs(pRod1.pAngle->desEndPos.len_offset_dist-pRod2.pAngle->desEndPos.len_offset_dist);
			if(distance>0.5)
				hShareDependRod=pRod1.pAngle->hEndDatumRod;
		}
		if(hShareDependRod>0x20)
			pMidRod=Ta.FromRodHandle(hShareDependRod);
	}

	double g=0;
	f3dLine axisline1,axisline2,midline;
	if(pRod1->IsAngle())
	{
		f3dPoint vWorkWingVec=pRod1.pAngle->GetWingVecY();
		if(fabs(pRod1.pAngle->vxWingNorm*dcs.axis_z)>fabs(pRod1.pAngle->vyWingNorm*dcs.axis_z))
		{
			g=pRod1.pAngle->GetWingXG(0);
			vWorkWingVec=pRod1.pAngle->GetWingVecX();
		}
		else
			g=pRod1.pAngle->GetWingYG(0);
		axisline1.startPt=pRod1.pAngle->Start()+vWorkWingVec*g;
		axisline1.endPt  =pRod1.pAngle->End()+vWorkWingVec*g;
	}
	else
	{
		axisline1.startPt=pRod1.pAngle->Start();
		axisline1.endPt  =pRod1.pAngle->End();
	}
	int hits=0;
	if(pMidRod.IsHasPtr())
	{
		if(pMidRod->IsAngle())
		{
			f3dPoint vWorkWingVec=pMidRod.pAngle->GetWingVecY();
			if(fabs(pMidRod.pAngle->vxWingNorm*dcs.axis_z)>fabs(pMidRod.pAngle->vyWingNorm*dcs.axis_z))
			{
				if(pPrevTipNode)
					g=pMidRod.pAngle->GetNodeWingXG(pPrevTipNode);
				else
					g=pMidRod.pAngle->GetWingXG(0);
				vWorkWingVec=pMidRod.pAngle->GetWingVecX();
			}
			else
				g=pMidRod.pAngle->GetWingYG(0);
			midline.startPt=pMidRod.pAngle->Start()+vWorkWingVec*g;
			midline.endPt  =pMidRod.pAngle->End()+vWorkWingVec*g;
		}
		else
		{
			midline.startPt=pMidRod->xPosStart;
			midline.endPt  =pMidRod->xPosEnd;
		}
		pBaseVertex->hDatumRod=pPrevRod->handle;
		pBaseVertex->hViceRod=pMidRod->handle;
		f3dPoint inters;
		axisline1.startPt-=((axisline1.startPt-dcs.origin)*dcs.axis_z)*dcs.axis_z;
		axisline1.endPt  -=((axisline1.endPt-dcs.origin)  *dcs.axis_z)*dcs.axis_z;
		midline.startPt-=((midline.startPt-dcs.origin)*dcs.axis_z)*dcs.axis_z;
		midline.endPt  -=((midline.endPt-dcs.origin)  *dcs.axis_z)*dcs.axis_z;
		if(Int3dpl(axisline1,midline,inters,1.0)>0)	//此处共面误差应容许有较大值，否则极易误判 wjh-2018.5.24
		{
			coord_trans(inters,dcs,FALSE,TRUE);
			inters*=m_fScale;
			inters.z=0;
			if(pBaseVertex)
				memcpy(&pBaseVertex->vertex,&inters.x,sizeof(double)*3);
			hits=1;
			pBaseVertex=pInsertVertex;
		}
		axisline1=midline;
		pBaseVertex->hDatumRod=pMidRod->handle;
		pBaseVertex->hViceRod=pNextRod->handle;
	}
	if(pRod2->IsAngle())
	{
		f3dPoint vWorkWingVec=pRod2.pAngle->GetWingVecY();
		if(fabs(pRod2.pAngle->vxWingNorm*dcs.axis_z)>fabs(pRod2.pAngle->vyWingNorm*dcs.axis_z))
		{
			g=pRod2.pAngle->GetWingXG(0);
			vWorkWingVec=pRod2.pAngle->GetWingVecX();
		}
		else
			g=pRod2.pAngle->GetWingYG(0);
		axisline2.startPt=pRod2.pAngle->Start()+vWorkWingVec*g;
		axisline2.endPt  =pRod2.pAngle->End()+vWorkWingVec*g;
	}
	f3dPoint inters;
	axisline1.startPt-=((axisline1.startPt-dcs.origin)*dcs.axis_z)*dcs.axis_z;
	axisline1.endPt  -=((axisline1.endPt-  dcs.origin)*dcs.axis_z)*dcs.axis_z;
	axisline2.startPt-=((axisline2.startPt-dcs.origin)*dcs.axis_z)*dcs.axis_z;
	axisline2.endPt  -=((axisline2.endPt  -dcs.origin)*dcs.axis_z)*dcs.axis_z;
	if(Int3dpl(axisline1,axisline2,inters,1.0)<=0)	//此处共面误差应容许有较大值，否则极易误判 wjh-2018.5.24
		return hits;
	coord_trans(inters,dcs,FALSE,TRUE);
	inters*=m_fScale;
	inters.z=0;
	if(pBaseVertex)
		memcpy(&pBaseVertex->vertex,&inters.x,sizeof(double)*3);
	hits++;
	return hits;
}
//0.表示无对称;1.X轴对称;2.Y轴对称;3.Z轴对称
int VerifyMirState(const double* point1,const double* point2,int verify_mirmode=0,double tolerance=0.001)
{
	if(verify_mirmode==0||verify_mirmode==1)
	{	//是否X轴对称
		if(fabs(point1[1]+point2[1])<tolerance&&fabs(point1[0]-point2[0])<tolerance&&fabs(point1[2]-point2[2])<tolerance)
			return 1;
	}
	if(verify_mirmode==0||verify_mirmode==2)
	{	//是否Y轴对称
		if(fabs(point1[0]+point2[0])<tolerance&&fabs(point1[1]-point2[1])<tolerance&&fabs(point1[2]-point2[2])<tolerance)
			return 2;
	}
	if(verify_mirmode==0||verify_mirmode==3)
	{	//是否Z轴对称
		if(fabs(point1[0]+point2[0])<tolerance&&fabs(point1[1]+point2[1])<tolerance&&fabs(point1[2]-point2[2])<tolerance)
			return 3;
	}
	return false;
}
bool IsDualMirSimPoly(IPreview::ISLAND* pSimPoly1,IPreview::ISLAND* pSimPoly2)
{
	if(pSimPoly1->arrayEdgeRods[2]==0||pSimPoly2->arrayEdgeRods[2]==0)
		return false;
	int i,j,nEdgeCount1=0,nEdgeCount2=0;
	for(i=0;i<8;i++)
	{
		if(pSimPoly1->arrayEdgeRods[i]>0x20)
			nEdgeCount1++;
		if(pSimPoly2->arrayEdgeRods[i]>0x20)
			nEdgeCount2++;
	}
	if(nEdgeCount1!=nEdgeCount2)
		return false;
	CLDSLinePart *pRod=NULL,*pMirRod=NULL;
	bool arrMatchRodStates[8]={false};
	for(i=0;i<nEdgeCount1;i++)
	{
		pRod=Ta.FromRodHandle(pSimPoly1->arrayEdgeRods[i]);
		if(pRod==NULL)
			return false;
		long hMirRod=0;
		for(RELATIVE_OBJECT* pRelaObj=pRod->relativeObjs.GetFirst();pRelaObj;pRelaObj=pRod->relativeObjs.GetNext())
		{
			if(pRelaObj->mirInfo.axis_flag!=1&&pRelaObj->mirInfo.axis_flag!=0x02&&pRelaObj->mirInfo.axis_flag!=0x04)
				continue;
			for(j=0;j<nEdgeCount2;j++)
			{
				if(!arrMatchRodStates[j]&&pSimPoly2->arrayEdgeRods[j]==pRelaObj->hObj)
				{
					hMirRod=pRelaObj->hObj;
					arrMatchRodStates[j]=true;
					break;
				}
			}
		}
		if(hMirRod==0)
		{
			for(j=0;j<nEdgeCount2;j++)
			{
				if(arrMatchRodStates[j])
					continue;
				if((pMirRod=Ta.FromRodHandle(pSimPoly2->arrayEdgeRods[j]))==NULL)
					return false;
				int iMirStyle=0;
				if((iMirStyle=VerifyMirState(pRod->pStart->xOriginalPos,pMirRod->pStart->xOriginalPos))>0)
				{
					if(VerifyMirState(pRod->pEnd->xOriginalPos,pMirRod->pEnd->xOriginalPos,iMirStyle)>0)
					{
						hMirRod=pMirRod->handle;
						arrMatchRodStates[j]=true;
						break;
					}
				}
				else if((iMirStyle=VerifyMirState(pRod->pStart->xOriginalPos,pMirRod->pEnd->xOriginalPos))>0)
				{
					if(VerifyMirState(pRod->pEnd->xOriginalPos,pMirRod->pStart->xOriginalPos,iMirStyle)>0)
					{
						hMirRod=pMirRod->handle;
						arrMatchRodStates[j]=true;
						break;
					}
				}
			}
		}
		if(hMirRod==0)	//只要存在一根杆件未对称匹配到，说明该两相似形就不是对称相似形，需要单独标注
			return false;
	}
	return true;
}
void CStruView::CreateSimPolyDimensions()			//智能提取添加相似形标注命令
{
	if(m_pAttachView==NULL||m_pAttachView->m_iViewType>=2)
		return;
	SmartPartPtr pPart;
	GECS ocs(dcs.origin,dcs.axis_x,dcs.axis_y,dcs.axis_z);
	preview->SetOCS(ocs,0.015);
	for(pPart=partSet.GetFirst();pPart.IsHasPtr();pPart=partSet.GetNext())
	{
		if(!pPart->IsLinePart()||pPart.pRod->pStart==NULL||pPart.pRod->pEnd==NULL)
			continue;
		preview->AppendLine(pPart.pRod->pStart->xPreliftPos,pPart.pRod->pEnd->xPreliftPos,pPart.pRod->handle);
	}
	preview->FlushPreview(0,0,0,0);
	if(preview->GetImageWidth()<3||preview->GetImageHeight()<3)
		return;	//位图太小，可能根本就不需相似形标注 wjh-2018.6.20
	//CXhChar16 szSegI;
	//if(this->m_pParentDrawing!=NULL)
	//	szSegI=((CDrawing*)this->m_pParentDrawing)->m_nSegI.ToString();
	//preview->WriteMonoBmpFile(CXhChar200("d:\\Seg<%s>#%s.bmp",(char*)szSegI,(char*)m_sName));
	CXhSimpleList<IPreview::ISLAND> listIslands;
	CDrawingCommand *pCmd=NULL;
	int count=preview->SearchIslands(&listIslands,5);
	if(count==0)
	{
		pCmd=cmdList.append();
		pCmd->m_pDrawingCom=this;
		pCmd->m_eCmdType=CMD_DIM_SIMPOLY;
		pCmd->InitCommandPara();
		pCmd->m_paraSimPoly.nSimPolyCmdType=0;	//点选拾取点添加相似形命令
	}
	else
	{
		IPreview::ISLAND *pIsland, *pMirIsland=NULL;
		SIMPLE_STACK<void*> stack;
		listIslands.AttachStack(&stack);
		//移除有对称关系的相似形
		for(pIsland=listIslands.EnumObjectFirst();pIsland;pIsland=listIslands.EnumObjectNext())
		{
			BOOL pushed=listIslands.push_stack();
			bool removeNowNode=false;
			for(pMirIsland=listIslands.EnumObjectNext();pMirIsland;pMirIsland=listIslands.EnumObjectNext())
			{
				if(IsDualMirSimPoly(pIsland,pMirIsland))
				{
					int diffx=(int)(pIsland->cx-pMirIsland->cx);
					int diffy=(int)(pIsland->cy-pMirIsland->cy);
					if(abs(diffx)<abs(diffy))
					{
						if(diffy>0)
							removeNowNode=true;
						else
							listIslands.DeleteCursor(false);
					}
					else
					{
						if(diffx>0)
							removeNowNode=true;
						else
							listIslands.DeleteCursor(false);
					}
				}
			}
			listIslands.pop_stack(pushed);
			if(removeNowNode)
				listIslands.DeleteCursor(false);
		}
		for(pIsland=listIslands.EnumObjectFirst();pIsland;pIsland=listIslands.EnumObjectNext())
		{
			pCmd=cmdList.append();
			pCmd->m_pDrawingCom=this;
			pCmd->m_eCmdType=CMD_DIM_SIMPOLY;
			pCmd->InitCommandPara();
			if(pIsland->arrayEdgeRods[2]==0)
			{
				pCmd->m_paraSimPoly.nSimPolyCmdType=2;	//提取相似形拾取点，并据此添加相似形命令
				GEPOINT pick=preview->TransPFromOcs(pIsland->cx,pIsland->cy);
				coord_trans(pick,dcs,FALSE,TRUE);
				pCmd->m_paraSimPoly.fPickPtX=pick.x*m_fScale;
				pCmd->m_paraSimPoly.fPickPtY=pick.y*m_fScale;
				pCmd->m_paraSimPoly.bPickPosIsLocalCoord=true;
			}
			else
			{
				pCmd->m_paraSimPoly.nSimPolyCmdType=3;	//提取相似形拾取点，并据此添加相似形命令
				SIMPOLY_VERTEX* pPolyVertex,xTmpInsertVertx;
				CLDSLinePart* pPrevRod=Ta.FromRodHandle(pIsland->arrayEdgeRods[0]),*pNextRod=NULL;
				bool error_occur=false;
				for(int i=1;i<8;i++)
				{
					pPolyVertex=pCmd->m_paraSimPoly.simPolyVertexList.append();
					pPolyVertex->hDatumRod=pIsland->arrayEdgeRods[i-1];
					bool endloop=(pIsland->arrayEdgeRods[i]==0);
					long hCurrRod=endloop?pIsland->arrayEdgeRods[0]:pIsland->arrayEdgeRods[i];
					pNextRod=hCurrRod>0x20 ? Ta.FromRodHandle(hCurrRod) : NULL;
					int count=CalDualRodIntersPoint(pPrevRod,pNextRod,pPolyVertex,&xTmpInsertVertx);
					if(pNextRod==NULL||count==0)
						pCmd->m_paraSimPoly.simPolyVertexList.DeleteCursor();
					if(count==2)
						pCmd->m_paraSimPoly.simPolyVertexList.append(xTmpInsertVertx);
					if(pNextRod)
						pPrevRod=pNextRod;
					if(endloop)
						break;
				}
				if(error_occur)
				{	//出错啦，暂退化为拾取点模式
					pCmd->m_paraSimPoly.nSimPolyCmdType=2;	//提取相似形拾取点，并据此添加相似形命令
					GEPOINT pick=preview->TransPFromOcs(pIsland->cx,pIsland->cy);
					coord_trans(pick,dcs,FALSE,TRUE);
					pCmd->m_paraSimPoly.fPickPtX=pick.x*m_fScale;
					pCmd->m_paraSimPoly.fPickPtY=pick.y*m_fScale;
					pCmd->m_paraSimPoly.bPickPosIsLocalCoord=true;
				}
				else
				{
					GEPOINT pick=preview->TransPFromOcs(pIsland->cx,pIsland->cy);
					coord_trans(pick,dcs,FALSE,TRUE);
					pCmd->m_paraSimPoly.fPickPtX=pick.x*m_fScale;
					pCmd->m_paraSimPoly.fPickPtY=pick.y*m_fScale;
					pCmd->m_paraSimPoly.bPickPosIsLocalCoord=true;
				}
			}
		}
	}
}
//生成螺栓垫圈标注命令列表
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
			continue;	//非螺栓
		if(pCmd->m_paraPart.para.bolt.front0_side1!=0)
			continue;	//非正面绘制螺栓
		CLDSBolt *pBolt=(CLDSBolt*)pCmd->m_paraPart.pPart;
		if(pBolt->DianQuan.N<=0||pBolt->DianQuan.thick<=0)
			continue;	//无垫圈
		boltSet.append(pBolt);
	}
	//添加垫圈标注命令
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

//绘制辅助线命令列表
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

void CStruView::ReDimBoltInfo()
{	//1.清空螺栓标注实体
	BOOL bDbEnt=FALSE;
	for(CDrawingCommand *pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_BOLTINFO)
			continue;
		if(pCmd->entIdList.GetNodeNum()>0)
			bDbEnt=TRUE;
		pCmd->EmptyEntList();
		cmdList.DeleteCursor(FALSE);
	}
	cmdList.Clean();
	//2.重新生成螺栓标注命令
	CreateDimBoltInfoCmdList();
	//3.重新执行命令
	if(bDbEnt)
	{
		for(CDrawingCommand *pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
		{
			if(pCmd->m_eCmdType==CMD_DIM_BOLTINFO)
				pCmd->Run();
		}
	}
}
//生成命令链表
BOOL CStruView::CreateCmdList(BOOL bInit/*=TRUE*/)
{
	hashAngleDrawInfo.Empty();
	//1.获取需要绘制的构件
	GetDrawingObjectSet(bInit);
	//2.生成相应命令列表
	//TODO:bInit=FALSE时应提前备份相似形标注，否则就丢掉了 wjh-2018.5.22
	cmdList.Empty();
	//2.1 绘制构件
	CreateDrawingPartCmdList();
	//2.2 绘制辅助线
	CreateDrawAssistLineCmdList();
	//2.3 标注正负头
	CreateDimOddmentCmdList();
	//2.4 标注构件明细
	CreateDimPartSpecCmdList();
	//2.5 标注螺栓特殊间距
	CreateDimSpecialLsSpaceCmdList();
	//2.6 标注角钢特殊准据
	CreateDimSpecialJgZhunJuCmdList();
	//2.7 标注螺栓信息
	CreateDimBoltInfoCmdList();
	//2.8 标注螺栓垫圈信息
	CreateDimBoltPadCmdList();
	//2.9 标注相似形
	//bInit==FALSE,不需要重新初始化相似形标注命令，否则在生成图元时会再次提示标注相似形，
	//如需标注相似形可手动点击添加进行标注 wht 13-04-19
	if(bInit)	
		CreateSimPolyDimensions();
	//2.10 标注脚钉、填板间距
	if(m_pAttachView&&m_pAttachView->m_iViewType==0)
	{
		CreateDimFootNailSpaceCmdList();	//仅需在图层视图中标注脚钉间距
		CreateDimFillPlateSpaceCmdList();	//仅需在图层视图中标注填板间距
	}
	if(bInit)	//3.计算视图初始装配位置
		CalInitAssemblyPosition();
	return TRUE;
}

//更新命令列表
void CStruView::UpdateCmdList(BOOL bUpdateViewDcs/*=FALSE*/)
{
	if(bUpdateViewDcs&&m_pAttachView)
		SetDCS(m_pAttachView->ucs);
	CHashList<CDrawingCommand> hashDrawPartList;		//key:pPart->handle
	CHashList<CDrawingCommand> hashDimOddmentList;		//key:pPart->handle
	CHashList<CDrawingCommand> hashPartSpecList;		//key:pPart->handle
	CStringKeyHashList<CDrawingCommand> hashLsSpaceList;//key:"startLs->handle endLs->handle"
	CStringKeyHashList<CDrawingCommand> hashJgZhunJuList;//key:"pJg->handle hRelaBolt"
	CStringKeyHashList<CDrawingCommand> hashBoltInfoList;//key:"螺栓句柄从小到大用#拼接的字符串"
	CHashList<CDrawingCommand> hashBoltPadList;			//key:pPart->handle 标注螺栓垫圈信息
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
				continue;	//无关联螺栓为旧版本的角钢准据
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
		else if(pCmd->m_eCmdType==CMD_DIM_BOLTINFO)
			pNewCmd=hashBoltInfoList.Add(pCmd->m_paraBoltGroupInfo.lsGroupDim.GetBoltIdKey());
		else 
			continue;
		pNewCmd->CopyProperty(pCmd);
	}
	//重新获得命令列表
	CreateCmdList(FALSE);
	//对比新生成的命令列表与原有命令列表,按顺序复制旧的命令参数值对应的命令中
	CDrawingCommand *pOldCmd=NULL;
	// 构件绘制命令
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		//相似形比例需要统一处理 wjh-2018.6.20
		//if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY)	
		//{	//根据当前设定更新相似形比例值
		//	pCmd->m_paraSimPoly.fScale=sys.dim_map.fSimTriScale;
		//}
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
				continue;	//绘制螺栓命令不需要复制原有命令参数(否则螺栓位置不能实时更新)
			}
			char cDrawFlag=pCmd->m_paraPart.cDrawFlag;	//构件绘制方式
			pCmd->CopyProperty(pOldCmd);
			pCmd->m_paraPart.cDrawFlag=cDrawFlag;		//更新已有构件,绘制方式
		}
	}
	// 正负头标注命令
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_ODDMENT)
			continue;
		pOldCmd=hashDimOddmentList.GetValue(pCmd->m_paraOddment.hLinePart);
		if(pOldCmd)
		{
			pCmd->CopyProperty(pOldCmd);
			//置为负数以便首次标注正负头时可以识别更新正负头标注方向　wjh-2011.9.17
			pCmd->m_paraOddment.fPoleAngle=-1.57;
		}
	}
	// 标注构件明细命令
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_PARTSPEC)
			continue;
		pOldCmd=hashPartSpecList.GetValue(pCmd->m_paraPartSpec.hObj);
		if(pOldCmd)
			pCmd->CopyProperty(pOldCmd);
	}
	//	角钢特殊准据
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_SPECIAL_JGZHUN)
			continue;
		char sKey[100]="";
		sprintf(sKey,"0X%X 0X%X",pCmd->m_paraDimJgZhun.hLineAngle,pCmd->m_paraDimJgZhun.zhun_dim.hRelaBolt);
		pOldCmd=hashJgZhunJuList.GetValue(sKey);
		if(pOldCmd)
		{	//仅复制用户可修该的变量,不复制zhun_dim,因为螺栓位置可能变化该数据需要使用最新数据 wht 11-08-10
			pCmd->m_paraDimJgZhun.bInside=pOldCmd->m_paraDimJgZhun.bInside;
			pCmd->m_paraDimJgZhun.fDimDistance=pOldCmd->m_paraDimJgZhun.fDimDistance;
			pCmd->m_paraDimJgZhun.fSpace=pOldCmd->m_paraDimJgZhun.fSpace;
		}
	}
	//  螺栓特殊间距
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_SPECIAL_LSSPACE)
			continue;
		char sKey[100]="";
		sprintf(sKey,"0X%X 0X%X",pCmd->m_paraDimLsSpace.spaceDim.hStartLs,pCmd->m_paraDimLsSpace.spaceDim.hEndLs);
		pOldCmd=hashLsSpaceList.GetValue(sKey);
		if(pOldCmd)
		{
			BOOL bExecute=pCmd->m_bExecute;
			pCmd->CopyProperty(pOldCmd);
			pCmd->m_bExecute=bExecute;
		}
	}	
	// 标注螺栓信息
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_BOLTINFO)
			continue;
		CXhChar500 sKey=pCmd->m_paraBoltGroupInfo.lsGroupDim.GetBoltIdKey();
		pOldCmd=hashBoltInfoList.GetValue(sKey);
		if(pOldCmd)
		{	//仅复制用户可修该的变量 wht 17-08-16
			//pCmd->CopyProperty(pOldCmd);
			pCmd->m_paraBoltGroupInfo.iDimPosType=pOldCmd->m_paraBoltGroupInfo.iDimPosType;
			pCmd->m_paraBoltGroupInfo.fDimPosOffsetX=pOldCmd->m_paraBoltGroupInfo.fDimPosOffsetX;
			pCmd->m_paraBoltGroupInfo.fDimPosOffsetY=pOldCmd->m_paraBoltGroupInfo.fDimPosOffsetY;
		}
	}
	// 垫圈标注命令
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_BOLTPAD)
			continue;
		pOldCmd=hashBoltPadList.GetValue(pCmd->m_paraBoltPad.hBolt);
		if(pOldCmd)
			pCmd->CopyProperty(pOldCmd);
	}
	// 脚钉间距标注命令
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_FOOTNAIL_SPACE)
			continue;
		pOldCmd=hashFootNailSpaceList.GetValue(pCmd->m_paraDimFootNailSpace.hLinePart);
		if(pOldCmd)
			pCmd->CopyProperty(pOldCmd);
	}
	// 填板间距标注命令
	for(pCmd=cmdList.GetFirst();pCmd;pCmd=cmdList.GetNext())
	{
		if(pCmd->m_eCmdType!=CMD_DIM_FILLPLATE_SPACE)
			continue;
		pOldCmd=hashFillPlateSpaceList.GetValue(pCmd->m_paraDimFillPlateSpace.hLinePart);
		if(pOldCmd)
			pCmd->CopyProperty(pOldCmd);
	}
	// 标注相似形命令直接复制原有相似形列表即可
	CDrawingCommand *pSimPolyCmd=NULL;
	for(pSimPolyCmd=simPolyList.GetFirst();pSimPolyCmd;pSimPolyCmd=simPolyList.GetNext())
	{
		pCmd=cmdList.append();
		pCmd->CopyProperty(pSimPolyCmd);
	}
}

//得到命令关联实体集合 wht 12-03-22
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
//根据构件句柄得到对应的绘制命令 wht 12-03-22
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

//从(除当前命令外的)螺栓标注命令中移除指定螺栓，并返回关联命令 
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
#ifdef AFX_TARG_ENU_ENGLISH
	CString strSpreadFace="0.Datum Plane|";
#else
	CString strSpreadFace="0.基准面|";
#endif
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
	{	//展开线列表
		CDrawingSpreadLine spreadLine;
		long i=0,nLineNum=0;
		buffer.ReadInteger(&nLineNum);
		for(i=0;i<nLineNum;i++)
			spreadLine.FromBuffer(buffer,version);
	}
	if(version>=1000022)
	{
		m_iMirMsg=buffer.ReadInteger();
		m_bDrawRefLine=buffer.ReadInteger();
	}
}

void CStruView::ToBuffer(CBuffer &buffer)
{
	CDrawingComponent::ToBuffer(buffer);
	//V1000022 新增件号标注设置	wht 17-08-08
	buffer.WriteInteger(m_iMirMsg);
	buffer.WriteInteger(m_bDrawRefLine);
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

//获得视图对应的实体集合
void CStruView::GetEntSet(ads_name &drag_ent_set,BOOL bSelectEnts/*=FALSE*/)
{
	//获取视图相关实体集
	CDrawingComponent::GetEntSet(drag_ent_set,bSelectEnts);
	//获取属于该视图的剖视图符实体集
	CSectionViewSymbol *pViewSymbol=NULL;
	long key=ParentDrawing()->listComponents.GetKey(this);
	for(pViewSymbol=ParentDrawing()->sectionViewSymbols.GetFirst();pViewSymbol;pViewSymbol=ParentDrawing()->sectionViewSymbols.GetNext())
	{
		if(pViewSymbol->hAttachViewKey!=key)
			continue;
		pViewSymbol->GetEntSet(drag_ent_set,2,bSelectEnts);
	}	
}

//将指定命令移动到目标命令之后
BOOL CStruView::MoveSpecCmdAfterDestCmd(CDrawingCommand *pSpecCmd,CDrawingCommand *pDestCmd)
{
	if(pSpecCmd==NULL||pDestCmd==NULL||
		pSpecCmd->GetDrawingSerialNo()<=0||
		pDestCmd->GetDrawingSerialNo()<=0||
		pSpecCmd->GetDrawingSerialNo()==pDestCmd->GetDrawingSerialNo())
		return FALSE;
	return MoveSpecCmdSerialNoAfterDestCmdSerialNo(pSpecCmd->GetDrawingSerialNo(),pDestCmd->GetDrawingSerialNo());
}
//将指定命令移动到目标命令之前
BOOL CStruView::MoveSpecCmdBeforeDestCmd(CDrawingCommand *pSpecCmd,CDrawingCommand *pDestCmd)
{
	if(pSpecCmd==NULL||pDestCmd==NULL||
		pSpecCmd->GetDrawingSerialNo()<=0||
		pDestCmd->GetDrawingSerialNo()<=0||
		pSpecCmd->GetDrawingSerialNo()==pDestCmd->GetDrawingSerialNo())
		return FALSE;
	return MoveSpecCmdSerialNoAfterDestCmdSerialNo(pSpecCmd->GetDrawingSerialNo(),pDestCmd->GetDrawingSerialNo()-1);
}

//将指定序号对应的命令调整至目的序号对应的命令之后
BOOL CStruView::MoveSpecCmdSerialNoAfterDestCmdSerialNo(int specSerialNo,int destSerialNo)
{
	CDrawingCommand *pCmd=NULL,*pSpecCmd=NULL;
	if(specSerialNo>destSerialNo)
	{	//指定序号大于目标序号，将>specSerialNo&&<destSerialNo的序号+1，将specSerialNo设置为destSerialNo+1
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
	{	//目标序号大于指定序号,将>specSerialNo&&<=destSerialNo的序号-1，将specserialNo设置为destSerialNo
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

//生成视图
void CPartDrawing::Create()
{
	DRAGSET.ClearEntSet();
	CDrawingCommand *pFirstCmd=cmdList.GetFirst();
	if(pFirstCmd==NULL)
		return;
	if(pFirstCmd->m_eCmdType!=CMD_DRAW_PART)
		return;
	if(m_bFirstCreate)
	{	//第一次生成，需要自动计算视图装配位置
		CalInitAssemblyPosition();
		m_bFirstCreate=FALSE;
	}
	//使用相对位置重新计算坐标系原点位置
	//CalAssemblyPosByRelativePos();
	//构件视图仅对应一条命令
	pFirstCmd->Run();
	//指定视图装配位置
	SpecifyAssemblyPosition();
	//标注视图名称
	InsertDrawingName();
	//插入图元后将图元矩形区域添加到绘图矩形框数组中
	UpdateDrawingRectArray();
	CStackAcadVar stackCmdEcho("CMDECHO",0);
#ifdef _ARX_2007
	ads_command(RTSTR,L"RE",RTNONE);	
#else
	ads_command(RTSTR,"RE",RTNONE);
#endif
}

//生成绘制零件图命令
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
	//更新零件图初始位置
	return TRUE;
}

//复制属性
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
	if(version==0||version>=1000024)
		buffer.ReadString(sPartNo,16);
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
	buffer.WriteString(sPartNo,16);
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
	m_eClassType=DRAW_COM_STDVIEW;	//视图类型
	m_fScale=1;
	m_pPart=NULL;
}

CStdView::~CStdView()
{
	
}

//生成视图
void CStdView::Create()
{
	DRAGSET.ClearEntSet();
	CDrawingCommand *pFirstCmd=cmdList.GetFirst();
	if(pFirstCmd==NULL || pFirstCmd->m_eCmdType!=CMD_DRAW_JG_JOINT)
		return;
	if(m_bFirstCreate)
	{	//第一次生成，需要自动计算视图装配位置
		CalInitAssemblyPosition();
		m_bFirstCreate=FALSE;
	}
	//构件视图仅对应一条命令
	pFirstCmd->Run();
	//指定视图装配位置
	SpecifyAssemblyPosition();
	//标注视图名称
	InsertDrawingName();
	//插入图元后将图元矩形区域添加到绘图矩形框数组中
	UpdateDrawingRectArray();
#ifdef _ARX_2007
	ads_command(RTSTR,L"RE",RTNONE);	
#else
	ads_command(RTSTR,"RE",RTNONE);
#endif
}
//生成命令链表
BOOL CStdView::CreateCmdList(BOOL bInit/*=TRUE*/)
{
	cmdList.Empty();
	if(m_eStdViewType==STD_VIEW_JOINT)
	{
		CDrawingCommand *pCommand=cmdList.append();
		pCommand->m_pDrawingCom=this;
		pCommand->m_eCmdType=CMD_DRAW_JG_JOINT;
		pCommand->InitCommandPara();
		pCommand->m_paraPart.pPart=m_pPart;
		pCommand->m_paraPart.hPart=m_pPart->handle;
	}
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
//剖视图符号
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
	bUpturned=FALSE;	//默认朝下
	viewName.Copy(name);
	startEntIds.Empty();
	endEntIds.Empty();
	hAttachViewKey=hViewKey=0;
	fLenght=sys.dim_map.fCutawayViewSymbolLen;
	fPolyLineWidth=sys.dim_map.fCutawayViewPolyLineWidth;
	fTextSize=sys.dim_map.fCutawayViewSymbolTextSize;
};	

//修改剖视图名称
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
			pText->setTextString((ACHAR*)_bstr_t(symbol_name));
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
			pText->setTextString((ACHAR*)_bstr_t(symbol_name));
		#else
			pText->setTextString(symbol_name);
		#endif
		}
		pEnt->close();
	}
	//更新界面
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
}

//删除相关的CAD实体
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
	//更新界面
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
}
//绘制剖视图符
void CSectionViewSymbol::Draw()
{
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	if(pBlockTableRecord==NULL)
		return;
	startEntIds.Empty();
	endEntIds.Empty();
	f3dPoint start_pt=startPt;
	f3dPoint end_pt=endPt;
	//计算剖视图参数
	f3dPoint line_vec,line_norm;
	line_vec=end_pt-start_pt;
	normalize(line_vec);	
	line_norm.Set(-line_vec.y,line_vec.x);
	if(!bUpturned)	//朝下
		line_norm*=-1.0;
	GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
	double symbol_len=fLenght;	//剖视图符号大小
	double text_rot_angle=Cal2dLineAng(f2dLine(f2dPoint(0,0),f2dPoint(line_norm.x,line_norm.y)))-0.5*Pi;
	if(text_rot_angle>=0.5*Pi)	//保证字体朝上
		text_rot_angle-=Pi;
	AcDbObjectId entId=0;
	//绘制剖视图符
	entId=CreateAcadPolyLine(pBlockTableRecord,start_pt,start_pt+symbol_len*line_vec,fPolyLineWidth);
	startEntIds.append(entId);	//始端
	entId=CreateAcadLine(pBlockTableRecord,start_pt,start_pt+symbol_len*line_norm);
	startEntIds.append(entId);	//始端
	entId=CreateAcadPolyLine(pBlockTableRecord,end_pt,end_pt-symbol_len*line_vec,fPolyLineWidth);
	endEntIds.append(entId);	//终端
	entId=CreateAcadLine(pBlockTableRecord,end_pt,end_pt+symbol_len*line_norm);
	endEntIds.append(entId);	//终端
	//标注剖视图名称
	f3dPoint dimPos=start_pt+line_vec*0.5*symbol_len+line_norm*0.5*symbol_len;
	entId=DimText(pBlockTableRecord,dimPos,viewName,TextStyleTable::hzfs.textStyleId,
		fTextSize,text_rot_angle,AcDb::kTextCenter,AcDb::kTextVertMid);
	startEntIds.append(entId);	//始端标注内容
	dimPos=end_pt-line_vec*0.5*symbol_len+line_norm*0.5*symbol_len;
	entId=DimText(pBlockTableRecord,dimPos,viewName,TextStyleTable::hzfs.textStyleId,
		fTextSize,text_rot_angle,AcDb::kTextCenter,AcDb::kTextVertMid);
	endEntIds.append(entId);	//终端标注内容
	//
	pBlockTableRecord->close();
	//更新界面
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
}

//更新剖视图符位置
void CSectionViewSymbol::UpdatePosition(double fOffsetX,double fOffsetY)
{
	startPt.x+=fOffsetX;
	startPt.y+=fOffsetY;
	endPt.x+=fOffsetX;
	endPt.y+=fOffsetY;
}

//得到绘图区域
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
	if(fabs(fabs(width)-200000)<EPS)	//空区域
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

//获得剖视图符对应的实体集合
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
				ads_redraw(ent_name,3);//高亮显示
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
				ads_redraw(ent_name,3);//高亮显示
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

//添加剖视图符
CSectionViewSymbol* CSectionViewSymbol::AddSectionViewSymbol(CStruView *pMainView,CStruView *pStruView)
{
	if(pStruView==NULL||pStruView->ParentDrawing()==NULL)	//||pMainView==NULL
		return FALSE;
	//输入剖视图起始点以及方向点
	ads_point start_pt,end_pt;
	f3dPoint startPt,endPt;
#ifdef AFX_TARG_ENU_ENGLISH
	CXhChar100 sPrompt("\n Please specify start point of [%s] sectional view symbol,<Enter Exit>:",&pStruView->m_sName);
#else
	CXhChar100 sPrompt("\n请指定[%s]剖视图符号的起始点,<Enter退出>:",&pStruView->m_sName);
#endif
	while(1)
	{
#ifdef _ARX_2007
		if(acedGetPoint(NULL,(ACHAR*)_bstr_t(sPrompt),start_pt)==RTNORM)
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
#ifdef AFX_TARG_ENU_ENGLISH
	sPrompt.Printf("\n Please specify end point of [%s] sectional view symbol,<Enter Exit>:",(char*)pStruView->m_sName);
#else
	sPrompt.Printf("\n请指定[%s]剖视图符号的终止点,<Enter退出>:",(char*)pStruView->m_sName);
#endif
	while(1)
	{
#ifdef _ARX_2007
		if(acedGetPoint(start_pt,(ACHAR*)_bstr_t(sPrompt),end_pt)==RTNORM)
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
	if(pStruView->m_sSectionViewCode.GetLength()<=0)
	{
		while(1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
			ACHAR sViewCode[50]={0};
			if(acedGetString(0,L"\n Please input a character,<Enter Exit>:\n",sViewCode)==RTNORM)
#else
			if(acedGetString(0,"\n Please input a character,<Enter Exit>:\n",pStruView->m_sSectionViewCode)==RTNORM)
#endif
#else
#ifdef _ARX_2007
			ACHAR sViewCode[50]={0};
			if(acedGetString(0,L"\n请输入一个字符,<Enter退出>:\n",sViewCode)==RTNORM)
#else
			if(acedGetString(0,"\n请输入一个字符,<Enter退出>:\n",pStruView->m_sSectionViewCode)==RTNORM)
#endif
#endif
			{
#ifdef _ARX_2007
				pStruView->m_sSectionViewCode.Copy(_bstr_t(sViewCode));
#endif
				break;
			}
		}
	}
	//将剖视图参数保存到主视图中
	CSectionViewSymbol view_symbol(startPt,endPt,pStruView->m_sSectionViewCode);
	CSectionViewSymbol *pViewSymbol=pStruView->ParentDrawing()->sectionViewSymbols.append(view_symbol);
	if(pMainView)	//保存剖视图关联视图key值,无主视图时需要标注完成后手动设置
		pViewSymbol->hAttachViewKey=pStruView->ParentDrawing()->listComponents.GetKey(pMainView);	
	pViewSymbol->hViewKey=pStruView->ParentDrawing()->listComponents.GetKey(pStruView);
	pViewSymbol->Draw();	//绘制剖视图符号
	return pViewSymbol;
}
//////////////////////////////////////////////////////////////////////////
//CJointWrapperView()
static f3dArcLine LineToArc(f3dLine &line)
{
	f3dArcLine arcline;
	arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),0);
	arcline.feature=line.feature;
	arcline.pen=line.pen;
	arcline.ID=line.ID;
	return arcline;
}
CJointWrapperView::CJointWrapperView()
{
	m_nGap=0;
	x_off_vec=y_off_vec=1;
	m_bInnerWrap=FALSE;
	m_bGroupJoint=FALSE;
}
CJointWrapperView::~CJointWrapperView()
{

}
//根据短角钢提取接头处的关联构件
BOOL CJointWrapperView::ExtractRelaParts(CLDSLineAngle* pShortJg,JOINT_RELA_PARTS& relaParts)
{
	if(pShortJg==NULL)
		return FALSE;
	relaParts.m_pShortJg=pShortJg;
	//查找始终端特征螺栓，提取依赖角钢及包板
	f3dPoint normX=pShortJg->vxWingNorm,normY=pShortJg->vyWingNorm;
	CMinDouble minXS,minXE,minYS,minYE;
	CLDSBolt *pXBoltS=NULL,*pXBoltE=NULL,*pYBoltS=NULL,*pYBoltE=NULL;
	for(CLsRef* pLsRef=pShortJg->GetFirstLsRef();pLsRef;pLsRef=pShortJg->GetNextLsRef())
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		double dds=DISTANCE(pBolt->ucs.origin,pShortJg->Start());
		double dde=DISTANCE(pBolt->ucs.origin,pShortJg->End());
		if(fabs(normX*pBolt->ucs.axis_z)>fabs(normY*pBolt->ucs.axis_z))
		{	//X肢螺栓
			if(minXS.Update(dds)==dds)
				pXBoltS=pBolt;
			if(minXE.Update(dde)==dde)
				pXBoltE=pBolt;
		}
		else
		{	//Y肢螺栓
			if(minYS.Update(dds)==dds)
				pYBoltS=pBolt;
			if(minYE.Update(dde)==dde)
				pYBoltE=pBolt;
		}
	}
	if(pXBoltS==NULL||pXBoltE==NULL||pYBoltS==NULL||pYBoltE==NULL)
		return FALSE;
	for(int i=0;i<2;i++)
	{	//提取短角钢始端关联构件
		CLDSBolt* pBolt=(i==0)?pXBoltS:pYBoltS;
		for(THICK_ITEM item=pBolt->GetL0ThickPara()->GetFirstItem();item.val!=0;item=pBolt->GetL0ThickPara()->GetNextItem())
		{
			if(!item.IsHandle() || abs(item.val)==pShortJg->handle)
				continue;
			CLDSPart *pPart=Ta.FromPartHandle(abs(item.val));
			if(pPart==NULL)
				continue;
			if(relaParts.m_pDependJgS && relaParts.m_pDependJgS->handle==pPart->handle)
				continue;
			if(relaParts.m_pDependJgS==NULL && pPart->GetClassTypeId()==CLS_LINEANGLE)
				relaParts.m_pDependJgS=(CLDSLineAngle*)pPart;
			CLDSPlate* pPlate=NULL;
			if(pPart->GetClassTypeId()==CLS_PLATE)
				pPlate=(CLDSPlate*)pPart;
			if(i==0 && pPlate)
			{
				if(relaParts.m_pXWrapPlate==NULL)
					relaParts.m_pXWrapPlate=(CLDSPlate*)pPart;
				else if(relaParts.m_pXPadPlate==NULL)
					relaParts.m_pXPadPlate=(CLDSPlate*)pPart;
			}
			else if(i==1 && pPlate)
			{
				if(relaParts.m_pYWrapPlate==NULL)
					relaParts.m_pYWrapPlate=(CLDSPlate*)pPart;
				else if(relaParts.m_pYPadPlate==NULL)
					relaParts.m_pYPadPlate=(CLDSPlate*)pPart;
			}
		}
	}
	if(relaParts.m_pDependJgS==NULL)
		return FALSE;
	if(relaParts.m_pXPadPlate&&relaParts.m_pYPadPlate)
		relaParts.m_bStartHasPad=TRUE;
	for(int i=0;i<2;i++)
	{	//提取短角钢终端关联构件
		CLDSBolt* pBolt=(i==0)?pXBoltE:pYBoltE;
		for(THICK_ITEM item=pBolt->GetL0ThickPara()->GetFirstItem();item.val!=0;item=pBolt->GetL0ThickPara()->GetNextItem())
		{
			if(!item.IsHandle() || abs(item.val)==pShortJg->handle)
				continue;
			CLDSPart *pPart=Ta.FromPartHandle(abs(item.val));
			if(pPart==NULL)
				continue;
			if(relaParts.m_pDependJgE && relaParts.m_pDependJgE->handle==pPart->handle)
				continue;
			if(relaParts.m_pDependJgE==NULL && pPart->GetClassTypeId()==CLS_LINEANGLE)
				relaParts.m_pDependJgE=(CLDSLineAngle*)pPart;
			CLDSPlate* pPlate=NULL;
			if(pPart->GetClassTypeId()==CLS_PLATE)
				pPlate=(CLDSPlate*)pPart;
			if(pPlate && !relaParts.m_bStartHasPad)
			{	//始端无垫板，判断终端是否有垫板
				if(i==0&&pPlate->handle!=relaParts.m_pXWrapPlate->handle)
					relaParts.m_pXPadPlate=pPlate;
				if(i==1&&pPlate->handle!=relaParts.m_pYWrapPlate->handle)
					relaParts.m_pYPadPlate=pPlate;
			}
			else if(pPlate && relaParts.m_bStartHasPad)
			{	//始端有垫板，根据终端螺栓确定正确的包板和垫板
				if(i==0&&pPlate->handle==relaParts.m_pXPadPlate->handle)
				{
					relaParts.m_pXPadPlate=relaParts.m_pXWrapPlate;
					relaParts.m_pXWrapPlate=pPlate;
				}
				if(i==1&&pPlate->handle==relaParts.m_pYPadPlate->handle)
				{
					relaParts.m_pYPadPlate=relaParts.m_pYWrapPlate;
					relaParts.m_pYWrapPlate=pPlate;
				}
			}
		}
	}
	if(relaParts.m_pDependJgE==NULL)
		return FALSE;
	if(relaParts.m_pXWrapPlate&&relaParts.m_pYPadPlate&&relaParts.m_bStartHasPad==FALSE)
		relaParts.m_bEndHasPad=TRUE;
	return TRUE;
}
//查找组合角钢接头中的另一短角钢
CLDSLineAngle* CJointWrapperView::FindOherShortJg(JOINT_RELA_PARTS& relaParts)
{
	CLDSGroupLineAngle* pGroupJg=NULL;
	if(relaParts.m_pDependJgS->group_father_jg_h>0)
		pGroupJg=(CLDSGroupLineAngle*)Ta.FromPartHandle(relaParts.m_pDependJgS->group_father_jg_h);
	if(pGroupJg&&pGroupJg->group_style==0)
	{	//对角组合角钢接头
		if(relaParts.m_pXWrapPlate&&relaParts.m_pXWrapPlate->GetLsCount()>0)
		{
			CLDSBolt* pBolt=relaParts.m_pXWrapPlate->GetFirstLsRef()->GetLsPtr();
			for(THICK_ITEM item=pBolt->GetL0ThickPara()->GetFirstItem();item.val!=0;item=pBolt->GetL0ThickPara()->GetNextItem())
			{
				if(!item.IsHandle() || abs(item.val)==m_pShortJg->handle)
					continue;
				CLDSPart *pPart=Ta.FromPartHandle(abs(item.val));
				if(pPart==NULL||pPart->GetClassTypeId()!=CLS_LINEANGLE)
					continue;
				CLDSLineAngle* pJg=(CLDSLineAngle*)pPart;
				if(pJg->pStart==NULL && pJg->pEnd==NULL)
					return pJg;
			}
		}
		if(relaParts.m_pYWrapPlate&&relaParts.m_pYWrapPlate->GetLsCount()>0)
		{
			CLDSBolt* pBolt=relaParts.m_pYWrapPlate->GetFirstLsRef()->GetLsPtr();
			for(THICK_ITEM item=pBolt->GetL0ThickPara()->GetFirstItem();item.val!=0;item=pBolt->GetL0ThickPara()->GetNextItem())
			{
				if(!item.IsHandle() || abs(item.val)==m_pShortJg->handle)
					continue;
				CLDSPart *pPart=Ta.FromPartHandle(abs(item.val));
				if(pPart==NULL||pPart->GetClassTypeId()!=CLS_LINEANGLE)
					continue;
				CLDSLineAngle* pJg=(CLDSLineAngle*)pPart;
				if(pJg->pStart==NULL && pJg->pEnd==NULL)
					return pJg;
			}
		}
	}
	return NULL;
}
void CJointWrapperView::BuildDrawCs()
{
	m_pShortJg->BuildUCS();
	CParaCS::PARAMETRIC_INFO param;
	//坐标原点
	param.desOrg.datum_pos_style=9;	//构件相对坐标
	param.desOrg.des_para.hPart=m_pShortJg->handle;
	param.desOrg.datum_pos.Set();
	//ZY轴
	param.m_eDatumPlane=CParaCS::DP_ZY;
	param.desAxisZ.norm_style=5;	//杆件特征方向
	param.desAxisZ.hVicePart=m_pShortJg->handle;
	param.desAxisZ.direction=0;		//始-终
	//
	param.desAxisY.norm_style=1;	//角钢肢法线
	param.desAxisY.hVicePart=m_pShortJg->handle;
	param.desAxisY.norm_wing=0;		//X肢
	param.desAxisY.direction=1;		//朝里
	//
	CParaCS cs;
	cs.SetParamInfo(param);
	cs.SetModifiedState();
	cs.UpdateACS(m_pShortJg->BelongModel());
	ucs=cs.GetACS();
}
void CJointWrapperView::UpdateOffVec(f3dPoint wingX_vec,f3dPoint wingY_vec)
{
	//计算角钢肢偏移方向
	if(fabs(wingX_vec*ucs.axis_x)>fabs(wingY_vec*ucs.axis_x))
	{	//坐标X轴与角钢X肢同向，坐标Y轴与角钢Y肢同向
		if(wingX_vec*ucs.axis_x<0)
			x_off_vec=-1;
		if(wingY_vec*ucs.axis_y<0)
			y_off_vec=-1;
	}
	else
	{	//坐标X轴与角钢Y肢同向，坐标Y轴与角钢X肢同向
		if(wingX_vec*ucs.axis_y<0)
			y_off_vec=-1;
		if(wingY_vec*ucs.axis_x<0)
			x_off_vec=-1;
	}
}
int CJointWrapperView::CalGapDist(JOINT_RELA_PARTS& relaParts)
{
	int nGapDist=0;
	CLDSLineAngle* pDependJg=NULL;
	if(!relaParts.m_bStartHasPad)
		pDependJg=relaParts.m_pDependJgS;
	else if(!relaParts.m_bEndHasPad)
		pDependJg=relaParts.m_pDependJgE;
	if(pDependJg==NULL)
		return nGapDist;
	double thick=m_pShortJg->Thick;
	if(m_bInnerWrap)
		thick=pDependJg->Thick;
	f3dPoint ber_pt=pDependJg->Start();
	coord_trans(ber_pt,ucs,FALSE);
	nGapDist=ftoi(fabs(ber_pt.x)-thick);
	return nGapDist;
}
void CJointWrapperView::CreateDrawing()
{
	JOINT_RELA_PARTS relaParts;
	if(!ExtractRelaParts(m_pShortJg,relaParts))
		return;
	CLDSLineAngle* pOtherShortJg=FindOherShortJg(relaParts);
	if(pOtherShortJg)
		m_bGroupJoint=TRUE;
	BuildDrawCs();
	f3dPoint ber_pt1=relaParts.m_pDependJgS->Start();
	f3dPoint ber_pt2=relaParts.m_pShortJg->Start();
	coord_trans(ber_pt1,ucs,FALSE);
	coord_trans(ber_pt2,ucs,FALSE);
	if(ber_pt1.x<ber_pt2.x)
		m_bInnerWrap=TRUE;
	else
		m_bInnerWrap=FALSE;
	m_nGap=CalGapDist(relaParts);	//计算相邻构件的间隙值
	UpdateOffVec(m_pShortJg->GetWingVecX(),m_pShortJg->GetWingVecY());
	f3dPoint orign;
	for(int i=0;i<2;i++)
	{
		CLDSLineAngle* pDependJg=NULL;
		if(i==0)
		{
			orign.Set();
			pDependJg=relaParts.m_pDependJgS;
		}
		else
		{
			orign.Set(4*m_pShortJg->GetWidth(),0,0);
			pDependJg=relaParts.m_pDependJgE;
		}
		//绘制角钢截面
		DrawLineAngleSection(m_pShortJg,orign);
		DrawLineAngleSection(pDependJg,orign,FALSE);
		//绘制钢板截面
		DrawPlateSection(relaParts.m_pXWrapPlate,orign);
		DrawPlateSection(relaParts.m_pYWrapPlate,orign);
		if((i==0&&relaParts.m_bStartHasPad)||(i==1&&relaParts.m_bEndHasPad))
		{
			DrawPlateSection(relaParts.m_pXPadPlate,orign,TRUE);
			DrawPlateSection(relaParts.m_pYPadPlate,orign,TRUE);	
		}
		//标注角钢螺栓
		double fThick=2;
		if(m_bInnerWrap&&relaParts.m_pXWrapPlate)
			fThick=relaParts.m_pXWrapPlate->Thick;
		else if(!m_bInnerWrap&&relaParts.m_pXPadPlate)
			fThick=relaParts.m_pXPadPlate->Thick;
		DimAngleLsSpace(m_pShortJg,pDependJg,orign,fThick);
	}
	//绘制对角组合角钢接头视图
	if(m_bGroupJoint)
	{
		CHashList<BOOL> hashPlate;
		if(relaParts.m_pXWrapPlate)
			hashPlate.SetValue(relaParts.m_pXWrapPlate->handle,TRUE);
		if(relaParts.m_pYWrapPlate)
			hashPlate.SetValue(relaParts.m_pYWrapPlate->handle,TRUE);
		JOINT_RELA_PARTS relaParts2;
		if(!ExtractRelaParts(pOtherShortJg,relaParts2))
			return;
		//计算偏移点
		CMinDouble off_X,off_Y;
		for(f3dArcLine* pArcLine=EnumFirstArc();pArcLine;pArcLine=EnumNextArc())
		{
			if(pArcLine->SectorAngle()>0)
				continue;
			f3dPoint line_vec=pArcLine->End()-pArcLine->Start();
			if(line_vec.mod()<50)
				continue;	//过滤厚度线
			line_vec.normalized();
			if(fabs(line_vec*f3dPoint(1,0,0))>fabs(line_vec*f3dPoint(0,1,0)))
			{	//水平直线
				off_Y.Update(pArcLine->Start().y);
				off_Y.Update(pArcLine->End().y);
			}
			else
			{	//竖向直线
				off_X.Update(pArcLine->Start().x);
				off_X.Update(pArcLine->End().x);
			}
		}
		f3dPoint off_pt(off_X.number,off_Y.number);
		pOtherShortJg->BuildUCS();
		ucs.origin=pOtherShortJg->ucs.origin;
		UpdateOffVec(pOtherShortJg->GetWingVecX(),pOtherShortJg->GetWingVecY());
		for(int i=0;i<2;i++)
		{
			CLDSLineAngle* pDependJg=NULL;
			if(i==0)
			{
				orign.Set(off_pt.x,off_pt.y);
				pDependJg=relaParts2.m_pDependJgS;
			}
			else
			{
				orign.Set(off_pt.x+4*m_pShortJg->GetWidth(),off_pt.y,0);
				pDependJg=relaParts2.m_pDependJgE;
			}
			orign.x+=x_off_vec*pDependJg->Thick;
			orign.y+=y_off_vec*pDependJg->Thick;
			//绘制角钢截面
			DrawLineAngleSection(relaParts2.m_pShortJg,orign);
			DrawLineAngleSection(pDependJg,orign,FALSE);
			//绘制钢板截面
			if(hashPlate.GetValue(relaParts2.m_pXWrapPlate->handle)==NULL)
				DrawPlateSection(relaParts2.m_pXWrapPlate,orign);
			if(hashPlate.GetValue(relaParts2.m_pYWrapPlate->handle)==NULL)
				DrawPlateSection(relaParts2.m_pYWrapPlate,orign);
			if((i==0&&relaParts2.m_bStartHasPad)||(i==1&&relaParts2.m_bEndHasPad))
			{
				DrawPlateSection(relaParts2.m_pXPadPlate,orign,TRUE);
				DrawPlateSection(relaParts2.m_pYPadPlate,orign,TRUE);	
			}
			//标注角钢螺栓
			double fThick=2;
			if(m_bInnerWrap&&relaParts2.m_pXWrapPlate)
				fThick=relaParts2.m_pXWrapPlate->Thick;
			else if(!m_bInnerWrap&&relaParts2.m_pXPadPlate)
				fThick=relaParts2.m_pXPadPlate->Thick;
			DimAngleLsSpace(pOtherShortJg,pDependJg,orign,fThick);
		}
	}
}
void CJointWrapperView::DrawLineAngleSection(CLDSLineAngle* pAngle,f3dPoint base_pnt,BOOL bShortJg/*=TRUE*/)
{
	double thick=pAngle->GetThick();
	double width=pAngle->GetWidth();
	f3dPoint ber_pt,axis_x_pt,axis_y_pt;
	if(bShortJg)
		ber_pt.Set(0,0);
	else
	{
		f3dPoint pt=pAngle->Start();
		coord_trans(pt,ucs,FALSE);
		int nOffDist=ftoi(fabs(pt.x)-m_nGap);
		if(m_bInnerWrap)	//内包
			ber_pt.Set(x_off_vec*(-nOffDist),y_off_vec*(-nOffDist));
		else	//外包
			ber_pt.Set(x_off_vec*nOffDist,y_off_vec*nOffDist);
	}
	axis_x_pt.Set(ber_pt.x+x_off_vec*width,ber_pt.y);
	axis_y_pt.Set(ber_pt.x,ber_pt.y+y_off_vec*width);
	f3dLine line;
	f3dArcLine arcLine;
	f3dPoint arc_start,arc_end,center_pick;
	line.pen.style=PS_SOLID;
	line.ID=0;
	//
	if((m_bInnerWrap&&bShortJg)||(!m_bInnerWrap&&!bShortJg))
	{//内包铁外楞需圆弧过渡
		datumPt3=ber_pt;
		datumPt4.Set(ber_pt.x+x_off_vec*thick,ber_pt.y+y_off_vec*thick);
		//
		line.startPt=axis_y_pt+base_pnt;
		line.endPt=f3dPoint(ber_pt.x,ber_pt.y+x_off_vec*thick)+base_pnt;
		edgeList.append(LineToArc(line));
		//
		arc_start=f3dPoint(ber_pt.x,ber_pt.y+x_off_vec*thick)+base_pnt;
		arc_end=f3dPoint(ber_pt.x+y_off_vec*thick,ber_pt.y)+base_pnt;
		center_pick=f3dPoint(arc_end.x,arc_start.y);
		arcLine.CreateMethod3(arc_start,arc_end,f3dPoint(0,0,x_off_vec*y_off_vec),thick,center_pick);
		edgeList.append(arcLine);
		//
		line.startPt=f3dPoint(ber_pt.x+y_off_vec*thick,ber_pt.y)+base_pnt;
		line.endPt=axis_x_pt+base_pnt;
		edgeList.append(LineToArc(line));
	}
	else
	{//
		datumPt1=ber_pt;
		datumPt2.Set(ber_pt.x+x_off_vec*thick,ber_pt.y+y_off_vec*thick);
		//
		line.startPt=axis_y_pt+base_pnt;
		line.endPt=ber_pt+base_pnt;
		edgeList.append(LineToArc(line));
		//
		line.startPt=ber_pt+base_pnt;
		line.endPt=axis_x_pt+base_pnt;
		edgeList.append(LineToArc(line));
	}
	//
	arc_start=axis_x_pt+base_pnt;
	arc_end=f3dPoint(axis_x_pt.x-y_off_vec*thick,axis_x_pt.y+x_off_vec*thick)+base_pnt;
	center_pick=f3dPoint(axis_x_pt.x-y_off_vec*thick,axis_x_pt.y)+base_pnt;
	arcLine.CreateMethod3(arc_start,arc_end,f3dPoint(0,0,x_off_vec*y_off_vec),thick,center_pick);
	edgeList.append(arcLine);
	//
	line.startPt=f3dPoint(axis_x_pt.x-y_off_vec*thick,axis_x_pt.y+x_off_vec*thick)+base_pnt;
	line.endPt=f3dPoint(ber_pt.x+2*y_off_vec*thick,ber_pt.y+x_off_vec*thick)+base_pnt;
	edgeList.append(LineToArc(line));
	//
	arc_start=f3dPoint(ber_pt.x+2*y_off_vec*thick,ber_pt.y+x_off_vec*thick)+base_pnt;
	arc_end=f3dPoint(ber_pt.x+y_off_vec*thick,ber_pt.y+x_off_vec*2*thick)+base_pnt;
	center_pick=f3dPoint(arc_start.x,arc_end.y);
	arcLine.CreateMethod3(arc_start,arc_end,f3dPoint(0,0,-1*x_off_vec*y_off_vec),thick,center_pick);
	edgeList.append(arcLine);
	//
	line.startPt=f3dPoint(ber_pt.x+y_off_vec*thick,ber_pt.y+x_off_vec*2*thick)+base_pnt;
	line.endPt=f3dPoint(axis_y_pt.x+y_off_vec*thick,axis_y_pt.y-x_off_vec*thick)+base_pnt;
	edgeList.append(LineToArc(line));
	//
	arc_start=f3dPoint(axis_y_pt.x+y_off_vec*thick,axis_y_pt.y-x_off_vec*thick)+base_pnt;
	arc_end=axis_y_pt+base_pnt;
	center_pick=f3dPoint(axis_y_pt.x,axis_y_pt.y-x_off_vec*thick)+base_pnt;
	arcLine.CreateMethod3(arc_start,arc_end,f3dPoint(0,0,x_off_vec*y_off_vec),thick,center_pick);
	edgeList.append(arcLine);
	//添加数据点
	CTextFrame data_point;
	data_point.InitTextFrame(sys.dim_map.PartNo.fPartNoTextSize,sys.dim_map.PartNo.fPartNoCirD);
	data_point.cls_id=pAngle->GetClassTypeId();
	data_point.hPart=pAngle->handle;
	if((m_bInnerWrap&&bShortJg)||(!m_bInnerWrap&&!bShortJg))
	{	//内侧
		data_point.dataPos=f3dPoint(ber_pt.x+y_off_vec*thick,ber_pt.y+x_off_vec*thick)+base_pnt;
		data_point.dimPos.Set(data_point.dataPos.x+y_off_vec*3*thick,data_point.dataPos.y+x_off_vec*3*thick);
	}
	else
	{	//外侧
		if(m_bGroupJoint)
		{
			data_point.dataPos=f3dPoint(ber_pt.x+x_off_vec*thick*0.5,ber_pt.y+y_off_vec*(width-thick))+base_pnt;
			data_point.dimPos.Set(data_point.dataPos.x+y_off_vec*3*thick,data_point.dataPos.y+x_off_vec*3*thick);
		}
		else
		{
			data_point.dataPos=ber_pt+base_pnt;
			data_point.dimPos.Set(data_point.dataPos.x-y_off_vec*3*thick,data_point.dataPos.y-x_off_vec*3*thick);
		}
	}
	data_point.BuildFrame(GetPartNoIncMat(pAngle->GetPartNo(),pAngle->cMaterial),NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
	dataPointList.append(data_point);
}
void CJointWrapperView::DrawPlateSection(CLDSPlate* pPlate,f3dPoint base_pnt,BOOL bPad/*=FALSE*/)
{
	if(pPlate==NULL)
		return;
	double dd1=pPlate->ucs.axis_z*ucs.axis_x;
	double dd2=pPlate->ucs.axis_z*ucs.axis_y;
	BOOL bInAxisX=TRUE;
	if(fabs(dd1)>fabs(dd2))
		bInAxisX=FALSE;
	f3dPoint fPtS,fPtE;
	for(int i=0;i<pPlate->vertex_list.GetNodeNum();i++)
	{
		f3dPoint vertex=pPlate->vertex_list[i].vertex;
		coord_trans(vertex,pPlate->ucs,TRUE);
		coord_trans(vertex,ucs,FALSE);
		vertex.z=0;
		if(i==0)
		{
			fPtS=fPtE=vertex;
			continue;
		}
		if(bInAxisX)
		{
			if(vertex.x<fPtS.x)
				fPtS=vertex;
			if(vertex.x>fPtE.x)
				fPtE=vertex;
		}
		else
		{
			if(vertex.y<fPtS.y)
				fPtS=vertex;
			if(vertex.y>fPtE.y)
				fPtE=vertex;
		}
	}
	double fThick=pPlate->GetThick();
	double fStart=bInAxisX?fPtS.x:fPtS.y;
	double fEnd=bInAxisX?fPtE.x:fPtE.y;
	f3dPoint pt[4];
	if(bPad)
	{	//垫板
		if(bInAxisX)
		{
			pt[0].Set(fStart,datumPt2.y);
			pt[1].Set(fEnd,datumPt2.y);
			pt[2].Set(fEnd,datumPt3.y);
			pt[3].Set(fStart,datumPt3.y);
		}
		else
		{
			pt[0].Set(datumPt2.x,fStart);
			pt[1].Set(datumPt3.x,fStart);
			pt[2].Set(datumPt3.x,fEnd);
			pt[3].Set(datumPt2.x,fEnd);
		}
	}
	else
	{	//包板
		f3dPoint datumPt;
		if(m_bInnerWrap)
			datumPt=datumPt1;	//外包板
		else
			datumPt=datumPt4;	//内包板
		if(bInAxisX)
		{
			pt[0].Set(fStart,datumPt.y);
			pt[1].Set(fEnd,datumPt.y);
			pt[2].Set(fEnd,datumPt.y-y_off_vec*fThick);
			pt[3].Set(fStart,datumPt.y-y_off_vec*fThick);
		}
		else
		{
			pt[0].Set(datumPt.x,fStart);
			pt[1].Set(datumPt.x,fEnd);
			pt[2].Set(datumPt.x-x_off_vec*fThick,fEnd);
			pt[3].Set(datumPt.x-x_off_vec*fThick,fStart);
		}
	}
	//添加轮廓边
	f3dLine line;
	line.pen.style=PS_SOLID;
	line.ID=0;
	for(int i=0;i<4;i++)
	{
		line.startPt=pt[i]+base_pnt;
		line.endPt=pt[(i+1)%4]+base_pnt;
		edgeList.append(LineToArc(line));
	}
	//添加数据点
	CTextFrame data_point;
	data_point.InitTextFrame(sys.dim_map.PartNo.fPartNoTextSize,sys.dim_map.PartNo.fPartNoCirD);
	data_point.cls_id=pPlate->GetClassTypeId();
	data_point.hPart=pPlate->handle;
	if(bPad)
	{	//垫板
		f3dPoint pt_S=(pt[0]+pt[3])*0.5;
		f3dPoint pt_E=(pt[1]+pt[2])*0.5;
		data_point.dataPos=(pt_S+pt_E)*0.5+base_pnt;
		if(bInAxisX)
			data_point.dimPos.Set(data_point.dataPos.x,data_point.dataPos.y+x_off_vec*(2*fThick+m_pShortJg->GetThick()));
		else
			data_point.dimPos.Set(data_point.dataPos.x+y_off_vec*(2*fThick+m_pShortJg->GetThick()),data_point.dataPos.y);
	}
	else
	{	//包板
		if(x_off_vec==1)
			data_point.dataPos=(pt[1]+pt[2])*0.5+base_pnt;
		else
			data_point.dataPos=(pt[0]+pt[3])*0.5+base_pnt;
		if(bInAxisX)
			data_point.dimPos.Set(data_point.dataPos.x+y_off_vec*3*fThick,data_point.dataPos.y);
		else
			data_point.dimPos.Set(data_point.dataPos.x,data_point.dataPos.y+x_off_vec*3*fThick);
	}
	data_point.BuildFrame(GetPartNoIncMat(pPlate->GetPartNo(),pPlate->cMaterial),NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
	dataPointList.append(data_point);
}
void CJointWrapperView::DimAngleLsSpace(CLDSLineAngle* pShortJg,CLDSLineAngle* pAngle,f3dPoint base_pnt,double plank_thick/*=2*/)
{
	if(pShortJg->handle==pAngle->handle)
		return;	//短角钢不处理螺栓标注
	double fX1=0,fX2=0,fY1=0,fY2=0;
	for(CLsRef* pLsRef=pShortJg->GetFirstLsRef();pLsRef;pLsRef=pShortJg->GetNextLsRef())
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		if(pAngle->FindLsByHandle(pBolt->handle)==NULL)
			continue;
		f3dPoint inters,ls_pos=pBolt->ucs.origin,ls_norm=pBolt->ucs.axis_z;
		double dd1=fabs(ls_norm*pAngle->vxWingNorm);
		double dd2=fabs(ls_norm*pAngle->vyWingNorm);
		if(dd1>dd2)
		{	//X肢螺栓
			Int3dlf(inters,ls_pos,ls_norm,pAngle->Start(),pAngle->vxWingNorm);
			double fDist=DistOf3dPtLine(inters,pAngle->Start(),pAngle->End());
			if(fX1==0)
				fX1=fX2=fDist;
			if(fDist<fX1)
				fX1=fDist;
			if(fDist>fX2)
				fX2=fDist;
		}
		else
		{	//Y肢螺栓
			Int3dlf(inters,ls_pos,ls_norm,pAngle->Start(),pAngle->vyWingNorm);
			double fDist=DistOf3dPtLine(inters,pAngle->Start(),pAngle->End());
			if(fY1==0)
				fY1=fY2=fDist;
			if(fDist<fY1)
				fY1=fDist;
			if(fDist>fY2)
				fY2=fDist;
		}
	}
	double fDist=0;
	if(fabs(pAngle->GetWingVecX()*ucs.axis_x)<fabs(pAngle->GetWingVecY()*ucs.axis_x))
	{
		fDist=fX1;fX1=fY1;fY1=fDist;
		fDist=fX2;fX2=fY2;fY2=fDist;
	}
	f3dPoint datumPt;
	if(m_bInnerWrap)
	{
		datumPt=datumPt1;
		fDist=plank_thick;
	}
	else
	{	
		datumPt=datumPt3;
		fDist=pShortJg->Thick;
		fDist+=plank_thick;
	}
	fDist+=pAngle->Thick;
	//绘制螺栓线
	f3dLine line;
	line.pen.style=PS_SOLID;
	line.ID=0;
	line.startPt=f3dPoint(datumPt.x+x_off_vec*fX1,datumPt.y-y_off_vec*fDist)+base_pnt;
	line.endPt = f3dPoint(datumPt.x+x_off_vec*fX1,datumPt.y+y_off_vec*fDist)+base_pnt;
	edgeList.append(LineToArc(line));
	if(fX2-fX1>2)
	{
		line.startPt=f3dPoint(datumPt.x+x_off_vec*fX2,datumPt.y-y_off_vec*fDist)+base_pnt;
		line.endPt = f3dPoint(datumPt.x+x_off_vec*fX2,datumPt.y+y_off_vec*fDist)+base_pnt;
		edgeList.append(LineToArc(line));
	}
	//
	line.startPt=f3dPoint(datumPt.x-x_off_vec*fDist,datumPt.y+y_off_vec*fY1)+base_pnt;
	line.endPt = f3dPoint(datumPt.x+x_off_vec*fDist,datumPt.y+y_off_vec*fY1)+base_pnt;
	edgeList.append(LineToArc(line));
	if(fY2-fY1>2)
	{
		line.startPt=f3dPoint(datumPt.x-x_off_vec*fDist,datumPt.y+y_off_vec*fY2)+base_pnt;
		line.endPt = f3dPoint(datumPt.x+x_off_vec*fDist,datumPt.y+y_off_vec*fY2)+base_pnt;
		edgeList.append(LineToArc(line));
	}
	//标注螺栓间距--标注X轴螺栓
	CSizeTextDim space_dim;
	double fdimPosY=datumPt.y-y_off_vec*fDist;
	space_dim.dimStartPos=f3dPoint(datumPt.x,fdimPosY)+base_pnt;
	space_dim.dimEndPos = f3dPoint(datumPt.x+x_off_vec*fX1,fdimPosY)+base_pnt;
	space_dim.dist=DISTANCE(space_dim.dimStartPos,space_dim.dimEndPos);
	space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
	space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
	spaceDimList.append(space_dim);
	if(fX2-fX1>2)
	{
		space_dim.dimStartPos=f3dPoint(datumPt.x+x_off_vec*fX1,fdimPosY)+base_pnt;
		space_dim.dimEndPos = f3dPoint(datumPt.x+x_off_vec*fX2,fdimPosY)+base_pnt;
		space_dim.dist=DISTANCE(space_dim.dimStartPos,space_dim.dimEndPos);
		space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
		space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
		spaceDimList.append(space_dim);
	}
	space_dim.dimStartPos=f3dPoint(datumPt.x+x_off_vec*fX2,fdimPosY)+base_pnt;
	space_dim.dimEndPos = f3dPoint(datumPt.x+x_off_vec*pAngle->GetWidth(),fdimPosY)+base_pnt;
	space_dim.dist=DISTANCE(space_dim.dimStartPos,space_dim.dimEndPos);
	space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
	space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
	spaceDimList.append(space_dim);
	//标注Y轴螺栓
	double fdimPosX=datumPt1.x-x_off_vec*(fDist+4);
	space_dim.dimStartPos=f3dPoint(fdimPosX,datumPt.y)+base_pnt;
	space_dim.dimEndPos = f3dPoint(fdimPosX,datumPt.y+y_off_vec*fY1)+base_pnt;
	space_dim.dist=DISTANCE(space_dim.dimStartPos,space_dim.dimEndPos);
	space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
	space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
	spaceDimList.append(space_dim);
	if(fY2-fY1>2)
	{
		space_dim.dimStartPos=f3dPoint(fdimPosX,datumPt.y+y_off_vec*fY1)+base_pnt;
		space_dim.dimEndPos = f3dPoint(fdimPosX,datumPt.y+y_off_vec*fY2)+base_pnt;
		space_dim.dist=DISTANCE(space_dim.dimStartPos,space_dim.dimEndPos);
		space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
		space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
		spaceDimList.append(space_dim);
	}
	space_dim.dimStartPos=f3dPoint(fdimPosX,datumPt.y+y_off_vec*fY2)+base_pnt;
	space_dim.dimEndPos = f3dPoint(fdimPosX,datumPt.y+y_off_vec*pAngle->GetWidth())+base_pnt;
	space_dim.dist=DISTANCE(space_dim.dimStartPos,space_dim.dimEndPos);
	space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
	space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
	spaceDimList.append(space_dim);
}
//////////////////////////////////////////////////////////////////////////
//初始化图纸列表
void InitDrawingList()
{
	if(drawingList.CreateNewAtom==NULL)
		return;	//未打开文件时CreateNewAtom为NULL
	if(drawingList.GetNodeNum()<=0)
	{
		CDrawing *pDrawing=NULL;
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)||defined(__TSA_FILE_)
		//司令图
#ifdef AFX_TARG_ENU_ENGLISH
		PutMessage(NULL,"Initializing Force Map......");
#else
		PutMessage(NULL,"正在初始化司令图......");
#endif
		pDrawing=drawingList.Add(0);
		pDrawing->m_bAutoLayoutComponent=TRUE;	//自动布局
		pDrawing->m_hActiveModule=Ta.m_hActiveModule;
		pDrawing->InitDrawing(CDrawing::DARWING_MECHANICS);	//司令图
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(pDrawing->m_sDrawingName,"Force Map");
#else
		strcpy(pDrawing->m_sDrawingName,"司令图");
#endif
#endif
		//总图
#ifdef AFX_TARG_ENU_ENGLISH
		PutMessage(NULL,"Initializing Sum Map.........");
#else
		PutMessage(NULL,"正在初始化总图.........");
#endif
		pDrawing=drawingList.Add(0);
		pDrawing->m_bAutoLayoutComponent=TRUE;	//自动布局
		pDrawing->m_hActiveModule=Ta.m_hActiveModule;
		pDrawing->InitDrawing(CDrawing::DARWING_SUMMARY);	//总图
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(pDrawing->m_sDrawingName,"Sum  Map");
#else
		strcpy(pDrawing->m_sDrawingName,"总  图");
#endif
		//分段结构图
		ATOM_LIST<SEGI> segNoList;
		int nSegNum=Ta.GetSegNoList(segNoList);
		for(SEGI *pSegI=segNoList.GetFirst();pSegI;pSegI=segNoList.GetNext())
		{	
			pDrawing=drawingList.Add(0);
#ifdef AFX_TARG_ENU_ENGLISH
			PutMessage(NULL,"Initializing %s segment structure map...",(char*)pSegI->ToString());
			pDrawing->InitDrawing(*pSegI);
			pDrawing->m_sDrawingName.Printf("%s segment",(char*)pSegI->ToString());
#else
			PutMessage(NULL,"正在初始化%s段结构图...",(char*)pSegI->ToString());
			pDrawing->InitDrawing(*pSegI);
			pDrawing->m_sDrawingName.Printf("%s段",(char*)pSegI->ToString());
#endif
		}
		PutMessage(NULL,NULL);
	}
}

//因修改构件段号,新增或减少分段时应更新分段结构图命令列表
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
		if(pSeg)	//存在对应段，需更新命令列表
		{
			pDrawing->UpdateCmdList();
			//设置已存在对应图纸的段hash值为负的段号
			if(*pSeg>0)
				segHashList.SetValue(pDrawing->m_nSegI,(*pSeg)*-1); 
			else 
				segHashList.SetValue(pDrawing->m_nSegI,-1); 
		}
		else		//已不存在对应段，需删除对应的分段结构图
			drawingList.DeleteCursor();
	}
	SEGI *pSeg=NULL;
	for(pSeg=segHashList.GetFirst();pSeg;pSeg=segHashList.GetNext())
	{
		if(*pSeg<0)
			continue;
		//不存在对应分段结构图的新增段号,应在此处添加对应的分段结构图
		pDrawing=drawingList.Add(0);
		pDrawing->InitDrawing(*pSeg);
#ifdef AFX_TARG_ENU_ENGLISH
		pDrawing->m_sDrawingName.Printf("%s segment",(char*)pSeg->ToString());
#else
		pDrawing->m_sDrawingName.Printf("%s段",(char*)pSeg->ToString());
#endif
	}
}
#endif
