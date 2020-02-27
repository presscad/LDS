// PartStatRecord.h: interface for the CPartStatRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTSTATRECORD_H__C695139F_B126_4004_B2CF_E98CEE8C7E59__INCLUDED_)
#define AFX_PARTSTATRECORD_H__C695139F_B126_4004_B2CF_E98CEE8C7E59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPartStatRecord  
{
public:
	char sPartNo[16];	//构件编号
	char spec[24];		//规格
	char cMaterial;		//材质简化字符
	char ciQualityLevel;//质量等级，0表示未特殊指定质量等级，其余应为'B','C','D'等字符
	int num;			//单元件数（对整塔的统材指单基件数，对于组合构件则是指一套组合构件内的件数）
	int idClassType;	//记录所对应的构件类型
	double width;		//宽度(mm)
	double length;		//长度(mm)
	double piece_weight;//单件重
	char sNotes[50];	//备注信息
	int numPerTower;	//单基数（青岛东方组焊图子件钢印号要与单件工艺卡对应）	wjh-2019.5.12
	__declspec(property(get = get_numPerTower)) int numPerTower;
	CPartStatRecord();
	~CPartStatRecord(){;}
};

#endif // !defined(AFX_PARTSTATRECORD_H__C695139F_B126_4004_B2CF_E98CEE8C7E59__INCLUDED_)
