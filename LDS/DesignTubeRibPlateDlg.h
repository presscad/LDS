#if !defined(AFX_DESIGNTUBERIBPLATEDLG_H__90423F85_F097_47F5_B1C3_0EB027122BE7__INCLUDED_)
#define AFX_DESIGNTUBERIBPLATEDLG_H__90423F85_F097_47F5_B1C3_0EB027122BE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignTubeRibPlateDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CDesignTubeRibPlateDlg dialog

class CDesignTubeRibPlateDlg : public CDialog
{
// Construction
public:
	BOOL AutoCalWN();
	CDesignTubeRibPlateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesignTubeRibPlateDlg)
	enum { IDD = IDD_DESIGN_TUBE_RIB_PLATE_DLG };
	double	m_fParamW;
	double	m_fParamH;
	double	m_fParamR;
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	CString	m_sPartNo;
	CString m_sSegI;
	int		m_iThick;
	int		m_iMaterial;
	double	m_fNormOffset;
	BOOL	m_bMirCreate;
	double	m_fParamN;
	BOOL	m_bDesignOuterPlate;
	BOOL	m_bCirPlate;
	BOOL	m_bTopArcConnection;
	//}}AFX_DATA
	BOOL	m_bModifyZ;
	CLDSLinePart *m_pSetUpPart;
	CLDSLineTube *m_pDatumTube;
	CLDSNode *m_pDatumNode;
	CLDSPlate *m_pDatumPlate1;	//, *m_pDatumPlate2;
	CLDSPart *m_pDatumPart2;	//第二个基准构件可以为钢管或者钢板 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignTubeRibPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignTubeRibPlateDlg)
	afx_msg void OnBnPartsDb();
	afx_msg void OnChangePartNo();
	afx_msg void OnChangeNormOffset();
	virtual BOOL OnInitDialog();
	afx_msg void OnDesignOuterPlate();
	afx_msg void OnCirPlate();
	afx_msg void OnTopArcConnection();
	afx_msg void OnKillfocusParamH();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNTUBERIBPLATEDLG_H__90423F85_F097_47F5_B1C3_0EB027122BE7__INCLUDED_)
