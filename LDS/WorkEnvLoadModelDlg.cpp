// RawWireLoadModelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "WorkEnvLoadModelDlg.h"
#include "XhCharString.h"
#include "WireLoadDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorkEnvLoadModelDlg dialog

#if defined(__TSA_)||defined(__LDS_)
CWorkEnvLoadModelDlg::CWorkEnvLoadModelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorkEnvLoadModelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWorkEnvLoadModelDlg)
	m_sWireLoadName = _T("");
	//}}AFX_DATA_INIT
	m_listCtrl.AddColumnHeader("��ʶ");
	m_listCtrl.AddColumnHeader("��������");
	m_listCtrl.AddColumnHeader("��ֱ(N)");
	m_listCtrl.AddColumnHeader("����(N)");
	m_listCtrl.AddColumnHeader("�����(N)");
	m_listCtrl.AddColumnHeader("��ֱ*(N)");
	m_listCtrl.AddColumnHeader("����*(N)");
	m_listCtrl.AddColumnHeader("�����*(N)");
}


void CWorkEnvLoadModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorkEnvLoadModelDlg)
	DDX_Control(pDX, IDC_TAB_GROUP, m_ctrlPropGroup);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Control(pDX, IDC_LIST_LOAD, m_listCtrl);
	DDX_Text(pDX, IDC_E_WIRELOAD_NAME, m_sWireLoadName);
	DDV_MaxChars(pDX, m_sWireLoadName, 50);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWorkEnvLoadModelDlg, CDialog)
	//{{AFX_MSG_MAP(CWorkEnvLoadModelDlg)
	ON_BN_CLICKED(IDC_BN_RESET_CLASSIC_WORKSTATUS, OnBnResetClassicWorkStatus)
	ON_COMMAND(ID_PASTE_ITEM, OnPasteData)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_GROUP, OnSelchangeTabGroup)
	ON_BN_CLICKED(IDC_BN_CAL_RAW_WIRELOAD, OnBnCalRawWireload)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkEnvLoadModelDlg message handlers
static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CWorkEnvLoadModelDlg *pNodeLoadListDlg=(CWorkEnvLoadModelDlg*)pListCtrl->GetParent();
	if(pNodeLoadListDlg==NULL)
		return FALSE;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	int nCurRow = pListCtrl->GetSelectedItem();
	int nCurCol = pListCtrl->GetCurSubItem();
	CRect rectItem;
	pListCtrl->GetItemRect(nCurRow,rectItem,LVIR_BOUNDS);
	if(!rectItem.PtInRect(point))
		return FALSE;	//��겻�ڵ�ǰѡ������
	if (nCurCol>1)
		pMenu->AppendMenu(MF_STRING,ID_PASTE_ITEM,"ճ��");
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pNodeLoadListDlg);
	return TRUE;
}

static BOOL ModifyItemValue(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CWorkEnvLoadModelDlg *pDlg = (CWorkEnvLoadModelDlg *)pPropList->GetParent();
	const int GROUP_GENERAL=1,GROUP_WIREINFO=2;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pItem->m_idProp==CWireLoadModel::GetPropID("hangingWireType"))
	{
		if(valueStr.Compare("����")==0)
			pDlg->wireModel.wireInfo.m_cXianType='E';
		else if(valueStr.Compare("����")==0)
			pDlg->wireModel.wireInfo.m_cXianType='C';
		else if(valueStr.Compare("����")==0)
			pDlg->wireModel.wireInfo.m_cXianType='O';
		else
			pDlg->wireModel.wireInfo.m_cXianType=0;
	}
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("hangingStyle"))
	{
		if(valueStr.Compare("���Ź���")==0)
			pDlg->wireModel.wireInfo.m_bNaiZhang=true;
		else	//"��������"
			pDlg->wireModel.wireInfo.m_bNaiZhang=false;
		if(pDlg->wireModel.wireInfo.m_bNaiZhang)
		{
			CItemInfo *lpInfo;
			CPropTreeItem* pParentItem,*pFindItem;
			char sText[500];
			long idProp=CWireLoadModel::GetPropID("allocHoriCoef");
			pFindItem = pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pFindItem==NULL)
			{	//��׼���߹���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "ˮƽ���ط���ϵ��";
				lpInfo->m_strPropHelp = "ˮƽ���ط���ϵ��";
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
				pFindItem->m_idProp = idProp;
				if(pDlg->wireModel.GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
				pFindItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);;
			}
			idProp=CWireLoadModel::GetPropID("allocVertCoef");
			pFindItem = pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pFindItem==NULL)
			{	//��׼���߹���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "��ֱ���ط���ϵ��";
				lpInfo->m_strPropHelp = "��ֱ���ط���ϵ��";
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
				pFindItem->m_idProp = idProp;
				if(pDlg->wireModel.GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
				pFindItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);;
			}
		}
		else
		{
			pPropList->DeleteItemByPropId(CWireLoadModel::GetPropID("allocHoriCoef"));
			pPropList->DeleteItemByPropId(CWireLoadModel::GetPropID("allocVertCoef"));
		}
	}
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("hangingSide"))
	{
		if(valueStr.Compare("ǰ��")==0)
			pDlg->wireModel.iNo=pDlg->wireModel.iNo-pDlg->wireModel.iNo%10;
		else
			pDlg->wireModel.iNo=pDlg->wireModel.iNo-pDlg->wireModel.iNo%10+2;
	}
	else if (pItem->m_idProp==CWireLoadModel::GetPropID("m_cFormula"))
	{
		if(valueStr.Compare("������")==0)
			pDlg->wireModel.wireInfo.m_cFormula=0;
		else
			pDlg->wireModel.wireInfo.m_cFormula=1;
	}
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("voltage"))
	{	//��Ҫ���ݵ�ѹ�ȼ��Զ�ƥ���Ե�Ӵ��ĳ��Ⱥ�����
		CString sText;
		sscanf(valueStr,"%d",&pDlg->wireModel.wireInfo.m_nVoltGrade);
		pDlg->wireModel.wireInfo.m_fAttachmentDiameter=280;
		if(!pDlg->wireModel.wireInfo.m_bNaiZhang)
		{
			if(pDlg->wireModel.wireInfo.m_nVoltGrade==35)
			{
				pDlg->wireModel.wireInfo.m_fAttachmentLength=700;
				pDlg->wireModel.wireInfo.m_fAttachmentWeight=25;
				pDlg->wireModel.wireInfo.m_fAttachmentDiameter=255;
			}
			else if(pDlg->wireModel.wireInfo.m_nVoltGrade==60)
			{
				pDlg->wireModel.wireInfo.m_fAttachmentLength=1000;
				pDlg->wireModel.wireInfo.m_fAttachmentWeight=35;
				pDlg->wireModel.wireInfo.m_fAttachmentDiameter=255;
			}
			else if(pDlg->wireModel.wireInfo.m_nVoltGrade==110)
			{
				pDlg->wireModel.wireInfo.m_fAttachmentLength=1600;
				pDlg->wireModel.wireInfo.m_fAttachmentWeight=85;
			}
			else if(pDlg->wireModel.wireInfo.m_nVoltGrade==220)
			{
				pDlg->wireModel.wireInfo.m_fAttachmentLength=2400;
				pDlg->wireModel.wireInfo.m_fAttachmentWeight=130;
			}
			else if(pDlg->wireModel.wireInfo.m_nVoltGrade==330)
			{
				pDlg->wireModel.wireInfo.m_fAttachmentLength=3900;
				pDlg->wireModel.wireInfo.m_fAttachmentWeight=210;
			}
			else if(pDlg->wireModel.wireInfo.m_nVoltGrade>=500)
			{
				pDlg->wireModel.wireInfo.m_fAttachmentLength=4900;
				pDlg->wireModel.wireInfo.m_fAttachmentWeight=370;
				pDlg->wireModel.wireInfo.m_fAttachmentDiameter=330;
			}
		}
		else
		{
			pDlg->wireModel.wireInfo.m_fAttachmentDiameter=255;
			if(pDlg->wireModel.wireInfo.m_nVoltGrade==35)
			{
				pDlg->wireModel.wireInfo.m_fAttachmentLength=950;
				pDlg->wireModel.wireInfo.m_fAttachmentWeight=35;
				pDlg->wireModel.wireInfo.m_fAttachmentDiameter=255;
			}
			else if(pDlg->wireModel.wireInfo.m_nVoltGrade==60)
			{
				pDlg->wireModel.wireInfo.m_fAttachmentLength=1250;
				pDlg->wireModel.wireInfo.m_fAttachmentWeight=45;
				pDlg->wireModel.wireInfo.m_fAttachmentDiameter=255;
			}
			else if(pDlg->wireModel.wireInfo.m_nVoltGrade==110)
			{
				pDlg->wireModel.wireInfo.m_fAttachmentLength=1900;
				pDlg->wireModel.wireInfo.m_fAttachmentWeight=100;
			}
			else if(pDlg->wireModel.wireInfo.m_nVoltGrade==220)
			{
				pDlg->wireModel.wireInfo.m_fAttachmentLength=2700;
				pDlg->wireModel.wireInfo.m_fAttachmentWeight=146;
			}
			else if(pDlg->wireModel.wireInfo.m_nVoltGrade==330)
			{
				pDlg->wireModel.wireInfo.m_fAttachmentLength=4200;
				pDlg->wireModel.wireInfo.m_fAttachmentWeight=226;
			}
			else if(pDlg->wireModel.wireInfo.m_nVoltGrade>=500)
			{
				pDlg->wireModel.wireInfo.m_fAttachmentLength=5210;
				pDlg->wireModel.wireInfo.m_fAttachmentWeight=393;
				pDlg->wireModel.wireInfo.m_fAttachmentDiameter=330;
			}
		}
		sText.Format("%.0f",pDlg->wireModel.wireInfo.m_fAttachmentDiameter);
		pPropList->SetItemPropValue(CWireLoadModel::GetPropID("m_fAttachmentDiameter"),sText);
		sText.Format("%.0f",pDlg->wireModel.wireInfo.m_fAttachmentLength);
		pPropList->SetItemPropValue(CWireLoadModel::GetPropID("m_fAttachmentLength"),sText);
		sText.Format("%.0f",pDlg->wireModel.wireInfo.m_fAttachmentWeight);
		pPropList->SetItemPropValue(CWireLoadModel::GetPropID("attachmentWeight"),sText);
	}
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("splitNumber"))
	{
		if (atoi(valueStr)>255)
		{
			MessageBox(NULL,"������ӦС��256","ERROR",MB_OK);
			return FALSE;
		}
		sscanf(valueStr,"%d",&pDlg->wireModel.wireInfo.m_nDaoXianFengLieNum);
	}
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("landHigh"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.wireInfo.land_high);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("attachmentSpec"))
		strncpy(pDlg->wireModel.wireInfo.specAttachment,valueStr,49);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("attachmentWeight"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.wireInfo.m_fAttachmentWeight);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("m_fAttachmentDiameter"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.wireInfo.m_fAttachmentDiameter);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("m_fAttachmentLength"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.wireInfo.m_fAttachmentLength);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("anchorLineAngle"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.wireInfo.m_fAchorLineAngle);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("tractionLineAngle"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.wireInfo.m_fTractionLineAngle);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("wireType"))
	{
		strncpy(pDlg->wireModel.wireInfo.wire.specWireType,valueStr,49);
		CPropTreeItem* pParentItem,*pFindItem;
		pFindItem=pPropList->FindItemByPropId(CWireLoadModel::GetPropID("wireSpec"),&pParentItem);
		if(pFindItem)
			pFindItem->m_lpNodeInfo->m_cmbItems=MakeWireSetString(tem_str);
	}
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("wireSpec"))
	{
		strncpy(pDlg->wireModel.wireInfo.wire.specWire,valueStr,49);
		CWireLoadDatabase db;
		char file_path[MAX_PATH];
		GetSysPath(file_path,NULL,NULL,NULL);
		strcat(file_path,"wireload.d");
		if(db.OpenDbFile(file_path))
		{
			DIANXIAN_STRU wire=pDlg->wireModel.wireInfo.wire;
			CXhChar50 wireType,temStr;
			CPropTreeItem *pFindItem=pPropList->FindItemByPropId(CWireLoadModel::GetPropID("wireType"),NULL);
			if(pFindItem)
			{
				wireType.Printf("%s",pFindItem->m_lpNodeInfo->m_strPropValue);
				if(db.QueryWire(wireType,tem_str,&wire))
				{
					pDlg->wireModel.wireInfo.wire=pDlg->wireModel.wireInfo.wire;
					temStr.Printf("%f",wire.m_fDiameter);
					SimplifiedNumString(temStr);
					pPropList->SetItemPropValue(CWireLoadModel::GetPropID("wireDiameter"),CString(temStr));
					temStr.Printf("%f",wire.m_fArea);
					SimplifiedNumString(temStr);
					pPropList->SetItemPropValue(CWireLoadModel::GetPropID("wireArea"),CString(temStr));
					temStr.Printf("%f",wire.m_fUnitWeight);
					SimplifiedNumString(temStr);
					pPropList->SetItemPropValue(CWireLoadModel::GetPropID("unitWeight"),CString(temStr));
					temStr.Printf("%f",wire.m_fForcePermit);
					SimplifiedNumString(temStr);
					pPropList->SetItemPropValue(CWireLoadModel::GetPropID("forcePermit"),CString(temStr));
					temStr.Printf("%f",wire.m_fElasticCoef);
					SimplifiedNumString(temStr);
					pPropList->SetItemPropValue(CWireLoadModel::GetPropID("elasticityCoef"),CString(temStr));
					temStr.Printf("%f",wire.m_fExpansionCoef);
					SimplifiedNumString(temStr);
					pPropList->SetItemPropValue(CWireLoadModel::GetPropID("expansionCoef"),CString(temStr));
					//���ʹ������
					if(wire.m_fSafeCoef!=0)
						wire.m_fMaxUseTension=wire.m_fForcePermit/wire.m_fSafeCoef;
					temStr.Printf("%f",wire.m_fMaxUseTension);
					SimplifiedNumString(temStr);
					pPropList->SetItemPropValue(CWireLoadModel::GetPropID("maxTension"),CString(temStr));
					pDlg->wireModel.wireInfo.wire=wire;
				}
			}
		}
	}
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("wireDiameter"))
		pDlg->wireModel.wireInfo.wire.m_fDiameter=atof(valueStr);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("wireArea"))
		pDlg->wireModel.wireInfo.wire.m_fArea=atof(valueStr);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("unitWeight"))
		pDlg->wireModel.wireInfo.wire.m_fUnitWeight=atof(valueStr);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("forcePermit"))
		pDlg->wireModel.wireInfo.wire.m_fForcePermit=atof(valueStr);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("saftyCoef"))
	{
		pDlg->wireModel.wireInfo.wire.m_fSafeCoef=atof(valueStr);
		pDlg->wireModel.wireInfo.wire.m_fSafeCoef=pDlg->wireModel.wireInfo.wire.m_fSafeCoef;
		if(pDlg->wireModel.wireInfo.wire.m_fSafeCoef!=0)
			pDlg->wireModel.wireInfo.wire.m_fMaxUseTension=pDlg->wireModel.wireInfo.wire.m_fForcePermit/pDlg->wireModel.wireInfo.wire.m_fSafeCoef;
		CString temStr;
		temStr.Format("%f",pDlg->wireModel.wireInfo.wire.m_fMaxUseTension);
		pPropList->SetItemPropValue(CWireLoadModel::GetPropID("maxTension"),temStr);
	}
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("maxTension"))
		pDlg->wireModel.wireInfo.wire.m_fMaxUseTension=atof(valueStr);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("elasticityCoef"))
		pDlg->wireModel.wireInfo.wire.m_fElasticCoef=atof(valueStr);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("expansionCoef"))
		pDlg->wireModel.wireInfo.wire.m_fExpansionCoef=atof(valueStr);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("horiSpace"))
		pDlg->wireModel.wireInfo.xianlu.horizon_space=atof(valueStr);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("vertSpace"))
		pDlg->wireModel.wireInfo.xianlu.vertical_space=atof(valueStr);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("minVertSpace"))
		pDlg->wireModel.wireInfo.xianlu.min_vert_space=atof(valueStr);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("xianluAngle"))
		pDlg->wireModel.wireInfo.xianlu.xianlu_angle=atoi(valueStr);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("representSpace"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.wireInfo.xianlu.represent_space);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("HtoL"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.wireInfo.xianlu.HtoL);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("allocHoriCoef"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.wireInfo.xianlu.horizon_space_allotcoef);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("allocVertCoef"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.wireInfo.xianlu.vertical_space_allotcoef);
	//��������
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("m_nLowestT"))
		sscanf(valueStr,"%d",&pDlg->wireModel.weather.m_nLowestT);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("m_nMaxWindT"))
		sscanf(valueStr,"%d",&pDlg->wireModel.weather.m_nMaxWindT);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("m_fMaxWindV"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.weather.m_fMaxWindV);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("m_fMaxWindVCheck"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.weather.m_fMaxWindVCheck);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("m_nMaxIceT"))
		sscanf(valueStr,"%d",&pDlg->wireModel.weather.m_nMaxIceT);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("m_fMaxIceWindV"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.weather.m_fMaxIceWindV);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("m_fMaxIceThick"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.weather.m_fMaxIceThick);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("m_fMaxIceThickCheck"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.weather.m_fMaxIceThickCheck);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("m_nSetupT"))
		sscanf(valueStr,"%d",&pDlg->wireModel.weather.m_nSetupT);
	else if(pItem->m_idProp==CWireLoadModel::GetPropID("m_fSetupWindV"))
		sscanf(valueStr,"%lf",&pDlg->wireModel.weather.m_fSetupWindV);
	return TRUE;
}
static BOOL ListCtrlModifyItemValue(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,int iSubItem,CString &valueStr)
{
	CWorkEnvLoadModelDlg *pDlg = (CWorkEnvLoadModelDlg *)pListCtrl->GetParent();
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	for(CWorkEnvLoad *pWorkEnv=pDlg->wireModel.hashLoad.GetFirst();pWorkEnv;pWorkEnv=pDlg->wireModel.hashLoad.GetNext())
	{
		if(pItem->m_lpNodeInfo->GetSubItemText(1).CompareNoCase(pWorkEnv->name)==0)
		{
			if(iSubItem==2)
				pWorkEnv->m_fLoadZ=atof(valueStr);
			else if(iSubItem==3)
				pWorkEnv->m_fLoadY=atof(valueStr);
			else if(iSubItem==4)
				pWorkEnv->m_fLoadX=atof(valueStr);
			else if(iSubItem==5)
				pWorkEnv->m_fMinVertSpaceLoadZ=atof(valueStr);
			else if(iSubItem==6)
				pWorkEnv->m_fMinVertSpaceLoadY=atof(valueStr);
			else if(iSubItem==7)
				pWorkEnv->m_fMinVertSpaceLoadX=atof(valueStr);
			pDlg->m_bModifyByUser=TRUE;
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CWorkEnvLoadModelDlg::OnInitDialog() 
{
	m_sWireLoadName=wireModel.name;
	CDialog::OnInitDialog();
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	CWireLoadModel::InitModelPropHashtable();
	DisplayPropertyList();
	long col_wide_arr[8]={40,125,60,60,75,70,70,80};
	m_listCtrl.InitListCtrl(col_wide_arr);
	m_listCtrl.SetContextMenuFunc(FireContextMenu);
	m_listCtrl.SetModifyValueFunc(ListCtrlModifyItemValue);
	RefreshListCtrl();
	m_bModifyByUser=FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CWorkEnvLoadModelDlg::OnPasteData()
{
	m_bModifyByUser=TRUE;
	m_listCtrl.PasteDataFromClipboard(8,NULL,TRUE);
	int nNum = m_listCtrl.GetCount();
	int i=0;
	for(i=0;i<nNum;i++)
	{
		CString sWorkEnvLoad=m_listCtrl.GetItemText(i,1);
		for(CWorkEnvLoad *pWireLoad=wireModel.hashLoad.GetFirst();pWireLoad;pWireLoad=wireModel.hashLoad.GetNext())
		{
			if(sWorkEnvLoad.CompareNoCase(pWireLoad->name)==0)
			{
				pWireLoad->m_fLoadZ=atof(m_listCtrl.GetItemText(i,2));
				pWireLoad->m_fLoadY=atof(m_listCtrl.GetItemText(i,3));
				pWireLoad->m_fLoadX=atof(m_listCtrl.GetItemText(i,4));
				pWireLoad->m_fMinVertSpaceLoadZ=atof(m_listCtrl.GetItemText(i,5));
				pWireLoad->m_fMinVertSpaceLoadY=atof(m_listCtrl.GetItemText(i,6));
				pWireLoad->m_fMinVertSpaceLoadX=atof(m_listCtrl.GetItemText(i,7));
				break;
			}
		}
	}
}

void CWorkEnvLoadModelDlg::OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_propList.m_iPropGroup=m_ctrlPropGroup.GetCurSel();
	m_propList.Redraw();
	*pResult = 0;
}

void CWorkEnvLoadModelDlg::DisplayPropertyList()
{
	m_propList.CleanTree();
	//�����������ص�����
	m_propList.SetModifyValueFunc(ModifyItemValue);
	char sText[MAX_TEMP_CHAR_100+1]="";
	const int GROUP_GENERAL=1,GROUP_WIREINFO=2,GROUP_WEATHER=3;
	CPropTreeItem *pRoot=m_propList.GetRootItem();
	CPropTreeItem *pPropItem,*pSonPropItem;

	m_ctrlPropGroup.DeleteAllItems();
	m_ctrlPropGroup.InsertItem(0,"����");
	m_ctrlPropGroup.InsertItem(1,"������Ϣ");
	m_ctrlPropGroup.InsertItem(2,"��������");
	m_ctrlPropGroup.SetCurSel(0);
	CItemInfo* lpInfo;
	//�ҽ�����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_strPropName = "�ҽ�����";
	lpInfo->m_strPropHelp = "�ҽӹ�������"; 
	lpInfo->m_cmbItems = "����|����|����";
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("hangingWireType");
	if(wireModel.GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_strPropName = "���߲�";
	lpInfo->m_strPropHelp = "����λ��"; 
	lpInfo->m_cmbItems = "ǰ��|���";
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("hangingSide");
	if(wireModel.GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	if(wireModel.iNo>=10)
		pPropItem->SetReadOnly();
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);

	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_strPropName = "�����������㹫ʽ";
	lpInfo->m_strPropHelp = "���������������㹫ʽ"; 
	lpInfo->m_cmbItems = "������|б������";
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("m_cFormula");
	if(wireModel.GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);

	//������Ϣ
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "������Ϣ";
	lpInfo->m_strPropHelp = "������Ϣ"; 
	lpInfo->m_cmbItems=MakeWireTypeSetString();
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1,FALSE);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("wireInfo");
	if(wireModel.GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_bHideChildren=FALSE;
	if(CWireLoadModel::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CWireLoadModel::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIREINFO);
	
	//��������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_strPropName = "��������";
	lpInfo->m_strPropHelp = "��������"; 
	lpInfo->m_cmbItems=MakeWireTypeSetString();
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("wireType");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIREINFO);
	//����ͺ�����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_strPropName = "����ͺ�";
	lpInfo->m_strPropHelp = "����ͺ�"; 
	lpInfo->m_cmbItems=MakeWireSetString(wireModel.wireInfo.wire.specWireType);
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("wireSpec");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIREINFO);
	//�����⾶
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "�����⾶(mm)";
	lpInfo->m_strPropHelp = "�����⾶(mm)��Ӱ����������ؼ��������ؼ���"; 
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("wireDiameter");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIREINFO);
	//���߽����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "�����(mm2)";
	lpInfo->m_strPropHelp = "���߽����(mm2)"; 
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("wireArea");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIREINFO);
	//���ߵ�λ������,kg/km
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "��λ����(kg/km)";
	lpInfo->m_strPropHelp = "���ߵ�λ������,kg/km"; 
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("unitWeight");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIREINFO);
	//���߶������,N
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "�������(N)";
	lpInfo->m_strPropHelp = "�������(N)���Ѽ�������ϵ��"; 
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("forcePermit");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIREINFO);
	//��ȫϵ��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "��ȫϵ��";
	lpInfo->m_strPropHelp = "��ȫϵ��"; 
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("saftyCoef");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIREINFO);
	//�������ʹ������,N
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "���ʹ������(N)";
	lpInfo->m_strPropHelp = "���ʹ������(N)=�������ֵ/��ȫϵ��"; 
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("maxTension");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIREINFO);
	//���ߵ���ϵ��,MPa
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "����ϵ��(MPa)";
	lpInfo->m_strPropHelp = "���ߵ���ϵ��,MPa"; 
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("elasticityCoef");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIREINFO);
	//�����¶�������ϵ��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "�¶�������ϵ��";
	lpInfo->m_strPropHelp = "�����¶�������ϵ��,�¶�����1��Cʱ����λ��(1m)���ߵ��쳤��(m)"; 
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("expansionCoef");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIREINFO);
	//�ҽӵ���ʱ��һ�ർ�ߵķ�����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "������";
	lpInfo->m_strPropHelp = "�ҽӵ���ʱ��һ�ർ�ߵķ�����"; 
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1,FALSE);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("splitNumber");
	if(wireModel.GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	//��ѹ�ȼ�
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "��ѹ�ȼ�(kv)";
	lpInfo->m_strPropHelp = "�����ҽӵ��ߵĵ�ѹ�ȼ�(kv)"; 
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1,FALSE);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("voltage");
	if(wireModel.GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	//���ߵ���ظ�(m)
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "������ظ�(m)";
	lpInfo->m_strPropHelp = "������ظ�(m)"; 
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1,FALSE);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("landHigh");
	if(wireModel.GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	
	//�ҽӷ�ʽ
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_strPropName = "�ҽӷ�ʽ";
	lpInfo->m_strPropHelp = "�����ߵĹҽӷ�ʽ"; 
	lpInfo->m_cmbItems = "���Ź���|��������";
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1,FALSE);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("hangingStyle");
	if(wireModel.GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_bHideChildren=FALSE;
	if(CWireLoadModel::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CWireLoadModel::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	//ˮƽ����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "ˮƽ����";
	lpInfo->m_strPropHelp = "ˮƽ����(m)"; 
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("horiSpace");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	//��ֱ����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "��ֱ����";
	lpInfo->m_strPropHelp = "��ֱ����(m)"; 
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("vertSpace");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	//��С��ֱ����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "��С��ֱ����";
	lpInfo->m_strPropHelp = "��С��ֱ����(m)����Ҫ�����������ϲ�����ʱʹ�á�"; 
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("minVertSpace");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	//��·ת��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "��·ת��";
	lpInfo->m_strPropHelp = "��·ת��(��)"; 
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("xianluAngle");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	//������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "������";
	lpInfo->m_strPropHelp = "������(m)�����������������ֱ����δ����ʱ���������������ʹ���������ǡ�"; 
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("representSpace");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	//�߲�ϵ��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "�߲�ϵ��";
	lpInfo->m_strPropHelp = "�߲�ϵ��"; 
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
	pSonPropItem->m_idProp = CWireLoadModel::GetPropID("HtoL");
	if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	if(wireModel.wireInfo.m_bNaiZhang)
	{
		//ˮƽ�������ϵ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "ˮƽ�������ϵ��";
		lpInfo->m_strPropHelp = "ˮƽ�������ϵ��"; 
		pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
		pSonPropItem->m_idProp = CWireLoadModel::GetPropID("allocHoriCoef");
		if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//��ֱ�������ϵ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��ֱ�������ϵ��";
		lpInfo->m_strPropHelp = "��ֱ�������ϵ��"; 
		pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,FALSE);
		pSonPropItem->m_idProp = CWireLoadModel::GetPropID("allocVertCoef");
		if(wireModel.GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	}
	//��������ϸ
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "��������ϸ";
	lpInfo->m_strPropHelp = "��߼���Ե�ӵȸ�������ϸ�������������� ',' ����"; 
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1,FALSE);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("attachmentSpec");
	pPropItem->m_bHideChildren=TRUE;
	if(CWireLoadModel::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CWireLoadModel::GetPropStatus(pPropItem->m_idProp);
	if(wireModel.GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	//����������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "����������(kg)";
	lpInfo->m_strPropHelp = "��߼���Ե�ӵȸ���������(kg)"; 
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1,FALSE);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("attachmentWeight");
	if(wireModel.GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	//������������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "��Ե�Ӵ�ֱ��(mm)";
	lpInfo->m_strPropHelp = "��Ե�Ӹ�ֱ��(mm)"; 
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1,FALSE);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("m_fAttachmentDiameter");
	if(wireModel.GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	//���������糤��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "��Ե�Ӵ�����(mm)";
	lpInfo->m_strPropHelp = "��Ե�Ӵ�����(mm)��˫��ʱ������Ҫ�ֶ���2.0��"; 
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1,FALSE);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("m_fAttachmentLength");
	if(wireModel.GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	//���߰�װ��Ϣ
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "���߰�װ��Ϣ";
	lpInfo->m_strPropHelp = "���߰�װ��Ϣ";
	CPropTreeItem* pWireSetupGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1,FALSE);
	pWireSetupGroupItem->m_idProp = CWireLoadModel::GetPropID("setupWireInfo");
	if(CWireLoadModel::GetPropStatus(pWireSetupGroupItem->m_idProp)!=-1)
		pWireSetupGroupItem->m_bHideChildren = CWireLoadModel::GetPropStatus(pWireSetupGroupItem->m_idProp);
	pWireSetupGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	//ê�߶Եؼн�
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "ê�߶Եؼн�";
	lpInfo->m_strPropHelp = "ê�߶Եؼн�(��)"; 
	pPropItem = m_propList.InsertItem(pWireSetupGroupItem,lpInfo, -1,FALSE);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("anchorLineAngle");
	if(wireModel.GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	//ǣ���߶Եؼн�
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "ǣ���߶Եؼн�";
	lpInfo->m_strPropHelp = "ǣ���߶Եؼн�(��)"; 
	pPropItem = m_propList.InsertItem(pWireSetupGroupItem,lpInfo, -1,FALSE);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("tractionLineAngle");
	if(wireModel.GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	//��������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName = "�������(��C)";
	lpInfo->m_strPropHelp = "�������(��C)";
	pPropItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("m_nLowestT");
	pPropItem->m_lpNodeInfo->m_strPropValue.Format("%d",wireModel.weather.m_nLowestT);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WEATHER); 
	//��繤��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName = "������ʱ�¶�(��C)";
	lpInfo->m_strPropHelp = "������ʱ�¶�(��C)";
	pPropItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("m_nMaxWindT");
	pPropItem->m_lpNodeInfo->m_strPropValue.Format("%d",wireModel.weather.m_nMaxWindT);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WEATHER); 
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName = "������(m/s)";
	lpInfo->m_strPropHelp = "������(m/s)";
	pPropItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("m_fMaxWindV");
	pPropItem->m_lpNodeInfo->m_strPropValue.Format("%f",wireModel.weather.m_fMaxWindV);
	SimplifiedNumString(pPropItem->m_lpNodeInfo->m_strPropValue);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WEATHER); 
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName = "�������(m/s)";
	lpInfo->m_strPropHelp = "�������(m/s)";
	pPropItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("m_fMaxWindVCheck");
	pPropItem->m_lpNodeInfo->m_strPropValue.Format("%f",wireModel.weather.m_fMaxWindVCheck);
	SimplifiedNumString(pPropItem->m_lpNodeInfo->m_strPropValue);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WEATHER); 
	//��������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName = "��󸲱�ʱ�¶�(��C)";
	lpInfo->m_strPropHelp = "��󸲱�ʱ�¶�(��C)";
	pPropItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("m_nMaxIceT");
	pPropItem->m_lpNodeInfo->m_strPropValue.Format("%d",wireModel.weather.m_nMaxIceT);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WEATHER); 
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName = "��󸲱�ʱ����(m/s)";
	lpInfo->m_strPropHelp = "��󸲱�ʱ����(m/s)";
	pPropItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("m_fMaxIceWindV");
	pPropItem->m_lpNodeInfo->m_strPropValue.Format("%f",wireModel.weather.m_fMaxIceWindV);
	SimplifiedNumString(pPropItem->m_lpNodeInfo->m_strPropValue);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WEATHER); 
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName = "�������(mm)";
	lpInfo->m_strPropHelp = "�������(mm)";
	pPropItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("m_fMaxIceThick");
	pPropItem->m_lpNodeInfo->m_strPropValue.Format("%f",wireModel.weather.m_fMaxIceThick);
	SimplifiedNumString(pPropItem->m_lpNodeInfo->m_strPropValue);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WEATHER); 
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName = "���㸲�����(mm)";
	lpInfo->m_strPropHelp = "���㸲�����(mm)";
	pPropItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("m_fMaxIceThickCheck");
	pPropItem->m_lpNodeInfo->m_strPropValue.Format("%f",wireModel.weather.m_fMaxIceThickCheck);
	SimplifiedNumString(pPropItem->m_lpNodeInfo->m_strPropValue);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WEATHER); 
	//��װ����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName = "��װʱ�¶�(��C)";
	lpInfo->m_strPropHelp = "��װʱ�¶�(��C)";
	pPropItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("m_nSetupT");
	pPropItem->m_lpNodeInfo->m_strPropValue.Format("%d",wireModel.weather.m_nSetupT);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WEATHER); 
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName = "��װʱ����(m/s)";
	lpInfo->m_strPropHelp = "��װʱ����(m/s)";
	pPropItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pPropItem->m_idProp = CWireLoadModel::GetPropID("m_fSetupWindV");
	pPropItem->m_lpNodeInfo->m_strPropValue.Format("%f",wireModel.weather.m_fSetupWindV);
	SimplifiedNumString(pPropItem->m_lpNodeInfo->m_strPropValue);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WEATHER); 

	m_propList.Redraw();
}

void CWorkEnvLoadModelDlg::RefreshListCtrl()
{
	CString sText;
	m_listCtrl.DeleteAllItems();
	for(CWorkEnvLoad *pWireLoad=wireModel.hashLoad.GetFirst();pWireLoad;pWireLoad=wireModel.hashLoad.GetNext())
	{
		CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
		sText.Format("%d",wireModel.hashLoad.GetCursorKey());
		lpRoot->AddSubItemText(sText,TRUE);
		lpRoot->AddSubItemText(pWireLoad->name,TRUE);
		sText.Format("%.0f",pWireLoad->m_fLoadZ);
		SimplifiedNumString(sText);
		lpRoot->AddSubItemText(sText);
		sText.Format("%.0f",pWireLoad->m_fLoadY);
		SimplifiedNumString(sText);
		lpRoot->AddSubItemText(sText);
		sText.Format("%.0f",pWireLoad->m_fLoadX);
		SimplifiedNumString(sText);
		lpRoot->AddSubItemText(sText);
		sText.Format("%.0f",pWireLoad->m_fMinVertSpaceLoadZ);
		SimplifiedNumString(sText);
		lpRoot->AddSubItemText(sText);
		sText.Format("%.0f",pWireLoad->m_fMinVertSpaceLoadY);
		SimplifiedNumString(sText);
		lpRoot->AddSubItemText(sText);
		sText.Format("%.0f",pWireLoad->m_fMinVertSpaceLoadX);
		SimplifiedNumString(sText);
		lpRoot->AddSubItemText(sText);
		CSuperGridCtrl::CTreeItem * pItem = m_listCtrl.InsertRootItem(lpRoot);
	}
}

void CWorkEnvLoadModelDlg::OnBnResetClassicWorkStatus() 
{
	wireModel.ResetWorkEnvLoadSet();
	RefreshListCtrl();
}

void CWorkEnvLoadModelDlg::OnBnCalRawWireload() 
{
	m_bModifyByUser=FALSE;
	wireModel.CalWorkEnvLoad(Ta.m_cLandscape,Ta.m_cLineRoadType);
	RefreshListCtrl();
}
//�û���������ʱ����ȱ�ٺ��غͻ��ص�����,��Ҫϵͳ���ݶ๤����Ͽ���
void CWorkEnvLoadModelDlg::ParseLiveLoad()
{
	CWorkEnvLoad *pWindEnvLoad;
	//����ú��ض��Ǹ��ݴ�繤���Ƴ�����,��Ϊ��繤���ް�װ���ء��������صȿɱ���أ�����׼ȷ�ĺ���ز�������
	for(pWindEnvLoad=wireModel.hashLoad.GetFirst();pWindEnvLoad;pWindEnvLoad=wireModel.hashLoad.GetNext())
	{
		if(wireModel.hashLoad.GetCursorKey()==WORKENVLOAD_WIND)
		{
			if(pWindEnvLoad->m_fAttachmentLoadZ<=0)
				pWindEnvLoad->m_fAttachmentLoadZ=wireModel.wireInfo.m_fAttachmentWeight*9.8;
			pWindEnvLoad->m_fWireLoadZ=pWindEnvLoad->m_fLoadZ-pWindEnvLoad->m_fAttachmentLoadZ;
			pWindEnvLoad->m_fMinVertSpaceLoadZ=pWindEnvLoad->m_fMinVertSpaceLoadZ-pWindEnvLoad->m_fAttachmentLoadZ;
			pWindEnvLoad->m_fWireLoadX=pWindEnvLoad->m_fLoadX-pWindEnvLoad->m_fAttachmentLoadX;
			break;
		}
	}
	if(pWindEnvLoad==NULL)
		return;
	for(CWorkEnvLoad *pEnvLoad=wireModel.hashLoad.GetFirst();pEnvLoad;pEnvLoad=wireModel.hashLoad.GetNext())
	{
		if(wireModel.hashLoad.GetCursorKey()==WORKENVLOAD_WIND)
			continue;	//�Ѽ���
		else //if(wireModel.hashLoad.GetCursorKey()==WORKENVLOAD_ICE)
		{
			pEnvLoad->m_fWireLoadZ=pWindEnvLoad->m_fWireLoadZ;
			pEnvLoad->m_fAttachmentLoadZ=pWindEnvLoad->m_fAttachmentLoadZ;
			pEnvLoad->m_fWireLoadX=pEnvLoad->m_fLoadX-pEnvLoad->m_fAttachmentLoadX;
			pEnvLoad->m_fMinVertSpaceLoadZ=pEnvLoad->m_fMinVertSpaceLoadZ-pEnvLoad->m_fAttachmentLoadZ-pEnvLoad->m_fSetupGa;
		}
	}
}

void CWorkEnvLoadModelDlg::OnOK() 
{
	UpdateData();
	_snprintf(wireModel.name,50,"%s",m_sWireLoadName);
	if(m_bModifyByUser)
		ParseLiveLoad();	
	CDialog::OnOK();
}
#endif
