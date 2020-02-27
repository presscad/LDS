//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "NativePropertyList.h"

long CParaCSPropertyList::InitPropHashtable(long idInit/*=1*/,long initHashSize/*=50*/,long initStatusHashSize/*=0*/)	//������һ����Id
{
	long id=CNativePropertyList::InitPropHashtable(1,50,8);
	//SetPropKey("des_cs.m_cDefStyle",id++);
	SetPropKey("m_eDatumPlane",id++);
	SetPropKey("ucs",id++);
	SetPropKey("ucs.origin",id++);
	SetPropKey("ucs.origin.x",id++);
	SetPropKey("ucs.origin.y",id++);
	SetPropKey("ucs.origin.z",id++);
	SetPropKey("ucs.axis_x",id++);
	SetPropKey("ucs.axis_x.x",id++);
	SetPropKey("ucs.axis_x.y",id++);
	SetPropKey("ucs.axis_x.z",id++);
	SetPropKey("ucs.axis_y",id++);
	SetPropKey("ucs.axis_y.x",id++);
	SetPropKey("ucs.axis_y.y",id++);
	SetPropKey("ucs.axis_y.z",id++);
	SetPropKey("ucs.axis_z",id++);
	SetPropKey("ucs.axis_z.x",id++);
	SetPropKey("ucs.axis_z.y",id++);
	SetPropKey("ucs.axis_z.z",id++);
	return id;
}
long CParaCSPropertyList::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	char sText[100]="";
	GECS cs=paracs.GetACS();
	/*if(GetPropID("basicInfo")==id)
		strcpy(sText,"");
	else if(GetPropID("sName")==id)
		strcpy(sText,this->name);
	else if(GetPropID("folderName")==id)
		sprintf(sText,"%d",this->folderName);
	else if(GetPropID("layerName")==id)
		sprintf(sText,"%d",this->defaultLayer);*/
	//
	/*else if(GetPropID("des_cs.m_cDefStyle")==id)
	{
		if(des_cs.GetParamInfo().m_cDefStyle==1)
			strcpy(sText,"1.�Գ�");
		else
			strcpy(sText,"0.����������");	
	}
	else */
	if(GetPropID("m_eDatumPlane")==id)
	{
		CParaCS::PARAMETRIC_INFO param=paracs.GetParamInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		if(param.m_eDatumPlane==CParaCS::DP_XY)
			strcpy(sText,"0.X-Y Plane");
		else if(param.m_eDatumPlane==CParaCS::DP_YX)
			strcpy(sText,"1.Y-X Plane");
		else if(param.m_eDatumPlane==CParaCS::DP_YZ)
			strcpy(sText,"2.Y-Z Plane");
		else if(param.m_eDatumPlane==CParaCS::DP_ZY)
			strcpy(sText,"3.Z-Y Plane");
		else if(param.m_eDatumPlane==CParaCS::DP_ZX)
			strcpy(sText,"4.Z-X Plane");
		else if(param.m_eDatumPlane==CParaCS::DP_XZ)
			strcpy(sText,"5.X-Z Plane");
#else 
		if(param.m_eDatumPlane==CParaCS::DP_XY)
			strcpy(sText,"0.X-Y��");
		else if(param.m_eDatumPlane==CParaCS::DP_YX)
			strcpy(sText,"1.Y-X��");
		else if(param.m_eDatumPlane==CParaCS::DP_YZ)
			strcpy(sText,"2.Y-Z��");
		else if(param.m_eDatumPlane==CParaCS::DP_ZY)
			strcpy(sText,"3.Z-Y��");
		else if(param.m_eDatumPlane==CParaCS::DP_ZX)
			strcpy(sText,"4.Z-X��");
		else if(param.m_eDatumPlane==CParaCS::DP_XZ)
			strcpy(sText,"5.X-Z��");
#endif
	}
	else if(GetPropID("ucs.origin.x")==id)
	{
		sprintf(sText,"%f",cs.origin.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.y")==id)
	{
		sprintf(sText,"%f",cs.origin.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.z")==id)
	{
		sprintf(sText,"%f",cs.origin.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x")==id)
		strcpy(sText,paracs.GetAxisDescription('X'));
	else if(GetPropID("ucs.axis_x.x")==id)
	{
		sprintf(sText,"%f",cs.axis_x.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.y")==id)
	{
		sprintf(sText,"%f",cs.axis_x.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.z")==id)
	{
		sprintf(sText,"%f",cs.axis_x.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y")==id)
		strcpy(sText,paracs.GetAxisDescription('Y'));
	else if(GetPropID("ucs.axis_y.x")==id)
	{
		sprintf(sText,"%f",cs.axis_y.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.y")==id)
	{
		sprintf(sText,"%f",cs.axis_y.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.z")==id)
	{
		sprintf(sText,"%f",cs.axis_y.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z")==id)
	{
		strcpy(sText,paracs.GetAxisDescription('Z'));
		CParaCS::PARAMETRIC_INFO param=paracs.GetParamInfo();
		if(strlen(sText)>2&&param.desAxisZ.norm_style==3)
		{
			if(param.desAxisZ.direction==0)
				strcat(sText,CXhChar16(":0X%X��",param.desAxisZ.hVicePart));
			else
				strcat(sText,CXhChar16(":0X%X��",param.desAxisZ.hVicePart));
		}
	}
	else if(GetPropID("ucs.axis_z.x")==id)
	{
		sprintf(sText,"%f",cs.axis_z.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.y")==id)
	{
		sprintf(sText,"%f",cs.axis_z.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.z")==id)
	{
		sprintf(sText,"%f",cs.axis_z.z);
		SimplifiedNumString(sText);
	}
	else
		return false;
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
bool CParaCSPropertyList::ModifyValueFunc(CPropTreeItem* pItem,CString& valueStr,BOOL *lpRslt)
{
	if(pItem->m_idProp==GetPropID("m_eDatumPlane"))
	{
		CParaCS::PARAMETRIC_INFO param=paracs.GetParamInfo();
		param.m_eDatumPlane=valueStr[0]-'0';
		paracs.SetParamInfo(param);
		char sText[100]="";
		long idProp=GetPropID("ucs.axis_x");
		if(GetPropValueStr(idProp,sText))
			SetItemPropValue(idProp,sText);
		idProp=GetPropID("ucs.axis_y");
		if(GetPropValueStr(idProp,sText))
			SetItemPropValue(idProp,sText);
		idProp=GetPropID("ucs.axis_z");
		if(GetPropValueStr(idProp,sText))
			SetItemPropValue(idProp,sText);
		return true;
	}
	return false;
}
void CParaCSPropertyList::CopyFrom(CParaCS* pParaCS)
{
	CParaCS::PARAMETRIC_INFO param =pParaCS->GetParamInfo();
	paracs.SetACS(pParaCS->GetACS(),false);
	paracs.SetParamInfo(param);
}
void CParaCSPropertyList::CopyTo(CParaCS* pParaCS)
{
	CParaCS::PARAMETRIC_INFO param =paracs.GetParamInfo();
	pParaCS->SetACS(paracs.GetACS(),false);
	pParaCS->SetParamInfo(param);
}
#include "PartUcsDlg.h"
#include "DesignVectorPropDlg.h"
#include "DatumPointPropertyDlg.h"
#include "reportvectordlg.h"
bool CParaCSPropertyList::CallBackCommand(UINT message,WPARAM wParam,BOOL *lpRslt)
{
	if( (message==GetPropID("ucs.axis_x")&&paracs.IsDesAxisX())||
		(message==GetPropID("ucs.axis_y")&&paracs.IsDesAxisY())||
		(message==GetPropID("ucs.axis_z")&&paracs.IsDesAxisZ()))
	{
		CDesignVectorPropDlg desVecDlg;
		CParaCS::PARAMETRIC_INFO param=paracs.GetParamInfo();
		if(message==GetPropID("ucs.axis_x"))
			desVecDlg.m_xVector = param.desAxisX;
		else if(message==GetPropID("ucs.axis_y"))
			desVecDlg.m_xVector = param.desAxisY;
		else 
			desVecDlg.m_xVector = param.desAxisZ;
		if(desVecDlg.DoModal()==IDOK)
		{
			if(message==GetPropID("ucs.axis_x"))
				param.desAxisX=desVecDlg.m_xVector;
			else if(message==GetPropID("ucs.axis_y"))
				param.desAxisY=desVecDlg.m_xVector;
			else 
				param.desAxisZ=desVecDlg.m_xVector;
			paracs.SetParamInfo(param);
			paracs.UpdateACS(m_pBelongModel);
			//pCurView->ucs=paracs.GetACS();
		}
	}
	else if(message==GetPropID("ucs.origin"))
	{
		CDatumPointPropertyDlg desPointDlg;
		CParaCS::PARAMETRIC_INFO param=paracs.GetParamInfo();
		desPointDlg.m_xPoint=param.desOrg;
		if(desPointDlg.DoModal()==IDOK)
		{
			param.desOrg=desPointDlg.m_xPoint;
			paracs.SetParamInfo(param);
			paracs.UpdateACS(m_pBelongModel);
			//pCurView->ucs=pCurView->des_cs.GetACS();
		}
	}
	//pViewDlg->m_iCurDesParaType=-1;	//��ʼ��
	return true;
}
void CParaCSPropertyList::DisplayPropertyList(int overwrite0_append1/*=0*/,DWORD dwPropGroup/*=1*/)
{
	CItemInfo* lpInfo=NULL;
	CXhChar100 sText;
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pLeafItem=NULL,*pRootItem=GetRootItem();

	if(overwrite0_append1==0)
	{
		CleanTree();
		m_nObjClassTypeId = 0;
	}

	//װ�䷽λ
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Assemble Position";
	lpInfo->m_strPropHelp = "Part's assemble position and assemble direction.";
#else 
	lpInfo->m_strPropName = "װ�䷽λ";
	lpInfo->m_strPropHelp = "������װ��λ�ü�װ��ʱ��װ�䷽��";
#endif
	pGroupItem = InsertItem(pRootItem,lpInfo, -1);
	pGroupItem->m_idProp = GetPropID("ucs");

	//װ��ƽ��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbItems=CParaCS::GetDatumPlaneCmbItems();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Datum Plane";
	lpInfo->m_strPropHelp = "Specify two axis direction��automatically calculate vertical and curent direction,correct 2nd axis direction and 1st is invariable��Such as:X-Y plane,first calculate z direction by x&y direction,then calculate y direction by x&y direction.";
#else 
	lpInfo->m_strPropName = "��׼��";
	lpInfo->m_strPropHelp = "ָ�������᷽���Զ��������ֱ�뵱ǰ��ķ��򣬵�һ���᷽�򲻱������ڶ����᷽����:X-Y��,����X��Y�����Z�᷽����X��Z������Y�᷽��";
#endif
	pLeafItem = InsertItem(pGroupItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("m_eDatumPlane");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;

	//װ��λ��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Position";
	lpInfo->m_strPropHelp = "Assemble datum point's position.";
#else 
	lpInfo->m_strPropName = "λ��";
	lpInfo->m_strPropHelp = "װ������λ�á�";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("ucs.origin");
	pPropItem->m_bHideChildren = FALSE;

	//װ������ϵԭ��λ��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "The origion point's x coord on assemble CSYS.";
#else 
	lpInfo->m_strPropName = "X����";
	lpInfo->m_strPropHelp = "װ������ϵԭ��λ��X���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.origin.x");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵԭ��λ��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "The origion point's y coord on assemble CSYS.";
#else 
	lpInfo->m_strPropName = "Y����";
	lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Y���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.origin.y");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵԭ��λ��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "The origion point's z coord on assemble CSYS.";
#else 
	lpInfo->m_strPropName = "Z����";
	lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Z���������";
#endif
	pLeafItem= InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.origin.z");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ��ʱX�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Axis Direction";
	lpInfo->m_strPropHelp = "The x axis direction when assembing";
#else 
	lpInfo->m_strPropName = "X�᷽��";
	lpInfo->m_strPropHelp = "װ��ʱX�᷽��";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("ucs.axis_x");
	pPropItem->m_bHideChildren = TRUE;
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//װ������ϵX��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "The x coordinate component of x axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "װ������ϵX��X���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_x.x");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵX��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "The y coordinate component of x axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "װ������ϵX��Y���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_x.y");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵX��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "The z coordinate component of x axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "װ������ϵX��Z���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_x.z");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ��ʱY�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Axis Direction";
	lpInfo->m_strPropHelp = "The y axis direction when assembing";
#else 
	lpInfo->m_strPropName = "Y�᷽��";
	lpInfo->m_strPropHelp = "װ��ʱY�᷽��";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("ucs.axis_y");
	pPropItem->m_bHideChildren = TRUE;
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	
	//װ������ϵY��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "The x coordinate component of y axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "װ������ϵY��X���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_y.x");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵY��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "The y coordinate component of y axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "װ������ϵY��Y���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_y.y");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵY��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "The z coordinate component of y axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "װ������ϵY��Z���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_y.z");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ��ʱZ�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Axis Direction";
	lpInfo->m_strPropHelp = "The z axis direction when assembing";
#else 
	lpInfo->m_strPropName = "Z�᷽��";
	lpInfo->m_strPropHelp = "װ��ʱZ�᷽��";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("ucs.axis_z");
	pPropItem->m_bHideChildren = TRUE;
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//װ������ϵZ��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "The x coordinate component of z axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "װ������ϵZ��X���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_z.x");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵZ��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "The y coordinate component of z axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "װ������ϵZ��Y���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_z.y");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵZ��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "The z coordinate component of z axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "װ������ϵZ��Z���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_z.z");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();

	Redraw();
}
bool CParaCSPropertyList::ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt)
{
	CPropTreeItem *pFindItem=NULL;
	char sText[MAX_TEMP_CHAR_100+1]="";
	CCallBackDialog* pParent=(CCallBackDialog*)GetParent();
	if(pItem->m_idProp==GetPropID("ucs"))
	{
		CPartUcsDlg part_ucs_dlg;
		part_ucs_dlg.ucs=paracs.GetACS();
		part_ucs_dlg.DoModal();
		//paracs.SetACS()
	}
	else if(pItem->m_idProp==GetPropID("ucs.origin"))
	{	
		//pViewDlg->m_iCurDesParaType=3;
		pParent->ExecuteCommand(this,pItem->m_idProp);
	}
	else if(pItem->m_idProp==GetPropID("ucs.axis_x"))
	{
		if(paracs.IsDesAxisX())
		{
			//pViewDlg->m_iCurDesParaType=0;
			pParent->ExecuteCommand(this,pItem->m_idProp);
		}
		else
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "X Axis Direction";
#else 
			vecDlg.m_sCaption = "X�᷽��";
#endif
			GECS cs=paracs.GetACS();
			vecDlg.m_fVectorX = cs.axis_x.x;
			vecDlg.m_fVectorY = cs.axis_x.y;
			vecDlg.m_fVectorZ = cs.axis_x.z;
			vecDlg.m_bReadOnlyX=TRUE;
			vecDlg.m_bReadOnlyY=TRUE;
			vecDlg.m_bReadOnlyZ=TRUE;
			vecDlg.DoModal();
		}
	}
	else if(pItem->m_idProp==GetPropID("ucs.axis_y"))
	{
		if(paracs.IsDesAxisY())
		{
			//pViewDlg->m_iCurDesParaType=1;
			pParent->ExecuteCommand(this,pItem->m_idProp);
		}
		else
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Y Axis Direction";
#else 
			vecDlg.m_sCaption = "Y�᷽��";
#endif
			GECS cs=paracs.GetACS();
			vecDlg.m_fVectorX = cs.axis_y.x;
			vecDlg.m_fVectorY = cs.axis_y.y;
			vecDlg.m_fVectorZ = cs.axis_y.z;
			vecDlg.m_bReadOnlyX=TRUE;
			vecDlg.m_bReadOnlyY=TRUE;
			vecDlg.m_bReadOnlyZ=TRUE;
			vecDlg.DoModal();
		}	
	}
	else if(pItem->m_idProp==GetPropID("ucs.axis_z"))
	{
		if(paracs.IsDesAxisZ())
		{
			//pViewDlg->m_iCurDesParaType=2;
			pParent->ExecuteCommand(this,pItem->m_idProp);
		}
		else
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Z Axis Direction";
#else 
			vecDlg.m_sCaption = "Z�᷽��";
#endif
			GECS cs=paracs.GetACS();
			vecDlg.m_fVectorX = cs.axis_z.x;
			vecDlg.m_fVectorY = cs.axis_z.y;
			vecDlg.m_fVectorZ = cs.axis_z.z;
			vecDlg.m_bReadOnlyX=TRUE;
			vecDlg.m_bReadOnlyY=TRUE;
			vecDlg.m_bReadOnlyZ=TRUE;
			vecDlg.DoModal();
		}	
	}
	else
		return false;
	return true;
}
//////////////////////////////////////////////////////////////////////
// CDefinedViewPropertyList Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool CDefinedViewPropertyList::ModifyValueFunc(CPropTreeItem* pItem,CString& valueStr,BOOL *lpRslt)
{
	if(CParaCSPropertyList::ModifyValueFunc(pItem,valueStr,lpRslt))
		return true;
	else if(pItem->m_idProp==GetPropID("sName"))
		strcpy(m_udfView.name,valueStr);
	else if(pItem->m_idProp==GetPropID("folderName"))
		strcpy(m_udfView.folderName,valueStr);
	else if(pItem->m_idProp==GetPropID("layerName"))
		strcpy(m_udfView.defaultLayer,valueStr);
	else
		return false;
	if(lpRslt!=NULL)
		*lpRslt=TRUE;
	return true;
}
long CDefinedViewPropertyList::InitPropHashtable(long initId/*=1*/,long initHashSize/*=50*/,long initStatusHashSize/*=0*/)	//������һ����Id
{
	long id=CParaCSPropertyList::InitPropHashtable(initId,initHashSize,initStatusHashSize);
	SetPropKey("basicInfo",id++);
	SetPropKey("sName",id++);
	SetPropKey("folderName",id++);
	SetPropKey("layerName",id++);
	return id;
}
long CDefinedViewPropertyList::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	long slen=0;
	if((slen=CParaCSPropertyList::GetPropValueStr(id,valueStr))>0)
		return slen;
	else if(GetPropID("basicInfo")==id)
		sText.Copy("");
	else if(GetPropID("sName")==id)
		sText.Copy(m_udfView.name);
	else if(GetPropID("folderName")==id)
		sText.Copy(m_udfView.folderName);
	else if(GetPropID("layerName")==id)
		sText.Copy(m_udfView.defaultLayer);
	else
		return false;
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
void CDefinedViewPropertyList::CopyFrom(CDisplayView* pUdfView)
{
	m_udfView.SetBelongModel(pUdfView->BelongModel());
	m_udfView.handle=pUdfView->handle;
	m_udfView.name[50]=0;
	strncpy(m_udfView.name,pUdfView->name,50);
	m_udfView.folderName[16]=0;
	strncpy(m_udfView.folderName,pUdfView->folderName,16);
	m_udfView.defaultLayer[3]=0;
	strncpy(m_udfView.defaultLayer,pUdfView->defaultLayer,3);
	pUdfView->des_cs.SetACS(pUdfView->ucs,false);
	CParaCSPropertyList::CopyFrom(&pUdfView->des_cs);
}
void CDefinedViewPropertyList::CopyTo(CDisplayView* pUdfView)
{
	//m_udfView.SetBelongModel(pUdfView->BelongModel());
	//m_udfView.handle=pUdfView->handle;
	pUdfView->name[50]=0;
	strncpy(pUdfView->name,m_udfView.name,50);
	pUdfView->folderName[16]=0;
	strncpy(pUdfView->folderName,m_udfView.folderName,16);
	pUdfView->defaultLayer[3]=0;
	strncpy(pUdfView->defaultLayer,m_udfView.defaultLayer,3);
	CParaCSPropertyList::CopyTo(&pUdfView->des_cs);
	pUdfView->ucs=pUdfView->des_cs.GetACS();
}
void CDefinedViewPropertyList::DisplayPropertyList(int overwrite0_append1/*=0*/,DWORD dwPropGroup/*=1*/)
{
	CItemInfo* lpInfo=NULL;
	CXhChar100 sText;
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pLeafItem=NULL,*pRootItem=GetRootItem();

	if(overwrite0_append1==0)
	{
		CleanTree();
		m_nObjClassTypeId = 0;
	}

	//��ͼ������Ϣ
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "View Basic Information";
	lpInfo->m_strPropHelp = "View Basic Information";
#else 
	lpInfo->m_strPropName = "��ͼ������Ϣ";
	lpInfo->m_strPropHelp = "��ͼ������Ϣ";
#endif
	pGroupItem = InsertItem(pRootItem,lpInfo, -1);
	pGroupItem->m_idProp = GetPropID("basicInfo");
	//
	//����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Name";
	lpInfo->m_strPropHelp = "Custom View Name";
#else 
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "������ͼ����";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("sName");
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Group";
	lpInfo->m_strPropHelp = "Custom View Group";
#else 
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "������ͼ����";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("folderName");
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//ͼ��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Layer";
	lpInfo->m_strPropHelp = "Custom View Layer";
#else 
	lpInfo->m_strPropName = "ͼ��";
	lpInfo->m_strPropHelp = "������ͼͼ��";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("layerName");
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	
	//װ�䷽λ
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Assemble Position";
	lpInfo->m_strPropHelp = "Part's assemble position and assemble direction.";
#else 
	lpInfo->m_strPropName = "װ�䷽λ";
	lpInfo->m_strPropHelp = "������װ��λ�ü�װ��ʱ��װ�䷽��";
#endif
	pGroupItem = InsertItem(pRootItem,lpInfo, -1);
	pGroupItem->m_idProp = GetPropID("ucs");

	//װ��ƽ��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbItems=CParaCS::GetDatumPlaneCmbItems();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Datum Plane";
	lpInfo->m_strPropHelp = "Specify two axis direction��automatically calculate vertical and curent direction,correct 2nd axis direction and 1st is invariable��Such as:X-Y plane,first calculate z direction by x&y direction,then calculate y direction by x&y direction.";
#else 
	lpInfo->m_strPropName = "��׼��";
	lpInfo->m_strPropHelp = "ָ�������᷽���Զ��������ֱ�뵱ǰ��ķ��򣬵�һ���᷽�򲻱������ڶ����᷽����:X-Y��,����X��Y�����Z�᷽����X��Z������Y�᷽��";
#endif
	pLeafItem = InsertItem(pGroupItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("m_eDatumPlane");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;

	//װ��λ��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Position";
	lpInfo->m_strPropHelp = "Assemble datum point's position.";
#else 
	lpInfo->m_strPropName = "λ��";
	lpInfo->m_strPropHelp = "װ������λ�á�";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("ucs.origin");
	if(GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = GetPropStatus(pPropItem->m_idProp);
	else
		pPropItem->m_bHideChildren = TRUE;

	//װ������ϵԭ��λ��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "The origion point's x coord on assemble CSYS.";
#else 
	lpInfo->m_strPropName = "X����";
	lpInfo->m_strPropHelp = "װ������ϵԭ��λ��X���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.origin.x");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵԭ��λ��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "The origion point's y coord on assemble CSYS.";
#else 
	lpInfo->m_strPropName = "Y����";
	lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Y���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.origin.y");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵԭ��λ��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "The origion point's z coord on assemble CSYS.";
#else 
	lpInfo->m_strPropName = "Z����";
	lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Z���������";
#endif
	pLeafItem= InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.origin.z");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ��ʱX�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Axis Direction";
	lpInfo->m_strPropHelp = "The x axis direction when assembing";
#else 
	lpInfo->m_strPropName = "X�᷽��";
	lpInfo->m_strPropHelp = "װ��ʱX�᷽��";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("ucs.axis_x");
	if(GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = GetPropStatus(pPropItem->m_idProp);
	else
		pPropItem->m_bHideChildren = TRUE;
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//װ������ϵX��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "The x coordinate component of x axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "װ������ϵX��X���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_x.x");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵX��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "The y coordinate component of x axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "װ������ϵX��Y���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_x.y");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵX��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "The z coordinate component of x axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "װ������ϵX��Z���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_x.z");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ��ʱY�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Axis Direction";
	lpInfo->m_strPropHelp = "The y axis direction when assembing";
#else 
	lpInfo->m_strPropName = "Y�᷽��";
	lpInfo->m_strPropHelp = "װ��ʱY�᷽��";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("ucs.axis_y");
	if(GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = GetPropStatus(pPropItem->m_idProp);
	else
		pPropItem->m_bHideChildren = TRUE;
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	
	//װ������ϵY��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "The x coordinate component of y axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "װ������ϵY��X���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_y.x");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵY��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "The y coordinate component of y axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "װ������ϵY��Y���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_y.y");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵY��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "The z coordinate component of y axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "װ������ϵY��Z���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_y.z");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ��ʱZ�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Axis Direction";
	lpInfo->m_strPropHelp = "The z axis direction when assembing";
#else 
	lpInfo->m_strPropName = "Z�᷽��";
	lpInfo->m_strPropHelp = "װ��ʱZ�᷽��";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("ucs.axis_z");
	if(GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = GetPropStatus(pPropItem->m_idProp);
	else
		pPropItem->m_bHideChildren = TRUE;
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//װ������ϵZ��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "The x coordinate component of z axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "װ������ϵZ��X���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_z.x");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵZ��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "The y coordinate component of z axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "װ������ϵZ��Y���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_z.y");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵZ��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "The z coordinate component of z axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "װ������ϵZ��Z���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_z.z");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();

	Redraw();
}
//////////////////////////////////////////////////////////////////////
// CSectionViewPropertyList Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool CSectionViewPropertyList::ModifyDatumPointPropValue(CPropTreeItem *pItem, CString &valueStr)
{
	IModel* pModel=NULL;
	CLDSPartGroup* pPartGroup=m_xSonView.BelongPartGroup();
	if(pPartGroup)
		pModel=pPartGroup->BelongModel();
	CDrawingAssistant* pAssist=m_xSonView.hashAssistPlanes.GetFirst();
	if(pAssist==NULL||pModel==NULL)
		return false;
	CLDSPoint *pStartPoint=&pAssist->m_xLineStart;
	char value_str[100]="";
	if(GetPropID("m_xLineStart.x")==pItem->m_idProp)
		pStartPoint->SetPositionX(atof(valueStr));
	else if(GetPropID("m_xLineStart.y")==pItem->m_idProp)
		pStartPoint->SetPositionY(atof(valueStr));
	else if(GetPropID("m_xLineStart.z")==pItem->m_idProp)
		pStartPoint->SetPositionZ(atof(valueStr));
	else if(GetPropID("m_xLineStart.des_para.RODEND.hRod")==pItem->m_idProp)	//�Ǹ־��
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.RODEND.hRod));
	else if(GetPropID("m_xLineStart.des_para.RODEND.direction")==pItem->m_idProp)	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
		pStartPoint->des_para.RODEND.direction=valueStr[0]-'0';
	else if(GetPropID("m_xLineStart.des_para.RODEND.offset_wing")==pItem->m_idProp)//0.X֫;1.Y֫
		pStartPoint->des_para.RODEND.offset_wing=valueStr[0]-'0';
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(GetPropID("m_xLineStart.des_para.RODEND.wing_offset_style")==pItem->m_idProp)	
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
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pModel->FromPartHandle(pStartPoint->des_para.RODEND.hRod,CLS_LINEANGLE);
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
		long idProp=GetPropID("m_xLineStart.des_para.RODEND.wing_offset_dist");
		if(GetPropValueStr(idProp,value_str))
			SetItemPropValue(idProp,CString(value_str));
		SetItemReadOnly(idProp,pStartPoint->des_para.RODEND.wing_offset_style!=4);
	}
#endif
	else if(GetPropID("m_xLineStart.des_para.RODEND.wing_offset_dist")==pItem->m_idProp)
		pStartPoint->des_para.RODEND.wing_offset_dist=atof(valueStr);
	else if(GetPropID("m_xLineStart.des_para.RODEND.len_offset_dist")==pItem->m_idProp)
		pStartPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
	else if(GetPropID("m_xLineStart.des_para.RODEND.bIncOddEffect")==pItem->m_idProp)		
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
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.hLinePart")==pItem->m_idProp)	//�Ǹ־��
		sscanf(valueStr,"%X",&(pStartPoint->des_para.RODNODE.hRod));
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.hNode")==pItem->m_idProp)	//��׼�ڵ���
		sscanf(valueStr,"%X",&(pStartPoint->des_para.RODNODE.hNode));
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.direction")==pItem->m_idProp)	
		pStartPoint->des_para.RODNODE.direction=valueStr[0]-'0';
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.offset_wing")==pItem->m_idProp)	
		pStartPoint->des_para.RODNODE.offset_wing=valueStr[0]-'0';
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_style")==pItem->m_idProp)
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
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pModel->FromPartHandle(pStartPoint->des_para.RODNODE.hRod,CLS_LINEANGLE);
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
		long idProp=GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_dist");
		if(GetPropValueStr(idProp,value_str))
			SetItemPropValue(idProp,CString(value_str));
		SetItemReadOnly(idProp,pStartPoint->des_para.RODNODE.wing_offset_style!=4);
	}	
#endif
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_dist")==pItem->m_idProp)
		pStartPoint->des_para.RODNODE.wing_offset_dist=atof(valueStr);
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.len_offset_dist")==pItem->m_idProp)	
		pStartPoint->des_para.RODNODE.len_offset_dist=atof(valueStr);
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.bFaceOffset")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else 
		if(valueStr.CompareNoCase("��")==0)
#endif
			pStartPoint->des_para.RODNODE.bFaceOffset=TRUE;
		else 
			pStartPoint->des_para.RODNODE.bFaceOffset=FALSE;
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]"));
		if(pStartPoint->des_para.RODNODE.bFaceOffset)
		{	//ƽ���淨��
			CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL,*pRootItem=GetRootItem();
			//
			CItemInfo *lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
			lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Offset Plane Normal";
			lpInfo->m_strPropHelp="Offset plane's normal line";
#else 
			lpInfo->m_strPropName="ƽ���淨��";
			lpInfo->m_strPropHelp="ƽ���淨��";
#endif
			pPropItem = InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset");
			if(GetPropValueStr(pPropItem->m_idProp,value_str))
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
			pSonPropItem = InsertItem(pPropItem,lpInfo,0,TRUE);
			pSonPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]");
			if(GetPropValueStr(pSonPropItem->m_idProp,value_str))
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
			pSonPropItem = InsertItem(pPropItem,lpInfo,1,TRUE);
			pSonPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]");
			if(GetPropValueStr(pSonPropItem->m_idProp,value_str))
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
			pSonPropItem = InsertItem(pPropItem,lpInfo,2,TRUE);
			pSonPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]");
			if(GetPropValueStr(pSonPropItem->m_idProp,value_str))
				pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
			pSonPropItem->SetReadOnly(vectorType!=0);
		}
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset")==pItem->m_idProp)
	{
		f3dPoint norm=CStdVector::GetVector(valueStr[0]-'0');
		pStartPoint->des_para.RODNODE.vPlaneNormal=norm;
		long idProp=GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]");
		SetItemPropValue(idProp,"%.f",pStartPoint->des_para.RODNODE.vPlaneNormal.x);
		SetItemReadOnly(idProp,valueStr[0]!='0');
		idProp=GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]");
		SetItemPropValue(idProp,"%.f",pStartPoint->des_para.RODNODE.vPlaneNormal.y);
		SetItemReadOnly(idProp,valueStr[0]!='0');
		idProp=GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]");
		SetItemPropValue(idProp,"%.f",pStartPoint->des_para.RODNODE.vPlaneNormal.z);
		SetItemReadOnly(idProp,valueStr[0]!='0');
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]")==pItem->m_idProp)
		pStartPoint->des_para.RODNODE.vPlaneNormal.x=atof(valueStr);
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]")==pItem->m_idProp)
		pStartPoint->des_para.RODNODE.vPlaneNormal.y=atof(valueStr);
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]")==pItem->m_idProp)
		pStartPoint->des_para.RODNODE.vPlaneNormal.z=atof(valueStr);
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.AXIS_INTERS.hDatum1));
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.AXIS_INTERS.hDatum2));
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style1")==pItem->m_idProp)
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
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pModel->FromPartHandle(pStartPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE);
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
		long idProp=GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist1");
		if(GetPropValueStr(idProp,value_str))
			SetItemPropValue(idProp,CString(value_str));
		SetItemReadOnly(idProp,pStartPoint->des_para.AXIS_INTERS.wing_offset_style1!=4);
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style2")==pItem->m_idProp)
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
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pModel->FromPartHandle(pStartPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
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
		long idProp=GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist2");
		if(GetPropValueStr(idProp,value_str))
			SetItemPropValue(idProp,CString(value_str));
		SetItemReadOnly(idProp,pStartPoint->des_para.AXIS_INTERS.wing_offset_style2!=4);
	}
#endif
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist1")==pItem->m_idProp)
		pStartPoint->des_para.AXIS_INTERS.wing_offset_dist1=atol(valueStr);
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist2")==pItem->m_idProp)
		pStartPoint->des_para.AXIS_INTERS.wing_offset_dist2=atol(valueStr);
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector")==pItem->m_idProp)
	{
		f3dPoint vec=CStdVector::GetVector(valueStr[0]-'0');
		pStartPoint->des_para.AXIS_INTERS.near_vector[0]=vec.x;
		pStartPoint->des_para.AXIS_INTERS.near_vector[1]=vec.y;
		pStartPoint->des_para.AXIS_INTERS.near_vector[2]=vec.z;
		long idProp=GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[0]");
		SetItemPropValue(idProp,"%.f",pStartPoint->des_para.AXIS_INTERS.near_vector[0]);
		SetItemReadOnly(idProp,valueStr[0]!='0');
		idProp=GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[1]");
		SetItemPropValue(idProp,"%.f",pStartPoint->des_para.AXIS_INTERS.near_vector[1]);
		SetItemReadOnly(idProp,valueStr[0]!='0');
		idProp=GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[2]");
		SetItemPropValue(idProp,"%.f",pStartPoint->des_para.AXIS_INTERS.near_vector[2]);
		SetItemReadOnly(idProp,valueStr[0]!='0');
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[0]")==pItem->m_idProp)
		pStartPoint->des_para.AXIS_INTERS.near_vector[0]=atof(valueStr);
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[1]")==pItem->m_idProp)
		pStartPoint->des_para.AXIS_INTERS.near_vector[1]=atof(valueStr);
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[2]")==pItem->m_idProp)
		pStartPoint->des_para.AXIS_INTERS.near_vector[2]=atof(valueStr);
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.AXIS_INTERS.hDatum1));
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.AXIS_INTERS.hDatum2));
	else if(GetPropID("m_xLineStart.des_para.DATUMLINE.hDatumLine")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.DATUMLINE.hDatumLine));
	else if(GetPropID("m_xLineStart.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==pItem->m_idProp)
		pStartPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=valueStr[0]-'0';
	else if(GetPropID("m_xLineStart.des_para.DATUMPLANE.hDatumPlane")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.DATUMPLANE.hDatumPlane));
	else if(GetPropID("m_xLineStart.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==pItem->m_idProp)
		pStartPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=valueStr[0]-'0';
	else if(GetPropID("m_xLineStart.des_para.RODEND.hRod")==pItem->m_idProp)	//�Ǹ־��
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.RODEND.hRod));
	else if(GetPropID("m_xLineStart.des_para.RODEND.direction")==pItem->m_idProp)	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
		pStartPoint->des_para.RODEND.direction=valueStr[0]-'0';
	else if(GetPropID("m_xLineStart.des_para.RODEND.len_offset_dist")==pItem->m_idProp)
		pStartPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
	else if(GetPropID("m_xLineStart.des_para.hNode")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.hNode));
	else if(GetPropID("m_xLineStart.des_para.hPart")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.hPart));
	else if(GetPropID("m_xLineStart.datum_pos.x")==pItem->m_idProp)
		pStartPoint->datum_pos.x=atof(valueStr);
	else if(GetPropID("datum_pos.y")==pItem->m_idProp)
		pStartPoint->datum_pos.y=atof(valueStr);
	else if(GetPropID("m_xLineStart.datum_pos.z")==pItem->m_idProp)
		pStartPoint->datum_pos.z=atof(valueStr);
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.hLinePart")==pItem->m_idProp)//��׼�˼����
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.SCALE_NODE.hLinePart));
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.hStartNode")==pItem->m_idProp)	//ʼ�˽ڵ���
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.SCALE_NODE.hStartNode));
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.hEndNode")==pItem->m_idProp)		//�ն˽ڵ���
		sscanf(valueStr.GetBuffer(1),"%X",&(pStartPoint->des_para.SCALE_NODE.hEndNode));
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.start_offset_dist")==pItem->m_idProp)//ʼ��ƫ����
		pStartPoint->des_para.SCALE_NODE.start_offset_dist=atof(valueStr);
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.end_offset_dist")==pItem->m_idProp)	//�ն�ƫ����
		pStartPoint->des_para.SCALE_NODE.end_offset_dist=atof(valueStr);
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.offset_scale")==pItem->m_idProp)		//ƫ�Ʊ���
		pStartPoint->des_para.SCALE_NODE.offset_scale=atof(valueStr);
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.offset_dist")==pItem->m_idProp)		//�ڵȷֻ����ϵ�ƫ��ֵ
		pStartPoint->des_para.SCALE_NODE.offset_dist=atof(valueStr);
	else 
		return false;
	if(pStartPoint->datum_pos_style!=0)
	{
		pStartPoint->UpdatePos(pModel);
		long idProp=GetPropID("m_xLineStart.x");
		if(GetPropValueStr(idProp,value_str))
			SetItemPropValue(idProp,value_str);
		idProp=GetPropID("m_xLineStart.y");
		if(GetPropValueStr(idProp,value_str))
			SetItemPropValue(idProp,value_str);
		idProp=GetPropID("m_xLineStart.z");
		if(GetPropValueStr(idProp,value_str))
			SetItemPropValue(idProp,value_str);
	}
	return true;
}

bool CSectionViewPropertyList::ModifyDatumVectorPropValue(CPropTreeItem *pItem, CString &valueStr)
{
	IModel* pModel=NULL;
	CLDSPartGroup* pPartGroup=m_xSonView.BelongPartGroup();
	if(pPartGroup)
		pModel=pPartGroup->BelongModel();
	CDrawingAssistant* pAssist=m_xSonView.hashAssistPlanes.GetFirst();
	if(pAssist==NULL||pModel==NULL)
		return false;
	CLDSVector *pVector=&pAssist->m_xFaceNorm;
	if(GetPropID("m_xFaceNorm.hVicePart")==pItem->m_idProp)//��׼�˼����
		sscanf(valueStr,"%d",&(pVector->hVicePart));
	else if(GetPropID("m_xFaceNorm.hCrossPart")==pItem->m_idProp)
		sscanf(valueStr,"%d",&(pVector->hCrossPart));
	else if(GetPropID("m_xFaceNorm.norm_wing")==pItem->m_idProp)
		pVector->norm_wing=valueStr[0]-'0';
	else if(GetPropID("m_xFaceNorm.direction")==pItem->m_idProp)
	{
		pVector->direction=valueStr[0]-'0';
		if(pVector->norm_style==5)
		{
			CSuperSmartPtr<CLDSPart>pDatumPart=pModel->FromPartHandle(pVector->hVicePart);
			if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
			{
				if(pVector->direction<2)
					DeleteItemByPropId(GetPropID("m_xFaceNorm.rot_ang"));
				else
				{
					int idProp=GetPropID("m_xFaceNorm.rot_ang");
					if(FindItemByPropId(idProp,NULL)==NULL)
					{
						CItemInfo* lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_COMBO;
						lpInfo->m_cmbType=CDT_EDIT;
						lpInfo->m_cmbItems = "0|90|180|270";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Rotate Angle";
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
						CPropTreeItem* pSonItem = InsertItem(pItem,lpInfo, -1,TRUE);
						pSonItem->m_idProp = idProp;
						char item_text[100]="";
						if(GetPropValueStr(pSonItem->m_idProp,item_text))
							pSonItem->m_lpNodeInfo->m_strPropValue = item_text;
					}
				}
			}
		}
	}
	else if(GetPropID("m_xFaceNorm.rot_ang")==pItem->m_idProp)
	{
		pVector->rot_ang = atof(valueStr);
	}
	else if(GetPropID("m_xFaceNorm.VectorType")==pItem->m_idProp)
	{
		if(valueStr[0]=='0')	//�Զ��巽��
		{
			SetItemReadOnly(GetPropID("m_xFaceNorm.norm.x"),pVector->norm_style!=0);
			SetItemReadOnly(GetPropID("m_xFaceNorm.norm.y"),pVector->norm_style!=0);
			SetItemReadOnly(GetPropID("m_xFaceNorm.norm.z"),pVector->norm_style!=0);
		}
		else
		{
			SetItemReadOnly(GetPropID("m_xFaceNorm.norm.x"),TRUE);
			SetItemReadOnly(GetPropID("m_xFaceNorm.norm.y"),TRUE);
			SetItemReadOnly(GetPropID("m_xFaceNorm.norm.z"),TRUE);
			f3dPoint vector=CStdVector::GetVector(valueStr[0]-'0');
			pVector->vector=vector;
			CString sText;
			sText.Format("%.f",vector.x);
			SetItemPropValue(GetPropID("m_xFaceNorm.norm.x"),sText);
			sText.Format("%.f",vector.y);
			SetItemPropValue(GetPropID("m_xFaceNorm.norm.y"),sText);
			sText.Format("%.f",vector.z);
			SetItemPropValue(GetPropID("m_xFaceNorm.norm.z"),sText);
		}
	}
	else if(GetPropID("m_xFaceNorm.nearVectorType")==pItem->m_idProp)
	{
		if(valueStr[0]-'0'==0)	//�Զ��巽��
		{
			CXhChar100 sText;
			CItemInfo *lpInfo;
			CPropTreeItem *pSonItem,*pFindItem=FindItemByPropId(GetPropID("m_xFaceNorm.nearVector.x"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "X";
				lpInfo->m_strPropHelp = "X Component";
#else 
				lpInfo->m_strPropName = "X����";
				lpInfo->m_strPropHelp = "X����";
#endif
				pSonItem = InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =GetPropID("m_xFaceNorm.nearVector.x");
				if(GetPropValueStr(pSonItem->m_idProp,sText))
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
			
			pFindItem=FindItemByPropId(GetPropID("m_xFaceNorm.nearVector.y"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Y";
				lpInfo->m_strPropHelp = "Y Component";
#else 
				lpInfo->m_strPropName = "Y����";
				lpInfo->m_strPropHelp = "Y����";
#endif
				pSonItem = InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =GetPropID("m_xFaceNorm.nearVector.y");
				if(GetPropValueStr(pSonItem->m_idProp,sText))
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
			
			pFindItem=FindItemByPropId(GetPropID("m_xFaceNorm.nearVector.z"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Z";
				lpInfo->m_strPropHelp = "Z Component";
#else 
				lpInfo->m_strPropName = "Z����";
				lpInfo->m_strPropHelp = "Z����";
#endif
				pSonItem = InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =GetPropID("m_xFaceNorm.nearVector.z");
				if(GetPropValueStr(pSonItem->m_idProp,sText))
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else
		{
			DeleteItemByPropId(GetPropID("m_xFaceNorm.nearVector.x"));
			DeleteItemByPropId(GetPropID("m_xFaceNorm.nearVector.y"));
			DeleteItemByPropId(GetPropID("m_xFaceNorm.nearVector.z"));
			pVector->nearVector=CStdVector::GetVector(valueStr[0]-'0');
		}
	}
	else if(GetPropID("m_xFaceNorm.nearVector.x")==pItem->m_idProp)
		pVector->nearVector.x=atof(valueStr);
	else if(GetPropID("m_xFaceNorm.nearVector.y")==pItem->m_idProp)
		pVector->nearVector.y=atof(valueStr);
	else if(GetPropID("m_xFaceNorm.nearVector.z")==pItem->m_idProp)
		pVector->nearVector.z=atof(valueStr);
	else
		return false;
	if(pVector->norm_style!=0)
	{
		char value_str[100]="";
		pVector->UpdateVector(pModel);
		long idProp=GetPropID("m_xFaceNorm.norm.x");
		if(GetPropValueStr(idProp,value_str))
			SetItemPropValue(idProp,value_str);
		idProp=GetPropID("m_xFaceNorm.norm.y");
		if(GetPropValueStr(idProp,value_str))
			SetItemPropValue(idProp,value_str);
		idProp=GetPropID("m_xFaceNorm.norm.z");
		if(GetPropValueStr(idProp,value_str))
			SetItemPropValue(idProp,value_str);
	}
	return true;
}

bool CSectionViewPropertyList::ModifyValueFunc(CPropTreeItem* pItem,CString& valueStr,BOOL *lpRslt)
{
	if(CParaCSPropertyList::ModifyValueFunc(pItem,valueStr,lpRslt))
		return true;
	else if(pItem->m_idProp==GetPropID("sName"))
		strcpy(m_xSonView.sName,valueStr);
	else if(pItem->m_idProp==GetPropID("m_bLocked"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Yes")==0)
#else 
		if(valueStr.Compare("��")==0)
#endif
			m_xSonView.m_bLocked=TRUE;
		else
			m_xSonView.m_bLocked=FALSE;
	}
	else if(pItem->m_idProp==GetPropID("m_iViewType"))
		m_xSonView.m_iViewType=(SECTION_VIEW_TYPE)(valueStr[0]-'0');
	else if(m_xSonView.GetViewTypeId()==CSectionView::CLS_SLAVE_VIEW)
	{
		CSlaveSectionView *pSlaveView=(CSlaveSectionView*)&m_xSonView;
		if(GetPropID("m_idHostView")==pItem->m_idProp)
		{
			int nIdStartPos=valueStr.ReverseFind(':')+1;
			CString subStr=valueStr.Mid(nIdStartPos,valueStr.GetLength()-1-nIdStartPos);
			int idView=atoi(subStr);
			if(pSlaveView->BelongPartGroup())
			{
				CSectionView *pHostView=pSlaveView->BelongPartGroup()->SonViews.GetValue(idView);
				if(pHostView)
					pSlaveView->m_idHostView=pHostView->GetId();
			}
		}
		else if(GetPropID("direct_offset")==pItem->m_idProp)//ƫ����
			pSlaveView->direct_offset=atof(valueStr);
		else if(GetPropID("symbol_dist")==pItem->m_idProp)//����ͼ������
			pSlaveView->symbol_dist=atof(valueStr);
		else if(GetPropID("symbol")==pItem->m_idProp)//����ͼ������
			pSlaveView->symbol.Copy(valueStr.GetString());
		else if(m_xSonView.hashAssistPlanes.GetNodeNum()>0)
		{
			CDrawingAssistant* pAssist=m_xSonView.hashAssistPlanes.GetFirst();
			if(GetPropID("m_xLineStart.datum_pos_style")==pItem->m_idProp)
			{
				pAssist->m_xLineStart.datum_pos_style=valueStr[0]-'0';
				UpdateDatumPointPropItem(pItem,&pAssist->m_xLineStart,TRUE);
			}
			else if(GetPropID("m_xFaceNorm.norm_style")==pItem->m_idProp)
			{
				pAssist->m_xFaceNorm.norm_style=valueStr[0]-'0';
				UpdateDatumVectorPropItem(pItem,TRUE);
			}
			else 
			{
				bool bRetCode=ModifyDatumPointPropValue(pItem,valueStr);
				if(!bRetCode)
					bRetCode=ModifyDatumVectorPropValue(pItem,valueStr);
				return bRetCode;
			}
			return true;
		}
	}
	else
		return false;
	if(lpRslt!=NULL)
		*lpRslt=TRUE;
	return true;
}
#include "ReportPointDlg.h"
#include "ScrTools.h"
bool CSectionViewPropertyList::ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt)
{
	IModel* pModel=NULL;
	CLDSPartGroup* pPartGroup=m_xSonView.BelongPartGroup();
	if(pPartGroup)
		pModel=pPartGroup->BelongModel();
	CCallBackDialog* pParent=(CCallBackDialog*)GetParent();
	if(pModel==NULL||pParent==NULL)
		return false;
	char sText[100]="";
	BOOL bRetCode=TRUE;
	if(CParaCSPropertyList::ButtonClick(pItem,lpRslt))
		return true;
	else if(m_xSonView.hashAssistPlanes.GetNodeNum()>0)
	{
		CDrawingAssistant* pAssist=m_xSonView.hashAssistPlanes.GetFirst();
		if(pAssist==NULL)
			return false;
		CLDSPoint *pStartPoint=&pAssist->m_xLineStart;
		CLDSVector *pVector=&pAssist->m_xFaceNorm;
		if(GetPropID("m_xLineStart.pos")==pItem->m_idProp)
		{
			CReportPointDlg pointDlg;
			if(pAssist->m_nStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
				pointDlg.m_sCaption = "Spread Plane Datum Point";
			else
				pointDlg.m_sCaption = "Ancillary Line Start Point";
#else 
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
				long idProp=GetPropID("m_xLineStart.x");
				if(GetPropValueStr(idProp,sText))
					SetItemPropValue(idProp,sText);
				idProp=GetPropID("m_xLineStart.y");
				if(GetPropValueStr(idProp,sText))
					SetItemPropValue(idProp,sText);
				idProp=GetPropID("m_xLineStart.z");
				if(GetPropValueStr(idProp,sText))
					SetItemPropValue(idProp,sText);
			}
		}
		else if(GetPropID("m_xFaceNorm.norm")==pItem->m_idProp)
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
				long idProp=GetPropID("m_xFaceNorm.norm.x");
				if(GetPropValueStr(idProp,sText))
					SetItemPropValue(idProp,sText);
				idProp=GetPropID("m_xFaceNorm.norm.y");
				if(GetPropValueStr(idProp,sText))
					SetItemPropValue(idProp,sText);
				idProp=GetPropID("m_xFaceNorm.norm.z");
				if(GetPropValueStr(idProp,sText))
					SetItemPropValue(idProp,sText);	
				idProp=GetPropID("m_xFaceNorm.VectorType");
				if(GetPropValueStr(idProp,sText))
					SetItemPropValue(idProp,sText);	
			}
		}
		if(GetPropID("m_xLineStart.des_para.RODEND.hRod")==pItem->m_idProp)
			pParent->SelectObject(CLS_LINEANGLE,pItem->m_idProp);//��׼�Ǹ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(GetPropID("m_xLineStart.des_para.RODEND.norm_offset")==pItem->m_idProp)
		{	//����ƫ����

			CString sThickStr;
			sThickStr=pStartPoint->des_para.RODEND.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			_snprintf(pStartPoint->des_para.RODEND.norm_offset,49,"%s",sThickStr);
			if(GetPropValueStr(pItem->m_idProp,sText))
				SetItemPropValue(pItem->m_idProp,CString(sText));
		}
#endif
		else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.hLinePart")==pItem->m_idProp)
			pParent->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�
		else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.hNode")==pItem->m_idProp)
			pParent->SelectObject(CLS_NODE,pItem->m_idProp);		//��׼�ڵ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.norm_offset")==pItem->m_idProp)
		{	//����ƫ����
			CString sThickStr;
			sThickStr=pStartPoint->des_para.RODNODE.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			_snprintf(pStartPoint->des_para.RODNODE.norm_offset,49,"%s",sThickStr);
			if(GetPropValueStr(pItem->m_idProp,sText))
				SetItemPropValue(pItem->m_idProp,CString(sText));
		}
#endif
		else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			pParent->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�1
		else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			pParent->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�2
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.norm_offset")==pItem->m_idProp)
		{	//����ƫ����
			CString sThickStr;
			sThickStr=pStartPoint->des_para.AXIS_INTERS.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			_snprintf(pStartPoint->des_para.AXIS_INTERS.norm_offset,49,"%s",sThickStr);
			if(GetPropValueStr(pItem->m_idProp,sText))
				SetItemPropValue(pItem->m_idProp,CString(sText));
		}
#endif
		else if(GetPropID("m_xLineStart.des_para.RODEND.hRod")==pItem->m_idProp)
			pParent->SelectObject(CLS_LINEANGLE,pItem->m_idProp);//��׼�Ǹ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(GetPropID("m_xLineStart.des_para.RODEND.norm_offset")==pItem->m_idProp)
		{	//����ƫ����

			CString sThickStr;
			sThickStr=pStartPoint->des_para.RODEND.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			_snprintf(pStartPoint->des_para.RODEND.norm_offset,49,"%s",sThickStr);
			if(GetPropValueStr(pItem->m_idProp,sText))
				SetItemPropValue(pItem->m_idProp,CString(sText));
		}
#endif
		else if(GetPropID("m_xLineStart.des_para.hNode")==pItem->m_idProp)
			pParent->SelectObject(CLS_NODE,pItem->m_idProp);	//��׼�ڵ�
		else if(GetPropID("m_xLineStart.des_para.hPart")==pItem->m_idProp)
			pParent->SelectObject(CLS_PART,pItem->m_idProp);	//��׼����
		else if(GetPropID("m_xLineStart.datum_pos")==pItem->m_idProp)
		{
			CReportPointDlg pointDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			pointDlg.m_sCaption = "Relative Coordinate";
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
				SetItemPropValue(GetPropID("m_xLineStart.datum_pos.x"),CString(sText));
				sText.Printf("%f",pointDlg.m_fPointY);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("m_xLineStart.datum_pos.y"),CString(sText));
				sText.Printf("%f",pointDlg.m_fPointY);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("m_xLineStart.datum_pos.z"),CString(sText));
				pStartPoint->datum_pos.Set(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ);
			}
		}
		else if(GetPropID("m_xFaceNorm.hVicePart")==pItem->m_idProp
			||GetPropID("m_xFaceNorm.hCrossPart")==pItem->m_idProp)
		{
			if(pVector->norm_style==1)
				pParent->SelectObject(CLS_LINEANGLE,pItem->m_idProp);
			else if(pVector->norm_style==2||pVector->norm_style==5)
				pParent->SelectObject(CLS_LINEPART,pItem->m_idProp);
			else 
				pParent->SelectObject(CLS_PART,pItem->m_idProp);
		}
		else 
			return FALSE;
	}
	return TRUE;
}
BOOL CSectionViewPropertyList::FinishSelectObjOper(long hPickObj,long idEventProp)
{
	CDrawingAssistant* pAssist=m_xSonView.hashAssistPlanes.GetFirst();
	if(pAssist)
	{
		if(idEventProp==GetPropID("m_xFaceNorm.hVicePart"))
			pAssist->m_xFaceNorm.hVicePart=hPickObj;
		else if(idEventProp==GetPropID("m_xFaceNorm.hCrossPart"))
			pAssist->m_xFaceNorm.hCrossPart=hPickObj;
		else if(idEventProp==GetPropID("m_xLineStart.des_para.RODEND.hRod"))
			pAssist->m_xLineStart.des_para.RODEND.hRod=hPickObj;	//��׼�Ǹ�
		else if(idEventProp==GetPropID("m_xLineStart.des_para.LINEPARTNODE.hLinePart"))
			pAssist->m_xLineStart.des_para.RODNODE.hRod=hPickObj;	//��׼�˼�
		else if(idEventProp==GetPropID("m_xLineStart.des_para.LINEPARTNODE.hNode"))
			pAssist->m_xLineStart.des_para.RODNODE.hNode=hPickObj;	//��׼�ڵ�
		else if(idEventProp==GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1"))
			pAssist->m_xLineStart.des_para.AXIS_INTERS.hDatum1=hPickObj;	//��׼�˼�
		else if(idEventProp==GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2"))
			pAssist->m_xLineStart.des_para.AXIS_INTERS.hDatum2=hPickObj;	//��׼�˼�
		else
			return FALSE;
		return TRUE;
	}
	return FALSE;
}
long CSectionViewPropertyList::InitPropHashtable(long initId/*=1*/,long initHashSize/*=50*/,long initStatusHashSize/*=0*/)	//������һ����Id
{
	long id=CParaCSPropertyList::InitPropHashtable(initId,initHashSize,initStatusHashSize);
	SetPropKey("basicInfo",id++);
	SetPropKey("sName",id++);
	SetPropKey("m_bLocked",id++);
	SetPropKey("m_iViewType",id++);
	SetPropKey("m_idHostView",id++);
	SetPropKey("slaveViewPara",id++);
	SetPropKey("symbol",id++);
	SetPropKey("location",id++);
	SetPropKey("location.x",id++);
	SetPropKey("location.y",id++);
	SetPropKey("location.z",id++);
	SetPropKey("direction",id++);
	SetPropKey("direction.x",id++);
	SetPropKey("direction.y",id++);
	SetPropKey("direction.z",id++);
	SetPropKey("direct_offset",id++);
	SetPropKey("symbol_list",id++);

	//������ʼ��(չ���߻�׼��)
	SetPropKey("m_xLineStart",id++);
	SetPropKey("m_xLineStart.datum_pos_style",id++);
	SetPropKey("m_xLineStart.pos",id++);
	SetPropKey("m_xLineStart.x",id++);
	SetPropKey("m_xLineStart.y",id++);
	SetPropKey("m_xLineStart.z",id++);
	//��ڵ�ͬλ
	SetPropKey("m_xLineStart.des_para.hNode",id++);
	//ָ�������ϵ��������
	SetPropKey("m_xLineStart.des_para.hPart",id++);
	SetPropKey("m_xLineStart.datum_pos",id++);
	SetPropKey("m_xLineStart.datum_pos.x",id++);
	SetPropKey("m_xLineStart.datum_pos.y",id++);
	SetPropKey("m_xLineStart.datum_pos.z",id++);
	//���߽���
	SetPropKey("m_xLineStart.des_para.AXIS_INTERS.hDatum1",id++);
	SetPropKey("m_xLineStart.des_para.AXIS_INTERS.hDatum2",id++);
	SetPropKey("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style1",id++);
	SetPropKey("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style2",id++);//0.g;1.g1;2.g2;3.g3;4.�Զ���
	SetPropKey("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist1",id++);
	SetPropKey("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist2",id++);
	SetPropKey("m_xLineStart.des_para.AXIS_INTERS.near_vector",id++);
	SetPropKey("m_xLineStart.des_para.AXIS_INTERS.near_vector[0]",id++);
	SetPropKey("m_xLineStart.des_para.AXIS_INTERS.near_vector[1]",id++);
	SetPropKey("m_xLineStart.des_para.AXIS_INTERS.near_vector[2]",id++);
	SetPropKey("m_xLineStart.des_para.AXIS_INTERS.norm_offset",id++);	//����ƫ����;
	//�Ǹ����
	SetPropKey("m_xLineStart.des_para.RODEND.hRod",id++);				//�Ǹ־��
	SetPropKey("m_xLineStart.des_para.RODEND.direction",id++);			//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	SetPropKey("m_xLineStart.des_para.RODEND.offset_wing",id++);		//0.X֫;1.Y֫
	SetPropKey("m_xLineStart.des_para.RODEND.wing_offset_style",id++);	//0.g;1.g1;2.g2;3.g3;4.�Զ���
	SetPropKey("m_xLineStart.des_para.RODEND.wing_offset_dist",id++);
	SetPropKey("m_xLineStart.des_para.RODEND.len_offset_dist",id++);	//֫��ƫ��ֵ;����ƫ��ֵ;
	SetPropKey("m_xLineStart.des_para.RODEND.bIncOddEffect",id++);		//����ƫ��ֵ��������ͷӰ��
	SetPropKey("m_xLineStart.des_para.RODEND.norm_offset",id++);		//����ƫ����
	//�˼��Ͻڵ�Ϊ��׼
	SetPropKey("m_xLineStart.des_para.LINEPARTNODE.hLinePart",id++);	//�Ǹ־��
	SetPropKey("m_xLineStart.des_para.LINEPARTNODE.hNode",id++);		//��׼�ڵ���
	SetPropKey("m_xLineStart.des_para.LINEPARTNODE.bFaceOffset",id++);
	SetPropKey("m_xLineStart.des_para.LINEPARTNODE.direction",id++);	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	SetPropKey("m_xLineStart.des_para.LINEPARTNODE.offset_wing",id++);	//0.X֫;1.Y֫
	SetPropKey("m_xLineStart.des_para.LINEPARTNODE.wing_offset_style",id++);//0.g;1.g1;2.g2;3.g3;4.�Զ���
	SetPropKey("m_xLineStart.des_para.LINEPARTNODE.wing_offset_dist",id++);
	SetPropKey("m_xLineStart.des_para.LINEPARTNODE.len_offset_dist",id++);	//֫��ƫ��ֵ;����ƫ��ֵ;
	SetPropKey("m_xLineStart.des_para.LINEPARTNODE.norm_offset",id++);		//����ƫ����;
	SetPropKey("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset",id++);
	SetPropKey("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]",id++);
	SetPropKey("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]",id++);
	SetPropKey("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]",id++);
	//��׼�����������겻���
	SetPropKey("m_xLineStart.des_para.DATUMLINE.hDatumLine",id++);
	SetPropKey("m_xLineStart.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2",id++);
	//��׼�����������겻���
	SetPropKey("m_xLineStart.des_para.DATUMPLANE.hDatumPlane",id++);
	SetPropKey("m_xLineStart.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2",id++);
	//���ڵ������ȷֵ�
	SetPropKey("m_xLineStart.des_para.SCALE_NODE.hLinePart",id++);			//��׼�˼����
	SetPropKey("m_xLineStart.des_para.SCALE_NODE.hStartNode",id++);		//ʼ�˽ڵ���
	SetPropKey("m_xLineStart.des_para.SCALE_NODE.hEndNode",id++);			//�ն˽ڵ���
	SetPropKey("m_xLineStart.des_para.SCALE_NODE.start_offset_dist",id++);	//ʼ��ƫ����
	SetPropKey("m_xLineStart.des_para.SCALE_NODE.end_offset_dist",id++);	//�ն�ƫ����
	SetPropKey("m_xLineStart.des_para.SCALE_NODE.offset_scale",id++);		//ƫ�Ʊ���
	SetPropKey("m_xLineStart.des_para.SCALE_NODE.offset_dist",id++);		//�ڵȷֻ����ϵ�ƫ��ֵ
	//չ���淨�߷���
	SetPropKey("m_xFaceNorm",id++);
	SetPropKey("m_xFaceNorm.norm",id++);
	SetPropKey("m_xFaceNorm.norm.x",id++);
	SetPropKey("m_xFaceNorm.norm.y",id++);
	SetPropKey("m_xFaceNorm.norm.z",id++);
	SetPropKey("m_xFaceNorm.norm_style",id++);
	SetPropKey("m_xFaceNorm.hVicePart",id++);
	SetPropKey("m_xFaceNorm.hCrossPart",id++);
	SetPropKey("m_xFaceNorm.norm_wing",id++);
	SetPropKey("m_xFaceNorm.direction",id++);
	SetPropKey("m_xFaceNorm.rot_ang",id++);
	SetPropKey("m_xFaceNorm.VectorType",id++);
	SetPropKey("m_xFaceNorm.nearVectorType",id++);
	SetPropKey("m_xFaceNorm.nearVector.x",id++);
	SetPropKey("m_xFaceNorm.nearVector.y",id++);
	SetPropKey("m_xFaceNorm.nearVector.z",id++);
	return id;
}
bool CSectionViewPropertyList::GetAssistLineStartPropValueStr(long id,char* valueStr)
{
	IModel* pModel=NULL;
	if(m_xSonView.BelongPartGroup())
		pModel=m_xSonView.BelongPartGroup()->BelongModel();
	CDrawingAssistant* pAssist=m_xSonView.hashAssistPlanes.GetFirst();
	if(pAssist==NULL||pModel==NULL)
		return false;
	CXhChar100 sText;
	if(GetPropID("m_xLineStart.datum_pos_style")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pAssist->m_xLineStart.datum_pos_style==1)
			sText.Copy("1.Angle Ridge Vertex Locating Point");
		else if(pAssist->m_xLineStart.datum_pos_style==2)
			sText.Copy("2.Rod Node Locating Point");
		else if(pAssist->m_xLineStart.datum_pos_style==3)
			sText.Copy("3.Angle Center Line Intersection Point");
		else if(pAssist->m_xLineStart.datum_pos_style==4)
			sText.Copy("4.Baseline Space Intersection Point");
		else if(pAssist->m_xLineStart.datum_pos_style==5)
			sText.Copy("5.Specified Component Point Of Datum Line");
		else if(pAssist->m_xLineStart.datum_pos_style==6)
			sText.Copy("6.Specified Component Point Of Datum Plane");
		else if(pAssist->m_xLineStart.datum_pos_style==7)
			sText.Copy("7.Outer Angle's Ridge Vertex Along Angle's Normal Offset");
		else if(pAssist->m_xLineStart.datum_pos_style==8)
			sText.Copy("8.With The Node In The Same Position");
		else if(pAssist->m_xLineStart.datum_pos_style==9)
			sText.Copy("9.Datum Rod Relative Coordinate");
		else //if(pAssist->m_xLineStart.datum_pos_style==0)
			sText.Copy("0.Manual Input");
#else 
		if(pAssist->m_xLineStart.datum_pos_style==1)
			sText.Copy("1.�Ǹ���㶨λ��");
		else if(pAssist->m_xLineStart.datum_pos_style==2)
			sText.Copy("2.�˼��ڵ㶨λ��");
		else if(pAssist->m_xLineStart.datum_pos_style==3)
			sText.Copy("3.�Ǹ����߽���");
		else if(pAssist->m_xLineStart.datum_pos_style==4)
			sText.Copy("4.��׼�߿ռ佻��");
		else if(pAssist->m_xLineStart.datum_pos_style==5)
			sText.Copy("5.��׼����ָ�����������");
		else if(pAssist->m_xLineStart.datum_pos_style==6)
			sText.Copy("6.��׼����ָ�������");
		else if(pAssist->m_xLineStart.datum_pos_style==7)
			sText.Copy("7.�ؽǸ���֫����ƫ�Ƶİ������");
		else if(pAssist->m_xLineStart.datum_pos_style==8)
			sText.Copy("8.��ڵ�ͬλ");
		else if(pAssist->m_xLineStart.datum_pos_style==9)
			sText.Copy("9.��׼�����ϵ��������");
		else //if(pAssist->m_xLineStart.datum_pos_style==0)
			sText.Copy("0.�ֶ�����");
#endif
	}
	else if(GetPropID("m_xLineStart.x")==id)
	{
		sText.Printf("%f",pAssist->m_xLineStart.Position().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.y")==id)
	{
		sText.Printf("%f",pAssist->m_xLineStart.Position().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.z")==id)
	{
		sText.Printf("%f",pAssist->m_xLineStart.Position().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.hRod")==id)	//�Ǹ־��
		sText.Printf("0X%X",pAssist->m_xLineStart.des_para.RODEND.hRod);
	else if(GetPropID("m_xLineStart.des_para.RODEND.direction")==id)	
	{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
		if(pAssist->m_xLineStart.des_para.RODEND.direction==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Start->End");
		else 
			sText.Copy("1.End->Start");
#else 
			sText.Copy("0.ʼ->��");
		else 
			sText.Copy("1.��->ʼ");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.offset_wing")==id)//0.X֫;1.Y֫
	{	//0.X֫;1.Y֫
		if(pAssist->m_xLineStart.des_para.RODEND.offset_wing==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.X Leg");
		else 
			sText.Copy("1.Y Leg");
#else 
			sText.Copy("0.X֫");
		else 
			sText.Copy("1.Y֫");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.wing_offset_style")==id)	
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(pAssist->m_xLineStart.des_para.RODEND.wing_offset_style==0)
			sText.Copy("g");
		else if(pAssist->m_xLineStart.des_para.RODEND.wing_offset_style==1)
			sText.Copy("g1");
		else if(pAssist->m_xLineStart.des_para.RODEND.wing_offset_style==2)
			sText.Copy("g2");
		else if(pAssist->m_xLineStart.des_para.RODEND.wing_offset_style==3)
			sText.Copy("g3");
		else 
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("User Defined");
#else 
			sText.Copy("�Զ���");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.wing_offset_dist")==id)
	{
		sText.Printf("%f",pAssist->m_xLineStart.des_para.RODEND.wing_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.len_offset_dist")==id)	
	{	//����ƫ��ֵ;
		sText.Printf("%f",pAssist->m_xLineStart.des_para.RODEND.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.bIncOddEffect")==id)		
	{	//����ƫ��ֵ��������ͷӰ��
		if(pAssist->m_xLineStart.des_para.RODEND.bIncOddEffect)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("Yes");
		else 
			sText.Copy("No");
#else 
			sText.Copy("��");
		else 
			sText.Copy("��");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.norm_offset")==id)//����ƫ����
	{
		CDesignThickPara thick(pAssist->m_xLineStart.des_para.RODEND.norm_offset);
		sText.Printf("%d",thick.Thick(pModel));
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.hLinePart")==id)	//�Ǹ־��
		sText.Printf("0X%X",pAssist->m_xLineStart.des_para.RODNODE.hRod);
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.hNode")==id)	//��׼�ڵ���
		sText.Printf("0X%X",pAssist->m_xLineStart.des_para.RODNODE.hNode);
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.bFaceOffset")==id)
	{
		if(pAssist->m_xLineStart.des_para.RODNODE.bFaceOffset)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("Yes");
		else 
			sText.Copy("No");
#else 
			sText.Copy("��");
		else 
			sText.Copy("��");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.direction")==id)	
	{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
		if(pAssist->m_xLineStart.des_para.RODNODE.direction==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Start->End");
		else 
			sText.Copy("1.End->Start");
#else 
			sText.Copy("0.ʼ->��");
		else 
			sText.Copy("1.��->ʼ");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.offset_wing")==id)	
	{	//0.X֫;1.Y֫
		if(pAssist->m_xLineStart.des_para.RODNODE.offset_wing==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.X Leg");
		else 
			sText.Copy("1.Y Leg");
#else 
			sText.Copy("0.X֫");
		else 
			sText.Copy("1.Y֫");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_style")==id)
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(pAssist->m_xLineStart.des_para.RODNODE.wing_offset_style==0)
			sText.Copy("g");
		else if(pAssist->m_xLineStart.des_para.RODNODE.wing_offset_style==1)
			sText.Copy("g1");
		else if(pAssist->m_xLineStart.des_para.RODNODE.wing_offset_style==2)
			sText.Copy("g2");
		else if(pAssist->m_xLineStart.des_para.RODNODE.wing_offset_style==3)
			sText.Copy("g3");
		else
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("User Defined");
#else 
			sText.Copy("�Զ���");
#endif
	}	
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_dist")==id)
	{
		sText.Printf("%f",pAssist->m_xLineStart.des_para.RODNODE.wing_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.len_offset_dist")==id)	
	{	//����ƫ��ֵ
		sText.Printf("%f",pAssist->m_xLineStart.des_para.RODNODE.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.norm_offset")==id)	//����ƫ����
	{
		CDesignThickPara thick(pAssist->m_xLineStart.des_para.RODNODE.norm_offset);
		sText.Printf("%d",thick.Thick(pModel));
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(pAssist->m_xLineStart.des_para.RODNODE.vPlaneNormal));
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]")==id)
	{
		sText.Printf("%f",pAssist->m_xLineStart.des_para.RODNODE.vPlaneNormal.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]")==id)
	{
		sText.Printf("%f",pAssist->m_xLineStart.des_para.RODNODE.vPlaneNormal.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]")==id)
	{
		sText.Printf("%f",pAssist->m_xLineStart.des_para.RODNODE.vPlaneNormal.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1")==id)
		sText.Printf("0X%X",pAssist->m_xLineStart.des_para.AXIS_INTERS.hDatum1);
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2")==id)
		sText.Printf("0X%X",pAssist->m_xLineStart.des_para.AXIS_INTERS.hDatum2);
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style1")==id)
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(pAssist->m_xLineStart.des_para.AXIS_INTERS.wing_offset_style1==0)
			sText.Copy("g");
		else if(pAssist->m_xLineStart.des_para.AXIS_INTERS.wing_offset_style1==1)
			sText.Copy("g1");
		else if(pAssist->m_xLineStart.des_para.AXIS_INTERS.wing_offset_style1==2)
			sText.Copy("g2");
		else if(pAssist->m_xLineStart.des_para.AXIS_INTERS.wing_offset_style1==3)
			sText.Copy("g3");
		else
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("User Defined");
#else 
			sText.Copy("�Զ���");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style2")==id)
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(pAssist->m_xLineStart.des_para.AXIS_INTERS.wing_offset_style2==0)
			sText.Copy("g");
		else if(pAssist->m_xLineStart.des_para.AXIS_INTERS.wing_offset_style2==1)
			sText.Copy("g1");
		else if(pAssist->m_xLineStart.des_para.AXIS_INTERS.wing_offset_style2==2)
			sText.Copy("g2");
		else if(pAssist->m_xLineStart.des_para.AXIS_INTERS.wing_offset_style2==3)
			sText.Copy("g3");
		else
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("User Defined");
#else 
			sText.Copy("�Զ���");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist1")==id)
	{
		sText.Printf("%d",pAssist->m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist1);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist2")==id)
	{
		sText.Printf("%d",pAssist->m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist2);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(f3dPoint(pAssist->m_xLineStart.des_para.AXIS_INTERS.near_vector)));
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[0]")==id)
	{
		sText.Printf("%f",pAssist->m_xLineStart.des_para.AXIS_INTERS.near_vector[0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[1]")==id)
	{
		sText.Printf("%f",pAssist->m_xLineStart.des_para.AXIS_INTERS.near_vector[1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[2]")==id)
	{
		sText.Printf("%f",pAssist->m_xLineStart.des_para.AXIS_INTERS.near_vector[2]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.norm_offset")==id)	//����ƫ����;
	{
		CDesignThickPara thick(pAssist->m_xLineStart.des_para.AXIS_INTERS.norm_offset);
		sText.Printf("%d",thick.Thick(pModel));
	}
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1")==id)
		sText.Printf("0X%X",pAssist->m_xLineStart.des_para.AXIS_INTERS.hDatum1);
	else if(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2")==id)
		sText.Printf("0X%X",pAssist->m_xLineStart.des_para.AXIS_INTERS.hDatum2);
	else if(GetPropID("m_xLineStart.des_para.DATUMLINE.hDatumLine")==id)
		sText.Printf("0X%X",pAssist->m_xLineStart.des_para.DATUMLINE.hDatumLine);
	else if(GetPropID("m_xLineStart.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pAssist->m_xLineStart.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
			sText.Printf("0.X Axis");
		else if(pAssist->m_xLineStart.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			sText.Printf("1.Y Axis");
		else 
			sText.Printf("2.Z Axis");
#else 
		if(pAssist->m_xLineStart.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
			sText.Printf("0.X��");
		else if(pAssist->m_xLineStart.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			sText.Printf("1.Y��");
		else 
			sText.Printf("2.Z��");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.DATUMPLANE.hDatumPlane")==id)
		sText.Printf("0X%X",pAssist->m_xLineStart.des_para.DATUMPLANE.hDatumPlane);
	else if(GetPropID("m_xLineStart.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pAssist->m_xLineStart.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
			sText.Printf("0.X Axis");
		else if(pAssist->m_xLineStart.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
			sText.Printf("1.Y Axis");
		else 
			sText.Printf("2.Z Axis");
#else 
		if(pAssist->m_xLineStart.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
			sText.Printf("0.X��");
		else if(pAssist->m_xLineStart.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
			sText.Printf("1.Y��");
		else 
			sText.Printf("2.Z��");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.hRod")==id)	//�Ǹ־��
		sText.Printf("0X%X",pAssist->m_xLineStart.des_para.RODEND.hRod);
	else if(GetPropID("m_xLineStart.des_para.RODEND.direction")==id)	
	{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
#ifdef AFX_TARG_ENU_ENGLISH
		if(pAssist->m_xLineStart.des_para.RODEND.direction==0)
			sText.Copy("0.Start->End");
		else 
			sText.Copy("1.End->Start");
#else 
		if(pAssist->m_xLineStart.des_para.RODEND.direction==0)
			sText.Copy("0.ʼ->��");
		else 
			sText.Copy("1.��->ʼ");
#endif
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.len_offset_dist")==id)	
	{	//����ƫ��ֵ;
		sText.Printf("%f",pAssist->m_xLineStart.des_para.RODEND.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.RODEND.norm_offset")==id)//����ƫ����
	{
		CDesignThickPara thick(pAssist->m_xLineStart.des_para.RODEND.norm_offset);
		sText.Printf("%d",thick.Thick(pModel));
	}
	else if(GetPropID("m_xLineStart.des_para.hNode")==id)
		sText.Printf("0X%X",pAssist->m_xLineStart.des_para.hNode);
	else if(GetPropID("des_para.hPart")==id)
		sText.Printf("0X%X",pAssist->m_xLineStart.des_para.hPart);
	else if(GetPropID("m_xLineStart.datum_pos.x")==id)
	{
		sText.Printf("%f",pAssist->m_xLineStart.datum_pos.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.datum_pos.y")==id)
	{
		sText.Printf("%f",pAssist->m_xLineStart.datum_pos.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.datum_pos.z")==id)
	{
		sText.Printf("%f",pAssist->m_xLineStart.datum_pos.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.hLinePart")==id)			//��׼�˼����
		sText.Printf("0X%0X",pAssist->m_xLineStart.des_para.SCALE_NODE.hLinePart);
	else if(GetPropID("des_para.SCALE_NODE.hStartNode")==id)		//ʼ�˽ڵ���
		sText.Printf("0X%0X",pAssist->m_xLineStart.des_para.SCALE_NODE.hStartNode);
	else if(GetPropID("des_para.SCALE_NODE.hEndNode")==id)			//�ն˽ڵ���
		sText.Printf("0X%0X",pAssist->m_xLineStart.des_para.SCALE_NODE.hEndNode);
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.start_offset_dist")==id)	//ʼ��ƫ����
	{
		sText.Printf("%f",pAssist->m_xLineStart.des_para.SCALE_NODE.start_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.end_offset_dist")==id)	//�ն�ƫ����
	{
		sText.Printf("%f",pAssist->m_xLineStart.des_para.SCALE_NODE.end_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.offset_scale")==id)		//ƫ�Ʊ���
	{
		sText.Printf("%f",pAssist->m_xLineStart.des_para.SCALE_NODE.offset_scale);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xLineStart.des_para.SCALE_NODE.offset_dist")==id)		//�ڵȷֻ����ϵ�ƫ��ֵ
	{
		sText.Printf("%f",pAssist->m_xLineStart.des_para.SCALE_NODE.offset_dist);
		SimplifiedNumString(sText);
	}
	else
		return false;
	strcpy(valueStr,sText);
	return true;
}
bool CSectionViewPropertyList::GetAssistFaceNormPropValueStr(long id,char* valueStr)
{
	IModel* pModel=NULL;
	if(m_xSonView.BelongPartGroup())
		pModel=m_xSonView.BelongPartGroup()->BelongModel();
	CDrawingAssistant* pAssist=m_xSonView.hashAssistPlanes.GetFirst();
	if(pAssist==NULL||pModel==NULL)
		return false;
	CXhChar100 sText;
	//��������
	if(GetPropID("m_xFaceNorm.norm_style")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pAssist->m_xFaceNorm.norm_style==1)
			sText.Printf("1.Single Angle Leg Normal");
		else if(pAssist->m_xFaceNorm.norm_style==2)
			sText.Printf("2.Two Rod Cross-bracing Normal");
		else if(pAssist->m_xFaceNorm.norm_style==3)
			sText.Printf("3.Part Work Normal");
		else if(pAssist->m_xFaceNorm.norm_style==4)
			sText.Printf("4.Part Relative Direction");
		else if(pAssist->m_xFaceNorm.norm_style==5)
			sText.Printf("5.Rod Characteristic Direction");
		else //if(pAssist->m_xFaceNorm.norm_style==0)
			sText.Printf("0.Specify Direction");
#else 
		if(pAssist->m_xFaceNorm.norm_style==1)
			sText.Printf("1.���Ǹ�֫����");
		else if(pAssist->m_xFaceNorm.norm_style==2)
			sText.Printf("2.���˼������淨��");
		else if(pAssist->m_xFaceNorm.norm_style==3)
			sText.Printf("3.������������");
		else if(pAssist->m_xFaceNorm.norm_style==4)
			sText.Printf("4.�����ϵ���Է���");
		else if(pAssist->m_xFaceNorm.norm_style==5)
			sText.Printf("5.�˼��ϵ���������");
		else //if(pAssist->m_xFaceNorm.norm_style==0)
			sText.Printf("0.ָ������");
#endif
	}
	else if(GetPropID("m_xFaceNorm.norm.x")==id)
	{
		sText.Printf("%lf",pAssist->m_xFaceNorm.vector.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xFaceNorm.norm.y")==id)
	{
		sText.Printf("%lf",pAssist->m_xFaceNorm.vector.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xFaceNorm.norm.z")==id)
	{
		sText.Printf("%lf",pAssist->m_xFaceNorm.vector.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xFaceNorm.hVicePart")==id)
		sText.Printf("0X%X",pAssist->m_xFaceNorm.hVicePart);
	else if(GetPropID("m_xFaceNorm.hCrossPart")==id)
		sText.Printf("0X%X",pAssist->m_xFaceNorm.hCrossPart);
	else if(GetPropID("m_xFaceNorm.norm_wing")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pAssist->m_xFaceNorm.norm_wing==0)
			sText.Printf("%s","0.X Leg Normal");
		else
			sText.Printf("%s","1.Y Leg Normal");
#else 
		if(pAssist->m_xFaceNorm.norm_wing==0)
			sText.Printf("%s","0.X֫����");
		else
			sText.Printf("%s","1.Y֫����");
#endif
	}
	else if(GetPropID("m_xFaceNorm.direction")==id)
	{
		if(pAssist->m_xFaceNorm.norm_style==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(pAssist->m_xFaceNorm.direction==0)
				sText.Printf("%s","0.Outward");
			else
				sText.Printf("%s","1.Inward");
#else 
			if(pAssist->m_xFaceNorm.direction==0)
				sText.Printf("%s","0.����");
			else
				sText.Printf("%s","1.����");
#endif
		}
		else if(pAssist->m_xFaceNorm.norm_style==5)
		{
			CSuperSmartPtr<CLDSPart>pDatumPart=pModel->FromPartHandle(pAssist->m_xFaceNorm.hVicePart);
#ifdef AFX_TARG_ENU_ENGLISH
			if(pAssist->m_xFaceNorm.direction==0)
				sText.Copy("0.Start->End");
			else if(pAssist->m_xFaceNorm.direction==1)
				sText.Copy("1.End->Start");
#else 
			if(pAssist->m_xFaceNorm.direction==0)
				sText.Copy("0.ʼ->��");
			else if(pAssist->m_xFaceNorm.direction==1)
				sText.Copy("1.��->ʼ");
#endif
			else
			{
				if(pDatumPart.IsHasPtr()&&pDatumPart->IsAngle())
				{
#ifdef AFX_TARG_ENU_ENGLISH
					if(pAssist->m_xFaceNorm.direction==2)
						sText.Copy("2.X Leg Normal+");
					else if(pAssist->m_xFaceNorm.direction==3)
						sText.Copy("3.X Leg Normal-");
					else if(pAssist->m_xFaceNorm.direction==4)
						sText.Copy("4.Y Leg Normal+");
					else 
						sText.Copy("5.Y Leg Normal-");
#else 
					if(pAssist->m_xFaceNorm.direction==2)
						sText.Copy("2.X֫����+");
					else if(pAssist->m_xFaceNorm.direction==3)
						sText.Copy("3.X֫����-");
					else if(pAssist->m_xFaceNorm.direction==4)
						sText.Copy("4.Y֫����+");
					else 
						sText.Copy("5.Y֫����-");
#endif
				}
				else if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
				{
#ifdef AFX_TARG_ENU_ENGLISH
					if(pAssist->m_xFaceNorm.direction==2)
						sText.Copy("2.Rotate Along Datum Plane");
					else if(pAssist->m_xFaceNorm.direction==3)
						sText.Copy("3.Rotate Along Weld Plane");
#else 
					if(pAssist->m_xFaceNorm.direction==2)
						sText.Copy("2.�Ի�׼����ת");
					else if(pAssist->m_xFaceNorm.direction==3)
						sText.Copy("3.�Ժ�������ת");
#endif
				}
			}
		}
		else 
		{
			if(pAssist->m_xFaceNorm.direction==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","0.Front Direction");
			else
				sText.Printf("%s","1.Reverse Direction");
#else 
				sText.Printf("%s","0.����");
			else
				sText.Printf("%s","1.����");
#endif
		}
	}
	else if(GetPropID("m_xFaceNorm.rot_ang")==id)
	{
		sText.Printf("%f",pAssist->m_xFaceNorm.rot_ang);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xFaceNorm.VectorType")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(pAssist->m_xFaceNorm.vector));
	else if(GetPropID("m_xFaceNorm.nearVectorType")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(pAssist->m_xFaceNorm.nearVector));
	else if(GetPropID("m_xFaceNorm.nearVector.x")==id)
	{
		sText.Printf("%lf",pAssist->m_xFaceNorm.nearVector.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xFaceNorm.nearVector.y")==id)
	{
		sText.Printf("%lf",pAssist->m_xFaceNorm.nearVector.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xFaceNorm.nearVector.z")==id)
	{
		sText.Printf("%lf",pAssist->m_xFaceNorm.nearVector.z);
		SimplifiedNumString(sText);
	}
	else
		return false;
	strcpy(valueStr,sText);
	return true;
}
long CSectionViewPropertyList::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	long slen=0;
	CXhChar100 sText;
	CDrawingAssistant* pAssist=m_xSonView.hashAssistPlanes.GetFirst();
	if((slen=CParaCSPropertyList::GetPropValueStr(id,valueStr))>0)
		return slen;
	else if(GetPropID("basicInfo")==id)
		sText.Copy("");
	else if(GetPropID("sName")==id)
		sText.Copy(m_xSonView.sName);
	else if(GetPropID("m_bLocked")==id)
	{
		if(m_xSonView.m_bLocked)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("Yes");
		else
			sText.Copy("No");
#else 
			sText.Copy("��");
		else
			sText.Copy("��");
#endif
	}
	else if(GetPropID("m_iViewType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_xSonView.m_iViewType==VIEW_MAIN)
			strcpy(sText,"1.Main View");
		else if(m_xSonView.m_iViewType==VIEW_FRONT)
			strcpy(sText,"2.Front View");
		else if(m_xSonView.m_iViewType==VIEW_NODEPLATE)
			strcpy(sText,"3.Node Plate View");
		else if(m_xSonView.m_iViewType==VIEW_CIRCOVERPLATE)
			strcpy(sText,"4.Circular Rib Plate View");
		else if(m_xSonView.m_iViewType==VIEW_CIRRIBPLATE)
			strcpy(sText,"5.Circular Sealing Plate View");
		else if(m_xSonView.m_iViewType==VIEW_TUBERIBPLATE)
			strcpy(sText,"6.Across Linking Plate View");
		else if(m_xSonView.m_iViewType==VIEW_FL)
			strcpy(sText,"7.Flange View");
		else if(m_xSonView.m_iViewType==VIEW_CIRPLATE)
			strcpy(sText,"8.Circular Plate View");
		else if(m_xSonView.m_iViewType==VIEW_INSERTPLATE)
			strcpy(sText,"9.Insert Plate View");
		else if(m_xSonView.m_iViewType==VIEW_FOOTNAILANGLE)
			strcpy(sText,"10.Foot Nail Angle View");
		else if(m_xSonView.m_iViewType==VIEW_FOOTNAILSLOT)
			strcpy(sText,"11.Foot Nail Slot Plate View");
		else if(m_xSonView.m_iViewType==VIEW_FOOT)
			strcpy(sText,"12.Base Plates");
		else if(m_xSonView.m_iViewType==VIEW_JOINSECTION)
			strcpy(sText,"13.Joint Section View");
		else if(m_xSonView.m_iViewType==VIEW_JOINTTUBE)
			strcpy(sText,"14.Relative Joint Tube View");
		else if(m_xSonView.m_iViewType==VIEW_KNODESKETCH)
			strcpy(sText,"15.K Node Intersecting View");
		else //if(m_xSonView.m_iViewType==VIEW_USERDEF)
			strcpy(sText,"100.User-defined View");
#else 
		if(m_xSonView.m_iViewType==VIEW_MAIN)
			strcpy(sText,"1.����ͼ");
		else if(m_xSonView.m_iViewType==VIEW_FRONT)
			strcpy(sText,"2.����ͼ");
		else if(m_xSonView.m_iViewType==VIEW_NODEPLATE)
			strcpy(sText,"3.�ڵ�����ͼ");
		else if(m_xSonView.m_iViewType==VIEW_CIRCOVERPLATE)
			strcpy(sText,"4.�����߰���ͼ");
		else if(m_xSonView.m_iViewType==VIEW_CIRRIBPLATE)
			strcpy(sText,"5.��������ͼ");
		else if(m_xSonView.m_iViewType==VIEW_TUBERIBPLATE)
			strcpy(sText,"6.��Խ������ͼ");
		else if(m_xSonView.m_iViewType==VIEW_FL)
			strcpy(sText,"7.������ͼ");
		else if(m_xSonView.m_iViewType==VIEW_CIRPLATE)
			strcpy(sText,"8.�������ͼ");
		else if(m_xSonView.m_iViewType==VIEW_INSERTPLATE)
			strcpy(sText,"9.�����ͼ");
		else if(m_xSonView.m_iViewType==VIEW_FOOTNAILANGLE)
			strcpy(sText,"10.�ǸֽŶ�");
		else if(m_xSonView.m_iViewType==VIEW_FOOTNAILSLOT)
			strcpy(sText,"11.�۸ֽŶ���");
		else if(m_xSonView.m_iViewType==VIEW_FOOT)
			strcpy(sText,"12.���Ű�");
		else if(m_xSonView.m_iViewType==VIEW_JOINSECTION)
			strcpy(sText,"13.������ͼ");
		else if(m_xSonView.m_iViewType==VIEW_JOINTTUBE)
			strcpy(sText,"14.�ԽӸֹܼ�ͼ");
		else if(m_xSonView.m_iViewType==VIEW_KNODESKETCH)
			strcpy(sText,"15.K�ڵ�����ͼ");
		else //if(m_xSonView.m_iViewType==VIEW_USERDEF)
			strcpy(sText,"100.�û�������ͼ");
#endif
	}
	else if(GetPropID("m_idHostView")==id)
	{
		if(m_xSonView.BelongPartGroup())
		{
			CSectionView *pHostView=m_xSonView.BelongPartGroup()->SonViews.GetValue(m_xSonView.m_idHostView);
			if(pHostView)
				sText.Printf("%s(Id:%d)",pHostView->sName,pHostView->GetId());
		}
	}
	else if(GetPropID("location")==id)		//������ͼ��ͼ����ϵ�µ�����ͼ����λ������
		sText.ConvertFromPoint(m_xSonView.location);
	else if(GetPropID("location.x")==id)
	{
		sText.Printf("%f",m_xSonView.location.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("location.y")==id)
	{
		sText.Printf("%f",m_xSonView.location.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("location.z")==id)
	{
		sText.Printf("%f",m_xSonView.location.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("direction")==id)		//������ͼ��ͼ����ϵ�µ�����ͼ�����߷���
		sText.ConvertFromPoint(m_xSonView.direction);
	else if(GetPropID("direction.x")==id)
	{
		sText.Printf("%f",m_xSonView.direction.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("direction.y")==id)
	{
		sText.Printf("%f",m_xSonView.direction.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("direction.z")==id)
	{
		sText.Printf("%f",m_xSonView.direction.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("direct_offset")==id)	//����ͼ��ʵ�ʱ�עλ���붨λ��������߷����ƫ�ƾ���
	{
		sText.Printf("%f",m_xSonView.direct_offset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("symbol_dist")==id)	//����ͼ��(�ɶ�)��ľ���
	{
		sText.Printf("%f",m_xSonView.symbol_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("symbol")==id)		//����ͼ���ϵı�ע����
		sText.Copy(m_xSonView.symbol);
	else if(GetAssistLineStartPropValueStr(id,sText)||GetAssistFaceNormPropValueStr(id,sText))
		;
	else
		return 0;//false;
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
void CSectionViewPropertyList::CopyFrom(CSectionView* pSectView)
{
	m_xSonView.sName[50]=0;
	strncpy(m_xSonView.sName,pSectView->sName,50);
	m_xSonView.SetBelongPartGroup(pSectView->BelongPartGroup());//UI����������ĸ�����, ����ֵ
	m_xSonView.m_iViewType=pSectView->m_iViewType;//UI����������ĸ�����, ����ֵ
	m_xSonView.m_bLocked=pSectView->m_bLocked;
	pSectView->des_cs.SetACS(pSectView->ucs,false);
	CParaCSPropertyList::CopyFrom(&pSectView->des_cs);
	m_bSlaveView=pSectView->IsSlaveView();
	if(m_bSlaveView)
	{
		m_xSonView.m_idHostView=((CSlaveSectionView*)pSectView)->m_idHostView;
		m_xSonView.location=((CSlaveSectionView*)pSectView)->location;			//������ͼ��ͼ����ϵ�µ�����ͼ����λ������
		m_xSonView.direction=((CSlaveSectionView*)pSectView)->direction;		//������ͼ��ͼ����ϵ�µ�����ͼ�����߷���
		m_xSonView.direct_offset=((CSlaveSectionView*)pSectView)->direct_offset;//����ͼ��ʵ�ʱ�עλ���붨λ��������߷����ƫ�ƾ���
		m_xSonView.symbol_dist=((CSlaveSectionView*)pSectView)->symbol_dist;	//����ͼ��(�ɶ�)��ľ���
		m_xSonView.symbol=((CSlaveSectionView*)pSectView)->symbol;				//����ͼ���ϵı�ע����
	}
	if(pSectView->hashAssistPlanes.GetNodeNum()>0)
	{
		CDrawingAssistant* pAssist=pSectView->hashAssistPlanes.GetFirst();
		CDrawingAssistant* pAssistClone=NULL;
		if(m_xSonView.hashAssistPlanes.GetNodeNum()>0)
			pAssistClone=m_xSonView.hashAssistPlanes.GetFirst();
		else
			pAssistClone=m_xSonView.hashAssistPlanes.Add(0);
		CBuffer buffer;
		pAssist->ToBuffer(buffer);
		buffer.SeekToBegin();
		pAssistClone->FromBuffer(buffer);
		pAssistClone->m_xFaceNorm.vector=pAssist->m_xFaceNorm.vector;	//ʵʱֵ�����л�ʱ���洢
		pAssistClone->m_xLineStart.SetPosition(pAssist->m_xLineStart.Position());
	}
}
void CSectionViewPropertyList::CopyTo(CSectionView* pSectView)
{
	pSectView->sName[50]=0;
	strncpy(pSectView->sName,m_xSonView.sName,50);
	pSectView->m_bLocked=m_xSonView.m_bLocked;
	CParaCSPropertyList::CopyTo(&pSectView->des_cs);
	pSectView->ucs=pSectView->des_cs.GetACS();
	if(m_bSlaveView)
	{
		((CSlaveSectionView*)pSectView)->m_idHostView=m_xSonView.m_idHostView;
		((CSlaveSectionView*)pSectView)->location=m_xSonView.location;			//������ͼ��ͼ����ϵ�µ�����ͼ����λ������
		((CSlaveSectionView*)pSectView)->direction=m_xSonView.direction;		//������ͼ��ͼ����ϵ�µ�����ͼ�����߷���
		((CSlaveSectionView*)pSectView)->direct_offset=m_xSonView.direct_offset;//����ͼ��ʵ�ʱ�עλ���붨λ��������߷����ƫ�ƾ���
		((CSlaveSectionView*)pSectView)->symbol_dist=m_xSonView.symbol_dist;	//����ͼ��(�ɶ�)��ľ���
		((CSlaveSectionView*)pSectView)->symbol=m_xSonView.symbol;				//����ͼ���ϵı�ע����
	}
	if(m_xSonView.hashAssistPlanes.GetNodeNum()>0)
	{
		CDrawingAssistant* pAssist=m_xSonView.hashAssistPlanes.GetFirst();
		CDrawingAssistant* pAssistClone=NULL;
		if(pSectView->hashAssistPlanes.GetNodeNum()>0)
			pAssistClone=pSectView->hashAssistPlanes.GetFirst();
		else
			pAssistClone=pSectView->hashAssistPlanes.Add(0);
		CBuffer buffer;
		pAssist->ToBuffer(buffer);
		pAssistClone->FromBuffer(buffer);
	}
}

//�ַ�����ʽΪ:��ͼ����[id:0x156]
static CString MakeHostPartGroupViewCmbItemStr(CLDSPartGroup *pPartGroup)
{
	CString strCmbItem = "";
	for(CSectionView *pView=pPartGroup->SonViews.GetFirst();pView;pView=pPartGroup->SonViews.GetNext())
	{
		if(!pView->IsHostView())
			continue;
		if(strCmbItem.GetLength()>0)
			strCmbItem+="|";
		//strCmbItem.Append.Format("%s%s(Id:%d)",strCmbItem,pView->sName,pView->GetId());
		strCmbItem.AppendFormat("%s(Id:%d)",pView->sName,pView->GetId());	//��ģʽCString��ӡ�����п��ܵ����쳣��wjh-2016.11.13
	}
	return strCmbItem;
}

void CSectionViewPropertyList::InsertPointPropItem(CPropTreeItem *pParentItem,char *sPropID,CString sPropName,CString sPropHelp)
{
	CXhChar100 sText;
	//װ��ʱX�᷽��
	CItemInfo *lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
	lpInfo->m_strPropName = sPropName;
	lpInfo->m_strPropHelp = sPropHelp;
	CPropTreeItem *pPropItem = InsertItem(pParentItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID(sPropID);
	pPropItem->m_bHideChildren = TRUE;
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//װ������ϵX��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp.Format("%sX coordinate component.",sPropHelp);
#else 
	lpInfo->m_strPropHelp.Format("%sX���������",sPropHelp);
#endif
	CPropTreeItem *pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID(CXhChar50("%s.x",sPropID));
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵX��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp.Format("%sY coordinate component.",sPropHelp);
#else 
	lpInfo->m_strPropHelp.Format("%sY���������",sPropHelp);
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID(CXhChar50("%s.y",sPropID));
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵX��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "Z";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp.Format("%sZ coordinate component.",sPropHelp);
#else 
	lpInfo->m_strPropHelp.Format("%sZ���������",sPropHelp);
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID(CXhChar50("%s.z",sPropID));
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
}

void CSectionViewPropertyList::UpdateDatumPointPropItem(CPropTreeItem *pParentItem,CLDSPoint *pPoint,BOOL bUpdate)
{
	IModel* pModel=m_xSonView.BelongPartGroup()->BelongModel();
	if(pParentItem==NULL||pModel==NULL)
		return;
	//
	CItemInfo* lpInfo=NULL;
	CXhChar100 valueStr;
	CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL;
	if(bUpdate)
	{
		//��ڵ�ͬλ
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.hNode"));
		//ָ�������ϵ��������
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.hPart"));
		DeleteItemByPropId(GetPropID("m_xLineStart.datum_pos"));
		DeleteItemByPropId(GetPropID("m_xLineStart.datum_pos.x"));
		DeleteItemByPropId(GetPropID("m_xLineStart.datum_pos.y"));
		DeleteItemByPropId(GetPropID("m_xLineStart.datum_pos.z"));
		//���߽���
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style1"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style2"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist1"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist2"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[0]"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[1]"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[2]"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.AXIS_INTERS.norm_offset"));
		//�Ǹ����
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.RODEND.hRod"));				
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.RODEND.direction"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.RODEND.offset_wing"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.RODEND.wing_offset_style"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.RODEND.wing_offset_dist"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.RODEND.len_offset_dist"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.RODEND.bIncOddEffect"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.RODEND.norm_offset"));	
		//�˼��Ͻڵ�Ϊ��׼
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.hLinePart"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.hNode"));	
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.bFaceOffset"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.direction"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.offset_wing"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_style"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_dist"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.len_offset_dist"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.norm_offset"));	
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]"));
		//��׼�����������겻���
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.DATUMLINE.hDatumLine"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2"));
		//��׼�����������겻���
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.DATUMPLANE.hDatumPlane"));
		DeleteItemByPropId(GetPropID("m_xLineStart.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2"));
	}
	//���»�׼��ֻ������
	SetItemReadOnly(GetPropID("m_xLineStart.x"),pPoint->datum_pos_style!=0);
	SetItemReadOnly(GetPropID("m_xLineStart.y"),pPoint->datum_pos_style!=0);
	SetItemReadOnly(GetPropID("m_xLineStart.z"),pPoint->datum_pos_style!=0);
	if(pPoint->datum_pos_style==1)	//�Ǹ����
	{	//��׼�Ǹ־��
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.hRod");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//ƫ��֫
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.X Leg|1.Y Leg";
		lpInfo->m_strPropName="Offset Leg";
		lpInfo->m_strPropHelp="Offset leg.";
#else 
		lpInfo->m_cmbItems="0.X֫|1.Y֫";
		lpInfo->m_strPropName="ƫ��֫";
		lpInfo->m_strPropHelp="ƫ��֫��";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.offset_wing");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="g|g1|g2|g3|User Defined|";
		lpInfo->m_strPropName="Leg Offset Style";
		lpInfo->m_strPropHelp="The offset stadia or user specified offset.";
#else 
		lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
		lpInfo->m_strPropName="֫��ƫ������";
		lpInfo->m_strPropHelp="ƫ������Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.wing_offset_style");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//ƫ�ƾ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Leg Direction Offset";
		lpInfo->m_strPropHelp="The offset of leg direction.";
#else 
		lpInfo->m_strPropName="֫��ƫ��ֵ";
		lpInfo->m_strPropHelp="֫��ƫ��ֵ��";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.wing_offset_dist");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->SetReadOnly(pPoint->des_para.RODEND.wing_offset_style!=4);
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Vertical Offset Direction";
		lpInfo->m_strPropHelp="Vertical Offset Direction,offset along the direction of datum angle start to the end or along the opposite direction.";
#else 
		lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
		lpInfo->m_strPropName="����ƫ�Ʒ���";
		lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.direction");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.len_offset_dist");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.bIncOddEffect");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.norm_offset");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
		lpInfo->m_strPropHelp="Datum rod's handle";
#else 
		lpInfo->m_strPropName="��׼�˼����";
		lpInfo->m_strPropHelp="��׼�˼������";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.hLinePart");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//��׼�ڵ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Node Handle";
		lpInfo->m_strPropHelp="Datum node's handle.";
#else 
		lpInfo->m_strPropName="��׼�ڵ���";
		lpInfo->m_strPropHelp="��׼�ڵ�����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.hNode");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pModel->FromPartHandle(pPoint->des_para.RODNODE.hRod,CLS_LINEPART);
		if(pLinePart&&pLinePart->IsAngle())
		{	//ƫ��֫
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X Leg|1.Y Leg";
			lpInfo->m_strPropName="Offset Leg";
			lpInfo->m_strPropHelp="Offset leg.";
#else 
			lpInfo->m_cmbItems="0.X֫|1.Y֫";
			lpInfo->m_strPropName="ƫ��֫";
			lpInfo->m_strPropHelp="ƫ��֫��";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.offset_wing");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|User Defined|";
			lpInfo->m_strPropName="Leg Offset Style";
			lpInfo->m_strPropHelp="The offset stadia or user specified offset.";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������";
			lpInfo->m_strPropHelp="ƫ������Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_style");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Direction Offset";
			lpInfo->m_strPropHelp="The offset of leg direction.";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ";
			lpInfo->m_strPropHelp="֫��ƫ��ֵ��";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.wing_offset_dist");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(pPoint->des_para.RODNODE.wing_offset_style!=4);
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Vertical Offset Direction";
		lpInfo->m_strPropHelp="Vertical Offset Direction,offset along the direction of datum angle start to the end or along the opposite direction.";
#else 
		lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
		lpInfo->m_strPropName="����ƫ�Ʒ���";
		lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.direction");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.len_offset_dist");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.norm_offset");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//ƽ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName="Planar Location";
		lpInfo->m_strPropHelp="Planar Location";
#else 
		lpInfo->m_cmbItems="��|��";
		lpInfo->m_strPropName="ƽ��";
		lpInfo->m_strPropHelp="ƽ��";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.bFaceOffset");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		if(pPoint->des_para.RODNODE.bFaceOffset)
		{	//ƽ���淨��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
			lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Offset Plane Normal";
			lpInfo->m_strPropHelp="Offset plane's normal line";
#else 
			lpInfo->m_strPropName="ƽ���淨��";
			lpInfo->m_strPropHelp="ƽ���淨��";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
			pSonPropItem = InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[0]");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
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
			pSonPropItem = InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[1]");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
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
			pSonPropItem = InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = GetPropID("m_xLineStart.des_para.LINEPARTNODE.face_norm_offset[2]");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
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
		lpInfo->m_strPropHelp="Datum rod 1's handle";
#else 
		lpInfo->m_strPropName="��׼�˼�1���";
		lpInfo->m_strPropHelp="��׼�˼�1�����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		CLDSLinePart *pLinePart1=(CLDSLinePart*)pModel->FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEPART);
		if(pLinePart1&&pLinePart1->IsAngle())
		{	//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|User Defined|";
			lpInfo->m_strPropName="Leg Offset Style 1";
			lpInfo->m_strPropHelp="Datum Angle1 Offset Type:Scheduled Stadia Or User Specify Offset Distance.";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������1";
			lpInfo->m_strPropHelp="��׼�Ǹ�1ƫ������:Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style1");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Direction Offset 1";
			lpInfo->m_strPropHelp="Datum Angle 1 Leg Offset.";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ1";
			lpInfo->m_strPropHelp="��׼�Ǹ�1֫��ƫ��ֵ��";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist1");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
		lpInfo->m_strPropHelp="Datum rod 2's handle";
#else 
		lpInfo->m_strPropName="��׼�˼�2���";
		lpInfo->m_strPropHelp="��׼�˼�2���";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		CLDSLinePart *pLinePart2=(CLDSLinePart*)pModel->FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEPART);
		if(pLinePart2&&pLinePart2->IsAngle())
		{	//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|User Defined|";
			lpInfo->m_strPropName="Leg Offset Style 2";
			lpInfo->m_strPropHelp="The offset type of datum angle 2:Scheduled stadia or user specify offset distance.";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������2";
			lpInfo->m_strPropHelp="��׼�Ǹ�2ƫ������:Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_style2");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Direction Offset 2";
			lpInfo->m_strPropHelp="The leg offset of datum angle 2.";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ2";
			lpInfo->m_strPropHelp="��׼�Ǹ�2֫��ƫ��ֵ��";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.wing_offset_dist2");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.norm_offset");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
			pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
			pSonPropItem = InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[0]");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The Y component of near normal";
#else 
			lpInfo->m_strPropHelp="���Ʒ���Y�������";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[1]");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The Z component of near normal";
#else 
			lpInfo->m_strPropHelp="���Ʒ���Z�������";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.near_vector[2]");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
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
		lpInfo->m_strPropHelp="Datum line 1's handle";
#else 
		lpInfo->m_strPropName="��׼��1���";
		lpInfo->m_strPropHelp="��׼��1���";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum1");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.AXIS_INTERS.hDatum2");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.DATUMLINE.hDatumLine");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.DATUMPLANE.hDatumPlane");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
		lpInfo->m_strPropHelp="Datum angle's handle.";
#else 
		lpInfo->m_strPropName="��׼�Ǹ־��";
		lpInfo->m_strPropHelp="��׼�Ǹ־����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.hRod");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Vertical Offset Direction";
		lpInfo->m_strPropHelp="Vertical Offset Direction,offset along the direction of datum angle start to the end or along the opposite direction.";
#else 
		lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
		lpInfo->m_strPropName="����ƫ�Ʒ���";
		lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.direction");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.len_offset_dist");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.RODEND.norm_offset");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
		lpInfo->m_strPropHelp="Datum node's handle.";
#else 
		lpInfo->m_strPropName="��׼�ڵ���";
		lpInfo->m_strPropHelp="��׼�ڵ�����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.hNode");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.des_para.hPart");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = GetPropID("m_xLineStart.datum_pos");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
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
		pSonPropItem = InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp = GetPropID("m_xLineStart.datum_pos.x");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
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
		pSonPropItem = InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp = GetPropID("m_xLineStart.datum_pos.y");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
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
		pSonPropItem = InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp = GetPropID("m_xLineStart.datum_pos.z");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
}
void CSectionViewPropertyList::UpdateDatumVectorPropItem(CPropTreeItem *pParentItem,BOOL bUpdate)
{
	IModel* pModel=m_xSonView.BelongPartGroup()->BelongModel();
	CDrawingAssistant *pAssist=m_xSonView.hashAssistPlanes.GetFirst();
	if(pParentItem==NULL||pAssist==NULL||pModel==NULL)
		return;
	//
	CItemInfo* lpInfo=NULL;
	CXhChar100 sText;
	CPropTreeItem *pPropItem=NULL,*pSonItem=NULL;
	if(bUpdate)
	{
		DeleteItemByPropId(GetPropID("m_xFaceNorm.hVicePart"));
		DeleteItemByPropId(GetPropID("m_xFaceNorm.hCrossPart"));
		DeleteItemByPropId(GetPropID("m_xFaceNorm.norm_wing"));
		DeleteItemByPropId(GetPropID("m_xFaceNorm.direction"));
		DeleteItemByPropId(GetPropID("m_xFaceNorm.VectorType"));
		DeleteItemByPropId(GetPropID("m_xFaceNorm.nearVectorType"));
		DeleteItemByPropId(GetPropID("m_xFaceNorm.nearVector.x"));
		DeleteItemByPropId(GetPropID("m_xFaceNorm.nearVector.y"));
		DeleteItemByPropId(GetPropID("m_xFaceNorm.nearVector.z"));
	}
	if(pAssist->m_xFaceNorm.norm_style!=0)
	{
		SetItemReadOnly(GetPropID("m_xFaceNorm.norm.x"),TRUE);
		SetItemReadOnly(GetPropID("m_xFaceNorm.norm.y"),TRUE);
		SetItemReadOnly(GetPropID("m_xFaceNorm.norm.z"),TRUE);
	}
	else 
	{
		int nVectorType=CStdVector::GetVectorType(pAssist->m_xFaceNorm.vector);
		SetItemReadOnly(GetPropID("m_xFaceNorm.norm.x"),nVectorType!=0);
		SetItemReadOnly(GetPropID("m_xFaceNorm.norm.y"),nVectorType!=0);
		SetItemReadOnly(GetPropID("m_xFaceNorm.norm.z"),nVectorType!=0);
	}
	//ָ������
	if(pAssist->m_xFaceNorm.norm_style==0)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specify Direction";
		lpInfo->m_strPropHelp = "Specify Direction";
#else 
		lpInfo->m_strPropName = "ָ������";
		lpInfo->m_strPropHelp = "ָ������";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.VectorType");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	//���Ǹ�֫����
	else if(pAssist->m_xFaceNorm.norm_style==1)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Angle";
		lpInfo->m_strPropHelp = "Datum angle's handle";
#else 
		lpInfo->m_strPropName = "��׼�Ǹ�";
		lpInfo->m_strPropHelp = "��׼�Ǹ־��";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems ="0.X Leg Normal|1.Y Leg Normal";
		lpInfo->m_strPropName = "Datum Leg";
		lpInfo->m_strPropHelp = "Datum leg";
#else 
		lpInfo->m_cmbItems ="0.X֫����|1.Y֫����";
		lpInfo->m_strPropName = "��׼֫";
		lpInfo->m_strPropHelp = "��׼֫";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.norm_wing");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
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
		pPropItem = InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.direction");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	else if(pAssist->m_xFaceNorm.norm_style==2)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Rod";
		lpInfo->m_strPropHelp = "Datum rod's handle";
#else 
		lpInfo->m_strPropName = "��׼�˼�";
		lpInfo->m_strPropHelp = "��׼�˼����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
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
		pPropItem = InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.hCrossPart");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
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
		pPropItem = InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.nearVectorType");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

		int nearVectorType=CStdVector::GetVectorType(pAssist->m_xFaceNorm.nearVector);
		if(nearVectorType==0)	//�Զ���
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X";
			lpInfo->m_strPropHelp = "X Component";
#else 
			lpInfo->m_strPropName = "X����";
			lpInfo->m_strPropHelp = "X����";
#endif
			pSonItem = InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =GetPropID("m_xFaceNorm.nearVector.x");
			if(GetPropValueStr(pSonItem->m_idProp,sText))
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y";
			lpInfo->m_strPropHelp = "Y Component";
#else 
			lpInfo->m_strPropName = "Y����";
			lpInfo->m_strPropHelp = "Y����";
#endif
			pSonItem = InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =GetPropID("m_xFaceNorm.nearVector.y");
			if(GetPropValueStr(pSonItem->m_idProp,sText))
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z";
			lpInfo->m_strPropHelp = "Z Component";
#else 
			lpInfo->m_strPropName = "Z����";
			lpInfo->m_strPropHelp = "Z����";
#endif
			pSonItem = InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =GetPropID("m_xFaceNorm.nearVector.z");
			if(GetPropValueStr(pSonItem->m_idProp,sText))
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
	}
	else if(pAssist->m_xFaceNorm.norm_style==3)
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
		pPropItem = InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems ="0.Front Direction|1.Reverse Direction";
		lpInfo->m_strPropName = "Direction";
		lpInfo->m_strPropHelp = "Front direction is datum part's plus normal ��reverse direction is opposite direction";
#else 
		lpInfo->m_cmbItems ="0.����|1.����";
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "������ָ��׼���������������߷��򣻷�����ָ��׼�����ĸ��������߷���";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.direction");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	else if(pAssist->m_xFaceNorm.norm_style==4)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Part";
		lpInfo->m_strPropHelp = "Datum part's handle";
#else 
		lpInfo->m_strPropName = "��׼����";
		lpInfo->m_strPropHelp = "��׼����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
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
		pPropItem = InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.nearVectorType");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

		int nearVectorType=CStdVector::GetVectorType(pAssist->m_xFaceNorm.nearVector);
		if(nearVectorType==0)	//�Զ���
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X";
			lpInfo->m_strPropHelp = "X Component";
#else 
			lpInfo->m_strPropName = "X����";
			lpInfo->m_strPropHelp = "X����";
#endif
			pSonItem = InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =GetPropID("m_xFaceNorm.nearVector.x");
			if(GetPropValueStr(pSonItem->m_idProp,sText))
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y";
			lpInfo->m_strPropHelp = "Y Component";
#else 
			lpInfo->m_strPropName = "Y����";
			lpInfo->m_strPropHelp = "Y����";
#endif
			pSonItem = InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =GetPropID("m_xFaceNorm.nearVector.y");
			if(GetPropValueStr(pSonItem->m_idProp,sText))
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z";
			lpInfo->m_strPropHelp = "Z Component";
#else 
			lpInfo->m_strPropName = "Z����";
			lpInfo->m_strPropHelp = "Z����";
#endif
			pSonItem = InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =GetPropID("m_xFaceNorm.nearVector.z");
			if(GetPropValueStr(pSonItem->m_idProp,sText))
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
	}
	else if(pAssist->m_xFaceNorm.norm_style==5)
	{	//5.��׼�˼��ϵ���������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Rod";
		lpInfo->m_strPropHelp = "Datum rod's handle";
#else 
		lpInfo->m_strPropName = "��׼�˼�";
		lpInfo->m_strPropHelp = "��׼�˼����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		
		//��������
		bool bNeedRotAnglePara=false;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pModel->FromPartHandle(pAssist->m_xFaceNorm.hVicePart,CLS_LINEPART);
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
			lpInfo->m_cmbItems = "0.Start->End|1.End->Start|2.Rotate Along Datum Plane|3.Rotate Along Weld Plane";
#else 
			lpInfo->m_cmbItems = "0.ʼ->��|1.��->ʼ|2.�Ի�׼����ת|3.�Ժ�������ת";
#endif
			if(pAssist->m_xFaceNorm.direction>=2)
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
		pPropItem = InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("m_xFaceNorm.direction");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		if(bNeedRotAnglePara)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType=CDT_EDIT;
			lpInfo->m_cmbItems = "0|90|180|270";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rotate Angle";
			if(pAssist->m_xFaceNorm.direction==2)
				lpInfo->m_strPropHelp="The rotation angle from tube's datum plane around the axial line��";
			else 
				lpInfo->m_strPropHelp="The rotation angle from tube's weld plane around the axial line��";
#else 
			lpInfo->m_strPropName = "��ת�Ƕ�";
			if(pAssist->m_xFaceNorm.direction==2)
				lpInfo->m_strPropHelp="�Ըֹܻ�׼��ƽ�淨�������ߵ���ת�Ƕȡ�";
			else 
				lpInfo->m_strPropHelp="�Ըֹܺ�����ƽ�淨�������ߵ���ת�Ƕȡ�";
#endif
			pSonItem = InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp = GetPropID("m_xFaceNorm.rot_ang");
			if(GetPropValueStr(pSonItem->m_idProp,sText))
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
	}
}
void CSectionViewPropertyList::DisplayPropertyList(int overwrite0_append1/*=0*/,DWORD dwPropGroup/*=1*/)
{
	CItemInfo* lpInfo=NULL;
	CXhChar100 sText;
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pLeafItem=NULL,*pRootItem=GetRootItem();

	if(overwrite0_append1==0)
	{
		CleanTree();
		m_nObjClassTypeId = 0;
	}

	//��ͼ������Ϣ
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "View Basic Information";
	lpInfo->m_strPropHelp = "View Basic Information";
#else 
	lpInfo->m_strPropName = "��ͼ������Ϣ";
	lpInfo->m_strPropHelp = "��ͼ������Ϣ";
#endif
	pGroupItem = InsertItem(pRootItem,lpInfo, -1);
	pGroupItem->m_idProp = GetPropID("basicInfo");
	//
	//����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Name";
	lpInfo->m_strPropHelp = "Custom View Name";
#else 
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "������ͼ����";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("sName");
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="Yes|No";
	lpInfo->m_strPropName = "Lock";
	lpInfo->m_strPropHelp = "Refresh will not update view after view-lock.";
#else 
	lpInfo->m_cmbItems="��|��";
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "��ͼ������,ˢ��ʱ�������¸�����ͼ���ݡ�";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("m_bLocked");
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//��ͼ����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "View Type";
	lpInfo->m_strPropHelp = "Attached view's type";
	lpInfo->m_cmbItems="1.Main View|2.Front View|3.Node Plate View|4.Circular Rib Plate View|5.Circular Sealing Plate View|6.Across Linking Plate View|7.Flange View|8.Circular Plate View|";
	lpInfo->m_cmbItems+="9.Insert Plate View|10.Foot Nail Angle View|11.Foot Nail Slot Plate View|12.Base Plates|13.Branch Tube End View|14.Relative Joint Tube View|15.K Node Intersecting View|100.User-defined View|";
#else 
	lpInfo->m_strPropName = "��ͼ����";
	lpInfo->m_strPropHelp = "������ͼ����";
	lpInfo->m_cmbItems="1.����ͼ|2.����ͼ|3.�ڵ�����ͼ|4.�����߰���ͼ|5.��������ͼ|6.��Խ������ͼ|7.������ͼ|8.�������ͼ|";
	lpInfo->m_cmbItems+="9.�����ͼ|10.�ǸֽŶ�|11.�۸ֽŶ���|12.���Ű�|13.֧�ܶ���ͼ|14.�ԽӸֹܼ�ͼ|15.K�ڵ�����ͼ|100.�û�������ͼ|";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("m_iViewType");
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->SetReadOnly();
	
	if(m_bSlaveView&&m_xSonView.GetViewTypeId()==CSectionView::CLS_SLAVE_VIEW)
	{	//����ͼ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Cutaway View Symbol";
		lpInfo->m_strPropHelp = "The location and parameters of cutaway view symbol that child views in host view.";
#else 
		lpInfo->m_strPropName = "����ͼ��";
		lpInfo->m_strPropHelp = "����ͼ��������ͼ�ж�Ӧ������ͼ��λ�ü�������";
#endif
		pGroupItem = InsertItem(pRootItem,lpInfo, -1);
		pGroupItem->m_idProp = GetPropID("slaveViewPara");
		//����ͼ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Cutaway View Symbol Name";
		lpInfo->m_strPropHelp = "The show name of drawing cutaway view symbol��maximum is two characters.";
#else 
		lpInfo->m_strPropName = "����ͼ������";
		lpInfo->m_strPropHelp = "��������ͼ����ʱ��ʾ�����ƣ����Ϊ�����ַ���";
#endif
		pLeafItem = InsertItem(pGroupItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("symbol");
		if(GetPropValueStr(pLeafItem->m_idProp,sText))
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;	
		//������ͼ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Host View";
		lpInfo->m_strPropHelp = "Curent view's host view.";
#else 
		lpInfo->m_strPropName = "������ͼ";
		lpInfo->m_strPropHelp = "��ǰ��ͼ��Ӧ��������ͼ";
#endif
		lpInfo->m_cmbItems=MakeHostPartGroupViewCmbItemStr(m_xSonView.BelongPartGroup());
		pPropItem = InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = GetPropID("m_idHostView");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		//��׼λ�ü�����
#ifdef AFX_TARG_ENU_ENGLISH
		InsertPointPropItem(pGroupItem,"location","Datum Position","The datum position of cutaway view symbol");
		InsertPointPropItem(pGroupItem,"direction","Direction","The direction of cutaway view symbol");
#else 
		InsertPointPropItem(pGroupItem,"location","��׼λ��","����ͼ���Ż��ƻ�׼λ��");
		InsertPointPropItem(pGroupItem,"direction","����","����ͼ���ų���");
#endif
		//ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Offset";
		lpInfo->m_strPropHelp = "Cutaway view symbol's location toward the direction of offset";
#else 
		lpInfo->m_strPropName = "ƫ����";
		lpInfo->m_strPropHelp = "����ͼ������λ��������ͼ���������ƫ������";
#endif
		pLeafItem = InsertItem(pGroupItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("direct_offset");
		if(GetPropValueStr(pLeafItem->m_idProp,sText))
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		//����ͼ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Width";
		lpInfo->m_strPropHelp = "The distance of cutaway view symbol name.";
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "����ͼ�����ơ�";
#endif
		pLeafItem = InsertItem(pGroupItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("symbol_dist");
		if(GetPropValueStr(pLeafItem->m_idProp,sText))
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	else if(m_xSonView.hashAssistPlanes.GetNodeNum()>0)
	{	//չ���������ʾ
		CDrawingAssistant *pAssist=m_xSonView.hashAssistPlanes.GetFirst();
		//չ���߻�׼��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_STATIC;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Spread Line Datum Point";
		lpInfo->m_strPropHelp="Spread line datum point��due to the datum point of the drawing datum plane exists uncertainty line,the datum points of spread-plane must be located in spread-line.";
#else 
		lpInfo->m_strPropName="չ���߻�׼��";
		lpInfo->m_strPropHelp="չ���߻�׼�㣬���ڻ�ͼ����Ļ�����ڲ�ȷ���ߣ�չ����Ļ������λ��չ�����ϡ�";
#endif
		CPropTreeItem *pPointItem = InsertItem(pRootItem,lpInfo,-1);
		pPointItem->m_idProp = GetPropID("m_xLineStart");
		if(GetPropValueStr(pPointItem->m_idProp,sText))
			pPointItem->m_lpNodeInfo->m_strPropValue=sText;
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
		pGroupItem = InsertItem(pPointItem,lpInfo,-1);
		pGroupItem->m_idProp = GetPropID("m_xLineStart.pos");
		if(GetPropValueStr(pGroupItem->m_idProp,sText))
			pGroupItem->m_lpNodeInfo->m_strPropValue=sText;
		pGroupItem->m_bHideChildren=TRUE;
		if(GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = GetPropStatus(pGroupItem->m_idProp);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="X";
		lpInfo->m_strPropHelp="X Coordinate";
#else 
		lpInfo->m_strPropName="X����";
		lpInfo->m_strPropHelp="X����";
#endif
		pLeafItem = InsertItem(pGroupItem,lpInfo,-1);
		pLeafItem->m_idProp = GetPropID("m_xLineStart.x");
		if(GetPropValueStr(pLeafItem->m_idProp,sText))
			pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
		pLeafItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
		pLeafItem->SetReadOnly(pAssist->m_xLineStart.datum_pos_style!=0);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Y";
		lpInfo->m_strPropHelp="Y Coordinate";
#else 
		lpInfo->m_strPropName="Y����";
		lpInfo->m_strPropHelp="Y����";
#endif
		pLeafItem = InsertItem(pGroupItem,lpInfo,-1);
		pLeafItem->m_idProp = GetPropID("m_xLineStart.y");
		if(GetPropValueStr(pLeafItem->m_idProp,sText))
			pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
		pLeafItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
		pLeafItem->SetReadOnly(pAssist->m_xLineStart.datum_pos_style!=0);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Z";
		lpInfo->m_strPropHelp="Z Coordinate";
#else 
		lpInfo->m_strPropName="Z����";
		lpInfo->m_strPropHelp="Z����";
#endif
		pLeafItem = InsertItem(pGroupItem,lpInfo,-1);
		pLeafItem->m_idProp = GetPropID("m_xLineStart.z");
		if(GetPropValueStr(pLeafItem->m_idProp,sText))
			pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
		pLeafItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
		pLeafItem->SetReadOnly(pAssist->m_xLineStart.datum_pos_style!=0);
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
		pGroupItem = InsertItem(pPointItem,lpInfo,-1);
		pGroupItem->m_idProp = GetPropID("m_xLineStart.datum_pos_style");
		if(GetPropValueStr(pGroupItem->m_idProp,sText))
			pGroupItem->m_lpNodeInfo->m_strPropValue=sText;
		pGroupItem->m_bHideChildren=FALSE;
		if(GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = GetPropStatus(pGroupItem->m_idProp);
		//��׼������
		UpdateDatumPointPropItem(pGroupItem,&pAssist->m_xLineStart,FALSE);

		//չ���淨�߷���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_STATIC;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Spread Plane Normal";
		lpInfo->m_strPropHelp="Spread plane's normal direction.";
#else 
		lpInfo->m_strPropName="չ���淨�߷���";
		lpInfo->m_strPropHelp="չ���淨�߷���";
#endif
		CPropTreeItem *pVectorItem = InsertItem(pRootItem,lpInfo,-1);
		pVectorItem->m_idProp = GetPropID("m_xFaceNorm");
		if(GetPropValueStr(pVectorItem->m_idProp,sText))
			pVectorItem->m_lpNodeInfo->m_strPropValue=sText;
		pVectorItem->m_bHideChildren=FALSE;
		if(GetPropStatus(pVectorItem->m_idProp)!=-1)
			pVectorItem->m_bHideChildren = GetPropStatus(pVectorItem->m_idProp);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Coordinate Component";
		lpInfo->m_strPropHelp="Coordinate's component";
#else 
		lpInfo->m_strPropName="�������";
		lpInfo->m_strPropHelp="�������";
#endif
		pGroupItem = InsertItem(pVectorItem,lpInfo,-1);
		pGroupItem->m_idProp = GetPropID("m_xFaceNorm.norm");
		if(GetPropValueStr(pGroupItem->m_idProp,sText))
			pGroupItem->m_lpNodeInfo->m_strPropValue=sText;
		pGroupItem->m_bHideChildren=TRUE;
		if(GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = GetPropStatus(pGroupItem->m_idProp);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Normal's x component";
#else 
		lpInfo->m_strPropHelp="���߷���X�������";
#endif
		pLeafItem = InsertItem(pGroupItem,lpInfo,-1);
		pLeafItem->m_idProp = GetPropID("m_xFaceNorm.norm.x");
		if(GetPropValueStr(pLeafItem->m_idProp,sText))
			pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
		pLeafItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Normal's y component";
#else 
		lpInfo->m_strPropHelp="���߷���Y�������";
#endif
		pLeafItem = InsertItem(pGroupItem,lpInfo,-1);
		pLeafItem->m_idProp = GetPropID("m_xFaceNorm.norm.y");
		if(GetPropValueStr(pLeafItem->m_idProp,sText))
			pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
		pLeafItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Normal's z component";
#else 
		lpInfo->m_strPropHelp="���߷���Z�������";
#endif
		pLeafItem = InsertItem(pGroupItem,lpInfo,-1);
		pLeafItem->m_idProp = GetPropID("m_xFaceNorm.norm.z");
		if(GetPropValueStr(pLeafItem->m_idProp,sText))
			pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
		pLeafItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
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
		pGroupItem = InsertItem(pVectorItem,lpInfo,-1);
		pGroupItem->m_idProp = GetPropID("m_xFaceNorm.norm_style");
		if(GetPropValueStr(pGroupItem->m_idProp,sText))
			pGroupItem->m_lpNodeInfo->m_strPropValue=sText;
		pGroupItem->m_bHideChildren=FALSE;
		if(GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = GetPropStatus(pGroupItem->m_idProp);
		//��������
		UpdateDatumVectorPropItem(pGroupItem,FALSE);
	}

	//װ�䷽λ
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Assemble Position";
	lpInfo->m_strPropHelp = "Part's assemble position and assemble direction.";
#else 
	lpInfo->m_strPropName = "װ�䷽λ";
	lpInfo->m_strPropHelp = "������װ��λ�ü�װ��ʱ��װ�䷽��";
#endif
	pGroupItem = InsertItem(pRootItem,lpInfo, -1);
	pGroupItem->m_idProp = GetPropID("ucs");

	//װ��ƽ��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbItems=CParaCS::GetDatumPlaneCmbItems();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Datum Plane";
	lpInfo->m_strPropHelp = "Specify two axis direction��automatically calculate vertical and curent direction,correct 2nd axis direction and 1st is invariable��Such as:X-Y plane,first calculate z direction by x&y direction,then calculate y direction by x&y direction.";
#else 
	lpInfo->m_strPropName = "��׼��";
	lpInfo->m_strPropHelp = "ָ�������᷽���Զ��������ֱ�뵱ǰ��ķ��򣬵�һ���᷽�򲻱������ڶ����᷽����:X-Y��,����X��Y�����Z�᷽����X��Z������Y�᷽��";
#endif
	pLeafItem = InsertItem(pGroupItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("m_eDatumPlane");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;

	//װ��λ��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Position";
	lpInfo->m_strPropHelp = "Assemble datum point's position.";
#else 
	lpInfo->m_strPropName = "λ��";
	lpInfo->m_strPropHelp = "װ������λ�á�";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("ucs.origin");
	if(GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = GetPropStatus(pPropItem->m_idProp);
	else
		pPropItem->m_bHideChildren = TRUE;

	//װ������ϵԭ��λ��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "The origion point's x coord on assemble CSYS.";
#else 
	lpInfo->m_strPropName = "X����";
	lpInfo->m_strPropHelp = "װ������ϵԭ��λ��X���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.origin.x");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵԭ��λ��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "The origion point's y coord on assemble CSYS.";
#else 
	lpInfo->m_strPropName = "Y����";
	lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Y���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.origin.y");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵԭ��λ��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "The origion point's z coord on assemble CSYS.";
#else 
	lpInfo->m_strPropName = "Z����";
	lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Z���������";
#endif
	pLeafItem= InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.origin.z");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ��ʱX�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Axis Direction";
	lpInfo->m_strPropHelp = "The x axis direction when assembing";
#else 
	lpInfo->m_strPropName = "X�᷽��";
	lpInfo->m_strPropHelp = "װ��ʱX�᷽��";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("ucs.axis_x");
	if(GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = GetPropStatus(pPropItem->m_idProp);
	else
		pPropItem->m_bHideChildren = TRUE;
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//װ������ϵX��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "The x coordinate component of x axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "װ������ϵX��X���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_x.x");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵX��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "The y coordinate component of x axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "װ������ϵX��Y���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_x.y");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵX��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "The z coordinate component of x axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "װ������ϵX��Z���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_x.z");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ��ʱY�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Axis Direction";
	lpInfo->m_strPropHelp = "The y axis direction when assembing";
#else 
	lpInfo->m_strPropName = "Y�᷽��";
	lpInfo->m_strPropHelp = "װ��ʱY�᷽��";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("ucs.axis_y");
	if(GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = GetPropStatus(pPropItem->m_idProp);
	else
		pPropItem->m_bHideChildren = TRUE;
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	
	//װ������ϵY��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "The x coordinate component of y axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "װ������ϵY��X���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_y.x");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵY��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "The y coordinate component of y axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "װ������ϵY��Y���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_y.y");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵY��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "The z coordinate component of y axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "װ������ϵY��Z���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_y.z");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ��ʱZ�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Axis Direction";
	lpInfo->m_strPropHelp = "The z axis direction when assembing";
#else 
	lpInfo->m_strPropName = "Z�᷽��";
	lpInfo->m_strPropHelp = "װ��ʱZ�᷽��";
#endif
	pPropItem = InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = GetPropID("ucs.axis_z");
	if(GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = GetPropStatus(pPropItem->m_idProp);
	else
		pPropItem->m_bHideChildren = TRUE;
	if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//װ������ϵZ��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "The x coordinate component of z axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "װ������ϵZ��X���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_z.x");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵZ��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "The y coordinate component of z axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "װ������ϵZ��Y���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_z.y");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵZ��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "The z coordinate component of z axis on assembling CS.";
#else 
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "װ������ϵZ��Z���������";
#endif
	pLeafItem = InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = GetPropID("ucs.axis_z.z");
	if(GetPropValueStr(pLeafItem->m_idProp,sText))
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();

	Redraw();
}
