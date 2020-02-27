#pragma once

#include "f_ent.h"
#include "list.h"
#include "Variant.h"
#include "..\StdFacePanel.h"
#include ".\IStdComTempl.h"
#include ".\StdComModel.h"

class CArmHeadComTempl;
struct CWireStruComTemplRepository
{
	struct CONDITION{
		long idProp;
		WORD wiLogicOperator;	//'>','=','<',">=","<=","!="
		CVariant varVal;
	};
	int Query(CONDITION* conditions,int count,IXhList<CArmHeadComTempl*> listRslts);
};
//CArmHeadComTempl类主要用于杆塔规划时，塔头整体外形的控制描述
class CArmHeadComTempl : public CStdComTempl
{
protected:
	virtual CXhChar50 _InternalGetSketchImgFileSubFolder() { return ""; }
public:
	struct INERNAL_PANEL{
		UINT id;
		union{
			struct{
				char ciPanelSchema;	//0.孤立面;1.节间面
				BYTE cidPanelType;	//节间面子标识类别
			};
			WORD widPanelType;
		};
		char ciSymmetryType;	//对称类别,'X','Y','Z','A';
		char description[17];	//描述
		void SetKey(DWORD key){id=key;}
	};
	struct WIRE_CLEARANCE_POINT{
		GEPOINT xWirePoint;		//电线挂点位置
		GEPOINT vWireFlowDirect;//线路方向
		GEPOINT xRotAxisOrg,vRotAxisVec;//挂点旋转轴基点，旋转轴方向
		short siFromSwingDegAngle;	//<=0	摇摆角的起始值
		short siToSwingDegAngle;	//>=0	摇摆角的终止值
		double fMinElecDistance;	//最小电气间隙，mm
	};
public:
	UINT m_idWireStruTmplType;	//导地线横担或塔头模板类型Id
	CHashListEx<CParaDatumPlane> listParaPlanes;
public:
	CArmHeadComTempl(void);
	virtual ~CArmHeadComTempl(void);
	//
	CVariant GetPropVal(long idProp);
	bool IsAdapt(long idProp,CVariant& varPropVal);
	DESIGN_PARAM_ITEM* EnumFirstProfileParam();	//遍历第一个外形尺寸控制参数
	DESIGN_PARAM_ITEM* EnumNextProfileParam();	//遍历下一个外形尺寸控制参数
	INERNAL_PANEL* EnumFirstStructPanel();
	INERNAL_PANEL* EnumNextStructPanel();
	bool IntelliPlanSubPanels();	//智能规划设计各子工作面
	//电气间隙校验接口
	WIRE_CLEARANCE_POINT* AddElecClearanceCheckPoint(WIRE_CLEARANCE_POINT* pCheckPoint=NULL);
	WIRE_CLEARANCE_POINT* EnumFirstElecCheckPoint();
	WIRE_CLEARANCE_POINT* EnumNextElecCheckPoint();
	bool IntelliDesignProfileParams();	//根据电气间隙智能设计塔头横担的尺寸控制参数
	//结构亚型判断条件
	BYTE RecogTemplSubTypeId();
public:
	//UCS_STRU rot_ucs;
	//CHashListEx<DESIGN_PARAM_ITEM>listParams;
	//CHashListEx<CParaNode>listParaNodes;
	//CSuperHashList<CParaRod>listParaRods;
	//键值对应的是相应CStdTemplParaBolt的标识，数值对应的是相应宿主构件上的一组挂孔
	//CHashList<CXhChar100> listWireHoleSet;
public:
	//CParaTemplate();
	//virtual ~CParaTemplate();
	long KeyNodeNum();
	void UpdateNodePos();
	void ToStream(IStream *pStream);
	void FromStream(IStream *pStream);
	bool AssertParamsValid();//调用各模板中应支持的部件参数合法性校验
public:
	//struct TmplPropListUI{
	//	virtual long InitPropHashtable(long initId=1,long initHashSize=50,long initStatusHashSize=50);	//返回下一空闲Id
	//	virtual long GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);
	//	virtual void DisplayPropertyList(int overwrite0_append1=0,DWORD dwPropGroup=1);
	//	virtual bool ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt);
	//	virtual bool ModifyValueFunc(CPropTreeItem* pItem,CString& valueStr,BOOL *lpRslt);
	//	virtual BOOL FinishSelectObjOper(long hPickObj,long idEventProp);
	//	virtual BOOL HelpPromptProperty(CPropTreeItem* pItem);
	//}ui;
public:
	virtual const char* GetName();
	virtual bool IsWireStruTmpl(){return true;}
	virtual bool GetSketchImage(IMAGE_INFO* pSketchImage);
	virtual void InitComponentTempl(TEMPL_SOURCE* pTemplSource=NULL);
	virtual bool IsMeetConditions();
	virtual long MappingLdsNodeH(long idTemplNode);
	virtual long SetMappingNode(long idTemplNode,long hLdsNode);
	virtual bool SetMappingNode(long idTemplNode,CLDSNode *pNode);
};
//////////////////////////////////////////////////////////////////////////
#ifdef __COMMON_PART_INC_
struct MAP_RODnNODE{
	static const BYTE TYPE_NODE = 0;
	static const BYTE TYPE_ROD  = 1;
public:
	BYTE ciNode0Rod1;	//0.映射节点;1.映射杆件
	BYTE ciCondGroup;	//同一序号的条件必须同时满足才为充分条件
	long idNodeOrRod;
	union{
		CLDSNode* pNode;
		CLDSLinePart* pRod;
	};
	void Set(long idTmplObj=0,CLDSDbObject* pMapLdsObj=NULL);
	MAP_RODnNODE(long idTmplObj=0,CLDSDbObject* pMapLdsObj=NULL){Set(idTmplObj,pMapLdsObj);}
};
struct WIREPOINT_GROUP {
private:
	BYTE _cnHoleD;
	WORD _wnReservedWidth;
public:
	BYTE ciGroupSerial;			//挂点分组号
	BYTE ciFittingType;			//金具类型0.EB挂板|1.GD挂板|2.U形螺栓
	BYTE cnShaftD;				//金具耳轴直径,mm
	WORD wnFittingWidth;		//挂接金具宽度
	WORD wiInsulatorStyle;		//挂接串型，如单(I)串、双串(单挂)、双串双挂、V串
	char szFittingSizeCode[17];	//金具型号
	char szUringSizeCode[17];	//EB/GD金具下的挂接U形环型号
	BYTE cnWirePointCount;		//挂点(孔)数量
	BYTE ciResidePlane;			//0.当前工作平面（挂线角钢工作肢或钢板平面）;1.挂线角钢另一肢
	BYTE ciLocateStyle;			//挂点定位类型0.从挂线角钢中点往两侧对称布置，依赖开档距离定位;1.挂点与附近横担角钢间的干涉间隙
	union {
		WORD wiSpaceL;		//挂孔间开档距离
		WORD wiClearance;	//干涉间隙
	};
	float sfMasterSwingAngle;	//主金具摆动角，度
	float sfSlaveUrSwingAngle;	//挂接U形环摆动角，度
public:
	WIREPOINT_GROUP();
	//预留安装孔径,mm，一般cnHoleD=cnShaftD+2
	BYTE get_cnHoleD() const {
		if (_cnHoleD > cnShaftD)
			return _cnHoleD;
		else
			return cnShaftD + 2;
	}
	BYTE set_cnHoleD(BYTE hole_d) {
		return _cnHoleD = hole_d; 
	}
	__declspec(property(put = set_cnHoleD,get = get_cnHoleD)) BYTE cnHoleD;
	//金具预留宽度,mm，一般wnReservedWidth=wnFittingWidth+4
	WORD get_wnReservedWidth() const { 
		if (_wnReservedWidth > wnFittingWidth)
			return _wnReservedWidth;
		else
			return wnFittingWidth + 4;
	}
	WORD set_wnReservedWidth(WORD reserved_width){
		return _wnReservedWidth = reserved_width;
	}
	__declspec(property(put = set_wnReservedWidth,get = get_wnReservedWidth)) WORD wnReservedWidth;
};
class CStdComTemplWireStructure : public CArmHeadComTempl
{
protected:
	void InitComTempl_EB_DANGLE_EARTH_WIRE_ARM1();
	void InitComTempl_EB_DANGLE_EARTH_WIRE_ARM2();
	void InitComTempl_EB_DANGLE_EARTH_WIRE_ARM3();
	void InitComTempl_EB_EARTH_WIRE_ARM();
	void InitComTempl_EB_EARTH_WIRE_PLATE();
	void InitComTempl_EB_DANGLE_WIRE_I_ARM1();
	void InitComTempl_EB_DANGLE_WIRE_I_ARM2();
	void InitComTempl_EB_DANGLE_WIRE_I_ARM3();
	void InitComTempl_EB_WIRE_V_ARM1();
	void InitComTempl_EB_WIRE_V_ARM2();
	void InitComTempl_EB_WIRE_V_ARM3();
	void InitComTempl_EB_WIRE_V_ARM4();
	void InitComTempl_EB_WIRE_V_ARM5();
	void InitComTempl_EB_WIRE_V_ARM6();
	void InitComTempl_META_SECT_BTM_1_EB();
	void InitComTempl_META_SECT_BTM_2_EB();
	void InitComTempl_META_SECT_FRT_1_GD();
	void InitComTempl_META_SECT_BTM_1_GD();
	void InitComTempl_NZ_FRT_2_GD();
	void InitComTempl_NZ_BTM_2_GD();
	void InitComTempl_NZ_BTM_3_GD();
	//
	bool TransTemlInstance_EB_DANGLE_EARTH_WIRE_ARM1();
	bool TransTemlInstance_EB_DANGLE_EARTH_WIRE_ARM2();
	bool TransTemlInstance_EB_DANGLE_EARTH_WIRE_ARM3();
	bool TransTemlInstance_EB_DANGLE_WIRE_I_ARM1();
	bool TransTemlInstance_EB_DANGLE_WIRE_I_ARM2();
	bool TransTemlInstance_NZ_FRT_2_GD();
	bool TransTemlInstance_NZ_BTM_2_GD();
	bool TransTemlInstance_NZ_BTM_3_GD();
	bool TransTemlInstance_SECT_BTM_1_EB();
	bool TransTemlInstance_SECT_BTM_2_EB();
	bool TransTemlInstance_SECT_FRT_1_GD();
	bool TransTemlInstance_SECT_BTM_1_GD();
public:
	static const UINT N10_ID=1;	//尖角地线挂点的顶部节点
	static const UINT N20_ID=2;	//地线挂点
	static const UINT N30_ID=3;	//地线架水平横杆塔心侧端节点
	static const UINT N40_ID=4;	//地线架竖杆与水平杆相交节点
	static const UINT N50_ID=5;	//挂线角钢中点，即EB挂点的装配点
	static const UINT N60_ID=6;	//挂线角钢中点，即EB挂点的装配点
	static const UINT N70_ID=7;	//
	static const UINT N80_ID=8;	//
	//杆件
	static const UINT R_W_T=4;	//T型组合挂线角钢
	PRESET_ARRAY4<MAP_RODnNODE> xarrKeyRodNodes;
	WIREPOINT_GROUP xarrWireGroups[2];	//目前模板仅支持至多两组挂点
	char m_cnWireGroupCount;			//当前挂线架的挂线角钢上挂点分组数量
	BYTE m_cnFixWireAngleConnBoltsN;	//固接挂线短角钢的连接螺栓数(因连接两个基座角钢，所以必须是偶数）
public:
	CStdComTemplWireStructure();
	bool TranslateTmplInstance();
	//
	virtual void FromBuffer(BUFFER_IO* pIO,DWORD version=0);
	virtual void ToBuffer(BUFFER_IO* pIO,DWORD version=0);
	virtual bool CloneStdTmplFrom(IStdComTempl* pStdComTmpl);
	virtual void InitComponentTempl(TEMPL_SOURCE* pTemplSource=NULL);
	virtual bool ClearTmplParams();
	virtual int RecogMatchRods(UINT idRecogRod=0);
	virtual bool IsMeetConditions();
	virtual bool SetRecogKeyNodesAndRods(MAP_RODnNODE* parrMapObjs=NULL,long count=1);
	virtual bool MeetRecogKeyObjs(MAP_RODnNODE* parrMapObjs=NULL,long count=1,MAP_RODnNODE* parrMeetMapObjs=NULL,long lnMaxMeetCount=4);
	virtual long SetMappingNode(long idTemplNode,long hLdsNode);
	virtual bool SetMappingNode(long idTemplNode,CLDSNode *pNode);
};
#endif
//////////////////////////////////////////////////////////////////////////
//
#ifdef __LDS_
struct TOWER_PLANINFO{
	bool biCircuitDC;	//是否为直流 
	char ciTensionType;	//杆塔类型:0.直线悬垂塔;1.耐张塔;2.终端塔;3.分岐塔;4.换位塔
	WORD wiVoltGrade;	//电压等级kV
	WORD wiCircuits;	//回路数
	WORD wiSeaLevel;	//海拔高度,m
	UINT uiNamedHeight;	//呼高(mm),0表示根据塔型及电压等级自动选择呼高
	CLDSNode* pSetupNode10,*pSetupNode20;
	CXhChar16 szTowerTypeCode;	//杆塔型号代码
	CXhChar16 szTowerTypeName;	//塔型名称
	TOWER_PLANINFO();
};
#include "..\ShankDefCls.h"
class CTowerPlanner : public CShankDefine
{
	bool AppendWireStruArms(IModel* pModel);
public:
	struct SETUP_PLANE{
		long ziPlane;
		double xfSectCorner,yfSectCorner;
	};
	struct ARM_ASSMBLY{
		long id;
		char ciDirection;		//'X'：X向横担或塔头;'x'：左侧X向横担；'Y'：Y向横担；'y'后侧Y向横担
		long idWireStruTmpl;	//关联塔头或横担的模板类型Id
		SETUP_PLANE xN10,xN20;
		CXhChar50 name;
		void SetKey(DWORD keyid){id=keyid;}
		ARM_ASSMBLY(){ciDirection='X';id=0;idWireStruTmpl=1;}
	};
	TOWER_PLANINFO xPlanInfo;	//线路规划设计参数信息
	CHashListEx<ARM_ASSMBLY> hashArmAssmblies;
	bool Planning(TOWER_PLANINFO& planInfo,CTower* pTower);
};
class CWireStruComTemplModel : public CStdComModel
{	
protected:
	//virtual bool BuildAcsOfAssmComponent();
public:
	CWireStruComTemplModel(IModel* pModel=NULL);
	~CWireStruComTemplModel(void){;}
	virtual bool IntelliDesignTowerParams(TOWER_PLANINFO& planinfo);
public:	//虚函数
	virtual CLDSNode* GetLdsNodeFromId(TMPLOBJID idNode);
};
#endif
