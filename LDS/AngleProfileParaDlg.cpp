// AngleProfileParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "lds_part.h"
#include "AngleProfileParaDlg.h"
#include "GlobalFunc.h"
#include "Query.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef __ANGLE_PART_INC_
/////////////////////////////////////////////////////////////////////////////
// CAngleProfileParaDlg dialog
CAngleProfileParaDlg::CAngleProfileParaDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CAngleProfileParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAngleProfileParaDlg)
	m_nG = 0;
	m_nG1 = 0;
	m_nG2 = 0;
	m_fBerSpace = 0.0;
	m_fWingSpace = 0.0;
	m_bStdProfile = FALSE;
	m_sRayAngleSize = _T("");
	m_bTwoEdgeProfile = FALSE;
	//}}AFX_DATA_INIT
	m_pRod=NULL;
	m_pBoltSet=NULL;
	x_wing0_y_wing1=0;
	m_iBerSpaceMode=1;
	m_iWingSpaceMode=1;
}

void CAngleProfileParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAngleProfileParaDlg)
	DDX_Text(pDX, IDC_E_G, m_nG);
	DDX_Text(pDX, IDC_E_G1, m_nG1);
	DDX_Text(pDX, IDC_E_G2, m_nG2);
	DDX_Text(pDX, IDC_E_BER_SPACE, m_fBerSpace);
	DDX_Text(pDX, IDC_E_WING_SPACE, m_fWingSpace);
	DDX_Check(pDX, IDC_CHK_STD_PROFILE, m_bStdProfile);
	DDX_Text(pDX, IDC_S_RAY_ANGLE_SIZE, m_sRayAngleSize);
	DDX_Check(pDX, IDC_CHK_TWO_EDGE_PROFILE, m_bTwoEdgeProfile);
	DDX_CBIndex(pDX, IDC_CMB_BER_SPACE_MODE, m_iBerSpaceMode);
	DDX_CBIndex(pDX, IDC_CMB_WING_SPACE_MODE, m_iWingSpaceMode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAngleProfileParaDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CAngleProfileParaDlg)
	ON_BN_CLICKED(IDC_CHK_STD_PROFILE, OnChkStdProfile)
	ON_EN_CHANGE(IDC_E_WING_SPACE, OnChangeWingSpace)
	ON_EN_CHANGE(IDC_E_BER_SPACE, OnChangeBerSpace)
	ON_EN_CHANGE(IDC_E_WING_SPACE, OnChangeWingSpace)
	ON_EN_CHANGE(IDC_E_BER_SPACE, OnChangeBerSpace)
	ON_CBN_SELCHANGE(IDC_CMB_BER_SPACE_MODE, OnCbnSelchangeBerSpaceMode)
	ON_CBN_SELCHANGE(IDC_CMB_WING_SPACE_MODE, OnCbnSelchangeWingSpaceMode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAngleProfileParaDlg message handlers

BOOL CAngleProfileParaDlg::OnInitDialog() 
{
	if(m_pRod==NULL)
		return FALSE;
	CDialog::OnInitDialog();
	CXhChar100 sSpec;
	m_pRod->GetSpecification(sSpec);
	m_sRayAngleSize=sSpec;
	JGZJ jgzj;
	if(m_pRod&&((CLDSLineAngle*)m_pRod)->m_bEnableTeG)
	{
		if(x_wing0_y_wing1==0)
			jgzj = ((CLDSLineAngle*)m_pRod)->xWingXZhunJu;
		else
			jgzj = ((CLDSLineAngle*)m_pRod)->xWingYZhunJu;
	}
	else
		getjgzj(jgzj,m_pRod->GetWidth());
	m_nG=jgzj.g;
	m_nG1=jgzj.g1;
	m_nG2=jgzj.g2;
	IntelliUpdateBoltSpace();
	UpdateData(FALSE);
	MoveWndToLocation();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAngleProfileParaDlg::OnChkStdProfile() 
{
	UpdateData();
	if(m_bStdProfile)
	{
		m_iWingSpaceMode=m_iBerSpaceMode=1;
		IntelliUpdateBoltSpace();
		UpdateData(FALSE);
	}
	
}
//0.指定边距;1.标准边距;2.延伸至边;3.延伸至边-厚
void CAngleProfileParaDlg::OnCbnSelchangeBerSpaceMode()
{
	UpdateData(TRUE);
	m_bStdProfile=(m_iBerSpaceMode==1&&m_iWingSpaceMode==1);
	IntelliUpdateBoltSpace();
	UpdateData(FALSE);
}

void CAngleProfileParaDlg::OnCbnSelchangeWingSpaceMode()
{
	UpdateData(TRUE);
	m_bStdProfile=(m_iBerSpaceMode==1&&m_iWingSpaceMode==1);
	IntelliUpdateBoltSpace();
	UpdateData(FALSE);
}

void  CAngleProfileParaDlg::OnChangeWingSpace()
{
	UpdateData(TRUE);
	m_iWingSpaceMode=0;
	m_bStdProfile=FALSE;
	UpdateData(FALSE);
}

void CAngleProfileParaDlg::OnChangeBerSpace()
{
	UpdateData(TRUE);
	m_iBerSpaceMode=0;
	m_bStdProfile=FALSE;
	UpdateData(FALSE);
}

void CAngleProfileParaDlg::IntelliUpdateBoltSpace() 
{
	int bolt_d=20;
	CXhPtrSet<CLDSBolt> boltSet;
	if(m_pBoltSet)
	{
		for(CLDSBolt* pBolt=m_pBoltSet->GetFirst();pBolt;pBolt=m_pBoltSet->GetNext())
		{
			if(m_pRod->FindLsByHandle(pBolt->handle))
			{
				boltSet.append(pBolt);
				bolt_d=max(bolt_d,pBolt->get_d());
			}
		}
	}
	LSSPACE_STRU LsSpace;
	if(m_iBerSpaceMode==1&&GetLsSpace(LsSpace,bolt_d))
		m_fBerSpace=LsSpace.EndSpace;
	if(m_iWingSpaceMode==1&&GetLsSpace(LsSpace,bolt_d))
		m_fWingSpace=LsSpace.EndSpace;
	if(m_iBerSpaceMode!=1 || m_iWingSpaceMode!=1)
	{
		double g,datum_g=0,max_g=0,min_g=m_pRod->GetWidth();
		f3dPoint startPos=m_pRod->Start();
		f3dPoint endPos=m_pRod->End();
		for(CLDSBolt *pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
		{
			pBolt->des_base_pos.datumPoint.UpdatePos(m_pRod->BelongModel());
			f3dPoint pos=pBolt->des_base_pos.datumPoint.Position();
			if (m_pRod->IsAngle())
			{
				CLDSLineAngle *pAngle=(CLDSLineAngle*)m_pRod;
				if(pBolt->des_base_pos.offset_wing==0)
					project_point(pos,pAngle->Start(),pAngle->get_norm_x_wing());
				else
					project_point(pos,pAngle->Start(),pAngle->get_norm_y_wing());
			}
			else if (m_pRod->GetClassTypeId()==CLS_LINEFLAT)
			{
				CLDSLineFlat *pFlat=(CLDSLineFlat*)m_pRod;
				project_point(pos,pFlat->Start(),pFlat->WorkPlaneNorm());
				startPos-=pFlat->ucs.axis_y*(m_pRod->GetWidth()*0.5);
				endPos-=pFlat->ucs.axis_y*(m_pRod->GetWidth()*0.5);
			}
			else if(m_pRod->GetClassTypeId()==CLS_LINESLOT)
			{
				CLDSLineSlot *pSlot=(CLDSLineSlot*)m_pRod;
				project_point(pos,pSlot->Start(),pSlot->WorkPlaneNorm());
			}
			datum_g=DistOf3dPtLine(pos,startPos,endPos);
			g=pBolt->des_base_pos.wing_offset_dist+datum_g;
			if(g>max_g)
				max_g=g;
			if(g<min_g)
				min_g=g;
		}
		if(m_iBerSpaceMode==0)
		{	//两侧轮廓时,如果楞线侧为指定边距,则默认为'延伸至边-肢厚' wjh-2015.2.28
			if(m_bTwoEdgeProfile)
				m_fBerSpace=ftoi(min_g-m_pRod->GetThick());
			else
				m_fBerSpace=ftoi(min_g);
		}
		else if(m_iBerSpaceMode==2)
			m_fBerSpace=ftoi(min_g);
		else if(m_iBerSpaceMode==3)
			m_fBerSpace=ftoi(min_g-m_pRod->GetThick());
		if(m_iWingSpaceMode==0 || m_iWingSpaceMode==2)
			m_fWingSpace=ftoi(m_pRod->GetWidth()-max_g);
		else if(m_iWingSpaceMode==3)
			m_fWingSpace=ftoi(m_pRod->GetWidth()-max_g-m_pRod->GetThick());
	}
}
#endif