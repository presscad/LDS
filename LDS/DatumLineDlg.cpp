// DatumLineDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "DatumLineDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDatumLineDlg dialog


CDatumLineDlg::CDatumLineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDatumLineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDatumLineDlg)
	m_shDatumLine = _T("");
	m_sLineDescription = _T("");
	m_sFirstPointDescrip = _T("");
	m_sSecondPointDescrip = _T("");
	m_iCmbFirstPoint = -1;
	m_iCmbSecPoint = -1;
	//}}AFX_DATA_INIT
	m_hFirstPoint=m_hSecondPoint=0;
}


void CDatumLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDatumLineDlg)
	DDX_Text(pDX, IDC_S_H_DATUM_LINE, m_shDatumLine);
	DDX_Text(pDX, IDC_E_LINE_DESCRIP, m_sLineDescription);
	DDX_Text(pDX, IDC_S_DATUM_POINT_FIR_DESCRIP, m_sFirstPointDescrip);
	DDX_Text(pDX, IDC_S_DATUM_POINT_SEC_DESCRIP, m_sSecondPointDescrip);
	DDX_CBIndex(pDX, IDC_CMB_FIRST_POINT, m_iCmbFirstPoint);
	DDX_CBIndex(pDX, IDC_CMB_SEC_POINT, m_iCmbSecPoint);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDatumLineDlg, CDialog)
	//{{AFX_MSG_MAP(CDatumLineDlg)
	ON_CBN_SELCHANGE(IDC_CMB_FIRST_POINT, OnSelchangeCmbFirstPoint)
	ON_CBN_SELCHANGE(IDC_CMB_SEC_POINT, OnSelchangeCmbSecPoint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDatumLineDlg message handlers

void CDatumLineDlg::OnOK() 
{
	UpdateData();
	if(m_hFirstPoint==m_hSecondPoint)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The handle of two endpoint of datum line is overlapping!");
#else 
		AfxMessageBox("基准线始终端基准点句柄重合!");
#endif
		return;
	}
	if( console.FromPointHandle(m_hFirstPoint)==NULL||
		console.FromPointHandle(m_hSecondPoint)==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The handle of specified datum node is illegal!");
#else 
		AfxMessageBox("指定基准点句柄非法!");
#endif
		return;
	}
	CDialog::OnOK();
}

BOOL CDatumLineDlg::OnInitDialog() 
{
	CString sPointHandle="";
	int iFirstCurSel=-1,iSecCurSel=-1;
	CComboBox *pCMBFir = (CComboBox*)GetDlgItem(IDC_CMB_FIRST_POINT);
	CComboBox *pCMBSec = (CComboBox*)GetDlgItem(IDC_CMB_SEC_POINT);
	for(CLDSPoint *pPoint=Ta.Point.GetFirst();pPoint;pPoint=Ta.Point.GetNext())
	{
		sPointHandle.Format("%X",pPoint->handle);
		int iCurSel=pCMBFir->AddString(sPointHandle);
		pCMBFir->SetItemData(iCurSel,(long)pPoint);
		iCurSel=pCMBSec->AddString(sPointHandle);
		pCMBSec->SetItemData(iCurSel,(long)pPoint);
		if(pPoint->handle==m_hFirstPoint)
		{
			iFirstCurSel=iCurSel;
			m_sFirstPointDescrip=pPoint->description;
		}
		if(pPoint->handle==m_hSecondPoint)
		{
			iSecCurSel=iCurSel;
			m_sSecondPointDescrip=pPoint->description;
		}
	}
	UpdateData(FALSE);
	pCMBFir->SetCurSel(iFirstCurSel);
	pCMBSec->SetCurSel(iSecCurSel);
	UpdateData();
	CDialog::OnInitDialog();
	return TRUE;
}

void CDatumLineDlg::OnSelchangeCmbFirstPoint() 
{
	UpdateData();
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_FIRST_POINT);
	CLDSPoint *pPoint=(CLDSPoint*)pCMB->GetItemData(pCMB->GetCurSel());
	if(pPoint)
	{
		m_sFirstPointDescrip=pPoint->description;
		m_hFirstPoint=pPoint->handle;
	}
	UpdateData(FALSE);
}

void CDatumLineDlg::OnSelchangeCmbSecPoint() 
{
	UpdateData();
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_SEC_POINT);
	CLDSPoint *pPoint=(CLDSPoint*)pCMB->GetItemData(pCMB->GetCurSel());
	if(pPoint)
	{
		m_sSecondPointDescrip=pPoint->description;
		m_hSecondPoint=pPoint->handle;
	}
	UpdateData(FALSE);
}
