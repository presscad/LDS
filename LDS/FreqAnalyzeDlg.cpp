// FreqAnalyzeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "FreqAnalyzeDlg.h"
#include "ParseAdaptNo.h"


// CFreqAnalyzeDlg 对话框

IMPLEMENT_DYNAMIC(CFreqAnalyzeDlg, CDialog)

CFreqAnalyzeDlg::CFreqAnalyzeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFreqAnalyzeDlg::IDD, pParent)
	, m_sHeightNo(_T(""))
	, m_iElemProcessMode(0)
	, m_nFreqOrderCount(0)
	, m_bWeightAmplifyCoefIncBraceRods(FALSE)
	, m_bAreaAmplifyCoefIncBraceRods(FALSE)
{

}

CFreqAnalyzeDlg::~CFreqAnalyzeDlg()
{
}

void CFreqAnalyzeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_CMB_CURR_CAL_HEIGHT, m_sHeightNo);
	DDX_Radio(pDX, IDC_RDO_ELEM_TMP_ANALYZE, m_iElemProcessMode);
	DDX_Text(pDX, IDC_E_ORDER_COUNT, m_nFreqOrderCount);
	DDV_MinMaxInt(pDX, m_nFreqOrderCount, 1, 8);
	DDX_Check(pDX, IDC_CHK_WEIGHT_COEF_INC_BRACE_RODS, m_bWeightAmplifyCoefIncBraceRods);
	DDX_Check(pDX, IDC_CHK_AREA_COEF_INC_BRACE_RODS, m_bAreaAmplifyCoefIncBraceRods);
}


BEGIN_MESSAGE_MAP(CFreqAnalyzeDlg, CDialog)
	ON_BN_CLICKED(IDC_CHK_WEIGHT_COEF_INC_BRACE_RODS, &CFreqAnalyzeDlg::OnChkWeightCoefIncBraceRods)
END_MESSAGE_MAP()

BOOL CFreqAnalyzeDlg::OnInitDialog()
{
	CXhChar200 szModuleItems = m_sModuleScope;
	int iModuleNo = FindAdaptNo(szModuleItems);
	CComboBox *pCMBHeight= (CComboBox*)GetDlgItem(IDC_CMB_CURR_CAL_HEIGHT);
	pCMBHeight->ResetContent();
	while (iModuleNo != 0)
	{
		int ii = pCMBHeight->AddString(CXhChar16(iModuleNo));
		iModuleNo = FindAdaptNo(NULL);
	}
	return CDialog::OnInitDialog();
}
// CFreqAnalyzeDlg 消息处理程序

void CFreqAnalyzeDlg::OnChkWeightCoefIncBraceRods()
{
	UpdateData();
	this->m_bAreaAmplifyCoefIncBraceRods = this->m_bWeightAmplifyCoefIncBraceRods;
	UpdateData(FALSE);
}
