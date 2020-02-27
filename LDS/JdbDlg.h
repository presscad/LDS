#ifndef __JDBDLGH_
#define __JDBDLGH_
#include "resource.h"       // main symbols
#include "Tower.h"	// Added by ClassView
/////////////////////////////////////////////////////////////////////////////
// CJdbJgJointDlg dialog

class CJdbJgJointDlg : public CDialog
{
// Construction
public:
	JGZJ jgzj;
	BOOL m_bEnable2RowsLs;
	LSSPACE_STRU LsSpace;
	CJdbJgJointDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CJdbJgJointDlg)
	enum { IDD = IDD_JDB_JG_JOINT_DESIGN_DLG };
	int		m_nLsN;
	int		m_nLsSpace;
	double	m_fOddment;
	int		m_iLsLayOutStyle;	//螺栓布置方式0:顶上去1:退半孔间隙
	int		m_nLsRowsN;
	CString	m_sRayJgGuiGe;
	int		m_nG;
	int		m_nG1;
	int		m_nG2;
	int		m_nLsEndSpace;
	CString	m_sLsGuiGe;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJdbJgJointDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJdbJgJointDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCmbLsGuige();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
