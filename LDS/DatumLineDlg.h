#if !defined(AFX_DATUMLINEDLG_H__12BFF805_7660_4DD0_B561_6AA86716D689__INCLUDED_)
#define AFX_DATUMLINEDLG_H__12BFF805_7660_4DD0_B561_6AA86716D689__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DatumLineDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDatumLineDlg dialog

class CDatumLineDlg : public CDialog
{
// Construction
public:
	CDatumLineDlg(CWnd* pParent = NULL);   // standard constructor
	THANDLE m_hFirstPoint,m_hSecondPoint;
// Dialog Data
	//{{AFX_DATA(CDatumLineDlg)
	enum { IDD = IDD_DATUM_LINE_DLG };
	CString	m_shDatumLine;
	CString	m_sLineDescription;
	CString	m_sFirstPointDescrip;
	CString	m_sSecondPointDescrip;
	int		m_iCmbFirstPoint;
	int		m_iCmbSecPoint;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatumLineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDatumLineDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCmbFirstPoint();
	afx_msg void OnSelchangeCmbSecPoint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATUMLINEDLG_H__12BFF805_7660_4DD0_B561_6AA86716D689__INCLUDED_)
