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
	char sPartNo[16];	//�������
	char spec[24];		//���
	char cMaterial;		//���ʼ��ַ�
	char ciQualityLevel;//�����ȼ���0��ʾδ����ָ�������ȼ�������ӦΪ'B','C','D'���ַ�
	int num;			//��Ԫ��������������ͳ��ָ����������������Ϲ�������ָһ����Ϲ����ڵļ�����
	int idClassType;	//��¼����Ӧ�Ĺ�������
	double width;		//���(mm)
	double length;		//����(mm)
	double piece_weight;//������
	char sNotes[50];	//��ע��Ϣ
	int numPerTower;	//���������ൺ�����麸ͼ�Ӽ���ӡ��Ҫ�뵥�����տ���Ӧ��	wjh-2019.5.12
	__declspec(property(get = get_numPerTower)) int numPerTower;
	CPartStatRecord();
	~CPartStatRecord(){;}
};

#endif // !defined(AFX_PARTSTATRECORD_H__C695139F_B126_4004_B2CF_E98CEE8C7E59__INCLUDED_)
