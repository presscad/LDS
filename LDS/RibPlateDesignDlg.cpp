//<LOCALE_TRANSLATE Confirm by hxr/>
// RibPlateDesignDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "RibPlateDesignDlg.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CRibPlateDesignDlg dialog


CRibPlateDesignDlg::CRibPlateDesignDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRibPlateDesignDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRibPlateDesignDlg)
	m_iMaterial = 0;
	m_iDatumLine = 0;
	m_fEndPosX = 0.0;
	m_fEndPosY = 0.0;
	m_fEndPosZ = 0.0;
	m_nHigh = 100;
	m_sPartNo = _T("");
	m_fPosAngle = 0.0;
	m_sSegI = _T("");
	m_fStartPosX = 0.0;
	m_fStartPosY = 0.0;
	m_fStartPosZ = 0.0;
	m_nThick = 8;
	m_nPlateNum = 1;
	m_fNormOffset = -4;
	m_fStartHigh = 5;
	m_fGradientAngle=90;
	//}}AFX_DATA_INIT
	m_bDatumTube=FALSE;
	m_iPlateType = 2;	//0.垂向加劲板 1.封头板 2.普通肋板
	m_pDatumPart = NULL;
	m_pDatumNode = NULL;
}


void CRibPlateDesignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRibPlateDesignDlg)
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_CBIndex(pDX, IDC_CMB_DATUM_LINE, m_iDatumLine);
	DDX_Text(pDX, IDC_E_END_POS_X, m_fEndPosX);
	DDX_Text(pDX, IDC_E_END_POS_Y, m_fEndPosY);
	DDX_Text(pDX, IDC_E_END_POS_Z, m_fEndPosZ);
	DDX_Text(pDX, IDC_E_HIGH, m_nHigh);
	DDX_Text(pDX, IDC_E_START_HIGH, m_fStartHigh);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_POS_ANGLE, m_fPosAngle);
	DDX_Text(pDX, IDC_E_SEG_I, m_sSegI);
	DDX_Text(pDX, IDC_E_START_POS_X, m_fStartPosX);
	DDX_Text(pDX, IDC_E_START_POS_Y, m_fStartPosY);
	DDX_Text(pDX, IDC_E_START_POS_Z, m_fStartPosZ);
	DDX_Text(pDX, IDC_E_THICK, m_nThick);
	DDX_Text(pDX, IDC_E_PLATE_NUM, m_nPlateNum);
	DDX_Text(pDX, IDC_E_NORM_OFFSET, m_fNormOffset);
	DDX_Text(pDX, IDC_E_GRADIENT_ANGLE, m_fGradientAngle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRibPlateDesignDlg, CDialog)
	//{{AFX_MSG_MAP(CRibPlateDesignDlg)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangeEPartNo)
	ON_EN_CHANGE(IDC_E_THICK, OnChangePlateThick)
	ON_BN_CLICKED(IDC_BN_PASTE_END_POINT, OnPasteEndPoint)
	ON_BN_CLICKED(IDC_BN_COPY_END_POINT, OnCopyEndPoint)
	ON_BN_CLICKED(IDC_BN_PASTE_START_POINT, OnPasteStartPoint)
	ON_BN_CLICKED(IDC_BN_COPY_START_POINT, OnCopyStartPoint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRibPlateDesignDlg message handlers

BOOL CRibPlateDesignDlg::OnInitDialog() 
{
	if(m_pDatumPart==NULL)
		return FALSE;
	if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
	{
		if(m_pDatumNode==NULL)
			return FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("tube's rib plate");
#else
		SetWindowText("钢管肋板");
#endif
		CLDSLineTube *pLineTube=(CLDSLineTube*)m_pDatumPart;
		m_fStartPosY=m_fEndPosY=pLineTube->GetDiameter()*0.5;
		m_bDatumTube=TRUE;
		if(m_iPlateType==2)
		{
			if(pLineTube->pStart->handle==m_pDatumNode->handle)
				m_fEndPosX=-pLineTube->startOdd();
			else if(pLineTube->pEnd->handle==m_pDatumNode->handle)
				m_fStartPosX=pLineTube->endOdd();
		}
		m_fStartHigh=0;	//起始角度
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_BEGIN)->SetWindowText("start angle:");
		GetDlgItem(IDC_S_START)->SetWindowText("---VV-relative coord. start point locate-VV--");
		GetDlgItem(IDC_S_END)->SetWindowText("---VV-relative coord.end point locate-VV--");
		CComboBox*pDatumLineCMB = (CComboBox*)GetDlgItem(IDC_CMB_DATUM_LINE);
		pDatumLineCMB->AddString("Datum Edge");
		if(pLineTube->m_bHasWeldRoad)
			pDatumLineCMB->AddString("Weld Line");
#else
		GetDlgItem(IDC_S_BEGIN)->SetWindowText("起始角度:");
		GetDlgItem(IDC_S_START)->SetWindowText("----------VV-相对坐标起点定位-VV---------");
		GetDlgItem(IDC_S_END)->SetWindowText("----------VV-相对坐标终点定位-VV---------");
		CComboBox*pDatumLineCMB = (CComboBox*)GetDlgItem(IDC_CMB_DATUM_LINE);
		pDatumLineCMB->AddString("基准边");
		if(pLineTube->m_bHasWeldRoad)
			pDatumLineCMB->AddString("焊道线");
#endif
		pDatumLineCMB->SetCurSel(m_iDatumLine);
	}
	else if(m_pDatumPart->GetClassTypeId()==CLS_PLATE)
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("plate's rib plate");
#else
		SetWindowText("钢板肋板");
#endif
		CLDSPlate *pPlate=(CLDSPlate*)m_pDatumPart;
		if(m_nHigh>0)
			m_fStartHigh=pPlate->GetThick()+pPlate->m_fNormOffset;
		else
			m_fStartHigh=pPlate->m_fNormOffset;
	}
	else if(m_pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("plate's rib plate");
#else
		SetWindowText("钢板肋板");
#endif
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)m_pDatumPart;
		m_fStartHigh=0.5*pParamPlate->thick;
	}
	m_sPartNo=m_pDatumPart->GetPartNo();
	m_sSegI=m_pDatumPart->iSeg.ToString();
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
	m_CurPlateBmp.Detach();
	if(m_bDatumTube)
		m_CurPlateBmp.LoadBitmap(IDB_BMP_RIB_PLATE);
	else
		m_CurPlateBmp.LoadBitmap(IDB_BMP_RIB_PLATE1);
	pPic->SetBitmap(m_CurPlateBmp);
	GetDlgItem(IDC_E_POS_ANGLE)->EnableWindow(m_bDatumTube);
	GetDlgItem(IDC_E_PLATE_NUM)->EnableWindow(m_bDatumTube);
	GetDlgItem(IDC_E_GRADIENT_ANGLE)->EnableWindow(!m_bDatumTube);
	GetDlgItem(IDC_CMB_DATUM_LINE)->EnableWindow(m_bDatumTube);
	GetDlgItem(IDC_E_START_POS_X)->EnableWindow(TRUE);
	GetDlgItem(IDC_E_START_POS_Y)->EnableWindow(TRUE);
	GetDlgItem(IDC_E_START_POS_Z)->EnableWindow(FALSE);
	GetDlgItem(IDC_E_END_POS_X)->EnableWindow(TRUE);
	GetDlgItem(IDC_E_END_POS_Y)->EnableWindow(TRUE);
	GetDlgItem(IDC_E_END_POS_Z)->EnableWindow(FALSE);
	if(m_iPlateType==0)	//垂向加劲板
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("vertical strengthen plate");
	else if(m_iPlateType==1) //end enclosure plate
		SetWindowText("end enclosure plate");
#else
		SetWindowText("垂向加劲板");
	else if(m_iPlateType==1) //封头板
		SetWindowText("封头板");
#endif
	CComboBox*pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MATERIAL);
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(m_iMaterial);

	CDialog::OnInitDialog();
	return TRUE;
}

void CRibPlateDesignDlg::OnChangeEPartNo() 
{
	UpdateData();
	SEGI iSeg;
	if(ParsePartNo(m_sPartNo.GetBuffer(),&iSeg,NULL))
		m_sSegI=iSeg.ToString();
	UpdateData(FALSE);
}

void CRibPlateDesignDlg::OnChangePlateThick() 
{
	UpdateData();
	m_fNormOffset=-0.5*m_nThick;
	if(m_iPlateType==1) //封头板
		m_fNormOffset=0;
	UpdateData(FALSE);
}
void CRibPlateDesignDlg::OnOK() 
{
	UpdateData();
	CDialog::OnOK();
}

void CRibPlateDesignDlg::OnPasteEndPoint() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fEndPosX = point.x;
		m_fEndPosY = point.y;
		m_fEndPosZ = point.z;
		UpdateData(FALSE);
	}	
}

void CRibPlateDesignDlg::OnCopyEndPoint() 
{
	f3dPoint point;
	point.Set(m_fEndPosX,m_fEndPosY,m_fEndPosZ);
	WritePointToClip(point);	
}

void CRibPlateDesignDlg::OnPasteStartPoint() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fStartPosX = point.x;
		m_fStartPosY = point.y;
		m_fStartPosZ = point.z;
		UpdateData(FALSE);
	}	
}

void CRibPlateDesignDlg::OnCopyStartPoint() 
{
	f3dPoint point;
	point.Set(m_fStartPosX,m_fStartPosY,m_fStartPosZ);
	WritePointToClip(point);
}
#endif