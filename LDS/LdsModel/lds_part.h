//<LOCALE_TRANSLATE BY hxr /> 2014-12-8
#ifndef __LDS_PART_H_
#define __LDS_PART_H_

#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "PartLib.h"
#include "XhLdsLm.h"
#include "HashTable.h"
#include "Hash64Table.h"
#include "LogFile.h"
#include "SmartPtr.h"
#include "Buffer.h"
#include "macro.h"
#include "ldsptr_list.h"
#include "IModel.h"
#include "IPartLibrary.h"
#include "ComplexId.h"
#include "LifeObj.h"
#ifdef __PART_DESIGN_INC_
#include "WirePlaceCode.h"
#include "Resolver.h"
#endif
#include "XhCharString.h"
#include "SolidBody.h"
#include "MemberProperty.h"
#include "SegI.h"
#include "PropListItem.h"
#ifdef __LDS_CONTEXT_
#include "..\BOM\BOM.h"
#endif
#include "SortFunc.h"
//#include "ConnectPointInfo.h"

/*
			��Ȩ����2001-2007 �����ź��������Ƽ����޹�˾

�ļ�����:
	LDS������������������Ϣ����
*/
#define SELECTINDEX_NODE		1
#define SELECTINDEX_LINE		2	//0x02
#define SELECTINDEX_CIRCLE		3	//0x04
#define SELECTINDEX_LINEANGLE	4	//0x08
#define SELECTINDEX_LINETUBE	5	//0x10
#define SELECTINDEX_LINEFLAT	6	//0x20
#define SELECTINDEX_LINESLOT	7	//0x40
#define SELECTINDEX_PLATE		8
#define SELECTINDEX_PARAMPLATE	9
#define SELECTINDEX_BOLT		10
#define SELECTINDEX_SPHERE		11
#define SELECTINDEX_ARCANGLE	12
#define SELECTINDEX_ARCSLOT		13
#define SELECTINDEX_ARCFLAT		14
#define SELECTINDEX_ARCTUBE		15
#define SELECTINDEX_STDPART		16
#define SELECTINDEX_GROUPLINEANGLE 17
#define SELECTINDEX_BLOCKREF	18
#define SELECTINDEX_HOLEWALL	19
#define SELECTINDEX_FITTINGS	20
#define SELECT_LINEPART			0x00000078
#define SELECT_ARCPART			0x00007800
#define	SELECT_PART				0x0001FFF8
#define SELECT_ALL				0xFFFFFFFF

#ifndef X_RESTRICT
	#define	X_RESTRICT		0x0001		//X����Լ��
#endif
#ifndef Y_RESTRICT
	#define	Y_RESTRICT		0x0002		//Y����Լ��
#endif
#ifndef	Z_RESTRICT
	#define	Z_RESTRICT		0x0004		//Z����Լ��
#endif
#ifndef X_ROT_RESTRICT
	#define	X_ROT_RESTRICT	0x0008		//X��ת��Լ��
#endif
#ifndef Y_ROT_RESTRICT
	#define	Y_ROT_RESTRICT	0x0010		//Y��ת��Լ��
#endif
#ifndef	Z_ROT_RESTRICT
	#define	Z_ROT_RESTRICT	0x0020		//Z��ת��Լ��
#endif
#ifndef ALL_RESTRICT
	#define	ALL_RESTRICT	0x003F		//������Լ��
#endif

#define MAX_CHAR_GUIGE_16		16
#define MAX_TEMP_CHAR_50		50
#define MAX_TEMP_CHAR_100		100
#define MAX_TEMP_CHAR_200		200
#define MAX_TEMP_CHAR_500		500

//�����ż������ wht 12-04-25
#define CHKPARTNO_NULLPARTNO			0x0001	//�ձ�Ź���
#define CHKPARTNO_PARTTYPE_DIFFER		0x0002	//�������Ͳ�ͬ
#define CHKPARTNO_RODINFO				0x0004	//ͬ��Ÿ˼����ȼ�����ͷ����񡢲��ʵȻ�����Ϣ
#define CHKPARTNO_RODBOLTINFO			0x0008	//ͬ��Ÿ˼�����˨��������񼰸˼�����˨λ����Ϣ�����ߵ��Աȣ�
#define CHKPARTNO_ANGLECUT				0x0010	//�Ǹ����н���Ϣ�����ߵ��Աȣ�
#define CHK_PLATE_BASICINFO				0x0020	//ͬ��Ÿְ��ȡ����ʡ����������Ȼ�����Ϣ
#define CHK_PLATE_PROFILEANDBOLTINFO	0x0040	//�ְ���˨�������������Կ�λ����߼����Σ��ߵ�������,
//У�˶ԳƸ˼�ʱ��������
#define CHK_MIRROD_BASICINFO			0x0080	//�ԳƸ˼���񡢲��ʡ���׼���ȵȻ�����Ϣ
#define CHK_MIRROD_LENGTHINFO			0x0100	//�ԳƸ˼���ʵ�ʳ�����Ϣ

//////////////////////////////////////////////////////////////////////////////

enum LDSPART_TYPE{
	TA_NODE				= 0,		// �ڵ�
	STEELTYPE_ANGLE		= 1,		// �Ǹ�
	STEELTYPE_BOLT		= 2,		// ��˨
	STEELTYPE_PLATE		= 3,		// �ְ�
	STEELTYPE_TUBE		= 4,		// �ֹ�
	STEELTYPE_FLAT		= 5,		// ����
	STEELTYPE_SLOT		= 6,		// �۸�
	TA_OTHER	= 100,
};
enum LDSCLASS_TYPEID{
	CLS_OBJECT = 1,
	CLS_DBOBJECT,
	CLS_NODE,
	CLS_PART,
	CLS_PLATE,
	CLS_PARAMPLATE,
	CLS_BOLT,
	CLS_LINEPART,
	CLS_GEOBJECT,
	CLS_GEPOINT,
	CLS_GELINE,
	CLS_GEPLANE,
	CLS_LINEANGLE,
	CLS_POLYJOINT,
	CLS_GROUPLINEANGLE,
	CLS_GROUPPOLYANGLE,
	CLS_LINETUBE,
	CLS_LINEFLAT,
	CLS_POLYFLAT,
	CLS_LINESLOT,
	CLS_POLYPART,
	CLS_FOOT,
	CLS_JOINT,
	CLS_TUBEJOINT,
	CLS_WELDLINE,
	CLS_DISPLAYVIEW,
	CLS_WELDPART,
	CLS_SPHERE,
	CLS_ARCPART,
	CLS_ARCANGLE,
	CLS_ARCSLOT,
	CLS_ARCFLAT,
	CLS_ARCTUBE,	//2009��10��10����ӣ��������ǰ�汾��LMA����׼�����ļ���ʽ���ܲ���
	CLS_STDPARTMODEL,
	CLS_CONETUBE,
	CLS_CONETUBEFL,
	CLS_CABLE,		//����
	CLS_FEMELEMENT,	//���޵�Ԫ
	CLS_ARCLIFT,	//Բ��Ԥ��
	CLS_SLOTARM,    //���ͺᵣ
	CLS_BLOCK,		//����
	CLS_BLOCKREF,	//��������
	CLS_PROFILEVERTEX,//���ζ���
	CLS_DELEGATEROD,
	CLS_EXTERNALPART,	//�ⲿ����
	CLS_FITTINGS,		//���
	CLS_INSULATOR,		//��Ե��
	CLS_BOLTU,			//U����˨
	CLS_WORKCASE,		//���ع������
	CLS_SUPPLEMENTPART,	//�ֶ�¼�븽�ӹ���
	CLS_LINEHSHAPE,
	CLS_ANCHORBOLT,		//�ؽ���˨
	CLS_FOUNDATION,		//����
	CLS_HOLEWALL,		//�����ϵĿױ�ʵ��
	CLS_WIREARROW,		//�ҵ㷽��ʾ���ͷ

	CLS_HEIGHTMODULE=100,
};
//�����������ݣ�AUX_AUTODESIGN:ȫ�Զ����;AUX_BY_START:����ʼ��֧�Ÿ˼�;AUX_BY_END:�����ն�֧�Ÿ˼�;
//AUX_ONLY_PRIMARY:�����Ƕ�б�ĵ�֧�������㡡wjh-2017.2.6
enum AUX_POLE_SEL_METHOD{AUX_AUTODESIGN=0,AUX_BY_START=1,AUX_BY_END=2,AUX_ONLY_PRIMARY=3};

#ifndef __CLS_CSOLIDPART_DEFINED_
#define __CLS_CSOLIDPART_DEFINED_
#include "HiberarchyId.h"
class CSolidPart{
public:
	HIBERID hiberid;
	long get_m_hPart(){return (long)hiberid.masterId;}
	long set_m_hPart(long hPartId){
		hiberid.masterId=(DWORD)hPartId;
		return hPartId;
	}
	// �����ľ��
	__declspec(property(put=set_m_hPart,get=get_m_hPart)) long m_hPart;
	int  m_idClassType;	// ����������
	GECS acs;			// ������װ������ϵ
	BOOL m_bDisplaySingle;	//���͹����Ƿ���Ҫ�ڵ���ͼ����ʾ
	BOOL m_bSelected;		//�����Ƿ�ǰ��ѡ��(������ֻ�ɳ������)
	int m_iLineType;		//0:����Ϊʵ�����ͣ�1:����Ϊֱ���͹�����2:����Ϊ�����͹���; 3:��ͷ
	f3dLine line;		// ����״̬�µļ��߼�	//��ͷ����
	f3dArcLine arc;			// ����״̬�µĻ��߼���
	double dfSphereRadius;
	CSolidBody *pBody;		// ��ʾ������ʵ����ʾ����
public:
	CSolidPart(){
		m_iLineType=1;
		m_bDisplaySingle=TRUE;
		m_bSelected=FALSE;pBody=NULL;
		m_hPart = m_idClassType=0;
		dfSphereRadius=0;
	}
};
#endif
enum TREEITEM_TYPE{
	NONE_INFO,
	//��ͼ
	VIEW_GROUP,
	VIEW_LAYERSET,
	VIEW_DEFINEDSET,
	VIEW_DEFINEDFOLDER,
	VIEW_CASE,
	//����
	BLOCK_REF,
	BLOCK_GROUP,
	BLOCKREF_GROUP,
	BLOCK_CASE,
	BLOCKREF_CASE,
	BLOCK_VIEW_CASE,
	BLOCKREF_VIEW_CASE,
	//����ģ����
	DATA_SPACE,
	MODEL_GROUP,
	MODEL_LANDSCAPE,
	MODEL_SAFEGRADE,
	MODEL_VOLTAGE,
	MODEL_ICE_DENSITY,
	MODEL_CASE,
	MODEL_LAND_HIGH,
	MODEL_QUAD1LEG_CFGNO,
	MODEL_QUAD2LEG_CFGNO,
	MODEL_QUAD3LEG_CFGNO,
	MODEL_QUAD4LEG_CFGNO,
	MODEL_BODY_LEGNO,
	MODEL_BURGLAR_LS_SCOPE,
	MODEL_DATUM_POINT_GROUP,
	MODEL_DATUM_POINT_CASE,
	MODEL_DATUM_LINE_GROUP,
	MODEL_DATUM_LINE_CASE,
	MODEL_DATUM_PLANE_GROUP,
	MODEL_DATUM_PLANE_CASE,
	MODEL_ARCLIFT_GROUP,
	MODEL_ARCLIFT_CASE,
	MODEL_RESTRICT,
	MODEL_RESTRICT_NODE,
	//���߷���
	WIRELAYOUT_GROUP,
	WIRELAYOUT_CASE,
	//�������ؼ���ģ��
	WIRELOAD_MODEL_GROUP,
	WIRELOAD_MODEL_CASE,
	//���㹤��
	WORKSTATUS_GROUP,
	WORKSTATUS_CASE,
	WORKSTATUS_SCOPE,
	WORKSTATUS_ICE_THICK,
	WORKSTATUS_POINTLOAD,
	WORKSTATUS_POINTLOAD_CASE,
	WORKSTATUS_WEIGHTLOAD,
	WORKSTATUS_WINDLOAD,
	WORKSTATUS_EARTHQUAKE,
	//���ӷ����
	WORKSTATUS_ATTACH_WINDLOAD,
	WORKSTATUS_ATTACH_WINDLOADITEM,
	WORKSTATUS_ATTACH_WINDLOADREF,
	WORKSTATUS_ATTACH_WINDLOADREFITEM,
	//��ѹ��
	WINDSEG_GROUP,
	WINDSEG_CASE,
	WINDSEG_MIRMSG,
	WINDSEG_TYPENO,
	WINDSEG_STYLE,
	WINDSEG_FRONTCOEF,
	WINDSEG_SIDECOEF,
	//����
	TREE_ITEM_MAT,
	MATERIAL_GROUP,
	MATERIAL_RECORD,
	MATERIAL_E_MODULUS,
	MATERIAL_F_INTENSITY,	//����ѹ��ǿ�����ֵ
	MATERIAL_FC_INTENSITY,	//�ױ۳�ѹǿ�����ֵ
	BOLT_GROUP,
	BOLT_TYPE_RECORD,
	//��������
	TREE_ITEM_PARTTYPE,
	TYPE_ANGLE_IRON,
	TYPE_COMPOSITE_ANGLE_IRON,
	TYPE_STEEL_TUBE,
	TYPE_SLOT_IRON,
	TYPE_FLAT_IRON,
	TYPE_STEEL_GUY,
	//�Ż�ѡ��
	FL_OPTIMIZOR_GROUP,
	//װ������ϵ
	PARAMETRIC_CS_GROUP,
};

struct ANGLEREPLACER_TAG{
	struct ANGLE_STATE{
		bool replaceStartDependRod;
		bool replaceEndDependRod;
		ANGLE_STATE(){
			replaceStartDependRod=replaceEndDependRod=false;
		}
	};
	CHashList<ANGLE_STATE> hashAngles;
};
struct HANDLEEX{
	long handle;
	long set_hPoint(long _hPoint);
	long get_hPoint();
	__declspec(property(put=set_hPoint,get=get_hPoint)) long hNodePoint;
	bool set_bSimpleNodePoint(bool _bSimpleNodePoint);
	bool get_bSimpleNodePoint();
	__declspec(property(put=set_bSimpleNodePoint,get=get_bSimpleNodePoint)) bool bSimpleNodePoint;
	operator long(){return handle;}
	long operator =(long _hPoint){
		handle=_hPoint&0x7fffffff;
		return handle;
	}
	HANDLEEX(long _hPoint=0,bool isSimpleNodePoint=false){
		handle=_hPoint&0x7fffffff;
		if(isSimpleNodePoint)
			handle|=0x80000000;
	}
};
struct HANDLEPOINT{
//protected:
	char ciHidType;	//handle����0.CLDSPoint.handle;'1':CLDSNode.handle;'S':CLDSLinePart.handle->PosStart;'E':CLDSLinePart.handle->PosEnd
public:
	long handle;
	bool set_bSimpleNodePoint(bool _bSimpleNodePoint);
	bool get_bSimpleNodePoint();
	__declspec(property(put=set_bSimpleNodePoint,get=get_bSimpleNodePoint)) bool bSimpleNodePoint;
	bool set_bRodTipStart(bool _bRodTipPoint);
	bool get_bRodTipStart();
	__declspec(property(put=set_bRodTipStart,get=get_bRodTipStart)) bool bRodTipStart;
	bool set_bRodTipEnd(bool _bRodTipPoint);
	bool get_bRodTipEnd();
	__declspec(property(put=set_bRodTipEnd,get=get_bRodTipEnd)) bool bRodTipEnd;
	operator long(){return handle;}
	long operator =(long _hPoint){
		handle=_hPoint&0x7fffffff;
		return handle;
	}
	HANDLEPOINT(long _hPoint=0,char ciHidType=0);
};
struct HANDLE_ADDR {
	BYTE ciAddrType;		//0.��ʾָ����ֵ�����ĵ�ַָ��;1.��ʾָ�򹹼����ֵ����ȳ������ַ���������ַָ��
	WORD wnMaxStrBuffLen;	//ciAddrType=1ʱ��pszThickPartStrָ����ַ������泤��
	union{
		long* pHandlVarAddr;	//�洢���ֵ���ڴ����Ա���
		char* pszThickPartStr;	//�洢�������ֵ����ȳ������ַ���
	};
	HANDLE_ADDR(long* pVarAddr = NULL) {
		wnMaxStrBuffLen = ciAddrType = 0;
		pHandlVarAddr = pVarAddr;
	}
	HANDLE_ADDR(char* pStrVarAddr,WORD wnStrBuffLen) {
		ciAddrType = 1;
		wnMaxStrBuffLen = wnStrBuffLen;
		pszThickPartStr = pStrVarAddr;
	}
	HANDLE_ADDR(void* pVarAddr, BYTE ciVarAddrType,WORD wnStrBuffLen=0) {
		ciAddrType = ciVarAddrType;
		wnMaxStrBuffLen = wnStrBuffLen;
		pHandlVarAddr = (long*)pVarAddr;
	}
	long get_HandleId() const;
	long set_HandleId(long hHandlValue);
	__declspec(property(put = set_HandleId, get = get_HandleId)) long HandleId;
};
class CHandleReplacer{
	struct SECTION{
		GEPOINT lineFilterStart,lineFilterEnd,lineStdVec;	//�����߶��������
		double sect_length;
		long hSectRod;
		CLDSLinePart* pSectRod;
		long priorSerial;	//���ȶ�(ֵԽ�����ȶ�Խ��)�������жϽڵ�Ϊ���߶ι�ͬ�˵�������жϽڵ�Ĺ�������
		SECTION();
	};
	bool m_bReplaceObjSelfHandle;	//true��ͬʱ�滻��������ı�ʶ�����false��ֻ�滻�����еĹ��������� wjh-2015.5.19
	bool m_bExcludeObjPointer;		//true:�滻���ʱ�ų�ֱ�Ӷ����ݶ����ָ�����ã���˼��Ķ˽ڵ�
	bool m_bVerifyGroupAngleReplaced;	//Ĭ��Ϊtrue����ȷ�ϵ�ǰ�滻�������ϽǸ�ʱ����������ӽǸ��滻����������Ч�� wjh-2016.11.10
	ATOM_LIST<SECTION> sections;
	long _hOld,_hOldSonAngles[4];
	bool m_bCollectiveTransition;
	bool m_bReverseStartAndEnd;
	bool m_bReverseWingXAndWingY;
public:
	long hNew;
	CLDSLinePart *pOldRod, *pNewRod;
	ANGLEREPLACER_TAG *pTag;
public:
	PROPERTY(BOOL,hOld);
	GET(hOld){return _hOld;}
	bool IsReplaceObjSelfHandle(){return m_bReplaceObjSelfHandle;}
	void SetReplaceObjSelfState(bool bReplaceSelf=true){m_bReplaceObjSelfHandle=bReplaceSelf;}
	bool IsExcludeReplaceObjPointer(){return m_bExcludeObjPointer;}
	void SetExcludeReplaceObjPointer(bool bExludeObjPointer=true){m_bExcludeObjPointer=bExludeObjPointer;}
	CHandleReplacer(long handleOld,long handleNew,f3dLine* line_filter=NULL,bool bCollectiveTransition=false,
		bool bReverseSAndE=false,bool bReverseXAndY=false);
	bool IsCollectiveTransition(){return m_bCollectiveTransition;}
	bool IsReverseStartAndEnd(){return m_bReverseStartAndEnd;}
	bool IsReverseWingXAndWingY(){return m_bReverseWingXAndWingY;}
	bool VerifyAndReplace(HANDLEEX* hCurrent,bool bObjSelfHandle=false);
	bool VerifyAndReplace(long* hCurrent,bool bObjSelfHandle=false);
	bool AppendSectRod(const double* lineStartCoord,const double* lineEndCoord,long hSectRod,long priorSerial=0);
	//�����ֵ㲻�����������������������������ͶӰ���жϣ���Ҫ�������������ϣ���Ӧ��AppendSectRodʱ��֤��wjh-2014.6.25
	long SelectPerfectPartHandle(const double* pos_coord);
};
//CMaxDouble��CMinDoubleת�Ƶ�XhMath.h�ж��� wjh-2019.2.21
#include "XhMath.h"
//CStdVector
//0.�Զ��� 1.X+ 2.X- 3.Y+ 4.Y- 5.Z+ 6.Z- 
class CStdVector
{
public:
	static f3dPoint GetVector(int nVecType);		//���ݷ������͵õ���׼����
	static int GetVectorType(const double* vec_coord);			//���ݷ���õ���׼��������
	static CString MakeVectorSetStr(bool bHasEditPara=false);				//��׼�����ַ���
	static CString GetVecTypeString(int nVecType);	//�õ��������Ͷ�Ӧ���ַ���
	static CString GetVecTypeString(const double* vec_coord);	//�õ����뷽���Ӧ���ַ���
};

class CLDSObject;
class CLDSDbObject;
class CLDSNode;
class CLDSPart;
class CLDSLinePart;
class CBlockModel;
class CBlockReference;
struct SUB_OBJ;
typedef CTmaPtrList<SUB_OBJ,SUB_OBJ*> SUBOBJSET;
typedef CTmaPtrList<CLDSObject,CLDSObject*> OBJSET;
typedef CHashSet<CLDSObject*> HASHOBJSET;
typedef CTmaPtrList<CLDSDbObject,CLDSDbObject*> DBOBJSET;
typedef CTmaPtrList<CLDSNode,CLDSNode*> NODESET;
#ifndef __PARTSET_DEF_
#define __PARTSET_DEF_
typedef CTmaPtrList<CLDSPart,CLDSPart*> PARTSET;
#endif
typedef CTmaPtrList<CLDSLinePart,CLDSLinePart*> LINEPARTSET;

/* �ò��ִ���������ComplexId.h,ComplexId.cpp�� wjh-2014.6.14
struct SUBID{
protected:
	long wholeId;
public:
	static const BYTE GROUP_DEFAULT	= 0;//Ĭ��ģʽ
	static const BYTE GROUP_BOUNDARY= 1;//���������α߿�
public:
	SUBID(long id=0,BYTE cGroup=0);	//��������ʶ�ϲ�Ϊ���ʶ
	virtual bool Set(long id=0,long group=0);
	virtual long Group() const;				//�����ʶ����ȡ��Ų��֣����ʶ��Ϣ����ռ5������λ
	virtual long Id() const;				//�����ʶ����ȡ��ʶ����
	long FromString(const char* subidStr);	//���ʶ���ӱ�ʶ��ͨ��'-'�ָ�
	CXhChar16 ToString(char cJunction='-',bool bIdOnAfter=true,int alignSize=0) const;
	operator DWORD(){return (DWORD)wholeId;}
	operator long(){return wholeId;}
	bool operator !=(long Id){return wholeId!=Id;}
};
*/
typedef struct tagLDSOBJNO
{
	int iNo;
	BOOL bInUse;
}LDSOBJNO;

class CObjNoManager;
class CObjNoGroup
{
	friend class CObjNoManager;
	long m_idInit;	//��ʼid(����С��idֵ),Ĭ��Ϊ1
	long m_idGroup;
	long m_nMaxNo;
	ATOM_LIST<LDSOBJNO>list;
public:
	CObjNoGroup(int nMaxNo=0);
	void InitMaxNo(int nMaxNo);
	int EnumIdleNo();
	void SetNoState(int iNo,BOOL bInUse=TRUE);
	long GetUsedNoCount();
	long GetFirstUsedNo();
	long GetNextUsedNo();
public:
	void EmptyNo();
};
class CObjNoManager
{
	ATOM_LIST<CObjNoGroup>NoGroupList;
public:
	CObjNoManager();
	CObjNoGroup* AppendGroup(long ID,long nMaxNo);
	CObjNoGroup* FromGroupID(long ID);
	void EmptyGroupNo();
	void EmptyGroup();
};
struct IConsole;
struct ISysPara
{
public:
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	BOOL m_bAuxPoleAnalysisInc;				//���Ĳ�Ԥ����Ԫ��ѧ��������
	//virtual int GetExecuteLoadCode()=0;
	//virtual double gamaG()=0;				//���ú��أ��������أ�����ϵ��
	//virtual double Beta_z_Design()=0;		//�Ż�ѡ��ʱ����ص���ϵ��������ϵ����
	//virtual double Beta_z_Foundation()=0;	//�������ʱ����ص���ϵ��������ϵ����
	//virtual bool IsReadOnly(int coefType)=0;
#endif
	virtual int GetBendClearance()=0;
	virtual BOOL IsPreferAngleSimplePosMode()=0;
	virtual BOOL IsDisplayAllHole()=0;
	virtual BYTE HoleWallDisplayMode()=0;
	virtual BOOL IsDelBoltsResideOnPlate()=0;
};
class CBlockParameter{
public:
	CBlockModel *m_pBlock;
	CBlockReference *m_pBlockRef;
	long m_hMotherObj;
	CBlockParameter(){
		m_pBlockRef=NULL;
		m_pBlock=NULL;
		m_hMotherObj=0;
	}
};
struct ILog
{
	virtual bool LogVersionCompare(void* pOldObj,void* pSchema,void** ppLogContents){return true;}
};
class COldVerDataFile;
long HexStrToLong(const char* idstr);
class CLDSObject : public ILog{
protected:
	BYTE m_bLocked;
public:
	static IConsole* _console;
	static ISysPara* sysPara;	//��ǰ����ѭ�ĺ��ؼ���淶
	static ILog2File* log2file;
	static const BYTE SHADOW_NONE	              = 0;	//��Ӱ�乹��
	static const BYTE SHADOW_ISOLATED             = 1;	//��ͨ��������Ӱ�乹��ʱ�Ĳ���(��¼ĸ�������������λ����ϵ
	static const BYTE SHADOW_EMBEDDED             = 2;	//Ƕ��ʽ������������Ӱ�乹��ʱ�Ĳ���(��¼ĸ��������������������Ŀ�����)
	static const BYTE SHADOW_EXTERBLK             = 3;	//����ʽ������������Ӱ�乹��ʱ�Ĳ���(��¼ĸ���������(���ڹ�����������������Ŀ�����
	static const BYTE SHADOW_ISOLATED_RELA_LOCATION = 4;//��������Ӱ�乹������Ӱ�乹��ʱ�Ĳ���(��¼ĸ���������������������Ӱ�乹�����)
	//Ӱ�乹������
	static const BYTE SHADOWMODE_NONE	= 0;	//��Ӱ�乹��
	static const BYTE SHADOWMODE_COMMON = 1;	//��ͨӰ�乹��,��ӦSHADOW_ISOLATED��SHADOW_ISOLATED_RELA_LOCATION
	static const BYTE SHADOWMODE_BLOCK	= 2;	//����װ��Ӱ�乹��,��ӦSHADOW_EMBEDDED��SHADOW_EXTERBLK
	static ILog2File* Log2File();	//��Զ�᷵��һ����Чָ��,��ֻ��log2file!=NULLʱ,�Ż�������¼������־ wjh-2015.2.7
protected:
	int m_nClassTypeId;
	//char m_sClassName[MAX_TEMP_CHAR_50+1];
	BOOL *m_pModified;
	bool is_undo_modified;	//������Undo����ʵ�ֹ����м�¼�����Ƿ������޸�
	IModel *m_pModel;
	//////////////////////////////////////////////////////////////////////////////////////
	//Ӱ�乹�����������
	CLDSObject *m_pMotherObj;
	/*
		m_iShadowObjType=SHADOW_ISOLATED, ��ͨӰ�乹��
		m_iShadowObjType=SHADOW_EMBEDDED, Ƕ��ʽ����װ�����ô�������Ӱ�乹��
		m_iShadowObjType=SHADOW_EXTERBLK, ����ʽ����װ������
		m_iShadowObjType=SHADOW_ISOLATED_RELA_LOCATION, ��ͨӰ�乹������Զ�λ����(��������˨��λ)
	*/
	BYTE m_iShadowObjType;		
	union SHADOW_PARA
	{
		struct{	//��ͨӰ�乹��, m_iShadowObjType=SHADOW_ISOLATED
			THANDLE m_hMotherObj;	//Ӱ��Դ������(m_pMotherObj,ע�ⲻһ����m_hAcsװ������ϵ�е�Դ������ͬ)
			THANDLE m_hAcs;			//Ӱ���Ĳ�����װ������ϵ���
		}COMMON_OBJ;
		struct{
			//m_iShadowObjType=SHADOW_EMBEDDEDʱָ������Ӱ��Դ��������m_iShadowObjType=SHADOW_EXTERBLKʱָ������Ӱ��Դ������
			THANDLE m_hMotherObj;
			THANDLE m_hBlockRef;	//װ�����ö���ľ��������ȷ����Ӱ�乹����װ������ϵ��
		}BLOCK_OBJ;
	}shadow;
public:
	THANDLE handle;
	long m_hBlock;				//������Ƕ��ʽ������������
	DWORD dwPermission;			//Ȩ���֣����ڱ�ʶ˭�Դ���ӵ���޸�Ȩ��
	long feature;				//�����û��Լ�����Ϣ(��ʱ����,���ɳ־ô洢��Ҳ��һ������)
	virtual void SetTagInfo(void* pTag){memcpy(&feature,&pTag,4);}
	virtual bool IsAcceptableTypeId(int type_id);
	virtual bool IsGeObject() const{return false;}
	virtual bool IsDbObject() const{return false;}
	virtual void CleanInvalidObjRef(){;}
	virtual void TransToCS(GECS& cs){;}
	virtual void Offset(const double* offset){;}
	virtual bool OffsetPosParam(const double* offset){return false;}
	CXhChar50 GetBlockIdName();
	CLDSObject();
	~CLDSObject();
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0){;}
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0){;}
	virtual void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	virtual void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	DWORD SetPermission(DWORD dwPerm);
	void GetClassType(char class_type[MAX_TEMP_CHAR_50+1]);
	virtual bool IsCompositeSonAngle(){return false;}	//�Ƿ�Ϊ��ϽǸֵ��ӽǸ�
	virtual int GetClassTypeId()const{return m_nClassTypeId;}
	virtual bool IsBelongTo(int iClassTypeId)const{return m_nClassTypeId==iClassTypeId;}
	void SetDocModifiedFlagAddress(BOOL *pModified)
	{	m_pModified=pModified;	}
	virtual void SetModified(BOOL bTrue=TRUE,BOOL bRenderSolid=TRUE){
		if(bTrue)
		{
			is_undo_modified=TRUE;
			SetDocModifiedFlag();
		}
	}
	virtual void ResetUndoModifiedFlag(){is_undo_modified=FALSE;}
	BOOL IsUndoModified(){return is_undo_modified;}
	void SetDocModifiedFlag()	//ֻ��֪ͨ�ĵ��������޸ģ����޸Ĺ��������κ�����
	{	if(m_pModified) *m_pModified=TRUE; }
	void SetBelongModel(IModel *pModel){m_pModel=pModel;}
	IModel* BelongModel() const;
	//bool bAddSubLevelRelaObj=false:��ʾ������Ӽ���������;true:��ʾ����Ӽ��������� wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true){;}
	bool Lock(){
		if(m_bLocked)
			return false;
		else
			m_bLocked = true;
		return true;
	}
	bool IsLocked(){return m_bLocked!=0;}
	void Unlock(){m_bLocked = false;}
	//Ӱ�ӹ�������ط���
	void SetShadowPara(CLDSObject *pMotherObj,long hObj,int iShadowObjType=SHADOW_ISOLATED);
	void SetShadowPara(CLDSObject *pMotherObj,CBlockReference *pBlockRef);
	virtual bool IsAggregated(){return false;}
	virtual void SetAggregateState(bool aggregated=true){;}	//���þۺ�״̬
	virtual BOOL GetObjPropReadOnlyDefaultState(long prop_id){return FALSE;}
	//����ͬһ�����Բ�ͬ�����ý����ͬ��GetPropValueStr����������Ϊvirtual ���� wjh-2017.5.17
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100){return 0;}	//��������ID�õ�����ֵ�ַ���
	void EmptyShadowPara();
	CLDSObject* UpdateMotherObject();
	//ancestor=trueʱ����ʼ�漶���󣬷��򷵻�ֱ�Ӹ�������Գ�ʱ��
	CLDSObject* MotherObject(bool ancestor=true);
	bool IsBlockRefShadowObject();	//�Ƿ�Ϊװ�䲿������ʱ������Ӱ�乹��
	BYTE ShadowMode();
	bool IsShadowObject();
	bool IsMotherObject();
	int GetMotherClassTypeId()const;
	bool IsMemberOfBlockRef(CBlockReference *pBlockRef);
	bool IsBlockRefPart();
	bool GetBlockParameter(CBlockParameter &blockPara);
	int GetShadowObjType(){return m_iShadowObjType;}
	CParaCS* GetParaCS();
	//
	bool ShadowParaAndBlockIdToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	bool ShadowParaAndBlockIdFromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
public://��������
	virtual bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType){return false;}
	virtual bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType){return false;}
	virtual long get_Id();
	virtual long set_Id(long hId);
	_declspec(property(put=set_Id,get=get_Id)) long Id;	//����ľ��ֵ
};
class COldVerLdsObjLife{
private:
	bool m_bRestore;
	long m_iVersion;
	long m_iDocType;
	CBuffer m_xBuffer;
	CLDSObject* m_pSrcObj;
	COldVerDataFile* m_pOldVerInfo;
public:
	COldVerLdsObjLife();
	COldVerLdsObjLife(COldVerDataFile* pOldVerInfo,CLDSObject* pObj,long version=0,long doc_type=0);
	~COldVerLdsObjLife();
};
class RELATIVE_OBJECT{
public:
	RELATIVE_OBJECT(){;}
	RELATIVE_OBJECT(long h,MIRMSG mirmsg){hObj=h;mirInfo=mirmsg;}
	long hObj;
	MIRMSG mirInfo;
	void SetKey(DWORD key){hObj=(long)key;}
};
class CFGINFO{
	CFGWORD _cfgword;	//��ĺ���Ϣ
	bool m_bLegObj;		//�Ƿ�Ϊ���ȶ���
public:
	CFGINFO(){m_bLegObj=true;}
	CFGINFO(const CFGWORD& cfgword,bool bLegObj){Set(cfgword,bLegObj);}
	void Set(const CFGWORD& cfgword,bool bLegObj)
	{
		m_bLegObj=bLegObj;
		if(!bLegObj)	//�Ա�����������ĺ�ͳһ������
			_cfgword=cfgword.ToBodyWord();
	}
	bool IsLegObj() const{return m_bLegObj;}
	CFGWORD Cfgword() const{return _cfgword;}
};
class MEMORY_BLOCK{
	WORD  wSize;
	union{
		UINT u4Bits;
		char* block;
	};
public:
	MEMORY_BLOCK(){wSize=4;u4Bits=0;}
	MEMORY_BLOCK(MEMORY_BLOCK& memblock);
	~MEMORY_BLOCK(){ReleaseBlockBuffer();}
	void ReleaseBlockBuffer();
	WORD Size() const{return wSize;}
	bool operator==(const MEMORY_BLOCK& memblock);
	MEMORY_BLOCK& operator=(const MEMORY_BLOCK& memblock);
	const char* Data(WORD wiByteIndex=0) const;
	bool IsZero() const{return (wSize<=4&&u4Bits==0);}
	bool GetBitState(WORD bitpos);	//bitpos��0Ϊ����
	void SetBitState(WORD bitpos, bool state=true);
	void CopyState(WORD addr_dest, const char* fromStr,WORD size);
	void FromBuffer(BUFFER_IO* buffer_io,long version=NULL,long doc_type=0);
	void ToBuffer(BUFFER_IO* buffer_io,long version=NULL,long doc_type=0);
};
class CLDSDbObject : public CLDSObject
{
protected:
	char layer_tag[4];		//����ͼ��
	SEGI _iSeg;				//�ֶκ�
	bool _bSyncShadowPartNo;
//protected:
	char m_ciLegQuad;		//���Ȳ��ֽڵ㡢�˼������������Ĺ�����������1��2��3��4��0��ʾ����λ�������ж�
	virtual char get_ciLegQuad();
	virtual char set_ciLegQuad(char ciBelongLegQuad);
public:
	PROPERTY(char,ciLegQuad);
	GET(ciLegQuad){ return get_ciLegQuad();}
	SET(ciLegQuad){ set_ciLegQuad(value); }
public:
	struct PROPVAL{
		static const BYTE TYPE_NA		=0;
		static const BYTE TYPE_BOOL		=1;
		static const BYTE TYPE_BYTE		=2;
		static const BYTE TYPE_WORD		=3;
		static const BYTE TYPE_DWORD	=4;
		static const BYTE TYPE_LONG		=5;
		static const BYTE TYPE_DOUBLE	=6;
		static const BYTE TYPE_COORD3D	=7;
		BYTE ciType;	//0.undefined;1.bool;2.BYTE;3.WORD;4.DWORD;5.long;6.double;7.COORD3D
		union{
			bool blVal;
			BYTE cbVal;
			WORD wiVal;
			DWORD dwVal;
			long liVal;
			double fVal;
			COORD3D vPoint;
		};
		BYTE ToByte();
		WORD ToWord();
		DWORD ToDword();
		long ToLong();
		double ToDouble();
	};
	static const UINT PROP_LOCATION	;//=1;		//װ��λ��
	static const UINT PROP_SIZESPEC	;//=2;		//�������
	static const UINT PROP_WIDTH	;//=3;		//�������
	static const UINT PROP_THICK	;//=4;		//�������
	static const UINT PROP_LENGTH	;//=5;		//��������
	static const UINT PROP_MATERIAL	;//=6;		//��������
	static const UINT PROP_NORMAL	;//=7;		//��������
	static const UINT PROP_WINGX_NORMAL	;//=8;	//�Ǹ�X֫����
	static const UINT PROP_WINGY_NORMAL	;//=9;	//�Ǹ�Y֫����
	static const UINT PROP_POS_START	;//=10;	//�˼�ʼ��λ��
	static const UINT PROP_POS_END		;//=11;	//�˼��ն�λ��
	static const UINT PROP_PROFILE		;//=12;	//�ְ�����
	virtual bool GetPropParam(long idObjProp,PROPVAL* pVal=NULL){return false;}
	virtual bool SetPropParam(long idObjProp,PROPVAL& propval){return false;}
	virtual void IncrementConstraintRef();
	CONSTRAINT* AppendConstraint(CONSTRAINT* pCopyFrom=NULL);
	CONSTRAINT* GetConstraintByPropID(WORD idDbProp);
	bool RemoveConstraint(long constraint_id);
	IXhTemplEnumerator<CONSTRAINT>* GetConstraintEnumerator();
	virtual bool ProcessConstraintFromMasterObj(CLDSDbObject* pMasterObj,CONSTRAINT* pConstraint);
	virtual long ReDesignConstraintSonObjs(WORD idMasterObjProp=0,CHashSet<CLDSDbObject*>* pHashUpdateObjSet=NULL);
protected:
	long m_nHasConstraintsCounts;
public:
	READONLY_PROPERTY(bool,bHasConstraints);
	GET(bHasConstraints){return m_nHasConstraintsCounts>0;}
public:	//��������
	PROPERTY(SEGI,iSeg);
	GET(iSeg){
		if(!_bSyncShadowPartNo)
			return _iSeg;
		CLDSDbObject* pMotherObj=(CLDSDbObject*)MotherObject();
		if(pMotherObj==this)
			return _iSeg;
		else
			return pMotherObj->iSeg;
	}
	SET(iSeg){
		if(!_bSyncShadowPartNo)
		{
			_iSeg=value;
			SetModified(TRUE,FALSE);
			return;
		}
		CLDSDbObject* pMotherObj=(CLDSDbObject*)MotherObject();
		if(_iSeg.iSeg!=value.iSeg)
		{
			_iSeg=value;
			UpdatePartNoBySegI();
		}
		if(pMotherObj!=this)
			pMotherObj->iSeg=value;
	}
	PROPERTY(bool,m_bSyncShadowPartNo);
	GET(m_bSyncShadowPartNo){ return _bSyncShadowPartNo;}
	SET(m_bSyncShadowPartNo){ _bSyncShadowPartNo=value; }
public:
	BOOL is_highlight;		//�Ƿ������ʾ
	BOOL is_visible;		//�Ƿ�����ʾ״̬
	char sNotes[MAX_TEMP_CHAR_50+1];
	CFGWORD cfgword;		//��ĺ�
	CHashListEx<RELATIVE_OBJECT> relativeObjs;
	RELATIVE_OBJECT* GetMirRelaObj(MIRMSG& mirmsg){
		for(RELATIVE_OBJECT* pMirRelaObj=relativeObjs.GetFirst();pMirRelaObj;pMirRelaObj=relativeObjs.GetNext())
		{
			if(pMirRelaObj->mirInfo.IsSameMirMsg(mirmsg))
				return pMirRelaObj;
		}
		return NULL;
	}
	MEMORY_BLOCK mpcl;		//��Ե�ǰ������ض����ԶԳƿ���
	virtual PROPLIST_TYPE* GetSyncPropList(){return NULL;}
	virtual MEMORY_BLOCK GetDefaultMPCL(){return mpcl;}
public:
	static void MirObjLayer(const char* sOrgLayer,char* sNewLayer,MIRMSG mirmsg);
	virtual CFGWORD UnifiedCfgword();//��Ϊ��������򷵻ز���ȫ������ռλ�����ĺ�
	virtual CFGWORD GetUnifiedCfgword() const;//��Ϊ��������򷵻ز���ȫ������ռλ�����ĺ�
	virtual CFGINFO Cfginfo(){return CFGINFO(cfgword,IsLegObj());}	//��ȡ�ö���������Ϣ
	virtual bool ModuleTogetherWith(const CFGINFO& cfginfo) const;	//�ж��������Ƿ񹲴���ͬһ����ʵ��
	virtual bool ModuleTogetherWith(CLDSDbObject* pDbObj) const;		//�ж��������Ƿ񹲴���ͬһ����ʵ��
	virtual void CopyModuleInstanceInfo(CLDSDbObject* pDbObj);	//�����������ģ��ʵ�������Ϣ����ĺż�ͼ������ĸ��
	virtual bool IsLegObj()const{return toupper(layer_tag[0])=='L';}	//�Ƿ�Ϊ���ȶ���
	virtual bool IsBodyObj(){return toupper(layer_tag[0])!='L';}//�Ƿ�Ϊ����(��ͷ����)����
	virtual bool IsAcceptableTypeId(int type_id);
	virtual bool IsGeObject() const{return false;}
	virtual bool IsDbObject() const{return true;}
	virtual bool IsPart() const{return false;}
	virtual bool IsLinePart() const{return false;}
	virtual bool IsNode() const{return false;}
	virtual void ResetRelativeObjs(){;}
	virtual long ArcLiftHandle(){return 0;}
	virtual long SetArcLift(long hArcLift){return 0;}
	virtual void UpdatePartNoBySegI(){;}
	virtual void CalPosition(bool bIgnoreRepeatFlag=false){;}
	virtual void UpdateRelaObjArcLiftProp();
	//ͬ������
	static const DWORD PROP_DISCONNECT	= 0;	//�Ͽ�
	static const DWORD PROP_SYNC_COPY	= 1;	//ͬ���Գ�
	static const BYTE MIR_X = 1;	//X��Գ�
	static const BYTE MIR_Y = 2;	//Y��Գ�
	static const BYTE MIR_Z = 3;	//Z��Գ�
	virtual bool IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr);
	virtual bool SyncMirProp(const char* propKeyStr=NULL,BYTE ciMirAll0X1Y2Z3=0,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ������
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ��λ�öԳ��޹ص�����
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,long hMirAcs,short wRotAngle,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff){return false;}
	RELATIVE_OBJECT *AppendRelativeObj(RELATIVE_OBJECT &relaObj);
	virtual BOOL IsFatherObject();
	RELATIVE_OBJECT* GetRelativeObjInfo(long h,bool bIncRotateMir=true);
	//bIncSelf �Ƿ�����ԶԳ����
	virtual long GetRelativeMirObj(MIRMSG msg,bool bIncSelf=TRUE);
	virtual BOOL GetFatherMirObj(RELATIVE_OBJECT *pRelaObj);
	virtual void CopyProperty(CLDSDbObject* pObj){;}
	CLDSDbObject();
	char& layer(int i);
	char  Layer(int i) const;
	char* layer();
	char get_chLayer1st();
	char set_chLayer1st(char chValue);
	__declspec(property(put=set_chLayer1st, get=get_chLayer1st)) char chLayer1st;
	char get_chLayer2nd();
	char set_chLayer2nd(char chValue);
	__declspec(property(put=set_chLayer2nd, get=get_chLayer2nd)) char chLayer2nd;
	char get_chLayer3rd();
	char set_chLayer3rd(char chValue);
	__declspec(property(put=set_chLayer3rd, get=get_chLayer3rd)) char chLayer3rd;
	BOOL SetLayer(const char *layer);
	const char* get_szLayer() { return this->layer_tag; }
	__declspec(property(get = get_szLayer)) const char* szLayer;
	virtual void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	virtual void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	//���Թ���
	static int m_iCurDisplayPropGroup;
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;		//����״̬��ϣ��
	UPDATE_PROPLISTITEM_FUNC(CLDSDbObject,propHashtable)
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
	static void InitDbObjectPropHashtable();
	static int GetPropStr(long id, char *propStr);		//�õ������ַ���
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
};
class CLDSAnchorBolt;
typedef CLDSAnchorBolt* CLDSAnchorBoltPtr;
class CLDSFoundation : public CLDSDbObject
{
	BOOL is_data_modified;
	BOOL is_solid_modified;
public:
	BYTE ciHeightSerial;//������Ӧ�ĺ��ߺ�
	BYTE ciLegSerial;	//�����ڹ��������еļ��Ⱥţ�һ�������Ϊ1����->�������𼶼�1��
	BYTE ciAnchorBoltD;	//������˨ֱ��
	WORD wiWidth;		//�������������ʾ��ȣ�����������ɫ��ʾ����
	WORD wiHeight;		//������(��)��
	COLORREF crColor;	//Ԥ����ʾ��ɫ
	//struct LCS{		//����������λ����ϵ
	GEPOINT location;	//�����������ĵ�λ��
	GEPOINT vWorkNormal;//������ƽ�淨�ߣ����泯�ϣ�
	GEPOINT vProjAxisX;	//�ֲ�(����ƽ��)����ϵ��X��ˮƽͶӰ����
	//};
	CSolidBody *pSolidBody;
	//BYTE ciQuad;		//���ڸ���CLDSDbObject�ж���
	struct LOCATION{
		long hAnchorBolt;	//�����ĵؽ���˨���
		short siPosX,siPosY;
		LOCATION(short posx=0,short posy=0,long _hRelaBolt=0){
			hAnchorBolt=_hRelaBolt;
			siPosX=posx;
			siPosY=posy;
		}
		void SetLocation(short posx,short posy)
		{
			siPosX=posx;
			siPosY=posy;
		}
	};
	PRESET_ARRAY8<LOCATION> anchors;
public:
	CLDSFoundation(IModel *pModel=NULL);
	~CLDSFoundation();
	//
	virtual char get_ciLegQuad();
	virtual void SetModified(BOOL bTrue=TRUE,BOOL bRenderSolid=TRUE);
	virtual CSolidPart GetSolidPartObject();
	BOOL Create3dSolidModel();
	BOOL IsModified(){return is_data_modified;}
	BOOL IsSolidModified(){return is_solid_modified;}
	GEPOINT GetAnchorLocalPosition(CLDSBolt* pAnchorBolt);
	GEPOINT TransToModelCS(const double* xAnchorBoltLocalLocation);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void LayoutAnchorBolts(int schema=0,short S=200);	//schema=0��ʾ����˨����;schema=1��ʾ����˨����
	GECS GetAcs();	//��ȡ������װ������ϵ
	UINT AddOrUpdateAnchorBolts(WORD wiBasePlateThick=0,bool blMatchDetachedAnchor=true,bool blAppendLostAnchorBolts=true);
	//������
	DECLARE_PROP_FUNC(CLDSFoundation);
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ
};

class CLDSGeObject : public CLDSObject
{
public:
	virtual bool IsAcceptableTypeId(int type_id);
	virtual bool IsGeObject() const{return true;}
	virtual bool IsDbObject() const{return false;}
	CLDSGeObject();
};
//������
#pragma pack(push,1)
struct THICK_ITEM{
public:
	static const int PART_THROUGH	 =1;	//�����������ڴ�����һItemType=3ʱ������ͨ��
	static const int THICK_THROUGH	 =2;	//��������ָ�����
	static const int PART_THROUGHnT  =3;	//�������������������Լ���ͨ�񹹼���
	char cItemType;	//0.FinalKey;1.���׹���(����ͨ�񣩾��ֵ;2.���ֵ;3.���ܺ������ƵĴ��ף����������Լ���ͨ�񹹼����������
public:
	int  val;
	THICK_ITEM(const char* data=NULL);
	//��Ҫ����GetNextItem()����������ָ���Ƿ��Ѿ�����
	bool IsFinalKey();	//{return (cItemType<=0||cItemType>3);}
	bool IsHandle();	//{return cItemType==1||cItemType==3;}
};
struct THICK_ITEMTAG : public THICK_ITEM
{
	CLDSPart *pPart;//�����Ĺ���
	int through;	//������ͨ��
	double zOffset;	//��������ʼλ��Zƫ����
public:
	THICK_ITEMTAG(const THICK_ITEM& item){
		cItemType=item.cItemType;
		val=item.val;
		pPart=NULL;
		zOffset=through=0;
	};
	THICK_ITEMTAG(){pPart=NULL;zOffset=through=0;}
};
#pragma pack(pop)
class CDesignThickPara
{
	CXhChar50 keystr;	//��ʱ(transient)�洢��,�����⹫��,��Ҫ����֧��GetFirstItem,GetNextItem
public:
	static IModel* m_pTower;
	bool ReplaceReferenceHandle(CHandleReplacer& replacer);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	void CleanInvalidObjRef(IModel *pModel=NULL);
	bool AddThick(int val,BOOL bHandle=FALSE,BOOL bSamePartFilter=FALSE);
	CDesignThickPara(){memset(this,0,sizeof(CDesignThickPara));}
	CDesignThickPara(const char *str){key_str.Copy(str);}
	CDesignThickPara(double dblThick);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	CXhChar50 key_str;
	long Thick(IModel* pTower,bool bEnableModuleFilter=false);	
	CDesignThickPara MirThickPara(IModel* pModel,MIRMSG msg);
	void ClearRedundant(IModel* pTower=NULL);
	void EmptyThick();
	THICK_ITEM GetFirstItem();
	THICK_ITEM GetNextItem();
	//void Serialize(CArchive &ar);
};
class IHeapMemory;
class CParaThick
{
	//��ʱ��MAX_BUFF_LEN�ӵ�220��δ��Ӧ����ͨ��IHeapMemory��������ּ������ wjh-2019.8.12
#ifndef __LDS_FILE_
	static const int MAX_BUFF_LEN	= 160;	//100->160����60�ֽڣ�ͨ��CLDSBolt�м�������ռ���ֽڽ�ʡ80���ֽ� wjh-2019.7.16
	static const int MAX_ITEM_COUNT =32;	//100->160����60�ֽڣ�ͨ��CLDSBolt�м�������ռ���ֽڽ�ʡ80���ֽ� wjh-2019.7.16
#else
	static const int MAX_BUFF_LEN	= 220;	//100->160����60�ֽڣ�ͨ��CLDSBolt�м�������ռ���ֽڽ�ʡ80���ֽ� wjh-2019.7.16
	static const int MAX_ITEM_COUNT =44;	//100->160����60�ֽڣ�ͨ��CLDSBolt�м�������ռ���ֽڽ�ʡ80���ֽ� wjh-2019.7.16
#endif
	char ciIndex,datapool[MAX_BUFF_LEN];
	char* m_pAllocData;	//m_pExterAllocData!=NULLʱ����m_pExterAllocDataΪ��׼
	WORD m_wnAllocDataLength;
	long get_hHeapMemory();
	long set_hHeapMemory(long hMemory);
	__declspec(property(put=set_hHeapMemory,get=get_hHeapMemory)) long hHeapMemory;
	char* get_pData();
	__declspec(property(get=get_pData)) char* data;
public:
	static IModel* m_pTower;
	static IHeapMemory* m_pExterHeapMemory;
	//THICK_ITEM::cItemType;	//0.FinalKey;1.���׹���(����ͨ�񣩾��ֵ;2.���ֵ;3.���ܺ������ƵĴ��ף�����ͨ�񣩹������
	void AttachExterData(char* pExterData,UINT uiDataLength);
	void CopyFromData(char* pExterData,UINT uiDataLength);
	void DetachAllocData();
	UINT MaxBuffLen() const;
	__declspec(property(get=MaxBuffLen)) UINT uiMaxBuffLength;
	bool ReplaceReferenceHandle(CHandleReplacer& replacer);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	//void CleanInvalidObjRef();
	bool AddThick(int val,BOOL bHandle=FALSE,BOOL bSamePartFilter=FALSE);
	void BuildFromStr(const char *str);	//��*����'X',��ʾ���ܵ�ǰ�������ƵĴ��׹���wjh-2015.4.10
	void FromBuffStr(CBuffer &buffer);
	CParaThick(){memset(this,0,sizeof(CParaThick));}
	CParaThick(const char *str){BuildFromStr(str);}
	CParaThick(double dblThick);
	bool SetItem(int i,THICK_ITEM item);	//iȡֵ0~19,��ʾ���������������ڴ�data��������wjh-2015.4.10
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//�ȶ�ʱ�����νӰ��ϵ���˨���ܴ���10~20��壬��ʱ�ַ�����ɴ�200����, 
	//�ʴ˷����ַ������CXhChar500 wjh-2019.7.11
	CXhChar500 ToThickStr(bool toLegacyMode=false) const;
	long Thick(IModel* pTower,bool bEnableModuleFilter=false,bool* pHasPriorPartThick=NULL);	
	CParaThick MirThickPara(IModel* pModel,MIRMSG msg);
	void ClearRedundant(IModel* pTower=NULL);
	void EmptyThick();
	THICK_ITEM GetFirstItem();
	THICK_ITEM GetNextItem();
};
class CLDSArcLift : public CLDSGeObject
{
public:
	long iNo;				//Ԥ�����
	char name[MAX_TEMP_CHAR_50+1];
	char m_ciLiftType;		//0.Բ��Ԥ��;1.��ֱԤ��(Z��Ԥ��)
	char m_cRotAxis;		//Ԥ��ʱ��ת��'X':X��;'Y':Y��
	GEPOINT m_xRotOrg;		//Ԥ����ת����
	GEPOINT m_xDatumLiftPos;//��׼Ԥ�����ʼλ��
	double m_fLiftHeight;	//��׼Ԥ�����Ԥ��ֵ,mm
public:
	CLDSArcLift();
	// initPos:��ʼ��λ�ã�bInRightSide:��ʼ������ת���Ҳ�(��෴����תԤ��)
	f3dPoint LiftPosTrans(const f3dPoint& initPos);
	//��������Ԥ����ԭ���㣨��Ҫ���ڸ���Ԥ����ָ���Ǹ�Ԥ��ǰ������꣩wjh-2015.5.19
	f3dPoint ReverseLiftPosTrans(const f3dPoint& arcPos);
	bool IsInRightSide(const f3dPoint& pickPos);
	// initPos:��ʼ����bInRightSide:��ʼ������ת���Ҳ�(��෴����תԤ��)
	f3dPoint LiftVecTrans(const f3dPoint& initVec,bool bInRightSide=true);
	f3dPoint LiftVecTrans(const f3dPoint& initVec,const f3dPoint& pickPos);
	f3dPoint ReverseLiftVecTrans(const f3dPoint& initVec,bool bInRightSide=true);
	f3dPoint ReverseLiftVecTrans(const f3dPoint& initVec,const f3dPoint& pickPos);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
};
struct VECTOR_PARA{
	//0.ָ��;1.�Ǹ�֫����;2.�˼������淨��3.������������4.��׼�����ϵ���Է���5.��׼�˼��ϵ���������
	short norm_style;
	//norm_style==1��3:	0.����;1.���� 
	//norm_style==5:	0.ʼ->��      1.��->ʼ 
	//					2.+�Ǹ�norm_x(�Ի�׼��ƽ�淨���Ƹֹ���ʱ����תrot_ang�ǶȺ�ķ���
	//					  ��ֹ������߷���+
	//					  �۸�֫���߷���(X�᷽��)+)
	//					3.-�Ǹ�norm_x(�Ժ�����ƽ�淨���Ƹֹ���ʱ����תrot_ang�ǶȺ�ķ���
	//					  ��ֹ������߷���-
	//					  �۸�֫���߷���(X�᷽��)-)
	//					4.+�Ǹ�norm_y (�ֹ�ʼ���и�ƽ�淨�ߡ��۸ֲ��淨�߷���(Y�᷽��)+)
	//					����.-�Ǹ�norm_y(�ֹ��ն��и�ƽ�淨�ߡ��۸ֲ��淨�߷���(Y�᷽��)-)
	short direction;
	THANDLE hVicePart;			//�������߻�׼�˼�
	THANDLE hCrossPart;			//����˼�
	union{
		short norm_wing;	//0.X֫;1.Y֫�����Ի�׼�����ǽǸ�ʱ��Ч��
		BYTE reside_plane;	//0.���ڸְ����(0��ʾ����;1.��ʾ��һ��������;2.��ʾ�ڶ���������) wjh-2015.2.7
	};
	double fRotAngAroundTube;	//�Ƹֹ���ʱ����תָ���ĽǶȡ�(��һ��ָ��ʱ���ǽǶ��ƣ��ʴ˴��洢�Ƕȵ�λ��)
	COORD3D vNearVector;
public:
	void ZeroClear();
	void ToBuffer(BUFFER_IO* pIO);
	void FromBuffer(BUFFER_IO* pIO);
};
struct POINT_PARA{
	/*  0.��λ�������Ϣ,ֱ��ָ��1.�Ǹ��������Ϊ��׼;2.�˼��Ͻڵ�Ϊ��׼;
		3.�Ǹ����߽���;4.��׼�߽���;5.��׼�����������겻���;
		6.��׼�����������겻���,7.�ؽǸ���֫����ƫ�Ƶİ������,8.��ڵ�ͬλ,
		9.ָ�������ϵ��������,10.���ڵ������ȷֵ�(�����ͽ�������嶨λ,���ڻ�׼��Ի�������ʾ)
		11.���涨λ��
	*/
	BYTE ciLocationType;	//datum_pos_style
	COORD3D xDatumPos;
	union
	{
		THANDLE hNode;	//��ڵ�ͬλ
		THANDLE hPart;	//ָ�������ϵ��������
		struct{
			THANDLE hDatum1;
			THANDLE hDatum2;
			short wing_offset_style1,wing_offset_style2;	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			long wing_offset_dist1,wing_offset_dist2;
			double near_vector[3];	//���ڷ���ƫ������ֵʱ��Ч
			char cProjectPlane;	//ͶӰ������0.�����棨�������Ϊ�淨�ߣ�;1.X-Zƽ��;2.Y-Zƽ��;3.X-Yƽ��
			char norm_offset[MAX_TEMP_CHAR_50+1];//����ƫ����;
		}AXIS_INTERS;
		struct tagRODEND{
			THANDLE hRod;		//�˼����
			short direction;	//0.��׼�˼�����:ʼ->��;1.��->ʼ
			short offset_wing;  //0.X֫;1.Y֫
			short wing_offset_style;	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			double wing_offset_dist,len_offset_dist;//�Ǹ�֫��ƫ��ֵ;����ƫ��ֵ;
			BOOL bIncOddEffect;			//����ƫ��ֵ��������ͷӰ��
			char norm_offset[MAX_TEMP_CHAR_50+1];			//����ƫ����
			void SetWingOffset(double wing_offset=0,short offset_g_style=4);
		}RODEND;	//�Ժ�Ϊ�������Ӧͳһ����ΪRODEND wjh-2013.11.16
		struct{
			THANDLE hRod;	//�Ǹ־��
			THANDLE hNode;	//��׼�ڵ���
			WORD bFaceOffset;
			short direction;	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
			short offset_wing;//0.X֫;1.Y֫
			short wing_offset_style;	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			double wing_offset_dist,len_offset_dist;//֫��ƫ��ֵ;����ƫ��ֵ;
			char norm_offset[MAX_TEMP_CHAR_50+1];//����ƫ����;
			COORD3D vPlaneNormal;//face_norm_offset[3];	ƽ���淨��
		}RODNODE;
		struct{
			THANDLE hDatumLine;
			char cMirTransType;	//0,'X','Y','Z'
			short fix_axis_val_style_x0_y1_z2;
		}DATUMLINE;
		struct{
			THANDLE hDatumPlane;
			char cMirTransType;	//0,'X','Y','Z'
			short fix_axis_style_x0_y1_z2;
		}DATUMPLANE;
		struct{
			THANDLE hLinePart;	//��׼�˼����
			THANDLE hStartNode;	//ʼ�˽ڵ���
			THANDLE hEndNode;	//�ն˽ڵ���
			double start_offset_dist;	//ʼ��ƫ����
			double end_offset_dist;		//�ն�ƫ����
			double offset_scale;//ƫ�Ʊ���
			double offset_dist;	//�ڵȷֻ����ϵ�ƫ��ֵ
		}SCALE_NODE; //���ڵ��ı����ȷֵ�,������Ŀǰ��������嶨λ wht 11-01-23
		struct tagCOLUMN_INTERS{
			THANDLE hLineTube;		//>0ʱ�Ըøֹܶ��������axis_vec,axis_pick,column_r��
			//�Ե͵���ָ��λΪ1��ʾ��Ӧʸ�����Ϊ�����������
			//��1λ:axis_vec;
			//��2λ:sect_center;
			//��3λ:sect_norm;
			BYTE cPointAndVectorFlag;
			union{
				COORD3D axis_vec;		//���������������
				long hVector;
			}AxisVector;
			union{
				COORD3D sect_center;	//�������������Ľ��㣬��Բ���棨Բ����Բ����Բ�ĵ�
				long hPoint;
			}SectCenter;
			struct{
				long hNode;
				double len_offset;	//��hNode�ڵ���hLineTubeʼ->��(hNode>0,��hNode<0ʱ��->ʼ)���������ƫ����
			}SectCenterParam;
			union{
				COORD3D sect_norm;	//���淨��Ϊ��ʱ��ʾΪ����Ĵ�ֱ����
				long hVector;
			}SectNorm;
			//0.ray_vecΪֱ�Ӷ�������߾��Է���;
			//1.ray_vecΪ�û�ָ���Ļ�����ת�Ƕ���ʼ��
			//2.hLineTube>0ʱ���ֹܻ�׼��Ϊ������ʼ��
			//3.hLineTube>0ʱ���ֹܺ�����Ϊ������ʼ��
			//4.hLineTube>0ʱ���ֹܽ����ϵ�ˮƽ���߷���
			BYTE ray_vec_style;		//0.ray_vecΪֱ�Ӷ�������߾��Է���;>0:ָ��������ʼ�ߣ�������ray_angleȷ�����߷���
			union{
				COORD3D ray_vec;		//���߷���
				long hVector;
			}RayVector;
			double ray_angle;		//ray_vec_style==1ʱ���߷�����������ʼ�ߵ���ʱ����ת�Ƕ�,��
			double column_r;		//����뾶
			double ray_para_offset;	//����ƽ���ƶ����루��ֱ���߷������ƽ�����ƶ�,����������������ƶ�Ϊ������һ��Ϊ����
			bool bSpecRo;			//true:����뾶�Ծ�����ͶӰ���Ծ��붨��false:����뾶���������߽������������ƫ��ȷ��
			double ray_offset_r;	//bSpecRo=true:����뾶; bSpecRo=false:���߷�������ƫ����
		public:
			//void SetAxisVector(double vx,double vy,double vz);
			//void SetSectNorm(double vx,double vy,double vz);
			//void SetSectCenter(double cx,double cy,double cz);
			//void SetRayVector(double vx,double vy,double vz);
		}COLUMN_INTERS;	// added by wjh-2013.10.13
	};
public:
	void ZeroClear();
	void ToBuffer(BUFFER_IO* pIO);
	void FromBuffer(BUFFER_IO* pIO);
};
class CParaPointLifeObj{
	bool m_bEnabled;
	static long CallAngleIds[2];	//���ټ�¼���ѵ��ýǸֵľ��ֵ
	static bool m_bLivingState;
	static CHashList<bool> hashAngleLockStates;
public:
	static bool LockCall(long hAngle);	//ָ���Ǹ�δ����ʱ����������true,���򷵻�false wjh-2018.1.5
	CParaPointLifeObj();
	~CParaPointLifeObj();
};
class CLDSPoint: public CLDSGeObject {
	GEPOINT pos;	//���շ���λ������
public:
	static const BYTE COLUMN_INTERS_AXIS_VEC	= 1;
	static const BYTE COLUMN_INTERS_SECT_CENTER = 2;
	static const BYTE COLUMN_INTERS_SECT_NORM	= 3;
	static const BYTE COLUMN_INTERS_RAY_VEC		= 4;
	static IModel* m_pTower;
	short iNo;
	char description[MAX_TEMP_CHAR_50+1];
	/*  0.��λ�������Ϣ,ֱ��ָ��1.�Ǹ��������Ϊ��׼;2.�˼��Ͻڵ�Ϊ��׼;
		3.�Ǹ����߽���;4.��׼�߽���;5.��׼�����������겻���;
		6.��׼�����������겻���,7.�ؽǸ���֫����ƫ�Ƶİ������,8.��ڵ�ͬλ,
		9.ָ�������ϵ��������,10.���ڵ������ȷֵ�(�����ͽ�������嶨λ,���ڻ�׼��Ի�������ʾ)
		11.���涨λ��
	*/
	BYTE datum_pos_style;
	long hAgentBlockRef;	//���Զ�����װ��ģ�Ϳռ�ת���������ռ��еĹ���ý�飨�������ã�
	union DES_PARA
	{
		THANDLE hNode;	//��ڵ�ͬλ
		THANDLE hPart;	//ָ�������ϵ��������
		struct{
			THANDLE hDatum1;
			THANDLE hDatum2;
			short wing_offset_style1,wing_offset_style2;	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			long wing_offset_dist1,wing_offset_dist2;
			double near_vector[3];	//���ڷ���ƫ������ֵʱ��Ч
			char cProjectPlane;	//ͶӰ������0.�����棨�������Ϊ�淨�ߣ�;1.X-Zƽ��;2.Y-Zƽ��;3.X-Yƽ��
			char norm_offset[MAX_TEMP_CHAR_50+1];//����ƫ����;
		}AXIS_INTERS;
		struct tagRODEND{
			THANDLE hRod;		//�˼����
			short direction;	//0.��׼�˼�����:ʼ->��;1.��->ʼ
			short offset_wing;  //0.X֫;1.Y֫
			short wing_offset_style;	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			double wing_offset_dist,len_offset_dist;//�Ǹ�֫��ƫ��ֵ;����ƫ��ֵ;
			BOOL bIncOddEffect;			//����ƫ��ֵ��������ͷӰ��
			char norm_offset[MAX_TEMP_CHAR_50+1];			//����ƫ����
			void SetWingOffset(double wing_offset=0,short offset_g_style=4);
		}RODEND;	//�Ժ�Ϊ�������Ӧͳһ����ΪRODEND wjh-2013.11.16
		struct{
			THANDLE hRod;	//�Ǹ־��
			THANDLE hNode;	//��׼�ڵ���
			WORD bFaceOffset;
			short direction;	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
			short offset_wing;//0.X֫;1.Y֫
			short wing_offset_style;	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			double wing_offset_dist,len_offset_dist;//֫��ƫ��ֵ;����ƫ��ֵ;
			char norm_offset[MAX_TEMP_CHAR_50+1];//����ƫ����;
			COORD3D vPlaneNormal;//face_norm_offset[3];	ƽ���淨��
		}RODNODE;
		struct{
			THANDLE hDatumLine;
			char cMirTransType;	//0,'X','Y','Z'
			short fix_axis_val_style_x0_y1_z2;
		}DATUMLINE;
		struct{
			THANDLE hDatumPlane;
			char cMirTransType;	//0,'X','Y','Z'
			short fix_axis_style_x0_y1_z2;
		}DATUMPLANE;
		struct{
			THANDLE hLinePart;	//��׼�˼����
			THANDLE hStartNode;	//ʼ�˽ڵ���
			THANDLE hEndNode;	//�ն˽ڵ���
			double start_offset_dist;	//ʼ��ƫ����
			double end_offset_dist;		//�ն�ƫ����
			double offset_scale;//ƫ�Ʊ���
			double offset_dist;	//�ڵȷֻ����ϵ�ƫ��ֵ
		}SCALE_NODE; //���ڵ��ı����ȷֵ�,������Ŀǰ��������嶨λ wht 11-01-23
		struct tagCOLUMN_INTERS{
			THANDLE hLineTube;		//>0ʱ�Ըøֹܶ��������axis_vec,axis_pick,column_r��
			//�Ե͵���ָ��λΪ1��ʾ��Ӧʸ�����Ϊ�����������
			//��1λ:axis_vec;
			//��2λ:sect_center;
			//��3λ:sect_norm;
			BYTE cPointAndVectorFlag;
			union{
				COORD3D axis_vec;		//���������������
				long hVector;
			}AxisVector;
			union{
				COORD3D sect_center;	//�������������Ľ��㣬��Բ���棨Բ����Բ����Բ�ĵ�
				long hPoint;
			}SectCenter;
			struct{
				long hNode;
				double len_offset;	//��hNode�ڵ���hLineTubeʼ->��(hNode>0,��hNode<0ʱ��->ʼ)���������ƫ����
			}SectCenterParam;
			union{
				COORD3D sect_norm;	//���淨��Ϊ��ʱ��ʾΪ����Ĵ�ֱ����
				long hVector;
			}SectNorm;
			//0.ray_vecΪֱ�Ӷ�������߾��Է���;
			//1.ray_vecΪ�û�ָ���Ļ�����ת�Ƕ���ʼ��
			//2.hLineTube>0ʱ���ֹܻ�׼��Ϊ������ʼ��
			//3.hLineTube>0ʱ���ֹܺ�����Ϊ������ʼ��
			//4.hLineTube>0ʱ���ֹܽ����ϵ�ˮƽ���߷���
			BYTE ray_vec_style;		//0.ray_vecΪֱ�Ӷ�������߾��Է���;>0:ָ��������ʼ�ߣ�������ray_angleȷ�����߷���
			union{
				COORD3D ray_vec;		//���߷���
				long hVector;
			}RayVector;
			double ray_angle;		//ray_vec_style==1ʱ���߷�����������ʼ�ߵ���ʱ����ת�Ƕ�,��
			double column_r;		//����뾶
			double ray_para_offset;	//����ƽ���ƶ����루��ֱ���߷������ƽ�����ƶ�,����������������ƶ�Ϊ������һ��Ϊ����
			bool bSpecRo;			//true:����뾶�Ծ�����ͶӰ���Ծ��붨��false:����뾶���������߽������������ƫ��ȷ��
			double ray_offset_r;	//bSpecRo=true:����뾶; bSpecRo=false:���߷�������ƫ����
		public:
			void SetAxisVector(double vx,double vy,double vz);
			void SetSectNorm(double vx,double vy,double vz);
			void SetSectCenter(double cx,double cy,double cz);
			void SetRayVector(double vx,double vy,double vz);
		}COLUMN_INTERS;	// added by wjh-2013.10.13
	}des_para;
public:
	CLDSPoint();
	//0.ָ�������5.��׼��6.��׼���ϵ�ʱ������Ԥ��ǰ����λ��
	//9.�ڻ�׼�����������ϵ�µ�λ������
	GEPOINT datum_pos;
	f3dPoint Position(){return pos;}
	f3dPoint SetRsltPositionOnly(const double* pxRsltPos);
	f3dPoint SetPosition(const f3dPoint& position);
	f3dPoint SetPosition(double x,double y,double z);
	f3dPoint SetPositionX(double x);
	f3dPoint SetPositionY(double y);
	f3dPoint SetPositionZ(double z);
	bool ReplaceRodEndNode(long hRod,long hOldEndNode,long hNewEndNode);
	bool ReplaceReferenceHandle(CHandleReplacer& replacer,double* pickpos_coord,int idClassType=0);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	virtual bool OffsetPosParam(double offset,const double* vOffsetDirection);
	DECLARE_READONLY_PROPERTY(long,m_hNode);
	DECLARE_READONLY_PROPERTY(long,m_hDatumRod);	//���ڽǸ�(������)��㶨λ���˼��ڵ㶨λʱ���ض�Ӧ�Ķ�λ��׼�˼� wjh-2016.11.8
	DECLARE_READONLY_PROPERTY(GEPOINT,xRsltPosition){return pos;}	//��׼�����յĲ�����������λ��
	BOOL UpdatePos(IModel *pModel=NULL,CLDSArcLift* pLift=NULL,bool bReverseLift=false);
	int MirToPoint(IModel *pModel,CLDSPoint &mirPoint,MIRMSG& mirinfo);	//0.��ʾ�ԳƳɹ���<0��ʾ�Գƴ�������
	BOOL AssertValid(IModel *pModel,char *error_info=NULL);
	virtual bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	virtual bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	void ToParaBuffer(BUFFER_IO* pIO);	//���Ѳ�������λ�еļ��β�����Ϣ��ȡΪPOINT_PARA��ʽд�뻺��
	void FromParaBuffer(BUFFER_IO* pIO);//��POINT_PARA���������ж����������λ�еļ��β�����Ϣ
	void CloneParaTo(CLDSPoint* pPoint);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:��ʾ������Ӽ���������;true:��ʾ����Ӽ��������� wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//�����趨�ɽǸ����(�����������ʽ)�Ļ�׼�����,����offsetWing=0:X֫ƫ��;1:Y֫ƫ��;����Ϊ�������˫֫����ƫ��
	void SetRapidAngleBer(THANDLE hJg,BYTE direction,BYTE wing_x0_y1_both2=0,double offsetDist=0.0,BYTE gStyle=4);
	//void Serialize(CArchive &ar);

	static void InitPropHashtable();						//��ʼ�����Թ�ϣ��
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	UPDATE_PROPLISTITEM_FUNC(CLDSPoint,propHashtable)
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
	static long GetPropID(char* propStr);			//�õ�����ID
};
class CLDSVector
{	//��ͨ������Ʋ���
public:
	static IModel* m_pTower;
	CLDSVector();
	bool ReplaceReferenceHandle(CHandleReplacer& replacer);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	void CleanInvalidObjRef(IModel *pModel=NULL);
	BOOL UpdateVector(IModel *pTower);
	BOOL AssertValid(IModel *pTower,char *error_info=NULL);
	long hAgentBlockRef;	//���Զ��巽���װ��ģ�Ϳռ�ת���������ռ��еĹ���ý�飨�������ã�
	GEPOINT vector,nearVector;	//ʵ��������ķ���ʸ��
	THANDLE hVicePart;			//�������߻�׼�˼�
	THANDLE hCrossPart;			//����˼�
	//0.ָ��;1.�Ǹ�֫����;2.�˼������淨��3.������������4.��׼�����ϵ���Է���5.��׼�˼��ϵ���������
	short norm_style;
	union{
		short norm_wing;	//0.X֫;1.Y֫�����Ի�׼�����ǽǸ�ʱ��Ч��
		BYTE reside_plane;	//0.���ڸְ����(0��ʾ����;1.��ʾ��һ��������;2.��ʾ�ڶ���������) wjh-2015.2.7
	};
	//norm_style==1��3:	0.����;1.���� 
	//norm_style==5:	0.ʼ->��      1.��->ʼ 
	//					2.+�Ǹ�norm_x(�Ի�׼��ƽ�淨���Ƹֹ���ʱ����תrot_ang�ǶȺ�ķ���
	//					  ��ֹ������߷���+
	//					  �۸�֫���߷���(X�᷽��)+)
	//					3.-�Ǹ�norm_x(�Ժ�����ƽ�淨���Ƹֹ���ʱ����תrot_ang�ǶȺ�ķ���
	//					  ��ֹ������߷���-
	//					  �۸�֫���߷���(X�᷽��)-)
	//					4.+�Ǹ�norm_y (�ֹ�ʼ���и�ƽ�淨�ߡ��۸ֲ��淨�߷���(Y�᷽��)+)
	//					����.-�Ǹ�norm_y(�ֹ��ն��и�ƽ�淨�ߡ��۸ֲ��淨�߷���(Y�᷽��)-)
	short direction;
	double rot_ang;		//�Ƹֹ���ʱ����תָ���ĽǶȡ�(��һ��ָ��ʱ���ǽǶ��ƣ��ʴ˴��洢�Ƕȵ�λ��)
	MIRMSG mirmsg;
	void ToParaBuffer(BUFFER_IO* pIO);	//���Ѳ�����ʸ���еļ��β�����Ϣ��ȡΪVECTOR_PARA��ʽд�뻺��
	void FromParaBuffer(BUFFER_IO* pIO);//��VECTOR_PARA���������ж��������ʸ���еļ��β�����Ϣ
	bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:��ʾ������Ӽ���������;true:��ʾ����Ӽ��������� wjh-2017.12.10
	//�����ܽ�����CLDSVector::AddToConsoleObjBuf��Ϊvirtual function ��������ط�һ��ֱ����'='����CLDSVector��ֵ�����ᵼ�»��Һ��쳣��wjh-2017.12.11
	void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//void Serialize(CArchive &ar);
	bool ReverseDirection(bool bUpdateVector=true);	//��ת���ⷽ��,���ڸֹ��ϻ�׼��ת����ʱ����false
	void MirToVector(IModel *pModel,CLDSVector &mirVector,MIRMSG& mirinfo);
	char *GetDirectionDes();
	char *GetDirectionCmbItems();
	char *GetDirectionHelpStr();
	char *GetNormStyleDes();
	static char *GetNormStyleCmbItems();

	static void InitPropHashtable();						//��ʼ�����Թ�ϣ��
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	UPDATE_PROPLISTITEM_FUNC(CLDSVector,propHashtable)		//������������º���
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
	static long GetPropID(char* propStr);			//�õ�����ID
};
//�ֽṹ�еı�ʾʵ�ʹ������͸֣���ļ���˨�������ڸ���ʵ�ʹ����Ľڵ�
struct SOLIDMODEL_DISPLAY_PARA{
	BOOL bDisplayHole;
	double scale_of_user2scr;
	double sample_len;
	int smoothness;
};
struct HOLE_WALL{
	bool is_visible;
	DWORD hBolt;
	CLDSBolt* pBolt;	//���ΪhBolt����˨����ָ��
	GEPOINT vStart,vEnd;
	CSolidBody *pSolidBody;
	struct HOLEITEM{
		GEPOINT ptS,ptE;
		long hRelaPart;
	}items[4];
	void SetKey(XHUINT64 key);
	HOLE_WALL();
	~HOLE_WALL();
public:
	BOOL UpdateSolid();
	BOOL Create3dSolidModel(double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	CSolidPart GetSolidPartObject();
};
struct IPartLibrary;
class CLsRef;
class CLsRefList;
//����Լ��
struct CONSTRAINT{
protected:
	BYTE synchronize;	//ͬ��״̬
	UINT id;			//����Լ���ı�ʶId
	//-1:��ʾ�����ع�����Ӧ���������仯�����ܿع���Ӧ��������Ӧ���;0��ʾA=M;
	//>0:������ֵ��ʾ���Ʊ��ʽ�ַ����ı�ʶId
	long idExpression;
public:
	WORD wiAcceptPropId;//�����ܿص����Ի����
	WORD wiMasterPropId;//���ع����Ŀ������Ի����
	long hAcceptObj;	//�����ܿع�������
	long hMasterObj;	//���ع�������
	READONLY_PROPERTY(UINT,Id);	//�Ѵ�Master->Accept������ͬ����������ѭ��
	GET(Id){return id;}
	DECLARE_PROPERTY(CXhChar50,expression);
	void ClearSynchronize(){synchronize&=0x01;}
	DECLARE_PROPERTY(bool,dual);	//˫��ͬ��
	DECLARE_PROPERTY(bool,gone);	//�Ѵ�Master->Accept������ͬ����������ѭ��
	DECLARE_PROPERTY(bool,came);	//˫��ͬ���Ѵ�Accept->Master������ͬ����������ѭ��
	bool IsOriginator(long hObj);	//hObj��ΪΪ��ǰԼ������ķ���
	void CopyProperty(CONSTRAINT* pCopyFrom);
public:
	static CHashList<CXhChar50> hashExpressions;
	void SetKey(DWORD keyid){id=keyid;}
	long DetachExprStr();
	CONSTRAINT();
	~CONSTRAINT();
	struct ACCEPTOR{ WORD wiAcceptPropId;CLDSDbObject* pAcceptObj;};
	bool Execute(IModel* pModel=NULL,CLDSDbObject* pMasterObj=NULL,CLDSDbObject* pAcceptObj=NULL,ACCEPTOR* lpAcceptor=NULL);
};
class CLDSPart : public CLDSDbObject
{
protected:
	bool is_data_modified;
	bool is_solid_modified;
	char *m_pMotherBodySolidBuf;	//�����жϵ�ǰӰ�ӹ���ʵ�������Ƿ���ĸ����ʵ�������Ƿ�汾ƥ��һ��
	bool _bVirtualPart;
	bool _bLiveNeedSupplyPart;	//�����ֳ���Ҫ���ϵĹ��������ܹ������У����ƣ���һЩ�ֽ����������ֳ��������ϣ� wjh-2016.9.4
protected:	//�빫�����Զ�Ӧ���ڴ�洢����
	char _material;
	char _cQualityLevel;
	CXhChar16 sPartNo;//�������
public:
	struct HOLE{
		BYTE cFlag;				//������˨�׼ӹ����յ�������Ϣ�ı�ʶϸ�ڣ����->�ߵ�1λΪ0x00��ʾ��ף�Ϊ0x01��ʾ���
		BYTE ciResidePlane;		//�Ǹ�:0.X֫;1.Y֫; �ְ�0.����;1.��һ������;2.�ڶ�������
		short wWaistLen;		//��Բ������������������װ������ϵ��X��ͬ��
		short wBoltD;			//������˨ֱ��
		float fHoleMargin;		//������˨����ܼ�϶�����׾�����ֵ
		CLDSBolt* pBolt;		//������˨����
		GEPOINT xLocalPosition;	//�����������ֲ�����ϵ�µĿ�λ��
		GEPOINT xMcsPosition;	//��ȫ�ֿ��������ϵ�µĿ�λ�ã�ģ������ϵ��
		GEPOINT vHoleAxisVec;	//�׷��ߣ�һ��Ϊ�����ڹ����������߷���ģ������ϵ��
		HOLE(CLsRef* pLsRef=NULL);
	public:	//�׾�����
		DECLARE_PROPERTY(float,fHoleD);
	};
protected:
	//ͨ��CLDSPart::HOLE��������˨�ף���������˨������������ף�ֱ�ӿ��������ɵĹ��տף����ϡ�wjh-2017.11.2
	virtual UINT AddMiscellaneousHoles(IXhList<HOLE>* pListHoles);	//��GetHoleSet�ڵ���,����ӳ�������˨����������
	virtual char get_ciLegQuad();
public:
	virtual UINT GetHoleSet(IXhList<HOLE>* pListHoles,bool bOnlyBoltHole=false);	//����ֵΪ������
public:	//��������
	char get_cMaterial();
	char set_cMaterial(char cBriefMaterialMark);
	char get_cQualityLevel();
	char set_cQualityLevel(char qualitylevel);
	bool get_blDefaultQualityLevel();
	bool set_blDefaultQualityLevel(bool blSetTrue);
	BYTE get_cbVirginQualityLevel();
	BYTE set_cbVirginQualityLevel(BYTE qualitylevel);
	__declspec(property(put=set_cMaterial,get=get_cMaterial)) char cMaterial;
	__declspec(property(put=set_cQualityLevel,get=get_cQualityLevel)) char cQualityLevel;
	//Ϊ���ٸļ̳к��ξɴ��룬��ȫ��cQualityLevel��ͬ
	__declspec(property(put=set_cQualityLevel,get=get_cQualityLevel)) char m_cQualityLevel;
	//true,��ʾ����ϱ�����Ӧ���ϵ�Ĭ�������ȼ�������false��ʾ��ָ����Ĭ�������ȼ�
	__declspec(property(put=set_blDefaultQualityLevel,get=get_blDefaultQualityLevel)) bool blDefaultQualityLevel;
	//��������ԭʼ�����ȼ��趨�ĵ��ֽ����ԣ����Ƿ�����ϱ�Ĭ�������ȼ����±仯��
	__declspec(property(put=set_cbVirginQualityLevel,get=get_cbVirginQualityLevel)) BYTE cbVirginQualityLevel;
	PROPERTY(double,Thick); 
	virtual GET(Thick){
		return 0;
	}
	virtual SET(Thick){
		return;
	}
	PROPERTY(bool,m_bVirtualPart);
	GET(m_bVirtualPart){
		if(IsAggregated())
			return true;
		else
			return _bVirtualPart;
	}
	SET(m_bVirtualPart){
		_bVirtualPart=value;
	}
	PROPERTY(bool,m_bLiveNeedSupplyPart);
	VIRTUAL_GET(m_bLiveNeedSupplyPart);
	VIRTUAL_SET(m_bLiveNeedSupplyPart);
	PROPERTY(long,m_hPartWeldParent);
	VIRTUAL_GET(m_hPartWeldParent);
	VIRTUAL_SET(m_hPartWeldParent);
	//����sPartNo
	const char* GetPartNo();
	__declspec( property(get=GetPartNo)) const char* Label;
	virtual void SetPartNo(const char *part_no,bool bIncStdPart=false);
	bool ModifyPartNoSegI(SEGI newseg);	//���¹�������еĶκŲ���
	virtual bool GetPropParam(long idObjProp,PROPVAL* pVal=NULL);
	virtual bool SetPropParam(long idObjProp,PROPVAL& propval);
public:
	static IPartLibrary* library;
	static int CustomerSerial;	//�ͻ����Ʊ�ʶ���
	static bool INTERNAL_TEST_VERSION;
	static BOOL UPDATE_PARTNO_AND_SEG;
	static bool IsLaboratoryFuncEnabled();
	CLDSPart();
	~CLDSPart();
	bool m_bDisplayHole;		//��ʾ�׼��и���
	UCS_STRU ucs;				//���������������ϵ���������ϵ(����װ��������)
	unsigned long  m_uStatMatCoef;		//ͳ��ϵ��
	CSolidBody *pSolidBody;
	long _hPartWeldParent;		//���Ӹ���������Ϊm_hPartWeldParent������Ϊ�˵��Է��� wjh-2017.11.16
	bool m_bUserSpecColor;		//�û�ָ��������ɫ
	COLORREF crMaterial;		//������ɫ
public:	//Ӱ�ӹ�����������Ժͷ���
	//ancestor=trueʱ����ʼ�漶���������򷵻�ֱ�Ӹ���������Գ�ʱ��
	CLDSPart* MotherPart(bool ancestor=true);
	virtual bool IsBelongTo(int iClassTypeId)const;
	virtual CLDSPart* NewCopy(bool bByShadowMode=false);
	virtual bool ShadowDesign();	//����m_hAcs��Ӧ��װ������ϵ����ucs
	virtual void Offset(const double* offset);
	virtual bool IsRelaUsed(){return false;}
	virtual bool IsExternalPart() {return false;}
	//��ȡ��˨�����Ǹ���Ƥ�Ľ���λ�� wjh-2015.4.8
	static const int DATUM_PLANE	= 0;	//������׼ƽ��
	static const int BOLTBTM_PLANE	= 1;	//�������˨Z(��)���ͨ�����
	virtual bool GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* cReserved=NULL,int iIntersPlane=0){return false;}
	//GetBoltIntersPos����˳�򲻺���(��Ϊ��ѡ�������Ӧ�������)�����ڴ���ά����ȫ�����Ժ�Ӧ����CalBoltIntersPosȡ�� wjh-2016.5.7
	virtual bool CalBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int iIntersPlane=0,int* cReserved=NULL){
		return GetBoltIntersPos(pBolt,inters_coords,cReserved,iIntersPlane);
	}
	//virtual void AddToConsoleObjBuf();
protected:	//�빫�����Զ�Ӧ���ڴ�洢����
	BYTE _cFuncType;
public:
	static const BYTE FUNC_COMMON		= 0;	//��ͨ����
	static const BYTE FUNC_FOOTNAIL		= 1;	//�Ŷ����۸֡��Ǹֱ�ʾ�Ŷ��������ְ��ʾ�Ŷ���;�ĸְ�
	static const BYTE FUNC_REINFORCE	= 2;	//��(ǿ)�̣��Ǹֱ�ʾ�Ӿ��̽Ǹ֣��ְ��ʾ��ǿ�߰�
	static const BYTE FUNC_WIREPLATE	= 3;	//���߰�
	static const BYTE FUNC_STEPPLATE	= 4;	//�����ϵ�̤��
	static const BYTE FUNC_SETUPPLATE	= 5;	//������װ�Ұ�
	static const BYTE FUNC_LEGANGLE		= 6;	//���ȽǸ�
	static const BYTE FUNC_FOOTPLATE	= 7;	//���Ű�
	static const BYTE FUNC_GASKET		= 8;	//���
	static const BYTE FUNC_BODYANGLE	= 9;	//�������ĽǸ�
	static const BYTE FUNC_HEADANGLE	= 10;	//ͷ�����ĽǸ�
	PROPERTY(BYTE,m_cFuncType);
	VIRTUAL_GET(m_cFuncType){return _cFuncType;}
	VIRTUAL_SET(m_cFuncType){_cFuncType=value;}
	virtual BYTE FuncType(){return m_cFuncType;}
	virtual BYTE FilterFuncType(){return m_cFuncType;}	//���˴��׽Ŷ�Ϊ�Ŷ�
public:
	void CloneBodyFrom(CSolidBody *pBody);//��ָ��ʵ���¡ʵ������
	virtual UCS_STRU GetUnifiedACS(){return ucs;}
#ifdef __LDS_CONTEXT_
	virtual bool CloneToBomPart(BOMPART* pBomPart);
#endif
	virtual CSolidPart GetSolidPartObject();
	//itemType--�������� -1��ʾ�����øò��� cMaterial--����
	static COLORREF GetColor(int idClassType, long itemType,BYTE cMaterial);
	static void GetArcSimuPolyVertex(f3dArcLine* pArcLine, f3dPoint vertex_arr[], int slices);
	static void GetCircleSimuPolyVertex(double radius, f3dPoint vertex_arr[],int slices);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	virtual BOOL IsHasFootNail();
	virtual int GetLsCount(short d=0);
	virtual CLsRef* FindLsByHandle(THANDLE h){return NULL;}
	virtual CLsRefList *GetLsRefList(){return NULL;}
	virtual bool UpdateHoleWall(HOLE_WALL* pHoleWall){return false;}
	virtual int GatherHoleWalls(CHash64ListEx<HOLE_WALL>& hashHoles){return 0;}
	//���ݹ�������˨�׵��Ⱥ�˳������˨���->��������ʶ(��1Ϊ����)ӳ�䣬������˨������ wjh-2016.1.05
	virtual long DispatchHoleIndexId(CHashList<long>* hashHoleIndexIdByBoltHandle);
	virtual CLsRef* AppendLsRef(CLsRef &ls_ref,BOOL bNearLsCheck=TRUE);
	virtual BOOL DeleteLsRef(THANDLE hBolt);
	virtual BOOL ResetSonBoltRayNo();
	virtual bool IsEqual(CLDSPart *pPart, BOOL bCompareBoltInfo,double epsilon=1.0,char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);
	struct PEER_BOLT{
		BYTE ciState;	//0.��λ���С��ͬ;1.��λ��ͬ;2.�׾���ͬ
		long hBaseBolt;	//��׼��˨���
		long hPeerBolt;	//�Ե���˨���
		double difference;	//����˨���ֵ��ciState=2ʱ����ʾ�Ե���˨�׾����׼��˨�׾���Ĳ�ֵ
		void SetKey(DWORD key){hBaseBolt=key;}
		PEER_BOLT(long hKeyBolt=0){
			hBaseBolt=hKeyBolt;
			difference=hPeerBolt=ciState=0;
		}
	};
	class PEER_PART{
	public:
		BYTE ciEqualState;	//0.��ȫһ�¿ɹ��ջ���;1.
		BYTE cbDetailFlag;	//0.������κ�ϸ��;0x01:���hashBoltsByBase; 0x02:���hashBoltsByPeer;
		CLDSPart* pPart;
		long hPeerPart;		//��鹹���ľ��
		DWORD m_hBlock;
		UCS_STRU csBase;	//��׼�����Ա�һ�»����ʱ�ľֲ���װ�䣩����ϵ
		UCS_STRU csPeer;	//��鹹���Ա�һ�»����ʱ�ľֲ���װ�䣩����ϵ
		CHashListEx<PEER_BOLT> hashBoltsByBase,hashBoltsByPeer;
	public:
		void SetKey(DWORD key){hPeerPart=key;}
		PEER_PART(){hPeerPart=cbDetailFlag=ciEqualState=0;m_hBlock=0;pPart=NULL;}
		void CopyFrom(PEER_PART& peerpart); 
	};
	virtual bool CompareLabelPart(CLDSPart *pPeerPart,MIRMSG* pMirmsg=NULL,BOOL bCmpls=TRUE,PEER_PART *pPeerInfo=NULL,double epsilon=1.0){return false;}
#endif
	virtual char* GetPartTypeName();
	virtual int GetSpecification(char *spec,BOOL bIncMat=TRUE){return 0;}
	//bAnalysisMode=trueʱ����size.idClassType�жϣ��������GetClassTypeId�ж�
	bool IsAngle(BOOL bAnalysisMode=false);
	bool IsTube(BOOL bAnalysisMode=false);	//�򻯹��������ж� wht 11-05-19
	virtual bool IsBolt(){return false;}
	virtual double GetWidth(){return 0;}
	virtual double GetThick(){return 0;}
	virtual double GetThick2(){return 0;}
	virtual double GetHeight(){return 0;}
	virtual double GetLength(){return 0;}
	virtual double GetWeight(BOOL bSolidLength=TRUE){return 0;}
	virtual BOOL Create3dSolidModel(SOLIDMODEL_DISPLAY_PARA displayPara);	//�˺���Ŀ���Ǽ������º����Ĳ����������⣬��δ���� wjh-2011.7.18
	virtual BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	//��ǰ�����Ƿ������ָ����ͼ����ϵ(��Z��Ϊ���߷���)������ס��һ����
	virtual bool IsCanHide(CLDSPart* pPart,const double* dcs_norm){return false;}
	BOOL CreateShadowPart3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0){;}
	virtual void ToXmlFile(FILE* fp,DWORD schema){;}
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0){;}
	virtual bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	virtual long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	virtual void UpdatePartNoBySegI();
	virtual void SetSolidModified(BOOL bRenderSolid=TRUE){is_solid_modified=(bRenderSolid!=0);}
	virtual void SetModified(BOOL bTrue=TRUE,BOOL bRenderSolid=TRUE);
	BOOL IsModified(){return is_data_modified;}
	BOOL IsSolidModified(){return is_solid_modified;}
	//���ڸ˼�0.��ȫ�Գ�һ��;1.��ʵ��������Գ�һ��;2.ʰȡ�㴦����ͬ��Գ�;3.ʰȡ�㴦��������Գ�
	virtual BYTE MirSimilarGradeWith(CLDSPart* pPart,const MIRMSG& msg,f3dPoint *pPickPos=NULL);
	//pPickPosΪָ���Գ�ʰȡ��,��Ҫ���ڸ˼�ʰȡ�㲿�ֶԳ�,��ʼ�ն˽ڵ�ĸ˼�Ӧ���뵥��ģ������,����Ӧ����ʵ������ wjh-2011.8.9
	//bCheckProfile�ڻ�ȡ�ԳƸְ�ʱ���������Ƿ�������αȶ� wht 11-07-22
	virtual CLDSPart *GetMirPart(MIRMSG mirmsg,const double *pPickPos=NULL,BOOL bCheckProfile=TRUE);
	virtual bool IsAcceptableTypeId(int type_id);
	virtual bool IsPart() const{return true;}
	virtual bool IsNode() const{return false;}
	virtual bool IsLinePart() const{return false;}
	virtual bool IsArcPart() const{return false;}
	virtual bool IsPolyPart() const{return false;}
	virtual bool IsPlate() const{return false;}
	virtual bool IsExternalPart() const{return false;}
	virtual bool IntelliRecogWeldLine(){return false;}
	virtual bool IsWeldWith(double* lineStartCoord,double* lineEndCoord){return false;}
	virtual bool IsWeldWith(f3dArcLine& arcline){return false;}
	virtual int ToPPIBuffer(CBuffer &buffer,DWORD dwInheritFlag=0);	//�õ�����������Ϣ�����ļ�buffer
	virtual int WriteProcessBuffer(CBuffer &buffer,bool bSaveSubSerial=false,long version=0){ return 0;}		//�õ�����������Ϣbuffer
	virtual void ResetRelativeObjs();	//�������ù�������
	//���Թ���
	/*static UINT crShortAngle,crAngleEdge,crPole,crAngleWingX,crAngleWingY,crBolt12,crBolt16,crBolt20,
		crBolt24,crBolt27,crBolt30,crBolt36,crBolt39,crBolt42,crBolt48,
		crOtherBolt,crPlate,crCutFaceOrPolyJoint,crStdPart;*/
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	static void InitPartPropHashtable();
	UPDATE_PROPLISTITEM_FUNC(CLDSPart,propHashtable) //������������º���
	static int GetPropStr(long id,char *propStr);		//�õ������ַ���
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
};
typedef CLDSPart* CLDSPartPtr;
class CPartTempUcsLife{
	UCS_STRU oldUCS;
	CLDSPart *m_pPart;
public:
	CPartTempUcsLife(CLDSPart *pPart,UCS_STRU temp_ucs)
	{
		m_pPart=pPart;
		if(m_pPart)
		{
			oldUCS=m_pPart->ucs;
			m_pPart->ucs=temp_ucs;
		}
	}
	~CPartTempUcsLife()
	{
		if(m_pPart)
			m_pPart->ucs=oldUCS;
	}
};
//���ӹ�������,ֻ���ڲ���ͳ��
class CSupplementPart : public CLDSPart {
public:
	CXhChar16 guige;	//���
	double length;		//����
	double piece_weight;//������
	int nQuantity;		//��������
	int part_type;		//��������:0.�Ǹ�;1.�ְ�2.����
	BOOL bLegPart;		//�Ȳ�������ͳ��ʱ���������ԣ�ֻ��һ�������н���ͳ�ƣ�;
	CSupplementPart();
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//void Serialize(CArchive &ar);
};
// ��˨
struct DIANQUAN_STRU{
	short _wholethick;//��Ȧ��(���Ƿ��Զ�ƥ���Ȧ��ȵ���Ϣ)
public:
	static const int DIANBAN  = 0;
	static const int DIANQUAN = 1;
	DECLARE_PROPERTY(bool,AutoMatchThick);	//���������˨ʱ�Ƿ��Զ�������˨ֱ��ƥ���Ȧ���������
	short get_thick();
	short set_thick(short _thickvalue);
	//��Ȧ��Ȼ�ȡ������,thick<256;
	__declspec(property(put=set_thick,get=get_thick)) short thick;
	short N;	//��Ȧ����
	double offset;	//����˨�����������ƫ����
};
struct DIANBAN_STRU{
	BOOL bOriginal;		//�Ƿ�Ϊ����
	long hDianBan;		//���
	short feature;		//����
};
class CLDSLine : public CLDSGeObject {
public:
	CLDSLine();
	/*BYTE m_cDefineType;	//0.ָ����׼�߶˵�;1.ָ���˼�������(�ֹ����߻�Ǹ�������offset_g���ʹ��)2.�Ǹ�X֫����3.�Ǹ�Y֫����
	union LINE_PARA{
		THANDLE hPointArr[2];
		CLDSPoint arrPoint[2];
		struct KEY_LINE{
			long m_hRod;
			long offset_nx;	//m_cDefineType��1��ָ���˼�Ϊ�Ǹ�ʱ����ʾ��X֫����ƫ����
			long offset_ny;	//m_cDefineType��1��ָ���˼�Ϊ�Ǹ�ʱ����ʾ��Y֫����ƫ����
		}keyline;
		struct WING_LINE{
			long m_hAngle;
			long offset_n;	//m_cDefineType��2��ָ���˼�Ϊ�Ǹ�ʱ����ʾ��Y֫ƫ����
			long offset_g;	//m_cDefineType��2��ָ���˼�Ϊ�Ǹ�ʱ����ʾ��X֫ƫ����
		}wingline;
	}des_para;*/
	HANDLEPOINT hPointArr[2];
	long  iNo;
	char description[MAX_TEMP_CHAR_100+1];	//�Ի�׼�ߵ���������
	GEPOINT start_pos,end_pos;
	BOOL UpdateLine(IModel *pTower);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:��ʾ������Ӽ���������;true:��ʾ����Ӽ��������� wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	bool ReplaceReferenceHandle(CHandleReplacer& replacer);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	void CleanInvalidObjRef(IModel *pModel=NULL);
	bool IsSameWith(const double* pxLineOrg,const double* pvLineVec);
	//void Serialize(CArchive &ar);
};
class CLDSPlane : public CLDSGeObject {
	GEPOINT base_pos,work_norm;
public:
	CLDSPlane();
	long  iNo;
	char description[MAX_TEMP_CHAR_100+1];	//�Ի�׼�����������
	HANDLEPOINT  hPointArr[3];	//���λΪ0x80000000ʱ��ʾֱ��������ǽڵ���
	BOOL UpdatePlane(IModel *pTower,CLDSArcLift* pLift=NULL,bool bReverseLift=false);
	BOOL IsValidPlane();
	f3dPoint GetWorkNorm();
	f3dPoint GetBasePos();
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	virtual bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:��ʾ������Ӽ���������;true:��ʾ����Ӽ��������� wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	bool IsSameWith(const double* pxPlaneOrg,const double* pvPlaneStdNormal);
	//����ƽ�涨λ����
	GEPOINT get_xPickPoint(){return base_pos;}
	__declspec(property(get=get_xPickPoint)) GEPOINT xPickPoint;
	//����ƽ�淨��
	GEPOINT get_vStdNormal(){return work_norm;}
	__declspec(property(get=get_vStdNormal)) GEPOINT vStdNormal;
	//void Serialize(CArchive &ar);
};
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	//��˨λ����Ʋ�������
enum LSPOS_STYLE{LS_POS_NO_PARA,LS_POS_AXIS_INTERS,LS_POS_JG_BER,LS_POS_NODE_PRJ,LS_POS_LS,LS_POS_NODE_FACEOFFSET,LS_POS_UCS};
LSPOS_STYLE TranslateLsPosStyleFromI(int i);
	//��˨λ����Ʋ���
class DESIGN_LS_POS
{
public:
	CLDSPoint datumPoint;
	THANDLE hPart;	//��λ��׼������������Ծ����Զ�λ���������ƫ�Ʒ�������ͺ����X/Y�ᣩ
	union{
		BYTE offset_wing;	//0.X֫;1.Y֫�����Ի�׼�����ǽǸ�ʱ��Ч��
		BYTE reside_plane;	//0.���ڸְ����(0��ʾ����;1.��ʾ��һ��������;2.��ʾ�ڶ���������) wjh-2015.2.7
	};
	static const BYTE GLOBAL_COORD = 0;
	static const BYTE LOCAL_COORD  = 1;
	BYTE cLocationStyle;	//0:GLOBAL_COORD��������Ϊ��������;1:LOCAL_COORD��������Ϊ��׼�������������
	//union{
		//struct{
			BYTE direction;	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ�����Ի�׼�����ǽǸ�ʱ��Ч��
			BYTE m_biWingOffsetStyle;	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			double wing_offset_dist,len_offset_dist;//֫��ƫ��ֵ��Y��;����ƫ��ֵ��X��;
		//}angle;
		//struct{
			BYTE cStartEdge;		//���ֹܶ�λʱ��,�Ƕ���ʼ�� 0:��׼��;1:������
			double offset_angle;	//��˨��Ϊ���в��� ��ñ�����¼��˨���ڣ���׼�����������ģ���׼�ֹܵ�ƫ�ƽǶȣ��Ըֹܺ����߻��׼��Ϊ��׼��
			double R;	//R<=0��ʾ��˨�ǻ��β��� ��˨Ϊ���β���ʱ �ñ�����¼��˨����Բ�İ뾶
		//}tube;
	//}location;
	CDesignThickPara norm_offset;	//����ƫ�����ַ���
	DESIGN_LS_POS();
	//void Serialize(CArchive &ar);
	void FromBuffer(CBuffer &buffer,long version,long doc_type);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
};
class CLDSBolt;
class CLDSLineAngle;
//��˨��Ʋ���
class CDesignLsPara
{
public:
	long iNo;
  	long d;	     			// ��˨ֱ��.
	SEGI iSeg;				// ��˨�κ�
	char grade[5];			// ��˨����
  	CParaThick L0;	// ��˨ͨ�����.
	double hole_d_increment;//��˨��ֱ������
	bool m_bVirtualPart;	//��������˨
	DWORD dwRayNo;			// ���ڵ����������ţ���ͼʱ��
	CLDSVector des_work_norm;	//��˨���������������
	DESIGN_LS_POS des_base_pos;	//��˨��׼���ʼ����(�����������ƶ�)�������
	CDesignThickPara pad_thick;	//��˨��(��Ȧ����)
	CDesignThickPara pad_offset;//��˨���Ի���ƫ����
	CLDSBolt *pMapLs;			// ��ʱ��,���洢
	int nLjJg;				// ��ʱ��,���洢,���Ա���������˨�����˼����Ǹ�
	CDesignLsPara();
	bool DesignLs(CLDSBolt *pLs);
	void FromBuffer(CBuffer &buffer);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void CloneTo(CDesignLsPara &ls_para);
};
//��˨�����Ʋ���
class CDesignPadPlankPara{
public:
	long iNo;
	char sPartNo[MAX_CHAR_GUIGE_16+1];// ��幹�����
  	char ls_str[MAX_TEMP_CHAR_200+1];// ��˨��Χ�ַ���
	long hBaseJg;			// ������׼�Ǹ�
	long hCutRefJg;			// �н�ʱ�Ļ�׼�Ǹ�,һ�������ӻ�׼�Ǹ�
  	CDesignThickPara thick;	// ��������
	CDesignPadPlankPara(){memset(this,0,sizeof(CDesignPadPlankPara));}
	void FromBuffer(CBuffer &buffer,long version=NULL);
	void ToBuffer(CBuffer &buffer);
	void ToXmlFile(FILE* fp,DWORD schema);
	//void Serialize(CArchive &ar);
};
struct ANGLE_SPACE_FLAG{
	BYTE cbFlag;
	static const BYTE SPACE_UDF	   =0;	//ָ����ֵ�߾�
	static const BYTE SPACE_BOLTEND=1;	//��׼��˨�˾�
	static const BYTE SPACE_TOEDGE =2;	//���쵽��Ե
	static const BYTE SPACE_TOBER  =3;	//���쵽���߱�Ե-֫���

	//static const BYTE SPACE_UDF	  =0;	//ָ����ֵ�߾�
	//static const BYTE SPACE_BOLTEND =1;	//��׼��˨�˾�
	static const BYTE SPACE_ALIGN	  =2;	//��׼��˨�˾�+�����˨��ࣨ������֫�������)
public:
	ANGLE_SPACE_FLAG(BYTE flag=0x05){cbFlag=flag;}	//Ber::SPACE_BOLTEND|Wing::SPACE_BOLTEND
	operator BYTE(){return cbFlag;}
	BYTE StyleOfBerSpace();
	BYTE StyleOfWingSpace();
	BYTE StyleOfEndSpace();
	BYTE SetBerSpaceStyle(BYTE cbSpaceStyle);
	BYTE SetWingSpaceStyle(BYTE cbSpaceStyle);
	BYTE SetEndSpaceStyle(BYTE cbSpaceStyle);
};
class CDesignLjPartPara{
	BYTE m_ciAsynDualEdgeEndSpace;		//0.ͬ��;1.�첽 �Ƿ��Ѿ��趨��˫����˨�˾�ʱ��ʼ�˶˾�,δ�趨ʱ��end_space����
	//end_space�൱��currEndSpace,��ʾ��ǰ���Ӷ˵���˨�˾��˫����˨�˾�ʱ�ĸ˼��ն˲���˨�˾� wjh-2017.11.23
	double dualEdgeStartSideEndSpace;	//˫����˨ʱ�ĸ˼�ʼ�˲���˨�˾�
	BYTE m_ciAsynDualEdgeBerSpace;		//0.ͬ��;1.�첽 �Ƿ��Ѿ��趨��˫����˨�˾�ʱ��ʼ�����߲�߾�,δ�趨ʱ��ber_space����
	//ber_space�൱��currBerSpace,��ʾ��ǰ���Ӷ˵����߲���˨�߾��˫����˨�˾�ʱ�ĸ˼��ն����߲���˨�߾� wjh-2017.11.23
	double dualEdgeStartSideBerSpace;	//˫����˨ʱ�ĸ˼�ʼ�����߲���˨�߾�
	BYTE m_ciAsynDualEdgeWingSpace;		//0.ͬ��;1.�첽 �Ƿ��Ѿ��趨��˫����˨�˾�ʱ��ʼ��֫���߾�,δ�趨ʱ��wing_space����
	//wing_space�൱��currWingSpace,��ʾ��ǰ���Ӷ˵���˨�˾��˫����˨�˾�ʱ�ĸ˼��ն�֫�����˨�߾� wjh-2017.11.23
	double dualEdgeStartSideWingSpace;	//˫����˨ʱ�ĸ˼�ʼ��֫�����˨�߾�
public:
	CDesignLjPartPara();
	void AddLsNo(int iLsNo);
	double CalOddment();
	//void Serialize(CArchive &ar);
public:
	struct DESIGNLJ_ANGLEPARA
	{
		BYTE cur_wing_x0_y1;	//��ǰ����֫
		ANGLE_SPACE_FLAG cbSpaceFlag;		//
		BOOL bEndLjJg;			//�����ӽǸ�
		BOOL bTwoEdge;			//�Ƿ���˨�����඼����������
	}angle;
	BOOL m_bAdjustLjEndPos;		//�ԳƸְ�ʱ�Ƿ���Ҫ�������߸˼����Ӷ˰ڷ�λ�ã���Ҫ�ڶԳƹ��ð�ʱʹ�� wht 10-02-26 
	long m_nClassTypeId;
	//int m_iAffectTubeEndPlateStyle;
	long hPart;//,hRefPart1,hRefPart2;	//��ǰ�ֹܾ��,����ͷУ����չ���
	short start0_end1,iFaceNo;			//���ʼ��0���ն�1;	����������
	double end_space,wing_space,ber_space;	//���������ʱ�Ĳ���
	DECLARE_PROPERTY(double,startSideEndSpace);
	DECLARE_PROPERTY(double,endSideEndSpace);
	DECLARE_PROPERTY(double,startSideBerSpace);
	DECLARE_PROPERTY(double,endSideBerSpace);
	DECLARE_PROPERTY(double,startSideWingSpace);
	DECLARE_PROPERTY(double,endSideWingSpace);
	DECLARE_PROPERTY(BYTE,ciAsynDualEdgeEndSpace);
	DECLARE_PROPERTY(BYTE,ciAsynDualEdgeBerSpace);
	DECLARE_PROPERTY(BYTE,ciAsynDualEdgeWingSpace);
	CLDSLinePart* pCurLinePart;		//��ʱ��,���洢
	void SetKey(DWORD key){hPart=key;}
	//BOOL bBasePart;				//���ӻ�׼�Ǹ�
	//CDesignThickPara norm_offset,datum_norm_offset;	//����ƫ����
	//long nCollideSpace;			//����϶��Ʋ���
	//char ls_str[MAX_TEMP_CHAR_100+1];	//��˨�ַ���
	//short odd_cal_style;		//0.��ʾ������ײ���ո˼�����;1.������˨λ�ü���;2.��ʾ����������ͷ
	//�������Ӹ˼�����
	DECLARE_PROP_FUNC(CDesignLjPartPara);
	int GetPropValueStr(long id, char *valueStr, UINT nMaxStrBufLen = 100);	//��������ID�õ�����ֵ
	static void InitViewPropHashtable();
};

class CLDSPlate;
class CObjNoGroup;
class CDesignLjPara{
public:
	BOOL m_bEnableFlexibleDesign;	//���ò�����������ƣ���Ҫ�����ϰ汾�ļ��������Ϣ�ã�
	long m_hBaseNode,m_hBasePart;	//��׼�ڵ�,��׼�˼�,˫��׼�ڶ���׼�˼�
	short iFaceType;			//�������ͣ�1-3����ʾ��Ӧ����������ͨ�壬4��ʾ�۵��壬5��ʾ��˨���
	short iProfileStyle0123;	//���������巽ʽ,0:��׼����1:��������2:��������3:�Զ�������
	long  iWeldLineStyle;		//0.���ɼ��㣻1.ָ�������ߣ�2:���뺸����
	double weld_start_high,weld_end_high,weld_width;	//��������ʱ�ĺ�����ʼ����ֹ�߼�������
	UINT uiStyleFlag;
	DECLARE_PROPERTY(bool,m_bWeldToDatumJg);		//������Ƿ񺸽ӵ���׼�Ǹ� wht 10-06-06
	//---VVV---������ʱֻΪ��LDS�ļ��趨,�Ժ����Ҫȥ��wjh-08-11-11)--
	//CDesignThickPara thick;		//���(��ʱֻΪ��LDS�ļ��趨,�Ժ����Ҫȥ��wjh-08-11-11)
	//char ls_str[MAX_TEMP_CHAR_200+1];	//��˨�ַ���(��ʱֻΪ��LDS�ļ��趨,�Ժ����Ҫȥ��wjh-08-11-11)
	//ATOM_LIST<CDesignLsPara>  ls_list;
	//ATOM_LIST<CDesignPadPlankPara> padplank_list;
	//CObjNoGroup lsnogroup,padplanknogroup;
	//---^^^-------------------------------------------
	bool m_bSelfCorrectBendline;	//�Զ���λ�����ߣ��Ա�֤������Ϊ���潻�� wjh-2016.5.14
	CLDSPoint origin,huoquline_start[2],huoquline_end[2];
	CLDSVector norm,huoqufacenorm[2];
	CHashListEx<CDesignLjPartPara>partList;
	fPtList additional_vertex;
	CDesignLjPara();
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//void Serialize(CArchive &ar);
	bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	//��¡��Ʋ��� wht 09-12-30
	void CloneTo(CDesignLjPara &designInfo);
	//��������ĸ˼����������Ӧ�����Ӹ˼���Ʋ��� wht 10-01-05
	CDesignLjPartPara *FromHandle(long hPart);
};
class CLDSSphere : public CLDSPart{
public:
	double D;	//����ֱ��(mm)
	double d;	//����ֱ��(mm)
	double thick;	//���� �ñ������洢
	long hDatumNode; //��׼�ڵ���
	CLDSSphere();
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	CLDSPart *GetMirPart(MIRMSG mirmsg,const double *pPickPos=NULL,BOOL bCheckProfile=TRUE);
	CSolidPart GetSolidPartObject();
	BOOL IsModified(){return is_data_modified;}
	void CopyProperty(CLDSDbObject *pObj);
	bool IsEqual(CLDSSphere* pMirSphere,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	//���Թ���
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	UPDATE_PROPLISTITEM_FUNC(CLDSSphere,propHashtable)   //������������º���
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
	static void InitSpherePropHashtable();
	static int GetPropStr(long id,char *propStr);		//�õ������ַ���
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬

	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
};
//////////////////////////////////////////////////////////////////////////////////
class CLsRef;
class CLDSBolt : public CLDSPart {
protected:
  	short _d;	     	// ��˨ֱ��.int->short(19.07.16)       ʡ2 Bytes
  	short _L;           // ��˨��Ч����.double->short(19.07.16)ʡ6 Bytes
	short _L0;			// ��˨ͨ�����.double->short(19.07.16)ʡ6 Bytes
	short _siInitStdL;      //�ָ���Ϊ�Ŷ�����˨��Ч����.double->short(19.07.16)ʡ6 Bytes
	CParaThick designL0;	//��˨ͨ�����Ʋ���
	bool m_bNeedUpdateL0;	//�Ƿ���Ҫ����L0.BOOL->bool(19.07.16)ʡ3 Bytes
	bool m_bCalL0IncGapByInters;	//���������㷨������˨ͨ�񣨰����м��ȱʧ����϶ֵ�� wjh-2019.7.11
	short _hFamily;		//������˨���ϵ��.long->short(19.07.16)ʡ2 Bytes
	char grade[5];		//��˨����
	bool m_bAggregated;	//true:����˨Ϊװ�䲿��Ӱ����˨��ͬʱ�ѱ��ۺϵ����������У�ʵ����ʾʱ������ʾ
	float m_sfHoleDIncrement;	//hole_d_increment��˨��ֱ������double->float(19.07.16)ʡ4 Bytes
public:	//��������
	static bool m_bEnableActiveHeightFilterOnL0InternalCal;	//get_L0()�ڲ�����ͨ�����ʱ�Ƿ����õ�ǰ���������֤��Ĭ��Ϊtrue
	//L0����ģʽ:0.��ʾ��������������ۼӼ���;1.��ʾ���Ǵ���������ֵ����;
	//2.����˨����m_bCalL0IncGapByInters���Լ���
	static const char L0CAL_BY_SUMM_THICK = 0;
	static const char L0CAL_INC_GAP_THICK = 1;
	static const char L0CAL_BY_BOLTSELF	= 2;
	static char BOLTL0_PREFER_MODE;	//��˨Ĭ�ϵ�ͨ�����������ͣ�ֻ��ȡֵ0��1
	static char BOLTL0_CALMODE;		//��˨��ǰͨ��ʵ�ʼ������ȡֵ0~2 wjh-2019.7.23
	//���������㷨������˨ͨ�񣨰����м��ȱʧ����϶ֵ�� wjh-2019.7.11
	bool get_blCalL0IncGapByInters();
	bool set_blCalL0IncGapByInters(bool blVal);
	__declspec(property(put = set_blCalL0IncGapByInters, get = get_blCalL0IncGapByInters)) bool blCalL0IncGapByInters;
	bool m_bDisableSimpleLocateMode;	//�Ƿ���ֹ�������ü򵥶�λ��ʾģʽ
	DECLARE_READONLY_PROPERTY(bool,m_bCanSimpleLocateMode);
	long get_m_hFamily();
	long set_m_hFamily(long value);
	__declspec(property(put = set_m_hFamily, get = get_m_hFamily)) long m_hFamily;
	char* Grade(){
		CLDSBolt* pMotherObj=(CLDSBolt*)MotherObject();
		if(pMotherObj==this)
			return grade;
		else
			return pMotherObj->Grade();
	}
	char* SetGrade(char* boltgrade)
	{
		CLDSBolt* pMotherObj=(CLDSBolt*)MotherObject();
		grade[4]=0;
		strncpy(grade,boltgrade,4);
		if(pMotherObj!=this)
			pMotherObj->SetGrade(grade);
		return grade;
	}
public:
	static const DWORD TWO_CAP;		//˫ñ��˨��ʶλ
	static const DWORD ANTI_THEFT;	//������˨��ʶλ
	static const DWORD ANTI_LOOSE;	//�����ɵ�Ȧ��ʶλ
	static const DWORD FOOT_NAIL;	//�Ŷ���˨��ʶλ
	static const DWORD REVERSE_NORM;//��ת��˨�����ʶλ
	char statSegStr[24];	// ͳ����˨ʱ�Ĺ����κ��ַ�������˨���ܴ��ڶ���ۼ�ͳ�Ƶ������ wjh-2011.9.28
	double get_hole_d_increment()const;
	double set_hole_d_increment(double value);
	//��˨��ֱ������
	__declspec(property(put = set_hole_d_increment,get= get_hole_d_increment)) double hole_d_increment;
	DIANQUAN_STRU DianQuan;	// ��˨��(��Ȧ����)
	DIANBAN_STRU DianBan;	// Ŀǰ��Ϊ��ȡLDS�ļ���
	BITFLAG m_dwFlag;		// ������Ҫ��ı�ʶλ����ANTITHEFT_BOLT(1),ANTILOOSE_BOLT(2),FOOTNAIL_BOLT(4)
	DWORD dwRayNo;			// ���ڵ����������ţ���ͼʱ��
	CLDSVector des_work_norm;	//��˨���������������
	DESIGN_LS_POS des_base_pos;	//��˨��׼���ʼ����(�����������ƶ�)�������
#ifdef __DRAWING_CONTEXT_
	GEPOINT baseRayVec;	//��des_base_pos.hPart��Ӧ�����߷���
	struct RELA_RAYPART{
		THANDLE hRelaPart;//�����������
		DWORD dwRayNo;
		GEPOINT rayVec;
	}relaRayArr[4];	//��Ӧ�����ߺ�
	BOOL SetRelaPartRayNo(long hPart,DWORD dwRayNo,const double* rayVec);
#endif
public:
	double GetWeight(BOOL bSolidLength=TRUE);
	int ValidCheck();
	CLDSBolt(IModel *pTower=NULL);
	~CLDSBolt();
	BITFLAG get_dwSafeFlag();
	__declspec(property(get=get_dwSafeFlag)) BITFLAG dwSafeFlag;	//���m_dwFlag������IsAntiTheft��IsAntiLoose���� wjh-2019.3.28
	virtual bool IsBolt(){return true;}
	static const BYTE FUNC_COMMON	= 0;//0.������˨
	static const BYTE FUNC_FOOTNAIL	= 1;//1.�Ŷ����۸֡��Ǹֱ�ʾ�Ŷ��������ְ��ʾ�Ŷ���;�ĸְ�
	static const BYTE FUNC_WIREHOLE	= 2;//2.���߿�;
	static const BYTE FUNC_EARTHHOLE= 3;//3.�ӵؿ�;
	static const BYTE FUNC_SETUPHOLE= 4;//4.װ���;
	static const BYTE FUNC_BRANDHOLE= 5;//5.���ƿ�;
	static const BYTE FUNC_WATERHOLE= 6;//6.������
	static const BYTE FUNC_FOOTNAIL2= 7;//7.���׽Ŷ�
	static const BYTE FUNC_BASEHOLE = 8;//8.�ؽ���˨��

	virtual BYTE FuncType();
	PROPERTY(BYTE,m_cFuncType);
	VIRTUAL_GET(m_cFuncType){return FuncType();}
	VIRTUAL_SET(m_cFuncType);
	virtual BYTE FilterFuncType();	//���˴��׽Ŷ�Ϊ�Ŷ�
	virtual bool IsRelaUsed();
	virtual bool IsAggregated();
	virtual void SetAggregateState(bool aggregated=true){m_bAggregated=aggregated;}	//���þۺ�״̬
	virtual bool IsBelongTo(int iClassTypeId)const;
	BOOL IsStatSeg(SEGI iSeg);
	int GetStatSegArr(SEGI seg_arr[],int maxSegN);
	//ULONG AddRef();		//����һ������
	//ULONG Release();	//ɾ��һ������
	//ULONG GetRefCounts();
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	f3dPoint set_norm(const double* norm);
	f3dPoint get_norm() const;
	__declspec( property(put=set_norm,get=get_norm)) f3dPoint Normal;
	BOOL correct_pos() ;
	void correct_worknorm();
	BOOL IsLocalCoordLocation(CLDSPart* pDatumPart=NULL) const;	//��˨�Ƿ��Թ������������Ϊ��׼��λ
	BOOL IsCanLocalCoordLocation(CLDSPart* pDatumPart=NULL) const;	//��˨�Ƿ��Թ������������Ϊ��׼��λ
	virtual bool ShadowDesign();			//����m_xDesOrgInfo��m_xMirAxisInfo��normOffset�Զ�����ucs
	short CalL0();
	short get_L0(BOOL bNeedUpdateL0=FALSE);
	CParaThick *GetL0ThickPara();	
	THICK_ITEM GetFirstL0ThickItem(){return designL0.GetFirstItem();}
	THICK_ITEM GetNextL0ThickItem(){return designL0.GetNextItem();}
	//double set_L0(double ls_L0);	//���ԭ����˨ͨ�����,ֱ��������˨ͨ��
	void EmptyL0DesignPara();	//���ԭ����˨ͨ�����
	void GetL0KeyStr(char *key_str,UINT nMaxStrBuffLen=200);//�õ�ͨ���ַ���
	void SetL0KeyStr(const char *key_str);//����ͨ���ַ���
	void SetL0ThickPara(CParaThick thick);//����ͨ���ַ���
	void AddL0Thick(long val,BOOL bHandle=FALSE,BOOL bSamePartFilter=FALSE);//������˨ͨ��
	int set_d(int d);
	int get_d();
	__declspec( property(put=set_d,get=get_d))int d;
	double add_L(double addL);	//ͨ������ֵ
	double set_L(double L);
	double get_L();
	__declspec( property(put=set_L,get=get_L))double L;
	double get_CurrL0() { return get_L0(false); }
	__declspec(property(get = get_CurrL0))double L0;
	double get_dfLe();
	__declspec(property(get = get_dfLe))double dfLe;	//ʵ�ʴ�������ڵ�����ĳ�¶��
	long get_oldL() const;
	long set_oldL(long ls_L);
	__declspec(property(put=set_oldL,get=get_oldL)) long liInitL;	//����Ϊ�Ŷ�ǰ����ͨ��˨����Ч����mm
	bool restore_oldL();
	bool PushInitialStdBoltL();	//����ͨ��˨����Ϊ���׽Ŷ�ʱ���浵��ǰ��ʼ��׼��˨��Ч���� wjh-2019.8.21
	bool PopInitialStdBoltL();
	bool CalGuigeAuto(BOOL bCalShadowBoltGuiGe=FALSE,BOOL bIncGrade=FALSE);
	bool IsFootNail();	//�Ƿ����ڽŶ�
	bool IsTwoCap();	//�Ƿ�Ϊ˫ñ��˨
	bool IsAntiTheft();	//�Ƿ�Ϊ������˨
	bool IsAntiLoose();	//�Ƿ��з��ɵ�Ȧ
	UCS_STRU GetUCS();
	CLDSPart *GetMirPart(MIRMSG mirmsg,const double *pPickPos=NULL,BOOL bCheckProfile=TRUE);
protected:
	BOOL CreateClimbNailSolidModel(fBody* pBody,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
public:
    BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void CalGasketOffset(PARTSET& partset);	//�����Ȧƫ����
	BOOL IsNeedGasket(PARTSET& partset);	//�ж���˨�Ƿ���Ҫ��Ȧ
	int CalPadPara(double dist);
	int CorrectBoltZnL0Para();
	static int CalPadPara(int d,double dist,DIANQUAN_STRU* pDianQuanInfo=NULL);	//0.��� 1.��Ȧ
	static int CorrectBoltZnL0Para(CLDSBolt** ppBolts,UINT count=1);	//�Զ�������˨Z�򴩲�λ�ü�ͨ��
	//blPushLAsInitStdBoltL=trueת����˨Ϊ�Ŷ�֮ǰ����ʼ����ǰ����Ϊ��˨�ı�׼��˨��ʼ��Ч���� wjh-2019.8.21
	void CopyProperty(CLDSBolt *pBolt,bool blPushLAsInitStdBoltL=true);
	CLsRef GetLsRef();
	void CloneTo(CLDSBolt &bolt);
	virtual bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	virtual long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	static CXhChar50 EmendZOffsetStr(CLDSBolt* pBolt,PARTSET& partset);
	static void CreateBoltSolidBody(fBody* pBody,int d,double L,double D,double H);
	static void CreateBoltCapSolidBody(fBody* pBody,int d,double D,double H2);
	//���Թ���
	DECLARE_PROP_FUNC(CLDSBolt);
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ
	static void InitBoltPropHashtable();
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//�õ��������Ե�Ĭ��ֻ��״̬
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//�õ��������Ե�Ĭ��ֻ��״̬
	virtual void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	virtual void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	virtual bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:��ʾ������Ӽ���������;true:��ʾ����Ӽ��������� wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
public:
	//ͬ������
	static MEMORY_BLOCK MPCL;	//Mirror Property Control List
	static PROPLIST_TYPE propMirSync;
	static void InitPropMirBitsMap();
	virtual PROPLIST_TYPE* GetSyncPropList(){return &propMirSync;}
	virtual MEMORY_BLOCK GetDefaultMPCL(){return MPCL;}
	virtual bool SyncMirProp(const char* propKeyStr=NULL,BYTE ciMirAll0X1Y2Z3=0,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ������
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ��λ�öԳ��޹ص�����
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
public:	//��¼��˨�䶯��������๹���ı䶯 wjh-2017.11.22
	static const BYTE CLEAR_BUFF_OF_RELA_PARTS	=0x01;
	static const BYTE DISABLE_RECORD_RELA_PARTS =0x02;
	static BYTE GetMovePartsUpdateFlag();
	static BYTE SetMovePartsUpdateFlag(BYTE cbFlag=0x01);
	static void ClearUpdateMovePartsBuff();
	static CLDSPart* EnumFirstRelaMovePart();
	static CLDSPart* EnumNextRelaMovePart();
};
typedef CLDSBolt* CLDSBoltPtr;
class CLDSGeneralPlate;
class CLDSAnchorBolt : public CLDSBolt
{
public:
	long m_hRelaVirtualBolt;	//��¼ԭʼ�׽Ű��ϵĹ���������˨���
	struct WASHER{
		WORD wiWidth,wiThick;
		float fHoleIncrement;	//���׾���Եؽ���˨ֱ��������ֵ
		long hWasherPlate;		//��������ģ�;��
	}washer;
	struct BASEPLATE{
		WORD wiBasePlateThick;	//���ŵ�������
		long hBasePlate;		//�������ŵ�������(��ֵʱ������wiBasePlateThick������)
	}baseplate;
	struct FOUNDATION{
		BYTE ciHeightSerial,ciLegSerial,ciLegQuad,ciBoltSerial;
		FOUNDATION();
	}Foundation;	//����������Ĺ�ϵ
	WORD get_wBasePlateThick();
	__declspec(property(get=get_wBasePlateThick)) WORD wBasePlateThick;
	BYTE m_ciAnchorType;//�ؽ���˨����;0.��ʾ��ͨ˫ͷ�͵ؽ���˨;1.L��;2.J��;3.��צ��;4.T��
	float m_fNutH;		//��ĸ�߶�
	float m_fNutTaper;	//��ĸ�������Բ׶����¶�h/w;
	float m_fNutDe;	//��ĸ���㵽�������
	float m_fNutDs;	//��ĸ�ߵ��߾���
	float m_fLe;	//��¶��
	float m_fLr;	//��������֮�µ�����(root)
	//ciQuad����:	B(4)  C(3)	ciBoltSerial����(����):	2  1
	//				A(2)  D(1)							3  4
public:
	CLDSAnchorBolt(IModel *pModel=NULL);
	CLDSAnchorBolt(const ANCHOR_BOLT& anchorbolt);
	~CLDSAnchorBolt();
	virtual bool IsBolt(){return true;}
	void SetAnchorInfo(const ANCHOR_BOLT* anchorbolt);
	void InitDefaultAnchorInfo();	//Ϊ����ʾЧ������һЩ�Ǳ�׼�ؽ���˨���в�����ʼ�� wjh-2019.5.9
	void UpdateByBasePlate(CLDSGeneralPlate* pBasePlate);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	//������
	DECLARE_PROP_FUNC(CLDSAnchorBolt);
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//�������ֵ�Ƿ����
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ
public:
	static BOOL CreateAnchorBoltSolidBody(fBody* pBody,const ANCHOR_BOLT* pAnchorBoltPara);
	static BOOL CreateAnchorBoltCapSolidBody(fBody* pBody,const ANCHOR_BOLT* pAnchorBoltPara);
    BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
};
class CLDSBoltU : public CLDSPart{
public:
	bool m_bExternalFittingPart;
	long d;		//��˨��ֱ֫��
	long m;		//U��ֱ��
	long b;		//�ݺ۵ĳ���
	long a;		//��֫�ĳ���
	long R;		//U�Ͱ뾶
	long m_uDisplaySlices;	//U����˨ʵ����ʾʱ����Ƭ����Ĭ��Ϊ6
	CLDSBoltU();
	virtual bool IsExternalPart() { return m_bExternalFittingPart; }
	virtual BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
}; 
typedef CLDSBolt* BOLT_PTR;
class CLsRef;
struct BOLTHOLE_INFO
{
	CLDSBolt* pBolt;
	double bolt_d;
	double hole_d;
	float hole_d_increment;		//��˨��ֱ������
	short waistLen;
	GEPOINT centre,norm,axis_x;
	BYTE cFuncType;		//��˨�׹�������
	DWORD dwRayNo;		//���ڵ����������ţ���ͼʱ��
public:
	PROPERTY(double,posX); 
	virtual GET(posX){return centre.x;}
	virtual SET(posX){centre.x=value;}
	PROPERTY(double,posY); 
	virtual GET(posY){return centre.y;}
	virtual SET(posY){centre.y=value;}
public:
	BOLTHOLE_INFO();
	BOLTHOLE_INFO(const COORD3D& center,CLsRef* pLsRef=NULL);
	BOLTHOLE_INFO(const COORD3D& center, short waistLen = 0, CLDSBolt* pBolt = NULL);
	long BoltHandle() const;
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
};
class CLsRef{
	CLDSBolt* pLs;
	long hAgentBlockRef;	//��˨��װ�䲿���п׵Ĺ�����������
public:
	DWORD dwRayNo;			// ���ڵ����������ţ���ͼʱ��
#ifdef __DRAWING_CONTEXT_
	GEPOINT rayVecArr[32];	//��ͼ������Ĭ�Ϲ̶�����32�����߷���(��������������꣩�Ŀռ�,�Ա�洢��dwRayNo��ƥ������߷��� wjh-2015.2.27
#endif
	// 1��ʾλ�ڽǸ�ʼ�˵���˨��2��ʾλ�ڽǸ��ն˵���˨��0��ʾλ�ڽǸ��м䣨�����������ϣ�����˨
	//ʮ�ֲ���ϵ���˨����	11��ʾX-Yƽ�棺Y���������Ӳ��Ͱ������˨��12��ʾ���Ӱ���������������˨��
	//						21��ʾX-Yƽ�棺Y���������Ӳ��Ͱ������˨��22��ʾ���Ӱ���������������˨��
	//						31��ʾX-Zƽ�棺Z���������Ӳ��Ͱ������˨��32��ʾ���Ӱ���������������˨��
	//						41��ʾX-Zƽ�棺Z���������Ӳ��Ͱ������˨��42��ʾ���Ӱ���������������˨��
	BYTE start1_end2_mid0;		
	static const BYTE PUNCH_HOLE	=0x00;
	static const BYTE DRILL_HOLE	=0x01;	//���
	static const BYTE ENLARGE_HOLE	=0x02;	//����
	BYTE cFlag;				//������˨�׼ӹ����յ�������Ϣ�ı�ʶϸ�ڣ����->�ߵ�1λΪ0x00��ʾ��ף�Ϊ0x01��ʾ���
	short waistLen;			// ��Բ������������������װ������ϵ��X��ͬ��
	long feature;			// ������ʱ��������һ����
	CLsRef();
	CLDSBolt *GetLsPtr(){return pLs;}
	__declspec( property(get=GetLsPtr)) CLDSBolt* pBolt;
	CLsRef& operator = (CLDSBolt* rPtr)
	{
		pLs = rPtr;
		return *this;
	}
	CLDSBolt* operator->()const
	{
		if(!pLs)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Encounter an pointer error";
#else
			throw "ָ������ʧ��";
#endif
		return pLs;
	}
	void FromBuffer(CBuffer &buffer,IModel* pTower,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer);
	void ToXmlFile(FILE* fp,DWORD schema);
};
#endif
#include "lslist.h"
// �ڵ�
class NODE_POS_TO_JG_STRU
{
public:
	NODE_POS_TO_JG_STRU(){memset(this,0,sizeof(NODE_POS_TO_JG_STRU));}
	long offset_X_dist_style;	// X֫0��ʾƫ����Ϊ��׼׼��,1Ϊg1,2Ϊg2,3Ϊg3,4Ϊ�Զ���
	long offset_Y_dist_style;	// Y֫0��ʾƫ����Ϊ��׼׼��,1Ϊg1,2Ϊg2,3Ϊg3,4Ϊ�Զ���
	double offset_X_dist,offset_Y_dist;		// �Զ���ƫ����ʱ�Ի�׼���ƫ����
};
enum REASON_TYPE
{
	REASON_DEL		= 0,
	REASON_MODIFIED	= 1,
	REVERSE_OFFSET  = 2,
	REASON_UNEXPECTED	= 100,
};
struct DESIGN_JGWING_NORM
{	//�Ǹ�֫������Ʋ���
	THANDLE hViceJg;
	GEPOINT near_norm;
	BOOL bSpecific;		//ָ��֫����
	BOOL bByOtherWing;
	CLDSVector spec_norm;
public:
	void ReverseDirection();
	BOOL IsSpecNormActual(){return bSpecific||(!bByOtherWing&&hViceJg==0);}
	BOOL IsByOtherWingActual(){return !bSpecific&&bByOtherWing;}
	BOOL IsByViceRodActual(){return !IsByOtherWingActual()&&!IsSpecNormActual()&&hViceJg>0x20;}
};
class CDesignPlankPara;
typedef CLDSLinePart* ROD_PTR;
typedef CLDSLinePart* CLDSLinePartPtr;

typedef struct tagDESIGN_TYPE
{
	BOOL bIntersBolt;//����˨
	BOOL bAngleJoint;//�Ǹֽ�ͷ
	BOOL bFlJoint;//�Խӷ���
	BOOL bSlopeTransition;	//���»�˫�䵥�ڵ�
	BOOL bBaseFoot;			//�������Žڵ�
	BOOL bL_1LB1;
	BOOL bL_1LB2;
	BOOL bL_1LE1;
	BOOL bL_1LE2;
	BOOL bL_2LB1;
	BOOL bL_2LB2;
	BOOL bL_2LB3;
	BOOL bL_2LB4;
	BOOL bL_2LE1;
	BOOL bL_2LE2;
	BOOL bL_2LC1;
	BOOL bL_2LC2;
	tagDESIGN_TYPE(){memset(this,0,sizeof(tagDESIGN_TYPE));};
}DESIGN_TYPE; 
//���ӵ�ṹ������Ϣ
struct SLOPE_TRANSITION
{	//������´��ǵ�ؼ�����
	bool bBaseFoot;
	CLDSNode* pNode;
	CLDSLinePart* pUpRod;
	CLDSLinePartPtr xarrDownRods[16];	//����������ڱ��µ��²����ĸ˼����ڶ������
	CLDSLinePart* get_pDownRod() { return xarrDownRods[0]; }
	__declspec(property(get = get_pDownRod)) CLDSLinePart* pDownRod;
	CLDSLinePart* pHoriRodX;
	CLDSLinePart* pHoriRodY;
	void ClearDownRods(){memset(xarrDownRods,0,sizeof(CLDSLinePartPtr)*16);}
	SLOPE_TRANSITION(){
		bBaseFoot=false;
		pNode = NULL;
		pUpRod = pHoriRodX = pHoriRodY = NULL;
		//xarrDownRods.ZeroPresetMemory();	//�����麯�����ܶ�����SLOPE����memset wjh-2019.5.26
		//memset(this,0,sizeof(SLOPE_TRANSITION));
	}
};
struct LDSNORMAL {	GEPOINT vLine,vSolid; };
struct CONNECT_NODEINFO;
class CConnectPointInfo
{
public:
	CConnectPointInfo();
	~CConnectPointInfo(){;}
	void Clear();
public:
	CFGWORD cfgword;
	struct SOURCE_CONN_NODEINFO{
		CONNECT_NODEINFO* pSrcNodeInfo;
		char ciConnPlaneIndex;
		SOURCE_CONN_NODEINFO(){pSrcNodeInfo=NULL;ciConnPlaneIndex=0;}
	}source;
	SLOPE_TRANSITION transit;
	bool m_bTransitConnect;	//������´�˫���
	long m_hBaseNode;	//��׼�ڵ�
	long m_hBasePart;	//��׼����
	long m_hBendlineAngle;	//1�Ż�����Ļ����߻�׼�Ǹ�
	long m_hBendlineAngle2;	//2�Ż�����Ļ����߻�׼�Ǹ�
	long m_hJointPart;  //�����ƽ�ͷ�洢��ͷ�˼����
	int m_nMaxRayLsNum;	//���߲���������˨����
	BYTE m_iConnectWing;//0���Ǹ�X֫1���Ǹ�Y֫
	BYTE m_ciBaseRodConnState;//0.��ʼδʶ��;1.�м������;2.�νӶ�ĩ������;3.�νӶνضϵ�����;4.���߽ضϵ�����
	DESIGN_TYPE designType;
	LDSNORMAL datumplane,bendplane,bendplane2;
	GEPOINT vDatumDirect;	//��ֱ�ڻ����߻�׼�˼������׼��ķ���ʸ��(�����л�����ʱ��Ч)
	LINEPARTSET linkPartSet;
	LINEPARTSET bendPlaneRodSet;	//���������߸˼�����
	LINEPARTSET bendPlaneRodSet2;	//2�Ż��������߸˼�����
	int BuildRodSet(CHashSet<CLDSLinePart*>* pHashRayRods);
};
struct CONNECT_PLANE {
	BYTE ciConnectWing;	//0���Ǹ�X֫1���Ǹ�Y֫
	WORD wnMaxRayLsNum;	//���߲���������˨����
	long hBendlineAngle;	//1�Ż�����Ļ����߻�׼�Ǹ�
	long hBendlineAngle2;	//2�Ż�����Ļ����߻�׼�Ǹ�
	DESIGN_TYPE designType;
	GEPOINT vDatumDirect;	//��ֱ�ڻ����߻�׼�˼������׼��ķ���ʸ��(�����л�����ʱ��Ч)
	LDSNORMAL datumplane,bendplane,bendplane2;
	SIMPLE_ARRAY<CLDSLinePart*,PRESET_OBJS16> xarrBasePlaneRods;	//�������Ӹ˼�(���ں�����ֽڵ������õ������ܰ���baserod) wjh-2019.10.8
	SIMPLE_ARRAY<CLDSLinePart*,PRESET_OBJS16> xarrBendPlaneRods;	//1#���������Ӹ˼�
	SIMPLE_ARRAY<CLDSLinePart*,PRESET_OBJS16> xarrElsePlaneRods;	//2#����(����)�����Ӹ˼�
	bool get_blValid() { return xarrBasePlaneRods.Count>0; }
	__declspec(property(get=get_blValid)) bool blValid;
public:
	int BuildRodSet(CHashSet<CLDSLinePart*>* pHashRayRods,long hExcludeBaseRod=0);
};
struct CONNECT_NODEINFO
{
protected:
	short m_siIterSerial;	//����ָ�����
public:
	BYTE cbDisableFlag;	//ĳЩ�������Ҫ��ĳЩ�ر����ӽ������ƣ����������Ѿ�����������ˣ� wjh-2019.10.5
	CONNECT_NODEINFO();
	void Clear();
public:
	//0.��ͨ���Ӱ�;1.���»�˫�䵥�ڵ�;2.�������Žڵ�
	char ciTransitConnType;	//
	SLOPE_TRANSITION transit;	//m_bTransitConnect=trueʱ��Ч
	struct COMMON_NODE_STRUCTURE {
		BYTE ciBaseRodConnState;//0.��ʼδʶ��;1.�м������;2.�νӶ�ĩ������;3.�νӶνضϵ�����;4.���߽ضϵ�����
		long hBaseNode;	//��׼�ڵ�
		long hBaseRod;	//��׼����
		long hJointRod;	//����ʱ��ƽ�ͷ���洢�ԽӸ˼��ľ��
		bool blFlJoint;		//true:��ͷΪ�ֹܷ�������;false:��ͷΪ�Ǹֽ�ͷ
		SIMPLE_ARRAY<CONNECT_PLANE,PRESET_OBJS4> planes;
	}xCommNode;	//��ͨ�ڵ㹹��
	CONNECT_PLANE* AddPlane(int *lpRsltPlaneIndex=NULL);
	CONNECT_PLANE& get_xXlegPlane();
	__declspec(property(get=get_xXlegPlane)) CONNECT_PLANE& xXlegPlane;
	CONNECT_PLANE& get_xYlegPlane();
	__declspec(property(get=get_xYlegPlane)) CONNECT_PLANE& xYlegPlane;
	CONNECT_PLANE& get_xAcrossPlane();
	__declspec(property(get=get_xAcrossPlane)) CONNECT_PLANE& xAcrossPlane;
	static const short CONNPLANE_LEGX	=  0;
	static const short CONNPLANE_LEGY	=  1;
	static const short CONNPLANE_JOINT  = -1;
	static const short CONNPLANE_ACROSS = -2;
	bool GetConnectPointInfo(short siConnPlaneIndex,CConnectPointInfo* pConnPointInfo);
	bool EnumConnPointFirst(CConnectPointInfo* pConnPointInfo);
	bool EnumConnPointNext(CConnectPointInfo* pConnPointInfo);
	bool InitFromConnectPointInfo(CConnectPointInfo* pConnPointInfo,int liToConnPlaneIndex);
};

class CNodeRestrict{
	WORD m_wRestrict;	//��ʮ��λ�ֱ��ʾ�Ƿ���X/Y/Z�����Լ��������޴���Լ��
public:
	THANDLE m_hWorkModule;
	CNodeRestrict(){m_wRestrict=0;m_hWorkModule=0;}
	WORD GetRestrictWord();
	void SetRestrict(WORD wFlag,BOOL bEnable=TRUE);
	void SetRestrictWord(WORD wFlag);
	BOOL IsHasRestrict(WORD wFlag);
	operator WORD(){return m_wRestrict;}
	CXhChar16 ToString(int style=0);	//style=0:����λ���ֱ�ʾ;style=1:����ĸ��ʾ
};
class CNodeLoad{
public:
	THANDLE m_hWorkStatus;	//��������Ӧ�Ĺ������
	f3dPoint windFxy,wireFxy,setupF,wirebreakF;//���������᷽��ļ��пɱ������ֵ(N)
	double iceFz,permanentFz;		//�������ؼ����������׼���������ֵ
	CNodeLoad(){m_hWorkStatus=0;iceFz=permanentFz=0.0;}	//ЧӦϵ��
};

/* ת��Tower.h�ļ���wjh-2014.4.27
//�ڵ㼯�к���
class CExternalNodeLoad
{
public:
	double permanentFz;	//���������׼���������ֵ(N)
	f3dPoint Fxyz;		//���������᷽��ļ��пɱ������ֵ(N)
	CExternalNodeLoad(){permanentFz=0.0;}
};
*/
typedef ATOM_LIST<CNodeRestrict> CNodeRestList;

class CAttachWindLoad : public CLDSDbObject
{
	ATOM_LIST<long>* nodeList;
protected:
	BOOL *m_pModified;
public:
	CAttachWindLoad();
	virtual ~CAttachWindLoad();
	void SetModifiedFlagAddress(BOOL *pModified)
	{	m_pModified=pModified;	}
	void SetModifiedFlag()
	{	if(m_pModified) *m_pModified=TRUE; }

	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);	
	void ToXmlFile(FILE* fp,DWORD schema);
	long handle;
	long iNo;					//��ʱ��֤Ψһ�ı��
	ATOM_LIST<long>* GetNodeList(){return nodeList;}
	char name[MAX_TEMP_CHAR_50+1];
	double m_fShieldArea;		//�������
	double m_fShieldCoef;		//����ϵ��
	double m_fProfileCoef;		//����ϵ��	
	double m_fAttachWeight;		//��������(kg)
	double m_fBetaZ;			//����ϵ��
	double m_fBetaZForFoundation;	//����ϵ��
};

struct XIANLU_STRU
{
	double horizon_space;	//ˮƽ����
	double vertical_space;	//��ֱ����
	double min_vert_space;	//��С��ֱ����
	double horizon_space_allotcoef;	//ˮƽ�������ϵ��
	double vertical_space_allotcoef;//��ֱ�������ϵ��
	double represent_space;	//������
	double HtoL;			//�߲�ϵ��
	double xianlu_angle;	//��λ��(��)
};
struct DIANXIAN_STRU
{
	double m_fDiameter;		// �����⾶,mm
	double m_fArea;			// ���߼�������,mm2
	double m_fUnitWeight;	// ���ߵ�λ������,kg/km
	double m_fForcePermit;	// ���߶������,N
	double m_fSafeCoef;		// ���߰�ȫϵ��
	double m_fMaxUseTension;// �������ʹ������,N
	double m_fElasticCoef;	// ���ߵ���ϵ��,MPa
	double m_fExpansionCoef;// �����¶�������ϵ��,1/1/��C
	char specWireType[50];	// ���ߵ����ʹ���
	char specWire[50];		// ���ߵĹ���ͺ�����
};
class CWireInfo
{
public:
	DIANXIAN_STRU wire;
	XIANLU_STRU xianlu;
	BYTE m_cXianType;	//'E'����Earth Wire,'C'����Conducting Wire,'O'Ϊ���ߵȸ����߹ҵ㣬����Ϊ�ǹ��ߵ�
	BYTE m_cFormula;    //�����������㹫ʽ0��������1б������ 
	BYTE m_nDaoXianFengLieNum;	//���߷�����
	bool m_bNaiZhang;	//true���Ź��ߵ� ,false����ֱ�߹��ߵ�
	int  m_nVoltGrade;	//��ѹ�ȼ�(kv)
	double land_high;	//(m)
	char specAttachment[50];			//��߼���Ե�ӵȸ�������ϸ
	double m_fAttachmentWeight;			//��߼���Ե�ӵȸ���������(kg)
	double m_fAttachmentDiameter;		//��Ե�ӵȸ�������Ȼ��⾶(mm)
	double m_fAttachmentLength;			//��Ե�ӵȸ���������(mm)
	double m_fAchorLineAngle;			//ê�߶Եؼн�
	double m_fTractionLineAngle;		//ǣ���߶Եؼн�
	CWireInfo();
};

struct WEATHER{
	//����Ϊ������¡�ƽ�����¡���硢�������¡�������¡���װ����
	long m_nHighestT,m_nLowestT,m_nAverageT,m_nMaxWindT,m_nMaxIceT,m_nSetupT;
	double m_fMaxIceThick;		//��󸲱����(mm)
	double m_fMaxIceThickCheck;	//���㸲�����(mm)
	double m_fMaxIceWindV;
	double m_fMaxWindV;			//����ٶ�(m/s)
	double m_fMaxWindVCheck;	//�������(m/s)
	double m_fSetupWindV;
	void CopyWeather(WEATHER* pWeather)
	{
		m_fMaxIceThick=pWeather->m_fMaxIceThick;		//��󸲱����(mm)
		m_fMaxIceThickCheck=pWeather->m_fMaxIceThick;	//���㸲�����(mm)
		m_fMaxIceWindV=pWeather->m_fMaxIceWindV;
		m_fMaxWindV=pWeather->m_fMaxWindV;		//����ٶ�(m/s)
		m_fMaxWindVCheck=pWeather->m_fMaxWindV;	//�������(m/s)
		m_fSetupWindV=pWeather->m_fSetupWindV;
		m_nAverageT=pWeather->m_nAverageT;
		m_nHighestT=pWeather->m_nHighestT;
		m_nLowestT=pWeather->m_nLowestT;
		m_nMaxIceT=pWeather->m_nMaxIceT;
		m_nMaxWindT=pWeather->m_nMaxWindT;
		m_nSetupT=pWeather->m_nSetupT;
	}
	void FromBuffer(CBuffer &buffer,long version=NULL);
	void ToBuffer(CBuffer &buffer);	
	void ToXmlFile(FILE* fp,DWORD schema);
};
struct WEATHER_REGION : public WEATHER
{
	char weatherCode[51];
	WEATHER_REGION(){memset(this,0,sizeof(WEATHER_REGION));}
	void FromBuffer(CBuffer &buffer,long version=NULL);
	void ToBuffer(CBuffer &buffer);	
	void ToXmlFile(FILE* fp,DWORD schema);
};
/* ������WireLayoutsEvolvement.h,cpp�С�wjh-2014.5.2
class CWorkEnvLoad
{
public:
	char name[41];					//��Ӧ��������
	double m_fLoadX;				//�����ṩ������������ܺ�(N)
	double m_fLoadY;				//�����ṩ��������ܺ�(N)
	double m_fLoadZ;				//�����ṩ��ֱ�����ܺ�(N)
	double m_fMinVertSpaceLoadX;	//�����ṩ�������(��С��ֱ����)�ܺ�(N)
	double m_fMinVertSpaceLoadY;	//�����ṩ�������(��С��ֱ����)�ܺ�(N)
	double m_fMinVertSpaceLoadZ;	//�����ṩ��ֱ����(��С��ֱ����)�ܺ�(N)
	double m_fWireLoadX;			//���ߵķ����(N)
	double m_fWireLoadZ;			//���ߵĴ�ֱ����(N)
	double m_fMinVertSpaceWireLoadZ;//��С��ֱ����ʱ���ߵĴ�ֱ����(N)
	double m_fAttachmentLoadX;		//�����������(������Ӱ��)(N)
	double m_fAttachmentLoadZ;		//��������ֱ����(������Ӱ��)(N)
	double m_fSetupGa;				//��װʱ����ĸ��Ӵ�ֱ����(N)
	double huchui;					//m,����
	CWorkEnvLoad();
	void SumCal();	//�����ַֺ��ػ��ܳ��ܺ���
};					//�����ṩ��ԭʼ��������
enum WORKENVLOAD_STATUS{
	WORKENVLOAD_WIND=1,WORKENVLOAD_ICE,WORKENVLOAD_LOWTEMP,WORKENVLOAD_ICEDIFF,
	WORKENVLOAD_ICELESS,WORKENVLOAD_ICEMORE,WORKENVLOAD_DUANXIANRUNNING,WORKENVLOAD_DUANXIANBREAK,
	WORKENVLOAD_SETUPSUSPEND,WORKENVLOAD_SETUPSUSPENDING,WORKENVLOAD_SETUPJINXIAN_OTHER_FINISH,WORKENVLOAD_SETUPJINXIAN_OTHER_EMPTY,
	WORKENVLOAD_ANCHORFINISHED,WORKENVLOAD_SETUPANCHORING,WORKENVLOAD_EARTHQUAKE};
class CWireLoadModel
{
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	CXuanLianResolver* m_pResolver;
public:
	WEATHER weather;
	char name[51];	//�������ؼ���ģ������
	long iNo;		//��ʶ���
	CWireInfo wireInfo;
	CHashList<CWorkEnvLoad>hashLoad;	//�����ṩ����
	CWireLoadModel(){memset(name,0,51);iNo=0;}
	//void InitWeather(CTower *pTower);	//Ϊʹ����ͷ�ļ�������������CTower����
	void ResetWorkEnvLoadSet();
	void CopyWireLoadModel(CWireLoadModel *pWireLoadModel);
	void FromBuffer(CBuffer &buffer,int nVersion=0,long doc_type=3);
	void ToBuffer(CBuffer &buffer);
	void ToXmlFile(FILE *fp,DWORD schema);
	//�жϵ����ߵĿ���������������ʼ������״̬����
	void InitDianXianEquation(
		WEATHER &weather,double gama_g,double gama_ice_w,double gama_max_w,double gama_ice);
	//weather����������
	//cal_style=0��ʹ�������߷��̼���cal_style=1:ʹ��б�����߷��̼���
	void CalWorkEnvLoad(char cLandscape,char cLineRoadType);
	//���Թ���
	static CHashStrTable<long> propHashtable;
	static CHashTable<DWORD> propStatusHashtable;
	int GetPropValueStr(long id,char *valueStr);				//�õ�����ֵ�ַ���
	static void InitModelPropHashtable();
	static int GetPropStr(long id,char *propStr);		//�õ������ַ���
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
#endif
};
*/
//���䵽�ڵ��ϵ�����أ����غ��ؼ����������أ�
class CProcessBodyLoad
{
public:
	CFGWORD cfgword;
	double iceWeightLoadCoef;	//�������صļ���ϵ������ϵ�����������(mm)����������(N)
	double iceWeightLoadCoef2;	//�������صļ���Ķ�����ϵ������ϵ����������ȡ��������(mm)����������(N)
	double selfWeightLoad;		//���㵽�˳����ڵ��ϵ����غ���(N)
	CProcessBodyLoad(){iceWeightLoadCoef=iceWeightLoadCoef2=selfWeightLoad=0;}
	CProcessBodyLoad& operator+=(const CProcessBodyLoad& other);
	friend CProcessBodyLoad operator+(const CProcessBodyLoad& v1,const CProcessBodyLoad& v2);
	CProcessBodyLoad& operator*=(double coef);
	CProcessBodyLoad operator*(double coef);
public:
	double IceLoadCoef();
	double IceLoadCoef2();
	double SelfWeight();
};
enum NODE_FORCE_TYPE{AUTO_NODE=0,FORCE_NODE=1,SWING_NODE=2,};	//�Զ��ڵ��Ƿ���������������������ڵ���һ���Ǵ�����,�ڶ��ڵ����������Լ��������аڶ�
enum FORCE_TYPE{CENTRIC_FORCE=0,ECCENTRIC_FORCE=1};	//�˲��������ӵ��������������������ƫ������
//<DEVELOP_PROCESS_MARK nameId="CLDSNode">
struct WIREPLACE
{
	SUBID16 idNodePlace;//��λ���,��ʽΪ"���߷�����-��λ���",���������λ��Ÿ�ռ2���ֽ�
	double fLoadAllocCoefX,fLoadAllocCoefY,fLoadAllocCoefZ;//�����ط����ķ���ϵ��
	WIREPLACE();
};
//�ڵ�������ܿ���Ϣ
struct SLAVE_NODE
{
protected:
	long _hRelaNode;
public:
	BYTE cFlag;	//0
	BYTE ciCtrlType;	//�ܿ�����:0.�ܿز���ֱ�ӿ�������;1.ָ������ϵ�¾ֲ�����;2.��׼���
	BYTE cbMirFlag;		//0x01:ͬʱ����X��Գƽڵ�;0x02:ͬʱ����Y��Գƽڵ�;0x03:ͬʱ����Z��Գƽڵ�
	long hAcs;			//��ָ������ϵ
	char posx_expr[20];
	char posy_expr[20];
	char posz_expr[20];
public:
	SLAVE_NODE();
	void SetKey(DWORD keyid){_hRelaNode=keyid;}
	long get_hRelaNode(){return _hRelaNode;}
	__declspec(property(get=get_hRelaNode)) long hRelaNode;
	void SetControlState(char cCoordType='X',bool controlled=true);
	bool IsControlCoordX();//{return cFlag&0x01;}
	bool IsControlCoordY();//{return cFlag&0x02;}
	bool IsControlCoordZ();//{return cFlag&0x04;}
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);	
};
class CLDSModule;
class CLDSNode : public CLDSDbObject{
	bool m_bInitArcLiftPos;	//Ԥ���λ��Ϊ����ֵ����Ҫ��SetPosition�и��ݽڵ��ʼλ�ø���ֵ
	bool _bVirtualObj;		//���⸨���ڵ�
	CONNECT_NODEINFO* m_pxConnNodeInfo;
	long m_hArcLift;
	GEPOINT pos;	//ԭʼ�ڵ�λ��
	GEPOINT m_xArcLiftPos;		//Ԥ��������
	GEPOINT m_xPreliftPosition;	//�������pos�������Ԥ�������꣬��ʱm_xPreliftPosition��ʾ��ʱ���������Ԥ��ǰ���꣬��������洢 wjh-2018.5.15

	// ------ vvvvvvv -------- �йؽڵ�������㷽ʽ�ı��� -----------
	/*
	�ڵ������������(ȥ��ԭ����"8.�ظ˼���ĳ�ڵ�Ĵ�ֱƫ�Ƶ�")
	0.���κ�����
	1.�˼���X����ֵ�����
	2.�˼���Y����ֵ�����
	3.�˼���Z����ֵ�����
	4.���˼����潻�㣬_cPosCalViceType��ʱ��ʾͶӰ������, 0:��ʾ������ΪͶӰ��,1:X-Zƽ��;2:Y-Zƽ��;3:X-Yƽ��
	5.�˼��ϵı����ȷֵ�
	6.�ظ˼���ĳ�ڵ����������Ľڵ�_cPosCalViceType=0,'Z'��ʾ��Z����,'X'��ʾ��X����,'Y'��ʾ��Y����
	7.�ظ˼���ĳ�ڵ��ƫ�Ƶ�_cPosCalViceType=0��ʾ�ظ˼�����ƫ��,'X'��ʾ��X����,'Y'��ʾ��Y����,'Z'��ʾ��Z����
	8.��׼����XY����ֵ����㣬��Ӧԭ����9��_cPosCalViceType=0��ʾ�޶ԳƲ���,'X'��ʾX��Գ�,'Y'��ʾY��Գ�,'Z'��ʾZ��Գ�,'I'��ʾͨ�����ڵ�ָ�������󽻵���
	9.��׼����YZ����ֵ����㣬��Ӧԭ����10��_cPosCalViceTypeͬ��
	10.��׼����XZ����ֵ����㣬��Ӧԭ����11��_cPosCalViceTypeͬ��
	11.ָ����׼��
	12.ԭʼ�ڵ����X/Y/Z�Գ����ɵĽڵ㣬�Գ���Ϊ_cPosCalViceType='X','Y','Z'
	13.����������㣬�ֱ��Ӧ��
	   pos.x=arrRelationNode[0].pos.x
	   pos.y=arrRelationNode[1].pos.y
	   pos.z=arrRelationNode[2].pos.z
	*/
	BYTE _cPosCalType;
	BYTE _cPosCalViceType;	// ����ڵ�����������͵����������������(���Լ�������4,6��10��Ч)
	double _attach_scale;	// ��������ռ����(�ԽǸ���ʼ�����ȫ�Ǹֳ�)
	double _attach_offset;	// ƫ�ƾ���
	long _hFatherPart;			// �ڵ��������Ǹ־��
	bool MigrateFatherRod(long hNewRod);
protected:
	virtual char get_ciLegQuad();
public:	//�����������m_cPosCalType��ö��ֵ
	static const BYTE COORD_INDEPENDENT	 = 0;
	static const BYTE COORD_X_SPECIFIED  = 1;
	static const BYTE COORD_Y_SPECIFIED  = 2;
	static const BYTE COORD_Z_SPECIFIED  = 3;
	static const BYTE COORD_INTERSECTION = 4;
	static const BYTE COORD_SCALE		 = 5;
	static const BYTE COORD_OFFSET		 = 7;
	static const BYTE PLANE_XY_SPECIFIED = 8;
	static const BYTE PLANE_YZ_SPECIFIED = 9;
	static const BYTE PLANE_XZ_SPECIFIED =10;

	static bool INTERNAL_TEST_VERSION;	//Ŀǰ�������������ڵ��涯����
	//��������Կ�������Ʋ������ܶԳƸ���
	static const DWORD PROP_BASIC_COPY			= 0;	//������Ϣ
	static const DWORD PROP_POS_COPLANAR_COPY	= 1;	//�ڵ��׼�涨λ��Ϣ

	static const DWORD PROP_DISCONNECT	= 0;	//�Ͽ�
	static const DWORD PROP_SYNC_COPY	= 1;	//ͬ���Գ�
	static MEMORY_BLOCK MPCL;	//Mirror Property Control List
	static PROPLIST_TYPE propMirSync;
	static void InitPropMirBitsMap();
	virtual PROPLIST_TYPE* GetSyncPropList(){return &propMirSync;}
	virtual MEMORY_BLOCK GetDefaultMPCL(){return MPCL;}
	virtual bool IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr);
	virtual bool SyncMirProp(const char* propKeyStr=NULL,BYTE ciMirAll0X1Y2Z3=0,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ������
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ��λ�öԳ��޹ص�����
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
	//
	static const BYTE CLEAR_BUFF_OF_RELA_PARTS	=0x01;
	static const BYTE DISABLE_RECORD_RELA_PARTS =0x02;
	static BYTE GetMovePartsUpdateFlag();
	static BYTE SetMovePartsUpdateFlag(BYTE cbFlag=0x01);
	static void ClearUpdateMovePartsBuff();
	static CLDSLinePart* EnumFirstRelaRod();
	static CLDSLinePart* EnumNextRelaRod();
	static CLDSNode* EnumFirstRelaNode();
	static CLDSNode* EnumNextRelaNode();
public:
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
public:
	GEPOINT Feq;					//�������δ���κ�ϵ���ı�׼ֵ
	GEPOINT vSwingLoadFxyz;		//�ɰڶ��ڵ㴦�����ͨ�������Ч�����������,��ʱ���ݲ��洢(N)
	CProcessBodyLoad bodyLoad;		//���䵽�ڵ��ϵĵ�Ч�ڵ������,��ʱ���ݲ��洢
	long m_hRelaWireLoad;
	CNodeRestrict restrict;			//�ڵ�Լ��
	//CHashList<CNodeLoad>hashLoad;	//�洢���ڵ��ڸ������µĸ��ֺ�����
	// ------ VVVVVVV -------- �й�����ԪӦ������Ĳ���------
	NODE_FORCE_TYPE force_type;	//��������
	GEPOINT AutoCalLoad;			//x/y/z���������ֱ��ʾ������������Զ��������ú���(�Ժ���)����ţ����Ҫ�������ʱ��ʱͳ�Ƽ��㣬�����洢(N)
	GEPOINT WindLoad;				//x/y/z���������ֱ��ʾ������������Զ�����ķ����(N)
	GEPOINT m_offset,m_rotoffset;	//�ڵ��λ�ƺ���תλ��
	GEPOINT external_force;		//�ýڵ��ϵ���ӵ�Ч������(N)
	GEPOINT internal_force;		//����Ԫ�㼯���ýڵ��ϵ�����(N)
	GEPOINT internal_moment;		//����Ԫ�㼯���ýڵ��ϵ�������
	GEPOINT unbalance_force;		//���ڵ�Ĳ�ƽ����(N)
	GEPOINT PermanentLoad;			//�˽ڵ��ϵ����ú���,�����ֵӦ�������ú��ط���ϵ����ЧӦϵ��(N)
	GEPOINT m_offsetAmplitude;		//ĳһ�����¸ýڵ�����
	double attachMass;			//������ʩ������������(kg)
	double dfAddedStruMass;		//ģ̬����ʱ�۳������ı���֮����Ҫ���ӵĽṹ�������ģ��ڵ��ȣ�,��ʱ������������ô洢 wjh-2019.7.5
	BYTE m_ciWireFractions;		//���·ּ����ҵ�ҵ�������
	WIREPLACE_CODE wireplace;	//��λ������"ǰ��1-S1"��"ǰ��1-V2"��"��1-2"��
	//BYTE m_cHangWireType;
	PROPERTY(char,m_cHangWireType);	//'E'����Earth Wire,'C'����Conducting Wire,'J'Ϊ���ߵȸ����߹ҵ㣬����Ϊ�ǹ��ߵ�
	SETEX(m_cHangWireType){return wireplace.ciWireType=value;}
	GET(m_cHangWireType){return wireplace.ciWireType;}
	CXhChar50 m_sHangWireDesc;	//��������
	//CHashList<WIREPLACE> wires;		//�ùҵ����ӵ���λ�ñ�ţ�������һ�������
public:
	virtual BOOL IsFatherObject();
	virtual BOOL GetFatherMirObj(RELATIVE_OBJECT *pRelaObj);
	//int GetForceFaceNum(f3dPoint axis_x,f3dPoint *force_face_norm);	//���ظõ������������(����2ʱ����2��forece_face_norm�洢�������淨�ߣ���ֱ�ڸ˵�Ԫ���߷���
	//f3dPoint GetBodyLoad(double ice_thick=0.0);	//���ص�Ч�ڵ������,��λN
	void UpdateWireDesc();
	bool IsHangWire();	//�Ƿ���и������͵ĵ���
	BOOL IsHasRestrict(WORD wRestrict);
	BOOL IsHasRestrict(THANDLE hModule,WORD wRestrict);	//ָ��ģ�����Ƿ���ָ��Լ��
	WORD GetRestrictWord(THANDLE hModule);		//���ָ��ģ���е�Լ����
	void SetRestrict(WORD wRestrict,BOOL bEnable=TRUE);
	void SetRestrict(THANDLE hModule,WORD wRestrict,BOOL bEnable=TRUE);//�趨ָ��ģ���е�ָ��Լ��
	void SetRestrictWord(THANDLE hModule,WORD wRestrict);//�趨ָ��ģ���е�ָ��Լ��
	//CHashList<CExternalNodeLoad>hashNodeLoad;		//�洢���ؽڵ�,keyֵΪ�ڵ���
#endif
	UINT point_i;					//�ڵ���(������Ӧ���ӿ�)
	DECLARE_READONLY_PROPERTY(UINT,pointI);
	DECLARE_READONLY_PROPERTY(GEPOINT,xPreliftPos);				//Ԥ��ǰ��ʼ������
	DECLARE_READONLY_PROPERTY(GEPOINT,xOriginalPos){return pos;}			//��������(��)��Ԥ��ʼ��ʼ���������
	DECLARE_READONLY_PROPERTY(GEPOINT,xArcliftPos);	//Ԥ���������
	DECLARE_READONLY_PROPERTY(GEPOINT,xActivePos);	//��ǰ��ʾ�ĵ�����
public:
	NODE_POS_TO_JG_STRU xFatherAngleZhunJu;

	PROPERTY(long,hFatherPart);
	GET(hFatherPart){return _hFatherPart;}
	SET(hFatherPart){
		if(MigrateFatherRod(value))
			SetModified();
	}
	PROPERTY(bool, m_bVirtualObj);		//���⸨���ڵ�
	GET(m_bVirtualObj){return _bVirtualObj;}
	SET(m_bVirtualObj){
		_bVirtualObj= value;
		SetModified();
	}
	PROPERTY(BYTE,m_cPosCalType);
	GET(m_cPosCalType){
		CLDSNode* pMotherNode=MotherNode();
		if(pMotherNode==this)
			return _cPosCalType;
		else
			return pMotherNode->m_cPosCalType;
	}
	SET(m_cPosCalType){
		if (value>=0)
		{
			CLDSNode* pMotherNode=MotherNode();
			_cPosCalType=value;
			if(pMotherNode!=this)
				pMotherNode->m_cPosCalType=value;
		}
	}
	PROPERTY(BYTE,m_cPosCalViceType);
	GET(m_cPosCalViceType){
		CLDSNode* pMotherNode=MotherNode();
		if(pMotherNode==this)
			return _cPosCalViceType;
		else
			return pMotherNode->m_cPosCalViceType;
	}
	SET(m_cPosCalViceType){
		if(value>=0)
		{
			CLDSNode* pMotherNode=MotherNode();
			_cPosCalViceType=value;
			if(pMotherNode!=this)
				pMotherNode->m_cPosCalViceType=value;
		}
	}
	PROPERTY(double,attach_scale);
	GET(attach_scale)
	{
		CLDSNode* pMotherNode=MotherNode();
		if(pMotherNode==this)
			return _attach_scale;
		else
			return pMotherNode->attach_scale;
	}
	SET(attach_scale)
	{
		CLDSNode* pMotherNode=MotherNode();
		_attach_scale=value;
		if(pMotherNode!=this)
			pMotherNode->attach_scale=value;
	}
	PROPERTY(double,attach_offset);
	GET(attach_offset)
	{
		CLDSNode* pMotherNode=MotherNode();
		if(pMotherNode==this)
			return _attach_offset;
		else 
			return pMotherNode->attach_offset;
	}
	SET(attach_offset)
	{
		CLDSNode* pMotherNode=MotherNode();
		_attach_offset=value;
		if(pMotherNode!=this)
			pMotherNode->attach_offset=value;
	}
	// �����뱾�ڵ��йص���������.
	THANDLE hDatumPoint;	// ��λ��׼��	wht 13-10-17
	THANDLE hRelationPlane;	// ��ػ�׼����
    THANDLE arrRelationNode[4];	// �������ڵ�������ȷֵ�
    THANDLE arrRelationPole[2];	// �������Ǹ��󽻲潻��
//4.���г�Ա��������
public:
	f3dPoint ActivePosition(){return Position(!m_bDisplayBeforeArcLift);}
	f3dPoint Position(bool bArcLiftPos=true);
	double Position(char cCoordType,bool bArcLiftPos=true);
	f3dPoint SetPosition(const f3dPoint& position);
	f3dPoint SetPosition(double x,double y,double z);
	f3dPoint SetPositionX(double coordx);
	f3dPoint SetPositionY(double coordy);
	f3dPoint SetPositionZ(double coordz);
	bool IsUdfCoordX() const;	//X��������Ƿ����û�����
	bool IsUdfCoordY() const;	//Y��������Ƿ����û�����
	bool IsUdfCoordZ() const;	//Z��������Ƿ����û�����
	BOOL Set(bool bIgnoreRepeatFlag=false);				// ���ָ�ֵ������
	bool IsIntegrityParams();	//�Ƿ�������������������
	bool ReverseUpdateParams();	//�������귴��������
	bool SearchRelaDependNodeRods(COLINEAR_TOLERANCE* pTolerance=NULL);	//�Զ����������ڵ�����������ڵ㼰�˼�
	CLDSNode *MotherNode();
	virtual long ArcLiftHandle(){return m_hArcLift;}
	virtual long SetArcLift(long hArcLift){m_hArcLift=hArcLift;SetModified();return m_hArcLift;}
	f3dPoint ArcLiftPos(){return m_xArcLiftPos;}
	virtual void CalPosition(bool bIgnoreRepeatFlag=false){Set(bIgnoreRepeatFlag);}
//4.���г�Ա��������
public:
	long RootFatherPartHandle();
	CLDSPart* RootFatherPart();
	virtual bool IsAcceptableTypeId(int type_id);
	virtual bool IsPart() const{return false;}
	virtual bool IsNode() const{return true;}
	virtual void TransToCS(GECS& cs);
	virtual void Offset(const double* offset);
	virtual bool OffsetPosParam(const double* offset);
	virtual void ResetRelativeObjs();
	void ClearErrorRelativeObjs();
	int ValidCheck();
	CLDSNode();
	~CLDSNode();
	CLDSNode& operator=(const CLDSNode &node);
	void CopyProperty(CLDSDbObject* pObj);
	THANDLE GetHandle(){return handle;}
	long GetRelativeMirObj(MIRMSG msg,bool bIncSelf=TRUE);
	CLDSNode * GetMirNode(MIRMSG mirmsg);
	BOOL AssertValid() const;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	bool RetrieveSlopeTransitionNode(SLOPE_TRANSITION* pTransition);
	bool RetrieveFootTransitionNode(SLOPE_TRANSITION* pTransition);
	CONNECT_NODEINFO* GetConnectNodeInfo();
	bool RetrieveConnectNodeInfo(CONNECT_NODEINFO* pConnNodeInfo,BOOL bDesignBendConnect=TRUE,CLDSModule *pWorkHeight=NULL);	//����ʶ�����ӵ���Ϣ
	BOOL GetIntersZhun(COORD3D* inters_zhun,double *pDist=NULL);
	void GetAttachNodeFatherJg(CLDSNode *pNode1,CLDSNode *pNode2,CLDSLineAngle **ppJg1,CLDSLineAngle **ppJg2);
	f3dPoint GetInverseNodePos(CLDSLineAngle *pJg=NULL);
	bool RetrieveConnectRods(const double* vzWorkNorm, LINEPARTSET& rodSet, bool bIncBendConnect = true);
#endif
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);	
	//bReadForCheck��ʾ�Ƿ��ȡΪ�˽���ģ�ͱȶԼ��(����ȡ������У�������Ϣ)
	void FromCdfBuffer(CBuffer &buffer,long version=0,bool bReadForCheck=false);
	void ToCdfBuffer(CBuffer &buffer,long version=0);	
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:��ʾ������Ӽ���������;true:��ʾ����Ӽ��������� wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	virtual long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	//update_type Ϊ�������� 0-���½ڵ㸸�˼���1-���½ڵ������˼�1��2-���½ڵ������˼�2
	void UpdateMirNodeFatherPart(MIRMSG mirmsg, CLDSPart *pPart, int update_type=0);
	//update_type 0-���������ڵ�1 1-���������ڵ�2 2-���������ڵ�3 3-���������ڵ�4
	void UpdateMirNodeAttachNode(MIRMSG mirmsg,CLDSNode *pAttachNode,int update_type=0);
	//���Թ���
private:
	static long m_idPropUdf;						//�û���ǰ�ɶ���ʹ�õ�����Id
	static long m_idMinPropUdf;						//��ʹ�õ���С�û��Զ�������Id
public:
	static BOOL m_bDisplayBeforeArcLift;			//��ʾԤ��ǰλ��
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	static long RegisterUdfPropId(char *propStr);		//����û��Զ�������Id
	DWORD RetrieveWirePlaceKey(char* propKeyStr);	//�õ�����ֵ
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);				//�õ�����ֵ�ַ���
	static void InitNodePropHashtable();
	UPDATE_PROPLISTITEM_FUNC(CLDSNode,propHashtable)		//������������º���
	static BOOL GetPropStr(long id,char *propStr,int maxStrLen=100);		//�õ������ַ���
	static BOOL FindUdfPropKeyStrByPropId(long id,char* propStr=NULL,int maxStrLen=100);		//�ж�����Id�Ƿ����û��Զ����Id
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//�õ��������Ե�Ĭ��ֻ��״̬
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//�õ��������Ե�Ĭ��ֻ��״̬
	//�汾������־��ط���
#if defined(__PART_DESIGN_INC_)&&!defined(__DRAWING_CONTEXT_)&&!defined(__IGNORE_VERSION_COMPARE_)
	//virtual bool IsIdentical(void* pOldObj);
	virtual bool LogVersionCompare(void* pOldObj,void* pSchema,void** ppLogContents);
#endif
	//����Ϊ�ɺ����������´���ԱȲ���
#ifdef __ALFA_TEST_VERSION_
	GEPOINT TestAutoCalLoad;			//x/y/z���������ֱ��ʾ������������Զ��������ú���(�Ժ���)����ţ����Ҫ�������ʱ��ʱͳ�Ƽ��㣬�����洢(N)
	GEPOINT TestWindLoad;				//x/y/z���������ֱ��ʾ������������Զ�����ķ����(N)
#endif
	bool HasRelativeNode(long hNode);
};
class CNodeMoveDetector{
	CLDSNode* m_pNode;
	GEPOINT m_xOldPos;
	bool m_bEnableDetector;
	static long m_nDepth;	//̽�����
protected:
	bool AppendPosSlaveNodes(CLDSNode* pMasterNode);
public:
	static bool m_bGlobalEnabled;	//�Ƿ�ȫ������/���ýڵ��ƶ�̽��
	CNodeMoveDetector(CLDSNode* pNode,bool enabled=true);
	bool Enable(bool enabled=true){return m_bEnableDetector=enabled;}
	~CNodeMoveDetector();
};
//</DEVELOP_PROCESS_MARK>
#ifdef __PART_DESIGN_INC_
struct HANG_NODE_INFO : public ICompareClass
{
	char m_ciType;
	char m_sName[50];
	GEPOINT pos;
	GEPOINT relaHolePt[2];
	WIREPLACE_CODE m_xWirePlace; 
	HANG_NODE_INFO(){m_ciType=0;strcpy(m_sName,"");}
	virtual int Compare(const ICompareClass *pCompareObj)const;
};
#endif

typedef CLDSNode* NODE_PTR;
typedef CLDSNode* CLDSNodePtr;
#include "TaperPolygon.h"
class CWindSegElemPicker;
class CWindSegment : public ILog{
	int m_nValidNodeCount;		//��ʱ��¼�Ա���ߴ���ִ��Ч��
public:
	CWindSegment();
	void ToBuffer(CBuffer& buffer, long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void FromBuffer(CBuffer& buffer, long version = NULL,long doc_type=3);
public:	//����������Լ�����
	double dfMuSa, dfMuSb;		//��ѹ��������紵�ļ�Ȩƽ������ϵ��(����ʱ�洢������ϵ�������ã����洢) wjh-2019.7.6
	CWindSegment* pMirArmWindSeg;	//����ϵ������ʱָ����ʱ�ĶԳƷ�ѹ�ζ��� wjh-2019.7.6
	TAPER_POLYGON xTaperScope;		//����ϵ������ʱ����ʶ��˼���Ԫ�Ƿ������ǰ��ѹ�εĶ����� 
	bool MirToWindSeg(CWindSegment* pMirWindSeg, int ciMirY1X2, IModel* pModel);
	bool IsIncludeElem(IModel* pTower, const double* pxElemStart, const double* pxElemEnd, double tolerance = 1.0);
	bool IsIncludePoint(IModel* pTower, const double* pxPoint, double tolerance = 1.0);
public:
	CWindSegElemPicker* pSideElemPicker;
	CWindSegElemPicker* pFrontElemPicker;
	GEPOINT xProjPlaneOrg;	//ͶӰ�����,һ��Ϊԭ��(0,0,0)
	int GetMaxNodeCount(){return 8;}//sizeof(nodeArr)/4;}
	int StatValidNodeCount(IModel *pTower);
	int ValidNodeCount(){return m_nValidNodeCount;}
	void InitNodePtrArr(IModel* pTower);
	//bFrontֻ��ԣ��������ʱ��ʾͳ�����滹�ǲ�����������
	bool StatProfileNodeSet(IModel* pTower,NODESET &nodeset,BOOL bFront=TRUE);
	bool IsHasLegObj(IModel* pTower);
	CFGWORD StatCfgword(IModel* pTower);
	long iNo;
	long nodeArr[8];			//�˸������ڵ�ľ��
	CLDSNode* nodePtrArr[8];	//�˸������ڵ��ָ�루��ʱ�洢����Ϊ�־û����ݣ�
	BYTE cType;					//��׼�κ�0.�˵���������;1.�Զ����ڵ�����;2.���������������
	BYTE cMirType;				//�Գ���Ϣ0:�޶Գ�;1:Y��Գ�;2:X��Գ�
	BYTE cCalStyle;				//���㷽ʽ0:������ʽ�����ѹ;1:��X����ˮƽ�ᵣ��ʽ�����ѹ;2:��Y����ˮƽ�ᵣ��ʽ�����ѹ
	BOOL m_bAutoCalProfileCoef;	//�Ƿ��Զ���Ȩƽ������Kֵ����ʱ���û��Լ�����
	double CqiFront, CqiSide;	//��/��������ϵ��
	double K;					//�������ض�����״ϵ������ϽǸ�ʱΪ1.1����Ϊ1.0
	double m_fProfileModeCoef;	//����ϵ���ļ�Ȩƽ������ϵ��
	double AfFront;				//m2����������ܷ�ѹ��ͶӰ���
	double AfSide;				//m2����������ܷ�ѹ��ͶӰ���
	double FaiFront;			//���浲��ϵ��
	double FaiSide;				//���浲��ϵ��
	BOOL   m_bSpecB_A;			//���û�ָ��ӭ�����뱳����֮�������ӭ������֮��ֵ
	struct B_A {
		int a, b;	//ӭ�������Ĵ���ȣ��߶Ƚ�խ�ķ��򣩣���ѹ�ε�ǰ�������ȣ�mm
		B_A() {	a = b = 0;	}
	}b2aFR,b2aSD;
	double B_AFront;			//��������ӭ�����뱳����֮�����/����ӭ������
	double B_ASide;				//��������ӭ�����뱳����֮�����/����ӭ������
	double BetaZ;				//����ϵ����Ʒ���ص���ϵ������z��
	double BetaZForFoundation;	//���ڻ�����Ƶķ���ϵ����Ʒ���ص���ϵ������z��
	bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	virtual long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	void CleanInvalidObjRef(IModel* pBelongModel);
	//������
	DECLARE_PROP_FUNC(CWindSegment);
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);
};
class WINDSEG_FILTER
{
	GECS cs,backcs;	//ǰ�൲����ֲ�����ϵ��ģ������ϵ�¶��壬����Ϊ�����淨�ߣ�
	GECS prjucs;	//��ѹ����������ͶӰ���������ϵ�����ڼ��㹹����Ԫ��ͶӰ�������
	double minZ,maxZ;
	POLYGON frontrgn;	//ǰ��������������
	POLYGON region;		//ͶӰ���������
	CFGWORD bodyword,legword;
	bool ModuleTogetherWith(CLDSDbObject* pDbObj) const;
public:
	WINDSEG_FILTER(){minZ=maxZ=0;}
	bool InitFilter(CWindSegment* pWindSeg);
	bool InitFilter(CLDSNodePtr* nodes,int count,f3dPoint prjOrg);
	bool IsIncludePoint(const double* point3d);
	bool IsIncludeLine(const double* lineStart,const double* lineEnd);
	bool IsIncludeRod(CLDSLinePart* pRod);
	bool IsLineInFrontWindBox(CLDSLinePart* pRod);
};
struct Lo_CALINFO
{
	BOOL bCalByElemLen;
	double coef;	//Lo=coef*DIST(pLoStart,pLoEnd)
	long hStartNode,hEndNode;
	Lo_CALINFO(){memset(this,0,sizeof(Lo_CALINFO));}
};
struct RODEND_NODES{
	long hStart,hEnd;
	RODEND_NODES(){hStart=hEnd=0;}
};
struct SMC_ITEM{
	//ͬʱ��ѹö������
	static const int SMC_NONE	= 0;	//��������ͬʱ��ѹ
	static const int SMC_REFER_MIRX	= 1;//����X��ԳƵ�Ԫͬʱ��ѹ
	static const int SMC_REFER_MIRY	= 2;//����Y��ԳƵ�Ԫͬʱ��ѹ
	static const int SMC_REFER_START= 4;//������ʼ�˽ڵ�ͬ�˵�ͬʱ��ѹ
	static const int SMC_REFER_END	= 8;//�������ն˽ڵ�ͬ�˵�ͬʱ��ѹ
	static const int SMC_REFER_2ENDS=16;//����������˽ڵ�ͬ�˵�ͬʱ��ѹ
	static const int SMC_REFER_UDF	=32;//������ָ����Ԫ��ͬʱ��ѹ
public:
	BYTE cSMCType;		//ͬʱ��ѹ����,��λ(��λ0x80)Ϊ0��ʾƽ���ᣬΪ1��ʾ��С�ᣬ�������ͼ������ö��ֵ
	bool enabled;		//Ĭ��ΪTrue,�������������ո˼�����ѹʱ����ֵΪfalse,
	double ratioL2toL3;	//>0��С��1ʱ��ʾΪ����б��,�ϳ��ָ���б���������ֵ,����������б�Ĵ���
	Lo_CALINFO LoRc;
	RODEND_NODES relaElem;	//�������յ�Ԫ��ʼĩ�˽ڵ���
	BYTE RetrieveSmcType(long hElemStart,long hElemEnd,IModel* pModel);
	SMC_ITEM(){cSMCType=0;enabled=true;ratioL2toL3=0;}
};
#ifndef _ENUM_JOINT_TYPE_
#define _ENUM_JOINT_TYPE_
enum JOINT_TYPE{JOINT_HINGE=0,JOINT_RIGID=1,JOINT_AUTOJUSTIFY=2};		//������ʽ
#endif
class CElemInfo;
struct ELEMINFO_KEY
{
	long hStartNode,hEndNode;
	CElemInfo *pElemInfo;
	void Clear(){hStartNode=hEndNode=0;pElemInfo=NULL;}
	ELEMINFO_KEY(){Clear();}
};
class CElemInfo : public CLDSObject
{
	static long globalId;
public:	//����Ҫ�洢������
	//long Id;	//������ͼ����ʾ������������ʱ��ʶ������ֻҪ��֤ͬһʱ���ȫ��Ψһ����
	long tag;
public://ֻ������
	long m_hParentRod;
	long m_hStartNode,m_hEndNode;
	BYTE ciFreedomStart,ciFreedomEnd;	//��ʱ�洢ʼĩ��ʵ������Ԫ����ʱ�����ɶȡ�wjh-2017.7.14
	//int start_joint_state,end_joint_state;	//0.�½�;1.�ս�;2.ǿ�ƽ½�
	//BOOL m_bValidStartRestStiff,m_bValidEndRestStiff;	//�Ƿ��г��ܲ���ת��Լ��������
	struct RELATIVE_ELEM
	{
		bool bFatherElem;	//�˵�Ԫ�Ƿ�Ϊ�Գƹ�����Ԫ�еĸ���Ԫ�����ڵ���ۺϽ�С�ĵ�Ԫ
		ELEMINFO_KEY xFatherElem,xElemMirX,xElemMirY,xElemMirZ;
		//CElemInfo *pFatherElem,*pElemMirX,*pElemMirY,*pElemMirZ;
	}relaElem;
public:	//�ɸ�������
	char cCalLenType;	//0.�����ж�;1~4.���ȹ���;5.ָ��ƽ�������С�����㳤��
	RODEND_NODES horiElem;			//�����˼�
	Lo_CALINFO LoRyo,LoRx;	//��С�ᡢƽ���ἰͬʱ��ѹʱ�ļ��㳤����Ϣ
	SMC_ITEM smcItems[3];	//��࿼������ͬʱ��ѹ���ո˼���Ԫ��0�Ž���б��1-2��ͬ�˽ڵ�ļ�����
	CElemInfo();
	bool IsSpecLoInfo(){return cCalLenType==5;}
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	short ElemType(CLDSLinePart* pParentRod=NULL);		//0:�����˻򾭵�����1:����
	int StartJointState(CLDSLinePart* pParentRod=NULL);	//0.�½�;1.�ս�;2.ǿ�ƽ½�
	int EndJointState(CLDSLinePart* pParentRod=NULL);	//0.�½�;1.�ս�;2.ǿ�ƽ½�
	int StartForceType(CLDSLinePart* pParentRod=NULL);
	int EndForceType(CLDSLinePart* pParentRod=NULL);
	int LoRyoCalType();	//0.��Ԫ����;1.ָ���ڼ�;2.������
	int LoRxCalType();	//0.��Ԫ����;1.ָ���ڼ�;2.������
	int LoCalType(Lo_CALINFO& LoInfo);
	int GetSmcItemsNum(int *scmItemsTag=NULL);	//�õ�ͬʱ��ѹ���ǵ�����������ѹ����б��
	static long GetNewId();
	static void ClearInitialGlobalId(){globalId=0;}
	//���Թ���
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	UPDATE_PROPLISTITEM_FUNC(CElemInfo,propHashtable)		//������������º���
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ
	static void InitElemInfoPropHashtable();
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
};
typedef CElemInfo* ELEMINFO_PTR;
typedef struct tagDESIGNODDMENT{
	long m_hRefPart1,m_hRefPart2;
	double m_fCollideDist;
	int m_iOddCalStyle;		//0.��ʾ������ײ���ո˼�����;1.������˨λ�ü���;2.��ʾ����������ͷ
	tagDESIGNODDMENT(){m_hRefPart1=m_hRefPart2=0;m_iOddCalStyle=0;m_fCollideDist=0;}
}DESIGNODDMENT;
struct POLE_CALLEN_COEF{
	long hRefPole,hHorizPole;	//��һ����ͬʱ��ѹ���ո˼����ڶ����Ǻ�����
	struct SPEC_LEN{
		long hStartNode,hEndNode;
		double coef;	//���ȼ���ϵ��
	}minR,paraR;
	//double spec_callenMinR,spec_callenParaR;//ָ���˼����㳤�ȣ���ϵ��ֻ����iTypeNo=5ʱ���ã����������ʱ�ж�
	long iTypeNo;	//0.�����ж�;1~4.���ȹ���;5.ָ��ƽ�������С�����㳤��;6.ָ������ļ��㳤��
	long iSelRFlag;	//��ת�뾶ѡ���ʶ���ԽǸ���˵0:��ʾƽ���᣻1:��ʾ��С��)���˲���ֻ��iTypeNo=0ʱ���ã�����ʱ����ʱ�ж�
	//�汾������־��ط���
#if defined(__PART_DESIGN_INC_)&&!defined(__DRAWING_CONTEXT_)&&!defined(__IGNORE_VERSION_COMPARE_)
	//virtual bool IsIdentical(void* pOldObj);
	bool LogVersionCompare(void* pOldObj,void* pSchema,void** ppLogContents);
#endif
};

//�˼���ͷ����������֮�䣬����˨��Ϣ���ڵ�������Ϣ
class CConnectInfo{
public:
	CConnectInfo();
	~CConnectInfo();
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void CopyBasicBoltInfo(CConnectInfo &src);	//����˨���������������������Ϣ
	BYTE m_iConnectWing;	//�Ǹ�����֫����0:�Ǹ�˫֫����;1:X֫����;2:Y֫���ӣ�
	BYTE m_iConnectType;	//���ڸֹ�0����ʾ��������;1.����(����)�������;2.˫��(U�ͻ�ʮ��)�������
	char grade[5];	//��˨����4.8;5.8;6.8;8.8;10.9
	long d;					//��˨ֱ�������
	//-------VVVV---��˨����----VVVV--------------
	union{
		long uiLegacyN;	//long N;
		WORD wnConnBoltN;	//�˼���׼������˨��(�Ǹֵ�֫��������
	};
	WORD get_wnSummBoltN();
	WORD set_wnSummBoltN(WORD cnBoltCount);
	//��ѧ����ʱ�ĵ�����˨����������0��ʾȡͬwnConnCount
	__declspec(property(put=set_wnSummBoltN,get=get_wnSummBoltN)) WORD wnSummBoltN;
	WORD get_wnDualShearConnBoltN();
	//��ͷ��˫����˨����һ��Ϊ������˨����һ�� wjh-2019.6.24
	__declspec(property(get = get_wnDualShearConnBoltN)) WORD wnDualShearConnBoltN;
	//-------^^^^---��˨����----^^^^--------------
	long rows;				//��˨��������
	double bearloadcoef;	//��˨����ϵ��
	union{
		char flNo[16];		//���ӷ���ʱ�ķ���ϵ�к�
		long idAngleJoint;	//�����˼�Ϊ�Ǹ�ʱ��Ч����ʾ�����Ľ�ͷ��ʶ(CTower::hashAngleJoint�еļ�ֵ��
	};
	long m_hCoupleRod;      //���ӷ���ʱ��¼��ͷ���ӵĸ˼����
	//��������ʱ��¼�������˨��༰�˾࣬���洢��ʱʹ�� wht 11-04-26
	struct tagCHIEF_LSSPACE{
		long SingleRowSpace;	//���ż��
		long DoubleRowSpace;	//˫�ż��
		long EndSpace;			//�˾�
		tagCHIEF_LSSPACE(){memset(this,0,sizeof(tagCHIEF_LSSPACE));}
	}LsSpace;
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	double maxTension;		//�˼���ͷ���������������������Ϊ����ѹΪ����,N
	double maxCompression;	//�˼���ͷ�������������ѹ������Ϊ����ѹΪ����,N
	double maxMoment;		//�˼���ͷ������������,N*mm;
#endif
};
struct SIZE_WTM{
	char cMaterial;
	BYTE idClsType;		//�������ͱ�ʶ
	char cSubClassType;	//�Ǹֵ�������,'L':���Ǹ�; 'D'
	double width,thick;	//�Ǹֻ���ϽǸ�֫��֫�񣬲۸ֿ�ߣ��ֹ�ֱ�����ں񣬱�ֿ������ֱ��
	void Set(char material='S',double size_wide=0,double size_thick=0,int idPartClsType=CLS_LINEANGLE,int subClassType='L')
	{
		cMaterial=material;
		idClsType=(BYTE)idPartClsType;
		cSubClassType=subClassType;
		width=size_wide;
		thick=size_thick;
	}
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer);
};
struct SIZESPEC{
	char spec[MAX_TEMP_CHAR_50],wireType[MAX_TEMP_CHAR_50];
	double wide,thick,height,wideUpSide,thick2;
	long idClassType;
	char cSubClassType;
	SIZESPEC(){strcpy(spec,"");strcpy(wireType,"");wide=thick=thick2=height=wideUpSide=0;idClassType=CLS_LINEANGLE;cSubClassType='L';}
#if defined(__PART_DESIGN_INC_)&&!defined(__DRAWING_CONTEXT_)&&!defined(__IGNORE_VERSION_COMPARE_)
	bool LogVersionCompare(void* pOldObj,void* pSchema,void** ppLogContents);
#endif
	bool IsEqual(const SIZESPEC& size);
};
struct ANGLEWINGNORM{
	WORD wLayoutStyle;	//֫�ڷŷ�ʽ1��4Ϊ������2��3Ϊ����
	GEPOINT viewNorm;	//�����淨��
	long hRefPoleArr[2];
};
class CLDSLineTube;
class CRodPosPara;
class CLDSGeneralPlate;
class CLDSLinePart : public CLDSPart{
protected:
	static const BYTE CLEAR_BUFF_OF_RELA_PARTS	=0x01;
	static const BYTE DISABLE_RECORD_RELA_PARTS =0x02;
	static BYTE _localUpdateMovePartsFlag;	//0x01:�������������¼����;0x02:��ʱ��ֹ��¼�Ǹ�λ�ñ䶯��������
	static CHashSet<CLDSBolt*> _localHashModifiedBolts;
	static CHashSet<CLDSGeneralPlate*> _localHashModifiedPlates;
	static CHashSet<CLDSLineAngle*> _localHashModifiedAngles;
	static CHashSet<CLDSLineTube*>	_localHashModifiedTubes;
public:
	static double ODDMENT_MINIMAL_MODIFIED;		//����ͷ��С�䶯ֵ��mm
	static double RODPOSITION_MINIMAL_MODIFIED;	//�˼��˵�λ���ƶ���С���ֵ��mm
	class MOVEROD_STACK{
		bool m_bTerminateStack;
		BYTE m_cbOldUpdateMovePartsState;
	public:
		MOVEROD_STACK();
		MOVEROD_STACK(bool enable);
		~MOVEROD_STACK();
		bool Enable();	//���ؾ�״̬
		bool Disable();	//���ؾ�״̬
		void RestoreStack();
	};
	static BYTE GetMovePartsUpdateFlag();
	static BYTE SetMovePartsUpdateFlag(BYTE cbFlag=0x01);
	static void ClearModifiedPartsBuff();
	static void ClearUpdateMovePartsBuff();
	static CLDSBolt* EnumFirstRelaMoveBolt();	//������һ�������һ��CLDSLineAngle::CalPositionӰ�쵽��λ����˨
	static CLDSBolt* EnumNextRelaMoveBolt();	//������һ�������һ��CLDSLineAngle::CalPositionӰ�쵽��λ����˨
	static CLDSGeneralPlate* EnumFirstRelaMovePlate();	//������һ�������һ��CLDSLineAngle::CalPositionӰ�쵽��λ�Ƹְ�
	static CLDSGeneralPlate* EnumNextRelaMovePlate();	//������һ�������һ��CLDSLineAngle::CalPositionӰ�쵽��λ�Ƹְ�
	static bool RemoveRelaModifiedPart(long hPart);		//�Ƴ��ӹ�����������Ƴ�ָ������������Undo/Redo��ɾ����ʱ��˲�����
	static bool IsHasModifiedRods();
	static bool IsHasRodRelaModifiedParts();	//�Ƿ������˼������䶯����Ҫ����ʵ��Ĺ��� wjh-2018.2.22
protected:
	CLDSPart* m_pOriginalPart;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLsRefList *pLsRefList;
	double start_oddment,end_oddment;
#endif
	BOOL m_bLockStart,m_bLockEnd,m_bLockStartPos,m_bLockEndPos;
	long m_hArcLift;
	f3dArcLine baseline;
	GEPOINT m_xStartBeforeLift,m_xEndBeforeLift;
	virtual bool ArcLift(){return false;}	//����Ԥ������
	DWORD _uStatMatNo;				//ͳ�����
	SIZESPEC size;
	struct NODE_REF{
		bool bActiveHandle;	//false:pNodeֵ��Ч;true:hNode��Ч��wjh-2017.8.21
		union{
			long hNode;
			CLDSNode *pNode;
		};
		NODE_REF(){hNode=0;bActiveHandle=false;}
	}xStartNode,xEndNode;
	virtual char get_ciLegQuad();
public:	//��������
	bool get_blGroupSonAngle();
	__declspec(property(get = get_blGroupSonAngle)) bool blGroupSonAngle;
	virtual f3dPoint WorkPlaneNorm(){return ucs.axis_z;}
	READONLY_PROPERTY(CLDSPart*,pDelegatePart);
	VIRTUAL_GET(pDelegatePart){return m_pOriginalPart;}
	PROPERTY(double,Thick); 
	virtual GET(Thick){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.thick;
		else
			return pMotherObj->Thick;
	}
	virtual SET(Thick){
		if(value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			size.thick = value;
			SetModified();
			if(pMotherObj!=this)
				pMotherObj->Thick=value;
		}
	}
	char* GetSpec(){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.spec;
		else
			return pMotherObj->GetSpec();
	}
	char* SetSpec(char* spec)
	{
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		size.spec[MAX_TEMP_CHAR_50-1]=0;
		strncpy(size.spec,spec,MAX_TEMP_CHAR_50-1);
		if(pMotherObj!=this)
			pMotherObj->SetSpec(size.spec);
		return size.spec;
	}
	char* GetWireType(){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.wireType;
		else
			return pMotherObj->GetWireType();
	}
	char* SetWireType(char* wireType)
	{
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		size.wireType[MAX_TEMP_CHAR_50-1]=0;
		strncpy(size.wireType,wireType,MAX_TEMP_CHAR_50-1);
		if(pMotherObj!=this)
			pMotherObj->SetWireType(size.wireType);
		return size.wireType;
	}
	PROPERTY(double,size_wide); 
	virtual GET(size_wide){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.wide;
		else
			return pMotherObj->size_wide;
	}
	virtual SET(size_wide){
		if(value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			size.wide = value;
			SetModified();
			if(pMotherObj!=this)
				pMotherObj->size_wide=value;
		}
	}
	PROPERTY(double,size_thick); 
	virtual GET(size_thick){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.thick;
		else
			return pMotherObj->size_thick;
	}
	virtual SET(size_thick){
		if(value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			size.thick = value;
			SetModified();
			if(pMotherObj!=this)
				pMotherObj->size_thick=value;
		}
	}
	PROPERTY(double,size_thick2); 
	virtual GET(size_thick2){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.thick2;
		else
			return pMotherObj->size_thick2;
	}
	virtual SET(size_thick2){
		if(value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			size.thick2 = value;
			SetModified();
			if(pMotherObj!=this)
				pMotherObj->size_thick2=value;
		}
	}
	PROPERTY(double,size_height); 
	virtual GET(size_height){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.height;
		else
			return pMotherObj->size_height;
	}
	virtual SET(size_height){
		if(value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			size.height = value;
			SetModified();
			if(pMotherObj!=this)
				pMotherObj->size_height=value;
		}
	}
	PROPERTY(double,size_wideUpSide); 
	virtual GET(size_wideUpSide){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.wideUpSide;
		else
			return pMotherObj->size_wideUpSide;
	}
	virtual SET(size_wideUpSide){
		if(value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			size.wideUpSide = value;
			SetModified();
			if(pMotherObj!=this)
				pMotherObj->size_wideUpSide=value;
		}
	}
	PROPERTY(long,size_idClassType); 
	virtual GET(size_idClassType){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.idClassType;
		else
			return pMotherObj->size_idClassType;
	}
	virtual SET(size_idClassType){
		if(value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			size.idClassType = value;
			SetModified();
			if(pMotherObj!=this)
				pMotherObj->size_idClassType=value;
		}
	}
	PROPERTY(char,size_cSubClassType);
	GET(size_cSubClassType);
	/*{ //����cSubClassType��Ҫ������ϽǸ������Ͷ��壬������ת���� lds_co_part.cpp�ж��� wjh-2019.12.31
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return size.cSubClassType;
		else
			return pMotherObj->size_cSubClassType;
	}*/
	SET(size_cSubClassType);
	/*{ //����cSubClassType��Ҫ������ϽǸ������Ͷ��壬������ת���� lds_co_part.cpp�ж��� wjh-2018.7.5
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		size.cSubClassType=value;
		if(pMotherObj!=this)
			pMotherObj->size_cSubClassType=value;
	}*/
	PROPERTY(DWORD,m_uStatMatNo);
	GET(m_uStatMatNo){
		CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
		if(pMotherObj==this)
			return _uStatMatNo;
		else
			return pMotherObj->m_uStatMatNo;
	}
	SET(m_uStatMatNo){
		if (value>=0)
		{
			CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
			_uStatMatNo=value;
			if(pMotherObj!=this)
				pMotherObj->m_uStatMatNo=value;
		}
	}
	PROPERTY(CLDSNode*,pStart);
	GET(pStart){
		return xStartNode.bActiveHandle?NULL:xStartNode.pNode;
	}
	SET(pStart){
		if (value!=xStartNode.pNode)
		{
			xStartNode.pNode=value;
			xStartNode.bActiveHandle=false;
			SetModified();
		}
	}
	PROPERTY(CLDSNode*,pEnd);
	GET(pEnd){
		return xEndNode.bActiveHandle?NULL:xEndNode.pNode;
	}
	SET(pEnd){
		if (value!=xEndNode.pNode)
		{
			xEndNode.pNode=value;
			xEndNode.bActiveHandle=false;
			SetModified();
		}
	}
	PROPERTY(long,hStartNode);
	GET(hStartNode){
		return xStartNode.bActiveHandle?xStartNode.hNode:(xStartNode.pNode!=NULL?xStartNode.pNode->handle:0);
	}
	SET(hStartNode){
		if (value!=xStartNode.hNode)
		{
			xStartNode.hNode=value;
			xStartNode.bActiveHandle=true;
			SetModified();
		}
	}
	PROPERTY(long,hEndNode);
	GET(hEndNode){
		return xEndNode.bActiveHandle?xEndNode.hNode:(xEndNode.pNode!=NULL?xEndNode.pNode->handle:0);
	}
	SET(hEndNode){
		if (value!=xEndNode.hNode)
		{
			xEndNode.hNode=value;
			xEndNode.bActiveHandle=true;
			SetModified();
		}
	}
public:
	struct USERDEF_DATA{
		double E;		//MPa
		double area;	//mm2
		double ft;		//����ǿ�����ֵ(MPa)
		double fc;		//��ѹǿ�����ֵ(MPa)
		double unit_weight;	//kg/m
		double pre_stress;	//ԤӦ��Pa
	}userdef;
	bool m_bSpecAngleWingNorm;
	ANGLEWINGNORM des_wing_norm;
	// --------- 1.1. ��˨ -----------
public:	//������˨��Ϣ
	CConnectInfo connectStart,connectMid,connectEnd;
	struct ENDBOLTS{
		bool dualConnect;
		char grade[5];
		BYTE n16,n20,n24,nX;//M16��˨��,M20��˨��,M24��˨��
		double Avf,Ahf;		//��˨��Ч�м����,�ױڳ�ѹ���
		ENDBOLTS();
		ENDBOLTS(WORD d,WORD N,const char* grade);
		void Init(WORD d,WORD N,const char* grade);
		void Clear(){Avf=Ahf=n16=n20=n24=nX=0;}
		DECLARE_READONLY_VIRTUAL_PROPERTY(CXhChar50,szBoltStr);
	};
	virtual bool GetEffectiveEndBolts(ENDBOLTS* pEndBolts,bool bRetrieveStart=true){return false;}
	virtual bool IntelliUpdateConnBoltsBySummBolts();	//������˨���������ܸ���(��֫)������
	WORD BoltsNumOn1Wing(BOOL bStart);
	WORD SetBoltsNumOn1Wing(int n,BOOL bStart);
	WORD get_nStartBoltNumOn1Wing(){return connectStart.wnConnBoltN;}
	WORD set_nStartBoltNumOn1Wing(WORD nNum){return connectStart.wnConnBoltN=nNum;}
	__declspec(property(put=set_nStartBoltNumOn1Wing,get=get_nStartBoltNumOn1Wing)) WORD wnStartConnBoltN;
	WORD get_nEndBoltNumOn1Wing(){return connectEnd.wnConnBoltN;}
	WORD set_nEndBoltNumOn1Wing(WORD nNum){return connectEnd.wnConnBoltN=nNum;}
	__declspec(property(put=set_nEndBoltNumOn1Wing,get=get_nEndBoltNumOn1Wing)) WORD wnEndConnBoltN;
public:
	int start_force_type,end_force_type;	//�Ǹ���������״��CENTRIC_FORCE,ECCENTRIC_FORCE
	//0����ȫ�½�JOINT_HINGE��1�����ָս�JOINT_RIGID�����Ƴ�Ťת���ת��Լ����,2���Զ��ж�JOINT_AUTOJUSTIFY
	int start_joint_type,end_joint_type;
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	BOOL m_bActivePole;	// ���ڳ����˳���ʱ������,��ѹʱ���Ƴ�
	BOOL m_bAutoCalHoleOutCoef;	//�Զ�������˨������������Ϊָ����˨������
	POLE_CALLEN_COEF CalLenCoef;
	/*
		���ƹ���0:δ֪1:��ʾ��ϸ�ȿ���2:����ǿ�ȿ���
		3:��ѹǿ�ȿ���4:��ʾ�ȶ�5:ͳ�ĺſ���6:��˨����
	*/
	void* pCtrlElem;			//���Ƹ˼����ĵ�Ԫ��ΪNULLʱ��ʾ��ͳ�ĺſ���
	short m_iElemType;			//0��ʾ�Զ��ж�;1��ʾ���������;2��ʾ��������Ԫ;3��ʾ������Ԫ
	union{
		struct{	//��������ȱ����ǰ��������ʱ,��ʱ�洢ԭ���Ϲ����ϸ
			BYTE _cSubClsType;
			BYTE _ciThick;
			WORD _wiWidth;
		}xInheritSize;
		int iInitialSizeIndex;	//���ڵ���̳�ѡ��ʱ�洢��ѡ��ǰ�˼��ĳ�ʼ�������ţ���ʱ�洢��
	};
	PROPERTY(char,cInheritSubClsType);
	GET(cInheritSubClsType){return xInheritSize._cSubClsType&0x7f;}
	SET(cInheritSubClsType){xInheritSize._cSubClsType=value;}
	PROPERTY(BYTE,ciInheritThick);
	GET(ciInheritThick){return xInheritSize._ciThick;}
	SET(ciInheritThick){xInheritSize._ciThick=value;}
	PROPERTY(WORD,wiInheritWidth);
	GET(wiInheritWidth){return xInheritSize._wiWidth;}
	SET(wiInheritWidth){xInheritSize._wiWidth=value;}
	PROPERTY(bool,bSpecInheritSizeDetails);
	GET(bSpecInheritSizeDetails){return (xInheritSize._cSubClsType&0x80)>0;}
	SET(bSpecInheritSizeDetails){
		if(value)
			xInheritSize._cSubClsType|=0x80;
		else
			xInheritSize._cSubClsType&=0x7f;
	}
	int iSizeIndex;			//���ڰ��չ������ȽϹ���С����ʱ�洢��
	AUX_POLE_SEL_METHOD sel_mat_method;	//����ѡ�ķ�ʽ
	double bear_load_coef;		//������֧����֧�Ÿ˼�������
	BOOL m_bNeedSelMat;			//��Ҫѡ�����
	BOOL m_bNeedSelSize;		//��Ҫѡ����
	double m_fWeightRaiseCoef;	//�����Ŵ�ϵ�����������塢��˨�ȶ������ķŴ�����
	double hole_out;			//�˼��������˨������
	//��1λ��-1.�����Ȳ�Vб�ļ���;0.����ͼ�����ж�;1.ǿ���԰��Ȳ�Vб�ļ���;���ݡ������涨���Ȳ�б�ļ��㳤��Ӧ��1.2����ϵ�� wjh-2017.4.1
	//��2λ��true�������ģ���Ԥ����Ԫ������������Ҫ���ݸ��Ľ������� wjh-2019.7.9
	char m_cbRodFlag;	//0x01
	bool get_bLegVRod();
	bool set_bLegVRod(bool blLegVRod);
	__declspec(property(put=set_bLegVRod,get=get_bLegVRod)) bool m_bLegVRod;
	bool get_blCheckByBrace();
	bool set_blCheckByBrace(bool blBraceRod);
	__declspec(property(put=set_blCheckByBrace,get=get_blCheckByBrace)) bool m_bCheckByBrace;

	double GetElasticityModulus(double para=1.0);	//para���ڼ�����Ϸ����Ե���ģ��(��Ӧ����),MPa
	virtual double GetWorkI(int para=0);	//���ظ˼�ָ�����Ǹ�ʱpara=0ƽ�����para=1��С�ᣩ���Ծ�(mm4)
	virtual double GetWorkR(int para=0);	//���ظ˼�ָ�����Ǹ�ʱpara=0ƽ�����para=1��С�ᣩ��ת�뾶(mm)
	virtual double GetWorkW(int para=0);	//���ظ˼�ָ�����Ǹ�ʱpara=0ƽ�����para=1��С�ᣩ��ת�뾶(mm3)
	double CalSpecLamdaLenMinR();
	double CalSpecLamdaLenParaR();
	virtual BOOL IsFatherObject();
	virtual BOOL GetFatherMirObj(RELATIVE_OBJECT *pRelaObj);
	//����ר�ò�ѯ����
	double m_fPreTension;			//����ê�̶˰�װԤ����,N
#if defined(__TSA_)||defined(__LDS_)//���õ���GetSysPath����������������MapLma.arx�е���
	double GetWireArea();			//���½����,mm2
	double GetWireUnitWeight();		//��λ������,kg/km
	double GetWireForcePermit();	//�������,N
	double GetWireElasticCoef();	//����ϵ��(ģ��),MPa
	double GetWireExpansionCoef();	//�¶�������ϵ��,
#endif
#endif
	BOOL IsAuxPole();	//�Ƿ��Ǹ�����
	BOOL IsPostRod();	//�Ƿ�Ϊ����(������)
	BOOL IsPrimaryRod(bool bOnlyBevelRod=false);	//�Ƿ�Ϊ����ͨ������(��б�ĺ�ĵ�)
	DESIGNODDMENT desStartOdd,desEndOdd;
	BOOL bHuoQuStart,bHuoQuEnd;	//ʼ/�ն˵��Ƿ�Ϊ������
	THANDLE huoqu_handle;		//���ǻ����˼���ʱ�������Ч����ʾ���������Ļ����˼�
	DWORD dwStartPermission,dwEndPermission;	//ʼ�ն˵��޸�Ȩ��
	static BOOL bDisplayLineVecMark;
public:	//Ӱ�乹����غ���
	static const BYTE LINKNODE_NULL		= 0;	//�����ӽڵ�
	static const BYTE LINKNODE_ACTUAL	= 1;	//ʵ�ʴ��ڵĽڵ�
	static const BYTE LINKNODE_SHADOW	= 2;	//Ӱ��Դ�˼��������ӽڵ�,�������������ӽڵ���Ϣ

	virtual bool IsBelongTo(int iClassTypeId)const;
	virtual int StartLinkNodeType();
	virtual int EndLinkNodeType();
	virtual f3dPoint GetDatumPos(CLDSNode* pNode);
	//��ʼ���ӽڵ��ģ��(Ӱ��)����
	virtual f3dPoint StartLinkNodePos(bool bArcLiftPos=true);
	//��ֹ���ӽڵ��ģ��(Ӱ��)����
	virtual f3dPoint EndLinkNodePos(bool bArcLiftPos=true);
	virtual bool CalShadowRodPos();//BOOL bStart,CLDSArcLift *pLift);
	virtual bool ShadowDesign();
public:
	f3dPoint GetMirPickPos(int start0_end1);
	//���ڸ˼�0.��ȫ�Գ�һ��;1.��ʵ��������Գ�һ��;2.ʰȡ�㴦����ͬ��Գ�;3.ʰȡ�㴦��������Գ�
	virtual BYTE MirSimilarGradeWith(CLDSPart *pPart,const MIRMSG& msg,const double *pPickPos=NULL);
	virtual void TransToCS(GECS& cs);
	virtual void Offset(const double* offset);
	virtual void CopyProperty(CLDSDbObject* pObj);
	SIZESPEC GetSizeSpec();
	void SetSizeSpec(SIZESPEC size_info);
	virtual CRodPosPara* DesStartPos(){return NULL;}
	virtual CRodPosPara* DesEndPos(){return NULL;}
	void UpdateMirLinePartEndNode(MIRMSG mirmsg,CLDSNode *pNewStart,CLDSNode *pNewEnd);
	void UpdateRodEndNodeRelation(CLDSNode *pOldNode,CLDSNode *pNewNode);
	//���¸˼�������Ϣ wht 09-12-24
	virtual void UpdateConnectInfo(CLDSNode *pNode){return;}
	virtual void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	virtual void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	//bReadForCheck��ʾ�Ƿ��ȡΪ�˽���ģ�ͱȶԼ��(����ȡ������У�������Ϣ)
	virtual void FromCdfBuffer(CBuffer &buffer,long version=0,bool bReadForCheck=false);
	virtual void ToCdfBuffer(CBuffer &buffer,long version=0);	
	//��Эͬ����ͨ���ܵ���ȡ����
	virtual void FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	//ͨ���ܵ���Эͬ����д������
	virtual void ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	virtual UCS_STRU GetUnifiedACS();	//����ͳһλ�õĹ���װ��(�ֲ�)����ϵ
	virtual UCS_STRU BuildUCS(){return ucs;}
	virtual double GetArea();	//��λΪmm2
	//bSolidLength��ʾ���㳤��ʱ�����߼��㻹�ǰ�ʵ�����,��Ҫ����ѧ����ʱͳ��������
	virtual double GetWeight(BOOL bSolidLength=TRUE);	//��λΪkg
	virtual char* GetBriefPartTypeMark();
	virtual int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	virtual double GetWidth();//{return size_wide;}	//��λΪmm
	virtual double GetThick();//{return size_thick;}	//��λΪmm
	virtual double GetHeight();//{return size_height;}	//��λΪmm
	virtual double GetLength();	//��λΪmm
	virtual double GetThick2();
	virtual void ClearFlag();
	virtual long ArcLiftHandle(){return m_hArcLift;}
	virtual long SetArcLift(long hArcLift){m_hArcLift=hArcLift;SetModified();return m_hArcLift;}
	virtual void CalPosition(bool bIgnoreRepeatFlag=false);
	virtual bool IterCalPosition(double tolerance=0.5,UINT nMaxIterCount=2);
	CLDSLinePart* MotherLinePart();
	CLDSLinePart();
	~CLDSLinePart();
	f3dPoint StartBeforeLift(){return m_xStartBeforeLift;}
	f3dPoint EndBeforeLift(){return m_xEndBeforeLift;}
	f3dPoint Start(bool bArcLift=true) const;
	void SetStart(f3dPoint start,bool bArcLift=true);
	f3dPoint End(bool bArcLift=true) const;
	void SetEnd(f3dPoint end,bool bArcLift=true);

	READONLY_PROPERTY(GEPOINT,xPosStart);
	GET(xPosStart){return Start();}
	READONLY_PROPERTY(GEPOINT,xPosEnd);
	GET(xPosEnd){return End();}

	BOOL LockStartPos(){	//�û������Ǹֶ˵�����λ��ʹ�䲻��
		if(m_bLockStartPos)
			return FALSE;
		else
			m_bLockStartPos = TRUE;
		return TRUE;
	}
	BOOL IsStartPosLocked(){return m_bLockStartPos;}
	void UnlockStartPos(){m_bLockStartPos = FALSE;}
	BOOL LockEndPos(){		//�û������Ǹֶ˵�����λ��ʹ�䲻��
		if(m_bLockEndPos)
			return FALSE;
		else
			m_bLockEndPos = TRUE;
		return TRUE;
	}
	BOOL IsEndPosLocked(){return m_bLockEndPos;}
	void UnlockEndPos(){m_bLockEndPos = FALSE;}
	BOOL LockStart(){	//��������ظ���������˵�����λ��
		if(m_bLockStart)
			return FALSE;
		else
			m_bLockStart = TRUE;
		return TRUE;
	}
	BOOL IsStartLocked(){return m_bLockStart;}
	void UnlockStart(){m_bLockStart = FALSE;}
	BOOL LockEnd(){	//��������ظ���������˵�����λ��
		if(m_bLockEnd)
			return FALSE;
		else
			m_bLockEnd = TRUE;
		return TRUE;
	}
	BOOL IsEndLocked(){return m_bLockEnd;}
	void UnlockEnd(){m_bLockEnd = FALSE;}
	BOOL IsCfgWordCompatible(CFGWORD cfgword);	//�ж϶˽ڵ���˼�������ĺ��Ƿ�һ��
	virtual bool IsAcceptableTypeId(int type_id);
	virtual bool IsLinePart() const{return true;}
	virtual bool IsPolyPart() const{return false;}
	virtual CLDSPart *GetMirPart(MIRMSG mirmsg,const double *pPickPos=NULL,BOOL bCheckProfile=TRUE);
	//��ѧ����ʱ�ĶԳƸ˼�(ֻ���ǵ��߶˽ڵ�����Գ�)
	CLDSLinePart *GetMirRod(MIRMSG mirmsg,double epsilong=EPS);
	bool IsEqualWith(CLDSLinePart *pLinePart,double epsilon=1.0,char *error_info=NULL,int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	double GetDatumLength();
	void GetMirLinePartsByPos(MIRMSG mirmsg,LINEPARTSET &mirRodSet);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	virtual void CalStartOddment(char ciAutoCalMode = 0,int liInitOddment=0);	//0.��ͨ��ײ������;1.ǿ���԰�ָ����ͷ�������ײ������;2.�������н�ʱ��֤�нǱ߾��������ײ������
	virtual void CalEndOddment(char ciAutoCalMode = 0,int liInitOddment=0);		//0.��ͨ��ײ������;1.ǿ���԰�ָ����ͷ�������ײ������;2.�������н�ʱ��֤�нǱ߾��������ײ������
#ifdef __LDS_CONTEXT_
	virtual bool CloneToBomPart(BOMPART* pBomPart){return false;}
#endif
	virtual double startOdd(bool bToFL=false);// const{return start_oddment;}
	virtual double endOdd(bool bToFL=false);// const{return end_oddment;}
	double get_StartOddment(){return startOdd(false);}
	double get_StartOddment2FL(){return startOdd(true);}
	__declspec(property(get=get_StartOddment)) double dfStartOdd;
	__declspec(property(get=get_StartOddment2FL)) double dfStartOdd2FL;
	double get_EndOddment(){return endOdd(false);}
	double get_EndOddment2FL(){return endOdd(true);}
	__declspec(property(get=get_EndOddment)) double dfEndOdd;
	__declspec(property(get=get_EndOddment2FL)) double dfEndOdd2FL;
	BOOL SetStartOdd(double start_odd,bool bToFL=false);//BOOL bCorrectLsPosPara=FALSE,BOOL bCorrectPlateProfile=FALSE,bool bToFL=false);
	BOOL SetEndOdd(double end_odd,bool bToFL=false);//BOOL bCorrectLsPosPara=FALSE,BOOL bCorrectPlateProfile=FALSE,bool bToFL=false);
	virtual long get_hStartDatumRod();
	virtual long get_hEndDatumRod();
	long get_StartDatumRod(){return get_hStartDatumRod();}
	long get_EndDatumRod(){return get_hEndDatumRod();}
	__declspec(property(get=get_StartDatumRod)) long hStartDatumRod;
	__declspec(property(get=get_EndDatumRod)) long hEndDatumRod;
	virtual CLsRefList *GetLsRefList();
	CLsRef* GetFirstLsRef(){return pLsRefList->GetFirst();}
	CLsRef* GetNextLsRef(){return pLsRefList->GetNext();}
	virtual CLsRef* AppendLsRef(CLsRef &ls_ref,BOOL bNearLsCheck=TRUE);
	CLsRef* AppendStartLsRef(CLsRef ls_ref,BOOL bNearLsCheck=TRUE);
	CLsRef* AppendMidLsRef(CLsRef ls_ref,BOOL bNearLsCheck=TRUE);
	CLsRef* AppendEndLsRef(CLsRef ls_ref,BOOL bNearLsCheck=TRUE);
	int GetLocalLsCount(short start1_end2_mid0=0,double length_scope=0);
	virtual int GetLsCount(short d=0);
	void EmptyLsRef(){ pLsRefList->Empty();}
	CLsRef* FindLsByUcsPos(f3dPoint base,UCS_STRU ucs,int iNormX0Y1Z2,double epsilon=EPS2)
	{return pLsRefList->FindLsByUcsPos(base,ucs,iNormX0Y1Z2,epsilon);}
	CLsRef* FindLsByPos(f3dPoint base,f3dPoint work_norm,double epsilon=EPS2)
	{return pLsRefList->FindLsByPos(base,work_norm,epsilon);}
	virtual CLsRef* FindLsByHandle(THANDLE h);
	friend BOOL IsLsRefListEqual(CLsRefList *pLsRefList,const UCS_STRU &ucs,
					   CLsRefList *pViceLsRefList, const UCS_STRU &vice_ucs,
					   double epsilon,char *error_info,long *pH);
#endif
	//ͬ������
	virtual bool IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr);
	virtual bool SyncMirProp(const char* propKeyStr=NULL,BYTE ciMirAll0X1Y2Z3=0,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ������
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ��λ�öԳ��޹ص�����
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
	//���Թ���
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	UPDATE_PROPLISTITEM_FUNC(CLDSLinePart,propHashtable)  //������������º���
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ
	static void InitLinePartPropHashtable();
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
	//�汾������־��ط���
#if defined(__PART_DESIGN_INC_)&&!defined(__DRAWING_CONTEXT_)&&!defined(__IGNORE_VERSION_COMPARE_)
	//virtual bool IsIdentical(void* pOldObj);
	virtual bool LogVersionCompare(void* pOldObj,void* pSchema,void** ppLogContents);
#endif
};
class CLDSCable : public CLDSLinePart{
public:
	CLDSCable();
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
};
//��������ʱ��Ϊ�˼�����ʹ�õĹ�����ʵ�ʿ��ܱ�ʾһ�鹹�������ӻ���
class CLDSDelegateRod : public CLDSLinePart{
	CLDSDelegateRod();
	virtual ~CLDSDelegateRod(){;}
};
/*struct OBJECT_CS
{
	GECS cs;		//��ָ������ϵ�������ڴ洢����ϵ�������ݣ����ڲ���������ϵֻ����ʱ��������ϵ������
	long m_idParaCS;	//��Ϊ����������ϵ�������Դ����Ӧ�Ĳ���������ϵ�ı�ʶID
	OBJECT_CS(){m_idParaCS=0;}
};
enum DATUM_PLANE_TYPE{
	PLANE_TYPE_XY=0,	//XΪ��׼�ᣬYΪ������
	PLANE_TYPE_YX,		//YΪ��׼�ᣬXΪ������
	PLANE_TYPE_YZ,		//YΪ��׼�ᣬZΪ������
	PLANE_TYPE_ZY,		//ZΪ��׼�ᣬYΪ������
	PLANE_TYPE_ZX,		//ZΪ��׼�ᣬXΪ������
	PLANE_TYPE_XZ,		//XΪ��׼�ᣬZΪ������
};
*/
class CParaCS
{
public:	//��̬��������ؽṹ�嶨��
	//����������������׼ƽ��(��)˳��
	static const BYTE DP_XY=0;	//XΪ��׼�ᣬYΪ������
	static const BYTE DP_YX=1;	//YΪ��׼�ᣬXΪ������
	static const BYTE DP_YZ=2;	//YΪ��׼�ᣬZΪ������
	static const BYTE DP_ZY=3;	//ZΪ��׼�ᣬYΪ������
	static const BYTE DP_ZX=4;	//ZΪ��׼�ᣬXΪ������
	static const BYTE DP_XZ=5;	//XΪ��׼�ᣬZΪ������
	//����ϵ��������
	static const BYTE CS_PARAM		=0;	//ͨ������������ȷ������ϵ
	static const BYTE CS_MIRTRANS	=1;	//ͨ����ԭʼ����ϵ���пռ�Գ�(��ת)�任��ȡ
	static const BYTE CS_TRANSLATE	=2;	//ͨ����ԭʼ����ϵ���пռ�ƽ�Ʊ任
	//���ͶԳ���ʽ
	static const BYTE CS_MIRX =1;	//���ڸ���ģ������ϵX-Zƽ��Գ�
	static const BYTE CS_MIRY =2;	//���ڸ���ģ������ϵY-Zƽ��Գ�
	static const BYTE CS_MIRZ =3;	//���ڸ���ģ������ϵZ��Գ�
	struct PARAMETRIC_INFO{	//����������ϵ����Ķ������
		//0.������������㼰װ��ƽ��;1.�Գ�;2.����ƽ��(��desAxisXΪƫ�Ʒ���m_xMirInfo.origin.xΪ����ƫ����)
		BYTE m_cDefStyle;
		//0.�û��Զ������������ϵ
		BYTE m_eDatumPlane;	//0.X-Y;1.Y-X;2.Y-Z;3.Z-Y;4.Z-X;5.X-Z 
		CLDSPoint desOrg;	//0.������������㼰װ��ƽ��
		CLDSVector desAxisX,desAxisY,desAxisZ;
		//1.�Գ�---�����ĸ�����ĶԳ���Ϣ
		//long m_hMotherPart;	//��׼ĸ����,��ԭʼ����ϵ
		MIRMSG m_xMirInfo;	//�޶Գƺ���ת��ʱ��m_xMirInfo.originֵ�������ƽ����
		PARAMETRIC_INFO(){m_cDefStyle=m_eDatumPlane=0;}
	};
private:
	GECS cs;				//ֻ����ʱ��������ϵ������
	bool m_bNeedUpdateCS;	//�Ƿ���Ҫͨ��������Ʋ���[param]��������ϵ������[cs]
	PARAMETRIC_INFO param;	//����ϵ������Ʋ���
	bool RightHandRuled();
public:
	long id;			//��ʶid
	CParaCS();
	void SetKey(DWORD key){id=(long)key;}
	/////////////////////////////////////////////////////
	BYTE GetDefStyle(){return param.m_cDefStyle;}
	PARAMETRIC_INFO GetParamInfo(){return param;};
	void SetParamInfo(PARAMETRIC_INFO para_info){param=para_info;m_bNeedUpdateCS=true;}
	void SetModifiedState(/*bool modified=true*/){m_bNeedUpdateCS=true;}
	//ָ������ϵʱ��Ҫ��ȷ����������ϵ������������������λ����
	bool SetACS(const GECS& ucs,bool bModifyParam=true);
	bool SetACS(const UCS_STRU& ucs,bool bModifyParam=true);
	GECS GetACS(){	return cs;}
	static BOOL CalMirAcs(GECS& cs,CLDSPart *pMotherPart,const MIRMSG& mirInfo);
	BOOL UpdateACS(CLDSPart *pMotherPart);
	BOOL UpdateACS(IModel* pModel);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	//
	f3dPoint CoordTrans(f3dPoint pos);
	f3dPoint VectorTrans(f3dPoint vec);
	BOOL IsDesAxisX();
	BOOL IsDesAxisY();
	BOOL IsDesAxisZ();
	char *GetAxisDescription(char cAxis);
	static char *GetDatumPlaneCmbItems();
};

typedef CLDSLinePart* POLE_PTR;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
class CLDSArcPart : public CLDSPart{
protected:
	CLsRefList *pLsRefList;
public:
	UINT m_uDisplaySlices;
	double m_fZoomLenCoef;//����ʱ�����Բ�����ߵĳ�������ϵ��
	double m_fBaseLineRadius;		//����Բ�����߰뾶
	short m_siLayoutStyle;		//�����������Բ���İڷŷ�ʽ
	BYTE flag;
	CLDSNode *pStart,*pEnd,*_pCenterPickNode,*_pPlanePickNode;
	f3dPoint arc_start;
	double start_rot_angle;		//Բ����ʼ�Ƕ�
	double sector_angle;		//���ν�
	double offset_h,offset_v;	//��׼ĸ�߾��������״����(�궨)ԭ��ĺ�������λ��
	f3dArcLine baseline;
	double start_oddment,end_oddment;
	CLDSPoint des_arc_center,des_arc_start;	//���ι���Բ�ļ���ʼ��
	CLDSVector des_arc_norm;				//���ι������߷���
	//Բ����������, 0:ָ��Բ��ʼĩ�˵㼰�뾶1:ָ��Բ���н�2:ָ��Բ���뾶3:��Բ��
	PROPERTY(BYTE,type);
	GET(type);
	SET(type);
	//Բ���뾶
	PROPERTY(double,radius);
	GET(radius){return m_fBaseLineRadius;}
	SET(radius){m_fBaseLineRadius=value;}
	//Բ��Բ�ĵ��Բ�Ĳ�ʰȡ��
	PROPERTY(CLDSNode*,pCenterPickNode);
	GET(pCenterPickNode);
	SET(pCenterPickNode);
	PROPERTY(GEPOINT,center);
	GET(center);
	SET(center);
	//Բ������ƽ��ʰȡ��
	PROPERTY(CLDSNode*,pPlanePickNode);
	GET(pPlanePickNode);
	SET(pPlanePickNode);
	PROPERTY(GEPOINT,worknorm);
	GET(worknorm);
	SET(worknorm);
public:
	CLDSArcPart();
	~CLDSArcPart();
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	BOOL DesignArcBaseLine();
	BOOL DesignArcBaseLine(CLDSPoint &center,CLDSPoint &start,CLDSVector &workNorm,double rotAngle,double sectorAngle);
	f3dPoint Center(){return baseline.Center();}
	f3dPoint Start(){return baseline.Start();}
	f3dPoint End(){return baseline.End();}
	double Radius(){return baseline.Radius();}
	f3dPoint WorkNorm(){return baseline.WorkNorm();}
	virtual f3dPoint GetDatumPos(f3dPoint pickPos){return pickPos;}
	f3dPoint PositionInAngle(double rad_ang){ return baseline.PositionInAngle(rad_ang); }
	virtual bool IsAcceptableTypeId(int type_id);
	virtual bool IsArcPart() const{return true;}
	virtual double GetWidth(){return 0;}
	virtual double GetThick(){return 0;}
	virtual double GetLength(){return 0;}
	virtual double GetWeight(BOOL bSolidLength=TRUE){return 0;}
	virtual void getUCS(UCS_STRU& ucs);
	virtual GEPOINT GetArcRayNorm(const double* point);		//���θ˼���pNode�ڵ㴦�����߷���(Բ�����淨��)
	virtual GEPOINT GetArcTangentVec(const double* point);	//���θ˼���pNode�ڵ㴦�����߷���(���ȷ�����ʸ��)
	virtual CLsRefList *GetLsRefList(){return pLsRefList;}
	CLsRef* GetFirstLsRef(){return pLsRefList->GetFirst();}
	CLsRef* GetNextLsRef(){return pLsRefList->GetNext();}
	virtual CLsRef* AppendLsRef(CLsRef &ls_ref,BOOL bNearLsCheck=TRUE);
	void EmptyLsRef(){ pLsRefList->Empty();}
	virtual CLsRef* FindLsByHandle(THANDLE h){return pLsRefList->FromHandle(h);}
	friend BOOL IsLsRefListEqual(CLsRefList *pLsRefList,const UCS_STRU &ucs,
					   CLsRefList *pViceLsRefList, const UCS_STRU &vice_ucs,
					   double epsilon,char *error_info,long *pH);
	virtual CLDSPart *GetMirPart(MIRMSG mirmsg,const double *pPickPos=NULL,BOOL bCheckProfile=TRUE);
	//bool bAddSubLevelRelaObj=false:��ʾ������Ӽ���������;true:��ʾ����Ӽ��������� wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//���Թ���
	static int m_iCurDisplayPropGroup;
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;		//����״̬��ϣ��
	UPDATE_PROPLISTITEM_FUNC(CLDSArcPart,propHashtable)   //������������º���
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
	static void InitArcPartPropHashtable();
	static int GetPropStr(long id,char* propStr);		//�õ������ַ���
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
};
class CLDSArcAngle : public CLDSArcPart{
public:
	double wing_wide,wing_thick;
	CLDSArcAngle();
	~CLDSArcAngle(){;}
public:
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer, long version = NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	bool IsEqual(CLDSArcAngle* pMirArcAngle,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	double GetWidth(){return wing_wide;}
	double GetThick(){return wing_thick;}
	double GetLength();
	double GetWeight(BOOL bSolidLength=TRUE);
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	//���Թ���
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	UPDATE_PROPLISTITEM_FUNC(CLDSArcAngle,propHashtable)  //������������º���
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
	static void InitArcAnglePropHashtable();
	static int GetPropStr(long id,char* propStr);		//�õ������ַ���
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
};

class CLDSArcFlat : public CLDSArcPart
{
public:
	double wide,thick;
	CLDSArcFlat();
	~CLDSArcFlat(){;}
public:
	f3dPoint GetDatumPos(f3dPoint pickPos);
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer, long version = NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	bool IsEqual(CLDSArcFlat* pMirArcFlat,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	double GetWidth(){return wide;}
	double GetThick(){return thick;}
	double GetLength();
	double GetWeight(BOOL bSolidLength=TRUE);
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	//���Թ���
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	UPDATE_PROPLISTITEM_FUNC(CLDSArcFlat,propHashtable)  //������������º���
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
	static void InitArcFlatPropHashtable();
	static int GetPropStr(long id,char* propStr);		//�õ������ַ���
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
};

class CLDSArcSlot : public CLDSArcPart
{
public:
	char spec[MAX_CHAR_GUIGE_16+1];
	double wide,thick,height; //��� ��� �߶�
	CLDSArcSlot();
	~CLDSArcSlot(){;}
public:
	f3dPoint GetDatumPos(f3dPoint pickPos);
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer, long version = NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	bool IsEqual(CLDSArcSlot* pMirArcSlot,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	double GetWidth(){return wide;}
	double GetThick(){return thick;}
	double GetHeight(){return height;}
	double GetLength();
	double GetWeight(BOOL bSolidLength=TRUE);
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	//���Թ���
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;		//����״̬��ϣ��
	UPDATE_PROPLISTITEM_FUNC(CLDSArcSlot,propHashtable)  //������������º���
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
	static void InitArcSlotPropHashtable();
	static int GetPropStr(long id,char* propStr);		//�õ������ַ���
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
};
class CLDSPolyPart : public CLDSPart{
public:
	CLDSPolyPart();
	~CLDSPolyPart();
	CTmaPtrList<CLDSLinePart,CLDSLinePart*> segset;
public:
	double GetWidth();
	double GetThick();
	double GetLength();
	void CopyProperty(CLDSDbObject* pObj);
	virtual double GetWeight(BOOL bSolidLength=TRUE);
	virtual int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	virtual bool IsEqual(CLDSPolyPart *pPart,double epsilon=1.0,char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);
	bool IsEqualPolyAngle(CLDSPolyPart *pPolyJg,double epsilon=1.0,char *error_info=NULL,int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);
	bool IsEqualSegAngle(CLDSLineAngle *pSegJg,CLDSLineAngle *pMirSegJg,BOOL bReverse=FALSE,double epsilon=1.0,char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);
	virtual bool CompareLabelPart(CLDSPart *pPeerPart,MIRMSG* pMirmsg=NULL,BOOL bCmpls=TRUE,PEER_PART *pPeerInfo=NULL,double epsilon=1.0);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0){;}
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0){;}
	bool IsLinePart() const{return false;}
	bool IsPolyPart() const{return true;}
	virtual int WriteProcessBuffer(CBuffer &buffer,bool bSaveSubSerial=false,long version=0); //�õ�����Ǹֹ�����չ��Ϣbuffer
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	UCS_STRU GetUCS(bool bIncOddment=false,bool bReverse=false);
	virtual UCS_STRU GetUnifiedACS(){return GetUCS(true);}
	double startOdd(bool bToFL=false);
	double endOdd(bool bToFL=false);
#ifdef __LDS_CONTEXT_
	virtual bool CloneToBomPart(BOMPART* pBomPart);
#endif
	//���ݹ�������˨�׵��Ⱥ�˳������˨���->��������ʶ(��1Ϊ����)ӳ�䣬������˨������ wjh-2016.1.05
	virtual long DispatchHoleIndexId(CHashList<long>* hashHoleIndexIdByBoltHandle);
	static ATOM_LIST<BOLTHOLE_INFO>boltListX,boltListY;
	static BOOL AnaylyzeAngleBolts(CLDSPart *pPolyAnglePart);
};
struct WING_OFFSET_STYLE{
	BYTE gStyle;		//�Ǹ�֫ƫ����:0.��׼׼��;1:g1;2:g2;3:g3;4:�Զ���
	BYTE operStyle;		//��������0.����ͶӰ;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
	double offsetDist;	//����gStyle=4ʱ��Ч
	WING_OFFSET_STYLE(){gStyle=operStyle=0;offsetDist=0.0;}
	void SetOffset(double offset_dist=0,BYTE g_style=4,BYTE oper_style=0);
	void FromBuffer(CBuffer &buffer);
	void ToBuffer(CBuffer &buffer);
	void ToXmlFile(FILE* fp,DWORD schema);
};
class CTubeEndPosPara;
class CJgBerPosPara;
class CMultiOffsetPos;
class CRodPosPara{
protected:
	int m_iPosParaType;	//0:�Ǹ�(��);1:�ֹ�;2:�Ǹ�(��)
public:
	CRodPosPara(){m_iPosParaType=0;}
	virtual BOOL IsInDatumJgBer()=0;		//�����Ƿ��ڻ�׼�Ǹ�������
	virtual BOOL IsInDatumJgWingX()=0;	//�����Ƿ��ڻ�׼�Ǹ�X֫��
	virtual BOOL IsInDatumJgWingY()=0;	//�����Ƿ��ڻ�׼�Ǹ�Y֫��
	CJgBerPosPara* AnglePosParaOld(){return (CJgBerPosPara*)this;}
	CTubeEndPosPara* TubePosPara(){return (CTubeEndPosPara*)this;}
	CMultiOffsetPos* AnglePosPara(){return (CMultiOffsetPos*)this;}
};
class CJgBerPosPara : public CRodPosPara{
public:
	/*long datum_pos;			//ֻ�ڽǸֲ�Ϊ�ڵ㸸�Ǹ�ʱ��Ч��0Ϊ���Ǹ�����,1Ϊ���Ǹ�X֫,2Ϊ���Ǹ�Y֫
	  long joint_style;			//0Ϊ��׼�Ǹ�Ԥ��׼��,1������,2Ϊƫ��׼��,3Ϊָ������λ��,4�ڵ�λ��
	  long offset_style;*/		//�ӻ�׼��ƫ�Ʒ�ʽ:0��ʾͶӰ;1��ʾ��ָ��ƽ��ƽ��;2��ʾָ������;3����ֱ�ߵ�ͶӰ
	//�Ǹ����ļ��㷽ʽ:0:��ʾ����ƫ��;1:��ʾ������ͶӰ;
	//2:��ʾ������ƽ��;3:��ʾֱ��ָ������
	long jgber_cal_style;
	//����ļ��㷽ʽ:0:�ڵ㼴����
	// 1:������ͶӰ      2:X֫����ͶӰ       3:Y֫����ͶӰ
	// 4:������ƽ��      5:X֫����ƽ��       6:Y֫����ƽ��
	// 7:ͶӰ���߻�����  8:ͶӰX֫���߻����� 9:ͶӰY֫���߻�����
	//10:ƽ�����߻����� 11:ƽ��X֫���߻�����12:ƽ��Y֫���߻�����
	//13:X֫ƫ��׼��    14:Y֫ƫ��׼��      15:ֱ��ָ����������
	long datum_pos_style;
	long datum_to_ber_style;	//���ݻ���������ķ�ʽ:0:��ʾͶӰ;1:��ʾƽ��
	short work_wing_auto0_x1_y2_both3;	//ֱ��ָ������ʱ���Ի���ƫ��ʱ�ĵ�ǰ����֫
	short huoqu_pos_auto_0_x_wing1_y_wing2;	//������λ�õ��ж�
	long prj_type;				//0.��ֱ��֫����ͶӰ 1.ƽ����֫����ͶӰ
	long offset_X_dist_style;	//X֫0��ʾƫ����Ϊ��׼׼��,1Ϊg1,2Ϊg2,3Ϊg3,4Ϊ�Զ���
	long offset_Y_dist_style;	//Y֫0��ʾƫ����Ϊ��׼׼��,1Ϊg1,2Ϊg2,3Ϊg3,4Ϊ�Զ���
	double len_offset_dist;		//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	double datum_offset_dist;	//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ط��ߵ�ƫ��
	double offset_X_dist,offset_Y_dist;	//�Զ���ƫ����ʱ�Ի�׼���ƫ����
	double fStartJgLengOffsetX;	//����ͶӰʱ��ʼ�Ǹֻ�׼��㷨��ƫ����
	double fStartJgLengOffsetY;	//����ͶӰʱ��ʼ�Ǹֻ�׼��㷨��ƫ����
	double fEndJgLengOffsetX;	//����ͶӰʱ��ֹ�Ǹֻ�׼��㷨��ƫ����
	double fEndJgLengOffsetY;	//����ͶӰʱ��ֹ�Ǹֻ�׼��㷨��ƫ����
	double fEccentricDist;		//ƫ�ľ�==>���Ǹִ���ڸֹ���ʱ����Ҫ�˱��� wht 09-10-17
	THANDLE datum_jg_h;			//���λ�û�׼�Ǹ�
	THANDLE start_jg_h;			//��ʼ�Ǹ־��
	THANDLE end_jg_h;			//��ֹ�Ǹ־��
	THANDLE huoqu_jg_h;			//������׼�Ǹ־��
	CLDSPoint datum_point;		//��׼����
	f3dPoint face_offset_norm;	//ƽ���淨��
	BYTE cFaceOffsetDatumLine;	//0:����;'X':X֫����;'Y':Y֫����
	double huoqu_dist;			//�����߼�϶
	double offset_zhun;			//ƫ��׼�ߵ�ƫ����
	long iStartJgBerStyle,iEndJgBerStyle;	//ָ����׼�ǸֵĻ�׼�˵�0:ʼ���;1:�����
	//ƽ�ƴ���
	BOOL bByOtherEndOffset;		//������һ�˽���ƽ�Ƽ���
public:
	CJgBerPosPara();
	BOOL IsFaceOffset();		//�Ƿ�Ϊƽ��	wht 10-07-30		
	BOOL IsInDatumJgBer();		//�����Ƿ��ڻ�׼�Ǹ�������
	BOOL IsInDatumJgWingX();	//�����Ƿ��ڻ�׼�Ǹ�X֫��
	BOOL IsInDatumJgWingY();	//�����Ƿ��ڻ�׼�Ǹ�Y֫��
	BOOL IsInHuoQuLine();		//�����Ƿ��ڻ�������
	BOOL IsInOffsetZhunLine();	//�����Ƿ���ƫ��׼����
	int GetDatumType();			//��û��㶨λ����
	BOOL ChangeToDatumType(int prj1_faceoffset2_huoqu3and4_offsetzhun5);	//������ת��Ϊ��һ����
	bool ReplaceReferenceHandle(CHandleReplacer& replacer);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
};
//�˼�(��Ҫ�ǽǸ�)��㶨λ��Ϣ��
class CMultiOffsetPos  : public CRodPosPara
{
public:
	BYTE jgber_cal_style;	//0.��������λ;1.ֱ��ָ���������;2.�򵥶�λģʽ
	//A. ��ʼλ�ö�λ��ʽ
	THANDLE datum_jg_h;		//���λ�û�׼�Ǹ�
	BYTE origin_pos_style;	//0.�ڵ�λ��;1.��׼��;2.��׼�Ǹֹؼ���;3.��һ�˽ڵ�����;4.��һ���������;5.����ʶ���
	BYTE cDatumAngleWing;	//�Ǹ�'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���(����origin_pos_style=2ʱ��Ч)
	WING_OFFSET_STYLE datumWingOffset;
	CLDSPoint datumPoint;
	//B. �ռ临���������(��ʼ��->����Ҳ�ɳƻ��㶨λ)����: 0.��׼��
	// 1:������ͶӰ      2:X֫����ͶӰ       3:Y֫����ͶӰ
	// 4:������ƽ��      5:X֫����ƽ��       6:Y֫����ƽ��
	// 7:ͶӰ���߻�����  8:ͶӰX֫���߻����� 9:ͶӰY֫���߻�����
	//10:ƽ�����߻����� 11:ƽ��X֫���߻�����12:ƽ��Y֫���߻�����
	//13:X֫ƫ��׼��    14:Y֫ƫ��׼��      15:��֫�������
	//16:ָ����ͶӰ		17:ָ����ƽ��		18:��ʽ���Ƶ�ͶӰ 19:��ʽ���Ƶ�ƽ��
	BYTE spatialOperationStyle;
	BYTE huoqu_pos_auto_0_x_wing1_y_wing2;	//������λ�õ��ж�
	double len_offset_dist;		//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	BYTE len_offset_type;		//len_offset_dist��ƫ������0.��׼�˼���������,1.���������ص�ǰ�˼���ֱ����ƫ��,'X','Y','Z'
	double datum_offset_dist;	//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ط��ߵ�ƫ��
	BYTE cFaceOffsetDatumLine;	//0:����;'X':X֫����;'Y':Y֫���ߣ���spatialOperationStyle==17ʱ��Ч
	GEPOINT face_offset_norm;	//ƽ���淨��
	THANDLE huoqu_jg_h;		//������׼�Ǹ־��
	double huoqu_dist;		//�����߼�϶
	double offset_zhun;		//ƫ��׼�ߵ�ƫ����
	union{
		double fEccentricDist;	//ƫ�ľ�==>���Ǹִ���ڸֹ���ʱ����Ҫ�˱��� wht 09-10-17
		double fLenOffsetOfCoordWing;	//��֫�������ʱ������ͶӰ֫���ƽ��֫����������ƫ���� wjh-2016.4.27
	};
	BYTE coordCtrlType;	//0:X-Z����;1:Y-Z����;2.X-Y����(����spatialOperationStyle=15ʱ��Ч)
	BYTE cMainCoordCtrlWing;//�Ǹ�����֫��'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
	WING_OFFSET_STYLE ctrlWing;	//�Ǹ���Ҫ����֫(�����������������֫)
	WING_OFFSET_STYLE otherWing;//�Ǹָ�������֫(����һ�����������֫)
	CLDSPoint datumLineStart,datumLineEnd;	//����spatialOperationStyle=16,17ʱ��Ч
	THANDLE hDatumLine;		//��׼�ߣ�����spatiaOperationStyle=16,17ʱ��Ч��hDatumLine=0ʱ��ʾ�����׼��
	BYTE cMirTransType;		//0,'X','Y','Z',��hDatumLine>0ʱ��Ч
	//C. ƫ�Ʋ���(����->����ʸ��ƫ��)���������ڴ�Ӹ˼���(��)���λ�ü���
	BYTE cWorkWingStyle;//�Ի������ƫ��ʱ��ǰ�ǸֵĻ�׼֫0:�Զ��ж�;1:X֫;2:Y֫;3:˫֫;�����Զ��ж�
	WING_OFFSET_STYLE wing_x_offset;	//X֫����ƫ��������ʽ
	WING_OFFSET_STYLE wing_y_offset;	//Y֫����ƫ��������ʽ(��ʱ����WING_OFFSET_STYLE::operStyle����)
	//////////////////////////////////////////////////////////
	WING_OFFSET_STYLE& get_xMasterWing();
	WING_OFFSET_STYLE& set_xMasterWing(WING_OFFSET_STYLE xOffsetStyle);
	__declspec(property(put = set_xMasterWing, get = get_xMasterWing)) WING_OFFSET_STYLE& xMasterWing;
	WING_OFFSET_STYLE& get_xSlaveWing();
	WING_OFFSET_STYLE& set_xSlaveWing(WING_OFFSET_STYLE xOffsetStyle);
	__declspec(property(put = set_xSlaveWing, get = get_xSlaveWing)) WING_OFFSET_STYLE& xSlaveWing;
	bool get_blLegayDualFairWingCtrlPoint();	//������Ƶ�ͶӰ��������Ϊ��׼��������Ƶ�ƽ�� wjh-2020.1.1
	__declspec(property(get = get_blLegayDualFairWingCtrlPoint)) bool blLegayDualFairWingCtrlPoint;
	//////////////////////////////////////////////////////////
	//jgber_cal_style=2�򵥶�λģʽʱ
private:
	int  GetSimpleDatumPoint();
	void SetSimpleDatumPoint(int iSimpleDatumPointType);
public:
	BYTE cbSimpleUdfFlag;	//����jgber_cal_style==2ʱ��Ч,���ڱ�ʾ�û��˹���Ԥ�Ŀ�����Ϣ wjh-2016.3.12
	//���㶨λ����(ռ��1λ), 0.�����ж�;1.�˹�ָ�����㶨λ����
	PROPERTY(BOOL,bUdfDatumPointMode);	//�����Ǹֵĵ�ǰ���֫
	GET(bUdfDatumPointMode){return cbSimpleUdfFlag&0x01;}
	SET(bUdfDatumPointMode){
		if(value)
			cbSimpleUdfFlag|=0x01;	//0000 0001
		else
			cbSimpleUdfFlag&=0xFE;	//1111 1110
	}
	//���ʱ����������Ǹֵ��ĸ�֫(ռ��2λ), 0.�����ж�;1.�˹�ָ���������֫
	DECLARE_PROPERTY(BOOL,bUdfLappedDatumWing);	//�����Ǹֵĵ�ǰ���֫
	//Ŀ�꽻��ģʽ(ռ��3~4λ), 0.�Զ��ж�;1.����;2.(֫)����;3.������;4.ƫ��׼�� wjh-2016.11.26
	static const BYTE TO_BERLINE	= 0x01;	//���ӵ�����	 (��ӦcbSimpleUdfFlag=0x01)
	static const BYTE TO_AXISLINE	= 0x02;	//���ӵ�X/Y֫����(��ӦcbSimpleUdfFlag=0x01)
	static const BYTE TO_BENDLINE	= 0x03;	//���ӵ�������	 (��ӦcbSimpleUdfFlag=0x02)
	static const BYTE TO_OFFSETZHUN	= 0x04;	//���ӵ�ƫ��׼�� (��ӦcbSimpleUdfFlag=0x03)
	DECLARE_PROPERTY(BYTE,ciDestLineMode)	//�����Ǹֵĵ�ǰ���֫
	//��֫��������ģʽ(ռ��5λ), 0.�Զ��ж�;1.�˹�ָ����֫�����������
	PROPERTY(BOOL,bUdfCtrlCoordsMode);
	GET(bUdfCtrlCoordsMode){return (cbSimpleUdfFlag&0x10)>>4;}
	SET(bUdfCtrlCoordsMode){
		if(value)
			cbSimpleUdfFlag|=0x10;	//0001 0000
		else 
			cbSimpleUdfFlag&=0xEF;	//1110 1111
	}
	//��������Ǹֵĵ�ǰ���ӻ�׼֫
	PROPERTY(BOOL,cSimpleDatumAngleWing);	//�����Ǹֵĵ�ǰ���֫
	GET(cSimpleDatumAngleWing){return cDatumAngleWing;}
	SET(cSimpleDatumAngleWing){cDatumAngleWing=value;}
	//��ǰ�Ǹ�����Ĺ���֫
	PROPERTY(BOOL,cSimpleSelfWorkWing);	//�����Ǹֵĵ�ǰ���֫
	GET(cSimpleSelfWorkWing){return cWorkWingStyle;}
	SET(cSimpleSelfWorkWing){cWorkWingStyle=value;}
	PROPERTY(int,m_iSimpleDatumPoint);	//�����Ǹֵĵ�ǰ���֫
	GET(m_iSimpleDatumPoint){return GetSimpleDatumPoint();}
	SET(m_iSimpleDatumPoint){SetSimpleDatumPoint(value);}
public:
	CMultiOffsetPos();
	void ClearContents();	//�������Բ���Ĭ��ֵ
	void FromJgBerPosPara(CJgBerPosPara *pPosPara);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	BOOL IsFaceOffset();		//�Ƿ�Ϊƽ��	wht 10-07-30		
	BOOL IsInDatumJgBer();		//�����Ƿ��ڻ�׼�Ǹ�������
	BOOL IsInDatumJgWingX();	//�����Ƿ��ڻ�׼�Ǹ�X֫��
	BOOL IsInDatumJgWingY();	//�����Ƿ��ڻ�׼�Ǹ�Y֫��
	BOOL IsInHuoQuLine(bool blIncOffsetGLine=false);	//�����Ƿ��ڻ�������,blIncOffsetGLine=true����ƫ��׼�����
	BOOL IsInOffsetZhunLine();	//�����Ƿ���ƫ��׼����
	int GetDatumType();			//��û��㶨λ����
	void SetToDatumPointStyle(){spatialOperationStyle=0;origin_pos_style=1;}
	BOOL ChangeToDatumType(int prj1_faceoffset2_huoqu3and4_offsetzhun5);	//������ת��Ϊ��һ����
	bool ReplaceReferenceHandle(CHandleReplacer& replacer,double* pickpos_coord);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
};
struct EXTRA_ODDINFO
{
	BYTE style;	//0:��ʾָ��Ϊodd��1:extraStartPoint-extraEndPoint�ļ��ֵ;2:���ݽǸ��Զ��ж�oddֵ
	double odd;	//style=0ʱ��Ч
	//extraStartPoint->extraEndPointʸ������ͷ���� positiveDirect������style==1ʱ��Ч
	CLDSPoint extraDatumPoint;	//style=1ʱ��Ч
	EXTRA_ODDINFO(){style=0;odd=0;}
};
//�Ǹֶ˶���(�̶�)����ͷ��Ϣ
struct EXTRA_ANGLEODD{
	EXTRA_ODDINFO wing_x,wing_y,ber;	//�ֱ��ʾX֫��Y֫������Ϊ��׼ʱ�Ķ�������ͷ
	EXTRA_ANGLEODD(){wing_x.style=wing_y.style=2;}
};
class CKaoMeAngle
{
public:
	SEGI iSeg;
	struct BOLTHOLE{
		short d;
		union POS{
			DWORD key;
			struct{short x,y;};
		}pos;
	};
	CXhChar16 sLabel;
	CXhChar16 sKaiHeAngle;
	short wide,thick,length;
	CHashList<BOLTHOLE> listHoles;
	CKaoMeAngle(){wide=thick=length=0;}
};
struct KAI_HE_JIAO_TEMP{
public:
	float decWingAngle;										//��֫�н�
	short Position(){return position;}						//�궨λ��
	short StartLength(){return position-startPosLength;}	//ʼ�˿��ϳ���
	short EndLength(){return endPosLength-position;}		//�ն˿��ϳ���
	//
	short StartPosLength()const {return startPosLength;}	//ʼ�˿���λ�ó���
	short EndPosLength()const {return endPosLength;}		//�ն˿���λ�ó���
	void SetStartPosLength(short startPosLen){startPosLength=startPosLen; UpdatePos();}
	void SetEndPosLength(short endPosLen){endPosLength=endPosLen; UpdatePos();}
private:
	void UpdatePos(){position=(short)((startPosLength+endPosLength)*0.5);}
	short position;			//�궨λ��
	short startPosLength;	//ʼ�˿���λ�ó���
	short endPosLength;		//�ն˿���λ�ó���	
};
class CLDSGeneralPlate;
class CLDSLineAngle : public CLDSLinePart{
	CLDSLineAngle *GetStartNodeOffsetDatumAngle();
	CLDSLineAngle *GetEndNodeOffsetDatumAngle();
	BOOL CreateSpecialHeJiaoSolidModel(fBody* pBody,BOOL bDisplayHole=FALSE);
protected:
	PRESET_ARRAY4<long> arrDatumBerLocks;	//long arrDatumBerLocks[4] ��ֹGetDatumBerPos�������� wjh-2016.3.26
	bool TestVisitLockLoop(long hNode);
protected:
	GEPOINT norm_x_wing,norm_y_wing;	//X֫Y֫�ķ���
	BYTE  _cnCalNormHits;				//��ֹ֫���߷���ݹ���������ѭ��,������̿�����ʱ����
	/*	#��ˢ�µı�־��
		�ڵ���г�����λ����ʱ���ýڵ�ֻҪ����һ�ξ��ܹ��ﵽ������λ��
		���ڶ��μ���ʱ��������ָñ�־�Ѿ����ˣ��Ͳ��ظ������ˡ�
		(����ظ������ˣ�������Ҳ�϶���ͬ��ֻ�������˹�������)
	*/
	bool _bFootNail;	//�Ŷ��Ǹ�(TRUE) �Ǹ�(FALSE)
	bool _bCutBer,_bCutRoot;
	BYTE _start_push_flat_x1_y2;			//ʼ��X/Y֫ѹ��
	double _start_push_flat_length;		//ʼ��ѹ�ⳤ��
	BYTE _end_push_flat_x1_y2;			//�ն�X/Y֫ѹ��
	double _end_push_flat_length;		//�ն�ѹ�ⳤ��
	BYTE _mid_push_flat_x1_y2;			//�м�X/Y֫ѹ��
	double _mid_push_flat_length;		//�м�ѹ�ⳤ��
	double _mid_push_flat_up_offset;	//��ƫ����
	double _fCutAngleCoefR;	//�нǼ�϶��Բ���뾶ռ��ϵ��
	short _nCutAngleSpaceOdd;//�Ǹ��н�ʱ����ȥ����Բ���뾶ϵ��֮�����ͷ��϶
	//�Ǹ��ĸ��ǵĵ���Բ���뾶��0��ʾ���赹��
	double _dfFilletROfSX,_dfFilletROfSY,_dfFilletROfEX,_dfFilletROfEY;
	//������˨�����㸺ͷʱ����ͷ��������λ����,��������ҪĿ����Ϊ��ƽ�������Ϸ�����Աϰ�� wjh-2019.8.13
	bool _blOddByBoltCanAcrossMiddleSect;	
	bool _blScatterBoltLocaTolerance;	//���ںᵣ�������ļ��ˮƽ�����Ǹ��ϷǱ����γ��ۻ����϶�ʱ������÷�ɢ���ķ�ʽ�����ܱ�֤��ࣩ wjh-2019.8.14
public:
	bool get_blOddByBoltCanAcrossMiddleSect() { return _blOddByBoltCanAcrossMiddleSect; }
	bool set_blOddByBoltCanAcrossMiddleSect(bool blValue);
	__declspec(property(put=set_blOddByBoltCanAcrossMiddleSect,get=get_blOddByBoltCanAcrossMiddleSect)) bool m_blOddByBoltCanAcrossMiddleSect;
	bool get_blScatterBoltLocaTolerance() { return _blScatterBoltLocaTolerance; }
	bool set_blScatterBoltLocaTolerance(bool blValue);
	__declspec(property(put=set_blScatterBoltLocaTolerance,get=get_blScatterBoltLocaTolerance)) bool m_blScatterBoltLocaTolerance;
	double get_dfFilletROfSX();
	double set_dfFilletRofSX(double dfFilletR);
	__declspec(property(put=set_dfFilletRofSX,get=get_dfFilletROfSX)) double dfFilletROfSX;
	double get_dfFilletROfSY();
	double set_dfFilletRofSY(double dfFilletR);
	__declspec(property(put=set_dfFilletRofSY,get=get_dfFilletROfSY)) double dfFilletROfSY;
	double get_dfFilletROfEX();
	double set_dfFilletRofEX(double dfFilletR);
	__declspec(property(put=set_dfFilletRofEX,get=get_dfFilletROfEX)) double dfFilletROfEX;
	double get_dfFilletROfEY();
	double set_dfFilletRofEY(double dfFilletR);
	__declspec(property(put=set_dfFilletRofEY,get=get_dfFilletROfEY)) double dfFilletROfEY;
	//�Ǹ�ĳ֫�ϵĿ���
	struct OPENING_CUT{
		short siCornerFilletR;		//����ڽǹ��ɻ�
		double dfLateralBtmXorY;	//֫���м俪�ڵײ��ĺ���λ�ã�>0��ʾY֫���ڣ�<0��ʾX֫����
		double dfLenPosFrom,dfLenPosTo;			//���ڵײ�����ֹ����λ�ã��ԽǸ���ʼ���棨��������ͷ��Ϊ���
		double dfTipDeltaS,dfTipDeltaE;	//��ֹ�㿪�ڶ�����֫��Ե������Եײ���ƫ������ʼ->��Ϊ����
		OPENING_CUT();
	};
	PRESET_ARRAY4<OPENING_CUT> xarrOpenCuts;	//�Ǹ��ϵĿ��ڲ���
	//�Ǹ־ֲ����Ͻ�����
	struct KAIHE_SECTION{
		char ciOperWing;		//���ϽǵĲ���֫��0.X; 1.Y; 'X'; 'Y'
		double dfFromX,dfToX;	//���ڵײ�����ֹ����λ�ã��ԽǸ���ʼ���棨��������ͷ��Ϊ���
		double dfReservedSizeY;	//���Ͻ�ʱ�ĺ��������
		double dfOpenAngle;		//>0�����Ƕ�����<0���ϽǶ�����0.���迪�Ͻ�
		KAIHE_SECTION();
	};
	PRESET_ARRAY4<KAIHE_SECTION> xarrKaiheSects;	//�Ǹ��Ͼֲ����Ͻǵ�������������
protected:
	//ͨ��CLDSPart::HOLE��������˨�ף���������˨������������ף�ֱ�ӿ��������ɵĹ��տף����ϡ�wjh-2017.11.2
	virtual UINT AddMiscellaneousHoles(IXhList<HOLE>* pListHoles);	//��GetHoleSet�ڵ���,����ӳ�������˨����������
public:
	virtual UINT GetHoleSet(IXhList<HOLE>* pListHoles,bool bOnlyBoltHole=false);	//����ֵΪ������
public:	//��������
	static double KAIHEJIAO_ANGLE_THRESHOLD;
	static double KAIHEJIAO_CLEARANCE_THRESHOLD;
	static CLDSLineAngle* AppendModifiedAngles(CLDSLineAngle* pModifiedAngle);	//��¼��Ҫ����ʵ��ĽǸ�
	static CLDSLineAngle* EnumFirstModifiedAngles();	//������һ�������仯����Ҫ����ʵ��ĽǸ�
	static CLDSLineAngle* EnumNextModifiedAngles();		//������һ�������仯����Ҫ����ʵ��ĽǸ�
	virtual BYTE get_ciStartAnchorWing();
	virtual BYTE get_ciEndAnchorWing();
	_declspec(property(get=get_ciStartAnchorWing)) BYTE ciStartAnchorWing;
	_declspec(property(get=get_ciEndAnchorWing))   BYTE ciEndAnchorWing;
	PROPERTY(BOOL,m_bFootNail);
	GET(m_bFootNail){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _bFootNail;
		else
			return pMotherObj->m_bFootNail;
	}
	SET(m_bFootNail){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		_bFootNail=(value!=0);
		if(pMotherObj!=this)
			pMotherObj->m_bFootNail=value;
	}
	PROPERTY(BOOL,m_bCutBer);
	GET(m_bCutBer){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _bCutBer;
		else
			return pMotherObj->m_bCutBer;
	}
	SET(m_bCutBer){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		_bCutBer=(value!=0);
		if(pMotherObj!=this)
			pMotherObj->m_bCutBer=value;
	}
	PROPERTY(BOOL,m_bCutRoot);
	GET(m_bCutRoot){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _bCutRoot;
		else
			return pMotherObj->m_bCutRoot;
	}
	SET(m_bCutRoot){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		_bCutRoot=(value!=0);
		if(pMotherObj!=this)
			pMotherObj->m_bCutRoot=value;
	}
	PROPERTY(int,start_push_flat_x1_y2);
	GET(start_push_flat_x1_y2){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _start_push_flat_x1_y2;
		else
			return pMotherObj->start_push_flat_x1_y2;
	}
	SET(start_push_flat_x1_y2){
		if (value==0||value==1||value==2)
		{
			_start_push_flat_x1_y2=(BYTE)value;
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			if(pMotherObj!=this)
				pMotherObj->start_push_flat_x1_y2=value;
		}
	}
	PROPERTY(double,start_push_flat_length);
	GET(start_push_flat_length){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _start_push_flat_length;
		else
			return pMotherObj->start_push_flat_length;
	}
	SET(start_push_flat_length){
		if (value>=0)
		{
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			_start_push_flat_length=value;
			if(pMotherObj!=this)
				pMotherObj->start_push_flat_length=value;
		}
	}
	PROPERTY(int,end_push_flat_x1_y2);
	GET(end_push_flat_x1_y2){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _end_push_flat_x1_y2;
		else
			return pMotherObj->end_push_flat_x1_y2;
	}
	SET(end_push_flat_x1_y2){
		if (value==0||value==1||value==2)
		{
			_end_push_flat_x1_y2=(BYTE)value;
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			if(pMotherObj!=this)
				pMotherObj->end_push_flat_x1_y2=value;
		}
	}
	PROPERTY(double,end_push_flat_length);
	GET(end_push_flat_length){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _end_push_flat_length;
		else
			return pMotherObj->end_push_flat_length;
	}
	SET(end_push_flat_length){
		if (value>=0)
		{
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			_end_push_flat_length=value;
			if(pMotherObj!=this)
				pMotherObj->end_push_flat_length=value;
		}
	}
	PROPERTY(int,mid_push_flat_x1_y2);
	GET(mid_push_flat_x1_y2){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _mid_push_flat_x1_y2;
		else
			return pMotherObj->mid_push_flat_x1_y2;
	}
	SET(mid_push_flat_x1_y2){
		if (value==0||value==1||value==2)
		{
			_mid_push_flat_x1_y2=(BYTE)value;
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			if(pMotherObj!=this)
				pMotherObj->mid_push_flat_x1_y2=value;
		}
	}
	PROPERTY(double,mid_push_flat_length);
	GET(mid_push_flat_length){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _mid_push_flat_length;
		else
			return pMotherObj->mid_push_flat_length;
	}
	PROPERTY(double, mid_push_flat_start_offset);
	GET(mid_push_flat_start_offset){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _mid_push_flat_length/2-_mid_push_flat_up_offset;
		else
			return pMotherObj->mid_push_flat_start_offset;
	}
	SET(mid_push_flat_start_offset)
	{
		if(value>=0)
		{
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			_mid_push_flat_length=mid_push_flat_end_offset+value;
			_mid_push_flat_up_offset=_mid_push_flat_length/2-value;
			if(pMotherObj!=this)
				pMotherObj->mid_push_flat_start_offset=value;
		}
	}
	PROPERTY(double, mid_push_flat_end_offset);
	GET(mid_push_flat_end_offset){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _mid_push_flat_length/2+_mid_push_flat_up_offset;
		else
			return pMotherObj->mid_push_flat_end_offset;
	}
	SET(mid_push_flat_end_offset)
	{
		if(value>=0)
		{
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			_mid_push_flat_length=mid_push_flat_start_offset+value;
			_mid_push_flat_up_offset=-(_mid_push_flat_length/2-value);
			if(pMotherObj!=this)
				pMotherObj->mid_push_flat_start_offset=value;
		}
	}
	//�нǼ�϶��Բ���뾶ռ��ϵ��
	PROPERTY(double,m_fCutAngleCoefR);
	GET(m_fCutAngleCoefR){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _fCutAngleCoefR;
		else
			return pMotherObj->m_fCutAngleCoefR;
	}
	SET(m_fCutAngleCoefR){
		if (value>=0)
		{
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			_fCutAngleCoefR=value;
			if(pMotherObj!=this)
				pMotherObj->m_fCutAngleCoefR=value;
			SetModified();
		}
	}
	//�Ǹ��н�ʱ����ȥ����Բ���뾶ϵ��֮�����ͷ��϶
	PROPERTY(short,m_nCutAngleSpaceOdd);
	GET(m_nCutAngleSpaceOdd){
		CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
		if(pMotherObj==this)
			return _nCutAngleSpaceOdd;
		else
			return pMotherObj->m_nCutAngleSpaceOdd;
	}
	SET(m_nCutAngleSpaceOdd){
		if (value>=0)
		{
			CLDSLineAngle* pMotherObj=(CLDSLineAngle*)MotherObject();
			_nCutAngleSpaceOdd=value;
			if(pMotherObj!=this)
				pMotherObj->m_nCutAngleSpaceOdd=value;
			SetModified();
		}
	}
	//��ȡX֫���ߣ�ֻ��
	READONLY_PROPERTY(GEPOINT,vxWingNorm);
	GET(vxWingNorm){
		return this->norm_x_wing;
	}
	//���������ڱ������ֵ֫��������¿�����PROPERTYȡ��BriefSetWingNormX/Y() wjh-2016.8.9
	void SetBriefWingNormX(const double* wingnorm,bool bToSpecNorm=true);
	void SetBriefWingNormY(const double* wingnorm,bool bToSpecNorm=true);
	bool UpdateParentGroupAngleByWingNormX();
	bool UpdateParentGroupAngleByWingNormY();
	//��ȡY֫���ߣ�ֻ��
	READONLY_PROPERTY(GEPOINT,vyWingNorm);
	GET(vyWingNorm){
		return this->norm_y_wing;
	}
public:	
	static BOOL m_bDisplaySolidAxisLine;	//��ʾ�Ǹ����� wht 10-12-27
	static const DWORD PROP_DISCONNECT	= 0;	//�Ͽ�
	static const DWORD PROP_SYNC_COPY	= 1;	//ͬ���Գ�
	static MEMORY_BLOCK MPCL;	//Mirror Property Control List
	static PROPLIST_TYPE propMirSync;
	static void InitPropMirBitsMap();
	virtual PROPLIST_TYPE* GetSyncPropList(){return &propMirSync;}
	virtual MEMORY_BLOCK GetDefaultMPCL(){return MPCL;}
	virtual bool IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr);
	virtual bool SyncMirProp(const char* propKeyStr=NULL,BYTE ciMirAll0X1Y2Z3=0,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ������
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ��λ�öԳ��޹ص�����
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
	virtual bool IsBelongTo(int iClassTypeId)const;
	virtual CRodPosPara* DesStartPos(){return &desStartPos;}
	virtual CRodPosPara* DesEndPos(){return &desStartPos;}
	double  GetWeigthCenterG();	//���ص��������ʱ�Ļ������ģ�һ��λ�ڽǸ������ߣ���Ӧ���ľ�ֵ��һ��ΪW/4 wjh-2019.3.31
	static 	JGZJ GetAngleZJ(double wide);		//��ѯ�Ǹ�׼��
	struct ANGLEPOINT{
		BYTE direction;	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
		BYTE offset_wing;//0.X֫;1.Y֫
		short wing_offset_style;//0.g;1.g1;2.g2;3.g3;4.�Զ���
		long  hResideNode;		//��Ҫ����ʶ��ڵ㴦��������׼������
		double wing_offset_dist,len_offset_dist;//֫��ƫ��ֵ;����ƫ��ֵ;
		char norm_offset[51];//����ƫ����;����Ӧ����������ܷ���CParaThickȡ�� wjh-2017.6.5
		ANGLEPOINT();
	};
	//��ȡ�������й����Գƹ�ϵ�ĽǸּ�ĶԳ�ƥ��״̬��ʼĩ�Ƿ������X/Y֫�����Ƿ����) wjh-2017.11.13
	bool GetMirMatchInfo(CLDSLineAngle* pToMirAngle,MIRMSG mirmsg,bool* pbReverseStartEnd=NULL,bool* pbReverseWingXnY=NULL);
	bool GetMirAnglePoint(const ANGLEPOINT from,ANGLEPOINT &to, CLDSLineAngle* pToMirAngle,MIRMSG mirmsg);
public:
	CLDSLineAngle* Fabricate(CLDSPart* pOrgPart,const double* xArcTagentPt=NULL);	//����һ��ԭʼ����α��һ���Ǹ֣��Խ���һЩ���Ǹֿɽ��еĲ��� wjh-2016.9.3
public:
	BOOL m_bWeldPart;
	BOOL m_bReplPart;	//���ù�񹹼�
	char org_material;	//ԭ����
	BYTE m_cHeJiaoReservedWidthType;	//'I'֫�ڲ�յ㵽�ڲ�ǵ��ȣ�'O'֫���յ㵽��������
	double m_fHeJiaoReservedWidth;			//�Ͻǽϴ�ʱ���������������ģ���������Բ��ԭ�򣬱��뱣�����߸���һ��ֱ�ǿ��
	double org_wing_wide,org_wing_thick;	//ԭ֫��֫��
	WORD m_nCutBerR;
	BOOL m_bEnableTeG;					//���ýǸ�����׼��
	BYTE m_cPosCtrlDatumLineType;		//'X':X֫��׼���߿���;'Y':Y֫��׼���߿���;�������߿���
	BYTE m_cPosCtrlType;				//λ�ÿ�������:0.����ͶӰ;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
	GEPOINT pre_huoqu_start_xyz,pre_huoqu_end_xyz;	//����ǰ��ֹ������
	JGZJ xWingXZhunJu,xWingYZhunJu;		//�Ǹ�����׼��
	THANDLE mid_push_flat_node_handle;	//�м�ѹ��ڵ��� wht 2012-10-10
	DWORD dwLayStyle;						//�ڷŷ�ʽ1��4Ϊ������2��3Ϊ����
	union{
		long hGroupAngle;	//��Ϊ���ü����㣬���Ӿ��ɶ������ wjh-2019.9.30
		THANDLE group_father_jg_h;			//��������ӽǸִ洢����ϸ��Ǹ־��
	};
	double huoqu_r;		//���ǻ����Ǹֶ�ʱ���������Ч����ʾ�����뾶
	double nominal_length;	//���峤��
	long kaihe_base_wing_x0_y1;		//���ϽǸֻ�׼֫
	long huoqu_base_wing_x0_y1_both2;//�����߻�׼֫
	long huoqu_start1_end2;			//��ػ�����λ�ã�1:ʼ�˻�����2:�ն˻�����),�����������ѱ�bHuoQuStart,bHuoQuEndȡ�����Ժ���ʱ��Ҫɾ����
	BOOL m_bUserSpecStartCutAngle;		//�ɲ�����Ա�ֶ�ά��ʼ���н���֫��Ϣ
	BOOL m_bUserSpecEndCutAngle;		//�ɲ�����Ա�ֶ�ά���ն��н���֫��Ϣ
	THANDLE son_jg_h[4];//�ӽǸ־��
	// --------- 1.2. ��֫ -----------
	/*cut_wing_para[0]��ʾʼ�˱���֫����cut_wing_para[1]��ʾ�ն˱���֫����
	...[][0]��ʾ�Ǹ�������ȡ����
	...[][1]��ʾ��һ֫��ȡ���
	...[][2]��ʾ֫������ȡ����
	*/
	double cut_wing_para[2][3];
	//��ʶ���нǸ�֫,0��ʾ����֫,1��ʾX֫,2��ʾY֫
	long cut_wing[2];
    // --------- 1.3. �н� -----------
	/*
	cut_angle[0]��ʾ���X֫
	cut_angle[1]��ʾ���Y֫
	cut_angle[2]��ʾ�յ�X֫
	cut_angle[3]��ʾ�յ�Y֫
	...[][0]��ʾƽ�����߷�����ȡ��  ...[][1]��ʾ��ֱ���߷�����ȡ��
	*/
	double cut_angle[4][2];
public:
	//�����϶: vertex_dist		���߽Ǹ�λ�����Ǹ����ʱʹ�ö����϶ֵ	ͨ������library->GetVertexDist();��� wht 10-07-02
	//�нǼ�϶: cut_angle_space �нǼ�϶Ĭ��ȡ2/3*r(�Ǹ���Բ���뾶) ���û�ָ���нǼ�϶�Ҵ���2/3*rʱʹ���û�����ֵ  
	void SetWeldPart(bool bWeldPart){m_bWeldPart=bWeldPart;}
	bool IsWeldPart();
	virtual bool IsCompositeSonAngle(){return group_father_jg_h>0x20;}	//�Ƿ�Ϊ��ϽǸֵ��ӽǸ�
	void SetHeJiaoReservedWidth(double fHeJiaoReservedWidth){m_fHeJiaoReservedWidth = fHeJiaoReservedWidth;}
	double GetHeJiaoReservedWidth(){return m_fHeJiaoReservedWidth;}
	void SetKaiHeBaseWing(long kaihe_base_wing){kaihe_base_wing_x0_y1 = kaihe_base_wing;}
	long GetKaiHeBaseWing(){return kaihe_base_wing_x0_y1;}
	void SetHuoQuBaseWing(long huoqu_base_wing){huoqu_base_wing_x0_y1_both2 = huoqu_base_wing;}
	long GetHuoQuBaseWing(){return huoqu_base_wing_x0_y1_both2;}
	static bool CoplanarConnectDistinguish(CLDSLineAngle* pAngle1,CLDSLineAngle* pAngle2,int* angle1_wing,int* angle2_wing,BOOL* same_direct=NULL,double justify=0.5);
	bool CalCutAngleInfo(CLDSLineAngle *pCutAngle1,CLDSLineAngle *pCutAngle2,bool bStart,int (*InputCollideDist)(int dist),int cut_angle_space=0);
	BOOL CalCutAngleInfo(f3dPoint face_pick,f3dPoint face_norm,int (*InputCollideDist)(int dist),int cut_angle_space=0);
	double GetCutAngleCoefR();//��ȡ�нǼ�϶��Բ���뾶ռ��ϵ��
	CXhChar16 GetCutAngleSpaceExpr();//��ȡ�Ǹ�֫�ڲ��н�ʱ�ļ�϶ֵ�������ʽ����Բ���뾶*ϵ��+��ͷ��
	JGZJ GetZhunJu(char cWorkWing='X');
	int GetWingYG(int styleG0123);
	int GetWingXG(int styleG0123);
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	virtual double GetArea();	//��λΪmm2
	//bSolidLength��ʾ���㳤��ʱ�����߼��㻹�ǰ�ʵ�����,��Ҫ����ѧ����ʱͳ��������
	virtual double GetWeight(BOOL bSolidLength=TRUE);	//��λΪkg
	double GetWidthWingY(){return IsUnequalAngle()?size_height:size_wide;}
	//double GetWidth(){return size_wide;}
	//double GetThick(){return size.thick;}
	double GetEndWingNormOffset(int x_wing0_y_wing1);
	double GetStartWingNormOffset(int x_wing0_y_wing1);
	virtual BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	double set_wing_wide(double wing_wide);
	double set_wing_thick(double wing_thick);
	double set_y_wing_width(double y_wing_wide);
	BOOL IsUnequalAngle(); //�жϵ�ǰ�Ǹ��Ƿ�Ϊ���ȱ߽Ǹ� wht 12-04-26
	static BOOL IsUnequalAngle(double x_wing_wide,double y_wing_wide,double thick);
	//�õ��򻯹���ַ����������������͡����ʵ���Ϣ������֫��֫�� wht12-04-27
	void GetAngleSimpleSpec(char *simple_spec);
	static void GetAngleSimpleSpec(char *simple_spec,double wing_wide,double y_wing_wide,double wide_thick);

	void RetrieveKaoMeAngle(CKaoMeAngle* pKM,long* hBoltArr,int nBolts,CLDSLineAngle *pCoupleAngle=NULL);
	void GetProcessStr(char *notes,size_t max_len,bool bByEnglish=false,bool bNumericKaiHeInfo=false);
	void RemoveKaiHeJiao();
	int ValidCheck();
	//bool bAddSubLevelRelaObj=false:��ʾ������Ӽ���������;true:��ʾ����Ӽ��������� wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//���½Ǹֵ�ǰ����֫��Ϣ(m_iConnectWing)
	void UpdateConnectInfo(CLDSNode *pNode);
	//��Эͬ����ͨ���ܵ���ȡ����
	virtual void FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	//ͨ���ܵ���Эͬ����д������
	virtual void ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	virtual void CalStartOddment(char ciAutoCalMode = 0,int liInitOddment=0);	//0.��ͨ��ײ������;1.ǿ���԰�ָ����ͷ�������ײ������;2.�������н�ʱ��֤�нǱ߾��������ײ������
	virtual void CalEndOddment(char ciAutoCalMode = 0,int liInitOddment=0);		//0.��ͨ��ײ������;1.ǿ���԰�ָ����ͷ�������ײ������;2.�������н�ʱ��֤�нǱ߾��������ײ������
	long get_hStartDatumRod();
	long get_hEndDatumRod();
	CLDSLinePart* GetStartDatumPart();
	CLDSLinePart* GetEndDatumPart();
	double GetStartOffsetByWingX();
	double GetStartOffsetByWingY();
	double GetEndOffsetByWingX();
	double GetEndOffsetByWingY();
	bool IsCutAngleOrWing();
	bool IsStartCutAngleOrWing();
	bool IsEndCutAngleOrWing();
	//-2:��ֱ����,��λ���߶������� 
	//-1:��ֱ����,��λ���߶��յ����
	//0:����ֱ���� 1:���߶ζ˵��� 2:���߶���
	int IsNodeInJg(CLDSNode *pNode);
	DWORD SetPermission(DWORD dwPerm);
	double GetNodeWingYG(CLDSNode* pNode);
	double GetNodeWingXG(CLDSNode* pNode);
	//0:X֫Ϊ��ǰ�˹���֫;1:Y֫Ϊ��ǰ�˹���֫;2:˫֫��Ϊ����֫���������ļ�����˫֫���ӹ���
	int GetStartWorkWing();
	//0:X֫Ϊ��ǰ�˹���֫;1:Y֫Ϊ��ǰ�˹���֫;2:˫֫��Ϊ����֫���������ļ�����˫֫���ӹ���
	int GetEndWorkWing();
	double GetStartExtraOdd(int wing_x0_y1_ber2);
	double GetEndExtraOdd(int wing_x0_y1_ber2);
	int AutoCorrectSonNode();
	void SetWingYLsPos(CLsRef *pLsRef,double z,double g,double depth);
	void SetWingXLsPos(CLsRef *pLsRef,double z,double g,double depth);
	double GetLsParamG(CLDSBolt *pBolt);	//��ȡ��˨���ڵ�ǰ�Ǹֵ��ľ��������ͬ��GetLsG���ӵ���ʵ�ʿռ����ֵ wjh-2016.1.8
	double GetLsG(CLsRef *pLsRef,double *depth=NULL,bool bInitUCS=true);
	double GetLsG(CLDSBolt *pLs,double *depth=NULL,bool bInitUCS=true);
	double GetLsG(f3dPoint ls_norm,f3dPoint ls_pos,double *depth=NULL);
	//����0��ʾʧ�ܣ�1:��ʾ������-1��ʾ��˨��ֱ��֫�棬������λ������δ�嵽�Ǹ�֫���ϣ�
	bool GetBoltLocalPos(CLDSBolt *pBolt,double *local_pos,bool bIncOddment,int* ciWingX0Y1=NULL);	//���ciWingX0Y1,0:��˨��X֫;1:��˨��Y֫
	virtual bool GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* wingx0_y1=NULL,int iIntersPlane=0);	//��ȡ��˨�����Ǹ���Ƥ�Ľ���λ�� wjh-2015.4.8
	virtual bool UpdateHoleWall(HOLE_WALL* pHoleWall);	//����˨��Ǹַ����仯ʱ��ʵʱ���¿ױ�
	virtual int GatherHoleWalls(CHash64ListEx<HOLE_WALL>& hashHoles);
	virtual bool GetEffectiveEndBolts(ENDBOLTS* pEndBolts,bool bRetrieveStart=true);
	/*
	����ֵ:
	 0��ʾ�Ϸ�;
	-1��˨�˾��С;
	-2��˨���Ʊ߾��С;
	-3��˨�нǱ߾�̫С 
	*/
	int ValidLsList(char sError[MAX_TEMP_CHAR_200+1],
		BOOL bRollingSpace=TRUE,BOOL bCutAngleSpace=TRUE,BOOL bLsEndSpace=TRUE,double epsilon=2);
	BOOL ValidLsPushFlat(char sError[MAX_TEMP_CHAR_200+1],double epsilon=2);
	f3dPoint GetStartInitPos();	//��ȡʼ�˶�λ��ʼλ��(���������Ƿ��������ֵ)
	f3dPoint GetEndInitPos();	//��ȡ�ն˶�λ��ʼλ��(���������Ƿ��������ֵ)
	bool GetStartInitPos(f3dPoint &init_pos);	//��ȡʼ�˶�λ��ʼλ��(���ݲ�������false)
	bool GetEndInitPos(f3dPoint &init_pos);		//��ȡ�ն˶�λ��ʼλ��(���ݲ�������false)
	f3dPoint GetStartDatumPos();
	f3dPoint GetEndDatumPos();
	f3dPoint GetEndDatumPosEx();	//����ǻ�����Ϊ��׼���򷵻ػ�����λ��
	f3dPoint GetStartDatumPosEx();
	f3dPoint get_end_in_ber();		//����ն˵��ڲ����λ��
	f3dPoint get_start_in_ber();	//���ʼ�˵��ڲ����λ��
	BOOL IsLsInWing(CLDSBolt *pLs,int *x_wing0_y_wing1=NULL,bool bNeglectLenLimit=false,double* pRsltIntersPoint=NULL);
	virtual void SetModified(BOOL bTrue=TRUE,BOOL bRenderSolid=TRUE);

	void ClearFlag();
	CLDSLineAngle &operator=(const CLDSLineAngle &jg);
	bool IsKaiJiao(double tolerance=0,double style_cosa0_clearance_1=0);
	bool IsHeJiao(double tolerance=0,double style_cosa0_clearance_1=0);
	bool IsReservedHeJiao();
	double GetRadWingAngle();
	double GetDecWingAngle();
	double ConnectClearance();	//�п��Ͻ�ʱ���������ӵ����߽Ǹ�����϶
	void CopyProperty(CLDSDbObject* pObj);
	CLDSLineAngle();
	~CLDSLineAngle();
	f3dPoint get_norm_x_wing();
	f3dPoint set_norm_x_wing(f3dPoint norm,BOOL bSpecific=TRUE);
	f3dPoint get_norm_y_wing();
	f3dPoint set_norm_y_wing(f3dPoint norm,BOOL bSpecific=TRUE);
	f3dPoint CalWingOffsetVec(char cOffsetWing,double spec_g);
	f3dPoint GetWingVecX();
	f3dPoint GetWingVecY();
	bool IsLenOffsetNode(CLDSNode *pNode);
	virtual f3dPoint GetDatumPos(CLDSNode* pNode);
	//cSpecDatumWing='X'��ʾX֫Ϊ��׼;'Y'��ʾY֫Ϊ��׼������Ϊ���ߣ��ò������ڻ�׼�Ǹ�Ϊ�б���֫�������Ͻ�ʱ��Чwjh-2011.10.21
	f3dPoint GetDatumPosBer(CLDSNode *pNode,char cSpecDatumWing=0,bool bClearLocks=true);//,BOOL bFatherJgPos=TRUE);
	f3dPoint GetDatumPosWingX(CLDSNode *pNode,double spec_g=0);//,BOOL bFatherJgPos=TRUE);
	f3dPoint GetDatumPosWingY(CLDSNode *pNode,double spec_g=0);//,BOOL bFatherJgPos=TRUE);
	//cSpecDatumWing='X'��ʾX֫Ϊ��׼;'Y'��ʾY֫Ϊ��׼������Ϊ���ߣ��ò������ڻ�׼�Ǹ�Ϊ�б���֫�������Ͻ�ʱ��Чwjh-2011.10.21
	f3dPoint GetDatumPosBerByFaceOffset(CLDSNode *pNode,f3dPoint face_norm,f3dPoint *face_pick=NULL,char cSpecDatumWing=0);
	f3dPoint GetDatumPosWingXByFaceOffset(CLDSNode *pNode,f3dPoint face_norm,f3dPoint *face_pick=NULL,double spec_g=0);
	f3dPoint GetDatumPosWingYByFaceOffset(CLDSNode *pNode,f3dPoint face_norm,f3dPoint *face_pick=NULL,double spec_g=0);
	f3dPoint GetSelfDatumPosWingXByFaceOffset(CLDSNode *pNode,f3dPoint face_norm,f3dPoint *face_pick=NULL);
	f3dPoint GetSelfDatumPosWingYByFaceOffset(CLDSNode *pNode,f3dPoint face_norm,f3dPoint *face_pick=NULL);
	f3dPoint GetStartWingDatumPosByFaceOffset(char cWing,f3dPoint face_norm);
	f3dPoint GetEndWingDatumPosByFaceOffset(char cWing,f3dPoint face_norm);
	virtual bool SetDirectMasterAngle();	//�������ϽǸ��ӽǸ�ʱ���趨����Ϊ��ϽǸ�֫���ߵļ����׼�Ǹ� wjh-2016.9.19
	bool cal_x_wing_norm(bool bIgnoreRepeatFlag=false);
	bool cal_y_wing_norm(bool bIgnoreRepeatFlag=false);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	virtual void ResetRelativeObjs();
	void UpdateRelateRayJgPos();	//������ؽǸְڷŷ�ʽ
	BYTE FuncType();
	char GetSelfWorkWing(BOOL bStart,const double* worknorm);//��ǰ�Ǹ�ʼĩ��������֫
	char GetLappedWorkWing(BOOL bStart,const double* worknorm);//ʼ�˻�׼�ǸֵĴ��֫
public:
	//���ܸ��¼򵥶�λʱ�Ǹ�ʼ�˵�λ����Ʋ�����wjh-2016.3.11
	bool IntelliUpdateSimplePosParam(BOOL bStart,CLDSLinePart* pFatherRod=NULL);
private:
	//�¸��ĽǸ����λ�ò������ԭ����ʼ��λ�ü��㺯�� wjh-2012-4-28
	void CalStartPosition(CLDSArcLift *pLift=NULL);
	//�¸��ĽǸ����λ�ò������ԭ�����ն�λ�ü��㺯�� wjh-2012-4-28
	void CalEndPosition(CLDSArcLift *pLift=NULL);
	//��ʱ�����λ�ü������������
	void cal_start_ber_pos();
	void cal_end_ber_pos();
public:
	virtual bool ShadowDesign();
	virtual void TransToCS(GECS& cs);
	virtual void Offset(const double* offset);
	virtual int GatherOddRelaParts(BYTE start1end2all3=3);
	virtual bool RecordRelaPartsAfterModified();
	virtual void CalPosition(bool bIgnoreRepeatFlag=false);
	virtual bool IsPosSelfDependent(bool bDetectStartTip);	//�жϽǸ�ָ�������λ�ò����Ƿ��������������ѭ��
	//ciTurnStyle0or1
	//0.��������ת	��֫��Ϊ����һ֫   ���򲻱�
	//				����֫��Ϊ����һ֫ ������
	//1.�����淭ת  ��֫��Ϊ����һ֫   ������
	//				����֫��Ϊ����һ֫ ���򲻱�
	virtual bool TurnLineAngle(int work_wing_x0_y1,char ciTurnStyle0or1);
	virtual UCS_STRU BuildUCS();
	void getUCS(UCS_STRU &ucs,int base_x0_y1_mid2=0);
	BOOL GetStartEndBoltD(int *start_d,int *end_d);
	int GetKaiHeWingX0Y1(long hPreventSideRod=0);	//hPreventSideRodָ���߸˼�ʱ���øú����ķ����˼���� wjh-2019.8.7
	int GetWorkWingX0Y1();
	bool IsStronglyNoneKaiHeAngle();	//�Ƿ�Ϊǿ�Ʋ����Ͻǵ���������������ģ����������
	bool IsEqual(CLDSLineAngle *pJg,double epsilon=1.0,char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	virtual bool CompareLabelPart(CLDSPart *pPeerPart,MIRMSG* pMirmsg=NULL,BOOL bCmpls=TRUE,PEER_PART *pPeerInfo=NULL,double epsilon=1.0);
	virtual void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	virtual void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	virtual bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	//bReadForCheck��ʾ�Ƿ��ȡΪ�˽���ģ�ͱȶԼ��(����ȡ������У�������Ϣ)
	virtual void FromCdfBuffer(CBuffer &buffer,long version=0,bool bReadForCheck=false);
	virtual void ToCdfBuffer(CBuffer &buffer,long version=0);	
	BOOL UpdateInnerCircleSlopeFace(fBody* pBody,f3dPoint wing_x_vec,f3dPoint wing_y_vec,f3dPoint norm_Y);
	BOOL UpdateWingFilletArcFace(fBody* pBody,f3dPoint wing_x_vec,f3dPoint wing_y_vec,f3dPoint norm_Y);
	virtual int WriteProcessBuffer(CBuffer &buffer,bool bSaveSubSerial=false,long version=0);			//�õ��Ǹֹ�����չ��Ϣ��buffer
	void EmendShortJgLsPosY(CLDSBolt* pBolt,double &fLsPosY);
#ifdef __LDS_CONTEXT_
	virtual bool CloneToBomPart(BOMPART* pBomPart);
#endif
	/*
	CLsRefList *GetLsRefList(){return pLsRefList;}
	CLsRef* GetFirstLsRef(){return pLsRefList->GetFirst();}
	CLsRef* GetNextLsRef(){return pLsRefList->GetNext();}
	int GetLsCount(short d=0){return pLsRefList->GetLsCount(d);}
	void EmptyLsRef(){ pLsRefList->Empty();}
	CLsRef* FindLsByUcsPos(f3dPoint base,UCS_STRU ucs,int iNormX0Y1Z2,double epsilon=EPS2)
	{return pLsRefList->FindLsByUcsPos(base,ucs,iNormX0Y1Z2,epsilon);}
	CLsRef* FindLsByPos(f3dPoint base,f3dPoint work_norm,double epsilon=EPS2)
	{return pLsRefList->FindLsByPos(base,work_norm,epsilon);}
	CLsRef* FindLsByHandle(THANDLE h){return pLsRefList->FromHandle(h);}
	friend BOOL IsLsRefListEqual(CLsRefList *pLsRefList,const UCS_STRU &ucs,
					   CLsRefList *pViceLsRefList, const UCS_STRU &vice_ucs,double epsilon);
	*/
	//�������
	DESIGN_JGWING_NORM des_norm_x,des_norm_y;
	CMultiOffsetPos desStartPos,desEndPos;
	CJgBerPosPara *pOldDesStartPos,*pOldDesEndPos;
	EXTRA_ANGLEODD extraStartOdd,extraEndOdd;	//ʼĩ�˵Ķ���(�̶�)����ͷ
	bool IsLappedAnchorStart();	//ʼ���Ƿ�Ϊ��Ӷ�λ
	bool IsLappedAnchorEnd();	//�ն��Ƿ�Ϊ��Ӷ�λ
	bool IsValidCombinationDesPara(bool bStartEnd);//�ж�ʼ(true)��(false)��λ����Ʋ�������Ƿ�Ϸ�
	static BOOL IsInsideAngle(CLDSLineAngle* pAngle,f3dPoint norm,int *x_wing0_y_wing1);
	//����
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	UPDATE_PROPLISTITEM_FUNC(CLDSLineAngle,propHashtable)		//������������º���
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
	static int GetPropValueStr(CLDSLineAngle* pLineAngle,long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
	static int InitLineAnglePropHashtable();
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//�õ��������Ե�Ĭ��ֻ��״̬
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//�õ��������Ե�Ĭ��ֻ��״̬
	//����Ϊ�ɺ����������´���ԱȲ���
#ifdef __ALFA_TEST_VERSION_
public:
	void Test_CalPosition();
	int Test_GetStartWorkWing();
	int Test_GetEndWorkWing();
	double Test_GetStartExtraOdd(int wing_x0_y1_ber2);
	double Test_GetEndExtraOdd(int wing_x0_y1_ber2);
	f3dPoint Test_GetStartDatumPos();
	f3dPoint Test_GetEndDatumPos();
	f3dPoint Test_GetStartDatumPosEx();
	f3dPoint Test_GetEndDatumPosEx();
	double Test_GetStartOffsetByWingX();
	double Test_GetEndOffsetByWingX();
	double Test_GetStartOffsetByWingY();
	double Test_GetEndOffsetByWingY();
	void Test_UpdateRelateRayJgPos();
	void Test_UpdateConnectInfo(CLDSNode *pNode);
	void Test_CopyProperty(CLDSDbObject* pObj);
#endif
	PROPERTY(int,LayStyle);	//�ڷŷ�ʽ1��4Ϊ������2��3Ϊ����
	GET(LayStyle){
		return dwLayStyle&0x000000ff;
	}
	SET(LayStyle){
		dwLayStyle=dwLayStyle&0x00000f00;	//ԭ0x00000100����Ǳ�����⣬ֻ��֮ǰǡ��δ�к��ʵ��á�wjh-2016.5.29
		dwLayStyle+=(value&0xff);
	}
	//MirLayStyle������
	static const DWORD MIRXorY_UNCHAHNGE	= 0;	//X��Y��Գ�ʱ,����ԭ��������
	static const DWORD MIRXorY_SAMEDIRECT	= 1;	//X��Y��Գ�ʱ,����֫����ͬ��
	static const DWORD MIRXorY_INVERT		= 2;	//X��Y��Գ�ʱ,����֫���ⷴ��
	PROPERTY(DWORD,MirLayStyle);
	GET(MirLayStyle){
		return (dwLayStyle&0x00000f00)>>8;
	}
	SET(MirLayStyle){
		dwLayStyle&=0x000000ff;
		dwLayStyle|=(value&0x0000000f)<<8;
	}
};
class CDesignProfileVertex{
public:
	f3dPoint ber_pos,edge_pos;
	THANDLE hJg;				//�Ǹ־��
	THANDLE hNode;				//��׼�ڵ���
	double g,ber_space,edge_space;	//�ľ�;����������һ���ƫ����;������֫��һ���ƫ����
	short direction;				//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	short offset_wing;			//0.X֫;1.Y֫
	double len_offset_dist;		//����ƫ��ֵ
	short face_i;				//1��ʾ��׼��,2��ʾ��һ������,3��ʾ�ڶ�������
public:
	CDesignProfileVertex();
	BOOL UpdateVertex(IModel* pTower);
};

//��Ʋ�������
enum DESIGN_PARAM_TYPE{
	PARAM_POS = 0,		//��λ����
	PARAM_SHAPE = 1,	//���β���
};
/* �ò��ִ���������ComplexId.h,ComplexId.cpp�� wjh-2014.6.14
union KEY2C{
private:
	static char pack_str[3];
	char label[2];
public:
	WORD wItem;
	operator WORD(){return wItem;}
	KEY2C(WORD wKey=0){wItem=wKey;}
	KEY2C(char* keyStr){
		wItem=0;
		if(keyStr!=NULL)
		{
			for(size_t i=0;i<strlen(keyStr)&&i<2;i++)
				label[i]=keyStr[i];
		}
	}
	char* Label();//����ָ������ʱ�ַ���ָ��
	bool IsEqual(KEY2C key){return wItem==key.wItem;}
};
union KEY4C{
private:
	char label[4];
public:
	DWORD dwItem;
	struct{
		WORD wItem;	//key��ǰ�벿��2�ֽ�һ����Ϊѭ������������ʶ
		WORD index;	//key�к�벿��2�ֽ�һ����Ϊѭ��������������ʶ
	}pair;
public:
	operator DWORD(){return dwItem;}
	KEY4C(DWORD dwKey=0){dwItem=dwKey;}
	KEY4C(WORD wItem,WORD wIndex);
	KEY4C(char* keyStr);
	char* Label();//����ָ������ʱ�ַ���ָ��, ����һ�ε���KEY4C::Label()ǰ��Ч
	KEY4C ClonePrior(WORD index) const;	//��¡ǰ�벿(wItem)����벿ȡֵ����indexֵ
	bool IsEqual(KEY4C key){return dwItem==key.dwItem;}
};
*/
/*
struct DESIGN_PARAM_ITEM
{
	KEY4C key;			//�ò����ı�ʶ��ֵ���򲻳���4�������ַ�����
	CXhChar50 sLabel;	//����������ʾ�Ķ����Ʊ�ǩ
	CXhChar100 sNotes;	//�����жԸ������ԵĽ���������
	union{double fValue;THANDLE hPart;long iType;}value;
	BYTE dataType;	//0.double��ֵ 1.��� 2. ����
	BOOL bModify;
	DESIGN_PARAM_ITEM(){dataType=0; bModify=FALSE;}
public:	//dataType����������
	static const BYTE TYPE_DOUBLE	= 0;
	static const BYTE TYPE_HANDLE	= 1;
	static const BYTE TYPE_INTEGER	= 2;
};
*/
#include "DesignParamItem.h"
struct DESIGN_PLATE_ITEM
{
	KEY2C key;
	CXhChar200 sNotes;
	struct DUAL_VALUE{THANDLE hObj;DWORD uiAttribute;};
	union{
		double fValue;
		struct {THANDLE hObj;UINT uiAttribute;};
		THANDLE hPart;
		long iType;
	}value;
	BYTE dataType;	//0.double��ֵ 1.��� 2. ���� 3.���+��������
	BOOL bModify;
	BYTE paramType;	//0.��λ���� 1.���β���
	void SetKey(DWORD dwKey){key=(WORD)dwKey;}
	double ToDouble();
	long   ToLong();
	double SetItemValue(double val);
	DECLARE_READONLY_PROPERTY(double,fValue){return ToDouble();}
	DECLARE_READONLY_PROPERTY(long,hValue)	{return ToLong();}
	DECLARE_READONLY_PROPERTY(long,iValue)	{return ToLong();}
	DECLARE_READONLY_PROPERTY(UINT,uiAttribute){
		if(dataType==3)
			return value.uiAttribute;
		else
			return 0;
	}
	DESIGN_PLATE_ITEM(){dataType=0; bModify=FALSE; paramType=PARAM_SHAPE;}
};
class CDesignPlateBoltPara{
public:
	CDesignPlateBoltPara(){;}
	~CDesignPlateBoltPara(){;}
	int d;				//��˨ֱ��
	int iSeg;			//��˨�κ�
	double x,y,z;		//��׼������ֵ
	int array_num;		//���γ�����˨����
	double scope_angle;	//���γ������Ƕ�,��λ:�㡣�Ƕ�С�ڵ���0ʱ���������;����0ʱ�����γ���
};

struct WELDLINE{
	static const char SMART_SLOPECUT = 0x01;	//�����ж��¿�
	static const char SPECIFY_HEIGHT = 0x02;	//ָ������ߣ��������ĸ�������м��㣩
	char cCalModeFlag;	//0.�¿ڣ�2.�����
public:
	static const char STATE_NEED_SLOPECUT  = 0x01;	//��Ҫ�����¿�
	static const char STATE_FSQUARE_WELD   = 0x02;	//����ƽ������ͻ�����죩������Ĭ��Ϊ����
	static const char STATE_BSQUARE_WELD   = 0x04;	//����ƽ������ͻ�����죩������Ĭ��Ϊ����
	char cStateFlag;	//0.�¿ڣ�2.����ƽ(��)����4.����ƽ(��)��
public:
	short height;			//����߶�,mm
	short length;			//���쳤��,mm
	short lenpos;		//�������λ��(�Ա�ʼ�˵��ƫ����), mm
	long hWeldMotherPart;	//����ĸ�������������ڵ�ĸ������
public:
	WELDLINE();
	//�����趨����������ѡ��ı�ʶ��,����ѡ�������ʶλ�ڵ�λ������ѡ���ʶλ�Ը�λ����
	char SetStateFlagDword(char cFlag,bool bSet=true);
	char SetCalModeFlagDword(char cFlag,bool bSet=true);
	//�Ƿ����ĳ�ֱ�ʶ�趨
	bool IsHasStateFlag(char cFlag);
	bool IsHasCalModeFlag(char cFlag);
	void FromBuffer(CBuffer &buffer);
	void ToBuffer(CBuffer &buffer);
};
class CLDSGeneralPlate;
struct PROFILE_EDGEINFO{
	bool m_bWeldEdge; 
	bool m_bRollEdge;				//�˱�����
	WELDLINE weldline;				//�������
	short manu_space;				//����ӹ�Ԥ����϶���߸߶�(mm)
	short roll_edge_offset_dist;	//���ƫ�ƾ��� wht 11-04-02
	double radius;					//��Բ������Ҫ�Ĳ���
	double sector_angle;			//��׼Բ������Ҫ�Ĳ���
	//NewPlate.cpp��OnCutPlateByColumnFace���õ���center.feature����ʱ�����ܸ�ΪGEPOINT wjh-2014.12.29
	f3dPoint center,column_norm;	//��Բ������Ҫ�Ĳ�������Զ�����������),center�����ʾԲ��Բ��(ʰȡ��)ʱ���������������
	double local_point_y;			//�Ի��λ�ã��ڸõ������ע
	int local_point_vec;			//0.��ʾ�޶Ի�㣬1.��ʾ�Ի�������ע50-->100�ķ����뺸�Ӹ��˼���start-->end������ͬ��2.��ʾ�෴
};
struct SIMPLE_VERTEX{
	BYTE type;					//0:��ֱͨ��1:ָ��Բ���н�2:ָ��Բ���뾶3:��Բ��
	BYTE ciPlaneNo;				//�ڵ���������1.��ʾ��׼��
	double radius;				//��Բ������Ҫ�Ĳ���
	double sector_angle;		//��׼Բ������Ҫ�Ĳ���
	GEPOINT vertex;				//�����λ������
	GEPOINT center,vColNormal;	//��Բ������Ҫ�Ĳ�������Զ�����������),center�����ʾԲ��Բ��(ʰȡ��)ʱ���������������
	SIMPLE_VERTEX(){type=0;ciPlaneNo=1;radius=sector_angle=0;}
};
class PROFILE_VERTEX{
public:
	static const BYTE DT_NONE		= 0;	//�û��Զ���
	static const BYTE DT_BER		= 1;	//���ݸ˼�����������߲ඥ��	
	static const BYTE DT_WING		= 2;	//���ݸ˼��������֫��ඥ��
	static const BYTE DT_BER2		= 3;	//��������ʱ���ݸ˼����������һ�����߲ඥ��
	static const BYTE DT_WING2		= 4;	//��������ʱ���ݸ˼����������һ��֫��ඥ��
	static const BYTE DT_ARCTANGENT	= 5;	//���е㣨Ҫ���ڱ�һ��Ϊֱ�ߣ���һ��ΪԲ�������������䣩����
	static const BYTE DT_INNERBEND	= 6;	//ֱ�߱��ڵ�����㣨�����ʼ��λ��ǰһ�������һ����������������߽����ϣ�
	static const BYTE DT_WELDSTART		= 7;	//�������ʼ���㣨��׼�˼�ʼ�˲ࣩ
	static const BYTE DT_WELDEND		= 8;	//�������ֹ���㣨��׼�˼�ʼ�˲ࣩ
	static const BYTE DT_WELDSTART_TIP	= 9;	//�������ʼ���㣨��׼�˼�ʼ�˲ࣩ
	static const BYTE DT_WELDEND_TIP	= 10;	//�������ֹ���㣨��׼�˼�ʼ�˲ࣩ*/
	static const BYTE DT_PARAMETRIC	= 11;	//��������������õĶ���*/
	static const BYTE DT_BEND_DATUM_POINT	= 12;	//�����߹�������壬�����׼��	wht 18-12-17
	static const BYTE DT_BEND_EXTEND_POINT	= 13;	//�����߹�������壬���������

	BYTE type;	//0:��ֱͨ��1:ָ��Բ���н�2:ָ��Բ���뾶3:��Բ��
	f3dPoint vertex;				//�����λ������
	BYTE get_ciPlaneNo() const{ return vertex.feature; }
	BYTE set_ciPlaneNo(BYTE _ciPlaneNo);
	//����ƽ��,1:��ʾ��׼��;2.��ʾ��һ������������;3.��ʾ�ڶ������������� wjh-2019.9.4
	__declspec(property(put=set_ciPlaneNo,get=get_ciPlaneNo)) BYTE ciPlaneNo;
	bool m_bCanCut;//�жϰ����Ƿ�ȥ��������
	bool m_bUpVertex;		//
	double rayVecX,rayVecY;	//
	bool m_bWeldEdge; 
	bool m_bRollEdge;				//�˱�����
	WELDLINE weldline;				//�������
	short manu_space;				//����ӹ�Ԥ����϶���߸߶�(mm)
	short roll_edge_offset_dist;	//���ƫ�ƾ��� wht 11-04-02
	double radius;					//��Բ������Ҫ�Ĳ���
	double sector_angle;			//��׼Բ������Ҫ�Ĳ���
	GEPOINT center,column_norm;	//��Բ������Ҫ�Ĳ�������Զ�����������),center�����ʾԲ��Բ��(ʰȡ��)ʱ���������������
	//�Խӵ�����ڳ�ʼ����ص�ǰ���ӱߵ�ƫ�������Ըֹ�ʼ->�շ���Ϊƫ������ֵ����
	double local_point_y;			//�Ի��λ�ã��ڸõ������ע
	long get_liLocationAlongOffset() const;
	long set_liLocationAlongOffset(long liAlongOffset);
	//�Խӵ�����ڳ�ʼ����ص�ǰ���ӱߵ�ƫ�������Ըֹ�ʼ->�շ���Ϊƫ������ֵ���� wjh-2019.11.02
	__declspec(property(put=set_liLocationAlongOffset,get=get_liLocationAlongOffset)) long liLocationAlongOffset;
	//���Ӱ����׼�ֹܼ�ĶԽ������ʶģʽ
	int local_point_vec;			//0.��ʾ�޶Ի�㣬1.��ʾ�Ի�������ע50-->100�ķ����뺸�Ӹ��˼���start-->end������ͬ��2.��ʾ�෴
	char get_ciWeldFlowDirection() const;
	char set_ciWeldFlowDirection(char ciFlowDirection);
	//��ͬlocal_point_vec����ʾ�����ӻ�׼�ֹ�ʼ->�յĶԽ�����ģʽ,0.��ʾ�޶Ի��;1.�Խ������ʶΪʼ->��;2.�Խ������ʶΪ��->ʼ
	__declspec(property(put=set_ciWeldFlowDirection,get=get_ciWeldFlowDirection)) char ciWeldFlowDirection;
	//�ְ��������ʱ,�����������Ա�����ֻ����¶�������
	BYTE m_cPosDesignType;			//0:�������˼����� 1:���߲ඥ�� 2:֫��ඥ�� 3:��������ʱ��һ�����߲ඥ�� 4:��������ʱ��һ��֫��ඥ��  
	//�ڲ����������ϣ��ڸְ�ֲ�����ϵ�µ�λ��ƫ�� wjh-2017.12.1
	struct LOCALOFFSET{
		BYTE ciOffMode;	//0.������ϵ��ƫ��|1.��ǰ����ƫ��|2.��󶥵�ƫ��
		float x,y;		//�˼����췽��ΪXֵ�������ľ�ΪYֵ��ʹ���нǱ�СΪ��������Ϊ�� wjh-2017.12.8
	public:
		bool Enabled(){return x!=0||y!=0;}
		__declspec(property(get=Enabled)) bool enabled;
		LOCALOFFSET(){ciOffMode=0;x=y=0;}
	}xOffset;
	long m_hRod;					//���������
	DECLARE_PROPERTY(double,fLengCutWidth);
	DECLARE_PROPERTY(double,fWingCutWidth);
	//bool b_AutoCalHuoqu;			//�Ƿ��Զ����㹲�߻���
	CLDSGeneralPlate* m_pBelongPlate;	//��ʱ�洢�����ڲ鿴������������Ϣ
	UCS_STRU* m_pTransCS;				//��ʱ�洢�����ڲ鿴��������ϵ�µĶ�������
	union TAG {
		UINT  uiTagValue;
		long  liTagValue;
		void* pTagObj;
	}xTag;	//�������ݴ����������ʱ����һЩ���壬�洢һЩ������Ϣ wjh-2019.9.4
public:
	PROFILE_VERTEX();
	PROFILE_VERTEX(COORD3D vertice,BYTE arctype=0);
	PROFILE_VERTEX(f3dPoint vertice,BYTE arctype=0);
	PROFILE_VERTEX(double x,double y,double z,BYTE arctype=0);
	void Initialize(double x,double y,double z,BYTE arctype=0);
	void CopyEdgeInfoTo(PROFILE_EDGEINFO* pEdgeInfo);
	void CopyEdgeInfoFrom(const PROFILE_EDGEINFO& edgeinfo);
	//ucsֻ����������Բ��ʱ�궨��Բ����������
	void RetrieveArcLine(f3dArcLine &arcline,f3dPoint endPoint,UCS_STRU *ucs);
	//����ְ��麸ʱ�Ķ�λ��ʶ��λ�ã��ں������ϣ�����λ�ü�����ʸ����Ӧת����ͬһ�������ϵ�·��ɽ���
	GEPOINT RetrieveWeldLocation(double* endPoint,double* tube_line_vec,double* datum_org=NULL);
	//�����ְ��麸���ϵĶ�λ��־��λ�ã������麸��־��������麸�߶���
	void AmendWeldLocationY(double* endPoint,double* tube_line_vec,double* datum_org=NULL);
	//pNextVertex���ڼ��Բ��һ����
	bool IsEqualWith(PROFILE_VERTEX* pCompVertex,PROFILE_VERTEX* pNextVertex=NULL,PROFILE_VERTEX* pCompNextVertex=NULL);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0,bool bParamPlateVertex=false,UCS_STRU* pPlateCS=NULL);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0,bool bParamPlateVertex=false);
	void SetBelongPlate(CLDSGeneralPlate* pBelongPlate,UCS_STRU* pWcs=NULL){m_pBelongPlate=pBelongPlate;m_pTransCS=pWcs;}
	void SetPosDesignType(BYTE cPosDesType,long hKey){m_cPosDesignType=cPosDesType;m_hRod=hKey;}
	//����
	DECLARE_PROP_FUNC(PROFILE_VERTEX)
	static void InitProfileVertexPropHashtable();
	static BOOL IsPropEqual(SUBOBJSET &selectObjs, long idProp);	//�������ֵ�Ƿ����
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);
	//int GetPropValueStr(long id,char* valueStr,UCS_STRU* pWcs=NULL,CLDSGeneralPlate* pBelongPlate=NULL);	//��������ID�õ�����ֵ�ַ���
};

//��������		wht 08-01-19
struct ATTACHPART_ITEM{
	long h,idClsType;
	char sPartNo[MAX_CHAR_GUIGE_16+1];
	ATTACHPART_ITEM(){strcpy(sPartNo,"");}
	void SetKey(DWORD key){h=key;}
};

//���ӷ�����:
typedef struct CONNECT_DIRECTION{
	long feature;	//���洢
	BYTE style;		//0:direct��ʾ����������;1:��ʾ�����湤������
	BYTE serial;	//���ڱ�ʶ�����ӷ�������һ��������Ӧ,һ��ȡֵ1��2,serial>=10ʱ������¼���η�����Ӧ�����Ӹ˼�
	CLDSVector direct;
	short end_space;	//��˨������ߵĶ˾�
	short high_space;	//��˨������߽Ǻ�����ͶӰ��ĸ߶�
	short tip_space;	//��˨�������֫���������ȣ���Ǹ�֫����
	//���ݻ�׼�㡢Y�᷽���Լ���ǰ���ӷ����ȡ���ӷ��������˨
	int GetRelaLsRefList(CLsRefList &boltList,IModel *pTower,f3dPoint axis_y,f3dPoint origin);
	CONNECT_DIRECTION(){
		feature=0;
		style=serial=0;
		end_space=high_space=tip_space=30;
	};
	CONNECT_DIRECTION(BYTE cStyle,CLDSVector vec,short nEndSpace,short nHighSpace){
		feature=0;
		style=cStyle;
		direct=vec;
		end_space=nEndSpace;
		high_space=tip_space=nHighSpace;
	};
	void SetKey(DWORD key){serial=(BYTE)key;}
	BOOL IsProfilePole(){return serial>=10;}
}*CONNECT_DIRECTION_PTR;

//������������
enum PARAM_PLATE_TYPE{
	TYPE_FL					= 2,	//������
	TYPE_UEND				= 3,	//U�Ͳ��
	TYPE_ROLLEND			= 4,	//��߲��
	TYPE_ELBOWPLATE			= 6,	//���
	TYPE_VERTPLATE			= 8,	//����Ӿ���(�ѷ�������)
	TYPE_ENDPLATE			= 9,	//��ͷ��(�ѷ�������)
	TYPE_CIRRIBPLATE		= 10,	//�����߰�
	TYPE_CIRCOVERPLATE		= 11,	//������ 
	TYPE_TUBERIBPLATE		= 12,	//�ֹ��߰�(��Խ���壬�𽥱�TYPE_CIRRIBPLATEȡ��)
	TYPE_LINEANGLERIBPLATE	= 13,	//�Ǹ��߰�
	TYPE_XEND				= 14,	//ʮ�ֲ��
	TYPE_FLD				= 15,	//�Ժ�����
	TYPE_FLP				= 16,	//ƽ������
	TYPE_WATERSHIELDEDPLATE	= 17,	//��ˮ��
	TYPE_RIBPLATE			= 18,	//�߰�(���߰壬���߰壬�Ǹ��߰壬�ְ��߰壬�ֹ��߰壬�ֹܼа�) wht 11-03-21
	TYPE_FLG				= 19,	//���Է���
	TYPE_FLR				= 20,	//���Է���
	TYPE_BASEPLATE			= 21,	//���ŵװ�
	TYPE_SHOEPLATE			= 22,	//����ѥ��
	TYPE_STATIONCONNPLATE	= 23,	//��繹�ܶ˳����������Ӱ�
	TYPE_STATIONSHOEPLATE	= 24,	//��繹����������ѥ�壨���˳�ʱ��ѥ�弫���ƣ�
	TYPE_STATIONCROSSPLATE	= 25,	//��繹�ܶ˳�������ʮ�ֲ��
	TYPE_STATIONLINKPLATE	= 26,	//��繹�ܶ˳��м����Ӱ�
	TYPE_STATIONNODEPLATE	= 27,	//��繹���������м����Ӱ�
	TYPE_TUBESTAYWIREPLATE	= 28,	//�ֹ����߰�
	TYPE_WALKWAYCIRPLATE	= 29,	//�ߵ��ļӾ�����
	TYPE_PEDALPLATE			= 30,	//��̤��
};

//�˽ṹ�е��������궼��չ���������ľ�������
struct CIRCULAR_PLATE_VERTEXSET{
	f3dPoint inner_arc_start,inner_arc_end;			//�ڻ��ؼ�����
	f3dPoint inner_arc_midhuoqu,weld_outer_huoqu;	//��Բ�����ⲿ���ɱ��ϵ������
	f3dPoint weld_outer_w,weld_outer_n;				//�������������˶���
	f3dPoint weld_outer_w_h,weld_outer_n_h;			//�������������˶���ƫ�Ƶ��Ǹߺ󶥵�
	f3dPoint weld_outer_w_arc,weld_outer_n_arc;		//�������������˶���Բ���˶���
};
struct PLATEEDIT_STATE{
	BYTE ciPickedDisplayMode;
	double m_fLocalWcsOrgX,m_fLocalWcsOrgY;
	double m_fLocalWcsAxisX_x,m_fLocalWcsAxisX_y;
	double m_fLocalWcsAxisY_x,m_fLocalWcsAxisY_y;
	PLATEEDIT_STATE(BYTE _ciPickedDisplayMode=1)
	{
		ciPickedDisplayMode=_ciPickedDisplayMode;
		m_fLocalWcsOrgX=m_fLocalWcsOrgY=0;
		m_fLocalWcsAxisX_x=1;m_fLocalWcsAxisX_y=0;
		m_fLocalWcsAxisY_x=0;m_fLocalWcsAxisY_y=1;
	}
};
class CLDSGeneralPlate : public CLDSPart{
protected:
	long _thick;			//���
	BYTE m_cDatumAxis;		//����ƽ���ϵ�����ϵ�����׼��ֻ���� 'X'��'Y'
	BYTE _cRollProcessType;	// 0.չ���󵹽Ǵ�Ϊ����ֱ��;1.���ٵ�����֤������߳���ΪL-N;2.���ٵ�����֤����ڲ�߳���ΪL-N
	BYTE _ciRollOffsetType;	//�ݲ��洢��ȡ�ļ�������Ӧ��ֵ������ְ����������趨������д洢
	BYTE _ciDeformType;		//�ݲ��洢��ȡ�ļ�������Ӧ��ֵ��
	double _fDeformCoef;	//���Բ�ϵ��
	double _fHuoquR;		//�����뾶
	union{
		bool m_bClockwiseCalProfile;	//ȥ�����㷨��true:��˳ʱ�뷽��;false:����ʱ�뷽����������
		BYTE m_cReverseAxisVec;			//��תָ���� 0.����ת  'X'.��תX��  'Y'.��תY��
	};
	//��0.��ʾ��ͨ�� 1.����� 2.���ð� 3.�۵��� 4.����˨���5.�����˨���
	//��0.��ʾ��ͨ�� 1.�۵��� 2.����� 3.�ֹܼа� 4.��˨��� 5.����ڻ�׼����λ�ò���ĸ��Ӱ� 6.���ð�
	union UNION_TYPE{
		UINT iLegacyStyle;		//�ɾ�_jdb_style�������� wjh-2017.1.21
		struct{
			WORD wiLegacyStyle;	//��ӦTMA V5.1.5֮ǰ��_jdb_style
			WORD wiType;		//��Ӧ��CLDSParamPlate::m_iParamType���Ϻ�ĸְ���Ʋ�������
		};
	}style;
	CLsRefList *pLsRefList;
	UINT m_uiInnerLoopBuffSize;
	char* m_pxInnerLoopBuff;	//loops.N(4)+loop(i).N+SIMPLE_VERTEX�洢��ʽ wjh-2019.9.7
	//ͨ��CLDSPart::HOLE��������˨�ף���������˨������������ף�ֱ�ӿ��������ɵĹ��տף����ϡ�wjh-2017.11.2
	virtual UINT AddMiscellaneousHoles(IXhList<HOLE>* pListHoles);	//��GetHoleSet�ڵ���,����ӳ�������˨����������
	void CalEquidistantShape(double minDistance,ATOM_LIST<PROFILE_VERTEX> *pList);
public:
	static BYTE m_ciDisplayVertexType;	//��ʾ�ְ����������� 0.����ʾ|1.��ʾ�����㼰����|2.ֻ��ʾ������(����ʾ����)
	static CHashList<PLATEEDIT_STATE> m_hashDisplayPlateStates;
	ATOM_LIST<PROFILE_VERTEX> vertex_list;				//�����Ķ�������꣨���������ϵ�£�
	ATOM_LIST<BOLTHOLE_INFO> hole_list;					//��������˨�����꣨���������ϵ�£�
	struct GASKETEDGESPACE{
		bool initialized;
		short endSpaceS,endSpaceE,berSpace,wingSpace;
		GASKETEDGESPACE(){endSpaceS=endSpaceE=berSpace=wingSpace=0;initialized=false;}
	}gasketEdgeSpace;
	bool IntelliRetrieveGasketEdgeSpace(GASKETEDGESPACE* pEdgeSpace=NULL,BOOL bInitPosDesType=TRUE);
	bool RetrieveGasketVertexCalType();
	CHashListEx<DESIGN_PLATE_ITEM>params;			
	char m_cPickedDisplayMode;	//��ʱ���洢����, ��ѡ�й�������ʾģʽ,0.��͸��������ʾ;1.����������ʾ
	struct DESIGN_PARAM
	{
		bool bExist;	//�Ƿ��ڲ��������ҵ��ò���
		BYTE cValueType;
		BYTE cParamType;
		WORD wItemKey;
		struct DUAL_VALUE{THANDLE hPart;UINT uiAttribute;};
		union{
			double fValue;
			struct {THANDLE hPart;UINT uiAttribute;};
			THANDLE hValue;
			long iValue;
		}value;
		void SetKey(DWORD dwKey){wItemKey=(WORD)dwKey;}
		READONLY_PROPERTY(double,fValue);
		GET(fValue){return ToDouble();}
		READONLY_PROPERTY(long,hValue);
		GET(hValue){return ToLong();}
		READONLY_PROPERTY(long,iValue);
		GET(iValue){return ToLong();}
		READONLY_PROPERTY(UINT,uiAttribute);
		GET(uiAttribute){
			if(cValueType==3)
				return value.uiAttribute;
			else
				return 0;
		}
	public:
		void Set(double fValue,bool exist=true);
		void Set(long iValue,bool exist=true);
		double ToDouble();
		long   ToLong();
		void CopyValue(const DESIGN_PARAM& item){bExist=item.bExist;value.fValue=item.value.fValue;}
		void InitBasicInfo(WORD wKey,BYTE cValType,BYTE cParamType=PARAM_SHAPE);
	};
	DESIGN_PLATE_ITEM *GetDesignItem(WORD wKey);
	virtual BOOL GetDesignItemValue(WORD wKey, double *pfValue,CHashTable<DESIGN_PLATE_ITEM*>*pTbl=NULL);
	virtual BOOL GetDesignItemValue(WORD wKey, long *piVal,CHashTable<DESIGN_PLATE_ITEM*>*pTbl=NULL);
	virtual BOOL GetDesignItemValue(WORD wKey, DESIGN_PARAM *pParamItem,CHashTable<DESIGN_PLATE_ITEM*>*pTbl=NULL);
public:
	CLDSGeneralPlate();
	~CLDSGeneralPlate();
	virtual bool GetPropParam(long idObjProp,PROPVAL* pVal=NULL);
	virtual bool SetPropParam(long idObjProp,PROPVAL& propval);
	virtual bool ProcessConstraintFromMasterObj(CLDSDbObject* pMasterObj,CONSTRAINT* pConstraint);
	virtual bool HasPreferShadowDirection(double* vShadowDirection){return false;}
	virtual CLDSGeneralPlate* CloneCopy(double offset,double* vUdfDirection);
	virtual CLDSGeneralPlate* ShadowCopy(double offset,double* vUdfDirection);
	virtual CLDSGeneralPlate* CloneCopy(GEPOINT vOffset,bool localAcsCoord=true);
	virtual CLDSGeneralPlate* ShadowCopy(double offset,bool localAcsCoord=true);
	virtual bool IsGeomMir(CLDSGeneralPlate *pPlate,MIRMSG mirmsg,BOOL bCheckProfile=TRUE,const double *srcGeomCenterVec=NULL);
	virtual bool IsEqualMirWith(CLDSGeneralPlate* pMirPlate,MIRMSG mirmsg);
	virtual bool OptimizeStructure();
	virtual bool StableCheck(ILog2File* pRptLog,DWORD dwCheckFlag=0xffff,
		double (*CallBackDialogUIFunc)(CLDSLineAngle* pAngle,double dfAxisNByBolts,
				DWORD *pdwCheckFlag)=NULL);
	struct EDGEBOLT{
		BOOL bToHuoquLine;		//
		long hBolt;				//�����ְ�߽����˨���
		double actualEndSpace;	//ʵ�ʵ���˨�߾�
		long needEndSpace;		//���涨��Ҫ����˨�߾�
		EDGEBOLT(){hBolt=0;actualEndSpace=0;needEndSpace=0;bToHuoquLine=FALSE;}
	};
protected://��ȡ�ְ���ӷ�������
	virtual WORD InternalGetDesignType();		//�ú�����������Ϊ�������ֱ��get=�����麯������ wjh-2017.1.20
	virtual void InternalSetDesignType(WORD wiDesignType);
	virtual PROFILE_VERTEX* InternalGet3PlanePlatePeakVertex(){return NULL;}
public:
	//1) �̳�ԭTMA�ĸְ���ʽCLDSPlate::jdb_style
	virtual WORD GetLegacyStyle();
	virtual void SetLegacyStyle(WORD wiLegacyStyle);
	PROPERTY(WORD,wiLegacyStyle); //��ӦTMA V5.1.5֮ǰ��CLDSPlate::jdb_style
	VIRTUAL_SET(wiLegacyStyle){SetLegacyStyle(value);}
	VIRTUAL_GET(wiLegacyStyle){return GetLegacyStyle();}
	//2) ��չ���ϲ�������������ͺ�ģ��������
	static const WORD CONNECT_PLATE = 100;	//���Ӱ�
	static const WORD WASHER_PLATE  = 101;	//��˨���
	static const WORD WRAPPER_PLATE	= 102;	//���ְ�
	static const WORD RIB_PLATE		= 103;	//��ǿ���߰�
	static const WORD ELSE_PLATE	=1000;	//�����
	PROPERTY(WORD,wiType);	//����CLDSParamPlate::m_iParamType��ĸְ����(����)����
	VIRTUAL_SET(wiType){InternalSetDesignType(value);}
	VIRTUAL_GET(wiType){return InternalGetDesignType();}

	static const BYTE VALIDATE_PROFILE_BASIC	 = 0x01;
	static const BYTE VALIDATE_PROFILE_EDGESPACE = 0x02;
	virtual BOOL ValidateProfile(double epsilon=1.0,EDGEBOLT *endBoltInfoArr=NULL,int *count=NULL,BYTE cbValidateFlag=0xff);
	virtual int IsEqualProfile(CLDSGeneralPlate* pMirPlate,BOOL bCompareLs=TRUE,double epsilon=1.0){return 1;}
	virtual bool CompareLabelPart(CLDSPart *pPeerPart,MIRMSG* pMirmsg=NULL,BOOL bCmpls=TRUE,PEER_PART *pPeerInfo=NULL,double epsilon=1.0);
	virtual void GetPeerPlateUcs(CLDSGeneralPlate *pPeerPlate,int base_i,BOOL bReverse,UCS_STRU& acs);
	virtual CLDSNode *GetBaseNode(bool bClearLocks=true){return NULL;}	//�������Ӱ�������ڵ�
	virtual long get_BaseNode(){return 0;}	//�������Ӱ�������ڵ�
	virtual long get_BaseRod() {return 0;}	//���ظְ��׼�ڵ�  wht 09-12-12
	__declspec(property(get=get_BaseNode)) long m_hBaseNode;
	__declspec(property(get=get_BaseRod))  long m_hBaseRod;
	virtual bool IsPtInPlate(double* pos_coord);
	virtual bool GetBendlineVertexies(PROFILE_VERTEX** ppBendLine1Start=NULL,PROFILE_VERTEX** ppBendLine1End=NULL,
		PROFILE_VERTEX** ppBendLine2Start=NULL,PROFILE_VERTEX** ppBendLine2End=NULL);
	virtual bool IntelliRecogWeldLine(PARTSET* pSearchSet=NULL);
	virtual bool IsWeldWith(double* lineStartCoord,double* lineEndCoord);
	virtual bool IsWeldWith(f3dArcLine& arcline);
	virtual bool IsConnectPlate();	//�ְ��Ƿ�Ϊ�и˼�����˨���ӵ����Ӱ�
	virtual bool IsPeakPointPlate(){return false;}	//�Ƿ�Ϊ�����ǵ�����塡wjh-2016.1.9
	virtual bool IsFoldPlate(){return false;}		//�Ƿ�Ϊ�����۵��塡wjh-2016.1.9
	virtual bool IsTubeFoldPlate();	//��繹���������۵��壨˫������Ϊ�����棬���淴�����ǹ��ɲ��������棩 wjh-2017.11.8
	virtual bool IsSplayFoldPlate();	//��繹�������������ϰ��ְ壨��׼�沢���ظֹ����� wjh-2017.11.8
	virtual bool IsMaybeBasePlate();	//�Ƿ�Ϊ���ŵ����壨������ӵؽ���˨��
	virtual void CalRollLineAndHuoquLine(PROFILE_VERTEX *pStartVertex,PROFILE_VERTEX *pEndVertex,PROFILE_VERTEX *pPreVertex,PROFILE_VERTEX *pNextVertex,
		f3dLine &huoqu_line,f3dLine &roll_line,BOOL *pbRetainRollLineStart=NULL,BOOL *pbRetainRollLineEnd=NULL);
	virtual bool ProcessRollEdgeVertex(ATOM_LIST<PROFILE_VERTEX> &vertex_list);
	virtual bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	virtual long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	//�����С������η�Χ�����
	virtual f2dRect GetMinWrapRect(UCS_STRU *Rectucs=NULL,double offset=0,BOOL bExportDeformedProfile=FALSE);
	virtual double CalAreaOfMinWrapRect(SCOPE_STRU *pScope=NULL);
	virtual double CalAreaOfNetWrap();
	virtual double CalPerimeter();		//����ְ��ܳ�,mm
	virtual double CalWeldEdgeLength();	//����ְ庸��߳�,mm
	virtual double CalWeldLineLength();	//����ְ�ʵ�ʺ����߳�
	virtual BOOL IsAngleRibPlate();		//�Ƿ�Ϊ�Ǹִ�����
	virtual BOOL IsBushWeldPlate();		//�Ƿ�Ϊ�׹ܺ��Ӱ�
	virtual long   GetEdgeNum(double lenGreatThan=0);	//����ְ��и�ȴ���ָ��ֵ(mm)�ı���
	virtual double GetNormOffset(){return 0;}	//��ȡ�ְ��������ڻ�׼ԭ��ķ���ƫ����
	virtual UCS_STRU GetUnifiedACS();
	virtual int GetFaceN(){ return 1;}							//�ְ�����
	virtual f3dLine GetHuoquLine(int index){return f3dLine();}	//��ȡָ������(0.��1��2��Ļ����� 1.��2��3��Ļ�ȡ��)��Ϊ������ϵ�µ��������
	virtual f3dPoint GetHuoquFaceNorm(int faceI){return f3dPoint();}//��ȡָ������ķ��ߣ�Ϊ��������
	virtual int GetNormFaceI(CLDSBolt* pBolt);//����һ���������������ش��������ڵİ�ƽ���0,1,2
	virtual int GetNormFaceI(const double* vCurrNormal);	//����һ���������������ش��������ڵİ�ƽ���0,1,2
	virtual int GetFaceNoByPos(GEPOINT pos);	//����һ��������(չ�����ְ�����ϵ)�����ش��������ڵİ�ƽ���
	virtual int WriteProcessBuffer(CBuffer &buffer,bool bSaveSubSerial=false,long version=0); //�õ�����Ǹֹ�����չ��Ϣbuffer
	virtual BOOL GetProfileByBoltHoles(ATOM_LIST<f3dPoint>& pt_list);
	virtual int CompareProfileVertex(ATOM_LIST<f3dPoint>& ptList1,ATOM_LIST<f3dPoint>& ptList2,BOOL bReverseOrder,int base_i,double epsilon);
	virtual int CompareVertexOnLine(ATOM_LIST<f3dPoint>& ptList1,ATOM_LIST<f3dPoint>& ptList2,BOOL bReverseOrder,double epsilon);
	virtual bool IsEqualProfile(ATOM_LIST<f3dPoint>& ptList1,ATOM_LIST<f3dPoint>& ptList2,double epsilon,int& base_i,BOOL& bReverse);
	virtual int GetAllBoltHoles(ATOM_LIST<BOLTHOLE_INFO>& holeList);
#ifdef __LDS_CONTEXT_
	virtual bool CloneToBomPart(BOMPART* pBomPart);
#endif	
	virtual double GetHuoQuAngle(int iFace,f3dLine *pHuoQuLine=NULL)=0;
	//Ҫ������Ķ�������Ǵ����Ӱ��ϵĶ���,����
	//���ܲ�����ת����ת������(��չ���������-->ʵ���������)
	//bCalVertex=true��ʾ��ȡ����false��ʾ��ȡʸ������
	virtual f3dPoint GetRealVertex(f3dPoint vertice,bool bCalVertex=true)=0;//{return vertice;}
	//���������ϵľ������꣨����-->չ�����ְ���ԣ�װ�䣩����ϵ;
	//bSpreadVertex=true��ʾչ������false��ʾչ��ʸ������
	virtual f3dPoint GetSpreadVertex(const double* V,int face_i=0,bool bSpreadVertex=true)=0;//{return V;}
	//Ҫ���������˨�����Ǵ����Ӱ��ϵ���˨,����
	//���ܲ�����ת����ת������(��ʵ�ʾ�������-->չ���������)
	virtual BOOL SpreadLsPos(CLDSBolt *pLs,double* ls_pos){return FALSE;}
	//Ҫ������Ķ�������Ǵ����Ӱ��ϵĶ���,����
	//���ܲ�����ת����ת������(��δ���Ǳ��εĳ�ʼ��ת��Ϊ���Ǳ��εĵ�)
	virtual f3dPoint GetDeformedVertex(f3dPoint vertice);
	//�����ְ�����ϵ�����������ε�����ʱʹ�� new_ucsΪ�����������ڵ��������ϵ wht 10-12-08
	virtual f3dPoint GetDeformedVertex(f3dPoint vertice,UCS_STRU new_ucs);
	//iResidePlane=0,1,2,�ֱ��ʾ��׼�桢�������桢��������
	static const int TOP_PLANE	= 2;	//����ְ嶥������˨���߽���
	static const int MID_PLANE	= 3;	//����ְ����Բ�����˨���߽���
	virtual bool GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* iResidePlane=NULL,int iIntersPlane=0){return false;}
	//GetBoltIntersPos����˳�򲻺���(��Ϊ��ѡ�������Ӧ�������)�����ڴ���ά����ȫ�����Ժ�Ӧ����CalBoltIntersPosȡ�� wjh-2016.5.7
	virtual bool CalBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int iIntersPlane=0,int* iResidePlane=NULL){
		return GetBoltIntersPos(pBolt,inters_coords,iResidePlane,iIntersPlane);
	}
	//cbFlag��FROM_MCS��FROM_ACS��IS_VECTOR��IS_VERTEX������ϣ�Ĭ��ΪFROM_MCS|IS_VERTEX
	//iResidePlaneȡֵ:1��2��3��ʾ��ת�����ʸ����פ������ wjh-2015.2.27
	static const BYTE FROM_MCS =0x00;	//coord3dΪģ������
	static const BYTE FROM_ACS =0x01;	//coord3dΪ��ǰ�ְ���װ������(���������)
	static const BYTE IS_VERTEX=0x00;	//coord3dΪλ�õ�, ִ��λ������ת��
	static const BYTE IS_VECTOR=0x02;	//coord3dΪ������, ִ��ʸ������ת��
	virtual f3dPoint SpreadToACS(const double* coord3d,int iResidePlane=1,BYTE cbFlag=0){return f3dPoint(coord3d);};
	static const BYTE TO_MCS =0x04;	//ת����ģ������
	static const BYTE TO_ACS =0x08;	//ת����Ϊ��ǰ�ְ���װ������(���������)
	virtual GEPOINT TransToActual(const double* coord3d,int iResidePlane=1,BYTE cbFlag=0)=0;
	//��˨������
	virtual int GatherHoleWalls(CHash64ListEx<HOLE_WALL>& hashHoles);
	virtual bool UpdateHoleWall(HOLE_WALL* pHoleWall);	//����˨��ְ巢���仯ʱ��ʵʱ���¿ױ�
	virtual CLsRefList *GetLsRefList(){return pLsRefList;}
	CLsRef* GetFirstLsRef(){return pLsRefList->GetFirst();}
	CLsRef* GetNextLsRef(){return pLsRefList->GetNext();}
	virtual UINT GetHoleSet(IXhList<HOLE>* pListHoles,bool bOnlyBoltHole=false);	//����ֵΪ������
	virtual int GetLsCount(short d=0);
	virtual CLsRef* AppendLsRef(CLsRef &ls_ref,BOOL bNearLsCheck=TRUE);
	void EmptyLsRef(){ pLsRefList->Empty();}
	CLsRef* FindLsByUcsPos(f3dPoint base,UCS_STRU ucs,int iNormX0Y1Z2,double epsilon=EPS2)
	{return pLsRefList->FindLsByUcsPos(base,ucs,iNormX0Y1Z2,epsilon);}
	CLsRef* FindLsByPos(f3dPoint base,f3dPoint work_norm,double epsilon=EPS2)
	{return pLsRefList->FindLsByPos(base,work_norm,epsilon);}
	virtual CLsRef* FindLsByHandle(THANDLE h){return pLsRefList->FromHandle(h);}
	virtual double GetHuoQuR(int iFace=0);
	virtual double GetDeformCoef(int iFace=0);
	virtual void SetHuoQuR(double r=0,int iFace=0){_fHuoquR=r;}
	virtual void SetDeformCoef(double deform_coef=0.35,int iFace=0){_fDeformCoef=deform_coef;}
	//������˨�ײ���
	BOLTHOLE_INFO* EnumFirstBoltHole(){return hole_list.GetFirst();}
	BOLTHOLE_INFO* EnumNextBoltHole(){return hole_list.GetNext();}
	//ͬ������
	static const DWORD PROP_DISCONNECT	= 0;	//�Ͽ�
	static const DWORD PROP_SYNC_COPY	= 1;	//ͬ���Գ�
	static MEMORY_BLOCK MPCL;	//Mirror Property Control List
	static PROPLIST_TYPE propMirSync;
	static void InitPropMirBitsMap();
	virtual PROPLIST_TYPE* GetSyncPropList(){return &propMirSync;}
	virtual MEMORY_BLOCK GetDefaultMPCL(){return MPCL;}
	virtual bool IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr);
	virtual bool SyncMirProp(const char* propKeyStr=NULL,BYTE ciMirAll0X1Y2Z3=0,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ������
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ��λ�öԳ��޹ص�����
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
public:
	DECLARE_PROPERTY(BYTE,cDatumAxis); //����ƽ���ϵ�����ϵ�����׼��ֻ���� 'X'��'Y'
	PROPERTY(bool,bClockwiseCalProfile);
	GET(bClockwiseCalProfile){return m_bClockwiseCalProfile;}
	SET(bClockwiseCalProfile){m_bClockwiseCalProfile=value;}
	PROPERTY(BYTE,m_cRollProcessType);
	GET(m_cRollProcessType){
		if (_cRollProcessType==0xFF)
			return  BelongModel()->GetRollProcessType();
		else 
			return _cRollProcessType;
	}
	SET(m_cRollProcessType){_cRollProcessType=value;}
	PROPERTY(BYTE,m_ciRollOffsetType);
	GET(m_ciRollOffsetType){
		if (_ciRollOffsetType==0xFF)
			return  BelongModel()->GetRollOffsetType();
		else 
			return _ciRollOffsetType;
	}
	SET(m_ciRollOffsetType){_ciRollOffsetType=value;}
	PROPERTY(BYTE,m_ciDeformType);
	GET(m_ciDeformType){
		if (_ciDeformType==0xFF)
			return  BelongModel()->GetDeformType();
		else 
			return _ciDeformType;
	}
	SET(m_ciDeformType){_ciDeformType=value;}
	DECLARE_PROPERTY(double,m_fDeformCoef);
	DECLARE_PROPERTY(double,m_fDeformCoef2);
	DECLARE_PROPERTY(double,m_fHuoquR);
	DECLARE_PROPERTY(double,m_fHuoquR2);
};
class DYNA_PROPITEM_GROUP{
public:
	//static CXhChar100 HELPLINK_URL;
	DWORD m_idGroup;	//�����ʶ
	CHashStrList<PROPLIST_ITEM>propHashtable;
	CHashSet<PROPLIST_ITEM*> hashPropItemById;
	CHashTable<DWORD>propStatusHashtable;
	//static int GetPropStr(long id, char *propStr);
	//static CXhChar50 GetPropName(long id);
	DYNA_PROPITEM_GROUP(DWORD key){m_idGroup=key;}
	long GetPropID(const char* propStr);
	int GetPropStatus(long id);
	BOOL SetPropStatus(long id,BOOL bHide);
	PROPLIST_ITEM* GetPropItem(long id);
	PROPLIST_ITEM* AddPropItem(char* propKey,PROPLIST_ITEM item);
	PROPLIST_ITEM* UpdatePropItem(char* propKey,char* propName=NULL);
	//BOOL IsPropEqual(CXhPtrSet<cls_name> &objSet,long idProp);
};
struct WELDCOVER_PLATE : public ICompareClass{
	CLDSGeneralPlate* pWeldPlate;
	double alfa;	//�����ƣ�
	double innerR;	//�������Ǵ�������ˮ�װ뾶
	GEPOINT vxRayVec;
	WELDCOVER_PLATE(CLDSGeneralPlate* pRelaPlate=NULL,double _innerR=0){
		pWeldPlate=pRelaPlate;
		innerR=_innerR;
		alfa=0;	//��Ҫ��������
	}
	virtual int Compare(const ICompareClass *pCompareObj)const
	{
		WELDCOVER_PLATE* pCover2=(WELDCOVER_PLATE*)pCompareObj;
		if(alfa>pCover2->alfa)
			return -1;
		else if(alfa<pCover2->alfa)
			return  1;
		else
			return 0;
	}
};
class CLDSStdPartModel;
class CLDSParamPlate : public CLDSGeneralPlate{
	//�˽ṹ�е��������궼��չ���������ľ�������
	CIRCULAR_PLATE_VERTEXSET vertexset;	//��ͼʱ��Ҫʹ�øýṹ�����ñ�����ʱʹ�� wht 110-05-30
	struct CIRCULAR_PLATE_KEYVEC{
		f3dPoint huoqufacenorm;			//������Ĺ�������(��������)
		f3dPoint huoquaxis_vec;			//�����߷���,�ƴ�����ʱ����ת<180��ǿɽ����淨����ת�������淨��
		f3dPoint weldvec_w,weldvec_n;	//��������������(��������)
		f3dPoint weldvec_w_vert,weldvec_n_vert;	//���������촹�߷���(��������,��ָ�����)
		f3dPoint weldvec_nw_vert;		//�ڶ����������촹�߷����ڻ����ڵ�չ������
		f3dPoint weldvec_nw;			//�ڶ�����������������ʱ���������ڻ����ڵ�չ������
		f3dPoint freevec,freevec_vert;	//���ɱ߷���,���ɱߴ��߷���
	}keyvecset;	//���ڱ��������Ϣ wht 11-04-11
	union RIBPROFILE_PARA{
		struct ELBOW{
			double fBottomWidth;//�ײ����
			double fInnerLen;	//�ڲ೤��
			double fTopWidth;	//�������
			double fOuterLen;	//��೤��
			double fCutAngleLen;//�нǿ��
		}common;//���
		struct GENERAL{
			double A;			//�߰�߶�fBottomWidth;
			double B;			//�߰峤��fInnerLen;	
			double C;			//̨�׸߶�fTopWidth;	
			double D;			//�������fOuterLen;	
			double E;			//�нǿ��fCutAngleLen;
		}corner;	//�����߰弰�������߰�
		void ClearZero(){corner.A=corner.B=corner.C=corner.D=corner.E=0.0;}
	};
	//�������ζ���������ӷ���
	BOOL CalProfileVertexByConnVector(CONNECT_DIRECTION *pVec,CLDSNode *pBaseNode);
protected:
	int _iParamType;	//PARAM_PLATE_TYPE
	/*��������ľ�����ʽ����
	�߰壺��ͨ�͡��Ǹ��߰塢�����߰塢�����߰�
	����壺��ͨ�͡�����͡�ֱ���Ͳ���֧�š�Բ���Ͳ���֧�š�ֱ���ͽ���֧�š�Բ���ͽ���֧�š���ˮ��
	*/
	long _iStyle;
	BOOL _bStdPart;	//TRUE:��׼���� FALSE:�Ǳ�׼����
public:	//��������
	PROPERTY(int,m_iParamType); 
	GET(m_iParamType){
		CLDSParamPlate* pMotherObj=(CLDSParamPlate*)MotherObject();
		if(pMotherObj==this)
			return _iParamType;
		else
			return pMotherObj->m_iParamType;
	}
	SET(m_iParamType){
		CLDSParamPlate* pMotherObj=(CLDSParamPlate*)MotherObject();
		_iParamType = value;
		if(pMotherObj!=this)
			pMotherObj->m_iParamType=value;
	}
	PROPERTY(long,m_iStyle); 
	GET(m_iStyle){
		CLDSParamPlate* pMotherObj=(CLDSParamPlate*)MotherObject();
		if(pMotherObj==this)
			return _iStyle;
		else
			return pMotherObj->m_iStyle;
	}
	SET(m_iStyle){
		if(value>=0)
		{
			CLDSParamPlate* pMotherObj=(CLDSParamPlate*)MotherObject();
			_iStyle = value;
			if(pMotherObj!=this)
				pMotherObj->m_iStyle=value;
		}
	}
	PROPERTY(double,Thick); 
	VIRTUAL_SET(Thick);
	VIRTUAL_GET(Thick);
	//��CLDSPart::Thick����ͳһ���������׳��ִ����д���� wjh-2014.12.31
	PROPERTY(long,thick); 
	GET(thick){ return ftoi(Thick); }
	SET(thick){ Thick=value; }

	PROPERTY(BOOL,m_bStdPart); 
	GET(m_bStdPart){
		CLDSParamPlate* pMotherObj=(CLDSParamPlate*)MotherObject();
		if(pMotherObj==this)
			return _bStdPart;
		else
			return pMotherObj->m_bStdPart;
	}
	SET(m_bStdPart){
		CLDSParamPlate* pMotherObj=(CLDSParamPlate*)MotherObject();
		_bStdPart = value;
		if(pMotherObj!=this)
			pMotherObj->m_bStdPart=value;
	}
public:
	struct PROP_PARAMITEM
	{
		WORD wItemKey;
		CXhChar50 strPropName,strPropHelp;
		void Set(DWORD key){wItemKey=(WORD)key;}
		PROP_PARAMITEM(const char* propName="",const char* propHelp="")
		{
			strPropName.Copy(propName);
			strPropHelp.Copy(propHelp);
		}
	};
	bool IsElbowRibPlate();	//�Ƿ�Ϊ�ֹ�������߰壬Ϊͳһ�����������߰���� wjh-2015.8.6
	struct RIBPLATE_CORNER{	//���߰�-��ͨ�����Ľ��߰�(��Ǹ������߰�)
		static const BYTE SUB_TYPE = 1;
		static CHashListEx<PROP_PARAMITEM> hashPropItems;
	public:
		DESIGN_PARAM hDatumPlate;	//��Ӧ�����е�'A'����
		DESIGN_PARAM hNode;			//��Ӧ�����е�'B'����
		DESIGN_PARAM hAssisPlate1;	//��Ӧ�����е�'C'����
		DESIGN_PARAM hAssisPlate2;	//��Ӧ�����е�'D'����
		DESIGN_PARAM hBolt;			//'O'����
		DESIGN_PARAM hVecPart;		//'V'����
		DESIGN_PARAM fOffsetX;		//'X'����
		DESIGN_PARAM fOffsetZ;		//'Z'����
		DESIGN_PARAM blSideEdgeLength;	//�Ƿ���ݸ߶ȼ����߳���A
		DESIGN_PARAM A,B,C,D,E,F,G,H,I;	//�߰�ʾ��ͼ�еĸ����β���
		DESIGN_PARAM Ox;			//"Ox";
		DESIGN_PARAM Oy;			//"Oy";
		DESIGN_PARAM hRelatedAngle;	//"Pa";
		DESIGN_PARAM rayAngle;		//"Ra";
		DESIGN_PARAM rayDist;		//"Rd"
		DESIGN_PARAM Xx;//,&VXx
		DESIGN_PARAM Xy;//,&VXy
		DESIGN_PARAM fGradientDegAngle;//"Ga"
		DESIGN_PARAM e;	//����ģʽ��0.�н� 1.�л�		wxc-2016.12.20
		void LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl=NULL);
	};
	struct RIBPLATE_ELBOW{	//���߰�-������߰�
		static const BYTE SUB_TYPE = 2;
		static CHashListEx<PROP_PARAMITEM> hashPropItems;
	public:
		DESIGN_PARAM A,B,C,D,E,F,G,H;
		DESIGN_PARAM J;			//����λ�ǵ���ת��ʼ�����ͣ�0.��׼��;1.������;2.��λ��˨;3.ָ��(ȫ��)����;4.�װ�ֲ������
		DESIGN_PARAM hBtmPlate;	//�ײ���׼�ְ���,����塮L��������Ӧ
		DESIGN_PARAM hDatumTube;//���򿿽Ӹֹܾ��,����塮K��������Ӧ
		DESIGN_PARAM rotAngle;	//����λ�ǵ���ת��ʼ����������׼�߻򺸵��ߵĶ��ⶨλƫת��,����塮Y��������Ӧ����
		DESIGN_PARAM hDatumBolt;//2.��λ��˨
		// 3.ָ��(ȫ��)����
		// 4.���ױ�(X��)�ڵװ��ϵ����췽���ʶ��ľֲ�����
		DESIGN_PARAM vAxisXVecX,vAxisXVecY,vAxisXVecZ;
		DESIGN_PARAM margin;	//-1.Ĭ��ָ���ױ߳���;>=0.���쵽�װ��Ե(����)
		DESIGN_PARAM V;	//0.�ظֹ�������壻1.��ֱ���Զ���λ�ĸְ�
		DESIGN_PARAM R;	//�ײ��⻷�뾶
		DESIGN_PARAM R1;	//��ˮ��1�뾶
		DESIGN_PARAM R2;	//��ˮ��2�뾶
		DESIGN_PARAM Z;	//-1.�ְ���沼�����; 1.�ְ嶥�沼�����
		DESIGN_PARAM b;	//0.�ɰ�����; 1.2015��8�¹淶���°����� wjh-2015.8.08
		DESIGN_PARAM e;	//����ģʽ��0.�н� 1.�л�		wxc-2016.12.20
		void InitPropItemHashtable();	//��ʼ������߰�������Ļ�����Ϣ(���ݼ�ֵ�����͡��������Ƽ�������ʾ��
		void LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl=NULL);
	};
	struct RIBPLATE_ANGLE{	//�Ǹ��߰�
		//long A=0,B=0,I=0,C=0;
		//double L,W,S,D=0,wing_thick=0,H=0,R=0,d=0,P=0;
		DESIGN_PARAM A,B,C,I;
		DESIGN_PARAM L,W,S,D,wing_thick,H,R,d,P,E;
		DESIGN_PARAM Ly,Wy,Hy,N,Nx,Ny,Nz;
		void LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl=NULL);
	};
	struct RIBPLATE_TUBE{	//�ֹ��ϵľ����߰�
		static CHashListEx<PROP_PARAMITEM> hashPropItems;
	public:
		DESIGN_PARAM J;			//����λ�ǵ���ת��ʼ�����ͣ�0.��׼�ߣ�1.������
		DESIGN_PARAM Nt;		//�������ͣ�0.�ְ����� 1.�ְ����� 2.�Զ���
		DESIGN_PARAM hNode;		//��Ӧ�����е�'B'����
		DESIGN_PARAM hDatumTube;//���� 'A'
		DESIGN_PARAM rotAngle;	//����λ�ǵ���ת��ʼ����������׼�߻򺸵��ߵĶ��ⶨλƫת��,����塮Y��������Ӧ����
		DESIGN_PARAM W,H;	//�������봹��߶�
		DESIGN_PARAM Lt,Rt;	//����X+/X-Բ��(�н�)����
		DESIGN_PARAM Lw,Lh,Rw,Rh; //����X+/X-Բ��(�н�)�뾶
		DESIGN_PARAM DM,DX;
		DESIGN_PARAM Nx,Ny,Nz,Wx,Wy,Wz;
		//long A=0,B=0,I=0,C=0;
		//double L,W,S,D=0,wing_thick=0,H=0,R=0,d=0,P=0;
		DESIGN_PARAM L,S,D,wing_thick,R,d,P;
		void LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl=NULL);
	};
	struct RIBPLATE_STAY_WIRE{	//�ֹ��ϵ����߰�
		static CHashListEx<PROP_PARAMITEM> hashPropItems;
	public:
		DESIGN_PARAM J;				//����λ�ǵ���ת��ʼ�����ͣ�0.��׼�ߣ�1.������
		DESIGN_PARAM hDatumTube;	//���� 'A'
		DESIGN_PARAM hAssisPlate;	//���� 'C'
		DESIGN_PARAM rotAngle;		//����λ�ǵ���ת��ʼ����������׼�߻򺸵��ߵĶ��ⶨλƫת��,����塮Y��������Ӧ����
		DESIGN_PARAM L,W,H;			//�������봹��߶�
		DESIGN_PARAM P,R,d,N;		//���߿���Ϣ
		void LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl=NULL);
	};
	struct RIBPLATE_PLATE{	//�ְ��߰�
		static CHashListEx<PROP_PARAMITEM> hashPropItems;
	public:
		DESIGN_PARAM hDatumPlate;//���򿿽Ӹֹܾ��,����塮A��������Ӧ
		DESIGN_PARAM W,H;	//�������봹��߶�
		DESIGN_PARAM hStartBolt;//��ӦO�������׼��˨
		DESIGN_PARAM Ox,Oy;
		DESIGN_PARAM hVecPart;	//O->E�������׼�˼���E���Ӧ��׼��˨���hEndBolt
		DESIGN_PARAM Vx,Vy;		//O->E�������û����붨��ֵ
		DESIGN_PARAM OffsetX,OffsetZ;
		DESIGN_PARAM fGradientDegAngle;//"Ga"
		//��չ���ܺ���������� wjh-2016.5.4
		//O�㼰E���ΪhDatumPlate�ְ��µľֲ�����
		DESIGN_PARAM Ex,Ey;			//O->EΪ�߰��׼�������ʼĩ����
		DESIGN_PARAM hTipBolt;		//������ĩ��T��Ļ�׼��˨
		DESIGN_PARAM Bx,By;			//O->EΪ�߰��׼�������ʼĩ����
		DESIGN_PARAM Tx,Ty;			//������ĩ��T����û����붨��ֵ
		DESIGN_PARAM hBendVecPart;	//B->TΪ�������߰�����ն˻�����B->������ĩ��T��
		DESIGN_PARAM TipOffsetX,TipOffsetZ;
		void LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl=NULL);
	};
public:	//������Ըְ�Ϊ�װ�Ĳ������߰�ʱʹ����������
	struct BASERIBPLANE{	//�洢ģ������
		int iResidePlane;	//-1��ʾδ��ʼ��
		GEPOINT origin;
		GEPOINT vpBendPoint;
		GEPOINT direction,norm;
		BASERIBPLANE(){iResidePlane=-1;}
	};
	struct BENDRIBPLANE{	//�洢ģ������
		int iResidePlane;	//-1��ʾδ��ʼ��
		GEPOINT vpTipPoint,pickPoint;	//����㼰������ĩ�˶���
		GEPOINT direction,norm;
		BENDRIBPLANE(){iResidePlane=-1;}
	};
	//BYTE m_cPlateType;	//�߰����� 0.���߰� 1.���߰� 2.�Ǹ��߰� 3.�ְ��߰� 4.�ֹ��߰�
public:
	CHashListEx<ATTACHPART_ITEM> AttachPart;
	CLDSVector heteroFlConnVec[3];	//���´����ͷ����������˼����ӷ���
	CHashListEx<CONNECT_DIRECTION> ConnDirectList;
	long m_iLsRayNo;	//��˨���ڵ����ߺ�
	BOOL m_bAutoCalParamPlateUcs;	//TRUE:�Զ��������������ϵ FALSE:ָ������������ϵ
	CLDSVector workNorm;	//�������ߵ���Ʋ���
	struct DESIGN{
		bool inited;	//����origin��δ�浽�����ļ���,��ʱ��Ӵ����Ա�ʾ�Ƿ���Ϊorigin,alongvec���и�ֵ wjh-2016.9.6
		CLDSPoint origin;
		CLDSVector alongvec;	//���Ӱ��������
	}design;
public:
	//1.��ʾ��С���ݾ���ͳ��;2.��ʾ�����ͳ��(ȥ���ڻ�)
	static int m_iCirclePlankAreaType;
	//��׼�������
	double m_fNormOffset;	//�ְ��׼�����������Ĭ��λ�õķ���ƫ�ľ�
	void SetNormOffset(double fNormOffset);	//fNormOffsetΪ�ְ��������ڻ�׼ԭ��ķ���ƫ����
	virtual double GetNormOffset();		//��ȡ�ְ��������ڻ�׼ԭ��ķ���ƫ����
	virtual long get_BaseNode();//�������Ӱ�������ڵ�
	virtual long get_BaseRod();	//���ظְ��׼�ڵ�  wht 09-12-12
	virtual bool HasPreferShadowDirection(double* vShadowDirection);
	virtual bool IsBelongTo(int iClassTypeId)const;
	virtual CLDSPart* NewCopy(bool bByShadowMode=false);
	virtual CLDSGeneralPlate* CloneCopy(double offset,double* vUdfDirection);
	virtual CLDSGeneralPlate* ShadowCopy(double offset,double* vUdfDirection);
	virtual CLDSGeneralPlate* CloneCopy(GEPOINT vOffset,bool localAcsCoord=true);
	virtual CLDSGeneralPlate* ShadowCopy(double offset,bool localAcsCoord=true);
	double GetOutterPlaneOffset(const f3dPoint &outter_vec);
	CLDSParamPlate();
	~CLDSParamPlate();
	void SetPartNo(const char *part_no,bool bIncStdPart=false);		//�Զ����ʱ����ʹ��ͬһ�������ñ�ţ���������޸ı�׼�������	
	bool IsPlate() const{return true;}
	bool IsZhiWan();
	double GetWidth();				//���չ��ͼ���
	double GetLength();				//���չ��ͼ����
	double GetPrjDrawingWidth();	//���ͶӰͼ���
	double GetPrjDrawingLength();	//���ͶӰͼ����
	double GetWeight(BOOL bSolidLength=TRUE);
	virtual CLDSNode *GetBaseNode(bool bClearLocks=true);	//���ظְ��׼�ڵ�  wht 09-12-12
	void CopyProperty(CLDSDbObject *pObj);
	virtual bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	virtual bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:��ʾ������Ӽ���������;true:��ʾ����Ӽ��������� wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	BOOL IsCanManualModify();	//�ж��Ƿ�֧���ֶ��޸�
	BOOL UpdateProfileVertex(ATOM_LIST<PROFILE_VERTEX> &src_vertex_list);	//���²�����������������
	BOOL DesignPlate(bool bDesignBolt=true);	//��ĳЩ�������˨�ǵ�����ƣ�Ϊ�ӿ��ٶȲ���Ҫ���������˨λ��
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	virtual CLDSPart *GetMirPart(MIRMSG mirmsg,const double *pPickPos=NULL,BOOL bCheckProfile=TRUE);
	//static const BYTE VALIDATE_PROFILE_BASIC	 = 0x01;
	//static const BYTE VALIDATE_PROFILE_EDGESPACE = 0x02;
	virtual BOOL ValidateProfile(double epsilon=1.0,EDGEBOLT *endBoltInfoArr=NULL,int *count=NULL,BYTE cbValidateFlag=0xff);
	virtual int IsEqualProfile(CLDSGeneralPlate* pMirPlate,BOOL bCompareLs=TRUE,double epsilon=1.0);
	bool IsEqual(CLDSParamPlate* pMirPlate,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	BOOL IsValidProfile(double epsilon=1.0);	//����������Ƿ�Ϸ���Ŀǰ��Ҫ���߾� wht 11-04-16
	//��Ʋ����б���
	void EmptyDesignItems(){params.Empty();}
	char* GetDesignItemNotes(WORD wKey);
	BOOL IsHasDesignItemValue(WORD wKey);
	BOOL DeleteDesignItem(WORD wKey);
	//BOOL SetDesignItemValue(char cKey,double fValue,char *sNotes=NULL);
	BOOL SetDesignItemValue(WORD wKey,double fValue,char *sNotes=NULL,int iParamType=PARAM_POS,int iDatatype=0,BOOL bModify=FALSE);
	BOOL SetDesignItem(WORD wKey,DESIGN_PARAM& param,char *sNotes=NULL,int iParamType=PARAM_POS,BOOL bModify=FALSE);
	BOOL GetStdPartParamValue(WORD wKey, double *pfValue);	//�õ���׼������Ӧ��Ʋ�����ֵ
	//���ӹ����б���
	CHashListEx<ATTACHPART_ITEM> *GetAttachPartList(){return &AttachPart;}
	ATTACHPART_ITEM* GetFirstAttachPart(){return AttachPart.GetFirst();}
	ATTACHPART_ITEM* GetNextAttachPart(){return AttachPart.GetNext();}
	ATTACHPART_ITEM* AppendAttachPart(CLDSDbObject *pObj);
	BOOL DeleteAttachPartItem(long handle);
	int GetAttachPartCount(){return AttachPart.GetNodeNum();}
	void EmptyAttachPartList(){AttachPart.Empty();}
	BOOL IsInAttachPartList(long h);
	//�õ�ʮ�ֲ�������������� wht 10-07-06
	BOOL GetVertPlateInXEndPlate(CLDSPlate **pUpPlate,CLDSPlate **pDownPlate);
	BOOL DesignStdPartUcs();
	bool GetStdPartModel(CLDSStdPartModel *pStdPartModel);
	double GetThick();
	//��¡�������ְ�
	void CloneTo(CLDSParamPlate &plate);
	virtual bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	virtual long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	//����RIBPLATE_ELBOW��ʼ�����ʽ�߰����Ʋ���
	void ShiftElbowAndRibPlateSchema(int iRibPlateSchema=TYPE_RIBPLATE);
	static const int TYPE_RIB_PLATE_ON_PLATE  = 1;		//�ְ�����߰�
	static const int TYPE_RIB_PLATE_ON_ANGLE  = 2;		//�Ǹ��߰�
	static const int TYPE_RIB_PLATE_ON_TUBE   = 3;		//�ֹܾ����߰�
	static const int TYPE_RIB_ELBOW_PLATE	  = 4;		//������߰�
	static const int TYPE_RIB_CORNER_PLATE	  = 5;		//���߰�
	static const int TYPE_RIB_SLOT_PLATE	  = 6;		//���߰�
	int GetRibPlateSubType();
	//
	static const int TYPE_COMMON_CIR_PLATE	=1;	//��ͨ�����
	static const int TYPE_3WL_CIR_PLATE		=2; //3WL�����
	static const int TYPE_3WA_CIR_PLATE		=3;	//3WA�����
	static const int TYPE_2WL_CIR_PLATE		=4;	//2WL�����
	static const int TYPE_2WA_CIR_PLATE		=5;	//2WA�����
	static const int TYPE_SPECIAL_CIR_PLATE =6;	//���ͷ���
	static const int TYPE_CIR_AISLE_PLATE	=7;	//�����ߵ���
	int GetCircularPlateSubType();
	//���Թ���
	DECLARE_PROP_FUNC(CLDSParamPlate);
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//�������ֵ�Ƿ����
	//
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//�õ��������Ե�Ĭ��ֻ��״̬
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//�õ��������Ե�Ĭ��ֻ��״̬
private:
	//static long m_idPropUdf,;						//�û���ǰ�ɶ���ʹ�õ�����Id
	static long m_idMinPropUdf;						//��ʹ�õ���С�û��Զ�������Id
public:	//��ʼ����������ʵ���Ĳ���������
	static CHashPtrList<DYNA_PROPITEM_GROUP> hashParamItemPropGroup;
	static void InitObjParamItemsPropHashTable(long idInitial);
	static long GetPropID(const char* propStr,int iParamType);
	static KEY2C GetPropKey(long id,int iParamType);
	static long GetDbPropId(KEY2C paramKey,int iParamType);
public:
	//���ӷ�����serial:0:Ĭ�Ϸǹ����ְ�����ӷ���;1��ʾ��һ������ְ��ϵ����ӷ���;2:��ʾ�ڶ�������ְ��ϵ����ӷ���
	CONNECT_DIRECTION* GetRelaConnInfo(int serial);
	//����ָ������������ӷ���(��������)
	f3dPoint GetRelaConnVec(int serial,CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);
	//����ָ��������Ĺ�������
	f3dPoint GetRelaConnPlaneNorm(int serial,CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);
	long GetRelaConnVecLinePart(int serial);
	//�������к�ɾ���������� wht 11-05-18
	void DeleteRelaConnVec(int serial);
	//���������ϵľ������꣨����-->չ�����ְ���ԣ�װ�䣩����ϵ;
	//bSpreadVertex=true��ʾչ������false��ʾչ��ʸ������
	f3dPoint SpreadTransV(f3dPoint V,bool bSpreadVertex=true);
	f3dPoint HuoquFaceNorm(){return keyvecset.huoqufacenorm;}
	f3dPoint HuoquAxisVec(){return keyvecset.huoquaxis_vec;}
	f3dPoint HuoquDirection();
	//����ֵ��ʾ�Ƿ������Բ,�������r��ʾ��Բ�뾶,column_norm��ʾԲ������ wht 11-05-27
	BOOL GetInnerCirRadius(double &r,double* column_norm);
	//��ȡ����嶥�㼯��,��Ҫ�ڳ�ͼʱʹ�� wht 11-05-30
	CIRCULAR_PLATE_VERTEXSET GetCirPlateVertexSet();
	//�Ƿ�Ϊ����
	BOOL IsFL(){ return m_iParamType==TYPE_FL||m_iParamType==TYPE_FLD||m_iParamType==TYPE_FLP||m_iParamType==TYPE_FLG||m_iParamType==TYPE_FLR; }
	//�Ƿ�Ϊ���
	BOOL IsInsertPlate(){ return m_iParamType==TYPE_ROLLEND||m_iParamType==TYPE_UEND||m_iParamType==TYPE_XEND;}
	//�����С������η�Χ�����
	virtual f2dRect GetMinWrapRect(UCS_STRU *Rectucs=NULL,double offset=0,BOOL bExportDeformedProfile=FALSE);
	/* ���������ĺ����ڴ�����ʵ��
	virtual double CalAreaOfMinWrapRect(SCOPE_STRU *pScope=NULL);
	virtual double CalAreaOfNetWrap();
	virtual double CalPerimeter();		//����ְ��ܳ�,mm
	*/
	virtual double CalWeldEdgeLength();	//����ְ庸��߳�,mm
	virtual bool ShadowDesign();
	virtual int GetFaceN();						//�ְ�����
	virtual f3dLine GetHuoquLine(int index);	//��ȡָ������(0.��1��2��Ļ����� 1.��2��3��Ļ�ȡ��)��Ϊ������ϵ�µ��������
	virtual f3dPoint GetHuoquFaceNorm(int faceI);
	virtual double GetHuoQuAngle(int iFace,f3dLine *pHuoQuLine=NULL);
	//Ҫ������Ķ�������Ǵ����Ӱ��ϵĶ���,����
	//���ܲ�����ת����ת������(��չ���������-->ʵ���������) wht 11-05-26
	//bCalVertex=true��ʾ��ȡ����false��ʾ��ȡʸ������
	virtual f3dPoint GetRealVertex(f3dPoint vertice,bool bCalVertex=true);
	virtual f3dPoint GetSpreadVertex(const double* V,int face_i=0,bool bSpreadVertex=true);
	//Ҫ���������˨�����Ǵ����Ӱ��ϵ���˨,����
	//���ܲ�����ת����ת������(��ʵ�ʾ�������-->չ���������)
	virtual BOOL SpreadLsPos(CLDSBolt *pLs,double* ls_pos);
	//iResidePlane=0,1,2,�ֱ��ʾ��׼�桢�������桢��������
	virtual bool GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* iResidePlane=NULL,int iIntersPlane=0);
	//cbFlag��FROM_MCS��FROM_ACS��IS_VECTOR��IS_VERTEX������ϣ�Ĭ��ΪFROM_MCS|IS_VERTEX
	//iResidePlaneȡֵ:1��2��3��ʾ��ת�����ʸ����פ������ wjh-2015.2.27
	//static const BYTE FROM_MCS =0x00;	//coord3dΪģ������
	//static const BYTE FROM_ACS =0x01;	//coord3dΪ��ǰ�ְ���װ������(���������)
	//static const BYTE IS_VERTEX=0x00;	//coord3dΪλ�õ�, ִ��λ������ת��
	//static const BYTE IS_VECTOR=0x02;	//coord3dΪ������, ִ��ʸ������ת��
	virtual f3dPoint SpreadToACS(const double* coord3d,int iResidePlane=1,BYTE cbFlag=0);
	virtual GEPOINT TransToActual(const double* coord3d,int iResidePlane=1,BYTE cbFlag=0);
	//ת��Ϊ�Ͱ��ļ��洢ʱ���ò��������Ƿ���Ҫ����Ϊ��ͨ�ְ� wjh-2015.2.16
	bool IsNeedConvertToPlate(int doc_type,int nVersion);
	//ת��Ϊ��ͨ�ְ�
	BOOL ConvertToPlate(CLDSPlate* pPlate);
	//������Ʋ���Ԥ�����������������᷽��ԭ��λ��(��Ӱ��ְ屾������ϵԭֵ) wjh-2017.1.18
	BOOL PreCalPlateCS(double* axis_x=NULL,double* axis_y=NULL,double* axis_z=NULL,double* origin=NULL);
	//ͬ������
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ��λ�öԳ��޹ص�����
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
private:
	bool MirDesignItemsFrom(CLDSParamPlate* pSrcParamPlate,MIRMSG mirmsg);
	BOOL DesignCircularPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);
	BOOL DesignTubeRibPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);
	BOOL DesignCircularRibPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);
	BOOL DesignCircularCoverPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);
	BOOL DesignWaterShieldedPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);
	BOOL DesignRibElbowPlate(CLDSLineTube* pDatumTube,CLDSGeneralPlate* pBtmPlate,CLDSParamPlate::RIBPLATE_ELBOW& elbow);	//������߰�
	BOOL DesignRibCornerOrSlotPlate(CLDSGeneralPlate* pDatumPlate,CLDSParamPlate::RIBPLATE_CORNER& corner);	//��ͨ���߰�
	BOOL DesignRibPlateOnPlate(CLDSGeneralPlate* pDatumPlate,CLDSParamPlate::RIBPLATE_PLATE& platerib);	//�Ǹ��߰�
	BOOL DesignRibPlateOnTube(CLDSLineTube* pDatumTube,CLDSNode* pDatumNode,CLDSParamPlate::RIBPLATE_TUBE& tuberib);	//�Ǹ��߰�
	BOOL DesignRibPlateOnAngle(CLDSLineAngle* pDatumAngle,CLDSParamPlate::RIBPLATE_ANGLE& anglerib);	//�Ǹ��߰�
	BOOL DesignTubeStayWirePlate(CLDSParamPlate::RIBPLATE_STAY_WIRE& stay_wire);	//�ֹ����߰�
	BOOL DesignRibPlate2(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);//�߰�
	BOOL DesignRibPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);	//�߰�
	BOOL DesignBasePlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);//��Ƶװ�
	BOOL DesignShoePlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);//���ѥ��
	BOOL DesignShoePlate2(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);//���ѥ��
	BOOL DesignPedalPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl = NULL);//��ƽ�̤��
	BOOL DesignStationConnPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL); //��ƶ˳Ŷ˰�
	BOOL DesignStationShoePlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);	//��������а�
	BOOL DesignStationCrossPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);	//��ƶ˳Ų��
	BOOL DesignStationLinkPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);	//��ƶ˳��м����Ӱ�
	BOOL DesignStationNodePlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);	//�������м����Ӱ�	BOOL DesignWalkWayCirPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);	//�ߵ��ļӾ�����
	BOOL DesignWalkWayCirPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl=NULL);	//�ߵ��ļӾ�����
};
class CLDSStdPartModel{ //: public CLDSPart{
	ATOM_LIST<f3dPoint> boltPosList;
	void CreateLsHole(fBody* pBody,f3dPolyFace *pTopFace,f3dPolyFace *pBottomFace,f3dPoint norm);
public:
	int m_iStdPartType;//��׼��������
	union DES_PARAM{
		INSERT_PLATE_PARAM insert_plate_param;	//������
		FL_PARAM fl_param;		//ƽ���������Ժ���������
		FL_PARAM2 fl_param2;	//���Է��������Է�������
	}param;
public:
	double m_fRelateTubeThick;
	CLDSStdPartModel();
	~CLDSStdPartModel();
	char* GetPartNo();
	void LayoutBolt();
	BOOL CreateBoltInPlate(CLDSParamPlate *pParamPlate);
	BOOL Create3dSolidModel(fBody* pBody,BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36,BYTE cRollType=0xFF);
	f3dPoint* GetFirstBoltPos(){return boltPosList.GetFirst();}
	f3dPoint* GetNextBoltPos(){return boltPosList.GetNext();}
	int GetBoltCount(){return boltPosList.GetNodeNum();}
};
struct RAYNO_RECORD{long hPart;DWORD dwRayNo;double yCoord;};
// ��
class CLDSPlate : public CLDSGeneralPlate {
protected:
	long _face_N;						//����
	double _fInnerRadius;				//��Բ���뾶
	//��0.��ʾ��ͨ�� 1.����� 2.���ð� 3.�۵��� 4.����˨���5.�����˨���
	//��0.��ʾ��ͨ�� 1.�۵��� 2.����� 3.�ֹܼа� 4.��˨��� 5.����ڻ�׼����λ�ò���ĸ��Ӱ� 6.���ð�
	/* ����CLDSGeneralPlate�� wjh-2017.1.21
	union UNION_TYPE{
		UINT iLegacyStyle;		//�ɾ�_jdb_style�������� wjh-2017.1.21
		struct{
			WORD wiLegacyStyle;	//��ӦTMA V5.1.5֮ǰ��_jdb_style
			WORD wiType;		//��Ӧ��CLDSParamPlate::m_iParamType���Ϻ�ĸְ���Ʋ�������
		};
	}style;*/
	BYTE _cbAlignment;	//0.�޶���;1.ʮ�ֽ������;2.��׼�Ǹ��Ͼ��λ�����;3.������λ�
protected://��ȡ�ְ���ӷ�������
	virtual WORD InternalGetDesignType();		//�ú�����������Ϊ�������ֱ��get=�����麯������ wjh-2017.1.20
	virtual void InternalSetDesignType(WORD wiDesignType);
public:	//��������
	PROPERTY(double,Thick); 
	VIRTUAL_SET(Thick);
	VIRTUAL_GET(Thick);
	PROPERTY(BYTE, cbAlignment); 
	virtual GET(cbAlignment){
		CLDSPlate* pMotherPlate=(CLDSPlate*)MotherPart();
		if(pMotherPlate!=NULL&&pMotherPlate!=this)
			return pMotherPlate->cbAlignment;
		else
			return _cbAlignment;
	}
	virtual SET(cbAlignment){
		if(value>=0)
		{
			CLDSPlate* pMotherObj=(CLDSPlate*)MotherObject();
			_cbAlignment = ftoi(value);
			SetModified();
			if(pMotherObj!=NULL&&pMotherObj!=this)
				pMotherObj->cbAlignment=value;
		}
	}
	PROPERTY(long,face_N); 
	GET(face_N){
		CLDSPlate* pMotherObj=(CLDSPlate*)MotherObject();
		if(pMotherObj!=NULL&&pMotherObj!=this)
			return pMotherObj->face_N;
		else
			return _face_N;
	}
	SET(face_N){
		if(value>=0)
		{
			CLDSPlate* pMotherObj=(CLDSPlate*)MotherObject();
			_face_N = value;
			SetModified();
			if(pMotherObj!=NULL&&pMotherObj!=this)
				pMotherObj->face_N=value;
		}
	}
	PROPERTY(double,m_fInnerRadius); 
	GET(m_fInnerRadius){
		CLDSPlate* pMotherObj=(CLDSPlate*)MotherObject();
		if(pMotherObj!=NULL&&pMotherObj!=this)
			return pMotherObj->m_fInnerRadius;
		else
			return _fInnerRadius;
	}
	SET(m_fInnerRadius){
		if(value>=0)
		{
			CLDSPlate* pMotherObj=(CLDSPlate*)MotherObject();
			_fInnerRadius = value;
			SetModified();
			if(pMotherObj!=NULL&&pMotherObj!=this)
				pMotherObj->m_fInnerRadius=value;
		}
	}
	DECLARE_PROPERTY(long,jdb_style); 
public:
	//0.��ʾ��������ͳ��;1.��ʾ��С���ݾ���ͳ��;2.��ʾ�����ͳ��
	static int m_iPlankWeightStatType;
	//m_siProfileAlgorΪ��ʱ����,�����㷨��ȫ������ȶ������,����ȫȡ�����㷨 wjh-2015.11.08
	static short m_siProfileAlgor;	//0.��ͳ���μ����㷨;1.�¸Ľ��������μ����㷨
	//���Ӱ����Ʋ���
	UCS_STRU ucsView;					//�˰����ʱ����ʾ��ͼ����ϵ����ʱ���ز��ش����ļ�
	long neutrosphere;					//��ʾ����ʱ���Բ�λ�ڼ���֮һ��λ��
	CDesignLjPara designInfo;
	double m_fNormOffset;				//װ��ʱ����λ��������˲������ڸֹ��Ϻ��Ӱ��λ���趨
	f3dPoint m_tInnerOrigin;			//��ԲԲ��λ��---�������	wht 09-08-25
	f3dPoint m_tInnerColumnNorm;		//��Բ���߷���
	BOOL m_bReplPart;					//���ù���
	BOOL m_bWeldPart;					//���Ӽ�
	BOOL m_bNeedFillet;					//��Ҫ�¿�
	int  m_iLsArrayStyle;				//�ְ�����˨�����з�ʽ	0-�������� ��0-��������	wht08-07-31	
	double m_fWeldWidth;				//�����϶
	double m_fDimStartDist;				//��ʼ����궨��϶
	double m_fDimEndDist;				//��ֹ����궨��϶

	//�������������׼���ķ��߼н�
	f3dPoint huoqufacenorm[2];		//�ڵ�����ķ��߷���
	f3dLine HuoQuLine[2];	//�����ߣ���������꣩
	bool m_bBendPlane2ResideBendPlane1;	//�ڶ��������Ե�һ������Ϊ��׼�������� wjh-2017.12.1
protected:
	PROFILE_VERTEX _xPeakVertex;
	virtual PROFILE_VERTEX* InternalGet3PlanePlatePeakVertex(){return &_xPeakVertex;}
	//�������������Ӹְ�����������ȡ��������������ʾʱ�����ְ����Σ����洢 wht 18-12-17
	double _peakVertexOffsetY;		//�����߹�������壬�����߽���Y����ƫ����
	double _extendVertexOffsetLen;	//�����߹�������壬�и�λ��X���ȷ���������
	double _peakPointMargin;		//�������Ĭ�ϼ�϶��
public:
	double getPeakVertexOffsetY();
	void setPeakVertexOffsetY(double offset);
	__declspec(property(put=setPeakVertexOffsetY,get=getPeakVertexOffsetY)) double peakVertexOffsetY;
	double getExtendVertexOffsetLen();
	void setExtendVertexOffsetLen(double offset);
	__declspec(property(put=setExtendVertexOffsetLen,get=getExtendVertexOffsetLen)) double extendVertexOffsetLen;
	double getPeakPointMargin();
	void setPeakPointMargin(double margin);
	__declspec(property(put=setPeakPointMargin,get=getPeakPointMargin)) double peakPointMargin;
public:
	f3dPoint& getTopPoint(){return _xPeakVertex.vertex;}
	void setTopPoint(f3dPoint& vxTopPoint){_xPeakVertex.vertex=vxTopPoint;}
	__declspec(property(put=setTopPoint,get=getTopPoint)) f3dPoint& top_point;	//������������ߵĽ��㣨��������꣩
	//�ְ�Ϊ�����ʱ�ñ�����Ч	wht 10-01-12
	THANDLE complement_plate_handle;//������¼����岹���ְ�ľ��

	char org_material;
	//THANDLE base_node_handle;			//��׼�ڵ���
	//THANDLE base_part_handle;			//��׼�˼����(ֻ�ܶ��ұ�����һ��)
	long m_iLsRayNo;	//��˨���ڵ����ߺ�
	//���Ҫ�ڸְ��ϻ�����������˨���˱�����ʾ��׼��Ϊ�����߻���Ϊ��׼��  �鿴�޸Ĵ˱���ֻ���ڲ��øֹ����ĶԻ����н���
	BOOL m_bAttachPartByWeldRoad;	
	virtual bool ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType=0);
	virtual long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
public:
	void DesignSetupUcs();
	virtual bool IsBelongTo(int iClassTypeId)const;
	virtual long get_BaseNode(){return designInfo.m_hBaseNode;}//�������Ӱ�������ڵ�
	virtual long get_BaseRod() {return designInfo.m_hBasePart;}	//���ظְ��׼�ڵ�  wht 09-12-12
	virtual CLDSPart* NewCopy(bool bByShadowMode=false);
	virtual CLDSGeneralPlate* CloneCopy(double offset,double* vUdfDirection);
	virtual CLDSGeneralPlate* ShadowCopy(double offset,double* vUdfDirection);
	virtual CLDSGeneralPlate* CloneCopy(GEPOINT vOffset,bool localAcsCoord=true);
	virtual CLDSGeneralPlate* ShadowCopy(double offset,bool localAcsCoord=true);
	virtual bool ShadowDesign();			//����m_xDesOrgInfo��m_xMirAxisInfo��normOffset�Զ�����ucs
	virtual bool OptimizeStructure();
	void CopyProperty(CLDSDbObject *pObj);
	int GetEdgeFaceNo(int start_feature,int end_feature);
	//CDesignLjPara* DesignLjPara(){return pDesign;}
	CDesignLjPartPara* GetFirstLjPara(){return designInfo.partList.GetFirst();}
	CDesignLjPartPara* GetNextLjPara(){return designInfo.partList.GetNext();}
	CDesignLjPartPara* GetLjRodPara(long handle);
	BOOL UpdateLjRodEdgeSpace(long hPart,BYTE cbEdgeFlag=0x03);	//cbEdgeFlag,0x01:���߲ࣻ0x02:֫��ࣻ0x04:�˾�
	BOOL UpdateLjRodEdgeSpace(CDesignLjPartPara* pLjPara, BYTE cbEdgeFlag = 0x03);
	BOOL DeleteLjPara(long handle);
	bool DesignTubeFoldPlate();
	bool DesignSubstationSplayFoldPlate();
	bool DesignPlate();
	BOOL DesignRhombusPlate();	//������ν�����
	void SortVertex(PROFILE_VERTEX *pPntArr, int nCount);
	//����ְ���������ָ��ͶӰ���ϵ�ͶӰ��������λ��(������ͶӰ����ֵ)
	//perpline_pick:������ϵ�µ�ͶӰ�߻���
	//perpline_vec:������ϵ�µ�ͶӰ�����췽��
	VALUE_SCOPE WeldLinePerpScope(f3dPoint perpline_pick,f3dPoint perpline_vec);
	void AlignWeldLine();
	void DesignProfile(BOOL bSortingVertex=TRUE);
	void DesignProfile2();	//�ð������ε�������д�����μ��㷽��
	virtual bool IsConnectPlate();	//�ְ��Ƿ�Ϊ�и˼�����˨���ӵ����Ӱ�
	virtual bool IsPeakPointPlate();//�Ƿ�Ϊ�����ǵ�����塡wjh-2016.1.9
	virtual bool IsFoldPlate();		//�Ƿ�Ϊ�����۵��塡wjh-2016.1.9
	virtual bool IsHuoQuLineCollinear3FacePlate();	//�Ƿ�Ϊ�����߹��ߵ������ wht 18-12-17
	bool IsPlate() const{return true;}
private:
	BOOL Create3dSolidRollEdge(fBody* pBody,f3dAtomLine *pAtomLine,PROFILE_VERTEX *pFeaVertex,int iFaceNo,COLORREF plank_color);
	void CalWeldLineVertex(CDesignLjPartPara *pLinePartPara,f3dPoint vertex_up,
							f3dPoint vertex_down,f3dPoint bolt_up,f3dPoint bolt_down,
							f3dPoint &weld_vertex_start,f3dPoint &weld_vertex_end,BOOL bFirstRayPart);
	BOOL CalProfileVertex(CDesignLjPartPara *pLinePartPara,BOOL bFirstRayPart=FALSE,f3dPoint *pWeldLineStart=NULL,f3dPoint *pWeldLineEnd=NULL);
	//������ư�����ʱֻ��������������,�������ӡ���ߵ���������
	BOOL CalProfileVertexPos(CDesignLjPartPara *pLinePartPara,ARRAY_LIST<f3dPoint>& vertexArr,f3dPoint *pBlotUp=NULL,f3dPoint *pBoltDown=NULL);
	bool UpdateProfileVertexAndPos();
	void InitBoltPadLjPartPara(CDesignLjPartPara *pLjPartPara);
public:
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	int ValidCheck();
	static const BYTE VALIDATE_PROFILE_BASIC	 = 0x01;
	static const BYTE VALIDATE_PROFILE_EDGESPACE = 0x02;
	virtual BOOL ValidateProfile(double epsilon=1.0,EDGEBOLT *endBoltInfoArr=NULL,int *count=NULL,BYTE cbValidateFlag=0xff);
	BOOL CalWrapLsProfile(double angle_lim=60.0);
	BOOL AssertValid();
	CLDSPart *GetMirPart(MIRMSG mirmsg,const double *pPickPos=NULL,BOOL bCheckProfile=TRUE);
	bool IsEqual(CLDSPlate* pMirPlank,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	virtual int IsEqualProfile(CLDSGeneralPlate* pMirPlate,BOOL bCompareLs=TRUE,double epsilon=1.0);
	//ָ����׼�����Ƚϸְ����� 
	int CompareProfile(CLDSPlate *pMirPlate,BOOL bReverseOrder,int base_i,BOOL bCompareLs,double epsilon,
					   ATOM_LIST<PROFILE_VERTEX> &src_vertex_list,ATOM_LIST<PROFILE_VERTEX> &mir_vertex_list);
	CLDSPlate(void);
	~CLDSPlate(void);
	BOOL CalStdProfile();
	/* ����Ϊ�麯������CLDSGeneralPlate��
	//�����С������η�Χ�����
	virtual f2dRect GetMinWrapRect(UCS_STRU *Rectucs=NULL,double offset=0,BOOL bExportDeformedProfile=FALSE);
	virtual double CalAreaOfMinWrapRect(SCOPE_STRU *pScope=NULL);
	virtual double CalAreaOfNetWrap();
	virtual double CalPerimeter();		//����ְ��ܳ�,mm
	*/
	virtual double CalWeldEdgeLength();	//����ְ庸��߳�,mm
	f2dRect GetWrapRect();
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	double GetWidth();
	double GetLength();
	double GetWeight(BOOL bSolidLength=TRUE);
	double GetThick();
	virtual CLDSNode *GetBaseNode(bool bClearLocks=true);	//���ظְ��׼�ڵ�  wht 09-12-12
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual bool PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	virtual bool PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long liPushVersion,long liDocType);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	//bool bAddSubLevelRelaObj=false:��ʾ������Ӽ���������;true:��ʾ����Ӽ��������� wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	void DesignProtrusiveProfile();
	//iResidePlane=0,1,2,�ֱ��ʾ��׼�桢�������桢��������
	virtual bool GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* iResidePlane=NULL,int iIntersPlane=0);
	//Ҫ���������˨�����Ǵ����Ӱ��ϵ���˨,����
	//���ܲ�����ת����ת������(��ʵ�ʾ�������-->չ���������)
	virtual BOOL SpreadLsPos(CLDSBolt *pLs,double* ls_pos);
	BOOL SpreadLsPos(const double* ,double* ls_pos_coord);
	BOOL SpreadLsPos(CLsRef *pLsRef,double* ls_pos){return SpreadLsPos(pLsRef->GetLsPtr(),ls_pos);}
	f3dPoint RestoreLsPos(f3dPoint ls_pos,int iFace012=0);
	//����ָ��������Ļ����Ƕȣ���һ�����������1,�ڶ�����������2
	virtual double GetHuoQuAngle(int iFace,f3dLine *pHuoQuLine=NULL);
	virtual double GetNormOffset(){return m_fNormOffset;}//��ȡ�ְ��������ڻ�׼ԭ��ķ���ƫ����
	//Ҫ������Ķ�������Ǵ����Ӱ��ϵĶ���,����
	//���ܲ�����ת����ת������(��չ���������-->ʵ���������)
	virtual f3dPoint GetRealVertex(f3dPoint vertice,bool bCalVertex=true);
	//(��ʵ���������-->չ���������);face_iȡֵΪ1,2,3�ֱ��ʾ���������;
	//bSpreadVertex=true��ʾչ������false��ʾչ��ʸ������
	virtual f3dPoint GetSpreadVertex(const double* coord3d,int face_i=0,bool bSpreadVertex=true);
	//cbFlag��FROM_MCS��FROM_ACS��IS_VECTOR��IS_VERTEX������ϣ�Ĭ��ΪFROM_MCS|IS_VERTEX
	//iResidePlaneȡֵ:1��2��3��ʾ��ת�����ʸ����פ������ wjh-2015.2.27
	//static const BYTE FROM_MCS =0x00;	//coord3dΪģ������
	//static const BYTE FROM_ACS =0x01;	//coord3dΪ��ǰ�ְ���װ������(���������)
	//static const BYTE IS_VERTEX=0x00;	//coord3dΪλ�õ�, ִ��λ������ת��
	//static const BYTE IS_VECTOR=0x02;	//coord3dΪ������, ִ��ʸ������ת��
	virtual f3dPoint SpreadToACS(const double* coord3d,int iResidePlane=1,BYTE cbFlag=0);
	//static const BYTE TO_MCS =0x04;	//ת����ģ������
	//static const BYTE TO_ACS =0x00;	//ת����Ϊ��ǰ�ְ���װ������(���������)
	virtual GEPOINT TransToActual(const double* coord3d,int iResidePlane=1,BYTE cbFlag=0);
	//���ְ���Ϣ���Ƶ�ָ��Buffer��
	BOOL CopyPlateToBuffer(CBuffer &buffer);
	//�������е���Ϣ���Ƶ��ְ���
	BOOL PastePlateFromBuffer(CBuffer &buffer,CLDSNode *pDatumNode);
	//��¡�ְ�
	void CloneTo(CLDSPlate &plate);
	//�ж��Ƿ�Ϊ��ͨ�ֹܵĸְ�
	BOOL IsTransPlate();
	//�õ��ְ弸������λ��
	static f3dPoint GetGeomCenter(CLDSPart *pPlate);

	friend BOOL IsLsRefListEqual(CLsRefList *pLsRefList,const UCS_STRU &ucs,
					   CLsRefList *pViceLsRefList, const UCS_STRU &vice_ucs,
					   double epsilon,char *error_info,long *pH);
public:
	double GetOutterPlaneOffset(const f3dPoint &outter_vec);
	//�˺���������������ͨ���������油��ʱ�Ĳ��ϸְ�Ķ�����ζ���
	bool GetComplementFaceVertex(f3dPoint &vertex1,f3dPoint &vertex2);
	//���㹫�е㣬�ʼ��PartEditor.cpp�м��㹫�е�Դ������ wjh-2013.6.6
	static bool UpdateVertexTangentPos(PROFILE_VERTEX* pPrev,PROFILE_VERTEX* pCurr,PROFILE_VERTEX* pNext);
	//�������岹���ְ�����
	bool Design3FacePlateComplementPlate(CLDSPlate *pDatumPlate);
	double GetRollEdgeHeight();
	virtual BYTE FuncType();
	virtual double GetDeformCoef(int iFace=0);
	virtual double GetHuoQuR(int iFace=0);
	virtual void SetDeformCoef(double deform_coef=0.35,int iFace=0);
	virtual void SetHuoQuR(double r=0,int iFace=0);
	//����ְ�����
	DECLARE_PROP_FUNC(CLDSPlate);
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//�õ�����ֵ
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//�������ֵ�Ƿ����
	//
	static int nPlankEdgeAngleThreshold;
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//�õ��������Ե�Ĭ��ֻ��״̬
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//�õ��������Ե�Ĭ��ֻ��״̬
	BOOL IsInsideAngle(CLDSLineAngle* pAngle, short* x_wing0_y_wing1);
	BOOL IsInsideAngle(CLDSLineAngle* pAngle, BYTE* x_wing0_y_wing1);
	virtual int GetFaceN(){ return _face_N;}	//�ְ�����
	virtual f3dLine GetHuoquLine(int index);	//��ȡָ������(0.��1��2��Ļ����� 1.��2��3��Ļ�ȡ��)��Ϊ������ϵ�µ��������
	virtual f3dPoint GetHuoquFaceNorm(int faceI);
	//ͬ������
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ��λ�öԳ��޹ص�����
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
};
/* ����ģʽ������ش���������ClonePlate.h,ClonePlate.cpp wjh-1017.8.7
struct LINKPOLEINFO{
protected:
	UINT id;
public:
	CLDSLinePart *pLinePart;//��¼���Ӹ˼�ָ��
	CLDSLinePart *pMatchLinePart;//��¼ƥ��˼�ָ��
	long hLinePart;			//���Ӹ˼����
	long hMatchPole;		//��¼�뵱ǰ�˼�ƥ������߸˼����
	double offset_angle;	//��¼���߸˼����׼�˼�֮��ļн�
	int vec_type;			//��¼���߸˼����췽�� 1��ʾ���׼�Ǹֵ�ǰ֫���췽����ͬ -1��ʾ���׼�Ǹֵ�ǰ֫���췽���෴
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
class CSchemaRod : public LINKPOLEINFO
{
public:
	struct DESIGNLJ_ANGLEPARA
	{
		BYTE ciWorkWingX0Y1;	//��ǰ����֫
		BYTE ciSonAngleQuad;	//��׼�Ǹ���Ϊ��ϽǸ��ҵ�ǰ�Ǹ�Ϊ��׼��ϽǸֵ��ӽǸ֣�����X-Zƽ���ڵ����޺�,ȡֵ:1-4��0��ʾ����Ǹ�
		ANGLE_SPACE_FLAG cbSpaceFlag;		//
		BOOL bEndLjAngle;		//�����ӽǸ�
		BOOL bDualEdge;			//�Ƿ���˨�����඼����������
	}angle;
	//BOOL m_bAdjustLjEndPos;		//�ԳƸְ�ʱ�Ƿ���Ҫ�������߸˼����Ӷ˰ڷ�λ�ã���Ҫ�ڶԳƹ��ð�ʱʹ�� wht 10-02-26 
	long m_idClassType;
	//int m_iAffectTubeEndPlateStyle;
	long hOddRefRod1,hOddRefRod2;;	//����ͷУ����չ���
	short siPlaneNo;				//��ǰ�˼����ڵ�������,0:��׼ƽ��;1.��һ������;2.�ڶ�������
	short siLinkStart0End1;			//���ʼ��0���ն�1;
	short siEndSpace,siWingSpace,siBerSpace;	//���������ʱ�Ĳ���
public:
	CSchemaRod();
	void CopyProperty(CSchemaRod* pScrSchemaRod);
};
//��¡�ְ�
class CClonePlate{
protected:
	//0.��ƥ�� 
	//1.��ȫƥ��(�����ͬ)  �Ƕ�ƥ�� ���߽Ǹֹ��ƥ�� ���Ĺ��ƥ�� ���߽Ǹ�������ͬ
	//2.���߸˼�����ƥ����ƥ�䵫�ǶȲ�ƥ��  ==>ȡ�ְ������Ϣ,���߽Ǹ���˨��Ϣ��׼�Ǹ���˨��Ϣ,�Զ���������ͷ
	//3.�����߸˼�����ƥ��
	int m_iMatchResult;	//ƥ����
	IModel *m_pTower;
	LINEPARTSET partset;	//�ڵ㵱ǰ����ظ˼�����
	CHashListEx<CSchemaRod> hashLinkRods;
	CHashSet<CSchemaRod*> hashLinkRodsByH;
	void EmptyLinkRods();
public:
	char ciDatumAxis;	//һ��Ϊ'Y'
	char ciDirectionAxisY;
	char ciDirectionAxisZ;	//0��ʾ��ǰ����֫������;1����ʾ��ǰ����֫������
	struct VECTORS{
		GEPOINT vDatumLenVec;	//��׼�˼����췽��һ��ҲΪY�᷽�򣩣�
		GEPOINT vDatumPlaneNorm;//�ְ��׼ƽ��ķ��߷���һ��ҲΪ��׼�ǸֵĹ���֫���߷���
		GEPOINT vBendPlaneNorm[2];
	}xLocalVec;	//xLocalVecΪԴģ��ֲ�����ϵ�µ�ʸ��,xModelVec��ǰģ������ϵ�µ�ʸ��
	VECTORS UpdateModelCSVectors(GECS acs);	//�ְ�ֲ�װ������ϵ
	int JustifySonAngleResideQuad(GECS acs,CLDSLineAngle* pAngle);
public:
	long m_hSrcPlate;		//��¼Դ�ְ���
	BYTE m_nPlaneN;			//�ְ幤������>1��ʾ������
	UINT m_idDatumRod;		//��׼�˼��ı�ʶId
	CLDSPlate *m_pSrcPlate;	//��¼ָ��Դ�ְ��ָ��
	CLDSNode *m_pDestNode;	//��Ҫճ���ְ�Ľڵ�
	CClonePlate();
	~CClonePlate();
	CClonePlate(IModel *pTower,CLDSPlate *pPlate=NULL,CLDSNode *pNode=NULL);
	void SetSuperiorTower(IModel *pTower);
	CSchemaRod* AppendSchemaRod(long hLinkRod,DWORD keyid=0);
	bool RemoveSchemaRod(CSchemaRod* pSchemaRod);
	void AnalyzePlateInfo(CLDSPlate *pPlate,IModel *pTower=NULL);	//�����ְ���Ϣ
	int IntelligentMatchPlate(CLDSNode *pNode,PARTSET& DispPartSet,IModel *pTower=NULL);//����ʶ��ǰ�ڵ����Ƿ��������ͬ��ŵĸְ�
	CLDSPlate *CreatePlate(PARTSET& DispPartSet,IModel *pTower=NULL);	//����ƥ���������¸ְ�
	void ToClipperBoard(CBuffer* pClipperBoard,IModel *pTower=NULL);		//���ְ���Ϣ�洢����������
	void FromClipperBoard(CBuffer* pClipperBoard,IModel *pTower=NULL);		//�Ӽ��а��ж�ȡ�ְ���Ϣ
	CLDSLinePart *FindMatchPoleBySrcHandle(long hLinePart);	//ͨ��Դ�˼����������֮��ƥ��ĸ˼�
	CLDSLinePart *FindSrcPoleByMatchHandle(long hMatchPole);//ͨ��ƥ��˼����������֮��ƥ���Դ�˼�
	//����ڵ�˼����� 0.�����߸˼����׼�˼�fabs(cosa)����  1.����ʱ�뷽������(��ָ�����߸˼����ڸְ�)
	void SortLinePartSet(CLDSLinePart *pDatumLinePart=NULL,int iSortType=0,CLDSPlate *pPlate=NULL);
	void CorrectPlateDesignPara(CLDSPlate *pPlate);	//�������߸˼��������ְ����߸˼�����
	void CorrectPlateLsInfo(CLDSPlate *pPlate);		//���ݸ˼���˨��Ϣ������˨����Լ���˨���
	void UpdateRayLinePartOffset(CLDSPlate *pPlate,int nAutoAdjustTubePos=0);//���¸ְ����߸˼���Ϣ
	void UpdateRayLinePartOddment(CLDSPlate *pPlate);//�������߸˼�����ͷ
};
*/
//#---------------------------------------------
//����
class CLDSWeldLine : public CLDSPart{
public:
	CLDSWeldLine();
	f3dArcLine weldline;
	short weld_type;	//0:���ֺ�;1:�;2:�Ժ�
	double weld_high,weld_wide;
	double thick;		//ֻ�Զ��ֺ��ͶԺ���Ч����ʾ�����ֺ���ʱ�����Ӱ�ĺ��
	//�ܺ������
	double Volume();
};
// ����
class CLDSCube : public CLDSLinePart {
public:
	double b;
	f3dPoint start_xyz,end_xyz;
	f3dPoint work_norm;			// �����淨�߷���
	CLDSCube(void);
};
//�ֹܽ�ͷ
struct TUBEJOINT{
	int type;	//0:ƽ�������;1.Բ���������;2.����;3.U�Ͳ��;4.C�Ͳ��;5.ʮ�ֲ��;6.�߾�����.
	f3dPoint cutPlanePos,cutPlaneNorm;
	double m_fAxisXRotAngle,m_fWorkPlaneRotAngle;
	THANDLE hLinkObj,hViceLinkObj;	//hViceLinkObj>0 ����˫����� =0��ͨ��� <0�Խ����
	BOOL bVertPlate;
	BOOL bEndPlate;
	int typeEndPlate;	//0:Բ��ͷ��  1:����ͷ��
	bool IsValidPlaneCut(){return type==0&&!cutPlaneNorm.IsZero();}
	TUBEJOINT(){hViceLinkObj=-1;}
};
class CTubeEndPosPara : public CRodPosPara{
public:
	BOOL IsInDatumJgBer();		//�����Ƿ��ڻ�׼�Ǹ�������
	BOOL IsInDatumJgWingX();	//�����Ƿ��ڻ�׼�Ǹ�X֫��
	BOOL IsInDatumJgWingY();	//�����Ƿ��ڻ�׼�Ǹ�Y֫��
	CTubeEndPosPara(){memset(this,0,sizeof(CTubeEndPosPara));m_iPosParaType=1;}
	bool ReplaceReferenceHandle(CHandleReplacer& replacer,double* pickpos_coord);
	long StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers);
	virtual void CleanInvalidObjRef(IModel *pModel=NULL);
	//0:�˵㼴�ڵ�;1:����ڽڵ㸸�ֹܸ˼���;
	//2:�ڵ���ָ����׼��ͶӰ;3.�˼����߽���;4.ָ������
	long endPosType;	
	// 0:������ͶӰ      1:X֫����ͶӰ       2:Y֫����ͶӰ
	// 3:������ƽ��      4:X֫����ƽ��       5:Y֫����ƽ��
	BYTE datum_pos_style;		//���㶨λ��ʽ,��׼�˼�Ϊ�Ǹ�ʱ���� wht 10-06-14
	double fEccentricDist;		//ƫ�ľ�
	bool bUseFaceOffsetNorm;	//����ƽ���淨��
	f3dPoint face_offset_norm;	//ƽ���淨��  wht 10-04-20 
	double fNormOffset;		//��׼�淨��ƫ����
	double fTransverseOffset;//����ƫ���� ��Ҫ���ڵ����۸֡���ֵ�λ�� wht 09-10-17	
	double fLenVecOffsetDist;//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ�� wht 11-04-23
	THANDLE hDatumPart;		//���λ�û�׼�˼�/�˼����߽��㶨λ�ǵĻ�׼�˼�
	THANDLE hDatumStartPart;//��ʼ�˼����/�˼����߽��㶨λʱ�Ľ���˼� wht 10-10-18
	THANDLE hDatumEndPart;	//��ֹ�˼����
	BOOL bDatumStartStartEnd,bDatumEndStartEnd;	//ָ����׼�ǸֵĻ�׼�˵�0:ʼ���;1:�����
	void FromBuffer(CBuffer &buffer, long version=0,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
};

// �ֹ�
class CLDSLineTube : public CLDSLinePart {
	f3dPoint workPlaneNorm;
	BOOL _bHasWeldRoad;		//�ֹ��Ƿ���ں�����
	bool _bCutSlot;			//�Ƿ���Ҫ���п��۴���
public:	//��������
	PROPERTY(BOOL,m_bHasWeldRoad);
	GET(m_bHasWeldRoad){
		CLDSLineTube* pMotherObj=(CLDSLineTube*)MotherObject();
		if(pMotherObj==this)
			return _bHasWeldRoad;
		else
			return pMotherObj->m_bHasWeldRoad;
	}
	SET(m_bHasWeldRoad){
		CLDSLineTube* pMotherObj=(CLDSLineTube*)MotherObject();
		_bHasWeldRoad=value;
		if(pMotherObj!=this)
			pMotherObj->m_bHasWeldRoad=value;
	}
	PROPERTY(bool,m_bCutSlot);
	GET(m_bCutSlot){
		CLDSLineTube* pMotherObj=(CLDSLineTube*)MotherObject();
		if(pMotherObj==this)
			return _bCutSlot;
		else
			return pMotherObj->m_bCutSlot;
	}
	SET(m_bCutSlot){
		CLDSLineTube* pMotherObj=(CLDSLineTube*)MotherObject();
		_bCutSlot=value;
		if(pMotherObj!=this)
			pMotherObj->m_bCutSlot=value;
	}
public:
	static const BYTE FUNC_COMMON		= 0;	//��ͨ�ֹ�
	static const BYTE FUNC_WIREBUSH		= 3;	//���߰��ϵ��׹�
	static BYTE ciFlTubeOddment;	//0.���ֹܶ���;1.����������
	//BYTE m_cFuncType;
	union{
		struct{	//��ͨ�ֹ�
			long hTransTube;		//�ᴩ�˸ֹܵĸֹ�
			long hWorkNormRefPart;	//���㹤���淨��ʱ�Ĳ��ո˼������Ϊ0������ʼ�˽ڵ���ն˽ڵ�ĸ��˼�Ϊ���ո˼�
			union{
				double pickWorkPlane[3];	//����ƽ����յ�(m_iNormCalStyle=2)����Ʒ��߷���nearWorkPlaneNorm(m_iNormCalStyle=1) wjh-2015.8.7
				COORD3D nearWorkPlaneNorm;
			};
		};
		struct BUSH{	//���߿��׹�
			short height;			//�׹ܸ߶�
			long hBushResidePlate;	//�׹����ڹ��߸ְ�
			long hBushRelyBolt;		//�׹��������Ķ�λ���߿�������˨
			long hUpBushRingPlate;	//�׹ܶ�����Ե���ӳ��׻���
			long hDownBushRingPlate;//�׹ܵײ���Ե���ӳ��׻���
		}bush;
	};
	f3dPoint cutPlaneOrigin,cutPlaneNorm;		//������һ���и��涨��ʵ��
	UINT m_uSlices,m_uDisplaySlices;
	TUBEJOINT m_tJointStart,m_tJointEnd;	//ʼ�ն˸ֹܽ�ͷ
	CLDSPoint xBushLocation;	//�׹ܶ�λ��(λ�ڹ��߿���������ְ���潻����)
	CTubeEndPosPara desStartPos,desEndPos;
	f3dPoint nearWeldRoadVec;	//���������ڷ���
	short  m_iWeldRoadStyle;	//ֻ����ʱ�������ƺ����ߵ���ʾ��ʽ���ñ���
	short  m_iNormCalStyle;	//��׼�淨�߼��㷽ʽ 0.ֱ��ָ�� 1.������ո˼� 2.������յ�
	static BOOL bDisplaySolidTransectFace;
public:
	static bool IsHasTubeRelaModifiedParts();
	static CLDSLineTube* AppendModifiedTube(CLDSLineTube* pModifiedTube);	//��¼��Ҫ����ʵ��ĸֹ�
	static CLDSLineTube* EnumFirstModifiedTube();	//������һ�������仯����Ҫ����ʵ��ĸֹ�
	static CLDSLineTube* EnumNextModifiedTube();	//������һ�������仯����Ҫ����ʵ��ĸֹ�
	virtual int GatherOddRelaParts(BYTE start1end2all3=3);
	virtual bool ShadowDesign();
	virtual void TransToCS(GECS& cs);
	virtual void Offset(const double* offset);
	void SetWorkPlaneNorm(f3dPoint work_norm){workPlaneNorm=work_norm;}
	double CalWeldRoadAngle();	//����ֵΪ����
	void CalWeldRoadVec(double fAngle);//����ֵΪ�Ƕ�ֵ
	virtual double GetArea();	//��λΪmm2
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	//bSolidLength��ʾ���㳤��ʱ�����߼��㻹�ǰ�ʵ�����,��Ҫ����ѧ����ʱͳ��������
	virtual double GetWeight(BOOL bSolidLength=TRUE);	//��λΪkg
	double GetEndActualOdd();	//�����ն��������������ʵ�����ٵ�����ͷ
	double GetStartActualOdd();	//����ʼ���������������ʵ�����ٵ�����ͷ
	BOOL CreateRoundBar3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);//�ֽ�ʵ������
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	//��ǰ�����Ƿ������ָ����ͼ����ϵ(��Z��Ϊ���߷���)������ס��һ����
	virtual bool IsCanHide(CLDSPart* pPart,const double* dcs_norm);
	virtual bool GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* cReserved=NULL,int iIntersPlane=0);
	//��øֹܵ�������Բ����ĳ��ȼ����������
	double GetCutCylinderFace(int slices,fPtList &vertex_list,f3dPoint &extend_vec);
	void CopyProperty(CLDSDbObject *pObj);
	void CalWorkPlaneNorm();
	virtual f3dPoint WorkPlaneNorm();
	f3dPoint GetPlateVertSpreadVec(CLDSGeneralPlate *pPlate,char ciPlane=0);			//��ȡ�ְ�ƽ���ϴ�ֱ�ڸֹ����ߵ���չ����
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer, long version = NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	//bReadForCheck��ʾ�Ƿ��ȡΪ�˽���ģ�ͱȶԼ��(����ȡ������У�������Ϣ)
	virtual void FromCdfBuffer(CBuffer &buffer,long version=0,bool bReadForCheck=false);
	virtual void ToCdfBuffer(CBuffer &buffer,long version=0);	
	//bool bAddSubLevelRelaObj=false:��ʾ������Ӽ���������;true:��ʾ����Ӽ��������� wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//��Эͬ����ͨ���ܵ���ȡ����
	virtual void FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	//ͨ���ܵ���Эͬ����д������
	virtual void ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	CLDSLineTube(void);
	~CLDSLineTube();
	double GetDiameter(){return size_wide;}
	void CalStartPos(CLDSArcLift *pLift=NULL);
	void CalEndPos(CLDSArcLift *pLift=NULL);
	virtual void CalPosition(bool bIgnoreRepeatFlag=false);
	virtual double GetLength();	//��λΪmm
	virtual bool IntelliRecogWeldLine();
	virtual bool IsWeldWith(double* lineStartCoord,double* lineEndCoord);
	virtual bool IsWeldWith(f3dArcLine& arcline);
	UCS_STRU BuildUCS();
	BOOL DesignStartJoint();
	BOOL DesignEndJoint();
	BOOL DesignStartJointUCS(CLDSParamPlate *pParamPlate=NULL);
	BOOL DesignEndJointUCS(CLDSParamPlate *pParamPlate=NULL);
	void GetProcessStr(char *notes, size_t max_len);
	virtual double startOdd(bool bToFL=false) const;
	virtual double endOdd(bool bToFL=false) const;
	virtual void CalStartOddment(char ciAutoCalMode = 0,int liInitOddment=0);	//0.��ͨ��ײ������;1.ǿ���԰�ָ����ͷ�������ײ������;2.�������н�ʱ��֤�нǱ߾��������ײ������
	virtual void CalEndOddment(char ciAutoCalMode = 0,int liInitOddment=0);		//0.��ͨ��ײ������;1.ǿ���԰�ָ����ͷ�������ײ������;2.�������н�ʱ��֤�нǱ߾��������ײ������
	//����ֹ�����ͷ�������޸ĸֹ�����ͷ�����ؼ���õ�������ͷֵ	wht 10-04-07
	double CalOddment(BOOL bStart,CLDSParamPlate *pParamPlate,bool bReturnOddToFL=false);
	//double GetWidth(){return size_wide;}
	//double GetThick(){return size_thick;}
	int GetFootNailCount(); //���ظֹ��ϽŶ��ĸ��� wht 11-04-23
	bool IsEqual(CLDSLineTube *pMirTube,double epsilon=1.0,char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);
	CLDSParamPlate *GetParamPlate(int start0_end1=0);	//��ȡ�ֹܶ�ͷ�������� wht 10-11-30
#ifdef __LDS_CONTEXT_
	virtual bool CloneToBomPart(BOMPART* pBomPart);
#endif
	//���Թ���
	DECLARE_PROP_FUNC(CLDSLineTube);
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);		//��������ID�õ�����ֵ
	static void InitLineTubePropHashtable();
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//�õ��������Ե�Ĭ��ֻ��״̬
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//�õ��������Ե�Ĭ��ֻ��״̬
	//ͬ������
	static MEMORY_BLOCK MPCL;	//Mirror Property Control List
	static PROPLIST_TYPE propMirSync;
	static void InitPropMirBitsMap();
	virtual PROPLIST_TYPE* GetSyncPropList(){return &propMirSync;}
	virtual MEMORY_BLOCK GetDefaultMPCL(){return MPCL;}
	virtual bool IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr);
	virtual bool SyncMirProp(const char* propKeyStr=NULL,BYTE ciMirAll0X1Y2Z3=0,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ������
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);//propKeyStr��ʾͬ���Գ�ȫ��λ�öԳ��޹ص�����
	virtual bool SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr=NULL,DWORD dwPropFilterFlag=0xffffffff);
	BOOL SyncMirTransectFaceTo(CLDSLineTube* pDestMirTube,MIRMSG mirmsg);
};
#ifdef __GGG_
// ׶��
class CLDSConeTube : public CLDSLinePart {
	f3dPoint workPlaneNorm;
public:
	int faceN;             //����
	int nNum;               //��������
	double longth;          //����
	TUBEJOINT m_tJointStart,m_tJointEnd;	//ʼ�ն˸ֹܽ�ͷ
	CTubeEndPosPara desStartPos,desEndPos;
	short  m_iNormCalStyle;	//��׼�淨�߼��㷽ʽ 0.ֱ��ָ�� 1.ʹ�ò��ո˼�����
	f3dPoint nearWorkPlaneNorm;	//nearWorkPlaneNorm�����չ����淨�ߵ�ȷ������
	long hWorkNormRefPart;		//���㹤���淨��ʱ�Ĳ��ո˼������Ϊ0������ʼ�˽ڵ���ն˽ڵ�ĸ��˼�Ϊ���ո˼�
	UINT m_uSlices,m_uDisplaySlices;
	f3dPoint basePoint; 
	BOOL m_bHasWeldRoad;
	f3dPoint cutPlaneOrigin,cutPlaneNorm;		//������һ���и��涨��ʵ��
	short  m_iWeldRoadStyle;	//ֻ����ʱ�������ƺ����ߵ���ʾ��ʽ���ñ���
	f3dPoint nearWeldRoadVec;	//���������ڷ���
	CLDSConeTube(void);
	~CLDSConeTube();
public:
	void CalWeldRoadVec(double fAngle);//����ֵΪ�Ƕ�ֵ
	double CalWeldRoadAngle();	//����ֵΪ����
	double GetRadius(){return size_wide;}
	double GetThick(){return size_thick;}
	void SetWorkPlaneNorm(f3dPoint work_norm){workPlaneNorm=work_norm;}
	void CalWorkPlaneNorm();
	void CopyProperty(CLDSConeTube &conetube);
	f3dPoint WorkPlaneNorm(){return workPlaneNorm;}
	UCS_STRU BuildUCS();
	virtual BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=0);//��������ƫ�Ʊ���ʹ��
	
	//���Թ���
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	UPDATE_PROPLISTITEM_FUNC(CLDSConeTube,propHashtable)  //������������º���
	int GetPropValueStr(long id,char* valueStr);		//��������ID�õ�����ֵ
	static void InitConeTubePropHashtable();
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
	// TODO: add your methods here.
};
#endif
// Բ��
class CLDSCylinder : public CLDSLinePart {
public:
	double d;			//Բ��ֱ��
	CLDSCylinder(void);
	// TODO: add your methods here.
};

//<DEVELOP_PROCESS_MARK nameId="CLDSLineSlot">
// �۸�
class CLDSLineSlot : public CLDSLinePart{
	f3dPoint workPlaneNorm;			// �����淨�߷���
public:
	char spec[MAX_CHAR_GUIGE_16+1];
	//double h,b,t;	//h--�� b--�� t--�� 
	f3dPoint nearWorkPlaneNorm;	//nearWorkPlaneNorm�����չ����淨�ߵ�ȷ������
	long hWorkNormRefPart;		//���㹤���淨��ʱ�Ĳ��ո˼������Ϊ0������ʼ�˽ڵ���ն˽ڵ�ĸ��˼�Ϊ���ո˼�
	short m_iNormCalStyle;		//��׼�淨�߼��㷽ʽ 0.ֱ��ָ�� 1.ʹ�ò��ո˼����� 2.������ˮƽ�淽�����ڽŶ�����
	CTubeEndPosPara desStartPos,desEndPos;
	//NAILBASE //������ֹ��ϵĲ۸ֶ�λ���� wht 13-10-17
	long hAttachTube;			//���������ֹܼ���۸ַ���ƫ������hAttachTube>0x20��ʾ�ò۸�Ϊ�Ŷ�����
	CLDSPoint desDatumPoint;	//����desDatumPoint����۸�����λ��
	double fLen;				//�������볤�ȼ���׼��λ�ü���۸�ʼ�ն�λ��

	CLDSLineSlot();
	~CLDSLineSlot();
	double GetHeight(){return size_height;}
	//double GetWidth(){return size.wide;}
	//double GetThick(){return size.thick;}
	void CopyProperty(CLDSDbObject* pObj);
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer, long version = NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	virtual BYTE FuncType();
	//bool bAddSubLevelRelaObj=false:��ʾ������Ӽ���������;true:��ʾ����Ӽ��������� wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//��Эͬ����ͨ���ܵ���ȡ����
	virtual void FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	//ͨ���ܵ���Эͬ����д������
	virtual void ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	int GetSpecification(char *slot_spec,BOOL bIncMat=TRUE);
	//bSolidLength��ʾ���㳤��ʱ�����߼��㻹�ǰ�ʵ�����,��Ҫ����ѧ����ʱͳ��������
	virtual double GetWeight(BOOL bSolidLength=TRUE);	//��λΪkg
	BOOL CalTubeSlotPos(CLDSArcLift *pLift=NULL);
	void CalStartPos(CLDSArcLift *pLift=NULL);
	void CalEndPos(CLDSArcLift *pLift=NULL);
	virtual CRodPosPara* DesStartPos(){return &desStartPos;}
	virtual CRodPosPara* DesEndPos(){return &desStartPos;}
	virtual void CalPosition(bool bIgnoreRepeatFlag=false);
	UCS_STRU BuildUCS();
	void CalWorkPlaneNorm();
	virtual f3dPoint WorkPlaneNorm(){return workPlaneNorm;}
	void SetWorkPlaneNorm(f3dPoint workNorm){workPlaneNorm=workNorm;}
	bool IsEqual(CLDSLineSlot* pMirSlot,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	int IsBaseSlot();	//�Ƿ�Ϊ�۸ֵ�����1:��ʾ�Ŷ�������������ʾ������ͨ�ҽӵ���
	//��ȡ��˨�����۸���Ƥ�Ľ���λ��
	virtual bool GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* btm0_left1_right2=NULL,int iIntersPlane=0);
#ifdef __LDS_CONTEXT_
	virtual bool CloneToBomPart(BOMPART* pBomPart);
#endif
	//����
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	UPDATE_PROPLISTITEM_FUNC(CLDSLineSlot,propHashtable) //������������º���
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
	static void InitLineSlotPropHashtable();
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//�õ��������Ե�Ĭ��ֻ��״̬
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//�õ��������Ե�Ĭ��ֻ��״̬
};
//</DEVELOP_PROCESS_MARK>

// ���
class CLDSLineFlat : public CLDSLinePart{
	f3dPoint workPlaneNorm;			// �����淨�߷���
public:
	f3dPoint nearWorkPlaneNorm;	//nearWorkPlaneNorm�����չ����淨�ߵ�ȷ������
	long hWorkNormRefPart;		//���㹤���淨��ʱ�Ĳ��ո˼������Ϊ0������ʼ�˽ڵ���ն˽ڵ�ĸ��˼�Ϊ���ո˼�
	short m_iNormCalStyle;		//��׼�淨�߼��㷽ʽ 0.ֱ��ָ�� 1.ʹ�ò��ո˼�����
	CTubeEndPosPara desStartPos,desEndPos;
	CLDSLineFlat();
	~CLDSLineFlat();
	void SetWorkPlaneNorm(f3dPoint work_norm){workPlaneNorm=work_norm;}
	void CopyProperty(CLDSDbObject* pObj);
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer, long version = NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	//bool bAddSubLevelRelaObj=false:��ʾ������Ӽ���������;true:��ʾ����Ӽ��������� wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//��Эͬ����ͨ���ܵ���ȡ����
	virtual void FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	//ͨ���ܵ���Эͬ����д������
	virtual void ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	//bSolidLength��ʾ���㳤��ʱ�����߼��㻹�ǰ�ʵ�����,��Ҫ����ѧ����ʱͳ��������
	virtual double GetWeight(BOOL bSolidLength=TRUE);	//��λΪkg
	void CalStartPos(CLDSArcLift *pLift=NULL);
	void CalEndPos(CLDSArcLift *pLift=NULL);
	virtual CRodPosPara* DesStartPos(){return &desStartPos;}
	virtual CRodPosPara* DesEndPos(){return &desStartPos;}
	virtual void CalPosition(bool bIgnoreRepeatFlag=false);
	//double GetWidth(){return size.wide;}
	//double GetThick(){return size.thick;}
	UCS_STRU BuildUCS();
	void CalWorkPlaneNorm();
	virtual f3dPoint WorkPlaneNorm(){return workPlaneNorm;}
	bool IsEqual(CLDSLineFlat* pMirFlat,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);//error_info>50 characters
	//���Թ���
	static int m_iCurDisplayPropGroup;					//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;		//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;		//����״̬��ϣ��
	UPDATE_PROPLISTITEM_FUNC(CLDSLineFlat,propHashtable) //������������º���
	static void InitLineFlatPropHashtable();
	static int GetPropStr(long id,char* propStr);		//�õ������ַ���
	static long GetPropID(char* propStr);				//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//�õ��������Ե�Ĭ��ֻ��״̬
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//�õ��������Ե�Ĭ��ֻ��״̬
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
};
#ifdef __H_SHAPE_STEEL_
// H�͸�
class CLDSLineHShape : public CLDSLinePart{
	f3dPoint workPlaneNorm;			// �����淨�߷���
public:
	BYTE m_ibHShapeType;		//0.HW(����Ե) 1.HM(����Ե) 2.HN(խ��Ե)  3.HP(����Ե)
	char spec[MAX_CHAR_GUIGE_16+1];
	f3dPoint nearWorkPlaneNorm;	//nearWorkPlaneNorm�����չ����淨�ߵ�ȷ������
	long hWorkNormRefPart;		//���㹤���淨��ʱ�Ĳ��ո˼������Ϊ0������ʼ�˽ڵ���ն˽ڵ�ĸ��˼�Ϊ���ո˼�
	short m_iNormCalStyle;		//��׼�淨�߼��㷽ʽ 0.ֱ��ָ�� 1.ʹ�ò��ո˼�����
	CTubeEndPosPara desStartPos,desEndPos;
public:
	CLDSLineHShape();
	~CLDSLineHShape();
	//���ݴ洢
	void IndependentShadowPropFromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void IndependentShadowPropToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void FromBuffer(CBuffer &buffer, long version = NULL,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);//��Эͬ����ͨ���ܵ���ȡ����
	void ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord);	//ͨ���ܵ���Эͬ����д������
	//bool bAddSubLevelRelaObj=false:��ʾ������Ӽ���������;true:��ʾ����Ӽ��������� wjh-2017.12.10
	virtual void AddToConsoleObjBuf(bool bAddRelaObj=false,bool bAddSubLevelRelaObj=true);
	//ʵ��+��λ����
	int GetSpecification(char *spec,BOOL bIncMat=TRUE);
	//bSolidLength��ʾ���㳤��ʱ�����߼��㻹�ǰ�ʵ�����,��Ҫ����ѧ����ʱͳ��������
	double GetWeight(BOOL bSolidLength=TRUE);	//��λΪkg
	void CalStartPos(CLDSArcLift *pLift=NULL);
	void CalEndPos(CLDSArcLift *pLift=NULL);
	void CalPosition(bool bIgnoreRepeatFlag=false);
	void SetWorkPlaneNorm(f3dPoint work_norm){workPlaneNorm=work_norm;}
	void CopyProperty(CLDSDbObject* pObj);
	BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	UCS_STRU BuildUCS();
	void CalWorkPlaneNorm();
	virtual f3dPoint WorkPlaneNorm(){return workPlaneNorm;}
	bool IsEqual(CLDSLineHShape* pMirHShape,double epsilon=1.0, char *error_info=NULL,
		int *pErrorInfo=NULL,DWORD *pChkStyleFlag=NULL);
	//���Թ���
	static int m_iCurDisplayPropGroup;					//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;		//�����ַ�����ϣ��
	static CXhChar100 HELPLINK_URL;
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;		//����״̬��ϣ��
	UPDATE_PROPLISTITEM_FUNC(CLDSLineHShape,propHashtable) //������������º���
	static void InitLineHShapePropHashtable();
	static long GetPropID(char* propStr);				//�õ�����ID
	static PROPLIST_ITEM* GetPropItem(long id);
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
	static BOOL GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode);	//�õ��������Ե�Ĭ��ֻ��״̬
	virtual BOOL GetObjPropReadOnlyDefaultState(long id);	//�õ��������Ե�Ĭ��ֻ��״̬
	int GetPropValueStr(long id,char* valueStr);		//��������ID�õ�����ֵ
};
#endif
class CLDSExternalPart : public CLDSPart
{
public:
	CLDSExternalPart();
	//double volume;
	//fPtList Vertexs;								//�洢����
	//fPtList VertexNormals;							//
	//ATOM_LIST< ATOM_LIST<int> > FaceVertexIndexs;	//�洢ÿ���������ֵ
	virtual BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
};
//��Ե��
class CLDSInsulator : public CLDSExternalPart
{
public:
	CLDSInsulator(void);
	CXhChar50 sName;	//����
	f3dPoint pos;		//λ��
};
//���
class CLDSFitting : public CLDSExternalPart
{
public:
	static const BYTE TYPE_EB = 0;	//EB�ҽӽ��
	static const BYTE TYPE_GD = 1;	//GD�ҽӽ��
	static const BYTE TYPE_UR = 2;	//U�͹ҽӻ�(Ring)
	static const BYTE TYPE_US = 3;	//U����˿
	static const BYTE TYPE_UB = 4;	//U�͹Ұ�
	BYTE m_ciFittingType;	//0.EB���;1.GD���;2.U�͹ҽӻ�;3.U����˨
	//ֱ�����������ӵ�һ���ҽӽ�ߣ�EB�ҽӽ�߻�GD�ҽӽ�ߣ�
	short m_iLiveSwingAngle;//һ���ҽӽ�ߵ�ǰ�ڶ���ֵ,��
	short m_siMinSwingAngle,m_siMaxSwingAngle;	//��߰ڶ��Ƿ�Χ
	//��һ���ҽӽ�����Ӻ���ڶ�U�͹һ�
	short m_iURLiveSwingAngle;//U�����ӹһ���ʵʱ�ڶ���
	short m_siMinURSwingAngle,m_siMaxURSwingAngle;	//U�ͻ���߰ڶ��Ƿ�Χ
	CXhChar50 sizespec;	//�ͺ�
	CXhChar50 sizespecUR;//��������U�ͻ����ͺ�
	struct HOLE{
		long hBolt,hResidePart;
		HOLE(){hBolt=hResidePart=0;}
	}holes[2];	//ʵ��ҿף�һ����1~2�������
	CHashListEx<CLDSGeneralPlate::DESIGN_PARAM>params;			
	struct BASICINFO {
		char   szSizeCode[16];	//�ͺŴ���
		short  siBoardThick;	//�Ұ���b
		double dfBoardL;		//���(�Ұ岿��)���
		//short  siThroughThick;//Ԥ�����߽Ǹ�֫(ͨ)��
		double dfTrunnionD;		//����ֱ��
		double dfTrunnionL;		//�����ܳ�
		double dfHoleD;			//�ҿ�ֱ��(diameter)��
		double dfHole2TrunnionH;//�ҿ����ĵ����������߾���H
		BASICINFO();
	}basic;
public:
	CLDSFitting();
	CLDSFitting(char ciFittingType,const char* szFittingSizeCode);
	~CLDSFitting();
	//
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual bool IsExternalPart() const{return true;}
	virtual BOOL Create3dSolidModel(BOOL bDisplayHole=FALSE,double scale_of_user2scr=1.0, double sample_len=5.0,int smoothness=36);
	virtual CSolidPart GetSolidPartObject();
	void DesignFitting();
	void CopyFrom(CLDSFitting* pSrcFitting);
	bool MatchUring(DWORD dwReservedMatchParam=0);	//����һ���ҽӽ���еĹҿ�ֱ��ƥ����ʵ�U�λ��ͺ�
	//����һ���ҽӽ���еĹҿ�ֱ��ƥ����ʵ�U�λ��ͺ�
	static CXhChar50 MatchUringSizeCode(char ciFittingType,const char* szFittingSizeCode);
	static BASICINFO LoadFittingBasicInfo(char ciFittingType,const char* szFittingSizeCode);
	//��������ʾ
	DECLARE_PROP_FUNC(CLDSFitting);
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ	
};

//������ͼ
class CPartGroupLocatePoint
{
	//��׼����Ϊ�˼�ʱ���� �궨��ʼ/�ն�ƫ���� ��Ҫ���ڸֹܶ����ӷ������������ʱ��������
	double m_fStartOffset,m_fEndOffset;	
public:
	//�궨�����������ͼ�л�׼�˼���ͼ����ԭ�������λ��
	double m_fLenPosToDatumPart,m_fVertPosToDatumPart;
	//�궨��������ͼ��ͼ����ϵ�е�����λ��
	f3dPoint m_xLocalCoordPos;
	long m_hPart;		//��׼������Ҫ���ڱ�ע�ֲ���϶�ߴ���
	int m_iOrderType;	//�궨�㼶��:0���涨λ���1������λ�ߴ��2�ֲ���϶�ߴ�3�Ŷ��ߴ�
	bool m_bDimSpace;	//�Ƿ�Ϊ���Գߴ�궨��
	IModel *pTower;	//��ʱ�������洢����������λ��
	UCS_STRU dcs;	//��ʱ�������洢����������λ��
public:
	CPartGroupLocatePoint();
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
};
class CCheckLineDimension
{
public:
	bool m_bSpecStartLocatePart,m_bSpecEndLocatePart;	//�Ƿ�ָ����λ���� 
	long m_hStartPart,m_hEndPart;						//��λ���� 
	//��ʼ���� ��ֹ���� ָ����λ����:�������->��������->��ͼ����  δָ����λ����:��ͼ����
	f3dPoint m_xStartLocalCoordPos,m_xEndLocalCoordPos;	
	bool m_bBeforeWeld;	//��ǰ�����/���Ӻ�����
	CCheckLineDimension();
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
};

class CAngularDimension
{
public:
	bool m_bStartLineIsDatumLine;	//TRUE �Ի�׼��Ϊ��ʼ��   FALSE �Ժ�����Ϊ��ʼ��
	long m_hStartPart,m_hEndPart;	//Ҫ��ס�нǵ���������
	double m_fDimR;					//��ע�߰뾶
	CAngularDimension();
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
};
//��ͼ����	��DefineView.h��ֲ
class CDrawingAssistant	
{ 
	//��������������ͼʱ��ʱʹ�ò��ô洢 wht 11-08-08
	struct ROTAXISLINE{
		GEPOINT startPt,endPt;
	}m_xRotateAxisLine;	//��ת��
protected:
	double m_fRotateAngle;		//��ת�Ƕ�
	GEPOINT m_xDrawNorm;		//��ͼ�淨��,�����ж��Ƿ���Ҫ���¼�����ת�ἰ��ת�Ƕ�
	union {
		UINT _uiType;
		struct{
			WORD _wStyle;	//�൱��֮ǰ��m_nStyle;
			WORD _wFlag;	//��Ե�ǰ��Style���͵Ĳ�������,��wStyle=0&&wFlag&0x0001ʱ��ʾm_xLineEnd������m_xFaceNorm;
		};
	};
	union {
		UINT uiSpreadPlane;	//0.��ʾ��ͼ����,����>0ֵ��ʾ����ͼ����Ӧ��չ����;;-1��ʾչ������������,-2��ʾչ����������,-3��ʾչ��������ǰ��,-4��ʾչ�������ܱ��� wjh-2017.11.25
		struct{
			WORD _wiResidePlane;	//չ����ƽ��ı�ʶId
			BYTE _ciSpreadDirection;//չ�����Ŀ�귽λ1.�����ϲ�2.�����²�3.����ǰ��4.���ܱ���
			bool _biSpreadOverRidge;//��תչ��ʱ����Խ�˸ֹ�������
		};
	};
public:
	CDrawingAssistant();
	//
	CLDSPoint m_xLineStart,m_xLineEnd;
	CLDSVector m_xFaceNorm;
	/*union{
		//�������߻������
		struct ASSIST_LINE{POINT_PARA xLineStart,xLineEnd;}line;
		//�ƻ�ͼƽ�������ߣ������ߣ���תչ��
		struct ASSIST_PLANE{
			VECTOR_PARA xPlaneNormPara;
			POINT_PARA xLineStart;	//CLDSPoint m_xLineStart,m_xLineEnd;
			POINT_PARA xLineEnd;	//CLDSVector m_xFaceNorm;
		}plane;
		//����������תչ��
		struct ASSIST_ROTPLANE{
			VECTOR_PARA xRotAxisPara;
			POINT_PARA xLineStart;
			double fRotAngle;	//������
		}rotaion;
	};*/
	UINT iNo;
	static const WORD SPREAD_TO_UPPER	= 1;
	static const WORD SPREAD_TO_UNDER	= 2;
	static const WORD SPREAD_TO_TOP		= 3;
	static const WORD SPREAD_TO_BACK	= 4;
	DECLARE_PROPERTY(bool,bSpreadOverRidge);	//��תչ��ʱ����Խ�˸ֹ�������
	DECLARE_PROPERTY(BYTE,ciSpreadDirection);	//չ�����Ŀ�귽λ1.�����ϲ�2.�����²�3.����ǰ��4.���ܱ���
	DECLARE_PROPERTY(long,iResidePlaneNo);		//0.��ʾ��ͼ����,����>0ֵ��ʾ����ͼ����Ӧ��չ����;;-1��ʾչ������������,-2��ʾչ����������,-3��ʾչ��������ǰ��,-4��ʾչ�������ܱ��� wjh-2017.11.25
	SEGI m_iSeg;
	char name[10];	//����
	void SetKey(DWORD key){iNo=key;}
	DECLARE_PROPERTY(UINT,m_nStyle);	//0.��תչ���� 1.�������� 2.���������� 3.�����ᣨxLineStart->xLineEnd)��תչ��
	static const WORD ENABLE_LINE_END = 0x0001;		//m_xLineEnd������m_xFaceNorm;
	static const WORD SPREAD_TO_DMPLANE = 0x0002;	//���۽��ƹ��滹�Ǵ�ֱ����ǿ��ת������׼ƽ��
	//��Ե�ǰ��Style���͵Ĳ�������,��wStyle=0&&wFlag&0x0001ʱ��ʾm_xLineEnd������m_xFaceNorm;
	//wStyle=0&&wFlag&0x0002ʱ��ʾ�����ǽ��ƹ��滹�ǽ��ƴ�ֱ����ǿ����תչ������ǰ��׼ƽ�棬���������չ������׼ƽ���ֱ�ڻ�׼ƽ�桡wjh-2017.11.8
	DECLARE_PROPERTY(WORD,wStyleFlag);
	DECLARE_PROPERTY(double,m_fRotRadAng);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void GetRotateAxisAndAngle(f3dPoint draw_norm,f3dLine &rotate_axis,double &rotate_angle,IModel *pTower=NULL);
};
//��ͼ:����ͼ			==>{����ͼ,��ͨ����ͼ}		   
//	   ����ͼ==>�ڼ���ͼ==>{�ڵ����ͼ(�޻����)�������߰���ͼ����������ͼ����Խ������ͼ}
//			    �ܶ���ͼ==>{������ͼ���������ͼ�������ͼ}	
//				�Ŷ���ͼ==>{�ǸֽŶ����۸ֽŶ�}	
//						==>{TMA���Ű�/ţ���}			
//�麸����ͼ���ͣ�1.����ͼ��2.����ͼ��3.�ڵ�����ͼ��4.�����߰���ͼ��5.��������ͼ��
//				  6.��Խ������ͼ,7.������ͼ��8.�������ͼ��9.�����ͼ,10.�ǸֽŶ�,
//				  11.�۸ֽŶ���, 12.���Ű�,13.֧�ܶ���ͼ,14.�ԽӸֹܼ�ͼ,15.K�ڵ�����ͼ,100.�û�������ͼ;
enum SECTION_VIEW_TYPE{VIEW_MAIN=1,VIEW_FRONT=2,VIEW_NODEPLATE=3,VIEW_CIRCOVERPLATE=4,VIEW_CIRRIBPLATE=5,
					VIEW_TUBERIBPLATE=6,VIEW_FL=7,VIEW_CIRPLATE=8,VIEW_INSERTPLATE=9,VIEW_FOOTNAILANGLE=10,
					VIEW_FOOTNAILSLOT=11,VIEW_FOOT=12,VIEW_JOINSECTION=13,VIEW_JOINTTUBE=14,VIEW_KNODESKETCH=15,VIEW_USERDEF=100};
#include "LifeObj.h"
struct DRAWFLAG{
	BYTE cDrawFlag;	//��������ģʽ��ˮƽ'H'����ֱ'V'
public:
	static const BYTE AUTO_JUSTIFY			= 0;	//��ȫ�Զ��жϣ��麸������ͼ��Ҳ���ܹ��˵������ƣ�
	static const BYTE AUTO_DRAW				= 1;	//���Ʒ�ʽ�Զ��жϣ��麸������ͼ��һ�����ƣ�
	static const BYTE VIRTUAL_DRAW			= 0x80;	//���߻���
	static const BYTE ANGLE_WING_X_INWARD	= 'X';	//X֫��������
	static const BYTE ANGLE_WING_X_OUTWARD	= 'x';	//X֫��������
	static const BYTE ANGLE_WING_Y_INWARD	= 'Y';	//Y֫��������
	static const BYTE ANGLE_WING_Y_OUTWARD	= 'y';	//Y֫��������
	static const BYTE ANGLE_SECT_Z			= 'Z';	//����ͼ����
	static const BYTE PLATE_WORKPLANE		= 'X';	//��׼����ƽ��ƽ��(������ͶӰ)���ƻ���
	static const BYTE PLATE_SPREADPLANE		= 'S';	//�������Ļ�����չ������
	static const BYTE PLATE_SECT_Z			= 'Z';	//��ֱ����ͼ����
	static const BYTE PART_HORIZON			= 'H';	//����ƽ��ͼ����
	static const BYTE PART_VERTICAL			= 'V';	//����ֱ��(����)ͼ����
public:
	DRAWFLAG(BYTE cPreferDrawFlag=0){cDrawFlag=cPreferDrawFlag;}
	//����ֵ��0.ƽ�̻��� 1.������� 2.���ƶ���,
	//���ز�����cActualDrawFlag��ʵ��ִ�еĻ���ģʽ
	int GetPartDrawType(CLDSPart* pPart,double* drawPlaneNorm,BYTE* cExecuteDrawFlag=NULL);
};

class CDrawCoordTrans
{
	UCS_STRU _cs;
public:
	double m_fDrawScale;			//��ͼ������,����λ��ͼ�����������ʵ�ʳ���,��1:20ʱm_fDrawScale=20.0
	double m_fSpreadAngle;			//չ���Ƕȣ�������
	f3dPoint m_xSpreadPlaneNorm;	//չ���淨��
	f3dLine m_xSpreadLine;			//չ����ת����
	struct SPREAD_PLANE{
		double fRotateAngle;	//չ��ʱ��ת�Ƕ�, ������
		GEPOINT vNormal;
		GEPOINT xRotAxisStart,xRotAxisEnd;
	}second;
public:
	CDrawCoordTrans();
	CDrawCoordTrans(UCS_STRU& dcs,double m_fDrawScale=1.0);
	//UCS_STRU CS(){return cs;}
	f3dPoint PrjNorm() const;
	f3dPoint PrjOrigin() const;
	//�趨��ǰ��ͼת������ϵ
	bool SetCS(UCS_STRU& cs);
	//���ص�ǰ��ͼת������ϵ
	operator UCS_STRU() const{return _cs;};
	//srcPosΪչ��ǰ������ϵ�µ�����,���ػ�ͼ����(��ACAD�µ�����)
	f3dPoint TransToDraw(f3dPoint srcPos,bool clearCoordZ=true);
	//srcVecΪչ��ǰ������ϵ�µ�ʸ��,���ػ�ͼʸ��(��ACAD�µ�����)
	f3dPoint TransVToDraw(f3dPoint srcVec);
	//drwPosΪ��ͼ����ϵ�µ�����,������ģ������ϵ�����꣨��;�����ݲ�ʵ�֣�
	//f3dPoint TransFromDraw(const double* drwPos);
	//drwVecΪ��ͼ����ϵ�µ�ʸ��,������ģ������ϵ��ʸ��
	f3dPoint TransVFromDraw(const double* drwVec);
public:	//����
	READONLY_PROPERTY(UCS_STRU,cs);
	GET(cs){return _cs;}
};
class CSlaveSectionView;
class CSectionView
{
	friend class CLDSPartGroup;
protected:
	int m_id;
	int _idViewType;	//���Ա�ʶ�麸����ͼ����𣬽����Ჿ�ֻ�ȫ������m_iViewType��
	SECTION_VIEW_TYPE _iLegacyViewType;
	CLDSPartGroup *m_pPartGroup;	//��ͼ�������麸��
	struct XHSTR_LEN2 {static const int BUF_LEN =  3;};
public:
	static const int CLS_BASIC_VIEW = 0;
	static const int CLS_SLAVE_VIEW = 1;
	static const int CLS_STDX_PLATES_VIEW	= 2;//ʮ�ֽ��������麸ͼ�����ษ��ͼ����ͨ��չ��ͼ������չ��ͼ����������ͼ��
	static const int CLS_ANGLE_FOOT_VIEW	= 3;//�Ǹ����Ű���ͼ
	static const int CLS_KAOME_ANGLE_VIEW	= 4;//��ģ�Ǹ���ͼ
	static const int CLS_ANGLE_SPREAD_VIEW	= 5;//�Ǹ�չ����ͼ(�ԽǸֹ��տ�ģʽ����)
	static const int CLS_JOINT_TUBES_VIEW	= 6;//�Ǹ�չ����ͼ
	static const int CLS_SLAVE_NODESECT_VIEW = 7;
	static const int CLS_HORIZ_BEAM_VIEW	= 8;//���ǹ����������տ�ˮƽ�ֹ����麸ͼ
	static const int CLS_DUALTUBE_JOINT_VIEW= 9;//˫�ֹܶԽ��������Ϊ���Ӹְ��麸����ͼ
	static const int CLS_BUSH_PLATE_VIEW	= 10;	//�ֹܳ��а���ͼ	wht 18-03-10
public:
	UCS_STRU ucs;
	CParaCS des_cs;
	char sName[MAX_TEMP_CHAR_50+1];
	int index;		//��ͼ����  
	BOOL m_bLocked;	//������ͼ  TRUE�������麸��ʱ��������ͼ FALSE�������麸��ʱ������ͼ wht 09-06-23
	int feature;	//�����û��Լ�����Ϣ(��ʱ����,���ɳ־ô洢��Ҳ��һ������)
	struct PART_ITEM{
		union {
			UINT uiSpreadPlane;	//���ڻ�ͼƽ�����к�:>0��ʾλ��ָ��ƽ��;-1��ʾչ������������,-2��ʾչ����������,-3��ʾչ��������ǰ��,-4��ʾչ�������ܱ��� wjh-2017.11.25
			struct{
				WORD _wiResidePlane;	//չ����ƽ��ı�ʶId
				BYTE _ciSpreadDirection;//չ�����Ŀ�귽λ1.�����ϲ�2.�����²�3.����ǰ��4.���ܱ���
				bool _biSpreadOverRidge;//��תչ��ʱ����Խ�˸ֹ�������
			};
		};
		static const WORD SPREAD_TO_UPPER	= 1;
		static const WORD SPREAD_TO_UNDER	= 2;
		static const WORD SPREAD_TO_TOP		= 3;
		static const WORD SPREAD_TO_BACK	= 4;
		DECLARE_PROPERTY(bool,bSpreadOverRidge);	//��תչ��ʱ����Խ�˸ֹ�������
		DECLARE_PROPERTY(BYTE,ciSpreadDirection);	//չ�����Ŀ�귽λ1.�����ϲ�2.�����²�3.����ǰ��4.���ܱ���
		DECLARE_PROPERTY(long,iResidePlaneNo);		//0.��ʾ��ͼ����,����>0ֵ��ʾ����ͼ����Ӧ��չ����;;-1��ʾչ������������,-2��ʾչ����������,-3��ʾչ��������ǰ��,-4��ʾչ�������ܱ��� wjh-2017.11.25
		long hSonPart;
		char cFlag;		//����ģʽ��0 �Զ��ж�  'X':�Ǹ�X֫/�ֹ�����/�����  'Y'�Ǹ�Y֫  'Z'�Ǹֽ���/�ֹܽ���/������ 'S'���ƹ���
		ITAG *pTagInfo; //��ʱʹ�ò��洢����ʡȥÿ�α�����������ʱ�ظ����ҵ�ǰ����ָ�� wht 09-12-12
		void SetTagInfo(ITAG* pTag){pTagInfo=pTag;}
		void SetKey(DWORD key){hSonPart=key;}
		PART_ITEM() {pTagInfo=NULL;uiSpreadPlane=hSonPart=cFlag=0;}
		~PART_ITEM(){
			if(pTagInfo) pTagInfo->DetachObject();
		}
	};
	struct ITEM_TAG_INFO : public TAG_INFO<PART_ITEM>{
	public:
		DRAWFLAG cDrawMode;
		CLDSPart *pCurPart;
	public:
		ITEM_TAG_INFO();
		virtual ~ITEM_TAG_INFO(){DetachObject();}
	};
	struct RELA_RIBPLATE{
		CLDSParamPlate* pPlate;
		long hDatumPlate;
	};
	CHashListEx<PART_ITEM> SonParts;	//��ֵPART_ITEM::hSonPart
	CHashList<CDrawingAssistant> hashAssistPlanes;	 //չ����
	//��PART_ITEM�Ѿ��йҽӶ���ʱִ��TagInfo(...), ���򴴽��¹ҽ����ݶ���󷵻�
	ITEM_TAG_INFO* LoadTagInfo(PART_ITEM* pItem);
	void SetBelongPartGroup(CLDSPartGroup *pPartGroup) {m_pPartGroup=pPartGroup;}
	CLDSPartGroup* BelongPartGroup() { return m_pPartGroup; }
public:	//ģ���������ͼ�����ı任
	struct ROTATION{
		double  m_fRotAngle;	//��ʵ���������ͼ����任�����е���ת�Ƕȣ�������
		GEPOINT xRotAxisStart,xRotAxisEnd;	//չ����ת����
		ROTATION(){m_fRotAngle=0;}
	};
	static GECS TransRotationsToCS(IXhEnumerator *pIter,bool bRotRightHand=true,bool bFowardIter=true);
	bool GetSpreadPlaneRotCS(int iAssistPlaneNo,GECS* pRotCS,IModel *pTower=NULL);
	GEPOINT TransPToDCS(const double* coord3d,GECS* pFromRotPlaneCS=NULL);
	GEPOINT TransPFromDCS(const double* coord3d,GECS* pToRotPlaneCS=NULL);
	GEPOINT TransVToDCS(const double* vector3d,GECS* pFromRotPlaneCS=NULL);
	GEPOINT TransVFromDCS(const double* vector3d,GECS* pToRotPlaneCS=NULL);
public:	//����
	PROPERTY(SECTION_VIEW_TYPE,m_iViewType);	//��ģʽ����ͼ����
	VIRTUAL_SET(m_iViewType);
	VIRTUAL_GET(m_iViewType);
	READONLY_PROPERTY(int,m_idViewType);//���Ա�ʶ�麸����ͼ����𣬽����Ჿ�ֻ�ȫ������m_iViewType��
	VIRTUAL_GET(m_idViewType){return _idViewType;}
	READONLY_PROPERTY(CSlaveSectionView*,pSlaveView);//���Ա�ʶ�麸����ͼ����𣬽����Ჿ�ֻ�ȫ������m_iViewType��
	VIRTUAL_GET(pSlaveView);
	virtual const char* get_internalSymbol();
	READONLY_PROPERTY(const char*,Symbol);//���Ա�ʶ�麸����ͼ����𣬽����Ჿ�ֻ�ȫ������m_iViewType��
	GET(Symbol){return get_internalSymbol();}
private:
	CHashList<bool>hashDualSideSlotNails;
	ATOM_LIST<ITEM_TAG_INFO>listTagInfo;	//��ʱ�ҽӵĸ��ӵ�������{PART_ITEM}�ϵ����ݶ�������
	//�ж��Ƿ�������֧��
	BOOL IsExistIntersectTube(CLDSLineTube *pDatumTube,IModel *pTower);
	//������֧�����׼�ֹ�ʼĩ�˵ļ����
	void AppendIntersectTubeCheckLines(CLDSLineTube* pDatumTube,IModel* pTower);
public:
	double m_fOrgLenOffset;	//���ڻ�׼����Ϊ�ֹܵ��麸ͼ����ͼԭ��(ʼ�������)����ֹ�ԭ��ĳ��ȷ���ƫ��
	CSectionView(int id=0);
	virtual ~CSectionView(){;}	//����ȥ��������ָ������ת��ʱ����ֵ�ַ�任�쳣 wjh-2014.3.29
	virtual int GetViewTypeId(){return m_idViewType;}
	virtual bool IsMainView(){return _iLegacyViewType==VIEW_MAIN;}
	virtual bool IsDualTubeJointView(){return false;}
	virtual bool GetDCS(long hPart,CDrawCoordTrans &dcs);
	bool IsDualSideSlotNail(long hSlot){return hashDualSideSlotNails.GetValue(hSlot)!=NULL;}
	ATOM_LIST<CPartGroupLocatePoint>LocatePoints;	//��ʱ�ɲ��洢
	ATOM_LIST<CCheckLineDimension>CheckLines;		//��ʱ�ɲ��洢
	ATOM_LIST<CAngularDimension>Angulars;			//��ʱ�ɲ��洢
	//���Ĭ�ϵı궨��
	virtual void AddDefaultLocatePoints(CLDSPart *pDatumPart,IModel *pTower);
	//���Ĭ�ϵĽǶȱ�ע��
	virtual void AddDefaultAngulars(CLDSPart *pDatumPart,IModel *pTower);
	//���Ĭ�ϵļ����
	virtual void AddDefaultCheckLines(CLDSPart *pDatumPart,IModel *pTower);
	//������ͼ���Ͳ��ҵ�ǰ��ͼ���������������
	CLDSPart *GetMainPartHandle(IModel *pTower);
	//�жϵ�ǰ��ͼ�Ƿ�Ϊ�ֹܱ�׼��ͼ
	BOOL IsBriefTubeView(IModel* pTower);
	//�жϵ�ǰ��ͼ�Ƿ�Ϊ�ֹ�������ͼ wht 18-08-22
	BOOL IsTubeTowerFootView(IModel* pTower,CLDSPlate **ppTowerFootPlate=NULL);
	//���ָ����������ǰ��ͼ��������	wht 10-08-10
	CSectionView::PART_ITEM *AddPart(long hSonPart,CLDSPart *pSonPart=NULL,int cFlag=0);
	virtual void ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	int GetId(){return m_id;}
	__declspec( property(get=GetId)) int Id;
	void CopyProperty(CSectionView *pSrcView);
	UCS_STRU UpdateUCS(IModel *pModel,CLDSPartGroup *pPartGroup);
	virtual int SmartFilterNeedDrawParts(IModel *pModel,PARTSET& partset);	//���˳���Ҫ���ƵĹ�������
	virtual bool IsHostView() { return true;}
	virtual bool IsSlaveView(){return false;}
	virtual bool IsTubeSectionView(CLDSLineTube** ppSectTube=NULL){return false;}
	virtual char GetViewSubType(){return 0;}
	//��������ͼ����
	static CHashStrTable<long>propHashtable;	//�����ַ�����ϣ��
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
	static void InitViewPropHashtable();
	static int GetPropStr(long id,char* propStr);		//�õ������ַ���
	static long GetPropID(char* propStr,...);			//�õ�����ID
	static BOOL IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
};
CSectionView::ITEM_TAG_INFO* TagInfo(CSectionView::PART_ITEM* pItem);
CLDSPart* TagPart(CSectionView::PART_ITEM* pItem);
class CJointTubesView : public CSectionView
{	//�����Ժ��ֹ�Ϊ�����麸������ͼ(�������Ժ��ֹܽ�����Ϊ��ͼƽ��)
public:
	char m_ciJointsViewType;	//0:չʾ�ԽӸֹܼнǵĸ���ͼ;1:չʾ�ԽӸֹ�ֱ�ߵ�ƽ��ͼ;2:�������ĶԽ���������ͼ
	long m_hRootTube;			//��׼�ֹܣ����̲���ʱ�Խϳ���Ϊ��׼�ֹܣ�
	long m_hJointTube;			//�ԽӸֹܣ����̲���ʱ�Խϳ���Ϊ��׼�ֹܣ�
	CJointTubesView(int id=0);
	bool UpdateJointViewType();	//�Զ�����m_ciJointsViewType������ֵ wjh-2018.3.6
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual bool IsMainView(){return true;}
public:	//����
	READONLY_PROPERTY(SECTION_VIEW_TYPE,m_iViewType);	//��ģʽ����ͼ����
	VIRTUAL_GET(m_iViewType);
};
class CStdXPlatesView : public CSectionView
{
public:
	char m_cSubType;			//'F'����ͼ;'S':����ͼ;'D':����ͼ
	long m_hBasePlate;			//ͨ����
	long m_hFrontVertPlate;		//��������
	long m_hBackVertPlate;		//��������
	long m_arrAngleHandle[4];	//�����Ǹ־��
	CXhCharTempl<XHSTR_LEN2> symbol;	//����ͼ���ϵı�ע����
	CStdXPlatesView(int id=0);
	virtual char GetViewSubType(){return m_cSubType;}
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual const char* get_internalSymbol(){return symbol;}
};
struct STDX_PLATES_PARTGROUP
{
	CLDSPlate* m_pBasePlate;			//ͨ��
	CLDSPlate* m_pFrontVertPlate;		//��������
	CLDSPlate* m_pBackVertPlate;		//��������
	CLDSLineAngle* m_arrAnglePtr[4];	//�����Ǹ�
	STDX_PLATES_PARTGROUP(){memset(this,0,sizeof(STDX_PLATES_PARTGROUP));}
};
struct ANGLE_FOOT_PARTGROUP
{
	CLDSGeneralPlate* m_pFootPlate;
	CLDSGeneralPlate* m_pShoePlate;			//��ͨ(���)ѥ��
	CLDSGeneralPlate* m_pRightShoePlate;	//�Ҳ�ѥ��
	CLDSGeneralPlate* m_pFrontShoePlate;	//ǰ��ѥ��
	CLDSGeneralPlate* m_pBackShoePlate;		//���ѥ��
	CLDSLineAngle* m_arrAnglePtr[4];		//�����Ǹ�
	ANGLE_FOOT_PARTGROUP(){memset(this,0,sizeof(ANGLE_FOOT_PARTGROUP));}
};
class CAngleFootView : public CSectionView
{
public:
	char m_cSubType;			//'F'����ͼ;'S':����ͼ;'D':����ͼ
	long m_hFootPlate;			//���Ż���
	long m_hShoePlate;			//��ͨ(���)ѥ��
	long m_hRightShoePlate;		//�Ҳ�ѥ��
	long m_hFrontShoePlate;		//ǰ��ѥ��
	long m_hBackShoePlate;		//���ѥ��
	long m_arrAngleHandle[4];	//�����Ǹ־��	
	long m_hDownLeftShoePlate;	//ţ������²�ѥ��
	long m_hDownRightShoePlate;	//ţ������²�ѥ��
	long m_hDownFrontShoePlate;	//ţ���ǰ�²�ѥ��
	long m_hDownBackShoePlate;	//ţ�����²�ѥ��
	long m_arrDownAngleHandle[4];	//ţ�������Ǹ־��
	CXhCharTempl<XHSTR_LEN2> symbol;	//����ͼ���ϵı�ע����
	CAngleFootView(int id=0);
	bool IsHoofView(){return m_hDownLeftShoePlate>0;}
	virtual char GetViewSubType(){return m_cSubType;}
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual const char* get_internalSymbol(){return symbol;}
};
class CDualTubeJointView : public CSectionView
{
protected:
	BYTE m_cTemporyJointState;	//��ʱ�洢�����ֹ����ĶԽ�״̬
	bool m_bMasterTubeOnLeft;	//ˮƽ��׼�ֹ����Ƿ����������������
public:
	long m_hTemporyStartConnPlate;	//��ʱ�洢�����ֹ����Ķ�ͷ���Ӹְ�
	long m_hTemporyEndConnPlate;	//��ʱ�洢���Ҳ�ֹ����Ķ�ͷ���Ӹְ�
	long m_hMasterTube;	//��X��ƽ�е�ˮƽ����
	long m_hJointTube;	//��ˮƽ���ܶԽӵ��տڸֹ�
public:
	CDualTubeJointView(int id=0);
	virtual bool IsDualTubeJointView(){return true;}
	virtual bool UpdateConnectState(IModel* pTower,CDualTubeJointView* pCopyFromDualJointTubeView=NULL);
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
public:
	READONLY_PROPERTY(BYTE,cbJointState);
	GET(cbJointState){return m_cTemporyJointState;}
	READONLY_PROPERTY(bool,bMasterTubeOnLeft);	//ˮƽ��׼�ֹ����Ƿ����������������
	GET(bMasterTubeOnLeft){return m_bMasterTubeOnLeft;}
	READONLY_PROPERTY(long,hStartJointPlate);
	GET(hStartJointPlate){return m_hTemporyStartConnPlate;}
	READONLY_PROPERTY(long,hEndJointPlate);
	GET(hEndJointPlate){return m_hTemporyEndConnPlate;}
};
class CHorizBeamTubeView : public CDualTubeJointView
{
public:
	static const char HORIZBEAM_FRONT_VIEW		= 'F';	//'F':����ͼ
	static const char HORIZBEAM_VERTICAL_VIEW	= 'V';	//'V':����ͼ
	char m_cSubType;	//
public:
	CHorizBeamTubeView(int id=0);
	virtual char GetViewSubType(){return m_cSubType;}
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
};

class CSlaveSectionView : public CSectionView
{
public:
	int m_idHostView;
	long m_hBaseNode;
	union{
		long m_hRootRod;	//����ͼ���и����׼�˼������򼴽���ͼ����
		long m_hVertRod;	//������ͼ��ͼ����ϵ�µ�����ͼ����λ������
	};
	GEPOINT location;	//������ͼ��ͼ����ϵ�µ�����ͼ����λ������
	GEPOINT direction;	//������ͼ��ͼ����ϵ�µ�����ͼ�����߷���
	double direct_offset;	//����ͼ��ʵ�ʱ�עλ���붨λ��������߷����ƫ�ƾ���
	double symbol_dist;		//����ͼ��(�ɶ�)��ľ���
public:
	CXhCharTempl<XHSTR_LEN2> symbol;	//����ͼ���ϵı�ע����
	virtual const char* get_internalSymbol(){return symbol;}
public:
	CSlaveSectionView(int id=0);
	virtual void ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void InitSlaveSectionView(CSectionView *pHostView,IModel *pModel);
	virtual bool IsHostView() { return false;}
	virtual bool IsSlaveView(){return true;}
	virtual bool IsTubeSectionView(CLDSLineTube** ppSectTube=NULL);
	virtual CLDSGeneralPlate* GetDatumPlateOfFLSectView();
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
};
class CSlaveNodeSectionView : public CSlaveSectionView
{
public:
	CSlaveNodeSectionView(int id=0);
	virtual void ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	virtual int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100){return 0;}	//��������ID�õ�����ֵ�ַ���
};
struct KAOMEANGLE{
	long hAngle;
	CLDSLineAngle* pAngle;
	CLDSGeneralPlate *pPlateInWingX,*pPlateInWingY;
	CTmaPtrList<CLDSBolt,CLDSBolt*> *pBoltsInWingX;
	CTmaPtrList<CLDSBolt,CLDSBolt*> *pBoltsInWingY;
	KAOMEANGLE(){
		hAngle=0;
		pAngle=NULL;
		pPlateInWingX=NULL;
		pPlateInWingY=NULL;
		pBoltsInWingX=NULL;
		pBoltsInWingY=NULL;
	}
	~KAOMEANGLE()
	{
		if(pBoltsInWingX)
			delete pBoltsInWingX;
		if(pBoltsInWingY)
			delete pBoltsInWingY;
	}
};
class CKaoMeAngleSectionView : public CSectionView
{
public:
	long m_hAngle;
	CLDSLineAngle *m_pAngle;
	CTmaPtrList<CLDSBolt,CLDSBolt*> boltSetInWingX;
	CTmaPtrList<CLDSBolt,CLDSBolt*> boltSetInWingY;
public:
	CKaoMeAngleSectionView(int id=0);
	virtual void ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type=0);
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0);
	void InitView(const KAOMEANGLE &kmAngle);
	CLDSBolt* EnumFirstBolt(BOOL bInWingX);
	CLDSBolt* EnumNextBolt(BOOL bInWingX);
};
class CAngleSpreadSectionView : public CSectionView
{
public:
	CAngleSpreadSectionView(int id=0);
};
class CLDSPartGroup : public CLDSObject
{
protected:
	int	 _iPartGroupType;	//�������
public:
	static const short GROUP_FOLDER		=-1;	//��Ϲ����飨Ŀ¼�ļ��У�TYPE_STATIONCONNPLATE
	static const short GROUP_COMMON		= 0;	//��ͨ�麸��
	static const short GROUP_ENDCONN	= 1;	//�ֹ����˲�������(����)�������ӵļ��麸��
	static const short GROUP_ANGLEFOOT	= 2;	//�Ǹ����Ű�
	static const short GROUP_TUBEFOOT	= 3;	//�ֹ����Ű�
	static const short GROUP_SLOPEJOINT = 4;	//���µ��֧���麸
	static const short GROUP_SPHERE		= 5;	//��ڵ�
	static const short GROUP_STDX_PLATES= 6;	//��ϽǸ��б�׼��ʮ�ִ�ֱ���棩����(��)�麸ͼ
	static const short GROUP_BUSHING	= 7;	//�׹ܹҰ��麸ͼ
	static const short GROUP_STATION_BTMTRIBEAM	= 8;//��繹�ܵ��������ײ��տ���
	static const short GROUP_DUALTUBE_JOINT	= 9;	//˫�ֹܶԽ��������Ϊ���Ӹְ��麸��
public:
	UCS_STRU ucs;
	//int	 m_iPartGroupType;	//0.��Ϲ��� 1.��Ϲ����� �����½���Ϲ�����ʱ��ʱʹ�ã����洢
	char sFolderName[MAX_CHAR_GUIGE_16+1];	//������ʾ������ʱ������ʾ��Ĭ���Զ�ͳ��ʱΪ��Ϲ������ڶκ�
	char sRootPartNo[MAX_CHAR_GUIGE_16+1];	//��ʱ�ò��洢
	char sGroupName[MAX_TEMP_CHAR_50+1];	//��Ϲ�������
	int get_iPartGroupType();				//�������
	int set_iPartGroupType(int iValType);	//������ͣ�Ϊ�����ʱ�����麸���ͣ����˱�����Ϊ���� wjh-2019.8.9��
	__declspec(property(put=set_iPartGroupType,get=get_iPartGroupType)) int m_iPartGroupType;	//�������
	long m_hJointTube;		//�ԽӸֹ��麸���жԽӸֹܵľ��
	DWORD m_uStatMatCoef;	//�������еĹ�������Ŀ
	BOOL m_bFootPartGroup;	//���Ű��麸��	wht 10-01-14
	CSuperHashList<CSectionView> SonViews;
	
	CLDSPartGroup();
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToXmlFile(FILE* fp,DWORD schema);
	void FromBuffer(CBuffer &buffer,long version=NULL,long doc_tma1_lma2_tsa3_lds4_default0=0);
	//���Ĭ�ϵ��麸��������ͼ��������麸��������ͼʱΪÿ����ͼ���Ĭ�ϵı궨�㣬������Լ��Ƕȱ�ע��
	BOOL AddDefaultAttachViews();
	int SmartIdentifyGroupType();	//����ʶ���������
	bool IdentifyAngleFoot(ANGLE_FOOT_PARTGROUP *pAngleFoot=NULL,bool upper=true,const double* front_direction=NULL);			//ʶ���Ƿ�Ǹ�����
	bool IdentifyStdXPlates(STDX_PLATES_PARTGROUP *pStdXPlatesPartGroup=NULL);//ʶ���Ƿ�Ϊ��ϽǸ��б�׼ʮ��������Ϻ���
	bool IdentifyBushingPlate();	//ʶ���Ƿ����׹ܵĹ��߰��麸��
	bool IdentifyDualTubeJoint(int *horiBeamOrGeneralDualTube);	//ʶ���Ƿ�Ϊ�������տڸֹ��麸ͼ
	bool IsNeedSidePlateView();//��ͨ�ְ��麸���Ƿ���Ҫ������ͼ
	KEY2C GetIdleSectSymbol();
	KEY2C GetIdleSectSymbol(KEY2C prefer);	//preferΪ�����Ƽ��ļ�ֵ���ͣ���'A1',������Axϵ�еĿ������ӷ���
private:
	//1.��ӶԽ����ֹ���ͼ
	BOOL AddJointTubeViews(CLDSLineTube *pRootTube,PARTSET &partset);		
	//2.�ĸ�֧����������ܵ�K�ڵ����麸��
	BOOL AddKNodeTransectViews(CLDSLineTube *pRootTube,PARTSET &partset);
	//3.��ӻ�׼����Ϊ�˼����麸����ͼ
	BOOL AddCommonRodPartGroupViews(CLDSLinePart *pRootLinePart,PARTSET &partset);
	//3.1 ������ͼ(��ֹ����˾�Ϊ�����Ҹֹ��������Ӱ�����)ʱ�������ͼ
	//3.2 ��Ӹֹ��м�ڵ��Ӧ�Ķ�����ͼ
	BOOL AddLineTubeMiddleNodeEndView(CLDSLineTube *pLineTube,CSectionView *pMainView,PARTSET &partset,int &view_index);
	//3.3 ��Ӹֹܶ˰���ͼ(����ƽ���и��������塢���Ű�)
	BOOL AddLineTubeEndLjView(CLDSLineTube *pLineTube,CSectionView *pMainView,PARTSET &partset,int iStartView,int &view_index);
	//3.4 Ϊ�麸���нŶ�������Ӷ���ͼ
	BOOL AddFootNailView(CLDSLineTube *pLineTube,CSectionView *pMainView,PARTSET &partset,int &view_index);
	//3.5 ��Ӹֹܱ�����ͼ
	BOOL AddTubeBushingPlateView(CLDSLineTube *pLineTube,PARTSET &partset);
	//���ݾ�߲��������ͼ���� wht 18-09-22
	BOOL AdjustFrontViewWorkNormByInsertPlate(CLDSLinePart *pRootRod,PARTSET &partset);
	//4.��ӻ�׼����Ϊ�ְ���麸����ͼ
	BOOL AddPlatePartGroupViews(CLDSPlate *pRootPlate,PARTSET &partset);
	//5.������ӱ�׼ʮ�ֽ��������麸��ͼ
	BOOL AddStdXPlatesViews(CLDSPart* pDatumPart,PARTSET& partset);
	//6.������ӽǸ����Ű��麸��ͼ
	BOOL AddAngleFootViews(CLDSPart* pDatumPart,PARTSET& partset);
	//7.�������Ϊ�Ǹֵ��麸����ͼ
	BOOL AddAnglePartGroupViews(CLDSLineAngle *pLineAngle,PARTSET& partset);
	//8��������Ӻ��׹ܹҰ��麸����ͼ
	BOOL AddBushingPlateView(CLDSPart* pDatumPart,PARTSET& partset);
	//9��������ӱ�繹���������ײ����տ����麸����ͼ
	BOOL AddHorizBeamTubeView(CLDSPart* pDatumPart,PARTSET& partset);
	//10��������ӱ�繹���������ײ����տ����麸����ͼ
	BOOL AddGeneralDualTubeJointView(CLDSPart* pDatumPart,PARTSET& partset);
protected:
	void UpdateDualTubeJointView(CDualTubeJointView *pMainView,CDualTubeJointView **ppMainView2,CLDSLineTube *pDatumTube,CLDSLineTube *pJointTube,PARTSET& partset1,PARTSET& partset2);
public:
	// ���ڴ洢�����������ص�һЩ������������ڹ��Ҳ������������
	// ����ڵ�ʱ�����û�ָ����ת���ܺ͸���֧�� wjh-2013.11.16
	ATOM_LIST<DESIGN_PLATE_ITEM>designItemList;
	//��Ʋ����б���
	void EmptyDesignItems(){designItemList.Empty();}
	//BOOL GetDesignItemValue(WORD wKey, double *pfValue,CHashTable<DESIGN_PLATE_ITEM*>*pTbl=NULL);
	BOOL GetDesignItemValue(WORD wKey, long *piVal,CHashTable<DESIGN_PLATE_ITEM*>*pTbl=NULL);
	char* GetDesignItemNotes(WORD wKey);
	DESIGN_PLATE_ITEM *GetDesignItem(WORD wKey);
	BOOL IsHasDesignItemValue(WORD wKey);
	BOOL DeleteDesignItem(WORD wKey);
	BOOL SetDesignItemValue(WORD wKey,double fValue,char *sNotes=NULL,int iParamType=PARAM_POS,int iDatatype=0,BOOL bModify=FALSE);
	void RetrieveKaoMeAngle(CHashList<KAOMEANGLE> &hashKaoMe);
};
#endif
//struct IPartLibrary{;} ת��IPartLibrary.h�ж���
class CLDSModule;
class CWorkStatus;
class CDisplayView;
struct IConsole : public IModel
{
	virtual CDisplayView *SetActiveView(long hView)=0;
	virtual CDisplayView* GetActiveView()=0;
	virtual BOOL DeleteDisplayView(long hView)=0;
	virtual long ActiveModuleHandle()=0;
	virtual IModel* GetActiveModel()=0;
	virtual CLDSModule* GetActiveModule()=0;
#ifdef __PART_DESIGN_INC_
	virtual CWorkStatus* GetActiveWorkStatus()=0;
#endif
	virtual UCS_STRU WCS()=0;	//��ǰ��������ϵ
	virtual GEPOINT TransPFromScr(const double* point)=0;
	virtual GEPOINT TransVFromScr(const double* vector)=0; 
public:
	virtual void EmptyBlkObjSetBuf(){;}
	virtual bool AddToObjSetBuf(CLDSObject* pObj)=0;
	virtual CLDSNode* EnumBlkBufNodeFirst()=0;
	virtual CLDSNode* EnumBlkBufNodeNext()=0;
	virtual CLDSPart* EnumBlkBufPartFirst(bool bOnlyRod=false)=0;
	virtual CLDSPart* EnumBlkBufPartNext(bool bOnlyRod=false)=0;
public:
	virtual void EmptyDispNode()=0;
	virtual void EmptyDispPart()=0;
	virtual CLDSNode* AddToDispSet(CLDSNode* pNode)=0;
	virtual CLDSPart* AddToDispSet(CLDSPart* pPart)=0;
	virtual NODESET& GetDispNodeSet()=0;
	virtual PARTSET& GetDispPartSet()=0;
	virtual CLDSNode* EnumFirstDispNode()=0;
	virtual CLDSNode* EnumNextDispNode()=0;
	virtual CLDSPart* EnumFirstDispPart()=0;
	virtual CLDSPart* EnumNextDispPart()=0;
	virtual bool EmptyAllDispObjList()=0;	//���������ʾ�����б�
	//ɾ���¼�
	virtual void FireDeleteObject(CLDSObject *pObj,BOOL bDelFromSolidDraw=FALSE)=0;
};

void CfgwordToBuffer(CFGWORD &cfgword,CBuffer &buffer,long version=0,long doc_type=0);
void CfgwordFromBuffer(CFGWORD &cfgword,CBuffer &buffer,long version=0,long doc_type=0);
f3dPoint SpreadFacePntToDatumFace(f3dPoint datum_face_norm,f3dLine huoqu_line,
					f3dPoint spread_face_norm,f3dPoint pick,BOOL bFromMuti=TRUE);
void MirPartHandle(IModel *pTower,long *mir_h,long org_h,MIRMSG msg,COORD3D *pPickPos=NULL);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
typedef CTmaPtrList<CLDSBolt,CLDSBolt*> BOLTSET;
typedef CTmaPtrList<CLDSLineAngle,CLDSLineAngle*> JGSET;
typedef CTmaPtrList<CLDSLineTube,CLDSLineTube*> TUBESET;
typedef CTmaPtrList<CLDSPlate,CLDSPlate*> PLATESET;
#ifdef __GGG_
typedef CTmaPtrList<CLDSConeTube,CLDSConeTube*> CONETUBESET;
#endif
void CreateArcLine(f3dAtomLine *pAtomLine,f3dPoint &workNorm,PROFILE_VERTEX &profileVertex);
BOOL IsPartsCollide(CLDSPart *pMainPart,CLDSPart *pVicePart,int nCollideVertexDist=-1);//,LS_POS_IN_JG_STRU first_ls_pos=NULL);
BOOL IsBerHuoQuJg(CLDSLineAngle *pHuoQuBaseJg, f3dPoint ray_jg_vec,int huoqu_x_wing0_y_wing1);
#endif
typedef CTmaPtrList<CBlockReference,CBlockReference*>BLOCKREFSET;
typedef CTmaPtrList<CLDSFoundation,CLDSFoundation*> FOUNDATIONSET;

#endif
