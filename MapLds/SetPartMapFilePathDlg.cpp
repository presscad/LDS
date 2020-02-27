//<LOCALE_TRANSLATE/>
// SetPartMapFilePathDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SetPartMapFilePathDlg.h"
#include "SysPara.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetPartMapFilePathDlg dialog


CSetPartMapFilePathDlg::CSetPartMapFilePathDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetPartMapFilePathDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetPartMapFilePathDlg)
	m_fDrawRectLen = 535.0;
	m_fDrawRectWidth = 260.0;
	m_sPartDrawingPath = _T("D:\\");
	m_bUseProcessCard = FALSE;
	m_bDimPartInfo = FALSE;
	//}}AFX_DATA_INIT
}


void CSetPartMapFilePathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetPartMapFilePathDlg)
	DDX_Text(pDX, IDC_E_DRAW_RECT_LEN, m_fDrawRectLen);
	DDX_Text(pDX, IDC_E_DRAW_RECT_WIDTH, m_fDrawRectWidth);
	DDX_Text(pDX, IDC_E_PART_DRAWING_PATH, m_sPartDrawingPath);
	DDX_Check(pDX, IDC_CHK_USE_PROCESS_CARD, m_bUseProcessCard);
	DDX_Check(pDX, IDC_CHK_DIM_PART_INFO, m_bDimPartInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetPartMapFilePathDlg, CDialog)
	//{{AFX_MSG_MAP(CSetPartMapFilePathDlg)
	ON_BN_CLICKED(IDC_BNT_SET_PATH, OnBntSetPath)
	ON_BN_CLICKED(IDC_CHK_USE_PROCESS_CARD, OnChkUseProcessCard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetPartMapFilePathDlg message handlers

void CSetPartMapFilePathDlg::OnBntSetPath() 
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
	bi.ulFlags=BIF_RETURNONLYFSDIRS;//只返回目录。其他标志看MSDN  
	bi.lpfn = NULL;             //回调函数，有时很有用  
	bi.lParam = 0;  
	bi.iImage = 0;  
	ITEMIDLIST *pidl=::SHBrowseForFolder(&bi);    
	//显示弹出窗口，ITEMIDLIST很重要  
	if(::SHGetPathFromIDList(pidl,fullPath))//在ITEMIDLIST中得到目录名的整个路径  
	{  //成功
		m_sPartDrawingPath.Format("%s",fullPath);
		UpdateData(FALSE);
	}
}

BOOL CSetPartMapFilePathDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_bUseProcessCard=sys.general_set.bProcessCardTbl;
	GetDlgItem(IDC_E_DRAW_RECT_LEN)->EnableWindow(!m_bUseProcessCard);
	GetDlgItem(IDC_E_DRAW_RECT_WIDTH)->EnableWindow(!m_bUseProcessCard);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetPartMapFilePathDlg::OnChkUseProcessCard() 
{
	UpdateData();
	sys.general_set.bProcessCardTbl=m_bUseProcessCard;
	GetDlgItem(IDC_E_DRAW_RECT_LEN)->EnableWindow(!m_bUseProcessCard);
	GetDlgItem(IDC_E_DRAW_RECT_WIDTH)->EnableWindow(!m_bUseProcessCard);
	UpdateData(FALSE);
}
