// CalPerfectSectsDlg.cpp : ʵ���ļ�
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "CalPerfectSectsDlg.h"
#include "GlobalFunc.h"
#include "afxdialogex.h"
#include "./Codes/Codes.h"

#ifdef __PART_DESIGN_INC_
// CCalPerfectSectsDlg �Ի���

IMPLEMENT_DYNAMIC(CCalPerfectSectsDlg, CDialog)

CCalPerfectSectsDlg::CCalPerfectSectsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCalPerfectSectsDlg::IDD, pParent)
{
	m_pRod=NULL;
	m_pSteel=NULL;
	m_fLsCoef=2;
	m_fR=0;	
	m_fRx=0;			
	m_nBoltD=0;		
	m_fGrossArea=0;	
	m_fNetArea=0;
	m_cSectionType='B';
}

CCalPerfectSectsDlg::~CCalPerfectSectsDlg()
{
}

void CCalPerfectSectsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_CMB_BOLT_SIZE, m_sLsGuiGe);
	DDX_CBString(pDX, IDC_CMB_SECT_NUM, m_sSectNum);
	DDX_Text(pDX, IDC_E_HOLE_OUT, m_sLsCoef);
	DDX_Text(pDX, IDC_E_COMPSTABLE_SIGEMA, m_sStress);
}


BEGIN_MESSAGE_MAP(CCalPerfectSectsDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_CMB_BOLT_SIZE, OnChangeCmbLsGuige)
	ON_CBN_EDITCHANGE(IDC_CMB_BOLT_SIZE, OnChangeCmbLsGuige)
	ON_CBN_SELCHANGE(IDC_CMB_SECT_NUM, OnChangeCmbSectNum)
	ON_CBN_EDITCHANGE(IDC_CMB_SECT_NUM, OnChangeCmbSectNum)
	ON_EN_CHANGE(IDC_E_HOLE_OUT, OnChangeEHoleOut)
END_MESSAGE_MAP()


// CCalPerfectSectsDlg ��Ϣ�������
BOOL CCalPerfectSectsDlg::OnInitDialog()
{
	if(m_pRod==NULL)
		return FALSE;
	m_pSteel=QuerySteelMatByBriefMark(m_pRod->cMaterial);	//�˼���Ӧ������
	m_fGrossArea=m_pRod->GetArea();			//ë�������mm2
	if(m_pRod->size_idClassType==CLS_LINEANGLE)
	{
		JG_PARA_TYPE* pJgType=FindJgType(m_pRod->GetWidth(),m_pRod->GetThick(),m_pRod->size_cSubClassType);
		if(pJgType==NULL)
			return FALSE;
		//������˨ֱ��
		m_nBoltD=SelBoltDByWingWide(m_pRod->GetWidth());
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,ftoi(m_nBoltD));
		//�Զ��жϸ˼��ϵ���˨����
		int ls_n=max(m_pRod->connectStart.wnSummBoltN,m_pRod->connectEnd.wnSummBoltN);
		int nMaxLsRows;
		if(ls_n<5)
			nMaxLsRows=1;
		else
			nMaxLsRows=2;
		//����Ǹ��ϵ���˨����Ӱ��
		JGZJ jgzj;
		getjgzj(jgzj,m_pRod->GetWidth());
		if(nMaxLsRows==1||jgzj.g2-jgzj.g1==0)
		{	//��˨���Ų���
			double lsn_coef1=1;	//��Ҫ������֫ͬ����˨������
			double g=jgzj.g*2-m_pRod->GetThick();	//��Ӧ�涨:g=2d1-t
			double lsn_coef2=2-pow(LsSpace.SingleRowSpace/2.0,2)/(4*g*m_nBoltD);
			m_fLsCoef=max(lsn_coef1,lsn_coef2);
		}
		else //if(nMaxLsRows==2)
		{	//��˨˫�Ų���ʱ
			double lsn_coef1=2;
			double lsn_coef2=3-pow(LsSpace.doubleRowSpace/2.0,2)/(4*(jgzj.g2-jgzj.g1)*m_nBoltD);
			double g=jgzj.g1*2-m_pRod->GetThick();	//��Ӧ�涨:g=2d1-t
			double lsn_coef3=4-pow(LsSpace.doubleRowSpace/2.0,2)*
				(2.0/(jgzj.g2-jgzj.g1)+1/g)/(4*m_nBoltD);
			m_fLsCoef=max(lsn_coef1,lsn_coef2);
			m_fLsCoef=max(m_fLsCoef,lsn_coef3);
		}
		if(toupper(pJgType->cType)=='T'||toupper(pJgType->cType)=='D')
			m_fLsCoef*=2;
		else if(toupper(pJgType->cType)=='X')
			m_fLsCoef*=4;
		//���㾻���
		m_fNetArea=m_fGrossArea-m_fLsCoef*m_nBoltD*m_pRod->GetThick();	//�۳���˨��
		//��ת�뾶����Բ���뾶
		m_fRx=pJgType->Rx*10;	//��ת�뾶 mm
		m_fR=pJgType->r;		//��Բ���뾶
	}
	else if(m_pRod->size_idClassType==CLS_LINETUBE)
	{
		/*TUBE_PARA_TYPE* pTubeType=FindTubeType(m_pRod->GetWidth(),m_pRod->GetThick());
		if(pTubeType==NULL)
			return FALSE;
		if(m_pRod->size_wide<g_sysPara.fTubeWeldRoadMinD)
			m_cSectionType='A';*/
		AfxMessageBox("��ʱ��֧�ָֹܵĽڼ���ѡ����!");
		return FALSE;
	}
	m_sLsCoef.Format("%.2f",m_fLsCoef);
	m_sLsGuiGe.Format("M%d",m_nBoltD);
	OptimizeSectCounts();		//����ѹ��Ӧ��
	m_sSectNum.Format("%d",m_nSectNum);
	CDialog::OnInitDialog();
	return TRUE;
}
double CCalPerfectSectsDlg::VerityCompStableStress()
{
	//1���������ѹ��
	double maxCompForce=m_fNetArea*m_pSteel->f(m_pRod->GetThick());	//�����/ѹ��
	//2���������ĸ˼���ѹ�ȶ�ϵ��
	IMemberDesignCode* pDesignCode=IMemberDesignCode::FromCodeSerial(Ta.m_iMemberSizeCheckCode);
	double length=DISTANCE(m_pRod->pStart->Position(false),m_pRod->pEnd->Position(false))/m_nSectNum;
	double lamda=length/m_fRx;	//��ϸ��
	double fai=pDesignCode->CompStableCoef(m_cSectionType,lamda,m_pSteel,m_pRod->GetThick());	//��ѹ�ȶ�ϵ��
	//3������ѹ���ȶ�ǿ���ۼ�ϵ��
	double fy=m_pSteel->fy(m_pRod->GetThick());	//����ǿ��
	double lamda1=lamda;
	if(lamda<30)
		lamda1=30;
	else if(lamda>100)
		lamda1=100;
	double lim=(10+0.1*lamda1)*sqrt(235/fy);
	double b=m_pRod->GetWidth()-m_pRod->GetThick()-m_fR;
	double fValue=b/m_pRod->GetThick();
	double mn=1;
	if(fValue>lim && fValue<=380/sqrt(fy))
		mn=1.677-0.677*(fValue/lim);
	//4������ѹ��Ӧ��
	double fStress=maxCompForce/(fai*m_fGrossArea*mn);
	return fStress;
}
void CCalPerfectSectsDlg::OptimizeSectCounts()
{
	double fStableStress=0;
	double fMaxStress=m_pSteel->f(m_pRod->GetThick());
	m_nSectNum=0;
	do{
		m_nSectNum++;
		fStableStress=VerityCompStableStress();
	}while(fStableStress>fMaxStress);
#ifdef AFX_TARG_ENU_ENGLISH
	m_sStress.Format("ѹ��Ӧ��(%.2fMPa)<ultimate strength(%.0fMPa)",fStableStress,fMaxStress);
#else 
	m_sStress.Format("ѹ��Ӧ��(%.2fMPa)<����ǿ��(%.0fMPa)",fStableStress,fMaxStress);
#endif
	UpdateData(FALSE);
}
void CCalPerfectSectsDlg::OnChangeCmbSectNum()
{
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_SECT_NUM);
	UpdateData();
	int iCurSel = pCMB->GetCurSel();
	pCMB->GetLBText(iCurSel,m_sSectNum);
	m_nSectNum=atoi(m_sSectNum);
	double fMaxStress=m_pSteel->f(m_pRod->GetThick());
	double fStableStress=VerityCompStableStress();
#ifdef AFX_TARG_ENU_ENGLISH
	if(fStableStress>fMaxStress)
		m_sStress.Format("ѹ��Ӧ��(%.2fMPa)>ultimate strength(%.0fMPa)",fStableStress,fMaxStress);
	else
		m_sStress.Format("ѹ��Ӧ��(%.2fMPa)<ultimate strength(%.0fMPa)",fStableStress,fMaxStress);
#else 
	if(fStableStress>fMaxStress)
		m_sStress.Format("ѹ��Ӧ��(%.2fMPa)>����ǿ��(%.0fMPa)",fStableStress,fMaxStress);
	else
		m_sStress.Format("ѹ��Ӧ��(%.2fMPa)<����ǿ��(%.0fMPa)",fStableStress,fMaxStress);
#endif
	UpdateData(FALSE);
}
void CCalPerfectSectsDlg::OnChangeEHoleOut()
{
	UpdateData();
	m_fLsCoef=atof(m_sLsCoef);
	double fMaxStress=m_pSteel->f(m_pRod->GetThick());
	m_fNetArea=m_fGrossArea-m_fLsCoef*m_nBoltD*m_pRod->GetThick();	//�۳���˨��
	double fStableStress=VerityCompStableStress();
#ifdef AFX_TARG_ENU_ENGLISH
	if(fStableStress>fMaxStress)
		m_sStress.Format("ѹ��Ӧ��(%.2fMPa)>ultimate strength(%.0fMPa)",fStableStress,fMaxStress);
	else
		m_sStress.Format("ѹ��Ӧ��(%.2fMPa)>ultimate strength(%.0fMPa)",fStableStress,fMaxStress);
#else 
	if(fStableStress>fMaxStress)
		m_sStress.Format("ѹ��Ӧ��(%.2fMPa)>����ǿ��(%.0fMPa)",fStableStress,fMaxStress);
	else
		m_sStress.Format("ѹ��Ӧ��(%.2fMPa)>����ǿ��(%.0fMPa)",fStableStress,fMaxStress);
#endif
	UpdateData(FALSE);
}
void CCalPerfectSectsDlg::OnChangeCmbLsGuige() 
{
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_BOLT_SIZE);
	UpdateData();
	int iCurSel = pCMB->GetCurSel();
	pCMB->GetLBText(iCurSel,m_sLsGuiGe);
	if(m_sLsGuiGe.Find("16",1)==1)
		m_nBoltD=16;
	else if(m_sLsGuiGe.Find("20",1)==1)
		m_nBoltD=20;
	else if(m_sLsGuiGe.Find("24",1)==1)
		m_nBoltD=24;
	double fMaxStress=m_pSteel->f(m_pRod->GetThick());
	m_fNetArea=m_fGrossArea-m_fLsCoef*m_nBoltD*m_pRod->GetThick();	//�۳���˨��
	double fStableStress=VerityCompStableStress();
#ifdef AFX_TARG_ENU_ENGLISH
	
	if(fStableStress>fMaxStress)
		m_sStress.Format("ѹ��Ӧ��(%.2fMPa)>ultimate strength(%.0fMPa)",fStableStress,fMaxStress);
	else
		m_sStress.Format("ѹ��Ӧ��(%.2fMPa)<ultimate strength(%.0fMPa)",fStableStress,fMaxStress);
#else 
	if(fStableStress>fMaxStress)
		m_sStress.Format("ѹ��Ӧ��(%.2fMPa)>����ǿ��(%.0fMPa)",fStableStress,fMaxStress);
	else
		m_sStress.Format("ѹ��Ӧ��(%.2fMPa)<����ǿ��(%.0fMPa)",fStableStress,fMaxStress);
#endif
	UpdateData(FALSE);
}
#endif