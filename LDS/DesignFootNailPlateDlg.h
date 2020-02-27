#if !defined(AFX_DESIGNFOOTNAILPLATEDLG_H__5BE0F139_C4A5_47DC_A427_FA1A18E65D47__INCLUDED_)
#define AFX_DESIGNFOOTNAILPLATEDLG_H__5BE0F139_C4A5_47DC_A427_FA1A18E65D47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignFootNailPlateDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CDesignFootNailPlateDlg dialog

class CDesignFootNailPlateDlg : public CDialog
{
// Construction
public:
	CDesignFootNailPlateDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSLineTube *m_pLineTube;
	int part_type;	//0-½Å¶¤°å 1-½Ç¸Ö½Å¶¤
	int m_iBoltD;	//ÂÝË¨Ö±¾¶
// Dialog Data
	//{{AFX_DATA(CDesignFootNailPlateDlg)
	enum { IDD = IDD_DESIGN_FOOT_NAIL_PLATE_DLG };
	double		m_fParamH;
	double		m_fParamL;
	double		m_fParamT;
	double		m_fParamW;
	CString		m_sSegNo;
	CString		m_sPartNo;
	CString		m_sGuiGe;
	int			m_iMaterial;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignFootNailPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignFootNailPlateDlg)
	virtual void OnOK();
	afx_msg void OnChangePartNo();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeGuige();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNFOOTNAILPLATEDLG_H__5BE0F139_C4A5_47DC_A427_FA1A18E65D47__INCLUDED_)
