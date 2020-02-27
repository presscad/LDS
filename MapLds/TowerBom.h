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
	//ʹ���б�
	ATOM_LIST<LEG_DESIGN> bodyList;
	ATOM_LIST<CManuPart> Parts;
	ATOM_LIST<CStatBolt> Bolts;
	CHashList<int>hashSegI;	//Ϊ��ֹ0�κţ��κ�+1Ϊ��ֵ���������Ϊ��ϣ���ڴ洢ֵ
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
