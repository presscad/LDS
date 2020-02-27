// ArcLiftDlg.cpp : ʵ���ļ�
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "ArcLiftDlg.h"
#include "XhCharString.h"
#include "LmaDlg.h"
#include ".\arcliftdlg.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "CmdLineDlg.h"
#include "MainFrm.h"

// CArcLiftDlg �Ի���

#ifdef __ANGLE_PART_INC_
int CArcLiftDlg::nScrLocationX=(int)(GetSystemMetrics(SM_CXSCREEN)*0.5);
int CArcLiftDlg::nScrLocationY=(int)(GetSystemMetrics(SM_CXSCREEN)*0.2);
IMPLEMENT_DYNAMIC(CArcLiftDlg, CDialog)
CArcLiftDlg::CArcLiftDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CArcLiftDlg::IDD, pParent)
{
	m_bEnableWindowMoveListen=false;
}

CArcLiftDlg::~CArcLiftDlg()
{
}

void CArcLiftDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
}


BEGIN_MESSAGE_MAP(CArcLiftDlg, CDialog)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BTN_CONFIRM, OnBnClickedBtnConfirm)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CArcLiftDlg ��Ϣ�������
static BOOL ModifyArcLiftProperty(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	CArcLiftDlg *pArcLiftDlg=(CArcLiftDlg*)pPropList->GetParent();
	if(pArcLiftDlg==NULL)
		return FALSE;
	BOOL bContinue=FALSE;
	if(CArcLiftDlg::GetPropID("name")==pItem->m_idProp)
		strncpy(pArcLiftDlg->m_xArcLift.name,valueStr,50);
	else if(CArcLiftDlg::GetPropID("m_fLiftHeight")==pItem->m_idProp)
		pArcLiftDlg->m_xArcLift.m_fLiftHeight=atof(valueStr);
	else if(CArcLiftDlg::GetPropID("m_ciLiftType")==pItem->m_idProp)
		pArcLiftDlg->m_xArcLift.m_ciLiftType = valueStr[0]-'0';
	else if(CArcLiftDlg::GetPropID("m_cRotAxis")==pItem->m_idProp)
		pArcLiftDlg->m_xArcLift.m_cRotAxis = valueStr[0];
	else if(CArcLiftDlg::GetPropID("m_xRotOrg.x")==pItem->m_idProp)
		pArcLiftDlg->m_xArcLift.m_xRotOrg.x=atof(valueStr);
	else if(CArcLiftDlg::GetPropID("m_xRotOrg.y")==pItem->m_idProp)
		pArcLiftDlg->m_xArcLift.m_xRotOrg.y=atof(valueStr);
	else if(CArcLiftDlg::GetPropID("m_xRotOrg.z")==pItem->m_idProp)
		pArcLiftDlg->m_xArcLift.m_xRotOrg.z=atof(valueStr);
	else if(CArcLiftDlg::GetPropID("m_xDatumLiftPos.x")==pItem->m_idProp)
		pArcLiftDlg->m_xArcLift.m_xDatumLiftPos.x=atof(valueStr);
	else if(CArcLiftDlg::GetPropID("m_xDatumLiftPos.y")==pItem->m_idProp)
		pArcLiftDlg->m_xArcLift.m_xDatumLiftPos.y=atof(valueStr);
	else if(CArcLiftDlg::GetPropID("m_xDatumLiftPos.z")==pItem->m_idProp)
		pArcLiftDlg->m_xArcLift.m_xDatumLiftPos.z=atof(valueStr);
	return TRUE;
} 
static BOOL ButtonClickPointItem(CPropertyList *pPropList,GEPOINT &point,
								 char *sIdProp,char *sCaption)
{
	CXhChar100 sText;
	CReportVectorDlg pointDlg;
	pointDlg.m_sCaption = sCaption;
	pointDlg.m_fVectorX = point.x;
	pointDlg.m_fVectorY = point.y;
	pointDlg.m_fVectorZ = point.z;
	if(pointDlg.DoModal()==IDOK)
	{
		sText.Printf("%f",pointDlg.m_fVectorX);
		SimplifiedNumString(sText);
		CXhChar100 sSonProp("%s.x",sIdProp);
		pPropList->SetItemPropValue(CArcLiftDlg::GetPropID(sSonProp),CString(sText));
		sText.Printf("%f",pointDlg.m_fVectorY);
		SimplifiedNumString(sText);
		sSonProp.Printf("%s.y",sIdProp);
		pPropList->SetItemPropValue(CArcLiftDlg::GetPropID(sSonProp),CString(sText));
		sText.Printf("%f",pointDlg.m_fVectorZ);
		SimplifiedNumString(sText);
		sSonProp.Printf("%s.z",sIdProp);
		pPropList->SetItemPropValue(CArcLiftDlg::GetPropID(sSonProp),CString(sText));
		point.Set(pointDlg.m_fVectorX,pointDlg.m_fVectorY,pointDlg.m_fVectorZ);
	}
	return TRUE;
}

static BOOL ArcLiftPopMenuClick(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu)
{
	if(pPropList==NULL || pItem==NULL)
		return FALSE;
	CArcLiftDlg *pArcLiftDlg=(CArcLiftDlg*)pPropList->GetParent();
	if(pArcLiftDlg==NULL)
		return FALSE;
	if(CArcLiftDlg::GetPropID("m_xRotOrg")==pItem->m_idProp)
	{
		if(0==iMenu)
			pArcLiftDlg->SelectObject(CLS_NODE,pItem->m_idProp);
		else if(1==iMenu)
			pArcLiftDlg->CopyCoordinate(pItem->m_idProp);
		else if(2==iMenu)
#ifdef AFX_TARG_ENU_ENGLISH
			ButtonClickPointItem(pPropList,pArcLiftDlg->m_xArcLift.m_xRotOrg,"m_xRotOrg","The prelift rotated point");
#else 
			ButtonClickPointItem(pPropList,pArcLiftDlg->m_xArcLift.m_xRotOrg,"m_xRotOrg","Ԥ����ת����");
#endif
	}
	else if(CArcLiftDlg::GetPropID("m_xDatumLiftPos")==pItem->m_idProp)
	{
		if(0==iMenu)
			pArcLiftDlg->SelectObject(CLS_NODE,pItem->m_idProp);
		else if(1==iMenu)
			pArcLiftDlg->CopyCoordinate(pItem->m_idProp);
		else if(2==iMenu)
#ifdef AFX_TARG_ENU_ENGLISH
			ButtonClickPointItem(pPropList,pArcLiftDlg->m_xArcLift.m_xDatumLiftPos,"m_xDatumLiftPos","The start pointion of prelift point");
#else 
			ButtonClickPointItem(pPropList,pArcLiftDlg->m_xArcLift.m_xDatumLiftPos,"m_xDatumLiftPos","��׼Ԥ�����ʼλ��");
#endif
	}
	return TRUE;
}

static BOOL FireCallBackCommand(CWnd* pCallWnd,UINT message,WPARAM wParam)
{
	//CArcLiftDlg *pArcLiftDlg = (CArcLiftDlg*)pCallWnd->GetParent();
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please copy point coordinates(Press enter key to end):","");
#else 
	pCmdLine->FillCmdLine("�뿽�������꣨���س�������:","");
#endif
	CString cmdStr="";
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	BOOL bLock=FALSE;
	if(pPropList->IsLocked())
	{
		pPropList->Unlock();
		bLock=TRUE;
	}
	while(1)
	{
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		g_pSolidDraw->Draw();
		g_pSolidDraw->ReleaseSnapStatus();
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
			break;
		else
			return FALSE;
	}
	pCmdLine->FinishCmdLine();
	if(bLock)
		pPropList->Lock();
	return TRUE;
}


const DWORD HASHTABLESIZE = 500;
CHashStrTable<long> CArcLiftDlg::propHashtable;
void CArcLiftDlg::InitArcLiftPropHashtable()
{
	int id = 1;
	propHashtable.CreateHashTable(HASHTABLESIZE);
	//
	propHashtable.SetValueAt("ArcLift",id++);
	propHashtable.SetValueAt("iNo",id++);
	propHashtable.SetValueAt("name",id++);
	propHashtable.SetValueAt("m_ciLiftType",id++);
	propHashtable.SetValueAt("m_cRotAxis",id++);
	propHashtable.SetValueAt("m_xRotOrg",id++);
	propHashtable.SetValueAt("m_xRotOrg.x",id++);
	propHashtable.SetValueAt("m_xRotOrg.y",id++);
	propHashtable.SetValueAt("m_xRotOrg.z",id++);
	propHashtable.SetValueAt("m_xDatumLiftPos",id++);
	propHashtable.SetValueAt("m_xDatumLiftPos.x",id++);
	propHashtable.SetValueAt("m_xDatumLiftPos.y",id++);
	propHashtable.SetValueAt("m_xDatumLiftPos.z",id++);
	propHashtable.SetValueAt("m_fLiftHeight",id++);
}

void CArcLiftDlg::OnDestroy()
{
	m_bEnableWindowMoveListen=false;
	CCallBackDialog::OnDestroy();
}
BOOL CArcLiftDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//��ʼ������ID
	InitArcLiftPropHashtable();
	m_propList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	SetCallBackFunc(FireCallBackCommand);
	//�ڲ�����
	if(m_bInernalStart)	
		FinishSelectObjOper();	//���ѡ�����ĺ�������
	//��ʾϵͳ����
	DisplayArcLiftProperty();
	if(m_bInernalStart)	
	{	//�ָ�ѡ����
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		m_propList.SetFocus();
		if(pItem)
			m_propList.SetCurSel(pItem->m_iIndex);	//����ѡ����	
	}
	UpdateData(FALSE);
	//�ƶ����ڵ�����λ��
	CRect rect;
	CWnd::GetWindowRect(rect);
	int width = rect.Width();
	int height=rect.Height();
	rect.left=nScrLocationX;
	rect.top=nScrLocationY;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;
	MoveWindow(rect, TRUE);
	m_bEnableWindowMoveListen=true;
	return TRUE;  
}
void CArcLiftDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}
//���ѡ�����ĺ�������
void CArcLiftDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	f3dPoint fpoint;
	CXhChar100 sText;
	CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
	if(GetEventPropId()==GetPropID("m_xRotOrg"))
	{
		if(pNode)
			m_xArcLift.m_xRotOrg.Set(pNode->Position().x,pNode->Position().y,pNode->Position().z);
		else if(ReadClipPoint(fpoint))
			m_xArcLift.m_xRotOrg.Set(fpoint.x,fpoint.y,fpoint.z);
	}
	else if(GetEventPropId()==GetPropID("m_xDatumLiftPos"))
	{	
		if(pNode)
			m_xArcLift.m_xDatumLiftPos.Set(pNode->Position().x,pNode->Position().y,pNode->Position().z);
		else if(ReadClipPoint(fpoint))
			m_xArcLift.m_xDatumLiftPos.Set(fpoint.x,fpoint.y,fpoint.z);
	}
}
void CArcLiftDlg::CopyCoordinate(long idProp)
{
	m_idEventProp=idProp;
	m_hPickObj=0;
	ClearClipboard();
	ExecuteCommand(&m_propList);
}
CString CArcLiftDlg::GetPropValueStr(long id)
{
	long handle=0;
	CXhChar100 sText;
	if(GetPropID("iNo")==id)
		sText.Printf("%d",m_xArcLift.iNo);
	else if(GetPropID("name")==id)
		sText.Printf("%s",m_xArcLift.name);
	else if(GetPropID("m_ciLiftType")==id)
	{
		if(m_xArcLift.m_ciLiftType==0)
			sText.Copy("0.Բ��Ԥ��");
		else 
			sText.Copy("1.��ֱԤ��");
	}
	else if(GetPropID("m_cRotAxis")==id)
	{
		sText.Printf("%c",m_xArcLift.m_cRotAxis);
	}
	else if(GetPropID("m_xRotOrg.x")==id)
	{
		sText.Printf("%f",m_xArcLift.m_xRotOrg.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xRotOrg.y")==id)
	{
		sText.Printf("%f",m_xArcLift.m_xRotOrg.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xRotOrg.z")==id)
	{
		sText.Printf("%f",m_xArcLift.m_xRotOrg.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xDatumLiftPos.x")==id)
	{
		sText.Printf("%f",m_xArcLift.m_xDatumLiftPos.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xDatumLiftPos.y")==id)
	{
		sText.Printf("%f",m_xArcLift.m_xDatumLiftPos.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xDatumLiftPos.z")==id)
	{
		sText.Printf("%f",m_xArcLift.m_xDatumLiftPos.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fLiftHeight")==id)
	{
		sText.Printf("%f",m_xArcLift.m_fLiftHeight);
		SimplifiedNumString(sText);
	}
	else 
		return "";
	return (char*)sText;
}

long CArcLiftDlg::GetPropID(char* propStr)
{
	long id;
	if(propHashtable.GetValueAt(propStr,id))
		return id;
	else
	{
		/*char prop_str[500];
		strcpy(prop_str,"û�ҵ���Ϊ\"");
		strcat(prop_str,propStr);
		strcat(prop_str,"\"������ID��";
		AfxMessageBox(prop_str);*/
		return 0;
	}
}

void CArcLiftDlg::DisplayArcLiftProperty()
{
	CPropTreeItem* pRootItem=m_propList.GetRootItem();
	CPropTreeItem* pGroupItem, *pPropItem,*pSonPropItem;
	const int GROUP_VECCAL=1,GROUP_CSTRANS=2,GROUP_3DCAL=3,GROUP_PREVLIFT=4;
	m_propList.SetModifyValueFunc(ModifyArcLiftProperty);
	m_propList.SetPopMenuClickFunc(ArcLiftPopMenuClick);
	m_propList.CleanTree();
	bool hasPermission=!theApp.m_bCooperativeWork || (GetPermWord(theApp.m_nUserId)&m_xArcLift.dwPermission)!=0;
	//Բ��Ԥ��
	CItemInfo *lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Prelife Arc";
#else 
	lpInfo->m_strPropName = "Բ��Ԥ��";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CArcLiftDlg::GetPropID("ArcLift");
	pGroupItem->m_bHideChildren=FALSE;

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Label Code";
#else 
	lpInfo->m_strPropName = "���";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CArcLiftDlg::GetPropID("iNo");
	pPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pPropItem->m_idProp);
	pPropItem->SetReadOnly();

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Name";
#else 
	lpInfo->m_strPropName = "����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CArcLiftDlg::GetPropID("name");
	pPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pPropItem->m_idProp);
	pPropItem->SetReadOnly(!hasPermission);

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Rotated Axis";
#else 
	lpInfo->m_strPropName = "��ת��";
#endif
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
	lpInfo->m_cmbItems="X|Y";
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CArcLiftDlg::GetPropID("m_cRotAxis");
	pPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pPropItem->m_idProp);
	pPropItem->SetReadOnly(!hasPermission);

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Rotated Axis";
#else 
	lpInfo->m_strPropName = "Ԥ����ʽ";
#endif
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
	lpInfo->m_cmbItems="0.Բ��Ԥ��|1.��ֱԤ��";
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CArcLiftDlg::GetPropID("m_ciLiftType");
	pPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pPropItem->m_idProp);
	pPropItem->SetReadOnly(!hasPermission);

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Prelife Height";
#else 
	lpInfo->m_strPropName = "Ԥ���߶�";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CArcLiftDlg::GetPropID("m_fLiftHeight");
	pPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pPropItem->m_idProp);
	pPropItem->SetReadOnly(!hasPermission);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_buttonType=BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Prelift Rotated Datum Point";
	lpInfo->m_cmbItems="Select Node|Copy Coordinates|View";
#else 
	lpInfo->m_strPropName = "Ԥ����ת����";
	lpInfo->m_cmbItems="ѡ��ڵ�|��������|�鿴";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CArcLiftDlg::GetPropID("m_xRotOrg");
	pPropItem->m_bHideChildren=FALSE;
	pPropItem->SetReadOnly(!hasPermission);

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Coordinate";
#else 
	lpInfo->m_strPropName = "X����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CArcLiftDlg::GetPropID("m_xRotOrg.x");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly(!hasPermission);

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Coordinate";
#else 
	lpInfo->m_strPropName = "Y����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CArcLiftDlg::GetPropID("m_xRotOrg.y");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly(!hasPermission);

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Coordinate";
#else 
	lpInfo->m_strPropName = "Z����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CArcLiftDlg::GetPropID("m_xRotOrg.z");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly(!hasPermission);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_buttonType=BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Prelife Point Start Position";
	lpInfo->m_cmbItems="Select Node|Copy Coordinates|View";
#else 
	lpInfo->m_strPropName = "��׼Ԥ�����ʼλ��";
	lpInfo->m_cmbItems="ѡ��ڵ�|��������|�鿴";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CArcLiftDlg::GetPropID("m_xDatumLiftPos");
	pPropItem->m_bHideChildren=FALSE;
	pPropItem->SetReadOnly(!hasPermission);

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Coordinate";
#else 
	lpInfo->m_strPropName = "X����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CArcLiftDlg::GetPropID("m_xDatumLiftPos.x");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly(!hasPermission);

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Coordinate";
#else 
	lpInfo->m_strPropName = "Y����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CArcLiftDlg::GetPropID("m_xDatumLiftPos.y");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly(!hasPermission);

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Coordinate";
#else 
	lpInfo->m_strPropName = "Z����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CArcLiftDlg::GetPropID("m_xDatumLiftPos.z");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly(!hasPermission);

	m_propList.Redraw();
}

void CArcLiftDlg::OnOK()
{

}

void CArcLiftDlg::OnBnClickedBtnConfirm()
{
	CDialog::OnOK();
}
#endif
