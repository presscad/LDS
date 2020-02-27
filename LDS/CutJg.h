#pragma once
#ifdef __COMMON_PART_INC_

#include "InputAnValDlg.h"
struct ANGLECUT_ADVICE{
	struct ANGLECUT_PARA{
		char cCutWing;	//'X'：切X肢；'Y'：切Y肢；其它表示无切肢
		struct CUTWING{
			double fCutWingLength;	//对应...[][2]表示肢棱线切取长度
			double fCutBerLength;	//对应...[][0]表示角钢楞线切取长度
			double fCutWidth;		//对应...[][1]表示另一肢切取宽度
		}wing;
		struct CUTCORNER{
			double fLength;	//切割长度（角钢延伸方向），与cut_angle[0][...]对应
			double fWidth;	//切割宽度（角钢肢宽方向），与cut_angle[1][...]对应
		}corner[2];//0.X;1.Y
	}start,end;
	BOOL bCutStart;			//是否在射线角钢头部
	void Init(CLDSLineAngle* pAngle);
	void MergeCutting(const ANGLECUT_ADVICE& pAdvice,CLDSLineAngle* pAngle=NULL);
	void RetriveValidCutData();
	void Clear(){memset(this,0,sizeof(ANGLECUT_ADVICE));}
};
struct ANGLE_CUT_INFO 
{
public:
	static const BYTE RELATION_MAINJG_RAYJG			 =1;	//射线角钢与搭接主角钢	
	static const BYTE RELATION_RAYJG_RAYJG			 =2;	//射线角钢与射线角钢
	static const BYTE RELATION_ANGLE_BOLT			 =3;	//角钢与螺栓间的碰撞检查
	//
	static const BYTE CUT_TWORAYJG_BY_PLANE 		 =1;	//非共面射线角钢，根据主角钢的角平分面切割射线角钢
	static const BYTE CUT_TWORAYJG_BY_45DU			 =2;	//共面对等角钢(肢与肢发生碰撞)，通过45度切角进行切割
	static const BYTE CUT_RAYJG_BY_DATUMJG			 =3;	//主射角钢或共面非对等角钢发生肢与楞的碰撞，根据基准角钢切割射线角钢
	static const BYTE CUT_RAYJG_BY_PLANE			 =4;	//主射角钢发生肢与肢碰撞，通过主角钢肢翼侧确定切割面，切割射线角钢
	static const BYTE CUT_RAYJG_BY_BOLT				 =5;	//主射角钢发生肢与肢碰撞，通过主角钢肢翼侧确定切割面，切割射线角钢
public:
	SEGI iSeg;
	THANDLE hPart1,hPart2;
	f3dPoint face_pick,face_norm;
	BYTE cFlag;					//&0x01：同端点射线角钢切角 &0x02：主辅搭接角钢切角 &0x04：角钢与螺栓切角
	BYTE cRelationType;			//两角钢关系： 1基准角钢与射线角钢  2射线角钢与射线角钢
	union RELATION{
		struct{	//
			THANDLE hMainJg;
			THANDLE hRayJg;
			BOOL bCutStart;			//是否在射线角钢头部
			BOOL bInMainWingX;		//是否在主角钢X肢
			BOOL bInRayWingX;		//是否在射线角钢X肢
			BOOL bInboard;			//是否在主角钢内侧
		}MAIN_RAY;
		struct{	//
			THANDLE hMainJg;
			THANDLE hRayJg[2];
			BOOL bCutStart[2];		//是否在射线角钢头部
			BOOL bInMainWingX[2];	//是否在主角钢X肢
			BOOL bInRayWingX[2];	//是否在射线角钢X肢
			BOOL bInboard[2];		//是否在主角钢内侧
		}RAY_RAY;
		struct{	//
			THANDLE hAngle;
			THANDLE hBolt;
			BOOL bCutStart;			//角钢始端切角
			BOOL bCutWingX;			//角钢Ｘ肢切角
			BOOL bCutBer;			//背切肢
		}ANGLE_BOLT;
	}relation;
	BYTE cCutMode;	//角钢切割方式：
	union CUT_PARA{
		struct{
			THANDLE hDatumJg;
			THANDLE hRayJg;
			BOOL bCutStart;
		}CUT_ONE_BY_DATUM;
		struct{
			THANDLE hDatumJg;
			THANDLE hRayJg;
			BOOL bCutStart;
		}CUT_ONE_BY_PLANE;
		struct{
			THANDLE hRayJg1;
			BOOL bCutStart1;
			THANDLE hRayJg2;
			BOOL bCutStart2;
		}CUT_TWO_BY_PLANE;
		struct{
			THANDLE hRayJg1;
			BOOL bCutStart1;
			BOOL bInRayWingX1;
			THANDLE hRayJg2;
			BOOL bCutStart2;
			BOOL bInRayWingX2;
		}CUT_TWO_BY_45DU;
		ANGLECUT_ADVICE CUT_BY_BOLT;
	}cut_para;
};
class CCutJg
{	//
public:
	CHashList<ANGLE_CUT_INFO> hashAngleCuts;
public:
	static BOOL IsMainAndRayRelation(CLDSLineAngle* pJg1,CLDSLineAngle* pJg2,BOOL* pCutStart=NULL,BOOL* pMainFirst=NULL);
	//初始化角钢切割信息
	static BOOL InitJgAndJgCutInfo(ANGLE_CUT_INFO* pJgCutInfo,BYTE cFlag=0);
	//进行角钢切角检测
	static BOOL CollisionCheck(ANGLE_CUT_INFO* pJgCutInfo,BYTE cFlag=0);
	static BOOL CutDataCheck(ANGLE_CUT_INFO* pJgCutInfo);
	static int FinishCheckCutAngle(ANGLE_CUT_INFO* pJgCutInfo);
	//角钢切角操作
	static BOOL FinishCutAngle(ANGLE_CUT_INFO* pJgCutInfo,int (*InputCollideDist)(int dist),int nCutDist=0,BOOL bCheck=FALSE);
	//
	void Empty(){hashAngleCuts.Empty();}
	~CCutJg(){Empty();}
};
struct ANGLE_CUTDATA
{
	/*切肢参数说明
	 *cut_wing[0]表示始端
	 *cut_wing[1]表示终端
	 *cut_wing_para[0]表示始端背切肢参数
	 *cut_wing_para[1]表示终端背切肢参数
	 *cut_wing_para[][0]表示角钢楞线切取长度
	 *cut_wing_para[][1]表示另一肢切取宽度
	 *cut_wing_para[][2]表示肢棱线切取长度
	*/
	long cut_wing[2];	//标识所切角钢肢,0表示无切肢,1表示X肢,2表示Y肢
	double cut_wing_para[2][3];
	/*切角参数说明
	 *cut_angle[0]表示起点X肢
	 *cut_angle[1]表示起点Y肢
	 *cut_angle[2]表示终点X肢
	 *cut_angle[3]表示终点Y肢
	 *cut_angle[][0]表示平行楞线方向切取量  
	 *cut_angle[][1]表示垂直楞线方向切取量
	*/
	double cut_angle[4][2];
	BOOL m_bUserSpecStartCutAngle;
	BOOL m_bUserSpecEndCutAngle;
	ANGLE_CUTDATA(){memset(cut_angle,0,sizeof(cut_angle));memset(cut_wing_para,0,sizeof(cut_wing_para));memset(cut_wing,0,sizeof(cut_wing));}
};
struct BOLTSOLID_PARA{
	short d;	// 螺栓直径
  	double D;   // 螺帽直径.
  	double H;	// 螺栓头高度.
  	double H2;	// 螺帽高度.
	BOLTSOLID_PARA(short boltd=0,double _D=0,double _H=0,double _H2=0);
public:
	static ARRAY_LIST<BOLTSOLID_PARA> BOLT_LIBRARY;
	static bool QuerySolidParams(short boltd,BOLTSOLID_PARA* pBoltSolidParams);
};

struct BOLTSOLID_COLUMN
{
	double radius,height;
	GEPOINT start,end;	//起末点
	GEPOINT vStdAxis;	//start->end的单位向量
	BOLTSOLID_COLUMN(double r){radius=r;height=0;}
};
extern CCutJg cutJg;

//切割间隙
extern int InputCutAngleCollideDist(int dist);

#endif