// DesignJoint.h: interface for the CDesignJoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DESIGNJOINT_H__347D30A8_C348_4BBF_AA03_201699D26396__INCLUDED_)
#define AFX_DESIGNJOINT_H__347D30A8_C348_4BBF_AA03_201699D26396__INCLUDED_
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "lds_co_part.h"
#include "DesignMultiLeg.h"

struct JOINT_DESIGN_PARA_STRU
{
	CLDSVector norm_x_wing,norm_y_wing;
	CLDSPoint start,end;
	CLDSNode* m_pCurNode;
	double m_fJgSpace1,m_fJgSpace2;
	CLDSLineAngle* m_pMainJg1;
	CLDSLineAngle* m_pMainJg2;
	JOINT_DESIGN_PARA_STRU(){m_fJgSpace1=0;m_fJgSpace2=0;}
};
struct EDGE_SPACE{
	short endSpace;
	short berSpace;
	short wingSpace;
	EDGE_SPACE(){endSpace=berSpace=wingSpace=0;}
};
class CDesignJoint  
{
private:
	CJoint *m_pJoint;
	JOINT_DESIGN_PARA_STRU xDesJointPara;
	int base_jg1_oddment;
	int LsRowsN1,LsRowsN2;
	double plank_width;
	double base_wing_wide1,base_wing_thick1;
	double base_wing_wide2,base_wing_thick2;
	int m_nLsLayStyle;
	int m_nLsLHigh0RLow1;
	// 以下为设计接头过程中的临时变量
	CLDSLineAngle *pOutWrapJg,*pInWrapJg;
	CLDSPlate *pYPlate,*pXPlate,*pXPadPlate,*pYPadPlate;
	JGZJ jgzj1,jgzj2;
	CConnectInfo ls_group1,ls_group2;
	LSSPACE_STRU LsSpace1,LsSpace2;
private:
	CMappingBolts mappingX, mappingY;
	//布置螺栓
	bool LayDownBaseJgLs();
	bool LayUpBaseJgLs();
	bool LayUpBaseJgSingleRowLs(CLDSBolt &ls);
	bool LayUpBaseJgDoubleRowLs(CLDSBolt &ls);
	bool LayUpBaseJgDoubleRowFirstLs(CLDSBolt & ls,UCS_STRU &ucs);
	int  LayUpBaseJgDoubleRowOtherLs(CLDSBolt & ls,int j,UCS_STRU &ucs);
	void InitJgLsDesPosPara(CLDSBolt* pLs,CLDSLineAngle* pMainJg,char cWorkWing,double wing_off_dist,BOOL bSingRow=TRUE);
	//设计包角钢和包钢板
	void CreateRefAxisNorm(CJoint *pJoint);
	void CreateWrapPlate(CLDSPlate *pWrapPlate,int x_wing0_y_wing1,BOOL bPadPlate);
	void CreateWrapJg(CLDSLineAngle *pWrapJg,BOOL bInWrapJg);
	void RetrieveVertexDesPara(CLDSPlate* pWrapPlate,CLDSLineAngle* pMainJg,EDGE_SPACE& edge_space);
	void RetrieveWrapPlateLjRodPara(CLDSPlate* pWrapPlate);
	//设计接头
	bool DesignJoint();
	BOOL DesJointSubPart(CLDSLineAngle *&pOutWrapJg,CLDSLineAngle *&pInWrapJg,
		CLDSPlate *&pYPlate,CLDSPlate *&pXPlate,CLDSPlate *&pXPadPlate,CLDSPlate *&pYPadPlate);
public:
	CDesignJoint();
	virtual ~CDesignJoint();
    //
	BOOL CreateJoint(CJoint* pJoint,BOOL blEnableMirCreate=TRUE,BOOL bIntelligent=FALSE);	//设计并生成塔材接头连接结构
	void MirJoint(CJoint *pJoint,int iMirMsg);
};

#endif
#endif // !defined(AFX_DESIGNJOINT_H__347D30A8_C348_4BBF_AA03_201699D26396__INCLUDED_)
