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
	char ciClassicType;	//0.��ͨ;'B':�Ʊ�;'M':èͷ;'Y':���
	//ģ���������ڵ�;
	struct PROFILE_NODE{
		//0.������;1.ָ��X ;2.ָ��Y;3.ָ��Z;4.��׼��ָ��XZ;5.���߽����;
		//��20:�Ʊ��������۽����(��Ϊ�Ҳ࣬��Ϊ���)
		//��21:�Ʊ�(èͷ)�кᵣ�����߽������½ڵ㣨��Ϊ�ϲ࣬��Ϊ�²ࣩ
		//��22:�Ʊ���������һ��ᵣ������ӽڵ㣨��Ϊ�Ҳ࣬��Ϊ��ࣩ
		//��23:�Ʊ���������һ��ᵣ�ڲ����ӽڵ㣨��Ϊ�Ҳ࣬��Ϊ��ࣩ
		//��24:èͷ�кᵣ��������˽ڵ㣨��Ϊ�Ҳ࣬��Ϊ��ࣩ
		//��25:èͷ�к��봦��������սǴ������ڵ㣨��Ϊ�Ҳ࣬��Ϊ��ࣩ
		//��26:èͷ�к��봦�������ڹսǴ��ڵ㣨��Ϊ�Ҳ࣬��Ϊ��ࣩ
		//��27:èͷ�кᵣ�������������֧���ڹյ㣨��Ϊ�Ҳ࣬��Ϊ��ࣩ
		//  28:800kvֱ�����������Ͻ����,èͷ�����������������ϵĽ����
		char ciPosCalType;					//0.������;1.ָ��X ;2.ָ��Y;3.ָ��Z;4.��׼��ָ��XZ;5.���߽����;
		char ciFractions;					//1.��ʾ��ʾһ����������ҵ㣬N��ʾ1/N�������ҵ�
		WIREPLACE_CODE wirecode;			//�ҵ������Ϣ
		char ciProfileNode;					//������,0.��������;1.������;2.�Ǳ���������
		WORD widNode;
		WORD widMasterRod;					//�ڵ㸸�˼�
		WORD widLinkPrevRod;				//���ӵ�ǰһ�˼�Id;0.��ʾǰ���޸˼�����
		struct PREV_LINK{
			short siMinRayAngle,siMaxRayAngle;	//ǰһ�����Ӹ˼�����С��������߽ǣ���ʱ�뷽�򣩣���
		}prevlink;
		short siMinDegAngle,siMaxDegAngle;	//�ö��㴦ǰ�������߼�нǷ�Χ
		struct RAYCROSS{
			WORD widOrgNode;	//����ԭ��
			WORD widEdgeStart,widEdgeEnd;		//�����߽�����ֹ�ڵ�
			WORD siMinRayAngle,siMaxRayAngle;	//������ʼ(��С)����ֹ(���)�ǣ������ͼ���Ӿ�����ʱ�������X��ļнǣ��൱��X->��Y����
			RAYCROSS(){widOrgNode=widEdgeStart=widEdgeEnd=0;siMinRayAngle=siMaxRayAngle=0;}
		}raynode;
		PROFILE_NODE();
		void SetKey(DWORD key){widNode=(WORD)key;}
	};
	struct PROFILE_ROD{
		WORD widRod,widStartNode,widEndNode;
		bool bHasSectView;	//�Ƿ����չ����
		char ciRodType;		//0.�����ⶨ��;'H':ˮƽ���;'V':��ֱ��
		char _ciAngleLayout;//1~3λ��ʾ�ǸֵĲ��÷�ʽ1,2,3,4;<=0��ʾ��ָ��֫����5~8λ��ʾָ������Ϊ���ƹ�������(ģ������ϵ)
		short siMinRayDegAngle,siMaxRayDegAngle;	//�ø˼���X����С(��)���߼нǷ�Χ����=0����ʾ��������
		struct TIPPOS{
			bool blPresetParams;		//true:Ԥ���˸˼���ͷ����λ�������Ϣ;false:δ�趨�������⴦��
			BYTE cTouchLine;			//0.X֫����;1.Y֫����;2.����
			BYTE cTouchLineType;		//0.����;1.������;2.ƫ��׼��;3.���Ƶ�;4.ָ����׼��
			struct DATUMPOINT{
				char ciCoordType;	//0.ָ��X-Z����;1.ָ��Y-Z����;2.ָ��X-Z����
				WORD widPlane;
			}datumpoint;
			BYTE cFaceOffsetDatumLine;  //ƽ��ģʽ�µ�����ƽ�ƻ�׼��
			GEPOINT face_offset_norm;	//ƽ���淨��
			WORD widBendByRod;	//�����������˼�,0��ʾĬ��Ϊ�˽ڵ㸸�˼�
			BYTE coordCtrlType;	//0:X-Z����;1:Y-Z����;2.X-Y����(����spatialOperationStyle=15ʱ��Ч)
			BYTE cMainCoordCtrlWing;//�Ǹ�����֫��'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
			char szXorCtrlWingOffset[4];//WING_OFFSET_STYLE XOrCtrlWing;	//�Ǹ���Ҫ����֫(�����������������֫)
			char szYorOthrWingOffset[4];//WING_OFFSET_STYLE YOrOtherWing;//�Ǹָ�������֫(����һ�����������֫)
			TIPPOS(){memset(this,0,sizeof(TIPPOS));datumpoint.ciCoordType=1;}
		}desPosStart,desPosEnd;
		//�ǸֵĲ��÷�ʽ
		char get_ciAngleLayout(){return _ciAngleLayout&0x07;}
		char set_ciAngleLayout(char ciType){
			_ciAngleLayout&=0xf8;
			_ciAngleLayout|=ciType&0x07;
			return ciType&0x07;
		}
		__declspec(property(put=set_ciAngleLayout,get=get_ciAngleLayout)) char ciAngleLayout;	//�ǸֵĲ��÷�ʽ1,2,3,4;<=0��ʾ��ָ��֫����
		//ָ�������淨�߽���������
		char get_ciNearNormAxis(){return _ciAngleLayout>>4;}
		char set_ciNearNormAxis(char ciNearAxis){
			_ciAngleLayout&=0x0f;
			_ciAngleLayout|=(ciNearAxis<<4);
			return ciNearAxis&0x0f;
		}
		__declspec(property(put=set_ciNearNormAxis,get=get_ciNearNormAxis)) char ciNearNormAxis;//ָ�������淨�߽���������0.��ָ��(һ��ΪY��);1.X;2.-X;3.Y;4.-Y;5.Z;6.-Z
		WORD widRefRodOfWorkWing;	//����֫���ո˼�
		//-----------------------------------------
		//�˼���������
		char ciSectFlag;	//�������͵�4λ��ʾ�������ͣ���4λ��ʾ��ϽǸ�ʱ��׼�ӽǸ�������
		char get_ciSectStyle(){return ciSectFlag&0x0f;}
		char set_ciSectStyle(char ciStyle){
			ciSectFlag&=0xf0;
			ciSectFlag|=ciStyle&0x0f;
			return ciStyle&0x0f;
		}
		__declspec(property(put=set_ciSectStyle,get=get_ciSectStyle)) char ciSectStyle;	//0.��ʾ����ԭ�����ͣ�1='L';2='T';3='D';4='X'��ʾָ���������
		//��׼�ӽǸֵ�������
		char get_ciMasterIndex(){return ciSectFlag>>4;}
		char set_ciMasterIndex(char _ciMasterIndex){
			ciSectFlag&=0x0f;
			ciSectFlag|=(_ciMasterIndex<<4);
			return _ciMasterIndex&0x0f;
		}
		__declspec(property(put=set_ciMasterIndex,get=get_ciMasterIndex)) char ciMasterIndex;	//��׼�ӽǸֵ�������,ֻ���ڸ˼�Ϊ��ϽǸ�ʱ����Ч

		void SetKey(DWORD key){widRod=(WORD)key;}
		PROFILE_ROD();
	};
	struct DYNA_LINKROD{	//��̬���Ӹ˼���δ��ģ���н��й̶���ŵķǱ߽�˼���
		WORD widRod;	//�˼���ʶ
		WORD widStartNode,widEndNode;	//��ʼ�ڵ㣬��ֹ�ڵ�
		char ciNormalDirection;			//���ƹ������ߣ�����֫������ӽ��������ᣩX��Y��Z������Сд��ʾ���᷽��
		char ciAngleLayout;				//�ǸֵĲ��÷�ʽ1,2,3,4;<=0��ʾ��ָ��֫����
		WORD widRefRodOfWorkWing;		//����֫���ո˼�
		char ciSectStyle;	//0.��ʾ����ԭ�����ͣ�'T','D','X'��ʾָ���������
		char ciMasterIndex;	//��׼�ӽǸֵ�������,ֻ���ڸ˼�Ϊ��ϽǸ�ʱ����Ч
		void SetKey(DWORD key){widRod=(WORD)key;}
		DYNA_LINKROD();
	};
	class WIREANGLE{
	public:
		WORD widAngle;
		WORD widBaseRod;		//�ְ�Ķ�λ��׼�Ǹ�
		WORD widBaseNode;		//�ְ嶨λ�ڵ�
		char ciLayer;	//����λ�ò㼶��0.��ʾ�붨λ�Ǹֽ�����Խ���ʾԽ���
		char ciDatumAngleWing;	//�ְ�Ķ�λ��׼�ǸֵĹ���֫��'X','Y'��
		char ciNormalOffset;	//0.��ʾ�Զ��ж�
		char ciAngleLayout;	//�ǸֵĲ��÷�ʽ1,2,3,4;<=0��ʾ��ָ��֫�������������֫���߿�
		char ciLenDirection;//�̽Ǹ�ʼ->�ս��Ʒ�����ӽ��������ᣩX��Y��Z������Сд��ʾ���᷽��
		WORD width,width2,thick;	//����֫����һ֫��֫��
		struct HOLE{
			char ciResideWing;
			BYTE ciHoleDiameter;
			char ciLocationMode;	//0.�̽Ǹ��е�ʼ->�շ���ƫ��;'S':��ʼ���濪ʼ���ն�ƫ��;'E'���ն��濪ʼ��ʼ��ƫ��
			short siLenOffset;
		};
		PRESET_ARRAY4<HOLE> holes;	//���߿�
		void SetKey(DWORD key){widAngle=(WORD)key;}
		WIREANGLE();
		void CloneFrom(WIREANGLE* pFrom);
	};
	struct CONNECT_ROD{
		short widRod;	//�߽�˼�����ͨ���Ӹ˼�
		struct DYNROD{
			short widStartNode,widEndNode;	//��̬�˽ڵ㶨��˼�
			short siRayMinDegAngle,siRayMaxDegAngle;	//widEndNode=0ʱ��ʾָ����Χ�������߸˼�
		}dynaRod;
		char ciConnectWing;	//'X' or 'Y',��д��ʾʼ������Сд��ʾ�ն�����,0��ʾ�Զ��ж�����֫
		char ciPlaneNo;		//���:1,2,3
		bool dualSideEdge;	//�Ƿ�ͬʱӰ��ʼĩ����ְ�߽�
		BYTE cnMinBoltN;	//����������˨��
		BYTE cnMaxSingularLineBolts;	//�����������ʱ�����Ų��������˨��
		WORD widCrossBoltAngle;	//0:��ʾ����Ҫ������;<0xffff��ʾͨ����˨��ָ���Ǹ�ֱ��;0xffff��ʾ�붨λ��׼�Ǹ�ֱ��
		CONNECT_ROD();
	};
	struct CONNPOINT_SCHEMA{	//���ӵ�ģʽ
		CXhChar50 name;
		struct SCHEMA_ROD{
			WORD widRod;		//1 based index;
			char ciRodType;		//'A':Across���˼���Խ��׼�ڵ�;'R':Radiation�˼��ӻ�׼�ڵ����߷���
			char szRodSectType[4];	//�˼���������,'L','T','D','X'
			char ciNecessity;	//��Ҫ�ԣ�0.������涨;1.��Ҫ;2.���п���
			//���ڿ�Խ�Ǹָ˼��ж�֫������Ҫʼ->�շ��򣬴�ʱ��ͼ��X���������������Ϊ׼
			char ciAngleLayout;	//�ǸֵĲ��÷�ʽ1,2,3,4;<=0��ʾ��ָ��֫�������������֫���߿�
			short siMinRayAngle,siMaxRayAngle;	//������ʼ(��С)����ֹ(���)�ǣ������ͼ���Ӿ�����ʱ�������X��ļнǣ��൱��X->��Y����
			struct ANGLE_LOCATION{
				char ciMasterIndex;	//ciRodSetTypeΪ��ϽǸ�ʱ����ֵ��ʾ��׼�ӽǸֵ������ţ�0 base index
				char ciLocationType;	//0.��֫�������;1.����ƽ�����;2.�������ߣ��������ߣ�;3.����ƫ��׼��
				WORD widBendlineAngle;	//���������ݽǸ�
				ANGLE_LOCATION(){widBendlineAngle=ciMasterIndex=ciLocationType=0;}
			}angle;
			SCHEMA_ROD(){
				szRodSectType[0]='L';
				szRodSectType[1]=szRodSectType[2]=szRodSectType[3]=0;
				ciRodType='R';ciNecessity=ciAngleLayout=0;widRod=siMinRayAngle=siMaxRayAngle=0;
			}
		};
	public:
		WORD widParentRod;	//>0��Ҫָ���ڵ㸸�˼�
		WORD widBaseRod;	//���Ӱ�Ķ�λ�����˼�
		bool m_bStrictReview;	//����Ӹ˼����ͼ������ϸ��������ӵ�����
		SCHEMA_ROD rodarr[8];	//Ԥ��8���˼���������һ��widRod=0�˼����� 
		CONNPOINT_SCHEMA(){widParentRod=widBaseRod=0;}
	};
	struct CONNPOINT{	//���ӵ�ģʽ
		CONNPOINT_SCHEMA schema;
		struct MATCHROD{
			char ciRodType;			//'A':Across���˼���Խ��׼�ڵ�;'R':Radiation�˼��ӻ�׼�ڵ����߷���
			char ciRodSectType;		//�˼���������,'L','T','D','X'��'O'(��ʾ�ֹ�)
			short siActualRayAngle;	//��Խ�˼�ȡֵΪ270<=alfa<360,0<=alfa<90,����ȡֵ0<=alfa<360
			IModelRod* pModelRod;
			MATCHROD(){ciRodType='R';ciRodSectType='L';siActualRayAngle=0;pModelRod=NULL;}
		}rodarr[8];
		WORD matchrod_indexes[8];	//
	};
	class PLATE{
	public:
		WORD widBaseRod;		//�ְ�Ķ�λ��׼�Ǹ�
		WORD widBaseNode;		//�ְ嶨λ�ڵ�
		char ciDatumAngleWing;	//�ְ�Ķ�λ��׼�ǸֵĹ���֫��'X','Y'��
		WORD widBendEdgeAngle1;	//��һ��������������߶�λ��׼�Ǹ֣���ְ巨���ڲ���Ϊ���������Ϊ������
		char ciBendEdgeWing1;	//'X','x','Y','y':��д��ʾ�����߲࣬Сд��֫���
		double fBendDegAngle1;	//����ǣ��������߻�׼�Ǹ�ʼ->��Ϊ����ʱ����תΪ��(�Ƕ��ƣ�0��ʾ�Զ�����)
		WORD widBendEdgeAngle2;	//��һ��������������߶�λ��׼�Ǹ֣���ְ巨���ڲ���Ϊ���������Ϊ������
		char ciBendEdgeWing2;	//'X','x','Y','y':��д��ʾ�����߲࣬Сд��֫���
		double fBendDegAngle2;	//����ǣ��������߻�׼�Ǹ�ʼ->��Ϊ����ʱ����תΪ��(�Ƕ��ƣ�0��ʾ�Զ�����)
		PRESET_ARRAY8<CONNECT_ROD> connrods;//�ְ����Ӹ˼�
	public:
		struct HOLE{
			WORD wiDistance;	//�����׼�ǸֵĴ�ֱ����
			short siUpperOffset;//�ְ�ֲ�Y��������ƫ�ƾ���
			BYTE ciHoleDiameter;//�׾�
			BYTE ciBushTubeDiameter;//�׹��⾶
			BYTE ciBushTubeThick;	//�׹ܱں�
			BYTE ciBushTubeHeight;	//�׹ܸ߶�
		};
		PRESET_ARRAY4<HOLE> holes;	//���߿�
		char ciCurveEdgeMode;	//0.��͹ʽ;1.����ʽ
		char ciEdgeRadius;	//�ҿױ߾��Բ���뾶
		char ciFilletRadius;//�ҿױ��ڰ��Ǵ�������Բ���뾶
	public:
		PLATE();
		void CloneFrom(PLATE* pFrom);
	};
	struct ANGLEPLATE{
		char ciRibType;	//�Ǹּ�ǿ�߰�����
		char ciLocationMode;	//0.�̽Ǹ��е�ʼ->�շ���ƫ��;'S':��ʼ���濪ʼ���ն�ƫ��;'E'���ն��濪ʼ��ʼ��ƫ��
		short siLenOffset;
		char thick;
		ANGLEPLATE();
	};
	//�����ı�����
	struct PROFILE_QUADSECT{
		short arrRods[2];	//ȡ��ֵʱ��ʾ���������ı���ʱ�������ոø˼�������������֫����
	};
	//ģ���а����Ļ�׼ƽ��
	struct DATUM_PLANE{
		WORD widPlane;
		struct DATUM_POINT{
			char ciType;	//0.�ڵ�;'S'�˼�ʼ�˵�;'E'�˼��ն˵�
			WORD widNodeOrRod;
		}points[3];
		void SetKey(DWORD key){widPlane=(WORD)key;}
	};
	struct MATCHNODE{
	public:
		WORD  widNode;
		short siActualDegAngle;	//��ǰ�������㴦�������������߼���ڼн�
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
		GEPOINT lenImgStdVec;	//ͼ������ϵ�¸˼��߽����췽��
		GEPOINT lenModelStdVec;	//ģ������ϵ�¸˼��߽����췽��
		PRESET_ARRAY<EDGEROD,PRESET_OBJS4> relaRods;
		void SetKey(DWORD key){widRod=(WORD)key;}
	};
	DWORD idTempl;
	void SetKey(DWORD key){idTempl=key;}
public:
	char name[51];
	char ciOnRightSide;	//'R':�ᵣ��X�������Ҳ�; 'L'�ᵣ��X�Ḻ�����;'U'����ͷ���¶��ϲ�
	bool bMaybeHeadZBorZM;	//�Ƿ����Ϊèͷ��Ʊ�����ҪΪ�����־Ʊ���èͷ�����������и���֮����
	CHashListEx<PROFILE_ROD>  hashParaRods;
	CHashListEx<DYNA_LINKROD> hashDynaLinkRods;
	CHashListEx<WIREANGLE>	  hashWireAngles;
	CHashListEx<PROFILE_NODE> hashParaNodes;
	CHashListEx<DATUM_PLANE> hashParaPlanes;
	CXhSimpleList<PROFILE_QUADSECT>listQuadSects;
	CXhSimpleList<PLATE> listParaPlates;
	CXhSimpleList<ANGLEPLATE> listAnglePlates;
	//CMPL_EXPRESSION condition;	//Լ������
public:
	//ʶ��ģ���кᵣ��ͷʱ��ʵ��ƥ����Ϣ
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