#if !defined(AFX_DESIGNONEBOLTDLG_H__611EA884_1A51_4930_9D73_61753246A312__INCLUDED_)
#define AFX_DESIGNONEBOLTDLG_H__611EA884_1A51_4930_9D73_61753246A312__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignOneBoltDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesignOneBoltDlg dialog

class CDesignOneBoltDlg : public CDialog
{
// Construction
public:
	CDesignOneBoltDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesignOneBoltDlg)
	enum { IDD = IDD_DESIGN_ONEBOLT_DLG };
	BOOL	m_bCheckUpLsNum;
	BOOL	m_bInterNodeBolt;
	BOOL	m_bSingleAngleEndBolt;
	BOOL	m_bSingleBoltNoPlate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignOneBoltDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignOneBoltDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNONEBOLTDLG_H__611EA884_1A51_4930_9D73_61753246A312__INCLUDED_)
