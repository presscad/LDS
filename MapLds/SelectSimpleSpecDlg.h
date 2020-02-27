#if !defined(AFX_SELECTSIMPLESPECDLG_H__F124A3FF_AC7A_454E_9BAB_3EC848BA85C1__INCLUDED_)
#define AFX_SELECTSIMPLESPECDLG_H__F124A3FF_AC7A_454E_9BAB_3EC848BA85C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectSimpleSpecDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectSimpleSpecDlg dialog

#include "resource.h"
#include "XhListCtrl.h"
#include "DrawUnit.h"

class CSelectSimpleSpecDlg : public CDialog
{
// Construction
	void UpdatePartList();
	void InitDimpartspec();
	ATOM_LIST<SPEC_MARK> m_specList;
public:
	CSelectSimpleSpecDlg(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CSelectSimpleSpecDlg)
	enum { IDD = IDD_SELECT_SIMPLE_SPEC_DLG };
	CXhListCtrl	m_listCtrl;
	ATOM_LIST<SPEC_MARK> *m_pSpecList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectSimpleSpecDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CDimPartSpecList m_dimpartspec;
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectSimpleSpecDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTSIMPLESPECDLG_H__F124A3FF_AC7A_454E_9BAB_3EC848BA85C1__INCLUDED_)
