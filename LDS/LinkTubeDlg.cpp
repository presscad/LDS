// LinkTubeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "LinkTubeDlg.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "LocalFeatureDef.h"
#include "DataCompare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinkTubeDlg dialog

CLinkTubeDlg::CLinkTubeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLinkTubeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinkTubeDlg)
	m_iPart1stChar = 1;
	m_iPart2ndChar = 0;
	m_sTubeGuige = _T("");
	m_iTubeMat = 0;
	m_sMirYPartNo = _T("");
	m_sMirXPartNo = _T("");
	m_sOriginPartNo = _T("");
	m_sMirZPartNo = _T("");
	m_sSegNo = _T("");
	m_bMirSideSpace = FALSE;
	m_bMirAxisX = FALSE;
	m_bMirAxisXYZ = FALSE;
	m_bMirAxisY = FALSE;
	m_bMirAxisZ = FALSE;
	//}}AFX_DATA_INIT
	m_iPoleType = 0;
	memset(sPartNoArr,0,4*4*16);
	m_pStartNode = NULL;
	m_pEndNode = NULL;
}


void CLinkTubeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkTubeDlg)
	DDX_Radio(pDX, IDC_HEAD_PART, m_iPart1stChar);
	DDX_Radio(pDX, IDC_MAIN_PART, m_iPart2ndChar);
	DDX_CBString(pDX, IDC_CMB_TUBE_GUIEGE, m_sTubeGuige);
	DDX_CBIndex(pDX, IDC_CMB_TUBE_MATERIAL, m_iTubeMat);
	DDX_Text(pDX, IDC_E_MIR_Y_ORIGIN_PART_NO, m_sMirYPartNo);
	DDX_Text(pDX, IDC_E_MIR_X_ORIGIN_PART_NO, m_sMirXPartNo);
	DDX_Text(pDX, IDC_E_ORIGIN_PART_NO, m_sOriginPartNo);
	DDX_Text(pDX, IDC_E_MIR_Z_ORIGIN_PART_NO, m_sMirZPartNo);
	DDX_Text(pDX, IDC_E_SEG_NO, m_sSegNo);
	DDX_Check(pDX, IDC_CHK_MIR_SIDE_FACE, m_bMirSideSpace);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_X, m_bMirAxisX);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_XYZ, m_bMirAxisXYZ);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_Y, m_bMirAxisY);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_Z, m_bMirAxisZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinkTubeDlg, CDialog)
	//{{AFX_MSG_MAP(CLinkTubeDlg)
	ON_EN_CHANGE(IDC_E_ORIGIN_PART_NO, OnChangeEOriginPartNo)
	ON_EN_CHANGE(IDC_E_MIR_X_ORIGIN_PART_NO, OnChangeEMirXOriginPartNo)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_XYZ, OnChkMirAxisXYZ)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_X, OnMirAxisX)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_Y, OnMirAxisY)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_Z, OnMirAxisZ)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkTubeDlg message handlers

BOOL CLinkTubeDlg::OnInitDialog() 
{
	if(m_pStartNode==NULL||m_pEndNode==NULL)
		return FALSE;
	CComboBox* pCMB = NULL;
	if(m_iPoleType==0)
	{	//�ֹ�
		GetDlgItem(IDC_S_GUIGE)->SetWindowText("�ֹܹ��:");
		GetDlgItem(IDC_S_MATERIAL)->SetWindowText("�ֹܲ���:");
		pCMB = (CComboBox*)GetDlgItem(IDC_CMB_TUBE_GUIEGE);
		AddTubeRecord(pCMB);
	}
	else if(m_iPoleType==1)
	{	//���
		GetDlgItem(IDC_S_GUIGE)->SetWindowText("��ֹ��:");
		GetDlgItem(IDC_S_MATERIAL)->SetWindowText("��ֲ���:");
		pCMB = (CComboBox*)GetDlgItem(IDC_CMB_TUBE_GUIEGE);
		AddBianRecord(pCMB);
	}
	else if(m_iPoleType==2)
	{	//�۸�
		GetDlgItem(IDC_S_GUIGE)->SetWindowText("�۸ֹ��:");
		GetDlgItem(IDC_S_MATERIAL)->SetWindowText("�۸ֲ���:");
		pCMB = (CComboBox*)GetDlgItem(IDC_CMB_TUBE_GUIEGE);
		AddSlotRecord(pCMB);
	}
	if(m_sTubeGuige.IsEmpty())
	{
		pCMB->SetCurSel(0);
		pCMB->GetLBText(0,m_sTubeGuige);
	}
	pCMB = (CComboBox*)GetDlgItem(IDC_CMB_TUBE_MATERIAL);
	AddSteelMatRecord(pCMB);
	xy1_yz2_xz3=0;	
	mirmsg=GetPoleValidMirMsg(m_pStartNode,m_pEndNode,&xy1_yz2_xz3);
	if(!(mirmsg.axis_flag&1))
	{	//���ܽ���X�Գ�
		m_bMirAxisX=FALSE;
		GetDlgItem(IDC_CHK_MIR_AXIS_X)->EnableWindow(FALSE);
	}
	if(!(mirmsg.axis_flag&2))
	{	//���ܽ���Y�Գ�
		m_bMirAxisY=FALSE;
		GetDlgItem(IDC_CHK_MIR_AXIS_Y)->EnableWindow(FALSE);
	}
	if(!(mirmsg.axis_flag&4))
	{	//���ܽ���Z�Գ�
		m_bMirAxisZ=FALSE;
		GetDlgItem(IDC_CHK_MIR_AXIS_Z)->EnableWindow(FALSE);
	}
	if(m_bMirAxisXYZ)
	{
		if(mirmsg.axis_flag&1)	//����X�Գ�
			m_bMirAxisX = m_bMirAxisXYZ;
		if(mirmsg.axis_flag&2)	//����Y�Գ�
			m_bMirAxisY = m_bMirAxisXYZ;
		if(mirmsg.axis_flag&4)	//����Z�Գ�
			m_bMirAxisZ = m_bMirAxisXYZ;
		if(xy1_yz2_xz3==1)		//XY�Գƻ���
			m_bMirAxisY=FALSE;
		else if(xy1_yz2_xz3==2)	//YZ�Գƻ���
			m_bMirAxisY=FALSE;
		else if(xy1_yz2_xz3==3)	//XZ�Գƻ���
			m_bMirAxisX=FALSE;
	}
	GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow (m_bMirAxisX);
	GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisY);
	GetDlgItem(IDC_E_MIR_Z_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisZ);
	BOOL bRetCode=CDialog::OnInitDialog();
#if defined(__TMA_)||defined(__LMA_)
	//If OnInitDialog returns nonzero, Windows sets the input focus to the first control in the dialog box.
	//The application can return 0 only if it has explicitly set the input focus to one of the controls in the dialog box.
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
	{
		GetDlgItem(IDC_E_ORIGIN_PART_NO)->SetFocus();	//�������ݶԱ�ʱ������ΪĬ�Ͻ��� wht 18-12-04
		bRetCode=FALSE;
	}
#endif
	return bRetCode;
}

void CLinkTubeDlg::OnChangeEOriginPartNo() 
{
	UpdateData();
	CString sNo=m_sOriginPartNo;
	if(m_sOriginPartNo.GetLength()>15)
	{
		AfxMessageBox("ԭʼ�������̫��!");
		m_sOriginPartNo = sNo;
		return;
	}
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sOriginPartNo);
	if(pPart)
	{	//�����ҵ���BOM��Ϣ�����¸øְ�Ļ�����Ϣ(�κš���񡢲���)
		if((m_iPoleType==0&&pPart->cPartType==BOMPART::TUBE)||(m_iPoleType==1&&pPart->cPartType==BOMPART::FLAT))
		{
			m_sTubeGuige.Format("%.0fX%.0f",pPart->wide,pPart->thick);
			m_iTubeMat=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
		}
		//else
			//AfxMessageBox(CXhChar100("Bom��û�м���Ϊ#%s�ĸְ�",(char*)pPart->sPartNo));
	}
#endif
	m_sMirZPartNo = m_sOriginPartNo;
	if(m_sMirXPartNo.GetLength()!=m_sOriginPartNo.GetLength())
		m_sMirXPartNo=m_sMirYPartNo=m_sOriginPartNo;
	for(int i=0;i<m_sOriginPartNo.GetLength();i++)
	{
		if(isdigit(m_sOriginPartNo[i]))
		{
			m_sMirXPartNo.SetAt(i,m_sOriginPartNo[i]);
			m_sMirYPartNo=m_sMirXPartNo;
		}
	}
	SEGI iSeg;
	if(ParsePartNo(sNo.GetBuffer(),&iSeg,NULL))
		m_sSegNo=iSeg.ToString();
	UpdateData(FALSE);
	
}

void CLinkTubeDlg::OnChangeEMirXOriginPartNo() 
{
	CString sNo=m_sMirXPartNo;
	UpdateData();
	if(m_sOriginPartNo.GetLength()>15)
	{
		AfxMessageBox("ԭʼ�������̫��!");
		m_sMirXPartNo = sNo;
		return;
	}
	m_sMirYPartNo=m_sMirXPartNo;	
	UpdateData(FALSE);
}

void CLinkTubeDlg::OnOK() 
{
	CDialog::OnOK();
	mirmsg.origin.Set();
	mirmsg.axis_flag = 0;
	if(m_bMirAxisX)
		mirmsg.axis_flag|=1;
	if(m_bMirAxisY)
		mirmsg.axis_flag|=2;
	if(m_bMirAxisZ)
		mirmsg.axis_flag|=4;
	if(m_bMirSideSpace)
		mirmsg.axis_flag|=8;
	mirmsg.array_num = 1;
	mirmsg.rotate_angle = -90;
}

void CLinkTubeDlg::OnChkMirAxisXYZ() 
{
	UpdateData();
	if(m_bMirAxisXYZ)
	{
		if(mirmsg.axis_flag&1)	//����X�Գ�
			m_bMirAxisX = m_bMirAxisXYZ;
		if(mirmsg.axis_flag&2)	//����Y�Գ�
			m_bMirAxisY = m_bMirAxisXYZ;
		if(mirmsg.axis_flag&4)	//����Z�Գ�
			m_bMirAxisZ = m_bMirAxisXYZ;
		if(xy1_yz2_xz3==1)		//XY�Գƻ���
			m_bMirAxisY=FALSE;
		else if(xy1_yz2_xz3==2)	//YZ�Գƻ���
			m_bMirAxisY=FALSE;
		else if(xy1_yz2_xz3==3)	//XZ�Գƻ���
			m_bMirAxisX=FALSE;
	}
	else
		m_bMirAxisX=m_bMirAxisY=m_bMirAxisZ=FALSE;
	GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow (m_bMirAxisX);
	GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisY);
	GetDlgItem(IDC_E_MIR_Z_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisZ);
	UpdateData(FALSE);
}

void CLinkTubeDlg::OnMirAxisX() 
{
	UpdateData();
	if(xy1_yz2_xz3==1)	//XY�Գƻ���
	{
		if(m_bMirAxisY)
			m_bMirAxisY=!m_bMirAxisX;
	}
	else if(xy1_yz2_xz3==3)	//XZ�Գƻ���
	{
		if(m_bMirAxisZ)
			m_bMirAxisZ=!m_bMirAxisX;
	}
	GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow (m_bMirAxisX);
	GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisY);
	GetDlgItem(IDC_E_MIR_Z_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisZ);
	UpdateData(FALSE);
}

void CLinkTubeDlg::OnMirAxisY() 
{
	UpdateData();
	if(xy1_yz2_xz3==2)	//YZ�Գƻ���
	{
		if(m_bMirAxisZ)
			m_bMirAxisZ=!m_bMirAxisY;
	}
	else if(xy1_yz2_xz3==1)	//XY�Գƻ���
	{
		if(m_bMirAxisX)
			m_bMirAxisX=!m_bMirAxisY;
	}
	GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow (m_bMirAxisX);
	GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisY);
	GetDlgItem(IDC_E_MIR_Z_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisZ);
	UpdateData(FALSE);
}

void CLinkTubeDlg::OnMirAxisZ() 
{
	UpdateData();
	if(xy1_yz2_xz3==2)	//YZ�Գƻ���
	{
		if(m_bMirAxisY)
			m_bMirAxisY=!m_bMirAxisZ;
	}
	else if(xy1_yz2_xz3==3)	//XZ�Գƻ���
	{
		if(m_bMirAxisX)
			m_bMirAxisX=!m_bMirAxisZ;
	}
	GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow (m_bMirAxisX);
	GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisY);
	GetDlgItem(IDC_E_MIR_Z_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisZ);
	UpdateData(FALSE);
}
