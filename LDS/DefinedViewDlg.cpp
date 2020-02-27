// DefinedViewDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "GlobalFunc.h"
#include "DefinedViewDlg.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "env_def.h"
#include "PartDrawMarkDlg.h"
#include "XhCharString.h"
#include "ReportPointDlg.h"
#include "LmaDlg.h"
#include "SortFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static BOOL ModifyDrawingAssistItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CDefinedViewDlg::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

static BOOL ModifyDatumPointPropValue(CPropertyList	*pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CDefinedViewDlg *pDefViewDlg=(CDefinedViewDlg*)pPropList->GetParent();
	if(pDefViewDlg==NULL||pDefViewDlg->m_pDrawingAssistant==NULL)
		return FALSE;
	CLDSPoint *pStartPoint=&pDefViewDlg->m_pDrawingAssistant->m_xLineStart;
	CLDSPoint *pEndPoint=&pDefViewDlg->m_pDrawingAssistant->m_xLineEnd;
	char value_str[100]="";
	if(CDefinedViewDlg::GetPropID("m_xLineStart.x")==pItem->m_idProp)
		pStartPoint->SetPositionX(atof(valueStr));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.y")==pItem->m_idProp)
		pStartPoint->SetPositionY(atof(valueStr));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.z")==pItem->m_idProp)
		pStartPoint->SetPositionZ(atof(valueStr));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.hRod")==pItem->m_idProp)	//�Ǹ־��
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.RODEND.hRod));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.direction")==pItem->m_idProp)	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
		pStartPoint->des_para.RODEND.direction=valueStr[0]-'0';
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.offset_wing")==pItem->m_idProp)//0.X֫;1.Y֫
		pStartPoint->des_para.RODEND.offset_wing=valueStr[0]-'0';
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.wing_offset_style")==pItem->m_idProp)	
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(valueStr.CompareNoCase("g")==0)
			pStartPoint->des_para.RODEND.wing_offset_style=0;
		else if(valueStr.CompareNoCase("g1")==0)
			pStartPoint->des_para.RODEND.wing_offset_style=1;
		else if(valueStr.CompareNoCase("g2")==0)
			pStartPoint->des_para.RODEND.wing_offset_style=2;
		else if(valueStr.CompareNoCase("g3")==0)
			pStartPoint->des_para.RODEND.wing_offset_style=3;
		else 
			pStartPoint->des_para.RODEND.wing_offset_style=4;
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pStartPoint->des_para.RODEND.hRod,CLS_LINEANGLE);
		if(pLineAngle)
		{
			JGZJ jgzj;
			if(pLineAngle->m_bEnableTeG&&pStartPoint->des_para.RODEND.offset_wing==0)
				jgzj=pLineAngle->xWingXZhunJu;	//X֫
			else if(pLineAngle->m_bEnableTeG&&pStartPoint->des_para.RODEND.offset_wing==1)
				jgzj=pLineAngle->xWingYZhunJu;	//X֫
			else
				getjgzj(jgzj,pLineAngle->GetWidth());
			if(pStartPoint->des_para.RODEND.wing_offset_style==0)
				pStartPoint->des_para.RODEND.wing_offset_dist=jgzj.g;
			else if(pStartPoint->des_para.RODEND.wing_offset_style==1)
				pStartPoint->des_para.RODEND.wing_offset_dist=jgzj.g1;
			else if(pStartPoint->des_para.RODEND.wing_offset_style==2)
				pStartPoint->des_para.RODEND.wing_offset_dist=jgzj.g2;
			else if(pStartPoint->des_para.RODEND.wing_offset_style==3)
				pStartPoint->des_para.RODEND.wing_offset_dist=jgzj.g3;
		}
		long idProp=CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.wing_offset_dist");
		if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
			pPropList->SetItemPropValue(idProp,CString(value_str));
		pPropList->SetItemReadOnly(idProp,pStartPoint->des_para.RODEND.wing_offset_style!=4);
	}
#endif
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.wing_offset_dist")==pItem->m_idProp)
		pStartPoint->des_para.RODEND.wing_offset_dist=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.len_offset_dist")==pItem->m_idProp)
		pStartPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.bIncOddEffect")==pItem->m_idProp)		
	{	//����ƫ��ֵ��������ͷӰ��
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else 
		if(valueStr.CompareNoCase("��")==0)
#endif
			pStartPoint->des_para.RODEND.bIncOddEffect=TRUE;
		else 
			pStartPoint->des_para.RODEND.bIncOddEffect=FALSE;
	}
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.hLinePart")==pItem->m_idProp)	//�Ǹ־��
		sscanf(valueStr,"%X",&(pStartPoint->des_para.RODNODE.hRod ));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.hNode")==pItem->m_idProp)	//��׼�ڵ���
		sscanf(valueStr,"%X",&(pStartPoint->des_para.RODNODE.hNode));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.direction")==pItem->m_idProp)	
		pStartPoint->des_para.RODNODE.direction=valueStr[0]-'0';
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.offset_wing")==pItem->m_idProp)	
		pStartPoint->des_para.RODNODE.offset_wing=valueStr[0]-'0';
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_style")==pItem->m_idProp)
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(valueStr.CompareNoCase("g")==0)
			pStartPoint->des_para.RODNODE.wing_offset_style=0;
		else if(valueStr.CompareNoCase("g1")==0)
			pStartPoint->des_para.RODNODE.wing_offset_style=1;
		else if(valueStr.CompareNoCase("g2")==0)
			pStartPoint->des_para.RODNODE.wing_offset_style=2;
		else if(valueStr.CompareNoCase("g3")==0)
			pStartPoint->des_para.RODNODE.wing_offset_style=3;
		else 
			pStartPoint->des_para.RODNODE.wing_offset_style=4;
		//
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pStartPoint->des_para.RODNODE.hRod ,CLS_LINEANGLE);
		if(pLineAngle)
		{
			JGZJ jgzj;
			if(pLineAngle->m_bEnableTeG&&pStartPoint->des_para.RODEND.offset_wing==0)
				jgzj=pLineAngle->xWingXZhunJu;	//X֫
			else if(pLineAngle->m_bEnableTeG&&pStartPoint->des_para.RODEND.offset_wing==1)
				jgzj=pLineAngle->xWingYZhunJu;	//X֫
			else
				getjgzj(jgzj,pLineAngle->GetWidth());
			if(pStartPoint->des_para.RODNODE.wing_offset_style==0)
				pStartPoint->des_para.RODNODE.wing_offset_dist=jgzj.g;
			else if(pStartPoint->des_para.RODNODE.wing_offset_style==1)
				pStartPoint->des_para.RODNODE.wing_offset_dist=jgzj.g1;
			else if(pStartPoint->des_para.RODNODE.wing_offset_style==2)
				pStartPoint->des_para.RODNODE.wing_offset_dist=jgzj.g2;
			else if(pStartPoint->des_para.RODNODE.wing_offset_style==3)
				pStartPoint->des_para.RODNODE.wing_offset_dist=jgzj.g3;
		}
		long idProp=CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_dist");
		if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
			pPropList->SetItemPropValue(idProp,CString(value_str));
		pPropList->SetItemReadOnly(idProp,pStartPoint->des_para.RODNODE.wing_offset_style!=4);
	}	
#endif
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_dist")==pItem->m_idProp)
		pStartPoint->des_para.RODNODE.wing_offset_dist=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.len_offset_dist")==pItem->m_idProp)	
		pStartPoint->des_para.RODNODE.len_offset_dist=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.bFaceOffset")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else 
		if(valueStr.CompareNoCase("��")==0)
#endif
			pStartPoint->des_para.RODNODE.bFaceOffset=TRUE;
		else 
			pStartPoint->des_para.RODNODE.bFaceOffset=FALSE;
		pPropList->DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset"));
		pPropList->DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]"));
		pPropList->DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]"));
		pPropList->DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]"));
		if(pStartPoint->des_para.RODNODE.bFaceOffset)
		{	//ƽ���淨��
			CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL,*pRootItem=pPropList->GetRootItem();
			//
			CItemInfo *lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
			lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Offset Plane Normal";
			lpInfo->m_strPropHelp="Offset flatly plane's normal";
#else 
			lpInfo->m_strPropName="ƽ���淨��";
			lpInfo->m_strPropHelp="ƽ���淨��";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset");
			if(pDefViewDlg->GetPropValueStr(pPropItem->m_idProp,value_str)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=value_str;
			int vectorType=CStdVector::GetVectorType(pStartPoint->des_para.RODNODE.vPlaneNormal);
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The x component of offset plane's normal";
#else 
			lpInfo->m_strPropHelp="ƽ���淨��X�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,TRUE);
			pSonPropItem->m_idProp = CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]");
			if(pDefViewDlg->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
			pSonPropItem->SetReadOnly(vectorType!=0);
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The y component of offset plane's normal";
#else 
			lpInfo->m_strPropHelp="ƽ���淨��Y�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,1,TRUE);
			pSonPropItem->m_idProp = CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]");
			if(pDefViewDlg->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
			pSonPropItem->SetReadOnly(vectorType!=0);
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The z component of offset plane's normal";
#else 
			lpInfo->m_strPropHelp="ƽ���淨��Z�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,2,TRUE);
			pSonPropItem->m_idProp = CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]");
			if(pDefViewDlg->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
			pSonPropItem->SetReadOnly(vectorType!=0);
		}
	}
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset")==pItem->m_idProp)
	{
		f3dPoint norm=CStdVector::GetVector(valueStr[0]-'0');
		pStartPoint->des_para.RODNODE.vPlaneNormal=norm;
		long idProp=CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]");
		pPropList->SetItemPropValue(idProp,"%.f",pStartPoint->des_para.RODNODE.vPlaneNormal.x);
		pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
		idProp=CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]");
		pPropList->SetItemPropValue(idProp,"%.f",pStartPoint->des_para.RODNODE.vPlaneNormal.y);
		pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
		idProp=CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]");
		pPropList->SetItemPropValue(idProp,"%.f",pStartPoint->des_para.RODNODE.vPlaneNormal.z);
		pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
	}
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]")==pItem->m_idProp)
		pStartPoint->des_para.RODNODE.vPlaneNormal.x=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]")==pItem->m_idProp)
		pStartPoint->des_para.RODNODE.vPlaneNormal.y=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]")==pItem->m_idProp)
		pStartPoint->des_para.RODNODE.vPlaneNormal.z=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.AXIS_INTERS.hDatum1));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.AXIS_INTERS.hDatum2));
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style1")==pItem->m_idProp)
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(valueStr.CompareNoCase("g")==0)
			pStartPoint->des_para.AXIS_INTERS.wing_offset_style1=0;
		else if(valueStr.CompareNoCase("g1")==0)
			pStartPoint->des_para.AXIS_INTERS.wing_offset_style1=1;
		else if(valueStr.CompareNoCase("g2")==0)
			pStartPoint->des_para.AXIS_INTERS.wing_offset_style1=2;
		else if(valueStr.CompareNoCase("g3")==0)
			pStartPoint->des_para.AXIS_INTERS.wing_offset_style1=3;
		else 
			pStartPoint->des_para.AXIS_INTERS.wing_offset_style1=4;
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pStartPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE);
		if(pLineAngle)
		{
			JGZJ jgzj;
			if(pLineAngle->m_bEnableTeG&&pStartPoint->des_para.RODEND.offset_wing==0)
				jgzj=pLineAngle->xWingXZhunJu;	//X֫
			else if(pLineAngle->m_bEnableTeG&&pStartPoint->des_para.RODEND.offset_wing==1)
				jgzj=pLineAngle->xWingYZhunJu;	//X֫
			else
				getjgzj(jgzj,pLineAngle->GetWidth());
			if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style1==0)
				pStartPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g;
			else if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style1==1)
				pStartPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g1;
			else if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style1==2)
				pStartPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g2;
			else if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style1==3)
				pStartPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g3;
		}
		long idProp=CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist1");
		if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
			pPropList->SetItemPropValue(idProp,CString(value_str));
		pPropList->SetItemReadOnly(idProp,pStartPoint->des_para.AXIS_INTERS.wing_offset_style1!=4);
	}
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style2")==pItem->m_idProp)
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(valueStr.CompareNoCase("g")==0)
			pStartPoint->des_para.AXIS_INTERS.wing_offset_style2=0;
		else if(valueStr.CompareNoCase("g1")==0)
			pStartPoint->des_para.AXIS_INTERS.wing_offset_style2=1;
		else if(valueStr.CompareNoCase("g2")==0)
			pStartPoint->des_para.AXIS_INTERS.wing_offset_style2=2;
		else if(valueStr.CompareNoCase("g3")==0)
			pStartPoint->des_para.AXIS_INTERS.wing_offset_style2=3;
		else 
			pStartPoint->des_para.AXIS_INTERS.wing_offset_style2=4;
		//
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pStartPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
		if(pLineAngle)
		{
			JGZJ jgzj;
			if(pLineAngle->m_bEnableTeG&&pStartPoint->des_para.RODEND.offset_wing==0)
				jgzj=pLineAngle->xWingXZhunJu;	//X֫
			else if(pLineAngle->m_bEnableTeG&&pStartPoint->des_para.RODEND.offset_wing==1)
				jgzj=pLineAngle->xWingYZhunJu;	//X֫
			else
				getjgzj(jgzj,pLineAngle->GetWidth());
			if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style2==0)
				pStartPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g;
			else if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style2==1)
				pStartPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g1;
			else if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style2==2)
				pStartPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g2;
			else if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style2==3)
				pStartPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g3;
		}
		long idProp=CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist2");
		if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
			pPropList->SetItemPropValue(idProp,CString(value_str));
		pPropList->SetItemReadOnly(idProp,pStartPoint->des_para.AXIS_INTERS.wing_offset_style2!=4);
	}
#endif
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist1")==pItem->m_idProp)
		pStartPoint->des_para.AXIS_INTERS.wing_offset_dist1=atol(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist2")==pItem->m_idProp)
		pStartPoint->des_para.AXIS_INTERS.wing_offset_dist2=atol(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector")==pItem->m_idProp)
	{
		f3dPoint vec=CStdVector::GetVector(valueStr[0]-'0');
		pStartPoint->des_para.AXIS_INTERS.near_vector[0]=vec.x;
		pStartPoint->des_para.AXIS_INTERS.near_vector[1]=vec.y;
		pStartPoint->des_para.AXIS_INTERS.near_vector[2]=vec.z;
		long idProp=CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[0]");
		pPropList->SetItemPropValue(idProp,"%.f",pStartPoint->des_para.AXIS_INTERS.near_vector[0]);
		pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
		idProp=CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[1]");
		pPropList->SetItemPropValue(idProp,"%.f",pStartPoint->des_para.AXIS_INTERS.near_vector[1]);
		pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
		idProp=CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[2]");
		pPropList->SetItemPropValue(idProp,"%.f",pStartPoint->des_para.AXIS_INTERS.near_vector[2]);
		pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
	}
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[0]")==pItem->m_idProp)
		pStartPoint->des_para.AXIS_INTERS.near_vector[0]=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[1]")==pItem->m_idProp)
		pStartPoint->des_para.AXIS_INTERS.near_vector[1]=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[2]")==pItem->m_idProp)
		pStartPoint->des_para.AXIS_INTERS.near_vector[2]=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.AXIS_INTERS.hDatum1));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.AXIS_INTERS.hDatum2));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.DATUMLINE.hDatumLine")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.DATUMLINE.hDatumLine));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==pItem->m_idProp)
		pStartPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=valueStr[0]-'0';
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.DATUMPLANE.hDatumPlane")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.DATUMPLANE.hDatumPlane));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==pItem->m_idProp)
		pStartPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=valueStr[0]-'0';
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.hRod")==pItem->m_idProp)	//�Ǹ־��
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.RODEND.hRod));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.direction")==pItem->m_idProp)	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
		pStartPoint->des_para.RODEND.direction=valueStr[0]-'0';
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.len_offset_dist")==pItem->m_idProp)
		pStartPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.hNode")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.hNode));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.hPart")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.hPart));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.datum_pos.x")==pItem->m_idProp)
		pStartPoint->datum_pos.x=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("datum_pos.y")==pItem->m_idProp)
		pStartPoint->datum_pos.y=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.datum_pos.z")==pItem->m_idProp)
		pStartPoint->datum_pos.z=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.SCALE_NODE.hLinePart")==pItem->m_idProp)//��׼�˼����
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.SCALE_NODE.hLinePart));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.SCALE_NODE.hStartNode")==pItem->m_idProp)	//ʼ�˽ڵ���
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.SCALE_NODE.hStartNode));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.SCALE_NODE.hEndNode")==pItem->m_idProp)		//�ն˽ڵ���
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.SCALE_NODE.hEndNode));
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.SCALE_NODE.start_offset_dist")==pItem->m_idProp)//ʼ��ƫ����
		pStartPoint->des_para.SCALE_NODE.start_offset_dist=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.SCALE_NODE.end_offset_dist")==pItem->m_idProp)	//�ն�ƫ����
		pStartPoint->des_para.SCALE_NODE.end_offset_dist=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.SCALE_NODE.offset_scale")==pItem->m_idProp)		//ƫ�Ʊ���
		pStartPoint->des_para.SCALE_NODE.offset_scale=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.SCALE_NODE.offset_dist")==pItem->m_idProp)		//�ڵȷֻ����ϵ�ƫ��ֵ
		pStartPoint->des_para.SCALE_NODE.offset_dist=atof(valueStr);
	else 
	{	//�������յ�
		if(CDefinedViewDlg::GetPropID("m_xLineEnd.x")==pItem->m_idProp)
			pEndPoint->SetPositionX(atof(valueStr));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.y")==pItem->m_idProp)
			pEndPoint->SetPositionY(atof(valueStr));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.z")==pItem->m_idProp)
			pEndPoint->SetPositionZ(atof(valueStr));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.hRod")==pItem->m_idProp)	//�Ǹ־��
			sscanf(valueStr.GetBuffer(1),"%X",&(pEndPoint->des_para.RODEND.hRod));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.direction")==pItem->m_idProp)	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
			pEndPoint->des_para.RODEND.direction=valueStr[0]-'0';
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.offset_wing")==pItem->m_idProp)//0.X֫;1.Y֫
			pEndPoint->des_para.RODEND.offset_wing=valueStr[0]-'0';
	#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.wing_offset_style")==pItem->m_idProp)	
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pEndPoint->des_para.RODEND.wing_offset_style=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pEndPoint->des_para.RODEND.wing_offset_style=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pEndPoint->des_para.RODEND.wing_offset_style=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pEndPoint->des_para.RODEND.wing_offset_style=3;
			else 
				pEndPoint->des_para.RODEND.wing_offset_style=4;
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pEndPoint->des_para.RODEND.hRod,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG&&pEndPoint->des_para.RODEND.offset_wing==0)
					jgzj=pLineAngle->xWingXZhunJu;	//X֫
				else if(pLineAngle->m_bEnableTeG&&pEndPoint->des_para.RODEND.offset_wing==1)
					jgzj=pLineAngle->xWingYZhunJu;	//X֫
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				if(pEndPoint->des_para.RODEND.wing_offset_style==0)
					pEndPoint->des_para.RODEND.wing_offset_dist=jgzj.g;
				else if(pEndPoint->des_para.RODEND.wing_offset_style==1)
					pEndPoint->des_para.RODEND.wing_offset_dist=jgzj.g1;
				else if(pEndPoint->des_para.RODEND.wing_offset_style==2)
					pEndPoint->des_para.RODEND.wing_offset_dist=jgzj.g2;
				else if(pEndPoint->des_para.RODEND.wing_offset_style==3)
					pEndPoint->des_para.RODEND.wing_offset_dist=jgzj.g3;
			}
			long idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.wing_offset_dist");
			if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pEndPoint->des_para.RODEND.wing_offset_style!=4);
		}
	#endif
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.wing_offset_dist")==pItem->m_idProp)
			pEndPoint->des_para.RODEND.wing_offset_dist=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.len_offset_dist")==pItem->m_idProp)
			pEndPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.bIncOddEffect")==pItem->m_idProp)		
		{	//����ƫ��ֵ��������ͷӰ��
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
#endif
				pEndPoint->des_para.RODEND.bIncOddEffect=TRUE;
			else 
				pEndPoint->des_para.RODEND.bIncOddEffect=FALSE;
		}
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.hLinePart")==pItem->m_idProp)	//�Ǹ־��
			sscanf(valueStr.GetBuffer(1),"%X",&(pEndPoint->des_para.RODNODE.hRod ));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.hNode")==pItem->m_idProp)	//��׼�ڵ���
			sscanf(valueStr.GetBuffer(1),"%X",&(pEndPoint->des_para.RODNODE.hNode));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.direction")==pItem->m_idProp)	
			pEndPoint->des_para.RODNODE.direction=valueStr[0]-'0';
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.offset_wing")==pItem->m_idProp)	
			pEndPoint->des_para.RODNODE.offset_wing=valueStr[0]-'0';
	#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.wing_offset_style")==pItem->m_idProp)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pEndPoint->des_para.RODNODE.wing_offset_style=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pEndPoint->des_para.RODNODE.wing_offset_style=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pEndPoint->des_para.RODNODE.wing_offset_style=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pEndPoint->des_para.RODNODE.wing_offset_style=3;
			else 
				pEndPoint->des_para.RODNODE.wing_offset_style=4;
			//
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pEndPoint->des_para.RODNODE.hRod ,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG&&pEndPoint->des_para.RODEND.offset_wing==0)
					jgzj=pLineAngle->xWingXZhunJu;	//X֫
				else if(pLineAngle->m_bEnableTeG&&pEndPoint->des_para.RODEND.offset_wing==1)
					jgzj=pLineAngle->xWingYZhunJu;	//X֫
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				if(pEndPoint->des_para.RODNODE.wing_offset_style==0)
					pEndPoint->des_para.RODNODE.wing_offset_dist=jgzj.g;
				else if(pEndPoint->des_para.RODNODE.wing_offset_style==1)
					pEndPoint->des_para.RODNODE.wing_offset_dist=jgzj.g1;
				else if(pEndPoint->des_para.RODNODE.wing_offset_style==2)
					pEndPoint->des_para.RODNODE.wing_offset_dist=jgzj.g2;
				else if(pEndPoint->des_para.RODNODE.wing_offset_style==3)
					pEndPoint->des_para.RODNODE.wing_offset_dist=jgzj.g3;
			}
			long idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.wing_offset_dist");
			if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pEndPoint->des_para.RODNODE.wing_offset_style!=4);
		}	
	#endif
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.wing_offset_dist")==pItem->m_idProp)
			pEndPoint->des_para.RODNODE.wing_offset_dist=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.len_offset_dist")==pItem->m_idProp)	
			pEndPoint->des_para.RODNODE.len_offset_dist=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.bFaceOffset")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
#endif
				pEndPoint->des_para.RODNODE.bFaceOffset=TRUE;
			else 
				pEndPoint->des_para.RODNODE.bFaceOffset=FALSE;
			pPropList->DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset"));
			pPropList->DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[0]"));
			pPropList->DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[1]"));
			pPropList->DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[2]"));
			if(pEndPoint->des_para.RODNODE.bFaceOffset)
			{	//ƽ���淨��
				CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL,*pRootItem=pPropList->GetRootItem();
				//
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
				lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Offset Plane Normal";
				lpInfo->m_strPropHelp="Offset flatly plane's normal";
#else 
				lpInfo->m_strPropName="ƽ���淨��";
				lpInfo->m_strPropHelp="ƽ���淨��";
#endif
				pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset");
				if(pDefViewDlg->GetPropValueStr(pPropItem->m_idProp,value_str)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				int vectorType=CStdVector::GetVectorType(pEndPoint->des_para.RODNODE.vPlaneNormal);
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The x component of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��X�������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,TRUE);
				pSonPropItem->m_idProp = CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[0]");
				if(pDefViewDlg->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The y component of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��Y�������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,1,TRUE);
				pSonPropItem->m_idProp = CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[1]");
				if(pDefViewDlg->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The z component of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��Z�������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,2,TRUE);
				pSonPropItem->m_idProp = CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[2]");
				if(pDefViewDlg->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
			}
		}
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset")==pItem->m_idProp)
		{
			f3dPoint norm=CStdVector::GetVector(valueStr[0]-'0');
			pEndPoint->des_para.RODNODE.vPlaneNormal=norm;
			long idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[0]");
			pPropList->SetItemPropValue(idProp,"%.f",pEndPoint->des_para.RODNODE.vPlaneNormal.x);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[1]");
			pPropList->SetItemPropValue(idProp,"%.f",pEndPoint->des_para.RODNODE.vPlaneNormal.y);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[2]");
			pPropList->SetItemPropValue(idProp,"%.f",pEndPoint->des_para.RODNODE.vPlaneNormal.z);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
		}
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[0]")==pItem->m_idProp)
			pEndPoint->des_para.RODNODE.vPlaneNormal.x=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[1]")==pItem->m_idProp)
			pEndPoint->des_para.RODNODE.vPlaneNormal.y=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[2]")==pItem->m_idProp)
			pEndPoint->des_para.RODNODE.vPlaneNormal.z=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pEndPoint->des_para.AXIS_INTERS.hDatum1));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pEndPoint->des_para.AXIS_INTERS.hDatum2));
	#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_style1")==pItem->m_idProp)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pEndPoint->des_para.AXIS_INTERS.wing_offset_style1=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pEndPoint->des_para.AXIS_INTERS.wing_offset_style1=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pEndPoint->des_para.AXIS_INTERS.wing_offset_style1=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pEndPoint->des_para.AXIS_INTERS.wing_offset_style1=3;
			else 
				pEndPoint->des_para.AXIS_INTERS.wing_offset_style1=4;
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pEndPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG&&pEndPoint->des_para.RODEND.offset_wing==0)
					jgzj=pLineAngle->xWingXZhunJu;	//X֫
				else if(pLineAngle->m_bEnableTeG&&pEndPoint->des_para.RODEND.offset_wing==1)
					jgzj=pLineAngle->xWingYZhunJu;	//X֫
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style1==0)
					pEndPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g;
				else if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style1==1)
					pEndPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g1;
				else if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style1==2)
					pEndPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g2;
				else if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style1==3)
					pEndPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g3;
			}
			long idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_dist1");
			if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pEndPoint->des_para.AXIS_INTERS.wing_offset_style1!=4);
		}
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_style2")==pItem->m_idProp)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pEndPoint->des_para.AXIS_INTERS.wing_offset_style2=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pEndPoint->des_para.AXIS_INTERS.wing_offset_style2=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pEndPoint->des_para.AXIS_INTERS.wing_offset_style2=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pEndPoint->des_para.AXIS_INTERS.wing_offset_style2=3;
			else 
				pEndPoint->des_para.AXIS_INTERS.wing_offset_style2=4;
			//
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pEndPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG&&pEndPoint->des_para.RODEND.offset_wing==0)
					jgzj=pLineAngle->xWingXZhunJu;	//X֫
				else if(pLineAngle->m_bEnableTeG&&pEndPoint->des_para.RODEND.offset_wing==1)
					jgzj=pLineAngle->xWingYZhunJu;	//X֫
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style2==0)
					pEndPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g;
				else if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style2==1)
					pEndPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g1;
				else if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style2==2)
					pEndPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g2;
				else if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style2==3)
					pEndPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g3;
			}
			long idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_dist2");
			if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pEndPoint->des_para.AXIS_INTERS.wing_offset_style2!=4);
		}
	#endif
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_dist1")==pItem->m_idProp)
			pEndPoint->des_para.AXIS_INTERS.wing_offset_dist1=atol(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_dist2")==pItem->m_idProp)
			pEndPoint->des_para.AXIS_INTERS.wing_offset_dist2=atol(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector")==pItem->m_idProp)
		{
			f3dPoint vec=CStdVector::GetVector(valueStr[0]-'0');
			pEndPoint->des_para.AXIS_INTERS.near_vector[0]=vec.x;
			pEndPoint->des_para.AXIS_INTERS.near_vector[1]=vec.y;
			pEndPoint->des_para.AXIS_INTERS.near_vector[2]=vec.z;
			long idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector[0]");
			pPropList->SetItemPropValue(idProp,"%.f",pEndPoint->des_para.AXIS_INTERS.near_vector[0]);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector[1]");
			pPropList->SetItemPropValue(idProp,"%.f",pEndPoint->des_para.AXIS_INTERS.near_vector[1]);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector[2]");
			pPropList->SetItemPropValue(idProp,"%.f",pEndPoint->des_para.AXIS_INTERS.near_vector[2]);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
		}
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector[0]")==pItem->m_idProp)
			pEndPoint->des_para.AXIS_INTERS.near_vector[0]=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector[1]")==pItem->m_idProp)
			pEndPoint->des_para.AXIS_INTERS.near_vector[1]=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector[2]")==pItem->m_idProp)
			pEndPoint->des_para.AXIS_INTERS.near_vector[2]=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pEndPoint->des_para.AXIS_INTERS.hDatum1));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pEndPoint->des_para.AXIS_INTERS.hDatum2));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.DATUMLINE.hDatumLine")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pEndPoint->des_para.DATUMLINE.hDatumLine));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==pItem->m_idProp)
			pEndPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=valueStr[0]-'0';
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.DATUMPLANE.hDatumPlane")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pEndPoint->des_para.DATUMPLANE.hDatumPlane));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==pItem->m_idProp)
			pEndPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=valueStr[0]-'0';
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.hRod")==pItem->m_idProp)	//�Ǹ־��
			sscanf(valueStr.GetBuffer(1),"%X",&(pEndPoint->des_para.RODEND.hRod));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.direction")==pItem->m_idProp)	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
			pEndPoint->des_para.RODEND.direction=valueStr[0]-'0';
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.len_offset_dist")==pItem->m_idProp)
			pEndPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.hNode")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pEndPoint->des_para.hNode));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.hPart")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pEndPoint->des_para.hPart));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.datum_pos.x")==pItem->m_idProp)
			pEndPoint->datum_pos.x=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("datum_pos.y")==pItem->m_idProp)
			pEndPoint->datum_pos.y=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.datum_pos.z")==pItem->m_idProp)
			pEndPoint->datum_pos.z=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.SCALE_NODE.hLinePart")==pItem->m_idProp)//��׼�˼����
			sscanf(valueStr.GetBuffer(1),"%X",&(pEndPoint->des_para.SCALE_NODE.hLinePart));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.SCALE_NODE.hStartNode")==pItem->m_idProp)	//ʼ�˽ڵ���
			sscanf(valueStr.GetBuffer(1),"%X",&(pEndPoint->des_para.SCALE_NODE.hStartNode));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.SCALE_NODE.hEndNode")==pItem->m_idProp)		//�ն˽ڵ���
			sscanf(valueStr.GetBuffer(1),"%X",&(pEndPoint->des_para.SCALE_NODE.hEndNode));
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.SCALE_NODE.start_offset_dist")==pItem->m_idProp)//ʼ��ƫ����
			pEndPoint->des_para.SCALE_NODE.start_offset_dist=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.SCALE_NODE.end_offset_dist")==pItem->m_idProp)	//�ն�ƫ����
			pEndPoint->des_para.SCALE_NODE.end_offset_dist=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.SCALE_NODE.offset_scale")==pItem->m_idProp)		//ƫ�Ʊ���
			pEndPoint->des_para.SCALE_NODE.offset_scale=atof(valueStr);
		else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.SCALE_NODE.offset_dist")==pItem->m_idProp)		//�ڵȷֻ����ϵ�ƫ��ֵ
			pEndPoint->des_para.SCALE_NODE.offset_dist=atof(valueStr);
		else
			return FALSE;
		if(pEndPoint->datum_pos_style!=0)
		{
			pEndPoint->UpdatePos(console.GetActiveModel());
			long idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.x");
			if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,value_str);
			idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.y");
			if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,value_str);
			idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.z");
			if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,value_str);
		}
		return TRUE;
	}
	if(pStartPoint->datum_pos_style!=0)
	{
		pStartPoint->UpdatePos(console.GetActiveModel());
		long idProp=CDefinedViewDlg::GetPropID("m_xLineStart.x");
		if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
			pPropList->SetItemPropValue(idProp,value_str);
		idProp=CDefinedViewDlg::GetPropID("m_xLineStart.y");
		if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
			pPropList->SetItemPropValue(idProp,value_str);
		idProp=CDefinedViewDlg::GetPropID("m_xLineStart.z");
		if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
			pPropList->SetItemPropValue(idProp,value_str);
	}
	return TRUE;
}

static BOOL ModifyDatumVectorPropValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CDefinedViewDlg *pDefViewDlg=(CDefinedViewDlg*)pPropList->GetParent();
	if(pDefViewDlg==NULL||pDefViewDlg->m_pDrawingAssistant==NULL)
		return FALSE;
	CLDSVector *pVector=&(pDefViewDlg->m_pDrawingAssistant->m_xFaceNorm);
	if(CDefinedViewDlg::GetPropID("m_xFaceNorm.hVicePart")==pItem->m_idProp)//��׼�˼����
		sscanf(valueStr,"%d",&(pVector->hVicePart));
	else if(CDefinedViewDlg::GetPropID("m_xFaceNorm.hCrossPart")==pItem->m_idProp)
		sscanf(valueStr,"%d",&(pVector->hCrossPart));
	else if(CDefinedViewDlg::GetPropID("m_xFaceNorm.norm_wing")==pItem->m_idProp)
		pVector->norm_wing=valueStr[0]-'0';
	else if(CDefinedViewDlg::GetPropID("m_xFaceNorm.direction")==pItem->m_idProp)
	{
		pVector->direction=valueStr[0]-'0';
		if(pVector->norm_style==5)
		{
			CSuperSmartPtr<CLDSPart>pDatumPart=console.FromPartHandle(pVector->hVicePart);
			if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
			{
				if(pVector->direction<2)
					pPropList->DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xFaceNorm.rot_ang"));
				else
				{
					int idProp=CDefinedViewDlg::GetPropID("m_xFaceNorm.rot_ang");
					if(pPropList->FindItemByPropId(idProp,NULL)==NULL)
					{
						CItemInfo* lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_COMBO;
						lpInfo->m_cmbType=CDT_EDIT;
						lpInfo->m_cmbItems = "0|90|180|270";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Rotation Angle";
						if(pVector->direction==2)
							lpInfo->m_strPropHelp="The rotation angle from tube's datum plane around the axial line(��)";
						else 
							lpInfo->m_strPropHelp="The rotation angle from tube's weld plane around the axial line(��)";
#else 
						lpInfo->m_strPropName = "��ת�Ƕ�";
						if(pVector->direction==2)
							lpInfo->m_strPropHelp="�Ըֹܻ�׼��ƽ�淨�������ߵ���ת�Ƕ�(��)";
						else 
							lpInfo->m_strPropHelp="�Ըֹܺ�����ƽ�淨�������ߵ���ת�Ƕ�(��)";
#endif
						CPropTreeItem* pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
						pSonItem->m_idProp = idProp;
						char item_text[100]="";
						if(pDefViewDlg->GetPropValueStr(pSonItem->m_idProp,item_text)>0)
							pSonItem->m_lpNodeInfo->m_strPropValue = item_text;
					}
				}
			}
		}
	}
	else if(CDefinedViewDlg::GetPropID("m_xFaceNorm.rot_ang")==pItem->m_idProp)
	{
		pVector->rot_ang = atof(valueStr);
	}
	else if(CDefinedViewDlg::GetPropID("m_xFaceNorm.VectorType")==pItem->m_idProp)
	{
		if(valueStr[0]=='0')	//�Զ��巽��
		{
			pPropList->SetItemReadOnly(CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.x"),pVector->norm_style!=0);
			pPropList->SetItemReadOnly(CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.y"),pVector->norm_style!=0);
			pPropList->SetItemReadOnly(CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.z"),pVector->norm_style!=0);
		}
		else
		{
			pPropList->SetItemReadOnly(CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.x"),TRUE);
			pPropList->SetItemReadOnly(CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.y"),TRUE);
			pPropList->SetItemReadOnly(CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.z"),TRUE);
			f3dPoint vector=CStdVector::GetVector(valueStr[0]-'0');
			pVector->vector=vector;
			CString sText;
			sText.Format("%.f",vector.x);
			pPropList->SetItemPropValue(CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.x"),sText);
			sText.Format("%.f",vector.y);
			pPropList->SetItemPropValue(CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.y"),sText);
			sText.Format("%.f",vector.z);
			pPropList->SetItemPropValue(CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.z"),sText);
		}
	}
	else if(CDefinedViewDlg::GetPropID("m_xFaceNorm.nearVectorType")==pItem->m_idProp)
	{
		if(valueStr[0]-'0'==0)	//�Զ��巽��
		{
			CXhChar100 sText;
			CItemInfo *lpInfo;
			CPropTreeItem *pSonItem,*pFindItem=pPropList->FindItemByPropId(CDefinedViewDlg::GetPropID("m_xFaceNorm.nearVector.x"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "X Component";
				lpInfo->m_strPropHelp = "X Component";
#else 
				lpInfo->m_strPropName = "X����";
				lpInfo->m_strPropHelp = "X����";
#endif
				pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =CDefinedViewDlg::GetPropID("m_xFaceNorm.nearVector.x");
				if(pDefViewDlg->GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
			
			pFindItem=pPropList->FindItemByPropId(CDefinedViewDlg::GetPropID("m_xFaceNorm.nearVector.y"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Y Component";
				lpInfo->m_strPropHelp = "Y Component";
#else 
				lpInfo->m_strPropName = "Y����";
				lpInfo->m_strPropHelp = "Y����";
#endif
				pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =CDefinedViewDlg::GetPropID("m_xFaceNorm.nearVector.y");
				if(pDefViewDlg->GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
			
			pFindItem=pPropList->FindItemByPropId(CDefinedViewDlg::GetPropID("m_xFaceNorm.nearVector.z"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Z Component";
				lpInfo->m_strPropHelp = "Z Component";
#else 
				lpInfo->m_strPropName = "Z����";
				lpInfo->m_strPropHelp = "Z����";
#endif
				pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =CDefinedViewDlg::GetPropID("m_xFaceNorm.nearVector.z");
				if(pDefViewDlg->GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else
		{
			pPropList->DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xFaceNorm.nearVector.x"));
			pPropList->DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xFaceNorm.nearVector.y"));
			pPropList->DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xFaceNorm.nearVector.z"));
			pVector->nearVector=CStdVector::GetVector(valueStr[0]-'0');
		}
	}
	else if(CDefinedViewDlg::GetPropID("m_xFaceNorm.nearVector.x")==pItem->m_idProp)
		pVector->nearVector.x=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xFaceNorm.nearVector.y")==pItem->m_idProp)
		pVector->nearVector.y=atof(valueStr);
	else if(CDefinedViewDlg::GetPropID("m_xFaceNorm.nearVector.z")==pItem->m_idProp)
		pVector->nearVector.z=atof(valueStr);
	else
		return FALSE;
	if(pVector->norm_style!=0)
	{
		char value_str[100]="";
		pVector->UpdateVector(console.GetActiveModel());
		long idProp=CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.x");
		if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
			pPropList->SetItemPropValue(idProp,value_str);
		idProp=CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.y");
		if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
			pPropList->SetItemPropValue(idProp,value_str);
		idProp=CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.z");
		if(pDefViewDlg->GetPropValueStr(idProp,value_str)>0)
			pPropList->SetItemPropValue(idProp,value_str);
	}
	return TRUE;
}

static BOOL ModifyDrawAssistPropValue(CPropertyList	*pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CDefinedViewDlg *pDefViewDlg=(CDefinedViewDlg*)pPropList->GetParent();
	if(pDefViewDlg==NULL||pDefViewDlg->m_pDrawingAssistant==NULL)
		return FALSE;
	CLDSPoint *pStartPoint=&(pDefViewDlg->m_pDrawingAssistant->m_xLineStart);
	CLDSPoint *pEndPoint=&(pDefViewDlg->m_pDrawingAssistant->m_xLineEnd);
	CLDSVector *pVector=&(pDefViewDlg->m_pDrawingAssistant->m_xFaceNorm);
	if(CDefinedViewDlg::GetPropID("residePlane")==pItem->m_idProp)
		pDefViewDlg->m_pDrawingAssistant->iResidePlaneNo=valueStr[0]-'0';
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.datum_pos_style")==pItem->m_idProp)
	{
		pStartPoint->datum_pos_style=valueStr[0]-'0';
		pDefViewDlg->UpdateDatumPointPropItem(pItem,pStartPoint,TRUE);
	}
	else if(CDefinedViewDlg::GetPropID("m_xLineEnd.datum_pos_style")==pItem->m_idProp)
	{
		pEndPoint->datum_pos_style=valueStr[0]-'0';
		pDefViewDlg->UpdateDatumPointPropItem(pItem,pEndPoint,TRUE);
	}
	else if(CDefinedViewDlg::GetPropID("m_xFaceNorm.norm_style")==pItem->m_idProp)
	{
		pVector->norm_style=valueStr[0]-'0';
		pDefViewDlg->UpdateDatumVectorPropItem(pItem,TRUE);
	}
	else 
	{
		BOOL bRetCode=ModifyDatumPointPropValue(pPropList,pItem,valueStr);
		if(!bRetCode)
			bRetCode=ModifyDatumVectorPropValue(pPropList,pItem,valueStr);
		return bRetCode;
	}
	return TRUE;
}

static BOOL ButtonClickDrawAssistProp(CPropertyList	*pPropList,CPropTreeItem *pItem)
{
	CDefinedViewDlg *pDefViewDlg=(CDefinedViewDlg*)pPropList->GetParent();
	if(pDefViewDlg==NULL||pDefViewDlg->m_pDrawingAssistant==NULL)
		return FALSE;
	CLDSPoint *pStartPoint=&pDefViewDlg->m_pDrawingAssistant->m_xLineStart;
	CLDSPoint *pEndPoint=&pDefViewDlg->m_pDrawingAssistant->m_xLineEnd;
	CLDSVector *pVector=&(pDefViewDlg->m_pDrawingAssistant->m_xFaceNorm);
	char sText[100]="";
	BOOL bRetCode=TRUE;
	if(CDefinedViewDlg::GetPropID("m_xLineStart.pos")==pItem->m_idProp)
	{
		CReportPointDlg pointDlg;
#ifdef AFX_TARG_ENU_ENGLISH
		if(pDefViewDlg->m_pDrawingAssistant->m_nStyle==0)
			pointDlg.m_sCaption = "Spread Plane Datum Point";
		else
			pointDlg.m_sCaption = "Ancillary Line Start Point";
#else 
		if(pDefViewDlg->m_pDrawingAssistant->m_nStyle==0)
			pointDlg.m_sCaption = "չ�����׼��";
		else
			pointDlg.m_sCaption = "������ʼ��";
#endif
		pointDlg.m_fPointX = pStartPoint->Position().x;
		pointDlg.m_fPointY = pStartPoint->Position().y;
		pointDlg.m_fPointZ = pStartPoint->Position().z;
		if(pStartPoint->datum_pos_style!=0)
			pointDlg.m_bReadOnly=TRUE;
		if(pointDlg.DoModal()==IDOK)
		{
			pStartPoint->SetPosition(f3dPoint(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ));
			long idProp=CDefinedViewDlg::GetPropID("m_xLineStart.x");
			if(pDefViewDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			idProp=CDefinedViewDlg::GetPropID("m_xLineStart.y");
			if(pDefViewDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			idProp=CDefinedViewDlg::GetPropID("m_xLineStart.z");
			if(pDefViewDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
		}
	}
	else if(CDefinedViewDlg::GetPropID("m_xLineEnd.pos")==pItem->m_idProp)
	{
		CReportPointDlg pointDlg;
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "Ancillary Line End Point";
#else 
		pointDlg.m_sCaption = "�������յ�";
#endif
		pointDlg.m_fPointX = pEndPoint->Position().x;
		pointDlg.m_fPointY = pEndPoint->Position().y;
		pointDlg.m_fPointZ = pEndPoint->Position().z;
		if(pEndPoint->datum_pos_style!=0)
			pointDlg.m_bReadOnly=TRUE;
		if(pointDlg.DoModal()==IDOK)
		{
			pEndPoint->SetPosition(f3dPoint(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ));
			long idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.x");
			if(pDefViewDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.y");
			if(pDefViewDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			idProp=CDefinedViewDlg::GetPropID("m_xLineEnd.z");
			if(pDefViewDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
		}
	}
	else if(CDefinedViewDlg::GetPropID("m_xFaceNorm.norm")==pItem->m_idProp)
	{
		CReportVectorDlg vecDlg;
		vecDlg.m_fVectorX=pVector->vector.x;
		vecDlg.m_fVectorY=pVector->vector.y;
		vecDlg.m_fVectorZ=pVector->vector.z;
		vecDlg.m_bReadOnlyX=(pVector->norm_style!=0);
		vecDlg.m_bReadOnlyY=(pVector->norm_style!=0);
		vecDlg.m_bReadOnlyZ=(pVector->norm_style!=0);
		if(vecDlg.DoModal()==IDOK)
		{
			pVector->vector.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			normalize(pVector->vector);
			long idProp=CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.x");
			if(pDefViewDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			idProp=CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.y");
			if(pDefViewDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			idProp=CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.z");
			if(pDefViewDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);	
			idProp=CDefinedViewDlg::GetPropID("m_xFaceNorm.VectorType");
			if(pDefViewDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);	
		}
	}
	if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.hRod")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);//��׼�Ǹ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.norm_offset")==pItem->m_idProp)
	{	//����ƫ����

		CString sThickStr;
		sThickStr=pStartPoint->des_para.RODEND.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
		Input("Input the offset string along normal",&sThickStr,'s');
#else 
		Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
		_snprintf(pStartPoint->des_para.RODEND.norm_offset,49,"%s",sThickStr);
		if(pDefViewDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
	}
#endif
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.hLinePart")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.hNode")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_NODE,pItem->m_idProp);		//��׼�ڵ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.norm_offset")==pItem->m_idProp)
	{	//����ƫ����
		CString sThickStr;
		sThickStr=pStartPoint->des_para.RODNODE.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
		Input("Input the offset string along normal",&sThickStr,'s');
#else 
		Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
		_snprintf(pStartPoint->des_para.RODNODE.norm_offset,49,"%s",sThickStr);
		if(pDefViewDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
	}
#endif
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�1
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�2
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.norm_offset")==pItem->m_idProp)
	{	//����ƫ����
		CString sThickStr;
		sThickStr=pStartPoint->des_para.AXIS_INTERS.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
		Input("Input the offset string along normal",&sThickStr,'s');
#else 
		Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
		_snprintf(pStartPoint->des_para.AXIS_INTERS.norm_offset,49,"%s",sThickStr);
		if(pDefViewDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
	}
#endif
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.hRod")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);//��׼�Ǹ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.norm_offset")==pItem->m_idProp)
	{	//����ƫ����
		
		CString sThickStr;
		sThickStr=pStartPoint->des_para.RODEND.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
		Input("Input the offset string along normal",&sThickStr,'s');
#else 
		Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
		_snprintf(pStartPoint->des_para.RODEND.norm_offset,49,"%s",sThickStr);
		if(pDefViewDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
	}
#endif
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.hNode")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_NODE,pItem->m_idProp);	//��׼�ڵ�
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.hPart")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_PART,pItem->m_idProp);	//��׼����
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.datum_pos")==pItem->m_idProp)
	{
		CReportPointDlg pointDlg;
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "Relative Coordinates";
#else 
		pointDlg.m_sCaption = "�������";
#endif
		pointDlg.m_fPointX = pStartPoint->datum_pos.x;
		pointDlg.m_fPointY = pStartPoint->datum_pos.y;
		pointDlg.m_fPointZ = pStartPoint->datum_pos.z;
		if(pointDlg.DoModal()==IDOK)
		{
			CXhChar100 sText("%f",pointDlg.m_fPointX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CDefinedViewDlg::GetPropID("m_xLineStart.datum_pos.x"),CString(sText));
			sText.Printf("%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CDefinedViewDlg::GetPropID("m_xLineStart.datum_pos.y"),CString(sText));
			sText.Printf("%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CDefinedViewDlg::GetPropID("m_xLineStart.datum_pos.z"),CString(sText));
			pStartPoint->datum_pos.Set(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ);
		}
	}
	else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.hRod")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);//��׼�Ǹ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.norm_offset")==pItem->m_idProp)
	{	//����ƫ����

		CString sThickStr;
		sThickStr=pStartPoint->des_para.RODEND.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
		Input("Input the offset string along normal",&sThickStr,'s');
#else 
		Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
		_snprintf(pStartPoint->des_para.RODEND.norm_offset,49,"%s",sThickStr);
		if(pDefViewDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
	}
#endif
	else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.hLinePart")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�
	else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.hNode")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_NODE,pItem->m_idProp);		//��׼�ڵ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.norm_offset")==pItem->m_idProp)
	{	//����ƫ����
		CString sThickStr;
		sThickStr=pEndPoint->des_para.RODNODE.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
		Input("Input the offset string along normal",&sThickStr,'s');
#else 
		Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
		_snprintf(pEndPoint->des_para.RODNODE.norm_offset,49,"%s",sThickStr);
		if(pDefViewDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
	}
#endif
	else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�1
	else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�2
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.norm_offset")==pItem->m_idProp)
	{	//����ƫ����
		CString sThickStr;
		sThickStr=pEndPoint->des_para.AXIS_INTERS.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
		Input("Input the offset string along normal",&sThickStr,'s');
#else 
		Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
		_snprintf(pEndPoint->des_para.AXIS_INTERS.norm_offset,49,"%s",sThickStr);
		if(pDefViewDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
	}
#endif
	else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.hRod")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);//��׼�Ǹ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.norm_offset")==pItem->m_idProp)
	{	//����ƫ����
		
		CString sThickStr;
		sThickStr=pEndPoint->des_para.RODEND.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
		Input("Input the offset string along normal",&sThickStr,'s');
#else 
		Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
		_snprintf(pEndPoint->des_para.RODEND.norm_offset,49,"%s",sThickStr);
		if(pDefViewDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
	}
#endif
	else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.hNode")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_NODE,pItem->m_idProp);	//��׼�ڵ�
	else if(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.hPart")==pItem->m_idProp)
		pDefViewDlg->SelectObject(CLS_PART,pItem->m_idProp);	//��׼����
	else if(CDefinedViewDlg::GetPropID("m_xLineEnd.datum_pos")==pItem->m_idProp)
	{
		CReportPointDlg pointDlg;
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "Relative Coordinates";
#else 
		pointDlg.m_sCaption = "�������";
#endif
		pointDlg.m_fPointX = pEndPoint->datum_pos.x;
		pointDlg.m_fPointY = pEndPoint->datum_pos.y;
		pointDlg.m_fPointZ = pEndPoint->datum_pos.z;
		if(pointDlg.DoModal()==IDOK)
		{
			CXhChar100 sText("%f",pointDlg.m_fPointX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CDefinedViewDlg::GetPropID("m_xLineEnd.datum_pos.x"),CString(sText));
			sText.Printf("%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CDefinedViewDlg::GetPropID("m_xLineEnd.datum_pos.y"),CString(sText));
			sText.Printf("%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CDefinedViewDlg::GetPropID("m_xLineEnd.datum_pos.z"),CString(sText));
			pEndPoint->datum_pos.Set(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ);
		}
	}
	else if(CDefinedViewDlg::GetPropID("m_xFaceNorm.hVicePart")==pItem->m_idProp
		||CDefinedViewDlg::GetPropID("m_xFaceNorm.hCrossPart")==pItem->m_idProp)
	{
		if(pVector->norm_style==1)
			pDefViewDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);
		else if(pVector->norm_style==2||pVector->norm_style==5)
			pDefViewDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);
		else 
			pDefViewDlg->SelectObject(CLS_PART,pItem->m_idProp);
	}
	else 
		return FALSE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CDefinedViewDlg dialog


CDefinedViewDlg::CDefinedViewDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDefinedViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDefinedViewDlg)
	m_fAxisX_X = 1.0;
	m_fAxisX_Y = 0.0;
	m_fAxisX_Z = 0.0;
	m_fAxisY_X = 0.0;
	m_fAxisY_Y = 1.0;
	m_fAxisY_Z = 0.0;
	m_fAxisZ_X = 0.0;
	m_fAxisZ_Y = 0.0;
	m_fAxisZ_Z = 1.0;
	m_fOriginPosX = 0.0;
	m_fOriginPosY = 0.0;
	m_fOriginPosZ = 0.0;
	m_bCanModifyAxisX = TRUE;
	m_bCanModifyAxisY = FALSE;
	m_bCanModifyAxisZ = TRUE;
	m_sViewName = _T("");
	m_sFolderName = _T("");
	m_sDefaultLayer = _T("");
	m_bSetDrawingAssistant = FALSE;
	m_iStyle = 1;
	//}}AFX_DATA_INIT
	ucs.axis_x.Set(1.0,0.0,0.0);
	ucs.axis_y.Set(0.0,1.0,0.0);
	ucs.axis_z.Set(0.0,0.0,1.0);
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("Part Handle");
	m_arrHeaderLabel.Add("Part Type");
	m_arrHeaderLabel.Add("Draw Mark");
	m_arrHeaderLabel.Add("Vest In Plane");
#else 
	m_arrHeaderLabel.Add("�������");
	m_arrHeaderLabel.Add("��������");
	m_arrHeaderLabel.Add("��ͼ���");
	m_arrHeaderLabel.Add("����ƽ��");
#endif
	listViewComparer.SetMaxColumnCount(4);
	m_pDefinedView=NULL;
	m_pDrawingAssistant=NULL;
	m_bLayerSegView=FALSE;
	m_iSegI=SEGI();
	//
#ifdef AFX_TARG_ENU_ENGLISH
	m_xDrawingAssistantList.AddColumnHeader("Name");
	m_xDrawingAssistantList.AddColumnHeader("Type");
#else 
	m_xDrawingAssistantList.AddColumnHeader("����");
	m_xDrawingAssistantList.AddColumnHeader("����");
#endif
}


void CDefinedViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefinedViewDlg)
	DDX_Control(pDX, IDC_LIST_DRAWING_ASSISTANT, m_xDrawingAssistantList);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Text(pDX, IDC_E_AXIS_X_X, m_fAxisX_X);
	DDX_Text(pDX, IDC_E_AXIS_X_Y, m_fAxisX_Y);
	DDX_Text(pDX, IDC_E_AXIS_X_Z, m_fAxisX_Z);
	DDX_Text(pDX, IDC_E_AXIS_Y_X, m_fAxisY_X);
	DDX_Text(pDX, IDC_E_AXIS_Y_Y, m_fAxisY_Y);
	DDX_Text(pDX, IDC_E_AXIS_Y_Z, m_fAxisY_Z);
	DDX_Text(pDX, IDC_E_AXIS_Z_X, m_fAxisZ_X);
	DDX_Text(pDX, IDC_E_AXIS_Z_Y, m_fAxisZ_Y);
	DDX_Text(pDX, IDC_E_AXIS_Z_Z, m_fAxisZ_Z);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_X, m_fOriginPosX);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_Y, m_fOriginPosY);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_Z, m_fOriginPosZ);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_X, m_bCanModifyAxisX);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_Y, m_bCanModifyAxisY);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_Z, m_bCanModifyAxisZ);
	DDX_Text(pDX, IDC_E_DEFINED_VIEW_NAME, m_sViewName);
	DDV_MaxChars(pDX, m_sViewName, 49);
	DDX_Text(pDX, IDC_E_FOLDER_NAME, m_sFolderName);
	DDV_MaxChars(pDX, m_sFolderName, 16);
	DDX_Text(pDX, IDC_E_DEFAULT_LAYER, m_sDefaultLayer);
	DDV_MaxChars(pDX, m_sDefaultLayer, 3);
	DDX_Check(pDX, IDC_CHK_SET_DRAWING_ASSISTANT, m_bSetDrawingAssistant);
	DDX_CBIndex(pDX, IDC_CMB_STYLE, m_iStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefinedViewDlg, CDialog)
	//{{AFX_MSG_MAP(CDefinedViewDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_X, OnBnPasteAxisX)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_Y, OnBnPasteAxisY)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_Z, OnBnPasteAxisZ)
	ON_BN_CLICKED(IDC_BN_PASTE_ORIGIN_POS, OnBnPasteOriginPos)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_X, OnBnCopyAxisX)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_Y, OnBnCopyAxisY)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_Z, OnBnCopyAxisZ)
	ON_BN_CLICKED(IDC_BN_COPY_ORIGIN_POS, OnBnCopyOriginPos)
	ON_BN_CLICKED(IDC_BN_CPY_UCS, OnBnCpyUcs)
	ON_BN_CLICKED(IDC_BN_PASTE_UCS, OnBnPasteUcs)
	ON_BN_CLICKED(IDC_BN_TO_RIGHT_HAND, OnBnToRightHand)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_X, OnChkEnableAxisX)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_Y, OnChkEnableAxisY)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_Z, OnChkEnableAxisZ)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PART, OnDblclkListPart)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_PART, OnKeydownListPart)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_PART, OnColumnclickListPart)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BN_DRAW_BY_SEL_PART_UCS, OnDrawBySelPartUcs)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PART, OnItemchangedListPart)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SPREAD_DRAW, OnSpreadDrawAllPlate)
	ON_COMMAND(ID_AUTO_JUDGE_DRAW, OnAutoJudgeDrawAllPlate)
	ON_BN_CLICKED(IDC_BN_REVERSE_AXIS_X, OnBnReverseAxisX)
	ON_BN_CLICKED(IDC_BN_REVERSE_AXIS_Y, OnBnReverseAxisY)
	ON_BN_CLICKED(IDC_BN_REVERSE_AXIS_Z, OnBnReverseAxisZ)
	ON_BN_CLICKED(IDC_BTN_ADD_DRAWING_ASSISTANT, OnAddDrawingAssistant)
	ON_BN_CLICKED(IDC_BTN_DELETE_DRAWING_ASSISTANT, OnDeleteDrawingAssistant)
	ON_BN_CLICKED(IDC_CHK_SET_DRAWING_ASSISTANT, OnSetDrawingAssistant)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DRAWING_ASSISTANT, OnItemchangedListDrawingAssistant)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DRAWING_ASSISTANT, OnEndlabeleditListDrawingAssistant)
	ON_CBN_SELCHANGE(IDC_CMB_SEG, OnSelchangeCmbSeg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefinedViewDlg message handlers

BOOL CDefinedViewDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CDrawingAssistant *pAssistant=NULL;
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	if(pPropList)	//����������,����ѡ�нǸֹ�񿴲���,������(�˴�������ʾ���������������)
		pPropList->Unlock();
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_PART);
	if(m_pListCtrl)
		m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
#ifdef __COMMON_PART_INC_
	if(m_bLayerSegView)
	{
		GetDlgItem(IDC_BN_DRAW_BY_SEL_PART_UCS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_S_CUR_SEG_I)->ShowWindow(SW_SHOW);
		CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_SEG);
		pCMB->ShowWindow(SW_SHOW);
		ATOM_LIST<SEGI> segNoList;
		Ta.GetSegNoList(segNoList);
		pCMB->ResetContent();
		CXhChar16 sItem,sSelItem;
		for(int i=0;i<segNoList.GetNodeNum();i++)
		{
			SEGI* iSeg=segNoList.GetByIndex(i);
			sItem=iSeg->ToString();
			int iInser=pCMB->AddString(sItem);
			pCMB->SetItemData(iInser,*iSeg);
			if(i==0&&m_iSegI.iSeg<=0)
			{
				pCMB->SetCurSel(iInser);
				sSelItem.Copy(sItem);
				m_iSegI=*segNoList.GetByIndex(0);
			}
			else if(m_iSegI.iSeg==iSeg->iSeg)	//��ǰ������ʾ�˶κ�
			{
				pCMB->SetCurSel(iInser);
				sSelItem.Copy(sItem);
			}
		}
		pCMB->SetWindowText(sSelItem);
	}
	else
#endif
	{
		GetDlgItem(IDC_BN_DRAW_BY_SEL_PART_UCS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_S_CUR_SEG_I)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CMB_SEG)->ShowWindow(SW_HIDE);
	}
	if(m_pDefinedView)
		UpdateViewPartList();

	//�趨������ͼ������
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.SetDividerScale(0.5);
	if(!m_bInernalStart)	
		InitPropHashtable();
	//���ûص�����
	m_propList.SetModifyValueFunc(ModifyDrawAssistPropValue);
	m_propList.SetButtonClickFunc(ButtonClickDrawAssistProp);
	m_propList.SetModifyStatusFunc(ModifyDrawingAssistItemStatus);
	long col_wide_arr[2]={60,80};
	m_xDrawingAssistantList.InitListCtrl(col_wide_arr);
	if(m_pDefinedView)
		m_pDefinedView->BackupTo(backupview);
	if(m_pDefinedView&&m_pDefinedView->listAssist.GetNodeNum()>0)
	{
		m_bSetDrawingAssistant=TRUE;
		CStringArray str_arr;
		str_arr.SetSize(2);
		m_xDrawingAssistantList.DeleteAllItems();
		int iSel=-1;
		m_bFireListItemChange=FALSE;	//�ص�ListCtrl��ItemChange�¼�����Ӧѡ��,��Ҫ��OninitDialog����ʱ������ΪTRUE
		for(pAssistant=m_pDefinedView->listAssist.GetFirst();pAssistant;pAssistant=m_pDefinedView->listAssist.GetNext())
		{
			if(m_bLayerSegView&&pAssistant->m_iSeg.iSeg!=m_iSegI.iSeg)
				continue;	//����ͼ����ͼ��ǰ�ֶ��ڵĸ���ͼԪ
			str_arr[0]=pAssistant->name;
#ifdef AFX_TARG_ENU_ENGLISH
			if(pAssistant->m_nStyle==0)
				str_arr[1]="Spread Plane";
			else if(pAssistant->m_nStyle==1)
				str_arr[1]="Ancillary Center Line";
			else if(pAssistant->m_nStyle==2)
				str_arr[1]="Ancillary Bending Line";
#else 
			if(pAssistant->m_nStyle==0)
				str_arr[1]="չ����";
			else if(pAssistant->m_nStyle==1)
				str_arr[1]="��������";
			else if(pAssistant->m_nStyle==2)
				str_arr[1]="����������";
#endif
			int iItem=m_xDrawingAssistantList.InsertItemRecord(-1,str_arr);
			m_xDrawingAssistantList.SetItemData(iItem,(DWORD)pAssistant);
			pAssistant->m_xLineStart.UpdatePos(console.GetActiveModel());
			if(pAssistant->m_nStyle==0)
				pAssistant->m_xFaceNorm.UpdateVector(console.GetActiveModel());
			else
				pAssistant->m_xLineEnd.UpdatePos(console.GetActiveModel());
			if(m_pDrawingAssistant==pAssistant)
				iSel=iItem;
		}
		if(m_xDrawingAssistantList.GetItemCount()>0&&iSel<0)
			iSel=0;
		m_xDrawingAssistantList.SetItemState(iSel,LVNI_SELECTED,LVNI_SELECTED);
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_CHK_SET_DRAWING_ASSISTANT)->SetWindowText("Drawing Ancillary<=");
#else 
		GetDlgItem(IDC_CHK_SET_DRAWING_ASSISTANT)->SetWindowText("��ͼ����<=");
#endif
		m_bFireListItemChange=TRUE;	//���´�ListCtrl�ж�ItemChange�¼�����Ӧ
	}
	else 
	{
		m_bSetDrawingAssistant=FALSE;
		ResetDlgSize();	
	}
	//�ڲ�����
	if(m_bInernalStart)	
		FinishSelectObjOper();	//���ѡ�����ĺ�������
	DisplayDrawingAssistProperty(m_pDrawingAssistant);	//�������FinishSelectObjOper֮��,��Ϊ�ú����Ĳ���������ܻ�Ӱ����ʾ�б�
	if(m_bInernalStart)	
	{
		//�ָ�ѡ����
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		m_propList.SetFocus();	//��������ý���
		if(pItem)
		{
			m_propList.SetCurSel(pItem->m_iIndex);	//����ѡ����
			char sText[100]="";
			if(GetPropValueStr(pItem->m_idProp,sText))
				m_propList.SetItemPropValue(pItem->m_idProp,sText);
		}
		Invalidate(FALSE);
	}
	else
	{
		m_bCanModifyAxisX=FALSE;
		m_bCanModifyAxisY=!m_bCanModifyAxisX;
		m_bCanModifyAxisZ=TRUE;
		UpdateUcs(FALSE);
	}

	GetDlgItem(IDC_E_DEFINED_VIEW_NAME)->EnableWindow(!m_bLayerSegView);
	GetDlgItem(IDC_E_FOLDER_NAME)->EnableWindow(!m_bLayerSegView);
	GetDlgItem(IDC_E_DEFAULT_LAYER)->EnableWindow(!m_bLayerSegView);
	GetDlgItem(IDC_BN_TO_RIGHT_HAND)->EnableWindow(!m_bLayerSegView);
	GetDlgItem(IDC_BN_PASTE_UCS)->EnableWindow(!m_bLayerSegView);
	GetDlgItem(IDC_BN_PASTE_ORIGIN_POS)->EnableWindow(!m_bLayerSegView);
	GetDlgItem(IDC_BN_REVERSE_AXIS_X)->EnableWindow(!m_bLayerSegView&&m_bCanModifyAxisX);
	GetDlgItem(IDC_BN_REVERSE_AXIS_Y)->EnableWindow(!m_bLayerSegView&&m_bCanModifyAxisY);
	GetDlgItem(IDC_BN_REVERSE_AXIS_Z)->EnableWindow(!m_bLayerSegView&&m_bCanModifyAxisZ);
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(!m_bLayerSegView&&m_bCanModifyAxisX);
	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(!m_bLayerSegView&&m_bCanModifyAxisY);
	GetDlgItem(IDC_BN_PASTE_AXIS_Z)->EnableWindow(!m_bLayerSegView&&m_bCanModifyAxisZ);
	GetDlgItem(IDC_CHK_ENABLE_AXIS_X)->EnableWindow(!m_bLayerSegView);
	GetDlgItem(IDC_CHK_ENABLE_AXIS_Y)->EnableWindow(!m_bLayerSegView);
	GetDlgItem(IDC_CHK_ENABLE_AXIS_Z)->EnableWindow(!m_bLayerSegView);
	GetDlgItem(IDC_E_ORIGIN_POS_X)->EnableWindow(!m_bLayerSegView);
	GetDlgItem(IDC_E_ORIGIN_POS_Y)->EnableWindow(!m_bLayerSegView);
	GetDlgItem(IDC_E_ORIGIN_POS_Z)->EnableWindow(!m_bLayerSegView);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(!m_bLayerSegView&&m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(!m_bLayerSegView&&m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(!m_bLayerSegView&&m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(!m_bLayerSegView&&m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(!m_bLayerSegView&&m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(!m_bLayerSegView&&m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Z_X)->EnableWindow(!m_bLayerSegView&&m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Y)->EnableWindow(!m_bLayerSegView&&m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Z)->EnableWindow(!m_bLayerSegView&&m_bCanModifyAxisZ);
	UpdateData(FALSE);
	return TRUE;
}

void CDefinedViewDlg::UpdateViewPartList()
{
	m_pListCtrl->DeleteAllItems();
	for(CViewPart *pItem=m_pDefinedView->GetFirstViewPart();pItem;pItem=m_pDefinedView->GetNextViewPart())
	{
		if(pItem->idClassType==CLS_NODE)
			continue;
		CLDSPart *pPart = pItem->GetPart();
		if(m_bLayerSegView&&pPart->iSeg.iSeg!=m_iSegI.iSeg)
			continue;	//���ǵ�ǰͼ����ͼ�ֶ��ڵĹ���
		CString tempStr;
		CStringArray str_arr;
		str_arr.SetSize(0,6);
		CXhChar50 sText;
		if(pPart)
		{
			tempStr.Format("0X%X",pItem->handle);
			str_arr.Add(tempStr);
			if(pPart->IsLinePart())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(pPart->GetClassTypeId()==CLS_LINEANGLE)
					str_arr.Add("Angle");
				else if(pPart->GetClassTypeId()==CLS_LINETUBE)
					str_arr.Add("Tube");
				else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
					str_arr.Add("Flat Steel");
				else if(pPart->GetClassTypeId()==CLS_LINESLOT)
					str_arr.Add("U-steel");
				else
					str_arr.Add("Rod");
#else 
				if(pPart->GetClassTypeId()==CLS_LINEANGLE)
					str_arr.Add("�Ǹ�");
				else if(pPart->GetClassTypeId()==CLS_LINETUBE)
					str_arr.Add("�ֹ�");
				else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
					str_arr.Add("���");
				else if(pPart->GetClassTypeId()==CLS_LINESLOT)
					str_arr.Add("�۸�");
				else
					str_arr.Add("�˼�");
#endif
			}
#ifndef __TSA_
			else if(pPart->GetClassTypeId()==CLS_PLATE)
			{
				CLDSPlate *pPlate=(CLDSPlate*)pPart;
#ifdef AFX_TARG_ENU_ENGLISH
				if(pPlate->face_N==1)
					str_arr.Add("Planar Plate");
				else if(pPlate->face_N==2)
					str_arr.Add("2 Planar Plate");
				else if(pPlate->jdb_style==0)
					str_arr.Add("3 Planar Plate"); 
				else
					str_arr.Add("Folded Plate");
#else 
				if(pPlate->face_N==1)
					str_arr.Add("�����");
				else if(pPlate->face_N==2)
					str_arr.Add("˫���");
				else if(pPlate->jdb_style==0)
					str_arr.Add("�����"); 
				else
					str_arr.Add("�۵���");
#endif
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				str_arr.Add("Parameter Plate");
			else if(pPart->GetClassTypeId()==CLS_BOLT)
				str_arr.Add("Bolt");
#else 
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				str_arr.Add("������");
			else if(pPart->GetClassTypeId()==CLS_BOLT)
				str_arr.Add("��˨");
#endif

#endif
			else 
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr.Add("Other Part");
#else 
				str_arr.Add("��������");
#endif
			sText.Empty();
			int iDrawwIndex=0;
			pItem->GetCurDrawFlagStr(sText,iDrawwIndex);
			str_arr.Add(CString(sText));
		}
		else if(pItem->handle==0)
			continue;
		else
		{
			tempStr.Format("0X%X",pItem->handle);
			str_arr.Add(tempStr);
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr.Add("Unknown");
			str_arr.Add("Unknown");
#else 
			str_arr.Add("δ֪");
			str_arr.Add("δ֪");
#endif
		}
#ifdef AFX_TARG_ENU_ENGLISH
		sText.Copy("Datum Plane");
#else 
		sText.Copy("����");
#endif
		for(CDrawingAssistant* pAssistant=m_pDefinedView->listAssist.GetFirst();pAssistant;pAssistant=m_pDefinedView->listAssist.GetNext())
		{
			if(m_bLayerSegView&&pAssistant->m_iSeg.iSeg!=m_iSegI.iSeg)
				continue;	//����ͼ����ͼ��ǰ�ֶ��ڵĸ���ͼԪ
			if(pAssistant->m_nStyle==0&&pAssistant->iNo==pItem->iResidePlaneNo)
			{
				sText.Copy(pAssistant->name);
				break;
			}
		}
		str_arr.Add(sText);
		int iCur=AppendRecord(-1,str_arr);
		m_pListCtrl->SetItemData(iCur,(DWORD)pItem);
	}
}

void CDefinedViewDlg::OnSelchangeCmbSeg() 
{
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_SEG);
	int iSel=pCMB->GetCurSel();;
	if(iSel>=0)
		m_iSegI=SEGI(pCMB->GetItemData(iSel));
	else
		m_iSegI=-1;
	if(m_pDefinedView&&m_pDefinedView->m_iViewType==0)
		UpdateViewPartList();
	if(m_pDefinedView&&m_pDefinedView->listAssist.GetNodeNum()>0)
	{
		m_bSetDrawingAssistant=TRUE;
		CStringArray str_arr;
		str_arr.SetSize(2);
		m_xDrawingAssistantList.DeleteAllItems();
		iSel=-1;
		m_bFireListItemChange=FALSE;	//�ص�ListCtrl��ItemChange�¼�����Ӧѡ��,��Ҫ��OninitDialog����ʱ������ΪTRUE
		for(CDrawingAssistant* pAssistant=m_pDefinedView->listAssist.GetFirst();pAssistant;pAssistant=m_pDefinedView->listAssist.GetNext())
		{
			if(m_bLayerSegView&&pAssistant->m_iSeg.iSeg!=m_iSegI.iSeg)
				continue;	//����ͼ����ͼ��ǰ�ֶ��ڵĸ���ͼԪ
			str_arr[0]=pAssistant->name;
#ifdef AFX_TARG_ENU_ENGLISH
			if(pAssistant->m_nStyle==0)
				str_arr[1]="Developed Plane";
			else if(pAssistant->m_nStyle==1)
				str_arr[1]="Ancillary Center Line";
			else if(pAssistant->m_nStyle==2)
				str_arr[1]="Ancillary Bending Line";
#else 
			if(pAssistant->m_nStyle==0)
				str_arr[1]="չ����";
			else if(pAssistant->m_nStyle==1)
				str_arr[1]="��������";
			else if(pAssistant->m_nStyle==2)
				str_arr[1]="����������";
#endif
			int iItem=m_xDrawingAssistantList.InsertItemRecord(-1,str_arr);
			m_xDrawingAssistantList.SetItemData(iItem,(DWORD)pAssistant);
			pAssistant->m_xLineStart.UpdatePos(console.GetActiveModel());
			if(pAssistant->m_nStyle==0)
				pAssistant->m_xFaceNorm.UpdateVector(console.GetActiveModel());
			else
				pAssistant->m_xLineEnd.UpdatePos(console.GetActiveModel());
			if(m_pDrawingAssistant==pAssistant)
				iSel=iItem;
		}
		if(m_xDrawingAssistantList.GetItemCount()>0&&iSel<0)
			iSel=0;
		m_xDrawingAssistantList.SetItemState(iSel,LVNI_SELECTED,LVNI_SELECTED);
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_CHK_SET_DRAWING_ASSISTANT)->SetWindowText("Drawing Ancillary<=");
#else 
		GetDlgItem(IDC_CHK_SET_DRAWING_ASSISTANT)->SetWindowText("��ͼ����<=");
#endif
		m_bFireListItemChange=TRUE;	//���´�ListCtrl�ж�ItemChange�¼�����Ӧ
	}
}

void CDefinedViewDlg::OnBnPasteAxisX() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisX_X = point.x;
		m_fAxisX_Y = point.y;
		m_fAxisX_Z = point.z;
		ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
		UpdateData(FALSE);
	}
}

void CDefinedViewDlg::OnBnPasteAxisY() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisY_X = point.x;
		m_fAxisY_Y = point.y;
		m_fAxisY_Z = point.z;
		ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
		UpdateData(FALSE);
	}
}

void CDefinedViewDlg::OnBnPasteAxisZ() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisZ_X = point.x;
		m_fAxisZ_Y = point.y;
		m_fAxisZ_Z = point.z;
		ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
		UpdateData(FALSE);
	}
}

void CDefinedViewDlg::OnBnPasteOriginPos() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fOriginPosX = point.x;
		m_fOriginPosY = point.y;
		m_fOriginPosZ = point.z;
		ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
		UpdateData(FALSE);
	}
}

void CDefinedViewDlg::UpdateUcs(BOOL bFlag)
{
	if(bFlag)	//����������ucs����
	{
		UpdateData(TRUE);
		ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
		ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
		ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
		ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	}
	else	//ucs�򽻻��������
	{
		m_fAxisX_X = ucs.axis_x.x;
		m_fAxisX_Y = ucs.axis_x.y;
		m_fAxisX_Z = ucs.axis_x.z;

		m_fAxisY_X = ucs.axis_y.x;
		m_fAxisY_Y = ucs.axis_y.y;
		m_fAxisY_Z = ucs.axis_y.z;

		m_fAxisZ_X = ucs.axis_z.x;
		m_fAxisZ_Y = ucs.axis_z.y;
		m_fAxisZ_Z = ucs.axis_z.z;

		m_fOriginPosX = ucs.origin.x;
		m_fOriginPosY = ucs.origin.y;
		m_fOriginPosZ = ucs.origin.z;
		UpdateData(FALSE);
	}
}

void CDefinedViewDlg::OnBnCopyAxisX() 
{
	f3dPoint point;
	point.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
	WritePointToClip(point);
}

void CDefinedViewDlg::OnBnCopyAxisY() 
{
	f3dPoint point;
	point.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
	WritePointToClip(point);
}

void CDefinedViewDlg::OnBnCopyAxisZ() 
{
	f3dPoint point;
	point.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
	WritePointToClip(point);
}

void CDefinedViewDlg::OnBnCopyOriginPos() 
{
	f3dPoint point;
	point.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	WritePointToClip(point);
}

void CDefinedViewDlg::OnBnCpyUcs() 
{
	UCS_STRU ucs;
	UpdateData();
	ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
	ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
	ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
	WriteUCSToClip(ucs);
	
}

void CDefinedViewDlg::OnBnPasteUcs() 
{
	UCS_STRU;
	if(ReadClipUCS(ucs))
	{
		m_fAxisX_X = ucs.axis_x.x;
		m_fAxisX_Y = ucs.axis_x.y;
		m_fAxisX_Z = ucs.axis_x.z;

		m_fAxisY_X = ucs.axis_y.x;
		m_fAxisY_Y = ucs.axis_y.y;
		m_fAxisY_Z = ucs.axis_y.z;

		m_fAxisZ_X = ucs.axis_z.x;
		m_fAxisZ_Y = ucs.axis_z.y;
		m_fAxisZ_Z = ucs.axis_z.z;

		m_fOriginPosX = ucs.origin.x;
		m_fOriginPosY = ucs.origin.y;
		m_fOriginPosZ = ucs.origin.z;
		UpdateData(FALSE);
	}
}

void CDefinedViewDlg::OnBnToRightHand() 
{
	UpdateUcs();
	if(!m_bCanModifyAxisZ)
	{	//��X-Y��Ϊ��׼	 X�᷽�����û����뷽�򱣳�һ�£���XY�����Z�᷽��Ȼ�����¼���Y�᷽��
		ucs.axis_z = ucs.axis_x^ucs.axis_y;
		normalize(ucs.axis_z);
		normalize(ucs.axis_x);
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
	}
	else if(!m_bCanModifyAxisY)
	{	//��Z-X��Ϊ��׼	 Z�᷽�����û����뷽�򱣳�һ��
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_y);
		normalize(ucs.axis_x);
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
	}
	else if(!m_bCanModifyAxisX)
	{	//��Y-Z��Ϊ��׼	Z�᷽�����û����뷽�򱣳�һ��
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
		normalize(ucs.axis_x);
		normalize(ucs.axis_z);
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
	}
	UpdateUcs(FALSE);
}

void CDefinedViewDlg::OnChkEnableAxisX() 
{
	UpdateData();
	if(!m_bCanModifyAxisX)
		m_bCanModifyAxisY=m_bCanModifyAxisZ=TRUE;
	else
	{
		m_bCanModifyAxisY = FALSE;
		m_bCanModifyAxisZ = TRUE;
	}
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_BN_PASTE_AXIS_Z)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_X)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Y)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Z)->EnableWindow(m_bCanModifyAxisZ);
	UpdateData(FALSE);
	Invalidate(FALSE);	//�ػ� wht 10-08-24
}

void CDefinedViewDlg::OnChkEnableAxisY() 
{
	UpdateData();
	if(!m_bCanModifyAxisY)
		m_bCanModifyAxisX=m_bCanModifyAxisZ=TRUE;
	else
	{
		m_bCanModifyAxisX = FALSE;
		m_bCanModifyAxisZ = TRUE;
	}
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_BN_PASTE_AXIS_Z)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_X)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Y)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Z)->EnableWindow(m_bCanModifyAxisZ);
	UpdateData(FALSE);
	Invalidate(FALSE);	//�ػ� wht 10-08-24
}

void CDefinedViewDlg::OnChkEnableAxisZ() 
{
	UpdateData();
	if(!m_bCanModifyAxisZ)
		m_bCanModifyAxisX=m_bCanModifyAxisY=TRUE;
	else
	{
		m_bCanModifyAxisX = TRUE;
		m_bCanModifyAxisY = FALSE;
	}
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_BN_PASTE_AXIS_Z)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_X)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Y)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Z)->EnableWindow(m_bCanModifyAxisZ);
	UpdateData(FALSE);
	Invalidate(FALSE);	//�ػ� wht 10-08-24
}

void CDefinedViewDlg::RefreshListViewHeader()
{
	LV_COLUMN lvc;
	int i, NUM_COLUMNS, nColumnCount;
	CHeaderCtrl *pHeader = m_pListCtrl->GetHeaderCtrl();
	nColumnCount = pHeader->GetItemCount();

	// Delete all of the columns.
	for (i=0;i < nColumnCount;i++)
	{
	   m_pListCtrl->DeleteColumn(0);
	}
	if(m_arrHeaderLabel.GetSize()>0)
		NUM_COLUMNS = m_arrHeaderLabel.GetSize();
	else
		NUM_COLUMNS = 0;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(i = 0; i<NUM_COLUMNS; i++)
	{
		lvc.iSubItem = i;
		lvc.pszText = m_arrHeaderLabel[i].GetBuffer(1);//_gszColumnLabel[i];
		if(i==3)
			lvc.cx = 85;
		else
			lvc.cx = 70;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}
int CDefinedViewDlg::AppendRecord(int iItem, CStringArray &str_arr)
{
	//insert items
	LV_ITEM lvi;
	if(iItem==-1)
		iItem=m_pListCtrl->GetItemCount();

	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = str_arr[0].GetBuffer(1);//_gszItem[i][0];
	iItem = m_pListCtrl->InsertItem(&lvi);
	//set item text for additional columns
	int nCount = m_arrHeaderLabel.GetSize();
	nCount = __min(nCount,str_arr.GetSize());
	for(int j = 1; j<nCount; j++)
	{
		str_arr[j].TrimLeft();
		str_arr[j].TrimRight();
		m_pListCtrl->SetItemText(iItem,j,str_arr[j]);
	}
	m_pListCtrl->RedrawItems(iItem,iItem);
	return iItem;
}

void CDefinedViewDlg::ModifyItem(int iCurSel)
{
	CPartDrawMarkDlg markDlg;
	markDlg.m_pViewPart=(CViewPart*)m_pListCtrl->GetItemData(iCurSel);
	if(markDlg.m_pViewPart==NULL)
		return;
	markDlg.m_pRelaView=m_pDefinedView;
	if(markDlg.DoModal()==IDOK)
	{
		int iDrawIndex=0;
		char sText[50]="";
		markDlg.m_pViewPart->GetCurDrawFlagStr(sText,iDrawIndex);
		m_pListCtrl->SetItemText(iCurSel,2,sText);
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(sText,"Datum Plane");
#else 
		strcpy(sText,"����");
#endif
		if(markDlg.m_pViewPart->iResidePlaneNo>0&&m_pDefinedView->m_iViewType==1)
		{
			for(CDrawingAssistant *pAssis=m_pDefinedView->listAssist.GetFirst();pAssis;pAssis=m_pDefinedView->listAssist.GetNext())
			{
				if(pAssis->m_nStyle==0&&pAssis->iNo==markDlg.m_pViewPart->iResidePlaneNo)
				{
					strcpy(sText,pAssis->name);
					break;
				}
			}
		}
		m_pListCtrl->SetItemText(iCurSel,3,sText);
	}
}

void CDefinedViewDlg::DeleteItem(int nCurSel) 
{
	m_pListCtrl->DeleteItem(nCurSel);
}

void CDefinedViewDlg::SelectPart(int nCurSel) 
{
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CViewPart *pViewPart=(CViewPart*)m_pListCtrl->GetItemData(nCurSel);
	if(pViewPart==NULL)
		return;
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(pViewPart->handle);
	pLDSView->UpdatePropertyPage();
}

void CDefinedViewDlg::OnDblclkListPart(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
			ModifyItem(iCurSel);
	}
	*pResult = 0;
}

void CDefinedViewDlg::OnKeydownListPart(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	if(pLVKeyDow->wVKey==VK_F3)
	{
		POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
		if(pos!=NULL)
		{
			int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
			if(iCurSel>=0)
				ModifyItem(iCurSel);
		}
	}
	else if(pLVKeyDow->wVKey==VK_DELETE)
	{
		POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			int iCurSel=m_pListCtrl->GetNextSelectedItem(pos);
			if(iCurSel>=0)
				DeleteItem(iCurSel);
			pos = m_pListCtrl->GetFirstSelectedItemPosition();
		}
	}
	*pResult = 0;
}

void CDefinedViewDlg::OnOK() 
{
	UpdateData();
	BOOL bRepeatedName = FALSE;
	//�ù����б��еĹ���������ͼ��������
	if(m_pDefinedView)
	{
		CViewPart *pViewPart=NULL;
		for(pViewPart=m_pDefinedView->GetFirstViewPart();pViewPart;pViewPart=m_pDefinedView->GetNextViewPart())
		{
			if(pViewPart->idClassType==CLS_NODE)
				continue;
			else
				m_pDefinedView->DeletCursorViewPart();
		}
		ATOM_LIST<CViewPart> partset;
		int i=0,nCount=m_pListCtrl->GetItemCount();
		for(i=0;i<nCount;i++)
		{
			pViewPart=(CViewPart*)m_pListCtrl->GetItemData(i);
			if(pViewPart==NULL)
				continue;
			partset.append(*pViewPart);
		}
		m_pDefinedView->CleanPartSet();
		for(pViewPart=partset.GetFirst();pViewPart;pViewPart=partset.GetNext())
			m_pDefinedView->AppendViewPart(*pViewPart);
	}
	for(CDisplayView *pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
	{
		if(pView==m_pDefinedView)
			continue;
		if(m_pDefinedView&&pView->m_iViewType!=m_pDefinedView->m_iViewType)
			continue;	//��ͼ���Ͳ�һ��
		if((pView->m_iViewType==0||(pView->m_iViewType==1
			&&m_sFolderName.CompareNoCase(pView->folderName)==0))
			&&m_sViewName.CompareNoCase(pView->name)==0)
		{	//������ͼ��ͬ�������������� wht 10-12-28
			bRepeatedName=TRUE;
			break;
		}
	}
	UpdateUcs();
#ifdef AFX_TARG_ENU_ENGLISH
	if(ucs.axis_x.IsZero()||ucs.axis_y.IsZero()||ucs.axis_z.IsZero())
		AfxMessageBox("Coordinates System Illegal(Coordinate axis direction can't be 0)");
	else if(!is_ucs_valid(ucs))
		AfxMessageBox("Coordinates System Illegal(Not the right hand spiral orthogonal coordinate)");
	else if(bRepeatedName)
		AfxMessageBox("View Name Illegal(Existing same view name)");
#else 
	if(ucs.axis_x.IsZero()||ucs.axis_y.IsZero()||ucs.axis_z.IsZero())
		AfxMessageBox("����ϵ���Ϸ�(�����᷽����Ϊ��)");
	else if(!is_ucs_valid(ucs))
		AfxMessageBox("����ϵ���Ϸ�(����������������������ϵ)");
	else if(bRepeatedName)
		AfxMessageBox("��ͼ�����Ϸ�(�Ѵ�����ͬ����ͼ��)");
#endif
	else
		CDialog::OnOK();
}


void CDefinedViewDlg::OnColumnclickListPart(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	listViewComparer.m_pListCtrl = m_pListCtrl;
	listViewComparer.m_iColumn = pNMListView->iSubItem;
	listViewComparer.sortOrderAscending[pNMListView->iSubItem]=!listViewComparer.sortOrderAscending[pNMListView->iSubItem];
	m_pListCtrl->SortItems(CompareItemProc,(DWORD)&listViewComparer);
	*pResult = 0;
}

HBRUSH CDefinedViewDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if(pWnd->GetDlgCtrlID()==IDC_S_RED)
		pDC->SetTextColor(RGB(255,0,0));
	else if(pWnd->GetDlgCtrlID()==IDC_S_BLUE)
		pDC->SetTextColor(RGB(0,130,0));
	else if(!m_bCanModifyAxisZ)
	{	//��X-Y��Ϊ��׼	 X�᷽�����û����뷽�򱣳�һ�£���XY�����Z�᷽��Ȼ�����¼���Y�᷽��
		if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_X)
			pDC->SetTextColor(RGB(255,0,0));	//���ɱ�
		else if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Y)
			pDC->SetTextColor(RGB(0,130,0));	//�ɱ�
	}
	else if(!m_bCanModifyAxisY)
	{	//��Z-X��Ϊ��׼	 Z�᷽�����û����뷽�򱣳�һ��
		if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Z)
			pDC->SetTextColor(RGB(255,0,0));
		else if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_X)
			pDC->SetTextColor(RGB(0,130,0));
	}
	else if(!m_bCanModifyAxisX)
	{	//��Y-Z��Ϊ��׼	Y�᷽�����û����뷽�򱣳�һ��
		if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Y)
			pDC->SetTextColor(RGB(255,0,0));
		else if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Z)
			pDC->SetTextColor(RGB(0,130,0));
	}
	return hbr;
}

void CDefinedViewDlg::OnDrawBySelPartUcs() 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos==NULL)
		return;
	char tem_str[100]="", sText[100]="";
	int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
	m_pListCtrl->GetItemText(iCurSel,0,tem_str,100);
	m_pListCtrl->GetItemText(iCurSel,2,sText,100);
	long hPart=0;
	sscanf(tem_str,"%X",&hPart);
	CLDSPart *pPart=console.FromPartHandle(hPart);
	if(pPart)
	{
		ucs.origin=pPart->ucs.origin;
		if(sText[0]=='X')
		{
			ucs.axis_z=pPart->ucs.axis_x;
			ucs.axis_x=pPart->ucs.axis_z;
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_y);
			ucs.axis_x=ucs.axis_y^ucs.axis_z;
			normalize(ucs.axis_x);
		}
		else if(sText[0]=='Y')
		{
			ucs.axis_z=pPart->ucs.axis_y;
			ucs.axis_y=pPart->ucs.axis_z;
			ucs.axis_x=ucs.axis_y^ucs.axis_z;
			normalize(ucs.axis_x);
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_y);
		}
		else if(sText[0]=='Z')
			ucs=pPart->ucs;
		UpdateUcs(FALSE);
		//else	//�Զ��ж�
		UpdateData(FALSE);
	}
}

void CDefinedViewDlg::OnItemchangedListPart(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
			SelectPart(iCurSel);
	}
	*pResult = 0;
}

void CDefinedViewDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
#ifndef __TSA_
	CRect rectItem;
	m_pListCtrl->GetWindowRect(rectItem);
	if(!rectItem.PtInRect(point))
		return;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
#ifdef AFX_TARG_ENU_ENGLISH
	pMenu->AppendMenu(MF_STRING,ID_SPREAD_DRAW,"Spread Draw All Plate");
	pMenu->AppendMenu(MF_STRING,ID_AUTO_JUDGE_DRAW,"Judge Automatic All Plate");
#else 
	pMenu->AppendMenu(MF_STRING,ID_SPREAD_DRAW,"չ���������иְ�");
	pMenu->AppendMenu(MF_STRING,ID_AUTO_JUDGE_DRAW,"�Զ��ж����иְ�");
#endif
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this);
#endif
}

void CDefinedViewDlg::SetAllPlateDrawFlag(BOOL bSpreadDraw)
{	//�������иְ���Ʊ��
#ifndef __TSA_
	int i=0;
	for(i=0;i<m_pListCtrl->GetItemCount();i++)
	{
		CViewPart *pViewPart=(CViewPart*)m_pListCtrl->GetItemData(i);
		if(pViewPart==NULL)
			continue;
		CLDSPart *pPart=pViewPart->GetPart();
		if(pPart==NULL)
			continue;
		if(pPart->GetClassTypeId()!=CLS_PLATE)
			continue;
		CLDSPlate *pPlate=(CLDSPlate*)pPart;
		if(bSpreadDraw)
		{
			if(pPlate->face_N<2)
				continue;	//����岻��Ҫչ������
			pViewPart->cFlag='S';	//չ������
#ifdef AFX_TARG_ENU_ENGLISH
			m_pListCtrl->SetItemText(i,2,"Spread Draw");
#else 
			m_pListCtrl->SetItemText(i,2,"չ������");
#endif
		}
		else
		{
			pViewPart->cFlag=0;	//�Զ��ж�
#ifdef AFX_TARG_ENU_ENGLISH
			m_pListCtrl->SetItemText(i,2,"Judge Automatic");
#else 
			m_pListCtrl->SetItemText(i,2,"�Զ��ж�");
#endif
		}
	}
#endif
}
void CDefinedViewDlg::OnSpreadDrawAllPlate()
{
	SetAllPlateDrawFlag(TRUE);
}

void CDefinedViewDlg::OnAutoJudgeDrawAllPlate()
{
	SetAllPlateDrawFlag(FALSE);
}

void CDefinedViewDlg::OnBnReverseAxisX() 
{
	m_fAxisX_X *= -1;
	m_fAxisX_Y *= -1;
	m_fAxisX_Z *= -1;
	ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
	UpdateData(FALSE);
}

void CDefinedViewDlg::OnBnReverseAxisY() 
{
	m_fAxisY_X *= -1;
	m_fAxisY_Y *= -1;
	m_fAxisY_Z *= -1;
	ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
	UpdateData(FALSE);
}

void CDefinedViewDlg::OnBnReverseAxisZ() 
{
	m_fAxisZ_X *= -1;
	m_fAxisZ_Y *= -1;
	m_fAxisZ_Z *= -1;
	ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
	UpdateData(FALSE);
}

void CDefinedViewDlg::OnAddDrawingAssistant() 
{
	if(m_pDefinedView==NULL)
		return;
	UpdateData();
	CObjNoGroup NoGroup;
	NoGroup.InitMaxNo(15);
	CDrawingAssistant *pAssistant;
	for(pAssistant=m_pDefinedView->listAssist.GetFirst();pAssistant;pAssistant=m_pDefinedView->listAssist.GetNext())
	{
		if(m_bLayerSegView&&m_iSegI.iSeg!=pAssistant->m_iSeg.iSeg)
			continue;
		NoGroup.SetNoState(pAssistant->iNo);
	}
	int idle_no=NoGroup.EnumIdleNo();
	if(idle_no>10)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("System does not support more than 10 ancillary  drawing elements!");
#else 
		AfxMessageBox("ϵͳ��֧��10�����ϻ�ͼ������ͼԪ��!");
#endif
		return;
	}
	if(m_bLayerSegView&&m_iSegI.iSeg<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Current seg number is illegal.Unable to add current ancillary drawing elements  in the layer!");
#else 
		AfxMessageBox("��ǰ�κŲ��Ϸ�,�޷����ͼ����ͼ�ĵ�ǰ�θ�����ͼԪ��!");
#endif
		return;
	}
	pAssistant=m_pDefinedView->listAssist.append();
	pAssistant->iNo=idle_no;
	if(m_bLayerSegView)
		pAssistant->m_iSeg=m_iSegI;
	pAssistant->m_nStyle=m_iStyle;
#ifdef AFX_TARG_ENU_ENGLISH
	if(pAssistant->m_nStyle==0)
		sprintf(pAssistant->name,"Spread Plane %d",pAssistant->iNo);
	else if(pAssistant->m_nStyle==1)
		sprintf(pAssistant->name,"Center Line %d",pAssistant->iNo);
	else if(pAssistant->m_nStyle==2)
		sprintf(pAssistant->name,"Bending Line %d",pAssistant->iNo);
#else 
	if(pAssistant->m_nStyle==0)
		sprintf(pAssistant->name,"չ����%d",pAssistant->iNo);
	else if(pAssistant->m_nStyle==1)
		sprintf(pAssistant->name,"����%d",pAssistant->iNo);
	else if(pAssistant->m_nStyle==2)
		sprintf(pAssistant->name,"������%d",pAssistant->iNo);
#endif
	
	//
	CStringArray str_arr;
	str_arr.SetSize(2);
	str_arr[0].Format("%s",pAssistant->name);
#ifdef AFX_TARG_ENU_ENGLISH
	if(pAssistant->m_nStyle==0)
		str_arr[1]="Spread Plane ";
	else if(pAssistant->m_nStyle==1)
		str_arr[1]="Ancillary Center Line";
	else if(pAssistant->m_nStyle==2)
		str_arr[1]="Ancillary Bending Line";	
#else 
	if(pAssistant->m_nStyle==0)
		str_arr[1]="չ����";
	else if(pAssistant->m_nStyle==1)
		str_arr[1]="��������";
	else if(pAssistant->m_nStyle==2)
		str_arr[1]="����������";	
#endif
	int iItem=m_xDrawingAssistantList.InsertItemRecord(-1,str_arr);
	m_xDrawingAssistantList.SetItemData(iItem,(DWORD)pAssistant);
	m_xDrawingAssistantList.SetItemState(-1,0,LVIS_SELECTED|LVIS_FOCUSED);	//ȡ������ѡ����
	m_xDrawingAssistantList.SetItemState(iItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
	m_pDrawingAssistant=pAssistant;
}

void CDefinedViewDlg::OnDeleteDrawingAssistant() 
{
	if(m_pDefinedView==NULL)
		return;
	POSITION pos=m_xDrawingAssistantList.GetFirstSelectedItemPosition();
	int iCurSel=m_xDrawingAssistantList.GetNextSelectedItem(pos);
	if(iCurSel<0)
		return;
	CDrawingAssistant *pAssis=(CDrawingAssistant*)m_xDrawingAssistantList.GetItemData(iCurSel);
	m_xDrawingAssistantList.DeleteItem(iCurSel);
	m_pDrawingAssistant=NULL;
	for(CDrawingAssistant* pTempAssis=m_pDefinedView->listAssist.GetFirst();pTempAssis;pTempAssis=m_pDefinedView->listAssist.GetNext())
	{
		if(pTempAssis==pAssis)
		{
			m_pDefinedView->listAssist.DeleteCursor(TRUE);
			break;
		}
	}
	DisplayDrawingAssistProperty();
}

void CDefinedViewDlg::ResetDlgSize()
{
	CRect rc;
	GetWindowRect(rc);
	int offset_dist=15;
	if(m_bSetDrawingAssistant)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_CHK_SET_DRAWING_ASSISTANT)->SetWindowText("Drawing Ancillary<=");
#else 
		GetDlgItem(IDC_CHK_SET_DRAWING_ASSISTANT)->SetWindowText("��ͼ����<=");
#endif
		rc.right+=245;
		offset_dist=-15;
	}
	else 
	{
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_CHK_SET_DRAWING_ASSISTANT)->SetWindowText("Drawing Ancillary=>");
#else 
		GetDlgItem(IDC_CHK_SET_DRAWING_ASSISTANT)->SetWindowText("��ͼ����=>");
#endif
		rc.right-=245;
	}
	MoveWindow(rc);	
	//
	long id_arr[8]={IDC_CMB_STYLE,IDC_S_BORDERLINE,IDC_BTN_ADD_DRAWING_ASSISTANT,IDC_LIST_BOX,IDC_S_STLYE_NAME,
		IDC_BTN_DELETE_DRAWING_ASSISTANT,IDC_LIST_DRAWING_ASSISTANT,IDC_E_PROP_HELP_STR};
	for(int i=0;i<8;i++)
	{
		CWnd *pSonWnd = GetDlgItem(id_arr[i]);
		if(pSonWnd==NULL)
			continue;
		CRect rcSon;
		pSonWnd->GetWindowRect(rcSon);
		ScreenToClient(rcSon);
		rcSon.left+=offset_dist;
		rcSon.right+=offset_dist;
		pSonWnd->MoveWindow(rcSon);
	}
}

void CDefinedViewDlg::OnSetDrawingAssistant() 
{
	UpdateData();
	ResetDlgSize();
	UpdateData(FALSE);
}

const DWORD HASHTABLESIZE = 500;
CHashStrTable<long> CDefinedViewDlg::propHashtable;
CHashTable<DWORD> CDefinedViewDlg::propStatusHashtable;	//����״̬��ϣ��
void CDefinedViewDlg::InitPropHashtable()
{
	int id = 1;
	propHashtable.CreateHashTable(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(50);
	propHashtable.SetValueAt("residePlane",id++);
//����(չ��)��ʼ��
	propHashtable.SetValueAt("m_xLineStart",id++);
	propHashtable.SetValueAt("m_xLineStart.datum_pos_style",id++);
	propHashtable.SetValueAt("m_xLineStart.pos",id++);
	propHashtable.SetValueAt("m_xLineStart.x",id++);
	propHashtable.SetValueAt("m_xLineStart.y",id++);
	propHashtable.SetValueAt("m_xLineStart.z",id++);
	//��ڵ�ͬλ
	propHashtable.SetValueAt("m_xLineStart.des_para.hNode",id++);
	//ָ�������ϵ��������
	propHashtable.SetValueAt("m_xLineStart.des_para.hPart",id++);
	propHashtable.SetValueAt("m_xLineStart.datum_pos",id++);
	propHashtable.SetValueAt("m_xLineStart.datum_pos.x",id++);
	propHashtable.SetValueAt("m_xLineStart.datum_pos.y",id++);
	propHashtable.SetValueAt("m_xLineStart.datum_pos.z",id++);
	//���߽���
	propHashtable.SetValueAt("m_xLineStart.des_para.AXIS_INTERS.hDatum1",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.AXIS_INTERS.hDatum2",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style1",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style2",id++);//0.g;1.g1;2.g2;3.g3;4.�Զ���
	propHashtable.SetValueAt("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist1",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist2",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.AXIS_INTERS.near_vector",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.AXIS_INTERS.near_vector[0]",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.AXIS_INTERS.near_vector[1]",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.AXIS_INTERS.near_vector[2]",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.AXIS_INTERS.norm_offset",id++);	//����ƫ����;
	//�Ǹ����
	propHashtable.SetValueAt("m_xLineStart.des_para.RODEND.hRod",id++);				//�Ǹ־��
	propHashtable.SetValueAt("m_xLineStart.des_para.RODEND.direction",id++);			//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	propHashtable.SetValueAt("m_xLineStart.des_para.RODEND.offset_wing",id++);		//0.X֫;1.Y֫
	propHashtable.SetValueAt("m_xLineStart.des_para.RODEND.wing_offset_style",id++);	//0.g;1.g1;2.g2;3.g3;4.�Զ���
	propHashtable.SetValueAt("m_xLineStart.des_para.RODEND.wing_offset_dist",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.RODEND.len_offset_dist",id++);	//֫��ƫ��ֵ;����ƫ��ֵ;
	propHashtable.SetValueAt("m_xLineStart.des_para.RODEND.bIncOddEffect",id++);		//����ƫ��ֵ��������ͷӰ��
	propHashtable.SetValueAt("m_xLineStart.des_para.RODEND.norm_offset",id++);		//����ƫ����
	//�˼��Ͻڵ�Ϊ��׼
	propHashtable.SetValueAt("m_xLineStart.des_para.LINEPARTNODE.hLinePart",id++);	//�Ǹ־��
	propHashtable.SetValueAt("m_xLineStart.des_para.LINEPARTNODE.hNode",id++);		//��׼�ڵ���
	propHashtable.SetValueAt("m_xLineStart.des_para.LINEPARTNODE.bFaceOffset",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.LINEPARTNODE.direction",id++);	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	propHashtable.SetValueAt("m_xLineStart.des_para.LINEPARTNODE.offset_wing",id++);	//0.X֫;1.Y֫
	propHashtable.SetValueAt("m_xLineStart.des_para.LINEPARTNODE.wing_offset_style",id++);//0.g;1.g1;2.g2;3.g3;4.�Զ���
	propHashtable.SetValueAt("m_xLineStart.des_para.LINEPARTNODE.wing_offset_dist",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.LINEPARTNODE.len_offset_dist",id++);	//֫��ƫ��ֵ;����ƫ��ֵ;
	propHashtable.SetValueAt("m_xLineStart.des_para.LINEPARTNODE.norm_offset",id++);		//����ƫ����;
	propHashtable.SetValueAt("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]",id++);
	//��׼�����������겻���
	propHashtable.SetValueAt("m_xLineStart.des_para.DATUMLINE.hDatumLine",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2",id++);
	//��׼�����������겻���
	propHashtable.SetValueAt("m_xLineStart.des_para.DATUMPLANE.hDatumPlane",id++);
	propHashtable.SetValueAt("m_xLineStart.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2",id++);
	//���ڵ������ȷֵ�
	propHashtable.SetValueAt("m_xLineStart.des_para.SCALE_NODE.hLinePart",id++);			//��׼�˼����
	propHashtable.SetValueAt("m_xLineStart.des_para.SCALE_NODE.hStartNode",id++);		//ʼ�˽ڵ���
	propHashtable.SetValueAt("m_xLineStart.des_para.SCALE_NODE.hEndNode",id++);			//�ն˽ڵ���
	propHashtable.SetValueAt("m_xLineStart.des_para.SCALE_NODE.start_offset_dist",id++);	//ʼ��ƫ����
	propHashtable.SetValueAt("m_xLineStart.des_para.SCALE_NODE.end_offset_dist",id++);	//�ն�ƫ����
	propHashtable.SetValueAt("m_xLineStart.des_para.SCALE_NODE.offset_scale",id++);		//ƫ�Ʊ���
	propHashtable.SetValueAt("m_xLineStart.des_para.SCALE_NODE.offset_dist",id++);		//�ڵȷֻ����ϵ�ƫ��ֵ
//�������յ�
	propHashtable.SetValueAt("m_xLineEnd",id++);
	propHashtable.SetValueAt("m_xLineEnd.datum_pos_style",id++);
	propHashtable.SetValueAt("m_xLineEnd.pos",id++);
	propHashtable.SetValueAt("m_xLineEnd.x",id++);
	propHashtable.SetValueAt("m_xLineEnd.y",id++);
	propHashtable.SetValueAt("m_xLineEnd.z",id++);
	//��ڵ�ͬλ
	propHashtable.SetValueAt("m_xLineEnd.des_para.hNode",id++);
	//ָ�������ϵ��������
	propHashtable.SetValueAt("m_xLineEnd.des_para.hPart",id++);
	propHashtable.SetValueAt("m_xLineEnd.datum_pos",id++);
	propHashtable.SetValueAt("m_xLineEnd.datum_pos.x",id++);
	propHashtable.SetValueAt("m_xLineEnd.datum_pos.y",id++);
	propHashtable.SetValueAt("m_xLineEnd.datum_pos.z",id++);
	//���߽���
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.hDatum1",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.hDatum2",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_style1",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_style2",id++);//0.g;1.g1;2.g2;3.g3;4.�Զ���
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_dist1",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_dist2",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.near_vector",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.near_vector[0]",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.near_vector[1]",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.near_vector[2]",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.norm_offset",id++);	//����ƫ����;
	//�Ǹ����
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.hRod",id++);				//�Ǹ־��
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.direction",id++);			//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.offset_wing",id++);		//0.X֫;1.Y֫
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.wing_offset_style",id++);	//0.g;1.g1;2.g2;3.g3;4.�Զ���
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.wing_offset_dist",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.len_offset_dist",id++);	//֫��ƫ��ֵ;����ƫ��ֵ;
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.bIncOddEffect",id++);		//����ƫ��ֵ��������ͷӰ��
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.norm_offset",id++);		//����ƫ����
	//�˼��Ͻڵ�Ϊ��׼
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.hLinePart",id++);	//�Ǹ־��
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.hNode",id++);		//��׼�ڵ���
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.bFaceOffset",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.direction",id++);	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.offset_wing",id++);	//0.X֫;1.Y֫
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.wing_offset_style",id++);//0.g;1.g1;2.g2;3.g3;4.�Զ���
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.wing_offset_dist",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.len_offset_dist",id++);	//֫��ƫ��ֵ;����ƫ��ֵ;
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.norm_offset",id++);		//����ƫ����;
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[0]",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[1]",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[2]",id++);
	//��׼�����������겻���
	propHashtable.SetValueAt("m_xLineEnd.des_para.DATUMLINE.hDatumLine",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2",id++);
	//��׼�����������겻���
	propHashtable.SetValueAt("m_xLineEnd.des_para.DATUMPLANE.hDatumPlane",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2",id++);
	//���ڵ������ȷֵ�
	propHashtable.SetValueAt("m_xLineEnd.des_para.SCALE_NODE.hLinePart",id++);			//��׼�˼����
	propHashtable.SetValueAt("m_xLineEnd.des_para.SCALE_NODE.hStartNode",id++);		//ʼ�˽ڵ���
	propHashtable.SetValueAt("m_xLineEnd.des_para.SCALE_NODE.hEndNode",id++);			//�ն˽ڵ���
	propHashtable.SetValueAt("m_xLineEnd.des_para.SCALE_NODE.start_offset_dist",id++);	//ʼ��ƫ����
	propHashtable.SetValueAt("m_xLineEnd.des_para.SCALE_NODE.end_offset_dist",id++);	//�ն�ƫ����
	propHashtable.SetValueAt("m_xLineEnd.des_para.SCALE_NODE.offset_scale",id++);		//ƫ�Ʊ���
	propHashtable.SetValueAt("m_xLineEnd.des_para.SCALE_NODE.offset_dist",id++);		//�ڵȷֻ����ϵ�ƫ��ֵ
	propHashtable.SetValueAt("m_xLineEnd.datum_pos_style",id++);
	propHashtable.SetValueAt("m_xLineEnd",id++);
	propHashtable.SetValueAt("m_xLineEnd.x",id++);
	propHashtable.SetValueAt("m_xLineEnd.y",id++);
	propHashtable.SetValueAt("m_xLineEnd.z",id++);
	//��ڵ�ͬλ
	propHashtable.SetValueAt("m_xLineEnd.des_para.hNode",id++);
	//ָ�������ϵ��������
	propHashtable.SetValueAt("m_xLineEnd.des_para.hPart",id++);
	propHashtable.SetValueAt("m_xLineEnd.datum_pos",id++);
	propHashtable.SetValueAt("m_xLineEnd.datum_pos.x",id++);
	propHashtable.SetValueAt("m_xLineEnd.datum_pos.y",id++);
	propHashtable.SetValueAt("m_xLineEnd.datum_pos.z",id++);
	//���߽���
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.hDatum1",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.hDatum2",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_style1",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_style2",id++);//0.g;1.g1;2.g2;3.g3;4.�Զ���
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_dist1",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_dist2",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.near_vector",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.near_vector[0]",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.near_vector[1]",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.near_vector[2]",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.AXIS_INTERS.norm_offset",id++);	//����ƫ����;
	//�Ǹ����
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.hRod",id++);				//�Ǹ־��
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.direction",id++);			//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.offset_wing",id++);		//0.X֫;1.Y֫
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.wing_offset_style",id++);	//0.g;1.g1;2.g2;3.g3;4.�Զ���
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.wing_offset_dist",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.len_offset_dist",id++);	//֫��ƫ��ֵ;����ƫ��ֵ;
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.bIncOddEffect",id++);		//����ƫ��ֵ��������ͷӰ��
	propHashtable.SetValueAt("m_xLineEnd.des_para.RODEND.norm_offset",id++);		//����ƫ����
	//�˼��Ͻڵ�Ϊ��׼
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.hLinePart",id++);	//�Ǹ־��
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.hNode",id++);		//��׼�ڵ���
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.bFaceOffset",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.direction",id++);	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.offset_wing",id++);	//0.X֫;1.Y֫
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.wing_offset_style",id++);//0.g;1.g1;2.g2;3.g3;4.�Զ���
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.wing_offset_dist",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.len_offset_dist",id++);	//֫��ƫ��ֵ;����ƫ��ֵ;
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.norm_offset",id++);		//����ƫ����;
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[0]",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[1]",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[2]",id++);
	//��׼�����������겻���
	propHashtable.SetValueAt("m_xLineEnd.des_para.DATUMLINE.hDatumLine",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2",id++);
	//��׼�����������겻���
	propHashtable.SetValueAt("m_xLineEnd.des_para.DATUMPLANE.hDatumPlane",id++);
	propHashtable.SetValueAt("m_xLineEnd.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2",id++);
	//���ڵ������ȷֵ�
	propHashtable.SetValueAt("m_xLineEnd.des_para.SCALE_NODE.hLinePart",id++);			//��׼�˼����
	propHashtable.SetValueAt("m_xLineEnd.des_para.SCALE_NODE.hStartNode",id++);		//ʼ�˽ڵ���
	propHashtable.SetValueAt("m_xLineEnd.des_para.SCALE_NODE.hEndNode",id++);			//�ն˽ڵ���
	propHashtable.SetValueAt("m_xLineEnd.des_para.SCALE_NODE.start_offset_dist",id++);	//ʼ��ƫ����
	propHashtable.SetValueAt("m_xLineEnd.des_para.SCALE_NODE.end_offset_dist",id++);	//�ն�ƫ����
	propHashtable.SetValueAt("m_xLineEnd.des_para.SCALE_NODE.offset_scale",id++);		//ƫ�Ʊ���
	propHashtable.SetValueAt("m_xLineEnd.des_para.SCALE_NODE.offset_dist",id++);		//�ڵȷֻ����ϵ�ƫ��ֵ
	//չ���淨�߷���
	propHashtable.SetValueAt("m_xFaceNorm",id++);
	propHashtable.SetValueAt("m_xFaceNorm.norm",id++);
	propHashtable.SetValueAt("m_xFaceNorm.norm.x",id++);
	propHashtable.SetValueAt("m_xFaceNorm.norm.y",id++);
	propHashtable.SetValueAt("m_xFaceNorm.norm.z",id++);
	propHashtable.SetValueAt("m_xFaceNorm.norm_style",id++);
	propHashtable.SetValueAt("m_xFaceNorm.hVicePart",id++);
	propHashtable.SetValueAt("m_xFaceNorm.hCrossPart",id++);
	propHashtable.SetValueAt("m_xFaceNorm.norm_wing",id++);
	propHashtable.SetValueAt("m_xFaceNorm.direction",id++);
	propHashtable.SetValueAt("m_xFaceNorm.rot_ang",id++);
	propHashtable.SetValueAt("m_xFaceNorm.VectorType",id++);
	propHashtable.SetValueAt("m_xFaceNorm.nearVectorType",id++);
	propHashtable.SetValueAt("m_xFaceNorm.nearVector.x",id++);
	propHashtable.SetValueAt("m_xFaceNorm.nearVector.y",id++);
	propHashtable.SetValueAt("m_xFaceNorm.nearVector.z",id++);
}
long CDefinedViewDlg::GetPropID(char* propStr)
{
	long id;
	if(propHashtable.GetValueAt(propStr,id))
		return id;
	else
		return 0;
}
int CDefinedViewDlg::GetPropStatus(long id)					//�õ�����״̬
{
	DWORD dwStatus;
	if(CDefinedViewDlg::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}
BOOL CDefinedViewDlg::SetPropStatus(long id,BOOL bHide)		//��������״̬
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CDefinedViewDlg::propStatusHashtable.SetValueAt(id,dwStatus);
}

int CDefinedViewDlg::GetPropValueStr(long id, char *valueStr)
{
	CXhChar100 sText;
	if(m_pDrawingAssistant==NULL)
		return 0;
	CLDSPoint *pStartPoint=&m_pDrawingAssistant->m_xLineStart;
	CLDSPoint *pEndPoint=&m_pDrawingAssistant->m_xLineEnd;
	CLDSVector *pVector=&m_pDrawingAssistant->m_xFaceNorm;
	if(GetPropID("residePlane")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		sText.Copy("0.Datum Plane");
#else 
		sText.Copy("0.����");
#endif
		if(m_pDefinedView&&m_pDrawingAssistant->m_nStyle==1)
		{
			for(CDrawingAssistant *pAssis=m_pDefinedView->listAssist.GetFirst();pAssis;pAssis=m_pDefinedView->listAssist.GetNext())
			{
				if(pAssis->m_nStyle==0&&pAssis->iNo==m_pDrawingAssistant->iResidePlaneNo)
				{
					sText.Printf("%d.%s",pAssis->iNo,pAssis->name);
					break;
				}
			}
		}
	}
	else if(GetPropID("m_xLineStart.datum_pos_style")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pStartPoint->datum_pos_style==1)
			sText.Copy("1.Angle Ridge Vertex Locating Point");
		else if(pStartPoint->datum_pos_style==2)
			sText.Copy("2.Rod Node Locating Point");
		else if(pStartPoint->datum_pos_style==3)
			sText.Copy("3.Angle Center Line Intersection Point");
		else if(pStartPoint->datum_pos_style==4)
			sText.Copy("4.Baseline Space Intersection Point");
		else if(pStartPoint->datum_pos_style==5)
			sText.Copy("5.Specified Component Point Of Datum Line");
		else if(pStartPoint->datum_pos_style==6)
			sText.Copy("6.Specified Component Point Of Datum Plane");
		else if(pStartPoint->datum_pos_style==7)
			sText.Copy("7.Outer Angle's Ridge Vertex Along Angle's Normal Offset");
		else if(pStartPoint->datum_pos_style==8)
			sText.Copy("8.With The Node In The Same Position");
		else if(pStartPoint->datum_pos_style==9)
			sText.Copy("9.Datum Rod Relative Coordinate");
		else //if(pStartPoint->datum_pos_style==0)
			sText.Copy("0.Manual Input");
#else 
		if(pStartPoint->datum_pos_style==1)
			sText.Copy("1.�Ǹ���㶨λ��");
		else if(pStartPoint->datum_pos_style==2)
			sText.Copy("2.�˼��ڵ㶨λ��");
		else if(pStartPoint->datum_pos_style==3)
			sText.Copy("3.�Ǹ����߽���");
		else if(pStartPoint->datum_pos_style==4)
			sText.Copy("4.��׼�߿ռ佻��");
		else if(pStartPoint->datum_pos_style==5)
			sText.Copy("5.��׼����ָ�����������");
		else if(pStartPoint->datum_pos_style==6)
			sText.Copy("6.��׼����ָ�������");
		else if(pStartPoint->datum_pos_style==7)
			sText.Copy("7.�ؽǸ���֫����ƫ�Ƶİ������");
		else if(pStartPoint->datum_pos_style==8)
			sText.Copy("8.��ڵ�ͬλ");
		else if(pStartPoint->datum_pos_style==9)
			sText.Copy("9.��׼�����ϵ��������");
		else //if(pStartPoint->datum_pos_style==0)
			sText.Copy("0.�ֶ�����");
#endif
	}
	else if(GetPropID("m_xLineStart.x")==id)
	{
		sText.Printf("%f",pStartPoint->Position().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.y")==id)
	{
		sText.Printf("%f",pStartPoint->Position().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.z")==id)
	{
		sText.Printf("%f",pStartPoint->Position().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.hRod")==id)	//�Ǹ־��
		sText.Printf("0X%X",pStartPoint->des_para.RODEND.hRod);
	else if(GetPropID("m_xLineStart.des_para.RODEND.direction")==id)	
	{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
#ifdef AFX_TARG_ENU_ENGLISH
		if(pStartPoint->des_para.RODEND.direction==0)
			sText.Copy("0.Start->End");
		else 
			sText.Copy("1.End->Start");
#else 
		if(pStartPoint->des_para.RODEND.direction==0)
			sText.Copy("0.ʼ->��");
		else 
			sText.Copy("1.��->ʼ");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.offset_wing")==id)//0.X֫;1.Y֫
	{	//0.X֫;1.Y֫
#ifdef AFX_TARG_ENU_ENGLISH
		if(pStartPoint->des_para.RODEND.offset_wing==0)
			sText.Copy("0.X Leg");
		else 
			sText.Copy("1.Y Leg");
#else 
		if(pStartPoint->des_para.RODEND.offset_wing==0)
			sText.Copy("0.X֫");
		else 
			sText.Copy("1.Y֫");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.wing_offset_style")==id)	
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(pStartPoint->des_para.RODEND.wing_offset_style==0)
			sText.Copy("g");
		else if(pStartPoint->des_para.RODEND.wing_offset_style==1)
			sText.Copy("g1");
		else if(pStartPoint->des_para.RODEND.wing_offset_style==2)
			sText.Copy("g2");
		else if(pStartPoint->des_para.RODEND.wing_offset_style==3)
			sText.Copy("g3");
		else 
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("user specify");
#else 
			sText.Copy("�Զ���");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.wing_offset_dist")==id)
	{
		sText.Printf("%f",pStartPoint->des_para.RODEND.wing_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.len_offset_dist")==id)	
	{	//����ƫ��ֵ;
		sText.Printf("%f",pStartPoint->des_para.RODEND.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.bIncOddEffect")==id)		
	{	//����ƫ��ֵ��������ͷӰ��
#ifdef AFX_TARG_ENU_ENGLISH
		if(pStartPoint->des_para.RODEND.bIncOddEffect)
			sText.Copy("Yes");
		else 
			sText.Copy("No");
#else 
		if(pStartPoint->des_para.RODEND.bIncOddEffect)
			sText.Copy("��");
		else 
			sText.Copy("��");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.norm_offset")==id)//����ƫ����
	{
		CDesignThickPara thick(pStartPoint->des_para.RODEND.norm_offset);
		sText.Printf("%d",thick.Thick(console.GetActiveModel()));
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.hLinePart")==id)	//�Ǹ־��
		sText.Printf("0X%X",pStartPoint->des_para.RODNODE.hRod );
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.hNode")==id)	//��׼�ڵ���
		sText.Printf("0X%X",pStartPoint->des_para.RODNODE.hNode);
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.bFaceOffset")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pStartPoint->des_para.RODNODE.bFaceOffset)
			sText.Copy("Yes");
		else 
			sText.Copy("No");
#else 
		if(pStartPoint->des_para.RODNODE.bFaceOffset)
			sText.Copy("��");
		else 
			sText.Copy("��");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.direction")==id)	
	{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
#ifdef AFX_TARG_ENU_ENGLISH
		if(pStartPoint->des_para.RODNODE.direction==0)
			sText.Copy("0.Start->End");
		else 
			sText.Copy("1.End->Start");
#else 
		if(pStartPoint->des_para.RODNODE.direction==0)
			sText.Copy("0.ʼ->��");
		else 
			sText.Copy("1.��->ʼ");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.offset_wing")==id)	
	{	//0.X֫;1.Y֫
#ifdef AFX_TARG_ENU_ENGLISH
		if(pStartPoint->des_para.RODNODE.offset_wing==0)
			sText.Copy("0.X Leg");
		else 
			sText.Copy("1.Y Leg");
#else 
		if(pStartPoint->des_para.RODNODE.offset_wing==0)
			sText.Copy("0.X֫");
		else 
			sText.Copy("1.Y֫");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_style")==id)
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(pStartPoint->des_para.RODNODE.wing_offset_style==0)
			sText.Copy("g");
		else if(pStartPoint->des_para.RODNODE.wing_offset_style==1)
			sText.Copy("g1");
		else if(pStartPoint->des_para.RODNODE.wing_offset_style==2)
			sText.Copy("g2");
		else if(pStartPoint->des_para.RODNODE.wing_offset_style==3)
			sText.Copy("g3");
		else
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("user specify");
#else 
			sText.Copy("�Զ���");
#endif
	}	
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_dist")==id)
	{
		sText.Printf("%f",pStartPoint->des_para.RODNODE.wing_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.len_offset_dist")==id)	
	{	//����ƫ��ֵ
		sText.Printf("%f",pStartPoint->des_para.RODNODE.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.norm_offset")==id)	//����ƫ����
	{
		CDesignThickPara thick(pStartPoint->des_para.RODNODE.norm_offset);
		sText.Printf("%d",thick.Thick(console.GetActiveModel()));
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(pStartPoint->des_para.RODNODE.vPlaneNormal));
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]")==id)
	{
		sText.Printf("%f",pStartPoint->des_para.RODNODE.vPlaneNormal.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]")==id)
	{
		sText.Printf("%f",pStartPoint->des_para.RODNODE.vPlaneNormal.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]")==id)
	{
		sText.Printf("%f",pStartPoint->des_para.RODNODE.vPlaneNormal.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1")==id)
		sText.Printf("0X%X",pStartPoint->des_para.AXIS_INTERS.hDatum1);
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2")==id)
		sText.Printf("0X%X",pStartPoint->des_para.AXIS_INTERS.hDatum2);
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style1")==id)
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style1==0)
			sText.Copy("g");
		else if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style1==1)
			sText.Copy("g1");
		else if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style1==2)
			sText.Copy("g2");
		else if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style1==3)
			sText.Copy("g3");
		else
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("user specify");
#else 
			sText.Copy("�Զ���");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style2")==id)
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style2==0)
			sText.Copy("g");
		else if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style2==1)
			sText.Copy("g1");
		else if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style2==2)
			sText.Copy("g2");
		else if(pStartPoint->des_para.AXIS_INTERS.wing_offset_style2==3)
			sText.Copy("g3");
		else
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("user specify");
#else 
			sText.Copy("�Զ���");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist1")==id)
	{
		sText.Printf("%d",pStartPoint->des_para.AXIS_INTERS.wing_offset_dist1);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist2")==id)
	{
		sText.Printf("%d",pStartPoint->des_para.AXIS_INTERS.wing_offset_dist2);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(f3dPoint(pStartPoint->des_para.AXIS_INTERS.near_vector)));
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[0]")==id)
	{
		sText.Printf("%f",pStartPoint->des_para.AXIS_INTERS.near_vector[0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[1]")==id)
	{
		sText.Printf("%f",pStartPoint->des_para.AXIS_INTERS.near_vector[1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[2]")==id)
	{
		sText.Printf("%f",pStartPoint->des_para.AXIS_INTERS.near_vector[2]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.norm_offset")==id)	//����ƫ����;
	{
		CDesignThickPara thick(pStartPoint->des_para.AXIS_INTERS.norm_offset);
		sText.Printf("%d",thick.Thick(console.GetActiveModel()));
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1")==id)
		sText.Printf("0X%X",pStartPoint->des_para.AXIS_INTERS.hDatum1);
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2")==id)
		sText.Printf("0X%X",pStartPoint->des_para.AXIS_INTERS.hDatum2);
	else if(GetPropID("m_xLineStart.des_para.DATUMLINE.hDatumLine")==id)
		sText.Printf("0X%X",pStartPoint->des_para.DATUMLINE.hDatumLine);
	else if(GetPropID("m_xLineStart.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pStartPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
			sText.Printf("0.X axis");
		else if(pStartPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			sText.Printf("1.Y axis");
		else 
			sText.Printf("2.Z axis");
#else 
		if(pStartPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
			sText.Printf("0.X��");
		else if(pStartPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			sText.Printf("1.Y��");
		else 
			sText.Printf("2.Z��");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.DATUMPLANE.hDatumPlane")==id)
		sText.Printf("0X%X",pStartPoint->des_para.DATUMPLANE.hDatumPlane);
	else if(GetPropID("m_xLineStart.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pStartPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
			sText.Printf("0.X axis");
		else if(pStartPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
			sText.Printf("1.Y axis");
		else 
			sText.Printf("2.Z axis");
#else 
		if(pStartPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
			sText.Printf("0.X��");
		else if(pStartPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
			sText.Printf("1.Y��");
		else 
			sText.Printf("2.Z��");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.hRod")==id)	//�Ǹ־��
		sText.Printf("0X%X",pStartPoint->des_para.RODEND.hRod);
	else if(GetPropID("m_xLineStart.des_para.RODEND.direction")==id)	
	{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
#ifdef AFX_TARG_ENU_ENGLISH
		if(pStartPoint->des_para.RODEND.direction==0)
			sText.Copy("0.Start->End");
		else 
			sText.Copy("1.End->Start");
#else 
		if(pStartPoint->des_para.RODEND.direction==0)
			sText.Copy("0.ʼ->��");
		else 
			sText.Copy("1.��->ʼ");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.len_offset_dist")==id)	
	{	//����ƫ��ֵ;
		sText.Printf("%f",pStartPoint->des_para.RODEND.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.norm_offset")==id)//����ƫ����
	{
		CDesignThickPara thick(pStartPoint->des_para.RODEND.norm_offset);
		sText.Printf("%d",thick.Thick(console.GetActiveModel()));
	}
	else if(GetPropID("m_xLineStart.des_para.hNode")==id)
		sText.Printf("0X%X",pStartPoint->des_para.hNode);
	else if(GetPropID("des_para.hPart")==id)
		sText.Printf("0X%X",pStartPoint->des_para.hPart);
	else if(GetPropID("m_xLineStart.datum_pos.x")==id)
	{
		sText.Printf("%f",pStartPoint->datum_pos.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.datum_pos.y")==id)
	{
		sText.Printf("%f",pStartPoint->datum_pos.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.datum_pos.z")==id)
	{
		sText.Printf("%f",pStartPoint->datum_pos.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.hLinePart")==id)			//��׼�˼����
		sText.Printf("0X%0X",pStartPoint->des_para.SCALE_NODE.hLinePart);
	else if(GetPropID("des_para.SCALE_NODE.hStartNode")==id)		//ʼ�˽ڵ���
		sText.Printf("0X%0X",pStartPoint->des_para.SCALE_NODE.hStartNode);
	else if(GetPropID("des_para.SCALE_NODE.hEndNode")==id)			//�ն˽ڵ���
		sText.Printf("0X%0X",pStartPoint->des_para.SCALE_NODE.hEndNode);
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.start_offset_dist")==id)	//ʼ��ƫ����
	{
		sText.Printf("%f",pStartPoint->des_para.SCALE_NODE.start_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.end_offset_dist")==id)	//�ն�ƫ����
	{
		sText.Printf("%f",pStartPoint->des_para.SCALE_NODE.end_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.offset_scale")==id)		//ƫ�Ʊ���
	{
		sText.Printf("%f",pStartPoint->des_para.SCALE_NODE.offset_scale);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.offset_dist")==id)		//�ڵȷֻ����ϵ�ƫ��ֵ
	{
		sText.Printf("%f",pStartPoint->des_para.SCALE_NODE.offset_dist);
		SimplifiedNumString(sText);
	}
	else if(m_pDrawingAssistant->m_nStyle>0)
	{
		if(GetPropID("m_xLineEnd.datum_pos_style")==id)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(pEndPoint->datum_pos_style==1)
				sText.Copy("1.Angle Ridge Vertex Locating Point");
			else if(pEndPoint->datum_pos_style==2)
				sText.Copy("2.Rod Node Locating Point");
			else if(pEndPoint->datum_pos_style==3)
				sText.Copy("3.Angle Center Line Intersection Point");
			else if(pEndPoint->datum_pos_style==4)
				sText.Copy("4.Baseline Space Intersection Point");
			else if(pEndPoint->datum_pos_style==5)
				sText.Copy("5.Specified Component Point Of Datum Line");
			else if(pEndPoint->datum_pos_style==6)
				sText.Copy("6.Specified Component Point Of Datum Plane");
			else if(pEndPoint->datum_pos_style==7)
				sText.Copy("7.Outer Angle's Ridge Vertex Along Angle's Normal Offset");
			else if(pEndPoint->datum_pos_style==8)
				sText.Copy("8.With The Node In The Same Position");
			else if(pEndPoint->datum_pos_style==9)
				sText.Copy("9.Datum Rod Relative Coordinate");
			else //if(pEndPoint->datum_pos_style==0)
				sText.Copy("0.Manual Input");
#else 
			if(pEndPoint->datum_pos_style==1)
				sText.Copy("1.�Ǹ���㶨λ��");
			else if(pEndPoint->datum_pos_style==2)
				sText.Copy("2.�˼��ڵ㶨λ��");
			else if(pEndPoint->datum_pos_style==3)
				sText.Copy("3.�Ǹ����߽���");
			else if(pEndPoint->datum_pos_style==4)
				sText.Copy("4.��׼�߿ռ佻��");
			else if(pEndPoint->datum_pos_style==5)
				sText.Copy("5.��׼����ָ�����������");
			else if(pEndPoint->datum_pos_style==6)
				sText.Copy("6.��׼����ָ�������");
			else if(pEndPoint->datum_pos_style==7)
				sText.Copy("7.�ؽǸ���֫����ƫ�Ƶİ������");
			else if(pEndPoint->datum_pos_style==8)
				sText.Copy("8.��ڵ�ͬλ");
			else if(pEndPoint->datum_pos_style==9)
				sText.Copy("9.��׼�����ϵ��������");
			else //if(pEndPoint->datum_pos_style==0)
				sText.Copy("0.�ֶ�����");
#endif
		}
		else if(GetPropID("m_xLineEnd.x")==id)
		{
			sText.Printf("%f",pEndPoint->Position().x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.y")==id)
		{
			sText.Printf("%f",pEndPoint->Position().y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.z")==id)
		{
			sText.Printf("%f",pEndPoint->Position().z);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.RODEND.hRod")==id)	//�Ǹ־��
			sText.Printf("0X%X",pEndPoint->des_para.RODEND.hRod);
		else if(GetPropID("m_xLineEnd.des_para.RODEND.direction")==id)	
		{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
#ifdef AFX_TARG_ENU_ENGLISH
			if(pEndPoint->des_para.RODEND.direction==0)
				sText.Copy("0.Start->End");
			else 
				sText.Copy("1.End->Start");
#else 
			if(pEndPoint->des_para.RODEND.direction==0)
				sText.Copy("0.ʼ->��");
			else 
				sText.Copy("1.��->ʼ");
#endif
		}
		else if(GetPropID("m_xLineEnd.des_para.RODEND.offset_wing")==id)//0.X֫;1.Y֫
		{	//0.X֫;1.Y֫
#ifdef AFX_TARG_ENU_ENGLISH
			if(pEndPoint->des_para.RODEND.offset_wing==0)
				sText.Copy("0.X Leg");
			else 
				sText.Copy("1.Y Leg");
#else 
			if(pEndPoint->des_para.RODEND.offset_wing==0)
				sText.Copy("0.X֫");
			else 
				sText.Copy("1.Y֫");
#endif
		}
		else if(GetPropID("m_xLineEnd.des_para.RODEND.wing_offset_style")==id)	
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(pEndPoint->des_para.RODEND.wing_offset_style==0)
				sText.Copy("g");
			else if(pEndPoint->des_para.RODEND.wing_offset_style==1)
				sText.Copy("g1");
			else if(pEndPoint->des_para.RODEND.wing_offset_style==2)
				sText.Copy("g2");
			else if(pEndPoint->des_para.RODEND.wing_offset_style==3)
				sText.Copy("g3");
			else 
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("user specify");
#else 
				sText.Copy("�Զ���");
#endif
		}
		else if(GetPropID("m_xLineEnd.des_para.RODEND.wing_offset_dist")==id)
		{
			sText.Printf("%f",pEndPoint->des_para.RODEND.wing_offset_dist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.RODEND.len_offset_dist")==id)	
		{	//����ƫ��ֵ;
			sText.Printf("%f",pEndPoint->des_para.RODEND.len_offset_dist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.RODEND.bIncOddEffect")==id)		
		{	//����ƫ��ֵ��������ͷӰ��
#ifdef AFX_TARG_ENU_ENGLISH
			if(pEndPoint->des_para.RODEND.bIncOddEffect)
				sText.Copy("Yes");
			else 
				sText.Copy("No");
#else 
			if(pEndPoint->des_para.RODEND.bIncOddEffect)
				sText.Copy("��");
			else 
				sText.Copy("��");
#endif
		}
		else if(GetPropID("m_xLineEnd.des_para.RODEND.norm_offset")==id)//����ƫ����
		{
			CDesignThickPara thick(pEndPoint->des_para.RODEND.norm_offset);
			sText.Printf("%d",thick.Thick(console.GetActiveModel()));
		}
		else if(GetPropID("m_xLineEnd.des_para.LINEPARTNODE.hLinePart")==id)	//�Ǹ־��
			sText.Printf("0X%X",pEndPoint->des_para.RODNODE.hRod );
		else if(GetPropID("m_xLineEnd.des_para.LINEPARTNODE.hNode")==id)	//��׼�ڵ���
			sText.Printf("0X%X",pEndPoint->des_para.RODNODE.hNode);
		else if(GetPropID("m_xLineEnd.des_para.LINEPARTNODE.bFaceOffset")==id)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(pEndPoint->des_para.RODNODE.bFaceOffset)
				sText.Copy("Yes");
			else 
				sText.Copy("No");
#else 
			if(pEndPoint->des_para.RODNODE.bFaceOffset)
				sText.Copy("��");
			else 
				sText.Copy("��");
#endif
		}
		else if(GetPropID("m_xLineEnd.des_para.LINEPARTNODE.direction")==id)	
		{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
#ifdef AFX_TARG_ENU_ENGLISH
			if(pEndPoint->des_para.RODNODE.direction==0)
				sText.Copy("0.Start->End");
			else 
				sText.Copy("1.End->Start");
#else 
			if(pEndPoint->des_para.RODNODE.direction==0)
				sText.Copy("0.ʼ->��");
			else 
				sText.Copy("1.��->ʼ");
#endif
		}
		else if(GetPropID("m_xLineEnd.des_para.LINEPARTNODE.offset_wing")==id)	
		{	//0.X֫;1.Y֫
#ifdef AFX_TARG_ENU_ENGLISH
			if(pEndPoint->des_para.RODNODE.offset_wing==0)
				sText.Copy("0.X Leg");
			else 
				sText.Copy("1.Y Leg");
#else 
			if(pEndPoint->des_para.RODNODE.offset_wing==0)
				sText.Copy("0.X֫");
			else 
				sText.Copy("1.Y֫");
#endif
		}
		else if(GetPropID("m_xLineEnd.des_para.LINEPARTNODE.wing_offset_style")==id)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(pEndPoint->des_para.RODNODE.wing_offset_style==0)
				sText.Copy("g");
			else if(pEndPoint->des_para.RODNODE.wing_offset_style==1)
				sText.Copy("g1");
			else if(pEndPoint->des_para.RODNODE.wing_offset_style==2)
				sText.Copy("g2");
			else if(pEndPoint->des_para.RODNODE.wing_offset_style==3)
				sText.Copy("g3");
			else
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("user specify");
#else 
				sText.Copy("�Զ���");
#endif
		}	
		else if(GetPropID("m_xLineEnd.des_para.LINEPARTNODE.wing_offset_dist")==id)
		{
			sText.Printf("%f",pEndPoint->des_para.RODNODE.wing_offset_dist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.LINEPARTNODE.len_offset_dist")==id)	
		{	//����ƫ��ֵ
			sText.Printf("%f",pEndPoint->des_para.RODNODE.len_offset_dist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.LINEPARTNODE.norm_offset")==id)	//����ƫ����
		{
			CDesignThickPara thick(pEndPoint->des_para.RODNODE.norm_offset);
			sText.Printf("%d",thick.Thick(console.GetActiveModel()));
		}
		else if(GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset")==id)
			sText.Printf("%s",CStdVector::GetVecTypeString(pEndPoint->des_para.RODNODE.vPlaneNormal));
		else if(GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[0]")==id)
		{
			sText.Printf("%f",pEndPoint->des_para.RODNODE.vPlaneNormal.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[1]")==id)
		{
			sText.Printf("%f",pEndPoint->des_para.RODNODE.vPlaneNormal.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[2]")==id)
		{
			sText.Printf("%f",pEndPoint->des_para.RODNODE.vPlaneNormal.z);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum1")==id)
			sText.Printf("0X%X",pEndPoint->des_para.AXIS_INTERS.hDatum1);
		else if(GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum2")==id)
			sText.Printf("0X%X",pEndPoint->des_para.AXIS_INTERS.hDatum2);
		else if(GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_style1")==id)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style1==0)
				sText.Copy("g");
			else if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style1==1)
				sText.Copy("g1");
			else if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style1==2)
				sText.Copy("g2");
			else if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style1==3)
				sText.Copy("g3");
			else
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("user specify");
#else 
				sText.Copy("�Զ���");
#endif
		}
		else if(GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_style2")==id)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style2==0)
				sText.Copy("g");
			else if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style2==1)
				sText.Copy("g1");
			else if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style2==2)
				sText.Copy("g2");
			else if(pEndPoint->des_para.AXIS_INTERS.wing_offset_style2==3)
				sText.Copy("g3");
			else
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("user specify");
#else 
				sText.Copy("�Զ���");
#endif
		}
		else if(GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_dist1")==id)
		{
			sText.Printf("%d",pEndPoint->des_para.AXIS_INTERS.wing_offset_dist1);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_dist2")==id)
		{
			sText.Printf("%d",pEndPoint->des_para.AXIS_INTERS.wing_offset_dist2);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector")==id)
			sText.Printf("%s",CStdVector::GetVecTypeString(f3dPoint(pEndPoint->des_para.AXIS_INTERS.near_vector)));
		else if(GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector[0]")==id)
		{
			sText.Printf("%f",pEndPoint->des_para.AXIS_INTERS.near_vector[0]);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector[1]")==id)
		{
			sText.Printf("%f",pEndPoint->des_para.AXIS_INTERS.near_vector[1]);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector[2]")==id)
		{
			sText.Printf("%f",pEndPoint->des_para.AXIS_INTERS.near_vector[2]);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.AXIS_INTERS.norm_offset")==id)	//����ƫ����;
		{
			CDesignThickPara thick(pEndPoint->des_para.AXIS_INTERS.norm_offset);
			sText.Printf("%d",thick.Thick(console.GetActiveModel()));
		}
		else if(GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum1")==id)
			sText.Printf("0X%X",pEndPoint->des_para.AXIS_INTERS.hDatum1);
		else if(GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum2")==id)
			sText.Printf("0X%X",pEndPoint->des_para.AXIS_INTERS.hDatum2);
		else if(GetPropID("m_xLineEnd.des_para.DATUMLINE.hDatumLine")==id)
			sText.Printf("0X%X",pEndPoint->des_para.DATUMLINE.hDatumLine);
		else if(GetPropID("m_xLineEnd.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==id)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(pEndPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
				sText.Printf("0.X axis");
			else if(pEndPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
				sText.Printf("1.Y axis");
			else 
				sText.Printf("2.Z axis");
#else 
			if(pEndPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
				sText.Printf("0.X��");
			else if(pEndPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
				sText.Printf("1.Y��");
			else 
				sText.Printf("2.Z��");
#endif
		}
		else if(GetPropID("m_xLineEnd.des_para.DATUMPLANE.hDatumPlane")==id)
			sText.Printf("0X%X",pEndPoint->des_para.DATUMPLANE.hDatumPlane);
		else if(GetPropID("m_xLineEnd.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==id)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(pEndPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
				sText.Printf("0.X axis");
			else if(pEndPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
				sText.Printf("1.Y axis");
			else 
				sText.Printf("2.Z axis");
#else 
			if(pEndPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
				sText.Printf("0.X��");
			else if(pEndPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
				sText.Printf("1.Y��");
			else 
				sText.Printf("2.Z��");
#endif
		}
		else if(GetPropID("m_xLineEnd.des_para.RODEND.hRod")==id)	//�Ǹ־��
			sText.Printf("0X%X",pEndPoint->des_para.RODEND.hRod);
		else if(GetPropID("m_xLineEnd.des_para.RODEND.direction")==id)	
		{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
#ifdef AFX_TARG_ENU_ENGLISH
			if(pEndPoint->des_para.RODEND.direction==0)
				sText.Copy("0.Start->End");
			else 
				sText.Copy("1.End->Start");
#else 
			if(pEndPoint->des_para.RODEND.direction==0)
				sText.Copy("0.ʼ->��");
			else 
				sText.Copy("1.��->ʼ");
#endif
		}
		else if(GetPropID("m_xLineEnd.des_para.RODEND.len_offset_dist")==id)	
		{	//����ƫ��ֵ;
			sText.Printf("%f",pEndPoint->des_para.RODEND.len_offset_dist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.RODEND.norm_offset")==id)//����ƫ����
		{
			CDesignThickPara thick(pEndPoint->des_para.RODEND.norm_offset);
			sText.Printf("%d",thick.Thick(console.GetActiveModel()));
		}
		else if(GetPropID("m_xLineEnd.des_para.hNode")==id)
			sText.Printf("0X%X",pEndPoint->des_para.hNode);
		else if(GetPropID("des_para.hPart")==id)
			sText.Printf("0X%X",pEndPoint->des_para.hPart);
		else if(GetPropID("m_xLineEnd.datum_pos.x")==id)
		{
			sText.Printf("%f",pEndPoint->datum_pos.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.datum_pos.y")==id)
		{
			sText.Printf("%f",pEndPoint->datum_pos.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.datum_pos.z")==id)
		{
			sText.Printf("%f",pEndPoint->datum_pos.z);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.SCALE_NODE.hLinePart")==id)			//��׼�˼����
			sText.Printf("0X%0X",pEndPoint->des_para.SCALE_NODE.hLinePart);
		else if(GetPropID("des_para.SCALE_NODE.hStartNode")==id)		//ʼ�˽ڵ���
			sText.Printf("0X%0X",pEndPoint->des_para.SCALE_NODE.hStartNode);
		else if(GetPropID("des_para.SCALE_NODE.hEndNode")==id)			//�ն˽ڵ���
			sText.Printf("0X%0X",pEndPoint->des_para.SCALE_NODE.hEndNode);
		else if(GetPropID("m_xLineEnd.des_para.SCALE_NODE.start_offset_dist")==id)	//ʼ��ƫ����
		{
			sText.Printf("%f",pEndPoint->des_para.SCALE_NODE.start_offset_dist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.SCALE_NODE.end_offset_dist")==id)	//�ն�ƫ����
		{
			sText.Printf("%f",pEndPoint->des_para.SCALE_NODE.end_offset_dist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.SCALE_NODE.offset_scale")==id)		//ƫ�Ʊ���
		{
			sText.Printf("%f",pEndPoint->des_para.SCALE_NODE.offset_scale);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xLineEnd.des_para.SCALE_NODE.offset_dist")==id)		//�ڵȷֻ����ϵ�ƫ��ֵ
		{
			sText.Printf("%f",pEndPoint->des_para.SCALE_NODE.offset_dist);
			SimplifiedNumString(sText);
		}
	}
	else
	{	//��������
		if(GetPropID("m_xFaceNorm.norm_style")==id)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(pVector->norm_style==1)
				sText.Printf("1.Single Angle Leg Normal");
			else if(pVector->norm_style==2)
				sText.Printf("2.Two Rod Cross-bracing Normal");
			else if(pVector->norm_style==3)
				sText.Printf("3.Part Work Normal");
			else if(pVector->norm_style==4)
				sText.Printf("4.Part Relative Direction");
			else if(pVector->norm_style==5)
				sText.Printf("5.Rod Characteristic Direction");
			else //if(pVector->norm_style==0)
				sText.Printf("0.Specify Direction");
#else 
			if(pVector->norm_style==1)
				sText.Printf("1.���Ǹ�֫����");
			else if(pVector->norm_style==2)
				sText.Printf("2.���˼������淨��");
			else if(pVector->norm_style==3)
				sText.Printf("3.������������");
			else if(pVector->norm_style==4)
				sText.Printf("4.�����ϵ���Է���");
			else if(pVector->norm_style==5)
				sText.Printf("5.�˼��ϵ���������");
			else //if(pVector->norm_style==0)
				sText.Printf("0.ָ������");
#endif
		}
		else if(GetPropID("m_xFaceNorm.norm.x")==id)
		{
			sText.Printf("%lf",pVector->vector.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xFaceNorm.norm.y")==id)
		{
			sText.Printf("%lf",pVector->vector.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xFaceNorm.norm.z")==id)
		{
			sText.Printf("%lf",pVector->vector.z);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xFaceNorm.hVicePart")==id)
			sText.Printf("0X%X",pVector->hVicePart);
		else if(GetPropID("m_xFaceNorm.hCrossPart")==id)
			sText.Printf("0X%X",pVector->hCrossPart);
		else if(GetPropID("m_xFaceNorm.norm_wing")==id)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(pVector->norm_wing==0)
				sText.Printf("%s","0.X Leg Normal");
			else
				sText.Printf("%s","1.Y Leg Normal");
#else 
			if(pVector->norm_wing==0)
				sText.Printf("%s","0.X֫����");
			else
				sText.Printf("%s","1.Y֫����");
#endif
		}
		else if(GetPropID("m_xFaceNorm.direction")==id)
		{
			if(pVector->norm_style==1)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(pVector->direction==0)
					sText.Printf("%s","0.Outward");
				else
					sText.Printf("%s","1.Inward");
#else 
				if(pVector->direction==0)
					sText.Printf("%s","0.����");
				else
					sText.Printf("%s","1.����");
#endif
			}
			else if(pVector->norm_style==5)
			{
				CSuperSmartPtr<CLDSPart>pDatumPart=console.FromPartHandle(pVector->hVicePart);
#ifdef AFX_TARG_ENU_ENGLISH
				if(pVector->direction==0)
					sText.Copy("0.Start->End");
				else if(pVector->direction==1)
					sText.Copy("1.End->Start");
#else 
				if(pVector->direction==0)
					sText.Copy("0.ʼ->��");
				else if(pVector->direction==1)
					sText.Copy("1.��->ʼ");
#endif
				else
				{
					if(pDatumPart.IsHasPtr()&&pDatumPart->IsAngle())
					{
#ifdef AFX_TARG_ENU_ENGLISH
						if(pVector->direction==2)
							sText.Copy("2.X Leg Normal+");
						else if(pVector->direction==3)
							sText.Copy("3.X Leg Normal-");
						else if(pVector->direction==4)
							sText.Copy("4.Y Leg Normal+");
						else 
							sText.Copy("5.Y Leg Normal-");
#else 
						if(pVector->direction==2)
							sText.Copy("2.X֫����+");
						else if(pVector->direction==3)
							sText.Copy("3.X֫����-");
						else if(pVector->direction==4)
							sText.Copy("4.Y֫����+");
						else 
							sText.Copy("5.Y֫����-");
#endif
					}
					else if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
					{
#ifdef AFX_TARG_ENU_ENGLISH
						if(pVector->direction==2)
							sText.Copy("2.Rotate along datum plane");
						else if(pVector->direction==3)
							sText.Copy("3.Rotate along weld road plane");
#else 
						if(pVector->direction==2)
							sText.Copy("2.�Ի�׼����ת");
						else if(pVector->direction==3)
							sText.Copy("3.�Ժ�������ת");
#endif
					}
				}
			}
			else 
			{
				if(pVector->direction==0)
#ifdef AFX_TARG_ENU_ENGLISH
					sText.Printf("%s","0.Forward direction");
				else
					sText.Printf("%s","1.Reverse direction");
#else 
					sText.Printf("%s","0.����");
				else
					sText.Printf("%s","1.����");
#endif
			}
		}
		else if(GetPropID("m_xFaceNorm.rot_ang")==id)
		{
			sText.Printf("%f",pVector->rot_ang);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xFaceNorm.VectorType")==id)
			sText.Printf("%s",CStdVector::GetVecTypeString(pVector->vector));
		else if(GetPropID("m_xFaceNorm.nearVectorType")==id)
			sText.Printf("%s",CStdVector::GetVecTypeString(pVector->nearVector));
		else if(GetPropID("m_xFaceNorm.nearVector.x")==id)
		{
			sText.Printf("%lf",pVector->nearVector.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xFaceNorm.nearVector.y")==id)
		{
			sText.Printf("%lf",pVector->nearVector.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_xFaceNorm.nearVector.z")==id)
		{
			sText.Printf("%lf",pVector->nearVector.z);
			SimplifiedNumString(sText);
		}
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
void CDefinedViewDlg::UpdateDatumVectorPropItem(CPropTreeItem *pParentItem,BOOL bUpdate)
{
	if(pParentItem==NULL||m_pDrawingAssistant==NULL)
		return;
	//
	CItemInfo* lpInfo=NULL;
	CXhChar100 sText;
	CPropTreeItem *pPropItem=NULL,*pSonItem=NULL;
	CLDSVector *pVector=&(m_pDrawingAssistant->m_xFaceNorm);
	if(bUpdate)
	{
		m_propList.DeleteItemByPropId(GetPropID("m_xFaceNorm.hVicePart"));
		m_propList.DeleteItemByPropId(GetPropID("m_xFaceNorm.hCrossPart"));
		m_propList.DeleteItemByPropId(GetPropID("m_xFaceNorm.norm_wing"));
		m_propList.DeleteItemByPropId(GetPropID("m_xFaceNorm.direction"));
		m_propList.DeleteItemByPropId(GetPropID("m_xFaceNorm.VectorType"));
		m_propList.DeleteItemByPropId(GetPropID("m_xFaceNorm.nearVectorType"));
		m_propList.DeleteItemByPropId(GetPropID("m_xFaceNorm.nearVector.x"));
		m_propList.DeleteItemByPropId(GetPropID("m_xFaceNorm.nearVector.y"));
		m_propList.DeleteItemByPropId(GetPropID("m_xFaceNorm.nearVector.z"));
	}
	if(pVector->norm_style!=0)
	{
		m_propList.SetItemReadOnly(GetPropID("m_xFaceNorm.norm.x"),TRUE);
		m_propList.SetItemReadOnly(GetPropID("m_xFaceNorm.norm.y"),TRUE);
		m_propList.SetItemReadOnly(GetPropID("m_xFaceNorm.norm.z"),TRUE);
	}
	else 
	{
		int nVectorType=CStdVector::GetVectorType(pVector->vector);
		m_propList.SetItemReadOnly(CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.x"),nVectorType!=0);
		m_propList.SetItemReadOnly(CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.y"),nVectorType!=0);
		m_propList.SetItemReadOnly(CDefinedViewDlg::GetPropID("m_xFaceNorm.norm.z"),nVectorType!=0);
	}
	//ָ������
	if(pVector->norm_style==0)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specify Direction";
		lpInfo->m_strPropHelp = "Specified Direction";
#else 
		lpInfo->m_strPropName = "ָ������";
		lpInfo->m_strPropHelp = "ָ������";
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.VectorType");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	//���Ǹ�֫����
	else if(pVector->norm_style==1)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Angle";
		lpInfo->m_strPropHelp = "Datum angle handle";
#else 
		lpInfo->m_strPropName = "��׼�Ǹ�";
		lpInfo->m_strPropHelp = "��׼�Ǹ־��";
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems ="0.X Leg Normal|1.Y Leg Normal";
		lpInfo->m_strPropName = "Datum Leg";
		lpInfo->m_strPropHelp = "Datum Leg";
#else 
		lpInfo->m_cmbItems ="0.X֫����|1.Y֫����";
		lpInfo->m_strPropName = "��׼֫";
		lpInfo->m_strPropHelp = "��׼֫";
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.norm_wing");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems ="0.Outward|1.Inward";
		lpInfo->m_strPropName = "Direction";
		lpInfo->m_strPropHelp = "Direction";
#else 
		lpInfo->m_cmbItems ="0.����|1.����";
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "����";
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.direction");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	else if(pVector->norm_style==2)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Rod";
		lpInfo->m_strPropHelp = "Datum rod handle";
#else 
		lpInfo->m_strPropName = "��׼�˼�";
		lpInfo->m_strPropHelp = "��׼�˼����";
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Cross-bracing Reference Rod";
		lpInfo->m_strPropHelp = "Cross-bracing reference rod's handle";
#else 
		lpInfo->m_strPropName = "������ո˼�";
		lpInfo->m_strPropHelp = "������ո˼����";
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.hCrossPart");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Near Direction";
		lpInfo->m_strPropHelp = "The near direction distinguish whether or not forward normal direction";
#else 
		lpInfo->m_strPropName = "���Ʒ���";
		lpInfo->m_strPropHelp = "���Ʒ�������������㷽��Ϊ������������߻��Ǹ����߷���";
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.nearVectorType");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

		int nearVectorType=CStdVector::GetVectorType(pVector->nearVector);
		if(nearVectorType==0)	//�Զ���
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Component";
			lpInfo->m_strPropHelp = "X Component";
#else 
			lpInfo->m_strPropName = "X����";
			lpInfo->m_strPropHelp = "X����";
#endif
			pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =GetPropID("m_xFaceNorm.nearVector.x");
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Component";
			lpInfo->m_strPropHelp = "Y Component";
#else 
			lpInfo->m_strPropName = "Y����";
			lpInfo->m_strPropHelp = "Y����";
#endif
			pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =GetPropID("m_xFaceNorm.nearVector.y");
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Component";
			lpInfo->m_strPropHelp = "Z Component";
#else 
			lpInfo->m_strPropName = "Z����";
			lpInfo->m_strPropHelp = "Z����";
#endif
			pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =GetPropID("m_xFaceNorm.nearVector.z");
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
	}
	else if(pVector->norm_style==3)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Part";
		lpInfo->m_strPropHelp = "Datum Part";
#else 
		lpInfo->m_strPropName = "��׼����";
		lpInfo->m_strPropHelp = "��׼����";
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems ="0.Forward direction|1.Reverse direction";
		lpInfo->m_strPropName = "Direction";
		lpInfo->m_strPropHelp = "Forward direction is datum part's plus normal ��reverse direction is opposite direction";
#else 
		lpInfo->m_cmbItems ="0.����|1.����";
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "������ָ��׼���������������߷��򣻷�����ָ��׼�����ĸ��������߷���";
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.direction");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	else if(pVector->norm_style==4)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Part";
		lpInfo->m_strPropHelp = "Datum Part";
#else 
		lpInfo->m_strPropName = "��׼����";
		lpInfo->m_strPropHelp = "��׼����";
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Relative Direction";
		lpInfo->m_strPropHelp = "The relative direction of datum part on assemble coordinate system.";
#else 
		lpInfo->m_strPropName = "��Է���";
		lpInfo->m_strPropHelp = "��׼����װ������ϵ�µ���Է���";
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.nearVectorType");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

		int nearVectorType=CStdVector::GetVectorType(pVector->nearVector);
		if(nearVectorType==0)	//�Զ���
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Component";
			lpInfo->m_strPropHelp = "X Component";
#else 
			lpInfo->m_strPropName = "X����";
			lpInfo->m_strPropHelp = "X����";
#endif
			pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =GetPropID("m_xFaceNorm.nearVector.x");
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Component";
			lpInfo->m_strPropHelp = "Y Component";
#else 
			lpInfo->m_strPropName = "Y����";
			lpInfo->m_strPropHelp = "Y����";
#endif
			pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =GetPropID("m_xFaceNorm.nearVector.y");
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Component";
			lpInfo->m_strPropHelp = "Z Component";
#else 
			lpInfo->m_strPropName = "Z����";
			lpInfo->m_strPropHelp = "Z����";
#endif
			pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =GetPropID("m_xFaceNorm.nearVector.z");
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
	}
	else if(pVector->norm_style==5)
	{	//5.��׼�˼��ϵ���������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Rod";
		lpInfo->m_strPropHelp = "Datum rod handle";
#else 
		lpInfo->m_strPropName = "��׼�˼�";
		lpInfo->m_strPropHelp = "��׼�˼����";
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		
		//��������
		bool bNeedRotAnglePara=false;
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(pVector->hVicePart,CLS_LINEPART);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		if(pLinePart&&pLinePart->IsAngle())
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems ="0.Start->End|1.End->Start|2.X Leg Normal+|3.X Leg Normal-|4.Y Leg Normal+|5.Y Leg Normal-";
#else 
			lpInfo->m_cmbItems ="0.ʼ->��|1.��->ʼ|2.X֫����+|3.X֫����-|4.Y֫����+|5.Y֫����-";
#endif
		else if(pLinePart&&pLinePart->IsTube())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems = "0.Start->End|1.End->Start|2.Rotate Along Datum Plane|3.Rotate Along Weld Road Plane";
#else 
			lpInfo->m_cmbItems = "0.ʼ->��|1.��->ʼ|2.�Ի�׼����ת|3.�Ժ�������ת";
#endif
			if(pVector->direction>=2)
				bNeedRotAnglePara=true;
		}
		else 
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems ="0.Start->End|1.End->Start";
		lpInfo->m_strPropName = "Direction Type";
		lpInfo->m_strPropHelp = "0.Rod start->end direction,1.Rod end->start direction,2.Angle X-leg  normal,3.Angle X-leg normal reverse direction,4.Angle Y-leg  normal,5.Angle Y-leg normal reverse direction";
#else 
			lpInfo->m_cmbItems ="0.ʼ->��|1.��->ʼ";
		lpInfo->m_strPropName = "��������";
		lpInfo->m_strPropHelp = "0.�˼�ʼ->�շ���,1.�˼���->ʼ����,2.�Ǹ�X֫���߷���,3.�Ǹ�X֫���߸�����,4.�Ǹ�Y֫���߷���,5.�Ǹ�Y֫���߸�����";
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.direction");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		if(bNeedRotAnglePara)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType=CDT_EDIT;
			lpInfo->m_cmbItems = "0|90|180|270";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rotation Angle";
			if(pVector->direction==2)
				lpInfo->m_strPropHelp="The rotation angle from tube's datum plane around the axial line��";
			else 
				lpInfo->m_strPropHelp="The rotation angle from tube's weld plane around the axial line��";
#else 
			lpInfo->m_strPropName = "��ת�Ƕ�";
			if(pVector->direction==2)
				lpInfo->m_strPropHelp="�Ըֹܻ�׼��ƽ�淨�������ߵ���ת�Ƕȡ�";
			else 
				lpInfo->m_strPropHelp="�Ըֹܺ�����ƽ�淨�������ߵ���ת�Ƕȡ�";
#endif
			pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp = GetPropID("m_xFaceNorm.rot_ang");
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
	}
}

void CDefinedViewDlg::UpdateDatumPointPropItem(CPropTreeItem *pParentItem,CLDSPoint *pPoint,BOOL bUpdate)
{
	if(pParentItem==NULL||m_pDrawingAssistant==NULL)
		return;
	//
	CItemInfo* lpInfo=NULL;
	CXhChar100 valueStr;
	CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL;
	if(pPoint==&m_pDrawingAssistant->m_xLineStart)
	{
		if(bUpdate)
		{
			//��ڵ�ͬλ
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.hNode"));
			//ָ�������ϵ��������
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.hPart"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.datum_pos"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.datum_pos.x"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.datum_pos.y"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.datum_pos.z"));
			//���߽���
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style1"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style2"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist1"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist2"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[0]"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[1]"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[2]"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.AXIS_INTERS.norm_offset"));
			//�Ǹ����
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.hRod"));				
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.direction"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.offset_wing"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.wing_offset_style"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.wing_offset_dist"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.len_offset_dist"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.bIncOddEffect"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.RODEND.norm_offset"));	
			//�˼��Ͻڵ�Ϊ��׼
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.hLinePart"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.hNode"));	
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.bFaceOffset"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.direction"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.offset_wing"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_style"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_dist"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.len_offset_dist"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.norm_offset"));	
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]"));
			//��׼�����������겻���
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.DATUMLINE.hDatumLine"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2"));
			//��׼�����������겻���
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.DATUMPLANE.hDatumPlane"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineStart.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2"));
		}
		//���»�׼��ֻ������
		m_propList.SetItemReadOnly(GetPropID("m_xLineStart.x"),pPoint->datum_pos_style!=0);
		m_propList.SetItemReadOnly(GetPropID("m_xLineStart.y"),pPoint->datum_pos_style!=0);
		m_propList.SetItemReadOnly(GetPropID("m_xLineStart.z"),pPoint->datum_pos_style!=0);
		if(pPoint->datum_pos_style==1)	//�Ǹ����
		{	//��׼�Ǹ־��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Angle Handle";
			lpInfo->m_strPropHelp="Datum angle handle";
#else 
			lpInfo->m_strPropName="��׼�Ǹ־��";
			lpInfo->m_strPropHelp="��׼�Ǹ־����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.hRod");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ��֫
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X Leg|1.Y Leg";
			lpInfo->m_strPropName="Offset Leg";
			lpInfo->m_strPropHelp="Offset Leg��";
#else 
			lpInfo->m_cmbItems="0.X֫|1.Y֫";
			lpInfo->m_strPropName="ƫ��֫";
			lpInfo->m_strPropHelp="ƫ��֫��";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.offset_wing");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3| user specify |";
			lpInfo->m_strPropName="Leg Offset Type";
			lpInfo->m_strPropHelp="The offset stadia or user specified offset.";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������";
			lpInfo->m_strPropHelp="ƫ������Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.wing_offset_style");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset";
			lpInfo->m_strPropHelp="Leg Offset";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ";
			lpInfo->m_strPropHelp="֫��ƫ��ֵ��";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.wing_offset_dist");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(pPoint->des_para.RODEND.wing_offset_style!=4);
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
			lpInfo->m_strPropName="Vertical Offset Direction";
			lpInfo->m_strPropHelp="Vertical offset direction,along the direction of datum angle start to the end offset or along the opposite direction";
#else 
			lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
			lpInfo->m_strPropName="����ƫ�Ʒ���";
			lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.direction");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ��ֵ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Vertical Direction Offset";
			lpInfo->m_strPropHelp="Vertical direction offset";
#else 
			lpInfo->m_strPropName="����ƫ��ֵ";
			lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.len_offset_dist");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ��ֵ��������ͷӰ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName="Odd Length Influence";
			lpInfo->m_strPropHelp="Whether or not included in odd length influence of vertical offset.";
#else 
			lpInfo->m_cmbItems="��|��";
			lpInfo->m_strPropName="����ƫ�Ƽ�������ͷӰ��";
			lpInfo->m_strPropHelp="����ƫ�Ƽ�������ͷӰ�졣";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.bIncOddEffect");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Normal Offset";
			lpInfo->m_strPropHelp="Normal Offset";
#else 
			lpInfo->m_strPropName="����ƫ����";
			lpInfo->m_strPropHelp="����ƫ����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.norm_offset");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		else if(pPoint->datum_pos_style==2)	//�˼��Ͻڵ㶨λ
		{
			//��׼�˼����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Rod Handle";
			lpInfo->m_strPropHelp="Datum Rod Handle��";
#else 
			lpInfo->m_strPropName="��׼�˼����";
			lpInfo->m_strPropHelp="��׼�˼������";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.hLinePart");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//��׼�ڵ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Node Handle";
			lpInfo->m_strPropHelp="Datum Node Handle��";
#else 
			lpInfo->m_strPropName="��׼�ڵ���";
			lpInfo->m_strPropHelp="��׼�ڵ�����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.hNode");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(pPoint->des_para.RODNODE.hRod ,CLS_LINEPART);
			if(pLinePart&&pLinePart->IsAngle())
			{	//ƫ��֫
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="0.X Leg|1.Y Leg";
				lpInfo->m_strPropName="Offset Leg";
				lpInfo->m_strPropHelp="Offset Leg��";
#else 
				lpInfo->m_cmbItems="0.X֫|1.Y֫";
				lpInfo->m_strPropName="ƫ��֫";
				lpInfo->m_strPropHelp="ƫ��֫��";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.offset_wing");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="g|g1|g2|g3| user specify |";
				lpInfo->m_strPropName="Leg Offset Type";
				lpInfo->m_strPropHelp="The offset stadia or user specified offset.";
#else 
				lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
				lpInfo->m_strPropName="֫��ƫ������";
				lpInfo->m_strPropHelp="ƫ������Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_style");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//ƫ�ƾ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Leg Offset";
				lpInfo->m_strPropHelp="Leg Offset��";
#else 
				lpInfo->m_strPropName="֫��ƫ��ֵ";
				lpInfo->m_strPropHelp="֫��ƫ��ֵ��";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_dist");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->SetReadOnly(pPoint->des_para.RODNODE.wing_offset_style!=4);
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			}
			//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
			lpInfo->m_strPropName="Vertical Offset Direction ";
			lpInfo->m_strPropHelp="Vertical offset direction,along the direction of datum angle start to the end offset or along the opposite direction";
#else 
			lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
			lpInfo->m_strPropName="����ƫ�Ʒ���";
			lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.direction");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ��ֵ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Vertical Direction Offset";
			lpInfo->m_strPropHelp="Vertical Direction Offset";
#else 
			lpInfo->m_strPropName="����ƫ��ֵ";
			lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.len_offset_dist");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Normal Offset";
			lpInfo->m_strPropHelp="Normal Offset";
#else 
			lpInfo->m_strPropName="����ƫ����";
			lpInfo->m_strPropHelp="����ƫ����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.norm_offset");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƽ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName="Offset Flatly";
			lpInfo->m_strPropHelp="Offset Flatly";
#else 
			lpInfo->m_cmbItems="��|��";
			lpInfo->m_strPropName="ƽ��";
			lpInfo->m_strPropHelp="ƽ��";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.bFaceOffset");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			if(pPoint->des_para.RODNODE.bFaceOffset)
			{	//ƽ���淨��
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
				lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Offset Plane Normal";
				lpInfo->m_strPropHelp="Offset flatly plane's normal";
#else 
				lpInfo->m_strPropName="ƽ���淨��";
				lpInfo->m_strPropHelp="ƽ���淨��";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				int vectorType=CStdVector::GetVectorType(pPoint->des_para.RODNODE.vPlaneNormal);
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The x component of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��X�������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(vectorType!=0);
				pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The y component of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��Y�������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(vectorType!=0);
				pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The z component of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��Z�������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(vectorType!=0);
				pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			}
		}
		else if(pPoint->datum_pos_style==3)	//�Ǹ����߽���
		{
			//��׼�˼����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Rod 1 Handle";
			lpInfo->m_strPropHelp="Datum Rod 1 Handle��";
#else 
			lpInfo->m_strPropName="��׼�˼�1���";
			lpInfo->m_strPropHelp="��׼�˼�1�����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			CLDSLinePart *pLinePart1=(CLDSLinePart*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEPART);
			if(pLinePart1&&pLinePart1->IsAngle())
			{	//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="g|g1|g2|g3| user specify |";
				lpInfo->m_strPropName="Leg Offset Type 1";
				lpInfo->m_strPropHelp="Datum Angle1 Offset Type:Scheduled Stadia Or User Specify Offset Distance��";
#else 
				lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
				lpInfo->m_strPropName="֫��ƫ������1";
				lpInfo->m_strPropHelp="��׼�Ǹ�1ƫ������:Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style1");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//ƫ�ƾ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Leg Offset 1";
				lpInfo->m_strPropHelp="Datum Angle1 Leg Offset��";
#else 
				lpInfo->m_strPropName="֫��ƫ��ֵ1";
				lpInfo->m_strPropHelp="��׼�Ǹ�1֫��ƫ��ֵ��";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist1");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->SetReadOnly(pPoint->des_para.AXIS_INTERS.wing_offset_style1!=4);
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			}
			//��׼�˼�2���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Rod 2 Handle";
			lpInfo->m_strPropHelp="Datum Rod 2 Handle";
#else 
			lpInfo->m_strPropName="��׼�˼�2���";
			lpInfo->m_strPropHelp="��׼�˼�2���";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			CLDSLinePart *pLinePart2=(CLDSLinePart*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEPART);
			if(pLinePart2&&pLinePart2->IsAngle())
			{	//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="g|g1|g2|g3| user specify |";
				lpInfo->m_strPropName="Leg Offset Type 2";
				lpInfo->m_strPropHelp="The offset type of datum angle 2:Scheduled stadia or user specify offset distance.";
#else 
				lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
				lpInfo->m_strPropName="֫��ƫ������2";
				lpInfo->m_strPropHelp="��׼�Ǹ�2ƫ������:Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style2");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//ƫ�ƾ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Leg Offset 2";
				lpInfo->m_strPropHelp="The leg offset of datum angle 2.";
#else 
				lpInfo->m_strPropName="֫��ƫ��ֵ2";
				lpInfo->m_strPropHelp="��׼�Ǹ�2֫��ƫ��ֵ��";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist2");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->SetReadOnly(pPoint->des_para.AXIS_INTERS.wing_offset_style2!=4);
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			}
			//����ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Normal Offset";
			lpInfo->m_strPropHelp="Normal Offset";
#else 
			lpInfo->m_strPropName="����ƫ����";
			lpInfo->m_strPropHelp="����ƫ����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.norm_offset");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//��򿪺����޸ķ���ƫ�ƺ���޷���ʱ��ʾ,�������ϴ�,�������� wjh-2011.8.10
			//if(strlen(pPoint->des_para.AXIS_INTERS.norm_offset)>0)
			{
				//���Ʒ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
				lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Near Normal";
				lpInfo->m_strPropHelp="Near Normal";
#else 
				lpInfo->m_strPropName="���Ʒ���";
				lpInfo->m_strPropHelp="���Ʒ���";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				pPropItem->m_bHideChildren=TRUE;
				if(GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = GetPropStatus(pPropItem->m_idProp);
				int vectorType=CStdVector::GetVectorType(f3dPoint(pPoint->des_para.AXIS_INTERS.near_vector));
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The X component of near normal";
#else 
				lpInfo->m_strPropHelp="���Ʒ���X�������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[0]");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(vectorType!=0);
				pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The y component of near normal";
#else 
				lpInfo->m_strPropHelp="���Ʒ���Y�������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[1]");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(vectorType!=0);
				pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The z component of near normal";
#else 
				lpInfo->m_strPropHelp="���Ʒ���Z�������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[2]");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(vectorType!=0);
				pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			}
		}
		else if(pPoint->datum_pos_style==4)	//��׼�߽���
		{
			//��׼��1���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Line 1 Handle";
			lpInfo->m_strPropHelp="Datum Line 1 Handle";
#else 
			lpInfo->m_strPropName="��׼��1���";
			lpInfo->m_strPropHelp="��׼��1���";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//��׼��2���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Line 2 Handle";
			lpInfo->m_strPropHelp="Datum Line 2 Handle";
#else 
			lpInfo->m_strPropName="��׼��2���";
			lpInfo->m_strPropHelp="��׼��2���";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		else if(pPoint->datum_pos_style==5)	//��׼�����������겻���
		{
			//��׼�߾��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Line Handle";
			lpInfo->m_strPropHelp="Datum Line Handle";
#else 
			lpInfo->m_strPropName="��׼�߾��";
			lpInfo->m_strPropHelp="��׼�߾��";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.DATUMLINE.hDatumLine");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ָ������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X axis |1.Y axis |2.Z axis ";
			lpInfo->m_strPropName="Specify Axial";
			lpInfo->m_strPropHelp="Specified axial";
#else 
			lpInfo->m_cmbItems="0.X��|1.Y��|2.Z��";
			lpInfo->m_strPropName="ָ������";
			lpInfo->m_strPropHelp="ָ������";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		else if(pPoint->datum_pos_style==6)	//��׼�����������겻���
		{
			//��׼����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Plane Handle";
			lpInfo->m_strPropHelp="Datum Plane Handle";
#else 
			lpInfo->m_strPropName="��׼����";
			lpInfo->m_strPropHelp="��׼����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.DATUMPLANE.hDatumPlane");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ָ������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X axis |1.Y axis |2.Z axis ";
			lpInfo->m_strPropName="Specify Axial";
			lpInfo->m_strPropHelp="Specified axial";
#else 
			lpInfo->m_cmbItems="0.X��|1.Y��|2.Z��";
			lpInfo->m_strPropName="ָ������";
			lpInfo->m_strPropHelp="ָ������";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		else if(pPoint->datum_pos_style==7)	//�ؽǸ���֫����ƫ�Ƶİ������
		{
			//��׼�Ǹ־��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Angle Handle";
			lpInfo->m_strPropHelp="Datum Angle Handle��";
#else 
			lpInfo->m_strPropName="��׼�Ǹ־��";
			lpInfo->m_strPropHelp="��׼�Ǹ־����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.hRod");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
			lpInfo->m_strPropName="Vertical Offset Direction ";
			lpInfo->m_strPropHelp="Vertical offset direction,along the direction of datum angle start to the end offset or along the opposite direction";
#else 
			lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
			lpInfo->m_strPropName="����ƫ�Ʒ���";
			lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.direction");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ��ֵ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Vertical Direction Offset";
			lpInfo->m_strPropHelp="Vertical Direction Offset";
#else 
			lpInfo->m_strPropName="����ƫ��ֵ";
			lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.len_offset_dist");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Normal Offset";
			lpInfo->m_strPropHelp="Normal Offset";
#else 
			lpInfo->m_strPropName="����ƫ����";
			lpInfo->m_strPropHelp="����ƫ����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.norm_offset");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		else if(pPoint->datum_pos_style==8)	//��ڵ�ͬΪ
		{	//��׼�ڵ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Node Handle";
			lpInfo->m_strPropHelp="Datum Node Handle��";
#else 
			lpInfo->m_strPropName="��׼�ڵ���";
			lpInfo->m_strPropHelp="��׼�ڵ�����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.hNode");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		else if(pPoint->datum_pos_style==9)	//ָ�������ϵ��������
		{
			//��׼�Ǹ־��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Part Handle";
			lpInfo->m_strPropHelp="Datum Part Handle��";
#else 
			lpInfo->m_strPropName="��׼�������";
			lpInfo->m_strPropHelp="��׼���������";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.hPart");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//�������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Relative Coordinates";
			lpInfo->m_strPropHelp="Relative Coordinates";
#else 
			lpInfo->m_strPropName="�������";
			lpInfo->m_strPropHelp="�������";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.datum_pos");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The x component of relative coordinates";
#else 
			lpInfo->m_strPropHelp="�������X�������";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = GetPropID("m_xLineStart.datum_pos.x");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The y component of relative coordinates";
#else 
			lpInfo->m_strPropHelp="�������Y�������";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = GetPropID("m_xLineStart.datum_pos.y");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The z component of relative coordinates";
#else 
			lpInfo->m_strPropHelp="�������Z�������";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = GetPropID("m_xLineStart.datum_pos.z");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
	}
	else
	{
		if(bUpdate)
		{
			//��ڵ�ͬλ
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.hNode"));
			//ָ�������ϵ��������
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.hPart"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.datum_pos"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.datum_pos.x"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.datum_pos.y"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.datum_pos.z"));
			//���߽���
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum1"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum2"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_style1"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_style2"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_dist1"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_dist2"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector[0]"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector[1]"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector[2]"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.AXIS_INTERS.norm_offset"));
			//�Ǹ����
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.hRod"));				
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.direction"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.offset_wing"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.wing_offset_style"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.wing_offset_dist"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.len_offset_dist"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.bIncOddEffect"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.RODEND.norm_offset"));	
			//�˼��Ͻڵ�Ϊ��׼
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.hLinePart"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.hNode"));	
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.bFaceOffset"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.direction"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.offset_wing"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.wing_offset_style"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.wing_offset_dist"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.len_offset_dist"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.norm_offset"));	
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[0]"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[1]"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[2]"));
			//��׼�����������겻���
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.DATUMLINE.hDatumLine"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2"));
			//��׼�����������겻���
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.DATUMPLANE.hDatumPlane"));
			m_propList.DeleteItemByPropId(CDefinedViewDlg::GetPropID("m_xLineEnd.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2"));
		}
		//���»�׼��ֻ������
		m_propList.SetItemReadOnly(GetPropID("m_xLineEnd.x"),pPoint->datum_pos_style!=0);
		m_propList.SetItemReadOnly(GetPropID("m_xLineEnd.y"),pPoint->datum_pos_style!=0);
		m_propList.SetItemReadOnly(GetPropID("m_xLineEnd.z"),pPoint->datum_pos_style!=0);
		if(pPoint->datum_pos_style==1)	//�Ǹ����
		{	//��׼�Ǹ־��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Angle Handle";
			lpInfo->m_strPropHelp="Datum Angle Handle��";
#else 
			lpInfo->m_strPropName="��׼�Ǹ־��";
			lpInfo->m_strPropHelp="��׼�Ǹ־����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.RODEND.hRod");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ��֫
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X Leg|1.Y Leg";
			lpInfo->m_strPropName="Offset Leg";
			lpInfo->m_strPropHelp="Offset Leg��";
#else 
			lpInfo->m_cmbItems="0.X֫|1.Y֫";
			lpInfo->m_strPropName="ƫ��֫";
			lpInfo->m_strPropHelp="ƫ��֫��";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.RODEND.offset_wing");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3| user specify |";
			lpInfo->m_strPropName="Leg Offset Type";
			lpInfo->m_strPropHelp="The offset stadia or user specified offset.";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������";
			lpInfo->m_strPropHelp="ƫ������Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.RODEND.wing_offset_style");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset";
			lpInfo->m_strPropHelp="Leg Offset��";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ";
			lpInfo->m_strPropHelp="֫��ƫ��ֵ��";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.RODEND.wing_offset_dist");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(pPoint->des_para.RODEND.wing_offset_style!=4);
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
			lpInfo->m_strPropName="Vertical Offset Direction ";
			lpInfo->m_strPropHelp="Vertical offset direction,along the direction of datum angle start to the end offset or along the opposite direction";
#else 
			lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
			lpInfo->m_strPropName="����ƫ�Ʒ���";
			lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.RODEND.direction");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ��ֵ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Vertical Direction Offset";
			lpInfo->m_strPropHelp="Vertical Direction Offset";
#else 
			lpInfo->m_strPropName="����ƫ��ֵ";
			lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.RODEND.len_offset_dist");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ��ֵ��������ͷӰ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName="Odd Length Influence";
			lpInfo->m_strPropHelp="Whether or not included in odd length influence of vertical offset.";
#else 
			lpInfo->m_cmbItems="��|��";
			lpInfo->m_strPropName="����ƫ�Ƽ�������ͷӰ��";
			lpInfo->m_strPropHelp="����ƫ�Ƽ�������ͷӰ�졣";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.RODEND.bIncOddEffect");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Normal Offset";
			lpInfo->m_strPropHelp="Normal Offset";
#else 
			lpInfo->m_strPropName="����ƫ����";
			lpInfo->m_strPropHelp="����ƫ����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.RODEND.norm_offset");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		else if(pPoint->datum_pos_style==2)	//�˼��Ͻڵ㶨λ
		{
			//��׼�˼����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Rod Handle";
			lpInfo->m_strPropHelp="Datum Rod Handle��";
#else 
			lpInfo->m_strPropName="��׼�˼����";
			lpInfo->m_strPropHelp="��׼�˼������";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.LINEPARTNODE.hLinePart");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//��׼�ڵ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Node Handle";
			lpInfo->m_strPropHelp="Datum Node Handle��";
#else 
			lpInfo->m_strPropName="��׼�ڵ���";
			lpInfo->m_strPropHelp="��׼�ڵ�����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.LINEPARTNODE.hNode");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(pPoint->des_para.RODNODE.hRod ,CLS_LINEPART);
			if(pLinePart&&pLinePart->IsAngle())
			{	//ƫ��֫
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="0.X Leg|1.Y Leg";
				lpInfo->m_strPropName="Offset Leg";
				lpInfo->m_strPropHelp="Offset Leg��";
#else 
				lpInfo->m_cmbItems="0.X֫|1.Y֫";
				lpInfo->m_strPropName="ƫ��֫";
				lpInfo->m_strPropHelp="ƫ��֫��";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.LINEPARTNODE.offset_wing");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="g|g1|g2|g3| user specify |";
				lpInfo->m_strPropName="Leg Offset Type";
				lpInfo->m_strPropHelp="The offset stadia or user specified offset.";
#else 
				lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
				lpInfo->m_strPropName="֫��ƫ������";
				lpInfo->m_strPropHelp="ƫ������Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.LINEPARTNODE.wing_offset_style");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//ƫ�ƾ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Leg Offset";
				lpInfo->m_strPropHelp="Leg Offset��";
#else 
				lpInfo->m_strPropName="֫��ƫ��ֵ";
				lpInfo->m_strPropHelp="֫��ƫ��ֵ��";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.LINEPARTNODE.wing_offset_dist");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->SetReadOnly(pPoint->des_para.RODNODE.wing_offset_style!=4);
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			}
			//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
			lpInfo->m_strPropName="Vertical Offset Direction ";
			lpInfo->m_strPropHelp="Vertical offset direction,along the direction of datum angle start to the end offset or along the opposite direction";
#else 
			lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
			lpInfo->m_strPropName="����ƫ�Ʒ���";
			lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.LINEPARTNODE.direction");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ��ֵ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Vertical Direction Offset";
			lpInfo->m_strPropHelp="Vertical Direction Offset";
#else 
			lpInfo->m_strPropName="����ƫ��ֵ";
			lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.LINEPARTNODE.len_offset_dist");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Normal Offset";
			lpInfo->m_strPropHelp="Normal Offset";
#else 
			lpInfo->m_strPropName="����ƫ����";
			lpInfo->m_strPropHelp="����ƫ����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.LINEPARTNODE.norm_offset");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƽ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName="Offset Flatly";
			lpInfo->m_strPropHelp="Offset flatly";
#else 
			lpInfo->m_cmbItems="��|��";
			lpInfo->m_strPropName="ƽ��";
			lpInfo->m_strPropHelp="ƽ��";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.LINEPARTNODE.bFaceOffset");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			if(pPoint->des_para.RODNODE.bFaceOffset)
			{	//ƽ���淨��
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
				lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Offset Plane Normal";
				lpInfo->m_strPropHelp="Offset flatly plane's normal";
#else 
				lpInfo->m_strPropName="ƽ���淨��";
				lpInfo->m_strPropHelp="ƽ���淨��";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				int vectorType=CStdVector::GetVectorType(pPoint->des_para.RODNODE.vPlaneNormal);
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The x component of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��X�������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[0]");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(vectorType!=0);
				pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The y component of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��Y�������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[1]");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(vectorType!=0);
				pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The z component of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��Z�������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.LINEPARTNODE.face_norm_offset[2]");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(vectorType!=0);
				pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			}
		}
		else if(pPoint->datum_pos_style==3)	//�Ǹ����߽���
		{
			//��׼�˼����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Rod 1 Handle";
			lpInfo->m_strPropHelp="Datum rod 1 handle��";
#else 
			lpInfo->m_strPropName="��׼�˼�1���";
			lpInfo->m_strPropHelp="��׼�˼�1�����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum1");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			CLDSLinePart *pLinePart1=(CLDSLinePart*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEPART);
			if(pLinePart1&&pLinePart1->IsAngle())
			{	//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="g|g1|g2|g3| user specify |";
				lpInfo->m_strPropName="Leg Offset Type 1";
				lpInfo->m_strPropHelp="The offset type of datum angle 1:Scheduled stadia or user specify offset distance��";
#else 
				lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
				lpInfo->m_strPropName="֫��ƫ������1";
				lpInfo->m_strPropHelp="��׼�Ǹ�1ƫ������:Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_style1");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//ƫ�ƾ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Leg Offset 1";
				lpInfo->m_strPropHelp="The leg offset of datum angle 1";
#else 
				lpInfo->m_strPropName="֫��ƫ��ֵ1";
				lpInfo->m_strPropHelp="��׼�Ǹ�1֫��ƫ��ֵ��";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_dist1");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->SetReadOnly(pPoint->des_para.AXIS_INTERS.wing_offset_style1!=4);
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			}
			//��׼�˼�2���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Rod 2 Handle";
			lpInfo->m_strPropHelp="Datum rod 2 handle";
#else 
			lpInfo->m_strPropName="��׼�˼�2���";
			lpInfo->m_strPropHelp="��׼�˼�2���";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum2");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			CLDSLinePart *pLinePart2=(CLDSLinePart*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEPART);
			if(pLinePart2&&pLinePart2->IsAngle())
			{	//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="g|g1|g2|g3| user specify |";
				lpInfo->m_strPropName="Leg Offset Type 2";
				lpInfo->m_strPropHelp="The offset type of datum angle 2:Scheduled stadia or user specify offset distance��";
#else 
				lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
				lpInfo->m_strPropName="֫��ƫ������2";
				lpInfo->m_strPropHelp="��׼�Ǹ�2ƫ������:Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_style2");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//ƫ�ƾ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Leg Offset 2";
				lpInfo->m_strPropHelp="The leg offset of datum angle 2.";
#else 
				lpInfo->m_strPropName="֫��ƫ��ֵ2";
				lpInfo->m_strPropHelp="��׼�Ǹ�2֫��ƫ��ֵ��";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.AXIS_INTERS.wing_offset_dist2");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->SetReadOnly(pPoint->des_para.AXIS_INTERS.wing_offset_style2!=4);
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			}
			//����ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Normal Offset";
			lpInfo->m_strPropHelp="Normal Offset";
#else 
			lpInfo->m_strPropName="����ƫ����";
			lpInfo->m_strPropHelp="����ƫ����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.AXIS_INTERS.norm_offset");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//if(strlen(pPoint->des_para.AXIS_INTERS.norm_offset)>0)
			{
				//���Ʒ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
				lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Near Normal";
				lpInfo->m_strPropHelp="Near normal";
#else 
				lpInfo->m_strPropName="���Ʒ���";
				lpInfo->m_strPropHelp="���Ʒ���";
#endif
				pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
				pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				pPropItem->m_bHideChildren=TRUE;
				if(GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = GetPropStatus(pPropItem->m_idProp);
				int vectorType=CStdVector::GetVectorType(f3dPoint(pPoint->des_para.AXIS_INTERS.near_vector));
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The X component of near normal";
#else 
				lpInfo->m_strPropHelp="���Ʒ���X�������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector[0]");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(vectorType!=0);
				pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The y component of near normal";
#else 
				lpInfo->m_strPropHelp="���Ʒ���Y�������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector[1]");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(vectorType!=0);
				pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The z component of near normal";
#else 
				lpInfo->m_strPropHelp="���Ʒ���Z�������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.AXIS_INTERS.near_vector[2]");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(vectorType!=0);
				pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			}
		}
		else if(pPoint->datum_pos_style==4)	//��׼�߽���
		{
			//��׼��1���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Line 1 Handle";
			lpInfo->m_strPropHelp="Datum line 1 handle";
#else 
			lpInfo->m_strPropName="��׼��1���";
			lpInfo->m_strPropHelp="��׼��1���";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum1");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//��׼��2���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Line 2 Handle";
			lpInfo->m_strPropHelp="Datum line 2 handle";
#else 
			lpInfo->m_strPropName="��׼��2���";
			lpInfo->m_strPropHelp="��׼��2���";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum2");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		else if(pPoint->datum_pos_style==5)	//��׼�����������겻���
		{
			//��׼�߾��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Line Handle";
			lpInfo->m_strPropHelp="Datum line handle";
#else 
			lpInfo->m_strPropName="��׼�߾��";
			lpInfo->m_strPropHelp="��׼�߾��";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.DATUMLINE.hDatumLine");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ָ������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X axis |1.Y axis |2.Z axis ";
			lpInfo->m_strPropName="Specify Axial";
			lpInfo->m_strPropHelp="Specified axial";
#else 
			lpInfo->m_cmbItems="0.X��|1.Y��|2.Z��";
			lpInfo->m_strPropName="ָ������";
			lpInfo->m_strPropHelp="ָ������";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		else if(pPoint->datum_pos_style==6)	//��׼�����������겻���
		{
			//��׼����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Plane Handle";
			lpInfo->m_strPropHelp="Datum plane handle";
#else 
			lpInfo->m_strPropName="��׼����";
			lpInfo->m_strPropHelp="��׼����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.DATUMPLANE.hDatumPlane");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ָ������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X axis|1.Y axis|2.Z axis";
			lpInfo->m_strPropName="Specify Axial";
			lpInfo->m_strPropHelp="Specified axial";
#else 
			lpInfo->m_cmbItems="0.X��|1.Y��|2.Z��";
			lpInfo->m_strPropName="ָ������";
			lpInfo->m_strPropHelp="ָ������";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		else if(pPoint->datum_pos_style==7)	//�ؽǸ���֫����ƫ�Ƶİ������
		{
			//��׼�Ǹ־��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Angle Handle";
			lpInfo->m_strPropHelp="Datum angle handle��";
#else 
			lpInfo->m_strPropName="��׼�Ǹ־��";
			lpInfo->m_strPropHelp="��׼�Ǹ־����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.RODEND.hRod");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
			lpInfo->m_strPropName="Vertical Offset Direction ";
			lpInfo->m_strPropHelp="Vertical offset direction,along the direction of datum angle start to the end offset or along the opposite direction";
#else 
			lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
			lpInfo->m_strPropName="����ƫ�Ʒ���";
			lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.RODEND.direction");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ��ֵ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Vertical Direction Offset";
			lpInfo->m_strPropHelp="Vertical direction offset";
#else 
			lpInfo->m_strPropName="����ƫ��ֵ";
			lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.RODEND.len_offset_dist");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//����ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Normal Offset";
			lpInfo->m_strPropHelp="Normal offset";
#else 
			lpInfo->m_strPropName="����ƫ����";
			lpInfo->m_strPropHelp="����ƫ����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.RODEND.norm_offset");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		else if(pPoint->datum_pos_style==8)	//��ڵ�ͬΪ
		{	//��׼�ڵ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Node Handle";
			lpInfo->m_strPropHelp="Datum node handle��";
#else 
			lpInfo->m_strPropName="��׼�ڵ���";
			lpInfo->m_strPropHelp="��׼�ڵ�����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.hNode");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		else if(pPoint->datum_pos_style==9)	//ָ�������ϵ��������
		{
			//��׼�Ǹ־��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Part Handle";
			lpInfo->m_strPropHelp="Datum part handle��";
#else 
			lpInfo->m_strPropName="��׼�������";
			lpInfo->m_strPropHelp="��׼���������";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.des_para.hPart");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//�������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Relative Coordinates";
			lpInfo->m_strPropHelp="Relative coordinates";
#else 
			lpInfo->m_strPropName="�������";
			lpInfo->m_strPropHelp="�������";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineEnd.datum_pos");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The x component of relative coordinates";
#else 
			lpInfo->m_strPropHelp="�������X�������";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = GetPropID("m_xLineEnd.datum_pos.x");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The y component of relative coordinates";
#else 
			lpInfo->m_strPropHelp="�������Y�������";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = GetPropID("m_xLineEnd.datum_pos.y");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The z component of relative coordinates";
#else 
			lpInfo->m_strPropHelp="�������Z�������";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = GetPropID("m_xLineEnd.datum_pos.z");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
	}
}
static CString GetDisplayViewPlaneSet(CDisplayView *pView)
{
#ifdef AFX_TARG_ENU_ENGLISH
	CString itemSet="0.Datum Plane";
#else 
	CString itemSet="0.����";
#endif
	if(pView->m_iViewType==1)
	{
		for(CDrawingAssistant *pAssis=pView->listAssist.GetFirst();pAssis;pAssis=pView->listAssist.GetNext())
		{
			if(pAssis->m_nStyle==0)
			{
				itemSet+="|";
				CString sName="";
				sName.Format("%d.%s",pAssis->iNo,pAssis->name);
				itemSet+=sName;
			}
		}
	}
	return itemSet;
}
void CDefinedViewDlg::DisplayDrawingAssistProperty(CDrawingAssistant *pAssist)
{
	CItemInfo* lpInfo=NULL;
	CXhChar100 valueStr;
	CPropTreeItem *pPropItem=NULL,*pPointItem=NULL,*pVectorItem=NULL,*pGroupItem=NULL,*pSonPropItem=NULL,*pRootItem=m_propList.GetRootItem();
	m_propList.CleanTree();
	if(pAssist==NULL)
		return;
	else
		m_pDrawingAssistant=pAssist;

	m_propList.m_nObjClassTypeId=0;
	if(pAssist->m_nStyle!=0)
		m_propList.m_nObjClassTypeId=1;
	
	CLDSPoint *pStartPoint=NULL;
	CLDSPoint *pEndPoint=NULL;
	CLDSVector *pVector=NULL;
	pStartPoint=&(pAssist->m_xLineStart);
	if(pAssist->m_nStyle==0)
		pVector=&(pAssist->m_xFaceNorm);
	else 
	{
		pEndPoint=&pAssist->m_xLineEnd;
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Affiliate plane";
		lpInfo->m_strPropHelp="Ancillary line in the draw plane";
#else 
		lpInfo->m_strPropName="����ƽ��";
		lpInfo->m_strPropHelp="���������ڵĻ���ƽ��";
#endif
		lpInfo->m_cmbItems=GetDisplayViewPlaneSet(m_pDefinedView);
		pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
		pGroupItem->m_idProp = GetPropID("residePlane");
		if(GetPropValueStr(pGroupItem->m_idProp,valueStr)>0)
			pGroupItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMPOINT);
	}
	if(pStartPoint)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_STATIC;
		if(pAssist->m_nStyle==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Spread Line Datum Point";
			lpInfo->m_strPropHelp="Spread line datum point��due to the datum point of the drawing datum plane exists uncertainty line,the datum points of spread-plane must be located in spread-line��";
#else 
			lpInfo->m_strPropName="չ���߻�׼��";
			lpInfo->m_strPropHelp="չ���߻�׼�㣬���ڻ�ͼ����Ļ�����ڲ�ȷ���ߣ�չ����Ļ������λ��չ�����ϡ�";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Ancillary Line Start Point";
			lpInfo->m_strPropHelp="Ancillary line start point��";
#else 
			lpInfo->m_strPropName="���������";
			lpInfo->m_strPropHelp="��������㡣";
#endif
		}
		pPointItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
		pPointItem->m_idProp = GetPropID("m_xLineStart");
		if(GetPropValueStr(pPointItem->m_idProp,valueStr)>0)
			pPointItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPointItem->m_bHideChildren=FALSE;
		if(GetPropStatus(pPointItem->m_idProp)!=-1)
			pPointItem->m_bHideChildren = GetPropStatus(pPointItem->m_idProp);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Loaction Coordinates";
		lpInfo->m_strPropHelp="Loaction coordinates";
#else 
		lpInfo->m_strPropName="λ������";
		lpInfo->m_strPropHelp="λ������";
#endif
		pGroupItem = m_propList.InsertItem(pPointItem,lpInfo,-1);
		pGroupItem->m_idProp = GetPropID("m_xLineStart.pos");
		if(GetPropValueStr(pGroupItem->m_idProp,valueStr)>0)
			pGroupItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pGroupItem->m_bHideChildren=TRUE;
		if(GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = GetPropStatus(pGroupItem->m_idProp);
		//pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMPOINT);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="X Coordinate";
		lpInfo->m_strPropHelp="X Coordinate";
#else 
		lpInfo->m_strPropName="X����";
		lpInfo->m_strPropHelp="X����";
#endif
		pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
		pSonPropItem->m_idProp = GetPropID("m_xLineStart.x");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
		pSonPropItem->SetReadOnly(pStartPoint->datum_pos_style!=0);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Y Coordinate";
		lpInfo->m_strPropHelp="Y Coordinate";
#else 
		lpInfo->m_strPropName="Y����";
		lpInfo->m_strPropHelp="Y����";
#endif
		pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
		pSonPropItem->m_idProp = GetPropID("m_xLineStart.y");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
		pSonPropItem->SetReadOnly(pStartPoint->datum_pos_style!=0);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Z Coordinate";
		lpInfo->m_strPropHelp="Z Coordinate";
#else 
		lpInfo->m_strPropName="Z����";
		lpInfo->m_strPropHelp="Z����";
#endif
		pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
		pSonPropItem->m_idProp = GetPropID("m_xLineStart.z");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
		pSonPropItem->SetReadOnly(pStartPoint->datum_pos_style!=0);
		//��׼�㶨λ��ʽ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Manual Input|1.Angle Ridge Vertex Locating Point|2.Rod Node Locating Point|3.Angle Center Line Intersection Point|4.Baseline Space Intersection Point|5.Specified Component Point Of Datum Line|6.Specified Component Point Of Datum Plane|7.Outer Angle's Ridge Vertex Along Angle's Normal Offset|8.With The Node In The Same Position|9.Datum Rod Relative Coordinate";
		lpInfo->m_strPropName="Locate Mode";
		lpInfo->m_strPropHelp="Locate mode";
#else 
		lpInfo->m_cmbItems="0.�ֶ�����|1.�Ǹ���㶨λ��|2.�˼��ڵ㶨λ��|3.�Ǹ����߽���|4.��׼�߿ռ佻��|5.��׼����ָ�����������|6.��׼����ָ�������|7.�ؽǸ���֫����ƫ�Ƶİ������|8.��ڵ�ͬλ|9.��׼�����ϵ��������";
		lpInfo->m_strPropName="��λ��ʽ";
		lpInfo->m_strPropHelp="��λ��ʽ";
#endif
		pGroupItem = m_propList.InsertItem(pPointItem,lpInfo,-1);
		pGroupItem->m_idProp = GetPropID("m_xLineStart.datum_pos_style");
		if(GetPropValueStr(pGroupItem->m_idProp,valueStr)>0)
			pGroupItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pGroupItem->m_bHideChildren=FALSE;
		if(GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = GetPropStatus(pGroupItem->m_idProp);
		//pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMPOINT);
		//��׼������
		UpdateDatumPointPropItem(pGroupItem,pStartPoint,FALSE);
	}
	if(pEndPoint)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_STATIC;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Ancillary Line End Point";
		lpInfo->m_strPropHelp="Ancillary line end point��";
#else 
		lpInfo->m_strPropName="�������յ�";
		lpInfo->m_strPropHelp="�������յ㡣";
#endif
		pPointItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
		pPointItem->m_idProp = GetPropID("m_xLineEnd");
		if(GetPropValueStr(pPointItem->m_idProp,valueStr)>0)
			pPointItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPointItem->m_bHideChildren=FALSE;
		if(GetPropStatus(pPointItem->m_idProp)!=-1)
			pPointItem->m_bHideChildren = GetPropStatus(pGroupItem->m_idProp);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Position Coordinate";
		lpInfo->m_strPropHelp="Position coordinate";
#else 
		lpInfo->m_strPropName="λ������";
		lpInfo->m_strPropHelp="λ������";
#endif
		pGroupItem = m_propList.InsertItem(pPointItem,lpInfo,-1);
		pGroupItem->m_idProp = GetPropID("m_xLineEnd.pos");
		if(GetPropValueStr(pGroupItem->m_idProp,valueStr)>0)
			pGroupItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pGroupItem->m_bHideChildren=TRUE;
		if(GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = GetPropStatus(pGroupItem->m_idProp);
		//pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMPOINT);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="X Coordinate";
		lpInfo->m_strPropHelp="X Coordinate";
#else 
		lpInfo->m_strPropName="X����";
		lpInfo->m_strPropHelp="X����";
#endif
		pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
		pSonPropItem->m_idProp = GetPropID("m_xLineEnd.x");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
		pSonPropItem->SetReadOnly(pStartPoint->datum_pos_style!=0);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Y Coordinate";
		lpInfo->m_strPropHelp="Y Coordinate";
#else 
		lpInfo->m_strPropName="Y����";
		lpInfo->m_strPropHelp="Y����";
#endif
		pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
		pSonPropItem->m_idProp = GetPropID("m_xLineEnd.y");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
		pSonPropItem->SetReadOnly(pStartPoint->datum_pos_style!=0);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Z Coordinate";
		lpInfo->m_strPropHelp="Z Coordinate";
#else 
		lpInfo->m_strPropName="Z����";
		lpInfo->m_strPropHelp="Z����";
#endif
		pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
		pSonPropItem->m_idProp = GetPropID("m_xLineEnd.z");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
		pSonPropItem->SetReadOnly(pStartPoint->datum_pos_style!=0);
		//��׼�㶨λ��ʽ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Manual Input|1.Angle Ridge Vertex Locating Point|2.Rod Node Locating Point|3.Angle Center Line Intersection Point|4.Baseline Space Intersection Point|5.Specified Component Point Of Datum Line|6.Specified Component Point Of Datum Plane|7.Outer Angle's Ridge Vertex Along Angle's Normal Offset|8.With The Node In The Same Position|9.Datum Rod Relative Coordinate";
		lpInfo->m_strPropName="Locate Mode";
		lpInfo->m_strPropHelp="Locate mode";
#else 
		lpInfo->m_cmbItems="0.�ֶ�����|1.�Ǹ���㶨λ��|2.�˼��ڵ㶨λ��|3.�Ǹ����߽���|4.��׼�߿ռ佻��|5.��׼����ָ�����������|6.��׼����ָ�������|7.�ؽǸ���֫����ƫ�Ƶİ������|8.��ڵ�ͬλ|9.��׼�����ϵ��������";
		lpInfo->m_strPropName="��λ��ʽ";
		lpInfo->m_strPropHelp="��λ��ʽ";
#endif
		pGroupItem = m_propList.InsertItem(pPointItem,lpInfo,-1);
		pGroupItem->m_idProp = GetPropID("m_xLineEnd.datum_pos_style");
		if(GetPropValueStr(pGroupItem->m_idProp,valueStr)>0)
			pGroupItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pGroupItem->m_bHideChildren=FALSE;
		if(GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = GetPropStatus(pGroupItem->m_idProp);
		//pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMPOINT);
		//��׼������
		UpdateDatumPointPropItem(pGroupItem,pEndPoint,FALSE);
	}
	if(pVector)
	{
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_STATIC;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Spread Plane Normal";
		lpInfo->m_strPropHelp="Spread Plane Normal��";
#else 
		lpInfo->m_strPropName="չ���淨�߷���";
		lpInfo->m_strPropHelp="չ���淨�߷���";
#endif
		pVectorItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
		pVectorItem->m_idProp = GetPropID("m_xFaceNorm");
		if(GetPropValueStr(pVectorItem->m_idProp,valueStr)>0)
			pVectorItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pVectorItem->m_bHideChildren=FALSE;
		if(GetPropStatus(pVectorItem->m_idProp)!=-1)
			pVectorItem->m_bHideChildren = GetPropStatus(pVectorItem->m_idProp);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Coordinate Component ";
		lpInfo->m_strPropHelp="Coordinate Component ";
#else 
		lpInfo->m_strPropName="�������";
		lpInfo->m_strPropHelp="�������";
#endif
		pGroupItem = m_propList.InsertItem(pVectorItem,lpInfo,-1);
		pGroupItem->m_idProp = GetPropID("m_xFaceNorm.norm");
		if(GetPropValueStr(pGroupItem->m_idProp,valueStr)>0)
			pGroupItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pGroupItem->m_bHideChildren=TRUE;
		if(GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = GetPropStatus(pGroupItem->m_idProp);
		//pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMVECTOR);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Normal's x component";
#else 
		lpInfo->m_strPropHelp="���߷���X�������";
#endif
		pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
		pSonPropItem->m_idProp = GetPropID("m_xFaceNorm.norm.x");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Normal's y component";
#else 
		lpInfo->m_strPropHelp="���߷���Y�������";
#endif
		pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
		pSonPropItem->m_idProp = GetPropID("m_xFaceNorm.norm.y");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Normal's z component";
#else 
		lpInfo->m_strPropHelp="���߷���Z�������";
#endif
		pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
		pSonPropItem->m_idProp = GetPropID("m_xFaceNorm.norm.z");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
		//��׼�㶨λ��ʽ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Specify Direction|1.Single Angle Leg Normal|2.Two Rod Cross-bracing Normal|3.Part Work Normal|4.Part Relative Direction|5.Rod Characteristic Direction";
		lpInfo->m_strPropName="Normal Locate Mode";
		lpInfo->m_strPropHelp="Normal locate mode";
#else 
		lpInfo->m_cmbItems="0.ָ������|1.���Ǹ�֫����|2.���˼������淨��|3.������������|4.�����ϵ���Է���|5.�˼��ϵ���������";
		lpInfo->m_strPropName="���߶�λ��ʽ";
		lpInfo->m_strPropHelp="���߶�λ��ʽ";
#endif
		pGroupItem = m_propList.InsertItem(pVectorItem,lpInfo,-1);
		pGroupItem->m_idProp = GetPropID("m_xFaceNorm.norm_style");
		if(GetPropValueStr(pGroupItem->m_idProp,valueStr)>0)
			pGroupItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pGroupItem->m_bHideChildren=FALSE;
		if(GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = GetPropStatus(pGroupItem->m_idProp);
		//pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMVECTOR);
		//��������
		UpdateDatumVectorPropItem(pGroupItem,FALSE);
	}
	m_propList.Redraw();
}

//���ѡ�����ĺ�������
void CDefinedViewDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart||m_pDrawingAssistant==NULL)
		return;
	CLDSPoint *pStartPoint=&(m_pDrawingAssistant->m_xLineStart);
	CLDSPoint *pEndPoint=&(m_pDrawingAssistant->m_xLineEnd);
	CLDSVector *pVector=&(m_pDrawingAssistant->m_xFaceNorm);
	if(CDefinedViewDlg::GetPropID("m_xFaceNorm.hVicePart")==GetEventPropId())
	{
		CLDSPart *pPart=NULL;
		if(pVector->norm_style==1)
			pPart=console.FromPartHandle(m_hPickObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		else if(pVector->norm_style==2||pVector->norm_style==5)
			pPart=console.FromPartHandle(m_hPickObj,CLS_LINEPART);
		else 
			pPart=console.FromPartHandle(m_hPickObj);
		if(pPart)
		{
			pVector->hVicePart=pPart->handle;
			pVector->UpdateVector(console.GetActiveModel());
		}
	}
	else if(pVector->norm_style==2&&CDefinedViewDlg::GetPropID("m_xFaceNorm.hCrossPart")==GetEventPropId())
	{
		CLDSPart *pPart=console.FromPartHandle(m_hPickObj);
		if(pPart)
		{
			pVector->hCrossPart=pPart->handle;
			pVector->UpdateVector(console.GetActiveModel());
		}
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(GetEventPropId()==GetPropID("m_xLineStart.des_para.RODEND.hRod"))
	{
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(m_hPickObj,CLS_LINEANGLE);
		if(pLineAngle)
		{
			pStartPoint->des_para.RODEND.hRod=pLineAngle->handle;
			pStartPoint->UpdatePos(console.GetActiveModel());
		}
	} 
#endif
	else if(GetEventPropId()==GetPropID("m_xLineStart.des_para.LINEPARTNODE.hLinePart"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
		if(pLinePart)
		{
			pStartPoint->des_para.RODNODE.hRod =pLinePart->handle;
			pStartPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(GetEventPropId()==GetPropID("m_xLineStart.des_para.LINEPARTNODE.hNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
		if(pNode)
		{
			pStartPoint->des_para.RODNODE.hNode=pNode->handle;
			pStartPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(GetEventPropId()==GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
		if(pLinePart)
		{
			pStartPoint->des_para.AXIS_INTERS.hDatum1=pLinePart->handle;
			pStartPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(GetEventPropId()==GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
		if(pLinePart)
		{
			pStartPoint->des_para.AXIS_INTERS.hDatum2=pLinePart->handle;
			pStartPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(GetEventPropId()==GetPropID("m_xLineStart.des_para.hNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
		if(pNode)
		{
			pStartPoint->des_para.hNode=pNode->handle;
			pStartPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(GetEventPropId()==GetPropID("m_xLineStart.des_para.hPart"))
	{
		CLDSPart *pPart=console.FromPartHandle(m_hPickObj);
		if(pPart)
		{
			pStartPoint->des_para.hPart=pPart->handle;
			pStartPoint->UpdatePos(console.GetActiveModel());
		}
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(GetEventPropId()==GetPropID("m_xLineEnd.des_para.RODEND.hRod"))
	{
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(m_hPickObj,CLS_LINEANGLE);
		if(pLineAngle)
		{
			pEndPoint->des_para.RODEND.hRod=pLineAngle->handle;
			pEndPoint->UpdatePos(console.GetActiveModel());
		}
	} 
#endif
	else if(GetEventPropId()==GetPropID("m_xLineEnd.des_para.LINEPARTNODE.hLinePart"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
		if(pLinePart)
		{
			pEndPoint->des_para.RODNODE.hRod =pLinePart->handle;
			pEndPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(GetEventPropId()==GetPropID("m_xLineEnd.des_para.LINEPARTNODE.hNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
		if(pNode)
		{
			pEndPoint->des_para.RODNODE.hNode=pNode->handle;
			pEndPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(GetEventPropId()==GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum1"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
		if(pLinePart)
		{
			pEndPoint->des_para.AXIS_INTERS.hDatum1=pLinePart->handle;
			pEndPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(GetEventPropId()==GetPropID("m_xLineEnd.des_para.AXIS_INTERS.hDatum2"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
		if(pLinePart)
		{
			pEndPoint->des_para.AXIS_INTERS.hDatum2=pLinePart->handle;
			pEndPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(GetEventPropId()==GetPropID("m_xLineEnd.des_para.hNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
		if(pNode)
		{
			pEndPoint->des_para.hNode=pNode->handle;
			pEndPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(GetEventPropId()==GetPropID("m_xLineEnd.des_para.hPart"))
	{
		CLDSPart *pPart=console.FromPartHandle(m_hPickObj);
		if(pPart)
		{
			pEndPoint->des_para.hPart=pPart->handle;
			pEndPoint->UpdatePos(console.GetActiveModel());
		}
	}
}

void CDefinedViewDlg::OnItemchangedListDrawingAssistant(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(m_bFireListItemChange&&pNMListView->iItem>=0)
	{
		m_pDrawingAssistant=(CDrawingAssistant*)m_xDrawingAssistantList.GetItemData(pNMListView->iItem);
		if(m_pDrawingAssistant)
			DisplayDrawingAssistProperty(m_pDrawingAssistant);
	}
	
	*pResult = 0;
}

void CDefinedViewDlg::OnEndlabeleditListDrawingAssistant(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM *pItem = &pDispInfo->item;
	// In case window is being destroyed
    if(!IsWindow(GetSafeHwnd()))
		return;
	if(pItem==NULL||(pItem&&pItem->pszText==NULL))
		return;
    // Only set as modified if (a) it actually was, and (b) ESC was not hit.
    if(pItem->lParam != VK_ESCAPE &&
       m_pListCtrl->GetItemText(pItem->iItem, pItem->iSubItem) != pItem->pszText)
	{
		m_xDrawingAssistantList.UpdateData();
		m_xDrawingAssistantList.SetItemText(pItem->iItem,pItem->iSubItem,pItem->pszText);
		CDrawingAssistant *pAssis=(CDrawingAssistant*)m_xDrawingAssistantList.GetItemData(pItem->iItem);
		if(pAssis)
			strncpy(pAssis->name,pItem->pszText,9);
	}
	
	*pResult = 0;
}

void CDefinedViewDlg::OnCancel() 
{
	if(m_pDefinedView)
		m_pDefinedView->RestoreFrom(backupview);
	
	CDialog::OnCancel();
}
