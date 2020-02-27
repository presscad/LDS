// SpreadVectorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "SpreadVectorDlg.h"
#include "afxdialogex.h"
#include "env_def.h"

// CSpreadVectorDlg 对话框

IMPLEMENT_DYNAMIC(CSpreadVectorDlg, CDialogEx)

CSpreadVectorDlg::CSpreadVectorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSpreadVectorDlg::IDD, pParent)
{

	m_fVectorX = 0.0;
	m_fVectorY = 0.0;
	m_fVectorZ = 0.0;
}

CSpreadVectorDlg::~CSpreadVectorDlg()
{
}

void CSpreadVectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_VEC_X, m_fVectorX);
	DDX_Text(pDX, IDC_E_VEC_Y, m_fVectorY);
	DDX_Text(pDX, IDC_E_VEC_Z, m_fVectorZ);
}


BEGIN_MESSAGE_MAP(CSpreadVectorDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BN_REVERSAL_VEC, &CSpreadVectorDlg::OnBnClickedBnReversalVec)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


void CSpreadVectorDlg::SetPlaneNormal(GEPOINT normal){
	if(fabs(normal.x)<EPS)
		normal.x=0;	//防止输入值为-0  wht 10-11-09
	if(fabs(normal.y)<EPS)
		normal.y=0;
	if(fabs(normal.z)<EPS)
		normal.z=0;
	m_fVectorX=normal.x;
	m_fVectorY=normal.y;
	m_fVectorZ=normal.z;
}
GEPOINT CSpreadVectorDlg::GetPlaneNormal(){
	return GEPOINT(m_fVectorX,m_fVectorY,m_fVectorZ);
}
// CSpreadVectorDlg 消息处理程序
void CSpreadVectorDlg::OnBnClickedBnReversalVec()
{
	UpdateData();
	m_fVectorX *= -1;
	m_fVectorY *= -1;
	m_fVectorZ *= -1;
	UpdateData(FALSE);
	g_pSolidDraw->DelWorkPlaneSketch(1);
	//输入面法向方向与视线方向相反
	g_pSolidDraw->NewWorkPlaneSketch(1,RGB(100,100,100),m_arrPlaneVertex,
		m_arrPlaneVertex.Size(),GEPOINT(-m_fVectorX,-m_fVectorY,-m_fVectorZ),"",150,false);
	g_pSolidDraw->Draw();
}

void CSpreadVectorDlg::InitPlaneVertex(GEPOINT *pVertexArr,int count)
{
	m_arrPlaneVertex.Resize(count);
	for(int i=0;i<count;i++)
		m_arrPlaneVertex[i]=pVertexArr[i];
}

void CSpreadVectorDlg::InitPlaneVertex(f3dPoint *pVertexArr,int count)
{
	m_arrPlaneVertex.Resize(count);
	for(int i=0;i<count;i++)
		m_arrPlaneVertex[i].Set(pVertexArr[i].x,pVertexArr[i].y,pVertexArr[i].z);
}

BOOL CSpreadVectorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//输入面法向方向与视线方向相反
	g_pSolidDraw->NewWorkPlaneSketch(1,RGB(100,100,100),m_arrPlaneVertex,
		m_arrPlaneVertex.Size(),GEPOINT(-m_fVectorX,-m_fVectorY,-m_fVectorZ),"",150,false);
	g_pSolidDraw->Draw();
	return TRUE;
}
void CSpreadVectorDlg::OnDestroy()
{
	g_pSolidDraw->DelWorkPlaneSketch(1);
	CDialogEx::OnDestroy();
}
