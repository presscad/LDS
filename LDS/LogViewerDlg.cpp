// LogViewerDlg.cpp : 实现文件
//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "LogViewerDlg.h"
#include "env_def.h"
#include ".\Log\LogVersionMsgDef.h"

#ifdef __PART_DESIGN_INC_
#include ".\Log\XhLog.h"
// CLogViewerDlg 对话框

IMPLEMENT_DYNCREATE(CLogViewerDlg, CDialog)
CLogViewerDlg::CLogViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogViewerDlg::IDD, pParent)
{
#ifdef AFX_TARG_ENU_ENGLISH
	m_xLogVersionList.AddColumnHeader("Version");
	m_xLogVersionList.AddColumnHeader("User");
	m_xLogVersionList.AddColumnHeader("Date");
#else 
	m_xLogVersionList.AddColumnHeader("版本");
	m_xLogVersionList.AddColumnHeader("用户");
	m_xLogVersionList.AddColumnHeader("日期");
#endif
}

CLogViewerDlg::~CLogViewerDlg()
{
}

void CLogViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG_VERSION, m_xLogVersionList);
}

BEGIN_MESSAGE_MAP(CLogViewerDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_VIEW, OnBnClickedBtnView)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LOG_VERSION, OnNMDblclkListLogVersion)
	ON_BN_CLICKED(IDC_BTN_EMPTY_LOG, OnBnClickedBtnEmptyLog)
	ON_COMMAND(ID_VIEW_LOG_VERSION,OnViewLogVersion)
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CLogViewerDlg::OnOK()
{

}

void CLogViewerDlg::OnCancel()
{

}
void CLogViewerDlg::RefreshLogVersionList()
{
	CStringArray str_arr;
	str_arr.SetSize(3);
	m_xLogVersionList.DeleteAllItems();
	for(CXhLogVersion *pVersion=logger.GetTailLogVersion();pVersion;pVersion=logger.GetPrevLogVersion())
	{
		str_arr[0].Format("%d",pVersion->GetSerialNo());
		str_arr[1]=pVersion->username;
		CTime time(pVersion->datetime);
		str_arr[2]=time.Format("%y-%m-%d %H:%M");
		int iItem=m_xLogVersionList.InsertItemRecord(-1,str_arr);
		m_xLogVersionList.SetItemData(iItem,(DWORD)pVersion);
	}
}

// CLogViewerDlg 消息处理程序
BOOL CLogViewerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	long col_wide_arr[3]={60,70,140};
	m_xLogVersionList.InitListCtrl(col_wide_arr);
	//
	RefreshLogVersionList();
	return TRUE;
}

static BOOL CreateLogVersionProcess( HANDLE hClientPipeRead, HANDLE hClientPipeWrite )
{
	TCHAR cmd_str[MAX_PATH]="D:\\CimsForTa\\LDS\\LogVersion\\Debug\\";
	GetSysPath(cmd_str);
	strcat(cmd_str,"LogVersion.exe -child");

	STARTUPINFO startInfo;
	memset( &startInfo, 0 , sizeof( STARTUPINFO));
	startInfo.cb= sizeof( STARTUPINFO );
	startInfo.dwFlags |= STARTF_USESTDHANDLES;
	startInfo.hStdError= 0;//hPipeWrite;
	startInfo.hStdInput= hClientPipeRead;
	startInfo.hStdOutput= hClientPipeWrite;

	PROCESS_INFORMATION processInfo;
	memset( &processInfo, 0, sizeof(PROCESS_INFORMATION) );

	BOOL b=CreateProcess( NULL,cmd_str,
		NULL,NULL, TRUE,CREATE_NEW_CONSOLE, NULL, NULL,&startInfo,&processInfo );
	DWORD er=GetLastError();
	/*if( b )
	{
		CloseHandle( processInfo.hProcess );
		CloseHandle( processInfo.hThread );
	}*/
	return b;
}

static BOOL WriteLogVersionToClient(HANDLE hPipeWrite,CXhLogVersion *pLogVersion,char *sLogVersionScope)
{
	if(pLogVersion==NULL)
		return FALSE;
	if( hPipeWrite == INVALID_HANDLE_VALUE )
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The abtained tube's handle is invalid\n");
#else 
		AfxMessageBox("获取管道句柄无效\n");
#endif
		return FALSE;
	}
	CBuffer buffer(1024);
	ASSERT(pLogVersion->m_pSchema);
	if(sLogVersionScope)
		buffer.WriteString(sLogVersionScope);
	else 
		buffer.WriteString("");
	pLogVersion->m_pSchema->ToBuffer(buffer);
	pLogVersion->ToBuffer(buffer);
	//从匿名管道中读取数据，显示，并返回给服务器
	return buffer.WriteToPipe(hPipeWrite,1024);
}

UINT ListenLogVersionProc(LPVOID pParam)
{
	HANDLE hPipeSrcRead=*((HANDLE*)pParam);
	LOGVERSION_MSGHEAD msg_head;
	DWORD dwBytes;
	while(ReadFile(hPipeSrcRead,&msg_head,9,&dwBytes,NULL))
	{
		if(msg_head.command_id==SELECT_OBJ_MSG)
		{
			WORD wDataType=0;
			long handle=0;
			ReadFile(hPipeSrcRead,&wDataType,2,&dwBytes,NULL);
			ReadFile(hPipeSrcRead,&handle,4,&dwBytes,NULL);
			SCOPE_STRU scope;
			if(wDataType==CLogContents::DT_LINEPART)
			{
				CSuperSmartPtr<CLDSPart> pLinePart=console.FromPartHandle(handle);
				if(pLinePart.IsHasPtr()&&pLinePart->IsLinePart())
				{
					scope.VerifyVertex(pLinePart.LinePartPointer()->End());
					scope.VerifyVertex(pLinePart.LinePartPointer()->Start());
				}
			}
			else if(wDataType==CLogContents::DT_NODE)
			{
				CLDSNode* pNode=console.FromNodeHandle(handle);
				if(pNode)
					scope.VerifyVertex(pNode->Position(false));
			}
			else
				continue;
			g_pSolidOper->FocusTo(scope,g_sysPara.m_fMaxScaleScr2User,handle);
			//g_pSolidDraw->ReleaseSnapStatus();
			//g_pSolidDraw->SetEntSnapStatus(handle);	//由于多线程冲突，暂不能这样写这部分代码 wjh-2013.4.10
			//g_pSolidDraw->Draw();
		}
	}
	return 0;
}

static void DisplayLogVersion(CXhLogVersion *pLogVersion,char *sLogVersionScope)
{
	//step1:创建第一个管道: 用于服务器端向客户端发送内容
	SECURITY_ATTRIBUTES attrib;
	attrib.nLength = sizeof( SECURITY_ATTRIBUTES );
	attrib.bInheritHandle= true;
	attrib.lpSecurityDescriptor = NULL;
	HANDLE hPipeClientRead=NULL, hPipeSrcWrite=NULL;
	if( !CreatePipe( &hPipeClientRead, &hPipeSrcWrite, &attrib, 0 ) )
		return;
	//因为此时两个句柄均可继承，故可复制一个不可继承的写句柄并将原来的写句柄关掉
	HANDLE hPipeSrcWriteDup=NULL;
	if( !DuplicateHandle( GetCurrentProcess(), hPipeSrcWrite, GetCurrentProcess(), &hPipeSrcWriteDup, 0, false, DUPLICATE_SAME_ACCESS ) )
		return;
	CloseHandle( hPipeSrcWrite );
	//step2:创建第二个管道，用于客户端向服务器端发送内容
	HANDLE hPipeClientWrite=NULL, hPipeSrcRead=NULL;
	if( !CreatePipe( &hPipeSrcRead, &hPipeClientWrite, &attrib, 0) )
		return;
	//复制不可继承的读句柄，并把原有的读句柄关掉
	HANDLE hPipeSrcReadDup=NULL;
	if( !DuplicateHandle( GetCurrentProcess(), hPipeSrcRead, GetCurrentProcess(), &hPipeSrcReadDup, 0, false, DUPLICATE_SAME_ACCESS ) )
		return;
	CloseHandle( hPipeSrcRead );

	CWinThread *pThread=AfxBeginThread(ListenLogVersionProc,&hPipeSrcReadDup,THREAD_PRIORITY_NORMAL);
	//step3:创建子进程,
	if( !CreateLogVersionProcess( hPipeClientRead, hPipeClientWrite))
		return;
	WriteLogVersionToClient(hPipeSrcWriteDup,pLogVersion,sLogVersionScope);
}

void CLogViewerDlg::OnBnClickedBtnView()
{
	OnViewLogVersion();
}
void CLogViewerDlg::OnViewLogVersion()
{
	if (m_xLogVersionList.GetSelectedCount()<=0)
		return;
	char sLogVersionScope[50]="";
	if(m_xLogVersionList.GetSelectedCount()==1)
	{	//当前仅选中一项
		int iCurSel=m_xLogVersionList.GetSelectedItem();
		if(iCurSel>=0)
		{
			CXhLogVersion *pVersion=(CXhLogVersion*)m_xLogVersionList.GetItemData(iCurSel);
			if(pVersion)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sLogVersionScope,"[Version Scope:%d]",pVersion->GetSerialNo());
#else 
				sprintf(sLogVersionScope,"[版本范围:%d]",pVersion->GetSerialNo());
#endif
				DisplayLogVersion(pVersion,sLogVersionScope);
			}
		}
	}
	else 
	{	//选中多项时查看索引值最小项与索引值最大项之间所有版本日志的全部改动内容
		int iMaxIndex=-1,iMinIndex=m_xLogVersionList.GetItemCount()+1;
		POSITION pos = m_xLogVersionList.GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			int iCurSel = m_xLogVersionList.GetNextSelectedItem(pos);
			if(iMaxIndex<iCurSel)
				iMaxIndex=iCurSel;
			if(iMinIndex>iCurSel)
				iMinIndex=iCurSel;
		}
		if(iMinIndex==iMaxIndex)
			iMinIndex=0;
		CXhLogVersion logVersion;
		for(int i=iMaxIndex;i>=iMinIndex;i--)
		{
			CXhLogVersion *pLogVersion=(CXhLogVersion*)m_xLogVersionList.GetItemData(i);
			CXhLog::MergeLogVersion(&logVersion,pLogVersion);
			if(i==iMaxIndex)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sLogVersionScope,"[Version Scope:%d-",pLogVersion->GetSerialNo());
#else 
				sprintf(sLogVersionScope,"[版本范围:%d-",pLogVersion->GetSerialNo());
#endif
			else if(i==iMinIndex)
				sprintf(sLogVersionScope,"%s%d]",sLogVersionScope,pLogVersion->GetSerialNo());
		}
		DisplayLogVersion(&logVersion,sLogVersionScope);
	}
}

void CLogViewerDlg::OnBnClickedBtnEmptyLog()
{
#ifdef AFX_TARG_ENU_ENGLISH
	if(IDOK==AfxMessageBox("Are you sure to empty log content？",MB_OKCANCEL))
#else 
	if(IDOK==AfxMessageBox("确定要清空日志内容吗？",MB_OKCANCEL))
#endif
	{
		logger.EmptyLogVersion();
		m_xLogVersionList.DeleteAllItems();
		CXhLogVersion::MAX_VERSIONID=0;
		logger.schema.EmptySubMembers();
		CLogSchema::InitCurrentRootSchema(&logger.schema);
	}
}

void CLogViewerDlg::OnNMDblclkListLogVersion(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iCurSel=m_xLogVersionList.GetSelectedItem();
	if(iCurSel<0)
		return;
	CXhLogVersion *pVersion=(CXhLogVersion*)m_xLogVersionList.GetItemData(iCurSel);
	if(pVersion==NULL)
		return;
	char sLogVersionScope[50]="";
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sLogVersionScope,"[Version Scope:%d]",pVersion->GetSerialNo());
#else 
	sprintf(sLogVersionScope,"[版本范围:%d]",pVersion->GetSerialNo());
#endif
	DisplayLogVersion(pVersion,sLogVersionScope);
	*pResult = 0;
}
void CLogViewerDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CRect rect;
	m_xLogVersionList.GetWindowRect(&rect);
	if(!rect.PtInRect(point))
		return;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
#ifdef AFX_TARG_ENU_ENGLISH
	pMenu->AppendMenu(MF_STRING,ID_VIEW_LOG_VERSION,"View Log");
#else 
	pMenu->AppendMenu(MF_STRING,ID_VIEW_LOG_VERSION,"查看日志");
#endif
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this);	
}

void CLogViewerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	RECT rectList;
	if (m_xLogVersionList.GetSafeHwnd())
	{
		m_xLogVersionList.GetWindowRect(&rectList);
		ScreenToClient(&rectList);
		if(m_xLogVersionList.GetSafeHwnd())
			m_xLogVersionList.MoveWindow(0,rectList.top,cx,cy-rectList.top);
	}
}

#endif