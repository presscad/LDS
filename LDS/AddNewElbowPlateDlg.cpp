// AddNewElbowPlateDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "AddNewElbowPlateDlg.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "Query.h"
#include "PartsDbDlg.h"
#include "XhCharString.h"
#include "LicFuncDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef __ANGLE_PART_INC_

/////////////////////////////////////////////////////////////////////////////
// CAddNewElbowPlateDlg dialog


CAddNewElbowPlateDlg::CAddNewElbowPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddNewElbowPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddNewElbowPlateDlg)
	m_fParaA = 150;
	m_fParaB = 52;
	m_fParaC = 40;
	m_fParaD = 25;
	m_fParaE = 5;
	m_fParaF = 30;
	m_fParaG = 20;
	m_fParaH = 10;
	m_fParaT = 10;
	m_fScopeAngle = 360.0;
	m_nItemsArrayNum = 4;
	m_iMaterial = 0;
	m_fPosAngle = 0.0;
	m_sPartNo = _T("");
	m_sSegI = _T("");
	m_fTubeOutterR = 0.0;
	m_fFlWidth = 0.0;
	m_iByWeld = 0;
	m_bVertical = FALSE;
	m_bObverse = TRUE;
	m_bInverse = FALSE;
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("Material");
	m_arrHeaderLabel.Add("α°");
	m_arrHeaderLabel.Add("Label Code");
	m_arrHeaderLabel.Add("Segment Number");
#else 
	m_arrHeaderLabel.Add("材质");
	m_arrHeaderLabel.Add("α°");
	m_arrHeaderLabel.Add("编号");
	m_arrHeaderLabel.Add("段号");
#endif
	m_pDatumTube = NULL;
}


void CAddNewElbowPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddNewElbowPlateDlg)
	DDX_Text(pDX, IDC_E_PARA_A, m_fParaA);
	DDX_Text(pDX, IDC_E_PARA_B, m_fParaB);
	DDX_Text(pDX, IDC_E_PARA_C, m_fParaC);
	DDX_Text(pDX, IDC_E_PARA_D, m_fParaD);
	DDX_Text(pDX, IDC_E_PARA_E, m_fParaE);
	DDX_Text(pDX, IDC_E_PARA_F, m_fParaF);
	DDX_Text(pDX, IDC_E_PARA_G, m_fParaG);
	DDX_Text(pDX, IDC_E_PARA_H, m_fParaH);
	DDX_Text(pDX, IDC_E_PARA_T, m_fParaT);
	DDX_Text(pDX, IDC_E_SCOPE_ANGLE, m_fScopeAngle);
	DDX_Text(pDX, IDC_E_ITEMS_ARRAY_NUM, m_nItemsArrayNum);
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_ELBOW_PLATE_POS_ANGLE, m_fPosAngle);
	DDX_Text(pDX, IDC_E_ELBOW_PLATE_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_ELBOW_PLATE_SEG_I, m_sSegI);
	DDX_Text(pDX, IDC_E_TUBE_R, m_fTubeOutterR);
	DDX_Text(pDX, IDC_E_FL_WIDTH, m_fFlWidth);
	DDX_Radio(pDX, IDC_RDO_BY_WELD, m_iByWeld);
	DDX_Check(pDX, IDC_CHK_VERTICAL, m_bVertical);
	DDX_Check(pDX, IDC_CHK_OBVERSE, m_bObverse);
	DDX_Check(pDX, IDC_CHK_INVERSE, m_bInverse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddNewElbowPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CAddNewElbowPlateDlg)
	ON_BN_CLICKED(IDC_BTN_ELBOW_PLATE_MODIFY, OnBtnElbowPlateModify)
	ON_BN_CLICKED(IDC_BTN_ADD_ELBOW_PLATE, OnBtnAddElbowPlate)
	ON_BN_CLICKED(IDC_BTN_LAYOUT_ELBOW_PLATE, OnBtnLayoutElbowPlate)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ELBOW_PLATE, OnDblclkListElbowPlate)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_ELBOW_PLATE, OnKeydownListElbowPlate)
	ON_EN_CHANGE(IDC_E_ELBOW_PLATE_PART_NO, OnChangeEElbowPlatePartNo)
	ON_BN_CLICKED(IDC_BN_PARTS_DB, OnBnPartsDb)
	ON_EN_CHANGE(IDC_E_ITEMS_ARRAY_NUM, OnChangeItemsArrayNum)
	ON_BN_CLICKED(IDC_CHK_INVERSE, OnInverse)
	ON_BN_CLICKED(IDC_CHK_OBVERSE, OnObverse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddNewElbowPlateDlg message handlers

BOOL CAddNewElbowPlateDlg::OnInitDialog() 
{
	if(m_pDatumPlate.IsNULL()||m_pDatumTube==NULL)
		return FALSE;
	else if(m_pDatumPlate->GetClassTypeId()!=CLS_PLATE&&m_pDatumPlate->GetClassTypeId()!=CLS_PARAMPLATE)
		return FALSE;

	CDialog::OnInitDialog();
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
	m_CurPlateBmp.Detach();
	m_CurPlateBmp.LoadBitmap(IDB_BMP_ELBOW_PLATE);
	pPic->SetBitmap(m_CurPlateBmp);
	if(!m_pDatumTube->m_bHasWeldRoad)
		GetDlgItem(IDC_RDO_BY_WELD)->EnableWindow(FALSE);
	long J=0;
	if(m_pDatumPlate->GetClassTypeId()==CLS_PLATE)
		J=m_pDatumPlate.PlatePointer()->m_bAttachPartByWeldRoad;
	else if(m_pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE)
	{
		if(!m_pDatumPlate.ParamPlatePointer()->GetDesignItemValue('J',&J))
			J=m_pDatumTube->feature;	//读取钢管中以前变量m_bAttachPartByWeldRoad的值 现在该变量已去掉
	}
	if(m_pDatumTube->m_bHasWeldRoad&&J!=0)
		m_iByWeld = 0;	//以焊道线为基准
	else 
		m_iByWeld = 1;	//以钢管基准线为基准
	
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_ELBOW_PLATE);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	m_pListCtrl->DeleteAllItems();
	m_fTubeOutterR=m_pDatumTube->GetDiameter()*0.5;
	m_pDatumTube->BuildUCS();
	UCS_STRU tubeUcs=m_pDatumTube->ucs;
	if(m_pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE)
	{
		if(m_pDatumPlate.ParamPlatePointer()->m_iParamType==TYPE_FL)
		{
			m_fScopeAngle = 360;
			m_sSegI = m_pDatumPlate->iSeg.ToString();
			m_sPartNo = m_pDatumPlate->GetPartNo();
			m_nItemsArrayNum = m_pDatumPlate.ParamPlatePointer()->GetLsCount();
			m_fPosAngle = 0;	//肘板一般都会避开基准边(焊道线)初始值给0较合适//(m_fScopeAngle/m_nItemsArrayNum)*0.5;
			double init_angle = (m_fPosAngle/180)*Pi;
			//肘板参数
			double R=0,W=0;
			m_pDatumPlate.ParamPlatePointer()->GetDesignItemValue('W', &W);
			m_pDatumPlate.ParamPlatePointer()->GetDesignItemValue('R', &R);
			m_fFlWidth = 0.5*W-R;
			m_fParaB = m_fFlWidth;
			UpdateData(FALSE);
		}
		//将已经存在的肘板加入到列表中
		if(m_pDatumPlate.ParamPlatePointer()->GetAttachPartCount()>0)
		{
			for(ATTACHPART_ITEM *pItem=m_pDatumPlate.ParamPlatePointer()->GetFirstAttachPart();
				pItem;pItem=m_pDatumPlate.ParamPlatePointer()->GetNextAttachPart())
			{
				CLDSParamPlate *pElbowPlate;
				if(pItem->idClsType==CLS_PARAMPLATE)
				{
					pElbowPlate = (CLDSParamPlate*)console.FromPartHandle(pItem->h);
					if(pElbowPlate&&(pElbowPlate->m_iParamType==TYPE_ELBOWPLATE))
					{
						CString ss;
						CStringArray str_arr;
						char steelmark[50];
						QuerySteelMatMark(pElbowPlate->cMaterial,steelmark);
						str_arr.Add(steelmark);	//材质
						//方向定位角
						double posAngle=0;
						if(!pElbowPlate->GetDesignItemValue('Y',&posAngle))
						{
							f3dPoint origin=pElbowPlate->ucs.origin;
							coord_trans(origin,tubeUcs,FALSE);
							posAngle=Cal2dLineAng(0,0,origin.x,origin.y)*DEGTORAD_COEF;
						}
						ss.Format("%f", posAngle);
						SimplifiedNumString(ss);
						str_arr.Add(ss);	//方位角
						str_arr.Add(pElbowPlate->GetPartNo());	//构件编号
						str_arr.Add(pElbowPlate->iSeg.ToString());	//段号
						int i=AppendRecord(-1,str_arr);
						m_pListCtrl->SetItemData(i,pElbowPlate->handle);
					}
				}
			}
		}
		//基准钢板为参数化板时不需要调整肘板朝向
		GetDlgItem(IDC_CHK_VERTICAL)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_OBVERSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_INVERSE)->EnableWindow(FALSE);
	}
	else if(m_pDatumPlate->GetClassTypeId()==CLS_PLATE)	//计算肘板宽度默认值
	{
		m_fFlWidth = m_pDatumPlate.PlatePointer()->GetWidth()*0.5-m_pDatumPlate.PlatePointer()->m_fInnerRadius;
		m_fParaB = m_fFlWidth;
		if(m_pDatumPlate.PlatePointer()->ucs.axis_z*f3dPoint(0,0,1)>0)
			m_bObverse=FALSE;	//反面
		else
			m_bObverse=TRUE;	//正面
		m_bInverse=!m_bObverse;
	}
	UpdateData(FALSE);
	return TRUE;
}

int CAddNewElbowPlateDlg::AppendRecord(int iItem, CStringArray &str_arr)
{
	//insert items
	LV_ITEM lvi;
	if(iItem==-1)
		iItem=m_pListCtrl->GetItemCount();

	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = str_arr[0].GetBuffer(1);//_gszItem[i][0];
	iItem = m_pListCtrl->InsertItem(&lvi);
	//set item text for additional columns
	int nCount = m_arrHeaderLabel.GetSize();
	nCount = __min(nCount,str_arr.GetSize());
	for(int j = 1; j<nCount; j++)
	{
		str_arr[j].TrimLeft();
		str_arr[j].TrimRight();
		m_pListCtrl->SetItemText(iItem,j,str_arr[j]);
	}
	m_pListCtrl->RedrawItems(iItem,iItem);
	return iItem;
}

void CAddNewElbowPlateDlg::RefreshListViewHeader()
{
	LV_COLUMN lvc;
	int i, NUM_COLUMNS, nColumnCount;
	CHeaderCtrl *pHeader = m_pListCtrl->GetHeaderCtrl();
	nColumnCount = pHeader->GetItemCount();

	// Delete all of the columns.
	for (i=0;i < nColumnCount;i++)
	{
	   m_pListCtrl->DeleteColumn(0);
	}
	if(m_arrHeaderLabel.GetSize()>0)
		NUM_COLUMNS = m_arrHeaderLabel.GetSize();
	else
		NUM_COLUMNS = 0;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(i = 0; i<NUM_COLUMNS; i++)
	{
		lvc.iSubItem = i;
		lvc.pszText = m_arrHeaderLabel[i].GetBuffer(1);//_gszColumnLabel[i];
		lvc.cx = 60;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}


void CAddNewElbowPlateDlg::OnBtnElbowPlateModify() 
{
	m_pListCtrl=(CListCtrl*)GetDlgItem(IDC_LIST_ELBOW_PLATE);
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		UpdateData();
		CXhChar100 ss;//[MAX_TEMP_CHAR_100+1]="";
		m_pListCtrl->SetItemText(iCurSel,0,CSteelMatLibrary::RecordAt(m_iMaterial).mark);	//材质
		ss.Printf("%f",m_fPosAngle);
		SimplifiedNumString(ss);
		m_pListCtrl->SetItemText(iCurSel,1,ss);			//方位角
		m_pListCtrl->SetItemText(iCurSel,2,m_sPartNo);	//构件编号
		m_pListCtrl->SetItemText(iCurSel,3,m_sSegI.GetBuffer());//段号
	}
}

void CAddNewElbowPlateDlg::OnBtnAddElbowPlate() 
{
	CString ss;
	CStringArray str_arr;
	UpdateData();
	str_arr.Add(CSteelMatLibrary::RecordAt(m_iMaterial).mark);	//材质
	ss.Format("%f",m_fPosAngle);
	SimplifiedNumString(ss);
	str_arr.Add(ss);			//方位角
	str_arr.Add(m_sPartNo);		//构件编号
	ss.Format("%d",m_sSegI);
	str_arr.Add(ss);	//段号
	AppendRecord(-1,str_arr);
}

void CAddNewElbowPlateDlg::OnBtnLayoutElbowPlate() 
{	
	m_pListCtrl=(CListCtrl*)GetDlgItem(IDC_LIST_ELBOW_PLATE);
	UpdateData();
	m_pListCtrl->DeleteAllItems();
	double init_angle = m_fPosAngle;
	double piece_angle = m_fScopeAngle/m_nItemsArrayNum;
	CStringArray str_arr;
	str_arr.SetSize(4);
	for(int i=0;i<m_nItemsArrayNum;i++)
	{
		str_arr[0]=CSteelMatLibrary::RecordAt(m_iMaterial).mark;	//材质
		str_arr[1].Format("%f",init_angle+piece_angle*i);
		SimplifiedNumString(str_arr[1]);			//方位角
		str_arr[2]=m_sPartNo;						//构件编号
		str_arr[3].Format("%d",m_sSegI);			//段号
		AppendRecord(-1,str_arr);
	}
}

void CAddNewElbowPlateDlg::OnDblclkListElbowPlate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_pListCtrl=(CListCtrl*)GetDlgItem(IDC_LIST_ELBOW_PLATE);
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		char sText[51];
		m_pListCtrl->GetItemText(iCurSel,0,sText,50);
		char cMat=QuerySteelBriefMatMark(sText);
		m_iMaterial=QuerySteelMatIndex(cMat);	//材质
		m_pListCtrl->GetItemText(iCurSel,1,sText,50);
		m_fPosAngle=atof(sText);				//方位角
		m_pListCtrl->GetItemText(iCurSel,2,sText,50);
		m_sPartNo=sText;						//构件编号
		m_pListCtrl->GetItemText(iCurSel,3,sText,50);
		m_sSegI=sText;					//段号
		UpdateData(FALSE);
	}
	*pResult = 0;
}

void CAddNewElbowPlateDlg::OnKeydownListElbowPlate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	m_pListCtrl=(CListCtrl*)GetDlgItem(IDC_LIST_ELBOW_PLATE);
	if(pLVKeyDow->wVKey==VK_DELETE)
	{
		POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
			long h=(long)m_pListCtrl->GetItemData(iCurSel);
			if(h>0)
			{
				console.DeletePart(h);
				if(m_pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE)
				{
					for(ATTACHPART_ITEM *pItem=m_pDatumPlate.ParamPlatePointer()->GetFirstAttachPart();pItem;
					pItem=m_pDatumPlate.ParamPlatePointer()->GetNextAttachPart())
					{
						if(pItem->h==h)
							m_pDatumPlate.ParamPlatePointer()->DeleteAttachPartItem(pItem->h);
					}
				}
			}
			m_pListCtrl->DeleteItem(iCurSel);
			pos = m_pListCtrl->GetFirstSelectedItemPosition();
		}
	}
	*pResult = 0;
}

void CAddNewElbowPlateDlg::OnChangeEElbowPlatePartNo() 
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
			m_fParaT=pPart->thick;
		}
		//else
		//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	UpdateData(FALSE);
}

void CAddNewElbowPlateDlg::OnOK() 
{
	int nCount=m_pListCtrl->GetItemCount();
	char sText[101];
	double z_level=0;
	if(m_pDatumPlate->GetClassTypeId()==CLS_PLATE)
		z_level=m_pDatumPlate.PlatePointer()->m_fNormOffset;
	for(int i=0;i<nCount;i++)
	{
		long h=(long)m_pListCtrl->GetItemData(i);
		if(h>0)	//原有肘板,跳过
			continue;
		CLDSParamPlate *pElbowPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pElbowPlate->m_iParamType = TYPE_ELBOWPLATE;
		pElbowPlate->m_hPartWeldParent = m_pDatumPlate->handle;
		pElbowPlate->cfgword=m_pDatumPlate->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
#ifdef AFX_TARG_ENU_ENGLISH
		pElbowPlate->SetDesignItemValue('A',m_fParaA,"Elbow plate parameter A",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('B',m_fParaB,"Elbow plate parameter B",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('C',m_fParaC,"Elbow plate parameter C",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('D',m_fParaD,"Elbow plate parameter D",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('E',m_fParaE,"Elbow plate parameter E",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('F',m_fParaF,"Elbow plate parameter F",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('G',m_fParaG,"Elbow plate parameter G",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('H',m_fParaH,"Elbow plate parameter H",PARAM_SHAPE);
		if(m_pDatumPlate->GetClassTypeId()==CLS_PLATE)
		{
			if(m_bObverse)	//在基准钢板正面布置肘板
				pElbowPlate->SetDesignItemValue('Z',1,"1 Front Layout Plate，-1 Reverse Layout Plate",PARAM_POS);
			else if(m_bInverse)
				pElbowPlate->SetDesignItemValue('Z',-1,"1 Front Layout Plate，-1 Reverse Layout Plate",PARAM_POS);
		}
#else 
		pElbowPlate->SetDesignItemValue('A',m_fParaA,"肘板参数A",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('B',m_fParaB,"肘板参数B",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('C',m_fParaC,"肘板参数C",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('D',m_fParaD,"肘板参数D",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('E',m_fParaE,"肘板参数E",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('F',m_fParaF,"肘板参数F",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('G',m_fParaG,"肘板参数G",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('H',m_fParaH,"肘板参数H",PARAM_SHAPE);
		if(m_pDatumPlate->GetClassTypeId()==CLS_PLATE)
		{
			if(m_bObverse)	//在基准钢板正面布置肘板
				pElbowPlate->SetDesignItemValue('Z',1,"1表示在基准钢板正面布置钢板，-1表示在反面布置",PARAM_POS);
			else if(m_bInverse)
				pElbowPlate->SetDesignItemValue('Z',-1,"1表示在基准钢板正面布置钢板，-1表示在反面布置",PARAM_POS);
		}
#endif
		if(m_iByWeld==0)		//以焊道线为基准
		{
			if(m_pDatumPlate->GetClassTypeId()==CLS_PLATE)
				m_pDatumPlate.PlatePointer()->m_bAttachPartByWeldRoad=TRUE;
			else if(m_pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE)
#ifdef AFX_TARG_ENU_ENGLISH
				m_pDatumPlate.ParamPlatePointer()->SetDesignItemValue('J',1,"Part circular locate mode:0 datum edge;1 weld road line");
#else 
				m_pDatumPlate.ParamPlatePointer()->SetDesignItemValue('J',1,"构件环向定位方式:0基准边为基准;1焊道线为基准。");
#endif
		}
		else //if(m_iByWeld==1)	//以钢管基准边为基准
		{
			if(m_pDatumPlate->GetClassTypeId()==CLS_PLATE)
				m_pDatumPlate.PlatePointer()->m_bAttachPartByWeldRoad=FALSE;
			else if(m_pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE)
#ifdef AFX_TARG_ENU_ENGLISH
				m_pDatumPlate.ParamPlatePointer()->SetDesignItemValue('J',0,"Part circular locate mode:0 datum edge;1 weld  roadLine。");
#else 
				m_pDatumPlate.ParamPlatePointer()->SetDesignItemValue('J',0,"构件环向定位方式:0基准边为基准;1焊道线为基准。");
#endif
		}
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_pDatumPlate->ucs.axis_z*m_pDatumTube->ucs.axis_z>0)
			pElbowPlate->SetDesignItemValue('K',-m_pDatumTube->handle,"The datum tube Of elbow plate relayed on ",PARAM_POS,1);
		else
			pElbowPlate->SetDesignItemValue('K',m_pDatumTube->handle,"The datum tube Of elbow plate relayed on",PARAM_POS,1);
		//肘板垂直基准钢管或者肘板沿基准钢管方向
		if(m_bVertical)
			pElbowPlate->SetDesignItemValue('V',1,"0.Elbow plate along datum tube 1.竖直且带自动正为的肘板");
		else 
			pElbowPlate->SetDesignItemValue('V',0,"0.Elbow plate along datum tube 1.竖直且带自动正为的肘板");
		pElbowPlate->SetDesignItemValue('L',m_pDatumPlate->handle,"Datum bottom plate",PARAM_POS,1);
#else 
		if(m_pDatumPlate->ucs.axis_z*m_pDatumTube->ucs.axis_z>0)
			pElbowPlate->SetDesignItemValue('K',-m_pDatumTube->handle,"肘板所依靠的基准钢管",PARAM_POS,1);
		else
			pElbowPlate->SetDesignItemValue('K',m_pDatumTube->handle,"肘板所依靠的基准钢管",PARAM_POS,1);
		//肘板垂直基准钢管或者肘板沿基准钢管方向
		if(m_bVertical)
			pElbowPlate->SetDesignItemValue('V',1,"0.沿基准钢管方向的肘板 1.竖直且带自动正为的肘板");
		else 
			pElbowPlate->SetDesignItemValue('V',0,"0.沿基准钢管方向的肘板 1.竖直且带自动正为的肘板");
		pElbowPlate->SetDesignItemValue('L',m_pDatumPlate->handle,"基准底板",PARAM_POS,1);
#endif
		pElbowPlate->m_bDisplayHole=TRUE;
		m_pListCtrl->GetItemText(i,0,sText,100);	//材质
		//设定材质
		pElbowPlate->cMaterial=QuerySteelBriefMatMark(sText);
		//设定肘板坐标系
		m_pListCtrl->GetItemText(i,1,sText,100);	//方位角
#ifdef AFX_TARG_ENU_ENGLISH
		pElbowPlate->SetDesignItemValue('Y',atof(sText),"Elbow plate locate angle°");
#else 
		pElbowPlate->SetDesignItemValue('Y',atof(sText),"肘板定位方向角°");
#endif
		CXhChar16 label;
		m_pListCtrl->GetItemText(i,2,label,16);	//编号
		pElbowPlate->SetPartNo(label);
		char ss[11];
		m_pListCtrl->GetItemText(i,3,ss,10);	//段号
		pElbowPlate->iSeg=SEGI(ss);
		pElbowPlate->SetLayer(m_pDatumPlate->layer());	//调整肘板的图层名
		pElbowPlate->thick = (long)m_fParaT;
		pElbowPlate->DesignPlate();
		double scale_user2scr=g_pSolidOper->GetScaleUserToScreen();
		double sample_len=g_pSolidSet->GetArcSamplingLength();
		pElbowPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,scale_user2scr,sample_len,g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pElbowPlate->GetSolidPartObject());
		g_pSolidDraw->Draw();
		if(m_pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE)//将肘板存放在法兰盘的依附构件链表里
			m_pDatumPlate.ParamPlatePointer()->AppendAttachPart(pElbowPlate);
	}
	CDialog::OnOK();
}

void CAddNewElbowPlateDlg::OnBnPartsDb() 
{
	CPartsDbDlg partsDlg;
	partsDlg.m_bRefreshDlgCtrl = TRUE;
	partsDlg.m_bIncParamElbow = TRUE;
	if(partsDlg.DoModal()==IDOK)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)partsDlg.m_pSelectPart;
		if(pParamPlate&&pParamPlate->m_iParamType==TYPE_ELBOWPLATE)
		{
			pParamPlate->GetDesignItemValue('A', &m_fParaA);
			pParamPlate->GetDesignItemValue('B', &m_fParaB);
			pParamPlate->GetDesignItemValue('C', &m_fParaC);
			pParamPlate->GetDesignItemValue('D', &m_fParaD);
			pParamPlate->GetDesignItemValue('E', &m_fParaE);
			pParamPlate->GetDesignItemValue('F', &m_fParaF);
			pParamPlate->GetDesignItemValue('G', &m_fParaG);
			pParamPlate->GetDesignItemValue('H', &m_fParaH);
			m_fParaT=pParamPlate->thick;
			m_sPartNo=pParamPlate->GetPartNo();
			m_sSegI=pParamPlate->iSeg.ToString();
			m_iMaterial=QuerySteelMatIndex(pParamPlate->cMaterial);
			UpdateData(FALSE);
		}
	}
}

void CAddNewElbowPlateDlg::OnChangeItemsArrayNum() 
{
	//UpdateData();
	//m_fPosAngle = (m_fScopeAngle/m_nItemsArrayNum)*0.5;
	//UpdateData(FALSE);
}

void CAddNewElbowPlateDlg::OnInverse() 
{
	UpdateData();
	m_bObverse=!m_bInverse;
	UpdateData(FALSE);
}

void CAddNewElbowPlateDlg::OnObverse() 
{
	UpdateData();
	m_bInverse=!m_bObverse;
	UpdateData(FALSE);
}
#endif
