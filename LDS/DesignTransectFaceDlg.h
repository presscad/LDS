#if !defined(AFX_DESIGNTRANSECTFACEDLG_H__82F0AE60_2D39_4D06_887A_2CAAC1E190C0__INCLUDED_)
#define AFX_DESIGNTRANSECTFACEDLG_H__82F0AE60_2D39_4D06_887A_2CAAC1E190C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignTransectFaceDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CDesignTransectFaceDlg dialog

class CDesignTransectFaceDlg : public CDialog
{
// Construction
public:
	BOOL m_bStartEnd;
	CLDSLineTube* m_pDatumTube;
	CDesignTransectFaceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesignTransectFaceDlg)
	enum { IDD = IDD_DESIGN_TRANSECT_FACE_DLG };
	BOOL	m_bFaceToFace;
	CString	m_hLinkObj;
	CString	m_hViceLinkObj;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignTransectFaceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignTransectFaceDlg)
	afx_msg void OnChkFaceToFace();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNTRANSECTFACEDLG_H__82F0AE60_2D39_4D06_887A_2CAAC1E190C0__INCLUDED_)
