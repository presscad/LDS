// displayPartInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "RetrieveSegIDlg.h"
#include "afxdialogex.h"
#include "env_def.h"
#include "SortFunc.h"

// CRetrieveSegIDlg 对话框

IMPLEMENT_DYNAMIC(CRetrieveSegIDlg, CDialogEx)

CRetrieveSegIDlg::CRetrieveSegIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRetrieveSegIDlg::IDD, pParent)
{

#ifdef AFX_TARG_ENU_ENGLISH
	m_xListCtrl.AddColumnHeader("handle");
	m_xListCtrl.AddColumnHeader("segI");
	m_xListCtrl.AddColumnHeader("now segI");
	m_xListCtrl.AddColumnHeader("part No");
	m_xListCtrl.AddColumnHeader("specification");
#else
	m_xListCtrl.AddColumnHeader("句柄");
	m_xListCtrl.AddColumnHeader("提取段号");
	m_xListCtrl.AddColumnHeader("段号");
	m_xListCtrl.AddColumnHeader("件号");
	m_xListCtrl.AddColumnHeader("规格");
#endif
	m_Segi = 0;
}

CRetrieveSegIDlg::~CRetrieveSegIDlg()
{
}

void CRetrieveSegIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DISPLAY_PART_INFO_LIST_CTRL, m_xListCtrl);
	DDX_Text(pDX, IDC_E_SEG_I, m_Segi);
}
BEGIN_MESSAGE_MAP(CRetrieveSegIDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BN_ADJUST_SEG_I, &CRetrieveSegIDlg::OnBnClickedBnAdjustSegI)
END_MESSAGE_MAP()


// displayPartInfoDlg 消息处理程序


static bool IsBriefMark(char cMat)
{
	if( CSteelMatLibrary::Q235BriefMark()==cMat||
		CSteelMatLibrary::Q345BriefMark()==cMat||
		CSteelMatLibrary::Q390BriefMark()==cMat||
		CSteelMatLibrary::Q420BriefMark()==cMat||
		CSteelMatLibrary::Q460BriefMark()==cMat)
		return true;
	else
		return false;
}
static SEGI RetrieveSegIByPartNo(CLDSPart *pPart,CHashSet<CLDSPart*> &hashDatumPartByHandle)
{
	CLDSPart *pCurPart=pPart;
	if(pPart->GetClassTypeId()==CLS_BOLT)
	{
		pCurPart=Ta.FromPartHandle(((CLDSBolt*)pPart)->des_base_pos.hPart);
		if(pCurPart&&hashDatumPartByHandle.GetValue(pCurPart->handle)!=NULL)
			pCurPart=hashDatumPartByHandle.GetValue(pCurPart->handle);
	}
	else if(pPart->GetClassTypeId()==CLS_LINESLOT&&((CLDSLineSlot*)pPart)->IsBaseSlot())
	{
		CLDSPart *pFatherPart=Ta.FromPartHandle(pPart->m_hPartWeldParent);
		if(pFatherPart!=NULL)
			pCurPart=pFatherPart;
	}
	if(pCurPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pCurPart;
		if(pParamPlate->IsFL()||pParamPlate->IsInsertPlate())
			pCurPart=Ta.FromPartHandle(pParamPlate->m_hPartWeldParent);
	}
	SEGI segI;
	if(pCurPart&&ParsePartNo(pCurPart->GetPartNo(),&segI,NULL))
	{
		if(IsBriefMark(segI.key.prefix[0]))
			segI.key.prefix[0]=' ';
		if(segI==0&&pCurPart->iSeg!=0)
			segI=pCurPart->iSeg;
	}
	else//提取失败保留原有段号
		segI=(pCurPart!=NULL)?pCurPart->iSeg:pPart->iSeg;
	return segI;
}

BOOL CRetrieveSegIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//初始化listctrl
	long col_wide_arr[5]={60,70,70,70,100};
	m_xListCtrl.InitListCtrl(col_wide_arr);
	m_xListCtrl.m_bEnableSortItems =TRUE;
	m_xListCtrl.SetColumnSortType(0,SORTTYPE_HEX_NUM);
	m_xListCtrl.SetColumnSortType(2,SORTTYPE_INTEGER);
	m_xListCtrl.SetColumnSortType(3,SORTTYPE_PARTNO);
	m_xListCtrl.SetColumnSortType(4,SORTTYPE_SPEC);
	//m_BnReviseSegI.EnableWindow(FALSE);
	m_xListCtrl.DeleteAllItems();
	CLDSPart *pPart=NULL;
	for(pPart=m_partSet.GetFirst();pPart;pPart=m_partSet.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
		if(pParamPlate->IsFL()||pParamPlate->IsInsertPlate())
		{
			for(ATTACHPART_ITEM* pItem=pParamPlate->AttachPart.GetFirst();pItem;pItem=pParamPlate->AttachPart.GetNext())
				m_hashDatumPartByHandle.SetValue(pItem->h,pParamPlate);
		}
	}
	//
	CStringArray str_arr;
	str_arr.SetSize(5);
	for(pPart=m_partSet.GetFirst();pPart;pPart=m_partSet.GetNext())
	{	//插入一行
		if(pPart->GetClassTypeId() == CLS_BOLT)
			continue;
		if(m_hashDatumPartByHandle.GetValue(pPart->handle)!=NULL)
			continue;
		str_arr[0].Format("0x%X",pPart->handle);
		str_arr[1]=RetrieveSegIByPartNo(pPart,m_hashDatumPartByHandle).ToString();
		str_arr[2]=pPart->iSeg.ToString();
		//件号
		str_arr[3]=pPart->GetPartNo();
		//规格
		CXhChar100 sSpec;
		pPart->GetSpecification(sSpec);
		str_arr[4]=(char*)sSpec;
		int	 iItem=m_xListCtrl.InsertItemRecord(-1,str_arr);
		m_xListCtrl.SetItemData(iItem,(DWORD)pPart);
	}
	UpdateData();
	return TRUE;
}
struct BOLT_RELAINFO{
	CLDSBolt* pBolt;
	CLDSPart* pPart;
	BOLT_RELAINFO(CLDSBolt* pBoltPtr=NULL,CLDSPart* pPartPtr=NULL){pBolt=pBoltPtr;pPart=pPartPtr;}
};
void CRetrieveSegIDlg::OnOK()
{
	int	 nItem = m_xListCtrl.GetItemCount();
	CHashList<BOLT_RELAINFO> hashBolts;
	for(int i =0; i<nItem;i++)
	{
		CString str=m_xListCtrl.GetItemText(i,1);
		SEGI segI(str);
		if(str.IsEmpty())
			continue;
		CLDSPart* pPart =(CLDSPart*)m_xListCtrl.GetItemData(i);
		pPart->iSeg=segI;
		CLsRefList* pLsRefList=pPart->GetLsRefList();
		if(pLsRefList)
		{
			for(CLsRef* pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
			{
				BOLT_RELAINFO* pBoltRela=hashBolts.GetValue((*pLsRef)->handle);
				if(pBoltRela&&!pBoltRela->pPart->IsPlate()&&pPart->IsPlate())
					pBoltRela->pPart=pPart;
				else if(pBoltRela==NULL)
					pBoltRela=hashBolts.SetValue((*pLsRef)->handle,BOLT_RELAINFO(pLsRef->GetLsPtr(),pPart));
			}
		}
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate* pParamPlate =(CLDSParamPlate*)pPart;
			AdjustInsertPartOrFLSegI(pParamPlate,segI);
		}
	}
	for(BOLT_RELAINFO* pBoltRela=hashBolts.GetFirst();pBoltRela;pBoltRela=hashBolts.GetNext())
		pBoltRela->pBolt->iSeg=pBoltRela->pPart->iSeg;
	//更新m_partSet关联节点段号
	for(CLDSNode *pNode=console.EnumNodeFirst();pNode;pNode=console.EnumNodeNext())
	{
		CLDSPart *pPart=m_partSet.FromHandle(pNode->hFatherPart);
		if(pPart)
			pNode->iSeg=pPart->iSeg;
	}
	UpdateData();
	CDialogEx::OnOK();
}

void CRetrieveSegIDlg::OnBnClickedBnAdjustSegI()
{
	UpdateData();
	int nSelected = m_xListCtrl.GetSelectedCount();
	if(nSelected<0)
		return;
	CString sSegI;
	sSegI.Format("%d",m_Segi);
	POSITION pos = m_xListCtrl.GetFirstSelectedItemPosition();
	while (pos)
	{
		int iItem = m_xListCtrl.GetNextSelectedItem(pos);
		m_xListCtrl.SetItemText(iItem,1,sSegI);
		m_xListCtrl.SetItemState(iItem, 0, -1);
	}
	UpdateData();
}

void CRetrieveSegIDlg::AdjustInsertPartOrFLSegI(CLDSParamPlate* pParamPlate,SEGI segI)
{
	if(!pParamPlate->IsFL()||!pParamPlate->IsInsertPlate())
	for (CLDSPart *pPart=m_partSet.GetFirst();pPart;pPart=m_partSet.GetNext())
	{
		CLDSPart* pDatumPart=m_hashDatumPartByHandle.GetValue(pPart->handle);
		if(pDatumPart==NULL)
			continue;
		pParamPlate->iSeg=segI;
	}
}
