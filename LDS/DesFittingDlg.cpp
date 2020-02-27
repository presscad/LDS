//DesHangFittingDlg.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "LDS.h"
#include "MainFrm.h"
#include "DesFittingDlg.h"
#include "PropertyListOper.h"
#include "GlobalFunc.h"
#include "UI.h"
#include "./StdPartSolid/Fitting.h"
//////////////////////////////////////////////////////////////////////////
//����������
static BOOL FireCallBackCommand(CWnd* pCallWnd,UINT message,WPARAM wParam)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please Preview part(Press enter or esc key to end):","");
#else 
	pCmdLine->FillCmdLine("��תԤ��������ESC����س��������߰�Ի���:","");
#endif
	CString cmdStr;
	g_pSolidSet->SetOperType(OPER_ROTATE);
	pCmdLine->GetStrFromCmdLine(cmdStr);
	return TRUE;
}
static BOOL ModifyProperty(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CDesFittingDlg *pDlg=(CDesFittingDlg*)pPropList->GetParent();
	if(pDlg==NULL)
		return FALSE;
	CPropertyListOper<CDesFittingDlg> oper(pPropList,pDlg);
	BOOL bReDesign=FALSE;
	if(pItem->m_idProp==CDesFittingDlg::GetPropID("iSeg"))
		pDlg->m_iSeg=SEGI(valueStr.GetBuffer());
	else if(pItem->m_idProp==CDesFittingDlg::GetPropID("type"))
	{
		BYTE ciType=valueStr[0]-'0';
		if (ciType == 0)
			pDlg->m_ciType = CLDSFitting::TYPE_EB;
		else if (ciType == 1)
			pDlg->m_ciType = CLDSFitting::TYPE_GD;
		else if (ciType == 2)
			pDlg->m_ciType = CLDSFitting::TYPE_UR;
		else if (ciType == 3)
			pDlg->m_ciType = CLDSFitting::TYPE_US;
		else if (ciType == 4)
			pDlg->m_ciType = CLDSFitting::TYPE_UB;
		//
		pDlg->InitFittingPara();
		bReDesign=TRUE;
		//
		CPropTreeItem* pFindItem=pPropList->FindItemByPropId(CDesFittingDlg::GetPropID("SizeCode"),NULL);
		if(pFindItem)
		{
			pFindItem->m_lpNodeInfo->m_cmbItems=MakeFittingSetString(ciType);
			oper.UpdatePropItemValue("SizeCode");
		}
		//
		pDlg->DisplaySubPropList();
		pDlg->UpdateSketchMap();
		bReDesign=TRUE;
	}
	else if(pItem->m_idProp==CDesFittingDlg::GetPropID("SizeCode"))
	{
		pDlg->xPreviewFitting.sizespec.Copy(valueStr);
		pDlg->xPreviewFitting.MatchUring();
		oper.UpdatePropItemValue("URSizeCode");
		pDlg->DisplaySubPropList();
		bReDesign=TRUE;
	}
	else if (pItem->m_idProp == CDesFittingDlg::GetPropID("URSizeCode"))
	{
		pDlg->xPreviewFitting.sizespecUR.Copy(valueStr);
		UI::UpdatePartSolid(&pDlg->xPreviewFitting);
	}
	else if(pItem->m_idProp==CDesFittingDlg::GetPropID("SwingAngle"))
	{
		pDlg->xPreviewFitting.m_iLiveSwingAngle=atoi(valueStr);
		bReDesign=TRUE;
	}
	else if(pItem->m_idProp==CDesFittingDlg::GetPropID("MinSwingAngle"))
	{
		pDlg->xPreviewFitting.m_siMinSwingAngle=atoi(valueStr);
		UI::UpdatePartSolid(&pDlg->xPreviewFitting);
	}
	else if(pItem->m_idProp==CDesFittingDlg::GetPropID("MaxSwingAngle"))
	{
		pDlg->xPreviewFitting.m_siMaxSwingAngle=atoi(valueStr);
		UI::UpdatePartSolid(&pDlg->xPreviewFitting);
	}
	else if(pItem->m_idProp==CDesFittingDlg::GetPropID("URSwingAngle"))
	{
		pDlg->xPreviewFitting.m_iURLiveSwingAngle=atoi(valueStr);
		UI::UpdatePartSolid(&pDlg->xPreviewFitting);
	}
	else if(pItem->m_idProp==CDesFittingDlg::GetPropID("MinURSwingAngle"))
	{
		pDlg->xPreviewFitting.m_siMinURSwingAngle=atoi(valueStr);
		UI::UpdatePartSolid(&pDlg->xPreviewFitting);
	}
	else if(pItem->m_idProp==CDesFittingDlg::GetPropID("MaxURSwingAngle"))
	{
		pDlg->xPreviewFitting.m_siMaxURSwingAngle=atoi(valueStr);
		UI::UpdatePartSolid(&pDlg->xPreviewFitting);
	}
	if(bReDesign)
		pDlg->CreateFitting(&pDlg->xPreviewFitting);
	return TRUE;
}
static BOOL ButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CDesFittingDlg *pDlg=(CDesFittingDlg*)pPropList->GetParent();
	if(pDlg==NULL)
		return FALSE;
	if(CDesFittingDlg::GetPropID("hHole1")==pItem->m_idProp||CDesFittingDlg::GetPropID("hHole2")==pItem->m_idProp)
		pDlg->SelectObject(CLS_HOLEWALL,pItem->m_idProp);
	return TRUE;
}
static BOOL FireHelpStr(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	if(pPropList->GetDlgCtrlID()!=IDC_LIST_BOX)
		return FALSE;	//�������������Բ���ӦFirePropHelpPrompt()����
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CDesFittingDlg *pDlg=(CDesFittingDlg*)pPropList->GetParent();
	g_pSolidDraw->DelCS(ISolidDraw::WORK_CS);
	pView->Invalidate(FALSE);
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// CDesFittingDlg �Ի���
IMPLEMENT_DYNAMIC(CDesFittingDlg, CCallBackDialog)
int CDesFittingDlg::nScrLocationX=0;
int CDesFittingDlg::nScrLocationY=0;
CDesFittingDlg::CDesFittingDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesFittingDlg::IDD, pParent)
{
	m_pFitting=NULL;
	m_pHole1=m_pHole2=NULL;
	m_pResidePart1=m_pResidePart2=NULL;
	m_bNewFitting=TRUE;
	xPreviewFitting.handle=ISolidDraw::BLK_LOCATE_CS+1;	//��ʱ������, ����Ҫ�ܿ�ϵͳ�ض�����ϵ��ID
	xPreviewFitting.SetBelongModel(&Ta);
}

CDesFittingDlg::~CDesFittingDlg()
{
}

void CDesFittingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Control(pDX, IDC_LIST_BOX_SUB, m_xSubPropList);
}


BEGIN_MESSAGE_MAP(CDesFittingDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_MOVE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_CONFIRM, OnConfirm)
	ON_BN_CLICKED(IDC_BTN_MODEL_VIEW_OPER, OnBnModelViewOper)
END_MESSAGE_MAP()


// CDesFittingDlg ��Ϣ�������
BOOL CDesFittingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.SetDividerScale(0.5);
	m_xSubPropList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xSubPropList.SetDividerScale(0.5);
	InitPropHashtable();
	SetCallBackFunc(FireCallBackCommand);
	if(m_bInernalStart)	//�ڲ�����
	{
		FinishSelectObjOper();	//���ѡ�����ĺ�������
		DisplayPropList();
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		m_propList.SetFocus();	//��������ý���
		if(pItem)
			m_propList.SetCurSel(pItem->m_iIndex);	//����ѡ����
		DisplaySubPropList(NULL);
	}
	else
	{
		if(!m_bNewFitting)
		{
			if(m_pFitting==NULL)
			{
				AfxMessageBox("δѡ����ȷ�Ľ�ߣ�");
				return FALSE;
			}
		}
		if(m_bNewFitting&&m_pFitting==NULL)
		{
			m_pFitting=&xPreviewFitting;
			InitFittingPara();
			CreateFitting(m_pFitting);
		}
		else if (m_pFitting!=NULL)
		{
			xPreviewFitting.sizespec=m_pFitting->sizespec;
			xPreviewFitting.sizespecUR=m_pFitting->sizespecUR;
			//һ���ҽӽ�ߵ�ǰ�ڶ���ֵ,��
			xPreviewFitting.m_iLiveSwingAngle=m_pFitting->m_iLiveSwingAngle;
			//һ���ҽӽ�ߵİڶ��Ƿ�Χ,��
			xPreviewFitting.m_siMinSwingAngle=m_pFitting->m_siMinSwingAngle;
			xPreviewFitting.m_siMaxSwingAngle=m_pFitting->m_siMaxSwingAngle;
			//��һ���ҽӽ�����Ӻ���ڶ�U����˨
			xPreviewFitting.m_iURLiveSwingAngle=m_pFitting->m_iURLiveSwingAngle;
			//U�ͻ���߰ڶ��Ƿ�Χ
			xPreviewFitting.m_siMinURSwingAngle=m_pFitting->m_siMinURSwingAngle;
			xPreviewFitting.m_siMaxURSwingAngle=m_pFitting->m_siMaxURSwingAngle;
		}
		DisplayPropList();
		DisplaySubPropList(NULL);
	}
	UpdateSketchMap();
	//�ƶ��Ի������Ͻ�
	CRect rect;
	GetWindowRect(rect);
	int width=rect.Width();
	int height=rect.Height();
	rect.left = nScrLocationX;
	rect.top = nScrLocationY;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;
	MoveWindow(rect,TRUE);
	m_bEnableWindowMoveListen=TRUE;
	return TRUE;
}
IMPLEMENT_PROP_FUNC(CDesFittingDlg);
const DWORD HASHTABLESIZE = 500;
void CDesFittingDlg::InitPropHashtable()
{
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(50);
	int id = 1;
	AddPropItem("preview",PROPLIST_ITEM(id++,"Ԥ������","֧��Ԥ��","Ԥ��"));
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"������Ϣ"));
	AddPropItem("iSeg",PROPLIST_ITEM(id++,"�κ�"));
	AddPropItem("type",PROPLIST_ITEM(id++,"�������","","0.EB�Ұ�|1.GD�Ұ�|2.U�ͻ�|3.U����˨|4.U�͹Ұ�"));
	AddPropItem("SizeCode",PROPLIST_ITEM(id++,"�Ұ��ͺ�",""));
	//
	AddPropItem("RelaInfo",PROPLIST_ITEM(id++,"������Ϣ"));
	AddPropItem("hHole1",PROPLIST_ITEM(id++,"1�Źҿ�","������ڹҿ�1"));
	AddPropItem("hPart1",PROPLIST_ITEM(id++,"���ڹ���","�ҿ����ڹ���1"));
	AddPropItem("hHole2",PROPLIST_ITEM(id++,"2�Źҿ�","������ڹҿ�2"));
	AddPropItem("hPart2",PROPLIST_ITEM(id++,"���ڹ���","�ҿ����ڹ���2"));
	//
	AddPropItem("location",PROPLIST_ITEM(id++,"װ�䷽λ"));
	AddPropItem("SwingAngle",PROPLIST_ITEM(id++,"����߰ڶ��ǡ�"));
	AddPropItem("MinSwingAngle",PROPLIST_ITEM(id++,"�ڶ���ʼ�ǡ�"));
	AddPropItem("MaxSwingAngle",PROPLIST_ITEM(id++,"�ڶ���ֹ�ǡ�"));
	AddPropItem("URSizeCode",PROPLIST_ITEM(id++,"U�λ��ͺ�",""));
	AddPropItem("URSwingAngle",PROPLIST_ITEM(id++,"U�λ��ڶ��ǡ�"));
	AddPropItem("MinURSwingAngle",PROPLIST_ITEM(id++,"�ڶ���ʼ�ǡ�"));
	AddPropItem("MaxURSwingAngle",PROPLIST_ITEM(id++,"�ڶ���ֹ�ǡ�"));
	//
	AddPropItem("profile",PROPLIST_ITEM(id++,"�������"));
	AddPropItem("eb.Thick",PROPLIST_ITEM(id++,"�Ұ���"));
	AddPropItem("eb.ThroughS",PROPLIST_ITEM(id++,"Ԥ��ͨ��S"));
	AddPropItem("eb.TrunnionD",PROPLIST_ITEM(id++,"����ֱ��M"));
	AddPropItem("eb.HoleD",PROPLIST_ITEM(id++,"�ҿ�ֱ����"));
	AddPropItem("eb.BoardL",PROPLIST_ITEM(id++,"�Ұ���L"));
	AddPropItem("eb.BoardH",PROPLIST_ITEM(id++,"�ҿ׵����ᴹ��H"));
	AddPropItem("eb.ChinR",PROPLIST_ITEM(id++,"Բ���뾶R"));
	AddPropItem("gd.Thick",PROPLIST_ITEM(id++,"�Ұ���"));
	AddPropItem("gd.ThroughS",PROPLIST_ITEM(id++,"Ԥ��ͨ��S"));
	AddPropItem("gd.TrunnionD",PROPLIST_ITEM(id++,"����ֱ��M"));
	AddPropItem("gd.TrunnionL",PROPLIST_ITEM(id++,"�����ܳ�L"));
	AddPropItem("gd.BoardL",PROPLIST_ITEM(id++,"�Ұ���L1"));
	AddPropItem("gd.HoleD",PROPLIST_ITEM(id++,"�ҿ�ֱ����"));
	AddPropItem("gd.BoardH",PROPLIST_ITEM(id++,"��Բ�����״���H"));
	AddPropItem("gd.BoardH1",PROPLIST_ITEM(id++,"��Բ�����ᴹ��H1"));
	AddPropItem("gd.BoardH2",PROPLIST_ITEM(id++,"�ҿ׵��ᴹ��H2"));
}
int CDesFittingDlg::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	FITTING_EB* pEB=QueryFitting<FITTING_EB>(xPreviewFitting.sizespec);
	FITTING_GD* pGD=QueryFitting<FITTING_GD>(xPreviewFitting.sizespec);
	CXhChar100 sText;
	if(GetPropID("iSeg")==id)
		sText.Copy(m_iSeg.ToString());
	else if(GetPropID("type")==id)
	{
		if(m_ciType==CLDSFitting::TYPE_EB)
			sText.Copy("0.EB�Ұ�");
		else if(m_ciType==CLDSFitting::TYPE_GD)
			sText.Copy("1.GD�Ұ�");
		else  if (m_ciType==CLDSFitting::TYPE_UR)
			sText.Copy("2.U�ͻ�");
		else if(m_ciType==CLDSFitting::TYPE_US)
			sText.Copy("3.U����˿");
		else if(m_ciType==CLDSFitting::TYPE_UB)
			sText.Copy("4.U�͹Ұ�");
	}
	else if (GetPropID("SizeCode")==id)
		sText.Copy(xPreviewFitting.sizespec);
	else if (GetPropID("URSizeCode")==id)
		sText.Copy(xPreviewFitting.sizespecUR);
	else if(GetPropID("SwingAngle")==id)
		sText.Printf("%d",xPreviewFitting.m_iLiveSwingAngle);
	else if(GetPropID("MinSwingAngle")==id)
		sText.Printf("%d",xPreviewFitting.m_siMinSwingAngle);
	else if(GetPropID("MaxSwingAngle")==id)
		sText.Printf("%d",xPreviewFitting.m_siMaxSwingAngle);
	else if(GetPropID("URSwingAngle")==id)
		sText.Printf("%d",xPreviewFitting.m_iURLiveSwingAngle);
	else if(GetPropID("MinURSwingAngle")==id)
		sText.Printf("%d",xPreviewFitting.m_siMinURSwingAngle);
	else if(GetPropID("MaxURSwingAngle")==id)
		sText.Printf("%d",xPreviewFitting.m_siMaxURSwingAngle);
	else if(GetPropID("hHole1")==id)
	{
		if(m_pHole1)
			sText.Printf("0X%X",m_pHole1->handle);
		else
			sText.Copy("0X0");
	}
	else if(GetPropID("hHole2")==id)
	{
		if(m_pHole2)
			sText.Printf("0X%X",m_pHole2->handle);
		else
			sText.Copy("0X0");
	}
	else if(GetPropID("hPart1")==id)
	{
		if(m_pResidePart1)
			sText.Printf("0X%X",m_pResidePart1->handle);
		else
			sText.Copy("0X0");
	}
	else if(GetPropID("hPart2")==id)
	{
		if(m_pResidePart2)
			sText.Printf("0X%X",m_pResidePart2->handle);
		else
			sText.Copy("0X0");
	}
	else if(GetPropID("eb.Thick")==id && pEB)
		sText.Printf("%d",pEB->siThick);
	else if(GetPropID("eb.ThroughS")==id && pEB)
		sText.Printf("%d",pEB->siThroughThick);
	else if(GetPropID("eb.TrunnionD")==id && pEB)
	{
		sText.Printf("%f",pEB->dfTrunnionD);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("eb.HoleD")==id && pEB)
	{
		sText.Printf("%f",pEB->dfHoleD);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("eb.BoardL")==id && pEB)
	{
		sText.Printf("%f",pEB->dfBoardL);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("eb.BoardH")==id && pEB)
	{
		sText.Printf("%f",pEB->dfBoardH);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("eb.ChinR")==id && pEB)
	{
		sText.Printf("%f",pEB->dfChinR);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("gd.Thick")==id && pGD)
		sText.Printf("%d",pGD->siThick);
	else if(GetPropID("gd.ThroughS")==id && pGD)
		sText.Printf("%d",pGD->siThroughThick);
	else if(GetPropID("gd.TrunnionD")==id && pGD)
	{
		sText.Printf("%f",pGD->dfTrunnionD);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("gd.TrunnionL")==id && pGD)
	{
		sText.Printf("%f",pGD->dfTrunnionL);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("gd.BoardL")==id && pGD)
	{
		sText.Printf("%f",pGD->dfBoardL);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("gd.HoleD")==id && pGD)
	{
		sText.Printf("%f",pGD->dfHoleD);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("gd.BoardH")==id && pGD)
	{
		sText.Printf("%f",pGD->dfBoardH);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("gd.BoardH1")==id && pGD)
	{
		sText.Printf("%f",pGD->dfBoardH1);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("gd.BoardH2")==id && pGD)
	{
		sText.Printf("%f",pGD->dfBoardH2);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
void CDesFittingDlg::DisplayPropList()
{
	m_propList.CleanTree();
	m_propList.SetModifyValueFunc(ModifyProperty);
	m_propList.SetButtonClickFunc(ButtonClick);
	m_propList.SetPropHelpPromptFunc(FireHelpStr);
	CPropertyListOper<CDesFittingDlg> oper(&m_propList,this);
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pRoot=m_propList.GetRootItem();
	//������Ϣ
	pGroupItem=oper.InsertPropItem(pRoot,"basicInfo");
	oper.InsertEditPropItem(pGroupItem,"iSeg");
	oper.InsertCmbListPropItem(pGroupItem,"type");
	oper.InsertCmbListPropItem(pGroupItem,"SizeCode",MakeFittingSetString(m_ciType));
	//������Ϣ
	pGroupItem=oper.InsertPropItem(pRoot,"RelaInfo");
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"hHole1");
	oper.InsertButtonPropItem(pPropItem,"hPart1");
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"hHole2");
	oper.InsertButtonPropItem(pPropItem,"hPart2");
	//
	pGroupItem=oper.InsertPropItem(pRoot,"location");
	if (m_ciType != CLDSFitting::TYPE_UR&&m_ciType != CLDSFitting::TYPE_US)
	{
		pPropItem = oper.InsertEditPropItem(pGroupItem, "SwingAngle");
		pPropItem->m_bHideChildren = FALSE;
		oper.InsertEditPropItem(pPropItem, "MinSwingAngle");
		oper.InsertEditPropItem(pPropItem, "MaxSwingAngle");
	}
	if(m_ciType == CLDSFitting::TYPE_EB|| m_ciType == CLDSFitting::TYPE_GD)
	{
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"URSizeCode",MakeFittingSetString(CLDSFitting::TYPE_UR));
		pPropItem=oper.InsertEditPropItem(pGroupItem,"URSwingAngle");
		pPropItem->m_bHideChildren = FALSE;
		oper.InsertEditPropItem(pPropItem,"MinURSwingAngle");
		oper.InsertEditPropItem(pPropItem,"MaxURSwingAngle");
	}
	m_propList.Redraw();
}
void CDesFittingDlg::DisplaySubPropList(CPropTreeItem *pItem/*=NULL*/)
{
	m_xSubPropList.CleanTree();
	m_xSubPropList.SetModifyValueFunc(ModifyProperty);
	m_xSubPropList.SetButtonClickFunc(ButtonClick);
	//
	CPropertyListOper<CDesFittingDlg> oper(&m_xSubPropList,this);
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pRoot=m_xSubPropList.GetRootItem();
	pGroupItem=oper.InsertPropItem(pRoot,"profile");
	if(m_ciType==0)
	{	//EB
		pPropItem=oper.InsertEditPropItem(pGroupItem,"eb.Thick");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"eb.ThroughS");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"eb.TrunnionD");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"eb.HoleD");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"eb.BoardL");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"eb.BoardH");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"eb.ChinR");
		pPropItem->SetReadOnly();
	}
	else if(m_ciType==1)
	{	//GD
		pPropItem=oper.InsertEditPropItem(pGroupItem,"gd.Thick");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"gd.ThroughS");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"gd.TrunnionD");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"gd.TrunnionL");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"gd.HoleD");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"gd.BoardL");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"gd.BoardH");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"gd.BoardH1");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"gd.BoardH2");
		pPropItem->SetReadOnly();
	}
	else
	{

	}
	m_xSubPropList.Redraw();
}
void CDesFittingDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	CLDSBolt* pBolt=NULL;
	if(GetEventPropId()==GetPropID("hHole1"))
	{
		if(m_hPickObj<0)
		{
			OBJECT_HANDLER handler;
			g_pSolidDraw->GetSolidObject(m_hPickObj,&handler);
			pBolt=(CLDSBolt*)console.FromPartHandle(handler.hiberid.masterId,CLS_BOLT);
			if(pBolt)
			{
				m_pHole1=pBolt;
				m_pResidePart1=console.FromPartHandle(handler.hiberid.HiberUpId(2));
			}
		}
		else
		{
			pBolt=(CLDSBolt*)console.FromPartHandle(m_hPickObj,CLS_BOLT);
			if(pBolt)
			{
				m_pHole1=pBolt;
				m_pResidePart1=console.FromPartHandle(m_pHole1->des_base_pos.hPart);
			}
		}
	}
	else if(GetEventPropId()==GetPropID("hHole2"))
	{
		if(m_hPickObj<0)
		{
			OBJECT_HANDLER handler;
			g_pSolidDraw->GetSolidObject(m_hPickObj,&handler);
			pBolt=(CLDSBolt*)console.FromPartHandle(handler.hiberid.masterId,CLS_BOLT);
			if(pBolt)
			{
				m_pHole2=pBolt;
				m_pResidePart2=console.FromPartHandle(handler.hiberid.HiberUpId(2));
			}
		}
		else
		{
			pBolt=(CLDSBolt*)console.FromPartHandle(m_hPickObj,CLS_BOLT);
			if(pBolt)
			{
				m_pHole2=pBolt;
				m_pResidePart2=console.FromPartHandle(m_pHole2->des_base_pos.hPart);
			}
		}
	}
	CreateFitting(m_pFitting);
}
void CDesFittingDlg::UpdateSketchMap()
{
	m_xCurPlateBmp.Detach();
	if (m_ciType == CLDSFitting::TYPE_EB)
		m_xCurPlateBmp.LoadBitmap(IDB_BMP_EB);
	else if (m_ciType == CLDSFitting::TYPE_GD)
		m_xCurPlateBmp.LoadBitmap(IDB_BMP_GD);
	else if (m_ciType == CLDSFitting::TYPE_US)
		m_xCurPlateBmp.LoadBitmap(IDB_BMP_GD);
	else if (m_ciType == CLDSFitting::TYPE_UB)
		m_xCurPlateBmp.LoadBitmap(IDB_BMP_GD);
	//
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_TYPE);
	pPic->SetBitmap(m_xCurPlateBmp);
}
//
void CDesFittingDlg::InitFittingPara()
{
	if(!m_bNewFitting)
		return;
	if(m_pResidePart1)
		m_iSeg=m_pResidePart1->iSeg;
	if(m_ciType== CLDSFitting::TYPE_EB)
	{
		IFittingLibrary<FITTING_EB>* pxarrEBLib=FittingLibrary<FITTING_EB>();
		xPreviewFitting.sizespec.Copy(pxarrEBLib->pxSizeTbl[0].szSizeCode);
	}
	else if(m_ciType== CLDSFitting::TYPE_GD)
	{
		IFittingLibrary<FITTING_GD>* pxarrGDLib=FittingLibrary<FITTING_GD>();
		xPreviewFitting.sizespec.Copy(pxarrGDLib->pxSizeTbl[0].szSizeCode);
	}
	else if (m_ciType == CLDSFitting::TYPE_US)
	{
		IFittingLibrary<FITTING_US>* pxarrUSLib = FittingLibrary<FITTING_US>();
		xPreviewFitting.sizespec.Copy(pxarrUSLib->pxSizeTbl[0].szSizeCode);
	}
	else if (m_ciType == CLDSFitting::TYPE_UB)
	{
		IFittingLibrary<FITTING_UB>* pxarrUBLib = FittingLibrary<FITTING_UB>();
		xPreviewFitting.sizespec.Copy(pxarrUBLib->pxSizeTbl[0].szSizeCode);
	}
	xPreviewFitting.MatchUring();
}
CLDSFitting* CDesFittingDlg::CreateFitting(CLDSFitting* pToExist/*=NULL*/)
{
	CLDSFitting *pFitting=NULL;
	if(pToExist==NULL&&(m_pFitting==NULL||m_pFitting==&xPreviewFitting))
		pFitting=m_pFitting=(CLDSFitting*)console.AppendPart(CLS_FITTINGS);
	else if(pToExist!=NULL)
		pFitting=pToExist;
	else
		pFitting=m_pFitting=&xPreviewFitting;
	//
	if (m_pResidePart1)
	{
		pFitting->cfgword = m_pResidePart1->cfgword;
		pFitting->cMaterial = m_pResidePart1->cMaterial;
		pFitting->SetLayer(m_pResidePart1->layer());
	}
	pFitting->iSeg = m_iSeg;
	pFitting->m_ciFittingType=m_ciType;
	pFitting->sizespec=xPreviewFitting.sizespec;
	pFitting->m_iLiveSwingAngle=xPreviewFitting.m_iLiveSwingAngle;
	pFitting->m_siMinSwingAngle=xPreviewFitting.m_siMinSwingAngle;
	pFitting->m_siMaxSwingAngle=xPreviewFitting.m_siMaxSwingAngle;
	pFitting->sizespecUR=xPreviewFitting.sizespecUR;
	pFitting->m_iURLiveSwingAngle=xPreviewFitting.m_iURLiveSwingAngle;
	pFitting->m_siMinURSwingAngle=xPreviewFitting.m_siMinURSwingAngle;
	pFitting->m_siMaxURSwingAngle=xPreviewFitting.m_siMaxURSwingAngle;
	if(m_pHole1)
		pFitting->holes[0].hBolt=m_pHole1->handle;
	if(m_pResidePart1)
		pFitting->holes[0].hResidePart=m_pResidePart1->handle;
	if(m_pHole2)
		pFitting->holes[1].hBolt=m_pHole2->handle;
	if(m_pResidePart2)
		pFitting->holes[1].hResidePart=m_pResidePart2->handle;
	pFitting->DesignFitting();
	pFitting->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pFitting->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return pFitting;
}
//��Ϣ��Ӧ�¼�
void CDesFittingDlg::OnOK()
{

}

void CDesFittingDlg::OnCancel()
{
	g_pSolidDraw->DelEnt(xPreviewFitting.handle);
	if(m_bNewFitting&&m_pFitting)
	{
		g_pSolidDraw->DelCS(m_pFitting->handle);
		g_pSolidDraw->DelEnt(m_pFitting->handle);
		console.DispPartSet.DeleteNode(m_pFitting->handle);
		console.DeletePart(m_pFitting->handle);
		g_pSolidDraw->Draw();
	}
	else if(!m_bNewFitting&&m_pFitting)
	{	//�鿴���߰�����رնԻ���֮������ѡ���߰�
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pFitting->handle);
	}
	CDialog::OnCancel();
}

void CDesFittingDlg::OnClose()
{
	g_pSolidDraw->DelEnt(xPreviewFitting.handle);
	if(m_bNewFitting&&m_pFitting)
	{
		g_pSolidDraw->DelCS(m_pFitting->handle);
		g_pSolidDraw->DelEnt(m_pFitting->handle);
		console.DispPartSet.DeleteNode(m_pFitting->handle);
		console.DeletePart(m_pFitting->handle);
		g_pSolidDraw->Draw();
	}
	else if(!m_bNewFitting&&m_pFitting)
	{	//�鿴���߰�����رնԻ���֮������ѡ���߰�
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pFitting->handle);
	}
	CDialog::OnClose();
}

int CDesFittingDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen=FALSE;
	if (CCallBackDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CDesFittingDlg::OnMove(int x, int y)
{
	CCallBackDialog::OnMove(x, y);

	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}

void CDesFittingDlg::OnConfirm()
{
	g_pSolidDraw->DelEnt(xPreviewFitting.handle);
	if(m_pFitting&&!m_bNewFitting)
	{	//�鿴���߰�����رնԻ���֮������ѡ���߰�
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pFitting->handle);
	}
	if(m_pFitting==&xPreviewFitting)
		m_pFitting=NULL;
	CDialog::OnOK();
}

void CDesFittingDlg::OnBnModelViewOper()
{
	ExecuteCommand(this,GetPropID("preview"));
}
