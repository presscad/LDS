// SetNcDrvDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "SetNcDrvDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetNcDrvDlg dialog


CSetNcDrvDlg::CSetNcDrvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetNcDrvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetNcDrvDlg)
	m_sJgNcDrv = _T("");
	m_sOutPutPath = _T("");
	m_sPartNoPrefix = _T("");
	m_iNcGroupType=0;
	//}}AFX_DATA_INIT
}


void CSetNcDrvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetNcDrvDlg)
	DDX_Text(pDX, IDC_E_JG_NC_DRV, m_sJgNcDrv);
	DDX_Text(pDX, IDC_E_NC_OUTPUT_PATH, m_sOutPutPath);
	DDX_Text(pDX, IDC_E_PART_NO_PREFIX, m_sPartNoPrefix);
	DDV_MaxChars(pDX, m_sPartNoPrefix, 19);
	DDX_CBIndex(pDX, IDC_CMB_NC_GROUP_TYPE, m_iNcGroupType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetNcDrvDlg, CDialog)
	//{{AFX_MSG_MAP(CSetNcDrvDlg)
	ON_BN_CLICKED(IDC_BN_FIND_JG_NC_DRV, OnBnFindJgNcDrv)
	ON_BN_CLICKED(IDC_BN_SET_NC_OUTPUT_PATH, &CSetNcDrvDlg::OnBnSetNcOutputPath)
	ON_CBN_SELCHANGE(IDC_CMB_NC_GROUP_TYPE, &CSetNcDrvDlg::OnCbnSelchangeCmbNcGroupType)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetNcDrvDlg message handlers
BOOL CSetNcDrvDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CComboBox* pCmb=(CComboBox*)GetDlgItem(IDC_CMB_NC_GROUP_TYPE);
	pCmb->AddString("0.无");
	pCmb->AddString("1.按规格");
	pCmb->AddString("2.按材质");
	pCmb->AddString("3.按规格材质");
	pCmb->SetCurSel(m_iNcGroupType);
	UpdateData(FALSE);
	return TRUE;
}
void CSetNcDrvDlg::OnBnFindJgNcDrv() 
{
	CFileDialog dlg(TRUE,"*",NULL,NULL,
#ifdef AFX_TARG_ENU_ENGLISH
		"Angle NC Drive(*.drv)|*.drv||");
#else 
		"角钢NC驱动(*.drv)|*.drv||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		m_sJgNcDrv = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CSetNcDrvDlg::OnBnSetNcOutputPath()
{
	BROWSEINFO bi;				//BROWSEINFO结构体  
	TCHAR buffer[512]="";  
	TCHAR fullPath[512]="";  
	bi.hwndOwner=m_hWnd;		//m_hWnd你的程序主窗口  
	bi.pidlRoot=NULL;  
	bi.pszDisplayName=buffer;   //返回选择的目录名的缓冲区  
#ifdef AFX_TARG_ENU_ENGLISH
	bi.lpszTitle="select a folder";  //The tip of a pop-up window
#else
	bi.lpszTitle="选择文件夹";  //弹出的窗口的文字提示  
#endif
	bi.ulFlags=BIF_RETURNONLYFSDIRS|0x40;
	bi.lpfn = NULL;					
	bi.lParam = 0; 
	bi.iImage = 0; 
	ITEMIDLIST *pidl=::SHBrowseForFolder(&bi);      
	if(::SHGetPathFromIDList(pidl,fullPath))
	{  
		m_sOutPutPath.Format("%s",fullPath);
		UpdateData(FALSE);
	}
}
void CSetNcDrvDlg::OnCbnSelchangeCmbNcGroupType()
{
	UpdateData();
	CComboBox* pCmb=(CComboBox*)GetDlgItem(IDC_CMB_NC_GROUP_TYPE);
	m_iNcGroupType=pCmb->GetCurSel();
	UpdateData(FALSE);
}
