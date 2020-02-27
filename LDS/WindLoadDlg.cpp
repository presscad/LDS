// WindLoadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "GridCtrl.h"
#include "WindLoadDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWindLoadDlg dialog


CWindLoadDlg::CWindLoadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWindLoadDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWindLoadDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nRowsN = 5;
	m_nColumnsN = 5;
}


void CWindLoadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWindLoadDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_GridControl(pDX, IDC_WIND_LOAD_GRID, m_Grid);
}


BEGIN_MESSAGE_MAP(CWindLoadDlg, CDialog)
	//{{AFX_MSG_MAP(CWindLoadDlg)
	ON_BN_CLICKED(IDC_BN_EXPORT_G_FILE, OnBnExportGFile)
	ON_BN_CLICKED(IDC_BN_IMPORT_G_FILE, OnBnImportGFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindLoadDlg message handlers

void CWindLoadDlg::OnBnExportGFile() 
{
	// TODO: Add your control notification handler code here
	
}

void CWindLoadDlg::OnBnImportGFile() 
{
	// TODO: Add your control notification handler code here
	
}

BOOL CWindLoadDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Grid.SetListMode(TRUE);
	
	m_Grid.SetHeaderSort(FALSE);
	
	m_Grid.SetEditable(TRUE);
	m_Grid.SetTextBkColor(RGB(0xFF, 0xFF, 0xFF));

	try {
		m_Grid.SetRowCount(m_nRowsN);
		m_Grid.SetColumnCount(m_nColumnsN);
		m_Grid.SetFixedRowCount(1);
		m_Grid.SetFixedColumnCount(1);
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
				m_Grid.SetColumnWidth(iCol,70);
				Item.nFormat = DT_RIGHT|DT_WORDBREAK;
				if(iCol==0)
					Item.szText.Format(_T("工况号"));
				else if(iCol==1)
					Item.szText.Format(_T("X向荷载"));
				else if(iCol==2)
					Item.szText.Format(_T("Y向荷载"));
				else if(iCol==3)
					Item.szText.Format(_T("Z向荷载"));
				else
					Item.szText.Format(_T("组合系数"));
			}
			else
			{
				if(iCol<1)
				{
					Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
					/*if((int)(jgguige_table[iRow-1][iCol]*10)%10>0)
						Item.szText.Format(_T("%.1f"),jgguige_table[iRow-1][iCol]);
					else
						Item.szText.Format(_T("%.0f"),jgguige_table[iRow-1][iCol]);*/
				}
				else
				{
					Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
					Item.szText.Format(_T("%.2f"),0);
				}
			}
			m_Grid.SetItem(&Item);
		}
		// Make jg wing wide column read-only
		m_Grid.SetItemState(iRow,0, m_Grid.GetItemState(iRow,0) | GVIS_READONLY);
	}

	//m_Grid.AutoSize();
	//m_Grid.SetRowHeight(0, 3*m_Grid.GetRowHeight(0)/2);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
