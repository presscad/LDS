// AddCircularPlateDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "AddCircularPlateDlg.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "Query.h"
#include "PartsDbDlg.h"
#include "lds_part.h"
#include "LicFuncDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef __ANGLE_PART_INC_

/////////////////////////////////////////////////////////////////////////////
// CAddCircularPlateDlg dialog


CAddCircularPlateDlg::CAddCircularPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddCircularPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddCircularPlateDlg)
	m_iMaterial = 1;
	m_fParaD = 150.0;
	m_fParaW = 200.0;
	m_fPlateT = 10.0;
	m_sPartNo = _T("");
	m_sSegI = _T("1");
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fParaS = 0.0;
	m_fNormZ = 0.0;
	m_fParaH = 50.0;
	m_bUseH = FALSE;
	m_fParaN = 200.0;
	m_bMirCreate = FALSE;
	m_bDesignOuterPlate = TRUE;
	m_bCir = FALSE;
	//}}AFX_DATA_INIT
	m_pDatumNode = NULL;
	m_pDatumTube = NULL;
	m_pDatumPlate1 = NULL;
	m_pDatumPlate2 = NULL;
	m_pDatumSphere = NULL;
}


void CAddCircularPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddCircularPlateDlg)
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_PARA_D, m_fParaD);
	DDX_Text(pDX, IDC_E_PARA_W, m_fParaW);
	DDX_Text(pDX, IDC_E_PLATE_T, m_fPlateT);
	DDX_Text(pDX, IDC_E_PLATE_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_PLATE_SEG_I, m_sSegI);
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_PLATE_S, m_fParaS);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	DDX_Text(pDX, IDC_E_PARAM_H, m_fParaH);
	DDX_Check(pDX, IDC_CHK_USE_H, m_bUseH);
	DDX_Text(pDX, IDC_E_PARA_N, m_fParaN);
	DDX_Check(pDX, IDC_CHK_MIR_CREATE, m_bMirCreate);
	DDX_Check(pDX, IDC_CHK_DESIGN_OUTER_PLATE, m_bDesignOuterPlate);
	DDX_Check(pDX, IDC_CHK_CIR, m_bCir);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddCircularPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CAddCircularPlateDlg)
	ON_BN_CLICKED(IDC_BN_PARTS_DB, OnBnPartsDb)
	ON_EN_CHANGE(IDC_E_PLATE_PART_NO, OnChangeEPlatePartNo)
	ON_EN_CHANGE(IDC_E_PLATE_S, OnChangeS)
	ON_BN_CLICKED(IDC_CHK_CIR, OnCir)
	ON_BN_CLICKED(IDC_BN_PASTE_NORM, OnPasteNorm)
	ON_EN_CHANGE(IDC_E_PARA_W, OnChangeParaW)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHK_USE_H, OnBnChkUseH)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddCircularPlateDlg message handlers

void CAddCircularPlateDlg::OnBnPartsDb() 
{
	CPartsDbDlg partsDlg;
	partsDlg.m_bRefreshDlgCtrl = TRUE;
	partsDlg.m_bIncParamCirRib=TRUE;
	if(partsDlg.DoModal()==IDOK)
	{
		CSuperSmartPtr<CLDSPart>pParamPlate=partsDlg.m_pSelectPart;
		if(pParamPlate.IsHasPtr()&&pParamPlate.ParamPlatePointer()->m_iParamType==TYPE_CIRRIBPLATE)
		{
			pParamPlate.ParamPlatePointer()->GetDesignItemValue('W', &m_fParaW);
			pParamPlate.ParamPlatePointer()->GetDesignItemValue('D', &m_fParaD);
			pParamPlate.ParamPlatePointer()->GetDesignItemValue('S', &m_fParaS);
			pParamPlate.ParamPlatePointer()->GetDesignItemValue('H', &m_fParaH);
			if(m_pDatumPlate2)
			{
				long O=-1;	//顶部圆弧连接
				pParamPlate.ParamPlatePointer()->GetDesignItemValue('N', &m_fParaN);
				if(m_fParaH<0)
					m_bUseH=FALSE;
				else if(m_fParaD<0)
					m_bUseH=TRUE;
				pParamPlate.ParamPlatePointer()->GetDesignItemValue('O', &O);
				if(O>0)
					m_bCir=TRUE;
				else
					m_bCir=FALSE;
			}
			else
			{
				double Y=0;
				pParamPlate.ParamPlatePointer()->GetDesignItemValue('Y',&Y);
				if(Y>=0)
					m_bDesignOuterPlate = TRUE;
				else
					m_bDesignOuterPlate = FALSE;
			}
			m_fPlateT=pParamPlate.ParamPlatePointer()->thick;
			m_sPartNo=pParamPlate.ParamPlatePointer()->GetPartNo();
			m_sSegI=pParamPlate.ParamPlatePointer()->iSeg.ToString();
			m_iMaterial=QuerySteelMatIndex(pParamPlate.ParamPlatePointer()->cMaterial);
			UpdateData(FALSE);
		}
	}
	if(m_pDatumPlate2)
	{
		GetDlgItem(IDC_CHK_CIR)->EnableWindow(TRUE);	//允许启用顶部连接
		GetDlgItem(IDC_CHK_USE_H)->EnableWindow(!m_bCir);
		GetDlgItem(IDC_E_PARA_N)->EnableWindow(!m_bCir);
		GetDlgItem(IDC_E_PARA_D)->EnableWindow(!m_bCir&&!m_bUseH);
		GetDlgItem(IDC_E_PARAM_H)->EnableWindow(!m_bCir&&m_bUseH);
		GetDlgItem(IDC_CHK_MIR_CREATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_DESIGN_OUTER_PLATE)->EnableWindow(FALSE);
	}
	else
	{
		m_bCir=FALSE;
		GetDlgItem(IDC_CHK_CIR)->EnableWindow(FALSE);	//不能启用顶部连接
		GetDlgItem(IDC_E_PARA_N)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_USE_H)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_MIR_CREATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_DESIGN_OUTER_PLATE)->EnableWindow(TRUE);
	}
	OnCir();	//根据是否顶部圆弧连接更新示意图
}

void CAddCircularPlateDlg::OnOK() 
{
	UpdateData();
	double r=0;
	if(m_pDatumTube)
		r=m_pDatumTube->GetDiameter()*0.5;
	else if(m_pDatumSphere)
		r=m_pDatumSphere->D*0.5;
	//顶部圆弧连接时不需要判断参数D是否大于钢管直径 wht 11-01-21
	if(!m_bCir&&!m_pDatumPlate2&&m_fParaD>r) 
	{
#ifdef  AFX_TARG_ENU_ENGLISH
		MessageBox("Parameter d is larger than the radius of datum tube，input error！");
#else
		MessageBox("参数D大于基准钢管半径，输入错误！");
#endif
	}
	else
		CDialog::OnOK();
}

void CAddCircularPlateDlg::OnChangeEPlatePartNo() 
{
	UpdateData();
	SEGI iSeg;
	if(ParsePartNo(m_sPartNo.GetBuffer(),&iSeg,NULL))
		m_sSegI=iSeg.ToString();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNo);
	if(pPart)
	{	//根据找到的BOM信息，更新该钢板的基本信息(段号、规格、材质)
		if(pPart->cPartType==BOMPART::PLATE)
		{
			m_iMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
			m_fPlateT=pPart->thick;
		}
		//else
		//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	UpdateData(FALSE);
}

BOOL CAddCircularPlateDlg::OnInitDialog() 
{
	if(m_pDatumPlate1==NULL||m_pDatumNode==NULL)
		return FALSE;
	if(m_pDatumPlate2)	//用是否存在第2块基准钢板来判断生成那种环向肋板
	{
		m_bCir=FALSE;
		GetDlgItem(IDC_CHK_CIR)->EnableWindow(TRUE);	//允许启用顶部连接
		GetDlgItem(IDC_CHK_USE_H)->EnableWindow(!m_bCir);
		GetDlgItem(IDC_E_PARA_N)->EnableWindow(!m_bCir);
		GetDlgItem(IDC_E_PARA_D)->EnableWindow(!m_bCir&&!m_bUseH);
		GetDlgItem(IDC_E_PARAM_H)->EnableWindow(!m_bCir&&m_bUseH);
		GetDlgItem(IDC_CHK_MIR_CREATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_DESIGN_OUTER_PLATE)->EnableWindow(FALSE);
		if(m_bUseH)
		{
			m_fParaH=50;
			m_fParaD=-1;
		}
		else
		{
			m_fParaH=-1;
			m_fParaD=150;
		}
	}
	else //if(m_pDautmPlate2==NULL)
	{	//仅有一块基准钢板
		//m_bCir=FALSE;
		//支持单个基准钢板的肋板顶部使用圆弧连接 wht 10-10-26
		//GetDlgItem(IDC_CHK_CIR)->EnableWindow(FALSE);	//不能启用顶部连接
		GetDlgItem(IDC_CHK_USE_H)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_MIR_CREATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_DESIGN_OUTER_PLATE)->EnableWindow(TRUE);
		//一块基准钢板时顶部为圆弧的肋板
		//顶部为圆弧时参数H不可用，m_fParaD保存肋板圆弧角度，非圆弧时参数N不可用
		GetDlgItem(IDC_E_PARA_N)->EnableWindow(m_bCir);
		GetDlgItem(IDC_E_PARAM_H)->EnableWindow(!m_bCir);	
		if(m_bCir)	
		{
			SetDlgItemText(IDC_S_PARA_NAME,"β:");
			m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE4);
		}
		else
		{
			SetDlgItemText(IDC_S_PARA_NAME,"D:");
			m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE);
		}
		if(m_pDatumTube)
			m_fParaD=m_pDatumTube->GetDiameter()*0.5-10;
		else if(m_pDatumSphere)
			m_fParaD=m_pDatumSphere->D*0.5-20;
		m_fParaH=0.5*((int)m_fParaD);
	}
	if(m_pDatumTube)
	{
		m_pDatumTube->BuildUCS();
		m_fNormX=m_pDatumTube->ucs.axis_z.x;
		m_fNormY=m_pDatumTube->ucs.axis_z.y;
		m_fNormZ=m_pDatumTube->ucs.axis_z.z;
	}
	else if(m_pDatumSphere&&m_pDatumPlate1->jdb_style==3)
	{
		
		CLDSLineTube *pLineTube1=NULL,*pLineTube2=NULL;
		for(CDesignLjPartPara *pLjPara=m_pDatumPlate1->designInfo.partList.GetFirst();pLjPara;pLjPara=m_pDatumPlate1->designInfo.partList.GetNext())
		{
			CLDSPart *pPart=console.FromPartHandle(pLjPara->hPart);
			if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE&&pLineTube1==NULL)
				pLineTube1=(CLDSLineTube*)pPart;
			else if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE&&pLineTube1&&pLineTube2==NULL)
				pLineTube2=(CLDSLineTube*)pPart;
		}
		if(pLineTube1&&pLineTube2)
		{
			f3dPoint axis_x_1,axis_x_2,mid_vec;
			if(pLineTube1->pStart->handle==m_pDatumSphere->hDatumNode)
				axis_x_1=pLineTube1->pEnd->Position(true)-pLineTube1->pStart->Position(true);
			else 
				axis_x_1=pLineTube1->pStart->Position(true)-pLineTube1->pEnd->Position(true);
			if(pLineTube2->pStart->handle==m_pDatumSphere->hDatumNode)
				axis_x_2=pLineTube2->pEnd->Position(true)-pLineTube2->pStart->Position(true);
			else 
				axis_x_2=pLineTube2->pStart->Position(true)-pLineTube2->pEnd->Position(true);
			normalize(axis_x_1);
			normalize(axis_x_2);
			mid_vec=axis_x_1+axis_x_2;
			normalize(mid_vec);
			f3dPoint axis_z=mid_vec^m_pDatumPlate1->ucs.axis_z;
			normalize(axis_z);
			m_fNormX=axis_z.x;
			m_fNormY=axis_z.y;
			m_fNormZ=axis_z.z;
		}
		else if(pLineTube1&&pLineTube2==NULL)
		{
			pLineTube1->BuildUCS();
			m_fNormX=pLineTube1->ucs.axis_z.x;
			m_fNormY=pLineTube1->ucs.axis_z.y;
			m_fNormZ=pLineTube1->ucs.axis_z.z;
		}
		else
			return FALSE;
	}
	AutoCalWN();
	CDialog::OnInitDialog();	
	return TRUE;
}

void CAddCircularPlateDlg::OnBnChkUseH()
{
	UpdateData();
	GetDlgItem(IDC_E_PARA_D)->EnableWindow(!m_bUseH);
	GetDlgItem(IDC_E_PARAM_H)->EnableWindow(m_bUseH);
	if(m_bUseH)
	{
		m_fParaH=50;
		m_fParaD=-1;
	}
	else
	{
		m_fParaH=-1;
		m_fParaD=150;
	}
	UpdateData(FALSE);
}

BOOL CAddCircularPlateDlg::AutoCalWN()
{
	if(m_bUseH)
		return FALSE;
	UCS_STRU ucs;	//假定的肋板坐标系
	ucs.axis_z.Set(m_fNormX,m_fNormY,m_fNormZ);
	normalize(ucs.axis_z);
	ucs.origin = m_pDatumNode->Position(true);
	if(m_pDatumNode&&m_pDatumTube)
	{
		m_pDatumTube->BuildUCS();
		SnapPerp(&ucs.origin,m_pDatumTube->Start(),m_pDatumTube->End(),m_pDatumNode->Position(true));
		ucs.origin+=ucs.axis_z*m_fParaS;
	}
	//基准钢板在钢管垂直截面内的射线方向
	f3dPoint axis_x_1,axis_x_2; 
	axis_x_1=ucs.axis_z^m_pDatumPlate1->ucs.axis_z;
	normalize(axis_x_1);
	if(m_pDatumPlate2)
	{
		axis_x_2=ucs.axis_z^m_pDatumPlate2->ucs.axis_z;
		normalize(axis_x_2);
	}
	//修正射线方向的正负号
	GEPOINT vertice,vertice_vec;
	if(m_pDatumPlate1&&m_pDatumPlate1->vertex_list.GetNodeNum()>0)
	{
		vertice=m_pDatumPlate1->vertex_list.GetFirst()->vertex;
		coord_trans(vertice,m_pDatumPlate1->ucs,TRUE);
		vertice_vec=vertice-ucs.origin;
		if(vertice_vec*axis_x_1<0)
			axis_x_1*=-1.0;
	}
	if(m_pDatumPlate2&&m_pDatumPlate2->vertex_list.GetNodeNum()>0)
	{
		vertice=m_pDatumPlate2->vertex_list.GetFirst()->vertex;
		coord_trans(vertice,m_pDatumPlate2->ucs,TRUE);
		vertice_vec=vertice-ucs.origin;
		if(vertice_vec*axis_x_2<0)
			axis_x_2*=-1.0;
	}
	double r=0;
	if(m_pDatumTube)
		r=m_pDatumTube->GetDiameter()*0.5;
	else if(m_pDatumSphere)
	{
		r=m_pDatumSphere->D*0.5;
		double y_offset_dist=m_pDatumPlate1->m_fNormOffset;
		if(m_bDesignOuterPlate)
			y_offset_dist+=m_pDatumPlate1->GetThick();
		r=sqrt(r*r-y_offset_dist*y_offset_dist-m_fParaS*m_fParaS);
	}
	PROFILE_VERTEX *pPreVertex=NULL;
	if(m_pDatumPlate1)
	{	//计算肋板与第一块钢板相交边的长度
		ucs.axis_x = axis_x_1;
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);		
		f3dPoint inter_pos,base_pos;
		base_pos=ucs.origin;
		pPreVertex=m_pDatumPlate1->vertex_list.GetTail(); //钢板的第一个顶点的前一个顶点为最后一个顶点
		for(PROFILE_VERTEX *pVertex=m_pDatumPlate1->vertex_list.GetFirst();pVertex;pVertex=m_pDatumPlate1->vertex_list.GetNext())
		{
			if(pPreVertex&&pPreVertex!=pVertex)
			{
				f3dLine line(pPreVertex->vertex,pVertex->vertex);
				coord_trans(line.startPt,m_pDatumPlate1->ucs,TRUE);
				coord_trans(line.endPt,m_pDatumPlate1->ucs,TRUE);
				project_point(base_pos,line.startPt,m_pDatumPlate1->ucs.axis_z);
				int ret=Int3dll(line.startPt,line.endPt,ucs.origin,ucs.origin+axis_x_1*10000,inter_pos);
				if(ret>0)
				{
					double dist=DISTANCE(inter_pos,ucs.origin);
					if(dist>r)
					{
						m_fParaW=dist-r;
						break;
					}
				}
				pPreVertex=pVertex;
			}
		}
	}
	if(m_pDatumPlate2)
	{	//计算肋板与第二块钢板相交边的长度
		ucs.axis_x = axis_x_2;
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		f3dPoint inter_pos,base_pos;
		base_pos=ucs.origin;
		pPreVertex=m_pDatumPlate2->vertex_list.GetTail(); //钢板的第一个顶点的前一个顶点为最后一个顶点
		for(PROFILE_VERTEX *pVertex=m_pDatumPlate2->vertex_list.GetFirst();pVertex;pVertex=m_pDatumPlate2->vertex_list.GetNext())
		{
			if(pPreVertex&&pPreVertex!=pVertex)
			{
				f3dLine line(pPreVertex->vertex,pVertex->vertex);
				coord_trans(line.startPt,m_pDatumPlate2->ucs,TRUE);
				coord_trans(line.endPt,m_pDatumPlate2->ucs,TRUE);
				project_point(base_pos,line.startPt,m_pDatumPlate2->ucs.axis_z);
				int ret=Int3dll(line.startPt,line.endPt,ucs.origin,ucs.origin+axis_x_2*10000,inter_pos);
				if(ret>0)
				{
					double dist=DISTANCE(inter_pos,ucs.origin);
					if(dist>r)
					{
						m_fParaN=dist-r;
						break;
					}
				}
				pPreVertex=pVertex;
			}
		}
	}
	return TRUE;
}

void CAddCircularPlateDlg::OnChangeS() 
{
	CString sText;
	GetDlgItem(IDC_E_PLATE_S)->GetWindowText(sText);
	if(sText.CompareNoCase("-")==0&&sText.GetLength()==1)
		return;
	UpdateData();
	AutoCalWN();
	UpdateData(FALSE);
}

void CAddCircularPlateDlg::OnCir() 
{
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
	m_CurPlateBmp.Detach();
	UpdateData();
	if(m_pDatumPlate1&&m_pDatumPlate2)
	{	//两块基准钢板时顶部为圆弧的肋板
		GetDlgItem(IDC_CHK_USE_H)->EnableWindow(!m_bCir);
		GetDlgItem(IDC_E_PARA_N)->EnableWindow(!m_bCir);
		GetDlgItem(IDC_E_PARA_D)->EnableWindow(!m_bCir&&!m_bUseH);
		GetDlgItem(IDC_E_PARAM_H)->EnableWindow(!m_bCir&&m_bUseH);	
		if(m_bCir)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE2);
		else
			m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE);
	}
	else if(m_pDatumPlate2==NULL)
	{	//一块基准钢板时顶部为圆弧的肋板
		//顶部为圆弧时参数H不可用，m_fParaD保存肋板圆弧角度，非圆弧时参数N不可用
		GetDlgItem(IDC_E_PARA_N)->EnableWindow(m_bCir);
		GetDlgItem(IDC_E_PARAM_H)->EnableWindow(!m_bCir);	
		if(m_bCir)	
		{
			SetDlgItemText(IDC_S_PARA_NAME,"β:");
			m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE4);
			m_fParaN=m_fParaW;	//顶部圆弧时N应该等于或小于W
		}
		else
		{
			SetDlgItemText(IDC_S_PARA_NAME,"D:");
			m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE);
		}
	}
	pPic->SetBitmap(m_CurPlateBmp);
	UpdateData(FALSE);
}
void CAddCircularPlateDlg::OnPasteNorm() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fNormX = point.x;
		m_fNormY = point.y;
		m_fNormZ = point.z;
		UpdateData(FALSE);
	}
}

void CAddCircularPlateDlg::OnChangeParaW() 
{
	UpdateData();
	if(m_pDatumPlate2==NULL&&m_bCir)
		m_fParaN=m_fParaW;
	UpdateData(FALSE);
}
#endif
