// WireLoadCalRunDlg.cpp : ʵ���ļ�
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
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CWireLoadCalRunDlg::WireLoadCalFinished(int bRunOK/*=TRUE*/)
{
	m_eProcessExecuteState=bRunOK?1:2;
	if(bRunOK)
		MessageBox("���ؼ������!","����",MB_OK);
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
		AfxMessageBox("ֻ������һ�����ؼ������!");
}

BOOL CreateWireLoadProcess( HANDLE hClientPipeRead, HANDLE hClientPipeWrite )
{
	//step1:��ȡ���ؼ������·��
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
		if(dwExitCode==STILL_ACTIVE)	;//�������ڻ״̬
	}*/

	return b;

}
#include "ArrayList.h"
#include "WireLoadModelBuffer.h"
#include "BufferStack.h"
static BOOL IsControlNode(CLDSNode* pNode)
{
	if( pNode->m_cPosCalType==0||	//�������������ƽڵ�
		pNode->m_cPosCalType<=3||	//ָ�����������
		pNode->m_cPosCalType>=8||	//�����
	   (pNode->m_cPosCalType==4&&pNode->m_cPosCalViceType>0)||	//���潻���
	   (pNode->m_cPosCalType==7&&pNode->m_cPosCalViceType>0))	//ָ��������ƫ�Ƶ�������ͬ��ָ�����������
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
	//��ȡ�ڵ���Ϣ
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->layer(0)=='L')
		{	//�Ȳ��ڵ�
			//TODO:�Ƿ������pAciveModule->IsSonPart(pNode)&&IsControlNode(pNode)? wjh-2017.2.10
			int iPos=CalPointQuadPos(pNode->ActivePosition());
			if(   (iPos==1&&pNode->cfgword.And(dwActiveQuad1LegWord))
				||(iPos==2&&pNode->cfgword.And(dwActiveQuad2LegWord))
				||(iPos==3&&pNode->cfgword.And(dwActiveQuad3LegWord))
				||(iPos==4&&pNode->cfgword.And(dwActiveQuad4LegWord)))
			{
				if(IsControlNode(pNode))		//���ƽڵ�
					node_set.append(pNode);
			}
		}
		else if(pNode->cfgword.And(dwBodyWord))
		{	//�����ͷ���ڵ�
			if(IsControlNode(pNode))		//���ƽڵ�
				node_set.append(pNode);
			else if(pNode->IsHangWire())	//���߽ڵ�
				node_set.append(pNode);
		}
	}
	//��ȡ�˼���Ϣ
	for(CLDSPart* pPart=Ta.EnumPartFirst();pPart;pPart=Ta.EnumPartNext())
	{
		if(!pPart->IsLinePart())
			continue;
		CLDSLinePart* pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->pStart==NULL || pLinePart->pEnd==NULL)
			continue;
		//TODO:�Ƿ������pAciveModule->IsSonPart(pNode)&&IsControlNode(pLinePart->pStart)||IsControlNode(pLinePart->pEnd)? wjh-2017.2.10
		if(pLinePart->IsLegObj())
		{	//�Ȳ��˼�
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
		{	//�����ͷ���˼�
			if(towupper(pLinePart->Layer(1))=='Z'|| towupper(pLinePart->Layer(1))=='H')
				rod_set.append(pLinePart);
			else if(IsControlNode(pLinePart->pStart)||IsControlNode(pLinePart->pEnd))
				rod_set.append(pLinePart);
		}
	}
	//TODO:�������Ƶ�ĸ˼��Ƿ�ҲӦ����ӽ�����
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
		AfxMessageBox("��ȡ�ܵ������Ч\n");
		return FALSE;
	}

	CBuffer buffer(1000000);	//1Mb
	CBufferStack bufStack;
	buffer.AttachStack(&bufStack);
	//0.д����ܹ�֤����Ϣ
	CXhChar50 szVersion;
	GetWireLoadClientVersion(szVersion, 51);
	if (compareVersion(szVersion, "1.1.0.0") >= 0)
	{
		UINT uiProcessAuthNumer=GetCurrentProcessSerialNumber();
		CXhChar200 szLicFilePath = APP_PATH;
		szLicFilePath.Append(CXhChar16("%d", uiProcessAuthNumer), '|');
		BYTE cProductType=PRODUCT_TYPE;
		buffer.WriteByte(cProductType);				//��Ʒ����
		buffer.WriteDword(theApp.m_version[0]);		//�汾��Ϣ
		buffer.WriteDword(theApp.m_version[1]);
		buffer.WriteString(szLicFilePath,201);		//֤��·��
	}
	//1.WIRELOAD_MODEL Buffer
	CAttachBuffer *pWireLayoutBuffer=Ta.AttachBuffer.GetValue(CTower::WIRELOAD_MODEL);
	if(pWireLayoutBuffer==NULL)
		pWireLayoutBuffer=Ta.AttachBuffer.Add(CTower::WIRELOAD_MODEL);
	if(pWireLayoutBuffer->GetLength()<=0)
		CWireLoadModelBuffer::InitWireLoadModelBuffer(pWireLayoutBuffer,Ta.weather);
	buffer.WriteInteger(pWireLayoutBuffer->GetLength());
	buffer.Write(pWireLayoutBuffer->GetBufferPtr(),pWireLayoutBuffer->GetLength());
	//2.�ҵ���Ϣ
	buffer.WriteInteger(1);	//VersionNumber=1
	BOOL pushed=buffer.PushPositionStack();
	long hits=0;
	buffer.WriteInteger(hits);
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(!pNode->IsHangWire())
			continue;
		hits++;
		buffer.WriteInteger(pNode->handle);			//��̨�����ʶ�����ò��䣩
		buffer.WriteInteger(pNode->point_i);		//д��ڵ��ţ���ʱȫ��Ψһ�����׶��׼ǣ�
		buffer.WriteByte(pNode->m_cHangWireType);	//д��ҵ������ҵ��ߵ����ͣ�'E'����Earth Wire,'C'����Conducting Wire,'J'Ϊ���ߵȸ����߹ҵ㣬����Ϊ�ǹ��ߵ�
	}
	buffer.PopPositionStack(pushed);
	buffer.WriteInteger(hits);
	buffer.RestoreNowPosition();
	//3.����ģ�����ݣ����ؽڵ�+�˼���
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
	//�������ܵ���д������

	return buffer.WriteToPipe(hPipeWrite,1024);
}
BOOL ReadFromWireLoadClient(HANDLE hPipeRead)
{
	if( hPipeRead == INVALID_HANDLE_VALUE)
	{
		logerr.Log("��ȡ�ܵ������Ч\n");
		return FALSE;
	}
	//�������ܵ��ж�ȡ���ݣ���ʾ�������ظ�������
	CBuffer buffer(1000000);	//1Mb
	if(!buffer.ReadFromPipe(hPipeRead,1024))
	{
		logerr.Log("���ؼ����쳣�ж�");
		return FALSE;
	}
	buffer.SeekToBegin();
	//1.��ȡ��־λ
	BYTE continueFlag=0;
	buffer.ReadByte(&continueFlag);
	if(continueFlag==0)
		return FALSE;
	//2.��ȡ���ӻ���
	long wireLoadBufferLen=0;
	buffer.ReadInteger(&wireLoadBufferLen);
	if(wireLoadBufferLen>0)
	{	//���¸��ӻ�������
		CAttachBuffer *pWireLayoutBuffer=Ta.AttachBuffer.GetValue(CTower::WIRELOAD_MODEL);
		pWireLayoutBuffer->ClearContents();
		pWireLayoutBuffer->SetBlockSize(wireLoadBufferLen);
		pWireLayoutBuffer->WriteAt(0,(void*)buffer.GetCursorBuffer(),wireLoadBufferLen);
		//�Ӹ��ӻ�����ȡ������������Ϣ
		CWireLoadModelBuffer wireLoadModelBuffer(pWireLayoutBuffer->GetBufferPtr(),pWireLayoutBuffer->GetLength());
		wireLoadModelBuffer.RetrieveWireLoadInfo(&Ta);
		//ʹ��LDS���ؼ���ģ���Ӧ�Զ�������ִ�й淶��Ϊ0.�����淶�������������TTA����Ļ���Ĭ�ϳ��û�ָ��ʱ��������ʱ�������ϵ���ʹ���.
		Ta.m_iLoadExecuteCode=0;
		/*ATOM_LIST<CWireLayout> listWireLayout;
		CWireLayoutSetSection wireLayoutSection=wireLoadModelBuffer.GetWireLayoutSetSection();
		wireLayoutSection.RetrieveWireLayoutSet(listWireLayout);*/
	}
	return TRUE;
}
// CWireLoadCalRunDlg ��Ϣ�������
static HANDLE hPipeSrcReadDup=NULL;
UINT RunWireLoadCalProcess(LPVOID pParam)
{
	CWireLoadCalRunDlg* pForeDlg=(CWireLoadCalRunDlg*)pParam;
	//step1:������һ���ܵ�: ���ڷ���������ͻ��˷�������
	SECURITY_ATTRIBUTES attrib;
	attrib.nLength = sizeof( SECURITY_ATTRIBUTES );
	attrib.bInheritHandle= true;
	attrib.lpSecurityDescriptor = NULL;
	HANDLE hPipeClientRead=NULL, hPipeSrcWrite=NULL;

	if( !CreatePipe( &hPipeClientRead, &hPipeSrcWrite, &attrib, 0 ) )
	{
		//printf("���������ܵ�ʧ��!GetLastError= %d\n", GetLastError() );
		pForeDlg->WireLoadCalFinished(FALSE);
		return FALSE;
	}

	//��Ϊ��ʱ����������ɼ̳У��ʿɸ���һ�����ɼ̳е�д�������ԭ����д����ص�
	HANDLE hPipeSrcWriteDup=NULL;
	if( !DuplicateHandle( GetCurrentProcess(), hPipeSrcWrite, GetCurrentProcess(), &hPipeSrcWriteDup, 0, false, DUPLICATE_SAME_ACCESS ) )
	{
		//printf("���ƾ��ʧ��,GetLastError=%d\n", GetLastError() );
		pForeDlg->WireLoadCalFinished(FALSE);
		CloseHandle(hPipeClientRead); 
		CloseHandle(hPipeSrcWrite);
		return FALSE;
	}
	CloseHandle( hPipeSrcWrite );

	//step2:�����ڶ����ܵ������ڿͻ�����������˷�������
	HANDLE hPipeClientWrite=NULL, hPipeSrcRead=NULL;
	if( !CreatePipe( &hPipeSrcRead, &hPipeClientWrite, &attrib, 0) )
	{
		//printf("�����ڶ��������ܵ�ʧ��,GetLastError=%d\n", GetLastError() );
		pForeDlg->WireLoadCalFinished(FALSE);
		return FALSE;
	}

	//���Ʋ��ɼ̳еĶ����������ԭ�еĶ�����ص�
	if( !DuplicateHandle( GetCurrentProcess(), hPipeSrcRead, GetCurrentProcess(), &hPipeSrcReadDup, 0, false, DUPLICATE_SAME_ACCESS ) )
	{
		//printf("���Ƶڶ������ʧ��,GetLastError=%d\n", GetLastError() );
		pForeDlg->WireLoadCalFinished(FALSE);
		CloseHandle(hPipeClientWrite);
		CloseHandle(hPipeSrcRead);
		return FALSE;
	}
	CloseHandle( hPipeSrcRead );

	//step3:�����ӽ���,
	if( !CreateWireLoadProcess( hPipeClientRead, hPipeClientWrite ) )
	{
		//printf("�����ӽ���ʧ��\n" );
		pForeDlg->WireLoadCalFinished(FALSE);
		return FALSE;
	}
	if(WriteToWireLoadClient(hPipeSrcWriteDup))
	{
		if(!ReadFromWireLoadClient(hPipeSrcReadDup))
		{
			pForeDlg->WireLoadCalFinished(FALSE);
			CloseHandle(hPipeClientWrite);  //�ر�
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