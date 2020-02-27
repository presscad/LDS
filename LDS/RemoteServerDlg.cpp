// RemoteServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "ServerTowerType.h"
#include "RemoteServerDlg.h"
#include "afxdialogex.h"
#include "image.h"
#include "LDS.h"
#include "FileGroupDlg.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "UserDefMsg.h"
#include "InputAnValDlg.h"
#include "GlobalFunc.h"
#include "SortFunc.h"

#ifdef __CONNECT_REMOTE_SERVER_
#include "TMS.h"
// CRemoteServerDlg 对话框

IMPLEMENT_DYNCREATE(CRemoteServerDlg, CDialogEx)

CRemoteServerDlg::CRemoteServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRemoteServerDlg::IDD, pParent)
{
	m_hPartSetItem=m_hSyncPartSetItem=m_hServerContentItem=NULL;
}

CRemoteServerDlg::~CRemoteServerDlg()
{
}

void CRemoteServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CTRL, m_treeCtrl);
}


BEGIN_MESSAGE_MAP(CRemoteServerDlg, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_CTRL, OnDblclkTreeCtrl)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_CTRL, &CRemoteServerDlg::OnNMRClickTreeCtrl)
	ON_COMMAND(ID_CONNECT_REMOTE_SERVER, OnConnectServer)
	ON_COMMAND(ID_DISCONNECT_REMOTE_SERVER, OnDisconnectServer)
	ON_COMMAND(ID_DOWNLOAD_ITEM, OnDownloadItem)
	ON_COMMAND(ID_UPLOAD_ITEM, OnUploadItem)
	ON_COMMAND(ID_ATTACH_MODEL_TO_SERVER, OnMappingWithCurrentModel)
	ON_COMMAND(ID_SAVE_AS_TOWERFILE, OnSaveAsTowerFile)
	ON_COMMAND(ID_NEW_ITEM, OnNewItem)
	ON_COMMAND(ID_DELETE_ITEM,OnDeleteItem)
	ON_COMMAND(ID_EDIT_ITEM_PROP_ITEM, OnEditItemPropItem)
	ON_COMMAND(ID_SET_ACTIVE_ITEM, OnSetActiveItem)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE_CTRL, &CRemoteServerDlg::OnTvnItemexpandingTreeCtrl)
END_MESSAGE_MAP()
CTowerTreeCtrl *CRemoteServerDlg::GetTreeCtrl()
{
	return &m_treeCtrl;
}
BOOL CRemoteServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	RECT rc;
	GetClientRect(&rc);
	CTowerTreeCtrl *pTreeCtrl = GetTreeCtrl();
	if(pTreeCtrl)
		pTreeCtrl->MoveWindow(&rc);
	m_ModelImages.Create(IDB_IL_TAB, 16, 1, RGB(0,255,0));
	pTreeCtrl->SetImageList(&m_ModelImages,TVSIL_NORMAL);
	pTreeCtrl->ModifyStyle(0,TVS_HASLINES|TVS_HASBUTTONS);
	RefreshTreeCtrl();
	return TRUE;
}

//defined in LDSView.cpp
void ReadProcessPartsFromAttachBuffer(CBuffer* pAttachBuff,CSuperHashStrList<CProcessPart> &hashPartByPartNo);
void CRemoteServerDlg::RefreshTreeCtrl()
{
	TREEITEM_INFO *pItemInfo;
	itemInfoList.Empty();
	CString ss;
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	pTreeCtrl->DeleteAllItems();

	HTREEITEM hTowerTypeGroupItem=NULL;
	int IMG_SERVER=theApp.m_bNetModelWork?IMG_MACHINE_SERVER:IMG_MACHINE_SERVER_OFFLINE;
	m_hServerContentItem=pTreeCtrl->InsertItem("服务器",IMG_SERVER,IMG_SERVER,TVI_ROOT);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_CONTENT,NULL));
	pTreeCtrl->SetItemData(m_hServerContentItem,(DWORD)pItemInfo);
	if(theApp.m_bNetModelWork)
	{
		//塔型组分类节点
		hTowerTypeGroupItem=pTreeCtrl->InsertItem("放样塔型",IMG_TOWER_TYPE,IMG_TOWER_TYPE,m_hServerContentItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERTYPESET1,NULL));
		pTreeCtrl->SetItemData(hTowerTypeGroupItem,(DWORD)pItemInfo);
		hTowerTypeGroupItem=pTreeCtrl->InsertItem("试组塔型",IMG_TOWER_TYPE,IMG_TOWER_TYPE,m_hServerContentItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERTYPESET2,NULL));
		pTreeCtrl->SetItemData(hTowerTypeGroupItem,(DWORD)pItemInfo);
		hTowerTypeGroupItem=pTreeCtrl->InsertItem("存档塔型",IMG_TOWER_TYPE,IMG_TOWER_TYPE,m_hServerContentItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERTYPESET3,NULL));
		pTreeCtrl->SetItemData(hTowerTypeGroupItem,(DWORD)pItemInfo);
		pTreeCtrl->Expand(m_hServerContentItem,TVE_EXPAND);
		//工件集合
		m_hPartSetItem=pTreeCtrl->InsertItem("构件集",IMG_TOWERPARTSET,IMG_TOWERPARTSET,TVI_ROOT);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPARTSET,NULL));
		pTreeCtrl->SetItemData(m_hPartSetItem,(DWORD)pItemInfo);
		//待同步工件集合
		m_hSyncPartSetItem=pTreeCtrl->InsertItem("待同步",IMG_TOWERPARTSET_SYNC,IMG_TOWERPARTSET_SYNC,TVI_ROOT);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPARTSET_SYNC,NULL));
		pTreeCtrl->SetItemData(m_hSyncPartSetItem,(DWORD)pItemInfo);
		//测试代码
		CProcessPart* pProcessPart;
		CSuperHashStrList<CProcessPart> hashPartsByPartNo;
		CAttachBuffer *pPpeModelBuffer=Ta.AttachBuffer.GetValue(CTower::PPE_MODEL);
		ReadProcessPartsFromAttachBuffer(pPpeModelBuffer,hashPartsByPartNo);
		CHashList<SEGI> hashSegIs;
		for(pProcessPart=hashPartsByPartNo.GetFirst();pProcessPart;pProcessPart=hashPartsByPartNo.GetNext())
			hashSegIs.SetValue(pProcessPart->iSeg.iSeg,pProcessPart->iSeg);
		for(SEGI* pSegI=hashSegIs.GetFirst();pSegI;pSegI=hashSegIs.GetNext())
		{
			HTREEITEM hSegmentItem=pTreeCtrl->InsertItem(CXhChar16("%s段",(char*)pSegI->ToString()),IMG_TOWERPART_SEGMENT,IMG_TOWERPART_SEGMENT,m_hPartSetItem);
			pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPART_SEGMENT,pSegI->iSeg));
			pTreeCtrl->SetItemData(hSegmentItem,(DWORD)pItemInfo);
			for(pProcessPart=hashPartsByPartNo.GetFirst();pProcessPart;pProcessPart=hashPartsByPartNo.GetNext())
			{
				if(pProcessPart->iSeg.iSeg!=pSegI->iSeg)
					continue;
				int iItemImage=pProcessPart->m_cPartType==CProcessPart::TYPE_LINEANGLE?IMG_ANGLEPART:IMG_PLATEPART;
				HTREEITEM hPartItem=pTreeCtrl->InsertItem(pProcessPart->Label,iItemImage,iItemImage,hSegmentItem);
				pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPART,NULL));
				pTreeCtrl->SetItemData(hSegmentItem,(DWORD)pItemInfo);
				//hPartItem=pTreeCtrl->InsertItem("102",IMG_TOWERPART_SYNCING,IMG_TOWERPART_SYNCING,hSegmentItem);
				//pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPART,NULL));
				//pTreeCtrl->SetItemData(hSegmentItem,(DWORD)pItemInfo);
				//hPartItem=pTreeCtrl->InsertItem("103",IMG_TOWERPART_DYNAMIC,IMG_TOWERPART_DYNAMIC,hSegmentItem);
				//pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPART,NULL));
				//pTreeCtrl->SetItemData(hSegmentItem,(DWORD)pItemInfo);
				pTreeCtrl->Expand(m_hPartSetItem,TVE_EXPAND);
			}
		}
		/*HTREEITEM hTowerItem=pTreeCtrl->InsertItem("文件",PRJ_IMG_CALMODULE,PRJ_IMG_CALMODULE,m_hServerContentItem);
		TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERFILE,(DWORD)&pServerTowerFile));
		pTreeCtrl->SetItemData(hTowerItem,(DWORD)pItemInfo);
		//
		hTowerItem=pTreeCtrl->InsertItem("文件组",PRJ_IMG_CALMODULE,PRJ_IMG_CALMODULE,m_hServerContentItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERFILEGROUP,(DWORD)&pTowerFileGroup));
		pTreeCtrl->SetItemData(hTowerItem,(DWORD)pItemInfo);
		//
		hTowerItem=pTreeCtrl->InsertItem("构件",PRJ_IMG_CALMODULE,PRJ_IMG_CALMODULE,m_hServerContentItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPART,(DWORD)&pTowerFileGroup));
		pTreeCtrl->SetItemData(hTowerItem,(DWORD)pItemInfo);
		//
		hTowerItem=pTreeCtrl->InsertItem("构件记录",PRJ_IMG_CALMODULE,PRJ_IMG_CALMODULE,m_hServerContentItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERFILEASSEMBLY1,(DWORD)&pTowerFileGroup));
		pTreeCtrl->SetItemData(hTowerItem,(DWORD)pItemInfo);*/
		//
		//RefreshServerContent();
	}
}
// CRemoteServerDlg 消息处理程序
void CRemoteServerDlg::RefreshServerContent(HTREEITEM hTowerTypeGroupItem)
{
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hTowerTypeGroupItem);
	if( pItemInfo==NULL||(pItemInfo->itemType!=SERVER_TOWERTYPESET1&&
		pItemInfo->itemType!=SERVER_TOWERTYPESET2&&pItemInfo->itemType!=SERVER_TOWERTYPESET3))
		return;
	//1.清空原有子项
	HTREEITEM hChildItem=m_treeCtrl.GetChildItem(hTowerTypeGroupItem);
	while(hChildItem)
	{
		pTreeCtrl->DeleteItem(hChildItem);
		hChildItem=m_treeCtrl.GetNextItem(hTowerTypeGroupItem,TVGN_CHILD);
	}
	//2.更新服务器数据树节点
	BOOL bExpandTowerItem=FALSE;
	CServerTowerType *pTower=NULL;
	for(pTower=AgentServer.hashTowerTypes.GetFirst();pTower;pTower=AgentServer.hashTowerTypes.GetNext())
	{
		bExpandTowerItem=FALSE;
		if( (pTower->state!=1&&pItemInfo->itemType==SERVER_TOWERTYPESET1)||
			(pTower->state!=2&&pItemInfo->itemType==SERVER_TOWERTYPESET2)||
			(pTower->state!=3&&pItemInfo->itemType==SERVER_TOWERTYPESET3))
			continue;
		CXhChar200 sText("%s(%d)",(char*)pTower->m_sName,pTower->Id);
		HTREEITEM hTowerItem=pTreeCtrl->InsertItem(sText,IMG_TOWER_TYPE,IMG_TOWER_TYPE,hTowerTypeGroupItem);
		TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERTYPE,(DWORD)pTower));
		pTreeCtrl->SetItemData(hTowerItem,(DWORD)pItemInfo);
		//
		BOOL bExpandFileGroupItem=FALSE;
		CServerTowerFileGroup *pFileGroup=NULL;
		for(pFileGroup=pTower->m_towerFileGroupList.GetFirst();pFileGroup;pFileGroup=pTower->m_towerFileGroupList.GetNext())
		{
			bExpandFileGroupItem=FALSE;
			HTREEITEM hFileGroupItem=pTreeCtrl->InsertItem(pFileGroup->m_sName,IMG_TOWER_FILE,IMG_TOWER_FILE,hTowerItem);
			pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERFILEGROUP,(DWORD)pFileGroup));
			pTreeCtrl->SetItemData(hFileGroupItem,(DWORD)pItemInfo);
			CServerTowerFile *pFile=NULL;
			for(pFile=pFileGroup->hashTowerFiles.GetFirst();pFile;pFile=pFileGroup->hashTowerFiles.GetNext())
			{
				CXhChar200 filename("V%d",pFile->dwVersion);
				filename.Append(pFile->m_sFileName,'#');
				HTREEITEM hFileItem=pTreeCtrl->InsertItem(filename,IMG_TOWER_FILE,IMG_TOWER_FILE,hFileGroupItem);
				pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERFILE,(DWORD)pFile));
				pTreeCtrl->SetItemData(hFileItem,(DWORD)pItemInfo);
				if(AgentServer.m_pActiveTowerFile==pFile)
				{
					pTreeCtrl->SetItemState(hFileItem,TVIS_BOLD,TVIS_BOLD);
					bExpandFileGroupItem=TRUE;
				}
			}
			if(bExpandFileGroupItem)
			{
				pTreeCtrl->Expand(hFileGroupItem,TVE_EXPAND);
				bExpandTowerItem=TRUE;
			}
		}
		if(bExpandTowerItem)
			pTreeCtrl->Expand(hTowerItem,TVE_EXPAND);
	}
	pTreeCtrl->Expand(hTowerTypeGroupItem,TVE_EXPAND);
}
static int _LocalCompareSEGI(const SEGI& seg1,const SEGI& seg2)
{
	if(seg1.Digital()>seg2.Digital())
		return 1;
	else if(seg1.Digital()<seg2.Digital())
		return -1;
	else
		return seg1.iSeg-seg2.iSeg;
}
#include "ComparePartNoString.h"
static int _LocalComparePartNo(const CServerTowerPartPtr& p1,const CServerTowerPartPtr& p2)
{
	return ComparePartNoString(p1->ppi.Label,p2->ppi.Label,"HGP");
}
void CRemoteServerDlg::RefreshProcessPartSet(HTREEITEM hPartSetItem)
{	//更新工件集合
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hPartSetItem);
	if(pItemInfo==NULL||pItemInfo->itemType!=SERVER_TOWERPARTSET)
		return;
	//1.清空原有子项
	HTREEITEM hChildItem=m_treeCtrl.GetChildItem(hPartSetItem);
	while(hChildItem)
	{
		pTreeCtrl->DeleteItem(hChildItem);
		hChildItem=m_treeCtrl.GetNextItem(hPartSetItem,TVGN_CHILD);
	}
	CServerTowerPart* pProcessPart;
	CHashList<SEGI> hashSegIs;
	for(pProcessPart=hashProcessParts.GetFirst();pProcessPart;pProcessPart=hashProcessParts.GetNext())
		hashSegIs.SetValue(pProcessPart->ppi.iSeg.iSeg,pProcessPart->ppi.iSeg);
	DYN_ARRAY<SEGI> arrValidSegs(hashSegIs.GetNodeNum());
	UINT i=0,j=0;
	for(SEGI* pSegI=hashSegIs.GetFirst();pSegI;pSegI=hashSegIs.GetNext(),i++)
		arrValidSegs[i]=*pSegI;
	CQuickSort<SEGI>::QuickSort(arrValidSegs,arrValidSegs.Size(),_LocalCompareSEGI);
	DYN_ARRAY<CServerTowerPart*> arrPartPtrs(hashProcessParts.GetNodeNum());
	for(pProcessPart=hashProcessParts.GetFirst(),j=0;pProcessPart;pProcessPart=hashProcessParts.GetNext(),j++)
		arrPartPtrs[j]=pProcessPart;
	CHeapSort<CServerTowerPart*>::HeapSort(arrPartPtrs,arrPartPtrs.Size(),_LocalComparePartNo);
	for(i=0;i<arrValidSegs.Size();i++)
	{
		SEGI* pSegI=&arrValidSegs[i];
		HTREEITEM hSegmentItem=pTreeCtrl->InsertItem(CXhChar16("%s段",(char*)pSegI->ToString()),IMG_TOWERPART_SEGMENT,IMG_TOWERPART_SEGMENT,m_hPartSetItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPART_SEGMENT,pSegI->iSeg));
		pTreeCtrl->SetItemData(hSegmentItem,(DWORD)pItemInfo);
		for(j=0;j<arrPartPtrs.Size();j++)
		{
			pProcessPart=arrPartPtrs[j];
			if(pProcessPart->ppi.iSeg.iSeg!=pSegI->iSeg)
				continue;
			int iItemImage=pProcessPart->ppi.m_cPartType==CProcessPart::TYPE_LINEANGLE?IMG_ANGLEPART:IMG_PLATEPART;
			HTREEITEM hPartItem=pTreeCtrl->InsertItem(pProcessPart->ppi.Label,iItemImage,iItemImage,hSegmentItem);
			pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPART,(DWORD)pProcessPart));
			pTreeCtrl->SetItemData(hSegmentItem,(DWORD)pItemInfo);
			pTreeCtrl->Expand(m_hPartSetItem,TVE_EXPAND);
		}
	}
	pTreeCtrl->Expand(hPartSetItem,TVE_EXPAND);
}

static CProcessPart* CreateProcessPart(int idPartClsType,DWORD key,void* pContext)
{
	CProcessPart* pPart=NULL;
	if(idPartClsType==CProcessPart::TYPE_LINEANGLE)
		pPart=new CProcessAngle();
	else if(idPartClsType==CProcessPart::TYPE_PLATE)
		pPart=new CProcessPlate();
	else
		pPart=new CProcessPart();	
	return pPart;
}
static BOOL DestroyProcessPart(CProcessPart* pPart)
{
	if(pPart->m_cPartType==CProcessPart::TYPE_LINEANGLE)
		delete (CProcessAngle*)pPart;
	else if(pPart->m_cPartType==CProcessPart::TYPE_PLATE)
		delete (CProcessPlate*)pPart;
	else
		delete (CProcessPart*)pPart;
	return TRUE;
}
void CRemoteServerDlg::RetrievedPartSet()
{
	CServerTowerFileGroup *pFileGroup=AgentServer.m_pActiveTowerFile->m_pBelongFileGroup;
	if(pFileGroup==NULL)
		return;
	CHashList<SEGI> segNoHashTbl;
	CXhChar200 sSegStr("%s,%s",(char*)pFileGroup->m_sSegStr,(char*)pFileGroup->shareSegIncStr);
	GetSegNoHashTblBySegStr(sSegStr,segNoHashTbl);
	if(segNoHashTbl.GetNodeNum()<=0)
	{
		AfxMessageBox("当前文件组未设置启用段号范围,无法提取待同步构件!");
		return;
	}
	CBuffer* pBuff=Ta.AttachBuffer.GetValue(CTower::PPE_MODEL);
	CProcessPartList hashPartsFromBuff;
	if(pBuff)
		ReadProcessPartsFromAttachBuffer(pBuff,hashPartsFromBuff);
	CLDSPart *pPart;
	POLYPARTLIST polylist;
	Ta.RetrievePolyPartList(polylist);
	int i=0;
	CBuffer partbuf(5120);	//5120=4096+1024为经验值，避免重复分配内存
	CServerTowerPart* pProcessPart;
	hashProcessParts.Empty();
	for(pPart=Ta.EnumPartFirst();pPart!=NULL;pPart=Ta.EnumPartNext(),i++)
	{
		if(pPart->m_bVirtualPart||pPart->GetClassTypeId()==CLS_BOLT)
			continue;	//隐含构件和螺栓不统计
		else if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			continue;	//材料汇总时不统计组合角钢(单线)重量 wht 11-03-11
		else if(pPart->IsLinePart())
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
			if(pLinePart->bHuoQuStart||pLinePart->bHuoQuEnd)
				continue;	//火曲子构件由火曲构件进行统计
		}
		if(pPart->Label[0]==0)
			continue;
		else if(!pPart->IsAngle()&&!pPart->IsPlate())
			continue;	//目前仅处理角钢与钢板两种构件的工艺
		else if(hashProcessParts.GetValue(pPart->Label)!=NULL)
			continue;	//已写入该构件
		if(segNoHashTbl.GetValue(pPart->iSeg)==NULL)
			continue;
		if((pProcessPart=hashProcessParts.Add(pPart->Label))!=NULL)
		{
			partbuf.ClearContents();
			CLdsPartListStack stack(&Ta);	//ToPPIBuffer中会涉及Ta.Parts遍历
			pPart->ToPPIBuffer(partbuf);
			partbuf.SeekToBegin();
			pProcessPart->FromPPIBuffer(partbuf);
		}
	}
	for(CLDSPolyPart *pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
	{
		CLDSPart* pFirstSegment=pPolyPart->segset.GetFirst();
		if(pFirstSegment==NULL||pFirstSegment->Label[0]==0||!pFirstSegment->IsAngle())
			continue;
		if(segNoHashTbl.GetValue(pPolyPart->iSeg)==NULL)
			continue;
		pProcessPart=hashProcessParts.Add(pPolyPart->Label);//,CProcessPart::TYPE_LINEANGLE);
		partbuf.ClearContents();
		pPolyPart->ToPPIBuffer(partbuf);
		partbuf.SeekToBegin();
		pProcessPart->FromPPIBuffer(partbuf);
	}
	for(pProcessPart=hashProcessParts.GetFirst();pProcessPart;pProcessPart=hashProcessParts.GetNext())
	{
		CProcessPart* pLegacyPpePart=hashPartsFromBuff.GetValue(pProcessPart->ppi.Label);
		if(pLegacyPpePart)
			pProcessPart->ppi.MergeToSelfWithPPE(pLegacyPpePart);
	}
}
void CRemoteServerDlg::RetrievedSyncPartSet()
{
	UINT i=0,j=0;
	CServerTowerPart* pProcessPart;
	DYN_ARRAY<UINT>verarr(hashProcessParts.GetNodeNum());
	hashSyncTowerParts.Empty();
	ARRAY_LIST<int> idarr;
	ARRAY_LIST<CServerTowerPart*> localParts;
	for(pProcessPart=hashProcessParts.GetFirst(),i=0;pProcessPart;pProcessPart=hashProcessParts.GetNext(),i++)
	{
		CServerTowerPart* pSyncPart=NULL;
		CTmsServer::VERSION_SERIAL *pVerSerial=TMS.hashPartsVerSerial.GetValue(pProcessPart->ppi.Label);
		if( pVerSerial==NULL||pProcessPart->Id<=0||pProcessPart->m_idSrvFileObj<=0)
		{	//需要上传
			pSyncPart=hashSyncTowerParts.Add(pProcessPart->ppi.Label);//,pProcessPart->ppi.m_cPartType);
			pSyncPart->m_cbSyncState=CServerTowerPart::SYNC_UPLOAD;
		}
		else if((pVerSerial->activePpi0Tid1==1&&pVerSerial->uiTidVerSerial>0)||
				(pVerSerial->activePpi0Tid1!=1&&pProcessPart->m_uiCurrTidVerSerial>pVerSerial->uiTidVerSerial))
		{	//服务器版本为TAP上传，TMA需要下载更新
			pSyncPart=hashSyncTowerParts.Add(pProcessPart->ppi.Label);//,pProcessPart->ppi.m_cPartType);
			pSyncPart->m_cbSyncState=CServerTowerPart::SYNC_DOWNLOAD;
		}
		else if(pVerSerial->activePpi0Tid1!=1&&pProcessPart->m_uiCurrPpiVerSerial>pVerSerial->uiPpiVerSerial)
		{	//服务器端有更新版本，存在冲突
			pSyncPart=hashSyncTowerParts.Add(pProcessPart->ppi.Label);//,pProcessPart->ppi.m_cPartType);
			pSyncPart->m_cbSyncState=CServerTowerPart::SYNC_JUSTIFY;
		}
		if(pSyncPart)
			pProcessPart->ClonePart(pSyncPart);
		else
		{
			localParts.append(pProcessPart);
			idarr.append(pProcessPart->ppi.Key);
		}
	}
	CSuperHashList<CProcessPart> hashServerParts;
	hashServerParts.CreateNewAtom=CreateProcessPart;
	hashServerParts.DeleteAtom=DestroyProcessPart;
	CBuffer rsltbuf;
	if(TMS.DownloadObjects("PpiPart",idarr.m_pData,idarr.GetSize(),0,&rsltbuf))
	{
		rsltbuf.SeekToBegin();
		int count=rsltbuf.ReadInteger();
		for(int i=0;i<count;i++)
		{
			int id=rsltbuf.ReadInteger();
			int len=rsltbuf.ReadInteger();
			if(len==0)
				continue;
			BYTE cPartType=CProcessPart::RetrievedPartTypeAndLabelFromBuffer(rsltbuf);
			CProcessPart* pPpiPart=hashServerParts.Add(id,cPartType);
			pPpiPart->FromPPIBuffer(rsltbuf);
			pProcessPart=localParts[i];
			if(!pPpiPart->IsEqual(&pProcessPart->ppi,0.5))
			{
				CServerTowerPart* pSyncPart=hashSyncTowerParts.Add(pProcessPart->ppi.Label);
				pSyncPart->m_cbSyncState=CServerTowerPart::SYNC_UPLOAD;
				pProcessPart->ClonePart(pSyncPart);
			}
		}
	}
}
void CRemoteServerDlg::RefreshSyncPartSet(HTREEITEM hPartSetItem)
{	//更新工件集合
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hPartSetItem);
	if(pItemInfo==NULL||pItemInfo->itemType!=SERVER_TOWERPARTSET_SYNC)
		return;
	//0.重新提取待同步构件集合
	RetrievedSyncPartSet();
	//1.清空原有子项
	HTREEITEM hChildItem=m_treeCtrl.GetChildItem(hPartSetItem);
	while(hChildItem)
	{
		pTreeCtrl->DeleteItem(hChildItem);
		hChildItem=m_treeCtrl.GetNextItem(hPartSetItem,TVGN_CHILD);
	}
	//2.更新待同步构件树节点
	UINT i=0,j=0;
	CHashList<SEGI> hashSegIs;
	CServerTowerPart *pProcessPart=NULL;
	for(pProcessPart=hashSyncTowerParts.GetFirst();pProcessPart;pProcessPart=hashSyncTowerParts.GetNext())
		hashSegIs.SetValue(pProcessPart->ppi.iSeg.iSeg,pProcessPart->ppi.iSeg);
	SEGI* pSegI;
	DYN_ARRAY<SEGI> arrValidSegs(hashSegIs.GetNodeNum());
	for(pSegI=hashSegIs.GetFirst(),i=0;pSegI;pSegI=hashSegIs.GetNext(),i++)
		arrValidSegs[i]=*pSegI;
	CQuickSort<SEGI>::QuickSort(arrValidSegs,arrValidSegs.Size(),_LocalCompareSEGI);
	DYN_ARRAY<CServerTowerPart*> arrPartPtrs(hashSyncTowerParts.GetNodeNum());
	for(pProcessPart=hashSyncTowerParts.GetFirst(),j=0;pProcessPart;pProcessPart=hashSyncTowerParts.GetNext(),j++)
		arrPartPtrs[j]=pProcessPart;
	CHeapSort<CServerTowerPart*>::HeapSort(arrPartPtrs,arrPartPtrs.Size(),_LocalComparePartNo);
	int idArr[3]={IMG_PARTSET_UPLOAD,IMG_PARTSET_DOWNLOAD,IMG_TOWERPARTSET_SYNC};
	int itemTypeArr[3]={SERVER_PARTSET_UPLOAD,SERVER_PARTSET_DOWNLOAD,SERVER_PARTSET_JUSTIFY};
	int stateArr[3]={CServerTowerPart::SYNC_UPLOAD,CServerTowerPart::SYNC_DOWNLOAD,CServerTowerPart::SYNC_JUSTIFY};
	CXhChar16 itemNameArr[3]={CXhChar16("待上传"),CXhChar16("待下载"),CXhChar16("待选择")};
	for(int k=0;k<3;k++)
	{
		HTREEITEM hPartSetItem=pTreeCtrl->InsertItem(itemNameArr[k],idArr[k],idArr[k],m_hSyncPartSetItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(itemTypeArr[k],NULL));
		pTreeCtrl->SetItemData(hPartSetItem,(DWORD)pItemInfo);
		for(i=0;i<arrValidSegs.Size();i++)
		{
			pSegI=&arrValidSegs[i];
			for(j=0;j<arrPartPtrs.Size();j++)
			{
				pProcessPart=arrPartPtrs[j];
				if(pProcessPart->ppi.iSeg.iSeg!=pSegI->iSeg)
					continue;
				if(pProcessPart->m_cbSyncState==stateArr[k])
					break;
			}
			if(j>=arrPartPtrs.Size())
				continue;	//当前段当前状态无构件，不需要添加
			HTREEITEM hSegmentItem=pTreeCtrl->InsertItem(CXhChar16("%s段",(char*)pSegI->ToString()),IMG_TOWERPART_SEGMENT,IMG_TOWERPART_SEGMENT,hPartSetItem);
			pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPART_SEGMENT,pSegI->iSeg));
			pTreeCtrl->SetItemData(hSegmentItem,(DWORD)pItemInfo);
			for(j=0;j<arrPartPtrs.Size();j++)
			{
				pProcessPart=arrPartPtrs[j];
				if(pProcessPart->ppi.iSeg.iSeg!=pSegI->iSeg)
					continue;
				int iItemImage=0;
				if(pProcessPart->ppi.m_cPartType==CProcessPart::TYPE_LINEANGLE)
					iItemImage=pProcessPart->Id<=0?IMG_ANGLEPART_NEW:IMG_ANGLEPART_SYNCING;
				else
					iItemImage=pProcessPart->Id<=0?IMG_PLATEPART_NEW:IMG_PLATEPART_SYNCING;
				HTREEITEM hPartItem=pTreeCtrl->InsertItem(pProcessPart->ppi.Label,iItemImage,iItemImage,hSegmentItem);
				pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPART_SYNCING,(DWORD)pProcessPart));
				pTreeCtrl->SetItemData(hPartItem,(DWORD)pItemInfo);
				pTreeCtrl->Expand(m_hPartSetItem,TVE_EXPAND);
			}
		}
	}
	pTreeCtrl->Expand(hPartSetItem,TVE_EXPAND);
}
void CRemoteServerDlg::ContextMenu(CWnd *pWnd, CPoint point)
{
	CPoint scr_point = point;
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	pTreeCtrl->ClientToScreen(&scr_point);
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	TREEITEM_INFO *pItemInfo=(hItem!=NULL)?(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem):NULL;
	if(pItemInfo==NULL)
		return;
	if(pItemInfo->itemType==SERVER_CONTENT)
	{
		if(TMS.m_pServer==NULL)
			pMenu->AppendMenu(MF_STRING,ID_CONNECT_REMOTE_SERVER,"连接");
		else
			pMenu->AppendMenu(MF_STRING,ID_DISCONNECT_REMOTE_SERVER,"断开");
	}
	else if(pItemInfo->itemType==SERVER_TOWERTYPESET1)
	{
		pMenu->AppendMenu(MF_STRING,ID_DOWNLOAD_ITEM,"加载塔型");
		//暂时不支持新建以下命令，后期需要完善 wht 17-12-06
		//pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		//pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"新建塔型");
	}
	else if(pItemInfo->itemType==SERVER_TOWERTYPESET2)
	{
		pMenu->AppendMenu(MF_STRING,ID_DOWNLOAD_ITEM,"加载塔型");
	}
	else if(pItemInfo->itemType==SERVER_TOWERTYPESET3)
	{
		pMenu->AppendMenu(MF_STRING,ID_DOWNLOAD_ITEM,"加载塔型");
	}
	else if(pItemInfo->itemType==SERVER_TOWERTYPE)
	{	//暂时不支持新建以下命令，后期需要完善 wht 17-12-06
		/*pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"编辑塔型");
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"新建分段文件");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除塔型");*/
	}
	else if(pItemInfo->itemType==SERVER_TOWERFILEGROUP)
	{
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"编辑");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		//pMenu->AppendMenu(MF_STRING,ID_ATTACH_MODEL_TO_SERVER,"附加当前模型");
		CServerTowerFileGroup* pFileGroup=(CServerTowerFileGroup*)pItemInfo->dwRefData;
		//if(pFileGroup->hashTowerFiles.GetNodeNum()==0&&pFileGroup->m_idActiveFile<=0)
		//	pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"上传至");
		//else //if(pFileGroup!=NULL&&pFileGroup->m_idActiveFile<=0)
		//	pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"上传为新版本");
		//if(pFileGroup!=NULL&&pFileGroup->m_idActiveFile>0)
		//	pMenu->AppendMenu(MF_STRING,ID_UPLOAD_ITEM,"覆盖当前版本");
		//pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除");
	}
	else if(pItemInfo->itemType==SERVER_TOWERFILE)
	{
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"激活文件");
		//pMenu->AppendMenu(MF_STRING,ID_DOWNLOAD_ITEM,"加载文件");
		//pMenu->AppendMenu(MF_STRING,ID_UPLOAD_ITEM,"上传文件");
		//pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"重命名文件");
		//pMenu->AppendMenu(MF_STRING,ID_SAVE_AS_TOWERFILE,"另存为...");
		CServerTowerFile* pFile=(CServerTowerFile*)pItemInfo->dwRefData;
		if(pFile!=NULL&&AgentServer.GetActiveTowerFile()==pFile)
		{
			pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
			pMenu->AppendMenu(MF_STRING,ID_UPLOAD_ITEM,"保存为当前版本");
			pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"另存为新版本");
		}
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除文件");
	}
	else if(pItemInfo->itemType==SERVER_TOWERPARTSET)
	{
		pMenu->AppendMenu(MF_STRING,ID_DOWNLOAD_ITEM,"提取构件集");
	}
	else if(pItemInfo->itemType==SERVER_TOWERPARTSET_SYNC)
	{
		pMenu->AppendMenu(MF_STRING,ID_DOWNLOAD_ITEM,"提取待同步构件集");
	}
	else if(pItemInfo->itemType==SERVER_TOWERPART_SEGMENT)
	{
		HTREEITEM hPartItem=pTreeCtrl->GetChildItem(hItem);
		HTREEITEM hParentItem=pTreeCtrl->GetParentItem(hItem);
		TREEITEM_INFO *pParentItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
		if(hPartItem!=NULL&&pParentItemInfo!=NULL)
		{
			pMenu->AppendMenu(MF_STRING,ID_UPLOAD_ITEM,"上传至服务器");
			if(pParentItemInfo->itemType==SERVER_PARTSET_JUSTIFY)
				pMenu->AppendMenu(MF_STRING,ID_DOWNLOAD_ITEM,"获取最新版本");
		}
	}
	else if(pItemInfo->itemType==SERVER_PARTSET_JUSTIFY)
	{
		pMenu->AppendMenu(MF_STRING,ID_UPLOAD_ITEM,"上传至服务器");
		pMenu->AppendMenu(MF_STRING,ID_DOWNLOAD_ITEM,"获取最新版本");
	}
	else if(pItemInfo->itemType==SERVER_PARTSET_UPLOAD)
		pMenu->AppendMenu(MF_STRING,ID_UPLOAD_ITEM,"上传至服务器");
	else if(pItemInfo->itemType==SERVER_PARTSET_DOWNLOAD)
		pMenu->AppendMenu(MF_STRING,ID_DOWNLOAD_ITEM,"获取最新版本");
	else if(pItemInfo->itemType==SERVER_TOWERPART_SYNCING)
	{
		CServerTowerPart *pServerPart=(CServerTowerPart*)pItemInfo->dwRefData;
		if(pServerPart!=NULL)
		{
			if(pServerPart->m_cbSyncState==CServerTowerPart::SYNC_UPLOAD||pServerPart->m_cbSyncState==CServerTowerPart::SYNC_JUSTIFY)
				pMenu->AppendMenu(MF_STRING,ID_UPLOAD_ITEM,"上传至服务器");
			if(pServerPart->m_cbSyncState==CServerTowerPart::SYNC_DOWNLOAD||pServerPart->m_cbSyncState==CServerTowerPart::SYNC_JUSTIFY)
				pMenu->AppendMenu(MF_STRING,ID_DOWNLOAD_ITEM,"获取最新版本");
		}
	}
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}

void CRemoteServerDlg::OnNMRClickTreeCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	TVHITTESTINFO HitTestInfo;
	GetCursorPos(&HitTestInfo.pt);
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	pTreeCtrl->ScreenToClient(&HitTestInfo.pt);
	pTreeCtrl->HitTest(&HitTestInfo);
	pTreeCtrl->Select(HitTestInfo.hItem,TVGN_CARET);

	ContextMenu(this,HitTestInfo.pt);
	*pResult = 0;
}
//下载
//defined in LDSView.cpp
void ReadProcessPartsFromAttachBuffer(CBuffer* pAttachBuff,CSuperHashStrList<CProcessPart> &hashPartByPartNo);
void CRemoteServerDlg::OnConnectServer()
{
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=hItem!=NULL?(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem):NULL;
	if(pItemInfo==NULL||TMS.m_pServer!=NULL||pItemInfo->itemType!=SERVER_CONTENT)
		return;
	CWaitCursor waitCursor;
	theApp.m_bNetModelWork=theApp.LogonTMSServer();
	if(theApp.m_bNetModelWork)
	{
		//塔型组分类节点
		pTreeCtrl->SetItemImage(hItem,IMG_MACHINE_SERVER,IMG_MACHINE_SERVER);
		HTREEITEM hTowerTypeGroupItem=NULL;
		hTowerTypeGroupItem=pTreeCtrl->InsertItem("放样塔型",IMG_TOWER_TYPE,IMG_TOWER_TYPE,m_hServerContentItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERTYPESET1,NULL));
		pTreeCtrl->SetItemData(hTowerTypeGroupItem,(DWORD)pItemInfo);
		//加载放样塔型
		pTreeCtrl->SelectItem(hTowerTypeGroupItem);
		OnDownloadItem();
		//
		hTowerTypeGroupItem=pTreeCtrl->InsertItem("试组塔型",IMG_TOWER_TYPE,IMG_TOWER_TYPE,m_hServerContentItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERTYPESET2,NULL));
		pTreeCtrl->SetItemData(hTowerTypeGroupItem,(DWORD)pItemInfo);
		hTowerTypeGroupItem=pTreeCtrl->InsertItem("存档塔型",IMG_TOWER_TYPE,IMG_TOWER_TYPE,m_hServerContentItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERTYPESET3,NULL));
		pTreeCtrl->SetItemData(hTowerTypeGroupItem,(DWORD)pItemInfo);
		pTreeCtrl->Expand(m_hServerContentItem,TVE_EXPAND);
		//工件集合
		m_hPartSetItem=pTreeCtrl->InsertItem("构件集",IMG_TOWERPARTSET,IMG_TOWERPARTSET,TVI_ROOT);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPARTSET,NULL));
		pTreeCtrl->SetItemData(m_hPartSetItem,(DWORD)pItemInfo);
		//待同步工件集合
		m_hSyncPartSetItem=pTreeCtrl->InsertItem("待同步",IMG_TOWERPARTSET_SYNC,IMG_TOWERPARTSET_SYNC,TVI_ROOT);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPARTSET_SYNC,NULL));
		pTreeCtrl->SetItemData(m_hSyncPartSetItem,(DWORD)pItemInfo);
		//测试代码
		CProcessPart* pProcessPart;
		CSuperHashStrList<CProcessPart> hashPartsByPartNo;
		CAttachBuffer *pPpeModelBuffer=Ta.AttachBuffer.GetValue(CTower::PPE_MODEL);
		ReadProcessPartsFromAttachBuffer(pPpeModelBuffer,hashPartsByPartNo);
		CHashList<SEGI> hashSegIs;
		for(pProcessPart=hashPartsByPartNo.GetFirst();pProcessPart;pProcessPart=hashPartsByPartNo.GetNext())
			hashSegIs.SetValue(pProcessPart->iSeg.iSeg,pProcessPart->iSeg);
		for(SEGI* pSegI=hashSegIs.GetFirst();pSegI;pSegI=hashSegIs.GetNext())
		{
			HTREEITEM hSegmentItem=pTreeCtrl->InsertItem(CXhChar16("%s段",(char*)pSegI->ToString()),IMG_TOWERPART_SEGMENT,IMG_TOWERPART_SEGMENT,m_hPartSetItem);
			pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPART_SEGMENT,pSegI->iSeg));
			pTreeCtrl->SetItemData(hSegmentItem,(DWORD)pItemInfo);
			for(pProcessPart=hashPartsByPartNo.GetFirst();pProcessPart;pProcessPart=hashPartsByPartNo.GetNext())
			{
				if(pProcessPart->iSeg.iSeg!=pSegI->iSeg)
					continue;
				int iItemImage=pProcessPart->m_cPartType==CProcessPart::TYPE_LINEANGLE?IMG_ANGLEPART:IMG_PLATEPART;
				HTREEITEM hPartItem=pTreeCtrl->InsertItem(pProcessPart->Label,iItemImage,iItemImage,hSegmentItem);
				pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPART,NULL));
				pTreeCtrl->SetItemData(hSegmentItem,(DWORD)pItemInfo);
				//hPartItem=pTreeCtrl->InsertItem("102",IMG_TOWERPART_SYNCING,IMG_TOWERPART_SYNCING,hSegmentItem);
				//pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPART,NULL));
				//pTreeCtrl->SetItemData(hSegmentItem,(DWORD)pItemInfo);
				//hPartItem=pTreeCtrl->InsertItem("103",IMG_TOWERPART_DYNAMIC,IMG_TOWERPART_DYNAMIC,hSegmentItem);
				//pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERPART,NULL));
				//pTreeCtrl->SetItemData(hSegmentItem,(DWORD)pItemInfo);
				pTreeCtrl->Expand(m_hPartSetItem,TVE_EXPAND);
			}
		}
	}
}
void CRemoteServerDlg::OnDisconnectServer()
{
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=hItem!=NULL?(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem):NULL;
	if(pItemInfo==NULL||TMS.m_pServer==NULL||pItemInfo->itemType!=SERVER_CONTENT)
		return;
	TMS.m_pServer=NULL;
	//theApp.LogoutTMSServer();
	theApp.m_bNetModelWork=false;
	CXhSimpleList<HTREEITEM> listTowerTypeItems;
	HTREEITEM hTowerTypeItem=pTreeCtrl->GetNextItem(m_hServerContentItem,TVGN_CHILD);
	do{
		if(hTowerTypeItem==NULL)
			break;
		listTowerTypeItems.AttachObject(hTowerTypeItem);
		hTowerTypeItem=pTreeCtrl->GetNextSiblingItem(hTowerTypeItem);
	}while(hTowerTypeItem!=NULL);
	for(HTREEITEM* pRemoveItem=listTowerTypeItems.EnumObjectFirst();pRemoveItem;pRemoveItem=listTowerTypeItems.EnumObjectNext())
		pTreeCtrl->DeleteItem(*pRemoveItem);
	if(m_hPartSetItem)
		pTreeCtrl->DeleteItem(m_hPartSetItem);
	if(m_hSyncPartSetItem)
		pTreeCtrl->DeleteItem(m_hSyncPartSetItem);
	AgentServer.hashTowerTypes.Empty();
	AgentServer.m_pActiveTowerFile=NULL;
}

static BOOL VisitTowerFileItem(CTowerTreeCtrl *pTreeCtrl,HTREEITEM hItem)
{	//设置所有塔型文件节点为普通格式
	TREEITEM_INFO *pItem=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	CString sText=pTreeCtrl->GetItemText(hItem);
	if(pItem&&pItem->itemType==CRemoteServerDlg::SERVER_TOWERFILE)
		pTreeCtrl->SetItemState(hItem,0,TVIS_BOLD);
	return FALSE;
}
bool UploadTowerFile(CServerTowerFile* pTowerFile,bool overwrite=true)
{
	int result=TMS.SaveOrUpdateObject("TowerFile",pTowerFile);
	bool upload_ok=false;
	if(result>0)
	{
		CXhChar200 tempfilename("%sremotetempory.upload",theApp.szWorkPath);
		CFile file(tempfilename,CFile::modeCreate|CFile::modeReadWrite);
		CArchive saveArchive(&file, CArchive::store | CArchive::bNoFlushOnDelete);
		theApp.GetLDSDoc()->Serialize(saveArchive);
		saveArchive.Flush();
		saveArchive.Close();	//必须在此关闭CArchive否则文件缓存中部分内容未刷存到物理文件中，后期返回的长度等都有问题。
		UINT uiFileSize=(UINT)file.GetLength();
		UINT uiLeastSize=uiFileSize;
		int indexpos=0;
		file.SeekToBegin();
		CHAR_ARRAY pool(0x500000);
		CBuffer buff(pool,0x500000);	//每次上传最大不超过5M的文件，大文件分片上传T
		while(uiLeastSize>0)
		{
			buff.ClearContents();
			UINT uiReadSize=min(uiLeastSize,0x500000);
			buff.Write(NULL,uiReadSize);
			file.Read(pool,uiReadSize);
			if(!TMS.UploadFileObject(pTowerFile->m_idSrvFileObj,indexpos,&buff,false))
				break;	//上传失败
			indexpos+=uiReadSize;
			uiLeastSize-=uiReadSize;
		}
		if(uiLeastSize==0)
			upload_ok=TMS.FinalizeFileObjectData(pTowerFile->m_idSrvFileObj,uiFileSize,overwrite);
		file.Close();
		::DeleteFile(tempfilename);
	}
	return upload_ok;
}
bool DownloadTowerFile(CServerTowerFile* pTowerFile,BUFFER_IO* pDestBuff=NULL)
{
	UINT uiFileDataLength=0;
	if( pTowerFile->m_idSrvFileObj<=0||	
		//创建一个远程文件内容提供者对象，返回压缩前或后的文件数据流字节总长度
		((uiFileDataLength=TMS.OpenFileObjectDataProvider(pTowerFile->m_idSrvFileObj, false))==0))
	{
		AfxMessageBox("缺少服务器端的指向数据文件！");
		return false;
	}
	int indexpos=0;
	UINT uiLeastSize=uiFileDataLength;
	CHAR_ARRAY pool(0x500000);
	CBuffer buff(pool,0x500000);	//每次上传最大不超过5M的文件，大文件分片上传T
	CBuffer fileBuff(0x100000);
	BUFFER_IO* pToBuff=pDestBuff!=NULL?pDestBuff:&fileBuff;
	while(uiLeastSize>0)
	{
		buff.ClearContents();
		UINT uiDownloadSize=min(uiLeastSize,0x500000);
		if(TMS.DownloadFileObject(pTowerFile->m_idSrvFileObj,indexpos,uiDownloadSize,false,&buff)<uiDownloadSize)
			break;	//下载失败
		pToBuff->Write(buff.GetBufferPtr(),uiDownloadSize);
		indexpos+=uiDownloadSize;
		uiLeastSize-=uiDownloadSize;
	}
	TMS.CloseFileObjectDataProvider(pTowerFile->m_idSrvFileObj);
	if(uiLeastSize==0)
	{
		if(pDestBuff!=NULL)
			return true;	//加载到指定缓存以便另存为物理文件
		//作为当前文件加载到系统内存
		CXhChar200 tempfilename("%sremotetempory.download",theApp.szWorkPath);
		CFile file(tempfilename,CFile::modeCreate|CFile::modeReadWrite);
		file.Write(fileBuff.GetBufferPtr(),fileBuff.GetLength());
		file.SeekToBegin();
		CArchive loadArchive(&file, CArchive::load | CArchive::bNoFlushOnDelete);
		theApp.GetLDSDoc()->Serialize(loadArchive);
		loadArchive.Close();	//必须在此关闭CArchive否则文件缓存中部分内容未刷存到物理文件中，后期返回的长度等都有问题。
		file.Close();
		return true;
	}
	else
		AfxMessageBox("数据文件下载失败!");
	return false;
}
#include "list.h"
BOOL UpdateProcessPartsToAttachBuffer(CHashStrList<CProcessPart*> &hashPartPtrByLabel);
BOOL UpdateModelProcessPart(CServerTowerPart *pServPart)
{
	if(pServPart==NULL)
		return FALSE;
	CHashStrList<CProcessPart*> hashPartPtrByLabel;
	hashPartPtrByLabel.SetValue(pServPart->ppi.Label,&pServPart->ppi);
	return UpdateProcessPartsToAttachBuffer(hashPartPtrByLabel);
}
BOOL UpdateModelProcessPart(IXhEnumerator *pIter)
{
	if(pIter==NULL)
		return FALSE;
	CHashStrList<CProcessPart*> hashPartPtrByLabel; 
	for(void* pObj=pIter->MoveFirst();pObj;pObj=pIter->MoveNext())
	{
		CServerTowerPart *pServerPart=(CServerTowerPart*)pObj;
		if(pServerPart!=NULL)
			hashPartPtrByLabel.SetValue(pServerPart->ppi.Label,&pServerPart->ppi);
	}
	return UpdateProcessPartsToAttachBuffer(hashPartPtrByLabel);
}
void CRemoteServerDlg::OnDownloadItem()
{
	CTowerTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=hItem!=NULL?(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem):NULL;
	if(pItemInfo==NULL||TMS.m_pServer==NULL)
		return;
	CWaitCursor waitCursor;
	if(pItemInfo->itemType==SERVER_TOWERTYPESET1)
	{	//加载在放样塔型列表
		CBuffer buffer;
		if(!TMS.QueryStateTowerTypes(&buffer,1,0,0,CServerTowerType::SERIALIZE_TMA))
		{
			AfxMessageBox("数据加载失败！");
			return;
		}
		buffer.SeekToBegin();
		AgentServer.ParseAndUpdateTowerTypesFromBuffer(buffer,1,CServerTowerType::SERIALIZE_TMA);
		//刷新树节点
		RefreshServerContent(hItem);
	}
	else if(pItemInfo->itemType==SERVER_TOWERTYPESET2)
	{	//加载试组塔型列表
		CBuffer buffer;
		if(!TMS.QueryStateTowerTypes(&buffer,2,0,0,CServerTowerType::SERIALIZE_TMA))
		{
			AfxMessageBox("数据加载失败！");
			return;
		}
		buffer.SeekToBegin();
		AgentServer.ParseAndUpdateTowerTypesFromBuffer(buffer,2,CServerTowerType::SERIALIZE_TMA);
		//刷新树节点
		RefreshServerContent(hItem);
	}
	else if(pItemInfo->itemType==SERVER_TOWERTYPESET3)
	{	//加载已存档塔型列表
		CBuffer buffer;
		if(!TMS.QueryStateTowerTypes(&buffer,3,0,0,CServerTowerType::SERIALIZE_TMA))
		{
			AfxMessageBox("数据加载失败！");
			return;
		}
		buffer.SeekToBegin();
		AgentServer.ParseAndUpdateTowerTypesFromBuffer(buffer,3,CServerTowerType::SERIALIZE_TMA);
		//刷新树节点
		RefreshServerContent(hItem);
	}
	else if(pItemInfo->itemType==SERVER_TOWERFILE)
	{
		CServerTowerFile *pTowerFile=(CServerTowerFile*)pItemInfo->dwRefData;
		if(DownloadTowerFile(pTowerFile))
		{
			TMS.InitTowerPartsVersionSerial(AgentServer.m_pActiveTowerType->Id);
			//设置当前项粗体显示
			pTreeCtrl->SetVisitItemFunc(VisitTowerFileItem);	
			pTreeCtrl->TraverseItem(m_hServerContentItem);		//遍历树
			pTreeCtrl->SetItemState(hItem,TVIS_BOLD,TVIS_BOLD);
			//切换激活文件并刷新显示区及树节点
			AgentServer.ActivateTowerFile(pTowerFile);
			theApp.GetLDSDoc()->SetTitle(pTowerFile->m_sFileName);
			theApp.GetLDSDoc()->UpdateAllViews(NULL);
		}
	}
	else if(pItemInfo->itemType==SERVER_TOWERPARTSET)
	{
		CServerTowerFileGroup *pFileGroup=AgentServer.m_pActiveTowerFile!=NULL?AgentServer.m_pActiveTowerFile->m_pBelongFileGroup:NULL;
		if(pFileGroup==NULL)
		{
			AfxMessageBox("请激活文件之后再提取构件！");
			return;
		}
		RetrievedPartSet();
		RefreshProcessPartSet(hItem);
	}
	else if(pItemInfo->itemType==SERVER_TOWERPARTSET_SYNC)
	{
		if(AgentServer.m_pActiveTowerFile==NULL||AgentServer.m_pActiveTowerFile->m_idTowerType<=0)
		{
			AfxMessageBox("当前模型未与服务器端任一塔型进行关联匹配，关联上传塔型后再同步构件!");
			return;
		}
		RetrievedPartSet();
		IXhEnumerator* pIter=hashProcessParts.NewEnumerator();
		CSelfDestoryObject<IXhEnumerator> selfdestroy(pIter,&IXhEnumerator::Destroy,true);
		//根据构件编号获取服务器端构件的Id、关联PPI文件的FileObj.Id及Tid信息中的构件工艺信息版本号
		if(TMS.MatchTowerParts(pIter,AgentServer.m_pActiveTowerFile->m_idTowerType,CServerTowerPart::SERIALIZE_LABELnID))
			RefreshSyncPartSet(hItem);
		else
			AfxMessageBox("同步更新失败!");
	}
	else if(pItemInfo->itemType==SERVER_PARTSET_DOWNLOAD||
			pItemInfo->itemType==SERVER_PARTSET_JUSTIFY)
	{
		CXhSimpleList<CServerTowerPart*> listSelSyncParts;
		HTREEITEM hSegItem=pTreeCtrl->GetChildItem(hItem);
		while(hSegItem!=NULL)
		{
			HTREEITEM hPartItem=pTreeCtrl->GetChildItem(hSegItem);
			while(hPartItem!=NULL)
			{
				TREEITEM_INFO *pPartItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hPartItem);
				if(pPartItemInfo!=NULL&&pPartItemInfo->itemType==SERVER_TOWERPART_SYNCING)
				{
					CServerTowerPart* pTowerPart=(CServerTowerPart*)pPartItemInfo->dwRefData;
					listSelSyncParts.AttachObject(pTowerPart);
				}
				hPartItem = pTreeCtrl->GetNextSiblingItem(hPartItem);
			}
			hSegItem = pTreeCtrl->GetNextSiblingItem(hSegItem);
		}
		CXhInternalTemplEnumerator<CServerTowerPart> enumer;//=CXhInternalTemplEnumerator<CServerTowerPart>*)IXhEnumerator::NewEnumerator();
		for(CServerTowerPartPtr* ppTowerPart=listSelSyncParts.EnumObjectFirst();ppTowerPart;ppTowerPart=listSelSyncParts.EnumObjectNext())
		{
			if(*ppTowerPart)
				enumer.AppendObjPtr(*ppTowerPart);
		}
		if(!UpdateModelProcessPart(&enumer))
			AfxMessageBox("更新模型构件失败，请重试");
		else
		{
			HTREEITEM hSegItem=pTreeCtrl->GetChildItem(hItem);
			while(hSegItem!=NULL)
			{
				HTREEITEM hPartItem=pTreeCtrl->GetChildItem(hSegItem);
				while(hPartItem!=NULL)
				{
					TREEITEM_INFO *pPartItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hPartItem);
					if(pPartItemInfo!=NULL&&pPartItemInfo->itemType==SERVER_TOWERPART_SYNCING)
					{
						CServerTowerPart* pTowerPart=(CServerTowerPart*)pPartItemInfo->dwRefData;
						int iItemImage=pTowerPart->ppi.m_cPartType==CProcessPart::TYPE_LINEANGLE?IMG_ANGLEPART:IMG_PLATEPART;
						pTreeCtrl->SetItemImage(hPartItem,iItemImage,iItemImage);
					}
					hPartItem = pTreeCtrl->GetNextSiblingItem(hPartItem);
				}
				hSegItem = pTreeCtrl->GetNextSiblingItem(hSegItem);
			}
		}
	}
	else if(pItemInfo->itemType==SERVER_TOWERPART_SEGMENT)
	{
		CXhSimpleList<CServerTowerPart*> listSelSyncParts;
		HTREEITEM hPartItem=pTreeCtrl->GetChildItem(hItem);
		while(hPartItem!=NULL)
		{
			TREEITEM_INFO *pPartItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hPartItem);
			if(pPartItemInfo!=NULL&&pPartItemInfo->itemType==SERVER_TOWERPART_SYNCING)
			{
				CServerTowerPart* pTowerPart=(CServerTowerPart*)pPartItemInfo->dwRefData;
				listSelSyncParts.AttachObject(pTowerPart);
			}
			hPartItem = pTreeCtrl->GetNextSiblingItem(hPartItem);
		}
		CXhInternalTemplEnumerator<CServerTowerPart> enumer;//=CXhInternalTemplEnumerator<CServerTowerPart>*)IXhEnumerator::NewEnumerator();
		for(CServerTowerPartPtr* ppTowerPart=listSelSyncParts.EnumObjectFirst();ppTowerPart;ppTowerPart=listSelSyncParts.EnumObjectNext())
		{
			if(*ppTowerPart)
				enumer.AppendObjPtr(*ppTowerPart);
		}
		if(!UpdateModelProcessPart(&enumer))
			AfxMessageBox("更新模型构件失败，请重试");
		else
		{
			HTREEITEM hPartItem=pTreeCtrl->GetChildItem(hItem);
			while(hPartItem!=NULL)
			{
				TREEITEM_INFO *pPartItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hPartItem);
				if(pPartItemInfo!=NULL&&pPartItemInfo->itemType==SERVER_TOWERPART_SYNCING)
				{
					CServerTowerPart* pTowerPart=(CServerTowerPart*)pPartItemInfo->dwRefData;
					int iItemImage=pTowerPart->ppi.m_cPartType==CProcessPart::TYPE_LINEANGLE?IMG_ANGLEPART:IMG_PLATEPART;
					pTreeCtrl->SetItemImage(hPartItem,iItemImage,iItemImage);
				}
				hPartItem = pTreeCtrl->GetNextSiblingItem(hPartItem);
			}
		}
	}
	else if(pItemInfo->itemType==SERVER_TOWERPART_SYNCING)
	{
		CServerTowerPart* pTowerPart=(CServerTowerPart*)pItemInfo->dwRefData;
		if(!UpdateModelProcessPart(pTowerPart))
			AfxMessageBox("更新模型构件失败，请重试");
		else
		{
			int iItemImage=pTowerPart->ppi.m_cPartType==CProcessPart::TYPE_LINEANGLE?IMG_ANGLEPART:IMG_PLATEPART;
			pTreeCtrl->SetItemImage(hItem,iItemImage,iItemImage);
		}
	}
}
//上传
void CRemoteServerDlg::OnUploadItem()
{
	CTowerTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=NULL;
	if(hItem)
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL||TMS.m_pServer==NULL)
		return;
	CWaitCursor waitCursor;
	if(pItemInfo->itemType==SERVER_TOWERFILE)
	{
		CServerTowerFile *pTowerFile=(CServerTowerFile*)pItemInfo->dwRefData;
		if(AgentServer.GetActiveTowerFile()==pTowerFile)
		{
			if(UploadTowerFile(pTowerFile,true))
				MessageBox("上传成功！");
			else
				AfxMessageBox("上传数据失败，请重试!");
		}
		else 
			AfxMessageBox("待上传文件不是当前激活塔型文件！");
	}
	else if(pItemInfo->itemType==SERVER_TOWERFILEGROUP)
	{
		CServerTowerFileGroup *pFileGroup=(CServerTowerFileGroup*)pItemInfo->dwRefData;
		CServerTowerFile towerfile;
		towerfile.m_idFileGroup=pFileGroup->m_id;
		CServerTowerFile* pTowerFile=pFileGroup->m_idActiveFile>0?pFileGroup->hashTowerFiles.GetValue(pFileGroup->m_idActiveFile):NULL;
		if(pTowerFile==NULL)
		{
			towerfile.m_idTowerType=pFileGroup->m_idTowerType;
			if(towerfile.m_sFileName.Length==0)
				towerfile.m_sFileName=theApp.GetLDSDoc()->GetTitle();
			pTowerFile=&towerfile;
		}
		if(UploadTowerFile(pTowerFile))
			MessageBox("上传成功！");
		else
			AfxMessageBox("上传数据失败，请重试!");
	}
	else if(pItemInfo->itemType==SERVER_PARTSET_UPLOAD||
			pItemInfo->itemType==SERVER_PARTSET_JUSTIFY)
	{
		CXhSimpleList<CServerTowerPart*> listSelSyncParts;
		HTREEITEM hSegItem=pTreeCtrl->GetChildItem(hItem);
		while(hSegItem!=NULL)
		{
			HTREEITEM hPartItem=pTreeCtrl->GetChildItem(hSegItem);
			while(hPartItem!=NULL)
			{
				TREEITEM_INFO *pPartItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hPartItem);
				if(pPartItemInfo!=NULL&&pPartItemInfo->itemType==SERVER_TOWERPART_SYNCING)
				{
					CServerTowerPart* pTowerPart=(CServerTowerPart*)pPartItemInfo->dwRefData;
					listSelSyncParts.AttachObject(pTowerPart);
				}
				hPartItem = pTreeCtrl->GetNextSiblingItem(hPartItem);
			}
			hSegItem = pTreeCtrl->GetNextSiblingItem(hSegItem);
		}
		CXhInternalTemplEnumerator<CServerTowerPart> enumer;//=CXhInternalTemplEnumerator<CServerTowerPart>*)IXhEnumerator::NewEnumerator();
		for(CServerTowerPartPtr* ppTowerPart=listSelSyncParts.EnumObjectFirst();ppTowerPart;ppTowerPart=listSelSyncParts.EnumObjectNext())
		{
			if(*ppTowerPart)
				enumer.AppendObjPtr(*ppTowerPart);
		}
		if(!TMS.SaveOrUpdateObjects("TowerPart",&enumer,CServerTowerPart::SERIALIZE_TMA))
			AfxMessageBox("同步上传工艺数据失败，请重试");
		else
		{
			HTREEITEM hSegItem=pTreeCtrl->GetChildItem(hItem);
			while(hSegItem!=NULL)
			{
				HTREEITEM hPartItem=pTreeCtrl->GetChildItem(hSegItem);
				while(hPartItem!=NULL)
				{
					TREEITEM_INFO *pPartItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hPartItem);
					if(pPartItemInfo!=NULL&&pPartItemInfo->itemType==SERVER_TOWERPART_SYNCING)
					{
						CServerTowerPart* pTowerPart=(CServerTowerPart*)pPartItemInfo->dwRefData;
						int iItemImage=pTowerPart->ppi.m_cPartType==CProcessPart::TYPE_LINEANGLE?IMG_ANGLEPART:IMG_PLATEPART;
						pTreeCtrl->SetItemImage(hPartItem,iItemImage,iItemImage);
					}
					hPartItem = pTreeCtrl->GetNextSiblingItem(hPartItem);
				}
				hSegItem = pTreeCtrl->GetNextSiblingItem(hSegItem);
			}
			//重新初始化版本信息
			TMS.InitTowerPartsVersionSerial(AgentServer.m_pActiveTowerType->Id);
		}
	}
	else if(pItemInfo->itemType==SERVER_TOWERPART_SEGMENT)
	{
		CXhSimpleList<CServerTowerPart*> listSelSyncParts;
		HTREEITEM hPartItem=pTreeCtrl->GetChildItem(hItem);
		while(hPartItem!=NULL)
		{
			TREEITEM_INFO *pPartItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hPartItem);
			if(pPartItemInfo!=NULL&&pPartItemInfo->itemType==SERVER_TOWERPART_SYNCING)
			{
				CServerTowerPart* pTowerPart=(CServerTowerPart*)pPartItemInfo->dwRefData;
				listSelSyncParts.AttachObject(pTowerPart);
			}
			hPartItem = pTreeCtrl->GetNextSiblingItem(hPartItem);
		}
		CXhInternalTemplEnumerator<CServerTowerPart> enumer;//=CXhInternalTemplEnumerator<CServerTowerPart>*)IXhEnumerator::NewEnumerator();
		for(CServerTowerPartPtr* ppTowerPart=listSelSyncParts.EnumObjectFirst();ppTowerPart;ppTowerPart=listSelSyncParts.EnumObjectNext())
		{
			if(*ppTowerPart)
				enumer.AppendObjPtr(*ppTowerPart);
		}
		if(!TMS.SaveOrUpdateObjects("TowerPart",&enumer,CServerTowerPart::SERIALIZE_TMA))
			AfxMessageBox("同步上传工艺数据失败，请重试");
		else
		{
			HTREEITEM hPartItem=pTreeCtrl->GetChildItem(hItem);
			while(hPartItem!=NULL)
			{
				TREEITEM_INFO *pPartItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hPartItem);
				if(pPartItemInfo!=NULL&&pPartItemInfo->itemType==SERVER_TOWERPART_SYNCING)
				{
					CServerTowerPart* pTowerPart=(CServerTowerPart*)pPartItemInfo->dwRefData;
					int iItemImage=pTowerPart->ppi.m_cPartType==CProcessPart::TYPE_LINEANGLE?IMG_ANGLEPART:IMG_PLATEPART;
					pTreeCtrl->SetItemImage(hPartItem,iItemImage,iItemImage);
				}
				hPartItem = pTreeCtrl->GetNextSiblingItem(hPartItem);
			}
			//重新初始化版本信息
			TMS.InitTowerPartsVersionSerial(AgentServer.m_pActiveTowerType->Id);
		}
	}
	else if(pItemInfo->itemType==SERVER_TOWERPART_SYNCING)
	{
		CServerTowerPart* pTowerPart=(CServerTowerPart*)pItemInfo->dwRefData;
		if(!TMS.SaveOrUpdateObject("TowerPart",pTowerPart,CServerTowerPart::SERIALIZE_TMA))
			AfxMessageBox("同步上传工艺数据失败，请重试");
		else
		{
			int iItemImage=pTowerPart->ppi.m_cPartType==CProcessPart::TYPE_LINEANGLE?IMG_ANGLEPART:IMG_PLATEPART;
			pTreeCtrl->SetItemImage(hItem,iItemImage,iItemImage);
			//重新初始化版本信息
			TMS.InitTowerPartsVersionSerial(AgentServer.m_pActiveTowerType->Id);
		}
	}
	else if( pItemInfo->itemType==SERVER_TOWERPART)
	{
		CServerTowerPart *pTowerPart=(CServerTowerPart*)pItemInfo->dwRefData;
		if(!TMS.SaveOrUpdateObject("TowerPart",pTowerPart,CServerTowerPart::SERIALIZE_TMA))
			AfxMessageBox("上传数据失败，请重试！");
		else 
		{
			TMS.InitTowerPartsVersionSerial(AgentServer.m_pActiveTowerType->Id);
			AfxMessageBox("上传成功！");
		}
	}
	/*else if (pItemInfo->itemType==SERVER_TOWERFILEASSEMBLY1)
	{
		CServerTowerAssembly *pTowerAssembly=(CServerTowerAssembly*)pItemInfo->dwRefData;
		if(!TMS.SaveOrUpdateObject("TowerTypeFileGroup",pTowerAssembly))
			AfxMessageBox("上传数据失败，请重试！");
		else 
			AfxMessageBox("上传成功！");
	}*/
}
//保存或另存
void CRemoteServerDlg::OnSaveAsTowerFile()
{
	CXhChar200 sFileName("新建");
	CServerTowerType *pTowerType=NULL;
	HTREEITEM hTowerTypeItem=NULL,hFileGroupItem=NULL;
	//根据输入的hSrcItem判断操作类型
	CTowerTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hSrcItem=m_treeCtrl.GetSelectedItem();
	if(hSrcItem==NULL)
		return;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hSrcItem);
	if(pItemInfo->itemType==SERVER_TOWERFILEGROUP||pItemInfo->itemType==SERVER_TOWERFILE)
	{	//选中塔型文件，右键另存为新文件
		CServerTowerFile *pTowerFile=NULL;
		CServerTowerFileGroup *pFileGroup=NULL;
		if(pItemInfo->itemType==SERVER_TOWERFILEGROUP)
		{
			if((pFileGroup=(CServerTowerFileGroup*)pItemInfo->dwRefData)!=NULL)
				pTowerFile=pFileGroup->hashTowerFiles.GetValue(pFileGroup->m_idActiveFile);
		}
		else
			pTowerFile=(CServerTowerFile*)pItemInfo->dwRefData;
		if(pTowerFile==NULL||pTowerFile->m_idSrvFileObj<=0)
			return;
		CBuffer fileBuff(0x100000);
		if(!DownloadTowerFile(pTowerFile,&fileBuff))
		{
			AfxMessageBox("文件下载失败！");
			return;
		}
		CXhChar16 ext;
		_splitpath(pTowerFile->m_sFileName,NULL,NULL,NULL,ext);
		if(ext[0]=='.')
			memmove(ext,(char*)ext+1,ext.Length);
		CXhChar100 szFilter;
#ifdef AFX_TARG_ENU_ENGLISH
		if(ext.EqualNoCase("tma"))
			szFilter="Tower Manufacturing Assistant(*.tma)|*.tma|all files(*.*)|*.*||";
		else
			szFilter="all files(*.*)|*.*||";
#else
		if(ext.EqualNoCase("tma"))
			szFilter="铁塔制造助手(*.tma)|*.tma|所有文件(*.*)|*.*||";
		else if(ext.EqualNoCase("lma"))
			szFilter="格构铁塔制造助手(*.lma)|*.lma|所有文件(*.*)|*.*||";
		else if(ext.EqualNoCase("tid"))
			szFilter="铁塔模型信息展示文件(*.tid)|*.tid|所有文件(*.*)|*.*||";
		else if(ext.EqualNoCase("ppi"))
			szFilter="构件工艺信息文件(*.ppi)|*.ppi|所有文件(*.*)|*.*||";
		else if(ext.EqualNoCase("ahi"))
			szFilter="塔例组装信息文件(*.ahi)|*.ahi|所有文件(*.*)|*.*||";
		else
			szFilter="所有文件(*.*)|*.*||";
#endif
		CFileDialog savefile(FALSE,ext,pTowerFile->m_sFileName,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
		if(savefile.DoModal()!=IDOK)
			return;
		CXhChar200 tempfilename("%sremotetempory.download",theApp.szWorkPath);
		CFile file(savefile.GetPathName(),CFile::modeCreate|CFile::modeReadWrite);
		file.Write(fileBuff.GetBufferPtr(),fileBuff.GetLength());
		file.Close();
	}
}

CServerTowerFile* CRemoteServerDlg::SaveOrUpdateTowerFile(HTREEITEM hItem,TREEITEM_INFO *pItemInfo,
														  const char* sOriginalFileName,
														  const char *sFileNotes,BOOL bDisplayDlg)
{
	CTowerTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hFileGroupItem=hItem;
	CServerTowerFile *pFile=NULL;
	CServerTowerFileGroup *pFileGroup=(CServerTowerFileGroup*)pItemInfo->dwRefData;
	if(pItemInfo->itemType==SERVER_TOWERFILE)
	{
		hFileGroupItem=pTreeCtrl->GetParentItem(hItem);
		pFile=(CServerTowerFile*)pItemInfo->dwRefData;
		pFileGroup=pFile?pFile->m_pBelongFileGroup:NULL;
	}
	HTREEITEM hTowerTypeItem=pTreeCtrl->GetParentItem(hFileGroupItem);
	TREEITEM_INFO *pTowerTypeItemInfo=hTowerTypeItem!=NULL?(TREEITEM_INFO*)pTreeCtrl->GetItemData(hTowerTypeItem):NULL;
	CServerTowerType *pTowerType=pTowerTypeItemInfo!=NULL?(CServerTowerType*)pTowerTypeItemInfo->dwRefData:NULL;
	CServerTowerFile towerfile;
	if(pFile)
	{
		CBuffer buffer;
		pFile->ToBuffer(buffer,false);
		buffer.SeekToBegin();
		towerfile.FromBuffer(buffer,false);
	}
	towerfile.m_idFileGroup=pFileGroup->m_id;
	towerfile.m_idTowerType=pFileGroup->m_idTowerType;
	towerfile.m_sFileName=theApp.GetLDSDoc()->GetTitle();
	CServerTowerFile* pTowerFile=&towerfile;
	DWORD maxVersion=0;
	for(CServerTowerFile *pFile=pFileGroup->hashTowerFiles.GetFirst();pFile;pFile=pFileGroup->hashTowerFiles.GetNext())
		maxVersion = max(pFile->dwVersion,maxVersion);
	pTowerFile->dwVersion=maxVersion+1;
	if(bDisplayDlg)
	{
		CFileGroupDlg fileGroupDlg;
		fileGroupDlg.m_pFileGroup=pFileGroup;
		fileGroupDlg.m_pCurFile=&towerfile;
		if(pTowerType)
			fileGroupDlg.m_sTowerTypeName=pTowerType->m_sName;
		fileGroupDlg.m_ciObjectType=CFileGroupDlg::OBJ_NEW_TOWERFILE;
		if(fileGroupDlg.DoModal()!=IDOK)
			return NULL;
		towerfile.m_sFileName=fileGroupDlg.m_sOriginalFileName;
		towerfile.m_sDescription=fileGroupDlg.m_sCurFileNotes;	
	}
	else
	{
		if(sOriginalFileName!=NULL)
			towerfile.m_sFileName.Copy(sOriginalFileName);
		if(sFileNotes!=NULL)
			towerfile.m_sDescription.Copy(sFileNotes);
	}
	if(UploadTowerFile(pTowerFile,false))
	{
		CBuffer buffer;
		pTowerFile->ToBuffer(buffer);
		buffer.SeekToBegin();
		//
		CServerTowerFile *pNewFile=pFileGroup->hashTowerFiles.Add(pTowerFile->Id);
		pNewFile->FromBuffer(buffer);
		pNewFile->m_pBelongFileGroup=pFileGroup;
		//插入树节点
		CXhChar200 filename("V%d",pNewFile->dwVersion);
		filename.Append(pNewFile->m_sFileName,'#');
		HTREEITEM hFileItem=pTreeCtrl->InsertItem(filename,IMG_TOWER_FILE,IMG_TOWER_FILE,hFileGroupItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERFILE,(DWORD)pNewFile));
		pTreeCtrl->SetItemData(hFileItem,(DWORD)pItemInfo);
		pTreeCtrl->SelectItem(hFileItem);
		OnSetActiveItem();
		return pNewFile;
	}
	else
		return NULL;
}

//新建
void CRemoteServerDlg::OnNewItem() 
{
	CTowerTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	else if(pItemInfo->itemType==SERVER_TOWERTYPESET1)
	{	//新建塔型
		CFileGroupDlg fileGroupDlg;
		fileGroupDlg.m_ciObjectType=CFileGroupDlg::EDIT_TOWERTYPE;
		fileGroupDlg.m_sName="默认分段模型";
		if(fileGroupDlg.DoModal()!=IDOK)
			return;
		CServerTowerType towertype,*pTowerType=NULL;
		towertype.m_sName.Copy(fileGroupDlg.m_sTowerTypeName);
		towertype.state=1;	//1表示在放样塔型
		//pTowerFile->m_fileMatter.ClearBuffer();
		//theApp.GetLDSDoc()->SerializeTowerFileBuffer(pTowerFile->m_fileMatter,TRUE);
		int id=0;
		if((id=TMS.SaveOrUpdateObject("TowerType",&towertype))==0)
		{
			AfxMessageBox("服务器端塔型创建失败!");
			return;
		}
		pTowerType=AgentServer.hashTowerTypes.Add(towertype.m_id);
		pTowerType->m_sName.Copy(towertype.m_sName);
		//2.插入树节点
		HTREEITEM hTowerTypeItem=pTreeCtrl->InsertItem(fileGroupDlg.m_sTowerTypeName,IMG_TOWER_TYPE,IMG_TOWER_TYPE,hItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERTYPE,(DWORD)pTowerType));
		pTreeCtrl->SetItemData(hTowerTypeItem,(DWORD)pItemInfo);
		//创建塔型分段文件
		CServerTowerFileGroup *pFileGroup=towertype.m_towerFileGroupList.append();
		pFileGroup->m_sName.Copy(fileGroupDlg.m_sName);
		pFileGroup->m_sSegStr.Copy(fileGroupDlg.m_sSegStr);
		pFileGroup->m_idTowerType=towertype.Id;
		if((id=TMS.SaveOrUpdateObject("TowerTypeFileGroup",pFileGroup))==0)
		{
			AfxMessageBox("塔型分段文件创建失败!");
			return;
		}
		pFileGroup->SetKey(id);
		//2.插入树节点
		HTREEITEM hFileGroupItem=pTreeCtrl->InsertItem(pFileGroup->m_sName,IMG_FILE_GROUP,IMG_FILE_GROUP,hTowerTypeItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERFILEGROUP,(DWORD)pFileGroup));
		pTreeCtrl->SetItemData(hFileGroupItem,(DWORD)pItemInfo);
		//3.选中当前文件组
		pTreeCtrl->SelectItem(hFileGroupItem);
	}
	else if(pItemInfo->itemType==SERVER_TOWERTYPE)
	{	//新建塔型或文件组
		CServerTowerType *pTowerType=(CServerTowerType*)pItemInfo->dwRefData;
		CFileGroupDlg fileGroupDlg;
		fileGroupDlg.m_ciObjectType=CFileGroupDlg::EDIT_FILEGROUP;
		fileGroupDlg.m_sTowerTypeName=pTowerType->m_sName;
		if(fileGroupDlg.DoModal()==IDOK)
		{	//1.新建文件组
			CServerTowerFileGroup *pFileGroup=pTowerType->m_towerFileGroupList.append();
			pFileGroup->m_sName.Copy(fileGroupDlg.m_sName);
			pFileGroup->m_sSegStr.Copy(fileGroupDlg.m_sSegStr);
			pFileGroup->m_idTowerType=pTowerType->Id;
			if(TMS.SaveOrUpdateObject("TowerTypeFileGroup",pFileGroup,CServerTowerFileGroup::SERIALIZE_BASIC))
			{	//2.插入树节点
				HTREEITEM hFileGroupItem=pTreeCtrl->InsertItem(pFileGroup->m_sName,IMG_TOWER_FILE,IMG_TOWER_FILE,hItem);
				pItemInfo=itemInfoList.append(TREEITEM_INFO(SERVER_TOWERFILEGROUP,(DWORD)pFileGroup));
				pTreeCtrl->SetItemData(hFileGroupItem,(DWORD)pItemInfo);
				//3.选中当前文件组
				pTreeCtrl->SelectItem(hFileGroupItem);
			}
			else 
				AfxMessageBox("上传数据失败，请重试！");
		}
	}
	else if(pItemInfo->itemType==SERVER_TOWERFILEGROUP||pItemInfo->itemType==SERVER_TOWERFILE)
	{	//覆盖当前模型分段文件组的当前版本数据文件
		CServerTowerFile *pNewFile=SaveOrUpdateTowerFile(hItem,pItemInfo,NULL,NULL,TRUE);
		if(pNewFile==NULL)
			AfxMessageBox("上传失败!");
		else
		{	//设置新加版本为当前激活文件
			CServerTowerFileGroup *pFileGroup=pNewFile->m_pBelongFileGroup;
			if(pFileGroup!=NULL&&pFileGroup->Id>0)
			{
				pFileGroup->m_idActiveFile=pNewFile->Id;
				if(!TMS.SaveOrUpdateObject("TowerTypeFileGroup",pFileGroup,CServerTowerFileGroup::SERIALIZE_BASIC))
					AfxMessageBox("分段文件基本信息上传失败，请重试！");
			}
		}
	}
	else if (pItemInfo->itemType==SERVER_TOWERPART)
	{

	}
	else if (pItemInfo->itemType==SERVER_TOWERFILEASSEMBLY)
	{

	}
}


void CRemoteServerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(m_treeCtrl.GetSafeHwnd()==NULL)
		return;
	m_treeCtrl.MoveWindow(0,0,cx,cy);
}
//删除
void CRemoteServerDlg::OnDeleteItem()
{
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();		//目前在工况组一级
	if(hItem)
		DeleteItem(hItem);
}
void CRemoteServerDlg::DeleteItem(HTREEITEM hItem)
{
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
#ifdef AFX_TARG_ENU_ENGLISH
	if(AfxMessageBox("Are you sure to delete selected item？",MB_YESNO)==IDNO)
#else 
	if(AfxMessageBox("确定要删除选中项吗？",MB_YESNO)==IDNO)
#endif
		return;
	if(pItemInfo->itemType==SERVER_TOWERFILEGROUP)
	{
		CServerTowerFileGroup *pFileGroup=(CServerTowerFileGroup*)pItemInfo->dwRefData;
		if(pFileGroup->Id>0)
		{
			if(!TMS.DeleteObject("TowerTypeFileGroup",pFileGroup->Id))
				AfxMessageBox("删除塔型分段文件失败，请重试！");
			else 
				m_treeCtrl.DeleteItem(hItem);
		}
		else 
			m_treeCtrl.DeleteItem(hItem);
	}
	else if(pItemInfo->itemType==SERVER_TOWERFILE)
	{
		CServerTowerFile *pTowerFile=(CServerTowerFile*)pItemInfo->dwRefData;
		if(pTowerFile->Id>0)
		{
			if(!TMS.DeleteObject("TowerFile",pTowerFile->Id))
				AfxMessageBox("删除塔型文件失败，请重试！");
			else 
			{
				if(pTowerFile->m_pBelongFileGroup)
					pTowerFile->m_pBelongFileGroup->hashTowerFiles.DeleteNode(pTowerFile->Id);
				m_treeCtrl.DeleteItem(hItem);
			}
		}
		else 
			m_treeCtrl.DeleteItem(hItem);
	}
	else if(pItemInfo->itemType==SERVER_TOWERTYPE)
	{
		CServerTowerType *pTowerType=(CServerTowerType*)pItemInfo->dwRefData;
		if(pTowerType->Id>0)
		{
			if(!TMS.DeleteObject("TowerType",pTowerType->Id))
				AfxMessageBox("删除塔型失败，请重试！");
			else 
				m_treeCtrl.DeleteItem(hItem);
		}
		else 
			m_treeCtrl.DeleteItem(hItem);
	}
}


//编辑
void CRemoteServerDlg::OnEditItemPropItem()
{
 	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=NULL,*pSubItemInfo=NULL;
	if(hItem)
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	else if(pItemInfo->itemType==SERVER_TOWERTYPE)
	{
		CServerTowerType *pTowerType=(CServerTowerType*)pItemInfo->dwRefData;
		CFileGroupDlg fileGroupDlg;
		fileGroupDlg.m_pFileGroup=NULL;
		fileGroupDlg.m_sTowerTypeName=pTowerType->m_sName;
		fileGroupDlg.m_sSegStr=pTowerType->m_sProjName;
		fileGroupDlg.m_ciObjectType=1;
		if(fileGroupDlg.DoModal()==IDOK)
		{
			pTowerType->m_sName.Copy(fileGroupDlg.m_sTowerTypeName);
			pTowerType->m_sProjName.Copy(fileGroupDlg.m_sSegStr);
			if(!TMS.SaveOrUpdateObject("TowerType",pTowerType))
				AfxMessageBox("塔型基本信息上传失败!");
			else
				pTreeCtrl->SetItemText(hItem,fileGroupDlg.m_sTowerTypeName);
		}
	}
	else if(pItemInfo->itemType==SERVER_TOWERFILEGROUP)
	{
		HTREEITEM hTowerTypeItem=pTreeCtrl->GetParentItem(hItem);
		TREEITEM_INFO *pTowerTypeItemInfo=hTowerTypeItem!=NULL?(TREEITEM_INFO*)pTreeCtrl->GetItemData(hTowerTypeItem):NULL;
		CServerTowerType *pTowerType=pTowerTypeItemInfo!=NULL?(CServerTowerType*)pTowerTypeItemInfo->dwRefData:NULL;
		CServerTowerFileGroup *pFileGroup=(CServerTowerFileGroup*)pItemInfo->dwRefData;
		CFileGroupDlg fileGroupDlg;
		fileGroupDlg.m_pFileGroup=pFileGroup;
		if(pTowerType)
			fileGroupDlg.m_sTowerTypeName=pTowerType->m_sName;
		if(fileGroupDlg.DoModal()==IDOK)
		{
			CServerTowerFile *pFile=fileGroupDlg.GetFileGroupCurFile();
			if(pFile==NULL)
			{
				pFile=SaveOrUpdateTowerFile(hItem,pItemInfo,fileGroupDlg.m_sOriginalFileName,fileGroupDlg.m_sCurFileNotes,FALSE);
				if(pFile==NULL)
				{
					AfxMessageBox("创建新版本失败！");
					return;
				}
			}
			pFileGroup->m_sName.Copy(fileGroupDlg.m_sName);
			pFileGroup->m_sSegStr.Copy(fileGroupDlg.m_sSegStr);
			if(pFile)
			{
				pFileGroup->m_idActiveFile=pFile->Id;
				pFileGroup->m_uiActFileVerSerial=pFile->dwVersion;
			}
			else 
				pFileGroup->m_idActiveFile=0;
			if(!TMS.SaveOrUpdateObject("TowerTypeFileGroup",pFileGroup,CServerTowerFileGroup::SERIALIZE_BASIC))
				AfxMessageBox("分段文件基本信息上传失败，请重试！");
		}
	}
	else if(pItemInfo->itemType==SERVER_TOWERFILE)
	{
		CServerTowerFile *pFile=(CServerTowerFile*)pItemInfo->dwRefData;
		CInputAnStringValDlg dlg;
		dlg.m_sItemTitle="重命名文件";
		dlg.m_sItemValue=pFile->m_sFileName;
		if(dlg.DoModal()==IDOK)
		{
			pFile->m_sFileName.Copy(dlg.m_sItemValue);
			pTreeCtrl->SetItemText(hItem,dlg.m_sItemValue);
		}
	}
}

void CRemoteServerDlg::OnMappingWithCurrentModel() 
{	//把数据库中的文件组记录与当前模型进行映射关联
	CTowerTreeCtrl* pTreeCtrl = GetTreeCtrl();
	HTREEITEM hCurItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=NULL;
	if(hCurItem)
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hCurItem);
	if(pItemInfo==NULL)
		return;
	if(pItemInfo->itemType==SERVER_TOWERFILEGROUP)
	{
		CServerTowerFileGroup *pFileGroup=(CServerTowerFileGroup*)pItemInfo->dwRefData;
		theApp.GetLDSDoc()->m_idServerFile=Ta.m_idServerFile=pFileGroup->m_idActiveFile;
		Ta.m_wiVerSerial=(WORD)pFileGroup->m_uiActFileVerSerial;
		CFileGroupDlg fileGroupDlg;
		fileGroupDlg.m_pFileGroup=pFileGroup;
		fileGroupDlg.m_ciStartMode=1;	//关联映射模式
		if(fileGroupDlg.DoModal()==IDOK)
		{
			pFileGroup->m_sName.Copy(fileGroupDlg.m_sName);
			pFileGroup->m_sSegStr=fileGroupDlg.m_sSegStr;
			if(pFileGroup->pActiveFile)
				pFileGroup->pActiveFile->m_sFileName=fileGroupDlg.m_sOriginalFileName;
			else
			{
				CServerTowerFile tmafile;
				tmafile.dwVersion=1;
				tmafile.m_idFileGroup=pFileGroup->Id;
				tmafile.m_sFileName=fileGroupDlg.m_sOriginalFileName;
				tmafile.m_idTowerType=pFileGroup->m_idTowerType;
				tmafile.m_dataBuff.ClearContents();	//清空后表示不上传文件
				int id=TMS.SaveOrUpdateObject("TowerFile",&tmafile);
				if(id>0)
				{
					CServerTowerFile* pTmaFile=pFileGroup->hashTowerFiles.Add(id);
					pTmaFile->dwVersion=tmafile.dwVersion;
					pTmaFile->m_idFileGroup=pFileGroup->Id;
					pTmaFile->m_sFileName=tmafile.m_sFileName;
					pTmaFile->m_idTowerType=tmafile.m_idTowerType;
					pFileGroup->m_idActiveFile=id;
					//pTmaFile->m_fileMatter
					pTmaFile->m_sFileName=fileGroupDlg.m_sOriginalFileName;
				}
				TMS.SaveOrUpdateObject("TowerTypeFileGroup",pFileGroup);
			}
			pTreeCtrl->SetItemText(hCurItem,pFileGroup->m_sName);
		}
		//设置当前项粗体显示
		pTreeCtrl->SetVisitItemFunc(VisitTowerFileItem);	
		pTreeCtrl->TraverseItem(m_hServerContentItem);		//遍历树
		pTreeCtrl->SetItemState(hCurItem,TVIS_BOLD,TVIS_BOLD);
		//切换激活文件并刷新显示区及树节点
		AgentServer.ActivateTowerFile(pFileGroup->pActiveFile);
	}
}

void CRemoteServerDlg::OnDblclkTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnSetActiveItem();//激活对应选项
	*pResult = 0;
}

//激活
void CRemoteServerDlg::OnSetActiveItem() 
{
	CTowerTreeCtrl* pTreeCtrl = GetTreeCtrl();
	HTREEITEM hCurItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=NULL;
	if(hCurItem)
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hCurItem);
	if(pItemInfo==NULL)
		return;
	 if(pItemInfo->itemType==SERVER_TOWERFILE)
	{
		CServerTowerFile *pTowerFile=(CServerTowerFile*)pItemInfo->dwRefData;
		if(DownloadTowerFile(pTowerFile))
		{
			TMS.InitTowerPartsVersionSerial(pTowerFile->m_idTowerType);
			//CXhChar16 label;
			//AgentServer.m_pActiveTowerType->hashPartsVerSerial
			//3.设置当前项粗体显示
			pTreeCtrl->SetVisitItemFunc(VisitTowerFileItem);	
			pTreeCtrl->TraverseItem(m_hServerContentItem);		//遍历树
			pTreeCtrl->SetItemState(hCurItem,TVIS_BOLD,TVIS_BOLD);
			//4.切换激活文件并刷新显示区及树节点
			AgentServer.ActivateTowerFile(pTowerFile);
			theApp.GetLDSDoc()->SetTitle(pTowerFile->m_sFileName);
			theApp.GetLDSDoc()->UpdateAllViews(NULL);
		}
	}
}

void CRemoteServerDlg::OnTvnItemexpandingTreeCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	TREEITEM_INFO* pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(pNMTreeView->itemNew.hItem);
	if(pItemInfo!=NULL&&pItemInfo->itemType==SERVER_CONTENT&&!theApp.m_bNetModelWork)
	{
		//TODO:展开时如果未连接服务器，自动进行连接
		theApp.m_bNetModelWork=theApp.LogonTMSServer();
		//m_hServerContentItem=pTreeCtrl->InsertItem("服务器",IMG_SERVER,IMG_SERVER,TVI_ROOT);
		if(theApp.m_bNetModelWork)
			pTreeCtrl->SetItemImage(m_hServerContentItem,IMG_MACHINE_SERVER,IMG_MACHINE_SERVER);
	}
	*pResult = 0;
}
#endif
