//DesHangFittingDlg.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "afxdialogex.h"
#include "DesHangFittingDlg.h"

//////////////////////////////////////////////////////////////////////////
// CDesHangFittingDlg �Ի���
IMPLEMENT_DYNAMIC(CDesHangFittingDlg, CDialogEx)
int CDesHangFittingDlg::nScrLocationX=0;
int CDesHangFittingDlg::nScrLocationY=0;
CDesHangFittingDlg::CDesHangFittingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDesHangFittingDlg::IDD, pParent)
{
	m_iCurSel=-1;
}

CDesHangFittingDlg::~CDesHangFittingDlg()
{
}

void CDesHangFittingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_xPropertyList);
	DDX_Control(pDX, IDC_WIREPOINT_LIST, m_xWirePtListCtrl);
	DDX_Control(pDX, IDC_CONNPOINT_LIST, m_xConnPtListCtrl);
}


BEGIN_MESSAGE_MAP(CDesHangFittingDlg, CDialog)
	ON_WM_MOVE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_EXTRACT_WIREPT, OnBnExtractWirePoints)
END_MESSAGE_MAP()


// CDesHangFittingDlg ��Ϣ�������
BOOL CDesHangFittingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//
	m_xWirePtListCtrl.AddColumnHeader("�ҵ����",60);
	m_xWirePtListCtrl.AddColumnHeader("�׾�",50);
	m_xWirePtListCtrl.AddColumnHeader("�������",60);
	m_xWirePtListCtrl.InitListCtrl();
	m_xWirePtListCtrl.Redraw();
	//
	m_xConnPtListCtrl.AddColumnHeader("���",40);
	m_xConnPtListCtrl.AddColumnHeader("��·",40);
	m_xConnPtListCtrl.AddColumnHeader("����",40);
	m_xConnPtListCtrl.AddColumnHeader("����",50);
	m_xConnPtListCtrl.AddColumnHeader("����",50);
	m_xConnPtListCtrl.AddColumnHeader("�ҵ�",60);
	m_xConnPtListCtrl.InitListCtrl();
	//�ƶ��Ի������Ͻ�
	CRect rect;
	GetWindowRect(rect);
	int width=rect.Width();
	int height=rect.Height();
	rect.left = nScrLocationX;
	rect.top = nScrLocationY;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;
	MoveWindow(rect,TRUE);
	m_bEnableWindowMoveListen=TRUE;
	UpdateData(FALSE);
	return TRUE;
}
void CDesHangFittingDlg::RefreshListCtrl()
{
}

BOOL CDesHangFittingDlg::FinishSelectObjOper(long hPickObj,long idEventProp)
{
	return TRUE;
}
//��Ϣ��Ӧ�¼�
void CDesHangFittingDlg::OnBnExtractWirePoints()
{
	
}
int CDesHangFittingDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen=FALSE;
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
void CDesHangFittingDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);

	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}
