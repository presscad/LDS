// DesignTubeRibPlateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignTubeRibPlateDlg.h"
#include "GlobalFunc.h"
#include "Query.h"
#include "PartsDbDlg.h"
#include "LicFuncDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CDesignTubeRibPlateDlg dialog


CDesignTubeRibPlateDlg::CDesignTubeRibPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignTubeRibPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignTubeRibPlateDlg)
	m_fParamW = 200.0;
	m_fParamH = 60.0;
	m_fParamR = 200.0;
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = 0.0;
	m_sPartNo = _T("");
	m_sSegI = _T("1");
	m_iThick = 10;
	m_iMaterial = 0;
	m_fNormOffset = 0.0;
	m_bMirCreate = FALSE;
	m_fParamN = 200.0;
	m_bDesignOuterPlate = TRUE;
	m_bCirPlate = FALSE;
	m_bTopArcConnection=FALSE;
	//}}AFX_DATA_INIT
	m_bModifyZ = TRUE;
	m_pDatumNode = NULL;
	m_pDatumTube = NULL;
	m_pDatumPlate1 = NULL;
	m_pDatumPart2 = NULL;
	m_pSetUpPart = NULL;
}


void CDesignTubeRibPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignTubeRibPlateDlg)
	DDX_Text(pDX, IDC_E_PARAM_W, m_fParamW);
	DDX_Text(pDX, IDC_E_PARAM_H, m_fParamH);
	DDX_Text(pDX, IDC_E_PARAM_R, m_fParamR);
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_SEG_I, m_sSegI);
	DDX_Text(pDX, IDC_E_THICK, m_iThick);
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_NORM_OFFSET, m_fNormOffset);
	DDX_Check(pDX, IDC_CHK_MIR_CREATE, m_bMirCreate);
	DDX_Text(pDX, IDC_E_PARAM_N, m_fParamN);
	DDX_Check(pDX, IDC_CHK_DESIGN_OUTER_PLATE, m_bDesignOuterPlate);
	DDX_Check(pDX, IDC_CHK_CIR_PLATE, m_bCirPlate);
	DDX_Check(pDX, IDC_CHK_TOP_ARC_CONNECTION, m_bTopArcConnection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignTubeRibPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignTubeRibPlateDlg)
	ON_BN_CLICKED(IDC_BN_PARTS_DB, OnBnPartsDb)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangePartNo)
	ON_EN_CHANGE(IDC_E_NORM_OFFSET, OnChangeNormOffset)
	ON_BN_CLICKED(IDC_CHK_DESIGN_OUTER_PLATE, OnDesignOuterPlate)
	ON_BN_CLICKED(IDC_CHK_CIR_PLATE, OnCirPlate)
	ON_BN_CLICKED(IDC_CHK_TOP_ARC_CONNECTION, OnTopArcConnection)
	ON_EN_KILLFOCUS(IDC_E_PARAM_H, OnKillfocusParamH)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignTubeRibPlateDlg message handlers

void CDesignTubeRibPlateDlg::OnChangePartNo() 
{
	UpdateData();
	SEGI iSeg;
	if(ParsePartNo(m_sPartNo.GetBuffer(),&iSeg,NULL))
		m_sSegI=iSeg.ToString();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNo,BOMPART::PLATE);
	if(pPart)
	{
		m_iMaterial=(int)pPart->thick;
		m_iMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
	}
#endif
	UpdateData(FALSE);
}

void CDesignTubeRibPlateDlg::OnBnPartsDb() 
{
	double Y=0;
	CPartsDbDlg partsDlg;
	partsDlg.m_bRefreshDlgCtrl = TRUE;
	partsDlg.m_bIncParamCirRib=TRUE;
	if(partsDlg.DoModal()==IDOK)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)partsDlg.m_pSelectPart;
		if(pParamPlate&&pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
		{
			pParamPlate->GetDesignItemValue('N', &m_fParamN);
			pParamPlate->GetDesignItemValue('H', &m_fParamH);
			pParamPlate->GetDesignItemValue('W', &m_fParamW);
			pParamPlate->GetDesignItemValue('R', &m_fParamR);
			pParamPlate->GetDesignItemValue('D', &m_fNormOffset);
			pParamPlate->GetDesignItemValue('Y', &Y);
			if(Y>=0)
				m_bDesignOuterPlate=TRUE;
			else
				m_bDesignOuterPlate=FALSE;
			m_iThick=pParamPlate->thick;
			m_sPartNo=pParamPlate->GetPartNo();
			m_sSegI=pParamPlate->iSeg.ToString();
			m_iMaterial=QuerySteelMatIndex(pParamPlate->cMaterial);
			UpdateData(FALSE);
		}
	}
}

BOOL CDesignTubeRibPlateDlg::OnInitDialog() 
{
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MATERIAL);
	AddSteelMatRecord(pCMB);
	GetDlgItem(IDC_E_NORM_X)->EnableWindow(m_bModifyZ);
	GetDlgItem(IDC_E_NORM_Y)->EnableWindow(m_bModifyZ);
	GetDlgItem(IDC_E_NORM_Z)->EnableWindow(m_bModifyZ);
	if(!m_pDatumTube||!m_pDatumNode||!m_pDatumPlate1)
		return FALSE;
	m_fParamR=m_pDatumTube->GetDiameter()*0.5+60;	//默认值为半径为钢管半径+60
	if(m_pDatumPart2==NULL&&m_pSetUpPart==NULL)
	{
		m_bCirPlate=TRUE;
		m_fParamW=0;
	}
	GetDlgItem(IDC_CHK_DESIGN_OUTER_PLATE)->EnableWindow(!m_bCirPlate);
	GetDlgItem(IDC_CHK_MIR_CREATE)->EnableWindow(!m_bCirPlate);
	GetDlgItem(IDC_CHK_TOP_ARC_CONNECTION)->EnableWindow(!m_bCirPlate);
	GetDlgItem(IDC_E_PARAM_R)->EnableWindow(m_bTopArcConnection);//顶部为圆弧连接
	f3dPoint axis_z=m_pDatumTube->Start()-m_pDatumTube->End();
	normalize(axis_z);
	m_fNormX=axis_z.x;
	m_fNormY=axis_z.y;
	m_fNormZ=axis_z.z;
	CDialog::OnInitDialog();
	AutoCalWN();
	return TRUE;
}

BOOL CDesignTubeRibPlateDlg::AutoCalWN()
{
	/*钢管肋板共有四种情况：
	  1.只有一块基准钢板且未指定定位X轴方向的基准杆件(为环向板) Y轴与钢板Y轴同向 基准钢板位于X轴正方向一侧;
	  2.只有一块基准钢板且该钢板从基准钢管中穿过 指定了定位X轴方向的基准杆件 Y轴与钢板Y轴同向;
	  3.有两块基准钢板并且指定为环向板 Y轴与第一块钢板Y轴同向 基准钢板位于X轴正方向一侧;
	  4.有两块基准钢板未指定为环向板 Y轴与第一块钢板Y轴同向(指外侧板);
	*/
	UpdateData();
	UCS_STRU ucs;	//假定的肋板坐标系
	ucs.axis_z.Set(m_fNormX,m_fNormY,m_fNormZ);
	normalize(ucs.axis_z);
	//计算钢板Ｙ轴方向
	if(m_bDesignOuterPlate)
		ucs.axis_y=m_pDatumPlate1->ucs.axis_z;
	else
		ucs.axis_y=-m_pDatumPlate1->ucs.axis_z;
	if(m_pSetUpPart&&m_pSetUpPart->IsLinePart())
	{	//指定Ｘ轴定位方向基准杆件
			//计算钢板Ｘ轴方向
		ucs.axis_x=m_pSetUpPart->End()-m_pSetUpPart->Start();//由基准杆件确定装备坐标系X轴方向
			//计算钢板Ｚ轴方向
		ucs.axis_z=ucs.axis_x^ucs.axis_y;
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		normalize(ucs.axis_z);
	} 
	else
	{	//未指定Ｘ轴定位方向基准杆件
			//计算钢板Ｚ轴方向（精确）
		f3dPoint axis_z=m_pDatumTube->End()-m_pDatumTube->Start();
		normalize(axis_z);
		if(ucs.axis_z*axis_z>0)
			ucs.axis_z=axis_z;
		else
			ucs.axis_z=-axis_z;
			//计算钢板Ｘ轴方向
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
	}
	if(m_pDatumNode&&m_pDatumTube)
		SnapPerp(&ucs.origin,m_pDatumTube->Start(),m_pDatumTube->End(),m_pDatumNode->Position(true));
	ucs.origin+=ucs.axis_z*m_fNormOffset;
	CLDSPlate *pDatumPlate2=NULL;
	if(m_pDatumPart2&&m_pDatumPart2->GetClassTypeId()==CLS_PLATE)
		pDatumPlate2=(CLDSPlate*)m_pDatumPart2;
	//修正射线方向的正负号
	GEPOINT vertice,vertice_vec;
	GEPOINT axis_x_1,axis_x_2;//基准钢板在钢管垂直截面内的射线方向
	if(m_pDatumPlate1)
	{
		axis_x_1 = ucs.axis_z^m_pDatumPlate1->ucs.axis_z;
		normalize(axis_x_1);
		if(m_pDatumPlate1->vertex_list.GetNodeNum()>0)
		{
			vertice=m_pDatumPlate1->vertex_list.GetFirst()->vertex;
			coord_trans(vertice,m_pDatumPlate1->ucs,TRUE);
			vertice_vec=vertice-ucs.origin;
			if(vertice_vec*axis_x_1<0)
				axis_x_1*=-1.0;
		}
	}
	if(pDatumPlate2)
	{
		axis_x_2 = ucs.axis_z^pDatumPlate2->ucs.axis_z;
		normalize(axis_x_2);
		if(pDatumPlate2->vertex_list.GetNodeNum()>0)
		{
			vertice=pDatumPlate2->vertex_list.GetFirst()->vertex;
			coord_trans(vertice,pDatumPlate2->ucs,TRUE);
			vertice_vec=vertice-ucs.origin;
			if(vertice_vec*axis_x_2<0)
				axis_x_2*=-1.0;
		}
	}
	double cosa=1;
	if(!pDatumPlate2&&m_pSetUpPart)
	{
		f3dPoint datum_tube_vec=m_pDatumTube->pEnd->Position(true)-m_pDatumTube->pStart->Position(true);
		normalize(datum_tube_vec);
		cosa=datum_tube_vec*ucs.axis_x;
		cosa=sqrt(1-cosa*cosa);
	}
	f3dPoint inter_pos,inter_pos_1,base_pos;
	base_pos=ucs.origin;
	PROFILE_VERTEX *pPreVertex=NULL;
	double temp_dist=0;
	if(m_pDatumPlate1)
	{
		pPreVertex=m_pDatumPlate1->vertex_list.GetTail(); //钢板的第一个顶点的前一个顶点为最后一个顶点
		for(PROFILE_VERTEX *pVertex=m_pDatumPlate1->vertex_list.GetFirst();pVertex;pVertex=m_pDatumPlate1->vertex_list.GetNext())
		{
			if(pPreVertex&&pPreVertex!=pVertex)
			{
				f3dLine line(pPreVertex->vertex,pVertex->vertex);
				coord_trans(line.startPt,m_pDatumPlate1->ucs,TRUE);
				coord_trans(line.endPt,m_pDatumPlate1->ucs,TRUE);
				project_point(base_pos,line.startPt,m_pDatumPlate1->ucs.axis_z);
				int ret=Int3dll(line.startPt,line.endPt,base_pos,base_pos+axis_x_1*10000,inter_pos);
				if(ret>0)
				{
					double dist=DISTANCE(inter_pos,ucs.origin);
					if(dist>m_pDatumTube->GetDiameter()*0.5)
					{
						if(!pDatumPlate2&&m_pSetUpPart)
						{
							if(line.startPt.x>0)
								m_fParamN=dist-(m_pDatumTube->GetDiameter()*0.5)/cosa;
							else
								m_fParamW=dist-(m_pDatumTube->GetDiameter()*0.5)/cosa;
						}
						else
						{
							if(dist>temp_dist)
							{
								m_fParamN=dist-m_pDatumTube->GetDiameter()*0.5;
								temp_dist=dist;
							}
						}
					}
				}
				if(!pDatumPlate2&&m_pSetUpPart)
				{
					int ret=Int3dll(line.startPt,line.endPt,base_pos,base_pos-axis_x_1*10000,inter_pos);
					if(ret>0)
					{
						double dist=DISTANCE(inter_pos,ucs.origin);
						if(dist>m_pDatumTube->GetDiameter()*0.5)
						{
							if(line.startPt.x>0)
								m_fParamN=dist-(m_pDatumTube->GetDiameter()*0.5)/cosa;
							else
								m_fParamW=dist-(m_pDatumTube->GetDiameter()*0.5)/cosa;
						}
					}
				}
				pPreVertex=pVertex;
			}
		}
	}
	if(pDatumPlate2)
	{
		temp_dist=0;
		pPreVertex=pDatumPlate2->vertex_list.GetTail(); //钢板的第一个顶点的前一个顶点为最后一个顶点
		for(PROFILE_VERTEX *pVertex=pDatumPlate2->vertex_list.GetFirst();pVertex;pVertex=pDatumPlate2->vertex_list.GetNext())
		{
			if(pPreVertex&&pPreVertex!=pVertex)
			{
				f3dLine line(pPreVertex->vertex,pVertex->vertex);
				coord_trans(line.startPt,pDatumPlate2->ucs,TRUE);
				coord_trans(line.endPt,pDatumPlate2->ucs,TRUE);
				project_point(base_pos,line.startPt,pDatumPlate2->ucs.axis_z);
				int ret=Int3dll(line.startPt,line.endPt,base_pos,base_pos+axis_x_2*10000,inter_pos_1);
				if(ret>0)
				{
					double dist=DISTANCE(inter_pos_1,ucs.origin);
					if(dist>m_pDatumTube->GetDiameter()*0.5)
					{
						if(dist>temp_dist)
						{
							m_fParamW=dist-m_pDatumTube->GetDiameter()*0.5;
							temp_dist=dist;
						}
					}
				}
				pPreVertex=pVertex;
			}
		}
	}
	//不为环向板且第二块基准不为空
	if(!m_bCirPlate&&pDatumPlate2&&axis_x_1*ucs.axis_x<0)
	{	//只有情况4即有两块基准钢板且不为环向板时才可能运行至此处
		double tempW=m_fParamW;
		m_fParamW=m_fParamN;
		m_fParamN=tempW;
	}
	UpdateData(FALSE);
	return TRUE;
}

void CDesignTubeRibPlateDlg::OnDesignOuterPlate() 
{
	UpdateData();
	if(m_pDatumPlate1&&m_pDatumPart2)
	{	//只有一块基准钢板时不需要交换W、N的值
		double tempW=m_fParamW;
		m_fParamW=m_fParamN;
		m_fParamN=tempW;
	}
	UpdateData(FALSE);
}

void CDesignTubeRibPlateDlg::OnChangeNormOffset() 
{
	CString strValue;
	GetDlgItemText(IDC_E_NORM_OFFSET,strValue);
	if(strValue.GetLength()==1&&strValue.CompareNoCase("-")==0)
		return;
	UpdateData();
	AutoCalWN();
	UpdateData(FALSE);
}

void CDesignTubeRibPlateDlg::OnCirPlate() 
{
	UpdateData();
	GetDlgItem(IDC_CHK_DESIGN_OUTER_PLATE)->EnableWindow(!m_bCirPlate);
	GetDlgItem(IDC_CHK_MIR_CREATE)->EnableWindow(!m_bCirPlate);
	GetDlgItem(IDC_CHK_TOP_ARC_CONNECTION)->EnableWindow(!m_bCirPlate);
	if(m_bCirPlate)
		GetDlgItem(IDC_E_PARAM_R)->EnableWindow(TRUE);//顶部为圆弧连接
	else
		GetDlgItem(IDC_E_PARAM_R)->EnableWindow(m_bTopArcConnection);//顶部为圆弧连接
	m_fParamW=0;
	m_fParamN=0;
	AutoCalWN();
	UpdateData(FALSE);
}

void CDesignTubeRibPlateDlg::OnTopArcConnection() 
{
	UpdateData();
	GetDlgItem(IDC_E_PARAM_R)->EnableWindow(m_bTopArcConnection);//顶部为圆弧连接
	m_fParamR=ftol(m_pDatumTube->GetDiameter()*0.5+min(m_fParamN,m_fParamW));
	//顶部圆弧连接且H==0时,无需输入W,N值 wht 10-07-30
	if(fabs(m_fParamH)<EPS&&m_bTopArcConnection)
	{
		GetDlgItem(IDC_E_PARAM_W)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_PARAM_N)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_E_PARAM_W)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_PARAM_N)->EnableWindow(TRUE);
	}
	UpdateData(FALSE);
}

void CDesignTubeRibPlateDlg::OnKillfocusParamH() 
{
	UpdateData();
	//顶部圆弧连接且H==0时,无需输入W,N值 wht 10-07-30
	if(fabs(m_fParamH)<EPS&&m_bTopArcConnection)
	{
		GetDlgItem(IDC_E_PARAM_W)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_PARAM_N)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_E_PARAM_W)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_PARAM_N)->EnableWindow(TRUE);
	}
	UpdateData(FALSE);
}
#endif



