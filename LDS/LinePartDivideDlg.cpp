//<LOCALE_TRANSLATE Confirm by hxr/>
// JgDivideDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tower.h"
#include "LinePartDivideDlg.h"
#include "resource.h"       // main symbols
#include "XhCharString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinePartDivideDlg dialog

/////////////////////////////////////////////////////////////////////////////
// CLinePartDivideDlg dialog


CLinePartDivideDlg::CLinePartDivideDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CLinePartDivideDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinePartDivideDlg)
	m_sCurItemText = _T("");
	m_fVerticalDist = 0.0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrompt = _T("along the axis direction in turn");
	m_sValType = _T("axis distance(mm)");
#else
	m_sPrompt = _T("沿坐标轴方向依次");
	m_sValType = _T("轴线距(mm)");
#endif
	m_sMutiLegMsg = _T("");
	m_fTotalHigh = 0.0;
	m_fHighRemainder = 0.0;
	m_nMirMsg = 4;
	m_iRefAxis = 2;
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCaption = "partition leg rods(according axis distance)";
#else
	m_sCaption = "塔材分段(根据轴线距离)";
#endif
	m_pCurPart = NULL;
}


void CLinePartDivideDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinePartDivideDlg)
	DDX_LBString(pDX, IDC_LIST_VERTICAL_DIST, m_sCurItemText);
	DDX_Text(pDX, IDC_EDIT_VERTICAL_DIST, m_fVerticalDist);
	DDV_MinMaxDouble(pDX, m_fVerticalDist, 0., 100000.);
	DDX_Text(pDX, IDC_STATIC_PROMPT, m_sPrompt);
	DDX_Text(pDX, IDC_STATIC_VAL_TYPE, m_sValType);
	DDX_Text(pDX, IDC_STATIC_LEG_MSG, m_sMutiLegMsg);
	DDX_Text(pDX, IDC_E_TOTAL_HIGH, m_fTotalHigh);
	DDX_Text(pDX, IDC_E_HIGH_REMAINDER, m_fHighRemainder);
	DDX_CBIndex(pDX, IDC_CMB_MIR_MSG, m_nMirMsg);
	DDX_CBIndex(pDX, IDC_CMB_MIR_AXIS, m_iRefAxis);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinePartDivideDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CLinePartDivideDlg)
	ON_BN_CLICKED(IDC_BN_ADD_ITEM, OnBnAddItem)
	ON_BN_CLICKED(IDC_BN_DELETE_ITEM, OnBnDeleteItem)
	ON_BN_CLICKED(IDC_BN_MODIFY_ITEM, OnBnModifyItem)
	ON_LBN_SELCHANGE(IDC_LIST_VERTICAL_DIST, OnSelchangeListVerticalDist)
	ON_CBN_SELCHANGE(IDC_CMB_MIR_AXIS, OnSelchangeCmbMirAxis)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinePartDivideDlg message handlers
void CLinePartDivideDlg::InitLinePart(CLDSLinePart *pLinePart)
{
	m_pCurPart = pLinePart;
	if(fabs(pLinePart->pStart->Position(false).x-pLinePart->pEnd->Position(false).x)<EPS
		&&fabs(pLinePart->pStart->Position(false).z-pLinePart->pEnd->Position(false).z)<EPS)
	{
		m_iRefAxis = 1;	//以Y轴为基准
		m_nMirMsg = 1;	//Y轴对称
	}
	else if(fabs(pLinePart->pStart->Position(false).y-pLinePart->pEnd->Position(false).y)<EPS
		&&fabs(pLinePart->pStart->Position(false).z-pLinePart->pEnd->Position(false).z)<EPS)
	{
		m_iRefAxis = 0;	//以X轴为基准
		m_nMirMsg = 2;	//X轴对称
	}
}
BOOL CLinePartDivideDlg::OnInitDialog() 
{
	m_pLB=(CListBox*)GetDlgItem(IDC_LIST_VERTICAL_DIST);
	CComboBox* pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MIR_MSG);
	pCMB->SetCurSel(0);
	UpdateData(FALSE);

	SetWindowText( m_sCaption );
	if(m_iRefAxis==0)		//沿X轴分段
		m_fTotalHigh = fabs(m_pCurPart->pEnd->Position(false).x-m_pCurPart->pStart->Position(false).x);
	else if(m_iRefAxis==1)	//沿Y轴分段
		m_fTotalHigh = fabs(m_pCurPart->pEnd->Position(false).y-m_pCurPart->pStart->Position(false).y);
	else if(m_iRefAxis==2)	//沿Z轴分段
		m_fTotalHigh = fabs(m_pCurPart->pEnd->Position(false).z-m_pCurPart->pStart->Position(false).z);
	else					//沿长度分段
		m_fTotalHigh = DISTANCE(m_pCurPart->End(),m_pCurPart->Start());
	m_fHighRemainder = m_fTotalHigh;
	MoveWndToLocation();
	return CDialog::OnInitDialog();
}

void CLinePartDivideDlg::OnSelchangeListVerticalDist() 
{
	int nCurSel = m_pLB->GetCurSel();
	UpdateData();
	m_fVerticalDist = StringToDouble(m_sCurItemText);
	UpdateData(FALSE);
	m_pLB->SetCurSel(nCurSel);
}

void CLinePartDivideDlg::OnBnAddItem() 
{
	CString TemStr;
	UpdateData();
	if(m_fHighRemainder-m_fVerticalDist<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Segment point beyond boundary，You can not segment any more!");
#else
		AfxMessageBox("分段点超界，你不能再进行分段了！");
#endif
		return;
	}
	m_fHighRemainder -= m_fVerticalDist;
	UpdateData(FALSE);

	TemStr = DoubleToString(10,m_fVerticalDist);
	if(m_pLB->GetCount()>100)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Applications can only handle data within 100");
#else
		AfxMessageBox("程序只能处理100个以内的数据");
#endif
		return;
	}
	m_pLB->AddString(TemStr);
}

void CLinePartDivideDlg::OnBnDeleteItem() 
{
	int nCurSel;
	double high;
	CString high_text;
	nCurSel = m_pLB->GetCurSel();
	if(nCurSel<0)
		return;
	m_pLB->GetText(nCurSel, high_text);
	m_pLB->DeleteString(nCurSel);
	high = atof(high_text);
	m_fHighRemainder += high;
	UpdateData(FALSE);
}

void CLinePartDivideDlg::OnBnModifyItem() 
{
	double high_prev,high_now;
	CString high_text;
	int nCurSel = m_pLB->GetCurSel();

	UpdateData();
	m_sCurItemText = DoubleToString(10,m_fVerticalDist);
	
	nCurSel = m_pLB->GetCurSel();
	m_pLB->GetText(nCurSel, high_text);
	high_prev = atof(high_text);
	high_now  = atof(m_sCurItemText);
	if(m_fHighRemainder-high_now+high_prev<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Segment point beyond boundary，You can not segment any more!");
#else
		AfxMessageBox("分段点超界，你不能再进行分段了！");
#endif
		return;
	}
	m_pLB->DeleteString(nCurSel);
	m_fHighRemainder -= (high_now-high_prev);
	m_pLB->InsertString(nCurSel, m_sCurItemText);
	m_pLB->SetCurSel(nCurSel);
	UpdateData(FALSE);
}

void CLinePartDivideDlg::OnOK() 
{
	m_nSegNum = m_pLB->GetCount();
	for(int i=0;i<m_nSegNum;i++)
	{
		CString TemStr;
		m_pLB->GetText(i, TemStr);
		VertDistList[i] = StringToDouble(TemStr);
	}
	CDialog::OnOK();
}

CString CLinePartDivideDlg::DoubleToString(int nLen, double fVerticalDist)
{
	CXhChar50 str;
	str.Printf("%f",fVerticalDist);
	str.ResizeLength(nLen,' ',true);
	return CString(str);
	/*int nStrLen;
	CString retStr;
	retStr = _gcvt(fVerticalDist,6,buffer);
	nStrLen = retStr.GetLength();
	if(nStrLen>nLen)
		retStr = retStr.Left(nLen);
	else
	{
		for(int i=0;i<nLen-nStrLen;i++)
			retStr.Insert(0," ");
	}
	return retStr;*/
}

double CLinePartDivideDlg::StringToDouble(CString &source)
{
	double retVal;
	retVal = atof(source);
	return retVal;
}

void CLinePartDivideDlg::OnSelchangeCmbMirAxis() 
{
	int old_ref_axis = m_iRefAxis;
	UpdateData();
	double used_len = m_fTotalHigh-m_fHighRemainder;
	if(m_iRefAxis==0)		//沿X轴分段
		m_fTotalHigh = fabs(m_pCurPart->pEnd->Position(false).x-m_pCurPart->pStart->Position(false).x);
	else if(m_iRefAxis==1)	//沿Y轴分段
		m_fTotalHigh = fabs(m_pCurPart->pEnd->Position(false).y-m_pCurPart->pStart->Position(false).y);
	else if(m_iRefAxis==2)	//沿Z轴分段
		m_fTotalHigh = fabs(m_pCurPart->pEnd->Position(false).z-m_pCurPart->pStart->Position(false).z);
	else if(m_iRefAxis==3)	//沿长度分段
		m_fTotalHigh = DISTANCE(m_pCurPart->End(),m_pCurPart->Start());
	m_fHighRemainder = m_fTotalHigh - used_len;
	if(m_fHighRemainder<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Segment point beyond boundary, You can't make this axis as the datum axis.");
#else
		AfxMessageBox("分段点超界，你不能以此轴为基准轴！");
#endif
		UpdateData();		//根据界面值还原实际变量值
		m_iRefAxis = old_ref_axis;
		UpdateData(FALSE);	//还原基准轴下拉框界面值
		return;
	}
	UpdateData(FALSE);
}
