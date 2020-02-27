// FolderNameDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "FolderNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFolderNameDlg dialog


CFolderNameDlg::CFolderNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFolderNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFolderNameDlg)
	m_sFolderName = _T("");
	//}}AFX_DATA_INIT
	m_nNameMaxLen = 100000000;	//默认一个足够大的值
	m_nDlgType = 1;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	m_pCurPartGroup = NULL;
#endif
	m_pCurView = NULL;
}


void CFolderNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFolderNameDlg)
	DDX_Text(pDX, IDC_E_FOLDER_NAME, m_sFolderName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFolderNameDlg, CDialog)
	//{{AFX_MSG_MAP(CFolderNameDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFolderNameDlg message handlers

void CFolderNameDlg::OnOK() 
{
	UpdateData();
	if(m_nDlgType==1)
	{	//视图组
		for(CDisplayView *pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
		{
			if(pView==m_pCurView)
				continue;
			if(pView->m_iViewType==2)
			{	
				if(m_sFolderName.CompareNoCase(pView->folderName)==0)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Existing same name of custom view group. Please enter a new group name");
#else 
					AfxMessageBox("已存在相同名字的定制视图组，请重新输入组名");
#endif
					return;
				}	
			}	
		}
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(m_nDlgType==2)
	{	//组合构件组
		for(CLDSPartGroup *pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			if(pPartGroup==m_pCurPartGroup)
				continue;
			if(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_FOLDER)
			{
				if(m_sFolderName.CompareNoCase(pPartGroup->sFolderName)==0)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Existing same name of group. Please enter a new group name");
#else 
					AfxMessageBox("已存在相同名字的组，请重新输入组名");
#endif
					return;
				}
			}
		}
	}
#endif
	if(m_sFolderName.GetLength()>m_nNameMaxLen)
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The length of string is too long，please input again");
#else 
		AfxMessageBox("字符串过长，请重新输入");
#endif
	else
		CDialog::OnOK();
}
