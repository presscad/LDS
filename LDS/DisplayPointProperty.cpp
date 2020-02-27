//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "DisplayPointProperty.h"
#include "CallBackDialog.h"
#include "ReportPointDlg.h"
#include "ScrTools.h"
#include "env_def.h"
#include "LmaDlg.h"

CDisplayPointProperty::CDisplayPointProperty(void)
{
}

CDisplayPointProperty::~CDisplayPointProperty(void)
{
}
//<DEVELOP_PROCESS_MARK nameId="CDisplayPointProperty.CPP">
BOOL CDisplayPointProperty::DatumPointButtonClick(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CCallBackDialog *pCallBackDlg = (CCallBackDialog *)pPropList->GetParent();
	if(pCallBackDlg==NULL)
		return FALSE;

	CLDSPoint *pPoint=(CLDSPoint*)pPropList->m_pObj;
	char sText[100]="";
	if(CLDSPoint::GetPropID("pos")==pItem->m_idProp)
	{
		CReportPointDlg pointDlg;
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "Datum Point Coordinate";
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
			pCallBackDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);//��׼�Ǹ�
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
			pCallBackDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�
		else if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.hNode")==pItem->m_idProp)
			pCallBackDlg->SelectObject(CLS_NODE,pItem->m_idProp);		//��׼�ڵ�
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
			pCallBackDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�1
		else if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			pCallBackDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�2
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
			pCallBackDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);//��׼�Ǹ�
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
			pCallBackDlg->SelectObject(CLS_NODE,pItem->m_idProp);	//��׼�ڵ�
	}
	else if(pPoint->datum_pos_style==9)
	{	//ָ�������ϵ����λ��
		if(CLDSPoint::GetPropID("des_para.hPart")==pItem->m_idProp)
			pCallBackDlg->SelectObject(CLS_PART,pItem->m_idProp);	//��׼����
		else if(CLDSPoint::GetPropID("datum_pos")==pItem->m_idProp)
		{
			CReportPointDlg pointDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			pointDlg.m_sCaption = "Relative Coordinate";
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
	else if(pPoint->datum_pos_style==10)
	{

	}
	else if(pPoint->datum_pos_style==11)
	{
		if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.hLineTube")==pItem->m_idProp)
			pCallBackDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);//��׼�ֹ�
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec")==pItem->m_idProp)
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Axis Vector";
#else 
			vecDlg.m_sCaption = "���߷���";
#endif
			vecDlg.m_fVectorX = pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[0];
			vecDlg.m_fVectorY = pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[1];
			vecDlg.m_fVectorZ = pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[2];
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.x"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.y"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.z"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				char sText[MAX_TEMP_CHAR_100+1];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.x"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.y"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.z"),CString(sText));
				pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[0] = vecDlg.m_fVectorX;
				pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[1] = vecDlg.m_fVectorY;
				pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[2] = vecDlg.m_fVectorZ;
			}
		}
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_center")==pItem->m_idProp)
		{
			CReportVectorDlg vecDlg;
			CLDSLineTube *pLineTube = (CLDSLineTube *)console.FromPartHandle(pPoint->des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE);
#ifdef AFX_TARG_ENU_ENGLISH
			if(pLineTube)
				vecDlg.m_sCaption = "Section Pick Point";
			else 
				vecDlg.m_sCaption = "Section Center";
#else 
			if(pLineTube)
				vecDlg.m_sCaption = "����ʰȡ��";
			else 
				vecDlg.m_sCaption = "����Բ��";
#endif
			vecDlg.m_fVectorX = pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[0];
			vecDlg.m_fVectorY = pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[1];
			vecDlg.m_fVectorZ = pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[2];
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_center.x"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_center.y"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_center.z"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				char sText[MAX_TEMP_CHAR_100+1];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_center.x"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_center.y"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_center.z"),CString(sText));
				pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[0] = vecDlg.m_fVectorX;
				pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[1] = vecDlg.m_fVectorY;
				pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[2] = vecDlg.m_fVectorZ;
			}
		}
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm")==pItem->m_idProp)
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Section Normal";
#else 
			vecDlg.m_sCaption = "���淨��";
#endif
			vecDlg.m_fVectorX = pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[0];
			vecDlg.m_fVectorY = pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[1];
			vecDlg.m_fVectorZ = pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[2];
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm.x"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm.y"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm.z"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				char sText[MAX_TEMP_CHAR_100+1];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm.x"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm.y"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm.z"),CString(sText));
				pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[0] = vecDlg.m_fVectorX;
				pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[1] = vecDlg.m_fVectorY;
				pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[2] = vecDlg.m_fVectorZ;
			}
		}
	}
	else
		return FALSE;
	return TRUE;
}

BOOL CDisplayPointProperty::ModifyDatumPointPropValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CLDSPoint *pPoint=(CLDSPoint*)pPropList->m_pObj;
	if(pPoint==NULL)
		return FALSE;
	char value_str[100]="";
	if (pItem->m_idProp==CLDSPoint::GetPropID("pos.x"))
		pPoint->SetPositionX(atof(valueStr));
	else if (pItem->m_idProp==CLDSPoint::GetPropID("pos.y"))
		pPoint->SetPositionY(atof(valueStr));
	else if (pItem->m_idProp==CLDSPoint::GetPropID("pos.z"))
		pPoint->SetPositionZ(atof(valueStr));
	else if(CLDSPoint::GetPropID("datum_pos_style")==pItem->m_idProp)
	{
		//pPoint->datum_pos_style=valueStr[0]-'0';
		sscanf(valueStr,"%d",&pPoint->datum_pos_style);
		CPropTreeItem *pDatumPointItem = pPropList->FindItemByPropId(CLDSPoint::GetPropID("pos"),NULL);
		if(pDatumPointItem)
		{
			if(pPoint->datum_pos_style!=11)
			{
				if(pDatumPointItem->m_bHideChildren)
					pPropList->Expand(pDatumPointItem,pDatumPointItem->m_iIndex);
			}
			else if(!pDatumPointItem->m_bHideChildren)
				pPropList->Collapse(pDatumPointItem);
		}	
		UpdateDatumPointPropItem(pPropList,pItem,pPoint,TRUE);
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
			long idProp=CLDSPoint::GetPropID("des_para.RODEND.wing_offset_dist");
			if(pPoint->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.RODEND.wing_offset_style!=4);
		}
#endif
		else if(CLDSPoint::GetPropID("des_para.RODEND.wing_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODEND.wing_offset_dist=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.RODEND.len_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.RODEND.bIncOddEffect")==pItem->m_idProp)		
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
		if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.hLinePart")==pItem->m_idProp)	//�Ǹ־��
			sscanf(valueStr,"%X",&(pPoint->des_para.RODNODE.hRod ));
		else if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.hNode")==pItem->m_idProp)	//��׼�ڵ���
			sscanf(valueStr,"%X",&(pPoint->des_para.RODNODE.hNode));
		else if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.direction")==pItem->m_idProp)	
			pPoint->des_para.RODNODE.direction=valueStr[0]-'0';
		else if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.offset_wing")==pItem->m_idProp)	
			pPoint->des_para.RODNODE.offset_wing=valueStr[0]-'0';
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.wing_offset_style")==pItem->m_idProp)
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
			long idProp=CLDSPoint::GetPropID("des_para.LINEPARTNODE.wing_offset_dist");
			if(pPoint->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.RODNODE.wing_offset_style!=4);
		}	
#endif
		else if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.wing_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODNODE.wing_offset_dist=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.len_offset_dist")==pItem->m_idProp)	
			pPoint->des_para.RODNODE.len_offset_dist=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.bFaceOffset")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
#endif
				pPoint->des_para.RODNODE.bFaceOffset=TRUE;
			else 
				pPoint->des_para.RODNODE.bFaceOffset=FALSE;
			pPropList->DeleteItemByPropId(CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset"));
			pPropList->DeleteItemByPropId(CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]"));
			pPropList->DeleteItemByPropId(CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]"));
			pPropList->DeleteItemByPropId(CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]"));
			if(pPoint->des_para.RODNODE.bFaceOffset)
			{	//ƽ���淨��
				CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL,*pRootItem=pPropList->GetRootItem();
				//
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
				lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Offset Plane Normal";
				lpInfo->m_strPropHelp="The offset plane's normal";
#else 
				lpInfo->m_strPropName="ƽ���淨��";
				lpInfo->m_strPropHelp="ƽ���淨��";
#endif
				pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset");
				if(pPoint->GetPropValueStr(pPropItem->m_idProp,value_str)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				int vectorType=CStdVector::GetVectorType(pPoint->des_para.RODNODE.vPlaneNormal);
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The x coordinate of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��X�������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,TRUE);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The y coordinate of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��Y�������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,1,TRUE);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The z coordinate of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��Z�������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,2,TRUE);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
			}
		}
		else if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset")==pItem->m_idProp)
		{
			f3dPoint norm=CStdVector::GetVector(valueStr[0]-'0');
			pPoint->des_para.RODNODE.vPlaneNormal=norm;
			long idProp=CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.RODNODE.vPlaneNormal.x);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.RODNODE.vPlaneNormal.y);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.RODNODE.vPlaneNormal.z);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
		}
		else if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]")==pItem->m_idProp)
			pPoint->des_para.RODNODE.vPlaneNormal.x=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]")==pItem->m_idProp)
			pPoint->des_para.RODNODE.vPlaneNormal.y=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]")==pItem->m_idProp)
			pPoint->des_para.RODNODE.vPlaneNormal.z=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==3)	//�˼����߽���
	{
		if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum1));
		else if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum2));
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.wing_offset_style1")==pItem->m_idProp)
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
			long idProp=CLDSPoint::GetPropID("des_para.AXIS_INTERS.wing_offset_dist1");
			if(pPoint->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.AXIS_INTERS.wing_offset_style1!=4);
		}
		else if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.wing_offset_style2")==pItem->m_idProp)
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
			long idProp=CLDSPoint::GetPropID("des_para.AXIS_INTERS.wing_offset_dist2");
			if(pPoint->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.AXIS_INTERS.wing_offset_style2!=4);
		}
#endif
		else if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.wing_offset_dist1")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.wing_offset_dist1=atol(valueStr);
		else if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.wing_offset_dist2")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.wing_offset_dist2=atol(valueStr);
		else if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.near_vector")==pItem->m_idProp)
		{
			f3dPoint vec=CStdVector::GetVector(valueStr[0]-'0');
			pPoint->des_para.AXIS_INTERS.near_vector[0]=vec.x;
			pPoint->des_para.AXIS_INTERS.near_vector[1]=vec.y;
			pPoint->des_para.AXIS_INTERS.near_vector[2]=vec.z;
			long idProp=CLDSPoint::GetPropID("des_para.AXIS_INTERS.near_vector[0]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.AXIS_INTERS.near_vector[0]);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CLDSPoint::GetPropID("des_para.AXIS_INTERS.near_vector[1]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.AXIS_INTERS.near_vector[1]);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CLDSPoint::GetPropID("des_para.AXIS_INTERS.near_vector[2]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.AXIS_INTERS.near_vector[2]);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
		}
		else if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.near_vector[0]")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.near_vector[0]=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.near_vector[1]")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.near_vector[1]=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.near_vector[2]")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.near_vector[2]=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==4)	//��׼�߿ռ佻��
	{
		if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum1));
		else if(CLDSPoint::GetPropID("des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum2));
	}
	else if(pPoint->datum_pos_style==5)	//��׼����ָ�����������
	{
		if(CLDSPoint::GetPropID("des_para.DATUMLINE.hDatumLine")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.DATUMLINE.hDatumLine));
		else if(CLDSPoint::GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==pItem->m_idProp)
			pPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=valueStr[0]-'0';
	}
	else if(pPoint->datum_pos_style==6)	//��׼����ָ�������
	{
		if(CLDSPoint::GetPropID("des_para.DATUMPLANE.hDatumPlane")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.DATUMPLANE.hDatumPlane));
		else if(CLDSPoint::GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==pItem->m_idProp)
			pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=valueStr[0]-'0';
	}
	else if(pPoint->datum_pos_style==7)	//�ؽǸ���֫����ƫ�Ƶİ������
	{
		if(CLDSPoint::GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)	//�Ǹ־��
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.RODEND.hRod));
		else if(CLDSPoint::GetPropID("des_para.RODEND.direction")==pItem->m_idProp)	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
			pPoint->des_para.RODEND.direction=valueStr[0]-'0';
		else if(CLDSPoint::GetPropID("des_para.RODEND.len_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==8)	//��ڵ�ͬλ
	{
		if(CLDSPoint::GetPropID("des_para.hNode")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.hNode));
	}
	else if(pPoint->datum_pos_style==9)	//ָ�������ϵ��������
	{
		if(CLDSPoint::GetPropID("des_para.hPart")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.hPart));
		else if(CLDSPoint::GetPropID("datum_pos.x")==pItem->m_idProp)
			pPoint->datum_pos.x=atof(valueStr);
		else if(CLDSPoint::GetPropID("datum_pos.y")==pItem->m_idProp)
			pPoint->datum_pos.y=atof(valueStr);
		else if(CLDSPoint::GetPropID("datum_pos.z")==pItem->m_idProp)
			pPoint->datum_pos.z=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==10)
	{
		if(CLDSPoint::GetPropID("des_para.SCALE_NODE.hLinePart")==pItem->m_idProp)			//��׼�˼����
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.SCALE_NODE.hLinePart));
		else if(CLDSPoint::GetPropID("des_para.SCALE_NODE.hStartNode")==pItem->m_idProp)	//ʼ�˽ڵ���
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.SCALE_NODE.hStartNode));
		else if(CLDSPoint::GetPropID("des_para.SCALE_NODE.hEndNode")==pItem->m_idProp)		//�ն˽ڵ���
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.SCALE_NODE.hEndNode));
		else if(CLDSPoint::GetPropID("des_para.SCALE_NODE.start_offset_dist")==pItem->m_idProp)//ʼ��ƫ����
			pPoint->des_para.SCALE_NODE.start_offset_dist=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.SCALE_NODE.end_offset_dist")==pItem->m_idProp)	//�ն�ƫ����
			pPoint->des_para.SCALE_NODE.end_offset_dist=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.SCALE_NODE.offset_scale")==pItem->m_idProp)		//ƫ�Ʊ���
			pPoint->des_para.SCALE_NODE.offset_scale=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.SCALE_NODE.offset_dist")==pItem->m_idProp)		//�ڵȷֻ����ϵ�ƫ��ֵ
			pPoint->des_para.SCALE_NODE.offset_dist=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==11)
	{
		if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.hLineTube")==pItem->m_idProp)
		{
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.COLUMN_INTERS.hLineTube));
			pPropList->DeleteItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.column_r"));
			pPropList->DeleteItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec"));
			pPropList->DeleteItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.x"));
			pPropList->DeleteItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.y"));
			pPropList->DeleteItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.z"));
			//
			CLDSLineTube *pLineTube = (CLDSLineTube*)console.FromPartHandle(pPoint->des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE);
			if(pLineTube==NULL)
			{	//����뾶	
				CXhChar100 sText;
				CPropTreeItem *pParentItem = pItem->m_pParent;
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Column Radius";
				lpInfo->m_strPropHelp = "Column's radius";
#else 
				lpInfo->m_strPropName = "����뾶";
				lpInfo->m_strPropHelp = "����뾶";
#endif
				CPropTreeItem *pPropItem = pPropList->InsertItem(pParentItem,lpInfo,pItem->m_iIndex,TRUE);
				pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.column_r");
				if(pPoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				//��������������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
				lpInfo->m_buttonType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Column Axis Positive Direction";
				lpInfo->m_strPropHelp = "Column axis's positive direction";
#else 
				lpInfo->m_strPropName = "��������������";
				lpInfo->m_strPropHelp = "��������������";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,pItem->m_iIndex+1,TRUE);
				pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec");
				if(pPoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_bHideChildren = TRUE;
				//x
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Column axis's X coordinate";
#else 
				lpInfo->m_strPropHelp="��������X����";
#endif
				CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.x");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
				//y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Column axis's Y coordinate";
#else 
				lpInfo->m_strPropHelp="��������Y����";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.y");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
				//z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Column axis's Z coordinate";
#else 
				lpInfo->m_strPropHelp="��������Z����";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.z");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
				//չ��
				pPropList->Expand(pItem,pItem->m_iIndex);
			}
			//����Բ�ĵ�
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_center"),NULL);
			if(pFindItem)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				CString sPropName = "Section Center";
				if(pLineTube)
					sPropName = "Section Pick Point";
				pPropList->SetItemPropName(pFindItem->m_idProp,sPropName);
				if(pLineTube)
					pFindItem->m_lpNodeInfo->m_strPropHelp = "Section pick point used to calculate section center";
				else 
					pFindItem->m_lpNodeInfo->m_strPropHelp = "Section Center";
#else 
				CString sPropName = "����Բ��";
				if(pLineTube)
					sPropName = "����ʰȡ��";
				pPropList->SetItemPropName(pFindItem->m_idProp,sPropName);
				if(pLineTube)
					pFindItem->m_lpNodeInfo->m_strPropHelp = "����ʰȡ�����������Բ�ĵ�";
				else 
					pFindItem->m_lpNodeInfo->m_strPropHelp = "����Բ��";
#endif
			}
		}
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.column_r")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.column_r=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.x")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[0]=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.y")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[1]=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.z")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[2]=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_center.x")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[0]=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_center.y")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[1]=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_center.z")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[2]=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm_std_style")==pItem->m_idProp)
		{
			long idPropX = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm.x");
			long idPropY = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm.y");
			long idPropZ = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm.z");
			if(valueStr[0]=='0')	//�Զ��巽��
			{
				pPropList->SetItemReadOnly(idPropX,FALSE);
				pPropList->SetItemReadOnly(idPropY,FALSE);
				pPropList->SetItemReadOnly(idPropZ,FALSE);
			}
			else
			{
				pPropList->SetItemReadOnly(idPropX,TRUE);
				pPropList->SetItemReadOnly(idPropY,TRUE);
				pPropList->SetItemReadOnly(idPropZ,TRUE);
				f3dPoint norm=CStdVector::GetVector(valueStr[0]-'0');
				pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[0]=norm.x;
				pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[1]=norm.y;
				pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[2]=norm.z;
				CXhChar100 sText;
				if(pPoint->GetPropValueStr(idPropX,sText)>0)
					pPropList->SetItemPropValue(idPropX,sText);
				if(pPoint->GetPropValueStr(idPropY,sText)>0)
					pPropList->SetItemPropValue(idPropY,sText);
				if(pPoint->GetPropValueStr(idPropZ,sText)>0)
					pPropList->SetItemPropValue(idPropZ,sText);
			}
		}
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm.x")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[0]=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm.y")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[1]=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm.z")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[2]=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec_style")==pItem->m_idProp)
		{	//���߷����巽ʽ
			pPoint->des_para.COLUMN_INTERS.ray_vec_style=valueStr[0]-'0';
			pPropList->DeleteAllSonItems(pItem);
			CXhChar100 sText;
			if(pPoint->des_para.COLUMN_INTERS.ray_vec_style==0||pPoint->des_para.COLUMN_INTERS.ray_vec_style==1)
			{	//��������
				CItemInfo* lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Specify Vector";
				lpInfo->m_strPropHelp = "Specify Vector";
#else 
				lpInfo->m_strPropName = "ָ������";
				lpInfo->m_strPropHelp = "ָ������";
#endif
				CPropTreeItem* pSonPropItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pSonPropItem->m_idProp =CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec_std_style");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
				if(pPoint->des_para.COLUMN_INTERS.ray_vec_style==0)
					lpInfo->m_strPropHelp="X coordinate of ray vector";
				else 
					lpInfo->m_strPropHelp="X coordinate of datum edge vector";
#else 
				if(pPoint->des_para.COLUMN_INTERS.ray_vec_style==0)
					lpInfo->m_strPropHelp="���߷���X����";
				else 
					lpInfo->m_strPropHelp="��׼�߷���X����";
#endif
				pSonPropItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec.x");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
				pSonPropItem->SetReadOnly(pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0);
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Y";
				if(pPoint->des_para.COLUMN_INTERS.ray_vec_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp="Y coordinate of ray vector";
				else 
					lpInfo->m_strPropHelp="Y coordinate of datum edge vector";
#else 
					lpInfo->m_strPropHelp="���߷���Y����";
				else 
					lpInfo->m_strPropHelp="��׼�߷���Y����";
#endif
				pSonPropItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec.y");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
				pSonPropItem->SetReadOnly(pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0);
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Z";
				if(pPoint->des_para.COLUMN_INTERS.ray_vec_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp="Z coordinate of ray vector";
				else 
					lpInfo->m_strPropHelp="Z coordinate of datum edge vector";
#else 
					lpInfo->m_strPropHelp="���߷���Z����";
				else 
					lpInfo->m_strPropHelp="��׼�߷���Z����";
#endif
				pSonPropItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec.z");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
				pSonPropItem->SetReadOnly(pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0);
			}
			if(pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0)
			{	//��ת�Ƕ�
				CItemInfo* lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Rotate Angle";
				lpInfo->m_strPropHelp="Rotate angle relates to specify datum edge,unit is ��";
#else 
				lpInfo->m_strPropName="��ת�Ƕ�";
				lpInfo->m_strPropHelp="�����ָ����׼�ߵ���ת�Ƕ�,��λΪ��";
#endif
				CPropTreeItem* pSonPropItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_angle");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
			}
		}
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_angle")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.ray_angle=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec_std_style")==pItem->m_idProp)
		{
			long idPropX = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec.x");
			long idPropY = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec.y");
			long idPropZ = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec.z");
			if(valueStr[0]=='0')	//�Զ��巽��
			{
				pPropList->SetItemReadOnly(idPropX,pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0);
				pPropList->SetItemReadOnly(idPropY,pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0);
				pPropList->SetItemReadOnly(idPropZ,pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0);
			}
			else
			{
				pPropList->SetItemReadOnly(idPropX,TRUE);
				pPropList->SetItemReadOnly(idPropY,TRUE);
				pPropList->SetItemReadOnly(idPropZ,TRUE);
				f3dPoint vector=CStdVector::GetVector(valueStr[0]-'0');
				pPoint->des_para.COLUMN_INTERS.RayVector.ray_vec[0]=vector.x;
				pPoint->des_para.COLUMN_INTERS.RayVector.ray_vec[1]=vector.y;
				pPoint->des_para.COLUMN_INTERS.RayVector.ray_vec[2]=vector.z;
				CXhChar100 sText;
				if(pPoint->GetPropValueStr(idPropX,sText)>0)
					pPropList->SetItemPropValue(idPropX,sText);
				if(pPoint->GetPropValueStr(idPropY,sText)>0)
					pPropList->SetItemPropValue(idPropY,sText);
				if(pPoint->GetPropValueStr(idPropZ,sText)>0)
					pPropList->SetItemPropValue(idPropZ,sText);
			}
		}
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec.x")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.RayVector.ray_vec[0]=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec.y")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.RayVector.ray_vec[1]=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec.z")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.RayVector.ray_vec[2]=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_para_offset")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.ray_para_offset=atof(valueStr);
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.bSpecRo")==pItem->m_idProp)
		{
			pPoint->des_para.COLUMN_INTERS.bSpecRo=(valueStr[0]-'0')!=0;
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_offset_r"),NULL);
			if(pFindItem)
			{
				if(pPoint->des_para.COLUMN_INTERS.bSpecRo)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					pPropList->SetItemPropName(pFindItem->m_idProp,CString("Offset"));
					pFindItem->m_lpNodeInfo->m_strPropHelp="Based on intersection point of ray and cylinder��the offset along ray vertor";
#else 
					pPropList->SetItemPropName(pFindItem->m_idProp,CString("ƫ����"));
					pFindItem->m_lpNodeInfo->m_strPropHelp="�����������潻��Ϊ��׼�������߷����ƫ����";
#endif
				}
				else 
				{
#ifdef AFX_TARG_ENU_ENGLISH
					pPropList->SetItemPropName(pFindItem->m_idProp,CString("Auxiliary Ray Radius"));
					pFindItem->m_lpNodeInfo->m_strPropHelp="Based on cylinder center��the offset along ray vector";
#else 
					pPropList->SetItemPropName(pFindItem->m_idProp,CString("�������߰뾶"));
					pFindItem->m_lpNodeInfo->m_strPropHelp="������Ϊ��׼�������߷����ƫ����";
#endif
				}
			}
		}
		else if(CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_offset_r")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.ray_offset_r=atof(valueStr);
	}
	//pExtraOddDlg->UpdateExtraOddDisplayValue();
	return TRUE;
}

void CDisplayPointProperty::UpdateDatumPointPropItem(CPropertyList *pPropList,CPropTreeItem *pParentItem,
													 CLDSPoint *pPoint,BOOL bUpdate,BOOL bDisplayPointSonItem/*=TRUE*/)
{
	CXhChar100 valueStr;
	CItemInfo* lpInfo=NULL;
	CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL;
	pPropList->SetItemReadOnly(CLDSPoint::GetPropID("pos.x"),pPoint->datum_pos_style!=0);
	pPropList->SetItemReadOnly(CLDSPoint::GetPropID("pos.y"),pPoint->datum_pos_style!=0);
	pPropList->SetItemReadOnly(CLDSPoint::GetPropID("pos.z"),pPoint->datum_pos_style!=0);
	pPropList->DeleteItemByPropId(CLDSPoint::GetPropID("des_para.hNode"));
	//ɾ����������
	pPropList->DeleteAllSonItems(pParentItem);

	DWORD dwPropGroup=GetSingleWord(pPropList->m_iPropGroup+1);
	if(pPoint->datum_pos_style==1)	//�Ǹ����
	{	//��׼�Ǹ־��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Angle Handle";
		lpInfo->m_strPropHelp="Datum angle's handle";
#else 
		lpInfo->m_strPropName="��׼�Ǹ־��";
		lpInfo->m_strPropHelp="��׼�Ǹ־����";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.RODEND.hRod");
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
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.RODEND.offset_wing");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="g|g1|g2|g3|User-Defined|";
		lpInfo->m_strPropName="Leg Offset Style";
		lpInfo->m_strPropHelp="Offset style is preconcert stadia or user-defining offset��";
#else 
		lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
		lpInfo->m_strPropName="֫��ƫ������";
		lpInfo->m_strPropHelp="ƫ������Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.RODEND.wing_offset_style");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		//ƫ�ƾ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Leg Offset";
		lpInfo->m_strPropHelp="Leg offset��";
#else 
		lpInfo->m_strPropName="֫��ƫ��ֵ";
		lpInfo->m_strPropHelp="֫��ƫ��ֵ��";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.RODEND.wing_offset_dist");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->SetReadOnly(pPoint->des_para.RODEND.wing_offset_style!=4);
		pPropItem->m_dwPropGroup=dwPropGroup;
		//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Length Offset Vertor";
		lpInfo->m_strPropHelp="Length offset vertor,along datum angle from start to end or from end to start offset.";
#else 
		lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
		lpInfo->m_strPropName="����ƫ�Ʒ���";
		lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.RODEND.direction");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		//����ƫ��ֵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Length Offset";
		lpInfo->m_strPropHelp="Length Offset";
#else 
		lpInfo->m_strPropName="����ƫ��ֵ";
		lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.RODEND.len_offset_dist");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		//����ƫ��ֵ��������ͷӰ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName="Length Offset Consider Oddment";
		lpInfo->m_strPropHelp="Length offset considers oddment's effect��";
#else 
		lpInfo->m_cmbItems="��|��";
		lpInfo->m_strPropName="����ƫ�Ƽ�������ͷӰ��";
		lpInfo->m_strPropHelp="����ƫ�Ƽ�������ͷӰ�졣";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.RODEND.bIncOddEffect");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
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
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.RODEND.norm_offset");
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
		lpInfo->m_strPropName="Datum Rod Handle";
		lpInfo->m_strPropHelp="Datum rod's handle��";
#else 
		lpInfo->m_strPropName="��׼�˼����";
		lpInfo->m_strPropHelp="��׼�˼������";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.hLinePart");
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
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.hNode");
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
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.offset_wing");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
			//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|User-Defined|";
			lpInfo->m_strPropName="Leg Offset Style";
			lpInfo->m_strPropHelp="Offset style is preconcert stadia or user-defining offset��";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������";
			lpInfo->m_strPropHelp="ƫ������Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.wing_offset_style");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset ";
			lpInfo->m_strPropHelp="Leg offset��";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ";
			lpInfo->m_strPropHelp="֫��ƫ��ֵ��";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.wing_offset_dist");
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
		lpInfo->m_strPropName="Length Offset Vertor";
		lpInfo->m_strPropHelp="Length offset vertor,along datum angle from start to end or from end to start offset.";
#else 
		lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
		lpInfo->m_strPropName="����ƫ�Ʒ���";
		lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.direction");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		//����ƫ��ֵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Length Offset";
		lpInfo->m_strPropHelp="Length Offset";
#else 
		lpInfo->m_strPropName="����ƫ��ֵ";
		lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.len_offset_dist");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
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
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.norm_offset");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		//ƽ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName="Plane Offset";
		lpInfo->m_strPropHelp="Plane Offset";
#else 
		lpInfo->m_cmbItems="��|��";
		lpInfo->m_strPropName="ƽ��";
		lpInfo->m_strPropHelp="ƽ��";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.bFaceOffset");
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
			lpInfo->m_strPropHelp="The offset plane's normal";
#else 
			lpInfo->m_strPropName="ƽ���淨��";
			lpInfo->m_strPropHelp="ƽ���淨��";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
			int vectorType=CStdVector::GetVectorType(pPoint->des_para.RODNODE.vPlaneNormal);
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The x coordinate of offset plane's normal";
#else 
			lpInfo->m_strPropHelp="ƽ���淨��X�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]");
			if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
			pSonPropItem->m_dwPropGroup=dwPropGroup;
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The y coordinate of offset plane's normal";
#else 
			lpInfo->m_strPropHelp="ƽ���淨��Y�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]");
			if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
			pSonPropItem->m_dwPropGroup=dwPropGroup;
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The z coordinate of offset plane's normal";
#else 
			lpInfo->m_strPropHelp="ƽ���淨��Z�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]");
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
		lpInfo->m_strPropHelp="Datum rod1's handle��";
#else 
		lpInfo->m_strPropName="��׼�˼�1���";
		lpInfo->m_strPropHelp="��׼�˼�1�����";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.AXIS_INTERS.hDatum1");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		CLDSLinePart *pLinePart1=(CLDSLinePart*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEPART);
		if(pLinePart1&&pLinePart1->IsAngle())
		{	//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|User-Defined|";
			lpInfo->m_strPropName="Leg Offset Style 1";
			lpInfo->m_strPropHelp="Datum angle offset style: preconcert stadia or user-defining offset��";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������1";
			lpInfo->m_strPropHelp="��׼�Ǹ�1ƫ������:Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.AXIS_INTERS.wing_offset_style1");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset 1";
			lpInfo->m_strPropHelp="Leg offset of datum angle 1.";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ1";
			lpInfo->m_strPropHelp="��׼�Ǹ�1֫��ƫ��ֵ��";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.AXIS_INTERS.wing_offset_dist1");
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
		lpInfo->m_strPropHelp="The handle of datum rod 2";
#else 
		lpInfo->m_strPropName="��׼�˼�2���";
		lpInfo->m_strPropHelp="��׼�˼�2���";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.AXIS_INTERS.hDatum2");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		CLDSLinePart *pLinePart2=(CLDSLinePart*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEPART);
		if(pLinePart2&&pLinePart2->IsAngle())
		{	//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|User-Defined|";
			lpInfo->m_strPropName="Leg Offset Style 2";
			lpInfo->m_strPropHelp="Datum angle 2 offset style: preconcert stadia or user-defining offset��";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������2";
			lpInfo->m_strPropHelp="��׼�Ǹ�2ƫ������:Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.AXIS_INTERS.wing_offset_style2");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset 2";
			lpInfo->m_strPropHelp="Leg offset of datum angle 2";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ2";
			lpInfo->m_strPropHelp="��׼�Ǹ�2֫��ƫ��ֵ��";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.AXIS_INTERS.wing_offset_dist2");
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
		lpInfo->m_strPropHelp="Normal Offset";
#else 
		lpInfo->m_strPropName="����ƫ����";
		lpInfo->m_strPropHelp="����ƫ����";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.AXIS_INTERS.norm_offset");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=dwPropGroup;
		//���Ʒ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Near Vector";
		lpInfo->m_strPropHelp="���Ʒ���";
#else 
		lpInfo->m_strPropName="���Ʒ���";
		lpInfo->m_strPropHelp="���Ʒ���";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.AXIS_INTERS.near_vector");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_bHideChildren=TRUE;
		pPropItem->m_dwPropGroup=dwPropGroup;
		int vectorType=CStdVector::GetVectorType(f3dPoint(pPoint->des_para.AXIS_INTERS.near_vector));
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="X coordinate of near normal";
#else 
		lpInfo->m_strPropHelp="���Ʒ���X�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.AXIS_INTERS.near_vector[0]");
		if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->SetReadOnly(vectorType!=0);
		pSonPropItem->m_dwPropGroup=dwPropGroup;
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Y coordinate of near normal";
#else 
		lpInfo->m_strPropHelp="���Ʒ���Y�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.AXIS_INTERS.near_vector[1]");
		if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->SetReadOnly(vectorType!=0);
		pSonPropItem->m_dwPropGroup=dwPropGroup;
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Z coordinate of near normal";
#else 
		lpInfo->m_strPropHelp="���Ʒ���Z�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.AXIS_INTERS.near_vector[2]");
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
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.AXIS_INTERS.hDatum1");
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
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.AXIS_INTERS.hDatum2");
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
		lpInfo->m_strPropHelp="Datum line handle";
#else 
		lpInfo->m_strPropName="��׼�߾��";
		lpInfo->m_strPropHelp="��׼�߾��";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.DATUMLINE.hDatumLine");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ָ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.X Axis|1.Y Axis|2.Z Axis";
		lpInfo->m_strPropName="Specify Axis";
		lpInfo->m_strPropHelp="Specify axis";
#else 
		lpInfo->m_cmbItems="0.X��|1.Y��|2.Z��";
		lpInfo->m_strPropName="ָ������";
		lpInfo->m_strPropHelp="ָ������";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pPoint->datum_pos_style==6)	//��׼�����������겻���
	{
		//��׼����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbItems=GetPlaneHandleStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Plane Handle";
#else 
		lpInfo->m_strPropName="��׼����";
#endif
		lpInfo->m_strPropHelp=GetSelectPlaneDesc(pPoint);
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.DATUMPLANE.hDatumPlane");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//����ԳƱ任����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="None|X Mirror|Y Mirror|Z Mirror";
		lpInfo->m_strPropName="Reference Point Mirror Style";
		lpInfo->m_strPropHelp="First according to current point create mirroring reference point,second intersects with datum plane,then mirrors to origin position��";
#else 
		lpInfo->m_cmbItems="�޶ԳƲ���|X��Գ�|Y��Գ�|Z��Գ�";
		lpInfo->m_strPropName="���յ�ԳƷ�ʽ";
		lpInfo->m_strPropHelp="�ȸ��ݵ�ǰ�����ɶԳƵĲ��յ㣬�����׼�����������Ȼ���ٶԳƻ�ԭλ�á�";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.DATUMPLANE.cMirTransType");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ָ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Y-Z|1.X-Z|2.X-Y";
		lpInfo->m_strPropName="Specify Coordinate";
		lpInfo->m_strPropHelp="Specify coordinate axis";
#else 
		lpInfo->m_cmbItems="0.Y-Z����|1.X-Z����|2.X-Y����";
		lpInfo->m_strPropName="ָ���������";
		lpInfo->m_strPropHelp="ָ���������";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2");
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
		lpInfo->m_strPropHelp="Datum angle's handle.";
#else 
		lpInfo->m_strPropName="��׼�Ǹ־��";
		lpInfo->m_strPropHelp="��׼�Ǹ־����";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.RODEND.hRod");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Length Offset Vertor";
		lpInfo->m_strPropHelp="Length offset vertor,along datum angle from start to end or from end to start offset.";
#else 
		lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
		lpInfo->m_strPropName="����ƫ�Ʒ���";
		lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.RODEND.direction");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//����ƫ��ֵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Length Offset";
		lpInfo->m_strPropHelp="Length Offset";
#else 
		lpInfo->m_strPropName="����ƫ��ֵ";
		lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.RODEND.len_offset_dist");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
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
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.RODEND.norm_offset");
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
		lpInfo->m_strPropHelp="Datum node's handle��";
#else 
		lpInfo->m_strPropName="��׼�ڵ���";
		lpInfo->m_strPropHelp="��׼�ڵ�����";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.hNode");
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
		lpInfo->m_strPropHelp="Datum part's handle.";
#else 
		lpInfo->m_strPropName="��׼�������";
		lpInfo->m_strPropHelp="��׼���������";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.hPart");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Relative Coordinate";
		lpInfo->m_strPropHelp="The relative coordinate";
#else 
		lpInfo->m_strPropName="�������";
		lpInfo->m_strPropHelp="�������";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("datum_pos");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The x coordinate";
#else 
		lpInfo->m_strPropHelp="�������X�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp = CLDSPoint::GetPropID("datum_pos.x");
		if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The y coordinate";
#else 
		lpInfo->m_strPropHelp="�������Y�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp = CLDSPoint::GetPropID("datum_pos.y");
		if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The z coordinate";
#else 
		lpInfo->m_strPropHelp="�������Z�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp = CLDSPoint::GetPropID("datum_pos.z");
		if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pPoint->datum_pos_style==10)
	{	//λ��
		//��ϽǸ־��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Rod Handle";
		lpInfo->m_strPropHelp="The handle of datum rod;consider the rod's odd length information when the datum node as datum part's endpoint��";
#else 
		lpInfo->m_strPropName="��׼�˼����";
		lpInfo->m_strPropHelp="��׼�˼��������׼�ڵ�Ϊ�˼���׼�˼��˵�ʱ�ῼ�Ǹ˼�������ͷ��Ϣ��";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.SCALE_NODE.hLinePart");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ʼ�˻�׼�ڵ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Node Start Handle";
		lpInfo->m_strPropHelp="The handle of datum rod;consider the rod's odd length information when the datum node as datum part's endpoint��";
#else 
		lpInfo->m_strPropName="ʼ�˻�׼�ڵ���";
		lpInfo->m_strPropHelp="ʼ�˻�׼�ڵ�������׼�ڵ�Ϊ�˼���׼�˼��˵�ʱ�ῼ�Ǹ˼�������ͷ��Ϣ��";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.SCALE_NODE.hStartNode");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ʼ��ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Start Offset";
		lpInfo->m_strPropHelp="The offset of start node along the direction of start node to end node��";
#else 
		lpInfo->m_strPropName="ʼ��ƫ����";
		lpInfo->m_strPropHelp="ʼ�˽ڵ�����ʼ�˽ڵ�->�ն˽ڵ㷽���ƫ������";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.SCALE_NODE.start_offset_dist");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//�ն˻�׼�ڵ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="End Datum Node Handle";
		lpInfo->m_strPropHelp="The handle of end node;consider the rod's odd length information when the datum node as datum part's endpoint��";
#else 
		lpInfo->m_strPropName="�ն˻�׼�ڵ���";
		lpInfo->m_strPropHelp="�ն˻�׼�ڵ�������׼�ڵ�Ϊ�˼���׼�˼��˵�ʱ�ῼ�Ǹ˼�������ͷ��Ϣ��";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.SCALE_NODE.hEndNode");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//�ն�ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="End Offset";
		lpInfo->m_strPropHelp="The offset of head node along the direction of end node to start node��";
#else 
		lpInfo->m_strPropName="�ն�ƫ����";
		lpInfo->m_strPropHelp="�ն˽ڵ������ն˽ڵ�->ʼ�˽ڵ㷽���ƫ������";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.SCALE_NODE.end_offset_dist");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ƫ�Ʊ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Offset Scale";
		lpInfo->m_strPropHelp="Calculate data from start node's offset position��";
#else 
		lpInfo->m_strPropName="ƫ�Ʊ���";
		lpInfo->m_strPropHelp="��ʼ�˽ڵ㿼��ʼ��ƫ�����Ժ��λ�ÿ�ʼ���㡣";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.SCALE_NODE.offset_scale");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ƫ�ƾ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Offset Distance";
		lpInfo->m_strPropHelp="Calculated on offset ratio first and then get distance after offset��";
#else 
		lpInfo->m_strPropName="ƫ�ƾ���";
		lpInfo->m_strPropHelp="��ƫ�Ʊ��������Ժ��ٽ���ƫ�Ƶľ��롣";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.SCALE_NODE.offset_dist");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	//���涨λ��
	else if(pPoint->datum_pos_style==11)
	{	//�ֹܾ��
		lpInfo= new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Tube Handle";
		lpInfo->m_strPropHelp = "The Tube's handle is used to determine the column";
#else 
		lpInfo->m_strPropName = "��׼�ֹܾ��";
		lpInfo->m_strPropHelp = "����ȷ������ĸֹܾ��";
#endif
		CPropTreeItem *pTubePropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pTubePropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.hLineTube");
		if(pPoint->GetPropValueStr(pTubePropItem->m_idProp,valueStr)>0)
			pTubePropItem->m_lpNodeInfo->m_strPropValue = valueStr;
		pTubePropItem->m_bHideChildren = FALSE;

		CLDSLineTube *pLineTube = (CLDSLineTube*)console.FromPartHandle(pPoint->des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE);
		if(pLineTube==NULL)
		{	//����뾶	
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Column Radius";
			lpInfo->m_strPropHelp = "Column's radius";
#else 
			lpInfo->m_strPropName = "����뾶";
			lpInfo->m_strPropHelp = "����뾶";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.column_r");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
			//��������������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Column Axis Positive Direction";
			lpInfo->m_strPropHelp = "Column axis's positive direction";
#else 
			lpInfo->m_strPropName = "��������������";
			lpInfo->m_strPropHelp = "��������������";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
			if(bDisplayPointSonItem)
			{
				pPropItem->m_bHideChildren = TRUE;
				//x
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Column axis's X coordinate";
#else 
				lpInfo->m_strPropHelp="��������X����";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.x");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				//y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Column axis's Y coordinate";
#else 
				lpInfo->m_strPropHelp="��������Y����";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.y");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				//z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Column axis's Z coordinate";
#else 
				lpInfo->m_strPropHelp="��������Z����";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.axis_vec.z");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			}
		}
		//����Բ�ĵ�
#ifdef AFX_TARG_ENU_ENGLISH
		CString sPropName = "Section Center";
		if(pLineTube)
			sPropName = "Section Pick Point";
#else 
		CString sPropName = "����Բ��";
		if(pLineTube)
			sPropName = "����ʰȡ��";
#endif
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMON;
		lpInfo->m_strPropName = sPropName;
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineTube)
			lpInfo->m_strPropHelp = "Section pick point used to calculate section center";
		else 
			lpInfo->m_strPropHelp = "Section Center";
#else 
		if(pLineTube)
			lpInfo->m_strPropHelp = "����ʰȡ�����������Բ�ĵ�";
		else 
			lpInfo->m_strPropHelp = "����Բ��";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_center");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
		if(bDisplayPointSonItem)
		{
			pPropItem->m_bHideChildren = TRUE;
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp=sPropName+"X Coordinate";
#else 
			lpInfo->m_strPropHelp=sPropName+"X����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_center.x");
			if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp=sPropName+"Y Coordinate";
#else 
			lpInfo->m_strPropHelp=sPropName+"Y����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_center.y");
			if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp=sPropName+"Z Coordinate";
#else 
			lpInfo->m_strPropHelp=sPropName+"Z����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_center.z");
			if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		}
		//���淨��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Section Normal";
		lpInfo->m_strPropHelp="Section normal:it means vertical section of cylinder when it's null";
#else 
		lpInfo->m_strPropName="���淨��";
		lpInfo->m_strPropHelp="���淨�ߣ�Ϊ��ʱ��ʾΪ����Ĵ�ֱ����";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
		if(bDisplayPointSonItem)
		{
			pPropItem->m_bHideChildren = TRUE;
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Specify Vector";
			lpInfo->m_strPropHelp = "Specify Vector";
#else 
			lpInfo->m_strPropName = "ָ������";
			lpInfo->m_strPropHelp = "ָ������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonPropItem->m_idProp =CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm_std_style");
			if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="X";
#else 
			lpInfo->m_strPropHelp="���淨��X����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm.x");
			if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="Y";
#else 
			lpInfo->m_strPropHelp="���淨��Y����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm.y");
			if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="Z";
#else 
			lpInfo->m_strPropHelp="���淨��Z����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.sect_norm.z");
			if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		}
		//�����巽ʽ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Specify Directly|1.Specify Datum Edge|";
		if(pLineTube)
			lpInfo->m_cmbItems+="2.Tube's Datum Edge As Datum|3.Tube's Weld Line As Datum";
		lpInfo->m_strPropName="Ray Vector";
		lpInfo->m_strPropHelp="Ray vector define type:0.Specify directly,1 to 3 input rotate angle (1.Specify Datum Edge,2.Tube's Datum Edge As Datum,3.Tube's Weld Line As Datum)";
#else 
		lpInfo->m_cmbItems="0.ֱ��ָ��|1.ָ����׼��|";
		if(pLineTube)
			lpInfo->m_cmbItems+="2.�Ըֹܻ�׼��Ϊ��׼|3.�Ըֹܺ�����Ϊ��׼";
		lpInfo->m_strPropName="���߷���";
		lpInfo->m_strPropHelp="���߷���������:0.ֱ��ָ��,1~3������ת�Ƕ�(1.ָ����׼��,2.�Ըֹܻ�׼��Ϊ��׼,3.�Ըֹܺ�����Ϊ��׼)";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec_style");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		if(pPoint->des_para.COLUMN_INTERS.ray_vec_style==0||pPoint->des_para.COLUMN_INTERS.ray_vec_style==1)
		{	
			if(bDisplayPointSonItem)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Specify Vector";
				lpInfo->m_strPropHelp = "Specify Vector";
#else 
				lpInfo->m_strPropName = "ָ������";
				lpInfo->m_strPropHelp = "ָ������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
				pSonPropItem->m_idProp =CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec_std_style");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="X";
				if(pPoint->des_para.COLUMN_INTERS.ray_vec_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp="X coordinate of ray vector";
				else 
					lpInfo->m_strPropHelp="X coordinate of datum edge vector";
#else 
					lpInfo->m_strPropHelp="���߷���X����";
				else 
					lpInfo->m_strPropHelp="��׼�߷���X����";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec.x");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0);
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Y";
				if(pPoint->des_para.COLUMN_INTERS.ray_vec_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp="Y coordinate of ray vector";
				else 
					lpInfo->m_strPropHelp="Y coordinate of datum edge vector";
#else 
					lpInfo->m_strPropHelp="���߷���Y����";
				else 
					lpInfo->m_strPropHelp="��׼�߷���Y����";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec.y");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0);
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Z";
				if(pPoint->des_para.COLUMN_INTERS.ray_vec_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp="Z coordinate of ray vector";
				else 
					lpInfo->m_strPropHelp="Z coordinate of datum edge vector";
#else 
					lpInfo->m_strPropHelp="���߷���Z����";
				else 
					lpInfo->m_strPropHelp="��׼�߷���Z����";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
				pSonPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_vec.z");
				if(pPoint->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0);
			}
		}
		if(pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0)
		{	//��ת�Ƕ�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Rotate Angle";
			lpInfo->m_strPropHelp="Rotate angle relates to specify datum edge,unit is ��";
#else 
			lpInfo->m_strPropName="��ת�Ƕ�";
			lpInfo->m_strPropHelp="�����ָ����׼�ߵ���ת�Ƕ�,��λΪ��";
#endif
			pPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_angle");
			if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		}
		//����ƽ���ƶ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Ray Parallel Offset";
		lpInfo->m_strPropHelp="Ray parallel offset��Parallel lines moving perpendicular to the ray direction,along the positive direction side moves with right hand screwing is positive,and the another side is negative��";
#else 
		lpInfo->m_strPropName="����ƽ�ƾ���";
		lpInfo->m_strPropHelp="����ƽ���ƶ����루��ֱ���߷������ƽ�����ƶ�,����������������ƶ�Ϊ������һ��Ϊ����";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_para_offset");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;

		//ƫ�ƻ�׼��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Ray-Cylinder Intersection|1.Cylinder Center";
		lpInfo->m_strPropName="Offset Datum Point";
		lpInfo->m_strPropHelp="0.Based on intersection point of ray and cylinder��offset along ray vertor,1.Based on cylinder center��offset along ray vector";
#else 
		lpInfo->m_cmbItems="0.���������潻��|1.Բ������";
		lpInfo->m_strPropName="ƫ�ƻ�׼��";
		lpInfo->m_strPropHelp="0.������������Ľ���Ϊ��׼�������߷���ƫ��,1.������Ϊ��׼�������߷���ƫ��";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.bSpecRo");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		if(pPoint->des_para.COLUMN_INTERS.bSpecRo)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Offset";
			lpInfo->m_strPropHelp="Based on intersection point of ray and cylinder��the offset along ray vertor";
#else 
			lpInfo->m_strPropName="ƫ����";
			lpInfo->m_strPropHelp="�����������潻��Ϊ��׼�������߷����ƫ����";
#endif
		}
		else 
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Auxiliary Ray Radius";
			lpInfo->m_strPropHelp="Based on cylinder center��the offset along ray vector";
#else 
			lpInfo->m_strPropName="�������߰뾶";
			lpInfo->m_strPropHelp="������Ϊ��׼�������߷����ƫ����";
#endif
		}
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.ray_offset_r");
		if(pPoint->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
}

void CDisplayPointProperty::DisplayPropList(CPropertyList *pPropList,CLDSPoint *pPoint)
{
	CPropTreeItem *pGroupItem,*pPropItem,*pRoot=pPropList->GetRootItem();
	char sText[MAX_TEMP_CHAR_100+1]="";
	CItemInfo* lpInfo = NULL;

	pPropList->m_pObj = pPoint;
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Datum Point Coordinate";
	lpInfo->m_strPropHelp="Datum point's coordinate";
#else 
	lpInfo->m_strPropName="��������";
	lpInfo->m_strPropHelp="��������";
#endif
	CPropTreeItem* pDatumPointItem = pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
	pGroupItem->m_idProp = CLDSPoint::GetPropID("pos");
	pGroupItem->m_bHideChildren=FALSE;

	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp="X Coordinate";
#else 
	lpInfo->m_strPropHelp="��׼��X����";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSPoint::GetPropID("pos.x");
	if(pPoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	pPropItem->SetReadOnly();
	CPropTreeItem *pXPosPropItem=pPropItem;
	//Y
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp="Y Coordinate";
#else 
	lpInfo->m_strPropHelp="��׼��Y����";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSPoint::GetPropID("pos.y");
	if(pPoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	pPropItem->SetReadOnly();
	CPropTreeItem *pYPosPropItem=pPropItem;
	//Z
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp="Z Coordinate";
#else 
	lpInfo->m_strPropHelp="��׼��Z����";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSPoint::GetPropID("pos.z");
	if(pPoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	pPropItem->SetReadOnly();
	CPropTreeItem *pZPosPropItem=pPropItem;

	//���㶨λ��ʽ
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Datum Point Position Style";
	lpInfo->m_strPropHelp="Datum point's position style";
	lpInfo->m_cmbItems="0.Manual Input|1.Angle Ridge Vertex|2.Rod Node Position|3.Bolt line Intersection Point|4.Datum Line Intersection|5.Coord. Specified On Datum Line|6.Coord. Specified  On Datum Plane|7.Angle Ridge Offset|8.Same Position With Node|9.Relative Coordinate On Datum Part|10.Scale Equal Point|11.Column Locate Point";
#else 
	lpInfo->m_strPropName="���㶨λ��ʽ";
	lpInfo->m_strPropHelp="���㶨λ��ʽ";
	lpInfo->m_cmbItems="0.�ֶ�����|1.�Ǹ���㶨λ��|2.�˼��ڵ㶨λ��|3.�Ǹ����߽���|4.��׼�߿ռ佻��|5.��׼����ָ�����������|6.��׼����ָ�����������|7.�ؽǸ���֫����ƫ�Ƶİ������|8.��ڵ�ͬλ|9.��׼�����ϵ��������|10.���ڵ��ı����ȷֵ�|11.���涨λ��";
#endif
	pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
	pGroupItem->m_idProp = CLDSPoint::GetPropID("datum_pos_style");
	if(pPoint->GetPropValueStr(pGroupItem->m_idProp,sText)>0)
		pGroupItem->m_lpNodeInfo->m_strPropValue=sText;
	pGroupItem->m_bHideChildren=FALSE;
	
	pDatumPointItem->m_bHideChildren = (pPoint->datum_pos_style!=0);
	if(pPoint->datum_pos_style==0)		//ֱ��ָ������
	{	//��ֱ�ӱ༭�޸�
		pXPosPropItem->SetReadOnly(FALSE);
		pYPosPropItem->SetReadOnly(FALSE);
		pZPosPropItem->SetReadOnly(FALSE);
	}
	else
		UpdateDatumPointPropItem(pPropList,pGroupItem,pPoint,FALSE);
}

BOOL CDisplayPointProperty::FinishSelectObjOper(CLDSPoint *pPoint,long hPickObj,long idEventProp)
{
	if(pPoint==NULL)
		return FALSE;
	if(idEventProp==CLDSPoint::GetPropID("des_para.LINEPARTNODE.hLinePart"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(hPickObj,CLS_LINEPART);
		if(pLinePart)
		{
			pPoint->des_para.RODNODE.hRod =pLinePart->handle;
			pPoint->UpdatePos(console.GetActiveModel());
		}
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(idEventProp==CLDSPoint::GetPropID("des_para.RODEND.hRod"))
	{
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(hPickObj,CLS_LINEANGLE);
		if(pLineAngle)
		{
			pPoint->des_para.RODEND.hRod=pLineAngle->handle;
			pPoint->UpdatePos(console.GetActiveModel());
		}
	} 
#endif
	else if(idEventProp==CLDSPoint::GetPropID("des_para.LINEPARTNODE.hNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(hPickObj);
		if(pNode)
		{
			pPoint->des_para.RODNODE.hNode=pNode->handle;
			pPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(idEventProp==CLDSPoint::GetPropID("des_para.AXIS_INTERS.hDatum1"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(hPickObj,CLS_LINEPART);
		if(pLinePart)
		{
			pPoint->des_para.AXIS_INTERS.hDatum1=pLinePart->handle;
			pPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(idEventProp==CLDSPoint::GetPropID("des_para.AXIS_INTERS.hDatum2"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(hPickObj,CLS_LINEPART);
		if(pLinePart)
		{
			pPoint->des_para.AXIS_INTERS.hDatum2=pLinePart->handle;
			pPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(idEventProp==CLDSPoint::GetPropID("des_para.hNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(hPickObj);
		if(pNode)
		{
			pPoint->des_para.hNode=pNode->handle;
			pPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(idEventProp==CLDSPoint::GetPropID("des_para.hPart"))
	{
		CLDSPart *pPart=console.FromPartHandle(hPickObj);
		if(pPart)
		{
			pPoint->des_para.hPart=pPart->handle;
			pPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else if(idEventProp==CLDSPoint::GetPropID("des_para.COLUMN_INTERS.hLineTube"))
	{
		CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(hPickObj,CLS_LINETUBE);
		if(pLineTube)
		{
			pPoint->des_para.COLUMN_INTERS.hLineTube=pLineTube->handle;
			pPoint->UpdatePos(console.GetActiveModel());
		}
	}
	else 
		return FALSE;
	return TRUE;
}

CString CDisplayPointProperty::GetPlaneHandleStr()
{
	CString sPlaneHandle="";
	CString sValue;
	int nPlane=Ta.Plane.GetNodeNum();
	for(int i=0;i<nPlane;i++)
	{
		CLDSPlane pPlane=Ta.Plane[i];
		if(i==nPlane-1)
			sValue.Format("0X%X",pPlane.handle);
		else 
			sValue.Format("0X%X|",pPlane.handle);
		sPlaneHandle+=sValue;
	}	
	return sPlaneHandle;
}
CString CDisplayPointProperty::GetSelectPlaneDesc(CLDSPoint* pPoint)
{
#ifdef AFX_TARG_ENU_ENGLISH
	CString sDescription="Datum Plane��";
#else 
	CString sDescription="��׼�棺";
#endif
	CString ss;
	CLDSPlane *pPlane=console.FromPlaneHandle(pPoint->des_para.DATUMPLANE.hDatumPlane);
	if(pPlane)
	{
		ss.Format("%s_0X%X",pPlane->description,pPlane->handle);
		sDescription+=ss;
	}
	return sDescription;
}
//</DEVELOP_PROCESS_MARK>