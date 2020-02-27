#if !defined(AFX_ADDCIRCULARCOVERPLATEDLG_H__474A8E96_B3AB_4C8B_A0FE_B51383ABCD67__INCLUDED_)
#define AFX_ADDCIRCULARCOVERPLATEDLG_H__474A8E96_B3AB_4C8B_A0FE_B51383ABCD67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifdef __ANGLE_PART_INC_
// AddCircularCoverPlateDlg.h : header file
//
#include "lds_part.h"

/////////////////////////////////////////////////////////////////////////////
// CAddCircularCoverPlateDlg dialog

class CAddCircularCoverPlateDlg : public CDialog
{
// Construction
public:
	CAddCircularCoverPlateDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSPart *pDatumPlate1,*pDatumPlate2;
// Dialog Data
	//{{AFX_DATA(CAddCircularCoverPlateDlg)
	enum { IDD = IDD_ADD_CIRCULAR_COVER_PLATE_DLG };
	double	m_fParaD;
	double	m_fParaW;
	CString	m_sPartNo;
	CString m_sSegI;
	int		m_nThick;
	int		m_iMaterial;
	double	m_fParaB;
	BOOL	m_bIncDownPlate;
	BOOL	m_bIncUpPlate;
	BOOL	m_bMirSide;	//在贯通连板另外一侧加封板
	double	m_fParaA;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddCircularCoverPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddCircularCoverPlateDlg)
	afx_msg void OnBnPartsDb();
	afx_msg void OnChangeEPlatePartNo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDCIRCULARCOVERPLATEDLG_H__474A8E96_B3AB_4C8B_A0FE_B51383ABCD67__INCLUDED_)
