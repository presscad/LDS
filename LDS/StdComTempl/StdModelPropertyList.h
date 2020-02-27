#pragma once
#include "..\LdsModel\IModel.h"
//#include ".\IStdComTempl.h"
#include ".\StdTemplPropertyList.h"

class CStdModelPropertyList :public CNativePropertyList
{
protected:
	IModel* m_pBelongModel;
	CBitmap* m_pSketchBitmap;
	CStatic* m_pSketchPanel;
	CNativeSlavePropertyList* m_pRelaSubPropList;
	//irtual CStdComTempl* _InternalGetStdComTempl()=0;
public:
	//CStdComTempl* get_pStdTempl(){return _InternalGetStdComTempl();}
	//__declspec(property(get=get_pStdTempl)) CStdComTempl* pStdTempl;
public:
	CStdModelPropertyList();
	virtual IModel* SetBelongModel(IModel* pModel){return m_pBelongModel=pModel;}
	virtual CBitmap* SetSketchBitmap(CBitmap* pBitmap);
	virtual CStatic* SetSketchImgPanel(CStatic* pSketchPanel);
	virtual CNativeSlavePropertyList* AttachSubPropList(CNativeSlavePropertyList* pSubPropList);
	virtual bool CreateComTemplInstance(){return false;}
	virtual void WorkPlaneSketch(){;}
	virtual void DelWorkPlaneSketch(){;}
	virtual void UpdateSketchMap(){;}
	virtual bool InitializeUI(bool bInternalStart=false,DWORD dwParam=0){return false;}
	virtual bool ConfirmOK(){return false;}
	virtual bool CancelUI(){return false;}
};
struct SZ1_MODEL_PARAMS{
	//0.������Ʋ���
	double ZB,ZC;	//���в㵼�ߺᵣ���߲���²㵼�ߺᵣ���߲�
	//1��һ��ᵣ�������߸��Ϻᵣ)
	//1.1һ������
	double Arm1_X1,Arm1_X2;
	double Arm1_Y1,Arm1_Y2;
	double Arm1_Z1,Arm1_Z2;
	//1.2��������
	double Arm1_X3;
	double Arm1_Z3;
	//2.�ڶ��㵼�ߺᵣ
	double Arm2_XB,Arm2_YB;
	//3.�����㵼�ߺᵣ
	double Arm3_XC,Arm3_YC;
	//SZ1_MODEL_PARAMS();
	bool IntelliDesignSlaveParams();
	bool WriteToModel(IModel* pTowerModel);
};