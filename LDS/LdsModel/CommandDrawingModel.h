#pragma once

#include "f_ent.h"
#include "HashTable.h"
#include "Buffer.h"
#include "IModel.h"

class CCommandDrawingModel;
class CCmdDrawNode
{
protected:
	long handle;
	CCommandDrawingModel* m_pModel;
public:
	BYTE cbMatchFlag;	//0x00司令图中的冗余节点;0x01司令图中杆件端节点;0x02可与三维模型中相应节点进行匹配映射
	BYTE ciMatchLevel;	//0.节点坐标匹配;1.人工指定句柄匹配;2.句柄相同自动匹配
	long pointI;
	CFGWORD cfgword;
	GEPOINT pos;
	BYTE m_cPosCalType,m_cPosCalViceType;
	long arrRelationNode[4];
	double attach_scale,attach_offset;
	long get_Id(){return handle;}
	__declspec(property(get=get_Id)) long Id;
public:
	CCmdDrawNode();
	void SetKey(DWORD key){handle=key;}
	void SetBelongModel(CCommandDrawingModel *pModel){m_pModel=pModel;}
	void RetrieveCdfInfo(CLDSNode* pNode);
};
class CCmdDrawRod
{
protected:
	CCommandDrawingModel* m_pModel;
public:
	long handle;
	UINT idClsType;		//杆件对象类型Id
	char ciMaterial;
	char ciSectType;	//'L','F'
	char cSubSectType;	//'L','T','D','X'
	WORD wWide,wThick,wHeight; 
	CFGWORD cfgword;
	CCmdDrawNode *pStart,*pEnd;
	BYTE ciStartJointType,ciEndJointType;	//JOINT_HINGE(0),JOINT_RIGID(1),JOINT_AUTOJUSTIFY(2);
	BYTE ciStartForceType,ciEndForceType;	//CENTRIC_FORCE(0);ECCENTRIC_FORCE(1)
	struct BOLT{
		WORD d,N;		//螺栓直径及数量
		char grade[5];	//螺栓级别
		BOLT(){d=20;N=2;grade[0]=grade[1]=grade[2]=grade[3]=grade[4]=0;}
	}startBolt,endBolt;
	struct MATCHED_ROD{
		BYTE cbMatchFlag;	//0x01:与pRod始端匹配;0x02:与pRod终端匹配;0x80与pRod始末端反向匹配
		CLDSLinePart* pRod;
		MATCHED_ROD(){pRod=NULL;cbMatchFlag=0;}
	}matchrod;
	CCmdDrawRod(){pStart=pEnd=NULL;}
	void SetKey(DWORD key){handle=key;}
	void SetBelongModel(CCommandDrawingModel *pModel){m_pModel=pModel;}
	void RetrieveCdfInfo(CLDSLinePart* pRod);
};
typedef CCmdDrawRod* CCmdDrawRodPtr;
struct LINEAR_VECTOR{
	DWORD id;
	double length;
	GEPOINT vStdLenVec,xStartPoint;
	void SetKey(DWORD key){id=key;}
	LINEAR_VECTOR(){length=id=0;}
	LINEAR_VECTOR(CCmdDrawNode* pStart,CCmdDrawNode* pEnd);
	LINEAR_VECTOR(CCmdDrawRod* pRod){Init(pRod);}
	bool IsInited(){return length>EPS;}
	bool Init(CCmdDrawNode* pStart,CCmdDrawNode* pEnd);
	bool Init(CCmdDrawRod* pRod);
};
class CCommandDrawingModel
{
public:
	CHashListEx<CCmdDrawNode>hashNodes;
	CHashListEx<CCmdDrawRod>hashRods;
	CCommandDrawingModel(void);
	virtual ~CCommandDrawingModel(void);
	void CheckModel(IModel* pTower,CHashList<long>* pHashPointIById=NULL,const char* exportMatchNodeFileName2=NULL,const char* segIncStr=NULL,CHashList<long>* pHashIdByPointI=NULL);
};
