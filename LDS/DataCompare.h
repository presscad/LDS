#pragma once
#include "ProcessPart.h"
#include "f_ent_list.h"
#include "XhCharString.h"
#include "LogFile.h"
#include "TowerBOM.h"
#include "Variant.h"
#include "LocalFeatureDef.h"
#include "LicFuncDef.h"

#if defined(__TMA_)||defined(__LMA_)
//////////////////////////////////////////////////////////////////////////
class CDataCmpModel
{
public:
	struct COMPARE_PART_RESULT
	{
		BOMPART *pOrgPart;
		BOMPART *pLoftPart;
		WORD wSumMapCount;	//ģ�͹�����ͼֽ�������ڶ��һӰ��ʱ����������ʾ����Ӱ�乹��������
		CHashStrList<BOOL> hashBoolByPropName;
		COMPARE_PART_RESULT(){pOrgPart = NULL;pLoftPart = NULL;wSumMapCount=0;};
	};
	CTowerBOM m_loftData;		//��άģ���з�����ϸ����
private:					
	ATOM_LIST<CTowerBOM> m_drawingDataList;	//ԭʼͼֽ��ϸ�����б�
	CHashStrList<COMPARE_PART_RESULT> m_hashCompareResultByPartNo;
	CTowerBOM* ImportExcelBomFileByDllFromat(CVariant2dArray &sheetContextMap);
	CTowerBOM* ImportExcelBomFileCore(CVariant2dArray &sheetContentMap,CHashStrList<DWORD> &hashColIndexByColTitle,int startRowIndex);
public:
	CDataCmpModel();
	~CDataCmpModel();
	//
	BOOL Empty();
	void ReadFromDrawAndBomBuffer(CBuffer* pBuffer);
	void WriteToDrawAndBomBuffer(CBuffer* pBuffer);
	CTowerBOM* AppendDrawingDataFromBOMD(const char* sFileName);
	CTowerBOM* AppendDrawingDataFromTBOM(const char* sFileName);
	CTowerBOM* AppendDrawingDataFromExce(const char* sFileName);
	CTowerBOM* EnumFirstDrawingData(){return m_drawingDataList.GetFirst();}
	CTowerBOM* EnumNextDrawingData(){return m_drawingDataList.GetNext();}
	UINT GetExterDrwBomPartsCount();
	void DeleteDwgBomFile(CTowerBOM *pBom);
	//У�����
	int ComparePartData(double LEN_TOLERANCE=5,double WEIGTH_TOLERANCE=0.5);
	void AddBomResultSheet(LPDISPATCH pSheet,int iSheet);
	void ExportCompareResult();
	DWORD GetResultCount(){return m_hashCompareResultByPartNo.GetNodeNum();}
	COMPARE_PART_RESULT* GetResult(const char* part_no){return m_hashCompareResultByPartNo.GetValue(part_no);}
	COMPARE_PART_RESULT* EnumFirstResult(){return m_hashCompareResultByPartNo.GetFirst();}
	COMPARE_PART_RESULT* EnumNextResult(){return m_hashCompareResultByPartNo.GetNext();}
	BOMPART* GetDrawingBomPart(const char* szPartLabel,BYTE cls_id=0);	//���ݹ�����Ų��ҵ����ʶ���ͼֽ�����е�BOM��ϸ
	//
	static void RestoreSpec(const char* spec,double *pfWidth,double *pfThick,char *matStr=NULL);
};
extern CDataCmpModel globalDataCmpModel;
#endif
