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
	bool IsFromTemplFile(){return (cbFlag&0x01)!=0;}//来源于模板文件
	bool IsFromModel(){return (cbFlag&0x02)!=0;}	//来源于实际模型结构提取
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
		FOOTTEMPL_FLAG cStateFlag;	//参数信息来源于导入模板文件
		KEY4C label;	//标识键值
		struct BOLTINFO{
			BYTE d;
			BYTE num;		//螺栓数量
			BYTE rows;		//0.单排 1.双排
			BYTE layout;	//0.单肢双排楞线侧靠前(双肢射线方向左高右低);1.单肢双排楞线侧靠后(双肢射线方向左低右高)
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
		double angleWidth;	//主要用于导入过往模板时检测角钢肢宽是否发生变化，如发生变化心距也将作废 wjh-2016.8.4
		double oddment;	//角钢连接端正负头
		short g,g1,g2;	//当前角钢的单排心距及双排的1、2排心距
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
		FOOTTEMPL_FLAG cStateFlag;	//参数信息来源于导入模板文件
		char cMaterial;
		char shape;	//0:表示为方板1:表示为圆板
		BYTE cNormOffsetType;	//0.上平面;1.下平面;2.指定偏移
		short nNormOffset;	//法高偏移
		short thick;
		double fR;	//圆形底板的外轮廓圆半径
		long XA,XB,YA,YB,XC[4],YC[4];	//A表示X(Y)轴正向,B表示X(Y)轴负向,C表示倒角宽
		CXhChar16 sPartNo;
		CLDSGeneralPlate* pPlate;
		CHashListEx<LINKANGLE>linkAngles;
		CXhSimpleList<HOLE> listHoles;
		BASEPLATE();
	};
	class SHOEPLATE{
	public:
		FOOTTEMPL_FLAG cStateFlag;	//参数信息来源于导入模板文件
		char cMaterial;
		short thick;
		short minheight;		//靴板的最小高度，0表示根据主材螺栓数计算，>0时如果根据主材螺栓数计算长度较短则需要补加螺栓以保证长度 wjh-2018.5.29
		double fCutAngleWidth;//内倒角宽度
		short norm_offset;		//沿法线方向偏移距离
		KEY4C label;//标签:如X0,命名规则为轴线沿向('X'或'Y')+外形类别(0.贯穿底板;1.肢翼单侧;2.楞线单侧)
		CXhChar16 sPartNo;
		CLDSGeneralPlate* pPlate;
		CHashListEx<LINKANGLE> linkAngles;
		bool m_bTranverse;	//true:贯穿整块底板的通板，false:中间分段板
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
		FOOTTEMPL_FLAG cStateFlag;	//参数信息来源于导入模版文件
		char cMaterial;
		CXhChar16 sPartNo;
		int thick,A,B,C,D,MH,MF,MG;
		int axisXType;			//肋板X轴的确定方式：0.指定角度|1.X+|2.X-|3.Y+|4.Y-|5.指定垂线角度
		bool upper;				//上侧肋板还是下侧肋板
		union POS_PARA
		{
			struct{
				double OX,OY;			//肋板定位点（相对坐标系下）
				double axisX_x,axisX_y;	//X轴方向
			}PARA1;		//指定X轴方向
			struct{
				double angle;			//射线角度
			}PARA2;
			struct{
				double angle;			//焊缝垂线射线角度
				double fRayDist;		//射线距离
			}PARA3;
		}pos_para;
		int water_slot_wide;			//流水槽宽
		int iCutMode;					//肋板倒角模式：0.切角|1.切弧
		double fCutAngleWidth;			//助板内侧的切角宽度,mm
		double fGradientAngle;			//助板倾斜角度，弧度制
		CLDSGeneralPlate* pPlate;
	public:
		RIBPLATE();
	};
	struct LINKSHOEPALTE{
		short A,B;	//靴板外侧台阶高及顶部外侧余量
		short C,D;	//靴板内侧台阶高度及顶部内侧余量
		short E,F;	//靴板在射线角钢处的上侧延伸量和下侧延伸量
		short M;	//靴板延伸至底板边缘时的间隙值M==0表示延伸至底板边缘
		short AL;	//靴板非连接侧角钢长度方向延伸量，默认根据角钢螺栓计算(做组合角钢圆形塔脚时主材角钢可能无螺栓此时需要手动指定该长度) wht 15-07-02
		short CM;	//靴板倒角模式：0.切角|1.切弧
		char style_bolt0_weld1;//靴板与主材角钢的连接形式:0.螺栓连接;1.对焊;2.搭焊
		bool m_bBreakXueBan0C;
		bool m_bBreakXueBan90C;
		LINKSHOEPALTE();
	};
public:
	CHashPtrList<SHOEPLATE> listShoePlates;
	CHashListEx<RIBPLATE> listRibPlates;
	BASEPLATE base;
	short C;	//角钢端棱点到底板距离(负值表指定负头)
	FOOTTEMPL_FLAG cStateFlag;	//参数信息来源于导入模板文件
	bool m_bWeldDiZuo;
	bool quadModes[4];	//组合角钢在底板四个象限中的状态:true有主材角钢(或包铁),false
	long m_hBaseNode;
	LINKANGLE primaryAngle,primaryAngleDown;
	LINKSHOEPALTE upLinkSP,downLinkSP;
	CLDSPoint origin;
	GEPOINT axis_z;	//底板工作法线
private:	//设计塔脚过程中的临时变量
	struct TEMPVAR{
		UCS_STRU ucs;	//底板的装配坐标系
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