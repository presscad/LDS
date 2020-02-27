#if !defined(AFX_DESIGNXENDRIBPLATEDLG_H__B67704E7_4403_4115_A2D1_679DB8D12DA7__INCLUDED_)
#define AFX_DESIGNXENDRIBPLATEDLG_H__B67704E7_4403_4115_A2D1_679DB8D12DA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignXEndRibPlateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesignXEndRibPlateDlg dialog

class CDesignXEndRibPlateDlg : public CDialog
{
// Construction
public:
	CDesignXEndRibPlateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesignXEndRibPlateDlg)
	enum { IDD = IDD_DESIGN_XEND_RIB_PLATE_DLG };
	BOOL	m_bMirCreate;
	BOOL	m_bUseAngleParam;
	int		m_iMaterial;
	double	m_fAngle;
	double	m_fBolEndSpace;
	double	m_fCutAngleWidth;
	double	m_fParaS;
	CString	m_sPartNo;
	double	m_fPlateLen;
	double	m_fPlateWidth;
	CString m_sSegI;
	int		m_nThick;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignXEndRibPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignXEndRibPlateDlg)
	afx_msg void OnChangePartNo();
	virtual BOOL OnInitDialog();
	afx_msg void OnUseAngleParam();
	afx_msg void OnChanegAngle();
	afx_msg void OnChanegCutAngleWidth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNXENDRIBPLATEDLG_H__B67704E7_4403_4115_A2D1_679DB8D12DA7__INCLUDED_)
