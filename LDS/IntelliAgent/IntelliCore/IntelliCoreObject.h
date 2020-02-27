#pragma once
#include "HashTable.h"
#include "ArrayList.h"
#include "List.h"
#include "MonoImage.h"
#include "f_alg_fun.h"
#include "LogFile.h"
#include "../IntelliModel.h"

#ifdef __RAPID_ROUGH_3DMODEL_

struct CONNECT_ARMROD{	//������ֱ����������ͷ�˼�
	char ciOnRightSide;	//'R':�ᵣ��X�������Ҳ�; 'L'�ᵣ��X�Ḻ�����;'U'����ͷ���¶��ϲ�
	IModelRod* pConnRod;
	double scopeMinZ,scopeMaxZ;
	long zJointCoord;
};
struct ARM_BAND{
	char ciOnRightSide;	//'R':�ᵣ��X�������Ҳ�; 'L'�ᵣ��X�Ḻ�����;'U'����ͷ���¶��ϲ�
	IModelRod* pUpperConnRod;
	IModelRod* pDownConnRod;
	double boundaryLineTopImgXY[2],boundaryLineBtmImgXY[2];	//�ᵣ������ֽ��ߣ�ͼ������ϵx->x;z->y)
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
	short siCurrScanPointY;//��ǰ��������ʼ��
	short siStartScanAngle;
	CMonoImage *pMonoImage;
	GEPOINT vCurrLenStdVec;//��ǰɨ������᷽��(�ѵ�λ��)
	long hCurrRod;
	GECS ocs;
	void CopyBasicInfo(SCAN_CONTEXT* pSrcContext)
	{
		siStartScanAngle=pSrcContext->siStartScanAngle;
		siCurrScanPointX=pSrcContext->siCurrScanPointX;
		siCurrScanPointY=pSrcContext->siCurrScanPointY;//��ǰ��������ʼ��
		pMonoImage=pSrcContext->pMonoImage;
		vCurrLenStdVec=pSrcContext->vCurrLenStdVec;//��ǰɨ������᷽��(�ѵ�λ��)
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
	GEPOINT lenStdVec;	//ͼ������ϵ��pRod�����췽���Ʊ߽���ʱ��)
	SEARCH_STEP step;
	short xHitI,yHitJ;	//�ҵ�pRod�˼�������λ��
	short majorStartI,minorStartJ;	//������ʼλ��
	short majorEndI,minorEndJ;		//������ֹλ��
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
	bool AddHeadType_ZM1A(STDTEMPL *pStdTempl,char ciSerial=0);	//ciSerial����Ҫȥ������Ϊͼʡ����ʱ��
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
	//������ȡ���ɵĺᵣͼ������ʵʱ�ĺᵣģʽ
	bool RecogStdArmTemplByCoreImage(STDTEMPL* pLiveMatchArmTempl,CMonoImage* pMonoImage,char ciArmsideOrHead,GECS& imgcs,double scaleImage2Model=0.01);
	bool RetrieveLiveEdgeRods(STDTEMPL::MATCHNODE *pMatchStart,STDTEMPL::MATCHNODE* pMatchEnd,GECS ocs,CXhSimpleList<STDTEMPL::EDGEROD>* pListEdgeRods);
	bool RetrieveLiveStdArmTemplRods(STDTEMPL* pLiveMatchArmTempl,GECS ocs,KEY_NODE keynodes);//IModelNode* pArmPostJointNodeRight=NULL,IModelNode* pArmPostJointNodeLeft=NULL);//��ȡ��ģ��ƥ����������˼�����
	void CalImgPoint(short* psiImgPointX,short* psiImgPointY,const double* modelPos,GECS& imgcs,double scaleImage2Model=0.01);
	bool SearchNextPofileEdgeRod(SCAN_CONTEXT& context,SEARCH_RESULT* pRslt,BYTE scan_radius=5);//,VERIFY_PIXEL *pVerifyPixel=NULL);
	bool RecogStdArmTemplPortal(STDTEMPL* pLiveMatchArmTempl,ARM_BAND *pArmBand);
	void ExtendArmBandScopeZ(ARM_BAND* pArmBand);
	/* 
	 * �ж��Ƿ���ʵ��ģ����ȡ�ĺᵣģʽƥ�䣬
	 * ��ƥ���Զ����������߽�˼��ı�ʶ���ڵ��ʶ����ʱ��˳���Զ�����
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
		bool m_bRightHand;		//true:��������z=x^y;false:��������z=y^x
		BYTE ciRadius;			//��ѡ�õ�ɨ��Բ�뾶3,4,5,6��ѡ
	public:
		char ciBaseEdgeIndex;	 //0����ʼ�߶�Ӧ�Ļ�׼������
		//short siBaseEdgeAngle;	//ͼ������ϵ���X�ᣨY�����£���׼��ʼ�߽Ƕȡ�
		BYTE SummaryScanPixels();//ѡ��ɨ��Բ�뾶����Ӧ��ȫ��������
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