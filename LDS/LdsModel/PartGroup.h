#ifndef __PART_GROUP_H_
#define __PART_GROUP_H_

#include "lds_part.h"

struct JOINTRAYTUBE	: public ICompareClass 
{
	THANDLE hRayTube;
	double fDist;
	virtual int Compare(const ICompareClass *pCompareObj)const{
		JOINTRAYTUBE* pCopareTube=(JOINTRAYTUBE*)pCompareObj;
		return this->fDist>pCopareTube->fDist;
	}
};
//
struct SECTION_PART{
	CLDSPart *pPart;
	f3dLine prjLine;
	BOOL m_bAddToView;
	SECTION_PART(){pPart=NULL;m_bAddToView=FALSE;}
};
//�ڵ�ڼ�
struct SECTION_SEG{
	const static int SECTION_SPACE_THRESHOLD	= 500;	//�ڵ���������ֵ
	//
	const static int SECTION_TYPE_NODEPLATE		= 0x0;	//0x00.�ڵ����ͼ 
	const static int SECTION_TYPE_TUBERIBPLATE	= 0x01;	//0x01.��Խ������ͼ 
	const static int SECTION_TYPE_CIRCOVERPLATE	= 0x02;	//0x02.��������ͼ 
	const static int SECTION_TYPE_CIRRIBPLATE	= 0x04;	//0x04.�����߰���ͼ 
	const static int SECTION_TYPE_CIRPLATE		= 0x08;	//0x08.����������ͼ
	//
	BYTE m_cSectionType;	
	SECTION_PART *m_pStartPart;
	SECTION_PART *m_pEndPart;
	SECTION_PART *m_pDatumPart;
	//ATOM_LIST<SECTION_PART> sectionPartList;
	CHashList<SECTION_PART> sectionPartList;
public:
	SECTION_SEG();
	~SECTION_SEG(){;}
	f3dPoint GetSectionDatumPos(){return f3dPoint();}
	BOOL IsIncludeSectionPart(SECTION_PART *pPart);
	int StatSectionPartCount(BOOL bOnlyNoneToView=TRUE);
};
//����ƽ��
class CPartGroupWorkPlane{
public:
	long iNo,iMatchView;
	CXhChar50 sName;
	CLDSPart *pDatumPart;
	f3dPoint workPlaneNorm,workPlaneOrg;
	CParaCS::PARAMETRIC_INFO csParaInfo;
	int nRayTubeNum,nPartNum;	//���������ȼ����ж�����
	bool bJointTubePlane;		//�ù�����Ϊ���ֹܶԽӺ��ӣ������ǽǷ���Ҳ������ָ��ƽ�棩
	GEPOINT vTempAxisY;			//��ʱ��¼��չ������չ����
	SECTION_PART* pTempSectionParts;
	CDrawingAssistant assistant,assistant2;	//assistant2Ϊ���������ƹ��湤������ģ���һ���ƹ��棨��ֱ���Ĺ�����
public:
	CPartGroupWorkPlane();
	~CPartGroupWorkPlane(){;}
	void SetKey(DWORD dwKey){iNo=(long)dwKey;}
	f3dPoint CalYAxisVec(CLDSLineTube* pRootTube=NULL);
};
struct DIFF_PLANE_ERROR_LOG
{
	long iPlaneNo;		//��Ӧ�Ĺ���ƽ��
	double diff;		//�ǶȲ���
	CXhChar100 error;	//������Ϣ
};
//
struct CIRPLATE_ITEM{
	f3dPoint m_xPrjPt;
	double m_fPrjLen;
	CLDSPart *m_pPart;
	POLYGONEX m_xRegion;
	CIRPLATE_ITEM(){m_pPart=NULL;m_fPrjLen=0;}
};
//�ֲ㵥Ԫ
class CLayerUnit{
	f3dPoint prj_pt;	//
public:
	BOOL m_bNeed;		//
	ATOM_LIST<CIRPLATE_ITEM> m_xPartSet;
public:
	CLayerUnit();
	~CLayerUnit(){}
	void SetProjectionPt(f3dPoint perp){prj_pt=perp;}
	bool IsInSameLayer(f3dPoint perp){return prj_pt.IsEqual(perp,0.5);}
	BOOL IsOverLapped(CLayerUnit* pUnit,double DECISION=1);
	BOOL IsCirCoverPlateLayer();
};
//�ֲ���
class CLayerGroup{
public:
	BOOL m_bFinish;
	ATOM_LIST<CLayerUnit*> m_xLayerUnitPtrSet;
public:
	CLayerGroup();
	~CLayerGroup(){}
	BOOL IsOverlapped(CLayerUnit* pOtherUnit);
	BOOL IsCirCoverPlateGroup();
	BOOL GetGroupName(CXhChar50& sName);
};
//
struct WELD_PLATES{
	static const BYTE WITH_NONEHOLE_PLATE	= 1;//���������߰�
	static const BYTE WITH_HOLE_PLATE		= 2;//����������Ҳ����˨�ף�����Ϊ����壬Ҳ����Ϊ������)
	static const BYTE WITH_BUDDY_BENDPLATE	= 3;//ͬΪ�����壨������˨�ף����һ����߹���
	static const BYTE WITH_TRIPPLE_PLATES	= 4;//����ͼ�麸��
	char cType;
public:
	CLDSPlate* pRootPlate;
	struct{
		CLDSGeneralPlate* pBuddyPlate;	//������ͼ��λ�ְ�
		CLDSGeneralPlate* pSectPlate;	//����������ְ�����߽����ϵĺ��Ӹְ�
	};
	WELD_PLATES(){memset(this,0,sizeof(WELD_PLATES));}
};

//
class CTubeBushPlateView : public CSectionView
{	//���ڰ�װ�ĳ��а���ͼ(�������Ժ��ֹܽ�����Ϊ��ͼƽ��)
public:
	//long m_hRootTube;			//��׼�ֹܣ����̲���ʱ�Խϳ���Ϊ��׼�ֹܣ�
	//long m_hBushPlate;		//���а�
	CTubeBushPlateView(int id=0);
};
#endif