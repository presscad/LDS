// InputWirePointDlg.cpp: ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "InputWirePointDlg.h"
#include "afxdialogex.h"
#include "PropertyListOper.h"
#include "env_def.h"
#include "GlobalFunc.h"

#ifdef __PART_DESIGN_INC_
//////////////////////////////////////////////////////////////////////////
//�ص�����
static BOOL ModifyProperty(CPropertyList *pPropList, CPropTreeItem *pItem, CString &valueStr)
{
	CInputWirePointDlg* pDlg = (CInputWirePointDlg*)pPropList->GetParent();
	if (pDlg == NULL)
		return FALSE;
	BOOL bUpdateGimName = FALSE;
	CPropertyListOper<CXhWirePoint> oper(pPropList, &pDlg->m_curWriePt);
	CXhChar100 sValue(valueStr);
	if (pItem->m_idProp == CXhWirePoint::GetPropID("WireType"))
	{
		if (valueStr.CompareNoCase("����") == 0)
			pDlg->m_curWriePt.m_xWirePlace.ciWireType = 'C';
		else if (valueStr.CompareNoCase("����") == 0)
			pDlg->m_curWriePt.m_xWirePlace.ciWireType = 'E';
		else if (valueStr.CompareNoCase("����") == 0)
			pDlg->m_curWriePt.m_xWirePlace.ciWireType = 'J';
		//
		pPropList->DeleteAllSonItems(pItem);
		if (pDlg->m_curWriePt.m_xWirePlace.ciWireType == 'E')
			oper.InsertCmbEditPropItem(pItem, "PhaseSerial", "", "", "", -1, TRUE);
		else
		{
			oper.InsertCmbEditPropItem(pItem, "CircuitSerial", "", "", "", -1, TRUE);
			oper.InsertCmbEditPropItem(pItem, "PhaseSerial", "", "", "", -1, TRUE);
		}
		bUpdateGimName = TRUE;
	}
	else if (pItem->m_idProp == CXhWirePoint::GetPropID("CircuitSerial"))
	{
		pDlg->m_curWriePt.m_xWirePlace.ciCircuitSerial = (BYTE)atoi(valueStr);
		bUpdateGimName = TRUE;
	}
	else if (pItem->m_idProp == CXhWirePoint::GetPropID("PhaseSerial"))
	{ 
		pDlg->m_curWriePt.m_xWirePlace.ciPhaseSerial = (BYTE)atoi(valueStr);
		bUpdateGimName = TRUE;
	}
	else if (pItem->m_idProp == CXhWirePoint::GetPropID("TensionType"))
	{
		if (valueStr.CompareNoCase("����") == 0)
			pDlg->m_curWriePt.m_xWirePlace.ciTensionType = 1;
		else if (valueStr.CompareNoCase("����") == 0)
			pDlg->m_curWriePt.m_xWirePlace.ciTensionType = 2;
		//
		pPropList->DeleteAllSonItems(pItem);
		if (pDlg->m_curWriePt.m_xWirePlace.ciTensionType == 2)
			oper.InsertCmbListPropItem(pItem, "WireDirec", "", "", "", -1, TRUE);
		bUpdateGimName = TRUE;
	}
	else if (pItem->m_idProp == CXhWirePoint::GetPropID("HangingStyle"))
	{
		if (valueStr.CompareNoCase("˫��") == 0)
			pDlg->m_curWriePt.m_xWirePlace.ciHangingStyle = 'S';
		else if (valueStr.CompareNoCase("V��") == 0)
			pDlg->m_curWriePt.m_xWirePlace.ciHangingStyle = 'V';
		else
			pDlg->m_curWriePt.m_xWirePlace.ciHangingStyle = 0;
		//
		pPropList->DeleteAllSonItems(pItem);
		if (pDlg->m_curWriePt.m_xWirePlace.ciHangingStyle != 0)
			oper.InsertCmbListPropItem(pItem, "PostCode", "", "", "", -1, TRUE);
		bUpdateGimName = TRUE;
	}
	else if (pItem->m_idProp == CXhWirePoint::GetPropID("PostCode"))
	{
		pDlg->m_curWriePt.m_xWirePlace.ciPostCode = atoi(valueStr);
		bUpdateGimName = TRUE;
	}
	else if (pItem->m_idProp == CXhWirePoint::GetPropID("GimName"))
		pDlg->m_curWriePt.m_sGimName.Copy(valueStr);
	else if (pItem->m_idProp == CXhWirePoint::GetPropID("HalfDegAngle"))
		pDlg->m_curWriePt.m_fHalfConeDegAngle = atof(valueStr);
	else if (pItem->m_idProp == CXhWirePoint::GetPropID("StyleName"))
		pDlg->m_curWriePt.m_szStyleName.Copy(valueStr);
	//
	if (bUpdateGimName)
	{
		gxWirePointModel.UpdateWirePointGimName(&pDlg->m_curWriePt);
		oper.UpdatePropItemValue("GimName");
	}
	return TRUE;
}

static BOOL ButtonWirePoint(CPropertyList *pPropList, CPropTreeItem* pItem)
{
	CInputWirePointDlg* pDlg = (CInputWirePointDlg*)pPropList->GetParent();
	if (pDlg == NULL)
		return FALSE;
	if (pItem->m_idProp == CXhWirePoint::GetPropID("HOLE1") ||
		pItem->m_idProp == CXhWirePoint::GetPropID("HOLE2"))
		pDlg->SelectObject(CLS_HOLEWALL, pItem->m_idProp);
	else if (pItem->m_idProp == CXhWirePoint::GetPropID("Fittings"))
		pDlg->SelectObject(CLS_FITTINGS, pItem->m_idProp);
	else if (pItem->m_idProp==CXhWirePoint::GetPropID("RelaNode"))
		pDlg->SelectObject(CLS_NODE, pItem->m_idProp);
	return TRUE;
}
static BOOL FirePopMenuClick(CPropertyList* pPropList, CPropTreeItem* pItem, CString sMenuName, int iMenu)
{
	CInputWirePointDlg *pDlg = (CInputWirePointDlg*)pPropList->GetParent();
	if (pDlg == NULL)
		return FALSE;
	if (pItem->m_idProp == CXhWirePoint::GetPropID("BasicInfo"))
	{
		if (iMenu == 0)	//��ӹҵ�
			pDlg->m_pSelWirePt = pDlg->m_hashWirePoint.Add(0);
		if (pDlg->m_pSelWirePt)
			pDlg->m_pSelWirePt->CloneProp(&pDlg->m_curWriePt);
		pDlg->RefreshListCtrl();
	}
	return TRUE;
}
static BOOL FireModifyValue(CSuperGridCtrl* pListCtrl, CSuperGridCtrl::CTreeItem* pSelItem, int iSubItem, CString& sTextValue)
{
	CInputWirePointDlg *pDlg = (CInputWirePointDlg*)pListCtrl->GetParent();
	if (pDlg == NULL)
		return FALSE;
	CXhWirePoint* pWireNode = (CXhWirePoint*)pSelItem->m_idProp;
	if (pWireNode == NULL)
		return FALSE;
	CString sOldValue = _T("");
	if (pSelItem&&pSelItem->m_lpNodeInfo)
		sOldValue = pSelItem->m_lpNodeInfo->GetSubItemText(iSubItem);
	//��ǰ��Ԫ���������޸ģ����Ĺ���Ϊ�༭״̬
	if (sOldValue.CompareNoCase(sTextValue) == 0)
		return FALSE;
	if (iSubItem == 2)
	{	//�����
		int iPhaseSerial = atoi(sTextValue);
		if(pWireNode->m_xWirePlace.ciWireType!='E'&&iPhaseSerial>0)
		{
			iPhaseSerial-=1;
			pWireNode->m_xWirePlace.ciCircuitSerial=1+(iPhaseSerial/3);
			pWireNode->m_xWirePlace.ciPhaseSerial=1+iPhaseSerial%3;
		}
		else
			pWireNode->m_xWirePlace.ciPhaseSerial=iPhaseSerial;
	}
	else if (iSubItem == 3)
	{	//��������
		if (sTextValue.CompareNoCase("����") == 0)
			pWireNode->m_xWirePlace.ciTensionType = 1;
		else if (sTextValue.CompareNoCase("����") == 0)
			pWireNode->m_xWirePlace.ciTensionType = 2;
	}
	gxWirePointModel.UpdateWirePointGimName(pWireNode);
	pListCtrl->SetSubItemText(pSelItem, 5, pWireNode->m_sGimName);
	return TRUE;
}
static BOOL FireDeleteItem(CSuperGridCtrl* pListCtrl, CSuperGridCtrl::CTreeItem* pItem)
{
	CInputWirePointDlg *pDlg = (CInputWirePointDlg*)pListCtrl->GetParent();
	if (pDlg == NULL)
		return FALSE;
	CXhWirePoint* pSelWireNode = (CXhWirePoint*)pItem->m_idProp;
	if (pSelWireNode == NULL)
		return FALSE;
	for (CXhWirePoint* pWireNode = pDlg->m_hashWirePoint.GetFirst(); pWireNode; pWireNode = pDlg->m_hashWirePoint.GetNext())
	{
		if (pWireNode == pSelWireNode)
		{
			pDlg->m_hashWirePoint.DeleteCursor();
			break;
		}
	}
	pDlg->m_hashWirePoint.Clean();
	return TRUE;
}
static BOOL FireItemChanged(CSuperGridCtrl* pListCtrl, CSuperGridCtrl::CTreeItem* pItem, NM_LISTVIEW* pNMListView)
{
	CInputWirePointDlg *pDlg = (CInputWirePointDlg*)pListCtrl->GetParent();
	if (pDlg == NULL)
		return FALSE;
	CXhWirePoint* pSelWireNode = (CXhWirePoint*)pItem->m_idProp;
	if (pSelWireNode == NULL)
		return FALSE;
	pDlg->m_pSelWirePt = pSelWireNode;
	pDlg->m_curWriePt.CloneProp(pSelWireNode);
	pDlg->DisplayProperty();
	//
	g_pSolidDraw->ReleaseSnapStatus();
	GEPOINT pos;
	CLDSNode* pNode = console.FromNodeHandle(pSelWireNode->m_hRelaNode);
	if (pNode)
		pos = pNode->Position(false);
	CLDSPart* pPart = console.FromPartHandle(pSelWireNode->m_hRelaPart);
	if (pPart==NULL)
		pPart = console.FromPartHandle(pSelWireNode->holes[0].hBolt);
	if(pPart)
		pos = pPart->ucs.origin;
	if (!pos.IsZero())
	{
		SCOPE_STRU scope;
		scope.VerifyVertex(pNode->Position(false));
		scope.fMinX -= 1500;
		scope.fMaxX += 1500;
		scope.fMinY -= 1500;
		scope.fMaxY += 1500;
		scope.fMinZ -= 1500;
		scope.fMaxZ += 1500;
		g_pSolidDraw->SetEntSnapStatus(pNode->handle);
		g_pSolidOper->FocusTo(scope, g_sysPara.m_fMaxScaleScr2User);
		g_pSolidDraw->Draw();
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// CInputWirePointDlg �Ի���

IMPLEMENT_DYNAMIC(CInputWirePointDlg, CDialog)
int CInputWirePointDlg::nScrLocationX = 0;
int CInputWirePointDlg::nScrLocationY = 0;
CInputWirePointDlg::CInputWirePointDlg(CWnd* pParent /*=nullptr*/)
	: CCallBackDialog(IDD_INPUT_WP_DLG, pParent)
{
	m_pSelWirePt = NULL;
	m_curWriePt.m_xWirePlace.ciWireType = 'C';
	m_curWriePt.m_xWirePlace.ciCircuitSerial = 1;
	m_curWriePt.m_xWirePlace.ciPhaseSerial = 1;
	m_curWriePt.m_xWirePlace.ciTensionType = 1;
	m_curWriePt.m_xWirePlace.ciPostCode = 1;
	m_curWriePt.m_xWirePlace.ciWireDirection = 0;
	gxWirePointModel.UpdateWirePointGimName(&m_curWriePt);
}

CInputWirePointDlg::~CInputWirePointDlg()
{
}

void CInputWirePointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CTRL, m_listCtrl);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
}


BEGIN_MESSAGE_MAP(CInputWirePointDlg, CDialog)
	ON_WM_MOVE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BN_IMPORT_WIRE_MODEL, OnBnImportWireModel)
	ON_BN_CLICKED(IDC_BN_EXPORT_WIRE_MODEL, OnBnExportWireModel)
END_MESSAGE_MAP()


// CInputWirePointDlg ��Ϣ�������
BOOL CInputWirePointDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (m_bInernalStart)	//�ڲ�����
		FinishSelectObjOper();
	else
	{	//��ȡ�ҵ���Ϣ
		if (gxWirePointModel.m_hashWirePoints.GetNodeNum() <= 0)
			Ta.RetrieveWireModelFromNodes(&gxWirePointModel);
		CXhWirePoint* pSrcWirePt = NULL, *pNewWirePt = NULL;
		for (pSrcWirePt = gxWirePointModel.m_hashWirePoints.GetFirst(); pSrcWirePt;
			pSrcWirePt = gxWirePointModel.m_hashWirePoints.GetNext())
		{
			pNewWirePt = m_hashWirePoint.Add(0);
			pNewWirePt->CloneProp(pSrcWirePt);
		}
	}
	//
	m_listCtrl.EmptyColumnHeader();
	m_listCtrl.AddColumnHeader("�ҵ��", 55);
	m_listCtrl.AddColumnHeader("����", 50);
	m_listCtrl.AddColumnHeader("�����", 55);
	m_listCtrl.AddColumnHeader("��������", 60);
	m_listCtrl.AddColumnHeader("��λ", 50);
	m_listCtrl.AddColumnHeader("����", 55);
	m_listCtrl.InitListCtrl(NULL, FALSE);
	m_listCtrl.SetModifyValueFunc(FireModifyValue);
	m_listCtrl.SetItemChangedFunc(FireItemChanged);
	m_listCtrl.SetDeleteItemFunc(FireDeleteItem);
	RefreshListCtrl();
	//
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.SetDividerScale(0.55);
	m_propList.SetModifyValueFunc(ModifyProperty);
	m_propList.SetButtonClickFunc(ButtonWirePoint);
	m_propList.SetPopMenuClickFunc(FirePopMenuClick);
	DisplayProperty();
	if (m_bInernalStart)
	{	//�ڲ�����,�ظ�ѡ����
		CPropTreeItem *pItem = m_propList.FindItemByPropId(m_idEventProp, NULL);
		m_propList.SetFocus();
		if (pItem)
			m_propList.SetCurSel(pItem->m_iIndex);	//����ѡ����
		//
		int i = 0;
		for(CXhWirePoint* pWirePt=m_hashWirePoint.GetFirst();pWirePt;pWirePt=m_hashWirePoint.GetNext(),i++)
		{
			CSuperGridCtrl::CTreeItem* pItem = m_listCtrl.GetTreeItem(i);
			if ((long)pWirePt == pItem->m_idProp)
				m_listCtrl.SetItemState(i, LVNI_SELECTED | LVNI_FOCUSED, LVNI_SELECTED | LVNI_FOCUSED);
		}
	}
	//�ƶ��Ի������Ͻ�
	CRect rect;
	GetWindowRect(rect);
	int width = rect.Width();
	int height = rect.Height();
	rect.left = nScrLocationX;
	rect.top = nScrLocationY;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	MoveWindow(rect, TRUE);
	m_bEnableWindowMoveListen = TRUE;
	UpdateData(FALSE);
	return TRUE;
}

void CInputWirePointDlg::DisplayProperty()
{
	m_propList.CleanTree();
	CPropTreeItem *pRoot = m_propList.GetRootItem();
	CPropTreeItem *pLeftItem = NULL, *pPropItem = NULL, *pGroupItem = NULL;
	CPropertyListOper<CXhWirePoint> oper(&m_propList, &m_curWriePt);
	//�ҵ���Ϣ
	pGroupItem = oper.InsertGrayPopMenuItem(pRoot, "BasicInfo");
	pPropItem = oper.InsertCmbListPropItem(pGroupItem, "WireType");		//��������
	if (m_curWriePt.m_xWirePlace.ciWireType == 'E')
		oper.InsertCmbEditPropItem(pPropItem, "PhaseSerial");	//���ߺ�
	else
	{
		oper.InsertCmbEditPropItem(pPropItem, "CircuitSerial");//��·��
		oper.InsertCmbEditPropItem(pPropItem, "PhaseSerial");	//���ߺ�
	}
	pPropItem = oper.InsertCmbListPropItem(pGroupItem, "TensionType");	//��������
	pPropItem = oper.InsertCmbListPropItem(pGroupItem, "HangingStyle");//װ�����
	if(m_curWriePt.m_xWirePlace.ciHangingStyle!=0)
		oper.InsertCmbListPropItem(pPropItem, "PostCode"); //������
	oper.InsertButtonPropItem(pGroupItem, "RelaNode");		//�����ڵ�
	pPropItem = oper.InsertEditPropItem(pGroupItem, "GimName");			//�ҵ�����
	pPropItem->SetReadOnly();
	//�����Ϣ
	pGroupItem = oper.InsertPropItem(pRoot, "JinjuInfo");
	oper.InsertButtonPropItem(pGroupItem, "Fittings");
	pPropItem = oper.InsertButtonPropItem(pGroupItem, "HOLE1");
	if (m_curWriePt.holes[0].hBolt > 0X20)
	{
		oper.InsertEditPropItem(pPropItem, "HOLE1.hBolt");
		oper.InsertEditPropItem(pPropItem, "HOLE1.hResidePart");
	}
	pPropItem = oper.InsertButtonPropItem(pGroupItem, "HOLE2");
	if (m_curWriePt.holes[1].hBolt > 0X20)
	{
		oper.InsertEditPropItem(pPropItem, "HOLE2.hBolt");
		oper.InsertEditPropItem(pPropItem, "HOLE2.hResidePart");
	}
	oper.InsertEditPropItem(pGroupItem, "HalfDegAngle");
	oper.InsertEditPropItem(pGroupItem, "StyleName");
	//
	m_propList.Redraw();
}

void CInputWirePointDlg::FinishSelectObjOper()
{
	if (!m_bInernalStart)
		return;
	CLDSPart *pPart = NULL;
	if (GetEventPropId() == CXhWirePoint::GetPropID("HOLE1") ||
		GetEventPropId() == CXhWirePoint::GetPropID("HOLE1"))
	{
		long hBolt = 0, hResidePart = 0;
		if (m_hPickObj < 0)
		{	//ѡ�йҿ����⴦��
			OBJECT_HANDLER handler;
			g_pSolidDraw->GetSolidObject(m_hPickObj, &handler);
			if (handler.hiberid.HiberUpId(1) == 1)
			{	//ͨ��(����)��˨���������������ױ�
				CLDSPart* pSelPart = console.FromPartHandle(handler.hiberid.masterId);
				if (pSelPart != NULL && pSelPart->IsBolt())
				{
					hBolt = pSelPart->handle;
					hResidePart = handler.hiberid.HiberUpId(2);
				}
			}
		}
		if (hBolt > 0 && hResidePart > 0)
		{
			if (GetEventPropId() == CXhWirePoint::GetPropID("HOLE1"))
			{
				m_curWriePt.holes[0].hBolt = hBolt;
				m_curWriePt.holes[0].hResidePart = hResidePart;
			}
			else
			{
				m_curWriePt.holes[1].hBolt = hBolt;
				m_curWriePt.holes[1].hResidePart = hResidePart;
			}
		}
	}
	else if (GetEventPropId() == CXhWirePoint::GetPropID("Fittings"))
	{
		pPart = Ta.FromPartHandle(m_hPickObj, CLS_FITTINGS);
		if (pPart)
		{
			CLDSFitting* pFitting = (CLDSFitting*)pPart;
			m_curWriePt.m_hRelaPart = pFitting->handle;
			m_curWriePt.holes[0].hBolt = pFitting->holes[0].hBolt;
			m_curWriePt.holes[0].hResidePart = pFitting->holes[0].hResidePart;
			m_curWriePt.holes[1].hBolt = pFitting->holes[1].hBolt;
			m_curWriePt.holes[1].hResidePart = pFitting->holes[1].hResidePart;
		}
	}
	else if (GetEventPropId() == CXhWirePoint::GetPropID("RelaNode"))
	{
		CLDSNode* pNode = console.FromNodeHandle(m_hPickObj);
		if (pNode)
			m_curWriePt.m_hRelaNode = m_hPickObj;
	}
}
void CInputWirePointDlg::UpdateUniWireModel()
{

}

void CInputWirePointDlg::RefreshListCtrl()
{
	CString sTextValue;
	m_listCtrl.DeleteAllItems();
	for (CXhWirePoint* pWireNode = m_hashWirePoint.GetFirst(); pWireNode; pWireNode = m_hashWirePoint.GetNext())
	{
		CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
		//�ҵ��
		sTextValue.Format("%d", pWireNode->Id);
		lpInfo->AddSubItemText(sTextValue,TRUE);
		lpInfo->SetControlType(0, GCT_EDIT);
		//����
		if (pWireNode->m_xWirePlace.ciWireType == 'C')
			sTextValue = "����";
		else if (pWireNode->m_xWirePlace.ciWireType == 'E')
			sTextValue = "����";
		else if (pWireNode->m_xWirePlace.ciWireType == 'J')
			sTextValue = "����";
		lpInfo->AddSubItemText(sTextValue, TRUE);
		lpInfo->SetControlType(1, GCT_CMB_LIST);
		lpInfo->SetListItemsStr(1, "����|����|����");
		//�����
		sTextValue.Format("%d", pWireNode->m_xWirePlace.iSerial);
		lpInfo->AddSubItemText(sTextValue);
		lpInfo->SetControlType(2, GCT_CMB_EDIT);
		lpInfo->SetListItemsStr(2, "1|2|3|4|5|6|");
		//��������
		if (pWireNode->m_xWirePlace.ciTensionType == 1)
			sTextValue = "����";
		else if (pWireNode->m_xWirePlace.ciTensionType == 2)
			sTextValue = "����";
		else
			sTextValue = "";
		lpInfo->AddSubItemText(sTextValue);
		lpInfo->SetControlType(3, GCT_CMB_EDIT);
		lpInfo->SetListItemsStr(3, "����|����");
		//λ��
		if (pWireNode->m_xWirePlace.ciTensionType == 2)
		{
			if (pWireNode->ciWirePosSymbol == 'Q')
				sTextValue = "ǰ��";
			else //if (pWireNode->ciWirePosSymbol == 'H')
				sTextValue = "���";
		}
		else
			sTextValue = "";
		lpInfo->AddSubItemText(sTextValue, FALSE);
		lpInfo->SetControlType(4, GCT_CMB_LIST);
		lpInfo->SetListItemsStr(4, "��|ǰ��|���");
		//����
		sTextValue.Format("%s", (char*)pWireNode->m_sGimName);
		lpInfo->AddSubItemText(sTextValue,TRUE);
		//
		CSuperGridCtrl::CTreeItem* pItem = m_listCtrl.InsertRootItem(lpInfo);
		if (pItem)
			pItem->m_idProp = (DWORD)pWireNode;
	}
	m_listCtrl.Redraw();
}
//
int CInputWirePointDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen = FALSE;
	if (CCallBackDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CInputWirePointDlg::OnMove(int x, int y)
{
	CCallBackDialog::OnMove(x, y);
	if (m_bEnableWindowMoveListen)
	{
		nScrLocationX = x;
		nScrLocationY = y;
	}
}

void CInputWirePointDlg::OnBnImportWireModel()
{
	CXhChar50 filename = theApp.GetLDSDoc()->GetTitle();
	char* pchDotDelimiter = SearchChar(filename, ".", true);
	if (pchDotDelimiter)
		*pchDotDelimiter = 0;
	CFileDialog dlg(TRUE, "wpm", filename,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"�����ҵ���Ϣģ��(*.wpm)|*.wpm|�����ļ�(*.*)|*.*||");
	if (dlg.DoModal() != IDOK)
		return;
	FILE* fp = fopen(dlg.GetPathName(), "rb");
	if (fp == NULL)
	{
		AfxMessageBox("�ļ���ʧ��");
		return;
	}
	UINT uiDocType, uiFileVersion, uiFileLength;
	fread(&uiDocType, 4, 1, fp);
	fread(&uiFileVersion, 4, 1, fp);
	fread(&uiFileLength, 4, 1, fp);
	CHAR_ARRAY filecontents(uiFileLength);
	fread(filecontents, 1, uiFileLength, fp);
	fclose(fp);
	CBuffer buff(filecontents, uiFileLength);
	gxWirePointModel.FromBuffer(buff);
	//UpdateUniWireModelToUIWireNodes();
}

void CInputWirePointDlg::OnBnExportWireModel()
{
	CXhChar50 filename = theApp.GetLDSDoc()->GetTitle();
	char* pchDotDelimiter = SearchChar(filename, ".", true);
	if (pchDotDelimiter)
		*pchDotDelimiter = 0;
	CFileDialog dlg(FALSE, "wpm", filename,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"�����ҵ���Ϣģ��(*.wpm)|*.wpm|�����ļ�(*.*)|*.*||");
	if (dlg.DoModal() != IDOK)
		return;
	FILE* fp = fopen(dlg.GetPathName(), "wb");
	if (fp == NULL)
	{
		AfxMessageBox("�ļ���ʧ��");
		return;
	}
	UpdateUniWireModel();
	UINT uiDocType = 3523490587, uiFileVersion = 1000000, uiFileLength;
	CBuffer buff(0x10000);
	gxWirePointModel.ToBuffer(buff);
	fwrite(&uiDocType, 4, 1, fp);
	fwrite(&uiFileVersion, 4, 1, fp);
	uiFileLength = buff.GetLength();
	fwrite(&uiFileLength, 4, 1, fp);
	fwrite(buff.GetBufferPtr(), 1, uiFileLength, fp);
	fclose(fp);
}

void CInputWirePointDlg::OnOK()
{
	UpdateUniWireModel();
	return CDialog::OnOK();
}
#endif
