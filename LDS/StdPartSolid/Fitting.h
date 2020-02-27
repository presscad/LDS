#pragma once
#include "..\Global.h"

static const UINT TYPE_EB = 0x00000001;
static const UINT TYPE_GD = 0x00000002;
static const UINT TYPE_UR = 0x00000003;
static const UINT TYPE_US = 0x00000004;	//U����˿(U��UJ-d-L)
static const UINT TYPE_UB = 0x00000005;	//U�͹Ұ�(UB-�ȼ�)

//���
struct FITTING_GD{	
	static UINT GetTypeId() { return TYPE_GD; }
public:
	char   szSizeCode[16];	//�ͺŴ���
	short  siThick;			//�Ұ���b
	short  siThroughThick;	//Ԥ�����߽Ǹ�֫(ͨ)��
	double dfTrunnionD;		//����ֱ��
	double dfHoleD;			//�ҿ�ֱ��(diameter)��
	double dfBoardH;		//��˨�����ĵ���Բ���ĳ�H
	double dfBoardH1;		//���ᵽ��Բ���ĳ�H1
	double dfBoardH2;		//�ҿ׵����������߾���H2
	double dfTrunnionL;		//�����ܳ�
	double dfBoardL;		//���(�Ұ岿��)��ȣ���Բ����
};
struct FITTING_EB{
	static UINT GetTypeId() { return TYPE_EB; }
public:
	char   szSizeCode[16];	//�ͺŴ���
	short  siThick;			//�Ұ���b
	short  siThroughThick;	//Ԥ�����߽Ǹ�֫(ͨ)��
	double dfTrunnionD;		//����ֱ��
	double dfHoleD;			//�ҿ�ֱ��
	double dfBoardL;		//���(�Ұ岿��)���
	double dfBoardH;		//�ҿ׵����������߾���H
	double dfChinR;			//�ҿ��°�Բ���뾶
};
struct FITTING_UR{
	static UINT GetTypeId() { return TYPE_UR; }
public:
	char szSizeCode[16];
	char ciType;			//0.��ͨ��U�ιһ�;1.�ӳ���U�ιһ�
	double dfNamedL;		//���峤(ת�������������ڲ�)L��mm
	double dfShaftD;		//ת����˨ֱ��d
	double dfMdEdgeSpace;	//U�ͻ���ת����ĸ������ı߾�D��mm
	double dfT;				//U�ͻ���EB��GD�Ұ������Ӳ��������ӱں�T��mm
	double dfRingBs;		//U�ͻ���ֱ��B(s),
	double dfBoardSpaceC;	//U�ͻ���EB��GD�Ұ������Ӽ�϶(>=EB/GD�Ұ���)
	double dfRadiusOfRing;	//U�ͻ�Բ���ڲ�뾶R��һ��>=dfBoardSpaceC/2
};
struct FITTING_US{
	static UINT GetTypeId() { return TYPE_US; }
public:
	char szSizeCode[16];
	char ciType;			//0.��ͨ��U����˿;1.��ǿ��UJ����˿
	double dfNamedC;		//U����˿������(�������ҿ׼����Ŀ��)C��mm
	double dfColumnM;		//U����˿ֱԲ��ֱ��
	double dfColumnd;		//U����˿����Բ��ֱ��
	double dfColumnL1;		//U����˿Բ����
	double dfColumnL2;		//U����˿��������˨�ײ�����L2
	short  siThroughThick;	//Ԥ�����߽Ǹ�֫(ͨ)��S
};
struct FITTING_UB{
	static UINT GetTypeId() { return TYPE_UB; }
public:
	char szSizeCode[16];
	char ciType;			//0.��ͨ��UB�Ұ�;1.�ӳ���UB�Ұ�
	double dfNamedL;		//U�͹Ұ��ڲ���C
	double dfTrunnionD;		//ת��ֱ��
	double dfBoardH;		//�Ұ�߶�(ת��������Բ��Բ��)
	double dfBoardW;		//�Ұ���
	short  siThroughThick;	//Ԥ�����߽Ǹ�֫(ͨ)��S
};
template <class FITTING_TYPE> struct IFittingLibrary : public IGlobalInitializer
{	//����֧�����޽Ǹֹ������� wjh-2019.9.11
	virtual int RecCount()=0;		//��¼��
	virtual int SetRecCount(int count)=0;
	virtual UINT FittingTypeId(){return FITTING_TYPE::GetTypeId();}
	virtual FITTING_TYPE* GetAt(int index)=0;
	virtual FITTING_TYPE* SetAt(int index,const FITTING_TYPE& sizerec)=0;
	virtual FITTING_TYPE* AppendSizeRecord(const FITTING_TYPE& sizerec)=0;
	virtual FITTING_TYPE* GetSizeRecTbl()=0;
	virtual FITTING_TYPE* QueryFittingRecord(const char* label)=0;
	virtual FITTING_TYPE* MatchFittingRecord(double dfHoleD)=0;
	virtual bool LoadSizeParams(const char* label,FITTING_TYPE* pFittingRecord=NULL)=0;
	operator FITTING_TYPE*() { return GetSizeRecTbl(); }
public:	//����
	int get_Count() { return RecCount(); }
	int set_Count(UINT count) { return SetRecCount(count); }
	__declspec(property(put=set_Count,get=get_Count)) int Count;
	FITTING_TYPE* get_pxSizeRecTbl() { return GetSizeRecTbl(); }
	__declspec(property(get=get_pxSizeRecTbl)) FITTING_TYPE* pxSizeTbl;
};
void* GetFittingLibrary(UINT idFittingType);
template <class FITTING_TYPE> FITTING_TYPE* QueryFitting(const char* sSpec)
{
	IFittingLibrary<FITTING_TYPE>* pFittingLibrary = FittingLibrary<FITTING_TYPE>();
	if (pFittingLibrary)
		return pFittingLibrary->QueryFittingRecord(sSpec);
	return NULL;
}
template <class FITTING_TYPE> FITTING_TYPE* MatchFitting(double dfHoleD)
{
	IFittingLibrary<FITTING_TYPE>* pFittingLibrary = FittingLibrary<FITTING_TYPE>();
	if (pFittingLibrary)
		return pFittingLibrary->MatchFittingRecord(dfHoleD);
	return NULL;
}
template <class FITTING_TYPE> IFittingLibrary<FITTING_TYPE>* FittingLibrary()
{
	return (IFittingLibrary<FITTING_TYPE>*)GetFittingLibrary(FITTING_TYPE::GetTypeId());
}
