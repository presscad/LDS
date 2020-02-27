// DesLineAngleRibPlateDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "DesLineAngleRibPlateDlg.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "LmaDlg.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "PropertyListOper.h"
#include "btc.h"
#include "DataCompare.h"
#include "LocalFeatureDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
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
static BOOL ModifyRibPlateProperty(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CDesLineAngleRibPlateDlg *pRibPlateDlg=(CDesLineAngleRibPlateDlg*)pPropList->GetParent();
	if(pRibPlateDlg==NULL)
		return FALSE;
	CLogFile errLogFile("c:\\err.log");
	CLogErrorLife logErrLife(&errLogFile);
	CPropertyListOper<CDesLineAngleRibPlateDlg> oper(pPropList,pRibPlateDlg);
	BOOL bReDesignPlate=FALSE;
	CLDSParamPlate *pRibPlate=pRibPlateDlg->m_pRibPlate;
	if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("sPartNo"))
	{
		pRibPlateDlg->m_sPartNo.Copy(valueStr);
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(valueStr,BOMPART::PLATE);
		if(pPart)
		{
			pRibPlateDlg->m_nThick=ftoi(pPart->thick);
			pRibPlateDlg->m_cMaterial=pPart->cMaterial;
			//
			CXhChar50 sValue("%d",pRibPlateDlg->m_nThick);
			pPropList->SetItemPropValue(CDesLineAngleRibPlateDlg::GetPropID("thick"),(char*)sValue);
			QuerySteelMatMark(pRibPlateDlg->m_cMaterial,sValue);
			pPropList->SetItemPropValue(CDesLineAngleRibPlateDlg::GetPropID("material"),(char*)sValue);
		}
#endif
		SEGI nSeg;
		if(ParsePartNo(pRibPlateDlg->m_sPartNo,&nSeg,NULL))
			pRibPlateDlg->m_nSegI=nSeg;
		pPropList->SetItemPropValue(CDesLineAngleRibPlateDlg::GetPropID("iSeg"),(char*)pRibPlateDlg->m_nSegI.ToString());
		//�����߰�κ�
		if(pRibPlate)
		{
			pRibPlate->iSeg=nSeg;
			pRibPlate->SetPartNo(pRibPlateDlg->m_sPartNo);
		}
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("iSeg"))
	{
		pRibPlateDlg->m_nSegI=SEGI(valueStr.GetBuffer());
		if(pRibPlate)
			pRibPlate->iSeg=pRibPlateDlg->m_nSegI;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("material"))
	{
		pRibPlateDlg->m_cMaterial=QuerySteelBriefMatMark(valueStr.GetBuffer(1));
		if(pRibPlate)
			pRibPlate->cMaterial=pRibPlateDlg->m_cMaterial;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("thick"))
	{
		pRibPlateDlg->m_nThick=atoi(valueStr);
		if(pRibPlate)
		{
			pRibPlate->thick=pRibPlateDlg->m_nThick;
			bReDesignPlate=TRUE;
		}
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("type"))
	{
		long iType=valueStr[0]-'0';
		if(pRibPlateDlg->m_cPlateType==iType)
			return FALSE;
		pRibPlateDlg->m_cPlateType=valueStr[0]-'0';
		CPropTreeItem* pFindItem=pPropList->FindItemByPropId(CDesLineAngleRibPlateDlg::GetPropID("location"),NULL);
		if(pFindItem)
		{
			pPropList->DeleteAllSonItems(pFindItem);
			if(pRibPlateDlg->m_cPlateType==2)
			{
				oper.InsertCmbListPropItem(pFindItem,"cDirection","","","",-1,TRUE);
				oper.InsertEditPropItem(pFindItem,"para.D","����ƫ����","�����췽���ƫ����",-1,TRUE);
			}
			else
			{
				oper.InsertCmbListPropItem(pFindItem,"cNormCalType","","","",-1,TRUE);
				oper.InsertEditPropItem(pFindItem,"para.D","����ƫ����","�ط��߷����ƫ����",-1,TRUE);
			}
		}
		pRibPlateDlg->UpdateSketchMap();
		pRibPlateDlg->DisplaySubPropList(pItem);
		//
		pRibPlateDlg->ModifyPlateParam('I',pRibPlateDlg->m_cPlateType);
		pRibPlateDlg->ModifyPlateParam('L',pRibPlateDlg->para.L);
		pRibPlateDlg->ModifyPlateParam('W',pRibPlateDlg->para.W);
		pRibPlateDlg->ModifyPlateParam('H',pRibPlateDlg->para.H);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("NewAssRibPlate"))
	{
		if(stricmp(valueStr,"��")==0)
			pRibPlateDlg->m_bNewAssRibPlate=TRUE;
		else 
			pRibPlateDlg->m_bNewAssRibPlate=FALSE;
		//
		pPropList->DeleteAllSonItems(pItem);
		if(pRibPlateDlg->m_bNewAssRibPlate)
		{
			oper.InsertEditPropItem(pItem,"ass_rib_para.sLable","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"ass_rib_para.thick","","",-1,TRUE);
			//
			pRibPlateDlg->UpdateSketchMap(pItem);
			pRibPlateDlg->DisplaySubPropList(pItem);
		}
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("cNormCalType"))
	{
		pRibPlateDlg->m_cNormCalType=valueStr[0]-'0';
		pRibPlateDlg->DisplaySubPropList(pItem);
		pRibPlateDlg->ModifyPlateParam('N',pRibPlateDlg->m_cNormCalType);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Nx"),pRibPlateDlg->ucs.axis_z.x);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Ny"),pRibPlateDlg->ucs.axis_z.y);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Nz"),pRibPlateDlg->ucs.axis_z.z);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("cDirection"))
	{
		pRibPlateDlg->m_cDirection=valueStr[0]-'0';
		pRibPlateDlg->ModifyPlateParam('N',pRibPlateDlg->m_cDirection);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("bFaceTop"))
	{
		if(stricmp(valueStr,"��")==0)
		{
			pRibPlateDlg->stay_wire_para.bOnRelaPlateTopFace=TRUE;
			pRibPlateDlg->ModifyPlateParam('C',pRibPlateDlg->m_pAssistPlate->handle);
		}
		else 
		{	
			pRibPlateDlg->stay_wire_para.bOnRelaPlateTopFace=FALSE;
			pRibPlateDlg->ModifyPlateParam('C',pRibPlateDlg->m_pAssistPlate->handle*-1);
		}
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("datum_edge"))
	{
		pRibPlateDlg->stay_wire_para.J=valueStr[0]-'0';
		pRibPlateDlg->ModifyPlateParam('J',pRibPlateDlg->stay_wire_para.J);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("rot_angle"))
	{
		pRibPlateDlg->stay_wire_para.rotAngle=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('K',pRibPlateDlg->stay_wire_para.rotAngle);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("para.D"))
	{
		pRibPlateDlg->para.D=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('D',pRibPlateDlg->para.D);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("ucs.axis_z"))
	{
		f3dPoint vec=CStdVector::GetVector(valueStr[0]-'0');
		f3dPoint line_vec=pRibPlateDlg->m_pDatumAngle->End()-pRibPlateDlg->m_pDatumAngle->Start();
		normalize(line_vec);
		double fCos=fabs(vec*line_vec);
		if(fabs(vec*line_vec)<0.5)
		{
			errLogFile.Log("ָ���ķ���ƫ��Ǹ����췽��");
			return FALSE;
		}
		pRibPlateDlg->ucs.axis_z=vec;
		pPropList->SetItemPropValue(CDesLineAngleRibPlateDlg::GetPropID("ucs.axis_z.x"),CXhChar16("%.f",vec.x));
		pPropList->SetItemPropValue(CDesLineAngleRibPlateDlg::GetPropID("ucs.axis_z.y"),CXhChar16("%.f",vec.y));
		pPropList->SetItemPropValue(CDesLineAngleRibPlateDlg::GetPropID("ucs.axis_z.z"),CXhChar16("%.f",vec.z));
		//
		pRibPlateDlg->ModifyPlateParam(KEY2C("Nx"),pRibPlateDlg->ucs.axis_z.x);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Ny"),pRibPlateDlg->ucs.axis_z.y);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Nz"),pRibPlateDlg->ucs.axis_z.z);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("para.LX"))
	{
		pRibPlateDlg->para.L=atof(valueStr);
		pRibPlateDlg->para.Ly=atof(valueStr);
		pPropList->SetItemPropValue(CDesLineAngleRibPlateDlg::GetPropID("para.LY"),valueStr);
		pRibPlateDlg->ModifyPlateParam('L',pRibPlateDlg->para.L);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Ly"),pRibPlateDlg->para.Ly);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("para.LY"))
	{
		pRibPlateDlg->para.Ly=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Ly"),pRibPlateDlg->para.Ly);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("para.WX"))
	{
		pRibPlateDlg->para.W=atof(valueStr);
		pRibPlateDlg->para.Wy=atof(valueStr);
		pPropList->SetItemPropValue(CDesLineAngleRibPlateDlg::GetPropID("para.WY"),valueStr);
		pRibPlateDlg->ModifyPlateParam('W',pRibPlateDlg->para.W);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Wy"),pRibPlateDlg->para.Wy);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("para.WY"))
	{
		pRibPlateDlg->para.Wy=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Wy"),pRibPlateDlg->para.Wy);
		bReDesignPlate=TRUE;
	}
	else if (pItem->m_idProp == CDesLineAngleRibPlateDlg::GetPropID("para.E"))
	{
		pRibPlateDlg->para.E = valueStr[0] - '0';
		pRibPlateDlg->ModifyPlateParam('E', pRibPlateDlg->para.E);
		bReDesignPlate = TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("para.S"))
	{
		pRibPlateDlg->para.S=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('S',pRibPlateDlg->para.S);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("para.HX"))
	{
		pRibPlateDlg->para.H=atof(valueStr);
		pRibPlateDlg->para.Hy=atof(valueStr);
		pPropList->SetItemPropValue(CDesLineAngleRibPlateDlg::GetPropID("para.HY"),valueStr);
		pRibPlateDlg->ModifyPlateParam('H',pRibPlateDlg->para.H);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Hy"),pRibPlateDlg->para.Hy);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("para.HY"))
	{
		pRibPlateDlg->para.Hy=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Hy"),pRibPlateDlg->para.Hy);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("para.C"))
	{
		pRibPlateDlg->para.C=valueStr[0]-'0';
		pPropList->DeleteAllSonItems(pItem);
		if(pRibPlateDlg->para.C==1)
			oper.InsertEditPropItem(pItem,"para.R","","",-1,TRUE);
		//
		pRibPlateDlg->UpdateSketchMap();
		pRibPlateDlg->ModifyPlateParam('C',(long)pRibPlateDlg->para.C);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("para.R"))
	{
		pRibPlateDlg->para.R=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('R',pRibPlateDlg->para.R);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("para.d"))
	{
		pRibPlateDlg->para.d=atof(valueStr);
		pPropList->DeleteAllSonItems(pItem);
		if(pRibPlateDlg->para.d>0 && pRibPlateDlg->m_cPlateType==0)
			oper.InsertEditPropItem(pItem,"para.P","","",-1,TRUE);
		//
		pRibPlateDlg->ModifyPlateParam('d',pRibPlateDlg->para.d);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("para.P"))
	{
		pRibPlateDlg->para.P=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('P',pRibPlateDlg->para.P);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("stay_wire_para.L"))
	{
		pRibPlateDlg->stay_wire_para.L=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('L',pRibPlateDlg->stay_wire_para.L);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("stay_wire_para.W"))
	{
		double fValue=atof(valueStr);
		if(fValue<=pRibPlateDlg->stay_wire_para.d)
		{
			AfxMessageBox("������W��С!");
			return FALSE;
		}
		pRibPlateDlg->stay_wire_para.W=fValue;
		pRibPlateDlg->ModifyPlateParam('W',fValue);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("stay_wire_para.H"))
	{
		double fValue=atof(valueStr);
		if(fValue<=0)
		{
			AfxMessageBox("����ĺ��ӿ��H��С!");
			return FALSE;
		}
		pRibPlateDlg->stay_wire_para.H=fValue;
		pRibPlateDlg->ModifyPlateParam('H',fValue);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("stay_wire_para.R"))
	{
		double fValue=atof(valueStr);
		if(fValue<=pRibPlateDlg->stay_wire_para.d*0.5)
		{
			AfxMessageBox("����ĵ��ǰ뾶R��С!");
			return FALSE;
		}
		pRibPlateDlg->stay_wire_para.R=fValue;
		pRibPlateDlg->ModifyPlateParam('R',fValue);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("stay_wire_para.N"))
	{
		long num=atol(valueStr);
		if(num<=0)
		{
			AfxMessageBox("���߿�������Ϊ0!");
			return FALSE;
		}
		pRibPlateDlg->stay_wire_para.N=num;
		pRibPlateDlg->ModifyPlateParam('N',num);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("stay_wire_para.d"))
	{
		double fValue=atof(valueStr);
		if(fValue<=0||fValue*0.5>pRibPlateDlg->stay_wire_para.R)
		{
			AfxMessageBox("���߿׾�������!");
			return FALSE;
		}
		pRibPlateDlg->stay_wire_para.d=fValue;
		pRibPlateDlg->ModifyPlateParam('d',fValue);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("stay_wire_para.P"))
	{
		double fValue=atof(valueStr);
		if(fValue<=pRibPlateDlg->stay_wire_para.d)
		{
			AfxMessageBox("���߿׾��С!");
			return FALSE;
		}
		pRibPlateDlg->stay_wire_para.P=fValue;
		pRibPlateDlg->ModifyPlateParam('P',fValue);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("ass_rib_para.sLable"))
	{
		pRibPlateDlg->m_sAssPartNo.Copy(valueStr);
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(valueStr,BOMPART::PLATE);
		if(pPart)
		{
			pRibPlateDlg->ass_rib_para.thick=ftoi(pPart->thick);
			CXhChar50 sValue("%d",pRibPlateDlg->ass_rib_para.thick);
			pPropList->SetItemPropValue(CDesLineAngleRibPlateDlg::GetPropID("ass_rib_para.thick"),(char*)sValue);
		}
#endif
	}
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("ass_rib_para.thick"))
		pRibPlateDlg->ass_rib_para.thick=atoi(valueStr);
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("ass_rib_para.RotAngle"))
		pRibPlateDlg->ass_rib_para.fRotAngle=atof(valueStr);
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("ass_rib_para.E"))
		pRibPlateDlg->ass_rib_para.E=atof(valueStr);
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("ass_rib_para.F"))
		pRibPlateDlg->ass_rib_para.F=atof(valueStr);
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("ass_rib_para.G"))
		pRibPlateDlg->ass_rib_para.G=atof(valueStr);
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("ass_rib_para.H"))
		pRibPlateDlg->ass_rib_para.H=atof(valueStr);
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("ass_rib_para.e"))
		pRibPlateDlg->ass_rib_para.e=valueStr[0]-'0';
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("ass_rib_para.I"))
		pRibPlateDlg->ass_rib_para.I=atof(valueStr);
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("ass_rib_para.MF"))
		pRibPlateDlg->ass_rib_para.MF=atof(valueStr);
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("ass_rib_para.MG"))
		pRibPlateDlg->ass_rib_para.MG=atof(valueStr);
	else if(pItem->m_idProp==CDesLineAngleRibPlateDlg::GetPropID("ass_rib_para.MH"))
	{
		pRibPlateDlg->ass_rib_para.MH=atof(valueStr);
		//
		pPropList->DeleteAllSonItems(pItem);
		if(pRibPlateDlg->ass_rib_para.MH>0)
		{
			oper.InsertEditPropItem(pItem,"ass_rib_para.MF","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"ass_rib_para.MG","","",-1,TRUE);
		}
	}
	if(bReDesignPlate)
		pRibPlateDlg->ReDesignPlate();
	return TRUE;
}
static BOOL ButtonClickRibPlate(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CDesLineAngleRibPlateDlg *pRibPlateDlg=(CDesLineAngleRibPlateDlg*)pPropList->GetParent();
	if(pRibPlateDlg==NULL)
		return FALSE;
	if(CDesLineAngleRibPlateDlg::GetPropID("hDatumAngle")==pItem->m_idProp)
		pRibPlateDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);
	else if(CDesLineAngleRibPlateDlg::GetPropID("hDatumBolt")==pItem->m_idProp)
		pRibPlateDlg->SelectObject(CLS_BOLT,pItem->m_idProp);
	else if(CDesLineAngleRibPlateDlg::GetPropID("hDatumNode")==pItem->m_idProp)
		pRibPlateDlg->SelectObject(CLS_NODE,pItem->m_idProp);
	else if(CDesLineAngleRibPlateDlg::GetPropID("hDatumTube")==pItem->m_idProp)
		pRibPlateDlg->SelectObject(CLS_LINETUBE,pItem->m_idProp);
	else if(CDesLineAngleRibPlateDlg::GetPropID("hDatumPlate")==pItem->m_idProp)
		pRibPlateDlg->SelectObject(CLS_PARAMPLATE,pItem->m_idProp);
	return TRUE;
}
static BOOL FireRibPlateHelpStr(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	if(pPropList->GetDlgCtrlID()!=IDC_LIST_BOX)
		return FALSE;	//�������������Բ���ӦFirePropHelpPrompt()����
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CDesLineAngleRibPlateDlg *pRibPlateDlg=(CDesLineAngleRibPlateDlg*)pPropList->GetParent();
	if(pRibPlateDlg==NULL||pView==NULL)
		return FALSE;
	g_pSolidDraw->DelCS(ISolidDraw::WORK_CS);
	pView->Invalidate(FALSE);
	long hCurPart=0;
	if(CDesLineAngleRibPlateDlg::GetPropID("hDatumAngle")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_pDatumAngle)
			hCurPart=pRibPlateDlg->m_pDatumAngle->handle;
	}
	else if(CDesLineAngleRibPlateDlg::GetPropID("hDatumBolt")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_pDatumBolt)
			hCurPart=pRibPlateDlg->m_pDatumBolt->handle;
	}
	else if(CDesLineAngleRibPlateDlg::GetPropID("hDatumNode")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_pDatumNode)
			hCurPart=pRibPlateDlg->m_pDatumNode->handle;
	}
	else if(CDesLineAngleRibPlateDlg::GetPropID("hDatumTube")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_pDatumTube)
			hCurPart=pRibPlateDlg->m_pDatumTube->handle;
	}
	else if(CDesLineAngleRibPlateDlg::GetPropID("hDatumPlate")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_pAssistPlate)
			hCurPart=pRibPlateDlg->m_pAssistPlate->handle;
	}
	pRibPlateDlg->DisplaySubPropList(pItem);
	pRibPlateDlg->UpdateSketchMap(pItem);
	//ѡ�ж�λ������ǰ������Ƶ��߰�
	if(hCurPart==0&&pRibPlateDlg->m_pRibPlate)
		hCurPart=pRibPlateDlg->m_pRibPlate->handle;
	if(hCurPart>0x20)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(hCurPart);
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// CDesLineAngleRibPlateDlg �Ի���
IMPLEMENT_DYNAMIC(CDesLineAngleRibPlateDlg, CCallBackDialog)
CDesLineAngleRibPlateDlg::CDesLineAngleRibPlateDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesLineAngleRibPlateDlg::IDD, pParent)
{
	m_pDatumTube=NULL;
	m_pAssistPlate=NULL;
	m_pDatumAngle=NULL;
	m_pDatumBolt=NULL;
	m_pDatumNode=NULL;
	m_cMaterial='S';
	m_nThick=8;
	m_cPlateType=0;
	m_bNewRibPlate=TRUE;
	m_pRibPlate=NULL;
	m_cNormCalType=0;
	m_cDirection=0;
	m_bNewAssRibPlate=FALSE;
	_previewRibPlate.handle=ISolidDraw::BLK_LOCATE_CS+1;	//��ʱ������, ����Ҫ�ܿ�ϵͳ�ض�����ϵ��ID
	_previewRibPlate.SetBelongModel(&Ta);
}

CDesLineAngleRibPlateDlg::~CDesLineAngleRibPlateDlg()
{
}

void CDesLineAngleRibPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX_SUB, m_xSubPropList);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
}


BEGIN_MESSAGE_MAP(CDesLineAngleRibPlateDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_MOVE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_CONFIRM, OnConfirm)
	ON_BN_CLICKED(IDC_BTN_MODEL_VIEW_OPER, OnBnModelViewOper)
END_MESSAGE_MAP()

int CDesLineAngleRibPlateDlg::nScrLocationX=0;
int CDesLineAngleRibPlateDlg::nScrLocationY=0;
bool CDesLineAngleRibPlateDlg::m_bValidStaticRibParam=false;
bool CDesLineAngleRibPlateDlg::m_bValidStaticStayWireParam=false;
bool CDesLineAngleRibPlateDlg::m_bValidStaticAssRibParam=false;
CDesLineAngleRibPlateDlg::RIBPLATE_PARAM CDesLineAngleRibPlateDlg::s_xRibPlateParam;			//�߰����
CDesLineAngleRibPlateDlg::STAYWIRE_RIBPLATE_PARA CDesLineAngleRibPlateDlg::s_xStayWireParam;	//���߰����
CDesLineAngleRibPlateDlg::TUBE_CORNER_RIBPLATE CDesLineAngleRibPlateDlg::s_xAssRibParam;		//���߰��߰����
CDesLineAngleRibPlateDlg::BASIC_PARRM CDesLineAngleRibPlateDlg::s_xBasicParam;
// CDesLineAngleRibPlateDlg ��Ϣ�������
BOOL CDesLineAngleRibPlateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.SetDividerScale(0.6);
	m_xSubPropList.SetDividerScale(0.6);
	m_xSubPropList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	InitPropHashtable();
	SetCallBackFunc(FireCallBackCommand);
	if(m_bInernalStart)	//�ڲ�����
	{	//�ָ�ѡ����
		FinishSelectObjOper();	//���ѡ�����ĺ�������
		DisplayPlatePropList();
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		m_propList.SetFocus();	//��������ý���
		if(pItem)
			m_propList.SetCurSel(pItem->m_iIndex);	//����ѡ����
		DisplaySubPropList(pItem);	//��ʾ������
		CPropTreeItem *pSubItem=m_xSubPropList.FindItemByPropId(m_idSubEventProp,NULL);
		m_xSubPropList.SetFocus();
		if(pSubItem)	//ѡ��������
			m_xSubPropList.SetCurSel(pSubItem->m_iIndex);
	}
	else
	{
		if(!m_bNewRibPlate)
		{
			if(m_pRibPlate==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("You haven't choose the right rib plate!");
#else
				AfxMessageBox("δѡ����ȷ�ĽǸ��߰壡");
#endif
				return FALSE;
			}
			if(m_pRibPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
			{	//��ʼ����λ����
				long A=0,B=0;
				m_pRibPlate->GetDesignItemValue('A',&A);
				m_pDatumAngle=(CLDSLineAngle*)console.FromPartHandle(A,CLS_LINEANGLE);
				if(m_pDatumAngle==NULL)
					return FALSE;
				m_pRibPlate->GetDesignItemValue('B',&B);
				m_pDatumNode=console.FromNodeHandle(B);
				m_pDatumBolt=(CLDSBolt*)console.FromPartHandle(B,CLS_BOLT);
				m_sPartNo.Copy(m_pRibPlate->GetPartNo());
				m_nThick=(int)m_pRibPlate->GetThick();
				m_cMaterial=m_pRibPlate->cMaterial;
				m_nSegI=m_pRibPlate->iSeg;
				//��ʼ����Ʋ���
				m_pRibPlate->GetDesignItemValue('I',&m_cPlateType);
				m_pRibPlate->GetDesignItemValue('N',&m_cNormCalType);
				m_pRibPlate->GetDesignItemValue('L',&para.L);
				if(para.L==0)
					para.L=m_pDatumAngle->GetWidth();
				m_pRibPlate->GetDesignItemValue(KEY2C("Ly"),&para.Ly);
				if(para.Ly==0)
					para.Ly=para.L;
				m_pRibPlate->GetDesignItemValue('W',&para.W);
				m_pRibPlate->GetDesignItemValue(KEY2C("Wy"),&para.Wy);
				if(para.Wy==0)
					para.Wy=para.W;
				m_pRibPlate->GetDesignItemValue('C',&para.C);
				m_pRibPlate->GetDesignItemValue('S',&para.S);
				m_pRibPlate->GetDesignItemValue('D',&para.D);
				m_pRibPlate->GetDesignItemValue('H',&para.H);
				if(para.H==0)
					para.H=m_pDatumAngle->GetWidth();
				m_pRibPlate->GetDesignItemValue(KEY2C("Hy"),&para.Hy);
				if(para.Hy==0)
					para.Hy=para.H;
				m_pRibPlate->GetDesignItemValue('R',&para.R);
				m_pRibPlate->GetDesignItemValue('d',&para.d);
				m_pRibPlate->GetDesignItemValue('P',&para.P);
				m_pRibPlate->GetDesignItemValue('E', &para.E);
			}
			else if(m_pRibPlate->m_iParamType==TYPE_TUBESTAYWIREPLATE)
			{
				long A=0,C=0;
				m_pRibPlate->GetDesignItemValue(KEY2C("A"),&A);
				m_pDatumTube=(CLDSLineTube*)console.FromPartHandle(A,CLS_LINETUBE);
				if(m_pDatumTube==NULL)
					return FALSE;
				m_pRibPlate->GetDesignItemValue(KEY2C("C"),&C);  //�����ְ�
				m_pAssistPlate=(CLDSGeneralPlate*)console.FromPartHandle(C,CLS_PARAMPLATE,CLS_PLATE);
				if(m_pAssistPlate==NULL)
					return FALSE;
				m_sPartNo.Copy(m_pRibPlate->GetPartNo());
				m_nThick=(int)m_pRibPlate->GetThick();
				m_cMaterial=m_pRibPlate->cMaterial;
				m_nSegI=m_pRibPlate->iSeg;
				//
				m_pRibPlate->GetDesignItemValue(KEY2C("K"),&stay_wire_para.rotAngle);
				m_pRibPlate->GetDesignItemValue(KEY2C("J"),&stay_wire_para.J);
				m_pRibPlate->GetDesignItemValue(KEY2C("L"),&stay_wire_para.L);  //���ڸֹ����򳤶�
				m_pRibPlate->GetDesignItemValue(KEY2C("W"),&stay_wire_para.W);  //��Ŀ��
				m_pRibPlate->GetDesignItemValue(KEY2C("H"),&stay_wire_para.H);  //���ĺ��ӳ���
				m_pRibPlate->GetDesignItemValue(KEY2C("R"),&stay_wire_para.R);  //���ǰ뾶
				m_pRibPlate->GetDesignItemValue(KEY2C("d"),&stay_wire_para.d);  //���߿׾�
				m_pRibPlate->GetDesignItemValue(KEY2C("N"),&stay_wire_para.N);  //���߿���
				m_pRibPlate->GetDesignItemValue(KEY2C("P"),&stay_wire_para.P);  //�׼��
			}
			else
				return FALSE;
		}
		if(m_bNewRibPlate&&m_pRibPlate==NULL)
		{
			m_pRibPlate=&_previewRibPlate;
			InitRibPlatePara();
			CreateRibPlate(m_pRibPlate);
		}
		if(m_pRibPlate)		//�����߰�����ϵ
			ucs=m_pRibPlate->ucs;
		//��ʾ�߰����
		DisplayPlatePropList();
		DisplaySubPropList(NULL);
	}
	UpdateSketchMap();
	//
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
	if(m_pDatumAngle)
		SetWindowText("�Ǹ��߰�");
	else if(m_pDatumTube)
		SetWindowText("�ֹ����߰�");
	return TRUE;
}
IMPLEMENT_PROP_FUNC(CDesLineAngleRibPlateDlg);
const DWORD HASHTABLESIZE = 500;
void CDesLineAngleRibPlateDlg::InitPropHashtable()
{
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(50);
	int id = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"basicInfo"));
	AddPropItem("sPartNo",PROPLIST_ITEM(id++,"sPartNo"));
	AddPropItem("iSeg",PROPLIST_ITEM(id++,"iSeg"));
	AddPropItem("material",PROPLIST_ITEM(id++,"material","material",MakeMaterialMarkSetString()));
	AddPropItem("thick",PROPLIST_ITEM(id++,"thick"));
	AddPropItem("type",PROPLIST_ITEM(id++,"plate-type","","0.inline|1.outline"));
	AddPropItem("NewAssRibPlate",PROPLIST_ITEM(id++,"bCreateAssRib","","Yes|No"));
	//
	AddPropItem("RelaInfo",PROPLIST_ITEM(id++,"relation"));
	AddPropItem("hDatumAngle",PROPLIST_ITEM(id++,"Datum Angle"));
	AddPropItem("hDatumNode",PROPLIST_ITEM(id++,"Datum Node"));
	AddPropItem("hDatumBolt",PROPLIST_ITEM(id++,"Datum Bolt"));
	AddPropItem("hDatumTube",PROPLIST_ITEM(id++,"hDatumTube",""));
	AddPropItem("hDatumPlate",PROPLIST_ITEM(id++,"hDatumPlate",""));
	//
	AddPropItem("location",PROPLIST_ITEM(id++,"location","location"));
	AddPropItem("cNormCalType",PROPLIST_ITEM(id++,"Norm Calculate Type","","0.Smart Cal|1.The User"));
	AddPropItem("cDirection",PROPLIST_ITEM(id++,"Extend vec type","","0.Start->End|1.End->Start"));
	AddPropItem("para.D",PROPLIST_ITEM(id++,"norm offset"));
	AddPropItem("preview",PROPLIST_ITEM(id++,"preview","",""));
	AddPropItem("bFaceTop",PROPLIST_ITEM(id++,"bFaceTop","","Yes|No"));
	AddPropItem("datum_edge",PROPLIST_ITEM(id++,"start_edge_type","","0.datum_edge|1.weld_line"));
	AddPropItem("rot_angle",PROPLIST_ITEM(id++,"strat_angle","",""));
	//��������
	AddPropItem("NormInfo",PROPLIST_ITEM(id++,"the normal"));
	AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"work vector","",CStdVector::MakeVectorSetStr()));
	AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X",""));
	AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y",""));
	AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z",""));
	//
	AddPropItem("profile",PROPLIST_ITEM(id++,"profile"));
	AddPropItem("para.LX",PROPLIST_ITEM(id++,"parameter Lx"));
	AddPropItem("para.LY",PROPLIST_ITEM(id++,"parameter Ly"));
	AddPropItem("para.WX",PROPLIST_ITEM(id++,"parameter Wx"));
	AddPropItem("para.WY",PROPLIST_ITEM(id++,"parameter Wy"));
	AddPropItem("para.E", PROPLIST_ITEM(id++, "parameter E"));
	AddPropItem("para.S",PROPLIST_ITEM(id++,"parameter S"));
	AddPropItem("para.C",PROPLIST_ITEM(id++,"Edge type","","0.Line|1.Arc"));
	AddPropItem("para.HX",PROPLIST_ITEM(id++,"parameter Hx"));
	AddPropItem("para.HY",PROPLIST_ITEM(id++,"parameter Hy"));
	AddPropItem("para.R",PROPLIST_ITEM(id++,"parameter R"));
	AddPropItem("para.P",PROPLIST_ITEM(id++,"parameter P"));
	AddPropItem("para.d",PROPLIST_ITEM(id++,"parameter d"));
	//
	AddPropItem("stay_wire_para.L",PROPLIST_ITEM(id++,"L",""));
	AddPropItem("stay_wire_para.W",PROPLIST_ITEM(id++,"W",""));
	AddPropItem("stay_wire_para.H",PROPLIST_ITEM(id++,"H",""));
	AddPropItem("stay_wire_para.R",PROPLIST_ITEM(id++,"R",""));
	AddPropItem("stay_wire_para.N",PROPLIST_ITEM(id++,"N",""));
	AddPropItem("stay_wire_para.d",PROPLIST_ITEM(id++,"d",""));
	AddPropItem("stay_wire_para.P",PROPLIST_ITEM(id++,"P",""));
	//
	AddPropItem("ass_rib_para.sLable",PROPLIST_ITEM(id++,"sLable",""));
	AddPropItem("ass_rib_para.RotAngle",PROPLIST_ITEM(id++,"RotAngle",""));
	AddPropItem("ass_rib_para.E",PROPLIST_ITEM(id++,"E",""));
	AddPropItem("ass_rib_para.F",PROPLIST_ITEM(id++,"F",""));
	AddPropItem("ass_rib_para.G",PROPLIST_ITEM(id++,"G",""));
	AddPropItem("ass_rib_para.H",PROPLIST_ITEM(id++,"H",""));
	AddPropItem("ass_rib_para.e",PROPLIST_ITEM(id++,"e",""));
	AddPropItem("ass_rib_para.I",PROPLIST_ITEM(id++,"I",""));
	AddPropItem("ass_rib_para.MF",PROPLIST_ITEM(id++,"MF",""));
	AddPropItem("ass_rib_para.MG",PROPLIST_ITEM(id++,"MG",""));
	AddPropItem("ass_rib_para.MH",PROPLIST_ITEM(id++,"MH",""));
#else 
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"������Ϣ"));
	AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	AddPropItem("iSeg",PROPLIST_ITEM(id++,"�κ�"));
	AddPropItem("material",PROPLIST_ITEM(id++,"����","����",MakeMaterialMarkSetString()));
	AddPropItem("thick",PROPLIST_ITEM(id++,"���","��Ȳ���T"));
	AddPropItem("type",PROPLIST_ITEM(id++,"�߰�����","","0.�ڲ��߰�|1.����߰�|2.�����߰�"));
	AddPropItem("NewAssRibPlate",PROPLIST_ITEM(id++,"�����߰�","","��|��"));
	//
	AddPropItem("RelaInfo",PROPLIST_ITEM(id++,"������Ϣ"));
	AddPropItem("hDatumAngle",PROPLIST_ITEM(id++,"��׼�Ǹ�","��λ��׼�Ǹ־��"));
	AddPropItem("hDatumNode",PROPLIST_ITEM(id++,"��׼�ڵ�","��λ��׼�ڵ���"));
	AddPropItem("hDatumBolt",PROPLIST_ITEM(id++,"��׼��˨","��λ��׼��˨���"));
	AddPropItem("hDatumTube",PROPLIST_ITEM(id++,"��׼�ֹ�","��λ��׼�ֹܾ��"));
	AddPropItem("hDatumPlate",PROPLIST_ITEM(id++,"�����ְ�","��λ�����ְ���"));
	//
	AddPropItem("location",PROPLIST_ITEM(id++,"װ�䷽λ","װ�䷽λ����"));
	AddPropItem("cNormCalType",PROPLIST_ITEM(id++,"������㷽ʽ","����ļ��㷽ʽ","0.���ܼ���|1.�û�ָ��"));
	AddPropItem("cDirection",PROPLIST_ITEM(id++,"���췽��ʽ","���췽��ʽ","0.ʼ->��|1.��->ʼ"));
	AddPropItem("para.D",PROPLIST_ITEM(id++,"����ƫ����","�ط��߷����ƫ�ƾ���"));
	AddPropItem("preview",PROPLIST_ITEM(id++,"Ԥ������","֧��Ԥ��","Ԥ��"));
	AddPropItem("bFaceTop",PROPLIST_ITEM(id++,"�ڸְ嶥��","λ�ڸְ�Ķ���","��|��"));
	AddPropItem("datum_edge",PROPLIST_ITEM(id++,"��ʼ������","λ�ڸְ�Ķ���","0.��׼��|1.������"));
	AddPropItem("rot_angle",PROPLIST_ITEM(id++,"��ʼ�Ƕ�","",""));
	//��������
	AddPropItem("NormInfo",PROPLIST_ITEM(id++,"�߰巨�߷���"));
	AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"��������","",CStdVector::MakeVectorSetStr()));
	AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X","X����"));
	AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y","Y����"));
	AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z","Z����"));
	//
	AddPropItem("profile",PROPLIST_ITEM(id++,"�������"));
	AddPropItem("para.LX",PROPLIST_ITEM(id++,"�߰峤��Lx","�߰���X֫�ϵĳ���"));
	AddPropItem("para.LY",PROPLIST_ITEM(id++,"�߰峤��Ly","�߰���Y֫�ϵĳ���"));
	AddPropItem("para.WX",PROPLIST_ITEM(id++,"̨�ڸ߶�Wx","�߰���X֫�ϵ�̨�ڸ߶�"));
	AddPropItem("para.WY",PROPLIST_ITEM(id++,"̨�ڸ߶�Wy","�߰���Y֫�ϵ�̨�ڸ߶�"));
	AddPropItem("para.E", PROPLIST_ITEM(id++, "��������", "", "0.ֱ��|1.Բ��"));
	AddPropItem("para.S",PROPLIST_ITEM(id++,"���ǿ��S"));
	AddPropItem("para.C",PROPLIST_ITEM(id++,"���ɱ�����","","0.ֱ��|1.Բ��"));
	AddPropItem("para.HX",PROPLIST_ITEM(id++,"�߰峤��Hx"));
	AddPropItem("para.HY",PROPLIST_ITEM(id++,"�߰峤��Hy"));
	AddPropItem("para.R",PROPLIST_ITEM(id++,"Բ���뾶R","�����Ŀ�λԲ�ĵģ�Բ�����ɱߵİ뾶"));
	AddPropItem("para.P",PROPLIST_ITEM(id++,"���Ŀ׾�P"));
	AddPropItem("para.d",PROPLIST_ITEM(id++,"���Ŀ׾�d"));
	//
	AddPropItem("stay_wire_para.L",PROPLIST_ITEM(id++,"�߰峤��L","�߰峤��"));
	AddPropItem("stay_wire_para.W",PROPLIST_ITEM(id++,"�߰���W","�׵����ܾ���"));
	AddPropItem("stay_wire_para.H",PROPLIST_ITEM(id++,"���ӿ��H","���ӿ��"));
	AddPropItem("stay_wire_para.R",PROPLIST_ITEM(id++,"����Բ��R","����Բ��"));
	AddPropItem("stay_wire_para.N",PROPLIST_ITEM(id++,"���߿���","���߿���"));
	AddPropItem("stay_wire_para.d",PROPLIST_ITEM(id++,"���߿׾�D","���߿׾�"));
	AddPropItem("stay_wire_para.P",PROPLIST_ITEM(id++,"���߿׾�P","���߿׾�"));
	//
	AddPropItem("ass_rib_para.sLable",PROPLIST_ITEM(id++,"����"));
	AddPropItem("ass_rib_para.thick",PROPLIST_ITEM(id++,"���"));
	AddPropItem("ass_rib_para.RotAngle",PROPLIST_ITEM(id++,"ƫ�ƽ�"));
	AddPropItem("ass_rib_para.E",PROPLIST_ITEM(id++,"�߰峤��A"));
	AddPropItem("ass_rib_para.F",PROPLIST_ITEM(id++,"̨�׸߶�C"));
	AddPropItem("ass_rib_para.G",PROPLIST_ITEM(id++,"�װ���B"));
	AddPropItem("ass_rib_para.H",PROPLIST_ITEM(id++,"��������D"));
	AddPropItem("ass_rib_para.e",PROPLIST_ITEM(id++,"����ģʽ","","0.�н�|1.�л�"));
	AddPropItem("ass_rib_para.I",PROPLIST_ITEM(id++,"�н�ֵ"));
	AddPropItem("ass_rib_para.MH",PROPLIST_ITEM(id++,"�׾�H"));
	AddPropItem("ass_rib_para.MF",PROPLIST_ITEM(id++,"�׾�F"));
	AddPropItem("ass_rib_para.MG",PROPLIST_ITEM(id++,"�׾�G"));
#endif
}
int CDesLineAngleRibPlateDlg::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)
{	//��������ID�õ�����ֵ
	CXhChar100 sText;
	if(GetPropID("sPartNo")==id)
		sText.Copy(m_sPartNo);
	else if(GetPropID("iSeg")==id)
		sText.Copy(m_nSegI.ToString());
	else if(GetPropID("thick")==id)
		sText.Printf("%d",m_nThick);
	else if(GetPropID("material")==id)
	{
		char matStr[20]="";
		QuerySteelMatMark(m_cMaterial,matStr);
		sText.Printf("%s",matStr);
	}
	else if(GetPropID("type")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_cPlateType==0)
			sText.Copy("0.Inline");
		else if(m_cPlateType==1)
			sText.Copy("1.OutLine");
		else
			sText.Copy("2.Lengthways")
#else
		if(m_cPlateType==0)
			sText.Copy("0.�ڲ��߰�");
		else if(m_cPlateType==1)
			sText.Copy("1.����߰�");
		else
			sText.Copy("2.�����߰�");
#endif
	}
	else if(GetPropID("hDatumAngle")==id)
	{
		sText.Copy("0X0");
		if(m_pDatumAngle)
			sText.Printf("0X%X",m_pDatumAngle->handle);
	}
	else if(GetPropID("hDatumNode")==id)
	{
		sText.Copy("0X0");
		if(m_pDatumNode)
			sText.Printf("0X%X",m_pDatumNode->handle);
	}
	else if(GetPropID("hDatumBolt")==id)
	{
		sText.Copy("0X0");
		if(m_pDatumBolt)
			sText.Printf("0X%X",m_pDatumBolt->handle);
	}
	else if(GetPropID("hDatumTube")==id)
	{
		sText.Copy("0X0");
		if(m_pDatumTube)
			sText.Printf("0X%X",m_pDatumTube->handle);
	}
	else if(GetPropID("hDatumPlate")==id)
	{
		sText.Copy("0X0");
		if(m_pAssistPlate)
			sText.Printf("0X%X",m_pAssistPlate->handle);
	}
	else if(GetPropID("cNormCalType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_cNormCalType==0)
			sText.Copy("0.Smart Cal");
		else
			sText.Copy("1.The User");
#else
		if(m_cNormCalType==0)
			sText.Copy("0.���ܼ���");
		else
			sText.Copy("1.�û�ָ��");
#endif
	}
	else if(GetPropID("cDirection")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_cDirection==0)
			sText.Copy("0.start->end");
		else
			sText.Copy("1.end->start");
#else
		if(m_cDirection==0)
			sText.Copy("0.ʼ->��");
		else
			sText.Copy("1.��->ʼ");
#endif
	}
	else if(GetPropID("bFaceTop")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(stay_wire_para.bOnRelaPlateTopFace)
			sText.Copy("Yes");
		else
			sText.Copy("No");
#else
		if(stay_wire_para.bOnRelaPlateTopFace)
			sText.Copy("��");
		else
			sText.Copy("��");
#endif
	}
	else if(GetPropID("datum_edge")==id)
	{
		if(stay_wire_para.J==0)
			sText.Copy("0.��ʼ��");
		else
			sText.Copy("1.������");
	}
	else if(GetPropID("rot_angle")==id)
	{
		sText.Printf("%f",stay_wire_para.rotAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z")==id)
	{
		int nVecType=CStdVector::GetVectorType(ucs.axis_z);
		sText.Printf("%s",CStdVector::GetVecTypeString(nVecType));
	}
	else if(GetPropID("ucs.axis_z.x")==id)
	{
		sText.Printf("%f",ucs.axis_z.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.y")==id)
	{
		sText.Printf("%f",ucs.axis_z.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.z")==id)
	{
		sText.Printf("%f",ucs.axis_z.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.D")==id)
	{
		sText.Printf("%f",para.D);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.C")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_cPlateType==0)
			sText.Copy("0.Line");
		else
			sText.Copy("1.Arc");
#else
		if(para.C==0)
			sText.Copy("0.ֱ��");
		else
			sText.Copy("1.Բ��");
#endif
	}
	else if(GetPropID("para.LX")==id)
	{
		sText.Printf("%f",para.L);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.LY")==id)
	{
		sText.Printf("%f",para.Ly);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.WX")==id)
	{
		sText.Printf("%f",para.W);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.WY")==id)
	{
		sText.Printf("%f",para.Wy);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("para.E") == id)
	{
		if (para.E == 0)
			sText.Copy("0.ֱ��");
		else
			sText.Copy("1.Բ��");
	}
	else if(GetPropID("para.S")==id)
	{
		sText.Printf("%f",para.S);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.HX")==id)
	{
		sText.Printf("%f",para.H);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.HY")==id)
	{
		sText.Printf("%f",para.Hy);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.R")==id)
	{
		sText.Printf("%f",para.R);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.d")==id)
	{
		sText.Printf("%f",para.d);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.P")==id)
	{
		sText.Printf("%f",para.P);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("stay_wire_para.L")==id)
	{
		sText.Printf("%f",stay_wire_para.L);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("stay_wire_para.W")==id)
	{
		sText.Printf("%f",stay_wire_para.W);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("stay_wire_para.H")==id)
	{
		sText.Printf("%f",stay_wire_para.H);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("stay_wire_para.R")==id)
	{
		sText.Printf("%f",stay_wire_para.R);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("stay_wire_para.N")==id)
		sText.Printf("%d",stay_wire_para.N);
	else if(GetPropID("stay_wire_para.d")==id)
	{
		sText.Printf("%f",stay_wire_para.d);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("stay_wire_para.P")==id)
	{
		sText.Printf("%f",stay_wire_para.P);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("NewAssRibPlate")==id)
	{
		if(m_bNewAssRibPlate)
			sText.Copy("��");
		else
			sText.Copy("��");
	}
	else if(GetPropID("ass_rib_para.sLable")==id)
		sText.Copy(m_sAssPartNo);
	else if(GetPropID("ass_rib_para.thick")==id)
		sText.Printf("%d",ass_rib_para.thick);
	else if(GetPropID("ass_rib_para.RotAngle")==id)
	{
		sText.Printf("%f",ass_rib_para.fRotAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ass_rib_para.E")==id)
	{
		sText.Printf("%f",ass_rib_para.E);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ass_rib_para.F")==id)
	{
		sText.Printf("%f",ass_rib_para.F);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ass_rib_para.G")==id)
	{
		sText.Printf("%f",ass_rib_para.G);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ass_rib_para.H")==id)
	{
		sText.Printf("%f",ass_rib_para.H);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ass_rib_para.e")==id)
	{
		if(ass_rib_para.e==0)
			sText.Copy("0.�н�");
		else if(ass_rib_para.e==1)
			sText.Copy("1.�л�");
	}
	else if(GetPropID("ass_rib_para.I")==id)
	{
		sText.Printf("%f",ass_rib_para.I);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ass_rib_para.MH")==id)
	{
		sText.Printf("%f",ass_rib_para.MH);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ass_rib_para.MF")==id)
	{
		sText.Printf("%f",ass_rib_para.MF);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ass_rib_para.MG")==id)
	{
		sText.Printf("%f",ass_rib_para.MG);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
void CDesLineAngleRibPlateDlg::DisplayPlatePropList()
{
	m_propList.CleanTree();
	m_propList.SetModifyValueFunc(ModifyRibPlateProperty);
	m_propList.SetButtonClickFunc(ButtonClickRibPlate);
	m_propList.SetPropHelpPromptFunc(FireRibPlateHelpStr);
	//
	CPropertyListOper<CDesLineAngleRibPlateDlg> oper(&m_propList,this);
	CPropTreeItem *pGroupItem,*pRoot=m_propList.GetRootItem();
	//������Ϣ
	pGroupItem=oper.InsertPropItem(pRoot,"basicInfo");
	oper.InsertEditPropItem(pGroupItem,"sPartNo");		//�������
	oper.InsertEditPropItem(pGroupItem,"iSeg");			//�κ�
	oper.InsertCmbListPropItem(pGroupItem,"material");	//����
	oper.InsertEditPropItem(pGroupItem,"thick");		//���
	if(m_pDatumAngle)
		oper.InsertCmbListPropItem(pGroupItem,"type");		//��������
	if(m_pDatumTube&&m_bNewRibPlate)
	{
		CPropTreeItem *pPropItem=oper.InsertCmbListPropItem(pGroupItem,"NewAssRibPlate");
		if(m_bNewAssRibPlate)
		{
			oper.InsertEditPropItem(pPropItem,"ass_rib_para.sLable");
			oper.InsertEditPropItem(pPropItem,"ass_rib_para.thick");
		}
	}
	//������Ϣ
	pGroupItem=oper.InsertPropItem(pRoot,"RelaInfo");
	if(m_pDatumAngle)
	{
		oper.InsertButtonPropItem(pGroupItem,"hDatumAngle");	//��׼�Ǹ�
		oper.InsertButtonPropItem(pGroupItem,"hDatumNode");		//��׼�ڵ�
		oper.InsertButtonPropItem(pGroupItem,"hDatumBolt");		//��׼��˨
	}
	else if(m_pDatumTube)
	{
		oper.InsertButtonPropItem(pGroupItem,"hDatumTube");		//��׼�Ǹ�
		oper.InsertButtonPropItem(pGroupItem,"hDatumPlate");	//��׼�ְ�
	}
	//װ�䷽λ
	pGroupItem=oper.InsertPropItem(pRoot,"location");
	if(m_pDatumAngle)
	{
		if(m_cPlateType==2)
		{
			oper.InsertCmbListPropItem(pGroupItem,"cDirection");
			oper.InsertEditPropItem(pGroupItem,"para.D","����ƫ����","�����췽���ƫ����",-1,TRUE);
		}
		else
		{
			oper.InsertCmbListPropItem(pGroupItem,"cNormCalType");
			oper.InsertEditPropItem(pGroupItem,"para.D","����ƫ����","�ط��߷����ƫ����",-1,TRUE);
		}
	}
	else if(m_pDatumTube)
	{
		oper.InsertCmbListPropItem(pGroupItem,"bFaceTop","","","",-1,TRUE);
		if(m_pDatumTube->m_bHasWeldRoad)
			oper.InsertCmbListPropItem(pGroupItem,"datum_edge","","","",-1,TRUE);
		oper.InsertEditPropItem(pGroupItem,"rot_angle","","",-1,TRUE);
	}
	//
	m_propList.Redraw();
}
//��ʾ������
void CDesLineAngleRibPlateDlg::DisplaySubPropList(CPropTreeItem *pItem/*=NULL*/)
{
	m_xSubPropList.CleanTree();
	m_xSubPropList.SetModifyValueFunc(ModifyRibPlateProperty);
	m_xSubPropList.SetButtonClickFunc(ButtonClickRibPlate);
	//
	CPropertyListOper<CDesLineAngleRibPlateDlg> oper(&m_xSubPropList,this);
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pRoot=m_xSubPropList.GetRootItem();
	if(pItem&&pItem->m_idProp==GetPropID("cNormCalType")&&m_cNormCalType==1)
	{	//��ʾ���߷���
		pGroupItem=oper.InsertPropItem(pRoot,"NormInfo");
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"ucs.axis_z");
		CPropTreeItem *pSonItem=NULL;
		pSonItem=oper.InsertEditPropItem(pPropItem,"ucs.axis_z.x");
		pSonItem->SetReadOnly();
		pSonItem=oper.InsertEditPropItem(pPropItem,"ucs.axis_z.y");
		pSonItem->SetReadOnly();
		pSonItem=oper.InsertEditPropItem(pPropItem,"ucs.axis_z.z");
		pSonItem->SetReadOnly();
	}
	else if(pItem&&pItem->m_idProp==GetPropID("NewAssRibPlate")&&m_bNewAssRibPlate)
	{	//�ֹܽ��߰����
		pGroupItem=oper.InsertPropItem(pRoot,"profile");
		oper.InsertEditPropItem(pGroupItem,"ass_rib_para.RotAngle");
		oper.InsertEditPropItem(pGroupItem,"ass_rib_para.E");
		oper.InsertEditPropItem(pGroupItem,"ass_rib_para.G");
		oper.InsertEditPropItem(pGroupItem,"ass_rib_para.F");
		oper.InsertEditPropItem(pGroupItem,"ass_rib_para.H");
		oper.InsertEditPropItem(pGroupItem,"ass_rib_para.e");
		oper.InsertEditPropItem(pGroupItem,"ass_rib_para.I");
		pPropItem=oper.InsertEditPropItem(pGroupItem,"ass_rib_para.MH");
		if(ass_rib_para.MH>0)
		{
			oper.InsertEditPropItem(pPropItem,"ass_rib_para.MF");
			oper.InsertEditPropItem(pPropItem,"ass_rib_para.MG");
		}
	}
	else 
	{	//��ʾ���β���
		pGroupItem=oper.InsertPropItem(pRoot,"profile");
		if(m_pDatumAngle)
		{
			if(m_cPlateType==1)
			{	//���Ǹ��߰�
				oper.InsertEditPropItem(pGroupItem,"para.HX");
				oper.InsertEditPropItem(pGroupItem,"para.HY");
				oper.InsertEditPropItem(pGroupItem,"para.WX");
				oper.InsertEditPropItem(pGroupItem,"para.WY");
				oper.InsertEditPropItem(pGroupItem,"para.S","����Բ��R","����Բ��R",-1,TRUE);
			}
			else if(m_cPlateType==0)
			{	//�ڲ�Ǹ��߰�
				oper.InsertEditPropItem(pGroupItem,"para.LX");
				oper.InsertEditPropItem(pGroupItem,"para.LY");
				oper.InsertEditPropItem(pGroupItem,"para.WX");
				oper.InsertEditPropItem(pGroupItem,"para.WY");
				oper.InsertCmbListPropItem(pGroupItem, "para.E");
				oper.InsertEditPropItem(pGroupItem,"para.S");
				pPropItem=oper.InsertCmbListPropItem(pGroupItem,"para.C");
				if(para.C==1)
					oper.InsertEditPropItem(pPropItem,"para.R");
				pPropItem=oper.InsertEditPropItem(pGroupItem,"para.d");
				if(para.d>0)
					oper.InsertEditPropItem(pPropItem,"para.P");
			}
			else if(m_cPlateType==2)
			{
				oper.InsertEditPropItem(pGroupItem,"para.LX","�߰峤��L","�߰峤��",-1,TRUE);
				oper.InsertEditPropItem(pGroupItem,"para.WX","�߰���W","�߰���",-1,TRUE);
				oper.InsertEditPropItem(pGroupItem,"para.R","����Բ��R","����Բ��R",-1,TRUE);
				oper.InsertEditPropItem(pGroupItem,"para.d","���Ŀ׾�d","���Ŀ׾�d",-1,TRUE);
			}
		}
		else if(m_pDatumTube)
		{
			oper.InsertEditPropItem(pGroupItem,"stay_wire_para.L","�߰峤��L","�߰峤��",-1,TRUE);
			oper.InsertEditPropItem(pGroupItem,"stay_wire_para.W","�߰���W","�׵����ܵľ���",-1,TRUE);
			oper.InsertEditPropItem(pGroupItem,"stay_wire_para.H","���ӿ��H","���ӿ��",-1,TRUE);
			oper.InsertEditPropItem(pGroupItem,"stay_wire_para.R","����Բ��R","����Բ��R",-1,TRUE);
			oper.InsertEditPropItem(pGroupItem,"stay_wire_para.N","���߿���","���߿���",-1,TRUE);
			oper.InsertEditPropItem(pGroupItem,"stay_wire_para.d","���߿׾�D","���߿׾�",-1,TRUE);
			oper.InsertEditPropItem(pGroupItem,"stay_wire_para.P","���߿׾�P","���߿׾�",-1,TRUE);
		}
	}
	m_xSubPropList.Redraw();
}
void CDesLineAngleRibPlateDlg::UpdateSketchMap(CPropTreeItem *pItem/*=NULL*/)
{
	m_xCurPlateBmp.Detach();
	if(m_pDatumAngle)
	{
		if(m_cPlateType==0&&para.C==0)
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE);
		else if(m_cPlateType==0&&para.C==1)
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE1);
		else if(m_cPlateType==1)
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE2);
		else if(m_cPlateType==2)
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE3);
	}
	else if(m_pDatumTube)
	{
		if( pItem&&m_bNewAssRibPlate&&
			(pItem->m_idProp==GetPropID("NewAssRibPlate")||
			 pItem->m_idProp==GetPropID("ass_rib_para.sLable")||
			 pItem->m_idProp==GetPropID("ass_rib_para.thick")))
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CORNER_RIB_PLATE);
		else
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_STAYWIRE_RIBPLATE);
	}
	//
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_MAP);
	pPic->SetBitmap(m_xCurPlateBmp);
}
void CDesLineAngleRibPlateDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	long hPart=0;
	if(GetEventPropId()==GetPropID("hDatumAngle"))
	{
		CLDSLineAngle *pPart=(CLDSLineAngle*)console.FromPartHandle(m_hPickObj,CLS_LINEANGLE);
		if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			m_pDatumAngle=pPart;
			ModifyPlateParam('A',pPart->handle);
		}
	}
	else if(GetEventPropId()==GetPropID("hDatumBolt"))
	{
		CLDSBolt *pBolt=(CLDSBolt*)console.FromPartHandle(m_hPickObj,CLS_BOLT);
		if(pBolt)
		{
			m_pDatumBolt=pBolt;
			m_pDatumNode=NULL;
			ModifyPlateParam('B',pBolt->handle);
		}
	}
	else if(GetEventPropId()==GetPropID("hDatumNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
		if(pNode)
		{
			m_pDatumNode=pNode;
			m_pDatumBolt=NULL;
			ModifyPlateParam('B',pNode->handle);
		}
	}
	if(GetEventPropId()==GetPropID("hDatumTube"))
	{
		CLDSLineTube *pPart=(CLDSLineTube*)console.FromPartHandle(m_hPickObj,CLS_LINETUBE);
		if(pPart)
		{
			m_pDatumTube=pPart;
			ModifyPlateParam('A',pPart->handle);
		}
	}
	if(GetEventPropId()==GetPropID("hDatumPlate"))
	{
		CLDSGeneralPlate *pPart=(CLDSGeneralPlate*)console.FromPartHandle(m_hPickObj,CLS_PLATE,CLS_PARAMPLATE);
		if(pPart)
		{
			m_pAssistPlate=pPart;
			ModifyPlateParam('C',pPart->handle);
		}
	}
	ReDesignPlate();	//��������߰�
}
void CDesLineAngleRibPlateDlg::AutoCalRibPlateWidth()
{
	if(m_pDatumTube==NULL || m_pAssistPlate==NULL)
		return;
	double width=30,tube_r=m_pDatumTube->GetDiameter()*0.5;
	if(m_pAssistPlate->GetClassTypeId()==CLS_PLATE)
	{	//
		CLDSPlate *pPlate=(CLDSPlate*)m_pAssistPlate;
		double inner_r=0,outer_r=0;
		int nVertex=pPlate->vertex_list.GetNodeNum();
		for(int j=0;j<nVertex;j++)
		{
			PROFILE_VERTEX *pStartVertex=&pPlate->vertex_list[j];
			PROFILE_VERTEX *pEndVertex=&pPlate->vertex_list[(j+1)/nVertex];
			if(pStartVertex->type==0)
				continue;
			double cur_r=0;
			if(pStartVertex->type==1)	//ָ��Բ���н�
			{
				int sign=1;
				f3dArcLine arcline;
				if(pStartVertex->sector_angle<0)
					sign=-1;
				arcline.CreateMethod2(pStartVertex->vertex,pEndVertex->vertex,f3dPoint(0,0,sign),sign*pStartVertex->sector_angle);
				cur_r=arcline.Radius();
			}
			else 
				cur_r=fabs(pStartVertex->radius);
			if(fabs(cur_r-tube_r)<10)	//10Ϊ���ļӹ���϶ֵ,�жϵ�ǰԲ���Ƿ���ֹ����� wht 11-08-29
				inner_r=cur_r;
			else 
				outer_r=max(outer_r,cur_r);
		}
		double r=(int)(pPlate->GetWidth()*0.5);
		if(fabs(inner_r)>0)
		{	//�ְ������ֹ���������Բ��
			if(outer_r>inner_r)
				width=outer_r-inner_r;
			else 
				width=r;
		}
		else
			width=r-tube_r;
	}
	else if(m_pAssistPlate->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate* pParaPlate=(CLDSParamPlate*)m_pAssistPlate;
		if(pParaPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{
			double W=0,R=0;
			pParaPlate->GetDesignItemValue('W',&W);
			pParaPlate->GetDesignItemValue('R',&R);
			width=W;
		}
		else if(pParaPlate->m_iParamType==TYPE_CIRRIBPLATE)
		{
			double W=0,Ro=0,V=0,D=0;
			pParaPlate->GetDesignItemValue('V',&V);
			pParaPlate->GetDesignItemValue('D',&D);
			pParaPlate->GetDesignItemValue('R',&Ro);
			double Ri=tube_r+V;	//��Բ���뾶
			if(Ro>0)	//���ͷ����������Խ�������ָ������Բ���뾶����ֵ
				D=Ro-Ri;
			width=D;
		}
	}
	//
	stay_wire_para.H=width;	
}
void CDesLineAngleRibPlateDlg::InitRibPlatePara()
{
	if(!m_bNewRibPlate)
		return;
	if(m_pDatumAngle)
	{
		m_nSegI=m_pDatumAngle->iSeg;
		m_cMaterial=m_pDatumAngle->cMaterial;
		m_cPlateType=0;	//�ڲ�
		para.m_fAngleWidth=m_pDatumAngle->GetWidth();
		para.D=0;
		para.W=30;
		para.Wy=para.W;
		para.S=10;
		para.C=0;
		para.E = 0;
		if(m_bValidStaticRibParam)
		{	//�ϴ�¼�����Ч����
			m_nSegI = s_xBasicParam.nSegI;
			m_cMaterial = s_xBasicParam.cMaterial;
			m_nThick = s_xBasicParam.nThick;
			m_cPlateType = s_xBasicParam.cPlateType;
			//
			para = s_xRibPlateParam;
		}
		if(!m_bValidStaticRibParam||fabs(s_xRibPlateParam.m_fAngleWidth-m_pDatumAngle->GetWidth())>EPS_COS)
		{
			para.H=m_pDatumAngle->GetWidth();
			para.Hy=para.H;
			para.L=m_pDatumAngle->GetWidth()-m_pDatumAngle->GetThick();
			para.Ly=para.L;
		}
	}
	else if(m_pDatumTube)
	{
		AutoCalRibPlateWidth();
		m_nSegI=m_pDatumTube->iSeg;
		m_cMaterial=m_pDatumTube->cMaterial;
		stay_wire_para.J=m_pDatumTube->m_bHasWeldRoad?1:0;
		stay_wire_para.rotAngle=0;	
		stay_wire_para.L=550;
		stay_wire_para.W=140;
		stay_wire_para.d=40;
		stay_wire_para.N=3;
		stay_wire_para.R=60;
		stay_wire_para.P=150;
		if (m_bValidStaticStayWireParam)
		{
			m_nSegI = s_xBasicParam.nSegI;
			m_cMaterial = s_xBasicParam.cMaterial;
			m_nThick = s_xBasicParam.nThick;
			//
			stay_wire_para = s_xStayWireParam;
		}
		//
		ass_rib_para.thick=m_nThick;
		ass_rib_para.fRotAngle=22.5;
		ass_rib_para.E=ass_rib_para.F=100;
		ass_rib_para.G=ass_rib_para.H=100;
		ass_rib_para.I=30;
		if(m_bValidStaticAssRibParam)
		{
			ass_rib_para=s_xAssRibParam;
			m_bNewAssRibPlate=TRUE;
		}
		if(m_pAssistPlate)
		{	//���߰�����ѡ����·����� wht 18-11-15
			if(m_pAssistPlate->ucs.axis_z.z>0)
				stay_wire_para.bOnRelaPlateTopFace=1;
			else
				stay_wire_para.bOnRelaPlateTopFace=-1;
		}
	}
}
CLDSParamPlate *CDesLineAngleRibPlateDlg::CreateRibPlate(CLDSParamPlate* pToExistPlate/*=NULL*/)
{
	BOOL bNewCreate=FALSE;
	CLDSParamPlate *pRibPlate=NULL;
	if(pToExistPlate==NULL&&(m_pRibPlate==NULL||m_pRibPlate==&_previewRibPlate))
	{
		m_pRibPlate=pRibPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		bNewCreate=TRUE;
	}
	else if(pToExistPlate!=NULL)	//��Ԥ�øְ�ʱ����������ʱԤ���ְ�_previewParamPlate�������������������¸ְ�
		pRibPlate=pToExistPlate;
	else
		pRibPlate=m_pRibPlate=&_previewRibPlate;
	//
	if(m_pDatumAngle)
	{	//�Ǹ��߰�
		pRibPlate->m_iParamType=TYPE_LINEANGLERIBPLATE;
		pRibPlate->m_hPartWeldParent=m_pDatumAngle->handle;
		pRibPlate->CopyModuleInstanceInfo(m_pDatumAngle);
		pRibPlate->thick=m_nThick;
		pRibPlate->iSeg=m_nSegI;
		pRibPlate->cMaterial=m_cMaterial;
		pRibPlate->SetPartNo(m_sPartNo);
		//
		pRibPlate->SetDesignItemValue('A',m_pDatumAngle->handle,NULL,PARAM_POS,1);
		if(m_pDatumBolt)
			pRibPlate->SetDesignItemValue('B',m_pDatumBolt->handle,NULL,PARAM_POS,1);
		else if(m_pDatumNode)
			pRibPlate->SetDesignItemValue('B',m_pDatumNode->handle,NULL,PARAM_POS,1);
		pRibPlate->SetDesignItemValue('N',(long)m_cNormCalType,NULL,PARAM_POS,1);
		pRibPlate->SetDesignItemValue('I',(long)m_cPlateType,NULL,PARAM_SHAPE,1);
		pRibPlate->SetDesignItemValue('W',para.W,NULL,PARAM_SHAPE);
		pRibPlate->SetDesignItemValue(KEY2C("Wy"),para.Wy,NULL,PARAM_SHAPE);
		pRibPlate->SetDesignItemValue('D',para.D,NULL,PARAM_SHAPE);
		pRibPlate->SetDesignItemValue('L',para.L,NULL,PARAM_SHAPE);
		pRibPlate->SetDesignItemValue(KEY2C("Ly"),para.Ly,NULL,PARAM_SHAPE);
		pRibPlate->SetDesignItemValue('S',para.S,NULL,PARAM_SHAPE);
		if(m_cPlateType==1)
		{
			pRibPlate->SetDesignItemValue('H',para.H,NULL,PARAM_SHAPE);
			pRibPlate->SetDesignItemValue(KEY2C("Hy"),para.Hy,NULL,PARAM_SHAPE);
		}
		else
		{
			pRibPlate->SetDesignItemValue('C',para.C,NULL,PARAM_SHAPE,1);
			pRibPlate->SetDesignItemValue('E',para.E,NULL,PARAM_SHAPE,1);
			pRibPlate->SetDesignItemValue('R',para.R,NULL,PARAM_SHAPE);
			pRibPlate->SetDesignItemValue('d',para.d,NULL,PARAM_SHAPE);
			pRibPlate->SetDesignItemValue('P',para.P,NULL,PARAM_SHAPE);
		}
		if(!m_pDatumAngle->m_bWeldPart)	//��ӽǸ��߰�����ýǸֺ������� wht 18-12-30
			m_pDatumAngle->m_bWeldPart=TRUE;
		m_bValidStaticRibParam=true;
		s_xRibPlateParam=para;
		s_xAssRibParam = ass_rib_para;
		s_xBasicParam.nSegI = m_nSegI;
		s_xBasicParam.cMaterial = m_cMaterial;
		s_xBasicParam.nThick = m_nThick;
		s_xBasicParam.cPlateType = m_cPlateType;
	}
	else if(m_pDatumTube)
	{	//�ֹ����߰�
		pRibPlate->m_iParamType=TYPE_TUBESTAYWIREPLATE;
		pRibPlate->m_hPartWeldParent=m_pDatumTube->handle;
		pRibPlate->CopyModuleInstanceInfo(m_pDatumTube);
		pRibPlate->thick=m_nThick;
		pRibPlate->iSeg=m_nSegI;
		pRibPlate->cMaterial=m_cMaterial;
		pRibPlate->SetPartNo(m_sPartNo);
		//
		long hPlate=m_pAssistPlate->handle;
		hPlate*=stay_wire_para.bOnRelaPlateTopFace?1:-1;
		pRibPlate->SetDesignItemValue(KEY2C("A"),m_pDatumTube->handle,"",PARAM_POS,1);
		pRibPlate->SetDesignItemValue(KEY2C("C"),hPlate,"",PARAM_POS,1);
		pRibPlate->SetDesignItemValue(KEY2C("K"),stay_wire_para.rotAngle,"",PARAM_POS);
		pRibPlate->SetDesignItemValue(KEY2C("J"),stay_wire_para.J,"",PARAM_POS,1);
		pRibPlate->SetDesignItemValue(KEY2C("L"),stay_wire_para.L,"",PARAM_SHAPE);
		pRibPlate->SetDesignItemValue(KEY2C("W"),stay_wire_para.W,"",PARAM_SHAPE);
		pRibPlate->SetDesignItemValue(KEY2C("H"),stay_wire_para.H,"",PARAM_SHAPE);
		pRibPlate->SetDesignItemValue(KEY2C("R"),stay_wire_para.R,"",PARAM_SHAPE);	//���ǰ뾶
		pRibPlate->SetDesignItemValue(KEY2C("d"),stay_wire_para.d,"",PARAM_SHAPE);  //���߿׾�
		pRibPlate->SetDesignItemValue(KEY2C("N"),stay_wire_para.N,"",PARAM_SHAPE,2);  //���߿���
		pRibPlate->SetDesignItemValue(KEY2C("P"),stay_wire_para.P,"",PARAM_SHAPE);  //�׼��
		//
		m_bValidStaticStayWireParam=true;
		s_xStayWireParam=stay_wire_para;
		s_xAssRibParam = ass_rib_para;
		s_xBasicParam.nSegI = m_nSegI;
		s_xBasicParam.cMaterial = m_cMaterial;
		s_xBasicParam.nThick = m_nThick;
	}
	pRibPlate->DesignPlate();
	pRibPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
	g_pSolidDraw->NewSolidPart(pRibPlate->GetSolidPartObject());
	g_pSolidDraw->AddCS(pRibPlate->handle,pRibPlate->ucs);
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(pRibPlate->handle);
	g_pSolidDraw->Draw();
	//�������߰�����С�߰�
	if(m_pDatumTube&&bNewCreate)
	{
		CLDSParamPlate *pAssRibPlate=NULL;
		double rotAngle=0;
		long hPlate=m_pAssistPlate->handle;
		hPlate*=stay_wire_para.bOnRelaPlateTopFace?1:-1;
		for(int i=0;i<2;i++)
		{
			if(i==0)
				rotAngle=stay_wire_para.rotAngle+ass_rib_para.fRotAngle;
			else
				rotAngle=stay_wire_para.rotAngle-ass_rib_para.fRotAngle;
			pAssRibPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pAssRibPlate->m_iParamType=TYPE_RIBPLATE;
			pAssRibPlate->m_hPartWeldParent=m_pDatumTube->handle;
			pAssRibPlate->CopyModuleInstanceInfo(m_pDatumTube);
			pAssRibPlate->thick=ass_rib_para.thick;
			pAssRibPlate->iSeg=m_nSegI;
			pAssRibPlate->cMaterial=m_cMaterial;
			pAssRibPlate->SetPartNo(m_sAssPartNo);
			pAssRibPlate->SetDesignItemValue('A',m_pDatumTube->handle,"",PARAM_POS,1);	//��׼����
			pAssRibPlate->SetDesignItemValue('C',hPlate,"",PARAM_POS,1);				//�߰嶨λ��������1
			pAssRibPlate->SetDesignItemValue('J',stay_wire_para.J,"",PARAM_POS,1);		//�߰嶨λ��׼��:0.��׼�� 1.������
			pAssRibPlate->SetDesignItemValue('K',rotAngle,"",PARAM_POS,0);
			pAssRibPlate->SetDesignItemValue('R',-1,"",PARAM_SHAPE);
			pAssRibPlate->SetDesignItemValue('E',ass_rib_para.E,"",PARAM_SHAPE);	//�߰�ײ����
			pAssRibPlate->SetDesignItemValue('F',ass_rib_para.F,"",PARAM_SHAPE);	//�߰嶥�����
			pAssRibPlate->SetDesignItemValue('G',ass_rib_para.G,"",PARAM_SHAPE);	//�߰��ڲ೤��
			pAssRibPlate->SetDesignItemValue('H',ass_rib_para.H,"",PARAM_SHAPE);	//�߰���೤��
			pAssRibPlate->SetDesignItemValue('e',ass_rib_para.e,"",PARAM_SHAPE,1);	//�߰嵹��ģʽ
			pAssRibPlate->SetDesignItemValue('I',ass_rib_para.I,"",PARAM_SHAPE);	//�߰��нǿ��
			pAssRibPlate->SetDesignItemValue(KEY2C("MF"),ass_rib_para.MF,"",PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_DOUBLE);	//�׾�F
			pAssRibPlate->SetDesignItemValue(KEY2C("MG"),ass_rib_para.MG,"",PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_DOUBLE);	//�׾�G
			pAssRibPlate->SetDesignItemValue(KEY2C("MH"),ass_rib_para.MH,"",PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_DOUBLE);	//�׾�H
			pAssRibPlate->DesignPlate();
			pAssRibPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pAssRibPlate->GetSolidPartObject());
			g_pSolidDraw->Draw();

			m_bValidStaticAssRibParam=true;
			s_xAssRibParam=ass_rib_para;
		}
	}
	return pRibPlate;
}
void CDesLineAngleRibPlateDlg::ReDesignPlate()
{
	if(m_pRibPlate&&m_pRibPlate->DesignPlate())
	{
		ucs=m_pRibPlate->ucs;
		m_pRibPlate->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(m_pRibPlate->GetSolidPartObject());
		g_pSolidDraw->AddCS(m_pRibPlate->handle,m_pRibPlate->ucs);
		CLDSView::RedrawPlateProfileDrawing(m_pRibPlate);
		g_pSolidDraw->Draw();
	}
}
void CDesLineAngleRibPlateDlg::ModifyPlateParam(WORD wKey,double fValue)
{
	if(m_pRibPlate==NULL)
		return;
	DESIGN_PLATE_ITEM *pItem=m_pRibPlate->GetDesignItem(wKey);
	if(pItem)
		pItem->value.fValue=fValue;
	else
		m_pRibPlate->SetDesignItemValue(wKey,fValue,"",PARAM_SHAPE,0);
}
void CDesLineAngleRibPlateDlg::ModifyPlateParam(WORD wKey,long nValue)
{
	if(m_pRibPlate==NULL)
		return;
	DESIGN_PLATE_ITEM *pItem=m_pRibPlate->GetDesignItem(wKey);
	if(pItem)
		pItem->value.hPart=nValue;
	else
		m_pRibPlate->SetDesignItemValue(wKey,nValue,"",PARAM_SHAPE,1);
}
void CDesLineAngleRibPlateDlg::DeletePlateParam(WORD wKey)
{
	if(m_pRibPlate==NULL)
		return;
	m_pRibPlate->DeleteDesignItem(wKey);
}
//
int CDesLineAngleRibPlateDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen=FALSE;
	if (CCallBackDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
void CDesLineAngleRibPlateDlg::OnMove(int x, int y)
{
	CCallBackDialog::OnMove(x, y);
	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}
void CDesLineAngleRibPlateDlg::OnOK()
{
}
void CDesLineAngleRibPlateDlg::OnCancel() 
{
	g_pSolidDraw->DelEnt(_previewRibPlate.handle);
	if(m_bNewRibPlate&&m_pRibPlate)
	{
		g_pSolidDraw->DelCS(m_pRibPlate->handle);
		g_pSolidDraw->DelEnt(m_pRibPlate->handle);
		console.DispPartSet.DeleteNode(m_pRibPlate->handle);
		console.DeletePart(m_pRibPlate->handle);
		g_pSolidDraw->Draw();
	}
	else if(!m_bNewRibPlate&&m_pRibPlate)
	{	//�鿴���߰�����رնԻ���֮������ѡ���߰�
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pRibPlate->handle);
	}
	CDialog::OnCancel();
}
void CDesLineAngleRibPlateDlg::OnClose()
{
	g_pSolidDraw->DelEnt(_previewRibPlate.handle);
	if(m_bNewRibPlate&&m_pRibPlate)
	{
		g_pSolidDraw->DelCS(m_pRibPlate->handle);
		g_pSolidDraw->DelEnt(m_pRibPlate->handle);
		console.DispPartSet.DeleteNode(m_pRibPlate->handle);
		console.DeletePart(m_pRibPlate->handle);
		g_pSolidDraw->Draw();
	}
	else if(!m_bNewRibPlate&&m_pRibPlate)
	{	//�鿴���߰�����رնԻ���֮������ѡ���߰�
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pRibPlate->handle);
	}
	CDialog::OnClose();
}
void CDesLineAngleRibPlateDlg::OnConfirm()
{
	g_pSolidDraw->DelEnt(_previewRibPlate.handle);
	if(m_pRibPlate&&!m_bNewRibPlate)
	{	//�鿴���߰�����رնԻ���֮������ѡ���߰�
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pRibPlate->handle);
	}
	if(m_pRibPlate==&_previewRibPlate)
		m_pRibPlate=NULL;
	CDialog::OnOK();
}
void CDesLineAngleRibPlateDlg::OnBnModelViewOper()
{	//��ʱ�ж�ת����ģ�Ͳ��������У��Ա����û�����άģ�����š���ת�鿴�Ȳ���
	ExecuteCommand(this,GetPropID("preview"));
}

