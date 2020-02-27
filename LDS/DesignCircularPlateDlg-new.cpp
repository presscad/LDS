// DesignCircularPlateDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "lds.h"
#include "GlobalFunc.h"
#include "DesignVectorPropDlg.h"
#include "DesignCircularPlateDlg.h"
#include "env_def.h"
#include "XhCharString.h"
#include "LmaDlg.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "PartsDbDlg.h"
#include "PropertyListOper.h"
#include "btc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////
//回调函数及静态函数处理
static void UpdatePropItemName(CPropertyList *pPropList,CPropTreeItem *pItem,int norm_style=0)
{
	CDesignCircularPlateDlg *pCirDlg=(CDesignCircularPlateDlg*)pPropList->GetParent();
	if(pCirDlg==NULL)
		return;
	if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("para.S"))
	{
		if(pCirDlg->m_iPlateType==1)		//环向封板
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropName = "Sealing Plate Position";
			pItem->m_lpNodeInfo->m_strPropHelp = "Sealing plate's position";
			pItem->m_lpNodeInfo->m_cmbItems="Up Sealing Plate|Down Sealing Plate|Up&Down Sealing Plate";
#else 
			pItem->m_lpNodeInfo->m_strPropName = "封板位置";
			pItem->m_lpNodeInfo->m_strPropHelp = "封板位置";
			pItem->m_lpNodeInfo->m_cmbItems="上封板|下封板|上下封板";
#endif
		}
		else if(pCirDlg->m_iPlateType<6)	//环向肋板
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropName="Position Offset S";
			pItem->m_lpNodeInfo->m_strPropHelp="Normal offset include normal offset along the plate's Z axis offset，vertical offset is along tube's direction offset(The same direction with Z axis).";
			if(nPlateType==0)
				pItem->m_lpNodeInfo->m_strPropHelp +="To support circular plate's offset,but need to choose one connect plate(select on sealing plate model).";
#else 
			pItem->m_lpNodeInfo->m_strPropName="位置偏移S";
			pItem->m_lpNodeInfo->m_strPropHelp="位置偏移包括法向偏移即沿钢板Z轴方向的偏移，纵向偏移即沿主管延伸方向(且与Z轴同向的方向)的偏移。";
			if(pCirDlg->m_iPlateType==0)
				pItem->m_lpNodeInfo->m_strPropHelp +="支持计算环向板偏移量，但需要选择一块关联基板(在封板模式下选择)。";
#endif
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("para.A"))
	{
		if(pCirDlg->m_iPlateType==0 && pCirDlg->ConnVecList.GetCount(TRUE)<=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropName= "Sector Start Angle α";
			pItem->m_lpNodeInfo->m_strPropHelp= "The start angle α°of sector circular plate";
#else 
			pItem->m_lpNodeInfo->m_strPropName= "扇形起始角α";
			pItem->m_lpNodeInfo->m_strPropHelp= "扇形环向板的起始角α°";
#endif
		}
		else if(pCirDlg->m_iPlateType==1&&pCirDlg->para.nCirCoverPlateType==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropName= "Locate Angle α";
			pItem->m_lpNodeInfo->m_strPropHelp= "Locate Angle α°";
#else 
			pItem->m_lpNodeInfo->m_strPropName= "定位角α";
			pItem->m_lpNodeInfo->m_strPropHelp= "定位角α°";
#endif
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("para.D"))
	{
		if(pCirDlg->m_iPlateType==0||pCirDlg->m_iPlateType==2||pCirDlg->m_iPlateType==4)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Circular Band Width D";
			lpInfo->m_strPropHelp = "Circular Band Width D";
#else 
			pItem->m_lpNodeInfo->m_strPropName = "自由边距D";
			pItem->m_lpNodeInfo->m_strPropHelp = "自由边距D";
#endif
		}
		else if(pCirDlg->m_iPlateType==3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropName = "Circular Band Width D";
			pItem->m_lpNodeInfo->m_strPropHelp = "Circular Band Width D";
#else 
			pItem->m_lpNodeInfo->m_strPropName = "环形带宽D";
			pItem->m_lpNodeInfo->m_strPropHelp = "环形带宽D";
#endif
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("para.W"))
	{
		if(pCirDlg->m_iPlateType==1||pCirDlg->m_iPlateType==5)	//封板
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropName = "Circular Band Width W";
			pItem->m_lpNodeInfo->m_strPropHelp = "Circular Band Width W";
#else 
			pItem->m_lpNodeInfo->m_strPropName = "环形带宽W";
			pItem->m_lpNodeInfo->m_strPropHelp = "环形带宽W";
#endif
		}
		else if(pCirDlg->m_iPlateType==2||pCirDlg->m_iPlateType==3||pCirDlg->m_iPlateType==4)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropName = "Parameter W";
			pItem->m_lpNodeInfo->m_strPropHelp = "The width W of first connect plate's brace edge";
#else 
			pItem->m_lpNodeInfo->m_strPropName = "参数W";
			pItem->m_lpNodeInfo->m_strPropHelp = "第一块关联板支撑边宽W";
#endif
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("para.nDHRelativeType"))
	{
		if(pCirDlg->m_iPlateType==2)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_cmbItems="0.Independent Parameter|1.Relative&Enable D|2.Relative&Enable H"; 
			pItem->m_lpNodeInfo->m_strPropName = "D&H Relation";
			pItem->m_lpNodeInfo->m_strPropHelp = "Enable Parameter D Or H.";
#else 
			pItem->m_lpNodeInfo->m_cmbItems="0.独立参数|1.关联且启用D|2.关联且启用H"; 
			pItem->m_lpNodeInfo->m_strPropName = "D和H关系";
			pItem->m_lpNodeInfo->m_strPropHelp = "启用参数D或H。";
#endif
		}
		else if(pCirDlg->m_iPlateType==3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_cmbItems="Yes|No";
			pItem->m_lpNodeInfo->m_strPropName = "Enable D&H Parameter";
			pItem->m_lpNodeInfo->m_strPropHelp = "When D&H parameter is enabled,to confirm profile vertext by 。";
#else 
			pItem->m_lpNodeInfo->m_cmbItems="是|否";
			pItem->m_lpNodeInfo->m_strPropName = "启用D和H参数";
			pItem->m_lpNodeInfo->m_strPropHelp = "启用D和H参数时,与顶部圆弧求切确认轮廓点。";
#endif
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("para.nCirCoverPlateType"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pCirDlg->m_pRelaPart1&&pCirDlg->m_pRelaPart2)
			pItem->m_lpNodeInfo->m_cmbItems="Common Sealing Plate|Two Simple Sealing Plate";
		else 
			pItem->m_lpNodeInfo->m_cmbItems="Common Sealing Plate|Simple Sealing Plate(Y+)|Simple Sealing Plate(Y-)";
#else 
		if(pCirDlg->m_pRelaPart1&&pCirDlg->m_pRelaPart2)
			pItem->m_lpNodeInfo->m_cmbItems="普通封板|两块简化封板";
		else 
			pItem->m_lpNodeInfo->m_cmbItems="普通封板|简化封板(Y+)|简化封板(Y-)";
#endif
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("design_axis_z.hVicePart")||
		pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("ConnDir.dir.hVicePart"))
	{
		if(norm_style==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropName = "Datum Angle";
			pItem->m_lpNodeInfo->m_strPropHelp = "Datum angle's handle";
#else 
			pItem->m_lpNodeInfo->m_strPropName = "基准角钢";
			pItem->m_lpNodeInfo->m_strPropHelp = "基准角钢句柄";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropName = "Datum Rod";
			pItem->m_lpNodeInfo->m_strPropHelp = "Datum rod's handle";
#else 
			pItem->m_lpNodeInfo->m_strPropName = "基准杆件";
			pItem->m_lpNodeInfo->m_strPropHelp = "基准杆件句柄";
#endif
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("design_axis_z.direction")||
		pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("ConnDir.dir.direction"))
	{
		if(norm_style==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_cmbItems ="0.Outward|1.Inward";
			pItem->m_lpNodeInfo->m_strPropName = "Direction";
			pItem->m_lpNodeInfo->m_strPropHelp = "Direction";
#else 
			pItem->m_lpNodeInfo->m_cmbItems ="0.朝外|1.朝里";
			pItem->m_lpNodeInfo->m_strPropName = "朝向";
			pItem->m_lpNodeInfo->m_strPropHelp = "朝向";
#endif
		}
		else if(norm_style==3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_cmbItems ="0.Front Direction|1.Reverse Direction";
			pItem->m_lpNodeInfo->m_strPropName = "Direction";
			pItem->m_lpNodeInfo->m_strPropHelp = "Front direction is datum part's front work normal；reverse direction is datum part's reverse work normal";
#else 
			pItem->m_lpNodeInfo->m_cmbItems ="0.正向|1.反向";
			pItem->m_lpNodeInfo->m_strPropName = "朝向";
			pItem->m_lpNodeInfo->m_strPropHelp = "正向是指基准构件的正工作法线方向；反向是指基准构件的负工作法线方向";
#endif
		}
		else if(norm_style==5)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_cmbItems ="0.Start->End|1.End->Start";
			pItem->m_lpNodeInfo->m_strPropName = "Direction";
			pItem->m_lpNodeInfo->m_strPropHelp = "0.Rod start->end,1.Rod end->start,2.Angle's X normal positive direction,3.Angle's X normal negative direction,4.Angle's Y normal positive direction,5.Angle's Y normal negative direction";
#else 
			pItem->m_lpNodeInfo->m_cmbItems ="0.始->终|1.终->始";
			pItem->m_lpNodeInfo->m_strPropName = "方向类型";
			pItem->m_lpNodeInfo->m_strPropHelp = "0.杆件始->终方向,1.杆件终->始方向,2.角钢X肢法线方向,3.角钢X肢法线负方向,4.角钢Y肢法线方向,5.角钢Y肢法线负方向";
#endif
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("design_axis_z.nearVectorType")||
			pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("ConnDir.dir.nearVectorType"))
	{
		if(norm_style==2)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropName = "Near Direction";
			pItem->m_lpNodeInfo->m_strPropHelp = "The near direction is to determine that calculate direction is positive normal or negative normal";
#else 
			pItem->m_lpNodeInfo->m_strPropName = "近似方向";
			pItem->m_lpNodeInfo->m_strPropHelp = "近似方向用于区别计算方向为交叉面的正法线还是负法线方向";
#endif
		}
		else if(norm_style==4)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropName = "Relative Direction";
			pItem->m_lpNodeInfo->m_strPropHelp = "The relative direction on datum part's assemble CSYS.";
#else 
			pItem->m_lpNodeInfo->m_strPropName = "相对方向";
			pItem->m_lpNodeInfo->m_strPropHelp = "基准构件装配坐标系下的相对方向";
#endif
		}
	}
}
///////////////////////////////////////
// CDesignCircularPlateDlg dialog
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

CONNECT_DIRECTION *CConnVectorList::Append()
{
	return vecList.append();
}

CONNECT_DIRECTION *CConnVectorList::Append(CONNECT_DIRECTION &vec)
{
	CONNECT_DIRECTION *pVec=vecList.append();
	*pVec=vec;
	return pVec;
}

long CConnVectorList::GetRelaConnVecLinePart(int serial,BOOL bProfilePole/*=FALSE*/)
{
	if(bProfilePole&&serial<10)
		return 0;
	long hPart=0;
	CONNECT_DIRECTION *pVec=NULL;
	BOOL bPush=vecList.push_stack();
	for(pVec=vecList.GetFirst();pVec;pVec=vecList.GetNext())
	{
		if(pVec->serial==serial)
		{
			hPart=pVec->direct.hVicePart;
			break;
		}
	}
	if(bPush)
		vecList.pop_stack();
	return hPart;
}

CONNECT_DIRECTION *CConnVectorList::GetRelaConnVecByFeature(long feature)
{
	if(feature==0)
		return NULL;
	CONNECT_DIRECTION *pVec=NULL;
	BOOL bPush=vecList.push_stack();
	for(pVec=vecList.GetFirst();pVec;pVec=vecList.GetNext())
	{
		if(pVec->feature==feature)
			break;
	}
	if(bPush)
		vecList.pop_stack();
	return pVec;
}

//根据设置的特征值删除关联方向并返回关联方向的序列号 wht 11-05-18
BYTE CConnVectorList::DeleteRelaConnVecByFeature(long feature)
{
	BYTE serial=-1;
	if(feature==0)
		return serial;
	CONNECT_DIRECTION *pVec=NULL;
	BOOL bPush=vecList.push_stack();
	for(pVec=vecList.GetFirst();pVec;pVec=vecList.GetNext())
	{
		if(pVec->feature==feature)
		{
			serial=pVec->serial;
			vecList.DeleteCursor(FALSE);
			break;
		}
	}
	vecList.Clean();
	if(bPush)
		vecList.pop_stack();
	return serial;
}

CONNECT_DIRECTION *CConnVectorList::GetRelaConnInfo(int serial,BOOL bProfilePole/*=FALSE*/)
{
	if(bProfilePole&&serial<10)
		return NULL;
	CONNECT_DIRECTION *pVec=NULL;
	BOOL bPush=vecList.push_stack();
	for(pVec=vecList.GetFirst();pVec;pVec=vecList.GetNext())
	{
		if(pVec->serial==serial)
			break;
	}
	if(bPush)
		vecList.pop_stack();
	return pVec;
}
CONNECT_DIRECTION *CConnVectorList::GetFirst(BOOL bProfilePole/*=FALSE*/)
{
	CONNECT_DIRECTION *pVec=vecList.GetFirst();
	while(bProfilePole&&pVec&&pVec->serial<10)
		pVec=vecList.GetNext();
	return pVec;
}
CONNECT_DIRECTION *CConnVectorList::GetNext(BOOL bProfilePole/*=FALSE*/)
{
	CONNECT_DIRECTION *pVec=vecList.GetNext();
	while(bProfilePole&&pVec&&pVec->serial<10)
		pVec=vecList.GetNext();
	return pVec;
}
int CConnVectorList::GetCount(BOOL bProfilePole/*=FALSE*/)
{
	int nCount=0;
	if(bProfilePole)
	{
		CONNECT_DIRECTION *pVec=NULL;
		BOOL bPush=vecList.push_stack();
		for(pVec=vecList.GetFirst();pVec;pVec=vecList.GetNext())
		{
			if(pVec->serial>=10)
				nCount++;
		}
		if(bPush)
			vecList.pop_stack();
	}
	else 
		nCount=vecList.GetNodeNum();
	return nCount;
}
void CConnVectorList::Empty(BOOL bProfilePole/*=FALSE*/)
{
	if(bProfilePole)
	{
		CONNECT_DIRECTION *pVec=NULL;
		BOOL bPush=vecList.push_stack();
		for(pVec=vecList.GetFirst();pVec;pVec=vecList.GetNext())
		{
			if(pVec->serial>=10)
				vecList.DeleteCursor(FALSE);
		}
		vecList.Clean();
		if(bPush)
			vecList.pop_stack();
	}
	else 
		vecList.Empty();
}
//根据是否存在serial==1和serial==2的连接方向判断
BOOL CConnVectorList::IsNeedBend()
{
	CONNECT_DIRECTION *pVec=NULL,*pVec1=NULL,*pVec2=NULL;
	BOOL bPush=vecList.push_stack();
	for(pVec=vecList.GetFirst();pVec;pVec=vecList.GetNext())
	{
		if(pVec->serial==1)
			pVec1=pVec;
		else if(pVec->serial==2)
			pVec2=pVec;
		else if(pVec1&&pVec2)
			break;
	}
	if(bPush)
		vecList.pop_stack();
	BOOL bNeedBend=FALSE;
	if(pVec1&&pVec2)
	{	//存在两个可用的连接方向，未进一步判断是否合法
		pVec1->direct.vector.Set();
		pVec2->direct.vector.Set();
		pVec1->direct.UpdateVector(console.GetActiveModel());
		pVec2->direct.UpdateVector(console.GetActiveModel());
		if(!pVec1->direct.vector.IsZero()&&!pVec2->direct.vector.IsZero())
			bNeedBend=TRUE;
	}
	return bNeedBend;
}
//更新环向板参数
static BOOL FireCircularPlateHelpStr(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	if(pPropList->GetDlgCtrlID()!=IDC_LIST_BOX)
		return FALSE;	//子属性栏的属性不响应FireCircularPlateHelpStr()函数
	CDesignCircularPlateDlg *pCirDlg=(CDesignCircularPlateDlg*)pPropList->GetParent();
	pCirDlg->m_pCurSelConnVec=NULL;
	
	if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("RelaBolt"))
	{	//选中所有螺栓
		g_pSolidDraw->ReleaseSnapStatus();
		CLsRef *pLsRef=NULL;
		for(pLsRef=pCirDlg->LsRefList.GetFirst();pLsRef;pLsRef=pCirDlg->LsRefList.GetNext())
			g_pSolidDraw->SetEntSnapStatus((*pLsRef)->handle);
	}
	else if(pItem->m_pParent->m_idProp==CDesignCircularPlateDlg::GetPropID("RelaPole"))
	{	//查找当前选中项
		pCirDlg->m_pCurSelConnVec=pCirDlg->ConnVecList.GetRelaConnVecByFeature(pItem->m_idProp);
		//显示子属性
		pCirDlg->DisplaySubPropList(NULL);
		//选中关联杆件
		g_pSolidDraw->ReleaseSnapStatus();
		if(pCirDlg->m_pCurSelConnVec)
			g_pSolidDraw->SetEntSnapStatus(pCirDlg->m_pCurSelConnVec->direct.hVicePart);
	}
	else if(((pCirDlg->m_iPlateType==2&&pCirDlg->para.nDHRelativeType==0)	//3WL D,H为独立参数 wht 11-05-11
		||(pCirDlg->m_iPlateType==3&&pCirDlg->para.nDHRelativeType))		//3WA D,H为独立参数
		&&(CDesignCircularPlateDlg::GetPropID("hRelaPart1")==pItem->m_idProp
		||CDesignCircularPlateDlg::GetPropID("hRelaPart2")==pItem->m_idProp))
	{	//选中关联钢板时查找关联钢板上的连接方向
		int serial=-1;
		if(CDesignCircularPlateDlg::GetPropID("hRelaPart1")==pItem->m_idProp)
			serial=1;
		else 
			serial=2;
		pCirDlg->m_pCurSelConnVec=pCirDlg->ConnVecList.GetRelaConnInfo(serial);
		//显示子属性
		pCirDlg->DisplaySubPropList(pItem);
	}
	else if(CDesignCircularPlateDlg::GetPropID("type")==pItem->m_idProp)
	{	//环向板类型
#ifdef AFX_TARG_ENU_ENGLISH
		if(pCirDlg->m_iPlateType==0)
			pItem->m_lpNodeInfo->m_strPropHelp="Ordinary circular plate";
		else if(pCirDlg->m_iPlateType==1)
			pItem->m_lpNodeInfo->m_strPropHelp="Circular sealing plate";
		else if(pCirDlg->m_iPlateType==2)
			pItem->m_lpNodeInfo->m_strPropHelp="Circular rib plate 3WL,3W said that the plate has three brace edge(It needs to be welded edge),L shows free edge(No need to be welded edge) is straight edge。";
		else if(pCirDlg->m_iPlateType==3)
			pItem->m_lpNodeInfo->m_strPropHelp="Circular rib plate 3WA,3W said that the plate has three brace edge(It needs to be welded edge),A shows free edge(No need to be welded edge) is arc edge。";
		else if(pCirDlg->m_iPlateType==4)
			pItem->m_lpNodeInfo->m_strPropHelp="Circular rib plate 2WL,2W said that the plate has two brace edge(It needs to be welded edge),L shows free edge(No need to be welded edge) is straight edge。";
		else if(pCirDlg->m_iPlateType==5)
			pItem->m_lpNodeInfo->m_strPropHelp="Circular rib plate 2WA,2W said that the plate has two brace edge(It needs to be welded edge),A shows free edge(No need to be welded edge) is arc edge。";
		else if(pCirDlg->m_iPlateType==6)
			pItem->m_lpNodeInfo->m_strPropHelp="Shelter Plate";
#else 
		if(pCirDlg->m_iPlateType==0)
			pItem->m_lpNodeInfo->m_strPropHelp="普通环向板";
		else if(pCirDlg->m_iPlateType==1)
			pItem->m_lpNodeInfo->m_strPropHelp="环向封板";
		else if(pCirDlg->m_iPlateType==2)
			pItem->m_lpNodeInfo->m_strPropHelp="环向肋板3WL,3W表示该环向板存在三条支撑边(即需要焊接的边),L表示自由边(即不需要焊接的边)为直线边。";
		else if(pCirDlg->m_iPlateType==3)
			pItem->m_lpNodeInfo->m_strPropHelp="环向肋板3WA,3W表示该环向板存在三条支撑边(即需要焊接的边),A表示自由边(即不需要焊接的边)为圆弧边。";
		else if(pCirDlg->m_iPlateType==4)
			pItem->m_lpNodeInfo->m_strPropHelp="环向肋板2WL,2W表示该环向板存在两条支撑边(即需要焊接的边),L表示自由边(即不需要焊接的边)为直线边。";
		else if(pCirDlg->m_iPlateType==5)
			pItem->m_lpNodeInfo->m_strPropHelp="环向肋板2WA,2W表示该环向板存在两条支撑边(即需要焊接的边),A表示自由边(即不需要焊接的边)为圆弧边。";
		else if(pCirDlg->m_iPlateType==6)
			pItem->m_lpNodeInfo->m_strPropHelp="遮水板";
#endif
		::SetWindowText(pPropList->m_hPromptWnd,pItem->m_lpNodeInfo->m_strPropHelp);
		::UpdateWindow(pPropList->m_hPromptWnd);
		//显示子属性
		pCirDlg->DisplaySubPropList(NULL);	
	}
	else //显示子属性
		pCirDlg->DisplaySubPropList(pItem);	

	if((pCirDlg->m_pRelaPart1&&CDesignCircularPlateDlg::GetPropID("para.Y")==pItem->m_idProp)
		||(pCirDlg->m_iPlateType!=1&&CDesignCircularPlateDlg::GetPropID("para.S")==pItem->m_idProp))
	{	//-1.基板内侧，1.基板外侧
		UCS_STRU object_ucs,ucs;
		if(pCirDlg->m_pRelaPart1&&CDesignCircularPlateDlg::GetPropID("para.Y")==pItem->m_idProp)
			ucs = pCirDlg->m_pRelaPart1->ucs;
		else if(pCirDlg->m_pCirPlate)
			ucs = pCirDlg->m_pCirPlate->ucs;	//显示当前正在编辑的环向板坐标系便于调整法线偏移量 wht 11-05-27
		else 
			ucs = pCirDlg->plate_ucs;			//显示当前正在设计的环向板的坐标系 wht 11-06-03
		g_pSolidSet->GetObjectUcs(object_ucs);
		coord_trans(ucs.origin,object_ucs,TRUE);
		vector_trans(ucs.axis_x,object_ucs,TRUE);
		vector_trans(ucs.axis_y,object_ucs,TRUE);
		vector_trans(ucs.axis_z,object_ucs,TRUE);
		g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
		//g_pSolidSet->SetDispWorkUcs();
		g_pSolidDraw->Draw();
	}
	//选中定位构件
	long hCurPart=0;
	if(CDesignCircularPlateDlg::GetPropID("hDatumTube")==pItem->m_idProp)
	{
		if(pCirDlg->m_pDatumTube)
			hCurPart=pCirDlg->m_pDatumTube->handle;
	}
	else if(CDesignCircularPlateDlg::GetPropID("hDatumSphere")==pItem->m_idProp)
	{
		if(pCirDlg->m_pDatumSphere)
			hCurPart=pCirDlg->m_pDatumSphere->handle;
	}
	else if(CDesignCircularPlateDlg::GetPropID("hRelaPart1")==pItem->m_idProp)
	{
		if(pCirDlg->m_pRelaPart1)
			hCurPart=pCirDlg->m_pRelaPart1->handle;
	}
	else if(CDesignCircularPlateDlg::GetPropID("hRelaPart2")==pItem->m_idProp)
	{
		if(pCirDlg->m_pRelaPart2)
			hCurPart=pCirDlg->m_pRelaPart2->handle;
	}
	else if(pItem->m_pParent&&pItem->m_pParent->m_idProp==CDesignCircularPlateDlg::GetPropID("RelaPole"))
	{	//关联杆件
		CONNECT_DIRECTION *pVec=NULL;
		for(pVec=pCirDlg->ConnVecList.GetFirst(TRUE);pVec;pVec=pCirDlg->ConnVecList.GetNext(TRUE))
		{
			if(pVec->feature==pItem->m_idProp)
			{
				hCurPart=pVec->direct.hVicePart;
				break;
			}
		}
	}
	if(hCurPart==0)
	{	//选中当前正在设计的肋板
		if(pCirDlg->m_pCirPlate)
			hCurPart=pCirDlg->m_pCirPlate->handle;
	}
	long *id_arr=NULL;
	int n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("RelaBolt")&&n>0)	
		hCurPart=0;	//当前已选中关联螺栓或关联杆件
	//需要设置选中状态的构件已处于选中状态时不用重新设置选中状态
	if(!(n==1&&id_arr[0]==hCurPart)&&hCurPart>0x20)
	{	//
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(hCurPart);
	}
	return TRUE;
}
static BOOL ModifyCircularPlateProperty(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CXhChar500 sText;
	CPropTreeItem *pSonItem=NULL,*pPropItem=NULL;
	BOOL bCalNorm=FALSE;	//根据参数计算法线
	CDesignCircularPlateDlg *pCircularDlg=(CDesignCircularPlateDlg*)pPropList->GetParent();
	CPropertyListOper<CDesignCircularPlateDlg> oper(pPropList,pCircularDlg);
	//
	CONNECT_DIRECTION *pPlateCurConVec=NULL;	//环向板当前关联方向,修改环向板连接方向时使用 wht 11-05-30
	if(pCircularDlg->m_pCirPlate&&pCircularDlg->m_pCurSelConnVec)
	{	//
		int serial=pCircularDlg->m_pCurSelConnVec->serial;
		pPlateCurConVec=pCircularDlg->m_pCirPlate->GetRelaConnInfo(serial);
	}
	if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("sPartNo"))
	{
		pCircularDlg->m_sPartNo.Copy(valueStr);
		SEGI iSeg;
		if(ParsePartNo(pCircularDlg->m_sPartNo,&iSeg,NULL))
		{
			pCircularDlg->m_sSeg=iSeg.ToString();
			pCircularDlg->m_xLayoutBolt.m_iBoltSegI=iSeg;	//螺栓段号
			CString sTempValue;
			sTempValue=iSeg.ToString();
			pPropList->SetItemPropValue(CDesignCircularPlateDlg::GetPropID("iSeg"),sTempValue);
			if(pCircularDlg->m_pCirPlate)
			{	//更新肋板编号和段号 wht 11-05-13
				pCircularDlg->m_pCirPlate->SetPartNo(valueStr.GetBuffer());
				pCircularDlg->m_pCirPlate->iSeg=iSeg;
			}
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("iSeg"))
	{
		pCircularDlg->m_sSeg=valueStr;
		pCircularDlg->m_xLayoutBolt.m_iBoltSegI=SEGI(pCircularDlg->m_sSeg.GetBuffer());
		if(pCircularDlg->m_pCirPlate)
			pCircularDlg->m_pCirPlate->iSeg=SEGI(pCircularDlg->m_sSeg.GetBuffer());
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("material"))
	{
		for(int i=0;i<CSteelMatLibrary::GetCount();i++)
		{
			STEELMAT mat=CSteelMatLibrary::RecordAt(i);
			if(valueStr.CompareNoCase(mat.mark)==0)
			{
				pCircularDlg->m_cMaterial=mat.cBriefMark;
				break;
			}
		}
		if(pCircularDlg->m_pCirPlate)
			pCircularDlg->m_pCirPlate->cMaterial=pCircularDlg->m_cMaterial;
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("thick"))
	{
		pCircularDlg->thick=atoi(valueStr);
		if(pCircularDlg->m_pCirPlate)
		{
			pCircularDlg->m_pCirPlate->thick=pCircularDlg->thick;
			pCircularDlg->ReDesignPlate();	//重新设计
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("type"))
	{
		//清空关联杆件关联螺栓
		pCircularDlg->LsRefList.Empty();
		pCircularDlg->ConnVecList.Empty();
		//
		pCircularDlg->m_iPlateType=valueStr[0]-'0';
		pCircularDlg->UpdateCirPlateParamProp();
		pCircularDlg->InitProfilePara();	//重新初始化钢板外形参数
		pCircularDlg->UpdateSketchMap();
		FireCircularPlateHelpStr(pPropList,pItem);	//更新帮助字符串
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("ucs.axis_z"))
	{
		pCircularDlg->design_axis_z.vector=CStdVector::GetVector(valueStr[0]-'0');
		pCircularDlg->design_axis_z.norm_style=0;	//为X+、X-、Y+、Y-等特殊方向时法线计算方式修改为指定法线方向
		pItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//显示子属性
		pCircularDlg->DisplaySubPropList(pItem);
		bCalNorm=TRUE;	//根据参数计算法线
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("ucs.axis_z.x"))
	{
		pCircularDlg->design_axis_z.vector.x = atof(valueStr);
		bCalNorm=TRUE;	//根据参数计算法线
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("ucs.axis_z.y"))
	{
		pCircularDlg->design_axis_z.vector.y = atof(valueStr);
		bCalNorm=TRUE;	//根据参数计算法线
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("ucs.axis_z.z"))
	{
		pCircularDlg->design_axis_z.vector.z = atof(valueStr);
		bCalNorm=TRUE;	//根据参数计算法线
	}
	//
	else if(CDesignCircularPlateDlg::GetPropID("para.A")==pItem->m_idProp)//alfa 定位角
	{
		pCircularDlg->para.A=atof(valueStr);
		pCircularDlg->ModifyPlateParam('A',pCircularDlg->para.A);
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.B")==pItem->m_idProp)//beta 扇形角	
	{
		pCircularDlg->para.B=atof(valueStr);
		pCircularDlg->ModifyPlateParam('B',pCircularDlg->para.B);
		if(pCircularDlg->m_iPlateType==4)
			pCircularDlg->UpdateSketchMap();
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.y")==pItem->m_idProp)//2WA型环向肋板的倒角角度°	
	{
		pCircularDlg->para.gama=atof(valueStr);
		pCircularDlg->ModifyPlateParam('y',pCircularDlg->para.gama);
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.D")==pItem->m_idProp)//自由边距		
	{
		pCircularDlg->para.D=atof(valueStr);
		if(pCircularDlg->m_iPlateType!=1)	//非环向封板
			pCircularDlg->ModifyPlateParam('D',pCircularDlg->para.D);
		if(pCircularDlg->m_iPlateType==4)
			pCircularDlg->UpdateSketchMap();
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.H")==pItem->m_idProp)//台口高度
	{
		pCircularDlg->para.H=atof(valueStr);
		if(pCircularDlg->m_iPlateType==1)
			pCircularDlg->ModifyPlateParam('D',pCircularDlg->para.H);
		else 
		{
			pCircularDlg->ModifyPlateParam('H',pCircularDlg->para.H);
			pCircularDlg->para.h=pCircularDlg->para.H;
			CPropTreeItem *pRelaItem=pPropList->FindItemByPropId(CDesignCircularPlateDlg::GetPropID("para.h"),NULL);
			if(pRelaItem)
				pPropList->SetItemPropValue(pRelaItem->m_idProp,valueStr);
		}
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.bMode")==pItem->m_idProp)//模式
	{
		if(valueStr.CompareNoCase("否")==0)
		{
			pCircularDlg->para.bWeldSemicircleMode = FALSE;
			pCircularDlg->para.R1 = pCircularDlg->para.R2 = 0;
			
		}
		else if(valueStr.CompareNoCase("是")==0)
		{
			pCircularDlg->para.bWeldSemicircleMode = TRUE;
			pCircularDlg->para.R1 = (pCircularDlg->m_pRelaPart1!=NULL)?pCircularDlg->m_pRelaPart1->Thick:0;
			pCircularDlg->para.R2 = (pCircularDlg->m_pRelaPart2!=NULL)?pCircularDlg->m_pRelaPart2->Thick:0;
		}
		pCircularDlg->ModifyPlateParam(KEY2C("R1"),pCircularDlg->para.R1);
		pCircularDlg->ModifyPlateParam(KEY2C("R2"),pCircularDlg->para.R2);
		//更新属性栏
		pPropList->DeleteAllSonItems(pItem);
		if(pCircularDlg->para.bWeldSemicircleMode)
		{
			pPropItem=oper.InsertEditPropItem(pItem,"para.R1","","",-1,TRUE);
			pPropItem->SetReadOnly(pCircularDlg->m_pRelaPart1==NULL);
			pPropItem=oper.InsertEditPropItem(pItem,"para.R2","","",-1,TRUE);
			pPropItem->SetReadOnly(pCircularDlg->m_pRelaPart2==NULL);
		}
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.R1")==pItem->m_idProp)//流水孔1半径
	{
		pCircularDlg->para.R1=atof(valueStr);
		pCircularDlg->ModifyPlateParam(KEY2C("R1"),pCircularDlg->para.R1);
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.R2")==pItem->m_idProp)//流水孔2半径
	{
		pCircularDlg->para.R2=atof(valueStr);
		pCircularDlg->ModifyPlateParam(KEY2C("R2"),pCircularDlg->para.R2);
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.h")==pItem->m_idProp)//N侧台口高度
	{
		pCircularDlg->para.h=atof(valueStr);
		pCircularDlg->ModifyPlateParam('h',pCircularDlg->para.h);
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.R")==pItem->m_idProp)//外圆半径
	{
		pCircularDlg->para.R=atof(valueStr);
		if(pCircularDlg->m_iPlateType!=1)	//非环向封板
		{
			if(pCircularDlg->m_iPlateType==3&&pCircularDlg->para.nDHRelativeType==0)
			{	//3WA 以前版本顶部圆弧连接的肋板W==N==D H=0,当前版本仅保存R wht 11-05-24
				pCircularDlg->ModifyPlateParam('W',(long)0);
				pCircularDlg->ModifyPlateParam('N',(long)0);
				pCircularDlg->ModifyPlateParam('H',(long)0);
				pCircularDlg->ModifyPlateParam('D',(long)0);
			}
			pCircularDlg->ModifyPlateParam('R',pCircularDlg->para.R);
		}
	}
	else if(CDesignCircularPlateDlg::GetPropID("innercir_type")==pItem->m_idProp)
	{	//环向板内圆类型 wht 11-05-27
		pPropList->DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.r"));
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("0.No Inner Circle")==0)
			pCircularDlg->para.r=0;
		else if(valueStr.CompareNoCase("1.Specify Inner Circle R")==0)
#else 
		if(valueStr.CompareNoCase("0.无内圆")==0)
			pCircularDlg->para.r=0;
		else if(valueStr.CompareNoCase("1.指定内圆半径")==0)
#endif
		{
			pCircularDlg->para.r=pCircularDlg->para.V;
			if(pCircularDlg->m_pDatumTube)
				pCircularDlg->para.r+=pCircularDlg->m_pDatumTube->GetDiameter()*0.5;
			//内圆半径
			oper.InsertEditPropItem(pItem,"para.r","","",-1,TRUE);
		}
		else //if(valueStr.CompareNoCase("2.根据钢管生成")==0)
			pCircularDlg->para.r=-1;
		pCircularDlg->ModifyPlateParam('r',pCircularDlg->para.r);
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.r")==pItem->m_idProp)		
	{
		pCircularDlg->para.r=atof(valueStr);	//内圆半径
		pCircularDlg->ModifyPlateParam('r',pCircularDlg->para.r);
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.S")==pItem->m_idProp)	
	{	//封板位置
		if(pCircularDlg->m_iPlateType==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Up Sealing Plate")==0)
				pCircularDlg->para.S=1;
			else if(valueStr.CompareNoCase("Down Sealing Plate")==0)
#else 
			if(valueStr.CompareNoCase("上封板")==0)
				pCircularDlg->para.S=1;
			else if(valueStr.CompareNoCase("下封板")==0)
#endif
				pCircularDlg->para.S=-1;
			else 
				pCircularDlg->para.S=0;
			pCircularDlg->ModifyPlateParam('S',pCircularDlg->para.S);
		}
		else //位置偏移
		{
			pCircularDlg->para.S=atof(valueStr);
			if(pCircularDlg->para.nOffsetType==0)
			{
				pCircularDlg->ModifyPlateParam('S',pCircularDlg->para.S);
				pCircularDlg->ModifyPlateParam('T',(double)0);
			}
			else 
			{
				pCircularDlg->ModifyPlateParam('T',pCircularDlg->para.S);
				pCircularDlg->ModifyPlateParam('S',(double)0);
			}
		}
		//修改偏移量后重新计算W、N的长度 wht 11-05-12
		if(pCircularDlg->m_iPlateType>1&&pCircularDlg->m_iPlateType<6)
		{
			pCircularDlg->AutoCalWN();
			pCircularDlg->ModifyPlateParam('W',pCircularDlg->para.W);
			pCircularDlg->ModifyPlateParam('N',pCircularDlg->para.N);
			//
			long idProp=CDesignCircularPlateDlg::GetPropID("para.W");
			if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
				pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
			idProp=CDesignCircularPlateDlg::GetPropID("para.N");
			if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
				pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		}
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.nCirCoverPlateType")==pItem->m_idProp)	
	{	//封板类型
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Two Simple Sealing Plate")==0)
			pCircularDlg->para.nCirCoverPlateType=2;	//生成两块简化钢板
		else if(valueStr.CompareNoCase("Simple Sealing Plate(Y+)")==0)
			pCircularDlg->para.nCirCoverPlateType=1;
		else if(valueStr.CompareNoCase("Simple Sealing Plate(Y-)")==0)
#else 
		if(valueStr.CompareNoCase("两块简化封板")==0)
			pCircularDlg->para.nCirCoverPlateType=2;	//生成两块简化钢板
		else if(valueStr.CompareNoCase("简化封板(Y+)")==0)
			pCircularDlg->para.nCirCoverPlateType=1;
		else if(valueStr.CompareNoCase("简化封板(Y-)")==0)
#endif
			pCircularDlg->para.nCirCoverPlateType=-1;
		else 
			pCircularDlg->para.nCirCoverPlateType=0;
		pCircularDlg->ModifyPlateParam('Y',pCircularDlg->para.nCirCoverPlateType);
		pCircularDlg->UpdateSketchMap();
		//更新外形属性 wht 12-03-27
		pCircularDlg->DisplaySubPropList(NULL);
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.bCreateMirPlate")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
#endif
			pCircularDlg->para.bCreateMirPlate=TRUE;
		else 
			pCircularDlg->para.bCreateMirPlate=FALSE;
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.V")==pItem->m_idProp)//加工间隙
	{
		pCircularDlg->para.V=atof(valueStr);
		if(pCircularDlg->m_iPlateType==1)
			pCircularDlg->ModifyPlateParam('R',pCircularDlg->para.V+pCircularDlg->m_pDatumTube->GetDiameter()*0.5);
		else 
			pCircularDlg->ModifyPlateParam('V',pCircularDlg->para.V);
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.W")==pItem->m_idProp)		
	{
		pCircularDlg->para.W=atof(valueStr);		//第一块关联板支撑边宽/环形带宽
		pCircularDlg->ModifyPlateParam('W',pCircularDlg->para.W);
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.N")==pItem->m_idProp)		
	{
		pCircularDlg->para.N=atof(valueStr);		//第二块关联板支撑边宽
		pCircularDlg->ModifyPlateParam('N',pCircularDlg->para.N);
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.Y")==pItem->m_idProp)		
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Datum Plate Outside")==0)
			pCircularDlg->para.Y=1;
		else if(valueStr.CompareNoCase("Datum Plate Inside")==0)
#else 
		if(valueStr.CompareNoCase("基板外侧")==0)
			pCircularDlg->para.Y=1;
		else if(valueStr.CompareNoCase("基板内侧")==0)
#endif
			pCircularDlg->para.Y=-1;
		else 
			pCircularDlg->para.Y=0;	//自动判断
		pCircularDlg->ModifyPlateParam('Y',pCircularDlg->para.Y);
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.nOffsetType")==pItem->m_idProp)		
	{
		pCircularDlg->para.nOffsetType=valueStr[0]-'0';
		if(pCircularDlg->m_iPlateType!=1&&pCircularDlg->m_iPlateType!=6)
		{	//根据偏移类型修改偏移值
			if(pCircularDlg->para.nOffsetType==0)
			{
				pCircularDlg->ModifyPlateParam('S',pCircularDlg->para.S);
				pCircularDlg->ModifyPlateParam('T',(double)0);
			}
			else 
			{
				pCircularDlg->ModifyPlateParam('T',pCircularDlg->para.S);
				pCircularDlg->ModifyPlateParam('S',(double)0);
			}
		}
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.nDHRelativeType")==pItem->m_idProp)
	{
		int nOldType=pCircularDlg->para.nDHRelativeType;	//记录该属性修改以前的值
		//
		pPropList->DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.D"));
		pPropList->DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.H"));
		pPropList->DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.W"));
		pPropList->DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.N"));
		pPropList->DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.h"));
		pPropList->DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.R"));
		pPropList->DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.V"));
		if(pCircularDlg->m_iPlateType==2)
		{
			pCircularDlg->para.nDHRelativeType=valueStr[0]-'0';
			if(pCircularDlg->para.nDHRelativeType==0||pCircularDlg->para.nDHRelativeType==1)
			{	//自由边距
				pPropItem=oper.InsertEditPropItem(pItem->m_pParent,"para.D","","",-1,TRUE);
				UpdatePropItemName(pPropList,pPropItem);
			}
			if(pCircularDlg->para.nDHRelativeType==0||pCircularDlg->para.nDHRelativeType==2)
			{	//台口高度
				oper.InsertEditPropItem(pItem->m_pParent,"para.H","","",-1,TRUE);
			}
			pPropItem=oper.InsertEditPropItem(pItem->m_pParent,"para.W","","",-1,TRUE);
			UpdatePropItemName(pPropList,pPropItem);
			//
			oper.InsertEditPropItem(pItem->m_pParent,"para.N","","",-1,TRUE);
			//
			oper.InsertEditPropItem(pItem->m_pParent,"para.V","","",-1,TRUE);
		}
		else if(pCircularDlg->m_iPlateType==3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else 
			if(valueStr.CompareNoCase("是")==0)
#endif
				pCircularDlg->para.nDHRelativeType=1;	//由3修改为1,赋值为3时切换到3WL参数显示不正确 wht 11-05-12
			else 
				pCircularDlg->para.nDHRelativeType=0;
			//
			if(pCircularDlg->para.nDHRelativeType!=0)
			{	
				//环形带宽D
				pPropItem=oper.InsertEditPropItem(pItem->m_pParent,"para.D","","",-1,TRUE);
				UpdatePropItemName(pPropList,pPropItem);
				//台口高度
				oper.InsertEditPropItem(pItem->m_pParent,"para.H","","",-1,TRUE);
				//N侧台口高度
				oper.InsertEditPropItem(pItem->m_pParent,"para.h","","",-1,TRUE);
				//参数W
				pPropItem=oper.InsertEditPropItem(pItem->m_pParent,"para.W","","",-1,TRUE);
				UpdatePropItemName(pPropList,pPropItem);
				//参数N
				oper.InsertEditPropItem(pItem->m_pParent,"para.N","","",-1,TRUE);
			}
			else //外圆半径Ro
				oper.InsertEditPropItem(pItem->m_pParent,"para.R","","",-1,TRUE);
			//
			oper.InsertEditPropItem(pItem->m_pParent,"para.V","","",-1,TRUE);
		}
		//更新关联基准钢板以及关联钢板2
		long idProp=CDesignCircularPlateDlg::GetPropID("hRelaPart1");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_propList.SetItemPropValue(idProp,sText);
		idProp=CDesignCircularPlateDlg::GetPropID("hRelaPart2");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_propList.SetItemPropValue(idProp,sText);
		//从独立参数切换到非独立参数，或从非独立参数切换到的独立参数时需要执行以下代码 wht 11-05-12
		if((nOldType>0)!=(pCircularDlg->para.nDHRelativeType>0))
		{	//跨越连板需要显示关联螺栓，关联杆件
			pCircularDlg->m_propList.DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("RelaBolt"));
			pCircularDlg->m_propList.DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("RelaPole"));
			//清空螺栓链表以及杆件链表 
			pCircularDlg->LsRefList.Empty();
			pCircularDlg->ConnVecList.Empty(TRUE);	//清空与外形相关的杆件
			CPropTreeItem *pRelaInfoItem=pCircularDlg->m_propList.FindItemByPropId(CDesignCircularPlateDlg::GetPropID("RelaInfo"),NULL);
			CPropertyListOper<CDesignCircularPlateDlg> oper2(&(pCircularDlg->m_propList),pCircularDlg);
			if(pRelaInfoItem
				&&((pCircularDlg->m_iPlateType==2&&pCircularDlg->para.nDHRelativeType==0)
				||(pCircularDlg->m_iPlateType==3&&pCircularDlg->para.nDHRelativeType)))
			{
				oper2.InsertButtonPropItem(pRelaInfoItem,"RelaBolt","","",-1,TRUE);	//关联螺栓
				oper2.InsertButtonPropItem(pRelaInfoItem,"RelaPole","","",-1,TRUE);	//关联杆件
			}
		}
		pCircularDlg->UpdateSketchMap(); //更新示意图
	}
	else if(CDesignCircularPlateDlg::GetPropID("water.W")==pItem->m_idProp)	
	{
		pCircularDlg->water.W=atof(valueStr);	//外环直径
		pCircularDlg->ModifyPlateParam('W',pCircularDlg->water.W);
	}
	else if(CDesignCircularPlateDlg::GetPropID("water.S")==pItem->m_idProp)	
	{
		pCircularDlg->water.S=atof(valueStr);	//法向偏移距离
		pCircularDlg->ModifyPlateParam('S',pCircularDlg->water.S);
	}
	else if(CDesignCircularPlateDlg::GetPropID("water.K")==pItem->m_idProp)	
	{
		pCircularDlg->water.K=atof(valueStr);	//流水槽宽度
		pCircularDlg->ModifyPlateParam('K',pCircularDlg->water.K);
	}
	else if(CDesignCircularPlateDlg::GetPropID("water.H")==pItem->m_idProp)	
	{
		pCircularDlg->water.H=atof(valueStr);	//流水槽深度
		pCircularDlg->ModifyPlateParam('H',pCircularDlg->water.H);
	}
	else if(CDesignCircularPlateDlg::GetPropID("water.L")==pItem->m_idProp)	
	{
		pCircularDlg->water.L=atof(valueStr);	//流水槽长度
		pCircularDlg->ModifyPlateParam('L',pCircularDlg->water.L);
	}
	else if(CDesignCircularPlateDlg::GetPropID("water.A")==pItem->m_idProp)	
	{
		pCircularDlg->water.A=atof(valueStr);	//两流水槽之间的夹角
		pCircularDlg->ModifyPlateParam('A',pCircularDlg->water.A);
	}
	else if(CDesignCircularPlateDlg::GetPropID("water.O")==pItem->m_idProp)	
	{
		pCircularDlg->water.O=atof(valueStr);	//两流水槽角平分线与基准边之间的偏移角
		pCircularDlg->ModifyPlateParam('O',pCircularDlg->water.O);
	}
	else if(CDesignCircularPlateDlg::GetPropID("water.E")==pItem->m_idProp)	
	{
		pCircularDlg->water.E=valueStr[0]-'0';	//定位角基准边:0.基准边1.焊道线
		pCircularDlg->ModifyPlateParam('E',pCircularDlg->water.E);
	}
	//环向布置螺栓参数
	else if(CDesignCircularPlateDlg::GetPropID("m_xLayoutBolt.m_nBoltNum")==pItem->m_idProp)
	{
		pCircularDlg->m_xLayoutBolt.m_nBoltNum=atoi(valueStr);
		pCircularDlg->m_propList.SetItemPropValue(CDesignCircularPlateDlg::GetPropID("CreateBolt"),"%d",atoi(valueStr));
	}
	else if(CDesignCircularPlateDlg::GetPropID("m_xLayoutBolt.m_nBoltD")==pItem->m_idProp)
		pCircularDlg->m_xLayoutBolt.m_nBoltD=atoi(valueStr);
	else if(CDesignCircularPlateDlg::GetPropID("m_xLayoutBolt.m_iDriection")==pItem->m_idProp)
		pCircularDlg->m_xLayoutBolt.m_iDirection=valueStr[0]-'0';
	else if(CDesignCircularPlateDlg::GetPropID("m_xLayoutBolt.m_iBoltSegI")==pItem->m_idProp)
		pCircularDlg->m_xLayoutBolt.m_iBoltSegI=atoi(valueStr);
	else if(CDesignCircularPlateDlg::GetPropID("m_xLayoutBolt.m_iStartEdge")==pItem->m_idProp)
		pCircularDlg->m_xLayoutBolt.m_iStartEdge=valueStr[0]-'0';
	else if(CDesignCircularPlateDlg::GetPropID("m_xLayoutBolt.m_fOffsetAngle")==pItem->m_idProp)
		pCircularDlg->m_xLayoutBolt.m_fOffsetAngle=atof(valueStr);
	else if(CDesignCircularPlateDlg::GetPropID("m_xLayoutBolt.m_fBoltLayoutR")==pItem->m_idProp)
		pCircularDlg->m_xLayoutBolt.m_fBoltLayoutR=atof(valueStr);
	//法线设计参数
	else if(CDesignCircularPlateDlg::GetPropID("design_axis_z.norm_style")==pItem->m_idProp)//基准杆件句柄
	{
		pCircularDlg->design_axis_z.norm_style=valueStr[0]-'0';
		pCircularDlg->UpdateVectorPropList(pItem,"design_axis_z",pCircularDlg->design_axis_z);
		bCalNorm=TRUE;	//根据参数计算法线
	}
	else if(CDesignCircularPlateDlg::GetPropID("design_axis_z.hVicePart")==pItem->m_idProp)//基准杆件句柄
	{
		sscanf(valueStr,"%d",&pCircularDlg->design_axis_z.hVicePart);
		bCalNorm=TRUE;	//根据参数计算法线
	}
	else if(CDesignCircularPlateDlg::GetPropID("design_axis_z.hCrossPart")==pItem->m_idProp)
	{
		sscanf(valueStr,"%d",&pCircularDlg->design_axis_z.hCrossPart);
		bCalNorm=TRUE;	//根据参数计算法线
	}
	else if(CDesignCircularPlateDlg::GetPropID("design_axis_z.norm_wing")==pItem->m_idProp)
	{
		pCircularDlg->design_axis_z.norm_wing=valueStr[0]-'0';
		bCalNorm=TRUE;	//根据参数计算法线
	}
	else if(CDesignCircularPlateDlg::GetPropID("design_axis_z.direction")==pItem->m_idProp)
	{
		pCircularDlg->design_axis_z.direction=valueStr[0]-'0';
		bCalNorm=TRUE;	//根据参数计算法线
	}
	else if(CDesignCircularPlateDlg::GetPropID("design_axis_z.VectorType")==pItem->m_idProp)
	{
		int nVecType=valueStr[0]-'0';
		pCircularDlg->design_axis_z.vector=CStdVector::GetVector(nVecType);
		long idProp=CDesignCircularPlateDlg::GetPropID("design_axis_z.vector.x");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		pCircularDlg->m_xSubPropList.SetItemReadOnly(idProp,nVecType!=0);
		idProp=CDesignCircularPlateDlg::GetPropID("design_axis_z.vector.y");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		pCircularDlg->m_xSubPropList.SetItemReadOnly(idProp,nVecType!=0);
		idProp=CDesignCircularPlateDlg::GetPropID("design_axis_z.vector.z");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		pCircularDlg->m_xSubPropList.SetItemReadOnly(idProp,nVecType!=0);
		bCalNorm=TRUE;	//根据参数计算法线
	}
	else if(CDesignCircularPlateDlg::GetPropID("design_axis_z.vector.x")==pItem->m_idProp)
	{
		pCircularDlg->design_axis_z.vector.x=atof(valueStr);
		bCalNorm=TRUE;	//根据参数计算法线
	}
	else if(CDesignCircularPlateDlg::GetPropID("design_axis_z.vector.y")==pItem->m_idProp)
	{
		pCircularDlg->design_axis_z.vector.y=atof(valueStr);
		bCalNorm=TRUE;	//根据参数计算法线
	}
	else if(CDesignCircularPlateDlg::GetPropID("design_axis_z.vector.z")==pItem->m_idProp)
	{
		pCircularDlg->design_axis_z.vector.z=atof(valueStr);
		bCalNorm=TRUE;	//根据参数计算法线
	}
	else if(CDesignCircularPlateDlg::GetPropID("design_axis_z.nearVectorType")==pItem->m_idProp)
	{
		int nVecType=valueStr[0]-'0';
		pCircularDlg->design_axis_z.nearVector=CStdVector::GetVector(nVecType);
		long idProp=CDesignCircularPlateDlg::GetPropID("design_axis_z.nearVector.x");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		pCircularDlg->m_xSubPropList.SetItemReadOnly(idProp,nVecType!=0);
		idProp=CDesignCircularPlateDlg::GetPropID("design_axis_z.nearVector.y");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		pCircularDlg->m_xSubPropList.SetItemReadOnly(idProp,nVecType!=0);
		idProp=CDesignCircularPlateDlg::GetPropID("design_axis_z.nearVector.z");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		pCircularDlg->m_xSubPropList.SetItemReadOnly(idProp,nVecType!=0);
		bCalNorm=TRUE;	//根据参数计算法线
	}
	else if(CDesignCircularPlateDlg::GetPropID("design_axis_z.nearVector.x")==pItem->m_idProp)
	{
		pCircularDlg->design_axis_z.nearVector.x=atof(valueStr);
		bCalNorm=TRUE;	//根据参数计算法线
	}
	else if(CDesignCircularPlateDlg::GetPropID("design_axis_z.nearVector.y")==pItem->m_idProp)
	{
		pCircularDlg->design_axis_z.nearVector.y=atof(valueStr);
		bCalNorm=TRUE;	//根据参数计算法线
	}
	else if(CDesignCircularPlateDlg::GetPropID("design_axis_z.nearVector.z")==pItem->m_idProp)
	{
		pCircularDlg->design_axis_z.nearVector.z=atof(valueStr);
		bCalNorm=TRUE;	//根据参数计算法线
	}
	//连接方向参数
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.style")==pItem->m_idProp)
	{
		pCircularDlg->m_pCurSelConnVec->style=valueStr[0]-'0';
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->style=valueStr[0]-'0';
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.end_space")==pItem->m_idProp)
	{
		pCircularDlg->m_pCurSelConnVec->end_space=(short)atol(valueStr);
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->end_space=pCircularDlg->m_pCurSelConnVec->end_space;
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.high_space")==pItem->m_idProp)
	{
		pCircularDlg->m_pCurSelConnVec->high_space=(short)atol(valueStr);
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->high_space=pCircularDlg->m_pCurSelConnVec->high_space;
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.dir.norm_style")==pItem->m_idProp)//基准杆件句柄
	{
		pCircularDlg->m_pCurSelConnVec->direct.norm_style=valueStr[0]-'0';
		pCircularDlg->UpdateVectorPropList(pItem,"ConnDir.dir",pCircularDlg->m_pCurSelConnVec->direct);
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->direct.norm_style=pCircularDlg->m_pCurSelConnVec->direct.norm_style;
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.dir.hVicePart")==pItem->m_idProp)//基准杆件句柄
	{
		sscanf(valueStr,"%d",&pCircularDlg->m_pCurSelConnVec->direct.hVicePart);
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->direct.hVicePart=pCircularDlg->m_pCurSelConnVec->direct.hVicePart;
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.dir.hCrossPart")==pItem->m_idProp)
	{
		sscanf(valueStr,"%d",&pCircularDlg->m_pCurSelConnVec->direct.hCrossPart);
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->direct.hCrossPart=pCircularDlg->m_pCurSelConnVec->direct.hCrossPart;
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.dir.norm_wing")==pItem->m_idProp)
	{
		pCircularDlg->m_pCurSelConnVec->direct.norm_wing=valueStr[0]-'0';
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->direct.norm_wing=pCircularDlg->m_pCurSelConnVec->direct.norm_wing;
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.dir.direction")==pItem->m_idProp)
	{
		pCircularDlg->m_pCurSelConnVec->direct.direction=valueStr[0]-'0';
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->direct.direction=pCircularDlg->m_pCurSelConnVec->direct.direction;
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.dir.VectorType")==pItem->m_idProp)
	{
		int nVecType=valueStr[0]-'0';
		pCircularDlg->m_pCurSelConnVec->direct.vector=CStdVector::GetVector(nVecType);
		long idProp=CDesignCircularPlateDlg::GetPropID("ConnDir.dir.vector.x");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		pCircularDlg->m_xSubPropList.SetItemReadOnly(idProp,nVecType!=0);
		idProp=CDesignCircularPlateDlg::GetPropID("ConnDir.dir.vector.y");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		pCircularDlg->m_xSubPropList.SetItemReadOnly(idProp,nVecType!=0);
		idProp=CDesignCircularPlateDlg::GetPropID("ConnDir.dir.vector.z");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		pCircularDlg->m_xSubPropList.SetItemReadOnly(idProp,nVecType!=0);
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->direct.vector=pCircularDlg->m_pCurSelConnVec->direct.vector;
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.dir.vector.x")==pItem->m_idProp)
	{
		pCircularDlg->m_pCurSelConnVec->direct.vector.x=atof(valueStr);
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->direct.vector.x=pCircularDlg->m_pCurSelConnVec->direct.vector.x;
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.dir.vector.y")==pItem->m_idProp)
	{
		pCircularDlg->m_pCurSelConnVec->direct.vector.y=atof(valueStr);
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->direct.vector.y=pCircularDlg->m_pCurSelConnVec->direct.vector.y;
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.dir.vector.z")==pItem->m_idProp)
	{
		pCircularDlg->m_pCurSelConnVec->direct.vector.z=atof(valueStr);
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->direct.vector.z=pCircularDlg->m_pCurSelConnVec->direct.vector.z;
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.dir.nearVectorType")==pItem->m_idProp)
	{
		int nVecType=valueStr[0]-'0';
		pCircularDlg->m_pCurSelConnVec->direct.nearVector=CStdVector::GetVector(nVecType);
		long idProp=CDesignCircularPlateDlg::GetPropID("ConnDir.dir.nearVector.x");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		pCircularDlg->m_xSubPropList.SetItemReadOnly(idProp,nVecType!=0);
		idProp=CDesignCircularPlateDlg::GetPropID("ConnDir.dir.nearVector.y");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		pCircularDlg->m_xSubPropList.SetItemReadOnly(idProp,nVecType!=0);
		idProp=CDesignCircularPlateDlg::GetPropID("ConnDir.dir.nearVector.z");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		pCircularDlg->m_xSubPropList.SetItemReadOnly(idProp,nVecType!=0);
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->direct.nearVector=pCircularDlg->m_pCurSelConnVec->direct.nearVector;
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.dir.nearVector.x")==pItem->m_idProp)
	{
		pCircularDlg->m_pCurSelConnVec->direct.nearVector.x=atof(valueStr);
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->direct.nearVector.x=pCircularDlg->m_pCurSelConnVec->direct.nearVector.x;
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.dir.nearVector.y")==pItem->m_idProp)
	{
		pCircularDlg->m_pCurSelConnVec->direct.nearVector.y=atof(valueStr);
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->direct.nearVector.y=pCircularDlg->m_pCurSelConnVec->direct.nearVector.y;
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pCircularDlg->m_pCurSelConnVec&&CDesignCircularPlateDlg::GetPropID("ConnDir.dir.nearVector.z")==pItem->m_idProp)
	{
		pCircularDlg->m_pCurSelConnVec->direct.nearVector.z=atof(valueStr);
		if(pCircularDlg->m_pCirPlate&&pPlateCurConVec)
		{	//修改环向板连接方向参数并重新设计 wht 11-05-30
			pPlateCurConVec->direct.nearVector.z=pCircularDlg->m_pCurSelConnVec->direct.nearVector.z;
			pCircularDlg->ReDesignPlate();
		}
	}
	if(bCalNorm)	//根据参数计算法线参数
	{
		if(pCircularDlg->design_axis_z.norm_style!=0)
			pCircularDlg->design_axis_z.vector.Set();
		pCircularDlg->design_axis_z.UpdateVector(console.GetActiveModel());
		long idProp=CDesignCircularPlateDlg::GetPropID("ucs.axis_z.x");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		pCircularDlg->m_xSubPropList.SetItemReadOnly(idProp,pCircularDlg->design_axis_z.norm_style!=0);
		idProp=CDesignCircularPlateDlg::GetPropID("ucs.axis_z.y");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		pCircularDlg->m_xSubPropList.SetItemReadOnly(idProp,pCircularDlg->design_axis_z.norm_style!=0);
		idProp=CDesignCircularPlateDlg::GetPropID("ucs.axis_z.z");
		if(pCircularDlg->GetPropValueStr(idProp,sText)>0)
			pCircularDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		pCircularDlg->m_xSubPropList.SetItemReadOnly(idProp,pCircularDlg->design_axis_z.norm_style!=0);
		//更新钢板Z轴方向 wht 11-05-13
		//暂时不支持在查看参数时调整法向方向,法线参数与关联基板方向冲突 wht 11-05-13
		/*if(pCircularDlg->m_pCirPlate)
		{
			CONNECT_DIRECTION *pVec=pCircularDlg->m_pCirPlate->GetRelaConnInfo(1);
			if(pVec)
				pVec->direct=pCircularDlg->design_axis_z;
			pCircularDlg->ReDesignPlate();
		}*/
	}
	return TRUE;
}
static BOOL ButtonClickCircularPlate(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	CDesignCircularPlateDlg *pCircularDlg=(CDesignCircularPlateDlg*)pPropList->GetParent();
	pCircularDlg->SetEventPropId(0);	//初始化属性栏id记录
	pCircularDlg->m_idSubEventProp=0;	//初始化子属性栏id记录
	if(CDesignCircularPlateDlg::GetPropID("hDatumTube")==pItem->m_idProp)
		pCircularDlg->SelectObject(CLS_LINETUBE,pItem->m_idProp);	//选择基准钢管
	else if(CDesignCircularPlateDlg::GetPropID("hRelaPart1")==pItem->m_idProp
		||CDesignCircularPlateDlg::GetPropID("hRelaPart2")==pItem->m_idProp)
		pCircularDlg->SelectObject(CLS_PLATE,pItem->m_idProp);		//选择基准钢板
	else if(CDesignCircularPlateDlg::GetPropID("hDatumNode")==pItem->m_idProp)
		pCircularDlg->SelectObject(CLS_NODE,pItem->m_idProp);		//选择基准节点
	else if(CDesignCircularPlateDlg::GetPropID("hDatumSphere")==pItem->m_idProp)
		pCircularDlg->SelectObject(CLS_SPHERE,pItem->m_idProp);		//选择基准球体
	else if(CDesignCircularPlateDlg::GetPropID("design_axis_z.hVicePart")==pItem->m_idProp
		||CDesignCircularPlateDlg::GetPropID("design_axis_z.hCrossPart")==pItem->m_idProp)
	{
		long idEventProp=0;
		if(pCircularDlg->m_propList.m_pCurEditItem)		//记录主属性表中当前选中项ID
			idEventProp=pCircularDlg->m_propList.m_pCurEditItem->m_idProp;
		pCircularDlg->m_idSubEventProp=pItem->m_idProp;	//记录触发事件的属性ID
		if(pCircularDlg->design_axis_z.norm_style==1)
			pCircularDlg->SelectObject(CLS_LINEANGLE,idEventProp);	//选择基准角钢
		else if(pCircularDlg->design_axis_z.norm_style==2
				||pCircularDlg->design_axis_z.norm_style==5)
			pCircularDlg->SelectObject(CLS_LINEPART,idEventProp);	//选择基准杆件
		else 
			pCircularDlg->SelectObject(CLS_PART,idEventProp);		//选择基准构件
	}
	else if(pCircularDlg->m_pCurSelConnVec
		&&(CDesignCircularPlateDlg::GetPropID("ConnDir.dir.hVicePart")==pItem->m_idProp
		||CDesignCircularPlateDlg::GetPropID("ConnDir.dir.hCrossPart")==pItem->m_idProp))
	{
		long idEventProp=0;
		if(pCircularDlg->m_propList.m_pCurEditItem)		//记录主属性表中当前选中项ID
			idEventProp=pCircularDlg->m_propList.m_pCurEditItem->m_idProp;
		pCircularDlg->m_idSubEventProp=pItem->m_idProp;	//记录触发事件的属性ID
		pCircularDlg->SetPickObjTypeId(CLS_PART);
		if(pCircularDlg->design_axis_z.norm_style==1)
			pCircularDlg->SelectObject(CLS_LINEANGLE,idEventProp);	//选择基准角钢
		else if(pCircularDlg->design_axis_z.norm_style==2
			||pCircularDlg->design_axis_z.norm_style==5)
			pCircularDlg->SelectObject(CLS_LINEPART,idEventProp);	//选择基准杆件
		else 
			pCircularDlg->SelectObject(CLS_PART,idEventProp);		//选择基准构件
	}
	else if(CDesignCircularPlateDlg::GetPropID("RelaPole")==pItem->m_idProp)
	{
		pCircularDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);
#ifdef AFX_TARG_ENU_ENGLISH
		pCircularDlg->SetCmdPickPromptStr("Please Select Circular Plate's Relative Rods<Enter-Key To Confirm>:");
#else 
		pCircularDlg->SetCmdPickPromptStr("请选择环向板关联杆件<回车确认>:");
#endif
	}
	else if(CDesignCircularPlateDlg::GetPropID("RelaBolt")==pItem->m_idProp)
	{
		pCircularDlg->m_bOpenWndSel=TRUE;	//框选螺栓 wht 11-05-16
		pCircularDlg->SelectObject(CLS_BOLT,pItem->m_idProp);
#ifdef AFX_TARG_ENU_ENGLISH
		pCircularDlg->SetCmdPickPromptStr("Please Select Circular Plate's Relative Bolts<Enter-Key To Confirm>:");
#else 
		pCircularDlg->SetCmdPickPromptStr("请选择环向板关联螺栓<回车确认>:");
#endif
	}
	else if(CDesignCircularPlateDlg::GetPropID("RelaPole")==pItem->m_pParent->m_idProp)
	{	//删除关联杆件
		CPropTreeItem *pParentItem=pItem->m_pParent;
		BYTE serial=pCircularDlg->ConnVecList.DeleteRelaConnVecByFeature(pItem->m_idProp);
		pPropList->DeleteItemByPropId(pItem->m_idProp);	//删除当前项
		pPropList->HideInputCtrl();
		//更新射线杆件数
		char sText[101]="";
		if(pCircularDlg->GetPropValueStr(pParentItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pParentItem->m_idProp,sText);
		//更新环向板关联构件列表 wht 11-05-18
		if(pCircularDlg->m_pCirPlate)
		{
			pCircularDlg->m_pCirPlate->DeleteRelaConnVec(serial);
			pCircularDlg->ReDesignPlate();	//重新设计环向板
		}
		if(pCircularDlg->m_iPlateType==0)
		{	//普通环向板需要扇形起始角度参数 wht 11-05-11
			long idProp=CDesignCircularPlateDlg::GetPropID("para.A");
			CPropTreeItem *pFindItem=pCircularDlg->m_propList.FindItemByPropId(idProp,NULL);
			CPropTreeItem *pParentItem=pCircularDlg->m_propList.FindItemByPropId(CDesignCircularPlateDlg::GetPropID("location"),NULL);
			if(pCircularDlg->ConnVecList.GetCount(TRUE)<=0&&pFindItem==NULL&&pParentItem)
			{	//扇形起始角
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Sector Start Angle α";
				lpInfo->m_strPropHelp = "The start angle α°of sector circular plate";
#else 
				lpInfo->m_strPropName = "扇形起始角α";
				lpInfo->m_strPropHelp = "扇形环向板的起始角α°";
#endif
				CPropTreeItem *pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = idProp;
				if(pCircularDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				//pPropItem->SetPropHelpPromptFunc(FireCircularPlateHelpStr);
			}
		}
		return FALSE;
	}
	else if(CDesignCircularPlateDlg::GetPropID("axis_z")==pItem->m_idProp)
	{
		pCircularDlg->design_axis_z.UpdateVector(console.GetActiveModel());
		CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Normal Direction";
#else 
		vecDlg.m_sCaption = "法线方向";
#endif
		vecDlg.m_fVectorX = pCircularDlg->design_axis_z.vector.x;
		vecDlg.m_fVectorY = pCircularDlg->design_axis_z.vector.y;
		vecDlg.m_fVectorZ = pCircularDlg->design_axis_z.vector.z;
		CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CDesignCircularPlateDlg::GetPropID("ucs.axis_z.x"),NULL);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CDesignCircularPlateDlg::GetPropID("ucs.axis_z.y"),NULL);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CDesignCircularPlateDlg::GetPropID("ucs.axis_z.z"),NULL);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK&&pCircularDlg->design_axis_z.norm_style==0)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CDesignCircularPlateDlg::GetPropID("ucs.axis_z.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CDesignCircularPlateDlg::GetPropID("ucs.axis_z.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CDesignCircularPlateDlg::GetPropID("ucs.axis_z.z"),CString(sText));
			pCircularDlg->design_axis_z.vector.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.S")==pItem->m_idProp)
	{
		double up_len=0,down_len=0;
		if(pCircularDlg->m_pRelaPart1==NULL&&pCircularDlg->m_pRelaPart2==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Please switch to【Sealing Plate】select one relative datum plate,then switch back to【Circular Plate】,click the button to calculate offset again！");
#else 
			AfxMessageBox("请切换至【封板】选择一块关联基板，再切换回【环向板】,再次点击该按钮计算偏移量！");
#endif
			return FALSE;
		}
		pCircularDlg->CalMaxMinOffsetDist(up_len,down_len);
#ifdef AFX_TARG_ENU_ENGLISH
		int nRetCode=AfxMessageBox("Select【Yes】to calculate up offset,select【No】to calculate down offset!",MB_YESNOCANCEL);
#else 
		int nRetCode=AfxMessageBox("选择【是】计算上侧偏移距离,选择【否】计算下侧偏移距离!",MB_YESNOCANCEL);
#endif
		char sText[100]="";
		if(nRetCode==IDYES)
			pCircularDlg->para.S=up_len;
		else if(nRetCode==IDNO)
			pCircularDlg->para.S=down_len;
		//根据计算结果更新偏移量 wht 11-05-31
		if(nRetCode!=IDCANCEL&&pCircularDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,sText);
	}
	return TRUE;
}

CDesignCircularPlateDlg::CDesignCircularPlateDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesignCircularPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignCircularPlateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDatumSphere=NULL;
	m_pDatumNode=NULL;
	m_pDatumTube=NULL;
	m_pRelaPart1=m_pRelaPart2=NULL;
	m_iPlateType=0;
	memset(&para,0,sizeof(CIRCULAR_PARAM));
	para.Y=0;
	para.gama=45;
	memset(&water,0,sizeof(PARAM_WATER));
	para.r=-1;	//根据钢管生成内圆
	thick=8;
	para.bWeldSemicircleMode = FALSE;
	//m_sSeg=0;
	m_cMaterial='S';
	m_pCurSelConnVec=NULL;
	m_pCirPlate=NULL;
	m_bNewCirPlate=TRUE;
	m_bReadOnlyPlateType=FALSE;
	m_sWndTitle=_T("");
	m_bDisplayDlg=TRUE;
}


void CDesignCircularPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignCircularPlateDlg)
	DDX_Control(pDX, IDC_LIST_BOX_SUB, m_xSubPropList);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignCircularPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignCircularPlateDlg)
	ON_BN_CLICKED(IDC_BTN_CONFIRM, OnConfirm)
	ON_BN_CLICKED(IDC_BTN_PARTS_DB, OnPartsDb)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////
// CDesignCircularPlateDlg message handlers

BOOL CDesignCircularPlateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_sWndTitle.GetLength()>0)
		SetWindowText(m_sWndTitle);
	InitPropHashtable();
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.SetDividerScale(0.5);
	m_propList.SetModifyValueFunc(ModifyCircularPlateProperty);
	m_propList.SetButtonClickFunc(ButtonClickCircularPlate);
	m_propList.SetPropHelpPromptFunc(FireCircularPlateHelpStr);
	//
	m_xSubPropList.SetDividerScale(0.6);
	m_xSubPropList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xSubPropList.SetModifyValueFunc(ModifyCircularPlateProperty);
	m_xSubPropList.SetButtonClickFunc(ButtonClickCircularPlate);
	m_xSubPropList.SetPropHelpPromptFunc(FireCircularPlateHelpStr);
	//更新界面
	CWnd *pWnd=GetDlgItem(IDC_BTN_PARTS_DB);
	if(pWnd&&m_bNewCirPlate)
		pWnd->ShowWindow(SW_SHOW);
	else if(pWnd)
		pWnd->ShowWindow(SW_HIDE);
	pWnd=GetDlgItem(IDC_BTN_CONFIRM);
	if(pWnd&&m_bNewCirPlate)
		pWnd->ShowWindow(SW_SHOW);
	else if(pWnd)
		pWnd->ShowWindow(SW_HIDE);
	if(!m_bNewCirPlate)
#ifdef AFX_TARG_ENU_ENGLISH
		SetDlgItemText(IDCANCEL,"Close");
#else 
		SetDlgItemText(IDCANCEL,"关闭");
#endif
	//内部启动
	if(m_bInernalStart)	
		FinishSelectObjOper();	//完成选择对象的后续操作
	else if(m_bNewCirPlate)
	{	//自动判断环向板类型
		if(m_iPlateType<=0)
		{	//未指定钢板类型时根据所选构件进行初始化 wht 11-05-23
			if(m_pDatumTube==NULL||m_pDatumNode==NULL)
				m_iPlateType=0;	//环向板
			else if(m_pRelaPart1&&m_pRelaPart2)
			{
				if(m_pRelaPart1->IsPlate()&&m_pRelaPart2->GetClassTypeId()==CLS_LINETUBE)
				{
					m_iPlateType=3;	//环向肋板
					para.nDHRelativeType=2; //D、H为独立参数 即跨越连板
					CONNECT_DIRECTION *pConnDir=ConnVecList.Append();
					pConnDir->style=1;	//焊接面方向
					pConnDir->high_space=pConnDir->end_space=30;
					pConnDir->serial=1;
					pConnDir->direct.norm_style=5;
					pConnDir->direct.hVicePart=m_pDatumTube->handle;
					pConnDir->direct.direction=0;
					//
					pConnDir=ConnVecList.Append();
					pConnDir->style=0;	//焊缝走向
					pConnDir->high_space=pConnDir->end_space=30;
					pConnDir->serial=2;
					pConnDir->direct.norm_style=5;
					pConnDir->direct.hVicePart=m_pRelaPart2->handle;
					CLDSLineTube *pRelaTube=(CLDSLineTube*)m_pRelaPart2;
					if(pRelaTube->pStart==m_pDatumNode)
						pConnDir->direct.direction=0;	//始->终
					else 
						pConnDir->direct.direction=1;	//终->始
				}
				else 
				{
					m_iPlateType=2;	//环向肋板
					para.nDHRelativeType=2; //启用H参数
				}
			}
			else if(m_pRelaPart1&&m_pRelaPart2==NULL)
			{
				if(m_pRelaPart1->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)m_pRelaPart1)->IsTransPlate())
				{	//关联基板为贯通连板 wht 11-05-13
					m_iPlateType=3;
					para.nDHRelativeType=1;
				}
				else if(m_pRelaPart1->GetClassTypeId()==CLS_PARAMPLATE)
				{
					CLDSParamPlate *pParamPlate=(CLDSParamPlate*)m_pRelaPart1;
					if(pParamPlate->IsFL()
						||pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
						m_iPlateType=6;	//遮水板
					else 
						m_iPlateType=5;	//2WA肋板
				}
				else 
				{
					if(m_pDatumTube->GetDiameter()<200)
						m_iPlateType=4;	//2WL肋板
					else 
						m_iPlateType=5;	//2WA肋板
				}
			}
		}
		if(m_pCirPlate==NULL)
			InitCirPlatePara();
		//初始化螺栓布置参数
		m_xLayoutBolt.m_iBoltSegI=SEGI(m_sSeg);
		m_xLayoutBolt.m_nBoltD=20;
		if(m_pDatumTube)
			m_xLayoutBolt.m_fBoltLayoutR=m_pDatumTube->GetDiameter()*0.5+30;
	}
	else if(m_pCirPlate)
	{	//查看环向板属性时用以下代码初始化对话框参数 wht 11-05-12
		strcpy(m_sPartNo,m_pCirPlate->GetPartNo());
		m_sSeg=m_pCirPlate->iSeg.ToString();
		m_cMaterial=m_pCirPlate->cMaterial;
		thick=m_pCirPlate->thick;
		//初始化螺栓引用列表
		LsRefList.Empty();
		CLsRef *pLsRef=0;
		for(pLsRef=m_pCirPlate->GetFirstLsRef();pLsRef;pLsRef=m_pCirPlate->GetNextLsRef())
			LsRefList.append(*pLsRef);
		if(m_pCirPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
		{
			long hNode=0,hDatumTube=0;
			m_pCirPlate->GetDesignItemValue('B',&hNode);
			m_pCirPlate->GetDesignItemValue('C',&hDatumTube);
			m_pCirPlate->GetDesignItemValue('D',&water.D);
			m_pCirPlate->GetDesignItemValue('W',&water.W);
			m_pCirPlate->GetDesignItemValue('S',&water.S);
			m_pCirPlate->GetDesignItemValue('K',&water.K);
			m_pCirPlate->GetDesignItemValue('H',&water.H);
			m_pCirPlate->GetDesignItemValue('L',&water.L);
			m_pCirPlate->GetDesignItemValue('A',&water.A);
			m_pCirPlate->GetDesignItemValue('O',&water.O);
			m_pCirPlate->GetDesignItemValue('E',&water.E);
			m_pDatumNode=console.FromNodeHandle(hNode);
			m_pDatumTube=(CLDSLineTube*)console.FromPartHandle(hDatumTube,CLS_LINETUBE);
			m_iPlateType=6;	//遮水板
		}
		else if(m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{
			m_pCirPlate->GetDesignItemValue('A',&para.A);
			m_pCirPlate->GetDesignItemValue('B',&para.B);
			m_pCirPlate->GetDesignItemValue('C',&para.C);
			m_pCirPlate->GetDesignItemValue('E',&para.P);
			m_pCirPlate->GetDesignItemValue('F',&para.E);
			m_pCirPlate->GetDesignItemValue('G',&para.F);
			m_pCirPlate->GetDesignItemValue('D',&para.H);
			m_pCirPlate->GetDesignItemValue('W',&para.W);
			m_pCirPlate->GetDesignItemValue('S',&para.S);
			m_pCirPlate->GetDesignItemValue('R',&para.R);
			m_pCirPlate->GetDesignItemValue('N',&para.N);
			m_pCirPlate->GetDesignItemValue(KEY2C("R1"),&para.R1);
			m_pCirPlate->GetDesignItemValue(KEY2C("R2"),&para.R2);
			para.bWeldSemicircleMode=(para.R1>0||para.R2>0);
			para.nCirCoverPlateType=0;
			m_pCirPlate->GetDesignItemValue('Y',&para.nCirCoverPlateType);	//封板类型
			CLDSPart *pPart=console.FromPartHandle(para.C,CLS_LINETUBE,CLS_SPHERE);
			if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
				m_pDatumTube=(CLDSLineTube*)pPart;
			else if(pPart)
				m_pDatumSphere=(CLDSSphere*)pPart;
			m_pDatumNode=console.FromNodeHandle(para.P);
			m_pRelaPart1=console.FromPartHandle(para.E,CLS_PLATE,CLS_PARAMPLATE);
			m_pRelaPart2=console.FromPartHandle(para.F,CLS_PLATE,CLS_LINETUBE,CLS_PARAMPLATE);
			if(m_pDatumTube)
				para.V=para.R-m_pDatumTube->GetDiameter()*0.5;
			m_iPlateType=1;	//环向封板
			if(para.S<0)	
				para.S=1;
			else 
				para.S=-1;
		}
		else
		{
			m_pCirPlate->GetDesignItemValue('A',&para.A);	//A:α,环向定位角(定位参数)
			m_pCirPlate->GetDesignItemValue('B',&para.B);	//B:β,环向扇形角(外形参数)
			m_pCirPlate->GetDesignItemValue('C',&para.C);	//C:定位基准钢管(球体)句柄(定位参数)
			m_pCirPlate->GetDesignItemValue('D',&para.D);	//D:基准钢管外壁到自由边距离，简称自由边距
			m_pCirPlate->GetDesignItemValue('E',&para.E);	//E:第一块关联基准钢板句柄
			m_pCirPlate->GetDesignItemValue('F',&para.F);	//F:第二块关联基准钢板句柄，或焊接关联钢管句柄(关联钢管是指与环向板有焊缝的钢管，不含上面的插板螺栓连接钢管，因此不可能出现两根)
			m_pCirPlate->GetDesignItemValue('G',&para.G);	//G:自由边类型(>0直线，<=0圆弧)
			m_pCirPlate->GetDesignItemValue('H',&para.H);	//H:台口高度
			if(!m_pCirPlate->GetDesignItemValue('h',&para.h))	//H:N侧台口高度
				para.h=para.H;
			m_pCirPlate->GetDesignItemValue('I',&para.I);	//I:第一根连接方向钢管句柄(指明有此钢管方向的连接意图，即插板螺栓连接或无连接制弯)
			m_pCirPlate->GetDesignItemValue('J',&para.J);	//J:第二根连接方向钢管句柄(指明有此钢管方向的连接意图，即插板螺栓连接或无连接制弯)
			m_pCirPlate->GetDesignItemValue('P',&para.P);	//P:定位基准节点句柄
			m_pCirPlate->GetDesignItemValue('R',&para.R);	//R:环向板外圆弧半径
			m_pCirPlate->GetDesignItemValue('r',&para.r);	//r:环向板内圆半径(异形法兰或360的环向板使用 r<0根据钢管生成内圆 r=0无内圆 r>0按指定半径生成内圆) wht 11-05-27
			m_pCirPlate->GetDesignItemValue('S',&para.S);	//S:自定位基准节点向定位原点的轴向偏移距离
			m_pCirPlate->GetDesignItemValue('T',&para.T);	//T:自定位基准节点向定位原点的轴向偏移距离在环向板法线上的投影距离
			m_pCirPlate->GetDesignItemValue('V',&para.V);	//V:加工间隙(基准钢管外壁到环向板内环的预留间隙)
			m_pCirPlate->GetDesignItemValue('Y',&para.Y);	//Y:半圆型环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)
			m_pCirPlate->GetDesignItemValue('y',&para.gama);//2WA型环向肋板倒角角度,单位°
			m_pCirPlate->GetDesignItemValue('W',&para.W);	//W:第一块关联钢板的焊缝长
			m_pCirPlate->GetDesignItemValue('N',&para.N);	//N:第二块关联钢板或钢管的焊缝长
			//位置偏移类型 wht 11-05-26
			if(fabs(para.T)>EPS)
			{	//法线偏移
				para.nOffsetType=1;
				para.S=para.T;
			}
			else //if(fabs(para.S)>EPS)
				para.nOffsetType=0;	//纵向偏移
			//初始化法线方向
			ConnVecList.Empty();
			CONNECT_DIRECTION *pVec=NULL,*pFirstVec=NULL;
			for(pVec=m_pCirPlate->ConnDirectList.GetFirst();pVec;pVec=m_pCirPlate->ConnDirectList.GetNext())
			{
				ConnVecList.Append(*pVec);
				if(pVec->serial==1)
					pFirstVec=pVec;
			}
			/*if(pFirstVec)
				design_axis_z=pFirstVec->direct;
			else
			{
				design_axis_z.norm_style=0;
				design_axis_z.vector=m_pCirPlate->ucs.axis_z;
			}*/
			design_axis_z=m_pCirPlate->workNorm;	//以上代友估计是以前遗留的考虑不周的临时代码 wjh-2011.11.22
			//初始化关联构件
			CLDSPart *pPart=console.FromPartHandle(para.C,CLS_LINETUBE,CLS_SPHERE);
			if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
				m_pDatumTube=(CLDSLineTube*)pPart;
			else if(pPart)
				m_pDatumSphere=(CLDSSphere*)pPart;
			m_pDatumNode=console.FromNodeHandle(para.P);
			m_pRelaPart1=console.FromPartHandle(para.E,CLS_PLATE,CLS_PARAMPLATE);
			m_pRelaPart2=console.FromPartHandle(para.F,CLS_PLATE,CLS_LINETUBE,CLS_PARAMPLATE);
			//初始化环向板类型
			if(m_pRelaPart1==NULL&&m_pRelaPart2==NULL)
				m_iPlateType=0;	//环向板
			else if(m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE)
			{
				if(m_pRelaPart1&&m_pRelaPart2)
				{
					if(para.G>0)	//自由边为直线
					{
						m_iPlateType=2;
						if(para.D>0)
							para.nDHRelativeType=1;	//关联且启用D参数
						else 
							para.nDHRelativeType=2;	//关联且启用H参数
					}
					else 
						m_iPlateType=3;
				}
				else if(m_pRelaPart1&&m_pRelaPart2==NULL)
				{
					if(para.G>0)	//自由边为直线
						m_iPlateType=4;
					else 
						m_iPlateType=5;
				}
			}
			else if(m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE)
			{
				m_iPlateType=3;	//3WA
				para.nDHRelativeType=1;	//D,H为独立参数
			}
		}
	}
	//显示环向板参数
	DisplayPlatePropList();
	//显示子属性
	DisplaySubPropList(NULL);
	if(m_bInernalStart)	
	{	//恢复选中项
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		m_propList.SetFocus();	//属性栏获得焦点
		if(pItem)
			m_propList.SetCurSel(pItem->m_iIndex);	//设置选中项
		DisplaySubPropList(pItem);	//显示子属性
		CPropTreeItem *pSubItem=m_xSubPropList.FindItemByPropId(m_idSubEventProp,NULL);
		m_xSubPropList.SetFocus();
		if(pSubItem)	//选中子属性
			m_xSubPropList.SetCurSel(pSubItem->m_iIndex);
	}
	UpdateSketchMap();
	//
	CRect rect,rc;
	GetWindowRect(rect);
	rc.top=rc.left=0;
	rc.right=rect.Width();
	rc.bottom=rect.Height();
	MoveWindow(rc,TRUE);
	return TRUE;
}
IMPLEMENT_PROP_FUNC(CDesignCircularPlateDlg);
const DWORD HASHTABLESIZE = 500;
void CDesignCircularPlateDlg::InitEngPropHashtable()
{
	int id = 1;
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Basic Infomation"));
	AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Label"));
	AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	AddPropItem("material",PROPLIST_ITEM(id++,"Material","",MakeMaterialMarkSetString()));
	AddPropItem("thick",PROPLIST_ITEM(id++,"Thickness T"));
	AddPropItem("type",PROPLIST_ITEM(id++,"Circular plate type","","0.Common Type|1.Sealing Plate|2.Rib Plate 3WL|3.Rib Plate 3WA|4.Rib Plate 2WL|5.Rib Plate 2WA|6.Shelter Plate"));
	AddPropItem("location",PROPLIST_ITEM(id++,"Assemble Position"));
	AddPropItem("axis_z",PROPLIST_ITEM(id++,"Normal Direction"));
	AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Work Normal","",CStdVector::MakeVectorSetStr()));
	AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z"));
	//
	AddPropItem("RelaInfo",PROPLIST_ITEM(id++,"Relative Information"));
	AddPropItem("hDatumSphere",PROPLIST_ITEM(id++,""));
	AddPropItem("hDatumTube",PROPLIST_ITEM(id++,"Datum Tube","Locating datum tube's handle."));
	AddPropItem("hDatumNode",PROPLIST_ITEM(id++,"Datum Node","Locating datum node's handle"));
	AddPropItem("hRelaPart1",PROPLIST_ITEM(id++,"Relative Datum Plate","The first relative plate's handle(W side)."));
	AddPropItem("hRelaPart2",PROPLIST_ITEM(id++,"Relative Part2","The second relative part's handle(The plate or tube on N side"));
	AddPropItem("RelaPole",PROPLIST_ITEM(id++,"Relative Rods","he plate's relative rods is to determine palte's profile","Add Rods"));
	AddPropItem("RelaBolt",PROPLIST_ITEM(id++,"Relative Bolts","Select bolts which belongs to plate","Select Bolts"));
	AddPropItem("CreateBolt",PROPLIST_ITEM(id++,"Bolts Layout Parameter"));
	AddPropItem("m_xLayoutBolt",PROPLIST_ITEM(id++,"Circular Layout Bolts"));
	AddPropItem("m_xLayoutBolt.m_iBoltSegI",PROPLIST_ITEM(id++,"Bolt Segment No"));		
	AddPropItem("m_xLayoutBolt.m_iDriection",PROPLIST_ITEM(id++,"Bolt Direction","","0.Outward|1.Inward"));
	AddPropItem("m_xLayoutBolt.m_nBoltD",PROPLIST_ITEM(id++,"Bolt Diameter","","12|16|20|24|27|30"));
	AddPropItem("m_xLayoutBolt.m_nBoltNum",PROPLIST_ITEM(id++,"Bolt Number"));
	AddPropItem("m_xLayoutBolt.m_iStartEdge",PROPLIST_ITEM(id++,"Start Edge","","0.Datum Edge|1.Weld Line")); 
	AddPropItem("m_xLayoutBolt.m_fOffsetAngle",PROPLIST_ITEM(id++,"1st Bolt Offset Angle"));
	AddPropItem("m_xLayoutBolt.m_fBoltLayoutR",PROPLIST_ITEM(id++,"Bolt Layout Radius"));
	//
	AddPropItem("profile",PROPLIST_ITEM(id++,"Profile Design"));
	AddPropItem("tube_r",PROPLIST_ITEM(id++,"Tube Radius R"));
	AddPropItem("para.A",PROPLIST_ITEM(id++,"Sector Start Angle α","The start angle α°of sector circular plate"));
	AddPropItem("para.B",PROPLIST_ITEM(id++,"Section Angle β"));
	AddPropItem("para.D",PROPLIST_ITEM(id++,"Free Margin D"));
	AddPropItem("para.H",PROPLIST_ITEM(id++,"Step Hatch Height H"));
	AddPropItem("para.h",PROPLIST_ITEM(id++,"Step Hatch Height Hn"));
	AddPropItem("para.R",PROPLIST_ITEM(id++,"Outer Circle Radius Ro"));
	AddPropItem("innercir_type",PROPLIST_ITEM(id++,"Inner Circle Type","","0.No Inner Circle|1.Specify Inner Circle R|2.Create By Tube"));
	AddPropItem("para.r",PROPLIST_ITEM(id++,"Inner Circle Radius r"));
	AddPropItem("para.S",PROPLIST_ITEM(id++,"Position Offset S","Normal offset include normal offset along the plate's Z axis offset，vertical offset is along tube's direction offset"));
	AddPropItem("para.V",PROPLIST_ITEM(id++,""));
	AddPropItem("para.W",PROPLIST_ITEM(id++,"Circular Band Width W"));
	AddPropItem("para.R1",PROPLIST_ITEM(id++,"R For Plate1"));
	AddPropItem("para.R2",PROPLIST_ITEM(id++,"R For Plate2"));
	AddPropItem("para.N",PROPLIST_ITEM(id++,"Parameter N"));
	AddPropItem("para.Y",PROPLIST_ITEM(id++,"Circular Plate Location","","Datum Plate Outside|Datum Plate Inside|Default"));
	AddPropItem("para.y",PROPLIST_ITEM(id++,"Chamfer Angle γ"));
	AddPropItem("para.nOffsetType",PROPLIST_ITEM(id++,"Offset Type","","0.Vertical Offset|1.Normal Offset"));
	AddPropItem("para.nDHRelativeType",PROPLIST_ITEM(id++,""));
	AddPropItem("para.bCreateMirPlate",PROPLIST_ITEM(id++,"Create Another Side Sealing Plate","","Yes|No"));
	AddPropItem("para.nCirCoverPlateType",PROPLIST_ITEM(id++,"Sealing Plate Type","Common sealing plate:section angle Y+,section angle Y-","Common Sealing Plate|Two Simple Sealing Plate"));
	//
	AddPropItem("water.W",PROPLIST_ITEM(id++,"Outer Circle Diameter W"));
	AddPropItem("water.S",PROPLIST_ITEM(id++,"Position Offset S"));
	AddPropItem("water.K",PROPLIST_ITEM(id++,"Water Tank K"));
	AddPropItem("water.H",PROPLIST_ITEM(id++,"Water Tank H"));
	AddPropItem("water.L",PROPLIST_ITEM(id++,"Water Tank L"));
	AddPropItem("water.A",PROPLIST_ITEM(id++,"Two Water Tank Angle β"));
	AddPropItem("water.O",PROPLIST_ITEM(id++,"Offset Angle","The offset angle between water tank's bisector and datum edge(°)"));
	AddPropItem("water.E",PROPLIST_ITEM(id++,"Datum Edge","","0.Datum Edge|1.Weld Line"));
	//法线设计参数
	AddPropItem("design_axis_z",PROPLIST_ITEM(id++,"Normal design param"));
	AddPropItem("design_axis_z.norm_style",PROPLIST_ITEM(id++,"Normal Calculate Type","","0.Specify Direction|1.Single Angle Leg Normal|2.Two Rod Cross-bracing Normal|3.Part Work Normal|4.Part Relative Direction|5.Rod Characteristic Direction"));
	AddPropItem("design_axis_z.hVicePart",PROPLIST_ITEM(id++,"Ray Part"));
	AddPropItem("design_axis_z.hCrossPart",PROPLIST_ITEM(id++,"Cross Rod"));
	AddPropItem("design_axis_z.norm_wing",PROPLIST_ITEM(id++,"Datum Leg","","0.X Leg Normal|1.Y Leg Normal"));
	AddPropItem("design_axis_z.direction",PROPLIST_ITEM(id++,""));
	AddPropItem("design_axis_z.VectorType",PROPLIST_ITEM(id++,"Specify Direction","",CStdVector::MakeVectorSetStr()));
	AddPropItem("design_axis_z.vector.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("design_axis_z.vector.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("design_axis_z.vector.z",PROPLIST_ITEM(id++,"Z"));
	AddPropItem("design_axis_z.nearVectorType",PROPLIST_ITEM(id++,"near vector","",CStdVector::MakeVectorSetStr()));
	AddPropItem("design_axis_z.nearVector.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("design_axis_z.nearVector.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("design_axis_z.nearVector.z",PROPLIST_ITEM(id++,"Z"));
	//关联方向
	AddPropItem("ConnDir",PROPLIST_ITEM(id++,"Connect Direction Parameter"));
	for(int i=1;i<=10;i++)
		AddPropItem(CXhChar50("ConnDir%d",i),PROPLIST_ITEM(id++,CXhChar50("Relative Rods %d",i),"","delete"));
	AddPropItem("ConnDir.style",PROPLIST_ITEM(id++,"Connect Direction Type","","0.Weld Seam Direction|1.Connect Plane Normal"));
	AddPropItem("ConnDir.end_space",PROPLIST_ITEM(id++,"End Space"));
	AddPropItem("ConnDir.high_space",PROPLIST_ITEM(id++,"End Space Height"));
	AddPropItem("ConnDir.dir",PROPLIST_ITEM(id++,""));
	AddPropItem("ConnDir.dir.norm_style",PROPLIST_ITEM(id++,"Normal Calculate Type","","0.Specify Direction|1.Single Angle Leg Normal|2.Two Rod Cross-bracing Normal|3.Part Work Normal|4.Part Relative Direction|5.Rod Characteristic Direction"));
	AddPropItem("ConnDir.dir.hVicePart",PROPLIST_ITEM(id++,"Ray Part"));
	AddPropItem("ConnDir.dir.hCrossPart",PROPLIST_ITEM(id++,"Cross Rod"));
	AddPropItem("ConnDir.dir.norm_wing",PROPLIST_ITEM(id++,"Datum Leg","","0.X Leg Normal|1.Y Leg Normal"));
	AddPropItem("ConnDir.dir.direction",PROPLIST_ITEM(id++,"Direction Type","","0.Start->End|1.End->Start"));
	AddPropItem("ConnDir.dir.VectorType",PROPLIST_ITEM(id++,"Specify Direction","",CStdVector::MakeVectorSetStr()));
	AddPropItem("ConnDir.dir.vector.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("ConnDir.dir.vector.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("ConnDir.dir.vector.z",PROPLIST_ITEM(id++,"Z"));
	AddPropItem("ConnDir.dir.nearVectorType",PROPLIST_ITEM(id++,"near vector","",CStdVector::MakeVectorSetStr()));
	AddPropItem("ConnDir.dir.nearVector.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("ConnDir.dir.nearVector.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("ConnDir.dir.nearVector.z",PROPLIST_ITEM(id++,"Z"));
}
void CDesignCircularPlateDlg::InitPropHashtable()
{
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(50);
#ifdef AFX_TARG_ENU_ENGLISH
	InitEngPropHashtable();
#else 
	int id = 1;
	CXhChar100 helpStr;
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"基本信息"));
	AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	AddPropItem("iSeg",PROPLIST_ITEM(id++,"段号"));
	AddPropItem("material",PROPLIST_ITEM(id++,"材质","材质",MakeMaterialMarkSetString()));
	AddPropItem("thick",PROPLIST_ITEM(id++,"厚度T","厚度参数T"));
	AddPropItem("type",PROPLIST_ITEM(id++,"环向板类型","","0.普通环板|1.封板|2.肋板3WL|3.肋板3WA|4.肋板2WL|5.肋板2WA|6.遮水板"));
	AddPropItem("location",PROPLIST_ITEM(id++,"装配方位","装配方位参数"));
	AddPropItem("axis_z",PROPLIST_ITEM(id++,"法线方向","环板法线方向"));
	AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"工作法向","",CStdVector::MakeVectorSetStr()));
	AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X","X分量"));
	AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y","Y分量"));
	AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z","Z分量"));
	//
	AddPropItem("RelaInfo",PROPLIST_ITEM(id++,"关联信息"));
	AddPropItem("hDatumSphere",PROPLIST_ITEM(id++,""));
	AddPropItem("hDatumTube",PROPLIST_ITEM(id++,"基准钢管","定位基准钢管句柄"));
	AddPropItem("hDatumNode",PROPLIST_ITEM(id++,"基准节点","定位基准节点句柄"));
	AddPropItem("hRelaPart1",PROPLIST_ITEM(id++,"关联基板","第一块关联钢板(即W侧)的句柄"));
	AddPropItem("hRelaPart2",PROPLIST_ITEM(id++,"关联构件2","第二个关联构件(即N侧钢板或钢管)的句柄"));
	AddPropItem("RelaPole",PROPLIST_ITEM(id++,"关联杆件","钢板关联杆件用来确定钢板外形","添加杆件"));
	AddPropItem("RelaBolt",PROPLIST_ITEM(id++,"关联螺栓","选择属于钢板的螺栓","选择螺栓"));
	AddPropItem("CreateBolt",PROPLIST_ITEM(id++,"螺栓布置参数"));
	AddPropItem("m_xLayoutBolt",PROPLIST_ITEM(id++,"螺栓环向布置参数"));
	AddPropItem("m_xLayoutBolt.m_iBoltSegI",PROPLIST_ITEM(id++,"螺栓段号"));		
	AddPropItem("m_xLayoutBolt.m_iDriection",PROPLIST_ITEM(id++,"螺栓朝向","","0.朝外|1.朝内"));
	AddPropItem("m_xLayoutBolt.m_nBoltD",PROPLIST_ITEM(id++,"螺栓直径","","12|16|20|24|27|30"));
	AddPropItem("m_xLayoutBolt.m_nBoltNum",PROPLIST_ITEM(id++,"螺栓个数"));
	AddPropItem("m_xLayoutBolt.m_iStartEdge",PROPLIST_ITEM(id++,"起始边","环向布置螺栓时的起始边","0.基准边|1.焊道线")); 
	AddPropItem("m_xLayoutBolt.m_fOffsetAngle",PROPLIST_ITEM(id++,"首螺栓偏移角度"));
	AddPropItem("m_xLayoutBolt.m_fBoltLayoutR",PROPLIST_ITEM(id++,"螺栓所在半径"));
	//
	AddPropItem("profile",PROPLIST_ITEM(id++,"外形设计"));
	AddPropItem("tube_r",PROPLIST_ITEM(id++,"钢管半径R","基准钢管半径"));
	AddPropItem("para.A",PROPLIST_ITEM(id++,"扇形起始角α","扇形环向板的起始角α°"));
	AddPropItem("para.B",PROPLIST_ITEM(id++,"扇形角β","环向板扇形夹角β,单位°"));
	AddPropItem("para.D",PROPLIST_ITEM(id++,"自由边距D"));
	AddPropItem("para.H",PROPLIST_ITEM(id++,"台口高度H"));
	AddPropItem("para.h",PROPLIST_ITEM(id++,"台口高度Hn","N侧台口高度H"));
	AddPropItem("para.R",PROPLIST_ITEM(id++,"外圆半径Ro"));
	AddPropItem("innercir_type",PROPLIST_ITEM(id++,"内圆类型","","0.无内圆|1.指定内圆半径|2.根据钢管生成"));
	AddPropItem("para.r",PROPLIST_ITEM(id++,"内圆半径r"));
	AddPropItem("para.S",PROPLIST_ITEM(id++,"位置偏移S","位置偏移包括法向偏移即沿钢板Z轴方向的偏移，纵向偏移即沿主管延伸方向(且与Z轴同向的方向)的偏移"));
	AddPropItem("para.V",PROPLIST_ITEM(id++,"加工间隙V"));
	AddPropItem("para.W",PROPLIST_ITEM(id++,"环形带宽W"));
	AddPropItem("para.bMode",PROPLIST_ITEM(id++,"焊接处生成半圆孔","是否在封板与基准钢管焊接位置生成半圆孔","是|否"));
	AddPropItem("para.R1",PROPLIST_ITEM(id++,"半圆孔半径1"));
	AddPropItem("para.R2",PROPLIST_ITEM(id++,"半圆孔半径2"));
	AddPropItem("para.N",PROPLIST_ITEM(id++,"参数N"));
	helpStr.Copy("自动判断时,如果存在两块关联钢板则默认在两钢板夹角锐角\n侧生成环向板，如果为一块关联钢板则默认在基板外侧生成环向板。");
	AddPropItem("para.Y",PROPLIST_ITEM(id++,"环形板方位",helpStr,"基板外侧|基板内侧|自动判断"));
	AddPropItem("para.y",PROPLIST_ITEM(id++,"倒角角度γ","倒角角度γ,单位°"));
	AddPropItem("para.nOffsetType",PROPLIST_ITEM(id++,"偏移类型","位置偏移包括法向偏移即沿钢板Z轴方向的偏移，纵向偏移即沿主管延伸方向(与板法线方向侧为正偏移)的偏移","0.纵向偏移|1.法向偏移"));
	AddPropItem("para.nDHRelativeType",PROPLIST_ITEM(id++,"启用D和H参数"));
	AddPropItem("para.bCreateMirPlate",PROPLIST_ITEM(id++,"生成另一侧封板","生成另一侧封板,封板扇形角为180°时该变量有效","是|否"));
	AddPropItem("para.nCirCoverPlateType",PROPLIST_ITEM(id++,"封板类型","普通封板,扇形角β在Y轴正方的简化封板,扇形角β在Y轴负方的简化封板","普通封板|两块简化封板"));
	//
	AddPropItem("water.W",PROPLIST_ITEM(id++,"外环直径W"));
	AddPropItem("water.S",PROPLIST_ITEM(id++,"位置偏移S"));
	AddPropItem("water.K",PROPLIST_ITEM(id++,"流水槽宽度"));
	AddPropItem("water.H",PROPLIST_ITEM(id++,"流水槽深度"));
	AddPropItem("water.L",PROPLIST_ITEM(id++,"流水槽长度"));
	AddPropItem("water.A",PROPLIST_ITEM(id++,"两流水槽夹角β","两流水槽夹角β,流水槽宽度、深度、长度为0或者两流水槽夹角为0时表示无流水槽"));
	AddPropItem("water.O",PROPLIST_ITEM(id++,"偏移角度","两流水槽角平分线与基准边之间的偏移角度(°)"));
	AddPropItem("water.E",PROPLIST_ITEM(id++,"基准边","流水槽的基准边","0.基准边|1.焊道线"));
	//法线设计参数
	AddPropItem("design_axis_z",PROPLIST_ITEM(id++,"法线设计参数"));
	AddPropItem("design_axis_z.norm_style",PROPLIST_ITEM(id++,"法线计算方式","","0.指定方向|1.单角钢肢法线|2.两杆件交叉面法线|3.构件工作法线|4.构件上的相对方向|5.杆件上的特征方向"));
	AddPropItem("design_axis_z.hVicePart",PROPLIST_ITEM(id++,"基准杆件"));
	AddPropItem("design_axis_z.hCrossPart",PROPLIST_ITEM(id++,"交叉参照杆件"));
	AddPropItem("design_axis_z.norm_wing",PROPLIST_ITEM(id++,"基准肢","","0.X肢法线|1.Y肢法线"));
	AddPropItem("design_axis_z.direction",PROPLIST_ITEM(id++,"朝向","","0.朝外|1.朝里"));
	AddPropItem("design_axis_z.VectorType",PROPLIST_ITEM(id++,"指定方向","",CStdVector::MakeVectorSetStr()));
	AddPropItem("design_axis_z.vector.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("design_axis_z.vector.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("design_axis_z.vector.z",PROPLIST_ITEM(id++,"Z"));
	AddPropItem("design_axis_z.nearVectorType",PROPLIST_ITEM(id++,"近似法向","",CStdVector::MakeVectorSetStr()));
	AddPropItem("design_axis_z.nearVector.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("design_axis_z.nearVector.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("design_axis_z.nearVector.z",PROPLIST_ITEM(id++,"Z"));
	//关联方向
	AddPropItem("ConnDir",PROPLIST_ITEM(id++,"连接方向参数"));
	for(int i=1;i<=10;i++)
		AddPropItem(CXhChar50("ConnDir%d",i),PROPLIST_ITEM(id++,CXhChar50("关联杆件%d",i),"钢板关联杆件用来确定钢板外形","删除"));
	AddPropItem("ConnDir.style",PROPLIST_ITEM(id++,"连接方向类型","","0.焊缝走向|1.连接面法向"));
	AddPropItem("ConnDir.end_space",PROPLIST_ITEM(id++,"端距"));
	AddPropItem("ConnDir.high_space",PROPLIST_ITEM(id++,"端距高"));
	AddPropItem("ConnDir.dir",PROPLIST_ITEM(id++,"方向设计参数"));
	AddPropItem("ConnDir.dir.norm_style",PROPLIST_ITEM(id++,"方向计算方式","","0.指定方向|1.单角钢肢法线|2.两杆件交叉面法线|3.构件工作法线|4.构件上的相对方向|5.杆件上的特征方向"));
	AddPropItem("ConnDir.dir.hVicePart",PROPLIST_ITEM(id++,"射线杆件"));
	AddPropItem("ConnDir.dir.hCrossPart",PROPLIST_ITEM(id++,"交叉参照杆件"));
	AddPropItem("ConnDir.dir.norm_wing",PROPLIST_ITEM(id++,"基准肢","","0.X肢法线|1.Y肢法线"));
	AddPropItem("ConnDir.dir.direction",PROPLIST_ITEM(id++,"方向类型","0.杆件始->终方向,1.杆件终->始方向","0.始->终|1.终->始"));
	AddPropItem("ConnDir.dir.VectorType",PROPLIST_ITEM(id++,"指定方向","",CStdVector::MakeVectorSetStr()));
	AddPropItem("ConnDir.dir.vector.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("ConnDir.dir.vector.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("ConnDir.dir.vector.z",PROPLIST_ITEM(id++,"Z"));
	AddPropItem("ConnDir.dir.nearVectorType",PROPLIST_ITEM(id++,"近似法向","",CStdVector::MakeVectorSetStr()));
	AddPropItem("ConnDir.dir.nearVector.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("ConnDir.dir.nearVector.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("ConnDir.dir.nearVector.z",PROPLIST_ITEM(id++,"Z"));
#endif
}

int CDesignCircularPlateDlg::GetPropValueStr(long id, char *valueStr)
{	//根据属性ID得到属性值
	CXhChar500 sText;
	if(id==GetPropID("sPartNo"))
		sText.Copy(m_sPartNo);
	else if(id==GetPropID("iSeg"))
		sText.Copy(m_sSeg.GetBuffer());
	else if(id==GetPropID("material"))
	{
		for(int i=0;i<CSteelMatLibrary::GetCount();i++)
		{
			STEELMAT mat=CSteelMatLibrary::RecordAt(i);
			if(m_cMaterial==mat.cBriefMark)
			{
				sText.Copy(mat.mark);
				break;
			}
		}
	}
	else if(id==GetPropID("thick"))
		sText.Printf("%d",thick);
	else if(id==GetPropID("type"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_iPlateType==0)
			sText.Copy("0.Common Type");
		else if(m_iPlateType==1)
			sText.Copy("1.Sealing Plate");
		else if(m_iPlateType==2)
			sText.Copy("2.Rib Plate 3WL");
		else if(m_iPlateType==3)
			sText.Copy("3.Rib Plate 3WA");
		else if(m_iPlateType==4)
			sText.Copy("4.Rib Plate 2WL");
		else if(m_iPlateType==5)
			sText.Copy("5.Rib Plate 2WA");
		else if(m_iPlateType==6)
			sText.Copy("6.Shelter Plate ");
#else 
		if(m_iPlateType==0)
		{
			if(ConnVecList.GetCount(TRUE)>0)	//异形法兰
				sText.Copy("异形法兰");
			else
				sText.Copy("0.普通环板");
		}
		else if(m_iPlateType==1)
			sText.Copy("1.封板");
		else if(m_iPlateType==2)
			sText.Copy("2.肋板3WL");
		else if(m_iPlateType==3)
			sText.Copy("3.肋板3WA");
		else if(m_iPlateType==4)
			sText.Copy("4.肋板2WL");
		else if(m_iPlateType==5)
			sText.Copy("5.肋板2WA");
		else if(m_iPlateType==6)
			sText.Copy("6.遮水板");
#endif
	}
	else if(id==GetPropID("ucs.axis_z"))
	{
		int nVecType=CStdVector::GetVectorType(design_axis_z.vector);
		sText.Printf("%s",CStdVector::GetVecTypeString(nVecType));
	}
	else if(id==GetPropID("ucs.axis_z.x"))
		sText.Printf("%f",design_axis_z.vector.x);
	else if(id==GetPropID("ucs.axis_z.y"))
		sText.Printf("%f",design_axis_z.vector.y);
	else if(id==GetPropID("ucs.axis_z.z"))
		sText.Printf("%f",design_axis_z.vector.z);
	//
	else if(GetPropID("hDatumSphere")==id)
	{
		if(m_pDatumSphere)
			sText.Printf("0X%X",m_pDatumSphere->handle);
		else 
			sText.Copy("0X0");
	}
	else if(GetPropID("hDatumTube")==id)
	{
		if(m_pDatumTube)
			sText.Printf("0X%X",m_pDatumTube->handle);
		else 
			sText.Copy("0X0");
	}
	else if(GetPropID("hDatumNode")==id)
	{
		if(m_pDatumNode)
			sText.Printf("0X%X",m_pDatumNode->handle);
		else 
			sText.Copy("0X0");
	}
	else if(GetPropID("hRelaPart1")==id)
	{
		char sVec[50]="";
		if((m_iPlateType==2&&para.nDHRelativeType==0)	//3WL D,H为独立参数 wht 11-05-11
			||(m_iPlateType==3&&para.nDHRelativeType))	//3WA D,H为独立参数
		{
			CONNECT_DIRECTION *pVec=ConnVecList.GetRelaConnInfo(1);
			if(pVec)
			{
				pVec->direct.vector.Set();
				pVec->direct.UpdateVector(console.GetActiveModel());
				if(!pVec->direct.vector.IsZero())
#ifdef AFX_TARG_ENU_ENGLISH
					strcpy(sVec,"(Specified)");
				else 
					strcpy(sVec,"(Unspecified)");
#else 
					strcpy(sVec,"(已指定)");
				else 
					strcpy(sVec,"(未指定)");
#endif
			}
		}
		if(m_pRelaPart1)
			sText.Printf("0X%X%s",m_pRelaPart1->handle,sVec);
		else 
			sText.Printf("0X0%s",sVec);
	}
	else if(GetPropID("hRelaPart2")==id)
	{
		char sVec[50]="";
		if((m_iPlateType==2&&para.nDHRelativeType==0)	//3WL D,H为独立参数 wht 11-05-11
			||(m_iPlateType==3&&para.nDHRelativeType))	//3WA D,H为独立参数
		{
			CONNECT_DIRECTION *pVec=ConnVecList.GetRelaConnInfo(2);
			if(pVec)
			{
				pVec->direct.vector.Set();
				pVec->direct.UpdateVector(console.GetActiveModel());
				if(!pVec->direct.vector.IsZero())
#ifdef AFX_TARG_ENU_ENGLISH
					strcpy(sVec,"(Specified)");
				else 
					strcpy(sVec,"(Unspecified)");
#else 
					strcpy(sVec,"(已指定)");
				else 
					strcpy(sVec,"(未指定)");
#endif
			}
		}
		if(m_pRelaPart2)
			sText.Printf("0X%X%s",m_pRelaPart2->handle,sVec);
		else 
			sText.Printf("0X0%s",sVec);
	}
	else if(GetPropID("RelaPole")==id)
	{
		int nPoleCount=ConnVecList.GetCount(TRUE);
		sText.Printf("%d",nPoleCount);
	}
	else if(GetPropID("RelaBolt")==id)
		sText.Printf("%d",LsRefList.GetNodeNum());
	//
	else if(GetPropID("tube_r")==id)//alfa 定位角
	{
		if(m_pDatumTube)
			sText.Printf("%f",m_pDatumTube->GetDiameter()*0.5);
		else 
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("No Datum Tube");
#else 
			sText.Printf("无基准钢管");
#endif
	}
	else if(GetPropID("para.A")==id)//alfa 定位角
		sText.Printf("%f",para.A);
	else if(GetPropID("para.B")==id)//beta 扇形角	
		sText.Printf("%f",para.B);
	else if(GetPropID("para.y")==id)//2WA型环向肋板倒角角度,单位°
		sText.Printf("%f",para.gama);
	else if(GetPropID("para.D")==id)//自由边距
		sText.Printf("%f",para.D);
	else if(GetPropID("para.H")==id)//台口高度
		sText.Printf("%f",para.H);
	else if(GetPropID("para.bMode")==id)//模式
	{
		if(para.bWeldSemicircleMode)
			sText.Printf("是");
		else
			sText.Printf("否");
	}
	else if (GetPropID("para.R1")==id) //流水孔1半径
		sText.Printf("%f",para.R1);
	else if (GetPropID("para.R2")==id) //流水孔2半径
		sText.Printf("%f",para.R2);
	else if(GetPropID("para.h")==id)//N侧台口高度
		sText.Printf("%f",para.h);
	else if(GetPropID("para.R")==id)//外圆半径
	{
		sText.Printf("%f",para.R);
		if(m_iPlateType==3&&para.nDHRelativeType==0&&para.R<EPS
			&&fabs(para.W-para.N)<EPS&&fabs(para.W-para.D)<EPS
			&&para.W>0&&fabs(para.H)<EPS&&m_pDatumTube&&m_pCirPlate)
		{	//3WA 以前版本顶部圆弧连接的肋板W==N==D H=0,当前版本仅保存R 
			//根据D计算外圆半径R wht 11-05-24
			sText.Printf("%f",para.D+para.V+m_pDatumTube->GetDiameter()*0.5);
		}
	}
	else if(GetPropID("innercir_type")==id)
	{	//环向板内圆类型 wht 11-05-27
		if(fabs(para.r)<EPS)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("0.No Inner Circle");
		else if(para.r>0)
			sText.Printf("1.Specify Inner Circle R");
		else //if(para.r<0)
			sText.Printf("2.Create By Tube");
#else 
			sText.Printf("0.无内圆");
		else if(para.r>0)
			sText.Printf("1.指定内圆半径");
		else //if(para.r<0)
			sText.Printf("2.根据钢管生成");
#endif
	}
	else if(GetPropID("para.r")==id)//内圆半径
		sText.Printf("%f",para.r);
	else if(GetPropID("para.S")==id)//上侧封板
	{
		if(m_iPlateType==1)
		{	//环向封板
#ifdef AFX_TARG_ENU_ENGLISH
			if(para.S>0)
				sText.Copy("Up Sealing Plate");
			else if(para.S<0)
				sText.Copy("Down Sealing Plate");
			else 
				sText.Copy("Up&Down Sealing Plate");
#else 
			if(para.S>0)
				sText.Copy("上封板");
			else if(para.S<0)
				sText.Copy("下封板");
			else 
				sText.Copy("上下封板");
#endif
		}
		else 
			sText.Printf("%f",para.S);
	}
	else if(GetPropID("para.nCirCoverPlateType")==id)
	{	//封板类型
#ifdef AFX_TARG_ENU_ENGLISH
		if(para.nCirCoverPlateType==2)
			sText.Copy("Two Simple Sealing Plate");
		else if(para.nCirCoverPlateType>0)
			sText.Copy("Simple Sealing Plate(Y+)");
		else if(para.nCirCoverPlateType<0)
			sText.Copy("Simple Sealing Plate(Y-)");
		else 
			sText.Copy("Common Sealing Plate");
#else 
		if(para.nCirCoverPlateType==2)
			sText.Copy("两块简化封板");
		else if(para.nCirCoverPlateType>0)
			sText.Copy("简化封板(Y+)");
		else if(para.nCirCoverPlateType<0)
			sText.Copy("简化封板(Y-)");
		else 
			sText.Copy("普通封板");
#endif
	}
	else if(GetPropID("para.V")==id)//加工间隙
		sText.Printf("%f",para.V);
	//
	else if(GetPropID("para.W")==id)		//第一块关联板支撑边宽/环向带宽
		sText.Printf("%f",para.W);
	else if(GetPropID("para.N")==id)		//第二块关联板支撑边宽
		sText.Printf("%f",para.N);
	else if(GetPropID("para.Y")==id)		//环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(para.Y>0)
			sText.Printf("Datum Plate Outside");
		else if(para.Y<0)
			sText.Printf("Datum Plate Inside");
		else 
			sText.Printf("Default");
#else 
		if(para.Y>0)
			sText.Printf("基板外侧");
		else if(para.Y<0)
			sText.Printf("基板内侧");
		else 
			sText.Printf("自动判断");
#endif
	}
	else if(GetPropID("para.nOffsetType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(para.nOffsetType==1)
			sText.Printf("1.Normal Offset");
		else 
			sText.Printf("0.Vertical Offset");
#else 
		if(para.nOffsetType==1)
			sText.Printf("1.法向偏移");
		else 
			sText.Printf("0.纵向偏移");
#endif
	}
	else if(GetPropID("para.nDHRelativeType")==id)//0.独立参数 1关联且启用D 2.关联且启用H
	{
		if(m_iPlateType==2)
		{
			//if(ConnVecList.IsNeedBend()) //此处暂时不需要该判断 wht 11-05-12
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(para.nDHRelativeType==1)
					sText.Printf("1.Relative&Enable D");
				else if(para.nDHRelativeType==2)
					sText.Printf("2.Relative&Enable H");
				else //if(para.nDHRelativeType==0)
					sText.Printf("0.Independent Parameter");
#else 
				if(para.nDHRelativeType==1)
					sText.Printf("1.关联且启用D");
				else if(para.nDHRelativeType==2)
					sText.Printf("2.关联且启用H");
				else //if(para.nDHRelativeType==0)
					sText.Printf("0.独立参数");
#endif
			}
			/*else 
			{	
				if(para.nDHRelativeType==1)
					sText.Printf("1.关联且启用D");
				else //if(para.nDHRelativeType==2)
					sText.Printf("2.关联且启用H");
			}*/
		}
		else if(m_iPlateType==3)
		{
			if(para.nDHRelativeType==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("No");
			else 
				sText.Copy("Yes");
#else 
				sText.Copy("否");
			else 
				sText.Copy("是");
#endif
		}
	}
	else if(GetPropID("para.bCreateMirPlate")==id)
	{	//生成另一侧钢板
		if(para.bCreateMirPlate)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("Yes");
		else 
			sText.Copy("No");
#else 
			sText.Copy("是");
		else 
			sText.Copy("否");
#endif
	}
	//
	else if(GetPropID("water.W")==id)	//外环直径
		sText.Printf("%f",water.W);
	else if(GetPropID("water.S")==id)	//法向偏移距离
		sText.Printf("%f",water.S);
	else if(GetPropID("water.K")==id)	//流水槽宽度
		sText.Printf("%f",water.K);
	else if(GetPropID("water.H")==id)	//流水槽深度
		sText.Printf("%f",water.H);
	else if(GetPropID("water.L")==id)	//流水槽长度
		sText.Printf("%f",water.L);
	else if(GetPropID("water.A")==id)	//两流水槽之间的夹角
		sText.Printf("%f",water.A);
	else if(GetPropID("water.O")==id)	//两流水槽角平分线与基准边之间的偏移角
		sText.Printf("%f",water.O);
	else if(GetPropID("water.E")==id)	//定位角基准边:0.基准边1.焊道线
	{
		if(water.E==1)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Weld Line");
		else 
			sText.Copy("0.Datum Edge");
#else 
			sText.Copy("1.焊道线");
		else 
			sText.Copy("0.基准边");
#endif
	}
	//环向布置螺栓参数
	else if(GetPropID("CreateBolt")==id
		||GetPropID("m_xLayoutBolt.m_nBoltNum")==id)
		sText.Printf("%d",m_xLayoutBolt.m_nBoltNum);
	else if(GetPropID("m_xLayoutBolt.m_nBoltD")==id)
		sText.Printf("%d",m_xLayoutBolt.m_nBoltD);
	else if(GetPropID("m_xLayoutBolt.m_iDriection")==id)
	{
		if(m_xLayoutBolt.m_iDirection==1)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Inward");
		else
			sText.Copy("0.Outward");
#else 
			sText.Copy("1.朝内");
		else
			sText.Copy("0.朝外");
#endif
	}
	else if(GetPropID("m_xLayoutBolt.m_iBoltSegI")==id)
		sText.Printf("%d",m_xLayoutBolt.m_iBoltSegI);
	else if(GetPropID("m_xLayoutBolt.m_iStartEdge")==id)
	{
		if(m_xLayoutBolt.m_iStartEdge==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Weld Line");
		else
			sText.Copy("0.Datum Edge");
#else 
			sText.Copy("1.焊道线");
		else
			sText.Copy("0.基准边");
#endif
	}
	else if(GetPropID("m_xLayoutBolt.m_fOffsetAngle")==id)
	{
		sText.Printf("%f",m_xLayoutBolt.m_fOffsetAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLayoutBolt.m_fBoltLayoutR")==id)	
	{
		sText.Printf("%f",m_xLayoutBolt.m_fBoltLayoutR);
		SimplifiedNumString(sText);
	}
	//法线设计参数
	else if(GetPropID("design_axis_z.norm_style")==id)
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		if(design_axis_z.norm_style==0)
			sText.Copy("0.Specify Direction");
		else if(design_axis_z.norm_style==1)
			sText.Copy("1.Single Angle Leg Normal");
		else if(design_axis_z.norm_style==2)
			sText.Copy("2.Two Rod Cross-bracing Normal");
		else if(design_axis_z.norm_style==3)
			sText.Copy("3.Part Work Normal");
		else if(design_axis_z.norm_style==4)
			sText.Copy("4.Part Relative Direction");
		else if(design_axis_z.norm_style==5)
			sText.Copy("5.Rod Characteristic Direction");
#else 
		if(design_axis_z.norm_style==0)
			sText.Copy("0.指定方向");
		else if(design_axis_z.norm_style==1)
			sText.Copy("1.角钢肢法线");
		else if(design_axis_z.norm_style==2)
			sText.Copy("2.杆件交叉面法线");
		else if(design_axis_z.norm_style==3)
			sText.Copy("3.构件工作法线");
		else if(design_axis_z.norm_style==4)
			sText.Copy("4.构件上的相对方向");
		else if(design_axis_z.norm_style==5)
			sText.Copy("5.杆件上的特征方向");
#endif
	}
	else if(GetPropID("design_axis_z.hVicePart")==id)
		sText.Printf("0X%X",design_axis_z.hVicePart);
	else if(GetPropID("design_axis_z.hCrossPart")==id)
		sText.Printf("0X%X",design_axis_z.hCrossPart);
	else if(GetPropID("design_axis_z.norm_wing")==id)
	{
		if(design_axis_z.norm_wing==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","0.X Leg Normal");
		else
			sText.Printf("%s","1.Y Leg Normal");
#else 
			sText.Printf("%s","0.X肢法线");
		else
			sText.Printf("%s","1.Y肢法线");
#endif
	}	
	else if(GetPropID("design_axis_z.direction")==id)
	{
		if(design_axis_z.norm_style==1)
		{
			if(design_axis_z.direction==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","0.Outward");
			else
				sText.Printf("%s","1.Inward");
#else 
				sText.Printf("%s","0.朝外");
			else
				sText.Printf("%s","1.朝里");
#endif
		}
		else if(design_axis_z.norm_style==5)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(design_axis_z.direction==0)
				sText.Copy("0.Start->End");
			else if(design_axis_z.direction==1)
				sText.Copy("1.End->Start");
			else if(design_axis_z.direction==2)
				sText.Copy("2.X Leg Normal+");
			else if(design_axis_z.direction==3)
				sText.Copy("3.X Leg Normal-");
			else if(design_axis_z.direction==4)
				sText.Copy("4.Y Leg Normal+");
			else 
				sText.Copy("5.Y Leg Normal-");
#else 
			if(design_axis_z.direction==0)
				sText.Copy("0.始->终");
			else if(design_axis_z.direction==1)
				sText.Copy("1.终->始");
			else if(design_axis_z.direction==2)
				sText.Copy("2.X肢法线+");
			else if(design_axis_z.direction==3)
				sText.Copy("3.X肢法线-");
			else if(design_axis_z.direction==4)
				sText.Copy("4.Y肢法线+");
			else 
				sText.Copy("5.Y肢法线-");
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(design_axis_z.direction==0)
				sText.Printf("%s","0.Front Direction");
			else
				sText.Printf("%s","1.Reverse Direction");
#else 
			if(design_axis_z.direction==0)
				sText.Printf("%s","0.正向");
			else
				sText.Printf("%s","1.反向");
#endif
		}
	}
	else if(GetPropID("design_axis_z.VectorType")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(design_axis_z.vector));
	else if(GetPropID("design_axis_z.vector.x")==id)
		sText.Printf("%lf",design_axis_z.vector.x);
	else if(GetPropID("design_axis_z.vector.y")==id)
		sText.Printf("%lf",design_axis_z.vector.y);
	else if(GetPropID("design_axis_z.vector.z")==id)
		sText.Printf("%lf",design_axis_z.vector.z);
	else if(GetPropID("design_axis_z.nearVectorType")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(design_axis_z.nearVector));
	else if(GetPropID("design_axis_z.nearVector.x")==id)
		sText.Printf("%lf",design_axis_z.nearVector.x);
	else if(GetPropID("design_axis_z.nearVector.y")==id)
		sText.Printf("%lf",design_axis_z.nearVector.y);
	else if(GetPropID("design_axis_z.nearVector.z")==id)
		sText.Printf("%lf",design_axis_z.nearVector.z);
	//
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.style")==id)
	{
		if(m_pCurSelConnVec->style==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("0.Weld Seam Direction");
		else 
			sText.Printf("0.Connect Plane Normal");
#else 
			sText.Printf("0.焊缝走向");
		else 
			sText.Printf("0.连接面法向");
#endif
	}
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.end_space")==id)
		sText.Printf("%d",m_pCurSelConnVec->end_space);
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.high_space")==id) 
		sText.Printf("%d",m_pCurSelConnVec->high_space);
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.dir.norm_style")==id)
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_pCurSelConnVec->direct.norm_style==0)
			sText.Copy("0.Specify Direction");
		else if(m_pCurSelConnVec->direct.norm_style==1)
			sText.Copy("1.Single Angle Leg Normal");
		else if(m_pCurSelConnVec->direct.norm_style==2)
			sText.Copy("2.Two Rod Cross-bracing Normal");
		else if(m_pCurSelConnVec->direct.norm_style==3)
			sText.Copy("3.Part Work Normal");
		else if(m_pCurSelConnVec->direct.norm_style==4)
			sText.Copy("4.Part Relative Direction");
		else if(m_pCurSelConnVec->direct.norm_style==5)
			sText.Copy("5.Rod Characteristic Direction");
#else 
		if(m_pCurSelConnVec->direct.norm_style==0)
			sText.Copy("0.指定方向");
		else if(m_pCurSelConnVec->direct.norm_style==1)
			sText.Copy("1.角钢肢法线");
		else if(m_pCurSelConnVec->direct.norm_style==2)
			sText.Copy("2.杆件交叉面法线");
		else if(m_pCurSelConnVec->direct.norm_style==3)
			sText.Copy("3.构件工作法线");
		else if(m_pCurSelConnVec->direct.norm_style==4)
			sText.Copy("4.构件上的相对方向");
		else if(m_pCurSelConnVec->direct.norm_style==5)
			sText.Copy("5.杆件上的特征方向");
#endif
	}
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.dir.hVicePart")==id)
		sText.Printf("0X%X",m_pCurSelConnVec->direct.hVicePart);
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.dir.hCrossPart")==id)
		sText.Printf("0X%X",m_pCurSelConnVec->direct.hCrossPart);
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.dir.norm_wing")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_pCurSelConnVec->direct.norm_wing==0)
			sText.Printf("%s","0.X Leg Normal");
		else
			sText.Printf("%s","1.Y Leg Normal");
#else 
		if(m_pCurSelConnVec->direct.norm_wing==0)
			sText.Printf("%s","0.X肢法线");
		else
			sText.Printf("%s","1.Y肢法线");
#endif
	}	
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.dir.direction")==id)
	{
		if(m_pCurSelConnVec->serial>=10)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_pCurSelConnVec->direct.direction==0)
				sText.Copy("0.Start->End");
			else if(m_pCurSelConnVec->direct.direction==1)
				sText.Copy("1.End->Start");
#else 
			if(m_pCurSelConnVec->direct.direction==0)
				sText.Copy("0.始->终");
			else if(m_pCurSelConnVec->direct.direction==1)
				sText.Copy("1.终->始");
#endif
		}
		else if(m_pCurSelConnVec->direct.norm_style==1)
		{
			if(m_pCurSelConnVec->direct.direction==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","0.Outward");
			else
				sText.Printf("%s","1.Inward");
#else 
				sText.Printf("%s","0.朝外");
			else
				sText.Printf("%s","1.朝里");
#endif
		}
		else if(m_pCurSelConnVec->direct.norm_style==5)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_pCurSelConnVec->direct.direction==0)
				sText.Copy("0.Start->End");
			else if(m_pCurSelConnVec->direct.direction==1)
				sText.Copy("1.End->Start");
			else if(m_pCurSelConnVec->direct.direction==2)
				sText.Copy("2.X Leg Normal+");
			else if(m_pCurSelConnVec->direct.direction==3)
				sText.Copy("3.X Leg Normal-");
			else if(m_pCurSelConnVec->direct.direction==4)
				sText.Copy("4.Y Leg Normal+");
			else 
				sText.Copy("5.Y Leg Normal-");
#else 
			if(m_pCurSelConnVec->direct.direction==0)
				sText.Copy("0.始->终");
			else if(m_pCurSelConnVec->direct.direction==1)
				sText.Copy("1.终->始");
			else if(m_pCurSelConnVec->direct.direction==2)
				sText.Copy("2.X肢法线+");
			else if(m_pCurSelConnVec->direct.direction==3)
				sText.Copy("3.X肢法线-");
			else if(m_pCurSelConnVec->direct.direction==4)
				sText.Copy("4.Y肢法线+");
			else 
				sText.Copy("5.Y肢法线-");
#endif
		}
		else
		{
			if(m_pCurSelConnVec->direct.direction==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","0.Front Direction");
			else
				sText.Printf("%s","1.Reverse Direction");
#else 
				sText.Printf("%s","0.正向");
			else
				sText.Printf("%s","1.反向");
#endif
		}
	}
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.dir.VectorType")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(m_pCurSelConnVec->direct.vector));
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.dir.vector.x")==id)
		sText.Printf("%lf",m_pCurSelConnVec->direct.vector.x);
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.dir.vector.y")==id)
		sText.Printf("%lf",m_pCurSelConnVec->direct.vector.y);
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.dir.vector.z")==id)
		sText.Printf("%lf",m_pCurSelConnVec->direct.vector.z);
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.dir.nearVectorType")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(m_pCurSelConnVec->direct.nearVector));
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.dir.nearVector.x")==id)
		sText.Printf("%lf",m_pCurSelConnVec->direct.nearVector.x);
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.dir.nearVector.y")==id)
		sText.Printf("%lf",m_pCurSelConnVec->direct.nearVector.y);
	else if(m_pCurSelConnVec&&GetPropID("ConnDir.dir.nearVector.z")==id)
		sText.Printf("%lf",m_pCurSelConnVec->direct.nearVector.z);
	else 
	{
		for(int i=0;i<10;i++)
		{
			CXhChar50 sKey("ConnDir%d",i);
			if(GetPropID(sKey)==id)
			{
				CONNECT_DIRECTION *pVec=ConnVecList.GetRelaConnVecByFeature(id);
				if(pVec)
					sText.Printf("0X%X",pVec->direct.hVicePart);
				else 
					sText.Printf("0X0");
			}
		}
	}
	if(valueStr)
	{
		//SimplifiedNumString(sText);
		//strcpy(valueStr,sText);
		CString ss=sText;
		SimplifiedNumString(ss);
		strcpy(valueStr,ss);
	}
	return strlen(sText);
}

//显示钢板外形参数
void CDesignCircularPlateDlg::UpdatePlateProfileParamProp(CPropTreeItem *pProfileItem)
{	
	if(pProfileItem==NULL)
		return;
	m_xSubPropList.DeleteAllSonItems(pProfileItem);
	//
	CPropTreeItem *pPropItem=NULL,*pItem=NULL;
	CPropertyListOper<CDesignCircularPlateDlg> oper(&m_xSubPropList,this);
	//基准钢管半径
	pPropItem=oper.InsertEditPropItem(pProfileItem,"tube_r","","",-1,TRUE);
	pPropItem->SetReadOnly(TRUE);
	if(m_iPlateType==0)
	{	//0.普通环向板
		BOOL bNeedDisplayInnerR=FALSE;
		if(ConnVecList.GetCount(TRUE)>0)
		{
			pPropItem=oper.InsertEditPropItem(pProfileItem,"para.D","","",-1,TRUE);
			UpdatePropItemName(&m_xSubPropList,pPropItem);
			bNeedDisplayInnerR=TRUE;	//需要显示内圆半径
		}
		else 
		{
			//扇形角β
			oper.InsertEditPropItem(pProfileItem,"para.B","","",-1,TRUE);
			//外圆半径
			oper.InsertEditPropItem(pProfileItem,"para.R","","",-1,TRUE);
			if(fabs(para.B-360)<EPS)//整圆
				bNeedDisplayInnerR=TRUE;	//需要显示内圆半径
		}
		//if(bNeedDisplayInnerR)
		//{	//内圆类型
		//	pPropItem=oper.InsertCmbListPropItem(pProfileItem,"innercir_type","","","",-1,TRUE);
		//	if(para.r>0)
		//		oper.InsertEditPropItem(pPropItem,"para.r","","",-1,TRUE);
		//}
	}
	else if(m_iPlateType==1)
	{	//环向封板
		//扇形角
		oper.InsertEditPropItem(pProfileItem,"para.B","","",-1,TRUE);
		//环形带宽
		oper.InsertEditPropItem(pProfileItem,"para.W","","",-1,TRUE);
		//台口高度
		oper.InsertEditPropItem(pProfileItem,"para.H","","",-1,TRUE);
		pPropItem=oper.InsertCmbListPropItem(pProfileItem,"para.bMode","","","",-1,TRUE);
		if(para.bWeldSemicircleMode)
		{
			//流水孔1半径
			oper.InsertEditPropItem(pPropItem,"para.R1","","",-1,TRUE);
			//流水孔2半径
			oper.InsertEditPropItem(pPropItem,"para.R2","","",-1,TRUE);
		}
		if(para.nCirCoverPlateType!=0)
		{	//参数N
			oper.InsertEditPropItem(pProfileItem,"para.N","","",-1,TRUE);
		}
	}
	else if(m_iPlateType==2||m_iPlateType==3)
	{
		pPropItem=oper.InsertCmbListPropItem(pProfileItem,"para.nDHRelativeType","","","",-1,TRUE);
		UpdatePropItemName(&m_xSubPropList,pPropItem);
		if(m_iPlateType==2)
		{	//直线型三边支撑环向肋板
			if(para.nDHRelativeType==0||para.nDHRelativeType==1)
			{	//自由边距
				pPropItem=oper.InsertEditPropItem(pProfileItem,"para.D","","",-1,TRUE);
				UpdatePropItemName(&m_xSubPropList,pPropItem);
			}
			if(para.nDHRelativeType==0||para.nDHRelativeType==2)
			{	//台口高度
				oper.InsertEditPropItem(pProfileItem,"para.H","","",-1,TRUE);
			}
			//
			pPropItem=oper.InsertEditPropItem(pProfileItem,"para.W","","",-1,TRUE);
			UpdatePropItemName(&m_xSubPropList,pPropItem);
			//
			oper.InsertEditPropItem(pProfileItem,"para.N","","",-1,TRUE);
		}
		else if(m_iPlateType==3)
		{	//圆弧型三边支撑环向肋板
			if(para.nDHRelativeType!=0)
			{	
				//环形带宽D
				pPropItem=oper.InsertEditPropItem(pProfileItem,"para.D","","",-1,TRUE);
				UpdatePropItemName(&m_xSubPropList,pPropItem);
				//台口高度
				oper.InsertEditPropItem(pProfileItem,"para.H","","",-1,TRUE);
				//N侧台口高度
				oper.InsertEditPropItem(pProfileItem,"para.h","","",-1,TRUE);
				//参数W
				pPropItem=oper.InsertEditPropItem(pProfileItem,"para.W","","",-1,TRUE);
				UpdatePropItemName(&m_xSubPropList,pPropItem);
				//参数N
				oper.InsertEditPropItem(pProfileItem,"para.N","","",-1,TRUE);
			}
			else //外圆半径Ro
				oper.InsertEditPropItem(pProfileItem,"para.R","","",-1,TRUE);
		}
	}
	else if(m_iPlateType==4)
	{	//直线型两边支撑环向肋板
		pPropItem=oper.InsertEditPropItem(pProfileItem,"para.W","","",-1,TRUE);
		UpdatePropItemName(&m_xSubPropList,pPropItem);
		//扇形角β
		pPropItem=oper.InsertEditPropItem(pProfileItem,"para.B","","",-1,TRUE);
		if(para.D>0)
			pPropItem->SetReadOnly();
		//自由边距
		pPropItem=oper.InsertEditPropItem(pProfileItem,"para.D","","",-1,TRUE);
		if(para.D==0&&para.B>0)
			pPropItem->SetReadOnly();
		//台口高度
		oper.InsertEditPropItem(pProfileItem,"para.H","","",-1,TRUE);
	}
	else if(m_iPlateType==5)
	{	//圆弧型两边支撑环向肋板
		pPropItem=oper.InsertEditPropItem(pProfileItem,"para.W","","",-1,TRUE);
		UpdatePropItemName(&m_xSubPropList,pPropItem);
		//扇形角β
		oper.InsertEditPropItem(pProfileItem,"para.B","","",-1,TRUE);
		//倒角角度γ
		oper.InsertEditPropItem(pProfileItem,"para.y","","",-1,TRUE);
		//台口高度H
		oper.InsertEditPropItem(pProfileItem,"para.H","","",-1,TRUE);
	}
	else if(m_iPlateType==6)
	{	//遮水板
		//外环直径
		oper.InsertEditPropItem(pProfileItem,"water.W","","",-1,TRUE);
		if(m_pDatumTube && m_pDatumTube->m_bHasWeldRoad)
		{	//定位角基准边:0.基准边1.焊道线
			oper.InsertCmbListPropItem(pProfileItem,"water.E","","","",-1,TRUE);
		}
		//两流水槽角平分线与基准边之间的偏移角
		oper.InsertEditPropItem(pProfileItem,"water.O","","",-1,TRUE);
		//两流水槽之间的夹角
		oper.InsertEditPropItem(pProfileItem,"water.A","","",-1,TRUE);
		//流水槽长度
		oper.InsertEditPropItem(pProfileItem,"water.L","","",-1,TRUE);
		//流水槽宽度
		oper.InsertEditPropItem(pProfileItem,"water.K","","",-1,TRUE);
		//流水槽深度
		oper.InsertEditPropItem(pProfileItem,"water.H","","",-1,TRUE);
	}
	if(m_iPlateType>1&&m_iPlateType<6)
	{

	}
	if(m_iPlateType<6)
	{	//加工间隙
		oper.InsertEditPropItem(pProfileItem,"para.V","","",-1,TRUE);
	}
}

//更新环向板参数属性栏显示
void CDesignCircularPlateDlg::UpdateCirPlateParamProp(CPropTreeItem *pCurItem/*=NULL*/)
{
	CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL;
	CPropertyListOper<CDesignCircularPlateDlg> oper(&m_propList,this);
	//基准参数
	CPropTreeItem *pBasicItem=m_propList.FindItemByPropId(CDesignCircularPlateDlg::GetPropID("basicInfo"),NULL);	
	m_propList.DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.S"));
	m_propList.DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.nCirCoverPlateType"));
	m_propList.DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.bCreateMirPlate"));
	if(m_iPlateType==1)	//环向封板
	{	//封板位置
		pPropItem=oper.InsertCmbListPropItem(pBasicItem,"para.S","","","",-1,TRUE);
		UpdatePropItemName(&m_propList,pPropItem);
		pPropItem->SetReadOnly(!m_bNewCirPlate);
		//环向封板类型
		pPropItem=oper.InsertCmbListPropItem(pBasicItem,"para.nCirCoverPlateType","","","",-1,TRUE);
		UpdatePropItemName(&m_propList,pPropItem);
		pPropItem->SetReadOnly(!m_bNewCirPlate);
		//
		if(fabs(para.B-180)<EPS&&para.nCirCoverPlateType==0&&m_bNewCirPlate)
			oper.InsertCmbListPropItem(pBasicItem,"para.bCreateMirPlate","","","",-1,TRUE);
	}
	//定位参数
	CPropTreeItem *pLocationItem=m_propList.FindItemByPropId(CDesignCircularPlateDlg::GetPropID("location"),NULL);
	m_propList.DeleteAllSonItems(pLocationItem);	
	if(m_iPlateType!=1)
	{	//环向封板不需要输入法线方向
		//工作法线
		pPropItem=oper.InsertCmbListPropItem(pLocationItem,"ucs.axis_z","","","",-1,TRUE);
		pPropItem->SetReadOnly(!m_bNewCirPlate);	//查看参数时暂不支持修改法线方向 wht 11-05-13
		if(m_iPlateType!=6)
		{	//偏移类型
			oper.InsertCmbListPropItem(pLocationItem,"para.nOffsetType","","","",-1,TRUE);
			//位置偏移
			oper.InsertEditPropItem(pLocationItem,"para.S","","",-1,TRUE);
		}
		else	//遮水板
			oper.InsertEditPropItem(pLocationItem,"water.S","","",-1,TRUE);	
	}
	//0.普通环向板 1.环向封板
	if((m_iPlateType==0 && ConnVecList.GetCount(TRUE)<=0)||
	   (m_iPlateType==1 && para.nCirCoverPlateType==0))
	{
		pPropItem=oper.InsertEditPropItem(pLocationItem,"para.A","","",-1,TRUE);	//扇形起始角
		UpdatePropItemName(&m_propList,pPropItem);
	}
	//2.环向肋板3WL,3.环向肋板3WA,4.环向肋板2WL,5.环向肋板2WA
	else if(m_iPlateType>1&&m_iPlateType<=5)
		oper.InsertCmbListPropItem(pLocationItem,"para.Y","","","",-1,TRUE);	//肋板位置 (>0基准板外侧;=0自动;<0基准板内侧)
	//关联信息
	CPropTreeItem *pRelaInfoItem=m_propList.FindItemByPropId(CDesignCircularPlateDlg::GetPropID("RelaInfo"),NULL);
	m_propList.DeleteAllSonItems(pRelaInfoItem);
	//基准钢管
	oper.InsertButtonPropItem(pRelaInfoItem,"hDatumTube","","",-1,TRUE);
	//基准节点
	oper.InsertButtonPropItem(pRelaInfoItem,"hDatumNode","","",-1,TRUE);
	if(m_iPlateType!=0)
	{	//关联基板
		oper.InsertButtonPropItem(pRelaInfoItem,"hRelaPart1","","",-1,TRUE);
	}
	if(m_iPlateType==1||m_iPlateType==2||m_iPlateType==3)
	{	//3W型肋板或封板需要关联钢板2
		oper.InsertButtonPropItem(pRelaInfoItem,"hRelaPart2","","",-1,TRUE);	
	}
	if(m_iPlateType==0&&m_bNewCirPlate)
	{	//生成螺栓 wht 05-16
		pPropItem=oper.InsertEditPropItem(pRelaInfoItem,"CreateBolt","","",-1,TRUE);
		pPropItem->SetReadOnly(TRUE);
	}
	//环向封板,环向肋板和遮水板不需要选择关联杆件,遮水板的关联螺栓会自动引入不用选择 wht 11-05-11
	if((m_iPlateType==0&&ConnVecList.GetCount()>0)	//异形法兰
		||(m_iPlateType==2&&para.nDHRelativeType==0)	//3WL 跨越连板
		||(m_iPlateType==3&&para.nDHRelativeType))	//3WA 跨越连板
	{	//关联螺栓
		oper.InsertButtonPropItem(pRelaInfoItem,"RelaBolt","","",-1,TRUE);
		//关联杆件
		pPropItem=oper.InsertButtonPropItem(pRelaInfoItem,"RelaPole","","",-1,TRUE);
		//关联构件
		int iIndex=1;
		for(CONNECT_DIRECTION *pVec=ConnVecList.GetFirst(TRUE);pVec;pVec=ConnVecList.GetNext(TRUE),iIndex++)
		{
			long idProp=CDesignCircularPlateDlg::GetPropID(CXhChar50("ConnDir%d",iIndex));
			pVec->feature=idProp; //连接方向中记录关联属性项id
			oper.InsertButtonPropItem(pPropItem,CXhChar50("ConnDir%d",iIndex),"","",-1,TRUE);
		}
	}
}

void CDesignCircularPlateDlg::DisplayPlatePropList()
{
	m_propList.CleanTree();
	m_propList.m_nObjClassTypeId = 0;
	//
	CPropTreeItem *pGroupItem,*pPropItem,*pRoot=m_propList.GetRootItem();
	CPropertyListOper<CDesignCircularPlateDlg> oper(&m_propList,this);
	//基本信息
	pGroupItem=oper.InsertPropItem(pRoot,"basicInfo");
	//构件编号
	oper.InsertEditPropItem(pGroupItem,"sPartNo");
	//段号
	oper.InsertEditPropItem(pGroupItem,"iSeg");
	//材质
	oper.InsertCmbListPropItem(pGroupItem,"material");
	//厚度
	oper.InsertEditPropItem(pGroupItem,"thick");
	//环板类型
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"type");
	pPropItem->SetReadOnly(!m_bNewCirPlate||m_bReadOnlyPlateType);	
	//关联信息
	oper.InsertPropItem(pRoot,"RelaInfo");
	//装配方位
	oper.InsertPropItem(pRoot,"location");
	//
	UpdateCirPlateParamProp();
	m_propList.Redraw();
}

//根据前缀字符串更新方向属性表
void CDesignCircularPlateDlg::UpdateVectorPropList(CPropTreeItem *pParentItem,char* sPrefix,CLDSVector xVector)
{
	m_xSubPropList.DeleteAllSonItems(pParentItem);
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pSonItem=NULL;
	//
	CPropertyListOper<CDesignCircularPlateDlg> oper(&m_xSubPropList,this);
	//指定法线
	if(xVector.norm_style==0)
	{
		pPropItem=oper.InsertCmbListPropItem(pParentItem,CXhChar50("%s.VectorType",sPrefix),"","","",-1,TRUE);
		int nearVectorType=CStdVector::GetVectorType(xVector.vector);
		pSonItem=oper.InsertEditPropItem(pPropItem,CXhChar50("%s.vector.x",sPrefix),"","",-1,TRUE);
		pSonItem->SetReadOnly(nearVectorType!=0);
		pSonItem=oper.InsertEditPropItem(pPropItem,CXhChar50("%s.vector.y",sPrefix),"","",-1,TRUE);
		pSonItem->SetReadOnly(nearVectorType!=0);
		pSonItem=oper.InsertEditPropItem(pPropItem,CXhChar50("%s.vector.z",sPrefix),"","",-1,TRUE);
		pSonItem->SetReadOnly(nearVectorType!=0);
	}
	//单角钢肢法线
	else if(xVector.norm_style==1)
	{
		pPropItem=oper.InsertButtonPropItem(pParentItem,CXhChar50("%s.hVicePart",sPrefix),"","",-1,TRUE);
		UpdatePropItemName(&m_xSubPropList,pPropItem,xVector.norm_style);
		//
		oper.InsertCmbListPropItem(pParentItem,CXhChar50("%s.norm_wing",sPrefix),"","","",-1,TRUE);
		//
		pPropItem=oper.InsertCmbListPropItem(pParentItem,CXhChar50("%s.direction",sPrefix),"","","",-1,TRUE);
		UpdatePropItemName(&m_xSubPropList,pPropItem,xVector.norm_style);
	}
	else if(xVector.norm_style==2)
	{
		pPropItem=oper.InsertButtonPropItem(pParentItem,CXhChar50("%s.hVicePart",sPrefix),"","",-1,TRUE);
		UpdatePropItemName(&m_xSubPropList,pPropItem,xVector.norm_style);
		//
		oper.InsertButtonPropItem(pParentItem,CXhChar50("%s.hCrossPart",sPrefix),"","",-1,TRUE);
		//
		pPropItem=oper.InsertCmbListPropItem(pParentItem,CXhChar50("%s.nearVectorType",sPrefix),"","","",-1,TRUE);
		UpdatePropItemName(&m_xSubPropList,pPropItem,xVector.norm_style);
		int nearVectorType=CStdVector::GetVectorType(xVector.nearVector);
		
		pSonItem=oper.InsertEditPropItem(pPropItem,CXhChar50("%s.nearVector.x",sPrefix),"","",-1,TRUE);
		pSonItem->SetReadOnly(nearVectorType!=0);
		
		pSonItem=oper.InsertEditPropItem(pPropItem,CXhChar50("%s.nearVector.y",sPrefix),"","",-1,TRUE);
		pSonItem->SetReadOnly(nearVectorType!=0);
		
		pSonItem=oper.InsertEditPropItem(pPropItem,CXhChar50("%s.nearVector.z",sPrefix),"","",-1,TRUE);
		pSonItem->SetReadOnly(nearVectorType!=0);
	}
	else if(xVector.norm_style==3)
	{
		pPropItem=oper.InsertButtonPropItem(pParentItem,CXhChar50("%s.hVicePart",sPrefix),"","",-1,TRUE);
		UpdatePropItemName(&m_xSubPropList,pPropItem,xVector.norm_style);
		//
		pPropItem=oper.InsertCmbListPropItem(pParentItem,CXhChar50("%s.direction",sPrefix),"","","",-1,TRUE);		
		UpdatePropItemName(&m_xSubPropList,pPropItem,xVector.norm_style);
	}
	else if(xVector.norm_style==4)
	{
		pPropItem=oper.InsertButtonPropItem(pParentItem,CXhChar50("%s.hVicePart",sPrefix),"","",-1,TRUE);
		UpdatePropItemName(&m_xSubPropList,pPropItem,xVector.norm_style);
		//
		pPropItem=oper.InsertCmbListPropItem(pParentItem,CXhChar50("%s.nearVectorType",sPrefix),"","","",-1,TRUE);
		UpdatePropItemName(&m_xSubPropList,pPropItem,xVector.norm_style);
		int nearVectorType=CStdVector::GetVectorType(xVector.nearVector);

		pSonItem=oper.InsertEditPropItem(pPropItem,CXhChar50("%s.nearVector.x",sPrefix),"","",-1,TRUE);
		pSonItem->SetReadOnly(nearVectorType!=0);
		
		pSonItem=oper.InsertEditPropItem(pPropItem,CXhChar50("%s.nearVector.y",sPrefix),"","",-1,TRUE);
		pSonItem->SetReadOnly(nearVectorType!=0);
		
		pSonItem=oper.InsertEditPropItem(pPropItem,CXhChar50("%s.nearVector.z",sPrefix),"","",-1,TRUE);
		pSonItem->SetReadOnly(nearVectorType!=0);
	}
	else if(xVector.norm_style==5)
	{	//5.基准杆件上的特征方向
		pPropItem=oper.InsertButtonPropItem(pParentItem,CXhChar50("%s.hVicePart",sPrefix),"","",-1,TRUE);
		UpdatePropItemName(&m_xSubPropList,pPropItem,xVector.norm_style);
		//方向类型
		pPropItem=oper.InsertCmbListPropItem(pParentItem,CXhChar50("%s.direction",sPrefix),"","","",-1,TRUE);
		UpdatePropItemName(&m_xSubPropList,pPropItem,xVector.norm_style);
	}
}
//显示子属性
void CDesignCircularPlateDlg::DisplaySubPropList(CPropTreeItem *pItem/*=NULL*/)
{
	m_xSubPropList.CleanTree();
	m_xSubPropList.m_nObjClassTypeId = 0;
	//
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pSonItem=NULL,*pRoot=m_xSubPropList.GetRootItem();
	CPropertyListOper<CDesignCircularPlateDlg> oper(&m_xSubPropList,this);
	if(pItem&&pItem->m_idProp==GetPropID("ucs.axis_z")
		&&pItem->m_lpNodeInfo->m_strPropValue.GetLength()>0
		&&!pItem->IsReadOnly())	//法线设计参数
	{	//显示法线方向
		pGroupItem=oper.InsertGrayButtonPropItem(pRoot,"axis_z","","","",-1,TRUE);
		
		pPropItem=oper.InsertEditPropItem(pGroupItem,"ucs.axis_z.x","","",-1,TRUE);
		pPropItem->SetReadOnly(design_axis_z.norm_style!=0);
		
		pPropItem=oper.InsertEditPropItem(pGroupItem,"ucs.axis_z.y","","",-1,TRUE);
		pPropItem->SetReadOnly(design_axis_z.norm_style!=0);
		
		pPropItem=oper.InsertEditPropItem(pGroupItem,"ucs.axis_z.z","","",-1,TRUE);
		pPropItem->SetReadOnly(design_axis_z.norm_style!=0);
		//法线设计参数
		//pGroupItem=oper.InsertPropItem(pRoot,"design_axis_z");
		pGroupItem=oper.InsertCmbListPropItem(pRoot,"design_axis_z.norm_style","","","",-1,TRUE);
		UpdateVectorPropList(pGroupItem,"design_axis_z",design_axis_z);
	}
	else if(pItem&&pItem->m_pParent&&pItem->m_pParent->m_idProp==CDesignCircularPlateDlg::GetPropID("RelaPole"))
	{	//当前选中项为方向索性
		if(m_pCurSelConnVec==NULL)
			return;
		pGroupItem=oper.InsertPropItem(pRoot,"ConnDir");
		//端距
		oper.InsertEditPropItem(pGroupItem,"ConnDir.end_space","","",-1,TRUE);
		//端距高
		oper.InsertEditPropItem(pGroupItem,"ConnDir.high_space","","",-1,TRUE);
		//方向设计参数
		//pGroupItem=oper.InsertPropItem(pRoot,"ConnDir.dir");
		pGroupItem=oper.InsertCmbListPropItem(pRoot,"ConnDir.dir.norm_style","","","",-1,TRUE);
		UpdateVectorPropList(pGroupItem,"ConnDir.dir",m_pCurSelConnVec->direct);	
	}
	else if(((m_iPlateType==2&&para.nDHRelativeType==0)||(m_iPlateType==3&&para.nDHRelativeType))
		&&(pItem&&pItem->m_idProp==GetPropID("hRelaPart1")||pItem&&pItem->m_idProp==GetPropID("hRelaPart2")))
	{	//连接方向
		int serial=-1;
		if(pItem->m_idProp==GetPropID("hRelaPart1"))
			serial=1;
		else 
			serial=2;
		if(m_pCurSelConnVec==NULL||m_pCurSelConnVec->serial!=serial)//查询钢板关联方向
			m_pCurSelConnVec=ConnVecList.GetRelaConnInfo(serial);
		if(m_pCurSelConnVec==NULL)
		{
			m_pCurSelConnVec=ConnVecList.Append();
			m_pCurSelConnVec->serial=(BYTE)serial;
			m_pCurSelConnVec->style=0;
			m_pCurSelConnVec->direct.norm_style=5;
			//更新当前项显示内容
			CXhChar50 sText;
			if(GetPropValueStr(pItem->m_idProp,sText)>0)
				m_propList.SetItemPropValue(pItem->m_idProp,sText);
		}
		pGroupItem=oper.InsertPropItem(pRoot,"ConnDir");
		//连接方向类型
		oper.InsertCmbListPropItem(pGroupItem,"ConnDir.style","","","",-1,TRUE);
		//端距
		oper.InsertEditPropItem(pGroupItem,"ConnDir.end_space","","",-1,TRUE);
		//边距
		oper.InsertEditPropItem(pGroupItem,"ConnDir.high_space","","",-1,TRUE);
		//法线设计参数
		//pGroupItem=oper.InsertPropItem(pRoot,"ConnDir.dir");
		pGroupItem=oper.InsertCmbListPropItem(pRoot,"ConnDir.dir.norm_style","","","",-1,TRUE);
		UpdateVectorPropList(pGroupItem,"ConnDir.dir",m_pCurSelConnVec->direct);
	}
	else if(m_bNewCirPlate&&pItem&&pItem->m_idProp==GetPropID("CreateBolt"))
	{
		//螺栓环向布置参数
		pGroupItem=oper.InsertPropItem(pRoot,"m_xLayoutBolt");
		//基准钢管半径
		pPropItem=oper.InsertEditPropItem(pGroupItem,"tube_r","","",-1,TRUE);
		pPropItem->SetReadOnly(TRUE);
		if(m_pDatumTube->m_bHasWeldRoad)
		{	//环向布置起始边,0.基准边,1.焊道线
			oper.InsertCmbListPropItem(pGroupItem,"m_xLayoutBolt.m_iStartEdge","","","",-1,TRUE);
		}
		//螺栓个数
		oper.InsertEditPropItem(pGroupItem,"m_xLayoutBolt.m_nBoltNum","","",-1,TRUE);
		//首螺栓相对于起始边之间的偏移角度
		oper.InsertEditPropItem(pGroupItem,"m_xLayoutBolt.m_fOffsetAngle","","",-1,TRUE);	
		//螺栓所在半径
		oper.InsertEditPropItem(pGroupItem,"m_xLayoutBolt.m_fBoltLayoutR","","",-1,TRUE);
		//螺栓直径
		oper.InsertCmbEditPropItem(pGroupItem,"m_xLayoutBolt.m_nBoltD","","","",-1,TRUE);
		//螺栓朝向
		oper.InsertCmbListPropItem(pGroupItem,"m_xLayoutBolt.m_iDriection","","","",-1,TRUE);
		//段号
		oper.InsertEditPropItem(pGroupItem,"m_xLayoutBolt.m_iBoltSegI","","",-1,TRUE);
	}
	else 
	{	//显示外形参数
		pGroupItem=oper.InsertPropItem(pRoot,"profile");
		UpdatePlateProfileParamProp(pGroupItem);
	}
	m_xSubPropList.Redraw();
}

void CDesignCircularPlateDlg::UpdateSketchMap()
{
	m_xCurPlateBmp.Detach();
	if(m_iPlateType==0)			
	{
		if(ConnVecList.GetCount(TRUE)>0)	//异形法兰
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_SPECIAL_PROFILE_FL);
		else				//普通环向板
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_PLATE);
	}
	else if(m_iPlateType==1)	//1.环向封板
	{
		if(para.nCirCoverPlateType==0)
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_COVERPLATE2);
		else 
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_COVERPLATE3);
	}
	else if(m_iPlateType==2)	//2.环向肋板3WL(三边支撑直线型)
	{
		if(para.nDHRelativeType==0)	//跨越链板	//if(para.D>0&&para.H>0)//ConnVecList.IsNeedBend())	//跨越链板
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE5);//IDB_BMP_CIRCULAR_RIBPLATE6);
		else
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE1);
	}
	else if(m_iPlateType==3)	//3.环向肋板3WA(三边支撑圆弧型)
	{
		if(para.nDHRelativeType!=0)	//跨越链板 //if(para.D>0&&para.H>0)//(ConnVecList.IsNeedBend()) //跨越链板
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE5);
		else	
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE2);
	}
	else if(m_iPlateType==4)	//4.环向肋板2WL(两边支撑直线型)
	{
		if(para.D>0)
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE3);
		else //if(para.B>=0)
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE7);
	}
	else if(m_iPlateType==5)	//5.环向肋板2WA(两肢支撑圆弧型)
		m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE4);
	else if(m_iPlateType==6)	//6.遮水板
		m_xCurPlateBmp.LoadBitmap(IDB_BMP_SHIELDEDWATER_PLATE);
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
	pPic->SetBitmap(m_xCurPlateBmp);
}

void CDesignCircularPlateDlg::AddRelaPole(CLDSLinePart *pPole)
{
	int nCurNodeNum=ConnVecList.GetCount();
	CONNECT_DIRECTION connectVec;
	connectVec.serial=10+nCurNodeNum;
	connectVec.end_space=30;
	connectVec.high_space=30;
	if(pPole->pStart&&pPole->pStart==m_pDatumNode)
		connectVec.direct.direction=0;
	else 
		connectVec.direct.direction=1;
	connectVec.direct.hVicePart=pPole->handle;
	ConnVecList.Append(connectVec);
	//更新环向板关联构件列表 wht 11-05-18
	if(m_pCirPlate)
	{
		m_pCirPlate->ConnDirectList.append(connectVec);
		ReDesignPlate();	//重新设计环向板
	}
	//异形法兰添加关联杆件时自动选择引入钢管插板相关螺栓 wht 11-05-19
	if(pPole->IsTube()&&m_iPlateType==0)
	{
		CLDSParamPlate *pParamPlate=NULL;
		CLDSLineTube *pLineTube=(CLDSLineTube*)pPole;
		if(pLineTube->pStart&&pLineTube->pStart==m_pDatumNode)
			pParamPlate=(CLDSParamPlate*)pLineTube->GetParamPlate(0);
		else if(pLineTube->pEnd&&pLineTube->pEnd==m_pDatumNode)
			pParamPlate=(CLDSParamPlate*)pLineTube->GetParamPlate(1);
		//
		CLsRef *pLsRef=NULL;
		design_axis_z.UpdateVector(console.GetActiveModel());
		if(pParamPlate&&fabs(pParamPlate->ucs.axis_z*design_axis_z.vector)>EPS_COS2
			&&(pParamPlate->m_iParamType==TYPE_ROLLEND||pParamPlate->m_iParamType==TYPE_UEND))
		{	//槽型插板或U型插板
			for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
			{
				if(LsRefList.FromHandle((*pLsRef)->handle))
					continue;	//避免重复引入
				LsRefList.append(*pLsRef);
			}
		}
		else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
		{	//十字插板
			double L=0;
			pParamPlate->GetDesignItemValue('L',&L);
			for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
			{
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				if(fabs(pBolt->ucs.axis_z*design_axis_z.vector)<EPS_COS2)
					continue;	
				if(LsRefList.FromHandle(pBolt->handle))
					continue;	//避免重复引入
				f3dPoint pos=pBolt->ucs.origin;
				coord_trans(pos,pParamPlate->ucs,FALSE);
				if(pos.x<L)
					continue;
				LsRefList.append(*pLsRef);
			}
		}
	}
}

//完成选择对象的后续操作
void CDesignCircularPlateDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	CLDSPart *pPart=NULL;
	if(GetEventPropId()==GetPropID("hDatumSphere"))
	{
		if(pPart=console.FromPartHandle(m_hPickObj,CLS_SPHERE))
			m_pDatumSphere=(CLDSSphere*)pPart;
	}
	else if(GetEventPropId()==GetPropID("hDatumTube"))
	{
		if(pPart=console.FromPartHandle(m_hPickObj,CLS_LINETUBE))
		{
			m_pDatumTube=(CLDSLineTube*)pPart;
			if(m_pCirPlate)	//基准钢管
				ModifyPlateParam('C',m_pDatumTube->handle);
		}
	}
	else if(GetEventPropId()==GetPropID("hDatumNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
		if(pNode)
		{
			m_pDatumNode=pNode;
			if(m_pCirPlate&&m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				ModifyPlateParam('E',m_pDatumNode->handle);
			else if(m_pCirPlate&&m_pCirPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
				ModifyPlateParam('B',m_pDatumNode->handle);
			else if(m_pCirPlate&&m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE)
				ModifyPlateParam('P',m_pDatumNode->handle);
		}
	}
	else if(m_idSubEventProp==0&&GetEventPropId()==GetPropID("hRelaPart1"))
	{
		if(pPart=console.FromPartHandle(m_hPickObj,CLS_PLATE,CLS_PARAMPLATE))
		{
			m_pRelaPart1=pPart;
			InitProfilePara();	//初始化外形参数
			if(m_pCirPlate&&m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				ModifyPlateParam('F',m_pRelaPart1->handle);
			else if(m_pCirPlate&&(m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE))
				ModifyPlateParam('E',m_pRelaPart1->handle);
		}
	}
	else if(m_idSubEventProp==0&&GetEventPropId()==GetPropID("hRelaPart2"))
	{
		if(pPart=console.FromPartHandle(m_hPickObj,CLS_PLATE,CLS_PARAMPLATE,CLS_LINETUBE))
		{
			m_pRelaPart2=pPart;
			InitProfilePara();	//初始化外形参数
			if(m_iPlateType!=1||
				(m_iPlateType==1&&pPart->GetClassTypeId()==CLS_PLATE))
			{
				if(m_pCirPlate&&m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
					ModifyPlateParam('G',m_pRelaPart2->handle);
				else if(m_pCirPlate&&(m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE))
					ModifyPlateParam('F',m_pRelaPart2->handle);
			}
		}
	}
	//法线设计参数
	else if(GetPropID("design_axis_z.hVicePart")==m_idSubEventProp
		||GetPropID("design_axis_z.hCrossPart")==m_idSubEventProp)
	{
		if(GetPropID("design_axis_z.hVicePart")==m_idSubEventProp)
		{
			CLDSPart *pPart=NULL;
			if(design_axis_z.norm_style==1)
				pPart=console.FromPartHandle(m_hPickObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			else if(design_axis_z.norm_style==2
					||design_axis_z.norm_style==5)
				pPart=console.FromPartHandle(m_hPickObj,CLS_LINEPART);
			else 
				pPart=console.FromPartHandle(m_hPickObj);
			if(pPart)
				design_axis_z.hVicePart=pPart->handle;
		}
		else if(design_axis_z.norm_style==2&&
			GetPropID("design_axis_z.hCrossPart")==m_idSubEventProp)
		{
			CLDSPart *pPart=console.FromPartHandle(m_hPickObj);
			if(pPart)
				design_axis_z.hCrossPart=pPart->handle;
		}
		design_axis_z.UpdateVector(console.GetActiveModel());
		if(m_pCirPlate)
		{
			CONNECT_DIRECTION *pPlateCurConVec=m_pCirPlate->GetRelaConnInfo(1);
			if(pPlateCurConVec)
			{	//修改环向板连接方向参数并重新设计 wht 11-05-31
				pPlateCurConVec->direct.hVicePart=design_axis_z.hVicePart;
				pPlateCurConVec->direct.hCrossPart=design_axis_z.hCrossPart;
				ReDesignPlate();
			}
		}
	}
	//连接方向参数
	else if(m_pCurSelConnVec
		&&(GetPropID("ConnDir.dir.hVicePart")==m_idSubEventProp
		||GetPropID("ConnDir.dir.hCrossPart")==m_idSubEventProp))
	{
		if(GetPropID("ConnDir.dir.hVicePart")==m_idSubEventProp)
		{
			CLDSPart *pPart=NULL;
			if(m_pCurSelConnVec->direct.norm_style==1)
				pPart=console.FromPartHandle(m_hPickObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			else if(m_pCurSelConnVec->direct.norm_style==2
				||m_pCurSelConnVec->direct.norm_style==5)
				pPart=console.FromPartHandle(m_hPickObj,CLS_LINEPART);
			else 
				pPart=console.FromPartHandle(m_hPickObj);
			if(pPart)
				m_pCurSelConnVec->direct.hVicePart=pPart->handle;
			if(m_pCurSelConnVec->serial==2)
				AutoCalWN();	//修改第二面连接方向时重新计算W、N值 wht 11-05-30
		}
		else if(design_axis_z.norm_style==2&&
			GetPropID("ConnDir.dir.hCrossPart")==m_idSubEventProp)
		{
			CLDSPart *pPart=console.FromPartHandle(m_hPickObj);
			if(pPart)
				m_pCurSelConnVec->direct.hCrossPart=pPart->handle;
		}
		m_pCurSelConnVec->direct.UpdateVector(console.GetActiveModel());
		CXhChar100 sText;
		if(m_pCurSelConnVec->serial==1)
		{	//更新主属性栏显示状态
			long idProp=GetPropID("hRelaPart1");
			if(GetPropValueStr(idProp,sText)>0)
				m_propList.SetItemPropValue(idProp,sText);
		}
		else if(m_pCurSelConnVec->serial==2)
		{	//更新主属性栏显示状态
			long idProp=GetPropID("hRelaPart2");
			if(GetPropValueStr(idProp,sText)>0)
				m_propList.SetItemPropValue(idProp,sText);
		}
		if(m_pCirPlate)
		{
			CONNECT_DIRECTION *pPlateCurConVec=m_pCirPlate->GetRelaConnInfo(m_pCurSelConnVec->serial);
			if(pPlateCurConVec)
			{	//修改环向板连接方向参数并重新设计 wht 11-05-31
				pPlateCurConVec->direct.hVicePart=m_pCurSelConnVec->direct.hVicePart;
				pPlateCurConVec->direct.hCrossPart=m_pCurSelConnVec->direct.hCrossPart;
				ReDesignPlate();
			}
		}
	}
	else if(CDesignCircularPlateDlg::GetPropID("RelaPole")==m_idEventProp)
	{	//关联杆件
		//ConnVecList.Empty(TRUE); 不清空原有杆件
		//
		long *id_arr;
		int nPoleNum=g_pSolidSnap->GetLastSelectEnts(id_arr);
		for(int i=0;i<nPoleNum;i++)
		{
			CLDSLinePart *pPole=(CLDSLinePart*)console.FromPartHandle(id_arr[i],CLS_LINEPART);
			if(pPole==NULL)
				continue;
			AddRelaPole(pPole);
		}
	}
	else if(CDesignCircularPlateDlg::GetPropID("RelaBolt")==m_idEventProp)
	{	//关联螺栓
		long *id_arr;
		int nBoltNum=g_pSolidSnap->GetLastSelectEnts(id_arr);
		LsRefList.Empty();
		for(int i=0;i<nBoltNum;i++)
		{
			CLDSBolt *pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[i],CLS_BOLT);
			if(pBolt==NULL)
				continue;
			LsRefList.append(pBolt->GetLsRef());
		}
		//更新环向板螺栓引用 wht 11-05-18
		if(m_pCirPlate)
		{
			m_pCirPlate->EmptyLsRef();
			CLsRef *pLsRef=NULL;
			for(pLsRef=LsRefList.GetFirst();pLsRef;pLsRef=LsRefList.GetNext())
				m_pCirPlate->AppendLsRef(*pLsRef);
			ReDesignPlate();
		}
	}
}

BOOL CDesignCircularPlateDlg::IsValidParam()
{
	BOOL bRet=FALSE;
	CLDSPlate *pTransPlate=NULL;
	if(m_pRelaPart1&&m_pRelaPart1->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)m_pRelaPart1)->IsTransPlate())
		pTransPlate=(CLDSPlate*)m_pRelaPart1;	//第一块钢板为贯通板 wht 11-06-03
	if(m_pDatumNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Lack of locating datum node！");
	else if(m_pDatumTube==NULL&&m_pDatumSphere==NULL)
		AfxMessageBox("Lack of locating datum tube！");
#else 
		AfxMessageBox("缺少定位基准节点！");
	else if(m_pDatumTube==NULL&&m_pDatumSphere==NULL)
		AfxMessageBox("缺少定位基准钢管！");
#endif
	else if(m_iPlateType>0&&m_iPlateType<6)
	{
		if(m_pRelaPart1==NULL||!m_pRelaPart1->IsPlate())
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Lack of locating datum plate！");
#else 
			AfxMessageBox("缺少定位基准钢板！");
#endif
		else if(m_pRelaPart2==NULL&&pTransPlate==NULL
			&&(m_iPlateType==2||m_iPlateType==3))
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Lack of 2nd locating datum plate！");
#else 
			AfxMessageBox("缺少定位基准第二块定位基准钢板！");
#endif
		else if(m_iPlateType==1)
		{
			if(m_pRelaPart1->GetClassTypeId()!=CLS_PLATE||
				(m_pRelaPart2&&m_pRelaPart2->GetClassTypeId()!=CLS_PLATE))
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Circular sealing plate's datum part should be plate！");
#else 
				AfxMessageBox("环向封板基准构件应为钢板！");
#endif
			else 
				bRet=TRUE;
		}
		else 
			bRet=TRUE;
	}
	else if(m_pDatumTube)
	{
		f3dLine line;
		if(m_pDatumTube->pStart)
			line.startPt=m_pDatumTube->pStart->Position(false);
		else 
			line.startPt=m_pDatumTube->Start();
		if(m_pDatumTube->pEnd)
			line.endPt=m_pDatumTube->pEnd->Position(false);
		else 
			line.endPt=m_pDatumTube->End();
		if(line.PtInLine(m_pDatumNode->Position(false))==0)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Datum node isn't on datum tube！");
#else 
			AfxMessageBox("基准节点不在基准钢管上！");
#endif
		else
			bRet=TRUE;
	}
	else 
		bRet=TRUE;
	if(bRet)
	{	//判断外形参数是否合法
		bRet=FALSE;
		int nValidPoleVec=0;
		CONNECT_DIRECTION *pVec=NULL,*pFirstVec=NULL,*pSecVec=NULL;
		for(pVec=ConnVecList.GetFirst();pVec;pVec=ConnVecList.GetNext())
		{
			if(m_iPlateType==0)
			{	//异形法兰
				if(pVec->direct.hVicePart>0x20)
					nValidPoleVec++;
			}
			else if(m_iPlateType==2)
			{	//制弯肋板
				if(pVec->serial==1)
					pFirstVec=pVec;
				else if(pVec->serial==2)
					pSecVec=pVec;
			}
		}
		if(m_iPlateType==0)
		{
			BOOL bSpecificFl=ConnVecList.GetCount(TRUE);
#ifdef AFX_TARG_ENU_ENGLISH
			if(para.B<=0)
				AfxMessageBox("Circular plate's section angle β is illegal！");
			else if(bSpecificFl&&nValidPoleVec<=0)	//异形法兰未输入射线杆件方向
				AfxMessageBox("Not input legal ray rod that can't create special-shaped flange！");
			else if(bSpecificFl&&para.D<=0)			//异形法兰未输入射线杆件方向
				AfxMessageBox("Not input legal free margin D that can't create special-shaped flange！");
#else 
			if(para.B<=0)
				AfxMessageBox("环向板扇形角β为非法数据！");
			else if(bSpecificFl&&nValidPoleVec<=0)	//异形法兰未输入射线杆件方向
				AfxMessageBox("未输入合法的射线杆件不能生成异形法兰！");
			else if(bSpecificFl&&para.D<=0)			//异形法兰未输入射线杆件方向
				AfxMessageBox("未输入合法的自由边距D,不能生成异形法兰！");
#endif
			else
				bRet=TRUE;
		}
		else if(m_iPlateType==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(para.B<=0)
				AfxMessageBox("Circular sealing plate's section angle β is illegal！");
			else if(para.W<=0)
				AfxMessageBox("Circular sealing plate's band width W is illegal！");
			else if(para.H<=0)
				AfxMessageBox("Circular sealing plate's param H is illega！");
			else if(para.B>=360)	//不支持生成大于等于360的环向封板 wht 11-05-31
				AfxMessageBox("Don't support to create circular sealing plate which is binger than 360！");
#else 
			if(para.B<=0)
				AfxMessageBox("环向封板扇形角β为非法数据！");
			else if(para.W<=0)
				AfxMessageBox("环向封板环形带宽W为非法数据！");
			else if(para.H<=0)
				AfxMessageBox("环向封板台口高度H为非法数据！");
			else if(para.B>=360)	//不支持生成大于等于360的环向封板 wht 11-05-31
				AfxMessageBox("不支持生成大于等于360的环向封板！");
#endif
			else 
				bRet=TRUE;
		}
		else if(m_iPlateType==2)
		{	//3WL肋板
			BOOL bNeedBend=ConnVecList.IsNeedBend();
			if(!bNeedBend&&para.nDHRelativeType==1&&(para.D<para.W||para.D<para.N))
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The free margin D is too small！");
#else 
				AfxMessageBox("自由边距D过小！");
#endif
			else if(bNeedBend)
			{
				if(pFirstVec==NULL||pSecVec==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Can't input legal connect direction！");
#else 
					AfxMessageBox("未输入合法的连接方向！");
#endif
				else 
				{
					pFirstVec->direct.UpdateVector(console.GetActiveModel());
					pSecVec->direct.UpdateVector(console.GetActiveModel());
					f3dPoint first_vec=pFirstVec->direct.vector;
					f3dPoint sec_vec=pSecVec->direct.vector;
					if(first_vec.IsZero()||sec_vec.IsZero()||sec_vec==first_vec)
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("Can't input legal connect direction！");
#else 
						AfxMessageBox("未输入合法的连接方向！");
#endif
					else 
						bRet=TRUE;
				}
			}
			else 
				bRet=TRUE;
		}
		else if(m_iPlateType==3)
		{	//3WA肋板
			if(para.nDHRelativeType==0&&para.R<=0)
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Circular R is illegal！");
#else 
				AfxMessageBox("环形带宽R为非法数据！");
#endif
			else
				bRet=TRUE;
		}
		else if(m_iPlateType==4)
		{	//2WL
			double R=m_pDatumTube->GetDiameter()*0.5;
			f2dCircle cir(R,f2dPoint(0,0));
			f2dPoint base_pt(R+para.W,para.H+m_pRelaPart1->GetThick()*0.5);	//拾取点
			f2dPoint tan_pt;
			//判断D参数输入值是否合法 wht 11-05-12
			if(Tang2dpc(base_pt,cir,f2dPoint(0,R),tan_pt))
			{
				if(para.D>tan_pt.y)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					CXhChar100 sText("The input parameter of D is too large,max value of D is%.1f!",tan_pt.y);
#else 
					CXhChar100 sText("参数D输入值过大，D最大取值为%.1f!",tan_pt.y);
#endif
					AfxMessageBox(sText);
				}
				else 
					bRet=TRUE;
			}
			else 
				bRet=TRUE;
		}
		else if(m_iPlateType==5)
			bRet=TRUE;
		else if(m_iPlateType==6)
		{
			if(water.W<=0)
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The shelter plate's outer diameter W is illegal！");
#else 
				AfxMessageBox("遮水板外环直径W为非法数据！");
#endif
			else 
				bRet=TRUE;
		}
	}
	if(bRet)
		return TRUE;
	return FALSE;
}

void CDesignCircularPlateDlg::OnOK()
{
	//屏蔽回车确认方便修改属性
}

void CDesignCircularPlateDlg::OnConfirm()
{
	if(!IsValidParam())
		return;	//检查参数合法性
	if(m_pCirPlate)
	{
		design_axis_z.UpdateVector(console.GetActiveModel());
		m_pCirPlate->ucs.axis_z=design_axis_z.vector;	//Z轴方向
	}
	CDialog::OnOK();
}

//自动计算W,N参数
BOOL CDesignCircularPlateDlg::AutoCalWN()
{
	if(m_pCirPlate&&para.G<=0&&m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE)
		return FALSE;	//修改参数不重新计算顶部圆弧连接环向板的W,N长度(3WA) wht 11-05-24
	if(m_iPlateType<2&&m_iPlateType>5)
		return FALSE;
	CLDSPart *pDatumPlate[2]={NULL};
	if(m_pRelaPart1&&m_pRelaPart1->IsPlate())
		pDatumPlate[0]=m_pRelaPart1;
	if(m_pRelaPart2&&m_pRelaPart2->IsPlate())
		pDatumPlate[1]=m_pRelaPart2;
	if(pDatumPlate[0]==NULL)
		return FALSE;
	//
	UCS_STRU ucs;	//假定的肋板坐标系
	design_axis_z.UpdateVector(console.GetActiveModel());
	ucs.axis_z=design_axis_z.vector;
	normalize(ucs.axis_z);
	//坐标原点初始位置
	double S=para.S;
	f3dPoint origin;	//坐标原点初始位置
	f3dPoint datum_axis_z;
	if(m_pDatumNode&&m_pDatumTube)
	{
		m_pDatumTube->BuildUCS();
		SnapPerp(&origin,m_pDatumTube->Start(),m_pDatumTube->End(),m_pDatumNode->Position(true));
		datum_axis_z=m_pDatumTube->End()-m_pDatumTube->Start();
		normalize(datum_axis_z);
		if(fabs(para.S)>0)
		{
			double cosa=fabs(datum_axis_z*ucs.axis_z);
			if(cosa>EPS) 
				S=para.S/cosa;
		}
	}
	ucs.origin = origin;
	//基准钢板在钢管垂直截面内的射线方向
	f3dPoint axis_x[2];
	typedef ATOM_LIST<PROFILE_VERTEX>* VertexListPtr; 
	VertexListPtr ptrVertexList[2]={NULL};
	int i;
	for(i=0;i<2;i++)
	{
		if(pDatumPlate[i]==NULL)
			continue;
		axis_x[i]=ucs.axis_z^pDatumPlate[i]->ucs.axis_z;
		normalize(axis_x[i]);
		if(pDatumPlate[i]->GetClassTypeId()==CLS_PLATE)
			ptrVertexList[i]=&(((CLDSPlate*)pDatumPlate[i])->vertex_list);
		else if(pDatumPlate[i]->GetClassTypeId()==CLS_PARAMPLATE)
			ptrVertexList[i]=&(((CLDSParamPlate*)pDatumPlate[i])->vertex_list);
		if(ptrVertexList[i]==NULL)
			continue;
		//修正射线方向的正负号
		f3dPoint vertice,vertice_vec;
		if(ptrVertexList[i]->GetNodeNum()>0)
		{
			vertice=ptrVertexList[i]->GetFirst()->vertex;
			coord_trans(vertice,pDatumPlate[i]->ucs,TRUE);
			vertice_vec=vertice-ucs.origin;
			if(vertice_vec*axis_x[i]<0)
				axis_x[i]*=-1.0;
		}
	}
	//
	f3dPoint verify_vec;	//用于确定坐标系
	if(m_iPlateType>1&&m_iPlateType<6&&pDatumPlate[0])
	{
		if(para.Y>=0)
		{	//跨越环向板在关联钢板外侧
			if(para.Y>0)	//基板正侧
				verify_vec=pDatumPlate[0]->ucs.axis_z;
			else
			{	//自动判断
				if(pDatumPlate[1]==NULL||fabs(axis_x[0]*axis_x[1])>EPS_COS)
					verify_vec=pDatumPlate[0]->ucs.axis_z;
				else
				{
					f3dPoint bi_vec=axis_x[0]+axis_x[1];
					normalize(bi_vec);
					if(pDatumPlate[0]->ucs.axis_z*bi_vec>0)	//第一块钢板工作法向指凹角内侧
						verify_vec=pDatumPlate[0]->ucs.axis_z;
					else									//第一块钢板工作法向指凹角外侧，需反号后再赋值
						verify_vec=-pDatumPlate[0]->ucs.axis_z;
				}
			}
		}
		else
		{	//跨越环向板在关联钢板内侧
			if(pDatumPlate[1]==NULL||fabs(axis_x[0]*axis_x[1])>EPS_COS)
				verify_vec=-pDatumPlate[0]->ucs.axis_z;
			else
			{
				f3dPoint bi_vec=axis_x[0]+axis_x[1];
				if(pDatumPlate[0]->ucs.axis_z*bi_vec>0)	//第一块钢板工作法向指凹角内侧
					verify_vec= pDatumPlate[0]->ucs.axis_z;
				else									//第一块钢板工作法向指凹角外侧，需反号后再赋值
					verify_vec=-pDatumPlate[0]->ucs.axis_z;
			}
		}
	}
	double r=0;
	if(m_pDatumTube)
		r=m_pDatumTube->GetDiameter()*0.5;
	else if(m_pDatumSphere)
	{
		r=m_pDatumSphere->D*0.5;
		double y_offset_dist=0;
		if(pDatumPlate[0]->GetClassTypeId()==CLS_PLATE)
			y_offset_dist=((CLDSPlate*)pDatumPlate[0])->m_fNormOffset;
		else if(pDatumPlate[1]->GetClassTypeId()==CLS_PARAMPLATE)
			y_offset_dist=((CLDSParamPlate*)pDatumPlate[0])->m_fNormOffset;
		//if(m_bDesignOuterPlate)
		//	y_offset_dist+=m_pDatumPlate1->GetThick();
		r=sqrt(r*r-y_offset_dist*y_offset_dist-para.S*para.S);
	}

	f3dPoint weldvec_n;
	f3dPoint huoqufacenorm;
	CONNECT_DIRECTION *pConVec2=ConnVecList.GetRelaConnInfo(2);
	if(pConVec2&&m_pRelaPart2)
	{	//计算第二面连接方向
		pConVec2->direct.UpdateVector(console.GetActiveModel()); //根据参数计算方向
		if(pConVec2->style==1)
			huoqufacenorm=pConVec2->direct.vector;
		else if(pConVec2->style==0)
		{
			if(m_pRelaPart2&&m_pRelaPart2->IsPlate())
				huoqufacenorm=m_pRelaPart2->ucs.axis_z^pConVec2->direct.vector;
			else if(m_pRelaPart2&&m_pRelaPart2->IsTube()&&m_pDatumTube)
			{
				m_pDatumTube->BuildUCS();
				((CLDSLineTube*)m_pRelaPart2)->BuildUCS();
				f3dPoint axis_z=m_pRelaPart2->ucs.axis_z^m_pDatumTube->ucs.axis_z;
				normalize(axis_z);
				huoqufacenorm=axis_z^pConVec2->direct.vector;
			}
		}
		normalize(huoqufacenorm);
		if(fabs(ucs.axis_z*huoqufacenorm)<EPS_COS)
		{	//钢板为火曲板计算第二面连接方向
			if(m_pRelaPart2&&m_pRelaPart2->IsTube()&&m_pDatumNode)
			{
				CLDSLineTube *pRelaTube=(CLDSLineTube*)m_pRelaPart2;
				if(pRelaTube->pStart&&m_pDatumNode==pRelaTube->pStart)
					weldvec_n=pRelaTube->End()-pRelaTube->Start();
				else
					weldvec_n=pRelaTube->Start()-pRelaTube->End();
			}
			else if(m_pRelaPart2&&m_pRelaPart2->IsPlate())
			{
				if(pConVec2->style==0)
				{
					weldvec_n=pConVec2->direct.vector;
					project_vector(weldvec_n,m_pRelaPart2->ucs.axis_z);
				}
				else
					weldvec_n=m_pRelaPart2->ucs.axis_z^pConVec2->direct.vector;
				if(weldvec_n.IsZero())	//焊缝走向为默认法向的环向板与关联板的两面交线
					weldvec_n=ucs.axis_z^m_pRelaPart2->ucs.axis_z;
			}
			normalize(weldvec_n);
		}	
	}
	for(i=0;i<2;i++)
	{
		if(pDatumPlate[i]==NULL)
			continue;
		BOOL bCalNByFirstPlate=FALSE;	
		if(i==0&&pDatumPlate[1]==NULL&&pDatumPlate[0]->GetClassTypeId()==CLS_PLATE
			&&((CLDSPlate*)pDatumPlate[0])->IsTransPlate())
			bCalNByFirstPlate=TRUE;	//仅选中一块钢板且钢板为贯通板时根据该钢板计算N值 wht 11-05-20
		PROFILE_VERTEX *pPreVertex=NULL;
		//计算肋板与基准钢板相交边的长度
		ucs.axis_x = axis_x[i];
		if(fabs(ucs.axis_z*huoqufacenorm)<EPS_COS
			&&i==1&&!weldvec_n.IsZero())
		{	
			ucs.axis_z=huoqufacenorm;
			//根据第二面延伸方向纠正axis_x[1]方向 wht 11-05-30
			if(weldvec_n*axis_x[1]<0)
				ucs.axis_x=-1.0*weldvec_n;
			else 
				ucs.axis_x=weldvec_n;
			ucs.axis_y = ucs.axis_z^ucs.axis_x;
			ucs.axis_z = ucs.axis_x^ucs.axis_y;
		}
		else 
			ucs.axis_y = ucs.axis_z^ucs.axis_x;
		if(ucs.axis_y*verify_vec<0)
		{
			ucs.axis_y*=-1.0;
			ucs.axis_z*=-1.0;
		}
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		normalize(ucs.axis_z);
		if(datum_axis_z*ucs.axis_z>0)
			ucs.origin=origin+datum_axis_z*S;
		else
			ucs.origin=origin-datum_axis_z*S;
		if(i==0)	//环向板肋板坐标系
			plate_ucs=ucs;
		//
		if(ptrVertexList[i]==NULL)
			continue;
		f3dPoint inter_pos,base_pos=ucs.origin;
		pPreVertex=ptrVertexList[i]->GetTail(); //钢板的第一个顶点的前一个顶点为最后一个顶点
		for(PROFILE_VERTEX *pVertex=ptrVertexList[i]->GetFirst();pVertex;pVertex=ptrVertexList[i]->GetNext())
		{
			if(pPreVertex&&pPreVertex!=pVertex)
			{
				f3dLine line(pPreVertex->vertex,pVertex->vertex);
				coord_trans(line.startPt,pDatumPlate[i]->ucs,TRUE);
				coord_trans(line.endPt,pDatumPlate[i]->ucs,TRUE);
				project_point(base_pos,line.startPt,pDatumPlate[i]->ucs.axis_z);
				int ret=Int3dll(line.startPt,line.endPt,base_pos-axis_x[i]*10000,base_pos+axis_x[i]*10000,inter_pos);
				if(ret>0)
				{
					double dist=DISTANCE(inter_pos,ucs.origin);
					if(bCalNByFirstPlate&&dist>(r+3))
					{	//仅选中一块钢板且钢板为贯通板时根据该钢板计算N值 wht 11-05-20
						f3dPoint verfiy_vec=inter_pos-ucs.origin;
						if(verfiy_vec*axis_x[i]>0)
							para.W=dist-r;
						else
							para.N=dist-r;
					}
					else if(dist>(r+3))	//3为误差值
					{
						if(i==0)
							para.W=dist-r;
						else 
							para.N=dist-r;
						break;
					}
				}
				pPreVertex=pVertex;
			}
		}
	}
	return TRUE;
}
//初始化环向板板参数
void CDesignCircularPlateDlg::InitCirPlatePara()
{
	if(!m_bNewCirPlate)
		return;
	m_cMaterial=(m_pDatumTube!=NULL)?m_pDatumTube->cMaterial:m_pDatumSphere->cMaterial;
	m_sSeg=(m_pDatumTube!=NULL)?m_pDatumTube->iSeg.ToString():m_pDatumSphere->iSeg.ToString();
	InitProfilePara();
	if(m_pCirPlate==NULL)
		m_pCirPlate=CreateCircularPlate();
}
void CDesignCircularPlateDlg::InitProfilePara()	//初始化钢板外形参数
{
	if(m_pDatumTube)
	{	//存在基准钢管且未设置环向板法线方向时设置法线方向
		para.R=m_pDatumTube->GetDiameter()*0.5+60;
		design_axis_z.UpdateVector(console.GetActiveModel());
		f3dPoint axis_z=design_axis_z.vector;
		if(axis_z.IsZero())
		{	//法线方向初始参数
			design_axis_z.norm_style=5;
			design_axis_z.hVicePart=m_pDatumTube->handle;
			design_axis_z.direction=0;
			design_axis_z.UpdateVector(console.GetActiveModel());
		}
	}
	//
	if(m_iPlateType==0)
	{	//普通环向板
		para.B=120;
		para.D=80;
	}	
	else if(m_iPlateType==1)
	{	//封板
		para.A = 0.0;
		para.H = para.h = 30.0;
		para.W = 60.0;
		para.N = 20.0;
		//para.S = 0;	外部可能已指定，此处无需初始化 wht 11-06-07
		para.nCirCoverPlateType=1;	//一个基准钢板默认为简化封板
		para.B=90;
		if(m_pRelaPart2&&m_pRelaPart2->GetClassTypeId()==CLS_PLATE)
		{
			para.nCirCoverPlateType=0;
			para.B=180;
		}	
	}
	else if(m_iPlateType==5)
	{	//2WA
		para.B=45;
		para.W=100;
		para.H=30;
	}
	else if(m_iPlateType==6&&m_pDatumTube&&m_pRelaPart1)
	{	//初始化遮水板参数
		if(m_pDatumTube->pStart==m_pDatumNode)
			water.S=m_pDatumTube->startOdd();
		else if(m_pDatumTube->pEnd==m_pDatumNode)
			water.S=m_pDatumTube->endOdd();
		if(m_pRelaPart1&&m_pRelaPart1->IsPlate())
			water.S+=m_pRelaPart1->GetThick()*0.5;
		//
		water.K=10;
		water.W=m_pDatumTube->GetDiameter();
		water.H=0.5*thick;
		water.A=60;
		if(m_pRelaPart1&&m_pRelaPart1->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)m_pRelaPart1;
			if(pParamPlate->GetLsCount()>0)
				water.A=360.0/pParamPlate->GetLsCount();
			//初始化遮水板外圆直径 wht 11-05-18
			if(pParamPlate->m_iParamType==TYPE_FL||
				pParamPlate->m_iParamType==TYPE_FLG||
				pParamPlate->m_iParamType==TYPE_FLR)
				pParamPlate->GetDesignItemValue('W',&water.W);
			else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
			{
				long hDatumTube=0;
				double V=0,R=0,D=0;
				pParamPlate->GetDesignItemValue('C',&hDatumTube);
				pParamPlate->GetDesignItemValue('V',&V);
				pParamPlate->GetDesignItemValue('R',&R);
				pParamPlate->GetDesignItemValue('D',&D);
				CLDSLineTube *pDatumTube=(CLDSLineTube*)console.FromPartHandle(hDatumTube,CLS_LINETUBE);
				if(pDatumTube)
				{
					if(R>0)	//异型法兰或其余跨越环向板中指定了外圆弧半径绝对值
						water.W=2*R;
					else if(D>0)
						water.W=(pDatumTube->GetDiameter()*0.5+D+V)*2;
				}
			}
			
		}
		water.L=0.5*water.W;
		//初始化Z轴方向
		design_axis_z.norm_style=5;
		design_axis_z.hVicePart=m_pDatumTube->handle;
		design_axis_z.direction=0;		//始->终
		if(m_pDatumNode==m_pDatumTube->pStart)
			design_axis_z.direction=1;	//终->始
	}
	else 
	{	//环形肋板 自动计算W,N长度
		AutoCalWN();
		//para.Y=-1;	//默认在钢板内侧生成肋板
		if(m_iPlateType==2)
		{	//2.肋板3WL 
			if(para.nDHRelativeType==2)
			{	//启用H参数 
				para.D=0;
				para.H=para.h = 30;
			}
			else if(para.nDHRelativeType==1)
			{
				para.H=0;
				para.D=100;
			}
			else 
			{
				para.H=para.h=30;
				para.D=100;
			}
		}
		else if(m_iPlateType==3)
		{	//3.肋板3WA
			if(para.nDHRelativeType==0)
				para.N=para.D=para.W;	//不启用DH
			else	
				para.D=80;				//启用DH
		}
		else if(m_iPlateType==4)
		{	//4.肋板2WL,可通过指定自由边距或扇形角两种方式定义外形范围
			para.H=30;
			if(para.D==0)
			{
				if(para.B<=0)
					para.B=30;
			}
			else
				para.B=0;
		}
	}
}

void CDesignCircularPlateDlg::OnPartsDb()
{	//零件库
	PARAM_PLATE_TYPE eParamType=TYPE_CIRRIBPLATE;	//当前构件类型
	if(m_iPlateType==1)
		eParamType=TYPE_CIRCOVERPLATE;		//环向封板
	else if(m_iPlateType==6)
		eParamType=TYPE_WATERSHIELDEDPLATE;	//遮水板
	else if((m_iPlateType==2&&para.nDHRelativeType==0)
		||(m_iPlateType==3&&para.nDHRelativeType))
		eParamType=TYPE_TUBERIBPLATE;		//跨越连板
	CPartsDbDlg partsDlg;
	partsDlg.m_bRefreshDlgCtrl = TRUE;
	if(eParamType==TYPE_CIRCOVERPLATE)
		partsDlg.m_bIncParamCirLid=TRUE;
	else if(eParamType==TYPE_CIRRIBPLATE||eParamType==TYPE_TUBERIBPLATE)
		partsDlg.m_bIncParamCirRib=TRUE;
	else if(eParamType==TYPE_WATERSHIELDEDPLATE)
		partsDlg.m_bIncParamWaterPlate=TRUE;
	if(partsDlg.DoModal()==IDOK)
	{
		CLDSParamPlate *pSrcParamPlate=(CLDSParamPlate*)partsDlg.m_pSelectPart;
		if(!pSrcParamPlate)
			return;
		if(pSrcParamPlate->m_iParamType!=eParamType)
			return;
		CXhChar100 temp_str;
		long idProp=0;
		m_sSeg=pSrcParamPlate->iSeg.ToString();
		m_sPartNo.Copy(pSrcParamPlate->GetPartNo());
		m_cMaterial=pSrcParamPlate->cMaterial;
		thick=pSrcParamPlate->thick;
		idProp=GetPropID("iSeg");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("material");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("thick");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		if(eParamType==TYPE_CIRCOVERPLATE)
		{
			pSrcParamPlate->GetDesignItemValue('A',&para.A);
			pSrcParamPlate->GetDesignItemValue('B',&para.B);
			pSrcParamPlate->GetDesignItemValue('D',&para.H);
			pSrcParamPlate->GetDesignItemValue('W',&para.W);
			CLDSLineTube *pSrcTube=(CLDSLineTube*)console.FromPartHandle(pSrcParamPlate->handle,CLS_LINETUBE);
			if(pSrcTube)
			{	//加工间隙
				double R=0;
				pSrcParamPlate->GetDesignItemValue('R',&R);
				para.V=R-pSrcTube->GetDiameter()*0.5;
			}
			//
			idProp=GetPropID("para.A");
			if(GetPropValueStr(idProp,temp_str)>0)
				m_propList.SetItemPropValue(idProp,temp_str);
			idProp=GetPropID("para.B");
			if(GetPropValueStr(idProp,temp_str)>0)
				m_xSubPropList.SetItemPropValue(idProp,temp_str);
			idProp=GetPropID("para.H");
			if(GetPropValueStr(idProp,temp_str)>0)
				m_xSubPropList.SetItemPropValue(idProp,temp_str);
			idProp=GetPropID("para.W");
			if(GetPropValueStr(idProp,temp_str)>0)
				m_xSubPropList.SetItemPropValue(idProp,temp_str);
			idProp=GetPropID("para.V");
			if(GetPropValueStr(idProp,temp_str)>0)
				m_xSubPropList.SetItemPropValue(idProp,temp_str);
		}
		else if(eParamType==TYPE_WATERSHIELDEDPLATE)
		{
			pSrcParamPlate->GetDesignItemValue('A',&water.A);
			pSrcParamPlate->GetDesignItemValue('H',&water.H);
			pSrcParamPlate->GetDesignItemValue('K',&water.K);
			pSrcParamPlate->GetDesignItemValue('L',&water.L);
			pSrcParamPlate->GetDesignItemValue('O',&water.O);
			pSrcParamPlate->GetDesignItemValue('W',&water.W);
			//
			idProp=GetPropID("water.A");
			if(GetPropValueStr(idProp,temp_str)>0)
				m_propList.SetItemPropValue(idProp,temp_str);
			idProp=GetPropID("water.H");
			if(GetPropValueStr(idProp,temp_str)>0)
				m_xSubPropList.SetItemPropValue(idProp,temp_str);
			idProp=GetPropID("water.K");
			if(GetPropValueStr(idProp,temp_str)>0)
				m_xSubPropList.SetItemPropValue(idProp,temp_str);
			idProp=GetPropID("water.K");
			if(GetPropValueStr(idProp,temp_str)>0)
				m_xSubPropList.SetItemPropValue(idProp,temp_str);
			idProp=GetPropID("water.O");
			if(GetPropValueStr(idProp,temp_str)>0)
				m_xSubPropList.SetItemPropValue(idProp,temp_str);
			idProp=GetPropID("water.W");
			if(GetPropValueStr(idProp,temp_str)>0)
				m_xSubPropList.SetItemPropValue(idProp,temp_str);
		}
		else
		{
			long hPart1=0,hPart2=0;
			pSrcParamPlate->GetDesignItemValue('E',&hPart1);
			pSrcParamPlate->GetDesignItemValue('F',&hPart2);
			CLDSPart *pRealPart1=console.FromPartHandle(hPart1);
			CLDSPart *pRealPart2=console.FromPartHandle(hPart2);
			if(m_iPlateType==0&&pRealPart1==NULL&&pRealPart2==NULL)
			{
				pSrcParamPlate->GetDesignItemValue('V',&para.V);
				idProp=GetPropID("para.V");
				if(GetPropValueStr(idProp,temp_str)>0)
					m_xSubPropList.SetItemPropValue(idProp,temp_str);
				if(ConnVecList.GetCount(TRUE)>0&&pSrcParamPlate->ConnDirectList.GetNodeNum()>1)
				{
					pSrcParamPlate->GetDesignItemValue('D',&para.D);
					idProp=GetPropID("para.D");
					if(GetPropValueStr(idProp,temp_str)>0)
						m_xSubPropList.SetItemPropValue(idProp,temp_str);
				}
				else 
				{
					pSrcParamPlate->GetDesignItemValue('A',&para.A);
					pSrcParamPlate->GetDesignItemValue('B',&para.B);
					pSrcParamPlate->GetDesignItemValue('R',&para.R);
					idProp=GetPropID("para.A");
					if(GetPropValueStr(idProp,temp_str)>0)
						m_xSubPropList.SetItemPropValue(idProp,temp_str);
					idProp=GetPropID("para.B");
					if(GetPropValueStr(idProp,temp_str)>0)
						m_xSubPropList.SetItemPropValue(idProp,temp_str);
					idProp=GetPropID("para.R");
					if(GetPropValueStr(idProp,temp_str)>0)
						m_xSubPropList.SetItemPropValue(idProp,temp_str);
				}
			}
			else if(m_iPlateType>1&&m_iPlateType<6)
			{
				pSrcParamPlate->GetDesignItemValue('V',&para.V);
				pSrcParamPlate->GetDesignItemValue('S',&para.S);
				if(fabs(para.S)<=EPS)
					pSrcParamPlate->GetDesignItemValue('T',&para.S);
				pSrcParamPlate->GetDesignItemValue('W',&para.W);
				pSrcParamPlate->GetDesignItemValue('N',&para.N);
				pSrcParamPlate->GetDesignItemValue('D',&para.D);
				pSrcParamPlate->GetDesignItemValue('H',&para.H);
				pSrcParamPlate->GetDesignItemValue('G',&para.G);
				pSrcParamPlate->GetDesignItemValue('B',&para.B);
				pSrcParamPlate->GetDesignItemValue('r',&para.r);
				//
				idProp=GetPropID("para.V");
				if(GetPropValueStr(idProp,temp_str)>0)
					m_xSubPropList.SetItemPropValue(idProp,temp_str);
				idProp=GetPropID("para.S");
				if(GetPropValueStr(idProp,temp_str)>0)
					m_xSubPropList.SetItemPropValue(idProp,temp_str);
				idProp=GetPropID("para.T");
				if(GetPropValueStr(idProp,temp_str)>0)
					m_xSubPropList.SetItemPropValue(idProp,temp_str);
				idProp=GetPropID("para.W");
				if(GetPropValueStr(idProp,temp_str)>0)
					m_xSubPropList.SetItemPropValue(idProp,temp_str);
				idProp=GetPropID("para.N");
				if(GetPropValueStr(idProp,temp_str)>0)
					m_xSubPropList.SetItemPropValue(idProp,temp_str);
				idProp=GetPropID("para.D");
				if(GetPropValueStr(idProp,temp_str)>0)
					m_xSubPropList.SetItemPropValue(idProp,temp_str);
				idProp=GetPropID("para.H");
				if(GetPropValueStr(idProp,temp_str)>0)
					m_xSubPropList.SetItemPropValue(idProp,temp_str);
				idProp=GetPropID("para.B");
				if(GetPropValueStr(idProp,temp_str)>0)
					m_xSubPropList.SetItemPropValue(idProp,temp_str);
				idProp=GetPropID("para.r");
				if(GetPropValueStr(idProp,temp_str)>0)
					m_xSubPropList.SetItemPropValue(idProp,temp_str);
				idProp=GetPropID("innercir_type");
				if(GetPropValueStr(idProp,temp_str)>0)
					m_xSubPropList.SetItemPropValue(idProp,temp_str);
			}
		}
	}
}
/*	各字符含义
	A:α,环向定位角(定位参数)
	B:β,环向扇形角(外形参数)
	C:定位基准钢管(球体)句柄(定位参数)
	D:基准钢管外壁到自由边距离，简称自由边距
	E:第一块关联基准钢板句柄
	F:第二块关联基准钢板句柄，或焊接关联钢管句柄(关联钢管是指与环向板有焊缝的钢管，不含上面的插板螺栓连接钢管，因此不可能出现两根)
	G:自由边类型(>0直线，<=0圆弧)
	H:台口高度
	I:第一根连接方向钢管句柄(指明有此钢管方向的连接意图，即插板螺栓连接或无连接制弯)
	J:第二根连接方向钢管句柄(指明有此钢管方向的连接意图，即插板螺栓连接或无连接制弯)
	P:定位基准节点句柄
	R:环向板外圆弧半径
	r:环向板内圆半径(异形法兰或360的环向板使用 r<0根据钢管生成内圆 r=0无内圆 r>0按指定半径生成内圆) wht 11-05-27
	S:自定位基准节点向定位原点的轴向偏移距离(S<0上封板;S>0下封板)
	T:自定位基准节点向定位原点的轴向偏移距离在环向板法线上的投影距离
	V:加工间隙(基准钢管外壁到环向板内环的预留间隙)
	Y:半圆型环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)
	W:第一块关联钢板的焊缝长
	N:第二块关联钢板或钢管的焊缝长
*/
//钢板生成函数
BOOL CreateProcessHole(CSuperSmartPtr<CLDSPart> pHoriPlate,CSuperSmartPtr<CLDSPart>pVertPlate,
					   BOOL bDispDlg,BOOL bIncUpHole=TRUE,BOOL bIncDownHole=TRUE);
CLDSParamPlate *CDesignCircularPlateDlg::CreateCircularPlate()
{
	if(m_iPlateType==6)	//单独设计生成遮水板 wht 11-05-11
		return CreateWaterShieldedPlate();
	Ta.BeginUndoListen();
	CLDSParamPlate *pCirPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pCirPlate->m_hPartWeldParent=m_pDatumTube->handle;
	pCirPlate->cfgword=m_pDatumTube->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
	pCirPlate->SetLayer(m_pDatumTube->layer());	//调整钢管肋板的图层名
	pCirPlate->thick=thick;
	pCirPlate->iSeg=SEGI(m_sSeg.GetBuffer());
	pCirPlate->cMaterial = m_cMaterial;
	//_snprintf(pCirPlate->sPartNo,MAX_CHAR_GUIGE_16,"%s",(char*)m_sPartNo);
	pCirPlate->SetPartNo(m_sPartNo);
	//引入螺栓
	pCirPlate->EmptyLsRef();
	CLsRef *pLsRef=NULL;
	for(pLsRef=LsRefList.GetFirst();pLsRef;pLsRef=LsRefList.GetNext())
	{
		CLDSBolt *pBolt=pLsRef->GetLsPtr();
		//调整螺栓通厚 
		pBolt->AddL0Thick(pCirPlate->handle,TRUE,TRUE);
		//调整槽型插板上朝内的螺栓的法线偏移量 wht 11-05-27
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pBolt->des_base_pos.hPart,CLS_PARAMPLATE);
		if(pParamPlate&&pParamPlate->m_iParamType==TYPE_ROLLEND&&pBolt->des_work_norm.direction==1)
			pBolt->des_base_pos.norm_offset.AddThick(-pCirPlate->handle,TRUE,TRUE);
		pCirPlate->AppendLsRef(*pLsRef);
		pBolt->correct_pos();
		pBolt->CalGuigeAuto();
		pBolt->SetModified();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	pCirPlate->ConnDirectList.Empty();
	pCirPlate->workNorm=design_axis_z;	//参数化工作法线赋值
	//关联方向
	BOOL bHasSerial1=FALSE;	//是否存在serial==1的连接方向
	CONNECT_DIRECTION *pVec=NULL;
	BOOL bNeedBend=ConnVecList.IsNeedBend();
	for(pVec=ConnVecList.GetFirst();pVec;pVec=ConnVecList.GetNext())
	{	
		if(pVec->serial==1)
			bHasSerial1=TRUE;
		CONNECT_DIRECTION *pNewVec=pCirPlate->ConnDirectList.append();
		pNewVec->serial=pVec->serial;
		pNewVec->style=pVec->style;
		pNewVec->end_space=pVec->end_space;
		pNewVec->high_space=pVec->high_space;
		pNewVec->direct=pVec->direct;
	}
	if(!bHasSerial1)
	{	//不存在serial==1的连接方向,保存基准面法线
		CONNECT_DIRECTION *pNewVec=pCirPlate->ConnDirectList.append();
		pNewVec->serial=1;
		pNewVec->style=1;
		pNewVec->direct=design_axis_z;
	}
	if(m_iPlateType==0)
	{	//异型法兰
		pCirPlate->m_iParamType=TYPE_CIRRIBPLATE;	//异形法兰
#ifdef AFX_TARG_ENU_ENGLISH
		pCirPlate->SetDesignItemValue('C',m_pDatumTube->handle,"Locating Datum Tube",PARAM_POS,1);
		pCirPlate->SetDesignItemValue('P',m_pDatumNode->handle,"Locating Datum Node",PARAM_POS,1);
		pCirPlate->SetDesignItemValue('V',para.V,"Maching Gap ",PARAM_SHAPE);
		if(ConnVecList.GetCount(TRUE)>0)
			pCirPlate->SetDesignItemValue('D',para.D,"Special-shape FL Free Margin",PARAM_SHAPE);
		else 
		{
			pCirPlate->SetDesignItemValue('A',para.A,"Sector Start Angle");
			pCirPlate->SetDesignItemValue('B',para.B,"Section Angle",PARAM_SHAPE);
			pCirPlate->SetDesignItemValue('R',para.R,"Circular Plate Outer Arc Radius",PARAM_SHAPE);
		}
		if(para.nOffsetType==0)	//用para.S保存偏移量，根据偏移量类型设置偏移参数 wht 11-05-13
		{
			pCirPlate->SetDesignItemValue('S',para.S,"Offset Along Datum Tube.",PARAM_POS);
			pCirPlate->SetDesignItemValue('T',0,"Offset Along Circular Plate's Normal.",PARAM_POS);
		}
		else 
		{
			pCirPlate->SetDesignItemValue('S',0,"Offset Along Datum Tube.",PARAM_POS);
			pCirPlate->SetDesignItemValue('T',para.S,"Offset Along Circular Plate's Normal.",PARAM_POS);
		}
		pCirPlate->SetDesignItemValue('r',para.r,"Inner Circle Radius:r<0 Create Inner Circle By Tube r=0 No Inner Circle r>0 Create Inner Circle By Specified Radius.",PARAM_SHAPE);
#else 
		pCirPlate->SetDesignItemValue('C',m_pDatumTube->handle,"定位基准钢管",PARAM_POS,1);
		pCirPlate->SetDesignItemValue('P',m_pDatumNode->handle,"定位基准节点",PARAM_POS,1);
		pCirPlate->SetDesignItemValue('V',para.V,"加工间隙",PARAM_SHAPE);
		if(ConnVecList.GetCount(TRUE)>0)
			pCirPlate->SetDesignItemValue('D',para.D,"异形法兰自由边距",PARAM_SHAPE);
		else 
		{
			pCirPlate->SetDesignItemValue('A',para.A,"扇形起始角度");
			pCirPlate->SetDesignItemValue('B',para.B,"扇形夹角",PARAM_SHAPE);
			pCirPlate->SetDesignItemValue('R',para.R,"环向板外圆弧半径",PARAM_SHAPE);
		}
		if(para.nOffsetType==0)	//用para.S保存偏移量，根据偏移量类型设置偏移参数 wht 11-05-13
		{
			pCirPlate->SetDesignItemValue('S',para.S,"沿基准钢管延伸方向的偏移距离。",PARAM_POS);
			pCirPlate->SetDesignItemValue('T',0,"沿环向板法线方向的偏移距离。",PARAM_POS);
		}
		else 
		{
			pCirPlate->SetDesignItemValue('S',0,"沿基准钢管延伸方向的偏移距离。",PARAM_POS);
			pCirPlate->SetDesignItemValue('T',para.S,"沿环向板法线方向的偏移距离。",PARAM_POS);
		}
		pCirPlate->SetDesignItemValue('r',para.r,"内圆半径:r<0根据钢管生成内圆 r=0无内圆 r>0按指定半径生成内圆。",PARAM_SHAPE);
#endif
		design_axis_z.UpdateVector(console.GetActiveModel());
		pCirPlate->ucs.axis_z=design_axis_z.vector;	//Z轴方向
		pCirPlate->DesignPlate();
		pCirPlate->SetModified();
		pCirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pCirPlate->GetSolidPartObject());
		//布置螺栓	//wht 11-05-16
		LayoutBolt(pCirPlate);
	}
	else if(m_iPlateType==1)
	{	//环向封板
		pCirPlate->m_iParamType=TYPE_CIRCOVERPLATE;		//环向封板
#ifdef AFX_TARG_ENU_ENGLISH
		pCirPlate->SetDesignItemValue('A',para.A,"Sealing Plate Rotate Angle");
		pCirPlate->SetDesignItemValue('B',para.B,"Section Angle",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('C',m_pDatumTube->handle,"Locating Datum Tube",PARAM_POS,1);
		pCirPlate->SetDesignItemValue('E',m_pDatumNode->handle,"Locating Datum Node's",PARAM_POS,1);
		pCirPlate->SetDesignItemValue('F',m_pRelaPart1->handle,"First Datum Plate",PARAM_POS,1);
		if(para.nCirCoverPlateType!=0)
		{
			pCirPlate->SetDesignItemValue('G',0,"Second Datum Plate",PARAM_POS,1);
			pCirPlate->SetDesignItemValue('Y',para.nCirCoverPlateType,"Common Sealing Plate;Main Section α Y+;Main Section α Y-",PARAM_SHAPE,1);
			pCirPlate->SetDesignItemValue('N',para.N,"Parameter N",PARAM_SHAPE);
		}
#else 
		pCirPlate->SetDesignItemValue('A',para.A,"封板旋转角度");
		pCirPlate->SetDesignItemValue('B',para.B,"扇形夹角",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('C',m_pDatumTube->handle,"定位基准基准钢管",PARAM_POS,1);
		pCirPlate->SetDesignItemValue('E',m_pDatumNode->handle,"定位基准节点",PARAM_POS,1);
		pCirPlate->SetDesignItemValue('F',m_pRelaPart1->handle,"第一块基准钢板",PARAM_POS,1);
		if(para.nCirCoverPlateType!=0)
		{
			pCirPlate->SetDesignItemValue('G',0,"第二块基准钢板",PARAM_POS,1);
			pCirPlate->SetDesignItemValue('Y',para.nCirCoverPlateType,"普通封板;主扇形角α在Y轴正方侧的简化封板;主扇形角α在Y轴负方向侧的简化封板",PARAM_SHAPE,1);
			pCirPlate->SetDesignItemValue('N',para.N,"参数N",PARAM_SHAPE);
		}
#endif
		else 
		{	//普通环向封板
			if(m_pRelaPart2&&m_pRelaPart2->GetClassTypeId()==CLS_PLATE)
#ifdef AFX_TARG_ENU_ENGLISH
				pCirPlate->SetDesignItemValue('G',m_pRelaPart2->handle,"Second Datum Plate",PARAM_POS,1);
#else 
				pCirPlate->SetDesignItemValue('G',m_pRelaPart2->handle,"第二块基准钢板",PARAM_POS,1);
#endif
		}
#ifdef AFX_TARG_ENU_ENGLISH
		pCirPlate->SetDesignItemValue('D',para.H,"Step Hatch Height H",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('W',para.W,"Circular Band Width W",PARAM_SHAPE);
		if(para.S>=0)
			pCirPlate->SetDesignItemValue('S',-1,"Design Up Sealing Plate");
		else 
			pCirPlate->SetDesignItemValue('S',1,"Design Down Sealing Plate");
		pCirPlate->SetDesignItemValue('R',m_pDatumTube->GetDiameter()*0.5+para.V,"Inner Arc's Radius of Circular Plate (=Tube Radius +Maching Gap)",PARAM_SHAPE);
#else 
		pCirPlate->SetDesignItemValue('D',para.H,"台口高度H",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue(KEY2C("R1"),para.R1,"半圆孔半径2",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue(KEY2C("R2"),para.R2,"半圆孔半径2",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('W',para.W,"环形带宽W",PARAM_SHAPE);
		if(para.S>=0)
			pCirPlate->SetDesignItemValue('S',-1,"设计上封板");
		else 
			pCirPlate->SetDesignItemValue('S',1,"设计下封板");
		pCirPlate->SetDesignItemValue('R',m_pDatumTube->GetDiameter()*0.5+para.V,"环向板内圆弧半径(=钢管半径+加工间隙)",PARAM_SHAPE);
#endif
		pCirPlate->DesignPlate();
		pCirPlate->SetModified();
		pCirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pCirPlate->GetSolidPartObject());
		//
		CLDSParamPlate *pSimpleCirPlate3=NULL,*pSimpleCirPlate4=NULL;	//记录另外两块简化封板
		if(para.nCirCoverPlateType==0&&para.bCreateMirPlate)
		{	//生成另一侧封板
			CLDSParamPlate *pMirCoverPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pMirCoverPlate->CopyProperty(pCirPlate);
			pMirCoverPlate->SetLayer(m_pDatumTube->layer()); //调整图层名
			pMirCoverPlate->cfgword=m_pDatumTube->cfgword;
#ifdef AFX_TARG_ENU_ENGLISH
			pMirCoverPlate->SetDesignItemValue('A',para.A+180,"Sealing Plate Rotate Angle");
#else 
			pMirCoverPlate->SetDesignItemValue('A',para.A+180,"封板旋转角度");
#endif
			pMirCoverPlate->m_hPartWeldParent = m_pDatumTube->handle;
			pMirCoverPlate->DesignPlate();
			pMirCoverPlate->SetModified();
			pMirCoverPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pMirCoverPlate->GetSolidPartObject());
		}
		else if(para.nCirCoverPlateType==2&&m_pRelaPart2!=NULL)
		{	//生成第二块钢板上的简化环向封板 wht 12-03-27
			pSimpleCirPlate3=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pSimpleCirPlate3->CopyProperty(pCirPlate);
			pSimpleCirPlate3->SetLayer(m_pDatumTube->layer()); //调整图层名
			pSimpleCirPlate3->cfgword=m_pDatumTube->cfgword;
#ifdef AFX_TARG_ENU_ENGLISH
			pSimpleCirPlate3->SetDesignItemValue('F',m_pRelaPart2->handle,"First Datum Plate",PARAM_POS,1);
			pSimpleCirPlate3->SetDesignItemValue('Y',-1*para.nCirCoverPlateType,"Common Sealing Plate;Main Section α Y+;Main Section α Y-",PARAM_SHAPE,1);
#else 
			pSimpleCirPlate3->SetDesignItemValue('F',m_pRelaPart2->handle,"第一块基准钢板",PARAM_POS,1);
			pSimpleCirPlate3->SetDesignItemValue('Y',-1*para.nCirCoverPlateType,"普通封板;主扇形角α在Y轴正方侧的简化封板;主扇形角α在Y轴负方向侧的简化封板",PARAM_SHAPE,1);
#endif
			pSimpleCirPlate3->m_hPartWeldParent = m_pDatumTube->handle;
			pSimpleCirPlate3->DesignPlate();
			pSimpleCirPlate3->SetModified();
			pSimpleCirPlate3->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pSimpleCirPlate3->GetSolidPartObject());
		}
		CLDSParamPlate *pCoverPlate1=NULL;
		if(para.S==0)
		{	//上下封一起设计时,在此处设计下封板
			pCoverPlate1=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pCoverPlate1->CopyProperty(pCirPlate);
			pCoverPlate1->CopyModuleInstanceInfo(m_pDatumTube); //调整图层名
#ifdef AFX_TARG_ENU_ENGLISH
			pCoverPlate1->SetDesignItemValue('S',(double)1,"Design Down Sealing Plate");
#else 
			pCoverPlate1->SetDesignItemValue('S',(double)1,"设计下封板");
#endif
			pCoverPlate1->m_hPartWeldParent = m_pDatumTube->handle;
			pCoverPlate1->DesignPlate();
			pCoverPlate1->SetModified();
			pCoverPlate1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pCoverPlate1->GetSolidPartObject());
			if(para.nCirCoverPlateType==0&&para.bCreateMirPlate)
			{	//生成另一侧封板
				CLDSParamPlate *pMirCoverPlate1=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
				pMirCoverPlate1->CopyProperty(pCoverPlate1);
				pMirCoverPlate1->CopyModuleInstanceInfo(m_pDatumTube); //调整图层名
#ifdef AFX_TARG_ENU_ENGLISH
				pMirCoverPlate1->SetDesignItemValue('A',para.A+180,"Sealing Plate Rotate Angle");
#else 
				pMirCoverPlate1->SetDesignItemValue('A',para.A+180,"封板旋转角度");
#endif
				pMirCoverPlate1->m_hPartWeldParent = m_pDatumTube->handle;
				pMirCoverPlate1->DesignPlate();
				pMirCoverPlate1->SetModified();
				pMirCoverPlate1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pMirCoverPlate1->GetSolidPartObject());
			}
			else if(para.nCirCoverPlateType==2&&m_pRelaPart2!=NULL)
			{	//生成第二块钢板上的环向封板 wht 12-03-27
				pSimpleCirPlate4=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
				pSimpleCirPlate4->CopyProperty(pCoverPlate1);
				pSimpleCirPlate4->CopyModuleInstanceInfo(m_pDatumTube); //调整图层名
#ifdef AFX_TARG_ENU_ENGLISH
				pSimpleCirPlate4->SetDesignItemValue('F',m_pRelaPart2->handle,"First Datum Plate",PARAM_POS,1);
				pSimpleCirPlate4->SetDesignItemValue('Y',-1*para.nCirCoverPlateType,"Common Sealing Plate;Main Section α Y+;Main Section α Y-",PARAM_SHAPE,1);
#else 
				pSimpleCirPlate4->SetDesignItemValue('F',m_pRelaPart2->handle,"第一块基准钢板",PARAM_POS,1);
				pSimpleCirPlate4->SetDesignItemValue('Y',-1*para.nCirCoverPlateType,"普通封板;主扇形角α在Y轴正方侧的简化封板;主扇形角α在Y轴负方向侧的简化封板",PARAM_SHAPE,1);
#endif
				pSimpleCirPlate4->m_hPartWeldParent = m_pDatumTube->handle;
				pSimpleCirPlate4->DesignPlate();
				pSimpleCirPlate4->SetModified();
				pSimpleCirPlate4->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pSimpleCirPlate4->GetSolidPartObject());
			}
		}
		BOOL bIncUpHole=TRUE,bIncDownHole=TRUE;
		if(para.nCirCoverPlateType!=0)
		{
			if(para.nCirCoverPlateType>0)
				bIncUpHole=(pCirPlate->ucs.axis_y*m_pRelaPart1->ucs.axis_z>0);
			else 
				bIncUpHole=(pCirPlate->ucs.axis_y*m_pRelaPart1->ucs.axis_z<0);
			bIncDownHole=!bIncUpHole;
		}
		if(para.bWeldSemicircleMode)
			return pCirPlate;
		BOOL bCreatHole=CreateProcessHole(pCirPlate,m_pRelaPart1,m_bDisplayDlg,bIncUpHole,bIncDownHole);
		if(bCreatHole)	//创建工艺孔
			CreateProcessHole(pCoverPlate1,m_pRelaPart1,FALSE,bIncUpHole,bIncDownHole);
		if(m_pRelaPart2&&m_pRelaPart1!=m_pRelaPart2)
		{
			bIncDownHole=bIncUpHole=TRUE;
			if(para.nCirCoverPlateType==0&&bCreatHole)
			{	//创建工艺孔
				CreateProcessHole(pCirPlate,m_pRelaPart2,FALSE,bIncUpHole,bIncDownHole);
				CreateProcessHole(pCoverPlate1,m_pRelaPart2,FALSE,bIncUpHole,bIncDownHole);
			}
			else if(para.nCirCoverPlateType==2&&bCreatHole&&pSimpleCirPlate3)
			{	//生成第二块钢板上简化环向封板上的工艺孔
				if(-1*para.nCirCoverPlateType>0)
					bIncUpHole=(pSimpleCirPlate3->ucs.axis_y*m_pRelaPart2->ucs.axis_z>0);
				else 
					bIncUpHole=(pSimpleCirPlate3->ucs.axis_y*m_pRelaPart2->ucs.axis_z<0);
				bIncDownHole=!bIncUpHole;
				CreateProcessHole(pSimpleCirPlate3,m_pRelaPart2,FALSE,bIncUpHole,bIncDownHole);
				if(pSimpleCirPlate4)
					CreateProcessHole(pSimpleCirPlate4,m_pRelaPart2,FALSE,bIncUpHole,bIncDownHole);
			}
		}
	}
	else 
	{	//环向肋板或跨越连板
		if(m_iPlateType==3&&para.nDHRelativeType==0)
		{
			para.W=para.N=para.D=para.W=para.H=para.h=0;
#ifdef AFX_TARG_ENU_ENGLISH
			pCirPlate->SetDesignItemValue('R',para.R,"The outer arc radius of circular plate.",PARAM_POS);
#else 
			pCirPlate->SetDesignItemValue('R',para.R,"环向板外圆弧半径。",PARAM_POS);
#endif
		}
		else if(m_iPlateType==5)
			para.N=para.D=0;
#ifdef AFX_TARG_ENU_ENGLISH
		pCirPlate->SetDesignItemValue('C',m_pDatumTube->handle,"Locating datum tube",PARAM_POS,1);
		pCirPlate->SetDesignItemValue('P',m_pDatumNode->handle,"Locating datum node's handle",PARAM_POS,1);
		pCirPlate->SetDesignItemValue('V',para.V,"Maching gap",PARAM_SHAPE);
		
		if((m_iPlateType==2&&para.nDHRelativeType==0)||(m_iPlateType==3&&para.nDHRelativeType))
			pCirPlate->m_iParamType=TYPE_TUBERIBPLATE;		//跨越连板
		else
			pCirPlate->m_iParamType=TYPE_CIRRIBPLATE;		//环向肋板
		pCirPlate->SetDesignItemValue('E',m_pRelaPart1->handle,"First datum plate",PARAM_POS,1);
		if(m_pRelaPart2&&m_pRelaPart2->IsPlate())
			pCirPlate->SetDesignItemValue('F',m_pRelaPart2->handle,"The second relative plate",PARAM_POS,1);
		else if(m_pRelaPart2&&m_pRelaPart2->IsTube())
			pCirPlate->SetDesignItemValue('F',m_pRelaPart2->handle,"The second relative part",PARAM_POS,1);
		else 
			pCirPlate->SetDesignItemValue('F',0,"Second datum plate",PARAM_POS,1);
		if(para.nOffsetType==0)	//用para.S保存偏移量，根据偏移量类型设置偏移参数 wht 11-05-13
		{
			pCirPlate->SetDesignItemValue('S',para.S,"Offset along datum tube.",PARAM_POS);
			pCirPlate->SetDesignItemValue('T',0,"Offset along circular plate's normal.",PARAM_POS);
		}
		else 
		{
			pCirPlate->SetDesignItemValue('S',0,"Offset along datum tube.",PARAM_POS);
			pCirPlate->SetDesignItemValue('T',para.S,"Offset along circular plate's normal.",PARAM_POS);
		}
		//
		pCirPlate->SetDesignItemValue('r',para.r,"Inner circle radius:r<0 create inner circle by tube r=0 no inner circle r>0 create inner circle by specified radius.",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('Y',para.Y,"Half circular sealing plate position(>0 plate outside;=0 default;<0 plate inside)",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('W',para.W,"First relative plate's weld length",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('N',para.N,"First relative plate's or tube's weld length",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('D',para.D,"The distance between datum tube's outer wall to free margin,it is free margin for short",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('H',para.H,"Step Hatch Height ",PARAM_SHAPE);
		if(m_iPlateType<4&&para.h!=para.H)
			pCirPlate->SetDesignItemValue('h',para.h,"N Side Step Hatch Height ",PARAM_SHAPE);
		if(m_iPlateType==5)
			pCirPlate->SetDesignItemValue('y',para.gama,"Chamfer Angle",PARAM_SHAPE);
		if(m_iPlateType==3||m_iPlateType==5||(m_iPlateType==2&&para.nDHRelativeType==0))
			pCirPlate->SetDesignItemValue('G',0,"Free Edge Type(>0 Line，<=0 Arc)");
		else 
			pCirPlate->SetDesignItemValue('G',1,"Free Edge Type(>0 Line，<=0 Arc)");
		if(para.B!=0)
			pCirPlate->SetDesignItemValue('B',para.B,"Section Angle");
#else 
		pCirPlate->SetDesignItemValue('C',m_pDatumTube->handle,"定位基准钢管",PARAM_POS,1);
		pCirPlate->SetDesignItemValue('P',m_pDatumNode->handle,"定位基准节点",PARAM_POS,1);
		pCirPlate->SetDesignItemValue('V',para.V,"加工间隙",PARAM_SHAPE);
		
		if((m_iPlateType==2&&para.nDHRelativeType==0)||(m_iPlateType==3&&para.nDHRelativeType))
			pCirPlate->m_iParamType=TYPE_TUBERIBPLATE;		//跨越连板
		else
			pCirPlate->m_iParamType=TYPE_CIRRIBPLATE;		//环向肋板
		pCirPlate->SetDesignItemValue('E',m_pRelaPart1->handle,"第一块基准钢板",PARAM_POS,1);
		if(m_pRelaPart2&&m_pRelaPart2->IsPlate())
			pCirPlate->SetDesignItemValue('F',m_pRelaPart2->handle,"第二块关联钢板",PARAM_POS,1);
		else if(m_pRelaPart2&&m_pRelaPart2->IsTube())
			pCirPlate->SetDesignItemValue('F',m_pRelaPart2->handle,"第二个关联构件",PARAM_POS,1);
		else 
			pCirPlate->SetDesignItemValue('F',0,"第二块基准钢板",PARAM_POS,1);
		if(para.nOffsetType==0)	//用para.S保存偏移量，根据偏移量类型设置偏移参数 wht 11-05-13
		{
			pCirPlate->SetDesignItemValue('S',para.S,"沿基准钢管延伸方向的偏移距离。",PARAM_POS);
			pCirPlate->SetDesignItemValue('T',0,"沿环向板法线方向的偏移距离。",PARAM_POS);
		}
		else 
		{
			pCirPlate->SetDesignItemValue('S',0,"沿基准钢管延伸方向的偏移距离。",PARAM_POS);
			pCirPlate->SetDesignItemValue('T',para.S,"沿环向板法线方向的偏移距离。",PARAM_POS);
		}
		//
		pCirPlate->SetDesignItemValue('r',para.r,"内圆半径:r<0根据钢管生成内圆 r=0无内圆 r>0按指定半径生成内圆。",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('Y',para.Y,"半圆型环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('W',para.W,"第一块关联钢板的焊缝长",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('N',para.N,"第二块关联钢板或钢管的焊缝长",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('D',para.D,"基准钢管外壁到自由边距离，简称自由边距",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('H',para.H,"台口高度",PARAM_SHAPE);
		if(m_iPlateType<4&&para.h!=para.H)
			pCirPlate->SetDesignItemValue('h',para.h,"N侧台口高度",PARAM_SHAPE);
		if(m_iPlateType==5)
			pCirPlate->SetDesignItemValue('y',para.gama,"倒角角度",PARAM_SHAPE);
		if(m_iPlateType==3||m_iPlateType==5||(m_iPlateType==2&&para.nDHRelativeType==0))
			pCirPlate->SetDesignItemValue('G',0,"自由边类型(>0直线，<=0圆弧)");
		else 
			pCirPlate->SetDesignItemValue('G',1,"自由边类型(>0直线，<=0圆弧)");
		if(para.B!=0)
			pCirPlate->SetDesignItemValue('B',para.B,"扇形角度");
#endif
		pCirPlate->DesignPlate();
		pCirPlate->SetModified();
		pCirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pCirPlate->GetSolidPartObject());
	}
	g_pSolidDraw->AddCS(pCirPlate->handle,pCirPlate->ucs);
	g_pSolidDraw->Draw();
	Ta.EndUndoListen();
	return pCirPlate;
}

CLDSParamPlate *CDesignCircularPlateDlg::CreateWaterShieldedPlate()
{
	CLDSNode *pBaseNode=m_pDatumNode;
	CLDSLineTube *pDatumTube=m_pDatumTube;
	CLDSPart *pPlate=m_pRelaPart1;
	if(pBaseNode==NULL||pDatumTube==NULL)	//||pPlate==NULL)
		return NULL;	//基准节点或基准钢管为NULL
	//
	UCS_STRU ucs;
	if(pBaseNode==pDatumTube->pStart)
		ucs.origin=pDatumTube->Start();
	else if(pBaseNode==pDatumTube->pEnd)
		ucs.origin=pDatumTube->End();
	else
		SnapPerp(&ucs.origin,pDatumTube->Start(),pDatumTube->End(),pBaseNode->Position(true));
	design_axis_z.UpdateVector(console.GetActiveModel());
	ucs.axis_z=design_axis_z.vector;
	normalize(ucs.axis_z);
	ucs.axis_x=pDatumTube->ucs.axis_x;
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_x);
	normalize(ucs.axis_y);
	//
	Ta.BeginUndoListen();
	CLDSParamPlate *pPlateAttachLaunder=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	//遮水板不需要焊接父构件
	//pPlateAttachLaunder->m_hPartWeldParent = pDatumTube->handle;
	pPlateAttachLaunder->CopyModuleInstanceInfo(pDatumTube);	//调整钢板配材号与基准构件或基准节点配材号一致
	pPlateAttachLaunder->ucs=ucs;
	//_snprintf(pPlateAttachLaunder->GetPartNo(),MAX_CHAR_GUIGE_16,"%s",(char*)m_sPartNo);
	pPlateAttachLaunder->SetPartNo(m_sPartNo);
	pPlateAttachLaunder->iSeg=SEGI(m_sSeg.GetBuffer());
	pPlateAttachLaunder->thick=thick;
	pPlateAttachLaunder->cMaterial=m_cMaterial;
	pPlateAttachLaunder->m_iParamType=TYPE_WATERSHIELDEDPLATE;
	//
	pPlateAttachLaunder->ucs.origin+=pPlateAttachLaunder->ucs.axis_z*water.S;
#ifdef AFX_TARG_ENU_ENGLISH
	pPlateAttachLaunder->SetDesignItemValue('B',pBaseNode->handle,"Datum node",PARAM_POS,1);
	pPlateAttachLaunder->SetDesignItemValue('C',pDatumTube->handle,"Datum tube",PARAM_POS,1);
	pPlateAttachLaunder->SetDesignItemValue('W',water.W,"Outer circle diameter ",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('S',water.S,"Normal Offset");
	pPlateAttachLaunder->SetDesignItemValue('K',water.K,"Water tank width",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('H',water.H,"Water tank depth",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('L',water.L,"Water tank length",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('A',water.A,"The two water tank's angle",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('O',water.O,"The offset angle between two water tank's bisector and datum edge",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('E',water.E,"Locating Datum Edge:0.Datum Edge1.Weld Line");
#else 
	pPlateAttachLaunder->SetDesignItemValue('B',pBaseNode->handle,"基准节点",PARAM_POS,1);
	pPlateAttachLaunder->SetDesignItemValue('C',pDatumTube->handle,"基准钢管",PARAM_POS,1);
	pPlateAttachLaunder->SetDesignItemValue('W',water.W,"外环直径",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('S',water.S,"法向偏移距离");
	pPlateAttachLaunder->SetDesignItemValue('K',water.K,"流水槽宽度",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('H',water.H,"流水槽深度",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('L',water.L,"流水槽长度",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('A',water.A,"两流水槽之间的夹角",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('O',water.O,"两流水槽角平分线与基准边之间的偏移角",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('E',water.E,"定位基准边:0.基准边1.焊道线");
#endif
	pPlateAttachLaunder->DesignPlate();
	pPlateAttachLaunder->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pPlateAttachLaunder->GetSolidPartObject());
	f3dPoint verfiy_vec;	//用来判断是否需要调整螺栓的法向偏移量 wht 11-05-18
	CLsRefList *pLsRefList=NULL;
	if(pPlate&&pPlate->GetClassTypeId()==CLS_PLATE)
	{
		pLsRefList=((CLDSPlate*)pPlate)->GetLsRefList();
		verfiy_vec=pPlateAttachLaunder->ucs.origin-pPlate->ucs.origin;
	}
	else if(pPlate&&pPlate->GetClassTypeId()==CLS_PARAMPLATE)
	{
		pLsRefList=((CLDSParamPlate*)pPlate)->GetLsRefList();
		verfiy_vec=pPlateAttachLaunder->ucs.origin-pPlate->ucs.origin;
	}
	normalize(verfiy_vec);
	if(pLsRefList)
	{	//引入螺栓
		for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
		{
			pPlateAttachLaunder->AppendLsRef(*pLsRef);
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			pBolt->AddL0Thick(pPlateAttachLaunder->handle,TRUE,TRUE);
			if(!verfiy_vec.IsZero()&&pBolt->ucs.axis_z*verfiy_vec<0) //调整螺栓法线偏移量 
				pBolt->des_base_pos.norm_offset.AddThick(-pPlateAttachLaunder->handle,TRUE);
			pBolt->CalGuigeAuto();
			pBolt->correct_pos();
			pBolt->SetModified();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
	}
	g_pSolidDraw->Draw();
	Ta.EndUndoListen();
	return pPlateAttachLaunder;
}

void CDesignCircularPlateDlg::ModifyPlateParam(WORD wKey,double fValue)
{
	if(m_iPlateType==4)
	{	//4.肋板2WL
		if(wKey=='B')
		{
			if(fValue<=0)	//启动自由边距定义外形
			{
				m_xSubPropList.SetItemReadOnly(GetPropID("para.D"),FALSE);
			}
			else	//启用扇形角定义外形,关闭自由边距输入
			{
				para.D=0;
				m_xSubPropList.SetItemReadOnly(GetPropID("para.D"),TRUE);
			}
		}
		else if(wKey=='D')
		{
			if(fValue<=0)	//启动扇形角定义外形
			{
				m_xSubPropList.SetItemReadOnly(GetPropID("para.B"),FALSE);
			}
			else	//启用自由边距定义外形,关闭扇形角输入
			{
				para.B=0;
				m_xSubPropList.SetItemReadOnly(GetPropID("para.B"),TRUE);
			}
		}
	}
	if(m_pCirPlate==NULL)
		return;
	DESIGN_PLATE_ITEM *pItem=m_pCirPlate->GetDesignItem(wKey);
	if(pItem)
		pItem->value.fValue=fValue;
	else	//部分可有可无的参数需要在这里设定
		m_pCirPlate->SetDesignItemValue(wKey,fValue,"",PARAM_SHAPE);
	if(wKey=='Y'&&m_pCirPlate->m_iParamType!=TYPE_CIRCOVERPLATE)
	{	//修改环向封板Y参数时不执行以下内容 wht 12-03-20
		f3dPoint axis_z=m_pCirPlate->ucs.axis_z;
		m_pCirPlate->DesignPlate(false);
		if(axis_z*m_pCirPlate->ucs.axis_z<0)
		{	//修改环向板内外侧时如果影响到Z轴方向需要将方向偏移量取反 wht 11-05-18
			double S=0,T=0;
			DESIGN_PLATE_ITEM *pItemS=m_pCirPlate->GetDesignItem('S');
			if(pItemS)
				pItemS->value.fValue*=-1;
			DESIGN_PLATE_ITEM *pItemT=m_pCirPlate->GetDesignItem('T');
			if(pItemT)
				pItemT->value.fValue*=-1;
			para.S*=-1;
			char sText[100]="";
			long idProp=GetPropID("para.S");
			if(GetPropValueStr(idProp,sText)>0)
				m_propList.SetItemPropValue(idProp,sText);
		}
	}
	else if(m_iPlateType==1&&wKey=='S')	
		pItem->value.fValue=-1*fValue;	//修改环向封板上下封板时S值需要取反 wht 11-06-07
	ReDesignPlate();
}
void CDesignCircularPlateDlg::ModifyPlateParam(WORD wKey,long nValue)
{
	if(m_pCirPlate==NULL)
		return;
	DESIGN_PLATE_ITEM *pItem=m_pCirPlate->GetDesignItem(wKey);
	if(pItem)
		pItem->value.hPart=nValue;
	ReDesignPlate();
}
void CDesignCircularPlateDlg::ReDesignPlate()
{
	if(m_pCirPlate)
	{
		m_pCirPlate->DesignPlate();
		m_pCirPlate->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(m_pCirPlate->GetSolidPartObject());
		g_pSolidDraw->AddCS(m_pCirPlate->handle,m_pCirPlate->ucs);
		g_pSolidDraw->Draw();
	}
}
//布置螺栓 wht 11-05-16
void CDesignCircularPlateDlg::LayoutBolt(CLDSParamPlate *pParamPlate)
{
	if(!m_bNewCirPlate||m_xLayoutBolt.m_nBoltNum<=0||pParamPlate==NULL)
		return;
	double init_angle = m_xLayoutBolt.m_fOffsetAngle;
	double piece_angle = 360.0/m_xLayoutBolt.m_nBoltNum;
	if(m_xLayoutBolt.m_iStartEdge==1&&m_pDatumTube->m_bHasWeldRoad)
		init_angle += m_pDatumTube->CalWeldRoadAngle()*DEGTORAD_COEF;
	for(int i=0;i<m_xLayoutBolt.m_nBoltNum;i++)
	{
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->set_d(m_xLayoutBolt.m_nBoltD);
		pBolt->iSeg = m_xLayoutBolt.m_iBoltSegI;
		pBolt->SetLayer(pParamPlate->layer());	//调整螺栓的图层名
		pBolt->cfgword=pParamPlate->cfgword;	//调整螺栓配材号与基准构件配材号一致
		pBolt->des_base_pos.hPart = pParamPlate->handle;
		pBolt->des_base_pos.cLocationStyle=TRUE; 
		pBolt->des_work_norm.norm_style=3;	//构件法线
		pBolt->des_work_norm.hVicePart=pParamPlate->handle;
		pBolt->des_work_norm.direction=m_xLayoutBolt.m_iDirection;
		//环形阵列
		pBolt->AddL0Thick(pParamPlate->handle,TRUE);//螺栓通厚
		double angle = (init_angle+i*piece_angle)*RADTODEG_COEF;	
		pBolt->des_base_pos.offset_angle = angle;			//螺栓相对于基准钢管的偏移角度
		pBolt->des_base_pos.R=m_xLayoutBolt.m_fBoltLayoutR;	//该变量小于0表示螺栓为非环向布置
		pBolt->des_base_pos.norm_offset.AddThick(-ftoi(0.5*pParamPlate->GetThick()));
		pParamPlate->AppendLsRef(pBolt->GetLsRef());

		pBolt->SetModified();
		pBolt->CalGuigeAuto();
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
}

void CDesignCircularPlateDlg::WorkPlaneSketch(int iType)
{
	DelWorkPlaneSketch();
	if(m_bNewCirPlate&&m_pCirPlate&&m_pDatumTube)
	{
		f3dPoint inters_pt;
		SnapPerp(&inters_pt,m_pDatumTube->Start(),m_pDatumTube->End(),m_pCirPlate->ucs.origin);
		GEPOINT base_pos,work_norm;
		int nLen=(WORD)m_pDatumTube->GetDiameter()*2;
		g_pSolidDraw->DelCS(m_pCirPlate->handle);
		if(iType==TYPE_DATUM_VECTOR)
		{	//起始方向--钢管基准方向
			if(m_iPlateType==6)
				nLen=(int)water.W;
			base_pos=inters_pt;
			work_norm=m_pDatumTube->WorkPlaneNorm();
			m_xSpreadVec1=m_pDatumTube->ucs.axis_x;
			btc::SKETCH_PLANE sketch1(base_pos,work_norm,m_xSpreadVec1,nLen);
			g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),sketch1.pVertexArr,sketch1.VertexCount,m_xSpreadVec1,"基准边",200);
			if(m_iPlateType==6)
			{	//焊道线
				double rotAngle=m_pDatumTube->CalWeldRoadAngle();
				m_xSpreadVec2.Set(cos(rotAngle),sin(rotAngle));
				vector_trans(m_xSpreadVec2,m_pDatumTube->ucs,TRUE);
				work_norm=m_pDatumTube->ucs.axis_z^m_xSpreadVec2;
				normalize(work_norm);
				btc::SKETCH_PLANE sketch2(base_pos,work_norm,m_xSpreadVec2,nLen);
				g_pSolidDraw->NewWorkPlaneSketch(2,RGB(252,252,252),sketch2.pVertexArr,sketch2.VertexCount,m_xSpreadVec2,"焊道边",200);
			}
		}
		else if(iType==TYPE_TUBE_SPREAD)
		{
			base_pos=inters_pt;
			if(m_iPlateType==0)
			{
				double angle=para.B*0.5*RADTODEG_COEF;
				base_pos.Set(para.R,0,0);
				rotate_point_around_axis(base_pos,angle,f3dPoint(0,0,0),f3dPoint(0,0,100));
				coord_trans(base_pos,m_pCirPlate->ucs,TRUE);
			}
			else if(m_iPlateType>1&&m_iPlateType<6)
			{
				if(para.W>0)
					base_pos+=m_pCirPlate->ucs.axis_x*(nLen*0.25+para.W);
				else if(para.R>0)
					base_pos+=m_pCirPlate->ucs.axis_x*para.R;
			}
			//根据环向板法线法线确定钢管轴向
			m_xSpreadVec1=m_pDatumTube->ucs.axis_z;
			if(m_pDatumTube->ucs.axis_z*m_pCirPlate->ucs.axis_z<0)
				m_xSpreadVec1*=-1;
			g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),&base_pos,1,m_xSpreadVec1,"钢管轴向",200);
		}
		else if(iType==TYPE_Z_AXIS_VECTOR)
		{
			base_pos=inters_pt;
			if(m_iPlateType==6)
				base_pos+=m_pCirPlate->ucs.axis_x*water.W*0.5;
			m_xSpreadVec1=m_pCirPlate->ucs.axis_z;
			g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),&base_pos,1,m_xSpreadVec1,"法向方向",200);
		}
		else if(iType==TYPE_X_AXIS_VECTOR)
		{
			base_pos=m_pCirPlate->ucs.origin;
			m_xSpreadVec1=m_pCirPlate->ucs.axis_x;
			g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),&base_pos,1,m_xSpreadVec1,"X+",200);
		}
		else if(iType==TYPE_Y_AXIS_VECTOR)
		{
			base_pos=m_pCirPlate->ucs.origin;
			m_xSpreadVec1=m_pCirPlate->ucs.axis_y;
			g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),&base_pos,1,m_xSpreadVec1,"Y+",200);
			m_xSpreadVec2=-1*m_pCirPlate->ucs.axis_y;
			g_pSolidDraw->NewWorkPlaneSketch(2,RGB(152,152,152),&base_pos,1,m_xSpreadVec2,"Y-",200);
		}
		g_pSolidDraw->Draw();
	}
}
void CDesignCircularPlateDlg::DelWorkPlaneSketch()
{
	if(!m_xSpreadVec1.IsZero())
		g_pSolidDraw->DelWorkPlaneSketch(1);
	if(!m_xSpreadVec2.IsZero())
		g_pSolidDraw->DelWorkPlaneSketch(2);
	g_pSolidDraw->Draw();
}
void CDesignCircularPlateDlg::ClearCirPlate()
{
	DelWorkPlaneSketch();
	if(m_bNewCirPlate&&m_pCirPlate)
	{
		g_pSolidDraw->DelCS(m_pCirPlate->handle);
		g_pSolidDraw->DelEnt(m_pCirPlate->handle);
		console.DispPartSet.DeleteNode(m_pCirPlate->handle);
		console.DeletePart(m_pCirPlate->handle);
		m_pCirPlate=NULL;
		g_pSolidDraw->Draw();
	}
}
void CDesignCircularPlateDlg::OnCancel() 
{
	DelWorkPlaneSketch();
	if(m_bNewCirPlate&&m_pCirPlate)
	{
		g_pSolidDraw->DelCS(m_pCirPlate->handle);
		g_pSolidDraw->DelEnt(m_pCirPlate->handle);
		console.DispPartSet.DeleteNode(m_pCirPlate->handle);
		console.DeletePart(m_pCirPlate->handle);
		g_pSolidDraw->Draw();
	}
	else if(!m_bNewCirPlate&&m_pCirPlate)
	{	//关闭对话框时选中当前编辑的构件
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pCirPlate->handle);
	}
	CDialog::OnCancel();
}

void CDesignCircularPlateDlg::OnClose() 
{
	DelWorkPlaneSketch();
	if(m_bNewCirPlate&&m_pCirPlate)
	{
		g_pSolidDraw->DelCS(m_pCirPlate->handle);
		g_pSolidDraw->DelEnt(m_pCirPlate->handle);
		console.DispPartSet.DeleteNode(m_pCirPlate->handle);
		console.DeletePart(m_pCirPlate->handle);
		g_pSolidDraw->Draw();
	}
	else if(!m_bNewCirPlate&&m_pCirPlate)
	{	//关闭对话框时选中当前编辑的构件
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pCirPlate->handle);
	}
	CDialog::OnClose();
}

//计算环向板上侧长度，下侧长度，便于生成类似封板的环向板 wht 11-05-31
void CDesignCircularPlateDlg::CalMaxMinOffsetDist(double &up_len,double &down_len)
{
	up_len=0;
	down_len=0;
	f3dPoint origin;
	if(m_pDatumTube==NULL)
		return;
	//坐标原点位置
	m_pDatumTube->BuildUCS();
	SnapPerp(&origin,m_pDatumTube->Start(),m_pDatumTube->End(),m_pDatumNode->Position(true));
	//Z轴方向
	design_axis_z.UpdateVector(console.GetActiveModel());
	f3dPoint axis_z=design_axis_z.vector;
	normalize(axis_z);
	//
	BOOL bFirstVertex=TRUE; 
	for(int i=0;i<2;i++)
	{
		CLDSPart *pPlate=NULL;
		ATOM_LIST<PROFILE_VERTEX>* pVertexList=NULL;
		if(i==0&&m_pRelaPart1&&m_pRelaPart1->IsPlate())
			pPlate=m_pRelaPart1;
		else if(i==1&&m_pRelaPart2&&m_pRelaPart2->IsPlate())
			pPlate=m_pRelaPart2;
		if(pPlate==NULL)
			continue;
		if(pPlate->GetClassTypeId()==CLS_PLATE)
			pVertexList=&(((CLDSPlate*)pPlate)->vertex_list);
		else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
			pVertexList=&(((CLDSParamPlate*)pPlate)->vertex_list);
		if(pVertexList==NULL)
			continue;
		PROFILE_VERTEX *pVertex=NULL;
		for(pVertex=pVertexList->GetFirst();pVertex;pVertex=pVertexList->GetNext())
		{
			f3dPoint vertice=pVertex->vertex;
			coord_trans(vertice,pPlate->ucs,TRUE);
			f3dPoint vertice_vec=vertice-origin;
			double dd=vertice_vec*axis_z;
			if(bFirstVertex)
			{
				up_len=down_len=dd;
				bFirstVertex=FALSE;
			}
			else if(dd<up_len)
				up_len=dd;
			else if(dd>down_len)
				down_len=dd;
		}
	}
	up_len-=0.5*thick;
	down_len+=0.5*thick;
}

#endif

