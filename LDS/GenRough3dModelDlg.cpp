// GenRough3dModelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "GenRough3dModelDlg.h"
#include "LicFuncDef.h"


// CGenRough3dModelDlg 对话框

IMPLEMENT_DYNAMIC(CGenRough3dModelDlg, CDialog)

CGenRough3dModelDlg::CGenRough3dModelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGenRough3dModelDlg::IDD, pParent)
	, m_bIntelliSetRodPosition(TRUE)
	, m_bIntelliDesignConnection(FALSE)
	, m_bMechanicPersist(FALSE)
	, m_bCombineColinearRods(TRUE)
	, m_bUsePerfectAngleG(TRUE)
	, m_fTolerance(5)
	, m_fToleranceTangent(0.001)
{
	m_nMinKeyLength = 300;
}

CGenRough3dModelDlg::~CGenRough3dModelDlg()
{
}

void CGenRough3dModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_INTELLI_SET_RODPOS, m_bIntelliSetRodPosition);
	DDX_Check(pDX, IDC_CHK_INTELLI_DESIGN_CONNECTION, m_bIntelliDesignConnection);
	DDX_Check(pDX, IDC_CHK_MECHANIC_PERSIST, m_bMechanicPersist);
	DDX_Check(pDX, IDC_CHK_COMBINE_COLINEAR_RODS, m_bCombineColinearRods);
	DDX_Check(pDX, IDC_CHK_USE_PERFECT_G, m_bUsePerfectAngleG);
	DDX_Text(pDX, IDC_E_TOLERANCE, m_fTolerance);
	DDX_Text(pDX, IDC_E_TOLERANCE_TANGENT, m_fToleranceTangent);
	DDX_Text(pDX, IDC_E_MIN_KEY_LENGTH, m_nMinKeyLength);
	DDV_MinMaxUInt(pDX, m_nMinKeyLength, 100, 10000);
}


BEGIN_MESSAGE_MAP(CGenRough3dModelDlg, CDialog)
END_MESSAGE_MAP()


// CGenRough3dModelDlg 消息处理程序


BOOL CGenRough3dModelDlg::OnInitDialog()
{
	if (!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_TIDMODEL_EXPORT))
		m_bIntelliDesignConnection = FALSE;
	CDialog::OnInitDialog();

	if (!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_TIDMODEL_EXPORT))
		GetDlgItem(IDC_CHK_INTELLI_DESIGN_CONNECTION)->ShowWindow(SW_HIDE);
	//调整对话框布局
	CWnd *pWnd = GetDlgItem(IDC_CHK_INTELLI_DESIGN_CONNECTION);
	if (pWnd && !pWnd->IsWindowVisible())
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		long nChkWndH = rect.Height();
		//移动按钮
		for (int i = 0; i < 2; i++)
		{
			pWnd = (i == 0) ? GetDlgItem(IDOK) : GetDlgItem(IDCANCEL);
			if (pWnd == NULL)
				continue;
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);
			long nBtnWndH = rect.Height();
			rect.top = rect.top - nChkWndH;
			rect.bottom = rect.top + nBtnWndH;
			pWnd->MoveWindow(rect);
		}
		//调整对话框
		CWnd::GetWindowRect(rect);
		int height = rect.Height();
		rect.bottom = rect.top + height - nChkWndH;
		MoveWindow(rect, TRUE);
	}
	return TRUE;
}
