#if !defined(AFX_DESIGNCONNECTIONPLATEDLG_H__190CBF20_1F77_447F_8CBC_F78012537232__INCLUDED_)
#define AFX_DESIGNCONNECTIONPLATEDLG_H__190CBF20_1F77_447F_8CBC_F78012537232__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignConnectionPlateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesignConnectionPlateDlg dialog
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
#include "RememberLocationDlg.h"
class CDesignConnectionPlateDlg : public CRememberLocationDlg
{
// Construction
public:
	CDesignConnectionPlateDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSLinePart *m_pLinePart;
// Dialog Data
	//{{AFX_DATA(CDesignConnectionPlateDlg)
	enum { IDD = IDD_DESIGN_CONNECTION_PLATE_DLG };
	BOOL	m_bAdjustWeldLen;
	BOOL	m_bSpecWeldScope;
	int		m_iPlateMaterial;
	int		m_iFaceN;
	CString	m_sPartNo;
	CString m_sSegI;
	UINT	m_nPlateThick;
	double	m_fWeldEndHigh;
	double	m_fWeldStartHigh;
	double	m_fWeldWidth;
	double  m_fEdgeSpace;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignConnectionPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignConnectionPlateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkSpecWeldScope();
	afx_msg void OnChangePartNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNCONNECTIONPLATEDLG_H__190CBF20_1F77_447F_8CBC_F78012537232__INCLUDED_)
