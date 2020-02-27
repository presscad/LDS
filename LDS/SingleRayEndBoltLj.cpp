//<LOCALE_TRANSLATE Confirm by hxr/>
// SingleRayEndBoltLj.cpp: implementation of the CSingleRayEndBoltLj class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "GUID.h"
#include "env_def.h"
#include "database.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "MainFrm.h"
#include "LibraryQuery.h"
#include "SingleRayEndBoltLj.h"
#include "UI.h"
#include "Deg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifndef __TSA_
CSingleRayEndBoltLj::CSingleRayEndBoltLj()
{
	memset(this,0,sizeof(CSingleRayEndBoltLj));
}

CSingleRayEndBoltLj::~CSingleRayEndBoltLj()
{

}
//如果射线角钢端头沿基准角钢杆件方向进行偏移，需根据偏移量新增偏移节点并更新射线角钢端头节点，保证实体与单线一致  wxc-2017.7.31
BOOL CSingleRayEndBoltLj::UpdateSingRayEndNode(int index)
{
	if(m_pBaseNode==NULL||m_pBaseAngle==NULL||m_arrRayAngle[index]==NULL||m_bRayAngleLenOff[index]==FALSE)
		return FALSE;
	//根据射线角钢沿基准角钢的偏移量新增偏移点
	CLDSLineAngle *pRayAngle = m_arrRayAngle[index];
	CMultiOffsetPos *pRayAnglePosPara = NULL;
	//需要拆分节点时，拆分后杆件失去原端节点 再按当前节点判断会有问题 wjh-2018.7.6
	BOOL bDesStart = (m_arrCurrRayNode[index] == 0) ? TRUE : FALSE;
	pRayAnglePosPara = bDesStart ? (&pRayAngle->desStartPos) : (&pRayAngle->desEndPos);
	//新增偏移节点的基准杆件优先取角钢设计端的搭接位置基准角钢 wxc-2019.9.20
	CLDSLineAngle* pDatumAngle = (CLDSLineAngle*)Ta.FromRodHandle(pRayAnglePosPara->datum_jg_h);
	if (pDatumAngle == NULL)
		pDatumAngle = m_pBaseAngle;
	f3dPoint datum_pos=m_pBaseNode->ActivePosition();
	f3dPoint len_offset_vec= (pDatumAngle->End()- pDatumAngle->Start()).normalized();
	f3dPoint offset_pos=datum_pos+len_offset_vec*pRayAnglePosPara->len_offset_dist;
	//添加偏移点
	CLDSNode* pNewNode = console.AppendNode();
	pNewNode->m_cPosCalType = 7;
	pNewNode->arrRelationNode[0]=m_pBaseNode->handle;
	pNewNode->arrRelationNode[1]= pDatumAngle->pStart->handle;
	pNewNode->arrRelationNode[2]= pDatumAngle->pEnd->handle;
	pNewNode->arrRelationPole[0]= pDatumAngle->handle;
	pNewNode->attach_offset=pRayAnglePosPara->len_offset_dist;
	pNewNode->SetPosition(offset_pos);
	pNewNode->hFatherPart= pDatumAngle->handle;
	pNewNode->cfgword= pDatumAngle->cfgword;
	pNewNode->iSeg= pDatumAngle->iSeg;
	pNewNode->SetLayer(pDatumAngle->layer());
	pNewNode->layer(1)='P';
	if (this->xBraceConnState.blRedesigned)	//设定特殊准据
		pNewNode->xFatherAngleZhunJu = m_pBaseNode->xFatherAngleZhunJu;
	if(UI::blEnableIntermediateUpdateUI)
		NewNode(pNewNode);
	//更新射线角钢的端节点信息
	if(bDesStart)
		pRayAngle->pStart=pNewNode;
	else
		pRayAngle->pEnd=pNewNode;
	pRayAnglePosPara->len_offset_dist=0;
	pRayAngle->ClearFlag();
	pRayAngle->CalPosition();
	//更新依赖杆件为该射线角钢的交叉节点设计信息
	pRayAngle->UpdateRodEndNodeRelation(m_pBaseNode,pNewNode);
	return TRUE;
}
CLDSBolt* CSingleRayEndBoltLj::LaySingleRayEndBolt(int index, CLDSBolt* pShareBolt /*= NULL*/)
{
	if (m_arrRayAngle[index] == NULL)
		return NULL;
	//需要拆分节点时，拆分后杆件失去原端节点 再按当前节点判断会有问题 wjh-2018.7.6
	CLDSLineAngle *pRayAngle = m_arrRayAngle[index];
	BOOL bDesStart=(m_arrCurrRayNode[index]==0);
	CMultiOffsetPos *pJgPosPara = NULL;
	if (bDesStart)
		pJgPosPara = &pRayAngle->desStartPos;
	else
		pJgPosPara = &pRayAngle->desEndPos;
	//新增螺栓的基准杆件优先取角钢设计端的搭接位置基准角钢 wxc-2019.9.20
	CLDSLineAngle* pDatumAngle = (CLDSLineAngle*)Ta.FromRodHandle(pJgPosPara->datum_jg_h);
	if (pDatumAngle == NULL)
		pDatumAngle = m_pBaseAngle;
	int iWorkWing=m_iBaseAngleWorkWing;
	f3dPoint vec1 = (pDatumAngle->End() - pDatumAngle->Start()).normalized();
	f3dPoint vec2 = (pRayAngle->End() - pRayAngle->Start()).normalized();
	f3dPoint norm = (vec1 ^ vec2).normalized();
	IsInsideJg(pDatumAngle, norm, &iWorkWing);
	//
	CLDSBolt *pBolt = NULL, *pSecBolt = NULL;
	pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
	pBolt->iSeg=m_pBaseNode->iSeg;	//段号
	pBolt->SetLayer(m_pBaseNode->layer());
	pBolt->set_d(m_arrConnInfo[index]->d);
	//pBolt->SetGrade(m_arrConnInfo[index]->grade);	//螺栓按规格库计算级别
	pBolt->cfgword= pDatumAngle->cfgword;	//调整螺栓配材号与基准构件配材号一致
	pBolt->AddL0Thick(pDatumAngle->handle,TRUE);
	pBolt->AddL0Thick(pRayAngle->handle,TRUE);
	pBolt->des_work_norm.norm_style=1;
	pBolt->des_work_norm.norm_wing= iWorkWing;
	pBolt->des_work_norm.direction=0;
	pBolt->des_work_norm.hVicePart= pDatumAngle->handle;
	pBolt->des_base_pos.datumPoint.datum_pos_style=3;
	pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1= pDatumAngle->handle;
	pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2=pRayAngle->handle;
	pBolt->des_base_pos.hPart= pDatumAngle->handle;
	if(m_arrConnInfo[index]->wnConnBoltN==2)	//双排螺栓时第一颗螺栓布在第一排心上，默认布置在标准心上
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=1;
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//心线设置:根据LdsLocale内设置的属性参数
	if (this->xBraceConnState.blRedesigned&&index<=xBraceConnState.cnBraceRayAngles)
	{
		BRACE_CONNECT_POINT::RAYANGLE* pCurrRayAngleInfo=&xBraceConnState.xarrRayAngles[index];
		if (pCurrRayAngleInfo->ciBaseStyleG<=3)
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1 = max(0,pCurrRayAngleInfo->ciBaseStyleG);
		else //if(pCurrRayAngleInfo->ciBaseStyleG>=4)
		{
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1=pCurrRayAngleInfo->siOffsetG;
			if (pCurrRayAngleInfo->ciBaseStyleG==5)
				pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1+=xBraceConnState.niBaseAngleWidth;
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	pBolt->des_base_pos.norm_offset.AddThick(-pDatumAngle->handle,TRUE);
	if(m_arrInsideAngle[index])
	{
		pBolt->des_base_pos.norm_offset.AddThick(-pRayAngle->handle,TRUE);
		if(m_arrRayAngleWorkWing[index]==0)
		{
			pJgPosPara->wing_x_offset.gStyle=4;
			pJgPosPara->wing_x_offset.offsetDist=-pDatumAngle->GetThick();
		}
		else
		{
			pJgPosPara->wing_y_offset.gStyle=4;
			pJgPosPara->wing_y_offset.offsetDist=-pDatumAngle->GetThick();
		}
		pRayAngle->CalPosition();
	}
	pBolt->CalGuigeAuto();
	pBolt->correct_worknorm();
	pBolt->correct_pos();
	//检测重叠螺栓
	if (pShareBolt)
	{
		GEPOINT intersPt1, intersPt2;
		Int3dlf(intersPt1, pBolt->ucs.origin, pBolt->ucs.axis_z, ucs.origin, ucs.axis_z);
		Int3dlf(intersPt2, pShareBolt->ucs.origin, pShareBolt->ucs.axis_z, ucs.origin, ucs.axis_z);
		if (intersPt1.IsEqual(intersPt2, 5))
		{
			Ta.Parts.DeleteCursor();
			pBolt = pShareBolt;
		}
	}
	if (UI::blEnableIntermediateUpdateUI)
		pBolt->Create3dSolidModel();
	if(m_arrConnInfo[index]->wnConnBoltN==2)	//布置双排螺栓第二颗螺栓，布在第二排心上
	{
		pSecBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pSecBolt->CopyProperty(pBolt);
		pSecBolt->cfgword= pDatumAngle->cfgword;	//调整螺栓配材号与基准构件配材号一致
		pSecBolt->SetLayer(m_pBaseNode->layer());
		pSecBolt->des_work_norm=pBolt->des_work_norm;
		pSecBolt->des_base_pos=pBolt->des_base_pos;
		pSecBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=2;
		pSecBolt->AddL0Thick(pDatumAngle->handle,TRUE);
		pSecBolt->AddL0Thick(pRayAngle->handle,TRUE);
		pSecBolt->CalGuigeAuto();
		pSecBolt->correct_worknorm();
		pSecBolt->correct_pos();
		if (UI::blEnableIntermediateUpdateUI)
		{
			pSecBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pSecBolt->GetSolidPartObject());
		}
	}
	if(bDesStart)
	{
		pRayAngle->AppendStartLsRef(pBolt->GetLsRef());
		if(pSecBolt)
			pRayAngle->AppendStartLsRef(pSecBolt->GetLsRef());
		pRayAngle->desStartOdd.m_iOddCalStyle=1;
		pRayAngle->CalStartOddment();
	}
	else
	{
		pRayAngle->AppendEndLsRef(pBolt->GetLsRef());
		if(pSecBolt)
			pRayAngle->AppendEndLsRef(pSecBolt->GetLsRef());
		pRayAngle->desEndOdd.m_iOddCalStyle=1;
		pRayAngle->CalEndOddment();
	}
	if(bDesStart)
	{
		pDatumAngle->CalStartOddment();
		pDatumAngle->AppendStartLsRef(pBolt->GetLsRef());
		if(pSecBolt)
			pDatumAngle->AppendStartLsRef(pSecBolt->GetLsRef());
	}
	else if(!bDesStart)
	{
		pDatumAngle->CalEndOddment();
		pDatumAngle->AppendEndLsRef(pBolt->GetLsRef());
		if(pSecBolt)
			pDatumAngle->AppendEndLsRef(pSecBolt->GetLsRef());
	}
	else
	{
		pDatumAngle->AppendMidLsRef(pBolt->GetLsRef());
		if(pSecBolt)
			pDatumAngle->AppendMidLsRef(pSecBolt->GetLsRef());
	}
	if (UI::blEnableIntermediateUpdateUI)
	{
		pRayAngle->Create3dSolidModel();
		pDatumAngle->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		g_pSolidDraw->NewSolidPart(pRayAngle->GetSolidPartObject());
		g_pSolidDraw->NewSolidPart(pDatumAngle->GetSolidPartObject());
		g_pSolidDraw->Draw();
	}
	return pBolt;
}
CSingleRayEndBoltLj::CAnglePositionLifeObj::CAnglePositionLifeObj(CLDSLineAngle* pAngle)
{
	Backup(pAngle);
}
bool CSingleRayEndBoltLj::CAnglePositionLifeObj::Backup(CLDSLineAngle* pAngle)
{
	if(pAngle==NULL)
		return false;
	BACKUP* pBackup=listBackupAngles.AttachObject();
	pBackup->pAngle=pAngle;
	pBackup->xStartPos=pAngle->Start();
	pBackup->xEndPos=pAngle->End();
	pBackup->bDataModified=pAngle->IsModified();
	return true;
}
CSingleRayEndBoltLj::CAnglePositionLifeObj::~CAnglePositionLifeObj()
{
	for(BACKUP* pBackup=listBackupAngles.EnumObjectFirst();pBackup;pBackup=listBackupAngles.EnumObjectNext())
	{
		pBackup->pAngle->SetStart(pBackup->xStartPos);
		pBackup->pAngle->SetEnd(pBackup->xEndPos);
		pBackup->pAngle->SetModified(pBackup->bDataModified);
	}
}
//设计交叉点处的单螺栓连接
#include "DianBanParaDlg.h"
extern BOOL DesignIntersNode(CLDSNode *pNode, BOOL bThrowError, CDianBanParaDlg *pDlg = NULL);
BOOL CSingleRayEndBoltLj::DesignIntersConnPoint(CConnectPointInfo *pConnectPoint, BRANCH_NODE_TEST* pOnlyBranchNodeTest/*=NULL*/)
{
	if (m_pBaseAngle==NULL || m_pBaseNode == NULL || m_pBaseNode->m_cPosCalType != 4)
		return FALSE;
	CLDSLineAngle* pCrossJg1 = (CLDSLineAngle*)Ta.FromPartHandle(m_pBaseNode->arrRelationPole[0], CLS_LINEANGLE);
	CLDSLineAngle* pCrossJg2 = (CLDSLineAngle*)Ta.FromPartHandle(m_pBaseNode->arrRelationPole[1], CLS_LINEANGLE);
	if (pCrossJg1 == NULL || pCrossJg2 == NULL || (m_pBaseAngle != pCrossJg1 && m_pBaseAngle != pCrossJg2))
		return FALSE;
	f3dPoint crossVec1 = (pCrossJg1->End() - pCrossJg1->Start()).normalized();
	f3dPoint crossVec2 = (pCrossJg2->End() - pCrossJg2->Start()).normalized();
	f3dPoint viewNorm = (crossVec1 ^ crossVec2).normalized();
	getjgzj(m_jgzjBase, m_pBaseAngle->GetWidth());
	int i = 0;
	CAnglePositionLifeObj backupAngles;
	for (CLDSLinePart* pRod = pConnectPoint->linkPartSet.GetFirst(); pRod; pRod = pConnectPoint->linkPartSet.GetNext())
	{
		if (pRod->GetClassTypeId() != CLS_LINEANGLE || pRod->pStart == NULL || pRod->pEnd == NULL)
			return FALSE;
		if (pRod->pStart != m_pBaseNode && pRod->pEnd != m_pBaseNode)
			return FALSE;
		CLDSLineAngle*pRayJg = (CLDSLineAngle*)pRod;
		f3dPoint rayLenV;
		if (pRayJg->pStart == m_pBaseNode)
			rayLenV = pRayJg->End() - pRayJg->Start();
		else
			rayLenV = pRayJg->Start() - pRayJg->End();
		normalize(rayLenV);
		if (fabs(rayLenV*crossVec1)> EPS_COS2 || fabs(rayLenV*crossVec2)>EPS_COS2)
			return FALSE;	//接近或已经共线
		if (pOnlyBranchNodeTest)
			backupAngles.Backup(pRayJg);
		int iWorkWing = 0;
		m_arrRayAngle[i] = pRayJg;
		m_arrRayVec[i] = rayLenV;
		m_arrCurrRayNode[i] = (pRayJg->pStart == m_pBaseNode) ? 0 : 1;
		m_arrInsideAngle[i] = IsInsideJg(pRayJg, viewNorm, &iWorkWing);
		m_arrRayAngleWorkWing[i] = iWorkWing;
		m_arrRayAngleWorkWingNorm[i] = (iWorkWing == 0) ? pRayJg->vxWingNorm : pRayJg->vyWingNorm;
		m_arrRayAngleWorkWingVec[i] = (iWorkWing == 0) ? pRayJg->GetWingVecX() : pRayJg->GetWingVecY();
		m_arrJgzjRay[i] = (iWorkWing == 0) ? pRayJg->xWingXZhunJu : pRayJg->xWingYZhunJu;
		if (!pRayJg->m_bEnableTeG)
			getjgzj(m_arrJgzjRay[i], pRayJg->GetWidth());
		m_arrConnInfo[i] = (m_arrCurrRayNode[i] == 0) ? (&pRayJg->connectStart) : (&m_arrRayAngle[i]->connectEnd);
		if (m_arrConnInfo[i]->wnConnBoltN > 2 || m_arrConnInfo[i]->wnConnBoltN < 0)
			return FALSE;
		if (m_arrConnInfo[i]->wnConnBoltN == 2 && (m_jgzjBase.g1 <= 0 || m_jgzjBase.g2 <= 0))
			return FALSE;	//基准材心距不支持双排
		i++;
	}
	double halfVertexDist = g_sysPara.VertexDist*0.5;
	double arrBackupCurrEndPosLenOffset[3] = { 0 };
	for (i = 0; i < 3; i++)
	{
		if(m_arrRayAngle[i]==NULL)
			continue;
		CMultiOffsetPos* pOffJgPosPara = NULL;
		if(m_arrCurrRayNode[i] == 0)
			pOffJgPosPara = &m_arrRayAngle[i]->desStartPos;
		else
			pOffJgPosPara = &m_arrRayAngle[i]->desEndPos;
		arrBackupCurrEndPosLenOffset[i] = pOffJgPosPara->len_offset_dist;
		//判断该射线角钢进行偏移的基准角钢和参照角钢
		double fAngle1 = min(cal_angle_of_2vec(m_arrRayVec[i],crossVec1), cal_angle_of_2vec(m_arrRayVec[i], crossVec1*(-1)));
		double fAngle2 = min(cal_angle_of_2vec(m_arrRayVec[i],crossVec2), cal_angle_of_2vec(m_arrRayVec[i], crossVec2*(-1)));
		CLDSLineAngle* pBaseJg = (fAngle1 < fAngle2) ? pCrossJg1 : pCrossJg2;
		CLDSLineAngle* pRelyJg = (fAngle1 < fAngle2) ? pCrossJg2 : pCrossJg1;
		if (m_arrInsideAngle[i] == IsInsideJg(pBaseJg, viewNorm, &m_iBaseAngleWorkWing))
		{	//射线角钢与基准角钢同里或同外，如果射线角钢在基准角钢楞线侧,则重新调整偏移基准角钢
			f3dPoint berLinePtS, berLinePtE, rayLinePtS, rayLinePtE, intersPt;
			Int3dlf(berLinePtS, pBaseJg->Start(), base_wing_norm, m_pBaseNode->xActivePos, base_wing_norm);
			Int3dlf(berLinePtE, pBaseJg->End(), base_wing_norm, m_pBaseNode->xActivePos, base_wing_norm);
			Int3dlf(rayLinePtS, m_arrRayAngle[i]->Start(), base_wing_norm, m_pBaseNode->xActivePos, base_wing_norm);
			Int3dlf(rayLinePtE, m_arrRayAngle[i]->End(), base_wing_norm, m_pBaseNode->xActivePos, base_wing_norm);
			if (Int3dll(berLinePtS, berLinePtE, rayLinePtS, rayLinePtE, intersPt) == 1)
			{	//调换偏移基准角钢和参照角钢
				CLDSLineAngle* pTemJg = pBaseJg;
				pBaseJg = pRelyJg;
				pRelyJg = pTemJg;
			}
		}
		f3dPoint workNorm, rely_vec, rely_wing_vec, rely_wing_norm;
		base_vec = (pBaseJg->End() - pBaseJg->Start()).normalized();
		rely_vec = (pRelyJg->End() - pRelyJg->Start()).normalized();
		workNorm = (rely_vec ^ base_vec).normalized();
		if (IsInsideJg(pBaseJg, workNorm, &m_iBaseAngleWorkWing))
			workNorm *= -1;
		base_wing_vec = (m_iBaseAngleWorkWing == 0) ? pBaseJg->GetWingVecX() : pBaseJg->GetWingVecY();
		base_wing_norm = (m_iBaseAngleWorkWing == 0) ? pBaseJg->vxWingNorm : pBaseJg->vyWingNorm;
		Int3dlf(ucs.origin, m_pBaseNode->xActivePos, workNorm, pBaseJg->Start(), base_wing_norm);
		ucs.axis_z = workNorm;
		ucs.axis_x = rely_vec;
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
		//计算参照杆件端点的有效区域
		int iRelyWorkWing = 0;
		IsInsideJg(pRelyJg, workNorm, &iRelyWorkWing);
		rely_wing_vec = (iRelyWorkWing == 0) ? pRelyJg->GetWingVecX() : pRelyJg->GetWingVecY();
		f3dPoint ptBerS, ptBerE, ptWingS, ptWingE, inters;
		ptBerS = pRelyJg->Start() - rely_wing_vec * halfVertexDist;
		ptBerE = pRelyJg->End() - rely_wing_vec * halfVertexDist;
		ptWingS = pRelyJg->Start() + rely_wing_vec * (pRelyJg->GetWidth() + halfVertexDist);
		ptWingE = pRelyJg->End() + rely_wing_vec * (pRelyJg->GetWidth() + halfVertexDist);
		coord_trans(ptBerS, ucs, FALSE);
		coord_trans(ptBerE, ucs, FALSE);
		coord_trans(ptWingS, ucs, FALSE);
		coord_trans(ptWingE, ucs, FALSE);
		ptBerS.z = ptBerE.z = ptWingS.z = ptWingE.z = 0;
		SCOPE_STRU scope;
		scope.VerifyVertex(ptBerS);
		scope.VerifyVertex(ptBerE);
		scope.VerifyVertex(ptWingS);
		scope.VerifyVertex(ptWingE);
		//杆件端节点进行移动
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace, m_arrConnInfo[i]->d);
		BOOL bInters = FALSE;
		int hits = 0;
		do {//
			hits++;
			//计算射线角钢与基准角钢的交点
			f3dLine baseAxis, rayAxis;
			getjgzj(m_jgzjBase, pBaseJg->GetWidth());
			baseAxis.startPt = pBaseJg->Start() + base_wing_vec * m_jgzjBase.g;
			baseAxis.endPt = pBaseJg->End() + base_wing_vec * m_jgzjBase.g;
			coord_trans(baseAxis.startPt, ucs, FALSE);
			coord_trans(baseAxis.endPt, ucs, FALSE);
			baseAxis.startPt.z = baseAxis.endPt.z = 0;
			rayAxis.startPt = m_arrRayAngle[i]->Start() + m_arrRayAngleWorkWingVec[i] * m_arrJgzjRay[i].g;
			rayAxis.endPt = m_arrRayAngle[i]->End() + m_arrRayAngleWorkWingVec[i] * m_arrJgzjRay[i].g;
			coord_trans(rayAxis.startPt, ucs, FALSE);
			coord_trans(rayAxis.endPt, ucs, FALSE);
			rayAxis.startPt.z = rayAxis.endPt.z = 0;
			Int3dpl(baseAxis, rayAxis, inters);
			//
			f3dPoint rayLenVec, rayWingVec, ptOrg, pickPtB, pickPtW;
			rayLenVec = m_arrRayVec[i];
			vector_trans(rayLenVec, ucs, FALSE);
			rayWingVec = m_arrRayAngleWorkWingVec[i];
			vector_trans(rayWingVec, ucs, FALSE);
			ptOrg = inters - rayLenVec * (LsSpace.EndSpace + halfVertexDist);
			pickPtB = ptOrg - rayWingVec * (m_arrJgzjRay[i].g + halfVertexDist);
			pickPtW = ptOrg + rayWingVec * (m_arrRayAngle[i]->GetWidth() - m_arrJgzjRay[i].g + halfVertexDist);
			pickPtB.z = pickPtW.z = 0;
			bInters = scope.IsIncludePoint(pickPtB);
			if (!bInters)
				bInters = scope.IsIncludePoint(pickPtW);
			if (bInters)
			{
				double halfSpaceGrow = g_sysPara.intelligentConnect.m_nOffsetAdd*0.5;
				if (m_arrRayVec[i] * base_vec > 0)//更靠近终端
					pOffJgPosPara->len_offset_dist += halfSpaceGrow * 2;
				else
					pOffJgPosPara->len_offset_dist -= halfSpaceGrow * 2;
				m_bRayAngleLenOff[i] = TRUE;
				pOffJgPosPara->datum_jg_h = pBaseJg->handle;
				if (m_iBaseAngleWorkWing == 0)
				{	//搭接在基准角钢X肢
					if (pOffJgPosPara->spatialOperationStyle == 3)
						pOffJgPosPara->spatialOperationStyle = 2;
					else if (pOffJgPosPara->spatialOperationStyle == 6)
						pOffJgPosPara->spatialOperationStyle = 5;
				}
				else
				{	//搭接在基准角钢Y肢
					if (pOffJgPosPara->spatialOperationStyle == 2)
						pOffJgPosPara->spatialOperationStyle = 3;
					else if (pOffJgPosPara->spatialOperationStyle == 5)
						pOffJgPosPara->spatialOperationStyle = 6;
				}
				m_arrRayAngle[i]->ClearFlag();
				m_arrRayAngle[i]->CalPosition();
			}
			if (pOffJgPosPara->spatialOperationStyle < 1 || pOffJgPosPara->spatialOperationStyle>14)
			{	//两根射线角钢全部为非搭接,不能靠增加偏移量来避免碰撞
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.LevelLog(1, "Two side ray angles(0x%X and 0x%X) all are not lap-joint, collision can not be avoided by increasing the deviation of ray angle end along datum angle!", pOffJg->handle);
#else
				logerr.LevelLog(1, "射线角钢0x%X为非搭接,无法通过增加射线角钢端头沿基准角钢偏移量的方式来避免碰撞!", m_arrRayAngle[i]->handle);
#endif
				break;
			}
			else if (pOffJgPosPara->len_offset_dist > 300)
			{	//防止因不合理的射线角钢位置参数导致死循环
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.LevelLog(1, "Collision can not be avoided by increasing the deviation of ray angles(0x%X and 0x%X) end along datum angle!", pOffJg->handle);
#else
				logerr.LevelLog(1, "无法通过增加射线角钢(0x%X)端头沿基准角钢偏移量的方式来避免碰撞!", m_arrRayAngle[i]->handle);
#endif
				break;
			}
			//增长步长等于0时不进行自动调整,角钢间距取初始间距值 wht 11-01-07
			if (g_sysPara.intelligentConnect.m_nOffsetAdd == 0)
				bInters = FALSE;
		} while (bInters&&hits <= 10);
		if (hits == 10)
			logerr.Log("0x%X角钢自动干涉偏移失败", m_arrRayAngle[i]->handle);
	}
	if (pOnlyBranchNodeTest)
	{	//恢复原有的基点纵向偏移量
		for (i = 0; i < 3; i++)
		{
			if (m_arrRayAngle[i] == NULL)
				continue;
			CMultiOffsetPos* pPosDesPara = NULL;
			if (m_arrCurrRayNode[i] == 0)
				pPosDesPara = &m_arrRayAngle[i]->desStartPos;
			else
				pPosDesPara = &m_arrRayAngle[i]->desEndPos;
			if(pPosDesPara->len_offset_dist== arrBackupCurrEndPosLenOffset[i])
				continue;
			for (int j = 0; j < 2; j++)
			{
				if (pOnlyBranchNodeTest->pRods[j] == NULL)
				{
					pOnlyBranchNodeTest->pRods[j] = m_arrRayAngle[i];
					pOnlyBranchNodeTest->pDatumRods[j] = Ta.FromRodHandle(pPosDesPara->datum_jg_h);
					pOnlyBranchNodeTest->ciRodCurrEndS0E1[j] = (m_arrCurrRayNode[i] == 0) ? 0 : 1;
					pOnlyBranchNodeTest->siLenOffset[j] = (short)ftoi(pPosDesPara->len_offset_dist);
					break;
				}
			}
			pPosDesPara->len_offset_dist = arrBackupCurrEndPosLenOffset[i];
		}
	}
	else
	{
		for (i = 0; i < 3; i++)
		{
			if (m_bRayAngleLenOff[i])
				UpdateSingRayEndNode(i);
			LaySingleRayEndBolt(i);
		}
		//设计交叉单螺栓
		DesignIntersNode(m_pBaseNode, FALSE);
	}
	return TRUE;
}

BOOL CSingleRayEndBoltLj::DesignConnectPoint(CConnectPointInfo *pConnectPoint,BRANCH_NODE_TEST* pOnlyBranchNodeTest/*=NULL*/)
{
	m_pBaseNode=Ta.Node.FromHandle(pConnectPoint->m_hBaseNode);
	m_pBaseAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pConnectPoint->m_hBasePart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if(m_pBaseNode==NULL||m_pBaseAngle==NULL)
		return FALSE;
	if (m_pBaseNode->m_cPosCalType == 4)
		return DesignIntersConnPoint(pConnectPoint, pOnlyBranchNodeTest);
	//初始化基准杆件连接信息
	CLDSLineAngle* pRayJg = (CLDSLineAngle*)pConnectPoint->linkPartSet.GetFirst();
	if (pRayJg == NULL)
		return FALSE;
	GEPOINT ray_vec, norm;
	base_vec=m_pBaseAngle->End()-m_pBaseAngle->Start();
	normalize(base_vec);
	ray_vec = pRayJg->xPosEnd - pRayJg->xPosStart;
	if (pRayJg->pEnd == m_pBaseNode)
		ray_vec *= -1;
	normalize(ray_vec);
	norm = ray_vec ^ base_vec;
	normalize(norm);
	double ddx = fabs(m_pBaseAngle->vxWingNorm*norm);
	double ddy = fabs(m_pBaseAngle->vyWingNorm*norm);
	m_iBaseAngleWorkWing = (ddx > ddy) ? 0 : 1;
	base_wing_vec = (ddx > ddy) ? m_pBaseAngle->GetWingVecX():m_pBaseAngle->GetWingVecY();
	base_wing_norm = (ddx > ddy) ? m_pBaseAngle->vxWingNorm : m_pBaseAngle->vyWingNorm;
	bool blReplaceTipDependAngle=false;
	if(m_pBaseNode->hFatherPart!=m_pBaseAngle->handle)
	{	//节点父杆件与当前基准角钢始->终方向反向，须替换定位基准杆件,否则会死循环 wjh-2019.7.1
		CLDSLinePart* pRod=Ta.FromRodHandle(m_pBaseNode->hFatherPart);
		if(pRod!=NULL&&(base_vec*(pRod->xPosEnd-pRod->xPosStart))<0)
			blReplaceTipDependAngle=true;
	}
	//初始化坐标系
	ucs.origin = m_pBaseNode->Position();
	ucs.axis_z = norm;
	if (norm*base_wing_norm < 0)
		ucs.axis_z *= -1;
	ucs.axis_x = base_vec;
	ucs.axis_y = ucs.axis_z^ucs.axis_x;
	if (ucs.axis_y*ray_vec < 0)	//保证Y轴方向在射线杆件一侧
		ucs.axis_y *= -1.0;	
	normalize(ucs.axis_y);
	ucs.axis_x = ucs.axis_y^ucs.axis_z;
	normalize(ucs.axis_x);
	//初始化射线杆件连接信息
	CAnglePositionLifeObj backupAngles;
	int i=0;
	do
	{
		if(i==0)
			pRayJg =(CLDSLineAngle*)pConnectPoint->linkPartSet.GetFirst();
		else
			pRayJg =(CLDSLineAngle*)pConnectPoint->linkPartSet.GetNext();
		if (pRayJg == NULL || pRayJg->GetClassTypeId() != CLS_LINEANGLE)
			break;
		if(pOnlyBranchNodeTest)
			backupAngles.Backup(pRayJg);
		ray_vec = (pRayJg->End() - pRayJg->Start()).normalized();
		if (pRayJg->pEnd == m_pBaseNode)
			ray_vec *= -1;
		if (ray_vec * base_vec > EPS_COS2)
			return FALSE;	//接近或已经共线
		BYTE ciWorkWing = 0;
		m_arrRayAngle[i] = pRayJg;
		m_arrRayVec[i] = ray_vec;
		m_arrCurrRayNode[i] = (pRayJg->pStart == m_pBaseNode) ? 0 : 1;
		m_arrInsideAngle[i] = IsInsideJg(pRayJg, ucs.axis_z, &ciWorkWing);
		m_arrRayAngleWorkWing[i] = ciWorkWing;
		m_arrRayAngleWorkWingNorm[i] = (ciWorkWing == 0) ? pRayJg->vxWingNorm : pRayJg->vyWingNorm;
		m_arrRayAngleWorkWingVec[i] = (ciWorkWing == 0) ? pRayJg->GetWingVecX() : pRayJg->GetWingVecY();
		if (!m_arrRayAngle[i]->m_bEnableTeG)
			getjgzj(m_arrJgzjRay[i], m_arrRayAngle[i]->GetWidth());
		else
		{
			if (m_arrRayAngleWorkWing[i] == 0)
				m_arrJgzjRay[i] = m_arrRayAngle[i]->xWingXZhunJu;
			else
				m_arrJgzjRay[i] = m_arrRayAngle[i]->xWingYZhunJu;
		}
		if (blReplaceTipDependAngle)
		{	//替换射线角钢的搭接基准角钢
			if(m_arrCurrRayNode[i]==0)
				m_arrRayAngle[i]->desStartPos.datum_jg_h = m_pBaseAngle->handle;
			else
				m_arrRayAngle[i]->desEndPos.datum_jg_h = m_pBaseAngle->handle;
		}
		i++;
	}while(m_arrRayAngle[i-1]&&i<3);
	if (!m_pBaseAngle->m_bEnableTeG)
		getjgzj(m_jgzjBase, m_pBaseAngle->GetWidth());
	else if (m_iBaseAngleWorkWing == 0)	//基准材X肢连接
		m_jgzjBase = m_pBaseAngle->xWingXZhunJu;
	else
		m_jgzjBase = m_pBaseAngle->xWingYZhunJu;
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//汇总判断是否为辅材连接点设计的相关信息 wjh-2019.11.10
	xBraceConnState.niBaseAngleWidth=(WORD)ftoi(m_pBaseAngle->size_wide);
	BRACE_CONNECT_POINT::RAYANGLE xarrBraceAngles[4];	//目前最多仅支持三射线角钢
	xBraceConnState.xarrRayAngles=xarrBraceAngles;
	if (pConnectPoint->bendPlaneRodSet.Count>0||pConnectPoint->linkPartSet.Count>3||m_pBaseAngle->chLayer2nd!='Z')
		xBraceConnState.cnBraceRayAngles=0;
	else
	{
		xBraceConnState.cnBraceRayAngles=0;
		for (CLDSLineAngle* pRayAngle = (CLDSLineAngle*)pConnectPoint->linkPartSet.GetFirst(); pRayAngle != NULL;
			pRayAngle = (CLDSLineAngle*)pConnectPoint->linkPartSet.GetNext(), xBraceConnState.cnBraceRayAngles++)
		{
			if (!pRayAngle->IsAuxPole())
			{	//不完全是射线辅材连接
				xBraceConnState.cnBraceRayAngles = 0;
				break;
			}
			BRACE_CONNECT_POINT::RAYANGLE* pCurrRayAngleInfo = &xarrBraceAngles[xBraceConnState.cnBraceRayAngles];
			GEPOINT vRayVec = ucs.TransVToCS(this->m_arrRayVec[xBraceConnState.cnBraceRayAngles]);
			pCurrRayAngleInfo->siRayAngle = (short)ftoi(DEG(vRayVec));
			pCurrRayAngleInfo->blInsideAngle = this->m_arrInsideAngle[xBraceConnState.cnBraceRayAngles];
			if (pRayAngle->pStart == this->m_pBaseNode)
			{
				pCurrRayAngleInfo->ciBoltD = (BYTE)pRayAngle->connectStart.d;
				pCurrRayAngleInfo->ciBoltN = (BYTE)pRayAngle->connectStart.wnConnBoltN;
			}
			else
			{
				pCurrRayAngleInfo->ciBoltD = (BYTE)pRayAngle->connectEnd.d;
				pCurrRayAngleInfo->ciBoltN = (BYTE)pRayAngle->connectEnd.wnConnBoltN;
			}
		}
		if (xBraceConnState.cnBraceRayAngles>0&&xBraceConnState.DesignConnectPoint())
		{
			BRACE_CONNECT_POINT::RAYANGLE* pBaseRayAngleInfo=&xBraceConnState.xarrRayAngles[0];
			if (pBaseRayAngleInfo->ciTipPosStyle>=0&&pBaseRayAngleInfo->ciBaseStyleG>=0)
			{
				if (pBaseRayAngleInfo->ciBaseStyleG<=3)
				{
					if (m_iBaseAngleWorkWing == 0)
						m_pBaseNode->xFatherAngleZhunJu.offset_X_dist_style = pBaseRayAngleInfo->ciBaseStyleG;
					else
						m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist_style = pBaseRayAngleInfo->ciBaseStyleG;
				}
				else if (pBaseRayAngleInfo->ciBaseStyleG>=4)
				{
					short siBaseOffsetG=pBaseRayAngleInfo->siOffsetG;
					if (pBaseRayAngleInfo->ciBaseStyleG==5)
						siBaseOffsetG+=xBraceConnState.niBaseAngleWidth;
					if (m_iBaseAngleWorkWing == 0)
					{
						m_pBaseNode->xFatherAngleZhunJu.offset_X_dist_style=4;
						m_pBaseNode->xFatherAngleZhunJu.offset_X_dist = siBaseOffsetG;
					}
					else
					{
						m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist_style=4;
						m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist = siBaseOffsetG;
					}
				}
				for (int i = 0; i < xBraceConnState.cnBraceRayAngles; i++)
					m_arrRayAngle[i]->CalPosition(true);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	LSSPACE_STRU LsSpace;
	double arrBackupCurrEndPosLenOffset[3] = { 0 };	//提前备份角钢当前连接端的基点纵向偏移量，以便拆分节点检测时恢复原有纵向偏移量 wjh-2017.12.16
	//判断单根角钢上的双螺栓是否可合法布置到基准材上的双排心线上
	for(i=0;i<3;i++)
	{
		if(m_arrRayAngle[i]==NULL)
			continue;
		arrBackupCurrEndPosLenOffset[i] = (m_arrRayAngle[i]->pStart == m_pBaseNode) ? m_arrRayAngle[i]->desStartPos.len_offset_dist : m_arrRayAngle[i]->desEndPos.len_offset_dist;
		if (m_arrRayAngle[i]->pStart == m_pBaseNode)
			m_arrConnInfo[i] = &m_arrRayAngle[i]->connectStart;
		else //if(m_arrRayAngle[i]->pEnd==pNode)
			m_arrConnInfo[i] = &m_arrRayAngle[i]->connectEnd; 
		if (m_arrConnInfo[i]->wnConnBoltN > 2 || m_arrConnInfo[i]->wnConnBoltN < 0)
			return FALSE;
		if(m_arrConnInfo[i]->wnConnBoltN==2)
		{
			if(m_jgzjBase.g1<=0||m_jgzjBase.g2<=0)
				return FALSE;	//基准材心距不支持双排
			double cosa=fabs(base_vec*m_arrRayVec[i]);
			double sina=sqrt(1-cosa*cosa);
			GetLsSpace(LsSpace,m_arrConnInfo[i]->d);
			double space=m_jgzjBase.g2-m_jgzjBase.g1;
			if(space/sina<LsSpace.SingleRowSpace)	//两端连接螺栓间距不足
				return FALSE;
		}
	}
	//初始化射线角钢端螺栓的偏移设计参数
	double halfVertexDist=g_sysPara.VertexDist*0.5;
	if(m_arrRayAngle[1]&&m_arrRayAngle[2]==NULL)	//双射线
	{
		if(m_arrInsideAngle[0]!=m_arrInsideAngle[1])	//双射线里外铁朝向不同
			return FALSE;
		int movePriorityArr[2]={2,2};	//0.X、Y坐标分量为0杆件，固定不动;1.水平杆件;2.斜向杆件，最优先移动
		if(g_sysPara.intelligentConnect.m_bPriorOffsetGradientRod)
		{
			for(i=0;i<2;i++)
			{
				GEPOINT line_vec=m_arrRayAngle[i]->pEnd->Position(false)-m_arrRayAngle[i]->pStart->Position(false);
				if(fabs(line_vec.x)<EPS||fabs(line_vec.y)<EPS)
					movePriorityArr[i]=0;
				else if(fabs(line_vec.z)<EPS)
					movePriorityArr[i]=1;
			}
			if(movePriorityArr[0]==movePriorityArr[1])
				movePriorityArr[0]=movePriorityArr[1]=1;
			else if(movePriorityArr[0]<movePriorityArr[1])
				movePriorityArr[0]=0;
			else if(movePriorityArr[0]>movePriorityArr[1])
				movePriorityArr[1]=0;
		}

		BOOL bInters=FALSE;
		int hits=0;
		do{
			CMultiOffsetPos *arrRayAnglePosPara[2];
			hits++;
			//必须放在do循环中，因为循环过程可能会更改m_arrRayAngle[0]的位置
			if(m_arrRayAngle[0]->pStart==m_pBaseNode)
			{
				ucs.origin=m_arrRayAngle[0]->Start();
				ucs.axis_x=m_arrRayAngle[0]->End()-m_arrRayAngle[0]->Start();
			}
			else
			{
				ucs.origin=m_arrRayAngle[0]->End();
				ucs.axis_x=m_arrRayAngle[0]->Start()-m_arrRayAngle[0]->End();
			}
			normalize(ucs.axis_x);
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			ucs.axis_x=ucs.axis_y^ucs.axis_z;
			
			f3dLine baseline(m_pBaseAngle->Start(),m_pBaseAngle->End());
			f3dLine baseaxisline;
			f2dRect baseRayRect2d;
			f2dLine viceRayLenLine[3],viceRayVertLine[3];
			f3dPoint arrLenVec[3];
			f3dPoint arrVertVec[3];
			for(i=0;i<2;i++)
			{
				f3dPoint lenVec;
				if(m_arrRayAngle[i]->pStart==m_pBaseNode)
				{
					arrRayAnglePosPara[i]=&m_arrRayAngle[i]->desStartPos;
					lenVec=m_arrRayAngle[i]->End()-m_arrRayAngle[i]->Start();
				}
				else
				{
					arrRayAnglePosPara[i]=&m_arrRayAngle[i]->desEndPos;
					lenVec=m_arrRayAngle[i]->Start()-m_arrRayAngle[i]->End();
				}
				if(i==0)
					lenVec=ucs.axis_x;
				else
					normalize(lenVec);
				m_arrRayVec[i]=lenVec;
				vector_trans(lenVec,ucs,FALSE);
				arrLenVec[i].Set(lenVec.x,lenVec.y);
				f3dPoint wingVec;
				if(m_arrRayAngleWorkWing[i]==0)
					wingVec=m_arrRayAngle[i]->GetWingVecX();
				else
					wingVec=m_arrRayAngle[i]->GetWingVecY();
				vector_trans(wingVec,ucs,FALSE);
				arrVertVec[i].Set(wingVec.x,wingVec.y);
				
				f3dLine rayline(m_arrRayAngle[i]->Start(),m_arrRayAngle[i]->End());
				if(m_arrRayAngle[i]->pEnd==m_pBaseNode)
				{
					rayline.startPt=m_arrRayAngle[i]->End();
					rayline.endPt=m_arrRayAngle[i]->Start();
				}
				coord_trans(rayline.startPt,ucs,FALSE);
				coord_trans(rayline.endPt,ucs,FALSE);
				f2dLine rayaxis2d;
				rayaxis2d.startPt=rayline.startPt+arrVertVec[i]*m_arrJgzjRay[i].g;
				rayaxis2d.endPt=rayline.endPt+arrVertVec[i]*m_arrJgzjRay[i].g;
				
				double extra_odd=0;	//因双排螺栓外侧心距与标准心距差带来的额外正头
				double fBaseWingOff = (m_arrConnInfo[i]->wnConnBoltN <= 1) ? m_jgzjBase.g : m_jgzjBase.g1;
				if (xBraceConnState.blRedesigned && m_arrConnInfo[i]->wnConnBoltN == 1)
				{	//启用特殊心距
					if (m_iBaseAngleWorkWing == 0)
						fBaseWingOff = m_pBaseNode->xFatherAngleZhunJu.offset_X_dist;
					else
						fBaseWingOff = m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist;
				}
				baseaxisline.startPt=baseline.startPt+base_wing_vec* fBaseWingOff;
				baseaxisline.endPt=baseline.endPt+base_wing_vec* fBaseWingOff;
				if(m_arrConnInfo[i]->wnConnBoltN==2)
				{
					double cosa=base_wing_vec*m_arrRayVec[i];
					if(cosa>EPS)	//射线在肢翼侧
						extra_odd=(m_jgzjBase.g-m_jgzjBase.g1)/cosa;
					else if(cosa<-EPS)	//射线在楞线侧
						extra_odd=(m_jgzjBase.g-m_jgzjBase.g2)/cosa;
				}
				coord_trans(baseaxisline.startPt,ucs,FALSE);
				coord_trans(baseaxisline.endPt,ucs,FALSE);
				f2dLine baseaxis2d(baseaxisline.startPt,baseaxisline.endPt);
				
				f3dPoint inters;
				Int2dpl(baseaxis2d,rayaxis2d,inters.x,inters.y);
				GetLsSpace(LsSpace,m_arrConnInfo[i]->d);
				LsSpace.EndSpace+=ftoi(extra_odd);
				f3dPoint rect2d;
				if(i==0)
				{
					if(arrVertVec[i].y>0)
					{
						baseRayRect2d.topLeft.Set(inters.x-LsSpace.EndSpace-halfVertexDist,m_arrRayAngle[0]->GetWidth()+halfVertexDist);
						baseRayRect2d.bottomRight.Set(m_arrRayAngle[0]->GetLength(),-halfVertexDist);
					}
					else
					{
						baseRayRect2d.topLeft.Set(inters.x-LsSpace.EndSpace-halfVertexDist,-m_arrRayAngle[0]->GetWidth()-halfVertexDist);
						baseRayRect2d.bottomRight.Set(m_arrRayAngle[0]->GetLength(),halfVertexDist);
					}
				}
				else
				{
					if(arrLenVec[i].y>0)	//基准射线上侧
					{
						if(arrVertVec[i].y>0)
						{
							viceRayLenLine[i].startPt=inters-arrLenVec[i]*(LsSpace.EndSpace+halfVertexDist)-
								arrVertVec[i]*(m_arrJgzjRay[i].g+halfVertexDist);
						}
						else
						{
							viceRayLenLine[i].startPt=inters-arrLenVec[i]*(LsSpace.EndSpace+halfVertexDist)+
								arrVertVec[i]*(m_arrRayAngle[i]->GetWidth()-m_arrJgzjRay[i].g+halfVertexDist);
						}
					}
					else					//基准射线下侧
					{
						if(arrVertVec[i].y>0)
						{
							viceRayLenLine[i].startPt=inters-arrLenVec[i]*(LsSpace.EndSpace+halfVertexDist)+
								arrVertVec[i]*(m_arrRayAngle[i]->GetWidth()-m_arrJgzjRay[i].g+halfVertexDist);
						}
						else
						{
							viceRayLenLine[i].startPt=inters-arrLenVec[i]*(LsSpace.EndSpace+halfVertexDist)-
								arrVertVec[i]*(m_arrJgzjRay[i].g+halfVertexDist);
						}
					}
					viceRayLenLine[i].endPt.x=viceRayLenLine[i].startPt.x+arrLenVec[i].x*m_arrRayAngle[i]->GetLength();
					viceRayLenLine[i].endPt.y=viceRayLenLine[i].startPt.y+arrLenVec[i].y*m_arrRayAngle[i]->GetLength();
					viceRayVertLine[i].startPt=inters-arrLenVec[i]*(LsSpace.EndSpace+halfVertexDist)-
						arrVertVec[i]*(m_arrJgzjRay[i].g+halfVertexDist);
					viceRayVertLine[i].endPt=inters-arrLenVec[i]*(LsSpace.EndSpace+halfVertexDist)+
						arrVertVec[i]*(m_arrRayAngle[i]->GetWidth()-m_arrJgzjRay[i].g+halfVertexDist);
				}
			}
			bInters=baseRayRect2d.IntersWithLine(viceRayLenLine[1]);
			if(!bInters)
				bInters=baseRayRect2d.IntersWithLine(viceRayVertLine[1]);
			if(bInters)
			{
				double halfSpaceGrow=g_sysPara.intelligentConnect.m_nOffsetAdd*0.5;
				double space=fabs(arrRayAnglePosPara[0]->len_offset_dist-arrRayAnglePosPara[1]->len_offset_dist);
				if(space<g_sysPara.intelligentConnect.m_nOffset)
					halfSpaceGrow=(g_sysPara.intelligentConnect.m_nOffset-space)/2;
				if( (arrRayAnglePosPara[0]->len_offset_dist==0&&arrRayAnglePosPara[1]->len_offset_dist!=0)||
					(movePriorityArr[0]==0&&movePriorityArr[1]>0))
				{
					if(m_arrRayVec[1]*base_vec>m_arrRayVec[0]*base_vec)
					{	//第二根更靠近终端
						arrRayAnglePosPara[1]->len_offset_dist+=halfSpaceGrow*2;
						m_bRayAngleLenOff[1]=TRUE;
					}
					else
					{
						arrRayAnglePosPara[1]->len_offset_dist-=halfSpaceGrow*2;
						m_bRayAngleLenOff[1]=TRUE;
					}
					m_arrRayAngle[1]->CalPosition(true);
				}
				else if((arrRayAnglePosPara[0]->len_offset_dist!=0&&arrRayAnglePosPara[1]->len_offset_dist==0)||
					(movePriorityArr[0]>0&&movePriorityArr[1]==0))
				{
					if(m_arrRayVec[0]*base_vec>m_arrRayVec[1]*base_vec)
					{	//第一根更靠近终端
						arrRayAnglePosPara[0]->len_offset_dist+=halfSpaceGrow*2;
						m_bRayAngleLenOff[0]=TRUE;
					}
					else
					{
						arrRayAnglePosPara[0]->len_offset_dist-=halfSpaceGrow*2;
						m_bRayAngleLenOff[0]=TRUE;
					}
					m_arrRayAngle[0]->CalPosition(true);
				}
				else //if(arrRayAnglePosPara[0]->len_offset_dist==0&&arrRayAnglePosPara[1]->len_offset_dist==0)
				{	//都为零，就向两边扩
					if(m_arrRayVec[0]*base_vec>m_arrRayVec[1]*base_vec)
					{	//第一根更靠近终端
						arrRayAnglePosPara[0]->len_offset_dist+=halfSpaceGrow;
						arrRayAnglePosPara[1]->len_offset_dist-=halfSpaceGrow;
					}
					else
					{
						arrRayAnglePosPara[0]->len_offset_dist-=halfSpaceGrow;
						arrRayAnglePosPara[1]->len_offset_dist+=halfSpaceGrow;
					}
					m_bRayAngleLenOff[0]=m_bRayAngleLenOff[1]=TRUE;
					m_arrRayAngle[0]->CalPosition(true);
					m_arrRayAngle[1]->CalPosition(true);
				}
			}
			if((arrRayAnglePosPara[0]->spatialOperationStyle<1||arrRayAnglePosPara[0]->spatialOperationStyle>14)&&
				(arrRayAnglePosPara[1]->spatialOperationStyle<1||arrRayAnglePosPara[1]->spatialOperationStyle>14))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "Two ray angles(0x%X and 0x%X) all are not lap-joint, collision can not be avoided by increasing the deviation of ray angle end along datum angle!",
#else
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"两根射线角钢(0x%X与0x%X)全部为非搭接,无法通过增加射线角钢端头沿基准角钢偏移量的方式来避免碰撞!",
#endif
					m_arrRayAngle[0]->handle, m_arrRayAngle[1]->handle);
				break;	//两根射线角钢全部为非搭接,不能靠增加偏移量来避免碰撞
			}
			else if(arrRayAnglePosPara[0]->len_offset_dist>300||arrRayAnglePosPara[1]->len_offset_dist>300)
			{	//防止因不合理的射线角钢位置参数导致死循环
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "Collision can not be avoided by increasing the deviation of ray angles(0x%X and 0x%X) end along datum angle!",
#else
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "无法通过增加射线角钢(0x%X与0x%X)端头沿基准角钢偏移量的方式来避免碰撞!",
#endif
					m_arrRayAngle[0]->handle, m_arrRayAngle[1]->handle);
				break;
			}
			//增长步长等于0时不进行自动调整,角钢间距取初始间距值 wht 11-01-07
			if(g_sysPara.intelligentConnect.m_nOffsetAdd==0)
				bInters=FALSE;
		}while(bInters&&hits<=10);
		if(hits==10)
			logerr.Log("0x%X角钢与0x%X角钢间自动干涉偏移失败",m_arrRayAngle[0]->handle,m_arrRayAngle[1]->handle);
	}
	else if(m_arrRayAngle[2]!=NULL&&m_arrInsideAngle[0]==m_arrInsideAngle[1]&&m_arrInsideAngle[0]==m_arrInsideAngle[2])
	{	//三射线，且里外铁朝向相同
		//确定中间杆件为基准杆件
		int index = -1;
		double cos1 = m_arrRayVec[0]*base_vec;
		double cos2 = m_arrRayVec[1]*base_vec;
		double cos3 = m_arrRayVec[2]*base_vec;
		if( (cos1>=cos2&&cos1<=cos3)||
			(cos1>=cos3&&cos1<=cos2))
			index = 0;
		else if((cos2>=cos1&&cos2<=cos3)||
			(cos2>=cos3&&cos2<=cos1))
			index = 1;
		else if((cos3>=cos1&&cos3<=cos2)||
			(cos3>=cos2&&cos3<=cos1))
			index = 2;
		if (index == -1)
			return FALSE;
		CLDSLineAngle* pBaseRayAngle = m_arrRayAngle[index];
		if(pBaseRayAngle->pStart==m_pBaseNode)
			ucs.origin=pBaseRayAngle->Start()+m_arrRayAngleWorkWingVec[index]*m_arrJgzjRay[index].g;
		else
			ucs.origin=pBaseRayAngle->End()+m_arrRayAngleWorkWingVec[index]*m_arrJgzjRay[index].g;
		ucs.axis_x = m_arrRayVec[index];
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		BOOL bInters=FALSE;
		int hits=0;
		do{
			CMultiOffsetPos *arrRayAnglePosPara[3];	
			hits++;
			f3dLine baseline(m_pBaseAngle->Start(),m_pBaseAngle->End());
			f3dLine baseaxisline;
			f2dRect baseRayRect2d;
			f2dLine viceRayLenLine[3],viceRayVertLine[3];
			f3dPoint arrLenVec[3];
			f3dPoint arrVertVec[3];
			for(i=0;i<3;i++)
			{
				if(m_arrRayAngle[i]->pStart==m_pBaseNode)
					arrRayAnglePosPara[i]=&m_arrRayAngle[i]->desStartPos;
				else
					arrRayAnglePosPara[i]=&m_arrRayAngle[i]->desEndPos;
				double fBaseWingOff = (m_arrConnInfo[i]->wnConnBoltN <= 1) ? m_jgzjBase.g : m_jgzjBase.g1;
				if (xBraceConnState.blRedesigned && m_arrConnInfo[i]->wnConnBoltN == 1)
				{	//启用特殊心距
					if (m_iBaseAngleWorkWing == 0)
						fBaseWingOff = m_pBaseNode->xFatherAngleZhunJu.offset_X_dist;
					else
						fBaseWingOff = m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist;
				}
				baseaxisline.startPt = baseline.startPt + base_wing_vec * fBaseWingOff;
				baseaxisline.endPt = baseline.endPt + base_wing_vec * fBaseWingOff;
				coord_trans(baseaxisline.startPt,ucs,FALSE);
				coord_trans(baseaxisline.endPt,ucs,FALSE);
				f2dLine baseaxis2d(baseaxisline.startPt,baseaxisline.endPt);
				f3dPoint lenVec=m_arrRayVec[i];
				vector_trans(lenVec,ucs,FALSE);
				arrLenVec[i].Set(lenVec.x,lenVec.y);
				f3dPoint wingVec=m_arrRayAngleWorkWingVec[i];
				vector_trans(wingVec,ucs,FALSE);
				arrVertVec[i].Set(wingVec.x,wingVec.y);
				f3dLine rayline(m_arrRayAngle[i]->Start(),m_arrRayAngle[i]->End());
				if(m_arrRayAngle[i]->pEnd==m_pBaseNode)
				{
					rayline.startPt=m_arrRayAngle[i]->End();
					rayline.endPt=m_arrRayAngle[i]->Start();
				}
				coord_trans(rayline.startPt,ucs,FALSE);
				coord_trans(rayline.endPt,ucs,FALSE);
				f2dLine rayaxis2d;
				rayaxis2d.startPt=rayline.startPt+arrVertVec[i]*m_arrJgzjRay[i].g;
				rayaxis2d.endPt=rayline.endPt+arrVertVec[i]*m_arrJgzjRay[i].g;
				f3dPoint inters;
				Int2dpl(baseaxis2d,rayaxis2d,inters.x,inters.y);
				GetLsSpace(LsSpace,m_arrConnInfo[index]->d);
				f3dPoint rect2d;
				if(i==index)
				{
					baseRayRect2d.topLeft.Set(inters.x-LsSpace.EndSpace-halfVertexDist,m_arrRayAngle[index]->GetWidth()-m_arrJgzjRay[i].g+halfVertexDist);
					baseRayRect2d.bottomRight.Set(m_arrRayAngle[index]->GetLength(),-m_arrJgzjRay[i].g-halfVertexDist);
				}
				else
				{
					if(arrLenVec[i].y>0)	//基准射线上侧
					{
						if(arrVertVec[i].y>0)
						{
							viceRayLenLine[i].startPt=inters-arrLenVec[i]*(LsSpace.EndSpace+halfVertexDist)-
								arrVertVec[i]*(m_arrJgzjRay[i].g+halfVertexDist);
						}
						else
							viceRayLenLine[i].startPt=inters-arrLenVec[i]*LsSpace.EndSpace+arrVertVec[i]*m_arrJgzjRay[i].g;
					}
					else					//基准射线下侧
					{
						if(arrVertVec[i].y>0)
						{
							viceRayLenLine[i].startPt=inters-arrLenVec[i]*(LsSpace.EndSpace+halfVertexDist)+
								arrVertVec[i]*(m_arrJgzjRay[i].g+halfVertexDist);
						}
						else
							viceRayLenLine[i].startPt=inters-arrLenVec[i]*LsSpace.EndSpace-arrVertVec[i]*m_arrJgzjRay[i].g;
					}
					viceRayLenLine[i].endPt.x=viceRayLenLine[i].startPt.x+arrLenVec[i].x*m_arrRayAngle[i]->GetLength();
					viceRayLenLine[i].endPt.y=viceRayLenLine[i].startPt.y+arrLenVec[i].y*m_arrRayAngle[i]->GetLength();
					viceRayVertLine[i].startPt=inters-arrLenVec[i]*(LsSpace.EndSpace+halfVertexDist)-
						arrVertVec[i]*(m_arrJgzjRay[i].g+halfVertexDist);
					viceRayVertLine[i].endPt=inters-arrLenVec[i]*(LsSpace.EndSpace+halfVertexDist)+
						arrVertVec[i]*(m_arrRayAngle[i]->GetWidth()-m_arrJgzjRay[i].g+halfVertexDist);
				}
			}
			int rayIndex[2];//存储要移动的角钢索引
			int j=0;
			for (i=0;i<3;i++)
			{
				if (i!=index)
				{
					rayIndex[j] = i;
					j++;
				}
			}
			bInters=baseRayRect2d.IntersWithLine(viceRayLenLine[rayIndex[0]]);
			if(!bInters)
				bInters=baseRayRect2d.IntersWithLine(viceRayVertLine[rayIndex[0]]);
			if (!bInters)
				bInters=baseRayRect2d.IntersWithLine(viceRayLenLine[rayIndex[1]]);
			if(!bInters)
				bInters=baseRayRect2d.IntersWithLine(viceRayVertLine[rayIndex[1]]);
			if(bInters)
			{	
				for(i=0;i<3;i++)
				{
					if(i==index)
						continue;
					double halfSpaceGrow=g_sysPara.intelligentConnect.m_nOffsetAdd*0.5;
					double space=fabs(arrRayAnglePosPara[index]->len_offset_dist-arrRayAnglePosPara[i]->len_offset_dist);
					if(space<g_sysPara.intelligentConnect.m_nOffset)
						halfSpaceGrow=(g_sysPara.intelligentConnect.m_nOffset-space)/2;
					if(m_arrRayVec[i]*base_vec>m_arrRayVec[index]*base_vec)//更靠近终端
						arrRayAnglePosPara[i]->len_offset_dist+=halfSpaceGrow*2;
					else
						arrRayAnglePosPara[i]->len_offset_dist-=halfSpaceGrow*2;
					m_bRayAngleLenOff[i]=TRUE;
					m_arrRayAngle[i]->ClearFlag();
					m_arrRayAngle[i]->CalPosition();
				}
			}
			if((arrRayAnglePosPara[rayIndex[0]]->spatialOperationStyle<1||arrRayAnglePosPara[rayIndex[0]]->spatialOperationStyle>14)||
				(arrRayAnglePosPara[rayIndex[1]]->spatialOperationStyle<1||arrRayAnglePosPara[rayIndex[1]]->spatialOperationStyle>14))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "Two side ray angles(0x%X and 0x%X) all are not lap-joint, collision can not be avoided by increasing the deviation of ray angle end along datum angle!",
#else
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"两侧射线角钢(0x%X与0x%X)为非搭接,无法通过增加射线角钢端头沿基准角钢偏移量的方式来避免碰撞!",
#endif
					m_arrRayAngle[0]->handle, m_arrRayAngle[1]->handle);
				break;	//两根射线角钢全部为非搭接,不能靠增加偏移量来避免碰撞
			}
			else if(arrRayAnglePosPara[rayIndex[0]]->len_offset_dist>300||arrRayAnglePosPara[rayIndex[1]]->len_offset_dist>300)
			{	//防止因不合理的射线角钢位置参数导致死循环
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "Collision can not be avoided by increasing the deviation of ray angles(0x%X and 0x%X) end along datum angle!",
#else
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "无法通过增加射线角钢(0x%X与0x%X)端头沿基准角钢偏移量的方式来避免碰撞!",
#endif
					m_arrRayAngle[0]->handle, m_arrRayAngle[1]->handle);
				break;
			}
			//增长步长等于0时不进行自动调整,角钢间距取初始间距值 wht 11-01-07
			if(g_sysPara.intelligentConnect.m_nOffsetAdd==0)
				bInters=FALSE;
		}while(bInters&&hits<=10);
		if(hits==10)
			logerr.Log("0x%X角钢与0x%X角钢间自动干涉偏移失败",m_arrRayAngle[0]->handle,m_arrRayAngle[1]->handle);
	}
	else if (m_arrRayAngle[2]!=NULL &&(m_arrInsideAngle[0]!=m_arrInsideAngle[1]||m_arrInsideAngle[0]!=m_arrInsideAngle[2]))
	{	//三射线里外铁朝向不同
		//确定中间杆件为基准杆件
		int indexS = -1, indexM = -1, indexB = -1, indexOff = -1;
		double cosArr[3] = { 0 };
		cosArr[0] = m_arrRayVec[0] * base_vec;
		cosArr[1] = m_arrRayVec[1] * base_vec;
		cosArr[2] = m_arrRayVec[2] * base_vec;
		if ((cosArr[0] >= cosArr[1] && cosArr[0] <= cosArr[2]) ||
			(cosArr[0] >= cosArr[2] && cosArr[0] <= cosArr[1]))
		{
			indexM = 0;
			indexS = (cosArr[0] >= cosArr[1] && cosArr[0] <= cosArr[2]) ? 1 : 2;
			indexB = (cosArr[0] >= cosArr[1] && cosArr[0] <= cosArr[2]) ? 2 : 1;
		}
		else if ((cosArr[1] >= cosArr[0] && cosArr[1] <= cosArr[2]) ||
				(cosArr[1] >= cosArr[2] && cosArr[1] <= cosArr[0]))
		{
			indexM = 1;
			indexS = (cosArr[1] >= cosArr[0] && cosArr[1] <= cosArr[2]) ? 0 : 2;
			indexB = (cosArr[1] >= cosArr[0] && cosArr[1] <= cosArr[2]) ? 2 : 0;
		}
		else if ((cosArr[2] >= cosArr[0] && cosArr[2] <= cosArr[1]) ||
				(cosArr[2] >= cosArr[1] && cosArr[2] <= cosArr[0]))
		{
			indexM = 2;
			indexS = (cosArr[2] >= cosArr[0] && cosArr[2] <= cosArr[1]) ? 0 : 1;
			indexB = (cosArr[2] >= cosArr[0] && cosArr[2] <= cosArr[1]) ? 1 : 0;
		}
		if (indexM == -1)
			return FALSE;
		if (m_arrRayAngle[indexM]->pStart == m_pBaseNode)
			ucs.origin = m_arrRayAngle[indexM]->Start() + m_arrRayAngleWorkWingVec[indexM] * m_arrJgzjRay[indexM].g;
		else
			ucs.origin = m_arrRayAngle[indexM]->End() + m_arrRayAngleWorkWingVec[indexM] * m_arrJgzjRay[indexM].g;
		ucs.axis_x = m_arrRayVec[indexM];
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
		//判断进行移动的射线杆件
		if (m_arrInsideAngle[indexS] != m_arrInsideAngle[indexB])
		{	//两侧角钢的里外朝向不同，取与中间角钢里外铁朝向相同的角钢进行移动
			if (m_arrInsideAngle[indexS] == m_arrInsideAngle[indexM])
				indexOff = indexS;
			else
				indexOff = indexB;
		}
		else
		{	//两侧角钢的里外朝向相同，则与中间角钢夹角较大的进行移动
			if (fabs(cosArr[indexM] - cosArr[indexS]) > fabs(cosArr[indexB] - cosArr[indexM]))
				indexOff = indexS;
			else
				indexOff = indexB;
		}
		if (indexOff == -1)
			return FALSE;
		//计算中间杆件端点的有效区域
		CLDSLineAngle* pJgM = m_arrRayAngle[indexM];
		CMultiOffsetPos* pMidJgPosPara= (pJgM->pStart==m_pBaseNode)?(&pJgM->desStartPos):(&pJgM->desEndPos);
		GetLsSpace(LsSpace, m_arrConnInfo[indexM]->d);
		f3dPoint inters, ray_wing_vec = m_arrRayAngleWorkWingVec[indexM];
		f3dLine baseAxis, rayAxis;
		double fBaseWingOff = (m_arrConnInfo[indexM]->wnConnBoltN <= 1) ? m_jgzjBase.g : m_jgzjBase.g1;
		if (xBraceConnState.blRedesigned && m_arrConnInfo[indexM]->wnConnBoltN == 1)
		{	//启用特殊心距后，
			if (m_iBaseAngleWorkWing == 0)
				fBaseWingOff = m_pBaseNode->xFatherAngleZhunJu.offset_X_dist;
			else
				fBaseWingOff = m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist;
		}
		baseAxis.startPt = m_pBaseAngle->Start() + base_wing_vec * fBaseWingOff;
		baseAxis.endPt = m_pBaseAngle->End() + base_wing_vec * fBaseWingOff;
		coord_trans(baseAxis.startPt, ucs, FALSE);
		coord_trans(baseAxis.endPt, ucs, FALSE);
		double fRayWingOff = m_arrJgzjRay[indexM].g;
		rayAxis.startPt = pJgM->Start() + ray_wing_vec * fRayWingOff;
		rayAxis.endPt = pJgM->End()+ ray_wing_vec * fRayWingOff;
		coord_trans(rayAxis.startPt, ucs, FALSE);
		coord_trans(rayAxis.endPt, ucs, FALSE);
		Int2dpl(f2dLine(baseAxis.startPt,baseAxis.endPt),f2dLine(rayAxis.startPt,rayAxis.endPt),inters.x,inters.y);
		f2dRect baseRayRect2d;
		baseRayRect2d.topLeft.x		= inters.x + pJgM->GetLength();
		baseRayRect2d.topLeft.y		= inters.y - fRayWingOff - halfVertexDist;
		baseRayRect2d.bottomRight.x = inters.x - LsSpace.EndSpace - halfVertexDist;
		baseRayRect2d.bottomRight.y = inters.y + pJgM->GetWidth() - fRayWingOff + halfVertexDist;
		//杆件端节点进行移动
		CLDSLineAngle* pOffJg = m_arrRayAngle[indexOff];
		BOOL bInters = FALSE;
		int hits = 0;
		do {
			hits++;
			CMultiOffsetPos* pOffJgPosPara = (pOffJg->pStart == m_pBaseNode) ? (&pOffJg->desStartPos) : (&pOffJg->desEndPos);
			baseAxis.startPt = m_pBaseAngle->Start() + base_wing_vec * fBaseWingOff;
			baseAxis.endPt = m_pBaseAngle->End() + base_wing_vec * fBaseWingOff;
			coord_trans(baseAxis.startPt, ucs, FALSE);
			coord_trans(baseAxis.endPt, ucs, FALSE);
			fRayWingOff = m_arrJgzjRay[indexOff].g;
			rayAxis.startPt = pOffJg->Start() + m_arrRayAngleWorkWingVec[indexOff] * fRayWingOff;
			rayAxis.endPt = pOffJg->End() + m_arrRayAngleWorkWingVec[indexOff] * fRayWingOff;
			coord_trans(rayAxis.startPt, ucs, FALSE);
			coord_trans(rayAxis.endPt, ucs, FALSE);
			Int2dpl(f2dLine(baseAxis.startPt,baseAxis.endPt),f2dLine(rayAxis.startPt,rayAxis.endPt),inters.x,inters.y);
			//
			f3dPoint rayLenVec, rayWingVec, ptS, ptE;
			rayLenVec = m_arrRayVec[indexOff];
			vector_trans(rayLenVec, ucs, FALSE);
			rayWingVec = m_arrRayAngleWorkWingVec[indexOff];
			vector_trans(rayWingVec, ucs, FALSE);
			if (rayLenVec.y > 0)	
			{	//基准射线上侧
				if (rayWingVec.y > 0)
					ptS =inters-rayLenVec*(LsSpace.EndSpace+halfVertexDist)-rayWingVec*(fRayWingOff+halfVertexDist);
				else
					ptS =inters-rayLenVec*LsSpace.EndSpace+rayWingVec*fRayWingOff;
			}
			else					
			{	//基准射线下侧
				if (rayWingVec.y > 0)
					ptS =inters-rayLenVec*(LsSpace.EndSpace+halfVertexDist)+rayWingVec*(fRayWingOff+halfVertexDist);
				else
					ptS =inters-rayLenVec*LsSpace.EndSpace-rayWingVec * fRayWingOff;
			}
			ptE = ptS + rayLenVec * pOffJg->GetLength();
			bInters = baseRayRect2d.IntersWithLine(f2dLine(ptS,ptE));
			if (!bInters)
			{
				f3dPoint pt = inters - rayLenVec * (LsSpace.EndSpace + halfVertexDist);
				ptS = pt - rayWingVec * (fRayWingOff + halfVertexDist);
				ptE = pt + rayWingVec * (pOffJg->GetWidth() - fRayWingOff + halfVertexDist);
				bInters = baseRayRect2d.IntersWithLine(f2dLine(ptS, ptE));
			}
			if (bInters)
			{
				double halfSpaceGrow = g_sysPara.intelligentConnect.m_nOffsetAdd*0.5;
				double space=fabs(pMidJgPosPara->len_offset_dist-pOffJgPosPara->len_offset_dist);
				if (space < g_sysPara.intelligentConnect.m_nOffset)
					halfSpaceGrow = (g_sysPara.intelligentConnect.m_nOffset - space) / 2;
				if (indexOff==indexB)//更靠近终端
					pOffJgPosPara->len_offset_dist += halfSpaceGrow * 2;
				else
					pOffJgPosPara->len_offset_dist -= halfSpaceGrow * 2;
				m_bRayAngleLenOff[indexOff] = TRUE;
				pOffJg->ClearFlag();
				pOffJg->CalPosition();
			}
			if (pOffJgPosPara->spatialOperationStyle < 1 || pOffJgPosPara->spatialOperationStyle>14)
			{	//两根射线角钢全部为非搭接,不能靠增加偏移量来避免碰撞
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.LevelLog(1, "Two side ray angles(0x%X and 0x%X) all are not lap-joint, collision can not be avoided by increasing the deviation of ray angle end along datum angle!", pOffJg->handle);
#else
				logerr.LevelLog(1, "射线角钢0x%X为非搭接,无法通过增加射线角钢端头沿基准角钢偏移量的方式来避免碰撞!", pOffJg->handle);
#endif
				break;	
			}
			else if (pOffJgPosPara->len_offset_dist > 300)
			{	//防止因不合理的射线角钢位置参数导致死循环
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.LevelLog(1, "Collision can not be avoided by increasing the deviation of ray angles(0x%X and 0x%X) end along datum angle!", pOffJg->handle);
#else
				logerr.LevelLog(1, "无法通过增加射线角钢(0x%X)端头沿基准角钢偏移量的方式来避免碰撞!", pOffJg->handle);
#endif
				break;
			}
			//增长步长等于0时不进行自动调整,角钢间距取初始间距值 wht 11-01-07
			if (g_sysPara.intelligentConnect.m_nOffsetAdd == 0)
				bInters = FALSE;
		} while (bInters&&hits <= 10);
		if (hits == 10)
			logerr.Log("0x%X角钢与0x%X角钢间自动干涉偏移失败", pOffJg->handle, pJgM->handle);
	}
	if(pOnlyBranchNodeTest)
	{	//恢复原有的基点纵向偏移量
		for(i=0;i<3;i++)
		{
			if(m_arrRayAngle[i]==NULL)
				continue;
			CMultiOffsetPos* pPosDesPara = NULL;
			if (m_arrCurrRayNode[i] == 0)
				pPosDesPara = &m_arrRayAngle[i]->desStartPos;
			else
				pPosDesPara = &m_arrRayAngle[i]->desEndPos;
			if (pPosDesPara->len_offset_dist == arrBackupCurrEndPosLenOffset[i])
				continue;
			for (int j = 0; j < 2; j++)
			{
				if (pOnlyBranchNodeTest->pRods[j] == NULL)
				{
					pOnlyBranchNodeTest->pRods[j] = m_arrRayAngle[i];
					pOnlyBranchNodeTest->pDatumRods[j] = Ta.FromRodHandle(pPosDesPara->datum_jg_h);
					pOnlyBranchNodeTest->ciRodCurrEndS0E1[j] = (m_arrCurrRayNode[i] == 0) ? 0 : 1;
					pOnlyBranchNodeTest->siLenOffset[j] = (short)ftoi(pPosDesPara->len_offset_dist);
					break;
				}
			}
			pPosDesPara->len_offset_dist = arrBackupCurrEndPosLenOffset[i];
		}
	}
	else
	{
		CLDSBolt* pShareBolt = NULL;
		for (i = 0; i < 3; i++)
		{
			if (m_bRayAngleLenOff[i])
			{	//杆件进行偏移，设计偏移节点和螺栓
				UpdateSingRayEndNode(i);
				LaySingleRayEndBolt(i);
			}
			else
			{	//选中节点处设计螺栓，考虑螺栓重叠
				CLDSBolt* pEndBolt = LaySingleRayEndBolt(i, pShareBolt);
				if (pShareBolt == NULL)
					pShareBolt = pEndBolt;
			}
		}
	}
	return TRUE;
}
#endif