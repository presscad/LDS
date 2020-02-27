#pragma once

#ifndef __TSA_FILE_
#include "list.h"
#include "Hashtable.h"
#include "PlankDrawing.h"
//
struct IDimPlateBolts{
	struct BOLT {
		short siReserve;	//���ò���
		GEPOINT location;
		GEPOINT adjustPt;	//
		BOLT(const double* pos=NULL) {
			if (pos != NULL)
				location.Set(pos[0], pos[1], pos[2]);
			siReserve = 0; 
		};
		BOLT(const double* pos, short reserved) {
			location.Set(pos[0], pos[1], pos[2]);
			siReserve = reserved;
		}
	};
	struct BOLTRAY {
		int iRayNo, featrue;
		//bIsDoubleLsRayĿǰ����CDimPlankLs������,δ��Ӧͳһ��CPlankDrawing��ȥ�� wjh-2015.2.27
		bool bIsDoubleLsRay;//�Ƿ�Ϊ˫����˨���� wht 11-06-30
		bool bDatumLsRay;	//�Ƿ�Ϊ��׼��˨����
		GEPOINT rayOrg;			//���ߵ���ʼ��
		GEPOINT rayResideVec;	//��������פ�湹�������߷���(����˨���޹�,һ��Ϊ������˼������߷���) wjh-2015.2.27
		ITAG* pTag;
		ARRAY_LIST<BOLT> ls_arr;
	public:
		BOLTRAY() {
			featrue = 0; bIsDoubleLsRay = false; pTag = NULL; bDatumLsRay = false; ls_arr.SetSize(0, 50);
		}
		~BOLTRAY(){
			if(pTag)
				pTag->DetachObject();
		}
		void SetKey(DWORD key) { iRayNo = (int)key; }
		void SetTagInfo(ITAG* pTagInfo) { pTag = pTagInfo; }
		void AppendBoltLoca(BOLT& bolt);
		void CloneMember(BOLTRAY* pSrcLsRay, BOOL bCopyLsArr = TRUE)
		{
			iRayNo = pSrcLsRay->iRayNo;
			rayOrg = pSrcLsRay->rayOrg;
			rayResideVec = pSrcLsRay->rayResideVec;
			bIsDoubleLsRay = pSrcLsRay->bIsDoubleLsRay;
			bDatumLsRay = pSrcLsRay->bDatumLsRay;
			if (bCopyLsArr)
			{
				for (BOLT* pBolt = pSrcLsRay->ls_arr.GetFirst(); pBolt; pBolt = pSrcLsRay->ls_arr.GetNext())
					ls_arr.append(*pBolt);
			}
		}
	};
	struct BOLTINFO_TAG : public TAG_INFO<BOLTRAY> {
		//int iRayNo;		//���ߺ�
		int d;			//��˨ֱ��
		int L;			//��˨��Ч��
		int N;			//��˨����
		double grade;	//��˨����
		BOLTINFO_TAG() { d = 16; L = N = 0; grade = 6.8; }
	};
	struct BOLTRAY_PARA {
		bool reverseLenvec;
		long hRod;
		double coord_x, coord_y;
		BOLTRAY_PARA(long rod = 0, double coordx = 0, double coordy = 0)
		{
			reverseLenvec = false;
			hRod = rod;
			coord_x = coordx;
			coord_y = coordy;
		}
	};
public:
	//��pItem�Ѿ��йҽӶ���ʱִ��TagInfo(...), ���򴴽��¹ҽ����ݶ���󷵻�
	BOLTINFO_TAG* LoadTagInfo(BOLTRAY* pItem);
	virtual BOLTINFO_TAG* AppendTagToList() = 0;
};
class CDimPlateBolts : public IDimPlateBolts
{
	struct INTERS_POINT {
		int nNum;
		GEPOINT xIntersPoint;
		INTERS_POINT() { nNum = 0; }
	};
private:
	CLDSGeneralPlate* m_pPlate;
	CDrawCoordTrans dcs;
	UCS_STRU* m_pAdjustCs;		//����У׼��˨��������ϵ��һ��ȡ���Ƹְ������ϵ
	POLYGON m_xEdgeRegion;
	//BOOL m_bDimStdBoltSpace;	Ϊʲôȥ����wjh-2019.5.18
	ARRAY_LIST<f3dCircle> m_xLsCirArr;
	ARRAY_LIST<f3dLine> m_xAssLsRayArr;
	CXhSimpleList<ANGULARDIM_STRU> m_xAngularDimList;
	CXhSimpleList<ROTATEDIM_STRU> m_xRotateDimList;
	//ARRAY_LIST<LSRAY_STRU> m_xLsRayArr;	//ʵ������hashBoltRaysӦ��ͬһ���� wjh-2019.5.18
	CHashListEx<BOLTRAY> hashBoltRays;
	ATOM_LIST<BOLTINFO_TAG>listTagInfo;	//��ʱ�ҽӵĸ��ӵ�������{PART_ITEM}�ϵ����ݶ�������
public:
	//��˨��ע����趨
	bool m_bDimRayLsSpace;			//��ע������˨���
	bool m_bDimLsRayAngle;			//��ע��˨���߽Ƕ�
	bool m_bDimSpaceToWeldEdge;		//��ע���뺸�ӱ��������˨�׵����ӱߵ���С��ֱ����
	bool m_bDimSpaceToHuoQuLine;	//��ע�������ƽ�е���˨���߼��
	bool m_bDimFeatureLsSpace;		//��ע����������˨֮�����˨����
	bool m_bDimSpaceToEdgeLine;		//��ע����������˨�������߾���
private:
	virtual BOLTINFO_TAG* AppendTagToList(){return listTagInfo.append();}
	BOOL DimInsertBoltsRaylineSpace(CLDSParamPlate *pPlate);
	void DimBoltsRayline();
	void DimLsEndSpace();
	bool AnalyzeBoltsRayline(const double* location, int iRayNo, CLsRef *pLsRef, const double* resideRayVec = NULL);
	bool AnalyzeBoltsRayline(BOLT& bolt, int iRayNo, const char *guige, const double* resideRayVec = NULL);
	//
	bool PtInPlateRgn(const double* poscoord);
	BOOL FindLsRayOrgAndDatumVec(f3dPoint &origin, f3dPoint &datum_vec);
	BOLTRAY* FindLsRay(f3dPoint* lsPos1, f3dPoint* lsPos2);
	void CompleteLsRayInfo();
	void CorrectLsRays();
	void DimWrapLsSpace();
public:
	CDimPlateBolts();
	virtual ~CDimPlateBolts();
	//
	BOOL Create(CLDSPlate *pPlate,UCS_STRU* pCs=NULL);
	BOOL Create(CLDSParamPlate *pPlate,UCS_STRU* pCs = NULL);
	BOOL CreateEx(CLDSGeneralPlate* pPlate, CDrawCoordTrans cs);
	void InitPlateRgn(f3dPoint* lp3dPoints, int nCount);
	//���ݲ���
	ANGULARDIM_STRU* EnumFirstAngleDim(){return m_xAngularDimList.EnumObjectFirst();}
	ANGULARDIM_STRU* EnumNextAngleDim(){return m_xAngularDimList.EnumObjectNext();}
	ROTATEDIM_STRU* EnumFirstRotateDim(){return m_xRotateDimList.EnumObjectFirst();}
	ROTATEDIM_STRU* EnumNextRotateDim(){return m_xRotateDimList.EnumObjectNext();}
	BOLTRAY* EnumFirstBoltRay() { return hashBoltRays.GetFirst(); }
	BOLTRAY* EnumNextBoltRay() { return hashBoltRays.GetNext(); }
	f3dCircle* EnumFirstLsCir() { return m_xLsCirArr.GetFirst(); }
	f3dCircle* EnumNextLsCir() { return m_xLsCirArr.GetNext(); }
};
#endif
