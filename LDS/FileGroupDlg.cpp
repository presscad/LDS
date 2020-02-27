//<LOCALE_TRANSLATE Confirm by hxr/>
// FileGroupDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "FileGroupDlg.h"
#include "GlobalFunc.h"
#include "afxdialogex.h"
#include "SortFunc.h"

// CFileGroupDlg 对话框
IMPLEMENT_DYNAMIC(CFileGroupDlg, CDialogEx)

CFileGroupDlg::CFileGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFileGroupDlg::IDD, pParent)
	, m_sSegStr(_T(""))
	, m_sName(_T(""))
	, m_sTowerTypeName(_T("新塔型"))
{
	m_pFileGroup=NULL;
	m_pCurFile=NULL;
	m_ciStartMode=0;
	m_sOriginalFileName = _T("");
	m_sCurFileNotes = _T("");
	m_ciObjectType=0;
}

CFileGroupDlg::~CFileGroupDlg()
{
}

void CFileGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileGroupDlg)
	DDX_Text(pDX, IDC_E_SEGSTR, m_sSegStr);
	DDX_Text(pDX, IDC_E_NAME, m_sName);
	DDX_Text(pDX, IDC_E_TA_TYPE, m_sTowerTypeName);
	DDX_Control(pDX, IDC_CMB_CUR_FILE, m_cmbCurFile);
	DDX_Text(pDX, IDC_E_ORIGINAL_NAME, m_sOriginalFileName);
	DDX_Text(pDX, IDC_E_NOTES, m_sCurFileNotes);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFileGroupDlg, CDialogEx)
	//{{AFX_MSG_MAP(CFileGroupDlg)
	ON_CBN_SELCHANGE(IDC_CMB_CUR_FILE, &CFileGroupDlg::OnSelchangeCmbCurFile)
	ON_EN_CHANGE(IDC_E_SEGSTR, &CFileGroupDlg::OnChangeSegStr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
typedef CServerTowerFile* CServerTowerFilePtr;
static int _LocalCompareFiles(const CServerTowerFilePtr& pFile1,const CServerTowerFilePtr& pFile2)
{
	if(pFile1->dwVersion<pFile2->dwVersion)
		return 1;
	else if(pFile1->dwVersion>pFile2->dwVersion)
		return -1;
	return 0;
}
// CFileGroupDlg 消息处理程序
BOOL CFileGroupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	GetDlgItem(IDC_E_TA_TYPE)->EnableWindow(m_ciObjectType==EDIT_TOWERTYPE);
	if(m_ciObjectType==1)
		GetDlgItem(IDC_S_PRJ_NAME)->SetWindowText("工程名称:");
	else
		GetDlgItem(IDC_S_PRJ_NAME)->SetWindowText("段号范围:");
	((CEdit*)GetDlgItem(IDC_E_TA_TYPE))->SetReadOnly(m_ciObjectType==2);
	((CEdit*)GetDlgItem(IDC_E_SEGSTR))->SetReadOnly(m_ciObjectType==2);
	((CEdit*)GetDlgItem(IDC_E_NAME))->SetReadOnly(m_ciObjectType==2);
	//if(m_ciObjectType==2)
	if(m_ciStartMode==1)
	{
		CWnd* pBtnOK=GetDlgItem(IDOK);
		if(pBtnOK)
			pBtnOK->SetWindowText("关联当前模型");
	}
	if(m_pFileGroup==NULL)
	{	//新建文件组
		if(m_ciStartMode==1)
		{
			CXhChar200 filename=theApp.GetLDSDoc()->GetTitle();
			char* separator=SearchChar(filename,'.',true);
			if(separator==NULL)
				filename.Append(".tma");
			else
				strcpy(separator,".tma");
			m_sName=filename;
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			m_sName="New 3d-model file";
#else
			m_sName=m_sTowerTypeName;
#endif
		}
		//初始化文件列表
		m_cmbCurFile.Clear();
		int iItem=m_cmbCurFile.AddString("创建新版本");
		m_cmbCurFile.SetItemData(iItem,0);
		m_cmbCurFile.SetCurSel(iItem);
	}
	else 
	{	//编辑文件组属性
		CXhChar200 filename=theApp.GetLDSDoc()->GetTitle();
		char* separator=SearchChar(filename,'.',true);
		if(separator==NULL)
			filename.Append(".tma");
		else
			strcpy(separator,".tma");
		m_sOriginalFileName=filename;
		if(m_ciStartMode==1)
		{
			theApp.GetLDSDoc()->m_idServerFile=Ta.m_idServerFile=m_pFileGroup->m_idActiveFile;
			Ta.m_wiVerSerial=(WORD)m_pFileGroup->m_uiActFileVerSerial;
		}
		else
			m_sName=m_pFileGroup->m_sName;
		m_sSegStr=m_pFileGroup->m_sSegStr;
		//初始化文件列表
		m_cmbCurFile.Clear();
		int i=0,iItem=m_cmbCurFile.AddString("创建新版本");
		m_cmbCurFile.SetItemData(iItem,0);
		m_cmbCurFile.SetCurSel(iItem);
		DYN_ARRAY<CServerTowerFile*> arrFiles(m_pFileGroup->hashTowerFiles.GetNodeNum());
		for(CServerTowerFile *pFile=m_pFileGroup->hashTowerFiles.GetFirst();pFile;pFile=m_pFileGroup->hashTowerFiles.GetNext(),i++)
			arrFiles[i]=pFile;
		if(arrFiles.Size()>0)
			CQuickSort<CServerTowerFile*>::QuickSort(arrFiles,arrFiles.Size(),_LocalCompareFiles);
		for(UINT i=0;i<arrFiles.Size();i++)
		{
			CServerTowerFile *pFile=arrFiles[i];
			iItem=m_cmbCurFile.AddString(CXhChar200("%s#V%d",(char*)pFile->m_sFileName,pFile->dwVersion));
			m_cmbCurFile.SetItemData(iItem,(DWORD_PTR)pFile);
			if(pFile==m_pFileGroup->pActiveFile)
				m_cmbCurFile.SetCurSel(iItem);
		}
		if(m_ciObjectType==2)
		{
			m_cmbCurFile.SetCurSel(0);
			GetDlgItem(IDC_E_ORIGINAL_NAME)->SetFocus();
		}
	}
	UpdateData(FALSE);
	return TRUE;
}

CServerTowerFile *CFileGroupDlg::GetFileGroupCurFile()
{
	return m_pCurFile;
}
void CFileGroupDlg::OnChangeSegStr()
{
	UpdateData();
	if(m_ciObjectType==0)
	{
		GetDlgItem(IDC_E_TA_TYPE)->GetWindowText(m_sName);
		m_sName.AppendChar('(');
		m_sName.Append(m_sSegStr);
		m_sName.AppendChar(')');
		GetDlgItem(IDC_E_NAME)->SetWindowText(m_sName);
	}
}

void CFileGroupDlg::OnSelchangeCmbCurFile()
{
	int iCurSel=m_cmbCurFile.GetCurSel();
	if(iCurSel>=0)
		m_pCurFile=(CServerTowerFile*)m_cmbCurFile.GetItemData(iCurSel);
	else 
		m_pCurFile=NULL;
	if(m_pCurFile)
	{
		m_sOriginalFileName=m_pCurFile->m_sFileName;
		m_sCurFileNotes=m_pCurFile->m_sDescription;
	}
	else
	{
		CXhChar200 filename=theApp.GetLDSDoc()->GetTitle();
		char* separator=SearchChar(filename,'.',true);
		if(separator==NULL)
			filename.Append(".tma");
		else
			strcpy(separator,".tma");
		m_sOriginalFileName=filename;
		m_sCurFileNotes="";
	}
	UpdateData(FALSE);
}

void CFileGroupDlg::OnOK()
{
	UpdateData();
	int iCurSel=m_cmbCurFile.GetCurSel();
	if(iCurSel>=0)
		m_pCurFile=(CServerTowerFile*)m_cmbCurFile.GetItemData(iCurSel);
	else 
		m_pCurFile=NULL;
	CDialogEx::OnOK();
}
