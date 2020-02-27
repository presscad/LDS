//<LOCALE_TRANSLATE/>
// MapSetPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MapSetPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CStruMapPage, CPropertyPage)
IMPLEMENT_DYNCREATE(CPartMapPage, CPropertyPage)
IMPLEMENT_DYNCREATE(CDimPage, CPropertyPage)
IMPLEMENT_DYNCREATE(CPartGroupPage, CPropertyPage)


/////////////////////////////////////////////////////////////////////////////
// CStruMapPage property page

CStruMapPage::CStruMapPage() : CPropertyPage(CStruMapPage::IDD)
{
	//{{AFX_DATA_INIT(CStruMapPage)
	m_nLineMapScale = 200;
	m_nPartMapScale = 20;
	m_nStruMapScale = 5;
	m_bExportDeformedProfile = FALSE;
	m_bDimPlaneAngle = FALSE;
	m_bDimHuoquType = FALSE;
	m_iLsApertureStyle = 0;
	m_fMinWingThick = 10.0;
	m_fHoleAugment = 0.0;
	m_bDimSpecialLsSpace = TRUE;
	m_bDimSpecialJgZhunJu = FALSE;
	m_bDimStruJgLen = FALSE;
	m_fMinWingWide = 0.0;
	m_bCardLayoutBaseRow = FALSE;
	m_nPartRowsN = 10;
	m_nColumnWidth = 200;
	m_nRowHigh = 150;
	m_bHasFrame = FALSE;
	m_bOneCardMultiPart = FALSE;
	m_sAngleCardPath = _T("");
	m_sPlankCardPath = _T("");
	m_sWeldPartGroupCardPath = _T("");
	m_sProcessPartStatCardPath = _T("");
	m_bProcessCardTbl = FALSE;
	//}}AFX_DATA_INIT
}

CStruMapPage::~CStruMapPage()
{
}

void CStruMapPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStruMapPage)
	DDX_Text(pDX, IDC_E_LINE_MAP_SCALE, m_nLineMapScale);
	DDV_MinMaxInt(pDX, m_nLineMapScale, 1, 500);
	DDX_Text(pDX, IDC_E_PART_MAP_SCALE, m_nPartMapScale);
	DDV_MinMaxInt(pDX, m_nPartMapScale, 1, 500);
	DDX_Text(pDX, IDC_E_STRU_MAP_SCALE, m_nStruMapScale);
	DDV_MinMaxInt(pDX, m_nStruMapScale, 1, 500);
	DDX_Check(pDX, IDC_CHK_EXPORT_DEFORMED_PROFILE, m_bExportDeformedProfile);
	DDX_Check(pDX, IDC_CHK_DIM_PALNE_ANGLE, m_bDimPlaneAngle);
	DDX_Check(pDX, IDC_CHK_HUOQU_TYPE, m_bDimHuoquType);
	DDX_Radio(pDX, IDC_RDO_LS_APERTURE, m_iLsApertureStyle);
	DDX_Text(pDX, IDC_E_MIN_WING_THICK, m_fMinWingThick);
	DDX_Text(pDX, IDC_E_HOLE_AUGMENT, m_fHoleAugment);
	DDV_MinMaxDouble(pDX, m_fHoleAugment, 0., 10.);
	DDX_Check(pDX, IDC_CHK_DIM_SPECIAL_LSSPACE, m_bDimSpecialLsSpace);
	DDX_Check(pDX, IDC_CHK_DIM_SPECIAL_JGZHUNJU, m_bDimSpecialJgZhunJu);
	DDX_Check(pDX, IDC_CHK_DIM_STRU_DIM_JGLEN, m_bDimStruJgLen);
	DDX_Text(pDX, IDC_E_MIN_WING_WIDE, m_fMinWingWide);
	DDX_Check(pDX, IDC_CHK_CARD_LAYOUT_BASE_ROW, m_bCardLayoutBaseRow);
	DDX_Text(pDX, IDC_E_PART_ROWS_N, m_nPartRowsN);
	DDX_Text(pDX, IDC_E_COLUMN_WIDTH, m_nColumnWidth);
	DDX_Text(pDX, IDC_E_ROW_HIGH, m_nRowHigh);
	DDX_Check(pDX, IDC_CHK_ONE_CARD_FRAME, m_bHasFrame);
	DDX_Check(pDX, IDC_CHK_ONE_CARD_MULTI_PART, m_bOneCardMultiPart);
	DDX_Text(pDX, IDC_E_JG_CARD_PATH, m_sAngleCardPath);
	DDX_Text(pDX, IDC_E_PLANK_CARD_PATH, m_sPlankCardPath);
	DDX_Text(pDX, IDC_E_WELD_PART_GROUP_CARD_PATH, m_sWeldPartGroupCardPath);
	DDX_Text(pDX, IDC_E_PROCESS_PART_STAT_CARD_PATH, m_sProcessPartStatCardPath);
	DDX_Check(pDX, IDC_CHK_PROCESS_CARD_TBL, m_bProcessCardTbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStruMapPage, CPropertyPage)
	//{{AFX_MSG_MAP(CStruMapPage)
	ON_BN_CLICKED(IDC_BN_LOCATE_JG_CARD, OnBnLocateJgCard)
	ON_BN_CLICKED(IDC_BN_LOCATE_WELD_PART_GROUP_CARD, OnBnLocateWeldPartGroupCard)
	ON_BN_CLICKED(IDC_BN_LOCATE_PLANK_CARD, OnBnLocatePlankCard)
	ON_BN_CLICKED(IDC_CHK_CARD_LAYOUT_BASE_ROW, OnChkCardLayoutBaseRow)
	ON_BN_CLICKED(IDC_BN_LOCATE_PROCESS_PART_STAT_CARD, OnBnLocateProcessPartStatCard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CStruMapPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	if(m_bCardLayoutBaseRow)
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_MAXROW_OR_COLUMN)->SetWindowText("the maximum column numbers in a row of part map:");
	else
		GetDlgItem(IDC_S_MAXROW_OR_COLUMN)->SetWindowText("the maximum column numbers in a row of part map:");
#else
		GetDlgItem(IDC_S_MAXROW_OR_COLUMN)->SetWindowText("构件图每列最多行数:");
	else
		GetDlgItem(IDC_S_MAXROW_OR_COLUMN)->SetWindowText("构件图每行最多列数:");
#endif
	return TRUE;
}

void CStruMapPage::OnChkCardLayoutBaseRow() 
{
	UpdateData();
	if(m_bCardLayoutBaseRow)
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_MAXROW_OR_COLUMN)->SetWindowText("the maximum row numbers in a column of part map:");
	else
		GetDlgItem(IDC_S_MAXROW_OR_COLUMN)->SetWindowText("the maximum row numbers in a column of part map:");
#else
		GetDlgItem(IDC_S_MAXROW_OR_COLUMN)->SetWindowText("构件图每列最多行数:");
	else
		GetDlgItem(IDC_S_MAXROW_OR_COLUMN)->SetWindowText("构件图每行最多列数:");
#endif
}

void CStruMapPage::OnBnLocateJgCard() 
{
	UpdateData();
	CFileDialog dlg(TRUE,"dwg","jgcard",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"angle process card(*.dwg)|*.dwg||");
#else
		"角钢工艺卡(*.dwg)|*.dwg||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	m_sAngleCardPath = dlg.GetPathName();
	UpdateData(FALSE);
}

void CStruMapPage::OnBnLocateWeldPartGroupCard() 
{
	UpdateData();
	CFileDialog dlg(TRUE,"dwg","weldpartcard",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"weld combined part process card(*.dwg)|*.dwg||");
#else
		"焊接组合件工艺卡(*.dwg)|*.dwg||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	m_sWeldPartGroupCardPath = dlg.GetPathName();
	UpdateData(FALSE);
}

void CStruMapPage::OnBnLocateProcessPartStatCard() 
{
	UpdateData();
	CFileDialog dlg(TRUE,"dwg","processstartcard",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Process part statistics card(*.dwg)|*.dwg||");
#else
		"工艺件统计卡(*.dwg)|*.dwg||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	m_sProcessPartStatCardPath = dlg.GetPathName();
	UpdateData(FALSE);
}

void CStruMapPage::OnBnLocatePlankCard() 
{
	UpdateData();
	CFileDialog dlg(TRUE,"dwg","plankcard",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"plate process card(*.dwg)|*.dwg||");
#else
		"板工艺卡(*.dwg)|*.dwg||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	m_sPlankCardPath = dlg.GetPathName();
	UpdateData(FALSE);	
}

/////////////////////////////////////////////////////////////////////////////
// CPartMapPage property page

CPartMapPage::CPartMapPage() : CPropertyPage(CPartMapPage::IDD)
{
	//{{AFX_DATA_INIT(CPartMapPage)
	m_bModulateLongJg = FALSE;
	m_fLsDistThreshold = 80.0;
	m_iDimPrecision = 0;
	m_fLsDistZoomCoef = 0.0;
	m_bDimJgGAtLeft = FALSE;
	m_bDimLsSpace = FALSE;
	m_bDimLsPos = FALSE;
	m_bDimCutAngle = FALSE;
	m_bDimCutAngleMap = FALSE;
	m_bDimPushFlatMap = FALSE;
	m_bDimKaiHe = FALSE;
	m_iLsPosStyle = 0;
	m_bDecarlCoord = FALSE;
	m_bDimCoord = FALSE;
	m_bPolarCoord = FALSE;
	m_bUseZhengFanQun = FALSE;
	m_bDimPlateOutline = FALSE;
	m_bDimPlateBoltAngle = FALSE;
	m_bDimPlateRayLs = FALSE;
	m_iPartMapType = 0;
	m_bJgUseSimpleLsMap = FALSE;
	m_bDimKaiheAngleMap = FALSE;
	m_bPlateUseSimpleLsMap = FALSE;
	//}}AFX_DATA_INIT
}

CPartMapPage::~CPartMapPage()
{
}

void CPartMapPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartMapPage)
	DDX_Check(pDX, IDC_CHK_MODULATE_LONG_JG, m_bModulateLongJg);
	DDX_Text(pDX, IDC_E_LS_DIST_THRESHOLD, m_fLsDistThreshold);
	DDX_Radio(pDX, IDC_RDO_HOLE_LEN_DIM_PRECISION, m_iDimPrecision);
	DDX_Text(pDX, IDC_E_LS_DIST_ZOOM_COEF, m_fLsDistZoomCoef);
	DDX_Check(pDX, IDC_DIM_JG_G_AT_LEFT, m_bDimJgGAtLeft);
	DDX_Check(pDX, IDC_CHK_DIM_LS_SPACE, m_bDimLsSpace);
	DDX_Check(pDX, IDC_CHK_DIM_LS_POS, m_bDimLsPos);
	DDX_Check(pDX, IDC_CHK_DIM_CUT_ANGLE, m_bDimCutAngle);
	DDX_Check(pDX, IDC_CHK_DIM_CUT_ANGLE_MAP, m_bDimCutAngleMap);
	DDX_Check(pDX, IDC_CHK_DIM_PUSH_FLAT_MAP, m_bDimPushFlatMap);
	DDX_Check(pDX, IDC_CHK_DIM_KAIHE, m_bDimKaiHe);
	DDX_CBIndex(pDX, IDC_CMB_LS_POS_STYLE, m_iLsPosStyle);
	DDX_Check(pDX, IDC_CHK_DECARL_COORD, m_bDecarlCoord);
	DDX_Check(pDX, IDC_CHK_DIM_COORD, m_bDimCoord);
	DDX_Check(pDX, IDC_CHK_POLAR_COORD, m_bPolarCoord);
	DDX_Check(pDX, IDC_CHK_USE_ZHENG_FAN_QUN, m_bUseZhengFanQun);
	DDX_Check(pDX, IDC_CHK_DIM_PLATE_OUTLINE, m_bDimPlateOutline);
	DDX_Check(pDX, IDC_DIM_PLATE_BOLT_ANGLE, m_bDimPlateBoltAngle);
	DDX_Check(pDX, IDC_DIM_PLATE_RAY_LS, m_bDimPlateRayLs);
	DDX_CBIndex(pDX, IDC_CMB_PART_MAP_TYPE, m_iPartMapType);
	DDX_Check(pDX, IDC_CHK_JG_USE_SIMPLE_LS_MAP, m_bJgUseSimpleLsMap);
	DDX_Check(pDX, IDC_CHK_DIM_KAIHE_ANGLE_MAP, m_bDimKaiheAngleMap);
	DDX_Check(pDX, IDC_CHK_PLATE_USE_SIMPLE_LS_MAP, m_bPlateUseSimpleLsMap);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartMapPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPartMapPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDimPage dialog


CDimPage::CDimPage() : CPropertyPage(CDimPage::IDD)
{
	//{{AFX_DATA_INIT(CDimPage)
	m_fLsGuigeTextSize = 0.0;
	m_fPartGuigeTextSize = 0.0;
	m_fPartNoTextSize = 0.0;
	m_fJgOddmentTextSize = 0.0;
	m_fDimArrowSize = 0.0;
	m_fDimTextSize = 0.0;
	m_fDimAngleTextSize = 0.0;
	m_fHeaderTextSize = 0.0;
	m_fCommonTextSize = 0.0;
	m_fNumericTextSize = 0.0;
	m_fSimTriScale = 0.0;
	m_fOtherTextSize = 0.0;
	m_sFontName = _T("gbhzfs.shx");
	m_fPartNoCirD = 0.0;
	//}}AFX_DATA_INIT
}


void CDimPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDimPage)
	DDX_Text(pDX, IDC_E_LS_GUIGE, m_fLsGuigeTextSize);
	DDX_Text(pDX, IDC_E_PART_GUIGE, m_fPartGuigeTextSize);
	DDX_Text(pDX, IDC_E_PART_NO, m_fPartNoTextSize);
	DDX_Text(pDX, IDC_E_JG_ODDMENT, m_fJgOddmentTextSize);
	DDX_Text(pDX, IDC_E_DIM_ARROW, m_fDimArrowSize);
	DDX_Text(pDX, IDC_E_DIM_TEXT, m_fDimTextSize);
	DDX_Text(pDX, IDC_E_DIM_ANGLE_TEXT, m_fDimAngleTextSize);
	DDX_Text(pDX, IDC_E_HEADER_TEXT, m_fHeaderTextSize);
	DDX_Text(pDX, IDC_E_COMMON_TEXT, m_fCommonTextSize);
	DDX_Text(pDX, IDC_E_NUMERIC_TEXT, m_fNumericTextSize);
	DDX_Text(pDX, IDC_E_SIM_TRI_SCALE, m_fSimTriScale);
	DDX_Text(pDX, IDC_E_OTHER_TEXT, m_fOtherTextSize);
	DDX_CBString(pDX, IDC_CMB_FONT_NAME, m_sFontName);
	DDX_Text(pDX, IDC_E_PART_NO_CIR_D, m_fPartNoCirD);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDimPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDimPage)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangePartNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDimPage::OnChangePartNo() 
{
	UpdateData();
	CString sText;
	GetDlgItem(IDC_E_PART_NO)->GetWindowText(sText);
	char sTemp[10]="";
	sprintf(sTemp,"%s",sText);
	int str_len=strlen(sTemp);
	if(sTemp[str_len-1]=='.')
		return;
	m_fPartNoCirD=2*m_fPartNoTextSize;
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CDimPage message handlers

/////////////////////////////////////////////////////////////////////////////
// CPartGroupPage dialog


CPartGroupPage::CPartGroupPage() : CPropertyPage(CPartGroupPage::IDD)
{
	//{{AFX_DATA_INIT(CPartGroupPage)
	m_bDimAbsoluteDist = TRUE;
	m_bDimSpaceDist = TRUE;
	m_bModulatePartGroupLen = TRUE;
	m_bUseYOrder = TRUE;
	m_bDimCheckLine = TRUE;
	m_bAngularDimension = TRUE;
	m_bDimPlatePoleRayAndAngle = FALSE;
	m_bDimAbsoluteOutermostLayer = TRUE;
	m_fLayerSpace = 10.0;
	m_fPartGroupDistThreshold = 200.0;
	m_fPartGroupDistZoomCoef = 0.5;
	m_bDimBeforeWeldCheckLine = FALSE;
	m_bDimAfterWeldCheckLine = FALSE;
	m_bAdjustScaleByProcessCard = FALSE;
	m_bAutoDimPartNo = TRUE;
	m_bAutoMatchView = TRUE;
	//}}AFX_DATA_INIT
}


void CPartGroupPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartGroupPage)
	DDX_Check(pDX, IDC_CHK_DIM_ABSOLUTE_DIST, m_bDimAbsoluteDist);
	DDX_Check(pDX, IDC_CHK_DIM_SPACE_DIST, m_bDimSpaceDist);
	DDX_Check(pDX, IDC_CHK_MODULATE_PARTGROUP_LEN, m_bModulatePartGroupLen);
	DDX_Check(pDX, IDC_CHK_USE_Y_ORDER, m_bUseYOrder);
	DDX_Check(pDX, IDC_CHK_DIM_CHECK_LINE, m_bDimCheckLine);
	DDX_Check(pDX, IDC_CHK_ANGULAR_DIMENSION, m_bAngularDimension);
	DDX_Check(pDX, IDC_CHK_DIM_PLATE_POLE_RAYANDANGLE, m_bDimPlatePoleRayAndAngle);
	DDX_Check(pDX, IDC_CHK_DIM_ABSOLUTE_OUTERMOST_LAYER, m_bDimAbsoluteOutermostLayer);
	DDX_Text(pDX, IDC_E_LAYER_SPACE, m_fLayerSpace);
	DDX_Text(pDX, IDC_E_PARTGROUP_DIST_THRESHOLD, m_fPartGroupDistThreshold);
	DDX_Text(pDX, IDC_E_PARTGROUP_DIST_ZOOM_COEF, m_fPartGroupDistZoomCoef);
	DDX_Check(pDX, IDC_CHK_DIM_BEFORE_WELD_CHECK_LINE, m_bDimBeforeWeldCheckLine);
	DDX_Check(pDX, IDC_CHK_DIM_AFTER_WELD_CHECK_LINE, m_bDimAfterWeldCheckLine);
	DDX_Check(pDX, IDC_CHK_ADJUST_SCALE_BY_PROCESS_CARD, m_bAdjustScaleByProcessCard);
	DDX_Check(pDX, IDC_CHK_AUTO_DIM_PART_NO, m_bAutoDimPartNo);
	DDX_Check(pDX, IDC_CHK_AUTO_MATCH_VIEW, m_bAutoMatchView);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartGroupPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPartGroupPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartGroupPage message handlers

