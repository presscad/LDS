// DesPedalPlateDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "DesPedalPlateDlg.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "PropertyListOper.h"
//////////////////////////////////////////////////////////////////////////
//调函数处理
static BOOL FireCallBackCommand(CWnd* pCallWnd, UINT message, WPARAM wParam)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please Preview part(Press enter or esc key to end):", "");
#else 
	pCmdLine->FillCmdLine("旋转预览构件（ESC键或回车键返回肋板对话框）:", "");
#endif
	CString cmdStr;
	g_pSolidSet->SetOperType(OPER_ROTATE);
	pCmdLine->GetStrFromCmdLine(cmdStr);
	return TRUE;
}
static BOOL ModifyPedalPlateProperty(CPropertyList *pPropList, CPropTreeItem *pItem, CString &valueStr)
{
	CDesPedalPlateDlg *pDlg = (CDesPedalPlateDlg*)pPropList->GetParent();
	if (pDlg == NULL)
		return FALSE;
	BOOL bReDesignPlate = FALSE, bRelayoutLs = FALSE;
	CLDSParamPlate *pPlate = pDlg->m_pPedalPlate;
	CPropertyListOper<CDesPedalPlateDlg> oper(pPropList, pDlg);	
	CPropertyListOper<CDesPedalPlateDlg> sub_oper(&pDlg->m_xSubPropList, pDlg);
	if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("sPartNo"))
	{
		pDlg->m_sPartNo.Copy(valueStr);
		SEGI nSeg;
		if (ParsePartNo(pDlg->m_sPartNo, &nSeg, NULL))
			pDlg->m_nSegI = nSeg;
		oper.UpdatePropItemValue("iSeg");
		//更新肋板段号
		if (pPlate)
		{
			pPlate->iSeg = nSeg;
			pPlate->SetPartNo(pDlg->m_sPartNo);
		}
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("iSeg"))
	{
		pDlg->m_nSegI = SEGI(valueStr.GetBuffer());
		if (pPlate)
			pPlate->iSeg = pDlg->m_nSegI;
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("material"))
	{
		pDlg->m_cMaterial = QuerySteelBriefMatMark(valueStr.GetBuffer(1));
		if (pPlate)
			pPlate->cMaterial = pDlg->m_cMaterial;
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("thick"))
	{
		pDlg->m_nThick = atoi(valueStr);
		if (pPlate)
		{
			pPlate->thick = pDlg->m_nThick;
			bReDesignPlate = TRUE;
		}
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("para.ciDirection"))
	{
		pDlg->para.ciDirection = valueStr[0] - '0';
		pDlg->ModifyPlateParam('V', (long)pDlg->para.ciDirection);
		bReDesignPlate = TRUE;
		//
		pDlg->DisplaySubPropList();
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("layout_para.nBoltD"))
	{
		int nBoltD = atoi(valueStr); 
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace, nBoltD);
		pDlg->layout_para.m_nBoltD = nBoltD;
		pDlg->layout_para.m_nBoltSpace = LsSpace.SingleRowSpace;
		//
		oper.UpdatePropItemValue("layout_para.nBoltSpace");
		bRelayoutLs = TRUE;
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("layout_para.nBoltNum"))
	{
		pDlg->layout_para.m_nBoltNum = atoi(valueStr);
		bRelayoutLs = TRUE;
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("layout_para.nBoltSpace"))
	{
		pDlg->layout_para.m_nBoltSpace = atoi(valueStr);
		bRelayoutLs = TRUE;
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("layout_para.ciWoreWing"))
	{
		pDlg->layout_para.m_ciWorkWing = atoi(valueStr);
		bRelayoutLs = TRUE;
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("layout_para.ciGType"))
	{
		if (valueStr.CompareNoCase("g") == 0)
			pDlg->layout_para.m_ciGType = 0;
		else if (valueStr.CompareNoCase("g1") == 0)
			pDlg->layout_para.m_ciGType = 1;
		else if (valueStr.CompareNoCase("g2") == 0)
			pDlg->layout_para.m_ciGType = 2;
		else if (valueStr.CompareNoCase("g3") == 0)
			pDlg->layout_para.m_ciGType = 3;
		else
			pDlg->layout_para.m_ciGType = 4;
		JGZJ jgzj;
		getjgzj(jgzj, pDlg->m_pDatumAngle->GetWidth());
		if (pDlg->m_pDatumAngle->m_bEnableTeG)
			jgzj = (pDlg->layout_para.m_ciWorkWing == 0) ? pDlg->m_pDatumAngle->xWingXZhunJu : pDlg->m_pDatumAngle->xWingYZhunJu;
		if (pDlg->layout_para.m_ciGType == 0)
			pDlg->layout_para.m_nGDist = jgzj.g;
		else if (pDlg->layout_para.m_ciGType == 1)
			pDlg->layout_para.m_nGDist = jgzj.g1;
		else if (pDlg->layout_para.m_ciGType == 2)
			pDlg->layout_para.m_nGDist = jgzj.g2;
		else if (pDlg->layout_para.m_ciGType == 3)
			pDlg->layout_para.m_nGDist = jgzj.g3;
		//
		oper.UpdatePropItemValue("layout_para.nGDist");
		CPropTreeItem* pFindItem = pPropList->FindItemByPropId(CDesPedalPlateDlg::GetPropID("layout_para.nGDist"), NULL);
		if (pFindItem)
			pFindItem->SetReadOnly(pDlg->layout_para.m_ciGType != 4);
		bRelayoutLs = TRUE;
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("layout_para.nGDist"))
	{
		pDlg->layout_para.m_nGDist = atoi(valueStr);
		bRelayoutLs = TRUE;
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("layout_para.fSpace"))
	{
		pDlg->layout_para.m_fSpace = atof(valueStr);
		bRelayoutLs = TRUE;
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("para.C"))
	{
		pDlg->para.C = valueStr[0] - '0';
		pDlg->ModifyPlateParam('C', pDlg->para.C);
		bReDesignPlate = TRUE;
		pDlg->UpdateSketchMap();
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("para.L"))
	{
		pDlg->para.L = atof(valueStr);
		pDlg->ModifyPlateParam('L', pDlg->para.L);
		bReDesignPlate = TRUE;
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("para.W"))
	{
		pDlg->para.W = atof(valueStr);
		pDlg->ModifyPlateParam('W', pDlg->para.W);
		bReDesignPlate = TRUE;
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("para.H"))
	{
		pDlg->para.H = atof(valueStr);
		pDlg->ModifyPlateParam('H', pDlg->para.H);
		bReDesignPlate = TRUE;
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("para.D"))
	{
		pDlg->para.D = atof(valueStr);
		pDlg->ModifyPlateParam('D', pDlg->para.D);
		bReDesignPlate = TRUE;
	}
	else if (pItem->m_idProp == CDesPedalPlateDlg::GetPropID("para.S"))
	{
		pDlg->para.S = atof(valueStr);
		pDlg->ModifyPlateParam('S', pDlg->para.S);
		bReDesignPlate = TRUE;
	}
	if(bRelayoutLs)
		pDlg->LayoutBolt(pPlate);
	if (bReDesignPlate || bRelayoutLs)
		pDlg->ReDesignPlate();
	return TRUE;
}
static BOOL ButtonClickPedalPlate(CPropertyList* pPropList, CPropTreeItem* pItem)
{
	CDesPedalPlateDlg *pDlg = (CDesPedalPlateDlg*)pPropList->GetParent();
	if (pDlg == NULL)
		return FALSE;
	if (CDesPedalPlateDlg::GetPropID("hDatumAngle") == pItem->m_idProp)
		pDlg->SelectObject(CLS_LINEANGLE, pItem->m_idProp);
	return TRUE;
}
static BOOL FirePedalPlateHelpStr(CPropertyList* pPropList, CPropTreeItem* pItem)
{
	if (pPropList->GetDlgCtrlID() != IDC_LIST_BOX)
		return FALSE;	//子属性栏的属性不响应FirePropHelpPrompt()函数
	CLDSView* pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CDesPedalPlateDlg *pDlg = (CDesPedalPlateDlg*)pPropList->GetParent();
	if (pDlg == NULL || pView == NULL)
		return FALSE;
	g_pSolidDraw->DelCS(ISolidDraw::WORK_CS);
	pView->Invalidate(FALSE);
	long hCurPart = 0;
	if (CDesPedalPlateDlg::GetPropID("hDatumAngle") == pItem->m_idProp)
	{
		if (pDlg->m_pDatumAngle)
			hCurPart = pDlg->m_pDatumAngle->handle;
	}
	pDlg->DisplaySubPropList(pItem);
	//选中定位构件或当前正在设计的肋板
	if (hCurPart == 0 && pDlg->m_pPedalPlate)
		hCurPart = pDlg->m_pPedalPlate->handle;
	if (hCurPart > 0x20)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(hCurPart);
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// CDesPedalPlateDlg 对话框
IMPLEMENT_DYNAMIC(CDesPedalPlateDlg, CCallBackDialog)
CDesPedalPlateDlg::CDesPedalPlateDlg(CWnd* pParent /*=nullptr*/)
	: CCallBackDialog(IDD_DES_PEDAL_PLATE, pParent)
{
	m_pDatumAngle = NULL;
	m_cMaterial = 'S';
	m_nThick = 12;
	m_bNewPlate = TRUE;
	m_pPedalPlate = NULL;
	_previewPlate.handle = ISolidDraw::BLK_LOCATE_CS + 1;	//临时对象句柄, 但需要避开系统特定坐标系的ID
	_previewPlate.SetBelongModel(&Ta);
}

CDesPedalPlateDlg::~CDesPedalPlateDlg()
{
}

void CDesPedalPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX_SUB, m_xSubPropList);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
}


BEGIN_MESSAGE_MAP(CDesPedalPlateDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_MOVE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_CONFIRM, OnConfirm)
	ON_BN_CLICKED(IDC_BTN_MODEL_VIEW_OPER, OnBnModelViewOper)
END_MESSAGE_MAP()

// CDesPedalPlateDlg 消息处理程序
int CDesPedalPlateDlg::nScrLocationX = 0;
int CDesPedalPlateDlg::nScrLocationY = 0;
bool CDesPedalPlateDlg::m_bValidStaticRibParam = false;
CDesPedalPlateDlg::PEDALPLATE_PARAM CDesPedalPlateDlg::s_xDesParam;
CDesPedalPlateDlg::LAYOUT_PARAM CDesPedalPlateDlg::s_xLayoutParam;
CDesPedalPlateDlg::BASIC_PARRM CDesPedalPlateDlg::s_xBasicParam;
BOOL CDesPedalPlateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.SetDividerScale(0.6);
	m_xSubPropList.SetDividerScale(0.6);
	m_xSubPropList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	InitPropHashtable();
	SetCallBackFunc(FireCallBackCommand);
	if (m_bInernalStart)	//内部启动
	{	//恢复选中项
		FinishSelectObjOper();	//完成选择对象的后续操作
		DisplayPlatePropList();
		CPropTreeItem *pItem = m_propList.FindItemByPropId(m_idEventProp, NULL);
		m_propList.SetFocus();	//属性栏获得焦点
		if (pItem)
			m_propList.SetCurSel(pItem->m_iIndex);	//设置选中项
		DisplaySubPropList(pItem);	//显示子属性
		CPropTreeItem *pSubItem = m_xSubPropList.FindItemByPropId(m_idSubEventProp, NULL);
		m_xSubPropList.SetFocus();
		if (pSubItem)	//选中子属性
			m_xSubPropList.SetCurSel(pSubItem->m_iIndex);
	}
	else
	{
		if (!m_bNewPlate)
		{
			if (m_pPedalPlate == NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("You haven't choose the right rib plate!");
#else
				AfxMessageBox("未选择正确的角钢肋板！");
#endif
				return FALSE;
			}
			long A = 0, B = 0;
			m_pPedalPlate->GetDesignItemValue('A', &A);
			m_pDatumAngle = (CLDSLineAngle*)console.FromPartHandle(A, CLS_LINEANGLE);
			if (m_pDatumAngle == NULL)
				return FALSE;
			m_sPartNo.Copy(m_pPedalPlate->GetPartNo());
			m_nThick = (int)m_pPedalPlate->GetThick();
			m_cMaterial = m_pPedalPlate->cMaterial;
			m_nSegI = m_pPedalPlate->iSeg;
			//初始化设计参数
			long type = 0;
			m_pPedalPlate->GetDesignItemValue('V', &type);
			para.ciDirection = (BYTE)type;
			m_pPedalPlate->GetDesignItemValue('C', &para.C);
			m_pPedalPlate->GetDesignItemValue('L', &para.L);
			m_pPedalPlate->GetDesignItemValue('W', &para.W);
			m_pPedalPlate->GetDesignItemValue('H', &para.H);
			m_pPedalPlate->GetDesignItemValue('D', &para.D);
			m_pPedalPlate->GetDesignItemValue('S', &para.S);
		}
		if (m_bNewPlate&&m_pPedalPlate == NULL)
		{
			m_pPedalPlate = &_previewPlate;
			m_xPreBoltList.Empty();
			InitPedalPlatePara();
			CreatePedalPlate(m_pPedalPlate);
		}
		//显示肋板参数
		m_propList.SetFocus();
		DisplayPlatePropList();
		DisplaySubPropList(NULL);
	}
	UpdateSketchMap();
	//
	CRect rect;
	GetWindowRect(rect);
	int width = rect.Width();
	int height = rect.Height();
	rect.left = nScrLocationX;
	rect.top = nScrLocationY;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	MoveWindow(rect, TRUE);
	m_bEnableWindowMoveListen = TRUE;
	return TRUE;
}
IMPLEMENT_PROP_FUNC(CDesPedalPlateDlg);
const DWORD HASHTABLESIZE = 500;
void CDesPedalPlateDlg::InitPropHashtable()
{
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(50);
	int id = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	AddPropItem("preview", PROPLIST_ITEM(id++, "preview", "", ""));
	//
	AddPropItem("basicInfo", PROPLIST_ITEM(id++, "basicinfo"));
	AddPropItem("sPartNo", PROPLIST_ITEM(id++, "partno"));
	AddPropItem("iSeg", PROPLIST_ITEM(id++, "seg"));
	AddPropItem("material", PROPLIST_ITEM(id++, "material", "", MakeMaterialMarkSetString()));
	AddPropItem("thick", PROPLIST_ITEM(id++, "thick", ""));
	//
	AddPropItem("location", PROPLIST_ITEM(id++, "location", ""));
	AddPropItem("hDatumAngle", PROPLIST_ITEM(id++, "DatumAngle", ));
	AddPropItem("hDatumBolt1", PROPLIST_ITEM(id++, "DatumBolt1",));
	AddPropItem("hDatumBolt2", PROPLIST_ITEM(id++, "DatumBolt2",));
	AddPropItem("hDatumBolt3", PROPLIST_ITEM(id++, "DatumBolt3",));
	AddPropItem("hDatumBolt4", PROPLIST_ITEM(id++, "DatumBolt4",));
	AddPropItem("ciWoreWing", PROPLIST_ITEM(id++, "WoreWing", "WoreWing", "0.X|1.Y"));
	AddPropItem("ciDirection", PROPLIST_ITEM(id++, "Direction", "Direction", "0.Wing|1.Ber"));
	AddPropItem("nLsD", PROPLIST_ITEM(id++, "Bolt_D"));
	AddPropItem("nLsNum", PROPLIST_ITEM(id++, "Bolt_Num"));
	AddPropItem("nLsSpace", PROPLIST_ITEM(id++, "Bolt_space"));
	//
	AddPropItem("profile", PROPLIST_ITEM(id++, "profile"));
	AddPropItem("para.C", PROPLIST_ITEM(id++, "Param C", "", "0.profileI|1.profileII"));
	AddPropItem("para.L", PROPLIST_ITEM(id++, "Param L", ""));
	AddPropItem("para.W", PROPLIST_ITEM(id++, "Param W", ""));
	AddPropItem("para.H", PROPLIST_ITEM(id++, "Param H", ""));
	AddPropItem("para.D", PROPLIST_ITEM(id++, "Param D", ""));
	AddPropItem("para.S", PROPLIST_ITEM(id++, "Param S"));
#else 
	AddPropItem("preview", PROPLIST_ITEM(id++, "预览环板", "支持预览", "预览"));
	//
	AddPropItem("basicInfo", PROPLIST_ITEM(id++, "基本信息"));
	AddPropItem("sPartNo", PROPLIST_ITEM(id++, "构件编号"));
	AddPropItem("iSeg", PROPLIST_ITEM(id++, "段号"));
	AddPropItem("material", PROPLIST_ITEM(id++, "材质", "材质", MakeMaterialMarkSetString()));
	AddPropItem("thick", PROPLIST_ITEM(id++, "厚度", "厚度参数T"));
	//
	AddPropItem("location", PROPLIST_ITEM(id++, "装配方位", "装配方位参数")); 
	AddPropItem("hDatumAngle", PROPLIST_ITEM(id++, "基准角钢", "定位基准角钢句柄"));
	AddPropItem("hDatumBolt1", PROPLIST_ITEM(id++, "关联螺栓1", "定位基准螺栓句柄"));
	AddPropItem("hDatumBolt2", PROPLIST_ITEM(id++, "关联螺栓2", "定位基准螺栓句柄"));
	AddPropItem("hDatumBolt3", PROPLIST_ITEM(id++, "关联螺栓3", "定位基准螺栓句柄"));
	AddPropItem("hDatumBolt4", PROPLIST_ITEM(id++, "关联螺栓4", "定位依赖螺栓句柄"));
	AddPropItem("para.ciDirection", PROPLIST_ITEM(id++, "延伸方向", "延伸方向方式", "0.肢翼侧|1.楞线侧"));
	AddPropItem("layout_para.nBoltD", PROPLIST_ITEM(id++, "螺栓直径", "螺栓直径","12|16|20|24"));
	AddPropItem("layout_para.nBoltNum", PROPLIST_ITEM(id++, "螺栓数量", "连接螺栓数"));
	AddPropItem("layout_para.nBoltSpace", PROPLIST_ITEM(id++, "螺栓间距", "螺栓间距"));
	AddPropItem("layout_para.ciWoreWing", PROPLIST_ITEM(id++, "所在工作肢", "法向所在肢", "0.X肢|1.Y肢"));
	AddPropItem("layout_para.ciGType", PROPLIST_ITEM(id++, "所在准据", "", "g|g1|g2|g3|自定义"));
	AddPropItem("layout_para.nGDist", PROPLIST_ITEM(id++, "肢向偏移", "肢向偏移距里"));
	AddPropItem("layout_para.fSpace", PROPLIST_ITEM(id++, "踏板间距"));
	AddPropItem("layout_para.fSumLen", PROPLIST_ITEM(id++, "角钢长度"));
	AddPropItem("layout_para.fCanLen", PROPLIST_ITEM(id++, "可用长度"));
	AddPropItem("layout_para.fPreRemainDist", PROPLIST_ITEM(id++, "前剩余量","前角钢剩余的长度"));
	//
	AddPropItem("profile", PROPLIST_ITEM(id++, "外形设计"));
	AddPropItem("para.C", PROPLIST_ITEM(id++, "外形样式", "", "0.外形I|1.外形II"));
	AddPropItem("para.S", PROPLIST_ITEM(id++, "连接宽度S", "S=0,默认延伸到轮廓边"));
	AddPropItem("para.L", PROPLIST_ITEM(id++, "踏板长度L", ""));
	AddPropItem("para.W", PROPLIST_ITEM(id++, "踏板宽度W", ""));
	AddPropItem("para.H", PROPLIST_ITEM(id++, "挡板高度H", ""));
	AddPropItem("para.D", PROPLIST_ITEM(id++, "台口高度D", ""));
#endif
}
int CDesPedalPlateDlg::GetPropValueStr(long id, char *valueStr, UINT nMaxStrBufLen/*=100*/)
{	//根据属性ID得到属性值
	CXhChar100 sText;
	if (GetPropID("sPartNo") == id)
		sText.Copy(m_sPartNo);
	else if (GetPropID("iSeg") == id)
		sText.Copy(m_nSegI.ToString());
	else if (GetPropID("thick") == id)
		sText.Printf("%d", m_nThick);
	else if (GetPropID("material") == id)
	{
		char matStr[20] = "";
		QuerySteelMatMark(m_cMaterial, matStr);
		sText.Printf("%s", matStr);
	}
	else if (GetPropID("hDatumAngle") == id)
	{
		sText.Copy("0X0");
		if (m_pDatumAngle)
			sText.Printf("0X%X", m_pDatumAngle->handle);
	}
	else if (GetPropID("para.ciDirection") == id)
	{
		if (para.ciDirection == 0)
			sText.Copy("0.肢翼侧");
		else
			sText.Copy("1.楞线侧");
	}
	else if (GetPropID("layout_para.nBoltD") == id)
		sText.Printf("%d", layout_para.m_nBoltD);
	else if (GetPropID("layout_para.nBoltNum") == id)
		sText.Printf("%d", layout_para.m_nBoltNum);
	else if (GetPropID("layout_para.nBoltSpace") == id)
		sText.Printf("%d", layout_para.m_nBoltSpace);
	else if (GetPropID("layout_para.ciWoreWing") == id)
	{
		if (layout_para.m_ciWorkWing == 0)
			sText.Copy("0.X肢");
		else
			sText.Copy("1.Y肢");
	}
	else if (GetPropID("layout_para.ciGType") == id)
	{
		if (layout_para.m_ciGType == 0)
			sText.Copy("g");
		else if (layout_para.m_ciGType == 1)
			sText.Copy("g1");
		else if (layout_para.m_ciGType == 2)
			sText.Copy("g2");
		else if (layout_para.m_ciGType == 3)
			sText.Copy("g3");
		else
			sText.Copy("自定义");
	}
	else if (GetPropID("layout_para.nGDist") == id)
		sText.Printf("%d", layout_para.m_nGDist);
	else if (GetPropID("layout_para.fSpace") == id)
	{
		sText.Printf("%f", layout_para.m_fSpace);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("layout_para.fSumLen") == id)
	{
		sText.Printf("%f", layout_para.m_fSumLen);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("layout_para.fCanLen") == id)
	{
		sText.Printf("%f", layout_para.m_fCanLen);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("layout_para.fPreRemainDist") == id)
	{
		sText.Printf("%f", layout_para.m_fPreRemainDist);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("para.C") == id)
	{
		if (para.C == 0)
			sText.Copy("0.外形I");
		else
			sText.Copy("1.外形II");
	}
	else if (GetPropID("para.L") == id)
	{
		sText.Printf("%f", para.L);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("para.W") == id)
	{
		sText.Printf("%f", para.W);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("para.H") == id)
	{
		sText.Printf("%f", para.H);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("para.D") == id)
	{
		sText.Printf("%f", para.D);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("para.S") == id)
	{
		sText.Printf("%f", para.S);
		SimplifiedNumString(sText);
	}
	if (valueStr)
		StrCopy(valueStr, sText, nMaxStrBufLen);
	return strlen(sText);
}
void CDesPedalPlateDlg::DisplayPlatePropList()
{
	m_propList.CleanTree();
	m_propList.SetModifyValueFunc(ModifyPedalPlateProperty);
	m_propList.SetButtonClickFunc(ButtonClickPedalPlate);
	m_propList.SetPropHelpPromptFunc(FirePedalPlateHelpStr);
	//
	CPropertyListOper<CDesPedalPlateDlg> oper(&m_propList, this);
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL, *pRoot = m_propList.GetRootItem();
	//基本信息
	pGroupItem = oper.InsertPropItem(pRoot, "basicInfo");
	oper.InsertEditPropItem(pGroupItem, "sPartNo");
	oper.InsertEditPropItem(pGroupItem, "iSeg");
	oper.InsertCmbListPropItem(pGroupItem, "material");
	oper.InsertEditPropItem(pGroupItem, "thick");
	//装配方位
	pGroupItem = oper.InsertPropItem(pRoot, "location");
	pPropItem = oper.InsertButtonPropItem(pGroupItem, "hDatumAngle");
	pPropItem->SetReadOnly();
	if (m_bNewPlate)
		oper.InsertCmbListPropItem(pGroupItem, "layout_para.ciWoreWing");
	oper.InsertCmbListPropItem(pGroupItem, "para.ciDirection");
	if (m_bNewPlate)
	{	//布置螺栓信息
		oper.InsertCmbListPropItem(pGroupItem, "layout_para.ciGType");
		pPropItem=oper.InsertEditPropItem(pGroupItem, "layout_para.nGDist");
		pPropItem->SetReadOnly(layout_para.m_ciGType != 4);
		oper.InsertEditPropItem(pGroupItem, "layout_para.nBoltNum");
		oper.InsertCmbListPropItem(pGroupItem, "layout_para.nBoltD");
		oper.InsertEditPropItem(pGroupItem, "layout_para.nBoltSpace");
		oper.InsertEditPropItem(pGroupItem, "layout_para.fSpace");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "layout_para.fSumLen");
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "layout_para.fCanLen");
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "layout_para.fPreRemainDist");
		pPropItem->SetReadOnly();
	}
	//
	m_propList.Redraw();
}
//显示子属性
void CDesPedalPlateDlg::DisplaySubPropList(CPropTreeItem *pItem/*=NULL*/)
{
	m_xSubPropList.CleanTree();
	m_xSubPropList.SetModifyValueFunc(ModifyPedalPlateProperty);
	m_xSubPropList.SetButtonClickFunc(ButtonClickPedalPlate);
	//
	CPropertyListOper<CDesPedalPlateDlg> oper(&m_xSubPropList, this);
	CPropTreeItem *pGroupItem = NULL, *pPropItem = NULL, *pRoot = m_xSubPropList.GetRootItem();
	pGroupItem = oper.InsertPropItem(pRoot, "profile");
	oper.InsertCmbListPropItem(pGroupItem, "para.C", "", "", "", -1, TRUE);
	oper.InsertEditPropItem(pGroupItem, "para.L", "", "", -1, TRUE);
	oper.InsertEditPropItem(pGroupItem, "para.W", "", "", -1, TRUE);
	oper.InsertEditPropItem(pGroupItem, "para.H", "", "", -1, TRUE);
	oper.InsertEditPropItem(pGroupItem, "para.D", "", "", -1, TRUE);
	oper.InsertEditPropItem(pGroupItem, "para.S", "", "", -1, TRUE);
	//
	m_xSubPropList.Redraw();
}
void CDesPedalPlateDlg::UpdateSketchMap()
{
	m_xCurPlateBmp.Detach();
	if (para.C==0)
		m_xCurPlateBmp.LoadBitmap(IDB_BMP_PEDAL_PLATE1);
	else
		m_xCurPlateBmp.LoadBitmap(IDB_BMP_PEDAL_PLATE2);
	CStatic *pPic = (CStatic*)GetDlgItem(IDC_S_PIC_MAP);
	pPic->SetBitmap(m_xCurPlateBmp);
}
void CDesPedalPlateDlg::FinishSelectObjOper()
{
	if (!m_bInernalStart)
		return;
	long hPart = 0;
	if (GetEventPropId() == GetPropID("hDatumAngle"))
	{
		CLDSLineAngle *pPart = (CLDSLineAngle*)console.FromPartHandle(m_hPickObj, CLS_LINEANGLE);
		if (pPart&&pPart->GetClassTypeId() == CLS_LINEANGLE)
		{
			m_pDatumAngle = pPart;
			ModifyPlateParam('A', pPart->handle);
		}
	}
}
void CDesPedalPlateDlg::ClearPreBolts()
{
	for (CLDSBolt* pBolt = m_xPreBoltList.GetFirst(); pBolt; pBolt = m_xPreBoltList.GetNext())
	{
		m_pDatumAngle->DeleteLsRef(pBolt->handle);
		m_pPedalPlate->DeleteLsRef(pBolt->handle);
		//
		g_pSolidDraw->DelEnt(pBolt->handle);
		console.DispPartSet.DeleteNode(pBolt->handle);
		console.DeletePart(pBolt->handle);
	}
	m_xPreBoltList.Empty();
}
void CDesPedalPlateDlg::InitPedalPlatePara()
{
	if (!m_bNewPlate || m_pDatumAngle==NULL)
		return;
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace, 16);
	JGZJ jgzj;
	getjgzj(jgzj, m_pDatumAngle->GetWidth());
	if (m_pDatumAngle->m_bEnableTeG)
		jgzj = (layout_para.m_ciWorkWing == 0) ? m_pDatumAngle->xWingXZhunJu : m_pDatumAngle->xWingYZhunJu;
	m_nSegI = m_pDatumAngle ? m_pDatumAngle->iSeg : 0;
	m_cMaterial = 'S';
	m_nThick = 6;
	layout_para.m_nBoltNum = 2;
	layout_para.m_nBoltD = 16;
	layout_para.m_nBoltSpace = LsSpace.SingleRowSpace;
	layout_para.m_fSpace = g_sysPara.FootNailDist * 2;
	layout_para.m_nGDist = jgzj.g;
	para.L = 120;
	para.W = 40;
	para.H = 40;
	para.D = 60;
	para.S = 0;
	para.C = 0;
	para.ciDirection = 0;
	if (m_bValidStaticRibParam)
	{	//上次录入的有效参数
		m_nSegI = s_xBasicParam.nSegI;
		m_cMaterial = s_xBasicParam.cMaterial;
		m_nThick = s_xBasicParam.nThick;
		layout_para.m_nBoltNum = s_xLayoutParam.m_nBoltNum;
		layout_para.m_nBoltD = s_xLayoutParam.m_nBoltD;
		layout_para.m_nBoltSpace = s_xLayoutParam.m_nBoltSpace;
		layout_para.m_nGDist = s_xLayoutParam.m_nGDist;
		layout_para.m_fSpace = s_xLayoutParam.m_fSpace;
		para = s_xDesParam;
	}
}
double CDesPedalPlateDlg::CalOffDist()
{
	double offset_dist = layout_para.m_fUseLen + layout_para.m_fSpace;
	if (layout_para.m_fExtraLen > 0)
		offset_dist += layout_para.m_fExtraLen;
	if (layout_para.m_fJointGap > 0)
		offset_dist += layout_para.m_fJointGap;
	if (layout_para.m_fPreRemainDist > 0)
		offset_dist -= layout_para.m_fPreRemainDist;
	return offset_dist;
}
void CDesPedalPlateDlg::LayoutBolt(CLDSParamPlate* pPlate,BOOL bNewCreate/*=FALSE*/)
{
	if (m_pDatumAngle == NULL || layout_para.m_nBoltNum <= 0)
		return;
	ClearPreBolts();	//清空之前的螺栓信息
	//填充螺栓设计信息
	double offset_dist = CalOffDist();
	CXhChar16 sGuiGe("M%d", layout_para.m_nBoltD);
	CLDSBolt ls(console.GetActiveModel());
	ls.des_base_pos.hPart = m_pDatumAngle->handle;
	ls.des_base_pos.len_offset_dist = ftoi(offset_dist);
	ls.des_base_pos.wing_offset_dist = layout_para.m_nGDist;
	ls.des_base_pos.offset_wing = layout_para.m_ciWorkWing;
	ls.des_base_pos.direction = layout_para.m_ciLineS0E1;
	sprintf(ls.des_base_pos.norm_offset.key_str, "-0X%X", m_pDatumAngle->handle);
	ls.des_base_pos.datumPoint.datum_pos_style = 1;	//角钢上端楞点为基准
	ls.des_base_pos.datumPoint.des_para.RODEND.hRod = m_pDatumAngle->handle;
	ls.des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect = TRUE;
	ls.des_base_pos.datumPoint.des_para.RODEND.direction = layout_para.m_ciLineS0E1;
	ls.des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist = 0;
	ls.des_base_pos.datumPoint.des_para.RODEND.wing_offset_style = 4;
	ls.des_base_pos.datumPoint.des_para.RODEND.offset_wing = layout_para.m_ciWorkWing;
	ls.des_work_norm.norm_style = 1;
	ls.des_work_norm.hVicePart = m_pDatumAngle->handle;
	ls.des_work_norm.norm_wing = layout_para.m_ciWorkWing;
	ls.des_work_norm.direction = 0;
	//设计螺栓
	CLDSBolt* pBolt = NULL;
	for (int i = 0; i < layout_para.m_nBoltNum; i++)
	{
		if (bNewCreate)
			pBolt = (CLDSBolt*)console.AppendPart(CLS_BOLT);
		else
		{
			pBolt = m_xPreBoltList.append();
			pBolt->handle = _previewPlate.handle + i + 1;
			pBolt->SetBelongModel(console.GetActiveModel());
		}
		pBolt->CopyProperty(&ls, false);
		pBolt->layer(0) = m_pDatumAngle->Layer(0);
		pBolt->cfgword = m_pDatumAngle->cfgword;
		pBolt->iSeg = m_pDatumAngle->iSeg;
		pBolt->set_d(layout_para.m_nBoltD);
		pBolt->des_base_pos = ls.des_base_pos;
		pBolt->des_base_pos.len_offset_dist += i*layout_para.m_nBoltSpace;
		pBolt->des_work_norm = ls.des_work_norm;
		pBolt->AddL0Thick(m_pDatumAngle->handle, TRUE);
		pBolt->AddL0Thick(pPlate->handle, TRUE);
		pBolt->CalGuigeAuto();
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		//
		m_pDatumAngle->AppendMidLsRef(pBolt->GetLsRef());
		pPlate->AppendLsRef(pBolt->GetLsRef());
	}
	g_pSolidDraw->Draw();
}
CLDSParamPlate *CDesPedalPlateDlg::CreatePedalPlate(CLDSParamPlate* pToExistPlate/*=NULL*/)
{
	if (m_pDatumAngle == NULL)
		return NULL;
	BOOL bNewCreate = FALSE;
	CLDSParamPlate *pPedalPlate = NULL;
	if (pToExistPlate == NULL && (m_pPedalPlate == NULL || m_pPedalPlate == &_previewPlate))
	{
		m_pPedalPlate = pPedalPlate = (CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		bNewCreate = TRUE;
	}
	else if (pToExistPlate != NULL)
		pPedalPlate = pToExistPlate;
	else
		pPedalPlate = m_pPedalPlate = &_previewPlate;
	//
	pPedalPlate->m_iParamType = TYPE_PEDALPLATE;
	pPedalPlate->m_hPartWeldParent = m_pDatumAngle->handle;
	pPedalPlate->CopyModuleInstanceInfo(m_pDatumAngle);
	pPedalPlate->thick = m_nThick;
	pPedalPlate->iSeg = m_nSegI;
	pPedalPlate->cMaterial = m_cMaterial;
	pPedalPlate->SetPartNo(m_sPartNo);
	pPedalPlate->SetDesignItemValue('A', m_pDatumAngle->handle, NULL, PARAM_POS, 1);
	pPedalPlate->SetDesignItemValue('V', (long)para.ciDirection, NULL, PARAM_POS, 1);
	pPedalPlate->SetDesignItemValue('C', para.C, NULL, PARAM_SHAPE, 1);
	pPedalPlate->SetDesignItemValue('L', para.L, NULL, PARAM_SHAPE);
	pPedalPlate->SetDesignItemValue('W', para.W, NULL, PARAM_SHAPE);
	pPedalPlate->SetDesignItemValue('H', para.H, NULL, PARAM_SHAPE);
	pPedalPlate->SetDesignItemValue('D', para.D, NULL, PARAM_SHAPE);
	pPedalPlate->SetDesignItemValue('S', para.S, NULL, PARAM_SHAPE);
	if (m_bNewPlate)	//布置螺栓
		LayoutBolt(pPedalPlate, bNewCreate);
	pPedalPlate->DesignPlate();
	pPedalPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole, g_pSolidOper->GetScaleUserToScreen(), g_pSolidSet->GetArcSamplingLength());
	g_pSolidDraw->NewSolidPart(pPedalPlate->GetSolidPartObject());
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(pPedalPlate->handle);
	g_pSolidDraw->Draw();
	//
	SCOPE_STRU scope;
	scope.VerifyVertex(pPedalPlate->ucs.origin);
	scope.fMinX -= 200;
	scope.fMaxX += 200;
	scope.fMinY -= 200;
	scope.fMaxY += 200;
	scope.fMinZ -= 200;
	scope.fMaxZ += 200;
	g_pSolidOper->FocusTo(scope, 0.4);
	//保存操作数据
	m_bValidStaticRibParam = true;
	s_xDesParam = para;
	s_xLayoutParam = layout_para;
	s_xBasicParam.cMaterial = m_cMaterial;
	s_xBasicParam.nThick = m_nThick;
	return pPedalPlate;
}
void CDesPedalPlateDlg::ReDesignPlate()
{
	if (m_pPedalPlate&&m_pPedalPlate->DesignPlate())
	{
		m_pPedalPlate->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(m_pPedalPlate->GetSolidPartObject());
		g_pSolidDraw->AddCS(m_pPedalPlate->handle, m_pPedalPlate->ucs);
		CLDSView::RedrawPlateProfileDrawing(m_pPedalPlate);
		g_pSolidDraw->Draw();
	}
}
void CDesPedalPlateDlg::ModifyPlateParam(WORD wKey, double fValue)
{
	if (m_pPedalPlate == NULL)
		return;
	DESIGN_PLATE_ITEM *pItem = m_pPedalPlate->GetDesignItem(wKey);
	if (pItem)
		pItem->value.fValue = fValue;
	else
		m_pPedalPlate->SetDesignItemValue(wKey, fValue, "", PARAM_SHAPE, 0);
}
void CDesPedalPlateDlg::ModifyPlateParam(WORD wKey, long nValue)
{
	if (m_pPedalPlate == NULL)
		return;
	DESIGN_PLATE_ITEM *pItem = m_pPedalPlate->GetDesignItem(wKey);
	if (pItem)
		pItem->value.hPart = nValue;
	else
		m_pPedalPlate->SetDesignItemValue(wKey, nValue, "", PARAM_SHAPE, 1);
}
void CDesPedalPlateDlg::DeletePlateParam(WORD wKey)
{
	if (m_pPedalPlate == NULL)
		return;
	m_pPedalPlate->DeleteDesignItem(wKey);
}
//
int CDesPedalPlateDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen = FALSE;
	if (CCallBackDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
void CDesPedalPlateDlg::OnMove(int x, int y)
{
	CCallBackDialog::OnMove(x, y);
	if (m_bEnableWindowMoveListen)
	{
		nScrLocationX = x;
		nScrLocationY = y;
	}
}
void CDesPedalPlateDlg::OnOK()
{
}
void CDesPedalPlateDlg::OnCancel()
{
	g_pSolidDraw->DelEnt(_previewPlate.handle);
	ClearPreBolts();
	if (m_bNewPlate&&m_pPedalPlate)
	{
		g_pSolidDraw->DelCS(m_pPedalPlate->handle);
		g_pSolidDraw->DelEnt(m_pPedalPlate->handle);
		console.DispPartSet.DeleteNode(m_pPedalPlate->handle);
		console.DeletePart(m_pPedalPlate->handle);
		g_pSolidDraw->Draw();
	}
	else if (!m_bNewPlate&&m_pPedalPlate)
	{	//查看完肋板参数关闭对话框之后重新选中肋板
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pPedalPlate->handle);
	}
	CDialog::OnCancel();
}
void CDesPedalPlateDlg::OnClose()
{
	g_pSolidDraw->DelEnt(_previewPlate.handle);
	ClearPreBolts();
	if (m_bNewPlate&&m_pPedalPlate)
	{
		g_pSolidDraw->DelCS(m_pPedalPlate->handle);
		g_pSolidDraw->DelEnt(m_pPedalPlate->handle);
		console.DispPartSet.DeleteNode(m_pPedalPlate->handle);
		console.DeletePart(m_pPedalPlate->handle);
		g_pSolidDraw->Draw();
	}
	else if (!m_bNewPlate&&m_pPedalPlate)
	{	//查看完肋板参数关闭对话框之后重新选中肋板
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pPedalPlate->handle);
	}
	CDialog::OnClose();
}
void CDesPedalPlateDlg::OnConfirm()
{
	g_pSolidDraw->DelEnt(_previewPlate.handle);
	ClearPreBolts();
	if(m_pPedalPlate)
		g_pSolidDraw->DelCS(m_pPedalPlate->handle);
	if (m_pPedalPlate && !m_bNewPlate)
	{	//查看完肋板参数关闭对话框之后重新选中肋板
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pPedalPlate->handle);
	}
	if (m_pPedalPlate == &_previewPlate)
		m_pPedalPlate = NULL;
	CDialog::OnOK();
}
void CDesPedalPlateDlg::OnBnModelViewOper()
{	//临时中断转换至模型操作环境中，以便于用户对三维模型缩放、旋转查看等操作
	ExecuteCommand(this, GetPropID("preview"));
}