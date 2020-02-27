#include "stdafx.h"
#include ".\slotarm.h"
#include "CreateFace.h"

#ifdef __GGG_
CLDSSlotArm::CLDSSlotArm(void)
{
	m_designInfo.cover_plate_thick=0;
	m_designInfo.cover_plate_width=0;
	m_designInfo.end_high=0;
	m_designInfo.rail_lever_length=0;
	m_designInfo.rib_plate_length=0;
	m_designInfo.rib_plate_thick=0;
	m_designInfo.root_high=0;
	m_designInfo.root_insert_lenght=0;
	m_designInfo.root_width=0;
	m_designInfo.side_plate_thick=0;
}

void CLDSSlotArm::Init(SLOTARMINFO designInfo)
{
	m_designInfo.cover_plate_width=designInfo.cover_plate_width;
	m_designInfo.cover_plate_thick=designInfo.cover_plate_thick;
	m_designInfo.end_high=designInfo.end_high;
	m_designInfo.side_plate_thick=designInfo.side_plate_thick;
	m_designInfo.rail_lever_length=designInfo.rail_lever_length;
	m_designInfo.rib_plate_length=designInfo.rib_plate_length;
	m_designInfo.rib_plate_thick=designInfo.rib_plate_thick;
	m_designInfo.root_high=designInfo.root_high;
	m_designInfo.root_insert_lenght=designInfo.root_insert_lenght;
	m_designInfo.root_width=designInfo.root_width;
}
double  CLDSSlotArm::GetCoverPlateThick()
{
	return m_designInfo.cover_plate_thick;
}
double CLDSSlotArm::GetCoverPlateWidth()
{
	return m_designInfo.cover_plate_width;
}
double CLDSSlotArm::GetEndHigh()
{
	return m_designInfo.end_high;
}
double CLDSSlotArm::GetRailLeverLen()
{
	return m_designInfo.rail_lever_length;
}
double CLDSSlotArm::GetRibPlateLen()
{
	return m_designInfo.rib_plate_length;
}
double CLDSSlotArm::GetSidePlateThick()
{
	return m_designInfo.side_plate_thick;
}
double CLDSSlotArm::GetRootHigh()
{
	return m_designInfo.root_high;
}
double CLDSSlotArm::GetRootInsertLen()
{
	return m_designInfo.root_insert_lenght;
}
double CLDSSlotArm::GetRootWidth()
{
	return m_designInfo.root_width;
}
double CLDSSlotArm::GetRibPlateThick()
{
	return m_designInfo.rib_plate_thick;
}
void CLDSSlotArm::Create3dSolidModel()
{
	if(pSolidBody == NULL)
		pSolidBody = new CSolidBody;
	else
		pSolidBody->Empty();
	fBody body;
	fBody* pBody=NULL;
	f3dPoint vertex_arr[50];
	double fCoverBoardLen=GetRailLeverLen()-GetRootInsertLen();
	double f=(GetCoverPlateWidth()-GetRootWidth())/2;
	//上盖板点坐标
	vertex_arr[0].Set(GetRootInsertLen(),GetRootHigh()/2,-GetCoverPlateWidth()/2);
	vertex_arr[1].Set(GetRootInsertLen(),GetRootHigh()/2+GetCoverPlateThick(),-GetCoverPlateWidth()/2);
	vertex_arr[2].Set(GetRootInsertLen(),GetRootHigh()/2+GetCoverPlateThick(),GetCoverPlateWidth()/2);
	vertex_arr[3].Set(GetRootInsertLen(),GetRootHigh()/2,GetCoverPlateWidth()/2);
	for(int i=0;i<4;i++)
		vertex_arr[i+4].Set(vertex_arr[i].x+fCoverBoardLen,vertex_arr[i].y,vertex_arr[i].z);
	//下盖板点坐标
	vertex_arr[8].Set(GetRootInsertLen(),-GetRootHigh()/2,-GetCoverPlateWidth()/2);
	vertex_arr[9].Set(GetRootInsertLen(),-GetRootHigh()/2-GetCoverPlateThick(),-GetCoverPlateWidth()/2);
	vertex_arr[10].Set(GetRootInsertLen(),-GetRootHigh()/2-GetCoverPlateThick(),GetCoverPlateWidth()/2);
	vertex_arr[11].Set(GetRootInsertLen(),-GetRootHigh()/2,GetCoverPlateWidth()/2);
	double d=GetEndHigh()-GetRootHigh()/2;
	vertex_arr[12].Set(GetRailLeverLen(),-d,-GetCoverPlateWidth()/2);
	vertex_arr[13].Set(GetRailLeverLen(),-d-GetCoverPlateThick(),-GetCoverPlateWidth()/2);
	vertex_arr[14].Set(GetRailLeverLen(),-d-GetCoverPlateThick(),GetCoverPlateWidth()/2);
	vertex_arr[15].Set(GetRailLeverLen(),-d,GetCoverPlateWidth()/2);
	
	//侧板1点坐标
	vertex_arr[20].Set(0,GetRootHigh()/2,GetRootWidth()/2-GetSidePlateThick());
	vertex_arr[21].Set(0,GetRootHigh()/2,GetRootWidth()/2);
	vertex_arr[22].Set(0,-GetRootHigh()/2,GetRootWidth()/2);
	vertex_arr[23].Set(0,-GetRootHigh()/2,GetRootWidth()/2-GetSidePlateThick());
	for(int i=20;i<24;i++)
		vertex_arr[i-4].Set(vertex_arr[i].x+GetRootInsertLen(),vertex_arr[i].y,vertex_arr[i].z);

	vertex_arr[24].Set(GetRailLeverLen(),GetRootHigh()/2,GetRootWidth()/2-GetSidePlateThick());
	vertex_arr[25].Set(GetRailLeverLen(),GetRootHigh()/2,GetRootWidth()/2);
	vertex_arr[26].Set(GetRailLeverLen(),-d,GetRootWidth()/2);
	vertex_arr[27].Set(GetRailLeverLen(),-d,GetRootWidth()/2-GetSidePlateThick());
	//侧板2点坐标
	for(int i=16;i<28;i++)
		vertex_arr[i+12].Set(vertex_arr[i].x,vertex_arr[i].y,-vertex_arr[i].z);
	//肋板点坐标
	double t=GetRootInsertLen()-GetRibPlateLen();
	vertex_arr[40].Set(t,GetRibPlateThick()/2,-GetRootWidth()/2+GetSidePlateThick());
	vertex_arr[41].Set(t,GetRibPlateThick()/2,GetRootWidth()/2-GetSidePlateThick());
	vertex_arr[42].Set(t,-GetRibPlateThick()/2,GetRootWidth()/2-GetSidePlateThick());
	vertex_arr[43].Set(t,-GetRibPlateThick()/2,-GetRootWidth()/2+GetSidePlateThick());
	for(int i=40;i<44;i++)
		vertex_arr[i+4].Set(vertex_arr[i].x+GetRibPlateLen(),vertex_arr[i].y,vertex_arr[i].z);

	for(i=0;i<48;i++)
	{	
		if(coord_trans(vertex_arr[i],ucs,TRUE)!=TRUE)//进行坐标系转换
			return;
		pBody->vertex.append(vertex_arr[i]);
	}
	
	f3dPolyFace *pFace;
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	//上盖板前
	pFace=pBody->faceList.append();
	pFace->material=RGB(0,125,0);
	createFace.NewOutterEdgeLine(pFace,1,0);
	createFace.NewOutterEdgeLine(pFace,2);
	createFace.NewOutterEdgeLine(pFace,3);
	createFace.NewOutterEdgeLine(pFace,0);
	//上盖板左
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,125,0);
	createFace.NewOutterEdgeLine(pFace,4,0);
	createFace.NewOutterEdgeLine(pFace,5);
	createFace.NewOutterEdgeLine(pFace,1);
	createFace.NewOutterEdgeLine(pFace,0);
	//上盖板后
	pFace=pBody->faceList.append();
	pFace->material=RGB(0,125,0);
	createFace.NewOutterEdgeLine(pFace,7,4);
	createFace.NewOutterEdgeLine(pFace,6);
	createFace.NewOutterEdgeLine(pFace,5);
	createFace.NewOutterEdgeLine(pFace,4);
	//上盖板右
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,125,0);
	createFace.NewOutterEdgeLine(pFace,2,3);
	createFace.NewOutterEdgeLine(pFace,6);
	createFace.NewOutterEdgeLine(pFace,7);
	createFace.NewOutterEdgeLine(pFace,3);
	//上盖板上
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,0,0);
	createFace.NewOutterEdgeLine(pFace,5,1);
	createFace.NewOutterEdgeLine(pFace,6);
	createFace.NewOutterEdgeLine(pFace,2);
	createFace.NewOutterEdgeLine(pFace,1);
	//上盖板下
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,0,0);
	createFace.NewOutterEdgeLine(pFace,3,0);
	createFace.NewOutterEdgeLine(pFace,7);
	createFace.NewOutterEdgeLine(pFace,4);
	createFace.NewOutterEdgeLine(pFace,0);
	//下盖板前
	pFace=pBody->faceList.append();
	pFace->material=RGB(0,125,0);
	createFace.NewOutterEdgeLine(pFace,11,8);
	createFace.NewOutterEdgeLine(pFace,10);
	createFace.NewOutterEdgeLine(pFace,9);
	createFace.NewOutterEdgeLine(pFace,8);
	//下盖板左
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,125,0);
	createFace.NewOutterEdgeLine(pFace,9,8);
	createFace.NewOutterEdgeLine(pFace,13);
	createFace.NewOutterEdgeLine(pFace,12);
	createFace.NewOutterEdgeLine(pFace,8);
	//下盖板后
	pFace=pBody->faceList.append();
	pFace->material=RGB(0,125,0);
	createFace.NewOutterEdgeLine(pFace,13,12);
	createFace.NewOutterEdgeLine(pFace,14);
	createFace.NewOutterEdgeLine(pFace,15);
	createFace.NewOutterEdgeLine(pFace,12);
	//下盖板右
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,125,0);
	createFace.NewOutterEdgeLine(pFace,11,10);
	createFace.NewOutterEdgeLine(pFace,15);
	createFace.NewOutterEdgeLine(pFace,14);
	createFace.NewOutterEdgeLine(pFace,10);
	//下盖板上
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,0,0);
	createFace.NewOutterEdgeLine(pFace,12,8);
	createFace.NewOutterEdgeLine(pFace,15);
	createFace.NewOutterEdgeLine(pFace,11);
	createFace.NewOutterEdgeLine(pFace,8);
	//下盖板下
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,0,0);
	createFace.NewOutterEdgeLine(pFace,10,9);
	createFace.NewOutterEdgeLine(pFace,14);
	createFace.NewOutterEdgeLine(pFace,13);
	createFace.NewOutterEdgeLine(pFace,9);
	//侧板1正
	pFace=pBody->faceList.append();
	pFace->material=RGB(0,125,0);
	createFace.NewOutterEdgeLine(pFace,21,20);
	createFace.NewOutterEdgeLine(pFace,22);
	createFace.NewOutterEdgeLine(pFace,23);
	createFace.NewOutterEdgeLine(pFace,20);
	//侧板1左外
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,125,0);
	createFace.NewOutterEdgeLine(pFace,23,20);
	createFace.NewOutterEdgeLine(pFace,19);
	createFace.NewOutterEdgeLine(pFace,16);
	createFace.NewOutterEdgeLine(pFace,20);
	//侧板1左里
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,125,0);
	createFace.NewOutterEdgeLine(pFace,19,16);
	createFace.NewOutterEdgeLine(pFace,27);
	createFace.NewOutterEdgeLine(pFace,24);
	createFace.NewOutterEdgeLine(pFace,16);
	//侧板1后面
	pFace=pBody->faceList.append();
	pFace->material=RGB(0,125,0);
	createFace.NewOutterEdgeLine(pFace,27,24);
	createFace.NewOutterEdgeLine(pFace,26);
	createFace.NewOutterEdgeLine(pFace,25);
	createFace.NewOutterEdgeLine(pFace,24);
	//侧板1上面
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,0,0);
	createFace.NewOutterEdgeLine(pFace,24,20);
	createFace.NewOutterEdgeLine(pFace,25);
	createFace.NewOutterEdgeLine(pFace,21);
	createFace.NewOutterEdgeLine(pFace,20);
	//侧板1下面外
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,0,0);
	createFace.NewOutterEdgeLine(pFace,18,22);
	createFace.NewOutterEdgeLine(pFace,19);
	createFace.NewOutterEdgeLine(pFace,23);
	createFace.NewOutterEdgeLine(pFace,22);
	//侧面1下面里
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,0,0);
	createFace.NewOutterEdgeLine(pFace,26,18);
	createFace.NewOutterEdgeLine(pFace,27);
	createFace.NewOutterEdgeLine(pFace,19);
	createFace.NewOutterEdgeLine(pFace,18);
	//侧面1右外
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,125,0);
	createFace.NewOutterEdgeLine(pFace,17,21);
	createFace.NewOutterEdgeLine(pFace,18);
	createFace.NewOutterEdgeLine(pFace,22);
	createFace.NewOutterEdgeLine(pFace,21);
	//侧面1右里
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,125,0);
	createFace.NewOutterEdgeLine(pFace,25,17);
	createFace.NewOutterEdgeLine(pFace,26);
	createFace.NewOutterEdgeLine(pFace,18);
	createFace.NewOutterEdgeLine(pFace,17);
	//侧板2正
	pFace=pBody->faceList.append();
	pFace->material=RGB(0,125,0);
	createFace.NewOutterEdgeLine(pFace,35,32);
	createFace.NewOutterEdgeLine(pFace,34);
	createFace.NewOutterEdgeLine(pFace,33);
	createFace.NewOutterEdgeLine(pFace,32);
	//侧板2左外
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,125,0);
	createFace.NewOutterEdgeLine(pFace,28,32);
	createFace.NewOutterEdgeLine(pFace,31);
	createFace.NewOutterEdgeLine(pFace,35);
	createFace.NewOutterEdgeLine(pFace,32);
	//侧板2左里
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,125,0);
	createFace.NewOutterEdgeLine(pFace,36,28);
	createFace.NewOutterEdgeLine(pFace,39);
	createFace.NewOutterEdgeLine(pFace,31);
	createFace.NewOutterEdgeLine(pFace,28);
	//侧板2后面
	pFace=pBody->faceList.append();
	pFace->material=RGB(0,125,0);
	createFace.NewOutterEdgeLine(pFace,37,36);
	createFace.NewOutterEdgeLine(pFace,38);
	createFace.NewOutterEdgeLine(pFace,39);
	createFace.NewOutterEdgeLine(pFace,36);
	//侧板2上面
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,0,0);
	createFace.NewOutterEdgeLine(pFace,33,32);
	createFace.NewOutterEdgeLine(pFace,37);
	createFace.NewOutterEdgeLine(pFace,36);
	createFace.NewOutterEdgeLine(pFace,32);
	//侧板2下面外
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,0,0);
	createFace.NewOutterEdgeLine(pFace,35,34);
	createFace.NewOutterEdgeLine(pFace,31);
	createFace.NewOutterEdgeLine(pFace,30);
	createFace.NewOutterEdgeLine(pFace,34);
	//侧面2下面里
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,0,0);
	createFace.NewOutterEdgeLine(pFace,31,30);
	createFace.NewOutterEdgeLine(pFace,39);
	createFace.NewOutterEdgeLine(pFace,38);
	createFace.NewOutterEdgeLine(pFace,30);
	//侧面2右外
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,125,0);
	createFace.NewOutterEdgeLine(pFace,34,33);
	createFace.NewOutterEdgeLine(pFace,30);
	createFace.NewOutterEdgeLine(pFace,29);
	createFace.NewOutterEdgeLine(pFace,33);
	//侧面2右里
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,125,0);
	createFace.NewOutterEdgeLine(pFace,30,29);
	createFace.NewOutterEdgeLine(pFace,38);
	createFace.NewOutterEdgeLine(pFace,37);
	createFace.NewOutterEdgeLine(pFace,29);
	//肋板前面
	pFace=pBody->faceList.append();
	pFace->material=RGB(0,125,0);
	createFace.NewOutterEdgeLine(pFace,41,40);
	createFace.NewOutterEdgeLine(pFace,42);
	createFace.NewOutterEdgeLine(pFace,43);
	createFace.NewOutterEdgeLine(pFace,40);
	//肋板左面
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,125,0);
	createFace.NewOutterEdgeLine(pFace,43,40);
	createFace.NewOutterEdgeLine(pFace,47);
	createFace.NewOutterEdgeLine(pFace,44);
	createFace.NewOutterEdgeLine(pFace,40);
	//肋板后面
	pFace=pBody->faceList.append();
	pFace->material=RGB(0,125,0);
	createFace.NewOutterEdgeLine(pFace,47,44);
	createFace.NewOutterEdgeLine(pFace,46);
	createFace.NewOutterEdgeLine(pFace,45);
	createFace.NewOutterEdgeLine(pFace,44);
	//肋板右面
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,125,0);
	createFace.NewOutterEdgeLine(pFace,45,41);
	createFace.NewOutterEdgeLine(pFace,46);
	createFace.NewOutterEdgeLine(pFace,42);
	createFace.NewOutterEdgeLine(pFace,41);
	//肋板上面
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,0,125);
	createFace.NewOutterEdgeLine(pFace,44,40);
	createFace.NewOutterEdgeLine(pFace,45);
	createFace.NewOutterEdgeLine(pFace,41);
	createFace.NewOutterEdgeLine(pFace,40);
	//肋板下面
	pFace=pBody->faceList.append();
	pFace->material=RGB(125,0,125);
	createFace.NewOutterEdgeLine(pFace,46,42);
	createFace.NewOutterEdgeLine(pFace,47);
	createFace.NewOutterEdgeLine(pFace,43);
	createFace.NewOutterEdgeLine(pFace,42);
	pSolidBody->ConvertFrom(pBody);
}
CSolidPart CLDSSlotArm::GetSolidPartObject()
{
	CSolidPart solidPart;
	solidPart.m_hPart = handle;
	solidPart.m_idClassType = 50;
	solidPart.acs = ucs;
	solidPart.m_iLineType = 0;
	solidPart.pBody = pSolidBody;
	return solidPart;
}
CLDSSlotArm::~CLDSSlotArm(void)
{
}

const DWORD HASHTABLESIZE = 500;
const DWORD STATUSHASHTABLESIZE = 50;
CStringKeyHashTable<long> CLDSSlotArm::propHashtable;
CHashTable<DWORD> CLDSSlotArm::propStatusHashtable;
void CLDSSlotArm::InitSlotArmPropHashtable()
{
	int id=1;
	CLDSSlotArm::propHashtable.CreateHashTable(HASHTABLESIZE);
	CLDSSlotArm::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	CLDSSlotArm::propHashtable.SetValueAt("basicInfo",id++);
	CLDSSlotArm::propHashtable.SetValueAt("handle",id++);
	CLDSSlotArm::propHashtable.SetValueAt("layer",id++);
	CLDSSlotArm::propHashtable.SetValueAt("cMaterial",id++);
}
int CLDSSlotArm::GetPropValueStr(long id,char* valueStr)
{
	char sText[100]="";
	if(GetPropID("basicInfo")==0)
		strcpy(valueStr,"");
	else if(GetPropID("handle")==id)
		sprintf(sText,"0X%X",handle);
	else if(GetPropID("layer")==id)
		strcpy(sText,layer());
	else if(GetPropID("cMaterial")==id)	
	{
		char matStr[20];
		if(QuerySteelMatMark(cMaterial,matStr))
			sprintf(sText,"%s",matStr);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
int CLDSSlotArm::GetPropStr(long id,char* propStr)
{
	char sText[100]="";
	if(GetPropID("basicInfo")==0)
		strcpy(sText,"basicInfo");
	else if(GetPropID("handle")==id)
		strcpy(sText,"handle");
	else if(GetPropID("layer")==id)
		strcpy(sText,"layer");
	else if(GetPropID("cMaterial")==id)
		strcpy(sText,"cMaterial");
	if(propStr)
		strcpy(propStr,sText);
	return strlen(sText);
}
long CLDSSlotArm::GetPropID(char* propStr)
{
	long id;
	if(CLDSSlotArm::propHashtable.GetValueAt(propStr,id))
		return id;
	else
		return 0;
	
}
int CLDSSlotArm::GetPropStatus(long id)
{
	DWORD dwStatus;
	if(CLDSSlotArm::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}
BOOL CLDSSlotArm::SetPropStatus(long id,BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSSlotArm::propStatusHashtable.SetValueAt(id,dwStatus);
}
BOOL CLDSSlotArm::IsPropEqual(HASHOBJSET &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSObject *pFirstObj = selectObjs.GetFirst();
	CLDSSlotArm *pSlotArm=NULL, *pFirstSlotArm = (CLDSSlotArm*)pFirstObj;
	pFirstSlotArm->GetPropValueStr(idProp,valueStr);
	for(pSlotArm=(CLDSSlotArm*)selectObjs.GetNext();pSlotArm;pSlotArm=(CLDSSlotArm*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pSlotArm->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}
#endif