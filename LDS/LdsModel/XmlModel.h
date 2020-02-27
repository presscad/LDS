//<LOCALE_TRANSLATE BY hxr /> 2015-5-12
//////////////////////////////////////////////////////////////////////////
//
#pragma once
#include "XhCharString.h"
#include "Markup.h"
#include "f_ent_list.h"
#include "HashTable.h"

#define MODEL_SPACE CString("*Model_Space")

#define ROOT_NODE CString("ist_drawing_info")
#define BLOCKTABLE_NODE CString("block_table")
#define OFFSET_NODE CString("offset")
#define VALUE_NODE CString("value")
#define LON_NODE CString("lon")
#define LAT_NODE CString("lat")
#define BLOCKRECORD_NODE CString("blockRecNode")
#define ENTITY_NODE CString("e")
#define POINTLIST_NODE CString("pl")
#define POINT_NODE CString("p")
#define FACELIST_NODE CString("ml")
#define FACE_NODE CString("m")
#define NORMLIST_NODE CString("nl")
#define NORM_NODE CString("n")

#define ENTTYPE_ATTRIBUTE CString("n")
#define HANDLE_ATTRIBUTE CString("h")
#define LAYER_ATTRIBUTE CString("l")
#define COLOR_ATTRIBUTE CString("c")
#define TYPEGUID_ATTRIBUTE CString("tg")
#define RECGUID_ATTRIBUTE CString("rg")
#define SP_ATTRIBUTE CString("sp")
#define BLKHANDLE_ATTRIBUTE CString("b")
#define MATRIX_ATTRIBUTE CString("m")
#define X_ATTRIBUTE CString("x")
#define Y_ATTRIBUTE CString("y")
#define Z_ATTRIBUTE CString("z")
#define NORX_ATTRIBUTE CString("norx")
#define NORY_ATTRIBUTE CString("nory")
#define NORZ_ATTRIBUTE CString("norz")
#define FACEINDEX_ATTRIBUTE CString("fl")
#define RADIUS_ATTRIBUTE CString("radius")
#define XRADIUS_ATTRIBUTE CString("xradius")
#define YRADIUS_ATTRIBUTE CString("yradius")
#define TRADIUS_ATTRIBUTE CString("tradius")
#define MAJORRADIUS_ATTRIBUTE CString("majorradius")
#define MINORRADIUS_ATTRIBUTE CString("minorradius")
#define OUTRADIUS_ATTRIBUTE CString("outradius")
#define INRADIUS_ATTRIBUTE CString("inradius")
#define TOPRADIUS_ATTRIBUTE CString("topradius")
#define OUTRADIUSBOTTOM_ATTRIBUTE CString("outradiusbottom")
#define KBRADIUS_ATTRIBUTE CString("kbradius")
#define MAJORAXIS_ATTRIBUTE CString("majoraxis")
#define MINORAXIS_ATTRIBUTE CString("minoraxis")
#define HEIGHT_ATTRIBUTE CString("height")
#define EDGE_ATTRIBUTE CString("edge")
#define ANGLE_ATTRIBUTE CString("angle")
#define OUTLENGTH_ATTRIBUTE CString("outlength")
#define INLENGTH_ATTRIBUTE CString("inlength")
#define SIDES_ATTRIBUTE CString("sides")
#define L1_ATTRIBUTE CString("l1")
#define L2_ATTRIBUTE CString("l2")
#define L3_ATTRIBUTE CString("l3")
#define BOTTOMLENGHT_ATTRIBUTE CString("bottomlenght")
#define TOPLENGHT_ATTRIBUTE CString("toplenght")
#define THICK_ATTRIBUTE CString("thick")
#define LEFTOFFSET_ATTRIBUTE CString("leftoffset")

#define REFERENCE_TYPE CString("r")
#define SOLID_TYPE CString("m")
#define SPHERE_TYPE CString("sphere")				//球体
#define HALFSPHERE_TYPE CString("hemisphere")		//半球体
#define POLYGONBOX_TYPE CString("polygonbox")		//正多面体
#define BOX_TYPE CString("box")						//长方体
#define COLUMN_TYPE CString("column")				//圆柱体
#define FRUSTUM_TYPE CString("frustum")				//圆台体
#define TORUS_TYPE CString("torus")					//圆环
#define TORUSELLIPSE_TYPE CString("torusellipse")	//椭圆环
#define KNEEBEND_TYPE CString("kneebend")			//弯管
#define PADDING_TYPE CString("padding")				//垫片
#define TAI_TYPE CString("tai")						//台型垫片
#define SQUAREPADDING_TYPE CString("squarepadding")	//方形垫片
#define PYRAMID_TYPE CString("pyramid")				//棱台
#define TRIANGLE_TYPE CString("triangle")			//三角柱
#define TRAPEZIUM_TYPE CString("trapezium")			//梯形体
#define LINES_TYPE CString("linkline")				//线体

//////////////////////////////////////////////////////////////////////////
//业务数据
#define YW_PROJECT_INFO	CString("switchset_layouts")	//业务数据  工程拓扑结构 节点
#define YW_DEVICE_ATTR	CString("property_array")		//业务数据  设备属性 节点
#define YW_BUILD_ATTR	CString("build_map_array")		//业务数据  设备安装图 节点
#define YW_PLANE_ATTR	CString("dev_plane_map_array")	//业务数据  设备平面图 节点
#define YW_SPACE_ATTR	CString("space_dm_map_array")	//业务数据  间隔断面图 节点
#define YW_OBJECT_TYPE	CString("object_type")			//设备类型
#ifdef	AFX_TARG_ENU_ENGLISH
#define YW_NORMAL_DEVICE CString("normalDevice")				//普通设备
#define YW_GROUP_DEVICE	CString("groupDevice")				//组合设备
#else
#define YW_NORMAL_DEVICE CString("普通设备")				//普通设备
#define YW_GROUP_DEVICE	CString("组合设备")				//组合设备
#endif
#define YW_HANDLE		CString("handle")				//句柄
#define YW_NAME			CString("name")					//名称
#define YW_VALUE		CString("value")				//值
#define YW_GROUP		CString("group")				//组
#define YW_TYPE			CString("type")					//类型
#define YW_LENGTH		CString("length")				//长度
#define YW_DEV_TAG		CString("dev_tag")				//设备标签
#define YW_ID_PROPERTY	CString("property_id")			//属性id
#define YW_ID_BUILD		CString("build_map_id")			//设备安装图id
#define YW_ID_SPACE		CString("space_dm_map_id")		//间隔断面图id
#define YW_ID_PLANE		CString("plane_map_id")			//设备平面图id
#define YW_MAP_ID		CString("map_id")				//图id
#define YW_MAP_PATH		CString("map_path")				//图路径

//线路
#define PLINE_ROUTE		CString("PLLineRoute")			//线路路径节点
#ifdef	AFX_TARG_ENU_ENGLISH
#define PLINE_LOOP		CString("wireLoop")				//线路回路节点
#define PLINE_TOWER		CString("rodTower")					//杆塔
#define PLINE_TOWERBASE	CString("rodTowerBase")				//..
#define PLINE_FS1		CString("wireFittingsBunch")			//..
#define PLINE_FS2		CString("earthWireFittingsBunch")			//..
#define PLINE_POWERLINE	CString("powerLine")				//..
#else
#define PLINE_LOOP		CString("导线回路")				//线路回路节点
#define PLINE_TOWER		CString("杆塔")					//杆塔
#define PLINE_TOWERBASE	CString("杆塔基础")				//..
#define PLINE_FS1		CString("导线金具串")			//..
#define PLINE_FS2		CString("地线金具串")			//..
#define PLINE_POWERLINE	CString("电力线")				//..
#endif
#define PLINE_POSITION	CString("position")				//..
#define PLINE_MODEL		CString("model")				//..
#define PLINE_DESC		CString("desc")					//..
#ifdef	AFX_TARG_ENU_ENGLISH
#define PLINE_LINEPS	CString("hangingPointPosition")				//..
#else
#define PLINE_LINEPS	CString("挂点位置")				//..
#endif
#define PLINE_OPPOWERLINE CString("PLOpPowerLine")		//..
#define PLINE_STARTPT	CString("startPoint")			//..
#define PLINE_ENDTPT	CString("endPoint")				//..
#define PLINE_PHASE		CString("phase")				//相序
#define PLINE_K			CString("k")					//k值
#ifdef	AFX_TARG_ENU_ENGLISH
#define PLINE_STARTX	CString("startPointX")
#define PLINE_STARTY	CString("startPointY")
#define PLINE_STARTZ	CString("startPointZ")
#define PLINE_ENDX		CString("endPointX")
#define PLINE_ENDY		CString("endPointY")
#define PLINE_ENDZ		CString("endPointZ")
#define PLINE_JJCH		CString("fittingsBunch")
#define PLINE_JJ		CString("fittings")					
#define PLINE_JYZ		CString("insulator")
#else
#define PLINE_STARTX	CString("前点X")
#define PLINE_STARTY	CString("前点Y")
#define PLINE_STARTZ	CString("前点Z")
#define PLINE_ENDX		CString("后点X")
#define PLINE_ENDY		CString("后点Y")
#define PLINE_ENDZ		CString("后点Z")
#define PLINE_JJCH		CString("金具串")
#define PLINE_JJ		CString("金具")					
#define PLINE_JYZ		CString("绝缘子")
#endif
#define PLINE_PROPERTS	CString("properties")			//属性表
#define PLINE_PROIN		CString("propertieinfo")		//属性表字段

class CXmlObject
{
public:
	virtual bool IsEntity(){return false;}
	virtual bool IsFittings(){return false;}
	virtual bool IsPropObj(){return false;}
	virtual bool IsBlockRec(){return false;}
};
//实体
class CXmlEntity : public CXmlObject
{
public:
	const static BYTE TYPE_SOLID		= 1;	//实体
	const static BYTE TYPE_LINES		= 2;	//线
	const static BYTE TYPE_BLOCKREC_REF = 3;	//块记录引用
	const static BYTE TYPE_OTHER		= 100;	//其它
public:
	CXmlEntity(){m_hHandle=0; m_cTypeId=TYPE_OTHER;}
	//
	BYTE GetTypeID() { return m_cTypeId; }
	long SetKey(long key) { return m_hHandle=key; }
	long GetHandle() const { return m_hHandle; }
	CString GetTypeName() const { return m_sTypeName; }
	CString GetLayer() const { return m_sLayer; }
	CString GetTypeGuid() const { return m_sTypeGuid; }
	CString GetRecGuid() const { return m_sRecGuid; }
	CString GetSP() const { return m_sSP; }
	virtual bool IsEntity(){return true;}
	virtual bool IsBlockRecRef() const { return false; }
public:
	long m_hHandle;
	int m_nRGBColor;
	BYTE m_cTypeId;
	CString m_sTypeName;
	CString m_sLayer;
	CString m_sTypeGuid;
	CString m_sRecGuid;
	CString m_sSP;
};

//Solid实体
class CXmlSolid : public CXmlEntity
{
public:
	CXmlSolid();
	double volume;
	fPtList m_arPts;						//存储顶点
	fPtList m_arPtNormal;	
	ATOM_LIST< ATOM_LIST<int> > m_arIndexs;	//存储每个面的索引值
};

//线体
class CXmlLines : public CXmlEntity
{
public:
	CXmlLines();
	fPtList m_arPts;
};

//块引用实体
class CXmlBlockRecRef : public CXmlEntity
{
public:
	CXmlBlockRecRef();
	virtual bool IsBlockRecRef() const { return true; }
	long m_hBlockRec;			//块引用所引用的块记录的Handle
	double m_arrMatrixd[4][4];	//矩阵
};

//////////////////////////////////////////////////////////////////////////
//块表记录
typedef CSuperHashList<CXmlEntity> XmlEntitySet;
class CXmlBlockRecord : public CXmlObject
{
public:
	CXmlBlockRecord();

	bool IsModelSpace() const { return m_bIsMSpace; }
	bool SetModelSpace(bool isMSpace){ return (m_bIsMSpace=isMSpace); }
	CXhChar50	 GetHandle() const { return m_sHanle; }
	virtual bool IsBlockRec(){return true;}
	XmlEntitySet m_arEntity;
protected:
	bool m_bIsMSpace;
	CXhChar50 m_sHanle;
};

//////////////////////////////////////////////////////////////////////////
// 金具与绝缘子
class CXmlFittings : public CXmlObject
{
public:
	CXmlFittings(){m_cType=0;}
	virtual bool IsFittings(){return true;}

	const static int TYPE_JYZ = 1;
	const static int TYPE_JJ  = 2;
	BYTE m_cType;
	long handle;
	CXhChar100 m_sName;
	CXhChar100 m_sModel;
	GEPOINT pos;
};
//对象属性
typedef struct tagXML_PROPERTY{
	CXhChar100 sName;
	CXhChar16 sType;
	int length;
	CXhChar100 sValue;
	CXhChar16 sUnit;
	tagXML_PROPERTY(){length=0;}
}XML_PROPERTY;
class CXmlObjPropertys : public CXmlObject
{
public:
	CXmlObjPropertys(){}
	virtual bool IsPropObj(){return true;}

	CXhChar100 m_sModel;
	CHashStrList<XML_PROPERTY> propTable;
};
//块表 块表记录的Handle作为块表Key
class CXmlModel
{
public:
	CXhChar100 m_sName;
	CHashList<CXmlBlockRecord> XmlBlockTable;
	CHashStrList<CXmlObjPropertys> PropertyeTable;			//金具、绝缘子属性
	CHashList<CXmlFittings> XmlFittings;					//金具、绝缘子
	bool ImportDataFromXML(CMarkup xml,CString sElemName,CXmlObject *pObj=NULL);
	bool LoadFromXmlFile(const char* sFilePath);
};