#pragma once
#include "material.h"
#include "Tower.h"
#include "MatStatRecord.h"
#include "excel9.h"
#include "stdlib.h"

class CTowerBOM
{
	void GetSegCoefHashTblBySegStr(char *segStr,CHashList<long> &segCoefHashSegI);
public:
	//使用列表
	ATOM_LIST<LEG_DESIGN> bodyList;
	ATOM_LIST<CManuPart> Parts;
	ATOM_LIST<CStatBolt> Bolts;
	CHashList<int>hashSegI;	//为防止0段号，段号+1为键值，索引序号为哈希表内存储值
	void ReadBodySheet(Range excel_range,_Worksheet excel_sheet);
	void ReadPartSheet(Range excel_range,_Worksheet excel_sheet);
	void ReadBoltSheet(Range excel_range,_Worksheet excel_sheet);
public:
	CTowerBOM();
	void Empty();
	void CopyFrom(CTowerBOM& srcBom);
	void ReadTextBOMFile(char* file_name);
	void WriteTextBOMFile(const char* file_name);
	void ReadExcelBOMFile(char* file_name);
	//void StatSegPart(int iSeg,CSegMaterialSummator* pSegMatSum,bool statPart,bool statBolt);
	void StatTower(ATOM_LIST<CMatStatRecord>* pTowerMatStatList);
};
extern CTowerBOM bom;
int GetClassTypeOrder(int idClassType);
