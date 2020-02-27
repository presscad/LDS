// LayerViewDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "LayerViewDlg.h"
#include "GlobalFunc.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayerViewDlg dialog


CLayerViewDlg::CLayerViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLayerViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLayerViewDlg)
	m_fAxisXX = 1.0;
	m_fAxisXY = 0.0;
	m_fAxisXZ = 0.0;
	m_fAxisYX = 0.0;
	m_fAxisYY = 1.0;
	m_fAxisYZ = 0.0;
	m_fAxisZX = 0.0;
	m_fAxisZY = 0.0;
	m_fAxisZZ = 1.0;
	m_sFirstCharSet = _T("TSL");
	m_fOriginPosY = 0.0;
	m_fOriginPosZ = 0.0;
	m_fOriginPosX = 0.0;
	m_sSecondCharSet = _T("ZXB");
	m_sThirdCharSet = _T("QHZY1234");
#ifdef AFX_TARG_ENU_ENGLISH
	m_sViewName = _T("New View");
#else 
	m_sViewName = _T("新建视图");
#endif
	m_sDefaultLayer = _T("");
	m_sFolderName = _T("");
	m_bCanModifyAxisX = TRUE;
	m_bCanModifyAxisY = FALSE;
	m_bCanModifyAxisZ = TRUE;
	//}}AFX_DATA_INIT
	ucs.axis_x.Set(1.0,0.0,0.0);
	ucs.axis_y.Set(0.0,1.0,0.0);
	ucs.axis_z.Set(0.0,0.0,1.0);
	m_nDlgType = 0;
	m_pCurView = NULL;
}


void CLayerViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLayerViewDlg)
	DDX_Text(pDX, IDC_E_AXIS_X_X, m_fAxisXX);
	DDX_Text(pDX, IDC_E_AXIS_X_Y, m_fAxisXY);
	DDX_Text(pDX, IDC_E_AXIS_X_Z, m_fAxisXZ);
	DDX_Text(pDX, IDC_E_AXIS_Y_X, m_fAxisYX);
	DDX_Text(pDX, IDC_E_AXIS_Y_Y, m_fAxisYY);
	DDX_Text(pDX, IDC_E_AXIS_Y_Z, m_fAxisYZ);
	DDX_Text(pDX, IDC_E_AXIS_Z_X, m_fAxisZX);
	DDX_Text(pDX, IDC_E_AXIS_Z_Y, m_fAxisZY);
	DDX_Text(pDX, IDC_E_AXIS_Z_Z, m_fAxisZZ);
	DDX_Text(pDX, IDC_E_FIRST_CHARSET, m_sFirstCharSet);
	DDV_MaxChars(pDX, m_sFirstCharSet, 15);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_Y, m_fOriginPosY);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_Z, m_fOriginPosZ);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_X, m_fOriginPosX);
	DDX_Text(pDX, IDC_E_SECOND_CHARSET, m_sSecondCharSet);
	DDX_Text(pDX, IDC_E_THIRD_CHARSET, m_sThirdCharSet);
	DDX_Text(pDX, IDC_E_VIEW_NAME, m_sViewName);
	DDV_MaxChars(pDX, m_sViewName, 49);
	DDX_Text(pDX, IDC_E_DEFAULT_LAYER, m_sDefaultLayer);
	DDV_MaxChars(pDX, m_sDefaultLayer, 3);
	DDX_Text(pDX, IDC_E_FOLDER_NAME, m_sFolderName);
	DDV_MaxChars(pDX, m_sFolderName, 16);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_X, m_bCanModifyAxisX);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_Y, m_bCanModifyAxisY);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_Z, m_bCanModifyAxisZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLayerViewDlg, CDialog)
	//{{AFX_MSG_MAP(CLayerViewDlg)
	ON_BN_CLICKED(IDC_BN_CPY_UCS, OnBnCpyUcs)
	ON_BN_CLICKED(IDC_BN_PASTE_UCS, OnBnPasteUcs)
	ON_BN_CLICKED(IDC_BN_PASTE_ORIGIN_POS, OnBnPasteOriginPos)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_Z, OnBnPasteAxisZ)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_Y, OnBnPasteAxisY)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_X, OnBnPasteAxisX)
	ON_BN_CLICKED(IDC_BN_COPY_ORIGIN_POS, OnBnCopyOriginPos)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_Z, OnBnCopyAxisZ)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_Y, OnBnCopyAxisY)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_X, OnBnCopyAxisX)
	ON_BN_CLICKED(IDC_BN_TO_RIGHT_HAND, OnBnToRightHand)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_X, OnChkEnableAxisX)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_Y, OnChkEnableAxisY)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_Z, OnChkEnableAxisZ)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayerViewDlg message handlers

BOOL CLayerViewDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_nDlgType==0)	//图层视图
		GetDlgItem(IDC_E_FOLDER_NAME)->EnableWindow(FALSE);
	else if(m_nDlgType==1)	//定制视图
	{
		GetDlgItem(IDC_E_FIRST_CHARSET)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_SECOND_CHARSET)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_THIRD_CHARSET)->EnableWindow(FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Custom View");
#else 
		SetWindowText("定制视图");
#endif
	}
	m_bCanModifyAxisX=FALSE;
	m_bCanModifyAxisY=!m_bCanModifyAxisX;
	m_bCanModifyAxisZ=TRUE;
	CDialog::OnInitDialog();
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

void CLayerViewDlg::OnOK() 
{
	UpdateData();
	for(CDisplayView *pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
	{
		if(pView==m_pCurView)
			continue;
		if(pView->m_iViewType==0||pView->m_iViewType==1)
		{
			if(m_sViewName.CompareNoCase(pView->name)==0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The same name view have already exist, please input view name again");
#else 
				AfxMessageBox("已存在相同名字的视图，请重新输入视图名");
#endif
				return;
			}
		}
	}
	UpdateUcs();
	if(ucs.axis_x.IsZero()||ucs.axis_y.IsZero()||ucs.axis_z.IsZero())
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("CSYS is not legal(Axis direction can't be zero)");
	else if(!is_ucs_valid(ucs))
		AfxMessageBox("CSYS is not legal(Not right hand spiral orthogonal coordinate)");
#else 
		AfxMessageBox("坐标系不合法(坐标轴方向不能为零)");
	else if(!is_ucs_valid(ucs))
		AfxMessageBox("坐标系不合法(不是右手螺旋正交的坐标系)");
#endif
	else
		CDialog::OnOK();
}

//COPY UCS
void CLayerViewDlg::OnBnCpyUcs() 
{
	UpdateData();
	UCS_STRU ucs;
	ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	ucs.axis_z.Set(m_fAxisZX,m_fAxisZY,m_fAxisZZ);
	ucs.axis_y.Set(m_fAxisYX,m_fAxisYY,m_fAxisYZ);
	ucs.axis_x.Set(m_fAxisXX,m_fAxisXY,m_fAxisXZ);
	WriteUCSToClip(ucs);
}

//PASTE UCS
void CLayerViewDlg::OnBnPasteUcs() 
{
	UpdateData();
	UCS_STRU ucs;
	if(ReadClipUCS(ucs))
	{
		m_fOriginPosX = ucs.origin.x;
		m_fOriginPosY = ucs.origin.y;
		m_fOriginPosZ = ucs.origin.z;
		m_fAxisXX = ucs.axis_x.x;
		m_fAxisXY = ucs.axis_x.y;
		m_fAxisXZ = ucs.axis_x.z;
		m_fAxisYX = ucs.axis_y.x;
		m_fAxisYY = ucs.axis_y.y;
		m_fAxisYZ = ucs.axis_y.z;
		m_fAxisZX = ucs.axis_z.x;
		m_fAxisZY = ucs.axis_z.y;
		m_fAxisZZ = ucs.axis_z.z;
	}
	UpdateData(FALSE);
}

void CLayerViewDlg::OnBnPasteOriginPos() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fOriginPosX = point.x;
		m_fOriginPosY = point.y;
		m_fOriginPosZ = point.z;
		UpdateData(FALSE);
	}
}

void CLayerViewDlg::OnBnPasteAxisZ() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisZX = point.x;
		m_fAxisZY = point.y;
		m_fAxisZZ = point.z;
		UpdateData(FALSE);
	}
}

void CLayerViewDlg::OnBnPasteAxisY() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisYX = point.x;
		m_fAxisYY = point.y;
		m_fAxisYZ = point.z;
		UpdateData(FALSE);
	}
}

void CLayerViewDlg::OnBnPasteAxisX() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisXX = point.x;
		m_fAxisXY = point.y;
		m_fAxisXZ = point.z;
		UpdateData(FALSE);
	}
}

void CLayerViewDlg::OnBnCopyOriginPos() 
{
	UpdateData(TRUE);
	f3dPoint point(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	WritePointToClip(point);
}

void CLayerViewDlg::OnBnCopyAxisZ() 
{
	UpdateData(TRUE);
	f3dPoint point(m_fAxisZX,m_fAxisZY,m_fAxisZZ);
	WritePointToClip(point);
}

void CLayerViewDlg::OnBnCopyAxisY() 
{
	UpdateData(TRUE);
	f3dPoint point(m_fAxisYX,m_fAxisYY,m_fAxisYZ);
	WritePointToClip(point);
}

void CLayerViewDlg::OnBnCopyAxisX() 
{
	UpdateData(TRUE);
	f3dPoint point(m_fAxisXX,m_fAxisXY,m_fAxisXZ);
	WritePointToClip(point);
}

void CLayerViewDlg::UpdateUcs(BOOL bFlag)
{
	if(bFlag)	//交互界面向ucs更新
	{
		UpdateData(TRUE);
		ucs.axis_x.Set(m_fAxisXX,m_fAxisXY,m_fAxisXZ);
		ucs.axis_y.Set(m_fAxisYX,m_fAxisYY,m_fAxisYZ);
		ucs.axis_z.Set(m_fAxisZX,m_fAxisZY,m_fAxisZZ);
		ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	}
	else	//ucs向交互界面更新
	{
		m_fAxisXX = ucs.axis_x.x;
		m_fAxisXY = ucs.axis_x.y;
		m_fAxisXZ = ucs.axis_x.z;

		m_fAxisYX = ucs.axis_y.x;
		m_fAxisYY = ucs.axis_y.y;
		m_fAxisYZ = ucs.axis_y.z;

		m_fAxisZX = ucs.axis_z.x;
		m_fAxisZY = ucs.axis_z.y;
		m_fAxisZZ = ucs.axis_z.z;

		m_fOriginPosX = ucs.origin.x;
		m_fOriginPosY = ucs.origin.y;
		m_fOriginPosZ = ucs.origin.z;
		UpdateData(FALSE);
	}
}

void CLayerViewDlg::OnBnToRightHand() 
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

void CLayerViewDlg::OnChkEnableAxisX() 
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

void CLayerViewDlg::OnChkEnableAxisY() 
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

void CLayerViewDlg::OnChkEnableAxisZ() 
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

HBRUSH CLayerViewDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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
