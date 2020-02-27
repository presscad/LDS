// BatchCreatePartDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "BatchCreatePartDlg.h"
#include "GlobalFunc.h"
#include "Query.h"
#include "TubeEndLjStyleDlg.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CBatchCreatePartDlg dialog


CBatchCreatePartDlg::CBatchCreatePartDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBatchCreatePartDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchCreatePartDlg)
	m_bUseGuige = TRUE;
	m_bUsePartNo = FALSE;
	m_iCreatePartType = 0;
	m_iEndLj = 2;
	m_iTubeGuige = 0;
	m_iTubeMaterial = 0;
	m_fEndOddment = 0.0;
	m_fStartOddment = 0.0;
	m_hTubeHandle = 0;
	m_sEndLj = _T("始/终端");
	m_sTubePartNo = _T("*");
	m_sTubeSegStr = _T("*");
	m_sTubeGuiGe =_T("89X4");
	m_bAutoCalcOddment = FALSE;
	//}}AFX_DATA_INIT
	m_pListCtrl=NULL;
	m_arrHeaderLabel.Add("句柄");
	m_arrHeaderLabel.Add("编号");
	m_arrHeaderLabel.Add("端连接");
	m_arrHeaderLabel.Add("始端正负头");
	m_arrHeaderLabel.Add("终端正负头");
	listViewComparer.SetMaxColumnCount(5);
}

void CBatchCreatePartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchCreatePartDlg)
	DDX_Check(pDX, IDC_CHK_USE_GUIGE, m_bUseGuige);
	DDX_Check(pDX, IDC_CHK_USE_PART_NO, m_bUsePartNo);
	DDX_CBIndex(pDX, IDC_CMB_CREATE_PART_TYPE, m_iCreatePartType);
	DDX_CBIndex(pDX, IDC_CMB_END_LJ, m_iEndLj);
	DDX_CBIndex(pDX, IDC_CMB_TUBE_GUIGE, m_iTubeGuige);
	DDX_CBIndex(pDX, IDC_CMB_TUBE_MATERIAL, m_iTubeMaterial);
	DDX_Text(pDX, IDC_E_END_ODDMENT, m_fEndOddment);
	DDX_Text(pDX, IDC_E_START_ODDMENT, m_fStartOddment);
	DDX_Text(pDX, IDC_E_TUBE_HANDLE, m_hTubeHandle);
	DDX_Text(pDX, IDC_E_TUBE_PART_NO, m_sTubePartNo);
	DDX_Text(pDX, IDC_E_TUBE_SEG_STR, m_sTubeSegStr);
	DDX_CBString(pDX, IDC_CMB_TUBE_GUIGE, m_sTubeGuiGe);
	DDX_CBString(pDX, IDC_CMB_END_LJ, m_sEndLj);
	DDX_Check(pDX, IDC_CHK_AUTO_CALC_ODDMENT, m_bAutoCalcOddment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBatchCreatePartDlg, CDialog)
	//{{AFX_MSG_MAP(CBatchCreatePartDlg)
	ON_BN_CLICKED(IDC_CHK_USE_PART_NO, OnChkUsePartNo)
	ON_BN_CLICKED(IDC_CHK_USE_GUIGE, OnChkUseGuige)
	ON_BN_CLICKED(IDC_BN_MODFIY_ITEM, OnModfiyItem)
	ON_BN_CLICKED(IDC_BN_ADD_ITEM, OnAddItem)
	ON_EN_CHANGE(IDC_E_TUBE_SEG_STR, OnChangeTubeSegStr)
	ON_EN_CHANGE(IDC_E_TUBE_PART_NO, OnChangeTubePartNo)
	ON_CBN_SELCHANGE(IDC_CMB_TUBE_GUIGE, OnSelchangeTubeGuige)
	ON_CBN_SELCHANGE(IDC_CMB_TUBE_MATERIAL, OnSelchangeTubeMaterial)
	ON_NOTIFY(LVN_KEYDOWN, IDC_TUBE_LIST, OnKeydownTubeList)
	ON_BN_CLICKED(IDC_CHK_AUTO_CALC_ODDMENT, OnAutoCalcOddment)
	ON_BN_CLICKED(IDC_BN_UPDATE_TUBE_LIST, OnUpdateTubeList)
	ON_NOTIFY(NM_DBLCLK, IDC_TUBE_LIST, OnDblclkTubeList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_TUBE_LIST, OnKeydownTubeList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_TUBE_LIST, OnColumnclickTubeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchCreatePartDlg message handlers

BOOL CBatchCreatePartDlg::OnInitDialog() 
{
	CComboBox*pCMB = (CComboBox*)GetDlgItem(IDC_CMB_TUBE_MATERIAL);
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(0);
	pCMB = (CComboBox*)GetDlgItem(IDC_CMB_TUBE_GUIGE);
	AddTubeRecord(pCMB);
	pCMB->SetCurSel(0);
	pCMB = (CComboBox*)GetDlgItem(IDC_CMB_END_LJ);
	pCMB->AddString("始端");
	pCMB->AddString("终端");
	pCMB->AddString("始/终端");
	pCMB->SetCurSel(2);
	pCMB = (CComboBox*)GetDlgItem(IDC_CMB_CREATE_PART_TYPE);
	pCMB->Clear();
	pCMB->AddString("槽型插板");
	pCMB->AddString("U型插板");
	pCMB->AddString("十字插板");
	pCMB->SetCurSel(0);
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_TUBE_LIST);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	m_pListCtrl->DeleteAllItems();
	RefreshTubeList();
	m_bUseGuige=TRUE;
	m_bUsePartNo=FALSE;
	GetDlgItem(IDC_CMB_TUBE_GUIGE)->EnableWindow(m_bUseGuige);
	GetDlgItem(IDC_CMB_TUBE_MATERIAL)->EnableWindow(m_bUseGuige);
	GetDlgItem(IDC_E_TUBE_PART_NO)->EnableWindow(m_bUsePartNo);
	m_bAutoCalcOddment=FALSE;
	GetDlgItem(IDC_E_START_ODDMENT)->EnableWindow(!m_bAutoCalcOddment);
	GetDlgItem(IDC_E_END_ODDMENT)->EnableWindow(!m_bAutoCalcOddment);
	CDialog::OnInitDialog();
	return TRUE;  
}

void CBatchCreatePartDlg::RefreshTubeList()
{
	CHashTable<long>segTbl;
	long seg_i;
	char seg_str[200]="";
	_snprintf(seg_str,199,"%s",m_sTubeSegStr);
	if(seg_str[0]=='*')
		segTbl.Empty();
	else
	{
		segTbl.CreateHashTable(160);
		for(seg_i=FindAdaptNo(seg_str,",","-");seg_i;seg_i=FindAdaptNo(NULL,",","-"))
			segTbl.SetValueAt(seg_i,seg_i);
	}
	CStringArray str_arr;
	str_arr.SetSize(5);
	m_pListCtrl->DeleteAllItems();
	double tube_d=0,tube_t=0;
	GetDlgItem(IDC_CMB_TUBE_GUIGE)->GetWindowText(m_sTubeGuiGe);
	restore_JG_guige(m_sTubeGuiGe,tube_d,tube_t);
	char cMat=steelmat_tbl[m_iTubeMaterial].cBriefMark;
	for(CLDSPart *pPart=Ta.Parts.GetFirst(CLS_LINETUBE);pPart;pPart=Ta.Parts.GetNext(CLS_LINETUBE))
	{
		if(pPart->GetClassTypeId()!=CLS_LINETUBE)
			continue;
		CLDSLineTube *pLineTube=(CLDSLineTube*)pPart;
		if(m_bUsePartNo&&(m_sTubePartNo.CompareNoCase(pPart->sPartNo)!=0&&m_sTubePartNo.CompareNoCase("*")!=0))
			continue;
		if(m_bUseGuige&&(pLineTube->GetDiameter()!=tube_d||pLineTube->GetThick()!=tube_t||pLineTube->cMaterial!=cMat))
			continue;
		if(segTbl.GetHashTableSize()>0&&!segTbl.GetValueAt(pPart->iSeg,seg_i))
			continue;	//分段号不在当前所选范围内
		if(pLineTube->m_tJointStart.hLinkObj>0X20&&pLineTube->m_tJointEnd.hLinkObj>0X20)
			continue;	//已存在端连接
		str_arr[0].Format("0X%X",pLineTube->handle);
		str_arr[1]=pLineTube->sPartNo;
		CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;
		if(pLineTube->m_tJointStart.hLinkObj>0x20&&(pLineTube->m_tJointStart.type>1))
			pStartParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		if(pLineTube->m_tJointEnd.hLinkObj>0x20&&(pLineTube->m_tJointEnd.type>1))
			pEndParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		if(pStartParamPlate==NULL&&pEndParamPlate==NULL)
			str_arr[2]="始/终端";
		else if(pStartParamPlate==NULL)
			str_arr[2]="始端";
		else if(pEndParamPlate==NULL)
			str_arr[2]="终端";
		str_arr[3].Format("%.1f",pLineTube->startOdd());
		str_arr[4].Format("%.1f",pLineTube->endOdd());
		AppendRecord(-1,str_arr);
	}
}

void CBatchCreatePartDlg::RefreshListViewHeader()
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
		if(i==0)
			lvc.cx=47;
		else if(i==1)
			lvc.cx=57;
		else if(i==2)
			lvc.cx=60;
		else
			lvc.cx = 80;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}

int CBatchCreatePartDlg::AppendRecord(int iItem, CStringArray &str_arr)
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

void CBatchCreatePartDlg::OnColumnclickTubeList(NMHDR* pNMHDR, LRESULT* pResult) 
{	//排序还有问题 
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	listViewComparer.m_pListCtrl = m_pListCtrl;
	listViewComparer.m_iColumn = pNMListView->iSubItem;
	listViewComparer.sortOrderAscending[pNMListView->iSubItem]=!listViewComparer.sortOrderAscending[pNMListView->iSubItem];
	m_pListCtrl->SortItems(CompareItemProc,(DWORD)&listViewComparer);
	*pResult = 0;
}

void CBatchCreatePartDlg::OnChangeTubeSegStr() 
{
	UpdateData();
	m_pListCtrl->DeleteAllItems();
	RefreshTubeList();
}

void CBatchCreatePartDlg::OnChangeTubePartNo() 
{
	UpdateData();
	m_pListCtrl->DeleteAllItems();
	RefreshTubeList();
}

void CBatchCreatePartDlg::OnSelchangeTubeGuige() 
{
	UpdateData();
	m_pListCtrl->DeleteAllItems();
	RefreshTubeList();
}

void CBatchCreatePartDlg::OnSelchangeTubeMaterial() 
{
	UpdateData();
	m_pListCtrl->DeleteAllItems();
	RefreshTubeList();
}

void CBatchCreatePartDlg::OnChkUsePartNo() 
{
	UpdateData();
	m_bUseGuige=!m_bUsePartNo;
	GetDlgItem(IDC_CMB_TUBE_GUIGE)->EnableWindow(m_bUseGuige);
	GetDlgItem(IDC_CMB_TUBE_MATERIAL)->EnableWindow(m_bUseGuige);
	GetDlgItem(IDC_E_TUBE_PART_NO)->EnableWindow(m_bUsePartNo);
	m_pListCtrl->DeleteAllItems();
	RefreshTubeList();
	UpdateData(FALSE);
}

void CBatchCreatePartDlg::OnChkUseGuige() 
{
	UpdateData();
	m_bUsePartNo=!m_bUseGuige;
	GetDlgItem(IDC_CMB_TUBE_GUIGE)->EnableWindow(m_bUseGuige);
	GetDlgItem(IDC_CMB_TUBE_MATERIAL)->EnableWindow(m_bUseGuige);
	GetDlgItem(IDC_E_TUBE_PART_NO)->EnableWindow(m_bUsePartNo);
	m_pListCtrl->DeleteAllItems();
	RefreshTubeList();
	UpdateData(FALSE);
}

void CBatchCreatePartDlg::OnModfiyItem() 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		ModfiyItemInfo(iCurSel);
	}
}

void CBatchCreatePartDlg::OnAddItem() 
{
	CString ss;
	CStringArray str_arr;
	UpdateData();
	GetDlgItem(IDC_E_TUBE_HANDLE)->GetWindowText(ss);
	CString sHandle;
	sHandle.Format("0X%s",ss);
	str_arr.Add(sHandle);
	long handle=0;
	sscanf(ss,"%X",&handle);
	CLDSLineTube *pLineTube=(CLDSLineTube*)Ta.Parts.FromHandle(handle,CLS_LINETUBE);
	if(pLineTube==NULL)
	{
		MessageBox("句柄输入错误，请重新输入!");
		return;
	}
	int n=m_pListCtrl->GetItemCount();
	for(int i=0;i<n;i++)
	{
		char tem_str[100];
		m_pListCtrl->GetItemText(i,0,tem_str,100);
		long handle=0;
		sscanf(tem_str,"%X",&handle);
		if(handle==pLineTube->handle)
		{
			MessageBox("不能添加重复记录!");
			return;
		}
	}
	ss.Format("%s",pLineTube->sPartNo);
	str_arr.Add(ss);
	GetDlgItem(IDC_CMB_END_LJ)->GetWindowText(ss);
	str_arr.Add(ss);
	CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;
	if(pLineTube->m_tJointStart.hLinkObj>0x20&&(pLineTube->m_tJointStart.type>1))
		pStartParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
	if(pLineTube->m_tJointEnd.hLinkObj>0x20&&(pLineTube->m_tJointEnd.type>1))
		pEndParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
	if((m_iEndLj==2||m_iEndLj==0)&&pStartParamPlate)
	{
		MessageBox("该钢管始端已存在端连接!");
		return;
	}
	else if((m_iEndLj==2||m_iEndLj==1)&&pEndParamPlate)
	{
		MessageBox("该钢管终端已存在端连接!");
		return;
	}
	GetDlgItem(IDC_E_START_ODDMENT)->GetWindowText(ss);
	str_arr.Add(ss);
	GetDlgItem(IDC_E_END_ODDMENT)->GetWindowText(ss);
	str_arr.Add(ss);
	AppendRecord(-1,str_arr);
}

static void CreateBoltInPart(CLDSPart *pPart,CLDSParamPlate *pSrcPlate)
{
	CLsRefList *pLsRefList=pSrcPlate->GetLsRefList();
	if(pPart==NULL||pLsRefList==NULL)
		return;
	CLDSPlate *pPlate=NULL;
	CLDSParamPlate *pParamPlate=NULL;
	if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		pParamPlate=(CLDSParamPlate*)pPart;
	else if(pPart->GetClassTypeId()==CLS_PLATE)
		pPlate=(CLDSPlate*)pPart;
	for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
	{
		CLDSBolt *pBolt=(CLDSBolt*)Ta.Parts.append(CLS_BOLT);
		pBolt->CopyProperty(pLsRef->GetLsPtr());
		pBolt->cfgword=pPart->cfgword;	//调整螺栓配材号与基准构件配材号一致
		pBolt->iSeg=pPart->iSeg;		//调整螺栓段号与基准构件一致
		pBolt->layer(2)=pPart->layer(2);	//调整螺栓的图层名
		pBolt->des_base_pos=pLsRef->GetLsPtr()->des_base_pos;
		pBolt->des_base_pos.hPart=pPart->handle;
		pBolt->des_work_norm=(*pLsRef)->des_work_norm;
		pBolt->des_work_norm.hVicePart=pPart->handle;
		pBolt->des_base_pos.norm_offset.EmptyThick();
		pBolt->des_base_pos.norm_offset.AddThick(ftoi(-0.5*pPart->GetThick()));
		pBolt->EmptyL0DesignPara();	//清空螺栓设计参数
		double L;
		if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
		{	//十字插板
			pParamPlate->GetDesignItemValue('L',&L);
			//螺栓在十字插板范围内修改螺栓通厚
			if(L>pBolt->des_base_pos.len_offset_dist&&(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==21))	
				pBolt->AddL0Thick(pPart->handle,TRUE);
		}
		else
			pBolt->AddL0Thick(pPart->handle,TRUE);
		pBolt->CalGuigeAuto();
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		CLsRef *pTempLsRef=NULL;
		if(pParamPlate)
			pTempLsRef=pParamPlate->AppendLsRef(pBolt->GetLsRef());
		else if(pPlate)
			pTempLsRef=pPlate->AppendLsRef(pBolt->GetLsRef());
		if(pTempLsRef)
			pTempLsRef->start1_end2_mid0=pLsRef->start1_end2_mid0;	//标记螺栓所在位置
		pBolt->SetModified();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
}

CLDSParamPlate *CBatchCreatePartDlg::CloneParamPlate(CLDSLineTube *pLineTube,CLDSParamPlate *pSrcParamPlate,BOOL bStartEnd,BOOL bAutoCalcOddment)
{
	CLDSParamPlate *pParamPlate=(CLDSParamPlate*)Ta.Parts.append(CLS_PARAMPLATE);
	pParamPlate->CopyProperty(pSrcParamPlate);
	pParamPlate->m_hPartWeldParent=pLineTube->handle;
	pParamPlate->layer(2)=pLineTube->layer(2); //调整图层名
	CLDSLineTube *pSrcLineTube=(CLDSLineTube*)Ta.Parts.FromHandle(pSrcParamPlate->m_hPartWeldParent,CLS_LINETUBE);
	if(bStartEnd)
	{
		pLineTube->m_tJointStart.type=pParamPlate->m_iParamType;
		if(pParamPlate)
			pLineTube->m_tJointStart.hLinkObj=pParamPlate->handle;
		//pLineTube->desStartPos.fNormOffset = m_fTubeNormOffset;	//钢管基准面法向偏移量
		if(bAutoCalcOddment)
		{	//自动计算正负头
			pLineTube->desStartOdd.m_iOddCalStyle=0;
			pLineTube->CalStartOddment();
		}
		pLineTube->DesignStartJoint();
	}
	else
	{
		pLineTube->m_tJointEnd.type=pParamPlate->m_iParamType;
		if(pParamPlate)
			pLineTube->m_tJointEnd.hLinkObj=pParamPlate->handle;
		//pLineTube->desEndPos.fNormOffset = m_fTubeNormOffset;	//钢管基准面法向偏移量
		if(bAutoCalcOddment)
		{	//自动计算正负头
			pLineTube->desEndOdd.m_iOddCalStyle=0;
			pLineTube->CalEndOddment();
		}
		pLineTube->DesignEndJoint();
	}
	pLineTube->SetModified();
	pParamPlate->SetModified();
	pLineTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pLineTube->GetSolidPartObject());
	g_pSolidDraw->NewSolidPart(pParamPlate->GetSolidPartObject());
	//生成参数板上的螺栓
	if(!pSrcParamPlate->m_bStdPart)
		CreateBoltInPart(pParamPlate,pSrcParamPlate);
	else 
	{	//根据参数化板编号类型查找标准模型
		CLDSStdPartModel stdPartModel;
		if(pSrcParamPlate->GetStdPartModel(&stdPartModel))
		{
			stdPartModel.CreateBoltInPlate(pParamPlate);//布置螺栓U/C/X型插板上的螺栓
			//绘制标准构件上的螺栓
			for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
		}
	}
	//生成附带构件(法兰上的肘板,U型板或卷边板上的垂向加劲板/封头板)
	double plate_thick=0;
	for(ATTACHPART_ITEM *pItem=pSrcParamPlate->GetFirstAttachPart();pItem;pItem=pSrcParamPlate->GetNextAttachPart())
	{
		if(pSrcParamPlate->m_iParamType==TYPE_UEND||pSrcParamPlate->m_iParamType==TYPE_ROLLEND||pSrcParamPlate->m_iParamType==TYPE_XEND)
		{
			CLDSPart *pPart=Ta.Parts.FromHandle(pItem->h,pItem->idClsType);
			if(pPart==NULL)
				continue;
			UCS_STRU ucs = pPart->ucs;
			coord_trans(ucs.origin,pSrcParamPlate->ucs,FALSE);
			vector_trans(ucs.axis_x,pSrcParamPlate->ucs,FALSE);
			vector_trans(ucs.axis_y,pSrcParamPlate->ucs,FALSE);
			vector_trans(ucs.axis_z,pSrcParamPlate->ucs,FALSE);
			coord_trans(ucs.origin,pParamPlate->ucs,TRUE);
			vector_trans(ucs.axis_x,pParamPlate->ucs,TRUE);
			vector_trans(ucs.axis_y,pParamPlate->ucs,TRUE);
			vector_trans(ucs.axis_z,pParamPlate->ucs,TRUE);
			if(pItem->idClsType==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pSrcVertPlate = (CLDSParamPlate*)pPart;
				if(pSrcVertPlate->m_iParamType!=TYPE_VERTPLATE&&pSrcVertPlate->m_iParamType!=TYPE_ENDPLATE)
					continue;
				if(pSrcVertPlate->m_iParamType==TYPE_VERTPLATE)
					continue;
				if(pSrcVertPlate->m_iParamType==TYPE_ENDPLATE)
					continue;
				CLDSParamPlate *pVertPlate = (CLDSParamPlate*)Ta.Parts.append(CLS_PARAMPLATE);
				pVertPlate->CopyProperty(pSrcVertPlate);
				pVertPlate->m_hPartWeldParent=pParamPlate->handle;
				pVertPlate->ucs = ucs;
				pVertPlate->DesignPlate();
				pVertPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pVertPlate->GetSolidPartObject());
				//将垂向加劲板/封头板存放在U型板/卷边板的依附构件链表里
				pParamPlate->AppendAttachPart(pVertPlate);
			}
			else if(pItem->idClsType==CLS_PLATE)
			{
				CLDSPlate *pPlate=(CLDSPlate*)pPart;
				CLDSPlate *pMirPlate = (CLDSPlate*)Ta.Parts.append(CLS_PLATE);
				pMirPlate->CopyProperty(pPlate);
				pMirPlate->m_hPartWeldParent=pParamPlate->handle;
				pMirPlate->ucs = ucs;
				for(PROFILE_VERTEX *pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
					pMirPlate->vertex_list.append(*pVertex);
				pMirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pMirPlate->GetSolidPartObject());
				//将垂向加劲板/封头板存放在U型板/卷边板的依附构件链表里
				pParamPlate->AppendAttachPart(pMirPlate);
				if(pParamPlate->m_iParamType==TYPE_XEND)
				{
					SCOPE_STRU scope;
					PROFILE_VERTEX *pVertex=NULL;
					plate_thick=pMirPlate->GetThick();
					for(pVertex=pMirPlate->vertex_list.GetFirst();pVertex;pVertex=pMirPlate->vertex_list.GetNext())
						scope.VerifyVertex(pVertex->vertex);
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{
						f3dPoint ls_pos=(*pLsRef)->ucs.origin;
						coord_trans(ls_pos,pMirPlate->ucs,FALSE);
						ls_pos.z=0;
						if(scope.IsIncludePoint(ls_pos))
						{
							pMirPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pMirPlate->handle,TRUE);
							(*pLsRef)->CalGuigeAuto();
						}
					}
				}
			}
		}
	}
	if(pParamPlate->m_iParamType==TYPE_XEND)
	{
		for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
		{
			(*pLsRef)->des_base_pos.norm_offset.AddThick(-(int)plate_thick);
			(*pLsRef)->correct_worknorm();
			(*pLsRef)->correct_pos();
			(*pLsRef)->CalGuigeAuto();
			(*pLsRef)->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
		}
	}
	return pParamPlate;
}

void CBatchCreatePartDlg::OnOK() 
{
	CDialog::OnOK();
	int n=m_pListCtrl->GetItemCount();
	static CTubeEndLjStyleDlg end_lj_dlg;
	CLDSParamPlate *pParamPlate=NULL;
	for(int i=0;i<n;i++)
	{
		CString sEndLj;
		int start0_end1_both2=-1;
		char tem_str[100];
		double start_oddment=0,end_oddment=0;
		m_pListCtrl->GetItemText(i,0,tem_str,100);
		long handle=0;
		sscanf(tem_str,"%X",&handle);
		CLDSLineTube *pLineTube=(CLDSLineTube*)Ta.Parts.FromHandle(handle,CLS_LINETUBE);
		if(pLineTube==NULL)
			continue;
		m_pListCtrl->GetItemText(i,2,tem_str,100);
		sEndLj.Format("%s",tem_str);
		if(sEndLj.CompareNoCase("始端")==0)
			start0_end1_both2=0;
		else if(sEndLj.CompareNoCase("终端")==0)
			start0_end1_both2=1;
		else if(sEndLj.CompareNoCase("始/终端")==0)
			start0_end1_both2=2;
		else
			continue;
		m_pListCtrl->GetItemText(i,3,tem_str,100);
		start_oddment=atoi(tem_str);
		m_pListCtrl->GetItemText(i,4,tem_str,100);
		end_oddment=atoi(tem_str);
		//重新设置钢管正负头
		pLineTube->SetStartOdd(start_oddment);
		pLineTube->SetEndOdd(end_oddment);
		if(i==0)
		{	
			end_lj_dlg.m_bBatchCreatePart=TRUE;
			end_lj_dlg.m_pLineTube=pLineTube;
			if(m_iCreatePartType==0)		//槽型插板
				end_lj_dlg.m_iTubeEndLjStyle=4;
			else if(m_iCreatePartType==1)	//U型插板
				end_lj_dlg.m_iTubeEndLjStyle=3;
			else //if(m_CreatePartType==2)	//十字插板
				end_lj_dlg.m_iTubeEndLjStyle=5;
			if(start0_end1_both2==1)
			{
				end_lj_dlg.m_bStartEnd=FALSE;
				end_lj_dlg.m_fOddment=pLineTube->endOdd();
			}
			else
			{
				end_lj_dlg.m_bStartEnd=TRUE;
				end_lj_dlg.m_fOddment=pLineTube->startOdd();
			}
			end_lj_dlg.m_fParamR = pLineTube->GetDiameter()*0.5 + g_sysPara.TubeFlDist;	//钢管与法兰的间隙
			end_lj_dlg.m_fWorkPlanePosX = pLineTube->m_tJointEnd.cutPlanePos.x;
			end_lj_dlg.m_fWorkPlanePosY = pLineTube->m_tJointEnd.cutPlanePos.y;
			end_lj_dlg.m_fWorkPlanePosZ = pLineTube->m_tJointEnd.cutPlanePos.z;
			end_lj_dlg.m_fWorkPlaneNormX = pLineTube->m_tJointEnd.cutPlaneNorm.x;
			end_lj_dlg.m_fWorkPlaneNormY = pLineTube->m_tJointEnd.cutPlaneNorm.y;
			end_lj_dlg.m_fWorkPlaneNormZ = pLineTube->m_tJointEnd.cutPlaneNorm.z;
			if(end_lj_dlg.DoModal()==IDOK)
			{
				pLineTube->m_tJointEnd.cutPlanePos.x =end_lj_dlg.m_fWorkPlanePosX;
				pLineTube->m_tJointEnd.cutPlanePos.y =end_lj_dlg.m_fWorkPlanePosY;
				pLineTube->m_tJointEnd.cutPlanePos.z =end_lj_dlg.m_fWorkPlanePosZ;
				pLineTube->m_tJointEnd.cutPlaneNorm.x=end_lj_dlg.m_fWorkPlaneNormX;
				pLineTube->m_tJointEnd.cutPlaneNorm.y=end_lj_dlg.m_fWorkPlaneNormY;
				pLineTube->m_tJointEnd.cutPlaneNorm.z=end_lj_dlg.m_fWorkPlaneNormZ;
				pLineTube->m_tJointEnd.type=end_lj_dlg.m_iTubeEndLjStyle;
			}
			if(m_bAutoCalcOddment&&end_lj_dlg.m_bStartEnd)
			{
				pLineTube->desStartOdd.m_iOddCalStyle=0;
				pLineTube->CalStartOddment();
				pLineTube->DesignStartJoint();
			}
			else if(m_bAutoCalcOddment&&!end_lj_dlg.m_bStartEnd)
			{
				pLineTube->desEndOdd.m_iOddCalStyle=0;
				pLineTube->CalEndOddment();
				pLineTube->DesignEndJoint();
			} 
			pParamPlate=end_lj_dlg.GetParamPlate();
			if(pParamPlate==NULL)
				return;
			if(m_bAutoCalcOddment)
			{	//自动计算正负头
				pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pParamPlate->GetSolidPartObject());
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//自动计算正负头后重新设置螺栓的位置
					(*pLsRef)->SetModified();
					(*pLsRef)->correct_pos();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
			if(start0_end1_both2==2&&pParamPlate)	//生成钢管另一端的插板
				CloneParamPlate(pLineTube,pParamPlate,!end_lj_dlg.m_bStartEnd,m_bAutoCalcOddment);
		}
		else
		{
			if(pParamPlate&&(start0_end1_both2==0||start0_end1_both2==2))	//生成钢管始端的插板
				CloneParamPlate(pLineTube,pParamPlate,TRUE,m_bAutoCalcOddment);
			if(pParamPlate&&(start0_end1_both2==1||start0_end1_both2==2))	//生成钢管终端的插板
				CloneParamPlate(pLineTube,pParamPlate,FALSE,m_bAutoCalcOddment);
		}
	}
	g_pSolidDraw->Draw();
}

void CBatchCreatePartDlg::OnAutoCalcOddment() 
{
	UpdateData();
	GetDlgItem(IDC_E_START_ODDMENT)->EnableWindow(!m_bAutoCalcOddment);
	GetDlgItem(IDC_E_END_ODDMENT)->EnableWindow(!m_bAutoCalcOddment);
	UpdateData(FALSE);
}

void CBatchCreatePartDlg::OnUpdateTubeList() 
{
	UpdateData();
	m_pListCtrl->DeleteAllItems();
	RefreshTubeList();
	UpdateData(FALSE);
}

void CBatchCreatePartDlg::ModfiyItemInfo(int iCurSel)
{
	UpdateData();
	CString ss;
	char tem_str[100]=" ";
	m_pListCtrl->GetItemText(iCurSel,0,tem_str,100);
	long handle=0;
	sscanf(tem_str,"%X",&handle);
	CLDSLineTube *pLineTube=(CLDSLineTube*)Ta.Parts.FromHandle(handle,CLS_LINETUBE);
	if(pLineTube==NULL)
	{
		MessageBox("存在不合法数据!");
		return;
	}
	GetDlgItem(IDC_CMB_END_LJ)->GetWindowText(ss);
	CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;
	if(pLineTube->m_tJointStart.hLinkObj>0x20&&(pLineTube->m_tJointStart.type>1))
		pStartParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
	if(pLineTube->m_tJointEnd.hLinkObj>0x20&&(pLineTube->m_tJointEnd.type>1))
		pEndParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
	if((m_iEndLj==2||m_iEndLj==0)&&pStartParamPlate)
	{
		MessageBox("该钢管始端已存在端连接!");
		return;
	}
	else if((m_iEndLj==2||m_iEndLj==1)&&pEndParamPlate)
	{
		MessageBox("该钢管终端已存在端连接!");
		return;
	}
	m_pListCtrl->SetItemText(iCurSel,2,ss);
	GetDlgItem(IDC_E_START_ODDMENT)->GetWindowText(ss);
	m_pListCtrl->SetItemText(iCurSel,3,ss);
	GetDlgItem(IDC_E_END_ODDMENT)->GetWindowText(ss);
	m_pListCtrl->SetItemText(iCurSel,4,ss);
}

void CBatchCreatePartDlg::OnDblclkTubeList(NMHDR* pNMHDR, LRESULT* pResult)
{
	CString ss;
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
		{
			ss=m_pListCtrl->GetItemText(iCurSel,0);
			m_hTubeHandle=atol(ss);	
			m_sEndLj=m_pListCtrl->GetItemText(iCurSel,2);
			ss=m_pListCtrl->GetItemText(iCurSel,3);
			m_fStartOddment=atof(ss);
			ss=m_pListCtrl->GetItemText(iCurSel,4);
			m_fEndOddment=atof(ss);
			UpdateData(FALSE);
		}
	}
	*pResult = 0;
}

void CBatchCreatePartDlg::OnKeydownTubeList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		/*if(pLVKeyDow->wVKey==VK_F3)
		{
			while(iCurSel>=0)
			{
				ModfiyItemInfo(iCurSel);
				pos=m_pListCtrl->GetFirstSelectedItemPosition();
				if(pos==NULL)
					break;
				iCurSel=m_pListCtrl->GetNextSelectedItem(pos);
			}
		}
		else*/ 
		if(pLVKeyDow->wVKey==VK_DELETE)
		{
			while(iCurSel>=0)
			{
				m_pListCtrl->DeleteItem(iCurSel);
				pos=m_pListCtrl->GetFirstSelectedItemPosition();
				if(pos==NULL)
					break;
				iCurSel=m_pListCtrl->GetNextSelectedItem(pos);
			}
		}
	}
	*pResult = 0;
}
#endif