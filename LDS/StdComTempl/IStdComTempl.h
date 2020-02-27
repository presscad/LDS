#pragma once
#include "Buffer.h"
#include "ArrayList.h"
#include "LifeObj.h"
#include "Expression.h"
#include "XhBitMap.h"
#include "Hashtable.h"
#include "XhCharString.h"
#include "ComplexId.h"
#include "DesignParamItem.h"
#include "AngleLayout.h"
#include "Catlog.h"
#include "../Global.h"
#include "XhLdsLm.h"	//defined MIRMSG class

union TMPLID{
	struct{
		WORD wiTmplId;
		union{
			struct{BYTE ciSubType,ciGroup;};
			WORD wiGroupType;
		};
	};
	DWORD dwTmplId;
	TMPLID(DWORD _dwTmplId){dwTmplId=_dwTmplId;}
	TMPLID(BYTE _ciGroup,BYTE _ciSubType,WORD _wiTmplId){
		ciGroup  =_ciGroup;
		ciSubType=_ciSubType;
		wiTmplId =_wiTmplId;
	}
	TMPLID(WORD wiGroupType,WORD _wiTmplId){
		BYTE* pcbGroupType=(BYTE*)&wiGroupType;
		ciSubType=*pcbGroupType;
		ciGroup=*(pcbGroupType+1);
		wiTmplId =_wiTmplId;
	}
	operator DWORD(){return *((DWORD*)this);}
};
class IModel;
class CLDSNode;
class CLDSBolt;
class CLDSLinePart;
class CLDSLineAngle;
class CLDSParamPlate;

struct IStdComTempl
{
	struct TEMPL_SOURCE{
		char ciSourceType;	//0.表示由idStdComTempl来确定;1.表示由指定文件来确定
		UINT idStdComTempl;
		UINT dwTemplParam;
		CXhChar200 szTemplFilePath;
		TEMPL_SOURCE(char _ciSourceType=0,UINT _idStdComTempl=1,UINT _dwTemplParam=0)
		{
			ciSourceType=_ciSourceType;
			idStdComTempl=_idStdComTempl;
			dwTemplParam=_dwTemplParam;
		}
	};
	struct IMAGE_INFO{
		char ciSrcType;	//0.表示来源于进程资源，通过资源ID来获取;1.表示指定(相对)目录下的图像文件名称
		union{
			UINT idResImage;
			char szImgFile[51];	//位于".\images\stdtmpl_images\"相对目录下
		};
	};
public:	//接口函数申明
	virtual UINT GetTmplId()=0;
	virtual const char* GetName()=0;
	virtual IModel* SetBelongModel(IModel* pModel)=0;
	virtual void FromBuffer(BUFFER_IO* pIO,DWORD version=0)=0;
	virtual void ToBuffer(BUFFER_IO* pIO,DWORD version=0)=0;
	virtual bool CloneStdTmplFrom(IStdComTempl* pStdComTmpl)=0;
	virtual bool IsWireStruTmpl(){return false;}
	virtual bool GetSketchImage(IMAGE_INFO* pSketchImage)=0;
	virtual void InitComponentTempl(TEMPL_SOURCE* pTemplSource=NULL)=0;
	virtual bool ClearTmplParams(){return false;}
	virtual bool IsMeetConditions()=0;
	virtual BOOL DeleteMappingNode(long idTemplNode)=0;
	virtual long MappingLdsNodeH(long idTemplNode)=0;
	virtual long SetMappingNode(long idTemplNode,long hLdsNode)=0;
	virtual bool SetMappingNode(long idTemplNode,CLDSNode *pNode)=0;
	virtual long ToLdsHandle(DWORD idObj)=0;
public:
	BYTE get_ciGroupType(){
		TMPLID tmplid=GetTmplId();
		return tmplid.ciGroup;
	}
	__declspec(property(get=get_ciGroupType)) BYTE ciGroupType;
	static const BYTE STDTMPL_FRAMELINE		= 0x00;
	static const BYTE STDTMPL_STRUCTURE		= 0x01;
	static const BYTE STDTMPL_CONNPOINT		= 0x02;
	static const BYTE STDTMPL_NODEPLATE		= 0x03;
	//单线框架组件
	static const DWORD STDTMPL_FRAMELINE_HYJ1	= 0x00000001;	//HYJ1
	static const DWORD STDTMPL_FRAMELINE_HX3	= 0x00000002;	//HX3

	static const DWORD STDTMPL_STRU_TUBE_WALKWAY	= 0x01000001;	//钢管塔走道架
	static const DWORD STDTMPL_STRU_TUBE_WORKBENCH	= 0x01000002;	//钢管塔上环形站位平台架
	//static const DWORD STDTMPL_STRU_TUBE_SAFEHOLDER	= 0x01000003;	//钢管塔上环形安全带挂点
	static const DWORD STDTMPL_STRU_HOOF_PLATES		= 0x01050001;	//组合角钢双变单，牛蹄板变坡下为一组倒K材
	static const DWORD STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM1	= 0x01020001;	//EB    悬垂型直线塔地线挂点支架
	static const DWORD STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM2	= 0x01020002;	//EB耐张悬垂型直线塔地线挂点支架
	static const DWORD STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM3	= 0x0102000E;	//EB耐张悬垂型上三角横担地线挂点支架
	static const DWORD STDTMPL_STRU_EB_EARTH_WIRE_ARM			= 0x01020003;	//EB型耐张塔地线挂点支架
	static const DWORD STDTMPL_STRU_EB_EARTH_WIRE_PLATE			= 0x01020004;	//EB型耐张塔地线挂点顶面板
	static const DWORD STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM1		= 0x01020005;	//EB悬垂型直线塔导线I串挂线(三角尖)
	static const DWORD STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM2		= 0x01020006;	//EB悬垂型直线塔导线I串挂线(三角横担平面)
	static const DWORD STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM3		= 0x01020007;	//EB悬垂型直线塔导线I串挂线(水平横担)
	static const DWORD STDTMPL_STRU_EB_WIRE_V_ARM1				= 0x01020008;	//EB型单挂点直线塔导线V串挂线(横担曲臂连接处)
	static const DWORD STDTMPL_STRU_EB_WIRE_V_ARM2				= 0x01020009;	//EB型双挂点直线塔导线V串挂线(横担曲臂连接处)
	static const DWORD STDTMPL_STRU_EB_WIRE_V_ARM3				= 0x0102000A;	//EB型单挂点直线塔导线V串挂线(方形横担)
	static const DWORD STDTMPL_STRU_EB_WIRE_V_ARM4				= 0x0102000B;	//EB型单挂点直线塔导线V串挂线(地线横担外侧挂点)
	static const DWORD STDTMPL_STRU_EB_WIRE_V_ARM5				= 0x0102000C;	//EB型单挂点直线塔导线V串挂线(塔身)
	static const DWORD STDTMPL_STRU_EB_WIRE_V_ARM6				= 0x0102000D;	//EB型单挂点直线塔导线V串挂线(上相导线V串内侧挂点)

	//static const DWORD STDTMPL_STRU_GD_WIRE_ARM					= 0x01030001;	//GD导线挂点支架
	//static const DWORD STDTMPL_STRU_GD_WIRE_ARM1				= 0x01030002;	//GD导线挂点支架（短角钢在侧面）
	//static const DWORD STDTMPL_STRU_GD_WIRE_ARM2				= 0x01030003;	//GD导线挂点支架（短角钢在底面）
	static const DWORD STDTMPL_STRU_META_SECT_BTM_1_EB			= 0x01030004;	//直线塔横隔材单EB挂点（短角钢在底面）
	static const DWORD STDTMPL_STRU_META_SECT_BTM_2_EB			= 0x01030005;	//直线塔横隔材双EB挂点（短角钢在底面）
	static const DWORD STDTMPL_STRU_META_SECT_FRT_1_GD			= 0x01030006;	//耐张塔塔横隔材正面单GD挂点（短角钢在正面）
	static const DWORD STDTMPL_STRU_META_SECT_BTM_1_GD			= 0x01030007;	//耐张塔塔横隔材底面单GD挂点（短角钢在底面）
	
	//static const DWORD STDTMPL_STRU_V_OUTER_1_EB				= 0x01030008;	//直线塔V串边相单EB挂点
	//static const DWORD STDTMPL_STRU_V_OUTER_2_EB				= 0x01030009;	//直线塔V串边相双EB挂点
	static const DWORD STDTMPL_STRU_NZ_FRT_2_GD					= 0x0103000A;	//耐张塔正面双GD挂点（短角钢在正面）
	static const DWORD STDTMPL_STRU_NZ_BTM_2_GD					= 0x0103000B;	//耐张塔底面双GD挂点（短角钢在底面）
	static const DWORD STDTMPL_STRU_NZ_BTM_3_GD					= 0x0103000D;	//耐张塔底面叁GD挂点（短角钢在底面）
};

struct TMPLOBJID{
	DWORD dwId;
	TMPLOBJID(DWORD _id=0){dwId=_id;}
	TMPLOBJID(KEY4C keyid){dwId=keyid.dwItem;}
	TMPLOBJID(DWORD _id,char _ciMirTransType);
	operator DWORD(){return dwId;}
	operator KEY4C(){return dwId;}
	char set_ciMirTransType(char ciMirType);
	char get_ciMirTransType();
	//根据字符串哈希出的Id后，自动识别的轴对称对象的标识，0表示原始对象,'X','Y','Z'分别表示相应轴对称
	__declspec(property(put=set_ciMirTransType,get=get_ciMirTransType)) char ciMirTransType;
	bool set_bLongStrId(bool bLongStr);
	bool get_bLongStrId();
	//根据字符串哈希Id时如果字符串不超过4(含4)个字符为短标识自动转换，超过后哈希计算(可能存在重叠)
	__declspec(property(put=set_bLongStrId,get=get_bLongStrId)) bool bLongStrId;
	UINT set_uiLongStrSerial(UINT uiSerial);
	UINT get_uiLongStrSerial();
	//根据字符串哈希出的Id重复时，自动设定的附加序号
	__declspec(property(put=set_uiLongStrSerial,get=get_uiLongStrSerial)) UINT uiLongStrSerial;
	//UINT set_uiOriginalId(UINT uiOrgId);
	UINT get_uiOriginalId(){return dwId&0x7f7fffff;}
	//返回或设置
	__declspec(property(get=get_uiOriginalId)) UINT uiOriginalId;
};
union CMPSTID{
	UINT uidRod;	//边界杆件或普通连接杆件
	struct{	//标识组合角钢子角钢的Id
		short sidGroupRod;	//组合角钢在模板中的主控Id
		short sidSerial;	//子角钢在组合角钢中的索引序号(1 based serial)
	};
	CMPSTID(UINT uidCmplxId=0){
		uidRod=uidCmplxId;
	}
	CMPSTID(short sidCmpstId,short siSerialIndex){
		sidGroupRod=sidCmpstId;
		sidSerial=siSerialIndex;
	}
	operator UINT(){return uidRod;}
};
struct ListTmplIdNode{
	TMPLOBJID id;
	ListTmplIdNode* next;
	char label[8];
public:
	ListTmplIdNode(DWORD hashid=0,const char* szLabel=NULL);
	~ListTmplIdNode();
	ListTmplIdNode* AppendListNode(const char* szLabel);
	UINT MappingId(const char* szLabel);
protected:
	bool IsEqual(const char* szLabel);
};
class CTmplIdManager
{
protected:
	CHashList<ListTmplIdNode> idcenter;
	UINT HashId(const char* id_str,int nidstr=0);
public:
	CTmplIdManager();
	UINT AddLabel2Id(const char* label_str);
	UINT Label2Id(const char* label_str);
};

class CStdTemplParaRod;
class CStdTemplParaPlate;
class CStdTemplPart{
public:
	KEY4C keyLabel;	//标签（字符形式的标识可用于参数化建模时代替Id索引，有别于构件编号）
	char cMaterial;	//'S':Q235;'H':Q345;'G':Q390;'T':Q420;'U':Q460
	char sPreferLayer[4];	//由模板指定的优化图层名
	BYTE cbMirCreateFlag;		//对称生成信息,0x01:X轴对称;0x02:Y轴对称;0x04:Z轴对称
	long m_idPartClsType;		//该杆件的构件类型，如角钢、钢管、钢板等, CLS_LINEANGLE,CLS_LINETUBE
	char exist_logicexpr[20];	//该节点对象是否存在的逻辑表达式，目前该表达式中只能出现一次'<'、'>'、'='、'<='、'>='、'!='逻辑操作符
public:
	CStdTemplPart(){
 		memset(this,0,sizeof(CStdTemplPart));
		m_idPartClsType=4;//CLS_PART;
		strcpy(exist_logicexpr,"1=0");
	}
	void SetKey(DWORD key){keyLabel=key;}
	UINT get_Id(){return keyLabel.dwItem;}
	__declspec(property(get=get_Id)) UINT Id;
public:
	struct TAG_ROD_INFO : public TAG_INFO<CStdTemplParaRod>
	{
		CLDSLinePart* pModelRod;
		TAG_ROD_INFO(){pModelRod=NULL;}
	};
	struct TAG_PLATE_INFO : public TAG_INFO<CStdTemplParaPlate>
	{
		CLDSParamPlate* pModelPlate;
		TAG_PLATE_INFO(){pModelPlate=NULL;}
	};
};

class CStdTemplParaNode{
public:
	//坐标计算类型，attach_type
	static const BYTE INDEPENDENT = 0;	//坐标无任何依赖的节点;
	static const BYTE COORD_X_SPECIFIED = 1;	//杆件上X坐标值不变点;
	static const BYTE COORD_Y_SPECIFIED = 2;	//杆件上Y坐标值不变点;
	static const BYTE COORD_Z_SPECIFIED = 3;	//杆件上Z坐标值不变点;
	static const BYTE INTERSECTION = 4;	//两杆件交叉交点;
	static const BYTE SCALE = 5;	//杆件上的比例等分点;
	static const BYTE PLANE_XY_SPECIFIED = 6;	//线面求交点
	static const BYTE PLANE_YZ_SPECIFIED = 7;	//线面求交点
	static const BYTE PLANE_XZ_SPECIFIED = 8;	//线面求交点
	static const BYTE OFFSET = 9;	//偏移点
	//
	struct TAG_NODE_INFO : public TAG_INFO<CStdTemplParaNode>
	{
		CLDSNode* pModelNode;
		TAG_NODE_INFO(){pModelNode=NULL;}
	};
protected:
	TAG_NODE_INFO* pTagInfo;
public:
	CLDSNode* pRelaNode;
	GEPOINT xPosition;
	bool m_bNeedUpdateRelaInfo;
	bool m_bNecessaryNode;		//标准组件中的必要节点（即杆塔模型中必须已存在相应的匹配节点)
	bool m_bOptionExterNode;	//标准组件中的非必要节点（杆塔模型中的可选节点，不会在标准组件中生成的节点)
	char sPreferLayer[3];		//由模板指定的优化图层名
	KEY4C keyLabel;				//节点的模板标识
	void SetKey(DWORD key){keyLabel=key;}
	UINT get_Id(){return keyLabel.dwItem;}
	__declspec(property(get=get_Id)) UINT Id;
	KEY4C keyLabelFatherRod;	//归属父杆件的模板标识
	char posx_expression[50];	//
	char posy_expression[50];
	char posz_expression[50];
	char scale_expression[50];
	char offset_expression[50];
	char exist_logicexpr[20];	//该节点对象是否存在的逻辑表达式，目前该表达式中只能出现一次'<'、'>'、'='、'<='、'>='、'!='逻辑操作符
	KEY4C keyRepeatParam;		//>0时表示代表该节点重复次数的CBlockParaModel::listParams参数键值，重复参数为"i"；其值为重复节点所归属的节点重复轴线的标识
	/*
	0-坐标无任何依赖的节点;
	1-杆件上X坐标值不变点;
	2-杆件上Y坐标值不变点;
	3-杆件上Z坐标值不变点;
	4-两杆件交叉交点;
	5-杆件上的比例等分点;
	6-线面求交点
	7-偏移点
	*/
	char ciPosCalType;	
	union{
		char ciOffsetType;	//0:长度;'X':X坐标分量;'Y':Y坐标分量;'Z':Z坐标分量
		char ciMirTransType;//
	};
	double attach_scale;	//正值表示小数形式的比例位置值,负值表示分子.分母
	WORD wiOffsetByPrevNode;	//i>ciOffsetByPrevNode时:表示自前一偏移节点为基准进行偏移
	TMPLOBJID attachNodeArr[4];
public:
	CStdTemplParaNode();
	virtual ~CStdTemplParaNode();
	//
	virtual void ToBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual void FromBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	void SetTagInfo(void* pTag){pTagInfo=(TAG_NODE_INFO*)pTag;}
	TAG_NODE_INFO* TagInfo(){return pTagInfo;}
};
struct BOLT_ORIGIN{
	//0.杆件当前端点(角钢楞点);1.节点投影截面;2.节点平推截面;3.与另一杆件的心线交点;4.板上另一颗螺栓;
	//5.保证与参照交叉材容许间隙基础上布置
	static const BYTE LOCA_RODEND	= 0;
	static const BYTE LOCA_RODNODE	= 1;
	static const BYTE LOCA_AXISINTERS = 3;
	static const BYTE LOCA_KEEPSPACE2ROD = 5;
	BYTE ciPosCalStyle;
	BYTE ciPlaneType;		//0.X-Y平面;1.Y-Z平面;2.X-Z平面
	static const BYTE POS_G		= 0;
	static const BYTE POS_G1	= 1;
	static const BYTE POS_G2	= 2;
	static const BYTE POS_G3	= 3;
	static const BYTE POS_WdS	= 5;	//W deduct(-) S（最小间隙或边距）
	static const BYTE POS_nS	= 6;	//negative(-)S
	static const BYTE POS_WaS	= 7;	//W add(+) S
	BYTE ciStyleG;		//0.g;1.g1;2.g2;3.g3;5.W-S;6.-S;7.W+S;S表示螺栓到肢边的最小边距或间隙(取决于螺栓直径)
	BYTE ciStyleCrossG;	//widCrossBoltAngle所指向交叉角钢的心距
	DWORD widBaseNodeOrBolt;//0:表示当前连接板基准节点或基点依据螺栓标识;
	CMPSTID widBaseAngle;	//与ciStyleG对应，	  0:表示不需要端连接;<0xffff表示通过螺栓与指定角钢直连;0xffff表示与定位基准角钢直连
	CMPSTID widCrossAngle;	//与ciStyleCrossG对应,0:表示不需要端连接;<0xffff表示通过螺栓与指定角钢直连;0xffff表示与定位基准角钢直连
	static int CalWidthByStyleG(int niMd,double dfWidth,BYTE ciStyleG=0);
	BOLT_ORIGIN(){widBaseNodeOrBolt=widBaseAngle=widCrossAngle=ciPosCalStyle=ciPlaneType=ciStyleG=ciStyleCrossG=0;}
};
struct BOLT_LOCATION
{
	BOLT_ORIGIN basepoint;
	char ciDatumWing;	//所在的角钢肢	'X' or 'Y'
	static const char FLOW_StoE	= 0;
	static const char FLOW_EtoS	= 1;
	char ciDirection;	//纵向的正方向，0.杆件始->终;1.反向
	short siLenOffset,siWingOffset;	//螺栓纵向及横向偏移量
	CLDSBolt* pRelaBolt;
	GEPOINT xTempPos;	//临时计算出的螺栓基点位置，以便匹配螺栓用
	BOLT_LOCATION(const double* pxTempPos = NULL) {
		siLenOffset = siWingOffset = ciDatumWing = 0;
		ciDirection = FLOW_StoE;
		pRelaBolt = NULL;
		xTempPos = GEPOINT(pxTempPos);
	}
	bool TestCalBoltPosition(CLDSLineAngle* pDatumAngle,CLDSNode* pBaseNode=NULL);
	void WriteParamToBolt(CLDSBolt* pDestBolt,CLDSLineAngle* pDatumAngle,CLDSNode* pBaseNode=NULL);
};
class CLDSLineAngle;
struct BOLT_LAYOUT{
	BOLT_ORIGIN basepoint;
	BYTE ciBoldMd;			//布置螺栓的直径
	BYTE cnCount;			//按照该布局模式布置的螺栓数,0.表示剩余的所有螺栓
	BYTE ciRows;			//0.自动判断排数;1.单排;2.双排
	BYTE cnMaxSingularLineBolts;//允许多排心线时，单排布孔最多螺栓数
	CMPSTID idDirectConnectToAngle1, idDirectConnectToAngle2;	//直接连接的其余角钢
	static const BYTE LAYOUT_ONESIDE  = 0;
	static const BYTE LAYOUT_DUALSIDE = 1;
	static BYTE MAX_SINGULARLINE_BOLTS;
	BYTE ciLayoutStyle;		//0.单侧布置;1.双侧对称布置;2.接头型式的两肢交替单向布置
	bool blPreferBerSide;	//楞线侧优先布置
	char ciDatumWing;		//所在的角钢肢:'X' or 'Y'
	char ciTipS0E1;	//当前连接端,0.表示始端;1.表示终端
	static const char FLOW_StoE	= 0;
	static const char FLOW_EtoS	= 1;
	char ciFlowDirection;	//螺栓纵向布置的走向，0.杆件始->终;1.反向
	char szLenOffsetOf1stBolt[8];	//首颗螺栓自定位基点的纵向偏移量,"L0"单侧布置时默认为端距,双侧布置时默认为0，"LE"端距,"LS"单排螺栓间距,"LD"双排螺栓间距
public:
	BOLT_LAYOUT(char _ciLayoutStyle=0,char _cnCount=0,char _ciRows=0);
	//螺栓朝向
	char get_ciSafeDirection();
	__declspec(property(get=get_ciSafeDirection)) char ciSafeDirection;
	//布置螺栓
	UINT LayoutBolts(short g[4],IXhList<BOLT_LOCATION>* pListBoltLocations,double fEndSpace,double fBaseSpace,double fDoubleRowSpace);
	UINT LayoutBolts(long g[4],IXhList<BOLT_LOCATION>* pListBoltLocations,double fEndSpace,double fBaseSpace,double fDoubleRowSpace);
	//
	void ToBuffer(BUFFER_IO *pIO, DWORD dwVersion = 0);
	void FromBuffer(BUFFER_IO *pIO, DWORD dwVersion = 0);
};
struct CONNROD_FLAG{
	//0x01继承工作肢;0x02表示工作肢筛选;0x04里外铁筛选;0x08筛选杆件截面类型;
	//0x10保证最小螺栓数;0x20筛选水平横材杆件
	BYTE _cbValue;
public:
	CONNROD_FLAG(BYTE cbConnRodFlag=0x01){_cbValue=cbConnRodFlag;}
	operator BYTE(){return _cbValue;}
public:
	//是否继承工作肢的一些关联属性（角钢工作肢可根据实际情况变换）
	bool set_bInheritWorkWing(bool bFilterVal);
	bool get_bInheritWorkWing(){return (_cbValue&0x01)>0;}
	__declspec(property(put=set_bInheritWorkWing,get=get_bInheritWorkWing)) bool bInheritWorkWing;
	//当前设定工作肢是否为筛选条件
	bool set_bFilterWorkWing(bool bFilterVal);
	bool get_bFilterWorkWing(){return (_cbValue&0x02)>0;}
	__declspec(property(put=set_bFilterWorkWing,get=get_bFilterWorkWing)) \
		bool bFilterWorkWing;	//当前设定工作肢是否为筛选条件
	//当前角钢里外铁是否为筛选条件
	bool set_bFilterInsideAngle(bool bFilterVal);
	bool get_bFilterInsideAngle(){return (_cbValue&0x04)>0;}
	__declspec(property(put=set_bFilterInsideAngle,get=get_bFilterInsideAngle)) bool bFilterInsideAngle;
	//当前杆件截面类型是否为筛选条件
	bool set_bFilterSectType(bool bFilterVal);
	bool get_bFilterSectType() { return (_cbValue & 0x08) > 0; }
	__declspec(property(put = set_bFilterSectType, get = get_bFilterSectType)) bool bFilterSectType;
	//当前杆件最少连接螺栓数是否为筛选条件
	bool set_bFilterConnBoltN(bool bFilterVal);
	bool get_bFilterConnBoltN() { return (_cbValue & 0x10) > 0; }
	__declspec(property(put = set_bFilterConnBoltN, get = get_bFilterConnBoltN)) bool bFilterConnBoltN;
	//当前杆件为水平横材是否为筛选条件
	bool set_bFilterHoriPrimaryRod(bool bFilterVal);
	bool get_bFilterHoriPrimaryRod() { return (_cbValue & 0x20) > 0; }
	__declspec(property(put = set_bFilterHoriPrimaryRod, get = get_bFilterHoriPrimaryRod)) bool bFilterHoriPrimaryRod;
};
struct SCHEMA_ROD{
	WORD widRod;		//1 based index;
	char ciRodType;		//'A':Across，杆件跨越基准节点;'R':Radiation杆件从基准节点射线发出
	char szRodSectType[4];	//杆件截面类型,'L','T','D','X'
	char ciNecessity;	//必要性：0.无特殊规定;1.必要;2.可有可无
	bool get_blNecessary() { return ciNecessity==1; }
	__declspec(property(get=get_blNecessary)) bool blNecessary;	//与连接点模板匹配时，模型中必须有的杆件 wjh-2019.8.20
	char ciPlaneNo;		//归属面号：0.自动判断(暂不支持);1.基面;2.第一制弯面;3.第二制弯面
	BYTE cnMinBoltN;	//须满足的最少螺栓数
	//对于跨越角钢杆件判定肢朝向，需要始->终方向，此时以图像X轴正向或由上至下为准
	ANGLE_WORKWING ciAngleWorkWing;	//'X','x','Y','y',大写为外铁，小写为里铁对应(角钢的布置方式1,3,4,2);<=0表示不指定肢朝向，相对于贴合肢法线看
	bool dualSideEdge;	//是否同时影响始末两侧钢板边界;
	CONNROD_FLAG xFlag;
	short siMinRayAngle,siMaxRayAngle;	//射线起始(最小)与终止(最大)角，相对于图面视觉上逆时针相对于X轴的夹角（相当于X->负Y）。
	//CLDSLinePart* pMapRod;
	long hMapRod;
	struct ANGLE_LOCATION{
		char ciMasterIndex;	//ciRodSetType为组合角钢时，此值表示基准子角钢的索引号，0 base index
		char ciLocationType;	//0.两肢坐标控制;1.交于平面基点;2.交于心线（含制弯线）;3.交于偏移准线
		WORD widBendlineAngle;	//制弯线依据角钢
		ANGLE_LOCATION(){widBendlineAngle=ciMasterIndex=ciLocationType=0;}
	}angle;
	SCHEMA_ROD(){
		ciPlaneNo=1;
		hMapRod=0;//pMapRod=NULL;
		dualSideEdge=false;
		cnMinBoltN = 1;
		szRodSectType[0]='L';
		szRodSectType[1]=szRodSectType[2]=szRodSectType[3]=0;
		ciRodType='R';ciNecessity=ciAngleWorkWing=0;widRod=siMinRayAngle=siMaxRayAngle=0;
	}
};
class CStdTemplParaRod : public CStdTemplPart{
protected:
	TAG_ROD_INFO* pTagInfo;
	bool m_bShortAngle;	//缺少端节点的附加短角钢
public:
	char m_ciNecessaryRod;		//标准组件中的必要杆件（即杆塔模型中必须已存在相应的匹配杆件)
	CLDSLinePart* pRelaRod;
	struct EDGEROD{
		bool bReverseDirection;
		float lenpos;
		CLDSLinePart* pMatchRod;
		EDGEROD(CLDSLinePart* pRod=NULL,float lenpos_x=0,bool reverse=false){
			pMatchRod=pRod;
			lenpos=lenpos_x;
			bReverseDirection=reverse;
		}
	};
	PRESET_ARRAY4<EDGEROD> relaRods;
	bool m_bNeedUpdateRelaInfo;
	bool m_bNeedUpdateNormal;	//对已存在的杆件是否需要更新工作法向（角钢肢朝向）
	BYTE m_cbNeedUpdatePosition;	//对已存在的杆件是否需要更新位置
	bool set_bNeedUpdatePosition(bool bUpdatePos);
	bool set_bUpdateStartPos(bool bUpdatePos);
	bool get_bUpdateStartPos();
	bool set_bUpdateEndPos(bool bUpdatePos);
	bool get_bUpdateEndPos();
	//是否更新坐标
	__declspec(property(put=set_bNeedUpdatePosition))	  bool m_bNeedUpdatePosition;
	//更新始端坐标
	__declspec(property(put=set_bUpdateStartPos,get=get_bUpdateStartPos))	  bool bUpdateStartPos;
	//更新终端坐标
	__declspec(property(put=set_bUpdateEndPos  ,get=get_bUpdateEndPos  ))	  bool bUpdateEndPos;
	char get_ciNecessaryRod();
	char set_ciNecessaryRod(char _ciNecessaryRod);
	__declspec(property(put=set_ciNecessaryRod  ,get=get_ciNecessaryRod  ))	  char ciNecessaryRod;	//false(0):非必须杆件;1:必有关键杆件;2:必有(缺少时自动添加)
	bool get_blNecessaryRod() { return m_ciNecessaryRod>0; }
	bool set_blNecessaryRod(bool _blNecessaryRod) { 
		m_ciNecessaryRod=_blNecessaryRod?0x01:0; 
		return m_ciNecessaryRod!=0;
	}
	__declspec(property(put=set_blNecessaryRod  ,get=get_blNecessaryRod  ))	  bool m_bNecessaryRod;	//false(0):非必须杆件;1:必有关键杆件;2:必有(缺少时自动添加)
	bool get_bShortAngle();
	bool set_bShortAngle(bool _bShortAngle);
	__declspec(property(put=set_bShortAngle  ,get=get_bShortAngle  ))	  bool blShortAngle;
	GEPOINT xStartPoint,xEndPoint,xMidPoint;
	GEPOINT vLocalNearVecS2E;	//杆件始->终轴向的近似值, 在从属模板工作(局部)坐标系下
	GEPOINT vVerifyLenStdVec;	//仅用于验证杆件始末端是否与模板定义相反情况
	WORD wiStartOddCalStyle,wiEndOddCalStyle;	//0.表示根据碰撞参照杆件计算;1.根据螺栓位置计算;2.表示不计算正负头
	short siMinRayAngle,siMaxRayAngle;	//射线起始(最小)与终止(最大)角，相对于图面视觉上逆时针相对于X轴的夹角（相当于X->负Y）。
	union{
		COORD3D eyeSideNorm;	//对于角钢人眼观看侧的法线方向
		COORD3D pickWorkPlane;	//用于确定钢管基准边所在工作面的拾取点 wjh-2015.9.1
	};
	BOOL m_bMajorMat;		//主材
	bool bReverseStartEnd;	//与杆塔模型匹配时实时赋值的变量，不作为模板的固有属性存储
	bool bReverseWingX_Y;	//
	bool bReverseMasterAngle;//组合角钢时，模板预定组合角钢的主角钢序号与实际颠倒
	BYTE cnLiveShareBoltN;	//连接板设计过程中的实时布置共用螺栓数（不作为模板属性一部分存储） wjh-2019.6.13
public:
	CStdTemplParaRod();
	virtual ~CStdTemplParaRod();
	virtual void ToBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual void FromBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual bool IsAngleObject(){return false;}
	void SetTagInfo(void* pTag){pTagInfo=(TAG_ROD_INFO*)pTag;}
	TAG_ROD_INFO* TagInfo(){return pTagInfo;}
	KEY4C keyStartNode,keyEndNode,keyMidNode;	//等同于idStartNode,idEndNode;
	//始端节点键值
	TMPLOBJID get_idStartNode(){return keyStartNode.dwItem;}
	TMPLOBJID set_idStartNode(UINT uidNode){return keyStartNode.dwItem=uidNode;}
	__declspec(property(put=set_idStartNode,get=get_idStartNode)) TMPLOBJID idStartNode;
	//终端节点键值
	TMPLOBJID get_idEndNode(){return keyEndNode.dwItem;}
	TMPLOBJID set_idEndNode(UINT uidNode){return keyEndNode.dwItem=uidNode;}
	__declspec(property(put=set_idEndNode,get=get_idEndNode))	  TMPLOBJID idEndNode;
	//中间节点键值
	TMPLOBJID get_idMidNode(){return keyMidNode.dwItem;}
	TMPLOBJID set_idMidNode(UINT uidNode){return keyMidNode.dwItem=uidNode;}
	__declspec(property(put=set_idMidNode,get=get_idMidNode))	  TMPLOBJID idMidNode;
	CXhChar16 m_sGuige;
	struct NORMAL{
		char ciNormalCalStyle;		//角钢朝向计算方式:0.保持不变（仅更新肢朝向）;1.角钢肢法向2.杆件交叉面法线;//3.构件工作法向;4.构件相对方向
		char ciNormalDirection;		//近似工作法线（工作肢法线最接近的坐标轴）X、Y、Z，（或小写表示负轴方向）,0表示自动判断
		bool blSyncOtherAngleLeg;	//90度更新角钢另一肢
		ANGLE_LAYOUT xAngleLayout;	//'X','x','Y','y',大写为外铁，小写为里铁对应(角钢的布置方式1,3,4,2);<=0表示不指定肢朝向，相对于贴合肢法线看
		UINT uidRefRodOfWorkWing;	//工作肢参照杆件
		NORMAL() { memset(this,0,sizeof(NORMAL)); ciNormalCalStyle = 2; blSyncOtherAngleLeg=true; }
	}normal;
	//杆件截面类型
	char ciSectFlag;	//截面类型低4位表示截面类型，高4位表示组合角钢时基准子角钢索引号
	char get_ciSectStyle();
	char set_ciSectStyle(char ciStyle);
	//0.表示保留原有类型，0='L';1='D';2='T';3='X'表示指定组合类型
	__declspec(property(put=set_ciSectStyle,get=get_ciSectStyle)) char ciSectStyle;
	//基准子角钢的索引号,只有在杆件为组合角钢时才有效
	char get_ciMasterIndex();
	char set_ciMasterIndex(char _ciMasterIndex);
	__declspec(property(put=set_ciMasterIndex,get=get_ciMasterIndex)) char ciMasterIndex;
};
struct GROUPANGLE_PROP{
	BYTE _cbValue;	//0x02,更新组合角钢正负头异(同)步属性为异步，0x03,虚拟角钢与子角钢正负头同步
	GROUPANGLE_PROP(BYTE cbState=0){_cbValue=cbState;}
	operator BYTE(){return _cbValue;}
	bool get_bUpdateOddSynState();
	bool set_bUpdateOddSynState(bool bSynState);
	__declspec(property(get=get_bUpdateOddSynState)) \
		bool bUpdateOddSynState;	//更新虚拟组合角钢与实体角钢间的同步关系属性
	bool set_bSynAnglesOdd(bool bSynState);
	bool get_bSynAnglesOdd();
	__declspec(property(put=set_bSynAnglesOdd,get=get_bSynAnglesOdd)) \
		bool bSynAnglesOdd;	//更新虚拟组合角钢与实体角钢间的同步关系属性
	bool set_bAsynAnglesOdd(bool bAsynState);
	bool get_bAsynAnglesOdd();
	__declspec(property(put=set_bAsynAnglesOdd,get=get_bAsynAnglesOdd)) \
		bool bAsynAnglesOdd;	//更新虚拟组合角钢与实体角钢间的异步关系属性
	BYTE set_ciMasterIndex(BYTE ciIndex);
	BYTE get_ciMasterIndex();
	__declspec(property(put=set_ciMasterIndex,get=get_ciMasterIndex)) \
		BYTE ciMasterIndex;	//与虚拟组合角钢同肢向的实体子角钢索引号
};
class CStdTemplParaSlot : public CStdTemplParaRod{
public:
	WORD wiAttachTube;			//根据依附钢管计算槽钢法线偏移量，hAttachTube>0x20表示该槽钢为脚钉底座
	double fLen;				//根据输入长度及基准点位置计算槽钢始终端位置
	struct COLUMN_INTERS{		//柱面点
		COORD3D sect_norm;		//截面法向
		WORD wiAttachNode;		//截面圆心所在节点
		double len_offset;		//圆心沿轴向的偏移量
		char ciNormalStyle;		//0.水平面;1.钢管垂直截面
		char ciRotFromEdgeStyle;//旋转定位角的起始边定义类型0.ray_vec定义;1.钢管基准边;2.钢管焊道边
		COORD3D ray_vec;		//射线方向
		double ray_angle;		//射线偏移角度
	}datum_pos;
public:
	CStdTemplParaSlot();
	virtual void ToBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual void FromBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
};
class CStdTemplParaAngle : public CStdTemplParaRod{
public:
	//----VVVV----------------------------------------
	//始(末)端节点为0时的角钢为外贴材角钢，外贴材角钢的定位取决于基准依赖角钢
	struct ATTACHANGLE{
		DWORD idBaseAngle;		//短角钢的定位基准角钢
		DWORD idBaseNode;		//短角钢的两端面独立定位节点
		DWORD idEndTipBaseAngle;//短角钢的终端定位基准角钢（为零时取值idBaseAngle)
		DWORD idEndTipBaseNode;	//个别较长短角钢的终端独立定位节点（为零时取值idBaseNode)
		char ciLayer;	//贴合位置层级，0.表示与定位角钢紧贴，越大表示越外层
		char ciDatumAngleWing;	//钢板的定位基准角钢的工作肢，'X','Y','I'(内包铁),'O'(外包铁)
		char ciAssistWing;		//短角钢的非工作肢法向计算:0.表示垂直工作肢;'X'.定位角钢的X肢;'Y'.定位角钢的Y肢
		char ciNormalOffset;	//0.表示自动判断
		char ciAngleLayout;		//外贴形式：0:沿向顺贴;1:沿向倒贴;2.垂向朝外贴;3.垂向朝里贴;4.外包 5.交叉角钢;
		char get_ciFixWorkWing() const;
		__declspec(property(get=get_ciFixWorkWing)) char ciFixWorkWing;	//外贴材与基准角钢的相贴合的角钢工作肢
		char ciLenDirection;	//短角钢始->终近似方向（最接近的坐标轴）X、Y、Z，或小写表示负轴方向（用于ZK14中的包铁无法通过ciAngleWorkWing判断肢朝向时） wjh-2019.11.10
		short siLenStartTipPos,siLenEndTipPos;	//默认偏移正向的顺序优先级为:射线方向>始->终方向
		/*WORD width,width2,thick;	//贴合肢宽，另一肢宽，肢厚
		struct HOLE{
			char ciResideWing;		//'X'或'Y'
			char ciLocationMode;	//0.短角钢中点始->终方向偏移;'S':从始端面开始向终端偏移;'E'从终端面开始向始端偏移
			short siLenOffset;
			char m_ciHoleType;		//0.普通连接螺栓；1.导线挂孔；2.地线挂孔；3.跳线挂孔；4.接地孔；5.施工孔；是否仅为构件上的穿孔（如挂孔、施工孔、接地孔等）
			float sfHoleDiameter;
		};
		PRESET_ARRAY4<HOLE> holes;	//挂线孔
		*/
		ATTACHANGLE();
		//void CloneFrom(ATTACHANGLE* pFrom);
	}attachangle;
	//存在始末端节点的杆件角钢
	struct POS{
		BYTE cTouchLine;			//0.X肢心线;1.Y肢心线;2.楞线;3.0xff自动识别（用于不好判断依赖杆件当前肢情况）
		BYTE cTouchLineType;		//0.心线;1.火曲线;2.偏移准线;3.控制点
		BYTE cFaceOffsetDatumLine;  //平推模式下的自身平推基准线, 0:楞线;'X':X肢心线;'Y':Y肢心线
		short siDatumLenOffset;		//沿搭接基准杆件始->终延伸方向的偏移量
		GEPOINT face_offset_norm;	//平推面法线
		UINT uidTouchLineRod;	//搭接依赖杆件与cTouchLine配合使用，一般为1号基准杆件 wjh-2019.6.12
		UINT uidBendByRod;	//火曲线依赖杆件,0表示默认为端节点父杆件
		BYTE coordCtrlType;	//0:X-Z主控;1:Y-Z主控;2.X-Y主控(仅在spatialOperationStyle=15时有效)3.端点位于节点指定截面(无主控坐标)4.端点位于节点平推截面(无主控坐标)
		union{
			BYTE cMainCoordCtrlWing;//角钢主控肢：'X'肢心线上关键点;'Y'肢心线上关键点;其余楞线上关键点
			BYTE ciSectionPlane;	//0.端点位于节点投影截面;'X'.端点位于节点的Y-Z截面;'Y'.端点位于节点的X-Z截面;'Z'.端点位于节点的X-Y截面
		};
		CXhChar50 szXorCtrlWingOffset;//[4];//WING_OFFSET_STYLE XOrCtrlWing ;角钢主要控制肢(控制两个坐标分量的肢)
		CXhChar50 szYorOthrWingOffset;//[4];//WING_OFFSET_STYLE YOrOtherWing;角钢辅助控制肢(控制一个坐标分量的肢)
		POS() {
			memset(this, 0, sizeof(POS));
			cTouchLine = 0xff; 
			uidTouchLineRod = 1;
		}
	}desStart,desEnd;
public://预定角钢当前工作肢
	char get_ciAngleWorkWing() {
		return normal.xAngleLayout.ciAngleWorkWing;
	}
	char set_ciAngleWorkWing(char ciAngleWorkWing) {
		return normal.xAngleLayout.ciAngleWorkWing=ciAngleWorkWing;
	}
	__declspec(property(put=set_ciAngleWorkWing,get=get_ciAngleWorkWing)) \
		char ciAngleWorkWing;	//'X','x','Y','y',大写为外铁，小写为里铁对应(角钢的布置方式1,3,4,2);<=0表示不指定肢朝向
	char get_ciAngleLayout() {
		return normal.xAngleLayout.ciLayoutStyle;
	}
	char set_ciAngleLayout(char ciLayoutStyle) {
		return normal.xAngleLayout.ciLayoutStyle=ciLayoutStyle;
	}
	__declspec(property(put=set_ciAngleLayout,get=get_ciAngleLayout)) \
		char ciAngleLayout;	//角钢的布置方式1,2,3,4;<=0表示不指定肢朝向
	CONNROD_FLAG xFlag;
	short siMinRayDegAngle,siMaxRayDegAngle;	//该杆件与X轴最小(大)射线夹角范围，均=0，表示不作限制
	GROUPANGLE_PROP cbGroupAngle;	//组合角钢正负头异(同)步属性更新状态
	WORD wiGroupAngleSpace;
	bool dualSideEdge;	//是否同时影响始末两侧钢板边界
	BYTE cnMinBoltN;	//最少连接螺栓数
	BYTE cnMaxSingularLineBolts;	//允许多排心线时，单排布孔最多螺栓数
	//---------------------------------------------
	PRESET_ARRAY4<BOLT_LAYOUT> m_layoutBoltArr;	//螺栓布置参数
public:
	CStdTemplParaAngle();
	virtual void ToBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual void FromBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual bool IsAngleObject(){return true;}
};
class CStdTemplParaBolt{
protected:
	UINT id;
	long m_idPartClsType;	//该杆件的构件类型，如角钢、钢管、钢板等, CLS_LINEANGLE,CLS_LINETUBE
public:
	long m_hBoltFamily;	//归属螺栓规格系列
	static const BYTE HOLE_CONNECT_BOLT	= 0;//0.连接螺栓孔
	static const BYTE HOLE_CONDUCT_WIRE	= 1;//1.导线挂线孔;
	static const BYTE HOLE_EARTH_WIRE	= 2;//2.地线挂线孔;
	static const BYTE HOLE_JUMPER_WIRE	= 3;//3.跳线挂线孔;
	static const BYTE HOLE_GROUND_DIVERSION	= 4;//4.接地导流孔;
	static const BYTE HOLE_ASSEMBLY		= 5;//5.装配孔;
	static const BYTE HOLE_BRAND		= 6;//6.挂牌孔;
	static const BYTE HOLE_WATER		= 7;//7.引流孔
	char m_ciHoleType;	//0.普通连接螺栓；1.导线挂孔；2.地线挂孔；3.跳线挂孔；4.接地孔；5.施工孔；是否仅为构件上的穿孔（如挂孔、施工孔、接地孔等）
	WORD m_wiDiameter;	//螺栓直径（名义直径）,mm
	float m_fHoleIncrement;	//孔径比螺栓直径的实际增量,mm
	int m_iWireGroup;		//归属的挂点组
	BYTE cbMirCreateFlag;	//对称生成信息,0x01:X轴对称;0x02:Y轴对称;0x04:Z轴对称
	struct ORIGIN{
		char ciDatumPointCalStyle;	//0.构件上的局部坐标;1.角钢楞点;2.角钢上指定节点截面;3.心线交点;4.另一螺栓
		UINT idBaseNode;	//定位节点标识
		UINT idBasePart;	//指向杆件时，表示纵向偏移的沿向基准杆件
		UINT idCrossRod;	//心线交点时的另一参照杆件
		UINT idWorkSonJg;	//TODO:组合角钢中的工作子角钢(未经审核) wjh-2019.9.3
		char ciDatumWing;	//基准构件为角钢时的螺栓所在角钢肢，0.X肢;1.Y肢
		char ciLenDirect;	//纵向偏移时的偏移正方向：0.始->终;1.终->始
		char szWingG[4];	//"g","g1","g2","w-30","40"等格式
		char szCrossWingG[4];	//TODO:未经审核 wjh-2019.9.3
		short siLenOffset;	//纵向偏移量,mm
		char szZvOffset;	//螺栓法向偏移量表达式
		//
		ORIGIN(){ memset(this, 0, sizeof(ORIGIN)); }
	}org;
	struct NORMAL{
		char ciNormalCalStyle;	//螺栓朝向计算方式:0.指定;1.角钢肢法向2.杆件交叉面法线;3.构件工作法向;4.构件相对方向
		char ciResidePlane;		//多面板时的螺栓所在制弯平面:0.基面;>=1.相应制弯面
		char ciNormalDirect;	//螺栓工作法线0.由里向外
		WORD widBasePart;		//0.表示与org.widBasePart
		//
		NORMAL() { memset(this, 0, sizeof(NORMAL)); ciNormalCalStyle = 1; }
	}normal;
	struct THROUGH_ITEM{
		static const int PART_THROUGH	 =1;	//穿过构件
		static const int THICK_THROUGH	 =2;	//单纯穿过指定厚度
		char cItemType;	//0.FinalKey;1.穿孔构件Id值;2.厚度值;
		char cPartType;	//0.构件;1.角钢;2.钢板
		WORD val;
		//
		THROUGH_ITEM() {cItemType = 2; val = 0; cPartType = 0;}
	}through[5];	//最多允许穿过5个构件
public:
	CStdTemplParaBolt();
	~CStdTemplParaBolt();
	//螺栓键值
	void SetKey(DWORD key){id=key;}
	UINT get_Id(){return id;}
	__declspec(property(get=get_Id)) UINT Id;
	//孔径
	double get_dfHoleDia();
	double set_dfHoleDia(double diameter);
	__declspec(property(put=set_dfHoleDia,get=get_dfHoleDia)) double dfHoleDia;
	//
	virtual void ToBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual void FromBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	static int CalWingOffDist(int niMd, double dfWingW, char* szWingG);
};
struct CONNECT_ROD{
	CMPSTID idRod;
	short get_sidRod(){return idRod.sidGroupRod;}
	short set_sidRod(short sid){return idRod.sidGroupRod=sid;}
	__declspec(property(put=set_sidRod,get=get_sidRod)) short sidRod;
	char ciCmpstSerial;	//0.表示虚拟角钢;>=1.表示相应序号(如1)号子角钢
	char ciNecessity;	//必要性：0.无特殊规定;1.必要;2.可有可无
	bool blCheckAngleConnWing;	//强制性检查角钢工作肢是否正确
	char ciCurrTipSideS0E1;	//0.始端为当前连接端;1.终端边当前连接端;2.中间连接
	char ciConnectWing;	//0表示自动判断连接肢; 'X' or 'Y',大写表示始端连接小写表示终端连接
	char ciPlaneNo;		//面号:1,2,3
	bool dualSideEdge;	//是否同时影响始末两侧钢板边界
	BYTE cnMinShareBoltN;	//最少的直连接共用螺栓数量

	static const BYTE SPACE_UDF	   =0;	//指定数值边距
	static const BYTE SPACE_BOLTEND=1;	//标准螺栓端距
	static const BYTE SPACE_TOEDGE =2;	//延伸到边缘
	static const BYTE SPACE_TOBER  =3;	//延伸到楞线边缘-肢厚度
	//static const BYTE SPACE_UDF	  =0;	//指定数值边距
	//static const BYTE SPACE_BOLTEND =1;	//标准螺栓端距
	static const BYTE SPACE_ALIGN	  =2;	//标准螺栓端距+半个螺栓间距（用于两肢连板对齐)
	ANGLE_SPACE_FLAG cbSpaceFlag;		//楞线侧边距占第1,2低位;肢翼侧边距占第3,4中位;端距占第5,6中位
	bool bEndLjJg;			//端头直连角钢
	char ciOddCalStyle;		//-1.不作处理;0.表示根据碰撞参照杆件计算;1.根据螺栓位置计算;2.表示不计算正负头;3.从标准正头开始根据碰撞原则往负头方向收缩检测
	union{
		short siSpecOddment;	//指定正负头值，一般仅用于一些特殊地方指定-5或-10保证角钢间隙 wjh-2019.8.23
		short sidOddRefRod[2];	//负端距计算时碰撞依据角钢
	};
	struct DYNROD{
		short sidStartNode,sidEndNode;	//动态端节点定义杆件
		short siRayMinDegAngle,siRayMaxDegAngle;	//widEndNode=0时表示指定范围搜索射线杆件
	}dynaRod;
	//double end_space,wing_space,ber_space;	//定义板外形时的参数
	//螺栓布置信息
	char ciBoltNCalMode;	//-1:角钢端头双剪螺栓数;0.角钢端头单剪螺栓数;1.作为基准角钢时根据射线角钢重新计算螺栓数;
	bool get_blCalBoltNByRayRodsOnBaseRod() { return ciBoltNCalMode>0; }	//作为基准角钢时根据射线角钢重新计算螺栓数
	__declspec(property(get=get_blCalBoltNByRayRodsOnBaseRod)) bool blCalBoltNByRayRodsOnBaseRod;
	BYTE cnMinBoltN;	//最少连接螺栓数
	BYTE cnMaxSingularLineBolts;//允许多排心线时，单排布孔最多螺栓数
	PRESET_ARRAY4<BOLT_LAYOUT> layoutbolts;

public:
	CONNECT_ROD(short idRod=0);
	void InitTipInfo(char ciTipS0E1Else2,char ciOddStyle=0,short sidOddRefRod1=1,short sidOddRefRod2=0);
	void InitTipSpecOddment(char ciTipS0E1Else2,short siSpecifyOddment=0);
	void InitBasicInfo(int idRod,bool bNecessity,BYTE ciFaceNo,int nMinBoltN,bool bDualSideEdge = false);
	void ToBuffer(BUFFER_IO *pIO, DWORD dwVersion = 0);
	void FromBuffer(BUFFER_IO *pIO, DWORD dwVersion = 0);
};
class CStdTemplParaPlate : public CStdTemplPart{
protected:
	TAG_PLATE_INFO* pTagInfo;
public:
	//钢板类型
	static const WORD TYPE_D_TOPPLATE	=1;	//端撑顶板
	static const WORD TYPE_D_ENDPLATE	=2;	//端撑端板
	static const WORD TYPE_D_CHAPLATE	=3;	//端撑插板
	static const WORD TYPE_D_LINKPLATE	=4;	//端撑连板
	static const WORD TYPE_L_NODEPLATE	=5;	//立柱连板
	static const WORD TYPE_L_TOPPLATE	=6;	//立柱顶板
	static const WORD TYPE_L_JIAPLATE	=7;	//立柱夹板
	static const WORD TYPE_Q_TOPPLATE	=8;	//曲梁顶板
	static const WORD TYPE_Q_BASEPLATE	=9;	//曲梁基板
	static const WORD TYPE_Q_RIBPLATE	=10;//曲梁肋板
	static const WORD TYPE_W_TUBECIRPLATE=13;	//钢板支架走道的环向加劲板
	static const WORD TYPE_W_CIRBOLTPLATE=14;	//角钢支架走道的螺栓连接板
	static const WORD TYPE_LINEANGLERIB_PLATE = 12;	//角钢肋板
	static const WORD TYPE_CONNECT_PLATE = 100;	//连接板
	static const WORD TYPE_WASHER_PLATE  = 101;	//螺栓垫板
	static const WORD TYPE_WRAPPER_PLATE	= 102;	//包钢板
	static const WORD TYPE_RIB_PLATE		= 103;	//加强劲肋板
	static const WORD TYPE_ELSE_PLATE	=1000;	//其余板
	static const WORD TYPE_VIRTUAL_PLATE=1001;	//虚拟(无)板连接
	WORD wiType;	//<100参数化板类型;100(CONNECT_PLATE).表示普通钢板;>100表示参数化板的类型
public:
	CDesignLjPara designInfo;
	struct DESIGN_INFO{
		long idBaseRod;
		long idBaseNode;
		long idBasePlate;
		long idAssistPart;
		char ciDatumAngleWing;	//'X','Y分别表示X肢、Y肢
		char ciDatumLenAxis;	//'X','Y'
		char ciProfileStyle;	//外轮廓定义方式,0:标准外形1:定制外形2:包络外形3:自定义外形
		struct BENDPLANE{
			char ciBendMode;	//0.指定制弯面法线;1.指定指制弯角度;2.指定角钢肢法线;3.两杆件交叉法线
			char ciBendEdgeWing;	//'X','x','Y','y':大写表示在楞线侧，小写在肢翼侧
			WORD widAngleBendBy;	//第一个制弯面的制弯线定位基准角钢，向钢板法线内侧曲为正，外侧曲为负，度，0表示平面求交确定火曲线
			BYTE biSelfCalBendline;	//0:指定火曲线|1:面面求交计算
			short siBendlineSpace;	//火曲线与角钢外缘边间的间隙
			union NORMAL{
				COORD3D vBendplaneNormal;
				float fBendDegAngle;	//制弯角，沿制弯线基准角钢始->终为轴逆时针旋转为正(角度制，0表示自动计算)
				struct{	//2.指定角钢肢法线
					DWORD idBendPlaneByAngle;
					char cDatumWingXorY;	//0.自动判断工作肢法线;'X':X肢法线;'Y':Y肢法线
				};
				struct{	//3.两杆件交叉法线
					WORD widBendPlaneRefRod1,widBendPlaneRefRod2;
					char ciDirectRefAxisXorY;//0.与基面法线一致;'X','Y','Z'坐标轴方向，小写表示负轴方向
				};
			}normal;
			BENDPLANE();
		}bends[2];
		BENDPLANE& get_bend() { return bends[0]; }
		BENDPLANE& get_bend2() { return bends[1]; }
		__declspec(property(get=get_bend)) BENDPLANE& bend;
		__declspec(property(get=get_bend2)) BENDPLANE& bend2;
		//char szNormalOffset[8];
	}design;
	struct HOLE{
		BYTE ciResidePlane;
		double x,y;
		float holed;		//孔径
		//WORD wiDistance;	//距离基准角钢的垂直距离
		//short siUpperOffset;//钢板局部Y轴正向侧的偏移距离
		float ciBushTubeDiameter;//套管外径
		float ciBushTubeThick;	//套管壁厚
		float ciBushTubeHeight;	//套管高度
	};
	union{
		struct WIREPLATE{
			char ciCurveEdgeMode;	//0.外凸式;1.波浪式
			char ciEdgeRadius;	//挂孔边距的圆弧半径
			char ciFilletRadius;//挂孔边内凹角处倒角内圆弧半径
		}wireplate;
		struct ANGLEPLATE{
			char ciRibType;	//角钢加强肋板类型
			char ciLocationMode;	//0.短角钢中点始->终方向偏移;'S':从始端面开始向终端偏移;'E'从终端面开始向始端偏移
			short siLenOffset;
			char thick;
			//void Init();
		}anglerib;
	};
	PRESET_ARRAY8<CONNECT_ROD> connrods;
public:
	CXhSimpleList<HOLE> listOnlyHoles;	//虚拟孔定义
	long _thick;
	BOOL _bStdPart;	//TRUE:标准构件 FALSE:非标准构件
	KEY4C keyAssistPart,keyAssistPart2;
	/*ATOM_LIST<ATTACHPART_ITEM> AttachPart;
	CLDSVector heteroFlConnVec[3];	//变坡处异型法兰的三根杆件连接方向
	ATOM_LIST<CONNECT_DIRECTION> ConnDirectList;
	ATOM_LIST<DESIGN_PLATE_ITEM>designItemList;*/
	char off_expression[50];	
	int feature;			//临时使用
public:
	CStdTemplParaPlate();
	~CStdTemplParaPlate();
	virtual void ToBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual void FromBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	//
	void SetTagInfo(void* pTag){pTagInfo=(TAG_PLATE_INFO*)pTag;}
	TAG_PLATE_INFO* TagInfo(){return pTagInfo;}
};
#include "ExpressionValue.h"
struct XHSTR_LEN8 {static const int BUF_LEN =  9;};
typedef CXhCharTempl<XHSTR_LEN8> CXhChar8;
#include "Catlog.h"
struct STDTEMPL_PARAM_ITEM{
	KEY4C key;			//该参数的标识键值（或不超过4个长的字符串）
	UINT uiCategoryId;	//首位为1即uiCategoryId&0x80000000>0表示该参数本身即为目录项
	CXhChar8 szVarName;
	CXhChar50 sLabel;	//属性栏中显示的短名称标签
	CXhChar100 sNotes;	//界面中对该项属性的解释性描述
	char exprstr[51];	//表达式
	EXPRESSION_VALUE value;
public:
	STDTEMPL_PARAM_ITEM();
	void SetKey(DWORD keyid);
};
//标准组件模板定义类
class CStdComTempl : public IStdComTempl
{
protected:
	GECS ocs;
	UINT m_idStdComTmpl;					//模板的标识Id
	IModel* m_pBelongModel;
	bool m_bTmplInitialized;				//表示该模板是否定义完成
	bool m_bEnableIntelliConnPointDesign;	//是否经过反复验证测试可正式应用于智能连接设计命令的标准节点板模板 wjh-2019.4.23
	bool m_bIntelliRecogRodNodes;			//是否开启自动智能识别杆件节点功能(SetMappingNode时调用)
	virtual CXhChar50 _InternalGetSketchImgFileSubFolder();
public:
	class MATCH_CONNECT_POINT {
	public:
		UINT idNode;		//匹配连接点所在的基准节点
		UINT idStdConnPoint;//匹配的标准连接点模式
		UINT idStdNodePlate;//该区配连接点处需要实际设计的节点板标准构造模式
		UINT idBaseRod, idRayRodOnAxisY;	//连接点的基准杆件,Y轴侧的射线杆件
		BYTE cbMirCreateFlag;			//对称生成信息,0x01:X轴对称;0x02:Y轴对称;0x04:Z轴对称
		PRESET_ARRAY8<UINT> xarrMapConnRods;//连接点涉及的射线杆件，为空时表示自动提取连接点射线杆件
	public:
		MATCH_CONNECT_POINT();
	};
	static CXhChar500 szImgRootPath;
	CXhChar50 name;
	CXhChar50 szSketchImgFile;
	XHBITMAP sketchimg;	//示意图
	//1.设计参数描述
	static const BYTE MIR_CREATE_X	= 0x01;
	static const BYTE MIR_CREATE_Y	= 0x02;
	static const BYTE MIR_CREATE_Z	= 0x04;
	static const BYTE MIR_CREATE_ALL= 0x07;
	char m_ciBaseMirType;	//0.无基准对称;'X','Y','Z'时分别表示负id与相应X轴对称数据对象
	char m_ciMirrorMatchMode;	//实际结构与模板间的匹配类型是否为镜像对称匹配
	BYTE m_cbDisableConnPlaneFlag;	//设计当前连接面同时阻止连接点的连接面标识如接头(0x80)
	CHashListEx<CATLOG_ITEM>hashCategoryItems;	//属性栏交互显示录入参数时的目录分级
	CHashListEx<STDTEMPL_PARAM_ITEM>listParams;
	//2.场景判别条件描述
	CHashList<long>hashMatchNodeHid;//键值为组件模型中的Id，数据为对应LDS模型中的节点句柄
	CHashList<long>hashMatchRodHid;	//键值为组件模型中的Id，数据为对应LDS模型中的杆件句柄
	CHashList<long>hashMatchPlates;	//键值为组件模型中的Id，数据为对应LDS模型中的杆件句柄
	CHashList<long>hashMatchBolts;	//键值为组件模型中的Id，数据为对应LDS模型中的螺栓句柄
	CHashList<long>hashMatchFittings;//键值为组件模型中的Id，数据为对应LDS模型中的金具句柄
	//3.杆件方位描述
	//4.实体模型描述
	CHashListEx<CStdTemplParaNode> listParaNodes;
	CSuperHashList<CStdTemplParaRod> listParaRods;
	CHashListEx<CStdTemplParaPlate> listParaPlates;
	CHashListEx<CStdTemplParaBolt> listParaBolts;
	CXhSimpleList<MATCH_CONNECT_POINT> listStdConnPoints;	//结构部件中关联的标准连接点
protected:
	void InitNodePlateTemp_HD1();
	void InitNodePlateTemp_HD1A();
	void InitNodePlateTemp_HD2();
	void InitNodePlateTemp_HD3();
	void InitNodePlateTemp_HD3_2();
	void InitNodePlateTemp_HD3_G5();
	void InitNodePlateTemp_HD4(bool blShrinkRodTrue2False1=true);
	void InitNodePlateTemp_HD5();
	void InitNodePlateTemp_HD6();
	void InitNodePlateTemp_HD7();
	void InitNodePlateTemp_HD7_T();
	void InitNodePlateTemp_HD8();
	void InitNodePlateTemp_HD8_YS();
	void InitNodePlateTemp_HD9();
	void InitNodePlateTemp_HD9_T();
	void InitNodePlateTemp_HD10();
	void InitNodePlateTemp_HD10_T();
	void InitNodePlateTemp_HD3A();
	void InitNodePlateTemp_HD12();
	void InitNodePlateTemp_HD12X();
	void InitNodePlateTemp_HD13();
	void InitNodePlateTemp_HD14();
	void InitNodePlateTemp_BP1or2(long idStdComTmpl);
	void InitNodePlateTemp_BP3();
	void InitNodePlateTemp_ZK14_K1();
	void InitNodePlateTemp_ZK14_K1_SIDE();
	void InitNodePlateTemp_ZK14_K2();
	void InitNodePlateTemp_ZK14_K3();
	void InitNodePlateTemp_ZK14_K3_SIDE();
	void InitNodePlateTemp_DX3();
	void InitNodePlateTemp_DX3A();
	void InitNodePlateTemp_GX1();
	void InitNodePlateTemp_GX2();
	void InitNodePlateTemp_GX3();
	void InitNodePlateTemp_GX4();
	void InitNodePlateTemp_GX5();
	void InitNodePlateTemp_DM1();
	void InitNodePlateTemp_DM1A();
	void InitNodePlateTemp_DM2();
	void InitNodePlateTemp_DM3();
public:
	CStdComTempl(void);
	virtual ~CStdComTempl(void);
	//
	virtual void ToBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual void FromBuffer(BUFFER_IO *pIO,DWORD dwVersion=0);
	virtual bool CloneStdTmplFrom(IStdComTempl* pStdComTmpl);
	static bool IsStdTmplChecked(UINT idStdComTmpl);
	static bool IsStdTmplAuthorized(UINT idStdComTmpl, UINT uiDogKey=0, UINT uiClientSerial= 0);
	virtual bool IsAuthorized(UINT uiDogKey=0, UINT uiClientSerial=0);
	bool get_blEnableIntelliConnPointDesign() const{return m_bEnableIntelliConnPointDesign;}
	__declspec(property(get=get_blEnableIntelliConnPointDesign)) bool blEnableIntelliConnPointDesign;
	bool EnableIntelliRecogRodNodes(bool enabled=true){return m_bIntelliRecogRodNodes=enabled;};
public:	//重载接口函数
	virtual bool get_blTmplIntialized(){return this->m_bTmplInitialized;}
	__declspec(property(get=get_blTmplIntialized)) bool blTmplIntialized;
	virtual UINT SetTmplId(UINT idStdComTmpl){return m_idStdComTmpl=idStdComTmpl;}
	virtual UINT GetTmplId(){return m_idStdComTmpl;}
	__declspec(property(put=SetTmplId,get=GetTmplId)) UINT Id;
	virtual const char* GetName(){return name;}
	virtual IModel* SetBelongModel(IModel* pModel){return m_pBelongModel=pModel;}
	virtual bool GetSketchImage(IMAGE_INFO* pSketchImage);
	virtual bool LoadSketchImage();
	virtual void InitComponentTempl(TEMPL_SOURCE* pTemplSource=NULL);
	virtual void InitComponentTempl(UINT idStdTmpl,UINT dwTmplParam=0){
		TEMPL_SOURCE source(0,idStdTmpl,dwTmplParam);
		InitComponentTempl(&source);
	}
	virtual bool ClearTmplParams();
	virtual bool IsMeetConditions();
	virtual bool RecogMatchRod(CStdTemplParaRod* pParaRod);
	virtual int RecogMatchRods(UINT idRecogRod=0);
	virtual BOOL DeleteMappingNode(long idTemplNode){return hashMatchNodeHid.DeleteNode(idTemplNode);}
	virtual long MappingLdsNodeH(long idTemplNode);
	virtual long SetMappingNode(long idTemplNode,long hLdsNode);
	virtual bool SetMappingNode(long idTemplNode,CLDSNode *pNode);
	virtual long ToLdsHandle(DWORD idObj);
	virtual void Empty();

	//当CParaModelNode已经有挂接对象时执行TagInfo(...), 否则创建新挂接数据对象后返回
	//CStdTemplParaNode::TAG_NODE_INFO* LoadTagInfo(CStdTemplParaNode* pItem);
	//CStdTemplParaRod::TAG_ROD_INFO*  LoadTagInfo(CStdTemplParaRod*  pItem);
	//CStdTemplParaPlate::TAG_PLATE_INFO* LoadTagInfo(CStdTemplParaPlate* pItem);
};
class CStdComTemplConnPoint : public CStdComTempl
{	//普通结构连接点构造模板
	CXhChar50 description;	//仅供内部研发人员临时备注连接点类型时用 wjh-2018.11.29
protected:
	virtual CXhChar50 _InternalGetSketchImgFileSubFolder() { return "Node\\"; }
public:
	struct MATCHROD_INFO{
		bool matched;
		char ciConnTipS0E1Els2;
		WORD wiRayDegAngle;
		CLDSLinePart* pRod;
		MATCHROD_INFO(CLDSLinePart* _pRod=NULL){
			matched=false;
			wiRayDegAngle=ciConnTipS0E1Els2=0;
			pRod=_pRod;
		}
	};
	struct MAPPING_ROD { 
		long idConnPointRod;			//主连接点模板中杆件Id
		union{
			long idNodePlateRod;		//关联匹配节点板模板杆件Id
			long idCoupleConnPointRod;	//配套连接点模板中映射的杆件Id
		};
		MAPPING_ROD(long _hConnRod=0,long _hPlateRod=0) {
			idConnPointRod=_hConnRod;
			idNodePlateRod=_hPlateRod;
		}
	};
	class CMatchStdPlate{
	protected:
		UINT _idStdNodePlate;
	public:
		bool blTmpPassedVerify;	//是否通过合规有效性验证，临时变量非模板特性 wjh-2019.6.12
		UINT get_idStdNodePlate(){return _idStdNodePlate;}
		__declspec(property(get=get_idStdNodePlate)) UINT idStdNodePlate;
	public:
		//索引对应StdNodePlte.connrods,而值对应StdConnPoint.SchemaRod.Id及StdNodePlate中的ParaRod.Id
		PRESET_ARRAY8<long> xarrMapConnRods;
		PRESET_ARRAY4<MAPPING_ROD> xarrReflectMapRods;	//由连接板反射到连接点的杆件映射表
		CMatchStdPlate(){_idStdNodePlate=0;}
		void SetKey(UINT key){
			_idStdNodePlate = key; 
			blTmpPassedVerify = true;
		}
		void InitDefaultMatchRods(int count){
			for(int i=0;i<count;i++)
				xarrMapConnRods.Set(i,i+1);
		}
	};
	struct VIEWPLANE {
		char ciBaseAxisXMode;	//0.始->终方向;1.射线方向;2.射线反方向，即SCHEMA_ROD中角度值的基准X轴(计算起始边）
		char ciViewPlaneNormal;	//观测投影面法线,'X','Y','Z',小写字母表示负方向，0表示不限制,'H'表示水平视向
		short siMinBaseRodAngle, siMaxBaseRodAngle;
		VIEWPLANE() { ciViewPlaneNormal = ciBaseAxisXMode = 0; siMinBaseRodAngle = siMaxBaseRodAngle = 0; }
	}xView;
	struct COUPLE_CONNPOINT{
		UINT idSideStdConnPoint;	//侧面需要匹配的标准连接点，0表示不需要侧面匹配
		UINT idSideStdNodePlate;	//侧面需要匹配的标准连接板，0表示不需要侧面匹配
		SIMPLE_ARRAY<MAPPING_ROD,PRESET_OBJS4> xarrShareRods;	//由连接板反射到连接点的杆件映射表
		COUPLE_CONNPOINT() { 
			idSideStdConnPoint=idSideStdNodePlate=0;
		}
	}xSideConnPoint;
	//
	WORD widParentRod;	//>0需要指定节点父杆件
	WORD widBaseRod;	//连接板的定位依赖杆件
	WORD idRayRodOnAxisY;	//连接点的基准杆件,Y轴侧的射线杆件
	char ciBaseRodSearchMode;	//0.以节点父杆件为基准杆件；1.自动根据工作平面视图搜索定位基准杆件
	char cnConnectPlane;	//连接面数，0表示不作为过滤筛选条件
	bool blSetParentRodOfBaseNode;//自动设定搜索到的定位基准杆件为节点的父杆件
	bool m_bStrictReview;	//必须从杆件类型及数量严格审查该连接点类型
	BYTE ciBaseRodConnState;//0.无限制;1.中间点连接;2.衔接段末端连接;3.衔接段截断点连接;4.共线截断点连接
	CHashListEx<CMatchStdPlate> hashMaybeTmplPlates;	//可能影射到的节点构造模板
	PRESET_ARRAY8<SCHEMA_ROD> rodarr;	//预留8根杆件(含基准杆件)，遇到第一根widRod=0杆件结束 
protected:
	void InitConnPointTempl_HD1();
	void InitConnPointTempl_HD1A();
	void InitConnPointTempl_HD2();
	void InitConnPointTempl_HD3();
	void InitConnPointTempl_HD3_G5();
	void InitConnPointTempl_HD4();
	void InitConnPointTempl_HD5();
	void InitConnPointTempl_HD6();
	void InitConnPointTempl_HD7();
	void InitConnPointTempl_HD8();
	void InitConnPointTempl_HD9();
	void InitConnPointTempl_HD10();
	void InitConnPointTempl_HD3A();
	void InitConnPointTempl_HD12();
	void InitConnPointTempl_HD12X();
	void InitConnPointTempl_HD13();
	void InitConnPointTempl_HD14();
	void InitConnPointTempl_BP1or2(long idStdComTmpl);
	void InitConnPointTempl_BP3();
	void InitConnPointTempl_ZK14_K1();	//上下曲臂主材共线截断
	void InitConnPointTempl_ZK14_K1_SIDE();	//酒杯上下曲臂K节点侧面（单面）
	void InitConnPointTempl_ZK14_K2();	//上下曲臂不断开
	void InitConnPointTempl_ZK14_K3();	//上下曲臂变坡截断
	void InitConnPointTempl_ZK14_K3_SIDE();	//酒杯上下曲臂K节点侧面（双面变坡）
	void InitConnPointTempl_DX3();
	void InitConnPointTempl_DX3A();
	void InitConnPointTempl_GX1();
	void InitConnPointTempl_GX2();
	void InitConnPointTempl_GX3();
	void InitConnPointTempl_GX4();
	void InitConnPointTempl_GX5();
	void InitConnPointTempl_DM1();
	void InitConnPointTempl_DM2();
	void InitConnPointTempl_DM3();
public:
	CStdComTemplConnPoint();
	~CStdComTemplConnPoint() { ; }
	//
	void AddConnPointAcsParam();
	virtual bool GetSketchImage(IMAGE_INFO* pSketchImage);
	virtual void InitComponentTempl(TEMPL_SOURCE* pTemplSource=NULL);
	virtual bool ClearTmplParams();
	virtual bool IntelliRecogRayRods();
	virtual bool UpdateMatchStdPlateTmpl(CStdComTempl* pStdPlateTmpl,BYTE ciMirMode=0);
};
//
class CStdComTemplExpression : public CExpression
{
protected:
	CStdComTempl* m_pContextStdTempl;
public:
	GECS acs;	//标准组件内部的随动定位坐标系（在模型坐标系下的描述），也即标准组件的装配坐标系
	virtual EXPRESSION_FUNC* FromFunctionName(const char* func_label);
	CStdComTemplExpression(CStdComTempl* pStdTempl = NULL, GECS* pAcs = NULL);
	void InitExpressionVarList(CStdComTempl* pStdTempl);
	void SetAcs(GECS cs) { acs = cs; }
	CStdComTempl* get_pStdTempl() { return m_pContextStdTempl; }
	__declspec(property(get = get_pStdTempl)) CStdComTempl* pStdTempl;
};
// CStdComTmplLibraryDlg 对话框
class CStdComTmplLibrary : public IGlobalInitializer
{
	bool m_bTmplLibraryCompiled;
protected:
	CSuperHashList<CStdComTempl> hashStdTmplPool;
	CATLOG_TREEITEM* InsertTreeNodeItem(CATLOG_TREEITEM* pParentItem,UINT idStdComTmpl, const char* szTreeItemName);
	void CompileTmplLibrary();
	void InitStdConnPointTmpls();
public:
	CStdComTmplLibrary();
	bool GlobalInitialze();
	CATLOG_TREEITEM xFrameCatlog,xStruCatlog,xNodeCatlog;
	CStdComTempl* LoadStdComTempl(const char* szStdComTemplId);
	CStdComTempl* LoadStdComTempl(UINT idStdComTempl);
	CStdComTempl* EnumFirstStdFrameTempl();
	CStdComTempl* EnumNextStdFrameTempl();
	CStdComTempl* EnumFirstStdStruTempl();
	CStdComTempl* EnumNextStdStruTempl();
	CStdComTempl* EnumFirstStdNodeTempl(bool bIncConnPoint=true,bool bIncNodePlate=true);
	CStdComTempl* EnumNextStdNodeTempl (bool bIncConnPoint=true,bool bIncNodePlate=true);
};
extern CStdComTmplLibrary globalStdComTmplLibrary;
