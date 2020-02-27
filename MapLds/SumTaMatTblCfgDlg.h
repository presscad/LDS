#if !defined(AFX_SUMTAMATTBLDLG_H__0D1228ED_5FF2_40ED_BD30_B12C213FDB7E__INCLUDED_)
#define AFX_SUMTAMATTBLDLG_H__0D1228ED_5FF2_40ED_BD30_B12C213FDB7E__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SumTaMatTblCfgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSumTaMatTblCfgDlg dialog
#include "resource.h"
#include "supergridctrl.h"
#include "Markup.h"
enum LISTITEM_TYPE
{
	DATA_TOWER	= 1,	//ËþÉí B
	DATA_LEG	= 2,	//ËþÍÈ L
};
class CSumTaMatTblCfgDlg : public CDialog
{
	// Construction
public:
	CSumTaMatTblCfgDlg(CWnd* pParent = NULL);   // standard constructor
	CSuperGridCtrl	m_listCtrl;
	int	m_iUnit;
	// Dialog Data
	//{{AFX_DATA(CSumTaMatTblCfgDlg)
	enum { IDD = IDD_SUMTAMAT_TBL_CFG_DLG };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	CSuperGridCtrl::CTreeItem *ExprotListChildItemToXML(
		CMarkup &xml,LISTITEM_TYPE data_type,CSuperGridCtrl::CTreeItem *pParentItem);
	
	CSuperGridCtrl::CTreeItem *ImprotXMLChildElemToList(
		CMarkup xml,LISTITEM_TYPE data_type,CSuperGridCtrl::CTreeItem *pParentItem);
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSumTaMatTblCfgDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CSumTaMatTblCfgDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	afx_msg void OnNewTowerBody();
	afx_msg void OnNewLeg();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUMTAMATTBLDLG_H__0D1228ED_5FF2_40ED_BD30_B12C213FDB7E__INCLUDED_)
