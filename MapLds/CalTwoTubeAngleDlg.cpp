//<LOCALE_TRANSLATE BY hxr /> 2015-04-25
// CalTwoTubeAngleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CalTwoTubeAngleDlg.h"
#include "DrawTaMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalTwoTubeAngleDlg dialog

#ifndef __TSA_FILE_
CCalTwoTubeAngleDlg::CCalTwoTubeAngleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCalTwoTubeAngleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCalTwoTubeAngleDlg)
	m_bDimTubeAngle = TRUE;
	m_fDimRadius = 100.0;
	m_sEndTubeHandle = _T("0X0");
	m_sStartTubeHandle = _T("0X0");
	m_fTubeAngle = 0.0;
	m_bInputTubeHandle = FALSE;
	m_sPromptStr = _T("");
	//}}AFX_DATA_INIT
	m_bCanDimAngle=FALSE;
}

void CCalTwoTubeAngleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalTwoTubeAngleDlg)
	DDX_Check(pDX, IDC_CHK_DIM_TUBE_ANGLE, m_bDimTubeAngle);
	DDX_Text(pDX, IDC_E_DIM_RADIUS, m_fDimRadius);
	DDX_Text(pDX, IDC_E_END_TUBE_HANDLE, m_sEndTubeHandle);
	DDX_Text(pDX, IDC_E_START_TUBE_HANDLE, m_sStartTubeHandle);
	DDX_Text(pDX, IDC_E_TUBE_ANGLE, m_fTubeAngle);
	DDX_Check(pDX, IDC_CHK_INPUT_TUBE_HANDLE, m_bInputTubeHandle);
	DDX_Text(pDX, IDC_S_PROMPT_STR, m_sPromptStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCalTwoTubeAngleDlg, CDialog)
	//{{AFX_MSG_MAP(CCalTwoTubeAngleDlg)
	ON_BN_CLICKED(IDC_BTN_CAL_TWO_TUBE_ANGLE, OnCalTwoTubeAngle)
	ON_BN_CLICKED(IDC_CHK_INPUT_TUBE_HANDLE, OnInputTubeHandle)
	ON_EN_KILLFOCUS(IDC_E_START_TUBE_HANDLE, OnKillfocusTubeHandle)
	ON_BN_CLICKED(IDC_CHK_DIM_TUBE_ANGLE, OnDimTubeAngle)
	ON_EN_KILLFOCUS(IDC_E_END_TUBE_HANDLE, OnKillfocusTubeHandle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalTwoTubeAngleDlg message handlers

BOOL CCalTwoTubeAngleDlg::OnInitDialog() 
{
	if(m_pStartTube&&m_pEndTube)
	{
		m_sStartTubeHandle.Format("0X%X",m_pStartTube->handle);
		m_sEndTubeHandle.Format("0X%X",m_pEndTube->handle);
		m_bInputTubeHandle=FALSE;	//ͨ��ѡ����ͼ�õ��ľ�������Զ���ע
		m_bDimTubeAngle=TRUE;		//Ĭ��ѡ���ע�Ƕ�	
		CalTwoTubeAngle();			//�������ֹܼн�
		m_bCanDimAngle=TRUE;		//����Ƿ���Ա�ע��
	}
	else
	{
		if(m_pStartTube)
			m_sStartTubeHandle.Format("0X%X",m_pStartTube->handle);
		if(m_pEndTube)
			m_sEndTubeHandle.Format("0X%X",m_pEndTube->handle);
		m_bInputTubeHandle=TRUE;//δѡ����ʹ��� 
		m_bDimTubeAngle=FALSE;	//���ܱ�ע�Ƕ�
	}
	if(!m_bCanDimAngle)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sPromptStr="Can't dimensions automatic";
	else 
		m_sPromptStr="Can dimensions automatic";
#else
		m_sPromptStr="�޷�ʵ���Զ���ע";
	else 
		m_sPromptStr="����ʵ���Զ���ע";
#endif
	GetDlgItem(IDC_E_START_TUBE_HANDLE)->EnableWindow(m_bInputTubeHandle);
	GetDlgItem(IDC_E_END_TUBE_HANDLE)->EnableWindow(m_bInputTubeHandle);
	GetDlgItem(IDC_CHK_DIM_TUBE_ANGLE)->EnableWindow(!m_bInputTubeHandle);
	GetDlgItem(IDC_E_DIM_RADIUS)->EnableWindow(!m_bInputTubeHandle&&m_bDimTubeAngle);
	CDialog::OnInitDialog();
	return TRUE;
}

double CCalTwoTubeAngleDlg::CalTwoTubeAngle()
{
	long hStartHandle=0, hEndHandle=0;
	sscanf(m_sStartTubeHandle,"%X",&hStartHandle);
	sscanf(m_sEndTubeHandle,"%X",&hEndHandle);
	m_pStartTube=(CLDSLineTube*)Ta.FromPartHandle(hStartHandle,CLS_LINETUBE);
	m_pEndTube=(CLDSLineTube*)Ta.FromPartHandle(hEndHandle,CLS_LINETUBE);
	if(m_pStartTube==NULL||m_pEndTube==NULL)
		return 0;
	int nRet=Int3dll(m_pStartTube->Start(),m_pStartTube->End(),m_pEndTube->Start(),m_pEndTube->End(),m_ptDimVertex);
	if(nRet>0||nRet==-2)
	{	//���ֹ��н���
		f3dPoint start_tube_vec, end_tube_vec;
		start_tube_vec=m_pStartTube->End()-m_pStartTube->Start();
		end_tube_vec=m_pEndTube->End()-m_pEndTube->Start();
		normalize(start_tube_vec);
		normalize(end_tube_vec);
		m_fTubeAngle=cal_angle_of_2vec(start_tube_vec,end_tube_vec);
		m_ptDimStart=m_ptDimVertex+start_tube_vec*m_fDimRadius;
		m_ptDimEnd=m_ptDimVertex+end_tube_vec*m_fDimRadius;
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("There is not intersection point between the two tubes,they are on different plane or parallel!");
#else
		MessageBox("���ֹ��޽��㣬�����ƽ��!");
#endif
	m_fTubeAngle*=DEGTORAD_COEF;	//������ת��Ϊ��
	UpdateData(FALSE);
	return m_fTubeAngle;
}

void CCalTwoTubeAngleDlg::OnCalTwoTubeAngle() 
{
	UpdateData();
	CalTwoTubeAngle();
	UpdateData();
}

void CCalTwoTubeAngleDlg::OnInputTubeHandle() 
{
	UpdateData();
	if(!m_bCanDimAngle)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sPromptStr="Can't dimensions automatic";
	else 
		m_sPromptStr="Can dimensions automatic";
#else
		m_sPromptStr="�޷�ʵ���Զ���ע";
	else 
		m_sPromptStr="����ʵ���Զ���ע";
#endif
	GetDlgItem(IDC_E_START_TUBE_HANDLE)->EnableWindow(m_bInputTubeHandle);
	GetDlgItem(IDC_E_END_TUBE_HANDLE)->EnableWindow(m_bInputTubeHandle);
	GetDlgItem(IDC_CHK_DIM_TUBE_ANGLE)->EnableWindow(!m_bInputTubeHandle&&m_bCanDimAngle);
	GetDlgItem(IDC_E_DIM_RADIUS)->EnableWindow(!m_bInputTubeHandle&&m_bDimTubeAngle&&m_bCanDimAngle);
	UpdateData(FALSE);
}

void CCalTwoTubeAngleDlg::OnKillfocusTubeHandle() 
{
	long hOldStartPart=0,hOldEndPart=0;
	if(m_pStartTube)
		hOldStartPart=m_pStartTube->handle;
	if(m_pEndTube)
		hOldEndPart=m_pEndTube->handle;
	UpdateData();
	long hStartHandle=0, hEndHandle=0;
	sscanf(m_sStartTubeHandle,"%X",&hStartHandle);
	sscanf(m_sEndTubeHandle,"%X",&hEndHandle);
	m_pStartTube=(CLDSLineTube*)Ta.FromPartHandle(hStartHandle,CLS_LINETUBE);
	m_pEndTube=(CLDSLineTube*)Ta.FromPartHandle(hEndHandle,CLS_LINETUBE);
	if(m_pStartTube&&m_pEndTube)
		CalTwoTubeAngle();
	else
		m_fTubeAngle=0;
	if(hStartHandle!=hOldStartPart||hOldEndPart!=hEndHandle)
	{
		m_bCanDimAngle=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
		m_sPromptStr="Can't dimensions automatic";
#else
		m_sPromptStr="�޷�ʵ���Զ���ע";
#endif
	}
	UpdateData(FALSE);
}

void CCalTwoTubeAngleDlg::OnDimTubeAngle() 
{
	UpdateData();
	if(!m_bCanDimAngle)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sPromptStr="Can't dimensions automatic";
	else 
		m_sPromptStr="Can dimensions automatic";
#else
		m_sPromptStr="�޷�ʵ���Զ���ע";
	else 
		m_sPromptStr="����ʵ���Զ���ע";
#endif
	GetDlgItem(IDC_E_START_TUBE_HANDLE)->EnableWindow(m_bInputTubeHandle);
	GetDlgItem(IDC_E_END_TUBE_HANDLE)->EnableWindow(m_bInputTubeHandle);
	GetDlgItem(IDC_CHK_DIM_TUBE_ANGLE)->EnableWindow(!m_bInputTubeHandle&&m_bCanDimAngle);
	GetDlgItem(IDC_E_DIM_RADIUS)->EnableWindow(!m_bInputTubeHandle&&m_bDimTubeAngle&&m_bCanDimAngle);
	UpdateData(FALSE);
}

void CCalTwoTubeAngleDlg::OnOK() 
{
	UpdateData();
	if(m_pStartTube&&m_pEndTube)
	{
		f3dPoint start_tube_vec, end_tube_vec;
		start_tube_vec=m_pStartTube->End()-m_pStartTube->Start();
		end_tube_vec=m_pEndTube->End()-m_pEndTube->Start();
		normalize(start_tube_vec);
		normalize(end_tube_vec);
		m_ptDimStart=m_ptDimVertex+start_tube_vec*m_fDimRadius;
		m_ptDimEnd=m_ptDimVertex+end_tube_vec*m_fDimRadius;
	}
	CDialog::OnOK();
}
#endif