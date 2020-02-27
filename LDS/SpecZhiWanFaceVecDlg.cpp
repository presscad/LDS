//<LOCALE_TRANSLATE Confirm by hxr/>
// SpecZhiWanFaceVecDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "SpecZhiWanFaceVecDlg.h"
#include "GlobalFunc.h"


// CSpecZhiWanFaceVecDlg �Ի���

IMPLEMENT_DYNAMIC(CSpecZhiWanFaceVecDlg, CDialog)
CSpecZhiWanFaceVecDlg::CSpecZhiWanFaceVecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpecZhiWanFaceVecDlg::IDD, pParent)
	, m_fNormX(0)
	, m_fNormY(1)
	, m_fNormZ(0)
	, m_fVectorX(0)
	, m_fVectorY(1)
	, m_fVectorZ(0)
{
}

CSpecZhiWanFaceVecDlg::~CSpecZhiWanFaceVecDlg()
{
}

void CSpecZhiWanFaceVecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRollPlankEdgeDlg)
	DDX_Text(pDX, IDC_E_FACE_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_FACE_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_FACE_NORM_Z, m_fNormZ);
	DDX_Text(pDX, IDC_E_VEC_X, m_fVectorX);
	DDX_Text(pDX, IDC_E_VEC_Y, m_fVectorY);
	DDX_Text(pDX, IDC_E_VEC_Z, m_fVectorZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpecZhiWanFaceVecDlg, CDialog)
	ON_BN_CLICKED(IDC_BN_PASTE_NORM, OnBnPasteNorm)
	ON_BN_CLICKED(IDC_BN_PASTE_VEC, OnBnPasteVec)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSpecZhiWanFaceVecDlg ��Ϣ�������

void CSpecZhiWanFaceVecDlg::OnBnPasteNorm()
{
	f3dPoint point;
	UpdateData(TRUE);
	ReadClipPoint(point);
	m_fNormX = point.x;
	m_fNormY = point.y;
	m_fNormZ = point.z;
	UpdateData(FALSE);
}

void CSpecZhiWanFaceVecDlg::OnBnPasteVec()
{
	f3dPoint point;
	UpdateData(TRUE);
	ReadClipPoint(point);
	m_fVectorX = point.x;
	m_fVectorY = point.y;
	m_fVectorZ = point.z;
	UpdateData(FALSE);
}

void CSpecZhiWanFaceVecDlg::OnBnClickedOk()
{
	if(fabs(m_fNormX)+fabs(m_fNormY)+fabs(m_fNormZ)<EPS)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("the normal line of blend plane is empty,please input again!");
#else
		AfxMessageBox("�����淨��Ϊ��,����������!");
#endif
		return;
	}
	else if(fabs(m_fVectorX)+fabs(m_fVectorY)+fabs(m_fVectorZ)<EPS)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("the direction vector of blend line is empty,please input again!");
#else
		AfxMessageBox("�����߷���ʸ��Ϊ��,����������!");
#endif
		return;
	}
	OnOK();
}
