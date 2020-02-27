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
#define SPHERE_TYPE CString("sphere")				//����
#define HALFSPHERE_TYPE CString("hemisphere")		//������
#define POLYGONBOX_TYPE CString("polygonbox")		//��������
#define BOX_TYPE CString("box")						//������
#define COLUMN_TYPE CString("column")				//Բ����
#define FRUSTUM_TYPE CString("frustum")				//Բ̨��
#define TORUS_TYPE CString("torus")					//Բ��
#define TORUSELLIPSE_TYPE CString("torusellipse")	//��Բ��
#define KNEEBEND_TYPE CString("kneebend")			//���
#define PADDING_TYPE CString("padding")				//��Ƭ
#define TAI_TYPE CString("tai")						//̨�͵�Ƭ
#define SQUAREPADDING_TYPE CString("squarepadding")	//���ε�Ƭ
#define PYRAMID_TYPE CString("pyramid")				//��̨
#define TRIANGLE_TYPE CString("triangle")			//������
#define TRAPEZIUM_TYPE CString("trapezium")			//������
#define LINES_TYPE CString("linkline")				//����

//////////////////////////////////////////////////////////////////////////
//ҵ������
#define YW_PROJECT_INFO	CString("switchset_layouts")	//ҵ������  �������˽ṹ �ڵ�
#define YW_DEVICE_ATTR	CString("property_array")		//ҵ������  �豸���� �ڵ�
#define YW_BUILD_ATTR	CString("build_map_array")		//ҵ������  �豸��װͼ �ڵ�
#define YW_PLANE_ATTR	CString("dev_plane_map_array")	//ҵ������  �豸ƽ��ͼ �ڵ�
#define YW_SPACE_ATTR	CString("space_dm_map_array")	//ҵ������  �������ͼ �ڵ�
#define YW_OBJECT_TYPE	CString("object_type")			//�豸����
#ifdef	AFX_TARG_ENU_ENGLISH
#define YW_NORMAL_DEVICE CString("normalDevice")				//��ͨ�豸
#define YW_GROUP_DEVICE	CString("groupDevice")				//����豸
#else
#define YW_NORMAL_DEVICE CString("��ͨ�豸")				//��ͨ�豸
#define YW_GROUP_DEVICE	CString("����豸")				//����豸
#endif
#define YW_HANDLE		CString("handle")				//���
#define YW_NAME			CString("name")					//����
#define YW_VALUE		CString("value")				//ֵ
#define YW_GROUP		CString("group")				//��
#define YW_TYPE			CString("type")					//����
#define YW_LENGTH		CString("length")				//����
#define YW_DEV_TAG		CString("dev_tag")				//�豸��ǩ
#define YW_ID_PROPERTY	CString("property_id")			//����id
#define YW_ID_BUILD		CString("build_map_id")			//�豸��װͼid
#define YW_ID_SPACE		CString("space_dm_map_id")		//�������ͼid
#define YW_ID_PLANE		CString("plane_map_id")			//�豸ƽ��ͼid
#define YW_MAP_ID		CString("map_id")				//ͼid
#define YW_MAP_PATH		CString("map_path")				//ͼ·��

//��·
#define PLINE_ROUTE		CString("PLLineRoute")			//��··���ڵ�
#ifdef	AFX_TARG_ENU_ENGLISH
#define PLINE_LOOP		CString("wireLoop")				//��·��·�ڵ�
#define PLINE_TOWER		CString("rodTower")					//����
#define PLINE_TOWERBASE	CString("rodTowerBase")				//..
#define PLINE_FS1		CString("wireFittingsBunch")			//..
#define PLINE_FS2		CString("earthWireFittingsBunch")			//..
#define PLINE_POWERLINE	CString("powerLine")				//..
#else
#define PLINE_LOOP		CString("���߻�·")				//��·��·�ڵ�
#define PLINE_TOWER		CString("����")					//����
#define PLINE_TOWERBASE	CString("��������")				//..
#define PLINE_FS1		CString("���߽�ߴ�")			//..
#define PLINE_FS2		CString("���߽�ߴ�")			//..
#define PLINE_POWERLINE	CString("������")				//..
#endif
#define PLINE_POSITION	CString("position")				//..
#define PLINE_MODEL		CString("model")				//..
#define PLINE_DESC		CString("desc")					//..
#ifdef	AFX_TARG_ENU_ENGLISH
#define PLINE_LINEPS	CString("hangingPointPosition")				//..
#else
#define PLINE_LINEPS	CString("�ҵ�λ��")				//..
#endif
#define PLINE_OPPOWERLINE CString("PLOpPowerLine")		//..
#define PLINE_STARTPT	CString("startPoint")			//..
#define PLINE_ENDTPT	CString("endPoint")				//..
#define PLINE_PHASE		CString("phase")				//����
#define PLINE_K			CString("k")					//kֵ
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
#define PLINE_STARTX	CString("ǰ��X")
#define PLINE_STARTY	CString("ǰ��Y")
#define PLINE_STARTZ	CString("ǰ��Z")
#define PLINE_ENDX		CString("���X")
#define PLINE_ENDY		CString("���Y")
#define PLINE_ENDZ		CString("���Z")
#define PLINE_JJCH		CString("��ߴ�")
#define PLINE_JJ		CString("���")					
#define PLINE_JYZ		CString("��Ե��")
#endif
#define PLINE_PROPERTS	CString("properties")			//���Ա�
#define PLINE_PROIN		CString("propertieinfo")		//���Ա��ֶ�

class CXmlObject
{
public:
	virtual bool IsEntity(){return false;}
	virtual bool IsFittings(){return false;}
	virtual bool IsPropObj(){return false;}
	virtual bool IsBlockRec(){return false;}
};
//ʵ��
class CXmlEntity : public CXmlObject
{
public:
	const static BYTE TYPE_SOLID		= 1;	//ʵ��
	const static BYTE TYPE_LINES		= 2;	//��
	const static BYTE TYPE_BLOCKREC_REF = 3;	//���¼����
	const static BYTE TYPE_OTHER		= 100;	//����
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

//Solidʵ��
class CXmlSolid : public CXmlEntity
{
public:
	CXmlSolid();
	double volume;
	fPtList m_arPts;						//�洢����
	fPtList m_arPtNormal;	
	ATOM_LIST< ATOM_LIST<int> > m_arIndexs;	//�洢ÿ���������ֵ
};

//����
class CXmlLines : public CXmlEntity
{
public:
	CXmlLines();
	fPtList m_arPts;
};

//������ʵ��
class CXmlBlockRecRef : public CXmlEntity
{
public:
	CXmlBlockRecRef();
	virtual bool IsBlockRecRef() const { return true; }
	long m_hBlockRec;			//�����������õĿ��¼��Handle
	double m_arrMatrixd[4][4];	//����
};

//////////////////////////////////////////////////////////////////////////
//����¼
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
// ������Ե��
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
//��������
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
//��� ����¼��Handle��Ϊ���Key
class CXmlModel
{
public:
	CXhChar100 m_sName;
	CHashList<CXmlBlockRecord> XmlBlockTable;
	CHashStrList<CXmlObjPropertys> PropertyeTable;			//��ߡ���Ե������
	CHashList<CXmlFittings> XmlFittings;					//��ߡ���Ե��
	bool ImportDataFromXML(CMarkup xml,CString sElemName,CXmlObject *pObj=NULL);
	bool LoadFromXmlFile(const char* sFilePath);
};