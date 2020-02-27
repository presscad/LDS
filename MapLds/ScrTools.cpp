//<LOCALE_TRANSLATE/>
#include "stdafx.h"
//#include "TMA.h"
#include "ScrTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeneralInputDlg dialog

/////////////////////////////////////////////////////////////////////////////
//----00.03.01Wjh programed, Input(* VarPrompt,...)ȫ�ֺ�������----//

int Input(char* VarPrompt, void* VarName, char VarType)
{ 
	CGeneralInputDlg dlg;
	dlg.m_sVarPrompt=VarPrompt;
	switch(VarType)
	{
	case 'c':
		dlg.m_sVarVal.SetAt(0,*(char*)VarName);
		break;
	case 's':
		dlg.m_sVarVal=*(CString*)VarName;
		break;
	case 't':
		dlg.m_sVarVal.Format("%d",*(short int*)VarName);
		break;
	case 'i':
	case 'd':
		dlg.m_sVarVal.Format("%d",*(int*)VarName);
		break;
	case 'l':
		dlg.m_sVarVal.Format("%d",*(long*)VarName);
		break;
	case 'e':
	case 'f':
		dlg.m_sVarVal.Format("%f",*(double*)VarName);
		break;
	default:
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Data type is wrong! Please consult the application developer!");
#else
		AfxMessageBox("������������,���ҳ�������ѯ!");
#endif
		return -1;
	}
	if(dlg.DoModal()!=IDOK)
		return 0;
	switch(VarType)
	{
	case 'c':
		(*(char*)VarName)=dlg.m_sVarVal[0];
		break;
	case 's':
		(*(CString*)VarName)=dlg.m_sVarVal;
		break;
	case 't':
		(*(short int*)VarName)=atoi(dlg.m_sVarVal);
		break;
	case 'd':
		(*(int*)VarName)=atoi(dlg.m_sVarVal);
		break;
	case 'l':
		(*(long*)VarName)=atoi(dlg.m_sVarVal);
		break;
	case 'e':
	case 'f':
		(*(double*)VarName)=atof(dlg.m_sVarVal);
		break;
	default:
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Data type is wrong! Please consult the application developer!");
#else
		AfxMessageBox("������������,���ҳ�������ѯ!");
#endif
		return -1;
	}
	return 1;
}

CGeneralInputDlg::CGeneralInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGeneralInputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGeneralInputDlg)
	m_sVarVal = _T("");
	m_sVarPrompt = _T("");
	//}}AFX_DATA_INIT
}


void CGeneralInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralInputDlg)
	DDX_Text(pDX, IDC_INPUT_VAL_EDIT, m_sVarVal);
	DDX_Text(pDX, IDC_VAR_PROMPT_STATIC, m_sVarPrompt);
	DDV_MaxChars(pDX, m_sVarPrompt, 68);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGeneralInputDlg, CDialog)
	//{{AFX_MSG_MAP(CGeneralInputDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneralInputDlg message handlers

/////////////////////////////////////////////////////////////////////////////
//----00.03.01Wjh programed, PromptBox(char* FormatStr,...)ȫ�ֺ�������----//
static UINT Key;
CEvent EventKill;
static CMessageDlg MsgDlg;

int PromptBox(char* FormatStr,...)
{
	va_list ap;
	CString str0, str1;

	str0= FormatStr;
	va_start(ap, FormatStr);
	str1.FormatV(str0,ap);
	MsgDlg.m_sInformationPrompt=str1;
	MsgDlg.DoModal();
	return Key;
}

//--------------------------CMessageDlg�Ի����ඨ��-------------------------//
// CMessageDlg dialog


CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessageDlg)
	m_sInformationPrompt = _T("");
	//}}AFX_DATA_INIT
}

#if defined(_WIN64)
INT_PTR CMessageDlg::DoModal()
#else
int CMessageDlg::DoModal() 
#endif
{
	// TODO: Add your specialized code here and/or call the base class
	int id;
	id=CDialog::DoModal();
	if(::WaitForSingleObject(EventKill,0)!=WAIT_OBJECT_0)
	{
		if(id==IDOK)
			Key= VK_RETURN;
		else
			Key= VK_ESCAPE;
	}
	EventKill.SetEvent();
	return id;
}

void CMessageDlg::DestroyDlg()
{
	EventKill.SetEvent();
	CDialog::OnOK();
}

void CMessageDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	Key= VK_RETURN;
	DestroyDlg();
}

void CMessageDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	Key= VK_ESCAPE;
	DestroyDlg();
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageDlg)
	DDX_Control(pDX, IDC_TALK_MESSAGE_STATIC, m_OneKeyDown);
	DDX_Text(pDX, IDC_TALK_MESSAGE_STATIC, m_sInformationPrompt);
	DDV_MaxChars(pDX, m_sInformationPrompt, 1000);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)
	//{{AFX_MSG_MAP(CMessageDlg)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CMessageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int sLength, nHeight, nWidth;
	sLength = m_sInformationPrompt.GetLength();
	if(sLength<=20)
	{
		nWidth  = 10+9*sLength;
		nHeight = 45;
	}
	else
	{
		nWidth  = 350;
		nHeight = 50+16*(sLength/50);
	}
	MoveWindow(0, 0, nWidth, nHeight);
	CenterWindow();
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMessageDlg::OnOK() 
{
	//�ÿպ�����ֹ�û���ESC��ENTER�����ر�MESSAGE��
}

void CMessageDlg::OnCancel() 
{
	//�ÿպ�����ֹ�û���ESC��ENTER�����ر�MESSAGE��
}

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg message handlers
/////////////////////////////////////////////////////////////////////////////
// COneKeyDown

COneKeyDown::COneKeyDown()
{

}

COneKeyDown::~COneKeyDown()
{
}

BEGIN_MESSAGE_MAP(COneKeyDown, CStatic)
	//{{AFX_MSG_MAP(COneKeyDown)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COneKeyDown message handlers

void COneKeyDown::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	Key= nChar;
	MsgDlg.DestroyDlg();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//----00.03.01Wjh programed, PutMessage(CView*, char* FormatStr,...)ȫ�ֺ�������----//
static CPutMessage* pDlg;

int PutMessage(CWnd* pWnd, const char* FormatStr,...)
{
	va_list ap;
	CString str0, str1;

	str0= FormatStr;
	va_start(ap, FormatStr);
	str1.FormatV(str0,ap);
	if(str1.GetLength()!=0)
	{
		if(pDlg->GetSafeHwnd()==0)
		{
			pDlg = new CPutMessage(pWnd);
			pDlg->m_sMessageContent = str1;
			pDlg->Create();
		}
		else
		{
			pDlg->m_sMessageContent = str1;
			pDlg->Refresh();
		}
	}
	else if(pDlg)
	{
		pDlg->DestroyWindow();
		delete pDlg;
		pDlg = NULL;
		return 0;
	}

	return 1;
}

//--------------------------CPutMessage�Ի����ඨ��-------------------------//
// CPutMessage dialog


CPutMessage::CPutMessage(CWnd* pWnd)
{
	m_pWnd = pWnd;
}

BOOL CPutMessage::Create()
{
	return CDialog::Create(CPutMessage::IDD, m_pWnd);
}

void CPutMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPutMessage)
	DDX_Text(pDX, IDC_MESSAGE_CONTENT_STATIC, m_sMessageContent);
	DDV_MaxChars(pDX, m_sMessageContent, 1000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPutMessage, CDialog)
	//{{AFX_MSG_MAP(CPutMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPutMessage message handlers

BOOL CPutMessage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int sLength, nHeight, nWidth;
	sLength = m_sMessageContent.GetLength();
	if(sLength<=20)
	{
		nWidth  = 10+7*sLength;
		nHeight = 35;
	}
	else
	{
		nWidth  = 350;
		nHeight = 35+15*(sLength/50);
	}
	MoveWindow(0, 0, nWidth, nHeight);
	CenterWindow();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPutMessage::OnOK() 
{
	//�ÿպ�����ֹ�û���ESC��ENTER�����ر�MESSAGE��
}

void CPutMessage::OnCancel() 
{
	//�ÿպ�����ֹ�û���ESC��ENTER�����ر�MESSAGE��
}

void CPutMessage::Refresh() 
{
	UpdateData(FALSE);
}

void CMessageDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}
