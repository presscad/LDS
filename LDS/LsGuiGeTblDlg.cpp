// LsGuiGeTblDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "LsGuiGeTblDlg.h"
#include "database.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLsGuiGeTblDlg dialog


CLsGuiGeTblDlg::CLsGuiGeTblDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLsGuiGeTblDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLsGuiGeTblDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	nColumnsN = 7;
	nRowsN = nLsXingHaoCount+1;
}


void CLsGuiGeTblDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLsGuiGeTblDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_GridControl(pDX, IDC_JG_GUIGE_GRID, m_Grid);
}


BEGIN_MESSAGE_MAP(CLsGuiGeTblDlg, CDialog)
	//{{AFX_MSG_MAP(CLsGuiGeTblDlg)
	ON_BN_CLICKED(IDC_BN_EXPORT_LS_FILE, OnBnExportLsFile)
	ON_BN_CLICKED(IDC_BN_IMPORT_LS_FILE, OnBnImportLsFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLsGuiGeTblDlg message handlers

BOOL CLsGuiGeTblDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Grid.SetListMode(TRUE);
	m_Grid.SetHeaderSort(FALSE);
	
	m_Grid.SetEditable(TRUE);
	m_Grid.SetTextBkColor(RGB(0xFF, 0xFF, 0xFF));

	try {
		m_Grid.SetRowCount(nRowsN);
		m_Grid.SetColumnCount(nColumnsN);
		m_Grid.SetFixedRowCount(1);
		m_Grid.SetFixedColumnCount(3);
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	// fill rows/cols with text
	for (int iRow=0;iRow<m_Grid.GetRowCount();iRow++)
	{
		for (int iCol= 0;iCol<m_Grid.GetColumnCount();iCol++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = iRow;
			Item.col = iCol;
			if (iRow<1)
			{
				Item.nFormat = DT_RIGHT|DT_WORDBREAK;
				if(iCol==0)
					Item.szText.Format(_T("级别"));
				else if(iCol==1)
					Item.szText.Format(_T("直径"));
				else if(iCol==2)
					Item.szText.Format(_T("有效长"));
				else if(iCol==3)
					Item.szText.Format(_T("无扣长"));
				else if(iCol==4)
					Item.szText.Format(_T("通厚下限"));
				else if(iCol==5)
					Item.szText.Format(_T("通厚上限"));
				else //if(iCol==6)
					Item.szText.Format(_T("理论重量"));
			}
			else
			{
				if(iCol==0)	//级别
				{
					Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
					Item.szText.Format(_T("%.1f"),lsguige_table[iRow-1].grade);
				}
				else if(iCol==1)	//直径
				{
					Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
					Item.szText.Format(_T("%d"),lsguige_table[iRow-1].d);
				}
				else if(iCol==2)	//有效长
				{
					Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
					Item.szText.Format(_T("%d"),lsguige_table[iRow-1].valid_len);
				}
				else if(iCol==3)	//无扣长
				{
					Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
					Item.szText.Format(_T("%d"),lsguige_table[iRow-1].no_thread_len);
				}
				else if(iCol==4)	//通厚下限
				{
					Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
					Item.szText.Format(_T("%d"),lsguige_table[iRow-1].down_lim);
				}
				else if(iCol==5)	//通厚上限
				{
					Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
					Item.szText.Format(_T("%d"),lsguige_table[iRow-1].up_lim);
				}
				else				//理论重量
				{
					Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
					Item.szText.Format(_T("%lf"),lsguige_table[iRow-1].weight);
					SimplifiedNumString(Item.szText);
				}
			}
			m_Grid.SetItem(&Item);
		}
	}

	m_Grid.AutoSize();
	m_Grid.SetRowHeight(0, 3*m_Grid.GetRowHeight(0)/2);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLsGuiGeTblDlg::OnOK() 
{
	CDialog::OnOK();
	int iRow,iCol;
	CString sText,sError;
	try{
		for(iRow=1;iRow<m_Grid.GetRowCount();iRow++)
		{
			for(iCol=0;iCol<m_Grid.GetColumnCount();iCol++)
			{
				sText = m_Grid.GetItemText(iRow,iCol);
				if(iCol==0)
					lsguige_table[iRow-1].grade = atof(sText);
				else if(iCol==1)
					lsguige_table[iRow-1].d = atoi(sText);
				else if(iCol==2)
					lsguige_table[iRow-1].valid_len = atoi(sText);
				else if(iCol==3)
					lsguige_table[iRow-1].no_thread_len = atoi(sText);
				else if(iCol==4)
					lsguige_table[iRow-1].down_lim = atoi(sText);
				else if(iCol==5)
					lsguige_table[iRow-1].up_lim = atoi(sText);
				else if(iCol==6)
					lsguige_table[iRow-1].weight = atof(sText);
				else
					throw "数据错误";
			}
		}
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
}

void CLsGuiGeTblDlg::OnBnExportLsFile() 
{
	CString sFileName;
	if(!Refresh(TRUE))
		return;
	CFileDialog dlg(FALSE,"lst","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"角钢规格文件(*.lst)|*.lst||");
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		LsGuiGeSerialize(sFileName,TRUE);
	}
}

void CLsGuiGeTblDlg::OnBnImportLsFile() 
{
	CString sFileName;
	CFileDialog dlg(TRUE,"lst","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"角钢规格文件(*.lst)|*.lst||");
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		LsGuiGeSerialize(sFileName,FALSE);
		nColumnsN = nLsXingHaoCount;
		m_Grid.SetRowCount(nLsXingHaoCount+1);

		Refresh(FALSE);
	}
}
BOOL CLsGuiGeTblDlg::Refresh(BOOL bFreshData) 
{
	int iRow,iCol;
	CString sText,sError;
	try
	{
		if(!bFreshData)
		{
			for(iRow=1;iRow<m_Grid.GetRowCount();iRow++)
			{
				for(iCol=0;iCol<m_Grid.GetColumnCount();iCol++)
				{
					if(iCol==0)	//级别
						sText.Format("%.1f",lsguige_table[iRow-1].grade);
					else if(iCol==1)	//直径
						sText.Format("%d",lsguige_table[iRow-1].d);
					else if(iCol==2)	//有效长
						sText.Format("%d",lsguige_table[iRow-1].valid_len);
					else if(iCol==3)	//无扣找
						sText.Format("%d",lsguige_table[iRow-1].no_thread_len);
					else if(iCol==4)
						sText.Format("%d",lsguige_table[iRow-1].down_lim);
					else if(iCol==5)
						sText.Format("%d",lsguige_table[iRow-1].up_lim);
					else
						sText.Format("%lf",lsguige_table[iRow-1].weight);
					m_Grid.SetItemText(iRow,iCol,sText);
					if(iCol<3)
					{
						m_Grid.SetItemFormat(iRow,iCol,DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
					}
				}
				m_Grid.RedrawRow(iRow);
			}
		}
		else
		{
			for(iRow=1;iRow<m_Grid.GetRowCount();iRow++)
			{
				for(iCol=0;iCol<m_Grid.GetColumnCount();iCol++)
				{
					sText = m_Grid.GetItemText(iRow,iCol);
					if(iCol==0)	//级别
						lsguige_table[iRow-1].grade = atof(sText);
					else if(iCol==1)	//直径
						lsguige_table[iRow-1].d = atoi(sText);
					else if(iCol==2)	//有效长
						lsguige_table[iRow-1].valid_len = atoi(sText);
					else if(iCol==3)	//无扣找
						lsguige_table[iRow-1].no_thread_len = atoi(sText);
					else if(iCol==4)	//通厚下限
						lsguige_table[iRow-1].down_lim = atoi(sText);
					else if(iCol==5)	//通厚上限
						lsguige_table[iRow-1].up_lim = atoi(sText);
					else
						lsguige_table[iRow-1].weight = atof(sText);
				}
			}
		}
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	return TRUE;
}
