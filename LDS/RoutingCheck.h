#include "StdAfx.h"
#include "SegI.h"
#include "lds_co_part.h"

struct ROUTINE_CHKINFO
{
	SEGI segI;
	CXhChar50 sLabel;
	BOOL bPassed;
	CXhChar16 sSpec;
	virtual CXhChar200 GetErrorMessage(){return "";};
	ROUTINE_CHKINFO(){bPassed=FALSE;}
};
//1.组合角钢编号及间隙放大值
struct GROUPANGLESPACE_CHKINFO  :public ROUTINE_CHKINFO
{
	CLDSGroupLineAngle* pGroupAngle;
	CLDSGeneralPlate* pPlate;
	GROUPANGLESPACE_CHKINFO(){
		pGroupAngle=NULL;
		pPlate=NULL;
	}
	static WORD MANU_SPACE;
	virtual CXhChar200 GetErrorMessage();
};
typedef GROUPANGLESPACE_CHKINFO* EXPORT_INFO_PTR;
//2.钢板边距(普通钢板\包钢板\垫板)异常警示信息
struct PLATESPACE_CHKINFO:public ROUTINE_CHKINFO
{
	static const int  GENERAL_PLATE	=0;
	static const int  WRAPPER_PLATE	=1;
	static const int  GASKET_PLATE	=2;
	long hPlate,hBolt;
	int iSubType;	//0.普通板;1.包钢板;2.垫板
	double actualEndSpace;	//实际的螺栓边距
	long needEndSpace;		//按规定需要的螺栓边距
	virtual CXhChar200 GetErrorMessage();
	PLATESPACE_CHKINFO(){iSubType=GENERAL_PLATE;}
};
//3.杆件正负头异常警示信息
struct RODODD_CHKINFO  :public ROUTINE_CHKINFO
{
	char ciCurrIsStart0End1;
	CLDSLinePart *pRod,*pRod2;
	RODODD_CHKINFO(){
		pRod=pRod2=NULL;
		ciCurrIsStart0End1=0;
	}
	virtual CXhChar200 GetErrorMessage();
};
//2.角肋板及槽肋板报告及内圆弧报告
struct RIB_EXPORT_INFO :public ROUTINE_CHKINFO
{
	double fThick;
	CXhChar16 sAngle;		//倒角
	CXhChar16 sPlateType;
	RIB_EXPORT_INFO(){fThick=0;sAngle=0;}
};
//3.卷边处理
struct ROLL_EDGE_EXPORT_INFO  :public ROUTINE_CHKINFO
{
	CXhChar100 sRollEdgeHeigth;
	int   nRollEdge;
	char cMaterial;
	CXhChar16 sRollEdgeDispose;
	ROLL_EDGE_EXPORT_INFO(){bPassed=FALSE;nRollEdge=0;cMaterial=' ';}
};

//4.端连接螺栓外孔间隙
struct BOLTEXTERSPACE_CHKINFO : public ROUTINE_CHKINFO
{
	WORD Md;			//螺栓直径
	WORD minSpace;		//允许最小值
	long hBolt;
	long hAngle;
	double clearance;	//实际外孔间隙
	CXhChar16 labelAngle,labelPlate;
};
//5、角钢螺栓边距
struct JGBOLTSPACE_CHKINFO:public ROUTINE_CHKINFO
{
	static const BYTE  ROLLING_SPACE	=0;	//轧制边距检测
	static const BYTE  CUTANGLE_SPACE	=1;	//切角边距检测
	static const BYTE  LS_END_SPACE		=2;	//螺栓端距检测
	//
	BYTE ciCheckType;
	long hJg,hBolt;
	BOOL bLsInStart;
	double realEndSpace;	//实际的螺栓边距
	long needEndSpace;		//按规定需要的螺栓边距
	JGBOLTSPACE_CHKINFO(){ciCheckType=0;hJg=hBolt=0;bLsInStart=FALSE;}
	virtual CXhChar200 GetErrorMessage();
};