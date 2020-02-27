//<LOCALE_TRANSLATE/>
// SelMapSegDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "SelMapSegDlg.h"
#include "Tower.h"
#include "RxTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelMapSegDlg dialog


CSelMapSegDlg::CSelMapSegDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelMapSegDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelMapSegDlg)
	m_sStruSegNo = _T("");
	m_nSegNum = 0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sTypeName = _T("segment");
	//}}AFX_DATA_INIT
	m_sCaption = _T("select current segment number");
#else
	m_sTypeName = _T("个分段");
	//}}AFX_DATA_INIT
	m_sCaption = _T("选择当前分段号");
#endif
}


void CSelMapSegDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelMapSegDlg)
	DDX_CBString(pDX, IDC_CMB_STRU_NO, m_sStruSegNo);
	DDX_Text(pDX, IDC_STRU_NUM, m_nSegNum);
	DDX_Text(pDX, IDC_S_TYPE, m_sTypeName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelMapSegDlg, CDialog)
	//{{AFX_MSG_MAP(CSelMapSegDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelMapSegDlg message handlers

BOOL CSelMapSegDlg::OnInitDialog() 
{
#ifdef __COMMON_PART_INC_
	CComboBox* pCMB = (CComboBox*)GetDlgItem(IDC_CMB_STRU_NO);
	SetWindowText(m_sCaption);
	ATOM_LIST<SEGI> segNoList;
	m_nSegNum=Ta.GetSegNoList(segNoList);
	CString sItem;
	for(SEGI *pSegI=segNoList.GetFirst();pSegI;pSegI=segNoList.GetNext())
	{
		sItem=pSegI->ToString();
		pCMB->AddString(sItem);
		if(Ta.m_iCurSeg.iSeg<=0)
			Ta.m_iCurSeg=*pSegI;
	}
	m_sStruSegNo=Ta.m_iCurSeg.ToString();
#endif
	CDialog::OnInitDialog();
	return TRUE;
}
