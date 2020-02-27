#if !defined(AFX_ADDNEWLAYER_H__501B6D0B_074D_4DE1_A878_F30A2554CC21__INCLUDED_)
#define AFX_ADDNEWLAYER_H__501B6D0B_074D_4DE1_A878_F30A2554CC21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddNewLayer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AddNewLayer dialog

class AddNewLayer : public CDialog
{
// Construction
public:
	AddNewLayer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AddNewLayer)
	enum { IDD = IDD_BOM_CFG_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AddNewLayer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AddNewLayer)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDNEWLAYER_H__501B6D0B_074D_4DE1_A878_F30A2554CC21__INCLUDED_)
