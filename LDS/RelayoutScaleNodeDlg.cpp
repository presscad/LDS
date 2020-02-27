// RelayoutScaleNodeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "RelayoutScaleNodeDlg.h"
#include "afxdialogex.h"
#include "Expression.h"

// CRelayoutScaleNodeDlg 对话框

IMPLEMENT_DYNAMIC(CRelayoutScaleNodeDlg, CDialogEx)

CRelayoutScaleNodeDlg::CRelayoutScaleNodeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRelayoutScaleNodeDlg::IDD, pParent)
	, m_nSectCountNow(0)
	, m_sPivotScale(_T("1"))
{
	m_fPivotScale=1.0;
	m_nSectCountDest = 0;
}

CRelayoutScaleNodeDlg::~CRelayoutScaleNodeDlg()
{
}

void CRelayoutScaleNodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_SECT_COUNT_NOW, m_nSectCountNow);
	DDX_Text(pDX, IDC_E_SECT_COUNT_DEST, m_nSectCountDest);
	DDV_MinMaxInt(pDX, m_nSectCountDest, 2, 15);
	DDX_Text(pDX, IDC_E_PIVOT_SCALE, m_sPivotScale);
}


BEGIN_MESSAGE_MAP(CRelayoutScaleNodeDlg, CDialogEx)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_E_SECT_COUNT_DEST, &CRelayoutScaleNodeDlg::OnChangeESectCountDest)
	ON_EN_CHANGE(IDC_E_SECT_COUNT_NOW, &CRelayoutScaleNodeDlg::OnChangeESectCountNow)
END_MESSAGE_MAP()


// CRelayoutScaleNodeDlg 消息处理程序


BOOL CRelayoutScaleNodeDlg::OnInitDialog()
{
	CXhChar16 sPivotScale("1");
	CExpression::ToExpression(m_fPivotScale,3,sPivotScale,16);
	m_sPivotScale=sPivotScale;
	CDialogEx::OnInitDialog();

	CSpinButtonCtrl* pSpinCtrl=(CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_SECT_COUNT);
	pSpinCtrl->SetBuddy((CEdit*)GetDlgItem(IDC_E_SECT_COUNT_DEST));
	pSpinCtrl->SetRange32(2,15);
	pSpinCtrl->SetPos(m_nSectCountDest);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CRelayoutScaleNodeDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if (pScrollBar->GetDlgCtrlID() == IDC_SPIN_SECT_COUNT)
	{
        CString strValue;
        strValue.Format("%d", nPos);
       ((CSpinButtonCtrl*) pScrollBar)->GetBuddy()->SetWindowText(strValue);
    }
	
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CRelayoutScaleNodeDlg::OnChangeESectCountDest()
{
	UpdateData();
	CSpinButtonCtrl* pSpinCtrl=(CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_SECT_COUNT);
	pSpinCtrl->SetPos(m_nSectCountDest);
}


void CRelayoutScaleNodeDlg::OnChangeESectCountNow()
{
	UpdateData();
	m_nSectCountDest=m_nSectCountNow;
	UpdateData(FALSE);
}


void CRelayoutScaleNodeDlg::OnOK()
{
	UpdateData();
	CExpression expression;
	m_fPivotScale=expression.SolveExpression(m_sPivotScale);
	CDialogEx::OnOK();
}
