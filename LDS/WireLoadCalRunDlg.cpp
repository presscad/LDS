// WireLoadCalRunDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "WireLoadCalRunDlg.h"
#include "Tower.h"
#include "env_def.h"


// CWireLoadCalRunDlg
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)

HANDLE CWireLoadCalRunDlg::ghWireLoadProcess=NULL;
CWireLoadCalRunDlg::CWireLoadCalRunDlg(CWnd* pParent)
	: CDialog(CWireLoadCalRunDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWireLoadCalRunDlg)
	//}}AFX_DATA_INIT
	m_pThread=NULL;
	m_eProcessExecuteState=0;	//Running
}

CWireLoadCalRunDlg::~CWireLoadCalRunDlg()
{
}

void CWireLoadCalRunDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWireLoadCalRunDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWireLoadCalRunDlg, CDialog)
	//{{AFX_MSG_MAP(CWireLoadCalRunDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, &CWireLoadCalRunDlg::OnBnOkCancel)
	ON_WM_NCPAINT()
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CWireLoadCalRunDlg::OnInitDialog()
{
	m_eProcessExecuteState=0;	//Running
	CDialog::OnInitDialog();
	StartWireLoadCalProcess();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CWireLoadCalRunDlg::WireLoadCalFinished(int bRunOK/*=TRUE*/)
{
	m_eProcessExecuteState=bRunOK?1:2;
	if(bRunOK)
		MessageBox("荷载计算完毕!","报告",MB_OK);
	::SendMessage(m_hWnd,WM_CLOSE,0,0);
}
void CWireLoadCalRunDlg::OnBnOkCancel()
{
	if(m_eProcessExecuteState==1)
		CDialog::OnOK();
	else
	{
		if(m_eProcessExecuteState==0&&m_pThread)
			m_pThread->ExitInstance();
		TerminateProcess(ghWireLoadProcess,0);
		CDialog::OnCancel();
	}
	ghWireLoadProcess=NULL;
}

UINT RunWireLoadCalProcess(LPVOID pParam);
void CWireLoadCalRunDlg::StartWireLoadCalProcess()
{
	if(ghWireLoadProcess==NULL)
		m_pThread=AfxBeginThread(RunWireLoadCalProcess,this,THREAD_PRIORITY_NORMAL);
	else
		AfxMessageBox("只能启动一个荷载计算进程!");
}

BOOL CreateWireLoadProcess( HANDLE hClientPipeRead, HANDLE hClientPipeWrite )
{
	//step1:获取荷载计算程序路径
	TCHAR cmd_str[MAX_PATH]="D:\\CimsForTa\\WireLoad\\Debug\\";
#ifndef _DEBUG
	GetSysPath(cmd_str);
#endif
	strcat(cmd_str,"WireLoad.exe -child");

	STARTUPINFO startInfo;
	memset( &startInfo, 0 , sizeof( STARTUPINFO ) );

	startInfo.cb= sizeof( STARTUPINFO );
	startInfo.dwFlags |= STARTF_USESTDHANDLES;
	startInfo.hStdError= 0;//hPipeWrite;
	startInfo.hStdInput= hClientPipeRead;
	startInfo.hStdOutput= hClientPipeWrite;

	PROCESS_INFORMATION processInfo;
	memset( &processInfo, 0, sizeof(PROCESS_INFORMATION) );

	BOOL b=CreateProcess( NULL,cmd_str,
		NULL,NULL, TRUE,CREATE_NEW_CONSOLE, NULL, NULL,&startInfo,&processInfo );
	CWireLoadCalRunDlg::ghWireLoadProcess=b?processInfo.hProcess:NULL;
	DWORD er=GetLastError();
	/*if( b )
	{
		CloseHandle( processInfo.hProcess );
		CloseHandle( processInfo.hThread );
		DWORD dwExitCode=0;
		GetExitCodeProcess(processInfo.hProcess,&dwExitCode)
		if(dwExitCode==STILL_ACTIVE)	;//正常处于活动状态
	}*/

	return b;

}
#include "ArrayList.h"
#include "WireLoadModelBuffer.h"
#include "BufferStack.h"
static BOOL IsControlNode(CLDSNode* pNode)
{
	if( pNode->m_cPosCalType==0||	//无坐标依赖控制节点
		pNode->m_cPosCalType<=3||	//指定坐标分量点
		pNode->m_cPosCalType>=8||	//共面点
	   (pNode->m_cPosCalType==4&&pNode->m_cPosCalViceType>0)||	//异面交叉点
	   (pNode->m_cPosCalType==7&&pNode->m_cPosCalViceType>0))	//指定坐标轴偏移点性质类同于指定坐标分量点
		return TRUE;
	else
		return FALSE;
}
BOOL ExtractBriefModelData(NODESET& node_set,LINEPARTSET& rod_set)
{
	CLDSModule* pAciveModule=Ta.GetActiveModule();
	if(pAciveModule==NULL)
		return FALSE;
	CFGWORD dwBodyWord(pAciveModule->GetBodyNo(),0),
			dwActiveQuad1LegWord(pAciveModule->m_arrActiveQuadLegNo[0]),
			dwActiveQuad2LegWord(pAciveModule->m_arrActiveQuadLegNo[1]),
			dwActiveQuad3LegWord(pAciveModule->m_arrActiveQuadLegNo[2]),
			dwActiveQuad4LegWord(pAciveModule->m_arrActiveQuadLegNo[3]);
	dwBodyWord.AddBodyLegs(pAciveModule->GetBodyNo());
	//提取节点信息
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->layer(0)=='L')
		{	//腿部节点
			//TODO:是否可以用pAciveModule->IsSonPart(pNode)&&IsControlNode(pNode)? wjh-2017.2.10
			int iPos=CalPointQuadPos(pNode->ActivePosition());
			if(   (iPos==1&&pNode->cfgword.And(dwActiveQuad1LegWord))
				||(iPos==2&&pNode->cfgword.And(dwActiveQuad2LegWord))
				||(iPos==3&&pNode->cfgword.And(dwActiveQuad3LegWord))
				||(iPos==4&&pNode->cfgword.And(dwActiveQuad4LegWord)))
			{
				if(IsControlNode(pNode))		//控制节点
					node_set.append(pNode);
			}
		}
		else if(pNode->cfgword.And(dwBodyWord))
		{	//塔身或头部节点
			if(IsControlNode(pNode))		//控制节点
				node_set.append(pNode);
			else if(pNode->IsHangWire())	//挂线节点
				node_set.append(pNode);
		}
	}
	//提取杆件信息
	for(CLDSPart* pPart=Ta.EnumPartFirst();pPart;pPart=Ta.EnumPartNext())
	{
		if(!pPart->IsLinePart())
			continue;
		CLDSLinePart* pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->pStart==NULL || pLinePart->pEnd==NULL)
			continue;
		//TODO:是否可以用pAciveModule->IsSonPart(pNode)&&IsControlNode(pLinePart->pStart)||IsControlNode(pLinePart->pEnd)? wjh-2017.2.10
		if(pLinePart->IsLegObj())
		{	//腿部杆件
			f3dPoint lineStart=pLinePart->pStart->ActivePosition();;
			f3dPoint lineEnd=pLinePart->pEnd->ActivePosition();
			char cPos=CalPoleQuad(lineStart,lineEnd);
			if(   (cPos=='1'&&pPart->cfgword.And(dwActiveQuad1LegWord))
				||(cPos=='2'&&pPart->cfgword.And(dwActiveQuad2LegWord))
				||(cPos=='3'&&pPart->cfgword.And(dwActiveQuad3LegWord))
				||(cPos=='4'&&pPart->cfgword.And(dwActiveQuad4LegWord)))
			{
				if(IsControlNode(pLinePart->pStart)||IsControlNode(pLinePart->pEnd))
					rod_set.append(pLinePart);
			}
		}
		else if(pLinePart->cfgword.And(dwBodyWord))
		{	//塔身或头部杆件
			if(towupper(pLinePart->Layer(1))=='Z'|| towupper(pLinePart->Layer(1))=='H')
				rod_set.append(pLinePart);
			else if(IsControlNode(pLinePart->pStart)||IsControlNode(pLinePart->pEnd))
				rod_set.append(pLinePart);
		}
	}
	//TODO:穿过控制点的杆件是否也应该添加进来？
	return TRUE;
}
bool GetLocalFileVersion(const char* filepathname,char* pszVersion,UINT uiMaxStrBufLen);//defined in LDSView.cpp
static bool GetWireLoadClientVersion(char* pszVersion, UINT uiMaxStrBufLen)
{
	TCHAR szFileName[MAX_PATH] = "D:\\CimsForTa\\WireLoad\\Debug\\";
	//GetSysPath(szFileName);
	strcat(szFileName, "WireLoad.exe");
	return GetLocalFileVersion(szFileName, pszVersion, uiMaxStrBufLen);
}
static BOOL WriteToWireLoadClient(HANDLE hPipeWrite)
{
	if( hPipeWrite == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox("获取管道句柄无效\n");
		return FALSE;
	}

	CBuffer buffer(1000000);	//1Mb
	CBufferStack bufStack;
	buffer.AttachStack(&bufStack);
	//0.写入加密狗证书信息
	CXhChar50 szVersion;
	GetWireLoadClientVersion(szVersion, 51);
	if (compareVersion(szVersion, "1.1.0.0") >= 0)
	{
		UINT uiProcessAuthNumer=GetCurrentProcessSerialNumber();
		CXhChar200 szLicFilePath = APP_PATH;
		szLicFilePath.Append(CXhChar16("%d", uiProcessAuthNumer), '|');
		BYTE cProductType=PRODUCT_TYPE;
		buffer.WriteByte(cProductType);				//产品类型
		buffer.WriteDword(theApp.m_version[0]);		//版本信息
		buffer.WriteDword(theApp.m_version[1]);
		buffer.WriteString(szLicFilePath,201);		//证书路径
	}
	//1.WIRELOAD_MODEL Buffer
	CAttachBuffer *pWireLayoutBuffer=Ta.AttachBuffer.GetValue(CTower::WIRELOAD_MODEL);
	if(pWireLayoutBuffer==NULL)
		pWireLayoutBuffer=Ta.AttachBuffer.Add(CTower::WIRELOAD_MODEL);
	if(pWireLayoutBuffer->GetLength()<=0)
		CWireLoadModelBuffer::InitWireLoadModelBuffer(pWireLayoutBuffer,Ta.weather);
	buffer.WriteInteger(pWireLayoutBuffer->GetLength());
	buffer.Write(pWireLayoutBuffer->GetBufferPtr(),pWireLayoutBuffer->GetLength());
	//2.挂点信息
	buffer.WriteInteger(1);	//VersionNumber=1
	BOOL pushed=buffer.PushPositionStack();
	long hits=0;
	buffer.WriteInteger(hits);
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(!pNode->IsHangWire())
			continue;
		hits++;
		buffer.WriteInteger(pNode->handle);			//后台句柄标识（永久不变）
		buffer.WriteInteger(pNode->point_i);		//写入节点编号（临时全局唯一，且易读易记）
		buffer.WriteByte(pNode->m_cHangWireType);	//写入挂点上所挂电线的类型：'E'地线Earth Wire,'C'导线Conducting Wire,'J'为跳线等辅助线挂点，其余为非挂线点
	}
	buffer.PopPositionStack(pushed);
	buffer.WriteInteger(hits);
	buffer.RestoreNowPosition();
	//3.简易模型数据（主控节点+杆件）
	NODESET nodeSet;
	LINEPARTSET rodSet;
	ExtractBriefModelData(nodeSet,rodSet);
	buffer.WriteInteger(nodeSet.GetNodeNum());
	for(CLDSNode* pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
	{
		buffer.WriteInteger(pNode->handle);
		buffer.WriteInteger(pNode->point_i);
		buffer.WriteByte(pNode->m_cHangWireType);
		buffer.WritePoint(pNode->Position());
		buffer.WriteString(pNode->layer());
	}
	//
	buffer.WriteInteger(rodSet.GetNodeNum());
	for(CLDSLinePart* pRod=rodSet.GetFirst();pRod;pRod=rodSet.GetNext())
	{
		buffer.WritePoint(pRod->pStart->Position());
		buffer.WritePoint(pRod->pEnd->Position());
		buffer.WriteString(pRod->layer());
	}
	//向匿名管道中写入数据

	return buffer.WriteToPipe(hPipeWrite,1024);
}
BOOL ReadFromWireLoadClient(HANDLE hPipeRead)
{
	if( hPipeRead == INVALID_HANDLE_VALUE)
	{
		logerr.Log("获取管道句柄无效\n");
		return FALSE;
	}
	//从匿名管道中读取数据，显示，并返回给服务器
	CBuffer buffer(1000000);	//1Mb
	if(!buffer.ReadFromPipe(hPipeRead,1024))
	{
		logerr.Log("荷载计算异常中断");
		return FALSE;
	}
	buffer.SeekToBegin();
	//1.读取标志位
	BYTE continueFlag=0;
	buffer.ReadByte(&continueFlag);
	if(continueFlag==0)
		return FALSE;
	//2.读取附加缓存
	long wireLoadBufferLen=0;
	buffer.ReadInteger(&wireLoadBufferLen);
	if(wireLoadBufferLen>0)
	{	//更新附加缓冲数据
		CAttachBuffer *pWireLayoutBuffer=Ta.AttachBuffer.GetValue(CTower::WIRELOAD_MODEL);
		pWireLayoutBuffer->ClearContents();
		pWireLayoutBuffer->SetBlockSize(wireLoadBufferLen);
		pWireLayoutBuffer->WriteAt(0,(void*)buffer.GetCursorBuffer(),wireLoadBufferLen);
		//从附加缓存提取工况及荷载信息
		CWireLoadModelBuffer wireLoadModelBuffer(pWireLayoutBuffer->GetBufferPtr(),pWireLayoutBuffer->GetLength());
		wireLoadModelBuffer.RetrieveWireLoadInfo(&Ta);
		//使用LDS荷载计算模块后应自动将荷载执行规范改为0.电力规范，否则如果导入TTA程序的话，默认成用户指定时塔身风计算时荷载组合系数就错了.
		Ta.m_iLoadExecuteCode=0;
		/*ATOM_LIST<CWireLayout> listWireLayout;
		CWireLayoutSetSection wireLayoutSection=wireLoadModelBuffer.GetWireLayoutSetSection();
		wireLayoutSection.RetrieveWireLayoutSet(listWireLayout);*/
	}
	return TRUE;
}
// CWireLoadCalRunDlg 消息处理程序
static HANDLE hPipeSrcReadDup=NULL;
UINT RunWireLoadCalProcess(LPVOID pParam)
{
	CWireLoadCalRunDlg* pForeDlg=(CWireLoadCalRunDlg*)pParam;
	//step1:创建第一个管道: 用于服务器端向客户端发送内容
	SECURITY_ATTRIBUTES attrib;
	attrib.nLength = sizeof( SECURITY_ATTRIBUTES );
	attrib.bInheritHandle= true;
	attrib.lpSecurityDescriptor = NULL;
	HANDLE hPipeClientRead=NULL, hPipeSrcWrite=NULL;

	if( !CreatePipe( &hPipeClientRead, &hPipeSrcWrite, &attrib, 0 ) )
	{
		//printf("创建匿名管道失败!GetLastError= %d\n", GetLastError() );
		pForeDlg->WireLoadCalFinished(FALSE);
		return FALSE;
	}

	//因为此时两个句柄均可继承，故可复制一个不可继承的写句柄并将原来的写句柄关掉
	HANDLE hPipeSrcWriteDup=NULL;
	if( !DuplicateHandle( GetCurrentProcess(), hPipeSrcWrite, GetCurrentProcess(), &hPipeSrcWriteDup, 0, false, DUPLICATE_SAME_ACCESS ) )
	{
		//printf("复制句柄失败,GetLastError=%d\n", GetLastError() );
		pForeDlg->WireLoadCalFinished(FALSE);
		CloseHandle(hPipeClientRead); 
		CloseHandle(hPipeSrcWrite);
		return FALSE;
	}
	CloseHandle( hPipeSrcWrite );

	//step2:创建第二个管道，用于客户端向服务器端发送内容
	HANDLE hPipeClientWrite=NULL, hPipeSrcRead=NULL;
	if( !CreatePipe( &hPipeSrcRead, &hPipeClientWrite, &attrib, 0) )
	{
		//printf("创建第二个匿名管道失败,GetLastError=%d\n", GetLastError() );
		pForeDlg->WireLoadCalFinished(FALSE);
		return FALSE;
	}

	//复制不可继承的读句柄，并把原有的读句柄关掉
	if( !DuplicateHandle( GetCurrentProcess(), hPipeSrcRead, GetCurrentProcess(), &hPipeSrcReadDup, 0, false, DUPLICATE_SAME_ACCESS ) )
	{
		//printf("复制第二个句柄失败,GetLastError=%d\n", GetLastError() );
		pForeDlg->WireLoadCalFinished(FALSE);
		CloseHandle(hPipeClientWrite);
		CloseHandle(hPipeSrcRead);
		return FALSE;
	}
	CloseHandle( hPipeSrcRead );

	//step3:创建子进程,
	if( !CreateWireLoadProcess( hPipeClientRead, hPipeClientWrite ) )
	{
		//printf("创建子进程失败\n" );
		pForeDlg->WireLoadCalFinished(FALSE);
		return FALSE;
	}
	if(WriteToWireLoadClient(hPipeSrcWriteDup))
	{
		if(!ReadFromWireLoadClient(hPipeSrcReadDup))
		{
			pForeDlg->WireLoadCalFinished(FALSE);
			CloseHandle(hPipeClientWrite);  //关闭
			CloseHandle(hPipeSrcWriteDup);
			return FALSE;
		}
		pForeDlg->WireLoadCalFinished(TRUE);
	} 
	else
		pForeDlg->WireLoadCalFinished(FALSE);
	CloseHandle(hPipeClientWrite);
	CloseHandle(hPipeSrcWriteDup);
	return TRUE;
}
#endif