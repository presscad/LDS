//<LOCALE_TRANSLATE Confirm by hxr/>
// TowerFileDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "LDS.h"
#include "Console.h"
#include "TowerFileDlg.h"
#include "afxdialogex.h"

// CTowerFileDlg 对话框
#ifdef __CONNECT_REMOTE_SERVER_

IMPLEMENT_DYNAMIC(CTowerFileDlg, CDialogEx)

CTowerFileDlg::CTowerFileDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTowerFileDlg::IDD, pParent)
	, m_sFileName(_T(""))
{
	m_pTowerType=NULL;
	m_pCurFileGroup=NULL;
	m_bDisplayTowerType=FALSE;
}

CTowerFileDlg::~CTowerFileDlg()
{
}

void CTowerFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_FILE_NAME, m_sFileName);
	DDX_Control(pDX, IDC_CMB_FILEGROUP, m_cmbFileGroup);
	DDX_Control(pDX, IDC_CMB_TOWERTYPE, m_cmbTowerType);
}


BEGIN_MESSAGE_MAP(CTowerFileDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_CMB_TOWERTYPE, &CTowerFileDlg::OnCbnSelchangeCmbTowertype)
END_MESSAGE_MAP()


// CTowerFileDlg 消息处理程序
BOOL CTowerFileDlg::OnInitDialog()
{	
	CDialogEx::OnInitDialog();
	//1.显示塔型下拉框时初始化塔型下拉框
	if(m_bDisplayTowerType)
	{	
		while(m_cmbTowerType.GetCount()>0)
			m_cmbTowerType.DeleteString(0);
		CServerTowerType *pTowerType=NULL;
		for(pTowerType=AgentServer.hashTowerTypes.GetFirst();pTowerType;pTowerType=AgentServer.hashTowerTypes.GetNext())
		{
			int iItem=m_cmbTowerType.AddString(pTowerType->m_sName);
			m_cmbTowerType.SetItemData(iItem,(DWORD)pTowerType);
			if(pTowerType==m_pTowerType)
				m_cmbTowerType.SetCurSel(iItem);
		}
		if(pTowerType==NULL)
		{
			m_cmbTowerType.SetCurSel(0);
			m_pTowerType=AgentServer.hashTowerTypes.GetFirst();
		}
	}
	else 
	{	//1.隐藏塔型下拉框
		GetDlgItem(IDC_S_TOWERTYPE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CMB_TOWERTYPE)->ShowWindow(SW_HIDE);
		//2.调整窗口尺寸及按钮位置
		CRect rc;
		GetDlgItem(IDC_CMB_TOWERTYPE)->GetWindowRect(rc);
		int nHeight=rc.Height();
		GetWindowRect(rc);
		rc.bottom-=nHeight;
		MoveWindow(rc);
		int wndIdArr[6]={IDC_S_FILEGROUP,IDC_CMB_FILEGROUP,
						 IDC_S_FILE,IDC_E_FILE_NAME,IDOK,IDCANCEL};
		for(int i=0;i<6;i++)
		{
			CRect rcSon;
			CWnd *pSonWnd=GetDlgItem(wndIdArr[i]);
			pSonWnd->GetWindowRect(rcSon);
			ScreenToClient(rcSon);
			rcSon.top-=nHeight;
			rcSon.bottom-=nHeight;
			pSonWnd->MoveWindow(rcSon);
		}
	}
	//2.初始化文件组下拉框
	InitFileGroupComobox();
	return TRUE;
}

void CTowerFileDlg::Init(CServerTowerType *pTowerType,CServerTowerFileGroup *pFileGroup)
{
	m_pTowerType=pTowerType;
	m_pCurFileGroup=pFileGroup;
}

CServerTowerFileGroup *CTowerFileDlg::GetCurFileGroup()
{
	return m_pCurFileGroup;
}

CServerTowerType *CTowerFileDlg::GetTowerType()
{
	return m_pTowerType;
}

void CTowerFileDlg::OnOK()
{
	UpdateData();
	int iCurSel=m_cmbFileGroup.GetCurSel();
	if(iCurSel>=0)
		m_pCurFileGroup=(CServerTowerFileGroup*)m_cmbFileGroup.GetItemData(iCurSel);
	if(m_pCurFileGroup==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Please select subordinate file group of file!");
#else
		AfxMessageBox("请选择文件从属的文件组！");
#endif
		return;
	}
	if(m_sFileName.GetLength()<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Please input file name!");
#else
		AfxMessageBox("请输入文件名称！");
#endif
		return;
	}
	CDialogEx::OnOK();
}

void CTowerFileDlg::InitFileGroupComobox()
{
	if(m_pTowerType)
	{
		while(m_cmbFileGroup.GetCount()>0)
			m_cmbFileGroup.DeleteString(0);
		CServerTowerFileGroup *pFileGroup=NULL;
		for(pFileGroup=m_pTowerType->m_towerFileGroupList.GetFirst();pFileGroup;pFileGroup=m_pTowerType->m_towerFileGroupList.GetNext())
		{
			int iItem=m_cmbFileGroup.AddString(pFileGroup->m_sName);
			m_cmbFileGroup.SetItemData(iItem,(DWORD)pFileGroup);
			if(pFileGroup==m_pCurFileGroup)
				m_cmbFileGroup.SetCurSel(iItem);
		}
		if(m_pCurFileGroup==NULL)
			m_cmbFileGroup.SetCurSel(0);
	}
}

void CTowerFileDlg::OnCbnSelchangeCmbTowertype()
{
	UpdateData();
	int iCurSel=m_cmbTowerType.GetCurSel();
	if(iCurSel>=0)
	{
		m_pTowerType=(CServerTowerType*)m_cmbTowerType.GetItemData(iCurSel);
		InitFileGroupComobox();
	}
	UpdateData(FALSE);
}
#endif