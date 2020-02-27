// ExtraAngleOddSetDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "lds.h"
#include "ExtraAngleOddSetDlg.h"
#include "DatumPointDlg.h"
#include "env_def.h"
#include "XhCharString.h"
#include "GlobalFunc.h"
#include "ScrTools.h"
#include "ReportPointDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL ModifyExtraOddDatumPointProperty(CPropertyList* pPropList,CPropTreeItem *pItem,CString& valueStr)
{
	CExtraAngleOddSetDlg *pExtraOddDlg=(CExtraAngleOddSetDlg*)pPropList->GetParent();
	if(pExtraOddDlg==NULL)
		return FALSE;
	CLDSPoint *pPoint=NULL;
	EXTRA_ODDINFO *pExtraInfo=pExtraOddDlg->GetCurExtraOddInfo();
	if(pExtraInfo==NULL)
		return FALSE;
	pPoint=&pExtraInfo->extraDatumPoint;
	CParaPointLifeObj locklife;	//��ֹCLDSPoint::UpdatePos��CLDSLineAngle::GetDatumPosBer����ѭ������ wjh-2018.1.5
	char value_str[100]="";
	if(CLDSPoint::GetPropID("datum_pos_style")==pItem->m_idProp)
	{
		pPoint->datum_pos_style=valueStr[0]-'0';
		pExtraOddDlg->UpdateDatumPointStyle(pItem,pPoint,TRUE);
	}
	else if(pPoint->datum_pos_style==1)	//�Ǹ���㶨λ
	{
		if(CLDSPoint::GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)	//�Ǹ־��
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.RODEND.hRod));
		else if(CLDSPoint::GetPropID("des_para.RODEND.direction")==pItem->m_idProp)	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
			pPoint->des_para.RODEND.direction=valueStr[0]-'0';
		else if(CLDSPoint::GetPropID("des_para.RODEND.offset_wing")==pItem->m_idProp)//0.X֫;1.Y֫
			pPoint->des_para.RODEND.offset_wing=valueStr[0]-'0';
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CLDSPoint::GetPropID("des_para.RODEND.wing_offset_style")==pItem->m_idProp)	
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pPoint->des_para.RODEND.wing_offset_style=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pPoint->des_para.RODEND.wing_offset_style=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pPoint->des_para.RODEND.wing_offset_style=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pPoint->des_para.RODEND.wing_offset_style=3;
			else 
				pPoint->des_para.RODEND.wing_offset_style=4;
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.RODEND.hRod,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG&&pPoint->des_para.RODEND.offset_wing==0)
					jgzj=pLineAngle->xWingXZhunJu;	//X֫
				else if(pLineAngle->m_bEnableTeG&&pPoint->des_para.RODEND.offset_wing==1)
					jgzj=pLineAngle->xWingYZhunJu;	//X֫
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				if(pPoint->des_para.RODEND.wing_offset_style==0)
					pPoint->des_para.RODEND.wing_offset_dist=jgzj.g;
				else if(pPoint->des_para.RODEND.wing_offset_style==1)
					pPoint->des_para.RODEND.wing_offset_dist=jgzj.g1;
				else if(pPoint->des_para.RODEND.wing_offset_style==2)
					pPoint->des_para.RODEND.wing_offset_dist=jgzj.g2;
				else if(pPoint->des_para.RODEND.wing_offset_style==3)
					pPoint->des_para.RODEND.wing_offset_dist=jgzj.g3;
			}
			long idProp=CExtraAngleOddSetDlg::GetPropID("des_para.RODEND.wing_offset_dist");
			if(pPoint->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.RODEND.wing_offset_style!=4);
		}
#endif
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.RODEND.wing_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODEND.wing_offset_dist=atof(valueStr);
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.RODEND.len_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.RODEND.bIncOddEffect")==pItem->m_idProp)		
		{	//����ƫ��ֵ��������ͷӰ��
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
#endif
				pPoint->des_para.RODEND.bIncOddEffect=TRUE;
			else 
				pPoint->des_para.RODEND.bIncOddEffect=FALSE;
		}
	}
	else if(pPoint->datum_pos_style==2)	//�˼��ڵ㶨λ
	{
		if(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.hLinePart")==pItem->m_idProp)	//�Ǹ־��
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.RODNODE.hRod ));
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.hNode")==pItem->m_idProp)	//��׼�ڵ���
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.RODNODE.hNode));
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.direction")==pItem->m_idProp)	
			pPoint->des_para.RODNODE.direction=valueStr[0]-'0';
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.offset_wing")==pItem->m_idProp)	
			pPoint->des_para.RODNODE.offset_wing=valueStr[0]-'0';
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.wing_offset_style")==pItem->m_idProp)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pPoint->des_para.RODNODE.wing_offset_style=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pPoint->des_para.RODNODE.wing_offset_style=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pPoint->des_para.RODNODE.wing_offset_style=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pPoint->des_para.RODNODE.wing_offset_style=3;
			else 
				pPoint->des_para.RODNODE.wing_offset_style=4;
			//
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.RODNODE.hRod ,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG&&pPoint->des_para.RODEND.offset_wing==0)
					jgzj=pLineAngle->xWingXZhunJu;	//X֫
				else if(pLineAngle->m_bEnableTeG&&pPoint->des_para.RODEND.offset_wing==1)
					jgzj=pLineAngle->xWingYZhunJu;	//X֫
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				if(pPoint->des_para.RODNODE.wing_offset_style==0)
					pPoint->des_para.RODNODE.wing_offset_dist=jgzj.g;
				else if(pPoint->des_para.RODNODE.wing_offset_style==1)
					pPoint->des_para.RODNODE.wing_offset_dist=jgzj.g1;
				else if(pPoint->des_para.RODNODE.wing_offset_style==2)
					pPoint->des_para.RODNODE.wing_offset_dist=jgzj.g2;
				else if(pPoint->des_para.RODNODE.wing_offset_style==3)
					pPoint->des_para.RODNODE.wing_offset_dist=jgzj.g3;
			}
			long idProp=CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.wing_offset_dist");
			if(pPoint->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.RODNODE.wing_offset_style!=4);
		}	
#endif
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.wing_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODNODE.wing_offset_dist=atof(valueStr);
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.len_offset_dist")==pItem->m_idProp)	
			pPoint->des_para.RODNODE.len_offset_dist=atof(valueStr);
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.bFaceOffset")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
#endif
				pPoint->des_para.RODNODE.bFaceOffset=TRUE;
			else 
				pPoint->des_para.RODNODE.bFaceOffset=FALSE;
			pPropList->DeleteItemByPropId(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset"));
			pPropList->DeleteItemByPropId(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]"));
			pPropList->DeleteItemByPropId(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]"));
			pPropList->DeleteItemByPropId(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]"));
			if(pPoint->des_para.RODNODE.bFaceOffset)
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
				pPropItem->m_idProp = CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset");
				if(pPoint->GetPropValueStr(pPropItem->m_idProp,value_str)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=value_str;
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
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,TRUE);
				pSonPropItem->m_idProp = CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
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
				pSonPropItem->m_idProp = CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
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
				pSonPropItem->m_idProp = CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
			}
		}
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset")==pItem->m_idProp)
		{
			f3dPoint norm=CStdVector::GetVector(valueStr[0]-'0');
			pPoint->des_para.RODNODE.vPlaneNormal=norm;
			long idProp=CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.RODNODE.vPlaneNormal.x);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.RODNODE.vPlaneNormal.y);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.RODNODE.vPlaneNormal.z);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
		}
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]")==pItem->m_idProp)
			pPoint->des_para.RODNODE.vPlaneNormal.x=atof(valueStr);
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]")==pItem->m_idProp)
			pPoint->des_para.RODNODE.vPlaneNormal.y=atof(valueStr);
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]")==pItem->m_idProp)
			pPoint->des_para.RODNODE.vPlaneNormal.z=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==3)	//�˼����߽���
	{
		if(CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum1));
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum2));
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_style1")==pItem->m_idProp)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=3;
			else 
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=4;
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE);
			CLDSLineAngle *pCrossAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG)
				{
					f3dPoint work_norm;
					if(pLineAngle&&pCrossAngle)
					{
						f3dPoint line_vec1=pLineAngle->End()-pLineAngle->Start();
						f3dPoint line_vec2=pCrossAngle->End()-pCrossAngle->Start();
						work_norm=line_vec1^line_vec2;
						normalize(work_norm);
					}
					if(fabs(work_norm*pLineAngle->get_norm_x_wing())>fabs(work_norm*pLineAngle->get_norm_y_wing()))
						jgzj=pLineAngle->xWingXZhunJu;	//X֫
					else 
						jgzj=pLineAngle->xWingYZhunJu;	//Y֫
				}
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==0)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==1)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g1;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==2)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g2;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==3)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g3;
			}
			long idProp=CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist1");
			if(pPoint->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.AXIS_INTERS.wing_offset_style1!=4);
		}
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_style2")==pItem->m_idProp)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=3;
			else 
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=4;
			//
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
			CLDSLineAngle *pCrossAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG)
				{
					f3dPoint work_norm;
					if(pLineAngle&&pCrossAngle)
					{
						f3dPoint line_vec1=pLineAngle->End()-pLineAngle->Start();
						f3dPoint line_vec2=pCrossAngle->End()-pCrossAngle->Start();
						work_norm=line_vec1^line_vec2;
						normalize(work_norm);
					}
					if(fabs(work_norm*pLineAngle->get_norm_x_wing())>fabs(work_norm*pLineAngle->get_norm_y_wing()))
						jgzj=pLineAngle->xWingXZhunJu;	//X֫
					else 
						jgzj=pLineAngle->xWingYZhunJu;	//Y֫
				}
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==0)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==1)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g1;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==2)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g2;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==3)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g3;
			}
			long idProp=CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist2");
			if(pPoint->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.AXIS_INTERS.wing_offset_style2!=4);
		}
#endif
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist1")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.wing_offset_dist1=atol(valueStr);
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist2")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.wing_offset_dist2=atol(valueStr);
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.near_vector")==pItem->m_idProp)
		{
			f3dPoint vec=CStdVector::GetVector(valueStr[0]-'0');
			pPoint->des_para.AXIS_INTERS.near_vector[0]=vec.x;
			pPoint->des_para.AXIS_INTERS.near_vector[1]=vec.y;
			pPoint->des_para.AXIS_INTERS.near_vector[2]=vec.z;
			long idProp=CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.near_vector[0]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.AXIS_INTERS.near_vector[0]);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.near_vector[1]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.AXIS_INTERS.near_vector[1]);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.near_vector[2]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.AXIS_INTERS.near_vector[2]);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
		}
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.near_vector[0]")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.near_vector[0]=atof(valueStr);
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.near_vector[1]")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.near_vector[1]=atof(valueStr);
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.near_vector[2]")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.near_vector[2]=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==4)	//��׼�߿ռ佻��
	{
		if(CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum1));
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum2));
	}
	else if(pPoint->datum_pos_style==5)	//��׼����ָ�����������
	{
		if(CExtraAngleOddSetDlg::GetPropID("des_para.DATUMLINE.hDatumLine")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.DATUMLINE.hDatumLine));
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==pItem->m_idProp)
			pPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=valueStr[0]-'0';
	}
	else if(pPoint->datum_pos_style==6)	//��׼����ָ�������
	{
		if(CExtraAngleOddSetDlg::GetPropID("des_para.DATUMPLANE.hDatumPlane")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.DATUMPLANE.hDatumPlane));
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==pItem->m_idProp)
			pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=valueStr[0]-'0';
	}
	else if(pPoint->datum_pos_style==7)	//�ؽǸ���֫����ƫ�Ƶİ������
	{
		if(CExtraAngleOddSetDlg::GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)	//�Ǹ־��
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.RODEND.hRod));
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.RODEND.direction")==pItem->m_idProp)	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
			pPoint->des_para.RODEND.direction=valueStr[0]-'0';
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.RODEND.len_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==8)	//��ڵ�ͬλ
	{
		if(CExtraAngleOddSetDlg::GetPropID("des_para.hNode")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.hNode));
	}
	else if(pPoint->datum_pos_style==9)	//ָ�������ϵ��������
	{
		if(CExtraAngleOddSetDlg::GetPropID("des_para.hPart")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.hPart));
		else if(CExtraAngleOddSetDlg::GetPropID("datum_pos.x")==pItem->m_idProp)
			pPoint->datum_pos.x=atof(valueStr);
		else if(CExtraAngleOddSetDlg::GetPropID("datum_pos.y")==pItem->m_idProp)
			pPoint->datum_pos.y=atof(valueStr);
		else if(CExtraAngleOddSetDlg::GetPropID("datum_pos.z")==pItem->m_idProp)
			pPoint->datum_pos.z=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==10)
	{
		if(CExtraAngleOddSetDlg::GetPropID("des_para.SCALE_NODE.hLinePart")==pItem->m_idProp)//��׼�˼����
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.SCALE_NODE.hLinePart));
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.SCALE_NODE.hStartNode")==pItem->m_idProp)	//ʼ�˽ڵ���
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.SCALE_NODE.hStartNode));
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.SCALE_NODE.hEndNode")==pItem->m_idProp)		//�ն˽ڵ���
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.SCALE_NODE.hEndNode));
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.SCALE_NODE.start_offset_dist")==pItem->m_idProp)//ʼ��ƫ����
			pPoint->des_para.SCALE_NODE.start_offset_dist=atof(valueStr);
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.SCALE_NODE.end_offset_dist")==pItem->m_idProp)	//�ն�ƫ����
			pPoint->des_para.SCALE_NODE.end_offset_dist=atof(valueStr);
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.SCALE_NODE.offset_scale")==pItem->m_idProp)		//ƫ�Ʊ���
			pPoint->des_para.SCALE_NODE.offset_scale=atof(valueStr);
		else if(CExtraAngleOddSetDlg::GetPropID("des_para.SCALE_NODE.offset_dist")==pItem->m_idProp)		//�ڵȷֻ����ϵ�ƫ��ֵ
			pPoint->des_para.SCALE_NODE.offset_dist=atof(valueStr);
	}
	pExtraOddDlg->UpdateExtraOddDisplayValue();
	return TRUE;
}
BOOL ExtraOddDatumPointButtonClick(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CExtraAngleOddSetDlg *pExtraOddDlg=(CExtraAngleOddSetDlg*)pPropList->GetParent();
	if(pExtraOddDlg==NULL)
		return FALSE;
	CLDSPoint *pPoint=&pExtraOddDlg->GetCurExtraOddInfo()->extraDatumPoint;
	char sText[100]="";
	if(CLDSPoint::GetPropID("pos")==pItem->m_idProp)
	{
		CReportPointDlg pointDlg;
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "Datum Point Coordiante";
#else 
		pointDlg.m_sCaption = "��������";
#endif
		pointDlg.m_fPointX = pPoint->Position().x;
		pointDlg.m_fPointY = pPoint->Position().y;
		pointDlg.m_fPointZ = pPoint->Position().z;
		if(pointDlg.DoModal()==IDOK)
		{
			CXhChar100 sText("%f",pointDlg.m_fPointX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSPoint::GetPropID("pos.x"),CString(sText));
			sText.Printf("%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSPoint::GetPropID("pos.y"),CString(sText));
			sText.Printf("%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSPoint::GetPropID("pos.z"),CString(sText));
			pPoint->SetPosition(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ);
		}
	}
	else if(pPoint->datum_pos_style==1)
	{	//�Ǹ���㶨λ
		if(CLDSPoint::GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)
			pExtraOddDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);//��׼�Ǹ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CLDSPoint::GetPropID("des_para.RODEND.norm_offset")==pItem->m_idProp)
		{	//����ƫ����

			CString sThickStr;
			sThickStr=pPoint->des_para.RODEND.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			_snprintf(pPoint->des_para.RODEND.norm_offset,49,"%s",sThickStr);
			if(pPoint->GetPropValueStr(pItem->m_idProp,sText)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		}
#endif
	}
	else if(pPoint->datum_pos_style==2)
	{	//�˼��Ͻڵ㶨λ
		if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.hLinePart")==pItem->m_idProp)
			pExtraOddDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�
		else if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.hNode")==pItem->m_idProp)
			pExtraOddDlg->SelectObject(CLS_NODE,pItem->m_idProp);		//��׼�ڵ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.norm_offset")==pItem->m_idProp)
		{	//����ƫ����
			CString sThickStr;
			sThickStr=pPoint->des_para.RODNODE.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			_snprintf(pPoint->des_para.RODNODE.norm_offset,49,"%s",sThickStr);
			if(pPoint->GetPropValueStr(pItem->m_idProp,sText)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		}
#endif
	}
	else if(pPoint->datum_pos_style==3)
	{	//�Ǹ����߽���
		if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			pExtraOddDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�1
		else if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			pExtraOddDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�2
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.norm_offset")==pItem->m_idProp)
		{	//����ƫ����
			CString sThickStr;
			sThickStr=pPoint->des_para.AXIS_INTERS.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			_snprintf(pPoint->des_para.AXIS_INTERS.norm_offset,49,"%s",sThickStr);
			if(pPoint->GetPropValueStr(pItem->m_idProp,sText)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		}
#endif
	}
	else if(pPoint->datum_pos_style==7)
	{	//�ؽǸ���֫����ƫ�Ƶİ������
		if(CLDSPoint::GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)
			pExtraOddDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);//��׼�Ǹ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CLDSPoint::GetPropID("des_para.RODEND.norm_offset")==pItem->m_idProp)
		{	//����ƫ����
			
			CString sThickStr;
			sThickStr=pPoint->des_para.RODEND.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			_snprintf(pPoint->des_para.RODEND.norm_offset,49,"%s",sThickStr);
			if(pPoint->GetPropValueStr(pItem->m_idProp,sText)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		}
#endif
	}
	else if(pPoint->datum_pos_style==8)
	{	//��ڵ�ͬλ
		if(CLDSPoint::GetPropID("des_para.hNode")==pItem->m_idProp)
			pExtraOddDlg->SelectObject(CLS_NODE,pItem->m_idProp);	//��׼�ڵ�
	}
	else if(pPoint->datum_pos_style==9)
	{	//ָ�������ϵ����λ��
		if(CLDSPoint::GetPropID("des_para.hPart")==pItem->m_idProp)
			pExtraOddDlg->SelectObject(CLS_PART,pItem->m_idProp);	//��׼����
		else if(CLDSPoint::GetPropID("datum_pos")==pItem->m_idProp)
		{
			CReportPointDlg pointDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			pointDlg.m_sCaption = "Relative Coordinates";
#else 
			pointDlg.m_sCaption = "�������";
#endif
			pointDlg.m_fPointX = pPoint->datum_pos.x;
			pointDlg.m_fPointY = pPoint->datum_pos.y;
			pointDlg.m_fPointZ = pPoint->datum_pos.z;
			if(pointDlg.DoModal()==IDOK)
			{
				CXhChar100 sText("%f",pointDlg.m_fPointX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPoint::GetPropID("datum_pos.x"),CString(sText));
				sText.Printf("%f",pointDlg.m_fPointY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPoint::GetPropID("datum_pos.y"),CString(sText));
				sText.Printf("%f",pointDlg.m_fPointY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPoint::GetPropID("datum_pos.z"),CString(sText));
				pPoint->datum_pos.Set(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ);
			}
		}
	}
	pExtraOddDlg->UpdateExtraOddDisplayValue();
	return TRUE;
}
BOOL ModifyExtraOddDatumPointItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSPoint::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CExtraAngleOddSetDlg dialog
CExtraAngleOddSetDlg::CExtraAngleOddSetDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CExtraAngleOddSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExtraAngleOddSetDlg)
	m_iOddCalStyle = 0;
	m_fExtraOdd = 0.0;
	//}}AFX_DATA_INIT
	start0_end1=0;
}

void CExtraAngleOddSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtraAngleOddSetDlg)
	DDX_Control(pDX, IDC_TAB_HEADER, m_tabHeader);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_CBIndex(pDX, IDC_CMB_ODD_CAL_STYLE, m_iOddCalStyle);
	DDX_Text(pDX, IDC_E_EXTRA_ODD, m_fExtraOdd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExtraAngleOddSetDlg, CDialog)
	//{{AFX_MSG_MAP(CExtraAngleOddSetDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_HEADER, OnSelchangeTabHeader)
	ON_CBN_SELCHANGE(IDC_CMB_ODD_CAL_STYLE, OnSelchangeCmbOddCalStyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtraAngleOddSetDlg message handlers
double FilletZero(double val)
{
	if(val>-eps&&val<eps)
		return 0;
	else
		return val;
}
void CExtraAngleOddSetDlg::UpdateExtraOddDisplayValue()
{
	EXTRA_ODDINFO *pExtraInfo=GetCurExtraOddInfo();
	m_iOddCalStyle=pExtraInfo->style;
	((CEdit*)GetDlgItem(IDC_E_EXTRA_ODD))->SetReadOnly(m_iOddCalStyle!=0);
	if(pExtraInfo->style!=0)
	{
		if(start0_end1==0)
			m_fExtraOdd=m_pAngle->GetStartExtraOdd(m_propList.m_iPropGroup);
		else
			m_fExtraOdd=m_pAngle->GetEndExtraOdd(m_propList.m_iPropGroup);
	}
	else
		m_fExtraOdd=FilletZero(pExtraInfo->odd);
	UpdateData(FALSE);
}

BOOL CExtraAngleOddSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(start0_end1==0)
		m_pExtraOdd=&m_pAngle->extraStartOdd;
	else
		m_pExtraOdd=&m_pAngle->extraEndOdd;
	oldExtraOdd=*m_pExtraOdd;
	m_tabHeader.DeleteAllItems();
#ifdef AFX_TARG_ENU_ENGLISH
	m_tabHeader.InsertItem(0,"X Leg");
	m_tabHeader.InsertItem(1,"Y Leg");
	m_tabHeader.InsertItem(2,"Ridge Line");
#else 
	m_tabHeader.InsertItem(0,"X֫");
	m_tabHeader.InsertItem(1,"Y֫");
	m_tabHeader.InsertItem(2,"����");
#endif
	if(m_bInernalStart)	
	{
		m_tabHeader.SetCurSel(m_propList.m_iPropGroup);
		FinishSelectObjOper();	//���ѡ�����ĺ�������
	}
	else
		m_propList.m_iPropGroup = m_tabHeader.GetCurSel();
	DisplayPropertyList();	//�������FinishSelectObjOper֮��,��Ϊ�ú����Ĳ���������ܻ�Ӱ����ʾ�б�
	if(m_bInernalStart)	
	{	//�ָ�ѡ����
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		m_propList.SetFocus();	//��������ý���
		if(pItem)
		{
			m_propList.SetCurSel(pItem->m_iIndex);	//����ѡ����
			char sText[100]="";
			if(GetCurExtraOddInfo()->extraDatumPoint.GetPropValueStr(pItem->m_idProp,sText))
				m_propList.SetItemPropValue(pItem->m_idProp,sText);
		}
		Invalidate(FALSE);
	}
	UpdateExtraOddDisplayValue();
	return TRUE;  
}

EXTRA_ODDINFO* CExtraAngleOddSetDlg::GetCurExtraOddInfo()
{
	EXTRA_ANGLEODD *pExtraOdd=NULL;
	if(start0_end1==0)
		pExtraOdd=&m_pAngle->extraStartOdd;
	else
		pExtraOdd=&m_pAngle->extraEndOdd;
	EXTRA_ODDINFO *pExtraInfo=NULL;
	if(m_propList.m_iPropGroup==0)
		return &pExtraOdd->wing_x;
	else if(m_propList.m_iPropGroup==1)
		return &pExtraOdd->wing_y;
	else
		return &pExtraOdd->ber;
}

void CExtraAngleOddSetDlg::UpdateOddStyle(int iTabSel)
{
	UpdateData();
	EXTRA_ODDINFO *pExtraInfo=GetCurExtraOddInfo();
	pExtraInfo->style=m_iOddCalStyle;
	if(pExtraInfo->style==0)
		pExtraInfo->odd=m_fExtraOdd;
}
void CExtraAngleOddSetDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(m_pAngle->handle);
	CLDSPoint *pPoint=&GetCurExtraOddInfo()->extraDatumPoint;
	CParaPointLifeObj locklife;	//��ֹCLDSPoint::UpdatePos��CLDSLineAngle::GetDatumPosBer����ѭ������ wjh-2018.1.5
	if(GetEventPropId()==GetPropID("des_para.LINEPARTNODE.hLinePart"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
		if(pLinePart)
		{
			pPoint->des_para.RODNODE.hRod =pLinePart->handle;
			pPoint->UpdatePos(console.GetActiveModel());
		}
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(GetEventPropId()==GetPropID("des_para.RODEND.hRod"))
	{
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(m_hPickObj,CLS_LINEANGLE);
		if(pLineAngle)
		{
			pPoint->des_para.RODEND.hRod=pLineAngle->handle;
			pPoint->UpdatePos(console.GetActiveModel());
		}
	} 
#endif
	else if(GetEventPropId()==GetPropID("des_para.LINEPARTNODE.hNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
		if(pNode)
		{
			pPoint->des_para.RODNODE.hNode=pNode->handle;
			pPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(GetEventPropId()==GetPropID("des_para.AXIS_INTERS.hDatum1"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
		if(pLinePart)
		{
			pPoint->des_para.AXIS_INTERS.hDatum1=pLinePart->handle;
			pPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(GetEventPropId()==GetPropID("des_para.AXIS_INTERS.hDatum2"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
		if(pLinePart)
		{
			pPoint->des_para.AXIS_INTERS.hDatum2=pLinePart->handle;
			pPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(GetEventPropId()==GetPropID("des_para.hNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
		if(pNode)
		{
			pPoint->des_para.hNode=pNode->handle;
			pPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(GetEventPropId()==GetPropID("des_para.hPart"))
	{
		CLDSPart *pPart=console.FromPartHandle(m_hPickObj);
		if(pPart)
		{
			pPoint->des_para.hPart=pPart->handle;
			pPoint->UpdatePos(console.GetActiveModel());
		}
	}
}

void CExtraAngleOddSetDlg::OnSelchangeTabHeader(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateOddStyle(m_propList.m_iPropGroup);
	m_propList.m_iPropGroup = m_tabHeader.GetCurSel();
	EXTRA_ODDINFO *pExtraInfo=GetCurExtraOddInfo();
	m_iOddCalStyle=pExtraInfo->style;
	if(pExtraInfo->style!=0)
	{
		if(start0_end1==0)
			m_fExtraOdd=m_pAngle->GetStartExtraOdd(m_propList.m_iPropGroup);
		else
			m_fExtraOdd=m_pAngle->GetEndExtraOdd(m_propList.m_iPropGroup);
	}
	else
		m_fExtraOdd=pExtraInfo->odd;
	CEdit *pEdit=(CEdit*)GetDlgItem(IDC_E_EXTRA_ODD);
	pEdit->SetReadOnly(pExtraInfo->style!=0);
	UpdateData(FALSE);
	DisplayPropertyList();
	*pResult = 0;
}

void CExtraAngleOddSetDlg::OnSelchangeCmbOddCalStyle() 
{
	UpdateData();
	EXTRA_ODDINFO *pExtraInfo=GetCurExtraOddInfo();
	pExtraInfo->style=m_iOddCalStyle;
	CEdit *pEdit=(CEdit*)GetDlgItem(IDC_E_EXTRA_ODD);
	pEdit->SetReadOnly(pExtraInfo->style!=0);
	DisplayPropertyList();
}
int CExtraAngleOddSetDlg::GetPropStatus(long id)					//�õ�����״̬
{
	return CLDSPoint::GetPropStatus(id);
}
BOOL CExtraAngleOddSetDlg::SetPropStatus(long id,BOOL bHide)		//��������״̬
{
	return CLDSPoint::SetPropStatus(id,bHide);
}
long CExtraAngleOddSetDlg::GetPropID(char* propStr)			//�õ�����ID
{
	return CLDSPoint::GetPropID(propStr);
}

void CExtraAngleOddSetDlg::UpdateDatumPointStyle(CPropTreeItem *pRootItem, CLDSPoint *pPoint,BOOL bUpdate)
{
	CXhChar100 valueStr;
	CItemInfo* lpInfo=NULL;
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pSonPropItem=NULL;
	pPoint=&GetCurExtraOddInfo()->extraDatumPoint;
	m_propList.DeleteItemByPropId(GetPropID("des_para.hNode"));
	//ָ�������ϵ��������
	m_propList.DeleteItemByPropId(GetPropID("des_para.hPart"));
	m_propList.DeleteItemByPropId(GetPropID("datum_pos"));
	m_propList.DeleteItemByPropId(GetPropID("datum_pos.x"));
	m_propList.DeleteItemByPropId(GetPropID("datum_pos.y"));
	m_propList.DeleteItemByPropId(GetPropID("datum_pos.z"));
	//���߽���
	m_propList.DeleteItemByPropId(GetPropID("des_para.AXIS_INTERS.hDatum1"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.AXIS_INTERS.hDatum2"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.AXIS_INTERS.wing_offset_style1"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.AXIS_INTERS.wing_offset_style2"));//0.g;1.g1;2.g2;3.g3;4.�Զ���
	m_propList.DeleteItemByPropId(GetPropID("des_para.AXIS_INTERS.wing_offset_dist1"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.AXIS_INTERS.wing_offset_dist2"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.AXIS_INTERS.near_vector"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.AXIS_INTERS.near_vector[0]"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.AXIS_INTERS.near_vector[1]"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.AXIS_INTERS.near_vector[2]"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.AXIS_INTERS.norm_offset"));	//����ƫ����;
	//�Ǹ����
	m_propList.DeleteItemByPropId(GetPropID("des_para.RODEND.hRod"));				//�Ǹ־��
	m_propList.DeleteItemByPropId(GetPropID("des_para.RODEND.direction"));			//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	m_propList.DeleteItemByPropId(GetPropID("des_para.RODEND.offset_wing"));		//0.X֫;1.Y֫
	m_propList.DeleteItemByPropId(GetPropID("des_para.RODEND.wing_offset_style"));	//0.g;1.g1;2.g2;3.g3;4.�Զ���
	m_propList.DeleteItemByPropId(GetPropID("des_para.RODEND.wing_offset_dist"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.RODEND.len_offset_dist"));	//֫��ƫ��ֵ;����ƫ��ֵ;
	m_propList.DeleteItemByPropId(GetPropID("des_para.RODEND.bIncOddEffect"));		//����ƫ��ֵ��������ͷӰ��
	m_propList.DeleteItemByPropId(GetPropID("des_para.RODEND.norm_offset"));		//����ƫ����
	//�˼��Ͻڵ�Ϊ��׼
	m_propList.DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.hLinePart"));	//�Ǹ־��
	m_propList.DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.hNode"));		//��׼�ڵ���
	m_propList.DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.bFaceOffset"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.direction"));	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	m_propList.DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.offset_wing"));	//0.X֫;1.Y֫
	m_propList.DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.wing_offset_style"));//0.g;1.g1;2.g2;3.g3;4.�Զ���
	m_propList.DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.wing_offset_dist"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.len_offset_dist"));	//֫��ƫ��ֵ;����ƫ��ֵ;
	m_propList.DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.norm_offset"));		//����ƫ����;
	m_propList.DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.face_norm_offset"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]"));
	//��׼�����������겻���
	m_propList.DeleteItemByPropId(GetPropID("des_para.DATUMLINE.hDatumLine"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2"));
	//��׼�����������겻���
	m_propList.DeleteItemByPropId(GetPropID("des_para.DATUMPLANE.hDatumPlane"));
	m_propList.DeleteItemByPropId(GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2"));

	DWORD dwPropGroup=GetSingleWord(m_propList.m_iPropGroup+1);
	if(pPoint->datum_pos_style==1)	//�Ǹ����
	{	//��׼�Ǹ־��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Angle Handle";
		lpInfo->m_strPropHelp="Datum angle's handle��";
#else 
		lpInfo->m_strPropName="��׼�Ǹ־��";
		lpInfo->m_strPropHelp="��׼�Ǹ־����";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.hRod");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
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
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.offset_wing");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="g|g1|g2|g3|user specify|";
		lpInfo->m_strPropName="Leg Offset Style";
		lpInfo->m_strPropHelp="The offset stadia or user specified offset.";
#else 
		lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
		lpInfo->m_strPropName="֫��ƫ������";
		lpInfo->m_strPropHelp="ƫ������Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.wing_offset_style");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		//ƫ�ƾ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Leg Direction Offset";
		lpInfo->m_strPropHelp="The offset of leg direction ��";
#else 
		lpInfo->m_strPropName="֫��ƫ��ֵ";
		lpInfo->m_strPropHelp="֫��ƫ��ֵ��";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.wing_offset_dist");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->SetReadOnly(pPoint->des_para.RODEND.wing_offset_style!=4);
		pPropItem->m_dwPropGroup=dwPropGroup;
		//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Vertical Offset Direction";
		lpInfo->m_strPropHelp="Vertical Offset Direction,offset along the direction of datum angle start to the end or along the opposite direction��";
#else 
		lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
		lpInfo->m_strPropName="����ƫ�Ʒ���";
		lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.direction");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		//����ƫ��ֵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Vertical Direction Offset";
		lpInfo->m_strPropHelp="The offset of vertical direction";
#else 
		lpInfo->m_strPropName="����ƫ��ֵ";
		lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.len_offset_dist");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		//����ƫ��ֵ��������ͷӰ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName="Odd Length Influence";
		lpInfo->m_strPropHelp="Whether or not included in odd length influence of vertical offset��";
#else 
		lpInfo->m_cmbItems="��|��";
		lpInfo->m_strPropName="����ƫ�Ƽ�������ͷӰ��";
		lpInfo->m_strPropHelp="����ƫ�Ƽ�������ͷӰ�졣";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.bIncOddEffect");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		//����ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="The offset of normal";
#else 
		lpInfo->m_strPropName="����ƫ����";
		lpInfo->m_strPropHelp="����ƫ����";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.norm_offset");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
	}
	else if(pPoint->datum_pos_style==2)	//�˼��Ͻڵ㶨λ
	{
		//��׼�˼����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Rod  Handle";
		lpInfo->m_strPropHelp="Datum rod's  handle��";
#else 
		lpInfo->m_strPropName="��׼�˼����";
		lpInfo->m_strPropHelp="��׼�˼������";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.hLinePart");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		//��׼�ڵ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Node Handle";
		lpInfo->m_strPropHelp="Datum node's handle��";
#else 
		lpInfo->m_strPropName="��׼�ڵ���";
		lpInfo->m_strPropHelp="��׼�ڵ�����";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.hNode");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
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
			pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.offset_wing");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
			//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|user specify|";
			lpInfo->m_strPropName="Leg Offset Style";
			lpInfo->m_strPropHelp="The offset stadia or user specified offset.";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������";
			lpInfo->m_strPropHelp="ƫ������Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.wing_offset_style");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Direction Offset";
			lpInfo->m_strPropHelp="The offset of leg direction ��";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ";
			lpInfo->m_strPropHelp="֫��ƫ��ֵ��";
#endif
			pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.wing_offset_dist");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(pPoint->des_para.RODNODE.wing_offset_style!=4);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Vertical Offset Direction";
		lpInfo->m_strPropHelp="Vertical Offset Direction,offset along the direction of datum angle start to the end or along the opposite direction��";
#else 
		lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
		lpInfo->m_strPropName="����ƫ�Ʒ���";
		lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.direction");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		//����ƫ��ֵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Vertical Direction Offset";
		lpInfo->m_strPropHelp="The offset of vertical direction";
#else 
		lpInfo->m_strPropName="����ƫ��ֵ";
		lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.len_offset_dist");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		//����ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="The offset of normal";
#else 
		lpInfo->m_strPropName="����ƫ����";
		lpInfo->m_strPropHelp="����ƫ����";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.norm_offset");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
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
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.bFaceOffset");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
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
			pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.face_norm_offset");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
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
			pSonPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]");
			if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
			pSonPropItem->m_dwPropGroup=dwPropGroup;
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
			pSonPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]");
			if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
			pSonPropItem->m_dwPropGroup=dwPropGroup;
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
			pSonPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]");
			if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
			pSonPropItem->m_dwPropGroup=dwPropGroup;
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
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.hDatum1");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		CLDSLinePart *pLinePart1=(CLDSLinePart*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEPART);
		if(pLinePart1&&pLinePart1->IsAngle())
		{	//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|user specify|";
			lpInfo->m_strPropName="Leg Offset Style 1";
			lpInfo->m_strPropHelp="The offset type of datum angle 1:Scheduled stadia or user specify offset distance��";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������1";
			lpInfo->m_strPropHelp="��׼�Ǹ�1ƫ������:Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.wing_offset_style1");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Direction Offset 1";
			lpInfo->m_strPropHelp="Datum Angle1 The offset of leg direction ��";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ1";
			lpInfo->m_strPropHelp="��׼�Ǹ�1֫��ƫ��ֵ��";
#endif
			pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.wing_offset_dist1");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(pPoint->des_para.AXIS_INTERS.wing_offset_style1!=4);
			pPropItem->m_dwPropGroup=dwPropGroup;
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
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.hDatum2");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		CLDSLinePart *pLinePart2=(CLDSLinePart*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEPART);
		if(pLinePart2&&pLinePart2->IsAngle())
		{	//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|user specify|";
			lpInfo->m_strPropName="Leg Offset Style 2";
			lpInfo->m_strPropHelp="Datum angle 2 offset style:reserve stadia or user specified offset��";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������2";
			lpInfo->m_strPropHelp="��׼�Ǹ�2ƫ������:Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.wing_offset_style2");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Direction Offset 2";
			lpInfo->m_strPropHelp="The offset of datum angle2's leg direction��";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ2";
			lpInfo->m_strPropHelp="��׼�Ǹ�2֫��ƫ��ֵ��";
#endif
			pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.wing_offset_dist2");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(pPoint->des_para.AXIS_INTERS.wing_offset_style2!=4);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		//����ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="The offset of normal";
#else 
		lpInfo->m_strPropName="����ƫ����";
		lpInfo->m_strPropHelp="����ƫ����";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.norm_offset");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
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
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.near_vector");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_bHideChildren=TRUE;
		if(GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=dwPropGroup;
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
		pSonPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.near_vector[0]");
		if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->SetReadOnly(vectorType!=0);
		pSonPropItem->m_dwPropGroup=dwPropGroup;
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The Y component of near normal";
#else 
		lpInfo->m_strPropHelp="���Ʒ���Y�������";
#endif
		pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.near_vector[1]");
		if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->SetReadOnly(vectorType!=0);
		pSonPropItem->m_dwPropGroup=dwPropGroup;
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The Z component of near normal";
#else 
		lpInfo->m_strPropHelp="���Ʒ���Z�������";
#endif
		pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.near_vector[2]");
		if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->SetReadOnly(vectorType!=0);
		pSonPropItem->m_dwPropGroup=dwPropGroup;
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
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.hDatum1");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
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
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.hDatum2");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
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
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.DATUMLINE.hDatumLine");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ָ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.X axis |1.Y axis |2.Z axis";
		lpInfo->m_strPropName="Specify Axial";
		lpInfo->m_strPropHelp="Specify Axial";
#else 
		lpInfo->m_cmbItems="0.X��|1.Y��|2.Z��";
		lpInfo->m_strPropName="ָ������";
		lpInfo->m_strPropHelp="ָ������";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pPoint->datum_pos_style==6)	//��׼�����������겻���
	{
		//��׼����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Plane Handle";
		lpInfo->m_strPropHelp="Datum plane's handle";
#else 
		lpInfo->m_strPropName="��׼����";
		lpInfo->m_strPropHelp="��׼����";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.DATUMPLANE.hDatumPlane");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ָ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.X axis |1.Y axis |2.Z axis";
		lpInfo->m_strPropName="Specify Axial";
		lpInfo->m_strPropHelp="Specify Axial";
#else 
		lpInfo->m_cmbItems="0.X��|1.Y��|2.Z��";
		lpInfo->m_strPropName="ָ������";
		lpInfo->m_strPropHelp="ָ������";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pPoint->datum_pos_style==7)	//�ؽǸ���֫����ƫ�Ƶİ������
	{
		//��׼�Ǹ־��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Angle Handle";
		lpInfo->m_strPropHelp="Datum angle's handle��";
#else 
		lpInfo->m_strPropName="��׼�Ǹ־��";
		lpInfo->m_strPropHelp="��׼�Ǹ־����";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.hRod");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Vertical Offset Direction";
		lpInfo->m_strPropHelp="Vertical Offset Direction,offset along the direction of datum angle start to the end or along the opposite direction��";
#else 
		lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
		lpInfo->m_strPropName="����ƫ�Ʒ���";
		lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.direction");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//����ƫ��ֵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Vertical Direction Offset";
		lpInfo->m_strPropHelp="The offset of vertical direction";
#else 
		lpInfo->m_strPropName="����ƫ��ֵ";
		lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.len_offset_dist");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//����ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="The offset of normal";
#else 
		lpInfo->m_strPropName="����ƫ����";
		lpInfo->m_strPropHelp="����ƫ����";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.norm_offset");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
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
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.hNode");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
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
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.hPart");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
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
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("datum_pos");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
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
		pSonPropItem->m_idProp = GetPropID("datum_pos.x");
		if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
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
		pSonPropItem->m_idProp = GetPropID("datum_pos.y");
		if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
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
		pSonPropItem->m_idProp = GetPropID("datum_pos.z");
		if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pPoint->datum_pos_style==10)
	{	//λ��
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Fill Plate Position";
#else 
		lpInfo->m_strPropName = "���λ�ò���";
#endif
		pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pGroupItem->m_idProp = GetPropID("Position");
		pGroupItem->m_bHideChildren=FALSE;
		//��ϽǸ־��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Rod  Handle";
		lpInfo->m_strPropHelp="The handle of datum rod;consider the rod's odd length information when the datum node as datum part's endpoint.";
#else 
		lpInfo->m_strPropName="��׼�˼����";
		lpInfo->m_strPropHelp="��׼�˼��������׼�ڵ�Ϊ�˼���׼�˼��˵�ʱ�ῼ�Ǹ˼�������ͷ��Ϣ��";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.SCALE_NODE.hLinePart");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ʼ�˻�׼�ڵ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Start Node Handle";
		lpInfo->m_strPropHelp="The handle of start node;consider the rod's odd length information when the datum node as datum part's endpoint.";
#else 
		lpInfo->m_strPropName="ʼ�˻�׼�ڵ���";
		lpInfo->m_strPropHelp="ʼ�˻�׼�ڵ�������׼�ڵ�Ϊ�˼���׼�˼��˵�ʱ�ῼ�Ǹ˼�������ͷ��Ϣ��";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.SCALE_NODE.hStartNode");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ʼ��ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Start Offset";
		lpInfo->m_strPropHelp="The offset of head node along the direction of start node to end node.";
#else 
		lpInfo->m_strPropName="ʼ��ƫ����";
		lpInfo->m_strPropHelp="ʼ�˽ڵ�����ʼ�˽ڵ�->�ն˽ڵ㷽���ƫ������";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.SCALE_NODE.start_offset_dist");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//�ն˻�׼�ڵ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="End Node Handle";
		lpInfo->m_strPropHelp="The handle of end node;consider the rod's odd length information when the datum node as datum part's endpoint.";
#else 
		lpInfo->m_strPropName="�ն˻�׼�ڵ���";
		lpInfo->m_strPropHelp="�ն˻�׼�ڵ�������׼�ڵ�Ϊ�˼���׼�˼��˵�ʱ�ῼ�Ǹ˼�������ͷ��Ϣ��";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.SCALE_NODE.hEndNode");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//�ն�ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="End Offset";
		lpInfo->m_strPropHelp="The offset of head node along the direction of end node to start node.";
#else 
		lpInfo->m_strPropName="�ն�ƫ����";
		lpInfo->m_strPropHelp="�ն˽ڵ������ն˽ڵ�->ʼ�˽ڵ㷽���ƫ������";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.SCALE_NODE.end_offset_dist");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ƫ�Ʊ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Offset Scale";
		lpInfo->m_strPropHelp="Calculate data from start node's offset position.";
#else 
		lpInfo->m_strPropName="ƫ�Ʊ���";
		lpInfo->m_strPropHelp="��ʼ�˽ڵ㿼��ʼ��ƫ�����Ժ��λ�ÿ�ʼ���㡣";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.SCALE_NODE.offset_scale");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ƫ�ƾ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Offset Distance";
		lpInfo->m_strPropHelp="Calculated on offset ratio first and then get distance after offset.";
#else 
		lpInfo->m_strPropName="ƫ�ƾ���";
		lpInfo->m_strPropHelp="��ƫ�Ʊ��������Ժ��ٽ���ƫ�Ƶľ��롣";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("des_para.SCALE_NODE.offset_dist");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
}

void CExtraAngleOddSetDlg::DisplayPropertyList()
{
	const int GROUP_WINGX=1,GROUP_WINGY=2,GROUP_BER=3;
	m_propList.CleanTree();
	m_propList.SetModifyValueFunc(ModifyExtraOddDatumPointProperty);
	m_propList.SetButtonClickFunc(ExtraOddDatumPointButtonClick);
	m_propList.SetModifyStatusFunc(ModifyExtraOddDatumPointItemStatus);
	CItemInfo* lpInfo=NULL;
	CXhChar100 valueStr;
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pSonPropItem=NULL,*pRootItem=m_propList.GetRootItem();
	CLDSPoint *pPoint=NULL;
	EXTRA_ODDINFO *pExtraInfo=GetCurExtraOddInfo();
	pPoint=&pExtraInfo->extraDatumPoint;
	if(pExtraInfo->style!=1)
	{
		m_propList.Redraw();
		return;
	}
	DWORD dwPropGroup=GetSingleWord(m_propList.m_iPropGroup+1);
	//ָ������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Datum Point Coordiante";
	lpInfo->m_strPropHelp="Datum point's coordiante";
#else 
	lpInfo->m_strPropName="��������";
	lpInfo->m_strPropHelp="��������";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = GetPropID("pos");
	pGroupItem->m_dwPropGroup=dwPropGroup;
	pGroupItem->m_bHideChildren=FALSE;
	if(GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup=dwPropGroup;
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp="Datum point's x coordinate";
#else 
	lpInfo->m_strPropHelp="��׼��X����";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = GetPropID("pos.x");
	if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	pPropItem->SetReadOnly();
	pPropItem->m_dwPropGroup=dwPropGroup;
	CPropTreeItem *pXPosPropItem=pPropItem;
	//Y
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp="Datum point's y coordinate";
#else 
	lpInfo->m_strPropHelp="��׼��Y����";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = GetPropID("pos.y");
	if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	pPropItem->SetReadOnly();
	pPropItem->m_dwPropGroup=dwPropGroup;
	CPropTreeItem *pYPosPropItem=pPropItem;
	//Z
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp="Datum point's z coordinate";
#else 
	lpInfo->m_strPropHelp="��׼��Z����";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = GetPropID("pos.z");
	if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	pPropItem->SetReadOnly();
	pPropItem->m_dwPropGroup=dwPropGroup;
	CPropTreeItem *pZPosPropItem=pPropItem;

	//���㶨λ��ʽ
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Datum Point Locate Type";
	lpInfo->m_strPropHelp="Datum point's locate type";
	lpInfo->m_cmbItems="0.Manual Input|1.Angle Ridge Vertex Locating Point|2.Rod Node Locating Point|3.Angle Center Line Intersection Point|4.Baseline Space Intersection Point|5.Specified Component Point Of Datum Line|6.Specified Component Point Of Datum Plane|7.Outer Angle's Ridge Vertex Along Angle's Normal Offset|8.With The Node In The Same Position|9.Datum Rod Relative Coordinate";
#else 
	lpInfo->m_strPropName="���㶨λ��ʽ";
	lpInfo->m_strPropHelp="���㶨λ��ʽ";
	lpInfo->m_cmbItems="0.�ֶ�����|1.�Ǹ���㶨λ��|2.�˼��ڵ㶨λ��|3.�Ǹ����߽���|4.��׼�߿ռ佻��|5.��׼����ָ�����������|6.��׼����ָ�������|7.�ؽǸ���֫����ƫ�Ƶİ������|8.��ڵ�ͬλ|9.��׼�����ϵ��������";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = GetPropID("datum_pos_style");
	if(pPoint->GetPropValueStr(pGroupItem->m_idProp,valueStr)>0)
		pGroupItem->m_lpNodeInfo->m_strPropValue=valueStr;
	pGroupItem->m_dwPropGroup=dwPropGroup;
	pGroupItem->m_bHideChildren=FALSE;
	if(GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup=dwPropGroup;
	if(pPoint->datum_pos_style==0)		//ֱ��ָ������
	{	//��ֱ�ӱ༭�޸�
		pXPosPropItem->SetReadOnly(FALSE);
		pYPosPropItem->SetReadOnly(FALSE);
		pZPosPropItem->SetReadOnly(FALSE);
	}
	else
		UpdateDatumPointStyle(pGroupItem,pPoint,FALSE);
	m_propList.Redraw();
}

void CExtraAngleOddSetDlg::OnOK() 
{
	UpdateOddStyle(m_propList.m_iPropGroup);
	CDialog::OnOK();
}

void CExtraAngleOddSetDlg::OnCancel() 
{	//ȡ��ʱ�ָ�ԭ�й̶�����ͷ��Ϣ
	if(start0_end1==0)
		m_pAngle->extraStartOdd=oldExtraOdd;
	else
		m_pAngle->extraEndOdd=oldExtraOdd;
	CDialog::OnCancel();
}
#endif
