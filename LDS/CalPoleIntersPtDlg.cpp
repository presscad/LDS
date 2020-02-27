// CalPoleIntersPtDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "lds_part.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "CalPoleIntersPtDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CCalPoleIntersPtDlg dialog
CCalPoleIntersPtDlg::CCalPoleIntersPtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCalPoleIntersPtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCalPoleIntersPtDlg)
	m_iBaseWingOffsetStyle = 0;
	m_iCurBaseWing = 0;
	m_iCurOtherWing = 0;
	m_iOtherWingOffsetStyle = 0;
	m_fBaseWingOffsetDist = 0.0;
	m_fOtherWingOffsetDist = 0.0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sBasePoleInfo = _T("Datum angle��");
	m_sOtherPoleInfo = _T("Another angle��");
	m_sCalResult = _T("Calculate result:");
#else 
	m_sBasePoleInfo = _T("��׼�Ǹ֣�");
	m_sOtherPoleInfo = _T("��һ�Ǹ֣�");
	m_sCalResult = _T("������:");
#endif
	m_fIntersX = 0.0;
	m_fIntersY = 0.0;
	m_fIntersZ = 0.0;
	//}}AFX_DATA_INIT
	m_pBasePole=m_pOtherPole=NULL;
}


void CCalPoleIntersPtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalPoleIntersPtDlg)
	DDX_CBIndex(pDX, IDC_CMB_BASE_WING_OFFSET_STYLE, m_iBaseWingOffsetStyle);
	DDX_CBIndex(pDX, IDC_CMB_CUR_BASE_WING, m_iCurBaseWing);
	DDX_CBIndex(pDX, IDC_CMB_CUR_OTHER_WING, m_iCurOtherWing);
	DDX_CBIndex(pDX, IDC_CMB_OTHER_WING_OFFSET_STYLE, m_iOtherWingOffsetStyle);
	DDX_Text(pDX, IDC_E_BASE_WING_OFFSET_DIST, m_fBaseWingOffsetDist);
	DDX_Text(pDX, IDC_E_OTHER_WING_OFFSET_DIST, m_fOtherWingOffsetDist);
	DDX_Text(pDX, IDC_S_BASE_POLE_INFO, m_sBasePoleInfo);
	DDX_Text(pDX, IDC_S_OTHER_POLE_INFO, m_sOtherPoleInfo);
	DDX_Text(pDX, IDC_E_INTERS_X, m_fIntersX);
	DDX_Text(pDX, IDC_E_INTERS_Y, m_fIntersY);
	DDX_Text(pDX, IDC_E_INTERS_Z, m_fIntersZ);
	DDX_Text(pDX, IDC_S_CAL_RESULT, m_sCalResult);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCalPoleIntersPtDlg, CDialog)
	//{{AFX_MSG_MAP(CCalPoleIntersPtDlg)
	ON_BN_CLICKED(ID_BN_CAL_INTERS, OnBnCalInters)
	ON_BN_CLICKED(ID_BN_COPY_INTERS_PT, OnBnCopyIntersPt)
	ON_CBN_SELCHANGE(IDC_CMB_CUR_BASE_WING, OnSelchangeCmbCurBaseWing)
	ON_CBN_SELCHANGE(IDC_CMB_CUR_OTHER_WING, OnSelchangeCmbCurOtherWing)
	ON_CBN_SELCHANGE(IDC_CMB_BASE_WING_OFFSET_STYLE, OnSelchangeCmbBaseWingOffsetStyle)
	ON_CBN_SELCHANGE(IDC_CMB_OTHER_WING_OFFSET_STYLE, OnSelchangeCmbOtherWingOffsetStyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalPoleIntersPtDlg message handlers

BOOL CCalPoleIntersPtDlg::OnInitDialog() 
{
	if(m_pBasePole==NULL||m_pOtherPole==NULL)
		return FALSE;
	if(m_pBasePole->GetClassTypeId()!=m_pOtherPole->GetClassTypeId())
		return FALSE;
	f3dPoint norm;
	if(m_pBasePole->GetClassTypeId()==CLS_LINEANGLE)
	{
		CLDSLineAngle *pBaseJg=(CLDSLineAngle*)m_pBasePole;
		CLDSLineAngle *pOtherJg=(CLDSLineAngle*)m_pOtherPole;
		GetWorkNorm(pBaseJg,pOtherJg,&norm);
		if(fabs(norm*pBaseJg->get_norm_x_wing())>EPS_COS2)
			m_iCurBaseWing=0;
		else
			m_iCurBaseWing=1;
		if(fabs(norm*pOtherJg->get_norm_x_wing())>EPS_COS2)
			m_iCurOtherWing=0;
		else
			m_iCurOtherWing=1;
		
		if(pBaseJg->m_bEnableTeG)
		{
			BaseZhunWingX=pBaseJg->xWingXZhunJu;
			BaseZhunWingY=pBaseJg->xWingYZhunJu;
		}
		else
		{
			getjgzj(BaseZhunWingX,pBaseJg->GetWidth());
			BaseZhunWingY=BaseZhunWingX;
		}
		if(m_iCurBaseWing==0)
		{	//X֫
			((CEdit*)GetDlgItem(IDC_E_BASE_WING_OFFSET_DIST))->SetReadOnly();
			if(m_iBaseWingOffsetStyle==0)		//g
				m_fBaseWingOffsetDist=BaseZhunWingX.g;
			else if(m_iBaseWingOffsetStyle==1)	//g1
				m_fBaseWingOffsetDist=BaseZhunWingX.g1;
			else if(m_iBaseWingOffsetStyle==2)	//g2
				m_fBaseWingOffsetDist=BaseZhunWingX.g2;
			else if(m_iBaseWingOffsetStyle==3)	//g3
				m_fBaseWingOffsetDist=BaseZhunWingX.g3;
			else
				((CEdit*)GetDlgItem(IDC_E_BASE_WING_OFFSET_DIST))->SetReadOnly(FALSE);
		}
		else//Y֫
		{
			((CEdit*)GetDlgItem(IDC_E_BASE_WING_OFFSET_DIST))->SetReadOnly();
			if(m_iBaseWingOffsetStyle==0)		//g
				m_fBaseWingOffsetDist=BaseZhunWingY.g;
			else if(m_iBaseWingOffsetStyle==1)	//g1
				m_fBaseWingOffsetDist=BaseZhunWingY.g1;
			else if(m_iBaseWingOffsetStyle==2)	//g2
				m_fBaseWingOffsetDist=BaseZhunWingY.g2;
			else if(m_iBaseWingOffsetStyle==3)	//g3
				m_fBaseWingOffsetDist=BaseZhunWingY.g3;
			else
				((CEdit*)GetDlgItem(IDC_E_BASE_WING_OFFSET_DIST))->SetReadOnly(FALSE);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		m_sBasePoleInfo.Format("Datum angle:0X%X(L%.0fX%.0f)",m_pBasePole->handle,m_pBasePole->GetWidth(),m_pBasePole->GetThick());
#else 
		m_sBasePoleInfo.Format("��׼�Ǹ�:0X%X(L%.0fX%.0f)",m_pBasePole->handle,m_pBasePole->GetWidth(),m_pBasePole->GetThick());
#endif
		if(pOtherJg->m_bEnableTeG)
		{
			OtherZhunWingX=pOtherJg->xWingXZhunJu;
			OtherZhunWingY=pOtherJg->xWingYZhunJu;
		}
		else
		{
			getjgzj(OtherZhunWingX,pOtherJg->GetWidth());
			OtherZhunWingY=OtherZhunWingX;
		}
		if(m_iCurOtherWing==0)
		{	//X֫
			((CEdit*)GetDlgItem(IDC_E_OTHER_WING_OFFSET_DIST))->SetReadOnly();
			if(m_iOtherWingOffsetStyle==0)		//g
				m_fOtherWingOffsetDist=OtherZhunWingX.g;
			else if(m_iOtherWingOffsetStyle==1)	//g1
				m_fOtherWingOffsetDist=OtherZhunWingX.g1;
			else if(m_iOtherWingOffsetStyle==2)	//g2
				m_fOtherWingOffsetDist=OtherZhunWingX.g2;
			else if(m_iOtherWingOffsetStyle==3)	//g3
				m_fOtherWingOffsetDist=OtherZhunWingX.g3;
			else
				((CEdit*)GetDlgItem(IDC_E_OTHER_WING_OFFSET_DIST))->SetReadOnly(FALSE);
		}
		else//Y֫
		{
			((CEdit*)GetDlgItem(IDC_E_OTHER_WING_OFFSET_DIST))->SetReadOnly();
			if(m_iOtherWingOffsetStyle==0)		//g
				m_fOtherWingOffsetDist=OtherZhunWingY.g;
			else if(m_iOtherWingOffsetStyle==1)	//g1
				m_fOtherWingOffsetDist=OtherZhunWingY.g1;
			else if(m_iOtherWingOffsetStyle==2)	//g2
				m_fOtherWingOffsetDist=OtherZhunWingY.g2;
			else if(m_iOtherWingOffsetStyle==3)	//g3
				m_fOtherWingOffsetDist=OtherZhunWingY.g3;
			else
				((CEdit*)GetDlgItem(IDC_E_OTHER_WING_OFFSET_DIST))->SetReadOnly(FALSE);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		m_sOtherPoleInfo.Format("Another angle:0X%X(L%.0fX%.0f)",m_pOtherPole->handle,m_pOtherPole->GetWidth(),m_pOtherPole->GetThick());
#else 
		m_sOtherPoleInfo.Format("��һ�Ǹ�:0X%X(L%.0fX%.0f)",m_pOtherPole->handle,m_pOtherPole->GetWidth(),m_pOtherPole->GetThick());
#endif
	}
	else 
	{
#ifdef AFX_TARG_ENU_ENGLISH
		CString sPoleName="Rod";
		if(m_pBasePole->GetClassTypeId()==CLS_LINETUBE)
			sPoleName.Format("Tube");
		else if(m_pBasePole->GetClassTypeId()==CLS_LINEFLAT)
			sPoleName.Format("Flat Steel");
		else if(m_pBasePole->GetClassTypeId()==CLS_LINESLOT)
			sPoleName.Format("U-Steel");
		if(m_pBasePole->GetClassTypeId()==CLS_LINETUBE)
		{
			m_sBasePoleInfo.Format("Datum%s:0X%X(��%.0fX%.0f)",sPoleName,m_pBasePole->handle,m_pBasePole->GetWidth(),m_pBasePole->GetThick());
			m_sOtherPoleInfo.Format("Another%s:0X%X(��%.0fX%.0f)",sPoleName,m_pOtherPole->handle,m_pOtherPole->GetWidth(),m_pOtherPole->GetThick());
		}
		else
		{
			m_sBasePoleInfo.Format("Datum%s:0X%X(L%.0fX%.0f)",sPoleName,m_pBasePole->handle,m_pBasePole->GetWidth(),m_pBasePole->GetThick());
			m_sOtherPoleInfo.Format("Another%s:0X%X(L%.0fX%.0f)",sPoleName,m_pOtherPole->handle,m_pOtherPole->GetWidth(),m_pOtherPole->GetThick());
		}
#else 
		CString sPoleName="�˼�";
		if(m_pBasePole->GetClassTypeId()==CLS_LINETUBE)
			sPoleName.Format("�ֹ�");
		else if(m_pBasePole->GetClassTypeId()==CLS_LINEFLAT)
			sPoleName.Format("���");
		else if(m_pBasePole->GetClassTypeId()==CLS_LINESLOT)
			sPoleName.Format("�۸�");
		if(m_pBasePole->GetClassTypeId()==CLS_LINETUBE)
		{
			m_sBasePoleInfo.Format("��׼%s:0X%X(��%.0fX%.0f)",sPoleName,m_pBasePole->handle,m_pBasePole->GetWidth(),m_pBasePole->GetThick());
			m_sOtherPoleInfo.Format("��һ%s:0X%X(��%.0fX%.0f)",sPoleName,m_pOtherPole->handle,m_pOtherPole->GetWidth(),m_pOtherPole->GetThick());
		}
		else
		{
			m_sBasePoleInfo.Format("��׼%s:0X%X(L%.0fX%.0f)",sPoleName,m_pBasePole->handle,m_pBasePole->GetWidth(),m_pBasePole->GetThick());
			m_sOtherPoleInfo.Format("��һ%s:0X%X(L%.0fX%.0f)",sPoleName,m_pOtherPole->handle,m_pOtherPole->GetWidth(),m_pOtherPole->GetThick());
		}
#endif
		GetDlgItem(IDC_CMB_CUR_BASE_WING)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMB_BASE_WING_OFFSET_STYLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMB_CUR_OTHER_WING)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMB_OTHER_WING_OFFSET_STYLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BASE_WING_OFFSET_DIST)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_OTHER_WING_OFFSET_DIST)->EnableWindow(FALSE);
	}
	UpdateData(FALSE);

	CDialog::OnInitDialog();
	OnBnCalInters();
	return TRUE;
}

void CCalPoleIntersPtDlg::OnBnCopyIntersPt() 
{
	f3dPoint point;
	UpdateData(TRUE);
	point.x = m_fIntersX;
	point.y = m_fIntersY;
	point.z = m_fIntersZ;
	WritePointToClip(point);
}

void CCalPoleIntersPtDlg::OnSelchangeCmbCurBaseWing() 
{
	UpdateData();
	if(m_iCurBaseWing==0)
	{	//X֫
		((CEdit*)GetDlgItem(IDC_E_BASE_WING_OFFSET_DIST))->SetReadOnly();
		if(m_iBaseWingOffsetStyle==0)		//g
			m_fBaseWingOffsetDist=BaseZhunWingX.g;
		else if(m_iBaseWingOffsetStyle==1)	//g1
			m_fBaseWingOffsetDist=BaseZhunWingX.g1;
		else if(m_iBaseWingOffsetStyle==2)	//g2
			m_fBaseWingOffsetDist=BaseZhunWingX.g2;
		else if(m_iBaseWingOffsetStyle==3)	//g3
			m_fBaseWingOffsetDist=BaseZhunWingX.g3;
		else
			((CEdit*)GetDlgItem(IDC_E_BASE_WING_OFFSET_DIST))->SetReadOnly(FALSE);
	}
	else//Y֫
	{
		((CEdit*)GetDlgItem(IDC_E_BASE_WING_OFFSET_DIST))->SetReadOnly();
		if(m_iBaseWingOffsetStyle==0)		//g
			m_fBaseWingOffsetDist=BaseZhunWingY.g;
		else if(m_iBaseWingOffsetStyle==1)	//g1
			m_fBaseWingOffsetDist=BaseZhunWingY.g1;
		else if(m_iBaseWingOffsetStyle==2)	//g2
			m_fBaseWingOffsetDist=BaseZhunWingY.g2;
		else if(m_iBaseWingOffsetStyle==3)	//g3
			m_fBaseWingOffsetDist=BaseZhunWingY.g3;
		else
			((CEdit*)GetDlgItem(IDC_E_BASE_WING_OFFSET_DIST))->SetReadOnly(FALSE);
	}
	UpdateData(FALSE);
}

void CCalPoleIntersPtDlg::OnSelchangeCmbCurOtherWing() 
{
	UpdateData();
	if(m_iCurOtherWing==0)
	{	//X֫
		((CEdit*)GetDlgItem(IDC_E_OTHER_WING_OFFSET_DIST))->SetReadOnly();
		if(m_iOtherWingOffsetStyle==0)		//g
			m_fOtherWingOffsetDist=OtherZhunWingX.g;
		else if(m_iOtherWingOffsetStyle==1)	//g1
			m_fOtherWingOffsetDist=OtherZhunWingX.g1;
		else if(m_iOtherWingOffsetStyle==2)	//g2
			m_fOtherWingOffsetDist=OtherZhunWingX.g2;
		else if(m_iOtherWingOffsetStyle==3)	//g3
			m_fOtherWingOffsetDist=OtherZhunWingX.g3;
		else
			((CEdit*)GetDlgItem(IDC_E_OTHER_WING_OFFSET_DIST))->SetReadOnly(FALSE);
	}
	else//Y֫
	{
		((CEdit*)GetDlgItem(IDC_E_OTHER_WING_OFFSET_DIST))->SetReadOnly();
		if(m_iOtherWingOffsetStyle==0)		//g
			m_fOtherWingOffsetDist=OtherZhunWingY.g;
		else if(m_iOtherWingOffsetStyle==1)	//g1
			m_fOtherWingOffsetDist=OtherZhunWingY.g1;
		else if(m_iOtherWingOffsetStyle==2)	//g2
			m_fOtherWingOffsetDist=OtherZhunWingY.g2;
		else if(m_iOtherWingOffsetStyle==3)	//g3
			m_fOtherWingOffsetDist=OtherZhunWingY.g3;
		else
			((CEdit*)GetDlgItem(IDC_E_OTHER_WING_OFFSET_DIST))->SetReadOnly(FALSE);
	}
	UpdateData(FALSE);
}

void CCalPoleIntersPtDlg::OnSelchangeCmbBaseWingOffsetStyle() 
{
	UpdateData();
	if(m_iCurBaseWing==0)
	{	//X֫
		((CEdit*)GetDlgItem(IDC_E_BASE_WING_OFFSET_DIST))->SetReadOnly();
		if(m_iBaseWingOffsetStyle==0)		//g
			m_fBaseWingOffsetDist=BaseZhunWingX.g;
		else if(m_iBaseWingOffsetStyle==1)	//g1
			m_fBaseWingOffsetDist=BaseZhunWingX.g1;
		else if(m_iBaseWingOffsetStyle==2)	//g2
			m_fBaseWingOffsetDist=BaseZhunWingX.g2;
		else if(m_iBaseWingOffsetStyle==3)	//g3
			m_fBaseWingOffsetDist=BaseZhunWingX.g3;
		else
			((CEdit*)GetDlgItem(IDC_E_BASE_WING_OFFSET_DIST))->SetReadOnly(FALSE);
	}
	else//Y֫
	{
		((CEdit*)GetDlgItem(IDC_E_BASE_WING_OFFSET_DIST))->SetReadOnly();
		if(m_iBaseWingOffsetStyle==0)		//g
			m_fBaseWingOffsetDist=BaseZhunWingY.g;
		else if(m_iBaseWingOffsetStyle==1)	//g1
			m_fBaseWingOffsetDist=BaseZhunWingY.g1;
		else if(m_iBaseWingOffsetStyle==2)	//g2
			m_fBaseWingOffsetDist=BaseZhunWingY.g2;
		else if(m_iBaseWingOffsetStyle==3)	//g3
			m_fBaseWingOffsetDist=BaseZhunWingY.g3;
		else
			((CEdit*)GetDlgItem(IDC_E_BASE_WING_OFFSET_DIST))->SetReadOnly(FALSE);
	}
	UpdateData(FALSE);
}

void CCalPoleIntersPtDlg::OnSelchangeCmbOtherWingOffsetStyle() 
{
	UpdateData();
	if(m_iCurOtherWing==0)
	{	//X֫
		((CEdit*)GetDlgItem(IDC_E_OTHER_WING_OFFSET_DIST))->SetReadOnly();
		if(m_iOtherWingOffsetStyle==0)		//g
			m_fOtherWingOffsetDist=OtherZhunWingX.g;
		else if(m_iOtherWingOffsetStyle==1)	//g1
			m_fOtherWingOffsetDist=OtherZhunWingX.g1;
		else if(m_iOtherWingOffsetStyle==2)	//g2
			m_fOtherWingOffsetDist=OtherZhunWingX.g2;
		else if(m_iOtherWingOffsetStyle==3)	//g3
			m_fOtherWingOffsetDist=OtherZhunWingX.g3;
		else
			((CEdit*)GetDlgItem(IDC_E_OTHER_WING_OFFSET_DIST))->SetReadOnly(FALSE);
	}
	else//Y֫
	{
		((CEdit*)GetDlgItem(IDC_E_OTHER_WING_OFFSET_DIST))->SetReadOnly();
		if(m_iOtherWingOffsetStyle==0)		//g
			m_fOtherWingOffsetDist=OtherZhunWingY.g;
		else if(m_iOtherWingOffsetStyle==1)	//g1
			m_fOtherWingOffsetDist=OtherZhunWingY.g1;
		else if(m_iOtherWingOffsetStyle==2)	//g2
			m_fOtherWingOffsetDist=OtherZhunWingY.g2;
		else if(m_iOtherWingOffsetStyle==3)	//g3
			m_fOtherWingOffsetDist=OtherZhunWingY.g3;
		else
			((CEdit*)GetDlgItem(IDC_E_OTHER_WING_OFFSET_DIST))->SetReadOnly(FALSE);
	}
	UpdateData(FALSE);
}

void CCalPoleIntersPtDlg::OnBnCalInters() 
{
	f3dPoint base_vec,other_vec,base_pt,base_norm,inters_pt;
	f3dLine base_zhun_line,other_zhun_line;
	UpdateData();
	if(m_pBasePole->GetClassTypeId()==CLS_LINEANGLE)
	{
		CLDSLineAngle *pBaseJg=(CLDSLineAngle*)m_pBasePole;
		CLDSLineAngle *pOtherJg=(CLDSLineAngle*)m_pOtherPole;
		f3dPoint base_line_vec=pBaseJg->End()-pBaseJg->Start();
		f3dPoint other_line_vec=pOtherJg->End()-pOtherJg->Start();
		base_norm=base_line_vec^other_line_vec;
		normalize(base_norm);
		if(m_iCurBaseWing==0)	//X֫
			base_vec=pBaseJg->GetWingVecX();
		else	//Y֫
			base_vec=pBaseJg->GetWingVecY();
		base_pt=pBaseJg->Start();
		base_zhun_line.startPt=pBaseJg->Start()+base_vec*m_fBaseWingOffsetDist;
		base_zhun_line.endPt=pBaseJg->End()+base_vec*m_fBaseWingOffsetDist;
		
		if(m_iCurOtherWing==0)	//X֫
			other_vec=pOtherJg->GetWingVecX();
		else	//Y֫
			other_vec=pOtherJg->GetWingVecY();
		other_zhun_line.startPt=pOtherJg->Start()+other_vec*m_fOtherWingOffsetDist;
		other_zhun_line.endPt=pOtherJg->End()+other_vec*m_fOtherWingOffsetDist;
		project_point(other_zhun_line.startPt,base_pt,base_norm);
		project_point(other_zhun_line.endPt,base_pt,base_norm);
	}
	else
	{
		base_pt=m_pBasePole->Start();
		base_zhun_line.startPt=m_pBasePole->Start();
		base_zhun_line.endPt=m_pBasePole->End();
		other_zhun_line.startPt=m_pOtherPole->Start();
		other_zhun_line.endPt=m_pOtherPole->End();
	}
	int ret=Int3dll(base_zhun_line,other_zhun_line,inters_pt);
	m_fIntersX=inters_pt.x;
	m_fIntersY=inters_pt.y;
	m_fIntersZ=inters_pt.z;
	switch(ret)
	{
#ifdef AFX_TARG_ENU_ENGLISH
	case -2:	//������Ч��ֱ����㣩
		m_sCalResult="Calculate result��The intersectant point is on the outside of line";
		break;
	case -1:	//ֱ�߶˵��غϣ���������
		m_sCalResult="Calculate result��Line's endpoint is overlap, the definition is wrong";
		break;
	case  0:	//L1��L2ƽ�С��غϻ�����
		m_sCalResult="Calculate result��L1 and L2 are parallel,overlapping or different surface";
		break;
	case  1:	//����Ϊ��ֱ�߶ε��ڵ�
		m_sCalResult="Calculate result��The intersectant point is the inner point of two line";
		break;
	case  2:	//����Ϊֱ�߶ζ˵�
		m_sCalResult="Calculate result��The intersectant point is line's endpoint";
		break;
	default:
		m_sCalResult="Calculate result��Calculate error";
		break;
#else 
	case -2:	//������Ч��ֱ����㣩
		m_sCalResult="��������ֱ�߶����";
		break;
	case -1:	//ֱ�߶˵��غϣ���������
		m_sCalResult="���������˵��غϣ���������";
		break;
	case  0:	//L1��L2ƽ�С��غϻ�����
		m_sCalResult="��������ֱ��ƽ�С��غϻ�����";
		break;
	case  1:	//����Ϊ��ֱ�߶ε��ڵ�
		m_sCalResult="����������ֱ�߶ε��ڵ�";
		break;
	case  2:	//����Ϊֱ�߶ζ˵�
		m_sCalResult="��������ֱ�߶ζ˵�";
		break;
	default:
		m_sCalResult="���������������쳣";
		break;
#endif
	}
	UpdateData(FALSE);
}
#endif