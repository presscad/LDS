#ifndef MATERIAL_H
#define MATERIAL_H
#include "XhCharString.h"
#include "SegI.h"

//呼高
typedef struct tagUSER_LEGDESIGN
{
	char sHeight[20];
	CXhChar50 bodySegStr;	//本体段号范置
	CXhChar50 legStrArr[4];	//接腿段号范置
	int coef[4];	//接腿个数（每个象限算一个腿，即统材系数）
	tagUSER_LEGDESIGN(){memset(this,0,sizeof(tagUSER_LEGDESIGN));}
}LEG_DESIGN;
//工件
class CManuPart
{
public:
	SEGI nSeg;//段号
	char sPartNo[16];//编号
	char sMaterial[20];//材质
	char cMaterial;	//材质
	char sSpec[20];//规格
	double length;//长度
	int nPartNum;//数量
	double singWeight;//单重
	double sumWeight;//总重
	char sNote[20];//备注
	int part_type;	//构件类型:0.角钢;1.钢板
	int nCount;		//临时使用
public:
	CManuPart(){memset(this,0,sizeof(CManuPart));}
	//CManuPart& operator=(CManuPart& manuPart); 
};
//螺栓
class CStatBolt
{
public:
	SEGI nSeg;//段号
	char sSpec[20];//级别规格
	int nCount;//数量
	double weight;//重量
	int type;//类型
	char sNote[20];//备注
	int part_type;
public:
	char get_cMaterial(){
		char cMaterial;
		if(type==2)	//垫圈
			cMaterial='S';
		else
		{
			CXhChar50 grade(sSpec);
			char* separator=strchr(grade,'M');
			if(separator)
				*separator=0;	//钢板在材料汇总表中的规格只需要板厚即可
			else	//缺少级别时（可能是一些特殊件如U型螺丝），归为6.8级分类 wjh-2018.10.10
				grade.Copy("6.8");
			if(type==1)
			{	//脚钉
				if(grade.Equal("4.8"))
					cMaterial='H';
				else if(grade.Equal("5.8"))
					cMaterial='I';
				else if(grade.Equal("6.8"))
					cMaterial='J';
				else if(grade.Equal("8.8"))
					cMaterial='K';
				else //if(grade.Equal("10.9"))
					cMaterial='L';
			}
			else if(type==3)
			{	//双帽
				if(grade.Equal("4.8"))
					cMaterial='a';
				else if(grade.Equal("5.8"))
					cMaterial='b';
				else if(grade.Equal("6.8"))
					cMaterial='c';
				else if(grade.Equal("8.8"))
					cMaterial='d';
				else //if(grade.Equal("10.9"))
					cMaterial='e';
			}
			else
			{
				if(grade.Equal("4.8"))
					cMaterial='A';
				else if(grade.Equal("5.8"))
					cMaterial='B';
				else if(grade.Equal("6.8"))
					cMaterial='C';
				else if(grade.Equal("8.8"))
					cMaterial='D';
				else //if(grade.Equal("10.9"))
					cMaterial='E';
			}
		}
		return cMaterial;
	}
	__declspec(property(get=get_cMaterial)) char cMaterialSymbol;	//根据type及螺栓级别获取对应的CMatStatRecord::cMaterial属性
	CStatBolt(){memset(this,0,sizeof(CStatBolt));}
	//CStatBolt& operator=(CStatBolt& statBolt);
}; 
#endif