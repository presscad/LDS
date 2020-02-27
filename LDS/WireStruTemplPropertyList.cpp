#include "stdafx.h"
#include "LDS.h"
#include "I_DrawSolid.h"
#include "btc.h"
#include "WireStruTemplPropertyList.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "CallBackDialog.h"
#include "Fitting.h"

#ifdef __COMMON_PART_INC_
///////////////////////////////////////////////////////////////////////////
CWireStruComPropertyList::CWireStruComPropertyList(void)
{
	params.m_pSetupNode10=params.m_pSetupNode20=params.m_pSetupNode30=params.m_pSetupNode40=NULL;
	m_hSetupNode10=m_hSetupNode20=0;
}

CWireStruComPropertyList::~CWireStruComPropertyList(void)
{
}
CLDSNode* CWireStruComPropertyList::get_pSetupNode10() { return params.m_pSetupNode10; }
CLDSNode* CWireStruComPropertyList::set_pSetupNode10(CLDSNode* _pSetupNode10)
{
	CStdTemplParaNode* pParaNode=pStdTempl->listParaNodes.Add(KEY4C(1));	//N10�ڵ�
	long hSetupNode=_pSetupNode10!=NULL?_pSetupNode10->handle:0;
	pStdTempl->SetMappingNode(1,hSetupNode);
	CLDSNode* pMirNodeX=_pSetupNode10->GetMirNode(MIRMSG(1));
	CStdTemplParaNode* pMirParaNode=pStdTempl->listParaNodes.Add(TMPLOBJID(1,'X'));	//N10�ڵ�
	if (pMirNodeX!=NULL&&pMirParaNode!=NULL)
	{
		pStdTempl->SetMappingNode(TMPLOBJID(1,'X'),pMirNodeX->handle);
		pMirParaNode->pRelaNode=pMirNodeX;
	}
	return params.m_pSetupNode10=pParaNode->pRelaNode=_pSetupNode10;
};
CLDSNode* CWireStruComPropertyList::get_pSetupNode20() { return params.m_pSetupNode20; }
CLDSNode* CWireStruComPropertyList::set_pSetupNode20(CLDSNode* _pSetupNode20)
{
	CStdTemplParaNode* pParaNode=pStdTempl->listParaNodes.Add(KEY4C(2));	//N20�ڵ�
	long hSetupNode=_pSetupNode20!=NULL?_pSetupNode20->handle:0;
	pStdTempl->SetMappingNode(2,hSetupNode);
	CLDSNode* pMirNodeX=_pSetupNode20->GetMirNode(MIRMSG(1));
	CStdTemplParaNode* pMirParaNode=pStdTempl->listParaNodes.Add(TMPLOBJID(2,'X'));	//N10�ڵ�
	if (pMirNodeX!=NULL&&pMirParaNode!=NULL)
	{
		pStdTempl->SetMappingNode(TMPLOBJID(2,'X'),pMirNodeX->handle);
		pMirParaNode->pRelaNode=pMirNodeX;
	}
	return params.m_pSetupNode20=pParaNode->pRelaNode=_pSetupNode20;
};
CLDSNode* CWireStruComPropertyList::get_pSetupNode30() { return params.m_pSetupNode30; }
CLDSNode* CWireStruComPropertyList::set_pSetupNode30(CLDSNode* _pSetupNode30)
{
	CStdTemplParaNode* pParaNode=pStdTempl->listParaNodes.Add(KEY4C(3));	//N30�ڵ�
	long hSetupNode=_pSetupNode30!=NULL?_pSetupNode30->handle:0;
	pStdTempl->SetMappingNode(3,hSetupNode);
	CLDSNode* pMirNodeX=_pSetupNode30->GetMirNode(MIRMSG(1));
	CStdTemplParaNode* pMirParaNode=pStdTempl->listParaNodes.Add(TMPLOBJID(3,'X'));	//N10�ڵ�
	if (pMirNodeX!=NULL&&pMirParaNode!=NULL)
	{
		pStdTempl->SetMappingNode(TMPLOBJID(3,'X'),pMirNodeX->handle);
		pMirParaNode->pRelaNode=pMirNodeX;
	}
	return params.m_pSetupNode30=pParaNode->pRelaNode=_pSetupNode30;
};
CLDSNode* CWireStruComPropertyList::get_pSetupNode40() {
	return params.m_pSetupNode40; 
}
CLDSNode* CWireStruComPropertyList::set_pSetupNode40(CLDSNode* _pSetupNode40)
{
	CStdTemplParaNode* pParaNode=pStdTempl->listParaNodes.Add(KEY4C(4));	//N40�ڵ�
	long hSetupNode=_pSetupNode40!=NULL?_pSetupNode40->handle:0;
	pStdTempl->SetMappingNode(4,hSetupNode);
	CLDSNode* pMirNodeX=_pSetupNode40->GetMirNode(MIRMSG(1));
	CStdTemplParaNode* pMirParaNode=pStdTempl->listParaNodes.Add(TMPLOBJID(4,'X'));	//N40�ڵ�
	if (pMirNodeX!=NULL&&pMirParaNode!=NULL)
	{
		pStdTempl->SetMappingNode(TMPLOBJID(4,'X'),pMirNodeX->handle);
		pMirParaNode->pRelaNode=pMirNodeX;
	}
	return params.m_pSetupNode40=pParaNode->pRelaNode=_pSetupNode40;
};
const DWORD HASHTABLESIZE = 500;
long CWireStruComPropertyList::InitPropHashtable(long initId/*=1*/,long initHashSize/*=50*/,long initStatusHashSize/*=50*/)	//������һ����Id
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(initStatusHashSize);
	//ģ����Ϣ
	AddPropItem("BasicInfo",PROPLIST_ITEM(id++,"������Ϣ"));
	AddPropItem("EB_WIDTH",PROPLIST_ITEM(id++,"���Ԥ�����",""));
	AddPropItem("SetupInfo",PROPLIST_ITEM(id++,"װ��λ��"));
	AddPropItem("WireHoleCount",PROPLIST_ITEM(id++,"���߿���",NULL,"1|3|5"));
	AddPropItem("WireHoleSpace#1-",PROPLIST_ITEM(id++,"1#�׼��"));
	AddPropItem("WireHole#0",PROPLIST_ITEM(id++,"��׼��"));
	AddPropItem("WireHole#1+",PROPLIST_ITEM(id++,"���߿�+1"));
	AddPropItem("WireHole#1-",PROPLIST_ITEM(id++,"���߿�-1"));
	AddPropItem("WireHoleSpace#2-",PROPLIST_ITEM(id++,"2#�׼��"));
	AddPropItem("WireHole#2+",PROPLIST_ITEM(id++,"���߿�+2"));
	AddPropItem("WireHole#2-",PROPLIST_ITEM(id++,"���߿�-2"));
	AddPropItem("WIRE_HOLES_STR",PROPLIST_ITEM(id++,"EB��װ�׾�",""));	//δ��Ӧȥ�������ڱ�����Ϊ�˱������� wjh-2019.8.11
	AddPropItem("WIREANGLE_SIZE",PROPLIST_ITEM(id++,"���߽Ǹֹ��"));
	AddPropItem("BASE_WIREANGLE_SIZE",PROPLIST_ITEM(id++,"�����Ǹֹ��", "���ߵ����Ǹֹ��"));
	AddPropItem("FIX_CONN_BOLTS_N",PROPLIST_ITEM(id++,"�̽���˨��","","2|4"));	//δ��Ӧȥ�������ڱ�����Ϊ�˱������� wjh-2019.8.11
	//��1��ҿ���Ʋ���
	AddPropItem("WirePointGroup1",PROPLIST_ITEM(id++,"1#�ҿ���Ʋ���"));
	AddPropItem("GP1.szFittingType",PROPLIST_ITEM(id++,"���������","","0.EB�Ұ�|1.GD�Ұ�"));
	AddPropItem("GP1.szFittingSizeCode",PROPLIST_ITEM(id++,"������ͺ�",""));
	AddPropItem("GP1.FITTING_RESERVE_WIDTH",PROPLIST_ITEM(id++,"Ԥ����߿��",""));
	AddPropItem("GP1.WIRE_HOLE_D",PROPLIST_ITEM(id++,"Ԥ����װ�׾�",""));
	AddPropItem("GP1.WIRE_HOLE_COUNT",PROPLIST_ITEM(id++,"�ҿ�����",""));
	AddPropItem("GP1.LOCATE_SPACE",PROPLIST_ITEM(id++,"��������L",""));
	AddPropItem("GP1.SwingAngle",PROPLIST_ITEM(id++,"����߰ڶ���","��������ӽ���ƶ����ת���Ƕȣ���λ��"));
	AddPropItem("GP1.URingSizeLabel",PROPLIST_ITEM(id++,"�ҽ�U�ͻ��ͺ�","����U�ͻ��ͺ�"));
	AddPropItem("GP1.URingSwingAngle",PROPLIST_ITEM(id++,"U�ͻ��ڶ���","�ҽ�U�ͻ��ƹҿ׵İڶ��ǣ���λ��"));
	//��2��ҿ���Ʋ���
	AddPropItem("WirePointGroup2",PROPLIST_ITEM(id++,"2#�ҿ���Ʋ���"));
	AddPropItem("GP2.szFittingType",PROPLIST_ITEM(id++,"���������","","0.EB�Ұ�|1.GD�Ұ�"));
	AddPropItem("GP2.szFittingSizeCode",PROPLIST_ITEM(id++,"������ͺ�",""));
	AddPropItem("GP2.FITTING_RESERVE_WIDTH",PROPLIST_ITEM(id++,"Ԥ����߿��",""));
	AddPropItem("GP2.WIRE_HOLE_D",PROPLIST_ITEM(id++,"Ԥ����װ�׾�",""));
	AddPropItem("GP2.WIRE_HOLE_COUNT",PROPLIST_ITEM(id++,"�ҿ�����",""));
	AddPropItem("GP2.LOCATE_SPACE",PROPLIST_ITEM(id++,"��������L",""));
	AddPropItem("GP2.SwingAngle",PROPLIST_ITEM(id++,"����߰ڶ���","��������ӽ���ƶ����ת���Ƕȣ���λ��"));
	AddPropItem("GP2.URingSizeLabel",PROPLIST_ITEM(id++,"�ҽ�U�ͻ��ͺ�","����U�ͻ��ͺ�"));
	AddPropItem("GP2.URingSwingAngle",PROPLIST_ITEM(id++,"U�ͻ��ڶ���","�ҽ�U�ͻ��ƹҿ׵İڶ��ǣ���λ��"));
	//�����ڵ㼰�˼�
	AddPropItem("Node_N10",PROPLIST_ITEM(id++,"�����ڵ�N10",""));
	AddPropItem("Node_N20",PROPLIST_ITEM(id++,"�����ڵ�N20",""));
	AddPropItem("Node_N30",PROPLIST_ITEM(id++,"�����ڵ�N30",""));
	AddPropItem("Node_N40",PROPLIST_ITEM(id++,"�����ڵ�N40",""));
	AddPropItem("ParaRod#1", PROPLIST_ITEM(id++, "�����˼�1", ""));
	AddPropItem("ParaRod#2", PROPLIST_ITEM(id++, "�����˼�2", ""));
	AddPropItem("ParaRod#3", PROPLIST_ITEM(id++, "�����˼�3", ""));
	AddPropItem("ParaRod#4", PROPLIST_ITEM(id++, "�����˼�4", ""));
	AddPropItem("ParaRod#5", PROPLIST_ITEM(id++, "�����˼�5", ""));
	AddPropItem("DimensionParamLevel1",PROPLIST_ITEM(id++,"һ���ߴ���Ʋ���"));
	if (this->pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_FRAMELINE_HYJ1)
	{
		AddPropItem("X1",PROPLIST_ITEM(id++,"���߹ҵ�λ��X1","���߹ҵ㵽���������ߵ�X��ͶӰ����"));
		AddPropItem("X2",PROPLIST_ITEM(id++,"���߹ҵ�λ��X2","���߹ҵ㵽���������ߵ�X��ͶӰ����"));
		AddPropItem("Y1",PROPLIST_ITEM(id++,"���߼ܿ��Y1","���߹ҵ�Y����"));
		AddPropItem("Y2",PROPLIST_ITEM(id++,"���߼ܿ��Y2","���߹ҵ�Y����"));
		AddPropItem("Z1",PROPLIST_ITEM(id++,"����Z1","�����߹ҵ��Z��ֱ���"));
		AddPropItem("Z2",PROPLIST_ITEM(id++,"���ߴ���Z2","���߹ҵ㵽�����洹��λ��(����ֵ)"));
		AddPropItem("DimensionParamLevel2",PROPLIST_ITEM(id++,"�����ߴ���Ʋ���"));
		AddPropItem("X3",PROPLIST_ITEM(id++,"���߼������X3",""));
		AddPropItem("X4",PROPLIST_ITEM(id++,"���߼�λ��X4",""));
		AddPropItem("Z3",PROPLIST_ITEM(id++,"���߼ܸ߶�Z3",""));
		AddPropItem("Z4",PROPLIST_ITEM(id++,"���߼ܸ߶�Z4",""));
		//������������
		AddPropItem("PanelInfo",PROPLIST_ITEM(id++,"�����渹��"));
	}
	else if (this->pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_FRAMELINE_HX3)
	{
		AddPropItem("X",PROPLIST_ITEM(id++,"���߹ҵ�λ��X","���߹ҵ㵽���������ߵ�X��ͶӰ����"));
		AddPropItem("Y",PROPLIST_ITEM(id++,"���߼ܿ��Y","���߹ҵ�Y����"));
		AddPropItem("DimensionParamLevel2",PROPLIST_ITEM(id++,"�����ߴ���Ʋ���"));
		AddPropItem("Z1",PROPLIST_ITEM(id++,"�ᵣ�߶�Z1","���ߺᵣ����Z��ֱ�߶�"));
		AddPropItem("Z2",PROPLIST_ITEM(id++,"�ҵ�����Z2","���߹ҵ����Ĵ���λ��"));
	}
	return id;
}
long CWireStruComPropertyList::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	STDTEMPL_PARAM_ITEM* pParam;
	if (GetPropID("Node_N10")==id)
	{	//��װ��ڵ�N10
		long hNode=pStdTempl->MappingLdsNodeH(1);
		sText.Printf("0x%X",hNode);
	}
	else if (GetPropID("Node_N20")==id)
	{	//��װ��ڵ�N20
		long hNode=pStdTempl->MappingLdsNodeH(2);
		sText.Printf("0x%X",hNode);
	}
	else if (GetPropID("Node_N30")==id)
	{	//��װ��ڵ�N30
		long hNode=pStdTempl->MappingLdsNodeH(3);
		sText.Printf("0x%X",hNode);
	}
	else if (GetPropID("Node_N40")==id)
	{	//��װ��ڵ�N40
		long hNode=pStdTempl->MappingLdsNodeH(4);
		sText.Printf("0x%X",hNode);
	}
	else if (GetPropID("ParaRod#1")==id)
	{	//�˼�
		long *phRod=pStdTempl->hashMatchRodHid.GetValue(1);
		if (phRod)
			sText.Printf("0x%X",*phRod);
		else
			sText.Copy("0x0");
	}
	else if (GetPropID("ParaRod#2") == id)
	{	//�˼�
		long *phRod = pStdTempl->hashMatchRodHid.GetValue(2);
		if (phRod)
			sText.Printf("0x%X", *phRod);
		else
			sText.Copy("0x0");
	}
	else if (GetPropID("ParaRod#2") == id)
	{	//�˼�
		long *phRod = pStdTempl->hashMatchRodHid.GetValue(2);
		if (phRod)
			sText.Printf("0x%X", *phRod);
		else
			sText.Copy("0x0");
	}
	else if (GetPropID("ParaRod#3") == id)
	{	//�˼�
		long *phRod = pStdTempl->hashMatchRodHid.GetValue(3);
		if (phRod)
			sText.Printf("0x%X", *phRod);
		else
			sText.Copy("0x0");
	}
	else if (GetPropID("ParaRod#4") == id)
	{	//�˼�
		long *phRod = pStdTempl->hashMatchRodHid.GetValue(4);
		if (phRod)
			sText.Printf("0x%X", *phRod);
		else
			sText.Copy("0x0");
	}
	else if (GetPropID("ParaRod#5") == id)
	{	//�˼�
		long *phRod = pStdTempl->hashMatchRodHid.GetValue(5);
		if (phRod)
			sText.Printf("0x%X", *phRod);
		else
			sText.Copy("0x0");
	}
	else if (GetPropID("EB_WIDTH")==id)
	{	//EB��߿��
		if ((pParam=pStdTempl->listParams.GetValue(2))==NULL)
		{	//"EBW"��EB�ҵ��߿��+��϶��mm
			pParam=pStdTempl->listParams.Add(2);
			pParam->value.fValue=84;
		}
		sText.Printf("%g",pParam->value.fValue);
	}
	else if (GetPropID("WIRE_HOLES_STR")==id)
	{	//�ҿ��嵥"D2.15@0#50,D26@200#50"
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C('D',1)))!=NULL)
			sText=pParam->value.szStrBuf;
	}
	else if (GetPropID("WIREANGLE_SIZE")==id)
	{	//����T����ϽǸֹ��
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("WAS")))!=NULL)
			sText=pParam->value.szStrBuf;
	}
	else if (GetPropID("BASE_WIREANGLE_SIZE")==id)
	{	//���ߵ����Ǹֹ��
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("BAS")))!=NULL)
			sText=pParam->value.szStrBuf;
	}
	else if (GetPropID("FIX_CONN_BOLTS_N")==id)
	{	//���߶̽ǸֵĹ̽���˨��
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("FBN")))!=NULL)
			sText.Printf("%d",pParam->value.liSafeVal);
	}
	else if (GetPropID("X")==id)
	{	//���߹ҵ�λ��X
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="10400";
	}
	else if (GetPropID("X1")==id)
	{	//���߹ҵ�λ��X1
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X1")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="10400";
	}
	else if (GetPropID("X2")==id)
	{	//���߹ҵ�λ��X2
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X2")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="7700";
	}
	else if (GetPropID("Y")==id)
	{	//���߼ܿ��Y
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Y")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="600";
	}
	else if (GetPropID("Y1")==id)
	{	//���߼ܿ��Y1
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Y1")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="10400";
	}
	else if (GetPropID("Y2")==id)
	{	//���߼ܿ��Y2
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Y2")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="7700";
	}
	else if (GetPropID("Z1")==id)
	{	//�����߲���Z1
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z1")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="2300";
	}
	else if (GetPropID("Z2")==id)
	{	//���ߴ���Z2�����߹ҵ㵽�����洹��λ��(����ֵ)
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z2")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="600";
	}
	else if (GetPropID("X3")==id)
	{	//���߼������X3
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X3")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="800";
	}
	//AddPropItem("X4",PROPLIST_ITEM(id++,"���߼�λ��X4",""));
	else if (GetPropID("Z3")==id)
	{	//���߼ܸ߶�Z3
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z3")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="600";
	}
	else if (GetPropID("Z4")==id)
	{	//���߼ܸ߶�Z4
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z4")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="600";
	}
	//�������
	else if (GetPropID("GP1.szFittingType")==id||GetPropID("GP2.szFittingType")==id)
	{	//�������
		int group=(id==GetPropID("GP2.szFittingType"))?1:0;
		if (this->pWireStruTmpl->xarrWireGroups[group].ciFittingType==CLDSFitting::TYPE_EB)
			sText.Copy("0.EB�Ұ�");
		else if (this->pWireStruTmpl->xarrWireGroups[group].ciFittingType==CLDSFitting::TYPE_GD)
			sText.Copy("1.GD�Ұ�");
		else if (this->pWireStruTmpl->xarrWireGroups[group].ciFittingType==CLDSFitting::TYPE_UR)
			sText.Copy("2.U�͹һ�");
		else if (this->pWireStruTmpl->xarrWireGroups[group].ciFittingType==CLDSFitting::TYPE_US)
			sText.Copy("3.U����˿");
		else if (this->pWireStruTmpl->xarrWireGroups[group].ciFittingType==CLDSFitting::TYPE_UB)
			sText.Copy("4.U�͹Ұ�");
	}
	else if (GetPropID("GP1.szFittingSizeCode")==id||GetPropID("GP2.szFittingSizeCode")==id)
	{	//������ͺ�
		int group=(id==GetPropID("GP2.szFittingSizeCode"))?1:0;
		sText.Copy(pWireStruTmpl->xarrWireGroups[group].szFittingSizeCode);
	}
	else if (GetPropID("GP1.WIRE_HOLE_COUNT")==id||GetPropID("GP2.WIRE_HOLE_COUNT")==id)
	{	//���߽ǸֵĹҿ��������������������
		int group=(id==GetPropID("GP2.WIRE_HOLE_COUNT"))?1:0;
		sText.Printf("%d",pWireStruTmpl->xarrWireGroups[group].cnWirePointCount);
	}
	else if (GetPropID("GP1.WIRE_HOLE_D")==id||GetPropID("GP2.WIRE_HOLE_D")==id)
	{	//Ԥ����߰�װ�׾�
		int group=(id==GetPropID("GP2.WIRE_HOLE_D"))?1:0;
		sText.Printf("%d",pWireStruTmpl->xarrWireGroups[group].cnHoleD);
	}
	else if (GetPropID("GP1.LOCATE_SPACE")==id||GetPropID("GP2.LOCATE_SPACE")==id)
	{	//��������
		int group=(id==GetPropID("GP2.LOCATE_SPACE"))?1:0;
		sText.Printf("%d",pWireStruTmpl->xarrWireGroups[group].wiSpaceL);
	}
	else if (GetPropID("GP1.FITTING_RESERVE_WIDTH")==id||GetPropID("GP2.FITTING_RESERVE_WIDTH")==id)
	{	//Ԥ����߿��
		int group=(id==GetPropID("GP2.FITTING_RESERVE_WIDTH"))?1:0;
		sText.Printf("%d",pWireStruTmpl->xarrWireGroups[group].wnReservedWidth);
	}
	else if (GetPropID("GP1.SwingAngle")==id||GetPropID("GP2.SwingAngle")==id)
	{	//����߰ڶ��ǣ���λ��
		int group=(id==GetPropID("GP2.SwingAngle"))?1:0;
		sText.Printf("%g",pWireStruTmpl->xarrWireGroups[group].sfMasterSwingAngle);
	}
	else if (GetPropID("GP1.URingSizeLabel")==id||GetPropID("GP2.URingSizeLabel")==id)
	{	//�ҽ�U�ͻ��ͺ�
		int group=(id==GetPropID("GP2.URingSizeLabel"))?1:0;
		sText.Copy(pWireStruTmpl->xarrWireGroups[group].szUringSizeCode);
	}
	else if (GetPropID("GP1.URingSwingAngle")==id||GetPropID("GP2.URingSwingAngle")==id)
	{	//�ҽ�U�λ��ڶ��ǣ���λ��
		int group=(id==GetPropID("GP2.URingSwingAngle"))?1:0;
		sText.Printf("%g",pWireStruTmpl->xarrWireGroups[group].sfSlaveUrSwingAngle);
	}
	if (valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
BOOL CWireStruComPropertyList::HelpPromptProperty(CPropTreeItem* pItem)
{
	g_pSolidDraw->ReleaseSnapStatus();
	if (pItem->m_idProp==GetPropID("Node_N10"))
	{
		if (params.m_pSetupNode10)
			g_pSolidDraw->SetEntSnapStatus(params.m_pSetupNode10->handle);
	}
	else if (pItem->m_idProp==GetPropID("Node_N20"))
	{
		if (params.m_pSetupNode20)
			g_pSolidDraw->SetEntSnapStatus(params.m_pSetupNode20->handle);
	}
	return true;
}
#include "PropertyListOper.h"
bool CWireStruComPropertyList::ModifyValueFunc(CPropTreeItem* pItem,CString &valueStr,BOOL *lpRslt)
{
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	STDTEMPL_PARAM_ITEM* pParam;
	long hNode=HexStrToLong(tem_str);
	CNativePropListManager oper(this);//(pPropList,pDlg);
	if (pItem->m_idProp==GetPropID("Node_N10"))		//��װ��ڵ�N10
	{
		if (params.m_pSetupNode10=this->m_pBelongModel->FromNodeHandle(hNode))
			pStdTempl->SetMappingNode(1,params.m_pSetupNode10);
		this->SetItemPropValue(GetPropID("Node_N20"),CXhChar16("0x%X",pStdTempl->MappingLdsNodeH(2)));
		this->SetItemPropValue(GetPropID("Node_N30"),CXhChar16("0x%X",pStdTempl->MappingLdsNodeH(3)));
		this->SetItemPropValue(GetPropID("Node_N40"),CXhChar16("0x%X",pStdTempl->MappingLdsNodeH(4)));
	}
	else if (pItem->m_idProp==GetPropID("Node_N20"))	//��װ��ڵ�N20
	{
		if (params.m_pSetupNode20=this->m_pBelongModel->FromNodeHandle(atoi(tem_str)))
			pStdTempl->SetMappingNode(2,atoi(tem_str));
		this->SetItemPropValue(GetPropID("Node_N30"),CXhChar16("0x%X",pStdTempl->MappingLdsNodeH(3)));
		this->SetItemPropValue(GetPropID("Node_N40"),CXhChar16("0x%X",pStdTempl->MappingLdsNodeH(4)));
	}
	else if (pItem->m_idProp==GetPropID("Node_N30"))	//��װ��ڵ�N20
	{
		params.m_pSetupNode30=this->m_pBelongModel->FromNodeHandle(atoi(tem_str));
		this->SetItemPropValue(GetPropID("Node_N40"),CXhChar16("0x%X",pStdTempl->MappingLdsNodeH(4)));
	}
	else if (pItem->m_idProp==GetPropID("Node_N40"))	//��װ��ڵ�N20
	{
		//params.m_pSetupNode40=this->m_pBelongModel->FromNodeHandle(atoi(tem_str));
	}
	else if (pItem->m_idProp==GetPropID("EB_WIDTH"))	//��װ��ڵ�N20
	{	//EB��߿��
		pParam=pStdTempl->listParams.Add(2);	//"EBW"��EB�ҵ��߿��+��϶��mm
		pParam->value.fValue=atof(valueStr);
	}
	else if (pItem->m_idProp==GetPropID("WIRE_HOLES_STR"))
	{	//EB��װ�׾�
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C('D',1)))!=NULL)
		{
			CXhString xhstr(pParam->value.szStrBuf,96);
			xhstr.Copy(valueStr);
		}
	}
	else if (pItem->m_idProp==GetPropID("WIREANGLE_SIZE"))
	{	//����T����ϽǸֹ��
		pParam=pStdTempl->listParams.Add(KEY4C("WAS"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_STRING;
		StrCopy(pParam->value.szStrBuf,valueStr,96);
	}
	else if (pItem->m_idProp==GetPropID("BASE_WIREANGLE_SIZE"))
	{	//���ߵ����Ǹֹ��
		pParam=pStdTempl->listParams.Add(KEY4C("BAS"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_STRING;
		StrCopy(pParam->value.szStrBuf,valueStr,96);
	}
	else if (pItem->m_idProp==GetPropID("FIX_CONN_BOLTS_N"))
	{	//���߶̽ǸֵĹ̽���˨��
		pParam=pStdTempl->listParams.Add(KEY4C("FBN"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_LONG;
		pParam->value.liValue=atoi(valueStr);
	}
	//}
	else if (GetPropID("X")==pItem->m_idProp)
	{	//���߹ҵ�λ��X
		params.X1=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X")))!=NULL)
			pParam->value.fValue=params.X1;
	}
	else if (GetPropID("X1")==pItem->m_idProp)
	{	//���߹ҵ�λ��X1
		params.X1=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X1")))!=NULL)
			pParam->value.fValue=params.X1;
	}
	else if (GetPropID("X2")==pItem->m_idProp)
	{	//���߹ҵ�λ��X2
		params.X2=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X2")))!=NULL)
			pParam->value.fValue=params.X2;
	}
	else if (GetPropID("Y")==pItem->m_idProp)
	{	//���߹ҵ�λ��X
		params.Y1=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Y")))!=NULL)
			pParam->value.fValue=params.Y1;
	}
	else if (GetPropID("Y1")==pItem->m_idProp)
	{	//���߼ܿ��Y1
		params.Y1=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Y1")))!=NULL)
			pParam->value.fValue=params.Y1;
	}
	else if (GetPropID("Y2")==pItem->m_idProp)
	{	//���߼ܿ��Y2
		params.Y2=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Y2")))!=NULL)
			pParam->value.fValue=params.Y2;
	}
	else if (GetPropID("Z1")==pItem->m_idProp)
	{	//�����߲���Z1
		params.Z1=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z1")))!=NULL)
			pParam->value.fValue=params.Z1;
	}
	else if (GetPropID("Z2")==pItem->m_idProp)
	{	//���ߴ���Z2�����߹ҵ㵽�����洹��λ��(����ֵ)
		params.Z2=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z2")))!=NULL)
			pParam->value.fValue=params.Z2;
	}
	else if (GetPropID("X3")==pItem->m_idProp)
	{	//���߼������X3
		params.X3=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X3")))!=NULL)
			pParam->value.fValue=params.X3;
	}
	//AddPropItem("X4",PROPLIST_ITEM(id++,"���߼�λ��X4",""));
	else if (GetPropID("Z3")==pItem->m_idProp)
	{	//���߼ܸ߶�Z3
		params.Z3=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z3")))!=NULL)
			pParam->value.fValue=params.Z3;
	}
	else if (GetPropID("Z4")==pItem->m_idProp)
	{	//���߼ܸ߶�Z4
		params.Z4=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z4")))!=NULL)
			pParam->value.fValue=params.Z4;
	}
	//�������
	else if (GetPropID("GP1.szFittingType")==pItem->m_idProp||GetPropID("GP2.szFittingType")==pItem->m_idProp)
	{	//�������:0.EB�Ұ�;1.GD�Ұ�;2.U����˨
		int group=(pItem->m_idProp==GetPropID("GP2.szFittingType"))?1:0;
		BYTE ciOldType = pWireStruTmpl->xarrWireGroups[group].ciFittingType;
		pWireStruTmpl->xarrWireGroups[group].ciFittingType=tem_str[0]-'0';
		if (ciOldType != pWireStruTmpl->xarrWireGroups[group].ciFittingType)
		{
			//ƥ����½����Ϣ
			if (pWireStruTmpl->xarrWireGroups[group].ciFittingType == 0)
			{
				FITTING_EB *pEBInfo = MatchFitting<FITTING_EB>(pWireStruTmpl->xarrWireGroups[group].cnHoleD);
				if (pEBInfo == NULL)
				{
					IFittingLibrary<FITTING_EB>* pEBLib = FittingLibrary<FITTING_EB>();
					pEBInfo = pEBLib->GetAt(0);
				}
				StrCopy(pWireStruTmpl->xarrWireGroups[group].szFittingSizeCode, pEBInfo->szSizeCode, 17);
			}
			else if (pWireStruTmpl->xarrWireGroups[group].ciFittingType == 1)
			{
				FITTING_GD *pGDInfo = MatchFitting<FITTING_GD>(pWireStruTmpl->xarrWireGroups[group].cnHoleD);
				if (pGDInfo == NULL)
				{
					IFittingLibrary<FITTING_GD>* pGDLib = FittingLibrary<FITTING_GD>();
					pGDInfo = pGDLib->GetAt(0);
				}
				StrCopy(pWireStruTmpl->xarrWireGroups[group].szFittingSizeCode, pGDInfo->szSizeCode, 17);
			}
			char* pszSizeLabel = (group == 0) ? "GP1.szFittingSizeCode" : "GP2.szFittingSizeCode";
			oper.UpdatePropItemValue(pszSizeLabel);
			CPropTreeItem* pFindItem = FindItemByPropId(GetPropID(pszSizeLabel), NULL);
			if (pFindItem)
			{
				if (pWireStruTmpl->xarrWireGroups[group].ciFittingType == 0)
					pFindItem->m_lpNodeInfo->m_cmbItems = MakeFittingSetString(CLDSFitting::TYPE_EB);
				else if (pWireStruTmpl->xarrWireGroups[group].ciFittingType == 1)
					pFindItem->m_lpNodeInfo->m_cmbItems = MakeFittingSetString(CLDSFitting::TYPE_GD);
				else
					pFindItem->m_lpNodeInfo->m_cmbItems = MakeFittingSetString(CLDSFitting::TYPE_UR);
			}
			//ƥ�����U�λ��ͺ�
			CLDSFitting fitting(pWireStruTmpl->xarrWireGroups[group].ciFittingType,
				pWireStruTmpl->xarrWireGroups[group].szFittingSizeCode);
			fitting.MatchUring();
			StrCopy(pWireStruTmpl->xarrWireGroups[group].szUringSizeCode, fitting.sizespecUR, 17);
			char* pszUringSizeLabel = (group == 0) ? "GP1.URingSizeLabel" : "GP2.URingSizeLabel";
			oper.UpdatePropItemValue(pszUringSizeLabel);
			//ƥ����¹ҿ�ֱ��
			pWireStruTmpl->xarrWireGroups[group].cnShaftD = (BYTE)ftoi(fitting.basic.dfTrunnionD);
			pWireStruTmpl->xarrWireGroups[group].cnHoleD = pWireStruTmpl->xarrWireGroups[group].cnShaftD + 2;
			char* pszWireHoleD = (group == 0) ? "GP1.WIRE_HOLE_D" : "GP2.WIRE_HOLE_D";
			oper.UpdatePropItemValue(pszWireHoleD);
			//����Ԥ����߰�װ���,ͬ����"EBW"��EB�ҵ��߿��+��϶��mm
			pWireStruTmpl->xarrWireGroups[group].wnFittingWidth = ftoi(fitting.basic.dfBoardL);
			pWireStruTmpl->xarrWireGroups[group].wnReservedWidth = pWireStruTmpl->xarrWireGroups[group].wnFittingWidth + 4;
			pParam = pWireStruTmpl->listParams.Add(2);
			pParam->value.fValue = pWireStruTmpl->xarrWireGroups[group].wnReservedWidth;
			oper.UpdatePropItemValue("EB_WIDTH");
		}
	}
	else if (GetPropID("GP1.szFittingSizeCode")==pItem->m_idProp||GetPropID("GP2.szFittingSizeCode")==pItem->m_idProp)
	{	//������ͺ�
		int group=(pItem->m_idProp==GetPropID("GP2.szFittingSizeCode"))?1:0;
		StrCopy(pWireStruTmpl->xarrWireGroups[group].szFittingSizeCode,tem_str,17);
		CLDSFitting fitting(pWireStruTmpl->xarrWireGroups[group].ciFittingType,
							pWireStruTmpl->xarrWireGroups[group].szFittingSizeCode);
		fitting.MatchUring();
		StrCopy(pWireStruTmpl->xarrWireGroups[group].szUringSizeCode,fitting.sizespecUR,17);
		//ƥ�����U�λ��ͺ�
		char* pszUringSizeLabel=(group==0)?"GP1.URingSizeLabel":"GP2.URingSizeLabel";
		oper.UpdatePropItemValue(pszUringSizeLabel);
		//ƥ����¹ҿ�ֱ��
		pWireStruTmpl->xarrWireGroups[group].cnShaftD=(BYTE)ftoi(fitting.basic.dfTrunnionD);
		pWireStruTmpl->xarrWireGroups[group].cnHoleD=(BYTE)ftoi(fitting.basic.dfTrunnionD)+2;
		char* pszWireHoleD=(group==0)?"GP1.WIRE_HOLE_D":"GP2.WIRE_HOLE_D";
		oper.UpdatePropItemValue(pszWireHoleD);
		//����Ԥ����߰�װ���,ͬ����"EBW"��EB�ҵ��߿��+��϶��mm
		pWireStruTmpl->xarrWireGroups[group].wnFittingWidth=ftoi(fitting.basic.dfBoardL);
		pWireStruTmpl->xarrWireGroups[group].wnReservedWidth=ftoi(fitting.basic.dfBoardL)+4;
		char* pszFittingSpace=(group==0)?"GP1.FITTING_RESERVE_WIDTH":"GP2.FITTING_RESERVE_WIDTH";
		oper.UpdatePropItemValue(pszFittingSpace);
		pParam=pWireStruTmpl->listParams.Add(2);
		pParam->value.fValue=pWireStruTmpl->xarrWireGroups[group].wnReservedWidth;
		oper.UpdatePropItemValue("EB_WIDTH");
	}
	else if (GetPropID("GP1.WIRE_HOLE_COUNT")==pItem->m_idProp||GetPropID("GP2.WIRE_HOLE_COUNT")==pItem->m_idProp)
	{	//���߽ǸֵĹҿ��������������������
		int group=(pItem->m_idProp==GetPropID("GP2.WIRE_HOLE_COUNT"))?1:0;
		pWireStruTmpl->xarrWireGroups[group].cnWirePointCount=atoi(tem_str);
	}
	else if (GetPropID("GP1.WIRE_HOLE_D")==pItem->m_idProp||GetPropID("GP2.WIRE_HOLE_D")==pItem->m_idProp)
	{	//Ԥ����߰�װ�׾�
		int group=(pItem->m_idProp==GetPropID("GP2.WIRE_HOLE_D"))?1:0;
		pWireStruTmpl->xarrWireGroups[group].cnHoleD=atoi(tem_str);
	}
	else if (GetPropID("GP1.LOCATE_SPACE")==pItem->m_idProp||GetPropID("GP2.LOCATE_SPACE")==pItem->m_idProp)
	{	//��������
		int group=(pItem->m_idProp==GetPropID("GP2.LOCATE_SPACE"))?1:0;
		pWireStruTmpl->xarrWireGroups[group].wiSpaceL=atoi(tem_str);
	}
	else if (GetPropID("GP1.FITTING_RESERVE_WIDTH")==pItem->m_idProp||GetPropID("GP2.FITTING_RESERVE_WIDTH")==pItem->m_idProp)
	{	//Ԥ����߿��
		int group=(pItem->m_idProp==GetPropID("GP2.FITTING_RESERVE_WIDTH"))?1:0;
		pWireStruTmpl->xarrWireGroups[group].wnReservedWidth=atoi(tem_str);
	}
	else if (GetPropID("GP1.SwingAngle")==pItem->m_idProp||GetPropID("GP2.SwingAngle")==pItem->m_idProp)
	{	//����߰ڶ��ǣ���λ��
		int group=(pItem->m_idProp==GetPropID("GP2.SwingAngle"))?1:0;
		pWireStruTmpl->xarrWireGroups[group].sfMasterSwingAngle=(float)atof(tem_str);
	}
	else if (GetPropID("GP1.URingSizeLabel")==pItem->m_idProp||GetPropID("GP2.URingSizeLabel")==pItem->m_idProp)
	{	//�ҽ�U�ͻ��ͺ�
		int group=(pItem->m_idProp==GetPropID("GP2.URingSizeLabel"))?1:0;
		StrCopy(pWireStruTmpl->xarrWireGroups[group].szUringSizeCode,tem_str,17);
	}
	else if (GetPropID("GP1.URingSwingAngle")==pItem->m_idProp||GetPropID("GP2.URingSwingAngle")==pItem->m_idProp)
	{	//�ҽ�U�λ��ڶ��ǣ���λ��
		int group=(pItem->m_idProp==GetPropID("GP2.URingSwingAngle"))?1:0;
		pWireStruTmpl->xarrWireGroups[group].sfSlaveUrSwingAngle=(float)atof(tem_str);
	}
	return TRUE;
}
bool CWireStruComPropertyList::ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt)
{
	CCallBackDialog* pCallBackDlg=(CCallBackDialog*)GetParent();
	if (pItem->m_idProp==GetPropID("Node_N10"))
		pCallBackDlg->SelectObject(CLS_NODE,pItem->m_idProp);
	else if (pItem->m_idProp==GetPropID("Node_N20"))
		pCallBackDlg->SelectObject(CLS_NODE,pItem->m_idProp);
	else if(pItem->m_idProp == GetPropID("Node_N30"))
		pCallBackDlg->SelectObject(CLS_NODE, pItem->m_idProp);
	else if(pItem->m_idProp == GetPropID("Node_N40"))
		pCallBackDlg->SelectObject(CLS_NODE, pItem->m_idProp);
	return TRUE;
}
bool CWireStruComPropertyList::CreateComTemplInstance()
{
	if (!this->pWireStruTmpl->TranslateTmplInstance())
		return false;
	CStdComModel modeler(&Ta);
	modeler.BuildModel(this->pStdTempl);
	return TRUE;
}
bool CWireStruComPropertyList::CallBackCommand(UINT message,WPARAM wParam,BOOL *lpRslt)
{
	return true;
}
BOOL CWireStruComPropertyList::FinishSelectObjOper(long hPickObj,long idEventProp)
{
	CCallBackDialog* pCallBackDlg=(CCallBackDialog*)GetParent();
	CLDSNode* pNode=NULL;
	if (idEventProp==GetPropID("Node_N10"))
	{
		if ((pNode=m_pBelongModel->FromNodeHandle(hPickObj))!=NULL)
		{
			this->pSetupNode10=pNode;
			this->pStdTempl->SetMappingNode(1,pNode);
			if (pStdTempl->Id==IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM1||
				pStdTempl->Id==IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM2||
				pStdTempl->Id==IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM3||
				pStdTempl->Id==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_EB||
				pStdTempl->Id==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_2_EB||
				pStdTempl->Id==IStdComTempl::STDTMPL_STRU_META_SECT_FRT_1_GD||
				pStdTempl->Id==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_GD||
				pStdTempl->Id==IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM1)
			{
				MAP_RODnNODE keynode(1,pSetupNode10);
				((CStdComTemplWireStructure*)pStdTempl)->SetRecogKeyNodesAndRods(&keynode);			
			}
		}
	}
	else if (idEventProp==GetPropID("Node_N20"))
	{
		if ((pNode=m_pBelongModel->FromNodeHandle(hPickObj))!=NULL)
		{
			this->pSetupNode20=pNode;
			this->pStdTempl->SetMappingNode(2,pNode);
			if (pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_NZ_FRT_2_GD||
				pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_NZ_BTM_2_GD||
				pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM2)
			{
				if (pSetupNode10!=NULL&&pSetupNode20!=NULL)
				{
					MAP_RODnNODE keynodes[2];
					keynodes[0].Set(1,pSetupNode10);
					keynodes[1].Set(2,pSetupNode20);
					((CStdComTemplWireStructure*)pStdTempl)->SetRecogKeyNodesAndRods(keynodes,2);
				}
			}
		}
	}
	else if (idEventProp == GetPropID("Node_N30"))
	{
		if ((pNode = m_pBelongModel->FromNodeHandle(hPickObj)) != NULL)
		{
			this->pSetupNode30 = pNode;
			this->pStdTempl->SetMappingNode(3, pNode);
			if (pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_NZ_BTM_3_GD)
			{
				if (pSetupNode10 != NULL && pSetupNode20 != NULL && this->pSetupNode30!=NULL)
				{
					MAP_RODnNODE keynodes[3];
					keynodes[0].Set(1, pSetupNode10);
					keynodes[1].Set(2, pSetupNode20);
					keynodes[2].Set(3, pSetupNode30);
					((CStdComTemplWireStructure*)pStdTempl)->SetRecogKeyNodesAndRods(keynodes, 3);
				}
			}
		}
	}
	else if (idEventProp == GetPropID("Node_N40"))
	{
		if ((pNode = m_pBelongModel->FromNodeHandle(hPickObj)) != NULL)
		{
			this->pSetupNode40 = pNode;
			this->pStdTempl->SetMappingNode(4, pNode);
		}
	}
	else
		return false;
	return true;
}
void CWireStruComPropertyList::DisplayPropertyList(int overwrite0_append1/*=0*/,DWORD dwPropGroup/*=1*/)
{
	CleanTree();
	CNativePropListManager oper(this);
	CPropTreeItem *pBasicItem=NULL,*pGroupItem=NULL,*pPropItem=NULL,*pRoot=GetRootItem();
	if (this->pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM1 )
	{	//EB�ҵ�
		//������Ϣ
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB��߿��
		//װ����Ϣ
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10");
		oper.InsertButtonPropItem(pGroupItem,"Node_N20");
		oper.InsertButtonPropItem(pGroupItem,"Node_N30");
		oper.InsertButtonPropItem(pGroupItem,"Node_N40");
#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#2");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#3");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#4");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#5");
#endif
		//��һ��ҿײ���
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#�ҿ���Ʋ���");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//�ҿ�����
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//Ԥ����߰�װ�׾�
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB����ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//�ҽ�U�ͻ��ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
		//�ڶ���ҿײ���
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup2", "2#�ҿ���Ʋ���");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP2.WIRE_HOLE_COUNT");	//�ҿ�����
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP2.LOCATE_SPACE");		//��������
		oper.InsertEditPropItem(pGroupItem, "GP2.WIRE_HOLE_D");
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP2.szFittingType");
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP2.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB����ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP2.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP2.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//�ҽ�U�ͻ��ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP2.URingSwingAngle");
	}
	else if (this->pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM2)
	{	//EB�ҵ�
		//������Ϣ
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB��߿��
		//װ����Ϣ
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10");
		oper.InsertButtonPropItem(pGroupItem,"Node_N20");
		oper.InsertButtonPropItem(pGroupItem,"Node_N30");
		oper.InsertButtonPropItem(pGroupItem,"Node_N40");
#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#2");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#3");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#4");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#5");
#endif
		//��һ��ҿײ���
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#�ҿ���Ʋ���");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//�ҿ�����
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//Ԥ����߰�װ�׾�
		oper.InsertEditPropItem(pGroupItem, "GP1.LOCATE_SPACE");		//��������
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB����ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//�ҽ�U�ͻ��ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (this->pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM3)
	{
		//������Ϣ
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB��߿��
		//װ����Ϣ
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem, "Node_N10", "�����ڵ�N10");
		oper.InsertButtonPropItem(pGroupItem, "Node_N20", "�����ڵ�N20");
		oper.InsertButtonPropItem(pGroupItem, "Node_N30", "�����ڵ�N30");
#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#2");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#4","T�ͺ�ĽǸ�");
#endif
		//��һ��ҿײ���
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#�ҿ���Ʋ���");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//�ҿ�����
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.LOCATE_SPACE");		//��������
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//Ԥ����߰�װ�׾�
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB����ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//�ҽ�U�ͻ��ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (this->pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM1 )
	{	//EB�ҵ�
		//������Ϣ
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB��߿��
		//װ����Ϣ
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10","�����ڵ�N10");
		oper.InsertButtonPropItem(pGroupItem,"Node_N20","�ڵ�N20");
		oper.InsertButtonPropItem(pGroupItem,"Node_N30","�ڵ�N30");
#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#2");
#endif
		//��һ��ҿײ���
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#�ҿ���Ʋ���");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//�ҿ�����
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//Ԥ����߰�װ�׾�
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB����ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//�ҽ�U�ͻ��ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (this->pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM2)
	{
		//������Ϣ
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertCmbListPropItem(pBasicItem, "WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB��߿��
		oper.InsertCmbEditPropItem(pBasicItem, "FIX_CONN_BOLTS_N");
		//װ����Ϣ
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem, "Node_N10", "�����ڵ�N10");
		oper.InsertButtonPropItem(pGroupItem, "Node_N20", "�����ڵ�N20");

#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#2");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#3");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#4");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#5");
#endif
		//�ҿ���Ʋ���
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#�ҿ���Ʋ���");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//�ҿ�����
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//Ԥ����߰�װ�׾�
		this->pWireStruTmpl->xarrWireGroups[0].ciFittingType = CLDSFitting::TYPE_EB;	//��ǰ��������趨ΪEB����
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");		//�������
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB����ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//�ҽ�U�ͻ��ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (this->pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM3)
	{
		//������Ϣ
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB��߿��
		//װ����Ϣ
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem, "Node_N10", "�����ڵ�N10");
		oper.InsertButtonPropItem(pGroupItem, "Node_N20", "�����ڵ�N20");
		oper.InsertButtonPropItem(pGroupItem, "Node_N30", "�����ڵ�N30");
#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#2");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#4","T�ͺ�ĽǸ�");
#endif
		//��һ��ҿײ���
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#�ҿ���Ʋ���");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//�ҿ�����
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.LOCATE_SPACE");		//��������
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//Ԥ����߰�װ�׾�
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB����ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//�ҽ�U�ͻ��ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_EB||
		pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_2_EB)
	{	//EB�ҵ�
		//������Ϣ
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertCmbListPropItem(pBasicItem, "WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB��߿��
		oper.InsertCmbEditPropItem(pBasicItem, "FIX_CONN_BOLTS_N");
		//װ����Ϣ
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10","�����ڵ�N10");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#1","T�ͺ�ĽǸ�");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#2", "�ᵣ���ĽǸ�");
		//�ҿ���Ʋ���
		pGroupItem=oper.InsertPropItem(pRoot,"WirePointGroup1","1#�ҿ���Ʋ���");
		pPropItem=oper.InsertEditPropItem(pGroupItem,"GP1.WIRE_HOLE_COUNT");	//�ҿ�����
		pPropItem->SetReadOnly();
		if (pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_META_SECT_BTM_2_EB)
			oper.InsertEditPropItem(pGroupItem, "GP1.LOCATE_SPACE");		//��������
		oper.InsertEditPropItem(pGroupItem,"GP1.WIRE_HOLE_D");		//Ԥ����߰�װ�׾�
		this->pWireStruTmpl->xarrWireGroups[0].ciFittingType=CLDSFitting::TYPE_EB;	//��ǰ��������趨ΪEB����
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"GP1.szFittingType");		//�������
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem,"GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB����ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem,"GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//�ҽ�U�ͻ��ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_META_SECT_FRT_1_GD||
		pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_GD)
	{	//GD�ҵ�
		//������Ϣ
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertCmbListPropItem(pBasicItem, "WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertCmbListPropItem(pBasicItem, "BASE_WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB��߿��
		oper.InsertCmbEditPropItem(pBasicItem, "FIX_CONN_BOLTS_N");
		//װ����Ϣ
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10","�����ڵ�N10");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#1","�ᵣ�������ĽǸ�");
		//�ҿ���Ʋ���
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#�ҿ���Ʋ���");
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//�ҿ�����
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//Ԥ����߰�װ�׾�
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//Ԥ����߰�װ�׾�
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");		//�������
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_GD));	//EB����ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//�ҽ�U�ͻ��ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_NZ_FRT_2_GD)
	{	//GD�ҵ�
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertCmbListPropItem(pBasicItem, "WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertCmbListPropItem(pBasicItem, "BASE_WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB��߿��
		oper.InsertCmbEditPropItem(pBasicItem, "FIX_CONN_BOLTS_N");
		//
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10","�����ڵ�N10");
		oper.InsertButtonPropItem(pGroupItem,"Node_N20","�����ڵ�N20");
		oper.InsertButtonPropItem(pGroupItem,"Node_N30","�ڵ�N30");
		oper.InsertButtonPropItem(pGroupItem,"Node_N40","�ڵ�N40");
#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#2");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#3");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#4");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#5");
#endif
		//�ҿ���Ʋ���
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#�ҿ���Ʋ���");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//�ҿ�����
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//Ԥ����߰�װ�׾�
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");		//�������
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_GD));	//EB����ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//�ҽ�U�ͻ��ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_NZ_BTM_2_GD)
	{
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertCmbListPropItem(pBasicItem, "WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertCmbListPropItem(pBasicItem, "BASE_WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB��߿��
		oper.InsertCmbEditPropItem(pBasicItem, "FIX_CONN_BOLTS_N");
		//
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem, "Node_N10", "�����ڵ�N10");
		oper.InsertButtonPropItem(pGroupItem, "Node_N20", "�����ڵ�N20");
	#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#2");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#3");
	#endif
		//�ҿ���Ʋ���
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#�ҿ���Ʋ���");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//�ҿ�����
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//Ԥ����߰�װ�׾�
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");		//�������
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_GD));	//EB����ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//�ҽ�U�ͻ��ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_NZ_BTM_3_GD)
	{	//GD�ҵ�
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertCmbListPropItem(pBasicItem, "WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertCmbListPropItem(pBasicItem, "BASE_WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB��߿��
		oper.InsertCmbEditPropItem(pBasicItem, "FIX_CONN_BOLTS_N");
		//
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem, "Node_N10", "�����ڵ�N10");
		oper.InsertButtonPropItem(pGroupItem, "Node_N20", "�����ڵ�N20");
		oper.InsertButtonPropItem(pGroupItem, "Node_N30", "�����ڵ�N30");
	#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#1");
	#endif
		//�ҿ���Ʋ���
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#�ҿ���Ʋ���");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//�ҿ�����
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//Ԥ����߰�װ�׾�
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");		//�������
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_GD));	//EB����ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//�ҽ�U�ͻ��ͺ�
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (this->pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_FRAMELINE_HYJ1)
	{
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10","��װ��ڵ�N10");
		oper.InsertButtonPropItem(pGroupItem,"Node_N20","��װ��ڵ�N20");
		//һ���ߴ���Ʋ���	
		pGroupItem=oper.InsertPropItem(pRoot,"DimensionParamLevel1");
		oper.InsertEditPropItem(pGroupItem,"X1");	//���߹ҵ����λ��X1
		oper.InsertEditPropItem(pGroupItem,"X2");	//���߹ҵ����λ��X2
		oper.InsertEditPropItem(pGroupItem,"Y1");	//���߼ܿ��Y1
		oper.InsertEditPropItem(pGroupItem,"Y2");	//���߼ܿ��Y2
		oper.InsertEditPropItem(pGroupItem,"Z1");	//����Z1
		oper.InsertEditPropItem(pGroupItem,"Z2");	//���ߴ���Z2
		//�����ߴ���Ʋ���	
		pGroupItem=oper.InsertPropItem(pRoot,"DimensionParamLevel2");
		oper.InsertEditPropItem(pGroupItem,"X3");	//����֧�������X3
		//oper.InsertEditPropItem(pGroupItem,"X4");	//���߼ܸ�������λ��X4
		oper.InsertEditPropItem(pGroupItem,"Z3");	//���߼ܸ߶�Z3
		oper.InsertEditPropItem(pGroupItem,"Z4");	//���߼ܸ߶�Z4
		//�����渹��������
		pGroupItem=oper.InsertPropItem(pRoot,"PanelInfo");
	}
	else if (this->pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_FRAMELINE_HX3)
	{
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10","��װ��ڵ�N10");
		oper.InsertButtonPropItem(pGroupItem,"Node_N20","��װ��ڵ�N20");
		pGroupItem=oper.InsertPropItem(pRoot,"DimensionParamLevel1");
		oper.InsertEditPropItem(pGroupItem,"X");	//���߹ҵ����λ��X1
		oper.InsertEditPropItem(pGroupItem,"Y");	//���߼ܿ��Y1
		//�����ߴ���Ʋ���	
		pGroupItem=oper.InsertPropItem(pRoot,"DimensionParamLevel2");
		oper.InsertEditPropItem(pGroupItem,"Z1");	//����Z1
		oper.InsertEditPropItem(pGroupItem,"Z2");	//���ߴ���Z2
	}
	//
	Redraw();
}
bool CWireStruComPropertyList::DisplaySlavePropertyList(CPropTreeItem* pItem,DWORD dwRelaInfo/*=0*/)
{
	if (this->m_pRelaSubPropList==NULL)
		return false;
	CNativePropListManager oper(m_pRelaSubPropList);
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pRoot=m_pRelaSubPropList->GetRootItem();
	m_pRelaSubPropList->CleanTree();
	//m_pRelaSubPropList->SetModifyValueFunc(ModifyProperty);
	//
	//pGroupItem=oper.InsertPropItem(pRoot,"PlateInfo");
	//oper.InsertEditPropItem(pGroupItem,"PlateThick");
	//oper.InsertCmbListPropItem(pGroupItem,"PlateMat",MakeMaterialMarkSetString());
	//if(params.m_ciWalkWayType==0)
	//	oper.InsertEditPropItem(pGroupItem,"para.A");
	//else
	//{
	//	oper.InsertEditPropItem(pGroupItem,"para.B");
	//	oper.InsertEditPropItem(pGroupItem,"para.W");
	//	//oper.InsertEditPropItem(pGroupItem,"para.H");
	//	//
	//	oper.InsertCmbEditPropItem(pGroupItem,"BoltMd","12|16|20|24","","",-1,TRUE);
	//	oper.InsertEditPropItem(pGroupItem,"BoltNum","","",-1,TRUE);
	//	oper.InsertEditPropItem(pGroupItem,"BoltSpace","","",-1,TRUE);
	//}
	////
	//m_pRelaSubPropList->Redraw();
	return true;
}
void CWireStruComPropertyList::UpdateSketchMap()
{
	if (m_pSketchBitmap==NULL)
		return;
	if (pStdTempl->sketchimg.bmWidth==0)
		pStdTempl->LoadSketchImage();
	m_pSketchBitmap->Detach();
	m_pSketchBitmap->CreateBitmapIndirect(&this->pStdTempl->sketchimg);
	HBITMAP hBitmap=(HBITMAP)m_pSketchBitmap->GetSafeHandle();
	m_pSketchPanel->SetBitmap(hBitmap);
}
void CWireStruComPropertyList::DelWorkPlaneSketch()
{
	g_pSolidDraw->DelWorkPlaneSketch(1);
	g_pSolidDraw->DelWorkPlaneSketch(2);
	g_pSolidDraw->Draw();
}
void CWireStruComPropertyList::WorkPlaneSketch()
{

}
bool CWireStruComPropertyList::InitializeUI(bool bInternalStart/*=false*/,DWORD idStdComTmpl/*=0*/)
{
	if (!bInternalStart)
	{
		CStdComTempl* pWireStruTmpl=globalStdComTmplLibrary.LoadStdComTempl(idStdComTmpl);
		if (pWireStruTmpl == NULL)
			return false;
		xWireStruTmpl.Id = idStdComTmpl;
		xWireStruTmpl.CloneStdTmplFrom(pWireStruTmpl);
		//
		InitPropHashtable();
	}
	UpdateSketchMap();
	WorkPlaneSketch();
	return true;
}
bool CWireStruComPropertyList::ConfirmOK()
{
	return true;
}
bool CWireStruComPropertyList::CancelUI()
{
	DelWorkPlaneSketch();
	return true;
}
#endif