//<LOCALE_TRANSLATE/>
// MapSizeDimDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "XhCharString.h"
#include "MapSizeDimDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapSizeDimDlg dialog


CMapSizeDimDlg::CMapSizeDimDlg(CWnd* pParent /*=NULL*/)
	:CCADCallBackDlg(CMapSizeDimDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapSizeDimDlg)
	m_iDirection = 2;
	m_fMapScale = 100.0;
	m_iPrecision = 0;
	m_fActualDist = 0;
	//}}AFX_DATA_INIT
}


void CMapSizeDimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapSizeDimDlg)
	DDX_Radio(pDX, IDC_DIMENSION_DRIECTION, m_iDirection);
	DDX_Text(pDX, IDC_E_CUR_MAPSCALE, m_fMapScale);
	DDX_Text(pDX, IDC_E_FLOAT_PRECISION, m_iPrecision);
	DDV_MinMaxInt(pDX, m_iPrecision, 0, 2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMapSizeDimDlg, CDialog)
	//{{AFX_MSG_MAP(CMapSizeDimDlg)
	ON_BN_CLICKED(IDC_BTN_SNAP_OBJ, OnBnClickedBtnSnapObj)
	ON_BN_CLICKED(IDC_DIMENSION_DRIECTION, &CMapSizeDimDlg::OnBnClickedDimensionDriection)
	ON_BN_CLICKED(IDC_VERTICAL, &CMapSizeDimDlg::OnBnClickedVertical)
	ON_BN_CLICKED(IDC_EXTEND, &CMapSizeDimDlg::OnBnClickedExtend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapSizeDimDlg message handlers
BOOL CMapSizeDimDlg::OnInitDialog()
{
	//内部启动
	CDialog::OnInitDialog();
	if(m_iDirection!=2)
		GetDlgItem(IDC_BTN_SNAP_OBJ)->ShowWindow(FALSE);
	else
		GetDlgItem(IDC_BTN_SNAP_OBJ)->ShowWindow(TRUE);
	if(m_bInernalStart&&m_iDirection==2)	
		FinishSelectObjOper();	//完成选择对象的后续操作
	UpdateData(FALSE);
	return TRUE;
}
//完成选择对象的后续操作
void CMapSizeDimDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	CAD_SCREEN_ENT* pStartCADEnt=resultList.GetByIndex(0);
	CAD_SCREEN_ENT* pEndCADEnt	=resultList.GetByIndex(1);
	if(pStartCADEnt==NULL || pEndCADEnt==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The number of selected point less then 2");
#else
		AfxMessageBox("选中点少于2个");
#endif
		return;
	}
	f3dPoint startPt,endPt;
	startPt=pStartCADEnt->m_ptPick;
	endPt=pEndCADEnt->m_ptPick;
	double map_dist=DISTANCE(startPt,endPt);
	//计算绘图比例（实际/图面）
	m_fMapScale=m_fActualDist/map_dist;
}
void CMapSizeDimDlg::OnBnClickedBtnSnapObj()
{
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	m_iBreakType=3;
	CDialog::OnOK();
}

void CMapSizeDimDlg::OnBnClickedDimensionDriection()
{
	UpdateData();
	GetDlgItem(IDC_BTN_SNAP_OBJ)->ShowWindow(FALSE);
}

void CMapSizeDimDlg::OnBnClickedVertical()
{
	UpdateData();
	GetDlgItem(IDC_BTN_SNAP_OBJ)->ShowWindow(FALSE);
}

void CMapSizeDimDlg::OnBnClickedExtend()
{
	UpdateData();
	GetDlgItem(IDC_BTN_SNAP_OBJ)->ShowWindow(TRUE);
}
