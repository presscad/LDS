#pragma once

#ifndef __TSA_FILE_
#include "list.h"
#include "Hashtable.h"
#include "PlankDrawing.h"
//
struct IDimPlateBolts{
	struct BOLT {
		short siReserve;	//备用参数
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
		//bIsDoubleLsRay目前仅在CDimPlankLs中有用,未来应统一到CPlankDrawing后去掉 wjh-2015.2.27
		bool bIsDoubleLsRay;//是否为双排螺栓射线 wht 11-06-30
		bool bDatumLsRay;	//是否为基准螺栓射线
		GEPOINT rayOrg;			//射线的起始点
		GEPOINT rayResideVec;	//该射线所驻存构件的射线方向(与螺栓数无关,一般为其寄生杆件的心线方向) wjh-2015.2.27
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
		//int iRayNo;		//射线号
		int d;			//螺栓直径
		int L;			//螺栓有效长
		int N;			//螺栓个数
		double grade;	//螺栓级别
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
	//当pItem已经有挂接对象时执行TagInfo(...), 否则创建新挂接数据对象后返回
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
	UCS_STRU* m_pAdjustCs;		//用于校准螺栓间距的坐标系，一般取绘制钢板的坐标系
	POLYGON m_xEdgeRegion;
	//BOOL m_bDimStdBoltSpace;	为什么去掉？wjh-2019.5.18
	ARRAY_LIST<f3dCircle> m_xLsCirArr;
	ARRAY_LIST<f3dLine> m_xAssLsRayArr;
	CXhSimpleList<ANGULARDIM_STRU> m_xAngularDimList;
	CXhSimpleList<ROTATEDIM_STRU> m_xRotateDimList;
	//ARRAY_LIST<LSRAY_STRU> m_xLsRayArr;	//实际上与hashBoltRays应是同一属性 wjh-2019.5.18
	CHashListEx<BOLTRAY> hashBoltRays;
	ATOM_LIST<BOLTINFO_TAG>listTagInfo;	//临时挂接的附加到构件项{PART_ITEM}上的数据对象链表
public:
	//螺栓标注风格设定
	bool m_bDimRayLsSpace;			//标注射线螺栓间距
	bool m_bDimLsRayAngle;			//标注螺栓射线角度
	bool m_bDimSpaceToWeldEdge;		//标注距离焊接边最近的螺栓孔到焊接边的最小垂直距离
	bool m_bDimSpaceToHuoQuLine;	//标注与火曲线平行的螺栓射线间距
	bool m_bDimFeatureLsSpace;		//标注轮廓特征螺栓之间的螺栓距离
	bool m_bDimSpaceToEdgeLine;		//标注轮廓特征螺栓到轮廓边距离
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
	//数据操作
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
