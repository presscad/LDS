#pragma once
#include "WirePlaceCode.h"
#include "XhCharString.h"
#include "ArrayList.h"
#include "f_ent.h"
#include "Hashtable.h"
#include "Tower.h"	//"lds_part.h"
#include "PropListItem.h"	

class CSolidBody;
struct SOLID_OBJECT{
	HIBERID hiberid;
	long get_m_hPart(){return (long)hiberid.masterId;}
	long set_m_hPart(long hPartId){
		hiberid.masterId=(DWORD)hPartId;
		return hPartId;
	}
	// �����ľ��
	__declspec(property(put=set_m_hPart,get=get_m_hPart)) long m_hPart;
	int  m_idClassType;	// ����������
	GECS acs;			// ������װ������ϵ
	char ciLineType;		//0:����Ϊʵ�����ͣ�1:����Ϊֱ���͹�����2:����Ϊ�����͹���
	char blShowBriefLine;	//���͹����Ƿ���Ҫ�ڵ���ͼ����ʾ
	double dfSphereRadius;	//��ʾ����İ뾶
	GELINE line;		// ����״̬�µļ���
	//f3dArcLine arc;		// ����״̬�µĻ��߼���
	CSolidBody *pBody;		// ��ʾ������ʵ����ʾ����
};
//ͨ�ùҵ�ģ��(Universal wire point)
class CUniWirePoint {
protected:
	UINT m_id;
	bool m_bSolidModified;
public:
	GECS acs;						//��߲��ҵ�װ��λ�ã�Z���ʾ����������������
	double m_fHalfConeDegAngle;		//���Χ��׶��
	CXhChar50 m_szStyleName;	//���͹ҽ��������ƣ�����ɸѡ�ɹҽӴ����ͣ�,��DS(����˫������DSX(����˫����������DN���������Ŵ���
	CXhChar50 m_sGimName;			//gim�ƽ�ʱ������
	WIREPLACE_CODE m_xWirePlace;
	CSolidBody* pSolidBody;
public:	//���Զ���
	//�ҵ�λ��
	GEPOINT get_xPosition(){return acs.origin;}
	GEPOINT set_xPosition(const double* position){return acs.origin=position;}
	__declspec(property(put=set_xPosition,get=get_xPosition)) GEPOINT xPosition;
	//�Ҵ����ƣ�������Ժ�ڲ�����������
	CXhString get_szInsulatorStyleName(){ return CXhString(m_szStyleName,51); }
	CXhString set_szInsulatorStyleName(const char* szName){
		StrCopy(m_szStyleName,szName,51);
		return CXhString(m_szStyleName,51);
	}
	__declspec(property(put=set_szInsulatorStyleName,get=get_szInsulatorStyleName)) CXhString szInsulatorStyleName;
	//��������
	GEPOINT get_vWirePull(){return acs.axis_z;}
	GEPOINT set_vWirePull(const double* vPull){return acs.axis_z=vPull;}
	__declspec(property(put=set_vWirePull,get=get_vWirePull)) GEPOINT vWirePull;
	//�ҵ����
	UINT get_Id(){return m_id;}
	__declspec(property(get=get_Id)) UINT Id;
	//�ҵ�λ�ñ�ʶ����(ǰ|��)
	char get_ciWirePosSymbol() {
		if (m_xWirePlace.ciWireType == 'J' || m_xWirePlace.ciTensionType != 2)
			return 0;	//���߼����������߲�����ǰ��
		if (m_xWirePlace.ciWireDirection == 'Y')	//���·���ΪY���ҵ���X�ᵣ��
		{	//����GIM�淶Ҫ��:X��ᵣ��Y������Ϊǰ,����Ϊ��;���ǽ�ģ����ϵ��Y����GIMģ������ϵ��Y���Ƿ����
			if (xPosition.y < 0)
				return 'Q';
			else
				return 'H';
		}
		else //if(m_xWirePlace.ciWireDirection=='X') //���·���ΪX���ҵ���Y�ᵣ��
		{	//����GIM�淶Ҫ��:Y��ᵣ��X�Ḻ��Ϊǰ,����Ϊ��;�ҽ�ģ����ϵ��X����GIMģ������ϵ��X��ͬ��
			if (xPosition.x < 0)
				return 'Q';
			else
				return 'H';
		}
	}
	__declspec(property(get = get_ciWirePosSymbol)) char ciWirePosSymbol;
public:
	CUniWirePoint()
	{
		m_id=0;
		m_fHalfConeDegAngle=15;
		pSolidBody=NULL;
		m_bSolidModified=true;
	}
	~CUniWirePoint(){;}
	bool IsSolidModified(){return m_bSolidModified;}
	bool SetModified(bool modified){return m_bSolidModified=modified;}
	//
	void SetKey(DWORD key){m_id=key;}
#if !defined(DISABLE_SOLID_BODY)&&!defined(_HANDOVER_TO_CLIENT_)
	virtual bool Create3dSolidModel(UINT slices=6);
	virtual CSolidPart GetSolidPartObject(double dfArrowLength=2000);
#endif
};
//����ģ���йҵ㣬�������ӽ�߲���
class CXhWirePoint : public CUniWirePoint
{
protected:
	char _szLayer[4];
public:
	BYTE m_ciLevel;				//�ҵ㶨�弶��0.Mod(���߽ڵ�)|1.Solid(ʵ��ҿ�)
	long m_hRelaPart;			//�뵱ǰ���߹ҵ����������ʵ��;��˨(���׹ҵ�)|���(˫�׹ҵ�)
	long m_hRelaNode;			//�뵱ǰ���߹ҵ��������LDSģ�ͽڵ���
	bool m_bRelyAngle;			//�ҿ��Ƿ������ڹ��߽Ǹ��ϣ�һ�㲻�������ڽǸ��Ͼ��������ڰ���
	float m_fHoleD;				//�ҿ״�С
	float m_fSpaceWidth;		//�ҿ׼�ࣨ��߿��+��϶��
	GEPOINT m_vStdBasicAxisVec;	//hole[0]->hole[1]�Ľ��ת�᷽��
	GEPOINT m_xBasicPoint;		//hole[0]�����������ĵ㣨�ְ����Բ����ĵ㣩
	GEPOINT m_vSketchArrow;		//����ģ���µĹ��߷���ʾ���ͷ
	struct HOLE{
		long hBolt,hResidePart;
		HOLE(){hBolt=hResidePart=0;}
	}holes[2];	//ʵ��ҿף�һ����1~2�������
	CSolidBody* pSolidBody;
public:	//���Զ���
	char* get_szLayer();
	char* set_szLayer(const char* layer);
	__declspec(property(put=set_szLayer,get=get_szLayer)) char* szLayer;
public:
	CXhWirePoint();
	~CXhWirePoint();
	void CloneProp(CXhWirePoint* pSrcWriePoint);
	//
	virtual bool Create3dSolidModel(UINT slices=6);
	//������
	DECLARE_PROP_FUNC(CXhWirePoint);
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ
};
//�ҵ���飬Ϊ�Ҵ���һ�����ӷ�������1�������ҵ�����һ���Ҵ�
class CWirePointGroup{
public:
	UINT m_uiGroupId;		//�������
	UINT uiHangingStyle;	//���͹ҽ�����
	CXhChar16 m_sGroupName;
	PRESET_ARRAY4<long> m_xWirePointArr;	//ͬһ�ҵ���������֧��4���ҵ�
public:
	CWirePointGroup()
	{
		m_uiGroupId=0;
		m_xWirePointArr.Set(0,1);
	}
	~CWirePointGroup(){;}
};
//�Ҵ���֧��һ����߹Ҵ����������ж������ӷ���������ҵ���飩
class CInsulatorBranch
{	
public:
	static const UINT INSULATOR_DS	= 0x00000101;	//����˫��
	static const UINT INSULATOR_DSX	= 0x00010101;	//����˫������
	static const UINT INSULATOR_DN	= 0x00020001;	//��������
	UINT m_uiInsulatorStyle;		//���͹ҽ�����
	CXhChar16 m_sInsulatorName;		//���͹ҽ���������,��DS(����˫������DSX(����˫����������DN���������Ŵ���
	UINT m_idConnPoint;				//������װ�����ӵ�
	UINT m_uiBranchSerial;			//�Ҵ���֧���
	UINT m_uiActiveGroupId;			//���ڼ���״̬�Ĺҵ����
	ARRAY_LIST<CWirePointGroup> m_xWirePtGroupArr;
public:
	CInsulatorBranch();
	~CInsulatorBranch();
	//
	CWirePointGroup* GetActiveWirePointGroup();
};
//�����ߵ�װ�����ӵ㣬��1�������Ҵ���֧����
//Ŀǰֻ��һ��װ�����һ���Ҵ���֧���Ӵ���
class CWireConnPoint{
protected:
	UINT m_uiSerial;
public:
	UINT m_uiCircuitId;		//������·��
	UINT m_uiPhaseSerial;	//���������
	char m_ciWiretType;  	//�������ͣ�'E'����|'C'����|'J'����
	char m_ciHangingType;  	//���ߵ�ĹҴ���ʽ��0.��ͨ����|'S'.˫��|'V'.V��
	GEPOINT m_xPos;			//����װ���λ��
	CXhChar50 m_sDes;		//��������:����(ǰ�أ����)������(ǰ������)�����ߡ�
	ARRAY_LIST<CInsulatorBranch> m_xInsulatorBranchArr;
public:	//���Զ���
	//װ���������
	UINT get_Id(){return m_uiSerial;}
	__declspec(property(get=get_Id)) UINT Id;
public:
	CWireConnPoint();
	~CWireConnPoint();
	//
	void SetKey(DWORD key){m_uiSerial=key;}
	GEPOINT CalAssemblyPos();
};
//ÿ�ർ�߶�Ӧ��һ��ʵ�ʵĹҽӵ�
class CPhaseWire
{	
public:
	BYTE m_ciPhaseSerial;	//�����
	PRESET_ARRAY4<long> m_xConnPts;		//ÿ��������������������ӵ�(����ʱ��ǰ�����ӵ�)��1���������ӵ�
public:
	CPhaseWire() { ; }
	~CPhaseWire() { ; }
};
//���߻�·
class CWireCircuit
{
public:
	UINT m_uiSerilaId;	//��·���
	BYTE m_ciCurrent;	//���������ͣ�0.DC(ֱ��)|1.AC(����)
	WORD m_wiVoltage;	//����ѹ,��λKV
	BYTE m_ciForceType;	//����������ʽ��0.����|1.����
	CXhChar50 m_sName;	//��·����
	ARRAY_LIST<CPhaseWire> m_xPhaseWireArr;	//ͬһ��·�����������
public:
	CWireCircuit();
	~CWireCircuit(){;}
	//
	UINT GetHangWireCount();
	UINT GetConnPointCount();
};
class CEarthCircuit
{
public:
	PRESET_ARRAY4<long> m_xConnPts;		//����·��������������������ӵ�(����ʱ��ǰ�����ӵ�)
public:
	CEarthCircuit() { ; }
	~CEarthCircuit() { ; }
};
//ͨ�ùҵ�����ģ��
class CUniWireModel{
public:
	bool m_bGimWireModelInherited;	//�Ѿ���֮ǰTowerģ���а�GIM�ҵ���Ϣ�̳й��� wjh-2019.4.18
	BYTE m_cnEarthWireCount;	//���߸���
	CXhChar50 m_sTowerType;		//��������
	CHashListEx<CXhWirePoint> m_hashWirePoints;
	CHashListEx<CWireConnPoint> m_hashConnPoints;
	ATOM_LIST<CWireCircuit> m_listWireCircuits;		//���߻�·
	ATOM_LIST<CEarthCircuit> m_listEarthCircuits;	//���߻�·
public:
	CUniWireModel();
	~CUniWireModel(){;}
	//
	void Empty();
	bool UpdateWirePointGimName(CUniWirePoint* pWirePoint);
	bool ExportUniWireXmlData(const char* sFile);
	bool ImPortUniWireXmlData(const char* sFile);
	void FromBuffer(CBuffer& buffer);
	void ToBuffer(CBuffer& buffer);
	//
	static CXhChar50 GetWirePtGimName(WIREPLACE_CODE wireplace,GEPOINT pos);
};
