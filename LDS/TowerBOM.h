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
	CTower *m_pTower;	//��¼��ǰ����ͳ�Ƶ�����
	//Ϊͳ�Ʋ������ú�ͳ���������ݴ��빲�ã���¼IModelָ�룬������ƺ���ʱʹ��m_pTower
	//ͳ�ƹ���ʱͳһʹ��m_pModel������ͳ�Ʋ����ڹ���ʱ����Ҫ�л�m_pModelΪ��ǰ����ָ�룬
	//�����ò����ڹ����κż���ļ��ɣ�ͳ���겿�����ٽ�m_pModel�л�Ϊm_pTower wht 12-09-13
	IModel *m_pModel;
protected:
	CSuperHashStrList<BOMPART> hashParts;	//������¼����
	CHashStrList<BOMBOLT> hashBolts;	//��˨��¼����
	ATOM_LIST<BOM_WELDPART> listWeldParts;	//�麸��
	bool m_bStatLsBySegI;
public:
	CTowerBOM();
private:
	void SummarizeParts();
	void SummarizeOnePart(CLDSPart *pPart,CTower* pTower=NULL);	//���ѡ�й����б�ʱ�������в���ͳ��
	void SummarizeOneLineAngle(CLDSLineAngle *pLineAngle);
	void SummarizeOneLineTube(CLDSLineTube *pTube);
	void SummarizeOneLineSlot(CLDSLineSlot *pLineSlot);
	void SummarizeOneBolt(CLDSBolt *pBolt);
	void SummarizeOnePlate(CLDSPlate *pPlate);
	void SummarizeOneParamPlate(CLDSParamPlate *pParamPlate);
	void SummarizeOnePolyPart(CLDSPolyPart *pPolyPart);
	void SummarizeSuppPart();	//ͳ�Ƹ��ӹ���
	void SummarizePartGroup();	//ͳ���麸��
	void SummarizeBlockRefs();
	void SummarizeOneBlockRef(CBlockReference *pBlockRef);
public:
	BOOL RunSummarize(CTower* pTower,bool bIncLsSegI=false);	//���в���ͳ��
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
