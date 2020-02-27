// XInsertPlateIntersTubeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "XInsertPlateIntersTubeDlg.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "PartsDbDlg.h"
#include "XhCharString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CXInsertPlateIntersTubeDlg dialog


CXInsertPlateIntersTubeDlg::CXInsertPlateIntersTubeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXInsertPlateIntersTubeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXInsertPlateIntersTubeDlg)
	m_sBoltSize = _T("20");
	m_iCoverPlateMat = 0;
	m_iWrapPlateMat = 0;
	m_iDatumPlateMat = 0;
	m_iTransPlateMat = 0;
	m_iVertPlateMat = 0;
	m_nBoltNum = 4;
	m_sCoverPlatePartNo = _T("");
	m_fCoverPlateSectorAngle = 90.0;
	m_nCoverPlateThick = 5;
	m_nCoverPlateWidth = 60;
	m_nCoverPlateCutWidth = 0;
	m_sDatumPlatePartNo = _T("");
	m_nDatumPlateThick = 10;
	m_nDatumPlateWidth = 220;
	m_sWeldPlatePartNo = _T("");
	m_sWrapPlatePartNo = _T("");
	m_sTransPlatePartNo = _T("");
	m_nTransPlateThick = 10;
	m_sVertPlatePartNo = _T("");
	m_nVertPlateThick = 10;
	m_nVertPlateWidth = 60;
	m_sSegI = _T("");
	m_nDatumPlateLen = 290;
	m_nInsertPlateWeldLen = 160;
	m_nWeldSpace = 10;
	m_nTubeOddment = 290;
	m_bStdPart = FALSE;
	m_iHoleType=2;
	m_fHoleR=25;
	m_nTransPlateWidth = 0;
	m_bTransPlate = TRUE;
	m_sStdPlatePartNo = _T("");
	m_iInsertPlateType = 2;
	m_bStaggerLayoutBolt = TRUE;
	m_bXEndDoubleRowLayout = FALSE;
	m_nBoltColNum = 2;
	m_nBoltRowNum = 2;
	m_fBoltRowSpace = 60.0;
	m_fBoltColSpace = 60.0;
	m_fFirstBoltPosX = 30.0;
	m_fFirstBoltPosY = 30.0;
	m_fFirstBoltPosZ = 0.0;
	m_fPlateParamA = 10.0;
	m_fPlateParamN = 60.0;
	m_fPlateParamR = 15.0;
	m_fPlateParamS = 160.0;
	m_fSpecBoltSpaceD = 90.0;
	m_fPlateParamW = 10.0;
	m_bCreateVertPlate = TRUE;
	m_fCoverPlateOffsetAngle = 0.0;
	m_bUseAngle = FALSE;
	m_fCoverPlateManuSpace = 0.0;
	m_fVertPlateManuSpace = 0.0;
	m_sJgGuige = _T("");
	//}}AFX_DATA_INIT
	m_pCrossTube = NULL;
	m_pDatumTube = NULL;
	m_listBoltRect.AddColumnHeader("ֱ��");
	m_listBoltRect.AddColumnHeader("X");
	m_listBoltRect.AddColumnHeader("Y");
	m_listBoltRect.AddColumnHeader("Z");
}

void CXInsertPlateIntersTubeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXInsertPlateIntersTubeDlg)
	DDX_Control(pDX, IDC_BOLT_RECT_LIST, m_listBoltRect);
	DDX_CBString(pDX, IDC_CMB_BOLT_SIZE, m_sBoltSize);
	DDX_CBIndex(pDX, IDC_CMB_COVER_PLATE_MAT, m_iCoverPlateMat);
	DDX_CBIndex(pDX, IDC_CMB_WRAP_PLATE_MAT, m_iWrapPlateMat);
	DDX_CBIndex(pDX, IDC_CMB_DATUM_PLATE_MAT, m_iDatumPlateMat);
	DDX_CBIndex(pDX, IDC_CMB_TRANS_PLATE_MAT, m_iTransPlateMat);
	DDX_CBIndex(pDX, IDC_CMB_VERT_PLATE_MAT, m_iVertPlateMat);
	DDX_Text(pDX, IDC_E_BOLT_NUM, m_nBoltNum);
	DDX_Text(pDX, IDC_E_COVER_PLATE_PART_NO, m_sCoverPlatePartNo);
	DDX_Text(pDX, IDC_E_COVER_PLATE_SECTOR_ANGLE, m_fCoverPlateSectorAngle);
	DDX_Text(pDX, IDC_E_COVER_PLATE_THICK, m_nCoverPlateThick);
	DDX_Text(pDX, IDC_E_COVER_PLATE_WIDTH, m_nCoverPlateWidth);
	DDX_Text(pDX, IDC_E_COVER_PLATE_CUT_WIDTH, m_nCoverPlateCutWidth);
	DDX_Text(pDX, IDC_E_DATUM_PLATE_PART_NO, m_sDatumPlatePartNo);
	DDX_Text(pDX, IDC_E_DATUM_PLATE_THICK, m_nDatumPlateThick);
	DDX_Text(pDX, IDC_E_DATUM_PLATE_WIDTH, m_nDatumPlateWidth);
	DDX_Text(pDX, IDC_E_WELD_PLATE_PART_NO, m_sWeldPlatePartNo);
	DDX_Text(pDX, IDC_E_WRAP_PLATE_PART_NO, m_sWrapPlatePartNo);
	DDX_Text(pDX, IDC_E_TRANS_PLATE_PART_NO, m_sTransPlatePartNo);
	DDX_Text(pDX, IDC_E_TRANS_PLATE_THICK, m_nTransPlateThick);
	DDX_Text(pDX, IDC_E_VERT_PLATE_PART_NO, m_sVertPlatePartNo);
	DDX_Text(pDX, IDC_E_VERT_PLATE_THICK, m_nVertPlateThick);
	DDX_Text(pDX, IDC_E_VERT_PLATE_WIDTH, m_nVertPlateWidth);
	DDX_Text(pDX, IDC_E_SEG_I, m_sSegI);
	DDX_Text(pDX, IDC_E_DATUM_PLATE_LEN, m_nDatumPlateLen);
	DDX_Text(pDX, IDC_E_INSERT_PLATE_WELD_LEN, m_nInsertPlateWeldLen);
	DDX_Text(pDX, IDC_E_WELD_SPACE, m_nWeldSpace);
	DDX_Text(pDX, IDC_E_TUBE_ODDMENT, m_nTubeOddment);
	DDX_Check(pDX, IDC_CHK_STD_PART, m_bStdPart);
	DDX_Text(pDX, IDC_E_HOLE_R, m_fHoleR);
	DDX_CBIndex(pDX, IDC_CMB_HOLE_TYPE, m_iHoleType);
	DDX_Text(pDX, IDC_E_TRANS_PLATE_WIDTH, m_nTransPlateWidth);
	DDX_Check(pDX, IDC_CHK_TRANS_PLATE, m_bTransPlate);
	DDX_CBString(pDX, IDC_CMB_STD_PLATE_PART_NO, m_sStdPlatePartNo);
	DDX_CBIndex(pDX, IDC_INSERT_PLATE_TYPE, m_iInsertPlateType);
	DDX_Check(pDX, IDC_CHK_STAGGER_LAYOUT_BOLT, m_bStaggerLayoutBolt);
	DDX_Check(pDX, IDC_CHK_XEND_DOUBLE_ROW_LAYOUT, m_bXEndDoubleRowLayout);
	DDX_Text(pDX, IDC_E_BOLT_COL_NUM, m_nBoltColNum);
	DDX_Text(pDX, IDC_E_BOLT_ROW_NUM, m_nBoltRowNum);
	DDX_Text(pDX, IDC_E_BOLT_ROW_SPACE, m_fBoltRowSpace);
	DDX_Text(pDX, IDC_E_BOLT_COL_SPACE, m_fBoltColSpace);
	DDX_Text(pDX, IDC_E_FIRST_BOLT_POS_X, m_fFirstBoltPosX);
	DDX_Text(pDX, IDC_E_FIRST_BOLT_POS_Y, m_fFirstBoltPosY);
	DDX_Text(pDX, IDC_E_FIRST_BOLT_POS_Z, m_fFirstBoltPosZ);
	DDX_Text(pDX, IDC_E_PLATE_PARAM_A, m_fPlateParamA);
	DDX_Text(pDX, IDC_E_PLATE_PARAM_N, m_fPlateParamN);
	DDX_Text(pDX, IDC_E_PLATE_PARAM_R, m_fPlateParamR);
	DDX_Text(pDX, IDC_E_PLATE_PARAM_S, m_fPlateParamS);
	DDX_Text(pDX, IDC_E_SPEC_BOLT_SPACE_D, m_fSpecBoltSpaceD);
	DDX_Text(pDX, IDC_E_PLATE_PARAM_W, m_fPlateParamW);
	DDX_Check(pDX, IDC_CHK_CREATE_VERT_PLATE, m_bCreateVertPlate);
	DDX_Text(pDX, IDC_E_COVER_PLATE_OFFSET_ANGLE, m_fCoverPlateOffsetAngle);
	DDX_Check(pDX, IDC_CHK_USE_ANGLE, m_bUseAngle);
	DDX_Text(pDX, IDC_E_COVER_PLATE_MANU_SPACE, m_fCoverPlateManuSpace);
	DDV_MinMaxDouble(pDX, m_fCoverPlateManuSpace, 0., 100.);
	DDX_Text(pDX, IDC_E_VERT_PLATE_MANU_SPACE, m_fVertPlateManuSpace);
	DDV_MinMaxDouble(pDX, m_fVertPlateManuSpace, 0., 100.);
	DDX_CBString(pDX, IDC_CMB_JG_GUIGE, m_sJgGuige);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CXInsertPlateIntersTubeDlg, CDialog)
	//{{AFX_MSG_MAP(CXInsertPlateIntersTubeDlg)
	ON_CBN_SELCHANGE(IDC_CMB_DATUM_PLATE_MAT, OnSelchangeDatumPlateMat)
	ON_EN_CHANGE(IDC_E_DATUM_PLATE_PART_NO, OnChangeDatumPlatePartNo)
	ON_CBN_SELCHANGE(IDC_CMB_TRANS_PLATE_MAT, OnSelchangeTransPlateMat)
	ON_EN_CHANGE(IDC_E_TRANS_PLATE_THICK, OnChangeTransPlateThick)
	ON_BN_CLICKED(IDC_CHK_STD_PART, OnStdPart)
	ON_EN_CHANGE(IDC_E_WELD_SPACE, UpdateTubeOddment)
	ON_BN_CLICKED(IDC_BN_PARTS_DB, OnBnPartsDb)
	ON_CBN_SELCHANGE(IDC_CMB_BOLT_SIZE, OnSelchangeCmbBoltSize)
	ON_EN_CHANGE(IDC_E_INSERT_PLATE_WELD_LEN, OnChangeEInsertPlateWeldLen)
	ON_CBN_SELCHANGE(IDC_CMB_STD_PLATE_PART_NO, OnSelChangeStdPlatePartNo)
	ON_CBN_SELCHANGE(IDC_INSERT_PLATE_TYPE, OnSelchangeInsertPlateType)
	ON_NOTIFY(LVN_KEYDOWN, IDC_BOLT_RECT_LIST, OnKeydownBoltRectList)
	ON_NOTIFY(NM_DBLCLK, IDC_BOLT_RECT_LIST, OnDblclkBoltRectList)
	ON_BN_CLICKED(IDC_BTN_LAYOUT_BOLT, OnLayoutBolt)
	ON_BN_CLICKED(IDC_BTN_ADD_NEW_BOLT_ITEM, OnAddNewBoltItem)
	ON_BN_CLICKED(IDC_BTN_MODIFY_BOLT_ITEM, OnModifyBoltItem)
	ON_BN_CLICKED(IDC_CHK_XEND_DOUBLE_ROW_LAYOUT, OnXendDoubleRowLayout)
	ON_EN_CHANGE(IDC_E_PLATE_PARAM_W, OnChangePlateParamW)
	ON_BN_CLICKED(IDC_CHK_CREATE_VERT_PLATE, OnCreateVertPlate)
	ON_BN_CLICKED(IDC_CHK_USE_ANGLE, OnChkUseAngle)
	ON_CBN_SELCHANGE(IDC_CMB_JG_GUIGE, OnSelchangeJgGuige)
	ON_CBN_EDITCHANGE(IDC_CMB_JG_GUIGE, OnEditchangeJgGuige)
	ON_CBN_KILLFOCUS(IDC_CMB_JG_GUIGE, OnKillfocusJgGuige)
	ON_CBN_SELCHANGE(IDC_CMB_HOLE_TYPE, OnCbnSelchangeCmbHoleType)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_E_COVER_PLATE_PART_NO, &CXInsertPlateIntersTubeDlg::OnEnChangeECoverPlatePartNo)
	ON_EN_CHANGE(IDC_E_VERT_PLATE_PART_NO, &CXInsertPlateIntersTubeDlg::OnEnChangeEVertPlatePartNo)
	ON_EN_CHANGE(IDC_E_TRANS_PLATE_PART_NO, &CXInsertPlateIntersTubeDlg::OnEnChangeETransPlatePartNo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXInsertPlateIntersTubeDlg message handlers

BOOL CXInsertPlateIntersTubeDlg::OnInitDialog() 
{
	if(m_pCrossTube==NULL||m_pDatumTube==NULL)
		return FALSE;
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_COVER_PLATE_MAT);
	AddSteelMatRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_DATUM_PLATE_MAT);
	AddSteelMatRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_WRAP_PLATE_MAT);
	AddSteelMatRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_TRANS_PLATE_MAT);
	AddSteelMatRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_VERT_PLATE_MAT);
	AddSteelMatRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_JG_GUIGE);
	AddJgRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_HOLE_TYPE);
	pCMB->AddString("��");
	pCMB->AddString("Բ��");
	pCMB->AddString("��Բ");
	CEdit* pEdit=(CEdit*)GetDlgItem(IDC_E_HOLE_R);
	pEdit->SetReadOnly(m_iHoleType!=2);
	//��ʼ���Ǹֹ�� wht 11-06-01
	m_sJgGuige.Format("%.fX%.f",m_fPlateParamN+m_fPlateParamR-m_nDatumPlateThick*0.5,m_fPlateParamW);
	GetDlgItem(IDC_CMB_JG_GUIGE)->EnableWindow(m_bUseAngle);
	//
	RefreshDlgCtrlState();
	m_sSegI=m_pDatumTube->iSeg.ToString();	//Ĭ�϶κ�ͬ��׼�ֹܶκ�һ��
	long col_wide_arr[4]={40,50,50,50};
	m_listBoltRect.InitListCtrl(col_wide_arr);
	//�����˨�б�
	m_listBoltRect.DeleteAllItems();
	CDialog::OnInitDialog();
	return TRUE;
}

void CXInsertPlateIntersTubeDlg::OnSelchangeDatumPlateMat() 
{
	UpdateData();
	m_iWrapPlateMat=m_iDatumPlateMat;
	UpdateData(FALSE);
	//�޸Ļ�����ʺ�������¹�����ʾ״̬��wht 10-07-14
	//char cMat=steelmat_tbl[m_iDatumPlateMat].cBriefMark;
	//RefreshDlgCtrlState();
}

void CXInsertPlateIntersTubeDlg::OnSelchangeTransPlateMat() 
{
	UpdateData();
	m_iVertPlateMat=m_iTransPlateMat;
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::CalOddment()
{
	f3dPoint datum_line_vec=m_pDatumTube->End()-m_pDatumTube->Start();
	f3dPoint cross_line_vec=m_pCrossTube->End()-m_pCrossTube->Start();
	normalize(datum_line_vec);
	normalize(cross_line_vec);
	double sina = fabs(datum_line_vec*cross_line_vec);
	double cosa = sqrt(1-sina*sina);
	double oddment=0;
	double plate_len=m_nDatumPlateLen-m_nInsertPlateWeldLen;
	if(m_iInsertPlateType==2)
		plate_len=m_fPlateParamS+m_fPlateParamA;
	oddment=-(m_pDatumTube->GetDiameter()*0.5+m_nWeldSpace)/cosa-plate_len
		-m_nDatumPlateWidth*0.5*sina/cosa;//+g_sysPara.VertexDist
	oddment=fto_halfi(oddment/10)*10;
	m_nTubeOddment = ftoi(oddment);	//����ͷ*/
}

void CXInsertPlateIntersTubeDlg::UpdateTubeOddment() 
{	
	UpdateData();
	CalOddment();//����ֹ�����ͷ
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::OnChangeTransPlateThick() 
{
	UpdateData();
	//m_nVertPlateThick=m_nTransPlateThick;
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::OnChangeDatumPlatePartNo() 
{
	UpdateData();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sDatumPlatePartNo,BOMPART::PLATE);
	if(pPart)
	{	//�����ҵ���BOM��Ϣ�����¸øְ�Ļ�����Ϣ(�κš���񡢲���)
		m_nDatumPlateThick=(UINT)pPart->thick;
		m_iDatumPlateMat=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
	}
#endif
	SEGI segi;
	if(ParsePartNo(m_sDatumPlatePartNo.GetBuffer(),&segi,NULL))
		m_sSegI=segi.ToString();
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::OnStdPart()
{
	UpdateData();
	RefreshDlgCtrlState();
	OnChkUseAngle();	//���°��ֹ��
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::RefreshDlgCtrlState()
{
	int i=0;
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_INSERT_PALTE_TYPE);
	m_CurPlateBmp.Detach();
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_STD_PLATE_PART_NO);
	INSERT_PLATE_PARAM *pPlateParam=NULL,*pFirstPlateParam=NULL;
	while(pCMB->GetCount()>0)
		pCMB->DeleteString(0);
	for(i=0;i<9;i++)
	{	//�˴���ʱ�����ҵ�һ��ƥ���ʮ�ֲ�壬Ӧ�����һ������������ wht 10-01-19
		if(m_iInsertPlateType==0)
			pPlateParam=FindRollEndParam(m_pCrossTube->GetDiameter(),m_pCrossTube->GetThick(),m_pCrossTube->cMaterial,i+1);
		else if(m_iInsertPlateType==1)
			pPlateParam=FindUEndParam(m_pCrossTube->GetDiameter(),m_pCrossTube->GetThick(),m_pCrossTube->cMaterial,i+1);
		else if(m_iInsertPlateType==2)
			pPlateParam=FindXEndParam(m_pCrossTube->GetDiameter(),m_pCrossTube->GetThick(),m_pCrossTube->cMaterial,i+1);
		if(pPlateParam&&pCMB)
		{
			if(pFirstPlateParam==NULL)
				pFirstPlateParam=pPlateParam;
			if(pCMB->FindString(-1,pPlateParam->codeName)==CB_ERR)
				pCMB->AddString(pPlateParam->codeName);
		}
	}
	if(pFirstPlateParam)
	{	//�������ɱ�׼����
		m_sStdPlatePartNo.Format("%s",pFirstPlateParam->codeName);
		GetDlgItem(IDC_CHK_STD_PART)->EnableWindow(TRUE);
	}
	else 
	{
		m_sStdPlatePartNo.Empty();
		GetDlgItem(IDC_CHK_STD_PART)->EnableWindow(FALSE);
		m_bStdPart=FALSE;	//δ�ҵ����������ı�׼����
	}
	if(m_iInsertPlateType==2)
	{	//ʮ�ֲ��
		m_bCreateVertPlate=TRUE;	//ʮ�ֲ����Ҫ
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_XEND_BOLT);
		//������˨ʱ����ز���
		GetDlgItem(IDC_S_BOLT_ROW)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_BOLT_ROW_NUM)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_S_BOLT_COL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_BOLT_COL_NUM)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_S_SPACE_D)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_SPEC_BOLT_SPACE_D)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHK_XEND_DOUBLE_ROW_LAYOUT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_STAGGER_LAYOUT_BOLT)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BOLT_ROW_SPACE)->EnableWindow(m_bXEndDoubleRowLayout);
		//�����ز���
		GetDlgItem(IDC_E_WELD_PLATE_PART_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_WRAP_PLATE_PART_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_CMB_WRAP_PLATE_MAT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CMB_STD_PLATE_PART_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_PLATE_PARAM_A)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_PLATE_PARAM_R)->EnableWindow(TRUE);
		GetDlgItem(IDC_S_PLATE_PART_NO)->SetWindowText("������:");
	}
	else //if(m_iInsertPlateType==0||m_iInsertPlateType==1)
	{	//C�Ͳ���U�Ͳ��
		m_bCreateVertPlate=FALSE;
		if(m_iInsertPlateType==0)	//���Ͳ��
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_ROLLEND_BOLT);
		else if(m_iInsertPlateType==1)//U�Ͳ��
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_UEND_BOLT);
		//������˨ʱ����ز���
		GetDlgItem(IDC_S_BOLT_ROW)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_BOLT_ROW_NUM)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_S_BOLT_COL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_BOLT_COL_NUM)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_S_SPACE_D)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_SPEC_BOLT_SPACE_D)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_XEND_DOUBLE_ROW_LAYOUT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_STAGGER_LAYOUT_BOLT)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BOLT_ROW_SPACE)->EnableWindow(TRUE);
		//�����ز���
		GetDlgItem(IDC_E_WELD_PLATE_PART_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_WRAP_PLATE_PART_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMB_WRAP_PLATE_MAT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMB_STD_PLATE_PART_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_PLATE_PARAM_A)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_PLATE_PARAM_R)->EnableWindow(FALSE);
		GetDlgItem(IDC_S_PLATE_PART_NO)->SetWindowText("�����:");
	}
	GetDlgItem(IDC_CMB_STD_PLATE_PART_NO)->EnableWindow(m_bStdPart);
	GetDlgItem(IDC_E_WELD_PLATE_PART_NO)->EnableWindow(!m_bStdPart&&(m_iInsertPlateType==2));
	GetDlgItem(IDC_E_WRAP_PLATE_PART_NO)->EnableWindow(!m_bStdPart&&(m_iInsertPlateType==2));
	GetDlgItem(IDC_E_DATUM_PLATE_PART_NO)->EnableWindow(!m_bStdPart);
	GetDlgItem(IDC_CMB_DATUM_PLATE_MAT)->EnableWindow(!m_bStdPart);
	GetDlgItem(IDC_CMB_WRAP_PLATE_MAT)->EnableWindow(!m_bStdPart);
	GetDlgItem(IDC_BTN_ADD_NEW_BOLT_ITEM)->EnableWindow(!m_bStdPart);
	GetDlgItem(IDC_BTN_MODIFY_BOLT_ITEM)->EnableWindow(!m_bStdPart);
	GetDlgItem(IDC_BTN_LAYOUT_BOLT)->EnableWindow(!m_bStdPart);
	//������β���
	GetDlgItem(IDC_E_DATUM_PLATE_THICK)->EnableWindow(!m_bStdPart);
	GetDlgItem(IDC_E_DATUM_PLATE_WIDTH)->EnableWindow(!m_bStdPart);
	GetDlgItem(IDC_E_DATUM_PLATE_LEN)->EnableWindow(!m_bStdPart);
	GetDlgItem(IDC_E_INSERT_PLATE_WELD_LEN)->EnableWindow(!m_bStdPart);
	GetDlgItem(IDC_E_PLATE_PARAM_W)->EnableWindow(!m_bStdPart);
	GetDlgItem(IDC_E_PLATE_PARAM_S)->EnableWindow(!m_bStdPart);
	GetDlgItem(IDC_E_PLATE_PARAM_N)->EnableWindow(!m_bStdPart);
	GetDlgItem(IDC_E_PLATE_PARAM_A)->EnableWindow(!m_bStdPart&&(m_iInsertPlateType==2));
	GetDlgItem(IDC_E_PLATE_PARAM_R)->EnableWindow(!m_bStdPart&&(m_iInsertPlateType==2));
	//������������
	GetDlgItem(IDC_CMB_VERT_PLATE_MAT)->EnableWindow(m_bCreateVertPlate);
	GetDlgItem(IDC_E_VERT_PLATE_PART_NO)->EnableWindow(m_bCreateVertPlate);
	GetDlgItem(IDC_E_VERT_PLATE_THICK)->EnableWindow(m_bCreateVertPlate);
	GetDlgItem(IDC_E_VERT_PLATE_WIDTH)->EnableWindow(m_bCreateVertPlate);
	if(pFirstPlateParam)	//m_bStdPart&&
	{	//�����ɱ�׼����,��ȡ��׼��������
		m_iWrapPlateMat=QuerySteelMatIndex(pFirstPlateParam->cMaterial);
		m_iDatumPlateMat=m_iWrapPlateMat;
		m_nBoltNum=0;
		for(int i=0;i<pFirstPlateParam->iBoltRect;i++)
			m_nBoltNum+=pFirstPlateParam->ls_param[i].iBoltNum;
		if(pFirstPlateParam->iBoltRect>0)
			m_sBoltSize.Format("%d",pFirstPlateParam->ls_param[0].iBoltD);
		m_nDatumPlateThick=ftoi(pFirstPlateParam->Thick);
		m_nDatumPlateWidth=ftoi(pFirstPlateParam->H);
		m_nDatumPlateLen=ftoi(pFirstPlateParam->L);
		m_nInsertPlateWeldLen=ftoi(pFirstPlateParam->M);
		m_fPlateParamW=pFirstPlateParam->W;
		m_fPlateParamN=pFirstPlateParam->N;
		m_fPlateParamS=pFirstPlateParam->S;
		m_fPlateParamA=pFirstPlateParam->A;
		m_fPlateParamR=pFirstPlateParam->R;
		if(m_iInsertPlateType==2)
		{
			m_sDatumPlatePartNo.Format("%s",pFirstPlateParam->datumPlatePartNo);
			m_sWeldPlatePartNo.Format("%s",pFirstPlateParam->weldPlatePartNo);
			m_sWrapPlatePartNo.Format("%s",pFirstPlateParam->wrapPlatePartNo);
			//�����������Ӧ����ʮ�ֲ�������ȱ���һ�� wht 10-01-24
			m_nVertPlateThick=(int)m_fPlateParamW;
		}
		else 
			m_sDatumPlatePartNo.Format("%s",pFirstPlateParam->codeName);
		if(m_iInsertPlateType==1)	//U�Ͱ�
			m_nTransPlateThick=(int)m_fPlateParamW;
	}
	pPic->SetBitmap(m_CurPlateBmp);
	CalOddment();//����ֹ�����ͷ
	UpdateData(FALSE);
}


void CXInsertPlateIntersTubeDlg::OnBnPartsDb() 
{
	UpdateData();
	CPartsDbDlg partsDlg;
	partsDlg.m_bRefreshDlgCtrl = TRUE;
	if(m_iInsertPlateType==0)
		partsDlg.m_bIncParamRollEnd=TRUE;
	else if(m_iInsertPlateType==1)
		partsDlg.m_bIncParamUend=TRUE;
	else
		partsDlg.m_bIncParamXEnd = TRUE;
	if(partsDlg.DoModal()==IDOK)
	{
		CLDSParamPlate *pSrcParamPlate=(CLDSParamPlate*)partsDlg.m_pSelectPart;
		if(!pSrcParamPlate)
			return;
		if(pSrcParamPlate->m_iParamType!=TYPE_XEND)
			return;

		CLDSStdPartModel stdPartModel;
		if(pSrcParamPlate->m_bStdPart&&(!pSrcParamPlate->GetStdPartModel(&stdPartModel)))
			return;
		if(pSrcParamPlate->m_bStdPart)	//��׼����
		{
			m_nBoltNum=0;
			for(int i=0;i<stdPartModel.param.insert_plate_param.iBoltRect;i++)
				m_nBoltNum+=stdPartModel.param.insert_plate_param.ls_param[i].iBoltNum;
			if(stdPartModel.param.insert_plate_param.iBoltRect>0)
				m_sBoltSize.Format("%d",stdPartModel.param.insert_plate_param.ls_param[0].iBoltD);
			m_nDatumPlateThick=ftoi(stdPartModel.param.insert_plate_param.Thick);
			m_nDatumPlateWidth=ftoi(stdPartModel.param.insert_plate_param.H);
			m_nDatumPlateLen=ftoi(stdPartModel.param.insert_plate_param.L);
			m_nInsertPlateWeldLen=ftoi(stdPartModel.param.insert_plate_param.M);
			m_fPlateParamW=stdPartModel.param.insert_plate_param.W;
			m_fPlateParamN=stdPartModel.param.insert_plate_param.N;
			m_fPlateParamS=stdPartModel.param.insert_plate_param.S;
			m_fPlateParamA=stdPartModel.param.insert_plate_param.A;
			m_fPlateParamR=stdPartModel.param.insert_plate_param.R;
			if(m_iInsertPlateType==2)
			{	
				m_sDatumPlatePartNo.Format("%s",stdPartModel.param.insert_plate_param.datumPlatePartNo);
				m_sWeldPlatePartNo.Format("%s",stdPartModel.param.insert_plate_param.weldPlatePartNo);
				m_sWrapPlatePartNo.Format("%s",stdPartModel.param.insert_plate_param.wrapPlatePartNo);
				//�����������Ӧ����ʮ�ֲ�������ȱ���һ�� wht 10-01-24
				m_nVertPlateThick=(int)m_fPlateParamW;
			}
			else
				m_sDatumPlatePartNo.Format("%s",stdPartModel.param.insert_plate_param.codeName);
			if(m_iInsertPlateType==1)	//U�Ͱ�
				m_nTransPlateThick=(int)m_fPlateParamW;
		}
		else
		{
			double temp=0;
			pSrcParamPlate->GetDesignItemValue('L',&temp);
			m_nDatumPlateLen=ftoi(temp);
			pSrcParamPlate->GetDesignItemValue('M',&temp);
			m_nInsertPlateWeldLen=ftoi(temp);
			pSrcParamPlate->GetDesignItemValue('H',&temp);
			m_nDatumPlateThick=ftoi(temp);
			pSrcParamPlate->GetDesignItemValue('W',&temp);
			m_fPlateParamW=temp;
			pSrcParamPlate->GetDesignItemValue('N',&temp);
			m_fPlateParamN=temp;
			pSrcParamPlate->GetDesignItemValue('S',&temp);
			m_fPlateParamS=temp;
			pSrcParamPlate->GetDesignItemValue('A',&temp);
			m_fPlateParamA=temp;
			pSrcParamPlate->GetDesignItemValue('R',&temp);
			m_fPlateParamR=temp;
			m_nBoltNum=pSrcParamPlate->GetLsCount();
			if(pSrcParamPlate->GetLsCount()>0)
				m_sBoltSize.Format("%d",pSrcParamPlate->GetFirstLsRef()->GetLsPtr()->get_d());
			if(m_iInsertPlateType==2)
			{	
				m_sDatumPlatePartNo.Format("%s",(char*)pSrcParamPlate->GetPartNo());
				//Ӧ�ñ������ӹ����б�õ���ȷ�Ĺ������ ��ʱ��ֵΪ�̶�ֵ wht 10-01-24
				m_sWeldPlatePartNo.Format("%s-P",(char*)pSrcParamPlate->GetPartNo());
				m_sWrapPlatePartNo.Format("%s-X",(char*)pSrcParamPlate->GetPartNo());
				//�����������Ӧ����ʮ�ֲ�������ȱ���һ�� wht 10-01-24
				m_nVertPlateThick=(int)m_fPlateParamW;
			}
			else
				m_sDatumPlatePartNo.Format("%s",(char*)pSrcParamPlate->GetPartNo());
			if(m_iInsertPlateType==1)	//U�Ͱ�
				m_nTransPlateThick=(int)m_fPlateParamW;
		}
		m_sSegI=pSrcParamPlate->iSeg.ToString();
		m_iDatumPlateMat=QuerySteelMatIndex(pSrcParamPlate->cMaterial);
		m_iWrapPlateMat=m_iDatumPlateMat;
		CalOddment();//���¼�������ͷ
		//��ѡ��Ĺ�������˨��ӵ���˨�б���  
		CLsRef *pLsRef=NULL;
		m_listBoltRect.DeleteAllItems();	//�����˨�б�
		for(pLsRef=pSrcParamPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcParamPlate->GetNextLsRef())
		{
			CStringArray str_arr;
			str_arr.SetSize(4);
			f3dPoint ls_pos=(*pLsRef)->ucs.origin;
			coord_trans(ls_pos,pSrcParamPlate->ucs,FALSE);
			//������˨�ķ���ƫ����
			//ȡ��˨�ı�־λ 11 12 21 22 31 32 41 42 ��ʮλ����  wht 10-01-24
			int ls_flag=0;
			if(pSrcParamPlate->m_iParamType==TYPE_XEND)
			{
				ls_flag=pLsRef->start1_end2_mid0/10;
				if(ls_flag==1||ls_flag==2)		//X-Yƽ���ϵ���˨
					ls_pos.z=-0.5*pSrcParamPlate->GetThick();
				else if(ls_flag==3||ls_flag==4)	//Y-Zƽ���ϵ���˨
					ls_pos.y=-0.5*pSrcParamPlate->GetThick();
			}
			else
				ls_pos.z=-0.5*pSrcParamPlate->GetThick();
			CString sBoltSpec;
			sBoltSpec.Format("M%d",(*pLsRef)->get_d());
			str_arr[0] = sBoltSpec;						//���
			str_arr[1].Format("%.2f",ls_pos.x);
			SimplifiedNumString(str_arr[1]);			//X
			str_arr[2].Format("%.2f",ls_pos.y);
			SimplifiedNumString(str_arr[2]);			//Y
			str_arr[3].Format("%.2f",ls_pos.z);
			SimplifiedNumString(str_arr[3]);			//Z	
			int iItem=m_listBoltRect.InsertItemRecord(-1,str_arr);
			m_listBoltRect.SetItemData(iItem,(DWORD)(ls_flag));//��˨λ�ñ�־
		}
		UpdateData(FALSE);
	}	
}

void CXInsertPlateIntersTubeDlg::OnSelchangeCmbBoltSize() 
{
	UpdateData();
	CComboBox* pCMB = (CComboBox*)GetDlgItem(IDC_CMB_BOLT_SIZE);
	if(pCMB==NULL)
		return;
	int iCurSel=pCMB->GetCurSel();
	if(iCurSel>=0)	//����ֻ�����뵫�޷�ѡ����˨ֱ�������� wht 11-07-01
		pCMB->GetLBText(iCurSel,m_sBoltSize);
	else
		pCMB->GetWindowText(m_sBoltSize);
	int d = atoi(m_sBoltSize);
	LSSPACE_STRU LsSpace;
	if(!GetLsSpace(LsSpace,d))
	{
		LsSpace.EndSpace=(int)(fto_halfi(d*0.15)*10);
		LsSpace.SingleRowSpace=2*LsSpace.EndSpace;
	}
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::OnChangeEInsertPlateWeldLen() 
{
	UpdateData();
	CalOddment();//����ֹ�����ͷ
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::OnSelChangeStdPlatePartNo() 
{
	UpdateData();
	char sPartNo[50]="";
	_snprintf(sPartNo,49,"%s",m_sStdPlatePartNo);
	INSERT_PLATE_PARAM *pPlateParam=NULL;
	if(m_iInsertPlateType==0)
		pPlateParam=FindRollEndParam(sPartNo);
	else if(m_iInsertPlateType==1)
		pPlateParam=FindUEndParam(sPartNo);
	else if(m_iInsertPlateType==2)
		pPlateParam=FindXEndParam(sPartNo);

	if(pPlateParam)
	{
		m_iWrapPlateMat=QuerySteelMatIndex(pPlateParam->cMaterial);
		m_iDatumPlateMat=m_iWrapPlateMat;
		m_sDatumPlatePartNo.Format("%s",pPlateParam->codeName);
		if(m_iInsertPlateType==2)
		{
			m_sDatumPlatePartNo.Format("%s",pPlateParam->datumPlatePartNo);
			m_sWeldPlatePartNo.Format("%s",pPlateParam->weldPlatePartNo);
			m_sWrapPlatePartNo.Format("%s",pPlateParam->wrapPlatePartNo);
		}
		m_nDatumPlateThick=ftoi(pPlateParam->Thick);
		m_nDatumPlateWidth=ftoi(pPlateParam->H);
		m_nDatumPlateLen=ftoi(pPlateParam->L);
		m_nInsertPlateWeldLen=ftoi(pPlateParam->M);
		m_fPlateParamW=pPlateParam->W;
		m_fPlateParamN=pPlateParam->N;
		m_fPlateParamS=pPlateParam->S;
		m_fPlateParamA=pPlateParam->A;
		m_fPlateParamR=pPlateParam->R;
	}
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::OnSelchangeInsertPlateType() 
{
	UpdateData();
	RefreshDlgCtrlState();
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::OnKeydownBoltRectList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	POSITION pos = m_listBoltRect.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = m_listBoltRect.GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_DELETE)
		{
			if(iCurSel>=0)
				m_listBoltRect.DeleteItem(iCurSel);
			pos = m_listBoltRect.GetFirstSelectedItemPosition();
		}
		else
			break;
	}
	*pResult = 0;
}

void CXInsertPlateIntersTubeDlg::OnDblclkBoltRectList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	POSITION pos = m_listBoltRect.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_listBoltRect.GetNextSelectedItem(pos);
		char sText[50]="";
		m_listBoltRect.GetItemText(iCurSel,0,sText,50);
		m_sBoltSize.Format("%s",sText+1);
		m_listBoltRect.GetItemText(iCurSel,1,sText,50);
		m_fFirstBoltPosX=atof(sText);
		m_listBoltRect.GetItemText(iCurSel,2,sText,50);
		m_fFirstBoltPosY=atof(sText);
		m_listBoltRect.GetItemText(iCurSel,3,sText,50);
		m_fFirstBoltPosZ=atof(sText);
		UpdateData(FALSE);
	}
	*pResult = 0;
}

void CXInsertPlateIntersTubeDlg::OnLayoutBolt() 
{
	UpdateData();
	int i,nBoltNum=m_nBoltNum;
	CString ss;
	CStringArray str_arr;
	CComboBox *pCmb = (CComboBox*)GetDlgItem(IDC_CMB_BOLT_SIZE);
	int iCur = pCmb->GetCurSel();
	if(iCur>=0)
		pCmb->GetLBText(iCur,m_sBoltSize);
	else
		pCmb->GetWindowText(m_sBoltSize);
	double posBolt[100][4];
	//��������ĵ�һ����˨��λ��,������,�Լ�����(��X��),��಼����˨
	//ʮ�ֲ���ϲ�����˨ʱ������Ҫ����˫����˨		wht 09-06-06
	//˫����˨�����ַ�ʽ����һ�־���Ĭ�ϵ�ÿ��С��������˨������ͬ��
	//					  �ڶ��־���ˮƽ������ϵ���˨����ֱ���ϵ���˨�ʽ�����
	if(m_iInsertPlateType==2)
	{	//�ڰ�������ϵ�²���ʮ�ֲ����˨
		double H=0,N=0,A=0,M=0,R=0,S=0;
		H=m_nDatumPlateWidth;
		N=m_fPlateParamN;
		M=m_nInsertPlateWeldLen;
		A=m_fPlateParamA;
		R=m_fPlateParamR;
		S=m_fPlateParamS;
		double fFirstBoltPosX=m_fFirstBoltPosX+A+M;
		double fFirstBoltPosY=m_fFirstBoltPosY+R;
		double fFirstBoltPosZ=m_fFirstBoltPosZ;
		//double Y=0.5*H-(N-(0.5*H-m_fBoltPosY));
		//����ʮ�ֲ����˨ʱ�û������Ϊ��԰��������˴���Ҫ����԰��������ת����ʮ�ֲ������ϵ��
		//ˮƽ������ֱ�潻����ʱ����˨Y������H�޹� wht 10-01-22
		double Y=R+(N-m_fFirstBoltPosY);	//������ʱ����˨Y����
		double offset_dist=0;
		if(m_bXEndDoubleRowLayout)
		{	//˫�Ų�����˨
			double fSecRowBoltY=0,fSecY=0;
			//�����׸���˨��λ���Զ��жϵڶ�����˨��Y����
			if((m_fFirstBoltPosY+m_fBoltRowSpace)>N)
				fSecRowBoltY=fFirstBoltPosY-m_fBoltRowSpace;
			else
				fSecRowBoltY=fFirstBoltPosY+m_fBoltRowSpace;
			fSecY=R+(N-(fSecRowBoltY-R));	//������ʱ����˨Y����
			for(i=0;i<nBoltNum;i++)
			{	//�ӵ�nBoltNum/2����˨��ʼ������������ɵ�ƫ��
				double pos_y=fFirstBoltPosY;
				if(i%2>0)
					pos_y=fSecRowBoltY;
				if(i>(nBoltNum/2-1))
				{	//�ڰ��嵫���ڻ����ϵ���˨����������������ɵ�ƫ��
					offset_dist=m_fSpecBoltSpaceD-m_fBoltColSpace;
					pos_y=fSecRowBoltY;
					if(i%2>0)
						pos_y=fFirstBoltPosY;
				}
				posBolt[4*i][0] = fFirstBoltPosX+m_fBoltColSpace*i+offset_dist;
				posBolt[4*i][1] = pos_y;
				posBolt[4*i][2] = fFirstBoltPosZ;
				posBolt[4*i][3] = 1;//X-Yƽ��: Y������һ�����˨
				posBolt[4*i+1][0] = fFirstBoltPosX+m_fBoltColSpace*i+offset_dist;
				posBolt[4*i+1][1] = -pos_y;
				posBolt[4*i+1][2] = fFirstBoltPosZ;
				posBolt[4*i+1][3] = 2;//X-Yƽ��: Y������һ�����˨
				if(m_bStaggerLayoutBolt)
				{
					pos_y=Y;
					if(i%2>0)
						pos_y=fSecY;	
					if(i>(nBoltNum/2-1))
					{
						pos_y=fSecY;
						if(i%2>0)
							pos_y=Y;
					}
					posBolt[4*i+2][0] = fFirstBoltPosX+m_fBoltColSpace*i+offset_dist;
					posBolt[4*i+2][1] = fFirstBoltPosZ;
					posBolt[4*i+2][2] = pos_y;
					posBolt[4*i+2][3] = 3;//X-Zƽ��: Z������һ�����˨
					posBolt[4*i+3][0] = fFirstBoltPosX+m_fBoltColSpace*i+offset_dist;
					posBolt[4*i+3][1] = fFirstBoltPosZ;
					posBolt[4*i+3][2] = -pos_y;
					posBolt[4*i+3][3] = 4;//X-Zƽ��: Z������һ�����˨
				}
				else
				{
					posBolt[4*i+2][0] = fFirstBoltPosX+m_fBoltColSpace*i+offset_dist;
					posBolt[4*i+2][1] = fFirstBoltPosZ;
					posBolt[4*i+2][2] = pos_y;
					posBolt[4*i+2][3] = 3;//X-Zƽ��: Z������һ�����˨
					posBolt[4*i+3][0] = fFirstBoltPosX+m_fBoltColSpace*i+offset_dist;
					posBolt[4*i+3][1] = fFirstBoltPosZ;
					posBolt[4*i+3][2] = -pos_y;
					posBolt[4*i+3][3] = 4;//X-Zƽ��: Z������һ�����˨
				}
			}
		}
		else 
		{	//ʹ�������಼�õ�����˨
			for(i=0;i<nBoltNum;i++)
			{	//�ӵ�nBoltNum/2����˨��ʼ������������ɵ�ƫ��
				if(i>(nBoltNum/2-1))
					offset_dist=m_fSpecBoltSpaceD-m_fBoltColSpace;
				posBolt[4*i][0] = fFirstBoltPosX+m_fBoltColSpace*i+offset_dist;
				posBolt[4*i][1] = fFirstBoltPosY;
				posBolt[4*i][2] = fFirstBoltPosZ;
				posBolt[4*i][3] = 1;//X-Yƽ��: Y������һ�����˨
				posBolt[4*i+1][0] = fFirstBoltPosX+m_fBoltColSpace*i+offset_dist;
				posBolt[4*i+1][1] = -fFirstBoltPosY;
				posBolt[4*i+1][2] = fFirstBoltPosZ;
				posBolt[4*i+1][3] = 2;//X-Yƽ��: Y������һ�����˨
				posBolt[4*i+2][0] = fFirstBoltPosX+m_fBoltColSpace*i+offset_dist;
				posBolt[4*i+2][1] = fFirstBoltPosZ;
				posBolt[4*i+2][2] = fFirstBoltPosY;
				posBolt[4*i+2][3] = 3;//X-Zƽ��: Z������һ�����˨
				posBolt[4*i+3][0] = fFirstBoltPosX+m_fBoltColSpace*i+offset_dist;
				posBolt[4*i+3][1] = fFirstBoltPosZ;
				posBolt[4*i+3][2] = -fFirstBoltPosY;
				posBolt[4*i+3][3] = 4;//X-Zƽ��: Z������һ�����˨
			}
		}
		nBoltNum*=4;
	}
	else
	{
		for(i=0;i<nBoltNum;i++)
		{
			posBolt[i][0] = m_fFirstBoltPosX+m_fBoltColSpace*(i/m_nBoltRowNum);
			posBolt[i][1] = m_fFirstBoltPosY+m_fBoltRowSpace*(i%m_nBoltRowNum);
			posBolt[i][2] = m_fFirstBoltPosZ;
			posBolt[i][3] = 0;
		}
		if(nBoltNum%m_nBoltRowNum)	//���һ����˨λ��������
			posBolt[nBoltNum-1][1] += 0.5*m_fBoltRowSpace;
	}
	str_arr.SetSize(4);
	for(i=0;i<nBoltNum;i++)
	{
		CString sBoltSpec;
		sBoltSpec.Format("M%s",m_sBoltSize);
		str_arr[0] = sBoltSpec;						//���
		str_arr[1].Format("%.2f",posBolt[i][0]);
		SimplifiedNumString(str_arr[1]);			//X
		str_arr[2].Format("%.2f",posBolt[i][1]);
		SimplifiedNumString(str_arr[2]);			//Y
		str_arr[3].Format("%.2f",posBolt[i][2]);
		SimplifiedNumString(str_arr[3]);			//Z	
		int iItem=m_listBoltRect.InsertItemRecord(-1,str_arr);
		m_listBoltRect.SetItemData(iItem,(DWORD)posBolt[i][3]);//��˨λ�ñ�־
	}
}

void CXInsertPlateIntersTubeDlg::OnAddNewBoltItem() 
{
	POSITION pos = m_listBoltRect.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_listBoltRect.GetNextSelectedItem(pos);
		UpdateData();
		CString sBoltSpec;
		sBoltSpec.Format("M%s",m_sBoltSize);
		m_listBoltRect.SetItemText(iCurSel,0,sBoltSpec);
		CXhChar100 ss;
		ss.Printf("%.2f",m_fFirstBoltPosX);
		SimplifiedNumString(ss);
		m_listBoltRect.SetItemText(iCurSel,1,ss);
		ss.Printf("%.2f",m_fFirstBoltPosY);
		SimplifiedNumString(ss);
		m_listBoltRect.SetItemText(iCurSel,2,ss);
		ss.Printf("%.2f",m_fFirstBoltPosZ);
		SimplifiedNumString(ss);
		m_listBoltRect.SetItemText(iCurSel,3,ss);
	}
}

void CXInsertPlateIntersTubeDlg::OnModifyBoltItem() 
{
	POSITION pos = m_listBoltRect.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_listBoltRect.GetNextSelectedItem(pos);
		UpdateData();
		CString sBoltSpec;
		sBoltSpec.Format("M%s",m_sBoltSize);
		m_listBoltRect.SetItemText(iCurSel,0,sBoltSpec);
		CXhChar100 ss;
		ss.Printf("%.2f",m_fFirstBoltPosX);
		SimplifiedNumString(ss);
		m_listBoltRect.SetItemText(iCurSel,1,ss);
		ss.Printf("%.2f",m_fFirstBoltPosY);
		SimplifiedNumString(ss);
		m_listBoltRect.SetItemText(iCurSel,2,ss);
		ss.Printf("%.2f",m_fFirstBoltPosZ);
		SimplifiedNumString(ss);
		m_listBoltRect.SetItemText(iCurSel,3,ss);
	}	
}

void CXInsertPlateIntersTubeDlg::CreateBolt(CLDSParamPlate *pParamPlate)
{	
	if(pParamPlate==NULL)
		return;
	DWORD dwRayNo = 0;
	//��˨���ݶ�����
	ATOM_LIST<RAYNO_RECORD>rayNoList;
	int iInitRayNo=1;
	f3dPoint *pPt=NULL;
	for(pPt=boltPosList.GetFirst();pPt;pPt=boltPosList.GetNext())
	{
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->set_d(atoi(m_sBoltSize.GetBuffer(10)));
		pBolt->iSeg = SEGI(m_sSegI.GetBuffer());
		pBolt->SetLayer(pParamPlate->layer());	//������˨��ͼ����
		pBolt->cfgword=pParamPlate->cfgword;	//������˨��ĺ����׼������ĺ�һ��
		pBolt->des_base_pos.hPart = pParamPlate->handle;
		pBolt->des_base_pos.cLocationStyle=TRUE; 
		pBolt->des_work_norm.norm_style=3;	//��������
		pBolt->des_work_norm.hVicePart=pParamPlate->handle;
		pBolt->des_work_norm.direction=0;
		pBolt->des_base_pos.len_offset_dist=pPt->x;
		double L=0;
		if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
		{	//ʮ�ֲ��	
			pParamPlate->GetDesignItemValue('L',&L);
			pBolt->des_base_pos.wing_offset_dist=pPt->y;
			if(pPt->feature==1)
			{	//X-Yƽ��: Y������һ�����˨
				if(L>pPt->x)//Y������һ��,���Ӳ����������˨
					pBolt->feature=11;
				else
					pBolt->feature=12;	//Y������һ��,���Ӱ����������������˨
				pBolt->des_base_pos.norm_offset.AddThick((LONG)pPt->z);
			}
			else if(pPt->feature==2)
			{	//X-Yƽ��: Y������һ�����˨
				if(L>pPt->x)//Y������һ��,���Ӳ����������˨
					pBolt->feature=21;
				else
					pBolt->feature=22;	//Y������һ��,���Ӱ����������������˨
				pBolt->des_base_pos.norm_offset.AddThick((LONG)pPt->z);
			}
			else if(pPt->feature==3)	
			{	//X-Zƽ��: Z������һ��
				if(L>pPt->x)
					pBolt->feature=31;	//Z������һ�࣬���������������������˨
				else
					pBolt->feature=32;	//Z������һ�࣬���Ӱ����������������˨
				pBolt->des_work_norm.norm_style=4;
				pBolt->des_work_norm.nearVector.Set(0,1,0);
				pBolt->des_base_pos.wing_offset_dist = pPt->z;
				pBolt->ucs.axis_z=pParamPlate->ucs.axis_y;
				pBolt->des_base_pos.norm_offset.AddThick((LONG)pPt->y);
			}
			else if(pPt->feature==4)	
			{	//X-Zƽ��: Z������һ��
				if(L>pPt->x)
					pBolt->feature=41;	//Z������һ�࣬���������������������˨
				else
					pBolt->feature=42;	//Z������һ�࣬���Ӱ����������������˨
				pBolt->des_work_norm.norm_style=4;
				pBolt->des_work_norm.nearVector.Set(0,1,0);
				pBolt->des_base_pos.wing_offset_dist = pPt->z;
				pBolt->ucs.axis_z=pParamPlate->ucs.axis_y;
				pBolt->des_base_pos.norm_offset.AddThick((LONG)pPt->y);
			}
			if(pBolt->feature==11||pBolt->feature==21)	//��˨��ʮ�ֲ�巶Χ���޸���˨ͨ��
				pBolt->AddL0Thick(pParamPlate->handle,TRUE);
		}
		else
		{
			pBolt->AddL0Thick(pParamPlate->handle,TRUE);
			pBolt->des_base_pos.wing_offset_dist = pPt->y;
			pBolt->des_base_pos.norm_offset.AddThick((LONG)pPt->z);
		}
		pBolt->des_work_norm.direction=0;	//0-���� 1-����
		for(RAYNO_RECORD *pRayNo=rayNoList.GetFirst();pRayNo;pRayNo=rayNoList.GetNext())
		{
			if(pRayNo->yCoord==pPt->y)
			{	
				pBolt->dwRayNo=pRayNo->dwRayNo;
				break;
			}
		}
		if(pRayNo==NULL)
		{
			pRayNo=rayNoList.append();
			pRayNo->dwRayNo=GetSingleWord(iInitRayNo);
			pRayNo->yCoord=pPt->y;
			pBolt->dwRayNo=pRayNo->dwRayNo;
			pRayNo->hPart=pParamPlate->handle;
			iInitRayNo++;
		}
		CLsRef *pLsRef=pParamPlate->AppendLsRef(pBolt->GetLsRef());
		if(pLsRef)
			pLsRef->start1_end2_mid0=(BYTE)pBolt->feature;	//�����˨����λ��
		pBolt->SetModified();
		pBolt->CalGuigeAuto();
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	//������˨�������ߺ�
	pParamPlate->GetLsRefList()->ResetBoltRefRayNo(pParamPlate);
}

void CXInsertPlateIntersTubeDlg::OnOK() 
{
	UpdateData();
	char sText[100]="";
	boltPosList.Empty();
	int nCount=m_listBoltRect.GetItemCount();
	for(int i=0;i<nCount;i++)
	{
		long h=(long)m_listBoltRect.GetItemData(i);
		if(h>0x20)	//ԭ����˨,����
			continue;
		f3dPoint *pPt=boltPosList.append();
		pPt->feature=0;	//���
		m_listBoltRect.GetItemText(i,1,sText,100);
		pPt->x=atof(sText);
		m_listBoltRect.GetItemText(i,2,sText,100);
		pPt->y=atof(sText);
		m_listBoltRect.GetItemText(i,3,sText,100);
		pPt->z=atof(sText);
		pPt->feature=h;
	}
	OnKillfocusJgGuige();	//ѡ����ʹ�ýǸִ������ʱ��Ҫ�ڵ��ȷ��ʱ����N,W���� wht 11-06-01
	CDialog::OnOK();
}

void CXInsertPlateIntersTubeDlg::OnXendDoubleRowLayout() 
{
	UpdateData();
	if(m_iInsertPlateType==2)
		GetDlgItem(IDC_E_BOLT_ROW_SPACE)->EnableWindow(m_bXEndDoubleRowLayout);
	else
		GetDlgItem(IDC_E_BOLT_ROW_SPACE)->EnableWindow(TRUE);
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::OnCreateVertPlate() 
{
	UpdateData();
	GetDlgItem(IDC_CMB_VERT_PLATE_MAT)->EnableWindow(m_bCreateVertPlate);
	GetDlgItem(IDC_E_VERT_PLATE_PART_NO)->EnableWindow(m_bCreateVertPlate);
	GetDlgItem(IDC_E_VERT_PLATE_THICK)->EnableWindow(m_bCreateVertPlate);
	GetDlgItem(IDC_E_VERT_PLATE_WIDTH)->EnableWindow(m_bCreateVertPlate);
	UpdateData(FALSE);
}
 
void CXInsertPlateIntersTubeDlg::OnChangePlateParamW() 
{
	UpdateData();
	if(m_iInsertPlateType==2)
		m_nVertPlateThick=(int)m_fPlateParamW;
	else if(m_iInsertPlateType==1)
		m_nTransPlateThick=(int)m_fPlateParamW;
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::OnChkUseAngle() 
{
	UpdateData();
	//��ʼ���Ǹֹ�� wht 11-06-01
	m_sJgGuige.Format("%.fX%.f",m_fPlateParamN+m_fPlateParamR-m_nDatumPlateThick*0.5,m_fPlateParamW);
	GetDlgItem(IDC_CMB_JG_GUIGE)->EnableWindow(m_bUseAngle);
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::OnSelchangeJgGuige() 
{	//�޸ĽǸֹ��ʱͬʱ���°����ȼ�������(���ɰ���ʱ����N,W����������ֹ��)
	UpdateData();
	if(!m_bUseAngle)
		return;
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_JG_GUIGE);
	int iCurSel = pCMB->GetCurSel();
	pCMB->GetLBText(iCurSel,m_sJgGuige);
	double wing_width=0,wing_thick=0;
	restore_JG_guige(m_sJgGuige.GetBuffer(),wing_width,wing_thick);
	m_fPlateParamW=wing_thick;
	m_fPlateParamN=wing_width+m_nDatumPlateThick-m_fPlateParamR;
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::OnEditchangeJgGuige() 
{
	/*UpdateData();
	if(!m_bUseAngle)
		return;
	double wing_width=0,wing_thick=0;
	restore_JG_guige(m_sJgGuige,wing_width,wing_thick);
	m_fPlateParamW=wing_thick;
	m_fPlateParamN=wing_width+m_nDatumPlateThick-m_fPlateParamR;
	UpdateData(FALSE);*/
}

void CXInsertPlateIntersTubeDlg::OnKillfocusJgGuige() 
{
	UpdateData();
	if(!m_bUseAngle)
		return;
	double wing_width=0,wing_thick=0;
	restore_JG_guige(m_sJgGuige.GetBuffer(),wing_width,wing_thick);
	m_fPlateParamW=wing_thick;
	m_fPlateParamN=wing_width+m_nDatumPlateThick-m_fPlateParamR;
	UpdateData(FALSE);
}
void CXInsertPlateIntersTubeDlg::OnCbnSelchangeCmbHoleType()
{
	UpdateData();
	CEdit* pEdit=(CEdit*)GetDlgItem(IDC_E_HOLE_R);
	pEdit->SetReadOnly(m_iHoleType!=2);
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::OnEnChangeECoverPlatePartNo()
{
	UpdateData();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sCoverPlatePartNo,BOMPART::PLATE);
	if(pPart)
	{	//�����ҵ���BOM��Ϣ�����¸øְ�Ļ�����Ϣ(�κš���񡢲���)
		m_nCoverPlateThick=(UINT)pPart->thick;
		m_iCoverPlateMat=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
	}
#endif
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::OnEnChangeEVertPlatePartNo()
{
	UpdateData();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sVertPlatePartNo,BOMPART::PLATE);
	if(pPart)
	{	//�����ҵ���BOM��Ϣ�����¸øְ�Ļ�����Ϣ(�κš���񡢲���)
		m_nVertPlateThick=(UINT)pPart->thick;
		m_iVertPlateMat=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
	}
#endif
	UpdateData(FALSE);
}

void CXInsertPlateIntersTubeDlg::OnEnChangeETransPlatePartNo()
{
	UpdateData();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sTransPlatePartNo,BOMPART::PLATE);
	if(pPart)
	{	//�����ҵ���BOM��Ϣ�����¸øְ�Ļ�����Ϣ(�κš���񡢲���)
		m_nTransPlateThick=(UINT)pPart->thick;
		m_iTransPlateMat=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
	}
#endif
	UpdateData(FALSE);
}

#endif