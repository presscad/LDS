#if !defined(AFX_FOLDERNAMEDLG_H__4D27F49E_5BD6_4D8C_A921_B25748153F4E__INCLUDED_)
#define AFX_FOLDERNAMEDLG_H__4D27F49E_5BD6_4D8C_A921_B25748153F4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FolderNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFolderNameDlg dialog

class CFolderNameDlg : public CDialog
{
// Construction
public:
	CFolderNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFolderNameDlg)
	enum { IDD = IDD_FOLDER_NAME };
	CString	m_sFolderName;
	//}}AFX_DATA
	int		m_nNameMaxLen;
	int		m_nDlgType;	//1.定制视图组名 2.组合构件组名
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLDSPartGroup *m_pCurPartGroup;
#endif
	CDisplayView *m_pCurView;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFolderNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFolderNameDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOLDERNAMEDLG_H__4D27F49E_5BD6_4D8C_A921_B25748153F4E__INCLUDED_)
