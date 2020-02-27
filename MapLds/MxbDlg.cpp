// MxbDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MxbDlg.h"
#include "SysPara.h"
#include "DrawTaMap.h"
#include "ParseAdaptNo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMxbDlg dialog


CMxbDlg::CMxbDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMxbDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMxbDlg)
	m_nLocatePos = 1;
	m_sSegNo = _T("1");
	//}}AFX_DATA_INIT
}


void CMxbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMxbDlg)
	DDX_Radio(pDX, IDC_R_LOCATE_POS, m_nLocatePos);
	DDX_CBString(pDX, IDC_CMB_SEG_NO, m_sSegNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMxbDlg, CDialog)
	//{{AFX_MSG_MAP(CMxbDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMxbDlg message handlers
BOOL CMxbDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
#ifndef __TSA_FILE_
	//初始化段号下拉框
	GetDlgItem(IDC_CMB_SEG_NO)->EnableWindow(sys.m_iCurMapType==3);
	if(sys.m_iCurMapType ==3)
	{	//出分段明细表
		ATOM_LIST<SEGI> segNoList;
		int nSegNum=Ta.GetSegNoList(segNoList);
		if(nSegNum>0)
		{	//
			CString sItem;
			CComboBox* pCMB = (CComboBox*)GetDlgItem(IDC_CMB_SEG_NO);
			for(SEGI *pSegI=segNoList.GetFirst();pSegI;pSegI=segNoList.GetNext())
			{
				sItem.Format("%s",(char*)pSegI->ToString());
				pCMB->AddString(sItem);	
				if(Ta.m_iCurSeg.iSeg<=0)
					Ta.m_iCurSeg=*pSegI;
			}
		}
		m_sSegNo.Format("%s",(char*)Ta.m_iCurSeg.ToString());
		UpdateData(FALSE);
	}
#else
	GetDlgItem(IDC_CMB_SEG_NO)->EnableWindow(FALSE);
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
}
void CMxbDlg::OnOK() 
{
	UpdateData();
#ifndef __TSA_FILE_
	//判断输入的段号是否合法
	if(sys.m_iCurMapType ==3)
	{
		m_xSegNoList.Empty();
		if(m_sSegNo[0]=='*')
			Ta.GetSegNoList(m_xSegNoList);
		else
		{
			CHashList<SEGI> segTbl;
			GetSegNoHashTblBySegStr(m_sSegNo,segTbl);
			for(SEGI *pSegI=segTbl.GetFirst();pSegI;pSegI=segTbl.GetNext())
				m_xSegNoList.append(*pSegI);
		}
		if(m_xSegNoList.GetNodeNum()<=0)
		{
			Ta.m_iCurSeg = SEGI(1);
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("the segment number is illegal,please input again!");
#else
			AfxMessageBox("输入了非法段号,请重新输入!");
#endif
			GetDlgItem(IDC_CMB_SEG_NO)->SetFocus();
			return;
		}
	}
#endif
	CDialog::OnOK();
}