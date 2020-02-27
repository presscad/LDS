#pragma once
#include "HashTable.h"
#include "List.h"
#include "ArrayList.h"
#include "f_ent.h"
#include "XhCharString.h"
//#include "StdTemplExpression.h"
#ifndef __PUBLIC_RELEASE_COMPILE_
#include "WirePlaceCode.h"
#else
#include "..\..\TowerCad\WirePlaceCode.h"
#endif
#ifdef __RAPID_ROUGH_3DMODEL_
struct IModelNode{
	long handle;
	long hParentRod;
	char layer[4];
	GEPOINT xPosition;
	virtual void* MappingNode(){return NULL;}
	virtual bool SetLayerRevState(bool modified=true){return modified;}
};
struct IModelRod{
	long handle;
	long hStartNode,hEndNode;
	char layer[4];
	IModelNode *pStart,*pEnd;
	GEPOINT xStartPoint,xEndPoint;
	virtual bool IsAngle(){return true;}
	virtual int GetClassTypeId(){return 0;}
	virtual void* MappingRod(){return NULL;}
	virtual bool SetLayerRevState(bool modified=true){return modified;}
};
class STDTEMPL
{
	WORD m_wnProfileNodeCount;
public:
	char ciClassicType;	//0.普通;'B':酒杯;'M':猫头;'Y':羊角
	//模板外轮廓节点;
	struct PROFILE_NODE{
		//0.孤立点;1.指定X ;2.指定Y;3.指定Z;4.基准面指定XZ;5.射线交叉点;
		//±20:酒杯上下曲臂交界点(正为右侧，负为左侧)
		//±21:酒杯(猫头)中横担中轴线截面上下节点（正为上侧，负为下侧）
		//±22:酒杯中上曲臂一与横担外侧连接节点（正为右侧，负为左侧）
		//±23:酒杯中上曲臂一与横担内侧连接节点（正为右侧，负为左侧）
		//±24:猫头中横担上梁两侧端节点（正为右侧，负为左侧）
		//±25:猫头中胡须处下曲臂外拐角处顶部节点（正为右侧，负为左侧）
		//±26:猫头中胡须处下曲臂内拐角处节点（正为右侧，负为左侧）
		//±27:猫头中横担上梁与两侧地线支架内拐点（正为右侧，负为左侧）
		//  28:800kv直流塔中轴线上交界点,猫头塔自下至上中轴线上的交汇点
		char ciPosCalType;					//0.孤立点;1.指定X ;2.指定Y;3.指定Z;4.基准面指定XZ;5.射线交叉点;
		char ciFractions;					//1.表示表示一个完整相序挂点，N表示1/N个完整挂点
		WIREPLACE_CODE wirecode;			//挂点编码信息
		char ciProfileNode;					//轮廓点,0.非轮廓点;1.轮廓点;2.非必须轮廓点
		WORD widNode;
		WORD widMasterRod;					//节点父杆件
		WORD widLinkPrevRod;				//连接的前一杆件Id;0.表示前面无杆件连接
		struct PREV_LINK{
			short siMinRayAngle,siMaxRayAngle;	//前一根连接杆件的最小与最大射线角（逆时针方向），°
		}prevlink;
		short siMinDegAngle,siMaxDegAngle;	//该顶点处前后轮廓边间夹角范围
		struct RAYCROSS{
			WORD widOrgNode;	//射线原点
			WORD widEdgeStart,widEdgeEnd;		//交叉点边界线起止节点
			WORD siMinRayAngle,siMaxRayAngle;	//射线起始(最小)与终止(最大)角，相对于图面视觉上逆时针相对于X轴的夹角（相当于X->负Y）。
			RAYCROSS(){widOrgNode=widEdgeStart=widEdgeEnd=0;siMinRayAngle=siMaxRayAngle=0;}
		}raynode;
		PROFILE_NODE();
		void SetKey(DWORD key){widNode=(WORD)key;}
	};
	struct PROFILE_ROD{
		WORD widRod,widStartNode,widEndNode;
		bool bHasSectView;	//是否具有展开面
		char ciRodType;		//0.无特殊定义;'H':水平横杆;'V':竖直杆
		char _ciAngleLayout;//1~3位表示角钢的布置方式1,2,3,4;<=0表示不指定肢朝向；5~8位表示指定轴向为近似工作法向(模型坐标系)
		short siMinRayDegAngle,siMaxRayDegAngle;	//该杆件与X轴最小(大)射线夹角范围，均=0，表示不作限制
		struct TIPPOS{
			bool blPresetParams;		//true:预设了杆件端头顶点位置设计信息;false:未设定，不特殊处理
			BYTE cTouchLine;			//0.X肢心线;1.Y肢心线;2.楞线
			BYTE cTouchLineType;		//0.心线;1.火曲线;2.偏移准线;3.控制点;4.指定基准点
			struct DATUMPOINT{
				char ciCoordType;	//0.指定X-Z分量;1.指定Y-Z分量;2.指定X-Z分量
				WORD widPlane;
			}datumpoint;
			BYTE cFaceOffsetDatumLine;  //平推模式下的自身平推基准线
			GEPOINT face_offset_norm;	//平推面法线
			WORD widBendByRod;	//火曲线依赖杆件,0表示默认为端节点父杆件
			BYTE coordCtrlType;	//0:X-Z主控;1:Y-Z主控;2.X-Y主控(仅在spatialOperationStyle=15时有效)
			BYTE cMainCoordCtrlWing;//角钢主控肢：'X'肢心线上关键点;'Y'肢心线上关键点;其余楞线上关键点
			char szXorCtrlWingOffset[4];//WING_OFFSET_STYLE XOrCtrlWing;	//角钢主要控制肢(控制两个坐标分量的肢)
			char szYorOthrWingOffset[4];//WING_OFFSET_STYLE YOrOtherWing;//角钢辅助控制肢(控制一个坐标分量的肢)
			TIPPOS(){memset(this,0,sizeof(TIPPOS));datumpoint.ciCoordType=1;}
		}desPosStart,desPosEnd;
		//角钢的布置方式
		char get_ciAngleLayout(){return _ciAngleLayout&0x07;}
		char set_ciAngleLayout(char ciType){
			_ciAngleLayout&=0xf8;
			_ciAngleLayout|=ciType&0x07;
			return ciType&0x07;
		}
		__declspec(property(put=set_ciAngleLayout,get=get_ciAngleLayout)) char ciAngleLayout;	//角钢的布置方式1,2,3,4;<=0表示不指定肢朝向
		//指定工作面法线近似坐标轴
		char get_ciNearNormAxis(){return _ciAngleLayout>>4;}
		char set_ciNearNormAxis(char ciNearAxis){
			_ciAngleLayout&=0x0f;
			_ciAngleLayout|=(ciNearAxis<<4);
			return ciNearAxis&0x0f;
		}
		__declspec(property(put=set_ciNearNormAxis,get=get_ciNearNormAxis)) char ciNearNormAxis;//指定工作面法线近似坐标轴0.不指定(一般为Y轴);1.X;2.-X;3.Y;4.-Y;5.Z;6.-Z
		WORD widRefRodOfWorkWing;	//工作肢参照杆件
		//-----------------------------------------
		//杆件截面类型
		char ciSectFlag;	//截面类型低4位表示截面类型，高4位表示组合角钢时基准子角钢索引号
		char get_ciSectStyle(){return ciSectFlag&0x0f;}
		char set_ciSectStyle(char ciStyle){
			ciSectFlag&=0xf0;
			ciSectFlag|=ciStyle&0x0f;
			return ciStyle&0x0f;
		}
		__declspec(property(put=set_ciSectStyle,get=get_ciSectStyle)) char ciSectStyle;	//0.表示保留原有类型，1='L';2='T';3='D';4='X'表示指定组合类型
		//基准子角钢的索引号
		char get_ciMasterIndex(){return ciSectFlag>>4;}
		char set_ciMasterIndex(char _ciMasterIndex){
			ciSectFlag&=0x0f;
			ciSectFlag|=(_ciMasterIndex<<4);
			return _ciMasterIndex&0x0f;
		}
		__declspec(property(put=set_ciMasterIndex,get=get_ciMasterIndex)) char ciMasterIndex;	//基准子角钢的索引号,只有在杆件为组合角钢时才有效

		void SetKey(DWORD key){widRod=(WORD)key;}
		PROFILE_ROD();
	};
	struct DYNA_LINKROD{	//动态连接杆件（未在模板中进行固定编号的非边界杆件）
		WORD widRod;	//杆件标识
		WORD widStartNode,widEndNode;	//起始节点，终止节点
		char ciNormalDirection;			//近似工作法线（工作肢法线最接近的坐标轴）X、Y、Z，（或小写表示负轴方向）
		char ciAngleLayout;				//角钢的布置方式1,2,3,4;<=0表示不指定肢朝向
		WORD widRefRodOfWorkWing;		//工作肢参照杆件
		char ciSectStyle;	//0.表示保留原有类型，'T','D','X'表示指定组合类型
		char ciMasterIndex;	//基准子角钢的索引号,只有在杆件为组合角钢时才有效
		void SetKey(DWORD key){widRod=(WORD)key;}
		DYNA_LINKROD();
	};
	class WIREANGLE{
	public:
		WORD widAngle;
		WORD widBaseRod;		//钢板的定位基准角钢
		WORD widBaseNode;		//钢板定位节点
		char ciLayer;	//贴合位置层级，0.表示与定位角钢紧贴，越大表示越外层
		char ciDatumAngleWing;	//钢板的定位基准角钢的工作肢，'X','Y'。
		char ciNormalOffset;	//0.表示自动判断
		char ciAngleLayout;	//角钢的布置方式1,2,3,4;<=0表示不指定肢朝向，相对于贴合肢法线看
		char ciLenDirection;//短角钢始->终近似方向（最接近的坐标轴）X、Y、Z，（或小写表示负轴方向）
		WORD width,width2,thick;	//贴合肢宽，另一肢宽，肢厚
		struct HOLE{
			char ciResideWing;
			BYTE ciHoleDiameter;
			char ciLocationMode;	//0.短角钢中点始->终方向偏移;'S':从始端面开始向终端偏移;'E'从终端面开始向始端偏移
			short siLenOffset;
		};
		PRESET_ARRAY4<HOLE> holes;	//挂线孔
		void SetKey(DWORD key){widAngle=(WORD)key;}
		WIREANGLE();
		void CloneFrom(WIREANGLE* pFrom);
	};
	struct CONNECT_ROD{
		short widRod;	//边界杆件或普通连接杆件
		struct DYNROD{
			short widStartNode,widEndNode;	//动态端节点定义杆件
			short siRayMinDegAngle,siRayMaxDegAngle;	//widEndNode=0时表示指定范围搜索射线杆件
		}dynaRod;
		char ciConnectWing;	//'X' or 'Y',大写表示始端连接小写表示终端连接,0表示自动判断连接肢
		char ciPlaneNo;		//面号:1,2,3
		bool dualSideEdge;	//是否同时影响始末两侧钢板边界
		BYTE cnMinBoltN;	//最少连接螺栓数
		BYTE cnMaxSingularLineBolts;	//允许多排心线时，单排布孔最多螺栓数
		WORD widCrossBoltAngle;	//0:表示不需要端连接;<0xffff表示通过螺栓与指定角钢直连;0xffff表示与定位基准角钢直连
		CONNECT_ROD();
	};
	struct CONNPOINT_SCHEMA{	//连接点模式
		CXhChar50 name;
		struct SCHEMA_ROD{
			WORD widRod;		//1 based index;
			char ciRodType;		//'A':Across，杆件跨越基准节点;'R':Radiation杆件从基准节点射线发出
			char szRodSectType[4];	//杆件截面类型,'L','T','D','X'
			char ciNecessity;	//必要性：0.无特殊规定;1.必要;2.可有可无
			//对于跨越角钢杆件判定肢朝向，需要始->终方向，此时以图像X轴正向或由上至下为准
			char ciAngleLayout;	//角钢的布置方式1,2,3,4;<=0表示不指定肢朝向，相对于贴合肢法线看
			short siMinRayAngle,siMaxRayAngle;	//射线起始(最小)与终止(最大)角，相对于图面视觉上逆时针相对于X轴的夹角（相当于X->负Y）。
			struct ANGLE_LOCATION{
				char ciMasterIndex;	//ciRodSetType为组合角钢时，此值表示基准子角钢的索引号，0 base index
				char ciLocationType;	//0.两肢坐标控制;1.交于平面基点;2.交于心线（含制弯线）;3.交于偏移准线
				WORD widBendlineAngle;	//制弯线依据角钢
				ANGLE_LOCATION(){widBendlineAngle=ciMasterIndex=ciLocationType=0;}
			}angle;
			SCHEMA_ROD(){
				szRodSectType[0]='L';
				szRodSectType[1]=szRodSectType[2]=szRodSectType[3]=0;
				ciRodType='R';ciNecessity=ciAngleLayout=0;widRod=siMinRayAngle=siMaxRayAngle=0;
			}
		};
	public:
		WORD widParentRod;	//>0需要指定节点父杆件
		WORD widBaseRod;	//连接板的定位依赖杆件
		bool m_bStrictReview;	//必须从杆件类型及数量严格审查该连接点类型
		SCHEMA_ROD rodarr[8];	//预留8根杆件，遇到第一根widRod=0杆件结束 
		CONNPOINT_SCHEMA(){widParentRod=widBaseRod=0;}
	};
	struct CONNPOINT{	//连接点模式
		CONNPOINT_SCHEMA schema;
		struct MATCHROD{
			char ciRodType;			//'A':Across，杆件跨越基准节点;'R':Radiation杆件从基准节点射线发出
			char ciRodSectType;		//杆件截面类型,'L','T','D','X'，'O'(表示钢管)
			short siActualRayAngle;	//跨越杆件取值为270<=alfa<360,0<=alfa<90,其余取值0<=alfa<360
			IModelRod* pModelRod;
			MATCHROD(){ciRodType='R';ciRodSectType='L';siActualRayAngle=0;pModelRod=NULL;}
		}rodarr[8];
		WORD matchrod_indexes[8];	//
	};
	class PLATE{
	public:
		WORD widBaseRod;		//钢板的定位基准角钢
		WORD widBaseNode;		//钢板定位节点
		char ciDatumAngleWing;	//钢板的定位基准角钢的工作肢，'X','Y'。
		WORD widBendEdgeAngle1;	//第一个制弯面的制弯线定位基准角钢，向钢板法线内侧曲为正，外侧曲为负，度
		char ciBendEdgeWing1;	//'X','x','Y','y':大写表示在楞线侧，小写在肢翼侧
		double fBendDegAngle1;	//制弯角，沿制弯线基准角钢始->终为轴逆时针旋转为正(角度制，0表示自动计算)
		WORD widBendEdgeAngle2;	//第一个制弯面的制弯线定位基准角钢，向钢板法线内侧曲为正，外侧曲为负，度
		char ciBendEdgeWing2;	//'X','x','Y','y':大写表示在楞线侧，小写在肢翼侧
		double fBendDegAngle2;	//制弯角，沿制弯线基准角钢始->终为轴逆时针旋转为正(角度制，0表示自动计算)
		PRESET_ARRAY8<CONNECT_ROD> connrods;//钢板连接杆件
	public:
		struct HOLE{
			WORD wiDistance;	//距离基准角钢的垂直距离
			short siUpperOffset;//钢板局部Y轴正向侧的偏移距离
			BYTE ciHoleDiameter;//孔径
			BYTE ciBushTubeDiameter;//套管外径
			BYTE ciBushTubeThick;	//套管壁厚
			BYTE ciBushTubeHeight;	//套管高度
		};
		PRESET_ARRAY4<HOLE> holes;	//挂线孔
		char ciCurveEdgeMode;	//0.外凸式;1.波浪式
		char ciEdgeRadius;	//挂孔边距的圆弧半径
		char ciFilletRadius;//挂孔边内凹角处倒角内圆弧半径
	public:
		PLATE();
		void CloneFrom(PLATE* pFrom);
	};
	struct ANGLEPLATE{
		char ciRibType;	//角钢加强肋板类型
		char ciLocationMode;	//0.短角钢中点始->终方向偏移;'S':从始端面开始向终端偏移;'E'从终端面开始向始端偏移
		short siLenOffset;
		char thick;
		ANGLEPLATE();
	};
	//主材四边形组
	struct PROFILE_QUADSECT{
		short arrRods[2];	//取负值时表示计算主材四边形时，仅参照该杆件，但不更改其肢法线
	};
	//模板中包含的基准平面
	struct DATUM_PLANE{
		WORD widPlane;
		struct DATUM_POINT{
			char ciType;	//0.节点;'S'杆件始端点;'E'杆件终端点
			WORD widNodeOrRod;
		}points[3];
		void SetKey(DWORD key){widPlane=(WORD)key;}
	};
	struct MATCHNODE{
	public:
		WORD  widNode;
		short siActualDegAngle;	//当前轮廓顶点处，两相连轮廓边间的内夹角
		long hRelaModelNode;
		IModelNode* pIntelliNode;
		MATCHNODE(){
			widNode=siActualDegAngle=0;
			hRelaModelNode=0;
			pIntelliNode=NULL;
		}
		void SetKey(DWORD key){widNode=(WORD)key;}
	};
	struct EDGEROD{
		bool bReverseDirection;
		float lenpos;
		IModelRod* pIntelliRod;
		EDGEROD(IModelRod* pRod=NULL,float lenpos_x=0,bool reverse=false){
			pIntelliRod=pRod;
			lenpos=lenpos_x;
			bReverseDirection=reverse;
		}
	};
	class MATCHROD{
	public:
		WORD widRod;
		GEPOINT lenImgStdVec;	//图像坐标系下杆件边界延伸方向
		GEPOINT lenModelStdVec;	//模型坐标系下杆件边界延伸方向
		PRESET_ARRAY<EDGEROD,PRESET_OBJS4> relaRods;
		void SetKey(DWORD key){widRod=(WORD)key;}
	};
	DWORD idTempl;
	void SetKey(DWORD key){idTempl=key;}
public:
	char name[51];
	char ciOnRightSide;	//'R':横担在X轴正向右侧; 'L'横担在X轴负向左侧;'U'：塔头在坡段上部
	bool bMaybeHeadZBorZM;	//是否可能为猫头与酒杯（主要为了区分酒杯与猫头，二者外形有个似之处）
	CHashListEx<PROFILE_ROD>  hashParaRods;
	CHashListEx<DYNA_LINKROD> hashDynaLinkRods;
	CHashListEx<WIREANGLE>	  hashWireAngles;
	CHashListEx<PROFILE_NODE> hashParaNodes;
	CHashListEx<DATUM_PLANE> hashParaPlanes;
	CXhSimpleList<PROFILE_QUADSECT>listQuadSects;
	CXhSimpleList<PLATE> listParaPlates;
	CXhSimpleList<ANGLEPLATE> listAnglePlates;
	//CMPL_EXPRESSION condition;	//约束条件
public:
	//识别模型中横担塔头时的实际匹配信息
	CHashListEx<MATCHNODE> hashMatchNodes;
	CHashListEx<MATCHROD> hashMatchRods;
public:
	STDTEMPL();
	WORD get_ProfileNodeCount();
	_declspec(property(get=get_ProfileNodeCount)) WORD ProfileNodeCount;
	void Empty();
	void ClearMatchInfo(){hashMatchNodes.Empty();hashMatchRods.Empty();}
	void CopyStdTemplFrom(STDTEMPL* pStdTempl);
};
struct ITowerModel
{
	virtual IModelNode* FromNodeHandle(long hNode)=0;
	virtual IModelRod* FromRodHandle(long hRod)=0;
	virtual IModelNode* EnumNodeFirst()=0;
	virtual IModelNode* EnumNodeNext()=0;
	virtual IModelRod* EnumRodFirst()=0;
	virtual IModelRod* EnumRodNext()=0;
	virtual UINT GetNodeCount()=0;
	virtual UINT GetRodCount()=0;
};
#endif