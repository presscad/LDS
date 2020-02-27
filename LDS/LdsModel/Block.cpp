//<LOCALE_TRANSLATE BY hxr /> 2014-12-9
#include "stdafx.h"
#include "Tower.h"
#include ".\block.h"
#include "SolidBodyBuffer.h"
#include "XmlModel.h"
#include "..\CryptBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GEPOINT DYNA_RELA_POINT::GetBlockPoint()
{
	return GEPOINT();
}
GEPOINT DYNA_RELA_POINT::GetModelPoint()
{
	return GEPOINT();
}
GEPOINT DYNA_RELA_VECTOR::GetBlockVector()
{
	return GEPOINT();
}
GEPOINT DYNA_RELA_VECTOR::GetModelVector()
{
	return GEPOINT();
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const DWORD HASHTABLESIZE = 500;
const DWORD STATUSHASHTABLESIZE = 50;
CHashStrList<PROPLIST_ITEM> CBlockModel::propHashtable;
CHashSet<PROPLIST_ITEM*>CBlockModel::hashPropItemById;
CXhChar100 CBlockModel::HELPLINK_URL;
CHashTable<DWORD> CBlockModel::propStatusHashtable;
int CBlockModel::m_iCurDisplayPropGroup;
void CBlockModel::InitBlockModelPropHashtable()
{
	int id=1;
	CBlockModel::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CBlockModel::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CBlockModel::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"block's basic info"));
	CBlockModel::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CBlockModel::AddPropItem("name",PROPLIST_ITEM(id++,"name"));
	CBlockModel::AddPropItem("m_bEmbeded",PROPLIST_ITEM(id++,"embedded"));
	CBlockModel::AddPropItem("partList",PROPLIST_ITEM(id++,"part's info"));
	//
	CBlockModel::AddPropItem("position",PROPLIST_ITEM(id++,"Positioning direction"));
	CBlockModel::AddPropItem("lcs.origin",PROPLIST_ITEM(id++,"location"));
	CBlockModel::AddPropItem("lcs.origin.x",PROPLIST_ITEM(id++,"X"));
	CBlockModel::AddPropItem("lcs.origin.y",PROPLIST_ITEM(id++,"Y"));
	CBlockModel::AddPropItem("lcs.origin.z",PROPLIST_ITEM(id++,"Z"));
	CBlockModel::AddPropItem("des_lcs.m_eDatumPlane",PROPLIST_ITEM(id++,"datum plane"));
	CBlockModel::AddPropItem("lcs.axis_x",PROPLIST_ITEM(id++,"X Axis"));
	CBlockModel::AddPropItem("lcs.axis_x.x",PROPLIST_ITEM(id++,"X"));
	CBlockModel::AddPropItem("lcs.axis_x.y",PROPLIST_ITEM(id++,"Y"));
	CBlockModel::AddPropItem("lcs.axis_x.z",PROPLIST_ITEM(id++,"Z"));
	CBlockModel::AddPropItem("lcs.axis_y",PROPLIST_ITEM(id++,"Y Axis"));
	CBlockModel::AddPropItem("lcs.axis_y.x",PROPLIST_ITEM(id++,"X"));
	CBlockModel::AddPropItem("lcs.axis_y.y",PROPLIST_ITEM(id++,"Y"));
	CBlockModel::AddPropItem("lcs.axis_y.z",PROPLIST_ITEM(id++,"Z"));
	CBlockModel::AddPropItem("lcs.axis_z",PROPLIST_ITEM(id++,"Z Axis"));
	CBlockModel::AddPropItem("lcs.axis_z.x",PROPLIST_ITEM(id++,"X"));
	CBlockModel::AddPropItem("lcs.axis_z.y",PROPLIST_ITEM(id++,"Y"));
	CBlockModel::AddPropItem("lcs.axis_z.z",PROPLIST_ITEM(id++,"Z"));
#else
	CBlockModel::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"������Ϣ"));
	CBlockModel::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CBlockModel::AddPropItem("name",PROPLIST_ITEM(id++,"����"));
	CBlockModel::AddPropItem("m_bEmbeded",PROPLIST_ITEM(id++,"��Ƕ"));
	CBlockModel::AddPropItem("partList",PROPLIST_ITEM(id++,"������Ϣ")); 
	//
	CBlockModel::AddPropItem("position",PROPLIST_ITEM(id++,"��λ��λ"));
	CBlockModel::AddPropItem("lcs.origin",PROPLIST_ITEM(id++,"λ��"));
	CBlockModel::AddPropItem("lcs.origin.x",PROPLIST_ITEM(id++,"X����"));
	CBlockModel::AddPropItem("lcs.origin.y",PROPLIST_ITEM(id++,"Y����"));
	CBlockModel::AddPropItem("lcs.origin.z",PROPLIST_ITEM(id++,"Z����"));
	CBlockModel::AddPropItem("des_lcs.m_eDatumPlane",PROPLIST_ITEM(id++,"��׼��"));
	CBlockModel::AddPropItem("lcs.axis_x",PROPLIST_ITEM(id++,"X�᷽��"));
	CBlockModel::AddPropItem("lcs.axis_x.x",PROPLIST_ITEM(id++,"X�������"));
	CBlockModel::AddPropItem("lcs.axis_x.y",PROPLIST_ITEM(id++,"Y�������"));
	CBlockModel::AddPropItem("lcs.axis_x.z",PROPLIST_ITEM(id++,"Z�������"));
	CBlockModel::AddPropItem("lcs.axis_y",PROPLIST_ITEM(id++,"Y�᷽��"));
	CBlockModel::AddPropItem("lcs.axis_y.x",PROPLIST_ITEM(id++,""));
	CBlockModel::AddPropItem("lcs.axis_y.y",PROPLIST_ITEM(id++,"Y�������"));
	CBlockModel::AddPropItem("lcs.axis_y.z",PROPLIST_ITEM(id++,"Z�������"));
	CBlockModel::AddPropItem("lcs.axis_z",PROPLIST_ITEM(id++,"Z�᷽��"));
	CBlockModel::AddPropItem("lcs.axis_z.x",PROPLIST_ITEM(id++,"X�������"));
	CBlockModel::AddPropItem("lcs.axis_z.y",PROPLIST_ITEM(id++,"Y�������"));
	CBlockModel::AddPropItem("lcs.axis_z.z",PROPLIST_ITEM(id++,"Z�������"));
#endif
}

CHashStrList<PROPLIST_ITEM> CBlockReference::propHashtable;
CXhChar100 CBlockReference::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*> CBlockReference::hashPropItemById;
CHashTable<DWORD> CBlockReference::propStatusHashtable;
int CBlockReference::m_iCurDisplayPropGroup;
void CBlockReference::InitBlockReferencePropHashtable()
{
	int id=1;
	CBlockReference::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CBlockReference::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CBlockReference::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CBlockReference::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	CBlockReference::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"block's basic info"));
	CBlockReference::AddPropItem("handle",PROPLIST_ITEM(id++,"handle"));
	CBlockReference::AddPropItem("descript",PROPLIST_ITEM(id++,"Name"));
	CBlockReference::AddPropItem("BelongBlock",PROPLIST_ITEM(id++,"Belong block"));
	CBlockReference::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segement No."));
	CBlockReference::AddPropItem("m_bExploded",PROPLIST_ITEM(id++,"explode"));
	CBlockReference::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Cfg word"));
	CBlockReference::AddPropItem("layer",PROPLIST_ITEM(id++,"layer"));
	//
	CBlockReference::AddPropItem("acs",PROPLIST_ITEM(id++,"Assembling location"));
	CBlockReference::AddPropItem("acs.origin",PROPLIST_ITEM(id++,"location"));
	CBlockReference::AddPropItem("acs.origin.x",PROPLIST_ITEM(id++,"X"));
	CBlockReference::AddPropItem("acs.origin.y",PROPLIST_ITEM(id++,"Y"));
	CBlockReference::AddPropItem("acs.origin.z",PROPLIST_ITEM(id++,"Z"));
	CBlockReference::AddPropItem("acs.axis_x",PROPLIST_ITEM(id++,"X Axis"));
	CBlockReference::AddPropItem("acs.axis_x.x",PROPLIST_ITEM(id++,"X"));
	CBlockReference::AddPropItem("acs.axis_x.y",PROPLIST_ITEM(id++,"Y"));
	CBlockReference::AddPropItem("acs.axis_x.z",PROPLIST_ITEM(id++,"Z"));
	CBlockReference::AddPropItem("acs.axis_y",PROPLIST_ITEM(id++,"Y Axis"));
	CBlockReference::AddPropItem("acs.axis_y.x",PROPLIST_ITEM(id++,"X"));
	CBlockReference::AddPropItem("acs.axis_y.y",PROPLIST_ITEM(id++,"Y"));
	CBlockReference::AddPropItem("acs.axis_y.z",PROPLIST_ITEM(id++,"Z"));
	CBlockReference::AddPropItem("acs.axis_z",PROPLIST_ITEM(id++,"Y Axis"));
	CBlockReference::AddPropItem("acs.axis_z.x",PROPLIST_ITEM(id++,"X"));
	CBlockReference::AddPropItem("acs.axis_z.y",PROPLIST_ITEM(id++,"Y"));
	CBlockReference::AddPropItem("acs.axis_z.z",PROPLIST_ITEM(id++,"Z"));
	//
	CBlockReference::AddPropItem("des_acs.m_cDefStyle",PROPLIST_ITEM(id++,""));
	CBlockReference::AddPropItem("des_acs.desOrg",PROPLIST_ITEM(id++,""));
	CBlockReference::AddPropItem("des_acs.m_eDatumPlane",PROPLIST_ITEM(id++,""));
	CBlockReference::AddPropItem("des_acs.desAxisX",PROPLIST_ITEM(id++,""));
	CBlockReference::AddPropItem("des_acs.desAxisY",PROPLIST_ITEM(id++,""));
	CBlockReference::AddPropItem("des_acs.desAxisZ",PROPLIST_ITEM(id++,""));
	CBlockReference::AddPropItem("des_acs.m_hMotherPart",PROPLIST_ITEM(id++,""));
	CBlockReference::AddPropItem("des_acs.m_xMirInfo",PROPLIST_ITEM(id++,""));
#else
	CBlockReference::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"�������û�����Ϣ"));
	CBlockReference::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CBlockReference::AddPropItem("descript",PROPLIST_ITEM(id++,"����"));
	CBlockReference::AddPropItem("BelongBlock",PROPLIST_ITEM(id++,"��������"));
	CBlockReference::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CBlockReference::AddPropItem("m_bExploded",PROPLIST_ITEM(id++,"ը��"));
	CBlockReference::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CBlockReference::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	//
	CBlockReference::AddPropItem("acs",PROPLIST_ITEM(id++,"װ�䷽λ"));
	CBlockReference::AddPropItem("acs.origin",PROPLIST_ITEM(id++,"λ��"));
	CBlockReference::AddPropItem("acs.origin.x",PROPLIST_ITEM(id++,"X����"));
	CBlockReference::AddPropItem("acs.origin.y",PROPLIST_ITEM(id++,"Y����"));
	CBlockReference::AddPropItem("acs.origin.z",PROPLIST_ITEM(id++,"Z����"));
	CBlockReference::AddPropItem("acs.axis_x",PROPLIST_ITEM(id++,"X�᷽��"));
	CBlockReference::AddPropItem("acs.axis_x.x",PROPLIST_ITEM(id++,"X�������"));
	CBlockReference::AddPropItem("acs.axis_x.y",PROPLIST_ITEM(id++,"Y�������"));
	CBlockReference::AddPropItem("acs.axis_x.z",PROPLIST_ITEM(id++,"Z�������"));
	CBlockReference::AddPropItem("acs.axis_y",PROPLIST_ITEM(id++,"Y�᷽��"));
	CBlockReference::AddPropItem("acs.axis_y.x",PROPLIST_ITEM(id++,"X�������"));
	CBlockReference::AddPropItem("acs.axis_y.y",PROPLIST_ITEM(id++,"Y�������"));
	CBlockReference::AddPropItem("acs.axis_y.z",PROPLIST_ITEM(id++,"Z�������"));
	CBlockReference::AddPropItem("acs.axis_z",PROPLIST_ITEM(id++,"Z�᷽��"));
	CBlockReference::AddPropItem("acs.axis_z.x",PROPLIST_ITEM(id++,"X�������"));
	CBlockReference::AddPropItem("acs.axis_z.y",PROPLIST_ITEM(id++,"Y�������"));
	CBlockReference::AddPropItem("acs.axis_z.z",PROPLIST_ITEM(id++,"Z�������"));
	//
	CBlockReference::AddPropItem("des_acs.m_cDefStyle",PROPLIST_ITEM(id++,""));
	CBlockReference::AddPropItem("des_acs.desOrg",PROPLIST_ITEM(id++,""));
	CBlockReference::AddPropItem("des_acs.m_eDatumPlane",PROPLIST_ITEM(id++,""));
	CBlockReference::AddPropItem("des_acs.desAxisX",PROPLIST_ITEM(id++,""));
	CBlockReference::AddPropItem("des_acs.desAxisY",PROPLIST_ITEM(id++,""));
	CBlockReference::AddPropItem("des_acs.desAxisZ",PROPLIST_ITEM(id++,""));
	CBlockReference::AddPropItem("des_acs.m_hMotherPart",PROPLIST_ITEM(id++,""));
	CBlockReference::AddPropItem("des_acs.m_xMirInfo",PROPLIST_ITEM(id++,""));
#endif
}

static void LoadDefaultAcsInfo(CParaCS* pAcs,DWORD key){pAcs->id=key;}
CBlockModel::CBlockModel(BOOL embeded/*=TRUE*/)
{
	strcpy(name,"");
	m_bEmbeded=embeded;
	feature=0;
	this->m_bDataModified=false;
	handle=m_hMinIdleHandle=0x20;
	GetNewHandleHookFunc=NULL;
	//AppendNodeHookFunc=NULL;
	//AppendPartHookFunc=NULL;
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN��������Ա���;
	DisplayProcess=NULL;
	Point.SetDocModifiedFlagAddress(&m_bDataModified);
	Line.SetDocModifiedFlagAddress(&m_bDataModified);
	Plane.SetDocModifiedFlagAddress(&m_bDataModified);
	View.SetDocModifiedFlagAddress(&m_bDataModified);
	Node.SetDocModifiedFlagAddress(&m_bDataModified);
	Parts.SetDocModifiedFlagAddress(&m_bDataModified);
	Point.SetBelongModel(this);
	Line.SetBelongModel(this);
	Plane.SetBelongModel(this);
	View.SetBelongModel(this);
	Node.SetBelongModel(this);
	Parts.SetBelongModel(this);
	LoadDefaultUCS(&lcs);
	lcs.axis_x.Set(1,0,0);
	lcs.axis_y.Set(0,1,0);
	lcs.axis_z.Set(0,0,1);
	CParaCS::PARAMETRIC_INFO param;
	param.m_cDefStyle=0;
	param.m_eDatumPlane=0;	//0.X-Y;1.Y-X;2.Y-Z;3.Z-Y;4.Z-X;5.X-Z 
	param.desOrg.datum_pos_style=0;
	param.desOrg.datum_pos=lcs.origin;
	param.desAxisX.norm_style=0;
	param.desAxisX.vector=lcs.axis_x;
	param.desAxisY.norm_style=0;
	param.desAxisY.vector=lcs.axis_y;
	param.desAxisZ.norm_style=0;
	param.desAxisZ.vector=lcs.axis_z;
	des_lcs.SetParamInfo(param);
	Acs.LoadDefaultObjectInfo=LoadDefaultAcsInfo;
}

CBlockModel::~CBlockModel(void)
{
}
void CBlockModel::ToBuffer(CBuffer& buffer,long version/*=0*/,long doc_type/*=0*/)
{
	/*if( version==0||
		(doc_type==1&&version>=4020004)||	//TMA(V4.2.0.4)
		(doc_type==2&&version>=1020004)||	//LMA(V1.2.0.4)
		(doc_type==3&&version>=1080104)||	//TSA(V1.8.1.4)
		(doc_type==4&&version>=1020004)||	//LDS(V1.2.0.4)
		(doc_type==5&&version>=1020004))	//TDA(V1.2.0.4)
	buffer.WriteDword(dwPermission);*/
	buffer.WritePoint(lcs.origin);
	buffer.WritePoint(lcs.axis_x);
	buffer.WritePoint(lcs.axis_y);
	buffer.WritePoint(lcs.axis_z);
	des_lcs.ToBuffer(buffer);//װ������ϵ��Ʋ���
	buffer.WriteString(name);	//��������������;
	buffer.WriteInteger(m_bEmbeded);//�Ƿ�Ϊ��Ƕʽ�����黹���ⲿ����ʽ
	if(m_bEmbeded)
		return;
	//��ϣ�������Ϣ����
	long objsN=0;
	buffer.LogPosition();
	buffer.WriteInteger(objsN);
	for(CLDSPoint *pPoint=Point.GetFirst();pPoint;pPoint=Point.GetNext())
	{
		buffer.WriteInteger(pPoint->handle);
		buffer.WriteInteger(CLS_GEPOINT);
		objsN++;
	}
	for(CLDSLine *pLine=Line.GetFirst();pLine;pLine=Line.GetNext())
	{
		buffer.WriteInteger(pLine->handle);
		buffer.WriteInteger(CLS_GELINE);
		objsN++;
	}
	for(CLDSPlane *pPlane=Plane.GetFirst();pPlane;pPlane=Plane.GetNext())
	{
		buffer.WriteInteger(pPlane->handle);
		buffer.WriteInteger(CLS_GEPLANE);
		objsN++;
	}
	for(CDisplayView *pView=View.GetFirst();pView;pView=View.GetNext())
	{
		buffer.WriteInteger(pView->handle);
		buffer.WriteInteger(CLS_DISPLAYVIEW);
		objsN++;
	}
	for(CLDSNode *pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		buffer.WriteInteger(pNode->handle);
		buffer.WriteInteger(CLS_NODE);
		objsN++;
	}
	for(CLDSPart *pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		buffer.WriteInteger(pPart->handle);
		buffer.WriteInteger(pPart->GetClassTypeId());
		objsN++;
	}
	buffer.RecallPosition();
	buffer.WriteInteger(objsN);
	buffer.RecallPosition();
	if(	version==0&&doc_type!=3||
		(doc_type==1&&version>=5010800)||	//TMA V5.1.8.0
		(doc_type==2&&version>=2000800)||	//LMA V2.0.8.0	//�����е����9�·ݷ��͵İ汾������11�·ݷ����İ���(˳̩,���ݷɽ������պ�Դ) wjh-2017.11.23
		(doc_type==4&&version>=1030900)||	//LDS V1.3.8.0
		(doc_type==5&&version>=1030800))	//TDA V1.3.8.0
	{
		buffer.WriteInteger(hashExterNodes.GetNodeNum());
		for(EXTERNAL_BLKNODE* pExterNode=hashExterNodes.GetFirst();pExterNode;pExterNode=hashExterNodes.GetNext())
		{
			buffer.WriteInteger(pExterNode->hNode);
			buffer.WritePoint(pExterNode->xPosition);
		}
		buffer.WriteInteger(hashExterRods.GetNodeNum());
		for(EXTERNAL_BLKROD* pExterRod=hashExterRods.GetFirst();pExterRod;pExterRod=hashExterRods.GetNext())
		{
			buffer.WriteInteger(pExterRod->hRod);
			buffer.WriteByte(pExterRod->cbValidProp);
			buffer.WriteInteger(pExterRod->idClsType);
			buffer.WritePoint(pExterRod->xStartPosition);
			buffer.WritePoint(pExterRod->xEndPosition);
			if(pExterRod->cbValidProp.GetBitState(EXTERNAL_BLKROD::PROP_WING_NORM))
			{
				buffer.WritePoint(pExterRod->vxWingNorm);
				buffer.WritePoint(pExterRod->vyWingNorm);
			}
		}
		buffer.WriteInteger(hashExterBolts.GetNodeNum());
		for(EXTERNAL_BLKBOLT* pExterBolt=hashExterBolts.GetFirst();pExterBolt;pExterBolt=hashExterBolts.GetNext())
		{
			buffer.WriteInteger(pExterBolt->hBolt);
			buffer.WriteWord(pExterBolt->wL);
			buffer.WritePoint(pExterBolt->vWorkNorm);
			buffer.WritePoint(pExterBolt->xBasePosition);
		}
	}
	//��ʾ��ͼ
	BUFFERPOP stack(&buffer,View.GetNodeNum());
	buffer.WriteInteger(View.GetNodeNum());
	for(CDisplayView *pView=View.GetFirst();pView;pView=View.GetNext())
	{
		buffer.WriteInteger(pView->handle);
		pView->ToBuffer(buffer,version,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		 CLDSObject::Log2File()->Log("The number record of display view is wrong!");
#else 
		CLDSObject::Log2File()->Log("��ʾ��ͼ��¼�����ִ���!");
#endif
	//��׼��
    stack.Initialize(&buffer,Point.GetNodeNum());
	buffer.WriteInteger(Point.GetNodeNum());
	for(CLDSPoint *pPoint=Point.GetFirst();pPoint;pPoint=Point.GetNext())
	{
		buffer.WriteInteger(pPoint->handle);
		pPoint->ToBuffer(buffer,version,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
	    CLDSObject::Log2File()->Log("The number record of datum point is wrong!");
#else
		CLDSObject::Log2File()->Log("��׼����ִ���!");
#endif
	//��׼��
	stack.Initialize(&buffer,Line.GetNodeNum());
	buffer.WriteInteger(Line.GetNodeNum());
	for(CLDSLine *pLine=Line.GetFirst();pLine;pLine=Line.GetNext())
	{
		buffer.WriteInteger(pLine->handle);
		pLine->ToBuffer(buffer,version,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of datum line is wrong!");
#else
		CLDSObject::Log2File()->Log("��׼�߼�¼�����ִ���!");
#endif
	//��׼ƽ��
	stack.Initialize(&buffer,Plane.GetNodeNum());	
	buffer.WriteInteger(Plane.GetNodeNum());
	for(CLDSPlane* pPlane=Plane.GetFirst();pPlane;pPlane=Plane.GetNext())
	{
		buffer.WriteInteger(pPlane->handle);
		pPlane->ToBuffer(buffer,version,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of datum plane is wrong!");
#else 
		CLDSObject::Log2File()->Log("��׼ƽ����ּ�¼������!");
#endif
	//�ڵ�
	stack.Initialize(&buffer,Node.GetNodeNum());
	buffer.WriteInteger(Node.GetNodeNum());
	for(CLDSNode* pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		buffer.WriteInteger(pNode->handle);
		pNode->ToBuffer(buffer,version,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of node is wrong!!");
#else
		CLDSObject::Log2File()->Log("�ڵ��¼�����ִ���!");
#endif
		
	//�����͸��Ƴɵ�������������Ǹ֡��ְ塢�ֹܡ��۸֡���˨��
	stack.Initialize(&buffer,Parts.GetNodeNum());
	buffer.WriteInteger(Parts.GetNodeNum());
	for(CLDSPart* pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		buffer.WriteInteger(pPart->handle);
		pPart->ToBuffer(buffer,version,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number of tower members made of various types of steel is a mistake!");
#else
		CLDSObject::Log2File()->Log("�����͸��Ƴɵ�����������¼�����ִ���!");
#endif
	//�����������ϵ	wht 13-01-06
	stack.Initialize(&buffer,Acs.GetNodeNum());
	buffer.WriteInteger(Acs.GetNodeNum());
	for (CParaCS *pCS=Acs.GetFirst();pCS;pCS=Acs.GetNext())
	{
		buffer.WriteInteger(pCS->id);
		pCS->ToBuffer(buffer,version,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of save parameter coordinate system  is wrong!");
#else
		CLDSObject::Log2File()->Log("�����������ϵ��¼�����ִ���!");
#endif
}
void CBlockModel::ToXmlFile(FILE* fp,DWORD schema)
{
	fprintf(fp,"<������Ϣ>\n");
	fprintf(fp,"<ԭ������ origin=\"(%.3f,%.3f,%.3f)\"/>\n",lcs.origin.x,lcs.origin.y,lcs.origin.z);
	fprintf(fp,"<x������ axis_x=\"(%.3f,%.3f,%.3f)\"/>\n",lcs.axis_x.x,lcs.axis_x.y,lcs.axis_x.z);
	fprintf(fp,"<y������ axis_y=\"(%.3f,%.3f,%.3f)\"/>\n",lcs.axis_y.x,lcs.axis_y.y,lcs.axis_y.z);
	fprintf(fp,"<z������ axis_z=\"(%.3f,%.3f,%.3f)\"/>\n",lcs.axis_z.x,lcs.axis_z.y,lcs.axis_z.z);
	des_lcs.ToXmlFile(fp,schema);
	fprintf(fp,"<�������� name=\"%s\"/>\n",name);
	fprintf(fp,"<�Ƿ�����Ƕ���� bEmbeded=\"%d\"/>\n",m_bEmbeded);
	if(m_bEmbeded)
	{
		fprintf(fp,"</������Ϣ>\n");
		return;
	}
	long objsN=0;
	fprintf(fp,"<��ʼͳ������ objsN=\"%d\"/>\n",objsN);
	for(CLDSPoint *pPoint=Point.GetFirst();pPoint;pPoint=Point.GetNext())
	{
		fprintf(fp,"<��׼�� handle=\"0x%x\" type=\"CLS_GEPOINT\"/>\n",pPoint->handle);
		objsN++;
	}
	for(CLDSLine *pLine=Line.GetFirst();pLine;pLine=Line.GetNext())
	{
		fprintf(fp,"<��׼�� handle=\"0x%x\" type=\"CLS_GELINE\"/>\n",pLine->handle);
		objsN++;
	}
	for(CLDSPlane *pPlane=Plane.GetFirst();pPlane;pPlane=Plane.GetNext())
	{
		fprintf(fp,"<��׼�� handle=\"0x%x\" type=\"CLS_GEPLANE\"/>\n",pPlane->handle);
		objsN++;
	}
	for(CDisplayView *pView=View.GetFirst();pView;pView=View.GetNext())
	{
		fprintf(fp,"<��ͼ handle=\"0x%x\" type=\"CLS_DISPLAYVIEW\"/>\n",pView->handle);
		objsN++;
	}
	for(CLDSNode *pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		fprintf(fp,"<�ڵ� handle=\"0x%x\" type=\"CLS_NODE\"/>\n",pNode->handle);
		objsN++;
	}
	for(CLDSPart *pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		fprintf(fp,"<���� handle=\"0x%x\" type=\"%d\"/>\n",pPart->handle,pPart->GetClassTypeId());
		objsN++;
	}
	fprintf(fp,"<���ͳ������ objsN=\"%d\"/>\n",objsN);
	fprintf(fp,"<��ͼ���� ViewNum=\"%d\"/>\n",View.GetNodeNum());
	for(CDisplayView *pView=View.GetFirst();pView;pView=View.GetNext())
	{
		fprintf(fp,"<��ͼ handle=\"0x%x\"/>\n",pView->handle);
		pView->ToXmlFile(fp,schema);
	}
	fprintf(fp,"<��׼������ PointNum=\"%d\"/>\n",Point.GetNodeNum());
	for(CLDSPoint *pPoint=Point.GetFirst();pPoint;pPoint=Point.GetNext())
	{
		fprintf(fp,"<��׼�� handle=\"0x%x\"/>\n",pPoint->handle);
		pPoint->ToXmlFile(fp,schema);
	}
	fprintf(fp,"<��׼������ LineNum=\"%d\"/>\n",Line.GetNodeNum());
	for(CLDSLine *pLine=Line.GetFirst();pLine;pLine=Line.GetNext())
	{
		fprintf(fp,"<��׼�� handle=\"0x%x\"/>\n",pLine->handle);
		pLine->ToXmlFile(fp,schema);
	}
	fprintf(fp,"<��׼������ PlaneNum=\"%d\"/>\n",Plane.GetNodeNum());
	for(CLDSPlane* pPlane=Plane.GetFirst();pPlane;pPlane=Plane.GetNext())
	{
		fprintf(fp,"<��׼�� handle=\"0x%x\"/>\n",pPlane->handle);
		pPlane->ToXmlFile(fp,schema);
	}
	fprintf(fp,"<�ڵ����� NodeNum=\"%d\"/>\n",Node.GetNodeNum());
	for(CLDSNode* pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		fprintf(fp,"<�ڵ� handle=\"0x%x\"/>\n",pNode->handle);
		pNode->ToXmlFile(fp,schema);
	}
	fprintf(fp,"<�������� PartNum=\"%d\"/>\n",Parts.GetNodeNum());
	for(CLDSPart* pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		fprintf(fp,"<���� handle=\"0x%x\"/>\n",pPart->handle);
		pPart->ToXmlFile(fp,schema);
	}
	fprintf(fp,"</������Ϣ>\n");
}
void CBlockModel::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	/*if( version==0||
		(doc_type==1&&version>=4020004)||	//TMA(V4.2.0.4)
		(doc_type==2&&version>=1020004)||	//LMA(V1.2.0.4)
		(doc_type==3&&version>=1080104)||	//TSA(V1.8.1.4)
		(doc_type==4&&version>=1020004)||	//LDS(V1.2.0.4)
		(doc_type==5&&version>=1020004))	//TDA(V1.2.0.4)
	buffer.ReadDword(&dwPermission);*/
	buffer.ReadPoint(lcs.origin);
	buffer.ReadPoint(lcs.axis_x);
	buffer.ReadPoint(lcs.axis_y);
	buffer.ReadPoint(lcs.axis_z);
	des_lcs.FromBuffer(buffer,version,doc_type);	//װ������ϵ��Ʋ���
	buffer.ReadString(name,50);	//��������������;
	buffer.ReadInteger(&m_bEmbeded);
	if(m_bEmbeded)
		return;
	long i,n,h,cls_typeid;
	//��ϣ�������Ϣ����
	buffer.ReadInteger(&n);
	CHashTable<CLDSObject*> hashObjs;
	hashObjs.CreateHashTable(n);
	BOOL bHasRepeatHandle=FALSE;
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadInteger(&cls_typeid);
		CLDSObject *pObj=NULL;
		if(hashObjs.GetValueAt(h,pObj))
		{
			if(cls_typeid==pObj->GetClassTypeId())
				continue;	//�Ѵ��ڴ˹������������
			else
			{	//���ֶ������ظ�,Ӧ����ļ�����ԭ��
				bHasRepeatHandle=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Find repeated object handle,communicate with supplier of the software please!";
#else
				throw "���ֶ������ظ������������Ӧ����ϵ��";
#endif
			}
		}
 		if(cls_typeid==CLS_GEPOINT)
		{
			CLDSPoint* pPoint=Point.append(FALSE);
			pPoint->handle=h;
			Point.UpdateHashtable(h,pPoint);
			hashObjs.SetValueAt(h,pPoint);
		}
		else if(cls_typeid==CLS_GELINE)
		{
			CLDSLine* pLine=Line.append(FALSE);
			pLine->handle=h;
			Line.UpdateHashtable(h,pLine);
			hashObjs.SetValueAt(h,pLine);
		}
		else if(cls_typeid==CLS_GEPLANE)
		{
			CLDSPlane* pPlane=Plane.append(FALSE);
			pPlane->handle=h;
			Plane.UpdateHashtable(h,pPlane);
			hashObjs.SetValueAt(h,pPlane);
		}
		else if(cls_typeid==CLS_DISPLAYVIEW)
		{
			CDisplayView* pView=View.append(FALSE);
			pView->handle=h;
			View.UpdateHashtable(h,pView);
			hashObjs.SetValueAt(h,pView);
		}
		else if(cls_typeid==CLS_NODE)
		{
			CLDSNode* pNode=Node.append(FALSE);
			pNode->handle=h;
			//Node.UpdateHashtable(h,pNode);
			hashObjs.SetValueAt(h,pNode);
		}
		else	//CLDSPart���͹���
		{
			CLDSPart* pPart=Parts.append(cls_typeid,FALSE);
			if(pPart)
			{
				pPart->handle=h;
				//Parts.UpdateHashtable(h,pPart);
				hashObjs.SetValueAt(h,pPart);
			}
		}
	}
	int buffer_len=buffer.GetLength();
	Node.RebuildHashtable();
	Parts.RebuildHashtable();
	double process_scale=100.0/buffer_len;
	if(  version==0&&doc_type!=3||
		(doc_type==1&&version>=5010800)||	//TMA V5.1.8.0
		(doc_type==2&&version>=2000800)||	//LMA V2.0.8.0	//�����е����9�·ݷ��͵İ汾������11�·ݷ����İ���(˳̩,���ݷɽ������պ�Դ) wjh-2017.11.23
		(doc_type==4&&version>=1030900)||	//LDS V1.3.8.0
		(doc_type==5&&version>=1030800))	//TDA V1.3.8.0
	{
		buffer.ReadInteger(&n);
		hashExterNodes.Empty();
		for(i=0;i<n;i++)
		{
			buffer.ReadInteger(&h);
			EXTERNAL_BLKNODE* pExterNode=hashExterNodes.Add(h);
			buffer.ReadPoint(pExterNode->xPosition);
		}
		buffer.ReadInteger(&n);
		hashExterRods.Empty();
		for(i=0;i<n;i++)
		{
			buffer.ReadInteger(&h);
			EXTERNAL_BLKROD* pExterRod=hashExterRods.Add(h);
			buffer.ReadByte((BYTE*)&pExterRod->cbValidProp);
			buffer.ReadInteger(&pExterRod->idClsType);
			buffer.ReadPoint(pExterRod->xStartPosition);
			buffer.ReadPoint(pExterRod->xEndPosition);
			if(pExterRod->cbValidProp.GetBitState(EXTERNAL_BLKROD::PROP_WING_NORM))
			{
				buffer.ReadPoint(pExterRod->vxWingNorm);
				buffer.ReadPoint(pExterRod->vyWingNorm);
			}
		}
		buffer.ReadInteger(&n);
		hashExterBolts.Empty();
		for(i=0;i<n;i++)
		{
			buffer.ReadInteger(&h);
			EXTERNAL_BLKBOLT* pExterBolt=hashExterBolts.Add(h);
			buffer.ReadWord(&pExterBolt->wL);
			buffer.ReadPoint(pExterBolt->vWorkNorm);
			buffer.ReadPoint(pExterBolt->xBasePosition);
		}
	}
	//��ʾ��ͼ
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CDisplayView* pView=View.FromHandle(h);
		pView->FromBuffer(buffer,version,doc_type);
		if(DisplayProcess!=NULL)
			DisplayProcess(ftoi(buffer.GetCursorPosition()*process_scale),NULL);
	}
	//��׼��
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CLDSPoint *pPoint=Point.FromHandle(h);
		pPoint->FromBuffer(buffer,version,doc_type);
		if(DisplayProcess!=NULL)
			DisplayProcess(ftoi(buffer.GetCursorPosition()*process_scale),NULL);
	}
	//��׼��
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CLDSLine *pLine=Line.FromHandle(h);
		pLine->FromBuffer(buffer,version,doc_type);
		if(DisplayProcess!=NULL)
			DisplayProcess(ftoi(buffer.GetCursorPosition()*process_scale),NULL);
	}
	//��׼ƽ��
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CLDSPlane* pPlane=Plane.FromHandle(h);
		pPlane->FromBuffer(buffer,version,doc_type);
		if(DisplayProcess!=NULL)
			DisplayProcess(ftoi(buffer.GetCursorPosition()*process_scale),NULL);
	}
	//�ڵ�
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CLDSNode* pNode=Node.FromHandle(h);
		pNode->FromBuffer(buffer,version,doc_type);
		if(DisplayProcess!=NULL)
			DisplayProcess(ftoi(buffer.GetCursorPosition()*process_scale),NULL);
	};
	//�����͸��Ƴɵ�������������Ǹ֡��ְ塢�ֹܡ��۸֡���˨��
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CLDSPart* pPart=Parts.FromHandle(h);
		pPart->FromBuffer(buffer,version,doc_type);
		if(DisplayProcess!=NULL)
			DisplayProcess(ftoi(buffer.GetCursorPosition()*process_scale),NULL);
	}
	//��ȡ����������ϵ wht 13-01-06
	if( version==0||
		(doc_type==1&&version>=4020008)||	//TMA(V4.2.0.4)
		(doc_type==2&&version>=1020008)||	//LMA(V1.2.0.4)
		(doc_type==3&&version>=2020008)||	//TSA(V1.8.1.4)
		(doc_type==4&&version>=1020008)||	//LDS(V1.2.0.4)
		(doc_type==5&&version>=1020008))	//TDA(V1.2.0.4)
	{
		n=0;
		buffer.ReadInteger(&n);
		Acs.Empty();
		for (i=0;i<n;i++)
		{
			int id=0;
			buffer.ReadInteger(&id);
			CParaCS *pCS = Acs.Add(id);//		pCS->id = id;
			pCS->FromBuffer(buffer,version,doc_type);
		}
	}
}
bool CBlockModel::ExportToBlkFile(const char* blk_file,long version,IModel *pTowerModel/*=NULL*/, BOOL bSaveTaAsModel/*=FALSE*/)
{
	CBuffer buffer(10000000);
	int doc_type=PRODUCT_TYPE;
	//
	buffer.WritePoint(lcs.origin);
	buffer.WritePoint(lcs.axis_x);
	buffer.WritePoint(lcs.axis_y);
	buffer.WritePoint(lcs.axis_z);
	des_lcs.ToBuffer(buffer,version,doc_type);//װ������ϵ��Ʋ���
	buffer.WriteString(name);	//��������������;
	if((m_bEmbeded||bSaveTaAsModel)&&(pTowerModel==NULL||!pTowerModel->IsTowerModel()))
		return false;
	CHashSet<CLDSNodePtr>hashNodePtr;
	CHashSet<CLDSPart*>hashPartPtr;
	CHashList<CLDSPoint*>hashPointPtr;
	CHashList<CLDSLine*>hashLinePtr;
	CHashList<CLDSPlane*>hashPlanePtr;
	CHashList<CDisplayView*>hashViewPtr;
	CLDSPoint *pPoint,**ppPoint;
	CLDSLine *pLine,**ppLine;
	CLDSPlane *pPlane,**ppPlane;
	CDisplayView *pView,**ppView;
	CLDSNode *pNode;
	CLDSPart *pPart;

	if(m_bEmbeded)
	{
		CTower *pTower=(CTower*)pTowerModel;
		for(pPoint=pTower->Point.GetFirst();pPoint;pPoint=pTower->Point.GetNext())
		{
			if(pPoint->m_hBlock==handle)
				hashPointPtr.SetValue(pPoint->handle,pPoint);
		}
		for(pLine=pTower->Line.GetFirst();pLine;pLine=pTower->Line.GetNext())
		{
			if(pLine->m_hBlock==handle)
				hashLinePtr.SetValue(pLine->handle,pLine);
		}
		for(pPlane=pTower->Plane.GetFirst();pPlane;pPlane=pTower->Plane.GetNext())
		{
			if(pPlane->m_hBlock==handle)
				hashPlanePtr.SetValue(pPlane->handle,pPlane);
		}
		for(pView=pTower->View.GetFirst();pView;pView=pTower->View.GetNext())
		{
			if(pView->m_hBlock==handle)
				hashViewPtr.SetValue(pView->handle,pView);
		}
		for(pNode=pTower->Node.GetFirst();pNode;pNode=pTower->Node.GetNext())
		{
			pNode->Unlock();
			if(pNode->m_hBlock==handle)
				hashNodePtr.SetValue(pNode->handle,pNode);
		}
		for(pPart=pTower->Parts.GetFirst();pPart;pPart=pTower->Parts.GetNext())
		{
			pPart->Unlock();
			if(pPart->m_hBlock==handle)
				hashPartPtr.SetValue(pPart->handle,pPart);
		}
	}
	else if(bSaveTaAsModel)
	{	//����Ƕʽ�Ĳ���������ǰ�ļ����Ϊ���� wht 12-08-24
		CTower *pTower=(CTower*)pTowerModel;
		for(pPoint=pTower->Point.GetFirst();pPoint;pPoint=pTower->Point.GetNext())
			hashPointPtr.SetValue(pPoint->handle,pPoint);
		for(pLine=pTower->Line.GetFirst();pLine;pLine=pTower->Line.GetNext())
			hashLinePtr.SetValue(pLine->handle,pLine);
		for(pPlane=pTower->Plane.GetFirst();pPlane;pPlane=pTower->Plane.GetNext())
			hashPlanePtr.SetValue(pPlane->handle,pPlane);
		for(pView=pTower->View.GetFirst();pView;pView=pTower->View.GetNext())
		{
			if(pView->m_iViewType<=1)	//ģ���н�����ͼ����ͼ��������ͼ
				hashViewPtr.SetValue(pView->handle,pView);
		}
		for(pNode=pTower->Node.GetFirst();pNode;pNode=pTower->Node.GetNext())
			hashNodePtr.SetValue(pNode->handle,pNode);
		for(pPart=pTower->Parts.GetFirst();pPart;pPart=pTower->Parts.GetNext())
			hashPartPtr.SetValue(pPart->handle,pPart);
		Acs.Empty();
		for(CParaCS* pAcs=pTower->Acs.GetFirst();pAcs;pAcs=pTower->Acs.GetNext())
			Acs.SetValue(pAcs->id,*pAcs);
	}
	else
	{
		for(pPoint=Point.GetFirst();pPoint;pPoint=Point.GetNext())
			hashPointPtr.SetValue(pPoint->handle,pPoint);
		for(pLine=Line.GetFirst();pLine;pLine=Line.GetNext())
			hashLinePtr.SetValue(pLine->handle,pLine);
		for(pPlane=Plane.GetFirst();pPlane;pPlane=Plane.GetNext())
			hashPlanePtr.SetValue(pPlane->handle,pPlane);
		for(pView=View.GetFirst();pView;pView=View.GetNext())
			hashViewPtr.SetValue(pView->handle,pView);
		for(pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
			hashNodePtr.SetValue(pNode->handle,pNode);
		for(pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
			hashPartPtr.SetValue(pPart->handle,pPart);
	}
	if(m_bEmbeded)
	{	//ʵʱ��ȡ��Χ�����ڵ���˼�
		SmartPartPtr pSmartPart;
		CLDSPart::_console->EmptyBlkObjSetBuf();
		for(pNode=hashNodePtr.GetFirst();pNode;pNode=hashNodePtr.GetNext())
			pNode->AddToConsoleObjBuf(true,false);
		for(pPart=hashPartPtr.GetFirst();pPart;pPart=hashPartPtr.GetNext())
			pPart->AddToConsoleObjBuf(true,false);
		hashExterNodes.Empty();
		hashExterRods.Empty();
		hashExterBolts.Empty();
		for(pNode=CLDSPart::_console->EnumBlkBufNodeFirst();pNode;pNode=CLDSPart::_console->EnumBlkBufNodeNext())
		{
			if(hashNodePtr.GetValue(pNode->handle)!=NULL)
				continue;
			hashExterNodes.SetValue(pNode->handle,EXTERNAL_BLKNODE(pNode));
		}
		for(pSmartPart=CLDSPart::_console->EnumBlkBufPartFirst();pSmartPart.IsHasPtr();pSmartPart=CLDSPart::_console->EnumBlkBufPartNext())
		{
			if(hashPartPtr.GetValue(pSmartPart->handle)!=NULL)
				continue;
			if(pSmartPart->GetClassTypeId()==CLS_BOLT)
				hashExterBolts.SetValue(pSmartPart.pBolt->handle,EXTERNAL_BLKBOLT(pSmartPart.pBolt));
			else if(pSmartPart->IsLinePart())
			{
				if(pSmartPart.pRod->pStart==NULL)
				{
					pSmartPart.pRod->Log2File()->Log(CXhChar50("��Χ�����˼�0x%Xʼ�˽ڵ�ȱʧ������������δ����",pSmartPart->handle));
					continue;
				}
				else if(pSmartPart.pRod->pEnd==NULL)
				{
					pSmartPart.pRod->Log2File()->Log(CXhChar50("��Χ�����˼�0x%X�ն˽ڵ�ȱʧ������������δ����",pSmartPart->handle));
					continue;
				}
				hashExterRods.SetValue(pSmartPart.pRod->handle,EXTERNAL_BLKROD(pSmartPart.pRod));
			}
		}

	}
	//else if(bSaveTaAsModel)	//����Ƕʽ�Ĳ���������ǰ�ļ����Ϊ����ʱ������������������Ҫ������Χ�˼���ڵ㡡wjh-2017.12.12
	//else if(!bSaveTaAsModel)	//����Ĳ������Զ���������Χ�����˼��ڵ㣬����Ҫ���� wjh-2017.12.12
	//��ϣ�������Ϣ����
	long objsN=0;
	buffer.LogPosition();
	buffer.WriteInteger(objsN);
	for(ppPoint=hashPointPtr.GetFirst();ppPoint;ppPoint=hashPointPtr.GetNext())
	{
		buffer.WriteInteger(hashPointPtr.GetCursorKey());
		buffer.WriteInteger(CLS_GEPOINT);
		objsN++;
	}
	for(ppLine=hashLinePtr.GetFirst();ppLine;ppLine=hashLinePtr.GetNext())
	{
		buffer.WriteInteger(hashLinePtr.GetCursorKey());
		buffer.WriteInteger(CLS_GELINE);
		objsN++;
	}
	for(ppPlane=hashPlanePtr.GetFirst();ppPlane;ppPlane=hashPlanePtr.GetNext())
	{
		buffer.WriteInteger(hashPlanePtr.GetCursorKey());
		buffer.WriteInteger(CLS_GEPLANE);
		objsN++;
	}
	for(ppView=hashViewPtr.GetFirst();ppView;ppView=hashViewPtr.GetNext())
	{
		buffer.WriteInteger(hashViewPtr.GetCursorKey());
		buffer.WriteInteger(CLS_DISPLAYVIEW);
		objsN++;
	}
	for(pNode=hashNodePtr.GetFirst();pNode;pNode=hashNodePtr.GetNext())
	{
		buffer.WriteInteger(hashNodePtr.GetCursorKey());
		buffer.WriteInteger(CLS_NODE);
		objsN++;
	}
	for(pPart=hashPartPtr.GetFirst();pPart;pPart=hashPartPtr.GetNext())
	{
		buffer.WriteInteger(hashPartPtr.GetCursorKey());
		buffer.WriteInteger(pPart->GetClassTypeId());
		objsN++;
	}
	buffer.RecallPosition();
	buffer.WriteInteger(objsN);
	buffer.RecallPosition();
	if(	version==0&&doc_type!=3||
		(doc_type==1&&version>=5010800)||	//TMA V5.1.8.0
		(doc_type==2&&version>=2000800)||	//LMA V2.0.8.0	//�����е����9�·ݷ��͵İ汾������11�·ݷ����İ���(˳̩,���ݷɽ������պ�Դ) wjh-2017.11.23
		(doc_type==4&&version>=1030900)||	//LDS V1.3.8.0
		(doc_type==5&&version>=1030800))	//TDA V1.3.8.0
	{
		buffer.WriteInteger(hashExterNodes.GetNodeNum());
		for(EXTERNAL_BLKNODE* pExterNode=hashExterNodes.GetFirst();pExterNode;pExterNode=hashExterNodes.GetNext())
		{
			buffer.WriteInteger(pExterNode->hNode);
			buffer.WritePoint(pExterNode->xPosition);
		}
		buffer.WriteInteger(hashExterRods.GetNodeNum());
		for(EXTERNAL_BLKROD* pExterRod=hashExterRods.GetFirst();pExterRod;pExterRod=hashExterRods.GetNext())
		{
			buffer.WriteInteger(pExterRod->hRod);
			buffer.WriteByte(pExterRod->cbValidProp);
			buffer.WriteInteger(pExterRod->idClsType);
			buffer.WritePoint(pExterRod->xStartPosition);
			buffer.WritePoint(pExterRod->xEndPosition);
			if(pExterRod->cbValidProp.GetBitState(EXTERNAL_BLKROD::PROP_WING_NORM))
			{
				buffer.WritePoint(pExterRod->vxWingNorm);
				buffer.WritePoint(pExterRod->vyWingNorm);
			}
		}
		buffer.WriteInteger(hashExterBolts.GetNodeNum());
		for(EXTERNAL_BLKBOLT* pExterBolt=hashExterBolts.GetFirst();pExterBolt;pExterBolt=hashExterBolts.GetNext())
		{
			buffer.WriteInteger(pExterBolt->hBolt);
			buffer.WriteWord(pExterBolt->wL);
			buffer.WritePoint(pExterBolt->vWorkNorm);
			buffer.WritePoint(pExterBolt->xBasePosition);
		}
	}
	//��ʾ��ͼ
	buffer.WriteInteger(hashViewPtr.GetNodeNum());
	for(ppView=hashViewPtr.GetFirst();ppView;ppView=hashViewPtr.GetNext())
	{
		buffer.WriteInteger((*ppView)->handle);
		(*ppView)->ToBuffer(buffer,version,doc_type);
	}
	//��׼��
	buffer.WriteInteger(hashPointPtr.GetNodeNum());
	for(ppPoint=hashPointPtr.GetFirst();ppPoint;ppPoint=hashPointPtr.GetNext())
	{
		buffer.WriteInteger((*ppPoint)->handle);
		(*ppPoint)->ToBuffer(buffer,version,doc_type);
	}
	//��׼��
	buffer.WriteInteger(hashLinePtr.GetNodeNum());
	for(ppLine=hashLinePtr.GetFirst();ppLine;ppLine=hashLinePtr.GetNext())
	{
		buffer.WriteInteger((*ppLine)->handle);
		(*ppLine)->ToBuffer(buffer,version,doc_type);
	}
	//��׼ƽ��
	buffer.WriteInteger(hashPlanePtr.GetNodeNum());
	for(ppPlane=hashPlanePtr.GetFirst();ppPlane;ppPlane=hashPlanePtr.GetNext())
	{
		buffer.WriteInteger((*ppPlane)->handle);
		(*ppPlane)->ToBuffer(buffer,version,doc_type);
	}
	//�ڵ�
	buffer.WriteInteger(hashNodePtr.GetNodeNum());
	for(pNode=hashNodePtr.GetFirst();pNode;pNode=hashNodePtr.GetNext())
	{
		buffer.WriteInteger(pNode->handle);
		pNode->ToBuffer(buffer,version,doc_type);
	}
	//�����͸��Ƴɵ�������������Ǹ֡��ְ塢�ֹܡ��۸֡���˨��
	buffer.WriteInteger(hashPartPtr.GetNodeNum());
	for(pPart=hashPartPtr.GetFirst();pPart;pPart=hashPartPtr.GetNext())
	{
		buffer.WriteInteger(pPart->handle);
		pPart->ToBuffer(buffer,version,doc_type);
	}
	//�����������ϵ	wht 13-01-06
	buffer.WriteInteger(Acs.GetNodeNum());
	for (CParaCS *pCS=Acs.GetFirst();pCS;pCS=Acs.GetNext())
	{
		buffer.WriteInteger(pCS->id);
		pCS->ToBuffer(buffer,version,doc_type);
	}
	FILE* fp=fopen(blk_file,"wb");
	if(fp==NULL)
		return false;
	fwrite(&version,4,1,fp);	//version,version����Ϊ0������ֵ,�����޷����ݰ汾�Ŷ�ȡ������
	fwrite(&doc_type,4,1,fp);	//doc_type
	long buf_size=buffer.GetLength();
	if( (doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
	{
		char ciEncryptMode=1;
		if( (doc_type!=3&&version==0)||
			(doc_type==1&&version>=5000700)||	//TMA(V5,0,7,0)
			(doc_type==2&&version>=2000800)||	//LMA(V2,0,8,0)
			(doc_type==4&&version>=1030800)||	//LDS(V1,3,8,0)
			(doc_type==5&&version>=1030800))	//TDA(V1,3,8,0)
			ciEncryptMode=2;	//��ָ�Ƶļ���ģʽ
		EncryptBuffer(buffer,ciEncryptMode);
		buf_size=buffer.GetLength();
		fwrite(&buf_size,4,1,fp);
	}
	fwrite(buffer.GetBufferPtr(),buf_size,1,fp);
	fclose(fp);
	return true;
}
bool CBlockModel::ImportFromBlkFile(char* blk_file)
{
	FILE* fp=fopen(blk_file,"rb");
	if(fp==NULL)
		return false;
	fseek(fp,0,SEEK_END);
	long buf_size=ftell(fp);
	fseek(fp,0,SEEK_SET);
	long version=0;
	int doc_type=0;
	fread(&version,4,1,fp);
	fread(&doc_type,4,1,fp);
	CBuffer buffer;
	if( (doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
		fread(&buf_size,4,1,fp);	//�������泤��
	buffer.Write(NULL,buf_size);
	fread(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);
	m_bEmbeded=FALSE;
	if( (doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
	{
		char ciEncryptMode=1;
		if( (doc_type!=3&&version==0)||
			(doc_type==1&&version>=5000700)||	//TMA(V5,0,7,0)
			(doc_type==2&&version>=2000800)||	//LMA(V2,0,8,0)
			(doc_type==4&&version>=1030800)||	//LDS(V1,3,8,0)
			(doc_type==5&&version>=1030800))	//TDA(V1,3,8,0)
			ciEncryptMode=2;	//��ָ�Ƶļ���ģʽ
		DecryptBuffer(buffer,ciEncryptMode,0);
	}
	buffer.SeekToBegin();
	//
	buffer.ReadPoint(lcs.origin);
	buffer.ReadPoint(lcs.axis_x);
	buffer.ReadPoint(lcs.axis_y);
	buffer.ReadPoint(lcs.axis_z);
	des_lcs.FromBuffer(buffer,version,doc_type);
	buffer.ReadString(name,50);	//��������������;
	long i,n,h,cls_typeid;
	//��ϣ�������Ϣ����
	buffer.ReadInteger(&n);
	CHashTable<CLDSObject*> hashObjs;
	hashObjs.CreateHashTable(n);
	BOOL bHasRepeatHandle=FALSE;
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadInteger(&cls_typeid);
		CLDSObject *pObj=NULL;
		if(hashObjs.GetValueAt(h,pObj))
		{
			if(cls_typeid==pObj->GetClassTypeId())
				continue;	//�Ѵ��ڴ˹������������
			else
			{	//���ֶ������ظ�,Ӧ����ļ�����ԭ��
				bHasRepeatHandle=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Find repeated object handle,communicate with supplier of the software please!";
#else
				throw "���ֶ������ظ������������Ӧ����ϵ��";
#endif
			}
		}
 		if(cls_typeid==CLS_GEPOINT)
		{
			CLDSPoint* pPoint=Point.append(FALSE);
			pPoint->handle=h;
			Point.UpdateHashtable(h,pPoint);
			hashObjs.SetValueAt(h,pPoint);
		}
		else if(cls_typeid==CLS_GELINE)
		{
			CLDSLine* pLine=Line.append(FALSE);
			pLine->handle=h;
			Line.UpdateHashtable(h,pLine);
			hashObjs.SetValueAt(h,pLine);
		}
		else if(cls_typeid==CLS_GEPLANE)
		{
			CLDSPlane* pPlane=Plane.append(FALSE);
			pPlane->handle=h;
			Plane.UpdateHashtable(h,pPlane);
			hashObjs.SetValueAt(h,pPlane);
		}
		else if(cls_typeid==CLS_DISPLAYVIEW)
		{
			CDisplayView* pView=View.append(FALSE);
			pView->handle=h;
			View.UpdateHashtable(h,pView);
			hashObjs.SetValueAt(h,pView);
		}
		else if(cls_typeid==CLS_NODE)
		{
			CLDSNode* pNode=Node.append(FALSE);
			pNode->handle=h;
			//Node.UpdateHashtable(h,pNode);
			hashObjs.SetValueAt(h,pNode);
		}
		else	//CLDSPart���͹���
		{
			CLDSPart* pPart=Parts.append(cls_typeid,FALSE);
			if(pPart)
			{
				pPart->handle=h;
				//Parts.UpdateHashtable(h,pPart);
				hashObjs.SetValueAt(h,pPart);
			}
		}
	}
	Node.RebuildHashtable();
	Parts.RebuildHashtable();
	if(  version==0&&doc_type!=3||
		(doc_type==1&&version>=5010800)||	//TMA V5.1.8.0
		(doc_type==2&&version>=2000800)||	//LMA V2.0.8.0	//�����е����9�·ݷ��͵İ汾������11�·ݷ����İ���(˳̩,���ݷɽ������պ�Դ) wjh-2017.11.23
		(doc_type==4&&version>=1030900)||	//LDS V1.3.8.0
		(doc_type==5&&version>=1030800))	//TDA V1.3.8.0
	{
		buffer.ReadInteger(&n);
		hashExterNodes.Empty();
		for(i=0;i<n;i++)
		{
			buffer.ReadInteger(&h);
			EXTERNAL_BLKNODE* pExterNode=hashExterNodes.Add(h);
			buffer.ReadPoint(pExterNode->xPosition);
		}
		buffer.ReadInteger(&n);
		hashExterRods.Empty();
		for(i=0;i<n;i++)
		{
			buffer.ReadInteger(&h);
			EXTERNAL_BLKROD* pExterRod=hashExterRods.Add(h);
			buffer.ReadByte((BYTE*)&pExterRod->cbValidProp);
			buffer.ReadInteger(&pExterRod->idClsType);
			buffer.ReadPoint(pExterRod->xStartPosition);
			buffer.ReadPoint(pExterRod->xEndPosition);
			if(pExterRod->cbValidProp.GetBitState(EXTERNAL_BLKROD::PROP_WING_NORM))
			{
				buffer.ReadPoint(pExterRod->vxWingNorm);
				buffer.ReadPoint(pExterRod->vyWingNorm);
			}
		}
		buffer.ReadInteger(&n);
		hashExterBolts.Empty();
		for(i=0;i<n;i++)
		{
			buffer.ReadInteger(&h);
			EXTERNAL_BLKBOLT* pExterBolt=hashExterBolts.Add(h);
			buffer.ReadWord(&pExterBolt->wL);
			buffer.ReadPoint(pExterBolt->vWorkNorm);
			buffer.ReadPoint(pExterBolt->xBasePosition);
		}
	}
	//��ʾ��ͼ
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CDisplayView* pView=View.FromHandle(h);
		pView->FromBuffer(buffer,version,doc_type);
	}
	//��׼��
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CLDSPoint *pPoint=Point.FromHandle(h);
		pPoint->FromBuffer(buffer,version,doc_type);
	}
	//��׼��
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CLDSLine *pLine=Line.FromHandle(h);
		pLine->FromBuffer(buffer,version,doc_type);
	}
	//��׼ƽ��
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CLDSPlane* pPlane=Plane.FromHandle(h);
		pPlane->FromBuffer(buffer,version,doc_type);
	}
	//�ڵ�
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CLDSNode* pNode=Node.FromHandle(h);
		pNode->FromBuffer(buffer,version,doc_type);
	}
	//�����͸��Ƴɵ�������������Ǹ֡��ְ塢�ֹܡ��۸֡���˨��
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CLDSPart* pPart=Parts.FromHandle(h);
		pPart->FromBuffer(buffer,version,doc_type);
	}
	//��ȡ����������ϵ wht 13-01-06
	n=0;
	buffer.ReadInteger(&n);
	Acs.Empty();
	for (i=0;i<n;i++)
	{
		int id=0;
		buffer.ReadInteger(&id);
		CParaCS *pCS = Acs.Add(id);//		pCS->id = id;
		pCS->FromBuffer(buffer,version,doc_type);
	}
	return true;
}
static f3dPoint MatrixdTransform(f3dPoint pt,double matrixdArr[4][4])
{
	double coordArr[4]={pt.x,pt.y,pt.z,1},retCoordArr[4]={0,0,0,0};
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
			retCoordArr[i]+=coordArr[j]*matrixdArr[i][j];
	}
	return f3dPoint(retCoordArr);
}
static UCS_STRU GetUcsFromMatrixd(double matrixdArr[4][4])
{	//������
	UCS_STRU ucs;
	ucs.axis_x.Set(matrixdArr[0][0],matrixdArr[0][1],matrixdArr[0][2]);
	ucs.axis_y.Set(matrixdArr[1][0],matrixdArr[1][1],matrixdArr[1][2]);
	ucs.axis_z.Set(matrixdArr[2][0],matrixdArr[2][1],matrixdArr[2][2]);
	ucs.origin.Set(matrixdArr[0][3],matrixdArr[1][3],matrixdArr[2][3]);
	return ucs;
}
void CreateLdsPartsByXmlBlockRec(IModel *pModel,CXmlBlockRecord *pBlockRec,double matrixdArr[4][4])
{
	//���´���ԭ������������������߼���Ե�Ӵ���άģ�͵ģ�Ŀǰ��Ϊ����淶��ʱ���ⲿ�ֹ�����δ������ע�͵� wjh-2019.4.3
	/*for(CXmlEntity *pEnt=pBlockRec->m_arEntity.GetFirst();pEnt;pEnt=pBlockRec->m_arEntity.GetNext())
	{
		if(pEnt->m_cTypeId!=CXmlEntity::TYPE_SOLID)
			continue;
		CXmlSolid *pSolid=(CXmlSolid*)pEnt;
		CLDSExternalPart *pPart=(CLDSExternalPart*)pModel->AppendPart(CLS_EXTERNALPART);
		//pPart->ucs=GetUcsFromMatrixd(matrixdArr);
		pPart->crMaterial=pSolid->m_nRGBColor;
		for(f3dPoint *pPt=pSolid->m_arPts.GetFirst();pPt;pPt=pSolid->m_arPts.GetNext())
			pPart->Vertexs.append(MatrixdTransform(*pPt,matrixdArr));
		for(f3dPoint *pPt=pSolid->m_arPtNormal.GetFirst();pPt;pPt=pSolid->m_arPtNormal.GetNext())
			pPart->VertexNormals.append(MatrixdTransform(*pPt,matrixdArr));
		for(ATOM_LIST<int> *pList=pSolid->m_arIndexs.GetFirst();pList;pList=pSolid->m_arIndexs.GetNext())
		{
			ATOM_LIST<int> *pNewList=pPart->FaceVertexIndexs.append();
			for(int *pIndex=pList->GetFirst();pIndex;pIndex=pList->GetNext())
				pNewList->append(*pIndex);
		}
	}*/
}
bool CBlockModel::ImportFromXMLFile(const char* xml_file)
{
#ifdef __LDS_CONTEXT_
	CXmlModel xmlModel;
	if(!xmlModel.LoadFromXmlFile(xml_file))
		return false;
	//��������ϵ
	lcs.axis_x.Set(1,0,0);
	lcs.axis_y.Set(0,0,1);
	lcs.axis_z.Set(0,-1,0);
	CParaCS::PARAMETRIC_INFO param;
	param.m_cDefStyle=0;
	param.m_eDatumPlane=0;	//0.X-Y;1.Y-X;2.Y-Z;3.Z-Y;4.Z-X;5.X-Z 
	param.desOrg.datum_pos_style=0;
	param.desOrg.datum_pos=lcs.origin;
	param.desAxisX.norm_style=0;
	param.desAxisX.vector=lcs.axis_x;
	param.desAxisY.norm_style=0;
	param.desAxisY.vector=lcs.axis_y;
	param.desAxisZ.norm_style=0;
	param.desAxisZ.vector=lcs.axis_z;
	des_lcs.SetParamInfo(param);
	UpdateLCS();
	//���ÿ�����
	strcpy(name,xmlModel.m_sName);
	//��ʼ���鹹���б�
	for(CXmlBlockRecord *pRec=xmlModel.XmlBlockTable.GetFirst();pRec;pRec=xmlModel.XmlBlockTable.GetNext())
	{
		if(!pRec->IsModelSpace())
			continue;
		for(CXmlEntity *pEnt=pRec->m_arEntity.GetFirst();pEnt;pEnt=pRec->m_arEntity.GetNext())
		{
			if(!pEnt->IsBlockRecRef())
				continue;
			CXmlBlockRecRef *pBlockRef=(CXmlBlockRecRef*)pEnt;
			CXmlBlockRecord *pOrgRec=xmlModel.XmlBlockTable.GetValue(pBlockRef->m_hBlockRec);
			if(pOrgRec)
				CreateLdsPartsByXmlBlockRec(this,pOrgRec,pBlockRef->m_arrMatrixd);
		}
	}
	return true;
#else
	return false;
#endif
}
long CBlockModel::GetMinValidObjHandle(bool bIncViewObj/*=false*/)//�����С���õĶ�����;
{
	long hMinUseHandle=handle;
	for(CLDSPoint *pPoint=EnumPointFirst();pPoint;pPoint=EnumPointNext())
		hMinUseHandle = min(hMinUseHandle,pPoint->handle);
	for(CLDSLine *pLine=EnumLineFirst();pLine;pLine=EnumLineNext())
		hMinUseHandle = min(hMinUseHandle,pLine->handle);
	for(CLDSPlane *pPlane=EnumPlaneFirst();pPlane;pPlane=EnumPlaneNext())
		hMinUseHandle = min(hMinUseHandle,pPlane->handle);
	for(CDisplayView* pView=EnumViewFirst();pView;pView=EnumViewNext())
		hMinUseHandle = min(hMinUseHandle,pView->handle);
	for(CLDSNode *pNode=EnumNodeFirst();pNode;pNode=EnumNodeNext())
		hMinUseHandle = min(hMinUseHandle,pNode->handle);
	for(CLDSPart *pPart=EnumPartFirst();pPart;pPart=EnumPartNext())
		hMinUseHandle = min(hMinUseHandle,pPart->handle);
	for(EXTERNAL_BLKNODE* pExterNode=hashExterNodes.GetFirst();pExterNode;pExterNode=hashExterNodes.GetNext())
		hMinUseHandle = min(hMinUseHandle,pExterNode->hNode);
	for(EXTERNAL_BLKROD* pExterRod=hashExterRods.GetFirst();pExterRod;pExterRod=hashExterRods.GetNext())
		hMinUseHandle = min(hMinUseHandle,pExterRod->hRod);
	for(EXTERNAL_BLKBOLT* pExterBolt=hashExterBolts.GetFirst();pExterBolt;pExterBolt=hashExterBolts.GetNext())
		hMinUseHandle = min(hMinUseHandle,pExterBolt->hBolt);
	if(bIncViewObj)
	{
		for(CDisplayView* pView=EnumViewFirst();pView;pView=EnumViewNext())
			hMinUseHandle = min(hMinUseHandle,pView->handle);
	}
	return hMinUseHandle;
}
long CBlockModel::GetNewHandle()
{
	if(GetNewHandleHookFunc!=NULL)
		return GetNewHandleHookFunc();
	long h=m_hMinIdleHandle;
	if(h<0X20)
		m_hMinIdleHandle=0X20;
	h=m_hMinIdleHandle++;
	return h;
}
DWORD CBlockModel::ShrinkObjHandle(DWORD hStartShrink/*=0x20*/)	//���������
{
	return m_hMinIdleHandle;
}
CLDSLinePart *CBlockModel::FromRodHandle(long hRod)
{
	CLDSPart* pPart=Parts.FromHandle(hRod);
	if(pPart&&pPart->IsLinePart())
		return (CLDSLinePart*)pPart;
	else
		return NULL;
}
//Լ����صĺ���
CONSTRAINT* CBlockModel::AppendConstraint(CONSTRAINT* pCopyFrom/*=NULL*/)
{
	CONSTRAINT* pConstraint=hashConstraints.Add(0);
	pConstraint->CopyProperty(pCopyFrom);
	return pConstraint;
}
CONSTRAINT* CBlockModel::GetConstraint(long constraint_id)
{
	return hashConstraints.GetValue(constraint_id);
}
bool CBlockModel::RemoveConstraint(long constraint_id,long hFromMasterObj/*=0*/)
{
	for(CONSTRAINT* pConstraint=hashConstraints.GetFirst();pConstraint;pConstraint=hashConstraints.GetNext())
	{
		if(pConstraint->Id!=constraint_id)	//������ɾ���ڵ�
			continue;
		if(pConstraint->hMasterObj!=hFromMasterObj&&hFromMasterObj>0x20)
			return false;
		else
			return hashConstraints.DeleteCursor()!=FALSE;
	}
	return false;
}
CONSTRAINT* CBlockModel::EnumConstraintFirst(BOOL bIterDelete/*=FALSE*/)
{
	return hashConstraints.GetFirst(bIterDelete);
}
CONSTRAINT* CBlockModel::EnumConstraintNext(BOOL bIterDelete/*=FALSE*/)
{
	return hashConstraints.GetNext(bIterDelete);
}
void CBlockModel::ClearConstraintSyncStates()
{
	for(CONSTRAINT* pConstraint=hashConstraints.GetFirst();pConstraint;pConstraint=hashConstraints.GetNext())
		pConstraint->ClearSynchronize();
}
long CBlockModel::GetObjNum(int CLASS_TYPEID)
{
	switch(CLASS_TYPEID)
	{
	case CLS_NODE:
		return Node.GetNodeNum();
	case CLS_GEPOINT:
		return Point.GetNodeNum();
	case CLS_GELINE:
		return Line.GetNodeNum();
	case CLS_GEPLANE:
		return Plane.GetNodeNum();
	case CLS_PART:
		return Parts.GetNodeNum();
	case CLS_DISPLAYVIEW:
		return View.GetNodeNum();
	default:
		return 0;
	}
}
//��ȡ����ģ�Ϳռ���ֱ�ӹ���ʵ�ʴ��ڵĹ������飬filterShadowPart=true��ʾֻ����ĸ��������Ӱ�乹�������S��
bool CBlockModel::GetRealityPartArr(DYN_ARRAY<CLDSBolt*>& arrBoltPtr,DYN_ARRAY<CLDSPartPtr>& arrPartPtr,void *pPolyPartList,bool filterShadowPart)
{
	CLDSPart* pPart;
	PARTSET partset,boltset;
	for(pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		if(pPart->m_bVirtualPart)
			continue;	//���⸨����������ʾ
		if(filterShadowPart&&pPart->IsShadowObject())
			continue;	//�S��Ӱ�乹��
		if(pPart->GetClassTypeId()==CLS_BOLT)
			boltset.append(pPart);
		else if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE)
			partset.append(pPart);
		else if(pPart->GetClassTypeId()==CLS_LINETUBE||
			pPart->GetClassTypeId()==CLS_LINEFLAT||pPart->GetClassTypeId()==CLS_LINESLOT)
			partset.append(pPart);
		else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			if(pPolyPartList==NULL||(!((CLDSLineAngle*)pPart)->bHuoQuStart&&!((CLDSLineAngle*)pPart)->bHuoQuEnd))
				partset.append(pPart);
		}
	}
	if(pPolyPartList)
	{
		for(CLDSPolyPart *pPolyPart=((POLYPARTLIST*)pPolyPartList)->GetFirst();pPolyPart;pPolyPart=((POLYPARTLIST*)pPolyPartList)->GetNext())
		{
			if(pPolyPart->m_bVirtualPart)
				continue;	//���⸨����������ʾ
			if(filterShadowPart&&pPolyPart->IsShadowObject())
				continue;	//�S��Ӱ�乹��
			partset.append(pPolyPart);
		}
	}
	int i=0;
	arrPartPtr.Resize(partset.GetNodeNum());
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext(),i++)
		arrPartPtr[i]=pPart;
	i=0;
	arrBoltPtr.Resize(boltset.GetNodeNum());
	for(pPart=boltset.GetFirst();pPart;pPart=boltset.GetNext(),i++)
		arrBoltPtr[i]=(CLDSBolt*)pPart;
	return (partset.GetNodeNum()+boltset.GetNodeNum())>0;
}
int CBlockModel::RetrievePolyPartList(POLYPARTLIST &list)
{
	return CTower::RetrievePolyPartList(this,list);
}
CLDSObject *CBlockModel::FromHandle(long h,BOOL bDbObjOnly/*=FALSE*/)
{
	CLDSObject *pObj=FromNodeHandle(h);
	if(pObj!=NULL)
		return pObj;
	pObj=FromPartHandle(h);
	if(pObj!=NULL)
		return pObj;
	pObj=FromViewHandle(h);
	if(pObj!=NULL)
		return pObj;
	if(!bDbObjOnly)
	{
		pObj=FromPointHandle(h);
		if(pObj!=NULL)
			return pObj;
		pObj=FromPlaneHandle(h);
		if(pObj!=NULL)
			return pObj;
		pObj=FromLineHandle(h);
		if(pObj!=NULL)
			return pObj;
	}
	return NULL;
}
/* ��û�����ͨ�����Ӻ���������ݶ����Ĳ�ѯ���⣬�ݲ����� wjh-2014.5.26
CLDSNode *CBlockModel::AppendNode(BOOL bGetNewHandle)
{
	if(AppendNodeHookFunc!=NULL)
		return AppendNodeHookFunc(bGetNewHandle);
	return Node.append(bGetNewHandle);
}
CLDSPart *CBlockModel::AppendPart(int CLASS_TYPEID,BOOL bGetNewHandle)
{
	if(AppendPartHookFunc!=NULL)
		return AppendPartHookFunc(CLASS_TYPEID,bGetNewHandle);
	return Parts.append(CLASS_TYPEID,bGetNewHandle);
}
*/
//TODO: ��ʱ��֧�ֲ������ھ���Ӱ�乹�������wjh 2013-1-17
CParaCS *CBlockModel::AppendAcs()
{
	return NULL;
	/*CObjNoGroup *pNoGroup=NoManager.FromGroupID(PARAMETRIC_CS_GROUP);
	long id=pNoGroup->EnumIdleNo();
	pNoGroup->SetNoState(id);
	CParaCS *pAssembleCS=Acs.Add(id);
	pAssembleCS->id=id;
	return pAssembleCS;*/
}
BOOL CBlockModel::DeleteAcs(long hAcs)
{
	return FALSE;
	/*CObjNoGroup *pNoGroup=NoManager.FromGroupID(PARAMETRIC_CS_GROUP);
	pNoGroup->SetNoState(hAcs);
	return Acs.DeleteNode(hAcs);*/
}
CParaCS *CBlockModel::FromAcsHandle(long hAcs)
{
	return Acs.GetValue(hAcs);
}
#ifdef __COMMON_PART_INC_
//�õ��麸����������ʱ֧�����͹��� wht 10-10-22
static CHashSet<CLDSPart*> hashLocalDeadlockLoop;
int CBlockModel::GetPartGroupPartSet(long hPartGroup,PARTSET &partset,int CLASS_TYPEID/*=0*/,bool bClearLock/*=true*/,
									 long* phExcludePartBranchArr/*=NULL*/,int excludeCount/*=1*/)
{
	PARTSET set;	//��ʱ��
	if(hPartGroup==0)
		return 0;
	Parts.push_stack();
	if(bClearLock)
		hashLocalDeadlockLoop.Empty();
	CLDSPart* pRootPart=hashLocalDeadlockLoop.GetValue(hPartGroup);
	if(pRootPart!=NULL)
	{
		pRootPart->Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X�������Ӹ������趨��������ѭ��",hPartGroup);
		return 0;
	}
	else
		hashLocalDeadlockLoop.SetValue(hPartGroup,FromPartHandle(hPartGroup));
	for(CLDSPart *pPart=Parts.GetFirst(CLASS_TYPEID);pPart;pPart=Parts.GetNext(CLASS_TYPEID))
	{
		if(pPart->handle==hPartGroup)
			continue;
		bool excluded=false;
		for(int k=0;phExcludePartBranchArr!=NULL&&k<excludeCount;k++)
		{
			if(phExcludePartBranchArr[k]!=pPart->handle)
				continue;
			excluded=true;
			break;
		}
		if(excluded)
			continue;	//�÷�֧�ѱ��ų���ͳ��֮��
		else if(pPart->m_hPartWeldParent==hPartGroup)
		{
			partset.append(pPart);
			set.Empty();
			if(GetPartGroupPartSet(pPart->handle,set,CLASS_TYPEID,false)>0)
			{
				for(CLDSPart *pTmPart=set.GetFirst();pTmPart;pTmPart=set.GetNext())
					partset.append(pTmPart);
			}
		}
	}
	Parts.pop_stack();
	return partset.GetNodeNum();
}
#endif
//��������ѡ��Ҫ��ʾ�ļ���
BOOL CBlockModel::SelectDisplaySet(NODESET& DispNodeSet,PARTSET& DispPartSet,CDisplayView *pView/*=NULL*/)
{
	DispNodeSet.Empty();
	DispPartSet.Empty();
	CLDSNode* pNode;
	CLDSPart *pPart;
	if(pView)
	{	
		for(CViewPart *pViewPart=pView->GetFirstViewPart(); pViewPart; pViewPart=pView->GetNextViewPart())
		{
			if(pViewPart->idClassType==CLS_NODE)
			{
				pNode=pView->BelongModel()->FromNodeHandle(pViewPart->handle);
				if(pNode)
					DispNodeSet.append(pNode);
			}
			else if(pViewPart->idClassType!=CLS_NODE)
			{
				pPart = pView->BelongModel()->FromPartHandle(pViewPart->handle);
				if(pPart)
					DispPartSet.append(pPart);
			}
		}
	}
	else 
	{
		if(IsEmbeded())
		{
			
		}
		else 
		{
			BOOL pushed=PushPartStack();
			for(pPart=EnumPartFirst();pPart;pPart=EnumPartNext())
			{
				pPart->is_visible=TRUE;
				DispPartSet.append(pPart);
			}
			PopPartStack(pushed);
			for(pNode=EnumNodeFirst();pNode;pNode=EnumNodeNext())
				DispNodeSet.append(pNode);
		}
	}
	return TRUE;
}
void CBlockModel::UpdateLCS()	//����װ��λ��
{
	if(des_lcs.UpdateACS(this))
		lcs=des_lcs.GetACS();
}
void CBlockModel::UpdatePos()	//����ԭ��λ��
{
	CParaCS::PARAMETRIC_INFO param=des_lcs.GetParamInfo();
	BOOL bRetCode = param.desOrg.UpdatePos(this);
	if(bRetCode)
		lcs.origin=param.desOrg.Position();
}
void CBlockModel::Empty()
{
	View.Empty();
	Point.Empty();
	Line.Empty();
	Plane.Empty();
	Node.Empty();
	Parts.Empty();
}
//���벿��װ�䶨λ����ϵ���Ӧ��װ������ϵת�����벿��ԭʼ�������Ӧ��װ������ϵ
UCS_STRU CBlockModel::TransToACS(const UCS_STRU& cs)
{
	UCS_STRU acs=lcs;
	ReverseUCS(&acs);
	coord_trans(acs.origin,cs,TRUE,TRUE);
	vector_trans(acs.axis_x,cs,TRUE,TRUE);
	vector_trans(acs.axis_y,cs,TRUE,TRUE);
	vector_trans(acs.axis_z,cs,TRUE,TRUE);
	return acs;
}

//
int CBlockModel::GetPropValueStr(long id,char* valueStr)
{
	char sText[100]="";
	if(GetPropID("basicInfo")==0)
		strcpy(sText,"");
	else if(GetPropID("handle")==id)
		sprintf(sText,"0X%X",handle);
	else if(GetPropID("name")==id)
		sprintf(sText,"%s",name);
	else if(GetPropID("m_bEmbeded")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bEmbeded==TRUE)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
		if(m_bEmbeded==TRUE)
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("des_lcs.m_eDatumPlane")==id)
	{
		CParaCS::PARAMETRIC_INFO param=des_lcs.GetParamInfo();
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
	else if(GetPropID("lcs.origin.x")==id)
	{
		sprintf(sText,"%f",lcs.origin.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("lcs.origin.y")==id)
	{
		sprintf(sText,"%f",lcs.origin.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("lcs.origin.z")==id)
	{
		sprintf(sText,"%f",lcs.origin.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("lcs.axis_x")==id)
		strcpy(sText,des_lcs.GetAxisDescription('X'));
	else if(GetPropID("lcs.axis_x.x")==id)
	{
		sprintf(sText,"%f",lcs.axis_x.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("lcs.axis_x.y")==id)
	{
		sprintf(sText,"%f",lcs.axis_x.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("lcs.axis_x.z")==id)
	{
		sprintf(sText,"%f",lcs.axis_x.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("lcs.axis_y")==id)
		strcpy(sText,des_lcs.GetAxisDescription('Y'));
	else if(GetPropID("lcs.axis_y.x")==id)
	{
		sprintf(sText,"%f",lcs.axis_y.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("lcs.axis_y.y")==id)
	{
		sprintf(sText,"%f",lcs.axis_y.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("lcs.axis_y.z")==id)
	{
		sprintf(sText,"%f",lcs.axis_y.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("lcs.axis_z")==id)
		strcpy(sText,des_lcs.GetAxisDescription('Z'));
	else if(GetPropID("lcs.axis_z.x")==id)
	{
		sprintf(sText,"%f",lcs.axis_z.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("lcs.axis_z.y")==id)
	{
		sprintf(sText,"%f",lcs.axis_z.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("lcs.axis_z.z")==id)
	{
		sprintf(sText,"%f",lcs.axis_z.z);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
long CBlockModel::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem =CBlockModel::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		/*CString prompt="Did not find a property ID named\"";
		prompt+=propStr;
		prompt+="\"!";
		AfxMessageBox(prompt);*/
#else
		/*CString prompt="û�ҵ���Ϊ\"";
		prompt+=propStr;
		prompt+="\"������ID��";
		AfxMessageBox(prompt);*/
#endif
		return 0;
	}
}

BOOL CBlockModel::IsPropEqual(HASHOBJSET &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CBlockModel *pBlock=NULL, *pFirstBlock = (CBlockModel*)selectObjs.GetFirst();
	pFirstBlock->GetPropValueStr(idProp,valueStr);
	for(pBlock=(CBlockModel*)selectObjs.GetNext();pBlock;pBlock=(CBlockModel*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pBlock->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CBlockModel::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CBlockModel::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}

BOOL CBlockModel::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CBlockModel::propStatusHashtable.SetValueAt(id,dwStatus);
}

//��鲿���нڵ��������
BOOL CBlockModel::CheckNodeIntegrality(IModel *pModel,NODESET& nodeSet)
{
	nodeSet.Empty();
	if(IsEmbeded())
	{
		PARTSET partSet;
		for(CLDSPart *pPart=pModel->EnumPartFirst();pPart;pPart=pModel->EnumPartNext())
		{
			if(pPart->m_hBlock!=handle)
				continue;
			if(!pPart->IsLinePart())
				continue;
			CLDSLinePart *pRod=(CLDSLinePart*)pPart;
			if(pRod->pStart!=NULL&&pRod->pStart->m_hBlock!=handle)
				nodeSet.append(pRod->pStart);
			if(pRod->pEnd!=NULL&&pRod->pEnd->m_hBlock!=handle)
				nodeSet.append(pRod->pEnd);
		}
	}
	else 
	{
		for(CLDSLinePart* pRod=Parts.GetFirstLinePart();pRod;pRod=Parts.GetNextLinePart())
		{
			if(pRod->pStart!=NULL&&Node.FromHandle(pRod->pStart->handle)==NULL)
				nodeSet.append(pRod->pStart);
			if(pRod->pEnd!=NULL&&Node.FromHandle(pRod->pEnd->handle)==NULL)
				nodeSet.append(pRod->pEnd);
		}
	}
	if(nodeSet.GetNodeNum()>0)
		return FALSE;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//CBlockReference
//////////////////////////////////////////////////////////////////////////////
CBlockReference::CBlockReference()
{
	SetLayer("SPQ");
	strcpy(descript,"");
	pSolidBody=NULL;
	m_hBlock=0;
	m_pInternalBlock=0;
	m_bExploded=FALSE;
	m_bDisplayLineFrame=false;
	is_data_modified=TRUE;
	LoadDefaultUCS(&acs);
	CParaCS::PARAMETRIC_INFO param=des_acs.GetParamInfo();
	param.m_cDefStyle=0;
	param.desOrg.datum_pos_style=0;
	param.desOrg.datum_pos=acs.origin;
	param.desAxisX.norm_style=0;
	param.desAxisX.vector=acs.axis_x;
	param.desAxisY.norm_style=0;
	param.desAxisY.vector=acs.axis_y;
	param.desAxisZ.norm_style=0;
	param.desAxisZ.vector=acs.axis_z;
	des_acs.SetParamInfo(param);
	m_nClassTypeId=CLS_BLOCKREF;
}
CBlockReference::~CBlockReference()
{
	if(pSolidBody)
		delete pSolidBody;
}
BOOL CBlockReference::Create3dSolidModel(double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	if(m_bExploded||(pSolidBody!=NULL&& !IsModified()))
	{
		if(m_bExploded&&pSolidBody)
			pSolidBody->Empty();
		return FALSE;
	}
	CBlockModel *pBlock=((CTower*)m_pModel)->Block.FromHandle(m_hBlock);
	if(pBlock==NULL)
		return FALSE;
	if(pSolidBody)
		pSolidBody->Empty();
	else
		pSolidBody=new CSolidBody();
	UCS_STRU cs=pBlock->TransToACS(acs);
	SetModified(FALSE);
	PARTSET partSet;
	if(pBlock->IsEmbeded())
	{	//Ƕ��ʽ����
		for(CLDSPart *pPart=m_pModel->EnumPartFirst();pPart;pPart=m_pModel->EnumPartNext())
		{
			if(pPart->m_hBlock==pBlock->handle)
				partSet.append(pPart);
		}
	}
	else 
	{	//��Ƕ��ʽ����
		for(CLDSPart* pPart=pBlock->Parts.GetFirst();pPart;pPart=pBlock->Parts.GetNext())
			partSet.append(pPart);
	}
	fBody body;
	//CSolidBody::m_pHookBody=&body;
	CHashList<f3dPoint*> hashVertexPtr;
	int i=0;
	CLDSPart* pPart;
	DWORD increment=0;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext(),i++)
	{
		pPart->Create3dSolidModel(FALSE,scale_of_user2scr,sample_len,smoothness);
		increment+=pPart->pSolidBody->BufferLength();
	}
	CSolidBodyBuffer solidbuf;
	if(solidbuf.AllocateMemory(increment))//increment<20000000)
	{
		CXhSimpleList<CSolidBody*>listBodies;
		for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext(),i++)
		{
			if(pPart->pSolidBody!=NULL)
				listBodies.AttachObject(pPart->pSolidBody);
		}
		pSolidBody->MergeListBodyBuffer(listBodies);
		UCS_STRU stdcs;
		LoadDefaultUCS(&stdcs);
		pSolidBody->TransACS(stdcs,cs);
	}
	else	//�����ر����ڴ�����п��ܻ�ʧ�ܣ�ʧ��ʱ�����鲿������ wjh-2016.3.7
	{
		CLDSObject::Log2File()->Log("����ʵ��ϲ�ʱ���ڴ����ʧ��!");
		m_bExploded=TRUE;
	}
	return TRUE;
}

void CBlockReference::UpdateAcs()	//����װ��λ��
{
	if(!des_acs.UpdateACS(m_pModel))
		return;
	acs=des_acs.GetACS();
	SetModified(TRUE);
}
void CBlockReference::UpdatePos()	//����ԭ��λ��
{
	CParaCS::PARAMETRIC_INFO param=des_acs.GetParamInfo();
	BOOL bRetCode = param.desOrg.UpdatePos(m_pModel);
	if(bRetCode)
		acs.origin=param.desOrg.Position();
	SetModified(TRUE);
}
GEPOINT CBlockReference::ShadowPositionBlkToModel(const double* blk_position)
{
	if(m_pInternalBlock==NULL||m_pInternalBlock->handle!=m_hBlock)
		m_pInternalBlock=((CTower*)m_pModel)->Block.FromHandle(m_hBlock);
	acs=des_acs.GetACS();
	GECS cs=m_pInternalBlock->TransToACS(acs);
	return cs.TransPFromCS(blk_position);
}
GEPOINT CBlockReference::ShadowPositionModelToBlk(const double* model_position)
{
	if(m_pInternalBlock==NULL||m_pInternalBlock->handle!=m_hBlock)
		m_pInternalBlock=((CTower*)m_pModel)->Block.FromHandle(m_hBlock);
	acs=des_acs.GetACS();
	GECS cs=m_pInternalBlock->TransToACS(acs);
	return cs.TransPToCS(model_position);
}
GEPOINT CBlockReference::ShadowVectorBlkToModel(const double* blk_vector)
{
	if(m_pInternalBlock==NULL||m_pInternalBlock->handle!=m_hBlock)
		m_pInternalBlock=((CTower*)m_pModel)->Block.FromHandle(m_hBlock);
	acs=des_acs.GetACS();
	GECS cs=m_pInternalBlock->TransToACS(acs);
	return cs.TransVFromCS(blk_vector);
}
GEPOINT CBlockReference::ShadowVectorModelToBlk(const double* model_vector)
{
	if(m_pInternalBlock==NULL||m_pInternalBlock->handle!=m_hBlock)
		m_pInternalBlock=((CTower*)m_pModel)->Block.FromHandle(m_hBlock);
	acs=des_acs.GetACS();
	GECS cs=m_pInternalBlock->TransToACS(acs);
	return cs.TransVToCS(model_vector);
}

CSolidPart CBlockReference::GetSolidPartObject()
{
	CSolidPart solidPart;
	solidPart.m_iLineType = 0;
	solidPart.m_hPart = handle;
	solidPart.m_idClassType = CLS_BLOCKREF;
	solidPart.acs = acs;
	solidPart.pBody = pSolidBody;
	return solidPart;
}

//
int CBlockReference::GetPropValueStr(long id,char* valueStr)
{
	char sText[100]="";
	CParaCS::PARAMETRIC_INFO paramcs=des_acs.GetParamInfo();
	if(GetPropID("basicInfo")==0)
		strcpy(sText,"");
	else if(GetPropID("handle")==id)
		sprintf(sText,"0X%X",handle);
	else if(GetPropID("BelongBlock")==id)
	{
		CBlockModel* pBlock=((CTower*)BelongModel())->Block.FromHandle(m_hBlock);
		if(pBlock)
			sprintf(sText,"%s(0X%X)",pBlock->name,m_hBlock);
		else
			strcpy(sText,"0X0");
	}
	else if(GetPropID("descript")==id)
		sprintf(sText,"%s",descript);
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("cfgword")==id)
		//return MakeWordToString(cfgword,valueStr);
	{
		if(layer(0)=='L')	//���ȹ���
			return cfgword.GetLegScopeStr(valueStr,50,true);
		else
			return cfgword.GetBodyScopeStr(valueStr);
	}
	else if(GetPropID("layer")==id)
		strcpy(sText,layer());
	else if(GetPropID("m_bExploded")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bExploded)
			strcpy(sText,"Yes");
		else 
			strcpy(sText,"No");
#else
		if(m_bExploded)
			strcpy(sText,"��");
		else 
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("des_acs.m_cDefStyle")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(paramcs.m_cDefStyle==1)
			strcpy(sText,"1.Symmetrical");
		else
			strcpy(sText,"0.Parametric Define");
#else
		if(paramcs.m_cDefStyle==1)
			strcpy(sText,"1.�Գ�");
		else
			strcpy(sText,"0.����������");
#endif
	}
	else if(GetPropID("des_acs.m_eDatumPlane")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(paramcs.m_eDatumPlane==CParaCS::DP_XY)
			strcpy(sText,"0.X-Y Plane");
		else if(paramcs.m_eDatumPlane==CParaCS::DP_YX)
			strcpy(sText,"1.Y-X Plane");
		else if(paramcs.m_eDatumPlane==CParaCS::DP_YZ)
			strcpy(sText,"2.Y-Z Plane");
		else if(paramcs.m_eDatumPlane==CParaCS::DP_ZY)
			strcpy(sText,"3.Z-Y Plane");
		else if(paramcs.m_eDatumPlane==CParaCS::DP_ZX)
			strcpy(sText,"4.Z-X Plane");
		else if(paramcs.m_eDatumPlane==CParaCS::DP_XZ)
			strcpy(sText,"5.X-Z Plane");
#else
		if(paramcs.m_eDatumPlane==CParaCS::DP_XY)
			strcpy(sText,"0.X-Y��");
		else if(paramcs.m_eDatumPlane==CParaCS::DP_YX)
			strcpy(sText,"1.Y-X��");
		else if(paramcs.m_eDatumPlane==CParaCS::DP_YZ)
			strcpy(sText,"2.Y-Z��");
		else if(paramcs.m_eDatumPlane==CParaCS::DP_ZY)
			strcpy(sText,"3.Z-Y��");
		else if(paramcs.m_eDatumPlane==CParaCS::DP_ZX)
			strcpy(sText,"4.Z-X��");
		else if(paramcs.m_eDatumPlane==CParaCS::DP_XZ)
			strcpy(sText,"5.X-Z��");
#endif
	}
	else if(GetPropID("acs.origin.x")==id)
	{
		sprintf(sText,"%f",acs.origin.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("acs.origin.y")==id)
	{
		sprintf(sText,"%f",acs.origin.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("acs.origin.z")==id)
	{
		sprintf(sText,"%f",acs.origin.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("acs.axis_x")==id)
		strcpy(sText,des_acs.GetAxisDescription('X'));
	else if(GetPropID("acs.axis_x.x")==id)
	{
		sprintf(sText,"%f",acs.axis_x.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("acs.axis_x.y")==id)
	{
		sprintf(sText,"%f",acs.axis_x.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("acs.axis_x.z")==id)
	{
		sprintf(sText,"%f",acs.axis_x.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("acs.axis_y")==id)
		strcpy(sText,des_acs.GetAxisDescription('Y'));
	else if(GetPropID("acs.axis_y.x")==id)
	{
		sprintf(sText,"%f",acs.axis_y.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("acs.axis_y.y")==id)
	{
		sprintf(sText,"%f",acs.axis_y.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("acs.axis_y.z")==id)
	{
		sprintf(sText,"%f",acs.axis_y.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("acs.axis_z")==id)
		strcpy(sText,des_acs.GetAxisDescription('Z'));
	else if(GetPropID("acs.axis_z.x")==id)
	{
		sprintf(sText,"%f",acs.axis_z.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("acs.axis_z.y")==id)
	{
		sprintf(sText,"%f",acs.axis_z.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("acs.axis_z.z")==id)
	{
		sprintf(sText,"%f",acs.axis_z.z);
		SimplifiedNumString(sText);
	}
		
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
long CBlockReference::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem =CBlockReference::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		/*CString prompt="Did not find a property ID named\"";
		prompt+=propStr;
		prompt+="\"!";
		AfxMessageBox(prompt);*/
#else
		/*CString prompt="û�ҵ���Ϊ\"";
		prompt+=propStr;
		prompt+="\"������ID��";
		AfxMessageBox(prompt);*/
#endif
		return 0;
	}
}

BOOL CBlockReference::IsPropEqual(HASHOBJSET &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CBlockReference *pBlockRef=NULL, *pFirstBlockRef = (CBlockReference*)selectObjs.GetFirst();
	pFirstBlockRef->GetPropValueStr(idProp,valueStr);
	for(pBlockRef=(CBlockReference*)selectObjs.GetNext();pBlockRef;pBlockRef=(CBlockReference*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pBlockRef->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CBlockReference::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CBlockReference::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}

BOOL CBlockReference::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CBlockReference::propStatusHashtable.SetValueAt(id,dwStatus);
}

void CBlockReference::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteDword(dwPermission);
	buffer.WriteInteger(iSeg);
	buffer.Write(cfgword.flag.word,24);
	buffer.WriteString(layer());

	buffer.WritePoint(acs.origin);
	buffer.WritePoint(acs.axis_x);
	buffer.WritePoint(acs.axis_y);
	buffer.WritePoint(acs.axis_z);
	des_acs.ToBuffer(buffer,version,doc_type);
	buffer.WriteInteger(m_hBlock);
	buffer.WriteString(descript);
	buffer.WriteInteger(m_bExploded);
#ifdef __COMMON_PART_INC_
	BUFFERPOP stack(&buffer,hashReserveBolts.GetNodeNum());
	buffer.WriteInteger(hashReserveBolts.GetNodeNum());
	for(CLDSBolt** ppBolt=hashReserveBolts.GetFirst();ppBolt;ppBolt=hashReserveBolts.GetNext())
	{
		buffer.WriteInteger(hashReserveBolts.GetCursorKey());
		if(*ppBolt!=NULL)
			buffer.WriteInteger((*ppBolt)->handle);
		else
			buffer.WriteInteger(0);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of Node concentrated load is wrong!");
#else
		CLDSObject::Log2File()->Log("�ڵ㼯�к��ؼ�¼�����ִ���!");
#endif
#endif
}

void CBlockReference::ToXmlFile(FILE* fp,DWORD schema)
{
	int i=0;
	char sText[500]="";
	fprintf(fp,"<����������Ϣ dwPermission=\"%d\" iSeg=\"%d\">\n",dwPermission,iSeg);
	fprintf(fp,"<��ĺ� cfgword=\"(%d,%d,%d,%d)\"/>\n",cfgword.flag.word[0],cfgword.flag.word[1],cfgword.flag.word[2],cfgword.flag.word[3]);
	fprintf(fp,"<����ͼ�� layer=\"%s\"/>\n",layer());
	fprintf(fp,"<ԭ������ origin=\"(%.3f,%.3f,%.3f)\"/>\n",acs.origin.x,acs.origin.y,acs.origin.z);
	fprintf(fp,"<X������ axis_x=\"(%.3f,%.3f,%.3f)\"/>\n",acs.axis_x.x,acs.axis_x.y,acs.axis_x.z);
	fprintf(fp,"<Y������ axis_y=\"(%.3f,%.3f,%.3f)\"/>\n",acs.axis_y.x,acs.axis_y.y,acs.axis_y.z);
	fprintf(fp,"<Z������ axis_z=\"(%.3f,%.3f,%.3f)\"/>\n",acs.axis_z.x,acs.axis_z.y,acs.axis_z.z);
	des_acs.ToXmlFile(fp,schema);
	fprintf(fp,"<�������� Block_handle=\"0x%x\"/>\n",m_hBlock);
	fprintf(fp,"<������������ descript=\"%s\"/>\n",descript);
	if(m_bExploded==1)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<�Ƿ�ը�� bExploded=\"%s\"/>\n",sText);
	fprintf(fp,"</����������Ϣ>\n");
}
void CBlockReference::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	buffer.ReadDword(&dwPermission);
	buffer.ReadInteger(&_iSeg.iSeg);
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	BelongModel()->EvolveCfgword(cfgword,version,doc_type);
	buffer.ReadString(layer(),4);

	buffer.ReadPoint(acs.origin);
	buffer.ReadPoint(acs.axis_x);
	buffer.ReadPoint(acs.axis_y);
	buffer.ReadPoint(acs.axis_z);
	des_acs.FromBuffer(buffer,version,doc_type);
	buffer.ReadInteger(&m_hBlock);
	buffer.ReadString(descript,50);
	buffer.ReadInteger(&m_bExploded);
#ifdef __COMMON_PART_INC_
	if(  version==0||
		(doc_type==1&&version>=4020006)||(doc_type==2&&version>=1020006)||
		(doc_type==3&&version>=2000006)||(doc_type==4&&version>=1020006)||(doc_type==5&&version>=1020006))
	{
		long reserve_bolt_n=0;
		buffer.ReadInteger(&reserve_bolt_n);
		for(int i=0;i<reserve_bolt_n;i++)
		{
			long hBolt=0,hBlockBolt=0;
			buffer.ReadInteger(&hBlockBolt);
			buffer.ReadInteger(&hBolt);
			CLDSBolt* pBolt=(CLDSBolt*)BelongModel()->FromPartHandle(hBolt,CLS_BOLT);
			if(pBolt)
			{
				hashReserveBolts.SetValue(hBlockBolt,pBolt);
				pBolt->SetAggregateState();
			}
		}
	}
#endif
}

BOOL CBlockReference::Exploded(BOOL bExploded)
{
	if(!m_pModel->IsTowerModel())
		return FALSE;
	CTower *pTower=(CTower*)m_pModel;
	CBlockModel *pBlock=pTower->Block.FromHandle(m_hBlock);
	if(pBlock==NULL)
		return FALSE;
	if(m_bExploded&&!bExploded)
	{
		/*for(CLDSPart *pPart=m_pModel->EnumPartFirst();pPart;pPart=m_pModel->EnumPartNext())
		{
			if(!pPart->IsMemberOfBlockRef(this))
				continue;
			m_pModel->DeletePart(pPart->handle);
		}
		for(CLDSNode* pNode=m_pModel->EnumNodeFirst();pNode;pNode=m_pModel->EnumNodeNext())
		{
			if(!pNode->IsMemberOfBlockRef(this))
				continue;
			m_pModel->DeleteNode(pNode->handle);
		}*/
		m_bExploded=FALSE;
	}
	else if(!m_bExploded&&bExploded)
	{
		CHashList<CLDSPart*> hashPartPtr;
		CHashList<CLDSNode*> hashNodePtr;
		if(pBlock->IsEmbeded())
		{
			for(CLDSNode* pNode=m_pModel->EnumNodeFirst();pNode;pNode=m_pModel->EnumNodeNext())
			{
				if(pNode->m_hBlock!=pBlock->handle)
					continue;
				int nPush=m_pModel->PushNodeStack();
				CLDSNode *pShadowNode=m_pModel->AppendNode();
				pShadowNode->SetShadowPara(pNode,this);
				pShadowNode->Set();
				m_pModel->PopNodeStack(nPush);
				hashNodePtr.SetValue((DWORD)pNode,pShadowNode);
			}
			for(CLDSPart *pPart=m_pModel->EnumPartFirst();pPart;pPart=m_pModel->EnumPartNext())
			{
				if(pPart->m_hBlock!=pBlock->handle)
					continue;
				int nPush=m_pModel->PushPartStack();
				CLDSPart *pShadowPart=NULL;
#ifdef __COMMON_PART_INC_
				CLDSBolt** ppBolt=hashReserveBolts.GetValue(pPart->handle);
				if(ppBolt!=NULL)
				{
					pShadowPart=*ppBolt;
					pShadowPart->SetAggregateState(false);
				}
				else
#endif
					pShadowPart=m_pModel->AppendPart(pPart->GetClassTypeId());
				pShadowPart->SetShadowPara(pPart,this);
				pShadowPart->ShadowDesign();
				if(pPart->IsLinePart())
				{
					CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
					CLDSLinePart *pShadowRod=(CLDSLinePart*)pShadowPart;
					if(pLinePart->pStart)
					{
						CLDSNode **ppNode=hashNodePtr.GetValue((DWORD)pLinePart->pStart);
						if(ppNode)
							pShadowRod->pStart=*ppNode;
					}
					if(pLinePart->pEnd)
					{
						CLDSNode **ppNode=hashNodePtr.GetValue((DWORD)pLinePart->pEnd);
						if(ppNode)
							pShadowRod->pEnd=*ppNode;
					}
				}
				m_pModel->PopPartStack(nPush);
				hashPartPtr.SetValue((DWORD)pPart,pShadowPart);
			}
		}
		else 
		{	
			for(CLDSNode* pNode=pBlock->EnumNodeFirst();pNode;pNode=pBlock->EnumNodeNext())
			{
				CLDSNode *pShadowNode;
				BOUNDARY_NODE *pBoundaryNode = boundary.GetValue(pNode->handle);
				if(pBoundaryNode!=NULL)	//�߽�ڵ㣬��������ģ���еĽڵ㲻�������²���Ӱ��ڵ�
					pShadowNode=m_pModel->FromNodeHandle(pBoundaryNode->hHostModelNode);
				else
				{
					pShadowNode=m_pModel->AppendNode();
					pShadowNode->SetShadowPara(pNode,this);
					pShadowNode->Set();
				}
				hashNodePtr.SetValue((DWORD)pNode,pShadowNode);
			}
			for(CLDSPart* pPart=pBlock->Parts.GetFirst();pPart;pPart=pBlock->Parts.GetNext())
			{
				CLDSPart *pShadowPart=NULL;
#ifdef __COMMON_PART_INC_
				CLDSBolt** ppBolt=hashReserveBolts.GetValue(pPart->handle);
				if(ppBolt!=NULL&&*ppBolt!=NULL)
				{
					pShadowPart=*ppBolt;
					pShadowPart->SetAggregateState(false);
				}
				else
#endif
					pShadowPart=m_pModel->AppendPart(pPart->GetClassTypeId());
				int nPush=m_pModel->PushPartStack();
				pShadowPart->SetShadowPara(pPart,this);
				if(pPart->IsLinePart())
					((CLDSLinePart*)pShadowPart)->CalPosition();
				pShadowPart->ShadowDesign();
				if(pPart->IsLinePart())
				{
					CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
					CLDSLinePart *pShadowRod=(CLDSLinePart*)pShadowPart;
					if(pLinePart->pStart)
					{
						CLDSNode **ppNode=hashNodePtr.GetValue((DWORD)pLinePart->pStart);
						if(ppNode)
							pShadowRod->pStart=*ppNode;
					}
					if(pLinePart->pEnd)
					{
						CLDSNode **ppNode=hashNodePtr.GetValue((DWORD)pLinePart->pEnd);
						if(ppNode)
							pShadowRod->pEnd=*ppNode;
					}
				}
				m_pModel->PopPartStack(nPush);
				hashPartPtr.SetValue((DWORD)pPart,pShadowPart);
			}
		}
		hashReserveBolts.Empty();
		//���鲿������ʱ�����½ڵ�ĸ��˼�����
		for(CLDSNode **ppNode=hashNodePtr.GetFirst();ppNode;ppNode=hashNodePtr.GetNext())
		{
			CLDSNode* pShadowNode=(CLDSNode*)*ppNode;
			CLDSNode *pKeyNode=(CLDSNode*)hashNodePtr.GetCursorKey();
			if(pKeyNode->hFatherPart>0)
			{
				CLDSPart* pNodeFaterPart=pKeyNode->BelongModel()->FromPartHandle(pKeyNode->hFatherPart);
				CLDSPart** pShadowNodeFatherPart=hashPartPtr.GetValue((DWORD)pNodeFaterPart);
				if(pShadowNodeFatherPart)
					pShadowNode->hFatherPart=(*pShadowNodeFatherPart)->handle;
			}
		}
#ifdef __COMMON_PART_INC_
		//���鲿������ʱ������˨���뵽��Ӧ�Ĺ�����
		for(CLDSPart **ppPart=hashPartPtr.GetFirst();ppPart;ppPart=hashPartPtr.GetNext())
		{
			CLDSPart *pKeyPart=(CLDSPart*)hashPartPtr.GetCursorKey();
			CLsRefList *pLsRefList=(*ppPart)->GetLsRefList();
			CLsRefList *pKeyLsRefList=pKeyPart->GetLsRefList();
			if(pLsRefList==NULL||pKeyLsRefList==NULL||pKeyLsRefList->GetLsCount()<=0)
				continue;
			int nPush=hashPartPtr.push_stack();
			for(CLsRef *pLsRef=pKeyLsRefList->GetFirst();pLsRef;pLsRef=pKeyLsRefList->GetNext())
			{
				CLDSPart **ppBlot=(CLDSPart**)hashPartPtr.GetValue((DWORD)pLsRef->GetLsPtr());
				if(ppBlot&&(*ppBlot)->GetClassTypeId()==CLS_BOLT)
					pLsRefList->append(((CLDSBolt*)(*ppBlot))->GetLsRef());
			}
			hashPartPtr.pop_stack(nPush);
		}
#endif
		m_bExploded=TRUE;
	}
	return TRUE;
}

long GetGlobalNewHandle();	//defined in CommandFileProcessFunc.cpp
//#ifndef __TIMER_COUNT_
//#define __TIMER_COUNT_
//#endif
#ifdef __TIMER_COUNT_
#include "TimerCount.h"
#endif
BOOL CBlockReference::IntegrateToTowerModel()
{
	if(!m_pModel->IsTowerModel())
		return FALSE;
	CTower *pTower=(CTower*)m_pModel;
	CBlockModel *pBlock=pTower->Block.FromHandle(m_hBlock);
	if(pBlock==NULL)
		return FALSE;
#ifdef __TIMER_COUNT_
	timer.Start();
#endif
	CTower model;
	CLDSPoint* pPoint,*pNewPoint;
	CLDSLine* pLine,*pNewLine;
	CLDSPlane* pPlane,*pNewPlane;
	CLDSNode* pNode,*pNewNode;
	CLDSPart* pPart,*pNewPart;
	//1.��Ӳ��������еĶ�����ǵ���ʱ����ģ��model��
	if(pBlock->IsEmbeded())
	{
		for(pPoint=m_pModel->EnumPointFirst();pPoint;pPoint=m_pModel->EnumPointNext())
		{
			if(pPoint->m_hBlock!=pBlock->handle)
				continue;
			pNewPoint=model.AppendPoint(FALSE);
			pNewPoint->handle=pPoint->handle;
		}
		for(pLine=m_pModel->EnumLineFirst();pLine;pLine=m_pModel->EnumLineNext())
		{
			if(pLine->m_hBlock!=pBlock->handle)
				continue;
			pNewLine=model.AppendLine(FALSE);
			pNewLine->handle=pLine->handle;
		}
		for(pPlane=m_pModel->EnumPlaneFirst();pPlane;pPlane=m_pModel->EnumPlaneNext())
		{
			if(pPlane->m_hBlock!=pBlock->handle)
				continue;
			pNewPlane=model.AppendPlane(FALSE);
			pNewPlane->handle=pPlane->handle;
		}
		for(pNode=m_pModel->EnumNodeFirst();pNode;pNode=m_pModel->EnumNodeNext())
		{
			if(pNode->m_hBlock!=pBlock->handle)
				continue;
			pNewNode=model.AppendNode(FALSE);
			pNewNode->handle=pNode->handle;
		}
		for(pPart=m_pModel->EnumPartFirst();pPart;pPart=m_pModel->EnumPartNext())
		{
			if(pPart->m_hBlock!=pBlock->handle)
				continue;
			pNewPart=model.Parts.append(pPart->GetClassTypeId(),FALSE);
			pNewPart->handle=pPart->handle;
		}
	}
	else 
	{
		for(pPoint=pBlock->EnumPointFirst();pPoint;pPoint=pBlock->EnumPointNext())
		{
			pNewPoint=model.AppendPoint(FALSE);
			pNewPoint->handle=pPoint->handle;
		}
		for(pLine=pBlock->EnumLineFirst();pLine;pLine=pBlock->EnumLineNext())
		{
			pNewLine=model.AppendLine(FALSE);
			pNewLine->handle=pLine->handle;
		}
		for(pPlane=pBlock->EnumPlaneFirst();pPlane;pPlane=pBlock->EnumPlaneNext())
		{
			pNewPlane=model.AppendPlane(FALSE);
			pNewPlane->handle=pPlane->handle;
		}
		for(pNode=pBlock->EnumNodeFirst();pNode;pNode=pBlock->EnumNodeNext())
		{
			//BOUNDARY_NODE *pBoundaryNode = boundary.GetValue(pNode->handle);
			//if(pBoundaryNode!=NULL)	//�߽�ڵ㣬��������ģ���еĽڵ㲻�������²���Ӱ��ڵ�
			//	pShadowNode=m_pModel->FromNodeHandle(pBoundaryNode->hHostModelNode);
			//else
			pNewNode=model.AppendNode(FALSE);
			pNewNode->handle=pNode->handle;
		}
		for(pPart=pBlock->EnumPartFirst();pPart;pPart=pBlock->EnumPartNext())
		{
			pNewPart=model.Parts.append(pPart->GetClassTypeId(),FALSE);
			pNewPart->handle=pPart->handle;
		}
	}
	model.Point.RebuildHashtable();
	model.Line.RebuildHashtable();
	model.Plane.RebuildHashtable();
	model.Node.RebuildHashtable();
	model.Parts.RebuildHashtable();
#ifdef __TIMER_COUNT_
	timer.Relay(1);
#endif
	//2.����������ݶ�������
	CBuffer clone(4096);
	if(pBlock->IsEmbeded())
	{
		for(pPoint=m_pModel->EnumPointFirst();pPoint;pPoint=m_pModel->EnumPointNext())
		{
			if(pPoint->m_hBlock!=pBlock->handle)
				continue;
			clone.ClearContents();
			pPoint->ToBuffer(clone);
			clone.SeekToBegin();
			pNewPoint=model.FromPointHandle(pPoint->handle);
			pNewPoint->FromBuffer(clone);
		}
		for(pLine=m_pModel->EnumLineFirst();pLine;pLine=m_pModel->EnumLineNext())
		{
			if(pLine->m_hBlock!=pBlock->handle)
				continue;
			clone.ClearContents();
			pLine->ToBuffer(clone);
			clone.SeekToBegin();
			pNewLine=model.FromLineHandle(pLine->handle);
			pNewLine->FromBuffer(clone);
		}
		for(pPlane=m_pModel->EnumPlaneFirst();pPlane;pPlane=m_pModel->EnumPlaneNext())
		{
			if(pPlane->m_hBlock!=pBlock->handle)
				continue;
			clone.ClearContents();
			pPlane->ToBuffer(clone);
			clone.SeekToBegin();
			pNewPlane=model.FromPlaneHandle(pPlane->handle);
			pNewPlane->FromBuffer(clone);
		}
		for(pNode=m_pModel->EnumNodeFirst();pNode;pNode=m_pModel->EnumNodeNext())
		{
			if(pNode->m_hBlock!=pBlock->handle)
				continue;
			clone.ClearContents();
			pNode->ToBuffer(clone);
			clone.SeekToBegin();
			pNewNode=model.FromNodeHandle(pNode->handle);
			pNewNode->FromBuffer(clone);
			//����ΪӰ��ڵ�,����װ������
			pNewNode->SetBelongModel(BelongModel());
			pNewNode->SetShadowPara(pNode,this);
			pNewNode->Set();
			pNewNode->SetBelongModel(&model);
			pNewNode->SetShadowPara(NULL,(long)0);//ȡ����ʱӰ��
		}
		for(pPart=m_pModel->EnumPartFirst();pPart;pPart=m_pModel->EnumPartNext())
		{
			if(pPart->m_hBlock!=pBlock->handle)
				continue;
			clone.ClearContents();
			pPart->ToBuffer(clone);
			clone.SeekToBegin();
			pNewPart=model.FromPartHandle(pPart->handle);
			pNewPart->FromBuffer(clone);
			//����ΪӰ��ڵ�,����װ������
			pNewPart->SetBelongModel(BelongModel());
			pNewPart->SetShadowPara(pPart,this);
			if(pNewPart->IsLinePart())
				((CLDSLinePart*)pNewPart)->CalPosition();
			pNewPart->ShadowDesign();
			pNewPart->SetBelongModel(&model);
			pNewPart->SetShadowPara(NULL,(long)0);//ȡ����ʱӰ��
		}
	}
	else 
	{	
		for(pPoint=pBlock->EnumPointFirst();pPoint;pPoint=pBlock->EnumPointNext())
		{
			clone.ClearContents();
			pPoint->ToBuffer(clone);
			clone.SeekToBegin();
			pNewPoint=model.FromPointHandle(pPoint->handle);
			pNewPoint->FromBuffer(clone);
		}
		for(pLine=pBlock->EnumLineFirst();pLine;pLine=pBlock->EnumLineNext())
		{
			clone.ClearContents();
			pLine->ToBuffer(clone);
			clone.SeekToBegin();
			pNewLine=model.FromLineHandle(pLine->handle);
			pNewLine->FromBuffer(clone);
		}
		for(pPlane=pBlock->EnumPlaneFirst();pPlane;pPlane=pBlock->EnumPlaneNext())
		{
			clone.ClearContents();
			pPlane->ToBuffer(clone);
			clone.SeekToBegin();
			pNewPlane=model.FromPlaneHandle(pPlane->handle);
			pNewPlane->FromBuffer(clone);
		}
		for(pNode=pBlock->EnumNodeFirst();pNode;pNode=pBlock->EnumNodeNext())
		{
			//BOUNDARY_NODE *pBoundaryNode = boundary.GetValue(pNode->handle);
			//if(pBoundaryNode!=NULL)	//�߽�ڵ㣬��������ģ���еĽڵ㲻�������²���Ӱ��ڵ�
			//	pShadowNode=m_pModel->FromNodeHandle(pBoundaryNode->hHostModelNode);
			//else
			clone.ClearContents();
			pNode->ToBuffer(clone);
			clone.SeekToBegin();
			pNewNode=model.FromNodeHandle(pNode->handle);
			pNewNode->FromBuffer(clone);
			//����ΪӰ��ڵ�,����װ������
			pNewNode->SetBelongModel(BelongModel());
			pNewNode->SetShadowPara(pNode,this);
			pNewNode->Set();
			pNewNode->SetBelongModel(&model);
			pNewNode->SetShadowPara(NULL,(long)0);//ȡ����ʱӰ��
		}
		for(pPart=pBlock->EnumPartFirst();pPart;pPart=pBlock->EnumPartNext())
		{
			clone.ClearContents();
			pPart->ToBuffer(clone);
			clone.SeekToBegin();
			pNewPart=model.FromPartHandle(pPart->handle);
			pNewPart->FromBuffer(clone);
			//����ΪӰ��ڵ�,����װ������
			pNewPart->SetBelongModel(BelongModel());
			pNewPart->SetShadowPara(pPart,this);
			if(pNewPart->IsLinePart())
				((CLDSLinePart*)pNewPart)->CalPosition();
			pNewPart->ShadowDesign();
			pNewPart->SetBelongModel(&model);
			pNewPart->SetShadowPara(NULL,(long)0);//ȡ����ʱӰ��
		}
	}
	long hMinCanUse=pTower->handle;
	long hBlkIncrement=max(pTower->handle,pBlock->GetMinAvailableHandle());
	model.ReplaceReferenceHandle(CHandleReplacer(0,hBlkIncrement,NULL,true));
	//�滻��Χ�����ڵ�͸˼����Ϊ��ǰģ���е�ƥ��ڵ��˼� wjh-2017.12.11
	for(EXTERNAL_BLKNODE* pExterNode=pBlock->hashExterNodes.GetFirst();pExterNode;pExterNode=pBlock->hashExterNodes.GetNext())
	{
		GEPOINT xPosition=this->ShadowPositionBlkToModel(pExterNode->xPosition);
		CLDSNode* pMatchNode=pTower->MatchExterNode(xPosition);
		if(pMatchNode==NULL)
		{
			CXhChar50 coordstr;
			coordstr.ConvertFromPoint(xPosition);
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"������0x%X��Χ�����ڵ�%s��δ�ڵ�ǰģ�����ҵ�ƥ��ڵ�!",
				pExterNode->hNode,(char*)coordstr);
		}
		else
			model.ReplaceReferenceHandle(CHandleReplacer(pExterNode->hNode+hBlkIncrement,pMatchNode->handle),CLS_NODE);
	}
	for(EXTERNAL_BLKROD* pExterRod=pBlock->hashExterRods.GetFirst();pExterRod;pExterRod=pBlock->hashExterRods.GetNext())
	{
		GEPOINT xStartPosition=this->ShadowPositionBlkToModel(pExterRod->xStartPosition);
		GEPOINT xEndPosition=this->ShadowPositionBlkToModel(pExterRod->xEndPosition);
		GEPOINT vxWingNorm,vyWingNorm;
		if(pExterRod->cbValidProp.GetBitState(EXTERNAL_BLKROD::PROP_WING_NORM))
		{
			vxWingNorm=this->ShadowVectorBlkToModel(pExterRod->vxWingNorm);
			vyWingNorm=this->ShadowVectorBlkToModel(pExterRod->vyWingNorm);
		}
		CLDSLinePart* pMatchRod=pTower->MatchExterRod(pExterRod->idClsType,pExterRod->cbValidProp,
			xStartPosition,xEndPosition,vxWingNorm,vyWingNorm);
		if(pMatchRod==NULL)
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"������0x%X��Χ�����˼�δ�ڵ�ǰģ�����ҵ�ƥ��˼�!",pExterRod->hRod);
		else
			model.ReplaceReferenceHandle(CHandleReplacer(pExterRod->hRod+hBlkIncrement,pMatchRod->handle));
	}
	for(EXTERNAL_BLKBOLT* pExterBolt=pBlock->hashExterBolts.GetFirst();pExterBolt;pExterBolt=pBlock->hashExterBolts.GetNext())
	{
		GEPOINT xBasePosition=this->ShadowPositionBlkToModel(pExterBolt->xBasePosition);
		GEPOINT vWorkNorm=this->ShadowVectorBlkToModel(pExterBolt->vWorkNorm);
		CLDSBolt* pMatchBolt=pTower->MatchExterBolt(pExterBolt->d,pExterBolt->wL,xBasePosition,vWorkNorm);
		if(pMatchBolt==NULL)
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"������0x%X��Χ������˨��δ�ڵ�ǰģ�����ҵ�ƥ��˼�!",pExterBolt->hBolt);
		else
			model.ReplaceReferenceHandle(CHandleReplacer(pExterBolt->hBolt+hBlkIncrement,pMatchBolt->handle));
	}
#ifdef __TIMER_COUNT_
	timer.Relay(2);
#endif
	//3.����ʱmodelģ�Ϳռ��е����ݶ�������ں���ȫ�ֵĸ���ģ��
	for(pPoint=model.EnumPointFirst();pPoint;pPoint=model.EnumPointNext())
	{
		pNewPoint=pTower->AppendPoint(FALSE);
		pNewPoint->handle=pPoint->handle;
		pTower->handle=max(pTower->handle,pNewPoint->handle);
	}
	for(pLine=model.EnumLineFirst();pLine;pLine=model.EnumLineNext())
	{
		pNewLine=pTower->AppendLine(FALSE);
		pNewLine->handle=pLine->handle;
		pTower->handle=max(pTower->handle,pNewLine->handle);
	}
	for(pPlane=model.EnumPlaneFirst();pPlane;pPlane=model.EnumPlaneNext())
	{
		pNewPlane=pTower->AppendPlane(FALSE);
		pNewPlane->handle=pPlane->handle;
		pTower->handle=max(pTower->handle,pNewPlane->handle);
	}
	for(pNode=model.EnumNodeFirst();pNode;pNode=model.EnumNodeNext())
	{
		pNewNode=pTower->AppendNode(FALSE);
		pNewNode->handle=pNode->handle;
		pTower->handle=max(pTower->handle,pNewNode->handle);
	}
	for(pPart=model.EnumPartFirst();pPart;pPart=model.EnumPartNext())
	{
		pNewPart=pTower->Parts.append(pPart->GetClassTypeId(),FALSE);
		pNewPart->handle=pPart->handle;
		pTower->handle=max(pTower->handle,pNewPart->handle);
	}
	pTower->handle++;
	pTower->Point.RebuildHashtable();
	pTower->Line.RebuildHashtable();
	pTower->Plane.RebuildHashtable();
	pTower->Node.RebuildHashtable();
	pTower->Parts.RebuildHashtable();
	pTower->ReBuildObjsHashTable();
#ifdef __TIMER_COUNT_
	timer.Relay(3);
#endif
	//4.����ʱmodelģ�Ϳռ��е����ݶ����ں���ȫ�ֵĸ���ģ��
	for(pPoint=model.EnumPointFirst();pPoint;pPoint=model.EnumPointNext())
	{
		clone.ClearContents();
		pPoint->ToBuffer(clone);
		clone.SeekToBegin();
		pNewPoint=pTower->FromPointHandle(pPoint->handle);
		pNewPoint->FromBuffer(clone);
		pNewPoint->m_hBlock=0;
	}
	for(pLine=model.EnumLineFirst();pLine;pLine=model.EnumLineNext())
	{
		clone.ClearContents();
		pLine->ToBuffer(clone);
		clone.SeekToBegin();
		pNewLine=pTower->FromLineHandle(pLine->handle);
		pNewLine->FromBuffer(clone);
		pNewLine->m_hBlock=0;
	}
	for(pPlane=model.EnumPlaneFirst();pPlane;pPlane=model.EnumPlaneNext())
	{
		clone.ClearContents();
		pPlane->ToBuffer(clone);
		clone.SeekToBegin();
		pNewPlane=pTower->FromPlaneHandle(pPlane->handle);
		pNewPlane->FromBuffer(clone);
		pNewPlane->m_hBlock=0;
	}
	for(pNode=model.EnumNodeFirst();pNode;pNode=model.EnumNodeNext())
	{
		clone.ClearContents();
		pNode->ToBuffer(clone);
		clone.SeekToBegin();
		pNewNode=pTower->FromNodeHandle(pNode->handle);
		pNewNode->FromBuffer(clone);
		pNewNode->m_hBlock=0;
	}
	for(pPart=model.EnumPartFirst();pPart;pPart=model.EnumPartNext())
	{
		clone.ClearContents();
		pPart->ToBuffer(clone);
		clone.SeekToBegin();
		pNewPart=pTower->FromPartHandle(pPart->handle);
		pNewPart->FromBuffer(clone);
		pNewPart->m_hBlock=0;
		//������ϽǸ������ӽǸֵ���ĩ�˽ڵ���ͬ����û�к�����Ч�İ취���ص��˼����֣����ݲ�����˼��ص���� wjh-2015.1.8
	}
#ifdef __TIMER_COUNT_
	timer.Relay(4);
#endif
	//�ϲ��ظ��ڵ㣬�˲��ִ��벻������ǰ���ȡ���ƹ�����Ϣ����֮ǰ��������ܿ��ܵ��²��ָ˼���ʧʼ�ն˽ڵ� wjh-2014.1.08
	for(pNode=model.EnumNodeFirst();pNode;pNode=model.EnumNodeNext())
	{
		CLDSNode* pOverlapNode=pTower->CheckOverlappedNode(pNode->Position(false),pNode->Cfginfo(),true,pNode->handle);
		pNewNode=pTower->FromNodeHandle(pNode->handle);
		if(pOverlapNode&&pOverlapNode!=pNewNode)
		{
			pTower->ReplaceReferenceHandle(CHandleReplacer(pNewNode->handle,pOverlapNode->handle));
			pTower->Node.DeleteNode(pNewNode->handle);
		}
		else
			pNewNode->relativeObjs.Empty();
	}
#ifdef __TIMER_COUNT_
	timer.Relay(5);
#endif
	//�ϲ��ظ��˼�
	CLDSModule* pModule=pTower->GetActiveModule();
	SmartRodPtr pRod,pNewRod;
	for(pRod=model.EnumRodFirst();pRod.IsHasPtr();pRod=model.EnumRodNext())
	{
		pNewRod=pTower->FromRodHandle(pRod->handle);
		if(pNewRod.IsNULL()||pNewRod->pStart==NULL || pNewRod->pEnd==NULL)
			continue;
		GEPOINT vxAngleWingNorm,vyAngleWingNorm;
		double* pvxAngleWingNorm=NULL,*pvyAngleWingNorm=NULL;
		if(pNewRod->IsAngle())
		{
			vxAngleWingNorm=pNewRod.pAngle->vxWingNorm;
			pvxAngleWingNorm=vxAngleWingNorm;
			vyAngleWingNorm=pNewRod.pAngle->vyWingNorm;
			pvyAngleWingNorm=vyAngleWingNorm;
		}
		SmartRodPtr pOverlapRod=pTower->CheckOverLappedLinePart(pNewRod->pStart->Position(false),pNewRod->pEnd->Position(false),pNewRod->Cfginfo(),
																	pNewRod->handle,pvxAngleWingNorm,pvyAngleWingNorm);
		if(pOverlapRod.IsHasPtr() && pOverlapRod->IsAngle()&&pNewRod->IsAngle())
		{
			if( pOverlapRod.pAngle->group_father_jg_h==pNewRod->handle||pNewRod.pAngle->group_father_jg_h==pOverlapRod->handle||
				pOverlapRod.pAngle->group_father_jg_h==pNewRod.pAngle->group_father_jg_h)
				pNewRod->relativeObjs.Empty();	//��ϽǸֻ�����ӽǸ�
			else
				pTower->IntelliMergeLineAngles(pOverlapRod.pAngle,pNewRod.pAngle);
		}
		else
			pNewRod->relativeObjs.Empty();
	}
#ifdef __TIMER_COUNT_
	timer.Relay(6);
#endif
	//�ϲ��ظ���˨
	/*for(CLDSBolt* pBolt=(CLDSBolt*)model.EnumPartFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)model.EnumPartNext(CLS_BOLT))
	{
		CLDSBolt* pNewBolt=(CLDSBolt*)pTower->FromPartHandle(pBolt->handle);
		if(pNewBolt==NULL)
			continue;
		CLDSBolt* pOverlapBolt=pTower->CheckOverLappedBolt(pNewBolt->ucs.origin,pNewBolt->ucs.axis_z,pNewBolt->handle);
		if(pOverlapBolt)
			pTower->IntelliMergeBolt(pOverlapBolt,pNewBolt);
	}*/
	//5.���������������
	pTower->ShrinkObjHandle(hMinCanUse);
#ifdef __TIMER_COUNT_
	timer.Relay(8);
	timer.End();
	for(DWORD* pdwTicks=timer.hashTicks.GetFirst();pdwTicks!=NULL;pdwTicks=timer.hashTicks.GetNext())
	{
		UINT key=timer.hashTicks.GetCursorKey();
		logerr.Log("step%2d#cost :%.1f", key, 0.001*(*pdwTicks));
	}
#endif
	return TRUE;
}
//���ݲ������öκż���ĺŸ��²����ڹ����Ķκż���ĺ�
void CBlockReference::UpdateBlockSegNoAndCfgWord()
{
	CBlockModel *pBlock=((CTower*)m_pModel)->Block.FromHandle(m_hBlock);
	if(pBlock==NULL)
		return;
	//һ���������ܶ�Ӧ����������ã��˴�������κż���ĺź󣬲��ûָ�ԭ��ֵ��
	//ÿ��ʹ��ʱ����Ҫ���ݵ�ǰ�������ý��и���
	for(CLDSNode *pNode=pBlock->EnumNodeFirst();pNode;pNode=pBlock->EnumNodeNext())
	{
		pNode->iSeg=iSeg;
		pNode->cfgword=cfgword;
	}
	for(CLDSPart *pPart=pBlock->EnumPartFirst();pPart;pPart=pBlock->EnumPartNext())
	{
		pPart->iSeg=iSeg;
		pPart->cfgword=cfgword;
	}
}
void CBlockReference::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	if(pModel==NULL)
	{
		if(m_pModel!=NULL)
			pModel=m_pModel;
	}
	if(pModel==NULL)
		return;
	pModel->ValidateObjRefH(&m_hBlock);
	for (BOUNDARY_NODE* pNode=boundary.GetFirst();pNode;pNode=boundary.GetNext())
	{
		pModel->ValidateObjRefH(&pNode->hBlockNode);
		pModel->ValidateObjRefH(&pNode->hHostModelNode);
	}
}