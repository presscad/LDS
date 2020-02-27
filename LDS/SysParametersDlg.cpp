//<LOCALE_TRANSLATE Confirm by hxr/>
// SysParametersDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "SysParametersDlg.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "./Codes/Codes.h"
#include "EarthQuakeTgSelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(__TSA_)||defined(__LDS_)
/////////////////////////////////////////////////////////////////////////////
// CSysParametersDlg dialog

/*static void UpdateWindBetaZForDesign(CSuperGridCtrl* pListCtrl)
{
	if(g_sysPara.m_iLoadExecuteCode==0)
	{
		CString sText;
		CSuperGridCtrl::CTreeItem *pItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("m_fWindBetaZForDesign"));
		sText.Format("%f",g_sysPara.Beta_z_Design());
		if(pItem)
		{
			pItem->m_lpNodeInfo->SetSubItemText(1,sText);
			pListCtrl->SetItemText(pItem->GetIndex(),1,sText);
			pListCtrl->RedrawItems(pItem->GetIndex(),pItem->GetIndex());
		}
	}
}*/
static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
							int iSubItem,CString& sTextValue)
{
	pListCtrl->SetSubItemText(pSelItem,iSubItem,sTextValue);
	if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.m_fTowerHeight"))
	{
		Ta.m_fTowerHeight=atof(sTextValue);
		//UpdateWindBetaZForDesign(pListCtrl);
	}
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.m_fIceDensity"))
		Ta.m_fIceDensity=atof(sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.m_fVibrationT1"))
		Ta.m_fVibrationT1=atof(sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.m_siSafetyGrade"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(sTextValue.CompareNoCase("Level 1:Very important rod tower")==0)
#else
		if(sTextValue.CompareNoCase("一级:特别重要的杆塔")==0)
#endif
		{
			Ta.m_siSafetyGrade=0;
			Ta.m_fSafetyCoef=1.1;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(sTextValue.CompareNoCase("Level 2:Common various rod tower")==0)
#else
		else if(sTextValue.CompareNoCase("二级:一般的各类杆塔")==0)
#endif
		{
			Ta.m_siSafetyGrade=1;
			Ta.m_fSafetyCoef=1.0;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(sTextValue.CompareNoCase("Level 3:Temporarily used rod tower")==0)
#else
		else if(sTextValue.CompareNoCase("三级:临时使用的杆塔")==0)
#endif
		{
			Ta.m_siSafetyGrade=2;
			Ta.m_fSafetyCoef=0.9;
		}
		else
		{
			Ta.m_siSafetyGrade=3;
			Ta.m_fSafetyCoef=1.0;
		}
		CString sValue;
		sValue.Format("%.1f",Ta.m_fSafetyCoef);
		CSuperGridCtrl::CTreeItem* pItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.m_fSafetyCoef"));
		if(pItem)
			pListCtrl->SetSubItemText(pItem,1,sValue);
		if(Ta.m_siSafetyGrade==3)
			pListCtrl->SetCellReadOnly(pItem->GetIndex(),1,FALSE);
		else
			pListCtrl->SetCellReadOnly(pItem->GetIndex(),1,TRUE);
	}
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.m_fSafetyCoef"))
	{
		Ta.m_fSafetyCoef=atof(sTextValue);
	}
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("m_iLoadExecuteCode"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(sTextValue.CompareNoCase("110~750kV transmission design specification")==0)
			Ta.m_iLoadExecuteCode=0;
		else if(sTextValue.CompareNoCase("high-rise structure design specification GB50135-2006")==0)
			Ta.m_iLoadExecuteCode=1;
		else if(sTextValue.CompareNoCase("building constructed load specification GB50009-2001")==0)
			Ta.m_iLoadExecuteCode=2;
#else
		if(sTextValue.CompareNoCase("110~750kV输电线路设计规定")==0)
			Ta.m_iLoadExecuteCode=0;
		else if(sTextValue.CompareNoCase("高耸结构设计规范 GB50135-2006")==0)
			Ta.m_iLoadExecuteCode=1;
		else if(sTextValue.CompareNoCase("建筑结构荷载规范 GB50009-2001")==0)
			Ta.m_iLoadExecuteCode=2;
		else if(sTextValue.CompareNoCase("架空输电线路荷载规范 DL/T5551-2018")==0)
			Ta.m_iLoadExecuteCode=4;
#endif
		else //if(sTextValue.CompareNoCase("用户指定")==0)
			Ta.m_iLoadExecuteCode=3;
		if(Ta.m_iLoadExecuteCode==3)
		{
			CSuperGridCtrl::CTreeItem* pItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("m_fGamaGForDesign"));
			pItem->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
			/*pItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("m_fWindBetaZForDesign"));
			pItem->m_lpNodeInfo->SetSubItemReadOnly(1,TRUE);
			pItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("m_fWindBetaZForFoundation"));
			pItem->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);*/
		}
		else 
		{
			CString sText;
			CSuperGridCtrl::CTreeItem* pItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("m_fGamaGForDesign"));
			Ta.m_fGamaGForDesign=1.2;//g_sysPara.gamaG();
			sText.Format("%f",Ta.m_fGamaGForDesign);
			SimplifiedNumString(sText);
			pListCtrl->SetSubItemText(pItem,1,sText);
			pItem->m_lpNodeInfo->SetSubItemReadOnly(1,TRUE);
		}
	}
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.m_cLandscape"))
		Ta.m_cLandscape=sTextValue[0];
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("m_fGamaGForDesign"))
		Ta.m_fGamaGForDesign=atof(sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("m_fLimitBearCoef"))
		Ta.m_fLimitBearCoef=atof(sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("m_iMemberSizeCheckCode")||
		pSelItem->m_idProp==CLDSApp::GetPropID("m_iSlendernessLimCode"))
	{
		if(pSelItem->m_idProp==CLDSApp::GetPropID("m_iMemberSizeCheckCode"))
		{
			if(sTextValue.CompareNoCase(IMemberDesignCode::CODE_DLT_2012()->CodeName())==0)
				Ta.m_iMemberSizeCheckCode=IMemberDesignCode::CODE_DLT_2012()->CodeSerial();
			else if(sTextValue.CompareNoCase(IMemberDesignCode::GB50135_2006()->CodeName())==0)
				Ta.m_iMemberSizeCheckCode=IMemberDesignCode::GB50135_2006()->CodeSerial();	//高耸规范
			else if(sTextValue.CompareNoCase(IMemberDesignCode::CODE_ASCE10_97()->CodeName())==0)
				Ta.m_iMemberSizeCheckCode=IMemberDesignCode::CODE_ASCE10_97()->CodeSerial();
			else if(sTextValue.CompareNoCase(IMemberDesignCode::CODE_DLT_2002()->CodeName())==0)
				Ta.m_iMemberSizeCheckCode=IMemberDesignCode::CODE_DLT_2002()->CodeSerial();
			else
				Ta.m_iMemberSizeCheckCode=IMemberDesignCode::CODE_DLT_2012()->CodeSerial();
			Ta.m_iSlendernessLimCode=Ta.m_iMemberSizeCheckCode;
			CSuperGridCtrl::CTreeItem* pItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("m_iSlendernessLimCode"));
			pListCtrl->SetSubItemText(pItem,1,sTextValue);
		}
		else if(pSelItem->m_idProp==CLDSApp::GetPropID("m_iSlendernessLimCode"))
		{	
#ifdef AFX_TARG_ENU_ENGLISH
			if(sTextValue.CollateNoCase("User-defined")==0)
#else
			if(sTextValue.CollateNoCase("用户指定")==0)
#endif
				Ta.m_iSlendernessLimCode=3;
			else
			{
				Ta.m_iSlendernessLimCode=Ta.m_iMemberSizeCheckCode;
				sTextValue=theApp.GetPropValueStr(pSelItem->m_idProp);
				pListCtrl->SetSubItemText(pSelItem,1,sTextValue);
			}
		}
		/*if(Ta.m_iSlendernessLimCode==3)
		{
			CSuperGridCtrl::CTreeItem* pItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nMainPoleSlendernessLimit"));
			pItem->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
			pItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nXiePoleSlendernessLimit"));
			pItem->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
			pItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nAuxPoleSlendernessLimit"));
			pItem->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
			pItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nTensivePoleSlendernessLimit"));
			pItem->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
			
			pItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nMainTubeSlendernessLimit"));
			pItem->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
			pItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nXieTubeSlendernessLimit"));
			pItem->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
			pItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nAuxTubeSlendernessLimit"));
			pItem->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
			pItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nTensiveTubeSlendernessLimit"));
			pItem->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
		}
		else*/
		{
			CSuperGridCtrl::CTreeItem* pItemMain=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nMainPoleSlendernessLimit"));
			CSuperGridCtrl::CTreeItem* pItemForce=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nXiePoleSlendernessLimit"));
			CSuperGridCtrl::CTreeItem* pItemAux=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nAuxPoleSlendernessLimit"));
			CSuperGridCtrl::CTreeItem* pItemTens=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nTensivePoleSlendernessLimit"));
			CSuperGridCtrl::CTreeItem* pItemTubeMain=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nMainTubeSlendernessLimit"));
			CSuperGridCtrl::CTreeItem* pItemTubeForce=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nXieTubeSlendernessLimit"));
			CSuperGridCtrl::CTreeItem* pItemTubeAux=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nAuxTubeSlendernessLimit"));
			CSuperGridCtrl::CTreeItem* pItemTubeTens=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("Ta.SelMatPara.m_nTensiveTubeSlendernessLimit"));
			pItemMain->m_lpNodeInfo->SetSubItemReadOnly(1,TRUE);
			pItemForce->m_lpNodeInfo->SetSubItemReadOnly(1,TRUE);
			pItemAux->m_lpNodeInfo->SetSubItemReadOnly(1,TRUE);
			pItemTens->m_lpNodeInfo->SetSubItemReadOnly(1,TRUE);
			pItemTubeMain->m_lpNodeInfo->SetSubItemReadOnly(1,TRUE);
			pItemTubeForce->m_lpNodeInfo->SetSubItemReadOnly(1,TRUE);
			pItemTubeAux->m_lpNodeInfo->SetSubItemReadOnly(1,TRUE);
			pItemTubeTens->m_lpNodeInfo->SetSubItemReadOnly(1,TRUE);
			if(Ta.DesignCode()!=NULL)//.m_iSlendernessLimCode==0)	//110~750kV输电线路设计规定
			{
				Ta.SelMatPara.m_nMainPoleSlendernessLimit=Ta.DesignCode()->GetPostRodKlamdaLimit();//150;
				Ta.SelMatPara.m_nXiePoleSlendernessLimit=Ta.DesignCode()->GetPrimaryRodKlamdaLimit();//200;
				Ta.SelMatPara.m_nAuxPoleSlendernessLimit=Ta.DesignCode()->GetBraceRodKlamdaLimit();//250;
				Ta.SelMatPara.m_nTensivePoleSlendernessLimit=Ta.DesignCode()->GetTensionRodKlamdaLimit();//400;
				Ta.SelMatPara.m_nMainTubeSlendernessLimit=130;
				Ta.SelMatPara.m_nXieTubeSlendernessLimit=200;
				Ta.SelMatPara.m_nAuxTubeSlendernessLimit=250;
				Ta.SelMatPara.m_nTensiveTubeSlendernessLimit=400;
				pListCtrl->SetSubItemText(pItemMain,1,CXhChar16("%d",Ta.SelMatPara.m_nMainPoleSlendernessLimit));//CString("150"));
				pListCtrl->SetSubItemText(pItemForce,1,CXhChar16("%d",Ta.SelMatPara.m_nXiePoleSlendernessLimit));//CString("200"));
				pListCtrl->SetSubItemText(pItemAux,1,CXhChar16("%d",Ta.SelMatPara.m_nAuxPoleSlendernessLimit));//CString("250"));
				pListCtrl->SetSubItemText(pItemTens,1,CXhChar16("%d",Ta.SelMatPara.m_nTensivePoleSlendernessLimit));//CString("400"));
				pListCtrl->SetSubItemText(pItemTubeMain,1,CString("130"));
				pListCtrl->SetSubItemText(pItemTubeForce,1,CString("200"));
				pListCtrl->SetSubItemText(pItemTubeAux,1,CString("250"));
				pListCtrl->SetSubItemText(pItemTubeTens,1,CString("400"));
			}
			/*else if(Ta.m_iSlendernessLimCode==1)	//高耸结构设计规定 GB50135-2006
			{
				Ta.SelMatPara.m_nMainPoleSlendernessLimit=150;
				Ta.SelMatPara.m_nXiePoleSlendernessLimit=180;
				Ta.SelMatPara.m_nAuxPoleSlendernessLimit=200;
				Ta.SelMatPara.m_nTensivePoleSlendernessLimit=350;
				Ta.SelMatPara.m_nMainTubeSlendernessLimit=130;
				Ta.SelMatPara.m_nXieTubeSlendernessLimit=180;
				Ta.SelMatPara.m_nAuxTubeSlendernessLimit=200;
				Ta.SelMatPara.m_nTensiveTubeSlendernessLimit=350;
				pListCtrl->SetSubItemText(pItemMain,1,CString("150"));
				pListCtrl->SetSubItemText(pItemForce,1,CString("180"));
				pListCtrl->SetSubItemText(pItemAux,1,CString("200"));
				pListCtrl->SetSubItemText(pItemTens,1,CString("350"));
				pListCtrl->SetSubItemText(pItemTubeMain,1,CString("130"));
				pListCtrl->SetSubItemText(pItemTubeForce,1,CString("180"));
				pListCtrl->SetSubItemText(pItemTubeAux,1,CString("200"));
				pListCtrl->SetSubItemText(pItemTubeTens,1,CString("350"));
			}
			else if(Ta.m_iSlendernessLimCode==2)	//美国 ASCE 10-97
			{
				Ta.SelMatPara.m_nMainPoleSlendernessLimit=150;
				Ta.SelMatPara.m_nXiePoleSlendernessLimit=200;
				Ta.SelMatPara.m_nAuxPoleSlendernessLimit=250;
				Ta.SelMatPara.m_nTensivePoleSlendernessLimit=500;
				Ta.SelMatPara.m_nMainTubeSlendernessLimit=130;
				Ta.SelMatPara.m_nXieTubeSlendernessLimit=200;
				Ta.SelMatPara.m_nAuxTubeSlendernessLimit=250;
				Ta.SelMatPara.m_nTensiveTubeSlendernessLimit=500;
				pListCtrl->SetSubItemText(pItemMain,1,CString("150"));
				pListCtrl->SetSubItemText(pItemForce,1,CString("200"));
				pListCtrl->SetSubItemText(pItemAux,1,CString("250"));
				pListCtrl->SetSubItemText(pItemTens,1,CString("500"));
				pListCtrl->SetSubItemText(pItemTubeMain,1,CString("130"));
				pListCtrl->SetSubItemText(pItemTubeForce,1,CString("200"));
				pListCtrl->SetSubItemText(pItemTubeAux,1,CString("250"));
				pListCtrl->SetSubItemText(pItemTubeTens,1,CString("500"));
			}*/
		}
	}
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.SelMatPara.m_nMainPoleSlendernessLimit"))
		Ta.SelMatPara.m_nMainPoleSlendernessLimit=atoi(sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.SelMatPara.m_nXiePoleSlendernessLimit"))
		Ta.SelMatPara.m_nXiePoleSlendernessLimit=atoi(sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.SelMatPara.m_nAuxPoleSlendernessLimit"))
		Ta.SelMatPara.m_nAuxPoleSlendernessLimit=atoi(sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.SelMatPara.m_nTensivePoleSlendernessLimit"))
		Ta.SelMatPara.m_nTensivePoleSlendernessLimit=atoi(sTextValue);
	//else if(pSelItem->m_idProp==CLDSApp::GetPropID("TubeSlendernessLimit"))
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.SelMatPara.m_nMainTubeSlendernessLimit"))
		Ta.SelMatPara.m_nMainTubeSlendernessLimit=atoi(sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.SelMatPara.m_nXieTubeSlendernessLimit"))
		Ta.SelMatPara.m_nXieTubeSlendernessLimit=atoi(sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.SelMatPara.m_nAuxTubeSlendernessLimit"))
		Ta.SelMatPara.m_nAuxTubeSlendernessLimit=atoi(sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.SelMatPara.m_nTensiveTubeSlendernessLimit"))
		Ta.SelMatPara.m_nTensiveTubeSlendernessLimit=atoi(sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.SelMatPara.m_csMinMainAngleSize"))
		sprintf(Ta.SelMatPara.m_csMinMainAngleSize,"%s",sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.SelMatPara.m_csMinForceAngleSize"))
		sprintf(Ta.SelMatPara.m_csMinForceAngleSize,"%s",sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.SelMatPara.m_csMinAngleSize"))
		sprintf(Ta.SelMatPara.m_csMinAngleSize,"%s",sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("EarthQuake.grade"))
	{
		Ta.EarthQuake.grade=sTextValue[0]-'0';
		if(Ta.EarthQuake.grade==6)
			Ta.EarthQuake.alfa_max=0.04;
		else if(Ta.EarthQuake.grade==7)
			Ta.EarthQuake.alfa_max=0.08;
		else if(Ta.EarthQuake.grade==8)
			Ta.EarthQuake.alfa_max=0.16;
		else if(Ta.EarthQuake.grade==9)
			Ta.EarthQuake.alfa_max=0.32;
		else
			return TRUE;
		CSuperGridCtrl::CTreeItem *pAlfaItem=pListCtrl->FindTreeItemById(CLDSApp::GetPropID("EarthQuake.AlfaMax"));
		if(pAlfaItem)
		{
			CString alfaStr;
			alfaStr.Format("%.2f",Ta.EarthQuake.alfa_max);
			pListCtrl->SetSubItemText(pAlfaItem,1,alfaStr);
		}
	}
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("EarthQuake.AlfaMax"))
		Ta.EarthQuake.alfa_max=atof(sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("EarthQuake.zeta"))
		Ta.EarthQuake.zeta=atof(sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("EarthQuake.Tg"))
		Ta.EarthQuake.Tg=atof(sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("EarthQuake.gamaRE"))
		Ta.EarthQuake.gamaRE=atof(sTextValue);
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("EarthQuake.nFreqsRank"))
	{
		Ta.EarthQuake.nFreqsRank=atoi(sTextValue);
		if(Ta.EarthQuake.nFreqsRank>16||Ta.EarthQuake.nFreqsRank<1)
		{
			Ta.EarthQuake.nFreqsRank=16;
			sTextValue="16";
		}
		else if(Ta.EarthQuake.nFreqsRank<1)
		{
			Ta.EarthQuake.nFreqsRank=1;
			sTextValue="1";
		}
	}
	return TRUE;
}
static BOOL FireButtonClick(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem)
{
	if(pSelItem->m_idProp==CLDSApp::GetPropID("Ta.m_fTowerHeight"))
	{
		SCOPE_STRU scope;
		Ta.GetTaSizeScope(scope);
		Ta.m_fTowerHeight=scope.depth()/1000;	//mm->m
		CString sText;
		sText.Format("%f",Ta.m_fTowerHeight);
		SimplifiedNumString(sText);
		pSelItem->m_lpNodeInfo->SetSubItemText(iSubItem,sText);
		pListCtrl->SetItemText(pSelItem->GetIndex(),iSubItem,sText);
		pListCtrl->RedrawItems(pSelItem->GetIndex(),pSelItem->GetIndex());
		//UpdateWindBetaZForDesign(pListCtrl);
	}
	else if(pSelItem->m_idProp==CLDSApp::GetPropID("EarthQuake.Tg"))
	{
		CEarthQuakeTgSelDlg seldlg;
		if(seldlg.DoModal()==IDOK)
		{
			Ta.EarthQuake.Tg=seldlg.m_fTg;
			CString tgStr;
			tgStr.Format("%.2f",Ta.EarthQuake.Tg);
			pListCtrl->SetSubItemText(pSelItem,1,tgStr);
		}
	}
	return TRUE;
}


CSysParametersDlg::CSysParametersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSysParametersDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSysParametersDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_listCtrlSysParams.AddColumnHeader("parameter name");
	m_listCtrlSysParams.AddColumnHeader("value");
	m_listCtrlSysParams.AddColumnHeader("Access level");
#else
	m_listCtrlSysParams.AddColumnHeader("参数名称");
	m_listCtrlSysParams.AddColumnHeader("取值");
	m_listCtrlSysParams.AddColumnHeader("权限等级");
#endif
}


void CSysParametersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSysParametersDlg)
	DDX_Control(pDX, IDC_LIST_SYS_PARAMETERS, m_listCtrlSysParams);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSysParametersDlg, CDialog)
	//{{AFX_MSG_MAP(CSysParametersDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysParametersDlg message handlers

BOOL CSysParametersDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	long col_wide_arr[3]={200,230,70};
	m_listCtrlSysParams.InitListCtrl(col_wide_arr);
	m_listCtrlSysParams.SetModifyValueFunc(FireValueModify);
	m_listCtrlSysParams.SetButtonClickFunc(FireButtonClick);
	m_listCtrlSysParams.m_hPromptWnd=GetDlgItem(IDC_E_PROMPT_STR)->GetSafeHwnd();
	
	CString sText;
	CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("level of specifications"),TRUE);
	CSuperGridCtrl::CTreeItem* pGroupItem = m_listCtrlSysParams.InsertRootItem(lpInfo);
	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("density of ice coating(kg/m3)"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("Ta.m_fIceDensity")));
	lpInfo->AddSubItemText("level of specifications",TRUE);
#else
	lpInfo->SetSubItemText(0,_T("规范级"),TRUE);
	CSuperGridCtrl::CTreeItem* pGroupItem = m_listCtrlSysParams.InsertRootItem(lpInfo);
	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("覆冰密度(kg/m3)"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("Ta.m_fIceDensity")));
	lpInfo->AddSubItemText("规范级",TRUE);
#endif
	CSuperGridCtrl::CTreeItem* pItem=m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.m_fIceDensity");

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("part specifications checking material choosing specification"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("构件规格验算选材规范"),TRUE);
#endif
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("m_iMemberSizeCheckCode")));
	lpInfo->SetControlType(1,GCT_CMB_LIST);
	CXhChar500 codes;
	IMemberDesignCode::GetSupportCodesStr(codes,501);
	lpInfo->SetListItemsStr(1, (char*)codes);//"110~750kV输电线路设计规定|高耸结构设计规范 GB50135-2006|建筑结构荷载规范 GB50009-2001|用户指定");
	//lpInfo->SetListItemsStr(1, "110~750kV输电线路设计规定|高耸结构设计规定 GB50135-2006|美国 ASCE 10-97");
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->AddSubItemText("level of specifications",TRUE);
#else
	lpInfo->AddSubItemText("规范级",TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("m_iMemberSizeCheckCode");
	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("load calculation execution specifications"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("m_iLoadExecuteCode")));
	lpInfo->AddSubItemText("levle of specifications",TRUE);
	lpInfo->SetControlType(1,GCT_CMB_LIST);
	//lpInfo->SetListItemsStr(1, (char*)codes);//"110~750kV transmission line design rules|high-rise constructure design specifications GB50135-2006|building constucture load specification GB50009-2001|specified by users");
	lpInfo->SetListItemsStr(1, "110~750kVtransmission line design rules|high-rise constructure design specifications GB50135-2006|building constucture load specification GB50009-2001|specified users");
#else
	lpInfo->SetSubItemText(0,_T("荷载计算执行规范"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("m_iLoadExecuteCode")));
	lpInfo->AddSubItemText("规范级",TRUE);
	lpInfo->SetControlType(1,GCT_CMB_LIST);
	//lpInfo->SetListItemsStr(1, (char*)codes);//"110~750kV输电线路设计规定|高耸结构设计规范 GB50135-2006|建筑结构荷载规范 GB50009-2001|用户指定");
	lpInfo->SetListItemsStr(1, 
		"110~750kV输电线路设计规定|高耸结构设计规范 GB50135-2006|"\
		"建筑结构荷载规范 GB50009-2001|架空输电线路荷载规范 DL/T5551-2018|用户指定");
#endif
	CSuperGridCtrl::CTreeItem* pLoadItem=m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);
	pLoadItem->m_idProp=CLDSApp::GetPropID("m_iLoadExecuteCode");

	/*lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("优化选材时βz"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("m_fWindBetaZForDesign")),g_sysPara.m_iLoadExecuteCode==0);
	lpInfo->AddSubItemText("规范级",TRUE);
	lpInfo->SetControlType(0,GCT_BTN_EDIT);
	lpInfo->SetSubItemReadOnly(1,TRUE);
	pItem=m_listCtrlSysParams.InsertItem(pLoadItem,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("m_fWindBetaZForDesign");

	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("基础设计时βz"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("m_fWindBetaZForFoundation")),g_sysPara.m_iLoadExecuteCode==0);
	lpInfo->AddSubItemText("规范级",TRUE);
	lpInfo->SetControlType(0,GCT_BTN_FILEPATH);
	pItem=m_listCtrlSysParams.InsertItem(pLoadItem,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("m_fWindBetaZForFoundation");*/

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("material Optimization γG"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("m_fGamaGForDesign")));
	lpInfo->AddSubItemText("specification level",TRUE);
#else
	lpInfo->SetSubItemText(0,_T("优化选材时γG"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("m_fGamaGForDesign")));
	lpInfo->AddSubItemText("规范级",TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pLoadItem,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("m_fGamaGForDesign");

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("rod's slenderness ratio limit"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("杆件长细比限制"),TRUE);
#endif
	//lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("m_iSlendernessLimCode")));
	//lpInfo->SetControlType(1,GCT_CMB_LIST);
	//lpInfo->SetListItemsStr(1, "110~750kV输电线路设计规定|高耸结构设计规定 GB50135-2006|美国 ASCE 10-97|用户指定");
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(2,"level of specification",TRUE);
#else
	lpInfo->SetSubItemText(2,"规范级",TRUE);
#endif
	CSuperGridCtrl::CTreeItem* pLamdaCalItem=m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);
	pLamdaCalItem->m_idProp=CLDSApp::GetPropID("m_iSlendernessLimCode");
	pLamdaCalItem->m_bHideChildren=TRUE;
	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("angle's slenderness ratio limit"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("角钢长细比限制"),TRUE);
#endif
	CSuperGridCtrl::CTreeItem* pAngleLamdaLim=m_listCtrlSysParams.InsertItem(pLamdaCalItem,lpInfo);
	pAngleLamdaLim->m_idProp=CLDSApp::GetPropID("SlendernessLimit");
	pAngleLamdaLim->m_bHideChildren=TRUE;

	lpInfo = new CListCtrlItemInfo();
	pItem=m_listCtrlSysParams.InsertItem(pAngleLamdaLim,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.SelMatPara.m_nMainPoleSlendernessLimit");
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("pressed leg member"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("受压主材"),TRUE);
#endif
	lpInfo->AddSubItemText(theApp.GetPropValueStr(pItem->m_idProp),Ta.m_iSlendernessLimCode!=3);

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("ohter pressed materials"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("其它受压材"),TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pAngleLamdaLim,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.SelMatPara.m_nXiePoleSlendernessLimit");
	lpInfo->AddSubItemText(theApp.GetPropValueStr(pItem->m_idProp),Ta.m_iSlendernessLimCode!=3);

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("auxiliary materials"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("辅助材"),TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pAngleLamdaLim,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.SelMatPara.m_nAuxPoleSlendernessLimit");
	lpInfo->AddSubItemText(theApp.GetPropValueStr(pItem->m_idProp),Ta.m_iSlendernessLimCode!=3);

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("pulled materials"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("受拉材"),TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pAngleLamdaLim,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.SelMatPara.m_nTensivePoleSlendernessLimit");
	lpInfo->AddSubItemText(theApp.GetPropValueStr(pItem->m_idProp),Ta.m_iSlendernessLimCode!=3);

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("angle's slenderness ratio limit"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("钢管长细比限制"),TRUE);
#endif
	CSuperGridCtrl::CTreeItem* pTubeLamdaLim=m_listCtrlSysParams.InsertItem(pLamdaCalItem,lpInfo);
	pTubeLamdaLim->m_idProp=CLDSApp::GetPropID("TubeSlendernessLimit");
	pTubeLamdaLim->m_bHideChildren=TRUE;

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("pressed leg member"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("受压主材"),TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pTubeLamdaLim,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.SelMatPara.m_nMainTubeSlendernessLimit");
	lpInfo->AddSubItemText(theApp.GetPropValueStr(pItem->m_idProp),Ta.m_iSlendernessLimCode!=3);

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("other pressed materials"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("其它受压材"),TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pTubeLamdaLim,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.SelMatPara.m_nXieTubeSlendernessLimit");
	lpInfo->AddSubItemText(theApp.GetPropValueStr(pItem->m_idProp),Ta.m_iSlendernessLimCode!=3);

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("auxiliary material"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("辅助材"),TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pTubeLamdaLim,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.SelMatPara.m_nAuxTubeSlendernessLimit");
	lpInfo->AddSubItemText(theApp.GetPropValueStr(pItem->m_idProp),Ta.m_iSlendernessLimCode!=3);

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("pulled material"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("受拉材"),TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pTubeLamdaLim,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.SelMatPara.m_nTensiveTubeSlendernessLimit");
	lpInfo->AddSubItemText(theApp.GetPropValueStr(pItem->m_idProp),Ta.m_iSlendernessLimCode!=3);

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("angle specification material choosing limit"),TRUE);
	lpInfo->AddSubItemText("",TRUE);
	lpInfo->AddSubItemText("specification level",TRUE);
#else
	lpInfo->SetSubItemText(0,_T("角钢规格选材限制"),TRUE);
	lpInfo->AddSubItemText("",TRUE);
	lpInfo->AddSubItemText("规范级",TRUE);
#endif
	CSuperGridCtrl::CTreeItem* pSelAngleSizeItem=m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);
	pSelAngleSizeItem->m_bHideChildren=TRUE;

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("minimum section area of leg member"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("主材最小截面积"),TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pSelAngleSizeItem,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.SelMatPara.m_csMinMainAngleSize");
	lpInfo->AddSubItemText(theApp.GetPropValueStr(pItem->m_idProp));
	lpInfo->SetControlType(1,GCT_CMB_EDIT);
	lpInfo->SetListItemsStr(1,MakeAngleSetString());

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("minimum section area of stressed material"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("受力材最小截面积"),TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pSelAngleSizeItem,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.SelMatPara.m_csMinForceAngleSize");
	lpInfo->AddSubItemText(theApp.GetPropValueStr(pItem->m_idProp));
	lpInfo->SetControlType(1,GCT_CMB_EDIT);
	lpInfo->SetListItemsStr(1,MakeAngleSetString());

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("minimum section area of other material"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("其它材最小截面积"),TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pSelAngleSizeItem,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.SelMatPara.m_csMinAngleSize");
	lpInfo->AddSubItemText(theApp.GetPropValueStr(pItem->m_idProp));
	lpInfo->SetControlType(1,GCT_CMB_EDIT);
	lpInfo->SetListItemsStr(1,MakeAngleSetString());

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("project level"),TRUE);
	pGroupItem = m_listCtrlSysParams.InsertRootItem(lpInfo);
	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("Security level"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("Ta.m_siSafetyGrade")));
	lpInfo->AddSubItemText("project level",TRUE);
	lpInfo->SetControlType(1,GCT_CMB_LIST);
	lpInfo->SetListItemsStr(1, "Level 1:Very important rod tower|Level 2:Common various rod tower|Level 3:Temporarily used rod tower|User-defined");
#else
	lpInfo->SetSubItemText(0,_T("工程级"),TRUE);
	pGroupItem = m_listCtrlSysParams.InsertRootItem(lpInfo);
	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("安全等级"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("Ta.m_siSafetyGrade")));
	lpInfo->AddSubItemText("工程级",TRUE);
	lpInfo->SetControlType(1,GCT_CMB_LIST);
	lpInfo->SetListItemsStr(1, "一级:特别重要的杆塔|二级:一般的各类杆塔|三级:临时使用的杆塔|用户指定");
#endif
	pItem=m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.m_siSafetyGrade");

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("Structure importance coefficient"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("Ta.m_fSafetyCoef")),Ta.m_siSafetyGrade!=3);
	lpInfo->AddSubItemText("project level",TRUE);
#else
	lpInfo->SetSubItemText(0,_T("结构重要性系数"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("Ta.m_fSafetyCoef")),Ta.m_siSafetyGrade!=3);
	lpInfo->AddSubItemText("工程级",TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pItem,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.m_fSafetyCoef");


	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("level of rod tower "),TRUE);
	pGroupItem = m_listCtrlSysParams.InsertRootItem(lpInfo);
	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("height of rod tower(m)"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("Ta.m_fTowerHeight")));
	lpInfo->AddSubItemText("level of rod tower",TRUE);
#else
	lpInfo->SetSubItemText(0,_T("杆塔级"),TRUE);
	pGroupItem = m_listCtrlSysParams.InsertRootItem(lpInfo);
	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("杆塔高度(m)"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("Ta.m_fTowerHeight")));
	lpInfo->AddSubItemText("杆塔级",TRUE);
#endif
	lpInfo->SetControlType(1,GCT_BTN_FILEPATH);
	pItem=m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.m_fTowerHeight");

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("basic natural vibration period(s)"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("Ta.m_fVibrationT1")));
	lpInfo->AddSubItemText("level of rod tower",TRUE);
#else
	lpInfo->SetSubItemText(0,_T("基本自振周期(s)"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("Ta.m_fVibrationT1")));
	lpInfo->AddSubItemText("杆塔级",TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.m_fVibrationT1");

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("landform of tower location"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("Ta.m_cLandscape")));
	lpInfo->SetControlType(1,GCT_CMB_LIST);
	lpInfo->SetListItemsStr(1, "A:Offshore Or Flat Desert Region|B:Inland Constructor Sparse Region|C:Downtown With Dense Buildings|D:Downtown With Dense And High-rise Buildings");
	lpInfo->AddSubItemText("level of rod tower",TRUE);
#else
	lpInfo->SetSubItemText(0,_T("塔位地形"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("Ta.m_cLandscape")));
	lpInfo->SetControlType(1,GCT_CMB_LIST);
	lpInfo->SetListItemsStr(1, "A:近海或沙漠平坦地区|B:内陆建筑物稀疏地区|C:有密集建筑群市区|D:有密集建筑群和高层建筑市区");
	lpInfo->AddSubItemText("杆塔级",TRUE);
#endif
	lpInfo->SetControlType(0,GCT_BTN_COMMON);
	pItem=m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("Ta.m_cLandscape");

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("Bearing capacity coefficient maximum design value"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("m_fLimitBearCoef")));
	lpInfo->AddSubItemText("rod tower level",TRUE);
#else
	lpInfo->SetSubItemText(0,_T("承载力系数最大设计值"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("m_fLimitBearCoef")));
	lpInfo->AddSubItemText("杆塔级",TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("m_fLimitBearCoef");

	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("anti-seismic checking and calculation"),TRUE);
#else
	lpInfo->SetSubItemText(0,_T("抗震验算"),TRUE);
#endif
	pItem=m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);
	pItem->m_idProp=CLDSApp::GetPropID("EarthQuake");
	pItem->m_bHideChildren=TRUE;
	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("Earthquake intensity"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("EarthQuake.grade")));
	lpInfo->SetControlType(1,GCT_CMB_LIST);
	lpInfo->SetListItemsStr(1, "Grade 6|Grade 7|Grade 8|Grade 9");
	lpInfo->AddSubItemText("rod tower level",TRUE);
#else
	lpInfo->SetSubItemText(0,_T("地震烈度"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("EarthQuake.grade")));
	lpInfo->SetControlType(1,GCT_CMB_LIST);
	lpInfo->SetListItemsStr(1, "6级|7级|8级|9级");
	lpInfo->AddSubItemText("杆塔级",TRUE);
#endif
	CSuperGridCtrl::CTreeItem* pPropItem=m_listCtrlSysParams.InsertItem(pItem,lpInfo);
	pPropItem->m_idProp=CLDSApp::GetPropID("EarthQuake.grade");
	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("maximum value of horizontal earthquake influence coefficient"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("EarthQuake.AlfaMax")));
	lpInfo->AddSubItemText("rod tower level",TRUE);
#else
	lpInfo->SetSubItemText(0,_T("水平地震影响系数最大值"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("EarthQuake.AlfaMax")));
	lpInfo->AddSubItemText("杆塔级",TRUE);
#endif
	pPropItem=m_listCtrlSysParams.InsertItem(pItem,lpInfo);
	pPropItem->m_idProp=CLDSApp::GetPropID("EarthQuake.AlfaMax");
	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("eigenperiod"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("EarthQuake.Tg")));
	lpInfo->SetControlType(1,GCT_BTN_FILEPATH);
	lpInfo->AddSubItemText("rod tower lever",TRUE);
#else
	lpInfo->SetSubItemText(0,_T("特征周期"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("EarthQuake.Tg")));
	lpInfo->SetControlType(1,GCT_BTN_FILEPATH);
	lpInfo->AddSubItemText("杆塔级",TRUE);
#endif
	pPropItem=m_listCtrlSysParams.InsertItem(pItem,lpInfo);
	pPropItem->m_idProp=CLDSApp::GetPropID("EarthQuake.Tg");
	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("Structural damping ratio"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("EarthQuake.zeta")));
	lpInfo->AddSubItemText("rod tower level",TRUE);
#else
	lpInfo->SetSubItemText(0,_T("结构阻尼比"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("EarthQuake.zeta")));
	lpInfo->AddSubItemText("杆塔级",TRUE);
#endif
	pPropItem=m_listCtrlSysParams.InsertItem(pItem,lpInfo);
	pPropItem->m_idProp=CLDSApp::GetPropID("EarthQuake.zeta");
	lpInfo = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->SetSubItemText(0,_T("Bearing capacity seismic adjustment coefficient"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("EarthQuake.gamaRE")));
	lpInfo->AddSubItemText("rod tower level",TRUE);
	pPropItem=m_listCtrlSysParams.InsertItem(pItem,lpInfo);
	pPropItem->m_idProp=CLDSApp::GetPropID("EarthQuake.gamaRE");
	lpInfo->SetSubItemHelpText(0,"not including main constructre parts, for example, bolts and weld seam");
	lpInfo->SetSubItemHelpText(1,"not including main constructre parts, for example, bolts and weld seam");
	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("earthquake type calculation order"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("EarthQuake.nFreqsRank")));
	lpInfo->AddSubItemText("rod tower level",TRUE);
	pPropItem=m_listCtrlSysParams.InsertItem(pItem,lpInfo);
	pPropItem->m_idProp=CLDSApp::GetPropID("EarthQuake.nFreqsRank");
	lpInfo->SetSubItemHelpText(0,"output modal order number of structural natural vibration in dynamic characteristics analysis");
	lpInfo->SetSubItemHelpText(1,"output modal order number of structural natural vibration in dynamic characteristics analysis");
#else
	lpInfo->SetSubItemText(0,_T("承载力抗震调整系数"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("EarthQuake.gamaRE")));
	lpInfo->AddSubItemText("杆塔级",TRUE);
	pPropItem=m_listCtrlSysParams.InsertItem(pItem,lpInfo);
	pPropItem->m_idProp=CLDSApp::GetPropID("EarthQuake.gamaRE");
	lpInfo->SetSubItemHelpText(0,"不包括螺栓及焊缝在内的主要结构构件");
	lpInfo->SetSubItemHelpText(1,"不包括螺栓及焊缝在内的主要结构构件");
	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("振型计算阶数"),TRUE);
	lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("EarthQuake.nFreqsRank")));
	lpInfo->AddSubItemText("杆塔级",TRUE);
	pPropItem=m_listCtrlSysParams.InsertItem(pItem,lpInfo);
	pPropItem->m_idProp=CLDSApp::GetPropID("EarthQuake.nFreqsRank");
	lpInfo->SetSubItemHelpText(0,"动力特性分析时输出的结构自由振动的振型阶数");
	lpInfo->SetSubItemHelpText(1,"动力特性分析时输出的结构自由振动的振型阶数");
#endif

	/*lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("工程绘图"),TRUE);
	pGroupItem = m_listCtrlSysParams.InsertRootItem(lpInfo);
	m_listCtrlSysParams.SetItemState(pGroupItem->GetIndex(),LVNI_SELECTED,LVNI_SELECTED);

	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("火曲间隙"),TRUE);
	m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);

	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("角钢对接间隙"),TRUE);
	m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);

	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("接头与板间隙"),TRUE);
	m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);

	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("法兰与钢管间隙"),TRUE);
	m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);

	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("顶点间隙"),TRUE);
	m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);

	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("应力扩散角"),TRUE);
	m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);

	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("脚钉间距"),TRUE);
	m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);

	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("脚钉代孔范围"),TRUE);
	m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);

	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("单螺栓垫圈最小厚度"),TRUE);
	m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);

	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("连接板邻边最小角度"),TRUE);
	m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);

	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("无缝钢管最大直径阈值"),TRUE);
	m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);

	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("自动调整钢管位置"),TRUE);
	m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);

	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("槽型插板卷边处理方式"),TRUE);
	m_listCtrlSysParams.InsertItem(pGroupItem,lpInfo);*/
	m_listCtrlSysParams.Redraw();
	return TRUE;
}
#endif