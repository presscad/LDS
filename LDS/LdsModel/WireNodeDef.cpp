#include "stdafx.h"
#include "WireNodeDef.h"

#if defined(_DEBUG)&&!defined(_DISABLE_DEBUG_NEW_)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
CUniWirePoint::CUniWirePoint()
{
	m_dfHalfConeDegAngle=15;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
long CWireConnPoint::AppendWirePoint(long hWirePoint)
{
	xarrWirePoints.Set(xarrWirePoints.Count,hWirePoint);
	return hWirePoint;
}
long CWirePointGroup::AppendWireConnPoint(long hConnPoint)
{
	this->xarrConnPoints.Append(hConnPoint);
	return hConnPoint;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
CWirePointGroup::CWirePointGroup()
{
	xarrConnPoints.Set(0,1);
}
CWirePointGroup::~CWirePointGroup()
{
}
char* CXhWirePoint::get_szLayer()
{
	_szLayer[3]=0;
	return _szLayer;
}
char* CXhWirePoint::set_szLayer(const char* layer)
{
	StrCopy(_szLayer,layer,4);
	return _szLayer;
}
CXhWirePoint::CXhWirePoint()
{
	blWireAngle=true;
	dfSpaceWidth=sfHoleD=0;
	//memset(holes,0,16);
	holes[0].hBolt=holes[0].hResidePart=0;
	holes[1].hBolt=holes[1].hResidePart=0;
	m_hRelaWireLoadNode=m_hLdsObj=0;
	szLayer[0]=szLayer[1]=szLayer[2]=szLayer[3]=0;
	pSolidBody=NULL;
}
CXhWirePoint::~CXhWirePoint()
{
	if(pSolidBody)
		delete pSolidBody;
}
#include "CreateFace.h"
#include "SolidBody.h"
UINT GetCirclePolyVertex(double radius, GEPOINT* parrVertexes, UINT uiMaxCount,bool blIntelliCalSlices/*=true*/);
bool CXhWirePoint::CreateSolidBody()
{
	if(pSolidBody!=NULL)
	{
		return true;	//delete pSolidBody;
	}
	else
		pSolidBody=new CSolidBody();
	double d=this->sfHoleD-2;
	PRESET_ARRAY16<GEPOINT> xarrBtmVertexes;
	UINT i,slices=GetCirclePolyVertex(d*0.5,xarrBtmVertexes,16,false);
	GECS acs;
	acs.origin=this->xBasicPoint;
	acs.axis_z=this->vStdBasicAxisVec;
	acs.axis_x=inters_vec(f3dPoint(acs.axis_z));
	acs.axis_y=acs.axis_z^acs.axis_x;
	normalize(acs.axis_y);
	acs.axis_x=acs.axis_y^acs.axis_z;
	//
	fBody body;
	for(i=0;i<slices;i++)
	{
		body.vertex.append(acs.TransPFromCS(xarrBtmVertexes[i]));	//������Բ����
		xarrBtmVertexes[i].z=this->dfSpaceWidth;
		body.vertex.append(acs.TransPFromCS(xarrBtmVertexes[i]));	//������Բ����
	}
	body.vertex.append(this->xBasicPoint);
	GEPOINT xTopCenter=this->xBasicPoint+this->vStdBasicAxisVec*this->dfSpaceWidth;
	body.vertex.append(xTopCenter);
	//��ʼ������
	CCreateFace createFace;
	createFace.InitVertexList(&body);

	f3dPoint axis_pick_start,axis_pick_end;
	//�ֹܵ��洴��
	f3dPolyFace *pFace;
	COLORREF crShaft=CLDSPart::GetColor(CLS_POLYJOINT,-1,' ');
	for(i=0;i<slices;i++)
	{
		//���ڲ����棬�������������ֳ�����������
		pFace=body.faceList.append();
		pFace->material = crShaft;
		createFace.NewOutterEdgeLine(pFace,slices*2+0,i*2);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*2);
		createFace.NewOutterEdgeLine(pFace,i*2);
	}
	//�ֹܶ��洴��
	for(i=0;i<slices;i++)
	{
		//���ڲ����棬�������������ֳ�����������
		pFace=body.faceList.append();
		pFace->material = crShaft;
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*2+1,i*2+1);
		createFace.NewOutterEdgeLine(pFace,slices*2+1);
		createFace.NewOutterEdgeLine(pFace,i*2+1);
	}
	//������洴��
	for(i=0;i<slices;i++)
	{
		//�ֹ�����洴��
		pFace = body.faceList.append();
		pFace->material = crShaft;
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*2+0,i*2);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*2+1);
		createFace.NewOutterEdgeLine(pFace,i*2+1);
		createFace.NewOutterEdgeLine(pFace,i*2);
	}
	pSolidBody->ConvertFrom(&body);
	return true;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
CPhaseWire::CPhaseWire()
{
	m_id=0;
	wireplace=0;
	m_ciActiveGroupSerial=1;//pWireGroup->Id;//1;
}
CPhaseWire::~CPhaseWire()
{
}
void CPhaseWire::AppendWireConnPoint(UINT uiWireConnPoint,UINT uiWirePointGroup/*=0*/)
{
	if(uiWirePointGroup==0)
		uiWirePointGroup=this->m_ciActiveGroupSerial;
	CWirePointGroup* pWireGroup=hashWireGroups.Add(uiWirePointGroup);
	pWireGroup->AppendWireConnPoint(uiWireConnPoint);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
CUniWireModel::CUniWireModel()
{
	m_cnEarthWireCount=2;//Ĭ��2������
	layouts[0].wiVolt=500;	//Ĭ�����һ��500kv˫��·��������
	layouts.ReSize(1);
}
//���ݹҽ�������Ϣ���ɸ����ϵ�Ψһ�������(based serial 1)
UINT CUniWireModel::ToWireSerial(WIREPLACE_CODE wireplace,WORD wiVoltage/*=0*/)
{
	BYTE ciAccuCircuit=0;	//�ۻ���·��
	UINT uiAccuSerial=1;	//�ۻ��������
	if(wireplace.ciPhaseSerial==0)	//����
		return min(m_cnEarthWireCount,wireplace.ciPhaseSerial);
	uiAccuSerial=m_cnEarthWireCount;
	for(BYTE ci=0;ci<layouts.Count;ci++)
	{
		if(wireplace.blCircuitDC!=layouts[ci].blCircuitDC)
			uiAccuSerial+=layouts[ci].cnWireCount;
		else if(wiVoltage>0&&wiVoltage!=layouts[ci].wiVolt)
			uiAccuSerial+=layouts[ci].cnWireCount;
		else
		{
			UINT cnPhasesPerCircuit=wireplace.blCircuitDC?2:3;
			return uiAccuSerial+wireplace.ciCircuitSerial*cnPhasesPerCircuit+wireplace.ciPhaseSerial;
		}
	}
	//TODO:�쳣���δ����
	return 0;
}
bool CUniWireModel::UpdateWirePointGimName(CUniWirePoint* pWirePoint)
{
	CXhChar50 sDesc;
	WORD wiVoltage=0;	//�ݲ�ָ����ѹ����
	UINT serial=this->ToWireSerial(pWirePoint->wireplace, wiVoltage);
	WIREPLACE_CODE wireplace=pWirePoint->wireplace;
	GEPOINT xOriginalPos=pWirePoint->xPosition;
	if(wireplace.ciWireDirection=='Y')
	{	//Y������·
		if(wireplace.ciWireType=='E')
		{
			if(xOriginalPos.y<-EPS2)
				sDesc.Printf("ǰ��%d",wireplace.iSerial);
			else if(xOriginalPos.y>EPS2)
				sDesc.Printf("���%d",wireplace.iSerial);
			else
				sDesc.Printf("��%d",wireplace.iSerial);
		}
		else if(wireplace.ciWireType=='C')
		{
			if(xOriginalPos.y<-EPS2)
				sDesc.Printf("ǰ��%d",wireplace.iSerial);
			else if(xOriginalPos.y>EPS2)
				sDesc.Printf("��%d",wireplace.iSerial);
			else
				sDesc.Printf("��%d",wireplace.iSerial);
			if(wireplace.ciHangingStyle=='S' && wireplace.ciPostCode>0)
				sDesc.Append(CXhChar16("-S%d",wireplace.ciPostCode));
			else if(wireplace.ciHangingStyle=='V'&& wireplace.ciPostCode>0)
				sDesc.Append(CXhChar16("-V%d",wireplace.ciPostCode));
		}
		else if(wireplace.ciWireType=='J')
		{
			sDesc.Printf("��%d",wireplace.iSerial);
			if(wireplace.ciPostCode>0)
				sDesc.Append(CXhChar16("-%d",wireplace.ciPostCode));
		}
	}
	else
	{

	}
	//
	pWirePoint->szGimName.Copy(sDesc);
	return sDesc.GetLength()>0;
}
void CUniWireModel::InitWireLayout(BYTE cnCircuitCount,bool blCircuitDC/*=false*/,
	WORD wiVoltage/*=500*/,BYTE cnEarthWireCount/*=2*/)
{
	BYTE ti;
	m_cnEarthWireCount=min(cnEarthWireCount,2);//Ĭ��2������
	CPhaseWire* pPhaseWire;
	for(ti=0;ti<cnEarthWireCount&&ti<2;ti++)
	{
		pPhaseWire=hashPhaseWires.Add(ti+1);
		pPhaseWire->wireplace.ciPhaseSerial=0;
		pPhaseWire->wireplace.ciCircuitSerial=0;
		pPhaseWire->wireplace.ciPostCode=ti;
	}
	layouts.ReSize(0);
	AppendWireLayout(cnCircuitCount,blCircuitDC,wiVoltage);
}
bool CUniWireModel::AppendWireLayout(BYTE cnCircuitCount,bool blCircuitDC/*=false*/,WORD wiVoltage/*=500*/)
{
	CIRCUIT_LAYOUT layout(cnCircuitCount,blCircuitDC,wiVoltage);
	BYTE ti;
	for(ti=0;ti<layouts.Count;ti++)
	{
		if(layout.blCircuitDC==layouts[ti].blCircuitDC&&layout.wiVolt==layouts[ti].wiVolt)
			return false;	//������Ӧƥ�䣨��ֱ�����ͼ���ѹ�ȼ����Ļ�·
	}
	layouts.Append(layout);
	CPhaseWire	phasewire;
	phasewire.wireplace.blCircuitDC=blCircuitDC;
	for(ti=0;ti<cnCircuitCount;ti++)
	{
		phasewire.wireplace.ciCircuitSerial=ti+1;
		BYTE cnPhasesPerCircuit=blCircuitDC?2:3;
		for(BYTE phase=0;phase<cnPhasesPerCircuit;phase++)
		{
			phasewire.wireplace.ciPhaseSerial=phase+1;
			UINT uiSerial=this->ToWireSerial(phasewire.wireplace,wiVoltage);
			CPhaseWire* pPhaseWire=hashPhaseWires.Add(uiSerial);
			pPhaseWire->wireplace=phasewire.wireplace;
		}
	}
	return true;
}
//�Զ����ҹҿ׶��γɹҵ����CUniWirePoint
//�������ʵ�ʵ���������CXhWirePoint
//�Զ��Թҵ��������γɹҵ����CWireConnPoint
//�ڹҵ��������װ�ɶԽӵ����CWireConnPoint
#include "../env_def.h"
void Test_WireModel()
{
	CUniWireModel model;
	model.InitWireLayout(2);//��ʼ�����߷�����Ĭ��Ϊ500kv����˫��·
	//model.AppendWireLayout(2);
	//������¹ҵ�
	CXhWirePoint* pWirePoint=model.hashWirePoints.Add(1);
	pWirePoint->holes[0].hBolt=0x345;
	pWirePoint->holes[0].hResidePart=0x456;
	pWirePoint->holes[1].hBolt=0x346;
	pWirePoint->holes[1].hResidePart=0x456;
	pWirePoint->uiInsulatorStyle=CUniWirePoint::INSULATOR_DS;
	pWirePoint->szInsulatorStyleName="����˫��";
	//�������װ��Խӵ�
	CWireConnPoint* pWireConnPoint=model.hashConnPoints.Add(1);
	pWireConnPoint->AppendWirePoint(1);	//��ӵ��߶Խӵ�
	//�������¶Խӵ��������������
	CPhaseWire* pPhaseWire=model.hashPhaseWires.GetValue(3);
	pPhaseWire->AppendWireConnPoint(pWireConnPoint->Id);
}

