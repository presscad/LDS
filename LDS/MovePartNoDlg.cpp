//<LOCALE_TRANSLATE Confirm by hxr/>
// MovePartNoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "MovePartNoDlg.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMovePartNoDlg dialog


CMovePartNoDlg::CMovePartNoDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CMovePartNoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMovePartNoDlg)
	m_sPartLable = _T("");
	m_iMoveType = 0;
	//}}AFX_DATA_INIT
	m_pPickPart = NULL;
	m_sPartNo = _T("");
	m_hPart = 0;
}


void CMovePartNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMovePartNoDlg)
	DDX_Text(pDX, IDC_E_PART, m_sPartLable);
	DDX_Radio(pDX, IDC_RAD_PARTNO, m_iMoveType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMovePartNoDlg, CDialog)
	//{{AFX_MSG_MAP(CMovePartNoDlg)
	ON_BN_CLICKED(IDC_RAD_PARTNO, OnMoveType)
	ON_BN_CLICKED(IDC_RAD_HANDLE, OnMoveType)
	ON_BN_CLICKED(IDC_BTN_SELECT_PART, OnSelectPart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMovePartNoDlg message handlers

BOOL CMovePartNoDlg::OnInitDialog() 
{
	m_pPickPart = NULL;
	if(m_bInernalStart&&m_hPickObj>0)
		m_pPickPart = Ta.FromPartHandle(m_hPickObj);
	if(m_pPickPart!=NULL)
	{
		if(m_iMoveType==0)
			m_sPartLable = m_pPickPart->GetPartNo();
		else 
			m_sPartLable.Format("0X%X",m_pPickPart->handle);
		UpdateData(FALSE);
	}
	CDialog::OnInitDialog();
	return TRUE;
}

void CMovePartNoDlg::InitPartByPartLable()
{
	if(m_iMoveType==0)
	{
		if(m_pPickPart&&m_sPartLable.CompareNoCase(m_pPickPart->GetPartNo())==0)
			return;	//不需要更新
		for(CLDSPart *pPart=Ta.EnumPartFirst();pPart!=NULL;pPart=Ta.EnumPartNext())
		{
			if(m_sPartLable.CompareNoCase(pPart->GetPartNo())==0)
			{
				m_pPickPart=pPart;
				break;
			}
		}
	}
	else 
	{
		long hPart;
		sscanf(m_sPartLable,"%X",&hPart);
		if(m_pPickPart&&m_pPickPart->handle==hPart)
			return;	//不需要更新
		m_pPickPart = Ta.FromPartHandle(hPart);
	}
}

void CMovePartNoDlg::OnMoveType()
{
	InitPartByPartLable();
	UpdateData();
	if(m_pPickPart)
	{
		if(m_iMoveType==0)
			m_sPartLable = m_pPickPart->GetPartNo();
		else 
			m_sPartLable.Format("0X%X",m_pPickPart->handle);
	}
	else 
		m_sPartLable.Empty();
	UpdateData(FALSE);
}

void CMovePartNoDlg::OnSelectPart() 
{
	g_pSolidSet->SetOperType(OPER_OTHER);

	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	CDialog::OnOK();
}

void CMovePartNoDlg::OnOK()
{
	UpdateData();
	InitPartByPartLable();
	if(m_pPickPart==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The input part does not exist, please input again!");
#else
		AfxMessageBox("输入的构件不存在,请重新输入！");
#endif
		return;
	}
	m_hPart = m_pPickPart->handle;
	m_sPartNo = m_pPickPart->GetPartNo();
	CDialog::OnOK();
}