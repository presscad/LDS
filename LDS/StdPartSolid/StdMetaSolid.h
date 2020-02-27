#pragma once
#include <WinDef.h>
#include "SolidBody.h"

//��ȡԲ����������
void GetAngleArcVertex(double dfChordL,double r,BOOL isOn,f3dPoint vertex_arr[],int slices);
struct METASOLID{
public:
	struct PROFVERTEX{
		bool blSkipEdge;	//���������ɴ˶���Ϊ����������
		char ciEdgeType;	//0:��ֱͨ��1:ָ��Բ���н�2:ָ��Բ���뾶3:��Բ��
		short snArcSlices;	//>0:��ʾԲ����ɢ�ɶ��۱���Ŀ��<0:��ʾָ����ɢ����
		GEPOINT pos;		//�����λ������
		double cx,cy;		//Բ�ĵ�����
		union{
			double dfArcRadius;	//��Բ������Ҫ�Ĳ���
			double dfSectAngle;	//��׼Բ������Ҫ�Ĳ���
		};
		GEPOINT vColNormal;	//��Բ������Ҫ�Ĳ���
		PROFVERTEX(){memset(this,0,sizeof(PROFVERTEX));}
	};
public:
	//parrVertexes����sizeof(f3dPoint)=2*sizeof(COORD3D)=2*sizeof(GEPOINT)���ʴ˴�parrVertexes��������Ϊf3dPoint[] wjh-2019.12.27
	static UINT CirclePolygonize(double radius, COORD3D* parrVertexes, UINT uiMaxCount,bool blIntelliCalSlices=true);
	//��һ��Բ�����ָ������������
	static int AppendSamplePointsToCirclePolyVertex(GEPOINT* parrSamplePoints, int nMaxSamplePointsCount, GEPOINT* parrCirPolyVertexes, int nCurrCirPolyVertexesCount, double TOLERANCE_ALFA);
	//��pBody���ʵ��Բ��Ԫʵ��
	static int AppendDiskSolid(fBody* pBody,double d,double H,GECS& acs,int slices,COLORREF crLs);
	//��pBody��ӿ���Բ��/��Ȧ/��Ͳ��Ԫʵ��,����DΪ�⾶
	static int AppendTubeSolid(fBody* pBody,double D,double t,double H,GECS& acs,int slices,COLORREF crColor);
	//��pBody���Բ��ʵ��,RԲ���⾶��rԲ���ھ���fAngle���Ƕ�(0~2Pi)
	static int AppendRingSolid(fBody* pBody, double R, double r, double fArcAngle, GECS& acs, int slices, COLORREF crColor);
	//��pBody��Ӻ����浹����������ĸԪʵ��
	static int AppendTaperNutSolid(fBody* pBody,double De,double Ds,double H,GECS& acs,int slices,COLORREF crLs);
	//��pBody��Ӽ�������ĸ�����������浹����Ԫʵ��
	static const BYTE NUT_INC_TOP  = 0x01;
	static const BYTE NUT_INC_BTM  = 0x02;
	static const BYTE NUT_INC_SIDE = 0x04;
	static int AppendHexPolygonNutSolid(fBody* pBody,double De,double H,GECS& acs,COLORREF crLs,DWORD dwFaceFlag=0);
	//������ָ������Σ���Բ���ߣ������γɵı�׼ʵ��
	static int AppendPolygonExtrudeSolid(fBody* pBody,GECS& acs,double dfExtrudeLength,PROFVERTEX* xarrProfVertexes,int niCount,COLORREF crColor);
	//����ָ���Ǹ�֫��������Ŀ��ϱ�����ʵ��
	struct KAIHEINFO {
		char   ciOperLeg;		//��ǰ���ϲ����ĽǸ�֫,ȡֵ 0/'X',1/'Y'
		double dfOpenRadAngle;	//>0��ʾ���ǣ�<0�ϽǶ���������ֵ
		double dfDeformedLenOfStart,dfDeformedLenOfEnd;	//����������ֹ����ı���������
		double dfZlenStart,dfZlenEnd;	//���Ͻǳ��ȷ�����ֹZֵ
		double dfLegWidth,dfLegThick;	//����֫�Ŀ������
		double dfReservedWidth;			//���Ͻ�֫����ֵ��֫��������������Ŀ�ȣ�һ��Ϊ֫��+2R/3,ȡֵ>=֫��
		KAIHEINFO() { memset(this,0,sizeof(KAIHEINFO)); }
		bool CalMidFlatVertexes(COORD3D xarrVertexes[14]) const;
		bool CalKaiheVertexes(COORD3D xarrVertexes[12]) const;
	};
	static int AppendMidFlatAngleWing(fBody* pBody,GECS& acs,const KAIHEINFO* pDefKaiheInfo,COLORREF crLegPlane=0,COLORREF crTipEdge=0);
	static int AppendKaiheAngleWing(fBody* pBody,GECS& acs,const KAIHEINFO* pDefKaiheInfo,COLORREF crLegPlane=0,COLORREF crTipEdge=0);
	static bool InitializeArcEdgeline(f3dAtomLine* pAtomLine,PROFVERTEX* pStart,PROFVERTEX* pEnd,bool bByStartVertex);
	//�����ض���
	static BOOL AppendPolyHole(GECS &hole_ucs, PROFVERTEX* xarrVertex, int nVertexCount,
		f3dPolyFace *pUpFace, f3dPolyFace *pDownFace, fBody *pBody, COLORREF crLsHole);
};