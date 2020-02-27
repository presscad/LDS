//<LOCALE_TRANSLATE Confirm by hxr/>
// DesignArcPartDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignArcPartDlg.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "Query.h"
#include ".\designarcpartdlg.h"
#include "DisplayPointProperty.h"
#include "DisplayVectorProperty.h"
#include "LicFuncDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignArcPartDlg dialog

static BOOL DatumPointButtonClick(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	BOOL bRetCode = CDisplayPointProperty::DatumPointButtonClick(pPropList,pItem);
	CDesignArcPartDlg *pArcPartDlg = (CDesignArcPartDlg*)pPropList->GetParent();
	if(pArcPartDlg!=NULL)
		pArcPartDlg->RefreshEditText();
	return bRetCode;
}

static BOOL DatumVectorButtonClick(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	BOOL bRetCode = CDisplayVectorProperty::VectorButtonClick(pPropList,pItem);
	CDesignArcPartDlg *pArcPartDlg = (CDesignArcPartDlg*)pPropList->GetParent();
	if(pArcPartDlg!=NULL)
		pArcPartDlg->RefreshEditText();
	return bRetCode;
}

static BOOL ModifyDatumPointPropValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	BOOL bRetCode = CDisplayPointProperty::ModifyDatumPointPropValue(pPropList,pItem,valueStr);
	CDesignArcPartDlg *pArcPartDlg = (CDesignArcPartDlg*)pPropList->GetParent();
	if(pArcPartDlg!=NULL)
		pArcPartDlg->RefreshEditText();
	return bRetCode;
}

static BOOL ModifyDatumVectorPropValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	BOOL bRetCode = CDisplayVectorProperty::ModifyDatumVectorPropValue(pPropList,pItem,valueStr);
	CDesignArcPartDlg *pArcPartDlg = (CDesignArcPartDlg*)pPropList->GetParent();
	if(pArcPartDlg!=NULL)
		pArcPartDlg->RefreshEditText();
	return bRetCode;
}

CDesignArcPartDlg::CDesignArcPartDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesignArcPartDlg::IDD, pParent)
	, m_sCurObjType(_T(""))
{
	//{{AFX_DATA_INIT(CDesignArcPartDlg)
	m_fWorkNormX = 0.0;
	m_fWorkNormY = 0.0;
	m_fWorkNormZ = -1.0;
	m_fCenterX = 0.0;
	m_fCenterY = 0.0;
	m_fCenterZ = 0.0;
	m_fStartPosX = 1000.0;
	m_fStartPosY = 0.0;
	m_fStartPosZ = 0.0;
	m_iMaterial = 0;
	m_sPartNo = _T("");
	m_sSegI = _T("1");
	m_fStartAngle = 0;
	m_iPartType = 0;
	m_sPartGuiGe = _T("");
	m_iLayoutStyle = 0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sGuiGeName = _T("Angle specification:");
#else
	m_sGuiGeName = _T("角钢规格:");
#endif
	m_fOffsetH = 20.0;
	m_fOffsetV = -20.0;
	m_fArcAngle = 90.0;
	//}}AFX_DATA_INIT
	m_norm.norm_style = 0;
	m_norm.vector.Set(0,0,-1);
	m_center.datum_pos_style = 0;
	m_arcStart.datum_pos_style = 0;
	m_arcStart.SetPosition(m_fStartPosX,m_fStartPosY,m_fStartPosZ);
	m_iCurDesParaType = PARA_TYPE_NORM;
}


void CDesignArcPartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignArcPartDlg)
	DDX_Text(pDX, IDC_E_WORK_NORM_X, m_fWorkNormX);
	DDX_Text(pDX, IDC_E_WORK_NORM_Y, m_fWorkNormY);
	DDX_Text(pDX, IDC_E_WORK_NORM_Z, m_fWorkNormZ);
	DDX_Text(pDX, IDC_E_CENTER_POS_X, m_fCenterX);
	DDX_Text(pDX, IDC_E_CENTER_POS_Y, m_fCenterY);
	DDX_Text(pDX, IDC_E_CENTER_POS_Z, m_fCenterZ);
	DDX_Text(pDX, IDC_E_START_POS_X, m_fStartPosX);
	DDX_Text(pDX, IDC_E_START_POS_Y, m_fStartPosY);
	DDX_Text(pDX, IDC_E_START_POS_Z, m_fStartPosZ);
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_SEG_I, m_sSegI);
	DDX_Text(pDX, IDC_E_START_ANGLE, m_fStartAngle);
	DDX_Radio(pDX, IDC_RDO_PART_TYPE, m_iPartType);
	DDX_CBString(pDX, IDC_CMB_PART_GUIGE, m_sPartGuiGe);
	DDX_CBIndex(pDX, IDC_CMB_LAYOUT_STYLE, m_iLayoutStyle);
	DDX_Text(pDX, IDC_S_GUIGE_NAME, m_sGuiGeName);
	DDX_Text(pDX, IDC_E_OFFSET_H, m_fOffsetH);
	DDX_Text(pDX, IDC_E_OFFSET_V, m_fOffsetV);
	DDX_Text(pDX, IDC_E_ARC_ANGLE, m_fArcAngle);
	DDV_MinMaxDouble(pDX, m_fArcAngle, 0., 360.);
	DDX_Text(pDX, IDC_S_CUR_OBJ, m_sCurObjType);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST_BOX_SUB, m_xPropList);
}


BEGIN_MESSAGE_MAP(CDesignArcPartDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignArcPartDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_WORK_NORM, OnBnPasteWorkNorm)
	ON_BN_CLICKED(IDC_BN_PASTE_CENTER_POS, OnBnPasteCenter)
	ON_BN_CLICKED(IDC_BN_PASTE_START_POS, OnBnPasteStartPos)
	ON_BN_CLICKED(IDC_BN_COPY_WORK_NORM, OnBnCopyWorkNorm)
	ON_BN_CLICKED(IDC_BN_COPY_CENTER_POS, OnBnCopyCenter)
	ON_BN_CLICKED(IDC_BN_COPY_START_POS, OnBnCopyStartPos)
	ON_BN_CLICKED(IDC_RDO_PART_TYPE, OnPartType)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangePartNo)
	ON_CBN_SELCHANGE(IDC_CMB_PART_GUIGE, OnSelchangePartGuige)
	ON_CBN_KILLFOCUS(IDC_CMB_PART_GUIGE, OnKillfocusPartGuige)
	ON_CBN_SELCHANGE(IDC_CMB_LAYOUT_STYLE, OnSelchangeLayoutStyle)
	ON_EN_CHANGE(IDC_E_CENTER_POS_Z, OnChangeCenterPosZ)
	ON_EN_CHANGE(IDC_E_START_POS_Z, OnChangeStartPosZ)
	ON_BN_CLICKED(IDC_RADIO12, OnPartType)
	ON_BN_CLICKED(IDC_RADIO14, OnPartType)
	ON_BN_CLICKED(IDC_RADIO15, OnPartType)
	ON_EN_CHANGE(IDC_E_ARC_ANGLE, OnChangeArcAngle)
	//}}AFX_MSG_MAP
	ON_EN_SETFOCUS(IDC_E_WORK_NORM_X, OnEnSetFocusWorkNorm)
	ON_EN_SETFOCUS(IDC_E_WORK_NORM_Y, OnEnSetFocusWorkNorm)
	ON_EN_SETFOCUS(IDC_E_WORK_NORM_Z, OnEnSetFocusWorkNorm)
	ON_EN_SETFOCUS(IDC_E_CENTER_POS_X, OnEnSetFocusCenterPos)
	ON_EN_SETFOCUS(IDC_E_CENTER_POS_Y, OnEnSetFocusCenterPos)
	ON_EN_SETFOCUS(IDC_E_CENTER_POS_Z, OnEnSetFocusCenterPos)
	ON_EN_SETFOCUS(IDC_E_START_POS_X, OnEnSetFocusStartPos)
	ON_EN_SETFOCUS(IDC_E_START_POS_Y, OnEnSetFocusStartPos)
	ON_EN_SETFOCUS(IDC_E_START_POS_Z, OnEnSetFocusStartPos)
	ON_BN_CLICKED(ID_BTN_CONFIRM, OnBnClickedBtnConfirm)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignArcPartDlg message handlers

BOOL CDesignArcPartDlg::OnInitDialog() 
{
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MATERIAL);
	//材质
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(m_iMaterial);
	//规格
	pCMB = (CComboBox*)GetDlgItem(IDC_CMB_PART_GUIGE);
	if(m_iPartType==0)		//角钢
		AddJgRecord(pCMB);
	else if(m_iPartType==1)	//槽钢
		AddSlotRecord(pCMB);
	else if(m_iPartType==2)	//扁钢
		AddBianRecord(pCMB);
	else if(m_iPartType==3)	//钢管
		AddTubeRecord(pCMB);
	if(m_sPartGuiGe.IsEmpty())
	{
		pCMB->SetCurSel(0);
		pCMB->GetLBText(0,m_sPartGuiGe);
		RetrievePreferOffsetParams(m_sPartGuiGe);
	}
	UpdateData(FALSE);
	RefreshDlgCtrlState();

	//内部启动
	if(m_bInernalStart)	
		FinishSelectObjOper();	//完成选择对象的后续操作
	DisplaySubPropList();
	if(m_bInernalStart)	
	{	//恢复选中项
		CPropTreeItem *pItem=m_xPropList.FindItemByPropId(m_idEventProp,NULL);
		m_xPropList.SetFocus();
		if(pItem)
			m_xPropList.SetCurSel(pItem->m_iIndex);	//设置选中项	
	}
	RefreshEditText();
	return CDialog::OnInitDialog();
}

void CDesignArcPartDlg::OnBnPasteWorkNorm() 
{
	if(m_norm.norm_style!=0)
		return;
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fWorkNormX = point.x;
		m_fWorkNormY = point.y;
		m_fWorkNormZ = point.z;
		UpdateData(FALSE);
		m_norm.vector = point;
		OnEnSetFocusWorkNorm();
	}
}

void CDesignArcPartDlg::OnBnPasteCenter() 
{
	if(m_center.datum_pos_style!=0)
		return;
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fCenterX = point.x;
		m_fCenterY = point.y;
		m_fCenterZ = point.z;
		UpdateData(FALSE);
		m_center.SetPosition(point);
		OnEnSetFocusCenterPos();}
}

void CDesignArcPartDlg::OnBnPasteStartPos() 
{
	if(m_arcStart.datum_pos_style!=0)
		return;
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fStartPosX = point.x;
		m_fStartPosY = point.y;
		m_fStartPosZ = point.z;
		UpdateData(FALSE);
		m_arcStart.SetPosition(point);
		OnEnSetFocusStartPos();
	}
}

void CDesignArcPartDlg::OnBnCopyWorkNorm() 
{
	f3dPoint point;
	point.Set(m_fWorkNormX,m_fWorkNormY,m_fWorkNormZ);
	WritePointToClip(point);
}

void CDesignArcPartDlg::OnBnCopyCenter() 
{
	f3dPoint point;
	point.Set(m_fCenterX,m_fCenterY,m_fCenterZ);
	WritePointToClip(point);
}

void CDesignArcPartDlg::OnBnCopyStartPos() 
{
	f3dPoint point;
	point.Set(m_fStartPosX,m_fStartPosY,m_fStartPosZ);
	WritePointToClip(point);
}

void CDesignArcPartDlg::OnOK() 
{
	//屏蔽按回车等同按确认按钮的功能
}

void CDesignArcPartDlg::OnPartType() 
{
	UpdateData();
	CComboBox *pCmbGuiGe = (CComboBox*)GetDlgItem(IDC_CMB_PART_GUIGE);
	while(pCmbGuiGe->GetCount()>0)
		pCmbGuiGe->DeleteString(0);
	if(m_iPartType==0)		//角钢
	{
		AddJgRecord(pCmbGuiGe);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sGuiGeName=_T("Angle Specification:");
#else
		m_sGuiGeName=_T("角钢规格:");
#endif
	}
	else if(m_iPartType==1)	//槽钢
	{
		AddSlotRecord(pCmbGuiGe);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sGuiGeName=_T("Slot specification:");
#else
		m_sGuiGeName=_T("槽钢规格:");
#endif
	}
	else if(m_iPartType==2)	//扁钢
	{
		AddBianRecord(pCmbGuiGe);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sGuiGeName=_T("Flat specification:");
#else
		m_sGuiGeName=_T("扁钢规格:");
#endif
	}
	else if(m_iPartType==3)	//钢管
	{
		AddTubeRecord(pCmbGuiGe);
#ifdef AFX_TARG_ENU_ENGLISH
		m_sGuiGeName=_T("Tube specification:");
#else
		m_sGuiGeName=_T("钢管规格:");
#endif
	}
	pCmbGuiGe->SetCurSel(0);
	pCmbGuiGe->GetLBText(0,m_sPartGuiGe);
	UpdateData(FALSE);
	OnSelchangeLayoutStyle();
}

void CDesignArcPartDlg::OnChangePartNo() 
{
	UpdateData();
	SEGI iSeg;
	if(ParsePartNo(m_sPartNo.GetBuffer(),&iSeg,NULL))
		m_sSegI=iSeg.ToString();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNo);
	if(pPart)
	{	//根据找到的BOM信息，更新该钢板的基本信息(段号、规格、材质)
		if( pPart->cPartType==BOMPART::ANGLE||pPart->cPartType==BOMPART::TUBE||
			pPart->cPartType==BOMPART::FLAT||pPart->cPartType==BOMPART::SLOT)
		{
			m_iMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
			m_sPartGuiGe.Format("%.fx%.f",pPart->wide,pPart->thick);
		}
		//else
		//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	UpdateData(FALSE);
}

void CDesignArcPartDlg::OnSelchangePartGuige() 
{
	UpdateData();
	CComboBox *pCmbGuiGe = (CComboBox*)GetDlgItem(IDC_CMB_PART_GUIGE);
	int iCurSel = pCmbGuiGe->GetCurSel();
	if(iCurSel>=0)
		pCmbGuiGe->GetLBText(iCurSel,m_sPartGuiGe);
	else
		pCmbGuiGe->GetWindowText(m_sPartGuiGe);
	RetrievePreferOffsetParams(m_sPartGuiGe);
}
void CDesignArcPartDlg::RetrievePreferOffsetParams(const char* sizespec_str)
{
	double wide=0.0,thick=0,height=0.0;
	if(m_iPartType==1)	
	{	//槽钢
#ifdef AFX_TARG_ENU_ENGLISH
		m_sGuiGeName=_T("Slot Size:");
#else
		m_sGuiGeName=_T("槽钢规格:");
#endif
		SLOT_PARA_TYPE *pSlotPara=FindSlotType(m_sPartGuiGe.GetBuffer(16));
		if(pSlotPara==NULL)
			return;
		wide=pSlotPara->b;	//宽度
		height=pSlotPara->h;//高度
		if(m_iLayoutStyle==0)
		{
			m_fOffsetH=0.5*wide;	//横向位移
			m_fOffsetV=0.5*height;	//竖向位移
		}
		else if(m_iLayoutStyle==1)
		{
			m_fOffsetH=-0.5*wide;	//横向位移
			m_fOffsetV=0.5*height;	//竖向位移
		}
		else if(m_iLayoutStyle==2)
		{
			m_fOffsetH=-0.5*wide;	//横向位移
			m_fOffsetV=-0.5*height;	//竖向位移
		}
		else if(m_iLayoutStyle==3)
		{
			m_fOffsetH=0.5*wide;	//横向位移
			m_fOffsetV=-0.5*height;	//竖向位移
		}
	}
	else if(m_iPartType==0)
	{	//角钢
#ifdef AFX_TARG_ENU_ENGLISH
		m_sGuiGeName=_T("Angle Size:");
#else
		m_sGuiGeName=_T("角钢规格:");
#endif
		restore_JG_guige(m_sPartGuiGe.GetBuffer(),wide,thick);
		if(m_iLayoutStyle==0)
		{
			m_fOffsetH=0.5*wide;	//横向位移
			m_fOffsetV=-0.5*wide;	//竖向位移
		}
		else if(m_iLayoutStyle==1)
		{
			m_fOffsetH=0.5*wide;	//横向位移
			m_fOffsetV=0.5*wide;	//竖向位移
		}
		else if(m_iLayoutStyle==2)
		{
			m_fOffsetH=-0.5*wide;	//横向位移
			m_fOffsetV=0.5*wide;	//竖向位移
		}
		else if(m_iLayoutStyle==3)
		{
			m_fOffsetH=-0.5*wide;	//横向位移
			m_fOffsetV=-0.5*wide;	//竖向位移
		}
	}
	else if(m_iPartType==2)
	{	//扁铁
#ifdef AFX_TARG_ENU_ENGLISH
		m_sGuiGeName=_T("Flat Size:");
#else
		m_sGuiGeName=_T("扁钢规格:");
#endif
		restore_JG_guige(m_sPartGuiGe.GetBuffer(),wide,thick);
		m_fOffsetV=-0.5*wide;	//竖向位移
		m_fOffsetH=0;
	}
	else if(m_iPartType==3)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_sGuiGeName=_T("Tube Size:");
#else
		m_sGuiGeName=_T("钢管规格:");
#endif
	}
	UpdateData(FALSE);
}

void CDesignArcPartDlg::OnKillfocusPartGuige() 
{
	OnSelchangePartGuige();
}

void CDesignArcPartDlg::OnSelchangeLayoutStyle() 
{	//更新摆放方式示意图
	UpdateData();
	RefreshDlgCtrlState();
	OnSelchangePartGuige();
	UpdateData(FALSE);
}

//刷新控件显示状态
void CDesignArcPartDlg::RefreshDlgCtrlState()
{
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_LAYOUT_STYLE_PIC);
	m_CurPlateBmp.Detach();
	if(m_iPartType==0)
	{//角钢
		GetDlgItem(IDC_CMB_LAYOUT_STYLE)->EnableWindow(TRUE);
		//更新摆放方式示意图
		if(m_iLayoutStyle==0)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_ANGLE_LAY1);
		if(m_iLayoutStyle==1)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_ANGLE_LAY2);
		if(m_iLayoutStyle==2)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_ANGLE_LAY3);
		if(m_iLayoutStyle==3)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_ANGLE_LAY4);
	}
	else if(m_iPartType==1)
	{//槽钢
		GetDlgItem(IDC_CMB_LAYOUT_STYLE)->EnableWindow(TRUE);
		//更新摆放方式示意图
		if(m_iLayoutStyle==0)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_SLOT_LAY1);
		if(m_iLayoutStyle==1)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_SLOT_LAY2);
		if(m_iLayoutStyle==2)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_SLOT_LAY3);
		if(m_iLayoutStyle==3)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_SLOT_LAY4);
	}
	else //if(m_iPartType==2||m_iPartType==3)
	{//扁钢、钢管
		//更新摆放方式示意图
		m_CurPlateBmp.LoadBitmap(IDB_BMP_ARC_FLAT_LAY);
		GetDlgItem(IDC_CMB_LAYOUT_STYLE)->EnableWindow(FALSE);
	}
	pPic->SetBitmap(m_CurPlateBmp);
	UpdateData(FALSE);
}

void CDesignArcPartDlg::OnChangeCenterPosZ() 
{	//圆弧起始点Z坐标应该与圆弧Z坐标保持一致
	UpdateData();
	m_fStartPosZ=m_fCenterZ;
	UpdateData(FALSE);
}

void CDesignArcPartDlg::OnChangeStartPosZ() 
{	//圆弧起始点Z坐标应该与圆弧Z坐标保持一致
	UpdateData();
	m_fCenterZ=m_fStartPosZ;
	UpdateData(FALSE);
}

void CDesignArcPartDlg::OnChangeArcAngle() 
{	//确保扇形角在0°~360°之间
	UpdateData();
	UpdateData(FALSE);
}

void CDesignArcPartDlg::DisplaySubPropList()
{
	m_xPropList.CleanTree();
	m_xPropList.m_nObjClassTypeId = 0;
	m_xPropList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xPropList.SetDividerScale(0.5);
	//
	CLDSPoint *pPoint = NULL;
	CLDSVector *pVector = NULL;
	if (m_iCurDesParaType==PARA_TYPE_CENTER)
	{
		pPoint=&m_center;			//圆心
#ifdef AFX_TARG_ENU_ENGLISH
		m_sCurObjType = "position of arc center:";
#else
		m_sCurObjType = "圆弧圆心位置:";
#endif
	}
	else if (m_iCurDesParaType==PARA_TYPE_NORM)
	{
		pVector=&m_norm;			//法线
#ifdef AFX_TARG_ENU_ENGLISH
		m_sCurObjType = "Normal of work plane:";
#else
		m_sCurObjType = "工作面法线:";
#endif
	}
	else if (m_iCurDesParaType==PARA_TYPE_ARCSTART)
	{
		pPoint=&m_arcStart;			//圆弧起始点
#ifdef AFX_TARG_ENU_ENGLISH
		m_sCurObjType = "position of Arc start point:";
	}
	else 
		m_sCurObjType = "edit objects not selected currently:";
#else
		m_sCurObjType = "圆弧起始点位置:";
	}
	else 
		m_sCurObjType = "当前未选中编辑对象:";
#endif
	UpdateData(FALSE);

	//
	if (pPoint)			//点设计
	{
		m_xPropList.SetModifyValueFunc(ModifyDatumPointPropValue);
		m_xPropList.SetButtonClickFunc(DatumPointButtonClick);
		CDisplayPointProperty::DisplayPropList(&m_xPropList,pPoint);
	}
	else if(pVector)	//方向设计
	{
		m_xPropList.SetModifyValueFunc(ModifyDatumVectorPropValue);
		m_xPropList.SetButtonClickFunc(DatumVectorButtonClick);
		CDisplayVectorProperty::DisplayPropList(&m_xPropList,pVector);
	}
	else
		m_xPropList.CleanTree();
	m_xPropList.Redraw();
}

void CDesignArcPartDlg::OnEnSetFocusWorkNorm()
{
	m_iCurDesParaType = PARA_TYPE_NORM;
	DisplaySubPropList();
}

void CDesignArcPartDlg::OnEnSetFocusCenterPos()
{
	m_iCurDesParaType = PARA_TYPE_CENTER;
	DisplaySubPropList();
}

void CDesignArcPartDlg::OnEnSetFocusStartPos()
{
	m_iCurDesParaType = PARA_TYPE_ARCSTART;
	DisplaySubPropList();
}

void CDesignArcPartDlg::RefreshEditText()
{
	if(m_iCurDesParaType==PARA_TYPE_ARCSTART)
	{
		((CButton*)GetDlgItem(IDC_BN_PASTE_START_POS))->EnableWindow(m_arcStart.datum_pos_style==0);
		m_arcStart.UpdatePos();
		m_fStartPosX = m_arcStart.Position().x;
		m_fStartPosY = m_arcStart.Position().y;
		m_fStartPosZ = m_arcStart.Position().z;
	}
	else if(m_iCurDesParaType==PARA_TYPE_CENTER)
	{
		((CButton*)GetDlgItem(IDC_BN_PASTE_CENTER_POS))->EnableWindow(m_center.datum_pos_style==0);
		m_center.UpdatePos();
		m_fCenterX = m_center.Position().x;
		m_fCenterY = m_center.Position().y;
		m_fCenterZ = m_center.Position().z;
	}
	else if(m_iCurDesParaType==PARA_TYPE_NORM)
	{
		((CButton*)GetDlgItem(IDC_BN_PASTE_WORK_NORM))->EnableWindow(m_norm.norm_style==0);
		m_norm.UpdateVector(console.GetActiveModel());
		m_fWorkNormX = m_norm.vector.x;
		m_fWorkNormY = m_norm.vector.y;
		m_fWorkNormZ = m_norm.vector.z;
	}
	UpdateData(FALSE);
}

void CDesignArcPartDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	CLDSPoint *pPoint = NULL;
	CLDSVector *pVector = NULL;
	if (m_iCurDesParaType==PARA_TYPE_ARCSTART)
		pPoint=&m_arcStart;
	else if (m_iCurDesParaType==PARA_TYPE_CENTER)
		pPoint=&m_center;
	else if (m_iCurDesParaType==PARA_TYPE_NORM)
		pVector=&m_norm;
	if (m_iCurDesParaType==PARA_TYPE_ARCSTART||m_iCurDesParaType==PARA_TYPE_CENTER)
		CDisplayPointProperty::FinishSelectObjOper(pPoint,m_hPickObj,GetEventPropId());
	else if (m_iCurDesParaType==PARA_TYPE_NORM)
		CDisplayVectorProperty::FinishSelectObjOper(pVector,m_hPickObj,GetEventPropId());
}

void CDesignArcPartDlg::OnBnClickedBtnConfirm()
{
	CDialog::OnOK();
}
