//<LOCALE_TRANSLATE/>
// SelectSimpleSpecDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SelectSimpleSpecDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectSimpleSpecDlg dialog


CSelectSimpleSpecDlg::CSelectSimpleSpecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectSimpleSpecDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectSimpleSpecDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_listCtrl.AddColumnHeader("spec");
	m_listCtrl.AddColumnHeader("occurrence number");
#else
	m_listCtrl.AddColumnHeader("规格");
	m_listCtrl.AddColumnHeader("出现次数");
#endif
	m_listCtrl.m_bEnableSortItems=TRUE;
}


void CSelectSimpleSpecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CTRL, m_listCtrl);
	//{{AFX_DATA_MAP(CSelectSimpleSpecDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectSimpleSpecDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectSimpleSpecDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectSimpleSpecDlg message handlers

BOOL CSelectSimpleSpecDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	long col_wide_arr[2]={120,120};
	m_listCtrl.InitListCtrl(col_wide_arr);
	m_listCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_CHECKBOXES);
	m_listCtrl.SetColumnSortType(0,SORTTYPE_SPEC);
	m_listCtrl.SetColumnSortType(1,SORTTYPE_INTEGER);
	InitDimpartspec();
	UpdatePartList();
	return TRUE; 
}

void CSelectSimpleSpecDlg::UpdatePartList()
{
	CStringArray str_arr;
	str_arr.SetSize(2);
	m_listCtrl.DeleteAllItems();
	CXhChar50 sSpec;
	for(SPEC_MARK *pMark=m_specList.GetFirst();pMark;pMark=m_specList.GetNext())
	{
		str_arr[0].Format("%s",pMark->spec);
		str_arr[1].Format("%d",pMark->N);
		int iItem=m_listCtrl.InsertItemRecord(-1,str_arr);
		m_listCtrl.SetItemData(iItem,(DWORD)pMark);
		for(SPEC_MARK *pTmpMark=m_pSpecList->GetFirst();pTmpMark;pTmpMark=m_pSpecList->GetNext())
			if (strcmp(pMark->spec,pTmpMark->spec)==0)
				m_listCtrl.SetCheck(iItem);
	}
	//默认按出现次数降序排列
	BOOL bAscending=FALSE;
	m_listCtrl.SortItemsBySpecSubItem(1,&bAscending);
}

void CSelectSimpleSpecDlg::OnOK() 
{
	m_pSpecList->Empty();
	char cMark='A';
	for(int i=0;i<m_listCtrl.GetItemCount();i++)
	{
		if(!m_listCtrl.GetCheck(i))
			continue;	
		SPEC_MARK *pMark=(SPEC_MARK*)m_listCtrl.GetItemData(i);
		if(pMark==NULL)
			continue;
		SPEC_MARK *pTmpMark = m_pSpecList->append();
		pTmpMark->N=pMark->N;
		strcpy(pTmpMark->spec,pMark->spec);
		pTmpMark->wide = pMark->wide;
		pTmpMark->thick = pMark->thick;
		pTmpMark->cMark = cMark;
		cMark++;
	}
	CDialog::OnOK();
}
void CSelectSimpleSpecDlg::InitDimpartspec()
{
	CXhChar50 sSpec;
	m_specList.Empty();
	for(CLDSLinePart *pPole=Ta.Parts.GetFirstLinePart();pPole;pPole=Ta.Parts.GetNextLinePart())
	{
		CXhChar16 sPara1,sPara2;
		sPara1.Printf("%f",pPole->size_wide);
		sPara2.Printf("%f",pPole->size_thick);
		SimplifiedNumString(sPara1);
		SimplifiedNumString(sPara2);
		if(pPole->size_idClassType==CLS_LINEANGLE)
			sSpec.Printf("L%sX%s%C",(char*)sPara1,(char*)sPara2,pPole->cMaterial);
		else if(pPole->size_idClassType==CLS_GROUPLINEANGLE)
			sSpec.Printf("%CL%sX%s%C",pPole->size_cSubClassType,(char*)sPara1,(char*)sPara2,pPole->cMaterial);
		else if(pPole->size_idClassType==CLS_LINETUBE)
#ifdef AFX_TARG_ENU_ENGLISH
			sSpec.Printf("Φ%sX%s%C",(char*)sPara1,(char*)sPara2,pPole->cMaterial);
#else
			sSpec.Printf("Φ%sX%s%C",(char*)sPara1,(char*)sPara2,pPole->cMaterial);
#endif
		else if(pPole->size_idClassType==CLS_LINEFLAT)
			sSpec.Printf("-%sX%s%C",(char*)sPara1,(char*)sPara2,pPole->cMaterial);
		else
			continue;
		if(pPole->cMaterial=='S'&&sSpec.GetLength()>0)
			sSpec[sSpec.GetLength()-1]='\0';
		for(SPEC_MARK *pMark=m_specList.GetFirst();pMark;pMark=m_specList.GetNext())
		{
			if(stricmp(sSpec,pMark->spec)==0)
			{
				pMark->N+=1;
				break;
			}
		}
		if(pMark==NULL)
		{
			pMark=m_specList.append();
			pMark->N=1;
			strcpy(pMark->spec,sSpec);
			pMark->wide = pPole->size_wide;
			pMark->thick = pPole->size_thick;
		}
	}
}
