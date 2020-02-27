#ifndef __TDA_LS_REF_LIST_H_
#define __TDA_LS_REF_LIST_H_

#include "macro.h"
#ifdef __COMMON_PART_INC_
typedef struct tagBOLT_RECORD{
	char grade[8];
	long d,num;
	double L;
	long m_hFamily;			// 螺栓规格系列标识
	BOOL m_bAntiTheft;	// 是否防盗螺栓
	BOOL m_bAntiLoose;	// 是否带弹簧垫
	//BOOL m_bTwoCapLs;		// 双帽螺栓
  	//BOOL m_bFootNail;		// 脚钉(TRUE)或螺栓(FALSE)
}BOLT_RECORD;
class CLsRefList : public CHashList<CLsRef>
{
//1.私有成员变量定义
	BOOL *m_pModified;	//修改标识
//2.公有成员变量定义
public:
	CLsRefList(){m_pModified=NULL;}
	~CLsRefList(){;}

	//THANDLE handle;
	//THANDLE attach_part_handle;	//所依附构件的句柄如角钢节点板
	//int single1_or_double2;	// 单排螺栓1或双排螺栓2
//3.私有成员函数定义
private:
//4.公有成员函数定义

public:
	void ResetBoltRefRayNo(CLDSPart *pPart);	//传入参数pPlate用来处理端连接的情况 wht 11-06-30
	bool MergeBolts(CLDSBolt *pFromBolt,CLDSBolt *pDestBolt);	//将pFromBolt合并到pDestBolt
	int GetLsString(char* ls_str);
	int GetBoltRecordArr(BOLT_RECORD *recArr=NULL);
	void SetModifiedFlagAddress(BOOL *pModified)
	{m_pModified = pModified;}
	int GetLsCount(short d=0);
	int GetLocalLsCount(short start1_end2_mid0=0);	//得到局部螺栓数 始端/终端/中间
	void FromBuffer(CBuffer &buffer,IModel *pTower,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//void Serialize(CArchive& ar);
	//CLsRef* append();		//在节点链表的末尾添加节点
	CLsRef* append(CLsRef Ls);//在节点链表的末尾添加节点
	//base点表示螺栓的基点位置(相对坐标系下),ucs表示一个螺栓的所在的相对坐标系,
	//此坐标系必须有一轴线与螺栓的工作法线相靠近,iNormX0Y1Z2就表示
	//与螺栓工作法线相靠近的相对坐标系的坐标轴
	CLsRef* FindLsByUcsPos(f3dPoint base,UCS_STRU ucs,int iNormX0Y1Z2,double epsilon=EPS2);
	CLsRef* FindLsByPos(f3dPoint base,f3dPoint work_norm,double epsilon=EPS2);
	BOOL DeleteNode(THANDLE handle);
	CLsRef* FromHandle(THANDLE);
};
#endif
#endif