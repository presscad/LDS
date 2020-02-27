//<LOCALE_TRANSLATE/>
// StdPartSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StdPartSetDlg.h"
#include "database.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef __TSA_FILE_
/////////////////////////////////////////////////////////////////////////////
// CStdPartSetDlg dialog


CStdPartSetDlg::CStdPartSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStdPartSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStdPartSetDlg)
	m_sLibFilePath = _T("");
	m_sPrompt = _T("");
	m_iRollType=0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sLibName = _T("standard part library:");
#else
	m_sLibName = _T("��׼������:");
#endif
	m_sCurPartName = _T("");
	//}}AFX_DATA_INIT
	m_iStdPartType=-1;
#ifdef AFX_TARG_ENU_ENGLISH
	m_listCtrl.AddColumnHeader("part No.");
	m_listCtrl.AddColumnHeader("description");
#else
	m_listCtrl.AddColumnHeader("�������");
	m_listCtrl.AddColumnHeader("����");
#endif
	m_listCtrl.m_bEnableSortItems=TRUE;
}


void CStdPartSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStdPartSetDlg)
	DDX_Control(pDX, IDC_LIST_CTRL, m_listCtrl);
	DDX_Text(pDX, IDC_E_LIB_FILE_PATH, m_sLibFilePath);
	DDX_Text(pDX, IDC_S_PROMPT, m_sPrompt);
	DDX_Text(pDX, IDC_S_LIB_NAME, m_sLibName);
	DDX_Text(pDX, IDC_S_CUR_PART, m_sCurPartName);
	DDX_CBIndex(pDX, IDC_CMB_ROLL_TYPE, m_iRollType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStdPartSetDlg, CDialog)
	//{{AFX_MSG_MAP(CStdPartSetDlg)
	ON_BN_CLICKED(IDC_BTN_BROWSE_FILE, OnBrowseFile)
	ON_BN_CLICKED(IDC_BTN_BATCH_CREATE, OnBatchCreate)
	ON_BN_CLICKED(IDC_BTN_CREATE_CUR_PART, OnCreateCurPart)
	ON_EN_CHANGE(IDC_E_LIB_FILE_PATH, OnChangeLibFilePath)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_CTRL, OnKeydownListCtrl)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CTRL, OnItemchangedListCtrl)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CTRL, OnDblclkListCtrl)
	ON_CBN_SELCHANGE(IDC_CMB_ROLL_TYPE, OnCbnSelchangeCmbRollType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStdPartSetDlg message handlers

void CStdPartSetDlg::OnBrowseFile() 
{
	UpdateData();
	CFileDialog dlg(TRUE,"cipl","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"trough board library(*.cipl)|*.cipl|U-shape board library(*.uipl)|*.uipl|cross board library(*.xipl)|*.xipl|weld neck flange library(*.fld)|*.fld|flat?welding?flange library(*.flp)|*.flp|rigid flange library(*.flg)|*.flg|flexible flange library(*.flr)|*.flr|");
#else
		"���Ͳ���(*.cipl)|*.cipl|U�Ͳ���(*.uipl)|*.uipl|ʮ�ֲ���(*.xipl)|*.xipl|�Ժ�������(*.fld)|*.fld|ƽ��������(*.flp)|*.flp|���Է�����(*.flg)|*.flg|���Է�����(*.flr)|*.flr|");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	m_sLibFilePath=dlg.GetPathName();
	UpdatePartList();
	UpdateData(FALSE);
}

void CStdPartSetDlg::OnBatchCreate() 
{
	//����Ļ��ǰ��ʾ����
	partList.Empty();
	int i,nCount=m_listCtrl.GetItemCount();
	for(i=0;i<nCount;i++)
	{
		int iCurIndex=m_listCtrl.GetItemData(i);
		if(m_iStdPartType==0&&iCurIndex>=0&&iCurIndex<rollend_param_N)
		{
			STDPART_ITEM *pItem=partList.append();
			pItem->iStdType=TYPE_ROLLEND;
			pItem->pInsertPlate=&rollend_param_table[iCurIndex];
		}
		else if(m_iStdPartType==1&&iCurIndex>=0&&iCurIndex<uend_param_N)
		{
			STDPART_ITEM *pItem=partList.append();
			pItem->iStdType=TYPE_UEND;
			pItem->pInsertPlate=&uend_param_table[iCurIndex];
		}
		else if(m_iStdPartType==2&&iCurIndex>=0&&iCurIndex<xend_param_N)
		{
			STDPART_ITEM *pItem=partList.append();
			pItem->iStdType=TYPE_XEND;
			pItem->pInsertPlate=&xend_param_table[iCurIndex];
		}
		else if(m_iStdPartType==3&&iCurIndex>=0&&iCurIndex<fld_param_N)
		{
			STDPART_ITEM *pItem=partList.append();
			pItem->iStdType=TYPE_FLD;
			pItem->pFlParam=&fld_param_table[iCurIndex];
		}
		else if(m_iStdPartType==4&&iCurIndex>=0&&iCurIndex<flp_param_N)
		{
			STDPART_ITEM *pItem=partList.append();
			pItem->iStdType=TYPE_FLP;
			pItem->pFlParam=&flp_param_table[iCurIndex];
		}
	}
	CDialog::OnOK();
}

void CStdPartSetDlg::OnCreateCurPart() 
{
	//����Ļ��ǰ��ʾ����
	partList.Empty();
	POSITION pos=m_listCtrl.GetFirstSelectedItemPosition();
	if(pos==NULL)
		CDialog::OnOK();
	int iCurSel=m_listCtrl.GetNextSelectedItem(pos);
	if(iCurSel<0)
		CDialog::OnOK();
	int iCurIndex=m_listCtrl.GetItemData(iCurSel);
	if(m_iStdPartType==0&&iCurIndex>=0&&iCurIndex<rollend_param_N)
	{
		STDPART_ITEM *pItem=partList.append();
		pItem->iStdType=TYPE_ROLLEND;
		pItem->pInsertPlate=&rollend_param_table[iCurIndex];
	}
	else if(m_iStdPartType==1&&iCurIndex>=0&&iCurIndex<uend_param_N)
	{
		STDPART_ITEM *pItem=partList.append();
		pItem->iStdType=TYPE_UEND;
		pItem->pInsertPlate=&uend_param_table[iCurIndex];
	}
	else if(m_iStdPartType==2&&iCurIndex>=0&&iCurIndex<xend_param_N)
	{
		STDPART_ITEM *pItem=partList.append();
		pItem->iStdType=TYPE_XEND;
		pItem->pInsertPlate=&xend_param_table[iCurIndex];
	}
	else if(m_iStdPartType==3&&iCurIndex>=0&&iCurIndex<fld_param_N)
	{
		STDPART_ITEM *pItem=partList.append();
		pItem->iStdType=TYPE_FLD;
		pItem->pFlParam=&fld_param_table[iCurIndex];
	}
	else if(m_iStdPartType==4&&iCurIndex>=0&&iCurIndex<flp_param_N)
	{
		STDPART_ITEM *pItem=partList.append();
		pItem->iStdType=TYPE_FLP;
		pItem->pFlParam=&flp_param_table[iCurIndex];
	}
	else if(m_iStdPartType==5&&iCurIndex>=0&&iCurIndex<flg_param_N)
	{
		STDPART_ITEM *pItem=partList.append();
		pItem->iStdType=TYPE_FLG;
		pItem->pFlParam2=&flg_param_tbl[iCurIndex];
	}
	else if(m_iStdPartType==6&&iCurIndex>=0&&iCurIndex<flr_param_N)
	{
		STDPART_ITEM *pItem=partList.append();
		pItem->iStdType=TYPE_FLR;
		pItem->pFlParam2=&flr_param_tbl[iCurIndex];
	}
	CDialog::OnOK();
}
void CStdPartSetDlg::UpdatePartList()
{
	int nLen=m_sLibFilePath.GetLength();
	if(nLen<=0)
		return;
	int iStart=m_sLibFilePath.ReverseFind('.');
	if(iStart<0||iStart>nLen)
		return;
	int i=0;
	CStringArray str_arr;
	str_arr.SetSize(2);
	m_listCtrl.DeleteAllItems();
	CString sExtName=m_sLibFilePath.Mid(iStart+1,nLen-iStart);
	if(sExtName.CompareNoCase("cipl")==0)
	{
		m_iStdPartType=0;
#ifdef AFX_TARG_ENU_ENGLISH
		m_sLibName.Format("trough board library:");
#else
		m_sLibName.Format("���Ͳ���:");
#endif
		CInsertPlateSerialize(m_sLibFilePath,FALSE);
		for(i=0;i<rollend_param_N;i++)
		{
			str_arr[0].Format("%s",rollend_param_table[i].codeName);
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[1]="trough board";
#else
			str_arr[1]="���Ͳ��";
#endif
			int iItem=m_listCtrl.InsertItemRecord(-1,str_arr);
			m_listCtrl.SetItemData(iItem,i);
		}
	}
	else if(sExtName.CompareNoCase("uipl")==0)
	{
		m_iStdPartType=1;
		UInsertPlateSerialize(m_sLibFilePath,FALSE);
		for(i=0;i<uend_param_N;i++)
		{
			str_arr[0].Format("%s",uend_param_table[i].codeName);
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[1]="U-shape board";
#else
			str_arr[1]="U�Ͳ��";
#endif
			int iItem=m_listCtrl.InsertItemRecord(-1,str_arr);
			m_listCtrl.SetItemData(iItem,i);
		}
	}
	else if(sExtName.CompareNoCase("xipl")==0)
	{
		m_iStdPartType=2;
		XInsertPlateSerialize(m_sLibFilePath,FALSE);
		for(i=0;i<xend_param_N;i++)
		{
			str_arr[0].Format("%s",xend_param_table[i].codeName);
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[1]="cross board";
#else
			str_arr[1]="ʮ�ֲ��";
#endif
			int iItem=m_listCtrl.InsertItemRecord(-1,str_arr);
			m_listCtrl.SetItemData(iItem,i);
		}
	}
	else if(sExtName.CompareNoCase("fld")==0)
	{
		m_iStdPartType=3;
		FlDSerialize(m_sLibFilePath,FALSE);
		for(i=0;i<fld_param_N;i++)
		{
			str_arr[0].Format("%s",fld_param_table[i].codeName);
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[1]="weld neck flange";
#else
			str_arr[1]="�Ժ�����";
#endif
			int iItem=m_listCtrl.InsertItemRecord(-1,str_arr);
			m_listCtrl.SetItemData(iItem,i);
		}
	}
	else if(sExtName.CompareNoCase("flp")==0)
	{
		m_iStdPartType=4;
		FlPSerialize(m_sLibFilePath,FALSE);
		for(i=0;i<flp_param_N;i++)
		{
			str_arr[0].Format("%s",flp_param_table[i].codeName);
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[1]="flat welding flange";
#else
			str_arr[1]="ƽ������";
#endif
			int iItem=m_listCtrl.InsertItemRecord(-1,str_arr);
			m_listCtrl.SetItemData(iItem,i);
		}
	}
	else if(sExtName.CompareNoCase("flg")==0)
	{
		m_iStdPartType=5;
		FlGSerialize(m_sLibFilePath,FALSE);
		for(i=0;i<flg_param_N;i++)
		{
			str_arr[0].Format("%s",flg_param_tbl[i].codeName);
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[1]="rigid flange";
#else
			str_arr[1]="���Է���";
#endif
			int iItem=m_listCtrl.InsertItemRecord(-1,str_arr);
			m_listCtrl.SetItemData(iItem,i);
		}
	}
	else if(sExtName.CompareNoCase("flr")==0)
	{
		m_iStdPartType=6;
		FlRSerialize(m_sLibFilePath,FALSE);
		for(i=0;i<flr_param_N;i++)
		{
			str_arr[0].Format("%s",flr_param_tbl[i].codeName);
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[1]="flexible flange";
#else
			str_arr[1]="���Է���";
#endif
			int iItem=m_listCtrl.InsertItemRecord(-1,str_arr);
			m_listCtrl.SetItemData(iItem,i);
		}
	}
	else
		return;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrompt.Format("There are %d record in total.",m_listCtrl.GetItemCount());
#else
	m_sPrompt.Format("��%d����¼",m_listCtrl.GetItemCount());
#endif
	POSITION pos=m_listCtrl.GetFirstSelectedItemPosition();
	if(pos)
	{
		int iCurSel=m_listCtrl.GetNextSelectedItem(pos);
		if(iCurSel>=0)
		m_sCurPartName=m_listCtrl.GetItemText(iCurSel,0);
	}
	if(m_iStdPartType==0)
	{
		GetDlgItem(IDC_S_ROLL_TYPE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CMB_ROLL_TYPE)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_S_ROLL_TYPE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CMB_ROLL_TYPE)->ShowWindow(SW_HIDE);
	}
	UpdateData(FALSE);
}

void CStdPartSetDlg::OnChangeLibFilePath() 
{
	UpdateData();
	UpdatePartList();
	UpdateData(FALSE);
}

BOOL CStdPartSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
#ifdef AFX_TARG_ENU_ENGLISH
	long col_wide_arr[4]={80,95};
#else 
	long col_wide_arr[4]={80,80};
#endif
	m_listCtrl.InitListCtrl(col_wide_arr);
	//
	CComboBox* pCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_ROLL_TYPE);
	pCmbBox->AddString("0.�������");
	pCmbBox->AddString("1.���ٵ���1");
	pCmbBox->AddString("2.���ٵ���2");
	pCmbBox->SetCurSel(0);
	GetDlgItem(IDC_S_ROLL_TYPE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CMB_ROLL_TYPE)->ShowWindow(SW_HIDE);
	//
	UpdatePartList();
	return TRUE; 
}

void CStdPartSetDlg::OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	if(pLVKeyDow->wVKey==VK_DELETE)
	{
		POSITION pos=m_listCtrl.GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			int iCurSel=m_listCtrl.GetNextSelectedItem(pos);
			m_listCtrl.DeleteItem(iCurSel);
			pos=m_listCtrl.GetFirstSelectedItemPosition();
		}
	}
	m_sCurPartName=_T("");
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrompt.Format("There are %d record in total.",m_listCtrl.GetItemCount());
#else
	m_sPrompt.Format("��%d����¼",m_listCtrl.GetItemCount());
#endif
	UpdateData(FALSE);
	*pResult = 0;
}

void CStdPartSetDlg::OnItemchangedListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	POSITION pos=m_listCtrl.GetFirstSelectedItemPosition();
	if(pos)
	{
		int iCurSel=m_listCtrl.GetNextSelectedItem(pos);
		if(iCurSel>=0)
			m_sCurPartName=m_listCtrl.GetItemText(iCurSel,0);
	}
	UpdateData(FALSE);
	*pResult = 0;
}

void CStdPartSetDlg::OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnCreateCurPart();	
	*pResult = 0;
}

void CStdPartSetDlg::OnCbnSelchangeCmbRollType()
{
	UpdateData();
	CComboBox* pCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_ROLL_TYPE);
	m_iRollType=pCmbBox->GetCurSel();
	UpdateData(FALSE);
}
#endif
