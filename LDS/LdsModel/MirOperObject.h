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
			BYTE ciMirFlag;		//0x01:X轴;0x02:Y轴;0x04:Z轴;0x08:旋转对称;0x10:平面镜像
			short siRotDegAng;	//旋转对称时的旋转角度
			DWORD dwStateFlag;	//当前对象的信息对称更新状态，根据pSrcObj类型有不同的含义
			CLDSDbObject* pMirObj;	//与pSrcObj遵循ciMirFlag对称规则的对称对象
			MIRMSG get_xMirMsg() const;
			__declspec(property(get=get_xMirMsg)) MIRMSG xMirMsg;
			bool IsHasState(DWORD dwFlag);
			DWORD RemoveState(DWORD dwFlag);
			MIROBJ() { dwStateFlag=siRotDegAng=ciMirFlag=0; }
		};
		GEPOINT xMirOrg,vMirNormal;	//平面镜像时的平面定义基点及法向，旋转轴方向或镜像对称平面法线
		SIMPLE_ARRAY<MIROBJ,PRESET_OBJS8> xarrMirObjs;
		MIROBJ* GetMirObj(MIRMSG mirmsg);
		MIRROR(CLDSDbObject* pObj=NULL) { hSrcObj=0;pSrcObj=pObj; }
		void SetKey(DWORD key) { hSrcObj=key; }
	};
	static CHashListEx<MIRROR> hashMirObjs;
	//MIRROR::MIROBJ::dwStateFlag枚举值
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
