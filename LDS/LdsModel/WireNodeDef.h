#pragma once
#include "WirePlaceCode.h"
#include "XhCharString.h"
#include "ArrayList.h"
#include "f_ent.h"
#include "Hashtable.h"
#include "Tower.h"	//"lds_part.h"

//�ҵ���顢�Խӵ㡢�ҵ�
//ͨ�ùҵ�ģ��(Universal wire point)
//class CLdsWirePoint : public CLDSDbObject
class CUniWirePoint{
protected:
	UINT m_id;
	GECS acs;	//��߲��ҵ�װ��λ�ã�Z���ʾ����������������
	char m_szStyleName[51];	//���͹ҽ��������ƣ�����ɸѡ�ɹҽӴ����ͣ�,��DS(����˫������DSX(����˫����������DN���������Ŵ���
	virtual GEPOINT _InternalGetWirePointPosition(){return acs.origin;}
public:
	double m_dfHalfConeDegAngle;	//���Χ��׶��
	static const UINT INSULATOR_DS	= 0x00000101;	//����˫��
	static const UINT INSULATOR_DSX	= 0x00010101;	//����˫������
	static const UINT INSULATOR_DN	= 0x00020001;	//��������
	UINT uiInsulatorStyle;	//���͹ҽ�����
	CXhChar50 szGimName;	//gim�ƽ�ʱ������
	WIREPLACE_CODE wireplace;
	void SetKey(DWORD key){m_id=key;}
	CUniWirePoint();
	CXhString get_szInsulatorStyleName(){ return CXhString(m_szStyleName,51); }
	CXhString set_szInsulatorStyleName(const char* szName){
		StrCopy(m_szStyleName,szName,51);
		return CXhString(m_szStyleName,51);
	}
	GEPOINT get_xPosition(){return _InternalGetWirePointPosition();}
	GEPOINT set_xPosition(const double* position){return acs.origin=position;}
	__declspec(property(put=set_xPosition,get=get_xPosition)) GEPOINT xPosition;
	__declspec(property(put=set_szInsulatorStyleName,get=get_szInsulatorStyleName)) CXhString szInsulatorStyleName;
	UINT get_Id(){return m_id;}
	__declspec(property(get=get_Id)) UINT Id;
};
class CWireConnPoint{	//�����ߵĶԽӵ㣬��װ�����
protected:
	UINT m_id;
public:
	CXhChar50 name;		//�Խӵ�����
	void SetKey(DWORD key){m_id=key;}
	PRESET_ARRAY8<long> xarrWirePoints;	//ÿ���Խӵ�ҵ���������֧��8���ҵ㣨�ҵ��ʶ��
	UINT get_Id(){return m_id;}
	__declspec(property(get=get_Id)) UINT Id;
	long AppendWirePoint(long hWirePoint);
};
//�ҵ���飨ÿ�������չҽ�һ���ҵ���飬ÿ���ҵ����ɰ�������ҵ㣩
class CWirePointGroup{
protected:
	UINT m_id;
	char m_szGroupName[17];	//���͹ҽ��������ƣ�����ɸѡ�ɹҽӴ����ͣ�,��DS(����˫������DSX(����˫����������DN���������Ŵ���
public:
	WIREPLACE_CODE wireplace;	//���洢�ҽ�����
	PRESET_ARRAY4<long> xarrConnPoints;	//ͬһ�ҵ���������֧��4���Խӵ�
	CXhChar50 name;			//��������
	UINT uiHangingStyle;	//���͹ҽ�����
	CXhString get_szHangingStyleName(){ return CXhString(m_szGroupName,17); }
	CXhString set_szHangingStyleName(const char* szName){
		StrCopy(m_szGroupName,szName,17);
		return CXhString(m_szGroupName,17);
	}
	void SetKey(DWORD key){m_id=key;}
	UINT get_Id(){return m_id;}
	__declspec(property(get=get_Id)) UINT Id;
	long AppendWireConnPoint(long hConnPoint);
	CWirePointGroup();
	~CWirePointGroup();
};
class CSolidBody;
class CXhWirePoint : public CUniWirePoint//,CLDSDbObject
{
protected:
	char _szLayer[4];
public:
	long m_hLdsObj;
	long m_hRelaWireLoadNode;	//�뵱ǰ���߹ҵ��������LDSģ�ͽڵ���
public:
	bool blWireAngle;	//�ҿ��Ƿ������ڹ��߽Ǹ��ϣ�һ�㲻�������ڽǸ��Ͼ��������ڰ���
	float sfHoleD;		//�ҿ״�С
	GEPOINT vStdBasicAxisVec;	//hole[0]->hole[1]�Ľ��ת�᷽��
	GEPOINT xBasicPoint;		//hole[0]�����������ĵ㣨�ְ����Բ����ĵ㣩
	double dfSpaceWidth;		//�ҿ׼�ࣨ��߿��+��϶��
	struct HOLE{
		long hBolt,hResidePart;
		HOLE(){hBolt=hResidePart=0;}
	}holes[2];	//ʵ��ҿף�һ����1~2�������
	CSolidBody* pSolidBody;
	char* get_szLayer();
	char* set_szLayer(const char* layer);
	__declspec(property(put=set_szLayer,get=get_szLayer)) char* szLayer;
public:
	CXhWirePoint();
	~CXhWirePoint();
	bool CreateSolidBody();
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0){;}
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0){;}
};
class CPhaseWire
{	//ÿ�ർ�߶�Ӧ��һ��ʵ�ʵĹҽӵ�
protected:
	UINT m_id;	//�������
public:
	UINT get_Id(){return m_id;}
	__declspec(property(get=get_Id)) UINT Id;
	void SetKey(DWORD key){m_id=key;}
public:
	WIREPLACE_CODE wireplace;	//����GIM�ҵ����Զ���
	long m_idConnPoint;//m_hWirePointGroup;
	//--PhaseWire����
	BYTE ciCircuitNumber;
	BYTE ciPhaseSerial;	//�����
	//--InsulatorBranch��������
	BYTE m_ciActiveGroupSerial;	//���ڼ���״̬�Ĺҵ����
	WORD wiBranchSerial;
	CXhChar16 m_sInsulatorType;	//="����"
	CXhChar50 m_sHangWireDesc;	//��������
	///
	CHashList<CWirePointGroup> hashWireGroups;
	CPhaseWire();
	~CPhaseWire();
	void AppendWireConnPoint(UINT uiWireConnPoint,UINT uiWirePointGroup=0);
};
class CUniWireModel{
public:
	CHashListEx<CXhWirePoint> hashWirePoints;	//CUniWirePoint
	CHashListEx<CWireConnPoint> hashConnPoints;
	CHashListEx<CPhaseWire>	hashPhaseWires;
	//���ݹҽ�������Ϣ���ɸ����ϵ�Ψһ�������(based serial 1)
	UINT ToWireSerial(WIREPLACE_CODE wireplace,WORD wiVoltage=0);
	struct CIRCUIT_LAYOUT{
		bool blCircuitDC;	//�Ƿ�Ϊֱ��
		BYTE cnCircuitCount;//��·��
		WORD wiVolt;//��ѹ�ȼ�(kV)
	public:
		CIRCUIT_LAYOUT(BYTE _cnCircuitCount=2,bool _blDC=false,WORD _wiVolt=500){
			blCircuitDC=_blDC;
			cnCircuitCount=_cnCircuitCount;
			wiVolt=_wiVolt;
		}
		BYTE get_cnWireCount(){
			if(wiVolt==0)
				return cnCircuitCount;
			else if(blCircuitDC)
				return cnCircuitCount*2;
			else
				return cnCircuitCount*3;
		}
		__declspec(property(get=get_cnWireCount)) BYTE cnWireCount;
	};
	BYTE m_cnEarthWireCount;//���߸���
	PRESET_ARRAY4<CIRCUIT_LAYOUT> layouts;
public:
	CUniWireModel();
	bool UpdateWirePointGimName(CUniWirePoint* pWirePoint);
	void InitWireLayout(BYTE cnCircuitCount,bool blCircuitDC=false,WORD wiVoltage=500,BYTE cnEarthWireCount=2);
	bool AppendWireLayout(BYTE cnCircuitCount,bool blCircuitDC=false,WORD wiVoltage=500);
};
