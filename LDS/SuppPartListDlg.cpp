//<LOCALE_TRANSLATE Confirm by hxr/>
// AddPartListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "SuppPartListDlg.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "CfgPartNoDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSuppPartListDlg dialog
#ifndef __TSA_
CSuppPartListDlg::CSuppPartListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSuppPartListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSuppPartListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSuppPartListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSuppPartListDlg)
	DDX_Control(pDX, IDC_LIST_ADDPART, m_AddPartList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSuppPartListDlg, CDialog)
	//{{AFX_MSG_MAP(CSuppPartListDlg)
	ON_BN_CLICKED(IDC_DELETE, OnDeleteDate)
	ON_BN_CLICKED(ID_BTN_CONFIRM, OnConfirm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSuppPartListDlg message handlers
void NewItem(CSuperGridCtrl *pAddPartList)
{
	int num = pAddPartList->GetItemCount();
	CString sItemText="";
	CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
	lpRoot->AddSubItemText(sItemText,TRUE);
	CSuperGridCtrl::CTreeItem* pItem=NULL;
	pItem = pAddPartList->InsertRootItem(lpRoot);
	pItem->m_idProp=num+1;
	int nNumItem = 9;	
	for (int j=0;j<nNumItem;j++)
	{
		lpRoot->SetControlType(3,GCT_CMB_LIST);
#ifdef AFX_TARG_ENU_ENGLISH
		lpRoot->SetListItemsStr(3,"Angle|Tube|Slot|Plate|Other");
#else
		lpRoot->SetListItemsStr(3,"�Ǹ�|�ֹ�|�۸�|�ְ�|����");
#endif
		lpRoot->SetControlType(2,GCT_CMB_LIST);
		lpRoot->SetListItemsStr(2,"Q235|Q345|Q390|Q420|Q460");
		pAddPartList->SetSubItemText(pItem,j,sItemText);
	}
	pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
}


static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	return TRUE;
}

static BOOL FireButtonClick(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem)
{
	return TRUE;
}

static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
							int iSubItem,CString& sTextValue)
{
	CString oldText = pSelItem->m_lpNodeInfo->GetSubItemText(iSubItem);
	if (oldText.CompareNoCase(sTextValue)==0)
		return FALSE;
	int num = pListCtrl->GetItemCount();
	if(iSubItem==0)
	{
		for (int i=0;i<num-1;i++)//�����Լ��Ƚ�Ҳ�������Ŀ����ݱȽ�
		{
			if (i==pSelItem->m_idProp-1)
				continue;
			char sPartNo[100]; 
			pListCtrl->GetItemText(i,0,sPartNo,100);
			if (sTextValue.CompareNoCase(sPartNo)==0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"the number existed already","ERROR",MB_OK);
#else
				MessageBox(NULL,"�˱���Ѵ���","ERROR",MB_OK);
#endif
				CString sText = "";
				pListCtrl->SetSubItemText(pSelItem,iSubItem,sText);
				return FALSE;
			}
		}
		SEGI iSeg;
		if(ParsePartNo(sTextValue.GetBuffer(),&iSeg,NULL))
		{
			CString sSeg=iSeg.ToString();
			pListCtrl->SetSubItemText(pSelItem,1,sSeg);
		}
	}
	else if(iSubItem==3)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(sTextValue.CompareNoCase("Angle")==0)
#else
		if(sTextValue.CompareNoCase("�Ǹ�")==0)
#endif
		{
			int itype; 
			pSelItem->m_lpNodeInfo->GetControlType(4,itype);
			if (itype!=GCT_CMB_LIST)
			{
				pSelItem->m_lpNodeInfo->SetControlType(4,GCT_CMB_LIST);
				CString sGuige = MakeAngleSetString();
				pSelItem->m_lpNodeInfo->SetListItemsStr(4,sGuige);
			}
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(sTextValue.CompareNoCase("Tube")==0)
#else
		else if(sTextValue.CompareNoCase("�ֹ�")==0)
#endif
		{
			int itype; 
			pSelItem->m_lpNodeInfo->GetControlType(4,itype);
			if (itype!=GCT_CMB_LIST)
			{
				pSelItem->m_lpNodeInfo->SetControlType(4,GCT_CMB_LIST);
				CString sGuige = MakeTubeSetString();
				pSelItem->m_lpNodeInfo->SetListItemsStr(4,sGuige);
			}
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(sTextValue.CompareNoCase("Flat")==0)
#else
		else if(sTextValue.CompareNoCase("����")==0)
#endif
		{
			int itype; 
			pSelItem->m_lpNodeInfo->GetControlType(4,itype);
			if (itype!=GCT_CMB_LIST)
			{
				pSelItem->m_lpNodeInfo->SetControlType(4,GCT_CMB_LIST);
				CString sGuige = MakeFlatSetString();
				pSelItem->m_lpNodeInfo->SetListItemsStr(4,sGuige);
			}
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(sTextValue.CompareNoCase("Slot")==0)
#else
		else if(sTextValue.CompareNoCase("�۸�")==0)
#endif
		{
			int itype; 
			pSelItem->m_lpNodeInfo->GetControlType(4,itype);
			if (itype!=GCT_CMB_LIST)
			{
				pSelItem->m_lpNodeInfo->SetControlType(4,GCT_CMB_LIST);
				CString sGuige = MakeSlotSetString();
				pSelItem->m_lpNodeInfo->SetListItemsStr(4,sGuige);
			}
		}
		else
		{
			pSelItem->m_lpNodeInfo->SetControlType(4,GCT_EDIT);
			CString sText = "";
			pListCtrl->SetSubItemText(pSelItem,4,sText);
		}
	}
	pListCtrl->SetSubItemText(pSelItem,iSubItem,sTextValue);
	if (pSelItem->m_idProp==num && sTextValue!="")
		NewItem(pListCtrl);
	return TRUE;
}

BOOL CSuppPartListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
#ifdef AFX_TARG_ENU_ENGLISH
	m_AddPartList.AddColumnHeader("number");
	m_AddPartList.AddColumnHeader("segment number");
	m_AddPartList.AddColumnHeader("material");
	m_AddPartList.AddColumnHeader("type");
	m_AddPartList.AddColumnHeader("size");
	m_AddPartList.AddColumnHeader("length");
	m_AddPartList.AddColumnHeader("amount");
	m_AddPartList.AddColumnHeader("piece weight");
	m_AddPartList.AddColumnHeader("notes");
#else
	m_AddPartList.AddColumnHeader("���");
	m_AddPartList.AddColumnHeader("�ֶκ�");
	m_AddPartList.AddColumnHeader("����");
	m_AddPartList.AddColumnHeader("����");
	m_AddPartList.AddColumnHeader("���");
	m_AddPartList.AddColumnHeader("����");
	m_AddPartList.AddColumnHeader("����");
	m_AddPartList.AddColumnHeader("����");
	m_AddPartList.AddColumnHeader("��ע");
#endif
	
	long col_wide_arr[10]={60,60,60,60,70,50,60,60,92};
	m_AddPartList.InitListCtrl(col_wide_arr);
	m_AddPartList.SetContextMenuFunc(FireContextMenu);
	m_AddPartList.SetModifyValueFunc(FireValueModify);
	m_AddPartList.SetButtonClickFunc(FireButtonClick);
	int j=0;
	for(CSupplementPart *pSuppPart=Ta.SuppPart.GetFirst();pSuppPart;pSuppPart=Ta.SuppPart.GetNext())
	{
		int num = m_AddPartList.GetItemCount();
		CString sItemText="";
		CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
		lpRoot->AddSubItemText(sItemText,TRUE);
		CSuperGridCtrl::CTreeItem* pItem=NULL;
		pItem = m_AddPartList.InsertRootItem(lpRoot);
		pItem->m_idProp=num+1;
		//���
		sItemText.Format("%s",(char*)pSuppPart->GetPartNo());
		m_AddPartList.SetSubItemText(pItem,0,sItemText);
		//�ֶκ�
		sItemText.Format("%s",(char*)pSuppPart->iSeg.ToString());
		m_AddPartList.SetSubItemText(pItem,1,sItemText);
		//����
		char sTemp[10];
		QuerySteelMatMark(pSuppPart->cMaterial,sTemp);
		sItemText = sTemp;
		m_AddPartList.SetSubItemText(pItem,2,sItemText);
		//����
		if(pSuppPart->part_type==STEELTYPE_ANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			sItemText = "Angle";
		else if(pSuppPart->part_type==STEELTYPE_PLATE)
			sItemText = "Plate";
		else if(pSuppPart->part_type==STEELTYPE_TUBE)
			sItemText = "Tube";
		else if(pSuppPart->part_type==STEELTYPE_FLAT)
			sItemText = "Flat";
		else if(pSuppPart->part_type==STEELTYPE_SLOT)
			sItemText = "Slot";
		else
			sItemText = "Other";
#else
			sItemText = "�Ǹ�";
		else if(pSuppPart->part_type==STEELTYPE_PLATE)
			sItemText = "�ְ�";
		else if(pSuppPart->part_type==STEELTYPE_TUBE)
			sItemText = "�ֹ�";
		else if(pSuppPart->part_type==STEELTYPE_FLAT)
			sItemText = "����";
		else if(pSuppPart->part_type==STEELTYPE_SLOT)
			sItemText = "�۸�";
		else
			sItemText = "����";
#endif
		m_AddPartList.SetSubItemText(pItem,3,sItemText);
		//���
		sItemText.Format("%s",(char*)pSuppPart->guige);
		m_AddPartList.SetSubItemText(pItem,4,sItemText);
		//����
		sItemText.Format("%f",pSuppPart->length);
		SimplifiedNumString(sItemText);
		m_AddPartList.SetSubItemText(pItem,5,sItemText);
		//����
		sItemText.Format("%d",pSuppPart->nQuantity);
		m_AddPartList.SetSubItemText(pItem,6,sItemText);
		//����
		sItemText.Format("%f",pSuppPart->piece_weight);
		SimplifiedNumString(sItemText);
		m_AddPartList.SetSubItemText(pItem,7,sItemText);
		//��ע
		sItemText.Format("%s",pSuppPart->sNotes);
		m_AddPartList.SetSubItemText(pItem,8,sItemText);
		lpRoot->SetControlType(3,GCT_CMB_LIST);
#ifdef AFX_TARG_ENU_ENGLISH
		lpRoot->SetListItemsStr(3,"Angle|Tube|Flat|Slot|Plate|Other");
#else
		lpRoot->SetListItemsStr(3,"�Ǹ�|�ֹ�|����|�۸�|�ְ�|����");
#endif
		lpRoot->SetControlType(2,GCT_CMB_LIST);
		lpRoot->SetListItemsStr(2,"Q235|Q345|Q390|Q420|Q460");
		pItem->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
		j++;
	}
	NewItem(&m_AddPartList);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSuppPartListDlg::OnDeleteDate() 
{
	int iCurItem = m_AddPartList.GetSelectedItem();
	int nSumItem = m_AddPartList.GetItemCount();
	if(nSumItem<2||nSumItem==iCurItem+1)
		return;
	for (int i = iCurItem;i<nSumItem;i++)
	{
		CSuperGridCtrl::CTreeItem* pItem = m_AddPartList.GetTreeItem(i);
		pItem->m_idProp-=1;
	}
	m_AddPartList.DeleteItem(iCurItem);
	m_AddPartList.UpdateData();
}

void CSuppPartListDlg::OnOK() 
{
	//���λس�
}

void CSuppPartListDlg::OnConfirm() 
{
	Ta.SuppPart.Empty();
	int sumItem = m_AddPartList.GetItemCount();
	for (int i=0;i<sumItem-1;i++)
	{
		char sItemText[50]="";
		CSupplementPart* pSuppPart = Ta.SuppPart.append();
		//���
		m_AddPartList.GetItemText(i,0,sItemText,50);
		pSuppPart->SetPartNo(sItemText);
		//�ֶκ�
		m_AddPartList.GetItemText(i,1,sItemText,50);
		pSuppPart->iSeg=SEGI(sItemText);
		//����
		m_AddPartList.GetItemText(i,2,sItemText,50);
		pSuppPart->cMaterial=QuerySteelBriefMatMark(sItemText);
		//����
		m_AddPartList.GetItemText(i,3,sItemText,50);
#ifdef AFX_TARG_ENU_ENGLISH
		if(strcmp(sItemText,"Angle")==0)
			pSuppPart->part_type=STEELTYPE_ANGLE;
		else if(strcmp(sItemText,"Plate")==0)
			pSuppPart->part_type=STEELTYPE_PLATE;
		else if(strcmp(sItemText,"Tube")==0)
			pSuppPart->part_type=STEELTYPE_TUBE;
		else if(strcmp(sItemText,"Flat")==0)
			pSuppPart->part_type=STEELTYPE_FLAT;
		else if(strcmp(sItemText,"Slot")==0)
#else
		if(strcmp(sItemText,"�Ǹ�")==0)
			pSuppPart->part_type=STEELTYPE_ANGLE;
		else if(strcmp(sItemText,"�ְ�")==0)
			pSuppPart->part_type=STEELTYPE_PLATE;
		else if(strcmp(sItemText,"�ֹ�")==0)
			pSuppPart->part_type=STEELTYPE_TUBE;
		else if(strcmp(sItemText,"����")==0)
			pSuppPart->part_type=STEELTYPE_FLAT;
		else if(strcmp(sItemText,"�۸�")==0)
#endif
			pSuppPart->part_type=STEELTYPE_SLOT;
		else
			pSuppPart->part_type=TA_OTHER;
		//���
		m_AddPartList.GetItemText(i,4,sItemText,50);
		pSuppPart->guige.Copy(sItemText);
		//����
		m_AddPartList.GetItemText(i,5,sItemText,50);
		pSuppPart->length=atof(sItemText);
		//����
		m_AddPartList.GetItemText(i,6,sItemText,50);
		pSuppPart->nQuantity = atoi(sItemText);
		pSuppPart->m_uStatMatCoef = pSuppPart->nQuantity;
		//����
		m_AddPartList.GetItemText(i,7,sItemText,50);
		pSuppPart->piece_weight = atof(sItemText);
		//��ע
		m_AddPartList.GetItemText(i,8,sItemText,50);
		sprintf(pSuppPart->sNotes,"%s",sItemText);
	}
	CDialog::OnOK();
}
#endif