#include "stdafx.h"
#include "Tower.h"
#include "UniWirePointModel.h"
#include "ElectricTower.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////////////
//���ļ�����ʵ���������صĹ��ܺ�������ҿ���ȡ����ߡ���Ե��Ԥ����
//////////////////////////////////////////////////////////////////////////////
CExterDataModel gxExterModel;
CUniWireModel gxWirePointModel;
CTowerGimInfo gxTowerGimInfo;
static const double EPS_001=0.001;
int XHWIREPOINT::Compare(const ICompareClass *pCompareObj)const
{
	XHWIREPOINT* pWirePoint=(XHWIREPOINT*)pCompareObj;
	//���Ϻ���
	if(this->xBasicPoint.z<pWirePoint->xBasicPoint.z-EPS_001)
		return -1;
	else if(this->xBasicPoint.z>pWirePoint->xBasicPoint.z+EPS_001)
		return  1;
	//�������
	else if(fabs(xBasicPoint.x)>fabs(pWirePoint->xBasicPoint.x)+EPS_001)
		return -1;
	else if(fabs(xBasicPoint.x)<fabs(pWirePoint->xBasicPoint.x)-EPS_001)
		return  1;
	//�������
	else if(xBasicPoint.x<0&&pWirePoint->xBasicPoint.x>0)
		return -1;
	else if(xBasicPoint.x>0&&pWirePoint->xBasicPoint.x<0)
		return  1;
	//��ǰ���
	else if(fabs(xBasicPoint.y)>fabs(pWirePoint->xBasicPoint.y)+EPS_001)
		return -1;
	else if(fabs(xBasicPoint.y)<fabs(pWirePoint->xBasicPoint.y)-EPS_001)
		return  1;
	else if(xBasicPoint.y>0&&pWirePoint->xBasicPoint.y<0)
		return -1;
	else if(xBasicPoint.y<0&&pWirePoint->xBasicPoint.x>0)
		return  1;
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
bool CTower::RetrieveWireModelFromNodes(CUniWireModel* pWireModel)
{
#if defined(__RAPID_ROUGH_3DMODEL_)&&defined(__PART_DESIGN_INC_)
	CLDSNode* pNode=NULL;
	pWireModel->m_hashWirePoints.Empty();
	pWireModel->m_bGimWireModelInherited=true;
#ifdef __PART_DESIGN_INC_
	CWorkStatus *pStatus = GetActiveWorkStatus();
#endif
	for(pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		if(pNode->m_cHangWireType!='C'&&pNode->m_cHangWireType!='E'&&pNode->m_cHangWireType!='J')
			continue;
		CXhWirePoint* pWirePoint=pWireModel->m_hashWirePoints.Append();
		pWirePoint->m_ciLevel=0;
		pWirePoint->xPosition=pNode->xOriginalPos;
		pWirePoint->m_xWirePlace.ciWireType=pNode->m_cHangWireType;
		pWirePoint->m_hRelaNode=pNode->handle;
		pWirePoint->m_sGimName.Copy(pNode->m_sHangWireDesc);
		CExternalNodeLoad *pLoad = (pStatus != NULL) ? pStatus->hashNodeLoad.GetValue(pNode->handle) : NULL;
		if (pLoad)
		{
			if (pStatus->m_bLoadIncFactor)
				pWirePoint->m_vSketchArrow = pLoad->designLoad;
			else
				pWirePoint->m_vSketchArrow = pLoad->Fxyz;
		}
		//pWirePoint->szInsulatorStyleName
		{
			//CXhChar50 szWireName="����";
			//if(pNode->wireplace.ciWireType=='J')
			//	szWireName.Copy("����");
			//else if(pNode->wireplace.ciWireType=='E')
			//	szWireName.Copy("����");
			//if(pNode->m_sHangWireDesc.GetLength()<=0)
			//	pNode->UpdateWireDesc();
			//if(pNode->m_sHangWireDesc.GetLength()>0)
			//	szWireName=pNode->m_sHangWireDesc;
		}
	}
#endif
	return true;
}
UINT CTower::RetrieveWireHoleSet(CUniWireModel* pWireModel)
{
	SmartPartPtr pBoltPart,pSmartPart;
	CHashSet<CLDSBolt*> hashHoleBolts;
	//�Զ����ҹҿ׶��γɹҵ����CUniWirePoint
	for(pBoltPart=EnumPartFirst(CLS_BOLT);pBoltPart.IsHasPtr();pBoltPart=EnumPartNext(CLS_BOLT))
	{
		if(!pBoltPart.pBolt->m_bVirtualPart||pBoltPart.pBolt->m_cFuncType!=CLDSBolt::FUNC_WIREHOLE)
			continue;
		hashHoleBolts.SetValue(pBoltPart->handle,pBoltPart.pBolt);
	}
	XHWIREPOINT* pWirePoint;
	CXhSimpleList<XHWIREPOINT> listWirePoints;
	for(pSmartPart=EnumPartFirst();pSmartPart.blValid;pSmartPart=EnumPartNext())
	{
		if(pSmartPart->GetClassTypeId()!=CLS_LINEANGLE&&!pSmartPart->IsPlate())
			continue;
		CLsRefList* pLsRefList=pSmartPart->GetLsRefList();
		for(CLsRef* pHole=pLsRefList->GetFirst();pHole;pHole=pLsRefList->GetNext())
		{
			if(!pHole->pBolt->m_bVirtualPart||hashHoleBolts.GetValue(pHole->pBolt->handle)==NULL)
				continue;
			int wingx0_y1=0;
			XHWIREPOINT wirepoint;
			wirepoint.sfHoleD=(float)(pHole->pBolt->d+pHole->pBolt->hole_d_increment);
			if(wirepoint.blWireAngle=pSmartPart->IsAngle())
			{
				pSmartPart->GetBoltIntersPos(pHole->pBolt,wirepoint.xBasicPoint,&wingx0_y1,CLDSPart::DATUM_PLANE);
				wirepoint.vStdBasicAxisVec=wingx0_y1==1?pSmartPart.pAngle->vyWingNorm:pSmartPart.pAngle->vxWingNorm;
			}
			else
			{
				wirepoint.vStdBasicAxisVec=pHole->pBolt->ucs.axis_z;
				pSmartPart->GetBoltIntersPos(pHole->pBolt,wirepoint.xBasicPoint,&wingx0_y1,CLDSPlate::MID_PLANE);
			}
			normalize(wirepoint.vStdBasicAxisVec);
			for(pWirePoint=listWirePoints.EnumObjectFirst();pWirePoint;pWirePoint=listWirePoints.EnumObjectNext())
			{
				if(pWirePoint->blWireAngle!=wirepoint.blWireAngle)
					continue;
				if(fabs(pWirePoint->sfHoleD-wirepoint.sfHoleD)>0.1)
					continue;	//�ҿ״�С����һ��
				if(fabs(pWirePoint->vStdBasicAxisVec*wirepoint.vStdBasicAxisVec)<0.99)	//cos(8��)=0.99
					continue;	//
				GEPOINT vxDistVec=wirepoint.xBasicPoint-pWirePoint->xBasicPoint;
				double distance=vxDistVec*pWirePoint->vStdBasicAxisVec;
				GEPOINT vxHoriPrj=vxDistVec-pWirePoint->vStdBasicAxisVec*distance;
				if(vxHoriPrj.mod2()>1)
					continue;	//���ҿ�ͶӰ����볬��1mm�������Χ
				if((pWirePoint->blWireAngle&&distance<25)||fabs(distance)<25||fabs(distance)>300)
					continue;	//25ΪԤ������С��ȣ�300ΪԤ���������
				if(pWirePoint->hlCoupl.hResidePart==0)
				{
					pWirePoint->hlCoupl.hBolt=pHole->pBolt->handle;
					pWirePoint->hlCoupl.hResidePart=pSmartPart->handle;
					if(distance<0)	//����ʼ����vStdBasicAxisVecΪָ����һ������
						pWirePoint->vStdBasicAxisVec*=-1;
					pWirePoint->dfSpaceWidth=fabs(distance);
					break;
				}
				else if(fabs(distance)<pWirePoint->dfSpaceWidth)
				{	//��ǰ�ҿ���֮ǰ�ҿ׾�����������ʺϽ�ϳ�һ�Թҿ�
					//��֮ǰ����Կ�����Ϊ��һ�ҵ�Ļ�׼��
					XHWIREPOINT tmp;
					tmp.sfHoleD=pWirePoint->sfHoleD;
					tmp.blWireAngle=pWirePoint->blWireAngle;
					tmp.holes[0]=pWirePoint->holes[1];
					tmp.xBasicPoint=pWirePoint->xBasicPoint+pWirePoint->vStdBasicAxisVec*pWirePoint->dfSpaceWidth;
					tmp.vStdBasicAxisVec=-pWirePoint->vStdBasicAxisVec;
					listWirePoints.Append(tmp);
					//����ǰ�ҿ����Ϊ��ǰ�ҵ����Կ�
					pWirePoint->hlCoupl.hBolt=pHole->pBolt->handle;
					pWirePoint->hlCoupl.hResidePart=pSmartPart->handle;
					if(distance<0)	//����ʼ����vStdBasicAxisVecΪָ����һ������
						pWirePoint->vStdBasicAxisVec*=-1;
					pWirePoint->dfSpaceWidth=fabs(distance);
					break;
				}
			}
			if(pWirePoint==NULL)
			{
				wirepoint.hlBasic.hBolt=pHole->pBolt->handle;
				wirepoint.hlBasic.hResidePart=pSmartPart->handle;
				listWirePoints.Append(wirepoint);
			}
		}
	}
	//ɸѡ�ҿ�
	ARRAY_LIST<XHWIREPOINT> xarrWirePoints(0,listWirePoints.Count);
	for(pWirePoint=listWirePoints.EnumObjectFirst();pWirePoint;pWirePoint=listWirePoints.EnumObjectNext())
	{
		if(pWirePoint->blWireAngle&&pWirePoint->hlCoupl.hResidePart==0)
			continue;	//���߽Ǹ��ϵĹҿ��ǳɶԵģ������ܴ��ڵ���
		else if(pWirePoint->sfHoleD<16)	//�ҿ׿׾���ӦС��16��������Ϊ���׽ӵط����׻�������ƿ׵�
			continue;
		xarrWirePoints.append(*pWirePoint);
	}	
	//�Թҵ�����Ȼ�����λ�ý��з��鲢��ӵ��ҵ�ģ����
	CHeapSort<XHWIREPOINT>::HeapSortClassic(xarrWirePoints.m_pData,xarrWirePoints.GetSize());
	CXhWirePoint *pWirePointObj=NULL,*pPrevWirePointObj=NULL,*pPrevWirePointObjGroup=NULL;
	int nWireGroupCount=0;
	pWireModel->m_hashWirePoints.Empty();
	for(int i=0;i<xarrWirePoints.GetSize();i++)
	{	//�������ʵ�ʵ���������CXhWirePoint
		pWirePoint=&xarrWirePoints[i];
		pWirePointObj=pWireModel->m_hashWirePoints.Add(0);
		//pWirePointObj->m_hLdsObj=GetNewHandle();
		pWirePointObj->holes[0].hBolt=pWirePoint->hlBasic.hBolt;
		pWirePointObj->holes[0].hResidePart=pWirePoint->hlBasic.hResidePart;
		pWirePointObj->holes[1].hBolt=pWirePoint->hlCoupl.hBolt;
		pWirePointObj->holes[1].hResidePart=pWirePoint->hlCoupl.hResidePart;
		pWirePointObj->m_bRelyAngle=pWirePoint->blWireAngle;
		pWirePointObj->m_fHoleD=pWirePoint->sfHoleD;
		pWirePointObj->m_fSpaceWidth=(float)pWirePoint->dfSpaceWidth;
		pWirePointObj->m_vStdBasicAxisVec=pWirePoint->vStdBasicAxisVec;
		pWirePointObj->m_xBasicPoint=pWirePoint->xBasicPoint;
		if(pPrevWirePointObj==NULL)
		{
			pWirePointObj->m_xWirePlace.ciWireType=WIREPLACE_CODE::EARTH_WIRE;//'E';
			pWirePointObj->m_xWirePlace.ciPhaseSerial=0;
			pWirePointObj->m_xWirePlace.ciCircuitSerial=0;
			pPrevWirePointObjGroup=pPrevWirePointObj=pWirePointObj;
		}
		else if(pWirePoint->xBasicPoint.z-pPrevWirePointObj->m_xBasicPoint.z<3000)
		{
			pWirePointObj->m_xWirePlace=pPrevWirePointObj->m_xWirePlace;
			if(pWirePointObj->m_xBasicPoint.x-pPrevWirePointObj->m_xBasicPoint.z>3000)
			{
				pPrevWirePointObjGroup=pPrevWirePointObj=pWirePointObj;
				if(pWirePointObj->m_xWirePlace.ciWireType!='E')
					nWireGroupCount++;
			}
		}
		else
		{
			pWirePointObj->m_xWirePlace.ciWireType=WIREPLACE_CODE::CONDUCTOR_WIRE;//'C';
			pWirePointObj->m_xWirePlace.ciPhaseSerial=1;
			pWirePointObj->m_xWirePlace.ciCircuitSerial=1;
			pPrevWirePointObjGroup=pPrevWirePointObj=pWirePointObj;
			nWireGroupCount++;
		}
	};
	for(pWirePointObj=pWireModel->m_hashWirePoints.GetFirst();pWirePointObj;pWirePointObj=pWireModel->m_hashWirePoints.GetNext())
		pWirePointObj->Create3dSolidModel();
	//�Զ��Թҵ��������γɹҵ����CWireConnPoint
	//�ڹҵ��������װ�ɶԽӵ����CWireConnPoint
	//��ʼ�����߷�����Ĭ��Ϊ500kv����˫��·
	//if(nWireGroupCount<=2)	//ֱ��
	//	pWireModel->InitWireLayout(1,true,800,2);
	//else if(nWireGroupCount==3)
	//	pWireModel->InitWireLayout(1,false,500,2);
	//else if(nWireGroupCount==6)
	//	pWireModel->InitWireLayout(2,false,500,2);
	//else if(nWireGroupCount==12)
	//	pWireModel->InitWireLayout(4,false,500,2);
	//else
	//	pWireModel->InitWireLayout(nWireGroupCount/3,false,500,2);
	/*
	//�������װ��Խӵ�
	CWireConnPoint* pWireConnPoint=model.hashConnPoints.Add(1);
	pWireConnPoint->AppendWirePoint(1);	//��ӵ��߶Խӵ�
	//�������¶Խӵ��������������
	CPhaseWire* pPhaseWire=model.hashPhaseWires.GetValue(3);
	pPhaseWire->AppendWireConnPoint(pWireConnPoint->Id);
	*/
	return 0;
}
//////////////////////////////////////////////////////////////////////////
//CTowerGimInfo
extern void UTF8ToANSI(const char* src_str, char* des_str);
extern void ANSIToUnicode(const char* src_str, wchar_t* des_str);
CTowerGimInfo::CTowerGimInfo()
{
	m_ciCodingType = 0;
}
BOOL CTowerGimInfo::ReadFamFile(const char* sFamFile)
{
	FILE *fp = fopen(sFamFile, "rt");
	if (fp == NULL)
	{
		logerr.Log("%s��ʧ��!", sFamFile);
		return FALSE;
	}
	char sLine1[MAX_PATH] = "", sLine2[MAX_PATH] = "";
	while (!feof(fp))
	{
		if (fgets(sLine1, MAX_PATH, fp) == NULL)
			continue;
		if (m_ciCodingType == 0)
			UTF8ToANSI(sLine1, sLine2);
		else
			strcpy(sLine2, sLine1);
		CXhChar100 sText(sLine2);
		sText.Replace('\t', ' ');
		sText.Replace('\n', ' ');
		sText.Remove(' ');
		char *skey1 = strtok(sText, "=");
		char *skey2 = strtok(NULL, "=");
		char *skey = strtok(NULL, "=");
		if (skey == NULL || strlen(skey) <= 0 || skey[0] == 10)
			continue;
		if (strstr(skey1, "VOLTAGE") || stricmp(skey2, "��ѹ�ȼ�") == 0)
			m_xTowerGimPropFile.m_sVoltGrade.Copy(skey);
		else if (stricmp(skey1, "TYPE") == 0 || stricmp(skey2, "����") == 0)
			m_xTowerGimPropFile.m_sType.Copy(skey);
		else if (stricmp(skey1, "TEXTURE") == 0 || stricmp(skey2, "��������") == 0)
			m_xTowerGimPropFile.m_sTexture.Copy(skey);
		else if (stricmp(skey1, "FIXEDTYPE") == 0 || stricmp(skey2, "�̶���ʽ") == 0)
			m_xTowerGimPropFile.m_sFixedType.Copy(skey);
		else if (stricmp(skey1, "TOWERTYPE") == 0 || stricmp(skey2, "��������") == 0)
			m_xTowerGimPropFile.m_sTaType.Copy(skey);
		else if (stricmp(skey1, "CIRCUIT") == 0 || stricmp(skey2, "��·��") == 0)
			m_xTowerGimPropFile.m_nCircuit = atoi(skey);
		else if (stricmp(skey1, "CONDUCTOR") == 0 || stricmp(skey2, "�����ͺ�") == 0)
			m_xTowerGimPropFile.m_sCWireSpec.Copy(skey);
		else if (stricmp(skey1, "GROUNDWIRE") == 0 || stricmp(skey2, "�����ͺ�") == 0)
			m_xTowerGimPropFile.m_sEWireSpec.Copy(skey);
		else if (stricmp(skey1, "REFERENCEWINDSPEED") == 0 || stricmp(skey2, "��ƻ�������") == 0)
			m_xTowerGimPropFile.m_fWindSpeed = atof(skey);
		else if (stricmp(skey1, "DESIGNICETHICKNESS") == 0 || stricmp(skey2, "��Ƹ������") == 0)
			m_xTowerGimPropFile.m_fNiceThick = atof(skey);
		else if (stricmp(skey1, "DESIGNWINDSPAN") == 0 || stricmp(skey2, "���ˮƽ����") == 0)
			m_xTowerGimPropFile.m_sWindSpan.Copy(skey);
		else if (stricmp(skey1, "DESIGNWEIGHTSPAN") == 0 || stricmp(skey2, "��ƴ�ֱ����") == 0)
			m_xTowerGimPropFile.m_sWeightSpan.Copy(skey);
		else if (stricmp(skey1, "DESIGNFRONTRULINGSPAN") == 0 || stricmp(skey2, "���ǰ�������") == 0)
			m_xTowerGimPropFile.m_fFrontRulingSpan = atof(skey);
		else if (stricmp(skey1, "DESIGNBACKRULINGSPAN") == 0 || stricmp(skey2, "��ƺ�������") == 0)
			m_xTowerGimPropFile.m_fBackRulingSpan = atof(skey);
		else if (stricmp(skey1, "DESIGNMAXSPAN") == 0 || stricmp(skey2, "�����󵵾�") == 0)
			m_xTowerGimPropFile.m_fMaxSpan = atof(skey);
		else if (stricmp(skey1, "ANGLERANGE") == 0 || stricmp(skey2, "���ת�Ƿ�Χ") == 0)
			m_xTowerGimPropFile.m_sAngleRange.Copy(skey);
		else if (stricmp(skey1, "DESIGNKV") == 0 || stricmp(skey2, "���Kvֵ") == 0)
			m_xTowerGimPropFile.m_fDesignKV = atof(skey);
		else if (stricmp(skey1, "RATEDNOMINALHEIGHT") == 0 || stricmp(skey2, "�������") == 0)
			m_xTowerGimPropFile.m_sRatedHeight.Copy(skey);
		else if (stricmp(skey1, "NOMINALHEIGHTRANGE") == 0 || stricmp(skey2, "���߷�Χ") == 0)
			m_xTowerGimPropFile.m_sHeightRange.Copy(skey);
		else if (stricmp(skey1, "TOWERWEIGHT") == 0 || stricmp(skey2, "����") == 0)
			m_xTowerGimPropFile.m_sTowerWeight.Copy(skey);
		else if (stricmp(skey1, "ROCKANGLEOFDESIGNPOWERFREQUENCY") == 0 || stricmp(skey2, "��ƹ�Ƶҡ�ڽǶ�") == 0)
			m_xTowerGimPropFile.m_fFrequencyRockAngle = atof(skey);
		else if (stricmp(skey1, "ROCKANGLEOFDESIGNLIGHTNING") == 0 || stricmp(skey2, "����׵�ҡ�ڽǶ�") == 0)
			m_xTowerGimPropFile.m_fLightningRockAngle = atof(skey);
		else if (stricmp(skey1, "ROCKANGLEOFDESIGNSWITCHING") == 0 || stricmp(skey2, "��Ʋ���ҡ�ڽǶ�") == 0)
			m_xTowerGimPropFile.m_fSwitchingRockAngle = atof(skey);
		else if (stricmp(skey1, "ROCKANGLEOFDESIGNLIVEWORKING") == 0 || stricmp(skey2, "��ƴ�����ҵҡ�ڽǶ�") == 0)
			m_xTowerGimPropFile.m_fWorkingRockAngle = atof(skey);
		else if (stricmp(skey1, "MANUFACTURER") == 0 || stricmp(skey2, "��������") == 0)
			m_xTowerGimPropFile.m_sManuFacturer.Copy(skey);
		else if (stricmp(skey1, "MATERIALCODE") == 0 || stricmp(skey2, "���ʱ���") == 0)
			m_xTowerGimPropFile.m_sMaterialCode.Copy(skey);
		else if (stricmp(skey1, "PROCESSMODELCODE") == 0 || stricmp(skey2, "װ��ģ�ͱ��") == 0)
			m_xTowerGimPropFile.m_sProModelCode.Copy(skey);
	}
	fclose(fp);
	return TRUE;
}
void CTowerGimInfo::WriteFamFile(const char* sFamFile)
{
	FILE *fp = NULL;
	if (m_ciCodingType == 1)
		fp = fopen(sFamFile, "wt");
	else
		fp = fopen(sFamFile, "wt,ccs=UTF-8");
	if (fp == NULL)
		return;
	CXhChar50 ss;
	if (m_ciCodingType == 1)
	{
		//��ѹ�ȼ�
		fprintf(fp, "VOLTAGE = ��ѹ�ȼ� = %s\n", (char*)m_xTowerGimPropFile.m_sVoltGrade);
		//����
		fprintf(fp, "TYPE = ���� = %s\n", (char*)m_xTowerGimPropFile.m_sType);
		//��������
		fprintf(fp, "TEXTURE = �������� = %s\n", (char*)m_xTowerGimPropFile.m_sTexture);
		//�̶���ʽ
		fprintf(fp, "FIXEDTYPE = �̶���ʽ = %s\n", (char*)m_xTowerGimPropFile.m_sFixedType);
		//��������
		fprintf(fp, "TOWERTYPE = �������� = %s\n", (char*)m_xTowerGimPropFile.m_sTaType);
		//��·��
		fprintf(fp, "CIRCUIT = ��·�� = %d\n", m_xTowerGimPropFile.m_nCircuit);
		//�����ͺ�
		fprintf(fp, "CONDUCTOR = �����ͺ� = %s\n", (char*)m_xTowerGimPropFile.m_sCWireSpec);
		//�����ͺ�
		fprintf(fp, "GROUNDWIRE = �����ͺ� = %s\n", (char*)m_xTowerGimPropFile.m_sEWireSpec);
		//��������
		ss.Printf("%f", m_xTowerGimPropFile.m_fWindSpeed);
		SimplifiedNumString(ss);
		fprintf(fp, "REFERENCEWINDSPEED = ��ƻ������� = %s\n", (char*)ss);
		//�������
		ss.Printf("%f", m_xTowerGimPropFile.m_fNiceThick);
		SimplifiedNumString(ss);
		fprintf(fp, "DESIGNICETHICKNESS = ��Ƹ������ = %s\n", (char*)ss);
		//ˮƽ����
		fprintf(fp, "DESIGNWINDSPAN = ���ˮƽ���� = %s\n", (char*)m_xTowerGimPropFile.m_sWindSpan);
		//��ֱ����
		fprintf(fp, "DESIGNWEIGHTSPAN = ��ƴ�ֱ���� = %s\n", (char*)m_xTowerGimPropFile.m_sWeightSpan);
		//ǰ�������
		ss.Printf("%f", m_xTowerGimPropFile.m_fFrontRulingSpan);
		SimplifiedNumString(ss);
		fprintf(fp, "DESIGNFRONTRULINGSPAN = ���ǰ������� = %s\n", (char*)ss);
		//��������
		ss.Printf("%f", m_xTowerGimPropFile.m_fBackRulingSpan);
		SimplifiedNumString(ss);
		fprintf(fp, "DESIGNBACKRULINGSPAN=��ƺ�������=%s\n", (char*)ss);
		//��󵵾�
		ss.Printf("%f", m_xTowerGimPropFile.m_fMaxSpan);
		SimplifiedNumString(ss);
		fprintf(fp, "DESIGNMAXSPAN = �����󵵾� =%s\n", (char*)ss);
		//ת�Ƿ�Χ
		fprintf(fp, "ANGLERANGE = ���ת�Ƿ�Χ =%s\n", (char*)m_xTowerGimPropFile.m_sAngleRange);
		//Kv
		ss.Printf("%f", m_xTowerGimPropFile.m_fDesignKV);
		SimplifiedNumString(ss);
		fprintf(fp, "DESIGNKV = ���Kvֵ = %s\n", (char*)ss);
		//�������
		fprintf(fp, "RATEDNOMINALHEIGHT = ������� = %s\n", (char*)m_xTowerGimPropFile.m_sRatedHeight);
		//���߷�Χ
		fprintf(fp, "NOMINALHEIGHTRANGE = ���߷�Χ = %s\n", (char*)m_xTowerGimPropFile.m_sHeightRange);
		//����
		fprintf(fp, "TOWERWEIGHT = ���� = %s\n", (char*)m_xTowerGimPropFile.m_sTowerWeight);
		//��ƹ�Ƶҡ�ڽǶ�
		ss.Printf("%f", m_xTowerGimPropFile.m_fFrequencyRockAngle);
		SimplifiedNumString(ss);
		fprintf(fp, "ROCKANGLEOFDESIGNPOWERFREQUENCY = ��ƹ�Ƶҡ�ڽǶ� = %s\n", (char*)ss);
		//����׵�ҡ�ڽǶ�
		ss.Printf("%f", m_xTowerGimPropFile.m_fLightningRockAngle);
		SimplifiedNumString(ss);
		fprintf(fp, "ROCKANGLEOFDESIGNLIGHTNING = ����׵�ҡ�ڽǶ� = %s\n", (char*)ss);
		//��Ʋ���ҡ�ڽǶ�
		ss.Printf("%f", m_xTowerGimPropFile.m_fSwitchingRockAngle);
		SimplifiedNumString(ss);
		fprintf(fp, "ROCKANGLEOFDESIGNSWITCHING = ��Ʋ���ҡ�ڽǶ� = %s\n", (char*)ss);
		//��ƴ�����ҵҡ�ڽǶ�
		ss.Printf("%f", m_xTowerGimPropFile.m_fWorkingRockAngle);
		SimplifiedNumString(ss);
		fprintf(fp, "ROCKANGLEOFDESIGNLIVEWORKING = ��ƴ�����ҵҡ�ڽǶ� = %s\n", (char*)ss);
		//��������
		fprintf(fp, "MANUFACTURER = �������� = %s\n", (char*)m_xTowerGimPropFile.m_sManuFacturer);
		//���ʱ���
		fprintf(fp, "MATERIALCODE = ���ʱ��� = %s\n", (char*)m_xTowerGimPropFile.m_sMaterialCode);
		//װ��ģ�ͱ��
		fprintf(fp, "PROCESSMODELCODE = װ��ģ�ͱ�� = %s\n", (char*)m_xTowerGimPropFile.m_sProModelCode);
	}
	else
	{
		CXhChar500 sValue;
		wchar_t sWValue[MAX_PATH];
		//��ѹ�ȼ�
		sValue.Printf("VOLTAGE = ��ѹ�ȼ� = %s", (char*)m_xTowerGimPropFile.m_sVoltGrade);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//����
		sValue.Printf("TYPE = ���� = %s", (char*)m_xTowerGimPropFile.m_sType);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//��������
		sValue.Printf("TEXTURE = �������� = %s", (char*)m_xTowerGimPropFile.m_sTexture);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//�̶���ʽ
		sValue.Printf("FIXEDTYPE = �̶���ʽ = %s", (char*)m_xTowerGimPropFile.m_sFixedType);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//��������
		sValue.Printf("TOWERTYPE = �������� = %s", (char*)m_xTowerGimPropFile.m_sTaType);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//��·��
		sValue.Printf("CIRCUIT = ��·�� = %d", m_xTowerGimPropFile.m_nCircuit);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//�����ͺ�
		sValue.Printf("CONDUCTOR = �����ͺ� = %s", (char*)m_xTowerGimPropFile.m_sCWireSpec);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//�����ͺ�
		sValue.Printf("GROUNDWIRE = �����ͺ� = %s", (char*)m_xTowerGimPropFile.m_sEWireSpec);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//��ƻ�������
		ss.Printf("%f", m_xTowerGimPropFile.m_fWindSpeed);
		SimplifiedNumString(ss);
		sValue.Printf("REFERENCEWINDSPEED = ��ƻ������� = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//��Ƹ������
		ss.Printf("%f", m_xTowerGimPropFile.m_fNiceThick);
		SimplifiedNumString(ss);
		sValue.Printf("DESIGNICETHICKNESS = ��Ƹ������ = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//���ˮƽ����
		sValue.Printf("DESIGNWINDSPAN = ���ˮƽ���� = %s", (char*)m_xTowerGimPropFile.m_sWindSpan);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//��ƴ�ֱ����
		sValue.Printf("DESIGNWEIGHTSPAN = ��ƴ�ֱ���� = %s", (char*)m_xTowerGimPropFile.m_sWeightSpan);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//���ǰ�������
		ss.Printf("%f", m_xTowerGimPropFile.m_fFrontRulingSpan);
		SimplifiedNumString(ss);
		sValue.Printf("DESIGNFRONTRULINGSPAN = ���ǰ������� = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//��ƺ�������
		ss.Printf("%f", m_xTowerGimPropFile.m_fBackRulingSpan);
		SimplifiedNumString(ss);
		sValue.Printf("DESIGNBACKRULINGSPAN = ��ƺ������� = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//�����󵵾�
		ss.Printf("%f", m_xTowerGimPropFile.m_fMaxSpan);
		SimplifiedNumString(ss);
		sValue.Printf("DESIGNMAXSPAN = �����󵵾� = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//���ת�Ƿ�Χ
		sValue.Printf("ANGLERANGE = ���ת�Ƿ�Χ = %s", (char*)m_xTowerGimPropFile.m_sAngleRange);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//Kv
		ss.Printf("%f", m_xTowerGimPropFile.m_fDesignKV);
		SimplifiedNumString(ss);
		sValue.Printf("DESIGNKV = ���Kvֵ = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//�������
		sValue.Printf("RATEDNOMINALHEIGHT = ������� = %s", (char*)m_xTowerGimPropFile.m_sRatedHeight);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//���߷�Χ
		sValue.Printf("NOMINALHEIGHTRANGE = ���߷�Χ = %s", (char*)m_xTowerGimPropFile.m_sHeightRange);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//����
		sValue.Printf("TOWERWEIGHT = ���� = %s", (char*)m_xTowerGimPropFile.m_sTowerWeight);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//��ƹ�Ƶҡ�ڽǶ�
		ss.Printf("%f", m_xTowerGimPropFile.m_fFrequencyRockAngle);
		SimplifiedNumString(ss);
		sValue.Printf("ROCKANGLEOFDESIGNPOWERFREQUENCY = ��ƹ�Ƶҡ�ڽǶ� = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//����׵�ҡ�ڽǶ�
		ss.Printf("%f", m_xTowerGimPropFile.m_fLightningRockAngle);
		SimplifiedNumString(ss);
		sValue.Printf("ROCKANGLEOFDESIGNLIGHTNING = ����׵�ҡ�ڽǶ� = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//��Ʋ���ҡ�ڽǶ�
		ss.Printf("%f", m_xTowerGimPropFile.m_fSwitchingRockAngle);
		SimplifiedNumString(ss);
		sValue.Printf("ROCKANGLEOFDESIGNSWITCHING = ��Ʋ���ҡ�ڽǶ� = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//��ƴ�����ҵҡ�ڽǶ�
		ss.Printf("%f", m_xTowerGimPropFile.m_fWorkingRockAngle);
		SimplifiedNumString(ss);
		sValue.Printf("ROCKANGLEOFDESIGNLIVEWORKING = ��ƴ�����ҵҡ�ڽǶ� = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//��������
		sValue.Printf("MANUFACTURER = �������� = %s", (char*)m_xTowerGimPropFile.m_sManuFacturer);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//���ʱ���
		sValue.Printf("MATERIALCODE = ���ʱ��� = %s", (char*)m_xTowerGimPropFile.m_sMaterialCode);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//װ��ģ�ͱ��
		sValue.Printf("PROCESSMODELCODE = װ��ģ�ͱ�� = %s", (char*)m_xTowerGimPropFile.m_sProModelCode);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
	}
	fclose(fp);
}
const DWORD HASHTABLESIZE = 500;
IMPLEMENT_PROP_FUNC(CTowerGimInfo);
void CTowerGimInfo::InitPropHashtable()
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(50);
	//������Ϣ
	AddPropItem("BasicInfo", PROPLIST_ITEM(id++, "GIMͷ����Ϣ", "GIMͷ����Ϣ"));
	AddPropItem("Designer", PROPLIST_ITEM(id++, "�����", "�����"));
	AddPropItem("Unit", PROPLIST_ITEM(id++, "��֯��λ", "��֯��λ"));
	AddPropItem("MajorVer", PROPLIST_ITEM(id++, "���汾��", "���汾��"));
	AddPropItem("MinorVer", PROPLIST_ITEM(id++, "�ΰ汾��", "�ΰ汾��"));
	//�����Ϣ
	AddPropItem("TowerInfo", PROPLIST_ITEM(id++, "������������", "������������"));
	AddPropItem("VoltGrade", PROPLIST_ITEM(id++, "��ѹ�ȼ�", "��ѹ�ȼ�"));
	AddPropItem("Type", PROPLIST_ITEM(id++, "����", "����"));
	AddPropItem("Texture", PROPLIST_ITEM(id++, "��������", "��������", "�Ǹ���|�ֹ���|�ֹܸ�|ˮ���|����"));
	AddPropItem("FixedType", PROPLIST_ITEM(id++, "�̶���ʽ", "�̶���ʽ", "����|����"));
	AddPropItem("TaType", PROPLIST_ITEM(id++, "��������", "��������", "������|������|��λ��"));
	AddPropItem("Circuit", PROPLIST_ITEM(id++, "��·��", "��·��"));
	AddPropItem("CWireSpec", PROPLIST_ITEM(id++, "�����ͺ�", "�����ͺ�"));
	AddPropItem("EWireSpec", PROPLIST_ITEM(id++, "�����ͺ�", "�����ͺ�"));
	AddPropItem("WindSpeed", PROPLIST_ITEM(id++, "��ƻ�������", "��ƻ�������"));
	AddPropItem("NiceThick", PROPLIST_ITEM(id++, "��Ƹ������", "��Ƹ������"));
	AddPropItem("WindSpan", PROPLIST_ITEM(id++, "���ˮƽ����", "���ˮƽ����"));
	AddPropItem("WeightSpan", PROPLIST_ITEM(id++, "��ƴ�ֱ����", "��ƴ�ֱ����"));
	AddPropItem("MaxSpan", PROPLIST_ITEM(id++, "�����󵵾�", "��󵵾�"));
	AddPropItem("FrontRulingSpan", PROPLIST_ITEM(id++, "���ǰ�������", "ǰ�������"));
	AddPropItem("BackRulingSpan", PROPLIST_ITEM(id++, "��ƺ�������", "��������"));
	AddPropItem("AngleRange", PROPLIST_ITEM(id++, "���ת�Ƿ�Χ", "ת�Ƿ�Χ"));
	AddPropItem("DesignKV", PROPLIST_ITEM(id++, "���Kvֵ", "���Kvֵ"));
	AddPropItem("RatedHeight", PROPLIST_ITEM(id++, "�������", "�������"));
	AddPropItem("HeightRange", PROPLIST_ITEM(id++, "���߷�Χ", "���߷�Χ"));
	AddPropItem("TowerWeight", PROPLIST_ITEM(id++, "����", "����"));
	AddPropItem("FrequencyRockAngle", PROPLIST_ITEM(id++, "��ƹ�Ƶҡ�ڽǶ�", "��ƹ�Ƶҡ�ڽǶ�"));
	AddPropItem("LightningRockAngle", PROPLIST_ITEM(id++, "����׵�ҡ�ڽǶ�", "����׵�ҡ�ڽǶ�"));
	AddPropItem("SwitchingRockAngle", PROPLIST_ITEM(id++, "��Ʋ���ҡ�ڽǶ�", "��Ʋ���ҡ�ڽǶ�"));
	AddPropItem("WorkingRockAngle", PROPLIST_ITEM(id++, "��ƴ�����ҵ�Ƕ�", "��ƴ�����ҵҡ�ڽǶ�"));
	AddPropItem("ManuFacturer", PROPLIST_ITEM(id++, "��������", "��������"));
	AddPropItem("MaterialCode", PROPLIST_ITEM(id++, "���ʱ���", "���ʱ���"));
}
int CTowerGimInfo::GetPropValueStr(long id, char* valueStr, UINT nMaxStrBufLen/*=100*/)	//��������ID�õ�����ֵ�ַ���
{
	char sText[101] = "";
	if (GetPropID("Designer") == id)
		strncpy(sText, m_xCurrGimFileHeader.m_sDesigner, 100);
	else if (GetPropID("Unit") == id)
		strncpy(sText, m_xCurrGimFileHeader.m_sUnit, 100);
	else if (GetPropID("MajorVer") == id)
		strcpy(sText, m_xCurrGimFileHeader.m_sMajorVersion);
	else if (GetPropID("MinorVer") == id)
		strcpy(sText, m_xCurrGimFileHeader.m_sMinorVersion);
	else if (GetPropID("VoltGrade") == id)
		strcpy(sText, m_xTowerGimPropFile.m_sVoltGrade);
	else if (GetPropID("Texture") == id)
		strcpy(sText, m_xTowerGimPropFile.m_sTexture);
	else if (GetPropID("FixedType") == id)
		strcpy(sText, m_xTowerGimPropFile.m_sFixedType);
	else if (GetPropID("Type") == id)
		strcpy(sText, m_xTowerGimPropFile.m_sType);
	else if (GetPropID("TaType") == id)
		strcpy(sText, m_xTowerGimPropFile.m_sTaType);
	else if (GetPropID("Circuit") == id)
		sprintf(sText, "%d", m_xTowerGimPropFile.m_nCircuit);
	else if (GetPropID("CWireSpec") == id)
		strcpy(sText, m_xTowerGimPropFile.m_sCWireSpec);
	else if (GetPropID("EWireSpec") == id)
		strcpy(sText, m_xTowerGimPropFile.m_sEWireSpec);
	else if (GetPropID("WindSpeed") == id)
	{
		sprintf(sText, "%f", m_xTowerGimPropFile.m_fWindSpeed);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("NiceThick") == id)
	{
		sprintf(sText, "%f", m_xTowerGimPropFile.m_fNiceThick);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("WindSpan") == id)
		strcpy(sText, m_xTowerGimPropFile.m_sWindSpan);
	else if (GetPropID("WeightSpan") == id)
		strcpy(sText, m_xTowerGimPropFile.m_sWeightSpan);
	else if (GetPropID("FrontRulingSpan") == id)
	{
		sprintf(sText, "%f", m_xTowerGimPropFile.m_fFrontRulingSpan);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("BackRulingSpan") == id)
	{
		sprintf(sText, "%f", m_xTowerGimPropFile.m_fBackRulingSpan);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("MaxSpan") == id)
	{
		sprintf(sText, "%f", m_xTowerGimPropFile.m_fMaxSpan);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("AngleRange") == id)
		strcpy(sText, m_xTowerGimPropFile.m_sAngleRange);
	else if (GetPropID("DesignKV") == id)
	{
		sprintf(sText, "%f", m_xTowerGimPropFile.m_fDesignKV);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("RatedHeight") == id)
		strcpy(sText, m_xTowerGimPropFile.m_sRatedHeight);
	else if (GetPropID("HeightRange") == id)
		strcpy(sText, m_xTowerGimPropFile.m_sHeightRange);
	else if (GetPropID("TowerWeight") == id)
		strcpy(sText, m_xTowerGimPropFile.m_sTowerWeight);
	else if (GetPropID("FrequencyRockAngle") == id)
	{
		sprintf(sText, "%f", m_xTowerGimPropFile.m_fFrequencyRockAngle);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("LightningRockAngle") == id)
	{
		sprintf(sText, "%f", m_xTowerGimPropFile.m_fLightningRockAngle);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("SwitchingRockAngle") == id)
	{
		sprintf(sText, "%f", m_xTowerGimPropFile.m_fSwitchingRockAngle);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("WorkingRockAngle") == id)
	{
		sprintf(sText, "%f", m_xTowerGimPropFile.m_fWorkingRockAngle);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("ManuFacturer") == id)
		strcpy(sText, m_xTowerGimPropFile.m_sManuFacturer);
	else if (GetPropID("MaterialCode") == id)
		strcpy(sText, m_xTowerGimPropFile.m_sMaterialCode);
	if (valueStr)
		StrCopy(valueStr, sText, nMaxStrBufLen);
	return strlen(sText);
}
