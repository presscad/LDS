//<LOCALE_TRANSLATE BY hxr /> 2015-04-25
// BomCfgDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "stdarx.h"
//#include "MapLds.h"
#include "BomCfgDlg.h"


// CBomCfgDlg �Ի���

IMPLEMENT_DYNAMIC(CBomCfgDlg, CDialog)
CBomCfgDlg::CBomCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBomCfgDlg::IDD, pParent)
{
	//���߱�
#ifdef AFX_TARG_ENU_ENGLISH
	m_listCtrlModule.AddColumnHeader("Module Name");
	m_listCtrlModule.AddColumnHeader("Seg No. Stat.");
	//������ϸ��
	m_listCtrlPart.AddColumnHeader("No.");
	m_listCtrlPart.AddColumnHeader("Material");
	m_listCtrlPart.AddColumnHeader("Spec");
	m_listCtrlPart.AddColumnHeader("Length");
	m_listCtrlPart.AddColumnHeader("Quantity");
	m_listCtrlPart.AddColumnHeader("Piece Weight kg");
	m_listCtrlPart.AddColumnHeader("Total Wight kg");
	m_listCtrlPart.AddColumnHeader("Note");
	//��˨��ϸ��
	m_listCtrlBolt.AddColumnHeader("Grade Spec");
	m_listCtrlBolt.AddColumnHeader("Quantity");
	m_listCtrlBolt.AddColumnHeader("Weight kg");
	m_listCtrlBolt.AddColumnHeader("Type");
	m_listCtrlBolt.AddColumnHeader("Note");
#else
	m_listCtrlModule.AddColumnHeader("��������");
	m_listCtrlModule.AddColumnHeader("�κ�ͳ��");
	//������ϸ��
	m_listCtrlPart.AddColumnHeader("���");
	m_listCtrlPart.AddColumnHeader("����");
	m_listCtrlPart.AddColumnHeader("���");
	m_listCtrlPart.AddColumnHeader("����");
	m_listCtrlPart.AddColumnHeader("����");
	m_listCtrlPart.AddColumnHeader("����kg");
	m_listCtrlPart.AddColumnHeader("����kg");
	m_listCtrlPart.AddColumnHeader("��ע");
	//��˨��ϸ��
	m_listCtrlBolt.AddColumnHeader("������");
	m_listCtrlBolt.AddColumnHeader("����");
	m_listCtrlBolt.AddColumnHeader("����kg");
	m_listCtrlBolt.AddColumnHeader("����");
	m_listCtrlBolt.AddColumnHeader("��ע");
#endif
}

CBomCfgDlg::~CBomCfgDlg()
{
}

void CBomCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBomCfgDlg)
	DDX_Control(pDX, IDC_LIST_MODULE, m_listCtrlModule);
	DDX_Control(pDX, IDC_LIST_PART, m_listCtrlPart);
	DDX_Control(pDX, IDC_LIST_BOLT, m_listCtrlBolt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBomCfgDlg, CDialog)
	ON_BN_CLICKED(IDC_BN_IMPORT_TXT_FILE, OnBnClickedBnImportTxtFile)
	ON_BN_CLICKED(IDC_BN_IMPORT_EXCEL_FILE, OnBnClickedBnImportExcelFile)
	ON_BN_CLICKED(IDC_BN_EMPTY_DATA, &CBomCfgDlg::OnBnClickedBnEmptyData)
END_MESSAGE_MAP()


// CBomCfgDlg ��Ϣ�������
static BOOL FireDeleteItem(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem)
{
	CBomCfgDlg *pCfgDlg=(CBomCfgDlg*)pListCtrl->GetParent();
	if(pCfgDlg==NULL)
		return FALSE;
	LEG_DESIGN *pBody=NULL,*pCurBody=(LEG_DESIGN*)pItem->m_idProp;
	for(pBody=pCfgDlg->m_xBomObj.bodyList.GetFirst();pBody;pBody=pCfgDlg->m_xBomObj.bodyList.GetNext())
	{
		if(pBody==pCurBody)
		{
			pCfgDlg->m_xBomObj.bodyList.DeleteCursor(TRUE);
			return TRUE;
		}
	}
	return FALSE;
}
static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,int iSubItem,CString& sValue)
{
	LEG_DESIGN *pCurBody=(LEG_DESIGN*)pItem->m_idProp;
	if(pCurBody==NULL)
		return FALSE;
	if(iSubItem==0)
		strcpy(pCurBody->sHeight,sValue);
	else if(iSubItem==1)
		strcpy(pCurBody->bodySegStr,sValue);
	return TRUE;
}
BOOL CBomCfgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	long col_wide_arr2[2]={80,280};
	long col_wide_arr8[8]={60,60,60,60,60,60,60,60};
	long col_wide_arr5[5]={150,60,60,60,80};
	m_listCtrlModule.InitImageList(IDR_SUM_MAT_CFG);//-1 
	m_listCtrlModule.InitListCtrl(col_wide_arr2);
	m_listCtrlPart.InitListCtrl(col_wide_arr8);
	m_listCtrlBolt.InitListCtrl(col_wide_arr5);
	m_listCtrlModule.SetDeleteItemFunc(FireDeleteItem);
	m_listCtrlModule.SetModifyValueFunc(FireValueModify);
	InitListCtrl();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣��OCX ����ҳӦ���� FALSE
}

void CBomCfgDlg::OnBnClickedBnImportTxtFile()
{
	CAcModuleResourceOverride resOverride;
#ifdef AFX_TARG_ENU_ENGLISH
	CFileDialog filedlg(TRUE,"bom","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Stat Material Info(*.bom)|*.bom|All Files(*.*)|*.*||");
#else
	CFileDialog filedlg(TRUE,"bom","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"ͳ����Ϣ(*.bom)|*.bom|�����ļ�(*.*)|*.*||");
#endif
	if(filedlg.DoModal()!=IDOK)
		return;
	//BeginWaitCursor();
	CXhChar200 filename("%s",filedlg.GetPathName());
	m_xBomObj.ReadTextBOMFile(filename);
	InitListCtrl();
	//EndWaitCursor();
}

void CBomCfgDlg::OnBnClickedBnImportExcelFile()
{
	CAcModuleResourceOverride resOverride;
#ifdef AFX_TARG_ENU_ENGLISH
	CFileDialog filedlg(TRUE,"xls","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Excel Stat Material Info(*.xls)|*.xls|All Files(*.*)|*.*||");
#else
	CFileDialog filedlg(TRUE,"xls","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Excelͳ����Ϣ(*.xls)|*.xls|�����ļ�(*.*)|*.*||");
#endif
	if(filedlg.DoModal()!=IDOK)
		return;
	//BeginWaitCursor();
	CXhChar200 filename("%s",filedlg.GetPathName());
	m_xBomObj.ReadExcelBOMFile(filename);
	InitListCtrl();
	//EndWaitCursor();
}

void CBomCfgDlg::InitListCtrl(void)
{
	m_listCtrlModule.DeleteAllItems();
	m_listCtrlPart.DeleteAllItems();
	m_listCtrlBolt.DeleteAllItems();
	for(LEG_DESIGN *pBody=m_xBomObj.bodyList.GetFirst();pBody;pBody=m_xBomObj.bodyList.GetNext())
	{
		CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
		lpInfo->SetImage(2);
		lpInfo->SetSubItemText(0,pBody->sHeight,FALSE);
		lpInfo->SetSubItemText(1,(char*)pBody->bodySegStr,FALSE);
		CSuperGridCtrl::CTreeItem* pItem=m_listCtrlModule.InsertRootItem(lpInfo);
		pItem->m_idProp=(long)pBody;
	}
	m_listCtrlModule.Redraw();
	for(CManuPart *pPart=m_xBomObj.Parts.GetFirst();pPart;pPart=m_xBomObj.Parts.GetNext())
	{
		CListCtrlItemInfo* lpInfo = NULL;
		CSuperGridCtrl::CTreeItem* pSegItem=m_listCtrlPart.FindTreeItemById(pPart->nSeg);
		if(pSegItem==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar16 segStr("%d segment",pPart->nSeg);
#else
			CXhChar16 segStr("%d��",pPart->nSeg);
#endif
			lpInfo = new CListCtrlItemInfo();
			lpInfo->SetSubItemText(0,(char*)segStr,TRUE);
			lpInfo->SetImage(1);
			pSegItem=m_listCtrlPart.InsertRootItem(lpInfo);
			pSegItem->m_idProp=pPart->nSeg;
			pSegItem->m_bHideChildren=TRUE;
		}
		lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,pPart->sPartNo,TRUE);
		lpInfo->SetSubItemText(1,pPart->sMaterial,TRUE);
		lpInfo->SetSubItemText(2,pPart->sSpec,TRUE);
		lpInfo->SetSubItemText(3,(char*)CXhChar50(pPart->length),TRUE);
		lpInfo->SetSubItemText(4,(char*)CXhChar50(pPart->nPartNum),TRUE);
		lpInfo->SetSubItemText(5,(char*)CXhChar50(pPart->singWeight),TRUE);
		lpInfo->SetSubItemText(6,(char*)CXhChar50(pPart->sumWeight),TRUE);
		lpInfo->SetSubItemText(7,pPart->sNote,TRUE);
		CSuperGridCtrl::CTreeItem* pItem=m_listCtrlPart.InsertItem(pSegItem,lpInfo);
		pItem->m_idProp=-1;	//������¼
	}
	m_listCtrlPart.Redraw();
	for(CStatBolt *pBolt=m_xBomObj.Bolts.GetFirst();pBolt;pBolt=m_xBomObj.Bolts.GetNext())
	{
		CListCtrlItemInfo* lpInfo = NULL;
		CSuperGridCtrl::CTreeItem* pSegItem=m_listCtrlBolt.FindTreeItemById(pBolt->nSeg);
		if(pSegItem==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar16 segStr("%d segment",pBolt->nSeg);
#else
			CXhChar16 segStr("%d��",pBolt->nSeg);
#endif
			lpInfo = new CListCtrlItemInfo();
			lpInfo->SetSubItemText(0,(char*)segStr,TRUE);
			lpInfo->SetImage(1);
			pSegItem=m_listCtrlBolt.InsertRootItem(lpInfo);
			pSegItem->m_idProp=pBolt->nSeg;
			pSegItem->m_bHideChildren=TRUE;
		}
		lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,pBolt->sSpec,TRUE);	//������
		lpInfo->SetSubItemText(1,(char*)CXhChar50(pBolt->nCount),TRUE);	//����
		lpInfo->SetSubItemText(2,(char*)CXhChar50(pBolt->weight),TRUE);
		CXhChar16 typeStr;
		if(pBolt->type==0)
#ifdef AFX_TARG_ENU_ENGLISH
			typeStr.Copy("Bolt");
		else if(pBolt->type==1)
			typeStr.Copy("Foot nail");
		else if(pBolt->type==2)
			typeStr.Copy("Washer");
		else if(pBolt->type==3)
			typeStr.Copy("Dual head");
#else
			typeStr.Copy("��˨");
		else if(pBolt->type==1)
			typeStr.Copy("�Ŷ�");
		else if(pBolt->type==2)
			typeStr.Copy("��Ȧ");
		else if(pBolt->type==3)
			typeStr.Copy("˫ñ");
#endif
		lpInfo->SetSubItemText(3,(char*)typeStr,TRUE);	//����
		if(!typeStr.Equal(pBolt->sNote))
			lpInfo->SetSubItemText(4,(char*)pBolt->sNote,TRUE);
		CSuperGridCtrl::CTreeItem* pItem=m_listCtrlBolt.InsertItem(pSegItem,lpInfo);
		pItem->m_idProp=-1;	//��˨��¼
	}
	m_listCtrlBolt.Redraw();
}

void CBomCfgDlg::OnBnClickedBnEmptyData()
{
	m_xBomObj.Empty();
	m_listCtrlModule.DeleteAllItems();
	m_listCtrlBolt.DeleteAllItems();
	m_listCtrlPart.DeleteAllItems();
}
