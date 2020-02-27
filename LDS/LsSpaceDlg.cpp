// LsSpaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "LsSpaceDlg.h"
#include "database.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLsSpaceDlg dialog


CLsSpaceDlg::CLsSpaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLsSpaceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLsSpaceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	nColumnsN = 7;
	nRowsN = 5;
}


void CLsSpaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLsSpaceDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_GridControl(pDX, IDC_LS_SPACE_GRID, m_Grid);
}


BEGIN_MESSAGE_MAP(CLsSpaceDlg, CDialog)
	//{{AFX_MSG_MAP(CLsSpaceDlg)
	ON_BN_CLICKED(ID_IMPORT_LS_SPACE_TBL, OnImportLsSpaceTbl)
	ON_BN_CLICKED(ID_EXPORT_LS_SPACE_TBL, OnExportLsSpaceTbl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLsSpaceDlg message handlers

BOOL CLsSpaceDlg::OnInitDialog() 
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
				Item.nFormat = DT_CENTER|DT_WORDBREAK;
				if(iCol==0)
					Item.szText.Format(_T("ÂÝË¨Ö±¾¶"));
				else if(iCol==1)
					Item.szText.Format(_T("µ¥ÅÅ¼ä¾à"));
				else if(iCol==2)
					Item.szText.Format(_T("Ë«ÅÅ¼ä¾à"));
				else if(iCol==3)
					Item.szText.Format(_T("¶Ë¾à"));
				else if(iCol==4)
					Item.szText.Format(_T("Á¬°å±ß¾à"));
				else if(iCol==5)
					Item.szText.Format(_T("ÔþÖÆ±ß¾à¡Ý"));
				else
					Item.szText.Format(_T("ÇÐ½Ç±ß¾à¡Ý"));
			}
			else
			{
				if(iCol==0)
					Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
				else
					Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
				Item.szText.Format(_T("%d"),LSSPACE_TABLE[iRow-1][iCol]);
			}
			m_Grid.SetItem(&Item);
		}
		// Make jg wing wide column read-only
		m_Grid.SetItemState(iRow,0, m_Grid.GetItemState(iRow,0) | GVIS_READONLY);
	}

	m_Grid.AutoSize();
	m_Grid.SetColumnWidth(0,40);
	m_Grid.SetColumnWidth(1,40);
	m_Grid.SetColumnWidth(2,40);
	m_Grid.SetColumnWidth(3,40);
	m_Grid.SetColumnWidth(4,60);
	m_Grid.SetColumnWidth(5,60);
	m_Grid.SetColumnWidth(6,60);
	m_Grid.SetRowHeight(0, 3*m_Grid.GetRowHeight(0)/2);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLsSpaceDlg::OnOK() 
{
	CDialog::OnOK();
	int iRow,iCol;
	CString sText,sError;
	int g;
	for(iRow=1;iRow<m_Grid.GetRowCount();iRow++)
	{
		for(iCol=1;iCol<m_Grid.GetColumnCount();iCol++)
		{
			sText = m_Grid.GetItemText(iRow,iCol);
			g = atoi(sText);
			LSSPACE_TABLE[iRow-1][iCol] = g;
		}
	}
}


void CLsSpaceDlg::OnImportLsSpaceTbl() 
{
	CString sFileName;
	CFileDialog dlg(TRUE,"lss","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"ÂÝË¨¼ä¾à±ß¾àÎÄ¼þ(*.lss)|*.lss||");
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		LsSpaceSerialize(sFileName,FALSE);
		Refresh(FALSE);
	}
}

void CLsSpaceDlg::OnExportLsSpaceTbl() 
{
	CString sFileName;
	CFileDialog dlg(TRUE,"lss","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"ÂÝË¨¼ä¾à±ß¾àÎÄ¼þ(*.lss)|*.lss||");
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		LsSpaceSerialize(sFileName,TRUE);
	}
}
BOOL CLsSpaceDlg::Refresh(BOOL bFreshData)
{
	int iRow,iCol,g;
	CString sText,sError;
	try
	{
		if(!bFreshData)
		{
			for(iRow=1;iRow<m_Grid.GetRowCount();iRow++)
			{
				for(iCol=0;iCol<m_Grid.GetColumnCount();iCol++)
				{
					sText.Format("%d",LSSPACE_TABLE[iRow-1][iCol]);
					m_Grid.SetItemText(iRow,iCol,sText);
				}
				m_Grid.RedrawRow(iRow);
				m_Grid.SetItemFormat(iRow,0,DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
				m_Grid.SetItemState(iRow,0,m_Grid.GetItemState(iRow,0) | GVIS_READONLY);
			}
		}
		else
		{
			for(iRow=1;iRow<m_Grid.GetRowCount();iRow++)
			{
				for(iCol=1;iCol<m_Grid.GetColumnCount();iCol++)
				{
					sText = m_Grid.GetItemText(iRow,iCol);
					g = atoi(sText);
					LSSPACE_TABLE[iRow-1][iCol] = g;
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
