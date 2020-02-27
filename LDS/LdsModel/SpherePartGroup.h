#pragma once
#include "lds_part.h"
#ifdef __COMPLEX_PART_INC_
struct TUBE_WORKPLANE{
	CLDSLineTube *m_pMainTube;
	CLDSLineTube *m_pViceTube;
	CHashList<CLDSLineTube*> m_hashTubePtrList;
	f3dPoint m_xWorkNormPlane;
	f3dPoint m_xViceTubeVec;
	double m_fViceVecCosa;		//辅助钢管射线方向与主管方向点积
	TUBE_WORKPLANE()
	{
		m_pMainTube=NULL;
		m_pViceTube=NULL;
		m_fViceVecCosa=0;
	}
};

//智能添加球节点组焊视图
class CSpherePartGroup
{
	CLDSPartGroup* m_pPartGroup;	//球节点组焊件
	//原点为球节点位置
	//Y轴为旋转主轴方向(基准钢管自外侧至球节点的轴向)
	//Z轴为由辅助支管方向vice_axis_vec与Y轴叉积确定(保证辅助支管在右上侧)
	//X轴由Y轴和Z轴共同叉积确定
	GECS dcs;
	CParaCS::PARAMETRIC_INFO datumcs_parainfo;
	CLDSLineTube* m_pDatumTube;	//旋转主轴基准钢管
	CLDSLineTube* m_pViceTube;	//与主轴基准钢管想配合的定位面辅助钢管,简称辅管
	CLDSSphere* m_pSphere;
public:
	CSpherePartGroup(CLDSPartGroup* pPartGroup);
	virtual ~CSpherePartGroup(void);
	BOOL AddSpherePartGroupViews(CLDSSphere *pSphere,PARTSET &partset);
private:
	//根据指定的主旋转构件及辅助杆件初始化工作面列表,返回主工作面
	TUBE_WORKPLANE *InitWorkPlaneList(CLDSLineTube *pMainTube,CLDSLineTube *pViceTube,
									 PARTSET &partset,ATOM_LIST<TUBE_WORKPLANE> &workPlaneList);
	//根据工作面添加视图
	CSectionView *AddSectionViewByWorkPlane(TUBE_WORKPLANE *pWorkPlane,PARTSET &partSet,CXhChar16 sViewName);
	//添加钢管端连接视图
	CSectionView *AddTubeEndSectionview(CSectionView *pMainView,CLDSLineTube *pLineTube,CXhChar16 sViewName);
};
#endif