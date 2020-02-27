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
	BYTE cbMatchFlag;	//0x00˾��ͼ�е�����ڵ�;0x01˾��ͼ�и˼��˽ڵ�;0x02������άģ������Ӧ�ڵ����ƥ��ӳ��
	BYTE ciMatchLevel;	//0.�ڵ�����ƥ��;1.�˹�ָ�����ƥ��;2.�����ͬ�Զ�ƥ��
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
	UINT idClsType;		//�˼���������Id
	char ciMaterial;
	char ciSectType;	//'L','F'
	char cSubSectType;	//'L','T','D','X'
	WORD wWide,wThick,wHeight; 
	CFGWORD cfgword;
	CCmdDrawNode *pStart,*pEnd;
	BYTE ciStartJointType,ciEndJointType;	//JOINT_HINGE(0),JOINT_RIGID(1),JOINT_AUTOJUSTIFY(2);
	BYTE ciStartForceType,ciEndForceType;	//CENTRIC_FORCE(0);ECCENTRIC_FORCE(1)
	struct BOLT{
		WORD d,N;		//��˨ֱ��������
		char grade[5];	//��˨����
		BOLT(){d=20;N=2;grade[0]=grade[1]=grade[2]=grade[3]=grade[4]=0;}
	}startBolt,endBolt;
	struct MATCHED_ROD{
		BYTE cbMatchFlag;	//0x01:��pRodʼ��ƥ��;0x02:��pRod�ն�ƥ��;0x80��pRodʼĩ�˷���ƥ��
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
