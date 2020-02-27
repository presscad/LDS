// BatchCalWindSegBetaZDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "afxdialogex.h"
#include "BatchCalWindSegBetaZDlg.h"


// CBatchCalWindSegBetaZDlg 对话框
#ifdef __PART_DESIGN_INC_

IMPLEMENT_DYNAMIC(CBatchCalWindSegBetaZDlg, CDialog)

CBatchCalWindSegBetaZDlg::CBatchCalWindSegBetaZDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBatchCalWindSegBetaZDlg::IDD, pParent)
	, m_iBatchCalBetaZStyle(0)
{
	m_fBetaZCalWindV = 30.0;
	modes_arr=NULL;
	m_iSelOrder=0;
	m_nOrderNum=1;
#ifdef AFX_TARG_ENU_ENGLISH
	m_listVibraMode.AddColumnHeader("vibration mode",40);
	m_listVibraMode.AddColumnHeader("self-vibration frequency",80);
	m_listVibraMode.AddColumnHeader("self-vibration period",80);
#else
	m_listVibraMode.AddColumnHeader("振型",40);
	m_listVibraMode.AddColumnHeader("自振频率",80);
	m_listVibraMode.AddColumnHeader("自振周期",80);
#endif
}

CBatchCalWindSegBetaZDlg::~CBatchCalWindSegBetaZDlg()
{
}

void CBatchCalWindSegBetaZDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VIBRATION_MODE_INFO, m_listVibraMode);
	DDX_Radio(pDX, IDC_RDO_BY_WIND_X, m_iBatchCalBetaZStyle);
}


BEGIN_MESSAGE_MAP(CBatchCalWindSegBetaZDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_INTELLI_SEL_VIBRA_MODE, &CBatchCalWindSegBetaZDlg::OnBnIntelliSelVibraMode)
	ON_BN_CLICKED(IDC_BTN_PREVIEW_CURR_VIBRA_MODE, &CBatchCalWindSegBetaZDlg::OnBnPreviewCurrVibraMode)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_VIBRATION_MODE_INFO, &CBatchCalWindSegBetaZDlg::OnLvnItemchangedVibrationModeInfo)
	ON_BN_CLICKED(IDOK, &CBatchCalWindSegBetaZDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CBatchCalWindSegBetaZDlg 消息处理程序

BOOL CBatchCalWindSegBetaZDlg::OnInitDialog()
{
	if (modes_arr == NULL)
		return FALSE;
	CDialog::OnInitDialog();

	m_listVibraMode.InitListCtrl();
	CStringArray str_arr;
	str_arr.SetSize(3);
	for(int i=0;i<m_nOrderNum;i++)
	{
		modes_arr[i].period=1/modes_arr[i].freqs;
		str_arr[0].Format("%d",i+1);
		str_arr[1].Format("%.3f",modes_arr[i].freqs);
		str_arr[2].Format("%.3f",modes_arr[i].period);
		//str_arr[3].Format("%.3f",modes_arr[i].gama_x);
		//str_arr[4].Format("%.3f",modes_arr[i].gama_y);
		int iItem=m_listVibraMode.InsertItemRecord(-1,str_arr);
	}
	m_listVibraMode.SetItemState(m_iSelOrder,LVIS_SELECTED|LVIS_FOCUSED ,LVIS_SELECTED|LVIS_FOCUSED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CBatchCalWindSegBetaZDlg::OnBnIntelliSelVibraMode()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CBatchCalWindSegBetaZDlg::OnBnPreviewCurrVibraMode()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CBatchCalWindSegBetaZDlg::OnLvnItemchangedVibrationModeInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	POSITION pos = m_listVibraMode.GetFirstSelectedItemPosition();
	int order = m_listVibraMode.GetNextSelectedItem(pos);
	if (order >= 0)
		m_iSelOrder = order;
	*pResult = 0;
}

void CBatchCalWindSegBetaZDlg::OnBnClickedOk()
{
	POSITION pos=m_listVibraMode.GetFirstSelectedItemPosition();
	int order = m_listVibraMode.GetNextSelectedItem(pos);
	if (order>=0)
		m_iSelOrder = order;
	CDialog::OnOK();
}

#endif
