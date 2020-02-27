#ifndef __LOFTING_DATA_H_
#define __LOFTING_DATA_H_

#include "XhCharString.h"
#include "Buffer.h"
#include "HashTable.h"
#include "SegI.h"
#include "Tower.h"

enum MARK_TYPE
{
	MARK_CMAT		= 0x00000001,	//材质
	MARK_SIZE		= 0x00000002,	//规格
	MARK_LENGTH		= 0x00000004,	//长度
	MARK_COUNT		= 0x00000008,	//单基数
	MARK_M16BOLT	= 0x00000040,	//M16螺栓数
	MARK_M20BOLT	= 0x00000080,	//M20螺栓数
	MARK_M24BOLT	= 0x00000100,	//M24螺栓数
	MARK_ELSEBOLT	= 0x00000200,	//其余非正规连接螺栓数
	MARK_HOLE		= 0x00000400,	//特殊功能孔数
	MARK_LINEANGLE	= 0x0000003F,	//角钢基本信息
	MARK_PLATE		= 0X0000000F,	//钢板基本信息
	MARK_ALL		= 0xFFFFFFFF,
};
struct ANGLEOFFSET{
	//DECLARE_PROPERTY(BYTE,ciOffsetWing);//cbWingOffsetMode中偏移肢,0.X肢;1.Y肢;2.楞点
	//DECLARE_PROPERTY(BYTE,ciStyleOfG);	//cbWingOffsetMode中偏移准距类型,0.g;1.g1;2.g2;3.g3;4.指定ciWingOffset
	BYTE ciOffsetWing;	//偏移肢,0.X肢;1.Y肢;2.楞点
	BYTE ciStyleOfG;	//偏移准距类型,0.g;1.g1;2.g2;3.g3;4.指定ciWingOffset
	BYTE ciOffsetPlane;	//0.垂向投影;1.X-Y平面;2.Y-Z平面;3.X-Z平面
	short wiWingOffset;
	short wiLenOffset;	//角钢关键点沿始->终方向偏移量
};
class CDataCheckModel{
public:
	static const int SIZE_SECTION_Z				= 0;	//节间垂距
	static const int SIZE_SEG_SPACE_Z			= 1;	//分段间距
	static const int SIZE_GUAXIAN_SPACE_Z		= 2;	//横担挂线点间垂直间距
	static const int SIZE_TOWER_HEIGHT			= 3;	//塔高
	static const int SIZE_SHANK_SPACE_XY		= 4;	//塔身坡段开口尺寸
	static const int SIZE_GUAXIAN_SPACE_XY		= 5;	//塔头(含挂线点)水平间距
	//
	static const int COORD_ONE_NODE_SIZE		= 0;	//节点坐标
	static const int COORD_TWO_NODE_SIZE		= 1;	//节点间尺寸
	static const int COORD_GEN_KAI_SIZE			= 2;	//根开尺寸
	struct KEYPOINT{
		BYTE ciPointType;	//0.角钢关键点;1.构件局部坐标点;2.孔位点
		union{
			struct ANGLEPOINT{
				long hAngle,hNode;
				ANGLEOFFSET offset;
			}angle;
			struct LOCALPOINT{
				long hPart;
				float x,y,z;	//hPart上的局部坐标
			}coord;
			struct PLATEHOLE{
				long hDatumPart,hBolt;
			}hole;
		};
	};
	class SIZE_DATA	//坐标及控制尺寸校审项
	{
		DWORD m_id;
		friend class CDataCheckModel;
		BYTE _ciSizeType;		//类型号0.关键点坐标;1.关键点间尺寸;2.铁塔根开;3.基础根开;4.孔距;5.分项孔距(横向,纵向);6.关键点到构件最小距离;7.构件到构件最小距离
	public:
		SEGI iSeg;
		char layer4[4];
		//StrKey编码规则:<类型号:起点句柄-终点句柄>或<类型号:句柄>
		CXhChar50 m_sKey;
		DECLARE_READONLY_PROPERTY(DWORD,Id){return m_id;}
		DECLARE_READONLY_PROPERTY(CXhChar50,idStr);
		SIZE_DATA(DWORD id=0);
		void SetKey(DWORD key){m_id=key;}
	public:
		DECLARE_PROPERTY(BYTE,ciSizeType);		//类型号0.关键点坐标;1.关键点间尺寸;2.铁塔根开;3.基础根开;4.孔距;5.分项孔距(横向,纵向);6.关键点到构件最小距离;7.构件到构件最小距离
		DECLARE_PROPERTY(BYTE,cbSizeDimFlag);	//0.长度; 0x01:X坐标; 0x02:Y坐标; 0x04:Z坐标
		BYTE ciClassifyType;	//节点间尺寸类型：0.节间垂距 1.分段间距 2.挂线点垂距 3.塔高 4.开口尺寸 5.塔头水平间距
		BOOL m_bCheck;			//已录入校正数据项，即dist为人工确认后的正确尺寸数据
		union DISTANCE{
			float distance;	//直线距离
			struct COORDS{
				float distance;	//直线距离
				float distInX;	//X轴向投影距离
				float distInY;	//Y轴向投影距离
				float distInZ;	//Z轴向投影距离
			}coords;
			struct HOLEDIST{	//孔距
				float distance;	//直线距离
				float distInLength;		//纵向孔距
				float distInTransverse;	//横向孔距
				long hDatumRod;			//纵向参照基准杆件
			}holes;
		}dist;
		KEYPOINT from,to;
		float tolerance;	//警示误差
	public:	//字符串标识码编码
		static CXhChar50 CodingIdStr(BYTE ciSizeType,const KEYPOINT& from,const KEYPOINT& to);
	public:	//尺寸标注显示数据
		struct DIMENSION{
			BYTE ciDimensionType;	//0.对齐标注(标注线垂直于基线);1.转角标注(标注线为基线向坐标轴投影);2.射线型标注
			short siUpOffset;	//文本标注点从标注基线的上移距离
			DIMENSION(){siUpOffset=0;ciDimensionType=0;}
		}dimension;	//标注信息
		GEPOINT posFrom,posTo;	//临时变量，不存储
	};
	typedef SIZE_DATA* SIZE_DATAPTR;

	struct PART_DATA		//构件校审项
	{
		static const BYTE TYPE_ANGLE = 1;
		static const BYTE TYPE_PLATE = 3;
		//构件校审时的属性项标识位，m_dwFlag对应的属性项
		static const UINT PROP_MATERIAL = 0x0001;	//材质
		static const UINT PROP_SIZE		= 0x0002;	//规格
		static const UINT PROP_LENGTH	= 0x0004;	//长度
		static const UINT PROP_COUNT	= 0x0008;	//单基数
		static const UINT PROP_M16_NX	= 0x0010;	//M16螺栓数（在角钢X肢上）
		static const UINT PROP_M16_NY	= 0x0020;	//M16螺栓数（在角钢Y肢上）
		static const UINT PROP_M20_NX	= 0x0040;	//M20螺栓数（在角钢X肢上）
		static const UINT PROP_M20_NY	= 0x0080;	//M20螺栓数（在角钢Y肢上）
		static const UINT PROP_M24_NX	= 0x0100;	//M24螺栓数（在角钢X肢上）
		static const UINT PROP_M24_NY	= 0x0200;	//M24螺栓数（在角钢Y肢上）
		static const UINT PROP_MX_NX	= 0x0400;	//Mxx螺栓数（在角钢X肢上）
		static const UINT PROP_MX_NY	= 0x0800;	//Mxx螺栓数（在角钢Y肢上）
		static const UINT PROP_MS_N		= 0x1000;	//特殊孔数
		union BOLT_COUNT
		{
			long count;
			struct{WORD countOfWingX,countOfWingY;};
			BOLT_COUNT(long nBoltCount=0){count=nBoltCount;}
			operator long(){return count;}
			DECLARE_PROPERTY(BYTE,nNailCount);		//脚钉孔
			DECLARE_PROPERTY(BYTE,nWireCount);		//挂线孔
			DECLARE_PROPERTY(BYTE,nEarthCount);		//接地孔
			DECLARE_PROPERTY(BYTE,nAssemCount);		//装配孔
			DECLARE_PROPERTY(BYTE,nSignMountCount);	//挂牌孔
		};
	public:
		DWORD m_dwFlag;	 //
		CXhChar16 m_sPartNo;
		char cMaterial;
		double m_fThick;
		double m_fWidth;
		double m_fLength;
		WORD nCount;	//单基数量
		BOLT_COUNT nM16Bolt;	//M16连接螺栓数量
		BOLT_COUNT nM20Bolt;	//M20连接螺栓数量
		BOLT_COUNT nM24Bolt;	//M24连接螺栓数量
		BOLT_COUNT nElseBolt;	//其余非常规孔径连接螺栓数量
		BOLT_COUNT nSpecialHole;//特殊孔（脚钉孔、挂线孔、接地孔、装配孔、挂牌孔）
		BYTE ciPartType;	//1.角钢 3.钢板 4.钢管 5.扁铁 6.槽钢
		SEGI iSeg;	//段号
		//
		PART_DATA(){memset(this,0,sizeof(PART_DATA));}
	};
	struct PART_CHECK_ITEM
	{
		CXhChar16 m_sKey;
		DWORD m_dwFlag;			//
		PART_DATA drawing;	//图纸构件数据
		PART_DATA model;	//放样构件数据
		CXhChar50 sNotes;		//校审备注信息
		//
		DECLARE_READONLY_PROPERTY(CXhChar16,StateMessage);
		void CleanFlag(){m_dwFlag=0;}
		void RefreshLoftPartInfo();
		void SetDrawingDefaultData();
		CXhChar16 GetMatStr(BOOL bLoft=TRUE);
		CXhChar16 GetGuige(BOOL bLoft=TRUE);
		PART_CHECK_ITEM(){CleanFlag();}
	};
private:
	NODESET nodeSet;
	PARTSET partSet;
	CHashStrList<SIZE_DATAPTR> hashKeyStrSizeItems;
	CHashListEx<SIZE_DATA> hashSizeItems;
	CHashStrList<PART_CHECK_ITEM> m_xHashPartCheck;
public:
	CDataCheckModel();
	~CDataCheckModel();
	void FromBuffer(CBuffer& attach_buffer);
	void ToBuffer(CBuffer& attach_buffer);
	//控制尺寸操作
	void GetOriginalObjectSet();
	void ChooseFeatPt(NODESET &selNodeSet,int iType);
	void ExtractCoordCheckItems();
	SIZE_DATA* GetCoordSizeItem(const char* idStr){
		SIZE_DATAPTR* ppItem=hashKeyStrSizeItems.GetValue(idStr);
		return ppItem!=NULL?*ppItem:NULL;
	}
	SIZE_DATA* AppendCoordSizeItem(const char* idStr=NULL);
	SIZE_DATA* EnumFirstCoordSizeItem(){return hashSizeItems.GetFirst();}
	SIZE_DATA* EnumNextCoordSizeItem(){return hashSizeItems.GetNext();}
	//构件数据操作
	void ExtractPartCheckItems();
	int GetPartCheckNum(BOOL bConfirm=FALSE);
	PART_CHECK_ITEM* AddPartCheckItem(const char* sPartNo);
	PART_CHECK_ITEM* EnumFirPartItem(){return m_xHashPartCheck.GetFirst();}
	PART_CHECK_ITEM* EnumNextPartItem(){return m_xHashPartCheck.GetNext();}
};
extern CDataCheckModel xDataCheckModel;
#endif