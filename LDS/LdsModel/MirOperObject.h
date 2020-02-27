#pragma once
#include "HashTable.h"
#include "XhLdsLm.h"

class CLDSDbObject;
class CMirOperObject
{
	bool m_bObjectStart;
	static bool m_bGlobalMirrorStart;
public:
	struct MIRROR{
		long hSrcObj;
		CLDSDbObject* pSrcObj;
		struct MIROBJ {
			BYTE ciMirFlag;		//0x01:X��;0x02:Y��;0x04:Z��;0x08:��ת�Գ�;0x10:ƽ�澵��
			short siRotDegAng;	//��ת�Գ�ʱ����ת�Ƕ�
			DWORD dwStateFlag;	//��ǰ�������Ϣ�ԳƸ���״̬������pSrcObj�����в�ͬ�ĺ���
			CLDSDbObject* pMirObj;	//��pSrcObj��ѭciMirFlag�Գƹ���ĶԳƶ���
			MIRMSG get_xMirMsg() const;
			__declspec(property(get=get_xMirMsg)) MIRMSG xMirMsg;
			bool IsHasState(DWORD dwFlag);
			DWORD RemoveState(DWORD dwFlag);
			MIROBJ() { dwStateFlag=siRotDegAng=ciMirFlag=0; }
		};
		GEPOINT xMirOrg,vMirNormal;	//ƽ�澵��ʱ��ƽ�涨����㼰������ת�᷽�����Գ�ƽ�淨��
		SIMPLE_ARRAY<MIROBJ,PRESET_OBJS8> xarrMirObjs;
		MIROBJ* GetMirObj(MIRMSG mirmsg);
		MIRROR(CLDSDbObject* pObj=NULL) { hSrcObj=0;pSrcObj=pObj; }
		void SetKey(DWORD key) { hSrcObj=key; }
	};
	static CHashListEx<MIRROR> hashMirObjs;
	//MIRROR::MIROBJ::dwStateFlagö��ֵ
	static const DWORD MIRBOLT_POS_PARAM = 0x01;
	static const DWORD MIRBOLT_NORM_PARAM= 0x02;
	static const DWORD MIRBOLT_ALL		 = 0x03;
	static DWORD FullStateOf(CLDSDbObject* pObj);
	static MIRROR* AddOrGetMirror(long hSrcObj,CLDSDbObject* pSrcObj=NULL) { 
		MIRROR* pMirror=hashMirObjs.Add(hSrcObj);
		pMirror->hSrcObj=hSrcObj;
		pMirror->pSrcObj=pSrcObj; 
		return pMirror;
	}
	static CLDSDbObject* GetInnerMirObj(long hSrcObj,MIRMSG mirmsg);
	static bool ComplementMirInfo(MIRROR* pMirror,int indexOfMirObj);
public:
	CMirOperObject();
	~CMirOperObject();
	void Finished();
};
