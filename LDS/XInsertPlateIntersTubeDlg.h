#if !defined(AFX_XINSERTPLATEINTERSTUBEDLG_H__3E4602E8_B0FE_4713_9818_50FF83D3B2EA__INCLUDED_)
#define AFX_XINSERTPLATEINTERSTUBEDLG_H__3E4602E8_B0FE_4713_9818_50FF83D3B2EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XInsertPlateIntersTubeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXInsertPlateIntersTubeDlg dialog
#include "XhListCtrl.h"
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

class CXInsertPlateIntersTubeDlg : public CDialog
{
// Construction
	ATOM_LIST<f3dPoint> boltPosList;
	CBitmap m_CurPlateBmp;
public:
	CXInsertPlateIntersTubeDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSLineTube *m_pCrossTube,*m_pDatumTube;
	void RefreshDlgCtrlState();
	void CalOddment();
	void CreateBolt(CLDSParamPlate *pParamPlate);
// Dialog Data
	//{{AFX_DATA(CXInsertPlateIntersTubeDlg)
	enum { IDD = IDD_X_INSERT_PLATE_INTERS_TUBE_DLG };
	CXhListCtrl	m_listBoltRect;
	CString	m_sBoltSize;
	int		m_iCoverPlateMat;
	int		m_iWrapPlateMat;
	int		m_iDatumPlateMat;
	int		m_iTransPlateMat;
	int		m_iVertPlateMat;
	int		m_nBoltNum;
	CString	m_sCoverPlatePartNo;
	double	m_fCoverPlateSectorAngle;
	int		m_nCoverPlateThick;
	int		m_nCoverPlateWidth;
	int		m_nCoverPlateCutWidth;
	CString	m_sDatumPlatePartNo;
	int		m_nDatumPlateThick;
	int		m_nDatumPlateWidth;
	CString	m_sWeldPlatePartNo;
	CString	m_sWrapPlatePartNo;
	CString	m_sTransPlatePartNo;
	int		m_nTransPlateThick;
	CString	m_sVertPlatePartNo;
	int		m_nVertPlateThick;
	int		m_nVertPlateWidth;
	CString m_sSegI;
	int		m_nDatumPlateLen;
	int		m_nInsertPlateWeldLen;
	int		m_nWeldSpace;
	int		m_nTubeOddment;
	BOOL	m_bStdPart;
	int		m_iHoleType;
	double  m_fHoleR;
	int		m_nTransPlateWidth;
	BOOL	m_bTransPlate;
	CString	m_sStdPlatePartNo;
	int		m_iInsertPlateType;
	BOOL	m_bStaggerLayoutBolt;
	BOOL	m_bXEndDoubleRowLayout;
	int		m_nBoltColNum;
	int		m_nBoltRowNum;
	double	m_fBoltRowSpace;
	double	m_fBoltColSpace;
	double	m_fFirstBoltPosX;
	double	m_fFirstBoltPosY;
	double	m_fFirstBoltPosZ;
	double	m_fPlateParamA;
	double	m_fPlateParamN;
	double	m_fPlateParamR;
	double	m_fPlateParamS;
	double	m_fSpecBoltSpaceD;
	double	m_fPlateParamW;
	BOOL	m_bCreateVertPlate;
	double	m_fCoverPlateOffsetAngle;
	BOOL	m_bUseAngle;
	double	m_fCoverPlateManuSpace;
	double	m_fVertPlateManuSpace;
	CString	m_sJgGuige;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXInsertPlateIntersTubeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CXInsertPlateIntersTubeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDatumPlateMat();
	afx_msg void OnChangeDatumPlatePartNo();
	afx_msg void OnSelchangeTransPlateMat();
	afx_msg void OnChangeTransPlateThick();
	afx_msg void OnStdPart();
	afx_msg void UpdateTubeOddment();
	afx_msg void OnEditchangeBoltSize();
	afx_msg void OnBnPartsDb();
	afx_msg void OnSelchangeCmbBoltSize();
	afx_msg void OnChangeEInsertPlateWeldLen();
	afx_msg void OnSelChangeStdPlatePartNo();
	afx_msg void OnSelchangeInsertPlateType();
	afx_msg void OnKeydownBoltRectList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkBoltRectList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLayoutBolt();
	afx_msg void OnAddNewBoltItem();
	afx_msg void OnModifyBoltItem();
	virtual void OnOK();
	afx_msg void OnXendDoubleRowLayout();
	afx_msg void OnChangePlateParamW();
	afx_msg void OnCreateVertPlate();
	afx_msg void OnChkUseAngle();
	afx_msg void OnSelchangeJgGuige();
	afx_msg void OnEditchangeJgGuige();
	afx_msg void OnKillfocusJgGuige();
	afx_msg void OnCbnSelchangeCmbHoleType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeECoverPlatePartNo();
	afx_msg void OnEnChangeEVertPlatePartNo();
	afx_msg void OnEnChangeETransPlatePartNo();
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XINSERTPLATEINTERSTUBEDLG_H__3E4602E8_B0FE_4713_9818_50FF83D3B2EA__INCLUDED_)
