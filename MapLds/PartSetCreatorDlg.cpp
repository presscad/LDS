//<LOCALE_TRANSLATE/>
// PartSetCreatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tower.h"
#include "PartSetCreatorDlg.h"
#include "GlobalFunc.h"
#include "DrawTaMap.h"
#include "AddAngleByRemoveNailDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartSetCreatorDlg dialog

#ifdef __COMMON_PART_INC_

IMPLEMENT_SET(CPartSetCreatorDlg,m_bGroupBySeg)
{
	_bGroupBySeg=value;
}
IMPLEMENT_GET(CPartSetCreatorDlg,m_bGroupBySeg)
{
	return GetSelectOnlyAngleOrPlate()&&(_bGroupBySeg||_bOneSegOneFile);
}
IMPLEMENT_SET(CPartSetCreatorDlg,m_bOneSegOneFile)
{
	_bOneSegOneFile=value;
	_bGroupBySeg=_bGroupBySeg||value;
}
IMPLEMENT_GET(CPartSetCreatorDlg,m_bOneSegOneFile)
{
	return GetSelectOnlyAngleOrPlate()&&_bOneSegOneFile;
}

CPartSetCreatorDlg::CPartSetCreatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartSetCreatorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartSetCreatorDlg)
	m_bIncAngle = FALSE;
	m_bIncPlate = FALSE;
	m_bIncTransectTube = FALSE;
	m_sSegStr = _T("*");
	m_sPartNo = _T("*");
	m_bIncStdPart= FALSE;
	m_nTaNum = 1;
	m_bIncTube = FALSE;
	m_bCreateTubeBrief = FALSE;
	m_bAutoArrangePlate = FALSE;
	m_sPartNum = _T("");
	m_bIncSlot = FALSE;
	m_bAutoCreateFile = FALSE;
	m_bCreateJgCutAngleBriefMap = FALSE;
	m_bIncArcPart = FALSE;
	m_bPatternDrawing = FALSE;
	m_bGroupBySeg = FALSE;
	m_bOneSegOneFile=FALSE;
	m_sDrawScale = _T("5");
	//}}AFX_DATA_INIT
	m_bBatchPlot = TRUE; 
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("part No.");
	m_arrHeaderLabel.Add("seg num");
	m_arrHeaderLabel.Add("type");
	m_arrHeaderLabel.Add("spec");
	m_arrHeaderLabel.Add("quantity");
	m_arrHeaderLabel.Add("description");
#else
	m_arrHeaderLabel.Add("�������");
	m_arrHeaderLabel.Add("�κ�");
	m_arrHeaderLabel.Add("����");
	m_arrHeaderLabel.Add("���");
	m_arrHeaderLabel.Add("����");
	m_arrHeaderLabel.Add("����");
#endif
	listViewComparer.SetMaxColumnCount(5);
	//���õ�һ�а���������
	listViewComparer.SetColumnSortType(0,SORTTYPE_PARTNO);
}

void CPartSetCreatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartSetCreatorDlg)
	DDX_Check(pDX, IDC_CHK_ANGLE, m_bIncAngle);
	DDX_Check(pDX, IDC_CHK_PLATE, m_bIncPlate);
	DDX_Check(pDX, IDC_CHK_TRANSECT_TUBE, m_bIncTransectTube);
	DDX_Text(pDX, IDC_E_SEG_STR, m_sSegStr);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_DRAW_SCALE, m_sDrawScale);
	DDX_Check(pDX, IDC_CHK_STD_PART, m_bIncStdPart);
	DDX_Text(pDX, IDC_E_TA_NUM, m_nTaNum);
	DDX_Check(pDX, IDC_CHK_TUBE, m_bIncTube);
	DDX_Check(pDX, IDC_CHK_CREATE_TUBE_BRIEF, m_bCreateTubeBrief);
	DDX_Check(pDX, IDC_CHK_AUTO_ARRANGE_PLATE, m_bAutoArrangePlate);
	DDX_Text(pDX, IDC_S_PART_NUM, m_sPartNum);
	DDX_Check(pDX, IDC_CHK_SLOT, m_bIncSlot);
	DDX_Check(pDX, IDC_CHK_AUTO_CREATE_FILE, m_bAutoCreateFile);
	DDX_Check(pDX, IDC_CHK_CREATE_JG_CUTANGLE_BRIEF_MAP, m_bCreateJgCutAngleBriefMap);
	DDX_Check(pDX, IDC_CHK_ARCPART, m_bIncArcPart);
	DDX_Check(pDX, IDC_CHK_PATTERN_DRAW, m_bPatternDrawing);
	DDX_Check(pDX, IDC_CHK_GROUP_BY_SEGI, _bGroupBySeg);
	DDX_Check(pDX, IDC_CHK_ONE_SEG_ONE_FILE, _bOneSegOneFile);
	DDX_Text(pDX,IDC_BN_CREATE_SELECTED_ITEMS,m_sCreate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartSetCreatorDlg, CDialog)
	//{{AFX_MSG_MAP(CPartSetCreatorDlg)
	ON_BN_CLICKED(IDC_BN_REBUILD_PARTSET, OnBnRebuildPartset)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_CTRL, OnKeydownListCtrl)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_CTRL, OnColumnclickListCtrl)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CTRL, OnDblclkListCtrl)
	ON_BN_CLICKED(IDC_CHK_ANGLE, OnUpdatePartType)
	ON_BN_CLICKED(IDC_CHK_SLOT, OnUpdatePartType)
	ON_BN_CLICKED(IDC_CHK_TRANSECT_TUBE, OnIncTransectTube)
	ON_BN_CLICKED(IDC_CHK_TUBE, OnIncTube)
	ON_BN_CLICKED(IDC_CHK_CREATE_TUBE_BRIEF, OnCreateTubeBrief)
	ON_BN_CLICKED(IDC_CHK_PLATE, OnUpdatePartType)
	ON_BN_CLICKED(IDC_CHK_STD_PART, OnUpdatePartType)
	ON_BN_CLICKED(IDC_CHK_AUTO_ARRANGE_PLATE, OnAutoArrangePlate)
	ON_BN_CLICKED(IDC_CHK_ARCPART, OnUpdatePartType)
	ON_BN_CLICKED(IDC_BN_ADD_PPI_FILE, OnAddProcessPartFile)
	ON_BN_CLICKED(IDC_CHK_PATTERN_DRAW, OnBnClickedChkPatternDraw)
	ON_COMMAND(ID_CREATE_ITEMS, OnBnClickedBnCreateSelectedItems)
	ON_COMMAND(ID_REVERSE_ANGLE,OnReverseAngle)
	ON_COMMAND(ID_NEW_ITEM,OnNewItem)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BN_CREATE_SELECTED_ITEMS, &CPartSetCreatorDlg::OnBnClickedBnCreateSelectedItems)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CTRL, &CPartSetCreatorDlg::OnClickListCtrl)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CTRL, OnRclickCmdListCommon)
	ON_BN_CLICKED(IDC_CHK_GROUP_BY_SEGI, &CPartSetCreatorDlg::OnBnClickedChkGroupBySegi)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartSetCreatorDlg message handlers

int CPartSetCreatorDlg::PATTERN_DRAW_SCALE= 1;
int CPartSetCreatorDlg::PART_DRAW_SCALE	= 5;
static CProcessPart *NewProcessPart(int CLSTYPE_ID,const char* key,void* pContext)
{
	CProcessPart *pPart=NULL;
	switch(CLSTYPE_ID)
	{
	case CProcessPart::TYPE_PLATE:
		pPart = new CProcessPlate();
		break;
	case CProcessPart::TYPE_LINEANGLE:
		pPart = new CProcessAngle();
		break;
	default:
		break;
	}
	return pPart;
}
BOOL DeleteProcessPart(CProcessPart* pPart);
static void ReadProcessPartsFromAttachBuffer(CSuperHashStrList<CProcessPart> &hashPartByPartNo)
{
	hashPartByPartNo.CreateNewAtom=NewProcessPart;
	hashPartByPartNo.DeleteAtom=DeleteProcessPart;
	hashPartByPartNo.Empty();
	CAttachBuffer *pPPEModelBuffer=Ta.AttachBuffer.GetValue(CTower::PPE_MODEL);
	if(pPPEModelBuffer==NULL||pPPEModelBuffer->GetLength()<=0)
		return;
	pPPEModelBuffer->SeekToBegin();
	int nPart=pPPEModelBuffer->ReadInteger();
	CProcessPart* pProcessPart=NULL;
	CXhChar16 sPartNo;
	for(int i=0;i<nPart;i++)
	{
		BYTE cPartType=CProcessPart::RetrievedPartTypeAndLabelFromBuffer(*pPPEModelBuffer,sPartNo,17);
		CProcessPart *pProcessPart=hashPartByPartNo.Add(sPartNo,cPartType);
		pProcessPart->FromPPIBuffer(*pPPEModelBuffer);
	}
}
BOOL CPartSetCreatorDlg::OnInitDialog() 
{
	/*
	if(m_bPatternDrawing)
		m_sDrawScale.Format("%d",PATTERN_DRAW_SCALE);
	else
		m_sDrawScale.Format("%d",PART_DRAW_SCALE);
	*/
	m_sDrawScale.Format("%g",sys.general_set.PartMapScale.fRealToDraw);
	CDialog::OnInitDialog();
	hashProcessParts.Empty();
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	if(Ta.m_nTaNum>0)
		m_nTaNum = Ta.m_nTaNum;
	m_pListCtrl->DeleteAllItems();
	if(m_bBatchPlot)
	{
		//GetDlgItem(IDC_E_PART_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_PART_NO)->SetWindowText("*");
		m_sPartNo="*";
	}
	else 
	{
		GetDlgItem(IDC_CHK_AUTO_CREATE_FILE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_CREATE_JG_CUTANGLE_BRIEF_MAP)->ShowWindow(SW_HIDE);
	}
	ReadProcessPartsFromAttachBuffer(hashProcessPartByLabel);
	partList.Empty();
	RebuildPartset();
	m_bCreateTubeBrief=FALSE;
	if(!m_bIncTube||m_bIncAngle||m_bIncPlate||m_bIncTransectTube||m_bIncSlot||m_bIncArcPart)
		GetDlgItem(IDC_CHK_CREATE_TUBE_BRIEF)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_CHK_CREATE_TUBE_BRIEF)->EnableWindow(TRUE);
	m_bAutoArrangePlate=FALSE;
	if(m_bBatchPlot&&(m_bIncPlate||m_bIncStdPart)) //�������ɹ���ʱ��֧���Զ��Ű�ְ� wht 10-11-18
		GetDlgItem(IDC_CHK_AUTO_ARRANGE_PLATE)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_CHK_AUTO_ARRANGE_PLATE)->EnableWindow(FALSE);
	//Ŀǰ��֧�ֽ��Ǹ�������ɵ�����Dwg�ļ��� wht 11-06-02
	if(!m_bIncAngle||m_bIncTube||m_bIncPlate||m_bIncTransectTube||m_bIncSlot||m_bIncArcPart)
		GetDlgItem(IDC_CHK_AUTO_CREATE_FILE)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_CHK_AUTO_CREATE_FILE)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_CREATE_JG_CUTANGLE_BRIEF_MAP)->EnableWindow(m_bIncAngle);
	GetDlgItem(IDC_CHK_GROUP_BY_SEGI)->EnableWindow(GetSelectOnlyAngleOrPlate());
	if(m_bGroupBySeg)
		GetDlgItem(IDC_CHK_ONE_SEG_ONE_FILE)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_CHK_ONE_SEG_ONE_FILE)->EnableWindow(FALSE);
	m_bAutoCreateFile=FALSE;
	m_bCreateJgCutAngleBriefMap=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCreate="create all parts map";
#else
	m_sCreate="�������й���ͼ";
#endif
	UpdateData(FALSE);
	return TRUE;
}
void CPartSetCreatorDlg::RefreshListViewHeader()
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
#ifdef AFX_TARG_ENU_ENGLISH
		if(i==0) lvc.cx=73;
		else if(i==1)	lvc.cx=75;
		else if(i==2)	lvc.cx=65;
		else if(i==3)	lvc.cx=100;	//���
		else if(i==4)	lvc.cx=75;	//����
		else
			lvc.cx = 125;
#else 
		if(i==0) lvc.cx=70;
		else if(i==1)	lvc.cx=45;
		else if(i==2)	lvc.cx=45;
		else if(i==3)	lvc.cx=100;	//���
		else if(i==4)	lvc.cx=45;	//����
		else
			lvc.cx = 120;
#endif
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}
//
static  void ReverseAngle( CProcessAngle* pSrcAngle,CProcessAngle* pAngle)
{
	//��ԭ�Ǹ�ת���ɹ�����Ϣ
	CBuffer buffer;
	pSrcAngle->ToPPIBuffer(buffer);
	buffer.SeekToBegin();
	pAngle->FromPPIBuffer(buffer);
	//1 ������˨�׵�λ��
	pAngle->m_xBoltInfoList.Empty();
	for (BOLT_INFO *pBoltInfo=pSrcAngle->m_xBoltInfoList.GetFirst();pBoltInfo;pBoltInfo=pSrcAngle->m_xBoltInfoList.GetNext())
	{
		BOLT_INFO *pNewBoltInfo=pAngle->m_xBoltInfoList.Add(0);
		pNewBoltInfo->CloneBolt(pBoltInfo);
		pNewBoltInfo->posY=-pBoltInfo->posY;
		pNewBoltInfo->posX=pSrcAngle->GetLength()-pBoltInfo->posX;
	}
	//2 �����Ͻ�	
	pAngle->kaiHeJiaoList.Empty();
	for (KAI_HE_JIAO* pKaiHeJiao=pSrcAngle->kaiHeJiaoList.GetFirst();pKaiHeJiao;pKaiHeJiao=pSrcAngle->kaiHeJiaoList.GetNext())
	{
		KAI_HE_JIAO* pNewKaiHeJiao=pAngle->kaiHeJiaoList.append();
		pNewKaiHeJiao->decWingAngle=pKaiHeJiao->decWingAngle;
		pNewKaiHeJiao->position=pAngle->GetLength()-pKaiHeJiao->position;
		pNewKaiHeJiao->startLength=pKaiHeJiao->endLength;
		pNewKaiHeJiao->endLength=pKaiHeJiao->startLength;
	}
	//2.1 ����֫����
	if (pAngle->kaihe_base_wing_x0_y1==0)
		pAngle->kaihe_base_wing_x0_y1=1;
	else if (pAngle->kaihe_base_wing_x0_y1==1)
		pAngle->kaihe_base_wing_x0_y1=0;
	//3 ѹ��
	if(pSrcAngle->wing_push_X1_Y2==1||pSrcAngle->wing_push_X1_Y2==2)
	{
		//3.1 ����ѹ���λ��
		pAngle->end_push_pos=pSrcAngle->GetLength()-pSrcAngle->start_push_pos;
		pAngle->start_push_pos=pSrcAngle->GetLength()-pSrcAngle->end_push_pos;
		//3.2 ����ѹ���֫
		if(pSrcAngle->wing_push_X1_Y2==1)
			pAngle->wing_push_X1_Y2=2;
		else if(pSrcAngle->wing_push_X1_Y2==2)
			pAngle->wing_push_X1_Y2=1;
	}
	//4.�н�
	for (int i=0;i<4;i++)
	{
		pAngle->cut_angle[i][0]=(float)pSrcAngle->cut_angle[3-i][0];
		pAngle->cut_angle[i][1]=(float)pSrcAngle->cut_angle[3-i][1];
	}
	//5.�����Ǹ��ϵ��߰�
	RIB_PLATE_INFO* pRibPlateInfox=pAngle->rib_plate_list.GetFirst();
	for (RIB_PLATE_INFO* pRibPlateInfo=pAngle->rib_plate_list.GetFirst();pRibPlateInfo;pRibPlateInfo=pAngle->rib_plate_list.GetNext())
		pRibPlateInfo->fPosZ=pSrcAngle->GetLength()-pRibPlateInfo->fPosZ;
	//6.��֫
	if(pSrcAngle->cut_wing[0]!=0&&pSrcAngle->cut_wing[1]!=0)//��֫���в�����
	{
		for (int i=0;i<2;i++)
		{
			if(pSrcAngle->cut_wing[i]==1)
				pAngle->cut_wing[1-i]=2;
			else if(pSrcAngle->cut_wing[i]==2)
				pAngle->cut_wing[1-i]=1;
			else if(pSrcAngle->cut_wing[i]==0)
				pAngle->cut_wing[1-i]=0;
		}
		for (int i=0;i<2;i++)
		{
			pAngle->cut_wing_para[i][0]=pSrcAngle->cut_wing_para[1-i][0];
			pAngle->cut_wing_para[i][1]=pSrcAngle->cut_wing_para[1-i][1];
			pAngle->cut_wing_para[i][2]=pSrcAngle->cut_wing_para[1-i][2];
		}
	}
}
int CPartSetCreatorDlg::AppendRecord(int iItem, CStringArray &str_arr)
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

void CPartSetCreatorDlg::OnBnRebuildPartset() 
{
	UpdateData();
	RebuildPartset();
	UpdateData(FALSE);
}

void CPartSetCreatorDlg::AddPartset(IModel *pModel,CHashList<SEGI> &segTbl,
									CBlockModel *pEmbededBlock/*=NULL*/,
									CBlockReference *pBlockRef/*=NULL*/)
{
	//�麸����ϣ��
	CHashTable<long>partGroupTbl;
	partGroupTbl.CreateHashTable(pModel->GetPartGroupNum());
	CLDSPartGroup *pPartGroup=NULL;
	for(pPartGroup=pModel->EnumPartGroupFirst();pPartGroup;pPartGroup=pModel->EnumPartGroupNext())
		partGroupTbl.SetValueAt(pPartGroup->handle,pPartGroup->handle);
	DWORD *pValue=NULL;
	CStringArray str_arr;
	str_arr.SetSize(6);
	bool filterByClsType=false;
	if(m_sPartNo.GetLength()==0||m_sPartNo.Compare("*")==0)
		filterByClsType=true;
	for(CLDSPart *pPart=pModel->EnumPartFirst();pPart;pPart=pModel->EnumPartNext())
	{
		if(pModel->IsTowerModel()&&pPart->IsBlockRefPart())//ը���Ĳ������ù��������Ƚ��г�ʼ��
			pPart->ShadowDesign();
		if(pEmbededBlock&&pEmbededBlock->handle!=pPart->m_hBlock&&!pPart->IsBlockRefPart())
			continue;	//���˵������ڵ�ǰ�����Ĺ���
		if(pPart->m_bVirtualPart)
			continue;
		int idClsType=pPart->GetClassTypeId();
		if(idClsType==CLS_LINEANGLE&&!m_bIncAngle&&filterByClsType)
			continue;
		if((idClsType==CLS_LINEFLAT||idClsType==CLS_LINESLOT)&&!m_bIncSlot&&filterByClsType)
			continue;
		if(idClsType==CLS_LINETUBE&&!m_bIncTransectTube&&!m_bIncTube&&filterByClsType)
			continue;
		if(idClsType==CLS_PLATE)
		{
			CLDSPlate* pPlate=(CLDSPlate*)pPart;
			BOOL bStdAttachPlate=FALSE;
			if(pPlate->designInfo.origin.datum_pos_style==9)
			{
				CLDSParamPlate *pXEndPlate=(CLDSParamPlate*)console.FromPartHandle(pPlate->designInfo.m_hBasePart,CLS_PARAMPLATE);
				if(pXEndPlate&&pXEndPlate->m_iParamType==TYPE_XEND&&pXEndPlate->AttachPart.GetValue(pPlate->handle))
					bStdAttachPlate=TRUE;
			}
			if(bStdAttachPlate&&!m_bIncStdPart&&filterByClsType)
				continue;
			if(!bStdAttachPlate&&!m_bIncPlate&&filterByClsType)
				continue;
		}
		if(idClsType==CLS_PARAMPLATE)
		{
			CLDSParamPlate* pParamPlate=(CLDSParamPlate*)pPart;
			if(pParamPlate->m_bStdPart&&!m_bIncStdPart&&filterByClsType)
				continue;
			if(!pParamPlate->m_bStdPart&&!m_bIncPlate&&filterByClsType)
				continue;
		}
		if((idClsType==CLS_ARCANGLE || idClsType==CLS_ARCFLAT || idClsType==CLS_ARCSLOT)&&!m_bIncArcPart&&filterByClsType)
			continue;
		if(idClsType!=CLS_LINEFLAT&&idClsType!=CLS_LINESLOT&&idClsType!=CLS_LINEANGLE
			&&idClsType!=CLS_LINETUBE&&idClsType!=CLS_PLATE&&idClsType!=CLS_PARAMPLATE
			&&idClsType!=CLS_ARCANGLE&&idClsType!=CLS_ARCSLOT&&idClsType!=CLS_ARCFLAT)
			continue;
		if(pPart->IsLinePart())
		{
			if(((CLDSLinePart*)pPart)->bHuoQuStart||((CLDSLinePart*)pPart)->bHuoQuEnd)
				continue;	//����������ͳ��
		}
		CXhChar16 sPartNo=pPart->GetPartNo();
		if(strlen(sPartNo)==0)
			continue;	//δ��Ź��������빹����
		else if(!(strstr(sPartNo,m_sPartNo)!=NULL||m_sPartNo=="*"||m_sPartNo.IsEmpty()))
			continue;	//��������ɹ���ͼʱ�������ָ�������������������
		else if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPart->iSeg.iSeg)==NULL&&filterByClsType)
			continue;	//�ֶκŲ��ڵ�ǰ��ѡ��Χ��
		if(pPart->GetClassTypeId()==CLS_LINETUBE&&m_bIncTransectTube)
		{	//ѡ�������ֹ�
			CLDSLineTube *pTube=(CLDSLineTube*)pPart;
			BOOL bStartTrans=FALSE,bEndTrans=FALSE;
			if(pTube->m_tJointStart.type==0&&!pTube->m_tJointStart.cutPlaneNorm.IsZero())
				bStartTrans=TRUE;
			else if(pTube->m_tJointStart.type==1&&pTube->m_tJointStart.hLinkObj>0)
				bStartTrans=TRUE;
			if(pTube->m_tJointEnd.type==0&&!pTube->m_tJointEnd.cutPlaneNorm.IsZero())
				bEndTrans=TRUE;
			else if(pTube->m_tJointEnd.type==1&&pTube->m_tJointEnd.hLinkObj>0)
				bEndTrans=TRUE;
			if(!bStartTrans&&!bEndTrans)
				continue;	//ʼ�ն˶����������
		}
		if(pBlockRef)
		{	//���ݲ������öκţ����¹����κż��������
			pPart->iSeg=pBlockRef->iSeg;
			char sSerialNo[16]="",sPartNo[16]="",sSeparator[16]="";
			SEGI iOldSeg;
			if(ParsePartNo(pPart->GetPartNo(),&iOldSeg,sSerialNo,NULL,sSeparator))
				sprintf(sPartNo,"%s%s%s",(char*)pPart->iSeg.ToString(),sSeparator,sSerialNo);
			pPart->SetPartNo(sPartNo);
		}
		if(pValue=m_xItemPtrHashPartNo.GetValue(pPart->GetPartNo()))
		{
			STDPART_ITEM *pItem=(STDPART_ITEM*)(*pValue);
			pItem->num+=pPart->m_uStatMatCoef;
			if(pPart->GetClassTypeId()==CLS_PLATE&&!pItem->bFlag)
			{	//������֤���ɸְ����ͼʱʹ�õĸְ�Ϊ�麸���г��ֹ��ĸְ� wht 10-11-12
				if(!partGroupTbl.ContainsKey(pItem->GetPart()->m_hPartWeldParent))
				{	//pItem->pPart���������麸���У���Ҫ����
					if(partGroupTbl.ContainsKey(pPart->m_hPartWeldParent))
					{
						pItem->SetPart(pPart);	//�������ɸְ�����ʱʹ�õĸְ�
						pItem->bFlag=TRUE;		//�����β���
					}
				}
			}
		}
		else
		{
			STDPART_ITEM *pItem=partList.append();
			pItem->bFlag=FALSE;
			CProcessPart* pProcessPart=hashProcessPartByLabel.GetValue(pPart->GetPartNo());
			if(pProcessPart&&pProcessPart->m_dwInheritPropFlag>0)
			{
				CBuffer ppibuf;
				pPart->ToPPIBuffer(ppibuf);
				//pProcessPart->MergeWith(ppibuf);
				pProcessPart->MergeToSelfWithModel(ppibuf);
				pItem->SetProcessPart(pProcessPart);
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[0].Format("%s(Edited)",(char*)pPart->GetPartNo());
#else
				str_arr[0].Format("%s(�ѱ༭)",(char*)pPart->GetPartNo());
#endif
			}
			else
			{
				pItem->SetPart(pPart);
				str_arr[0]=pPart->GetPartNo();
			}
			pItem->num=pPart->m_uStatMatCoef;
			m_xItemPtrHashPartNo.SetValue(pPart->GetPartNo(),(DWORD)pItem);
			str_arr[1].Format("%s",(char*)pPart->iSeg.ToString());
			if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[2]="Angle";
#else
				str_arr[2]="�Ǹ�";
#endif
				char sProcess[50]="";
				((CLDSLineAngle*)pPart)->GetProcessStr(sProcess,50);
				str_arr[5].Format("%s",sProcess);
			}
			else if(pPart->GetClassTypeId()==CLS_LINESLOT)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[2]="Slot";
#else
				str_arr[2]="�۸�";
#endif
				str_arr[5]="";
			}
			else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[2]="Flat";
#else
				str_arr[2]="����";
#endif
				str_arr[5]="";
			}
			else if(pPart->GetClassTypeId()==CLS_ARCANGLE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[2]="Arc Angle";
#else
				str_arr[2]="���νǸ�";
#endif
				str_arr[5]="";
			}
			else if(pPart->GetClassTypeId()==CLS_ARCFLAT)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[2]="Arc Flat";
#else
				str_arr[2]="���α���";
#endif
				str_arr[5]="";
			}
			else if(pPart->GetClassTypeId()==CLS_ARCSLOT)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[2]="Arc Slot";
#else
				str_arr[2]="���β۸�";
#endif
				str_arr[5]="";
			}
			else if(pPart->GetClassTypeId()==CLS_LINETUBE)
			{
				CLDSLineTube *pLineTube=(CLDSLineTube*)pPart;
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[2]="Tube";
#else
				str_arr[2]="�ֹ�";
#endif
				if(m_bIncTransectTube)
				{
					int nType=0;//1���� 2���и� 3�����
					for(int i=0;i<2;i++)
					{
						TUBEJOINT *pCurJoint=&pLineTube->m_tJointStart;
						if(i==1)
							pCurJoint=&pLineTube->m_tJointEnd;
						if((pCurJoint->type==0||pCurJoint->type==1)&&!pCurJoint->cutPlaneNorm.IsZero())
							nType=max(2,nType);
						else if((pCurJoint->type==3||pCurJoint->type==4||pCurJoint->type==5)&&pCurJoint->hLinkObj>0x20)
							nType=max(1,nType);
						else if(pCurJoint->type==1&&pCurJoint->hLinkObj>0x20)
							nType=max(3,nType);
					}
					if(nType==2)
#ifdef AFX_TARG_ENU_ENGLISH
						str_arr[5]="plane cutting";
					else if(nType==3)
						str_arr[5]="intersecting tube";
#else
						str_arr[5]="ƽ���и�";
					else if(nType==3)
						str_arr[5]="���ֹ�";
#endif
				}
				else
				{
					int nType[2]={0,0};	//-1����� 1���и� 2����
					for(int i=0;i<2;i++)
					{
						TUBEJOINT *pCurJoint=&pLineTube->m_tJointStart;
						if(i==1)
							pCurJoint=&pLineTube->m_tJointEnd;
						if((pCurJoint->type==0||pCurJoint->type==1)&&!pCurJoint->cutPlaneNorm.IsZero())
							nType[i]=1;
						else if(pCurJoint->type==1&&pCurJoint->hLinkObj>0x20)
							nType[i]=-1;
						else if((pCurJoint->type==3||pCurJoint->type==4||pCurJoint->type==5)&&pCurJoint->hLinkObj>0x20)
							nType[i]=2;
					}
					if(nType[0]!=0||nType[1]!=0)
					{
						int nSumType=abs(nType[0])+abs(nType[1]);
						if(nSumType==1)
						{
							if(nType[0]==-1||nType[1]==-1)
#ifdef AFX_TARG_ENU_ENGLISH
								str_arr[5]="intersecting at one end";
							else
								str_arr[5]="cutting at one end";
#else
								str_arr[5]="һ�����";
							else
								str_arr[5]="һ���и�";
#endif
						}
						else if(nSumType==2&&abs(nType[0])==abs(nType[1]))
						{
							if(nType[0]==-1&&nType[1]==-1)
#ifdef AFX_TARG_ENU_ENGLISH
								str_arr[5]="intersecting at both ends";
							else if(nType[0]==-1||nType[1]==-1)
								str_arr[5]="cutting at one end and intersecting at the other end";
							else 
								str_arr[5]="cutting at both ends";
						}
						else if(nSumType==2&&abs(nType[0])!=abs(nType[1]))
							str_arr[5]="grooving at one end";
#else
								str_arr[5]="�������";
							else if(nType[0]==-1||nType[1]==-1)
								str_arr[5]="һ���и�һ�����";
							else 
								str_arr[5]="�����и�";
						}
						else if(nSumType==2&&abs(nType[0])!=abs(nType[1]))
							str_arr[5]="һ�˿���";
#endif
						else if(nSumType==3)
						{
							if(nType[0]==-1||nType[1]==-1)
#ifdef AFX_TARG_ENU_ENGLISH
								str_arr[5]="intersecting at one end and grooving at the other end";
							else
								str_arr[5]="cutting at one end and grooving at the other end";
						}
						else if(nSumType==4)
							str_arr[5]="grooving at both ends";
#else
								str_arr[5]="һ�����һ�˿���";
							else
								str_arr[5]="һ���и�һ�˿���";
						}
						else if(nSumType==4)
							str_arr[5]="���˿���";
#endif
					}
					else
						str_arr[5]="";
				}
			}
			else if(pPart->GetClassTypeId()==CLS_PLATE)
			{
				CLDSPlate *pPlate=(CLDSPlate*)pPart;
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[2]="Plate";
#else
				str_arr[2]="�ְ�";
#endif
				//����� ˫��� ����� �۵��� wht 10-11-18
				if(pPlate->face_N==3)
				{
					if(pPlate->IsFoldPlate())
#ifdef AFX_TARG_ENU_ENGLISH
						str_arr[5]="folded plate";
					else 
						str_arr[5]="3-plane plate";
				}
				else if(pPlate->face_N==2)
					str_arr[5]="2-plane plate";
#else
						str_arr[5]="�۵���";
					else 
						str_arr[5]="�����";
				}
				else if(pPlate->face_N==2)
					str_arr[5]="˫���";
#endif
				else //if(pPlate->face_N==1)
				{
					if(pPlate->jdb_style==4)
#ifdef AFX_TARG_ENU_ENGLISH
						str_arr[5]="bolt gasket";
#else
						str_arr[5]="��˨���";
#endif
					else
					{
						BYTE cType=0;
						if( pPlate->designInfo.origin.datum_pos_style==9)
						{
							CLDSParamPlate *pXEndPlate=(CLDSParamPlate*)console.FromPartHandle(pPlate->designInfo.m_hBasePart,CLS_PARAMPLATE);
							if(pXEndPlate!=NULL&&pXEndPlate->m_iParamType==TYPE_XEND&&pXEndPlate->AttachPart.GetValue(pPlate->handle))
							{
								if(pPlate->designInfo.origin.datum_pos.x==0&&pPlate->designInfo.origin.datum_pos.y==0&&pPlate->designInfo.origin.datum_pos.z!=0)
									cType=1;
								else
									cType=2;
							}
						}
#ifdef AFX_TARG_ENU_ENGLISH
						if(cType==1)
							str_arr[5]="vertical plate of cross plate";
						else if(cType==2)
							str_arr[5]="wrap plate of cross plate";
						else
							str_arr[5]="planar plate";
#else
						if(cType==1)
							str_arr[5]="ʮ�ֲ������";
						else if(cType==2)
							str_arr[5]="ʮ�ֲ�����";
						else
							str_arr[5]="�����";
#endif
					}
				}
				if(pPlate->GetRollEdgeHeight()>0)
#ifdef AFX_TARG_ENU_ENGLISH
					str_arr[5].Append(" Roll edge");
#else
					str_arr[5].Append(" ���");
#endif
			}
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[2]="Plate";
				if(pParamPlate->m_iParamType==TYPE_FL)
					str_arr[5]="flange";
				else if(pParamPlate->m_iParamType==TYPE_FLR)
					str_arr[5]="flexible flange";
				else if(pParamPlate->m_iParamType==TYPE_FLG)
					str_arr[5]="rigid flange";
				else if(pParamPlate->m_iParamType==TYPE_FLD)
					str_arr[5]="welding neck flange";
				else if(pParamPlate->m_iParamType==TYPE_FLP)
					str_arr[5]="flat welding flange";
				else if(pParamPlate->m_iParamType==TYPE_UEND)
					str_arr[5]="U plate";
				else if(pParamPlate->m_iParamType==TYPE_ROLLEND)
					str_arr[5]="slot plate";
				else if(pParamPlate->m_iParamType==TYPE_XEND)
				{
					str_arr[5]="cross plate";
#else
				str_arr[2]="�ְ�";
				if(pParamPlate->m_iParamType==TYPE_FL)
					str_arr[5]="����";
				else if(pParamPlate->m_iParamType==TYPE_FLR)
					str_arr[5]="���Է���";
				else if(pParamPlate->m_iParamType==TYPE_FLG)
					str_arr[5]="���Է���";
				else if(pParamPlate->m_iParamType==TYPE_FLD)
					str_arr[5]="�Ժ�����";
				else if(pParamPlate->m_iParamType==TYPE_FLP)
					str_arr[5]="ƽ������";
				else if(pParamPlate->m_iParamType==TYPE_UEND)
					str_arr[5]="U�Ͳ��";
				else if(pParamPlate->m_iParamType==TYPE_ROLLEND)
					str_arr[5]="���Ͳ��";
				else if(pParamPlate->m_iParamType==TYPE_XEND)
				{
					str_arr[5]="ʮ���Ͳ��";
#endif
					//�ж�ʮ�ֲ������������Ƿ�ֱ wht 10-03-06
					ATTACHPART_ITEM *pItem=NULL;
					for(pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
					{
						CLDSPart *pAttachPart=pModel->FromPartHandle(pItem->h);
						if(pAttachPart==NULL)
							continue;
						if(pAttachPart->GetClassTypeId()!=CLS_PLATE)
							continue;
						CLDSPlate *pPlate=(CLDSPlate*)pAttachPart;
						if(pPlate->designInfo.origin.des_para.hPart!=pParamPlate->handle)
							continue;
						f3dPoint plate_pos=pPlate->ucs.origin;
						coord_trans(plate_pos,pParamPlate->ucs,FALSE);
						if(plate_pos.x>EPS||plate_pos.y>EPS)
							continue;	//����λ��װ����ʮ�ֲ���������ϵ֮��X��Y����Ӧ��Ϊ0  wht 10-06-30
						if(fabs(pPlate->ucs.axis_z*pParamPlate->ucs.axis_z)>EPS)
#ifdef AFX_TARG_ENU_ENGLISH
							str_arr[5]="non-90��cross plate";
#else
							str_arr[5]="��90��ʮ���Ͳ��";
#endif
					}
				}
				else if(pParamPlate->m_iParamType==TYPE_ELBOWPLATE)
#ifdef AFX_TARG_ENU_ENGLISH
					str_arr[5]="elbow plate";
				else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
					str_arr[5]="circular";
				else if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
					str_arr[5]="ring sealing plate";
				else if(pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
					str_arr[5]="cross?connecting plate";
				else if(pParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
					str_arr[5]="angle's rib plate";
				else if(pParamPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
					str_arr[5]="flashing board";
				else if(pParamPlate->m_iParamType==TYPE_BASEPLATE)
					str_arr[5]="tower foot base plate";
				else if(pParamPlate->m_iParamType==TYPE_SHOEPLATE)
					str_arr[5]="tower foot shoe plate";
				else
					str_arr[5]="other parametric plate";
#else
					str_arr[5]="���";
				else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
					str_arr[5]="�����߰�";
				else if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
					str_arr[5]="������";
				else if(pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
					str_arr[5]="��Խ�ֹ�����";
				else if(pParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
					str_arr[5]="�Ǹ��߰�";
				else if(pParamPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
					str_arr[5]="��ˮ��";
				else if(pParamPlate->m_iParamType==TYPE_BASEPLATE)
					str_arr[5]="���ŵװ�";
				else if(pParamPlate->m_iParamType==TYPE_SHOEPLATE)
					str_arr[5]="����ѥ��";
				else
					str_arr[5]="�����������";
#endif
			}
			else
				str_arr[2]="";
			CXhChar50 spec;
			pPart->GetSpecification(spec);
			str_arr[3]=(char*)spec;
			str_arr[4]="1";
			int iCur=AppendRecord(-1,str_arr);
			m_pListCtrl->SetItemData(iCur,(DWORD)pItem);
		}
	}
	if(pModel->IsTowerModel())
	{
		polypartlist.Empty();
		((CTower*)pModel)->RetrievePolyPartList(polypartlist);
		for(CLDSPolyPart *pPolyPart=polypartlist.GetFirst();pPolyPart;pPolyPart=polypartlist.GetNext())
		{
			CLDSPart *pSonPart=pPolyPart->segset.GetFirst();
			int idClsType=pSonPart->GetClassTypeId();
			if(idClsType!=CLS_LINEANGLE||(filterByClsType&&!m_bIncAngle))
				continue;
			CXhChar16 sPartNo=pSonPart->GetPartNo();
			if(!(strstr(sPartNo,m_sPartNo)!=NULL||m_sPartNo=="*"||m_sPartNo.IsEmpty()))
				continue;	//��������ɹ���ͼʱ �������ָ��������������
			if(strlen(pSonPart->GetPartNo())==0)
				continue;	//δ��Ź��������빹����
			if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pSonPart->iSeg.iSeg)==NULL)
				continue;	//�ֶκŲ��ڵ�ǰ��ѡ��Χ��
			if(pValue=m_xItemPtrHashPartNo.GetValue(pSonPart->GetPartNo()))
			{
				STDPART_ITEM *pItem=(STDPART_ITEM*)(*pValue);
				pItem->num++;
			}
			else
			{
				STDPART_ITEM *pItem=partList.append();
				pItem->SetPart(pPolyPart);
				pItem->num=pSonPart->m_uStatMatCoef;
				m_xItemPtrHashPartNo.SetValue(pSonPart->GetPartNo(),(DWORD)pItem);
				CProcessPart* pProcessPart=hashProcessPartByLabel.GetValue(pSonPart->GetPartNo());
				if(pProcessPart&&pProcessPart->m_dwInheritPropFlag>0)
				{
					CBuffer ppibuf;
					pPolyPart->ToPPIBuffer(ppibuf);
					//pProcessPart->MergeWith(ppibuf);
					pProcessPart->MergeToSelfWithModel(ppibuf);
					pItem->SetProcessPart(pProcessPart);
#ifdef AFX_TARG_ENU_ENGLISH
					str_arr[0].Format("%s(Edited)",(char*)pSonPart->GetPartNo());
#else
					str_arr[0].Format("%s(�ѱ༭)",(char*)pSonPart->GetPartNo());
#endif
				}
				else
					str_arr[0]=pSonPart->GetPartNo();
				str_arr[1].Format("%s",(char*)pSonPart->iSeg.ToString());
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[2]="blending angle";
#else
				str_arr[2]="����Ǹ�";
#endif
				CXhChar50 spec;
				pPolyPart->GetSpecification(spec);
				str_arr[3]=(char*)spec;
				str_arr[4]="1";
				CXhChar50 sProcess;
				((CLDSLineAngle*)pSonPart)->GetProcessStr(sProcess,50);
				str_arr[5].Format("%s",(char*)sProcess);
				int iCur=AppendRecord(-1,str_arr);
				m_pListCtrl->SetItemData(iCur,(DWORD)pItem);
			}
		}
	}
}

void CPartSetCreatorDlg::AddBlockRefsToPartSet(CHashList<SEGI> &segTbl)
{
	CHashList<SEGI> segNoTbl;	//�յĶκŹ�ϣ��
	for(CBlockReference *pBlockRef=Ta.BlockRef.GetFirst();pBlockRef;pBlockRef=Ta.BlockRef.GetNext())
	{
		if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pBlockRef->iSeg.iSeg)==NULL)
			continue;	//�ֶκŲ��ڵ�ǰ��ѡ��Χ��
		if(pBlockRef->IsExploded())
			continue;
		CBlockModel *pBlock=Ta.Block.FromHandle(pBlockRef->m_hBlock);
		if(pBlock==NULL)
			continue;
		if(pBlock->IsEmbeded())	//Ƕ��ʽ����
			AddPartset(&Ta,segNoTbl,pBlock,pBlockRef);
		else							//����ʽ���� 
			AddPartset(pBlock,segNoTbl,NULL,pBlockRef);
	}
}

BOOL CPartSetCreatorDlg::RebuildPartset()
{
	CHashList<SEGI>segTbl;
	GetSegNoHashTblBySegStr(m_sSegStr,segTbl);
	//
	partList.Empty();
	m_xItemPtrHashPartNo.Empty();
	m_pListCtrl->DeleteAllItems();
	//
	AddPartset(&Ta,segTbl);
	AddBlockRefsToPartSet(segTbl);
	AddProcessPart();
	AddNewProcessAngle();
	m_pListCtrl->EnsureVisible(0,TRUE);
	BOOL bIncrCol0Width=FALSE,bIncrCol2Width=FALSE;
	int nCount=m_pListCtrl->GetItemCount();
	for(int j=0;j<nCount;j++)
	{
		STDPART_ITEM *pItem=(STDPART_ITEM*)m_pListCtrl->GetItemData(j);
		CString str;
		str.Format("%d",pItem->num);
		m_pListCtrl->SetItemText(j,4,str);
		if(!bIncrCol0Width)
		{
			str=m_pListCtrl->GetItemText(j,0);
			bIncrCol0Width=str.Find('(')>0;	//����(�ѱ༭)��������Ҫ�Ӵ��п�
		}
		if(!bIncrCol2Width)
		{
			str=m_pListCtrl->GetItemText(j,2);
			bIncrCol2Width=str.Find('(')>0;//����(��)��(��)��������Ҫ�Ӵ��п� wht 16-04-27
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPartNum.Format("There are %d parts in current list.",nCount);
	m_pListCtrl->SetColumnWidth(2,bIncrCol2Width?75:65);
	m_pListCtrl->SetColumnWidth(0,bIncrCol0Width?100:73);
#else
	m_sPartNum.Format("��ǰ�б��й���%d��������",nCount);
	m_pListCtrl->SetColumnWidth(2,bIncrCol2Width?65:45);
	m_pListCtrl->SetColumnWidth(0,bIncrCol0Width?90:70);
#endif
	return TRUE;
}

void CPartSetCreatorDlg::OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	if(pLVKeyDow->wVKey==VK_DELETE)
	{
		POSITION pos=m_pListCtrl->GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
			STDPART_ITEM *pItem=(STDPART_ITEM*)m_pListCtrl->GetItemData(iCurSel);
			if(pItem)
			{
				for(STDPART_ITEM *pSelItem=partList.GetFirst();pSelItem;pSelItem=partList.GetNext())
				{
					if(pSelItem==pItem)
					{
						partList.DeleteCursor();
						break;
					}
				}
			}
			m_pListCtrl->DeleteItem(iCurSel);
			pos=m_pListCtrl->GetFirstSelectedItemPosition();
		}
#ifdef AFX_TARG_ENU_ENGLISH
		m_sPartNum.Format("There are %d parts in current list.",m_pListCtrl->GetItemCount());
#else
		m_sPartNum.Format("��ǰ�б��й���%d��������",m_pListCtrl->GetItemCount());
#endif
		UpdateData(FALSE);
	}
	*pResult = 0;
}

void CPartSetCreatorDlg::OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	listViewComparer.m_pListCtrl = m_pListCtrl;
	listViewComparer.m_iColumn = pNMListView->iSubItem;
	listViewComparer.sortOrderAscending[pNMListView->iSubItem]=!listViewComparer.sortOrderAscending[pNMListView->iSubItem];
	m_pListCtrl->SortItems(CompareItemProc,(DWORD)&listViewComparer);
	*pResult = 0;
}

void CPartSetCreatorDlg::OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_pListCtrl->GetSelectedCount()==1)
		ConfirmCreate(true);
	*pResult = 0;
}

void CPartSetCreatorDlg::OnClickListCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData();
	if(m_pListCtrl->GetSelectedCount()==0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sCreate="generate all parts map";
	else
		m_sCreate="generate selected parts map";
#else
		m_sCreate="�������й���ͼ";
	else
		m_sCreate="����ѡ�й���ͼ";
#endif
	UpdateData(FALSE);
	*pResult = 0;
}

void CPartSetCreatorDlg::OnBnClickedChkPatternDraw()
{
	UpdateData();
	/*
	if(!m_bPatternDrawing)
		m_sDrawScale.Format("%d",PART_DRAW_SCALE);
	else
		m_sDrawScale.Format("%d",PATTERN_DRAW_SCALE);
	*/
	m_sDrawScale.Format("%g",sys.general_set.PartMapScale.fRealToDraw);
	UpdateData(FALSE);
}

void CPartSetCreatorDlg::OnBnClickedBnCreateSelectedItems()
{
	if(m_pListCtrl->GetSelectedCount()==0)
		ConfirmCreate(false);
	else
		ConfirmCreate(true);
}

void CPartSetCreatorDlg::OnReverseAngle()
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		STDPART_ITEM *pItem=(STDPART_ITEM*)m_pListCtrl->GetItemData(iCurSel);
		CLDSPart* pPart=pItem->GetPart();
		CProcessPart *pProcessPart=pItem->GetProcessPart();
		if((pPart&&!pPart->IsAngle())||(pProcessPart&&!pProcessPart->IsAngle()))
			continue;
		if(pItem->bReverseAngle)
			pItem->bReverseAngle=FALSE;
		else
		{
			CProcessAngle srcAngle;
			CProcessAngle *pSrcAngle=&srcAngle;
			if(pPart)
			{
				CBuffer buffer;
				pPart->ToPPIBuffer(buffer);
				buffer.SeekToBegin();
				srcAngle.FromPPIBuffer(buffer);
			}
			else 
				pSrcAngle=(CProcessAngle*)pProcessPart;
			ReverseAngle(pSrcAngle,&(pItem->m_xReverseAngle));
			pItem->bReverseAngle=TRUE;
		}
		CString str=m_pListCtrl->GetItemText(iCurSel,2);
#ifdef AFX_TARG_ENU_ENGLISH
		if(pItem->bReverseAngle)
			str+="(reverse)";
		else
			str.Replace("(reverse)","");
#else
		if(pItem->bReverseAngle)
			str+="(��)";
		else 
			str.Replace("(��)","");
#endif
		m_pListCtrl->SetItemText(iCurSel,2,str);
	}
	CString str;
	BOOL bIncrCol2Width=FALSE;
	int nCount=m_pListCtrl->GetItemCount();
	for(int j=0;j<nCount;j++)
	{
		if(!bIncrCol2Width)
		{
			str=m_pListCtrl->GetItemText(j,2);
			bIncrCol2Width=str.Find('(')>0;//����(��)��(��)��������Ҫ�Ӵ��п� wht 16-04-27
			if(bIncrCol2Width)
				break;
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	m_pListCtrl->SetColumnWidth(2,bIncrCol2Width?75:65);
#else
	m_pListCtrl->SetColumnWidth(2,bIncrCol2Width?65:45);
#endif
}
void CPartSetCreatorDlg::ConfirmCreate(bool bOnlySelecteItems)
{
	//����Ļ��ǰ��ʾ����
	STDPART_ITEM *pItem;
	ATOM_LIST<STDPART_ITEM>tmpartlist;
	int i,nCount=m_pListCtrl->GetItemCount();
	if(bOnlySelecteItems)
	{	//����ѡ����
		POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			i = m_pListCtrl->GetNextSelectedItem(pos);
			pItem=(STDPART_ITEM*)m_pListCtrl->GetItemData(i);
			tmpartlist.append(*pItem);
		}
	}
	else
	{
		for(i=0;i<nCount;i++)
		{
			pItem=(STDPART_ITEM*)m_pListCtrl->GetItemData(i);
			tmpartlist.append(*pItem);
		}
	}
	partList.Empty();
	for(pItem=tmpartlist.GetFirst();pItem;pItem=tmpartlist.GetNext())
		partList.append(*pItem);
	UpdateData();
	/*
	if(m_bPatternDrawing)
		PATTERN_DRAW_SCALE=atoi(m_sDrawScale);
	else
		PART_DRAW_SCALE	=atoi(m_sDrawScale);
	*/
	sys.general_set.PartMapScale.fRealToDraw=atoi(m_sDrawScale);
	CDialog::OnOK();
}
BOOL CPartSetCreatorDlg::GetSelectOnlyAngleOrPlate()	
{
	if(m_bIncTransectTube||m_bIncTube||m_bCreateTubeBrief||
		m_bAutoArrangePlate||m_bIncSlot||m_bAutoCreateFile|m_bCreateJgCutAngleBriefMap||m_bIncArcPart
		||m_bProcessPart)
		return FALSE;
	if((!m_bIncAngle&&(m_bIncPlate||m_bIncStdPart))||(m_bIncAngle&&!m_bIncPlate&&!m_bIncStdPart))
		return TRUE;
	else
		return FALSE;
}
void CPartSetCreatorDlg::OnUpdatePartType() 
{
	UpdateData();
	//ѡ�г��ֹ���Ϊ���������͹���ʱ���������ɸֹܼ�ͼ
	m_bCreateTubeBrief=FALSE;
	if(!m_bIncPlate)
	{
		m_bAutoArrangePlate = FALSE;
		GetDlgItem(IDC_CHK_TUBE)->EnableWindow(!m_bAutoArrangePlate);
		GetDlgItem(IDC_CHK_ANGLE)->EnableWindow(!m_bAutoArrangePlate);
		GetDlgItem(IDC_CHK_SLOT)->EnableWindow(!m_bAutoArrangePlate);
		GetDlgItem(IDC_CHK_TRANSECT_TUBE)->EnableWindow(!m_bAutoArrangePlate);
		GetDlgItem(IDC_CHK_ARCPART)->EnableWindow(!m_bAutoArrangePlate);
	}
	GetDlgItem(IDC_CHK_GROUP_BY_SEGI)->EnableWindow(GetSelectOnlyAngleOrPlate());
	if(!m_bIncTube||m_bIncAngle||m_bIncPlate||m_bIncTransectTube||m_bIncSlot||m_bIncArcPart)
		GetDlgItem(IDC_CHK_CREATE_TUBE_BRIEF)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_CHK_CREATE_TUBE_BRIEF)->EnableWindow(TRUE);
	if(m_bBatchPlot&&(m_bIncPlate||m_bIncStdPart)) //�������ɹ���ʱ��֧���Զ��Ű�ְ� wht 10-11-18
		GetDlgItem(IDC_CHK_AUTO_ARRANGE_PLATE)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_CHK_AUTO_ARRANGE_PLATE)->EnableWindow(FALSE);
	//Ŀǰ��֧�ֽ��Ǹ�������ɵ�����Dwg�ļ��� wht 11-06-02
	if(!m_bIncAngle||m_bIncTube||m_bIncPlate||m_bIncTransectTube||m_bIncSlot||m_bIncArcPart)
		GetDlgItem(IDC_CHK_AUTO_CREATE_FILE)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_CHK_AUTO_CREATE_FILE)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_CREATE_JG_CUTANGLE_BRIEF_MAP)->EnableWindow(m_bIncAngle);
	m_bAutoCreateFile=FALSE;
	m_bCreateJgCutAngleBriefMap=FALSE;
	UpdateData(FALSE);
	OnBnRebuildPartset();
}
void CPartSetCreatorDlg::AddProcessPart()
{
	CStringArray str_arr;
	str_arr.SetSize(6);
	DWORD *pValue=NULL;
	if(m_bIncPlate||m_bIncAngle)
	{
		for (CProcessPart *pPart=hashProcessParts.GetFirst();pPart;pPart=hashProcessParts.GetNext())
		{
			if(pValue=m_xItemPtrHashPartNo.GetValue(pPart->GetPartNo()))
			{
				STDPART_ITEM *pItem=(STDPART_ITEM*)(*pValue);
				pItem->SetProcessPart(pPart);
			}
			else if((m_bIncAngle&&pPart->m_cPartType==CProcessPart::TYPE_LINEANGLE) ||
				(m_bIncPlate&&pPart->m_cPartType==CProcessPart::TYPE_PLATE))
			{
				STDPART_ITEM *pItem=partList.append();
				pItem->bFlag=FALSE;
				pItem->SetProcessPart(pPart);
				pItem->num=1;
				m_xItemPtrHashPartNo.SetValue(pPart->GetPartNo(),(DWORD)pItem);
				str_arr[0]=pPart->GetPartNo();
				str_arr[1].Format("%s",(char*)pPart->GetSegI().ToString());
				char sProcess[50]="";
				if(pPart->m_cPartType==CProcessPart::TYPE_LINEANGLE)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					str_arr[2]="angle(middle)";
#else
					str_arr[2]="�Ǹ�(��)";
#endif
					CProcessAngle *pAngle=(CProcessAngle*)pPart;
					pAngle->GetProcessStr(sProcess,50);
				}
				else
#ifdef AFX_TARG_ENU_ENGLISH
					str_arr[2]="plate(middle)";
#else
					str_arr[2]="�ְ�(��)";
#endif
				str_arr[3]=pPart->GetSpec();
				str_arr[4]="1";
				str_arr[5].Format("%s",sProcess);
				int iCur=AppendRecord(-1,str_arr);
				m_pListCtrl->SetItemData(iCur,(DWORD)pItem);
			}
		}
	}
}
//��¼����������ļ�
static CProcessPart *NewProcessPart(int CLSTYPE_ID,DWORD key,void* pContext)
{
	CProcessPart *pPart=NULL;
	switch(CLSTYPE_ID)
	{
	case CProcessPart::TYPE_PLATE:
		pPart = new CProcessPlate();
		break;
	case CProcessPart::TYPE_LINEANGLE:
		pPart = new CProcessAngle();
		break;
	default:
		break;
	}
	return pPart;
}
static BOOL DeleteProcessPart(CProcessPart* pPart)
{
	switch(pPart->m_cPartType)
	{
	case CProcessPart::TYPE_PLATE:
		delete (CProcessPlate*)pPart;
		break;
	case CProcessPart::TYPE_LINEANGLE:
		delete (CProcessAngle*)pPart;
		break;
	default:
		break;
	}
	return TRUE;
}
void CPartSetCreatorDlg::OnAddProcessPartFile()
{
	hashProcessParts.CreateNewAtom=NewProcessPart;
	hashProcessParts.DeleteAtom=DeleteProcessPart;
	hashProcessParts.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
	CFileDialog dlg(TRUE,"ppi","neutral file",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,
		"neutral file(*.ppi)|*.ppi|all files(*.*)|*.*||");
#else
	CFileDialog dlg(TRUE,"ppi","�����ļ�",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,
		"�����ļ�(*.ppi)|*.ppi|�����ļ�(*.*)|*.*||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	POSITION pos=dlg.GetStartPosition();
	while(pos)
	{
		CString sFilePath=dlg.GetNextPathName(pos);//��ȡ�ļ��� 
		FILE *fp;
		if((fp=fopen(sFilePath,"rb"))==NULL)
			return ;
		CBuffer buffer;
		long file_len=0;
		fread(&file_len,sizeof(long),1,fp);
		buffer.Write(NULL,file_len);
		fread(buffer.GetBufferPtr(),file_len,1,fp);
		fclose(fp);
		buffer.SeekToBegin();
		BYTE cPartType=CProcessPart::RetrievedPartTypeAndLabelFromBuffer(buffer);
		buffer.SeekToBegin();
		CProcessPart *pPart=hashProcessParts.Add(0,cPartType);
		pPart->FromPPIBuffer(buffer);
	}
	AddProcessPart();
}
void CPartSetCreatorDlg::OnIncTransectTube()
{
	UpdateData();
	if(m_bIncTransectTube&&m_bIncTube)
		m_bIncTube=FALSE;
	//ѡ�г��ֹ���Ϊ���������͹���ʱ���������ɸֹܼ�ͼ
	m_bCreateTubeBrief=FALSE;
	m_bAutoArrangePlate = FALSE;
	if(!m_bIncTube||m_bIncAngle||m_bIncPlate||m_bIncTransectTube||m_bIncSlot||m_bIncArcPart)
		GetDlgItem(IDC_CHK_CREATE_TUBE_BRIEF)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_CHK_CREATE_TUBE_BRIEF)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_AUTO_ARRANGE_PLATE)->EnableWindow(m_bIncPlate
		&&!m_bIncTransectTube&&!m_bIncTube&&!m_bIncAngle&&!m_bIncSlot&&m_bBatchPlot); //��������ʱ��֧���Զ��Ű�ְ�
	UpdateData(FALSE);
	OnBnRebuildPartset();
}
void CPartSetCreatorDlg::OnIncTube()
{
	UpdateData();
	if(m_bIncTransectTube&&m_bIncTube)
		m_bIncTransectTube=FALSE;
	m_bCreateTubeBrief=FALSE;
	//��ѡ�иֹ�����,�������ɸֹܼ�ͼ
	if(m_bIncTube&&!m_bIncAngle&&!m_bIncPlate&&!m_bIncTransectTube&&!m_bIncSlot||!m_bIncArcPart)
		GetDlgItem(IDC_CHK_CREATE_TUBE_BRIEF)->EnableWindow(TRUE);
	else 
		GetDlgItem(IDC_CHK_CREATE_TUBE_BRIEF)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_PLATE)->EnableWindow(!m_bCreateTubeBrief);
	GetDlgItem(IDC_CHK_ANGLE)->EnableWindow(!m_bCreateTubeBrief);
	GetDlgItem(IDC_CHK_SLOT)->EnableWindow(!m_bCreateTubeBrief);
	GetDlgItem(IDC_CHK_TRANSECT_TUBE)->EnableWindow(!m_bCreateTubeBrief);
	GetDlgItem(IDC_CHK_ARCPART)->EnableWindow(!m_bCreateTubeBrief);
	GetDlgItem(IDC_CHK_STD_PART)->EnableWindow(!m_bCreateTubeBrief);
	GetDlgItem(IDC_CHK_AUTO_ARRANGE_PLATE)->EnableWindow((m_bIncPlate)
		&&!m_bIncTransectTube&&!m_bIncTube&&!m_bIncAngle&&!m_bIncSlot&&m_bBatchPlot); //��������ʱ��֧���Զ��Ű�ְ�
	UpdateData(FALSE);
	OnBnRebuildPartset();
}

void CPartSetCreatorDlg::OnCreateTubeBrief() 
{
	UpdateData();
	GetDlgItem(IDC_CHK_PLATE)->EnableWindow(!m_bCreateTubeBrief);
	GetDlgItem(IDC_CHK_ANGLE)->EnableWindow(!m_bCreateTubeBrief);
	GetDlgItem(IDC_CHK_SLOT)->EnableWindow(!m_bCreateTubeBrief);
	GetDlgItem(IDC_CHK_TRANSECT_TUBE)->EnableWindow(!m_bCreateTubeBrief);
	GetDlgItem(IDC_CHK_ARCPART)->EnableWindow(!m_bCreateTubeBrief);
	GetDlgItem(IDC_CHK_STD_PART)->EnableWindow(!m_bCreateTubeBrief);
	UpdateData(FALSE);
}

void CPartSetCreatorDlg::OnAutoArrangePlate() 
{
	UpdateData();
	GetDlgItem(IDC_CHK_TUBE)->EnableWindow(!m_bAutoArrangePlate);
	GetDlgItem(IDC_CHK_ANGLE)->EnableWindow(!m_bAutoArrangePlate);
	GetDlgItem(IDC_CHK_SLOT)->EnableWindow(!m_bAutoArrangePlate);
	GetDlgItem(IDC_CHK_TRANSECT_TUBE)->EnableWindow(!m_bAutoArrangePlate);
	GetDlgItem(IDC_CHK_ARCPART)->EnableWindow(!m_bAutoArrangePlate);
	GetDlgItem(IDC_CHK_STD_PART)->EnableWindow(!m_bCreateTubeBrief);
	UpdateData(FALSE);
}

void CPartSetCreatorDlg::OnRclickCmdListCommon(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CAcModuleResourceOverride resOverride;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CPoint point = pNMListView->ptAction;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_MENU_SCRIPT);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	if(pMenu==NULL)
		return;
	while(pMenu->GetMenuItemCount()>0)
		pMenu->DeleteMenu(0,MF_BYPOSITION);
#ifdef AFX_TARG_ENU_ENGLISH
	pMenu->AppendMenu(MF_STRING,ID_CREATE_ITEMS,"generate part map");
	pMenu->AppendMenu(MF_STRING,ID_REVERSE_ANGLE,"reverse angle");
	pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"New Angle");
#else
	pMenu->AppendMenu(MF_STRING,ID_CREATE_ITEMS,"���ɹ���ͼ");
	pMenu->AppendMenu(MF_STRING,ID_REVERSE_ANGLE,"��ת�Ǹ�");
	pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"��ӽǸ�");
#endif
	CPoint menu_pos=point;
	m_pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,this);
	*pResult = 0;
}
void CPartSetCreatorDlg::OnNewItem()
{
	CAcModuleResourceOverride resOverride;
	int iInsertPos=-1;
	CXhPtrSet<STDPART_ITEM> partItemSet;
	if(m_pListCtrl->GetSelectedCount()>0)
	{
		POSITION pos=m_pListCtrl->GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
			STDPART_ITEM *pItem=(STDPART_ITEM*)m_pListCtrl->GetItemData(iCurSel);
			if(pItem==NULL)
				continue;
			CLDSPart *pPart=pItem->GetPart();
			if(pPart==NULL||!pPart->IsAngle()||!((CLDSLineAngle*)pPart)->IsHasFootNail())
				continue;
			partItemSet.append(pItem);
			iInsertPos=max(iInsertPos,iCurSel);
		}
	}
	else
	{
		for(STDPART_ITEM *pItem=partList.GetFirst();pItem;pItem=partList.GetNext())
		{
			CLDSPart *pPart=pItem->GetPart();
			if(pPart==NULL||!pPart->IsAngle()||!((CLDSLineAngle*)pPart)->IsHasFootNail())
				continue;
			partItemSet.append(pItem);
		}
	}
	CAddAngleByRemoveNailDlg dlg;
	dlg.m_pPartItemSet=&partItemSet;
	if(dlg.DoModal()!=IDOK)
		return;
	CBuffer buffer;
	for(STDPART_ITEM *pPartItem=partItemSet.GetFirst();pPartItem;pPartItem=partItemSet.GetNext())
	{
		CLDSPart *pPart=pPartItem->GetPart();
		if(pPart==NULL||!pPart->IsAngle()||!((CLDSLineAngle*)pPart)->IsHasFootNail())
			continue;
		CProcessAngle *pNewAngle=hashProcessAngleBySrcLabel.GetValue(pPartItem->sLabelWithOutNail);
		if(pPartItem->sLabelWithOutNail.GetLength()<=0)
		{
			if(pNewAngle)
				hashProcessAngleBySrcLabel.DeleteNode(pPartItem->sLabelWithOutNail);
		}
		else if(pNewAngle==NULL)
		{
			pNewAngle=hashProcessAngleBySrcLabel.Add(pPartItem->sLabelWithOutNail);
			buffer.ClearContents();
			pPart->ToPPIBuffer(buffer);
			buffer.SeekToBegin();
			pNewAngle->FromPPIBuffer(buffer);
			pNewAngle->SetPartNo(pPartItem->sLabelWithOutNail);
			for(BOLT_INFO *pBoltInfo=pNewAngle->m_xBoltInfoList.GetFirst();pBoltInfo;pBoltInfo=pNewAngle->m_xBoltInfoList.GetNext())
			{
				if(pBoltInfo->cFuncType==BOLT_INFO::FUNC_FOOTNAIL)
					pNewAngle->m_xBoltInfoList.DeleteCursor(FALSE);
				else if(pBoltInfo->cFuncType==BOLT_INFO::FUNC_FOOTNAIL2)
					pBoltInfo->cFuncType=BOLT_INFO::FUNC_COMMON;
			}
			pNewAngle->m_xBoltInfoList.Clean();
		}
	}
	AddNewProcessAngle(iInsertPos);
}
void CPartSetCreatorDlg::AddNewProcessAngle(int iInsertPos/*=-1*/)
{
	CStringArray str_arr;
	str_arr.SetSize(6);
	if(m_bIncAngle)
	{
		for (CProcessAngle *pAngle=hashProcessAngleBySrcLabel.GetFirst();pAngle;pAngle=hashProcessAngleBySrcLabel.GetNext())
		{
			if(m_xItemPtrHashPartNo.GetValue(pAngle->GetPartNo()))
				continue;
			STDPART_ITEM *pItem=partList.append();
			pItem->bFlag=FALSE;
			pItem->SetProcessPart(pAngle);
			pItem->num=1;
			m_xItemPtrHashPartNo.SetValue(pAngle->GetPartNo(),(DWORD)pItem);
			str_arr[0]=pAngle->GetPartNo();
			str_arr[1].Format("%s",(char*)pAngle->GetSegI().ToString());
			char sProcess[50]="";
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[2]="angle(new)";
#else
			str_arr[2]="�Ǹ�(��)";
#endif
			pAngle->GetProcessStr(sProcess,50);
			str_arr[3]=pAngle->GetSpec();
			str_arr[4]="1";
			str_arr[5].Format("%s",sProcess);
			int iCur=AppendRecord(iInsertPos,str_arr);
			m_pListCtrl->SetItemData(iCur,(DWORD)pItem);
			if(iInsertPos>0)
				iInsertPos++;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		m_pListCtrl->SetColumnWidth(2,75);
#else
		m_pListCtrl->SetColumnWidth(2,65);
#endif
		if(iInsertPos>0)
			m_pListCtrl->EnsureVisible(iInsertPos,TRUE);
		else 
			m_pListCtrl->EnsureVisible(m_pListCtrl->GetItemCount()-1,TRUE);
	}
}
void CPartSetCreatorDlg::OnBnClickedChkGroupBySegi()
{
	UpdateData();
	CButton* pBox=(CButton*)GetDlgItem(IDC_CHK_ONE_SEG_ONE_FILE);
	if(pBox&&m_bGroupBySeg)
		pBox->EnableWindow(TRUE);
	else
	{
		pBox->EnableWindow(FALSE);
		m_bOneSegOneFile=m_bGroupBySeg;	
		UpdateData(FALSE);
	}
}
#endif


