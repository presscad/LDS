#if !defined(AFX_DATUMFACEOFFSETDLG_H__AFFD45F3_2545_4C89_8B0A_D52D3EA6D3D7__INCLUDED_)
#define AFX_DATUMFACEOFFSETDLG_H__AFFD45F3_2545_4C89_8B0A_D52D3EA6D3D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DatumFaceOffsetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDatumFaceOffsetDlg dialog

class CDatumFaceOffsetDlg : public CDialog
{
// Construction
public:
	CDatumFaceOffsetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDatumFaceOffsetDlg)
	enum { IDD = IDD_JGEND_DATUM_FACE_OFFSET_SET_DLG };
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	int		m_iBaseStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatumFaceOffsetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDatumFaceOffsetDlg)
	afx_msg void OnBnPasteNorm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATUMFACEOFFSETDLG_H__AFFD45F3_2545_4C89_8B0A_D52D3EA6D3D7__INCLUDED_)
