#ifndef __EMEND_BOLT_GASKET_
#define __EMEND_BOLT_GASKET_
//#ifdef __SMART_DRAW_INC_

#include "OutputInfo.h"
///////////////////////////////////////////////////////////////////////////////////////////////
//-------VVVVVVVVVVV------- 以下区域为根据TAP改进的垫板有效间隙检测代码 wjh-2017.12.5
class CEmendBoltGasket;
struct BOLT_RELA_PART{
	short _zDepth;	//相对于装配螺栓基点沿螺栓法向的Ｚ向偏移量,(由于螺栓位置可能调整,其绝对值意义不大,但各穿越构件间的zDepth差值有意义)
	short siThick;	//穿越构件自zDepth点沿螺栓法向Ｚ的通厚偏移量(允许为负值)
	DWORD hPart;	//关联的装配构件记录索引Id
	CFGWORD cfgword;
	BOLT_RELA_PART(CLDSPart* pRelaPart=NULL);
	void SetKey(DWORD key){hPart=key;}
	bool IsWasherPlate();
public:	//属性
	double get_zDepth() const{return _zDepth*0.1;}
	void set_zDepth(double vf){_zDepth=(short)ftoi(vf*10);}
	_declspec(property(put=set_zDepth,get=get_zDepth)) double zDepth;
	//CLDSPart* pPart属性
	CLDSPart* get_pPart();
	void set_pPart(CLDSPart* pRelaPart);
	_declspec(property(put=set_pPart,get=get_pPart)) CLDSPart* pPart;
protected:
	CLDSPart* _pRelaPart;
};
class CBoltGasket;
class CBoltRelaPartList{
	DWORD m_idAssmBolt;	//装配螺栓的索引Id
	DWORD m_idAnchorPart;
	CXhSimpleList<BOLT_RELA_PART> listRelaObjs;
	CEmendBoltGasket* m_pContext;	//归属的数据模型
	//DWORD m_idFilterInstance;	//记录当前激活塔例
public:
	const static BYTE TYPE_SUCCESS				= 0;	//0.修订成功
	const static BYTE TYPE_NONE_CONNECTED		= 1;	//1.无关联构件 
	const static BYTE TYPE_VIRTUAL_BOLT			= 2;	//2.非脚钉螺栓仅连接一个构件
	const static BYTE TYPE_MISS_CROSSED_PAD		= 3;	//3.交叉点处少垫片
	const static BYTE TYPE_MISS_END_PAD			= 4;	//4.端头需要添加螺栓垫片
	const static BYTE TYPE_PART_POS_ERROR		= 5;	//5.构件位置错误
	const static BYTE TYPE_ERROR_GASKET_THICK	= 6;	//6.垫圈厚度不正确
	const static BYTE TYPE_WIRE_VIRTUAL_BOLT	= 7;	//7.挂线孔假螺栓
	const static BYTE TYPE_NONE					= 0xFF;	//未初始化
	BYTE m_cErrorType;	//修订螺栓时错误类型 
public:
	CBoltRelaPartList();
	void SetKey(DWORD key){m_idAssmBolt=key;}
	CEmendBoltGasket* GetContext(){return m_pContext;}
	void SetContext(CEmendBoltGasket* pContext){m_pContext=pContext;}
	BOLT_RELA_PART* AddRelaPart(DWORD idAssmPart,double zDepth,short siThick,UINT idInstance=0,BOOL bSort=TRUE);
	bool RemoveRelaPart(DWORD idAssmPart,UINT idInstance=0);
	BOLT_RELA_PART* EnumFirstRelaPart();
	BOLT_RELA_PART* EnumNextRelaPart();
	bool IsVirtualHole();	//是否为虚拟孔（除脚钉外仅穿过一个构件的螺栓及Ｔ型组合角钢上的挂线孔穿两根角钢）
	//DWORD IntelliRecogAnchorAssmPart();	//返回最佳的螺栓锚位基准构件
	double GetMaxSpaceAndSumThroughThick(WORD *pwSumThick=NULL,
										 BOLT_RELA_PART **ppBtmRelaPart1=NULL,
										 BOLT_RELA_PART **ppTopRelaPart2=NULL,
										 BOLT_RELA_PART **ppGasketRelaPart=NULL,
										 BOOL bInGasketThick=TRUE);
	bool InitializeBoltGasket(CBoltGasket* pBoltGasket);
	bool IsThroughGroupAngle();
	int GetRelaPartCount(bool bIncExistGasket=true);
public:	//属性
	READONLY_PROPERTY(DWORD,Id);	//装配螺栓的索引Id
	GET(Id){return m_idAssmBolt;}
	DECLARE_PROPERTY(DWORD,AnchorPartId);	//装配螺栓的索引Id
	DECLARE_PROPERTY(BOLT_RELA_PART*,pExistGasket);	//装配螺栓的索引Id
	//WRITEONLY_PROPERTY(DWORD,FilterInstanceId);
	//SET(FilterInstanceId){m_idFilterInstance=value;}
};
class CAssemblePart{
protected:
	long m_hPart;
	CLDSPart* m_pPart;
	CEmendBoltGasket* m_pContext;	//归属的数据模型
	CHashList<CLDSPart::HOLE> hashHoles;
public:
	CAssemblePart();
	void SetContext(CEmendBoltGasket* pContainer,CLDSPart* pPart);
	void ClearBoltHoleRefList(){hashHoles.Empty();}
	void SetKey(long key){m_hPart=key;}
	BOOL DeleteHole(DWORD hBolt);
	UINT StatHoles();
	CLDSPart::HOLE *GetBoltHole(DWORD hBolt);
	DWORD GetHoleRefCount(){return hashHoles.GetNodeNum();}
	CLDSPart::HOLE *EnumHoleRefFirst(){return hashHoles.GetFirst();}
	CLDSPart::HOLE *EnumHoleRefNext(){return hashHoles.GetNext();}
public:	//常用属性定义
	CEmendBoltGasket* get_pContext(){return m_pContext;}
	__declspec(property(get=get_pContext)) CEmendBoltGasket* pContext;
	long get_hPart(){return m_hPart;}
	__declspec(property(get=get_hPart)) long hPart;
	SmartPartPtr get_pSmartPart(){return m_pPart;}
	__declspec(property(get=get_pSmartPart)) SmartPartPtr pSmartPart;
};
//以上区域为根据TAP改进的垫板有效间隙检测代码 wjh-2017.12.5
//-------------^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^---------------------------------
#include "LifeObj.h"
class CBoltGasket
{	//所有有可能需要螺栓垫圈\板的螺栓
public:
	long feature;
	CLDSBolt *pBolt;	//基准螺栓
	double gasketOffset;//垫板自基准螺栓在基准构件上的穿插交点的偏移值，用来确定垫板位置（基准构件的工作法线为偏移正向)
	double washerOffset;//垫圈自螺栓的偏移距
	double gasketThick;	//垫板垫圈厚度
	double boltLengthPos;	//螺栓在定位基准角钢纵向上投影距离位置（始->终方向距始端楞点距离）,个别函数内临时使用参数
	//以螺栓朝向为基准时，所需要添加垫板位置的底部构件与顶部构件句柄
	long hBtmGasketRelaPart,hTopGasketRelaPart;
	CHashSet<CLDSPart*> hashRelaPartPtrs;//与螺栓相关联的构建
	CBoltGasket(){hBtmGasketRelaPart=hTopGasketRelaPart=feature=0;gasketOffset=gasketThick=0;washerOffset=0;boltLengthPos=0;pBolt=NULL;}
public:	//系统当前垫板(垫圈)的厚度属性
	WORD get_wiCurrDianQuanThick();
	WORD get_wiCurrGasketThick();
	__declspec(property(get=get_wiCurrDianQuanThick)) WORD wiCurrDianQuanThick;
	__declspec(property(get=get_wiCurrGasketThick))   WORD wiCurrGasketThick;
};
typedef CBoltGasket* BOLTGASKET_PTR;
class CDatumPartBolts
{
public:
	f3dPoint norm;
	f3dPoint datum_origin;
	CXhPtrSet<CBoltGasket> boltGasketSet;
};
enum EMENDGASKET_TYPE{
	ERROR_ISOLATED_BOLT=1,		//无关联构件的孤立螺栓, 处理方式为:忽略或确认清除
	ERROR_VIRTUAL_HOLE_LIKELY,	//目前有一个关联构件的非脚钉疑似虚拟孔，处理方式为：确认设为虚拟螺栓孔
	ERROR_VIRTUAL_WIREHOLE_LIKELY,	//挂线孔虚拟螺栓，处理方式为：确认设为虚拟螺栓孔
	ERROR_MISSING_CROSSGASKET,	//交叉点处垫板缺失，处理方式为：添加垫板或垫圈
	ERROR_MISSING_ENDGASKET,	//端连接处垫板缺失，处理方式为：添加垫板或垫圈
	ERROR_GASKET_THICK,			//垫板或垫圈厚度错误，处理方式为：重新修订垫板或垫圈厚度
	ERROR_PART_POS,				//构件位置错误
	ERROR_BOLTPAD_CUTANGLE,		//垫板缺少切角处理
	ERROR_GASKET_PROFILE,		//垫板外形不合适，处理方式为：修正垫板外形
	ERROR_REDUNDANT_GASKET,		//冗余垫板，处理方式为：清除垫板
	ERROR_SPECIAL_GASKET,		//特殊垫板(菱形或轮廓点有误)

	NEW_BOLTPAD,			//新增的垫板
	EMEND_BOLTPAD_THICK,	//修正垫板厚度
	EMEND_BOLTPAD_PROFILE,	//修正垫板外形
	REDUNDANT_BOLTPAD,		//冗余的螺栓垫片
	BOLTPAD_CUTANTLE,		//垫板切角处理
	NEW_GASKET,				//新增垫圈
	EMEND_GASKET,			//修正垫圈
	ONE_PART_BOLT,          //无效的螺栓
	EMEND_OTHER,
};
struct GASKET_WARNING_RECORD
{
	DWORD idDatumBolt;	//垫圈或垫板关联螺栓中的一颗基准索引螺栓
	void SetKey(DWORD key){idDatumBolt=key;}
	long feature;
	EMENDGASKET_TYPE style;	
	int gasketNum;
	double thick;	//垫板或垫圈厚度
	long idRawGasket;
	CLDSBolt* pBolt;
	CLDSLineAngle* pCutAngle;
	union{
		CLDSPart* pPart;
		CLDSPlate* pGasketPlate;	//对应：ERROR_GASKET_THICK,ERROR_REDUNDANT_GASKET
	};
	double gasketOffset;//垫板自基准螺栓在基准构件上的穿插交点的偏移值，用来确定垫板位置（基准构件的工作法线为偏移正向)
	double washerOffset;//垫圈自螺栓的偏移距
	CLDSPart* pAttachPart;
	GASKET_WARNING_RECORD();
	CXhChar50 GetDescription();
};
//原始垫板
class CRawGasket
{
	UINT _id;
public:
	static const int ONEHOLE        = 1;	//单孔
	static const int MULTIHOLE      = 2;	//非交叉多孔
	static const int CROSSMULTIHOLE = 3;	//交叉多孔
	static const int WASHER		    = 4;	//垫圈
public:
	int type;				//垫板类型	1.单孔 2.非交叉多孔 3.交叉多孔点垫板
	CLDSNode *pCrossNode;	//记录多孔交叉点垫板对应的交叉节点
	CLDSLineAngle* pCutAngle;//对垫板进行切角的切割角钢
	CHashSet<CBoltGasket*> boltGasketSet;	//关联螺栓
	CRawGasket(){type=0;pCrossNode=NULL;pCutAngle=NULL;}
	void SetKey(DWORD key){_id=key;}
	UINT get_id(){return _id;}
	__declspec(property(get=get_id)) UINT Id;
};
struct INTERS_POINT{
	CLDSPart *pPart;
	f3dPoint inters_pt;
	double distance;
	double GetHalfThick()
	{
		if(pPart)
			return pPart->GetThick()*0.5;
		else 
			return 0;
	}
	INTERS_POINT(){pPart=NULL;distance=0;}
	INTERS_POINT(CLDSPart *pPart,f3dPoint intersPt,double dist)
	{
		this->pPart=pPart;
		inters_pt = intersPt;
		distance = dist;
	}
};
struct BOLT_ITEM 
{
	double d;		//螺栓直径
	f2dPoint vertex;//模式坐标系下的顶点坐标
};
struct VERTEX_ITEM
{
	PROFILE_VERTEX* pVertex;	//板相对坐标系下的顶点信息
	f2dPoint vertex2d;	//模式坐标下的顶点坐标
	BOOL bRectangular;	//是否是直角
	int feature;	//特殊标注
	VERTEX_ITEM(){bRectangular=FALSE;feature=-1;}
};
class CBoltPadProperty
{
public:
	static const int CUTANGELBOLTPAD  = 1;	//切角垫板
	static const int RECTANGLEBOLTPAD = 2;	//矩形垫板
	static const int SPECIALBOLTPAD	  = 3;	//其他垫板
private:
	UCS_2dSTRU modeUcs;	//模式坐标系
	int m_iSignIndex;	//顶点开始标记索引
	ARRAY_LIST<VERTEX_ITEM> vertexItemArr;	//顶点属性列表
	ARRAY_LIST<BOLT_ITEM> boltItemArr;	//螺栓属性列表
	//对于矩形垫板不能唯一确定匹配模式坐标，需要通过与基准垫板的螺栓进行原向，上下翻转，左右翻转，对角翻转比较后才能确定
	UCS_2dSTRU cs2d[4];
	ATOM_LIST<BOLT_ITEM> boltItemList[4]; //cs2d[4]坐标下的螺栓属性列表
public:
	CLDSPlate *m_pBoltPad;
	int m_iType;
public:
	CBoltPadProperty();
	void InitProperty(CLDSPlate* pPlate);	
	void InitCutAnglePadProperty();
	void InitRectanglePadProperty();
	BOOL CompareBoltInfo(CBoltPadProperty* pDatumPadProp);
	void MarkProfileVertexs();
	void UpdateMinProfileVertex(CBoltPadProperty* pDatumPadProp);
	BOOL EmendProfileVertexs(CBoltPadProperty* pDatumPadProp);
public:	//测试函数
	void Test_InitProperty(int index,CLDSPlate* boltPad);
	void Test_CompareBoltInfo(int index,CBoltPadProperty* datumPadProp);
	void Test_MarkProfileVertexs(int index);
	void Test_UpdateMinProfileVertex(int index,CBoltPadProperty* datumPadProp);
	void Test_EmendProfileVertexs(int index,CBoltPadProperty* datumPadProp);
};
struct SAMESCHEMA_BOLTPAD  
{
	CBoltPadProperty* pDatumProperty; 
	CHashList<CLDSPlate*> boltPadSet;
	SAMESCHEMA_BOLTPAD(){pDatumProperty=NULL;}
	int IsSameSpec(CBoltPadProperty* pPadProperty);	// 0.不相等 1.相等
};
//修订螺栓垫片或垫圈
class CEmendBoltGasket
{
public:
	static const int MINMODIFYDIST = 1;
	static const int MINSPACING = 3;			//最小间隙
	static const int BOLTSPACE_THRESHOLD = 150;	//判断是否为多孔垫片时需要的螺栓间距值

	CHashList<CBoltGasket> hashBoltGaskets;	//所有需要添加垫圈或垫片的螺栓
	CHashListEx<CRawGasket> hashRawGaskets;     //未加工的垫片集合 
	PLATESET existingBoltPadSet;				//已存在的垫片
	CHashList<CLDSPlate*> hashNewGaskets;		//记录新生成垫片
	CHashListEx<GASKET_WARNING_RECORD> hashErrorRecords;	//记录垫片或垫圈的修正信息
	CHashSet<GASKET_WARNING_RECORD*>   hashErrorRecordsView;//根据螺栓句柄定位警示信息(多孔垫板时除主螺栓外其余螺栓通过此哈希表集查找对应的警示信息)
	GASKET_WARNING_RECORD* GetWarningRecord(long hBolt);
private:
	//查找螺栓垫板的信息包括垫板的位置和厚度
	//1.遍历所有螺栓，查找需要添加垫圈或垫片的螺栓
	void InitGasketBoltSet(bool advanceMode=true);
	//3.修正现有垫板信息，并将关联螺栓从gasketBoltSet中移除
	void EmendExistingBoltPadSet(bool advanceMode=true);
	//4.初始化未加工的垫片集合，并初始化垫板模式(垫圈、单孔、多孔及多孔交叉点垫板)
	void InitRawGasketSet();
	void InitRawGasketSetAdvance();
	//5.修正类型为WAHSER的的螺栓垫圈信息，并从rawGasketSet移除。
	void EmendBoltWasherInfo();
	//6.生成未加工垫片,并添加到newGasketSet中
	void CreateGaskets();
	//7.合并同一段内外形相近的垫片
	void MergeSimilarGasketInSameSeg();
	void AppendOutputInfoToList(GASKET_WARNING_RECORD* pRecordPart,CTaskOutputList* pInfoList);
public:
	CEmendBoltGasket(IModel* pModel){m_pModel=pModel;}
	void Execute(DATACENTERCMD_CONTEXT *pContext,bool advanceMode=true);
	void InitOutputInfoList(CTaskOutputList* pInfoList);
	//以下为根据TAP改进的垫板有效间隙检测代码 wjh-2017.12.5
protected:
	IModel* m_pModel;
	struct WIRE_ANGLE{
		long idAssmAngle;
		BYTE ciWireWing;	//'X':X肢为挂线肢;'Y':Y肢为挂线肢
		WIRE_ANGLE(long _idAssmAngle=0,BYTE _ciWireWing=0)
		{
			idAssmAngle=_idAssmAngle;
			ciWireWing=_ciWireWing;
		}
	};
	CHashList<WIRE_ANGLE> hashWireAngles;
	CHashListEx<CBoltRelaPartList> hashBoltRelaObjs;
	CHashListEx<CAssemblePart> hashAssmParts;
	BYTE JustifyErrorEndPosAngle(CAssemblePart* pAssmPart,CBoltGasket* pGasket);
	BYTE ValidateTheroyGasket(CBoltGasket* pGasket,long* phErrPosAngle=NULL);
public:
	void StatWireAngles();
	BYTE GetAngleWireWing(long idAssmAngle);//0:非挂线角钢;'X':X肢为挂线肢;'Y':Y肢为挂线肢
	void UpdateBoltRelaObjs(DWORD* pAssmBoltIds=NULL,UINT count=0);	//pAssmBolts=NULL,表示更新全部螺栓关联信息，否则仅更新指定螺栓集合
	void UpdateBoltHoles(DWORD* phAssmParts=NULL,UINT count=1);
	//idAssmBolt:锚固定位螺栓装配对象;idCurrAssmPart:当前需要仿真定位的构件装配对象;pAnchorPoint返回仿真定位锚固点
	//bool CalDestAnchorPoint(DWORD idAssmBolt,DWORD idCurrAssmPart,CAssemblePart::ANCHOR_POINT* pAnchorPoint);
	CBoltRelaPartList* GetBoltRelaParts(DWORD hBolt);
	CBoltRelaPartList* AddBoltRelaParts(DWORD hBolt);
	CAssemblePart* GetAssemblePart(DWORD hPart);
	CAssemblePart* AddAssemblePart(CLDSPart* pPart);
	CLDSPart* FromPartHandle(long hPart,int idClassType=0){return m_pModel->FromPartHandle(hPart,idClassType);}
	CAssemblePart* FromAssmPartHandle(long hPart,int idClassType=0);
	BOOL ExecuteEmendBoltSpec(DATACENTERCMD_CONTEXT *pContext);
};
#endif