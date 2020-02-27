#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
#include "Markup.h"
#include "env_def.h"
#include "ComplexId.h"

struct FOOTTEMPL_FLAG{
	BYTE cbFlag;
public:
	FOOTTEMPL_FLAG(BYTE flag=0){cbFlag=flag;}
	operator BYTE(){return cbFlag;}
	void SetFromTemplFile(){cbFlag|=0x01;}
	void SetFromModel(){cbFlag|=0x02;}
	void ClearFromModel(){cbFlag%=2;}
	bool IsFromTemplFile(){return (cbFlag&0x01)!=0;}//��Դ��ģ���ļ�
	bool IsFromModel(){return (cbFlag&0x02)!=0;}	//��Դ��ʵ��ģ�ͽṹ��ȡ
};
class CFootTempl{
	struct XmlNode{
		static const int TYPE_BASICPARAM= 0;
		static const int TYPE_BASEPLATE = 1;
		static const int TYPE_SHOEPLATE = 2;
	public:
		char* sElemName;
		int m_iObjType;
		void* pRelaObj;
		XmlNode(char* elemName,int type=0,void* pObj=NULL){
			sElemName=elemName;
			m_iObjType=type;
			pRelaObj=pObj;
		}
	};
public:
	struct LINKANGLE{
		FOOTTEMPL_FLAG cStateFlag;	//������Ϣ��Դ�ڵ���ģ���ļ�
		KEY4C label;	//��ʶ��ֵ
		struct BOLTINFO{
			BYTE d;
			BYTE num;		//��˨����
			BYTE rows;		//0.���� 1.˫��
			BYTE layout;	//0.��֫˫�����߲࿿ǰ(˫֫���߷�������ҵ�);1.��֫˫�����߲࿿��(˫֫���߷�������Ҹ�)
			short siEndSpace,siBoltSpace;
			BOLTINFO(){
				d=20;
				num=rows=1;
				layout=0;
				siEndSpace=30;
				siBoltSpace=60;
			}
		}bolt;
		CLDSLineAngle* pAngle;
		double angleWidth;	//��Ҫ���ڵ������ģ��ʱ���Ǹ�֫���Ƿ����仯���緢���仯�ľ�Ҳ������ wjh-2016.8.4
		double oddment;	//�Ǹ����Ӷ�����ͷ
		short g,g1,g2;	//��ǰ�Ǹֵĵ����ľ༰˫�ŵ�1��2���ľ�
		void* pLinkPlate;
		LINKANGLE(){pAngle=NULL;pLinkPlate=NULL;g=g1=g2=0;angleWidth=oddment=0;}
		void SetKey(DWORD key){label=key;}
		void UpdateBoltInfo(CLDSNode* pBaseNode);
		void UpdateBoltInfo(double fMinH);
		CXhChar16 PosLabel();
	};
	class BASEPLATE{
	public:
		struct HOLE{short x,y,d,hole_d;};
		FOOTTEMPL_FLAG cStateFlag;	//������Ϣ��Դ�ڵ���ģ���ļ�
		char cMaterial;
		char shape;	//0:��ʾΪ����1:��ʾΪԲ��
		BYTE cNormOffsetType;	//0.��ƽ��;1.��ƽ��;2.ָ��ƫ��
		short nNormOffset;	//����ƫ��
		short thick;
		double fR;	//Բ�εװ��������Բ�뾶
		long XA,XB,YA,YB,XC[4],YC[4];	//A��ʾX(Y)������,B��ʾX(Y)�Ḻ��,C��ʾ���ǿ�
		CXhChar16 sPartNo;
		CLDSGeneralPlate* pPlate;
		CHashListEx<LINKANGLE>linkAngles;
		CXhSimpleList<HOLE> listHoles;
		BASEPLATE();
	};
	class SHOEPLATE{
	public:
		FOOTTEMPL_FLAG cStateFlag;	//������Ϣ��Դ�ڵ���ģ���ļ�
		char cMaterial;
		short thick;
		short minheight;		//ѥ�����С�߶ȣ�0��ʾ����������˨�����㣬>0ʱ�������������˨�����㳤�Ƚ϶�����Ҫ������˨�Ա�֤���� wjh-2018.5.29
		double fCutAngleWidth;//�ڵ��ǿ��
		short norm_offset;		//�ط��߷���ƫ�ƾ���
		KEY4C label;//��ǩ:��X0,��������Ϊ��������('X'��'Y')+�������(0.�ᴩ�װ�;1.֫����;2.���ߵ���)
		CXhChar16 sPartNo;
		CLDSGeneralPlate* pPlate;
		CHashListEx<LINKANGLE> linkAngles;
		bool m_bTranverse;	//true:�ᴩ����װ��ͨ�壬false:�м�ֶΰ�
		BOOL bAutoLayBolt;
	public:
		SHOEPLATE(DWORD key);
		CXhChar16 PosLabel();
	};
	class RIBPLATE{
		DWORD serial;
	public:
		DWORD Serial() const{return serial;}
		void SetKey(DWORD key){serial=key;}
		FOOTTEMPL_FLAG cStateFlag;	//������Ϣ��Դ�ڵ���ģ���ļ�
		char cMaterial;
		CXhChar16 sPartNo;
		int thick,A,B,C,D,MH,MF,MG;
		int axisXType;			//�߰�X���ȷ����ʽ��0.ָ���Ƕ�|1.X+|2.X-|3.Y+|4.Y-|5.ָ�����߽Ƕ�
		bool upper;				//�ϲ��߰廹���²��߰�
		union POS_PARA
		{
			struct{
				double OX,OY;			//�߰嶨λ�㣨�������ϵ�£�
				double axisX_x,axisX_y;	//X�᷽��
			}PARA1;		//ָ��X�᷽��
			struct{
				double angle;			//���߽Ƕ�
			}PARA2;
			struct{
				double angle;			//���촹�����߽Ƕ�
				double fRayDist;		//���߾���
			}PARA3;
		}pos_para;
		int water_slot_wide;			//��ˮ�ۿ�
		int iCutMode;					//�߰嵹��ģʽ��0.�н�|1.�л�
		double fCutAngleWidth;			//�����ڲ���нǿ��,mm
		double fGradientAngle;			//������б�Ƕȣ�������
		CLDSGeneralPlate* pPlate;
	public:
		RIBPLATE();
	};
	struct LINKSHOEPALTE{
		short A,B;	//ѥ�����̨�׸߼������������
		short C,D;	//ѥ���ڲ�̨�׸߶ȼ������ڲ�����
		short E,F;	//ѥ�������߽Ǹִ����ϲ����������²�������
		short M;	//ѥ���������װ��Եʱ�ļ�϶ֵM==0��ʾ�������װ��Ե
		short AL;	//ѥ������Ӳ�Ǹֳ��ȷ�����������Ĭ�ϸ��ݽǸ���˨����(����ϽǸ�Բ������ʱ���ĽǸֿ�������˨��ʱ��Ҫ�ֶ�ָ���ó���) wht 15-07-02
		short CM;	//ѥ�嵹��ģʽ��0.�н�|1.�л�
		char style_bolt0_weld1;//ѥ�������ĽǸֵ�������ʽ:0.��˨����;1.�Ժ�;2.�
		bool m_bBreakXueBan0C;
		bool m_bBreakXueBan90C;
		LINKSHOEPALTE();
	};
public:
	CHashPtrList<SHOEPLATE> listShoePlates;
	CHashListEx<RIBPLATE> listRibPlates;
	BASEPLATE base;
	short C;	//�Ǹֶ���㵽�װ����(��ֵ��ָ����ͷ)
	FOOTTEMPL_FLAG cStateFlag;	//������Ϣ��Դ�ڵ���ģ���ļ�
	bool m_bWeldDiZuo;
	bool quadModes[4];	//��ϽǸ��ڵװ��ĸ������е�״̬:true�����ĽǸ�(�����),false
	long m_hBaseNode;
	LINKANGLE primaryAngle,primaryAngleDown;
	LINKSHOEPALTE upLinkSP,downLinkSP;
	CLDSPoint origin;
	GEPOINT axis_z;	//�װ幤������
private:	//������Ź����е���ʱ����
	struct TEMPVAR{
		UCS_STRU ucs;	//�װ��װ������ϵ
		f3dPoint face_pick,wing_vec_x,wing_vec_y,datumRayVec;
		CLDSParamPlate *pXUpFrontPlank,*pXUpBackPlank,*pYUpFrontPlank,*pYUpBackPlank;
		CLDSParamPlate *pXDownFrontPlank,*pXDownBackPlank,*pYDownFrontPlank,*pYDownBackPlank;
	}tempory;
private:
	bool ImportFootDataFromXML(CMarkup xml,const XmlNode& xmlnode);
	bool ExportFootDataToXML(CMarkup& xml);
	void ExportXmlNodeLinkAngle(CMarkup& xml,LINKANGLE* pLinkAngle);
	void ExprotXmlNodeLinkXueBan(CMarkup& xml,LINKSHOEPALTE* pLinkShoePlate);
	BOOL DesignBasePlate();
	bool DesignShoePlates();
	BOOL DesignRibPlates();
	void CalBaseJgOddment(CLDSLineAngle * pBaseJg,BOOL bUpJg=TRUE);
	void LayBaseJgLs(bool upper);
	bool LayRayAngleBolt(ATOM_LIST<CDesignLsPara> *pLsList,CLDSLineAngle* pRayAngle,CLDSLineAngle *pBaseAngle,double *ber_space,double *wing_space);
	void PresetRayJgEndPos(CLDSLineAngle *pAngle,CLDSLineAngle *pBaseAngle,BYTE bolt0_weld1,double fPlateThick);
	void PresetGroupRayJgEndPos(CLDSGroupLineAngle* pGroupAngle,CLDSLineAngle *pBaseAngle,BYTE bolt0_weld1,double fPlateThick);
	bool DesignRayAngle(LINKANGLE *pLinkAngle,SHOEPLATE* pShoePlate);
	bool FindAssistPlates(RIBPLATE* pRibPlate,int bUpShoePlate,long& hAssistPlate1,long& hAssistPlate2);
public:
	CFootTempl();
	void GetUCS(UCS_STRU& ucs,bool bReUpdateCS=true);
	BOOL ToXmlTemplFile(const char* filename);
	BOOL FromXmlTemplFile(const char* filename);
	BOOL DesignFoot();
	void RetrieveShoePlates();
	void RetrieveLinkAngles(SHOEPLATE* pShoePlate=NULL);
	void RetrieveCirularDibanRibPlates();
	void InitRibPlateParaInfo(RIBPLATE* pRibPlate);
	void UpdateLinkAngleOddment(LINKANGLE& line_Angle,bool bUpJg=true);
	void UpdateLinkAngleZhunJu(LINKANGLE& line_Angle);
	void UpdateBoltEndSpace(LINKANGLE& line_Angle);
};

#endif