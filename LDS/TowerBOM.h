// TowerBOM.h: interface for the CTowerBOM class.
//
//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#ifdef __ANGLE_PART_INC_

#include "Tower.h"
#include "BOM\\Bom.h"
#include "HashTable.h"

class CTowerBOM
{
	CTower *m_pTower;	//记录当前正在统计的塔型
	//为统计部件引用和统计塔型数据代码共用，记录IModel指针，仅在设计呼高时使用m_pTower
	//统计构件时统一使用m_pModel，故在统计部件内构件时仅需要切换m_pModel为当前部件指针，
	//并设置部件内构件段号及配材即可，统计完部件后再将m_pModel切换为m_pTower wht 12-09-13
	IModel *m_pModel;
protected:
	CSuperHashStrList<BOMPART> hashParts;	//构件记录集合
	CHashStrList<BOMBOLT> hashBolts;	//螺栓记录集合
	ATOM_LIST<BOM_WELDPART> listWeldParts;	//组焊件
	bool m_bStatLsBySegI;
public:
	CTowerBOM();
private:
	void SummarizeParts();
	void SummarizeOnePart(CLDSPart *pPart,CTower* pTower=NULL);	//输出选中构件列表时用来进行材料统计
	void SummarizeOneLineAngle(CLDSLineAngle *pLineAngle);
	void SummarizeOneLineTube(CLDSLineTube *pTube);
	void SummarizeOneLineSlot(CLDSLineSlot *pLineSlot);
	void SummarizeOneBolt(CLDSBolt *pBolt);
	void SummarizeOnePlate(CLDSPlate *pPlate);
	void SummarizeOneParamPlate(CLDSParamPlate *pParamPlate);
	void SummarizeOnePolyPart(CLDSPolyPart *pPolyPart);
	void SummarizeSuppPart();	//统计附加构件
	void SummarizePartGroup();	//统计组焊件
	void SummarizeBlockRefs();
	void SummarizeOneBlockRef(CBlockReference *pBlockRef);
public:
	BOOL RunSummarize(CTower* pTower,bool bIncLsSegI=false);	//进行材料统计
	void CloneToModelBOM(CModelBOM* pBom);
	void CloneFromModelBom(CModelBOM* pBom);
	//
	CXhChar50 m_sBomName;
	BOMPART* AddBomPart(const char* sPartNo,int cls_id){return hashParts.Add(sPartNo,cls_id);}
	BOMPART* FromPartNo(const char* sPartNo){return hashParts.GetValue(sPartNo);}
	BOMPART* EnumFirstPart(){return hashParts.GetFirst();}
	BOMPART* EnumNextPart(){return hashParts.GetNext();}
	int GetPartNum(){return hashParts.GetNodeNum();}
};
//#endif
