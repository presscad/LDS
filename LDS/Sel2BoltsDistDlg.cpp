// Sel2BoltsDistDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "Sel2BoltsDistDlg.h"
#include "afxdialogex.h"
//#include "env_def.h"
#include "LicFuncDef.h"
#include "OriginalData.h"
#include "LDSView.h"


// CSel2BoltsDistDlg 对话框

//IMPLEMENT_DYNAMIC(CSel2BoltsDistDlg, CRememberLocationDlg)

CSel2BoltsDistDlg::CSel2BoltsDistDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CSel2BoltsDistDlg::IDD, pParent)
{
	m_cbDimFlag=0;
	m_fDeitaD=m_fDeitaL=m_fDeitaG=0;
	m_fChkDeitaD=m_fChkDeitaL=m_fChkDeitaG=0;
	m_bAddChkItemD=m_bAddChkItemL=m_bAddChkItemG=FALSE;
	m_pDatumBolt=m_pDestBolt=NULL;
	m_pDatumAngle=NULL;
}

CSel2BoltsDistDlg::~CSel2BoltsDistDlg()
{
}

void CSel2BoltsDistDlg::DoDataExchange(CDataExchange* pDX)
{
	CRememberLocationDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSel2BoltsDistDlg)
	DDX_Text(pDX, IDC_S_START_BOLT_H, m_shStartBolt);
	DDX_Text(pDX, IDC_S_END_BOLT_H, m_shEndBolt);
	DDX_Text(pDX, IDC_S_DATUM_ANGLE_H, m_shDatumAngle);
	DDX_Text(pDX, IDC_E_DIST, m_fDeitaD);
	DDX_Text(pDX, IDC_E_DIST_L, m_fDeitaL);
	DDX_Text(pDX, IDC_E_DIST_G, m_fDeitaG);
	DDX_Text(pDX, IDC_E_CHECK_DIST_D, m_fChkDeitaD);
	DDX_Text(pDX, IDC_E_CHECK_DIST_L, m_fChkDeitaL);
	DDX_Text(pDX, IDC_E_CHECK_DIST_G, m_fChkDeitaG);
	DDX_Check(pDX, IDC_CHK_DIST_D, m_bAddChkItemD);
	DDX_Check(pDX, IDC_CHK_DIST_L, m_bAddChkItemL);
	DDX_Check(pDX, IDC_CHK_DIST_G, m_bAddChkItemG);
	//}}AFX_DATA_MAP
}


// CSel2BoltsDistDlg 消息处理程序

BEGIN_MESSAGE_MAP(CSel2BoltsDistDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CSel2BoltsDistDlg)
	ON_BN_CLICKED(IDC_CHK_DIST_D, &CSel2BoltsDistDlg::OnBnClickedChkDistD)
	ON_BN_CLICKED(IDC_CHK_DIST_L, &CSel2BoltsDistDlg::OnBnClickedChkDistL)
	ON_BN_CLICKED(IDC_CHK_DIST_G, &CSel2BoltsDistDlg::OnBnClickedChkDistG)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSel2BoltsDistDlg::OnInitDialog()
{
	CRememberLocationDlg::OnInitDialog();
	GEPOINT vDistVec=posEnd-posStart;
	m_fDeitaD=DISTANCE(posStart,posEnd);
	if(m_pDatumBolt)
		m_shStartBolt.Format("起始螺栓:0x%X",m_pDatumBolt->handle);
	else
		m_shStartBolt="起始螺栓:0x0";
	if(m_pDestBolt)
		m_shEndBolt.Format("终止螺栓:0x%X",m_pDestBolt->handle);
	else
		m_shEndBolt="终止螺栓:0x0";
	m_shDatumAngle="基准角钢:0x0";
	if(m_pDatumAngle)
	{
		m_shDatumAngle.Format("基准角钢:0x%X",m_pDatumAngle->handle);
		GEPOINT vWorkNormal,vWingVec;
		GEPOINT vLenVec=m_pDatumAngle->End()-m_pDatumAngle->Start();
		normalize(vLenVec);
		if(abs(m_pDatumAngle->vxWingNorm*m_pDatumBolt->ucs.axis_z)>abs(m_pDatumAngle->vyWingNorm*m_pDatumBolt->ucs.axis_z))
		{
			vWorkNormal=m_pDatumAngle->vxWingNorm;
			vWingVec=m_pDatumAngle->GetWingVecX();
		}
		else
		{
			vWorkNormal=m_pDatumAngle->vyWingNorm;
			vWingVec=m_pDatumAngle->GetWingVecY();
		}
		m_fDeitaG=abs(vWingVec*vDistVec);
		m_fDeitaL=abs(vLenVec*vDistVec);
		UpdateData(FALSE);
	}
	return TRUE;
}
void CSel2BoltsDistDlg::OnBnClickedChkDistD()
{
	UpdateData();
	if(m_bAddChkItemD)
		m_cbDimFlag|=0x01;
	else
		m_cbDimFlag&=0xfe;	//设置0x01位
	m_fChkDeitaD=fto_halfi(m_fDeitaD);
	GetDlgItem(IDC_E_CHECK_DIST_D)->EnableWindow(m_bAddChkItemD);
	((CEdit*)GetDlgItem(IDC_E_CHECK_DIST_D))->SetReadOnly(!m_bAddChkItemD);
	UpdateData(FALSE);
}
void CSel2BoltsDistDlg::OnBnClickedChkDistL()
{
	UpdateData();
	if(m_bAddChkItemL)
		m_cbDimFlag|=0x02;
	else
		m_cbDimFlag&=0xfd;	//设置0x02位
	m_fChkDeitaL=fto_halfi(m_fDeitaL);
	GetDlgItem(IDC_E_CHECK_DIST_L)->EnableWindow(m_bAddChkItemL);
	((CEdit*)GetDlgItem(IDC_E_CHECK_DIST_L))->SetReadOnly(!m_bAddChkItemL);
	UpdateData(FALSE);
}
void CSel2BoltsDistDlg::OnBnClickedChkDistG()
{
	UpdateData();
	if(m_bAddChkItemG)
		m_cbDimFlag|=0x04;
	else
		m_bAddChkItemD&=0xfb;	//设置0x04位
	m_fChkDeitaG=fto_halfi(m_fDeitaG);
	GetDlgItem(IDC_E_CHECK_DIST_G)->EnableWindow(m_bAddChkItemG);
	((CEdit*)GetDlgItem(IDC_E_CHECK_DIST_G))->SetReadOnly(!m_bAddChkItemG);
	UpdateData(FALSE);
}
void CSel2BoltsDistDlg::OnOK()
{
	UpdateData();
	if(theApp.m_bManuRevisionMode&&PRODUCT_FUNC::IsHasInternalTest())
	{
		if(m_bAddChkItemD||m_bAddChkItemL||m_bAddChkItemG)//ConvertToSizeCheckKeyPoint(&m_xStartPoint,&keyPointS)&&ConvertToSizeCheckKeyPoint(&m_xEndPoint,&keyPointE))
		{
			CDataCheckModel::SIZE_DATA* pSizeCoord=xDataCheckModel.AppendCoordSizeItem();
			if(m_bAddChkItemD&&!m_bAddChkItemL&&!m_bAddChkItemG)
				pSizeCoord->ciSizeType=4;
			else if(m_bAddChkItemL||m_bAddChkItemG)
			{
				pSizeCoord->ciSizeType=5;	//分项孔距标注
				pSizeCoord->cbSizeDimFlag=0;
				if(m_bAddChkItemD)
					pSizeCoord->cbSizeDimFlag|=0x01;
				if(m_bAddChkItemL)
					pSizeCoord->cbSizeDimFlag|=0x02;
				if(m_bAddChkItemG)
					pSizeCoord->cbSizeDimFlag|=0x04;
			}
			pSizeCoord->cbSizeDimFlag=m_cbDimFlag;
			pSizeCoord->m_bCheck=TRUE;
			pSizeCoord->dist.holes.hDatumRod=m_pDatumAngle->handle;
			pSizeCoord->from.ciPointType=pSizeCoord->to.ciPointType=2;	//孔位点
			pSizeCoord->from.hole.hBolt=m_pDatumBolt->handle;
			pSizeCoord->from.hole.hDatumPart=m_pDatumAngle->handle;
			pSizeCoord->to.hole.hBolt=m_pDestBolt->handle;
			pSizeCoord->to.hole.hDatumPart=m_pDatumAngle->handle;
			int wingx0_y1=0,_wingx0_y1=0;
			m_pDatumAngle->GetBoltIntersPos(m_pDatumBolt,pSizeCoord->posFrom,&wingx0_y1);
			m_pDatumAngle->GetBoltIntersPos(m_pDestBolt,pSizeCoord->posTo,&_wingx0_y1);
			if(m_bAddChkItemD)
				pSizeCoord->dist.holes.distance=(float)m_fChkDeitaD;//(float)DISTANCE(pSizeCoord->posFrom,pSizeCoord->posTo);
			if(m_bAddChkItemL)
				pSizeCoord->dist.holes.distInLength=(float)m_fChkDeitaL;//(float)DISTANCE(pSizeCoord->posFrom,pSizeCoord->posTo);
			if(m_bAddChkItemG)
				pSizeCoord->dist.holes.distInTransverse=(float)m_fChkDeitaG;
			CLDSView::AppendSizeCoordCheckItem(pSizeCoord);
		}
	}
	CRememberLocationDlg::OnOK();
}
void CSel2BoltsDistDlg::OnCancel()
{
	CRememberLocationDlg::OnCancel();
}