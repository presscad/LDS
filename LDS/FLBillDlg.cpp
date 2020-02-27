//<LOCALE_TRANSLATE Confirm by hxr/>
// FLBillDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "FLBillDlg.h"
#include "SortFunc.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "Query.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(__TSA_) || defined(__LDS_)
#include "PostProcessor.h"
/////////////////////////////////////////////////////////////////////////////
// CFLBillDlg dialog
struct TUBE_SIZE_SORT_ITEM{
	char sizeSpec[100];
	double specF;
};
int compare_tube_size(const TUBE_SIZE_SORT_ITEM& item1,const TUBE_SIZE_SORT_ITEM& item2)
{
	if(item1.specF>item2.specF)
		return 1;
	else if(item1.specF<item2.specF)
		return -1;
	else
		return 0;
}

CFLBillDlg::CFLBillDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFLBillDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFLBillDlg)
	m_sIncSegStr = _T("*");
	m_iMat = 0;
	m_sTubeGuige = _T("");
	m_iBoltGrade = 0;
	m_fAxisForce = 0.0;
	m_fAxisMoment = 0.0;
	m_fBoltCirDia = 0.0;
	m_nBoltNum = 0;
	m_sFlNo = _T("");
	m_fFlDia = 0.0;
	m_nFlThick = 0;
	m_nRibHeight = 0;
	m_iBoltD = 0;
	m_nCoupleFlThick = 0;
	m_sCoupleFlNo = _T("");
	m_nCoupleRibHeight = 0;
	m_bSpecBoltNum = FALSE;
	m_bSpecFlDia = FALSE;
	m_bSpecRibHeight = FALSE;
	m_bSpecRibThick = FALSE;
	m_bSpecBoltCir = FALSE;
	m_iFlMat = -1;
	m_nRibThick = 0;
	m_fCoupleSumWeight = 0.0;
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_listCtrl.AddColumnHeader("Flange No.",70);
	m_listCtrl.AddColumnHeader("Tube handle",70);
	m_listCtrl.AddColumnHeader("Tube spec",70);
	m_listCtrl.AddColumnHeader("Connecting end",60);
	m_listCtrl.AddColumnHeader("Axial force,N",70);
	m_listCtrl.AddColumnHeader("Bending moment,N*m",70);
	m_listCtrl.AddColumnHeader("Flange thickness",70);
	m_listCtrl.AddColumnHeader("Rib plate thickness",70);
	m_listCtrl.AddColumnHeader("Bolts spec",70);
	m_listCtrl.AddColumnHeader("Bolts amount",70);
	m_listCtrl.AddColumnHeader("Bolt diameter",80);
#else
	m_listCtrl.AddColumnHeader("法兰编号",70);
	m_listCtrl.AddColumnHeader("钢管句柄",70);
	m_listCtrl.AddColumnHeader("钢管规格",70);
	m_listCtrl.AddColumnHeader("连接端",60);
	m_listCtrl.AddColumnHeader("轴向力,N",70);
	m_listCtrl.AddColumnHeader("弯矩,N*m",70);
	m_listCtrl.AddColumnHeader("法兰厚度",70);
	m_listCtrl.AddColumnHeader("肋板厚度",70);
	m_listCtrl.AddColumnHeader("螺栓规格",70);
	m_listCtrl.AddColumnHeader("螺栓数量",70);
	m_listCtrl.AddColumnHeader("螺栓圆直径",80);
#endif
	m_listCtrl.m_bEnableSortItems=TRUE;
}


void CFLBillDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFLBillDlg)
	DDX_Control(pDX, IDC_TUBE_FL_LIST_CTRL, m_listCtrl);
	DDX_Text(pDX, IDC_E_INC_SEG_STR, m_sIncSegStr);
	DDX_CBIndex(pDX, IDC_CMB_TUBE_MATERIAL, m_iMat);
	DDX_CBString(pDX, IDC_CMB_TUBE_GUIGE, m_sTubeGuige);
	DDX_CBIndex(pDX, IDC_CMB_BOLT_GRADE, m_iBoltGrade);
	DDX_Text(pDX, IDC_E_AXIS_FORCE, m_fAxisForce);
	DDX_Text(pDX, IDC_E_AXIS_MOMENT, m_fAxisMoment);
	DDX_Text(pDX, IDC_E_BOLT_CIR_DIA, m_fBoltCirDia);
	DDX_Text(pDX, IDC_E_BOLT_NUM, m_nBoltNum);
	DDX_Text(pDX, IDC_E_FL_NO, m_sFlNo);
	DDX_Text(pDX, IDC_E_FL_DIA, m_fFlDia);
	DDX_Text(pDX, IDC_E_FL_THICK, m_nFlThick);
	DDX_Text(pDX, IDC_E_RIB_HEIGHT, m_nRibHeight);
	DDX_CBIndex(pDX, IDC_CMB_BOLT_D, m_iBoltD);
	DDX_Text(pDX, IDC_E_COUPLE_FL_THICK, m_nCoupleFlThick);
	DDX_Text(pDX, IDC_E_COUPLE_FL_NO, m_sCoupleFlNo);
	DDX_Text(pDX, IDC_E_COUPLE_RIB_HEIGHT, m_nCoupleRibHeight);
	DDX_Check(pDX, IDC_CHK_SPEC_BOLT_NUM, m_bSpecBoltNum);
	DDX_Check(pDX, IDC_CHK_SPEC_FL_DIA, m_bSpecFlDia);
	DDX_Check(pDX, IDC_CHK_SPEC_RIB_HEIGHT, m_bSpecRibHeight);
	DDX_Check(pDX, IDC_CHK_SPEC_RIB_THICK, m_bSpecRibThick);
	DDX_Check(pDX, IDC_CHK_SPEC_BOLT_CIRCLE, m_bSpecBoltCir);
	DDX_CBIndex(pDX, IDC_CMB_FL_MATERIAL, m_iFlMat);
	DDX_Text(pDX, IDC_E_RIB_THICK, m_nRibThick);
	DDX_Text(pDX, IDC_E_COUPLE_SUM_WEIGHT, m_fCoupleSumWeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFLBillDlg, CDialog)
	//{{AFX_MSG_MAP(CFLBillDlg)
	ON_COMMAND(ID_DESIGN_COMMON_FL, OnDesignCommonFl)
	ON_COMMAND(ID_AUTO_MATCH_P_WELD_FL, OnAutoMatchPWeldFl)
	ON_COMMAND(ID_AUTO_MATCH_D_WELD_FL, OnAutoMatchDWeldFl)
	ON_NOTIFY(NM_RCLICK, IDC_TUBE_FL_LIST_CTRL, OnRclickTubeFlListCtrl)
	ON_CBN_SELCHANGE(IDC_CMB_TUBE_MATERIAL, OnSelchangeCmbTubeMaterial)
	ON_CBN_SELCHANGE(IDC_CMB_TUBE_GUIGE, OnSelchangeCmbTubeGuige)
	ON_EN_CHANGE(IDC_E_INC_SEG_STR, OnChangeEIncSegStr)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TUBE_FL_LIST_CTRL, OnItemchangedTubeFlListCtrl)
	ON_BN_CLICKED(IDC_BN_FL_SAVE, OnBnFlSave)
	ON_BN_CLICKED(IDC_BN_FL_REDESIGN_HAS_RIB, OnBnFlRedesignHasRib)
	ON_BN_CLICKED(IDC_BN_FL_REDESIGN_NONE_RIB, OnBnFlRedesignNoneRib)
	ON_BN_CLICKED(IDC_BN_FL_REDESIGN_ALL, OnBnFlRedesignAll)
	ON_BN_CLICKED(IDC_CHK_SPEC_FL_DIA, OnChkSpecPara)
	ON_BN_CLICKED(IDC_CHK_SPEC_BOLT_NUM, OnChkSpecPara)
	ON_BN_CLICKED(IDC_CHK_SPEC_BOLT_CIRCLE, OnChkSpecPara)
	ON_BN_CLICKED(IDC_CHK_SPEC_RIB_HEIGHT, OnChkSpecPara)
	ON_BN_CLICKED(IDC_CHK_SPEC_RIB_THICK, OnChkSpecPara)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFLBillDlg message handlers

BOOL CFLBillDlg::OnInitDialog() 
{
	CComboBox* pCMBFlMat = (CComboBox*)GetDlgItem(IDC_CMB_FL_MATERIAL);
	AddSteelMatRecord(pCMBFlMat);
	CDialog::OnInitDialog();
	OnChkSpecPara();
	m_listCtrl.InitListCtrl();
	//设置第一列按件号排序
	m_listCtrl.SetColumnSortType(0,SORTTYPE_PARTNO);
	//设置第二列按句柄排序
	m_listCtrl.SetColumnSortType(1,SORTTYPE_HEX_NUM);
	//设置第三列按规格排序
	m_listCtrl.SetColumnSortType(2,SORTTYPE_SPEC);
	//统计生成法兰匹配对
	CJointCouple::StatJointLinkPart(&flOpt.listCouple,CLS_LINETUBE);
	CJointCouple *pCouple;
	//确定法兰设计依据力学分析单元
	for(CElement* pElem=postprocessor.hashElems.GetFirst();pElem;pElem=postprocessor.hashElems.GetNext())
	{
		if(pElem->pStart!=pElem->pOrgPole->pStart&&pElem->pEnd!=pElem->pOrgPole->pEnd)
			continue;
		if(pElem->pOrgPole->size_idClassType!=CLS_LINETUBE)
			continue;
		for(pCouple=flOpt.listCouple.GetFirst();pCouple;pCouple=flOpt.listCouple.GetNext())
		{
			if(pCouple->pLinkRod==pElem->pOrgPole)
			{
				if( (pElem->pStart==pElem->pOrgPole->pStart&&pElem->pStart==pCouple->pNode)||
					(pElem->pEnd==pElem->pOrgPole->pEnd&&pElem->pEnd==pCouple->pNode))
				{
					pCouple->N=pElem->MaxTension.N;
					pCouple->M=0;	//暂不考虑弯矩
				}
			}
		}
	}
	for(pCouple=flOpt.listCouple.GetFirst();pCouple;pCouple=flOpt.listCouple.GetNext())
	{
		if(pCouple->pRod->size_idClassType!=CLS_LINETUBE)
			continue;
		for(CConnectFL *pConnFl=Ta.hashConnectFl.GetFirst();pConnFl;pConnFl=Ta.hashConnectFl.GetNext())
		{
			if( (pCouple->pRod->pStart==pCouple->pNode&&stricmp(pCouple->pRod->connectStart.flNo,pConnFl->m_sPartNo)==0)||
				(pCouple->pRod->pEnd==pCouple->pNode&&stricmp(pCouple->pRod->connectEnd.flNo,pConnFl->m_sPartNo)==0))
				pCouple->info.FL.pFLInfo=pConnFl;
			if( (pCouple->pLinkRod->pStart==pCouple->pNode&&stricmp(pCouple->pLinkRod->connectStart.flNo,pConnFl->m_sPartNo)==0)||
				(pCouple->pLinkRod->pEnd==pCouple->pNode&&stricmp(pCouple->pLinkRod->connectEnd.flNo,pConnFl->m_sPartNo)==0))
				pCouple->info.FL.pCoupleFlInfo=pConnFl;
			if(pCouple->info.FL.pFLInfo&&pCouple->info.FL.pCoupleFlInfo)
				break;
		}
#ifdef __LDS_
		for(CLDSPart *pFl=Ta.Parts.GetFirst(CLS_PARAMPLATE);pFl;pFl=Ta.Parts.GetNext(CLS_PARAMPLATE))
		{
			if(pCouple->pRod->GetClassTypeId()!=CLS_LINETUBE)
				continue;
			CLDSLineTube *pRodTube=(CLDSLineTube*)pCouple->pRod;
			CLDSLineTube *pCoupleRodTube=(CLDSLineTube*)pCouple->pLinkRod;
			if( (pRodTube->pStart==pCouple->pNode&&pRodTube->m_tJointStart.hLinkObj==pFl->handle)||
				(pRodTube->pEnd==pCouple->pNode&&pRodTube->m_tJointEnd.hLinkObj==pFl->handle))
			{
				pCouple->info.FL.pFL=(CLDSParamPlate*)pFl;
				break;
			}
			if( (pCoupleRodTube->pStart==pCouple->pNode&&pCoupleRodTube->m_tJointStart.hLinkObj==pFl->handle)||
				(pCoupleRodTube->pEnd==pCouple->pNode&&pCoupleRodTube->m_tJointEnd.hLinkObj==pFl->handle))
			{
				pCouple->info.FL.pCoupleFl=(CLDSParamPlate*)pFl;
				break;
			}
		}
#endif
	}
	OnChangeEIncSegStr();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFLBillDlg::OnChkSpecPara() 
{
	UpdateData();
	CEdit* pEdit=(CEdit*)GetDlgItem(IDC_E_BOLT_CIR_DIA);
	pEdit->SetReadOnly(!m_bSpecBoltCir);
	pEdit=(CEdit*)GetDlgItem(IDC_E_BOLT_NUM);
	pEdit->SetReadOnly(!m_bSpecBoltNum);
	pEdit=(CEdit*)GetDlgItem(IDC_E_FL_DIA);
	pEdit->SetReadOnly(!m_bSpecFlDia);
	pEdit=(CEdit*)GetDlgItem(IDC_E_RIB_HEIGHT);
	pEdit->SetReadOnly(!m_bSpecRibHeight);
	pEdit=(CEdit*)GetDlgItem(IDC_E_COUPLE_RIB_HEIGHT);
	pEdit->SetReadOnly(!m_bSpecRibHeight);
	pEdit=(CEdit*)GetDlgItem(IDC_E_RIB_THICK);
	pEdit->SetReadOnly(!m_bSpecRibThick);
}

void CFLBillDlg::OnChangeEIncSegStr() 
{
	UpdateData();
	char seg_str[200]="";
	if(m_sIncSegStr.Compare("*")!=0)
		strcpy(seg_str,m_sIncSegStr);
	CHashList<SEGI>hashSeg;
	for(SEGI segI=FindAdaptNoForSegI(seg_str);segI.iSeg>0;segI=FindAdaptNoForSegI(NULL))
		hashSeg.SetValue(segI.iSeg,segI);
	CHashList<char>hashMat;
	for(CJointCouple* pCouple=flOpt.listCouple.GetFirst();pCouple;pCouple=flOpt.listCouple.GetNext())
	{
		if(m_sIncSegStr[0]=='*'||hashSeg.GetValue(pCouple->pRod->iSeg.iSeg))
			hashMat.SetValue(pCouple->pRod->cMaterial,pCouple->pRod->cMaterial);
	}
	CComboBox* pCMB=(CComboBox*)GetDlgItem(IDC_CMB_TUBE_MATERIAL);
	pCMB->ResetContent();
	int iOrder=pCMB->AddString("*");
	pCMB->SetItemData(iOrder,0);
	for(char* pcMaterial=hashMat.GetFirst();pcMaterial;pcMaterial=hashMat.GetNext())
	{
		char steelmark[20]="";
		if(QuerySteelMatMark(*pcMaterial,steelmark))
		{
			iOrder=pCMB->AddString(steelmark);
			pCMB->SetItemData(iOrder,*pcMaterial);
		}
	}
	if(pCMB->GetCount()>0)
		m_iMat=pCMB->SetCurSel(0);
	OnSelchangeCmbTubeMaterial();
}
void CFLBillDlg::OnSelchangeCmbTubeMaterial() 
{
	CStringKeyHashList<double>hashTubeSize;
	CComboBox* pCMB=(CComboBox*)GetDlgItem(IDC_CMB_TUBE_MATERIAL);
	CComboBox* pCMBSize=(CComboBox*)GetDlgItem(IDC_CMB_TUBE_GUIGE);
	pCMBSize->ResetContent();
	m_listCtrl.DeleteAllItems();
	m_iMat=pCMB->GetCurSel();
	if(m_iMat<0)
		return;
	char cSelMat=(char)pCMB->GetItemData(pCMB->GetCurSel());
	char seg_str[200];
	CHashList<SEGI>hashSeg;
	for(SEGI segI=FindAdaptNoForSegI(seg_str);segI.iSeg>0;segI=FindAdaptNoForSegI(NULL))
		hashSeg.SetValue(segI.iSeg,segI);
	strcpy(seg_str,m_sIncSegStr);
	for(CJointCouple *pCouple=flOpt.listCouple.GetFirst();pCouple;pCouple=flOpt.listCouple.GetNext())
	{
		if(pCouple->pRod->size_idClassType!=CLS_LINETUBE)
			continue;
		if(cSelMat!=0&&cSelMat!=pCouple->pRod->cMaterial)
			continue;	//材质不在当前显示范围内
		if(m_sIncSegStr[0]=='*'||hashSeg.GetValue(pCouple->pRod->iSeg.iSeg))
		{
			char sizeSpec[100]="";
			pCouple->pRod->GetSpecification(sizeSpec,FALSE);
			double specF=pCouple->pRod->GetWidth()+pCouple->pRod->GetThick()/1000.0;
			hashTubeSize.SetValue(sizeSpec,specF);
		}
	}
	TUBE_SIZE_SORT_ITEM *sizeArr= new TUBE_SIZE_SORT_ITEM[hashTubeSize.GetNodeNum()];
	DWORD i=0;
	for(double *pValue=hashTubeSize.GetFirst();pValue;pValue=hashTubeSize.GetNext(),i++)
	{
		strcpy(sizeArr[i].sizeSpec,hashTubeSize.GetCursorKey());
		sizeArr[i].specF=*pValue;
	}
	CBubbleSort<TUBE_SIZE_SORT_ITEM>::BubSort(sizeArr,hashTubeSize.GetNodeNum(),compare_tube_size);
	pCMBSize->AddString("*");
	for(i=0;i<hashTubeSize.GetNodeNum();i++)
		pCMBSize->AddString(sizeArr[i].sizeSpec);
	delete []sizeArr;
	if(pCMBSize->GetCount()>0)
		pCMBSize->SetCurSel(0);
	OnSelchangeCmbTubeGuige();
}

void CFLBillDlg::RefreshItemCtrlValue(int iItem)
{
	if(iItem<0)
		return;
	CJointCouple *pSelCouple=(CJointCouple*)m_listCtrl.GetItemData(iItem);
	if(pSelCouple==NULL)
		return;
	if(pSelCouple->info.FL.pFL)
		m_sFlNo=pSelCouple->info.FL.pFL->GetPartNo();
	else if(pSelCouple->info.FL.pFLInfo)
	{
		m_sFlNo=pSelCouple->info.FL.pFLInfo->m_sPartNo;
		if(stricmp(pSelCouple->info.FL.pFLInfo->bolt_grade,"4.8")==0)
			m_iBoltGrade=0;
		else if(stricmp(pSelCouple->info.FL.pFLInfo->bolt_grade,"5.8")==0)
			m_iBoltGrade=1;
		else if(stricmp(pSelCouple->info.FL.pFLInfo->bolt_grade,"6.8")==0)
			m_iBoltGrade=2;
		else //if(stricmp(pSelCouple->info.FL.pFLInfo->bolt_grade,"8.8")==0)
			m_iBoltGrade=3;
		if(pSelCouple->info.FL.pFLInfo->bolt_d==16)
			m_iBoltD=0;	//M16
		else if(pSelCouple->info.FL.pFLInfo->bolt_d==20)
			m_iBoltD=1;	//M20
		else
			m_iBoltD=2;	//M24
		CComboBox *pCMBFlMat=(CComboBox*)GetDlgItem(IDC_CMB_FL_MATERIAL);
		for(int i=0;i<pCMBFlMat->GetCount();i++)
		{
			if((BYTE)pCMBFlMat->GetItemData(i)==pSelCouple->info.FL.pFLInfo->cMaterial)
			{
				m_iFlMat=i;
				break;
			}
		}
		if(pSelCouple->info.FL.pCoupleFlInfo)
			m_sCoupleFlNo=pSelCouple->info.FL.pCoupleFlInfo->m_sPartNo;
		m_fBoltCirDia=pSelCouple->info.FL.pFLInfo->m_fBoltRadius*2;
		m_nBoltNum=pSelCouple->info.FL.pFLInfo->bolt_n;
		m_nFlThick=pSelCouple->info.FL.pFLInfo->m_nThickFL;
		m_nCoupleFlThick=pSelCouple->info.FL.pFLInfo->m_nCoupleThickFL;
		m_fFlDia=(pSelCouple->info.FL.pFLInfo->m_fInnerRadius+pSelCouple->info.FL.pFLInfo->m_nWidthFL)*2.0;
		m_nRibThick=pSelCouple->info.FL.pFLInfo->m_nThickRib;
		m_nRibHeight=pSelCouple->info.FL.pFLInfo->m_nHeightRib;
		m_nCoupleRibHeight=pSelCouple->info.FL.pFLInfo->m_nCoupleHeightRib;
		m_fCoupleSumWeight=pSelCouple->info.FL.pFLInfo->SumWeight();
	}
	else
		m_sFlNo=pSelCouple->label;
	m_fAxisForce=pSelCouple->N;
	m_fAxisMoment=pSelCouple->M;
	UpdateData(FALSE);
}

void CFLBillDlg::RefreshListViewItem(CJointCouple* pJoint,int iItem)
{
	CComboBox* pCMBMat=(CComboBox*)GetDlgItem(IDC_CMB_TUBE_MATERIAL);
	CComboBox* pCMBSize=(CComboBox*)GetDlgItem(IDC_CMB_TUBE_GUIGE);
	char cSelMat=(char)pCMBMat->GetItemData(pCMBMat->GetCurSel());
	CStringArray str_arr;
	str_arr.SetSize(m_listCtrl.m_arrHeader.GetSize());
	if(pJoint->info.FL.pFL)
		str_arr[0]=pJoint->info.FL.pFL->GetPartNo();
	else if(pJoint->info.FL.pFLInfo)
		str_arr[0]=pJoint->info.FL.pFLInfo->m_sPartNo;
	else
		str_arr[0]=pJoint->label;
	str_arr[1].Format("0X%X",pJoint->pRod->handle);
	str_arr[4].Format("%.0f",pJoint->N);
	str_arr[5].Format("%.0f",pJoint->M);
	m_listCtrl.SetItemText(iItem,0,str_arr[0]);
	m_listCtrl.SetItemText(iItem,1,str_arr[1]);
	m_listCtrl.SetItemText(iItem,4,str_arr[4]);
	m_listCtrl.SetItemText(iItem,5,str_arr[5]);
	if(pJoint->info.FL.pFL)
		str_arr[6].Format("%.0f",pJoint->info.FL.pFL->GetThick());
	else if(pJoint->info.FL.pFLInfo)
	{
		str_arr[6].Format("%d",pJoint->info.FL.pFLInfo->m_nThickFL);
		str_arr[7].Format("%d",pJoint->info.FL.pFLInfo->m_nThickRib);
		char boltSize[24]="";
		sprintf(boltSize,"%sM%d",pJoint->info.FL.pFLInfo->bolt_grade,pJoint->info.FL.pFLInfo->bolt_d);
		str_arr[8]=boltSize;
		str_arr[9].Format("%d",pJoint->info.FL.pFLInfo->bolt_n);
		str_arr[10].Format("%.1f",pJoint->info.FL.pFLInfo->m_fBoltRadius*2);
		m_listCtrl.SetItemText(iItem,7,str_arr[7]);
		m_listCtrl.SetItemText(iItem,8,str_arr[8]);
		m_listCtrl.SetItemText(iItem,9,str_arr[9]);
		m_listCtrl.SetItemText(iItem,10,str_arr[10]);
	}
	m_listCtrl.SetItemText(iItem,6,str_arr[6]);
	m_listCtrl.RedrawItems(iItem,iItem);
}
void CFLBillDlg::OnSelchangeCmbTubeGuige() 
{
	m_listCtrl.DeleteAllItems();
	CComboBox* pCMBMat=(CComboBox*)GetDlgItem(IDC_CMB_TUBE_MATERIAL);
	CComboBox* pCMBSize=(CComboBox*)GetDlgItem(IDC_CMB_TUBE_GUIGE);
	char cSelMat=(char)pCMBMat->GetItemData(pCMBMat->GetCurSel());
	char seg_str[200]="";
	if(m_sIncSegStr.Compare("*")!=0)
		strcpy(seg_str,m_sIncSegStr);
	CHashList<SEGI>hashSeg;
	for(SEGI segI=FindAdaptNoForSegI(seg_str);segI.iSeg>0;segI=FindAdaptNoForSegI(NULL))
		hashSeg.SetValue(segI.iSeg,segI);
	CString sizeSpec;
	pCMBSize->GetLBText(pCMBSize->GetCurSel(),sizeSpec);
	CStringArray str_arr;
	str_arr.SetSize(m_listCtrl.m_arrHeader.GetSize());
	for(CJointCouple *pCouple=flOpt.listCouple.GetFirst();pCouple;pCouple=flOpt.listCouple.GetNext())
	{
		if(pCouple->pRod->size_idClassType!=CLS_LINETUBE)
			continue;
		if(m_sIncSegStr[0]!='*'&&!hashSeg.GetValue(pCouple->pRod->iSeg.iSeg))
			continue;	//段号不在当前显示范围内
		if(cSelMat!=0&&cSelMat!=pCouple->pRod->cMaterial)
			continue;	//材质不在当前显示范围内
		char spec[100]="";
		pCouple->pRod->GetSpecification(spec,FALSE);
		if(sizeSpec[0]!='*'&&sizeSpec.CompareNoCase(spec)!=0)
			continue;	//规格不在当前显示范围内

		if(pCouple->info.FL.pFL)
			str_arr[0]=pCouple->info.FL.pFL->GetPartNo();
		else if(pCouple->info.FL.pFLInfo)
			str_arr[0]=pCouple->info.FL.pFLInfo->m_sPartNo;
		else
			str_arr[0]=pCouple->label;
		str_arr[1].Format("0X%X",pCouple->pRod->handle);
		str_arr[2]=spec;
		if(pCouple->pRod->pStart==pCouple->pNode)
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[3]="starting";
		else
			str_arr[3]="ending";
#else
			str_arr[3]="始端";
		else
			str_arr[3]="终端";
#endif
		str_arr[4].Format("%.0f",pCouple->N);
		str_arr[5].Format("%.0f",pCouple->M);
		if(pCouple->info.FL.pFL)
			str_arr[6].Format("%.0f",pCouple->info.FL.pFL->GetThick());
		else if(pCouple->info.FL.pFLInfo)
		{
			str_arr[6].Format("%d",pCouple->info.FL.pFLInfo->m_nThickFL);
			str_arr[7].Format("%d",pCouple->info.FL.pFLInfo->m_nThickRib);
			char boltSize[24]="";
			sprintf(boltSize,"%sM%d",pCouple->info.FL.pFLInfo->bolt_grade,pCouple->info.FL.pFLInfo->bolt_d);
			str_arr[8]=boltSize;
			str_arr[9].Format("%d",pCouple->info.FL.pFLInfo->bolt_n);
			str_arr[10].Format("%.1f",pCouple->info.FL.pFLInfo->m_fBoltRadius*2);
		}
		int iInsert=m_listCtrl.InsertItemRecord(-1,str_arr);
		m_listCtrl.SetItemData(iInsert,(DWORD)pCouple);
	}
}

void CFLBillDlg::ContextMenu(CWnd *pWnd, CPoint point)
{
	CPoint scr_point = point;
	m_listCtrl.ClientToScreen(&scr_point);
	//if(m_listCtrl.GetSelectedCount()<=0)
	//	return;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_FL_BILL_DESIGN);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}

void CFLBillDlg::OnDesignCommonFl() 
{
	// TODO: Add your command handler code here
	
}

void CFLBillDlg::OnAutoMatchPWeldFl() 
{
	// TODO: Add your command handler code here
	
}

void CFLBillDlg::OnAutoMatchDWeldFl() 
{
	// TODO: Add your command handler code here
	
}

void CFLBillDlg::OnRclickTubeFlListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TVHITTESTINFO HitTestInfo;
	GetCursorPos(&HitTestInfo.pt);
	m_listCtrl.ScreenToClient(&HitTestInfo.pt);
	//m_listCtrl.HitTest(&HitTestInfo);
	//m_listCtrl.SetItemState(HitTestInfo.hItem,TVGN_CARET);

	ContextMenu(this,HitTestInfo.pt);

	*pResult = 0;
}

void CFLBillDlg::OnItemchangedTubeFlListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(pNMListView->iItem>=0)
		RefreshItemCtrlValue(pNMListView->iItem);
	
	*pResult = 0;
}

void CFLBillDlg::OnBnFlRedesignHasRib() 
{
	FLDESIGN_PROC_INFO design;
	double inner_radius_arr[2];
	UpdateData();
	if(m_listCtrl.GetSelectedCount()!=1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Select a flange record to be redesigned");
#else
		AfxMessageBox("需要选中一条待修正设计的法兰记录");
#endif
		return;
	}
	BOLTSIZE_ITEM boltSize;
	CString boltGradeStr;
	CComboBox *pCMBBoltGrade=(CComboBox*)GetDlgItem(IDC_CMB_BOLT_GRADE);
	pCMBBoltGrade->GetLBText(m_iBoltGrade,boltGradeStr);
	boltSize.gradeF=atof(boltGradeStr);
	boltSize.d=16;
	if(m_iBoltD==0)
		boltSize.d=16;
	else if(m_iBoltD==1)
		boltSize.d=20;
	else if(m_iBoltD==2)
		boltSize.d=24;
	//strcpy(pSelCouple->info.FL.pCoupleFlInfo->bolt_grade,boltGradeStr);
	flOpt.InitBoltSizeTbl(&boltSize,1);

	CComboBox *pCMBFlMat=(CComboBox*)GetDlgItem(IDC_CMB_FL_MATERIAL);
	char cSelMat=(char)pCMBFlMat->GetItemData(m_iFlMat);
	POSITION pos=m_listCtrl.GetFirstSelectedItemPosition();
	int iNow=m_listCtrl.GetNextSelectedItem(pos);
	CJointCouple *pSelCouple=(CJointCouple*)m_listCtrl.GetItemData(iNow);
	inner_radius_arr[0]=pSelCouple->pRod->GetWidth()*0.5;
	inner_radius_arr[1]=pSelCouple->pLinkRod->GetWidth()*0.5;
	flOpt.designStyleFlag.Clear();
	if(m_bSpecBoltNum)
	{
		flOpt.designStyleFlag.SetFlagDword(CFLSizeOptimizor::eSpecBoltNum);
		design.bolt_n=this->m_nBoltNum;
	}
	if(m_bSpecBoltCir)
	{
		flOpt.designStyleFlag.SetFlagDword(CFLSizeOptimizor::eSpecBoltDiameter);
		design.bolt_dia=this->m_fBoltCirDia;
	}
	if(m_bSpecFlDia)
	{
		flOpt.designStyleFlag.SetFlagDword(CFLSizeOptimizor::eSpecFlDiameter);
		design.fl_dia=this->m_fFlDia;
	}
	if(m_bSpecRibHeight)
	{
		flOpt.designStyleFlag.SetFlagDword(CFLSizeOptimizor::eSpecRibHeight);
		design.rib_height=this->m_nRibHeight;
		design.rib_height_couple=this->m_nCoupleRibHeight;
	}
	if(m_bSpecRibThick)
	{
		flOpt.designStyleFlag.SetFlagDword(CFLSizeOptimizor::eSpecRibThick);
		design.rib_thick=this->m_nRibThick;
	}
	pSelCouple->N=m_fAxisForce;
	pSelCouple->M=m_fAxisMoment;
	CConnectFL fl=flOpt.SelFLHasRib(&design,m_fAxisMoment,m_fAxisForce,inner_radius_arr,cSelMat);
	if(design.retCode==0)
	{
		if(pSelCouple->info.FL.pFLInfo)
		{
			strcpy(fl.m_sPartNo,pSelCouple->info.FL.pFLInfo->m_sPartNo);
			*pSelCouple->info.FL.pFLInfo=fl;
			strcpy(pSelCouple->info.FL.pFLInfo->m_sPartNo,this->m_sFlNo);
		}
		if(pSelCouple->info.FL.pCoupleFlInfo)
		{
			strcpy(fl.m_sPartNo,pSelCouple->info.FL.pFLInfo->m_sPartNo);
			*pSelCouple->info.FL.pCoupleFlInfo=fl;
			pSelCouple->info.FL.pCoupleFlInfo->m_nThickFL=fl.m_nCoupleThickFL;
			pSelCouple->info.FL.pCoupleFlInfo->m_nHeightRib=fl.m_nCoupleHeightRib;
			pSelCouple->info.FL.pCoupleFlInfo->m_nCoupleThickFL=fl.m_nThickFL;
			pSelCouple->info.FL.pCoupleFlInfo->m_nCoupleHeightRib=fl.m_nHeightRib;
			strcpy(pSelCouple->info.FL.pCoupleFlInfo->m_sPartNo,this->m_sCoupleFlNo);
		}
		//更新相关法兰信息
		if(pSelCouple->info.FL.pFLInfo==NULL)
			RefreshListViewItem(pSelCouple,iNow);
		else 
		{
			for(int i=0;i<m_listCtrl.GetItemCount();i++)
			{
				CJointCouple *pCouple=(CJointCouple*)m_listCtrl.GetItemData(i);
				if(pCouple->info.FL.pFLInfo&&pCouple->info.FL.pFLInfo==pSelCouple->info.FL.pFLInfo)
					RefreshListViewItem(pCouple,i);
			}
		}
		RefreshItemCtrlValue(iNow);
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("correction succeed!");
	}
	else if(design.retCode==1)
		AfxMessageBox("bolts strength is not enough");
	else if(design.retCode==2)
		AfxMessageBox("thickness of specified rib plate is too small to meet intensity needs");
	else if(design.retCode==3)
		AfxMessageBox("height of specified rib plate can not meet intensity needs ");
	else if(design.retCode==4)
		AfxMessageBox("the specified height of match flange rib plate cannot meet intensity needs");
#else
		MessageBox("修正成功！");
	}
	else if(design.retCode==1)
		AfxMessageBox("螺栓强度不足");
	else if(design.retCode==2)
		AfxMessageBox("指定肋板厚度小，不满足强度要求");
	else if(design.retCode==3)
		AfxMessageBox("指定肋板高度不满足强度要求");
	else if(design.retCode==4)
		AfxMessageBox("配对法兰肋板指定高度不满足强度要求");
#endif
}

void CFLBillDlg::OnBnFlRedesignNoneRib() 
{
	FLDESIGN_PROC_INFO design;
	double inner_radius_arr[2];
	UpdateData();
	if(m_listCtrl.GetSelectedCount()!=1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Select a flange record to be redesigned");
#else
		AfxMessageBox("需要选中一条待修正设计的法兰记录");
#endif
		return;
	}
	BOLTSIZE_ITEM boltSize;
	CString boltGradeStr;
	CComboBox *pCMBBoltGrade=(CComboBox*)GetDlgItem(IDC_CMB_BOLT_GRADE);
	pCMBBoltGrade->GetLBText(m_iBoltGrade,boltGradeStr);
	boltSize.gradeF=atof(boltGradeStr);
	boltSize.d=16;
	if(m_iBoltD==0)
		boltSize.d=16;
	else if(m_iBoltD==1)
		boltSize.d=20;
	else if(m_iBoltD==2)
		boltSize.d=24;
	flOpt.InitBoltSizeTbl(&boltSize,1);

	CComboBox *pCMBFlMat=(CComboBox*)GetDlgItem(IDC_CMB_FL_MATERIAL);
	char cSelMat=(char)pCMBFlMat->GetItemData(m_iFlMat);
	POSITION pos=m_listCtrl.GetFirstSelectedItemPosition();
	int iNow=m_listCtrl.GetNextSelectedItem(pos);
	CJointCouple *pSelCouple=(CJointCouple*)m_listCtrl.GetItemData(iNow);
	inner_radius_arr[0]=pSelCouple->pRod->GetWidth()*0.5;
	inner_radius_arr[1]=pSelCouple->pLinkRod->GetWidth()*0.5;
	if(inner_radius_arr[0]!=inner_radius_arr[1])
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("only flange without strength of the same diameter to connect tubes is supported currently!");
#else
		AfxMessageBox("目前只支持同直径的连接钢管间的无劲法兰设计！");
#endif
		return;
	}
	flOpt.designStyleFlag.Clear();
	if(m_bSpecBoltNum)
	{
		flOpt.designStyleFlag.SetFlagDword(CFLSizeOptimizor::eSpecBoltNum);
		design.bolt_n=this->m_nBoltNum;
	}
	if(m_bSpecBoltCir)
	{
		flOpt.designStyleFlag.SetFlagDword(CFLSizeOptimizor::eSpecBoltDiameter);
		design.bolt_dia=this->m_fBoltCirDia;
	}
	if(m_bSpecFlDia)
	{
		flOpt.designStyleFlag.SetFlagDword(CFLSizeOptimizor::eSpecFlDiameter);
		design.fl_dia=this->m_fFlDia;
	}
	pSelCouple->N=m_fAxisForce;
	pSelCouple->M=m_fAxisMoment;
	CConnectFL fl=flOpt.SelFLNoneRib(&design,m_fAxisMoment,m_fAxisForce,inner_radius_arr[0],cSelMat);
	if(design.retCode==0)
	{
		m_fCoupleSumWeight=pSelCouple->info.FL.pFLInfo->SumWeight();
		if(pSelCouple->info.FL.pFLInfo)
		{
			*pSelCouple->info.FL.pFLInfo=fl;
			strcpy(pSelCouple->info.FL.pFLInfo->m_sPartNo,this->m_sFlNo);
			//更新相关法兰信息
			if(pSelCouple->info.FL.pFLInfo==NULL)
				RefreshListViewItem(pSelCouple,iNow);
			else 
			{
				for(int i=0;i<m_listCtrl.GetItemCount();i++)
				{
					CJointCouple *pCouple=(CJointCouple*)m_listCtrl.GetItemData(i);
					if(pCouple->info.FL.pFLInfo&&pCouple->info.FL.pFLInfo==pSelCouple->info.FL.pFLInfo)
						RefreshListViewItem(pCouple,i);
				}
			}
			RefreshItemCtrlValue(iNow);
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox("correction succeed！");
#else
			MessageBox("修正成功！");
#endif
		}
		if(pSelCouple->info.FL.pCoupleFlInfo)
		{
			*pSelCouple->info.FL.pCoupleFlInfo=fl;
			strcpy(pSelCouple->info.FL.pCoupleFlInfo->m_sPartNo,this->m_sCoupleFlNo);
			pSelCouple->info.FL.pCoupleFlInfo->m_nThickFL=fl.m_nCoupleThickFL;
			pSelCouple->info.FL.pCoupleFlInfo->m_nCoupleThickFL=fl.m_nThickFL;
			strcpy(pSelCouple->info.FL.pCoupleFlInfo->m_sPartNo,this->m_sCoupleFlNo);
		}
	}
	else if(design.retCode==1)
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("bolts strength is not enough");
	else if(design.retCode==2)
		AfxMessageBox("thickness of specified rib plate is too small to meet intensity needs");
	else if(design.retCode==3)
		AfxMessageBox("height of specified rib plate cannot meet intensity needs");
	else if(design.retCode==4)
		AfxMessageBox("height of match flange rib plate cannot meet intensity needs");
#else
		AfxMessageBox("螺栓强度不足");
	else if(design.retCode==2)
		AfxMessageBox("指定肋板厚度小，不满足强度要求");
	else if(design.retCode==3)
		AfxMessageBox("指定肋板高度不满足强度要求");
	else if(design.retCode==4)
		AfxMessageBox("配对法兰肋板指定高度不满足强度要求");
#endif
}

void CFLBillDlg::OnBnFlSave() 
{
	UpdateData();
	CComboBox *pCMBFlMat=(CComboBox*)GetDlgItem(IDC_CMB_FL_MATERIAL);
	char cSelMat=(char)pCMBFlMat->GetItemData(m_iFlMat);
	POSITION pos=m_listCtrl.GetFirstSelectedItemPosition();
	int iNow=m_listCtrl.GetNextSelectedItem(pos);
	if(iNow<0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("No flange selected，unable to save!");
#else
		AfxMessageBox("未选中法兰项，无法保存!");
#endif
		return;
	}
	CJointCouple *pSelCouple=(CJointCouple*)m_listCtrl.GetItemData(iNow);
	if(pSelCouple->info.FL.pFLInfo)
	{
		strcpy(pSelCouple->info.FL.pFLInfo->m_sPartNo,m_sFlNo);
		if(pSelCouple->info.FL.pCoupleFlInfo)
			strcpy(pSelCouple->info.FL.pCoupleFlInfo->m_sPartNo,this->m_sCoupleFlNo);
		if(m_iBoltGrade==0)
			strcpy(pSelCouple->info.FL.pFLInfo->bolt_grade,"4.8");
		else if(m_iBoltGrade==1)
			strcpy(pSelCouple->info.FL.pFLInfo->bolt_grade,"5.8");
		else if(m_iBoltGrade==2)
			strcpy(pSelCouple->info.FL.pFLInfo->bolt_grade,"6.8");
		else //if(m_iBoltGrade==3)
			strcpy(pSelCouple->info.FL.pFLInfo->bolt_grade,"8.8");
		if(m_iBoltD==0)	//M16
			pSelCouple->info.FL.pFLInfo->bolt_d=16;
		else if(m_iBoltD==1)	//M20
			pSelCouple->info.FL.pFLInfo->bolt_d=20;
		else //if(m_iBoltD=2)	//M24
			pSelCouple->info.FL.pFLInfo->bolt_d=24;
		CComboBox *pCMBFlMat=(CComboBox*)GetDlgItem(IDC_CMB_FL_MATERIAL);
		pSelCouple->info.FL.pFLInfo->cMaterial=(char)pCMBFlMat->GetItemData(m_iFlMat);
		//pSelCouple->labelCouple=m_sCoupleFlNo;
		pSelCouple->info.FL.pFLInfo->m_fBoltRadius=m_fBoltCirDia;
		pSelCouple->info.FL.pFLInfo->bolt_n=m_nBoltNum;
		pSelCouple->info.FL.pFLInfo->m_nThickFL=m_nFlThick;
		pSelCouple->info.FL.pFLInfo->m_nCoupleThickFL=m_nCoupleFlThick;
		pSelCouple->info.FL.pFLInfo->m_nWidthFL=ftoi(m_fFlDia*0.5-pSelCouple->info.FL.pFLInfo->m_fInnerRadius);
		pSelCouple->info.FL.pFLInfo->m_nThickRib=m_nRibThick;
		pSelCouple->info.FL.pFLInfo->m_nHeightRib=m_nRibHeight;
		pSelCouple->info.FL.pFLInfo->m_nCoupleHeightRib=m_nCoupleRibHeight;
		if(pSelCouple->info.FL.pCoupleFlInfo)
		{
			pSelCouple->info.FL.pCoupleFlInfo->m_nHeightRib=m_nCoupleRibHeight;
			pSelCouple->info.FL.pCoupleFlInfo->m_nCoupleThickFL=m_nCoupleFlThick;
			pSelCouple->info.FL.pCoupleFlInfo->m_nCoupleThickFL=m_nFlThick;
			pSelCouple->info.FL.pCoupleFlInfo->m_nCoupleHeightRib=m_nRibHeight;
		}
	}
	pSelCouple->N=m_fAxisForce;
	pSelCouple->M=m_fAxisMoment;
	//更新相关法兰信息
	if(pSelCouple->info.FL.pFLInfo==NULL)
		RefreshListViewItem(pSelCouple,iNow);
	else 
	{
		for(int i=0;i<m_listCtrl.GetItemCount();i++)
		{
			CJointCouple *pCouple=(CJointCouple*)m_listCtrl.GetItemData(i);
			if(pCouple->info.FL.pFLInfo&&pCouple->info.FL.pFLInfo==pSelCouple->info.FL.pFLInfo)
				RefreshListViewItem(pCouple,i);
		}
	}
}

void CFLBillDlg::OnBnFlRedesignAll() 
{
	//统计生成法兰匹配对
	flOpt.Clear();
	CJointCouple::StatJointLinkPart(&flOpt.listCouple,CLS_LINETUBE);
	CJointCouple *pCouple;
	//确定法兰设计依据力学分析单元
	for(CElement* pElem=postprocessor.hashElems.GetFirst();pElem;pElem=postprocessor.hashElems.GetNext())
	{
		if(pElem->pStart!=pElem->pOrgPole->pStart&&pElem->pEnd!=pElem->pOrgPole->pEnd)
			continue;
		if(pElem->pOrgPole->size_idClassType!=CLS_LINETUBE)
			continue;
		for(pCouple=flOpt.listCouple.GetFirst();pCouple;pCouple=flOpt.listCouple.GetNext())
		{
			if(pCouple->pLinkRod==pElem->pOrgPole)
			{
				if( (pElem->pStart==pElem->pOrgPole->pStart&&pElem->pStart==pCouple->pNode)||
					(pElem->pEnd==pElem->pOrgPole->pEnd&&pElem->pEnd==pCouple->pNode))
				{
					pCouple->N=pElem->MaxTension.N;
					pCouple->M=0;	//暂不考虑弯矩
				}
			}
		}
	}
	flOpt.GenFLSet();
	for(pCouple=flOpt.listCouple.GetFirst();pCouple;pCouple=flOpt.listCouple.GetNext())
	{
		if(pCouple->pRod->size_idClassType!=CLS_LINETUBE)
			continue;
		for(CConnectFL *pConnFl=Ta.hashConnectFl.GetFirst();pConnFl;pConnFl=Ta.hashConnectFl.GetNext())
		{
			if( (pCouple->pRod->pStart==pCouple->pNode&&stricmp(pCouple->pRod->connectStart.flNo,pConnFl->m_sPartNo)==0)||
				(pCouple->pRod->pEnd==pCouple->pNode&&stricmp(pCouple->pRod->connectEnd.flNo,pConnFl->m_sPartNo)==0))
			{
				pCouple->info.FL.pFLInfo=pConnFl;
				break;
			}
		}
#ifdef __LDS_
		for(CLDSPart *pFl=Ta.Parts.GetFirst(CLS_PARAMPLATE);pFl;pFl=Ta.Parts.GetNext(CLS_PARAMPLATE))
		{
			if(pCouple->pRod->GetClassTypeId()!=CLS_LINETUBE)
				continue;
			CLDSLineTube *pRodTube=(CLDSLineTube*)pCouple->pRod;
			if( (pRodTube->pStart==pCouple->pNode&&pRodTube->m_tJointStart.hLinkObj==pFl->handle)||
				(pRodTube->pEnd==pCouple->pNode&&pRodTube->m_tJointEnd.hLinkObj==pFl->handle))
			{
				pCouple->info.FL.pFL=(CLDSParamPlate*)pFl;
				break;
			}
		}
#endif
	}
	OnChangeEIncSegStr();
}
#endif
