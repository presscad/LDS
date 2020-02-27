#include "stdafx.h"
#include "DesignEndBraceTemp.h"
#include "env_def.h"

//////////////////////////////////////////////////////////////////////////
//CFootTempl::BASEPLATE
CEndBraceTempl::BASEPLATE::BASEPLATE()
{
	cMaterial=CSteelMatLibrary::Q235BriefMark();
	thick=16;
	nNormOffset=0;
	fR=300;
	pPlate=NULL;
}
//////////////////////////////////////////////////////////////////////////
//CFootTempl::ENDPLATE
CEndBraceTempl::ENDPLATE::ENDPLATE()
{
	cMaterial=CSteelMatLibrary::Q235BriefMark();
	thick=16;
	C=50;
	r=100;
	R=200;
	H=400;
	ibPosVec=0;
	pPlate=NULL;
}
//////////////////////////////////////////////////////////////////////////
//CFootTempl::SPILEPLATE
CEndBraceTempl::SPILEPLATE::SPILEPLATE()
{
	cMaterial=CSteelMatLibrary::Q235BriefMark();
	thick=16;
	H=0;
	pPlate=NULL;
	ibPosVec=0;
}
//////////////////////////////////////////////////////////////////////////
//CFootTempl
CEndBraceTempl::CEndBraceTempl()
{
	m_fEndPlateDist=40;
	m_hBaseNode=0;
	m_hBaseTube=0;
	m_iEndPlateType=1;
	m_iSpilePlateType=1;
}
f3dPoint CEndBraceTempl::CalPosition()
{
	f3dPoint origin;
	CLDSNode* pBaseNode=console.FromNodeHandle(m_hBaseNode);
	if(pBaseNode==NULL)
		return origin;
	CLDSLineTube* pBaseTube=(CLDSLineTube*)console.FromPartHandle(m_hBaseTube);
	if(pBaseTube==NULL)
		return origin;
	f3dPoint startPt,endPt,line_vec;
	startPt=pBaseTube->Start();
	endPt=pBaseTube->End();
	line_vec=endPt-startPt;
	normalize(line_vec);
	if(pBaseTube->pStart->handle==m_hBaseNode)
		origin=startPt-line_vec*pBaseTube->startOdd();
	else
		origin=endPt+line_vec*pBaseTube->endOdd();
	return origin;
}
void CEndBraceTempl::RetrieveAttachPlates()
{
	//��ȡ�˰�
	listEndPlates.Empty();
	ENDPLATE* pEndPlate=NULL;
	if(m_iEndPlateType==1)
	{	//X��Ķ˰�
		pEndPlate=listEndPlates.append();
		pEndPlate->ibPosVec=0;	//X+
		pEndPlate=listEndPlates.append();
		pEndPlate->ibPosVec=1;	//X-
	}
	else
	{	//Y��Ķ˰�
		pEndPlate=listEndPlates.append();
		pEndPlate->ibPosVec=2;	//Y+
		pEndPlate=listEndPlates.append();
		pEndPlate->ibPosVec=3;	//Y-
	}
	//��ȡ���
	listSpilePlates.Empty();
	SPILEPLATE* pSilePlate=NULL;
	if(m_iSpilePlateType==1)
	{	//
		pSilePlate=listSpilePlates.append();
		pSilePlate->ibPosVec=0;	//X
		pSilePlate=listSpilePlates.append();
		pSilePlate->ibPosVec=4;	//Y+
		pSilePlate=listSpilePlates.append();
		pSilePlate->ibPosVec=5;	//Y-
	}
	else if(m_iSpilePlateType==2)
	{
		pSilePlate=listSpilePlates.append();
		pSilePlate->ibPosVec=3;	//Y
		pSilePlate=listSpilePlates.append();
		pSilePlate->ibPosVec=1;	//X+
		pSilePlate=listSpilePlates.append();
		pSilePlate->ibPosVec=2;	//X-
	}
}
BOOL CEndBraceTempl::DesignBasePlate()
{
	CLDSNode* pBaseNode=console.FromNodeHandle(m_hBaseNode);
	if(pBaseNode==NULL)
		return FALSE;
	CLDSLineTube* pBaseTube=(CLDSLineTube*)console.FromPartHandle(m_hBaseTube,CLS_LINETUBE);
	if(pBaseTube==NULL)
		return FALSE;
	CLDSParamPlate* pParamPlate =(CLDSParamPlate*)Ta.Parts.append(CLS_PARAMPLATE);
	m_xBasePlate.pPlate=pParamPlate;
	pParamPlate->m_iParamType=TYPE_BASEPLATE;	//����
	pParamPlate->m_bDisplayHole = true;
	pParamPlate->cMaterial=m_xBasePlate.cMaterial;
	pParamPlate->SetPartNo(m_xBasePlate.sPartNo);
	pParamPlate->thick=m_xBasePlate.thick;
	pParamPlate->CopyModuleInstanceInfo(pBaseTube);
	SEGI iSeg;
	if(strlen(m_xBasePlate.sPartNo)>0&&ParsePartNo(m_xBasePlate.sPartNo,&iSeg,NULL))
		pParamPlate->iSeg=iSeg;
	else
		pParamPlate->iSeg=pBaseTube->iSeg;
	//
	pParamPlate->SetDesignItemValue('A',m_hBaseNode,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);
	pParamPlate->SetDesignItemValue('L',m_hBaseTube,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);
	pParamPlate->SetDesignItemValue('N',m_xBasePlate.nNormOffset,NULL,PARAM_POS);	//����ƫ��
	pParamPlate->SetDesignItemValue('S',1,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER);		//�װ�����
	pParamPlate->SetDesignItemValue('R',m_xBasePlate.fR,NULL,PARAM_SHAPE);	//�뾶
	f3dPoint origon=CalPosition();
	pParamPlate->design.origin.SetPosition(origon);
	pParamPlate->DesignPlate();
	return TRUE;
}

BOOL CEndBraceTempl::DesignStationConnPlates()
{
	if(m_xBasePlate.pPlate==NULL)
		return FALSE;
	for(ENDPLATE* pEndPlate=listEndPlates.GetFirst();pEndPlate;pEndPlate=listEndPlates.GetNext())
	{
		CLDSParamPlate* pParamPlate =(CLDSParamPlate*)Ta.Parts.append(CLS_PARAMPLATE);
		pEndPlate->pPlate=pParamPlate;
		pParamPlate->m_hPartWeldParent=m_xBasePlate.pPlate->handle;
		pParamPlate->CopyModuleInstanceInfo(m_xBasePlate.pPlate);
		pParamPlate->m_iParamType=TYPE_STATIONCONNPLATE;
		pParamPlate->cMaterial=pEndPlate->cMaterial;
		pParamPlate->thick=pEndPlate->thick;
		pParamPlate->SetPartNo(pEndPlate->sPartNo);
		SEGI iSeg;
		if(strlen(pEndPlate->sPartNo)>0&&ParsePartNo(pEndPlate->sPartNo,&iSeg,NULL))
			pParamPlate->iSeg=iSeg;
		else
			pParamPlate->iSeg=m_xBasePlate.pPlate->iSeg;
		//���β���
		pParamPlate->SetDesignItemValue('H',pEndPlate->H,NULL,PARAM_SHAPE);
		pParamPlate->SetDesignItemValue('C',pEndPlate->C,NULL,PARAM_SHAPE);
		pParamPlate->SetDesignItemValue('r',pEndPlate->r,NULL,PARAM_SHAPE);
		pParamPlate->SetDesignItemValue('R',pEndPlate->R,NULL,PARAM_SHAPE);
		//װ�����
		pParamPlate->SetDesignItemValue(KEY2C("LP"),m_xBasePlate.pPlate->handle,NULL,0,DESIGN_PARAM_ITEM::TYPE_HANDLE);
		pParamPlate->SetDesignItemValue('I',pEndPlate->ibPosVec,NULL,0,DESIGN_PARAM_ITEM::TYPE_INTEGER);
		pParamPlate->SetDesignItemValue('D',m_fEndPlateDist*0.5,NULL,PARAM_POS);
		pParamPlate->DesignPlate(false);
	}
	return TRUE;
}
CEndBraceTempl::SPILEPLATE* CEndBraceTempl::FindAssistPlate(SPILEPLATE* pDesPlate)
{
	if(pDesPlate==NULL)
		return NULL;
	SPILEPLATE* pAssistPlate=NULL;
	for(pAssistPlate=listSpilePlates.GetFirst();pAssistPlate;pAssistPlate=listSpilePlates.GetNext())
	{
		if(pDesPlate->ibPosVec==1|| pDesPlate->ibPosVec==2)		//X+��X-
		{
			if(pAssistPlate->ibPosVec==3)	//Yͨ
				break;
		}
		if(pDesPlate->ibPosVec==4 || pDesPlate->ibPosVec==5)	//Y+��Y-		
		{
			if(pAssistPlate->ibPosVec==0)	//Xͨ
				break;
		}
	}
	return pAssistPlate;
}
BOOL CEndBraceTempl::DesignStationCrossPlates()
{
	if(m_xBasePlate.pPlate==NULL)
		return FALSE;
	for(SPILEPLATE* pSpilePlate=listSpilePlates.GetFirst();pSpilePlate;pSpilePlate=listSpilePlates.GetNext())
	{
		int nPush=listSpilePlates.push_stack();
		CLDSParamPlate* pParamPlate =(CLDSParamPlate*)Ta.Parts.append(CLS_PARAMPLATE);
		pSpilePlate->pPlate=pParamPlate;
		pParamPlate->m_hPartWeldParent=m_xBasePlate.pPlate->handle;
		pParamPlate->CopyModuleInstanceInfo(m_xBasePlate.pPlate);
		pParamPlate->m_iParamType=TYPE_STATIONCROSSPLATE;
		pParamPlate->cMaterial=pSpilePlate->cMaterial;
		pParamPlate->thick=pSpilePlate->thick;
		pParamPlate->SetPartNo(pSpilePlate->sPartNo);
		SEGI iSeg;
		if(strlen(pSpilePlate->sPartNo)>0&&ParsePartNo(pSpilePlate->sPartNo,&iSeg,NULL))
			pParamPlate->iSeg=iSeg;
		else
			pParamPlate->iSeg=m_xBasePlate.pPlate->iSeg;
		//��Ʋ���
		pParamPlate->SetDesignItemValue(KEY2C("LP"),m_xBasePlate.pPlate->handle,NULL,0,DESIGN_PARAM_ITEM::TYPE_HANDLE);
		pParamPlate->SetDesignItemValue('I',pSpilePlate->ibPosVec,NULL,0,DESIGN_PARAM_ITEM::TYPE_INTEGER);
		if(pSpilePlate->ibPosVec!=0&&pSpilePlate->ibPosVec!=3)
		{
			SPILEPLATE* pAssistPlate=FindAssistPlate(pSpilePlate);
			if(pAssistPlate&&pAssistPlate->pPlate)
				pParamPlate->SetDesignItemValue(KEY2C("LA"),pAssistPlate->pPlate->handle,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);
		}
		pParamPlate->SetDesignItemValue('H',pSpilePlate->H,NULL,PARAM_SHAPE);
		pParamPlate->DesignPlate(false);
		listSpilePlates.pop_stack(nPush);
	}
	return TRUE;
}
BOOL CEndBraceTempl::DesignEndBrace()
{
	//���ɶ���
	if(!DesignBasePlate())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Base plates design failed!");
#else
		AfxMessageBox("�������ʧ��!");
#endif
		return FALSE;
	}
	//��ȡ���Ӱ�
	RetrieveAttachPlates();
	//���ɶ˰�
	DesignStationConnPlates();
	//���ɲ��
	DesignStationCrossPlates();
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
CPillorPlywoodTemp::CPillorPlywoodTemp()
{
	m_hBasePlate=0;
	m_hEndPlate=0;
	R=200;
	H=850;
	A=50;
	B=150;
	fCutR=150;
	thick=20;
	m_pPlate=NULL;
}
BOOL CPillorPlywoodTemp::DesignStationShoePlate()
{
	CLDSGeneralPlate* pBasePlate=(CLDSGeneralPlate*)console.FromPartHandle(m_hBasePlate);
	if(pBasePlate==NULL)
		return FALSE;
	CLDSParamPlate* pParamPlate =(CLDSParamPlate*)Ta.Parts.append(CLS_PARAMPLATE);
	m_pPlate=pParamPlate;
	pParamPlate->m_iParamType=TYPE_STATIONSHOEPLATE;	//����
	pParamPlate->m_bDisplayHole = true;
	pParamPlate->cMaterial=cMaterial;
	pParamPlate->SetPartNo(sPartNo);
	pParamPlate->thick=thick;
	pParamPlate->CopyModuleInstanceInfo(pBasePlate);
	SEGI iSeg;
	if(strlen(sPartNo)>0&&ParsePartNo(sPartNo,&iSeg,NULL))
		pParamPlate->iSeg=iSeg;
	else
		pParamPlate->iSeg=pBasePlate->iSeg;
	//��λ����
	CLDSParamPlate* pEndPlate=(CLDSParamPlate*)console.FromPartHandle(m_hEndPlate);
	pParamPlate->SetDesignItemValue(KEY2C("LB"),m_hBasePlate,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);
	pParamPlate->SetDesignItemValue(KEY2C("LE"),m_hEndPlate,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);
	//���β���
	if(pEndPlate&&pEndPlate->m_iParamType==TYPE_STATIONCONNPLATE)
	{
		pParamPlate->SetDesignItemValue('R',R,NULL,PARAM_SHAPE);	//�뾶
		double r=0;
		if(pEndPlate->GetDesignItemValue('r',&r))
			pParamPlate->SetDesignItemValue('r',r,NULL,PARAM_SHAPE);	//�ڻ��뾶
	}
	else
		pParamPlate->SetDesignItemValue('H',H,NULL,PARAM_SHAPE);	//���
	pParamPlate->SetDesignItemValue('A',A,NULL,PARAM_SHAPE);		//����
	pParamPlate->SetDesignItemValue('B',B,NULL,PARAM_SHAPE);	//����ƫ�ƾ�
	pParamPlate->SetDesignItemValue(KEY2C("CR"),fCutR,NULL,PARAM_SHAPE);		//���ǰ뾶
	pParamPlate->DesignPlate();
	return TRUE;
}