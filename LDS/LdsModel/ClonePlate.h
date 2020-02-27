#pragma once
#include "MemberProperty.h"
#include "lds_part.h"
#include "Markup.h"
#include "AngleLayout.h"


struct LINKPOLEINFO{
protected:
	UINT id;
public:
	CLDSLinePart *pLinePart;//记录连接杆件指针
	CLDSLinePart *pMatchLinePart;//记录匹配杆件指针
	long hLinePart;			//连接杆件句柄
	long hMatchPole;		//记录与当前杆件匹配的射线杆件句柄
	double offset_angle;	//记录射线杆件与基准杆件之间的夹角
	int vec_type;			//记录射线杆件延伸方向 1表示与基准角钢当前肢延伸方向相同 -1表示与基准角钢当前肢延伸方向相反
	void SetKey(DWORD key){id=key;}
	ADD_READONLY_PROPERTY(UINT,Id){return id;}
	LINKPOLEINFO()
	{
		pLinePart=NULL;
		pMatchLinePart=NULL;
		hLinePart=0;
		offset_angle=0;
		hMatchPole=0;
		vec_type=0;
	}
};
class ANGLE_MAPPING{
	ANGLE_LAYOUT xSrcLayout;
	bool Match(CLDSLineAngle* pDestAngle,char _ciDestRayOrgTipS0E1,GECS& acsMapTo);
public:
	char ciSrcRayOrgTipS0E1;
	char ciLegX2DestLeg;	//源角钢的X肢在目标角钢中的匹配映射肢，'X/x','Y/y'大写字母为外铁，小写字母为里铁
	char ciLegY2DestLeg;	//源角钢的Y肢在目标角钢中的匹配映射肢，'X/x','Y/y'大写字母为外铁，小写字母为里铁
	//bool blReverseTipSnE;
	//char ciDestRayOrgTipS0E1
	ANGLE_MAPPING(){
		//blReverseTipSnE=false;
		ciLegX2DestLeg='X';
		ciLegY2DestLeg='Y';
		ciSrcRayOrgTipS0E1=0;
	}
	void InitSrcAngle(CLDSLineAngle* pSrcAngle,char _ciSrcRayOrgTipS0E1,GECS& lcsSrc);
	bool Mapping(CLDSLineAngle* pDestAngle,char _ciDestRayOrgTipS0E1,GECS& lcsMapTo);
};
class CSchemaRod : public LINKPOLEINFO
{
protected:
	bool m_bInitSchemaInfo;
	bool m_bUpdateMatchRodInfo;
public:
	bool get_blInitSchemaInfo(){return m_bInitSchemaInfo;}
	__declspec(property(get=get_blInitSchemaInfo)) bool blInitSchemaInfo;
	bool get_blUpdateMatchRodInfo(){return m_bUpdateMatchRodInfo;}
	__declspec(property(get=get_blUpdateMatchRodInfo)) bool blUpdateMatchRodInfo;

	long m_idClassType;
	struct DESIGNLJ_ANGLEPARA
	{
		BYTE ciSonAngleQuad;	//基准角钢若为组合角钢且当前角钢为基准组合角钢的子角钢，所在X-Z平面内的象限号,取值:1-4，0表示其余角钢
		BYTE ciWorkWingX0Y1;	//当前连接肢,未来抽空应把此属性完全用xWingLayout取代掉 wjh-2019.3.29
		ANGLE_LAYOUT xWingLayout;//工作肢布置方式
		ANGLE_SPACE_FLAG cbSpaceFlag;		//
		BOOL bEndLjAngle;		//端连接角钢
		BOOL bDualEdge;			//是否螺栓线两侧都有外轮廓点
	}angle;
	struct DESIGN_ODD
	{
		long hOddRefRod1,hOddRefRod2;	//正负头校验参照构件
		double fCollideDist;
		BYTE ciOddCalStyle;	//0.表示根据碰撞参照杆件计算;1.根据螺栓位置计算;2.表示不计算正负头
	}odd;
	BYTE ciConnectTipS0E1;	//模板中杆件原始设计连接端：始端0，终端1;
	short siPlaneNo;		//当前杆件所在的连接面,0:基准平面;1.第一火曲面;2.第二火曲面
	short siEndSpace,siWingSpace,siBerSpace;	//定义板外形时的参数
	//struct MAPPING{	// MAPPING
		//当前映射角钢是否需要颠倒X/Y肢 wjh-2019.3.29
		bool blReverseTipSnE;
		//当前映射角钢与模板中连接角钢间的肢映射关系
		char ciLegX2DestLeg;	//源角钢的X肢在目标角钢中的匹配映射肢，'X/x','Y/y'大写字母为外铁，小写字母为里铁
		char ciLegY2DestLeg;	//源角钢的Y肢在目标角钢中的匹配映射肢，'X/x','Y/y'大写字母为外铁，小写字母为里铁
		//当前映射角钢是否需要颠倒X/Y肢 wjh-2019.3.29
		bool get_blReverseLegXnY(){return ciLegX2DestLeg=='Y'||ciLegX2DestLeg=='y';}
		__declspec(property(get=get_blReverseLegXnY)) bool blReverseLegXnY;
	//}mapto;
public:
	CSchemaRod();
	bool ClearMatchRodInfo(){m_bUpdateMatchRodInfo=false;}
	void CopyProperty(CSchemaRod* pScrSchemaRod);
	void InitSchemaAngle(CLDSLineAngle* pSrcAngle,GECS& scs);
	bool Match(CLDSLineAngle* pDestAngle,char ciDestConnTipS0E1,GECS& lcs);
};

struct ANCHOR_LOCATION{
	BYTE ciAnchorType;	//1.角钢两端楞点为基准;2.杆件上节点为基准;3.角钢心线交点;
	BOOL bIncOddEffect;	//当ciAnchorType=1时，表示纵向偏移量是否考虑端头正负头
	BYTE ciIntersPlane;	//平推面，0表示不平推;1:X-Y平推;2:X-Z;3:Y-Z;TODO:未来可与ciAnchorType整合到一个字节里
	COORD3D vPlaneNormal;	//平推面法向
	UINT idPriorAngle,idSecondaryAngle;
	union INTERS_ANGLE_G{
		DWORD dwParamG;
		struct{short g,ciStyleG;};	//ciStyleG=0:g;1:g1;2:g2;3:g3;4:指定值
	}g_arr[2];
	BYTE ciLenDirection;	//0.基准角钢方向:始->终;1.终->始（仅对基准构件是角钢时有效）
	short siLenOffset;	//沿anchor.idResideAngle角钢的纵向偏移量,横向偏移量存在G_ARR
};
class CSchemaBolt
{
	UINT id;
	WORD m_wStateFlag;	//用以控制螺栓设计中若干参数的随动更新，如直径是否需要根据肢宽选定等
public:
	long d;	     			// 螺栓直径.
	char grade[5];			// 螺栓级别
	bool m_bOnlyHole;		//仅孔无螺栓
	DWORD dwRayNo;			// 所在的射线索引号，出图时用
	struct WORKNORMAL{
		BYTE ciNormStyle;	//目前对于角钢只能为1，表示角钢肢法线方向;
		BYTE ciDirection;	//0.朝外;1.朝内
		BYTE ciWorkWing;	//螺栓在idResideAngle指向角钢的的寄宿肢x0,y1,如果idResideAngle=0，表示以钢板为基准
		UINT idResideAngle;	//螺栓所在的角钢，既螺栓法线的的计算依据角钢
	}normal;	//螺栓工作法线设计数据
	struct THIN_THICKITEM{
		char ciItemType;	//0.FinalKey;1.穿孔构件Id值(0xff表示负板厚,0x7f表示正板厚);2.指定厚度值;
		BYTE ciThickVal;	//根据ciItemType指定的厚度值
		THIN_THICKITEM(){ciItemType=0;ciThickVal=0;}
	};
	struct PARATHICK{	//参照CParaThick
		THIN_THICKITEM items[6];
		char* ToThickStr();
	};
	struct DESIGN_BASE_POS
	{
		UINT idResideAngle;	//螺栓所在的角钢，既螺栓纵向及横向偏移量的计算依据角钢(可能与法线不一样)
		ANCHOR_LOCATION anchor;	//螺栓基准点初始坐标(不考虑纵向移动)设计数据
		static const BYTE GLOBAL_COORD = 0;
		static const BYTE LOCAL_COORD  = 1;
		BYTE cLocationStyle;	//0:GLOBAL_COORD基点坐标为绝对坐标;1:LOCAL_COORD基点坐标为基准构件的相对坐标
		BYTE ciDirection;			//0.基准角钢方向:始->终;1.终->始（仅对基准构件是角钢时有效）
		BYTE ciWingOffsetStyle;	//0.g;1.g1;2.g2;3.g3;4.自定义
		short wing_offset_dist,len_offset_dist;//肢向偏移值（Y）;纵向偏移值（X）;
		union{
			BYTE ciWorkWing;	//0.X肢;1.Y肢（仅对基准构件是角钢时有效）
			BYTE ciResidePlane;	//0.所在钢板面号(0表示基面;1.表示第一个火曲面;2.表示第二个火曲面) wjh-2015.2.7
		};
		PARATHICK norm_offset;	//法向偏移量字符串
	}des_base_pos;
	PARATHICK through;	// 螺栓通过厚度.对应L0
public:
	CSchemaBolt();
	void InitThrough(const char* thickStr);
	void SetKey(DWORD key){id=key;}
	ADD_READONLY_PROPERTY(UINT,Id){return id;}
};

//克隆钢板
class CClonePlate{
protected:
	//0.不匹配 
	//1.完全匹配(编号相同)  角度匹配 射线角钢规格匹配 主材规格匹配 射线角钢数量相同
	//2.射线杆件数量匹配规格匹配但角度不匹配  ==>取钢板基本信息,射线角钢螺栓信息基准角钢螺栓信息,自动计算正负头
	//3.仅射线杆件数量匹配
	int m_iMatchResult;	//匹配结果
	IModel *m_pTower;
	CLDSPlate *m_pSrcPlate;	//记录指向源钢板的指针
	CLDSNode *m_pDestNode;	//需要粘贴钢板的节点
	LINEPARTSET partset;	//节点当前面相关杆件集合
	CHashListEx<CSchemaRod> hashLinkRods;
	CHashSet<CSchemaRod*> hashLinkRodsByH;
	void EmptyLinkRods();
public:
	char m_cMateria;
	int m_nThick;
	BYTE m_nPlaneN;			//钢板工作面数>1表示火曲板
	UINT m_idDatumRod;		//基准杆件的标识Id
	BYTE m_ciProfileSty;	//外轮廓定义方式
	BOOL m_bFlexibleDesign;	//请用柔性设计
	char ciDatumAxis;		//一般为'Y'
	short ciDirectionAxisY;
	short ciDirectionAxisZ;	//0表示当前工作肢正法线;1：表示当前工作肢负法线
	ANCHOR_LOCATION origin,huoqu_pt[2];
	struct HUOQU_NORM{
		UINT m_idViceRod;
		UINT m_idCrossRod;
	}huoqu_norm[2];
	struct VECTORS{
		GEPOINT vDatumLenVec;	//基准杆件延伸方向（一般也为Y轴方向），
		GEPOINT vDatumPlaneNorm;//钢板基准平面的法线方向（一般也为基准角钢的工作肢法线方向）
		GEPOINT vBendPlaneNorm[2];
	}xLocalVec;	//xLocalVec为源模板局部坐标系下的矢量,xModelVec当前模型坐标系下的矢量
	VECTORS UpdateModelCSVectors(GECS acs);	//钢板局部装配坐标系
	int JustifySonAngleResideQuad(GECS acs,CLDSLineAngle* pAngle);
public:
	CClonePlate();
	~CClonePlate();
	CClonePlate(IModel *pTower,CLDSPlate *pPlate=NULL,CLDSNode *pNode=NULL);
	void SetSuperiorTower(IModel *pTower);
	CSchemaRod* AppendSchemaRod(long hLinkRod,DWORD keyid=0);
	bool RemoveSchemaRod(CSchemaRod* pSchemaRod);
	void AnalyzePlateInfo(CLDSPlate *pPlate,IModel *pTower=NULL);	//分析钢板信息
	int IntelligentMatchPlate(CLDSNode *pNode,PARTSET& DispPartSet,IModel *pTower=NULL);//智能识别当前节点上是否可生成相同编号的钢板
	CLDSPlate *CreatePlate(PARTSET& DispPartSet,IModel *pTower=NULL);	//根据匹配结果生成新钢板
	void ToClipperBoard(CBuffer* pClipperBoard,IModel *pTower=NULL);		//将钢板信息存储到剪贴板中
	void FromClipperBoard(CBuffer* pClipperBoard,IModel *pTower=NULL);		//从剪切板中读取钢板信息
	CLDSLinePart *FindMatchPoleBySrcHandle(long hLinePart);	//通过源杆件句柄查找与之相匹配的杆件
	CLDSLinePart *FindSrcPoleByMatchHandle(long hMatchPole);//通过匹配杆件句柄查找与之相匹配的源杆件
	CSchemaRod* QuerySchemaRod(long hRod,char ciRodTypeSrc0Map1=0);
	bool UpdateTipPosition(long hSchemaRelaRod,CLDSPlate* pNewPlate=NULL);
	//排序节点杆件集合 0.按射线杆件与基准杆件fabs(cosa)排序  1.按逆时针方向排序(需指定射线杆件所在钢板)
	void SortLinePartSet(CLDSLinePart *pDatumLinePart=NULL,int iSortType=0,CLDSPlate *pPlate=NULL);
	void CorrectPlateDesignPara(CLDSPlate *pPlate);	//根据射线杆件规格调整钢板射线杆件参数
	void CorrectPlateLsInfo(CLDSPlate *pPlate);		//根据杆件螺栓信息更新螺栓规格以及螺栓间距
	void UpdateRayLinePartOffset(CLDSPlate *pPlate,int nAutoAdjustTubePos=0);//更新钢板射线杆件信息
	void UpdateRayLinePartOddment(CLDSPlate *pPlate);//更新射线杆件正负头
};
class CDeepClonePlate : public CClonePlate
{
	struct XmlNode{
		char* sElemName;
		void* pRelaObj;
		XmlNode(char* elemName,void* pObj=NULL){
			sElemName=elemName;
			pRelaObj=pObj;
		}
	};
protected:
	CHashListEx<CSchemaBolt> hashTemplBolts;
	CHashSet<CSchemaBolt*> hashBoltById;
protected:
	int GetSchemaRodId(long hRod,BOOL bMatchRod=FALSE);
#ifdef __LDS_CONTEXT_
	bool ImportTemplChildXmlNode(CMarkup xml,const XmlNode& xmlnode);
#endif
	bool UpdateBoltBySchema(CSchemaBolt* pSchemaBolt,CLDSBolt* pBolt);
	char* GetThroughStr(CSchemaBolt* pSchemaBolt);
	void UpdatePlateDesUcs(CLDSPlate* pNewPlate);
public:
	CDeepClonePlate(IModel *pTower);
	~CDeepClonePlate();
	GECS CalACS(CSchemaRod* pDatumSchemaRod);
	//
	void ExtractPlateInfo(CLDSPlate* pScrPlate);
	CLDSPlate *CreatePlate(PARTSET& dispPartSet,IModel *pTower=NULL);
#ifdef __LDS_CONTEXT_
	//模板数据操作
	BOOL FromXmlSchemaFile(const char* filename);
	BOOL ToXmlSchemaFile(const char* filename);
	BOOL WriteToClipboard();
	BOOL ReadFromClipboard();
#endif
};