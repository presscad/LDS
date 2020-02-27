// PlankDrawing.h: interface for the CPlankDrawing class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLANKDRAWING_H__A1203127_5F46_4E86_95F5_4F3EEA09FFE5__INCLUDED_)
#define AFX_PLANKDRAWING_H__A1203127_5F46_4E86_95F5_4F3EEA09FFE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Tower.h"
#include "XhCharString.h"
#include "LocalDrawing.h"
#include "PartGroupDrawing.h"

#ifndef __TSA_FILE_
#define PS_WELDLINE		7
#define PS_TUBEBREAK	8
#define PS_DAMAGELINE	9
#define PS_OTHERLINE	10	//TODO:����ע��,����˭��֪����������Ǹ����? wjh-2017.1.11

typedef struct tagALIGNDIM_STRU
{
	f3dPoint startPos;
	f3dPoint endPos;
	f3dPoint dimPos;
	double dist;
	int nLsSpaceNum;
	tagALIGNDIM_STRU(){dist=0;nLsSpaceNum=1;}
}ALIGNDIM_STRU;
struct ANGULARDIM_STRU
{
	f3dPoint vertex;
	f3dPoint startPos;
	f3dPoint endPos;
	f3dPoint dimPos;
	double angle;
	double fRadius;
	CXhChar16 sDimText;
	ANGULARDIM_STRU(){angle=0;fRadius=0;sDimText.Copy("<>");}
};
typedef struct tagROTATEDIM_STRU
{
	static double PRECISION;		//�������־����עʱ�ľ���,ֻȡ1.0,0.5,0.1����ֵ
	f3dPoint startPos;
	f3dPoint endPos;
	f3dPoint dimPos;
	double angle;
	double dist;
	CXhChar16 sDimText;		//���ñ�ע����ʱdist������Ч
	BOOL bCanTextOutter;
	BOOL bDiameterStyle;
	BOOL bDisplayDimLine;	//�Ƿ���ʾ�ߴ���
	int nHits;				//�����ĵȾ�ΪS�ı�ע�����Ժϲ�Ϊһ��S X nHits�ı�ע
	CXhChar16 DimText();
	tagROTATEDIM_STRU(){memset(this,0,sizeof(tagROTATEDIM_STRU));}
}ROTATEDIM_STRU;
struct RAYROD_AXIS
{
	bool bDimAxisLine;			//��Ҫ��ע��������(�Ǹ�����)
	bool bDimAxisEndPerpLine;	//��Ҫ��ע�����ߴ�ֱ�Ķ���ͷ����
	bool bDatumRod;
	UINT hRod;
	UINT nSubAxisCount;	//�ø˼���ͳ�ܵ���������Ŀ(����1ʱ,����ƶ�������ͼ����ע)
	CDesignLjPartPara* pLjPart;
	BYTE ciWorkWingX0Y1;
	int start_end,iFaceNo;
	GEPOINT org;
	GEPOINT axis_vec,wing_vec;
	double dfOrgTipExtend;	//���߸˼���ǰ���Ӷ˴Ӷ�λ�ˣ����������������� wjh-2019.5.19
	double ber_space,wing_space;	//��׼���ߵ����ߡ�֫���ߵ�ƽ�о���
	double ray_dist,ber_dist,wing_dist;		//����ͷ���ߵ����߲༰֫����������
	double fAngle,fRadius;
	void SetKey(DWORD key){hRod=key;}
	RAYROD_AXIS(){
		bDatumRod=false;
		iFaceNo=1;
		ciWorkWingX0Y1=0;
		start_end=0;
		pLjPart=NULL;
		nSubAxisCount=0;bDimAxisLine=bDimAxisEndPerpLine=true;
		dfOrgTipExtend=ber_space=wing_space=ber_dist=wing_dist=fAngle=fRadius=0;
		ray_dist=100000;//Ĭ�ϸ�һ��ֵ
	}
};
typedef struct tagLSRAY_STRU
{
	int iRayNo,featrue;
	GEPOINT rayOrg;			//���ߵ���ʼ��
	GEPOINT rayResideVec;	//��������פ�湹�������߷���(����˨���޹�,һ��Ϊ������˼������߷���) wjh-2015.2.27
	fPtList ls_arr;
	//bIsDoubleLsRayĿǰ����CDimPlankLs������,δ��Ӧͳһ��CPlankDrawing��ȥ�� wjh-2015.2.27
	BOOL bIsDoubleLsRay;//�Ƿ�Ϊ˫����˨���� wht 11-06-30
	BOOL bDatumLsRay;	//�Ƿ�Ϊ��׼��˨����
	tagLSRAY_STRU(){featrue=0;bIsDoubleLsRay=FALSE;bDatumLsRay=FALSE;}
	void CloneMember(tagLSRAY_STRU* pSrcLsRay,BOOL bCopyLsArr=TRUE)
	{
		iRayNo=pSrcLsRay->iRayNo;
		rayOrg=pSrcLsRay->rayOrg;
		rayResideVec=pSrcLsRay->rayResideVec;
		bIsDoubleLsRay=pSrcLsRay->bIsDoubleLsRay;
		bDatumLsRay=pSrcLsRay->bDatumLsRay;
		if(bCopyLsArr)
		{
			for(int i=0;i<pSrcLsRay->ls_arr.GetNodeNum();i++)
				ls_arr.append(pSrcLsRay->ls_arr[i]);
		}
	}
}LSRAY_STRU;
struct TXTDIM_STRU
{
	f3dPoint dimPos;
	f3dPoint dimVec,dimPtS,dimPtE;	//���Ǳ�����
	double txtHeight;
	AcDb::TextHorzMode horzAlignMode;	//ˮƽ����ģʽ
	AcDb::TextVertMode vertAlignMode;	//��ֱ����ģʽ
	CXhChar100 dimText;	
	TXTDIM_STRU(){
		txtHeight=0;	//��ʾϵͳĬ������ֵ
		horzAlignMode=AcDb::kTextLeft;
		vertAlignMode=AcDb::kTextBottom;
	}
};
struct PT_ITEM{
	f3dPoint origPt,extendPt;
	int nNum;
	PT_ITEM(){nNum=0;}
};
class CPlankDrawing;
class CWeldPlateDrawing : public CXeroDrawing
{
private:
	CPlankDrawing* m_pBelongDrawing;
	CLDSPart* m_pPlate;
	CDrawingEntsContext m_xEntContext;
public:
	TXTDIM_STRU m_xTxtDim;
	ROTATEDIM_STRU m_xRotateDim;
	double m_fWeldThick;
	double m_fWeldLen;
	double m_fTxtOff;
	BOOL m_bFrontWeld;
	f3dPoint topLeft,topRight,btmLeft,btmRight;
public:
	CWeldPlateDrawing();
	CWeldPlateDrawing(CPlankDrawing* pParentDrawing);
	~CWeldPlateDrawing();
	//
	void InitWeldDrawing(CPlankDrawing* pParentDrawing=NULL);
	void ClipLine(f3dLine &line);
	void PerfectWeldDrawing();
	BOOL CreatePlateSideEdge(CLDSGeneralPlate *pSubPlate,long hDatumPlate,UCS_STRU& draw_ucs);	
	//BOOL CreateParaPlateSideEdge(CLDSParamPlate* pSubParamPlate,long hDatumPlate,UCS_STRU& draw_ucs);
	BOOL IsOverlap(CWeldPlateDrawing& weld_drawing);
	void CopyMember(CWeldPlateDrawing& weld_drawing);
};
typedef struct tagDIAMETERDIM_STRU
{
	f3dPoint chordPos;
	f3dPoint farChordPos;	//��ע�뾶ʱ��ʾԲ��
	double leaderLength;
	double dist;			//��ע�뾶ʱ��ʾ�뾶
	BOOL bDimRadial;		//TRUE:��ע���ǰ뾶;FALSE:��ע����ֱ��
	tagDIAMETERDIM_STRU(){memset(this,0,sizeof(tagDIAMETERDIM_STRU));}
}DIAMETERDIM_STRU;
typedef struct tagPART_PLANK_CARD_INFO
{
	CXhChar16 sSeg;				//�κ�
	CXhChar16 sPartNo;			//����
	CXhChar16 sLength;			//����
	CXhChar16 sThick;			//���
	CXhChar16 sDesMaterial;		//��Ʋ���
	CXhChar16 sReplMaterial;	//���ò���
	CXhChar16 sGuiGeIncMat;		//����������
	CXhChar16 sDesGuiGe;		//��ƹ��
	CXhChar16 sReplGuiGe;		//���ù��
	CXhChar16 sPartNum;			//������
	CXhChar16 sSumPartNum;		//�ܼӹ���=������*����
	CXhChar16 sPartWeight;		//����
	CXhChar16 sSumPartWeight;	//����
	CXhChar16 sM12HoleNum;		//M12��˨����
	CXhChar16 sM16HoleNum;		//M16��˨����
	CXhChar16 sM18HoleNum;		//M18��˨����
	CXhChar16 sM20HoleNum;		//M20��˨����
	CXhChar16 sM22HoleNum;		//m22��˨����
	CXhChar16 sM24HoleNum;		//M24��˨����
	CXhChar16 sMXHoleNum;		//MX��˨����
	CXhChar16 sSumHoleNum;		//�ܿ���
	CXhChar16 sChongKongNum;	//�����
	CXhChar16 sZuanKongNum;		//�����
	CXhChar16 sCutEdge;			//����
	CXhChar16 sWrapEdge;		//���
	CXhChar16 sHuoQuFst;		//�״λ���
	CXhChar16 sHuoQuSec;		//���λ���
	CXhChar16 sMapScale;		//ͼ�����
	CXhChar16 sWeld;			//����
	CXhChar50 sNotes;			//��ע
	CXhChar16 sWeldLength;		//���쳤��
	CXhChar16 sEdgeNum;			//����
	CXhChar16 sArea;			//���
	CXhChar16 sFillet;			//�¿�
	CXhChar16 sCutArc;			//�л�
	int m_nPageNum,m_nPageIndex;//�� ҳ,�� ҳ
	int m_nTaNum;				//����	wht 09-12-31
	BOOL m_bPunchShear;			//���
	BOOL m_bGasCutting;			//����
	//
	tagPART_PLANK_CARD_INFO(){
		m_bPunchShear=FALSE;
		m_bGasCutting=FALSE;
	}
}PART_PLANK_CARD_INFO;
typedef struct{	int iRayNo,iPolarOrigin;}POLAR_MAP_STRU;
typedef struct{
	f3dLine line_arr[5];
	f3dPoint origin,offset_vec,prj_vec;
	double offset_dist;	//����ƫ�Ʒ���
	double plane_angle;
}HUOQU_MARK;

f3dArcLine CreateIsometricEdgeLine(PROFILE_VERTEX* pPrev,PROFILE_VERTEX* pCurrent,CLDSPart* pPart,UCS_STRU *dcs=NULL,double inner_offset=0);
class CPlankDrawing  : public CXeroDrawing
{
	CDrawingEntsContext m_xEntContext;
	CLDSPart *m_pPlate;		
	BOOL m_bSpecifyUCS;		//�Ƿ�Ϊ�ⲿָ������ϵ wht 10-08-13
	f3dPoint geom_center;	//�ְ弸������(��������)
	UCS_STRU new_ucs;		//��ע�������ʱ��Ҫ�����ְ�����ϵ��new_ucs������¼�����������ϵ
	f3dPoint polar_origin;
	POLYGON profileRegion;
	bool m_bPrevIsRollEdgeLine;
	GEPOINT m_xRollEdgeLineStart,m_xRollEdgeLineEnd;
	struct LSRAYLINE_PARA{
		bool reverseLenvec;
		long hRod;
		double coord_x,coord_y;
		LSRAYLINE_PARA(long rod=0,double coordx=0,double coordy=0)
		{
			reverseLenvec=false;
			hRod=rod;
			coord_x=coordx;
			coord_y=coordy;
		}
	};
	CArray<TXTDIM_STRU,TXTDIM_STRU&> text_dim_arr;
	CArray<ROTATEDIM_STRU,ROTATEDIM_STRU&>rotate_dim_arr;
	CArray<ANGULARDIM_STRU,ANGULARDIM_STRU&>angular_dim_arr;
	CArray<DIAMETERDIM_STRU,DIAMETERDIM_STRU&>diameter_dim_arr;
	CArray<f3dCircle,f3dCircle&>cir_arr;
	CArray<f3dArcLine,f3dArcLine&>edge_arr;
	CArray<LSRAY_STRU,LSRAY_STRU>ls_ray_arr;
	CArray<f3dLine,f3dLine>assist_ls_ray_arr;
	BOOL CreateParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL CreateUEndOrRollEndParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL CreateFLParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL CreateStdFLParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);	//���ɸ߾�����
	BOOL CreateElbowParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL CreateFootParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL CreatePedalParamPlate(CLDSParamPlate *pParamPlate, BOOL bPatternDrawing = TRUE);
	BOOL CreateCircularParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL CreateCircularParamPlate2(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL CreateRelativePlateInCirParamPlate(CLDSParamPlate *pPlate,BOOL bPatternDrawing,f3dPoint origin);
	BOOL CreateWaterShieldedPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL DrawPlateLocateFlag(CLDSPlate *pPlank,BOOL bAdjustPlateUcs);
	BOOL Create1FacePlank(CLDSPlate *pPlank,BOOL bPatternDrawing=TRUE);
	BOOL Create2FacePlank(CLDSPlate *pPlank,BOOL bPatternDrawing=TRUE);
	BOOL Create3FacePlank(CLDSPlate *pPlank,BOOL bPatternDrawing=TRUE);
	BOOL CreateFoldPlank(CLDSPlate *pPlank,BOOL bPatternDrawing=TRUE);
	BOOL CreatePlateInnerCir(CLDSPlate *pPlate,BOOL bPatternDrawing,BOOL bAdjustPlateUcs);	//���ɸְ���Բ��
	void DimCoordValue(CLDSPart *pPart,f3dPoint pos);	//��ע�������
	void DimPlateEdgeLen(f3dLine line);	//��ע��߳�
	void MergeParallelEdgeDim(int iStart);	//�ϲ����ߵı߳���ע��Ŀ
	void DimArcLineCoordValue(CLDSPart *pPart, f3dArcLine arcLine); //��Բ���ϱ�עnCount���������
	void ProcessPlateWeldEdge(PROFILE_VERTEX *pFeatureVertex,f3dArcLine &arcline);
	void DrawSlopeCutLine(f3dArcLine arcLine);		//�����¿���(���ӱ�ƽ����)
	void DimSlopeCutLineText(f3dArcLine arcLine);	//��ע�����ı�
	void CreateLineOnWeldCorner(CLDSGeneralPlate* pPart,ATOM_LIST<PROFILE_VERTEX> *pVertexList);//���������ǣ��нǻ��л���
	void DrawInnerEdge(CLDSGeneralPlate* pPart,ATOM_LIST<PROFILE_VERTEX> *pVertexList);	//�����ڲ�ƽ��������
	void DrawWeldSubPlate(CLDSGeneralPlate* pDatumPlate,BOOL bAdjustPlateUcs);	//���ƺ����ӹ���������
	void MarkHuoquLine(f3dLine line);
	//���ݾ�߸߶��Լ�������������ƾ������߶� wht 11-04-09
	void DrawRollEdgeLine(PROFILE_VERTEX *pStartVertex,PROFILE_VERTEX *pEndVertex,
						  PROFILE_VERTEX *pPreVertex,PROFILE_VERTEX *pNextVertex,
						  CLDSPlate *pPlate,f3dLine &roll_edge_line,f3dLine &huoqu_line,BOOL bPatternDrawing);
	//�ְ��ϵ���Բ������ݵײ����ײ�������зֶλ���
	void DrawPlateEllipseLine(CLDSGeneralPlate* pPlate,f3dArcLine arcline,BOOL bOriInBtmPlane=TRUE);
	//��ע����趨
public:
	static MASKFLAG PREFER_STYLEFLAG;
	MASKFLAG dimStyleFlag;
	static DWORD DimBoltSpaceToWeldEdge;	//0x00 0001 00:00000000 0000000000000001 00000000
	static DWORD DimHoleDiameterAlways;		//0x00 0002 00:00000000 0000000000000010 00000000
	static DWORD DimBoltSpaceToHuoQuLine;	//0x00 0004 00:00000000 0000000000000100 00000000
	static DWORD DimBoltSpaceToEdgeLine;	//0x00 0008 00:00000000 0000000000001000 00000000
	static DWORD DimBoltSpaceToNextLsRay;	//0x00 0010 00:00000000 0000000000010000 00000000
	static DWORD DimBoltAngle;				//0x00 0020 00:00000000 0000000000100000 00000000
public:
	BYTE m_ciHuoQuType;	//0.�޻���|1.����(����)|2.����(����)
	HUOQU_MARK huoqu_mark[2];
	f3dPoint long_edge_mid,huoqu_edge_mid[2];	//�ְ���ߵ����ļ������ߵ��е�
	double long_edge_angle;	//�ְ���ߵĽǶȣ�������ϸһ���ظְ���߷����ע
	f3dLine datumLine;		//�ְ�Ļ�׼�ߣ�һ����˨����Ӧ���������˻�׼�ߣ��԰��������洢��
public:
	f2dRect GetDrawRegion(double PART_MAP_SCALE);
	//��ע��˨���߼����ϵ���˨���
	void DimLsRay(CLDSPart *pPart=NULL,BOOL bAdjustPlateUcs=FALSE);
	//��ע����������˨���
	void DimWrapLsSpace(CLDSGeneralPlate* pGeneralPlate);
	//normΪ���Ӱ��������ϵ�µ�ʸ��
	void AddHuoQuPrjMap(f3dLine huoqu_line,HUOQU_MARK mark,char cMark='A');
	void AnalizeLsRay(f3dPoint ls_centre,int iRayNo,const double* resideRayVec=NULL);
	BOOL FindLsRayOrgAndDatumVec(f3dPoint &origin,f3dPoint &datum_vec);
	void CorrectLsRays();
	void CompleteLsRayInfo(CLDSGeneralPlate* pGeneralPlate,BOOL bAdjustPlateUcs=FALSE);
	LSRAY_STRU* FindLsRay(f3dPoint* lsPos1,f3dPoint* lsPos2);
	//��ע��������ʱ��������ϵ����Ҫ�����˼�����
	void AnalizePoleRay(CLDSPlate *pPlate);
	CPlankDrawing();
	virtual ~CPlankDrawing();
	BOOL Create(CLDSPart *pPlankTypePart,BOOL bPatternDrawing=TRUE);
	int GetLsCirCount();
	int GetLsRayCount();
	int GetAssistLsRayCount();
	int GetEdgeCount();
	int GetAlignDimCount();
	int GetAngularDimCount();
	int GetRotateDimCount();
	int GetDiameterDimCount();
	int GetTextDimCount();
	BOOL GetAngularDimAt(int index, ANGULARDIM_STRU &dim);
	BOOL GetRotateDimAt(int index, ROTATEDIM_STRU &dim);
	BOOL GetDiameterDimAt(int index, DIAMETERDIM_STRU&dim);
	BOOL GetTextDimAt(int index, TXTDIM_STRU&dim);
	BOOL GetEdgeAt(int index, f3dArcLine &line);
	BOOL GetLsCirAt(int index, f3dCircle &Cir);
	BOOL GetLsRayLineAt(int index, f3dLine &line,DWORD* pwDrawFlag=NULL);
	BOOL GetAssLsRayLineAt(int index, f3dLine &line);
	f3dPoint GetGeomCenter() {return geom_center;}
	f3dPoint Get2dGeomCenter();
	bool PtInPlateRgn(const double* poscoord);
	GEPOINT GetPlateRgnPt(int index){return profileRegion.GetVertexAt(index);}
	int GetPlateRgnPtNum(){return profileRegion.GetVertexCount();}
	UCS_STRU GetNewUcs() {return new_ucs;}
	//���û�ͼ����ϵ wht 10-08-13
	void SetNewUcs(CLDSPart *pPart,UCS_STRU plate_ucs);	
	//���Ҹְ�����˵����עλ��
	BOOL FindRectValidPos(f3dPoint &dimPos,CLDSPart *pPlate,double fWidth,double fHeight,double zoomCoef=1.0);
	CLDSPart *GetPlate(){return m_pPlate;}
};
#endif
#endif // !defined(AFX_PLANKDRAWING_H__A1203127_5F46_4E86_95F5_4F3EEA09FFE5__INCLUDED_)
