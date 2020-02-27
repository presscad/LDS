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
	int m_nSegN;	//总段数
	int m_nBodyN;	//本体数(即基本高数)
	double *body_weight_arr;
	double *seg_weight_arr;
public:
	int idClassTypeOrder;	//记录所对应的构件类型顺序号
	char cMaterial;			//材质
	char spec[24];			//规格
	CMatStatRecord();
	~CMatStatRecord();
	double GetSegWeight(int iSegIndex);//索引值以0为基数
	double GetBodyWeight(int iBody);//索引值以0为基数
	BOOL SetSegWeight(int iSegIndex,double weight);
	BOOL SetBodyWeight(int iBody,double weight);
	BOOL AddSegWeight(int iSegIndex,double weight);
	void AddBodyWeight(char *limit_str,double weight,BOOL bExclusive=FALSE);
	BOOL Create(int nSegN,int nBodyN);
};

#endif // !defined(AFX_MATSTATRECORD_H__05B6B300_19B8_493A_A6DA_94AE78DC4FA4__INCLUDED_)
