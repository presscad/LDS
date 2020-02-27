//<LOCALE_TRANSLATE Confirm by hxr/>
// PartGroupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "GlobalFunc.h"
#include "PartGroupDlg.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CPartGroupDlg dialog

//TODO:更改此对话框为属性栏式，同时去掉坐标系参数，增加组焊件类型m_iPartGroupType属性 wjh-2017.9.25
CPartGroupDlg::CPartGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartGroupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartGroupDlg)
	m_sPartGroupName = _T("");
	m_fAxisX_X = 1.0;
	m_fAxisX_Y = 0.0;
	m_fAxisX_Z = 0.0;
	m_fAxisY_X = 0.0;
	m_fAxisY_Y = 1.0;
	m_fAxisY_Z = 0.0;
	m_fAxisZ_X = 0.0;
	m_fAxisZ_Y = 0.0;
	m_fAxisZ_Z = 1.0;
	m_fOriginPosX = 0.0;
	m_fOriginPosY = 0.0;
	m_fOriginPosZ = 0.0;
	m_bCanModifyAxisX = TRUE;
	m_bCanModifyAxisY = FALSE;
	m_bCanModifyAxisZ = TRUE;
	m_uStatNum = 0;
	m_sFolderName = _T("");
	m_sRootPartHandle = _T("0X0");
	m_bTowerFoot = FALSE;
	m_sPartGroupName = _T("");
	//}}AFX_DATA_INIT
	ucs.axis_x.Set(1.0,0.0,0.0);
	ucs.axis_y.Set(0.0,1.0,0.0);
	ucs.axis_z.Set(0.0,0.0,1.0);
	m_pPartGroup = NULL;
	m_hOldRootPart=0;	//记录旧的基准构件句柄
	m_bCopyAndNew=FALSE;	
}


void CPartGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartGroupDlg)
	DDX_Text(pDX, IDC_E_PARTGROUP_NAME, m_sPartGroupName);
	DDV_MaxChars(pDX, m_sPartGroupName, 49);
	DDX_Text(pDX, IDC_E_AXIS_X_X, m_fAxisX_X);
	DDX_Text(pDX, IDC_E_AXIS_X_Y, m_fAxisX_Y);
	DDX_Text(pDX, IDC_E_AXIS_X_Z, m_fAxisX_Z);
	DDX_Text(pDX, IDC_E_AXIS_Y_X, m_fAxisY_X);
	DDX_Text(pDX, IDC_E_AXIS_Y_Y, m_fAxisY_Y);
	DDX_Text(pDX, IDC_E_AXIS_Y_Z, m_fAxisY_Z);
	DDX_Text(pDX, IDC_E_AXIS_Z_X, m_fAxisZ_X);
	DDX_Text(pDX, IDC_E_AXIS_Z_Y, m_fAxisZ_Y);
	DDX_Text(pDX, IDC_E_AXIS_Z_Z, m_fAxisZ_Z);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_X, m_fOriginPosX);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_Y, m_fOriginPosY);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_Z, m_fOriginPosZ);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_X, m_bCanModifyAxisX);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_Y, m_bCanModifyAxisY);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_Z, m_bCanModifyAxisZ);
	DDX_Text(pDX, IDC_E_STAT_NUM, m_uStatNum);
	DDX_Text(pDX, IDC_E_FOLDER_NAME, m_sFolderName);
	DDV_MaxChars(pDX, m_sFolderName, 16);
	DDX_CBString(pDX, IDC_CMB_ROOT_PART_HANDLE, m_sRootPartHandle);
	DDX_Check(pDX, IDC_CHK_TOWER_FOOT, m_bTowerFoot);
	DDX_Text(pDX, IDC_S_PARTGROUP_NUM, m_sPartGroupNum);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPartGroupDlg, CDialog)
	//{{AFX_MSG_MAP(CPartGroupDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_X, OnBnPasteAxisX)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_Y, OnBnPasteAxisY)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_Z, OnBnPasteAxisZ)
	ON_BN_CLICKED(IDC_BN_PASTE_ORIGIN_POS, OnBnPasteOriginPos)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_X, OnBnCopyAxisX)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_Y, OnBnCopyAxisY)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_Z, OnBnCopyAxisZ)
	ON_BN_CLICKED(IDC_BN_COPY_ORIGIN_POS, OnBnCopyOriginPos)
	ON_BN_CLICKED(IDC_BN_CPY_UCS, OnBnCpyUcs)
	ON_BN_CLICKED(IDC_BN_PASTE_UCS, OnBnPasteUcs)
	ON_BN_CLICKED(IDC_BN_TO_RIGHT_HAND, OnBnToRightHand)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_X, OnChkEnableAxisX)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_Y, OnChkEnableAxisY)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_Z, OnChkEnableAxisZ)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_CMB_ROOT_PART_HANDLE, OnSelchangeRootPartHandle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartGroupDlg message handlers

BOOL CPartGroupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_pPartGroup)
	{
		CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_ROOT_PART_HANDLE);
		CLDSPart *pRootPart=console.FromPartHandle(m_pPartGroup->handle);
		if(pRootPart)
		{	//查找构件编号相同的构件
			m_hOldRootPart=pRootPart->handle;//记录旧的基准构件句柄，如果修改了基准构件则需要更新组焊件视图
			CString sPartNo,sHandle;
			m_sRootPartHandle.Format("0X%X",pRootPart->handle);
			pCMB->AddString(m_sRootPartHandle);
			sPartNo.Format("%s",(char*)pRootPart->GetPartNo());
			for(CLDSPart *pPart=Ta.Parts.GetFirst(pRootPart->GetClassTypeId());pPart;pPart=Ta.Parts.GetNext(pRootPart->GetClassTypeId()))
			{
				if(pPart==pRootPart)
					continue;
				if(strlen(pPart->GetPartNo())==0)
					continue;
				if(sPartNo.CompareNoCase(pPart->GetPartNo())==0)
				{
					sHandle.Format("0X%X",pPart->handle);
					pCMB->AddString(sHandle);
				}
			}
			pCMB->SelectString(-1,m_sRootPartHandle);
			GetDlgItem(IDC_CHK_TOWER_FOOT)->EnableWindow(pRootPart->GetClassTypeId()==CLS_PLATE);
			//显示组焊件构件数量
			PARTSET partSet;
			int nPartGroupNum=Ta.GetPartGroupPartSet(pRootPart->handle,partSet);
#ifdef AFX_TARG_ENU_ENGLISH
			m_sPartGroupNum.Format("There are %d part in total",nPartGroupNum);
#else
			m_sPartGroupNum.Format("共有%d个构件",nPartGroupNum);
#endif

		}
		else
			m_sRootPartHandle="0X0";
	}
	m_bCanModifyAxisX=FALSE;
	m_bCanModifyAxisY=!m_bCanModifyAxisX;
	m_bCanModifyAxisZ=TRUE;
	GetDlgItem(IDC_CHK_ENABLE_AXIS_X)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_ENABLE_AXIS_Y)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_ENABLE_AXIS_Z)->EnableWindow(FALSE);
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
	UpdateUcs(FALSE);
	return TRUE;
}

void CPartGroupDlg::OnOK() 
{
	UpdateData();
	UpdateUcs();
	long hRootPart=0;
	sscanf(m_sRootPartHandle,"%X",&hRootPart);
	for(CLDSPartGroup *pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
	{
		if(!m_bCopyAndNew)
		{	//当前操作为: 通过克隆当前组焊件生成新的组焊件是也需要与当前组焊件比较 wht 10-11-08
			if(pPartGroup==m_pPartGroup)
				continue;
		}
		if(hRootPart==pPartGroup->handle)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Existing group weld parts of the same handle，please input group weld parts handle again");
#else
			AfxMessageBox("已存在相同句柄的组合焊件，请重新输入组合焊件句柄");
#endif
			return;
		}
		if(pPartGroup->m_iPartGroupType!=CLDSPartGroup::GROUP_FOLDER)
		{
			if(m_sPartGroupName.CompareNoCase(pPartGroup->sGroupName)==0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Existing group weld parts of the same name，please input group weld parts name again");
#else
				AfxMessageBox("已存在相同名字的组合焊件，请重新输入组合焊件名称");
#endif
				return;
			}	
		}
	}
	CLDSLinePart *pDatumLinePart=NULL;
	CLDSPart *pPart=console.FromPartHandle(hRootPart);
	if(m_bTowerFoot&&pPart->GetClassTypeId()==CLS_PLATE)
		pDatumLinePart=(CLDSLinePart*)pPart->BelongModel()->FromPartHandle(((CLDSPlate*)pPart)->designInfo.m_hBasePart,CLS_LINEPART);
	if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("datum part handle is illegal (no corresponding parts)");
	else if(ucs.axis_x.IsZero()||ucs.axis_y.IsZero()||ucs.axis_z.IsZero())
		AfxMessageBox("coordinate is illegal (axis direction cannot be 0)");
	else if(!is_ucs_valid(ucs))
		AfxMessageBox("coordinate is illegal (no right hand spiral orthogonal coordinate)");
	else if(m_sPartGroupName.GetLength()>16)
		AfxMessageBox("The length of name is too long，please input again");
#else
		AfxMessageBox("基准构件句柄不合法(没有相对应的构件)");
	else if(ucs.axis_x.IsZero()||ucs.axis_y.IsZero()||ucs.axis_z.IsZero())
		AfxMessageBox("坐标系不合法(坐标轴方向不能为零)");
	else if(!is_ucs_valid(ucs))
		AfxMessageBox("坐标系不合法(不是右手螺旋正交的坐标系)");
	else if(m_sPartGroupName.GetLength()>16)
		AfxMessageBox("名字过长，请重新输入");
#endif
	else if(m_bTowerFoot&&(pDatumLinePart==NULL||(pDatumLinePart
		&&pDatumLinePart->GetClassTypeId()!=CLS_LINEANGLE
		&&pDatumLinePart->GetClassTypeId()!=CLS_GROUPLINEANGLE)))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("plate without datum rod or datum rod non-angle, cannot be set as tower foot plate group weld!");
#else
		AfxMessageBox("钢板无基准杆件或者基准杆件非角钢，不可设置为塔脚板组焊件!");
#endif
		m_bTowerFoot=FALSE;
		UpdateData();
	}
	else 
		CDialog::OnOK();
}

void CPartGroupDlg::UpdateUcs(BOOL bFlag)
{
	if(bFlag)	//交互界面向ucs更新
	{
		UpdateData(TRUE);
		ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
		ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
		ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
		ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	}
	else	//ucs向交互界面更新
	{
		m_fAxisX_X = ucs.axis_x.x;
		m_fAxisX_Y = ucs.axis_x.y;
		m_fAxisX_Z = ucs.axis_x.z;

		m_fAxisY_X = ucs.axis_y.x;
		m_fAxisY_Y = ucs.axis_y.y;
		m_fAxisY_Z = ucs.axis_y.z;

		m_fAxisZ_X = ucs.axis_z.x;
		m_fAxisZ_Y = ucs.axis_z.y;
		m_fAxisZ_Z = ucs.axis_z.z;

		m_fOriginPosX = ucs.origin.x;
		m_fOriginPosY = ucs.origin.y;
		m_fOriginPosZ = ucs.origin.z;
		UpdateData(FALSE);
	}
}

void CPartGroupDlg::OnBnPasteAxisX() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisX_X = point.x;
		m_fAxisX_Y = point.y;
		m_fAxisX_Z = point.z;
		ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
		UpdateData(FALSE);
	}
}

void CPartGroupDlg::OnBnPasteAxisY() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisY_X = point.x;
		m_fAxisY_Y = point.y;
		m_fAxisY_Z = point.z;
		ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
		UpdateData(FALSE);
	}
}

void CPartGroupDlg::OnBnPasteAxisZ() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisZ_X = point.x;
		m_fAxisZ_Y = point.y;
		m_fAxisZ_Z = point.z;
		ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
		UpdateData(FALSE);
	}
}

void CPartGroupDlg::OnBnPasteOriginPos() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fOriginPosX = point.x;
		m_fOriginPosY = point.y;
		m_fOriginPosZ = point.z;
		ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
		UpdateData(FALSE);
	}
}

void CPartGroupDlg::OnBnCopyAxisX() 
{
	f3dPoint point;
	point.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
	WritePointToClip(point);
}

void CPartGroupDlg::OnBnCopyAxisY() 
{
	f3dPoint point;
	point.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
	WritePointToClip(point);
}

void CPartGroupDlg::OnBnCopyAxisZ() 
{
	f3dPoint point;
	point.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
	WritePointToClip(point);
}

void CPartGroupDlg::OnBnCopyOriginPos() 
{
	f3dPoint point;
	point.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	WritePointToClip(point);
}

void CPartGroupDlg::OnBnCpyUcs() 
{
	UCS_STRU ucs;
	UpdateData();
	ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
	ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
	ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
	WriteUCSToClip(ucs);
	
}

void CPartGroupDlg::OnBnPasteUcs() 
{
	UCS_STRU;
	if(ReadClipUCS(ucs))
	{
		m_fAxisX_X = ucs.axis_x.x;
		m_fAxisX_Y = ucs.axis_x.y;
		m_fAxisX_Z = ucs.axis_x.z;

		m_fAxisY_X = ucs.axis_y.x;
		m_fAxisY_Y = ucs.axis_y.y;
		m_fAxisY_Z = ucs.axis_y.z;

		m_fAxisZ_X = ucs.axis_z.x;
		m_fAxisZ_Y = ucs.axis_z.y;
		m_fAxisZ_Z = ucs.axis_z.z;

		m_fOriginPosX = ucs.origin.x;
		m_fOriginPosY = ucs.origin.y;
		m_fOriginPosZ = ucs.origin.z;
		UpdateData(FALSE);
	}
}

void CPartGroupDlg::OnBnToRightHand() 
{
	UpdateUcs();
	if(!m_bCanModifyAxisZ)
	{	//以X-Y轴为基准	 X轴方向与用户输入方向保持一致，由XY计算出Z轴方向然后重新计算Y轴方向
		ucs.axis_z = ucs.axis_x^ucs.axis_y;
		normalize(ucs.axis_z);
		normalize(ucs.axis_x);
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
	}
	else if(!m_bCanModifyAxisY)
	{	//以Z-X轴为基准	 Z轴方向与用户输入方向保持一致
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_y);
		normalize(ucs.axis_x);
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
	}
	else if(!m_bCanModifyAxisX)
	{	//以Y-Z轴为基准	Y轴方向与用户输入方向保持一致
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		ucs.axis_z = ucs.axis_x^ucs.axis_y;
	}
	UpdateUcs(FALSE);
}

void CPartGroupDlg::OnChkEnableAxisX() 
{
	UpdateData();
	if(!m_bCanModifyAxisX)
		m_bCanModifyAxisY=m_bCanModifyAxisZ=TRUE;
	else
	{
		m_bCanModifyAxisY = TRUE;
		m_bCanModifyAxisZ = FALSE;
	}
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_BN_PASTE_AXIS_Z)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_X)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Y)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Z)->EnableWindow(m_bCanModifyAxisZ);
	UpdateData(FALSE);
	Invalidate(FALSE);	//重绘 wht 10-08-24
}

void CPartGroupDlg::OnChkEnableAxisY() 
{
	UpdateData();
	if(!m_bCanModifyAxisY)
		m_bCanModifyAxisX=m_bCanModifyAxisZ=TRUE;
	else
	{
		m_bCanModifyAxisX = TRUE;
		m_bCanModifyAxisZ = FALSE;
	}
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_BN_PASTE_AXIS_Z)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_X)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Y)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Z)->EnableWindow(m_bCanModifyAxisZ);
	UpdateData(FALSE);
	Invalidate(FALSE);	//重绘 wht 10-08-24
}

void CPartGroupDlg::OnChkEnableAxisZ() 
{
	UpdateData();
	if(!m_bCanModifyAxisZ)
		m_bCanModifyAxisX=m_bCanModifyAxisY=TRUE;
	else
	{
		m_bCanModifyAxisX = TRUE;
		m_bCanModifyAxisY = FALSE;
	}
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_BN_PASTE_AXIS_Z)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_X)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Y)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Z)->EnableWindow(m_bCanModifyAxisZ);
	UpdateData(FALSE);
	Invalidate(FALSE);	//重绘 wht 10-08-24
}

HBRUSH CPartGroupDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if(pWnd->GetDlgCtrlID()==IDC_S_RED)
		pDC->SetTextColor(RGB(255,0,0));
	else if(pWnd->GetDlgCtrlID()==IDC_S_BLUE)
		pDC->SetTextColor(RGB(0,130,0));
	else if(!m_bCanModifyAxisZ)
	{	//以X-Y轴为基准	 X轴方向与用户输入方向保持一致，由XY计算出Z轴方向然后重新计算Y轴方向
		if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_X)
			pDC->SetTextColor(RGB(255,0,0));	//不可变
		else if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Y)
			pDC->SetTextColor(RGB(0,130,0));	//可变
	}
	else if(!m_bCanModifyAxisY)
	{	//以Z-X轴为基准	 Z轴方向与用户输入方向保持一致
		if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Z)
			pDC->SetTextColor(RGB(255,0,0));
		else if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_X)
			pDC->SetTextColor(RGB(0,130,0));
	}
	else if(!m_bCanModifyAxisX)
	{	//以Y-Z轴为基准	Y轴方向与用户输入方向保持一致
		if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Y)
			pDC->SetTextColor(RGB(255,0,0));
		else if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Z)
			pDC->SetTextColor(RGB(0,130,0));
	}
	return hbr;
}
void CPartGroupDlg::OnSelchangeRootPartHandle() 
{
	long hPartGroup=0;
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_ROOT_PART_HANDLE);
	if(pCMB==NULL)
		return;
	pCMB->GetLBText(pCMB->GetCurSel(),m_sRootPartHandle);
	sscanf(m_sRootPartHandle,"%X",&hPartGroup);
	//显示组焊件构件数量
	PARTSET partSet;
	int nPartGroupNum=Ta.GetPartGroupPartSet(hPartGroup,partSet);
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPartGroupNum.Format("There are %d part in total",nPartGroupNum);
#else
	m_sPartGroupNum.Format("共有%d个构件",nPartGroupNum);
#endif
	UpdateData(FALSE);
}
#endif


