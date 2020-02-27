#include "stdafx.h"
#include "XhLdsLm.h"
#include "StdModelPropertyList.h"
#include "..\LdsModel\Tower.h"

///////////////////////////////////////////////////////////////////////////
/*SZ1_MODEL_PARAMS::SZ1_MODEL_PARAMS()
{
	ZB=12400;
	ZC=11200;	//���в㵼�ߺᵣ���߲���²㵼�ߺᵣ���߲�
	//1��һ��ᵣ�������߸��Ϻᵣ)
	//1.1һ������
	Arm1_X1=10400;
	Arm1_X2=7700;
	Arm1_Y1=600;
	Arm1_Y2=1200;
	Arm1_Z1=2300;
	Arm1_Z2=600;
	//1.2��������
	Arm1_X3=800;
	Arm1_Z3=600;
	//2.�ڶ��㵼�ߺᵣ
	Arm2_XB=10100;
	Arm2_YB=1200;
	//3.�����㵼�ߺᵣ
	Arm3_XC=8350;
	Arm3_YC=1400;
}*/
bool SZ1_MODEL_PARAMS::IntelliDesignSlaveParams()
{
	Arm1_X3=800;
	Arm1_Z3=600;
	return true;
}
bool SZ1_MODEL_PARAMS::WriteToModel(IModel* pTowerModel)
{
	if(pTowerModel==NULL||!pTowerModel->IsTowerModel())
		return false;
	CTower* pTower=(CTower*)pTowerModel;
	DESIGN_PARAM_ITEM* pParam;
	if((pParam=pTower->hashParams.GetValue(KEY4C("ZB")))==NULL)
		return false;
	pParam->value.fValue=ZB;//12400;���в㵼�ߺᵣ���߲�
	if((pParam=pTower->hashParams.GetValue(KEY4C("ZC")))==NULL)
		return false;
	pParam->value.fValue=ZC;//11200;���²㵼�ߺᵣ���߲�
	//1��һ��ᵣ�������߸��Ϻᵣ)
	//1.1һ������
	if((pParam=pTower->hashParams.GetValue(KEY4C("X1")))==NULL)
		return false;
	pParam->value.fValue=Arm1_X1;//10400
	if((pParam=pTower->hashParams.GetValue(KEY4C("X2")))==NULL)
		return false;
	pParam->value.fValue=Arm1_X2;//7700
	if((pParam=pTower->hashParams.GetValue(KEY4C("Y1")))==NULL)
		return false;
	pParam->value.fValue=Arm1_Y1;//600

	if((pParam=pTower->hashParams.GetValue(KEY4C("Y2")))==NULL)
		return false;
	pParam->value.fValue=Arm1_Y2;//1200

	if((pParam=pTower->hashParams.GetValue(KEY4C("Z1")))==NULL)
		return false;
	pParam->value.fValue=Arm1_Z1;//2300

	if((pParam=pTower->hashParams.GetValue(KEY4C("Z2")))==NULL)
		return false;
	pParam->value.fValue=Arm1_Z2;//600
	//1.2��������

	if((pParam=pTower->hashParams.GetValue(KEY4C("X3")))==NULL)
		return false;
	pParam->value.fValue=Arm1_X3;//800

	if((pParam=pTower->hashParams.GetValue(KEY4C("Z3")))==NULL)
		return false;
	pParam->value.fValue=Arm1_Z3;//600
	//2.�ڶ��㵼�ߺᵣ
	if((pParam=pTower->hashParams.GetValue(KEY4C("XB")))==NULL)
		return false;
	pParam->value.fValue=Arm2_XB;//10100

	if((pParam=pTower->hashParams.GetValue(KEY4C("YB")))==NULL)
		return false;
	pParam->value.fValue=Arm2_YB;//1200
	//3.�����㵼�ߺᵣ
	if((pParam=pTower->hashParams.GetValue(KEY4C("XC")))==NULL)
		return false;
	pParam->value.fValue=Arm3_XC;//8350

	if((pParam=pTower->hashParams.GetValue(KEY4C("YC")))==NULL)
		return false;
	pParam->value.fValue=Arm3_YC;//1400
	return true;
}
///////////////////////////////////////////////////////////////////////////
CStdModelPropertyList::CStdModelPropertyList()
{
	m_pBelongModel=NULL;
	m_pSketchBitmap=NULL;
	m_pSketchPanel=NULL;
	m_pRelaSubPropList=NULL;
}
CBitmap* CStdModelPropertyList::SetSketchBitmap(CBitmap* pBitmap)
{
	return m_pSketchBitmap=pBitmap;
}

CStatic* CStdModelPropertyList::SetSketchImgPanel(CStatic* pSketchPanel)
{
	return m_pSketchPanel=pSketchPanel;
}
CNativeSlavePropertyList* CStdModelPropertyList::AttachSubPropList(CNativeSlavePropertyList* pSubPropList)
{
	pSubPropList->SetMasterPropList(this);
	return m_pRelaSubPropList=pSubPropList;
}
