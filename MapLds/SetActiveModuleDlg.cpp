// SetActiveModuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SetActiveModuleDlg.h"
#include "Tower.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetActiveModuleDlg dialog


CSetActiveModuleDlg::CSetActiveModuleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetActiveModuleDlg::IDD, pParent)
	, m_iBodyItem(0)
{
	//{{AFX_DATA_INIT(CSetActiveModuleDlg)
	//}}AFX_DATA_INIT
}


void CSetActiveModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetActiveModuleDlg)
	DDX_CBIndex(pDX, IDC_CMB_MODULE_NO, m_iBodyItem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetActiveModuleDlg, CDialog)
	//{{AFX_MSG_MAP(CSetActiveModuleDlg)
	ON_CBN_SELCHANGE(IDC_CMB_MODULE_NO, OnCbnSelchangeCmbModuleNo)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_CMB_LEG_QUAD_A, OnCbnSelchangeCmbLegQuadA)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetActiveModuleDlg message handlers

extern CTower Ta;
BOOL CSetActiveModuleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CComboBox *pListModules=(CComboBox*)GetDlgItem(IDC_CMB_MODULE_NO);
	for(CLDSModule* pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		int index=pListModules->AddString(pModule->description);
		pListModules->SetItemData(index,pModule->handle);
		if(pModule->handle==Ta.m_hActiveModule)
			m_iBodyItem=index;	//当前激活呼高分组
	}
	if(m_iBodyItem>=0)
		pListModules->SetCurSel(m_iBodyItem);
	OnCbnSelchangeCmbModuleNo();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetActiveModuleDlg::OnCbnSelchangeCmbModuleNo()
{
	UpdateData();
	CComboBox *pListModules=(CComboBox*)GetDlgItem(IDC_CMB_MODULE_NO);
	int iCurSel=pListModules->GetCurSel();
	DWORD hModule=pListModules->GetItemData(iCurSel);
	CLDSModule* pModule=Ta.Module.FromHandle(hModule);
	CComboBox *pCmbQuadA=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_A);
	CComboBox *pCmbQuadB=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_B);
	CComboBox *pCmbQuadC=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_C);
	CComboBox *pCmbQuadD=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_D);
	if(pModule!=NULL)
	{
		pCmbQuadA->ResetContent();
		pCmbQuadB->ResetContent();
		pCmbQuadC->ResetContent();
		pCmbQuadD->ResetContent();
		Ta.m_hActiveModule=hModule;
		int nMaxLegs=CFGLEG::MaxLegs();
		int iBodyNo=pModule->GetBodyNo();
		char legMark[2]={0};
		int index,quad_act[4]={0};
		for(int i=0;i<nMaxLegs;i++)
		{
			if(pModule->m_dwLegCfgWord.And(CFGWORD(iBodyNo,i+1)))
			{
				legMark[0]='A'+i;
				index=pCmbQuadA->AddString(legMark);
				if((pModule->m_arrActiveQuadLegNo[1]-1)%nMaxLegs==i)
					quad_act[1]=index;//(iBodyNo-1)*nMaxLegs+i+1;
				index=pCmbQuadB->AddString(legMark);
				if((pModule->m_arrActiveQuadLegNo[3]-1)%nMaxLegs==i)
					quad_act[3]=index;//(iBodyNo-1)*nMaxLegs+i+1;
				index=pCmbQuadC->AddString(legMark);
				if((pModule->m_arrActiveQuadLegNo[2]-1)%nMaxLegs==i)
					quad_act[2]=index;//(iBodyNo-1)*nMaxLegs+i+1;
				index=pCmbQuadD->AddString(legMark);
				if((pModule->m_arrActiveQuadLegNo[0]-1)%nMaxLegs==i)
					quad_act[0]=index;//(iBodyNo-1)*nMaxLegs+i+1;
			}
		}
		pCmbQuadA->SetCurSel(quad_act[1]);
		pCmbQuadB->SetCurSel(quad_act[3]);
		pCmbQuadC->SetCurSel(quad_act[2]);
		pCmbQuadD->SetCurSel(quad_act[0]);
		UpdateData(FALSE);
	}
}

void CSetActiveModuleDlg::OnBnClickedOk()
{
	UpdateData();
	CComboBox *pListModules=(CComboBox*)GetDlgItem(IDC_CMB_MODULE_NO);
	int iCurSel=pListModules->GetCurSel();
	DWORD hModule=pListModules->GetItemData(iCurSel);
	CLDSModule* pModule=Ta.Module.FromHandle(hModule);
	CComboBox *pCmbQuadA=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_A);
	CComboBox *pCmbQuadB=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_B);
	CComboBox *pCmbQuadC=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_C);
	CComboBox *pCmbQuadD=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_D);
	if(pModule)
	{
		Ta.m_hActiveModule=pModule->handle;
		char legMarkStr[5]={0};
		int iBodyNo=pModule->GetBodyNo();
		int nMaxLegs=CFGLEG::MaxLegs();
		iCurSel=pCmbQuadA->GetCurSel();
		pCmbQuadA->GetLBText(iCurSel,legMarkStr);
		pModule->m_arrActiveQuadLegNo[1]=(iBodyNo-1)*nMaxLegs+legMarkStr[0]-'@';

		iCurSel=pCmbQuadB->GetCurSel();
		memset(legMarkStr,0,5);
		pCmbQuadB->GetLBText(iCurSel,legMarkStr);
		pModule->m_arrActiveQuadLegNo[3]=(iBodyNo-1)*nMaxLegs+legMarkStr[0]-'@';

		iCurSel=pCmbQuadC->GetCurSel();
		memset(legMarkStr,0,5);
		pCmbQuadC->GetLBText(iCurSel,legMarkStr);
		pModule->m_arrActiveQuadLegNo[2]=(iBodyNo-1)*nMaxLegs+legMarkStr[0]-'@';

		iCurSel=pCmbQuadD->GetCurSel();
		memset(legMarkStr,0,5);
		pCmbQuadD->GetLBText(iCurSel,legMarkStr);
		pModule->m_arrActiveQuadLegNo[0]=(iBodyNo-1)*nMaxLegs+legMarkStr[0]-'@';
	}
	OnOK();
}

void CSetActiveModuleDlg::OnCbnSelchangeCmbLegQuadA()
{
	CComboBox *pCmbQuadA=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_A);
	CComboBox *pCmbQuadB=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_B);
	CComboBox *pCmbQuadC=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_C);
	CComboBox *pCmbQuadD=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_D);
	int iCurSel=pCmbQuadA->GetCurSel();
	pCmbQuadB->SetCurSel(iCurSel);
	pCmbQuadC->SetCurSel(iCurSel);
	pCmbQuadD->SetCurSel(iCurSel);
}
