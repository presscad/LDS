// TidToDwgDlg.cpp : 实现文件
//

#include <stdafx.h>
#include "StdArx.h"
#include "TidToDwgDlg.h"
#include "ArrayList.h"
#include "RxTools.h"

#if defined(_DEBUG)&&!defined(_DISABLE_DEBUG_NEW_)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////
//Tid文件操作
#ifdef __LDS_FILE_
ITidModel* gpTidModel;
static CFile *gpFile;
static CArchive *gpArchive;
void OpenTidFile(CString sFilePath)
{
	gpFile=new CFile(sFilePath,CFile::modeRead);
	gpArchive=new CArchive(gpFile,CArchive::load);
	//
	gpFile->SeekToEnd();
	DWORD file_len=(DWORD)gpFile->GetPosition();
	gpFile->SeekToBegin();
	DYN_ARRAY<char> pBuf(file_len);
	gpArchive->Read((char*)pBuf,file_len);
	if(gpTidModel==NULL)
		gpTidModel=CTidModelFactory::CreateTidModel();
	gpTidModel->InitTidBuffer(pBuf,file_len);
}
void CloseTidFile()
{
	if(gpArchive!= NULL)
	{
		gpArchive->Close();
		delete gpArchive;
		gpArchive = NULL;
	}
	if(	gpFile != NULL)
	{
		gpFile->Close();
		delete gpFile;
		gpFile = NULL;
	}
	if(gpTidModel!=NULL)
		CTidModelFactory::Destroy(gpTidModel->GetSerialId());
	gpTidModel=NULL;
}
//////////////////////////////////////////////////////////////////////////
// CTidToDwgDlg 对话框

IMPLEMENT_DYNAMIC(CTidToDwgDlg, CDialog)

CTidToDwgDlg::CTidToDwgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTidToDwgDlg::IDD, pParent)
{
	m_bDrawLine=FALSE;
	m_bDrawSolie=TRUE;
	m_sFilePath="";
	//
	m_listCtrl.AddColumnHeader("塔例名称");
	m_listCtrl.AddColumnHeader("本体配段");
	m_listCtrl.AddColumnHeader("腿部配段");
}

CTidToDwgDlg::~CTidToDwgDlg()
{
}

void CTidToDwgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_TID_FILE, m_sFilePath);
	DDX_Check(pDX, IDC_CHK_LINE_MODEL, m_bDrawLine);
	DDX_Check(pDX, IDC_CHK_SOLID_MODEL, m_bDrawSolie);
	DDX_Control(pDX, IDC_LIST_CTRL, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CTidToDwgDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_IMPORT_TID_FILE, OnBnClickedChkImportTidFile)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CTRL, OnDblclkListCtrl)
END_MESSAGE_MAP()


// CTidToDwgDlg 消息处理程序
BOOL CTidToDwgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//
	long col_wide_arr[3]={80,90,90};
	m_listCtrl.InitListCtrl(col_wide_arr);
	RefreshListCtrl();
	//
	//GetDlgItem(IDC_CHK_LINE_MODEL)->ShowWindow(FALSE);
	//GetDlgItem(IDC_CHK_SOLID_MODEL)->ShowWindow(FALSE);
	UpdateData(FALSE);
	return TRUE;
}

void CTidToDwgDlg::OnBnClickedChkImportTidFile()
{
	CAcModuleResourceOverride resOverride;
	//
	CFileDialog dlg(TRUE,"tid","TID",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"TID文件(*.tid)|*.tid|所有文件(*.*)|*.*||");
	if(dlg.DoModal()!=IDOK)
		return;
	m_sFilePath=dlg.GetPathName();
	CloseTidFile();
	OpenTidFile(m_sFilePath);
	//
	RefreshListCtrl();
}

void CTidToDwgDlg::RefreshListCtrl()
{
	if(gpTidModel==NULL)
		return;
	m_listCtrl.DeleteAllItems();
	CStringArray str_arr;
	str_arr.SetSize(2);
	//统计呼高信息
	for(int i=0;i<gpTidModel->HeightGroupCount();i++)
	{
		ITidHeightGroup* pModule=gpTidModel->GetHeightGroupAt(i);
		if(pModule==NULL)
			continue;
		CXhChar100 sModuleName;
		pModule->GetName(sModuleName,sModuleName.GetLengthMax());
		str_arr[0].Format("%s",(char*)sModuleName);
		//str_arr[1].Format("本体号:%d",pModule->GetSerialId());
		int iCur=m_listCtrl.InsertItemRecord(-1,str_arr);    //插入记录
		m_listCtrl.SetItemData(iCur,(DWORD)pModule);
	}
	//
	UpdateData(FALSE);
}

void CTidToDwgDlg::ConfirmCreate(bool bOnlySelecteItems)
{
	xTidModuleList.Empty();
	ITidHeightGroup* pModule=NULL;
	if(bOnlySelecteItems)
	{	//生成选中项
		POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			int iSel=m_listCtrl.GetNextSelectedItem(pos);
			pModule=(ITidHeightGroup*)m_listCtrl.GetItemData(iSel);
			xTidModuleList.append(pModule);
		}
	}
	else
	{	//生成所有项
		for(int i=0;i<m_listCtrl.GetItemCount();i++)
		{
			pModule=(ITidHeightGroup*)m_listCtrl.GetItemData(i);
			xTidModuleList.append(pModule);
		}
	}
	UpdateData();
	CDialog::OnOK();
}

void CTidToDwgDlg::OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_listCtrl.GetSelectedCount()==1)
		ConfirmCreate(true);
	*pResult = 0;
}

void CTidToDwgDlg::OnOK()
{
	if(m_listCtrl.GetSelectedCount()==0)
		ConfirmCreate(false);
	else
		ConfirmCreate(true);
}
#endif