// MatStatRecord.h: interface for the CMatStatRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATSTATRECORD_H__05B6B300_19B8_493A_A6DA_94AE78DC4FA4__INCLUDED_)
#define AFX_MATSTATRECORD_H__05B6B300_19B8_493A_A6DA_94AE78DC4FA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMatStatRecord  
{
	int m_nSegN;	//�ܶ���
	int m_nBodyN;	//������(����������)
	double *body_weight_arr;
	double *seg_weight_arr;
public:
	int idClassTypeOrder;	//��¼����Ӧ�Ĺ�������˳���
	char cMaterial;			//����
	char spec[24];			//���
	CMatStatRecord();
	~CMatStatRecord();
	double GetSegWeight(int iSegIndex);//����ֵ��0Ϊ����
	double GetBodyWeight(int iBody);//����ֵ��0Ϊ����
	BOOL SetSegWeight(int iSegIndex,double weight);
	BOOL SetBodyWeight(int iBody,double weight);
	BOOL AddSegWeight(int iSegIndex,double weight);
	void AddBodyWeight(char *limit_str,double weight,BOOL bExclusive=FALSE);
	BOOL Create(int nSegN,int nBodyN);
};

#endif // !defined(AFX_MATSTATRECORD_H__05B6B300_19B8_493A_A6DA_94AE78DC4FA4__INCLUDED_)
