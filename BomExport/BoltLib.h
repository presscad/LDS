#pragma once
#include "XhCharString.h"
#include "HashTable.h"
#include "BOM.h"

struct BOLT_LIB_REC{
public:
	CXhChar16 m_sSpec;
	int d,L;
	double m_fWeight;
	double m_fWeight2;
	BOLT_LIB_REC(){d=L=0;m_fWeight=m_fWeight2=0;}
};
class CBoltLib
{
public:
	BYTE m_cType;	//��˨������
	CHashList<double> m_xHashWeightByD;	//��¼ÿ5mm����
	CMapList<BOLT_LIB_REC> m_xMapBoltRec;
	CBoltLib(){m_cType=0;}
	double FindWeight(int d,int L,BOOL bTwoCap=FALSE);
};

class CBoltLibSet
{
private:
	bool ReadSheet(const char* file_path,BYTE bolt_type);
	
public:
	CBoltLibSet(void){;}
	~CBoltLibSet(void){;}

	CHashList<CBoltLib> m_xHashBoltsLib;
	static const BYTE TYPE_THIN_NUT		= 1;//����ĸ
	static const BYTE TYPE_FASTEN_NUT	= 2;//�۽���ĸ

	static const BYTE TYPE_COMMON_L					= 2;	//��ͨ��˨	˫ñ��˨(����)
	static const BYTE TYPE_COMMON_T					= 6;	//˫ñ��˨ ����������,1��ĸ1��ж��˨1ƽ��Ȧ��
	static const BYTE TYPE_COMMON_T_THIN			= 30;	//������˨ ��������˨Ϊ����ĸ������
	static const BYTE TYPE_COMMON_T_FASTEN			= 31;	//������˨ (��δ����)
	static const BYTE TYPE_FOOTNAIL_TWO_CAP_L		= 8;	//���ɽŶ�(�Ŷ���2��ĸ1����ĸ1ƽ��Ȧ,�Ŷ���2��ĸ1ƽ��Ȧ1���ɿ۽���ĸ)
	static const BYTE TYPE_FOOTNAIL_TWO_CAP_T		= 7;	//�����Ŷ�(�Ŷ���2��ĸ1��ж��ĸ1ƽ��Ȧ)
	static const BYTE TYPE_FOOTNAIL_THREE_CAP		= 36;	//�����Ŷ�(�Ŷ���3��ĸ1ƽ��Ȧ)
	static const BYTE TYPE_DIANQUAN					= 15;	//��Ȧ
	static const BYTE TYPE_DIANQUAN_SPRING			= 37;	//���ɵ�Ȧ
	double FindWeight(int d,int L,BYTE cType,BOOL bTwoCap=FALSE);
	double FindWeight(BOMBOLT &bolt,BYTE cBaseBoltType);
	bool ImportBoltLibByExcelFile(const char* file_path);
};

