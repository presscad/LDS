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
//本文件用于实现与电气相关的功能函数，如挂孔提取、金具、绝缘子预览等
//////////////////////////////////////////////////////////////////////////////
CExterDataModel gxExterModel;
CUniWireModel gxWirePointModel;
CTowerGimInfo gxTowerGimInfo;
static const double EPS_001=0.001;
int XHWIREPOINT::Compare(const ICompareClass *pCompareObj)const
{
	XHWIREPOINT* pWirePoint=(XHWIREPOINT*)pCompareObj;
	//先上后下
	if(this->xBasicPoint.z<pWirePoint->xBasicPoint.z-EPS_001)
		return -1;
	else if(this->xBasicPoint.z>pWirePoint->xBasicPoint.z+EPS_001)
		return  1;
	//先外后内
	else if(fabs(xBasicPoint.x)>fabs(pWirePoint->xBasicPoint.x)+EPS_001)
		return -1;
	else if(fabs(xBasicPoint.x)<fabs(pWirePoint->xBasicPoint.x)-EPS_001)
		return  1;
	//先左后右
	else if(xBasicPoint.x<0&&pWirePoint->xBasicPoint.x>0)
		return -1;
	else if(xBasicPoint.x>0&&pWirePoint->xBasicPoint.x<0)
		return  1;
	//先前后后
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
			//CXhChar50 szWireName="导线";
			//if(pNode->wireplace.ciWireType=='J')
			//	szWireName.Copy("跳线");
			//else if(pNode->wireplace.ciWireType=='E')
			//	szWireName.Copy("地线");
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
	//自动查找挂孔对形成挂点对象CUniWirePoint
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
					continue;	//挂孔大小必须一致
				if(fabs(pWirePoint->vStdBasicAxisVec*wirepoint.vStdBasicAxisVec)<0.99)	//cos(8°)=0.99
					continue;	//
				GEPOINT vxDistVec=wirepoint.xBasicPoint-pWirePoint->xBasicPoint;
				double distance=vxDistVec*pWirePoint->vStdBasicAxisVec;
				GEPOINT vxHoriPrj=vxDistVec-pWirePoint->vStdBasicAxisVec*distance;
				if(vxHoriPrj.mod2()>1)
					continue;	//两挂孔投影点距离超过1mm误差允许范围
				if((pWirePoint->blWireAngle&&distance<25)||fabs(distance)<25||fabs(distance)>300)
					continue;	//25为预设金具最小宽度，300为预设金具最大宽度
				if(pWirePoint->hlCoupl.hResidePart==0)
				{
					pWirePoint->hlCoupl.hBolt=pHole->pBolt->handle;
					pWirePoint->hlCoupl.hResidePart=pSmartPart->handle;
					if(distance<0)	//反向，始终令vStdBasicAxisVec为指向另一孔轴向
						pWirePoint->vStdBasicAxisVec*=-1;
					pWirePoint->dfSpaceWidth=fabs(distance);
					break;
				}
				else if(fabs(distance)<pWirePoint->dfSpaceWidth)
				{	//当前挂孔与之前挂孔距离更近，更适合结合成一对挂孔
					//将之前的配对孔另作为下一挂点的基准孔
					XHWIREPOINT tmp;
					tmp.sfHoleD=pWirePoint->sfHoleD;
					tmp.blWireAngle=pWirePoint->blWireAngle;
					tmp.holes[0]=pWirePoint->holes[1];
					tmp.xBasicPoint=pWirePoint->xBasicPoint+pWirePoint->vStdBasicAxisVec*pWirePoint->dfSpaceWidth;
					tmp.vStdBasicAxisVec=-pWirePoint->vStdBasicAxisVec;
					listWirePoints.Append(tmp);
					//将当前挂孔添加为当前挂点的配对孔
					pWirePoint->hlCoupl.hBolt=pHole->pBolt->handle;
					pWirePoint->hlCoupl.hResidePart=pSmartPart->handle;
					if(distance<0)	//反向，始终令vStdBasicAxisVec为指向另一孔轴向
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
	//筛选挂孔
	ARRAY_LIST<XHWIREPOINT> xarrWirePoints(0,listWirePoints.Count);
	for(pWirePoint=listWirePoints.EnumObjectFirst();pWirePoint;pWirePoint=listWirePoints.EnumObjectNext())
	{
		if(pWirePoint->blWireAngle&&pWirePoint->hlCoupl.hResidePart==0)
			continue;	//挂线角钢上的挂孔是成对的，不可能存在单孔
		else if(pWirePoint->sfHoleD<16)	//挂孔孔径不应小于16，否则视为避雷接地分流孔或其余挂牌孔等
			continue;
		xarrWirePoints.append(*pWirePoint);
	}	
	//对挂点排序，然后根据位置进行分组并添加到挂点模型中
	CHeapSort<XHWIREPOINT>::HeapSortClassic(xarrWirePoints.m_pData,xarrWirePoints.GetSize());
	CXhWirePoint *pWirePointObj=NULL,*pPrevWirePointObj=NULL,*pPrevWirePointObjGroup=NULL;
	int nWireGroupCount=0;
	pWireModel->m_hashWirePoints.Empty();
	for(int i=0;i<xarrWirePoints.GetSize();i++)
	{	//智能添加实际导地线序列CXhWirePoint
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
	//自动对挂点对象分组形成挂点分组CWireConnPoint
	//在挂点分组内组装成对接点对象CWireConnPoint
	//初始化架线方案，默认为500kv交流双回路
	//if(nWireGroupCount<=2)	//直流
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
	//添加线缆装配对接点
	CWireConnPoint* pWireConnPoint=model.hashConnPoints.Add(1);
	pWireConnPoint->AppendWirePoint(1);	//添加导线对接点
	//关联线缆对接点与线缆相序分组
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
		logerr.Log("%s打开失败!", sFamFile);
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
		if (strstr(skey1, "VOLTAGE") || stricmp(skey2, "电压等级") == 0)
			m_xTowerGimPropFile.m_sVoltGrade.Copy(skey);
		else if (stricmp(skey1, "TYPE") == 0 || stricmp(skey2, "塔型") == 0)
			m_xTowerGimPropFile.m_sType.Copy(skey);
		else if (stricmp(skey1, "TEXTURE") == 0 || stricmp(skey2, "杆塔材质") == 0)
			m_xTowerGimPropFile.m_sTexture.Copy(skey);
		else if (stricmp(skey1, "FIXEDTYPE") == 0 || stricmp(skey2, "固定方式") == 0)
			m_xTowerGimPropFile.m_sFixedType.Copy(skey);
		else if (stricmp(skey1, "TOWERTYPE") == 0 || stricmp(skey2, "杆塔类型") == 0)
			m_xTowerGimPropFile.m_sTaType.Copy(skey);
		else if (stricmp(skey1, "CIRCUIT") == 0 || stricmp(skey2, "回路数") == 0)
			m_xTowerGimPropFile.m_nCircuit = atoi(skey);
		else if (stricmp(skey1, "CONDUCTOR") == 0 || stricmp(skey2, "导线型号") == 0)
			m_xTowerGimPropFile.m_sCWireSpec.Copy(skey);
		else if (stricmp(skey1, "GROUNDWIRE") == 0 || stricmp(skey2, "地线型号") == 0)
			m_xTowerGimPropFile.m_sEWireSpec.Copy(skey);
		else if (stricmp(skey1, "REFERENCEWINDSPEED") == 0 || stricmp(skey2, "设计基本风速") == 0)
			m_xTowerGimPropFile.m_fWindSpeed = atof(skey);
		else if (stricmp(skey1, "DESIGNICETHICKNESS") == 0 || stricmp(skey2, "设计覆冰厚度") == 0)
			m_xTowerGimPropFile.m_fNiceThick = atof(skey);
		else if (stricmp(skey1, "DESIGNWINDSPAN") == 0 || stricmp(skey2, "设计水平档距") == 0)
			m_xTowerGimPropFile.m_sWindSpan.Copy(skey);
		else if (stricmp(skey1, "DESIGNWEIGHTSPAN") == 0 || stricmp(skey2, "设计垂直档距") == 0)
			m_xTowerGimPropFile.m_sWeightSpan.Copy(skey);
		else if (stricmp(skey1, "DESIGNFRONTRULINGSPAN") == 0 || stricmp(skey2, "设计前侧代表档距") == 0)
			m_xTowerGimPropFile.m_fFrontRulingSpan = atof(skey);
		else if (stricmp(skey1, "DESIGNBACKRULINGSPAN") == 0 || stricmp(skey2, "设计后侧代表档距") == 0)
			m_xTowerGimPropFile.m_fBackRulingSpan = atof(skey);
		else if (stricmp(skey1, "DESIGNMAXSPAN") == 0 || stricmp(skey2, "设计最大档距") == 0)
			m_xTowerGimPropFile.m_fMaxSpan = atof(skey);
		else if (stricmp(skey1, "ANGLERANGE") == 0 || stricmp(skey2, "设计转角范围") == 0)
			m_xTowerGimPropFile.m_sAngleRange.Copy(skey);
		else if (stricmp(skey1, "DESIGNKV") == 0 || stricmp(skey2, "设计Kv值") == 0)
			m_xTowerGimPropFile.m_fDesignKV = atof(skey);
		else if (stricmp(skey1, "RATEDNOMINALHEIGHT") == 0 || stricmp(skey2, "计算呼高") == 0)
			m_xTowerGimPropFile.m_sRatedHeight.Copy(skey);
		else if (stricmp(skey1, "NOMINALHEIGHTRANGE") == 0 || stricmp(skey2, "呼高范围") == 0)
			m_xTowerGimPropFile.m_sHeightRange.Copy(skey);
		else if (stricmp(skey1, "TOWERWEIGHT") == 0 || stricmp(skey2, "塔重") == 0)
			m_xTowerGimPropFile.m_sTowerWeight.Copy(skey);
		else if (stricmp(skey1, "ROCKANGLEOFDESIGNPOWERFREQUENCY") == 0 || stricmp(skey2, "设计工频摇摆角度") == 0)
			m_xTowerGimPropFile.m_fFrequencyRockAngle = atof(skey);
		else if (stricmp(skey1, "ROCKANGLEOFDESIGNLIGHTNING") == 0 || stricmp(skey2, "设计雷电摇摆角度") == 0)
			m_xTowerGimPropFile.m_fLightningRockAngle = atof(skey);
		else if (stricmp(skey1, "ROCKANGLEOFDESIGNSWITCHING") == 0 || stricmp(skey2, "设计操作摇摆角度") == 0)
			m_xTowerGimPropFile.m_fSwitchingRockAngle = atof(skey);
		else if (stricmp(skey1, "ROCKANGLEOFDESIGNLIVEWORKING") == 0 || stricmp(skey2, "设计带电作业摇摆角度") == 0)
			m_xTowerGimPropFile.m_fWorkingRockAngle = atof(skey);
		else if (stricmp(skey1, "MANUFACTURER") == 0 || stricmp(skey2, "生产厂家") == 0)
			m_xTowerGimPropFile.m_sManuFacturer.Copy(skey);
		else if (stricmp(skey1, "MATERIALCODE") == 0 || stricmp(skey2, "物资编码") == 0)
			m_xTowerGimPropFile.m_sMaterialCode.Copy(skey);
		else if (stricmp(skey1, "PROCESSMODELCODE") == 0 || stricmp(skey2, "装配模型编号") == 0)
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
		//电压等级
		fprintf(fp, "VOLTAGE = 电压等级 = %s\n", (char*)m_xTowerGimPropFile.m_sVoltGrade);
		//塔型
		fprintf(fp, "TYPE = 塔型 = %s\n", (char*)m_xTowerGimPropFile.m_sType);
		//杆塔材质
		fprintf(fp, "TEXTURE = 杆塔材质 = %s\n", (char*)m_xTowerGimPropFile.m_sTexture);
		//固定方式
		fprintf(fp, "FIXEDTYPE = 固定方式 = %s\n", (char*)m_xTowerGimPropFile.m_sFixedType);
		//杆塔类型
		fprintf(fp, "TOWERTYPE = 杆塔类型 = %s\n", (char*)m_xTowerGimPropFile.m_sTaType);
		//回路数
		fprintf(fp, "CIRCUIT = 回路数 = %d\n", m_xTowerGimPropFile.m_nCircuit);
		//导线型号
		fprintf(fp, "CONDUCTOR = 导线型号 = %s\n", (char*)m_xTowerGimPropFile.m_sCWireSpec);
		//地线型号
		fprintf(fp, "GROUNDWIRE = 地线型号 = %s\n", (char*)m_xTowerGimPropFile.m_sEWireSpec);
		//基本风速
		ss.Printf("%f", m_xTowerGimPropFile.m_fWindSpeed);
		SimplifiedNumString(ss);
		fprintf(fp, "REFERENCEWINDSPEED = 设计基本风速 = %s\n", (char*)ss);
		//覆冰厚度
		ss.Printf("%f", m_xTowerGimPropFile.m_fNiceThick);
		SimplifiedNumString(ss);
		fprintf(fp, "DESIGNICETHICKNESS = 设计覆冰厚度 = %s\n", (char*)ss);
		//水平档距
		fprintf(fp, "DESIGNWINDSPAN = 设计水平档距 = %s\n", (char*)m_xTowerGimPropFile.m_sWindSpan);
		//垂直档距
		fprintf(fp, "DESIGNWEIGHTSPAN = 设计垂直档距 = %s\n", (char*)m_xTowerGimPropFile.m_sWeightSpan);
		//前侧代表档距
		ss.Printf("%f", m_xTowerGimPropFile.m_fFrontRulingSpan);
		SimplifiedNumString(ss);
		fprintf(fp, "DESIGNFRONTRULINGSPAN = 设计前侧代表档距 = %s\n", (char*)ss);
		//后侧代表档距
		ss.Printf("%f", m_xTowerGimPropFile.m_fBackRulingSpan);
		SimplifiedNumString(ss);
		fprintf(fp, "DESIGNBACKRULINGSPAN=设计后侧代表档距=%s\n", (char*)ss);
		//最大档距
		ss.Printf("%f", m_xTowerGimPropFile.m_fMaxSpan);
		SimplifiedNumString(ss);
		fprintf(fp, "DESIGNMAXSPAN = 设计最大档距 =%s\n", (char*)ss);
		//转角范围
		fprintf(fp, "ANGLERANGE = 设计转角范围 =%s\n", (char*)m_xTowerGimPropFile.m_sAngleRange);
		//Kv
		ss.Printf("%f", m_xTowerGimPropFile.m_fDesignKV);
		SimplifiedNumString(ss);
		fprintf(fp, "DESIGNKV = 设计Kv值 = %s\n", (char*)ss);
		//计算呼高
		fprintf(fp, "RATEDNOMINALHEIGHT = 计算呼高 = %s\n", (char*)m_xTowerGimPropFile.m_sRatedHeight);
		//呼高范围
		fprintf(fp, "NOMINALHEIGHTRANGE = 呼高范围 = %s\n", (char*)m_xTowerGimPropFile.m_sHeightRange);
		//塔重
		fprintf(fp, "TOWERWEIGHT = 塔重 = %s\n", (char*)m_xTowerGimPropFile.m_sTowerWeight);
		//设计工频摇摆角度
		ss.Printf("%f", m_xTowerGimPropFile.m_fFrequencyRockAngle);
		SimplifiedNumString(ss);
		fprintf(fp, "ROCKANGLEOFDESIGNPOWERFREQUENCY = 设计工频摇摆角度 = %s\n", (char*)ss);
		//设计雷电摇摆角度
		ss.Printf("%f", m_xTowerGimPropFile.m_fLightningRockAngle);
		SimplifiedNumString(ss);
		fprintf(fp, "ROCKANGLEOFDESIGNLIGHTNING = 设计雷电摇摆角度 = %s\n", (char*)ss);
		//设计操作摇摆角度
		ss.Printf("%f", m_xTowerGimPropFile.m_fSwitchingRockAngle);
		SimplifiedNumString(ss);
		fprintf(fp, "ROCKANGLEOFDESIGNSWITCHING = 设计操作摇摆角度 = %s\n", (char*)ss);
		//设计带电作业摇摆角度
		ss.Printf("%f", m_xTowerGimPropFile.m_fWorkingRockAngle);
		SimplifiedNumString(ss);
		fprintf(fp, "ROCKANGLEOFDESIGNLIVEWORKING = 设计带电作业摇摆角度 = %s\n", (char*)ss);
		//生产厂家
		fprintf(fp, "MANUFACTURER = 生产厂家 = %s\n", (char*)m_xTowerGimPropFile.m_sManuFacturer);
		//物资编码
		fprintf(fp, "MATERIALCODE = 物资编码 = %s\n", (char*)m_xTowerGimPropFile.m_sMaterialCode);
		//装配模型编号
		fprintf(fp, "PROCESSMODELCODE = 装配模型编号 = %s\n", (char*)m_xTowerGimPropFile.m_sProModelCode);
	}
	else
	{
		CXhChar500 sValue;
		wchar_t sWValue[MAX_PATH];
		//电压等级
		sValue.Printf("VOLTAGE = 电压等级 = %s", (char*)m_xTowerGimPropFile.m_sVoltGrade);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//塔型
		sValue.Printf("TYPE = 塔型 = %s", (char*)m_xTowerGimPropFile.m_sType);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//杆塔材质
		sValue.Printf("TEXTURE = 杆塔材质 = %s", (char*)m_xTowerGimPropFile.m_sTexture);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//固定方式
		sValue.Printf("FIXEDTYPE = 固定方式 = %s", (char*)m_xTowerGimPropFile.m_sFixedType);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//杆塔类型
		sValue.Printf("TOWERTYPE = 杆塔类型 = %s", (char*)m_xTowerGimPropFile.m_sTaType);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//回路数
		sValue.Printf("CIRCUIT = 回路数 = %d", m_xTowerGimPropFile.m_nCircuit);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//导线型号
		sValue.Printf("CONDUCTOR = 导线型号 = %s", (char*)m_xTowerGimPropFile.m_sCWireSpec);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//地线型号
		sValue.Printf("GROUNDWIRE = 地线型号 = %s", (char*)m_xTowerGimPropFile.m_sEWireSpec);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//设计基本风速
		ss.Printf("%f", m_xTowerGimPropFile.m_fWindSpeed);
		SimplifiedNumString(ss);
		sValue.Printf("REFERENCEWINDSPEED = 设计基本风速 = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//设计覆冰厚度
		ss.Printf("%f", m_xTowerGimPropFile.m_fNiceThick);
		SimplifiedNumString(ss);
		sValue.Printf("DESIGNICETHICKNESS = 设计覆冰厚度 = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//设计水平档距
		sValue.Printf("DESIGNWINDSPAN = 设计水平档距 = %s", (char*)m_xTowerGimPropFile.m_sWindSpan);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//设计垂直档距
		sValue.Printf("DESIGNWEIGHTSPAN = 设计垂直档距 = %s", (char*)m_xTowerGimPropFile.m_sWeightSpan);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//设计前侧代表档距
		ss.Printf("%f", m_xTowerGimPropFile.m_fFrontRulingSpan);
		SimplifiedNumString(ss);
		sValue.Printf("DESIGNFRONTRULINGSPAN = 设计前侧代表档距 = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//设计后侧代表档距
		ss.Printf("%f", m_xTowerGimPropFile.m_fBackRulingSpan);
		SimplifiedNumString(ss);
		sValue.Printf("DESIGNBACKRULINGSPAN = 设计后侧代表档距 = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//设计最大档距
		ss.Printf("%f", m_xTowerGimPropFile.m_fMaxSpan);
		SimplifiedNumString(ss);
		sValue.Printf("DESIGNMAXSPAN = 设计最大档距 = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//设计转角范围
		sValue.Printf("ANGLERANGE = 设计转角范围 = %s", (char*)m_xTowerGimPropFile.m_sAngleRange);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//Kv
		ss.Printf("%f", m_xTowerGimPropFile.m_fDesignKV);
		SimplifiedNumString(ss);
		sValue.Printf("DESIGNKV = 设计Kv值 = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//计算呼高
		sValue.Printf("RATEDNOMINALHEIGHT = 计算呼高 = %s", (char*)m_xTowerGimPropFile.m_sRatedHeight);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//呼高范围
		sValue.Printf("NOMINALHEIGHTRANGE = 呼高范围 = %s", (char*)m_xTowerGimPropFile.m_sHeightRange);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//塔重
		sValue.Printf("TOWERWEIGHT = 塔重 = %s", (char*)m_xTowerGimPropFile.m_sTowerWeight);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//设计工频摇摆角度
		ss.Printf("%f", m_xTowerGimPropFile.m_fFrequencyRockAngle);
		SimplifiedNumString(ss);
		sValue.Printf("ROCKANGLEOFDESIGNPOWERFREQUENCY = 设计工频摇摆角度 = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//设计雷电摇摆角度
		ss.Printf("%f", m_xTowerGimPropFile.m_fLightningRockAngle);
		SimplifiedNumString(ss);
		sValue.Printf("ROCKANGLEOFDESIGNLIGHTNING = 设计雷电摇摆角度 = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//设计操作摇摆角度
		ss.Printf("%f", m_xTowerGimPropFile.m_fSwitchingRockAngle);
		SimplifiedNumString(ss);
		sValue.Printf("ROCKANGLEOFDESIGNSWITCHING = 设计操作摇摆角度 = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//设计带电作业摇摆角度
		ss.Printf("%f", m_xTowerGimPropFile.m_fWorkingRockAngle);
		SimplifiedNumString(ss);
		sValue.Printf("ROCKANGLEOFDESIGNLIVEWORKING = 设计带电作业摇摆角度 = %s", (char*)ss);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//生产厂家
		sValue.Printf("MANUFACTURER = 生产厂家 = %s", (char*)m_xTowerGimPropFile.m_sManuFacturer);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//物资编码
		sValue.Printf("MATERIALCODE = 物资编码 = %s", (char*)m_xTowerGimPropFile.m_sMaterialCode);
		ANSIToUnicode(sValue, sWValue);
		fwprintf(fp, L"%s\n", sWValue);
		//装配模型编号
		sValue.Printf("PROCESSMODELCODE = 装配模型编号 = %s", (char*)m_xTowerGimPropFile.m_sProModelCode);
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
	//基本信息
	AddPropItem("BasicInfo", PROPLIST_ITEM(id++, "GIM头部信息", "GIM头部信息"));
	AddPropItem("Designer", PROPLIST_ITEM(id++, "设计者", "设计者"));
	AddPropItem("Unit", PROPLIST_ITEM(id++, "组织单位", "组织单位"));
	AddPropItem("MajorVer", PROPLIST_ITEM(id++, "主版本号", "主版本号"));
	AddPropItem("MinorVer", PROPLIST_ITEM(id++, "次版本号", "次版本号"));
	//设计信息
	AddPropItem("TowerInfo", PROPLIST_ITEM(id++, "杆塔公共属性", "杆塔公共属性"));
	AddPropItem("VoltGrade", PROPLIST_ITEM(id++, "电压等级", "电压等级"));
	AddPropItem("Type", PROPLIST_ITEM(id++, "塔型", "塔型"));
	AddPropItem("Texture", PROPLIST_ITEM(id++, "杆塔材质", "杆塔材质", "角钢塔|钢管塔|钢管杆|水泥杆|铁杆"));
	AddPropItem("FixedType", PROPLIST_ITEM(id++, "固定方式", "固定方式", "自立|拉线"));
	AddPropItem("TaType", PROPLIST_ITEM(id++, "杆塔类型", "杆塔类型", "悬垂塔|耐张塔|换位塔"));
	AddPropItem("Circuit", PROPLIST_ITEM(id++, "回路数", "回路数"));
	AddPropItem("CWireSpec", PROPLIST_ITEM(id++, "导线型号", "导线型号"));
	AddPropItem("EWireSpec", PROPLIST_ITEM(id++, "地线型号", "地线型号"));
	AddPropItem("WindSpeed", PROPLIST_ITEM(id++, "设计基本风速", "设计基本风速"));
	AddPropItem("NiceThick", PROPLIST_ITEM(id++, "设计覆冰厚度", "设计覆冰厚度"));
	AddPropItem("WindSpan", PROPLIST_ITEM(id++, "设计水平档距", "设计水平档距"));
	AddPropItem("WeightSpan", PROPLIST_ITEM(id++, "设计垂直档距", "设计垂直档距"));
	AddPropItem("MaxSpan", PROPLIST_ITEM(id++, "设计最大档距", "最大档距"));
	AddPropItem("FrontRulingSpan", PROPLIST_ITEM(id++, "设计前侧代表档距", "前侧代表档距"));
	AddPropItem("BackRulingSpan", PROPLIST_ITEM(id++, "设计后侧代表档距", "后侧代表档距"));
	AddPropItem("AngleRange", PROPLIST_ITEM(id++, "设计转角范围", "转角范围"));
	AddPropItem("DesignKV", PROPLIST_ITEM(id++, "设计Kv值", "设计Kv值"));
	AddPropItem("RatedHeight", PROPLIST_ITEM(id++, "计算呼高", "计算呼高"));
	AddPropItem("HeightRange", PROPLIST_ITEM(id++, "呼高范围", "呼高范围"));
	AddPropItem("TowerWeight", PROPLIST_ITEM(id++, "塔重", "塔重"));
	AddPropItem("FrequencyRockAngle", PROPLIST_ITEM(id++, "设计工频摇摆角度", "设计工频摇摆角度"));
	AddPropItem("LightningRockAngle", PROPLIST_ITEM(id++, "设计雷电摇摆角度", "设计雷电摇摆角度"));
	AddPropItem("SwitchingRockAngle", PROPLIST_ITEM(id++, "设计操作摇摆角度", "设计操作摇摆角度"));
	AddPropItem("WorkingRockAngle", PROPLIST_ITEM(id++, "设计带电作业角度", "设计带电作业摇摆角度"));
	AddPropItem("ManuFacturer", PROPLIST_ITEM(id++, "生产厂家", "生产厂家"));
	AddPropItem("MaterialCode", PROPLIST_ITEM(id++, "物资编码", "物资编码"));
}
int CTowerGimInfo::GetPropValueStr(long id, char* valueStr, UINT nMaxStrBufLen/*=100*/)	//根据属性ID得到属性值字符串
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
