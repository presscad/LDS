#pragma once
#include "..\Global.h"

static const UINT TYPE_EB = 0x00000001;
static const UINT TYPE_GD = 0x00000002;
static const UINT TYPE_UR = 0x00000003;
static const UINT TYPE_US = 0x00000004;	//U型螺丝(U或UJ-d-L)
static const UINT TYPE_UB = 0x00000005;	//U型挂板(UB-等级)

//金具
struct FITTING_GD{	
	static UINT GetTypeId() { return TYPE_GD; }
public:
	char   szSizeCode[16];	//型号代码
	short  siThick;			//挂板厚度b
	short  siThroughThick;	//预留挂线角钢肢(通)厚
	double dfTrunnionD;		//耳轴直径
	double dfHoleD;			//挂孔直径(diameter)Ф
	double dfBoardH;		//螺栓孔中心到下圆弧的长H
	double dfBoardH1;		//耳轴到上圆弧的长H1
	double dfBoardH2;		//挂孔到耳轴中心线距离H2
	double dfTrunnionL;		//耳轴总长
	double dfBoardL;		//金具(挂板部分)宽度，上圆弧长
};
struct FITTING_EB{
	static UINT GetTypeId() { return TYPE_EB; }
public:
	char   szSizeCode[16];	//型号代码
	short  siThick;			//挂板厚度b
	short  siThroughThick;	//预留挂线角钢肢(通)厚
	double dfTrunnionD;		//耳轴直径
	double dfHoleD;			//挂孔直径
	double dfBoardL;		//金具(挂板部分)宽度
	double dfBoardH;		//挂孔到耳轴中心线距离H
	double dfChinR;			//挂孔下巴圆弧半径
};
struct FITTING_UR{
	static UINT GetTypeId() { return TYPE_UR; }
public:
	char szSizeCode[16];
	char ciType;			//0.普通型U形挂环;1.加长型U形挂环
	double dfNamedL;		//名义长(转轴中心至环壁内侧)L，mm
	double dfShaftD;		//转轴螺栓直径d
	double dfMdEdgeSpace;	//U型环与转轴螺母贴合面的边距D，mm
	double dfT;				//U型环与EB或GD挂板间的连接部单侧连接壁厚T，mm
	double dfRingBs;		//U型环柱直径B(s),
	double dfBoardSpaceC;	//U型环与EB或GD挂板间的连接间隙(>=EB/GD挂板厚度)
	double dfRadiusOfRing;	//U型环圆环内侧半径R，一般>=dfBoardSpaceC/2
};
struct FITTING_US{
	static UINT GetTypeId() { return TYPE_US; }
public:
	char szSizeCode[16];
	char ciType;			//0.普通型U形螺丝;1.加强型UJ形螺丝
	double dfNamedC;		//U形螺丝名义宽度(左右两挂孔间中心宽度)C，mm
	double dfColumnM;		//U型螺丝直圆柱直径
	double dfColumnd;		//U型螺丝环型圆柱直径
	double dfColumnL1;		//U型螺丝圆柱高
	double dfColumnL2;		//U型螺丝柱顶到螺栓底部距离L2
	short  siThroughThick;	//预留挂线角钢肢(通)厚S
};
struct FITTING_UB{
	static UINT GetTypeId() { return TYPE_UB; }
public:
	char szSizeCode[16];
	char ciType;			//0.普通型UB挂板;1.加长型UB挂板
	double dfNamedL;		//U型挂板内测间距C
	double dfTrunnionD;		//转轴直径
	double dfBoardH;		//挂板高度(转轴中心至圆弧圆心)
	double dfBoardW;		//挂板宽度
	short  siThroughThick;	//预留挂线角钢肢(通)厚S
};
template <class FITTING_TYPE> struct IFittingLibrary : public IGlobalInitializer
{	//此类支持无限角钢规格库数量 wjh-2019.9.11
	virtual int RecCount()=0;		//记录数
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
public:	//属性
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
