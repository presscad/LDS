// LayAngleBoltDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "LayAngleBoltDlg.h"
#include "GlobalFunc.h"
#include "ScrTools.h"
#include "env_def.h"
#include "ModifyJgLsDlg.h"
#include "LmaDlg.h"
#include "btc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CLayAngleBoltDlg dialog

int CLayAngleBoltDlg::nScrLocationX=0;
int CLayAngleBoltDlg::nScrLocationY=0;
int CLayAngleBoltDlg::PREV_BERSPACE_MODE =0;
int CLayAngleBoltDlg::PREV_WINGSPACE_MODE=0;
CLayAngleBoltDlg::CLayAngleBoltDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CLayAngleBoltDlg::IDD, pParent)
	, m_iBerSpaceMode(0)
	, m_iWingSpaceMode(0)
{
	//{{AFX_DATA_INIT(CLayAngleBoltDlg)
	m_bEnableWindowMoveListen=true;
	m_bMirLayLs = FALSE;
	m_nCrossAngleG = 0;
	m_hCurEditOddPart=0;
	m_nG = 0;
	m_nG1 = 0;
	m_nG2 = 0;
	m_nLsEndSpace = 0;
	m_nLsNum = 0;
	m_nLsSpace = 0;
	m_iLsLayoutStyle = 0;
	m_iBoltNorm = 0;
	m_iRayDirection = 0;
	m_hBaseNode = _T("");
	m_sRayAngleSize = _T("");
	m_iDatumPointStyle = 0;
	m_iOffsetWing = -1;	//-1��ʾ���ݵ�ǰ������ͼ���Զ�������˨���ڸ˼��Ĺ���֫
	m_hCrossAngle = _T("");
	m_fWingOffsetDist = 0.0;
	m_fLenOffsetDist = 0.0;
	m_bTwoEdgeProfile = FALSE;
	m_bUseSdtEdgeSpace=FALSE;
	m_fBerSpace = 0.0;
	m_fWingSpace = 0.0;
	m_sBoltSize = _T("");
	m_fOddment = 0.0;
	m_sExtraOdd = _T("");
	m_iOddCalStyle=2;
	m_fAngleEndNormOffset = 0.0;
	m_fPlateEdgeSpace = 0.0;
	m_iBoltRows = 0;
	m_nFirstLsJgSpace = 0;
	m_iType = -1;
	//}}AFX_DATA_INIT
	m_hPlate=0;
	m_pLinePart=NULL;
	m_pNode=NULL;
	m_bEndLjJg=FALSE;
	m_bEndLjDatumJg=FALSE;
	m_pEndLjRayJg=NULL;
	m_bIncPlateProfilePara=FALSE;
	face_norm_offset.Set(0,0,1);
	m_bEnable2RowsLs=TRUE;
	m_bRayEndFaceOffset=FALSE;
	m_pLegacyLsList=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	m_listOddRefRods.AddColumnHeader("Ends Reference Rod",160);
#else 
	m_listOddRefRods.AddColumnHeader("��ͷ���ո˼�",105);
#endif
	m_fHuoquWingOffsetDist=0;
	m_ciSectType = 0;
	m_iHoleFuncType = 0;
	m_bPunchHole = TRUE;
	m_bDrillHole = !m_bPunchHole;
}
static BOOL FireDeleteItem(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem); //Ϊ�˷������Ա� �ڴ����� 
static BOOL FireLButtonDblclk(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,int iSubItem);  //Ϊ�˷������Ա� �ڴ�����
static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem,CString& sTextValue)
{
	CDesignLsPara *pDesignLsPara=(CDesignLsPara*)pSelItem->m_idProp;
	if(pDesignLsPara==NULL)
		return FALSE;
	CLayAngleBoltDlg *pLayAngleBoltDlg=(CLayAngleBoltDlg*)pListCtrl->GetParent();
	if(iSubItem==0)
	{
		if(sTextValue.CompareNoCase("M12")==0)
			pDesignLsPara->d =12;
		else if(sTextValue.CompareNoCase("M16")==0)
			pDesignLsPara->d =16;
		else if(sTextValue.CompareNoCase("M20")==0)
			pDesignLsPara->d =20;
		else if(sTextValue.CompareNoCase("M24")==0)
			pDesignLsPara->d =24;
		else if(sTextValue.CompareNoCase("M27")==0)
			pDesignLsPara->d =27;
		else 
			pDesignLsPara->d =atoi(sTextValue);
		pLayAngleBoltDlg->UpdatePlateSpace();
		pLayAngleBoltDlg->PreviewBolt(pDesignLsPara);	//Ԥ����˨
	}
	else if(iSubItem==2)
	{
		pDesignLsPara->des_base_pos.len_offset_dist = atof(sTextValue);
		pLayAngleBoltDlg->PreviewBolt(pDesignLsPara);	//Ԥ����˨
	}
	else if(iSubItem==3)
	{
		pDesignLsPara->des_base_pos.wing_offset_dist = atof(sTextValue);
		pLayAngleBoltDlg->PreviewBolt(pDesignLsPara);	//Ԥ����˨
	}
	return TRUE;
}
void CLayAngleBoltDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLayAngleBoltDlg)
	DDX_Check(pDX, IDC_CHK_MIR_LAY_LS, m_bMirLayLs);
	DDX_Text(pDX, IDC_E_CROSS_ANGLE_G, m_nCrossAngleG);
	DDX_Text(pDX, IDC_E_G, m_nG);
	DDX_Text(pDX, IDC_E_G1, m_nG1);
	DDX_Text(pDX, IDC_E_G2, m_nG2);
	DDX_Text(pDX, IDC_E_LS_END_SPACE, m_nLsEndSpace);
	DDX_Text(pDX, IDC_E_LS_NUM, m_nLsNum);
	DDX_Text(pDX, IDC_E_LS_SPACE, m_nLsSpace);
	DDX_Radio(pDX, IDC_LS_LAYOUT_STYLE, m_iLsLayoutStyle);
	DDX_Radio(pDX, IDC_RDO_BOLT_NORM, m_iBoltNorm);
	DDX_Radio(pDX, IDC_RDO_RAY_DIRECTION, m_iRayDirection);
	DDX_Text(pDX, IDC_S_BASE_NODE, m_hBaseNode);
	DDX_Text(pDX, IDC_S_RAY_ANGLE_SIZE, m_sRayAngleSize);
	DDX_CBIndex(pDX, IDC_CMB_DATUM_POINT_STYLE, m_iDatumPointStyle);
	DDX_Radio(pDX, IDC_RDO_OFFSET_WING, m_iOffsetWing);
	DDX_Text(pDX, IDC_E_CROSS_ANGLE, m_hCrossAngle);
	DDX_Text(pDX, IDC_E_WING_OFFSET_DIST, m_fWingOffsetDist);
	DDX_Text(pDX, IDC_E_LEN_OFFSET_DIST, m_fLenOffsetDist);
	DDX_Check(pDX, IDC_CHK_TWO_EDGE_PROFILE, m_bTwoEdgeProfile);
	DDX_Check(pDX, IDC_CHK_STD_EDGE_SPACE, m_bUseSdtEdgeSpace);
	DDX_Text(pDX, IDC_E_BER_SPACE, m_fBerSpace);
	DDX_Text(pDX, IDC_E_WING_SPACE, m_fWingSpace);
	DDX_CBString(pDX, IDC_CMB_BOLT_SIZE, m_sBoltSize);
	DDX_Text(pDX, IDC_E_ODDMENT, m_fOddment);
	DDX_Text(pDX, IDC_S_EXTRA_ODDMENT, m_sExtraOdd);
	DDX_CBIndex(pDX, IDC_CMB_ODD_CAL_STYLE, m_iOddCalStyle);
	DDX_Text(pDX, IDC_E_ANGLE_END_NORM_OFFSET, m_fAngleEndNormOffset);
	DDX_Text(pDX, IDC_E_PLATE_EDGE_SPACE, m_fPlateEdgeSpace);
	DDX_CBIndex(pDX, IDC_CMB_LS_ROWS, m_iBoltRows);
	DDX_Text(pDX, IDC_E_FIRST_LS_JG_SPACE, m_nFirstLsJgSpace);
	DDX_CBIndex(pDX, IDC_CMB_TYPE, m_iType);
	DDX_Control(pDX, IDC_LIST_ODD_REF_RODS, m_listOddRefRods);
	DDX_CBIndex(pDX, IDC_CMB_BER_SPACE_MODE, m_iBerSpaceMode);
	DDX_CBIndex(pDX, IDC_CMB_WING_SPACE_MODE, m_iWingSpaceMode);
	DDX_Control(pDX, IDC_LIST_LS_SET, m_xListCtrlLsSet);
	DDX_CBIndex(pDX, IDC_CMB_HOLE_TYPE, m_iHoleFuncType);
	DDX_Check(pDX, IDC_CHK_PUNCH, m_bPunchHole);
	DDX_Check(pDX, IDC_CHK_DRILL, m_bDrillHole);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLayAngleBoltDlg, CDialog)
	//{{AFX_MSG_MAP(CLayAngleBoltDlg)
	ON_BN_CLICKED(IDC_BN_LAYOUT_BOLTS, OnBnLayoutBolts)
	ON_CBN_SELCHANGE(IDC_CMB_DATUM_POINT_STYLE, OnSelchangeCmbDatumPointStyle)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_LS_SET, OnKeydownListLsSet)
	ON_EN_CHANGE(IDC_E_CROSS_ANGLE, OnChangeECrossAngle)
	ON_CBN_SELCHANGE(IDC_CMB_BOLT_SIZE, OnChangeCmbBoltSize)
	ON_BN_CLICKED(IDC_BN_MODIFY_BOLT_ITEM, OnBnModifyBoltItem)
	ON_BN_CLICKED(IDC_LS_LAYOUT_STYLE, OnLsLayoutStyle)
	ON_CBN_SELCHANGE(IDC_CMB_LS_ROWS, OnSelchangeCmbLsRows)
	ON_BN_CLICKED(IDC_RDO_OFFSET_WING, OnRdoOffsetWing)
	ON_EN_CHANGE(IDC_E_ODDMENT, OnChangeOddment)
	ON_WM_MOVE()
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_CMB_ODD_CAL_STYLE, &OnSelchangeCmbOddCalStyle)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LS_SET, OnItemchangedListLsSet)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LS_SET, OnClickListLsSet)
	ON_BN_CLICKED(IDC_BN_SEL_CROSS_ANGLE, OnBnSelCrossAngle)
	ON_BN_CLICKED(IDC_RDO_BOLT_NORM, OnRdoBoltNorm)
	ON_BN_CLICKED(IDC_LS_LAYOUT_STYLE2, OnLsLayoutStyle)
	ON_BN_CLICKED(IDC_RDO_OFFSET_WING2, OnRdoOffsetWing)
	ON_BN_CLICKED(IDC_RDO_BOLT_NORM2, OnRdoBoltNorm)
	ON_CBN_SELCHANGE(IDC_CMB_TYPE, OnSelchangeCmbType)
	ON_COMMAND(ID_NEW_ITEM, OnAddItem)
	ON_COMMAND(ID_EDIT_ITEM_PROP_ITEM, OnModifyItem)
	ON_COMMAND(ID_DELETE_ITEM, OnDeleteItem)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ODD_REF_RODS, &CLayAngleBoltDlg::OnItemchangedListOddRefRods)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ODD_REF_RODS, &CLayAngleBoltDlg::OnDblclkListOddRefRods)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_ODD_REF_RODS, &CLayAngleBoltDlg::OnRclickListOddRefRods)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_ODD_REF_RODS, &CLayAngleBoltDlg::OnKeydownListOddRefRods)
	ON_EN_CHANGE(IDC_E_LS_NUM, &CLayAngleBoltDlg::OnChangeELsNum)
	ON_CBN_SELCHANGE(IDC_CMB_BER_SPACE_MODE, &CLayAngleBoltDlg::OnCbnSelchangeBerSpaceMode)
	ON_CBN_SELCHANGE(IDC_CMB_WING_SPACE_MODE, &CLayAngleBoltDlg::OnCbnSelchangeWingSpaceMode)
	ON_EN_CHANGE(IDC_E_WING_SPACE,&CLayAngleBoltDlg::OnChangeWingSpace)
	ON_EN_CHANGE(IDC_E_BER_SPACE, &CLayAngleBoltDlg::OnChangeBerSpace)
	ON_BN_CLICKED(IDC_BN_SEL_BASE_NODE, &CLayAngleBoltDlg::OnBnSelBaseNode)
	ON_BN_CLICKED(IDC_CHK_STD_EDGE_SPACE, &CLayAngleBoltDlg::OnBnClickedChkStdEdgeSpace)
	ON_BN_CLICKED(IDC_CHK_PUNCH, OnClickedChkPunchHole)
	ON_BN_CLICKED(IDC_CHK_DRILL, OnClickedChkDrillHole)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayAngleBoltDlg message handlers

int CLayAngleBoltDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen=false;
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CLayAngleBoltDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}

BOOL CLayAngleBoltDlg::OnInitDialog() 
{
	if(m_pLinePart==NULL)	//	m_pNode==NULL)
		return FALSE;
	CDialog::OnInitDialog();	//��˨�б��޸�ΪSuperGridCtrl����InitListCtrl��������OnInitDialog֮��,�ʽ�OnInitDialog()��ǰ wht 16-10-24
	hashTempPrevViewBolts.SetMinmalId(Ta.GetMinAvailableHandle());
	if(!m_bInernalStart)
	{
		if (m_iOffsetWing == -1)
		{	//���ݵ�ǰ������ͼ���Զ�������˨���ڸ˼��Ĺ���֫
			if (m_pLinePart->IsAngle())
			{
				CLDSLineAngle *pAngle = (CLDSLineAngle*)m_pLinePart;
				double ddx = pAngle->get_norm_x_wing()*viewNorm;
				double ddy = pAngle->get_norm_y_wing()*viewNorm;
				if (fabs(ddx) >= fabs(ddy))
					m_iOffsetWing = 0;	//��ǰ���Ӻ�ƫ��֫ΪX֫
				else
					m_iOffsetWing = 1;	//��ǰ���Ӻ�ƫ��֫ΪY֫
			}
			else
				m_iOffsetWing = 0;
		}
		//�������ڹ���֫��������˨����
		f3dPoint work_norm;
		if (m_pLinePart->IsAngle())
		{
			CLDSLineAngle *pAngle = (CLDSLineAngle*)m_pLinePart;
			work_norm = (m_iOffsetWing == 0) ? pAngle->vxWingNorm : pAngle->vyWingNorm;
		}
		else if (m_pLinePart->GetClassTypeId() == CLS_LINEFLAT)
			work_norm = ((CLDSLineFlat*)m_pLinePart)->WorkPlaneNorm();
		else if(m_pLinePart->GetClassTypeId()==CLS_LINESLOT)
			work_norm= ((CLDSLineSlot*)m_pLinePart)->WorkPlaneNorm();
		m_iBoltNorm = (work_norm*viewNorm > 0) ? 0 : 1;

		if(strlen(m_tBoltNormOffset.key_str)==0)
		{	//û�и���˨����ƫ��Ĭ��ֵ
			if ((m_pLinePart->IsAngle()&&m_iBoltNorm==0)	//����
				||(m_pLinePart->GetClassTypeId()==CLS_LINEFLAT&&m_iBoltNorm==1)
				||(m_pLinePart->GetClassTypeId()==CLS_LINESLOT&&m_iBoltNorm==0))
				m_tBoltNormOffset.AddThick(-m_pLinePart->handle,TRUE);
			else if(m_pLinePart->GetClassTypeId()==CLS_LINESLOT && m_iOffsetWing>0)
				m_tBoltNormOffset.AddThick(-m_pLinePart->handle, TRUE);
		}
		if(m_sRayAngleSize.GetLength()==0)	//û�и�����ַ���Ĭ��ֵʱ�����¼�����ַ���(ͬʱ��ʾ��š���񼰲��ʱ���У��) wjh-2015.2.9
		{
			CXhChar50 specstr;
			m_pLinePart->GetSpecification(specstr,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
			m_sRayAngleSize.Format("Label��%s %s%C",(char*)m_pLinePart->GetPartNo(),(char*)specstr,m_pLinePart->cMaterial);
#else 
			m_sRayAngleSize.Format("��ţ�%s %s%C",(char*)m_pLinePart->GetPartNo(),(char*)specstr,m_pLinePart->cMaterial);
#endif
		}
		m_hBaseNode.Format("0X%X",m_pNode?m_pNode->handle:0);
		JGZJ jgzj;
		if(m_pLinePart->IsAngle())
		{
			CLDSLineAngle *pAngle=(CLDSLineAngle*)m_pLinePart;
			getjgzj(jgzj,pAngle->GetWidth());
			//�ڵ㸸�˼��ǵ�ǰ�����Ǹ֣��������ýڵ������׼���趨	wxc--2016.8.3
			if(m_iOffsetWing==0)
			{	//����֫ΪX֫
				if(m_pNode && m_pNode->hFatherPart==pAngle->handle &&m_pNode->xFatherAngleZhunJu.offset_X_dist_style==4)
					jgzj.g=(long)m_pNode->xFatherAngleZhunJu.offset_X_dist;
				else if(pAngle->m_bEnableTeG)	//���������ľ�
					jgzj = pAngle->xWingXZhunJu;
			}
			else
			{	//����֫ΪY֫
				if(m_pNode && m_pNode->hFatherPart==pAngle->handle &&m_pNode->xFatherAngleZhunJu.offset_Y_dist_style==4)
					jgzj.g=(long)m_pNode->xFatherAngleZhunJu.offset_Y_dist;
				else if(pAngle->m_bEnableTeG)	//���������ľ�
					jgzj = pAngle->xWingYZhunJu;
			}
			m_nG = jgzj.g;
			m_nG1= jgzj.g1;
			m_nG2= jgzj.g2;
		}
		else if (m_pLinePart->GetClassTypeId() == CLS_LINESLOT && m_iOffsetWing > 0)
		{
			CLDSLineSlot* pSlot = (CLDSLineSlot*)m_pLinePart;
			m_nG = ftoi(pSlot->GetWidth()*0.5);
		}
		else
		{
			jgzj.g = 0;
			m_nG = m_nG1 = m_nG2 = 0;
		}
		if(m_pNode&&m_pLinePart->pEnd==m_pNode || m_ciSectType==2)
		{
			m_iRayDirection=1;	//�գ�>ʼ
			m_nLsNum=m_pLinePart->wnEndConnBoltN;//BoltsNumOn1Wing(FALSE);//connectEnd.N;
			m_sBoltSize.Format("%d",m_pLinePart->connectEnd.d);
			m_iBoltRows=m_pLinePart->connectEnd.rows-1;
		}
		else if(m_pNode&&m_pLinePart->pStart==m_pNode || m_ciSectType == 1)
		{
			m_iRayDirection=0;	//ʼ��>��
			m_nLsNum=m_pLinePart->wnStartConnBoltN;//BoltsNumOn1Wing(TRUE);//connectStart.N;
			m_sBoltSize.Format("%d",m_pLinePart->connectStart.d);
			m_iBoltRows=m_pLinePart->connectStart.rows-1;
		}
		else 
		{	//��׼�Ǹ���˨��=MAX(���߽Ǹ���˨��)*1.5;  //wht 10-11-23
			//��ʹ�ýڵ������ʱ�Ѿ����㲢������connectMid��
			m_iRayDirection=0;	//ʼ��>��
			m_nLsNum=m_pLinePart->connectMid.wnConnBoltN;
			m_sBoltSize.Format("%d",m_pLinePart->connectMid.d);
		}
		if(m_nLsNum>=5&&m_nG1>0)	//��˨������5ʱ��Ҫ����˫����˨
			m_iBoltRows=1;	
		if(m_pLinePart->pStart!=m_pNode&&m_pLinePart->pEnd!=m_pNode)
		{
			GetDlgItem(IDC_E_ODDMENT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_ODD_CAL_STYLE)->EnableWindow(FALSE);
		}
		//������˨�ߴ�����˾�
		int bolt_d=atoi(m_sBoltSize);
		if(bolt_d==16)
			m_nFirstLsJgSpace = 20;
		else if(bolt_d==20)
			m_nFirstLsJgSpace = 25;
		else
			m_nFirstLsJgSpace = 30;
		LSSPACE_STRU LsSpace;
		if(GetLsSpace(LsSpace,atoi(m_sBoltSize)))
		{
			if(m_iBoltRows==0)
				m_nLsSpace = LsSpace.SingleRowSpace;
			else
				m_nLsSpace = LsSpace.doubleRowSpace;
			//�������û�и��˾ำֵ������Ҫ��ֵ
			if(m_nLsEndSpace == 0)
				m_fPlateEdgeSpace=m_nLsEndSpace = LsSpace.EndSpace;
			else
				m_fPlateEdgeSpace = LsSpace.EndSpace;
			if(m_iDatumPointStyle==0)
				m_fLenOffsetDist = LsSpace.EndSpace;
			else
				m_fLenOffsetDist = 0;
			if(m_bEndLjJg)
			{
				m_fOddment = m_nLsEndSpace;
				//m_bOddAutoCorrect=TRUE;
			}
		}
		if(m_pLinePart->pStart==m_pNode||m_pLinePart->pEnd==m_pNode)
		{
			if(m_bEndLjJg&&m_hCrossAngle.GetLength()>0)
			{
				m_iOddCalStyle=1;	//�����ӽǸֵ�����ͷ����ԭ��Ĭ��Ϊ������˨�ж� wjh-2019.7.29
				m_iDatumPointStyle=3;	//��ǰ�Ǹ�Ϊ�����ӽǸ�Ĭ�ϻ��㶨λ��ʽΪ���߽���wht 10-01-14
			}
			else if(m_bEndLjDatumJg&&m_pEndLjRayJg)
			{	//��ǰ�Ǹ�Ϊ��׼�Ǹ��Ҵ��ڶ�Ӧ�����߽Ǹ֣�Ĭ�϶�λ��ʽΪ���߽���
				m_iDatumPointStyle=3;	
				m_hCrossAngle.Format("0X%X",m_pEndLjRayJg->handle);
			}
			else
				m_iDatumPointStyle=0;	//�����ڵ�Ϊ�Ǹֶ˽ڵ�ʱĬ���ԽǸ����Ϊ��λ����
			m_fWingOffsetDist = jgzj.g;
			m_fLenOffsetDist = LsSpace.EndSpace;
		}
		else if(m_iDatumPointStyle==0)
		{	//��˨��λ��ʽ�������ⲿ�Ѿ�ָ�� Ĭ��Ϊ��㶨λʱ��Ҫ������λ��ʽ wht 10-01-14
			if(fabs(m_pLinePart->End().z-m_pLinePart->Start().z)<1)	//��ʱ�����ж� wjh-2011.8.16
				m_bRayEndFaceOffset=FALSE;
			if(m_bRayEndFaceOffset)
			{	//�����ڵ�Ϊ�м�ڵ�ʱĬ���Խڵ���Ǹ�����ƽ��Ϊ���㶨λ��ʽ
				m_iDatumPointStyle=2;
				//ƽ���淨��ӦĬ������˨��λ��׼�Ǹ�ƽ���淨��һ��
				if (m_pLinePart->IsAngle())
				{
					CLDSLineAngle *pAngle=(CLDSLineAngle*)m_pLinePart;
					if(pAngle->desStartPos.IsFaceOffset())
						face_norm_offset=pAngle->desStartPos.face_offset_norm;
					else if(pAngle->desEndPos.IsFaceOffset())
						face_norm_offset=pAngle->desEndPos.face_offset_norm;
				}
				else if(m_pLinePart->GetClassTypeId()==CLS_LINEFLAT)
				{
					CLDSLineFlat *pFlat=(CLDSLineFlat*)m_pLinePart;
					if(pFlat->desStartPos.datum_pos_style>2)
						face_norm_offset=pFlat->desStartPos.face_offset_norm;
					else if(pFlat->desEndPos.datum_pos_style>2)
						face_norm_offset=pFlat->desEndPos.face_offset_norm;
				}
				else 
				{
					CLDSLineSlot *pSlot=(CLDSLineSlot*)m_pLinePart;
					if(pSlot->desStartPos.datum_pos_style>2)
						face_norm_offset=pSlot->desStartPos.face_offset_norm;
					else if(pSlot->desEndPos.datum_pos_style>2)
						face_norm_offset=pSlot->desEndPos.face_offset_norm;
				}
			}
			else if(m_pNode)
				m_iDatumPointStyle=1;	//�����ڵ�Ϊ�м�ڵ�ʱĬ���Խڵ���Ǹ�����ͶӰΪ���㶨λ��ʽ
			m_fWingOffsetDist = 0;
			m_fLenOffsetDist = 0;
			if(m_bEndLjDatumJg&&m_pEndLjRayJg)
			{	//��ǰ�Ǹ�Ϊ��׼�Ǹ��Ҵ��ڶ�Ӧ�����߽Ǹ֣�Ĭ�϶�λ��ʽΪ���߽��� wht 16-10-09
				GEPOINT lenStdVec=m_pLinePart->End()-m_pLinePart->Start();
				normalize(lenStdVec);
				GEPOINT lenStdVec2=m_pEndLjRayJg->End()-m_pEndLjRayJg->Start();
				normalize(lenStdVec2);
				if(fabs(lenStdVec*lenStdVec2)<EPS_COS2)
				{	//����������ƽ�нǸֽ������߽������ wjh-2016.11.02
					m_iDatumPointStyle=3;	//���߽���
					m_hCrossAngle.Format("0X%X",m_pEndLjRayJg->handle);
				}
			}
		}
		if(PREV_BERSPACE_MODE<=2)//3.��������-��ʱһ��ֻ�����ڻ�׼�Ǹ֣��ʲ��̳� wjh-2015.3.22
			m_iBerSpaceMode=PREV_BERSPACE_MODE;
		if(PREV_WINGSPACE_MODE<=2)
			m_iWingSpaceMode=PREV_WINGSPACE_MODE;
		if(m_pNode&&m_pNode->hFatherPart==m_pLinePart->handle)// �����ӵ�˼����ڽǸ־��ʱ  ��ʾ��׼�߾�, ������ʾ��������
			m_iBerSpaceMode=1;
		else
			m_iBerSpaceMode=2;
		m_iWingSpaceMode=2;   // Ĭ������������
		if(m_bIncPlateProfilePara)
		{
			if(m_iBoltRows==0)
			{
				if(m_bTwoEdgeProfile)
					m_fBerSpace=m_nG-m_pLinePart->GetThick();
				else
					m_fBerSpace=m_nG;
				m_fWingSpace=m_pLinePart->GetWidth()-m_nG;
			}
			else
			{
				if(m_bTwoEdgeProfile)
					m_fBerSpace=m_nG1-m_pLinePart->GetThick();
				else
					m_fBerSpace=m_nG1;
				m_fWingSpace=m_pLinePart->GetWidth()-m_nG2;
			}
			//ȥ��֫����������߲��� ���ֵ���ܳ���60������ wht 10-09-26
			//m_fWingSpace=min(60,m_fWingSpace);
			//m_fBerSpace=min(60,m_fBerSpace);
			//����߾��Ѹ�ֵ��������߾�Ӧ���ڱ�׼��˨�˾� LsSpace.EndSpace wht 12-05-31
			//m_fPlateEdgeSpace=m_nLsEndSpace;
			if(m_iBerSpaceMode==1)
				m_fBerSpace=LsSpace.EndSpace;	
			if(m_iWingSpaceMode==1)
				m_fWingSpace=LsSpace.EndSpace;
		}
		if(m_fHuoquWingOffsetDist>0&&m_fHuoquWingOffsetDist<m_pLinePart->GetWidth()/2)
		{
			m_iBerSpaceMode=1;
			m_fBerSpace=LsSpace.EndSpace;	
		}
		if(m_fHuoquWingOffsetDist>m_pLinePart->GetWidth()/2&&m_fHuoquWingOffsetDist<m_pLinePart->GetWidth())
		{
			m_iWingSpaceMode=1;
			m_fBerSpace=LsSpace.EndSpace;	
		}
		m_xListCtrlLsSet.EmptyColumnHeader();
#ifdef AFX_TARG_ENU_ENGLISH
		m_xListCtrlLsSet.AddColumnHeader("Specs",50);
		m_xListCtrlLsSet.AddColumnHeader("Datum Point Locate Style",246);
		m_xListCtrlLsSet.AddColumnHeader("Vertical Offset/Datum Center D",225);
		m_xListCtrlLsSet.AddColumnHeader("Leg Offset/Ray Center D",177);
		m_xListCtrlLsSet.AddColumnHeader("Normal Offset",110);
#else 
		m_xListCtrlLsSet.AddColumnHeader("���",50);
		m_xListCtrlLsSet.AddColumnHeader("���㶨λ��ʽ",130);
		m_xListCtrlLsSet.AddColumnHeader("����ƫ��/��׼�ľ�",130);
		m_xListCtrlLsSet.AddColumnHeader("֫��ƫ��/�����ľ�",130);
		m_xListCtrlLsSet.AddColumnHeader("����ƫ��",75);
#endif
		m_xListCtrlLsSet.SetLButtonDblclkFunc(FireLButtonDblclk);
		m_xListCtrlLsSet.SetModifyValueFunc(FireValueModify);
		m_xListCtrlLsSet.SetDeleteItemFunc(FireDeleteItem);
		//
		CComboBox* pCMB = (CComboBox*)GetDlgItem(IDC_CMB_HOLE_TYPE);
		pCMB->AddString("0.��˨");
		pCMB->AddString("1.�Ŷ���");
		pCMB->AddString("2.���߿�");
		pCMB->AddString("3.�ӵؿ�");
		pCMB->AddString("4.װ���");
		pCMB->AddString("5.���ƿ�");
		pCMB->SetCurSel(m_iHoleFuncType);
	}
	//else
	//{}
	m_xListCtrlLsSet.InitListCtrl(NULL,FALSE);
	//����Ѿ�ָ����˨��λ��ʽΪ�Ǹ����߽���
	//����ָ������Ǹ֣����ݽ���Ǹ־�����㽻��Ǹ�׼�� wht 10-01-14
	f3dPoint wing_norm;
	if(m_pLinePart->IsAngle())
	{
		CLDSLineAngle *pAngle=(CLDSLineAngle*)m_pLinePart;
		if(m_iOffsetWing==0)
			wing_norm=pAngle->get_norm_x_wing();
		else
			wing_norm=pAngle->get_norm_y_wing();
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Set Bolts Of Angle");
#else 
		SetWindowText("���ýǸ���˨");
#endif
	}
	else if (m_pLinePart->GetClassTypeId()==CLS_LINEFLAT)
	{
		CLDSLineFlat *pFlat=(CLDSLineFlat*)m_pLinePart;
		wing_norm=pFlat->WorkPlaneNorm();
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Set Bolts Of Flat Iron");
#else 
		SetWindowText("���ñ�����˨");
#endif
	}
	else
	{
		CLDSLineSlot *pSlot=(CLDSLineSlot*)m_pLinePart;
		if (m_iOffsetWing==0)
			wing_norm=pSlot->WorkPlaneNorm();
		//else if(m_iOffsetWing==1)
		//else
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Set Bolts Of U-steel");
#else 
		SetWindowText("���ò۸���˨");
#endif
	}
	m_pLsSetList=&m_xListCtrlLsSet;
    CString str_arr[5];
	if(m_pLegacyLsList!=NULL)
	{
		for(CLDSBolt* pBolt=m_pLegacyLsList->GetFirst();pBolt;pBolt=m_pLegacyLsList->GetNext())
		{
			str_arr[0].Format("M%d",pBolt->get_d());
			if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[1] = "Center Intersection Point";
#else 
				str_arr[1] = "���߽���";
#endif
			else if(pBolt->des_base_pos.datumPoint.datum_pos_style==1)
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[1] = "Angle Ridge Vertice";
#else 
				str_arr[1] = "�Ǹ����";
#endif
			else if(pBolt->des_base_pos.datumPoint.datum_pos_style==2)
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[1] = "Node projects to the angle's center line";
#else 
				str_arr[1] = "�ڵ���Ǹ�����ͶӰ";
#endif
			str_arr[2].Format("%f",pBolt->des_base_pos.len_offset_dist);
			SimplifiedNumString(str_arr[2]);
			str_arr[3].Format("%f",pBolt->des_base_pos.wing_offset_dist);
			SimplifiedNumString(str_arr[3]);
			str_arr[4].Format("%d",pBolt->des_base_pos.norm_offset.Thick(m_pLinePart->BelongModel()));
			AppendRecord(str_arr,NULL);
		}
	}
	for(CDesignLsPara *pLsPara=m_pLsList->GetFirst();pLsPara;pLsPara=m_pLsList->GetNext())
	{
		str_arr[0].Format("M%d",pLsPara->d);
		if(pLsPara->des_base_pos.datumPoint.datum_pos_style==3)
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[1] = "Center Line Intersection Point";
#else 
			str_arr[1] = "���߽���";
#endif
		else if(pLsPara->des_base_pos.datumPoint.datum_pos_style==1)
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[1] = "Angle Ridge Vertice";
#else 
			str_arr[1] = "�Ǹ����";
#endif
		else if(pLsPara->des_base_pos.datumPoint.datum_pos_style==2)
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[1] = "Node projects to the angle's center line";
#else 
			str_arr[1] = "�ڵ���Ǹ�����ͶӰ";
#endif
		str_arr[2].Format("%f",pLsPara->des_base_pos.len_offset_dist);
		SimplifiedNumString(str_arr[2]);
		str_arr[3].Format("%f",pLsPara->des_base_pos.wing_offset_dist);
		SimplifiedNumString(str_arr[3]);
		str_arr[4].Format("%d",pLsPara->des_base_pos.norm_offset.Thick(m_pLinePart->BelongModel()));
		AppendRecord(str_arr,(void*) pLsPara);
	}
	if(!m_bInernalStart)
	{
		if(m_pNode&&m_pLinePart->pStart==m_pNode&&m_pLinePart->desStartOdd.m_hRefPart1==0&&m_pLinePart->desStartOdd.m_hRefPart2==0)
		{
			if(m_pLinePart->handle!=m_pNode->hFatherPart)	//��ֹ��׼�����ʱ���ԭ�н�ͷ���ȷ��������ͷ wjh-2015.2.5
				m_pLinePart->desStartOdd.m_hRefPart1=m_pNode->hFatherPart;
			m_pLinePart->desStartOdd.m_iOddCalStyle=m_iOddCalStyle;//���޴��ж����ӽǸֿ��ܻ�������ײԭ����д������ wjh-2015.3.10
			m_pLinePart->CalStartOddment(1);
			m_fOddment=m_pLinePart->startOdd();
		}
		else if(m_pNode&&m_pLinePart->pEnd==m_pNode&&m_pLinePart->desEndOdd.m_hRefPart1==0&&m_pLinePart->desEndOdd.m_hRefPart2==0)
		{
			if(m_pLinePart->handle!=m_pNode->hFatherPart)
				m_pLinePart->desEndOdd.m_hRefPart1=m_pNode->hFatherPart;
			m_pLinePart->desEndOdd.m_iOddCalStyle=m_iOddCalStyle;
			m_pLinePart->CalEndOddment(1);
			m_fOddment=m_pLinePart->endOdd();
		}
	}

	if(!m_bInernalStart&&m_pNode&&m_pNode!=m_pLinePart->pStart&&m_pNode!=m_pLinePart->pEnd)
	{
		m_bMirLayLs=TRUE;
		if(m_iDatumPointStyle==0)
			m_iDatumPointStyle=1;	//��Ȼѡ���˽ڵ��Ĭ�ϸ�Ϊ�ڵ�������ͶӰ���� wjh-2016.11.8
	}
	else if(m_bInernalStart&&m_hPickObj>0&&m_iPickObjType==0)
		m_hCrossAngle.Format("0X%X",m_hPickObj);
	else if(m_bInernalStart&&m_hPickObj>0&&m_iPickObjType==3) //�ڵ�
	{
		m_hBaseNode.Format("0X%X",m_hPickObj);
		if(m_iDatumPointStyle==0&&((m_pNode==NULL&&m_hPickObj>0)||m_pNode->handle!=m_hPickObj))
		{
			m_bMirLayLs=TRUE;
			m_iDatumPointStyle=1;	//��Ȼѡ���˽ڵ��Ĭ�ϸ�Ϊ�ڵ�������ͶӰ���� wjh-2016.11.8
		}
		if(m_pLinePart&&m_pLinePart->IsAngle())
		{
			CLDSNode* pDatumNode=Ta.Node.FromHandle(m_hPickObj);
			if(m_iOffsetWing==0)
				m_nG=ftoi(((CLDSLineAngle*)m_pLinePart)->GetNodeWingXG(pDatumNode));
			else
				m_nG=ftoi(((CLDSLineAngle*)m_pLinePart)->GetNodeWingYG(pDatumNode));
		}
		m_pNode=Ta.Node.FromHandle(m_hPickObj);
	}
	else if(m_bInernalStart&&m_hPickObj>0&&m_iPickObjType>=1)
	{	//��ӻ��޸���ײ���ո˼�
		CXhChar16 hRefRod("0X%X",m_hPickObj);
		if(m_pLinePart->pStart==m_pNode)
		{
			if(m_pLinePart->desStartOdd.m_hRefPart1==m_hPickObj||m_pLinePart->desStartOdd.m_hRefPart2==m_hPickObj)
			{
				if(m_iPickObjType==1)
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("The rod has been in existence");
#else 
					AfxMessageBox("�ø˼��Ѵ���");
#endif
				hRefRod.Copy("");
			}
			else if(m_iPickObjType==1)
			{
				if(m_pLinePart->desStartOdd.m_hRefPart1==0)
					m_pLinePart->desStartOdd.m_hRefPart1=m_hPickObj;
				else if(m_pLinePart->desStartOdd.m_hRefPart2==0)
					m_pLinePart->desStartOdd.m_hRefPart2=m_hPickObj;
			}
			else if(m_iPickObjType==2&&m_pLinePart->desStartOdd.m_hRefPart1==m_hCurEditOddPart)
				m_pLinePart->desStartOdd.m_hRefPart1=m_hPickObj;
			else if(m_iPickObjType==2&&m_pLinePart->desStartOdd.m_hRefPart2==m_hCurEditOddPart)
				m_pLinePart->desStartOdd.m_hRefPart2=m_hPickObj;
			m_pLinePart->CalStartOddment(1);
			m_fOddment=m_pLinePart->startOdd();
		}
		else
		{
			if(m_pLinePart->desEndOdd.m_hRefPart1==m_hPickObj||m_pLinePart->desEndOdd.m_hRefPart2==m_hPickObj)
			{
				if(m_iPickObjType==1)
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("The rod has been in existence");
#else 
					AfxMessageBox("�ø˼��Ѵ���");
#endif
				hRefRod.Copy("");
			}
			else if(m_iPickObjType==1)
			{
				if(m_pLinePart->desEndOdd.m_hRefPart1==0)
					m_pLinePart->desEndOdd.m_hRefPart1=m_hPickObj;
				else if(m_pLinePart->desEndOdd.m_hRefPart2==0)
					m_pLinePart->desEndOdd.m_hRefPart2=m_hPickObj;
			}
			else if(m_iPickObjType==2&&m_pLinePart->desEndOdd.m_hRefPart1==m_hCurEditOddPart)
				m_pLinePart->desEndOdd.m_hRefPart1=m_hPickObj;
			else if(m_iPickObjType==2&&m_pLinePart->desEndOdd.m_hRefPart2==m_hCurEditOddPart)
				m_pLinePart->desEndOdd.m_hRefPart2=m_hPickObj;
			if(m_iPickObjType==1&&hRefRod.GetLength()>0)		//�����ײ���ո˼�
				m_listOddRefRods.InsertItem(1,hRefRod);
			else if(m_iPickObjType==2&&hRefRod.GetLength()>0&&m_iCurSelOddRefRod>=0)	//�޸���ײ���ո˼�
				m_listOddRefRods.SetItemText(m_iCurSelOddRefRod,0,hRefRod);
			m_pLinePart->CalEndOddment(1);
			m_fOddment=m_pLinePart->endOdd();
		}
		m_pLinePart->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(m_pLinePart->GetSolidPartObject());
		g_pSolidDraw->Draw();
	}
	long hCrossAngle;
	sscanf(m_hCrossAngle,"%X",&hCrossAngle);
	CLDSLineAngle *pCrossAngle=(CLDSLineAngle*)console.FromPartHandle(hCrossAngle,CLS_LINEANGLE);
	if(pCrossAngle)
	{
		JGZJ jgzj;
		if(pCrossAngle->m_bEnableTeG)
		{
			if(fabs(wing_norm*pCrossAngle->get_norm_x_wing())>fabs(wing_norm*pCrossAngle->get_norm_y_wing()))
				jgzj=pCrossAngle->xWingXZhunJu;
			else
				jgzj=pCrossAngle->xWingYZhunJu;
		}
		else
			getjgzj(jgzj,pCrossAngle->GetWidth());
		m_nCrossAngleG=jgzj.g;
	}
	UpdateData(FALSE);
	//GetDlgItem(IDC_RDO_OFFSET_WING3)->ShowWindow(FALSE);
	GetDlgItem(IDC_CMB_LS_ROWS)->EnableWindow(m_bEnable2RowsLs);
	//������Ӱ�ʱ�Ǹֶ԰����ε�Ӱ��ϵ��
	GetDlgItem(IDC_CHK_TWO_EDGE_PROFILE)->ShowWindow(m_bIncPlateProfilePara);
	GetDlgItem(IDC_CHK_STD_EDGE_SPACE)->ShowWindow(m_bIncPlateProfilePara);
	GetDlgItem(IDC_S_BER_SPACE_MODE_TITLE)->ShowWindow(m_bIncPlateProfilePara);
	GetDlgItem(IDC_CMB_BER_SPACE_MODE)->ShowWindow(m_bIncPlateProfilePara);
	GetDlgItem(IDC_S_BER_SPACE_TITLE)->ShowWindow(m_bIncPlateProfilePara);
	GetDlgItem(IDC_E_BER_SPACE)->ShowWindow(m_bIncPlateProfilePara);
	GetDlgItem(IDC_S_PLATE_EDGE_SPACE_TILTLE)->ShowWindow(m_bIncPlateProfilePara);
	GetDlgItem(IDC_E_PLATE_EDGE_SPACE)->ShowWindow(m_bIncPlateProfilePara);
	GetDlgItem(IDC_S_WING_SPACE_MODE_TITLE)->ShowWindow(m_bIncPlateProfilePara);
	GetDlgItem(IDC_CMB_WING_SPACE_MODE)->ShowWindow(m_bIncPlateProfilePara);
	GetDlgItem(IDC_S_WING_SPACE_TILTLE)->ShowWindow(m_bIncPlateProfilePara);
	GetDlgItem(IDC_E_WING_SPACE)->ShowWindow(m_bIncPlateProfilePara);
	OnSelchangeCmbDatumPointStyle();
	//�����Ի�����ø���ƫ������������ʹ�����߽��㲼����˨ʱ�ֶ������ƫ��ֵ���治��
	if(!m_bInernalStart)	
		OnRdoOffsetWing();
	double extra_odd=0;
	m_listOddRefRods.DeleteAllItems();
	m_listOddRefRods.InitListCtrl(NULL);
	if(m_pLinePart->IsAngle())
	{
		CLDSLineAngle *pAngle=(CLDSLineAngle*)m_pLinePart;
		if(pAngle->pStart==m_pNode)
		{
			extra_odd = pAngle->GetStartExtraOdd(m_iOffsetWing);
			if(pAngle->desStartOdd.m_hRefPart1>0)
				m_listOddRefRods.InsertItem(0,CXhChar16("0X%X",pAngle->desStartOdd.m_hRefPart1));
			if(pAngle->desStartOdd.m_hRefPart2>0)
				m_listOddRefRods.InsertItem(1,CXhChar16("0X%X",pAngle->desStartOdd.m_hRefPart2));
		}
		else
		{
			extra_odd = pAngle->GetEndExtraOdd(m_iOffsetWing);
			if(pAngle->desEndOdd.m_hRefPart1>0)
				m_listOddRefRods.InsertItem(0,CXhChar16("0X%X",pAngle->desEndOdd.m_hRefPart1));
			if(pAngle->desEndOdd.m_hRefPart2>0)
				m_listOddRefRods.InsertItem(1,CXhChar16("0X%X",pAngle->desEndOdd.m_hRefPart2));
		}
		if(fabs(extra_odd)>EPS)
			m_sExtraOdd.Format("%.0f",extra_odd);
		m_listOddRefRods.RedrawItems(0,2);
	}
	double x=fabs(face_norm_offset.x);
	double y=fabs(face_norm_offset.y);
	double z=fabs(face_norm_offset.z);
	if(x>EPS&&y<EPS&&z<EPS)
		m_iType=0;
	else if(y>EPS&&x<EPS&&z<EPS)
		m_iType=1;
	else if(z>EPS&&y<EPS&&x<EPS)
		m_iType=2;
	else
		m_iType=3;
	UpdateData(FALSE);
	//��ʾ��˨��׼��Ĺ�����
	WorkPlaneSketch();
	//�ƶ��Ի������Ͻ�
	CRect rect;
	GetWindowRect(rect);
	int width = rect.Width();
	int height=rect.Height();
	rect.left=nScrLocationX;
	rect.top=nScrLocationY;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;
	MoveWindow(rect,TRUE);
	m_bEnableWindowMoveListen=true;
	//
	return TRUE;
}
void CLayAngleBoltDlg::AppendRecord(CString*str_arr,void* pLsPara/*=NULL*/)
{
	CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
	BOOL bEidt=(pLsPara==NULL)?TRUE:FALSE;
	lpInfo->SetSubItemText(0,str_arr[0],bEidt);
	lpInfo->SetControlType(0,GCT_CMB_EDIT);
	lpInfo->SetListItemsStr(0,"M12|M16|M20|M24|M27");
	lpInfo->SetSubItemText(1,str_arr[1],TRUE);
	lpInfo->SetSubItemText(2,str_arr[2],bEidt);
	lpInfo->SetControlType(2,GCT_EDIT);
	lpInfo->SetSubItemText(3,str_arr[3],bEidt);
	lpInfo->SetControlType(3,GCT_EDIT);
	lpInfo->SetSubItemText(4,str_arr[4],TRUE);
	CSuperGridCtrl::CTreeItem* pItem = m_xListCtrlLsSet.InsertRootItem(lpInfo);
	if(pLsPara)
		pItem->m_idProp = (DWORD)pLsPara;
}

int CLayAngleBoltDlg::AppendRecord(CListCtrl *pListCtrl, int iItem, CStringArray &str_arr)
{
// insert items
	if(iItem==-1)
		iItem=pListCtrl->GetItemCount();

	iItem = pListCtrl->InsertItem(iItem,str_arr[0]);
// set item text for additional columns
	int nCount = str_arr.GetSize();
	for(int j = 1; j<nCount; j++)
	{
		str_arr[j].TrimLeft();
		str_arr[j].TrimRight();
		pListCtrl->SetItemText(iItem,j,str_arr[j]);
	}
	pListCtrl->RedrawItems(iItem,iItem);
	return iItem;
}

void CLayAngleBoltDlg::DisplayItemInfo(int iCurSel)
{
	CString ss;
	char tem_str[100];
	if(iCurSel<0)
		return;
	CSuperGridCtrl::CTreeItem *pCurItem=m_xListCtrlLsSet.GetTreeItem(iCurSel);
	CDesignLsPara *pLsPara=(CDesignLsPara*)pCurItem->m_idProp;
	if(pLsPara==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Share the bolt with other parts.It cannot be modified!");
#else 
		AfxMessageBox("�����๹����Ĺ�����˨�������޸�!");
#endif
		return;
	}
	if(pLsPara->des_base_pos.datumPoint.datum_pos_style==2&&
		pLsPara->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset)
	{
		face_norm_offset=pLsPara->des_base_pos.datumPoint.des_para.RODNODE.vPlaneNormal;
	}
	m_pLsSetList->GetItemText(iCurSel,0,tem_str,100);
	m_sBoltSize.Format("%s",tem_str+1);
	m_pLsSetList->GetItemText(iCurSel,1,tem_str,100);
	ss.Format("%s",tem_str);
#ifdef AFX_TARG_ENU_ENGLISH
	if(ss.CompareNoCase("Angle Ridge Vertice")==0)
		m_iDatumPointStyle = 0;
	else if(ss.CompareNoCase("Node projects to the angle's center line")==0)
		m_iDatumPointStyle = 1;	//2
	else if(ss.CompareNoCase("Center Line Intersection Point")==0)
#else 
	if(ss.CompareNoCase("�Ǹ����")==0)
		m_iDatumPointStyle = 0;
	else if(ss.CompareNoCase("�ڵ���Ǹ�����ͶӰ")==0)
		m_iDatumPointStyle = 1;	//2
	else if(ss.CompareNoCase("���߽���")==0)
#endif
	{
		m_iDatumPointStyle = 3;
		m_hCrossAngle.Format("0X%X",pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2);
		m_nCrossAngleG=pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2;
		m_nG=pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1;
	}
	if(pLsPara->des_base_pos.datumPoint.datum_pos_style==2)
	{
		if(pLsPara->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset==FALSE)
			m_iDatumPointStyle = 1;	//�ڵ���Ǹ���ͶӰΪ��λ����
		else
			m_iDatumPointStyle = 2;	//�ڵ��ڽǸ�����ƽ��Ϊ��λ����
	}
	m_iRayDirection=pLsPara->des_base_pos.direction;
	m_iBoltNorm=pLsPara->des_work_norm.direction;
	m_pLsSetList->GetItemText(iCurSel,2,tem_str,100);
	m_fLenOffsetDist = atof(tem_str);
	m_pLsSetList->GetItemText(iCurSel,3,tem_str,100);
	m_fWingOffsetDist = atof(tem_str);
	m_pLsSetList->GetItemText(iCurSel,4,tem_str,100);
	UpdateData(FALSE);
}

static BOOL FireDeleteItem(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem)
{
	CListCtrl* m_pLsSetList=pListCtrl;//Ϊ���ٸĶ����� ���ⶨ����ʱ����
	CLayAngleBoltDlg *pLayAngleBoltDlg=(CLayAngleBoltDlg*)pListCtrl->GetParent();
	{
		CDesignLsPara *pTmLsPara=(CDesignLsPara*)pSelItem->m_idProp;
		if(pTmLsPara==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Share the bolt with other parts.It cannot be deleted!");
#else 
			AfxMessageBox("�����๹����Ĺ�����˨������ɾ��!");
#endif
			return FALSE;
		}
		for(CDesignLsPara *pLsPara=pLayAngleBoltDlg->m_pLsList->GetFirst();pLsPara;pLsPara=pLayAngleBoltDlg->m_pLsList->GetNext())
		{
			if(pLsPara==pTmLsPara)
			{
				pLayAngleBoltDlg->m_pLsList->DeleteCursor();
				//ɾ����Ӧ����˨ʵ��
				if(pLsPara->pMapLs&&pLayAngleBoltDlg->hashTempPrevViewBolts.GetValue(pLsPara->pMapLs->handle))
				{
					pLsPara->pMapLs->feature=0;	//���õ�ǰ��˨Ϊ����״̬
					g_pSolidDraw->DelEnt(pLsPara->pMapLs->handle);
					pLsPara->pMapLs=NULL;
				}
				break;
			}
		}
	}
	return TRUE;	 
}

void CLayAngleBoltDlg::OnItemchangedListLsSet(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;
}

void CLayAngleBoltDlg::OnClickListLsSet(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	UpdateData();
	DisplayItemInfo(pNMListView->iItem);
	//
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(m_pLinePart->handle);
	POSITION pos=m_pLsSetList->GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel=m_pLsSetList->GetNextSelectedItem(pos);
		CSuperGridCtrl::CTreeItem *pCurItem=m_xListCtrlLsSet.GetTreeItem(iCurSel);
		CDesignLsPara *pLsPara=(CDesignLsPara*)pCurItem->m_idProp;
		if(pLsPara&&pLsPara->pMapLs)
			g_pSolidDraw->SetEntSnapStatus(pLsPara->pMapLs->handle);
	}
	*pResult = 0;
}

void CLayAngleBoltDlg::OnKeydownListLsSet(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	POSITION pos = m_pLsSetList->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pLsSetList->GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_F3)
		{
			if(iCurSel>=0)
				DisplayItemInfo(iCurSel);
		}
	}

	*pResult = 0;
}
void CLayAngleBoltDlg::InitDesignLsPara(CDesignLsPara& ls_stru)
{
	ls_stru.d = atoi(m_sBoltSize);
	if(m_xiPreferSegI.iSeg!=0)
		ls_stru.iSeg = m_xiPreferSegI;//SEGI(m_sSegNo);
	else if(m_pLinePart)
		ls_stru.iSeg = m_pLinePart->iSeg;//SEGI(m_sSegNo);
	//��˨�����趨
	if (m_pLinePart->IsAngle())
	{
		ls_stru.des_work_norm.norm_style = 1;
		ls_stru.des_work_norm.hVicePart = m_pLinePart->handle;
		ls_stru.des_base_pos.norm_offset = m_tBoltNormOffset;
		if (m_iBoltNorm == 0)	//����
			ls_stru.des_work_norm.direction = 0;
		else
			ls_stru.des_work_norm.direction = 1;
		ls_stru.des_work_norm.norm_wing = m_iOffsetWing;
	}
	else if (m_pLinePart->GetClassTypeId() == CLS_LINEFLAT)
	{
		ls_stru.des_work_norm.norm_style = 3;
		ls_stru.des_work_norm.hVicePart = m_pLinePart->handle;
		ls_stru.des_base_pos.norm_offset = m_tBoltNormOffset;
		if (m_iBoltNorm == 0)	//����
			ls_stru.des_work_norm.direction = 0;
		else
			ls_stru.des_work_norm.direction = 1;
		ls_stru.des_work_norm.norm_wing = m_iOffsetWing;
	}
	else
	{
		ls_stru.des_work_norm.norm_style = 5;
		ls_stru.des_work_norm.hVicePart = m_pLinePart->handle;
		ls_stru.des_base_pos.norm_offset = m_tBoltNormOffset;
		if (m_iOffsetWing == 0)
		{
			if (m_iBoltNorm == 0)	//����
				ls_stru.des_work_norm.direction = 4;
			else
				ls_stru.des_work_norm.direction = 5;
		}
		else if (m_iOffsetWing == 1)
			ls_stru.des_work_norm.direction = 2;
		else if (m_iOffsetWing == 2)
			ls_stru.des_work_norm.direction = 3;
		//ls_stru.des_work_norm.norm_wing = m_iOffsetWing;
	}
	//��˨λ���趨
	ls_stru.des_base_pos.direction = m_iRayDirection;
	ls_stru.des_base_pos.offset_wing = m_iOffsetWing;
	if (m_iDatumPointStyle == 0)	//�Ǹ����Ϊ��λ����
	{
		ls_stru.des_base_pos.datumPoint.datum_pos_style = 1;
		ls_stru.des_base_pos.hPart = ls_stru.des_base_pos.datumPoint.des_para.RODEND.hRod = m_pLinePart->handle;
		ls_stru.des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect = TRUE;
		ls_stru.des_base_pos.datumPoint.des_para.RODEND.direction = m_iRayDirection;
		ls_stru.des_base_pos.datumPoint.des_para.RODEND.offset_wing = m_iOffsetWing;
		ls_stru.des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist = 0;
		if (m_pLinePart->IsAngle())
			ls_stru.des_base_pos.datumPoint.des_para.RODEND.wing_offset_style = 4;
	}
	else if (m_iDatumPointStyle == 1 || m_iDatumPointStyle == 2)	//�ڵ���Ǹ�����ͶӰ/ƽ��Ϊ��λ����
	{
		ls_stru.des_base_pos.datumPoint.datum_pos_style = 2;
		ls_stru.des_base_pos.datumPoint.des_para.RODNODE.direction = m_iRayDirection;
		if (m_iDatumPointStyle == 1)
			ls_stru.des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset = FALSE;
		else
		{
			ls_stru.des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset = TRUE;
			ls_stru.des_base_pos.datumPoint.des_para.RODNODE.vPlaneNormal = face_norm_offset;
		}
		ls_stru.des_base_pos.hPart = ls_stru.des_base_pos.datumPoint.des_para.RODNODE.hRod = m_pLinePart->handle;
		ls_stru.des_base_pos.datumPoint.des_para.RODNODE.hNode = m_pNode ? m_pNode->handle : 0;
		ls_stru.des_base_pos.datumPoint.des_para.RODNODE.offset_wing = m_iOffsetWing;
		ls_stru.des_base_pos.datumPoint.des_para.RODNODE.direction = m_iRayDirection;
		int offset_dist_style = 0;
		double wing_offset_dist = 0;
		if (m_pLinePart->IsAngle())
		{
			CLDSLineAngle *pAngle = (CLDSLineAngle*)m_pLinePart;
			if (m_iOffsetWing == 0)
			{
				if (m_pNode&&m_pNode->hFatherPart == m_pLinePart->handle)
					offset_dist_style = m_pNode->xFatherAngleZhunJu.offset_X_dist_style;
				wing_offset_dist = pAngle->GetNodeWingXG(m_pNode);
			}
			else
			{
				if (m_pNode&&m_pNode->hFatherPart == m_pLinePart->handle)
					offset_dist_style = m_pNode->xFatherAngleZhunJu.offset_Y_dist_style;
				wing_offset_dist = pAngle->GetNodeWingYG(m_pNode);
			}
		}
		else
		{
			offset_dist_style = 4;
			wing_offset_dist = 0;
		}
		ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style = offset_dist_style;
		ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist = wing_offset_dist;
		if (offset_dist_style == 0 && wing_offset_dist != m_nG)
		{
			ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style = 4;
			ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist = m_nG;
		}
		else if (offset_dist_style == 1 && wing_offset_dist != m_nG1)
		{
			ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style = 4;
			ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist = m_nG1;
		}
		else if (offset_dist_style == 2 && wing_offset_dist != m_nG2)
		{
			ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style = 4;
			ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist = m_nG2;
		}
	}
	else if (m_iDatumPointStyle == 3)	//�Ǹ����߽���Ϊ��λ����
	{
		ls_stru.des_base_pos.datumPoint.datum_pos_style = 3;
		ls_stru.des_base_pos.hPart = ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = m_pLinePart->handle;
		sscanf(m_hCrossAngle, "%X", &ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2);
		ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = m_nG;
		ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 = m_nCrossAngleG;
		ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1 = 4;
		ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2 = 4;
	}
}
void CLayAngleBoltDlg::LayoutBolts(BOOL bUpdateData/*=TRUE*/)
{
	CDesignLsPara ls_stru;
	try{
		UpdateData();
		InitDesignLsPara(ls_stru);
		//
		CLDSLineAngle *pCrossAngle=NULL;
		BOOL bHasEndLjBolt=FALSE;
		if (m_iDatumPointStyle == 1 || m_iDatumPointStyle == 2)
		{
			if (m_bEndLjDatumJg&&m_pEndLjRayJg)	//��ǰ�Ǹ�Ϊ�����ӻ�׼�Ǹ� wht 10-01-14
			{
				bHasEndLjBolt = TRUE;
				pCrossAngle = m_pEndLjRayJg;
			}
		}
		else if (m_iDatumPointStyle == 3)
		{
			pCrossAngle = (CLDSLineAngle*)console.FromPartHandle(ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2, CLS_LINEANGLE);
			if (pCrossAngle&&m_nCrossAngleG > 0 && m_nCrossAngleG < pCrossAngle->GetWidth())
				bHasEndLjBolt = TRUE;
		}
		if(m_iLsLayoutStyle!=0&&m_iBoltRows==0)	//������˨ʱ��˨�˾������˨���з�ʽ����
			m_nLsEndSpace *=2;
		JGZJ defaultZJ,defaultCrossZJ;
		if(m_pLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{
			if(ls_stru.des_base_pos.offset_wing==0)
				defaultZJ=((CLDSLineAngle*)m_pLinePart)->GetZhunJu('X');
			else
				defaultZJ=((CLDSLineAngle*)m_pLinePart)->GetZhunJu('Y');
			if(pCrossAngle!=NULL)
			{
				f3dPoint vec1 = m_pLinePart->End()-m_pLinePart->Start();
				f3dPoint vec2 = pCrossAngle->End()-pCrossAngle->Start();
				f3dPoint norm = vec1^vec2;
				normalize(norm);
				char cWorkWing='X';
				if(fabs(norm*pCrossAngle->get_norm_x_wing())<fabs(norm*pCrossAngle->get_norm_y_wing()))
					cWorkWing='Y';
				defaultCrossZJ=pCrossAngle->GetZhunJu(cWorkWing);
			}
		}
		else
		{
			defaultZJ.g = defaultZJ.g1 = defaultZJ.g2 = defaultZJ.g3 = 0;
			defaultCrossZJ.g = defaultCrossZJ.g1 = defaultCrossZJ.g2 = defaultCrossZJ.g3 = 0;
		}
		CDesignLsPara *pLsPara=NULL;
		ATOM_LIST<CDesignLsPara> lslist;
		for(int i=0;i<m_nLsNum;i++)
		{
			if(m_bMirLayLs&&m_iDatumPointStyle!=0)
			{	//����ԳƲ���
				if(m_iBoltRows==0)	//��������ԳƲ���
				{
					if(i==0)
						ls_stru.des_base_pos.len_offset_dist=-(m_nLsNum/2)*m_nLsSpace+(m_nLsNum+1)%2*m_nLsSpace*0.5;
					else
						ls_stru.des_base_pos.len_offset_dist+=m_nLsSpace;
				}
				else				//˫������ԳƲ���
				{
					int maxrow_num=(m_nLsNum+1)/2;	//˫���в���˨�϶��������˨��
					if(i==0)
						ls_stru.des_base_pos.len_offset_dist=-m_nLsSpace*(m_nLsNum-1)/4;
					else
						ls_stru.des_base_pos.len_offset_dist+=0.5*m_nLsSpace;
				}
				if(m_iLsLayoutStyle==0)
				{	//����˨�����Ǵ��
					if(m_iBoltRows==0)	//��������ԳƲ���
						ls_stru.des_base_pos.wing_offset_dist=0;
					else				//˫������ԳƲ���
					{
						if(i%2==0)
							ls_stru.des_base_pos.wing_offset_dist=m_nG1-m_nG;
						else
							ls_stru.des_base_pos.wing_offset_dist=m_nG2-m_nG;
					}
				}
				else
				{	//����˨���Ǵ��
					if(m_iBoltRows==0)	//����
					{
						ls_stru.des_base_pos.len_offset_dist+=0.5*m_nLsSpace;
						ls_stru.des_base_pos.wing_offset_dist=0;
					}
					else				//˫��
					{
						if(i%2==0)
							ls_stru.des_base_pos.wing_offset_dist=m_nG2-m_nG;
						else
							ls_stru.des_base_pos.wing_offset_dist=m_nG1-m_nG;
					}
				}
			}
			else
			{	//���򲼿�
				if(i==0)
				{
					if(m_iDatumPointStyle==0)	//�Ǹ���㶨λ
						ls_stru.des_base_pos.len_offset_dist=m_nLsEndSpace;
					else
						ls_stru.des_base_pos.len_offset_dist=0;
				}
				else if(m_iBoltRows==0)	//���ŵ��򲼿�
					ls_stru.des_base_pos.len_offset_dist+=m_nLsSpace;
				else					//˫�ŵ��򲼿�
					ls_stru.des_base_pos.len_offset_dist+=0.5*m_nLsSpace;
				if(m_iLsLayoutStyle==0)
				{	//����˨�����Ǵ��
					if(m_iBoltRows==0)	//����
					{
						if(m_iDatumPointStyle==0)	//�Ǹ���㶨λ
							ls_stru.des_base_pos.wing_offset_dist=m_nG;
						else
							ls_stru.des_base_pos.wing_offset_dist=0;
					}
					else				//˫��
					{
						if(m_iDatumPointStyle==0)	//�Ǹ���㶨λ
						{
							if(i%2==0)
								ls_stru.des_base_pos.wing_offset_dist=m_nG1;
							else
								ls_stru.des_base_pos.wing_offset_dist=m_nG2;
						}
						else
						{
							if(i%2==0)
								ls_stru.des_base_pos.wing_offset_dist=m_nG1-m_nG;
							else
								ls_stru.des_base_pos.wing_offset_dist=m_nG2-m_nG;
						}
					}
				}
				else
				{	//��˨���Ǵ��
					if(m_iBoltRows==0)	//����
					{
						if(m_iDatumPointStyle==0)	//�Ǹ���㶨λ
							ls_stru.des_base_pos.wing_offset_dist=m_nG;
						else
							ls_stru.des_base_pos.wing_offset_dist=0;
					}
					else				//˫��
					{
						if(m_iDatumPointStyle==0)	//�Ǹ���㶨λ
						{
							if(i%2==0)
								ls_stru.des_base_pos.wing_offset_dist=m_nG2;
							else
								ls_stru.des_base_pos.wing_offset_dist=m_nG1;
						}
						else
						{
							if(i%2==0)
								ls_stru.des_base_pos.wing_offset_dist=m_nG2-m_nG;
							else
								ls_stru.des_base_pos.wing_offset_dist=m_nG1-m_nG;
						}
					}
				}
			}
			//������ڽ���Ǹ����׸���˨�׵Ķ�λ
			int iFirstOutterBoltIndex=-1,iFirstOutterBoltIndex2=1;
			if(m_bMirLayLs&&m_iDatumPointStyle!=0)
			{
				if(m_nLsNum%2>0)
				{
					iFirstOutterBoltIndex=m_nLsNum/2-1;
					iFirstOutterBoltIndex2=m_nLsNum/2+1;
				}
				else
				{
					iFirstOutterBoltIndex=m_nLsNum/2-1;
					iFirstOutterBoltIndex2=m_nLsNum/2;
				}
			}
			double old_len_offset_dist=ls_stru.des_base_pos.len_offset_dist;
			if((i==iFirstOutterBoltIndex||i==iFirstOutterBoltIndex2)&&bHasEndLjBolt&&pCrossAngle)
			{	
				ls_stru.des_base_pos.datumPoint.UpdatePos(m_pLinePart->BelongModel());
				f3dPoint line_vec,wing_vec,cross_wing_vec,cross_line_vec,cross_wing_norm;
				cross_line_vec=pCrossAngle->End()-pCrossAngle->Start();
				normalize(cross_line_vec);
				if(m_pLinePart->IsAngle())
				{
					CLDSLineAngle *pAngle=(CLDSLineAngle*)m_pLinePart;
					if(m_iOffsetWing==0)
					{
						if(fabs(pAngle->get_norm_x_wing()*pCrossAngle->get_norm_x_wing())>
							fabs(pAngle->get_norm_x_wing()*pCrossAngle->get_norm_y_wing()))
						{
							cross_wing_norm=pCrossAngle->get_norm_x_wing();
							cross_wing_vec=pCrossAngle->GetWingVecX();
						}
						else
						{
							cross_wing_norm=pCrossAngle->get_norm_y_wing();
							cross_wing_vec=pCrossAngle->GetWingVecY();
						}
						wing_vec=pAngle->GetWingVecX();
					}
					else
					{
						if(fabs(pAngle->get_norm_y_wing()*pCrossAngle->get_norm_x_wing())>
							fabs(pAngle->get_norm_y_wing()*pCrossAngle->get_norm_y_wing()))
							cross_wing_vec=pCrossAngle->GetWingVecX();
						else
							cross_wing_vec=pCrossAngle->GetWingVecY();
						wing_vec=pAngle->GetWingVecY();
					}
				}
				else
				{

				}
				if(m_iRayDirection==0)
					line_vec=m_pLinePart->End()-m_pLinePart->Start();
				else
					line_vec=m_pLinePart->Start()-m_pLinePart->End();
				normalize(wing_vec);
				normalize(line_vec);
				f3dPoint pos=ls_stru.des_base_pos.datumPoint.Position()+
					line_vec*ls_stru.des_base_pos.len_offset_dist+
					wing_vec*ls_stru.des_base_pos.wing_offset_dist;
				UCS_STRU ucs;
				ucs.origin=pCrossAngle->Start();
				ucs.axis_x=cross_line_vec;
				ucs.axis_y=cross_wing_vec;
				ucs.axis_z=ucs.axis_x^ucs.axis_y;
				coord_trans(pos,ucs,FALSE);
				double cosa=line_vec*cross_wing_vec;
				double dd=0;
				if(i==iFirstOutterBoltIndex)
					cosa*=-1.0;	//�ԳƲ�����һ�ฺ����
				if(cosa>0)	//��˨λ��֫���
					dd=pos.y-pCrossAngle->GetWidth();
				else		//��˨λ�����߲�
				{
					dd=-pos.y;
					cosa*=-1.0;
				}
				if(dd<m_nFirstLsJgSpace)	//Ŀǰ��϶С����׼�϶
				{
					double add_offset=(m_nFirstLsJgSpace-dd)/cosa;
					double round_offset=fto_halfi(add_offset/10)*10;	//Բ���������ֵ
					if(i==iFirstOutterBoltIndex&&m_bMirLayLs)
					{	//�ԳƲ�����˨ʱ�������׸������˨������˨λ��ʱ��������ǰ�����˨����ƫ����
						lslist.push_stack();
						for(pLsPara=lslist.GetFirst();pLsPara;pLsPara=lslist.GetNext())
						{
							if(round_offset<add_offset)
								pLsPara->des_base_pos.len_offset_dist-=ftoi(round_offset)+5;
							else
								pLsPara->des_base_pos.len_offset_dist-=ftoi(round_offset);
						}
						lslist.pop_stack();
					}
					if(i==iFirstOutterBoltIndex)
					{
						if(round_offset<add_offset)
							ls_stru.des_base_pos.len_offset_dist-=ftoi(round_offset)+5;
						else
							ls_stru.des_base_pos.len_offset_dist-=ftoi(round_offset);
					}
					else
					{
						if(round_offset<add_offset)
							ls_stru.des_base_pos.len_offset_dist+=ftoi(round_offset)+5;
						else
							ls_stru.des_base_pos.len_offset_dist+=ftoi(round_offset);
					}
				}
			}
			//�Ǹ����߽��㲼����˨����ڶ�����ʱ�ԳƲ�����˨Ӧ��֤һ����˨λ�ڻ���λ�� wht 10-05-26
			if(i>iFirstOutterBoltIndex&&i<iFirstOutterBoltIndex2&&(m_iDatumPointStyle==3||bHasEndLjBolt))
				ls_stru.des_base_pos.len_offset_dist=0;
			if(m_iDatumPointStyle==0)
			{
				if(ls_stru.des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist==defaultZJ.g)
					ls_stru.des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=0;	//�߱�׼׼��
				else if(ls_stru.des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist==defaultZJ.g1)
					ls_stru.des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=1;	//��׼��g1
				else if(ls_stru.des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist==defaultZJ.g2)
					ls_stru.des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=2;	//��׼��g2
				else if(ls_stru.des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist==defaultZJ.g3)
					ls_stru.des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=3;	//��׼��g3
			}
			else if(m_iDatumPointStyle==1||m_iDatumPointStyle==2)
			{
				if(ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist==defaultZJ.g)
					ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style=0;	//�߱�׼׼��
				else if(ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist==defaultZJ.g1)
					ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style=1;	//��׼��g1
				else if(ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist==defaultZJ.g2)
					ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style=2;	//��׼��g2
				else if(ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist==defaultZJ.g3)
					ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style=3;	//��׼��g3
			}
			else if(m_iDatumPointStyle==3)
			{
				if(ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1==defaultZJ.g)
					ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=0;	//�߱�׼׼��
				else if(ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1==defaultZJ.g1)
					ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=1;	//��׼��g1
				else if(ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1==defaultZJ.g2)
					ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=2;	//��׼��g2
				else if(ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1==defaultZJ.g3)
					ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=3;	//��׼��g3

				if(ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2==defaultCrossZJ.g)
					ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;	//�߱�׼׼��
				else if(ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2==defaultCrossZJ.g1)
					ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=1;	//��׼��g1
				else if(ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2==defaultCrossZJ.g2)
					ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=2;	//��׼��g2
				else if(ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2==defaultCrossZJ.g3)
					ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=3;	//��׼��g3
			}
			pLsPara=lslist.append(ls_stru);
		}
		CString str_arr[5];
		for(pLsPara=lslist.GetFirst();pLsPara;pLsPara=lslist.GetNext())
		{
			pLsPara=m_pLsList->append(*pLsPara);	//�����д��У���Ϊ��˨��¼��Ҫ��Ի���ͬ�������ڣ���������˺���ͬ����
			str_arr[0].Format("M%d",pLsPara->d);
			if(pLsPara->des_base_pos.datumPoint.datum_pos_style==3)
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[1] = "Center Line Intersection Point";
#else 
				str_arr[1] = "���߽���";
#endif
			else if(pLsPara->des_base_pos.datumPoint.datum_pos_style==1)
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[1] = "Angle Ridge Vertice";
#else 
				str_arr[1] = "�Ǹ����";
#endif
			else if(pLsPara->des_base_pos.datumPoint.datum_pos_style==2)
			{
				if(!pLsPara->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset)
#ifdef AFX_TARG_ENU_ENGLISH
					str_arr[1] = "Node project to angle's center line";
				else
					str_arr[1] = "Node offset to angle's center line";
#else 
					str_arr[1] = "�ڵ���Ǹ�����ͶӰ";
				else
					str_arr[1] = "�ڵ���Ǹ�����ƽ��";
#endif
			}
			str_arr[2].Format("%f",pLsPara->des_base_pos.len_offset_dist);
			SimplifiedNumString(str_arr[2]);
			str_arr[3].Format("%f",pLsPara->des_base_pos.wing_offset_dist);
			SimplifiedNumString(str_arr[3]);
			str_arr[4].Format("%d",pLsPara->des_base_pos.norm_offset.Thick(m_pLinePart->BelongModel()));
			AppendRecord(str_arr,(void *)pLsPara);
			PreviewBolt(pLsPara);
		}
		double g,datum_g=0,max_g=0,min_g=m_pLinePart->GetWidth();
		f3dPoint startPos=m_pLinePart->Start();
		f3dPoint endPos=m_pLinePart->End();
		for(pLsPara=m_pLsList->GetFirst();pLsPara;pLsPara=m_pLsList->GetNext())
		{
			pLsPara->des_base_pos.datumPoint.UpdatePos(m_pLinePart->BelongModel());
			f3dPoint pos=pLsPara->des_base_pos.datumPoint.Position();
			if(m_pLinePart->IsAngle())
			{
				CLDSLineAngle *pAngle=(CLDSLineAngle*)m_pLinePart;
				if(pLsPara->des_base_pos.offset_wing==0)
					project_point(pos,pAngle->Start(),pAngle->get_norm_x_wing());
				else
					project_point(pos,pAngle->Start(),pAngle->get_norm_y_wing());
			}
			else if(m_pLinePart->GetClassTypeId()==CLS_LINEFLAT)
			{
				CLDSLineFlat *pFlat=(CLDSLineFlat*)m_pLinePart;
				project_point(pos,pFlat->Start(),pFlat->WorkPlaneNorm());
				startPos-=pFlat->ucs.axis_y*(m_pLinePart->GetWidth()*0.5);
				endPos-=pFlat->ucs.axis_y*(m_pLinePart->GetWidth()*0.5);
			}
			else
			{
				CLDSLineSlot *pSlot=(CLDSLineSlot*)m_pLinePart;
				project_point(pos,pSlot->Start(),pSlot->WorkPlaneNorm());
			}
			datum_g=DistOf3dPtLine(pos,startPos,endPos);
			g=pLsPara->des_base_pos.wing_offset_dist+datum_g;
			if(g>max_g)
				max_g=g;
			if(g<min_g)
				min_g=g;
		}
		//������˨���ֶܷ�ν��в��ã���ÿ�β�������˨����Ҫ����m_fBerSpace��m_fWingSpace
		//���û�е��������˨��ťֱ�ӵ���ȷ����������ִ�����²���bUpdateData=FALSE
		if(bUpdateData)
			IntelliUpdateBoltSpace();
		UpdateData(FALSE);
		CWnd *pButtonOK=GetDlgItem(IDOK);
		pButtonOK->SetFocus();
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
}

void CLayAngleBoltDlg::OnBnLayoutBolts() 
{
	//���²�����˨��ť��Ҫ�������߲�߾࣬֫���߾�
	LayoutBolts(TRUE);
	WorkPlaneSketch();
}

void CLayAngleBoltDlg::OnSelchangeCmbDatumPointStyle() 
{
	int iOldDatumPointStyle=m_iDatumPointStyle;
	UpdateData();
	if(m_pNode==NULL&&(m_iDatumPointStyle==1||m_iDatumPointStyle==2))
	{
		m_iDatumPointStyle=iOldDatumPointStyle;
		UpdateData(FALSE);
		AfxMessageBox("δѡ���׼�ڵ㲻֧��[�ڵ���������ͶӰ(ƽ��)]������˨��");
		return;
	}
	//ֻ�������߽��㶨λ��ʽʱ��׼�϶�������� wjh-2013-9-7
	GetDlgItem(IDC_E_FIRST_LS_JG_SPACE)->EnableWindow(m_iDatumPointStyle==3);
	//ֻ���ڽǸ���㶨λ��ʽʱ�˾�������� wjh-2013-9-7
	GetDlgItem(IDC_E_LS_END_SPACE)->EnableWindow(m_iDatumPointStyle==0);
	if(m_iDatumPointStyle==3)	
	{	//��λ��ʽΪ���߽���ʱ ������Ҫѡ����ϽǸֵ��ӽǸ� ���л�Ϊʵ����ʾģʽ
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		Invalidate(FALSE);
	}
	if(m_iDatumPointStyle==2)
	{
		GetDlgItem(IDC_S_OFFSET_FACE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CMB_TYPE)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_BN_SEL_CROSS_ANGLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_CROSS_ANGLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_CROSS_ANGLE_G)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_S_ZHUNJU)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_S_OFFSET_FACE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CMB_TYPE)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_BN_SEL_CROSS_ANGLE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_CROSS_ANGLE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_CROSS_ANGLE_G)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_S_ZHUNJU)->ShowWindow(SW_SHOW);
	}
	GetDlgItem(IDC_E_CROSS_ANGLE)->EnableWindow(m_iDatumPointStyle==3);		//���߽���
	GetDlgItem(IDC_BN_SEL_CROSS_ANGLE)->EnableWindow(m_iDatumPointStyle==3);		//���߽���
	GetDlgItem(IDC_E_CROSS_ANGLE_G)->EnableWindow(m_iDatumPointStyle==3);	//���߽���
	GetDlgItem(IDC_CMB_TYPE)->EnableWindow(m_iDatumPointStyle==2);
	GetDlgItem(IDC_CHK_MIR_LAY_LS)->EnableWindow(m_iDatumPointStyle>0);
	if(m_iDatumPointStyle==0)	//�Ǹ����Ϊ��λ����
	{
		if(m_iBoltRows==0)	//����
			m_fWingOffsetDist = m_nG;
		else if(m_iLsLayoutStyle==0)	//����˨���ڲ�����
			m_fWingOffsetDist = m_nG1;
		else							//����˨���������
			m_fWingOffsetDist = m_nG2;
		m_fLenOffsetDist = m_nLsEndSpace;
		UpdateData(FALSE);
	}
	else //
	{
		if(m_iDatumPointStyle==2)	//ƽ��
		{
			//ƽ���淨��ӦĬ������˨��λ��׼�Ǹ�ƽ���淨��һ��
			if (m_pLinePart->IsAngle())
			{
				CLDSLineAngle *pAngle=(CLDSLineAngle*)m_pLinePart;
				if(pAngle->desStartPos.IsFaceOffset())
					face_norm_offset=pAngle->desStartPos.face_offset_norm;
				else if(pAngle->desEndPos.IsFaceOffset())
					face_norm_offset=pAngle->desEndPos.face_offset_norm;
			}
			else if (m_pLinePart->GetClassTypeId()==CLS_LINEFLAT)
			{
                CLDSLineFlat *pFlat=(CLDSLineFlat*)m_pLinePart;
				if (pFlat->desStartPos.datum_pos_style>2)
					face_norm_offset=pFlat->desStartPos.face_offset_norm;
				else if(pFlat->desEndPos.datum_pos_style>2)
					face_norm_offset=pFlat->desEndPos.face_offset_norm;
			}
			else
			{
				CLDSLineSlot *pSlot=(CLDSLineSlot*)m_pLinePart;
				if (pSlot->desStartPos.datum_pos_style>2)
					face_norm_offset=pSlot->desStartPos.face_offset_norm;
				else if(pSlot->desEndPos.datum_pos_style>2)
					face_norm_offset=pSlot->desEndPos.face_offset_norm;
			}
		}
		m_fLenOffsetDist = 0;
		m_fWingOffsetDist = 0;
		UpdateData(FALSE);
	}
}

void CLayAngleBoltDlg::OnChangeCmbBoltSize() 
{
	CString sText;
	CComboBox *pCmb = (CComboBox*)GetDlgItem(IDC_CMB_BOLT_SIZE);
	int iCur = pCmb->GetCurSel();
	UpdateData();
	pCmb->GetLBText(iCur,m_sBoltSize);
	int ls_d = atoi(m_sBoltSize);

	if(ls_d<=12)
		m_nFirstLsJgSpace = 15;
	else if(ls_d<=16)//M20
		m_nFirstLsJgSpace = 20;
	else if(ls_d<=20)
		m_nFirstLsJgSpace = 25;
	else	//>=M24
		m_nFirstLsJgSpace = 30;
	LSSPACE_STRU LsSpace;
	if(GetLsSpace(LsSpace,ls_d))
	{
		if(m_iBoltRows==0)
			m_nLsSpace = LsSpace.SingleRowSpace;
		else
			m_nLsSpace = LsSpace.doubleRowSpace;
		m_fPlateEdgeSpace=m_nLsEndSpace = LsSpace.EndSpace;
		if(m_iDatumPointStyle==0)
			m_fLenOffsetDist = LsSpace.EndSpace;
		else
			m_fLenOffsetDist = 0;
		if(m_bEndLjJg)
			m_fOddment = LsSpace.EndSpace;
		if(m_iBerSpaceMode==1)
			m_fBerSpace=LsSpace.EndSpace;
		if(m_iWingSpaceMode==1)
			m_fWingSpace=LsSpace.EndSpace;
		UpdateData(FALSE);
	}
}

void CLayAngleBoltDlg::OnChangeECrossAngle() 
{
	UpdateData();
	char tem_str[51];
	_snprintf(tem_str,50,"%s",m_hCrossAngle);
	long hPart=0;
	sscanf(tem_str,"%X",&hPart);
	CLDSLineAngle *pCrossAngle = (CLDSLineAngle*)console.FromPartHandle(hPart,CLS_LINEANGLE);
	m_nCrossAngleG = 0;
	if(pCrossAngle)
	{
		JGZJ jgzj;
		if(getjgzj(jgzj,pCrossAngle->GetWidth()))
			m_nCrossAngleG = jgzj.g;
	}
	UpdateData(FALSE);
}

void CLayAngleBoltDlg::OnLsLayoutStyle() 
{
	UpdateData();
	if(m_iDatumPointStyle==0)
	{
		if(m_iBoltRows==0)
			m_fWingOffsetDist=m_nG;
		else if(m_iLsLayoutStyle==0)
			m_fWingOffsetDist=m_nG1;
		else
			m_fWingOffsetDist=m_nG2;
		UpdateData(FALSE);
	}
}

void CLayAngleBoltDlg::OnBnModifyBoltItem() 
{
	m_pLsSetList=(CListCtrl*)GetDlgItem(IDC_LIST_LS_SET);
	POSITION pos = m_pLsSetList->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pLsSetList->GetNextSelectedItem(pos);
		UpdateData();
		if(iCurSel>=0)
		{
			CSuperGridCtrl::CTreeItem *pCurItem=m_xListCtrlLsSet.GetTreeItem(iCurSel);
			CDesignLsPara *pLsPara=(CDesignLsPara*)pCurItem->m_idProp;
			pLsPara->d = atoi(m_sBoltSize);
			pLsPara->des_base_pos.direction=m_iRayDirection;
			pLsPara->des_work_norm.direction=m_iBoltNorm;
			if(m_iDatumPointStyle==0)
			{
				pLsPara->des_base_pos.datumPoint.des_para.RODEND.direction=m_iRayDirection;
				pLsPara->des_base_pos.datumPoint.datum_pos_style=1;	//�Ǹ����
			}
			if(m_iDatumPointStyle==1||m_iDatumPointStyle==2)
			{
				pLsPara->des_base_pos.datumPoint.des_para.RODNODE.direction=m_iRayDirection;
				pLsPara->des_base_pos.datumPoint.datum_pos_style=2;
				if(m_iDatumPointStyle==1)		//�ڵ���Ǹ���ͶӰ
					pLsPara->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset=FALSE;
				else	//ƽ��Ϊ��λ����
					pLsPara->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset=TRUE;	
			}
			else if(m_iDatumPointStyle==3)
			{
				pLsPara->des_base_pos.hPart=pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1=m_pLinePart->handle;
				char temstr[50];
				sprintf(temstr,"%s",m_hCrossAngle);
				sscanf(temstr,"%X",&pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2);
				pLsPara->des_base_pos.datumPoint.datum_pos_style=3;
				pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = m_nG;
				pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 = m_nCrossAngleG;
				pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
				pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=4;
			}
			pLsPara->des_base_pos.len_offset_dist = m_fLenOffsetDist;
			pLsPara->des_base_pos.wing_offset_dist = m_fWingOffsetDist;
			pLsPara->des_base_pos.norm_offset = m_tBoltNormOffset;
			//������˨�б�
			CString ss;
			ss.Format("M%d",pLsPara->d);
			m_pLsSetList->SetItemText(iCurSel,0,ss);
			if(pLsPara->des_base_pos.datumPoint.datum_pos_style==3)
#ifdef AFX_TARG_ENU_ENGLISH
				m_pLsSetList->SetItemText(iCurSel,1,"Center Line Intersection Point");
			else if(pLsPara->des_base_pos.datumPoint.datum_pos_style==1)
				m_pLsSetList->SetItemText(iCurSel,1,"Angle Ridge Vertice");
#else 
				m_pLsSetList->SetItemText(iCurSel,1,"���߽���");
			else if(pLsPara->des_base_pos.datumPoint.datum_pos_style==1)
				m_pLsSetList->SetItemText(iCurSel,1,"�Ǹ����");
#endif
			else if(pLsPara->des_base_pos.datumPoint.datum_pos_style==2)
			{
				if(m_iDatumPointStyle==1)
#ifdef AFX_TARG_ENU_ENGLISH
					m_pLsSetList->SetItemText(iCurSel,1,"Node project to angle's center line");
				else
					m_pLsSetList->SetItemText(iCurSel,1,"Node offset to angle's center line");
#else 
					m_pLsSetList->SetItemText(iCurSel,1,"�ڵ���Ǹ�����ͶӰ");
				else
					m_pLsSetList->SetItemText(iCurSel,1,"�ڵ���Ǹ�����ƽ��");
#endif
			}
			ss.Format("%f",pLsPara->des_base_pos.len_offset_dist);
			SimplifiedNumString(ss);
			m_pLsSetList->SetItemText(iCurSel,2,ss);
			ss.Format("%f",pLsPara->des_base_pos.wing_offset_dist);
			SimplifiedNumString(ss);
			m_pLsSetList->SetItemText(iCurSel,3,ss);
			ss.Format("%d",pLsPara->des_base_pos.norm_offset.Thick(m_pLinePart->BelongModel()));
			m_pLsSetList->SetItemText(iCurSel,4,ss);
		}
	}
}

void CLayAngleBoltDlg::OnSelchangeCmbLsRows() 
{
	UpdateData();
	LSSPACE_STRU LsSpace;
	BOOL bRet=FALSE;
	long ls_d=atoi(m_sBoltSize);
	if(GetLsSpace(LsSpace,ls_d))
	{
		if(m_iBoltRows==0)
			m_nLsSpace = LsSpace.SingleRowSpace;
		else
			m_nLsSpace = LsSpace.doubleRowSpace;
	}
	UpdateData(FALSE);
}

void CLayAngleBoltDlg::OnRdoBoltNorm() 
{
	UpdateData();
	if ((m_pLinePart->IsAngle()&&m_iBoltNorm==0)	//����
		||(m_pLinePart->GetClassTypeId()==CLS_LINESLOT&&m_iBoltNorm==0))
		sprintf(m_tBoltNormOffset.key_str,"-0X%X",m_pLinePart->handle);
	else if(m_pLinePart->GetClassTypeId()==CLS_LINEFLAT&&m_iBoltNorm==1)	//����
		sprintf(m_tBoltNormOffset.key_str,"-0X%X,-0X%X",m_pLinePart->handle,m_hPlate);
	else 
		sprintf(m_tBoltNormOffset.key_str,"-0X%X",m_hPlate);
}

BOOL UnifyAngleBoltParamG(DESIGN_LS_POS &designLsPos);
BOOL CLayAngleBoltDlg::IsOverlapLegacyLs(CDesignLsPara *pLsPara)
{
	if(m_pLegacyLsList==NULL||pLsPara==NULL||!m_pLinePart->IsAngle())
		return FALSE;
	if(pLsPara->des_base_pos.hPart!=m_pLinePart->handle)
		return FALSE;
	CLDSBolt temp_ls;
	pLsPara->DesignLs(&temp_ls);
	CLDSLineAngle* pAngle=(CLDSLineAngle*)m_pLinePart;
	f3dPoint lsPos1,lsPos2,face_norm,face_pt=pAngle->Start();
	if(pLsPara->des_work_norm.norm_wing==0)
		face_norm=pAngle->vxWingNorm;
	else
		face_norm=pAngle->vyWingNorm;
	Int3dlf(lsPos1,temp_ls.ucs.origin,temp_ls.ucs.axis_z,face_pt,face_norm);
	for(CLDSBolt* pBolt=m_pLegacyLsList->GetFirst();pBolt;pBolt=m_pLegacyLsList->GetNext())
	{
		if(m_pLinePart->FindLsByHandle(pBolt->handle)==NULL)
			continue;
		Int3dlf(lsPos2,pBolt->ucs.origin,pBolt->ucs.axis_z,face_pt,face_norm);
		if(lsPos1.IsEqual(lsPos2,1))
			return TRUE;
	}
	return FALSE;
}
void CLayAngleBoltDlg::OnOK() 
{
	//��Ҫ����˨����û�в���ʱ��ȷ��,��ʱϵͳ���Զ�������˨
	//δ���²�����˨��ť���������߲�߾ࡢ֫���߾࣬�����޸Ĺ������ݽ������
	if (m_pLsList&&m_pLsList->GetNodeNum() == 0 && m_nLsNum > 0)
	{
		LayoutBolts(FALSE);
		WorkPlaneSketch();
	}
	if (m_pLegacyLsList != NULL)
	{	//�ڶ����Ӹ˼�������������˨���ڲ���ʱ��Ҫɾ���ص�����˨��Ʋ���
		for(CDesignLsPara* pLsPara=m_pLsList->GetFirst();pLsPara;pLsPara=m_pLsList->GetNext())
		{
			if(IsOverlapLegacyLs(pLsPara))
				m_pLsList->DeleteCursor();
		}
		m_pLsList->Clean();
	}
	//��˨λ�ú���ƫ�Ʒ�ʽʶ��
	if(m_pLinePart->IsAngle())
	{
		for(CDesignLsPara* pLsPara=m_pLsList->GetFirst();pLsPara;pLsPara=m_pLsList->GetNext())
		{
			if(pLsPara->des_base_pos.datumPoint.datum_pos_style==2)	//��ֹ�˴������׼����ýǸ��趨������׼�಻ƥ��,�������º����Զ�����׼�� wjh-2016.11.8
				pLsPara->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style=4;
			else if(pLsPara->des_base_pos.datumPoint.datum_pos_style==1||pLsPara->des_base_pos.datumPoint.datum_pos_style==7)
				pLsPara->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
			UnifyAngleBoltParamG(pLsPara->des_base_pos);	//ͳһ�Ǹ���˨׼�ݲ��� wht 16-10-09
		}
	}
	if(m_iBerSpaceMode==0)
		cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_UDF);
	else if(m_iBerSpaceMode==1)
		cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
	else if(m_iBerSpaceMode==2)
		cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
	else if(m_iBerSpaceMode==3)
		cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOBER);
	if(m_iWingSpaceMode==0)
		cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_UDF);
	else if(m_iWingSpaceMode==1)
		cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
	else if(m_iWingSpaceMode==2)
		cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
	else if(m_iWingSpaceMode==3)
		cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOBER);
	//ɾ������Ԥ��ʾ����˨
	for(CLDSBolt *pBolt=hashTempPrevViewBolts.GetFirst();pBolt;pBolt=hashTempPrevViewBolts.GetNext())
		g_pSolidDraw->DelEnt(pBolt->handle);
	DelWorkPlaneSketch();
	CDialog::OnOK();
}

void CLayAngleBoltDlg::OnCancel()
{	//ɾ������Ԥ��ʾ����˨
	for(CLDSBolt *pBolt=hashTempPrevViewBolts.GetFirst();pBolt;pBolt=hashTempPrevViewBolts.GetNext())
		g_pSolidDraw->DelEnt(pBolt->handle);
	DelWorkPlaneSketch();
	CCallBackDialog::OnCancel();
}

void CLayAngleBoltDlg::OnRdoOffsetWing() 
{
	UpdateData();
	if (m_pLinePart->IsAngle())
	{
		CLDSNode* pDatumNode=NULL;
		if(m_hBaseNode.GetLength()>0)
			pDatumNode=Ta.Node.FromHandle(HexStrToLong(m_hBaseNode));
		CLDSLineAngle *pAngle=(CLDSLineAngle*)m_pLinePart;
		if(m_iOffsetWing==0)	//X֫Ϊƫ��֫
		{	//�л����֫ʱ��Ҫ��ʱ���¶�Ӧ֫��׼��ֵ
			JGZJ jgzj=pAngle->GetZhunJu('X');
			m_nG=jgzj.g;
			m_nG1=jgzj.g1;
			m_nG2=jgzj.g2;
			//
			if(pAngle->pStart==m_pNode)
			{
				GetDlgItem(IDC_E_ANGLE_END_NORM_OFFSET)->EnableWindow(pAngle->desStartPos.wing_x_offset.gStyle==4);
				if(pAngle->desStartPos.wing_x_offset.gStyle==4)
					m_fAngleEndNormOffset=pAngle->desStartPos.wing_x_offset.offsetDist;
			}
			else if(pAngle->pEnd==m_pNode)
			{
				GetDlgItem(IDC_E_ANGLE_END_NORM_OFFSET)->EnableWindow(pAngle->desEndPos.wing_x_offset.gStyle==4);
				if(pAngle->desEndPos.wing_x_offset.gStyle==4)
					m_fAngleEndNormOffset=pAngle->desEndPos.wing_x_offset.offsetDist;
			}
			else if(pDatumNode)
				m_nG=ftoi(pAngle->GetNodeWingXG(pDatumNode));
		}
		else					//Y֫Ϊƫ��֫
		{
			JGZJ jgzj=pAngle->GetZhunJu('Y');
			m_nG=jgzj.g;
			m_nG1=jgzj.g1;
			m_nG2=jgzj.g2;
			//
			if(pAngle->pStart==m_pNode)
			{
				GetDlgItem(IDC_E_ANGLE_END_NORM_OFFSET)->EnableWindow(pAngle->desStartPos.wing_y_offset.gStyle==4);
				if(pAngle->desStartPos.wing_y_offset.gStyle==4)
					m_fAngleEndNormOffset=pAngle->desStartPos.wing_y_offset.offsetDist;
			}
			else if(pAngle->pEnd==m_pNode)
			{
				GetDlgItem(IDC_E_ANGLE_END_NORM_OFFSET)->EnableWindow(pAngle->desEndPos.wing_y_offset.gStyle==4);
				if(pAngle->desEndPos.wing_y_offset.gStyle==4)
					m_fAngleEndNormOffset=pAngle->desEndPos.wing_y_offset.offsetDist;
			}
			else if(pDatumNode)
				m_nG=ftoi(pAngle->GetNodeWingYG(pDatumNode));
		}
	}
	else if(m_pLinePart->GetClassTypeId()==CLS_LINEFLAT)
	{
		CLDSLineFlat* pFlat=(CLDSLineFlat*)m_pLinePart;
		GetDlgItem(IDC_E_ANGLE_END_NORM_OFFSET)->EnableWindow();
		if(pFlat->pStart==m_pNode)
			m_fAngleEndNormOffset=pFlat->desStartPos.fNormOffset;
		else if(pFlat->pEnd==m_pNode)
			m_fAngleEndNormOffset=pFlat->desEndPos.fNormOffset;
	}
	else
	{

	}
	UpdateData(FALSE);
	
}

void CLayAngleBoltDlg::OnChangeOddment() 
{	//�ֶ��޸�����ͷ������ͷ���㷽ʽ����Ϊ����������ͷ
	if(m_pLinePart->pStart==m_pNode)
		m_pLinePart->desStartOdd.m_iOddCalStyle=m_iOddCalStyle=2;	
	else if(m_pLinePart->pEnd==m_pNode)
		m_pLinePart->desEndOdd.m_iOddCalStyle=m_iOddCalStyle=2;
	CComboBox* pCMB=(CComboBox*)GetDlgItem(IDC_CMB_ODD_CAL_STYLE);
	pCMB->SetCurSel(m_iOddCalStyle);
}

void CLayAngleBoltDlg::OnSelchangeCmbOddCalStyle()
{
	UpdateData();
	if(m_iOddCalStyle==0)
	{
		if(m_pLinePart->pStart==m_pNode)
		{
			m_pLinePart->CalStartOddment();
			m_fOddment=m_pLinePart->startOdd();
		}
		else if(m_pLinePart->pEnd==m_pNode)
		{
			m_pLinePart->CalEndOddment();
			m_fOddment=m_pLinePart->endOdd();
		}
		UpdateData(FALSE);
	}
}

void CLayAngleBoltDlg::UpdatePlateSpace()
{
	//���ܸ�����˨�߾�
	IntelliUpdateBoltSpace();
	//���������˨ֱ�������¶˾�
	int nOutBoltD = atoi(m_sBoltSize);
	if(m_pLsList && m_pLsList->GetNodeNum() > 0)
	{
		CDesignLsPara *pLsPara = m_pLsList->GetFirst();
		if (pLsPara)
			nOutBoltD = pLsPara->d;
		if (m_bTwoEdgeProfile && (pLsPara = m_pLsList->GetTail()))
			nOutBoltD = max(nOutBoltD, pLsPara->d);
	}
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace, nOutBoltD);
	m_fPlateEdgeSpace = LsSpace.EndSpace;
	UpdateData(FALSE);
}

void CLayAngleBoltDlg::IntelliUpdateBoltSpace() 
{
	//�������ֱ����˨������˨�߾�
	int nMaxBoltD=0;
	if (m_pLsList&&m_pLsList->GetNodeNum() > 0)
	{	//���ݲ�����˨��ȡ���ֱ��
		nMaxBoltD = m_pLsList->GetFirst()->d;
		for (CDesignLsPara *pLsPara = m_pLsList->GetNext(); pLsPara; pLsPara = m_pLsList->GetNext())
		{
			if (pLsPara->d > nMaxBoltD)
				nMaxBoltD = pLsPara->d;
		}
	}
	else
		nMaxBoltD = atoi(m_sBoltSize);
	LSSPACE_STRU LsSpace;
	if(m_iBerSpaceMode==1&&GetLsSpace(LsSpace, nMaxBoltD))
		m_fBerSpace=LsSpace.EndSpace;
	if(m_iWingSpaceMode==1&&GetLsSpace(LsSpace, nMaxBoltD))
		m_fWingSpace=LsSpace.EndSpace;
	if(m_iBerSpaceMode!=1 || m_iWingSpaceMode!=1)
	{
		double g,datum_g=0,max_g=0,min_g=m_pLinePart->GetWidth();
		f3dPoint startPos=m_pLinePart->Start();
		f3dPoint endPos=m_pLinePart->End();
		for(CDesignLsPara *pLsPara=m_pLsList->GetFirst();pLsPara;pLsPara=m_pLsList->GetNext())
		{
			pLsPara->des_base_pos.datumPoint.UpdatePos(m_pLinePart->BelongModel());
			f3dPoint pos=pLsPara->des_base_pos.datumPoint.Position();
			if (m_pLinePart->IsAngle())
			{
				CLDSLineAngle *pAngle=(CLDSLineAngle*)m_pLinePart;
				if(pLsPara->des_base_pos.offset_wing==0)
					project_point(pos,pAngle->Start(),pAngle->get_norm_x_wing());
				else
					project_point(pos,pAngle->Start(),pAngle->get_norm_y_wing());
			}
			else if (m_pLinePart->GetClassTypeId()==CLS_LINEFLAT)
			{
				CLDSLineFlat *pFlat=(CLDSLineFlat*)m_pLinePart;
				project_point(pos,pFlat->Start(),pFlat->WorkPlaneNorm());
				startPos-=pFlat->ucs.axis_y*(m_pLinePart->GetWidth()*0.5);
				endPos-=pFlat->ucs.axis_y*(m_pLinePart->GetWidth()*0.5);
			}
			else
			{
				CLDSLineSlot *pSlot=(CLDSLineSlot*)m_pLinePart;
				project_point(pos,pSlot->Start(),pSlot->WorkPlaneNorm());
			}
			datum_g=DistOf3dPtLine(pos,startPos,endPos);
			g=pLsPara->des_base_pos.wing_offset_dist+datum_g;
			if(g>max_g)
				max_g=g;
			if(g<min_g)
				min_g=g;
		}
		if(m_iBerSpaceMode==0)
		{	//��������ʱ,������߲�Ϊָ���߾�,��Ĭ��Ϊ'��������-֫��' wjh-2015.2.28
			if(m_bTwoEdgeProfile)
				m_fBerSpace=ftoi(min_g-m_pLinePart->GetThick());
			else
				m_fBerSpace=ftoi(min_g);
		}
		else if(m_iBerSpaceMode==2)
			m_fBerSpace=ftoi(min_g);
		else if(m_iBerSpaceMode==3)
			m_fBerSpace=ftoi(min_g-m_pLinePart->GetThick());
		if(m_iWingSpaceMode==0 || m_iWingSpaceMode==2)
			m_fWingSpace=ftoi(m_pLinePart->GetWidth()-max_g);
		else if(m_iWingSpaceMode==3)
			m_fWingSpace=ftoi(m_pLinePart->GetWidth()-max_g-m_pLinePart->GetThick());
	}
}

void CLayAngleBoltDlg::OnBnSelCrossAngle() 
{
	UpdateData();
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	m_dwSelectPartsType=GetSingleWord(SELECTINDEX_LINEANGLE);
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCmdPickPrompt="Please select cross auxiliary angle which the cross bolt locate��<Enter key>Confirm";
#else 
	m_sCmdPickPrompt="��ѡ�����߽���㶨λ��˨�Ľ��渨���Ǹ֣�<�س���>ȷ��";
#endif
	m_iPickObjType=0;	//ѡ�����߽���Ǹ�
	CDialog::OnOK();
}

static BOOL FireLButtonDblclk(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,int iSubItem) 
{
	CModifyJgLsDlg dlg;
	CString ss;
	char tem_str[100]="";
	CListCtrl* m_pLsSetList =pListCtrl; //Ϊ���ٸĶ����� ���ⶨ����ʱ����
	int iCurSel=-1;
	POSITION pos = m_pLsSetList->GetFirstSelectedItemPosition();
	if(pos!=NULL)
		iCurSel = m_pLsSetList->GetNextSelectedItem(pos);
	if(iCurSel<0)
		return FALSE;
	CLayAngleBoltDlg *pLayAngleBoltDlg=(CLayAngleBoltDlg*)pListCtrl->GetParent();
	CDesignLsPara *pLsPara=(CDesignLsPara*)pItem->m_idProp;
	if(pLsPara==NULL)
		return FALSE;
	if(pLsPara->des_base_pos.datumPoint.datum_pos_style==2&&
		pLsPara->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset)
	{
		dlg.face_norm_offset=pLsPara->des_base_pos.datumPoint.des_para.RODNODE.vPlaneNormal;
	}
	m_pLsSetList->GetItemText(iCurSel,0,tem_str,100);
	dlg.m_sLsGuiGe.Format("%s",tem_str+1);
	m_pLsSetList->GetItemText(iCurSel,1,tem_str,100);
	ss.Format("%s",tem_str);
#ifdef AFX_TARG_ENU_ENGLISH
	if(ss.CompareNoCase("Angle Ridge Vertice")==0)
		dlg.m_iLsPosStyle = 0;
	else if(ss.CompareNoCase("Node projects to the angle's center line")==0)
		dlg.m_iLsPosStyle = 1;	//2
	else if(ss.CompareNoCase("Center Line Intersection Point")==0)
#else 
	if(ss.CompareNoCase("�Ǹ����")==0)
		dlg.m_iLsPosStyle = 0;
	else if(ss.CompareNoCase("�ڵ���Ǹ�����ͶӰ")==0)
		dlg.m_iLsPosStyle = 1;	//2
	else if(ss.CompareNoCase("���߽���")==0)
#endif
	{
		dlg.m_iLsPosStyle = 3;
		dlg.m_hRayJg.Format("0X%X",pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2);
		dlg.m_nRayJgG=pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2;
		dlg.m_nDatumJgG=pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1;
	}
	if(pLsPara->des_base_pos.datumPoint.datum_pos_style==2)
	{
		if(pLsPara->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset==FALSE)
			dlg.m_iLsPosStyle = 1;	//�ڵ���Ǹ���ͶӰΪ��λ����
		else
			dlg.m_iLsPosStyle = 2;	//�ڵ��ڽǸ�����ƽ��Ϊ��λ����
	}
	dlg.m_iRayDirection=pLsPara->des_base_pos.direction;
	//m_iBoltNorm=pLsPara->des_work_norm.direction;
	m_pLsSetList->GetItemText(iCurSel,2,tem_str,100);
	dlg.m_fAxisLen = atof(tem_str);
	m_pLsSetList->GetItemText(iCurSel,3,tem_str,100);
	dlg.m_fWingOffsetDist = atof(tem_str);
	dlg.m_sBoltNormOffset.Format("%s",(char*)pLsPara->des_base_pos.norm_offset.key_str);
	if(dlg.DoModal()==IDOK)
	{
		pLsPara->d = atoi(dlg.m_sLsGuiGe);
		pLsPara->des_base_pos.direction=dlg.m_iRayDirection;
		//pLsPara->des_work_norm.direction=m_iBoltNorm;
		if(dlg.m_iLsPosStyle==0)
		{
			pLsPara->des_base_pos.datumPoint.des_para.RODEND.direction=dlg.m_iRayDirection;
			pLsPara->des_base_pos.datumPoint.datum_pos_style=1;	//�Ǹ����
		}
		if(dlg.m_iLsPosStyle==1||dlg.m_iLsPosStyle==2)
		{
			pLsPara->des_base_pos.datumPoint.des_para.RODNODE.direction=dlg.m_iRayDirection;
			pLsPara->des_base_pos.datumPoint.datum_pos_style=2;
			if(dlg.m_iLsPosStyle==1)		//�ڵ���Ǹ���ͶӰ
				pLsPara->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset=FALSE;
			else	//ƽ��Ϊ��λ����
			{
				pLsPara->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset=TRUE;
				pLsPara->des_base_pos.datumPoint.des_para.RODNODE.vPlaneNormal=dlg.face_norm_offset;
			}
		}
		else if(dlg.m_iLsPosStyle==3)
		{
			pLsPara->des_base_pos.hPart=pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1=pLayAngleBoltDlg->m_pLinePart->handle;
			char temstr[50];
			sprintf(temstr,"%s",dlg.m_hRayJg);
			sscanf(temstr,"%X",&pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2);
			pLsPara->des_base_pos.datumPoint.datum_pos_style=3;
			pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = dlg.m_nDatumJgG;
			pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 = dlg.m_nRayJgG;
			pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
			pLsPara->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=4;
		}
		pLsPara->des_base_pos.len_offset_dist = dlg.m_fAxisLen;
		pLsPara->des_base_pos.wing_offset_dist = dlg.m_fWingOffsetDist;
		pLsPara->des_base_pos.norm_offset.key_str.Copy(dlg.m_sBoltNormOffset);
		pLayAngleBoltDlg->PreviewBolt(pLsPara);	//Ԥ����˨
		//������˨�б�
		CString ss;
		ss.Format("M%d",pLsPara->d);
		m_pLsSetList->SetItemText(iCurSel,0,ss);
		if(pLsPara->des_base_pos.datumPoint.datum_pos_style==3)
#ifdef AFX_TARG_ENU_ENGLISH
			m_pLsSetList->SetItemText(iCurSel,1,"Center Line Intersection Point");
#else 
			m_pLsSetList->SetItemText(iCurSel,1,"���߽���");
#endif
		else if(pLsPara->des_base_pos.datumPoint.datum_pos_style==1)
#ifdef AFX_TARG_ENU_ENGLISH
			m_pLsSetList->SetItemText(iCurSel,1,"Angle Ridge Vertice");
#else 
			m_pLsSetList->SetItemText(iCurSel,1,"�Ǹ����");
#endif
		else if(pLsPara->des_base_pos.datumPoint.datum_pos_style==2)
		{
			if(pLayAngleBoltDlg->m_iDatumPointStyle==1)
#ifdef AFX_TARG_ENU_ENGLISH
				m_pLsSetList->SetItemText(iCurSel,1,"Node project to angle's center line");
			else
				m_pLsSetList->SetItemText(iCurSel,1,"Node offset to angle's center line");
#else 
				m_pLsSetList->SetItemText(iCurSel,1,"�ڵ���Ǹ�����ͶӰ");
			else
				m_pLsSetList->SetItemText(iCurSel,1,"�ڵ���Ǹ�����ƽ��");
#endif
		}
		ss.Format("%f",pLsPara->des_base_pos.len_offset_dist);
		SimplifiedNumString(ss);
		m_pLsSetList->SetItemText(iCurSel,2,ss);
		ss.Format("%f",pLsPara->des_base_pos.wing_offset_dist);
		SimplifiedNumString(ss);
		m_pLsSetList->SetItemText(iCurSel,3,ss);
		ss.Format("%d",pLsPara->des_base_pos.norm_offset.Thick(pLayAngleBoltDlg->m_pLinePart->BelongModel()));
		m_pLsSetList->SetItemText(iCurSel,4,ss);
	}
	return TRUE;
}
void CLayAngleBoltDlg::OnSelchangeCmbType() 
{
	UpdateData();
	if(m_iType==3)
	{
		CReportVectorDlg dlg;
#ifdef AFX_TARG_ENU_ENGLISH
		dlg.m_sCaption="Input offset normal";
#else 
		dlg.m_sCaption="����ƽ���淨��";
#endif
		dlg.m_fVectorX = face_norm_offset.x;
		dlg.m_fVectorY = face_norm_offset.y;
		dlg.m_fVectorZ = face_norm_offset.z;
		if(dlg.DoModal()==IDOK)
		{
			face_norm_offset.x = dlg.m_fVectorX;
			face_norm_offset.y = dlg.m_fVectorY;
			face_norm_offset.z = dlg.m_fVectorZ;
		}
	}
	else if(m_iType==0)
	{
		face_norm_offset.x = 1;
		face_norm_offset.y = 0;
		face_norm_offset.z = 0;
	}
	else if(m_iType==1)
	{
		face_norm_offset.x = 0;
		face_norm_offset.y = 1;
		face_norm_offset.z = 0;
	}
	else
	{
		face_norm_offset.x = 0;
		face_norm_offset.y = 0;
		face_norm_offset.z = 1;
	}
}

void CLayAngleBoltDlg::ContextMenu(CPoint point)
{
	CPoint scr_point = point;
	ClientToScreen(&scr_point);
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	if(m_listOddRefRods.GetSelectedCount()==0&&m_listOddRefRods.GetItemCount()<2)
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"Add");
#else 
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"���");
#endif
	else if(m_listOddRefRods.GetSelectedCount()==1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Edit");
#else 
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"�༭");
#endif
		pMenu->AppendMenu(MF_SEPARATOR,ID_SEPARATOR,"");
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete");
#else 
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ��");
#endif
	}
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}
void CLayAngleBoltDlg::OnAddItem()
{
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	m_dwSelectPartsType=GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_LINETUBE);
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCmdPickPrompt="Please select a member to add negative collision reference rod:";
#else 
	m_sCmdPickPrompt="��ѡ�����ӵĸ˼��ĸ�ͷ��ײ���ո˼�:";
#endif
	m_iPickObjType=1;	//�����ײ���ո˼�
	CDialog::OnOK();
}
void CLayAngleBoltDlg::OnModifyItem()
{
	m_iCurSelOddRefRod=m_listOddRefRods.GetSelectedItem();
	if(m_iCurSelOddRefRod<0)
		return;
	char tem_str[101];
	m_listOddRefRods.GetItemText(m_iCurSelOddRefRod,0,tem_str,100);
	m_hCurEditOddPart=HexStrToLong(tem_str);
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	m_dwSelectPartsType=GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_LINETUBE);
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCmdPickPrompt="Please select the negative collision reference rod of current rod again :";
#else 
	m_sCmdPickPrompt="������ѡ��ǰ�˼��ĸ�ͷ��ײ���ո˼�:";
#endif
	m_iPickObjType=2;	//����ѡ����ײ���ո˼�
	CDialog::OnOK();
}

void CLayAngleBoltDlg::OnKeydownListOddRefRods(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if(pLVKeyDow->wVKey==VK_DELETE)
		OnDeleteItem();
	*pResult = 0;
}
void CLayAngleBoltDlg::OnDeleteItem()
{
	int iCurSel=m_listOddRefRods.GetSelectedItem();
	if(iCurSel>=0)
	{
		char tem_str[101];
		m_listOddRefRods.GetItemText(iCurSel,0,tem_str,100);
		long hCurPart=HexStrToLong(tem_str);
		if(m_pLinePart->pStart==m_pNode)
		{
			if(m_pLinePart->desStartOdd.m_hRefPart1==hCurPart)
				m_pLinePart->desStartOdd.m_hRefPart1=0;
			else if(m_pLinePart->desStartOdd.m_hRefPart2==hCurPart)
				m_pLinePart->desStartOdd.m_hRefPart2=0;
			m_pLinePart->CalStartOddment();
			m_fOddment=m_pLinePart->startOdd();
		}
		else
		{
			if(m_pLinePart->desEndOdd.m_hRefPart1==hCurPart)
				m_pLinePart->desEndOdd.m_hRefPart1=0;
			else if(m_pLinePart->desEndOdd.m_hRefPart2==hCurPart)
				m_pLinePart->desEndOdd.m_hRefPart2=0;
			m_pLinePart->CalEndOddment();
			m_fOddment=m_pLinePart->endOdd();
		}
		UpdateData(FALSE);
		m_listOddRefRods.DeleteItem(iCurSel);
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pLinePart->handle);
		return;
	}
}

void CLayAngleBoltDlg::OnItemchangedListOddRefRods(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int iCurSel=m_listOddRefRods.GetSelectedItem();
	if(iCurSel<0)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pLinePart->handle);
		return;
	}
	char tem_str[101];
	m_listOddRefRods.GetItemText(iCurSel,0,tem_str,100);
	long hCurPart=HexStrToLong(tem_str);
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(hCurPart);
	*pResult = 0;
}


void CLayAngleBoltDlg::OnDblclkListOddRefRods(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	OnModifyItem();
	*pResult = 0;
}

void CLayAngleBoltDlg::OnRclickListOddRefRods(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	CRect rc;
	CWnd *pWnd = GetDlgItem(IDC_LIST_ODD_REF_RODS);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	CPoint mousepos(rc.left+pNMItemActivate->ptAction.x,rc.top+pNMItemActivate->ptAction.y);
	//if(rc.PtInRect(pNMItemActivate->ptAction))
		ContextMenu(mousepos);
	*pResult = 0;
}

void CLayAngleBoltDlg::OnChangeELsNum()
{
#ifdef __SMART_DRAW_INC_
	UpdateData();
	if(m_nLsNum>=5)
		m_iBoltRows=1;
	else
		m_iBoltRows=0;
	UpdateData(FALSE);
	OnSelchangeCmbLsRows();
#endif
}

//0.ָ���߾�;1.��׼�߾�;2.��������;3.��������-��
void CLayAngleBoltDlg::OnCbnSelchangeBerSpaceMode()
{
	UpdateData(TRUE);
	IntelliUpdateBoltSpace();
	PREV_BERSPACE_MODE=m_iBerSpaceMode;
	UpdateData(FALSE);
}

void CLayAngleBoltDlg::OnCbnSelchangeWingSpaceMode()
{
	UpdateData(TRUE);
	IntelliUpdateBoltSpace();
	PREV_WINGSPACE_MODE=m_iWingSpaceMode;
	UpdateData(FALSE);
}

void  CLayAngleBoltDlg::OnChangeWingSpace()
{
	UpdateData(TRUE);
    m_iWingSpaceMode=0;
    UpdateData(FALSE);
}

void CLayAngleBoltDlg::OnChangeBerSpace()
{
	UpdateData(TRUE);
	m_iBerSpaceMode=0;
	UpdateData(FALSE);
}

void CLayAngleBoltDlg::PreviewBolt(CDesignLsPara *pLsPara)
{
	if(pLsPara->pMapLs==NULL)
	{	//����plsPara��Ӧ�Ĺ�����˨
		for(CLDSBolt *pBolt=hashTempPrevViewBolts.GetFirst();pBolt;pBolt=hashTempPrevViewBolts.GetNext())
		{
			if(pBolt->feature==0)
			{
				pLsPara->pMapLs=pBolt;
				pBolt->feature=(long)pLsPara;
				break;
			}
		}
		if(pLsPara->pMapLs==NULL)
		{
			pLsPara->pMapLs=hashTempPrevViewBolts.Add(0);
			pLsPara->pMapLs->handle=hashTempPrevViewBolts.GetCursorKey();
			pLsPara->pMapLs->feature=(long)pLsPara;
		}
	}
	if(pLsPara->pMapLs&&hashTempPrevViewBolts.GetValue(pLsPara->pMapLs->handle))
	{
		pLsPara->DesignLs(pLsPara->pMapLs);
		pLsPara->pMapLs->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pLsPara->pMapLs->GetSolidPartObject());
		g_pSolidDraw->Draw();
	}
}
void CLayAngleBoltDlg::OnBnSelBaseNode()
{
	UpdateData();
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	m_dwSelectPartsType=GetSingleWord(SELECTINDEX_NODE);
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCmdPickPrompt="Please select locate node";
#else 
	m_sCmdPickPrompt="��ѡ��λ�ڵ� ";
#endif
	m_iPickObjType=3; //3����ڵ㣬�պ���lds_part�� 3����CLS_NODE
					  //�����ѡ��ֻ����CLS_NODE ����ѡ��ڵ�
	CDialog::OnOK();
}
//���ñ�׼�߾�ʱ������ber_space��wing_space,end_space
void CLayAngleBoltDlg::OnBnClickedChkStdEdgeSpace()
{	
	UpdateData();
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,atoi(m_sBoltSize));
	if(m_bUseSdtEdgeSpace)
	{
		m_iBerSpaceMode=1;
		m_iWingSpaceMode=1;
		m_fBerSpace=LsSpace.EndSpace;
		m_fWingSpace=LsSpace.EndSpace;
		m_fPlateEdgeSpace=LsSpace.EndSpace;
	}
	UpdateData(FALSE);
}
//
void CLayAngleBoltDlg::OnClickedChkPunchHole()
{
	UpdateData();
	m_bDrillHole = !m_bPunchHole;
	UpdateData(FALSE);
}
void CLayAngleBoltDlg::OnClickedChkDrillHole()
{
	UpdateData();
	m_bPunchHole = !m_bDrillHole;
	UpdateData(FALSE);
}
void CLayAngleBoltDlg::WorkPlaneSketch()
{
	DelWorkPlaneSketch();
	GEPOINT base_pos, vFrontDirection;
	BYTE direction = 0;
	if (hashTempPrevViewBolts.GetNodeNum() == 0)
	{
		CDesignLsPara ls_para;
		InitDesignLsPara(ls_para);
		ls_para.des_base_pos.datumPoint.UpdatePos(m_pLinePart->BelongModel());
		base_pos = ls_para.des_base_pos.datumPoint.Position();
		ls_para.des_work_norm.UpdateVector(m_pLinePart->BelongModel());
		vFrontDirection = ls_para.des_work_norm.vector;
		direction = ls_para.des_base_pos.direction;
	}
	else
	{
		CLDSBolt* pBolt = hashTempPrevViewBolts.GetFirst();
		vFrontDirection = g_pSolidOper->TransVFromGL(GEPOINT(0, 0, 1));
		base_pos = pBolt->ucs.origin;
		if (vFrontDirection*pBolt->ucs.axis_z < 0)
			vFrontDirection = -pBolt->ucs.axis_z;
		else
			vFrontDirection = pBolt->ucs.axis_z;
		base_pos += vFrontDirection * pBolt->get_L();
		direction = pBolt->des_base_pos.direction;
	}
	m_xLineVec = m_pLinePart->End() - m_pLinePart->Start();
	if (direction == 1)
		m_xLineVec = m_pLinePart->Start() - m_pLinePart->End();
	normalize(m_xLineVec);
	if (m_pLinePart->GetClassTypeId() == CLS_LINEANGLE)
	{
		CLDSLineAngle* pAngle = (CLDSLineAngle*)m_pLinePart;
		m_xWingOffsetVec = (m_iOffsetWing == 0) ? pAngle->GetWingVecX() : pAngle->GetWingVecY();
	}
	else if (m_pLinePart->GetClassTypeId() == CLS_LINESLOT)
	{
		CLDSLineSlot* pSlot = (CLDSLineSlot*)m_pLinePart;
		if (m_iOffsetWing > 0)
			m_xWingOffsetVec = m_pLinePart->ucs.axis_y;
		else
			m_xWingOffsetVec = m_pLinePart->ucs.axis_x;
	}
	else if (m_pLinePart->GetClassTypeId() == CLS_LINEFLAT)
		m_xWingOffsetVec = m_pLinePart->ucs.axis_y;
	btc::SKETCH_PLANE sketch(base_pos, m_xLineVec, vFrontDirection);
	g_pSolidDraw->NewWorkPlaneSketch(1, RGB(152, 152, 152), sketch.pVertexArr, sketch.VertexCount, sketch.normal, "��L");
	g_pSolidDraw->NewWorkPlaneSketch(2, RGB(152, 152, 152), &base_pos, 1, m_xWingOffsetVec, "��G", 75);
	g_pSolidDraw->Draw();
}
void CLayAngleBoltDlg::DelWorkPlaneSketch()
{
	if (!m_xLineVec.IsZero())
		g_pSolidDraw->DelWorkPlaneSketch(1);
	if (!m_xWingOffsetVec.IsZero())
		g_pSolidDraw->DelWorkPlaneSketch(2);
	g_pSolidDraw->Draw();
}
#endif

