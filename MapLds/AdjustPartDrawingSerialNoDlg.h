#if !defined(AFX_ADJUSTPARTDRAWINGSERIALNODLG_H__E7DAA152_80DF_4067_909B_4A39A2338524__INCLUDED_)
#define AFX_ADJUSTPARTDRAWINGSERIALNODLG_H__E7DAA152_80DF_4067_909B_4A39A2338524__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdjustPartDrawingSerialNoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdjustPartDrawingSerialNoDlg dialog
#include "resource.h"
#include "Drawing.h"

class CAdjustPartDrawingSerialNoDlg : public CDialog
{
// Construction
public:
	CAdjustPartDrawingSerialNoDlg(CWnd* pParent = NULL);   // standard constructor
	int m_nMaxSerialNo;
	int m_nCurSerialNo,m_nDestSerialNo;
// Dialog Data
	//{{AFX_DATA(CAdjustPartDrawingSerialNoDlg)
	enum { IDD = IDD_ADJUST_PART_DRAWING_SERIALNO };
	int		m_iCmdSerialNo;
	CString	m_sSpecPartSerialNo;
	//}}AFX_DATA

	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdjustPartDrawingSerialNoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdjustPartDrawingSerialNoDlg)
	afx_msg void OnBtnMoveAfter();
	afx_msg void OnBtnMoveBefore();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADJUSTPARTDRAWINGSERIALNODLG_H__E7DAA152_80DF_4067_909B_4A39A2338524__INCLUDED_)
