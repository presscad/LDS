//<LOCALE_TRANSLATE Confirm by hxr/>
// DesignSimpleLadderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "GlobalFunc.h"
#include "DesignSimpleLadderDlg.h"
#include "env_def.h"
#include "Query.h"
#include "CmdLineDlg.h"
#include "MainFrm.h"
#include "MultiLineInputDlg.h"
#include "LicFuncDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CDesignSimpleLadderDlg dialog


CDesignSimpleLadderDlg::CDesignSimpleLadderDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesignSimpleLadderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignSimpleLadderDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDesignSimpleLadderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignSimpleLadderDlg)
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Control(pDX, IDC_TAB_GROUP, m_ctrlPropGroup);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDesignSimpleLadderDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignSimpleLadderDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_GROUP, OnSelchangeTabGroup)
	ON_BN_CLICKED(ID_CONFIRM_CREATE, OnConfirmCreate)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignSimpleLadderDlg message handlers
BOOL IsLadderLenRight(CDesignSimpleLadderDlg *pSimpleLadderDlg,CXhChar100 tem_str)
{
	//根据圆孔钢板上端距以及钢板间距字符串计算爬梯钢管的总长度
	double fSumLen=pSimpleLadderDlg->PlatePara.fUpEndSpace;
	int uStrLen = strlen(tem_str);
	CString sTmpPlateOffset;
	for(int i=0;i<uStrLen;i++)
	{
		if(tem_str[i] == ',')
		{
			fSumLen+=atof(sTmpPlateOffset);
			sTmpPlateOffset = "";
		}
		else
			sTmpPlateOffset+=tem_str[i];
		if(i == uStrLen-1)
			fSumLen+=atof(sTmpPlateOffset);
	}
	//主管长度
	if(pSimpleLadderDlg->LineTubePara.pBaseLineTube)
	{
		double fBaseTubeLen=pSimpleLadderDlg->LineTubePara.pBaseLineTube->GetLength();
		if((fBaseTubeLen-pSimpleLadderDlg->LineTubePara.fDownSpace)<fSumLen)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Length of Ladder tube is larger than the length of main tube, please input again!");
#else
			AfxMessageBox("爬梯钢管长度已超过主管长度，请重新输入!");
#endif
			return FALSE;
		}
	}
	return TRUE;
}
static BOOL ButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CDesignSimpleLadderDlg* pDlg = (CDesignSimpleLadderDlg*)pPropList->GetParent();
#ifdef AFX_TARG_ENU_ENGLISH
	pDlg->SetCmdPickPromptStr("please select the datum tube which needs design ladder:");
#else
	pDlg->SetCmdPickPromptStr("请选择要设计爬梯的基准钢管:");
#endif
	if(pItem->m_idProp == CDesignSimpleLadderDlg::GetPropID("LineTubePara.pBaseLineTube"))
		pDlg->SelectLineTube();
	else if(pItem->m_idProp==CDesignSimpleLadderDlg::GetPropID("PlatePara.sBetweenSpace"))
	{
		CMultiLineInputDlg sPaceDlg;
		CXhChar100 sTextValue;
		sPaceDlg.m_sSpaceValue= pDlg->PlatePara.sBetweenSpace;
		if(sPaceDlg.DoModal()!=IDOK)
			return FALSE;
		strcpy(sTextValue,sPaceDlg.m_sRichSpaceValue);
		if(IsLadderLenRight(pDlg,sTextValue))
		{
			strcpy(pDlg->PlatePara.sBetweenSpace,sTextValue);
			pPropList->SetItemPropValue(CDesignSimpleLadderDlg::GetPropID("PlatePara.sBetweenSpace"),CString(sPaceDlg.m_sRichSpaceValue));
			char* key=strtok(sTextValue,",");
			pDlg->PlatePara.fDownEndSpace=atof(key)+pDlg->LineTubePara.fDownSpace;
			if(pDlg->GetPropValueStr(CDesignSimpleLadderDlg::GetPropID("PlatePara.fDownEndSpace"),sTextValue)>0)
				pPropList->SetItemPropValue(CDesignSimpleLadderDlg::GetPropID("PlatePara.fDownEndSpace"),CString(sTextValue));
		}
	}
	pDlg->ReDraw();
	return true;
}
static BOOL ModifyItemValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CItemInfo *lpInfo=NULL;
	CPropTreeItem *pFindItem=NULL,*pPropItem=NULL;
	CDesignSimpleLadderDlg *pSimpleLadderDlg=(CDesignSimpleLadderDlg*)pPropList->GetParent();
	CXhChar100 tem_str;
	tem_str.Printf("%s",valueStr);
	CString sTempValue="";
	char sTextValue[100]="";
	BOOL bUpdateLayoutPara=FALSE;
	//脚钉信息
	if(CDesignSimpleLadderDlg::GetPropID("FootNailPara.fDownEndSpace")==pItem->m_idProp)
		pSimpleLadderDlg->FootNailPara.fDownEndSpace = atof(tem_str);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.sBetweenSpace")==pItem->m_idProp	//钢板间距
			||CDesignSimpleLadderDlg::GetPropID("PlatePara.fUpEndSpace")==pItem->m_idProp	//钢板上端距
			||CDesignSimpleLadderDlg::GetPropID("FootNailPara.fBetweenSpace")==pItem->m_idProp)//脚钉间距
	{	
		if(CDesignSimpleLadderDlg::GetPropID("PlatePara.sBetweenSpace")==pItem->m_idProp)
		{
			if(IsLadderLenRight(pSimpleLadderDlg,tem_str))
			{
				strcpy(pSimpleLadderDlg->PlatePara.sBetweenSpace,tem_str);
				//角钢距钢管下端距离
				char* key=strtok(tem_str,",");
				pSimpleLadderDlg->PlatePara.fDownEndSpace=atof(key)+pSimpleLadderDlg->LineTubePara.fDownSpace;
				if(pSimpleLadderDlg->GetPropValueStr(CDesignSimpleLadderDlg::GetPropID("PlatePara.fDownEndSpace"),sTextValue))
					pPropList->SetItemPropValue(CDesignSimpleLadderDlg::GetPropID("PlatePara.fDownEndSpace"),CString(sTextValue));
			}
		}
		else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.fUpEndSpace")==pItem->m_idProp)
		{
			//根据圆孔钢板上端距以及钢板间距字符串计算爬梯钢管的总长度
			double fSumLen=atof(tem_str);
			int uStrLen = strlen(pSimpleLadderDlg->PlatePara.sBetweenSpace);
			CString sTmpPlateOffset;
			for(int i=0;i<uStrLen;i++)
			{
				if(pSimpleLadderDlg->PlatePara.sBetweenSpace[i] == ',')
				{
					fSumLen+=atof(sTmpPlateOffset);
					sTmpPlateOffset = "";
				}
				else
					sTmpPlateOffset+=pSimpleLadderDlg->PlatePara.sBetweenSpace[i];
				if(i == uStrLen-1)
					fSumLen+=atof(sTmpPlateOffset);
			}
			//主管长度
			if(pSimpleLadderDlg->LineTubePara.pBaseLineTube)
			{
				double fBaseTubeLen=pSimpleLadderDlg->LineTubePara.pBaseLineTube->GetLength();
				if((fBaseTubeLen-pSimpleLadderDlg->LineTubePara.fDownSpace)<fSumLen)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Length of Ladder tube is larger than the length of main tube, please input again!");
#else
					AfxMessageBox("爬梯钢管长度已超过主管长度，请重新输入!");
#endif
					return FALSE;
				}
			}
			pSimpleLadderDlg->PlatePara.fUpEndSpace = atoi(tem_str);
		}
		else if(CDesignSimpleLadderDlg::GetPropID("FootNailPara.fBetweenSpace")==pItem->m_idProp)
			pSimpleLadderDlg->FootNailPara.fBetweenSpace = atof(tem_str);
		//根据圆孔钢板上端距以及钢板间距字符串计算爬梯钢管的总长度
		double fSumLen=pSimpleLadderDlg->PlatePara.fUpEndSpace;
		int uStrLen = strlen(pSimpleLadderDlg->PlatePara.sBetweenSpace);
		CString sTmpPlateOffset;
		for(int i=0;i<uStrLen;i++)
		{
			if(pSimpleLadderDlg->PlatePara.sBetweenSpace[i] == ',')
			{
				fSumLen+=atof(sTmpPlateOffset);
				sTmpPlateOffset = "";
			}
			else
				sTmpPlateOffset+=pSimpleLadderDlg->PlatePara.sBetweenSpace[i];
			if(i == uStrLen-1)
				fSumLen+=atof(sTmpPlateOffset);
		}
		//根据钢管总长度以及第一个脚钉的下端距、脚钉间距计算脚钉个数
		pSimpleLadderDlg->FootNailPara.nNum = ftoi(fSumLen-pSimpleLadderDlg->FootNailPara.fDownEndSpace)/ftoi(pSimpleLadderDlg->FootNailPara.fBetweenSpace);
		//FootNailPara.fUpEndSpace=钢管爬梯总长度-脚钉下间距-脚钉个数*脚钉间距
		pSimpleLadderDlg->FootNailPara.fUpEndSpace=(fSumLen-pSimpleLadderDlg->FootNailPara.fDownEndSpace)
			-pSimpleLadderDlg->FootNailPara.nNum*pSimpleLadderDlg->FootNailPara.fBetweenSpace;
		int idPropItem=CDesignSimpleLadderDlg::GetPropID("FootNailPara.nNum");
		if(pSimpleLadderDlg->GetPropValueStr(idPropItem,sTextValue)>0)
			pPropList->SetItemPropValue(idPropItem,CString(sTextValue));
		idPropItem=CDesignSimpleLadderDlg::GetPropID("FootNailPara.fUpEndSpace");
		if(pSimpleLadderDlg->GetPropValueStr(idPropItem,sTextValue)>0)
			pPropList->SetItemPropValue(idPropItem,CString(sTextValue));
	}
	else if(CDesignSimpleLadderDlg::GetPropID("FootNailPara.shortSpecification")==pItem->m_idProp)
	{
		long d,L;
		restore_Ls_guige(tem_str,&pSimpleLadderDlg->FootNailPara.short_grade,&d,&L);
		pSimpleLadderDlg->FootNailPara.D = d;
		pSimpleLadderDlg->FootNailPara.fShortLen = L;
	}
	else if(CDesignSimpleLadderDlg::GetPropID("FootNailPara.longSpecification")==pItem->m_idProp)
	{
		long d,L;
		restore_Ls_guige(tem_str,&pSimpleLadderDlg->FootNailPara.long_grade,&d,&L);
		pSimpleLadderDlg->FootNailPara.D = d;
		pSimpleLadderDlg->FootNailPara.fLongLen = L;
	}
	else if(CDesignSimpleLadderDlg::GetPropID("FootNailPara.iDirection")==pItem->m_idProp)
	{
		if(tem_str[0] == '0')
			pSimpleLadderDlg->FootNailPara.iDirection = 0;
		else
			pSimpleLadderDlg->FootNailPara.iDirection = 1;
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.sPartNo")==pItem->m_idProp)
		strcpy(pSimpleLadderDlg->PlatePara.sPartNo,tem_str);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.sAnglePartNo")==pItem->m_idProp)
		strcpy(pSimpleLadderDlg->PlatePara.sAnglePartNo,tem_str);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.sEllipsePartNo")==pItem->m_idProp)
		strcpy(pSimpleLadderDlg->PlatePara.sEllipsePartNo,tem_str);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.fCircinalPlateA")==pItem->m_idProp)
		pSimpleLadderDlg->PlatePara.fCircinalPlateA = atof(tem_str);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.fCircinalPlateB")==pItem->m_idProp)
		pSimpleLadderDlg->PlatePara.fCircinalPlateB = atof(tem_str);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.nLSD")==pItem->m_idProp)
	{
		memmove(tem_str,tem_str+1,99);
		pSimpleLadderDlg->PlatePara.nLSD = atoi(tem_str);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.fEllipsePlateC")==pItem->m_idProp)
		pSimpleLadderDlg->PlatePara.fEllipsePlateC = atof(tem_str);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.nPlateThick")==pItem->m_idProp)
		pSimpleLadderDlg->PlatePara.nPlateThick = atoi(tem_str);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.fLineAngleLen")==pItem->m_idProp)
		pSimpleLadderDlg->PlatePara.fLineAngleLen = atof(tem_str);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.fDownEndSpace")==pItem->m_idProp)
	{
		pSimpleLadderDlg->PlatePara.fDownEndSpace = atof(tem_str);
		strcpy(sTextValue,pSimpleLadderDlg->PlatePara.sBetweenSpace);
		char *key=strtok(sTextValue,",");
		double fFirBetweenSpace = atof(key);
		pSimpleLadderDlg->LineTubePara.fDownSpace = pSimpleLadderDlg->PlatePara.fDownEndSpace - fFirBetweenSpace;
		if(pSimpleLadderDlg->GetPropValueStr(CDesignSimpleLadderDlg::GetPropID("LineTubePara.fDownSpace"),sTextValue)>0)
			pPropList->SetItemPropValue(CDesignSimpleLadderDlg::GetPropID("LineTubePara.fDownSpace"),CString(sTextValue));
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.iEllipseAperture")==pItem->m_idProp)
		pSimpleLadderDlg->PlatePara.iEllipseAperture = atoi(tem_str);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.iConnectType")==pItem->m_idProp)
	{
		if(tem_str[0] == '0')
			pSimpleLadderDlg->PlatePara.iConnectType=0;
		else
			pSimpleLadderDlg->PlatePara.iConnectType=1;
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.iAngleDirect")==pItem->m_idProp)
	{
		if(tem_str[0] == '0')
			pSimpleLadderDlg->PlatePara.iAngleDirect=0;
		else
			pSimpleLadderDlg->PlatePara.iAngleDirect=1;
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.specification")==pItem->m_idProp)
	{
		double wide,thick;
		restore_JG_guige(tem_str,wide,thick);
		pSimpleLadderDlg->PlatePara.thick = thick;
		pSimpleLadderDlg->PlatePara.wide = wide;
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.cMaterial")==pItem->m_idProp)
		pSimpleLadderDlg->PlatePara.cMaterial = QuerySteelBriefMatMark(tem_str);
	else if(CDesignSimpleLadderDlg::GetPropID("LineTubePara.sPartNo")==pItem->m_idProp)
	{
		strcpy(pSimpleLadderDlg->LineTubePara.sPartNo,tem_str);
		SEGI iSeg;
		if(ParsePartNo(pSimpleLadderDlg->LineTubePara.sPartNo,&iSeg,NULL))
			pSimpleLadderDlg->LineTubePara.iSeg=iSeg;
		sTempValue.Format("%s",(char*)pSimpleLadderDlg->LineTubePara.iSeg.ToString());
		pPropList->SetItemPropValue(CDesignSimpleLadderDlg::GetPropID("LineTubePara.iSeg"),sTempValue);
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(valueStr,BOMPART::TUBE);
		if(pPart)
		{
			pSimpleLadderDlg->LineTubePara.wide=pPart->wide;
			pSimpleLadderDlg->LineTubePara.thick=pPart->thick;
			//pSimpleLadderDlg->LineTubePara.cMaterial=pPart->cMaterial;
			//
			CXhChar50 sValue("%.0fx%.0f",pPart->wide,pPart->thick);
			pPropList->SetItemPropValue(CDesignSimpleLadderDlg::GetPropID("LineTubePara.specification"),(char*)sValue);
			//QuerySteelMatMark(pPart->cMaterial,sValue);
			//pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("RibPlate.cMaterial"),(char*)sValue);
		}
#endif
	}
	else if(CDesignSimpleLadderDlg::GetPropID("LineTubePara.iSeg")==pItem->m_idProp)
		pSimpleLadderDlg->LineTubePara.iSeg = SEGI(tem_str);
	else if(CDesignSimpleLadderDlg::GetPropID("LineTubePara.pBaseLineTube")==pItem->m_idProp)
		sscanf(tem_str,"%X",&pSimpleLadderDlg->LineTubePara.pBaseLineTube->handle);
	else if(CDesignSimpleLadderDlg::GetPropID("LineTubePara.fDownSpace")==pItem->m_idProp)
	{	pSimpleLadderDlg->LineTubePara.fDownSpace = atof(tem_str);
		strcpy(sTextValue,pSimpleLadderDlg->PlatePara.sBetweenSpace);
		char *key=strtok(sTextValue,",");
		double fFirBetweenSpace = atof(key);
		pSimpleLadderDlg->PlatePara.fDownEndSpace = pSimpleLadderDlg->LineTubePara.fDownSpace + fFirBetweenSpace;
		if(pSimpleLadderDlg->GetPropValueStr(CDesignSimpleLadderDlg::GetPropID("PlatePara.fDownEndSpace"),sTextValue)>0)
			pPropList->SetItemPropValue(CDesignSimpleLadderDlg::GetPropID("PlatePara.fDownEndSpace"),CString(sTextValue));
	}
	else if(CDesignSimpleLadderDlg::GetPropID("LineTubePara.fAngle")==pItem->m_idProp)
		pSimpleLadderDlg->LineTubePara.fAngle = atof(tem_str)*RADTODEG_COEF;
	else if(CDesignSimpleLadderDlg::GetPropID("LineTubePara.specification")==pItem->m_idProp)
	{
		double wide,thick;
		restore_JG_guige(tem_str,wide,thick);
		pSimpleLadderDlg->LineTubePara.thick = thick;
		pSimpleLadderDlg->LineTubePara.wide = wide;
	}

	pSimpleLadderDlg->ReDraw();
	return TRUE;
}

void CDesignSimpleLadderDlg::OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iCurSel=m_ctrlPropGroup.GetCurSel();
	m_propList.m_iPropGroup = iCurSel;
	m_propList.Redraw();
	*pResult = 0;
}

const DWORD HASHTABLESIZE = 500;
CStringKeyHashTable<long> CDesignSimpleLadderDlg::propHashtable;
void CDesignSimpleLadderDlg::InitPropHashtable()
{
	int id = 1;
	propHashtable.CreateHashTable(HASHTABLESIZE);
	//脚钉信息
	propHashtable.SetValueAt("FootNailPara.iSeg",id++);
	propHashtable.SetValueAt("FootNailPara.fUpEndSpace",id++);
	propHashtable.SetValueAt("FootNailPara.fDownEndSpace",id++);
	propHashtable.SetValueAt("FootNailPara.fBetweenSpace",id++);
	propHashtable.SetValueAt("FootNailPara.nNum",id++);
	propHashtable.SetValueAt("FootNailPara.iDirection",id++);
	propHashtable.SetValueAt("FootNailPara.longSpecification",id++);
	propHashtable.SetValueAt("FootNailPara.shortSpecification",id++);
	//连接板的信息
	propHashtable.SetValueAt("PlatePara.sPartNo",id++);
	propHashtable.SetValueAt("PlatePara.sEllipsePartNo",id++);
	propHashtable.SetValueAt("PlatePara.iSeg",id++);
	propHashtable.SetValueAt("PlatePara.sBetweenSpace",id++);
	propHashtable.SetValueAt("PlatePara.fCircinalPlateA",id++);
	propHashtable.SetValueAt("PlatePara.fCircinalPlateB",id++);
	propHashtable.SetValueAt("PlatePara.fEllipsePlateC",id++);
	propHashtable.SetValueAt("PlatePara.nLSD",id++);
	propHashtable.SetValueAt("PlatePara.fLineAngleLen",id++);
	propHashtable.SetValueAt("PlatePara.fDownEndSpace",id++);
	propHashtable.SetValueAt("PlatePara.iConnectType",id++);
	propHashtable.SetValueAt("PlatePara.specification",id++);
	propHashtable.SetValueAt("PlatePara.cMaterial",id++);
	propHashtable.SetValueAt("PlatePara.iAngleDirect",id++);
	propHashtable.SetValueAt("PlatePara.nPlateThick",id++);
	propHashtable.SetValueAt("PlatePara.sAnglePartNo",id++);
	propHashtable.SetValueAt("PlatePara.iEllipseAperture",id++);
	//钢管的信息
	propHashtable.SetValueAt("LineTubePara.sPartNo",id++);
	propHashtable.SetValueAt("LineTubePara.iSeg",id++);
	propHashtable.SetValueAt("LineTubePara.pBaseLineTube",id++);
	propHashtable.SetValueAt("LineTubePara.fDownSpace",id++);
	propHashtable.SetValueAt("LineTubePara.fAngle",id++);
	propHashtable.SetValueAt("LineTubePara.specification",id++);
}

long CDesignSimpleLadderDlg::GetPropID(char* propStr)
{
	long id;
	if(propHashtable.GetValueAt(propStr,id))
		return id;
	else
	{
		/*char prop_str[500];
		strcpy(prop_str,"没找到名为\"");
		strcat(prop_str,propStr);
		strcat(prop_str,"\"的属性ID！";
		AfxMessageBox(prop_str);*/
		return 0;
	}
}

BOOL CDesignSimpleLadderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	UpdateData(FALSE);
	m_ctrlPropGroup.SetMinTabWidth(50);	//设置TabCtrl最小宽度
	InitPropHashtable();	//初始化属性哈希表
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	if(m_bInernalStart == FALSE)
	{
		FootNailPara.short_grade=FootNailPara.long_grade=6.8;
		FootNailPara.fLongLen = 360;
		FootNailPara.fShortLen = 180;
		FootNailPara.iDirection = 1;
		FootNailPara.D = 16;
		PlatePara.fCircinalPlateA = 20;
		PlatePara.fCircinalPlateB = 35;
		PlatePara.fEllipsePlateC = 35;
		PlatePara.fLineAngleLen = 100;
		PlatePara.fDownEndSpace = 765;
		PlatePara.iConnectType = 0;
		PlatePara.nPlateThick = 5;
		PlatePara.nLSD = 16;
		PlatePara.thick = 5;
		PlatePara.wide = 50;
		PlatePara.cMaterial = QuerySteelBriefMatMark("Q390");
		PlatePara.iConnectType = 0;
		PlatePara.iAngleDirect = 0;
		PlatePara.iEllipseAperture = 20;
		PlatePara.fUpEndSpace = 365;
		strcpy(PlatePara.sBetweenSpace,"365,1375,1875,1875");
		LineTubePara.fAngle = 0;
		LineTubePara.fDownSpace = 400;
		LineTubePara.iSeg = SEGI(1);
		LineTubePara.thick = 4;
		LineTubePara.wide = 50;
		FootNailPara.fBetweenSpace = 375;
		FootNailPara.fDownEndSpace = 50;
		double fSumLen=PlatePara.fUpEndSpace+(365+1375+1875*2);
		FootNailPara.nNum = ftoi(fSumLen-FootNailPara.fDownEndSpace)/ftoi(FootNailPara.fBetweenSpace);
		FootNailPara.fUpEndSpace = (fSumLen-FootNailPara.fDownEndSpace)-FootNailPara.nNum*FootNailPara.fBetweenSpace;
	}
	else
	{
		LineTubePara.pBaseLineTube = (CLDSLineTube*)console.FromPartHandle(m_hPickObj,CLS_LINETUBE);
		if(LineTubePara.pBaseLineTube==NULL)
			CDialog::OnCancel();
	}
	DisplayPropertyList();
	InitPropHashtable();
	UpdateData(FALSE);

	//生成画笔
	if(solid_pen.GetSafeHandle()==0)
		solid_pen.CreatePen(PS_SOLID,1,RGB(0,0,200));
	if(dash_pen.GetSafeHandle()==0)
		dash_pen.CreatePen(PS_DASH,1,RGB(0,0,0));
	if(dash_dot_pen.GetSafeHandle()==0)
		dash_dot_pen.CreatePen(PS_DASHDOT,1,RGB(255,0,0));
	if(green_solid_pen.GetSafeHandle()==0)
		green_solid_pen.CreatePen(PS_SOLID,2,RGB(0,255,0));
	if(green_dash_pen.GetSafeHandle()==0)
		green_dash_pen.CreatePen(PS_DASH,1,RGB(0,255,0));
	if(red_solid_pen.GetSafeHandle()==0)
		red_solid_pen.CreatePen(PS_SOLID,1,RGB(255,0,0));
	if(blue_solid_pen.GetSafeHandle()==0)
		blue_solid_pen.CreatePen(PS_SOLID,1,RGB(0,0,255));
	if(purple_solid_pen.GetSafeHandle()==0)	//紫色画笔
		purple_solid_pen.CreatePen(PS_SOLID,1,RGB(128,0,255));
	//移动对话框到左上角
	CRect rect;
	GetWindowRect(rect);
	rect.right = rect.Width();
	rect.left = 0;
	rect.bottom = rect.Height();
	rect.top = 0;
	MoveWindow(rect,TRUE);
	return TRUE;
}

void CDesignSimpleLadderDlg::DisplayPropertyList(int iCurSel/* =0 */)
{
	CItemInfo* lpInfo;
	CPropTreeItem *pPropItem,*pGroupItem,*pRoot=m_propList.GetRootItem();
	const int GROUP_PLATE=1,GROUP_PLACE=2,GROUP_FOOT_NAIL=3;
	char valueStr[100]="";
	m_propList.CleanTree();
	m_arrPropGroupLabel.RemoveAll();
	m_arrPropGroupLabel.SetSize(3);
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrPropGroupLabel.SetAt(GROUP_FOOT_NAIL-1,"foot nail");
	m_arrPropGroupLabel.SetAt(GROUP_PLATE-1,"plate");
	m_arrPropGroupLabel.SetAt(GROUP_PLACE-1,"tube");
#else
	m_arrPropGroupLabel.SetAt(GROUP_FOOT_NAIL-1,"脚钉");
	m_arrPropGroupLabel.SetAt(GROUP_PLATE-1,"钢板");
	m_arrPropGroupLabel.SetAt(GROUP_PLACE-1,"钢管");
#endif
	RefreshTabCtrl(iCurSel); 
	
	//设置属性栏回调函数
	m_propList.SetModifyValueFunc(ModifyItemValue);
	m_propList.SetButtonClickFunc(ButtonClick);
	//脚钉信息
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName.Format("long foot nail info");
	lpInfo->m_strPropHelp.Format("long foot nail information");
#else
	lpInfo->m_strPropName.Format("长脚钉信息");
	lpInfo->m_strPropHelp.Format("长脚钉信息");
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp=0;
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_FOOT_NAIL);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "bottom distance D";
	lpInfo->m_strPropHelp.Format("distance from foot nail to bottom end");
#else
	lpInfo->m_strPropName = "下端距D";
	lpInfo->m_strPropHelp.Format("长脚钉距下端的距离");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("FootNailPara.fDownEndSpace");
	if(GetPropValueStr(GetPropID("FootNailPara.fDownEndSpace"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_FOOT_NAIL);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbType=CDT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "spec";
	lpInfo->m_cmbItems = MakeBoltSetString(3);//"M16X180|M20X200|M24X240";
	lpInfo->m_strPropHelp.Format("specifications for foot nail");
#else
	lpInfo->m_strPropName = "规格";
	lpInfo->m_cmbItems = MakeBoltSetString(3);//"M16X180|M20X200|M24X240";
	lpInfo->m_strPropHelp.Format("长脚钉的规格");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("FootNailPara.longSpecification");
	if(GetPropValueStr(GetPropID("FootNailPara.longSpecification"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_FOOT_NAIL);
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName.Format("short foot nail info");
	lpInfo->m_strPropHelp.Format("short foot nail information");
#else
	lpInfo->m_strPropName.Format("短脚钉信息");
	lpInfo->m_strPropHelp.Format("短脚钉信息");
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp=0;
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_FOOT_NAIL);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "distance L1";
	lpInfo->m_strPropHelp.Format("distance between two bolts");
#else
	lpInfo->m_strPropName = "间距L1";
	lpInfo->m_strPropHelp.Format("两个脚钉之间的距离");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("FootNailPara.fBetweenSpace");
	if(GetPropValueStr(GetPropID("FootNailPara.fBetweenSpace"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_FOOT_NAIL);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "top distance U";
	lpInfo->m_strPropHelp.Format("distance from the first foot nail to the top");
#else
	lpInfo->m_strPropName = "上端距U";
	lpInfo->m_strPropHelp.Format("第一个短脚钉距上端的距离");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("FootNailPara.fUpEndSpace");
	if(GetPropValueStr(GetPropID("FootNailPara.fUpEndSpace"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_FOOT_NAIL);
	pPropItem->SetReadOnly();

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Number of short foot nail";
	lpInfo->m_strPropHelp.Format("Number of short foot nail on ladder");
#else
	lpInfo->m_strPropName = "短脚钉个数";
	lpInfo->m_strPropHelp.Format("爬梯上短脚钉的个数");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("FootNailPara.nNum");
	if(GetPropValueStr(GetPropID("FootNailPara.nNum"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_FOOT_NAIL);
	pPropItem->SetReadOnly();

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbType=CDT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "spec";
	lpInfo->m_cmbItems = MakeBoltSetString(3);//"M16X180|M20X200|M24X240";
	lpInfo->m_strPropHelp.Format("specification for short foot nail");
#else
	lpInfo->m_strPropName = "规格";
	lpInfo->m_cmbItems = MakeBoltSetString(3);//"M16X180|M20X200|M24X240";
	lpInfo->m_strPropHelp.Format("短脚钉的规格");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("FootNailPara.shortSpecification");
	if(GetPropValueStr(GetPropID("FootNailPara.shortSpecification"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_FOOT_NAIL);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems = "0.To Left|1.To Right";
	lpInfo->m_strPropName.Format("foot nail downward");
	lpInfo->m_strPropHelp.Format("direction of the short foot nail nearest to long foot nail(stagger layout later)");
#else
	lpInfo->m_cmbItems = "0.向左|1.向右";
	lpInfo->m_strPropName.Format("脚钉朝向");
	lpInfo->m_strPropHelp.Format("距离长脚钉最近的短脚钉的朝向(以后交错布置)");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("FootNailPara.iDirection");
	if(GetPropValueStr(GetPropID("FootNailPara.iDirection"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_FOOT_NAIL);
	
	//钢板信息
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName.Format("round hole plate info");
	lpInfo->m_strPropHelp.Format("round hole plate information");
#else
	lpInfo->m_strPropName.Format("圆孔钢板信息");
	lpInfo->m_strPropHelp.Format("圆孔钢板信息");
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp=0;
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "plate No.";
	lpInfo->m_strPropHelp.Format("Round hole plate's No. on ladder(that is, the first plate's No. from the top)");
#else
	lpInfo->m_strPropName = "钢板编号";
	lpInfo->m_strPropHelp.Format("爬梯上圆孔钢板的编号(即从上端开始第一个钢板的编号)");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.sPartNo");
	if(GetPropValueStr(GetPropID("PlatePara.sPartNo"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "top distance H";
	lpInfo->m_strPropHelp.Format("plate's top distance (the length between round hole to tube's top");
#else
	lpInfo->m_strPropName = "上端距H";
	lpInfo->m_strPropHelp.Format("钢板的上端距(一般是圆形孔距钢管上端的长度)");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.fUpEndSpace");
	if(GetPropValueStr(GetPropID("PlatePara.fUpEndSpace"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "A";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp.Format("length of round plate A");
#else
	lpInfo->m_strPropHelp.Format("圆形钢板A的长度");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.fCircinalPlateA");
	if(GetPropValueStr(GetPropID("PlatePara.fCircinalPlateA"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "B";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp.Format("length of round plate B");
#else
	lpInfo->m_strPropHelp.Format("圆形钢板B的长度");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.fCircinalPlateB");
	if(GetPropValueStr(GetPropID("PlatePara.fCircinalPlateB"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "thickness";
	lpInfo->m_strPropHelp.Format("thickness of connecting plate");
#else
	lpInfo->m_strPropName = "厚度";
	lpInfo->m_strPropHelp.Format("连接钢板的厚度");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.nPlateThick");
	if(GetPropValueStr(GetPropID("PlatePara.nPlateThick"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "bolt spec";
	lpInfo->m_strPropHelp.Format("connecting bolt's specification on round plate");
#else
	lpInfo->m_strPropName = "螺栓规格";
	lpInfo->m_strPropHelp.Format("圆形钢板连接螺栓的规格");
#endif
	lpInfo->m_cmbItems = "M12|M16|M20|M24";
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.nLSD");
	if(GetPropValueStr(GetPropID("PlatePara.nLSD"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName.Format("waist round hole plate info");
	lpInfo->m_strPropHelp.Format("waist round hole plate information");
#else
	lpInfo->m_strPropName.Format("腰圆孔钢板信息");
	lpInfo->m_strPropHelp.Format("腰圆孔钢板信息");
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp=0;
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "plate No.";
	lpInfo->m_strPropHelp.Format("waist round hole plate No. on ladder");
#else
	lpInfo->m_strPropName = "钢板编号";
	lpInfo->m_strPropHelp.Format("爬梯上腰圆孔钢板的编号");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.sEllipsePartNo");
	if(GetPropValueStr(GetPropID("PlatePara.sEllipsePartNo"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "waist round C";
	lpInfo->m_strPropHelp.Format("length of waist round plate C");
#else
	lpInfo->m_strPropName = "腰圆C";
	lpInfo->m_strPropHelp.Format("腰圆形钢板C的长度");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.fEllipsePlateC");
	if(GetPropValueStr(GetPropID("PlatePara.fEllipsePlateC"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "length of waist round";
	lpInfo->m_strPropHelp.Format("length of the rectangle in the middle of waist round hole");
#else
	lpInfo->m_strPropName = "腰圆长度";
	lpInfo->m_strPropHelp.Format("腰圆孔的中间长方形的长度");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.iEllipseAperture");
	if(GetPropValueStr(GetPropID("PlatePara.iEllipseAperture"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
	lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "distance (E1,E2..)";
	lpInfo->m_strPropHelp.Format("input the distance between plates in turn, and sperate them with comma, (distance to tube's bottom, distance between waist round holes, distance to round hole plate)");
#else
	lpInfo->m_strPropName = "间距(E1,E2..)";
	lpInfo->m_strPropHelp.Format("依次输入钢板之间的距离用逗号隔开(依次是距钢管下端的距离,各腰圆孔钢板间的距离,距圆孔钢板的距离)");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.sBetweenSpace");
	if(GetPropValueStr(GetPropID("PlatePara.sBetweenSpace"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName.Format("basic info of connecting angle");
	lpInfo->m_strPropHelp.Format("basic information of connecting angle");
#else
	lpInfo->m_strPropName.Format("连接角钢基本信息");
	lpInfo->m_strPropHelp.Format("连接角钢基本信息");
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp=0;
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "No.";
	lpInfo->m_strPropHelp.Format("Number of angle which connects ladder and datum tube");
#else
	lpInfo->m_strPropName = "编号";
	lpInfo->m_strPropHelp.Format("连接爬梯和基准钢管的角钢的编号");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.sAnglePartNo");
	if(GetPropValueStr(GetPropID("PlatePara.sAnglePartNo"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "angle length";
	lpInfo->m_strPropHelp.Format("length of angle which connects ladder and datum tube");
#else
	lpInfo->m_strPropName = "角钢长度";
	lpInfo->m_strPropHelp.Format("连接爬梯和基准钢管的角钢的长度");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.fLineAngleLen");
	if(GetPropValueStr(GetPropID("PlatePara.fLineAngleLen"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "bottom distance";
	lpInfo->m_strPropHelp.Format("the distance from the first angle to the datum tube's bottom");
#else
	lpInfo->m_strPropName = "下端距";
	lpInfo->m_strPropHelp.Format("第一个角钢距基准钢管下端的距离");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.fDownEndSpace");
	if(GetPropValueStr(GetPropID("PlatePara.fDownEndSpace"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems = "0.Angle Welding|1.Plate Welding";
	lpInfo->m_strPropName.Format("connecting mode");
	lpInfo->m_strPropHelp.Format("connecting mode of ladder and datum tube");
#else
	lpInfo->m_cmbItems = "0.角钢焊接|1.钢板焊接";
	lpInfo->m_strPropName.Format("连接方式");
	lpInfo->m_strPropHelp.Format("爬梯与基准钢管的连接方式");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.iConnectType");
	if(GetPropValueStr(GetPropID("PlatePara.iConnectType"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "angle spec";
	lpInfo->m_strPropHelp = "specification of connecting angle(width of leg X thickness of leg)";
#else
	lpInfo->m_strPropName = "角钢规格";
	lpInfo->m_strPropHelp = "连接角钢的规格(肢宽X肢厚)";
#endif
	lpInfo->m_cmbItems = MakeAngleSetString();
	lpInfo->m_cmbType = CDT_EDIT;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.specification");
	if(GetPropValueStr(GetPropID("PlatePara.specification"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "angle material";
	lpInfo->m_strPropHelp = "steel material of connecting angle";
#else
	lpInfo->m_strPropName = "角钢材质";
	lpInfo->m_strPropHelp = "连接角钢的材质";
#endif
	lpInfo->m_cmbItems = MakeMaterialMarkSetString();
	lpInfo->m_cmbType = CDT_EDIT;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.cMaterial");
	if(GetPropValueStr(GetPropID("PlatePara.cMaterial"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems = "0.Open Downward|1.Open Upward";
	lpInfo->m_strPropName.Format("connecting mode");
	lpInfo->m_strPropHelp.Format("connecting mode for ladder and datum tube");
#else
	lpInfo->m_cmbItems = "0.开口朝下|1.开口朝上";
	lpInfo->m_strPropName.Format("连接方式");
	lpInfo->m_strPropHelp.Format("爬梯与基准钢管的连接方式");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("PlatePara.iAngleDirect");
	if(GetPropValueStr(GetPropID("PlatePara.iAngleDirect"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLATE);
	//钢管信息
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName.Format("plate info");
	lpInfo->m_strPropHelp.Format("plate information");
#else
	lpInfo->m_strPropName.Format("钢管信息");
	lpInfo->m_strPropHelp.Format("钢管信息");
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp=0;
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_PLACE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "No.";
	lpInfo->m_strPropHelp.Format("Ladder tube's No.");
#else
	lpInfo->m_strPropName = "编号";
	lpInfo->m_strPropHelp.Format("爬梯钢管的编号");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("LineTubePara.sPartNo");
	if(GetPropValueStr(GetPropID("LineTubePara.sPartNo"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLACE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "segment No.";
	lpInfo->m_strPropHelp.Format("ladder segment No.");
#else
	lpInfo->m_strPropName = "段号";
	lpInfo->m_strPropHelp.Format("爬梯的段号");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("LineTubePara.iSeg");
	if(GetPropValueStr(GetPropID("LineTubePara.iSeg"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLACE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "spec";
	lpInfo->m_strPropHelp = "tube's specification(diameterXthickness)";
#else
	lpInfo->m_strPropName = "规格";
	lpInfo->m_strPropHelp = "钢管的规格(直径X厚度)";
#endif
	lpInfo->m_cmbItems = MakeTubeSetString();
	lpInfo->m_cmbType = CDT_EDIT;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("LineTubePara.specification");
	if(GetPropValueStr(GetPropID("LineTubePara.specification"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLACE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
	lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "datum tube";
	lpInfo->m_strPropHelp.Format("datum tube connected with ladder");
#else
	lpInfo->m_strPropName = "基准钢管";
	lpInfo->m_strPropHelp.Format("爬梯连接的基准钢管");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("LineTubePara.pBaseLineTube");
	if(GetPropValueStr(GetPropID("LineTubePara.pBaseLineTube"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLACE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "bottom distance";
	lpInfo->m_strPropHelp.Format("distance from ladder to datum tube's bottom");
#else
	lpInfo->m_strPropName = "下端距";
	lpInfo->m_strPropHelp.Format("爬梯距离基准钢管下端的距离");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("LineTubePara.fDownSpace");
	if(GetPropValueStr(GetPropID("LineTubePara.fDownSpace"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLACE);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "angle";
	lpInfo->m_strPropHelp.Format("angle between ladder and datum edge");
#else
	lpInfo->m_strPropName = "夹角";
	lpInfo->m_strPropHelp.Format("爬梯与基准边之间的夹角");
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CDesignSimpleLadderDlg::GetPropID("LineTubePara.fAngle");
	if(GetPropValueStr(GetPropID("LineTubePara.fAngle"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PLACE);

	m_propList.Redraw();
}

void CDesignSimpleLadderDlg::SelectLineTube()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("please select datum plate:","");
#else
	pCmdLine->FillCmdLine("请选择基准钢管:","");
#endif
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	m_bInernalStart = TRUE;
	CDialog::OnOK();
}

void CDesignSimpleLadderDlg::SetLineTube()
{
	LineTubePara.pBaseLineTube = (CLDSLineTube*)console.FromPartHandle(m_hPickObj,CLS_LINETUBE);
}

int CDesignSimpleLadderDlg::GetPropStr(long id, char *propStr)
{
	if(propStr==NULL)
		return 0;
	//基本信息
	if(GetPropID("FootNailPara.iSeg")==id)
		strcpy(propStr,"FootNailPara.iSeg");
	else if(GetPropID("FootNailPara.fUpEndSpace")==id)
		strcpy(propStr,"FootNailPara.fUpEndSpace");
	else if(GetPropID("FootNailPara.fDownEndSpace")==id)
		strcpy(propStr,"FootNailPara.fDownEndSpace");
	else if(GetPropID("FootNailPara.fBetweenSpace")==id)
		strcpy(propStr,"FootNailPara.fBetweenSpace");
	else if(GetPropID("FootNailPara.nNum")==id)
		strcpy(propStr,"FootNailPara.nNum");
	else if(GetPropID("FootNailPara.shortSpecification")==id)
		strcpy(propStr,"FootNailPara.shortSpecification");
	else if(GetPropID("FootNailPara.longSpecification")==id)
		strcpy(propStr,"FootNailPara.longSpecification");
	else if(GetPropID("FootNailPara.iDirection")==id)
		strcpy(propStr,"FootNailPara.iDirection");
	else if(GetPropID("PlatePara.sPartNo")==id)
		strcpy(propStr,"PlatePara.sPartNo");
	else if(GetPropID("PlatePara.sEllipsePartNo")==id)
		strcpy(propStr,"PlatePara.sEllipsePartNo");
	else if(GetPropID("PlatePara.nLSD")==id)
		strcpy(propStr,"PlatePara.nLSD");
	else if(GetPropID("PlatePara.iSeg")==id)
		strcpy(propStr,"PlatePara.iSeg");
	else if(GetPropID("PlatePara.sBetweenSpace")==id)
		strcpy(propStr,"PlatePara.sBetweenSpace");
	else if(GetPropID("PlatePara.sBetweenSpace")==id)
		strcpy(propStr,"PlatePara.sBetweenSpace");
	else if(GetPropID("PlatePara.fCircinalPlateA")==id)
		strcpy(propStr,"PlatePara.fCircinalPlateA");
	else if(GetPropID("PlatePara.fCircinalPlateB")==id)
		strcpy(propStr,"PlatePara.fCircinalPlateB");
	else if(GetPropID("PlatePara.fEllipsePlateC")==id)
		strcpy(propStr,"PlatePara.fEllipsePlateC");
	else if(GetPropID("PlatePara.fEllipseGuige")==id)
		strcpy(propStr,"PlatePara.fEllipseGuige");
	else if(GetPropID("PlatePara.nPlateThick")==id)
		strcpy(propStr,"PlatePara.nPlateThick");
	else if(GetPropID("PlatePara.fUpEndSpace")==id)
		strcpy(propStr,"PlatePara.fUpEndSpace");
	else if(GetPropID("PlatePara.iAngleDirect")==id)
		strcpy(propStr,"PlatePara.iAngleDirect");
	else if(GetPropID("PlatePara.sAnglePartNo")==id)
		strcpy(propStr,"PlatePara.sAnglePartNo");
	else if(GetPropID("PlatePara.iConnectType"))
		strcpy(propStr,"PlatePara.iConnectType");
	else if(GetPropID("PlatePara.fLineAngleLen")==id)
		strcpy(propStr,"PlatePara.fLineAngleLen");
	else if(GetPropID("PlatePara.fDownEndSpace")==id)
		strcpy(propStr,"PlatePara.fDownEndSpace");
	else if(GetPropID("PlatePara.specification")==id)
		strcpy(propStr,"PlatePara.specification");
	else if(GetPropID("LineTubePara.sPartNo")==id)
		strcpy(propStr,"LineTubePara.sPartNo");
	else if(GetPropID("LineTubePara.cMaterial")==id)
		strcpy(propStr,"LineTubePara.cMaterial");
	else if(GetPropID("LineTubePara.iSeg")==id)
		strcpy(propStr,"LineTubePara.iSeg");
	else if(GetPropID("LineTubePara.pBaseLineTube")==id)
		strcpy(propStr,"LineTubePara.pBaseLineTube");
	else if(GetPropID("LineTubePara.fDownSpace")==id)
		strcpy(propStr,"LineTubePara.fDownSpace");
	else if(GetPropID("LineTubePara.fAngle")==id)
		strcpy(propStr,"LineTubePara.fAngle");
	else if(GetPropID("LineTubePara.specification")==id)
		strcpy(propStr,"LineTubePara.specification");

	return strlen(propStr);
}

int CDesignSimpleLadderDlg::GetPropValueStr(long id, char *valueStr)
{
	CXhChar100 sText;
	//脚钉信息
	if(CDesignSimpleLadderDlg::GetPropID("FootNailPara.fUpEndSpace")==id)
	{	//爬梯钢管总长度
		double fSumLen=PlatePara.fUpEndSpace;
		int uStrLen = strlen(PlatePara.sBetweenSpace);
		CString sTmpPlateOffset;
		for(int i=0;i<uStrLen;i++)
		{
			if(PlatePara.sBetweenSpace[i] == ',')
			{
				fSumLen+=atof(sTmpPlateOffset);
				sTmpPlateOffset = "";
			}
			else
				sTmpPlateOffset+=PlatePara.sBetweenSpace[i];
			if(i == uStrLen-1)
				fSumLen+=atof(sTmpPlateOffset);
		}
		//FootNailPara.fUpEndSpace=钢管爬梯总长度-脚钉下间距-脚钉个数*脚钉间距
		FootNailPara.fUpEndSpace=(fSumLen-FootNailPara.fDownEndSpace)-FootNailPara.nNum*FootNailPara.fBetweenSpace;
		sText.Printf("%f",FootNailPara.fUpEndSpace);
		SimplifiedNumString(sText);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("FootNailPara.fDownEndSpace")==id)
	{
		sText.Printf("%f",FootNailPara.fDownEndSpace);
		SimplifiedNumString(sText);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("FootNailPara.shortSpecification")==id)
	{
		char s_D[200],s_len[200];
		sprintf(s_D,"%d",FootNailPara.D);
		sprintf(s_len,"%f",FootNailPara.fShortLen);
		SimplifiedNumString(s_D);
		SimplifiedNumString(s_len);
		sprintf(sText,"%.1fM%sX%s", FootNailPara.short_grade,s_D,s_len);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("FootNailPara.longSpecification")==id)
	{
		char s_D[200],s_len[200];
		sprintf(s_D,"%d",FootNailPara.D);
		sprintf(s_len,"%f",FootNailPara.fLongLen);
		SimplifiedNumString(s_D);
		SimplifiedNumString(s_len);
		sprintf(sText,"%.1fM%sX%s", FootNailPara.long_grade,s_D,s_len);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("FootNailPara.fBetweenSpace")==id)
	{
		sText.Printf("%f",FootNailPara.fBetweenSpace);
		SimplifiedNumString(sText);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("FootNailPara.nNum")==id)
	{	//根据钢管总长度以及第一个脚钉的下端距、脚钉间距计算脚钉个数
		double fSumLen=PlatePara.fUpEndSpace;
		int uStrLen = strlen(PlatePara.sBetweenSpace);
		CString sTmpPlateOffset;
		for(int i=0;i<uStrLen;i++)
		{
			if(PlatePara.sBetweenSpace[i] == ',')
			{
				fSumLen+=atof(sTmpPlateOffset);
				sTmpPlateOffset = "";
			}
			else
				sTmpPlateOffset+=PlatePara.sBetweenSpace[i];
			if(i == uStrLen-1)
				fSumLen+=atof(sTmpPlateOffset);
		}
		FootNailPara.nNum = ftoi(fSumLen-FootNailPara.fDownEndSpace)/ftoi(FootNailPara.fBetweenSpace);
		sText.Printf("%d",FootNailPara.nNum);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("FootNailPara.iDirection")==id)
	{
		if(FootNailPara.iDirection)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","1.To Right");
		else
			sText.Printf("%s","0.To Left");
#else
			sText.Printf("%s","1.向右");
		else
			sText.Printf("%s","0.向左");
#endif
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.sPartNo")==id)
		sText.Printf("%s",(char*)PlatePara.sPartNo);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.sAnglePartNo")==id)
		sText.Printf("%s",(char*)PlatePara.sAnglePartNo);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.nLSD")==id)
		sprintf(sText,"M%d",PlatePara.nLSD);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.iEllipseAperture")==id)
		sprintf(sText,"%d",PlatePara.iEllipseAperture);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.sEllipsePartNo")==id)
		sText.Printf("%s",(char*)PlatePara.sEllipsePartNo);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.sBetweenSpace")==id)
		sText.Printf("%s",(char*)PlatePara.sBetweenSpace);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.nPlateThick")==id)
		sText.Printf("%d",PlatePara.nPlateThick);
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.fCircinalPlateA")==id)
	{
		sText.Printf("%f",PlatePara.fCircinalPlateA);
		SimplifiedNumString(sText);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.fCircinalPlateB")==id)
	{
		sText.Printf("%f",PlatePara.fCircinalPlateB);
		SimplifiedNumString(sText);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.fEllipsePlateC")==id)
	{
		sText.Printf("%f",PlatePara.fEllipsePlateC);
		SimplifiedNumString(sText);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.fUpEndSpace")==id)
	{
		sText.Printf("%f",PlatePara.fUpEndSpace);
		SimplifiedNumString(sText);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.iAngleDirect")==id)
	{
		if(PlatePara.iAngleDirect)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","1.Open Upward");
		else
			sText.Printf("%s","0.Open Downward");
#else
			sText.Printf("%s","1.开口朝上");
		else
			sText.Printf("%s","0.开口朝下");
#endif
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.fLineAngleLen")==id)
	{
		sText.Printf("%f",PlatePara.fLineAngleLen);
		SimplifiedNumString(sText);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.fDownEndSpace")==id)
	{
		sText.Printf("%f",PlatePara.fDownEndSpace);
		SimplifiedNumString(sText);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.specification")==id)
	{
		char s_wing_wide[200],s_wing_thick[200];
		sprintf(s_wing_wide,"%f",PlatePara.wide);
		sprintf(s_wing_thick,"%f",PlatePara.thick);
		SimplifiedNumString(s_wing_wide);
		SimplifiedNumString(s_wing_thick);
		sprintf(sText,"%sX%s", s_wing_wide,s_wing_thick);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.cMaterial")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(PlatePara.cMaterial,matStr))
			sprintf(sText,"%s",matStr);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("PlatePara.iConnectType")==id)
	{
		if(PlatePara.iConnectType == 0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","0.Angle Welding");
		else
			sText.Printf("%s","1.Plate Welding");
#else
			sText.Printf("%s","0.角钢焊接");
		else
			sText.Printf("%s","1.钢板焊接");
#endif
	}
	else if(CDesignSimpleLadderDlg::GetPropID("LineTubePara.sPartNo")==id)
		sText.Printf("%s",(char*)LineTubePara.sPartNo);
	else if(CDesignSimpleLadderDlg::GetPropID("LineTubePara.iSeg")==id)
		sText.Printf("%s",(char*)LineTubePara.iSeg.ToString());
	else if(CDesignSimpleLadderDlg::GetPropID("LineTubePara.pBaseLineTube")==id)
	{
		if(LineTubePara.pBaseLineTube)
			sText.Printf("0X%X",LineTubePara.pBaseLineTube->handle);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("LineTubePara.fDownSpace")==id)
	{
		sText.Printf("%f",LineTubePara.fDownSpace);
		SimplifiedNumString(sText);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("LineTubePara.fAngle")==id)
	{
		sText.Printf("%f",LineTubePara.fAngle*DEGTORAD_COEF);
		SimplifiedNumString(sText);
	}
	else if(CDesignSimpleLadderDlg::GetPropID("LineTubePara.specification")==id)
	{
		char s_wing_wide[200],s_wing_thick[200];
		sprintf(s_wing_wide,"%f",LineTubePara.wide);
		sprintf(s_wing_thick,"%f",LineTubePara.thick);
		SimplifiedNumString(s_wing_wide);
		SimplifiedNumString(s_wing_thick);
		sprintf(sText,"%sX%s", s_wing_wide,s_wing_thick);
	}

	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}

void CDesignSimpleLadderDlg::RefreshTabCtrl(int iCurSel)
{
	m_ctrlPropGroup.DeleteAllItems();
	for(int i=0;i<m_arrPropGroupLabel.GetSize();i++)
		m_ctrlPropGroup.InsertItem(i,m_arrPropGroupLabel[i]);
	if(m_arrPropGroupLabel.GetSize()>0)
	{
		m_ctrlPropGroup.SetCurSel(iCurSel);
		m_propList.m_iPropGroup=iCurSel;
	}
	else //不需要分组显示
		m_propList.m_iPropGroup=0;
	//根据分组数调整窗口位置
	RECT rcPropList,rcMainWnd;
	GetClientRect(&rcMainWnd);
	CWnd *pTopWnd = GetDlgItem(IDC_LIST_BOX);
	if(pTopWnd)
		pTopWnd->GetWindowRect(&rcPropList);
	ScreenToClient(&rcPropList);
	if(m_arrPropGroupLabel.GetSize()>0)
		rcPropList.top=rcMainWnd.top+35;
	else 
		rcPropList.top=rcMainWnd.top+10;
	if(pTopWnd)
		pTopWnd->MoveWindow(&rcPropList);
}

BOOL CDesignSimpleLadderDlg::CreateSimpleLadder()
{
	double PlateOffsetArr[10];
	int PlateNum=0;//连接板数量
	int i=0;
	//记录连接钢板偏移量的数组
	PlateOffsetArr[0] = PlatePara.fUpEndSpace; 
	CString sTmpPlateOffset;
	double fLastOffset=0;
	int uStrLen = strlen(PlatePara.sBetweenSpace);
	for(i=0;i<uStrLen;i++)
	{
		if(PlatePara.sBetweenSpace[i] == ',')
		{
			fLastOffset+=atof(sTmpPlateOffset);
			PlateOffsetArr[PlateNum] = fLastOffset;
			PlateNum++;
			sTmpPlateOffset = "";
		}
		else
			sTmpPlateOffset+=PlatePara.sBetweenSpace[i];
		if(i == uStrLen-1)
		{
			//加上圆孔钢板
			fLastOffset+=atof(sTmpPlateOffset);
			PlateOffsetArr[PlateNum] = fLastOffset;
			PlateNum++;
			//钢管长度
			fLastOffset+=PlatePara.fUpEndSpace;
			PlateOffsetArr[PlateNum] = fLastOffset;
		}
	}
	//当没有腰圆孔钢板时只统计圆孔钢板
	if(strlen(PlatePara.sBetweenSpace)==0)
	{
		PlateOffsetArr[0] = PlatePara.fUpEndSpace;
		PlateNum++;
		PlateOffsetArr[1] = PlateOffsetArr[0]+PlatePara.fUpEndSpace;
	}
	Ta.BeginUndoListen();
	//创建爬梯钢管
	CLDSLineTube *pLadderLineTube = (CLDSLineTube*)console.AppendPart(CLS_LINETUBE);
	double fVectorLen=0.0;
	LSSPACE_STRU ls_space;
	GetLsSpace(ls_space,PlatePara.nLSD);
	if(PlatePara.iConnectType==0&&PlatePara.iConnectType==0)
	{
		fVectorLen = PlatePara.fLineAngleLen+PlatePara.fCircinalPlateB
					+LineTubePara.pBaseLineTube->GetDiameter()*0.5-ls_space.EndSpace;
	}
	else if(PlatePara.iConnectType==1&&PlatePara.iConnectType==0)
	{
		fVectorLen = PlatePara.fLineAngleLen+PlatePara.fCircinalPlateB*2
			+LineTubePara.pBaseLineTube->GetDiameter()*0.5-ls_space.EndSpace;
	}
	else
	{
		fVectorLen = PlatePara.fLineAngleLen+PlatePara.fCircinalPlateB*2
			+LineTubePara.pBaseLineTube->GetDiameter()*0.5-ls_space.EndSpace*2;
	}
	CreateLineTube(PlateOffsetArr[PlateNum],pLadderLineTube,fVectorLen);
	//创建爬梯钢管上的连接板
	for(i=0;i<PlateNum;i++)
	{
		CLDSLineAngle *pLineAngle=CreateConnectLineAngle(PlateOffsetArr[i]+LineTubePara.fDownSpace);
		if(i==(PlateNum-1))
		{
			CreateConnectPalte(0,PlateOffsetArr[i],pLadderLineTube,pLineAngle);
			if(PlatePara.iConnectType==1)
				CreateConnectPalte(0,PlateOffsetArr[i]+LineTubePara.fDownSpace,LineTubePara.pBaseLineTube,pLineAngle);
		}
		else
		{
			CreateConnectPalte(1,PlateOffsetArr[i],pLadderLineTube,pLineAngle);
			if(PlatePara.iConnectType==1)
				CreateConnectPalte(1,PlateOffsetArr[i]+LineTubePara.fDownSpace,LineTubePara.pBaseLineTube,pLineAngle);
		}
	}
	//生成脚钉
	double fOffsetSpace = FootNailPara.fDownEndSpace;
	CreateFootNail(pLadderLineTube,fOffsetSpace,2);
	for(i=0;i<FootNailPara.nNum;i++)
	{
		int iType=1;
		if(FootNailPara.iDirection == 0)
			iType = 0;
		fOffsetSpace+=FootNailPara.fBetweenSpace;
		if(i%2==iType)
			CreateFootNail(pLadderLineTube,fOffsetSpace,0);
		else
			CreateFootNail(pLadderLineTube,fOffsetSpace,1);
	}
	Ta.EndUndoListen();
	return true;
}

CLDSLineAngle* CDesignSimpleLadderDlg::CreateConnectLineAngle(double fOffsetSpace)
{
	CLDSLineTube *pBaseLineTube = LineTubePara.pBaseLineTube;
	CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
	//基准钢管方向
	pBaseLineTube->BuildUCS();
	f3dPoint tube_len_vec = pBaseLineTube->Start()-pBaseLineTube->End();
	normalize(tube_len_vec);
	pLineAngle->m_bFootNail=TRUE;//表示该角钢为脚钉
	pLineAngle->SetPartNo(PlatePara.sAnglePartNo);//角钢编号
	pLineAngle->m_hPartWeldParent=pBaseLineTube->handle;
	f3dPoint angle_extend_vec;//角钢朝向
	angle_extend_vec = pBaseLineTube->ucs.axis_x;
	normalize(angle_extend_vec);
	RotateVectorAroundVector(angle_extend_vec,LineTubePara.fAngle,pBaseLineTube->ucs.axis_z);
	//建立坐标系
	pLineAngle->ucs.axis_z = angle_extend_vec;
	pLineAngle->ucs.axis_x = tube_len_vec;
	pLineAngle->ucs.axis_y=pLineAngle->ucs.axis_z^pLineAngle->ucs.axis_x;
	normalize(pLineAngle->ucs.axis_y);
	pLineAngle->ucs.axis_x = pLineAngle->ucs.axis_y^pLineAngle->ucs.axis_z;
	normalize(pLineAngle->ucs.axis_x);
	if(PlatePara.iAngleDirect == 0&&PlatePara.iConnectType==0)
	{
		pLineAngle->ucs.origin = pBaseLineTube->End()+angle_extend_vec*pBaseLineTube->GetDiameter()*0.5
							+(fOffsetSpace+PlatePara.wide*0.5)*tube_len_vec+pLineAngle->ucs.axis_y*PlatePara.nPlateThick*0.5;
	}
	else if(PlatePara.iAngleDirect == 1&&PlatePara.iConnectType==0)
	{
		pLineAngle->ucs.origin = pBaseLineTube->End()+angle_extend_vec*pBaseLineTube->GetDiameter()*0.5
							+(fOffsetSpace-PlatePara.wide*0.5)*tube_len_vec+pLineAngle->ucs.axis_y*PlatePara.nPlateThick*0.5;
	}
	else
	{
		LSSPACE_STRU ls_space;
		GetLsSpace(ls_space,PlatePara.nLSD);
		pLineAngle->ucs.origin = pBaseLineTube->End()+angle_extend_vec*(pBaseLineTube->GetDiameter()*0.5
								+(PlatePara.fCircinalPlateB-ls_space.EndSpace))+(fOffsetSpace+PlatePara.wide*0.5)*tube_len_vec
								+pLineAngle->ucs.axis_y*PlatePara.nPlateThick*0.5;
	}
	//肢翼 法线参数
	pLineAngle->set_wing_thick(PlatePara.thick);
	pLineAngle->set_wing_wide(PlatePara.wide);
	pLineAngle->cMaterial = PlatePara.cMaterial;

	pLineAngle->SetStart(pLineAngle->ucs.origin);
	pLineAngle->SetEnd(pLineAngle->ucs.origin+angle_extend_vec*PlatePara.fLineAngleLen);
	pLineAngle->des_norm_y.spec_norm.vector=-pLineAngle->ucs.axis_x;
	pLineAngle->desStartPos.datumPoint.SetPosition(pLineAngle->Start());
	pLineAngle->desEndPos.datumPoint.SetPosition(pLineAngle->End());
	pLineAngle->des_norm_y.bSpecific=TRUE;
	pLineAngle->des_norm_x.bSpecific=FALSE;
	pLineAngle->des_norm_x.bByOtherWing=TRUE;
	//
	pLineAngle->set_norm_x_wing(-pLineAngle->ucs.axis_y);
	pLineAngle->set_norm_y_wing(-pLineAngle->ucs.axis_x);
	//修正肢朝向
	if(PlatePara.iAngleDirect == 0)
	{
		pLineAngle->des_norm_y.spec_norm.vector*=-1;
		DESIGN_JGWING_NORM tempwingnorm=pLineAngle->des_norm_x;
		pLineAngle->des_norm_x = pLineAngle->des_norm_y;
		pLineAngle->des_norm_y = tempwingnorm;
	}
	
	pLineAngle->cal_x_wing_norm();
	pLineAngle->cal_y_wing_norm();
	pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
	g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
	g_pSolidDraw->Draw();

	return pLineAngle;
}

//创建脚钉
BOOL CDesignSimpleLadderDlg::CreateFootNail(CLDSLineTube *pLadderLineTube,double fOffsetSpace,int iType)
{
	double fFootNailLen;
	int fNormOffset,fL0;
	f3dPoint BasePos = pLadderLineTube->End();
	f3dPoint tube_len_vec = pLadderLineTube->Start()-pLadderLineTube->End();
	f3dPoint foot_nail_vec = pLadderLineTube->ucs.axis_x;
	RotateVectorAroundVector(foot_nail_vec,Pi*0.5,pLadderLineTube->ucs.axis_z);
	if(iType==0)
	{	//左侧短脚钉
		fFootNailLen = FootNailPara.fShortLen;
		fNormOffset = -ftoi(pLadderLineTube->GetDiameter()*0.5);
		fL0 = ftoi(pLadderLineTube->GetDiameter());
	}
	else if(iType==1)
	{	//右侧短脚钉
		fFootNailLen = FootNailPara.fShortLen;
		fNormOffset = ftoi(pLadderLineTube->GetDiameter()*0.5);
		fL0 = ftoi(pLadderLineTube->GetDiameter());
	}
	else if(iType==2)
	{	//首颗长脚钉
		fFootNailLen = FootNailPara.fLongLen;
		fNormOffset = -ftoi(FootNailPara.fLongLen*0.5);
		fL0 = ftoi(FootNailPara.fLongLen);
	}

	CLDSBolt *pFootNail = (CLDSBolt*)console.AppendPart(CLS_BOLT);
	pFootNail->layer(2) = pLadderLineTube->layer(2);
	pFootNail->set_d(FootNailPara.D);
	pFootNail->set_L(fFootNailLen);
	pFootNail->AddL0Thick(fL0);
	pFootNail->ucs.axis_z = foot_nail_vec;
	pFootNail->ucs.axis_x = tube_len_vec;
	pFootNail->cfgword=pLadderLineTube->cfgword;	//调整螺栓配材号与基准构件配材号一致

	pFootNail->des_base_pos.datumPoint.datum_pos_style=2;//杆件节点定位
	pFootNail->des_base_pos.hPart=pLadderLineTube->handle;
	pFootNail->des_base_pos.datumPoint.des_para.RODNODE.hRod  = pLadderLineTube->handle;
	pFootNail->des_base_pos.datumPoint.des_para.RODNODE.hNode = pLadderLineTube->pEnd->handle;
	pFootNail->des_base_pos.datumPoint.des_para.RODNODE.direction = 1;//终端向始端偏移
	pFootNail->des_base_pos.datumPoint.des_para.RODNODE.len_offset_dist = fOffsetSpace;
	pFootNail->des_base_pos.norm_offset.AddThick(fNormOffset);

	pFootNail->set_norm(pFootNail->ucs.axis_z);
	pFootNail->des_work_norm.hVicePart = pLadderLineTube->handle;
	if(iType==0||iType==2)
		pFootNail->des_work_norm.nearVector.Set(0,1,0);
	else
		pFootNail->des_work_norm.nearVector.Set(0,-1,0);
	pFootNail->des_work_norm.direction = 0;
	pFootNail->des_work_norm.norm_style = 4;
	pFootNail->m_hFamily=CLsLibrary::GetFootNailFamily();	//脚钉系列
	pFootNail->CalGuigeAuto();
	pFootNail->correct_pos();
	pFootNail->correct_worknorm();
	pFootNail->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),
								g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pFootNail->GetSolidPartObject());
	CLsRef *pLineTubeLsRef=pLadderLineTube->AppendMidLsRef(pFootNail->GetLsRef());//添加爬梯钢管螺栓引用
	return TRUE;
}

//创建爬梯钢管
BOOL CDesignSimpleLadderDlg::CreateLineTube(double fLineTubeLen,CLDSLineTube *pLineTube,double fVectorLen)
{
	//基准钢管
	CLDSLineTube *pBaseLineTube = LineTubePara.pBaseLineTube;
	f3dPoint tube_len_vec = pBaseLineTube->Start()-pBaseLineTube->End();
	normalize(tube_len_vec);
	f3dPoint plate_extend_vec;//钢板延伸方向
	pBaseLineTube->BuildUCS();
	plate_extend_vec = pBaseLineTube->ucs.axis_x;
	normalize(plate_extend_vec);
	RotateVectorAroundVector(plate_extend_vec,LineTubePara.fAngle,pBaseLineTube->ucs.axis_z);
	//段号 构件号 图层
	pLineTube->iSeg = LineTubePara.iSeg;
	pLineTube->SetPartNo(LineTubePara.sPartNo);
	pLineTube->layer(2) = pBaseLineTube->layer(2);
	//设置钢管规格
	pLineTube->size_wide = LineTubePara.wide;
	pLineTube->size_thick = LineTubePara.thick;
	pLineTube->CalPosition();
	//定位钢管的开始和结束节点
	pLineTube->pStart = console.AppendNode();
	pLineTube->pEnd = console.AppendNode();
	pLineTube->pStart->m_cPosCalType = 0;
	pLineTube->pEnd->m_cPosCalType = 0;
	f3dPoint TmpPoint;
	TmpPoint = pBaseLineTube->End();
	fVectorLen += pLineTube->GetDiameter()*0.5;
	TmpPoint+=fVectorLen*plate_extend_vec;
	pLineTube->pEnd->SetPosition( TmpPoint+LineTubePara.fDownSpace*tube_len_vec);
	pLineTube->pStart->SetPosition( pLineTube->pEnd->Position(false)+fLineTubeLen*tube_len_vec);
	pLineTube->pStart->layer(2) = pBaseLineTube->layer(2);
	pLineTube->pEnd->layer(2) = pBaseLineTube->layer(2);
	NewNode(pLineTube->pStart);
	NewNode(pLineTube->pEnd);
	//基准线
	RotateVectorAroundVector(plate_extend_vec,Pi*0.5,pBaseLineTube->ucs.axis_z);
	pLineTube->SetWorkPlaneNorm(-plate_extend_vec);
	//开始,结束节点
	pLineTube->SetStart(pLineTube->pStart->Position(true));
	pLineTube->SetEnd(pLineTube->pEnd->Position(true));
 
	pLineTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),
								g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pLineTube->GetSolidPartObject());
	g_pSolidDraw->Draw();

	return TRUE;
}

//创建连接板
BOOL CDesignSimpleLadderDlg::CreateConnectPalte(int iPlateType,double fOffsetSpace,CLDSLineTube *pBaseLineTube,CLDSLineAngle *pLineAngle)
{	
	CLDSPlate *pPlate = (CLDSPlate*)console.AppendPart(CLS_PLATE);
	pPlate->cfgword=pBaseLineTube->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
	pPlate->m_hPartWeldParent=pBaseLineTube->handle;
	f3dPoint tube_len_vec = pBaseLineTube->Start()-pBaseLineTube->End();
	normalize(tube_len_vec);
	f3dPoint plate_extend_vec;//钢板延伸方向
	pBaseLineTube->BuildUCS();
	plate_extend_vec = pBaseLineTube->ucs.axis_x;
	normalize(plate_extend_vec);
	if(pBaseLineTube->handle == LineTubePara.pBaseLineTube->handle)
		RotateVectorAroundVector(plate_extend_vec,LineTubePara.fAngle,pBaseLineTube->ucs.axis_z);
	//建立坐标系
	pPlate->ucs.origin = pBaseLineTube->End()+plate_extend_vec*pBaseLineTube->GetDiameter()*0.5+fOffsetSpace*tube_len_vec;
	pPlate->ucs.axis_y = tube_len_vec;
	pPlate->ucs.axis_x = plate_extend_vec;
	pPlate->ucs.axis_z = pPlate->ucs.axis_x^pPlate->ucs.axis_y;
	normalize(pPlate->ucs.axis_z);
	pPlate->ucs.axis_y = pPlate->ucs.axis_z^pPlate->ucs.axis_x;
	normalize(pPlate->ucs.axis_y);
	//坐标原点偏移半个板厚
	pPlate->Thick=PlatePara.nPlateThick;
	pPlate->ucs.origin-=pPlate->ucs.axis_z*pPlate->GetThick()*0.5;
	//钢板各边长度 __ __ fUpLeft fUpRight
	//	  fleftUp |
	//	fleftDown |
	double fLeftUp,fLeftDown,fUpLeft,fUpRight;
	if(iPlateType == 0)
	{	//圆孔钢板
		fLeftDown=fLeftUp=fUpLeft=PlatePara.fCircinalPlateA;
		fUpRight = PlatePara.fCircinalPlateB;
		//编号
		pPlate->SetPartNo(PlatePara.sPartNo);
	}
	else
	{	//腰圆孔
		fLeftDown=fLeftUp=PlatePara.fEllipsePlateC;
		fUpLeft = PlatePara.fCircinalPlateA;
		fUpRight = PlatePara.fCircinalPlateB;
		//编号
		pPlate->SetPartNo(PlatePara.sEllipsePartNo);
	}
	//段号 图层名
	pPlate->iSeg = LineTubePara.iSeg;
	pPlate->layer(2) = pBaseLineTube->layer(2);
	//添加顶点
	PROFILE_VERTEX *pPlateVertex;
	f3dPoint vector_x,vector_y;
	vector_x.Set(1,0,0);
	vector_y.Set(0,1,0);
	f3dPoint tmpPoint;

	pPlateVertex = pPlate->vertex_list.append();
	tmpPoint = -fLeftDown*vector_y;
	pPlateVertex->vertex = tmpPoint;
	pPlateVertex->vertex.z=0;
	pPlateVertex->vertex.feature = 1;

	pPlateVertex = pPlate->vertex_list.append();
	tmpPoint+=(fUpRight+fUpLeft)*vector_x;
	pPlateVertex->vertex = tmpPoint;
	pPlateVertex->vertex.z=0;
	pPlateVertex->vertex.feature = 1;

	pPlateVertex = pPlate->vertex_list.append();
	tmpPoint+=(fLeftUp+fLeftDown)*vector_y;
	pPlateVertex->vertex = tmpPoint;
	pPlateVertex->vertex.z=0;
	pPlateVertex->vertex.feature = 1;

	pPlateVertex = pPlate->vertex_list.append();
	pPlateVertex->vertex = tmpPoint-(fUpRight+fUpLeft)*vector_x;
	pPlateVertex->vertex.z=0;
	pPlateVertex->vertex.feature = 1;

	pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),
								g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());

	//添加螺栓
	CLDSBolt *pBolt = (CLDSBolt*)console.AppendPart(CLS_BOLT);
	pBolt->layer(2) = pPlate->layer(2);
	pBolt->set_d(PlatePara.nLSD);
	pBolt->cfgword=pPlate->cfgword;	//调整螺栓配材号与基准构件配材号一致
	pBolt->des_base_pos.hPart = pPlate->handle;
	pBolt->des_base_pos.datumPoint.datum_pos_style = 9;
	pBolt->des_base_pos.datumPoint.des_para.hPart = pPlate->handle;
	pBolt->des_base_pos.datumPoint.datum_pos.Set(fUpRight);
	
	pBolt->des_work_norm.norm_style = 4;
	pBolt->des_work_norm.direction = 0;
	pBolt->des_work_norm.hVicePart = pPlate->handle;
	if(pBaseLineTube->handle == LineTubePara.pBaseLineTube->handle)
		pBolt->des_work_norm.nearVector.Set(0,0,1);
	else
	{
		pBolt->des_work_norm.nearVector.Set(0,0,-1);
		pBolt->des_base_pos.norm_offset.AddThick(-pPlate->handle,TRUE);
	}

	pBolt->ucs.axis_x = pPlate->ucs.axis_y;
	pBolt->ucs.axis_z = pPlate->ucs.axis_z;
	pBolt->AddL0Thick(pPlate->handle,TRUE);
	pBolt->AddL0Thick(pLineAngle->handle,TRUE);
	pBolt->CalGuigeAuto();
	pBolt->correct_worknorm();
	pBolt->correct_pos();
	pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),
								g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	CLsRef *pLsRef = pPlate->AppendLsRef(pBolt->GetLsRef());
	CLsRef *pAngleLsRef = pLineAngle->AppendEndLsRef(pBolt->GetLsRef());
	if(iPlateType == 1)
		pLsRef->waistLen = PlatePara.iEllipseAperture;
	g_pSolidDraw->Draw();

	return true;
}

void CDesignSimpleLadderDlg::OnConfirmCreate() 
{
	if(LineTubePara.pBaseLineTube==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("no datum tube selected，quit or not");
#else
		AfxMessageBox("没有选中基准钢管，是否退出");
#endif
		return;
	}
	CreateSimpleLadder();
#ifdef AFX_TARG_ENU_ENGLISH
	SetCmdPickPromptStr("please contiue to the datum tube needed to design ladder:");
#else
	SetCmdPickPromptStr("请继续选择要设计爬梯的基准钢管:");
#endif
	SelectLineTube();
}

//标注构件尺寸 
//iOffsetType -1.X轴负方向 -2.Y轴负方向 1.X轴正方向 2.Y轴正方向
static void DimText(CPaintDC *pDc,f2dPoint start,f2dPoint end,CString sLen,
					int iOffsetType,int nOffsetDist=0)
{
	SimplifiedNumString(sLen);
	CSize sizeText=pDc->GetTextExtent(sLen);
	int nSpaceDist=5;	//标注文字与轮廓边之间的间隙值
	f2dPoint dim_pos,dim_start,dim_end,mid_start,mid_end,ext_start,ext_end;
	dim_start=start; 
	dim_end=end;
	if(abs(iOffsetType)==1)
	{	//垂直标注
		if(nOffsetDist==0)	//标注位置相对于标注点的偏移距离
			nOffsetDist=ftoi(sizeText.cx*0.5)+4;
		dim_start.x+=(iOffsetType*nOffsetDist);
		dim_end.x+=(iOffsetType*nOffsetDist);
		//
		ext_start=dim_start;
		ext_end=dim_end;
		ext_start.x+=(iOffsetType*sizeText.cy*0.25);
		ext_end.x+=(iOffsetType*sizeText.cy*0.25);
		dim_pos.Set(dim_start.x,0.5*(dim_start.y+dim_end.y));
		//增加两个标注点空出文字空间
		int nFlag=1;
		if(dim_start.y<dim_end.y)
			nFlag=-1;
		mid_start.Set(dim_start.x,dim_pos.y+0.6*sizeText.cy*nFlag);
		mid_end.Set(dim_start.x,dim_pos.y-0.6*sizeText.cy*nFlag);
	}
	else if(abs(iOffsetType)==2)
	{	//水平标注
		iOffsetType/=2;	//除以2
		if(nOffsetDist==0)	//标注位置相对于标注点的偏移距离
			nOffsetDist=ftoi(sizeText.cy*0.6);
		dim_start.y+=(iOffsetType*nOffsetDist);
		dim_end.y+=(iOffsetType*nOffsetDist);
		dim_pos.Set(0.5*(dim_start.x+dim_end.x),dim_start.y);
		//
		ext_start=dim_start;
		ext_end=dim_end;
		ext_start.y+=(iOffsetType*sizeText.cy*0.25);
		ext_end.y+=(iOffsetType*sizeText.cy*0.25);
		//增加两个标注点空出文字空间
		int nFlag=1;
		if(dim_start.x<dim_end.x)
			nFlag=-1;
		mid_start.Set(dim_pos.x+0.6*sizeText.cx*nFlag,dim_start.y);
		mid_end.Set(dim_pos.x-0.6*sizeText.cx*nFlag,dim_start.y);
		iOffsetType*=2;	//乘以2
	}
	//绘制标注线
	pDc->MoveTo(ftoi(start.x),ftoi(start.y));
	pDc->LineTo(ftoi(ext_start.x),ftoi(ext_start.y));
	pDc->MoveTo(ftoi(end.x),ftoi(end.y));
	pDc->LineTo(ftoi(ext_end.x),ftoi(ext_end.y));
	//
	int nDimSpace=(int)DISTANCE(f3dPoint(dim_start.x,dim_start.y,0),f3dPoint(dim_end.x,dim_end.y,0));
	if(nDimSpace<sizeText.cx)
	{
		pDc->MoveTo(ftoi(dim_start.x),ftoi(dim_start.y));
		pDc->LineTo(ftoi(dim_end.x),ftoi(dim_end.y));
		//绘制标注内容
		dim_pos.x+=sizeText.cx;
		if(abs(iOffsetType)==2)
			pDc->TextOut(ftoi(dim_pos.x-1.5*sizeText.cx),ftoi(dim_pos.y-sizeText.cy),sLen);
		else
			pDc->TextOut(ftoi(dim_pos.x-0.5*sizeText.cx),ftoi(dim_pos.y-sizeText.cy*0.5),sLen);
	}
	else
	{
		pDc->MoveTo(ftoi(dim_start.x),ftoi(dim_start.y));
		pDc->LineTo(ftoi(mid_start.x),ftoi(mid_start.y));
		pDc->MoveTo(ftoi(mid_end.x),ftoi(mid_end.y));
		pDc->LineTo(ftoi(dim_end.x),ftoi(dim_end.y));
		//绘制标注内容
		pDc->TextOut(ftoi(dim_pos.x-0.5*sizeText.cx),ftoi(dim_pos.y-sizeText.cy*0.5),sLen);
	}
}

void CDesignSimpleLadderDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rectDrawRgn; //当前绘图区域
	CWnd *pWnd = GetDlgItem(IDC_S_MAP);
	pWnd->GetWindowRect(&rectDrawRgn);
	ScreenToClient(&rectDrawRgn);
	int width=rectDrawRgn.Width();
	int height=rectDrawRgn.Height();
	double lineTube_Len=FootNailPara.fBetweenSpace*FootNailPara.nNum+FootNailPara.fDownEndSpace+FootNailPara.fUpEndSpace
			,lineTube_height=20,scale=1;
	double fFootNailLen = FootNailPara.fShortLen;
	double scale_x=(width*0.7)/lineTube_Len;
	double scale_y=(height*0.2)/fFootNailLen;
	lineTube_Len *= scale_x;
	fFootNailLen *= scale_y;
	double T=5;
	//
	UCS_2dSTRU draw_ucs;	//绘图坐标系
	draw_ucs.origin.Set(rectDrawRgn.BottomRight().x-rectDrawRgn.Width()+40,rectDrawRgn.BottomRight().y-10);
	draw_ucs.axis_x.Set(1,0);
	draw_ucs.axis_y.Set(0,-1);
	scale=scale_x;//min(scale_x,scale_y);
	dc.SelectStockObject(LTGRAY_BRUSH); 
	dc.Rectangle(rectDrawRgn);
	dc.SelectStockObject(HOLLOW_BRUSH);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0,0,0));
	//开始绘制
	f2dPoint origin=draw_ucs.origin;
	int axis_y_len=ftoi(100*scale)+3;
	int axis_x_len=axis_y_len;
	//绘制钢管
	dc.MoveTo(ftoi(origin.x),ftoi(origin.y-fFootNailLen+lineTube_height));
	dc.LineTo(ftoi(origin.x+lineTube_Len),ftoi(origin.y-fFootNailLen+lineTube_height));

	dc.MoveTo(ftoi(origin.x),ftoi(origin.y-fFootNailLen+lineTube_height));
	dc.LineTo(ftoi(origin.x),ftoi(origin.y-fFootNailLen));

	dc.MoveTo(ftoi(origin.x),ftoi(origin.y-fFootNailLen));
	dc.LineTo(ftoi(origin.x+lineTube_Len),ftoi(origin.y-fFootNailLen));
	
	dc.MoveTo(ftoi(origin.x+lineTube_Len),ftoi(origin.y-fFootNailLen+lineTube_height));
	dc.LineTo(ftoi(origin.x+lineTube_Len),ftoi(origin.y-fFootNailLen));
	//长脚钉
	CPen *pOldPen=dc.SelectObject(&purple_solid_pen);
	dc.MoveTo(ftoi(origin.x+FootNailPara.fDownEndSpace*scale_x),ftoi(origin.y-fFootNailLen+lineTube_height));
	dc.LineTo(ftoi(origin.x+FootNailPara.fDownEndSpace*scale_x),ftoi(origin.y));

	dc.MoveTo(ftoi(origin.x+FootNailPara.fDownEndSpace*scale_x),ftoi(origin.y));
	dc.LineTo(ftoi(origin.x+FootNailPara.fDownEndSpace*scale_x+5),ftoi(origin.y));

	dc.MoveTo(ftoi(origin.x+FootNailPara.fDownEndSpace*scale_x+5),ftoi(origin.y));
	dc.LineTo(ftoi(origin.x+FootNailPara.fDownEndSpace*scale_x+5),ftoi(origin.y-fFootNailLen+lineTube_height));

	dc.MoveTo(ftoi(origin.x+FootNailPara.fDownEndSpace*scale_x),ftoi(origin.y-fFootNailLen));
	dc.LineTo(ftoi(origin.x+FootNailPara.fDownEndSpace*scale_x),ftoi(origin.y-fFootNailLen*2+lineTube_height));
	
	dc.MoveTo(ftoi(origin.x+FootNailPara.fDownEndSpace*scale_x),ftoi(origin.y-fFootNailLen*2+lineTube_height));
	dc.LineTo(ftoi(origin.x+FootNailPara.fDownEndSpace*scale_x+5),ftoi(origin.y-fFootNailLen*2+lineTube_height));
	
	dc.MoveTo(ftoi(origin.x+FootNailPara.fDownEndSpace*scale_x+5),ftoi(origin.y-fFootNailLen*2+lineTube_height));
	dc.LineTo(ftoi(origin.x+FootNailPara.fDownEndSpace*scale_x+5),ftoi(origin.y-fFootNailLen));
	
	f2dPoint dim_start,dim_end;
	dim_start.Set(ftoi(origin.x+FootNailPara.fDownEndSpace*scale_x),ftoi(origin.y));
	dim_end.Set(ftoi(origin.x+FootNailPara.fDownEndSpace*scale_x),ftoi(origin.y-fFootNailLen*2+lineTube_height));
	dc.SelectObject(red_solid_pen);
	CString text;
	text.Format("%f",FootNailPara.fLongLen);
	DimText(&dc,dim_start,dim_end,text,-1);
	dc.SelectObject(purple_solid_pen);
	int	x_between = ftoi(lineTube_Len/8);
	int x_offset = 0;
	int y_offset = 0,y1_offset = 0;
	int i;
	for(i=1;i<8;i++)
	{
		int iType=1;
		if(FootNailPara.iDirection == 0)
			iType = 0;
		if(i%2==iType)
		{ 
			x_offset = ftoi(x_between*i+FootNailPara.fDownEndSpace*scale_x)+5;
			dc.MoveTo(ftoi(origin.x+x_offset),ftoi(origin.y-fFootNailLen+lineTube_height));
			dc.LineTo(ftoi(origin.x+x_offset),ftoi(origin.y));
			
			dc.MoveTo(ftoi(origin.x+x_offset),ftoi(origin.y));
			dc.LineTo(ftoi(origin.x+x_offset+5),ftoi(origin.y));
			
			dc.MoveTo(ftoi(origin.x+x_offset+5),ftoi(origin.y));
			dc.LineTo(ftoi(origin.x+x_offset+5),ftoi(origin.y-fFootNailLen+lineTube_height));
			if(i==7)
			{
				y_offset = ftoi(origin.y-fFootNailLen+lineTube_height);
				y1_offset = ftoi(origin.y);
			}
		}
		else
		{
			x_offset = ftoi(x_between*i+FootNailPara.fDownEndSpace*scale_x)+5;
			dc.MoveTo(ftoi(origin.x+x_offset),ftoi(origin.y-fFootNailLen));
			dc.LineTo(ftoi(origin.x+x_offset),ftoi(origin.y-fFootNailLen*2+lineTube_height));
			
			dc.MoveTo(ftoi(origin.x+x_offset),ftoi(origin.y-fFootNailLen*2+lineTube_height));
			dc.LineTo(ftoi(origin.x+x_offset+5),ftoi(origin.y-fFootNailLen*2+lineTube_height));
			
			dc.MoveTo(ftoi(origin.x+x_offset+5),ftoi(origin.y-fFootNailLen*2+lineTube_height));
			dc.LineTo(ftoi(origin.x+x_offset+5),ftoi(origin.y-fFootNailLen));
			if(i==7)
			{
				y_offset = ftoi(origin.y-fFootNailLen);
				y1_offset = ftoi(origin.y-fFootNailLen*2+lineTube_height);
			}
		}
	}
	dc.SelectObject(red_solid_pen);
	dim_start.Set(origin.x+lineTube_Len,y_offset);
	dim_end.Set(origin.x+lineTube_Len,y1_offset);
	text.Format("%f",FootNailPara.fShortLen);
	DimText(&dc,dim_start,dim_end,text,1);
	int line_len=ftoi(lineTube_Len/14);
	for(i=0;i<7;i++)
	{
		x_offset = line_len*i*2;
		dc.SelectObject(red_solid_pen);
		dc.MoveTo(ftoi(origin.x+x_offset),ftoi(origin.y-fFootNailLen+2));
		dc.LineTo(ftoi(origin.x+x_offset+line_len),ftoi(origin.y-fFootNailLen+2));
		dc.MoveTo(ftoi(origin.x+x_offset),ftoi(origin.y-fFootNailLen+lineTube_height-2));
		dc.LineTo(ftoi(origin.x+x_offset+line_len),ftoi(origin.y-fFootNailLen+lineTube_height-2));
		dc.SelectObject(green_dash_pen);
		dc.MoveTo(ftoi(origin.x+x_offset-line_len*0.5),ftoi(origin.y-fFootNailLen+lineTube_height*0.5));
		dc.LineTo(ftoi(origin.x+x_offset+line_len*0.5),ftoi(origin.y-fFootNailLen+lineTube_height*0.5));
	}
	x_offset = line_len*7*2;
	dc.MoveTo(ftoi(origin.x+x_offset-line_len*0.5),ftoi(origin.y-fFootNailLen+lineTube_height*0.5));
	dc.LineTo(ftoi(origin.x+x_offset+line_len*0.5),ftoi(origin.y-fFootNailLen+lineTube_height*0.5));
	//标注脚钉
	dc.SelectObject(red_solid_pen);
	x_offset = ftoi(origin.x+FootNailPara.fDownEndSpace*scale_x)+2;
	dim_start.Set(x_offset,ftoi(origin.y-fFootNailLen*2+lineTube_height));
	x_offset = ftoi(line_len*6*2+5+line_len*0.5);
	dim_end.Set(ftoi(origin.x+x_offset),ftoi(origin.y-fFootNailLen*2+lineTube_height));
	text.Format("%f",FootNailPara.fBetweenSpace);
	SimplifiedNumString(text);
	CString tmpText;
	tmpText.Format("%s*%d",text,FootNailPara.nNum);
	DimText(&dc,dim_start,dim_end,tmpText,-2);
	//标注终端
	dim_start.Set(ftoi(origin.x+x_offset),ftoi(origin.y-fFootNailLen*2+lineTube_height));
	x_offset = ftoi(lineTube_Len);
	dim_start.Set(ftoi(origin.x+x_offset),ftoi(origin.y-fFootNailLen*2+lineTube_height));
	tmpText.Format("%f",FootNailPara.fUpEndSpace);
	DimText(&dc,dim_start,dim_end,tmpText,-2);
	//绘制钢管
	dc.SelectObject(pOldPen);
	origin.y-=100;
	dc.MoveTo(ftoi(origin.x),ftoi(origin.y-fFootNailLen+lineTube_height));
	dc.LineTo(ftoi(origin.x+lineTube_Len),ftoi(origin.y-fFootNailLen+lineTube_height));
	
	dc.MoveTo(ftoi(origin.x),ftoi(origin.y-fFootNailLen+lineTube_height));
	dc.LineTo(ftoi(origin.x),ftoi(origin.y-fFootNailLen));
	
	dc.MoveTo(ftoi(origin.x),ftoi(origin.y-fFootNailLen));
	dc.LineTo(ftoi(origin.x+lineTube_Len),ftoi(origin.y-fFootNailLen));
	
	dc.MoveTo(ftoi(origin.x+lineTube_Len),ftoi(origin.y-fFootNailLen+lineTube_height));
	dc.LineTo(ftoi(origin.x+lineTube_Len),ftoi(origin.y-fFootNailLen));
	line_len=ftoi(lineTube_Len/14);
	for(i=0;i<7;i++)
	{
		x_offset = line_len*i*2;
		dc.SelectObject(red_solid_pen);
		dc.MoveTo(ftoi(origin.x+x_offset),ftoi(origin.y-fFootNailLen+2));
		dc.LineTo(ftoi(origin.x+x_offset+line_len),ftoi(origin.y-fFootNailLen+2));
		dc.MoveTo(ftoi(origin.x+x_offset),ftoi(origin.y-fFootNailLen+lineTube_height-2));
		dc.LineTo(ftoi(origin.x+x_offset+line_len),ftoi(origin.y-fFootNailLen+lineTube_height-2));
		dc.SelectObject(green_dash_pen);
		dc.MoveTo(ftoi(origin.x+x_offset-line_len*0.5),ftoi(origin.y-fFootNailLen+lineTube_height*0.5));
		dc.LineTo(ftoi(origin.x+x_offset+line_len*0.5),ftoi(origin.y-fFootNailLen+lineTube_height*0.5));
	}
	x_offset = line_len*7*2;
	dc.MoveTo(ftoi(origin.x+x_offset-line_len*0.5),ftoi(origin.y-fFootNailLen+lineTube_height*0.5));
	dc.LineTo(ftoi(origin.x+x_offset+line_len*0.5),ftoi(origin.y-fFootNailLen+lineTube_height*0.5));

	//标注板间距
	CString sTmpPlateOffset;
	double fLastOffset=0;
	int nStrLen = PlatePara.sBetweenSpace.GetLength();
	y_offset = ftoi(origin.y-fFootNailLen);
	int y2_offset = y_offset-ftoi((PlatePara.fCircinalPlateA+PlatePara.fCircinalPlateB)*scale_y);
	//相对于钢管长度板宽太小 所以给个固定板宽20
	for(i=0;i<nStrLen;i++)
	{
		if(PlatePara.sBetweenSpace[i] == ',')
		{
			dc.SelectObject(red_solid_pen);
			if(fLastOffset==0)
				dim_start.Set(ftoi(origin.x+fLastOffset),y2_offset);
			else
				dim_start.Set(ftoi(origin.x+fLastOffset+10),y2_offset);
			fLastOffset+=(atoi(sTmpPlateOffset)*scale_x);
			dim_end.Set(ftoi(origin.x+fLastOffset+10),y2_offset);
			DimText(&dc,dim_start,dim_end,sTmpPlateOffset,-2);
			
			dc.SelectObject(pOldPen);
			dc.MoveTo(ftoi(origin.x+fLastOffset),y_offset);
			dc.LineTo(ftoi(origin.x+fLastOffset),y2_offset);

			dc.MoveTo(ftoi(origin.x+fLastOffset),y2_offset);
			dc.LineTo(ftoi(origin.x+fLastOffset+20),y2_offset);

			dc.MoveTo(ftoi(origin.x+fLastOffset+20),y_offset);
			dc.LineTo(ftoi(origin.x+fLastOffset+20),y2_offset);
			sTmpPlateOffset = "";
		}
		else
			sTmpPlateOffset+=PlatePara.sBetweenSpace[i];
		if(i == nStrLen-1)
		{
			//加上圆孔钢板  
			dc.SelectObject(red_solid_pen);
			dim_start.Set(ftoi(origin.x+fLastOffset+10),y2_offset);
			fLastOffset+=(atoi(sTmpPlateOffset)*scale_x);
			dim_end.Set(ftoi(origin.x+fLastOffset+5),y2_offset);
			//text.Format("%f",sTmpPlateOffset)
			DimText(&dc,dim_start,dim_end,sTmpPlateOffset,-2);
			
			dc.SelectObject(pOldPen);
			dc.MoveTo(ftoi(origin.x+fLastOffset),y_offset);
			dc.LineTo(ftoi(origin.x+fLastOffset),y2_offset);
			
			dc.MoveTo(ftoi(origin.x+fLastOffset),y2_offset);
			dc.LineTo(ftoi(origin.x+fLastOffset+10),y2_offset);
			
			dc.MoveTo(ftoi(origin.x+fLastOffset+10),y_offset);
			dc.LineTo(ftoi(origin.x+fLastOffset+10),y2_offset);
		}
	}
}

void CDesignSimpleLadderDlg::SetCmdPickPromptStr(CString sPromptStr)
{
	m_sCmdPickPrompt=sPromptStr;
}

void CDesignSimpleLadderDlg::ReDraw()
{
	Invalidate(FALSE);
}
#endif