// PartDrawMarkDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "PartDrawMarkDlg.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartDrawMarkDlg dialog


CPartDrawMarkDlg::CPartDrawMarkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartDrawMarkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartDrawMarkDlg)
	m_iDrawMark = 0;
	m_iResidePlane = 0;
	//}}AFX_DATA_INIT
	m_pViewPart=NULL;
	m_pRelaView=NULL;
	m_bOnlySetSpreadFace=FALSE;
}


void CPartDrawMarkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartDrawMarkDlg)
	DDX_CBIndex(pDX, IDC_CMB_DRAW_MARK, m_iDrawMark);
	DDX_CBIndex(pDX, IDC_CMB_RESIDE_PLANE, m_iResidePlane);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartDrawMarkDlg, CDialog)
	//{{AFX_MSG_MAP(CPartDrawMarkDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartDrawMarkDlg message handlers

BOOL CPartDrawMarkDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//初始化绘制方式下拉框
	CComboBox *pCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_DRAW_MARK);
	if(pCmbBox==NULL)
		return FALSE;
	pCmbBox->ResetContent();
	int i=0,i2=0,iIndex=0;
	char sDrawFlag[10]="";
	if(m_pViewPart)
		m_pViewPart->GetDrawFlagCmbStrList(m_cmbItemStrList,sDrawFlag);
	while ((i2=m_cmbItemStrList.Find('|',i)) != -1)
	{
		pCmbBox->AddString(m_cmbItemStrList.Mid(i,i2-i));
		pCmbBox->SetItemData(iIndex,sDrawFlag[iIndex]);
		iIndex++;
		i=i2+1;
	}
	if(i<m_cmbItemStrList.GetLength())
	{
		pCmbBox->AddString(m_cmbItemStrList.Right(m_cmbItemStrList.GetLength()-i));
		pCmbBox->SetItemData(iIndex,sDrawFlag[iIndex]);
	}
	if(m_pViewPart)
		m_pViewPart->GetCurDrawFlagStr(NULL,m_iDrawMark);
	if(m_iDrawMark>=0)
		pCmbBox->SetCurSel(m_iDrawMark);
	//初始化绘图平面下拉框
	pCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_RESIDE_PLANE);
	if(pCmbBox==NULL)
		return FALSE;
	pCmbBox->ResetContent();
#ifdef AFX_TARG_ENU_ENGLISH
	iIndex=pCmbBox->AddString("Datum Plane");
#else 
	iIndex=pCmbBox->AddString("基面");
#endif
	pCmbBox->SetItemData(iIndex,0);
	SEGI iSelSegI=SEGI();
	if(m_pRelaView->m_iViewType==0)
	{
		CLDSPart* pPart=m_pViewPart->GetPart();
		if(pPart) iSelSegI=pPart->iSeg;
	}
	if(m_pRelaView!=NULL&&(m_pRelaView->m_iViewType==0||m_pRelaView->m_iViewType==1))
	{
		for(CDrawingAssistant *pAssis=m_pRelaView->listAssist.GetFirst();pAssis;pAssis=m_pRelaView->listAssist.GetNext())
		{
			if(m_pRelaView->m_iViewType==0&&pAssis->m_iSeg.iSeg!=iSelSegI.iSeg)
				continue;
			if(pAssis->m_nStyle==0)
			{
				iIndex=pCmbBox->AddString(pAssis->name);
				pCmbBox->SetItemData(iIndex,pAssis->iNo);
				if(m_pViewPart&&m_pViewPart->iResidePlaneNo==pAssis->iNo)
					m_iResidePlane=iIndex;
			}
		}
	}
	GetDlgItem(IDC_CMB_DRAW_MARK)->EnableWindow(!m_bOnlySetSpreadFace);
	UpdateData(FALSE);
	return TRUE;
}

void CPartDrawMarkDlg::OnOK() 
{
	UpdateData();
	CComboBox *pCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_DRAW_MARK);
	if(pCmbBox==NULL)
		return;
	if(m_pViewPart)
	{
		if(m_iDrawMark>=0&&!m_bOnlySetSpreadFace)
			m_pViewPart->cFlag=(char)pCmbBox->GetItemData(m_iDrawMark);
		pCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_RESIDE_PLANE);
		m_pViewPart->iResidePlaneNo=pCmbBox->GetItemData(pCmbBox->GetCurSel());
	}
	CDialog::OnOK();
}
