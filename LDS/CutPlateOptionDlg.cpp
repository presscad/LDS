// CutPlateOptionDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "CutPlateOptionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CCutPlateOptionDlg dialog


CCutPlateOptionDlg::CCutPlateOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCutPlateOptionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCutPlateOptionDlg)
	m_bSaveAnotherPlate = FALSE;
	m_iFaceNo = 0;
	m_fOffsetDist = 1.0;
	m_iTopFace = 0;
	m_fCutFaceNormX = 0.0;
	m_fCutFaceNormY = 0.0;
	m_fCutFaceNormZ = 0.0;
	m_fCutFacePickX = 0.0;
	m_fCutFacePickY = 0.0;
	m_fCutFacePickZ = 0.0;
	m_bWeld = FALSE;
	m_bMirCut = FALSE;
	//}}AFX_DATA_INIT
	m_pDatumPart=NULL;
}


void CCutPlateOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCutPlateOptionDlg)
	DDX_Check(pDX, IDC_CHK_SAVE_ANOTHER_PLATE, m_bSaveAnotherPlate);
	DDX_CBIndex(pDX, IDC_CMB_FACE_NO, m_iFaceNo);
	DDX_Text(pDX, IDC_E_OFFSET_DIST, m_fOffsetDist);
	DDX_Radio(pDX, IDC_RDO_TOP_FACE, m_iTopFace);
	DDX_Text(pDX, IDC_E_CUT_FACE_NORM_X, m_fCutFaceNormX);
	DDX_Text(pDX, IDC_E_CUT_FACE_NORM_Y, m_fCutFaceNormY);
	DDX_Text(pDX, IDC_E_CUT_FACE_NORM_Z, m_fCutFaceNormZ);
	DDX_Text(pDX, IDC_E_CUT_FACE_PICK_X, m_fCutFacePickX);
	DDX_Text(pDX, IDC_E_CUT_FACE_PICK_Y, m_fCutFacePickY);
	DDX_Text(pDX, IDC_E_CUT_FACE_PICK_Z, m_fCutFacePickZ);
	DDX_Check(pDX, IDC_CHK_WELD, m_bWeld);
	DDX_Check(pDX, IDC_CHK_MIR_CUT, m_bMirCut);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCutPlateOptionDlg, CDialog)
	//{{AFX_MSG_MAP(CCutPlateOptionDlg)
	ON_BN_CLICKED(IDC_CHK_SAVE_ANOTHER_PLATE, OnSaveAnotherPlate)
	ON_CBN_SELCHANGE(IDC_CMB_FACE_NO, OnSelchangeCmbFaceNo)
	ON_BN_CLICKED(IDC_RDO_TOP_FACE, OnRdoTopBtmFace)
	ON_BN_CLICKED(IDC_RDO_BOTTOM_FACE, OnRdoTopBtmFace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCutPlateOptionDlg message handlers

BOOL CCutPlateOptionDlg::OnInitDialog() 
{
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_FACE_NO);
	if(pCMB==NULL)
		return FALSE;
	m_fTopBtmSpace=0;
	m_bMirCut=FALSE;	//默认不支持对称切割
	if(m_pDatumPart==NULL)
	{
		GetDlgItem(IDC_CMB_FACE_NO)->EnableWindow(FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_RDO_TOP_FACE)->SetWindowText("Z Axis Forward Direction");
		GetDlgItem(IDC_RDO_TOP_FACE)->SetWindowText("Z Axis Backward Direction");
#else 
		GetDlgItem(IDC_RDO_TOP_FACE)->SetWindowText("Z轴正方向");
		GetDlgItem(IDC_RDO_TOP_FACE)->SetWindowText("Z轴负方向");
#endif
	}
	else if(m_pDatumPart&&m_pDatumPart->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pPlate=(CLDSPlate*)m_pDatumPart;
		for(int i=0;i<pPlate->face_N;i++)
		{
			CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
			ss.Format("NO.%d plane",i+1);
#else 
			ss.Format("%d号面",i+1);
#endif
			pCMB->AddString(ss);
		}
		if(pPlate->face_N==2)	
#ifdef AFX_TARG_ENU_ENGLISH
			pCMB->AddString("Double plane cut");
#else 
			pCMB->AddString("双面切割");
#endif
		m_fTopBtmSpace=pPlate->GetThick();
		//基准钢板为单面板时支持对称切割 wht 11-01-07
		GetDlgItem(IDC_CHK_MIR_CUT)->ShowWindow(pPlate->face_N==1);
	}
	else if(m_pDatumPart&&m_pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCMB->AddString("No.1 plane");
#else 
		pCMB->AddString("1号面");
#endif
		CLDSParamPlate* pParamPlate=(CLDSParamPlate*)m_pDatumPart;
		if(pParamPlate->m_iParamType==TYPE_UEND)
		{
			double W=0;
			pParamPlate->GetDesignItemValue('W',&W);
			m_fTopBtmSpace=W+pParamPlate->thick*2;
		}
		else
			m_fTopBtmSpace=pParamPlate->thick;
	}
	pCMB->SetCurSel(0);
	GetDlgItem(IDC_RDO_TOP_FACE)->EnableWindow(!m_bSaveAnotherPlate);
	GetDlgItem(IDC_RDO_BOTTOM_FACE)->EnableWindow(!m_bSaveAnotherPlate);
	CDialog::OnInitDialog();
	UpdateCutFaceNorm();
	return TRUE;
}

void CCutPlateOptionDlg::OnSaveAnotherPlate() 
{
	UpdateData();
	GetDlgItem(IDC_RDO_TOP_FACE)->EnableWindow(!m_bSaveAnotherPlate);
	GetDlgItem(IDC_RDO_BOTTOM_FACE)->EnableWindow(!m_bSaveAnotherPlate);
	UpdateData(FALSE);
	UpdateCutFaceNorm();
}

void CCutPlateOptionDlg::UpdateCutFaceNorm()
{
	UpdateData(TRUE);
	if(m_pDatumPart)
	{
		f3dPoint face_pick;
		if(m_pDatumPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate *pDatumPlate=(CLDSPlate*)m_pDatumPart;
			if(m_iFaceNo==0||(pDatumPlate->face_N==2&&m_iFaceNo==2))
			{	//双面切割或使用基准面切割 wht 11-01-20
				m_fCutFaceNormX=pDatumPlate->ucs.axis_z.x;
				m_fCutFaceNormY=pDatumPlate->ucs.axis_z.y;
				m_fCutFaceNormZ=pDatumPlate->ucs.axis_z.z;
				face_pick=pDatumPlate->ucs.origin;
			}
			else if(m_iFaceNo==1)
			{
				m_fCutFaceNormX=pDatumPlate->huoqufacenorm[0].x;
				m_fCutFaceNormY=pDatumPlate->huoqufacenorm[0].y;
				m_fCutFaceNormZ=pDatumPlate->huoqufacenorm[0].z;
				face_pick=pDatumPlate->HuoQuLine[0].startPt;
				coord_trans(face_pick,pDatumPlate->ucs,TRUE);
			}
			else
			{
				m_fCutFaceNormX=pDatumPlate->huoqufacenorm[1].x;
				m_fCutFaceNormY=pDatumPlate->huoqufacenorm[1].y;
				m_fCutFaceNormZ=pDatumPlate->huoqufacenorm[1].z;
				face_pick=pDatumPlate->HuoQuLine[1].startPt;
				coord_trans(face_pick,pDatumPlate->ucs,TRUE);
			}
			face_pick+=pDatumPlate->ucs.axis_z*pDatumPlate->m_fNormOffset;
			if(m_iTopFace==0)
			{	//顶面负法线方向切割
				m_fCutFaceNormX*=-1.0;
				m_fCutFaceNormY*=-1.0;
				m_fCutFaceNormZ*=-1.0;
				if(m_iFaceNo==0)
					face_pick+=pDatumPlate->ucs.axis_z*pDatumPlate->GetThick();
				else if(m_iFaceNo==1)
					face_pick+=pDatumPlate->huoqufacenorm[0]*pDatumPlate->GetThick();
				else //if(m_iFaceNo==2)
					face_pick+=pDatumPlate->huoqufacenorm[1]*pDatumPlate->GetThick();
			}
		}
		else if(m_pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)m_pDatumPart;
			double W=0;
			if(pParamPlate->m_iParamType==TYPE_FL||
				pParamPlate->m_iParamType==TYPE_RIBPLATE||
				pParamPlate->m_iParamType==TYPE_FLR||
				pParamPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
				face_pick=pParamPlate->ucs.origin;
			else if(pParamPlate->m_iParamType==TYPE_UEND)
			{
				pParamPlate->GetDesignItemValue('W',&W);
				face_pick=pParamPlate->ucs.origin-pParamPlate->ucs.axis_z*(0.5*W+pParamPlate->thick);
			}
			else 
				face_pick=pParamPlate->ucs.origin-pParamPlate->ucs.axis_z*(0.5*pParamPlate->thick);
			if(m_iTopFace==0)
			{	//顶面负法线方向切割
				m_fCutFaceNormX=-pParamPlate->ucs.axis_z.x;
				m_fCutFaceNormY=-pParamPlate->ucs.axis_z.y;
				m_fCutFaceNormZ=-pParamPlate->ucs.axis_z.z;
				face_pick+=pParamPlate->ucs.axis_z*m_fTopBtmSpace;
			}
			else
			{	//底面正法线方向切割
				m_fCutFaceNormX=m_pDatumPart->ucs.axis_z.x;
				m_fCutFaceNormY=m_pDatumPart->ucs.axis_z.y;
				m_fCutFaceNormZ=m_pDatumPart->ucs.axis_z.z;
			}
		}
		else
		{
			face_pick=m_pDatumPart->ucs.origin;
			if(m_iTopFace==0)
			{	//顶面负法线方向切割
				m_fCutFaceNormX=-m_pDatumPart->ucs.axis_z.x;
				m_fCutFaceNormY=-m_pDatumPart->ucs.axis_z.y;
				m_fCutFaceNormZ=-m_pDatumPart->ucs.axis_z.z;
				face_pick+=m_pDatumPart->ucs.axis_z*m_pDatumPart->GetThick();
			}
			else
			{	//底面正法线方向切割
				m_fCutFaceNormX=m_pDatumPart->ucs.axis_z.x;
				m_fCutFaceNormY=m_pDatumPart->ucs.axis_z.y;
				m_fCutFaceNormZ=m_pDatumPart->ucs.axis_z.z;
			}
		}
		m_fCutFacePickX=face_pick.x;
		m_fCutFacePickY=face_pick.y;
		m_fCutFacePickZ=face_pick.z;
	}
	UpdateData(FALSE);
}

void CCutPlateOptionDlg::OnSelchangeCmbFaceNo() 
{
	UpdateCutFaceNorm();
}

void CCutPlateOptionDlg::OnRdoTopBtmFace() 
{
	UpdateCutFaceNorm();
}
#endif
