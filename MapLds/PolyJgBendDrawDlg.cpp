// PolyJgBendDrawDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PolyJgBendDrawDlg.h"
#include "ParseAdaptNo.h"

// CPolyJgBendDrawDlg 对话框

IMPLEMENT_DYNAMIC(CPolyJgBendDrawDlg, CDialog)

CPolyJgBendDrawDlg::CPolyJgBendDrawDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPolyJgBendDrawDlg::IDD, pParent)
{
	m_fDrawScale=1;
	m_fMinDist=10;
	m_sSegStr = _T("*");
	m_sPartNo = _T("*");
}

CPolyJgBendDrawDlg::~CPolyJgBendDrawDlg()
{
}

void CPolyJgBendDrawDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_SEG_STR, m_sSegStr);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_DRAW_SCALE, m_fDrawScale);
	DDX_Text(pDX, IDC_E_MIN_DISTANCE, m_fMinDist);
	DDX_Control(pDX,IDC_LIST_CTRL, m_xListCtrl);
	DDX_Text(pDX,IDC_BN_CREATE_SELECTED_ITEMS,m_sCreate);
}


BEGIN_MESSAGE_MAP(CPolyJgBendDrawDlg, CDialog)
	ON_BN_CLICKED(IDC_BN_REBUILD_PARTSET, OnBnRebuildPartSet)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CTRL, OnClickListCtrl)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CTRL, OnDblclkListCtrl)
	ON_BN_CLICKED(IDC_BN_CREATE_SELECTED_ITEMS, OnBnClickedConfirm)
END_MESSAGE_MAP()


// CPolyJgBendDrawDlg 消息处理程序
BOOL CPolyJgBendDrawDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//
	m_sCreate="生成所有构件图";
	m_xListCtrl.AddColumnHeader("件号",60);
	m_xListCtrl.AddColumnHeader("段号",60);
	m_xListCtrl.AddColumnHeader("规格",60);
	m_xListCtrl.AddColumnHeader("件数",60);
	m_xListCtrl.AddColumnHeader("描述",80);
	m_xListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	m_xListCtrl.InitListCtrl();
	m_xListCtrl.m_bEnableSortItems=TRUE;
	RefreshListCtr();
	UpdateData(FALSE);
	return TRUE;
}
void CPolyJgBendDrawDlg::RefreshListCtr()
{
	CHashList<SEGI>segTbl;
	GetSegNoHashTblBySegStr(m_sSegStr,segTbl);
	polyPartList.Empty();
	Ta.RetrievePolyPartList(polyPartList);
	m_polyHashByPartNo.Empty();
	m_xListCtrl.DeleteAllItems();
	for(CLDSPolyPart *pPolyPart=polyPartList.GetFirst();pPolyPart;pPolyPart=polyPartList.GetNext())
	{
		CLDSPart *pSonPart=pPolyPart->segset.GetFirst();
		if(pSonPart->GetClassTypeId()!=CLS_LINEANGLE)
			continue;
		CLDSLineAngle* pSonJg=(CLDSLineAngle*)pSonPart;
		CXhChar16 sPartNo=pSonPart->GetPartNo();
		if(strlen(sPartNo)==0)
			continue;	//
		if(!(strstr(sPartNo,m_sPartNo)!=NULL||m_sPartNo=="*"||m_sPartNo.IsEmpty()))
			continue;	//在逐个生成构件图时 如果不是指定构件编号则继续
		if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pSonPart->iSeg.iSeg)==NULL)
			continue;	//分段号不在当前所选范围内
		if(m_polyHashByPartNo.GetValue(sPartNo))
		{
			m_polyHashByPartNo.SetValue(sPartNo,(DWORD)pPolyPart);
			continue;
		}
		CXhChar50 sGuige,sProcess;
		pSonJg->GetSpecification(sGuige,FALSE);
		pSonJg->GetProcessStr(sProcess,50);
		CStringArray str_arr;
		str_arr.SetSize(5);
		str_arr[0].Format("%s",(char*)sPartNo);
		str_arr[1].Format("%s",(char*)pSonJg->iSeg.ToString());
		str_arr[2].Format("%s",(char*)sGuige);
		str_arr[3]="1";
		str_arr[4].Format("%s",(char*)sProcess);
		int iCur=m_xListCtrl.InsertItemRecord(-1,str_arr);
		m_xListCtrl.SetItemData(iCur,(DWORD)pPolyPart);
	}
}

void CPolyJgBendDrawDlg::OnBnRebuildPartSet() 
{
	UpdateData();
	RefreshListCtr();
	UpdateData(FALSE);
}

void CPolyJgBendDrawDlg::ConfirmCreate(bool bOnlySelecteItems)
{
	partSet.Empty();
	CLDSPolyPart* pPolyPart=NULL;
	if(bOnlySelecteItems)
	{	//生成选中项
		POSITION pos = m_xListCtrl.GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			int i=m_xListCtrl.GetNextSelectedItem(pos);
			pPolyPart=(CLDSPolyPart*)m_xListCtrl.GetItemData(i);
			partSet.append(pPolyPart);
		}
	}
	else
	{	//全选
		for(int i=0;i<m_xListCtrl.GetItemCount();i++)
		{
			pPolyPart=(CLDSPolyPart*)m_xListCtrl.GetItemData(i);
			partSet.append(pPolyPart);
		}
	}
	UpdateData();
	CDialog::OnOK();
}

void CPolyJgBendDrawDlg::OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_xListCtrl.GetSelectedCount()==1)
		ConfirmCreate(true);
	*pResult = 0;
}

void CPolyJgBendDrawDlg::OnClickListCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData();
	if(m_xListCtrl.GetSelectedCount()==0)
		m_sCreate="生成所有构件图";
	else
		m_sCreate="生成选中构件图";
	UpdateData(FALSE);
	*pResult = 0;
}

void CPolyJgBendDrawDlg::OnBnClickedConfirm()
{
	if(m_xListCtrl.GetSelectedCount()==0)
		ConfirmCreate(false);
	else
		ConfirmCreate(true);
}