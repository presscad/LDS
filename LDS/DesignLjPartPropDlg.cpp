//<LOCALE_TRANSLATE Confirm by hxr/>
// DesignLjPartPropDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "DesignLjPartPropDlg.h"
#include "XhCharString.h"
#include ".\designljpartpropdlg.h"
#include "GlobalFunc.h"
#include "PropertyListOper.h"

#ifdef __COMMON_PART_INC_
//////////////////////////////////////////////////////////////////////////
//��̬�ص�����
static BOOL ModifyLjPartProperty(CPropertyList *pPropList, CPropTreeItem* pItem, CString &valueStr)
{
	CDesignLjPartPropDlg *pLjPartParaDlg = (CDesignLjPartPropDlg*)pPropList->GetParent();
	if (pLjPartParaDlg == NULL)
		return FALSE;
	CDesignLjPartPara* pCurLjPart = &pLjPartParaDlg->m_xLjPartPara;
	CPropertyListOper<CDesignLjPartPara> oper(pPropList, pCurLjPart);
	if (pItem->m_idProp == CDesignLjPartPara::GetPropID("hPart"))
	{
		long hPart = 0;
		sscanf(valueStr, "%X", &hPart);
		CLDSPart *pPart = console.FromPartHandle(hPart);
		if (pPart && pPart->IsLinePart())
		{
			pLjPartParaDlg->m_xLjPartPara.hPart = hPart;
			pLjPartParaDlg->m_xLjPartPara.m_nClassTypeId = pPart->GetClassTypeId();
			//
			CPropTreeItem* pFindItem = pPropList->FindItemByPropId(CDesignLjPartPara::GetPropID("profileInfo"), NULL);
			if (pFindItem)
				pLjPartParaDlg->UpdateProfileProperty(pFindItem);
		}
	}
	else if (pItem->m_idProp == CDesignLjPartPara::GetPropID("iFaceNo"))
		pCurLjPart->iFaceNo = valueStr[0] - '0';
	else if (pItem->m_idProp == CDesignLjPartPara::GetPropID("start0_end1"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if (valueStr.Compare("starting") == 0)
			pCurLjPart->start0_end1 = 0;
		else if (valueStr.Compare("ending") == 0)
#else
		if (valueStr.Compare("ʼ��") == 0)
			pCurLjPart->start0_end1 = 0;
		else if (valueStr.Compare("�ն�") == 0)
#endif
			pCurLjPart->start0_end1 = 1;
		else
			pCurLjPart->start0_end1 = 2;
	}
	else if (pItem->m_idProp == CDesignLjPartPara::GetPropID("angle.cur_wing_x0_y1"))
	{
#ifdef AFX_TARG_ENU_ENGLISH

		if (valueStr.Compare("X Leg") == 0)
#else
		if (valueStr.Compare("X֫") == 0)
#endif
			pCurLjPart->angle.cur_wing_x0_y1 = 0;
		else
			pCurLjPart->angle.cur_wing_x0_y1 = 1;
	}
	else if (pItem->m_idProp == CDesignLjPartPara::GetPropID("angle.bTwoEdge"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if (valueStr.Compare("Yes") == 0)
#else
		if (valueStr.Compare("��") == 0)
#endif
			pCurLjPart->angle.bTwoEdge = TRUE;
		else
			pCurLjPart->angle.bTwoEdge = FALSE;
	}
	else if (pItem->m_idProp == CDesignLjPartPara::GetPropID("wing_space_style"))
	{
		pCurLjPart->angle.cbSpaceFlag.SetWingSpaceStyle(valueStr[0] - '0');
		if (pLjPartParaDlg->UpdateLjRodEdgeSpace(0X02))
			oper.UpdatePropItemValue("wing_space");
		pPropList->SetItemReadOnly(CDesignLjPartPara::GetPropID("wing_space"), valueStr[0] != '0');
	}
	else if (pItem->m_idProp == CDesignLjPartPara::GetPropID("wing_space"))
		pCurLjPart->wing_space = atof(valueStr);
	else if (pItem->m_idProp == CDesignLjPartPara::GetPropID("end_space"))
		pCurLjPart->end_space = atof(valueStr);
	else if (pItem->m_idProp == CDesignLjPartPara::GetPropID("ber_space_style"))
	{
		pCurLjPart->angle.cbSpaceFlag.SetBerSpaceStyle(valueStr[0] - '0');
		if (pLjPartParaDlg->UpdateLjRodEdgeSpace(0X01))
			oper.UpdatePropItemValue("ber_space");
		pPropList->SetItemReadOnly(CDesignLjPartPara::GetPropID("ber_space"), valueStr[0] != '0');
	}
	else if (pItem->m_idProp == CDesignLjPartPara::GetPropID("ber_space"))
		pCurLjPart->ber_space = atof(valueStr);
	return TRUE;
}
static BOOL ButtonClickLjPart(CPropertyList* pPropList, CPropTreeItem* pItem)
{
	CDesignLjPartPropDlg *pLjPartParaDlg = (CDesignLjPartPropDlg*)pPropList->GetParent();
	if (pLjPartParaDlg == NULL)
		return FALSE;
	if (CDesignLjPartPara::GetPropID("hPart") == pItem->m_idProp)
		pLjPartParaDlg->SelectObject(CLS_LINEPART, pItem->m_idProp);
	else if (CDesignLjPartPara::GetPropID("link_ls") == pItem->m_idProp)
		pLjPartParaDlg->SelectObject(CLS_BOLT, pItem->m_idProp);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CDesignLjPartPropDlg �Ի���

IMPLEMENT_DYNAMIC(CDesignLjPartPropDlg, CCallBackDialog)
CDesignLjPartPropDlg::CDesignLjPartPropDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesignLjPartPropDlg::IDD, pParent)
{
	m_pLjPara=NULL;
	m_hCurEditLjPart=0;
	m_hPlate=0;
	m_bNewAddRod = FALSE;
}

CDesignLjPartPropDlg::~CDesignLjPartPropDlg()
{
}

void CDesignLjPartPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_xPropList);
}


BEGIN_MESSAGE_MAP(CDesignLjPartPropDlg, CDialog)
	ON_BN_CLICKED(ID_BTN_CONFIRM, OnBnClickedBtnConfirm)
END_MESSAGE_MAP()


// CDesignLjPartPropDlg ��Ϣ�������
BOOL CDesignLjPartPropDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//��ʼ������ID
	CDesignLjPartPara::InitViewPropHashtable();
	m_xPropList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xPropList.SetDividerScale(0.6);
	m_xPropList.SetModifyValueFunc(ModifyLjPartProperty);
	m_xPropList.SetButtonClickFunc(ButtonClickLjPart);
	if (m_bInernalStart)
	{	//�ڲ�����,�ָ�ѡ����
		FinishSelectObjOper();
		DisplayLjPartProperty();
		CPropTreeItem *pItem = m_xPropList.FindItemByPropId(m_idEventProp, NULL);
		m_xPropList.SetFocus();
		if (pItem)
			m_xPropList.SetCurSel(pItem->m_iIndex);	//����ѡ����
	}
	else
	{
		m_hCurEditLjPart = m_xLjPartPara.hPart;
		CLDSLinePart *pRod = console.FromRodHandle(m_xLjPartPara.hPart);
		if (pRod)
			m_xLjPartPara.m_nClassTypeId = pRod->GetClassTypeId();
		DisplayLjPartProperty();
	}
	return TRUE;  
}
void CDesignLjPartPropDlg::DisplayLjPartProperty()
{
	m_xPropList.CleanTree();
	m_xPropList.m_nObjClassTypeId = 0;
	CPropTreeItem *pPropItem = NULL, *pGroupItem = NULL, *pRootItem = m_xPropList.GetRootItem();
	CPropertyListOper<CDesignLjPartPara> oper(&m_xPropList,&m_xLjPartPara);
	//���Ӹ˼�������Ϣ
	pGroupItem=oper.InsertPropItem(pRootItem, "baseInfo");
	//���
	oper.InsertEditPropItem(pGroupItem, "hPart");
	//������
	oper.InsertCmbListPropItem(pGroupItem, "iFaceNo");
	//���Ӷ�
	oper.InsertCmbListPropItem(pGroupItem, "start0_end1");
	//����֫
	if (m_xLjPartPara.m_nClassTypeId == CLS_LINEANGLE)
		oper.InsertCmbListPropItem(pGroupItem, "angle.cur_wing_x0_y1");
	//������˨
	if (m_bNewAddRod)
	{
		pPropItem=oper.InsertButtonPropItem(pGroupItem, "link_ls");
		pPropItem->m_lpNodeInfo->m_strPropValue = MakeLinkBoltStr();
	}
	//������Ʋ���
	pGroupItem = oper.InsertPropItem(pRootItem, "profileInfo");
	UpdateProfileProperty(pGroupItem);
	m_xPropList.Redraw();
}
void CDesignLjPartPropDlg::UpdateProfileProperty(CPropTreeItem* pProfileInfoItem)
{
	if (pProfileInfoItem == NULL)
		return;
	CPropTreeItem *pPropItem = NULL, *pSonItem = NULL;
	CPropertyListOper<CDesignLjPartPara> oper(&m_xPropList, &m_xLjPartPara);
	//�Ǹ����Ӱ����ζ���
	if (m_xLjPartPara.m_nClassTypeId == CLS_LINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
		pProfileInfoItem->m_lpNodeInfo->m_strPropName = "angle connecting plate profile definition";
	else if (pLjPartPara->m_nClassTypeId == CLS_LINETUBE)
		pProfileInfoItem->m_lpNodeInfo->m_strPropName = "tube connecting plate profile definition";
	else
		pProfileInfoItem->m_lpNodeInfo->m_strPropName = "rod connecting plate profile definition";
#else
		pProfileInfoItem->m_lpNodeInfo->m_strPropName = "�Ǹ����Ӱ����ζ���";
	else if (m_xLjPartPara.m_nClassTypeId == CLS_LINETUBE)
		pProfileInfoItem->m_lpNodeInfo->m_strPropName = "�ֹ����Ӱ����ζ���";
	else
		pProfileInfoItem->m_lpNodeInfo->m_strPropName = "�˼����Ӱ����ζ���";
#endif
	if (m_xLjPartPara.m_nClassTypeId == CLS_LINEANGLE)
	{	//��������
		oper.InsertCmbListPropItem(pProfileInfoItem, "angle.bTwoEdge", "", "", "", -1, TRUE);
		//֫����
		pPropItem = oper.InsertCmbListPropItem(pProfileInfoItem, "wing_space_style", "", "", "", -1, TRUE);
		pPropItem->m_bHideChildren = FALSE;
		//֫����
		pSonItem = oper.InsertEditPropItem(pPropItem, "wing_space", "", "", -1, TRUE);
		pSonItem->SetReadOnly(m_xLjPartPara.angle.cbSpaceFlag.StyleOfWingSpace() != ANGLE_SPACE_FLAG::SPACE_UDF);
	}
	//���߲��
	pPropItem = oper.InsertCmbListPropItem(pProfileInfoItem, "ber_space_style", "", "", "", -1, TRUE);
	pPropItem->m_bHideChildren = FALSE;
	//���߲��
	pSonItem = oper.InsertEditPropItem(pPropItem, "ber_space", "", "", -1, TRUE);
	pSonItem->SetReadOnly(m_xLjPartPara.angle.cbSpaceFlag.StyleOfBerSpace() != ANGLE_SPACE_FLAG::SPACE_UDF);
	if (m_xLjPartPara.m_nClassTypeId == CLS_LINEANGLE)
	{	//��������
		//�˾�
		oper.InsertEditPropItem(pProfileInfoItem, "end_space", "", "", -1, TRUE);
	}
}
CString CDesignLjPartPropDlg::MakeLinkBoltStr()
{
	CString boltStr;
	int i = 0;
	for(CLDSBolt* pBolt=m_xSelLinkLs.GetFirst();pBolt;pBolt=m_xSelLinkLs.GetNext(),i++)
	{
		boltStr += CXhChar16("0X%X", pBolt->handle);
		if (i < m_xSelLinkLs.GetNodeNum() - 1)
			boltStr += ',';
	}
	return boltStr;
}
void CDesignLjPartPropDlg::FinishSelectObjOper()
{
	if (!m_bInernalStart)
		return;
	long hPart = 0;
	if (GetEventPropId() == CDesignLjPartPara::GetPropID("hPart"))
	{
		CLDSPart* pPart = console.FromPartHandle(m_hPickObj);
		if (pPart && pPart->IsLinePart())
			m_xLjPartPara.hPart = pPart->handle;
	}
	else if (GetEventPropId() == CDesignLjPartPara::GetPropID("link_ls"))
	{
		CLDSBolt* pBolt = (CLDSBolt*)console.FromPartHandle(m_hPickObj, CLS_BOLT);
		if (pBolt && m_xSelLinkLs.FromHandle(pBolt->handle) == NULL)
			m_xSelLinkLs.append(pBolt);
	}
}
BOOL CDesignLjPartPropDlg::UpdateLjRodEdgeSpace(BYTE cbEdgeFlag /*= 0x03*/)
{
	CLDSPlate* pPlate = (CLDSPlate*)Ta.FromPartHandle(m_hPlate, CLS_PLATE);
	if (pPlate == NULL)
		return FALSE;
	return pPlate->UpdateLjRodEdgeSpace(&m_xLjPartPara, cbEdgeFlag);
}
void CDesignLjPartPropDlg::OnOK()
{
	
}
void CDesignLjPartPropDlg::OnBnClickedBtnConfirm()
{
	long h=m_xLjPartPara.hPart;
	CLDSLinePart *pRod=console.FromRodHandle(h);
	if( pRod==NULL||(pRod->GetClassTypeId()!=CLS_LINETUBE&&pRod->GetClassTypeId()!=CLS_LINEANGLE&&
		pRod->GetClassTypeId()!=CLS_GROUPLINEANGLE&&pRod->GetClassTypeId()!=CLS_LINEFLAT))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("specified rods do not exist��");
#else
		AfxMessageBox("ָ���˼������ڣ�");
#endif
		return;
	}
	for(CDesignLjPartPara *pLjPara=m_pLjPara->partList.GetFirst();pLjPara;pLjPara=m_pLjPara->partList.GetNext())
	{	//�Ѵ��ڵĸ˼������ظ����
		if(m_hCurEditLjPart>0x20&&m_hCurEditLjPart==pLjPara->hPart)
			continue;
		else if(pLjPara->hPart==h)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("specified rods exist��");
#else
			AfxMessageBox("��ָ���˼��Ѵ��ڣ�");
#endif
			return;
		}
	}
	CDialog::OnOK();
}
#endif