#include "stdafx.h"
#include "DesignDiaphragm.h"
#include "env_def.h"
#include "IntelliModel.h"
#include ".\StdComTempl\IStdComTempl.h"

CDesignDiaphragm::CDesignDiaphragm()
{
	m_pDesignerContext = NULL;
}

CDesignDiaphragm::~CDesignDiaphragm()
{

}
typedef CLDSLineAngle* CLDSLineAnglePtr;
int LayoutAngleBolts(CLDSLineAngle* pAngle, CLDSNode* pBaseNode, BYTE ciWorkWingX0Y1, UINT uiBoltMd, BOLT_LAYOUT& layout, BOLTSET& boltset);
CLDSPlate *CDesignDiaphragm::DesignSharedPlate(CLDSNode *pBaseNode, CLDSLinePart *pHorizRod1, 
											   CLDSLinePart *pHorizRod2, CLDSLinePart *pXieRod)
{
	if (pBaseNode == NULL || pHorizRod1 == NULL || pHorizRod2 == NULL || pXieRod == NULL ||
		!pHorizRod1->IsAngle() || !pHorizRod2->IsAngle() || !pXieRod->IsAngle())
		return NULL;
	//1.计算共用板链接基准角钢连接肢
	GEPOINT line_vec1 = pHorizRod1->Start() - pHorizRod1->End();
	GEPOINT line_vec2 = pHorizRod2->Start() - pHorizRod2->End();
	GEPOINT work_norm = line_vec1 ^ line_vec2;
	normalize(work_norm);
	CLDSLineAngle *pHorizAngle1 = (CLDSLineAngle*)pHorizRod1;
	CLDSLineAngle *pHorizAngle2 = (CLDSLineAngle*)pHorizRod2;
	CLDSLineAngle *pXieAngle = (CLDSLineAngle*)pXieRod;
	char cWorkWingArr[3] = { 0 };
	BYTE ciWorkWingX0Y1Arr[3] = {0};
	CLDSLineAnglePtr anglePtrArr[3] = { pHorizAngle1,pHorizAngle2,pXieAngle };
	double ddx[3] = { 0 }, ddy[3] = {0};
	for (int i = 0; i < 3; i++)
	{
		ddx[i] = anglePtrArr[i]->vxWingNorm*work_norm;
		ddy[i] = anglePtrArr[i]->vyWingNorm*work_norm;
		if (fabs(ddx[i]) > fabs(ddy[i]))
			cWorkWingArr[i] = 'X';
		else
			cWorkWingArr[i] = 'Y';
		ciWorkWingX0Y1Arr[i] = cWorkWingArr[i] - 'X';
		if (i == 0)
		{	//根据横材工作肢方向修订work_norm
			if (cWorkWingArr[i] == 'X' && ddx[i] < 0)
			{
				work_norm *= -1.0;
				ddx[i] *= -1.0;
				ddy[i] *= -1.0;
			}
			else if (cWorkWingArr[i] == 'Y' && ddy[i] < 0)
			{
				work_norm *= -1.0;
				ddx[i] *= -1.0;
				ddy[i] *= -1.0;
			}
		}
	}
	CLDSPlate *pPlate = NULL;
	//2.判断当前节点共用板是否已经设计过
	CUnifyModelLayer::PLATE plate1(pBaseNode->handle, pHorizAngle1->handle, cWorkWingArr[0]);
	CUnifyModelLayer::PLATE plate2(pBaseNode->handle, pHorizAngle2->handle, cWorkWingArr[1]);
	if (m_pDesignerContext != NULL)
	{
		CUnifyModelLayer::PLATE* pDesignPlate = m_pDesignerContext->hashPlates.GetValue(plate1.StrKey());
		if(pDesignPlate==NULL)
			pDesignPlate = m_pDesignerContext->hashPlates.GetValue(plate2.StrKey());
		if (pDesignPlate && (pPlate=(CLDSPlate*)Ta.FromPartHandle(pDesignPlate->hPlate,CLS_PLATE))!=NULL)
			return pPlate;
	}
	//3.设计共用板
	CDesignLjPara designer;
	designer.m_bEnableFlexibleDesign = true;
	designer.m_hBaseNode = pBaseNode->handle;
	designer.m_hBasePart = pHorizAngle1->handle;
	//基准点
	designer.origin.datum_pos_style = 3;	//角钢心线交点
	designer.origin.des_para.AXIS_INTERS.hDatum1 = pHorizAngle1->handle;
	designer.origin.des_para.AXIS_INTERS.hDatum2 = pHorizAngle2->handle;
	designer.origin.des_para.AXIS_INTERS.wing_offset_style1 = 0;
	designer.origin.des_para.AXIS_INTERS.wing_offset_style2 = 0;
	//法线参数
	designer.norm.norm_style = 2;	//2.杆件交叉面法线
	designer.norm.hVicePart = pHorizAngle1->handle;
	designer.norm.hCrossPart = pHorizAngle2->handle;
	GEPOINT work_wing_norm = cWorkWingArr[0] == 'X' ? pHorizAngle1->vxWingNorm : pHorizAngle1->vyWingNorm;
	if (work_wing_norm.z > 0)
		designer.norm.nearVector.Set(0, 0, 1);
	else
		designer.norm.nearVector.Set(0, 0, -1);
	//
	CLDSBolt* pBolt;
	BOLTSET boltset;
	BOLT_LAYOUT layout;
	BYTE ciMainAngleBoltNum = 2, ciXieAngleBoltNum = 1;
	//添加钢板连接杆件并布置螺栓
	pPlate = (CLDSPlate*)Ta.Parts.append(CLS_PLATE);
	pPlate->SetLayer("SPG");
	pPlate->layer(0) = pHorizAngle1->Layer(0);
	pPlate->cfgword = pHorizAngle1->cfgword;
	pPlate->face_N = 1;
	//
	//添加共用板上的基准材连接杆件
	CDesignLjPartPara* pConnAnglePara = NULL;
	for (int i = 0; i < 3; i++)
	{
		CLDSLineAngle *pCurAngle = anglePtrArr[i];
		pConnAnglePara = designer.partList.Add(pCurAngle->handle);
		pConnAnglePara->m_nClassTypeId = pCurAngle->GetClassTypeId();
		pConnAnglePara->angle.bTwoEdge = (i < 2) ? true : false;
		pConnAnglePara->iFaceNo = 1;
		pConnAnglePara->start0_end1 = (pCurAngle->pStart == pBaseNode) ? 0 : 1;
		pConnAnglePara->angle.cur_wing_x0_y1 = ciWorkWingX0Y1Arr[i];
		//
		layout.ciLayoutStyle = BOLT_LAYOUT::LAYOUT_ONESIDE;
		layout.cnCount = ciMainAngleBoltNum;
		layout.ciTipS0E1 = (BYTE)pConnAnglePara->start0_end1;
		layout.ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
		layout.ciDatumWing = ciWorkWingX0Y1Arr[i];
		WORD wiBoltMd = 16;
		if (layout.ciTipS0E1 == 0)
			wiBoltMd = (pCurAngle->connectStart.d > wiBoltMd) ? (WORD)pCurAngle->connectStart.d : wiBoltMd;
		else if (layout.ciTipS0E1 == 1)
			wiBoltMd = (pCurAngle->connectEnd.d > wiBoltMd) ? (WORD)pCurAngle->connectEnd.d : wiBoltMd;
		else
			wiBoltMd = (pCurAngle->connectMid.d > wiBoltMd) ? (WORD)pCurAngle->connectMid.d : wiBoltMd;
		boltset.Empty();
		LayoutAngleBolts(pCurAngle, NULL, ciWorkWingX0Y1Arr[i], wiBoltMd, layout, boltset);
		double fTipNodeLenOffset = 0;
		if (i < 2)
		{
			LSSPACE_STRU LsSpace;
			GetLsSpace(LsSpace, wiBoltMd);
			fTipNodeLenOffset = LsSpace.EndSpace;
		}
		for (pBolt = boltset.GetFirst(); pBolt; pBolt = boltset.GetNext())
		{
			if (fTipNodeLenOffset!=0 && pBolt->des_base_pos.datumPoint.datum_pos_style == 1)
				pBolt->des_base_pos.datumPoint.des_para.RODEND.len_offset_dist += fTipNodeLenOffset;
			pPlate->AppendLsRef(pBolt->GetLsRef());
		}
		if (i == 2)
		{
			BYTE cCurWorkWing = cWorkWingArr[i];
			CMultiOffsetPos *pDesPos = (pConnAnglePara->start0_end1 == 0) ? &anglePtrArr[i]->desStartPos : &anglePtrArr[i]->desEndPos;
			if (cCurWorkWing == 'X'&&ddx[i] < 0)	//先默认为里铁
			{	//外铁
				pDesPos->wing_x_offset.gStyle = 4;
				pDesPos->wing_x_offset.offsetDist = -pPlate->Thick;
			}
			else if (cCurWorkWing == 'Y'&&ddy[i] < 0)
			{	//外铁
				pDesPos->wing_y_offset.gStyle = 4;
				pDesPos->wing_y_offset.offsetDist = -pPlate->Thick;
			}
		}
		//重新修订一下杆件及螺栓位置
		pCurAngle->CalPosition(true);
		if (pCurAngle->pStart == pBaseNode)
			pCurAngle->CalStartOddment();
		else if (pCurAngle->pEnd == pBaseNode)
			pCurAngle->CalEndOddment();
	}
	for (CLsRef* pLsRef = pPlate->GetFirstLsRef(); pLsRef; pLsRef = pPlate->GetNextLsRef())
		pLsRef->GetLsPtr()->correct_pos();
	designer.CloneTo(pPlate->designInfo);
	pPlate->DesignPlate();
	//4.注册共用板
	if (pPlate&&m_pDesignerContext)
	{	//共用板需注册两次（两根横材分别进行注册）
		CUnifyModelLayer::PLATE* pDesignPlate = m_pDesignerContext->hashPlates.GetValue(plate1.StrKey());
		pDesignPlate->hPlate = pPlate->handle;
		//
		pDesignPlate = m_pDesignerContext->hashPlates.GetValue(plate2.StrKey());
		pDesignPlate->hPlate = pPlate->handle;
	}
	return NULL;
}

void CDesignDiaphragm::DesignAllSharedPlate(CIntelliModelDesigner* pIntelliDesigner/*=NULL*/)
{
	m_pDesignerContext = pIntelliDesigner;
	//
	NODESET DispNodeSet;
	PARTSET DispPartSet;
	BLOCKREFSET DispBlockRefSet;
	FOUNDATIONSET DispFoundationSet;
	CDisplayView *pView = NULL;
	for (pView = Ta.GetFirstDisplayView(); pView; pView = Ta.GetNextDisplayView())
	{
		if (stricmp(pView->defaultLayer, "SPG") != 0 || strlen(pView->name) <= 0)
			continue;
		if (pView->name[0] != 'H'&&pView->name[0] != 'h')
			continue;
		pView->SelectDispSet(DispNodeSet, DispPartSet, DispBlockRefSet, DispFoundationSet);
		for (CLDSNode *pNode = DispNodeSet.GetFirst(); pNode; pNode = DispNodeSet.GetNext())
		{
			int nRodCount = 0;
			CLDSLinePart *pHorizRod1 = NULL, *pHorizRod2 = NULL, *pXieRod = NULL;
			for (CLDSPart *pPart = DispPartSet.GetFirst(); pPart; pPart = DispPartSet.GetNext())
			{
				if (!pPart->IsLinePart())
					continue;
				CLDSLinePart *pRod = (CLDSLinePart*)pPart;
				if (pRod->pStart == NULL || pRod->pEnd == NULL || (pRod->pStart != pNode && pRod->pEnd != pNode))
					continue;
				if (pRod->layer(1) == 'H')
				{
					if (pHorizRod1 == NULL)
						pHorizRod1 = pRod;
					else if (pHorizRod2 == NULL)
						pHorizRod2 = pRod;
				}
				else
				{
					if (pXieRod == NULL)
						pXieRod = pRod;
				}
				nRodCount++;
			}
			if (nRodCount == 3 && pHorizRod1&&pHorizRod2&&pXieRod)
			{
				DesignSharedPlate(pNode, pHorizRod1, pHorizRod2, pXieRod);
			}
		}
	}
}

