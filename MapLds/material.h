#ifndef MATERIAL_H
#define MATERIAL_H
#include "XhCharString.h"
#include "SegI.h"

//����
typedef struct tagUSER_LEGDESIGN
{
	char sHeight[20];
	CXhChar50 bodySegStr;	//����κŷ���
	CXhChar50 legStrArr[4];	//���ȶκŷ���
	int coef[4];	//���ȸ�����ÿ��������һ���ȣ���ͳ��ϵ����
	tagUSER_LEGDESIGN(){memset(this,0,sizeof(tagUSER_LEGDESIGN));}
}LEG_DESIGN;
//����
class CManuPart
{
public:
	SEGI nSeg;//�κ�
	char sPartNo[16];//���
	char sMaterial[20];//����
	char cMaterial;	//����
	char sSpec[20];//���
	double length;//����
	int nPartNum;//����
	double singWeight;//����
	double sumWeight;//����
	char sNote[20];//��ע
	int part_type;	//��������:0.�Ǹ�;1.�ְ�
	int nCount;		//��ʱʹ��
public:
	CManuPart(){memset(this,0,sizeof(CManuPart));}
	//CManuPart& operator=(CManuPart& manuPart); 
};
//��˨
class CStatBolt
{
public:
	SEGI nSeg;//�κ�
	char sSpec[20];//������
	int nCount;//����
	double weight;//����
	int type;//����
	char sNote[20];//��ע
	int part_type;
public:
	char get_cMaterial(){
		char cMaterial;
		if(type==2)	//��Ȧ
			cMaterial='S';
		else
		{
			CXhChar50 grade(sSpec);
			char* separator=strchr(grade,'M');
			if(separator)
				*separator=0;	//�ְ��ڲ��ϻ��ܱ��еĹ��ֻ��Ҫ��񼴿�
			else	//ȱ�ټ���ʱ��������һЩ�������U����˿������Ϊ6.8������ wjh-2018.10.10
				grade.Copy("6.8");
			if(type==1)
			{	//�Ŷ�
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
			{	//˫ñ
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
	__declspec(property(get=get_cMaterial)) char cMaterialSymbol;	//����type����˨�����ȡ��Ӧ��CMatStatRecord::cMaterial����
	CStatBolt(){memset(this,0,sizeof(CStatBolt));}
	//CStatBolt& operator=(CStatBolt& statBolt);
}; 
#endif