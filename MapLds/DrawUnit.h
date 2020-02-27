// DrawUnit.h: interface for the CDrawUnit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWUNIT_H__A9AC6807_68A8_4062_910D_1834A20E79DF__INCLUDED_)
#define AFX_DRAWUNIT_H__A9AC6807_68A8_4062_910D_1834A20E79DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ptr_list.h"
#include "ldsptr_list.h"
#include "XhCharString.h"
#include "RxTools.h"
#include "SysPara.h"
#include "DrawDim.h"
#include "MemberProperty.h"

//wbt:CTextFrame��CDimStyle��CTextOnlyDrawing��CSizeTextDim��CZhunXianTextDim��CKaiHeAngleDim�Ѿ��Ƶ�DrawDim.h��
//CSizeAngleDim ����DrawDim.h�� wht 18-08-23
struct PAGE_INDEX{
//protected:
	long _liPage;
public:
	BYTE ciPagesOfSerial;	//��ǰ��Ű�������ҳ��
	BYTE ciIndexInSerial;	//��ǰҳ�ڵ�ǰ����е�������ţ�0Ϊ��ʼ�������
	WORD wiSerial;			//��ǰ���տ�����ţ�0Ϊ��ʼ���
	long get_liPage();
	__declspec(property(get=get_liPage)) long liPage;			//��ǰҳ�ڵ�ǰ����е���ҳ����ţ�0Ϊ��ʼҳ�����
	PAGE_INDEX(){_liPage=wiSerial=ciIndexInSerial=0;ciPagesOfSerial=1;}
	void IncrementSerial();
	void IncrementLiPage();
};

class CDrawingRect
{
	friend class CDrawingRectLayout;
	friend class CDrawingRectArray;
	BOOL m_bLayout;			//�ڲ��Ż�����ʱ���ڱ�ʶ�Ƿ��Ѿ�����λ��
	f3dPoint m_PrevTopLeft;	//�ڲ��Ż�����ʱ���ڴ洢ǰһ���ַ��������Ͻ�λ��,�Ա��ڻع�����
public:
	int m_iPageIndex;		//0Ϊ��ʼҳ������
	f3dPoint topLeft;		//���ݾ������²��ֺ�����Ͻ�����λ��
	f3dPoint initTopLeft;	//�����²���ǰ��ͼ���ݾ��εĳ�ʼ���Ͻ����꣬���������²��ֺ�topLeft����ֵһ��ȷ�����ݾ��ε�ƫ����
	double width,height;	//��ͼ���ƾ��εĿ�Ⱥ͸߶�
	//ָ�������CSectionViewDrawing����
	//�޹�������ʱ��ʾ�þ���Ϊ�̶�����
	void *m_pDrawing;
	//
	CDrawingRect(){m_pDrawing=NULL;m_iPageIndex=0;m_bLayout=FALSE;}
	BOOL IsLayout(){return m_bLayout;}
};
struct SEGREC{SEGI iSeg;int nPageCount;};
class CDrawingRectLayout
{
	double m_fFrameWidth;	//�߿���
	double m_fFrameHeight;	//�߿�߶�
	double m_fBlkZoomcoef;	//ԭʼͼԪ�ߴ�/����ͼԪ�ߴ�
	bool IsDefiniteNotEnoughSpace(double width,double height,bool canExtendByX=false,bool canExtendByY=false);
	BOOL GetBlankRectTopLeft(double width,double height,f3dPoint *topLeft,double marginSpace,bool bSearchHori=true);
	BOOL GetBlankRectTopLeftTemp(CArray<double,double&>&startXArray,double width,double height,f3dPoint *topLeft);
	CArray<double,double&>m_arrRowHeight;	//
	//����ʵ�ʻ�ͼ�����С�������û�ͼ���������λ��
	void ResetDrawingRectPosition();	
public:
	PRESET_ARRAY4<GEPOINT2D> arrOrgOfSubpages;
	bool m_bAutoZoomFixRect;//�涯�������Ź̶�λ�þ���
	bool m_bPlaceFixRect;	//�̶������ȸ߶Ƚ�����ռλ����ҳʱ���ڵ�������ͼ����һ�� wht 18-08-03
	double m_fFixRectWidth;	//�̶�λ�þ��ο�
	double m_fFixRectHeight;//�̶�λ�þ��θ�
	f3dPoint m_FixRectTopLeft;
	DIMALIGN_TYPE fixRectAlign;	//�̶�λ�þ��εĶ�λ��ʽ
	CDrawingRectLayout();
	CArray<CDrawingRect,CDrawingRect&> drawRectArr;
	void Relayout(double h_to_w,double marginSpace);
	void Relayout3ViewsPartGroup(double h_to_w,double marginSpace);
	void AdjustTubeOneFrontViewPartGroupLayout(double marginSpace,double limitWidth=0,double maxHeight=0);
	bool RelayoutTubeCommonPartGroup(double h_to_w,double marginSpace,double limitWidth=0,double *pfPageCount=NULL,bool bZoomRelayout=false);
	void RelayoutPlateCommonPartGroup(double h_to_w,double marginSpace);
#ifdef __COMMON_PART_INC_
	void RelayoutSphereGroup(double h_to_w,double marginSpace);
#endif
	BOOL RelayoutTemp(double hight=1000,double width=10000000000000000);
	BOOL RelayoutPlateDraw(double marginSpace,DYN_ARRAY<SEGREC>& segarr);
	BOOL RelayoutDraw(double marginSpace);
	double GetFrameWidth(){return m_fFrameWidth;}
	double GetFrameHeight(){return m_fFrameHeight;}
	double GetRealDrawingHeight();
};

//���ڼ�¼ͼ�����
class CDrawingRectArray
{
	double m_fFrameWidth;	//�߿���
	double m_fFrameHeight;	//�߿�߶�
	double m_fMarginSpace;	//���
	CArray<CDrawingRect,CDrawingRect&> drawRectArr;
public:
	CDrawingRectArray();
	CDrawingRectArray& operator = (CDrawingRectArray &drawRectArr);
	void CopyProperty(CDrawingRectArray *pDrawingRectArray);
	void InitDrawingRectArray(double width,double height,double marginSpace);
	int AddUsedDrawingRect(f2dPoint topLeft,double width,double height);
	void EmptyDrawRectArray(){drawRectArr.RemoveAll();}
	void RemoveDrawRectAt(int i){drawRectArr.RemoveAt(i);}
	int GetDrawRectArrSize(){return (int)drawRectArr.GetSize();}
	CDrawingRect GetDrawRectAt(int i){return drawRectArr.GetAt(i);}
	BOOL GetBlankRectTopLeft(double width,double height,f3dPoint *topLeft,BOOL bSearchHori=TRUE);
};

#ifndef __TSA_FILE_
//��������������
extern LINEPARTSET OUTSIDEJGSETDRAW;
extern LINEPARTSET INSIDEJGSETDRAW;
void DrawAngleToMap(LINEPARTSET &partset,BOOL bInside,AcDbBlockTableRecord *pBlockTableRecord,
					CDrawCoordTrans dcs, BOOL bSetupDrawing,AcDbObjectId *pOutEntId=NULL);
class CPartDrawUnit
{
public:
	CLDSPart *pPart;
	f3dLine m_xRotateAxis;	//չ��ʱ����ת����
	double m_fRotateAngle;	//����ʱ����ת�Ƕ�,������
	CPartDrawUnit(){pPart=NULL;m_fRotateAngle=0;}
};
class CLineAngleDrawUnit : public CPartDrawUnit
{
public:
};
class CPlateDrawUnit : public CPartDrawUnit
{
public:
	CLDSPlate *pPlate;
	int front0_side1;	//������˨���ǲ�����˨
	CPlateDrawUnit(){pPlate=NULL;front0_side1=0;}
};
extern ATOM_LIST<CPlateDrawUnit>PLATESETDRAW;
class CLsDrawUnit
{
public:
	CLDSBolt *pLs;
	BOOL bOriginal;		//�Ƿ�������˨
	bool bDimHoleDiameter;
	bool bDrawSideLine;	//ƽ����˨����Ϊ�ȳ�ֱ��
	int front0_side1;	//������˨���ǲ�����˨
	f3dPoint pos;		//��˨��עλ��
	f3dPoint work_norm;	//��ͼͶӰ����
	f3dLine spreadLine;	//չ���� wht 10-12-10
	double rotate_angle;//��ת�Ƕȴ���0ʱ��Ҫ��ת����
	CLsDrawUnit(){pLs=NULL;bOriginal=TRUE;front0_side1=0;rotate_angle=0;bDrawSideLine=false;bDimHoleDiameter=false;}
};
class CLsListDraw
{
	ATOM_LIST<CLsDrawUnit> drawls;
public:
	CLsListDraw(){;}
	CLsDrawUnit *AppendLsRef(CLDSBolt *pBolt, BOOL bOriginal,f3dPoint view_norm,
		f3dLine *pSpreadLine=NULL,double rotate_angle=0);
	BOOL DelLsRef(long handle);
	void EmptyDrawList();
	CLsDrawUnit* GetFirst();
	CLsDrawUnit* GetNext();
	void PushStack();
	void PopStack();
	void DrawToMap(AcDbBlockTableRecord *pBlockTableRecord,UCS_STRU &ucs,double scale);
	void InsertPadPoint(AcDbBlockTableRecord *pBlockTableRecord,CLsDrawUnit *pLsDraw,
		f3dPoint pos,AcDbObjectId *pOutEntId=NULL);
	//���Ƶ�����˨
	void DrawBolt(AcDbBlockTableRecord *pBlockTableRecord,CLsDrawUnit *pLsDraw,CDrawCoordTrans dcs,AcDbObjectId *pOutEntId=NULL);
};
extern CLsListDraw LSDRAW;
class CLsSpaceDimUnit
{
public:
	long hFatherPlate;		//��˨�����ְ��� wht 11-07-23
	long hStartLs,hEndLs;
	AcDbObjectId StartLsId,EndLsId;
	double dist;
	BOOL bCalDimAngle;
	double dim_angle;
	int nLsSpaceNum;		//��������˨�����
	BOOL bMergeDimLsSpace;	//�ϲ���ע������˨���
	CLDSLinePart* GetDatumRod(BYTE dimvec0_plate1=0);
	CLsSpaceDimUnit(){hStartLs=hEndLs=0;dist=0.0;bCalDimAngle=TRUE;dim_angle=0;nLsSpaceNum=1;bMergeDimLsSpace=TRUE;}
};
extern ATOM_LIST<CLsSpaceDimUnit> LSSPACEDIM;
void DimLsSpaceToMap(AcDbBlockTableRecord *pBlockTableRecord,UCS_STRU &ucs);
//��˨���ע��Ԫ
enum LSGROUP_POS{GROUP_MID=0,GROUP_START,GROUP_END};	//��˨����λ��
class CLsGroupDimUnit
{
public:
	struct BOLTINFO{
	private:
		CLDSBolt *pBolt;
	public:
		long hBolt;
		f2dPoint pos2d;
		BOLTINFO(){hBolt=0;pBolt=NULL;}
		BOLTINFO(long boltHandle){hBolt=boltHandle;pBolt=NULL;}
		BOLTINFO(CLDSBolt *boltPtr){hBolt=boltPtr->handle;pBolt=boltPtr;}
		CLDSBolt *GetBolt()
		{
			if(pBolt==NULL)
				pBolt=(CLDSBolt*)Ta.FromPartHandle(hBolt,CLS_BOLT);
			return pBolt;
		}
		long GetBoltD()
		{
			if(pBolt==NULL)
				pBolt=(CLDSBolt*)Ta.FromPartHandle(hBolt,CLS_BOLT);
			if(pBolt)
				return pBolt->get_d();
			return 0;
		}
		long GetBoltL()
		{
			if(pBolt==NULL)
				pBolt=(CLDSBolt*)Ta.FromPartHandle(hBolt,CLS_BOLT);
			if(pBolt)
				return ftoi(pBolt->get_L());
			return 0;
		}
	};
private:
	long hDatumRod;
	long hDatumBolt;
	long d,L;	//��˨���ֱ������Ч��,���Լ�����
	CHashList<BOLTINFO> hashBoltsByH;
	//
	CLDSBolt *pDatumBolt;
	CLDSLinePart *pDatumRod;
	//
	//BOOL m_bInitAxisXByRod;	//�Ƿ��Ѿ����ݻ�׼�˼���ʼ������ϵ����
	//��ʼ����˨���׼�˼�
	void InitDatumRod(CLDSBolt *pBolt);
	//��ȡ��˨��ע�����׼�˼�
	static CLDSLinePart* GetDimVecDatumRod(CLDSBolt *pBolt);
public:
	UCS_STRU ucs;			//��˨������ϵ
	SCOPE_STRU group_scope;	//��˨����������
	CLsGroupDimUnit()
	{
		hDatumRod=hDatumBolt=0;
		pDatumBolt=NULL;
		pDatumRod=NULL;
		hashBoltsByH.SetHashTableGrowSize(10);	//Ĭ����˨������
		d=L=0;
		//m_bHasGroupUcs=FALSE;
	}
	long GetBoltD(){return d;}
	long GetBoltL(){return L;}
	void SetDatumRod(CLDSLinePart *pRod);
	f3dPoint TransToLocalCoord(f3dPoint pos);
	void GetDimText(char *sDimText);
	void FromBuffer(CBuffer &buffer, long version=NULL);
	void ToBuffer(CBuffer &buffer);
	//
	void Init(CLDSBolt *pBolt);			//��ʼ����˨����Ϣ
	bool Init(CLDSPlate *pPlate,BOLTSET &boltSet);
	bool IsPlateLsGroup();
	void AddBolt(BOLTINFO boltInfo);	//�����˨����˨��
	BOOL RemoveBolt(CLDSBolt *pBolt);	//�Ƴ�ָ����˨,�������Ƿ��Ƴ��ɹ�
	BOOL IsIncludeBolt(BOLTINFO boltInfo);//�ж���˨�Ƿ����ڵ�ǰ��
	void CopyProperty(CLsGroupDimUnit *pLsGroupDim);
	CLDSBolt *GetDatumBolt();
	CLDSLinePart *GetDatumRod();
	BOLTINFO *GetFirstBoltInfo() { return hashBoltsByH.GetFirst(); }
	BOLTINFO *GetNxetBoltInfo() { return hashBoltsByH.GetNext(); }
	int GetBoltNum() { return hashBoltsByH.GetNodeNum(); }
	BOOL ContainsBolt(long hBolt) { return hashBoltsByH.GetValue(hBolt)!=NULL;}
	//�ж���˨���ڻ�׼�˼�ʼ�ˡ��ն˻����м�,�Զ�����λ��ʱ�����жϲ��ҷ���
	LSGROUP_POS GetLsGroupPos(f3dPoint base_pos,AcDbLine *pDatumLine,double fScale);
	CXhChar500 GetBoltIdKey();

	static double fLsSpaceVerticalThreshold;	//��˨���ˮƽ������ֵ
	static double fLsSpaceHorizontalThreshold;	//��˨�����ֱ������ֵ
};
#endif

typedef struct
{
	double wide;    //
	double thick;
	char cMark;		//����ʶ��
	char spec[50];	//���
	int  N;			//�˹��˼�����
}SPEC_MARK;

class CDimPartSpec
{
public:
	CXhChar16 sPartNo;
	CXhChar16 sBackPartNo;
	CXhChar50 guige,sLength;
	long hPart;
	double text_angle;
	int cls_typeid; 
	long iNo;				//��ӦTSA�ڵ���
	f3dPoint searchDirect;	//����ϽǸ��ӽǸ�Ӧָ����ű�ע����������,Ĭ�Ͽ�ʸ�������ʾ��ָ��
	//
	BOOL bDimPartNoOnly;	//����ע�������
	BOOL m_bDimRefLineOnly;	//ֻ��ͼ������
	BOOL bReverseLsInfo;	//TSA��ͼʹ��
	f3dPoint base_pos,dim_pos;
	//�¼ӱ��� wht 10-12-03
	int  bAutoSearchDimPos;	//�Զ����ұ�עλ��,�޺���λ��ʱҪ���û�ָ��,������ʱֱ��ʹ���û�ָ���ı�עλ��
	BOOL bDimJgLength;		//��ע�Ǹֳ���
	BOOL bDrawRefLine;		//�Ƿ����������
	double fPoleDimPosCoef;	//�˼���עλ��ϵ�� //LTMA 0.3  TAS 0.5
	//ʹ�û�ͼ�ű���ע���ʱʹ��
	double fBasePosOffsetX;	//���ݵ�λ��X����ƫ����	wht 11-07-25
	double fBasePosOffsetY;	//���ݵ�λ��Y����ƫ����
	double fDimPosOffsetX;	//��עλ��X�᷽��ƫ����
	double fDimPosOffsetY;	//��עλ��Y�᷽��ƫ����
	double fGuigeOffsetX;	//����עλ��X�᷽��ƫ����
	double fGuigeOffsetY;	//����עλ��Y�᷽��ƫ����
	//�˼�������X��ļнǣ���ע��������׼���ʱ���ڵ�������ظ˼������ע wht 12-03-22
	double rod_angle;		//�˼�������X��ļн�
	f3dPoint rod_vec;		//�˼�����,���ƴ��Ƕ���Բ���λ����ʱʹ��
	int iMirMsg;			//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
	long hMirPart;			//�Գƹ���������ж�Y��Գƹ�����������ʱ��Ҫ���ݾ���ҵ�λ�� wht 12-08-19
	BOOL bLeftPart;			//TRUE:��ǰ����Ϊ�󹹼�,�Գƹ���Ϊ�ҹ��� FALSE:��֮ 
	CDimPartSpec()
	{
		memset(this,'\0',sizeof(CDimPartSpec));
		bAutoSearchDimPos=TRUE;	//Ĭ���Զ�����������עλ��
		bDimJgLength=sys.stru_map.bDimStruJgLen;
		rod_angle=0;
		iMirMsg=-1;
		hMirPart=0;
		bLeftPart=TRUE;
#if defined(__TSA_FILE_)
		fPoleDimPosCoef=0.5;
#else
		fPoleDimPosCoef=0.3;
#endif
	}
};
class CDimPartSpecList
{
	ATOM_LIST<CDimPartSpec> dimpart;
	int back_mir_style;	//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
	CDimPartSpec *AppendNodeSpec(CLDSNode *pNode,f3dPoint dim_pos,BOOL bDimByAnalysisMode,BOOL bDrawRefLine);	
	CDimPartSpec *AppendLinePartSpec(CLDSLinePart *pLinePart,f3dPoint dim_pos,BOOL bDimByAnalysisMode,
									BOOL bDrawRefLine,double angle);
#ifndef __TSA_FILE_
	CDimPartSpec *AppendPlatePartSpec(CLDSPlate *pPlank,f3dPoint dim_pos,BOOL bDrawRefLine);
	CDimPartSpec *AppendParamPlatePartSpec(CLDSParamPlate *pPlate,f3dPoint dim_pos,BOOL bDrawRefLine);
#endif
	void DimPartSpec(AcDbBlockTableRecord *pBlockTableRecord,CDimPartSpec* pPartSpec, CTextFrame* pTextFrame);
public:
	ATOM_LIST<SPEC_MARK> SpecList;	//�򻯹��ʱʹ��
public:
	CDimPartSpec *AppendPartSpec(CLDSDbObject *pDbObj,f3dPoint dim_pos,BOOL bDimByAnalysisMode,
								BOOL bDrawRefLine=FALSE,double angle=0);
	CDimPartSpecList() : m_bMergeNearSampePartNoDim(TRUE)
	{back_mir_style=-1;}
	void SetBackMirStyle(int AXISX0_AXISY1);
	int BackMirStyle();	//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
	BOOL DelPartSpecDim(long handle);
	void EmptyPartSpecDim();
	CDimPartSpec* GetFirst();
	CDimPartSpec* GetNext();
	void PushStack();
	void PopStack();
	void DimToMap(AcDbBlockTableRecord *pBlockTableRecord,BOOL bDimByAnalysisMode=FALSE);
	// �ϲ�λ�ý��ڵ�ͬ��ű�ע
	BOOL m_bMergeNearSampePartNoDim;
	// ��������עָ������ wht 10-11-29
	void AnalyseAndDimPartSpec(AcDbBlockTableRecord *pBlockTableRecord,
		CDimPartSpec *pPartSpec,BOOL bDimByAnalysisMode,bool bDimStartPartSpec=false);
#ifdef __SMART_DRAW_INC_
	//���ݼ�¼�����ݱ�ע������ϸ
	void AutoDimPartSpec(AcDbBlockTableRecord *pBlockTableRecord,CDimPartSpec *pPartSpec,
		BOOL bDimByAnalysisMode,BOOL bSelectPos=FALSE,bool bDimStartPartSpec=false);
#endif
};

//��������
//1.��XYƽ��Ϊ�����
//	Z>0��ʾ��γ��γ�ȷ�ΧΪ(0~90)
//	Z<0��ʾ��γ��γ�ȷ�ΧΪ(0~-90)
//2.��XZƽ��X+Ϊ���������߼�����Ϊ0
//	Y+����Ϊ����(0~180)
//	Y-����Ϊ����(0~-180)
struct SPHERE_COORD{
	double longitude;	//����
	double latitude;	//γ��
	//
	SPHERE_COORD(){memset(this,0,sizeof(SPHERE_COORD));}
	SPHERE_COORD(double fLongitude,double fLatitude)
	{
		longitude=fLongitude;
		latitude=fLatitude;
	}
	BOOL IsEqual(SPHERE_COORD sphereCoord);
	CXhChar200 ToDegreeString();	//�Ƕ����ַ���
	CXhChar200 ToRadianString();	//�������ַ���
	//ֱ������ת��Ϊ��γ������
	static SPHERE_COORD TransToSphereCoord(f3dPoint pt);
	//��γ������ת��Ϊֱ������
	static f3dPoint TransToRectangularCoord(SPHERE_COORD sphereCoord,double R);
};
/*ֱ�������󽻵�
  return value:
	0:ֱ�������޽���
	1:�������غ�Ϊһ���е�
	2:�ҵ�������Ч����
	-1:ֱ�߻���������
*/
int Int2dls(f3dPoint &inters1,f3dPoint &inters2,f3dLine line,double sphere_r);
//
extern CDimPartSpecList dimpartspec;
#endif // !defined(AFX_DRAWUNIT_H__A9AC6807_68A8_4062_910D_1834A20E79DF__INCLUDED_)
