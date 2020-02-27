#ifndef __PART_GROUP_DRAWING_H_
#define __PART_GROUP_DRAWING_H_

#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "PartLib.h"
#include "XhLdsLm.h"
#include "Buffer.h"
#include "XhCharString.h"
#include "DrawUnit.h"
#include "LocalDrawing.h"
#include "FormatTable.h"
#include "list.h"
#include "MemberProperty.h"
#include "RectLayout.h"
#include "DbEntContext.h"
#include "PlateDrawer.h"
#include "JgDrawing.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifndef __TSA_FILE_
class CLDSPart;
class CLDSPartGroup;
typedef struct tagPART_GROUP_CARD_INFO
{
	CXhChar16	m_sPartNo;		//����
	CXhChar16   m_sSegStr;		//�κű�ʶ
	CXhChar16	m_sLength;		//����
	CXhChar16	m_sMaterial;	//��Ʋ���
	CXhChar16	m_sGuiGe;		//��ƹ��
	CXhChar16	m_sPartNum;		//������
	CXhChar16	m_sSumPartNum;	//�ܼӹ���=������*����
	CXhChar16	m_sCutRoot;		//�ٸ�
	CXhChar16	m_sCutBer;		//����
	int			m_nCutBerR;		//����Բ���뾶mm,һ���������ĵĽǸ��ڻ�ֵr+2
	CXhChar16	m_sCutAngSX;	//ʼ��X֫�н�	
	CXhChar16	m_sCutAngSY;	//ʼ��Y֫�н�
	CXhChar16	m_sCutAngEX;	//�ն�X֫�н�
	CXhChar16	m_sCutAngEY;	//�ն�Y֫�н�
	CXhChar16	m_sCutAngParaSX;//ʼ��X֫�нǲ�����ʾ
	CXhChar16	m_sCutAngParaSY;//ʼ��Y֫�нǲ�����ʾ
	CXhChar16	m_sCutAngParaEX;//�ն�X֫�нǲ�����ʾ
	CXhChar16	m_sCutAngParaEY;//�ն�Y֫�нǲ�����ʾ
	CXhChar16	m_sPushFlat;	//ѹ��
	CXhChar16	m_sWeld;		//����
	CXhChar16	m_sKaiJiao;		//����
	CXhChar16	m_sHeJiao;		//�Ͻ�
	CXhChar16	m_sKaiHeJiao;	//���ϽǾ������
	CXhChar50	m_sNotes;		//��ע
	CXhChar50	m_sFootNailPlate;//�Ŷ��������ϱ�
	int m_nPageNum,m_nPageIndex;//�� ҳ,�� ҳ
	int m_nTaNum;				//���� 
}PART_GROUP_CARD_INFO;

class CAttachPartsSection
{
public:
	double m_fStartX,m_fEndX;
	CAttachPartsSection(){m_fStartX = m_fEndX=0;}
};


/*typedef struct tagPARTGROUP_ANGULARDIM_STRU
{
	f3dPoint vertex;
	f3dPoint startPos;
	f3dPoint endPos;
	f3dPoint dimPos;
}PARTGROUP_ANGULARDIM_STRU;*/

//��չ���ݵ��������湹�������Ϣ
/*typedef struct tagDATAPOINT_STRU
{
	f3dPoint	dim_pos;	//��עλ��
	f3dPoint	data_pos;	//����λ��
	char		sPartNo[16];//�������
	int			cur_layer;	//��ǰ��
	long		hPart;		//��ع������
	int			cls_id;		//��ع�������
}DATAPOINT_STRU;*/
//CDrawingEntsContext ����DbEntContext.h wht 18-08-23
class CSectionViewDrawing;
class CSectionDrawingEntsManager : public CDrawingEntsContext{
	CLDSPart* m_pRootPart;	//�麸���Ļ�׼����
public:
	struct VERTICAL_PLATE{
		CLDSGeneralPlate *pPlate;
		f3dLine face_line[3][2];	//[0~2][0]Ϊ��
		double fParallelDist;		//ƽ������ְ����
		f3dLine parallel_line;		//����ƽ����
		GEPOINT startBoltPos,endBoltPos;	//z>0��ʾδ��ʼ��
		BOOL bDimStart,bDimEnd;
		VERTICAL_PLATE(){pPlate=NULL; fParallelDist=0;startBoltPos.z=endBoltPos.z=1000;bDimStart=bDimEnd=TRUE;}
	};
	f3dLine tube_edge_line[2];	//�ֹ������ߣ��޶��������Ӱ����߰�����ʱ�õ� wht 18-08-25
	CSectionViewDrawing* ValidSectDrawing();
	CHashList<VERTICAL_PLATE> m_hashVertPlateList;
	struct HORZ_PLATE{
		CLDSGeneralPlate *pPlate;
		ARRAY_LIST<f3dLine> edgeLineList;
	};
	CHashList<HORZ_PLATE> m_hashFootConnectPlateList;
	void UpdateVerticalPlateParallelLine(GELINE line);
	void UpdateVerticalPlateLocateBoltPos(GEPOINT bolt_pos);
public:
	CSectionDrawingEntsManager(CLDSPart* pRootPart=NULL,CSectionViewDrawing* pSectDrawing=NULL,STORE_MODE cStoreMode=STORE_ACAD){
		InitContext(pRootPart,pSectDrawing,cStoreMode);
	}
	CSectionDrawingEntsManager(CLDSPart* pRootPart,IDrawingCommand* pXeroCmd,STORE_MODE cStoreMode=STORE_ACAD){
		InitContext(pRootPart,pXeroCmd,cStoreMode);
	}
	CSectionDrawingEntsManager(CLDSPart* pRootPart,IDrawing* pXeroDrawing,STORE_MODE cStoreMode=STORE_ACAD){
		InitContext(pRootPart,pXeroDrawing,cStoreMode);
	}
	~CSectionDrawingEntsManager(){;}
	void InitContext(CLDSPart* pRootPart,CSectionViewDrawing* pSectDrawing,STORE_MODE cStoreMode=STORE_ACAD);
	void InitContext(CLDSPart* pRootPart,IDrawingCommand* pXeroCmd,STORE_MODE cStoreMode=STORE_ACAD);
	void InitContext(CLDSPart* pRootPart,IDrawing* pXeroDrawing,STORE_MODE cStoreMode=STORE_ACAD);
	void AppendAngularDim(f3dPoint vertex,f3dPoint start_vec,f3dPoint end_vec,double fDimR=0,char *dimText=NULL,double fDimAngleTextSize=3.0);
	void AppendAngularDim(CSizeAngleDim& angularDim);
	void AppendLine(f3dLine& line);
	void AppendLine(f3dArcLine& arcline);
	void AppendSpLine(fPtList& vertex_list);
	void AppendCircle(f3dCircle& circle);
	void AppendSizeTextDim(CSizeTextDim& sizeTextDim);
	void AppendTextDim(CTextOnlyDrawing& pureTextDim);
	//�������ݱ�ʶ��
	void AppendDataPoint(CTextFrame& dataPoint);
	DATAPOINT_TAG* AppendDataPoineTag();
	virtual double ZoomToModelTextSize(double fSysFontSize);
	//���ƺ�����
	void DrawWeldLineSymbol(f3dLine line,f3dPoint pickPt,double width=0,double height=0);
	void DrawWeldLineSymbol(f3dPoint start,f3dPoint end,double width=0,double height=0);
	void DrawWeldLineSymbol(f3dArcLine arcline,double width=0,double height=0);
	double TransToModelLength(double scr_draw_length);
	//�ֹܶ���ͼ�Ļ���Ǳ�ע��׼��
	void AppendTubeBaseLine(BYTE iMode,CLDSLineTube* pBaseTube,f3dPoint arrowVec,f3dPoint arrowBtm,f3dPoint arrowHead);
	void AppendTubeTaCoreVecLine(f3dPoint vec,f3dPoint xBtm,f3dPoint xHead);

public:
	virtual bool IsSectionDrawing(){return true;}
	virtual bool IsBranchRod(CLDSLinePart* pBranchRod){
		if(m_pRootPart==NULL||!m_pRootPart->IsLinePart())
			return false;
		return pBranchRod!=m_pRootPart;
	}
	virtual CLDSPart* WeldRootPart(){return m_pRootPart;}
};
class CPlateDrawing{
	void AnalizePoleRay(CLDSPlate *pPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext);//�����ְ����߸˼�
	void DimVerticalPlateLocateSize(CLDSPlate *pPlate,IDbEntsContext* pContext);	//��ע���嶨λ�ߴ�
public:
	struct PLATE_DRAWING_STYLE{
		BYTE geomcenter0_left1_right2;	//��Ϣ��ǵ�λ��=0
		bool bDrawWeldLine;	//=TRUE���ƺ����߱��
		bool bDimBoltSpace;		//��ע��˨���
		bool bDimBoltEdgeSpace;	//��ע��˨�߾�
		bool bDimRayRodAngle;	//�Ƿ���Ҫ��ע������߷����Ƕ�
		bool bDrawInnerCircle;	//Բ�ε׽Ÿְ����Բ���ڸֹ�����ѻ�������£����ٻ���
		bool bMandatoryDimLabel;//ǿ���Ա�ע���
		bool bSpreadBendPlane;	//���ƻ�������˨�Ͷ���ʱ����������չ�����ƣ�һ�㲻��Ҫչ����������ڹ������麸ͼ����ʱ���漰����߱�ע����Ҫչ��) wjh-2017.10.24
		char ciDrawModeH0V1;
		BYTE cbDrawPlanes;
		short penStyle;	//��ӦԭbCoplanar=TRUEʱ������PS_DASH����
		short siMinFilterZ,siMaxFilterZ;	//���Ƹְ����ͼʱ�������Z���꣨��ͼ����ϵ�£���Χ wjh-2018.3.10
	}style;
	CPlateDrawing();
	BOOL CreatePlateDrawing(CLDSPlate *pPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext,AcDbObjectId *pOutEntId=NULL,int h0v1=-1);
	//��繹�ܺ����۵��������Ҫ��������
	BOOL CreateSubstationFoldPlateDrawing(CLDSPlate *pPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext,AcDbObjectId *pOutEntId=NULL);
	//��繹�ܶ������ֻ����������Ҫ��������
	BOOL CreateSubstationSplayFoldPlateDrawing(CLDSPlate *pPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext,AcDbObjectId *pOutEntId=NULL);
};
struct ACAD_HATCH
{
	fPtList vertexList;
	CXhChar50 sHatchStyle;
	double fScale;
	double fAngle;
	COLORREF color;
	void ZoomScale(double zoomCoef);
	ACAD_HATCH(){fScale=fAngle=0; color=RGB(255,255,255);sHatchStyle.Copy("SOLID");fScale=1;}
};
struct RADIAL_ITEM{
	long hObj;
	BYTE ciObjType;			//���߷�������ͣ�Ŀǰ�������ڽ�����չ�洢�����������ı�ʶ)
	bool bDualDirection;	//����˫�������
	double radAngle;
	GEPOINT rayVec,rayOrg;	//ģ������ϵ������
public:
	RADIAL_ITEM(){radAngle=hObj=ciObjType=0;bDualDirection=false;}
};
class CDimAngularManager{
public:
	struct ANGULARDIM_ITEM{
		long hStartObj,hEndObj;
		double  fDimRadius;			//�Ӷ��㵽��עԲ���ľ���
		GEPOINT vTipPoint;			//��ע�ǵĶ���
		GEPOINT vStartVec,vEndVec;	//��ע�ǵ���ʼ��ֹ�ߣ�ģ������ϵ������ֵ��
		double degSectAngle;
	};
private:
	CXhSimpleList<ANGULARDIM_ITEM> m_xAngularDimList;
public:
	GEPOINT m_vNorm;
	GEPOINT m_vStartEdgeAxisVec;
	double m_fDimAngleTextSize;
public:
	CDimAngularManager(double fDimTextSize=2);
	~CDimAngularManager();
	void CreateAngularItems(const double* normal,const double* cStartEdgeAxisVec,const double* vTubeLenVec,RADIAL_ITEM* items,int count);
	ANGULARDIM_ITEM* EnumFirstDimItem(){return m_xAngularDimList.EnumObjectFirst();}
	ANGULARDIM_ITEM* EnumNextDimItem(){return m_xAngularDimList.EnumObjectNext();}
};
class CPartGroupDrawing;
struct PART_DIMnDRAW{
	long hPart,idView;
	char ciDrawModeH0V1;
	BYTE ciAttachPlateType;	//��������������ʱ��ͬ������������(��ʮ�ֲ�������塢�׽��߰��)
	BYTE ciLeadLineMode;	//0.��������;1.������;2.�Զ��ж�������
	BYTE cbDrawPlanes;	//���Ըְ���Ч��0x01:���ƻ���;0x02:����1�Ż�����;0x04:����2�Ż�����
	bool bDrawSketch;	//true:��hView��ͼ�л���;false:������
	bool bDimPartNo;	//true:��hView��ͼ�б�ע�������
	bool bMandatoryDimLabel;//ǿ���Ա����ע���
	CLDSPart* pPart;	//hPartָ��Ĺ�������
	GEPOINT vPreferDimFlowVec;	//������ŵ������Զ���ע����������
	PART_DIMnDRAW();
};
class CSectionViewDrawing : public CXeroDrawingComponent
{
	friend class CPartGroupDrawing;
public:
	struct SEARCH_CELL{
		int xi,yj;
		double weight;//��������Ȩ�أ�ԽСԽ����
		static double fHoriPriorCoef;
		static double fVertPriorCoef;
	public:
		SEARCH_CELL(int x=0,int y=0);
		double CalPriorWeight(int x,int y);
	};
	struct SECT_SYMBOL_ENT
	{
		CTextOnlyDrawing *pText;		//��ͼ����
		CTextOnlyDrawing *pScaleDimText;//��ͼ���Ʊ���
		IDbPolyline* pLine1;	//����
		IDbLine* pLine2;		//ϸ��
		SECT_SYMBOL_ENT(){pText=pScaleDimText=NULL;pLine1=NULL;pLine2=NULL;}
	} sectionEnt;
	IDbLine *m_pArrowLine1,*m_pArrowLine2;				//��ͨ��ͷ
	IDbPolyline *m_pArrowPolyLine1,*m_pArrowPolyLine2;	//�����ͷ
	//����֮��̬ȷ�ϵĻ�����صĲ���
	DRAWRECT drawingRect;
protected:
	UCS_STRU cs;
	IModel *m_pTower;
	CLDSPart *_pRootPart;
	//CLDSPartGroup *m_pPartGroup;
	CSectionView *m_pSectionView;
	BOOL m_bSamePartNoDimOnce;				//ͬһ���Ž���עһ��,Ĭ������,������ͼ(�����Ű帩��ͼ)ʱ��Ҫ�ر� wht 15-04-09
	double m_fUser2ScrDrawScale;			//��ͼ����
	bool m_bFinalDrawing;	//�Ƿ���ִ����RelayoutDrawing()
	//��ͼ������
	ATOM_LIST<f3dCircle>cirList;			//Բ
	ATOM_LIST<f3dArcLine>edgeList;			//����
	ATOM_LIST<ACAD_HATCH>hatchList;			//���
	//��ͼ�еĸ����ע
	ATOM_LIST<CTextFrame>dataPointList;			//�������ݵ�
	ATOM_LIST<CSizeAngleDim>angularDimList;		//�Ƕȱ�ע
	ATOM_LIST<CSizeTextDim>spaceDimList;		//��Գߴ�
	ATOM_LIST<CTextOnlyDrawing>pureTextDimList;	//���Գߴ�
	CHashList<DATAPOINT_TAG> m_xDataPointTagHash;
	//void AnalizePoleRay(CLDSPlate *pPlate,CDrawCoordTrans& dcs);//�����ְ����߸˼�
	PART_DIMnDRAW* RegisterPartDimDraw(CLDSPart *pPart,int idPartClsType,char ciDrawModeH0V1,bool bMandatoryDimLabel=false);
	BOOL CreatePlateDrawing(CLDSPlate *pPlate,CDrawCoordTrans& dcs,BOOL bCoplanar=TRUE,BOOL bDrawWeldLineByWeldProp=TRUE,BYTE geomcenter0_left1_right2=0,int h0v1=-1);
	BOOL CreateParamPlateDrawing(CLDSParamPlate *pParamPlate,CDrawCoordTrans& dcs,BOOL bCoplanar=TRUE,BOOL bDrawWeldLineByWeldProp=TRUE,int h0v1=-1);
	/*static*/ BOOL CreateLineAngleDrawing(CLDSLineAngle *pLineAngle,CDrawCoordTrans& dcs);
	//���ɽǸ��Ϻ��ӵĽǸ��߰��������
	void CreateJgRibPlateEdge(CLDSLineAngle *pJg,f3dLine ber_line,f3dPoint wing_vec,BOOL bInsideJg);
	/*static*/ BOOL CreateLineTubeDrawing(CLDSLineTube *pLineTube,CDrawCoordTrans& dcs,BOOL bDrawWeldLineByWeldProp=TRUE);
	BOOL CreateLineSlotDrawing(CLDSLineSlot *pLineSlot,CDrawCoordTrans& dcs);
	BOOL CreateCutTubeDrawing(CLDSLineTube *pLineTube,BOOL bCoplanar=TRUE);		//��ڵ��麸���еĸֹ�ʾ��ͼ
	BOOL CreateSphereDrawing(CLDSSphere* pSphere);			//��ڵ��躸���е�����ʾ��ͼ
	BOOL CreateSpecialSectionView();	//�����������ͼ wht 10-08-13
	void DimCheckLine();	//��ע�����
	void DimFlCheckLine();	//����ڵ��麸������ӷ��������
	void DimAssemblyPos();	//����ڵ��麸�������װ�����
	void DimAngle();		//��ע�Ƕ�
	void AppendDefaultAngularDimension(CAngularDimension* pAngular,CLDSLineTube* pDatumLineTube,
		const double* vWeldRoadDirect,BOOL bIncludeDatumTube,CLDSLineTube *pJointTube);
	void DimSectionViewSymbol();//��ע����ͼ��
	bool DimSectionViewSymbol2();
	CSizeTextDim* NewSizeDimByLocatePoint(CSizeTextDim *pPrevSizeDim,int iOrderType,
		CPartGroupLocatePoint *pPrevLocate,CPartGroupLocatePoint *pLocate);
	void StatSizeDimsByLocatePoint(CPartGroupLocatePoint locatePointArr[],int num,int iOrderType);
	void DimLocatePoint(CLDSLinePart *pDatumPart);	//��ע�궨��λ��
	void ResetPosition();	//��������ԭ��
	//Ϊ�����Ч���������ֱ������������ǵ�λ������,�ٶ�basePosλ�ھ�������
	bool IsHasIntersObj(f3dPoint rgnVert[4],void *pExcusiveSelfDrawObj=NULL);
	bool SearchNoneIntersPos(f3dPoint rgnVert[4],const double* vcVertSearchVec,
		double maxHroiSearchStep=5.0,int maxVertSearchStep=5,bool bEnableHoriSearch=true,bool bSearchVertPrefer=true,bool bDuoDirect=true);
	bool SearchNoneIntersPosEx(f3dPoint rgnVert[4],const double* vcSearchFlowDirection,
		double maxHoriSearchDist=5.0,double maxVertSearchDist=5,double fHoriPriorCoef=0.5,double fVertPriorCoef=0.5,
		char cbDualDirectMode=0x0f,void *pExcusiveSelfDrawObj=NULL);
	void RelayoutDrawing(double fSpecZoomCoef=0.0);	//fSpecZoomCoef<=0.0��ʾ����m_fUser2ScrScale���� wjh-2018.3.7
	void ClearDimItemFlag();
public:
	CSectionDrawingEntsManager m_xEntManager;
	CLDSPartGroup *m_pPartGroup;
	//����ͼ����עλ�ü���
	struct SECTSYMBOLDIM{
		GEPOINT xLineStart,xLineEnd,xPrjTipPoint;
		GEPOINT xTextDimPos;
		GEPOINT vFlowDirect,vVertDirect;
		double fSymbolFlowAngle;	//������
		int hAcadMode,vAcadMode;	//ACAD��עʱ���ı����뷽ʽ
		SECTSYMBOLDIM();
	};
	static bool CalSectViewSymbolDimPos(CTextOnlyDrawing* pSectSymbol,
		f3dPoint rgnVert[4],double fSymbolFontSize=2.0,SECTSYMBOLDIM* pxDimPos=NULL);

	MASKFLAG drawStyleFlag;	//�麸ͼ���Ʒ��ѡ��
	struct FL_SECT_AXIS_OFFSET{
		short siMinFilterZ,siMaxFilterZ;	//���Ƹְ����ͼʱ�������Z���꣨��ͼ����ϵ�£���Χ wjh-2018.3.10
		short siDistToUpper,siDistToBottom;
		GEPOINT vxTubeStdUpperAxisVec;		//ģ������ϵ�µĸֹܵ�λ�����򣨳���ǰ��ͼZ������һ�ࣩ
		FL_SECT_AXIS_OFFSET(){siMinFilterZ=siMaxFilterZ=siDistToUpper=siDistToBottom=0;}
	}xFlSectNormalOffset;

	READONLY_PROPERTY(CPartGroupDrawing*,m_pPartGroupDrawing);
	GET(m_pPartGroupDrawing){return (CPartGroupDrawing*)m_pParentDrawing;}
	//CPartGroupDrawing *m_pPartGroupDrawing;	//�����ڳ��ȷ���ѹ����
	READONLY_PROPERTY(CLDSPart*,m_pRootPart);
	GET(m_pRootPart){return _pRootPart;}//CLDSPart *m_pRootPart;

	CSectionViewDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionViewDrawing();
	virtual UCS_STRU UpdateDCS();	//��������ͼ���ɸ�����ͼ�Ļ�ͼ����ϵ
	BOOL GetDCS(long hPart,CDrawCoordTrans &dcs);
	virtual void FinalDrawing();
	virtual void ZoomDrawing(double fZoomCoef=0);	//0��ʾ����ԭ��ͼ�������䣬�����ʾ����״���б������Ż��� wjh-2018.3.7
	//��ӽǶȱ�ע�� ��ע�뾶Ĭ��Ϊ100 ��Ҫ������ʾ����
	void AppendAngularDim(f3dPoint vertex,f3dPoint start_vec,f3dPoint end_vec,double fDimR=0,char *dimText=NULL);
	bool CheckSameAngularDim(double* vertex,double* start_vec,double* end_vec);
	//���ƹ�������
	/*static*/ void CreatePartSketch(CSectionView::PART_ITEM* pPartItem,BOOL bCoplanar=TRUE);
	//���ƿ�ģ�Ǹֵļ�����ͼ
	BOOL IsPartGroupHasKmAngleView();
	void CreateKaoMeAngleSketch(CKaoMeAngleSectionView* pKmAngleView);
	//���Ƹֹ����Ź�����ͼ wht 18-08-24
	// 1>. �����߰�ͼ
	// 2>. �������Ӱ���װ���˿�Ϸ�
	void CompleteTubeTowerFootViewSketch();
	//���ƺ�����
	void DrawWeldLineSymbol(f3dPoint start,f3dPoint end,double width=0,double height=0);
	void DrawWeldLineSymbol(f3dArcLine arcline,double width=0,double height=0);
	double TransToModelLength(double scr_draw_length){return scr_draw_length*m_fUser2ScrDrawScale;}
	virtual double ZoomToModelTextSize(double fSysFontSize);	//fSysFontSize��ϵͳ�趨������߶ȣ�����ֵ�ǿ��ǵ���ͼ�������ģ�ͳߴ緶�������߶ȡ�wjh-2017.11.9
	virtual FRAMESIZE ZoomToModelTextSize(FRAMESIZE& framesize);	//fSysFontSize��ϵͳ�趨������߶ȣ�����ֵ�ǿ��ǵ���ͼ�������ģ�ͳߴ緶�������߶ȡ�wjh-2017.11.9
	virtual BOOL CreateSectionViewDrawing(CSectionView *pSectionView,IModel *pTower,CLDSPartGroup *pPartGroup,bool bFinalDrawing=true);		//������ͼ��ͼ
	BOOL CreateSphereSectionViewDrawing(CSectionView* pSectionVIew,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);		//������ڵ��麸����ͼ��ͼ
	virtual f2dRect GetDrawRegion(bool bIncSectSymbol=false);	//�����Ƿ��������ͼ�ײ���ͼ�������� wjh-2018.8.1
	double GetInternalDrawingScale() const { return m_fUser2ScrDrawScale; }
	__declspec( property(get=GetInternalDrawingScale)) double dfScaleOfModel2Draw;
	CSectionView* GetSectionView(){return m_pSectionView;}
	__declspec( property(get=GetSectionView)) CSectionView* pSectionView;
	UCS_STRU GetCS() {return cs;}	//���ػ�ͼ����ϵ

	void AppendAngularDim(CSizeAngleDim& angularDim){angularDimList.append(angularDim);}
	void AppendLine(f3dLine& line);
	void AppendLine(f3dArcLine& arcline){edgeList.append(arcline);}
	void AppendCircle(f3dCircle& circle){cirList.append(circle);}
	void AppendSizeTextDim(CSizeTextDim& sizeTextDim){spaceDimList.append(sizeTextDim);}
	//��ӳߴ��ע
	CSizeTextDim* AppendSizeTextDim(const double* dimStartCoords,const double* dimEndCoords,const double* dimPosCoords,
		double fDist,double angle,BOOL bAllowRelayout=FALSE,BOOL bCanTextOutter=FALSE){
		AppendSizeTextDim(CSizeTextDim(dimStartCoords,dimEndCoords,dimPosCoords,fDist,angle,bAllowRelayout,bCanTextOutter));
		return spaceDimList.GetCursor();
	}
	void AppendTextDim(CTextOnlyDrawing& pureTextDim){pureTextDimList.append(pureTextDim);}
	void AppendDataPoint(CTextFrame& dataPoint){dataPointList.append(dataPoint);}
	DATAPOINT_TAG* AppendDataPoineTag(){
		DATAPOINT_TAG* pTag=m_xDataPointTagHash.Add(0);
		pTag->key_tag=m_xDataPointTagHash.GetCursorKey();
		return pTag;
	}
	ATOM_LIST<f3dCircle> *GetLsCirList() {return &cirList;}
	ATOM_LIST<f3dArcLine> *GetEdgeLineList() {return &edgeList;}
	ATOM_LIST<ACAD_HATCH> *GetHatchList() {return &hatchList;}
	ATOM_LIST<CSizeTextDim> *GetSpaceDimList() {return &spaceDimList;}
	ATOM_LIST<CTextOnlyDrawing> *GetPureTextDimList() {return &pureTextDimList;}
	ATOM_LIST<CTextFrame> *GetDataPointDimList() {return &dataPointList;}
	ATOM_LIST<CSizeAngleDim> *GetAngularDimList() {return &angularDimList;}
public:
	struct TUBE_BASELINE{
		BYTE m_iDraw;	//0.������ 1.���Ǽ�ͷ 2.��׼��ͷ
		double m_fArrowH;	//��ͷ����
		f3dPoint m_xArrowBtm;
		f3dPoint m_xArrowHead;
		f3dPoint m_xArrowStdVec;
		f3dPoint m_xTaCoreVec;	//���ĳ���
		f3dPoint m_xTaCoreVecHead;
		f3dPoint m_xTaCoreVecBtm;
		CLDSLineTube* m_pBaseTube;
		TUBE_BASELINE(){m_iDraw=1;m_fArrowH=20;m_pBaseTube=NULL;}
	}tube_base_line;
protected:
	void CreateTubeFeatureLine(SCOPE_STRU& region);
	void CreateDimAngleFeatureLine(SCOPE_STRU& region);
	GEPOINT GetFeatureLineTipPoint(const double* vRaylineVec,bool bAlongVec=true);
	bool CreateProcessAidedPlate(SCOPE_STRU& region);	//���ɸ�����װ����
public:
	static const int CLS_BASIC_VIEW = 0;
	static const int CLS_SLAVE_VIEW = 1;
	static const int CLS_FL_VIEW	= 2;
	static const int CLS_BOM_VIEW   = 3;
	static const int CLS_JOINT_VIEW = 4;
	static const int CLS_STDX_PLATES_VIEW = 5;
	static const int CLS_ANGLE_FOOT_VIEW = 6;
	static const int CLS_KM_ANGLE_VIEW = 7;
	static const int CLS_ANGLE_SPREAD_VIEW = 8;
	static const int CLS_BRIEFTUBE_VIEW = 9;	//���˲������ĸֹܱ�׼��ͼ
	static const int CLS_TUBE_ENDPLATE_VIEW = 10;	//�ֹܶ˰���ͼ
};
class CSectionBriefTubeDrawing : public CSectionViewDrawing
{
	CLDSGeneralPlate *pStartPlate,*pEndPlate;
public:
	CSectionBriefTubeDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionBriefTubeDrawing(){;}
	virtual BOOL CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);	//������ͼ��ͼ
};
class CSectionSlaveDrawing : public CSectionViewDrawing
{
public:
	CSectionSlaveDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionSlaveDrawing(){;}
	virtual UCS_STRU UpdateDCS();	//��������ͼ���ɸ�����ͼ�Ļ�ͼ����ϵ
};
class CSectionFlDrawing : public CSectionSlaveDrawing
{
	CLDSLineTube* m_pDrawTube;
	CLDSParamPlate* m_pDrawFL;
public:
	CSectionFlDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionFlDrawing(){;}
	virtual BOOL CreateSectionViewDrawing(CSectionView *pSectionView,IModel *pTower,CLDSPartGroup *pPartGroup,bool bFinalDrawing=true);	//������ͼ��ͼ
	virtual void FinalDrawing();
};
class CSectionJointDrawing : public CSectionViewDrawing
{
	GEPOINT m_vAngularJointPlaneNorm;
	CLDSLineTube* m_pRootTube,*m_pJointTube;
public:
	CSectionJointDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionJointDrawing(){;}
	READONLY_PROPERTY(CLDSLineTube*,pRootTube);
	GET(pRootTube){return m_pRootTube;}
	READONLY_PROPERTY(CLDSLineTube*,pJointTube);
	GET(pJointTube){return m_pJointTube;}
	virtual BOOL CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);	//������ͼ��ͼ
	virtual void FinalDrawing();
};
class CSectionStdXPlatesDrawing : public CSectionViewDrawing
{
	STDX_PLATES_PARTGROUP m_xParam;
	BOOL CreateFrontViewDrawing();
	BOOL CreateSideViewDrawing();
	BOOL CreateTopViewDrawing();
	void DimViewHorizontalLsSpace(CDrawCoordTrans &dcs,BOOL bFrontView,const double* pxRayOrg);
	void DimViewVerticalLsSpace(CDrawCoordTrans &dcs,BOOL bFrontView,const double* pxRayOrg);
	void DrawLineAngleSection(f3dPoint ber_pos,f3dPoint x_vec_wing_pos,f3dPoint y_vec_wing_pos,double thick);
	void DrawSectionViewSymbol(CDrawCoordTrans &dcs);
public:
	static const BYTE WELDLINE_FRONT_ZZ = 0x01;		//ǰ������Z���������к���
	static const BYTE WELDLINE_FRONT_ZF = 0x02;		//ǰ������Z�Ḻ�����к���
	static const BYTE WELDLINE_BACK_ZZ  = 0x04;		//�������Z���������к���
	static const BYTE WELDLINE_BACK_ZF  = 0x08;		//�������Z�Ḻ�����к���
	BYTE m_cWeldLinePos;
	BYTE m_cKmAngle1Pos,m_cKmAngle2Pos;				//��¼��Ҫ���ƿ�ģ�Ǹֵ�λ��
public:
	CSectionStdXPlatesDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionStdXPlatesDrawing(){;}
	//��ȡʮ�ֺ��Ӱ崦���������ĽǺ����¿�����,��Ҫ���ڽ���ʮ�ֺ��ӽ���ͼ�¿ڱ�ע��wjh-2015.4.24
	static BYTE InitWeldLinePos(CLDSGeneralPlate* platePtrArr[3],CLDSLineAngle* arrAnglePtr[4],BYTE *pcKmAngle1Pos,BYTE *pcKmAngle2Pos);
	static void DimPlateLsSpace(CDrawCoordTrans &dcs,CLDSGeneralPlate *pPlate,ATOM_LIST<CSizeTextDim> &spaceDimList);
	virtual BOOL CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);	//������ͼ��ͼ
	virtual void FinalDrawing();
};

class CSectionAngleFootDrawing : public CSectionViewDrawing
{
	ANGLE_FOOT_PARTGROUP m_xParam,m_xDownParam;
	BOOL CreateFrontViewDrawing();
	BOOL CreateSideViewDrawing();
	BOOL CreateTopViewDrawing();
	void DimFootPlateLsSpaceAndShoePlatePos(f3dPoint foot_center,GEPOINT plateDimPosArr[4]);
	BOOL DrawVerticalShoePlate(CLDSGeneralPlate *pPlate,f3dLine inters_line,f3dPoint base_plate_center,
							   CXhPtrSet<ACAD_HATCH> &hatchSet,GEPOINT &key_pt_for_dim);
	void DrawSectionViewSymbol(CDrawCoordTrans &dcs);
public:
	static const BYTE WELDLINE_FRONT_ZZ = 0x01;		//ǰ������Z���������к���
	static const BYTE WELDLINE_FRONT_ZF = 0x02;		//ǰ������Z�Ḻ�����к���
	static const BYTE WELDLINE_BACK_ZZ  = 0x04;		//�������Z���������к���
	static const BYTE WELDLINE_BACK_ZF  = 0x08;		//�������Z�Ḻ�����к���
	BYTE m_cWeldLinePos;
	BYTE m_cKmAngle1Pos,m_cKmAngle2Pos;				//��¼��Ҫ���ƿ�ģ�Ǹֵ�λ��
public:
	CSectionAngleFootDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionAngleFootDrawing(){;}
	bool IsHoofViewDrawing(){return m_xDownParam.m_pFootPlate!=NULL;}
	virtual BOOL CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);	//������ͼ��ͼ
	virtual void FinalDrawing();
};
typedef struct tagSPHEREPOINT
{
	long hTube;					//�ֹܾ��
	CXhChar16 sSonTag;			//��������ͼ
	SPHERE_COORD  intersPt1;	//����1
	SPHERE_COORD  intersPt2;	//����2
	double fEdgeDist;			//���ĵ�������������

}SPHERE_POINT;
class CPartGroupTblDrawing : public CSectionViewDrawing
{
public:
	static const int PARTGROUP_BOM		= 1;	//�麸����ϸ��
	static const int SPHEREINTERS_TBL	= 2;	//װ�����Ϣ��
public:
	int m_nTableType;
	CFormatTbl m_xPartTbl;
	CHashList<SPHERE_POINT> m_pointInfoHash;
protected:
	void InitSpherePartGroupInsters();
	void CreatePartGroupBom(double scale);		//�����麸����ϸ��
	void CreateSphereIntersTbl(double scale);	//������ڵ��麸��װ�����Ϣ��
public:
	CPartGroupTblDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	~CPartGroupTblDrawing();
	void CreatePartGroupTable(double scale);
	double GetTblWidth(double zoomscale=1.0);
	f2dRect GetDrawRegion(bool bIncSectSymbol=false);
};
class CSectionKmAngleViewDrawing : public CSectionViewDrawing
{
public:
	CKaoMeAngle m_xKaoMeAngle;
	static void InitKaoMeAngle(CKaoMeAngle &kaoMeAngle,CLDSLineAngle *pAngle,long* boltHandleArr,int nBoltCount,CLDSLineAngle *pCoupleAngle=NULL);
public:
	CSectionKmAngleViewDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionKmAngleViewDrawing(){;}
	f2dRect GetDrawRegion(bool bIncSectSymbol=false);
	virtual BOOL CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);	//������ͼ��ͼ
};
class CSectionAngleViewDrawing : public CSectionViewDrawing
{
public:
	CLineAngleDrawing m_xAngleDrawing;
public:
	CSectionAngleViewDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionAngleViewDrawing(){;}
	f2dRect GetDrawRegion(bool bIncSectSymbol=false);
	virtual BOOL CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);	//������ͼ��ͼ
};
class CSectionTubeEndPlateViewDrawing : public CSectionViewDrawing
{
	CPlateDrawer plateDrawer;
	CSlaveSectionView m_tempView;
public:
	CLDSParamPlate *m_pParamPlate;
	CSectionTubeEndPlateViewDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionTubeEndPlateViewDrawing(){;}
	f2dRect GetDrawRegion(bool bIncSectSymbol=false);
	virtual BOOL CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);	//������ͼ��ͼ
};
class CPartGroupDrawing : public CXeroDrawing
{
	friend class CPartGroupRectLayout;
	friend class CPartGroupDrawingLayoutOper;

	IModel *m_pTower;
	CLDSPartGroup *m_pPartGroup;
	CLDSPart *m_pRootPart;
	PARTSET SonParts;
	ATOM_LIST<CAttachPartsSection> sectionList;
	UCS_STRU cs;	//drawing coordinate system ��ͼ����ϵ
	double AnalyzeLenModulate();
	//�麸�������Զ��������Ȼ���ʱ�ĳߴ绻��
	//fromReal=TRUE:����ʵ�ʳ���,�����ʾ����
	//fromReal=FALSE:������ʾ����,���ʵ�ʳ���
	double LenCompressTrans(double len,BOOL fromReal=TRUE,double zoomThreshold=0.0);
	double m_fMaxDrawingLength;
	double m_fMaxDrawingHeight;
protected:
	CDrawSysPara::WELDDRAWING xDrawSet;
	bool m_bAutoLayoutViewIgnoreScale;//������ָ����ͼ�����Զ����ָ��麸����ͼ(�������¼����ͼ����)
	double m_fRealToDraw;//������ͼ(Y�����ְ�)�����ߣ�ʵ�ʳߴ�/��ͼ�ߴ磬��1:20ʱ��fRealToDraw=20
	double m_fSectViewScaleRealToDraw;
	double m_fMajorRodViewScaleRealToDraw;
	void IntelliCalDrawScale(CHashList<double>* pHashScaleOfView);	//���ܼ�������ͼ���˼�����ͼ�Ļ�ͼ����
	void InitDrawSetting();
	struct RESETSYSPARA_LIFEOBJ{
		RESETSYSPARA_LIFEOBJ(int mode=0);
		~RESETSYSPARA_LIFEOBJ();
	protected:
		bool bDrawWeldLineSymbol;
		bool bZoomFontSizeWithDrawScale;
	};
public:
	double get_fRealToDraw(){return m_fRealToDraw;}
	__declspec(property(get=get_fRealToDraw)) double scaleOfRealToDraw;
	CHashStrList<PART_DIMnDRAW*> hashDimLabels;
	PART_DIMnDRAW* RegisterPartDimDraw(long idView,long hPart);
	bool StatSectionViewDimDraws(CSectionView* pView);
	PART_DIMnDRAW* GetPartDimDraw(long idView,long hPart){return hashDimDrawParts.GetValue(idView,hPart);}
	//double AutoCalUser2ScrDrawScale();
	double GetMaxDrawingLength() { return m_fMaxDrawingLength; }
	double GetMaxDrawingHeight() { return m_fMaxDrawingHeight; }
private:
	CMapList<PART_DIMnDRAW> hashDimDrawParts;
public:
	MASKFLAG drawStyleFlag;	//�麸ͼ���Ʒ��ѡ��
	static DWORD CPartGroupDrawing::DimPartNo;				//�Զ���ע�������
	static DWORD CPartGroupDrawing::LayoutViews;			//�Զ�����麸����ͼ
	static DWORD CPartGroupDrawing::DimAbsoluteDist;		//��ע��λ����Գߴ�
	static DWORD CPartGroupDrawing::DimPlateBetweenAngle;	//��ע�Ƕ���
	static DWORD CPartGroupDrawing::DimWeldPosLocateLine;	//��ע����ǰ��λ��
	static DWORD CPartGroupDrawing::DimWeldPosCheckLine;	//��ע���Ӻ�����
	static DWORD CPartGroupDrawing::DimPlateRayLineAngle;	//��ע�ְ������Լ��������׼��֮��ļн�
	static DWORD CPartGroupDrawing::ModulatePartGroupLen;	//�Զ�ѹ���麸ͼ����
	static DWORD CPartGroupDrawing::CreatePartGroupBOM;		//���ɹ�����ϸ��
	static DWORD CPartGroupDrawing::DispPartGroupNo;		//�麸�����
	static DWORD CPartGroupDrawing::DispPartGroupNum;		//�麸������
	static DWORD CPartGroupDrawing::DispPartNo;				//�������
	static DWORD CPartGroupDrawing::DispPartNum;			//��������
	static DWORD CPartGroupDrawing::DispPartGuiGe;			//�������
	static DWORD CPartGroupDrawing::DispPartLen;			//��������
	static DWORD CPartGroupDrawing::DispPartWeight;			//���������Լ�ͬһ��Ź�������С�� ����
	static DWORD CPartGroupDrawing::DispNotes;				//��ע
	static DWORD CPartGroupDrawing::DispInternal;			//���켶������
	static DWORD CPartGroupDrawing::DispAppearance;			//���켶�����
public:
	//ATOM_LIST<CSectionViewDrawing> viewDrawingList;
	CPartGroupDrawing(void);
	~CPartGroupDrawing(void);
	UCS_STRU GetCS() {return cs;}	//���ػ�ͼ����ϵ
	double TransCoordX(double real_x);
	bool SelectFootPartGroupView(CSectionView *pSectionView);	//�������Ű��麸ͼ wht 10-01-14
	CSectionTubeEndPlateViewDrawing* CreateTubeEndPlateViewDrawing(CLDSParamPlate *pPlate,double scaleOfReal2Draw=0);
	CSectionViewDrawing* CreateSectionViewDrawing(CSectionView *pSectionView,double scaleOfReal2Draw=0);
	bool CreatePartGroupDrawing();
	f2dRect GetDrawRegion(double PART_MAP_SCALE=1.0);
	bool InitPartGroupDrawing(CLDSPartGroup *pPartGroup,double maxDrawingLength,double maxDrawingHeight);
	int Relayout(ARRAY_LIST<double> &zoomdraw_coef_arr);
	//����ֹܶ�ͷ������ͼ
	BYTE InitEndInsertPlateDrawing(CSectionTubeEndPlateViewDrawing *pStartDrawing,CSectionTubeEndPlateViewDrawing *pEndDrawing);
};
#include "LifeObj.h"
struct TAG_INFO_PART_DRAWCMD_TUBE_SECT;
struct TAG_INFO_PART_DRAWCMD_FL_FLAT;
class CStruDrawingCommand;
struct TAG_INFO_PART_DRAWCMD : public TAG_INFO<CStruDrawingCommand>{
public:
	long hPart;
	TAG_INFO_PART_DRAWCMD(){hPart=0;}
	virtual ~TAG_INFO_PART_DRAWCMD(){DetachObject();}
	//��ǩ��������ת��
	TAG_INFO_PART_DRAWCMD_FL_FLAT* FlatFL();
	TAG_INFO_PART_DRAWCMD_TUBE_SECT* TubeSect();
public:
	static const BYTE PART_TUBE_SECT= 1;
	static const BYTE PART_FL_FLAT	= 2;
};
struct TAG_INFO_PART_DRAWCMD_TUBE_SECT : public TAG_INFO_PART_DRAWCMD
{
	bool bDrawExterCircle;	//��Ҫ���Ƹֹ����Բ
	TAG_INFO_PART_DRAWCMD_TUBE_SECT(){
		m_iTagType=TAG_INFO_PART_DRAWCMD::PART_TUBE_SECT;
		bDrawExterCircle=true;
	}
};
struct TAG_INFO_PART_DRAWCMD_FL_FLAT : public TAG_INFO_PART_DRAWCMD
{
	bool bDrawInnerCircle;	//��Ҫ���Ʒ�����Բ��
	TAG_INFO_PART_DRAWCMD_FL_FLAT(){
		m_iTagType=TAG_INFO_PART_DRAWCMD::PART_FL_FLAT;
		bDrawInnerCircle=true;
	}
};
class CStruDrawingCommand : public CXeroDrawingCommand
{
protected:
	void *pTagInfo; //��ʱʹ�ò��洢���ɸ���һЩ���ͼ����ִ�й�����ص���ʱ����
public:
	CLDSPart* m_pPart;
	CStruDrawingCommand(int id=0,DrawingCommandContext* pContext=NULL);
	virtual ~CStruDrawingCommand(){;}
	void SetTagInfo(void* pTag){pTagInfo=pTag;}
	TAG_INFO_PART_DRAWCMD* TagInfo(){return (TAG_INFO_PART_DRAWCMD*)pTagInfo;}
	//BOOL GetDCS(CDrawCoordTrans &dcs);
	virtual void Redraw();
public:
	static const int CLS_BASIC_CMD = 0;
	static const int CLS_PART_GENERAL_DRAW_CMD = 1;	//������ͨƽ�̽ṹ��ͼ����
	static const int CLS_PART_UPRIGHT_DRAW_CMD = 2;	//������������ṹ��ͼ����
	static const int CLS_JOINT_TUBE_DIM_CMD	   = 3;	//�����ͼ�ߴ���Ƕȵı�ע
};
class CFLDrawing{
public:
	struct FL_DRAWING_STYLE{
		bool bDrawInnerWeldCircle;	//=true������ֹܺ���Բ����
		char ciDrawModeH0V1;
		short penStyle;	//��ӦԭbCoplanar=TRUEʱ������PS_DASH����
		FL_DRAWING_STYLE(){bDrawInnerWeldCircle=true;ciDrawModeH0V1=0;penStyle=PS_SOLID;}
	}style;

	void DrawFLStruDrawing(CLDSParamPlate* pFL,CDrawCoordTrans& dcs,IDbEntsContext* pContext);
};
//���ƹ�����������ͼ{�ְ�ƽ����ͼ���˼�ƽ����ͼ}�Ĺ����ṹͼ
class CPartGeneralDrawingCommand : public CStruDrawingCommand
{
protected:
public:
	//CLDSPart* m_pPart;
	CPartGeneralDrawingCommand(int id=0,DrawingCommandContext* pContext=NULL);
	virtual ~CPartGeneralDrawingCommand(){;}
	virtual void Redraw();
};
//���ƹ�����������ͼ{�ְ�ƽ����ͼ���˼�ƽ����ͼ}�Ĺ����ṹͼ
class CPartUprightDrawingCommand : public CStruDrawingCommand
{
protected:
public:
	//CLDSPart* m_pPart;
	CPartUprightDrawingCommand(int id=0,DrawingCommandContext* pContext=NULL);
	virtual ~CPartUprightDrawingCommand(){;}
	virtual void Redraw();
	void DrawTubeStruDrawing();
};
class CJointTubeDimDrawingCommand : public CStruDrawingCommand
{
protected:
public:
	CJointTubeDimDrawingCommand(int id=0,DrawingCommandContext* pContext=NULL);
	virtual ~CJointTubeDimDrawingCommand(){;}
	virtual void Redraw();
	void CreateJointTubeDims();
};
#endif
#endif
