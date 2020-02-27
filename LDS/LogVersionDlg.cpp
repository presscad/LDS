// LogVersionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "LogVersionDlg.h"
#include ".\logversiondlg.h"

// CLogVersionDlg 对话框

IMPLEMENT_DYNAMIC(CLogVersionDlg, CDialog)
CLogVersionDlg::CLogVersionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogVersionDlg::IDD, pParent)
{
	m_pLogVersion=NULL;
	m_pCurContents=NULL;
}

CLogVersionDlg::~CLogVersionDlg()
{
}

void CLogVersionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG_CONTENT, m_xLogContentList);
	DDX_Control(pDX, IDC_NAVIGATION_BAR, m_xNavigationBar);
}


BEGIN_MESSAGE_MAP(CLogVersionDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CLogVersionDlg 消息处理程序

void CLogVersionDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(m_xNavigationBar.GetSafeHwnd())
	{
		m_xNavigationBar.MoveWindow(45,0,cx,25);
		m_xNavigationBar.SetWidth(cx-45);
		m_xNavigationBar.SetLeftTop(45,0);
		m_xNavigationBar.SetHeight(25);
	}
	if(m_xLogContentList.GetSafeHwnd())
		m_xLogContentList.MoveWindow(0,25,cx,cy);
}


static BOOL FireButtonClick(CLinkButton *pBtn)
{
	AfxMessageBox("跳转至当前选中目录！");
	return TRUE;
}
BOOL CLogVersionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	ShowWindow(SW_SHOWMAXIMIZED);
	//
	m_xNavigationBar.SetButtonHandleCursor(AfxGetApp()->LoadCursor(IDC_HANDCUR));
	m_xNavigationBar.SetFireButtonClick(FireButtonClick);
	InitSuperGridCtrl();
	if(m_pLogVersion==NULL)
		return FALSE;
	else 
		m_pCurContents=&m_pLogVersion->contents;
	InitNavigationBar();
	RefreshSuperGridCtrl();
	return TRUE;  
}


void CLogVersionDlg::InitSuperGridCtrl()
{
	m_xLogContentList.AddColumnHeader("名称");
	m_xLogContentList.AddColumnHeader("新值");
	m_xLogContentList.AddColumnHeader("旧值");
	m_xLogContentList.AddColumnHeader("操作类型");
	long col_wide_arr[4]={150,150,150,150};
	m_xLogContentList.InitListCtrl(col_wide_arr);
}

void CLogVersionDlg::InitNavigationBar()
{
	m_xNavigationBar.Empty();
	if(m_pCurContents==NULL)
		return;
	m_xNavigationBar.AddLinkButton(m_pCurContents->Schema()->Name(),(DWORD)m_pCurContents);
}

static void InsertLogContentItem(CSuperGridCtrl *pListCtrl,CLogContents *pContent)
{
	if(pListCtrl==NULL||pContent==NULL)
		return;

	CStringArray str_arr;
	str_arr.SetSize(4);
	str_arr[0]=pContent->Schema()->Name();
	if(pContent->m_cType=='M')
		str_arr[1]="修改";
	else if(pContent->m_cType=='A')
		str_arr[1]="添加";
	else if(pContent->m_cType=='D')
		str_arr[1]="删除";
	else 
		str_arr[1]="未知";
	pContent->DataBufferToUnionData();
	if(pContent->m_wDataType==CLogContents::DT_I1)
	{
		str_arr[2].Format("%c",pContent->log_data.DATA_I1.m_cOld);
		str_arr[3].Format("%c",pContent->log_data.DATA_I1.m_cNew);
	}
	else if(pContent->m_wDataType==CLogContents::DT_I2)
	{
		str_arr[2].Format("%d",pContent->log_data.DATA_I2.m_wOld);
		str_arr[3].Format("%d",pContent->log_data.DATA_I2.m_wNew);
	}
	else if(pContent->m_wDataType==CLogContents::DT_I4||pContent->m_wDataType==CLogContents::DT_HANDLE)
	{
		str_arr[2].Format("%d",pContent->log_data.DATA_I4.m_iOld);
		str_arr[3].Format("%d",pContent->log_data.DATA_I4.m_iNew);
	}
	else if(pContent->m_wDataType==CLogContents::DT_R8)
	{
		str_arr[2].Format("%f",pContent->log_data.DATA_R8.m_fOld);
		str_arr[3].Format("%f",pContent->log_data.DATA_R8.m_fNew);
	}
	else if(pContent->m_wDataType==CLogContents::DT_STRING)
	{
		str_arr[2].Format("%s",pContent->log_data.DATA_STRING.m_sOld);
		str_arr[3].Format("%s",pContent->log_data.DATA_STRING.m_sNew);
	}
	else if(pContent->m_wDataType==CLogContents::DT_POINT)
	{
		str_arr[2].Format("(%f,%f,%f)",pContent->log_data.DATA_POINT.oldPtArr[0],
			pContent->log_data.DATA_POINT.oldPtArr[1],pContent->log_data.DATA_POINT.oldPtArr[2]);
		str_arr[3].Format("(%f,%f,%f)",pContent->log_data.DATA_POINT.newPtArr[0],
			pContent->log_data.DATA_POINT.newPtArr[1],pContent->log_data.DATA_POINT.newPtArr[2]);
	}
	else if(pContent->m_wDataType==CLogContents::DT_CFGWORD)
	{
		char sCfgWrod[100]="";
		CFGWORD cfgWord;
		memcpy(cfgWord.word,pContent->log_data.DATA_CFGWORD.oldCfgWord,sizeof(DWORD)*4);
		MakeWordToString(cfgWord,sCfgWrod);
		str_arr[2]=sCfgWrod;
		memcpy(cfgWord.word,pContent->log_data.DATA_CFGWORD.newCfgWord,sizeof(DWORD)*4);
		MakeWordToString(cfgWord,sCfgWrod);
		str_arr[3]=sCfgWrod;
	}
	CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
	lpInfo->AddSubItemText(str_arr[0],TRUE);
	lpInfo->AddSubItemText(str_arr[1],TRUE);
	lpInfo->AddSubItemText(str_arr[2],TRUE);
	lpInfo->AddSubItemText(str_arr[3],TRUE);
	CSuperGridCtrl::CTreeItem *pItem=pListCtrl->InsertRootItem(lpInfo);
	pItem->m_idProp=(long)pContent;
}

void CLogVersionDlg::RefreshSuperGridCtrl()
{
	if(m_pLogVersion==NULL||m_pCurContents==NULL)
		return;
	CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
	lpInfo->AddSubItemText("..上一级目录",TRUE);
	CSuperGridCtrl::CTreeItem *pItem=m_xLogContentList.InsertRootItem(lpInfo);
	pItem->m_idProp=0;
	if(m_pCurContents->GetMemberCount()==0)
		InsertLogContentItem(&m_xLogContentList,m_pCurContents);
	else 
	{
		for(CLogContents *pSonContent=m_pCurContents->GetFirstMember();pSonContent;pSonContent=m_pCurContents->GetNextMember())
			InsertLogContentItem(&m_xLogContentList,pSonContent);
	}
}
