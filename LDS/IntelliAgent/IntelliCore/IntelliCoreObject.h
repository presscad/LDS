#pragma once
#include "HashTable.h"
#include "ArrayList.h"
#include "List.h"
#include "MonoImage.h"
#include "f_alg_fun.h"
#include "LogFile.h"
#include "../IntelliModel.h"

#ifdef __RAPID_ROUGH_3DMODEL_

struct CONNECT_ARMROD{	//与塔身直接相连的塔头杆件
	char ciOnRightSide;	//'R':横担在X轴正向右侧; 'L'横担在X轴负向左侧;'U'：塔头在坡段上部
	IModelRod* pConnRod;
	double scopeMinZ,scopeMaxZ;
	long zJointCoord;
};
struct ARM_BAND{
	char ciOnRightSide;	//'R':横担在X轴正向右侧; 'L'横担在X轴负向左侧;'U'：塔头在坡段上部
	IModelRod* pUpperConnRod;
	IModelRod* pDownConnRod;
	double boundaryLineTopImgXY[2],boundaryLineBtmImgXY[2];	//横担与塔身分界线（图像坐标系x->x;z->y)
	double scopeMinZ,scopeMaxZ;
};
struct SEARCH_STEP{
	short siStepMajor,siStepMinor;
	struct SEARCH_DIRECTION{double vfStdScanX,vfStdScanY;}major,minor;
	void UpdateMinorDirection()
	{
		minor.vfStdScanX= major.vfStdScanY;
		minor.vfStdScanY=-major.vfStdScanX;
	}
};
struct VERIFY_PIXEL{
	long hVerifyRod;
	BYTE ciScanRadius;
	VERIFY_PIXEL(long hRod,BYTE ciRadius=2){
		hVerifyRod=hRod;ciScanRadius=ciRadius;
	}
};
class SCAN_CONTEXT{
public:
	short siCurrScanPointX;
	short siCurrScanPointY;//当前搜索的起始点
	short siStartScanAngle;
	CMonoImage *pMonoImage;
	GEPOINT vCurrLenStdVec;//当前扫描的主轴方向(已单位化)
	long hCurrRod;
	GECS ocs;
	void CopyBasicInfo(SCAN_CONTEXT* pSrcContext)
	{
		siStartScanAngle=pSrcContext->siStartScanAngle;
		siCurrScanPointX=pSrcContext->siCurrScanPointX;
		siCurrScanPointY=pSrcContext->siCurrScanPointY;//当前搜索的起始点
		pMonoImage=pSrcContext->pMonoImage;
		vCurrLenStdVec=pSrcContext->vCurrLenStdVec;//当前扫描的主轴方向(已单位化)
		hCurrRod=pSrcContext->hCurrRod;
		ocs=pSrcContext->ocs;
	}
	CHashSet<IModelRod*> hashRecogRods;
	SCAN_CONTEXT(SCAN_CONTEXT* pSrcContext=NULL){
		if(pSrcContext)
			CopyBasicInfo(pSrcContext);
		else
		{
			pMonoImage=NULL;
			siStartScanAngle=15;
			siCurrScanPointX=siCurrScanPointY=0;
			hCurrRod=0;
		}
	}
};
struct SEARCH_RESULT{
	long hRod;
	IModelRod* pRod;
	GEPOINT lineStart,lineEnd;
	GEPOINT lenStdVec;	//图像坐标系下pRod的延伸方向（绕边界逆时针)
	SEARCH_STEP step;
	short xHitI,yHitJ;	//找到pRod杆件的像素位置
	short majorStartI,minorStartJ;	//搜索起始位置
	short majorEndI,minorEndJ;		//搜索终止位置
	short StepX(short major,short minor){
		return ftoi(major*step.major.vfStdScanX+minor*step.minor.vfStdScanX);
	}
	short StepY(short major,short minor){
		return ftoi(major*step.major.vfStdScanY+minor*step.minor.vfStdScanY);
	}
	SEARCH_RESULT(){hRod=0;pRod=NULL;xHitI=yHitJ=-1;}
};
class CIntelliCore{
	static int PREDEF_ARM_COUNT;// =19;
	static const int PREDEF_ARM_PART_COUNT =5;
	double SCALE_OF_IMAGE2MODEL;
	ARRAY_LIST<STDTEMPL> ARM_ARR;//PRESET_ARRAY32 [PREDEF_ARM_COUNT];
	STDTEMPL ARM_PART_ARR[PREDEF_ARM_PART_COUNT];
	bool AddHeadType_ZB1(STDTEMPL *pStdTempl);
	bool AddHeadType_ZB2(STDTEMPL *pStdTempl);
	bool AddHeadType_ZB2A(STDTEMPL *pStdTempl);
	bool AddHeadType_ZBnZJC(STDTEMPL *pStdTempl);
	bool AddHeadType_ZM1A(STDTEMPL *pStdTempl,char ciSerial=0);	//ciSerial将来要去掉，现为图省事临时加
	bool AddHeadType_ZM2(STDTEMPL *pStdTempl);
	bool AddHeadType_ZMC(STDTEMPL *pStdTempl);
	bool AddHeadType_HD1(STDTEMPL *pStdTempl);
	int InitClassicArmTemplArr();
	COLINEAR_TOLERANCE xTolerance;
	ILog2File* m_pLogErrFile;
	union KEY_NODE{
		struct ZB{
			IModelNode *pMidBeamTopNode,*pMidBeamBtmNode;						//type=21
			IModelNode *pArmBeamJointNodeOutRight,*pArmBeamJointNodeOutLeft;	//type=22
			IModelNode *pArmBeamJointNodeInnerRight,*pArmBeamJointNodeInnerLeft;//type=23
			IModelNode *pArmPostJointNodeRight,*pArmPostJointNodeLeft;			//type=20
		}zb;
		struct ZM{
			IModelNode *pMidBeamTopNode,*pMidBeamBtmNode,*pBtmArmMidNode;						//type=21
			IModelNode *pArmBeamJointNodeOutRight,*pArmBeamJointNodeOutLeft;	//type=22
			IModelNode *pArmBeamJointNodeInnerRight,*pArmBeamJointNodeInnerLeft;//type=23
			IModelNode *pArmBeamUpperInnerNodeRight,*pArmBeamUpperInnerNodeLeft;//type=27
			IModelNode *pArmBeamUpperTipNodeRight,*pArmBeamUpperTipNodeLeft;	//type=24
			IModelNode *pArmBeardJointNodeOutRight,*pArmBeardJointNodeOutLeft;		//type=25
			IModelNode *pArmBeardJointNodeInnerRight,*pArmBeardJointNodeInnerLeft;	//type=26
			IModelNode *pArmPostJointNodeRight,*pArmPostJointNodeLeft;				//type=20
		}zm;
		struct ZL{
			IModelNode *pMidBeamNode;						//type=28
		}zl;
		KEY_NODE(){
			/*zb.pMidBeamTopNode=zb.pMidBeamBtmNode=
				zb.pArmBeamJointNodeInnerRight=zb.pArmBeamJointNodeInnerLeft=
				zb.pArmBeamJointNodeOutRight=zb.pArmBeamJointNodeOutLeft=
				zb.pArmPostJointNodeRight=zb.pArmPostJointNodeLeft=NULL;*/
			memset(this,0,sizeof(KEY_NODE));
		}
	};
private:
	ILog2File* LogErrFile();
	_declspec(property(get=LogErrFile)) ILog2File* pLogErr;
	ITowerModel* m_pModel;
	bool RecogZBorZMInnerKeyNodes(STDTEMPL* pLiveMatchArmTempl,CMonoImage* pMonoImage,GECS& ocs,KEY_NODE* keynodes);
	//根据提取生成的横担图像生成实时的横担模式
	bool RecogStdArmTemplByCoreImage(STDTEMPL* pLiveMatchArmTempl,CMonoImage* pMonoImage,char ciArmsideOrHead,GECS& imgcs,double scaleImage2Model=0.01);
	bool RetrieveLiveEdgeRods(STDTEMPL::MATCHNODE *pMatchStart,STDTEMPL::MATCHNODE* pMatchEnd,GECS ocs,CXhSimpleList<STDTEMPL::EDGEROD>* pListEdgeRods);
	bool RetrieveLiveStdArmTemplRods(STDTEMPL* pLiveMatchArmTempl,GECS ocs,KEY_NODE keynodes);//IModelNode* pArmPostJointNodeRight=NULL,IModelNode* pArmPostJointNodeLeft=NULL);//提取与模板匹配的外轮廓杆件集合
	void CalImgPoint(short* psiImgPointX,short* psiImgPointY,const double* modelPos,GECS& imgcs,double scaleImage2Model=0.01);
	bool SearchNextPofileEdgeRod(SCAN_CONTEXT& context,SEARCH_RESULT* pRslt,BYTE scan_radius=5);//,VERIFY_PIXEL *pVerifyPixel=NULL);
	bool RecogStdArmTemplPortal(STDTEMPL* pLiveMatchArmTempl,ARM_BAND *pArmBand);
	void ExtendArmBandScopeZ(ARM_BAND* pArmBand);
	/* 
	 * 判断是否与实际模型提取的横担模式匹配，
	 * 若匹配自动更新轮廓边界杆件的标识，节点标识按逆时针顺序自动编制
	 */
	static bool MatchWith(STDTEMPL* pStdArmTempl,STDTEMPL* pLiveArmTempl);
	static bool MatchWith(STDTEMPL::CONNPOINT_SCHEMA* pConnPointSchema,STDTEMPL::CONNPOINT* pConnPoint);
public:
	struct HIT_TEST{
		long hRod,hits;
		HIT_TEST(){hRod=hits=0;}
	};
	struct PIXEL{char X,Y;};
	class CIRCLE_SCAN{
		bool m_bRightHand;		//true:右手螺旋z=x^y;false:左手螺旋z=y^x
		BYTE ciRadius;			//所选用的扫描圆半径3,4,5,6可选
	public:
		char ciBaseEdgeIndex;	 //0度起始边对应的基准索引号
		//short siBaseEdgeAngle;	//图像坐标系相对X轴（Y轴向下）基准起始边角度°
		BYTE SummaryScanPixels();//选定扫描圆半径所对应的全部索引号
		//bool Init(short siLenRayVecDegAng,BYTE radius=4);
		bool Init(const double* lenStdVec2d,BYTE radius=4);
		short ScanPixels(double fromDegAngle=15,double toDegAngle=345,bool righthandcs=true,PIXEL* scanpixels=NULL,short maxpixels=0);
		short ScanSectorPixels(int xOrgI,int yOrgJ,int xCurrI,int yCurrJ,BYTE radius=2,bool righthandcs=true,PIXEL* scanpixels=NULL,short maxpixels=0);
	};
	CIntelliCore();
	int RecognizeAllStdTempl(ITowerModel* pModel,const double* vxSlopeTop3d,const double* vxSlopeBtm3d,IXhList<STDTEMPL>* pListLiveArms,ILog2File* pLogErrFile=NULL,UINT uiMinKeyLength=300,COLINEAR_TOLERANCE* pxTolerance=NULL);
	bool RecognizeConnPointSchema(STDTEMPL::CONNPOINT *pConnPoint);
};

#endif