#if !defined(AFX_TUBEJOINTTUBETYPEDLG_H__33760CA1_42A4_4F95_8ADF_F22B31A92E6C__INCLUDED_)
#define AFX_TUBEJOINTTUBETYPEDLG_H__33760CA1_42A4_4F95_8ADF_F22B31A92E6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TubeJointTubeTypeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTubeJointTubeTypeDlg dialog

class CTubeJointTubeTypeDlg : public CDialog
{
// Construction
public:
	CTubeJointTubeTypeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTubeJointTubeTypeDlg)
	enum { IDD = IDD_TUBE_JOINT_TUBE_TYPE_DLG };
	int		m_iFl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTubeJointTubeTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTubeJointTubeTypeDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TUBEJOINTTUBETYPEDLG_H__33760CA1_42A4_4F95_8ADF_F22B31A92E6C__INCLUDED_)
