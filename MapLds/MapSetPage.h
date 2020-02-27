// MapSetPage.h : header file
//

#ifndef __MAPSETPAGE_H__
#define __MAPSETPAGE_H__

/////////////////////////////////////////////////////////////////////////////
// CStruMapPage dialog

class CStruMapPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CStruMapPage)

// Construction
public:
	CStruMapPage();
	~CStruMapPage();

// Dialog Data
	//{{AFX_DATA(CStruMapPage)
	enum { IDD = IDD_STRU_MAP_PROPPAGE };
	int		m_nLineMapScale;
	int		m_nPartMapScale;
	int		m_nStruMapScale;
	BOOL	m_bExportDeformedProfile;
	BOOL	m_bDimPlaneAngle;
	BOOL	m_bDimHuoquType;
	int		m_iLsApertureStyle;
	double	m_fMinWingThick;
	double	m_fHoleAugment;
	BOOL	m_bDimSpecialLsSpace;
	BOOL	m_bDimSpecialJgZhunJu;
	BOOL	m_bDimStruJgLen;
	double	m_fMinWingWide;
	BOOL	m_bCardLayoutBaseRow;
	int		m_nPartRowsN;
	int		m_nColumnWidth;
	int		m_nRowHigh;
	BOOL	m_bHasFrame;
	BOOL	m_bOneCardMultiPart;
	CString	m_sAngleCardPath;
	CString	m_sPlankCardPath;
	CString	m_sWeldPartGroupCardPath;
	CString	m_sProcessPartStatCardPath;
	BOOL	m_bProcessCardTbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStruMapPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CStruMapPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnLocateJgCard();
	afx_msg void OnBnLocateWeldPartGroupCard();
	afx_msg void OnBnLocatePlankCard();
	afx_msg void OnBnLocateProcessPartStatCard();
	afx_msg void OnChkCardLayoutBaseRow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CPartMapPage dialog

class CPartMapPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPartMapPage)

// Construction
public:
	CPartMapPage();
	~CPartMapPage();

// Dialog Data
	//{{AFX_DATA(CPartMapPage)
	enum { IDD = IDD_PART_MAP_PROPPAGE };
	BOOL	m_bModulateLongJg;
	double	m_fLsDistThreshold;
	int		m_iDimPrecision;
	double	m_fLsDistZoomCoef;
	BOOL	m_bDimJgGAtLeft;
	BOOL	m_bDimLsSpace;
	BOOL	m_bDimLsPos;
	BOOL	m_bDimCutAngle;
	BOOL	m_bDimCutAngleMap;
	BOOL	m_bDimPushFlatMap;
	BOOL	m_bDimKaiHe;
	int		m_iLsPosStyle;
	BOOL	m_bDecarlCoord;
	BOOL	m_bDimCoord;
	BOOL	m_bPolarCoord;
	BOOL	m_bUseZhengFanQun;
	BOOL	m_bDimPlateOutline;
	BOOL	m_bDimPlateBoltAngle;
	BOOL	m_bDimPlateRayLs;
	int		m_iPartMapType;
	BOOL	m_bJgUseSimpleLsMap;
	BOOL	m_bDimKaiheAngleMap;
	BOOL	m_bPlateUseSimpleLsMap;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPartMapPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPartMapPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CDimPage dialog

class CDimPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDimPage)
// Construction
public:
	CDimPage();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDimPage)
	enum { IDD = IDD_TEXT_AND_DIM_PROPPAGE };
	double	m_fLsGuigeTextSize;
	double	m_fPartGuigeTextSize;
	double	m_fPartNoTextSize;
	double	m_fJgOddmentTextSize;
	double	m_fDimArrowSize;
	double	m_fDimTextSize;
	double	m_fDimAngleTextSize;
	double	m_fHeaderTextSize;
	double	m_fCommonTextSize;
	double	m_fNumericTextSize;
	double	m_fSimTriScale;
	double	m_fOtherTextSize;
	CString	m_sFontName;
	double	m_fPartNoCirD;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDimPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDimPage)
	afx_msg void OnChangePartNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDrawPenPage dialog

class CDrawPenPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDrawPenPage)

// Construction
public:
	CDrawPenPage();
	~CDrawPenPage();

// Dialog Data
	//{{AFX_DATA(CDrawPenPage)
	enum { IDD = IDD_PEN_COLOR_PAGE };
	int		m_iHuoQuLine;
	int		m_iAxisLine;
	int		m_iSingleLine;
	int		m_iVisibleOutline;
	int		m_iLsHole;
	int		m_iDashLine;
	int		m_iDragLine;
	int		m_iDimSimTri;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDrawPenPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDrawPenPage)
	afx_msg void OnChangeEHuoquLine();
	afx_msg void OnChangeEAxisLine();
	afx_msg void OnChangeESingleLine();
	afx_msg void OnChangeEVisibleOutline();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeELsHole();
	afx_msg void OnChangeEDim();
	afx_msg void OnChangeEDashLine();
	afx_msg void OnChangeEDragLine();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CPartGroupPage dialog

class CPartGroupPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPartGroupPage)
// Construction
public:
	CPartGroupPage();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPartGroupPage)
	enum { IDD = IDD_PART_GROUP_PROPPAGE };
	BOOL	m_bDimAbsoluteDist;
	BOOL	m_bDimSpaceDist;
	BOOL	m_bModulatePartGroupLen;
	BOOL	m_bUseYOrder;
	BOOL	m_bDimCheckLine;
	BOOL	m_bAngularDimension;
	BOOL	m_bDimPlatePoleRayAndAngle;
	BOOL	m_bDimAbsoluteOutermostLayer;
	double	m_fLayerSpace;
	double	m_fPartGroupDistThreshold;
	double	m_fPartGroupDistZoomCoef;
	BOOL	m_bDimBeforeWeldCheckLine;
	BOOL	m_bDimAfterWeldCheckLine;
	BOOL	m_bAdjustScaleByProcessCard;
	BOOL	m_bAutoDimPartNo;
	BOOL	m_bAutoMatchView;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartGroupPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartGroupPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __MAPSETPAGE_H__
