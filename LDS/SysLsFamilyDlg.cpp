// SysLsFamilyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "SysLsFamilyDlg.h"
#include "afxdialogex.h"
#include "GlobalFunc.h"

// CSysLsFamilyDlg 对话框

IMPLEMENT_DYNAMIC(CSysLsFamilyDlg, CRememberLocationDlg)

CSysLsFamilyDlg::CSysLsFamilyDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CSysLsFamilyDlg::IDD, pParent)
{
	m_pBoltLib=NULL;
	//
	m_xLsFamilyListCtrl.AddColumnHeader("状态");
	m_xLsFamilyListCtrl.AddColumnHeader("螺栓系列");
	//
	m_xListCtrl.AddColumnHeader("级别");
	m_xListCtrl.AddColumnHeader("直径");
	m_xListCtrl.AddColumnHeader("有效长");
	m_xListCtrl.AddColumnHeader("无扣长");
	m_xListCtrl.AddColumnHeader("通厚下限");
	m_xListCtrl.AddColumnHeader("通厚上限");
	m_xListCtrl.AddColumnHeader("重量");
}

CSysLsFamilyDlg::~CSysLsFamilyDlg()
{
}

void CSysLsFamilyDlg::DoDataExchange(CDataExchange* pDX)
{
	CRememberLocationDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CTRL, m_xListCtrl);
	DDX_Control(pDX, IDC_LS_FAMILY_LIST_CTRL, m_xLsFamilyListCtrl);
}


BEGIN_MESSAGE_MAP(CSysLsFamilyDlg, CRememberLocationDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LS_FAMILY_LIST_CTRL, &CSysLsFamilyDlg::OnNMDblclkLsFamilyListCtrl)
	ON_NOTIFY(NM_CLICK, IDC_LS_FAMILY_LIST_CTRL, &CSysLsFamilyDlg::OnNMClickLsFamilyListCtrl)
END_MESSAGE_MAP()


// CSysLsFamilyDlg 消息处理程序
BOOL CSysLsFamilyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//初始化列表控件
	long col_wide_arr1[7]={40,40,52,52,65,65,70};
	m_xListCtrl.InitListCtrl(col_wide_arr1);
	long col_wide_arr2[2]={40,90};
	m_xLsFamilyListCtrl.InitListCtrl(col_wide_arr2);
	//
	RefreshLsSeriesListCtrl();
	RefreshListCtrl();
	//移动对话框到左上角
	MoveWndToLocation();
	return TRUE;
}
void CSysLsFamilyDlg::RefreshLsSeriesListCtrl()
{
	if(m_pBoltLib==NULL)
		return;
	m_xLsFamilyListCtrl.DeleteAllItems();
	CStringArray str_arr;
	str_arr.SetSize(2);
	for(CLsFamily* pLsFamily=m_pBoltLib->GetFirstFamily();pLsFamily;pLsFamily=m_pBoltLib->GetNextFamily())
	{
		str_arr[0]="";
		str_arr[1].Format("%s",pLsFamily->name);
		int iItem=m_xLsFamilyListCtrl.InsertItemRecord(-1,str_arr);
		m_xLsFamilyListCtrl.SetItemData(iItem,(DWORD)pLsFamily);
	}
}
void CSysLsFamilyDlg::RefreshListCtrl(int iCurSelLsSeries/*=0*/)
{
	CLsFamily* pSelLsFamily=(CLsFamily*)m_xLsFamilyListCtrl.GetItemData(iCurSelLsSeries);
	if(pSelLsFamily==NULL)
		return;
	m_xListCtrl.DeleteAllItems();
	CStringArray str_arr;
	str_arr.SetSize(7);
	for(int i=0;i<pSelLsFamily->GetCount();i++)
	{
		LS_XING_HAO *pXingHao=pSelLsFamily->RecordAt(i);
		str_arr[0].Format("%f",pXingHao->grade);
		SimplifiedNumString(str_arr[0]);
		str_arr[1].Format("%d",pXingHao->d);
		str_arr[2].Format("%d",pXingHao->valid_len);
		str_arr[3].Format("%d",pXingHao->no_thread_len);
		str_arr[4].Format("%d",pXingHao->down_lim);		
		str_arr[5].Format("%d",pXingHao->up_lim);
		str_arr[6].Format("%f",pXingHao->weight);
		m_xListCtrl.InsertItemRecord(-1,str_arr);
	}
}
void CSysLsFamilyDlg::OnNMDblclkLsFamilyListCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iCurSel=m_xLsFamilyListCtrl.GetSelectedItem();
	if(iCurSel<0)
		return;
	CXhChar16 sValue;
	m_xLsFamilyListCtrl.GetItemText(iCurSel,0,sValue,16);
	if(sValue.Equal("√"))
		m_xLsFamilyListCtrl.SetItemText(iCurSel,0,"");
	else
		m_xLsFamilyListCtrl.SetItemText(iCurSel,0,"√");
	//
	RefreshListCtrl(iCurSel);
	*pResult = 0;
}
void CSysLsFamilyDlg::OnNMClickLsFamilyListCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iCurSel=m_xLsFamilyListCtrl.GetSelectedItem();
	if(iCurSel>=0)
		RefreshListCtrl(iCurSel);
	*pResult = 0;
}
void CSysLsFamilyDlg::OnOK()
{
	//保存匹配好的螺栓系列库
	for(int i=0;i<m_xLsFamilyListCtrl.GetItemCount();i++)
	{
		CXhChar16 sValue;
		m_xLsFamilyListCtrl.GetItemText(i,0,sValue,16);
		if(sValue.Equal("√")==false)
		{
			CLsFamily *pLsFamily=(CLsFamily*)m_xLsFamilyListCtrl.GetItemData(i);
			m_pBoltLib->RemoveFamily(pLsFamily->GetFamilyHandle());
		}
	}
	return CDialog::OnOK();
}
void CSysLsFamilyDlg::OnCancel()
{
	return CDialog::OnCancel();
}