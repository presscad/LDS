// Bolt.cpp: implementation of the C3dLs class.
//wbt translation
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Hashtable.h"
#include "PropListItem.h"
#include "PartLib.h"
#include "FoundationParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLDSFoundation
//////////////////////////////////////////////////////////////////////////
// FOUNDATION_PARA
IMPLEMENT_PROP_FUNC(FOUNDATION_PARA)

FOUNDATION_PARA::FOUNDATION_PARA()
{
	biBasePlateType=1;		//0.������ƽ�� 1.������ƽ�� 2.�����м� 
	wiBasePlateThick=40;	//������
	wiBasePlateWidth=800;	//������
	wiBasePlateHoleD=40;	//����׾�
	biLayoutType=0;			//�ؽ���˨�������ͣ�0.4��˨ 1.8��˨
	wiBoltD=42;				//�ؽ���˨ֱ��
	wiBoltSapce=S2=200;		//��˨���
	wiFoundationWidth=1200;	//�������
	//���Ű�ѥ��/�߰���Ʋ���
	cnMainAngleBoltNum = 5;
	cnMaxSingularRowBolts = 4;
	cnRows=0;	//����������˨������������,0.��ʾ������˨����cnMaxSingularRowBolts�жϵ�˫��;1.����;2.˫��
	wiShoeThick = 16;
	wiRibThick = 10;
	wiRibMargin = 25;				//�װ��ȡ�ѥ���ȣ��߰��ȡ��߾�
	wRibWidth = 200;		//�߰���(�װ庸�ӱ�)
	wRibOuterHeight = 200;	//�߰����߶�
	wRibInnerHeight = 200;	//�߰��ڲ�߶�
}
void FOUNDATION_PARA::InitAnchorsLocation()
{
	if(biLayoutType==0)
	{	//����˨����
		anchors.Append(LOCATION( wiBoltSapce, wiBoltSapce));
		anchors.Append(LOCATION(-wiBoltSapce, wiBoltSapce));
		anchors.Append(LOCATION(-wiBoltSapce,-wiBoltSapce));
		anchors.Append(LOCATION( wiBoltSapce,-wiBoltSapce));
	}
	else if(biLayoutType==1)
	{	//����˨����
		anchors.Append(LOCATION( wiBoltSapce+S2, wiBoltSapce));
		anchors.Append(LOCATION( wiBoltSapce   , wiBoltSapce+S2));
		anchors.Append(LOCATION(-wiBoltSapce   , wiBoltSapce+S2));
		anchors.Append(LOCATION(-wiBoltSapce-S2, wiBoltSapce));
		anchors.Append(LOCATION(-wiBoltSapce-S2,-wiBoltSapce));
		anchors.Append(LOCATION(-wiBoltSapce   ,-wiBoltSapce-S2));
		anchors.Append(LOCATION( wiBoltSapce   ,-wiBoltSapce-S2));
		anchors.Append(LOCATION( wiBoltSapce+S2,-wiBoltSapce));
	}
}
CXhChar200 GetAnchorBoltCmbSizeStr()
{
	CXhChar200 szSizeList;
	int i,count=ANCHOR_BOLT::GetCount();
	for(i=0;i<count;i++)
	{
		const ANCHOR_BOLT* pAnchorBolt=ANCHOR_BOLT::RecordAt(i);
		szSizeList.Append(CXhChar16("%d",pAnchorBolt->d),'|');
	}
	return szSizeList;
}
void FOUNDATION_PARA::InitPropHashtable()
{
	int id = 1;
	const DWORD HASHTABLESIZE = 500;
	const DWORD STATUSHASHTABLESIZE = 50;
	propHashtable.Empty();
	propStatusHashtable.Empty();
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	AddPropItem("PrimaryAngle",PROPLIST_ITEM(id++,"(��)��������"));
	AddPropItem("ConnBoltCountOfPrimaryAngle",PROPLIST_ITEM(id++,"������˨��","���ĵ�������˨��",""));
	AddPropItem("ConnBoltRowsOfPrimaryAngle",PROPLIST_ITEM(id++,"��˨����","����������˨��������","1.����|2.˫��"));
	AddPropItem("BasePlate",PROPLIST_ITEM(id++,"���Ű�"));
	AddPropItem("biBasePlateType",PROPLIST_ITEM(id++,"�װ�����","�װ�����","0.������ƽ��|1.������ƽ��|2.�����м�|"));
	AddPropItem("wiBasePlateHoleD",PROPLIST_ITEM(id++,"�װ�׾�"));
	AddPropItem("wiBasePlateThick",PROPLIST_ITEM(id++,"�װ���"));
	AddPropItem("wiBasePlateWidth",PROPLIST_ITEM(id++,"�װ���"));
	AddPropItem("wiShoePlateThick",PROPLIST_ITEM(id++,"ѥ����"));
	AddPropItem("wiShoePlateWidth",PROPLIST_ITEM(id++,"ѥ����"));
	AddPropItem("wiRibPlateThick",PROPLIST_ITEM(id++,"�߰���"));
	AddPropItem("wiRibPlateWidth",PROPLIST_ITEM(id++,"�߰���","�װ庸�ӱ߿��"));
	AddPropItem("wiRibPlateHeightOuter",PROPLIST_ITEM(id++,"�߰�(��)�߶�","�߰�߶�"));
	AddPropItem("wiRibPlateHeightInner",PROPLIST_ITEM(id++,"�߰�(��)�߶�","�߰�߶�"));
	AddPropItem("wiRibPlateMargin",PROPLIST_ITEM(id++,"�߰�߾�","�߰��װ���Ե�߾�"));
	AddPropItem("FoundationPara",PROPLIST_ITEM(id++,"��������"));
	AddPropItem("biLayoutType",PROPLIST_ITEM(id++,"�ؽ���˨����",NULL,"4�ؽ���˨|8�ؽ���˨"));
	AddPropItem("wiBoltD",PROPLIST_ITEM(id++,"�ؽ���˨ֱ��M",NULL,GetAnchorBoltCmbSizeStr()));
	AddPropItem("ciAnchorBoltType",PROPLIST_ITEM(id++,"�ؽ���˨����:", NULL, "��ֱͨ��|���乴"));
	AddPropItem("wiBoltSapce",PROPLIST_ITEM(id++,"��˨����S1"));
	AddPropItem("wiFoundationWidth",PROPLIST_ITEM(id++,"����ֱ��"));
	AddPropItem("wiFoundationDepth",PROPLIST_ITEM(id++,"�������"));
}

int FOUNDATION_PARA::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(GetPropID("biBasePlateType")==id)
	{
		if(biBasePlateType==TYPE_BTM)
			sText.Copy("1.������ƽ��");
		else if(biBasePlateType==TYPE_MID)
			sText.Copy("2.�����м�");
		else
			sText.Copy("0.������ƽ��");
	}
	else if(GetPropID("wiBasePlateThick")==id)
		sText.Printf("%d",wiBasePlateThick);
	else if(GetPropID("wiBasePlateWidth")==id)
		sText.Printf("%d",wiBasePlateWidth);
	else if(GetPropID("wiBasePlateHoleD")==id)
		sText.Printf("%d",wiBasePlateHoleD);
	else if(GetPropID("wiBoltD")==id)
		sText.Printf("%d",this->wiBoltD);
	else if(GetPropID("biLayoutType")==id)
	{
		if(biLayoutType==1)
			sText.Copy("8�ؽ���˨");
		else
			sText.Copy("4�ؽ���˨");
	}
	else if(GetPropID("wiBoltSapce")==id)
		sText.Printf("%d",wiBoltSapce);
	else if(GetPropID("wiFoundationWidth")==id)
		sText.Printf("%d",wiFoundationWidth);
	else if (GetPropID("ConnBoltCountOfPrimaryAngle") == id)
		sText.Printf("%d", cnMainAngleBoltNum);
	else if (GetPropID("ConnBoltRowsOfPrimaryAngle") == id)
	{
		if (cnRows == 2)
			sText.Copy("2.˫��");
		else
			sText.Copy("1.����");
	}
	else if (GetPropID("wiShoePlateThick") == id)
		sText.Printf("%d", wiShoeThick);
	else if (GetPropID("wiRibPlateThick") == id)
		sText.Printf("%d", wiRibThick);
	else if (GetPropID("wiRibPlateWidth") == id)
		sText.Printf("%d", wRibWidth);
	else if (GetPropID("wiRibPlateHeightOuter") == id)
		sText.Printf("%d", wRibOuterHeight);
	else if (GetPropID("wiRibPlateHeightInner") == id)
		sText.Printf("%d", wRibInnerHeight);
	else if (GetPropID("wiRibPlateMargin") == id)
		sText.Printf("%d", wiRibMargin);
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
