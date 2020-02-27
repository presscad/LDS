#if !defined(AFX_CALTWOTUBEANGLEDLG_H__A3C43A1C_8D16_457A_8EFF_DBD64A48D830__INCLUDED_)
#define AFX_CALTWOTUBEANGLEDLG_H__A3C43A1C_8D16_457A_8EFF_DBD64A48D830__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CalTwoTubeAngleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCalTwoTubeAngleDlg dialog
#include "Tower.h"
#include "resource.h"

#ifndef __TSA_FILE_
class CCalTwoTubeAngleDlg : public CDialog
{
// Construction
public:
	CCalTwoTubeAngleDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSLineTube *m_pStartTube, *m_pEndTube;
	double CalTwoTubeAngle();
	f3dPoint m_ptDimStart,m_ptDimEnd,m_ptDimVertex;
	BOOL m_bCanDimAngle;	//两基准钢管句柄是通过选择得到才可标注角度
// Dialog Data
	//{{AFX_DATA(CCalTwoTubeAngleDlg)
	enum { IDD = IDD_CAL_TWO_TUBE_ANGLE_DLG };
	BOOL	m_bDimTubeAngle;
	double	m_fDimRadius;
	CString	m_sEndTubeHandle;
	CString	m_sStartTubeHandle;
	double	m_fTubeAngle;
	BOOL	m_bInputTubeHandle;
	CString	m_sPromptStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalTwoTubeAngleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCalTwoTubeAngleDlg)
	afx_msg void OnCalTwoTubeAngle();
	virtual BOOL OnInitDialog();
	afx_msg void OnInputTubeHandle();
	afx_msg void OnKillfocusTubeHandle();
	afx_msg void OnDimTubeAngle();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALTWOTUBEANGLEDLG_H__A3C43A1C_8D16_457A_8EFF_DBD64A48D830__INCLUDED_)
