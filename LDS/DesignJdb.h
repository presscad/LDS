// DesignJdb.h: interface for the CDesignJdb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DESIGNJDB_H__22B6EB81_E612_11D4_895A_00E098212149__INCLUDED_)
#define AFX_DESIGNJDB_H__22B6EB81_E612_11D4_895A_00E098212149__INCLUDED_
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Tower.h"

enum RAY_JOINT_STYLE{
	JOINT_HUOQU_LINE=0,
	JOINT_PRESET_ZHUN=1,
};
struct JG_STATE_STRU
{
	f3dPoint start_ber,end_ber;
	THANDLE handle;
	double start_oddment,end_oddment;
	// --------- 1.2. 切肢 -----------
	double cut_wing_para[2][3];
	//标识所切角钢肢,0表示无切肢,1表示X肢,2表示Y肢
	int cut_wing[2];
    // --------- 1.3. 切角 -----------
	double cut_angle[4][2];
	CJgBerPosPara start_pos,end_pos;
};	//角钢状态（仅抽取角钢中几个重要成员变量组成的结构）
struct LJB_DESIGN_PARA_STRU
{
	long plank_thick;		//板厚,板厚为0时表示连接不需要板
	char cMaterial;			//'S'表示Q235;'H'表示Q345
	int iRayIntersStyle;	//0表示交于预定心线;1表示交于火曲线上;2表示交于偏移准线
	BOOL bHuoQuPosByCtrlPt;	//火曲线位置根据用户输入预先输入的控制点定位
	BOOL bHuoQuNearBer[2];
	char sPartNo[MAX_CHAR_GUIGE_16+1];	//构件编号
	int nHuoQuDist[2];					//火曲线间隙距离角钢棱线
	int nOffsetZhun[2];					//偏移准线指由火曲线偏移距离
	int nVertexDist;					//角钢顶点干涉间隙
	BOOL iProfileCalType01;				//板外形计算类型(0定制外形1包络外形)
	double fNormOffset;					//连接板沿法向偏移量
	BOOL bWeldToDatumJg;				//节点板焊接到基准角钢(仅用于单面板) wht 10-06-06
	BOOL bAtAngleOuter;					//节点板交于基准角钢外皮  wxc-17.3.3
	double fInsertPlateEdgeSpace;				//插板边距
};
//对角钢上的单个螺栓描述结构
typedef struct tagLS_AT_JG_STRU
{
	tagLS_AT_JG_STRU(){memset(this,0,sizeof(tagLS_AT_JG_STRU));}
	char guige[MAX_CHAR_GUIGE_16+1];
	int pos_para_style;	//0.表示由螺栓孔相对距离来定;1.表示由交叉心线来定;2表示相对螺栓
	double len_dist;	//角钢长度方向距连接端的距离或所在基准角钢心线心距
	double ber_dist;	//角钢肢宽方向距离楞线的距离或射线角钢心距
	double L0;			//通过的厚度
	double depth_offset;//从外皮沿角钢工作肢法线的移动量
	BOOL bBaseLs;
	DWORD dwRayNo;		//螺栓射线号
	THANDLE hCrossJg;
}LS_AT_JG_STRU;
//对角钢连接时的接头与螺栓组的描述结构
struct JGLJ_ODDMENT_LS_STRU
{
	double oddment;
	BOOL m_bAutoCorrect;		//自动计算正负头
	ATOM_LIST<LS_AT_JG_STRU>ls_list;
};
typedef struct tagHUOQU_LINE_STRU	//火曲线位置
{
	CSuperSmartPtr<CLDSLineAngle> pBaseJg;
	BOOL bOffsetWingX;
	long offset_dist;				//火曲线偏移距离 
}HUOQU_LINE_STRU;
class CDesignJdb
{
	CLDSLineAngle _delegateAngle;
	BOOL m_bTerminateByUser;	//被用户终止
	int m_nViewFlag;			//工作面视向
	BOOL m_bRayEndFaceOffset;	//射线材端头平推处理，此时基准材螺栓默认也应为平推
	const static BYTE TYPE_COMMON_PLATE		= 0;	//普通板
	const static BYTE TYPE_TOP_SPLAY_PLATE	= 1;	//变电构架左右收口顶部八字板
	const static BYTE TYPE_TOP_FOLD_PLATE	= 2;	//变电构架左右收口顶部折叠板(三根射线杆件且其中一根始终端抬起高度一致并大于MIN_NORM_OFFSET)
	const static BYTE TYPE_BTM_FOLD_PLATE1	= 3;	//变电构架左右收口底部折叠板(两根射线杆件,两条火曲线)
	const static BYTE TYPE_BTM_FOLD_PLATE2	= 4;	//变电构架左右收口底部折叠板(三根射线杆件,四条火曲线)
	BYTE m_cPlateType;

	LJB_DESIGN_PARA_STRU xDesLjbPara;	//连接板设计参数
	JGLJ_ODDMENT_LS_STRU xDesJgResult;	//角钢设计结果(包括螺栓和正负头)
	HUOQU_LINE_STRU design_huoqu[2];	//火曲线的设计依据
	f3dPoint HuoQuJoint[2];				
	
	SmartRodPtr GetBaseRodPart(){
		return (m_pBasePart.IsHasPtr()&&m_pBasePart->IsLinePart())?m_pBasePart.pRod:NULL;
	}
	SmartPartPtr m_pBasePart;
	__declspec( property(get=GetBaseRodPart)) SmartRodPtr m_pBaseRod;

	SmartRodPtr m_pPrevPart;
	CSmartPtr<CLDSNode> m_pBaseNode;
	CDesignLsPara m_xBaseLs;			//对于有端连接的情况才有用(记录基准点上的螺栓)
	BOOL m_bAdjustWeldLen;	//是否调整同一节点上的所有连接板焊缝长度一致，基准定位杆件为钢管时此变量才其作用
	
	BOOL DesignLineFlatOrSlot(int iFaceNo,CLDSLinePart* pFlatOrSlot,BOLTSET *pLegacyBoltSet=NULL,BOOL bBasePart=FALSE);
	BOOL DesignLineTube(int iFaceNo,CLDSLineTube* pTube,CLDSLinePart* pNextPart=NULL);
	void DesLjbPara();
	void DesJgLjParaSet(BOOL bBaseJg=FALSE);
	bool CreateArcBasePlateAcs(CLDSLinePart* pRayRod);
	void CreateRefAxis();
	void Create3FaceRefAxis();
	void DesignHuoQu();
	BOOL DesignCommonPlank_Internal(BOOL bDisplayDlg);
// Operations
public:
	BOOL DesignCrossPlank(CLDSPlate* pPlank);
	bool AssureNormDirect(double* vNormCoords);	//返回值true:确保工作法向过程中进行了反向;false:方向保持未变
	BOOL DesignFoldPlank(CLDSPlate* pPlank);
	LINEPARTSET sum_partset,partset;
	TUBESET	face2_tubeset;//处理第二面有钢管情况
	JGSET face2_jgset,face3_jgset,end_lj_jgset;
	BOOL DesignOneBoltJdb();
	CLDSPlate * m_pPlank;
	//计算端连接角钢两侧的首个基准角钢螺栓位置
	void FirstMirLsPos(int ls_d, int initBoltSpace, double *head_offset_p, double *tail_offset_p);
	BOOL IsBerHuoQuJg(int i);
	void DesRayJgEndPos(CLDSLineAngle *pCurAngle, double fNormOffset);
	double PresetRayJgEndPos(CLDSLineAngle *pCurAngle);
	void QuitJgEndLsGroup(CLDSLineAngle *pJg, int quit_dist);
	void QuitJgStartLsGroup(CLDSLineAngle *pJg, int quit_dist);
	f3dPoint MutiFacePntToDatumFace(f3dPoint pt,int face_i,BOOL bFromMuti=TRUE);
	int GetJgInFaceI(CLDSLineAngle *pJg);
	double PresetRayFlatEndPos(CLDSLinePart* pCurFlatOrSlot);
	CDesignJdb();
	BOOL InputJdbLinePart();
	void SetFeatureJg();
	BOOL IsOneLsDesignValid();
	BOOL IsInsideJg(CLDSLineAngle *pJg,short *x_wing0_y_wing1=NULL);
	BOOL IsInsideJg(CLDSLineAngle *pJg,BYTE *x_wing0_y_wing1=NULL);
	//0表示始端连接，1表示终端连接，2表示中间连接
	int LjPosInPart(CLDSLinePart *pLinePart);
	static int LjPosInPart(CLDSLinePart *pDatumLinePart,CLDSNode *pDatumNode,CLDSLinePart *pLinePart);
	//火曲线定位基准方式：
	//0:由基准角钢推出1:由控制点确定
	void SetViewFlag(int nViewFlag){m_nViewFlag = nViewFlag;}
	//void CreateRayJg2Vertex(BOOL bBaseJg=FALSE);
	BOOL DesignCommonPlank(CLDSPlate* pPlank);		//设计生成V面节点板
	//装配定位杆件为钢管时,连板与定位杆件间需要焊接,调整同一节点上的焊接连接板焊缝线长度一致
	static BOOL AdjustPlateWeldLenOnSameNode(CLDSLinePart *pBaseLinePart,CLDSNode *pBaseNode);
private:
	void DesignLineAngle(int iFaceNo,CLDSLineAngle* pJg,CLDSLinePart* pNextPart=NULL,BOOL bBasePart=FALSE,
						CLDSLineAngle *pBackToBackJg=NULL,BOLTSET *pLegacyBoltSet=NULL);
	long CalJdbThick();
	void CalCurJgPreSetLsN(CLDSLineAngle *pCurAngle,BOOL bBasePart);
	void SortLinePartSet();
	void DesignHuoQuVertex(CLDSLineAngle *pBaseHuoQuJg, f3dPoint &v1, f3dPoint &v2);
};

/////////////////////////////////////////////////////////////////////////////

#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNJDB_H__22B6EB81_E612_11D4_895A_00E098212149__INCLUDED_)
