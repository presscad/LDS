// Bolt.cpp: implementation of the C3dLs class.
//wbt translation
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tower.h"
#include "lds_part.h"
#include "SolidBody.h"
#include "CreateFace.h"
#include "../StdPartSolid/EBBoard.h"
#include "../StdPartSolid/GDBoard.h"
#include "../StdPartSolid/URing.h"
#include "../StdPartSolid/UBBoard.h"
#include "../StdPartSolid/UScrew.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CLDSExternalPart
//////////////////////////////////////////////////////////////////////////
CLDSExternalPart::CLDSExternalPart()
{
	m_nClassTypeId=CLS_EXTERNALPART;
	//volume=0;
}
BOOL CLDSExternalPart::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/,
	double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	return FALSE;
	/*if(CreateShadowPart3dSolidModel(bDisplayHole,scale_of_user2scr,sample_len,smoothness))
		return TRUE;

	if(pSolidBody==NULL)
		pSolidBody = new CSolidBody;
	else if(!IsModified())	//数据未修改
		return TRUE;
	else
		pSolidBody->Empty();
	SetModified(FALSE);
	fBody body;
	fBody *pBody=&body;
	for(f3dPoint *pPt=Vertexs.GetFirst();pPt;pPt=Vertexs.GetNext())
		pBody->vertex.append(*pPt);
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	for(ATOM_LIST<int> *pList=FaceVertexIndexs.GetFirst();pList;pList=FaceVertexIndexs.GetNext())
	{
		f3dPolyFace *pFace=pBody->faceList.append();
		pFace->material = crMaterial;
		int *pPrevIndex=pList->GetTail();
		for(int *pIndex=pList->GetFirst();pIndex;pIndex=pList->GetNext())
		{
			createFace.NewOutterEdgeLine(pFace,*pIndex,*pPrevIndex);
			pPrevIndex=pIndex;
		}
	}
	for(f3dPoint *pVertex=pBody->vertex.GetFirst();pVertex;pVertex=pBody->vertex.GetNext())
		coord_trans(*pVertex,ucs,TRUE);
	pSolidBody->ConvertFrom(pBody);
	SetModified(FALSE);
	return TRUE;*/
}
//////////////////////////////////////////////////////////////////////
// CLDSInsulator class definition
//////////////////////////////////////////////////////////////////////
CLDSInsulator::CLDSInsulator()
{
	m_nClassTypeId=CLS_INSULATOR;
	//strcpy(m_sClassName,"CLDSInsulator");
}
//////////////////////////////////////////////////////////////////////
//  CLDSFitting class definition
//////////////////////////////////////////////////////////////////////
CLDSFitting::CLDSFitting()
{
	m_nClassTypeId=CLS_FITTINGS;
	//strcpy(m_sClassName,"CLDSFitting");
	this->m_ciFittingType=CLDSFitting::TYPE_EB;
	this->sizespec.Copy("EB-21/42-100");
	m_iLiveSwingAngle=m_siMinSwingAngle=m_siMaxSwingAngle=0;
	sizespecUR.Copy("U-16");//
	this->m_iURLiveSwingAngle = 0;
	this->m_siMinURSwingAngle = -15;
	this->m_siMaxURSwingAngle =  15;
	this->is_data_modified=this->is_solid_modified=true;
	m_iURLiveSwingAngle=0;//U型连接挂环的实时摆动角
	m_siMinURSwingAngle=-15;
	m_siMaxURSwingAngle= 15;	//U型环金具摆动角范围
}
CLDSFitting::CLDSFitting(char ciFittingType,const char* szFittingSizeCode)
{
	m_nClassTypeId=CLS_FITTINGS;
	//strcpy(m_sClassName,"CLDSFitting");
	this->m_ciFittingType=ciFittingType;
	this->sizespec.Copy(szFittingSizeCode);
	m_iLiveSwingAngle=m_siMinSwingAngle=m_siMaxSwingAngle=0;
#if defined(__LDS_CONTEXT_)||defined(__LDS_GROUP_CONTEXT_)
	sizespecUR=MatchUringSizeCode(ciFittingType,szFittingSizeCode);
#endif
	basic=LoadFittingBasicInfo(ciFittingType,szFittingSizeCode);
	this->m_iURLiveSwingAngle = 0;
	this->m_siMinURSwingAngle = -15;
	this->m_siMaxURSwingAngle =  15;
	m_iURLiveSwingAngle=0;//U型连接挂环的实时摆动角
	m_siMinURSwingAngle=-15;
	m_siMaxURSwingAngle= 15;	//U型环金具摆动角范围
	this->is_data_modified=this->is_solid_modified=true;
}
CLDSFitting::BASICINFO::BASICINFO()
{
	memset(this,0,sizeof(BASICINFO));
}
CLDSFitting::BASICINFO CLDSFitting::LoadFittingBasicInfo(char ciFittingType,const char* szFittingSizeCode)
{
	CLDSFitting::BASICINFO basicinfo;
	if (ciFittingType == 0)
	{	//0.EB金具
		FITTING_EB *pEBInfo = QueryFitting<FITTING_EB>(szFittingSizeCode);
		if (pEBInfo != NULL)
		{
			StrCopy(basicinfo.szSizeCode,pEBInfo->szSizeCode,16);	//型号代码
			basicinfo.siBoardThick=pEBInfo->siThick;	//挂板厚度b
			basicinfo.dfBoardL=pEBInfo->dfBoardL;		//金具(挂板部分)宽度
			basicinfo.dfTrunnionD=pEBInfo->dfTrunnionD;	//耳轴直径
			//basicinfo.dfTrunnionL=pEBInfo->;	//耳轴总长
			basicinfo.dfHoleD=pEBInfo->dfHoleD;				//挂孔直径(diameter)Ф
			basicinfo.dfHole2TrunnionH=pEBInfo->dfBoardH;	//挂孔中心到耳轴中心线距离H
		}
	}
	else if (ciFittingType == 1)
	{	//1.GD金具(挂板：Clevis）
		FITTING_GD *pGDInfo = QueryFitting<FITTING_GD>(szFittingSizeCode);
		if (pGDInfo != NULL)
		{
			StrCopy(basicinfo.szSizeCode,pGDInfo->szSizeCode,16);	//型号代码
			basicinfo.siBoardThick=pGDInfo->siThick;	//挂板厚度b
			basicinfo.dfBoardL=pGDInfo->dfBoardL;		//金具(挂板部分)宽度
			basicinfo.dfTrunnionD=pGDInfo->dfTrunnionD;	//耳轴直径
			basicinfo.dfTrunnionL=pGDInfo->dfTrunnionL;	//耳轴总长
			basicinfo.dfHoleD=pGDInfo->dfHoleD;				//挂孔直径(diameter)Ф
			basicinfo.dfHole2TrunnionH=pGDInfo->dfBoardH2;	//挂孔中心到耳轴中心线距离H
		}
	}
	return basicinfo;
}
CLDSFitting::~CLDSFitting()
{
	if (pSolidBody)
	{
		delete pSolidBody;
		pSolidBody=NULL;
	}
}
void CLDSFitting::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020200) ||	//TMA V5.2.2.1
		(doc_type == 2 && version >= 2010200) ||	//LMA V2.1.2.0
		(doc_type == 4 && version >= 1030901) ||	//LDS V1.3.9.1
		(doc_type == 5 && version >= 1030901))		//TDA V1.3.9.1
	{
		buffer.WriteByte(m_ciFittingType);
		buffer.WriteWord(m_iLiveSwingAngle);
		buffer.WriteWord(m_siMinSwingAngle);
		buffer.WriteWord(m_siMaxSwingAngle);

		buffer.WriteWord(m_iURLiveSwingAngle);
		buffer.WriteWord(m_siMinURSwingAngle);
		buffer.WriteWord(m_siMaxURSwingAngle);
		buffer.WriteString(sizespec);
		buffer.WriteString(sizespecUR);
		buffer.WriteInteger(holes[0].hBolt);
		buffer.WriteInteger(holes[0].hResidePart);
		buffer.WriteInteger(holes[1].hBolt);
		buffer.WriteInteger(holes[1].hResidePart);
		int count = params.GetNodeNum();
		buffer.WriteInteger(count);
		for (CLDSGeneralPlate::DESIGN_PARAM *pParam = params.GetFirst(); pParam != NULL; pParam = params.GetNext())
		{
			buffer.WriteWord(pParam->wItemKey);
			buffer.WriteByte(pParam->cParamType);
			buffer.WriteByte(pParam->cValueType);
			buffer.WriteDouble(pParam->value.fValue);
		}
	}
	if ((version == 0 && doc_type != 3)||
		(doc_type==1&&version>=5020300)||	//TMA V5.2.3.0
		(doc_type==2&&version>=2010300)||	//LMA V2.1.3.0
		(doc_type==4&&version>=1031100)||	//LDS V1.3.11.0
		(doc_type==5&&version>=1031100))	//TDA V1.3.11.0
	{
		buffer.WriteInteger(handle);
		buffer.WriteString(layer_tag);
		buffer.WriteInteger(iSeg);
		buffer.WritePoint(ucs.origin);
		buffer.WritePoint(ucs.axis_x);
		buffer.WritePoint(ucs.axis_y);
		buffer.WritePoint(ucs.axis_z);
	}
}
void CLDSFitting::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020200) ||	//TMA V5.2.2.1
		(doc_type == 2 && version >= 2010200) ||	//LMA V2.1.2.0
		(doc_type == 4 && version >= 1030901) ||	//LDS V1.3.9.1
		(doc_type == 5 && version >= 1030901))		//TDA V1.3.9.1
	{
		buffer.ReadByte(&m_ciFittingType);
		buffer.ReadWord(&m_iLiveSwingAngle);
		buffer.ReadWord(&m_siMinSwingAngle);
		buffer.ReadWord(&m_siMaxSwingAngle);
		buffer.ReadWord(&m_iURLiveSwingAngle);
		buffer.ReadWord(&m_siMinURSwingAngle);
		buffer.ReadWord(&m_siMaxURSwingAngle);
		buffer.ReadString(sizespec,50);
		buffer.ReadString(sizespecUR,50);
		buffer.ReadInteger(&holes[0].hBolt);
		buffer.ReadInteger(&holes[0].hResidePart);
		buffer.ReadInteger(&holes[1].hBolt);
		buffer.ReadInteger(&holes[1].hResidePart);

		int count = 0;
		buffer.ReadInteger(&count);
		for (int i = 0; i < count; i++)
		{
			CLDSGeneralPlate::DESIGN_PARAM param;
			buffer.ReadWord(&param.wItemKey);
			buffer.ReadByte(&param.cParamType);
			buffer.ReadByte(&param.cValueType);
			buffer.ReadDouble(&param.value.fValue);
			param.bExist = true;
			params.Append(param,param.wItemKey);
		}
	}
	if ((version==0&&doc_type!=3) ||
		(doc_type==1&&version>=5020300)||	//TMA V5.2.3.0
		(doc_type==2&&version>=2010300)||	//LMA V2.1.3.0
		(doc_type==4&&version>=1031100)||	//LDS V1.3.11.0
		(doc_type==5&&version>=1031100))	//TDA V1.3.11.0
	{
		buffer.ReadInteger(&handle);
		buffer.ReadString(layer_tag);
		buffer.ReadInteger(&_iSeg.iSeg);
		buffer.ReadPoint(ucs.origin);
		buffer.ReadPoint(ucs.axis_x);
		buffer.ReadPoint(ucs.axis_y);
		buffer.ReadPoint(ucs.axis_z);
	}
}
CSolidPart CLDSFitting::GetSolidPartObject()
{
	CSolidPart solidPart;
	solidPart.m_hPart = handle;
	solidPart.m_idClassType = CLS_FITTINGS;
	solidPart.m_iLineType = 0;
	solidPart.pBody = pSolidBody;
	return solidPart;
}
void CLDSFitting::CopyFrom(CLDSFitting* pSrcFitting)
{
	char pool[2048];
	CBuffer buff(pool,2048);
	pSrcFitting->ToBuffer(buff);
	buff.SeekToBegin();
	FromBuffer(buff);
}
void CLDSFitting::DesignFitting()
{
	SetModified();
	CLDSBolt* pHole1=(CLDSBolt*)BelongModel()->FromPartHandle(holes[0].hBolt,CLS_BOLT);
	CLDSBolt* pHole2=(CLDSBolt*)BelongModel()->FromPartHandle(holes[1].hBolt,CLS_BOLT);
	CLDSPart* pResidePart1=BelongModel()->FromPartHandle(holes[0].hResidePart);
	CLDSPart* pResidePart2=BelongModel()->FromPartHandle(holes[1].hResidePart);
	double fHoleL=0,fBoardL=0;
	if (m_ciFittingType!=CLDSFitting::TYPE_UR)
	{
		if (pHole1==NULL||pHole2==NULL||pResidePart1==NULL||pResidePart2==NULL)
			return;
		f3dPoint xBasePtL,xBasePtR,xBaseVec;
		int liPlane=0;
		pResidePart1->GetBoltIntersPos(pHole1,xBasePtL,&liPlane,CLDSPart::DATUM_PLANE);
		pResidePart2->GetBoltIntersPos(pHole2,xBasePtR,&liPlane,CLDSPart::DATUM_PLANE);
		//计算坐标系
		ucs.axis_x=(xBasePtR-xBasePtL).normalized();
		if (pResidePart1->IsPlate())
		{	//钢板的基准面即底面，并不一定是内贴面，故需要差别处理 wjh-2019.12.27
			SmartPlatePtr pBasicPlate=(CLDSGeneralPlate*)pResidePart1;
			GEPOINT vzWorkNormal=(liPlane==0)?pBasicPlate->ucs.axis_z:pBasicPlate->GetHuoquFaceNorm(liPlane);
			if (vzWorkNormal*ucs.axis_x>0)	//左侧孔所在钢板法线与耳轴方向相同，应增加板厚 wjh-2019.12.27
				xBasePtL+=vzWorkNormal*pBasicPlate->Thick;
		}
		if (pResidePart2->IsPlate())
		{	//钢板的基准面即底面，并不一定是内贴面，故需要差别处理 wjh-2019.12.27
			SmartPlatePtr pBasicPlate=(CLDSGeneralPlate*)pResidePart2;
			GEPOINT vzWorkNormal=(liPlane==0)?pBasicPlate->ucs.axis_z:pBasicPlate->GetHuoquFaceNorm(liPlane);
			if (vzWorkNormal*ucs.axis_x<0)	//右侧孔所在钢板法线与耳轴方向相反，应增加板厚 wjh-2019.12.27
				xBasePtR+=vzWorkNormal*pBasicPlate->Thick;
		}
		ucs.origin=(xBasePtL+xBasePtR)*0.5;

		fHoleL=DISTANCE(xBasePtL,xBasePtR);
		if(pResidePart1)
			fHoleL+=pResidePart1->GetThick();
		if(pResidePart2)
			fHoleL+=pResidePart2->GetThick();
		if (m_ciFittingType == CLDSFitting::TYPE_EB|| 
			m_ciFittingType == CLDSFitting::TYPE_US||
			m_ciFittingType == CLDSFitting::TYPE_UB)
			ucs.axis_y.Set(0, 0, -1);
		else if(m_ciFittingType == CLDSFitting::TYPE_GD)
		{
			double dd1=ucs.axis_x*f3dPoint(1,0,0);
			double dd2=ucs.axis_x*f3dPoint(0,1,0);
			if (fabs(dd1)>fabs(dd2))
			{
				if (dd1>0)
					ucs.axis_y.Set(0,-1,0);
				else
					ucs.axis_y.Set(0,1,0);
			}
			else
			{
				if (dd2>0)
					ucs.axis_y.Set(1,0,0);
				else
					ucs.axis_y.Set(-1,0,0);
			}
		}
		if (abs(m_iLiveSwingAngle)>0)
		{
			double angle=m_iLiveSwingAngle*RADTODEG_COEF;
			RotateVectorAroundVector(ucs.axis_y,angle,ucs.axis_x);
		}
		ucs.axis_z=ucs.axis_x^ucs.axis_y;
		normalize(ucs.axis_z);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
	}
	else
	{

	}
	FITTING_EB* pEB=QueryFitting<FITTING_EB>(sizespec);
	FITTING_GD* pGD=QueryFitting<FITTING_GD>(sizespec);
	FITTING_US* pUS = QueryFitting<FITTING_US>(sizespec);
	FITTING_UB* pUB = QueryFitting<FITTING_UB>(sizespec);
	if (m_ciFittingType == CLDSFitting::TYPE_EB || 
		m_ciFittingType == CLDSFitting::TYPE_GD||
		m_ciFittingType == CLDSFitting::TYPE_UB)
	{
		if (m_ciFittingType == CLDSFitting::TYPE_EB && pEB)
			fBoardL = pEB->dfBoardL;
		if (m_ciFittingType == CLDSFitting::TYPE_GD && pGD)
			fBoardL = pGD->dfBoardL;
		if (m_ciFittingType == CLDSFitting::TYPE_UB&& pUB)
			fBoardL = pUB->dfBoardW;
		if (fHoleL > 0 && fHoleL > fBoardL)
		{
			CLDSGeneralPlate::DESIGN_PARAM paramS;
			paramS.InitBasicInfo(KEY2C("S"), DESIGN_PARAM_ITEM::TYPE_INTEGER);
			paramS.value.iValue = ftoi((fHoleL - fBoardL)*0.5);
			this->params.SetValue(KEY4C("S"), paramS);
		}
	}
	if (m_ciFittingType == CLDSFitting::TYPE_US)
	{
		if (pResidePart1)
		{
			CLDSGeneralPlate::DESIGN_PARAM paramS;
			paramS.InitBasicInfo(KEY2C("S"), DESIGN_PARAM_ITEM::TYPE_INTEGER);
			paramS.value.iValue = ftoi(pResidePart1->Thick);
			this->params.SetValue(KEY4C("S"), paramS);
		}
	}
}
//根据一级挂接金具中的挂孔直径匹配合适的U形环型号
#if defined(__LDS_CONTEXT_)||defined(__LDS_GROUP_CONTEXT_)
CXhChar50 CLDSFitting::MatchUringSizeCode(char ciFittingType,const char* szFittingSizeCode)
{
	double dfHoleD = 0;
	if (ciFittingType == 0)
	{	//0.EB金具
		FITTING_EB *pEBInfo = QueryFitting<FITTING_EB>(szFittingSizeCode);
		if (pEBInfo == NULL)
			return "";
		dfHoleD = pEBInfo->dfHoleD;
	}
	else if (ciFittingType == 1)
	{	//1.GD金具(挂板：Clevis）
		FITTING_GD *pGDInfo = QueryFitting<FITTING_GD>(szFittingSizeCode);
		if (pGDInfo == NULL)
			return "";
		dfHoleD = pGDInfo->dfHoleD;
	}
	if (dfHoleD==0)
		return "";
	IFittingLibrary<FITTING_UR>* pxarrURLib=FittingLibrary<FITTING_UR>();
	for (int i=1;i<pxarrURLib->Count;i++)
	{
		if ((*pxarrURLib)[i].dfShaftD>=dfHoleD)
			return CXhChar50((*pxarrURLib)[i-1].szSizeCode);
	}
	return CXhChar50((*pxarrURLib)[pxarrURLib->Count-1].szSizeCode);
}
bool CLDSFitting::MatchUring(DWORD dwReservedMatchParam/*=0*/)
{
	double dfHoleD = 0;
	basic=LoadFittingBasicInfo(m_ciFittingType,sizespec);
	if (m_ciFittingType == 0)
	{	//0.EB金具
		FITTING_EB *pEBInfo = QueryFitting<FITTING_EB>(sizespec);
		if (pEBInfo == NULL)
			return FALSE;
		dfHoleD = pEBInfo->dfHoleD;
	}
	else if (m_ciFittingType == 1)
	{	//1.GD金具(挂板：Clevis）
		FITTING_GD *pGDInfo = QueryFitting<FITTING_GD>(sizespec);
		if (pGDInfo == NULL)
			return FALSE;
		dfHoleD = pGDInfo->dfHoleD;
	}
	if (dfHoleD==0)
		return FALSE;
	IFittingLibrary<FITTING_UR>* pxarrURLib=FittingLibrary<FITTING_UR>();
	for (int i=1;i<pxarrURLib->Count;i++)
	{
		if ((*pxarrURLib)[i].dfShaftD>=dfHoleD)
		{
			sizespecUR.Copy((*pxarrURLib)[i-1].szSizeCode);
			return true;
		}
	}
	sizespecUR.Copy((*pxarrURLib)[pxarrURLib->Count-1].szSizeCode);
	return true;
}
#endif
BOOL CLDSFitting::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/,double sample_len/*=5.0*/,int smoothness/*=36*/)
{
#ifndef __DRAWING_CONTEXT_
	if (pSolidBody!=NULL&& !IsSolidModified())
		return TRUE;
	else
	{
		if (pSolidBody == NULL)
			pSolidBody = new CSolidBody;
		else
			pSolidBody->Empty();
	}
	SetModified(FALSE);
	if (m_ciFittingType==0)
	{	//0.EB金具
		FITTING_EB *pEBInfo=QueryFitting<FITTING_EB>(sizespec);
		if (pEBInfo==NULL)
			return FALSE;
		CEBBoard eb(pEBInfo);
		CLDSGeneralPlate::DESIGN_PARAM *pParamS=params.GetValue(KEY2C("S"));
		if (pParamS)
			eb.eb.siThroughThick=(short)pParamS->iValue;
		FITTING_UR* pURInfo = QueryFitting<FITTING_UR>(sizespecUR);
		if (pURInfo)
		{
			fBody body;
			eb.acs.InitStdCS();
			eb.AppendSolidBody(&body,false);
			GECS acs(GEPOINT(0,-pEBInfo->dfBoardH,0),GEPOINT(0,0,1),GEPOINT(0,-1,0),GEPOINT(1,0,0));
			double dfLiveUrSwingRadAngle=this->m_iURLiveSwingAngle*RADTODEG_COEF;
			RotateVectorAroundVector(acs.axis_y,dfLiveUrSwingRadAngle,GEPOINT(0,0,1));
			RotateVectorAroundVector(acs.axis_z,dfLiveUrSwingRadAngle,GEPOINT(0,0,1));
			CURing ur(pURInfo);
			ur.acs = acs;
			ur.AppendSolidBody(&body, true);
			//装配到模型实际位置
			for (f3dPolyFace* pFace=body.faceList.GetFirst();pFace;pFace=body.faceList.GetNext())
			{
				if (pFace->poly_norm.IsZero())
					continue;
				vector_trans(pFace->poly_norm,ucs,TRUE,TRUE);
			}
			for (f3dPoint* pVertice=body.vertex.GetFirst();pVertice;pVertice=body.vertex.GetNext())
			{
				if (coord_trans(*pVertice,ucs,TRUE,TRUE)!=TRUE)
					return FALSE;
			}
			return pSolidBody->ConvertFrom(&body);
		}
		else
		{
			eb.acs=ucs;
			return eb.Create3dSolidModel(pSolidBody);
		}
	}
	else if (m_ciFittingType==1)
	{	//1.GD金具(挂板：Clevis）
		FITTING_GD *pGDInfo=QueryFitting<FITTING_GD>(sizespec);
		if (pGDInfo==NULL)
			return FALSE;
		CGDBoard gd(pGDInfo);
		CLDSGeneralPlate::DESIGN_PARAM *pParamS=params.GetValue(KEY2C("S"));
		if (pParamS)
			gd.gd.siThroughThick=(short)pParamS->iValue;
		FITTING_UR* pURInfo = QueryFitting<FITTING_UR>(sizespecUR);
		if (pURInfo)
		{
			fBody body;
			gd.acs.InitStdCS();
			gd.AppendSolidBody(&body,false);
			GECS acs(GEPOINT(0,-pGDInfo->dfBoardH2,0),GEPOINT(0,0,1),GEPOINT(0,-1,0),GEPOINT(1,0,0));
			double dfLiveUrSwingRadAngle=this->m_iURLiveSwingAngle*RADTODEG_COEF;
			RotateVectorAroundVector(acs.axis_y,dfLiveUrSwingRadAngle,GEPOINT(0,0,1));
			RotateVectorAroundVector(acs.axis_z,dfLiveUrSwingRadAngle,GEPOINT(0,0,1));
			CURing ur(pURInfo);
			ur.acs = acs;
			ur.AppendSolidBody(&body, true);
			//装配到模型实际位置
			for (f3dPolyFace* pFace=body.faceList.GetFirst();pFace;pFace=body.faceList.GetNext())
			{
				if (pFace->poly_norm.IsZero())
					continue;
				vector_trans(pFace->poly_norm,ucs,TRUE,TRUE);
			}
			for (f3dPoint* pVertice=body.vertex.GetFirst();pVertice;pVertice=body.vertex.GetNext())
			{
				if (coord_trans(*pVertice,ucs,TRUE,TRUE)!=TRUE)
					return FALSE;
			}
			return pSolidBody->ConvertFrom(&body);
		}
		else
		{
			gd.acs=ucs;
			return gd.Create3dSolidModel(pSolidBody);
		}
	}
	else if (m_ciFittingType==2)
	{	//2.U型螺栓挂环 
		CURing ur;
		return ur.Create3dSolidModel(pSolidBody);
	}
	else if (m_ciFittingType == 3)
	{	//3.U型螺栓
		FITTING_US *pUSInfo = QueryFitting<FITTING_US>(sizespec);
		if (pUSInfo == NULL)
			return FALSE;
		UScrew us(pUSInfo);
		CLDSGeneralPlate::DESIGN_PARAM *pParamS = params.GetValue(KEY2C("S"));
		if(pParamS)
			us.us.siThroughThick= (short)pParamS->iValue;
		FITTING_UR* pURInfo = QueryFitting<FITTING_UR>(sizespecUR);
		if (pURInfo)
		{
			fBody body;
			us.acs.InitStdCS();
			us.AppendSolidBody(&body, false);
			double fOffDist = us.us.dfColumnL1 - us.us.dfColumnL2 + us.GetNutH();
			GECS acs(GEPOINT(0, -fOffDist-pURInfo->dfNamedL, 0), GEPOINT(0, 0, 1), GEPOINT(0,1,0), GEPOINT(-1, 0, 0));
			double dfLiveUrSwingRadAngle = this->m_iURLiveSwingAngle*RADTODEG_COEF;
			rotate_point_around_axis(acs.origin, dfLiveUrSwingRadAngle, GEPOINT(0, -fOffDist, 0), GEPOINT(0, -fOffDist, 1));
			RotateVectorAroundVector(acs.axis_y, dfLiveUrSwingRadAngle, GEPOINT(0, 0, 1));
			RotateVectorAroundVector(acs.axis_z, dfLiveUrSwingRadAngle, GEPOINT(0, 0, 1));
			CURing ur(pURInfo);
			ur.acs = acs;
			ur.AppendSolidBody(&body, true);
			//装配到模型实际位置
			for (f3dPolyFace* pFace = body.faceList.GetFirst(); pFace; pFace = body.faceList.GetNext())
			{
				if (pFace->poly_norm.IsZero())
					continue;
				vector_trans(pFace->poly_norm, ucs, TRUE, TRUE);
			}
			for (f3dPoint* pVertice = body.vertex.GetFirst(); pVertice; pVertice = body.vertex.GetNext())
			{
				if (coord_trans(*pVertice, ucs, TRUE, TRUE) != TRUE)
					return FALSE;
			}
			return pSolidBody->ConvertFrom(&body);
		}
		else
		{
			us.acs = ucs;
			return us.Create3dSolidModel(pSolidBody);
		}
	}
	else if (m_ciFittingType == 4)
	{	//4.U型挂板
		FITTING_UB *pUBInfo = QueryFitting<FITTING_UB>(sizespec);
		if (pUBInfo == NULL)
			return FALSE;
		UBBoard ub(pUBInfo);
		CLDSGeneralPlate::DESIGN_PARAM *pParamS = params.GetValue(KEY2C("S"));
		if (pParamS)
			ub.ub.siThroughThick = (short)pParamS->iValue;
		ub.acs = ucs;
		return ub.Create3dSolidModel(pSolidBody);
	}
#endif
	return FALSE;
}
IMPLEMENT_PROP_FUNC(CLDSFitting);
void CLDSFitting::InitPropHashtable()
{
	int id=1;
	CLDSFitting::propHashtable.SetHashTableGrowSize(500);
	CLDSFitting::propStatusHashtable.CreateHashTable(50);
#ifdef AFX_TARG_ENU_ENGLISH
	AddPropItem("BasicInfo",PROPLIST_ITEM(id++,"BasicInfo"));
	AddPropItem("FittingType",PROPLIST_ITEM(id++,"FittingType","","0.EB|1.GD|2.U"));
	AddPropItem("SizeCode",PROPLIST_ITEM(id++,"SizeCode",""));
	AddPropItem("Hole1",PROPLIST_ITEM(id++,"Hole1"));
	AddPropItem("Hole1.hBolt",PROPLIST_ITEM(id++,"Bolt"));
	AddPropItem("Hole1.hPart",PROPLIST_ITEM(id++,"Part"));
	AddPropItem("Hole2",PROPLIST_ITEM(id++,"Hole1"));
	AddPropItem("Hole2.hBolt",PROPLIST_ITEM(id++,"Bolt"));
	AddPropItem("Hole2.hPart",PROPLIST_ITEM(id++,"Part"));
	AddPropItem("SwingAngle",PROPLIST_ITEM(id++,"SwingAngle"));
	AddPropItem("Profile",PROPLIST_ITEM(id++,"Profile"));
	AddPropItem("eb.Thick",PROPLIST_ITEM(id++,"Thick"));
	AddPropItem("eb.ThroughS",PROPLIST_ITEM(id++,"ThroughS"));
	AddPropItem("eb.TrunnionD",PROPLIST_ITEM(id++,"TrunnionD"));
	AddPropItem("eb.HoleD",PROPLIST_ITEM(id++,"HoleD"));
	AddPropItem("eb.BoardL",PROPLIST_ITEM(id++,"BoardL"));
	AddPropItem("eb.BoardH",PROPLIST_ITEM(id++,"BoardH"));
	AddPropItem("eb.ChinR",PROPLIST_ITEM(id++,"ChinR"));
	AddPropItem("gd.Thick",PROPLIST_ITEM(id++,"Thick"));
	AddPropItem("gd.ThroughS",PROPLIST_ITEM(id++,"ThroughS"));
	AddPropItem("gd.TrunnionD",PROPLIST_ITEM(id++,"TrunnionD"));
	AddPropItem("gd.TrunnionL",PROPLIST_ITEM(id++,"TrunnionL"));
	AddPropItem("gd.BoardL",PROPLIST_ITEM(id++,"BoardL"));
	AddPropItem("gd.HoleD",PROPLIST_ITEM(id++,"HoleD"));
	AddPropItem("gd.BoardH",PROPLIST_ITEM(id++,"BoardH"));
	AddPropItem("gd.BoardH1",PROPLIST_ITEM(id++,"BoardH1"));
	AddPropItem("gd.BoardH2",PROPLIST_ITEM(id++,"BoardH2"));
#else
	AddPropItem("BasicInfo",PROPLIST_ITEM(id++,"基本信息"));
	AddPropItem("handle", PROPLIST_ITEM(id++, "句柄", ""));
	AddPropItem("layer", PROPLIST_ITEM(id++, "图层", ""));
	AddPropItem("iSeg", PROPLIST_ITEM(id++, "分段号", ""));
	AddPropItem("FittingType",PROPLIST_ITEM(id++,"金具类型","","0.EB挂板|1.GD挂板|2.U型挂环|3.U型螺母|4.U型挂板"));
	AddPropItem("SizeCode",PROPLIST_ITEM(id++,"挂板型号",""));
	AddPropItem("URSizeCode",PROPLIST_ITEM(id++,"U形环型号",""));
	AddPropItem("Hole1",PROPLIST_ITEM(id++,"1号挂孔","金具所在挂孔1"));
	AddPropItem("Hole1.hBolt",PROPLIST_ITEM(id++,"关联螺栓孔","挂孔关联螺栓孔"));
	AddPropItem("Hole1.hPart",PROPLIST_ITEM(id++,"所在构件","挂孔所在构件"));
	AddPropItem("Hole2",PROPLIST_ITEM(id++,"2号挂孔","金具所在挂孔2"));
	AddPropItem("Hole2.hBolt",PROPLIST_ITEM(id++,"关联螺栓孔","挂孔关联螺栓孔"));
	AddPropItem("Hole2.hPart",PROPLIST_ITEM(id++,"所在构件","挂孔所在构件"));
	AddPropItem("SwingAngle",PROPLIST_ITEM(id++,"主金具当前摆角,°"));
	AddPropItem("MinSwingAngle",PROPLIST_ITEM(id++,"起始摆角,°"));
	AddPropItem("MaxSwingAngle",PROPLIST_ITEM(id++,"终止摆角,°"));
	AddPropItem("URSwingAngle",PROPLIST_ITEM(id++,"U形环当前摆角,°"));
	AddPropItem("MinURSwingAngle",PROPLIST_ITEM(id++,"起始摆角,°"));
	AddPropItem("MaxURSwingAngle",PROPLIST_ITEM(id++,"终止摆角,°"));

	AddPropItem("assemblePos", PROPLIST_ITEM(id++, "装配方位"));
	AddPropItem("ucs.origin", PROPLIST_ITEM(id++, "位置"));
	AddPropItem("ucs.origin.x", PROPLIST_ITEM(id++, "X坐标"));
	AddPropItem("ucs.origin.y", PROPLIST_ITEM(id++, "Y坐标"));
	AddPropItem("ucs.origin.z", PROPLIST_ITEM(id++, "Z坐标"));
	AddPropItem("ucs.axis_x", PROPLIST_ITEM(id++, "X轴方向"));
	AddPropItem("ucs.axis_x.x", PROPLIST_ITEM(id++, "X坐标分量"));
	AddPropItem("ucs.axis_x.y", PROPLIST_ITEM(id++, "Y坐标分量"));
	AddPropItem("ucs.axis_x.z", PROPLIST_ITEM(id++, "Z坐标分量"));
	AddPropItem("ucs.axis_y", PROPLIST_ITEM(id++, "Y轴方向"));
	AddPropItem("ucs.axis_y.x", PROPLIST_ITEM(id++, "X坐标分量"));
	AddPropItem("ucs.axis_y.y", PROPLIST_ITEM(id++, "Y坐标分量"));
	AddPropItem("ucs.axis_y.z", PROPLIST_ITEM(id++, "Z坐标分量"));
	AddPropItem("ucs.axis_z", PROPLIST_ITEM(id++, "Z轴方向"));
	AddPropItem("ucs.axis_z.x", PROPLIST_ITEM(id++, "X坐标分量"));
	AddPropItem("ucs.axis_z.y", PROPLIST_ITEM(id++, "Y坐标分量"));
	AddPropItem("ucs.axis_z.z", PROPLIST_ITEM(id++, "Z坐标分量"));

	AddPropItem("Profile",PROPLIST_ITEM(id++,"外形设计"));
	AddPropItem("eb.Thick",PROPLIST_ITEM(id++,"挂板厚度"));
	AddPropItem("eb.ThroughS",PROPLIST_ITEM(id++,"预留通厚S"));
	AddPropItem("eb.TrunnionD",PROPLIST_ITEM(id++,"耳轴直径M"));
	AddPropItem("eb.HoleD",PROPLIST_ITEM(id++,"挂孔直径φ"));
	AddPropItem("eb.BoardL",PROPLIST_ITEM(id++,"挂板宽度L"));
	AddPropItem("eb.BoardH",PROPLIST_ITEM(id++,"挂孔到耳轴垂距H"));
	AddPropItem("eb.ChinR",PROPLIST_ITEM(id++,"圆弧半径R"));
	AddPropItem("gd.Thick",PROPLIST_ITEM(id++,"挂板厚度"));
	AddPropItem("gd.ThroughS",PROPLIST_ITEM(id++,"预留通厚S"));
	AddPropItem("gd.TrunnionD",PROPLIST_ITEM(id++,"耳轴直径M"));
	AddPropItem("gd.TrunnionL",PROPLIST_ITEM(id++,"耳轴总长L"));
	AddPropItem("gd.BoardL",PROPLIST_ITEM(id++,"挂板宽度L1"));
	AddPropItem("gd.HoleD",PROPLIST_ITEM(id++,"挂孔直径φ"));
	AddPropItem("gd.BoardH",PROPLIST_ITEM(id++,"下圆弧到孔垂距H"));
	AddPropItem("gd.BoardH1",PROPLIST_ITEM(id++,"上圆弧到轴垂距H1"));
	AddPropItem("gd.BoardH2",PROPLIST_ITEM(id++,"挂孔到轴垂距H2"));
#endif
}

int CLDSFitting::GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	FITTING_EB* pEB=QueryFitting<FITTING_EB>(sizespec);
	FITTING_GD* pGD=QueryFitting<FITTING_GD>(sizespec);
	if (GetPropID("FittingType")==id)
	{
		if (m_ciFittingType==CLDSFitting::TYPE_EB)
			sText.Copy("0.EB挂板");
		else if (m_ciFittingType==CLDSFitting::TYPE_GD)
			sText.Copy("1.GD挂板");
		else if (m_ciFittingType == CLDSFitting::TYPE_UR)
			sText.Copy("2.U型挂环");
		else if (m_ciFittingType == CLDSFitting::TYPE_US)
			sText.Copy("3.U型螺丝");
		else if (m_ciFittingType == CLDSFitting::TYPE_UB)
			sText.Copy("4.U型挂板");
	}
	else if (GetPropID("handle") == id)
	{
		sText.Printf("0X%X", handle);
	}
	else if (GetPropID("layer") == id)
		strcpy(sText, layer());
	else if (GetPropID("ucs.origin.x") == id)
		sprintf(sText, "%f", ucs.origin.x);
	else if (GetPropID("ucs.origin.y") == id)
		sprintf(sText, "%f", ucs.origin.y);
	else if (GetPropID("ucs.origin.z") == id)
		sprintf(sText, "%f", ucs.origin.z);
	else if (GetPropID("ucs.axis_x.x") == id)
		sprintf(sText, "%f", ucs.axis_x.x);
	else if (GetPropID("ucs.axis_x.y") == id)
		sprintf(sText, "%f", ucs.axis_x.y);
	else if (GetPropID("ucs.axis_x.z") == id)
		sprintf(sText, "%f", ucs.axis_x.z);
	else if (GetPropID("ucs.axis_y.x") == id)
		sprintf(sText, "%f", ucs.axis_y.x);
	else if (GetPropID("ucs.axis_y.y") == id)
		sprintf(sText, "%f", ucs.axis_y.y);
	else if (GetPropID("ucs.axis_y.z") == id)
		sprintf(sText, "%f", ucs.axis_y.z);
	else if (GetPropID("ucs.axis_z.x") == id)
		sprintf(sText, "%f", ucs.axis_z.x);
	else if (GetPropID("ucs.axis_z.y") == id)
		sprintf(sText, "%f", ucs.axis_z.y);
	else if (GetPropID("ucs.axis_z.z") == id)
		sprintf(sText, "%f", ucs.axis_z.z);
	else if (GetPropID("iSeg") == id)
		strcpy(sText, (char*)iSeg.ToString());
	else if (GetPropID("SizeCode")==id)
		sText.Copy(sizespec);
	else if (GetPropID("URSizeCode")==id)
		sText.Copy(sizespecUR);
	else if (GetPropID("SwingAngle")==id)
		sText.Printf("%d",m_iLiveSwingAngle);
	else if (GetPropID("MinSwingAngle")==id)
		sText.Printf("%d",m_siMinSwingAngle);
	else if (GetPropID("MaxSwingAngle")==id)
		sText.Printf("%d",m_siMaxSwingAngle);
	else if (GetPropID("URSwingAngle")==id)
		sText.Printf("%d",m_iURLiveSwingAngle);
	else if (GetPropID("MinURSwingAngle")==id)
		sText.Printf("%d",m_siMinURSwingAngle);
	else if (GetPropID("MaxURSwingAngle")==id)
		sText.Printf("%d",m_siMaxURSwingAngle);
	else if (GetPropID("Hole1.hBolt")==id)
	{
		if (holes[0].hBolt>0)
			sText.Printf("0X%X",holes[0].hBolt);
		else
			sText.Copy("0X0");
	}
	else if (GetPropID("Hole2.hBolt")==id)
	{
		if (holes[1].hBolt>0)
			sText.Printf("0X%X",holes[1].hBolt);
		else
			sText.Copy("0X0");
	}
	else if (GetPropID("Hole1.hPart")==id)
	{
		if (holes[0].hResidePart>0)
			sText.Printf("0X%X",holes[0].hResidePart);
		else
			sText.Copy("0X0");
	}
	else if (GetPropID("Hole2.hPart")==id)
	{
		if (holes[1].hResidePart>0)
			sText.Printf("0X%X",holes[1].hResidePart);
		else
			sText.Copy("0X0");
	}
	else if (GetPropID("eb.Thick")==id && pEB)
		sText.Printf("%d",pEB->siThick);
	else if (GetPropID("eb.ThroughS")==id && pEB)
		sText.Printf("%d",pEB->siThroughThick);
	else if (GetPropID("eb.TrunnionD")==id && pEB)
	{
		sText.Printf("%f",pEB->dfTrunnionD);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("eb.HoleD")==id && pEB)
	{
		sText.Printf("%f",pEB->dfHoleD);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("eb.BoardL")==id && pEB)
	{
		sText.Printf("%f",pEB->dfBoardL);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("eb.BoardH")==id && pEB)
	{
		sText.Printf("%f",pEB->dfBoardH);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("eb.ChinR")==id && pEB)
	{
		sText.Printf("%f",pEB->dfChinR);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("gd.Thick")==id && pGD)
		sText.Printf("%d",pGD->siThick);
	else if (GetPropID("gd.ThroughS")==id && pGD)
		sText.Printf("%d",pGD->siThroughThick);
	else if (GetPropID("gd.TrunnionD")==id && pGD)
	{
		sText.Printf("%f",pGD->dfTrunnionD);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("gd.TrunnionL")==id && pGD)
	{
		sText.Printf("%f",pGD->dfTrunnionL);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("gd.BoardL")==id && pGD)
	{
		sText.Printf("%f",pGD->dfBoardL);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("gd.HoleD")==id && pGD)
	{
		sText.Printf("%f",pGD->dfHoleD);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("gd.BoardH")==id && pGD)
	{
		sText.Printf("%f",pGD->dfBoardH);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("gd.BoardH1")==id && pGD)
	{
		sText.Printf("%f",pGD->dfBoardH1);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("gd.BoardH2")==id && pGD)
	{
		sText.Printf("%f",pGD->dfBoardH2);
		SimplifiedNumString(sText);
	}

	if (valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen-1);
	return strlen(sText);
}
