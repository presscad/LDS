// Bolt.cpp: implementation of the C3dLs class.
//wbt translation
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tower.h"
#include "AnchorBolt.h"
#include "SolidBody.h"
#include "CreateFace.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AnchorBolt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLDSFoundation
CLDSFoundation::CLDSFoundation(IModel *pModel/*=NULL*/)
{
	m_nClassTypeId=CLS_FOUNDATION;
	//strcpy(m_sClassName,"CLDSFoundation");
	wiWidth=1000;		//�������������ʾ��ȣ�����������ɫ��ʾ����
	wiHeight=3000;
	crColor=RGB(90,90,90);//240,228,176);//195,195,195);	//ˮ���ɫRGB(240,228,176);//
	pSolidBody=NULL;
	m_pModel=pModel;
	ciAnchorBoltD=36;
	ciHeightSerial=1;
	ciLegSerial=1;
	m_ciLegQuad=0;
	vWorkNormal.Set(0,0,-1);//������ƽ�淨�ߣ����泯�ϣ�
	vProjAxisX.Set(1,0,0);	//�ֲ�(����ƽ��)����ϵ��X��ˮƽͶӰ����

	this->is_data_modified=this->is_solid_modified=true;
}
CLDSFoundation::~CLDSFoundation()
{
	if(pSolidBody)
		delete pSolidBody;
}
IMPLEMENT_PROP_FUNC(CLDSFoundation);
void CLDSFoundation::InitPropHashtable()
{
	int id = 1;
	CLDSFoundation::propHashtable.SetHashTableGrowSize(500);
	CLDSFoundation::propStatusHashtable.CreateHashTable(50);
#ifdef AFX_TARG_ENU_ENGLISH
	AddPropItem("BasicInfo", PROPLIST_ITEM(id++, "BasicInfo"));
	AddPropItem("handle", PROPLIST_ITEM(id++, "Handle"));
	AddPropItem("HeightSerial", PROPLIST_ITEM(id++, "HeightSerial"));
	AddPropItem("LegSerial", PROPLIST_ITEM(id++, "LegSerial"));
	AddPropItem("LegQuad", PROPLIST_ITEM(id++, "LegQuad"));
	AddPropItem("Width", PROPLIST_ITEM(id++, "Width"));
	AddPropItem("Height", PROPLIST_ITEM(id++, "Height"));
	AddPropItem("AnchorBoltD", PROPLIST_ITEM(id++, "AnchorBoltD"));
	AddPropItem("location", PROPLIST_ITEM(id++, "location"));
	AddPropItem("location.x", PROPLIST_ITEM(id++, "X"));
	AddPropItem("location.y", PROPLIST_ITEM(id++, "Y"));
	AddPropItem("location.z", PROPLIST_ITEM(id++, "Z"));
	AddPropItem("WorkNormal", PROPLIST_ITEM(id++, "WorkNormal"));
	AddPropItem("WorkNormal.x", PROPLIST_ITEM(id++, "X"));
	AddPropItem("WorkNormal.y", PROPLIST_ITEM(id++, "Y"));
	AddPropItem("WorkNormal.z", PROPLIST_ITEM(id++, "Z"));
	AddPropItem("ProjAxisX", PROPLIST_ITEM(id++, "ProjAxisX"));
	AddPropItem("ProjAxisX.x", PROPLIST_ITEM(id++, "X"));
	AddPropItem("ProjAxisX.y", PROPLIST_ITEM(id++, "Y"));
	AddPropItem("ProjAxisX.z", PROPLIST_ITEM(id++, "Z"));
#else
	AddPropItem("BasicInfo", PROPLIST_ITEM(id++, "������Ϣ"));
	AddPropItem("handle", PROPLIST_ITEM(id++, "���"));
	AddPropItem("HeightSerial", PROPLIST_ITEM(id++, "��������"));
	AddPropItem("LegSerial", PROPLIST_ITEM(id++, "��������"));
	AddPropItem("LegQuad", PROPLIST_ITEM(id++, "��������", "", "1|2|3|4"));
	AddPropItem("Width", PROPLIST_ITEM(id++, "�������"));
	AddPropItem("Height", PROPLIST_ITEM(id++, "�������"));
	AddPropItem("AnchorBoltD", PROPLIST_ITEM(id++, "��˨ֱ��"));
	AddPropItem("location", PROPLIST_ITEM(id++, "����ԭ��"));
	AddPropItem("location.x", PROPLIST_ITEM(id++, "X"));
	AddPropItem("location.y", PROPLIST_ITEM(id++, "Y"));
	AddPropItem("location.z", PROPLIST_ITEM(id++, "Z"));
	AddPropItem("WorkNormal", PROPLIST_ITEM(id++, "��������"));
	AddPropItem("WorkNormal.x", PROPLIST_ITEM(id++, "X"));
	AddPropItem("WorkNormal.y", PROPLIST_ITEM(id++, "Y"));
	AddPropItem("WorkNormal.z", PROPLIST_ITEM(id++, "Z"));
	AddPropItem("ProjAxisX", PROPLIST_ITEM(id++, "ˮƽͶӰ"));
	AddPropItem("ProjAxisX.x", PROPLIST_ITEM(id++, "X"));
	AddPropItem("ProjAxisX.y", PROPLIST_ITEM(id++, "Y"));
	AddPropItem("ProjAxisX.z", PROPLIST_ITEM(id++, "Z"));
#endif

}
int CLDSFoundation::GetPropValueStr(long id, char *valueStr, UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if (id == GetPropID("handle"))
		sText.Printf("0X%X", handle);
	else if (id == GetPropID("HeightSerial"))
		sText.Printf("%d", ciHeightSerial);
	else if (id == GetPropID("LegSerial"))
		sText.Printf("%d", ciLegSerial);
	else if (id == GetPropID("LegQuad"))
		sText.Printf("%d", ciLegQuad);
	else if (id == GetPropID("Width"))
		sText.Printf("%d", wiWidth);
	else if (id == GetPropID("Height"))
		sText.Printf("%d", wiHeight);
	else if (id == GetPropID("AnchorBoltD"))
		sText.Printf("%d", ciAnchorBoltD);
	else if (id == GetPropID("location.x"))
	{
		sText.Printf("%f", location.x);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("location.y"))
	{
		sText.Printf("%f", location.y);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("location.z"))
	{
		sText.Printf("%f", location.z);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("WorkNormal.x"))
	{
		sText.Printf("%f", vWorkNormal.x);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("WorkNormal.y"))
	{
		sText.Printf("%f", vWorkNormal.y);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("WorkNormal.z"))
	{
		sText.Printf("%f", vWorkNormal.z);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("ProjAxisX.x"))
	{
		sText.Printf("%f", vProjAxisX.x);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("ProjAxisX.y"))
	{
		sText.Printf("%f", vProjAxisX.y);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("ProjAxisX.z"))
	{
		sText.Printf("%f", vProjAxisX.z);
		SimplifiedNumString(sText);
	}
	//
	if (valueStr)
		StrCopy(valueStr, sText, nMaxStrBufLen - 1);
	return strlen(sText);
}
CSolidPart CLDSFoundation::GetSolidPartObject()
{
	CSolidPart solidPart;
	solidPart.m_hPart = handle;
	solidPart.m_idClassType = CLS_FOUNDATION;
	//solidPart.ucs = foundation.a;
	solidPart.m_iLineType = 0;
	solidPart.pBody = pSolidBody;
	return solidPart;
}
BOOL CLDSFoundation::Create3dSolidModel()
{
	fBody body;
	if(pSolidBody!=NULL && !IsSolidModified())
		return TRUE;
	else
	{
		if(pSolidBody == NULL)
			pSolidBody = new CSolidBody;
		else
			pSolidBody->Empty();
	}
	SetModified(FALSE);
	double halfw=wiWidth*0.5;
	typedef f3dPoint* f3dPointPtr;
	static const int SLICES=72;
	f3dPoint btm_vertex_arr[SLICES];
	CLDSPart::GetCircleSimuPolyVertex(halfw,btm_vertex_arr,SLICES);
	f3dPointPtr vertexptrs[SLICES+SLICES];
	int i;
	for(i=0;i<SLICES;i++)
		vertexptrs [i] =body.vertex.append(btm_vertex_arr[i].x+location.x,btm_vertex_arr[i].y+location.y,location.z);
	for(i=0;i<SLICES;i++)
		vertexptrs[i+SLICES]=body.vertex.append(btm_vertex_arr[i].x+location.x,btm_vertex_arr[i].y+location.y,location.z+wiHeight);

	CCreateFace createFace;
	createFace.InitVertexList(&body);
	//����
	f3dPolyFace *pFace=body.faceList.append();
	pFace->material = crColor;//RGB(195,195,195);	//ˮ���ɫRGB(240,228,176);//
	//pFace->poly_norm.Set(0,0,-1);
	createFace.NewOutterEdgeLine(pFace,SLICES-1,0);
	for(i=SLICES-2;i>=0;i--)
		createFace.NewOutterEdgeLine(pFace,i);
	//����
	pFace=body.faceList.append();
	pFace->material = crColor;//RGB(195,195,195);	//ˮ���ɫRGB(240,228,176);//
	//pFace->poly_norm.Set(0,0, 1);
	createFace.NewOutterEdgeLine(pFace,SLICES+1,SLICES);
	for(i=SLICES+2;i<SLICES+SLICES;i++)
		createFace.NewOutterEdgeLine(pFace,i);
	createFace.NewOutterEdgeLine(pFace,SLICES);
	//����
	for(i=0;i<SLICES;i++)
	{
		pFace=body.faceList.append();
		pFace->material = crColor;//RGB(195,195,195);	//ˮ���ɫRGB(240,228,176);//
		createFace.NewOutterEdgeLine(pFace,(i+1)%SLICES,i);
		createFace.NewOutterEdgeLine(pFace,(i+1)%SLICES+SLICES);
		createFace.NewOutterEdgeLine(pFace, i+SLICES);
		createFace.NewOutterEdgeLine(pFace, i);
	}
	//����ϵת����ģ������ϵ��
	pSolidBody->ConvertFrom(&body);
	return TRUE;
}
void CLDSFoundation::SetModified(BOOL bTrue/*=TRUE*/,BOOL bRenderSolid/*=TRUE*/)
{
	is_data_modified = bTrue;
	if(bTrue)
	{
		is_undo_modified = TRUE;
		is_solid_modified|=bRenderSolid;
	}
	else
		is_solid_modified=bRenderSolid&&is_data_modified;
	if(m_pModel)
		SetDocModifiedFlag();
}
char CLDSFoundation::get_ciLegQuad()
{
	if(m_ciLegQuad>0&&m_ciLegQuad<5)
		return m_ciLegQuad;
	char ciQuad=layer_tag[2]-'0';
	if(ciQuad>0&&ciQuad<5)
		return ciQuad;
	if(location.x>EPS&&location.y>EPS)
		return 1;
	else if(location.x<EPS&&location.y>EPS)
		return 2;
	else if(location.x>EPS&&location.y<EPS)
		return 3;
	else if(location.x<EPS&&location.y<EPS)
		return 4;
	else
		return 0;
}
void CLDSFoundation::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	buffer.ReadByte(&ciHeightSerial);//������Ӧ�ĺ��ߺ�
	buffer.ReadByte(&ciLegSerial);	//������Ӧ�Ľ������
	buffer.ReadByte(&m_ciLegQuad);	//������Ӧ�������
	buffer.ReadByte(&ciAnchorBoltD);	//������˨ֱ��
	buffer.ReadWord(&wiWidth);		//�������������ʾ��ȣ�����������ɫ��ʾ����
	buffer.ReadPoint(location);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
	{
		buffer.ReadPoint(vWorkNormal);
		buffer.ReadPoint(vProjAxisX);
	}
	else
	{
		vWorkNormal.Set(0,0,-1);
		vProjAxisX.Set(1,0,0);
	}
	BYTE cnCount=0;
	buffer.ReadByte(&cnCount);
	anchors.ReSize(0);
	for(BYTE i=0;i<cnCount;i++)
	{
		LOCATION anchor;
		buffer.ReadInteger(&anchor.hAnchorBolt);	//�����ĵؽ���˨���
		buffer.ReadWord(&anchor.siPosX);
		buffer.ReadWord(&anchor.siPosY);
		if(anchor.hAnchorBolt>0x20)
			anchors.Append(anchor);
	}
}
void CLDSFoundation::ToBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	buffer.WriteByte(ciHeightSerial);//������Ӧ�ĺ��ߺ�
	buffer.WriteByte(ciLegSerial);	//������Ӧ�Ľ������
	buffer.WriteByte(ciLegQuad);	//������Ӧ�������
	buffer.WriteByte(ciAnchorBoltD);	//������˨ֱ��
	buffer.WriteWord(wiWidth);		//�������������ʾ��ȣ�����������ɫ��ʾ����
	buffer.WritePoint(location);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
	{
		buffer.WritePoint(vWorkNormal);
		buffer.WritePoint(vProjAxisX);
	}
	buffer.WriteByte((BYTE)anchors.Count);
	for(WORD i=0;i<anchors.Count;i++)
	{
		buffer.WriteInteger(anchors.GetAt(i)->hAnchorBolt);	//�����ĵؽ���˨���
		buffer.WriteWord(anchors.GetAt(i)->siPosX);
		buffer.WriteWord(anchors.GetAt(i)->siPosY);
	}
}
void CLDSFoundation::LayoutAnchorBolts(int schema/*=0*/,short S/*=200*/)	//schema=0��ʾ����˨����;schema=1��ʾ����˨����
{
	LOCATION* pLocation=NULL;
	if(schema==0)
	{	//Ŀǰ����������˨����
		anchors.ReSize(4);
		anchors[0].SetLocation( S, S);
		anchors[1].SetLocation(-S, S);
		anchors[2].SetLocation(-S,-S);
		anchors[3].SetLocation( S,-S);
	}
	else if(schema==1)
	{
		short S1=S;
		short S2=S+S/10;
		anchors.ReSize(8);
		anchors[0].SetLocation( S1+S2, S1);
		anchors[1].SetLocation( S1   , S1+S2);
		anchors[2].SetLocation(-S1   , S1+S2);
		anchors[3].SetLocation(-S1-S2, S1);
		anchors[4].SetLocation(-S1-S2,-S1);
		anchors[5].SetLocation(-S1   ,-S1-S2);
		anchors[6].SetLocation( S1   ,-S1-S2);
		anchors[7].SetLocation( S1+S2,-S1);
	}
}
GEPOINT CLDSFoundation::GetAnchorLocalPosition(CLDSBolt* pAnchorBolt)
{
	GEPOINT xLocation;
	if(vWorkNormal.IsEqual(0,0,-1)&&vProjAxisX.IsEqual(1,0,0))
	{
		xLocation = pAnchorBolt->ucs.origin-location;
		xLocation.z *= -1;
		xLocation.y *= -1;
	}
	else
	{
		GECS acs(location, vProjAxisX, GEPOINT(0, -1, 0), vWorkNormal);
		acs.axis_y = acs.axis_z^acs.axis_x;
		normalize(acs.axis_y);
		acs.axis_x = acs.axis_y^acs.axis_z;
		normalize(acs.axis_x);
		xLocation = acs.TransPToCS(pAnchorBolt->ucs.origin);
	}
	return xLocation;
}
GEPOINT CLDSFoundation::TransToModelCS(const double* xAnchorBoltLocalLocation)
{
	GEPOINT xModelLoca(xAnchorBoltLocalLocation);
	xModelLoca += location;
	return xModelLoca;
}
GECS CLDSFoundation::GetAcs()
{
	GECS acs(location,vProjAxisX,GEPOINT(0,-1,0),vWorkNormal);
	if (vWorkNormal.IsEqual(0,0,-1)&&vProjAxisX.IsEqual(1,0,0))
		return acs;
	else if (vWorkNormal.IsZero()||vProjAxisX.IsZero())
		return acs;
	else //if (!vWorkNormal.IsEqual(0,0,-1)||!vProjAxisX.IsEqual(1,0,0))
	{
		acs.axis_y = acs.axis_z^acs.axis_x;
		normalize(acs.axis_y);
		acs.axis_x = acs.axis_y^acs.axis_z;
		normalize(acs.axis_x);
		return acs;
	}
}

UINT CLDSFoundation::AddOrUpdateAnchorBolts(WORD wiBasePlateThick/*=0*/,bool blMatchDetachedAnchor/*=true*/,
				bool blAppendLostAnchorBolts/*=true*/)
{
	UINT i;
	CLDSAnchorBolt* pAnchorBolt=NULL;
	chLayer1st = 'L';
	cfgword.Clear();
	cfgword.SetBodyLegs(this->ciHeightSerial, GetSingleWord(this->ciLegSerial));
	GECS acs=GetAcs();
	for(i=0;i<anchors.Count;i++)
	{
		LOCATION* pLocation=anchors.GetAt(i);
		pAnchorBolt = pLocation->hAnchorBolt > 0 ? ((CTower*)BelongModel())->FromAnchorBolt(pLocation->hAnchorBolt) : NULL;
		if (pAnchorBolt==NULL&&blMatchDetachedAnchor)
		{	//�Ѳ鿴�Ƿ�������״̬�ĵؽ���˨��δ�����������һ��ĵؽ���˨)
			GEPOINT vRayStdVec(pLocation->siPosX,pLocation->siPosY);
			normalize(vRayStdVec);
			CLdsPartListStack stackparts(m_pModel);
			for (CLDSPart* pPart = m_pModel->EnumPartFirst(); pPart != NULL; pPart = m_pModel->EnumPartNext())
			{
				if (pPart->GetClassTypeId() != CLS_ANCHORBOLT || !this->ModuleTogetherWith(pPart))
					continue;
				CLDSAnchorBolt* pExistAnchor = (CLDSAnchorBolt*)pPart;
				GEPOINT xLocation = GetAnchorLocalPosition(pExistAnchor);
				GEPOINT vDelta=GEPOINT(xLocation.x-pLocation->siPosX,xLocation.y-pLocation->siPosY);
				normalize(xLocation);
				if (vDelta.mod2()<10000&&vRayStdVec*xLocation>0.90)	//cos(30��)=0.866,30=360/12
				{	//����ƫ��ܳ�100������������ʶ��Ϊ��Ӧ�ĵؽ���˨
					pAnchorBolt = pExistAnchor;
					pLocation->hAnchorBolt = pAnchorBolt->handle;
					break;
				}
			}
		}
		else if (pAnchorBolt!=NULL)
		{
			pAnchorBolt->cfgword=this->cfgword;	//��ֹ������֫��˨��ʼ��ĺ����ݴ���
			//pAnchorBolt->SetLayer(this->szLayer());//ȷ������ͼ����ȷʱӦ���µؽ���˨ͼ��
		}
		if(pAnchorBolt==NULL&&blAppendLostAnchorBolts)
		{
			pAnchorBolt=((CTower*)BelongModel())->AppendAnchorBolt();
			pLocation->hAnchorBolt=pAnchorBolt->handle;
		}
		else if(pAnchorBolt==NULL&&!blAppendLostAnchorBolts)
			continue;
		pAnchorBolt->ucs.origin=location+pLocation->siPosX*acs.axis_x+pLocation->siPosY*acs.axis_y;
		pAnchorBolt->ucs.axis_x=acs.axis_x;
		pAnchorBolt->ucs.axis_y=acs.axis_y;
		pAnchorBolt->ucs.axis_z=acs.axis_z;
		pAnchorBolt->SetLayer(CXhChar16("LB%d",ciLegQuad));
		pAnchorBolt->ciLegQuad=ciLegQuad;
		pAnchorBolt->cfgword=CFGWORD(ciHeightSerial,ciLegSerial);
		const ANCHOR_BOLT* pAnchorBoltInfo=ANCHOR_BOLT::FromBoltD(ciAnchorBoltD);
		if(pAnchorBoltInfo)
			pAnchorBolt->SetAnchorInfo(pAnchorBoltInfo);
		else
		{
			pAnchorBolt->d=ciAnchorBoltD;
			pAnchorBolt->InitDefaultAnchorInfo();
		}
		pAnchorBolt->baseplate.wiBasePlateThick=wiBasePlateThick;
		pAnchorBolt->m_fLr=wiHeight>200?(float)wiHeight-200:200;
		pAnchorBolt->Foundation.ciHeightSerial = this->ciHeightSerial;
		pAnchorBolt->Foundation.ciLegSerial = this->ciLegSerial;
		pAnchorBolt->Foundation.ciLegQuad = this->ciLegQuad;
	}
	return anchors.Count;
}
/////////////////////////////////////////////////////////////////////////////
// CLDSAnchorBolt
CLDSAnchorBolt::FOUNDATION::FOUNDATION(){
	ciHeightSerial=ciLegSerial=ciLegQuad=ciBoltSerial=1;
}
CLDSAnchorBolt::CLDSAnchorBolt(IModel *pModel/*=NULL*/){
	m_nClassTypeId=CLS_ANCHORBOLT;
	//strcpy(m_sClassName,"CLDSAnchorBolt");
	m_pModel=pModel;
	m_hRelaVirtualBolt=0;
	m_ciAnchorType=0;
	washer.wiWidth=150;
	washer.wiThick=20;
	washer.hWasherPlate=50;
	washer.fHoleIncrement=2;
	baseplate.hBasePlate=0;
	baseplate.wiBasePlateThick=40;
	m_fNutH=100;
	L=m_fLe=400;
	m_fNutDe=100;
	m_fNutDs=80;
	d=30;
	m_fLr=200;
	m_fNutTaper=0.35f;
}
CLDSAnchorBolt::CLDSAnchorBolt(const ANCHOR_BOLT& anchor)
{
	SetAnchorInfo(&anchor);
}
CLDSAnchorBolt::~CLDSAnchorBolt()
{
	if(pSolidBody)
		delete pSolidBody;
	pSolidBody=NULL;
}
IMPLEMENT_PROP_FUNC(CLDSAnchorBolt);
void CLDSAnchorBolt::InitPropHashtable()
{
	int id=1;
	CLDSAnchorBolt::propHashtable.SetHashTableGrowSize(500);
	CLDSAnchorBolt::propStatusHashtable.CreateHashTable(50);
#ifdef AFX_TARG_ENU_ENGLISH
	AddPropItem("BasicInfo",PROPLIST_ITEM(id++,"BasicInfo"));
	AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	AddPropItem("Layer",PROPLIST_ITEM(id++,"Layer"));
	AddPropItem("cfgword",PROPLIST_ITEM(id++,"cfgword"));
	AddPropItem("iSeg",PROPLIST_ITEM(id++,"Seg"));
	AddPropItem("d",PROPLIST_ITEM(id++,"Bolt_D"));
	AddPropItem("Le",PROPLIST_ITEM(id++,"Bolt_Le"));
	AddPropItem("Lr",PROPLIST_ITEM(id++,"Bolt_Lr"));
	AddPropItem("NutH",PROPLIST_ITEM(id++,"NutH"));
	AddPropItem("NutDe",PROPLIST_ITEM(id++,"NutDe"));
	AddPropItem("NutDs",PROPLIST_ITEM(id++,"NutDs"));
	AddPropItem("washer",PROPLIST_ITEM(id++,"washer"));
	AddPropItem("washer.Width",PROPLIST_ITEM(id++,"washerW"));
	AddPropItem("washer.Thick",PROPLIST_ITEM(id++,"washerT"));
	AddPropItem("washer.HoleIncrement",PROPLIST_ITEM(id++,"HoleIncrement"));
	AddPropItem("Location",PROPLIST_ITEM(id++,"Location"));
	AddPropItem("ucs",PROPLIST_ITEM(id++,"UCS"));
	AddPropItem("ucs.orgion",PROPLIST_ITEM(id++,"orgion"));
	AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"axis_x"));
	AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"axis_y"));
	AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"axis_z"));
	AddPropItem("basePlate",PROPLIST_ITEM(id++,"basePlate"));
	AddPropItem("basePlate.Thick",PROPLIST_ITEM(id++,"basePlateT"));
	AddPropItem("Foundation",PROPLIST_ITEM(id++,"Foundation"));
	AddPropItem("Foundation.HeightSerial",PROPLIST_ITEM(id++,"HeightSerial"));
	AddPropItem("Foundation.LegSerial",PROPLIST_ITEM(id++,"LegSerial"));
	AddPropItem("Foundation.LegQuad",PROPLIST_ITEM(id++,"Quad"));
	AddPropItem("Foundation.BoltSerial",PROPLIST_ITEM(id++,"BoltSerial"));
#else
	AddPropItem("BasicInfo",PROPLIST_ITEM(id++,"������Ϣ"));
	AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	AddPropItem("Layer",PROPLIST_ITEM(id++,"ͼ����"));
	AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	AddPropItem("d",PROPLIST_ITEM(id++,"�ؽ���˨ֱ��M"));
	AddPropItem("Le",PROPLIST_ITEM(id++,"��¶��"));
	AddPropItem("Lr",PROPLIST_ITEM(id++,"����"));
	AddPropItem("NutH",PROPLIST_ITEM(id++,"��ĸ�߶�"));
	AddPropItem("NutDe",PROPLIST_ITEM(id++,"��ĸ�����߾���"));
	AddPropItem("NutDs",PROPLIST_ITEM(id++,"��ĸ���������"));
	AddPropItem("washer",PROPLIST_ITEM(id++,"���"));
	AddPropItem("washer.Width",PROPLIST_ITEM(id++,"�����"));
	AddPropItem("washer.Thick",PROPLIST_ITEM(id++,"�����"));
	AddPropItem("washer.HoleIncrement",PROPLIST_ITEM(id++,"�׾�����ֵ"));
	AddPropItem("Location",PROPLIST_ITEM(id++,"��λ��Ϣ"));
	AddPropItem("ucs.orgion",PROPLIST_ITEM(id++,"ԭ��"));
	AddPropItem("ucs.orgion.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("ucs.orgion.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("ucs.orgion.z",PROPLIST_ITEM(id++,"Z"));
	AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X��"));
	AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z"));
	AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y��"));
	AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z"));
	AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z��"));
	AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z"));
	AddPropItem("basePlate",PROPLIST_ITEM(id++,"��������"));
	AddPropItem("basePlate.Thick",PROPLIST_ITEM(id++,"������"));
	AddPropItem("Foundation",PROPLIST_ITEM(id++,"��������"));
	AddPropItem("Foundation.HeightSerial",PROPLIST_ITEM(id++,"��Ӧ���ߺ�"));
	AddPropItem("Foundation.LegSerial",PROPLIST_ITEM(id++,"��Ӧ���Ⱥ�"));
	AddPropItem("Foundation.LegQuad",PROPLIST_ITEM(id++,"��������"));
	AddPropItem("Foundation.BoltSerial",PROPLIST_ITEM(id++,"��˨���"));
#endif
}
BOOL CLDSAnchorBolt::IsPropEqual(HASHOBJSET &selectObjs,long idProp)
{
	//�������ֵ�Ƿ����
	return FALSE;
}
int CLDSAnchorBolt::GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(id==GetPropID("handle"))
		sText.Printf("0X%X",handle);
	else if(id==GetPropID("Layer"))
		sText.Copy(layer_tag);
	else if(id==GetPropID("cfgword"))
	{
		if(layer(0)=='L')	//���ȹ���
			cfgword.GetLegScopeStr(sText,50,true);
		else
			cfgword.GetBodyScopeStr(sText);
	}
	else if(id==GetPropID("iSeg"))
		sText.Copy(iSeg.ToString());
	else if(id==GetPropID("d"))
		sText.Printf("%d",get_d());
	else if(id==GetPropID("Le"))
	{
		sText.Printf("%f",m_fLe);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("Lr"))
	{
		sText.Printf("%f",m_fLr);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("NutH"))
	{
		sText.Printf("%f",m_fNutH);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("NutDe"))
	{
		sText.Printf("%f",m_fNutDe);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("NutDs"))
	{
		sText.Printf("%f",m_fNutDs);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("washer.Width"))
		sText.Printf("%d",washer.wiWidth);
	else if(id==GetPropID("washer.Thick"))
		sText.Printf("%d",washer.wiThick);
	else if(id==GetPropID("washer.HoleIncrement"))
	{
		sText.Printf("%f",washer.fHoleIncrement);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("ucs.orgion.x"))
	{
		sprintf(sText,"%f",ucs.origin.x);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("ucs.orgion.y"))
	{
		sprintf(sText,"%f",ucs.origin.y);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("ucs.orgion.z"))
	{
		sprintf(sText,"%f",ucs.origin.z);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("ucs.axis_x.x"))
	{
		sprintf(sText,"%f",ucs.axis_x.x);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("ucs.axis_x.y"))
	{
		sprintf(sText,"%f",ucs.axis_x.y);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("ucs.axis_x.z"))
	{
		sprintf(sText,"%f",ucs.axis_x.z);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("ucs.axis_y.x"))
	{
		sprintf(sText,"%f",ucs.axis_y.x);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("ucs.axis_y.y"))
	{
		sprintf(sText,"%f",ucs.axis_y.y);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("ucs.axis_y.z"))
	{
		sprintf(sText,"%f",ucs.axis_y.z);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("ucs.axis_z.x"))
	{
		sprintf(sText,"%f",ucs.axis_z.x);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("ucs.axis_z.y"))
	{
		sprintf(sText,"%f",ucs.axis_z.y);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("ucs.axis_z.z"))
	{
		sprintf(sText,"%f",ucs.axis_z.z);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("basePlate"))
		sText.Printf("0X%X",baseplate.hBasePlate);
	else if(id==GetPropID("basePlate.Thick"))
		sText.Printf("%d",baseplate.wiBasePlateThick);
	else if(id==GetPropID("Foundation.HeightSerial"))
		sText.Printf("%d",Foundation.ciHeightSerial);
	else if(id==GetPropID("Foundation.LegSerial"))
		sText.Printf("%d",Foundation.ciLegSerial);
	else if(id==GetPropID("Foundation.LegQuad"))
		sText.Printf("%d",Foundation.ciLegQuad);
	else if(id==GetPropID("Foundation.BoltSerial"))
		sText.Printf("%d",Foundation.ciBoltSerial);
	//
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen-1);
	return strlen(sText);
}
WORD CLDSAnchorBolt::get_wBasePlateThick()
{
	CLDSGeneralPlate *pBasePlate=NULL;
	if(this->baseplate.hBasePlate>0x20)
		pBasePlate=(CLDSGeneralPlate*)m_pModel->FromPartHandle(baseplate.hBasePlate,CLS_PLATE,CLS_PARAMPLATE);
	if(pBasePlate)
		return baseplate.wiBasePlateThick=ftoi(pBasePlate->Thick);
	else
		return baseplate.wiBasePlateThick;
}
void CLDSAnchorBolt::SetAnchorInfo(const ANCHOR_BOLT* pAnchorBolt)
{
	washer.wiWidth=pAnchorBolt->wiWidth;
	washer.wiThick=pAnchorBolt->wiThick;
	washer.fHoleIncrement=pAnchorBolt->wiHoleIncrement;
	m_fNutH=pAnchorBolt->m_fNutH;
	L=m_fLe=pAnchorBolt->m_fLe;
	m_fLr=500;
	m_fNutDe=pAnchorBolt->m_fNutDe;
	m_fNutDs=pAnchorBolt->m_fNutDs;
	d=pAnchorBolt->d;
	m_fNutTaper=pAnchorBolt->m_fNutTaper;
}
//Ϊ����ʾЧ������һЩ�Ǳ�׼�ؽ���˨���в�����ʼ�� wjh-2019.5.9
void CLDSAnchorBolt::InitDefaultAnchorInfo()
{
	int i,liAnchorCount=ANCHOR_BOLT::GetCount();
	const ANCHOR_BOLT *pAnchorBolt, *pPrevAnchorBolt = NULL, *pNextAnchorBolt = NULL;
	for(i=0;i<liAnchorCount;i++)
	{ 
		pAnchorBolt = ANCHOR_BOLT::RecordAt(i);
		if (pAnchorBolt->d == this->d)
		{	//�ҵ����ʵ�
			this->SetAnchorInfo(pAnchorBolt);
			return;
		}
		else if(pAnchorBolt->d>this->d)
		{
			pNextAnchorBolt = pAnchorBolt;
			break;
		}
		pPrevAnchorBolt = pAnchorBolt;

	}
	if (pPrevAnchorBolt == NULL)
		pPrevAnchorBolt = pNextAnchorBolt;
	else if (pNextAnchorBolt == NULL)
		pNextAnchorBolt = pPrevAnchorBolt;
	if (pNextAnchorBolt == NULL)
	{
		Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "�ؽ���˨�ߴ������ʼ��ʧ��!");
		return;
	}
	washer.wiWidth = (pPrevAnchorBolt->wiWidth+ pNextAnchorBolt->wiWidth)/2;
	washer.wiThick = (pPrevAnchorBolt->wiThick + pNextAnchorBolt->wiThick) / 2;
	washer.wiThick = (pPrevAnchorBolt->wiThick + pNextAnchorBolt->wiThick) / 2;
	washer.fHoleIncrement = (pPrevAnchorBolt->wiHoleIncrement + pNextAnchorBolt->wiHoleIncrement)*0.5f;
	m_fNutH = (pPrevAnchorBolt->m_fNutH + pNextAnchorBolt->m_fNutH) / 2;
	L = m_fLe = (pPrevAnchorBolt->m_fLe + pNextAnchorBolt->m_fLe) / 2;
	m_fLr = 500;
	m_fNutH = (pPrevAnchorBolt->m_fNutH + pNextAnchorBolt->m_fNutH) / 2;
	m_fNutDe = (pPrevAnchorBolt->m_fNutDe + pNextAnchorBolt->m_fNutDe) / 2;
	m_fNutDs = (pPrevAnchorBolt->m_fNutDs + pNextAnchorBolt->m_fNutDs) / 2;
	m_fNutTaper = pPrevAnchorBolt->m_fNutTaper;
}
void CLDSAnchorBolt::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	buffer.ReadString(layer_tag,4);
	buffer.Read(&cfgword,sizeof(CFGWORD));
	buffer.ReadPoint(ucs.origin);
	buffer.ReadPoint(ucs.axis_x);
	buffer.ReadPoint(ucs.axis_y);
	buffer.ReadPoint(ucs.axis_z);
	buffer.ReadWord(&washer.wiWidth);
	buffer.ReadWord(&washer.wiThick);
	buffer.ReadFloat(&washer.fHoleIncrement);
	buffer.ReadInteger(&washer.hWasherPlate);
	buffer.ReadWord(&baseplate.wiBasePlateThick);
	buffer.ReadInteger(&baseplate.hBasePlate);
	if(version==0||(doc_type==4&&version>=1030804))	//LDS(V1.3.8.4)
		_d=(short)buffer.ReadInteger();
	//m_fNutTaper=pAnchorBolt->m_fNutTaper;
	buffer.ReadFloat(&m_fNutH);
	buffer.ReadFloat(&m_fNutDe);
	buffer.ReadFloat(&m_fNutDs);
	buffer.ReadFloat(&m_fLe);
	L=m_fLe;
	buffer.ReadFloat(&m_fLr);
	buffer.ReadByte(&Foundation.ciHeightSerial);
	buffer.ReadByte(&Foundation.ciLegSerial);
	buffer.ReadByte(&Foundation.ciLegQuad);
	buffer.ReadByte(&Foundation.ciBoltSerial);
}
void CLDSAnchorBolt::ToBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	buffer.WriteString(layer_tag,4);
	buffer.Write(&cfgword,sizeof(CFGWORD));
	buffer.WritePoint(ucs.origin);
	buffer.WritePoint(ucs.axis_x);
	buffer.WritePoint(ucs.axis_y);
	buffer.WritePoint(ucs.axis_z);
	buffer.WriteWord(washer.wiWidth);
	buffer.WriteWord(washer.wiThick);
	buffer.WriteFloat(washer.fHoleIncrement);
	buffer.WriteInteger(washer.hWasherPlate);
	buffer.WriteWord(baseplate.wiBasePlateThick);
	buffer.WriteInteger(baseplate.hBasePlate);
	if(version==0||(doc_type==4&&version>=1030804))	//LDS(V1.3.8.4)
		buffer.WriteInteger(d);
	buffer.WriteFloat(m_fNutH);
	buffer.WriteFloat(m_fNutDe);
	buffer.WriteFloat(m_fNutDs);
	buffer.WriteFloat(m_fLe);
	buffer.WriteFloat(m_fLr);
	buffer.WriteByte(Foundation.ciHeightSerial);
	buffer.WriteByte(Foundation.ciLegSerial);
	buffer.WriteByte(Foundation.ciLegQuad);
	buffer.WriteByte(Foundation.ciBoltSerial);
}
void CLDSAnchorBolt::UpdateByBasePlate(CLDSGeneralPlate* pBasePlate)
{
	;
}
//////////////////////////////////////////////////////////////////////
//���ʺ���Ϊ��Ա�����ķ�װ����
static void  CreateFaceBy4Vertex(int *vertexArr,CCreateFace *pCreateFace,COLORREF bolt_color,fBody *pBody)
{
	f3dPolyFace* pFace=pBody->faceList.append();
	pFace->material=bolt_color;
	pCreateFace->NewOutterEdgeLine(pFace,vertexArr[1],vertexArr[0]);
	pCreateFace->NewOutterEdgeLine(pFace,vertexArr[2]);
	pCreateFace->NewOutterEdgeLine(pFace,vertexArr[3]);
	pCreateFace->NewOutterEdgeLine(pFace,vertexArr[0]);
	f3dPoint vertex1=pCreateFace->VertexAt(vertexArr[0]);
	f3dPoint vertex2=pCreateFace->VertexAt(vertexArr[1]);
	f3dPoint vertex3=pCreateFace->VertexAt(vertexArr[2]);
	f3dPoint vec1=vertex1-vertex2;
	f3dPoint vec2=vertex3-vertex2;
	pFace->poly_norm=vec1^vec2;
	normalize(pFace->poly_norm);
};
static void  CreateFaceBy3Vertex(int *vertexArr,CCreateFace *pCreateFace,COLORREF bolt_color,fBody *pBody)
{
	f3dPolyFace* pFace=pBody->faceList.append();
	pFace->material=bolt_color;
	pCreateFace->NewOutterEdgeLine(pFace,vertexArr[1],vertexArr[0]);
	pCreateFace->NewOutterEdgeLine(pFace,vertexArr[2]);
	pCreateFace->NewOutterEdgeLine(pFace,vertexArr[0]);
	f3dPoint vertex1=pCreateFace->VertexAt(vertexArr[0]);
	f3dPoint vertex2=pCreateFace->VertexAt(vertexArr[1]);
	f3dPoint vertex3=pCreateFace->VertexAt(vertexArr[2]);
	f3dPoint vec1=vertex2-vertex1;
	f3dPoint vec2=vertex3-vertex2;
	pFace->poly_norm=vec1^vec2;
	normalize(pFace->poly_norm);
};
int Append3dSolidBodyRound(fBody* pBody,double D,double HLe,double Hld,int count=32,COLORREF bolt_color=RGB(155,50,0))
{
	int slices=count;//��Ƭ��
	//���ɶ����
	f3dPoint btm_vertex_arr[200],vertex;
	CLDSPart::GetCircleSimuPolyVertex(D*0.5,btm_vertex_arr,slices);
	//���������body
	f3dPolyFace *pFace;
	for(int i=0;i<slices;i++)
		pBody->vertex.append(f3dPoint(btm_vertex_arr[i].x,btm_vertex_arr[i].y,-Hld));
	for(int i=0;i<slices;i++)
		pBody->vertex.append(f3dPoint(btm_vertex_arr[i].x,btm_vertex_arr[i].y,HLe));
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	int vertexIndexArr[4]={0,0,0,0};
	//��������
	pFace=pBody->faceList.append();
	pFace->poly_norm.Set(0,0,1);
	pFace->material=bolt_color;
	for (int i=slices;i<2*slices;i++)
		createFace.NewOutterEdgeLine(pFace,(i+1)%slices+slices,i);
	//��������
	pFace=pBody->faceList.append();
	pFace->poly_norm.Set(0,0,-1);
	pFace->material=bolt_color;
	for (int i=0;i<slices;i++)
		createFace.NewOutterEdgeLine(pFace,(i+1)%slices,i);
	//3.���������
	for(int i=0;i<slices;i++)
	{
		vertexIndexArr[0]=i;
		vertexIndexArr[1]=i+slices;
		vertexIndexArr[2]=(i+1)%slices+slices;
		vertexIndexArr[3]=(i+1)%slices;
		CreateFaceBy4Vertex(vertexIndexArr,&createFace,bolt_color,pBody);
	}

	return slices*2;//pBody->vertex.GetNodeNum();
}

int AppendBoltCapSolidBodydown(fBody* pBody,float D,float d,float H2,float origin_x,float origin_y,float origin_z,int count=48,COLORREF crLs=RGB(0,250,50))
{
	double m_fNutTaper=0.35;
	int i=0;
	int SolidLines=6;//��ĸ�ı���
	double D_2=D/2;
	f3dPoint vertex_arr[12];
	double cosa_arr[6]={1.0,0.5,-0.5,-1.0,-0.5,0.5};
	double sina_arr[6]={0.0,0.8660254037844,0.8660254037844,0.0,-0.8660254037844,-0.8660254037844};
	double w,h;
	w=(D-d)/2;
	double d_2;
	d_2=min(D_2*cos(2*Pi/(SolidLines*2)),d/2);
	h=w*cos(m_fNutTaper);//�¶ȵĸ�
	//���ɲ����
	for(i=0;i<SolidLines;i++)
	{
		vertex_arr[i].Set(cosa_arr[i]*D_2+origin_x,sina_arr[i]*D_2+origin_y,origin_z+h);		//��ñ������
		vertex_arr[SolidLines+i].Set(vertex_arr[i].x,vertex_arr[i].y,vertex_arr[i].z+H2-2*h);//��ñ����
	}
	int index=pBody->vertex.GetNodeNum();
	//���ɶ����
	f3dPoint btm_vertex_arr[200],b_vertex_arr[100];
	CLDSPart::GetCircleSimuPolyVertex(d_2,b_vertex_arr,count);
	for(i=0;i<count;i++)
	{
		btm_vertex_arr[i].Set(b_vertex_arr[i].x+origin_x,b_vertex_arr[i].y+origin_y,origin_z);		//��ñ������
		btm_vertex_arr[count+i].Set(btm_vertex_arr[i].x,btm_vertex_arr[i].y,btm_vertex_arr[i].z+H2);//��ñ����
	}
	//���������Body������ �²����+�¶����+�ϲ����+�϶����
	for (i=0;i<=2;i++)
	{
		for(int j=SolidLines*i;j<SolidLines*(i+1);j++)
			pBody->vertex.append(vertex_arr[j]);
		for(int j=i*count;j<count*(i+1);j++)
			pBody->vertex.append(btm_vertex_arr[j]);
	}
	//��ʼ������
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	int vertexIndexArr[4]={0,0,0,0};
	//��ĸ����
	f3dPolyFace *pFace=pBody->faceList.append();
	pFace->poly_norm.Set(0,0,1);
	pFace->material=crLs;
	for (i=count+SolidLines*2;i<(count+SolidLines)*2;i++)
		createFace.NewOutterEdgeLine(pFace,(i-SolidLines*2+1)%count+count+SolidLines*2+index,i+index);
	//��ĸ����
	pFace=pBody->faceList.append();
	pFace->poly_norm.Set(0,0,-1);
	pFace->material=crLs;
	for (i=SolidLines;i<count+SolidLines;i++)
		createFace.NewOutterEdgeLine(pFace,(i-SolidLines+1)%count+SolidLines+index,i+index);
	//��ĸ��������
	for(i=SolidLines;i<count+SolidLines;i++)
	{
		int j=0;
		int k=(i-SolidLines+count/(SolidLines*2))%count;
		j=k/(count/SolidLines);
		vertexIndexArr[0]=i+index;
		vertexIndexArr[1]=(i-SolidLines+1)%count+SolidLines+index;
		vertexIndexArr[2]=j+index;
		CreateFaceBy3Vertex(vertexIndexArr,&createFace,crLs,pBody);
	}
	//��ĸ��������
	for(i=count+SolidLines*2;i<(count+SolidLines)*2;i++)
	{
		int j=0;
		int k=(i-SolidLines*2-count+count/(SolidLines*2))%count;
		j=k/(count/SolidLines)+SolidLines+count;
		
		vertexIndexArr[2]=i+index;
		vertexIndexArr[1]=(i-SolidLines*2+1)%count+count+SolidLines*2+index;
		vertexIndexArr[0]=j+index;
		CreateFaceBy3Vertex(vertexIndexArr,&createFace,crLs,pBody);
	}
	int iDifference=count/(SolidLines*2)+SolidLines;//������붥��Ͳ�����е��������
	//��ĸ�Ĳ���
	for(i=0;i<SolidLines;i++)
	{
		vertexIndexArr[3]=i+index;
		vertexIndexArr[2]=(i+1)%SolidLines+index;
		vertexIndexArr[1]=(i+1)%SolidLines+count+SolidLines+index;
		vertexIndexArr[0]=i+count+SolidLines+index;
		CreateFaceBy4Vertex(vertexIndexArr,&createFace,crLs,pBody);
		//��ĸ�����붥���е㴦���ɵ�С��
		vertexIndexArr[2]=i+index;
		vertexIndexArr[1]=(i+1)%SolidLines+index;
		vertexIndexArr[0]=i*count/SolidLines+iDifference+index;
		CreateFaceBy3Vertex(vertexIndexArr,&createFace,crLs,pBody);
		vertexIndexArr[0]=i+count+SolidLines+index;
		vertexIndexArr[1]=(i+1)%SolidLines+count+SolidLines+index;
		vertexIndexArr[2]=i*count/SolidLines+count+SolidLines+iDifference+index;
		CreateFaceBy3Vertex(vertexIndexArr,&createFace,crLs,pBody);
	}
	return pBody->vertex.GetNodeNum();
}


int AppendBoltCapSolidBodyEx(fBody* pBody,double D,double d,double H2,double origin_x,double origin_y,double origin_z,int count=48,COLORREF crLs=RGB(0,150,150))
{
	int i=0;
	int SolidLines=6;//��ĸ����
	double D_2=D/2.0;
	f3dPoint vertex_arr[12];
	double cosa_arr[6]={1.0,0.5,-0.5,-1.0,-0.5,0.5};
	double sina_arr[6]={0.0,0.8660254037844,0.8660254037844,0.0,-0.8660254037844,-0.8660254037844};
	double w,h;
	w=(D-d)/2;  
	double m_fNutTaper=0.35;
	h=w*cos(m_fNutTaper);//�¶ȵ�H
	int index=pBody->vertex.GetNodeNum();
	//������ĸ�����
	for(i=0;i<SolidLines;i++)
	{
		vertex_arr[i].Set(cosa_arr[i]*D_2+origin_x,sina_arr[i]*D_2+origin_y,origin_z);		//��ñ������
		vertex_arr[SolidLines+i].Set(vertex_arr[i].x,vertex_arr[i].y,vertex_arr[i].z+H2-h);//��ñ����
	}
	//������ĸ�����
	double d_2=min(D_2*cos(2*Pi/(SolidLines*2)),d/2);
	f3dPoint btm_vertex_arr[100];
	CLDSPart::GetCircleSimuPolyVertex(d_2,btm_vertex_arr,count);
	//���������pBody�����������㣬�����붥��Բ�ĵ�
	for(int j=0;j<SolidLines*2;j++)
		pBody->vertex.append(vertex_arr[j]);
	for(int j=0;j<count;j++)
		pBody->vertex.append(f3dPoint(btm_vertex_arr[j].x+origin_x,btm_vertex_arr[j].y+origin_y,origin_z+H2));
	
	//��ʼ������
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	int vertexIndexArr[4]={0,0,0,0};
	//����
	f3dPolyFace*pFace=pBody->faceList.append();
	pFace->poly_norm.Set(0,0,-1);
	pFace->material=crLs;
	for (i=0;i<SolidLines;i++)
		createFace.NewOutterEdgeLine(pFace,(i+1)%SolidLines+index,i+index);
	//����
	pFace=pBody->faceList.append();
	pFace->poly_norm.Set(0,0,1);
	pFace->material=crLs;
	for (i=SolidLines*2;i<count+SolidLines*2;i++)
		createFace.NewOutterEdgeLine(pFace,(i-SolidLines*2+1)%count+SolidLines*2+index,i+index);
	//��ĸ����
	for(i=SolidLines*2;i<count+SolidLines*2;i++)
	{
		int j=0;
		int q=(i-SolidLines*2+count/(SolidLines*2))%count;
		j=q/(count/SolidLines)+SolidLines;
		vertexIndexArr[2]=i+index;
		vertexIndexArr[1]=(i-SolidLines*2+1)%count+SolidLines*2+index;
		vertexIndexArr[0]=j+index;
		CreateFaceBy3Vertex(vertexIndexArr,&createFace,crLs,pBody);
	}
	int iDifference=count/(SolidLines*2)+SolidLines;//������붥��Ͳ�����е��������
	//��ĸ�Ĳ���
	for(i=0;i<SolidLines;i++)
	{
		vertexIndexArr[3]=i+index;
		vertexIndexArr[2]=(i+1)%SolidLines+index;
		vertexIndexArr[1]=(i+1)%SolidLines+SolidLines+index;
		vertexIndexArr[0]=i+SolidLines+index;
		CreateFaceBy4Vertex(vertexIndexArr,&createFace,crLs,pBody);
		//��ĸ�����붥���е㴦���ɵ���
		vertexIndexArr[0]=i+SolidLines+index;
		vertexIndexArr[1]=(i+1)%SolidLines+SolidLines+index;
		vertexIndexArr[2]=i*count/SolidLines+SolidLines+iDifference+index;
		CreateFaceBy3Vertex(vertexIndexArr,&createFace,crLs,pBody);
	}
	return pBody->vertex.GetNodeNum();
}


int AppendBoltWasherPlateSolidBody(fBody* pBody,double D,double H2,double origin_x,double origin_y,double origin_z,int lines=4,COLORREF crLs=RGB(55,50,50))
{
	int i=0;
	double D_2=D/2.0;
	f3dPoint vertex_arr[8];
	double cosa_arr[4]={1.0,-1.0,-1.0,1.0};
	double sina_arr[4]={1.0,1.0,-1.0,-1.0};
	//���ɵ�
	for(i=0;i<lines;i++)
	{
		vertex_arr[i].Set(cosa_arr[i]*D_2+origin_x,sina_arr[i]*D_2+origin_y,origin_z);		//��ñ������
		vertex_arr[lines+i].Set(vertex_arr[i].x,vertex_arr[i].y,vertex_arr[i].z+H2);//��ñ����
	}
	int index=pBody->vertex.GetNodeNum();
	for(i=0;i<(lines*2);i++)
		pBody->vertex.append(vertex_arr[i]);
	//��ʼ������
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	//1.��Ƭ��������
	f3dPolyFace* pFace = pBody->faceList.append();
	pFace->material = crLs;
	pFace->poly_norm.Set(0,0,-1);
	for(i=0;i<lines;i++)
		createFace.NewOutterEdgeLine(pFace,((lines-1)*2-i)%lines+index,lines-1-i+index);
	//2.��Ƭ����
	pFace = pBody->faceList.append();
	pFace->material = crLs;
	pFace->poly_norm.Set(0,0,1);
	for(i=0;i<lines;i++)
		createFace.NewOutterEdgeLine(pFace,lines+(i+1)%lines+index,lines+i+index);
	//3.��Ƭ���������
	int vertexIndexArr[4]={0,0,0,0};
	for(i=0;i<lines;i++)
	{
		vertexIndexArr[3]=i+index;
		vertexIndexArr[2]=(i+1)%lines+index;
		vertexIndexArr[1]=(i+1)%lines+lines+index;
		vertexIndexArr[0]=i+lines+index;
		CreateFaceBy4Vertex(vertexIndexArr,&createFace,crLs,pBody);
	}
	return pBody->vertex.GetNodeNum();
}
BOOL CLDSAnchorBolt::CreateAnchorBoltSolidBody(fBody* pBody,const ANCHOR_BOLT* pAnchorBoltPara)
{	//���ɵؽ���˨��ʵ�壨Ҫ����˨Բ��������32����Ƭ���ȣ���ñ�Ϲ���Բ׶��Ƭ����Ϊ48��)
	int Index=Append3dSolidBodyRound(pBody,pAnchorBoltPara->d,pAnchorBoltPara->m_fLe,200,32,RGB(200,200,200));
	return true;
}
BOOL CLDSAnchorBolt::CreateAnchorBoltCapSolidBody(fBody* pBody,const ANCHOR_BOLT* pAnchorBoltPara)
{
	WORD wiOffsetZ=0;
	//���ɵؽ���˨���ʵ��
	AppendBoltWasherPlateSolidBody(pBody,pAnchorBoltPara->wiWidth,pAnchorBoltPara->wiThick,0,0,wiOffsetZ);
	//�����������ؽ���˨ñʵ��
	AppendBoltCapSolidBodyEx(pBody,pAnchorBoltPara->m_fNutDe,pAnchorBoltPara->m_fNutDs,pAnchorBoltPara->m_fNutH,0,0,wiOffsetZ+pAnchorBoltPara->wiThick,48,RGB(200,200,200));
	AppendBoltCapSolidBodydown(pBody,pAnchorBoltPara->m_fNutDe,pAnchorBoltPara->m_fNutDs,pAnchorBoltPara->m_fNutH,0,0,wiOffsetZ+pAnchorBoltPara->wiThick+pAnchorBoltPara->m_fNutH,48,RGB(200,200,200)); 
	return false;
}
//////////////////////////////////////////////////////////////////////
// ���ɵؽ���˨ʵ��
#include "../StdPartSolid/StdMetaSolid.h"
BOOL CLDSAnchorBolt::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	fBody body;
	fBody* pBody=&body;
	if(pSolidBody!=NULL&& !IsSolidModified())
		return TRUE;
	else
	{
		if(pSolidBody == NULL)
			pSolidBody = new CSolidBody;
		else
			pSolidBody->Empty();
	}
	SetModified(FALSE);
	//���ɵؽ���˨ʵ�壨Ҫ����˨Բ��������32����Ƭ���ȣ���ñ�Ϲ���Բ׶��Ƭ����Ϊ48��)
	int Index=0;
	COLORREF crAnchorBolt=RGB(200,200,200);
	//���ɵؽ���˨��ʵ��
	Append3dSolidBodyRound(pBody,d,m_fLe,m_fLr,32,crAnchorBolt);
	//���ɵؽ���˨���ʵ��
	AppendBoltWasherPlateSolidBody(pBody,washer.wiWidth,washer.wiThick,0,0,wBasePlateThick);
	//�����������ؽ���˨ñʵ��
	int SLICES=48;
	GECS acs;
	acs.InitStdCS();
	acs.origin.z=wBasePlateThick+washer.wiThick;
	METASOLID::AppendTaperNutSolid(pBody,m_fNutDe,m_fNutDs,m_fNutH,acs,SLICES,crAnchorBolt);
	acs.origin.z=wBasePlateThick+washer.wiThick+m_fNutH;
	METASOLID::AppendTaperNutSolid(pBody,m_fNutDe,m_fNutDs,m_fNutH,acs,SLICES,crAnchorBolt);
	//AppendBoltCapSolidBodyEx(pBody,m_fNutDe,m_fNutDs,m_fNutH,0,0,wBasePlateThick+washer.wiThick,48,crAnchorBolt);
	//AppendBoltCapSolidBodydown(pBody,m_fNutDe,m_fNutDs,m_fNutH,0,0,wBasePlateThick+washer.wiThick+m_fNutH,48,crAnchorBolt); 
	//����ϵת����ģ������ϵ��
	//GECS acs=ucs;
	acs=ucs;
	//Ĭ������ϵ����������ʱ��ʵ����ʾ����� wjh-2019.4.16
	acs.axis_y=acs.axis_z^acs.axis_x;
	normalize(acs.axis_y);
	for(f3dPoint* pVertex=pBody->vertex.GetFirst();pVertex;pVertex=pBody->vertex.GetNext())
	{
		GEPOINT pos=acs.TransPFromCS(*pVertex);
		pVertex->Set(pos.x,pos.y,pos.z);
	}
	for(f3dPolyFace* pFace=pBody->faceList.GetFirst();pFace;pFace=pBody->faceList.GetNext())
	{
		if(!pFace->poly_norm.IsZero())
			pFace->poly_norm=acs.TransVFromCS(pFace->poly_norm);
	}
	//TODO:��������fBody�н�����̫���С�ڴ������ͷţ��ᵼ�³������к���������Debug�汾�� wjh-2018.8.9
	pSolidBody->ConvertFrom(pBody);
	return TRUE;
}
