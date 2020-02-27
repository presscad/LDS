// LocatePartByNoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "LocatePartByNoDlg.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLocatePartByNoDlg dialog


CLocatePartByNoDlg::CLocatePartByNoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLocatePartByNoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLocatePartByNoDlg)
	m_sPartNo = _T("");
	//}}AFX_DATA_INIT
}


void CLocatePartByNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLocatePartByNoDlg)
	DDX_Control(pDX, IDC_CMB_PART_HANDLE, m_cmbPartHandle);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLocatePartByNoDlg, CDialog)
	//{{AFX_MSG_MAP(CLocatePartByNoDlg)
	ON_BN_CLICKED(IDC_BTN_FIND_PART, OnFindPart)
	ON_CBN_SELCHANGE(IDC_CMB_PART_HANDLE, OnSelchangePartHandle)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_BTN_SELECT_ALL, OnBtnClickedSelectAll)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocatePartByNoDlg message handlers

BOOL CLocatePartByNoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n>0)
	{
		CLDSPart *pPart=console.FromPartHandle(id_arr[0]);
		if(pPart)
		{	//根据选中项设置默认件号，并查找对应构件
			m_sPartNo=pPart->GetPartNo();
			if(n>1)
			{
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(pPart->handle);
			}
			for(int i=0;i<m_cmbPartHandle.GetCount();)
				m_cmbPartHandle.DeleteString(i);
			for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
			{
				if(pPart->m_bVirtualPart)
					continue;
				else if(m_sPartNo.CompareNoCase(pPart->GetPartNo())==0)
				{
					CString sText;
					sText.Format("0X%X",pPart->handle);
					int iItem=m_cmbPartHandle.AddString(sText);
					if(pPart->handle==id_arr[0])
						m_cmbPartHandle.SetCurSel(iItem);
				}
			}
			UpdateData(FALSE);
		}
	}
	return TRUE;
}

void CLocatePartByNoDlg::OnFindPart() 
{	//根据构件编号查找相应构件
	UpdateData();
	CLDSPart *pPart=NULL;
	int i=0;
	for(i=0;i<m_cmbPartHandle.GetCount();)
		m_cmbPartHandle.DeleteString(i);
	g_pSolidDraw->ReleaseSnapStatus();
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->m_bVirtualPart)
			continue;
		else if(m_sPartNo.CompareNoCase(pPart->GetPartNo())==0)
		{
			CString sText;
			sText.Format("0X%X",pPart->handle);
			m_cmbPartHandle.AddString(sText);
		}
	}
	if(m_cmbPartHandle.GetCount()==0)
		m_cmbPartHandle.AddString("0X0");
	m_cmbPartHandle.SetCurSel(0);
	OnSelchangePartHandle();
	UpdateData(FALSE);
}

void CLocatePartByNoDlg::OnSelchangePartHandle() 
{
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	char tem_str[101]="";
	int nCurSel=m_cmbPartHandle.GetCurSel();
	m_cmbPartHandle.GetLBText(nCurSel,tem_str);
	long hCurPart=0;
	sscanf(tem_str,"%X",&hCurPart);
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(hCurPart);
	pLDSView->UpdatePropertyPage();
	//设置构件坐标原点为旋转中心点方便构件定位
	CLDSPart *pPart=console.FromPartHandle(hCurPart);
	if(pPart)
	{
		g_pSolidSet->SetOperType(OPER_ROTATE);	//设置操作类型为旋转
		f3dPoint org=pPart->ucs.origin;
		g_pSolidSet->SetRotOrg(org);
		g_pSolidSet->SetZoomStyle(ROT_CENTER);	//设置为指定旋转中心类型
		Invalidate(FALSE);
	}
	UpdateData(FALSE);
}
//<DEVELOP_PROCESS_MARK nameId="CLocatePartByNoDlg::OnBtnClickedSelectAll">
void CLocatePartByNoDlg::OnBtnClickedSelectAll()
{
	if(m_cmbPartHandle.GetCount()<=0)
		return;
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	g_pSolidDraw->ReleaseSnapStatus();
	//选中所有同件号构件
	char tem_str[101]="";
	for(int i=0;i<m_cmbPartHandle.GetCount();i++)
	{
		long hCurPart=0;
		m_cmbPartHandle.GetLBText(i,tem_str);
		sscanf(tem_str,"%X",&hCurPart);
		g_pSolidDraw->SetEntSnapStatus(hCurPart);
		if(i==0)
		{	//设置构件坐标原点为旋转中心点方便构件定位
			CLDSPart *pPart=console.FromPartHandle(hCurPart);
			if(pPart)
			{
				g_pSolidSet->SetOperType(OPER_ROTATE);	//设置操作类型为旋转
				f3dPoint org=pPart->ucs.origin;
				g_pSolidSet->SetRotOrg(org);
				g_pSolidSet->SetZoomStyle(ROT_CENTER);	//设置为指定旋转中心类型
			}
		}
	}
	pLDSView->UpdatePropertyPage();
	Invalidate(FALSE);
	CDialog::OnOK();
}
//</DEVELOP_PROCESS_MARK>
