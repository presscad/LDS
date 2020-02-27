#include "stdafx.h"
#include "DesignQuadSection.h"
#include "env_def.h"
#include "..\IntelliAgent\IntelliModel.h"
#include ".\StdComTempl\IStdComTempl.h"

#ifdef __RAPID_ROUGH_3DMODEL_
CDesignQuadSection::CDesignQuadSection()
{
	m_pDesignerContext = NULL;
}

CDesignQuadSection::~CDesignQuadSection()
{

}
typedef CLDSLineAngle* CLDSLineAnglePtr;
int LayoutAngleBolts(CLDSLineAngle* pAngle, CLDSNode* pBaseNode, BYTE ciWorkWingX0Y1, UINT uiBoltMd, BOLT_LAYOUT& layout, BOLTSET& boltset);
CLDSPlate *CDesignQuadSection::DesignCornerPlate(CLDSNode *pBaseNode, CLDSLinePart *pHorizRod1, 
											   CLDSLinePart *pHorizRod2, CLDSLinePart *pXieRod)
{
	if (pBaseNode == NULL || pHorizRod1 == NULL || pHorizRod2 == NULL || pXieRod == NULL ||
		!pHorizRod1->IsAngle() || !pHorizRod2->IsAngle() || !pXieRod->IsAngle())
		return NULL;
	//1.���㹲�ð����ӻ�׼�Ǹ�����֫
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
		{	//���ݺ�Ĺ���֫�����޶�work_norm
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
	//2.�жϵ�ǰ�ڵ㹲�ð��Ƿ��Ѿ���ƹ�
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
	//3.��ƹ��ð�
	CDesignLjPara designer;
	designer.m_bEnableFlexibleDesign = true;
	designer.m_hBaseNode = pBaseNode->handle;
	designer.m_hBasePart = pHorizAngle1->handle;
	//��׼��
	designer.origin.datum_pos_style = 3;	//�Ǹ����߽���
	designer.origin.des_para.AXIS_INTERS.hDatum1 = pHorizAngle1->handle;
	designer.origin.des_para.AXIS_INTERS.hDatum2 = pHorizAngle2->handle;
	designer.origin.des_para.AXIS_INTERS.wing_offset_style1 = 0;
	designer.origin.des_para.AXIS_INTERS.wing_offset_style2 = 0;
	//���߲���
	designer.norm.norm_style = 2;	//2.�˼������淨��
	designer.norm.hVicePart = pHorizAngle1->handle;
	designer.norm.hCrossPart = pHorizAngle2->handle;
	GEPOINT work_wing_norm = cWorkWingArr[0] == 'X' ? pHorizAngle1->vxWingNorm : pHorizAngle1->vyWingNorm;
	//if (work_wing_norm.z > 0)
	//	designer.norm.nearVector.Set(0, 0, 1);
	//else
	//	designer.norm.nearVector.Set(0, 0, -1);
	//
	CLDSBolt* pBolt;
	BOLTSET boltset;
	BOLT_LAYOUT layout;
	BYTE ciMainAngleBoltNum = 2, ciXieAngleBoltNum = 1;
	//��Ӹְ����Ӹ˼���������˨
	pPlate = (CLDSPlate*)Ta.Parts.append(CLS_PLATE);
	pPlate->SetLayer("SPG");
	pPlate->layer(0) = pHorizAngle1->Layer(0);
	pPlate->cfgword = pHorizAngle1->cfgword;
	pPlate->face_N = 1;
	//
	//��ӹ��ð��ϵĻ�׼�����Ӹ˼�
	CDesignLjPartPara* pConnAnglePara = NULL;
	GECS lcs;
	designer.origin.UpdatePos(pBaseNode->BelongModel());
	lcs.origin=designer.origin.xRsltPosition;
	designer.norm.UpdateVector(pBaseNode->BelongModel());
	lcs.axis_z=designer.norm.vector;
	lcs.axis_x=pHorizAngle1->xPosEnd-pHorizAngle1->xPosStart;
	lcs.axis_y=lcs.axis_z^lcs.axis_x;
	normalize(lcs.axis_x);
	normalize(lcs.axis_y);
	normalize(lcs.axis_z);
	GEPOINT xCutLineStart,xCutLineEnd;
	if(pHorizAngle1->pStart==pBaseNode)
		xCutLineStart=pHorizAngle1->xPosStart-pHorizAngle1->dfStartOdd*lcs.axis_x;
	else
		xCutLineStart=pHorizAngle1->xPosEnd+pHorizAngle1->dfStartOdd*lcs.axis_x;
	GEPOINT vStdLenVec2=pHorizAngle2->xPosEnd-pHorizAngle2->xPosStart;
	normalize(vStdLenVec2);
	if(pHorizAngle2->pStart==pBaseNode)
		xCutLineEnd=pHorizAngle2->xPosStart-pHorizAngle2->dfStartOdd*vStdLenVec2;
	else
		xCutLineEnd=pHorizAngle2->xPosEnd+pHorizAngle2->dfStartOdd*vStdLenVec2;
	xCutLineStart=lcs.TransPToCS(xCutLineStart);
	xCutLineEnd=lcs.TransPToCS(xCutLineEnd);
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
			if (cCurWorkWing == 'X'&&ddx[i] < 0)	//��Ĭ��Ϊ����
			{	//����
				pDesPos->wing_x_offset.gStyle = 4;
				pDesPos->wing_x_offset.offsetDist = -pPlate->Thick;
			}
			else if (cCurWorkWing == 'Y'&&ddy[i] < 0)
			{	//����
				pDesPos->wing_y_offset.gStyle = 4;
				pDesPos->wing_y_offset.offsetDist = -pPlate->Thick;
			}
		}
		//�����޶�һ�¸˼�����˨λ��
		pCurAngle->CalPosition(true);
		if(pCurAngle!=pHorizRod1&&pCurAngle!=pHorizRod2)
		{
			/////////////////////////////////////////////////////////
			//���㹲�ð������߲ĵĳ�ʼ��ͷֵ wjh-2019.12.15
			GELINE rayline(pCurAngle->xPosStart,pCurAngle->xPosEnd);
			rayline.start=lcs.TransPToCS(rayline.start);
			rayline.end=lcs.TransPToCS(rayline.end);
			GEPOINT inters2d,tippoint;
			Int2dll(f2dLine(xCutLineStart,xCutLineEnd),f2dLine(rayline.start,rayline.end),inters2d.x,inters2d.y);
			tippoint=lcs.TransPToCS(pConnAnglePara->start0_end1==0?pCurAngle->xPosStart:pCurAngle->xPosEnd);
			tippoint.z=0;
			int liInitOddment=ftoi(DISTANCE(inters2d,tippoint));
			if(liInitOddment%5>0)
				liInitOddment=liInitOddment-liInitOddment%5+5;
			/////////////////////////////////////////////////////////
			if (pCurAngle->pStart == pBaseNode)
			{
				pCurAngle->desStartOdd.m_iOddCalStyle=0;
				pCurAngle->desStartOdd.m_hRefPart1=pHorizRod1->handle;
				pCurAngle->desStartOdd.m_hRefPart2=pHorizRod2->handle;
				pCurAngle->CalStartOddment(0,-liInitOddment);
			}
			else if (pCurAngle->pEnd == pBaseNode)
			{
				pCurAngle->desEndOdd.m_iOddCalStyle=0;
				pCurAngle->desEndOdd.m_hRefPart1=pHorizRod1->handle;
				pCurAngle->desEndOdd.m_hRefPart2=pHorizRod2->handle;
				pCurAngle->CalEndOddment(0,-liInitOddment);
			}
		}
	}
	for (CLsRef* pLsRef = pPlate->GetFirstLsRef(); pLsRef; pLsRef = pPlate->GetNextLsRef())
		pLsRef->GetLsPtr()->correct_pos();
	designer.CloneTo(pPlate->designInfo);
	pPlate->DesignPlate();
	//4.ע�Ṳ�ð�
	if (pPlate&&m_pDesignerContext)
	{	//���ð���ע�����Σ�������ķֱ����ע�ᣩ
		CUnifyModelLayer::PLATE* pDesignPlate = m_pDesignerContext->hashPlates.Add(plate1.StrKey());
		if(pDesignPlate)
			pDesignPlate->hPlate = pPlate->handle;
		//
		if((pDesignPlate = m_pDesignerContext->hashPlates.Add(plate2.StrKey()))!=NULL)
			pDesignPlate->hPlate = pPlate->handle;
	}
	return pPlate;
}

static CLDSLinePart* _LocalGetSectPrimaryRod(CLDSNode* pStart,CLDSNode* pEnd)
{
	if(pStart==NULL || pEnd==NULL)
		return NULL;
	IModel* pModel=pStart->BelongModel();
	CLdsPartListStack stackparts(pModel);
	for(CLDSLinePart* pRod=pModel->EnumRodFirst();pRod;pRod=pModel->EnumRodNext())
	{
		if((pRod->pStart==pStart && pRod->pEnd==pEnd) ||
			(pRod->pStart==pEnd && pRod->pEnd==pStart))
			return pRod;
	}
	return NULL;
}
bool CDesignQuadSection::QUADSECT::IsInsideDiagRod(CLDSLinePart* pDiagRod,int indexOfCheckCorner)
{
	GEPOINT lenDiagRodVec;
	if (pDiagRod->pStart == xarrCornerNodes[indexOfCheckCorner])
		lenDiagRodVec=pDiagRod->pEnd->xOriginalPos-pDiagRod->pStart->xOriginalPos;
	else if(pDiagRod->pEnd == xarrCornerNodes[indexOfCheckCorner])
		lenDiagRodVec=pDiagRod->pStart->xOriginalPos-pDiagRod->pEnd->xOriginalPos;
	else
		return false;
	GEPOINT vs, ve;
	if(indexOfCheckCorner==0)
	{
		vs=xarrCornerNodes[2]->xOriginalPos-xarrCornerNodes[0]->xOriginalPos;
		ve=xarrCornerNodes[1]->xOriginalPos-xarrCornerNodes[0]->xOriginalPos;
	}
	else if(indexOfCheckCorner==1)
	{
		vs=xarrCornerNodes[0]->xOriginalPos-xarrCornerNodes[1]->xOriginalPos;
		ve=xarrCornerNodes[3]->xOriginalPos-xarrCornerNodes[1]->xOriginalPos;
	}
	else if(indexOfCheckCorner==2)
	{
		vs=xarrCornerNodes[3]->xOriginalPos-xarrCornerNodes[2]->xOriginalPos;
		ve=xarrCornerNodes[0]->xOriginalPos-xarrCornerNodes[2]->xOriginalPos;
	}
	else if(indexOfCheckCorner==3)
	{
		vs=xarrCornerNodes[1]->xOriginalPos-xarrCornerNodes[3]->xOriginalPos;
		ve=xarrCornerNodes[2]->xOriginalPos-xarrCornerNodes[3]->xOriginalPos;
	}
	else
		return false;
	GEPOINT vns = vs ^ lenDiagRodVec;
	GEPOINT vne = lenDiagRodVec ^ ve;
	return (vns*vne)>0&&(lenDiagRodVec*vs)>0&&(lenDiagRodVec*ve)>0;
}
long CDesignQuadSection::DesignQuadSects(CTower *pTower,bool blAddCornerPlate/*=true*/,CIntelliModelDesigner* pIntelliDesigner/*=NULL*/)
{
	QUADSECT _xarrQuadSects[200];
	ARRAY_LIST<QUADSECT> xarrQuadSects(_xarrQuadSects, 200, 20);
	m_pDesignerContext=pIntelliDesigner;
	long i, hits=0, count = RetriveQuadSections(pTower, &xarrQuadSects);
	for(i=0;i<count;i++)
	{
		QUADSECT sect = xarrQuadSects[i];
		for (int j = 0; j < 4; j++)
		{
			if (sect.xarrDiagRods[j] == NULL || !sect.xarrDiagRods[j]->IsAngle())
				continue;
			CMultiOffsetPos* pCurrTipPos = NULL;
			SmartRodPtr pSmartRod = sect.xarrDiagRods[j];
			if (pSmartRod->pStart == sect.xarrCornerNodes[j])
				pCurrTipPos = &pSmartRod.pAngle->desStartPos;
			else if (pSmartRod->pEnd == sect.xarrCornerNodes[j])
				pCurrTipPos = &pSmartRod.pAngle->desEndPos;
			else
				continue;
			CLDSLinePart* pPrimaryRod1, *pPrimaryRod2;
			if(j==0)
			{
				pPrimaryRod1 = sect.pFrontRod;
				pPrimaryRod2 = sect.pRightRod;
			}
			else if(j==1)
			{
				pPrimaryRod1 = sect.pFrontRod;
				pPrimaryRod2 = sect.pLeftRod;
			}
			else if(j==2)
			{
				pPrimaryRod1 = sect.pBackRod;
				pPrimaryRod2 = sect.pRightRod;
			}
			else //if(j==3)
			{
				pPrimaryRod1 = sect.pBackRod;
				pPrimaryRod2 = sect.pLeftRod;
			}
			if (pPrimaryRod1->IsAngle() && pPrimaryRod2->IsAngle())
			{
				pCurrTipPos->spatialOperationStyle = 0;
				pCurrTipPos->datumPoint.datum_pos_style = 3;
				pCurrTipPos->datumPoint.des_para.AXIS_INTERS.hDatum1 = pPrimaryRod1->handle;
				pCurrTipPos->datumPoint.des_para.AXIS_INTERS.hDatum2 = pPrimaryRod2->handle;
			}
			pSmartRod.pAngle->CalPosition();
			if (blAddCornerPlate)
				this->DesignCornerPlate(sect.xarrCornerNodes[j],pPrimaryRod1,pPrimaryRod2,pSmartRod);
			hits++;
		}
	}
	return hits;
}
long CDesignQuadSection::RetriveQuadSections(CTower *pTower, ARRAY_LIST<QUADSECT>* pxarrQuadSects)
{
	long liInitSectCount = pxarrQuadSects->Count;
	/*1.��ȡ����������ͼ
	������ʽ:'H'+'X'/'Y'+'������'+'-�Ҳ�ڵ���'
	��ȡԭ��:�����������Ľڵ�Գ�, ˮƽ���в���
	*/
	UCS_STRU ucs;
	CLDSNode* pNode;
	NODESET nodeset;
	for (pNode = pTower->EnumNodeFirst(); pNode; pNode = pTower->EnumNodeNext())
		nodeset.append(pNode);
	QUADSECT quadsect;
	CLDSLinePartPtr _xarrRods[20000];
	CLDSLinePart* pRod,*pMirRod;
	ARRAY_LIST<CLDSLinePartPtr> xarrRods(_xarrRods, 20000, 1000);
	for (pRod = pTower->EnumRodFirst(); pRod; pRod = pTower->EnumRodNext())
	{
		if (pRod->pStart == NULL || pRod->pEnd == NULL)
			continue;
		xarrRods.append(pRod);
	}
	for (pNode = nodeset.GetFirst(); pNode; pNode = nodeset.GetNext())
	{
		//1������ͼ�㼰�Գ�����ѡȡ��׼�ڵ�
		if (pNode->chLayer1st != 'S')			//�ڵ�λ������
			continue;
		if (pNode->xOriginalPos.x > 0 || pNode->xOriginalPos.y < 0)	//�Եڶ����޵Ľڵ�Ϊ��׼�ڵ�
			continue;
		CLDSLinePart* pFatherPart = pTower->FromRodHandle(pNode->hFatherPart);
		if (pFatherPart!=NULL&&pFatherPart->chLayer2nd != 'Z')		//�ڵ㸸�˼���������
			continue;
		CLDSNode* pMirX = pNode->GetMirNode(MIRMSG(1));
		CLDSNode* pMirY = pNode->GetMirNode(MIRMSG(2));
		CLDSNode* pMirZ = pNode->GetMirNode(MIRMSG(3));
		if (pMirX == NULL || pMirY == NULL || pMirZ == NULL)		//�������ԳƵĽڵ�
			continue;
		memset(quadsect.xarrDiagRods,0,sizeof(CLDSLinePartPtr)*4);
		quadsect.xarrCornerNodes[0]=pMirY;
		quadsect.xarrCornerNodes[1]=pNode;
		quadsect.xarrCornerNodes[2]=pMirZ;
		quadsect.xarrCornerNodes[3]=pMirX;
		quadsect.pFrontRod=_LocalGetSectPrimaryRod(quadsect.xarrCornerNodes[0],quadsect.xarrCornerNodes[1]);
		quadsect.pBackRod =_LocalGetSectPrimaryRod(quadsect.xarrCornerNodes[2],quadsect.xarrCornerNodes[3]);
		quadsect.pRightRod=_LocalGetSectPrimaryRod(quadsect.xarrCornerNodes[0],quadsect.xarrCornerNodes[2]);
		quadsect.pLeftRod =_LocalGetSectPrimaryRod(quadsect.xarrCornerNodes[1],quadsect.xarrCornerNodes[3]);
		if(quadsect.pFrontRod==NULL||quadsect.pBackRod==NULL||quadsect.pRightRod==NULL||quadsect.pLeftRod==NULL)
			continue;
		//2���жϺ�������Ƿ񲼲ģ����ο�����б�ģ�
		for (int j=0;j<xarrRods.Count;j++)
		{
			CLDSLinePart* pDiagRod = xarrRods[j];
			if (pDiagRod == quadsect.pFrontRod || pDiagRod == quadsect.pBackRod ||
				pDiagRod == quadsect.pRightRod || pDiagRod == quadsect.pLeftRod)
				continue;
			GEPOINT lenRodVec= pDiagRod->pEnd->xOriginalPos-pDiagRod->pStart->xOriginalPos;
			if (fabs(lenRodVec.z) > EPS2)
				continue;
			for (int k = 0; k < 4; k++)
			{
				if(quadsect.IsInsideDiagRod(pDiagRod,k))
					quadsect.xarrDiagRods[k] = pDiagRod;
			}
			if (quadsect.xarrDiagRods[0]&&quadsect.xarrDiagRods[1]&&
				quadsect.xarrDiagRods[2]&&quadsect.xarrDiagRods[3])
				break;
		}
		if (quadsect.xarrDiagRods[0]==NULL||quadsect.xarrDiagRods[1]==NULL)
			continue;
		pxarrQuadSects->append(quadsect);
		//logerr.Log("%5d{0x%X}���棬�ǲ�0x%X-0x%X", quadsect.xarrCornerNodes[0]->pointI,quadsect.xarrCornerNodes[0]->handle, quadsect.xarrDiagRods[0]->handle,quadsect.xarrDiagRods[1]->handle);
	}
	//2.��ȡͷ���������ͼ
	//��ȡԭ��:ͷ�����ļ�ǰ������ҶԳ�, չ�����в���,չ����������
	for (int i=0;i<xarrRods.Count;i++)
	{
		pRod = xarrRods[i];
		//1������ͼ�㼰�Գƹ�ϵ��ȡ��׼�˼�
		if (pRod->pStart == NULL || pRod->pEnd == NULL)		//�������˼�
			continue;
		if (pRod->chLayer1st != 'T'||pRod->chLayer3rd=='G')	//��ͷ������
			continue;
		if (pRod->GetLength() > 4000)
			continue;	//>4m����Ϊ�Ҹ�
		CLDSNode* pStart = pRod->pStart;
		CLDSNode* pEnd = pRod->pEnd;
		if (pStart->xOriginalPos.y < 0 || pEnd->xOriginalPos.y < 0)			//�Եڶ����޵ĸ˼�Ϊ��׼�˼�
			continue;
		if((pMirRod = pRod->GetMirRod(MIRMSG(1)))==NULL)
			continue;	//û�жԳƵ�����
		//2�����ݻ�׼�˼���ȡ�ᵣչ������ͼ�Ĺ�������
		CLDSNode* pMirTipStart = pMirRod->pStart, *pMirTipEnd = pMirRod->pEnd;
		if (pMirTipStart == NULL || pMirTipEnd == NULL)
			continue;
		long hStartParentRod = pRod->pStart->hFatherPart;
		long hEndParentRod = pRod->pEnd->hFatherPart;
		if (hStartParentRod == pRod->handle || hEndParentRod == pRod->handle)
			continue;
		GEPOINT lenRodVec= pEnd->xOriginalPos-pStart->xOriginalPos;
		normalize(lenRodVec);
		CLDSLinePart* pParentRod = pTower->FromRodHandle(hStartParentRod);
		if (pParentRod==NULL||pParentRod->pStart == NULL || pParentRod->pEnd == NULL)
			continue;
		GEPOINT lenParentRodVec = pParentRod->pEnd->xOriginalPos - pParentRod->pStart->xOriginalPos;
		normalize(lenParentRodVec);
		if (fabs(lenRodVec*lenParentRodVec) > EPS_COS2)
			continue;
		pParentRod = pTower->FromRodHandle(hEndParentRod);
		if (pParentRod==NULL||pParentRod->pStart == NULL || pParentRod->pEnd == NULL)
			continue;
		lenParentRodVec = pParentRod->pEnd->xOriginalPos - pParentRod->pStart->xOriginalPos;
		normalize(lenParentRodVec);
		if (fabs(lenRodVec*lenParentRodVec) > EPS_COS2)
			continue;

		quadsect.xarrCornerNodes[0]=pRod->pEnd;
		quadsect.xarrCornerNodes[1]=pRod->pStart;
		quadsect.xarrCornerNodes[2]=pMirTipEnd;
		quadsect.xarrCornerNodes[3]=pMirTipStart;
		quadsect.pFrontRod=_LocalGetSectPrimaryRod(quadsect.xarrCornerNodes[0],quadsect.xarrCornerNodes[1]);
		quadsect.pBackRod =_LocalGetSectPrimaryRod(quadsect.xarrCornerNodes[2],quadsect.xarrCornerNodes[3]);
		quadsect.pRightRod=_LocalGetSectPrimaryRod(quadsect.xarrCornerNodes[0],quadsect.xarrCornerNodes[2]);
		quadsect.pLeftRod =_LocalGetSectPrimaryRod(quadsect.xarrCornerNodes[1],quadsect.xarrCornerNodes[3]);
		if(quadsect.pFrontRod==NULL||quadsect.pBackRod==NULL||quadsect.pRightRod==NULL||quadsect.pLeftRod==NULL)
			continue;
		//3���жϺᵣչ�������Ƿ񲼲�(���ο�����б��)
		memset(quadsect.xarrDiagRods,0,sizeof(CLDSLinePartPtr)*4);
		//if (lenStdVec.z > 0)	//�Ǹַ�����ȡΪ����Ϊ��
		//	lenStdVec = -1.0*lenStdVec;
		ucs.axis_y = pMirTipStart->xOriginalPos-pStart->xOriginalPos;
		ucs.axis_z = cross_prod(lenRodVec, ucs.axis_y);
		normalize(ucs.axis_z);
		ucs.origin = pStart->xOriginalPos;
		for (int j=0;j<xarrRods.Count;j++)
		{
			CLDSLinePart* pDiagRod = xarrRods[j];
			if (pDiagRod == quadsect.pFrontRod || pDiagRod == quadsect.pBackRod ||
				pDiagRod == quadsect.pRightRod || pDiagRod == quadsect.pLeftRod)
				continue;
			lenRodVec= pDiagRod->pEnd->xOriginalPos-pDiagRod->pStart->xOriginalPos;
			if (fabs(lenRodVec*ucs.axis_z) > EPS2)
				continue;
			for (int k = 0; k < 4; k++)
			{
				if(quadsect.IsInsideDiagRod(pDiagRod,k))
					quadsect.xarrDiagRods[k] = pDiagRod;
			}
			if (quadsect.xarrDiagRods[0]&&quadsect.xarrDiagRods[1]&&
				quadsect.xarrDiagRods[2]&&quadsect.xarrDiagRods[3])
				break;
		}
		if (quadsect.xarrDiagRods[0]==NULL&&quadsect.xarrDiagRods[1]==NULL&&
			quadsect.xarrDiagRods[2]==NULL&&quadsect.xarrDiagRods[3]==NULL)
			continue;
		//4������ͷ���������ͼ
		pxarrQuadSects->append(quadsect);
		//long hDiagRod1 = quadsect.xarrDiagRods[0] != NULL ? quadsect.xarrDiagRods[0]->handle : 0;
		//long hDiagRod2 = quadsect.xarrDiagRods[1] != NULL ? quadsect.xarrDiagRods[1]->handle : 0;
		//logerr.Log("T%5d{0x%X}����0x%X���ǲ�0x%X-0x%X", quadsect.xarrCornerNodes[0]->pointI,quadsect.xarrCornerNodes[0]->handle, 
		//	quadsect.pFrontRod->handle,hDiagRod1,hDiagRod2);
	}
	return pxarrQuadSects->Count-liInitSectCount;
}
#endif
