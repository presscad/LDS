#ifndef __LOFTING_DATA_H_
#define __LOFTING_DATA_H_

#include "XhCharString.h"
#include "Buffer.h"
#include "HashTable.h"
#include "SegI.h"
#include "Tower.h"

enum MARK_TYPE
{
	MARK_CMAT		= 0x00000001,	//����
	MARK_SIZE		= 0x00000002,	//���
	MARK_LENGTH		= 0x00000004,	//����
	MARK_COUNT		= 0x00000008,	//������
	MARK_M16BOLT	= 0x00000040,	//M16��˨��
	MARK_M20BOLT	= 0x00000080,	//M20��˨��
	MARK_M24BOLT	= 0x00000100,	//M24��˨��
	MARK_ELSEBOLT	= 0x00000200,	//���������������˨��
	MARK_HOLE		= 0x00000400,	//���⹦�ܿ���
	MARK_LINEANGLE	= 0x0000003F,	//�Ǹֻ�����Ϣ
	MARK_PLATE		= 0X0000000F,	//�ְ������Ϣ
	MARK_ALL		= 0xFFFFFFFF,
};
struct ANGLEOFFSET{
	//DECLARE_PROPERTY(BYTE,ciOffsetWing);//cbWingOffsetMode��ƫ��֫,0.X֫;1.Y֫;2.���
	//DECLARE_PROPERTY(BYTE,ciStyleOfG);	//cbWingOffsetMode��ƫ��׼������,0.g;1.g1;2.g2;3.g3;4.ָ��ciWingOffset
	BYTE ciOffsetWing;	//ƫ��֫,0.X֫;1.Y֫;2.���
	BYTE ciStyleOfG;	//ƫ��׼������,0.g;1.g1;2.g2;3.g3;4.ָ��ciWingOffset
	BYTE ciOffsetPlane;	//0.����ͶӰ;1.X-Yƽ��;2.Y-Zƽ��;3.X-Zƽ��
	short wiWingOffset;
	short wiLenOffset;	//�Ǹֹؼ�����ʼ->�շ���ƫ����
};
class CDataCheckModel{
public:
	static const int SIZE_SECTION_Z				= 0;	//�ڼ䴹��
	static const int SIZE_SEG_SPACE_Z			= 1;	//�ֶμ��
	static const int SIZE_GUAXIAN_SPACE_Z		= 2;	//�ᵣ���ߵ�䴹ֱ���
	static const int SIZE_TOWER_HEIGHT			= 3;	//����
	static const int SIZE_SHANK_SPACE_XY		= 4;	//�����¶ο��ڳߴ�
	static const int SIZE_GUAXIAN_SPACE_XY		= 5;	//��ͷ(�����ߵ�)ˮƽ���
	//
	static const int COORD_ONE_NODE_SIZE		= 0;	//�ڵ�����
	static const int COORD_TWO_NODE_SIZE		= 1;	//�ڵ��ߴ�
	static const int COORD_GEN_KAI_SIZE			= 2;	//�����ߴ�
	struct KEYPOINT{
		BYTE ciPointType;	//0.�Ǹֹؼ���;1.�����ֲ������;2.��λ��
		union{
			struct ANGLEPOINT{
				long hAngle,hNode;
				ANGLEOFFSET offset;
			}angle;
			struct LOCALPOINT{
				long hPart;
				float x,y,z;	//hPart�ϵľֲ�����
			}coord;
			struct PLATEHOLE{
				long hDatumPart,hBolt;
			}hole;
		};
	};
	class SIZE_DATA	//���꼰���Ƴߴ�У����
	{
		DWORD m_id;
		friend class CDataCheckModel;
		BYTE _ciSizeType;		//���ͺ�0.�ؼ�������;1.�ؼ����ߴ�;2.��������;3.��������;4.�׾�;5.����׾�(����,����);6.�ؼ��㵽������С����;7.������������С����
	public:
		SEGI iSeg;
		char layer4[4];
		//StrKey�������:<���ͺ�:�����-�յ���>��<���ͺ�:���>
		CXhChar50 m_sKey;
		DECLARE_READONLY_PROPERTY(DWORD,Id){return m_id;}
		DECLARE_READONLY_PROPERTY(CXhChar50,idStr);
		SIZE_DATA(DWORD id=0);
		void SetKey(DWORD key){m_id=key;}
	public:
		DECLARE_PROPERTY(BYTE,ciSizeType);		//���ͺ�0.�ؼ�������;1.�ؼ����ߴ�;2.��������;3.��������;4.�׾�;5.����׾�(����,����);6.�ؼ��㵽������С����;7.������������С����
		DECLARE_PROPERTY(BYTE,cbSizeDimFlag);	//0.����; 0x01:X����; 0x02:Y����; 0x04:Z����
		BYTE ciClassifyType;	//�ڵ��ߴ����ͣ�0.�ڼ䴹�� 1.�ֶμ�� 2.���ߵ㴹�� 3.���� 4.���ڳߴ� 5.��ͷˮƽ���
		BOOL m_bCheck;			//��¼��У���������distΪ�˹�ȷ�Ϻ����ȷ�ߴ�����
		union DISTANCE{
			float distance;	//ֱ�߾���
			struct COORDS{
				float distance;	//ֱ�߾���
				float distInX;	//X����ͶӰ����
				float distInY;	//Y����ͶӰ����
				float distInZ;	//Z����ͶӰ����
			}coords;
			struct HOLEDIST{	//�׾�
				float distance;	//ֱ�߾���
				float distInLength;		//����׾�
				float distInTransverse;	//����׾�
				long hDatumRod;			//������ջ�׼�˼�
			}holes;
		}dist;
		KEYPOINT from,to;
		float tolerance;	//��ʾ���
	public:	//�ַ�����ʶ�����
		static CXhChar50 CodingIdStr(BYTE ciSizeType,const KEYPOINT& from,const KEYPOINT& to);
	public:	//�ߴ��ע��ʾ����
		struct DIMENSION{
			BYTE ciDimensionType;	//0.�����ע(��ע�ߴ�ֱ�ڻ���);1.ת�Ǳ�ע(��ע��Ϊ������������ͶӰ);2.�����ͱ�ע
			short siUpOffset;	//�ı���ע��ӱ�ע���ߵ����ƾ���
			DIMENSION(){siUpOffset=0;ciDimensionType=0;}
		}dimension;	//��ע��Ϣ
		GEPOINT posFrom,posTo;	//��ʱ���������洢
	};
	typedef SIZE_DATA* SIZE_DATAPTR;

	struct PART_DATA		//����У����
	{
		static const BYTE TYPE_ANGLE = 1;
		static const BYTE TYPE_PLATE = 3;
		//����У��ʱ���������ʶλ��m_dwFlag��Ӧ��������
		static const UINT PROP_MATERIAL = 0x0001;	//����
		static const UINT PROP_SIZE		= 0x0002;	//���
		static const UINT PROP_LENGTH	= 0x0004;	//����
		static const UINT PROP_COUNT	= 0x0008;	//������
		static const UINT PROP_M16_NX	= 0x0010;	//M16��˨�����ڽǸ�X֫�ϣ�
		static const UINT PROP_M16_NY	= 0x0020;	//M16��˨�����ڽǸ�Y֫�ϣ�
		static const UINT PROP_M20_NX	= 0x0040;	//M20��˨�����ڽǸ�X֫�ϣ�
		static const UINT PROP_M20_NY	= 0x0080;	//M20��˨�����ڽǸ�Y֫�ϣ�
		static const UINT PROP_M24_NX	= 0x0100;	//M24��˨�����ڽǸ�X֫�ϣ�
		static const UINT PROP_M24_NY	= 0x0200;	//M24��˨�����ڽǸ�Y֫�ϣ�
		static const UINT PROP_MX_NX	= 0x0400;	//Mxx��˨�����ڽǸ�X֫�ϣ�
		static const UINT PROP_MX_NY	= 0x0800;	//Mxx��˨�����ڽǸ�Y֫�ϣ�
		static const UINT PROP_MS_N		= 0x1000;	//�������
		union BOLT_COUNT
		{
			long count;
			struct{WORD countOfWingX,countOfWingY;};
			BOLT_COUNT(long nBoltCount=0){count=nBoltCount;}
			operator long(){return count;}
			DECLARE_PROPERTY(BYTE,nNailCount);		//�Ŷ���
			DECLARE_PROPERTY(BYTE,nWireCount);		//���߿�
			DECLARE_PROPERTY(BYTE,nEarthCount);		//�ӵؿ�
			DECLARE_PROPERTY(BYTE,nAssemCount);		//װ���
			DECLARE_PROPERTY(BYTE,nSignMountCount);	//���ƿ�
		};
	public:
		DWORD m_dwFlag;	 //
		CXhChar16 m_sPartNo;
		char cMaterial;
		double m_fThick;
		double m_fWidth;
		double m_fLength;
		WORD nCount;	//��������
		BOLT_COUNT nM16Bolt;	//M16������˨����
		BOLT_COUNT nM20Bolt;	//M20������˨����
		BOLT_COUNT nM24Bolt;	//M24������˨����
		BOLT_COUNT nElseBolt;	//����ǳ���׾�������˨����
		BOLT_COUNT nSpecialHole;//����ף��Ŷ��ס����߿ס��ӵؿס�װ��ס����ƿף�
		BYTE ciPartType;	//1.�Ǹ� 3.�ְ� 4.�ֹ� 5.���� 6.�۸�
		SEGI iSeg;	//�κ�
		//
		PART_DATA(){memset(this,0,sizeof(PART_DATA));}
	};
	struct PART_CHECK_ITEM
	{
		CXhChar16 m_sKey;
		DWORD m_dwFlag;			//
		PART_DATA drawing;	//ͼֽ��������
		PART_DATA model;	//������������
		CXhChar50 sNotes;		//У��ע��Ϣ
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
	//���Ƴߴ����
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
	//�������ݲ���
	void ExtractPartCheckItems();
	int GetPartCheckNum(BOOL bConfirm=FALSE);
	PART_CHECK_ITEM* AddPartCheckItem(const char* sPartNo);
	PART_CHECK_ITEM* EnumFirPartItem(){return m_xHashPartCheck.GetFirst();}
	PART_CHECK_ITEM* EnumNextPartItem(){return m_xHashPartCheck.GetNext();}
};
extern CDataCheckModel xDataCheckModel;
#endif