// DesWireNodeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Lds.h"
#include "DesWireNodeDlg.h"
#include "WireInfoInputDlg.h"
#include "afxdialogex.h"
#include "GlobalFunc.h"
#include "SortFunc.h"
#include "LDSDoc.h"

#ifdef __PART_DESIGN_INC_
//�ص�����
static int LocalCompareFunc(const WIRE_NODE& item1,const WIRE_NODE& item2)
{
	if(item1.pos.x*item2.pos.x>0)
	{
		if(item1.pos.z>item2.pos.z)
			return 1;
		else if(item1.pos.z<item2.pos.z)
			return -1;
		else
		{
			if(fabs(item1.pos.x)>fabs(item2.pos.x))
				return -1;
			else if(fabs(item1.pos.x)<fabs(item2.pos.x))
				return 1;
			else
				return 0;
		}
	}
	else if(item1.pos.x>item2.pos.x)
		return 1;
	else if(item1.pos.x<item2.pos.x)
		return -1;
	else
		return 0;
}
static int comparefun(const WIRE_NODE& item1,const WIRE_NODE& item2)
{
	BYTE cType1=2,cType2=2;
	if(item1.m_xWirePlace.ciWireType=='E')
		cType1=1;
	else if(item1.m_xWirePlace.ciWireType =='J')
		cType1=3;
	if(item2.m_xWirePlace.ciWireType =='E')
		cType2=1;
	else if(item2.m_xWirePlace.ciWireType =='J')
		cType2=3;
	//�������������
	if(item1.m_xWirePlace.iSerial>item2.m_xWirePlace.iSerial)
		return 1;
	else if(item1.m_xWirePlace.iSerial <item2.m_xWirePlace.iSerial)
		return -1;
	else
	{	//���ݹ�����������
		if(cType1>cType2)
			return 1;
		else if(cType1<cType2)
			return -1;
		else
		{	//���ݸ���������
			if(item1.m_xWirePlace.ciPostCode>item2.m_xWirePlace.ciPostCode)
				return 1;
			else if(item1.m_xWirePlace.ciPostCode <item2.m_xWirePlace.ciPostCode)
				return -1;
			else
				return 0;
		}
	}
}
static BOOL FireModifyValue(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem,CString& sTextValue)
{
	CDesWireNodeDlg *pDlg=(CDesWireNodeDlg*)pListCtrl->GetParent();
	if(pDlg==NULL)
		return FALSE;
	WIRE_NODE* pWireNode=(WIRE_NODE*)pSelItem->m_idProp;
	if(pWireNode==NULL)
		return FALSE;
	CString sOldValue=_T("");
	if(pSelItem&&pSelItem->m_lpNodeInfo)
		sOldValue=pSelItem->m_lpNodeInfo->GetSubItemText(iSubItem);
	//��ǰ��Ԫ���������޸ģ����Ĺ���Ϊ�༭״̬
	if(sOldValue.CompareNoCase(sTextValue)==0)
		return FALSE;
	if(iSubItem==1)
		pWireNode->m_iNo=atoi(sTextValue);
	else if(iSubItem==2)
	{	//����
		if(sTextValue.CompareNoCase("����")==0)
			pWireNode->m_xWirePlace.ciWireType='C';
		else if(sTextValue.CompareNoCase("����")==0)
			pWireNode->m_xWirePlace.ciWireType ='E';
		else if(sTextValue.CompareNoCase("����")==0)
			pWireNode->m_xWirePlace.ciWireType ='J';
		else if(sTextValue.CompareNoCase("�޹���")==0)
			pWireNode->m_xWirePlace.ciWireType =0;
	}
	else if(iSubItem==3)
	{	//�ҽӷ�ʽ
		if(sTextValue.CompareNoCase("˫��")==0)
			pWireNode->m_xWirePlace.ciHangingStyle='S';
		else if(sTextValue.CompareNoCase("V��")==0)
			pWireNode->m_xWirePlace.ciHangingStyle='V';
		else if(sTextValue.CompareNoCase("��ͨ")==0)
			pWireNode->m_xWirePlace.ciHangingStyle=0;//'D';
	}
	else if(iSubItem==4)
	{	//���·���
		if(sTextValue.CompareNoCase("X��")==0)
			pWireNode->m_xWirePlace.ciWireDirection='X';
		else if(sTextValue.CompareNoCase("Y��")==0)
			pWireNode->m_xWirePlace.ciWireDirection='Y';
	}
	else if (iSubItem==5)
	{	//��������
		if (sTextValue.CompareNoCase("����")==0)
			pWireNode->m_xWirePlace.ciTensionType=1;
		else if (sTextValue.CompareNoCase("����")==0)
			pWireNode->m_xWirePlace.ciTensionType=2;
	}
	else if(iSubItem==6)	//�����
		pWireNode->m_xWirePlace.iSerial=atoi(sTextValue);
	else if(iSubItem==7)
		pWireNode->m_xWirePlace.ciPostCode=atoi(sTextValue);
	CXhChar50 sName=CUniWireModel::GetWirePtGimName(pWireNode->m_xWirePlace,pWireNode->pos);
	pListCtrl->SetSubItemText(pSelItem,8,sName);
	return TRUE;
}

static BOOL FireButtonClick(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem)
{
	CDesWireNodeDlg *pDlg=(CDesWireNodeDlg*)pListCtrl->GetParent();
	if(pDlg==NULL)
		return FALSE;
	WIRE_NODE* pWireNode=(WIRE_NODE*)pSelItem->m_idProp;
	if(pWireNode==NULL)
		return FALSE;
	if (iSubItem == 1)
	{
		pDlg->SetCmdPickPromptStr("��ѡ��ڵ㡢�ҿ׻���<�س�ȷ��>:");
		pDlg->SelectObject(0, (long)pWireNode);
	}
	return TRUE;
}
static BOOL FireDeleteItem(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem)
{	
	CDesWireNodeDlg *pDlg=(CDesWireNodeDlg*)pListCtrl->GetParent();
	if(pDlg==NULL)
		return FALSE;
	WIRE_NODE* pSelWireNode=(WIRE_NODE*)pItem->m_idProp;
	if(pSelWireNode==NULL)
		return FALSE;
	for(WIRE_NODE* pWireNode=pDlg->m_arrWireNode.GetFirst();pWireNode;pWireNode=pDlg->m_arrWireNode.GetNext())
	{
		if(pWireNode==pSelWireNode)
		{
			pDlg->m_arrWireNode.DeleteCursor();
			break;
		}
	}
	pDlg->m_arrWireNode.Clean();
	return TRUE;
}
static BOOL FireItemChanged(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,NM_LISTVIEW* pNMListView)
{	
	CDesWireNodeDlg *pDlg=(CDesWireNodeDlg*)pListCtrl->GetParent();
	if(pDlg==NULL)
		return FALSE;
	WIRE_NODE* pSelWireNode=(WIRE_NODE*)pItem->m_idProp;
	if(pSelWireNode==NULL)
		return FALSE;
	//������������ʾ
	pDlg->SetSelectIndex(pListCtrl->GetSelectedItem());
	//ѡ�йҵ�
	g_pSolidDraw->ReleaseSnapStatus();
	CLDSPart* pPart = NULL;
	CLDSNode* pNode=console.FromNodeHandle(pSelWireNode->m_hRelatedObj);
	if (pNode == NULL)
		pPart = console.FromPartHandle(pSelWireNode->m_hRelatedObj);
	if(pNode || pPart)
	{
		GEPOINT center = (pNode) ? pNode->xOriginalPos : pPart->ucs.origin;
		SCOPE_STRU scope;
		scope.VerifyVertex(center);
		scope.fMinX -= 500;
		scope.fMaxX += 500;
		scope.fMinY -= 500;
		scope.fMaxY += 500;
		scope.fMinZ -= 500;
		scope.fMaxZ += 500;
		g_pSolidDraw->SetEntSnapStatus(pSelWireNode->m_hRelatedObj);
		g_pSolidOper->FocusTo(scope,g_sysPara.m_fMaxScaleScr2User);
		g_pSolidDraw->Draw();
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// CDesWireNodeDlg �Ի���
IMPLEMENT_DYNAMIC(CDesWireNodeDlg, CDialogEx)
int CDesWireNodeDlg::nScrLocationX=0;
int CDesWireNodeDlg::nScrLocationY=0;
int CDesWireNodeDlg::nClientRectW = 467;
int CDesWireNodeDlg::nClientRectH = 384;
CDesWireNodeDlg::CDesWireNodeDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesWireNodeDlg::IDD, pParent)
{
	m_iCurSel=-1;
	m_nBtnHeigh = 0;
	m_nMarginBottom = m_nMarginRight = 12;
}

CDesWireNodeDlg::~CDesWireNodeDlg()
{
}

void CDesWireNodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CTRL, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CDesWireNodeDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BN_ADD_WIRE_NODE, &CDesWireNodeDlg::OnBnAddWireNode)
	ON_BN_CLICKED(IDC_BN_IMPORT_WIRE_MODEL, &CDesWireNodeDlg::OnBnImportWireModel)
	ON_BN_CLICKED(IDC_BN_EXPORT_WIRE_MODEL, &CDesWireNodeDlg::OnBnExportWireModel)
END_MESSAGE_MAP()


// CDesWireNodeDlg ��Ϣ�������
BOOL CDesWireNodeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_bInernalStart)	//�ڲ�����
		FinishSelectObjOper(m_hPickObj,m_idEventProp);
	else
		ExtractWireNodeFromTa();
	//
	m_listCtrl.EmptyColumnHeader();
	m_listCtrl.AddColumnHeader("..");
	m_listCtrl.AddColumnHeader("���");
	m_listCtrl.AddColumnHeader("����");
	m_listCtrl.AddColumnHeader("����");
	m_listCtrl.AddColumnHeader("�߷���");
	m_listCtrl.AddColumnHeader("����");
	m_listCtrl.AddColumnHeader("�����");
	m_listCtrl.AddColumnHeader("������");
	m_listCtrl.AddColumnHeader("����");
	long col_wide_arr[9]={20,47,50,50,53,50,53,53,75};
	m_listCtrl.InitListCtrl(col_wide_arr,FALSE);
	m_listCtrl.SetModifyValueFunc(FireModifyValue);
	m_listCtrl.SetItemChangedFunc(FireItemChanged);
	m_listCtrl.SetDeleteItemFunc(FireDeleteItem);
	m_listCtrl.SetButtonClickFunc(FireButtonClick);
	m_listCtrl.SetImageWidth(0);	//������
	RefreshListCtrl();
	if(m_bInernalStart)
	{	//�ڲ�����,�ظ�ѡ����
		for(int i=0;i<m_listCtrl.GetItemCount();i++)
		{	//
			CSuperGridCtrl::CTreeItem* pItem=m_listCtrl.GetTreeItem(i);
			if(m_idEventProp==pItem->m_idProp)
				m_listCtrl.SetItemState(i,LVNI_SELECTED|LVNI_FOCUSED,LVNI_SELECTED|LVNI_FOCUSED);
		}
	}
	else
	{
		//��¼�Ի���߽�
		CRect clientRc, rect;
		GetClientRect(clientRc);
		CWnd *pWnd = GetDlgItem(IDC_LIST_CTRL);
		if (pWnd)
		{
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_nMarginRight = clientRc.right - rect.right;
		}
		pWnd = GetDlgItem(IDOK);
		if (pWnd)
		{
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_nMarginBottom = clientRc.bottom - rect.bottom;
			m_nBtnHeigh = rect.Height();
		}
	}
	//�ƶ��Ի������Ͻ�
	CRect rect;
	rect.left = nScrLocationX;
	rect.top = nScrLocationY;
	rect.right = rect.left + nClientRectW + 16;
	rect.bottom = rect.top + nClientRectH + 40;
	MoveWindow(rect,TRUE);
	LayoutDlg();
	m_bEnableWindowMoveListen=TRUE;
	m_bEnableWindowSizeListen=TRUE;
	UpdateData(FALSE);
	return TRUE;
}
void CDesWireNodeDlg::RefreshListCtrl()
{
	m_listCtrl.DeleteAllItems();
	CString sTextValue;
	for(WIRE_NODE* pWireNode=m_arrWireNode.GetFirst();pWireNode;pWireNode=m_arrWireNode.GetNext())
	{
		CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
		lpInfo->AddSubItemText("",TRUE);
		//�ڵ���
		sTextValue.Format("%d",pWireNode->m_iNo);
		lpInfo->AddSubItemText(sTextValue);
		lpInfo->SetControlType(1,GCT_BTN_EDIT);
		//����
		if(pWireNode->m_xWirePlace.ciWireType=='C')
			sTextValue="����";
		else if(pWireNode->m_xWirePlace.ciWireType=='E')
			sTextValue="����";
		else if(pWireNode->m_xWirePlace.ciWireType=='J')
			sTextValue="����";
		else
			sTextValue="�޹���";
		lpInfo->AddSubItemText(sTextValue,FALSE);
		lpInfo->SetControlType(2,GCT_CMB_LIST);
		lpInfo->SetListItemsStr(2,"����|����|����|�޹���");
		//�ҽӷ�ʽ
		if(pWireNode->m_xWirePlace.ciHangingStyle=='S')
			sTextValue="˫��";
		else if(pWireNode->m_xWirePlace.ciHangingStyle=='V')
			sTextValue="V��";
		else if(pWireNode->m_xWirePlace.ciHangingStyle==0||
				pWireNode->m_xWirePlace.ciHangingStyle=='D')
			sTextValue="��ͨ";
		lpInfo->AddSubItemText(sTextValue,pWireNode->m_xWirePlace.ciWireType !='C');
		lpInfo->SetControlType(3,GCT_CMB_LIST);
		lpInfo->SetListItemsStr(3,"��ͨ|˫��|V��");
		//���·���
		if(pWireNode->m_xWirePlace.ciWireDirection=='X')
			sTextValue="X��";
		else
			sTextValue="Y��";
		lpInfo->AddSubItemText(sTextValue);
		lpInfo->SetControlType(4,GCT_CMB_LIST);
		lpInfo->SetListItemsStr(4,"X��|Y��");
		//��������
		if (pWireNode->m_xWirePlace.ciTensionType==1)
			sTextValue="����";
		else if (pWireNode->m_xWirePlace.ciTensionType==2)
			sTextValue="����";
		else
			sTextValue="";
		lpInfo->AddSubItemText(sTextValue);
		lpInfo->SetControlType(5, GCT_CMB_EDIT);
		lpInfo->SetListItemsStr(5, "����|����");
		//�����
		sTextValue.Format("%d",pWireNode->m_xWirePlace.iSerial);
		lpInfo->AddSubItemText(sTextValue);
		lpInfo->SetControlType(5,GCT_CMB_EDIT);
		lpInfo->SetListItemsStr(6,"1|2|3|4|5|6|");
		//������
		sTextValue.Format("%d",pWireNode->m_xWirePlace.ciPostCode,pWireNode->m_xWirePlace.ciWireType=='E');
		lpInfo->AddSubItemText(sTextValue);
		//����
		CXhChar50 sName=CUniWireModel::GetWirePtGimName(pWireNode->m_xWirePlace,pWireNode->pos);
		sTextValue.Format("%s",(char*)sName);
		lpInfo->AddSubItemText(sTextValue,TRUE);
		//
		CSuperGridCtrl::CTreeItem* pItem=m_listCtrl.InsertRootItem(lpInfo);
		if(pItem)
			pItem->m_idProp=(DWORD)pWireNode;
	}
	m_listCtrl.Redraw();
}

WIRE_NODE* CDesWireNodeDlg::GetSelWireNode()
{
	int iCurSel=m_listCtrl.GetSelectedItem();
	CSuperGridCtrl::CTreeItem *pCurItem=m_listCtrl.GetTreeItem(iCurSel);
	if(pCurItem==NULL)
		pCurItem=m_listCtrl.GetTreeItem(m_iCurSel);
	if(pCurItem==NULL)
		return NULL;
	WIRE_NODE *pSelNode=(WIRE_NODE*)pCurItem->m_idProp;
	return pSelNode;
}
void CDesWireNodeDlg::OnBnAddWireNode()
{
	//SetCmdPickPromptStr("��ѡ��ڵ㡢�ҿ׻���<�س�ȷ��>:");
	SelectObject(CLS_NODE,1);	//��ӹ��ߵ�
}
BOOL CDesWireNodeDlg::FinishSelectObjOper(long hPickObj,long idEventProp)
{
	CLDSNode* pNode = NULL;
	CLDSPart* pPart = NULL;
	if (hPickObj < 0)
	{
		OBJECT_HANDLER handler;
		g_pSolidDraw->GetSolidObject((DWORD)hPickObj, &handler);
		pPart = console.FromPartHandle(handler.hiberid.masterId);
	}
	else
		pPart = console.FromPartHandle(hPickObj);
	if (pPart == NULL)
		pNode = console.FromNodeHandle(hPickObj);
	if (pPart == NULL && pNode == NULL)
		return FALSE;
	if (pPart && pPart->GetClassTypeId() != CLS_FITTINGS && pPart->GetClassTypeId() != CLS_BOLT)
		return FALSE;
	if(idEventProp==1)
	{	//��ӹҵ�
		static CWireInfoInputDlg inputdlg;
		if(inputdlg.DoModal()!=IDOK)
			return FALSE;
		if (pNode)
		{
			CLDSNodePtr nodeArr[2] = { pNode,pNode->GetMirNode(MIRMSG(1)) };
			for (int i = 0; i < 2; i++)
			{
				if (nodeArr[i] == NULL)
					continue;
				WIRE_NODE* pWireNode = NULL;
				for (pWireNode = m_arrWireNode.GetFirst(); pWireNode; pWireNode = m_arrWireNode.GetNext())
				{
					if (pWireNode->m_hRelatedObj == nodeArr[i]->handle)
						break;
				}
				if (pWireNode == NULL)
				{
					pWireNode = m_arrWireNode.append();
					pWireNode->m_hRelatedObj = nodeArr[i]->handle;
					pWireNode->m_iNo = nodeArr[i]->point_i;
				}
				pWireNode->pos = nodeArr[i]->xOriginalPos;
				//E.����|C.����|J.����
				if (inputdlg.m_iWireType == 0)
					pWireNode->m_xWirePlace.ciWireType = 'C';
				else if (inputdlg.m_iWireType == 1)
					pWireNode->m_xWirePlace.ciWireType = 'E';
				else
					pWireNode->m_xWirePlace.ciWireType = 'J';
				//0.����|'S'.˫��|'V'.V��
				if (inputdlg.m_iHangStyle == 0)
					pWireNode->m_xWirePlace.ciHangingStyle = 0;
				else
					pWireNode->m_xWirePlace.ciHangingStyle = (inputdlg.m_iHangStyle == 1) ? 'S' : 'V';
				pWireNode->m_xWirePlace.ciWireDirection = inputdlg.m_ciWireDir;
				pWireNode->m_xWirePlace.ciTensionType = inputdlg.m_iTentionType;
				pWireNode->m_xWirePlace.ciPostCode = inputdlg.m_nPostCode;	//������,
				pWireNode->m_xWirePlace.iSerial = inputdlg.m_iPhaseSerial;
			}
		}
		else if(pPart)
		{
			WIRE_NODE* pWireNode = NULL;
			for (pWireNode = m_arrWireNode.GetFirst(); pWireNode; pWireNode = m_arrWireNode.GetNext())
			{
				if (pWireNode->m_hRelatedObj == pPart->handle)
					break;
			}
			if (pWireNode == NULL)
			{
				pWireNode = m_arrWireNode.append();
				pWireNode->m_hRelatedObj = pPart->handle;
			}
			pWireNode->pos = pPart->ucs.origin;
			if (inputdlg.m_iWireType == 0)
				pWireNode->m_xWirePlace.ciWireType = 'C';
			else if (inputdlg.m_iWireType == 1)
				pWireNode->m_xWirePlace.ciWireType = 'E';
			else
				pWireNode->m_xWirePlace.ciWireType = 'J';
			//0.����|'S'.˫��|'V'.V��
			if (inputdlg.m_iHangStyle == 0)
				pWireNode->m_xWirePlace.ciHangingStyle = 0;
			else
				pWireNode->m_xWirePlace.ciHangingStyle = (inputdlg.m_iHangStyle == 1) ? 'S' : 'V';
			pWireNode->m_xWirePlace.ciWireDirection = inputdlg.m_ciWireDir;
			pWireNode->m_xWirePlace.ciTensionType = inputdlg.m_iTentionType;
			pWireNode->m_xWirePlace.ciPostCode = inputdlg.m_nPostCode;	//������,
			pWireNode->m_xWirePlace.iSerial = inputdlg.m_iPhaseSerial;
		}
	}
	else
	{
		WIRE_NODE* pWireNode=(WIRE_NODE*)idEventProp;
		if (pNode)
		{
			pWireNode->m_hRelatedObj = pNode->handle;
			pWireNode->m_iNo = pNode->point_i;
			pWireNode->pos = pNode->xOriginalPos;
		}
		else if (pPart)
		{
			pWireNode->m_hRelatedObj = pPart->handle;
			pWireNode->pos = pPart->ucs.origin;
		}
	}
	return TRUE;
}
void CDesWireNodeDlg::ExtractWireNodeFromTa()
{
	if(gxWirePointModel.m_hashWirePoints.GetNodeNum()<=0)
		Ta.RetrieveWireModelFromNodes(&gxWirePointModel);
	//��ȡ�ҵ�
	WIRE_NODE wire_node;
	PRESET_ARRAY128<WIRE_NODE> arrWireNode;
	WIRE_NODE* pWireNode=NULL,*pMirWireNode=NULL;
	for(CXhWirePoint* pWirePt=gxWirePointModel.m_hashWirePoints.GetFirst();pWirePt;pWirePt=gxWirePointModel.m_hashWirePoints.GetNext())
	{
		CLDSNode* pNode=Ta.FromNodeHandle(pWirePt->m_hRelaNode);
		pWireNode=arrWireNode.Append(wire_node);
		if (pWirePt->m_hRelaNode > 0X20)
			pWireNode->m_hRelatedObj = pWirePt->m_hRelaNode;
		else if (pWirePt->m_hRelaPart > 0X20)
			pWireNode->m_hRelatedObj = pWirePt->m_hRelaPart;
		//else if (pWirePt->holes[0].hBolt > 0X20)
		//	pWireNode->m_hRelatedObj = pWirePt->holes[0].hBolt;
		pWireNode->m_iNo=pNode?pNode->point_i:0;
		pWireNode->pos=pWirePt->xPosition;
		pWireNode->vSketchArrow = pWirePt->m_vSketchArrow;
		pWireNode->m_xWirePlace = pWirePt->m_xWirePlace;
	}
	//�����Ϊ��ʱ����е���
	if(pWireNode&&pWireNode->m_xWirePlace.iSerial<=0)
	{
		CQuickSort<WIRE_NODE>::QuickSort(arrWireNode.pPresetObjs,arrWireNode.Count,LocalCompareFunc);
		int iPhaseSerialC=0,iPhaseSerialE=0;
		double posXC=0,posXE=0;
		for(UINT i=0;i<arrWireNode.Count;i++)
		{
			pWireNode=&arrWireNode.At(i);
			if(pWireNode->m_xWirePlace.ciWireType=='C')
			{
				if(fabs(posXC-pWireNode->pos.x)>1)
				{
					iPhaseSerialC++;
					posXC=pWireNode->pos.x;
				}
				pWireNode->m_xWirePlace.iSerial=iPhaseSerialC;
			}
			else if(pWireNode->m_xWirePlace.ciWireType=='E')
			{
				if(fabs(posXE-pWireNode->pos.x)>1)
				{
					iPhaseSerialE++;
					posXE=pWireNode->pos.x;
				}
				pWireNode->m_xWirePlace.iSerial=iPhaseSerialE;
			}
		}
	}
	//��ʼ��¼����Ϣ
	if(arrWireNode.Count>0)
	{	//���ڹҵ�
		CQuickSort<WIRE_NODE>::QuickSort(arrWireNode.pPresetObjs,arrWireNode.Count,comparefun);
		for(UINT i=0;i<arrWireNode.Count;i++)
		{
			WIRE_NODE* pWireNode=m_arrWireNode.append();
			pWireNode->ClonProp(&arrWireNode.At(i));
		}
	}
}
//��Ϣ��Ӧ�¼�
int CDesWireNodeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen=FALSE;
	m_bEnableWindowSizeListen=FALSE;
	if (CCallBackDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
void CDesWireNodeDlg::OnMove(int x, int y)
{
	CCallBackDialog::OnMove(x, y);

	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}

void CDesWireNodeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	if (m_bEnableWindowSizeListen)
	{
		nClientRectW = cx;
		nClientRectH = cy;
		LayoutDlg();
	}
}

void CDesWireNodeDlg::LayoutDlg()
{
	int cx = nClientRectW;
	int cy = nClientRectH;
	//�����б�
	CRect rect;
	CWnd *pTopWnd = GetDlgItem(IDC_LIST_CTRL);
	if (pTopWnd)
	{
		pTopWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.right = cx - m_nMarginRight;
		rect.bottom = cy - m_nMarginBottom - m_nBtnHeigh - 2;
		pTopWnd->MoveWindow(rect);
	}
	//�ײ���ť
	CWnd *pBtmWnd[5] = { NULL };
	pBtmWnd[0] = GetDlgItem(IDC_BN_ADD_WIRE_NODE);
	pBtmWnd[1] = GetDlgItem(IDC_BN_IMPORT_WIRE_MODEL);
	pBtmWnd[2] = GetDlgItem(IDC_BN_EXPORT_WIRE_MODEL);
	pBtmWnd[3] = GetDlgItem(IDOK);
	pBtmWnd[4] = GetDlgItem(IDCANCEL);
	for (int i = 0; i < 5; i++)
	{
		if (pBtmWnd[i])
		{
			pBtmWnd[i]->GetWindowRect(&rect);
			ScreenToClient(&rect);
			rect.bottom = cy - m_nMarginBottom;
			rect.top = rect.bottom - m_nBtnHeigh;
			pBtmWnd[i]->MoveWindow(rect);
		}
	}
}

void CDesWireNodeDlg::UpdateUIWireNodesToUniWireModel()
{
	gxWirePointModel.m_hashWirePoints.Empty();
	//�����û��������¸��¹ҵ�
	for(WIRE_NODE* pWireNode=m_arrWireNode.GetFirst();pWireNode;pWireNode=m_arrWireNode.GetNext())
	{
		CLDSNode* pNode = console.FromNodeHandle(pWireNode->m_hRelatedObj);
		CLDSPart* pPart = console.FromPartHandle(pWireNode->m_hRelatedObj);
		//
		CXhWirePoint* pWirePt=gxWirePointModel.m_hashWirePoints.Add(0);
		if (pNode)
			pWirePt->m_hRelaNode = pNode->handle;
		else if (pPart)
		{
			pWirePt->m_hRelaPart = pPart->handle;
			if (pPart->GetClassTypeId() == CLS_FITTINGS)
			{	//
				CLDSFitting* pFitting = (CLDSFitting*)pPart;
				pWirePt->holes[0].hBolt = pFitting->holes[0].hBolt;
				pWirePt->holes[0].hResidePart = pFitting->holes[0].hResidePart;
				pWirePt->holes[1].hBolt = pFitting->holes[1].hBolt;
				pWirePt->holes[1].hResidePart = pFitting->holes[1].hResidePart;
			}
			else if (pPart->GetClassTypeId() == CLS_BOLT)
			{	//
				CLDSBolt* pBolt = (CLDSBolt*)pPart;
				pWirePt->holes[0].hBolt = pBolt->handle;
				pWirePt->holes[0].hResidePart = pBolt->des_base_pos.hPart;
			}
		}	
		pWirePt->xPosition = pWireNode->pos;
		pWirePt->m_vSketchArrow = pWireNode->vSketchArrow;
		pWirePt->m_xWirePlace = pWireNode->m_xWirePlace;
		gxWirePointModel.UpdateWirePointGimName(pWirePt);
	}
}
void CDesWireNodeDlg::UpdateUniWireModelToUIWireNodes()
{
	m_arrWireNode.Empty();
	//��ȡ�ҵ�
	for(CXhWirePoint* pWirePt=gxWirePointModel.m_hashWirePoints.GetFirst();pWirePt;pWirePt=gxWirePointModel.m_hashWirePoints.GetNext())
	{
		CLDSNode* pNode=console.FromNodeHandle(pWirePt->m_hRelaNode);
		WIRE_NODE* pWireNode=m_arrWireNode.append();
		if (pWirePt->m_hRelaNode > 0X20)
			pWireNode->m_hRelatedObj = pWirePt->m_hRelaNode;
		else if (pWirePt->m_hRelaPart > 0X20)
			pWireNode->m_hRelatedObj = pWirePt->m_hRelaPart;
		//else if (pWirePt->holes[0].hBolt > 0X20)
		//	pWireNode->m_hRelatedObj = pWirePt->holes[0].hBolt;
		pWireNode->m_iNo=pNode?pNode->point_i:0;
		pWireNode->pos=pWirePt->xPosition;
		pWireNode->vSketchArrow = pWirePt->m_vSketchArrow;
		pWireNode->m_xWirePlace = pWirePt->m_xWirePlace;
	}
	this->RefreshListCtrl();
}
void CDesWireNodeDlg::OnBnImportWireModel()
{
	CXhChar50 filename=theApp.GetLDSDoc()->GetTitle();
	char* pchDotDelimiter = SearchChar(filename, ".", true);
	if (pchDotDelimiter)
		*pchDotDelimiter = 0;
	CFileDialog dlg(TRUE,"wpm",filename,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"�����ҵ���Ϣģ��(*.wpm)|*.wpm|�����ļ�(*.*)|*.*||");
	if(dlg.DoModal()!=IDOK)
		return;
	FILE* fp=fopen(dlg.GetPathName(),"rb");
	if(fp==NULL)
	{
		AfxMessageBox("�ļ���ʧ��");
		return;
	}
	UINT uiDocType,uiFileVersion,uiFileLength;
	fread(&uiDocType,4,1,fp);
	fread(&uiFileVersion,4,1,fp);
	fread(&uiFileLength,4,1,fp);
	CHAR_ARRAY filecontents(uiFileLength);
	fread(filecontents,1,uiFileLength,fp);
	fclose(fp);
	CBuffer buff(filecontents,uiFileLength);
	gxWirePointModel.FromBuffer(buff);
	UpdateUniWireModelToUIWireNodes();
}

void CDesWireNodeDlg::OnBnExportWireModel()
{
	CXhChar50 filename = theApp.GetLDSDoc()->GetTitle();
	char* pchDotDelimiter = SearchChar(filename, ".", true);
	if (pchDotDelimiter)
		*pchDotDelimiter = 0;
	CXhChar500 filter = "�����ҵ���Ϣģ��(*.xml)|*.xml|";
	filter.Append("�����ҵ���Ϣģ��(*.wpm)|*.wpm|");
	filter.Append("�����ļ�(*.*)|*.*||");
	CFileDialog dlg(FALSE, "", filename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	if(dlg.DoModal()!=IDOK)
		return;
	UpdateUIWireNodesToUniWireModel();
	CString ext = dlg.GetFileExt();
	if (ext.CompareNoCase("xml") == 0)
		gxWirePointModel.ExportUniWireXmlData(dlg.GetPathName());
	else
	{
		FILE* fp=fopen(dlg.GetPathName(),"wb");
		if (fp==NULL)
		{
			AfxMessageBox("�ļ���ʧ��");
			return;
		}
		UINT uiDocType=3523490587,uiFileVersion=1000000,uiFileLength;
		CBuffer buff(0x10000);
		gxWirePointModel.ToBuffer(buff);
		fwrite(&uiDocType,4,1,fp);
		fwrite(&uiFileVersion,4,1,fp);
		uiFileLength=buff.GetLength();
		fwrite(&uiFileLength,4,1,fp);
		fwrite(buff.GetBufferPtr(),1,uiFileLength,fp);
		fclose(fp);
	}
}
void CDesWireNodeDlg::OnOK()
{
	UpdateUIWireNodesToUniWireModel();
	return CDialog::OnOK();
}
#endif
