// ConeTubeLjParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "ConeTubeLjParaDlg.h"
#include "env_def.h"
#include "ConeTubeParts.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 

/////////////////////////////////////////////////////////////////////////////
// CConeTubeLjParaDlg dialog

#ifdef __GGG_
CConeTubeLjParaDlg::CConeTubeLjParaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConeTubeLjParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConeTubeLjParaDlg)
	m_sType = _T("套接");
	m_fLenght = 0.0;
	m_fFlThick = 0.0;
	m_fOutterRadius = 0.0;
	m_bStartLj = FALSE;
	//}}AFX_DATA_INIT
}


void CConeTubeLjParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConeTubeLjParaDlg)
	DDX_CBString(pDX, IDC_LINK_TYPE, m_sType);
	DDX_Text(pDX, IDC_LONG, m_fLenght);
	DDX_Text(pDX, IDC_FL_THICK, m_fFlThick);
	DDX_Text(pDX, IDC_OUTTER_RADIUS, m_fOutterRadius);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConeTubeLjParaDlg, CDialog)
	//{{AFX_MSG_MAP(CConeTubeLjParaDlg)
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConeTubeLjParaDlg message handlers

void CConeTubeLjParaDlg::OnOK() 
{
	UpdateData();
	CLDSConeTube *pBaseConeTube = (CLDSConeTube *)Ta.Parts.FromHandle(m_handle,CLS_CONETUBE);
	CLDSNode *pNode=NULL;
	if(pBaseConeTube==NULL)
		return;
	if (m_bStartLj)
		pNode=pBaseConeTube->pStart;
	else
		pNode=pBaseConeTube->pEnd;
	CLDSConeTube *pRelatedTube=NULL;
	CSuperSmartPtr<CLDSPart>pPart;
	for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr(); pPart=Ta.Parts.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_CONETUBE||pPart->handle==pBaseConeTube->handle) //排除当前钢管
			continue;
		if(pPart.ConeTubePointer()->pStart!=pNode&&pPart.ConeTubePointer()->pEnd!=pNode)
			continue;
		f3dPoint len_vec1=pBaseConeTube->End()-pBaseConeTube->Start();
		f3dPoint len_vec2=pPart.ConeTubePointer()->End()-pPart.ConeTubePointer()->Start();
		normalize(len_vec1);
		normalize(len_vec2);
		if(fabs(len_vec1*len_vec2)>EPS_COS)
		{
			pRelatedTube = pPart.ConeTubePointer();
			if (m_bStartLj)//记录参与设计的构件
			{
				pBaseConeTube->m_tJointStart.hLinkObj = pRelatedTube->handle;
				pRelatedTube->m_tJointEnd.hLinkObj = pBaseConeTube->handle;
			}
			else
			{
				pBaseConeTube->m_tJointEnd.hLinkObj = pRelatedTube->handle;
				pRelatedTube->m_tJointStart.hLinkObj = pBaseConeTube->handle;
			}
			break;
		}
	}
	if (m_sType == "套接")
	{
		if (pRelatedTube)
		{
			if (m_bStartLj)
				pBaseConeTube->SetStartOdd(m_fLenght);
			else
				pRelatedTube->SetStartOdd(m_fLenght);
			if (pRelatedTube->m_tJointEnd.type != 1 && pBaseConeTube->m_tJointStart.type != 1
				&& m_bStartLj)
			{
				double thick = pBaseConeTube->size_thick;
				pRelatedTube->size_wideUpSide += thick;
				pRelatedTube->size_wide += thick;
				pRelatedTube->m_tJointEnd.type = 1;
				pBaseConeTube->m_tJointStart.type = 1;
			}
			else if(pRelatedTube->m_tJointStart.type != 1 && pBaseConeTube->m_tJointEnd.type != 1
				&&(!m_bStartLj))
			{
				double thick = pRelatedTube->size_thick;
				pBaseConeTube->size_wideUpSide += thick;
				pBaseConeTube->size_wide += thick;
				pBaseConeTube->m_tJointEnd.type = 1;
				pRelatedTube->m_tJointStart.type = 1;
			}
			pRelatedTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),0);
			g_pSolidDraw->NewSolidPart(pRelatedTube->GetSolidPartObject());
		}
	}
	else
	{
		if (pRelatedTube)
		{
			pRelatedTube->m_tJointEnd.type = 2;
			pBaseConeTube->m_tJointStart.type = 2;
			UCS_STRU ucs;
			ucs = pBaseConeTube->ucs;
			CLDSConeTubeFl *flA = (CLDSConeTubeFl *)console.AppendPart(CLS_CONETUBEFL);
			CLDSConeTubeFl	*flB = (CLDSConeTubeFl *)console.AppendPart(CLS_CONETUBEFL);
			f3dPoint basePt = pBaseConeTube->Start();
			flA->SetBasePt(basePt);
			double upWide = pBaseConeTube->size_wide;
			double downWide = upWide + (pBaseConeTube->size_wideUpSide - upWide)/pBaseConeTube->longth*30/2;
			flA->SetFaceNum(pBaseConeTube->faceN);
			flA->SetUpRadius(upWide);
			flA->SetDownRadius(downWide);
			flA->SetThick(15);
			flA->SetOutsideR(500);
			flA->handle = 123;
			flA->Create3dSolidModel(ucs);
			g_pSolidDraw->NewSolidPart(flA->GetSolidPartObject());
			
			ucs = pRelatedTube->ucs;
			basePt = basePt+ucs.axis_z*30/2;
			flB->SetBasePt(basePt);
			upWide = downWide;
			downWide = upWide + (pRelatedTube->size_wideUpSide - pRelatedTube->size_wide)/pRelatedTube->longth*30/2;
			flB->SetFaceNum(pBaseConeTube->faceN);
			flB->SetUpRadius(upWide);
			flB->SetDownRadius(downWide);
			flB->SetThick(15);
			flB->SetOutsideR(500);
			flB->handle = 124;
			flB->Create3dSolidModel(ucs);
			g_pSolidDraw->NewSolidPart(flB->GetSolidPartObject());
		}
	}
	CDialog::OnOK();
}

BOOL CConeTubeLjParaDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CLDSConeTube *pBaseConeTube = (CLDSConeTube *)Ta.Parts.FromHandle(m_handle,CLS_CONETUBE);
	if (m_bStartLj)
		m_fLenght = pBaseConeTube->startOdd();
	else
	{
		CLDSConeTube *pRelatedTube = (CLDSConeTube *)Ta.Parts.FromHandle(pBaseConeTube->m_tJointEnd.hLinkObj,CLS_CONETUBE);
		m_fLenght = pRelatedTube->startOdd();
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConeTubeLjParaDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}
#endif