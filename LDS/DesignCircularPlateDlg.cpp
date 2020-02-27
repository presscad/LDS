// DesignCircularPlateDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "lds.h"
#include "GlobalFunc.h"
#include "DesignVectorPropDlg.h"
#include "DesignCircularPlateDlg.h"
#include "CirPlateTypePage.h"
#include "env_def.h"
#include "XhCharString.h"
#include "LmaDlg.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "PartsDbDlg.h"
#include "PropertyListOper.h"
#include "btc.h"
#include "DataCompare.h"
#include "LocalFeatureDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __COMPLEX_PART_INC_
//////////////////////////////////////////////////////////////////////////
static CHashList<CDesignCircularPlateDlg::CIRCULAR_PARAM> _localCirTypeParams;
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
			if(pCirDlg->m_iPlateType==0)
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
			pItem->m_lpNodeInfo->m_strPropName = "Circular Band Width D";
			pItem->m_lpNodeInfo->m_strPropHelp = "Circular Band Width D";
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
			pItem->m_lpNodeInfo->m_strPropName = "焊缝长W(板1)";
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
			pItem->m_lpNodeInfo->m_cmbItems="1.启用D|2.启用H"; //0.独立参数|
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
			pItem->m_lpNodeInfo->m_cmbItems="简化封板(Y+)|简化封板(Y-)";//普通封板|
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
static BOOL FireCallBackCommand(CWnd* pCallWnd,UINT message,WPARAM wParam)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please Preview part(Press enter or esc key to end):","");
#else 
	pCmdLine->FillCmdLine("旋转预览构件（ESC键或回车键返回环向板对话框）:","");
#endif
	CString cmdStr;
	g_pSolidSet->SetOperType(OPER_ROTATE);
	pCmdLine->GetStrFromCmdLine(cmdStr);
	/*DWORD hPickObj=0,dwExportFlag=0;
	while(1)
	{
		if((g_pSolidSnap->SnapObject(&hPickObj,&dwExportFlag))<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		break;
	}
	pCmdLine->FinishCmdLine();*/
	return TRUE;
}
///////////////////////////////////////
// CDesignCircularPlateDlg dialog

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
	g_pSolidDraw->ReleaseSnapStatus();
	if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("RelaBolt"))
	{	//选中所有螺栓
		for(CLsRef *pLsRef=pCirDlg->LsRefList.GetFirst();pLsRef;pLsRef=pCirDlg->LsRefList.GetNext())
			g_pSolidDraw->SetEntSnapStatus((*pLsRef)->handle);
	}
	else if(pItem->m_pParent->m_idProp==CDesignCircularPlateDlg::GetPropID("RelaPole"))
	{	//查找当前选中项
		pCirDlg->m_pCurSelConnVec=pCirDlg->ConnVecList.GetRelaConnVecByFeature(pItem->m_idProp);
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
	}
	pCirDlg->UpdateSketchMap(CDesignCircularPlateDlg::GetPropID("para.biHoleType")==pItem->m_idProp);
	pCirDlg->DisplaySubPropList(pItem);	
	pCirDlg->RedrawSketchDrawing(pItem);
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
		if(pCirDlg->m_pCurSelConnVec)
			hCurPart=pCirDlg->m_pCurSelConnVec->direct.hVicePart;
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
static BOOL FireCircularPlatePopMenuClick(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu)
{
	CDesignCircularPlateDlg *pCircularDlg=(CDesignCircularPlateDlg*)pPropList->GetParent();
	CPropertyListOper<CDesignCircularPlateDlg> oper(pPropList,pCircularDlg);
	if(CDesignCircularPlateDlg::GetPropID("para.B")==pItem->m_idProp)
	{
		if(iMenu==0)	//整圆环向板
			pCircularDlg->para.B=360;
		pCircularDlg->ModifyPlateParam('B',pCircularDlg->para.B);
		oper.SetItemPropValue(pItem->m_idProp);
		//
		pPropList->DeleteAllSonItems(pItem);
		if(fabs(pCircularDlg->para.B-360)<EPS)
		{
			CPropTreeItem* pPropItem=oper.InsertBtnPopMenuEditPropItem(pItem,"innercir_type","","","0.指定半径|1.贴合钢壁",-1,TRUE);
			//0.指定半径|1.贴合钢壁
			if(pCircularDlg->para.r<0)
				pPropItem->m_lpNodeInfo->SetCheck(2);
			else if(pCircularDlg->para.r>0)
				pPropItem->m_lpNodeInfo->SetCheck(1);
		}
	}
	else if(CDesignCircularPlateDlg::GetPropID("innercir_type")==pItem->m_idProp)
	{
		if(iMenu==0)	//指定内圆半径
		{
			pPropList->SetItemReadOnly(pItem->m_idProp,FALSE);
			if(pCircularDlg->m_pDatumTube)
				pCircularDlg->para.r=pCircularDlg->m_pDatumTube->GetDiameter()*0.5;
			pItem->m_lpNodeInfo->SetCheck(1);
		}
		else if(iMenu==1)	//根据钢管生成
		{
			pCircularDlg->para.r=-1;
			pPropList->SetItemReadOnly(pItem->m_idProp,TRUE);
			pItem->m_lpNodeInfo->SetCheck(2);
		}
		pCircularDlg->ModifyPlateParam('r',pCircularDlg->para.r);
		oper.SetItemPropValue(pItem->m_idProp);
		return TRUE;
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.r")==pItem->m_idProp)
	{
		pPropList->DeleteAllSonItems(pItem);
		if(iMenu==0)
		{	//根据钢管生成
			pCircularDlg->para.r=-1;
			pPropList->SetItemReadOnly(pItem->m_idProp,TRUE);
			pItem->m_lpNodeInfo->SetCheck(1);
		}
		else if(iMenu==1)	
		{	//指定内圆半径
			if(pCircularDlg->m_pDatumTube&&pCircularDlg->para.r<=0)
				pCircularDlg->para.r=pCircularDlg->m_pDatumTube->GetDiameter()*0.5;
			pPropList->SetItemReadOnly(pItem->m_idProp,FALSE);
			pItem->m_lpNodeInfo->SetCheck(2);
		}
		else if(iMenu==2)
		{	//指定内侧边距
			if(pCircularDlg->para.d>0)
				pCircularDlg->para.r=pCircularDlg->m_pDatumTube->GetDiameter()*0.5-pCircularDlg->m_pDatumTube->GetThick()-pCircularDlg->para.d;
			pPropList->SetItemReadOnly(pItem->m_idProp,TRUE);
			pItem->m_lpNodeInfo->SetCheck(3);
			oper.InsertEditPropItem(pItem,"para.d","","",-1,TRUE);
		}
		pCircularDlg->ModifyPlateParam('r',pCircularDlg->para.r);
		oper.SetItemPropValue(pItem->m_idProp);
		return TRUE;
	}
	else if(CDesignCircularPlateDlg::GetPropID("edge_space")==pItem->m_idProp)
	{
		double fSpace=30;
		CLsRef* pLsRef=pCircularDlg->LsRefList.GetFirst();
		if(pLsRef)
		{
			LSSPACE_STRU LsSpace;
			GetLsSpace(LsSpace,pLsRef->GetLsPtr()->get_d());
			fSpace=LsSpace.PlateEdgeSpace;
		}
		if(iMenu==0)	
		{	//指定边距
			pCircularDlg->para.fEdgeSpace=fSpace;
			pPropList->SetItemReadOnly(pItem->m_idProp,FALSE);
			pItem->m_lpNodeInfo->SetCheck(1);
			oper.SetItemPropValue(pItem->m_idProp);
		}
		else if(iMenu==1)
		{	//标准边距
			pCircularDlg->para.fEdgeSpace=0;
			pPropList->SetItemReadOnly(pItem->m_idProp,TRUE);
			pItem->m_lpNodeInfo->SetCheck(2);
		}
		else if(iMenu==2)
		{	//延伸到边
			pCircularDlg->para.fEdgeSpace=-1;
			pPropList->SetItemReadOnly(pItem->m_idProp,TRUE);
			pItem->m_lpNodeInfo->SetCheck(3);
		}
		pCircularDlg->ModifyPlateParam(KEY2C("ES"),pCircularDlg->para.fEdgeSpace);
		return TRUE;
	}
	return FALSE;
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
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(valueStr);
		if(pPart!=NULL)
		{
			pCircularDlg->m_cMaterial=pPart->cMaterial;
			pCircularDlg->thick=ftoi(pPart->thick);
			//更新属性栏
			STEELMAT *pSteel=QuerySteelMatByBriefMark(pPart->cMaterial);
			if(pSteel)
				pPropList->SetItemPropValue(CDesignCircularPlateDlg::GetPropID("material"),pSteel->mark);
			pPropList->SetItemPropValue(CDesignCircularPlateDlg::GetPropID("thick"),CXhChar16(pCircularDlg->thick));
			if(pCircularDlg->m_pCirPlate)
			{
				pCircularDlg->m_pCirPlate->thick=pCircularDlg->thick;
				//TODO:
				pCircularDlg->ReDesignPlate();	//重新设计
			}
		}
#endif
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
	if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("MirCoverPlateLabel"))
	{
		pCircularDlg->m_sMirPartLabel.Copy(valueStr);
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
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("para.biHoleType"))
	{
		pCircularDlg->para.biHoleType=valueStr[0]-'0';
		pPropList->DeleteAllSonItems(pItem);
		if(pCircularDlg->para.biHoleType==0)
		{	
			if(pCircularDlg->para.BH==0)
				pCircularDlg->para.BH=13.5;
			oper.InsertEditPropItem(pItem,"para.BH","","",-1,TRUE);
		}
		//
		pCircularDlg->InitWaterleakHolePara();
		pCircularDlg->DisplaySubPropList(pItem);
		pCircularDlg->ReDesignPlate();
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("para.BH"))
	{
		double fValue=atof(valueStr);
		if(fValue<EPS)
		{
			AfxMessageBox("流水孔直径不能为0，请重新输入!");
			return FALSE;
		}
		else
			pCircularDlg->para.BH=fValue;
		//
		pCircularDlg->ModifyPlateParam(KEY2C("BH"),pCircularDlg->para.BH);
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
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hRelaPart1"))
	{
		pCircularDlg->m_pRelaPart1=Ta.FromPartHandle(HexStrToLong(valueStr));
		if(pCircularDlg->m_pRelaPart1&&pCircularDlg->m_pCirPlate)
		{
			if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				pCircularDlg->ModifyPlateParam('F',pCircularDlg->m_pRelaPart1->handle);
			else if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||pCircularDlg->m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE)
				pCircularDlg->ModifyPlateParam('E',pCircularDlg->m_pRelaPart1->handle);
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hRelaPart1.R"))
	{
		UINT R=ftoi(atof(valueStr)*10);
		if(pCircularDlg->m_pRelaPart1&&pCircularDlg->m_pCirPlate)
		{
			if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				pCircularDlg->ModifyPlateParam('F',pCircularDlg->m_pRelaPart1->handle,R);
			else if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||pCircularDlg->m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE)
				pCircularDlg->ModifyPlateParam('E',pCircularDlg->m_pRelaPart1->handle,R);
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hRelaPart1.BG"))
	{
		DESIGN_PLATE_ITEM *pItem=NULL;
		if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			pItem=pCircularDlg->m_pCirPlate->GetDesignItem('F');
		else if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||pCircularDlg->m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE)
			pItem=pCircularDlg->m_pCirPlate->GetDesignItem('E');
		if(pItem)
		{
			pItem->dataType=3;
			pItem->value.hObj=pCircularDlg->m_pRelaPart1->handle;
			WORD* pw_BGorBS=(WORD*)&(pItem->value.uiAttribute);
			*pw_BGorBS	  =atoi(valueStr);
			//
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hRelaPart1.BS"))
	{
		DESIGN_PLATE_ITEM *pItem=NULL;
		if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			pItem=pCircularDlg->m_pCirPlate->GetDesignItem('F');
		else if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||pCircularDlg->m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE)
			pItem=pCircularDlg->m_pCirPlate->GetDesignItem('E');
		if(pItem)
		{
			pItem->dataType=3;
			pItem->value.hObj=pCircularDlg->m_pRelaPart1->handle;
			WORD* pw_BGorBS=(WORD*)&(pItem->value.uiAttribute);
			*(pw_BGorBS+1)=atoi(valueStr);
			//
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hRelaPart2"))
	{
		pCircularDlg->m_pRelaPart2=Ta.FromPartHandle(HexStrToLong(valueStr));
		if(pCircularDlg->m_pRelaPart2&&pCircularDlg->m_pCirPlate)
		{
			if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				pCircularDlg->ModifyPlateParam('G',pCircularDlg->m_pRelaPart2->handle);
			else if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||pCircularDlg->m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE)
				pCircularDlg->ModifyPlateParam('F',pCircularDlg->m_pRelaPart2->handle);
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hRelaPart2.R"))
	{
		UINT R=ftoi(atof(valueStr)*10);
		if(pCircularDlg->m_pRelaPart2&&pCircularDlg->m_pCirPlate)
		{
			if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				pCircularDlg->ModifyPlateParam('G',pCircularDlg->m_pRelaPart2->handle,R);
			else if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||pCircularDlg->m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE)
				pCircularDlg->ModifyPlateParam('F',pCircularDlg->m_pRelaPart2->handle,R);
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hRelaPart2.BG"))
	{
		DESIGN_PLATE_ITEM *pItem=NULL;
		if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			pItem=pCircularDlg->m_pCirPlate->GetDesignItem('G');
		else if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||pCircularDlg->m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE)
			pItem=pCircularDlg->m_pCirPlate->GetDesignItem('F');
		if(pItem)
		{
			pItem->dataType=3;
			pItem->value.hObj=pCircularDlg->m_pRelaPart2->handle;
			WORD* pw_BGorBS=(WORD*)&(pItem->value.uiAttribute);
			*pw_BGorBS	  =atoi(valueStr);
			//
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hRelaPart2.BS"))
	{
		DESIGN_PLATE_ITEM *pItem=NULL;
		if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			pItem=pCircularDlg->m_pCirPlate->GetDesignItem('G');
		else if(pCircularDlg->m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||pCircularDlg->m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE)
			pItem=pCircularDlg->m_pCirPlate->GetDesignItem('F');
		if(pItem)
		{
			pItem->dataType=3;
			pItem->value.hObj=pCircularDlg->m_pRelaPart2->handle;
			WORD* pw_BGorBS=(WORD*)&(pItem->value.uiAttribute);
			*(pw_BGorBS+1)=atoi(valueStr);
			//
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate0"))
	{
		pCircularDlg->m_pCoverPart0=Ta.FromPartHandle(HexStrToLong(valueStr));
		if(pCircularDlg->m_pCoverPart0)
			pCircularDlg->ModifyPlateParam(KEY2C("P0"),pCircularDlg->m_pCoverPart0->handle);
		else
			pCircularDlg->ModifyPlateParam(KEY2C("P0"),0,0,TRUE);
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate0.R"))
	{
		UINT R=ftoi(atof(valueStr)*10);
		if(pCircularDlg->m_pCoverPart0)
			pCircularDlg->ModifyPlateParam(KEY2C("P0"),pCircularDlg->m_pCoverPart0->handle,R);
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate0.BG"))
	{
		DESIGN_PLATE_ITEM *pItem=pCircularDlg->m_pCirPlate->GetDesignItem(KEY2C("P0"));
		if(pItem && pCircularDlg->m_pCoverPart0)
		{
			pItem->dataType=3;
			pItem->value.hObj=pCircularDlg->m_pCoverPart0->handle;
			WORD* pw_BGorBS=(WORD*)&(pItem->value.uiAttribute);
			*pw_BGorBS	  =atoi(valueStr);
			//
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate0.BS"))
	{
		DESIGN_PLATE_ITEM *pItem=pCircularDlg->m_pCirPlate->GetDesignItem(KEY2C("P0"));
		if(pItem && pCircularDlg->m_pCoverPart0)
		{
			pItem->dataType=3;
			pItem->value.hObj=pCircularDlg->m_pCoverPart0->handle;
			WORD* pw_BGorBS=(WORD*)&(pItem->value.uiAttribute);
			*(pw_BGorBS+1)=atoi(valueStr);
			//
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate1"))
	{
		pCircularDlg->m_pCoverPart1=Ta.FromPartHandle(HexStrToLong(valueStr));
		if(pCircularDlg->m_pCoverPart1)
			pCircularDlg->ModifyPlateParam(KEY2C("P1"),pCircularDlg->m_pCoverPart1->handle);
		else
			pCircularDlg->ModifyPlateParam(KEY2C("P1"),0,0,TRUE);
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate1.R"))
	{
		UINT R=ftoi(atof(valueStr)*10);
		if(pCircularDlg->m_pCoverPart1)
			pCircularDlg->ModifyPlateParam(KEY2C("P1"),pCircularDlg->m_pCoverPart1->handle,R);
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate1.BG"))
	{
		DESIGN_PLATE_ITEM *pItem=pCircularDlg->m_pCirPlate->GetDesignItem(KEY2C("P1"));
		if(pItem && pCircularDlg->m_pCoverPart1)
		{
			pItem->dataType=3;
			pItem->value.hObj=pCircularDlg->m_pCoverPart1->handle;
			WORD* pw_BGorBS=(WORD*)&(pItem->value.uiAttribute);
			*pw_BGorBS	  =atoi(valueStr);
			//
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate1.BS"))
	{
		DESIGN_PLATE_ITEM *pItem=pCircularDlg->m_pCirPlate->GetDesignItem(KEY2C("P1"));
		if(pItem && pCircularDlg->m_pCoverPart1)
		{
			pItem->dataType=3;
			pItem->value.hObj=pCircularDlg->m_pCoverPart1->handle;
			WORD* pw_BGorBS=(WORD*)&(pItem->value.uiAttribute);
			*(pw_BGorBS+1)=atoi(valueStr);
			//
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate2"))
	{
		pCircularDlg->m_pCoverPart2=Ta.FromPartHandle(HexStrToLong(valueStr));
		if(pCircularDlg->m_pCoverPart2)
			pCircularDlg->ModifyPlateParam(KEY2C("P2"),pCircularDlg->m_pCoverPart2->handle);
		else
			pCircularDlg->ModifyPlateParam(KEY2C("P2"),0,0,TRUE);
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate2.R"))
	{
		UINT R=ftoi(atof(valueStr)*10);
		if(pCircularDlg->m_pCoverPart2)
			pCircularDlg->ModifyPlateParam(KEY2C("P2"),pCircularDlg->m_pCoverPart2->handle,R);
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate2.BG"))
	{
		DESIGN_PLATE_ITEM *pItem=pCircularDlg->m_pCirPlate->GetDesignItem(KEY2C("P2"));
		if(pItem && pCircularDlg->m_pCoverPart2)
		{
			pItem->dataType=3;
			pItem->value.hObj=pCircularDlg->m_pCoverPart2->handle;
			WORD* pw_BGorBS=(WORD*)&(pItem->value.uiAttribute);
			*pw_BGorBS	  =atoi(valueStr);
			//
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate2.BS"))
	{
		DESIGN_PLATE_ITEM *pItem=pCircularDlg->m_pCirPlate->GetDesignItem(KEY2C("P2"));
		if(pItem && pCircularDlg->m_pCoverPart2)
		{
			pItem->dataType=3;
			pItem->value.hObj=pCircularDlg->m_pCoverPart2->handle;
			WORD* pw_BGorBS=(WORD*)&(pItem->value.uiAttribute);
			*(pw_BGorBS+1)=atoi(valueStr);
			//
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate3"))
	{
		pCircularDlg->m_pCoverPart3=Ta.FromPartHandle(HexStrToLong(valueStr));
		if(pCircularDlg->m_pCoverPart3)
			pCircularDlg->ModifyPlateParam(KEY2C("P3"),pCircularDlg->m_pCoverPart3->handle);
		else
			pCircularDlg->ModifyPlateParam(KEY2C("P3"),0,0,TRUE);
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate3.R"))
	{
		UINT R=ftoi(atof(valueStr)*10);
		if(pCircularDlg->m_pCoverPart3)
			pCircularDlg->ModifyPlateParam(KEY2C("P3"),pCircularDlg->m_pCoverPart3->handle,R);
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate3.BG"))
	{
		DESIGN_PLATE_ITEM *pItem=pCircularDlg->m_pCirPlate->GetDesignItem(KEY2C("P3"));
		if(pItem && pCircularDlg->m_pCoverPart3)
		{
			pItem->dataType=3;
			pItem->value.hObj=pCircularDlg->m_pCoverPart3->handle;
			WORD* pw_BGorBS=(WORD*)&(pItem->value.uiAttribute);
			*pw_BGorBS	  =atoi(valueStr);
			//
			pCircularDlg->ReDesignPlate();
		}
	}
	else if(pItem->m_idProp==CDesignCircularPlateDlg::GetPropID("hCoverPlate3.BS"))
	{
		DESIGN_PLATE_ITEM *pItem=pCircularDlg->m_pCirPlate->GetDesignItem(KEY2C("P3"));
		if(pItem && pCircularDlg->m_pCoverPart0)
		{
			pItem->dataType=3;
			pItem->value.hObj=pCircularDlg->m_pCoverPart3->handle;
			WORD* pw_BGorBS=(WORD*)&(pItem->value.uiAttribute);
			*(pw_BGorBS+1)=atoi(valueStr);
			//
			pCircularDlg->ReDesignPlate();
		}
	}
	//
	else if(CDesignCircularPlateDlg::GetPropID("para.j")==pItem->m_idProp)
	{
		pCircularDlg->para.j=atof(valueStr);
		pCircularDlg->ModifyPlateParam('j',pCircularDlg->para.j);
	}
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
		//
		pPropList->DeleteAllSonItems(pItem);
		if(fabs(pCircularDlg->para.B-360)<EPS)
		{
			pPropItem=oper.InsertBtnPopMenuEditPropItem(pItem,"innercir_type","","","0.指定半径|1.贴合钢壁",-1,TRUE);
			//0.指定半径|1.贴合钢壁
			if(pCircularDlg->para.r<0)
				pPropItem->m_lpNodeInfo->SetCheck(2);
			else if(pCircularDlg->para.r>0)
				pPropItem->m_lpNodeInfo->SetCheck(1);
		}
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
	else if(CDesignCircularPlateDlg::GetPropID("para.d")==pItem->m_idProp)//自由边距		
	{
		pCircularDlg->para.d=atof(valueStr);
		pCircularDlg->para.r=pCircularDlg->m_pDatumTube->GetDiameter()*0.5-pCircularDlg->m_pDatumTube->GetThick()-pCircularDlg->para.d;
		pCircularDlg->ModifyPlateParam('r',pCircularDlg->para.r);
		//
		oper.SetItemPropValue(CDesignCircularPlateDlg::GetPropID("para.r"));
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.H")==pItem->m_idProp)//台口高度
	{
		pCircularDlg->para.H=atof(valueStr);
		if(pCircularDlg->m_iPlateType==1)
			pCircularDlg->ModifyPlateParam('D',pCircularDlg->para.H);
		else 
		{
			pCircularDlg->para.h=pCircularDlg->para.H;
			CPropTreeItem *pRelaItem=pPropList->FindItemByPropId(CDesignCircularPlateDlg::GetPropID("para.h"),NULL);
			if(pRelaItem)
				pPropList->SetItemPropValue(pRelaItem->m_idProp,valueStr);
			//
			pCircularDlg->ModifyPlateParam('H',pCircularDlg->para.H);
			pCircularDlg->ModifyPlateParam('h',pCircularDlg->para.h);
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
		pCircularDlg->para.r=max(0,atof(valueStr));
		pCircularDlg->ModifyPlateParam('r',pCircularDlg->para.r);
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.r")==pItem->m_idProp)		
	{
		pCircularDlg->para.r=atof(valueStr);	//内圆半径
		pCircularDlg->ModifyPlateParam('r',pCircularDlg->para.r);
	}
	else if(CDesignCircularPlateDlg::GetPropID("edge_space")==pItem->m_idProp)
	{
		pCircularDlg->para.fEdgeSpace=max(0,atof(valueStr));
		pCircularDlg->ModifyPlateParam(KEY2C("ES"),pCircularDlg->para.fEdgeSpace);
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
		/*修改环形肋板外置不应该实时计算外形参数，否则丢失用户输入的外形参数
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
		*/
	}
	else if(CDesignCircularPlateDlg::GetPropID("para.nCirCoverPlateType")==pItem->m_idProp)	
	{	//封板类型
#ifdef AFX_TARG_ENU_ENGLISH
		//if(valueStr.CompareNoCase("Two Simple Sealing Plate")==0)
		//	pCircularDlg->para.nCirCoverPlateType=2;	//生成两块简化钢板
		//else 
		if(valueStr.CompareNoCase("Y+ Cover Plate")==0)
			pCircularDlg->para.nCirCoverPlateType=1;
		else if(valueStr.CompareNoCase("Y- Cover Plate")==0)
#else 
		//if(valueStr.CompareNoCase("两块简化封板")==0)
		//	pCircularDlg->para.nCirCoverPlateType=2;	//生成两块简化钢板
		//else 
		if(valueStr.CompareNoCase("简化封板(Y+)")==0)
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
		//
		pPropList->DeleteAllSonItems(pItem);
		if(pCircularDlg->para.bCreateMirPlate)
			oper.InsertEditPropItem(pItem,"MirCoverPlateLabel","构件编号","",-1,TRUE);
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
		double fN=atof(valueStr);
		if(pCircularDlg->m_iPlateType==1&&fN<0)
		{
			AfxMessageBox("简化封板在依赖钢板处的延伸距离不能为负!");
			return FALSE;
		}
		pCircularDlg->para.N=fN;		//第二块关联板支撑边宽
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
		pCircularDlg->RedrawSketchDrawing(pItem);
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
		pPropList->DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.h"));
		pPropList->DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.R"));
		pPropList->DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.V"));
		if(pCircularDlg->m_iPlateType==2)
		{
			pCircularDlg->para.nDHRelativeType=valueStr[0]-'0';
			pCircularDlg->AutoCalDH();
			//
			pPropItem=oper.InsertEditPropItem(pItem->m_pParent,"para.W","","",-1,TRUE);
			UpdatePropItemName(pPropList,pPropItem);
			oper.InsertEditPropItem(pItem->m_pParent,"para.N","","",-1,TRUE);
			if(pCircularDlg->para.nDHRelativeType==0||pCircularDlg->para.nDHRelativeType==1)
			{	//自由边距
				pPropItem=oper.InsertEditPropItem(pItem->m_pParent,"para.D","","",-1,TRUE);
				UpdatePropItemName(pPropList,pPropItem);
			}
			if(pCircularDlg->para.nDHRelativeType==0||pCircularDlg->para.nDHRelativeType==2)
			{	//台口高度
				oper.InsertEditPropItem(pItem->m_pParent,"para.H","","",-1,TRUE);
				//N侧台口高度
				oper.InsertEditPropItem(pItem->m_pParent,"para.h","","",-1,TRUE);
			}
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
			{
				//环形带宽D
				pPropItem=oper.InsertEditPropItem(pItem->m_pParent,"para.D","","",-1,TRUE);
				//UpdatePropItemName(pPropList,pPropItem);
				//oper.InsertEditPropItem(pItem->m_pParent,"para.R","","",-1,TRUE);
			}
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
		pCircularDlg->ModifyPlateParam('D',pCircularDlg->para.D,FALSE);
		if(pCircularDlg->m_iPlateType==3&&nOldType>0&&pCircularDlg->para.nDHRelativeType==0)
		{
			pCircularDlg->m_pCirPlate->params.DeleteNode('H');
			pCircularDlg->m_pCirPlate->params.DeleteNode('W');
			pCircularDlg->m_pCirPlate->params.DeleteNode('N');
		}
		else
			pCircularDlg->ModifyPlateParam('H',pCircularDlg->para.H,FALSE);
		pCircularDlg->ModifyPlateParam('h',pCircularDlg->para.h);
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
	if(pCircularDlg->idPlateType>0)
		_localCirTypeParams.SetValue(pCircularDlg->idPlateType,pCircularDlg->para);
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
	else if(CDesignCircularPlateDlg::GetPropID("hCoverPlate0")==pItem->m_idProp)
		pCircularDlg->SelectObject(CLS_PLATE,pItem->m_idProp);		//选择覆盖钢板0
	else if(CDesignCircularPlateDlg::GetPropID("hCoverPlate1")==pItem->m_idProp)
		pCircularDlg->SelectObject(CLS_PLATE,pItem->m_idProp);		//选择覆盖钢板1
	else if(CDesignCircularPlateDlg::GetPropID("hCoverPlate2")==pItem->m_idProp)
		pCircularDlg->SelectObject(CLS_PLATE,pItem->m_idProp);		//选择覆盖钢板2
	else if(CDesignCircularPlateDlg::GetPropID("hCoverPlate3")==pItem->m_idProp)
		pCircularDlg->SelectObject(CLS_PLATE,pItem->m_idProp);		//选择覆盖钢板3
	else if(CDesignCircularPlateDlg::GetPropID("type")==pItem->m_idProp)
	{
		CCirPlateTypePage selTypPage;
		selTypPage.idCirPlateType=pCircularDlg->idPlateType;
		if(selTypPage.DoModal()!=IDOK || pCircularDlg->idPlateType==selTypPage.idCirPlateType)
			return FALSE;
		if(pCircularDlg->idPlateType==selTypPage.idCirPlateType)
			return TRUE;	//未发生变化，不进行后续适应性参数处理 wjh-2018.3.7
		int idOldPlateType=pCircularDlg->idPlateType;
		pCircularDlg->idPlateType=selTypPage.idCirPlateType;
		if(!pCircularDlg->IsValidParam(FALSE))
		{	//不满足必要的设计条件
			pCircularDlg->idPlateType=idOldPlateType;
		}
		else
		{	//
			CXhChar100 itemStr;
			pCircularDlg->GetPropValueStr(pItem->m_idProp,itemStr);
			pItem->m_lpNodeInfo->m_strPropValue=itemStr;
			//重新初始化钢板外形参数,生成钢板实体方便预览
			pCircularDlg->LsRefList.Empty();
			pCircularDlg->ConnVecList.Empty();
			pCircularDlg->InitProfilePara();
			pCircularDlg->InitWaterleakHolePara();
			pCircularDlg->UpdateCirPlateBasicProp();
			pCircularDlg->UpdateCirPlateRelaProp();
			pCircularDlg->UpdateCirPlateLocationProp();
			pCircularDlg->UpdateSketchMap();
			FireCircularPlateHelpStr(pPropList,pItem);	//更新帮助字符串
			pCircularDlg->PreviewCircularPlate();
		}
		return TRUE;
		//return ModifyCircularPlateProperty(pPropList,pItem, CString(itemStr));
	}
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
	else if(CDesignCircularPlateDlg::GetPropID("RelaInfo")==pItem->m_idProp)
	{
		pCircularDlg->SelectObject(CLS_PLATE,pItem->m_idProp);
#ifdef AFX_TARG_ENU_ENGLISH
		pCircularDlg->SetCmdPickPromptStr("Please Select Circular Plate's Relative cover plate:");
#else 
		pCircularDlg->SetCmdPickPromptStr("请选择添加封板覆盖的关联钢板:");
#endif
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
	else if(CDesignCircularPlateDlg::GetPropID("para.Y")==pItem->m_idProp)
	{
		int side=pCircularDlg->SideOfCirRibPlateOnOutwardToBasicPlate();
		pCircularDlg->para.Y=-side;
		if(pCircularDlg->m_iPlateType==1)	//环向封板
			pCircularDlg->para.nCirCoverPlateType=(long)pCircularDlg->para.Y;
		pCircularDlg->ModifyPlateParam('Y',pCircularDlg->para.Y);
		CXhChar100 itemText;
		pCircularDlg->GetPropValueStr(pItem->m_idProp,itemText);
		pPropList->SetItemPropValue(pItem->m_idProp,itemText);
	}
	else if(CDesignCircularPlateDlg::GetPropID("m_xLayoutBolt")==pItem->m_idProp&&!pCircularDlg->m_bNewCirPlate)
	{	//布置螺栓
		if(pCircularDlg->m_pCirPlate==NULL)
			return FALSE;
		pCircularDlg->LayoutBolt(pCircularDlg->m_pCirPlate);
	}
	return TRUE;
}

int CDesignCircularPlateDlg::nScrLocationX=0;
int CDesignCircularPlateDlg::nScrLocationY=0;
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
	m_pCoverPart0=m_pCoverPart1=m_pCoverPart2=m_pCoverPart3=NULL;
	m_iPlateType=0;
	memset(&para,0,sizeof(CIRCULAR_PARAM));
	para.Y=0;
	para.gama=45;
	memset(&water,0,sizeof(PARAM_WATER));
	para.r=-1;	//根据钢管生成内圆
	thick=8;
	para.bWeldSemicircleMode = FALSE;
	para.BH=0;//13.5;
	para.biHoleType=1;
	para.d=0;
	para.j=0;	//起始边
	para.fEdgeSpace=30;
	//m_sSeg=0;
	m_cMaterial='S';
	m_pCurSelConnVec=NULL;
	m_pCirPlate=NULL;
	m_bNewCirPlate=TRUE;
	m_bReadOnlyPlateType=FALSE;
	m_sWndTitle=_T("");
	m_bDisplayDlg=TRUE;
	_previewParamPlate.handle=ISolidDraw::BLK_LOCATE_CS+1;	//临时对象句柄, 但需要避开系统特定坐标系的ID
	_previewParamPlate.SetBelongModel(&Ta);
	m_idPlateType=0;	//0表示未初始化
}
void CDesignCircularPlateDlg::OnBnModelViewOper()
{	//临时中断转换至模型操作环境中，以便于用户对三维模型缩放、旋转查看等操作 wjh-2018.3.2
	ExecuteCommand(this,GetPropID("preview"));
}
int CDesignCircularPlateDlg::SideOfCirRibPlateOnOutwardToBasicPlate()	//环向肋板是否在基板外侧
{
	if(m_iPlateType==1&&abs(para.nCirCoverPlateType)==2)
		return para.nCirCoverPlateType;
	else if(m_iPlateType<2||m_iPlateType>5)
		return 0;
	else if(para.Y>0)
		return 1;
	else if(para.Y<0)
		return -1;
	else if(m_pCirPlate!=NULL&&m_pRelaPart1!=NULL)
		return m_pCirPlate->ucs.axis_y*m_pRelaPart1->ucs.axis_z>0?1:-1;
	else
		return -1;
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
	ON_BN_CLICKED(IDC_BTN_MODEL_VIEW_OPER, &CDesignCircularPlateDlg::OnBnModelViewOper)
	ON_WM_CLOSE()
	ON_WM_MOVE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////
// CDesignCircularPlateDlg message handlers

BOOL CDesignCircularPlateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetCallBackFunc(FireCallBackCommand);
	if(m_sWndTitle.GetLength()>0)
		SetWindowText(m_sWndTitle);
	InitPropHashtable();
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.SetDividerScale(0.55);
	m_propList.SetModifyValueFunc(ModifyCircularPlateProperty);
	m_propList.SetButtonClickFunc(ButtonClickCircularPlate);
	m_propList.SetPropHelpPromptFunc(FireCircularPlateHelpStr);
	//
	m_xSubPropList.SetDividerScale(0.6);
	m_xSubPropList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xSubPropList.SetModifyValueFunc(ModifyCircularPlateProperty);
	m_xSubPropList.SetPopMenuClickFunc(FireCircularPlatePopMenuClick);
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
				if(m_pRelaPart1->GetClassTypeId()==CLS_LINEANGLE)
					m_iPlateType=7;
				else if(m_pRelaPart1->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)m_pRelaPart1)->IsTransPlate())
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
		if(m_idPlateType==0)
		{	//未初始化时才需要显示环向板类型选择窗口 wjh-2018.3.1
			CCirPlateTypePage selTypPage;
			selTypPage.idCirPlateType=_IntelliRecogPlateTypeId();
			if(selTypPage.DoModal()==IDOK)
			{
				if(m_idPlateType!=selTypPage.idCirPlateType)
				{
					long idPrevType=m_idPlateType;
					idPlateType=selTypPage.idCirPlateType;
					if(!IsValidParam(FALSE))	//不满足必要的设计条件时类型回滚至初始类型 wjh-2018.3.7
						idPlateType=idPrevType;
				}
			}
		}
		if(m_pCirPlate==NULL)
		{
			m_cMaterial=(m_pDatumTube!=NULL)?m_pDatumTube->cMaterial:m_pDatumSphere->cMaterial;
			m_sSeg=(m_pDatumTube!=NULL)?m_pDatumTube->iSeg.ToString():m_pDatumSphere->iSeg.ToString();
			m_pCirPlate=&_previewParamPlate;	//在新建环板时，m_pCirPlate初始化为预览构件
			InitProfilePara();
			InitWaterleakHolePara();	//要求m_pCirPlate!=NULL才能正确初始化，故须先赋值m_pCirPlate==NULL情况 wjh-2018.3.1
			PreviewCircularPlate();
		}
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
			m_pCirPlate->GetDesignItemValue('D',&para.H);
			m_pCirPlate->GetDesignItemValue('W',&para.W);
			m_pCirPlate->GetDesignItemValue('S',&para.S);
			m_pCirPlate->GetDesignItemValue('R',&para.R);
			m_pCirPlate->GetDesignItemValue('N',&para.N);
			m_pCirPlate->GetDesignItemValue(KEY2C("R1"),&para.R1);
			m_pCirPlate->GetDesignItemValue(KEY2C("R2"),&para.R2);
			if(!m_pCirPlate->GetDesignItemValue(KEY2C("BH"),&para.BH)||para.BH<-EPS)
				para.biHoleType=2;	//无流水孔
			else
				para.biHoleType=(para.BH>EPS)?0:1;
			//基准板1
			DESIGN_PLATE_ITEM* pItemPara=m_pCirPlate->GetDesignItem('F');
			if(pItemPara->dataType==3)
				para.E=pItemPara->value.hPart;
			else
			{
				para.E=pItemPara->ToLong();
				//更新封板的设计参数
				pItemPara->dataType=3;
				pItemPara->value.hObj=para.E;
				pItemPara->value.uiAttribute=ftoi(para.R1>0?para.R1*10:0);
			}
			//基准板2
			if(pItemPara=m_pCirPlate->GetDesignItem('G'))
			{
				if(pItemPara->dataType==3)
					para.F=pItemPara->value.hPart;
				else
				{
					para.F=pItemPara->ToLong();
					//更新封板的设计参数
					pItemPara->dataType=3;
					pItemPara->value.hObj=para.F;
					pItemPara->value.uiAttribute=ftoi(para.R2>0?para.R2*10:0);
				}
			}
			//覆盖板
			for(int i=0;i<4;i++)
			{
				CXhChar16 key_str("P%d",i);
				CLDSGeneralPlate::DESIGN_PARAM item_para;
				if(!m_pCirPlate->GetDesignItemValue(KEY2C(key_str),&item_para))
					continue;
				long hCoverPart=0;
				if(item_para.cValueType==3)
					hCoverPart=item_para.value.hPart;
				else
					hCoverPart=item_para.ToLong();
				if(m_pCoverPart0==NULL)
					m_pCoverPart0=console.FromPartHandle(hCoverPart,CLS_PLATE,CLS_PARAMPLATE);
				else if(m_pCoverPart1==NULL)
					m_pCoverPart1=console.FromPartHandle(hCoverPart,CLS_PLATE,CLS_PARAMPLATE);
				else if(m_pCoverPart2==NULL)
					m_pCoverPart2=console.FromPartHandle(hCoverPart,CLS_PLATE,CLS_PARAMPLATE);
				else if(m_pCoverPart1==NULL)
					m_pCoverPart1=console.FromPartHandle(hCoverPart,CLS_PLATE,CLS_PARAMPLATE);
			}
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
			m_pCirPlate->GetDesignItemValue('j',&para.j);	//j:起始边 0.基准边|1.焊道线
			//E:第一块关联基准钢板句柄
			DESIGN_PLATE_ITEM* pItemPara;
			if((pItemPara=m_pCirPlate->GetDesignItem('E'))!=NULL)
			{
				if(pItemPara->dataType==3)
					para.E=pItemPara->value.hPart;
				else
					para.E=pItemPara->ToLong();
			}
			//F:第二块关联基准钢板句柄，或焊接关联钢管句柄(关联钢管是指与环向板有焊缝的钢管，不含上面的插板螺栓连接钢管，因此不可能出现两根)
			if((pItemPara=m_pCirPlate->GetDesignItem('F'))!=NULL)
			{
				if(pItemPara->dataType==3)
					para.F=pItemPara->value.hPart;
				else
					para.F=pItemPara->ToLong();
			}
			//BH流水孔类型
			if(!m_pCirPlate->GetDesignItemValue(KEY2C("BH"),&para.BH)||para.BH<-EPS)
				para.biHoleType=2;	//无流水孔
			else
				para.biHoleType=(para.BH>EPS)?0:1;
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
			design_axis_z=m_pCirPlate->workNorm;	//以上代友估计是以前遗留的考虑不周的临时代码 wjh-2011.11.22
			//初始化关联构件
			CLDSPart *pPart=console.FromPartHandle(para.C,CLS_LINETUBE,CLS_SPHERE);
			if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
				m_pDatumTube=(CLDSLineTube*)pPart;
			else if(pPart)
				m_pDatumSphere=(CLDSSphere*)pPart;
			m_pDatumNode=console.FromNodeHandle(para.P);
			m_pRelaPart1=console.FromPartHandle(para.E,CLS_PLATE,CLS_PARAMPLATE,CLS_LINEANGLE);
			m_pRelaPart2=console.FromPartHandle(para.F,CLS_PLATE,CLS_LINETUBE,CLS_PARAMPLATE);
			//初始化环向板类型
			if(m_pRelaPart1==NULL&&m_pRelaPart2==NULL)
			{
				m_iPlateType=0;	//环向板(一般是异型法兰)
				if(ConnVecList.GetCount(TRUE)>0)
					para.d=m_pDatumTube->GetDiameter()*0.5-m_pDatumTube->GetThick()-para.r;
				//初始化螺栓布置参数
				m_xLayoutBolt.m_iBoltSegI=SEGI(m_sSeg);
				m_xLayoutBolt.m_nBoltD=20;
				if(m_pDatumTube)
					m_xLayoutBolt.m_fBoltLayoutR=m_pDatumTube->GetDiameter()*0.5+30;
			}
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
					if(m_pRelaPart1->IsAngle())
					{
						m_pCirPlate->GetDesignItemValue(KEY2C("ES"),&para.fEdgeSpace);
						m_iPlateType=7;
					}
					else //if(m_pRelaPart1->IsPlate())
					{
						if(para.G>0)	//自由边为直线
							m_iPlateType=4;
						else 
							m_iPlateType=5;
					}
				}
			}
			else if(m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE)
			{
				m_iPlateType=3;	//3WA
				para.nDHRelativeType=1;	//D,H为独立参数
			}
		}
		_IntelliRecogPlateTypeId();	//自动识别封板类型
		if(m_pCirPlate->m_iParamType!=TYPE_WATERSHIELDEDPLATE)
			_localCirTypeParams.SetValue(m_idPlateType,para);
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
	CRect rect;
	GetWindowRect(rect);
	int width = rect.Width();
	int height=rect.Height();
	rect.left=nScrLocationX;
	rect.top=nScrLocationY;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;
	MoveWindow(rect,TRUE);
	m_bEnableWindowMoveListen=true;
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
	AddPropItem("para.j",PROPLIST_ITEM(id++,"sector start edge","","0.datum line|1.weld line"));
	AddPropItem("para.A",PROPLIST_ITEM(id++,"Sector Start Angle α","The start angle α°of sector circular plate"));
	AddPropItem("para.B",PROPLIST_ITEM(id++,"Section Angle β"));
	AddPropItem("para.D",PROPLIST_ITEM(id++,"Free Margin D"));
	AddPropItem("para.d",PROPLIST_ITEM(id++,"Free Margin d"));
	AddPropItem("para.H",PROPLIST_ITEM(id++,"Step Hatch Height H"));
	AddPropItem("para.h",PROPLIST_ITEM(id++,"Step Hatch Height Hn"));
	AddPropItem("para.R",PROPLIST_ITEM(id++,"Outer Circle Radius Ro"));
	AddPropItem("innercir_type",PROPLIST_ITEM(id++,"Inner Circle Type","","0.No Inner Circle|1.Create By Tube"));
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
	AddPropItem("MirCoverPlateLabel",PROPLIST_ITEM(id++,"Another Side Sealing Plate's label","",""));
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
	AddPropItem("preview",PROPLIST_ITEM(id++,"预览环板","支持预览环板","预览"));
	//
	AddPropItem("RelaInfo",PROPLIST_ITEM(id++,"关联信息","","添加覆盖板"));
	AddPropItem("hDatumSphere",PROPLIST_ITEM(id++,""));
	AddPropItem("hDatumTube",PROPLIST_ITEM(id++,"基准钢管","定位基准钢管句柄"));
	AddPropItem("hDatumNode",PROPLIST_ITEM(id++,"基准节点","定位基准节点句柄"));
	AddPropItem("para.biHoleType",PROPLIST_ITEM(id++,"流水孔类型","关联钢板焊缝处的流水孔类型","0.圆孔|1.半圆孔|2.无"));
	AddPropItem("para.BH",PROPLIST_ITEM(id++,"流水孔直径","关联钢板焊缝处的流水孔直径"));
	AddPropItem("hRelaPart1",PROPLIST_ITEM(id++,"关联基板","第一块关联钢板(即W侧)的句柄"));
	AddPropItem("hRelaPart1.R",PROPLIST_ITEM(id++,"半圆孔半径","第一块关联钢板处的流水孔半径"));
	AddPropItem("hRelaPart1.BG",PROPLIST_ITEM(id++,"流水孔间隙G","流水孔到管壁距离G"));
	AddPropItem("hRelaPart1.BS",PROPLIST_ITEM(id++,"流水孔间隙S","流水孔到立板壁距离S"));
	AddPropItem("hRelaPart2",PROPLIST_ITEM(id++,"关联构件2","第二个关联构件(即N侧钢板或钢管)的句柄"));
	AddPropItem("hRelaPart2.R",PROPLIST_ITEM(id++,"半圆孔半径","第二块关联钢板处的流水孔半径"));
	AddPropItem("hRelaPart2.BG",PROPLIST_ITEM(id++,"流水孔间隙G","流水孔到管壁距离G"));
	AddPropItem("hRelaPart2.BS",PROPLIST_ITEM(id++,"流水孔间隙S","流水孔到立板壁距离S"));

	AddPropItem("hCoverPlate0",PROPLIST_ITEM(id++,"覆盖钢板", "除定位基准钢板外的关联覆盖钢板"));
	AddPropItem("hCoverPlate0.R",PROPLIST_ITEM(id++,"半圆孔半径","覆盖钢板处的流水孔半径"));
	AddPropItem("hCoverPlate0.BG",PROPLIST_ITEM(id++,"流水孔间隙G","流水孔到管壁距离G"));
	AddPropItem("hCoverPlate0.BS",PROPLIST_ITEM(id++,"流水孔间隙S","流水孔到立板壁距离S"));
	AddPropItem("hCoverPlate1",PROPLIST_ITEM(id++,"覆盖钢板2","除定位基准钢板外的关联覆盖钢板"));
	AddPropItem("hCoverPlate1.R",PROPLIST_ITEM(id++,"半圆孔半径","覆盖钢板2处的流水孔半径"));
	AddPropItem("hCoverPlate1.BG",PROPLIST_ITEM(id++,"流水孔间隙G","流水孔到管壁距离G"));
	AddPropItem("hCoverPlate1.BS",PROPLIST_ITEM(id++,"流水孔间隙S","流水孔到立板壁距离S"));
	AddPropItem("hCoverPlate2",PROPLIST_ITEM(id++,"覆盖钢板3","除定位基准钢板外的关联覆盖钢板"));
	AddPropItem("hCoverPlate2.R",PROPLIST_ITEM(id++,"半圆孔半径","覆盖钢板3处的流水孔半径"));
	AddPropItem("hCoverPlate2.BG",PROPLIST_ITEM(id++,"流水孔间隙G","流水孔到管壁距离G"));
	AddPropItem("hCoverPlate2.BS",PROPLIST_ITEM(id++,"流水孔间隙S","流水孔到立板壁距离S"));
	AddPropItem("hCoverPlate3",PROPLIST_ITEM(id++,"覆盖钢板4","除定位基准钢板外的关联覆盖钢板"));
	AddPropItem("hCoverPlate3.R",PROPLIST_ITEM(id++,"半圆孔半径","覆盖钢板4处的流水孔半径"));
	AddPropItem("hCoverPlate3.BG",PROPLIST_ITEM(id++,"流水孔间隙G","流水孔到管壁距离G"));
	AddPropItem("hCoverPlate3.BS",PROPLIST_ITEM(id++,"流水孔间隙S","流水孔到立板壁距离S"));
	
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
	AddPropItem("para.j",PROPLIST_ITEM(id++,"扇形起始边","","0.基准边|1.焊道线"));
	AddPropItem("para.A",PROPLIST_ITEM(id++,"扇形起始角α","扇形环向板的起始角α°"));
	AddPropItem("para.B",PROPLIST_ITEM(id++,"扇形角β","环向板扇形夹角β,单位°"));
	AddPropItem("para.D",PROPLIST_ITEM(id++,"自由边距D","钢管外壁到环板外径的距离"));
	AddPropItem("para.d",PROPLIST_ITEM(id++,"内侧边距d","钢管内壁到环板内径的距离"));
	AddPropItem("para.H",PROPLIST_ITEM(id++,"台口高度Hw","W侧台口高度H"));
	AddPropItem("para.h",PROPLIST_ITEM(id++,"台口高度Hn","N侧台口高度H,0值表示与Hw取值相同"));
	AddPropItem("para.R",PROPLIST_ITEM(id++,"外圆半径Ro"));
	AddPropItem("innercir_type",PROPLIST_ITEM(id++,"内圆半径","","0.指定半径|1.贴合钢壁"));
	AddPropItem("para.r",PROPLIST_ITEM(id++,"内圆半径r"));
	AddPropItem("para.S",PROPLIST_ITEM(id++,"位置偏移S","位置偏移包括法向偏移即沿钢板Z轴方向的偏移，纵向偏移即沿主管延伸方向(且与Z轴同向的方向)的偏移"));
	AddPropItem("para.V",PROPLIST_ITEM(id++,"加工间隙V"));
	AddPropItem("para.W",PROPLIST_ITEM(id++,"环形带宽W"));
	AddPropItem("para.R1",PROPLIST_ITEM(id++,"半圆孔半径1"));
	AddPropItem("para.R2",PROPLIST_ITEM(id++,"半圆孔半径2"));
	AddPropItem("para.N",PROPLIST_ITEM(id++,"焊缝长N(板2)"));
	helpStr.Copy("自动判断时,如果存在两块关联钢板则默认在两钢板夹角锐角\n侧生成环向板，如果为一块关联钢板则默认在基板外侧生成环向板。");
	AddPropItem("para.Y",PROPLIST_ITEM(id++,"前后侧切换",helpStr,"基板外侧|基板内侧|自动判断"));
	AddPropItem("para.y",PROPLIST_ITEM(id++,"倒角角度γ","倒角角度γ,单位°"));
	AddPropItem("para.nOffsetType",PROPLIST_ITEM(id++,"偏移类型","位置偏移包括法向偏移即沿钢板Z轴方向的偏移，纵向偏移即沿主管延伸方向(与板法线方向侧为正偏移)的偏移","0.纵向偏移|1.法向偏移"));
	AddPropItem("para.nDHRelativeType",PROPLIST_ITEM(id++,"启用D和H参数"));
	AddPropItem("para.bCreateMirPlate",PROPLIST_ITEM(id++,"生成另一侧封板","生成另一侧封板,封板扇形角为180°时该变量有效","是|否"));
	AddPropItem("MirCoverPlateLabel",PROPLIST_ITEM(id++,"构件编号","",""));
	AddPropItem("para.nCirCoverPlateType",PROPLIST_ITEM(id++,"封板类型","普通封板,扇形角β在Y轴正方的简化封板,扇形角β在Y轴负方的简化封板","简化封板(Y+)|简化封板(Y-)"));
	AddPropItem("edge_space",PROPLIST_ITEM(id++,"螺栓边距","","0.指定边距|1.标准边距|2.延伸到边"));
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
	//
	AddPropItem("WaterleakHole",PROPLIST_ITEM(id++,"流水孔参数"));
	AddPropItem("WaterleakHole_RP1",PROPLIST_ITEM(id++,"关联基板1流水孔"));
	AddPropItem("WaterleakHole_RP2",PROPLIST_ITEM(id++,"关联基板2流水孔"));
	AddPropItem("WaterleakHole_CP0",PROPLIST_ITEM(id++,"覆盖钢板1流水孔"));
	AddPropItem("WaterleakHole_CP1",PROPLIST_ITEM(id++,"覆盖钢板2流水孔"));
	AddPropItem("WaterleakHole_CP2",PROPLIST_ITEM(id++,"覆盖钢板3流水孔"));
	AddPropItem("WaterleakHole_CP3",PROPLIST_ITEM(id++,"覆盖钢板4流水孔"));
#endif
}

int CDesignCircularPlateDlg::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)
{	//根据属性ID得到属性值
	CXhChar500 sText;
	if(id==GetPropID("sPartNo"))
		sText.Copy(m_sPartNo);
	else if(id==GetPropID("MirCoverPlateLabel"))
		sText.Copy(m_sMirPartLabel);
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
			if(m_idPlateType==TYPE_CIR_BASE)
				sText.Copy("0.环形底座板");
			else if(ConnVecList.GetCount(TRUE)>0)	//异形法兰
				sText.Copy("异形法兰");
			else
				sText.Copy("0.普通环板");
		}
		else if(m_iPlateType==1)
		{
			if(m_idPlateType==TYPE_COVERPLATE3)
				sText.Copy("1.简化封板");
			else if(m_idPlateType==TYPE_COVERPLATE2)
				sText.Copy("1.圆弧封板");
			else if(m_idPlateType==TYPE_COVERPLATE4)
				sText.Copy("1.直角封板");
		}
		else if(m_iPlateType==2)
			sText.Copy("2.肋板3WL");
		else if(m_iPlateType==3)
		{
			if(m_idPlateType==TYPE_3WA_1)
				sText.Copy("3.连板3WA");	//跨越连板
			else if(m_idPlateType==TYPE_3WA_2)
				sText.Copy("3.肋板3WA");
		}
		else if(m_iPlateType==4)
		{
			if(m_idPlateType==TYPE_2WL_1)
				sText.Copy("4.肋板2WL(指定D)");
			else //if(m_idPlateType==TYPE_2WL_2)
				sText.Copy("4.肋板2WL(指定β角)");
		}
		else if(m_iPlateType==5)
			sText.Copy("5.肋板2WA");
		else if(m_iPlateType==6)
			sText.Copy("6.遮水板");
		else if(m_iPlateType==7)
			sText.Copy("7.走道板");
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
	else if(GetPropID("para.biHoleType")==id)
	{
		if(para.biHoleType==0)
			sText.Copy("0.圆孔");
		else if(para.biHoleType==1)
			sText.Copy("1.半圆孔");
		else //if(para.biHoleType==2)
			sText.Copy("2.无");
	}
	else if(GetPropID("para.BH")==id)
	{
		if(para.biHoleType==0)
			sText.Printf("%g",para.BH);
		else
			sText.Copy("0");
	}
	else if(GetPropID("hRelaPart1")==id)
	{
		if((m_iPlateType==2&&para.nDHRelativeType==0)	//3WL D,H为独立参数 wht 11-05-11
			||(m_iPlateType==3&&para.nDHRelativeType))	//3WA D,H为独立参数
		{
			CONNECT_DIRECTION *pVec=ConnVecList.GetRelaConnInfo(1);
			if(pVec)
			{
				pVec->direct.vector.Set();
				pVec->direct.UpdateVector(console.GetActiveModel());
			}
		}
		if(m_pRelaPart1)
			sText.Printf("0X%X",m_pRelaPart1->handle);
		else 
			sText.Copy("0X0");
	}
	else if(GetPropID("hRelaPart1.R")==id)
	{
		double R=0;
		CLDSParamPlate::DESIGN_PARAM param;
		if(m_pCirPlate&&m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			m_pCirPlate->GetDesignItemValue('F',&param);
		if(m_pCirPlate&&(m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE))
			m_pCirPlate->GetDesignItemValue('E',&param);
		if(param.cValueType==3)	//句柄+数值
			R=param.value.uiAttribute*0.1;
		sText.Printf("%.1f",R);
	}
	else if(GetPropID("hRelaPart1.BG")==id)
	{
		WORD BG=0;
		CLDSParamPlate::DESIGN_PARAM param;
		if(m_pCirPlate&&m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			m_pCirPlate->GetDesignItemValue('F',&param);
		if(m_pCirPlate&&(m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE))
			m_pCirPlate->GetDesignItemValue('E',&param);
		if(param.cValueType==3)
		{
			WORD* pw_BGorBS=(WORD*)&param.value.uiAttribute;
			BG=*pw_BGorBS;
		}
		sText.Printf("%d",BG);
	}
	else if(GetPropID("hRelaPart1.BS")==id)
	{
		WORD BS=0;
		CLDSParamPlate::DESIGN_PARAM param;
		if(m_pCirPlate&&m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			m_pCirPlate->GetDesignItemValue('F',&param);
		if(m_pCirPlate&&(m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE))
			m_pCirPlate->GetDesignItemValue('E',&param);
		if(param.cValueType==3)
		{
			WORD* pw_BGorBS=(WORD*)&param.value.uiAttribute;
			BS=*(pw_BGorBS+1);
		}
		sText.Printf("%d",BS);
	}
	else if(GetPropID("hRelaPart2")==id)
	{
		if((m_iPlateType==2&&para.nDHRelativeType==0)	//3WL D,H为独立参数 wht 11-05-11
			||(m_iPlateType==3&&para.nDHRelativeType))	//3WA D,H为独立参数
		{
			CONNECT_DIRECTION *pVec=ConnVecList.GetRelaConnInfo(2);
			if(pVec)
			{
				pVec->direct.vector.Set();
				pVec->direct.UpdateVector(console.GetActiveModel());
			}
		}
		if(m_pRelaPart2)
			sText.Printf("0X%X",m_pRelaPart2->handle);
		else 
			sText.Copy("0X0");
	}
	else if(GetPropID("hRelaPart2.R")==id)
	{
		double R=0;
		CLDSParamPlate::DESIGN_PARAM param;
		if(m_pCirPlate&&m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			m_pCirPlate->GetDesignItemValue('G',&param);
		if(m_pCirPlate&&(m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE))
			m_pCirPlate->GetDesignItemValue('F',&param);
		if(param.cValueType==3)	//句柄+数值
			R=param.value.uiAttribute*0.1;
		sText.Printf("%.1f",R);
	}
	else if(GetPropID("hRelaPart2.BG")==id)
	{
		WORD BG=0;
		CLDSParamPlate::DESIGN_PARAM param;
		if(m_pCirPlate&&m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			m_pCirPlate->GetDesignItemValue('G',&param);
		if(m_pCirPlate&&(m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE))
			m_pCirPlate->GetDesignItemValue('F',&param);
		if(param.cValueType==3)	//句柄+数值
		{
			WORD* pw_BGorBS=(WORD*)&param.value.uiAttribute;
			BG=*pw_BGorBS;
		}
		sText.Printf("%d",BG);
	}
	else if(GetPropID("hRelaPart2.BS")==id)
	{
		WORD BS=0;
		CLDSParamPlate::DESIGN_PARAM param;
		if(m_pCirPlate && m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			m_pCirPlate->GetDesignItemValue('G',&param);
		if(m_pCirPlate&&(m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE||m_pCirPlate->m_iParamType==TYPE_TUBERIBPLATE))
			m_pCirPlate->GetDesignItemValue('F',&param);
		if(param.cValueType==3)	//句柄+数值
		{
			WORD* pw_BGorBS=(WORD*)&param.value.uiAttribute;
			BS=*(pw_BGorBS+1);
		}
		sText.Printf("%d",BS);
	}
	else if(GetPropID("hCoverPlate0")==id)
	{
		if(m_pCirPlate==NULL||m_pCirPlate->m_iParamType!=TYPE_CIRCOVERPLATE)
			sText="";
		else
		{
			long hPlate=0;
			m_pCirPlate->GetDesignItemValue(KEY2C("P0"),&hPlate);
			sText.Printf("0X%X",hPlate);
		}
	}
	else if(GetPropID("hCoverPlate0.R")==id)
	{
		double R=0;
		if(m_pCirPlate && m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{
			CLDSParamPlate::DESIGN_PARAM param;
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P0"),&param))
			{
				if(param.cValueType==3)	//句柄+数值
					R=param.value.uiAttribute*0.1;
			}
		}
		sText.Printf("%.1f",R);
	}
	else if(GetPropID("hCoverPlate0.BG")==id)
	{
		WORD BG=0;
		if(m_pCirPlate && m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{
			CLDSParamPlate::DESIGN_PARAM param;
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P0"),&param))
			{
				if(param.cValueType==3)	//句柄+数值
				{
					WORD* pw_BGorBS=(WORD*)&param.value.uiAttribute;
					BG=*pw_BGorBS;
				}
			}
		}
		sText.Printf("%d",BG);
	}
	else if(GetPropID("hCoverPlate0.BS")==id)
	{
		WORD BS=0;
		if(m_pCirPlate && m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{
			CLDSParamPlate::DESIGN_PARAM param;
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P0"),&param))
			{
				if(param.cValueType==3)	//句柄+数值
				{
					WORD* pw_BGorBS=(WORD*)&param.value.uiAttribute;
					BS=*(pw_BGorBS+1);
				}
			}
		}
		sText.Printf("%d",BS);
	}
	else if(GetPropID("hCoverPlate1")==id)
	{
		if(m_pCirPlate==NULL||m_pCirPlate->m_iParamType!=TYPE_CIRCOVERPLATE)
			sText="";
		else
		{
			long hPlate=0;
			m_pCirPlate->GetDesignItemValue(KEY2C("P1"),&hPlate);
			sText.Printf("0X%X",hPlate);
		}
	}
	else if(GetPropID("hCoverPlate1.R")==id)
	{
		double R=0;
		if(m_pCirPlate && m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{
			CLDSParamPlate::DESIGN_PARAM param;
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P1"),&param))
			{
				if(param.cValueType==3)	//句柄+数值
					R=param.value.uiAttribute*0.1;
			}
		}
		sText.Printf("%.1f",R);
	}
	else if(GetPropID("hCoverPlate1.BG")==id)
	{
		WORD BG=0;
		if(m_pCirPlate && m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{
			CLDSParamPlate::DESIGN_PARAM param;
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P1"),&param))
			{
				if(param.cValueType==3)	//句柄+数值
				{
					WORD* pw_BGorBS=(WORD*)&param.value.uiAttribute;
					BG=*pw_BGorBS;
				}
			}
		}
		sText.Printf("%d",BG);
	}
	else if(GetPropID("hCoverPlate1.BS")==id)
	{
		WORD BS=0;
		if(m_pCirPlate && m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{
			CLDSParamPlate::DESIGN_PARAM param;
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P1"),&param))
			{
				if(param.cValueType==3)	//句柄+数值
				{
					WORD* pw_BGorBS=(WORD*)&param.value.uiAttribute;
					BS=*(pw_BGorBS+1);
				}
			}
		}
		sText.Printf("%d",BS);
	}
	else if(GetPropID("hCoverPlate2")==id)
	{
		if(m_pCirPlate==NULL||m_pCirPlate->m_iParamType!=TYPE_CIRCOVERPLATE)
			sText="";
		else
		{
			long hPlate=0;
			m_pCirPlate->GetDesignItemValue(KEY2C("P2"),&hPlate);
			sText.Printf("0X%X",hPlate);
		}
	}
	else if(GetPropID("hCoverPlate2.R")==id)
	{
		double R=0;
		if(m_pCirPlate && m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{
			CLDSParamPlate::DESIGN_PARAM param;
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P2"),&param))
			{
				if(param.cValueType==3)	//句柄+数值
					R=param.value.uiAttribute*0.1;
			}
		}
		sText.Printf("%.1f",R);
	}
	else if(GetPropID("hCoverPlate2.BG")==id)
	{
		WORD BG=0;
		if(m_pCirPlate && m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{
			CLDSParamPlate::DESIGN_PARAM param;
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P2"),&param))
			{
				if(param.cValueType==3)	//句柄+数值
				{
					WORD* pw_BGorBS=(WORD*)&param.value.uiAttribute;
					BG=*pw_BGorBS;
				}
			}
		}
		sText.Printf("%d",BG);
	}
	else if(GetPropID("hCoverPlate2.BS")==id)
	{
		WORD BS=0;
		if(m_pCirPlate && m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{
			CLDSParamPlate::DESIGN_PARAM param;
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P2"),&param))
			{
				if(param.cValueType==3)	//句柄+数值
				{
					WORD* pw_BGorBS=(WORD*)&param.value.uiAttribute;
					BS=*(pw_BGorBS+1);
				}
			}
		}
		sText.Printf("%d",BS);
	}
	else if(GetPropID("hCoverPlate3")==id)
	{
		if(m_pCirPlate==NULL||m_pCirPlate->m_iParamType!=TYPE_CIRCOVERPLATE)
			sText="";
		else
		{
			long hPlate=0;
			m_pCirPlate->GetDesignItemValue(KEY2C("P3"),&hPlate);
			sText.Printf("0X%X",hPlate);
		}
	}
	else if(GetPropID("hCoverPlate3.R")==id)
	{
		double R=0;
		if(m_pCirPlate && m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{
			CLDSParamPlate::DESIGN_PARAM param;
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P3"),&param))
			{
				if(param.cValueType==3)	//句柄+数值
					R=param.value.uiAttribute*0.1;
			}
		}
		sText.Printf("%.1f",R);
	}
	else if(GetPropID("hCoverPlate3.BG")==id)
	{
		WORD BG=0;
		if(m_pCirPlate && m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{
			CLDSParamPlate::DESIGN_PARAM param;
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P3"),&param))
			{
				if(param.cValueType==3)	//句柄+数值
				{
					WORD* pw_BGorBS=(WORD*)&param.value.uiAttribute;
					BG=*pw_BGorBS;
				}
			}
		}
		sText.Printf("%d",BG);
	}
	else if(GetPropID("hCoverPlate3.BS")==id)
	{
		WORD BS=0;
		if(m_pCirPlate && m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{
			CLDSParamPlate::DESIGN_PARAM param;
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P3"),&param))
			{
				if(param.cValueType==3)	//句柄+数值
				{
					WORD* pw_BGorBS=(WORD*)&param.value.uiAttribute;
					BS=*(pw_BGorBS+1);
				}
			}
		}
		sText.Printf("%d",BS);
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
	else if(GetPropID("para.j")==id)
	{
		if(para.j==0)
			sText.Copy("0.基准边");
		else
			sText.Copy("1.焊道线");
	}
	else if(GetPropID("para.A")==id)//alfa 定位角
		sText.Printf("%f",para.A);
	else if(GetPropID("para.B")==id)//beta 扇形角	
		sText.Printf("%f",para.B);
	else if(GetPropID("para.y")==id)//2WA型环向肋板倒角角度,单位°
		sText.Printf("%f",para.gama);
	else if(GetPropID("para.D")==id)//自由边距
		sText.Printf("%.0f",para.D);
	else if(GetPropID("para.d")==id)//内侧边距
		sText.Printf("%.0f",para.d);
	else if(GetPropID("para.H")==id)//台口高度
		sText.Printf("%.0f",para.H);
	else if (GetPropID("para.R1")==id) //流水孔1半径
		sText.Printf("%.0f",para.R1);
	else if (GetPropID("para.R2")==id) //流水孔2半径
		sText.Printf("%.0f",para.R2);
	else if(GetPropID("para.h")==id)//N侧台口高度
		sText.Printf("%.0f",para.h);
	else if(GetPropID("para.R")==id)//外圆半径
	{
		sText.Printf("%.0f",para.R);
		if(m_iPlateType==3&&para.nDHRelativeType==0&&para.R<EPS
			&&fabs(para.W-para.N)<EPS&&fabs(para.W-para.D)<EPS
			&&para.W>0&&fabs(para.H)<EPS&&m_pDatumTube&&m_pCirPlate)
		{	//3WA 以前版本顶部圆弧连接的肋板W==N==D H=0,当前版本仅保存R 
			//根据D计算外圆半径R wht 11-05-24
			sText.Printf("%.0f",para.D+para.V+m_pDatumTube->GetDiameter()*0.5);
		}
	}
	else if(GetPropID("innercir_type")==id)
	{	//环向板内圆类型 wht 11-05-27
		if(para.r<-EPS&&m_pDatumTube)
			sText.Printf("%g",m_pDatumTube->GetDiameter()*0.5);
		else if(para.r<EPS)
			sText="0";
		else
			sText.Printf("%g",para.r);
	}
	else if(GetPropID("edge_space")==id)
		sText.Printf("%g",para.fEdgeSpace);
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
		{
			if(m_iPlateType>1&&m_pDatumTube&&m_pCirPlate)	//环向肋板
			{
				GEPOINT vDaumAxisVec=m_pDatumTube->End()-m_pDatumTube->Start();
				if(vDaumAxisVec*m_pCirPlate->ucs.axis_z<0)
					sText.Printf("%f",-para.S);
				else
					sText.Printf("%f",para.S);
			}
			else
				sText.Printf("%f",para.S);
		}
	}
	else if(GetPropID("para.nCirCoverPlateType")==id)
	{	//封板类型
#ifdef AFX_TARG_ENU_ENGLISH
		//if(para.nCirCoverPlateType==2)
		//	sText.Copy("Two Simple Sealing Plate");
		//else 
		if(para.nCirCoverPlateType>0)
			sText.Copy("Y+ Cover Plate");
		else if(para.nCirCoverPlateType<0)
			sText.Copy("Y- Cover Plate");
		else 
			sText.Copy("Common Sealing Plate");
#else 
		//if(para.nCirCoverPlateType==2)
		//	sText.Copy("两块简化封板");
		//else 
		if(para.nCirCoverPlateType>0)
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
		sText.Printf("%.0f",para.W);
	else if(GetPropID("para.N")==id)		//第二块关联板支撑边宽
		sText.Printf("%.0f",para.N);
	else if(GetPropID("para.Y")==id)		//环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)
	{
		int side=SideOfCirRibPlateOnOutwardToBasicPlate();	//para.Y
#ifdef AFX_TARG_ENU_ENGLISH
		if(side>0)
			sText.Printf("Datum Plate Outside");
		else if(side<0)
			sText.Printf("Datum Plate Inside");
		else 
			sText.Printf("Default");
#else 
		if(side>0)
			sText.Printf("基板外侧");
		else if(side<0)
			sText.Printf("基板内侧");
		else //判断实际相对于基板的内外侧
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
					sText.Printf("1.启用D");
				else if(para.nDHRelativeType==2)
					sText.Printf("2.启用H");
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
			sText.Printf("1.Connect Plane Normal");
#else 
			sText.Printf("0.焊缝走向");
		else 
			sText.Printf("1.连接面法向");
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
		CString ss=sText;
		SimplifiedNumString(ss);
		StrCopy(valueStr,ss,nMaxStrBufLen);
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
		if(m_idPlateType==TYPE_SPECIAL_FL)
		{	//异型法兰
			pPropItem=oper.InsertEditPropItem(pProfileItem,"para.D","","",-1,TRUE);
			UpdatePropItemName(&m_xSubPropList,pPropItem);
			pItem=oper.InsertBtnPopMenuEditPropItem(pProfileItem,"para.r","","","0.贴合管壁|1.指定半径|2.指定内侧边距",-1,TRUE);
			if(para.r<0)
				pItem->m_lpNodeInfo->SetCheck(1);
			else if(para.r>0)
				pItem->m_lpNodeInfo->SetCheck(2);
		}
		else if(m_idPlateType==TYPE_COMMON_PLATE)
		{	//普通环向板
			pPropItem=oper.InsertBtnPopMenuEditPropItem(pProfileItem,"para.B","","","整圆",-1,TRUE);	//扇形角β
			if(fabs(para.B-360)<EPS)
			{
				pItem=oper.InsertBtnPopMenuEditPropItem(pPropItem,"innercir_type","","","0.指定半径|1.贴合钢壁",-1,TRUE);
				if(para.r<0)
					pItem->m_lpNodeInfo->SetCheck(2);
				else if(para.r>0)
					pItem->m_lpNodeInfo->SetCheck(1);
			}
			oper.InsertEditPropItem(pProfileItem,"para.R","","",-1,TRUE);	//外圆半径
		}
		else if(m_idPlateType==TYPE_CIR_BASE)
		{	//环形基座板
			oper.InsertEditPropItem(pProfileItem,"para.R","","",-1,TRUE);	//外圆半径
		}
	}
	else if(m_iPlateType==1)
	{	//环向封板
		if(para.nCirCoverPlateType==3)
		{	//直角封板
			oper.InsertEditPropItem(pProfileItem,"para.W","封板宽带W","",-1,TRUE);
			oper.InsertEditPropItem(pProfileItem,"para.N","封板长度H","",-1,TRUE);
		}
		else
		{
			//扇形角
			oper.InsertEditPropItem(pProfileItem,"para.B","","",-1,TRUE);
			//环形带宽
			oper.InsertEditPropItem(pProfileItem,"para.W","","",-1,TRUE);
			//台口高度
			oper.InsertEditPropItem(pProfileItem,"para.H","","",-1,TRUE);
			if(para.nCirCoverPlateType!=0)
				oper.InsertEditPropItem(pProfileItem,"para.N","延伸距离N","",-1,TRUE);
		}
	}
	else if(m_iPlateType==2)
	{	//直线型三边支撑环向肋板
		pPropItem=oper.InsertEditPropItem(pProfileItem,"para.W","","",-1,TRUE);
		UpdatePropItemName(&m_xSubPropList,pPropItem);
		oper.InsertEditPropItem(pProfileItem,"para.N","","",-1,TRUE);
		//
		pPropItem=oper.InsertCmbListPropItem(pProfileItem,"para.nDHRelativeType","","","",-1,TRUE);
		UpdatePropItemName(&m_xSubPropList,pPropItem);
		if(para.nDHRelativeType==0||para.nDHRelativeType==1)
		{	//自由边距
			pPropItem=oper.InsertEditPropItem(pProfileItem,"para.D","","",-1,TRUE);
			UpdatePropItemName(&m_xSubPropList,pPropItem);
		}
		if(para.nDHRelativeType==0||para.nDHRelativeType==2)
		{	//台口高度
			oper.InsertEditPropItem(pProfileItem,"para.H","","",-1,TRUE);
			oper.InsertEditPropItem(pProfileItem,"para.h","","",-1,TRUE);
		}
	}
	else if(m_iPlateType==3)
	{	//圆弧型三边支撑环向肋板
		m_propList.DeleteItemByPropId(GetPropID("para.nDHRelativeType"));
		if(para.nDHRelativeType!=0)
		{	
			pPropItem=oper.InsertEditPropItem(pProfileItem,"para.D","","",-1,TRUE);	//环形带宽D
			UpdatePropItemName(&m_xSubPropList,pPropItem);
			pPropItem=oper.InsertEditPropItem(pProfileItem,"para.W","","",-1,TRUE);	//焊缝长W
			UpdatePropItemName(&m_xSubPropList,pPropItem);
			oper.InsertEditPropItem(pProfileItem,"para.N","","",-1,TRUE);			//焊缝长N
			oper.InsertEditPropItem(pProfileItem,"para.H","","",-1,TRUE);	//台口高度
			oper.InsertEditPropItem(pProfileItem,"para.h","","",-1,TRUE);	//N侧台口高度
		}
		else //外圆半径Ro=环形带宽D+钢管半径
		{	//环形带宽D
			oper.InsertEditPropItem(pProfileItem,"para.D","环形带宽W","环形带宽W",-1,TRUE);
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
		oper.InsertEditPropItem(pProfileItem,"para.H","台口高度H","",-1,TRUE);
		if(para.D==0&&para.B>0)
			oper.InsertEditPropItem(pProfileItem,"para.h","台口高度h","",-1,TRUE);
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
	else if(m_iPlateType==7)
	{	//走道板
		oper.InsertEditPropItem(pProfileItem,"para.B","","",-1,TRUE);	//扇形角β
		oper.InsertEditPropItem(pProfileItem,"para.W","延伸距离W","",-1,TRUE);
		oper.InsertEditPropItem(pProfileItem,"para.H","","",-1,TRUE);	//台口高度H
		pPropItem=oper.InsertBtnPopMenuEditPropItem(pProfileItem,"edge_space","","","",-1,TRUE);
		pPropItem->SetReadOnly(para.fEdgeSpace<=0);
	}
	if(m_iPlateType<=5)
	{	//加工间隙
		oper.InsertEditPropItem(pProfileItem,"para.V","","",-1,TRUE);
	}
}

void CDesignCircularPlateDlg::UpdateCirPlateBasicProp(CPropTreeItem *pBasicItem/*=NULL*/)
{
	if(pBasicItem==NULL)
		pBasicItem=m_propList.FindItemByPropId(CDesignCircularPlateDlg::GetPropID("basicInfo"),NULL);
	if(pBasicItem==NULL)
		return;
	CPropTreeItem* pPropItem=NULL;
	CPropertyListOper<CDesignCircularPlateDlg> oper(&m_propList,this);
	//
	m_propList.DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.S"));
	m_propList.DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.nCirCoverPlateType"));
	m_propList.DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.bCreateMirPlate"));
	m_propList.DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("MirCoverPlateLabel"));
	m_propList.DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.biHoleType"));
	m_propList.DeleteItemByPropId(CDesignCircularPlateDlg::GetPropID("para.BH"));
	if(m_iPlateType==1)	//环向封板
	{	//封板位置
		pPropItem=oper.InsertCmbListPropItem(pBasicItem,"para.S","","","",-1,TRUE);
		UpdatePropItemName(&m_propList,pPropItem);
		pPropItem->SetReadOnly(!m_bNewCirPlate);
		//环向封板类型
		if(para.nCirCoverPlateType==1||para.nCirCoverPlateType==-1)
		{	//
			pPropItem=oper.InsertCmbListPropItem(pBasicItem,"para.nCirCoverPlateType","","","",-1,TRUE);
			UpdatePropItemName(&m_propList,pPropItem);
			pPropItem->SetReadOnly(!m_bNewCirPlate);
		}
		//
		if(fabs(para.B-180)<EPS&&para.nCirCoverPlateType==0&&m_bNewCirPlate)
		{
			pPropItem=oper.InsertCmbListPropItem(pBasicItem,"para.bCreateMirPlate","","","",-1,TRUE);
			if(para.bCreateMirPlate)
				oper.InsertEditPropItem(pPropItem,"MirCoverPlateLabel","构件编号","",-1,TRUE);
		}
	}
	//流水孔类型
	if(m_iPlateType>=1&&m_iPlateType<=5)
	{	
		pPropItem=oper.InsertCmbListPropItem(pBasicItem,"para.biHoleType","","","",-1,TRUE);
		if(para.biHoleType==0)	//圆孔
			oper.InsertEditPropItem(pPropItem,"para.BH","","",-1,TRUE);
	}
}
void CDesignCircularPlateDlg::UpdateCirPlateRelaProp(CPropTreeItem *pRelaInfoItem/*=NULL*/)
{
	if(pRelaInfoItem==NULL)
		pRelaInfoItem=m_propList.FindItemByPropId(CDesignCircularPlateDlg::GetPropID("RelaInfo"),NULL);
	if(pRelaInfoItem==NULL)
		return;
	CPropTreeItem* pPropItem=NULL,*pSonPropItem=NULL;
	CPropertyListOper<CDesignCircularPlateDlg> oper(&m_propList,this);
	m_propList.DeleteAllSonItems(pRelaInfoItem);
	//基准钢管
	pPropItem=oper.InsertBtnEditPropItem(pRelaInfoItem,"hDatumTube","","",-1,TRUE);
	pPropItem->SetReadOnly();
	//基准节点
	pPropItem=oper.InsertBtnEditPropItem(pRelaInfoItem,"hDatumNode","","",-1,TRUE);
	pPropItem->SetReadOnly();
	if(m_iPlateType==1)
	{	//封板
		oper.InsertBtnEditPropItem(pRelaInfoItem,"hRelaPart1","定位基板","",-1,TRUE);
		oper.InsertBtnEditPropItem(pRelaInfoItem,"hRelaPart2","定位基板2","",-1,TRUE);	
		//覆盖板
		if(m_pCirPlate)
		{
			long hCoverPlate=0;
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P0"),&hCoverPlate))
				oper.InsertBtnEditPropItem(pRelaInfoItem,"hCoverPlate0");
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P1"),&hCoverPlate))
				oper.InsertBtnEditPropItem(pRelaInfoItem,"hCoverPlate1");
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P2"),&hCoverPlate))
				oper.InsertBtnEditPropItem(pRelaInfoItem,"hCoverPlate2");
			if(m_pCirPlate->GetDesignItemValue(KEY2C("P3"),&hCoverPlate))
				oper.InsertBtnEditPropItem(pRelaInfoItem,"hCoverPlate3");
		}
	}
	else if(m_iPlateType==7)
	{
		pPropItem=oper.InsertBtnEditPropItem(pRelaInfoItem,"hRelaPart1","依赖角钢","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPlateType>1);
		oper.InsertButtonPropItem(pRelaInfoItem,"RelaBolt","关联螺栓","",-1,TRUE);	
	}
	else		
	{	//其他环板关联信息显示
		//关联钢板
		pPropItem=oper.InsertBtnEditPropItem(pRelaInfoItem,"hRelaPart1","","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPlateType>1);
		//3W型肋板需要关联钢板2
		if(m_iPlateType==2||m_iPlateType==3)
		{	
			pPropItem=oper.InsertBtnEditPropItem(pRelaInfoItem,"hRelaPart2","","",-1,TRUE);	
			pPropItem->SetReadOnly(m_iPlateType>1);
		}
		//普通环向板生成螺栓 wht 05-16
		if(m_iPlateType==0)
		{	
			pPropItem=oper.InsertEditPropItem(pRelaInfoItem,"CreateBolt","","",-1,TRUE);
			pPropItem->SetReadOnly(TRUE);
		}
		//环向封板,环向肋板和遮水板不需要选择关联杆件,遮水板的关联螺栓会自动引入不用选择 wht 11-05-11
		if((m_iPlateType==0&&m_idPlateType!=TYPE_CIR_BASE)	//异形法兰
			||(m_iPlateType==2&&para.nDHRelativeType==0)	//3WL 跨越连板
			||(m_iPlateType==3&&para.nDHRelativeType))	//3WA 跨越连板
		{	
			oper.InsertButtonPropItem(pRelaInfoItem,"RelaBolt","","",-1,TRUE);				//关联螺栓
			pPropItem=oper.InsertButtonPropItem(pRelaInfoItem,"RelaPole","","",-1,TRUE);	//关联杆件
			//关联构件
			int iIndex=1;
			for(CONNECT_DIRECTION *pVec=ConnVecList.GetFirst(TRUE);pVec;pVec=ConnVecList.GetNext(TRUE),iIndex++)
			{
				long idProp=CDesignCircularPlateDlg::GetPropID(CXhChar50("ConnDir%d",iIndex));
				pVec->feature=idProp; //连接方向中记录关联属性项id
				pSonPropItem=oper.InsertBtnEditPropItem(pPropItem,CXhChar50("ConnDir%d",iIndex),"","",-1,TRUE);
				pSonPropItem->SetReadOnly();
			}
		}
	}
}
void CDesignCircularPlateDlg::UpdateCirPlateLocationProp(CPropTreeItem *pLocationItem/*=NULL*/)
{
	if(pLocationItem==NULL)
		pLocationItem=m_propList.FindItemByPropId(CDesignCircularPlateDlg::GetPropID("location"),NULL);
	if(pLocationItem==NULL)
		return;
	CPropTreeItem* pPropItem=NULL;
	CPropertyListOper<CDesignCircularPlateDlg> oper(&m_propList,this);
	m_propList.DeleteAllSonItems(pLocationItem);	
	if(m_iPlateType!=1)
	{	//环向封板不需要输入法线方向
		//工作法线
		pPropItem=oper.InsertCmbListPropItem(pLocationItem,"ucs.axis_z","","","",-1,TRUE);
		//根据立波反应,用户要求设计完环板后能够修改法线方向 wxc-2018-10-29
		//pPropItem->SetReadOnly(!m_bNewCirPlate);	//查看参数时暂不支持修改法线方向 wht 11-05-13
		if(m_iPlateType!=6)
		{	//偏移类型
			oper.InsertCmbListPropItem(pLocationItem,"para.nOffsetType","","","",-1,TRUE);
			//位置偏移
			oper.InsertEditPropItem(pLocationItem,"para.S","","",-1,TRUE);
		}
		else	//遮水板
			oper.InsertEditPropItem(pLocationItem,"water.S","","",-1,TRUE);	
	}
	if((m_iPlateType==0 && ConnVecList.GetCount(TRUE)<=0)||
		(m_iPlateType==1 && para.nCirCoverPlateType==0))
	{	//0.普通环向板 1.环向封板
		if(m_iPlateType==0 && ConnVecList.GetCount(TRUE)<=0)
			oper.InsertCmbListPropItem(pLocationItem,"para.j","","","",-1,TRUE);
		pPropItem=oper.InsertEditPropItem(pLocationItem,"para.A","","",-1,TRUE);	//扇形起始角
		UpdatePropItemName(&m_propList,pPropItem);
	}
	else if(m_iPlateType>=1&&m_iPlateType<=5)	//m_iPlateType=0普通环板时para.Y如显示,在FireCircularPlateHelpStr更新未意图时会死机 wjh-2018.3.9
	{	//1.环向封板(简化)2.环向肋板3WL,3.环向肋板3WA,4.环向肋板2WL,5.环向肋板2WA
		//oper.InsertCmbListPropItem(pLocationItem,"para.Y","","","",-1,TRUE);	//肋板位置 (>0基准板外侧;=0自动;<0基准板内侧)
		pPropItem=oper.InsertBtnEditPropItem(pLocationItem,"para.Y","","",-1,TRUE);	//肋板位置 (>0基准板外侧;=0自动;<0基准板内侧)
		pPropItem->m_lpNodeInfo->m_sButtonName="...";
		pPropItem->SetReadOnly();
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
	//pPropItem=oper.InsertCmbListPropItem(pGroupItem,"type");
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"type");
	pPropItem->m_lpNodeInfo->m_sButtonName="...";
	pPropItem->SetReadOnly(!m_bNewCirPlate||m_bReadOnlyPlateType);
	UpdateCirPlateBasicProp(pGroupItem);
	//关联信息
	if(m_pCirPlate&&m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		pGroupItem=oper.InsertGrayButtonPropItem(pRoot,"RelaInfo");
	else
		pGroupItem=oper.InsertPropItem(pRoot,"RelaInfo");
	UpdateCirPlateRelaProp(pGroupItem);
	//装配方位
	pGroupItem=oper.InsertPropItem(pRoot,"location");
	UpdateCirPlateLocationProp(pGroupItem);
	//
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
			if(m_pCirPlate)
				UpdateCircularPlateConnVecList(m_pCirPlate);
			//更新当前项显示内容
			CXhChar50 sText;
			if(GetPropValueStr(pItem->m_idProp,sText)>0)
				m_propList.SetItemPropValue(pItem->m_idProp,sText);
		}
		pGroupItem=oper.InsertPropItem(pRoot,"ConnDir");
		//端距
		oper.InsertEditPropItem(pGroupItem,"ConnDir.end_space","","",-1,TRUE);
		//边距
		oper.InsertEditPropItem(pGroupItem,"ConnDir.high_space","","",-1,TRUE);
		//连接方向类型
		oper.InsertCmbListPropItem(pGroupItem,"ConnDir.style","","","",-1,TRUE);
		//法线设计参数
		//pGroupItem=oper.InsertPropItem(pRoot,"ConnDir.dir");
		pGroupItem=oper.InsertCmbListPropItem(pRoot,"ConnDir.dir.norm_style","","","",-1,TRUE);
		UpdateVectorPropList(pGroupItem,"ConnDir.dir",m_pCurSelConnVec->direct);
	}
	else if(pItem&&pItem->m_idProp==GetPropID("CreateBolt"))
	{
		//螺栓环向布置参数
		if(m_bNewCirPlate)
			pGroupItem=oper.InsertPropItem(pRoot,"m_xLayoutBolt");
		else if(m_pCirPlate)
			pGroupItem=oper.InsertGrayButtonPropItem(pRoot,"m_xLayoutBolt","","","布置螺栓");
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
	else if(pItem && pItem->m_idProp==GetPropID("para.biHoleType"))
	{	//流水孔参数
		if(m_pRelaPart1)
		{
			pGroupItem=oper.InsertPropItem(pRoot,"WaterleakHole_RP1","","",PIT_STATIC,0,"",-1,TRUE);
			if(para.biHoleType==1)	//环向封板钢板相应的流水孔半径
				oper.InsertEditPropItem(pGroupItem,"hRelaPart1.R","","",-1,TRUE);
			else if(para.biHoleType==0)
			{
				oper.InsertEditPropItem(pGroupItem,"hRelaPart1.BG","","",-1,TRUE);
				oper.InsertEditPropItem(pGroupItem,"hRelaPart1.BS","","",-1,TRUE);
			}
		}
		if(m_pRelaPart2)
		{
			pGroupItem=oper.InsertPropItem(pRoot,"WaterleakHole_RP2","","",PIT_STATIC,0,"",-1,TRUE);
			if(para.biHoleType==1)	//环向封板钢板相应的流水孔半径
				oper.InsertEditPropItem(pGroupItem,"hRelaPart2.R","","",-1,TRUE);
			else if(para.biHoleType==0)
			{
				oper.InsertEditPropItem(pGroupItem,"hRelaPart2.BG","","",-1,TRUE);
				oper.InsertEditPropItem(pGroupItem,"hRelaPart2.BS","","",-1,TRUE);
			}
		}
		if(m_pCoverPart0)
		{
			pGroupItem=oper.InsertPropItem(pRoot,"WaterleakHole_CP0","","",PIT_STATIC,0,"",-1,TRUE);
			if(para.biHoleType==1)	//半圆弧
				oper.InsertEditPropItem(pGroupItem,"hCoverPlate0.R","","",-1,TRUE);
			else if(para.biHoleType==0)
			{
				oper.InsertEditPropItem(pGroupItem,"hCoverPlate0.BG","","",-1,TRUE);
				oper.InsertEditPropItem(pGroupItem,"hCoverPlate0.BS","","",-1,TRUE);
			}
		}
		if(m_pCoverPart1)
		{
			pGroupItem=oper.InsertPropItem(pRoot,"WaterleakHole_CP1","","",PIT_STATIC,0,"",-1,TRUE);
			if(para.biHoleType==1)	//半圆弧
				oper.InsertEditPropItem(pGroupItem,"hCoverPlate1.R","","",-1,TRUE);
			else if(para.biHoleType==0)
			{
				oper.InsertEditPropItem(pGroupItem,"hCoverPlate1.BG","","",-1,TRUE);
				oper.InsertEditPropItem(pGroupItem,"hCoverPlate1.BS","","",-1,TRUE);
			}
		}
		if(m_pCoverPart2)
		{
			pGroupItem=oper.InsertPropItem(pRoot,"WaterleakHole_CP2","","",PIT_STATIC,0,"",-1,TRUE);
			if(para.biHoleType==1)	//半圆弧
				oper.InsertEditPropItem(pGroupItem,"hCoverPlate2.R","","",-1,TRUE);
			else if(para.biHoleType==0)
			{
				oper.InsertEditPropItem(pGroupItem,"hCoverPlate2.BG","","",-1,TRUE);
				oper.InsertEditPropItem(pGroupItem,"hCoverPlate2.BS","","",-1,TRUE);
			}
		}
		if(m_pCoverPart3)
		{
			pGroupItem=oper.InsertPropItem(pRoot,"WaterleakHole_CP3","","",PIT_STATIC,0,"",-1,TRUE);
			if(para.biHoleType==1)	//半圆弧
				oper.InsertEditPropItem(pGroupItem,"hCoverPlate3.R","","",-1,TRUE);
			else if(para.biHoleType==0)
			{
				oper.InsertEditPropItem(pGroupItem,"hCoverPlate3.BG","","",-1,TRUE);
				oper.InsertEditPropItem(pGroupItem,"hCoverPlate3.BS","","",-1,TRUE);
			}
		}
	}
	else 
	{	//显示外形参数
		pGroupItem=oper.InsertPropItem(pRoot,"profile");
		UpdatePlateProfileParamProp(pGroupItem);
	}
	m_xSubPropList.Redraw();
}

const int CDesignCircularPlateDlg::TYPE_COMMON_PLATE= 1;	//0.普通环向板
const int CDesignCircularPlateDlg::TYPE_SPECIAL_FL	= 2;	//0.异型法兰
const int CDesignCircularPlateDlg::TYPE_COVERPLATE2	= 3;	//1.环向封板
const int CDesignCircularPlateDlg::TYPE_COVERPLATE3	= 4;	//1.环向封板
const int CDesignCircularPlateDlg::TYPE_COVERPLATE4	= 5;	//1.环向封板(自由边为直线型的封板)
const int CDesignCircularPlateDlg::TYPE_3WL_1		= 6;	//2.3WL(三边支撑直线型)IDB_BMP_CIRCULAR_RIBPLATE5跨越链板
const int CDesignCircularPlateDlg::TYPE_3WL_2		= 7;	//2.3WL(三边支撑直线型)IDB_BMP_CIRCULAR_RIBPLATE1
const int CDesignCircularPlateDlg::TYPE_3WA_1		= 8;	//3.3WA(三边支撑圆弧型)IDB_BMP_CIRCULAR_RIBPLATE5跨越链板
const int CDesignCircularPlateDlg::TYPE_3WA_2		= 9;	//3.3WA(三边支撑圆弧型)IDB_BMP_CIRCULAR_RIBPLATE2
const int CDesignCircularPlateDlg::TYPE_2WL_1		=10;	//4.环向肋板2WL(两边支撑直线型)IDB_BMP_CIRCULAR_RIBPLATE3
const int CDesignCircularPlateDlg::TYPE_2WL_2		=11;	//4.环向肋板2WL(两边支撑直线型)IDB_BMP_CIRCULAR_RIBPLATE7
const int CDesignCircularPlateDlg::TYPE_2WA			=12;	//5.环向肋板2WA(两肢支撑圆弧型)IDB_BMP_CIRCULAR_RIBPLATE4
const int CDesignCircularPlateDlg::TYPE_SHIELD_WATER=13;	//6.遮水板IDB_BMP_SHIELDEDWATER_PLATE
const int CDesignCircularPlateDlg::TYPE_CIR_BASE	=14;	//6.圆底座
const int CDesignCircularPlateDlg::TYPE_AISLE_PLATE	=15;	//7.走道板
int CDesignCircularPlateDlg::_IntelliRecogPlateTypeId()
{
	if(m_iPlateType==0)			
	{
		if(ConnVecList.GetCount(TRUE)>0)
			m_idPlateType=TYPE_SPECIAL_FL;	//异形法兰
		else
		{
			if(para.r==0)
				m_idPlateType=TYPE_CIR_BASE;	//环形底座板
			else
				m_idPlateType=TYPE_COMMON_PLATE;//普通环向板
		}
	}
	else if(m_iPlateType==1)	//1.环向封板
	{
		if(para.nCirCoverPlateType==0)
			m_idPlateType=TYPE_COVERPLATE2;	//圆弧封板
		else if(abs(para.nCirCoverPlateType)==1||abs(para.nCirCoverPlateType)==2)
			m_idPlateType=TYPE_COVERPLATE3;	//简化封板
		else if(para.nCirCoverPlateType==3)
			m_idPlateType=TYPE_COVERPLATE4;	//直角封板
	}
	else if(m_iPlateType==2)	//2.环向肋板3WL(三边支撑直线型)
	{
		if(para.nDHRelativeType==0)	//跨越链板	//if(para.D>0&&para.H>0)//ConnVecList.IsNeedBend())	//跨越链板
			m_idPlateType=TYPE_3WL_1;
		else
			m_idPlateType=TYPE_3WL_2;
	}
	else if(m_iPlateType==3)	//3.环向肋板3WA(三边支撑圆弧型)
	{
		if(para.nDHRelativeType!=0)	//跨越链板 //if(para.D>0&&para.H>0)//(ConnVecList.IsNeedBend()) //跨越链板
			m_idPlateType=TYPE_3WA_1;
		else	
			m_idPlateType=TYPE_3WA_2;
	}
	else if(m_iPlateType==4)	//4.环向肋板2WL(两边支撑直线型)
	{
		if(para.D>0)
			m_idPlateType=TYPE_2WL_1;
		else //if(para.B>=0)
			m_idPlateType=TYPE_2WL_2;
	}
	else if(m_iPlateType==5)	//5.环向肋板2WA(两肢支撑圆弧型)
		m_idPlateType=TYPE_2WA;
	else if(m_iPlateType==6)	//6.遮水板
		m_idPlateType=TYPE_SHIELD_WATER;
	else if(m_iPlateType==7)
		m_idPlateType=TYPE_AISLE_PLATE;
	else
		m_idPlateType=-1;
	return m_idPlateType;
}
int CDesignCircularPlateDlg::get_idPlateType()
{
	if(m_idPlateType>0)
		return m_idPlateType;
	_IntelliRecogPlateTypeId();
	return m_idPlateType;
}
int CDesignCircularPlateDlg::set_idPlateType(int idType)
{
	if(idType>0)
		m_idPlateType=idType;
	if(m_idPlateType==TYPE_COMMON_PLATE||m_idPlateType==TYPE_SPECIAL_FL)
	{
		m_iPlateType=0;	//普通环向板或异型法兰
		para.r=-1;	//内圆根据钢管生成
	}
	else if(m_idPlateType==TYPE_COVERPLATE2||m_idPlateType==TYPE_COVERPLATE3||m_idPlateType==TYPE_COVERPLATE4)
	{
		m_iPlateType=1;	//1.环向封板
		if(m_iPlateType==TYPE_COVERPLATE2)
			para.nCirCoverPlateType=0;
		else if(m_idPlateType==TYPE_COVERPLATE3)
		{
			if(m_pRelaPart2!=NULL&&m_pRelaPart2->GetClassTypeId()==CLS_PLATE)
				para.nCirCoverPlateType=2;
			else
				para.nCirCoverPlateType=1;
		}
		else if(m_idPlateType==TYPE_COVERPLATE4)
			para.nCirCoverPlateType=3;
	}
	else if(m_idPlateType==TYPE_3WL_1||m_idPlateType==TYPE_3WL_2)
	{
		m_iPlateType=2;	//2.环向肋板3WL(三边支撑直线型)
		para.nDHRelativeType=(m_idPlateType==TYPE_3WL_1)?0:1;	//or 2;
	}
	else if(m_idPlateType==TYPE_3WA_1||m_idPlateType==TYPE_3WA_2)
	{
		m_iPlateType=3;	//3.环向肋板3WA(三边支撑圆弧型)
		para.nDHRelativeType=(m_idPlateType==TYPE_3WA_2)?0:1;	//or 2;
	}
	else if(m_idPlateType==TYPE_2WL_1||m_idPlateType==TYPE_2WL_2)
	{
		m_iPlateType=4;	//4.环向肋板2WL(两边支撑直线型)
		if(m_idPlateType==TYPE_2WL_1)
		{
			para.B=0;
			if(para.D<=EPS)
				para.D=max(para.H+20,50);
		}
		else if(m_idPlateType==TYPE_2WL_2)
		{
			para.D=0;
			if(para.B<=EPS)
				para.B=30;
		}
	}
	else if(m_idPlateType==TYPE_2WA)
		m_iPlateType=5;	//5.环向肋板2WA(两肢支撑圆弧型)
	else if(m_idPlateType==TYPE_SHIELD_WATER)
		m_iPlateType=6;	//6.遮水板
	else if(m_idPlateType==TYPE_AISLE_PLATE)
		m_iPlateType=7;	//7.走道板
	else //if(m_idPlateType==TYPE_CIR_BASE)
	{
		m_iPlateType=0;	//0.基座板
		para.B=360;
		para.r=0;	//无内圆
	}
	CIRCULAR_PARAM* pMemoryTypeParam= m_idPlateType<=0?NULL:_localCirTypeParams.GetValue(m_idPlateType);
	if(pMemoryTypeParam)
		para=*pMemoryTypeParam;
	return m_idPlateType;
}
void CDesignCircularPlateDlg::UpdateSketchMap(BOOL bShowWaterHole/*=FALSE*/)
{
	m_xCurPlateBmp.Detach();
	_IntelliRecogPlateTypeId();
	//if(m_idPlateType==TYPE_COMMON_PLATE)
	//	m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_PLATE);
	//else if(m_idPlateType==TYPE_SPECIAL_FL)
	//	m_xCurPlateBmp.LoadBitmap(IDB_BMP_SPECIAL_PROFILE_FL);
	if(bShowWaterHole)
		m_xCurPlateBmp.LoadBitmap(IDB_BMP_GY_HOLE2);
	else if(m_iPlateType==0)			
	{
		if(m_idPlateType==TYPE_CIR_BASE)
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_BASE_PLATE);
		else if(ConnVecList.GetCount(TRUE)>0)	//异形法兰
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_SPECIAL_PROFILE_FL);
		else				//普通环向板
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_PLATE);
	}
	else if(m_iPlateType==1)	//1.环向封板
	{
		if(para.nCirCoverPlateType==0)
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_COVERPLATE2);	//将来完善后应改为IDB_BMP_CIRCULAR_COVERPLATE wjh-2018.2.11
		else if(abs(para.nCirCoverPlateType)==1||abs(para.nCirCoverPlateType)==2) 
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_COVERPLATE3);
		else if(para.nCirCoverPlateType==3)
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_COVERPLATE4);
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
	else if(m_iPlateType==7)
		m_xCurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE8);
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
		m_pCirPlate->ConnDirectList.SetValue(connectVec.serial,connectVec);
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

static CLDSParamPlate* GetXEndParamPlateByBolt(CLDSBolt *pBolt)
{
	CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(pBolt->des_base_pos.hPart,CLS_PLATE);
	if(pPlate==NULL||pPlate->designInfo.origin.datum_pos_style!=9)
		return NULL;
	CLDSParamPlate *pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pPlate->designInfo.origin.des_para.hPart,CLS_PARAMPLATE);
	if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
	{
		pParamPlate->feature=pPlate->handle;
		return pParamPlate;
	}
	else
		return NULL;
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
		if(pPart=console.FromPartHandle(m_hPickObj,CLS_PLATE,CLS_PARAMPLATE,CLS_LINEANGLE))
		{
			m_pRelaPart1=pPart;
			InitProfilePara();	//初始化外形参数
			InitWaterleakHolePara(pPart);
			//PreviewCircularPlate();	由于后续ModifyPlateParam会触发预览生成代码，故此处不需要提前Preview wjh-2018.3.7
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
			InitWaterleakHolePara(pPart);
			//PreviewCircularPlate();	由于后续ModifyPlateParam会触发预览生成代码，故此处不需要提前Preview wjh-2018.3.7
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
	else if(m_idSubEventProp==0&&GetEventPropId()>=GetPropID("hCoverPlate0")&&GetEventPropId()<=GetPropID("hCoverPlate3"))
	{
		pPart=console.FromPartHandle(m_hPickObj,CLS_PLATE,CLS_PARAMPLATE);
		if(pPart==NULL||m_pCirPlate==NULL||m_pCirPlate->m_iParamType!=TYPE_CIRCOVERPLATE)
			return;	//
		UINT uiAttribute=0;
		WORD* pw_BGorBS=(WORD*)&uiAttribute;
		*pw_BGorBS	  =25;
		*(pw_BGorBS+1)=25;
		if(para.biHoleType==1)
			uiAttribute=_InitWaterHoleRadiusByThick(pPart->Thick);
		if(GetEventPropId()==GetPropID("hCoverPlate0"))
		{
			m_pCoverPart0=pPart;
			ModifyPlateParam(KEY2C("P0"),m_pCoverPart0->handle,uiAttribute,TRUE);
		}
		else if(GetEventPropId()==GetPropID("hCoverPlate1"))
		{
			m_pCoverPart1=pPart;
			ModifyPlateParam(KEY2C("P1"),m_pCoverPart1->handle,uiAttribute,TRUE);
		}
		else if(GetEventPropId()==GetPropID("hCoverPlate2"))
		{
			m_pCoverPart2=pPart;
			ModifyPlateParam(KEY2C("P2"),m_pCoverPart2->handle,uiAttribute,TRUE);
		}
		else if(GetEventPropId()==GetPropID("hCoverPlate3"))
		{
			m_pCoverPart3=pPart;
			ModifyPlateParam(KEY2C("P3"),m_pCoverPart3->handle,uiAttribute,TRUE);
		}
	}
	else if(m_idSubEventProp==0&&GetEventPropId()==GetPropID("RelaInfo"))
	{
		if(pPart=console.FromPartHandle(m_hPickObj,CLS_PLATE,CLS_PARAMPLATE))
		{
			if((m_pRelaPart1&&m_pRelaPart1->handle==m_hPickObj)||(m_pRelaPart2&&m_pRelaPart2->handle==m_hPickObj))
			{
				AfxMessageBox("不能重复添加覆盖钢板");
				return;
			}
			CLDSGeneralPlate* pCoveredPlate=(CLDSGeneralPlate*)pPart;
			if(m_pCirPlate&&m_pCirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			{
				int idle=-1;
				CLDSParamPlate::DESIGN_PARAM itemparam;
				char szKeyStr[3]="P0";
				CXhString keystr(szKeyStr,3);;
				for(int i=0;i<4;i++)
				{
					keystr[1]='0'+i;
					BOOL bExist=m_pCirPlate->GetDesignItemValue(KEY2C(keystr),&itemparam);
					if(bExist && itemparam.hValue<0X20)
						bExist=FALSE;
					if(!bExist&&idle<0)
						idle=i;
					else if(bExist&&itemparam.hValue==m_hPickObj)
					{
						idle=-1;
						AfxMessageBox("不能重复添加覆盖钢板");
						break;
					}
				}
				if(idle<0||idle>3)
					return;
				UINT uiAttribute=0;
				if(para.biHoleType==1)
					uiAttribute=_InitWaterHoleRadiusByThick(pCoveredPlate->Thick);
				else
				{
					WORD* pw_BGorBS=(WORD*)&uiAttribute;
					*pw_BGorBS	  =25;
					*(pw_BGorBS+1)=25;
				}
				if(idle==0)
				{
					m_pCoverPart0=pCoveredPlate;
					ModifyPlateParam(KEY2C("P0"),m_pCoverPart0->handle,uiAttribute,TRUE);
				}
				else if(idle==1)
				{	
					m_pCoverPart1=pCoveredPlate;
					ModifyPlateParam(KEY2C("P1"),m_pCoverPart1->handle,uiAttribute,TRUE);
				}
				else if(idle==2)
				{	
					m_pCoverPart2=pCoveredPlate;
					ModifyPlateParam(KEY2C("P2"),m_pCoverPart2->handle,uiAttribute,TRUE);
				}
				else if(idle==3)
				{	
					m_pCoverPart3=pCoveredPlate;
					ModifyPlateParam(KEY2C("P3"),m_pCoverPart3->handle,uiAttribute,TRUE);
				}
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
			BOOL bUpdateNormParam=TRUE;
			typedef CLDSParamPlate* CLDSParamPlatePtr;
			CLDSParamPlatePtr platePtrArr[2]={NULL,NULL};
			for(CLsRef* pLsRef=LsRefList.GetFirst();pLsRef;pLsRef=LsRefList.GetNext())
			{
				m_pCirPlate->AppendLsRef(*pLsRef);
				if(bUpdateNormParam)
				{
					CLDSParamPlate *pParamPlate=GetXEndParamPlateByBolt(pLsRef->GetLsPtr());
					bUpdateNormParam=(pParamPlate!=NULL);
					if(platePtrArr[0]==NULL)
						platePtrArr[0]=pParamPlate;
					else if(platePtrArr[1]==NULL&&platePtrArr[0]!=pParamPlate)
						platePtrArr[1]=pParamPlate;
				}
			}
			if( bUpdateNormParam&&platePtrArr[0]!=NULL&&
				(m_iPlateType==2&&para.nDHRelativeType==0)||	//3WL D,H为独立参数 wht 11-05-11
				(m_iPlateType==3&&para.nDHRelativeType))		//3WA D,H为独立参数
			{
				for(int i=0;i<2;i++)
				{
					if(platePtrArr[i]==NULL)
						continue;
					int iVec=-1;
					if(m_pRelaPart1&&m_pRelaPart1->GetClassTypeId()==CLS_PLATE)
					{
						if(((CLDSPlate*)m_pRelaPart1)->designInfo.FromHandle(platePtrArr[i]->m_hPartWeldParent)!=NULL)
							iVec=1;
					}
					if(m_pRelaPart2&&m_pRelaPart2->GetClassTypeId()==CLS_PLATE)
					{
						if(((CLDSPlate*)m_pRelaPart2)->designInfo.FromHandle(platePtrArr[i]->m_hPartWeldParent)!=NULL)
							iVec=2;
					}
					if(iVec<0)
						continue;
					CONNECT_DIRECTION *pVec=ConnVecList.GetRelaConnInfo(iVec);
					if(pVec==NULL)
					{
						pVec=ConnVecList.Append();
						pVec->serial=(BYTE)iVec;
					}
					pVec->style=1;
					pVec->direct.norm_style=3;
					CLsRef *pLsRef=platePtrArr[i]->GetFirstLsRef();
					if(pLsRef)
					{
						LSSPACE_STRU lsSpace;
						GetLsSpace(lsSpace,pLsRef->GetLsPtr()->get_d());
						pVec->high_space=pVec->end_space=(short)lsSpace.EndSpace;
					}
					if(console.FromPartHandle(platePtrArr[i]->feature))
						pVec->direct.hVicePart=platePtrArr[i]->feature;
				}
				UpdateCircularPlateConnVecList(m_pCirPlate);
			}
			ReDesignPlate();
		}
	}
}

BOOL CDesignCircularPlateDlg::IsValidParam(BOOL bCheckPropfileParam/*=TRUE*/)
{
	BOOL bRet=FALSE;
	CLDSPlate *pTransPlate=NULL;
	if(m_pRelaPart1&&m_pRelaPart1->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)m_pRelaPart1)->IsTransPlate())
		pTransPlate=(CLDSPlate*)m_pRelaPart1;	//第一块钢板为贯通板 wht 11-06-03
	if(m_iPlateType!=7&&m_pDatumNode==NULL)
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
			if(!m_pRelaPart1->IsPlate()||
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
	else if(m_pDatumTube&&m_pDatumNode)
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
	if(bRet && bCheckPropfileParam)
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
			//else if(para.H<=0)
				//AfxMessageBox("Circular sealing plate's param H is illega！");
			else if(para.B>=360)	//不支持生成大于等于360的环向封板 wht 11-05-31
				AfxMessageBox("Don't support to create circular sealing plate which is binger than 360！");
#else 
			if(para.B<=0)
				AfxMessageBox("环向封板扇形角β为非法数据！");
			else if(para.W<=0)
				AfxMessageBox("环向封板环形带宽W为非法数据！");
			//else if(para.H<=0)	//支持H为0
				//AfxMessageBox("环向封板台口高度H为非法数据！");
			else if(para.B>=360)	//不支持生成大于等于360的环向封板 wht 11-05-31
				AfxMessageBox("不支持生成大于等于360的环向封板！");
#endif
			else 
				bRet=TRUE;
		}
		else if(m_iPlateType==2)
		{	//3WL肋板
			BOOL bNeedBend=ConnVecList.IsNeedBend();
			if(!bNeedBend&&((para.nDHRelativeType==1&&para.D<=0)||(para.nDHRelativeType==2&&para.H<=0)))
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The free margin D is too small！");
#else 
				AfxMessageBox("自由边距D或台阶高度H不能小于0！");
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
			if(para.nDHRelativeType==0&&para.R<=0&&para.D<=0)
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
		else if(m_iPlateType==7)
		{
			if(para.B<=0)
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The shelter plate's outer diameter B is illegal！");
#else 
				AfxMessageBox("走道板夹角B为非法数据！");
#endif
			else if(para.W<=0)
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The shelter plate's outer diameter W is illegal！");
#else 
				AfxMessageBox("走道板环型带宽W为非法数据！");
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
	DelWorkPlaneSketch();
	g_pSolidDraw->DelEnt(_previewParamPlate.handle);
	if(!IsValidParam())
		return;	//检查参数合法性
	if(m_pCirPlate)
	{
		design_axis_z.UpdateVector(console.GetActiveModel());
		m_pCirPlate->ucs.axis_z=design_axis_z.vector;	//Z轴方向
	}
	if(m_pCirPlate==&_previewParamPlate)
		m_pCirPlate=NULL;
	CDialog::OnOK();
}

//自动计算W,N参数
BOOL CDesignCircularPlateDlg::AutoCalWN()
{
	if(m_pCirPlate&&para.G<=0&&m_pCirPlate->m_iParamType==TYPE_CIRRIBPLATE)
		return FALSE;	//修改参数不重新计算顶部圆弧连接环向板的W,N长度(3WA) wht 11-05-24
	if(m_iPlateType<2||m_iPlateType>5)
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
		GEPOINT vertice,vertice_vec;
		if(ptrVertexList[i]->GetNodeNum()>0)
		{
			vertice=ptrVertexList[i]->GetFirst()->vertex;
			coord_trans(vertice,pDatumPlate[i]->ucs,TRUE);
			vertice_vec=vertice-ucs.origin;
			if(vertice_vec*axis_x[i]<0)
				axis_x[i]*=-1.0;
		}
	}
	connVec1=axis_x[0];
	connVec2=axis_x[1];
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
				ProjectPoint(base_pos,line.startPt,pDatumPlate[i]->ucs.axis_z);
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
//3WL环向肋板需自动计算DH
BOOL CDesignCircularPlateDlg::AutoCalDH()
{
	if(m_iPlateType!=2)
		return FALSE;
	if(para.nDHRelativeType==0)
	{	//同时启用D和H，作为3WA进行处理
		para.H=30;
		para.D=50;
		para.h=para.H;
	}
	else if(para.nDHRelativeType==1)
	{	//关联且启用D，根据D参数进行外形设计
		para.H=0;
		para.D=50;
		para.h=para.H;
	}
	else if(para.nDHRelativeType==2)
	{	//关联且启用H，根据H参数进行外形设计
		double r=m_pDatumTube->GetDiameter()*0.5;
		f3dPoint vec,vec1,vec2,pt,pt_w,pt_h;
		vec=connVec1-connVec2;
		normalize(vec);
		vec1=vec;
		RotateVectorAroundVector(vec1,Pi/2.0,plate_ucs.axis_z);
		pt=plate_ucs.origin+vec1*(r+20);
		pt_w=plate_ucs.origin+connVec1*para.W;
		vec2=connVec1;
		normalize(vec2);
		RotateVectorAroundVector(vec2,Pi/2.0,plate_ucs.axis_z);
		Int3dll(pt_w,pt_w+vec2*10,pt,pt+vec*10,pt_h);
		if(!pt_h.IsZero())
			para.H=(int)DISTANCE(pt_w,pt_h);
		else
			para.H=30;
		para.D=0;
		para.h=para.H;
	}
	return TRUE;
}
//初始化环向板板参数
void CDesignCircularPlateDlg::InitCirPlatePara()
{
	m_cMaterial=(m_pDatumTube!=NULL)?m_pDatumTube->cMaterial:m_pDatumSphere->cMaterial;
	m_sSeg=(m_pDatumTube!=NULL)?m_pDatumTube->iSeg.ToString():m_pDatumSphere->iSeg.ToString();
	InitProfilePara();
	if(m_pCirPlate==NULL)
		m_pCirPlate=&_previewParamPlate;
	InitWaterleakHolePara();	//要求m_pCirPlate!=NULL才能正确初始化，故须先赋值m_pCirPlate==NULL情况 wjh-2018.3.1
}
void CDesignCircularPlateDlg::PreviewCircularPlate()
{
	CUndoOperObject undo(&Ta);
	CreateCircularPlate(m_pCirPlate);
}
void CDesignCircularPlateDlg::InitProfilePara()	//初始化钢板外形参数
{
	if(m_pDatumTube)
	{	//存在基准钢管且未设置环向板法线方向时设置法线方向
		para.R=m_pDatumTube->GetDiameter()*0.5+para.D;
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
	CIRCULAR_PARAM* pMemoryTypeParam= m_idPlateType<=0?NULL:_localCirTypeParams.GetValue(m_idPlateType);
	if(pMemoryTypeParam!=NULL&&m_iPlateType!=6)
	{
		para=*pMemoryTypeParam;
		if(m_iPlateType==3&&para.nDHRelativeType==0)
		{	//3.肋板3WA(根据环形带宽D自动计算R)
			AutoCalWN();
			para.D=(para.W<=0)?80:para.W;
			para.W=para.N=para.H=para.h=para.R=0;
		}
		else if(m_iPlateType==1)
		{
			if(m_idPlateType==TYPE_COVERPLATE2)
				para.nCirCoverPlateType=0;
			else if(m_idPlateType==TYPE_COVERPLATE3)
			{
				if(m_pRelaPart2!=NULL&&m_pRelaPart2->GetClassTypeId()==CLS_PLATE)
					para.nCirCoverPlateType=2;
				else
					para.nCirCoverPlateType=1;
			}
			else if(m_idPlateType==TYPE_COVERPLATE4)
				para.nCirCoverPlateType=3;
		}
	}
	else if(m_iPlateType==0)
	{	//普通环向板
		para.B=360;
		para.D=80;
		para.R=m_pDatumTube->GetDiameter()*0.5+para.D;
		para.j=m_pDatumTube->m_bHasWeldRoad?1:0;
	}	
	else if(m_iPlateType==1)
	{	//封板
		para.A = 0.0;
		para.H = para.h = 30.0;
		para.W = 60.0;
		para.B=90;
		if(m_idPlateType==TYPE_COVERPLATE2)
		{
			para.nCirCoverPlateType=0;
			para.B=(m_pRelaPart2)?180:90;
		}
		else if(m_idPlateType==TYPE_COVERPLATE3)
		{
			if(m_pRelaPart2!=NULL&&m_pRelaPart2->GetClassTypeId()==CLS_PLATE)
				para.nCirCoverPlateType=2;
			else
				para.nCirCoverPlateType=1;
			para.N = 20.0;
		}
		else if(m_idPlateType==TYPE_COVERPLATE4)
		{
			para.nCirCoverPlateType=3;	
			para.N = 100.0;
		}
	}
	else if(m_iPlateType==5)
	{	//2WA
		para.B=45;
		para.W=100;
		para.H=30;
	}
	else if(m_iPlateType==7)
	{	//初始化走道板参数Aisle
		para.B=180;
		para.W=60;
		para.H=0;
		para.fEdgeSpace=30;
	}
	else if(m_iPlateType==6&&m_pDatumTube)
	{	//初始化遮水板参数
		if(m_pDatumTube->pStart==m_pDatumNode)
			water.S=m_pDatumTube->startOdd();
		else if(m_pDatumTube->pEnd==m_pDatumNode)
			water.S=m_pDatumTube->endOdd();
		if(m_pRelaPart1&&m_pRelaPart1->IsPlate())
			water.S+=m_pRelaPart1->GetThick()*0.5;
		//
		water.W=m_pDatumTube->GetDiameter();
		if(pMemoryTypeParam==NULL)
		{
			water.K=10;
			water.H=0.5*thick;
			water.A=60;
		}
		//else if(pMemoryTypeParam!=NULL)
		//	;遮水板时pMemoryTypeParam存在仅表示继承之前的遮水板参数water，而不重新初始参数 wjh-2018.2.5

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
			AutoCalDH();
			para.G=(para.nDHRelativeType==0)?0:1;
		}
		else if(m_iPlateType==3)
		{	//3.肋板3WA
			if(para.nDHRelativeType==0)
			{	//不启用DH(根据环形带宽D自动计算R)
				para.D=(para.W<=0)?80:para.W;
				para.W=para.N=para.H=para.h=para.R=0;
			}
			else	
				para.D=80;				//启用DH
			para.G=1;
		}
		else if(m_iPlateType==4)
		{	//4.肋板2WL,可通过指定自由边距或扇形角两种方式定义外形范围
			para.H=para.h=30;
			if(para.D==0)
			{
				if(para.B<=0)
					para.B=30;
			}
			else
				para.B=0;
		}
	}
	//根据m_idPlateType对设计参数para进行合法化修正
	if(m_idPlateType==TYPE_CIR_BASE)
	{
		para.r=0;
		para.B=360;
	}
	if(m_idPlateType>0)
		_localCirTypeParams.SetValue(m_idPlateType,para);
}
UINT CDesignCircularPlateDlg::_InitWaterHoleRadiusByThick(double fPlateThick)
{
	//焊接半圆孔半径(旧代码中半圆型流水孔的默认值一直都是25mm半径） wjh-2018.2.11
	UINT uiAttribute=250;
	//if(m_iPlateType>=1&&m_iPlateType<=5)//环向封板及肋板上的焊接半圆孔半径需根据依赖钢板厚度进行初始化(宏源提供资料) wxc-2018.2.24
	if(m_iPlateType==1)
	{	//环向封板的焊接半圆孔半径需根据依赖钢板厚度进行初始化(宏源提供资料,经常利波核对确认) wjh-2018.3.10
		if(fPlateThick<=12)
			uiAttribute=200;
		else if(fPlateThick<=16)
			uiAttribute=250;
		else if(fPlateThick>=18)
			uiAttribute=300;
	}
	return uiAttribute;
}
void CDesignCircularPlateDlg::InitWaterleakHolePara(CLDSPart* pSpecRelyVertPlate/*=NULL*/)
{
	if(m_pCirPlate==NULL)
		return;
	if(m_iPlateType<1||m_iPlateType>5)
		return;
	if(para.biHoleType==0)	//圆孔
		m_pCirPlate->SetDesignItemValue(KEY2C("BH"),para.BH);
	else if(para.biHoleType==1)	//半圆孔
		m_pCirPlate->SetDesignItemValue(KEY2C("BH"),0.0);
	else //if(para.biHoleType==2)	//无
		m_pCirPlate->SetDesignItemValue(KEY2C("BH"),-1.0);
	if(para.biHoleType==2)
		return;
	//根据依赖钢板厚度初始化焊接半圆孔半径(宏源提供资料) wxc-2018.2.24
	UINT uiAttribute=0;
	WORD* pw_BGorBS=(WORD*)&uiAttribute;
	*pw_BGorBS	  =25;
	*(pw_BGorBS+1)=25;
	if(m_pRelaPart1&&(pSpecRelyVertPlate==NULL||pSpecRelyVertPlate==m_pRelaPart1))
	{
		if(para.biHoleType==1)
			uiAttribute=_InitWaterHoleRadiusByThick(m_pRelaPart1->Thick);
		if(m_iPlateType==1)
			ModifyPlateParam('F',m_pRelaPart1->handle,uiAttribute,FALSE);
		else if(m_iPlateType<=5)
			ModifyPlateParam('E',m_pRelaPart1->handle,uiAttribute,FALSE);
	}
	if(m_pRelaPart2&&(pSpecRelyVertPlate==NULL||pSpecRelyVertPlate==m_pRelaPart2))
	{
		if(para.biHoleType==1)
			uiAttribute=_InitWaterHoleRadiusByThick(m_pRelaPart2->Thick);
		if(m_iPlateType==1&&para.nCirCoverPlateType==0)
			ModifyPlateParam('G',m_pRelaPart2->handle,uiAttribute,FALSE);
		else if(m_iPlateType>1&&m_iPlateType<=5)
			ModifyPlateParam('F',m_pRelaPart2->handle,uiAttribute,FALSE);
	}
	if(m_pCoverPart0&&(pSpecRelyVertPlate==NULL||pSpecRelyVertPlate==m_pCoverPart0))
	{
		if(para.biHoleType==1)
			uiAttribute=_InitWaterHoleRadiusByThick(m_pCoverPart0->Thick);
		ModifyPlateParam(KEY2C("P0"),m_pCoverPart0->handle,uiAttribute,FALSE);
	}
	if(m_pCoverPart1&&(pSpecRelyVertPlate==NULL||pSpecRelyVertPlate==m_pCoverPart0))
	{
		if(para.biHoleType==1)
			uiAttribute=_InitWaterHoleRadiusByThick(m_pCoverPart1->Thick);
		ModifyPlateParam(KEY2C("P1"),m_pCoverPart1->handle,uiAttribute,FALSE);
	}
	if(m_pCoverPart2&&(pSpecRelyVertPlate==NULL||pSpecRelyVertPlate==m_pCoverPart0))
	{
		if(para.biHoleType==1)
			uiAttribute=_InitWaterHoleRadiusByThick(m_pCoverPart2->Thick);
		ModifyPlateParam(KEY2C("P2"),m_pCoverPart2->handle,uiAttribute,FALSE);
	}
	if(m_pCoverPart3&&(pSpecRelyVertPlate==NULL||pSpecRelyVertPlate==m_pCoverPart0))
	{
		if(para.biHoleType==1)
			uiAttribute=_InitWaterHoleRadiusByThick(m_pCoverPart3->Thick);
		ModifyPlateParam(KEY2C("P3"),m_pCoverPart3->handle,uiAttribute,FALSE);
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
void CDesignCircularPlateDlg::UpdateCircularPlateConnVecList(CLDSParamPlate *pCirPlate)
{
	//关联方向
	CONNECT_DIRECTION *pVec=NULL,*pNewVec=NULL;
	pCirPlate->ConnDirectList.Empty();
	//至少应存在一个标识为1的连接方向,保存基准面法线
	pNewVec=pCirPlate->ConnDirectList.Add(1);
	pNewVec->serial=1;
	pNewVec->style=1;
	pNewVec->direct=design_axis_z;
	BOOL bNeedBend=ConnVecList.IsNeedBend();
	for(pVec=ConnVecList.GetFirst();pVec;pVec=ConnVecList.GetNext())
	{	
		pNewVec=pCirPlate->ConnDirectList.Add(pVec->serial);
		pNewVec->serial=pVec->serial;
		pNewVec->style=pVec->style;
		pNewVec->end_space=pVec->end_space;
		pNewVec->high_space=pVec->high_space;
		pNewVec->direct=pVec->direct;
	}
}
CLDSParamPlate* CDesignCircularPlateDlg::CreateCircularPlate(CLDSParamPlate* pToExistPlate/*=NULL*/)
{
	if(m_iPlateType==6)	//单独设计生成遮水板 wht 11-05-11
		return CreateWaterShieldedPlate(pToExistPlate);
	//
	if(!IsValidParam(FALSE))	//不满足必要的设计条件
		return NULL;
	CLDSParamPlate *pCirPlate=NULL;
	bool bCreateNewPlate=false;
	if(pToExistPlate==NULL&&(m_pCirPlate==NULL||m_pCirPlate==&_previewParamPlate))
	{	//没有指定钢板对象且系统预设板m_pCirPlate不是已有钢板时，创建一新参数化钢板对象　wjh-2018.1.29
		m_pCirPlate=pCirPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		_previewParamPlate.dwPermission=pCirPlate->dwPermission;
		_previewParamPlate.CloneTo(*pCirPlate);
		bCreateNewPlate=true;
	}
	else if(pToExistPlate!=NULL)	//有预置钢板时（无论是临时预览钢板_previewParamPlate还是其它）均不创建新钢板
		pCirPlate=pToExistPlate;
	else if(m_pCirPlate!=NULL)
		pCirPlate=m_pCirPlate;
	else
		pCirPlate=m_pCirPlate=&_previewParamPlate;
	pCirPlate->m_hPartWeldParent=m_pDatumTube->handle;
	pCirPlate->cfgword=m_pDatumTube->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
	pCirPlate->SetLayer(m_pDatumTube->layer());	//调整钢管肋板的图层名
	pCirPlate->thick=thick;
	pCirPlate->iSeg=SEGI(m_sSeg.GetBuffer());
	pCirPlate->cMaterial = m_cMaterial;
	pCirPlate->SetPartNo(m_sPartNo);
	//引入螺栓
	pCirPlate->EmptyLsRef();
	for(CLsRef *pLsRef=LsRefList.GetFirst();pLsRef;pLsRef=LsRefList.GetNext())
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
	pCirPlate->workNorm=design_axis_z;	//参数化工作法线赋值
	BOOL bNeedBend=ConnVecList.IsNeedBend();
	UpdateCircularPlateConnVecList(pCirPlate);
	if(m_iPlateType==0)
	{	//异型法兰
		pCirPlate->m_iParamType=TYPE_CIRRIBPLATE;	//异形法兰 
		pCirPlate->SetDesignItemValue('C',m_pDatumTube->handle,"",PARAM_POS,1);	//定位基准钢管
		pCirPlate->SetDesignItemValue('P',m_pDatumNode->handle,"",PARAM_POS,1);	//定位基准节点
		pCirPlate->SetDesignItemValue('V',para.V,"",PARAM_SHAPE);	//加工间隙
		if(ConnVecList.GetCount(TRUE)>0)
			pCirPlate->SetDesignItemValue('D',para.D,"",PARAM_SHAPE);	//异形法兰自由边距
		else 
		{
			pCirPlate->SetDesignItemValue('j',para.j,"");	//起始边
			pCirPlate->SetDesignItemValue('A',para.A,"");	//扇形起始角度
			pCirPlate->SetDesignItemValue('B',para.B,"",PARAM_SHAPE);	//扇形夹角
			pCirPlate->SetDesignItemValue('R',para.R,"",PARAM_SHAPE);	//环向板外圆弧半径
		}
		
		pCirPlate->SetDesignItemValue('S',para.nOffsetType==0?para.S:0,"",PARAM_POS);	//沿基准钢管延伸方向的偏移距离
		pCirPlate->SetDesignItemValue('T',para.nOffsetType==0?0:para.S,"",PARAM_POS);	//沿环向板法线方向的偏移距离
		pCirPlate->SetDesignItemValue('r',para.r,"",PARAM_SHAPE);	//内圆半径:r<0根据钢管生成内圆 r=0无内圆
		design_axis_z.UpdateVector(console.GetActiveModel());
		pCirPlate->ucs.axis_z=design_axis_z.vector;	//Z轴方向
		//
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
		pCirPlate->SetDesignItemValue('A',para.A,"");
		pCirPlate->SetDesignItemValue('B',para.B,"",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('C',m_pDatumTube->handle,"",PARAM_POS,1);
		pCirPlate->SetDesignItemValue('E',m_pDatumNode->handle,"",PARAM_POS,1);
		if(para.biHoleType==0)		//0.圆孔
			pCirPlate->SetDesignItemValue(KEY2C("BH"),para.BH);
		else if(para.biHoleType==1)	//>0.半圆孔
			pCirPlate->SetDesignItemValue(KEY2C("BH"),0);
		else						//<0.无流水孔设计
			pCirPlate->SetDesignItemValue(KEY2C("BH"),-1);
		CLDSGeneralPlate::DESIGN_PARAM para_item1,para_item2;
		if(m_pRelaPart1&&(!pCirPlate->GetDesignItemValue('F',&para_item1)||para_item1.cValueType!=3))
		{
			para_item1.cValueType=3;
			para_item1.value.hPart=m_pRelaPart1->handle;
			if(para.biHoleType==1)	//
				para_item1.value.uiAttribute=ftoi(para.R1*10);
			else if(para.biHoleType==0)
			{
				WORD* pw_BGorBS=(WORD*)&(para_item1.value.uiAttribute);
				*pw_BGorBS	  =25;
				*(pw_BGorBS+1)=25;
			}
		}
		if(m_pRelaPart2&&(!pCirPlate->GetDesignItemValue('G',&para_item2)||para_item2.cValueType!=3))
		{
			para_item2.cValueType=3;
			para_item2.value.hPart=m_pRelaPart2->handle;
			para_item2.value.uiAttribute=para_item1.value.uiAttribute;
		}
		pCirPlate->SetDesignItem('F',para_item1);
		pCirPlate->SetDesignItemValue('Y',para.nCirCoverPlateType,"",PARAM_SHAPE,1);
		if(abs(para.nCirCoverPlateType)==1||abs(para.nCirCoverPlateType)==2)
			pCirPlate->SetDesignItemValue('G',0,"",PARAM_POS,1);
		else if(m_pRelaPart2&&m_pRelaPart2->GetClassTypeId()==CLS_PLATE)	//普通环向封板
			pCirPlate->SetDesignItem('G',para_item2);
		pCirPlate->SetDesignItemValue('N',para.N,"",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('D',para.H,"",PARAM_SHAPE);
		pCirPlate->SetDesignItemValue('W',para.W,"",PARAM_SHAPE);
		if(para.S>=0)
			pCirPlate->SetDesignItemValue('S',-1,"");
		else 
			pCirPlate->SetDesignItemValue('S',1,"");
		pCirPlate->SetDesignItemValue('R',m_pDatumTube->GetDiameter()*0.5+para.V,"",PARAM_SHAPE);
		pCirPlate->m_bDisplayHole=TRUE;
		pCirPlate->DesignPlate();
		pCirPlate->SetModified();
		pCirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pCirPlate->GetSolidPartObject());
		if(bCreateNewPlate==false)
		{	//实时预览的封板，只需生成一块即可，方便通过m_pCirPlate进行参数调整
			g_pSolidDraw->AddCS(pCirPlate->handle,pCirPlate->ucs);
			g_pSolidDraw->Draw();
			return pCirPlate;
		}
		CLDSParamPlate *pSimpleCirPlate3=NULL,*pSimpleCirPlate4=NULL;	//记录另外两块简化封板
		if(para.nCirCoverPlateType==0&&para.bCreateMirPlate)
		{	//生成另一侧封板
			CLDSParamPlate *pMirCoverPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pMirCoverPlate->CopyProperty(pCirPlate);
			pMirCoverPlate->SetPartNo(m_sMirPartLabel);
			pMirCoverPlate->SetLayer(m_pDatumTube->layer()); //调整图层名
			pMirCoverPlate->SetDesignItemValue('A',para.A+180);
			pMirCoverPlate->m_hPartWeldParent = m_pDatumTube->handle;
			pMirCoverPlate->DesignPlate();
			pMirCoverPlate->SetModified();
			pMirCoverPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pMirCoverPlate->GetSolidPartObject());
		}
		else if(abs(para.nCirCoverPlateType)==2&&m_pRelaPart2!=NULL)
		{	//生成第二块钢板上的简化环向封板 wht 12-03-27
			pSimpleCirPlate3=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pSimpleCirPlate3->CopyProperty(pCirPlate);
			pSimpleCirPlate3->SetLayer(m_pDatumTube->layer()); //调整图层名
			pSimpleCirPlate3->SetDesignItem('F',para_item2);
			pSimpleCirPlate3->SetDesignItemValue('Y',-1*para.nCirCoverPlateType,"",PARAM_SHAPE,1);
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
			pCoverPlate1->SetLayer(m_pDatumTube->layer()); //调整图层名
			pCoverPlate1->SetDesignItemValue('S',(double)1,"");
			pCoverPlate1->SetDesignItem('F',para_item1);
			pCoverPlate1->m_hPartWeldParent = m_pDatumTube->handle;
			pCoverPlate1->DesignPlate();
			pCoverPlate1->SetModified();
			pCoverPlate1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pCoverPlate1->GetSolidPartObject());
			if(para.nCirCoverPlateType==0&&para.bCreateMirPlate)
			{	//生成另一侧封板
				CLDSParamPlate *pMirCoverPlate1=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
				pMirCoverPlate1->CopyProperty(pCoverPlate1);
				pMirCoverPlate1->SetPartNo(m_sMirPartLabel);
				pMirCoverPlate1->SetLayer(m_pDatumTube->layer()); //调整图层名
				pMirCoverPlate1->SetDesignItemValue('A',para.A+180,"");
				pMirCoverPlate1->m_hPartWeldParent = m_pDatumTube->handle;
				pMirCoverPlate1->DesignPlate();
				pMirCoverPlate1->SetModified();
				pMirCoverPlate1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pMirCoverPlate1->GetSolidPartObject());
			}
			else if(abs(para.nCirCoverPlateType)==2&&m_pRelaPart2!=NULL)
			{	//生成第二块钢板上的环向封板 wht 12-03-27
				pSimpleCirPlate4=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
				pSimpleCirPlate4->CopyProperty(pCoverPlate1);
				pSimpleCirPlate4->SetLayer(m_pDatumTube->layer()); //调整图层名
				pSimpleCirPlate4->SetDesignItem('F',para_item2);
				pSimpleCirPlate4->SetDesignItemValue('Y',-1*para.nCirCoverPlateType,"",PARAM_SHAPE,1);
				pSimpleCirPlate4->m_hPartWeldParent = m_pDatumTube->handle;
				pSimpleCirPlate4->DesignPlate();
				pSimpleCirPlate4->SetModified();
				pSimpleCirPlate4->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pSimpleCirPlate4->GetSolidPartObject());
			}
		}
	}
	else if(m_iPlateType==7)
	{	//环向走道板
		pCirPlate->m_iParamType=TYPE_CIRRIBPLATE;
		pCirPlate->SetDesignItemValue('C',m_pDatumTube->handle,"",PARAM_POS,1);	//定位基准钢管
		pCirPlate->SetDesignItemValue('E',m_pRelaPart1->handle,"",PARAM_POS,1);	//依赖角钢
		pCirPlate->SetDesignItemValue('V',para.V,"",PARAM_SHAPE);	//加工间隙
		pCirPlate->SetDesignItemValue('W',para.W,"",PARAM_SHAPE);	//
		pCirPlate->SetDesignItemValue('H',para.H,"",PARAM_SHAPE);	//台口高度
		pCirPlate->SetDesignItemValue('B',para.B,"",PARAM_SHAPE);	//扇形角度
		pCirPlate->SetDesignItemValue(KEY2C("ES"),para.fEdgeSpace,"",PARAM_SHAPE);
		//
		pCirPlate->DesignPlate();
		pCirPlate->SetModified();
		pCirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pCirPlate->GetSolidPartObject());
	}
	else 
	{	//环向肋板或跨越连板
		if((m_iPlateType==2&&para.nDHRelativeType==0)||(m_iPlateType==3&&para.nDHRelativeType))
			pCirPlate->m_iParamType=TYPE_TUBERIBPLATE;		//跨越连板
		else
			pCirPlate->m_iParamType=TYPE_CIRRIBPLATE;		//环向肋板
		//
		if(m_iPlateType==3&&para.nDHRelativeType==0)
		{
			para.W=para.N=para.R=para.H=para.h=0;
			pCirPlate->SetDesignItemValue('R',para.R,"",PARAM_POS);	//环向板外圆弧半径
		}
		else if(m_iPlateType==5)
			para.N=para.D=0;
		pCirPlate->SetDesignItemValue('C',m_pDatumTube->handle,"",PARAM_POS,1);	//定位基准钢管
		pCirPlate->SetDesignItemValue('P',m_pDatumNode->handle,"",PARAM_POS,1);	//定位基准节点
		pCirPlate->SetDesignItemValue('V',para.V,"",PARAM_SHAPE);	//加工间隙
		if(para.biHoleType==0)		//0.圆孔
			pCirPlate->SetDesignItemValue(KEY2C("BH"),para.BH);
		else if(para.biHoleType==1)	//>0.半圆孔
			pCirPlate->SetDesignItemValue(KEY2C("BH"),0);
		else						//<0.无流水孔设计
			pCirPlate->SetDesignItemValue(KEY2C("BH"),-1);
		DESIGN_PLATE_ITEM* pParaItem=pCirPlate->GetDesignItem('E');
		if(pParaItem==NULL||pParaItem->dataType!=3)
		{
			pParaItem=pCirPlate->params.Add('E');
			pParaItem->dataType=3;
			pParaItem->value.hPart=m_pRelaPart1->handle;
			if(para.biHoleType==1)	//
				pParaItem->value.uiAttribute=ftoi(para.R1*10);
			else if(para.biHoleType==0)
			{
				WORD* pw_BGorBS=(WORD*)&(pParaItem->value.uiAttribute);
				*pw_BGorBS	  =25;
				*(pw_BGorBS+1)=25;
			}
		}
		if(m_iPlateType<4&&m_pRelaPart2&&(m_pRelaPart2->IsPlate()||m_pRelaPart2->IsTube()))
		{	//第二块关联构件(钢板|钢管)
			pParaItem=pCirPlate->GetDesignItem('F');
			if(pParaItem==NULL||pParaItem->dataType!=3)
			{
				pParaItem=pCirPlate->params.Add('F');
				pParaItem->dataType=3;
				pParaItem->value.hPart=m_pRelaPart2->handle;
				if(para.biHoleType==1)	//半圆孔
					pParaItem->value.uiAttribute=ftoi(para.R2*10);
				else if(para.biHoleType==0)
				{
					WORD* pw_BGorBS=(WORD*)&(pParaItem->value.uiAttribute);
					*pw_BGorBS	  =25;
					*(pw_BGorBS+1)=25;
				}
			}
		}
		else 
			pCirPlate->SetDesignItemValue('F',0,"",PARAM_POS,1);
		pCirPlate->SetDesignItemValue('S',para.nOffsetType==0?para.S:0,"",PARAM_POS);	//沿基准钢管延伸方向的偏移距离
		pCirPlate->SetDesignItemValue('T',para.nOffsetType==0?0:para.S,"",PARAM_POS);	//沿环向板法线方向的偏移距离
		pCirPlate->SetDesignItemValue('r',para.r,"",PARAM_SHAPE);	//内圆半径:r<0根据钢管生成内圆 r=0无内圆 r>0按指定半径生成内圆
		//计算屏幕朝外的法线方向(模型坐标系)
		if(para.Y==0&&m_pRelaPart1!=NULL)
		{	//未明确指定环向肋板前后（内外）侧时，自动取当初视图的可见侧为当前侧　wjh-2018.2.1
			GEPOINT dynScrNorm=g_pSolidOper->TransVFromScr(GEPOINT(0,0,-1));	//屏幕坐标系Y轴向下Z轴向后 wjh-2018.1.26
			para.Y=(dynScrNorm*m_pRelaPart1->ucs.axis_z>0)?1:-1;
		}
		pCirPlate->SetDesignItemValue('Y',para.Y,"",PARAM_SHAPE);	//半圆型环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)
		pCirPlate->SetDesignItemValue('W',para.W,"",PARAM_SHAPE);	//第一块关联钢板的焊缝长
		pCirPlate->SetDesignItemValue('N',para.N,"",PARAM_SHAPE);	//第二块关联钢板或钢管的焊缝长
		pCirPlate->SetDesignItemValue('D',para.D,"",PARAM_SHAPE);	//基准钢管外壁到自由边距离，简称自由边距
		pCirPlate->SetDesignItemValue('H',para.H,"",PARAM_SHAPE);	//台口高度
		if(m_iPlateType<4&&para.h!=para.H)
			pCirPlate->SetDesignItemValue('h',para.h,"",PARAM_SHAPE);	//N侧台口高度
		if(m_iPlateType==5)
			pCirPlate->SetDesignItemValue('y',para.gama,"",PARAM_SHAPE);//倒角角度
		//自由边类型(>0直线，<=0圆弧)
		if(m_iPlateType==3||m_iPlateType==5||(m_iPlateType==2&&para.nDHRelativeType==0))
			pCirPlate->SetDesignItemValue('G',0);
		else 
			pCirPlate->SetDesignItemValue('G',1);
		if(para.B!=0)
			pCirPlate->SetDesignItemValue('B',para.B);	//扇形角度
		//
		pCirPlate->m_bDisplayHole=TRUE;
		pCirPlate->DesignPlate();
		pCirPlate->SetModified();
		pCirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pCirPlate->GetSolidPartObject());
	}
	g_pSolidDraw->AddCS(pCirPlate->handle,pCirPlate->ucs);
	g_pSolidDraw->Draw();
	return pCirPlate;
}

CLDSParamPlate *CDesignCircularPlateDlg::CreateWaterShieldedPlate(CLDSParamPlate* pToExistPlate/*=NULL*/)
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
	CLDSParamPlate *pPlateAttachLaunder=NULL;
	if(pToExistPlate==NULL&&(m_pCirPlate==NULL||m_pCirPlate==&_previewParamPlate))
		pPlateAttachLaunder=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	else if(pToExistPlate!=NULL)	//有预置钢板时（无论是临时预览钢板_previewParamPlate还是其它）均不创建新钢板
		pPlateAttachLaunder=pToExistPlate;
	else if(m_pCirPlate!=NULL)
		pPlateAttachLaunder=m_pCirPlate;
	else
		pPlateAttachLaunder=m_pCirPlate=&_previewParamPlate;
	//遮水板不需要焊接父构件
	//pPlateAttachLaunder->m_hPartWeldParent = pDatumTube->handle;
	pPlateAttachLaunder->CopyModuleInstanceInfo(pDatumTube);	//调整钢板配材号与基准构件或基准节点配材号一致
	pPlateAttachLaunder->ucs=ucs;
	pPlateAttachLaunder->SetPartNo(m_sPartNo);
	pPlateAttachLaunder->iSeg=SEGI(m_sSeg.GetBuffer());
	pPlateAttachLaunder->thick=thick;
	pPlateAttachLaunder->cMaterial=m_cMaterial;
	pPlateAttachLaunder->m_iParamType=TYPE_WATERSHIELDEDPLATE;
	//
	pPlateAttachLaunder->ucs.origin+=pPlateAttachLaunder->ucs.axis_z*water.S; 
	pPlateAttachLaunder->SetDesignItemValue('B',pBaseNode->handle,"",PARAM_POS,1);	//基准节点
	pPlateAttachLaunder->SetDesignItemValue('C',pDatumTube->handle,"",PARAM_POS,1);	//基准钢管
	pPlateAttachLaunder->SetDesignItemValue('W',water.W,"",PARAM_SHAPE);	//外环直径
	pPlateAttachLaunder->SetDesignItemValue('S',water.S,"",PARAM_POS);		//法向偏移距离
	pPlateAttachLaunder->SetDesignItemValue('K',water.K,"",PARAM_SHAPE);	//流水槽宽度
	pPlateAttachLaunder->SetDesignItemValue('H',water.H,"",PARAM_SHAPE);	//流水槽深度
	pPlateAttachLaunder->SetDesignItemValue('L',water.L,"",PARAM_SHAPE);	//流水槽长度
	pPlateAttachLaunder->SetDesignItemValue('A',water.A,"",PARAM_SHAPE);	//两流水槽之间的夹角
	pPlateAttachLaunder->SetDesignItemValue('O',water.O,"",PARAM_SHAPE);	//两流水槽角平分线与基准边之间的偏移角
	pPlateAttachLaunder->SetDesignItemValue('E',water.E,"",PARAM_POS);		//定位基准边:0.基准边1.焊道线
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
	return pPlateAttachLaunder;
}

void CDesignCircularPlateDlg::ModifyPlateParam(WORD wKey,double fValue,BOOL bReDesign/*=TRUE*/)
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
	else if(m_iPlateType>1&&wKey=='S'&&m_pDatumTube&&m_pCirPlate)
	{	//环向肋板
		GEPOINT vDaumAxisVec=m_pDatumTube->End()-m_pDatumTube->Start();
		if(vDaumAxisVec*m_pCirPlate->ucs.axis_z<0)
			para.S=pItem->value.fValue=-para.S;
		else
			para.S=pItem->value.fValue= para.S;
	}
	if(bReDesign)
		ReDesignPlate();
}
void CDesignCircularPlateDlg::ModifyPlateParam(WORD wKey,long nValue,BOOL bReDesign/*=TRUE*/)
{
	if(m_pCirPlate==NULL)
		return;
	DESIGN_PLATE_ITEM *pItem=m_pCirPlate->GetDesignItem(wKey);
	if(pItem)
	{
		if(pItem->dataType==3)
			pItem->value.hObj=nValue;
		else
			pItem->SetItemValue(nValue);
	}
	if(bReDesign)
		ReDesignPlate();
}

void CDesignCircularPlateDlg::ModifyPlateParam(WORD wKey,long hPart,UINT uiValue,BOOL bReDesign/*=TRUE*/)
{
	if(m_pCirPlate==NULL)
		return;
	DESIGN_PLATE_ITEM *pItem=m_pCirPlate->params.Add(wKey);	//GetDesignItem(wKey)存在＝NULL情况 wjh-2018.3.1
	if(pItem)
	{
		pItem->dataType=3;
		pItem->value.hObj=hPart;
		pItem->value.uiAttribute=uiValue;
	}
	if(bReDesign)
		ReDesignPlate();
}
void CDesignCircularPlateDlg::ReDesignPlate()
{
	if(m_pCirPlate)
	{
		if(m_iPlateType==3||m_iPlateType==5||(m_iPlateType==2&&para.nDHRelativeType==0))
			para.G=0;
		else 
			para.G=1;
		if(m_iPlateType>1&&m_iPlateType<6)
			m_pCirPlate->SetDesignItemValue('G',para.G,"自由边类型(>0直线，<=0圆弧)");
		m_pCirPlate->DesignPlate();
		m_pCirPlate->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(m_pCirPlate->GetSolidPartObject());
		g_pSolidDraw->AddCS(m_pCirPlate->handle,m_pCirPlate->ucs);
		CLDSView::RedrawPlateProfileDrawing(m_pCirPlate);
		g_pSolidDraw->Draw();
	}
}
//布置螺栓 wht 11-05-16
void CDesignCircularPlateDlg::LayoutBolt(CLDSParamPlate *pParamPlate)
{
	if(m_xLayoutBolt.m_nBoltNum<=0||pParamPlate==NULL)
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
	if(m_pCirPlate&&m_pDatumTube)
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
			//实际操作中发现下列代码会导致环向肋板的提示方向与钢管轴向反向
			//推测应该是之前参数板计算偏移量以板法线为正向导致临时把方向调整的与真实偏移方向一致，
			//现在已在GetPropValueStr中偏移量显示正负值中进行相应调整　wjh-2018.2.1
			//if(m_pDatumTube->ucs.axis_z*m_pCirPlate->ucs.axis_z<0)
			//	m_xSpreadVec1*=-1;
			g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),&base_pos,1,m_xSpreadVec1,"钢管轴向",200);
		}
		else if(iType==TYPE_Z_AXIS_VECTOR)
		{
			base_pos=inters_pt;
			if(m_iPlateType==6)
				base_pos+=m_pCirPlate->ucs.axis_x*water.W*0.5;
			else if(m_iPlateType==0)
			{
				double angle=para.B*0.5*RADTODEG_COEF;
				base_pos.Set(para.R,0,0);
				rotate_point_around_axis(base_pos,angle,f3dPoint(0,0,0),f3dPoint(0,0,100));
				coord_trans(base_pos,m_pCirPlate->ucs,TRUE);
			}
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
		else if(iType==TYPE_SPREAD_VECTOR)
		{	//环板方位
			if(m_pRelaPart1==NULL)
			{
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"环向板示意图错误");
				return;
			}
			base_pos=m_pCirPlate->ucs.origin;
			if(para.Y>0)
				m_xSpreadVec1=-m_pRelaPart1->ucs.axis_z;
			else if(para.Y<0)
				m_xSpreadVec1=m_pRelaPart1->ucs.axis_z;
			else
			{
				f3dPoint conn_wingvec1=m_pCirPlate->GetRelaConnVec(1);
				f3dPoint conn_wingvec2=m_pCirPlate->GetRelaConnVec(2);
				if(m_pRelaPart2==NULL||fabs(conn_wingvec1*conn_wingvec2)>EPS_COS)
					m_xSpreadVec1=m_pRelaPart1->ucs.axis_z;
				else
				{
					f3dPoint bi_vec=conn_wingvec1+conn_wingvec2;
					normalize(bi_vec);
					if(m_pRelaPart1->ucs.axis_z*bi_vec>0)	//第一块钢板工作法向指凹角内侧
						m_xSpreadVec1=m_pRelaPart1->ucs.axis_z;
					else					//第一块钢板工作法向指凹角外侧，需反号后再赋值
						m_xSpreadVec1=-m_pRelaPart1->ucs.axis_z;
				}
			}
			btc::SKETCH_PLANE sketch(base_pos,m_xSpreadVec1,m_pRelaPart1->ucs.axis_x,nLen);
			g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),sketch.pVertexArr,sketch.VertexCount,m_xSpreadVec1,"环板方位",200);
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
void CDesignCircularPlateDlg::RedrawSketchDrawing(CPropTreeItem *pItem)
{
	if(pItem==NULL||m_pCirPlate==NULL)
		return;
	DelWorkPlaneSketch();
	if(GetPropID("para.Y")==pItem->m_idProp)	//环形板方位
		WorkPlaneSketch(TYPE_SPREAD_VECTOR);
	else if(GetPropID("para.A")==pItem->m_idProp&&m_iPlateType==0)		
		WorkPlaneSketch(TYPE_DATUM_VECTOR);	
	else if((GetPropID("para.S")==pItem->m_idProp||GetPropID("para.nOffsetType")==pItem->m_idProp)&&m_iPlateType!=1)
	{
		if(para.nOffsetType==0)
			WorkPlaneSketch(TYPE_TUBE_SPREAD);
		else
			WorkPlaneSketch(TYPE_Z_AXIS_VECTOR);
	}
}
int CDesignCircularPlateDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen=false;
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CDesignCircularPlateDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}
void CDesignCircularPlateDlg::OnCancel() 
{
	DelWorkPlaneSketch();
	g_pSolidDraw->DelEnt(_previewParamPlate.handle);
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
//OnDestroy会在对话框中途中断退出时调用，故不适合添加删除预览实体等相关代码 wjh-2018.1.24
//void CDesignCircularPlateDlg::OnDestroy()

void CDesignCircularPlateDlg::OnClose() 
{
	DelWorkPlaneSketch();
	g_pSolidDraw->DelEnt(_previewParamPlate.handle);
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
