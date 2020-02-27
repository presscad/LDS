//<LOCALE_TRANSLATE Confirm by hxr/>
// UpdateNormDLg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "UniformNormDlg.h"


// CUpdateNormDLg 对话框

IMPLEMENT_DYNAMIC(CUniformNormDLg, CDialog)
CUniformNormDLg::CUniformNormDLg(CWnd* pParent /*=NULL*/)
	: CDialog(CUniformNormDLg::IDD, pParent)
	, m_iNormStyle(0)
	, m_fVectorX(0)
	, m_fVectorY(0)
	, m_fVectorZ(0)
	
{
}

CUniformNormDLg::~CUniformNormDLg()
{
}

void CUniformNormDLg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_CMB_NORM_STYLE, m_iNormStyle);
	DDX_Text(pDX, IDC_E_X_VALUE, m_fVectorX);
	DDX_Text(pDX, IDC_E_Y_VALUE, m_fVectorY);
	DDX_Text(pDX, IDC_E_Z_VALUE, m_fVectorZ);
}
BEGIN_MESSAGE_MAP(CUniformNormDLg, CDialog)
	//{{AFX_MSG_MAP(CUpdateNormDLg)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_CMB_NORM_STYLE, OnCbnSelchangeCmbNormStyle)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////
BOOL CUniformNormDLg::OnInitDialog()
{
	CDialog::OnInitDialog();
	((CEdit*)GetDlgItem(IDC_E_X_VALUE))->SetReadOnly(m_iNormStyle!=0);
	((CEdit*)GetDlgItem(IDC_E_Y_VALUE))->SetReadOnly(m_iNormStyle!=0);
	((CEdit*)GetDlgItem(IDC_E_Z_VALUE))->SetReadOnly(m_iNormStyle!=0);
	UpdateMap();
	return TRUE;
}

void CUniformNormDLg::OnCbnSelchangeCmbNormStyle()
{
	UpdateData();
	((CEdit*)GetDlgItem(IDC_E_X_VALUE))->SetReadOnly(m_iNormStyle!=0);
	((CEdit*)GetDlgItem(IDC_E_Y_VALUE))->SetReadOnly(m_iNormStyle!=0);
	((CEdit*)GetDlgItem(IDC_E_Z_VALUE))->SetReadOnly(m_iNormStyle!=0);
	m_fVectorX=m_fVectorY=m_fVectorZ=0;
	if(m_iNormStyle==1)			//X+
		m_fVectorX=1;
	else if(m_iNormStyle==2)	//X-
		m_fVectorX=-1;
	else if(m_iNormStyle==3)	//Y+
		m_fVectorY=1;
	else if(m_iNormStyle==4)	//Y-
		m_fVectorY=-1;
	else if(m_iNormStyle==5)	//Z+
		m_fVectorZ=1;
	else if(m_iNormStyle==6)	//Z-
		m_fVectorZ=-1;
	UpdateMap();
	UpdateData(FALSE);
}
void CUniformNormDLg::UpdateMap()
{
	m_bitMap.Detach();
	if(m_iNormStyle==0)
		m_bitMap.LoadBitmap(IDB_BMP_NORM_COORDINATE);
	else if(m_iNormStyle==1)	//1.X+
		m_bitMap.LoadBitmap(IDB_BMP_NORM_COORDINATE1);
	else if(m_iNormStyle==2)	//2.X-
		m_bitMap.LoadBitmap(IDB_BMP_NORM_COORDINATE2);
	else if(m_iNormStyle==3)	//3.Y+
		m_bitMap.LoadBitmap(IDB_BMP_NORM_COORDINATE5);
	else if(m_iNormStyle==4)	//4.Y-
		m_bitMap.LoadBitmap(IDB_BMP_NORM_COORDINATE6);
	else if(m_iNormStyle==5)	//5.Z+
		m_bitMap.LoadBitmap(IDB_BMP_NORM_COORDINATE3);
	else if(m_iNormStyle==6)	//6.Z-
		m_bitMap.LoadBitmap(IDB_BMP_NORM_COORDINATE4);
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_COORD_BITMAP);
	pPic->SetBitmap(m_bitMap);
}
void CUniformNormDLg::OnOK()
{
	UpdateData();
	norm.Set(m_fVectorX,m_fVectorY,m_fVectorZ);
	if(norm.IsZero())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("the specified normal line direction is wrong!");
#else
		AfxMessageBox("指定的法线方向不正确!");
#endif
		return;
	}
	if(m_iNormStyle==0)	//指定方向
		normalize(norm);
	CDialog::OnOK();
}