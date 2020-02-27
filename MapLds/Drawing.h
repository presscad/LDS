// Drawing.h: interface for the CDrawing class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWING_H__0EBE37F4_B612_4A73_9CB4_D45B171C1F29__INCLUDED_)
#define AFX_DRAWING_H__0EBE37F4_B612_4A73_9CB4_D45B171C1F29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "StdArx.h"
#include "Tower.h"
#include "HashTable.h"
#include "XhCharString.h"
#include "DrawUnit.h"
#include "FormatTable.h"
#include "DragEntSet.h"
#include "LocalDrawing.h"
#include "UnifyModelLayer.h"
#include "../IntelliAgent/IntelliCore/IPreview.h"

#ifdef __SMART_DRAW_INC_
//�嵥����
enum DRAW_BOM_TYPE{
	BOM_SUMPART		= 1,	//���ϻ��ܱ�
	BOM_SEGPART		= 2,	//�ֶι�����ϸ��
	BOM_SEGBOLT		= 3,	//�ֶ���˨��ϸ��
	BOM_PARTGROUP	= 4,	//�麸����ϸ��
	BOM_FL			= 5,	//������ϸ��
	BOM_GUIGE		= 6,	//�򻯹����ձ�
	BOM_JGZJ		= 7,	//�Ǹ�׼�ݱ�
	BOM_GENKAI		= 8,	//����������
};

//��׼��ͼ����
//TODO��Ŀǰ���ڹ�ϣ���ֵԭ��STD_VIEW_TYPE�����9��ʼ��δ��Ӧ��ϸ���ܹ����� wjh-2018.5.18
enum STD_VIEW_TYPE{
	STD_VIEW_FILLPLATE	= 9,	//���
	STD_VIEW_JOINT		=10,	//��ͷ
	STD_VIEW_FOOTNAIL	=11,	//�Ŷ�
};

//��ͼ��ͼ����
enum DRAW_COMPONENT_TYPE{
	DRAW_COM_LINEVIEW		= 1,	//����ͼ
	DRAW_COM_STRUVIEW		= 2,	//�ṹͼ
	DRAW_COM_STDVIEW		= 3,	//��׼ͼ
	DRAW_COM_PARTVIEW		= 4,	//���ͼ
	DRAW_COM_BOM			= 5,	//�����嵥
	DRAW_COM_OTHER			= 100,	//����
};

enum DRAW_CMD_TYPE{
	CMD_DRAW_PART=1,		//���ƹ����ṹͼ
	CMD_DRAW_TABLE=2,		//���Ʊ��
	CMD_DIM_PARTSPEC=3,		//��ע������ϸ
	CMD_DIM_ODDMENT=4,		//��ע����ͷ
	CMD_DIM_BOLTPAD=5,		//��ע��Ȧ��Ϣ
	CMD_DIM_BOLTINFO=6,		//��ע��˨��Ϣ
	CMD_DIM_SIMPOLY=7,		//��ע������
	CMD_DIM_MODELSIZE=8,	//��עģ�ͳߴ�
	CMD_DIM_MAPSIZE=9,		//��עͼ��ߴ�
	CMD_DIM_SPECIAL_LSSPACE=10,	//��ע��˨������
	CMD_DIM_SPECIAL_JGZHUN=11,	//��ע�Ǹ�����׼��
	CMD_DRAW_LINEPART=12,		//���Ƹ˼�����ͼ
	CMD_DIM_SEGI=13,			//��ע�κ�	
	CMD_DIM_FOOTNAIL_SPACE=14,	//�Ŷ����
	CMD_DRAW_ASSIST_LINE=15,	//���Ƹ�����
	CMD_DIM_BOLTORFL_INFO=16,	//˾��ͼ�б�ע�˼�ʼ�˻��ն���˨��Ϣ������Ϣ wht 12-06-01
	CMD_DIM_FILLPLATE_SPACE=17,	//�����
	CMD_DIM_ANGLEJOINT=18,		//��ע˾��ͼ�еĽ�ͷ��Ϣ wht 14-07-02
	CMD_DRAW_JG_JOINT=19,		//���ƽǸֽ�ͷ
	CMD_OTHER=100,				//����
};
//ģ�ͳߴ��ע����
enum DIM_MODE_SIZE_TYPE{
	DIM_NODE_VSPACE=0,		//�ڼ䴹��     
	DIM_HENGDAN_VSPACE=1,	//�ᵣ���ߵ�䴹ֱ���
	DIM_SEG_VSPACE=2,		//�ֶμ��
	DIM_TOWERHIGH=3,		//����
	DIM_SLOPE_HSPACE=4,		//���¿���
	DIM_HENGDAN_HSPACE=5,	//���ߵ�ˮƽ���
};
//������������
struct ANGLEWING_OFFSET{
	WORD wiData;
public:
	ANGLEWING_OFFSET(){wiData=0;}
	BYTE get_ciStyleG();	//
	BYTE set_ciStyleG(BYTE _ciStyleG);
	__declspec(property(put=set_ciStyleG,get=get_ciStyleG)) BYTE ciStyleG;	//�Ǹ�֫ƫ����:0.��׼׼��;1:g1;2:g2;3:g3;4:�Զ���
	BYTE get_ciSectStyle();
	BYTE set_ciSectStyle(BYTE _ciSectStyle);
	__declspec(property(put=set_ciSectStyle,get=get_ciSectStyle)) BYTE _ciSectStyle;//0.�ڵ㴹��������ߵ�;1.�ڵ�X-Yƽ�����ߵ�;2.�ڵ�Y-Zƽ�����ߵ�;3.�ڵ�X-Zƽ�����ߵ�
	short get_siOffsetDist();
	short set_siOffsetDist(short gDist);
	__declspec(property(put=set_siOffsetDist,get=get_siOffsetDist)) short siDistG;	//�ľ�(-511mm ~ 511mm)
};
struct SIMPOLY_VERTEX
{
	//hNextPole>0ʱ:��׼���߽���;�������hDatumRod��wxDatumRodOffsetȷ��
	ANGLEWING_OFFSET wxDatumRodOffset;
	long hDatumRod;
	long hViceRod;
	GEPOINT vertex;
	SIMPOLY_VERTEX(){memset(this,0,sizeof(SIMPOLY_VERTEX));}
};
//����ͼ����
class CStruView;
class CSectionViewSymbol
{
public:
	double fLenght;			//����
	double fTextSize;		//����߶�
	double fPolyLineWidth;	//����ͼ����߿��
	f3dPoint startPt;		//����ͼʼ��λ��
	f3dPoint endPt;			//����ͼ�ն�λ��
	BOOL bUpturned;			//startPt->entPt˳ʱ����ת90��Ϊ��,��ʱ����ת90��Ϊ��
	CXhChar16 viewName;		//����ͼ����
	//
	long hAttachViewKey;	//������ͼ(����������ͼ������ͼ)keyֵ
	long hViewKey;			//����ͼ��������ͼ
	ATOM_LIST<AcDbObjectId> startEntIds;//������ʵ��(���洢)
	ATOM_LIST<AcDbObjectId> endEntIds;	//�յ����ʵ��(���洢)
public:
	CSectionViewSymbol();
	CSectionViewSymbol(f3dPoint start,f3dPoint end,CXhChar16 name);
	void Draw();								//��������ͼ��
	void ModifySymbolName(char *symbol_name);	//�޸�����ͼ����
	void EraseRelativeAcadEntity();				//ɾ����ص�CADʵ��
	void UpdatePosition(double fOffsetX,double fOffsetY);//��������ͼ��λ��
	f2dRect GetDrawRegion();					//�õ���ͼ����
	SCOPE_STRU GetEntScope(int start0_end1_all2);
	void GetEntSet(ads_name &drag_ent_set,int start0_end1_all2=2,BOOL bSelectEnts=FALSE);//�������ͼ����Ӧ��ʵ�弯��
	static CSectionViewSymbol* AddSectionViewSymbol(CStruView *pMainView,CStruView *pStruView);
	void CopyProperty(CSectionViewSymbol *pSymbol);
	//
	void FromBuffer(CBuffer &buffer, long version=NULL);
	void ToBuffer(CBuffer &buffer);
};
//�Ǹֽ�ͷ����ͼ
struct JOINT_RELA_PARTS
{
	CLDSLineAngle *m_pShortJg;
	CLDSLineAngle *m_pDependJgS,*m_pDependJgE;
	CLDSPlate *m_pXWrapPlate,*m_pYWrapPlate;
	CLDSPlate *m_pXPadPlate,*m_pYPadPlate;
	BOOL m_bStartHasPad,m_bEndHasPad;
	//
	JOINT_RELA_PARTS()
	{
		m_pShortJg=NULL;
		m_pDependJgS=NULL;
		m_pDependJgE=NULL;
		m_pXWrapPlate=NULL;
		m_pYWrapPlate=NULL;
		m_pXPadPlate=NULL;
		m_pYPadPlate=NULL;
		m_bStartHasPad=FALSE;
		m_bEndHasPad=FALSE;
	}
};
class CJointWrapperView
{
	UCS_STRU ucs;
	int m_nGap;	//��϶ֵ
	int x_off_vec,y_off_vec;
	f3dPoint datumPt1,datumPt2,datumPt3,datumPt4;
	BOOL m_bInnerWrap,m_bGroupJoint;	//
	ATOM_LIST<f3dArcLine> edgeList;
	ATOM_LIST<CSizeTextDim> spaceDimList;	//��Գߴ�
	ATOM_LIST<CTextFrame> dataPointList;	//���ݵ�
public:
	CLDSLineAngle *m_pShortJg;
private:
	BOOL ExtractRelaParts(CLDSLineAngle* pShortJg,JOINT_RELA_PARTS& relaParts);
	int CalGapDist(JOINT_RELA_PARTS& relaParts);
	void UpdateOffVec(f3dPoint wingX_vec,f3dPoint wingY_vec);
	CLDSLineAngle* FindOherShortJg(JOINT_RELA_PARTS& relaParts);
public:
	CJointWrapperView();
	~CJointWrapperView();
	//
	void CreateDrawing();
	void BuildDrawCs();
	void DrawLineAngleSection(CLDSLineAngle* pAngle,f3dPoint base_pnt,BOOL bShortJg=TRUE);
	void DrawPlateSection(CLDSPlate* pPlate,f3dPoint base_pnt,BOOL bPad=FALSE);
	void DimAngleLsSpace(CLDSLineAngle* pShortJg,CLDSLineAngle* pAngle,f3dPoint base_pnt,double plank_thick=2);
	//
	f3dArcLine* EnumFirstArc(){return edgeList.GetFirst();}
	f3dArcLine* EnumNextArc(){return edgeList.GetNext();}
	CSizeTextDim* EnumFirstSizeDim(){return spaceDimList.GetFirst();}
	CSizeTextDim* EnumNextSizeDim(){return spaceDimList.GetNext();}
	CTextFrame* EnumFirstTextFrm(){return dataPointList.GetFirst();}
	CTextFrame* EnumNextTextFrm(){return dataPointList.GetNext();}
};
//������״̬
#define CMD_STATE_FREE			0x0		//����
#define CMD_STATE_NEXTPOS		0x01	//��һ��Ҫִ�е�����
#define CMD_STATE_BREAKPOINT	0x02	//�ϵ�
#define CMD_STATE_BOOKMARK		0x04	//��ǩ

class CDrawingComponent;
class CDrawingCommand : public CXeroDrawingCommand
{
	BOOL m_bFlag;	//������ʾ�������Ƿ��Ѿ�ִ�й�
	long m_nDrawingSerialNo;	//��������˳���
public:
	static CHashList<AcDbObjectId>hashHighlightEnts;
	long feature;
	CDrawPartTagInfo drawPartInfo;
	CDrawLineAngleTagInfo drawAngleInfo;
	CDrawingComponent *m_pDrawingCom;
public:
	CDrawingCommand();
	virtual ~CDrawingCommand();
	virtual IDrawingComponent* BelongDrawingComponent(){return (IDrawingComponent*)m_pDrawingCom;}
	typedef struct tagPART_PARA
	{
		CLDSPart *pPart;		//��ȡ���������õ�pPartָ��
		long hPart;				//�������,�ļ��ر��ٴ򿪺�ָ��ʧЧ�ʸ��þ��
		double fPoleAxisLineStartOutSpreadLen;	//�˼�����ʼ�����쳤��
		double fPoleAxisLineEndOutSpreadLen;	//�˼������ն����쳤��
		f2dPoint drawStartBasePos,drawEndBasePos;//�˼�ʼĩ�˵Ļ��ƻ���,��ʱ��¼���ڱ�ע����ͷʱ��
		char cDrawFlag;			//���������� 'X'.X���� 'Y'.Y����,'Z'.Z���� �Զ��ж� '0'.ƽ�̻��� '1'.��ֱ����
		struct PARA
		{	
			//�Ǹֻ��Ʋ���
			struct{
				int nXWingDatumAxisG;	//�Ǹ�X֫��׼�ľ�
				int nYWingDatumAxisG;	//�Ǹ�Y֫��׼�ľ�
				BOOL bDrawAxisLine;		//���ƽǸ�׼�ߣ��ݱ����Ա��������ýǸ��Ƿ���ƻ�׼�߿���ʱ���ø����ļ��汾 wjh-2011.8.31
				short extendStyle;		//֫�������ʱ����չ��ʽ,0:������չ;1:��֫�����չ;2:�����߲���չ
			}angle;
			//��˨���Ʋ���
			struct{
				BOOL bOriginal;		//�Ƿ�������˨
				bool bDimHoleDiameter;	//��������˨���Ƿ��ע��˨��ֱ��
				bool bDrawSideBoltLine;	//ƽ�Ż��Ƶȳ�ֱ��
				short dimAngle;		//��ע��ת�Ƕ�(��)
				short dimFarOffset;	//��ע����ƫ�ƾ���
				int front0_side1;	//������˨���ǲ�����˨
			}bolt;
		}para;
	}PART_PARA;
	typedef struct tagLINEPART_PARA
	{
		CLDSLinePart *pLinePart;//��ȡ�˼������õ�pLinePartָ��
		long hLinePart;			//�������,�ļ��ر��ٴ򿪺�ָ��ʧЧ�ʸ��þ��
		BOOL bSumMap;			//�Ƿ�Ϊ��ͼ
		bool bAdjoinRod;		//���ƺᵣͬʱ��Ҫ���Ƶ���������θ˼����ߣ�Ӧ��Ϊ���� wjh-2018.5.22
		BOOL bDimByAnalysisMode;//�Ƿ�Ϊ��ѧ����ģʽ
		f2dPoint drawStartBasePos,drawEndBasePos;//�˼�ʼĩ�˵Ļ��ƻ���,��ʱ��¼���ڱ�ע��˨��Ϣ������Ϣʱ��
	}LINEPART_PARA;
	typedef struct tagPART_SPEC_PARA{
		CLDSDbObject *pObj;		//��ȡ��������õ�pObjָ��
		long hObj;				//�������,�ļ��ر��ٴ򿪺�ָ��ʧЧ�ʸ��þ��
		double fBasePosOffsetX;	//���ݵ�λ��X����ƫ����	wht 11-07-25
		double fBasePosOffsetY;	//���ݵ�λ��Y����ƫ����
		double fDimPosOffsetX;	//��עλ��X����ƫ��ֵ
		double fDimPosOffsetY;	//��עλ��Y����ƫ��ֵ
		double fGuigeOffsetX;	//�ְ����עλ��X����ƫ��ֵ
		double fGuigeOffsetY;	//�ְ����עλ��Y����ƫ��ֵ
		int  iDimPosType;		//0.�Զ�����λ�� 1.ָ��λ��(����ָ��λ����ԭ��עλ�õ�ƫ��ֵ) 2.ѡ��λ��
		BOOL bOnlyDimPartNo;	//����ע�˼����
		//�¼ӱ��� wht 10-12-03
		BOOL bDrawRefLine;		//�Ƿ����������
		double fPoleDimPosCoef;	//�˼���עλ��ϵ�� //LTMA 0.3  TAS 0.5
		int iMirMsg;			//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
	}PART_SPEC_PARA;
	typedef struct tagODDMENT_PARA{
		CLDSLinePart *pLinePart;//��ȡ�˼������õ�pLinePartָ��
		long hLinePart;			//�������,�ļ��ر��ٴ򿪺�ָ��ʧЧ�ʸ��þ��
		//�¼�����,����ʵ������ͷ��עλ�ÿɵ��� wht 11-07-26
		int iDimPosType;		//0.�Զ�����λ�� 1.ָ��λ��
		BOOL bDimStartOdd;
		BOOL bDimEndOdd;
		double fStartOddBasePosX;	//ʼ������ͷ��ע����X����(��ʱ�洢�����ֶ�ƫ��,������ű��ļ�)
		double fStartOddBasePosY;	//ʼ������ͷ��ע����Y����(��ʱ�洢�����ֶ�ƫ��,������ű��ļ�)
		double fEndOddBasePosX;		//ʼ������ͷ��ע����X����(��ʱ�洢�����ֶ�ƫ��,������ű��ļ�)
		double fEndOddBasePosY;		//ʼ������ͷ��ע����Y����(��ʱ�洢�����ֶ�ƫ��,������ű��ļ�) -wjh 2011.9.25
		double fStartOddPosOffsetX;	//����ڸ˼���ͷ��׼λ��(�Ǹ�����,�ֹ������߶˵�,δ������ͷ)
		double fStartOddPosOffsetY;	//����ڸ˼���ͷ��׼λ��(�Ǹ�����,�ֹ������߶˵�,δ������ͷ)
		double fEndOddPosOffsetX;	//����ڸ˼���ͷ��׼λ��(�Ǹ�����,�ֹ������߶˵�,δ������ͷ)
		double fEndOddPosOffsetY;	//����ڸ˼���ͷ��׼λ��(�Ǹ�����,�ֹ������߶˵�,δ������ͷ)
		double fPoleAngle;
	}ODDMENT_PARA;
	typedef struct tagBOLTPAD_PARA{
		CLDSBolt *pBolt;		//��ȡ��˨�����õ�pBoltָ��
		long hBolt;				//�������,�ļ��ر��ٴ򿪺�ָ��ʧЧ�ʸ��þ��
		int iDimPosType;		//0.�Զ�����λ�� 1.ָ��λ��
		double fDimPosOffsetX;	//��עλ��X����ƫ��ֵ
		double fDimPosOffsetY;	//��עλ��Y����ƫ��ֵ
	}BOLTPAD_PARA;
	typedef struct tagBOLTINFO_PARA{
		CLsGroupDimUnit lsGroupDim;
		int iDimPosType;		//0.�Զ�����λ�� 1.ָ��λ��
		double fDimPosOffsetX;	//��עλ��X����ƫ��ֵ
		double fDimPosOffsetY;	//��עλ��Y����ƫ��ֵ
	}BOLTINFO_PARA;
	typedef struct tagSIMPOLY_PARA{
		int nSimPolyCmdType;	//�������������� 0.��ѡʰȡ����������� 1.ѡ����������������� 2.��ָ�������������� 3.δָ��������������
		double fScale;			//ָ�������α���
		double fPickPtX;		//ʰȡ��X����(�������)
		double fPickPtY;		//ʰȡ��Y����(�������)
		bool   bPickPosIsLocalCoord;	//ʰȡ���������������ͼԭ��ֲ����꣬���������������ͼֽԭ�㣬����ʱ��ҪתΪ����ͼֽԭ�� wjh-2018.5.22
		bool   bDrawRefLine;	//������������ƫ��ʱ���Ƿ���������� wjh-2017.8.15
		double fDimPosOffsetX;	//�����α�עλ��X�᷽��ƫ����		
		double fDimPosOffsetY;	//�����α�עλ��Y�᷽��ƫ����
		tagSIMPOLY_PARA(){bDrawRefLine=bPickPosIsLocalCoord=false;fDimPosOffsetX=fDimPosOffsetY=0;fScale=0.3;}
		ATOM_LIST<SIMPOLY_VERTEX> simPolyVertexList;	
	}SIMPOLY_PARA;
	typedef struct tagBOM_TABLE_PARA{
		int nLocatePos;		//0.���� 1.���� 2.���� 3.����
		BOOL bSelectPos;	//ѡ����λ��
		BOOL bDisplayDlg;	//��ʾѡ��λ��λ�õĶԻ���
		double fDimPosX;	//���λ��X����		
		double fDimPosY;	//���λ��Y����
		double fTableWidth;	//��񳤶�
		double fTableHeight;//
		int iTableType;		//������� 
		int iFormatTbl;		//iFormatTbl=1��2��ʾ����������������ͼԪ��xFormatTbl1��xFormatTbl2��Ӧ�� wht 11-07-14
	}BOM_TABLE_PARA;
	//��עģ�ͳߴ�,ͼ��ߴ�
	typedef struct tagDIMSIZE_PARA{
		//ͼ��ߴ��ע��ʼ��(�������ϵ��)
		double fDimStartX;
		double fDimStartY;
		//ͼ��ߴ��ע��ʼ��(�������ϵ��)
		double fDimEndX;
		double fDimEndY;
		//ͼ��ߴ��עλ��(�������ϵ��)
		double fDimPosX;
		double fDimPosY;
		double fScale;	//�ߴ����
		int iDimDirect;	//��ע���� 0.X�᷽�� 1.Y�᷽�� 2.���췽��		
	}DIMSIZE_PARA;
	//��עģ�ͳߴ�
	struct DIMNODE{
		long hNode;
		CLDSNode* pNode;
		DIMNODE(CLDSNode* _pNode=NULL) {
			pNode=_pNode;
			hNode=_pNode!=NULL?_pNode->handle:0;
		}
	};
	struct DIM_MODELSIZE_PARA{
		static CLDSNode xMidAxisNode;
		ATOM_LIST<DIMNODE> listDimNodes;	//�ڵ�������(�ļ��رպ��ٴ򿪽ڵ�ָ�뷢���仯,�ʴ˴�����ڵ�����)
		int iDimType;		//��׼����0Ϊ�ڼ䴹�� 1Ϊ�ᵣ���ߵ�䴹ֱ��� 2Ϊ�ֶμ�� 3Ϊ���� 4���¿��� 5Ϊ���ߵ�ˮƽ���
		double fDimDistance;//��ע�������ע��ʼ����ֵ
		double fSpace;		//��ע��ʼ����������ֵ
		double fMinX;       //�����עʱ����߽ڵ��X����
		BOOL bUpDim;        //ˮƽ��עʱ��עƫ�Ʒ���
	public:
		DIMNODE* AppendDimNode(CLDSNode* _pNode=NULL);
	};
	//��ע�Ǹ�����׼��
	typedef struct tagDIM_JGZHUN_PARA
	{	
		CLDSLineAngle *pLineAngle;	//��ȡ�Ǹ־����õ�pLineAngleָ��
		long hLineAngle;			//�������,�ļ��ر��ٴ򿪺�ָ��ʧЧ�ʸ��þ��
		CZhunXianTextDim zhun_dim;
		double fDimDistance;		//��ע�������ע��ʼ��ļ��ֵ
		double fSpace;				//��ע��ʼ������˨�ļ��ֵ
		BOOL bInside;				//���Ʊ�ע����
	}DIM_JGZHUN_PARA;
	//��ע��˨������
	typedef struct tagDIM_LSSPACE_PARA
	{	
		CLDSPart *pPlate;			//��ȡ���������õ�pPartָ��
		long hPlate;				//�������,�ļ��ر��ٴ򿪺�ָ��ʧЧ�ʸ��þ��
		CLsSpaceDimUnit spaceDim;	//��˨����¼
		double fDimDistance;		//��ע�������ע��ʼ��ļ��ֵ
		double fSpace;				//��ע��ʼ������˨�ļ��ֵ
		BOOL bInside;				//���Ʊ�ע����
	}DIM_LSSPACE_PARA;
	//��ע�κ�
	typedef struct tagDIM_SEGI_PARA
	{
		int bSelectPos;		//0.���趨λ��;1.ѡ��λ��;2.�Զ���ʼ��Ϊ�ֶ����ĵ�Ϊ��׼��λ��ƫ��
		SEGI nSegI;			//�����κ�
		double fDimPosX;	//��עλ��X����(��ͼ����ϵ�£�����ԭ��ȡ����bSelectPos)
		double fDimPosY;	//��עλ��Y����(��ͼ����ϵ�£�����ԭ��ȡ����bSelectPos)
	}DIM_SEGI_PARA;
	typedef struct tagDIM_FOOTNAILSPACE_PARA{
		CLDSLinePart *pLinePart;//��ȡ�˼������õ�pLinePartָ��
		long hLinePart;			//�������,�ļ��ر��ٴ򿪺�ָ��ʧЧ�ʸ��þ��
		double fDimDistance;	//��ע�������ע��ʼ��ļ��ֵ
		double fSpace;			//��ע��ʼ����˼����ļ��ֵ
	}DIM_FOOTNAILSPACE_PARA;
	typedef struct tagDRAW_ASSIST_LINE
	{
		int iNo;	// ������������
		int nStyle;	// 1.�������� 2.����������
	}DRAW_ASSIST_LINE;
	typedef struct tagANGLEJOINT_PARA{
		ANGLE_JOINT *pJoint;	//��ȡ�Ǹֽ�ͷid��õ��Ǹֽ�ͷָ��
		long idAngleJoint;		//��ͷid,�ļ��ر��ٴ򿪺�ָ��ʧЧ�ʸ��þ��
		int iDimPosType;		//0.�Զ�����λ�� 1.ָ��λ��
		double fDimPosOffsetX;	//��עλ��X����ƫ��ֵ
		double fDimPosOffsetY;	//��עλ��Y����ƫ��ֵ
	}ANGLEJOINT_PARA;
public:
	DRAW_CMD_TYPE m_eCmdType;	//��������
	BYTE m_stateCmd;			//����״̬
	BOOL m_bExecute;			//�Ƿ�ִ�и�����
	ATOM_LIST<AcDbObjectId> entIdList;	//���������ʵ��ID�б�
	//�������
	PART_PARA m_paraPart;
	LINEPART_PARA m_paraLinePart;
	PART_SPEC_PARA m_paraPartSpec;
	ODDMENT_PARA m_paraOddment;
	BOLTPAD_PARA m_paraBoltPad;
	BOLTINFO_PARA m_paraBoltGroupInfo;
	SIMPOLY_PARA m_paraSimPoly;
	BOM_TABLE_PARA m_paraBOMTable;
	DIMSIZE_PARA m_paraDimSize;
	DIM_JGZHUN_PARA m_paraDimJgZhun;
	DIM_LSSPACE_PARA m_paraDimLsSpace;
	DIM_MODELSIZE_PARA m_paraDimModelSize;
	DIM_SEGI_PARA m_paraDimSegI;
	DIM_FOOTNAILSPACE_PARA m_paraDimFootNailSpace,m_paraDimFillPlateSpace;
	DRAW_ASSIST_LINE m_paraDrawAssistLine;
	ODDMENT_PARA m_paraBoltOrFlInfo;	//˾��ͼ����˨������Ϣ wht 12-08-01
	ANGLEJOINT_PARA m_paraAngleJoint;	//˾��ͼ�б�ע�Ǹֽ�ͷ��Ϣ wht 14-07-03
public:
	void InitCommandPara(int eDrawCmdType=0);	//�����������ͳ�ʼ���������
	int GetDrawType(char *sDrawType=NULL);	//0.ƽ�̻��� 1.��ֱ���� 2.������� wht 11-07-22
	long GetCmdInfo(char *sCmdId,char *sHelp,char *sPartNo=NULL);//�õ�������Ϣ ��ʶ�ַ����Լ�������Ϣ
	CString GetDrawPartName();			//��ȡ��ǰ���ƹ���������
	BOOL GetDCS(CDrawCoordTrans &dcs);			//�ɻ�ͼ����ϵ��ƽ��ƫ��������õ��µ�����ϵ
	void SetCmdFlag(BOOL bFalg=TRUE){m_bFlag=bFalg;} //���������־λ ������ʾ�������Ƿ���ִ��
	BOOL IsNeedExecute() {return !m_bFlag&&m_bExecute;}	//�жϸ������Ƿ���Ҫִ��
	int Run();				//ִ�и�����
	int Rerun();			//�������и�����
	void EmptyEntList();	//ɾ���ɸ��������ɵ�ʵ��
	void FromBuffer(CBuffer &buffer, long version=NULL);
	void ToBuffer(CBuffer &buffer);
	void CopyProperty(CDrawingCommand *pCommand);
	SCOPE_STRU GetCADEntScope();
	void UpdateCommandPara(double fOffsetX,double fOffsetY);
	//��������Ӧ��ʵ�弯��
	int GetEntSet(ads_name &drag_ent_set,BOOL bSelectEnts=FALSE);
	//���������Ӧ��ʵ�弯��Ϊ������ʾ
	void SetEntSetHighlight(int type=0);	//type����ʱ���ڸ�����ʾ�Ǹ�����ͷ,0.��ʾȫ��,1.��ʾʼ��,2.��ʾ�ն�
	static void ReleaseHighlightEnts();
	//ͨ���������ͼֽ��������ص�ָ��,������ٴδ��ļ�ʱʹ�� wht 11-07-22
	void UpdatePtrByHandle();
	//���ù�������˳���
	int SetDrawingSerialNo(int serialNo);
	//�õ���������˳���
	int GetDrawingSerialNo();
private:
	//�õ��κŶ�Ӧ��ʵ�弯�ϼ���������
	BOOL GetEntSetAboutDimSegICmd(ATOM_LIST<AcDbObjectId> &entIdList,BOOL bHighlight,SCOPE_STRU *pScope=NULL);
public:
	SEGI GetSegI(void);
};

typedef struct tagCAD_OBJECT_ID
{
	int entType;	//1.��ͼ���� 2.�ϽӶκ�
	AcDbObjectId entId;
	DWORD dwDataRef;	
	tagCAD_OBJECT_ID(){entId.setNull();entType=0;dwDataRef=0;}
	tagCAD_OBJECT_ID(int ent_type,AcDbObjectId ent_id,DWORD ref_data=0)
	{	
		dwDataRef=ref_data; 
		entType=ent_type;
		entId=ent_id;
	}
}CAD_OBJECT_ID;
//��ͼ����:
//1.ͬһ����ͼ���漰�������˼��˵㣩�ڵ���������һ��
//2.ͬһ����ͼ���漰��ͷ���߷��Ž��������һ��
//3.һ��˾��ͼֽ���������ͼ)���й����ԳƵĸ˼�����ͷ���ٻ���һ�Σ�ÿ�����Գ���Ͻ������עһ��,�ұ����עһ��
struct DRAWN_STATE{	//һ��ͼֽ��ÿ���˼�������һ�Σ��й����ԳƵĸ˼����ٻ���һ�������ֻ�ܱ�עһ��
	bool drawn;		//�ѻ���
	bool dimspec;	//�˼�����ѱ�ע��
	DRAWN_STATE(){drawn=dimspec=false;}
};
class CDrawing;
class CDrawingComponent : public CXeroDrawingComponent
{
protected:
	PARTSET partSet;
	NODESET nodeSet;
	ATOM_LIST<long> partHandleList;
	ATOM_LIST<long> nodeHandleList;
	f2dRect draw_rect; 
	virtual void GetDrawingObjectSet(int bInitDCS){;}	//��ȡ��Ҫ���ƵĶ��󼯺�
	UCS_STRU dcs;				//��ͼ����ϵ,��������ϵת��Acad��������ϵ��ת������ϵ
	char m_cCurrSegType;	//'T','S','L'
public:
	CHashList<CDrawLineAngleTagInfo>hashAngleDrawInfo;
	f3dPoint drawing_offset;	//��Acad��������ϵĬ�ϻ��ƻ���ƽ�Ƶ�ͼ��Ŀ����������ƫ��ʸ����ֻ�漰X��Y��ƫ�ƣ�
	long feature;				//��ʱʹ�ò��洢
	//CDrawing *m_pDrawing;		//����ͼֽ
	long m_hAttachView;			//������ͼ���
	CDisplayView *m_pAttachView;//������ͼ
	long m_hModule;				//�������߾��
	CLDSModule *m_pModule;		//˾��ͼ����ͼ�л��Ʒǵ�ǰ�������ȵ���ͼ
	CXhChar16 m_sDrawingLayer;	//ͼ�㷶Χ
	double m_fNamePosSpace;		//��ͼ��������ͼ�ײ��ļ�϶ֵ

	CXhChar50 m_sName;	//��ͼ����
	BOOL m_bDimName;	//��ע��ͼ����
	CXhChar16 m_sSectionViewCode;		//����ͼ����
	DRAW_COMPONENT_TYPE m_eClassType;	//��ͼ���� 
	TBL_LOCATE_STYLE m_nAnchorType;		//��λ���� 0.���� 1.���� 2.���� 3.����
	BOOL m_bSpecifyComPos;				//ָ��ͼԪλ��
	BOOL m_bNeedAdjust;					//����ͼ��Ҫ����
	BOOL m_bFirstCreate;				//��һ�����ɸ���ͼ����Ҫ�Զ������ʼװ��λ��
	double m_fScale;					//���Ʊ���
	BOOL m_bDimScale;					//��ע���Ʊ���
	ATOM_LIST<CDrawingCommand> cmdList;	//��ͼ��������
	CHashList<AcDbObjectId> keyEntIdList;//��Ӧ��CAD��ʵ��ID,��ע������ϸʱʹ��
	ATOM_LIST<CAD_OBJECT_ID> entIdList;	//ʵ��ID�б� ��ͼ����Id,�ϽӶκ�Id ���ڼ�¼���ƶ�Ӧ��ʵ��ID���ڵ���λ�� 
public:
	CDrawingComponent(int id=0,IDrawing* pDrawing=NULL);
	virtual ~CDrawingComponent();
	
	UCS_STRU GetDCS() const{return dcs;}
	void SetDCS(UCS_STRU ucs){dcs=ucs;}
	CLDSPart *GetFirstPart(){return partSet.GetFirst();}
	CLDSPart *GetNextPart(){return partSet.GetNext();}
	long GetPartCount(){return partSet.GetNodeNum();}
	CLDSNode *GetFirstNode(){return nodeSet.GetFirst();}
	CLDSNode *GetNextNode(){return nodeSet.GetNext();}
	long GetNodeCount(){return nodeSet.GetNodeNum();}
	BOOL IsNeedSpecifyComPos();			//��Ҫ�ƶ���ͼ�ڷ�λ��
	f3dPoint GetMapFrmInsideTopLeft();	//�õ���׼�㼴ͼ���ڲ�������ϽǶ���
	AcDbEntity *FindCADEntByHandle(long hObj);	//���ݹ�������õ�ָ����CADʵ��
	f3dPoint SpecifyAssemblyPosition();				//ָ����ͼװ��λ��
	void CalInitAssemblyPosition();				//������ͼ��ʼװ��λ��
	//void CalAssemblyPosByRelativePos();		//ͨ�����λ�ü���װ��λ��
	void InsertDrawingName();					//������ͼ����
	//�޸Ķ�λ�����λ��
	void ModifyAnchorPointRelativePos(double *pOffsetX,double *pOffsetY);
	f3dPoint GetAnchorPointRelativePos();				//��ȡ��λ�������ͼ�����Ͻǵ����λ��
	void UpdatePtrByHandle();					//ͨ���������ͼֽ��������ص�ָ��,������ٴδ��ļ�ʱʹ�� wht 11-07-22
	void InitCmdFeature(int feature);			//��ʼ����������Ӧ��feature wht 11-07-22
	//��ȡ����ʵ�弯
	void GetAttachEntSet(ads_name &drag_ent_set,int name1_seg2_all0=0,
		BOOL bSelectEnts=FALSE,SCOPE_STRU *pScope=NULL);
	void ModifyComponentName(char *com_name);	//�޸��������
	//�����ͼ��Ӧ��ʵ�弯��
	virtual void GetEntSet(ads_name &drag_ent_set,BOOL bSelectEnts=FALSE);	
	virtual int UpdateDrawingRectArray();		//������ͼ����Ҫ���»�ͼ����
	virtual f2dRect GetComponentRect();			//��ͼ��ͼ����
	virtual void Create();						//������ͼ
	virtual void EraseRelaCadEnt();				//ɾ��������CADʵ�� wht 13-04-19
	virtual void ReCreate();					//ɾ����ǰ��ͼ����������������
	//�ڼ�����ͼ��ʵ��ͼ����ʱͨ��bIncludePartEnt���ƽ����㹹����������(��ȡ���Ʊ�עλ��ʱʹ��)
	virtual f2dRect GetDrawRegion(BOOL bRealRect=TRUE,BOOL bOnlyIncludePartEnt=FALSE);	//�õ���ͼ��������
	virtual BOOL CreateCmdList(BOOL bInit=TRUE){return TRUE;}//������������
	virtual void CopyProperty(CDrawingComponent* pCom);	//��������
	virtual void UpdateCmdList(BOOL bUpdateViewDcs=FALSE){;}						//���������б� wht 11-07-31
	//
	virtual void FromBuffer(CBuffer &buffer, long version=NULL);
	virtual void ToBuffer(CBuffer &buffer);
public:	//���ڼ��ٴ�������д�ļ��׷�װ����
	CDrawing* ParentDrawing(){return (CDrawing*)m_pParentDrawing;}
private:
	CHashList<DRAWN_STATE> hashRodDrawnStates;
	CHashList<DRAWN_STATE> hashJointDrawnStates;
	CHashList<DRAWN_STATE> hashNodesDrawn;	//���ָ���ڵ��Ƿ��Ѿ��ڵ�ǰ��ͼ�л��ƹ�(Ŀǰ��ͨ��CLDSNode::is_visible�趨��ʵ��)
public:
	static const int TYPE_ROD	= 1;
	static const int TYPE_JOINT = 2;
	static const int TYPE_NODE	= 3;
	void ClearDrawnStates(int stateType=TYPE_ROD);
	bool IsItemDrawn(int stateType,long idItem);	//����ָ����Ŀ�Ƿ��ڵ�ǰͼֽ�л��ƹ�
	bool IsItemDimmed(int stateType,long idItem);	//����ָ����Ŀ�Ƿ��ڵ�ǰͼֽ�б�ע��
	void MarkItemDrawnState(int stateType,long idItem,bool drawn=true);
	void MarkItemDimSpecState(int stateType,long idItem,bool dimmed=true);
};

class CDrawingBOM : public CDrawingComponent
{
public:
	DRAW_BOM_TYPE m_eBOMType;	//�������
	ATOM_LIST<SPEC_MARK> SpecList;	//�򻯹��ʱʹ��
	BOOL m_bAutoNewTable;		//������ʱ�Զ������µı��
	BOOL m_bHasTableHead;		//�±���Ƿ����ͷ
	CFormatTbl xFormatTbl1,xFormatTbl2;
	int m_nBriefGuigeN;			//�򻯹�������˼��򻯱���ʹ�� wht 11-06-15
public:
	CDrawingBOM(int id=0,IDrawing* pDrawing=NULL);
	~CDrawingBOM();
	//
	BOOL PrepareTable();
	void Create();				//������ͼ
	BOOL CreateCmdList(BOOL bInit=TRUE);//������������
	void UpdateTableInitPos();	//���±���ʼλ��
	f2dRect GetComponentRect();	//��ͼ��ͼ����
	int UpdateDrawingRectArray();//������ͼ����Ҫ���»�ͼ����
	void CopyProperty(CDrawingComponent* pCom);	//��������
	//
	void ToBuffer(CBuffer &buffer);
	void FromBuffer(CBuffer &buffer, long version=NULL);
};

//����ͼ
class CLineView : public CDrawingComponent
{
	double TOPEST_Z;			//������ߵ�
	double TOPEST_TRUNK_Z;		//������ߵ�
	double LOWEST_WIREPOINT_z;	//��͹ҵ�
protected:
	void GetAjointSegInfo();
	void GetDrawingObjectSet(int bInitDCS);	//��ȡ��Ҫ���ƵĶ��󼯺�
	//���ر�עʱX����
	//iType: 0.Ϊ�ڼ䴹�� 1.�ᵣ���ߵ�䴹ֱ��� 2.�ֶμ�� 
	//		 3.���� 4.���¿��� 5.���ߵ�ˮƽ��� 
	double ChooseFeatPt(NODESET &nodesetSeleted,int iType=0,char cDrawAxisX='X');
	void CreateDimPartSpecCmdList();	//���ɹ�����ϸ��ע�����б�
	void CreateDimBoltOrFlInfoCmdList();	//������˨��ϸ��ע�����б�	
	void CreateAngleJointCmdList();			//���ɽǸֽ�ͷ��ע�����б�	
public:
	struct RODLINE{
		long hRod;
		GELINE briefline;
		RODLINE(){hRod=0;}
		RODLINE(GELINE line,long _hRod=0)
		{
			briefline=line;
			hRod=_hRod;
		}
		RODLINE(const double* lineStart,const double* lineEnd,long _hRod=0)
		{
			briefline.start=lineStart;
			briefline.end=lineEnd;
			hRod=_hRod;
		}
	};
	struct DIMSIZE_NODE{
		long hNode;
		char cbDimSizeFlag;					//һ��ͼ/��ͼ/˾��ͼ����Ҫ���еĳߴ��ע�0x01:pos.x;0x02;pos.y;0x04:pos.z;
		long hDimCmdCoordX;
		long hDimCmdCoordY;
		long hDimCmdCoordZ;
	public:
		DIMSIZE_NODE();
		void SetKey(DWORD key){hNode=(long)key;}
	public:
		bool get_blNeedAddDimCoordX();
		bool get_blNeedAddDimCoordY();
		bool get_blNeedAddDimCoordZ();
		__declspec(property(get=get_blNeedAddDimCoordX)) bool blNeedAddDimCoordX;
		__declspec(property(get=get_blNeedAddDimCoordY)) bool blNeedAddDimCoordY;
		__declspec(property(get=get_blNeedAddDimCoordZ)) bool blNeedAddDimCoordZ;
	};
	CHashListEx<DIMSIZE_NODE> hashNodeDimensions;
public:
//iType: 0.Ϊ�ڼ䴹�� 1.�ᵣ���ߵ�䴹ֱ��� 2.�ֶμ�� 
//		 3.���� 4.ˮƽ��ע(��ͷ�������ڳߴ�) 5.���ߵ�ˮƽ��� 
	static const int DIM_SECTION_Z			= 0;	//Ϊ�ڼ䴹���ע
	static const int DIM_SEG_SPACE_Z		= 1;	//�ֶμ��
	static const int DIM_GUAXIAN_SPACE_Z	= 2;	//�ᵣ���ߵ�䴹ֱ���
	static const int DIM_TOWER_HEIGHT		= 3;	//����
	static const int DIM_SHANK_SPACE_XY		= 4;	//�����¶ο��ڳߴ�
	static const int DIM_GUAXIAN_SPACE_XY	= 5;	//��ͷ(�����ߵ�)ˮƽ����ע
	static BOOL m_bUsePolyLineSumMap;	//
	static BOOL m_bUsePolyLineChiefMap;
	BOOL m_bAnalysisMode;	//��ѧ����ģʽ
	BOOL m_bDimNodeNo;		//��ע�ڵ���,��ѧ����ʱ�ñ�����Ч wht 11-06-14
	BOOL m_bDimPoleGuige;	//��ע�˼����,��ѧ����ʱ�ñ�����Ч
	CHashList<AcDbObjectId> jointEntIdList;			//��ͷCADʵ��id��˾��ͼ�б�ע��ͷ��Ϣʱʹ�� wht 14-07-03
	PREVIEW preview;
	CUnifyModelLayer trunk;
	void CreateHorzDimensions();	//����ˮƽ�ߴ��ע
	void CreateVertDimensions(double BASE_ZERO_Z=0);	//���ɴ�ֱ�ߴ��ע
public:
	CLineView(int id=0,IDrawing* pDrawing=NULL);
	~CLineView();
	
	void EraseRelaCadEnt();				//ɾ��������CADʵ�� wht 14-07-03
	void Create();						//������ͼ
	void IntelliCreateDimensions();		//������ͷ/�ᵣ����ʶ��Ľ������ʶ��ߴ��ע
	void CreateDimensions();
	void CreateSegmentLabelDimensions(SEGI *pOnlyDrawSegI=NULL,SEGI *pAdjoinTrunkSegI=NULL);
	BOOL CreateCmdList(BOOL bInit=TRUE);//������������
	void UpdateCmdList(BOOL bUpdateViewDcs=FALSE);	//���������б� wht 11-07-31
public:	//
	void ToBuffer(CBuffer &buffer);
	void FromBuffer(CBuffer &buffer, long version=NULL);
};

//////////////////////////////////////////////////////////////////////////
//չ����
class CDrawingSpreadLine
{
	CLDSLineAngle *m_pLineAngle;	//��׼�Ǹ֣������ظ����ҽǸ�
public:
	CXhChar50 m_sLineName;	//չ��������
	BYTE m_iDefType;		//0.ָ���˼� 1.ֱ��ָ��
	long m_hDatumPole;		//��׼�˼� 
	BYTE m_iCurWing;		//0.X֫ 1.Y֫
	double m_fOffsetDist;	//ƫ�ƾ���
	f3dLine m_lineSpread;	//չ����
	
	CDrawingSpreadLine();
	CLDSLineAngle *GetDatumPole();
	void FromBuffer(CBuffer &buffer, long version=NULL);
	void ToBuffer(CBuffer &buffer);
};
//////////////////////////////////////////////////////////////////////////
//�˼���עλ�ã��ж�ͬ���Ź����Ǹ�Ӧ�ñ�ע���ͳ���ʱʹ��
class CRodDimPos
{
private:
	long hRod;
	f3dPoint startPos,endPos,dimPos;
	f3dPoint CalRodDimPos()
	{
		double fDimPosCoef=0.3;
		double angle=Cal2dLineAng(startPos.x,startPos.y,endPos.x,endPos.y);
		if(angle>Pi/2.0&&angle<=1.5*Pi)
		{
			angle-=Pi;
			dimPos=endPos+(startPos-endPos)*fDimPosCoef;
		}
		else
			dimPos=startPos+(endPos-startPos)*fDimPosCoef;
		return dimPos;
	}	
public:
	void InitRodDimPos(long hPart,f3dPoint start,f3dPoint end)
	{
		hRod=hPart;
		startPos=start;
		endPos=end;
		dimPos=CalRodDimPos();
	}
	CRodDimPos(long hPart,f2dPoint start,f2dPoint end)
	{
		f3dPoint start_pt(start.x,start.y,0),end_pt(end.x,end.y,0);
		InitRodDimPos(hPart,start_pt,end_pt);
	}
	CRodDimPos(long hPart,f3dPoint start,f3dPoint end)
	{
		InitRodDimPos(hPart,start,end);
	}
	CRodDimPos(){;}
	long GetRodHandle(){return hRod;}
	f3dPoint GetDimPos(){return dimPos;}
};
//////////////////////////////////////////////////////////////////////////
//�ṹͼ
class CStruView : public CDrawingComponent
{
	POLYPARTLIST polyPartlist;
	void GetDrawingObjectSet(BOOL bInitDCS);	//��ȡ��Ҫ���ƵĶ��󼯺�
	void CreateDrawingPartCmdList();//���ƹ��������б�
	void CreateDimOddmentCmdList();	//��������ͷ��ע����
	void CreateDimPartSpecCmdList();//���ɹ�����ϸ��ע
	void CreateDimFootNailSpaceCmdList();	//���ɽŶ�����ע����
	void CreateDimFillPlateSpaceCmdList();	//����������ע����
	void CreateDimSpecialLsSpaceCmdList();	//������˨�������ע����
	void CreateDimSpecialJgZhunJuCmdList();	//���ɽǸ�����׼�ݱ�ע����
	void CreateDimBoltInfoCmdList();		//������˨��Ϣ��ע�����б�
	void CreateDimBoltPadCmdList();			//������˨��Ȧ��ע�����б�
	void CreateDrawAssistLineCmdList();		//���Ƹ����������б�
	int  CalDualRodIntersPoint(CLDSLinePart* pPrevRod,CLDSLinePart* pNextRod,SIMPOLY_VERTEX* pBaseVertex,SIMPOLY_VERTEX* pInsertVertex);
	void CreateSimPolyDimensions();			//������ȡ��������α�ע����
	PREVIEW preview;
public:
	static BOOL m_bDimPartSpec;
	static BOOL m_bDimPoleOddment;
	static BOOL m_bDimFootNailSpace;
	static BOOL m_bDimFillPlateSpace;
	static BOOL m_bDimLsSpace;
	static BOOL m_bDimJgZhunJu;
	static BOOL m_bDimBoltInfo;
	static BOOL m_bDimBoltPad;
	static BOOL m_bDimSimPoly;
public: 
	BOOL m_bDrawRefLine;	//�Ƿ����������
	int m_iMirMsg;			//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
public:
	CStruView(int id=0,IDrawing* pDrawing=NULL);
	~CStruView();

	void Create();			//������ͼ
	BOOL CreateCmdList(BOOL bInit=TRUE);//������������
	CString MakeSpreadFaceSetStr();
	CDrawingAssistant *GetDrawingAssistantByNo(int iNo);
	CDrawingAssistant *GetDrawingAssistantByHandle(long hPart);
	CLDSPolyPart *GetFirstPolyPart(){return polyPartlist.GetFirst();}
	CLDSPolyPart *GetNextPolyPart(){return polyPartlist.GetNext();}
	int GetPolyPartCount(){return polyPartlist.GetNodeNum();}
	//
	void ToBuffer(CBuffer &buffer);
	void FromBuffer(CBuffer &buffer, long version=NULL);
	//��ע����ͼ��
	CSectionViewSymbol *GetSectionViewSymbol();
	//�����ͼ��Ӧ��ʵ�弯��
	void GetEntSet(ads_name &drag_ent_set,BOOL bSelectEnts=FALSE);
	//ͼ�淨�߷����仯֮����Ҫ������˨���Ʒ�ʽ
	void UpdatePartDrawTypeAndSpecialDim();
	void UpdateCmdList(BOOL bUpdateViewDcs=FALSE);	//���������б� wht 11-07-31
	//�õ��������ʵ�弯�� wht 12-03-22
	void GetDimBoltInfoCmdAttachEntSet(CDrawingCommand *pMainCmd, ATOM_LIST<AcDbObjectId> &entIdList);
	//���ݹ�������õ���Ӧ�Ļ������� wht 12-03-22
	CDrawingCommand* GetDrawingPartCmdFromHandle(long hPart);
	//��(����ǰ�������)��˨��ע�������Ƴ�ָ����˨�������ع������� 
	void RemovBoltFromDimBoltInfoCmd(CDrawingCommand *pCurCmd,BOLTSET &boltSet,
									CHashList<CDrawingCommand*> &cmdHashList);
	//��ָ�������ƶ���Ŀ������֮��
	BOOL MoveSpecCmdAfterDestCmd(CDrawingCommand *pSpecCmd,CDrawingCommand *pDestCmd);
	//��ָ�������ƶ���Ŀ������֮ǰ
	BOOL MoveSpecCmdBeforeDestCmd(CDrawingCommand *pSpecCmd,CDrawingCommand *pDestCmd);
	//��ָ����Ŷ�Ӧ�����������Ŀ����Ŷ�Ӧ������֮��
	BOOL MoveSpecCmdSerialNoAfterDestCmdSerialNo(int specSerialNo,int destSerialNo);
	//
	void ReDimBoltInfo();
};

//���ͼ
class CPartDrawing : public CDrawingComponent
{
public:
	long m_hPart;
	CLDSPart *m_pPart;
	CLDSPolyPart polyPart;	//��ʱʹ�ò��洢
	BOOL m_bPolyPart;	
	CXhChar16 sPartNo;	//������ţ����ڸ��ݹ�������Զ����¹������
	ATOM_LIST<long> sonPoleHandleList;
public:
	CPartDrawing(int id=0,IDrawing* pDrawing=NULL);
	~CPartDrawing();
	
	void Create();			//������ͼ
	BOOL CreateCmdList(BOOL bInit=TRUE);//���ɻ������ͼ����
	void CopyProperty(CDrawingComponent* pCom);	//��������
	//
	void FromBuffer(CBuffer &buffer, long version=NULL);
	void ToBuffer(CBuffer &buffer);
};
//��׼��ͼ
class CStdView : public CDrawingComponent
{
public:
	STD_VIEW_TYPE m_eStdViewType;	//��׼��ͼ����
	CLDSPart *m_pPart;
public:
	CStdView(int id=0,IDrawing* pDrawing=NULL);
	~CStdView();

	void Create();			//������ͼ
	BOOL CreateCmdList(BOOL bInit=TRUE);		//������������
	void CopyProperty(CDrawingComponent* pCom);	//��������
	//
	void FromBuffer(CBuffer &buffer, long version=NULL);
	void ToBuffer(CBuffer &buffer);
};

class CDrawing : public CXeroDrawing
{
public:
	long feature;				//��ʱʹ�ò��洢
	SEGI m_nSegI;				//�����κ� -1��ʾ˾��ͼ,-2��ʾ��ͼ
	SEGI m_nAdjoinSegI;			//�����Σ�һ��Ϊ��ᵣֱ������������Σ���0��ʾ������������
	union{
		long liDrwPageSerial;		//ͬһ����ֶ�ͼֽ�е�����ţ�0 based serial)
		struct{
			short liPageSerialInAll;	//������ͼֽ�е���ţ�0 based serial)
			short liPageSerialInDrw;	//��ͬһ�ֶ�(��)ͼֽ�е�����ţ�0 based serial)
		};
	};
	long get_liDrwType() const{return m_nSegI.iSeg;}
	long set_liDrwType(long liDrawingType){return m_nSegI.iSeg=liDrawingType;}
	__declspec(property(put=set_liDrwType,get=get_liDrwType)) long liDrwType;	//ͼֽ��� -1��ʾ˾��ͼ,-2��ʾ��ͼ
	UCS_STRU dcs;				//����ϵ
	int m_nDrawingNumber;		//ͼֽ����
	MAP_FRAME_SIZE m_xMapFrm;	//ͼ��
	CXhChar50 m_sDrawingName;	//ͼֽ����
	CXhChar50 m_sDrawingNo;		//ͼ��
	BOOL m_bSpecifyMapFrmPos;	//ָ��ͼ��λ��
	long m_hActiveModule;		//��ǰ������߾��
	CLDSModule *m_pActiveModule;//��ǰ�������
	BOOL m_bAutoLayoutComponent;//�Զ�����
	double m_fComponentSpace;	//������ͼ֮��Ļ�ͼ��϶
	//��ͼ��ϣ����keyֵ����:
	//1.��ͼ��˾��ͼ  ����ͼ	key=pView->handle-abs(m_nSegI);
	//				  ����ͼ	key=pView->handle+pModule->handle;
	//				  ������ͼ	key=pView->handle;
	//				  ���		key=������� (1-15)
	//2.�ṹͼ	����ͼ			key=(pView->handle+m_nSegI)%16+16 (16-31)
	//			ͼ����ͼ�ṹͼ  key=pView->handle+m_nSegI;
	//			������ͼ�ṹͼ	key=pView->handle;
	//			���			key=������� (1-15) ����15ʱ���ܻ�����ظ�keyֵ
	//CSuperHashList<CDrawingComponent> hashListComponent;	//��ͼ�б�
	ATOM_LIST<CSectionViewSymbol> sectionViewSymbols;		//����ͼ��
	CDrawingRectArray drawingRectArr;	//�Ѳ��þ�������
public:
	CDrawing(int id=0);
	virtual ~CDrawing();
	CHashList<CLDSPartPtr> hashProcessedParts;	//���ڶ�һ��ͼֽˢ��ʱ����¼

	// ����ָ������ͼ��ͼֽ�����ɶ�Ӧ����ͼ
	// pView ָ����ͼ
	// pModule ����˾��ͼ����ͼʱ�ñ�����Ч,����ָ�����߷���
	// bFlag   �ṹͼ:��ʾ���ɵ���ͼ���ǽṹͼ ˾��ͼ\��ͼ:��ʾ������ͼ������ͼ
	CDrawingComponent *AddComponentByView(CDisplayView *pView,CLDSModule *pModule,BOOL bFlag=FALSE);
	CDrawing& operator = (const CDrawing &drawing);
	//void CopyProperty(CDrawing *pDrawing);
	void InitPartDrawing(CStruView *pStruView,CHashStrList<CLDSPart*>& part_list);
	static const long DARWING_MECHANICS	= 0xffffffff;	//-1����ѧ���˾��ͼ
	static const long DARWING_SUMMARY	= 0xfffffffe;	//-2��ʩ��ͼ��ͼ
	void InitDrawing(long liDrwTypeOrSegI);		//��ʼ��ͼֽ
	void CreateComponents();			//����ͼֽ�ڵ���ͼ
	void InsertMapFrm();				//����ͼ��
	void DimSectionViewSymbol();		//��ע����ͼ��
	f3dPoint GetMapFrmInsideTopLeft();	//�õ���׼�㼴ͼ���ڲ�������ϽǶ���
	void UpdateBOMTableInitPos(CDrawingBOM *pDrawingBOM);	//���������ɱ�����¼��㵱ǰ����ʼλ��
	CString MakeComponentSetStr();
	void UpdateSimpleSpecList();
	void UpdatePtrByHandle();			//ͨ���������ͼֽ��������ص�ָ��,������ٴδ��ļ�ʱʹ�� wht 11-07-22
	void UpdateCmdList();				//����ͼֽ������ͼԪ�������б�	wht 11-07-31
	void EraseRelaCadEnt();				//ɾ��������CADʵ��				wht 13-04-19
	//
	void ToBuffer(CBuffer &buffer);
	void FromBuffer(CBuffer &buffer, long version=NULL);
public:	//���ڼ��ٴ�������д�ļ��׷�װ����
	CDrawingComponent* AddComponent(DWORD key,int CLSTYPE_ID=0,bool loadDefaultObjInfo=true){
		return (CDrawingComponent*)listComponents.Add(key,CLSTYPE_ID,loadDefaultObjInfo);
	}
	CDrawingComponent* GetFirstDrawingComponent(){return (CDrawingComponent*)listComponents.GetFirst();}
	CDrawingComponent* GetNextDrawingComponent(){return (CDrawingComponent*)listComponents.GetNext();}
public:
	void ClearDrawnStates(int stateType=CDrawingComponent::TYPE_ROD);
	bool IsItemDrawn(int stateType,long idItem);	//����ָ����Ŀ�Ƿ��ڵ�ǰͼֽ�л��ƹ�
	bool IsItemDimmed(int stateType,long idItem);	//����ָ����Ŀ�Ƿ��ڵ�ǰͼֽ�б�ע��
	void MarkItemDrawnState(CDrawingComponent *pComponent,int stateType,long idItem,bool drawn=true);
	void MarkItemDimSpecState(CDrawingComponent *pComponent,int stateType,long idItem,bool dimmed=true);
};

//ͼֽ
extern CSuperHashList<CDrawing> drawingList; //�����ƶ�Ԫ��λ��
#endif
#endif // !defined(AFX_DRAWING_H__0EBE37F4_B612_4A73_9CB4_D45B171C1F29__INCLUDED_)
