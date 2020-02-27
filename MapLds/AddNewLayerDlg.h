#if !defined(AFX_ADDNEWLAYERDLG_H__6F9240AA_C6BB_4157_9366_F21C70CCBA43__INCLUDED_)
#define AFX_ADDNEWLAYERDLG_H__6F9240AA_C6BB_4157_9366_F21C70CCBA43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#include "resource.h"
#endif // _MSC_VER > 1000
// AddNewLayerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddNewLayerDlg dialog
#include "RxTools.h"
#include "ColorSelectComboBox.h"

class CAddNewLayerDlg : public CDialog
{
// Construction
	CComboBox *m_pLineTypeCmbBox;
	CColorSelectComboBox *m_pColorCmbBox;
public:
	CAddNewLayerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddNewLayerDlg)
	enum { IDD = IDD_ADD_NEW_LAYER_DLG };
	CColorSelectComboBox m_cmbColor;
	CComboBox  *m_pLineWeightCmbBox;
	CString	m_sLayerName;
	CString m_sLineType;
	//}}AFX_DATA
	TMADRAWING_LAYER m_xLayer;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddNewLayerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddNewLayerDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//afx_msg void OnAddNewLayer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDNEWLAYERDLG_H__6F9240AA_C6BB_4157_9366_F21C70CCBA43__INCLUDED_)
