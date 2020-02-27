//<LOCALE_TRANSLATE BY hxr /> 2015-04-22
#include "stdafx.h"
#include "stdio.h"
#include "Tower.h"
#include ".\spherepartgroup.h"
#include "LogFile.h"
#include "SortFunc.h"
#include "SquareTree.h"
#include "PartGroup.h"
#include "LdsLocale.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

//////////////////////////////////////////////////////////////////////////
//CPartGroupLocatePoint
//////////////////////////////////////////////////////////////////////////
CPartGroupLocatePoint::CPartGroupLocatePoint()
{
	m_hPart=0;
	m_iOrderType=0;
	m_bDimSpace=true;
	m_fStartOffset=m_fEndOffset=m_fLenPosToDatumPart=m_fVertPosToDatumPart=0;
}
void CPartGroupLocatePoint::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.ReadPoint(m_xLocalCoordPos);
	buffer.ReadInteger(&m_hPart);
	buffer.ReadInteger(&m_iOrderType);
	buffer.ReadBoolean(&m_bDimSpace);
	buffer.ReadDouble(&m_fStartOffset);
	buffer.ReadDouble(&m_fEndOffset);
}
void CPartGroupLocatePoint::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WritePoint(m_xLocalCoordPos);
	buffer.WriteInteger(m_hPart);
	buffer.WriteInteger(m_iOrderType);
	buffer.WriteBoolean(m_bDimSpace);
	buffer.WriteDouble(m_fStartOffset);
	buffer.WriteDouble(m_fEndOffset);
}

void CPartGroupLocatePoint::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[500]="";
	fprintf(fp,"<标定点信息>\n");
	fprintf(fp,"<标定点坐标位置 LocalCoordPos=\"(%.3f,%.3f,%.3f)\"/>\n",m_xLocalCoordPos.x,m_xLocalCoordPos.y,m_xLocalCoordPos.z);
	fprintf(fp,"<基准构件 handle=\"0x%x\"/>\n",m_hPart);
	fprintf(fp,"<标定点级别 OrderType=\"%d\"/>\n",m_iOrderType);
	if(m_bDimSpace)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<是否为绝对尺寸标注点 bDimSpace=\"%s\"/>\n",sText);
	sprintf(sText,"%f",m_fStartOffset);
	SimplifiedNumString(sText);
	fprintf(fp,"<标定点始端偏移量 StartOffset=\"%s\"/>\n",sText);
	sprintf(sText,"%f",m_fEndOffset);
	SimplifiedNumString(sText);
	fprintf(fp,"<标定点终端偏移量 EndOffset=\"%s\"/>\n",sText);
	fprintf(fp,"</标定点信息>\n");
}
//////////////////////////////////////////////////////////////////////////
//CCheckLineDimension
//////////////////////////////////////////////////////////////////////////
CCheckLineDimension::CCheckLineDimension()
{
	m_bBeforeWeld=m_bSpecStartLocatePart=m_bSpecEndLocatePart=false;
	m_hStartPart=m_hEndPart=0;
}
void CCheckLineDimension::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.ReadBoolean(&m_bSpecStartLocatePart);
	buffer.ReadBoolean(&m_bSpecEndLocatePart);
	buffer.ReadInteger(&m_hStartPart);
	buffer.ReadInteger(&m_hEndPart);
	buffer.ReadPoint(m_xStartLocalCoordPos);
	buffer.ReadPoint(m_xEndLocalCoordPos);
	buffer.ReadBoolean(&m_bBeforeWeld);
}
void CCheckLineDimension::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteBoolean(m_bSpecStartLocatePart);
	buffer.WriteBoolean(m_bSpecEndLocatePart);
	buffer.WriteInteger(m_hStartPart);
	buffer.WriteInteger(m_hEndPart);
	buffer.WritePoint(m_xStartLocalCoordPos);
	buffer.WritePoint(m_xEndLocalCoordPos);
	buffer.WriteBoolean(m_bBeforeWeld);
}

void CCheckLineDimension::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[500]="";
	fprintf(fp,"<信息>\n");
	if(m_bSpecStartLocatePart)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<是否指定始端定位构件 bSpecStartLocatePart=\"%s\"/>\n",sText);
	if(m_bSpecEndLocatePart)
		strcpy(sText,"true");
	else
		strcpy(sText,"false");
	fprintf(fp,"<是否指定终端定位杆件 bSpecEndLocatePart=\"%s\"/>\n",sText);
	fprintf(fp,"<始端定位杆件 hStartPart=\"0x%x\"/>\n",m_hStartPart);
	fprintf(fp,"<终端定位杆件 hEndPart=\"0x%x\"/>\n",m_hEndPart);
	fprintf(fp,"<起始坐标 StartLocalCoordPos=\"(%.3f,%.3f,%.3f)\"/>\n",m_xStartLocalCoordPos.x,m_xStartLocalCoordPos.y,m_xStartLocalCoordPos.z);
	fprintf(fp,"<终止坐标 EndLocalCoorPos=\"(%.3f,%.3f,%.3f)\"/>\n",m_xEndLocalCoordPos.x,m_xEndLocalCoordPos.y,m_xEndLocalCoordPos.z);
	if(m_bBeforeWeld)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<是否为焊前检测线 bBeforeWeld=\"%s\"/>\n",sText);
	fprintf(fp,"</信息>\n");
}
//////////////////////////////////////////////////////////////////////////
//CAngularDimension
//////////////////////////////////////////////////////////////////////////
CAngularDimension::CAngularDimension()
{
	m_bStartLineIsDatumLine=true;
	m_hStartPart=m_hEndPart=0;
	m_fDimR=0;
}
void CAngularDimension::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.ReadBoolean(&m_bStartLineIsDatumLine);
	buffer.ReadInteger(&m_hStartPart);
	buffer.ReadInteger(&m_hEndPart);
	buffer.ReadDouble(&m_fDimR);
}
void CAngularDimension::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteBoolean(m_bStartLineIsDatumLine);
	buffer.WriteInteger(m_hStartPart);
	buffer.WriteInteger(m_hEndPart);
	buffer.WriteDouble(m_fDimR);
}

void CAngularDimension::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[500]="";
	fprintf(fp,"<信息>\n");
	if(m_bStartLineIsDatumLine)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<是否以基准线为起始边 bStartLineIsDatumLine=\"%s\"/>\n",sText);
	fprintf(fp,"<进行标注的起始构件 handle=\"0x%x\"/>\n",m_hStartPart);
	fprintf(fp,"<进行标注的终止构件 handle=\"0x%x\"/>\n",m_hEndPart);
	sprintf(sText,"%f",m_fDimR);
	SimplifiedNumString(sText);
	fprintf(fp,"<标注线半径 DimR=\"%s\"/>\n",sText);
	fprintf(fp,"</信息>\n");
}
////////////////////////////////////////////////////////////////////////////////
//DRAWFLAG
////////////////////////////////////////////////////////////////////////////////
int DRAWFLAG::GetPartDrawType(CLDSPart* pPart,double* drawPlaneNorm,BYTE* cExecuteDrawFlag/*=NULL*/)
{
	int h0v1e2=-1;	//0.平铺绘制 1.侧向绘制 2.绘制端面
	f3dPoint axis_z(drawPlaneNorm);
	if(pPart->IsAngle())
	{
		int x_wing0_y_wing1=-1;
		CLDSLineAngle *pJg=(CLDSLineAngle*)pPart;
		f3dPoint cur_jg_vec=pJg->End()-pJg->Start();
		normalize(cur_jg_vec);
		BOOL bInside=-1;
		if(cDrawFlag=='X'||cDrawFlag=='S')
			x_wing0_y_wing1=0;	//X肢为当前绘制肢
		else if(cDrawFlag=='Y'||cDrawFlag=='Q')
			x_wing0_y_wing1=1;	//Y肢为当前绘制肢
		else if(cDrawFlag=='Z'||fabs(cur_jg_vec*axis_z)>0.70710)
			x_wing0_y_wing1=2;
		else
		{
			double ddx = pJg->get_norm_x_wing()*axis_z;
			double ddy = pJg->get_norm_y_wing()*axis_z;
			bInside=FALSE;
			if(fabs(ddx)>fabs(ddy))
			{
				x_wing0_y_wing1 = 0;
				if(ddx>0)	//里铁
					bInside=TRUE;
			}
			else 
			{
				x_wing0_y_wing1 = 1;
				if(ddy>0)	//里铁
					bInside=TRUE;
			}
		}
		if(bInside==-1&&x_wing0_y_wing1==0)
		{
			if(pJg->get_norm_x_wing()*axis_z>0)
				bInside=TRUE;
			else
				bInside=FALSE;
		}
		else if(bInside==-1&&x_wing0_y_wing1==1)
		{
			if(pJg->get_norm_y_wing()*axis_z>0)
				bInside=TRUE;
			else
				bInside=FALSE;
		}
		if(pJg->m_bFootNail)
		{
			f3dPoint endPt=pJg->End();
			project_point(endPt,pJg->Start(),axis_z);
			f3dPoint cur_jg_vec=endPt-pJg->Start();
			normalize(cur_jg_vec);
			if(fabs(cur_jg_vec*axis_z)>0.70710)
				x_wing0_y_wing1=2;	//角钢脚钉截面
			else
				x_wing0_y_wing1=3;	//角钢脚钉正面
		}
		if(x_wing0_y_wing1==2)
			h0v1e2=1;
		else 
			h0v1e2=0;
		if(cExecuteDrawFlag)	//角钢当前绘制肢 wht 11-08-02
		{
			if(x_wing0_y_wing1==0&&bInside)
				*cExecuteDrawFlag='X';
			else if(x_wing0_y_wing1==0&&!bInside)
				*cExecuteDrawFlag='x';
			else if(x_wing0_y_wing1==1&&bInside)
				*cExecuteDrawFlag='Y';
			else if(x_wing0_y_wing1==1&&!bInside)
				*cExecuteDrawFlag='y';
			else
				*cExecuteDrawFlag='Z';
		}
	}
	else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pPlate=(CLDSParamPlate*)pPart;
		if(pPlate->m_iParamType==TYPE_ELBOWPLATE)
		{
			double dd=fabs(pPlate->ucs.axis_z*axis_z);
			if(dd>EPS_COS2)
				h0v1e2=0;
			else if(dd<0.707)
				h0v1e2=1;
		}
		else
		{	//未指定绘制方式的非肘板参数化板，提前自动判断绘制方式
			if((pPlate->m_iParamType==TYPE_UEND||pPlate->m_iParamType==TYPE_ROLLEND)
				&&fabs(axis_z*pPlate->ucs.axis_x)>EPS_COS2)
				h0v1e2=2;		//槽型插板或者U型插板需要绘制端面图即竖直绘制
			else if(fabs(axis_z*pPlate->ucs.axis_z)>0.707)
				h0v1e2=0;	//平铺绘制
			else
				h0v1e2=1;	//侧向绘制
		}
	}
	else if(pPart->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pPlate=(CLDSPlate*)pPart;
		if(cDrawFlag=='X')		//平铺绘制
			h0v1e2=0;
		else if(cDrawFlag=='Z')	//竖直绘制
			h0v1e2=1;
		else if(cDrawFlag=='S'&&pPlate->face_N>1)	//展开绘制多面板
			h0v1e2=0;
		else	//未指定绘制方式，自动判断绘制方式
		{
			GEPOINT vPrjNormal=pPlate->ucs.axis_z;
			if(pPlate->IsSplayFoldPlate()||pPlate->IsTubeFoldPlate())
			{
				vPrjNormal.Set();
				for(int i=0;i<pPlate->face_N-1;i++)
					vPrjNormal+=pPlate->GetHuoquFaceNorm(i);
				normalize(vPrjNormal);
			}
			if(fabs(axis_z*vPrjNormal)>0.707)
				h0v1e2=0;	//水平绘制
			else
				h0v1e2=1;	//竖直绘制
		}
	}
	else if(pPart->IsTube())
	{
		CLDSLineTube *pLineTube=(CLDSLineTube*)pPart;
		f3dPoint len_vec=pLineTube->End()-pLineTube->Start();
		normalize(len_vec);
		if(fabs(len_vec*axis_z)<0.707)
			h0v1e2=0;
		else
			h0v1e2=1;
	}
	else if(pPart->GetClassTypeId()==CLS_LINESLOT)
	{
		if(fabs(pPart->ucs.axis_z*axis_z)>0.707)
			h0v1e2=2;	//端面
		else if(fabs(pPart->ucs.axis_y*axis_z)>0.707)
			h0v1e2=0;	//平铺
		else 
			h0v1e2=1;	//侧面
	}
	else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
	{
		if(fabs(pPart->ucs.axis_x*axis_z)>0.707)
			h0v1e2=2;	//端面
		else if(fabs(pPart->ucs.axis_z*axis_z)>0.707)
			h0v1e2=0;	//平铺
		else
			h0v1e2=1;
	}
	/*else if(pPart->GetClassTypeId()==CLS_BOLT)
	{
		CLsListDraw boltDrawList;
		CLsDrawUnit *pDrawUnit=boltDrawList.AppendLsRef((CLDSBolt*)pPart,TRUE,axis_z);
		h0v1e2=pDrawUnit->front0_side1;
	}*/
	return h0v1e2;
}
//////////////////////////////////////////////////////////////////////////
//SECTION_SEG
//////////////////////////////////////////////////////////////////////////
SECTION_SEG::SECTION_SEG()
{
	m_cSectionType=0;
	m_pStartPart=m_pEndPart=m_pDatumPart=NULL;
}
BOOL SECTION_SEG::IsIncludeSectionPart(SECTION_PART *pPart)
{	
	if(m_pStartPart==NULL||m_pEndPart==NULL)
		return TRUE;
	//1.两线段重叠
	if(pPart->prjLine.endPt.z>m_pStartPart->prjLine.startPt.z&&pPart->prjLine.startPt.z<=m_pEndPart->prjLine.endPt.z)	
		return TRUE;
	//2.与终端构件间距大于节点区间间距阈值
	else if(pPart->prjLine.startPt.z>m_pEndPart->prjLine.endPt.z+SECTION_SPACE_THRESHOLD)
		return FALSE;
	//3.与终端构件间距小于等于节点区间间距阈值
	else 
		return TRUE;
}
int SECTION_SEG::StatSectionPartCount(BOOL bOnlyNoneToView/*=TRUE*/)
{	//主要用于统计沿未处理（在相应截面视图中表达）的构件数量
	int iNum=0;
	for(SECTION_PART* pSectionPart=sectionPartList.GetFirst();pSectionPart;pSectionPart=sectionPartList.GetNext())
	{
		if(!pSectionPart->m_bAddToView||!bOnlyNoneToView)
			iNum++;
	}
	return iNum;
}
#if defined( __LDS_CONTEXT_)||defined(__DRAWING_CONTEXT_)
//////////////////////////////////////////////////////////////////////////
//CLayerUnit
//////////////////////////////////////////////////////////////////////////
CLayerUnit::CLayerUnit()
{
	m_bNeed=TRUE;
}
BOOL CLayerUnit::IsOverLapped(CLayerUnit* pUnit,double DECISION/*=1*/)
{
	if(pUnit==NULL)
			return FALSE;
		else if(this==pUnit)
			return TRUE;
		/*根据板A与板B在绘图坐标系下重叠面积△计算结果，进行分析：
		 △<=DECISION，则板A与板B归于同一视图
		 △=A=B,板B则不需要新建视图
		 △>DECISION,则板B另建视图*/
		for(CIRPLATE_ITEM* pItem=m_xPartSet.GetFirst();pItem;pItem=m_xPartSet.GetNext())
		{
			for(CIRPLATE_ITEM* pTempItem=pUnit->m_xPartSet.GetFirst();pTempItem;pTempItem=pUnit->m_xPartSet.GetNext())
			{
				int nNum=pTempItem->m_xRegion.GetVertexCount();
				DYN_ARRAY<GEPOINT> vertexArr(nNum);
				for(int i=0;i<pTempItem->m_xRegion.GetVertexCount();i++)
					vertexArr[i]=pTempItem->m_xRegion.GetVertexAt(i);
				double fOverlappedArea=pItem->m_xRegion.OverlappedWith(vertexArr,nNum,NULL,DECISION);
				//double fOverlappedArea2=pItem->m_xRegion.OverlappedWith(vertexArr,nNum,NULL,DECISION*0.5);
				//double fOverlappedArea3=pItem->m_xRegion.OverlappedWith(vertexArr,nNum,NULL,DECISION*0.25);
				//double fOverlappedArea4=pItem->m_xRegion.OverlappedWith(vertexArr,nNum,NULL,DECISION*0.125);
				if(fOverlappedArea>DECISION)
				{
					double L=pItem->m_xRegion.CalPerimeter();	//周长
					double fArea1=pItem->m_xRegion.CalArea();
					double fScale1=fOverlappedArea/(fabs(fArea1)-L*DECISION);	//判断时需扣除边周长时固有的矩齿形计算误差影响的面积后95% wjh-2017.11.23
					double fArea2=pTempItem->m_xRegion.CalArea();
					double fScale2=fOverlappedArea/(fabs(fArea2)-L*DECISION);
					if(fScale1<0.01||fScale2<0.01)
						return FALSE;		//重叠面积很小，默认没有重叠
					if(pUnit->m_xPartSet.GetNodeNum()==1&&fabs(fArea1-fArea2)<EPS2&&fScale1>0.95)
						pUnit->m_bNeed=FALSE;	//完全重叠不需要新建视图
					return TRUE;
				}
			}
		}
		return FALSE;
}
BOOL CLayerUnit::IsCirCoverPlateLayer()
{
	for(CIRPLATE_ITEM* pItem=m_xPartSet.GetFirst();pItem;pItem=m_xPartSet.GetNext())
	{
		if(pItem->m_pPart->GetClassTypeId()!=CLS_PARAMPLATE)
			return FALSE;
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pItem->m_pPart;
		if(pParamPlate->m_iParamType!=TYPE_CIRCOVERPLATE)
			return FALSE;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
//CLayerGroup
//////////////////////////////////////////////////////////////////////////
CLayerGroup::CLayerGroup()
{
	m_bFinish=FALSE;
}
BOOL CLayerGroup::IsOverlapped(CLayerUnit* pOtherUnit)
{
	for(CLayerUnit** ppUnit=m_xLayerUnitPtrSet.GetFirst();ppUnit;ppUnit=m_xLayerUnitPtrSet.GetNext())
	{
		if((*ppUnit)->IsOverLapped(pOtherUnit))
			return TRUE;
	}
	return FALSE;
}
BOOL CLayerGroup::IsCirCoverPlateGroup()
{
	if(m_xLayerUnitPtrSet.GetNodeNum()!=1)
		return FALSE;
	CLayerUnit** ppUnit=m_xLayerUnitPtrSet.GetFirst();
	return (*ppUnit)->IsCirCoverPlateLayer();
}
BOOL CLayerGroup::GetGroupName(CXhChar50& sName)
{
	CLayerUnit** ppUnit=m_xLayerUnitPtrSet.GetFirst();
	CIRPLATE_ITEM* pItem=(*ppUnit)->m_xPartSet.GetFirst();
	if(pItem && pItem->m_pPart)
		sName=pItem->m_pPart->GetPartNo();
	return sName.GetLength()>0;
}
#endif
//////////////////////////////////////////////////////////////////////////
//CPartGroupWorkPlane
//////////////////////////////////////////////////////////////////////////
CPartGroupWorkPlane::CPartGroupWorkPlane()
{
	iNo=0;
	bJointTubePlane=false;
	nRayTubeNum=nPartNum=0;
	csParaInfo.m_cDefStyle=CParaCS::CS_PARAM;
	csParaInfo.m_eDatumPlane=CParaCS::DP_XY;
	pTempSectionParts=NULL;
	iMatchView=0;
}
f3dPoint CPartGroupWorkPlane::CalYAxisVec(CLDSLineTube* pRootTube)
{
	CParaCS des_cs;
	des_cs.SetParamInfo(csParaInfo);
	des_cs.SetModifiedState();
	des_cs.UpdateACS(pDatumPart->BelongModel());
	CLDSPart* pPart=pDatumPart->BelongModel()->FromPartHandle(csParaInfo.desAxisZ.hVicePart);
	GEPOINT vSimilarAxisY;
	if(pDatumPart->IsLinePart()&&pRootTube)
	{
		CLDSLinePart* pRod=(CLDSLinePart*)pDatumPart;
		GEPOINT lenStdRootVec=pRootTube->End()-pRootTube->Start();
		normalize(lenStdRootVec);
		GEPOINT lenBranchVec=pRod->End()-pRod->Start();
		GEPOINT lenVecStart=pRod->Start()-pRootTube->Start();
		GEPOINT lenVecEnd=pRod->End()-pRootTube->Start();
		if(fabs(lenVecStart*lenStdRootVec)>fabs(lenVecEnd*lenStdRootVec))
			lenBranchVec=pRod->Start()-pRod->End();	//始端点距离钢管更远
		vSimilarAxisY=lenBranchVec;

	}
	else if(!pDatumPart->IsPlate()||((CLDSGeneralPlate*)pDatumPart)->vertex_list.GetNodeNum()==0)
		return vTempAxisY=des_cs.GetACS().axis_y;
	else
	{
		CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pDatumPart;
		if(pRootTube&&pPlate->m_hPartWeldParent==pRootTube->handle)
			vSimilarAxisY=pRootTube->GetPlateVertSpreadVec(pPlate);
		else
		{
			GEPOINT pick_y=pPlate->vertex_list.GetFirst()->vertex;
			coord_trans(pick_y,pPlate->ucs,TRUE);
			vSimilarAxisY=pick_y-des_cs.GetACS().origin;
		}
	}
	if(vSimilarAxisY*des_cs.GetACS().axis_y<0)
		return vTempAxisY=-des_cs.GetACS().axis_y;
	else
		return vTempAxisY=des_cs.GetACS().axis_y;
}
////////////////////////////////////////////////////////////////////////////////
//CDrawCoordTrans
////////////////////////////////////////////////////////////////////////////////

CDrawCoordTrans::CDrawCoordTrans()
{
	LoadDefaultUCS(&cs);
	m_fDrawScale=1.0;
	m_fSpreadAngle=second.fRotateAngle=0;
}
CDrawCoordTrans::CDrawCoordTrans(UCS_STRU& dcs,double drawScale/*=1.0*/)
{
	m_fDrawScale=drawScale;
	m_fSpreadAngle=second.fRotateAngle=0;
	SetCS(dcs);
}
f3dPoint CDrawCoordTrans::PrjNorm() const
{
	if(fabs(m_fSpreadAngle)>EPS)
		return m_xSpreadPlaneNorm;
	else
		return _cs.axis_z;
}
f3dPoint CDrawCoordTrans::PrjOrigin() const
{
	if(fabs(m_fSpreadAngle)>EPS)	//展开面上构件以展开线起点为投影面基点
		return m_xSpreadLine.startPt;
	else
		return _cs.origin;
}

//设定当前绘图转换坐标系
bool CDrawCoordTrans::SetCS(UCS_STRU& ucs)
{
	_cs=ucs;
	if(!normalize(_cs.axis_x))
		return false;
	if(!normalize(_cs.axis_y))
		return false;
	if(!normalize(_cs.axis_z))
		return false;
	return true;
}
//srcPos为展开前塔坐标系下的坐标,返回绘图坐标(即ACAD下的坐标)
f3dPoint CDrawCoordTrans::TransToDraw(f3dPoint srcPos,bool clearCoordZ/*=true*/)
{
	if(fabs(m_fSpreadAngle)>EPS)	//需要先展开到绘图基面上
		rotate_point_around_axis(srcPos,m_fSpreadAngle,m_xSpreadLine);
	if(fabs(second.fRotateAngle)>EPS)
		rotate_point_around_axis(srcPos,second.fRotateAngle,second.xRotAxisStart,second.xRotAxisEnd);
	coord_trans(srcPos,cs,FALSE,TRUE);
	if(m_fDrawScale!=1.0&&fabs(m_fDrawScale)>EPS)
	{
		srcPos.x/=m_fDrawScale;
		srcPos.y/=m_fDrawScale;
		if(clearCoordZ)
			srcPos.z=0;
		else
			srcPos.z/=m_fDrawScale;
	}
	else if(clearCoordZ)
		srcPos.z=0;
	return srcPos;
}
//srcVec为展开前塔坐标系下的矢量,返回绘图矢量(即ACAD下的坐标)
f3dPoint CDrawCoordTrans::TransVToDraw(f3dPoint srcVec)
{
	if(fabs(m_fSpreadAngle)>EPS)	//需要先展开到绘图基面上
	{
		f3dPoint axis=m_xSpreadLine.endPt-m_xSpreadLine.startPt;
		RotateVectorAroundVector(srcVec,m_fSpreadAngle,axis);
	}
	if(fabs(second.fRotateAngle)>EPS)	//需要先展开到绘图基面上
	{
		f3dPoint axis=second.xRotAxisEnd-second.xRotAxisStart;
		RotateVectorAroundVector(srcVec,second.fRotateAngle,axis);
	}
	vector_trans(srcVec,cs,FALSE,TRUE);
	srcVec.z=0;
	return srcVec;
}
//srcVec为绘图坐标系下的矢量,返回塔模型坐标系下矢量
f3dPoint CDrawCoordTrans::TransVFromDraw(const double* drvVecCoord)
{
	GEPOINT drvVec(drvVecCoord);
	vector_trans(drvVec,cs,TRUE,TRUE);
	if(fabs(second.fRotateAngle)>EPS)	//需要先展开到绘图基面上
	{
		f3dPoint axis=second.xRotAxisEnd-second.xRotAxisStart;
		RotateVectorAroundVector(drvVec,-second.fRotateAngle,axis);
	}
	if(fabs(m_fSpreadAngle)>EPS)	//需要先展开到绘图基面上
	{
		f3dPoint axis=m_xSpreadLine.endPt-m_xSpreadLine.startPt;
		RotateVectorAroundVector(drvVec,-m_fSpreadAngle,axis);
	}
	return drvVec;
}
bool CSectionView::GetDCS(long hPart,CDrawCoordTrans &dcs)
{
	dcs.SetCS(ucs);
	CSectionView::PART_ITEM* pItem=NULL;
	if((pItem=SonParts.GetValue(hPart))==NULL||m_pPartGroup==NULL)
		return false;
	SmartPartPtr pCurPart=LoadTagInfo(pItem)->pCurPart;
	if(LoadTagInfo(pItem)->pCurPart==NULL)
		pCurPart=LoadTagInfo(pItem)->pCurPart=m_pPartGroup->BelongModel()->FromPartHandle(hPart);
	if(m_idViewType==CSectionView::CLS_JOINT_TUBES_VIEW)
	{
		CJointTubesView* pJointView=(CJointTubesView*)this;
		CLDSLineTube* pRootTube =(CLDSLineTube*)m_pPartGroup->BelongModel()->FromPartHandle(pJointView->m_hRootTube ,CLS_LINETUBE);
		CLDSLineTube* pJointTube=(CLDSLineTube*)m_pPartGroup->BelongModel()->FromPartHandle(pJointView->m_hJointTube,CLS_LINETUBE);
		if(pJointView->m_ciJointsViewType>0)
		{
			GEPOINT lenRootVec,lenJointVec,xJointPosition;
			//if(hPart==pJointView->m_hJointTube)
			{
				lenRootVec =pRootTube->End() - pRootTube->Start();
				if(pRootTube->pEnd==pJointTube->pStart||pRootTube->pEnd==pJointTube->pEnd)
					lenRootVec*=-1;	//终端对接
				normalize(lenRootVec);
				lenJointVec=pJointTube->End()-pJointTube->Start();
				xJointPosition=pJointTube->Start();
				if(pJointTube->pEnd==pRootTube->pStart||pJointTube->pEnd==pRootTube->pEnd)
				{
					xJointPosition=pJointTube->End();
					lenJointVec*=-1;	//终端对接
				}
				GECS rotcs;
				rotcs.axis_z=lenRootVec;
				if(pJointView->m_ciJointsViewType==1)
				{	//平视图
					rotcs.axis_x=ucs.axis_z;
					rotcs.axis_y=rotcs.axis_z^rotcs.axis_x;
				}
				else if(pJointView->m_ciJointsViewType==2)
				{	//塔身正侧面视图
					rotcs.axis_y=ucs.axis_z;
					rotcs.axis_x=rotcs.axis_y^rotcs.axis_z;
				}
				//TODO:规范化旋转坐标系
				GEPOINT lenPrjJointVec=rotcs.TransVToCS(lenJointVec);
				lenPrjJointVec.z=0;
				normalize(lenPrjJointVec);
				double rotRadAngle=ACOS(lenPrjJointVec.x);
				if(lenPrjJointVec.x<0)
					rotRadAngle=Pi-rotRadAngle;
				if(-lenPrjJointVec.y*lenPrjJointVec.x<0)
					rotRadAngle*=-1;	//沿lenRotVec方向的旋转展开角
				dcs.m_xSpreadLine.startPt=pRootTube->Start();
				dcs.m_xSpreadLine.endPt +=dcs.m_xSpreadLine.startPt+lenRootVec*100;
				dcs.m_fSpreadAngle=rotRadAngle;
				dcs.m_xSpreadPlaneNorm=ucs.axis_z;
				RotateVectorAroundVector(dcs.m_xSpreadPlaneNorm,-rotRadAngle,lenRootVec);
			}
			if(pCurPart->m_hPartWeldParent==pJointView->m_hJointTube)
			{
				if(pCurPart->GetClassTypeId()==CLS_PLATE)
				{
					normalize(lenJointVec);
					GEPOINT vPlateNormal=pCurPart.pPlate->ucs.axis_z;
					double justify,justify2;
					justify=vPlateNormal*lenJointVec;
					if(pCurPart.pPlate->designInfo.iFaceType==6&&pCurPart.pPlate->face_N>1)
					{	//变电构架折叠板
						for(int i=1;i<pCurPart.pPlate->face_N;i++)
						{
							GEPOINT vBendPlaneNormal=pCurPart.pPlate->GetHuoquFaceNorm(i-1);
							justify2=vBendPlaneNormal*lenJointVec;
							if(fabs(justify2)<fabs(justify))
							{
								justify=justify2;
								vPlateNormal=vBendPlaneNormal;
							}
						}
					}
					RotateVectorAroundVector(vPlateNormal,dcs.m_fSpreadAngle,lenRootVec);
					rotate_point_around_axis(xJointPosition,dcs.m_fSpreadAngle,dcs.m_xSpreadLine.startPt,dcs.m_xSpreadLine.endPt);
					RotateVectorAroundVector(lenJointVec,dcs.m_fSpreadAngle,lenRootVec);
					GEPOINT prjAxisZ=dcs.cs.axis_z-(dcs.cs.axis_z*lenJointVec)*lenJointVec;
					normalize(prjAxisZ);
					justify=vPlateNormal*prjAxisZ;
					if(fabs(justify)>EPS_COS)	//钢板在当前绘制平面上
						dcs.second.fRotateAngle=0;
					else if(fabs(justify)<0.707)//竖直绘制，或不需要绘制的钢板，因此不需要展开
						dcs.second.fRotateAngle=0;
					else	//需要展开绘制的钢板
					{
						dcs.second.fRotateAngle=ACOS(fabs(justify));	//展开角必须是锐角，否则就转到另一侧了，负值需要通过展开轴方向来考虑
						dcs.second.vNormal=vPlateNormal;
						f3dPoint rot_axis_vec=dcs.second.vNormal^prjAxisZ;
						if(justify>0)	//
							rot_axis_vec/= rot_axis_vec.mod();	//单位化
						else
							rot_axis_vec/=-rot_axis_vec.mod();
						dcs.second.xRotAxisStart=xJointPosition;
						dcs.second.xRotAxisEnd=dcs.second.xRotAxisStart+rot_axis_vec;
					}
				}
				//else if(pCurPart->GetClassTypeId()==CLS_PARAMPLATE) 暂未实现参数化板
			}
			else if(hPart!=pJointView->m_hJointTube)
				dcs.m_fSpreadAngle=0;
		}
	}
	else if(IsDualTubeJointView())//m_idViewType==CSectionView::CLS_DUALTUBE_JOINT_VIEW||m_pSectionView->m_idViewType==CSectionView::CLS_HORIZ_BEAM_VIEW)
	{
		dcs.m_fSpreadAngle=0;
		//计算展开面信息
		CDrawingAssistant* pAssist=hashAssistPlanes.GetValue(pItem->iResidePlaneNo);
		if(pAssist!=NULL)
		{
			if(pAssist->m_nStyle==0&&pAssist->iResidePlaneNo==0)
			{
				pAssist->GetRotateAxisAndAngle(dcs.cs.axis_z,dcs.m_xSpreadLine,dcs.m_fSpreadAngle);
				dcs.m_xSpreadPlaneNorm=pAssist->m_xFaceNorm.vector;
			}
			else if(pAssist->m_nStyle==0)
			{
				GECS rotcs;
				GetSpreadPlaneRotCS(pAssist->iNo,&rotcs,m_pPartGroup->BelongModel());
				//合并绘图坐标系与dcs与旋转展开坐标系rotcs;
				coord_trans(rotcs.origin,dcs.cs,FALSE,TRUE);
				vector_trans(rotcs.axis_x,dcs.cs,FALSE,TRUE);
				vector_trans(rotcs.axis_y,dcs.cs,FALSE,TRUE);
				vector_trans(rotcs.axis_z,dcs.cs,FALSE,TRUE);
				ReverseUCS(&rotcs);
				dcs.SetCS(UCS_STRU(rotcs));
			}
			else if(pAssist->m_nStyle==3)
			{
				dcs.m_fSpreadAngle=pAssist->m_fRotRadAng;
				pAssist->m_xLineStart.UpdatePos(m_pPartGroup->BelongModel());
				pAssist->m_xFaceNorm.UpdateVector(m_pPartGroup->BelongModel());
				dcs.m_xSpreadLine.startPt=pAssist->m_xLineStart.Position();
				dcs.m_xSpreadLine.endPt=dcs.m_xSpreadLine.startPt+pAssist->m_xFaceNorm.vector*1000;//pAssist->m_xLineEnd.Position();
				dcs.m_xSpreadPlaneNorm=dcs.cs.axis_z;
				double sina=sin(dcs.m_fSpreadAngle);
				double cosa=SQRT(1-sina*sina);
				RotateVectorAroundVector(dcs.m_xSpreadPlaneNorm,-sina,cosa,pAssist->m_xFaceNorm.vector);
			}
		}
	}
	else
	{
		dcs.m_fSpreadAngle=0;
		//计算展开面信息
		CDrawingAssistant* pAssist=hashAssistPlanes.GetValue(pItem->iResidePlaneNo);
		if(pAssist)
		{
			pAssist->GetRotateAxisAndAngle(dcs.cs.axis_z,dcs.m_xSpreadLine,dcs.m_fSpreadAngle);
			dcs.m_xSpreadPlaneNorm=pAssist->m_xFaceNorm.vector;
		}
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//CSectionView
////////////////////////////////////////////////////////////////////////////////
static const DWORD HASHTABLESIZE = 500;
static const DWORD STATUSHASHTABLESIZE = 50;
CStringKeyHashTable<long> CSectionView::propHashtable;
CHashTable<DWORD> CSectionView::propStatusHashtable;
void CSectionView::InitViewPropHashtable()
{
	int id=1;
	CSectionView::propHashtable.CreateHashTable(HASHTABLESIZE);
	CSectionView::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	CSectionView::propHashtable.SetValueAt("basicInfo",id++);
	CSectionView::propHashtable.SetValueAt("sName",id++);
	CSectionView::propHashtable.SetValueAt("index",id++);
	CSectionView::propHashtable.SetValueAt("m_bLocked",id++);
	CSectionView::propHashtable.SetValueAt("m_iViewType",id++);
	//
	CSectionView::propHashtable.SetValueAt("assemblePos",id++);
	CSectionView::propHashtable.SetValueAt("des_cs.m_cDefStyle",id++);
	CSectionView::propHashtable.SetValueAt("des_cs.m_eDatumPlane",id++);
	CSectionView::propHashtable.SetValueAt("ucs",id++);
	CSectionView::propHashtable.SetValueAt("ucs.origin",id++);
	CSectionView::propHashtable.SetValueAt("ucs.origin.x",id++);
	CSectionView::propHashtable.SetValueAt("ucs.origin.y",id++);
	CSectionView::propHashtable.SetValueAt("ucs.origin.z",id++);
	CSectionView::propHashtable.SetValueAt("ucs.axis_x",id++);
	CSectionView::propHashtable.SetValueAt("ucs.axis_x.x",id++);
	CSectionView::propHashtable.SetValueAt("ucs.axis_x.y",id++);
	CSectionView::propHashtable.SetValueAt("ucs.axis_x.z",id++);
	CSectionView::propHashtable.SetValueAt("ucs.axis_y",id++);
	CSectionView::propHashtable.SetValueAt("ucs.axis_y.x",id++);
	CSectionView::propHashtable.SetValueAt("ucs.axis_y.y",id++);
	CSectionView::propHashtable.SetValueAt("ucs.axis_y.z",id++);
	CSectionView::propHashtable.SetValueAt("ucs.axis_z",id++);
	CSectionView::propHashtable.SetValueAt("ucs.axis_z.x",id++);
	CSectionView::propHashtable.SetValueAt("ucs.axis_z.y",id++);
	CSectionView::propHashtable.SetValueAt("ucs.axis_z.z",id++);
	//
	CSectionView::propHashtable.SetValueAt("m_idHostView",id++);
	CSectionView::propHashtable.SetValueAt("slaveViewPara",id++);
	CSectionView::propHashtable.SetValueAt("location",id++);		//宿主视图绘图坐标系下的剖面图符定位点坐标
	CSectionView::propHashtable.SetValueAt("location.x",id++);
	CSectionView::propHashtable.SetValueAt("location.y",id++);
	CSectionView::propHashtable.SetValueAt("location.z",id++);
	CSectionView::propHashtable.SetValueAt("direction",id++);		//宿主视图绘图坐标系下的剖面图符视线方向
	CSectionView::propHashtable.SetValueAt("direction.x",id++);
	CSectionView::propHashtable.SetValueAt("direction.y",id++);
	CSectionView::propHashtable.SetValueAt("direction.z",id++);
	CSectionView::propHashtable.SetValueAt("direct_offset",id++);	//剖面图符实际标注位置与定位点间沿视线方向的偏移距离
	CSectionView::propHashtable.SetValueAt("symbol_dist",id++);		//剖面图符(成对)间的距离
	CSectionView::propHashtable.SetValueAt("symbol",id++);		//剖面图符上的标注文字
}

long CSectionView::GetPropID(char *sProp,...)
{	//得到属性ID
	va_list ap;
	va_start(ap, sProp);
	CXhChar500 sRealProp;
	_vsnprintf((char*)sRealProp,500,sProp,ap);
	va_end(ap);
	long id;
	if(propHashtable.GetValueAt(sRealProp,id))
		return id;
	else
		return 0;
}

int CSectionView::GetPropStr(long id,char* propStr)
{
	char sText[100]="";
	
	if(propStr)
		strcpy(propStr,sText);
	return strlen(sText);
}

BOOL CSectionView::IsPropEqual(HASHOBJSET &selectObjs, long idProp)
{
	return TRUE;
}

int CSectionView::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CSectionView::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}

BOOL CSectionView::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CSectionView::propStatusHashtable.SetValueAt(id,dwStatus);
}

int CSectionView::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	char sText[100]="";
	if(GetPropID("basicInfo")==id)
		strcpy(sText,"");
	else if(GetPropID("sName")==id)
		strcpy(sText,sName);
	else if(GetPropID("index")==id)
		sprintf(sText,"%d",index);
	else if(GetPropID("m_bLocked")==id)
	{
		if(m_bLocked)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else 
			strcpy(sText,"No");
#else
			strcpy(sText,"是");
		else 
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_iViewType")==id)
	{
		if(m_iViewType==VIEW_MAIN)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"1.Main View");
		else if(m_iViewType==VIEW_FRONT)
			strcpy(sText,"2.Front View");
		else if(m_iViewType==VIEW_NODEPLATE)
			strcpy(sText,"3.Node Plate View");
		else if(m_iViewType==VIEW_CIRCOVERPLATE)
			strcpy(sText,"4.Circular Over Plate View");
		else if(m_iViewType==VIEW_CIRRIBPLATE)
			strcpy(sText,"5.Circular Rib Plate View");
		else if(m_iViewType==VIEW_TUBERIBPLATE)
			strcpy(sText,"6.Across Linking Plate View");
		else if(m_iViewType==VIEW_FL)
			strcpy(sText,"7.Flange View");
		else if(m_iViewType==VIEW_CIRPLATE)
			strcpy(sText,"8.Circular Plate View");
		else if(m_iViewType==VIEW_INSERTPLATE)
			strcpy(sText,"9.Insert Plate View");
		else if(m_iViewType==VIEW_FOOTNAILANGLE)
			strcpy(sText,"10.Foot Nail Angle View");
		else if(m_iViewType==VIEW_FOOTNAILSLOT)
			strcpy(sText,"11.Foot Nail Slot Plate View");
		else if(m_iViewType==VIEW_FOOT)
			strcpy(sText,"12.Foot Plate View");
		else if(m_iViewType==VIEW_JOINSECTION)
			strcpy(sText,"13.Joint Section View");
		else if(m_iViewType==VIEW_JOINTTUBE)
			strcpy(sText,"14.Relative Joint Tube View");
		else if(m_iViewType==VIEW_KNODESKETCH)
			strcpy(sText,"15.K Node Intersecting View");
		else //if(m_iViewType==VIEW_USERDEF)
			strcpy(sText,"100.User-defined View");
#else
			strcpy(sText,"1.主视图");
		else if(m_iViewType==VIEW_FRONT)
			strcpy(sText,"2.正视图");
		else if(m_iViewType==VIEW_NODEPLATE)
			strcpy(sText,"3.节点板端视图");
		else if(m_iViewType==VIEW_CIRCOVERPLATE)
			strcpy(sText,"4.环向封板视图");
		else if(m_iViewType==VIEW_CIRRIBPLATE)
			strcpy(sText,"5.环向肋板视图");
		else if(m_iViewType==VIEW_TUBERIBPLATE)
			strcpy(sText,"6.跨越连板视图");
		else if(m_iViewType==VIEW_FL)
			strcpy(sText,"7.法兰视图");
		else if(m_iViewType==VIEW_CIRPLATE)
			strcpy(sText,"8.环向板视图");
		else if(m_iViewType==VIEW_INSERTPLATE)
			strcpy(sText,"9.插板视图");
		else if(m_iViewType==VIEW_FOOTNAILANGLE)
			strcpy(sText,"10.角钢脚钉");
		else if(m_iViewType==VIEW_FOOTNAILSLOT)
			strcpy(sText,"11.槽钢脚钉板");
		else if(m_iViewType==VIEW_FOOT)
			strcpy(sText,"12.塔脚板");
		else if(m_iViewType==VIEW_JOINSECTION)
			strcpy(sText,"13.相贯截面图");
		else if(m_iViewType==VIEW_JOINTTUBE)
			strcpy(sText,"14.对接钢管简图");
		else if(m_iViewType==VIEW_KNODESKETCH)
			strcpy(sText,"15.K节点相贯简图");
		else //if(m_iViewType==VIEW_USERDEF)
			strcpy(sText,"100.用户定义视图");
#endif
	}
	//
	else if(GetPropID("des_cs.m_cDefStyle")==id)
	{
		if(des_cs.GetParamInfo().m_cDefStyle==1)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"1.Mirror");
		else
			strcpy(sText,"0.Parametric Define");	
#else
			strcpy(sText,"1.对称");
		else
			strcpy(sText,"0.参数化定义");	
#endif
	}
	else if(GetPropID("des_cs.m_eDatumPlane")==id)
	{
		CParaCS::PARAMETRIC_INFO param=des_cs.GetParamInfo();
		if(param.m_eDatumPlane==CParaCS::DP_XY)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.X-Y Plane");
		else if(param.m_eDatumPlane==CParaCS::DP_YX)
			strcpy(sText,"1.Y-X Plane");
		else if(param.m_eDatumPlane==CParaCS::DP_YZ)
			strcpy(sText,"2.Y-Z Plane");
		else if(param.m_eDatumPlane==CParaCS::DP_ZY)
			strcpy(sText,"3.Z-Y Plane");
		else if(param.m_eDatumPlane==CParaCS::DP_ZX)
			strcpy(sText,"4.Z-X Plane");
		else if(param.m_eDatumPlane==CParaCS::DP_XZ)
			strcpy(sText,"5.X-Z Plane");
#else
			strcpy(sText,"0.X-Y面");
		else if(param.m_eDatumPlane==CParaCS::DP_YX)
			strcpy(sText,"1.Y-X面");
		else if(param.m_eDatumPlane==CParaCS::DP_YZ)
			strcpy(sText,"2.Y-Z面");
		else if(param.m_eDatumPlane==CParaCS::DP_ZY)
			strcpy(sText,"3.Z-Y面");
		else if(param.m_eDatumPlane==CParaCS::DP_ZX)
			strcpy(sText,"4.Z-X面");
		else if(param.m_eDatumPlane==CParaCS::DP_XZ)
			strcpy(sText,"5.X-Z面");
#endif
	}
	else if(GetPropID("ucs.origin.x")==id)
	{
		sprintf(sText,"%f",ucs.origin.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.y")==id)
	{
		sprintf(sText,"%f",ucs.origin.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.z")==id)
	{
		sprintf(sText,"%f",ucs.origin.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x")==id)
		strcpy(sText,des_cs.GetAxisDescription('X'));
	else if(GetPropID("ucs.axis_x.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y")==id)
		strcpy(sText,des_cs.GetAxisDescription('Y'));
	else if(GetPropID("ucs.axis_y.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z")==id)
		strcpy(sText,des_cs.GetAxisDescription('Z'));
	else if(GetPropID("ucs.axis_z.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.z);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}

IMPLEMENT_GET(CSectionView::PART_ITEM,bSpreadOverRidge){
	return this->_biSpreadOverRidge;
}
IMPLEMENT_SET(CSectionView::PART_ITEM,bSpreadOverRidge){
	this->_biSpreadOverRidge=value;
}
//展开后的目标方位1.主管上侧2.主管下侧3.主管前面4.主管背面
IMPLEMENT_GET(CSectionView::PART_ITEM,ciSpreadDirection){
	return this->_ciSpreadDirection;
}
IMPLEMENT_SET(CSectionView::PART_ITEM,ciSpreadDirection){
	this->_ciSpreadDirection=value;
}
//0.表示视图基面,其余>0值表示该视图内相应的展开面;;-1表示展开至主管下面,-2表示展至主管上面,-3表示展开至主管前面,-4表示展开至主管背面 wjh-2017.11.25
IMPLEMENT_GET(CSectionView::PART_ITEM,iResidePlaneNo){
	return this->_wiResidePlane;
}
IMPLEMENT_SET(CSectionView::PART_ITEM,iResidePlaneNo){
	this->_wiResidePlane=(WORD)value;
}
void LoadAssistPlane(CDrawingAssistant* pObj,DWORD key){pObj->iNo=key; }
IMPLEMENT_GET(CSectionView,m_iViewType){return _iLegacyViewType;}
IMPLEMENT_SET(CSectionView,m_iViewType){_iLegacyViewType=value;}
IMPLEMENT_GET(CSectionView,pSlaveView){return IsSlaveView() ? (CSlaveSectionView*)this : NULL;}
CSectionView::CSectionView(int id)
{
	m_id=id;
	_idViewType=CLS_BASIC_VIEW;
	strcpy(sName,"");
	//strcpy(sPartNo,"");
	m_bLocked=FALSE;
	feature=0;
	index=0;
	m_fOrgLenOffset=0.0;
	LoadDefaultUCS(&ucs);
	_iLegacyViewType=VIEW_USERDEF;
	m_pPartGroup=NULL;
	hashAssistPlanes.LoadDefaultObjectInfo=LoadAssistPlane;
}
const char* CSectionView::get_internalSymbol(){
	static char emptyStr[3]={0,0,0};
	return emptyStr;//不直接返回""是为了防止,其余地方误调用Symbol[1]这样的事情导致内存溢出 wjh-2016.11.17
}
CSectionView::ITEM_TAG_INFO::ITEM_TAG_INFO()
{
	m_pAttachObj=NULL;
	cDrawMode=0;
	pCurPart=NULL;
}
/*void CSectionView::ITEM_TAG_INFO::AttachObject(void* pObj)
{
	m_pAttachObj=pObj;
	((PART_ITEM*)pObj)->pTagInfo=this;
}
void CSectionView::ITEM_TAG_INFO::DetachObject()
{
	if(m_pAttachObj&&((PART_ITEM*)m_pAttachObj)->pTagInfo==this)
		((PART_ITEM*)m_pAttachObj)->pTagInfo=NULL;
	m_pAttachObj=NULL;
}*/
//当PART_ITEM已经有挂接对象时执行TagInfo(...), 否则创建新挂接数据对象后返回
CSectionView::ITEM_TAG_INFO* CSectionView::LoadTagInfo(PART_ITEM* pItem)
{
	if(pItem->pTagInfo!=NULL)
		return (ITEM_TAG_INFO*)pItem->pTagInfo;
	ITEM_TAG_INFO* pTagInfo=listTagInfo.append();
	pItem->pTagInfo=pTagInfo;
	pTagInfo->AttachObject(pItem);
	return pTagInfo;
}
CSectionView::ITEM_TAG_INFO* TagInfo(CSectionView::PART_ITEM* pItem)
{
	return (CSectionView::ITEM_TAG_INFO*)pItem->pTagInfo;
}
CLDSPart* TagPart(CSectionView::PART_ITEM* pItem)
{
	if(pItem->pTagInfo!=NULL)
		return ((CSectionView::ITEM_TAG_INFO*)pItem->pTagInfo)->pCurPart;
	return NULL;
}

//添加默认的标定点
void CSectionView::AddDefaultLocatePoints(CLDSPart *pDatumPart,IModel *pTower)
{
	int i,j;
	if(pDatumPart==NULL||pDatumPart->GetClassTypeId()!=CLS_LINETUBE||(m_iViewType!=VIEW_MAIN
		&&m_iViewType!=VIEW_FRONT&&m_iViewType!=VIEW_USERDEF))
		return;	//目前只处理基准构件为钢管时的定位点添加
	CSectionView::PART_ITEM *pPartItem = NULL;
	CPartGroupLocatePoint *pLocatePoint=NULL;
	CLDSLineTube *pLineTube=(CLDSLineTube*)pDatumPart;
	if((pLineTube->m_tJointStart.type==1&&pLineTube->m_tJointStart.hViceLinkObj<0)||
		(pLineTube->m_tJointEnd.type==1&&pLineTube->m_tJointEnd.hViceLinkObj<0))
		return;		//对接相贯组焊件的相应视图特殊处理
	UCS_STRU dcs;	//以基准构件为基准的绘图坐标系
	dcs.axis_z=pLineTube->End()-pLineTube->Start();
	normalize(dcs.axis_z);
	if(fabs(dcs.axis_z*ucs.axis_z)>EPS_COS)
		return;	//端面图不添加标定点
	f3dLine line,tube_line;
	tube_line.startPt=pLineTube->Start()-dcs.axis_z*pLineTube->startOdd();
	tube_line.endPt=pLineTube->End()+dcs.axis_z*pLineTube->endOdd();
	dcs.origin=tube_line.startPt;
	bool reverseStartEnd=false;
	if(dcs.axis_z*ucs.axis_x<0)
	{
		dcs.axis_z*=-1;
		dcs.origin=tube_line.endPt;
		reverseStartEnd=true;
	}
	//添加钢管始终端标定点
	TUBEJOINT *pTubeJoint=NULL; 
	for(i=0;i<2;i++)
	{
		if(i==0)		//始端
			pTubeJoint=&pLineTube->m_tJointStart;
		else //if(i==1)	//终端
			pTubeJoint=&pLineTube->m_tJointEnd;
		//平面切割钢管后应该添加标注值 平面切割或对接相贯
		if((pTubeJoint->type==0||(pTubeJoint->type==1&&pTubeJoint->hLinkObj>0&&pTubeJoint->hViceLinkObj<0))
			&&!pTubeJoint->cutPlaneNorm.IsZero())
		{	//钢管始端被平面切割
			double inner_radius=pLineTube->GetDiameter()/2-pLineTube->GetThick();
			f3dPoint up_vec=pTubeJoint->cutPlaneNorm-(pTubeJoint->cutPlaneNorm*dcs.axis_z)*dcs.axis_z;
			/*f3dPoint d=(dcs.axis_z^pTubeJoint->cutPlaneNorm)^dcs.axis_z;
			normalize(d);*/
			normalize(up_vec);
			if(ucs.axis_y*up_vec<0)
				up_vec*=-1.0;
			f3dPoint inters_point;
			CPartGroupLocatePoint* locatePointArr[2];
			for(j=0;j<2;j++)
			{
				if(j==0)
				{
					line.startPt=tube_line.startPt+up_vec*inner_radius;
					line.endPt=tube_line.endPt+up_vec*inner_radius;
				}
				else
				{
					line.startPt=tube_line.startPt-up_vec*inner_radius;
					line.endPt=tube_line.endPt-up_vec*inner_radius;
				}
				Int3dlf(inters_point,line,pTubeJoint->cutPlanePos,pTubeJoint->cutPlaneNorm);
				locatePointArr[j]=LocatePoints.append();
				//locatePointArr[j]->m_hPart=0;
				locatePointArr[j]->m_fLenPosToDatumPart=dcs.axis_z*(inters_point-dcs.origin);
				locatePointArr[j]->m_bDimSpace=FALSE;
			}
			if(m_iViewType==VIEW_JOINTTUBE)
			{	//相贯钢管
				locatePointArr[0]->m_hPart=pLineTube->handle;
				locatePointArr[0]->m_fVertPosToDatumPart=0.5*pLineTube->GetDiameter();
				locatePointArr[0]->m_iOrderType= 2;	//局部尺寸标定点
				locatePointArr[1]->m_hPart=pLineTube->handle;
				locatePointArr[1]->m_fVertPosToDatumPart=-0.5*pLineTube->GetDiameter();
				locatePointArr[1]->m_iOrderType= -2;	//局部尺寸标定点
			}
			else
			{
				if(locatePointArr[1]->m_fLenPosToDatumPart>locatePointArr[0]->m_fLenPosToDatumPart)
				{
					locatePointArr[0]->m_iOrderType= 0;	//上侧为端面定位点
					locatePointArr[1]->m_iOrderType=-1;	//下侧为构件定位点
					if(i==(!reverseStartEnd?0:1))
						m_fOrgLenOffset=-locatePointArr[0]->m_fLenPosToDatumPart;
				}
				else
				{
					locatePointArr[0]->m_iOrderType= 1;	//上侧构件定位点
					locatePointArr[1]->m_iOrderType= 0;	//下侧端面定位点
					if(i==(!reverseStartEnd?0:1))
						m_fOrgLenOffset=-locatePointArr[1]->m_fLenPosToDatumPart;
				}
			}
		}
		else 
		{
			//将钢管的始/终端作为一个基准的标定点
			pLocatePoint=LocatePoints.append();
			f3dPoint locate_point;
			if(i==0)	//始端
				locate_point=tube_line.startPt;
			else		//终端
				locate_point=tube_line.endPt;
			pLocatePoint->m_fLenPosToDatumPart=dcs.axis_z*(locate_point-dcs.origin);
			pLocatePoint->m_iOrderType=0;	//钢管端面构件定位尺寸
			pLocatePoint->m_bDimSpace=FALSE;
			CLDSParamPlate *pEndLjPlate=(CLDSParamPlate*)pTower->FromPartHandle(pTubeJoint->hLinkObj,CLS_PARAMPLATE);
			if(pEndLjPlate==NULL)
				continue;
			pLocatePoint->m_hPart=pEndLjPlate->handle;
			if(m_iViewType==VIEW_JOINTTUBE)	
			{	//为标注对接相贯钢管上下两层里皮长度,在钢管端头上下两侧分别添加一个局部尺寸标定点 wht 10-08-11
				CPartGroupLocatePoint *pTubeEndPoint=LocatePoints.append();
				pTubeEndPoint->m_fLenPosToDatumPart=dcs.axis_z*(locate_point-dcs.origin);
				pTubeEndPoint->m_iOrderType=2;	//钢管端面构件定位尺寸
				pTubeEndPoint->m_bDimSpace=FALSE;
				pTubeEndPoint->m_hPart=pLineTube->handle;
				pTubeEndPoint->m_fVertPosToDatumPart=0.5*pLineTube->GetDiameter();
				//
				pTubeEndPoint=LocatePoints.append();
				pTubeEndPoint->m_fLenPosToDatumPart=dcs.axis_z*(locate_point-dcs.origin);
				pTubeEndPoint->m_iOrderType=-2;	//钢管端面构件定位尺寸
				pTubeEndPoint->m_bDimSpace=FALSE;
				pTubeEndPoint->m_hPart=pLineTube->handle;
				pTubeEndPoint->m_fVertPosToDatumPart=-0.5*pLineTube->GetDiameter();
			}
			if( pEndLjPlate&&(pEndLjPlate->IsFL()))
			{	//法兰 平焊法兰 对焊法兰
				pLocatePoint->m_iOrderType=2;	//钢管端面局部间隙尺寸
				pLocatePoint=LocatePoints.append();
				f3dPoint origin=pEndLjPlate->ucs.origin;
				if(i==(!reverseStartEnd?0:1))
					origin+=pEndLjPlate->GetOutterPlaneOffset(-dcs.axis_z)*pEndLjPlate->ucs.axis_z;
				else
					origin+=pEndLjPlate->GetOutterPlaneOffset( dcs.axis_z)*pEndLjPlate->ucs.axis_z;
				pLocatePoint->m_fLenPosToDatumPart=(origin-dcs.origin)*dcs.axis_z;
				pLocatePoint->m_iOrderType=0;	//端面定位点
				pLocatePoint->m_hPart=pEndLjPlate->handle;
				pLocatePoint->m_bDimSpace=FALSE;
				if(i==(!reverseStartEnd?0:1))
					m_fOrgLenOffset=-pLocatePoint->m_fLenPosToDatumPart;
			}
			else if(pEndLjPlate&&(pEndLjPlate->m_iParamType==TYPE_XEND||
				pEndLjPlate->m_iParamType==TYPE_UEND||pEndLjPlate->m_iParamType==TYPE_ROLLEND))
			{	//钢管始端连有插板 
				//在插板上的螺栓处插入标定点，显示侧面的插板也应该添加标定点
				PARTSET partset;
				CLDSPart *pPart=NULL;
				double L=1000000,max_x=-1000;		//将插板上X坐标最大的螺栓作为定位尺寸
				CLDSBolt *pBoltMaxX=NULL;
				pEndLjPlate->GetDesignItemValue('L',&L);
				for(CLsRef *pLsRef=pEndLjPlate->GetFirstLsRef();pLsRef;pLsRef=pEndLjPlate->GetNextLsRef())
				{
					f3dPoint temp_pos,ls_pos=(*pLsRef)->ucs.origin;
					coord_trans(ls_pos,pEndLjPlate->ucs,FALSE);
					if(pEndLjPlate->m_iParamType==TYPE_XEND&&ls_pos.x>L)
						continue;	//跳过不在插板基板长度范围内的螺栓 wht 10-08-06
					if((ls_pos.x-max_x)>EPS)	//应使用EPS进行比较
					{
						max_x=ls_pos.x;
						pBoltMaxX=pLsRef->GetLsPtr();
					}
					for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
					{
						temp_pos=pPart->ucs.origin;
						coord_trans(temp_pos,pEndLjPlate->ucs,FALSE);
						if(fabs(ls_pos.x-temp_pos.x)<EPS2)
							break;
					}
					if(pPart==NULL)
						partset.append(pLsRef->GetLsPtr());
				}
				//此处一定会有一个X坐标最大的螺栓，在此将钢管端面标定点修改为局部间隙尺寸标定点 wht 10-08-08
				pLocatePoint->m_iOrderType=2;	
				double width=pEndLjPlate->GetPrjDrawingWidth();
				for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
				{
					pLocatePoint=LocatePoints.append();
					pLocatePoint->m_hPart=pEndLjPlate->handle;
					pLocatePoint->m_fLenPosToDatumPart=(pPart->ucs.origin-dcs.origin)*dcs.axis_z;
					f3dPoint pt=pPart->ucs.origin;
					coord_trans(pt,pEndLjPlate->ucs,FALSE);
					if(pBoltMaxX&&(pBoltMaxX->handle==pPart->handle||fabs(pt.x-max_x)<EPS2))
					{
						pLocatePoint->m_iOrderType=0;	//端面定位点
						if(i==(!reverseStartEnd?0:1)) //绘图原点(始端插板X相对坐标最大螺栓)距离钢管原点的长度方向偏移 wht 10-08-08
							m_fOrgLenOffset=-pLocatePoint->m_fLenPosToDatumPart;
					}
					else
					{
						pLocatePoint->m_fVertPosToDatumPart=0.5*width+1; //局部间隙尺寸需要指定偏移距离
						pLocatePoint->m_iOrderType=2;	//局部间隙尺寸
					}
					pLocatePoint->m_hPart=pEndLjPlate->handle;
					pLocatePoint->m_bDimSpace=FALSE;
				}
				//插板原点处插入标定点
				pLocatePoint=LocatePoints.append();
				pLocatePoint->m_hPart=pEndLjPlate->handle;
				pLocatePoint->m_fLenPosToDatumPart=(pEndLjPlate->ucs.origin-dcs.origin)*dcs.axis_z;
				pLocatePoint->m_fVertPosToDatumPart=0.5*width+1;
				pLocatePoint->m_iOrderType=2;	//局部间隙尺寸		
				pLocatePoint->m_bDimSpace=FALSE;
			}
		}
	}
	for(pPartItem=SonParts.GetFirst();pPartItem;pPartItem=SonParts.GetNext())
	{
		CSuperSmartPtr<CLDSPart> pPart=pTower->FromPartHandle(pPartItem->hSonPart);
		if(pPart.IsNULL())
			continue;
		BOOL bNeedJudgeOrder=FALSE;			//是否需要区分上下侧标定点 wht 10-08-16
		CPartGroupLocatePoint locate_point;	//locate_point 此处要求必须有基准构件
		CLDSPart *pLocatePointPart=NULL;
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{	//添加普通钢板标定点
			bNeedJudgeOrder=TRUE;
			CLDSNode *pNode=pTower->FromNodeHandle(pPart.PlatePointer()->designInfo.m_hBaseNode);
			if(pNode==NULL)
				continue;
			if(fabs(pPart->ucs.axis_z*ucs.axis_z)>0.707)	//小于45°
			{
				locate_point.m_hPart=pPart->handle;
				double locate_point_y=0;//对活点Y坐标
				PROFILE_VERTEX *pPrevPnt=pPart.PlatePointer()->vertex_list.GetTail();
				for(PROFILE_VERTEX *pVertex=pPart.PlatePointer()->vertex_list.GetFirst();pVertex;pVertex=pPart.PlatePointer()->vertex_list.GetNext())
				{
					if(pPrevPnt->m_bWeldEdge)
					{
						if(pPrevPnt->local_point_vec==1||pPrevPnt->local_point_vec==2)
						{
							locate_point_y=pPrevPnt->local_point_y;
							break;
						}
					}
					pPrevPnt = pVertex;
				}
				f3dPoint vertex(0,locate_point_y,0);
				coord_trans(vertex,pPart->ucs,TRUE);
				locate_point.m_fLenPosToDatumPart=(vertex-dcs.origin)*dcs.axis_z;
				//大致判断板的位置 上/下
				vertex=pPart.PlatePointer()->vertex_list.GetFirst()->vertex;
				coord_trans(vertex,pPart->ucs,TRUE);
				vertex-=dcs.origin;
				if(vertex*ucs.axis_y>0)
					locate_point.m_iOrderType=1;	//构件定位尺寸
				else
					locate_point.m_iOrderType=-1;	//构件定位尺寸
				locate_point.m_bDimSpace=FALSE;
				pLocatePointPart=pPart.PlatePointer();
			}
		}
		else if(pPart->GetClassTypeId()==CLS_LINESLOT)
		{	//添加脚钉底座标定点
			if(!pPart.LineSlotPointer()->IsBaseSlot())//GetLsCount()!=1)
				continue;	
			long hLocatePart=pPart->handle;
			CLDSPart* pLocatePart=pPart;
			f3dPoint location=0.5*(pPart.pRod->Start()+pPart.pRod->End());
			if(pPart.pSlot->GetFirstLsRef()&&pPart->GetLsCount()==1)
			{
				CLDSBolt* pBolt=pPart.pSlot->GetFirstLsRef()->GetLsPtr();
				hLocatePart=pBolt->handle;
				location=pBolt->ucs.origin;
				pLocatePart=pBolt;
			}
			locate_point.m_hPart=hLocatePart;
			locate_point.m_xLocalCoordPos.Set();
			locate_point.m_fLenPosToDatumPart=(location-dcs.origin)*dcs.axis_z;
			locate_point.m_iOrderType=-3;	//脚钉定位尺寸
			locate_point.m_bDimSpace=FALSE;
			pLocatePointPart=pLocatePart;
		}
		else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{	//角钢脚钉
			if(!pPart.LineAnglePointer()->m_bFootNail)
				continue;
			pPart.LineAnglePointer()->getUCS(pPart->ucs);	//构建角钢坐标系
			locate_point.m_hPart=pPart->handle;
			locate_point.m_xLocalCoordPos.Set();
			locate_point.m_fLenPosToDatumPart=(pPart->ucs.origin-dcs.origin)*dcs.axis_z;
			locate_point.m_iOrderType=-3;	//脚钉底座尺寸
			locate_point.m_bDimSpace=FALSE;
			pLocatePointPart=pPart.LineAnglePointer();
		}
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{	//环向肋板
			if(pPart.ParamPlatePointer()->m_iParamType!=TYPE_CIRRIBPLATE)
				continue;
			locate_point.m_hPart=pPart->handle;
			locate_point.m_xLocalCoordPos.Set();
			locate_point.m_fLenPosToDatumPart=(pPart->ucs.origin-dcs.origin)*dcs.axis_z;
			locate_point.m_iOrderType=1;	//构件定位尺寸
			locate_point.m_bDimSpace=FALSE;
			pLocatePointPart=pPart.ParamPlatePointer();
		}
		else if(pPart->GetClassTypeId()==CLS_LINETUBE)
		{	//为钢管上的脚钉添加标定点 wht 12-08-30
			CLDSLineTube *pCurTube=pPart.LineTubePointer();
			for(CLsRef *pLsRef=pCurTube->GetFirstLsRef();pLsRef;pLsRef=pCurTube->GetNextLsRef())
			{
				if(pLsRef->GetLsPtr()==NULL||!pLsRef->GetLsPtr()->IsFootNail())
					continue;
				CPartGroupLocatePoint locatePt;
				locatePt.m_hPart=pLsRef->GetLsPtr()->handle;
				locatePt.m_xLocalCoordPos.Set();
				locatePt.m_fLenPosToDatumPart=(pLsRef->GetLsPtr()->ucs.origin-dcs.origin)*dcs.axis_z;
				locatePt.m_iOrderType=-3;	//脚钉定位尺寸
				locatePt.m_bDimSpace=FALSE;
				//避免在同一截面位置添加标定点
				for(pLocatePoint=LocatePoints.GetFirst();pLocatePoint;pLocatePoint=LocatePoints.GetNext())
				{
					if(pLocatePoint->m_fLenPosToDatumPart==locatePt.m_fLenPosToDatumPart)
					{
						if(!bNeedJudgeOrder)
							break;
						else if(pLocatePoint->m_iOrderType==locatePt.m_iOrderType)
							break;
					}
				}
				if(pLocatePoint==NULL)
					LocatePoints.append(locatePt);
			}
			//相贯支管 wht 10-08-08
			CLDSLineTube *pBranchTube=pPart.LineTubePointer(),*pMainTube=NULL,*pViceTube=NULL;
			f3dPoint branch_tube_vec;
			if(pBranchTube->m_tJointStart.type==1&&pBranchTube->m_tJointStart.hViceLinkObj>=0)
			{	//始端相贯(此处不处理对接相贯)
				if(pBranchTube->m_tJointStart.hLinkObj>0x20)
					pMainTube=(CLDSLineTube*)pTower->FromPartHandle(pBranchTube->m_tJointStart.hLinkObj,CLS_LINETUBE);
				if(pBranchTube->m_tJointStart.hViceLinkObj>0x20)
					pViceTube=(CLDSLineTube*)pTower->FromPartHandle(pBranchTube->m_tJointStart.hViceLinkObj,CLS_LINETUBE);
				branch_tube_vec=pBranchTube->End()-pBranchTube->Start();
			}
			else if(pBranchTube->m_tJointEnd.type==1&&pBranchTube->m_tJointEnd.hViceLinkObj>=0)
			{	//终端相贯(此处不处理对接相贯)
				if(pBranchTube->m_tJointEnd.hLinkObj>0x20)
					pMainTube=(CLDSLineTube*)pTower->FromPartHandle(pBranchTube->m_tJointEnd.hLinkObj,CLS_LINETUBE);
				if(pBranchTube->m_tJointEnd.hViceLinkObj>0x20)
					pViceTube=(CLDSLineTube*)pTower->FromPartHandle(pBranchTube->m_tJointEnd.hViceLinkObj,CLS_LINETUBE);
				branch_tube_vec=pBranchTube->Start()-pBranchTube->End();
			}
			normalize(branch_tube_vec);
			if(pMainTube==NULL)
				continue;
			if(pMainTube->handle!=pDatumPart->handle&&(pViceTube==NULL||
				(pViceTube&&pViceTube->handle!=pDatumPart->handle)))
				continue;
			f3dPoint inters_pos;
			int nRetCode=Int3dpl(f3dLine(pLineTube->Start(),pLineTube->End()),
				f3dLine(pBranchTube->Start(),pBranchTube->End()),inters_pos);
			if(nRetCode!=1)
				continue;
			locate_point.m_hPart=pLineTube->handle;
			locate_point.m_xLocalCoordPos.Set();
			locate_point.m_fLenPosToDatumPart=(inters_pos-dcs.origin)*dcs.axis_z;
			if(branch_tube_vec*ucs.axis_y>0)
				locate_point.m_iOrderType=1;	//构件定位尺寸
			else 
				locate_point.m_iOrderType=-1;	//构件定位尺寸
			locate_point.m_bDimSpace=FALSE;
			pLocatePointPart=pLineTube;
		}
		if(pLocatePointPart==NULL||locate_point.m_hPart<0x20)
			continue;	//无基准构件
		//避免在同一截面位置添加标定点
		for(pLocatePoint=LocatePoints.GetFirst();pLocatePoint;pLocatePoint=LocatePoints.GetNext())
		{
			if(fabs(pLocatePoint->m_fLenPosToDatumPart-locate_point.m_fLenPosToDatumPart)<1)
			{
				if(!bNeedJudgeOrder)
					break;
				else if(pLocatePoint->m_iOrderType==locate_point.m_iOrderType)
					break;
			}
		}
		if(pLocatePoint==NULL)
			LocatePoints.append(locate_point);
	}
}

//添加默认的角度标注线
class PARTSETOBJ : public PARTSET{
public:
	PARTSETOBJ(DWORD key){;}
};
void CSectionView::AddDefaultAngulars(CLDSPart *pDatumPart,IModel *pTower)
{
	if(pDatumPart==NULL)
		return;
	CSectionView::PART_ITEM *pPartItem = NULL;
	if(pDatumPart->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube *pLineTube=(CLDSLineTube*)pDatumPart;
		pLineTube->BuildUCS();
		if(m_iViewType==VIEW_MAIN||m_iViewType==VIEW_FRONT)
			return;
		if(fabs(pLineTube->ucs.axis_z*ucs.axis_z)<EPS2)
			return;	//主/正视图的角度标注在绘制板和支管时处理,故此处跳过
		CHashPtrList<PARTSETOBJ> hashNodePartset;
		PARTSET footNailSet;//,partset[10],	//变电构架有超过10个节点的可能 wjh-2016.11.18
		int i=0,j=-1,n=SonParts.GetNodeNum();//,nNodeNum=0
		CHashTable<int>partsetTbl;
		//partsetTbl.CreateHashTable(n);
		int nAngleNum=0;
		double angle[20];	//记录每个脚钉与基准边之间的夹角
		CLDSLineTube *pDatumRayTube=NULL;	//基准射线钢管	为支管视图添加角度标注
		f3dPoint datum_ray_tube_vec;		//基准射线钢管方向（视图坐标系下的相对方向）
		CLDSParamPlate *pXEndPlate=NULL;
		BOOL bIncludeDatumTube=FALSE;		//当前视图中是否包括基准钢管 
		for(pPartItem=SonParts.GetFirst();pPartItem;pPartItem=SonParts.GetNext())
		{
			if(pPartItem->hSonPart==pDatumPart->handle)
			{
				bIncludeDatumTube=TRUE;	
				break;
			}
		}
		CLDSLineTube *pJointTube=NULL;	//查找与基准钢管对接相贯的钢管 wht 10-08-12
		if(!bIncludeDatumTube)
		{
			for(pPartItem=SonParts.GetFirst();pPartItem;pPartItem=SonParts.GetNext())
			{	//统计连接有钢板的节点数目以及附带的钢板
				CLDSPart *pTempPart=pTower->FromPartHandle(pPartItem->hSonPart);
				if(pTempPart==NULL)
					continue;
				if(pTempPart->GetClassTypeId()!=CLS_LINETUBE)
					continue;
				CLDSLineTube *pTempTube=(CLDSLineTube*)pTempPart;
				if(pLineTube->m_tJointStart.hLinkObj!=pTempTube->handle
					&&pLineTube->m_tJointEnd.hLinkObj!=pTempTube->handle)
					continue;
				pJointTube=pTempTube;
				pJointTube->BuildUCS();
				break;
			}
		}
		for(pPartItem=SonParts.GetFirst();pPartItem;pPartItem=SonParts.GetNext())
		{	//统计连接有钢板的节点数目以及附带的钢板
			CLDSPlate *pPlate=NULL;
			CLDSPart *pPart=pTower->FromPartHandle(pPartItem->hSonPart);
			if(pPart==NULL)
				continue;
			if(pPart->GetClassTypeId()==CLS_LINESLOT)
			{	//将组焊件中与钢管基准边夹角不同的脚钉底座加入到脚钉底座列表中
				CLDSLineSlot *pLineSlot=(CLDSLineSlot*)pPart;
				if(pLineSlot==NULL)
					continue;
				pLineSlot->BuildUCS();
				if(!pLineSlot->IsBaseSlot())
					continue;	//非脚钉底座板
				/*if(pLineSlot->GetLsCount()!=1)
					continue;	//非脚钉底座板
				CLDSBolt *pBolt=pLineSlot->GetFirstLsRef()->GetLsPtr();
				if(pBolt==NULL)
					continue;	
				if(!pBolt->IsFootNail())
					continue;	//非脚钉底座板
				pBolt->GetUCS();
				f3dPoint ls_norm=pBolt->get_norm();
				*/
				f3dPoint ls_norm=pLineSlot->WorkPlaneNorm();
				//将螺栓法线方向转换到钢管坐标系下
				vector_trans(ls_norm,pLineTube->ucs,FALSE);
				double offset_angle=Cal2dLineAng(f2dPoint(0,0),f2dPoint(ls_norm.x,ls_norm.y))*DEGTORAD_COEF;
				BOOL bFlag=TRUE;
				for(i=0;i<nAngleNum;i++)
				{
					if(fabs(angle[i]-offset_angle)<EPS2) //误差0.03°
					{	//已存在该角度值
						bFlag=FALSE;
						break;
					}
				}
				if(bFlag)
				{	//列表没有相同的角度值
					angle[nAngleNum]=offset_angle;
					nAngleNum++;
					footNailSet.append(pLineSlot);
				}
			}
			else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			{	//角钢脚钉
				CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPart;
				if(pLineAngle==NULL)
					continue;
				if(!pLineAngle->m_bFootNail)
					continue;
				pLineAngle->getUCS(pLineAngle->ucs);
				double offset_angle=Cal2dLineAng(f2dPoint(0,0),f2dPoint(pLineAngle->ucs.axis_z.x,pLineAngle->ucs.axis_z.y))*DEGTORAD_COEF;
				BOOL bFlag=TRUE;
				for(i=0;i<nAngleNum;i++)
				{
					if(fabs(angle[i]-offset_angle)<EPS2) //误差0.03°
					{	//已存在该角度值
						bFlag=FALSE;
						break;
					}
				}
				if(bFlag)
				{	//列表没有相同的角度值
					angle[nAngleNum]=offset_angle;
					nAngleNum++;
					footNailSet.append(pLineAngle);
				}
			}
			else if(pPart->GetClassTypeId()==CLS_PLATE)
				pPlate=(CLDSPlate*)pPart;
			else if(pPart->GetClassTypeId()==CLS_LINETUBE)
			{
				if(pPart->handle==pDatumPart->handle)
					continue;
				CLDSLineTube *pRayTube=(CLDSLineTube*)pPart;
				f3dPoint ray_tube_vec=pRayTube->End()-pRayTube->Start();
				normalize(ray_tube_vec);
				project_vector(ray_tube_vec,ucs.axis_z);
				if(pDatumRayTube==NULL)
				{	//初始化基准钢管
					pDatumRayTube=pRayTube;
					datum_ray_tube_vec=ray_tube_vec;
				}
				else if(fabs(datum_ray_tube_vec*ray_tube_vec)<EPS_COS2)
				{	//添加支管角度标注
					CAngularDimension *pAngular=Angulars.append();
					pAngular->m_bStartLineIsDatumLine=!pLineTube->m_bHasWeldRoad;
					pAngular->m_hStartPart=pDatumRayTube->handle;
					pAngular->m_hEndPart=pRayTube->handle;
					pAngular->m_fDimR=150;
				}
			}
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{	//查找十字插板 用来标注十字插板立板与基板之间的夹角 wht 10-06-29
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
				if(pParamPlate->m_iParamType==TYPE_XEND)
					pXEndPlate=pParamPlate;
			}
			if(pPlate==NULL)
				continue;
			if(pPlate->m_fInnerRadius>0)
				continue;	//跳过环形钢板不进行统计
			//某些特殊视图的基准构件不时基准钢管 wht 10-08-12
			if(bIncludeDatumTube)
			{
				int nFaceN=1;
				if(pPlate->designInfo.iFaceType==6)
					nFaceN=pPlate->face_N;
				GEPOINT vWorkNorm=pPlate->ucs.axis_z;
				bool hasAlongPlane=false;
				for(int iface=0;iface<pPlate->face_N;iface++)
				{
					if(iface>0)
						vWorkNorm=pPlate->GetHuoquFaceNorm(iface);
					if(fabs(vWorkNorm*pDatumPart->ucs.axis_z)>EPS2)
						continue;	//跳过不平行于钢管的钢板(环向板或者封板)
					hasAlongPlane=true;
					break;
				}
				if(!hasAlongPlane)
					continue;
			}
			else if(pJointTube&&(fabs(pPlate->ucs.axis_z*pJointTube->ucs.axis_z)>EPS2))
				continue;	//跳过不平行于钢管的钢板(环向板或者封板))
			CLDSNode *pNode=pTower->FromNodeHandle(pPlate->designInfo.m_hBaseNode);
			if(pNode==NULL)
				continue;
			PARTSET* pPartset=hashNodePartset.Add(pPlate->designInfo.m_hBaseNode);
			pPartset->append(pPlate);
			/*if(partsetTbl.GetValueAt(pPlate->designInfo.m_hBaseNode,j))
				partset[j].append(pPlate);
			else 
			{
				CLDSNode *pNode=pTower->FromNodeHandle(pPlate->designInfo.m_hBaseNode);
				if(pNode==NULL)
					continue;
				partsetTbl.SetValueAt(pPlate->designInfo.m_hBaseNode,nNodeNum);
				partset[nNodeNum].append(pPlate);
				nNodeNum++;
			}*/
		}
		CLDSPart *pPart=NULL;
		CAngularDimension *pAngular=NULL;
		CLDSLineSlot *pLineSlot=NULL;
		CLDSLinePart *pDatumLinePart=NULL;
		for(pPart=footNailSet.GetFirst();pPart;pPart=footNailSet.GetNext())
		{	
			if(pPart->GetClassTypeId()!=CLS_LINESLOT&&pPart->GetClassTypeId()!=CLS_LINEANGLE)
				continue;
			if(pPart->GetClassTypeId()==CLS_LINEANGLE&&!((CLDSLineAngle*)pPart)->m_bFootNail)
				continue;
			if(pDatumLinePart==NULL)
			{	//基准构件
				pDatumLinePart=(CLDSLinePart*)pPart;
				continue;
			}
			pAngular=Angulars.append();
			pAngular->m_bStartLineIsDatumLine=!pLineTube->m_bHasWeldRoad;
			pAngular->m_hStartPart=pDatumLinePart->handle;
			pAngular->m_hEndPart=pPart->handle;
			pAngular->m_fDimR=pLineTube->GetDiameter()*0.5*(Angulars.GetNodeNum()+1);
		}
		//for(i=0;i<nNodeNum;i++)
		for(PARTSETOBJ* pPartset=hashNodePartset.GetFirst();pPartset;pPartset=hashNodePartset.GetNext())
		{
			CLDSPlate *pPlate=NULL, *pDatumPlate=NULL;
			//pPart=partset[i].GetFirst();
			pPart=pPartset->GetFirst();
			if(pPart&&pPart->GetClassTypeId()==CLS_PLATE)
				pDatumPlate=(CLDSPlate*)pPart;
			else
				continue;
			if(pPartset->GetNodeNum()==1)
			{	//只有一个板,添加板到钢管基准边(焊道线)的角度标注
				pAngular=Angulars.append();
				pAngular->m_bStartLineIsDatumLine=!pLineTube->m_bHasWeldRoad;
				pAngular->m_hStartPart=pDatumPlate->handle;
				pAngular->m_hEndPart=0;
				pAngular->m_fDimR=pLineTube->GetDiameter()*0.5*(Angulars.GetNodeNum()+1);
			}
			else
			{	//添加板与板之间的角度标注
				for(pPart=pPartset->GetNext();pPart;pPart=pPartset->GetNext())
				{
					if(pPart==NULL||(pPart&&pPart->GetClassTypeId()!=CLS_PLATE))
						continue;
					CLDSPlate *pPlate=(CLDSPlate*)pPart;
					pAngular=Angulars.append();
					pAngular->m_bStartLineIsDatumLine=!pLineTube->m_bHasWeldRoad;
					pAngular->m_hStartPart=pDatumPlate->handle;
					pAngular->m_hEndPart=pPlate->handle;
					pAngular->m_fDimR=pLineTube->GetDiameter()*0.5*(Angulars.GetNodeNum()+1);
				}
			}
		}
		//暂不支持包板用角钢代替的十字插板非90度时的角度标注问题 wht 10-06-30
		if(m_iViewType==VIEW_INSERTPLATE&&pXEndPlate)
		{	
			ATTACHPART_ITEM *pItem=NULL;
			CLDSPlate *pVertPlate[2]={NULL,NULL};
			for(pItem=pXEndPlate->GetFirstAttachPart();pItem;pItem=pXEndPlate->GetNextAttachPart())
			{
				CLDSPlate *pPlate=(CLDSPlate*)pXEndPlate->BelongModel()->FromPartHandle(pItem->h,CLS_PLATE);
				if(pPlate==NULL)
					continue;
				f3dPoint plate_pos=pPlate->ucs.origin;
				coord_trans(plate_pos,pXEndPlate->ucs,FALSE);
				if(plate_pos.x<EPS&&plate_pos.y<EPS)
				{
					if(pVertPlate[0]==NULL)
						pVertPlate[0]=pPlate;	//立板1
					else if(pVertPlate[1]==NULL)
						pVertPlate[1]=pPlate;	//立板2
					else
						break;
				}
			}
			if(pVertPlate[0]&&pVertPlate[1])
			{	
				for(int i=0;i<2;i++)
				{
					f3dPoint norm_vec=pVertPlate[i]->ucs.axis_z;
					vector_trans(norm_vec,pXEndPlate->ucs,FALSE);
					if(fabs(norm_vec*f3dPoint(0,1,0))<EPS_COS)
					{
						pAngular=Angulars.append();
						pAngular->m_bStartLineIsDatumLine=!pLineTube->m_bHasWeldRoad;
						pAngular->m_hStartPart=pXEndPlate->handle;
						pAngular->m_hEndPart=pVertPlate[i]->handle;
						pAngular->m_fDimR=pVertPlate[i]->GetWidth()*0.5;
					}
				}
			}
		}
	}
	else if(pDatumPart->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pSonPlate=NULL;
		CLDSPlate *pDatumPlate=(CLDSPlate*)pDatumPart;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pTower->FromPartHandle(pDatumPlate->designInfo.m_hBasePart,CLS_LINEPART);
		if(pLinePart&&pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			f3dPoint len_vec=pLinePart->Start()-pLinePart->End();
			normalize(len_vec);
			if(fabs(len_vec*ucs.axis_z)<EPS2)
				return;	//正视图不需要标注角度
			CLDSGroupLineAngle *pGroupLineAngle=NULL;
			pGroupLineAngle=(CLDSGroupLineAngle*)pLinePart;
			for(pPartItem=SonParts.GetFirst();pPartItem;pPartItem=SonParts.GetNext())
			{	
				CLDSPlate *pPlate=(CLDSPlate*)pTower->FromPartHandle(pPartItem->hSonPart,CLS_PLATE);
				if(pPlate&&pPlate->designInfo.m_hBaseNode==pDatumPlate->designInfo.m_hBaseNode
					&&pPlate->designInfo.m_hBasePart==pDatumPlate->designInfo.m_hBasePart&&pPlate->handle!=pDatumPlate->handle)
				{	//找到一块与基准钢板基准节点以及基准构件均相同的钢板（排除加劲板）
					pSonPlate=pPlate;
					break;
				}
			}
			if(pGroupLineAngle&&pSonPlate)
			{
				CAngularDimension *pAngular=Angulars.append();
				pAngular->m_bStartLineIsDatumLine=FALSE;
				pAngular->m_hStartPart=pDatumPart->handle;
				pAngular->m_hEndPart=pSonPlate->handle;
				pAngular->m_fDimR=150;
			}
		}
		else if(pLinePart&&pLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{	//塔脚板 牛蹄板
			//...
		}
	}
}

static BOOL CalFLAndPlateInters(CLDSParamPlate *pFL,CLDSBolt *pBolt,f3dPoint &inters_vertex)
{	
	if(pBolt==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("When calculate the checking line between flange and plate,bolt is null！");
#else
		logerr.Log("计算法兰与钢板间检测线时，遭遇螺栓为空！");
#endif
		return FALSE;
	}
	CLDSPlate *pPlate=(CLDSPlate*)pFL->BelongModel()->FromPartHandle(pBolt->feature,CLS_PLATE);
	if(pPlate==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("When calculate the checking line between flange and plate,bolt is null！");
#else
		logerr.Log("计算法兰与钢板间检测线时，遭遇钢板为空！");
#endif
		return FALSE;
	}
	//1.钢管所在平面与法兰上端面所在面求交
	f3dPoint inters_pick,inters_line_vec;
	f3dPoint face1_pick=pFL->ucs.origin+pFL->ucs.axis_z*pFL->GetThick();
	if(Int3d2f(&inters_pick,&inters_line_vec,face1_pick,pFL->ucs.axis_z,pPlate->ucs.origin,pPlate->ucs.axis_z)==1)
	{	//2.面面求交，求得的交线与法兰外圆球交点，取离基准点较近的点
		//2.1 将交线方向转换到法兰坐标系下
		normalize(inters_line_vec);
		vector_trans(inters_line_vec,pFL->ucs,FALSE);
		inters_pick.z=inters_line_vec.z=0;
		//2.2 线与圆求交
		double D=0;
		if(pFL->m_iParamType==TYPE_FL||
			pFL->m_iParamType==TYPE_FLG||
			pFL->m_iParamType==TYPE_FLR)
			pFL->GetDesignItemValue('W',&D);
		else
			pFL->GetDesignItemValue('D',&D);
		f3dPoint startPt=inters_line_vec*D,endPt=-inters_line_vec*D;
		f2dLine line(f2dPoint(startPt.x,startPt.y),f2dPoint(endPt.x,endPt.y));
		f3dPoint inters_vertex1,inters_vertex2;
		//2.3 交线与法兰外圆求交点
		int nRetCode=Int2dlc(line,f2dCircle(0.5*D,0,0),inters_vertex1.x,inters_vertex1.y,inters_vertex2.x,inters_vertex2.y);
		if(pFL->m_iParamType!=TYPE_FLD&&pFL->m_iParamType!=TYPE_FLP)
			inters_vertex1.z=inters_vertex2.z=pFL->GetThick();	//只有普通法兰（含刚性及柔性等非高颈法兰）坐标系由内向外 wjh-2016.6.21
		coord_trans(inters_vertex1,pFL->ucs,TRUE);
		coord_trans(inters_vertex2,pFL->ucs,TRUE);
		if(nRetCode==1)
			inters_vertex=inters_vertex1;
		else if(nRetCode==2) 
		{
			if(DISTANCE(inters_vertex1,pBolt->ucs.origin)>DISTANCE(inters_vertex2,pBolt->ucs.origin))
				inters_vertex=inters_vertex2;
			else 
				inters_vertex=inters_vertex1;
		}
		else 
			return FALSE;
		return TRUE;
	}
	return FALSE;
}

static void AddCheckLinesByBoltSet(CSectionView *pSectionView,CLDSLineTube *pLineTube,
									BOLTSET &partSet,BOOL bDimUpCheckLine,CHashSet<CLDSLineTube*>* pHashBoltToTubes=NULL)
{	//添加检测线
	CLsRef *pLsRef=NULL;
	CCheckLineDimension checkLine,*pCheckLine=NULL;
	CLDSPart *pPart=NULL,*pDownPart=NULL,*pPrePart=NULL;
	CDualTubeJointView* pDualTubeJointView=pSectionView->IsDualTubeJointView()?(CDualTubeJointView*)pSectionView:NULL;
	//1 添加检测线
	//1.1 在始端构件与UpPartSet第一个构件之间以及UPdateSet相邻两构件之间添加检测线
	long hStartPlate=pDualTubeJointView!=NULL?pDualTubeJointView->hStartJointPlate:pLineTube->m_tJointStart.hLinkObj;
	long hEndPlate=pDualTubeJointView!=NULL?pDualTubeJointView->hEndJointPlate:pLineTube->m_tJointEnd.hLinkObj;
	CLDSParamPlate *pStartParamPlate=(CLDSParamPlate*)pLineTube->BelongModel()->FromPartHandle(hStartPlate,CLS_PARAMPLATE);
	CLDSParamPlate *pEndParamPlate=(CLDSParamPlate*)pLineTube->BelongModel()->FromPartHandle(hEndPlate,CLS_PARAMPLATE);
	if(pStartParamPlate)
		pPrePart=pStartParamPlate;
	//视图X轴方向与钢管Z轴方向想反时上下侧螺栓判断错误，此处调整坐标系与钢管延伸方向一致 wht 18-03-14
	UCS_STRU cs=pSectionView->ucs;
	cs.origin=pLineTube->Start();
	cs.axis_y=pSectionView->ucs.axis_z^pLineTube->ucs.axis_z;
	cs.axis_x=cs.axis_y^pSectionView->ucs.axis_z;
	normalize(cs.axis_x);
	normalize(cs.axis_y);
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_BOLT)
		{	//焊前检测线
			pCheckLine=pSectionView->CheckLines.append();
			pCheckLine->m_hStartPart=pPart->handle;
			CLDSLineTube* pWeldTube=pHashBoltToTubes!=NULL?pHashBoltToTubes->GetValue(pPart->handle):NULL;
			if(pWeldTube)
				pCheckLine->m_hEndPart=pWeldTube->handle;
			else
				pCheckLine->m_hEndPart=pLineTube->handle;
			pCheckLine->m_bBeforeWeld=TRUE;
		}
		if(pPrePart)
		{	//焊接后检测线
			if(pPrePart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPrePart;
				if(pParamPlate->IsFL())
				{
					/*double D=0,fBoltR=0; 
					if(pParamPlate->m_iParamType==TYPE_FL||
						pParamPlate->m_iParamType==TYPE_FLG||
						pParamPlate->m_iParamType==TYPE_FLR)
						pParamPlate->GetDesignItemValue('W',&D);
					else
						pParamPlate->GetDesignItemValue('D',&D);
					f3dPoint pos(0,0.5*D,0);
					coord_trans(pos,cs,TRUE);
					//f3dPoint pos=pParamPlate->ucs.origin+ucs.axis_y*0.5*D;
					coord_trans(pos,pParamPlate->ucs,FALSE);
					checkLine.m_xStartLocalCoordPos.Set(pos.x,pos.y,pParamPlate->GetThick());*/
					f3dPoint inters_vertex;
					if(CalFLAndPlateInters(pParamPlate,(CLDSBolt*)pPart,inters_vertex))
					{
						checkLine.m_xStartLocalCoordPos=inters_vertex;
						coord_trans(checkLine.m_xStartLocalCoordPos,pParamPlate->ucs,FALSE);
					}
					else 
#ifdef AFX_TARG_ENU_ENGLISH
						logerr.Log("Group weld part 0x%X- view %s,failed to add checking line，error part (0x%X)",
						pSectionView->BelongPartGroup()->handle,pSectionView->sName,pParamPlate->handle);
#else
						logerr.Log("组焊件0x%X-视图%s,添加检测线时错误，出错构件(0x%X)",
						pSectionView->BelongPartGroup()->handle,pSectionView->sName,pParamPlate->handle);
#endif
				}
				else if(pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND
					||pParamPlate->m_iParamType==TYPE_XEND)
				{
					if(pParamPlate->m_iParamType!=TYPE_XEND&&fabs(pParamPlate->ucs.axis_z*cs.axis_z)<EPS_COS2)
					{
						pPrePart=pPart;
						continue;	//该插板在当前视图中显示侧面
					}
					CLDSBolt *pCurBolt=NULL;
					double max_y=-1000000,min_y=1000000,L=10000000;
					pParamPlate->GetDesignItemValue('L',&L);
					for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{
						if(fabs((*pLsRef)->get_norm()*cs.axis_z)<EPS_COS2)
							continue;	//跳过插板上法线方向与当前视图Z方向不同的螺栓
						f3dPoint ls_pos=pLsRef->GetLsPtr()->ucs.origin;
						GEPOINT localpos=ls_pos;
						coord_trans(localpos,pParamPlate->ucs,FALSE);
						if(pParamPlate->m_iParamType==TYPE_XEND&&localpos.x>L)
							continue;	//跳过不在插板基板长度范围内的螺栓 wht 10-08-06
						coord_trans(ls_pos,cs,FALSE);
						if(bDimUpCheckLine)
						{
							if(ls_pos.y<max_y)	//Y坐标最大
								continue;
							pCurBolt=pLsRef->GetLsPtr();
							max_y=ls_pos.y;
						}
						else
						{
							if(ls_pos.y>min_y)	//Y坐标最小
								continue;
							pCurBolt=pLsRef->GetLsPtr();
							min_y=ls_pos.y;
						}
					}
					pPrePart=pCurBolt;
				}
				else
				{
					pPrePart=pPart;
					continue;
				}
			}
			else 
				checkLine.m_xStartLocalCoordPos.Set();
			if(pPrePart)
			{
				if(pPrePart->GetClassTypeId()==CLS_BOLT&&pPart->GetClassTypeId()==CLS_BOLT&&pPrePart->feature==pPart->feature)
				{
					pPrePart=pPart;
					continue;	//同一钢板上的螺栓不需要添加焊后检测线
				}
				//检测尺寸除第一个和最后一个构件可能不为螺栓外，其余构件都为螺栓
				//故m_xStartLocalCoordPos需要单独计算,m_xEndLocalCoordPos设置为(0,0,0)即可 wht 11-07-18
				checkLine.m_xEndLocalCoordPos.Set();
				checkLine.m_hStartPart=pPrePart->handle;
				checkLine.m_hEndPart=pPart->handle;
				checkLine.m_bBeforeWeld=FALSE;
				pSectionView->CheckLines.append(checkLine);
			}
		}
		pPrePart=pPart;
	}
	CLDSBolt* pEndBolt=(CLDSBolt*)partSet.GetTail();
	//1.2 添加终端构件与partSet最后一个构件之间的检测线
	if(pEndParamPlate&&partSet.GetNodeNum()>0&&pEndParamPlate->handle!=pEndBolt->handle)
	{
		CLDSPart *pEndPart=pEndParamPlate;
		if(pEndParamPlate->IsFL())
		{
			/*double D=0;
			if(pEndParamPlate->m_iParamType==TYPE_FL||
				pEndParamPlate->m_iParamType==TYPE_FLG||
				pEndParamPlate->m_iParamType==TYPE_FLR)
				pEndParamPlate->GetDesignItemValue('W',&D);
			else
				pEndParamPlate->GetDesignItemValue('D',&D);
			f3dPoint pos(0,0.5*D,0);
			coord_trans(pos,cs,TRUE);
			//f3dPoint pos=pEndParamPlate->ucs.origin+ucs.axis_y*0.5*D;
			coord_trans(pos,pEndParamPlate->ucs,FALSE);
			checkLine.m_xEndLocalCoordPos.Set(pos.x,pos.y,pEndParamPlate->GetThick());*/
			f3dPoint inters_vertex;
			if(CalFLAndPlateInters(pEndParamPlate,pEndBolt,inters_vertex))
			{
				checkLine.m_xEndLocalCoordPos=inters_vertex;
				coord_trans(checkLine.m_xEndLocalCoordPos,pEndParamPlate->ucs,FALSE);
			}
			else 
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Group weld part 0x%X-view %s,failed to add checking line,error part (0x%X)",
				pSectionView->BelongPartGroup()->handle,pSectionView->sName,pEndParamPlate->handle);
#else
				logerr.Log("组焊件0x%X-视图%s,添加检测线时错误，出错构件(0x%X)",
				pSectionView->BelongPartGroup()->handle,pSectionView->sName,pEndParamPlate->handle);
#endif
		}
		else if(pEndParamPlate->m_iParamType==TYPE_UEND||pEndParamPlate->m_iParamType==TYPE_ROLLEND
			||pEndParamPlate->m_iParamType==TYPE_XEND)
		{
			if(pEndParamPlate->m_iParamType!=TYPE_XEND&&fabs(pEndParamPlate->ucs.axis_z*cs.axis_z)<EPS_COS2)
				pEndPart=NULL;	//该插板在当前视图中显示侧面
			else
			{
				CLDSBolt *pCurBolt=NULL;
				double max_y=-1000000,min_y=1000000,L=1000000;
				pEndParamPlate->GetDesignItemValue('L',&L);
				for(pLsRef=pEndParamPlate->GetFirstLsRef();pLsRef;pLsRef=pEndParamPlate->GetNextLsRef())
				{
					if(fabs((*pLsRef)->get_norm()*cs.axis_z)<EPS_COS2)
						continue;	//跳过插板上法线方向与当前视图Z方向不同的螺栓
					f3dPoint ls_pos=pLsRef->GetLsPtr()->ucs.origin;
					GEPOINT localpos=ls_pos;
					coord_trans(localpos,pEndParamPlate->ucs,FALSE);
					if(pEndParamPlate->m_iParamType==TYPE_XEND&&localpos.x>L)
						continue;	//跳过不在插板基板长度范围内的螺栓 wht 10-08-06
					coord_trans(ls_pos,cs,FALSE);
					if(bDimUpCheckLine)
					{
						if(ls_pos.y<max_y)		//Y坐标最大
							continue;	
						pCurBolt=pLsRef->GetLsPtr();
						max_y=ls_pos.y;
					}
					else
					{
						if(ls_pos.y>min_y)		//Y坐标最小
							continue;
						pCurBolt=pLsRef->GetLsPtr();
						min_y=ls_pos.y;
					}
				}
				pEndPart=pCurBolt;
			}
		}
		else 
			checkLine.m_xEndLocalCoordPos.Set();
		if(pEndPart)
		{
			//检测尺寸除第一个和最后一个构件可能不为螺栓外，其余构件都为螺栓
			//m_xEndLocalCoordPos单独计算,m_xStartLocalCoordPos设置为(0,0,0) wht 11-07-18
			checkLine.m_xStartLocalCoordPos.Set();
			checkLine.m_hStartPart=partSet.GetTail()->handle;
			checkLine.m_hEndPart=pEndPart->handle;
			checkLine.m_bBeforeWeld=FALSE;
			pSectionView->CheckLines.append(checkLine);
		}
	}
}
//获取钢管端连接参数化板上的指定检测点
CLDSPart* GetTubeJointPart(CLDSLineTube* pLineTube,BOOL bIntersect);
BOOL GetTubeCheckPoint(CLDSLineTube* pLineTube,BOOL bLeft,f3dPoint workPlaneNorm,f3dPoint &checkPt,BOOL bStart=FALSE)
{
	if(pLineTube==NULL)
		return FALSE;
	CLDSPart* pPart=NULL;
	f3dPoint datumPt,datumVec;
	BOOL bJointTube=FALSE;
	if(pLineTube->m_tJointStart.type>=2 && pLineTube->m_tJointEnd.type>=2)
	{
		if(bStart)
		{
			pPart=pLineTube->BelongModel()->FromPartHandle(pLineTube->m_tJointStart.hLinkObj);
			datumVec=pLineTube->Start()-pLineTube->End();
			normalize(datumVec);
			datumPt=pLineTube->Start()+datumVec*pLineTube->startOdd();
		}
		else
		{	
			pPart=pLineTube->BelongModel()->FromPartHandle(pLineTube->m_tJointEnd.hLinkObj);
			datumVec=pLineTube->End()-pLineTube->Start();
			normalize(datumVec);
			datumPt=pLineTube->End()+datumVec*pLineTube->endOdd();
		}
	}
	else if(pLineTube->m_tJointStart.type==1 || pLineTube->m_tJointEnd.type==1)
	{
		bJointTube=TRUE;
		pPart=GetTubeJointPart(pLineTube,FALSE);
		if(pLineTube->m_tJointStart.type==1)
		{
			datumVec=pLineTube->End()-pLineTube->Start();
			normalize(datumVec);
			datumPt=pLineTube->End()+datumVec*pLineTube->endOdd();
		}
		else
		{
			datumVec=pLineTube->Start()-pLineTube->End();
			normalize(datumVec);
			datumPt=pLineTube->Start()+datumVec*pLineTube->startOdd();
		}
	}
	else
		return FALSE;
	if(pPart==NULL || pPart->GetClassTypeId()!=CLS_PARAMPLATE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"%X tube's end connecting part doesn't have parametric plate！",pLineTube->handle);
#else
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"%X钢管端连接构件没有参数板！",pLineTube->handle);
#endif
		return FALSE;
	}
	UCS_STRU ucs;
	ucs.origin=datumPt;
	ucs.axis_x=datumVec;
	ucs.axis_z=workPlaneNorm;
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	CLDSParamPlate* pParaPlate=(CLDSParamPlate*)pPart;
	if(pParaPlate->IsFL())
	{
		double H=0,W=0;
		pParaPlate->GetDesignItemValue('H',&H);
		pParaPlate->GetDesignItemValue('W',&W);
		datumPt=datumPt+datumVec*H;
		if(bLeft)
			checkPt=datumPt+ucs.axis_y*W*0.5;
		else
			checkPt=datumPt-ucs.axis_y*W*0.5;
	}
	else if(pParaPlate->m_iParamType==TYPE_UEND||pParaPlate->m_iParamType==TYPE_ROLLEND
		||pParaPlate->m_iParamType==TYPE_XEND)
	{
		double max_x=-10000,min_x=10000,max_y=-10000,min_y=10000,L=100000;
		CXhChar50 lsXValue,lsYValue;
		pParaPlate->GetDesignItemValue('L',&L);
		for(CLsRef* pLsRef=pParaPlate->GetFirstLsRef();pLsRef;pLsRef=pParaPlate->GetNextLsRef())
		{
			f3dPoint ls_pos=pLsRef->GetLsPtr()->ucs.origin;
			coord_trans(ls_pos,ucs,FALSE);
			lsXValue.Printf("%.f",ls_pos.x);
			lsYValue.Printf("%.f",ls_pos.y);
			if(bLeft)
			{
				if(atoi(lsYValue)<max_y || atoi(lsXValue)<max_x)	//Y坐标最大且X坐标最大
					continue;
				max_y=atoi(lsYValue);
				max_x=atoi(lsXValue);
				checkPt=ls_pos;
			}
			else
			{
				if(atoi(lsYValue)>min_y || atoi(lsXValue)<max_x)	//Y坐标最小且X坐标最大
					continue;
				min_y=atoi(lsYValue);
				max_x=atoi(lsXValue);
				checkPt=ls_pos;
			}
		}
		coord_trans(checkPt,ucs,TRUE);
	}
	return TRUE;
}
//获取钢管的端连接构件
CLDSPart* GetTubeJointPart(CLDSLineTube* pLineTube,BOOL bIntersect)
{
	if(pLineTube==NULL)
		return NULL;
	IModel* pModel=pLineTube->BelongModel();
	if(pLineTube->m_tJointStart.type==1)		//始端相贯
	{
		if(bIntersect)		//相贯处构件
			return pModel->FromPartHandle(pLineTube->m_tJointStart.hLinkObj);
		else				//非相贯处构件
			return pModel->FromPartHandle(pLineTube->m_tJointEnd.hLinkObj);
	}
	else if(pLineTube->m_tJointEnd.type==1)		//终端相贯
	{
		if(bIntersect)		//相贯处构件
			return pModel->FromPartHandle(pLineTube->m_tJointEnd.hLinkObj);
		else				//非相贯处构件
			return pModel->FromPartHandle(pLineTube->m_tJointStart.hLinkObj);
	}
	else
		return NULL;
}
//增加相贯支管检测线(支管参数化板到主管参数化板的距离)
static void AddCheckLinesByTubeSet(CSectionView *pSectionView,CLDSLineTube* pDatumTube,TUBESET &RayTubeSet,BOOL bDimUpCheckLine)
{
	//1、获取基准钢管的始终端检测点
	IModel* pModel=pDatumTube->BelongModel();
	f3dPoint startCheckPt,endCheckPt;
	if(pDatumTube->m_tJointStart.type>=2&&pDatumTube->m_tJointEnd.type>=2)			//无相贯钢管
	{	
		if(bDimUpCheckLine)
		{
			GetTubeCheckPoint(pDatumTube,FALSE,pSectionView->ucs.axis_z,startCheckPt,TRUE);
			GetTubeCheckPoint(pDatumTube,TRUE,pSectionView->ucs.axis_z,endCheckPt,FALSE);
		}
		else
		{
			GetTubeCheckPoint(pDatumTube,TRUE,pSectionView->ucs.axis_z,startCheckPt,TRUE);
			GetTubeCheckPoint(pDatumTube,FALSE,pSectionView->ucs.axis_z,endCheckPt,FALSE);
		}
	}
	else if((pDatumTube->m_tJointStart.type==1&&pDatumTube->m_tJointStart.hViceLinkObj<0) 
		|| (pDatumTube->m_tJointEnd.type==1&&pDatumTube->m_tJointEnd.hViceLinkObj<0))	//对接相贯钢管
	{	
		CLDSLineTube* pJointTube=(CLDSLineTube*)GetTubeJointPart(pDatumTube,TRUE);
		if(bDimUpCheckLine)
		{
			GetTubeCheckPoint(pDatumTube,FALSE,pSectionView->ucs.axis_z,startCheckPt);
			GetTubeCheckPoint(pJointTube,TRUE,pSectionView->ucs.axis_z,endCheckPt);
		}
		else
		{
			GetTubeCheckPoint(pDatumTube,TRUE,pSectionView->ucs.axis_z,startCheckPt);
			GetTubeCheckPoint(pJointTube,FALSE,pSectionView->ucs.axis_z,endCheckPt);
		}
	}
	else
		return;
	if(startCheckPt.IsZero() || endCheckPt.IsZero())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("%X datum tube's end connecting doesn't have parametric plate!",pDatumTube->handle);
#else
		logerr.Log("%X基准钢管端连接没有参数化板!",pDatumTube->handle);
#endif
		return;
	}
	//2、始端参数化板与ParaPlateSet第一个构件间检测线及ParaPlateSet相邻构件之间的检测线
	f3dPoint startPt=startCheckPt,endPt,vertex1,vertex2;
	for(CLDSLineTube* pRayTube=RayTubeSet.GetFirst();pRayTube;pRayTube=RayTubeSet.GetNext())
	{
		CLDSPart* pPart=GetTubeJointPart(pRayTube,FALSE);
		if(pPart==NULL || pPart->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		if(bDimUpCheckLine)
		{
			GetTubeCheckPoint(pRayTube,TRUE,pSectionView->ucs.axis_z,vertex1);
			GetTubeCheckPoint(pRayTube,FALSE,pSectionView->ucs.axis_z,vertex2);
		}
		else
		{	
			GetTubeCheckPoint(pRayTube,FALSE,pSectionView->ucs.axis_z,vertex1);
			GetTubeCheckPoint(pRayTube,TRUE,pSectionView->ucs.axis_z,vertex2);
		}
		//添加检测线
		endPt=vertex1;
		CCheckLineDimension *pCheckLine=pSectionView->CheckLines.append();
		pCheckLine->m_bSpecStartLocatePart=FALSE;
		pCheckLine->m_xStartLocalCoordPos=startPt;
		pCheckLine->m_bSpecEndLocatePart=FALSE;
		pCheckLine->m_xEndLocalCoordPos=endPt;
		pCheckLine->m_bBeforeWeld=FALSE;
		//
		startPt=vertex2;
	}
	//3、添加终端参数化板与RayTubeSet最后一个构件之间的检测线
	endPt=endCheckPt;
	CCheckLineDimension *pCheckLine=pSectionView->CheckLines.append();
	pCheckLine->m_bSpecStartLocatePart=FALSE;
	pCheckLine->m_xStartLocalCoordPos=startPt;
	pCheckLine->m_bSpecEndLocatePart=FALSE;
	pCheckLine->m_xEndLocalCoordPos=endPt;
	pCheckLine->m_bBeforeWeld=FALSE;
}
BOOL CSectionView::IsExistIntersectTube(CLDSLineTube *pDatumTube,IModel *pTower)
{
	if(pDatumTube==NULL)
		return FALSE;
	for(CSectionView::PART_ITEM *pPartItem=SonParts.GetFirst();pPartItem;pPartItem=SonParts.GetNext())
	{
		CLDSPart* pPart=pTower->FromPartHandle(pPartItem->hSonPart);
		if(pPart==NULL || pPart->GetClassTypeId()!=CLS_LINETUBE)
			continue;
		CLDSLineTube* pLineTube=(CLDSLineTube*)pPart;
		if(fabs(pLineTube->ucs.axis_z*ucs.axis_z)>EPS_COS)	//过滤钢管端面图
			continue;
		if(pLineTube->m_tJointStart.hLinkObj==pDatumTube->handle&&pLineTube->m_tJointStart.type==1
			&&pLineTube->m_tJointStart.hViceLinkObj>=0)	//支管始端相贯
			return TRUE;
		else if(pLineTube->m_tJointEnd.hLinkObj==pDatumTube->handle&&pLineTube->m_tJointEnd.type==1
			&&pLineTube->m_tJointEnd.hViceLinkObj>=0)	//支管终端相贯
			return TRUE;
	}
	return FALSE;
}
//判断当前视图是否为钢管标准简图(主视图或正视图)
BOOL CSectionView::IsBriefTubeView(IModel* pTower)
{
	CLDSPart* pDatumPart=(CLDSPart*)pTower->FromPartHandle(m_pPartGroup->handle);
	if(pDatumPart==NULL || pDatumPart->GetClassTypeId()!=CLS_LINETUBE)
		return FALSE;
	CLDSLineTube* pTube=(CLDSLineTube*)pDatumPart;
	CLDSParamPlate* pStartLjPlate=(CLDSParamPlate*)pTower->FromPartHandle(pTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
	CLDSParamPlate* pEndLjPlate	 =(CLDSParamPlate*)pTower->FromPartHandle(pTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
	if(pStartLjPlate==NULL||pEndLjPlate==NULL)
		return FALSE;
	if(m_iViewType!=VIEW_MAIN&&m_iViewType!=VIEW_FRONT)
		return FALSE;
	if((pStartLjPlate->IsFL()&&pEndLjPlate->IsFL())||
		(pStartLjPlate->IsInsertPlate()&&pEndLjPlate->IsInsertPlate()))
		return TRUE;
	return FALSE;
}
BOOL CSectionView::IsTubeTowerFootView(IModel* pTower,CLDSPlate **ppTowerFootPlate/*=NULL*/)
{
	if(pTower==NULL)
		return FALSE;
	if(m_iViewType!=VIEW_MAIN&&m_iViewType!=VIEW_FRONT)
		return FALSE;
	CLDSLineTube *pDatumTube=(CLDSLineTube*)pTower->FromPartHandle(m_pPartGroup->handle,CLS_LINETUBE);
	if(pDatumTube==NULL)
		return FALSE;
	//查找钢管水平塔脚板
	CLDSPlate *pTowerFootPlate=NULL;
	for(CSectionView::PART_ITEM *pPartItem=SonParts.GetFirst();pPartItem;pPartItem=SonParts.GetNext())
	{
		if(TagPart(pPartItem)==NULL)
			LoadTagInfo(pPartItem)->pCurPart=pTower->FromPartHandle(pPartItem->hSonPart);
		CLDSPart *pPart=TagPart(pPartItem);		
		if(pPart==NULL||pPart->GetClassTypeId()!=CLS_PLATE)
			continue;
		if(pPart->ucs.axis_z.IsEqual(f3dPoint(0,0,1))||pPart->ucs.axis_z.IsEqual(f3dPoint(0,0,-1)))
		{
			pTowerFootPlate=(CLDSPlate*)pPart;
			break;
		}
	}
	if(ppTowerFootPlate)
		*ppTowerFootPlate=pTowerFootPlate;
	return (pTowerFootPlate!=NULL);
}
void CSectionView::AppendIntersectTubeCheckLines(CLDSLineTube* pDatumTube,IModel* pTower)
{
	if(pDatumTube==NULL)
		return;
	//端面图不添加检测线
	if(fabs(pDatumTube->ucs.axis_z*ucs.axis_z)>EPS_COS)
		return;
	//1、查找相贯支管集合并进行上下分组
	ARRAY_LIST<JOINTRAYTUBE> upRayTubeList;		//统计视图上侧添加检测线的支管
	ARRAY_LIST<JOINTRAYTUBE> downRayTubeList;	//统计视图下册添加检测线的支管
	f3dPoint origin=ucs.origin,vertex;
	JOINTRAYTUBE* pRayTubeInfo=NULL;
	for(CSectionView::PART_ITEM *pPartItem=SonParts.GetFirst();pPartItem;pPartItem=SonParts.GetNext())
	{
		CLDSPart* pPart=pTower->FromPartHandle(pPartItem->hSonPart);
		if(pPart==NULL || pPart->GetClassTypeId()!=CLS_LINETUBE)
			continue;
		CLDSLineTube* pLineTube=(CLDSLineTube*)pPart;
		if(fabs(pLineTube->ucs.axis_z*ucs.axis_z)>EPS_COS)
			continue;
		f3dPoint len_vec;
		if(pLineTube->m_tJointStart.hLinkObj==pDatumTube->handle&&pLineTube->m_tJointStart.type==1
			&&pLineTube->m_tJointStart.hViceLinkObj>=0)	//钢管始端相贯
		{
			len_vec=pLineTube->End()-pLineTube->Start();
			normalize(len_vec);
			vertex=pLineTube->End()+len_vec*pLineTube->endOdd();
			Int3dlf(vertex,vertex,ucs.axis_y,origin,ucs.axis_y);
			if(len_vec*ucs.axis_y>0)
			{
				pRayTubeInfo=upRayTubeList.append();
				pRayTubeInfo->hRayTube=pLineTube->handle;
				pRayTubeInfo->fDist=DISTANCE(origin,vertex);
			}
			else
			{
				pRayTubeInfo=downRayTubeList.append();
				pRayTubeInfo->hRayTube=pLineTube->handle;
				pRayTubeInfo->fDist=DISTANCE(origin,vertex);
			}
		}
		else if(pLineTube->m_tJointEnd.hLinkObj==pDatumTube->handle&&pLineTube->m_tJointEnd.type==1
			&&pLineTube->m_tJointEnd.hViceLinkObj>=0)	//钢管终端相贯
		{	
			len_vec=pLineTube->Start()-pLineTube->End();
			normalize(len_vec);
			vertex=pLineTube->Start()+len_vec*pLineTube->startOdd();
			Int3dlf(vertex,vertex,ucs.axis_y,origin,ucs.axis_y);
			if(len_vec*ucs.axis_y>0)
			{
				pRayTubeInfo=upRayTubeList.append();
				pRayTubeInfo->hRayTube=pLineTube->handle;
				pRayTubeInfo->fDist=DISTANCE(origin,vertex);
			}
			else
			{	
				pRayTubeInfo=downRayTubeList.append();
				pRayTubeInfo->hRayTube=pLineTube->handle;
				pRayTubeInfo->fDist=DISTANCE(origin,vertex);
			}
		}
	}
	//2、添加上检测线
	int upRayTubeNum=upRayTubeList.GetSize();
	if(upRayTubeNum>0)
	{
		//对上侧支管集合进行排序
		if(upRayTubeNum>1)
			CQuickSort<JOINTRAYTUBE>::QuickSortClassic(upRayTubeList.m_pData,upRayTubeNum);
		TUBESET upRayTubeSet;
		for(pRayTubeInfo=upRayTubeList.GetFirst();pRayTubeInfo;pRayTubeInfo=upRayTubeList.GetNext())
		{
			CLDSLineTube* pRayTube=(CLDSLineTube*)pTower->FromPartHandle(pRayTubeInfo->hRayTube);
			upRayTubeSet.append(pRayTube);
		}
		//添加检测线
		AddCheckLinesByTubeSet(this,pDatumTube,upRayTubeSet,TRUE);
	}
	//3、添加下检测线
	int downRayTubeNum=downRayTubeList.GetSize();
	if(downRayTubeNum>0)
	{
		//对下侧钢管集合进行排序
		if(downRayTubeNum>1)
			CQuickSort<JOINTRAYTUBE>::QuickSortClassic(downRayTubeList.m_pData,downRayTubeNum);
		TUBESET downRayTubeSet;
		for(pRayTubeInfo=downRayTubeList.GetFirst();pRayTubeInfo;pRayTubeInfo=downRayTubeList.GetNext())
		{
			CLDSLineTube* pRayTube=(CLDSLineTube*)pTower->FromPartHandle(pRayTubeInfo->hRayTube);
			downRayTubeSet.append(pRayTube);
		}
		//添加检测线
		AddCheckLinesByTubeSet(this,pDatumTube,downRayTubeSet,FALSE);
	}
}
//添加默认的检测线
void CSectionView::AddDefaultCheckLines(CLDSPart *pDatumPart,IModel *pTower)
{
	if(pDatumPart==NULL)
		return;
	CLogErrorLife loglife;
	CCheckLineDimension *pCheckLine=NULL;
	CSectionView::PART_ITEM *pPartItem = NULL;
	if(this->IsDualTubeJointView())//m_idViewType==CSectionView::CLS_HORIZ_BEAM_VIEW)
	{
		CDualTubeJointView* pHorizBeamView=(CDualTubeJointView*)this;
		CLDSLineTube* pHorizTube=(CLDSLineTube*)((pHorizBeamView->m_hMasterTube==pDatumPart->handle)?pDatumPart:pTower->FromPartHandle(pHorizBeamView->m_hMasterTube));
		CLDSLineTube* pJointTube=(CLDSLineTube*)((pHorizBeamView->m_hJointTube==pDatumPart->handle)?pDatumPart:pTower->FromPartHandle(pHorizBeamView->m_hJointTube));
		if(pHorizTube==NULL||pJointTube==NULL)
			return;
		pHorizBeamView->UpdateConnectState(pTower);
		//2.添加钢管始终端端头和钢管中间的连接板之间的检测线(焊前检测线和焊后检测线)
		//2.1 查找需要添加检测线的钢板
		PLATESET PlateSet;	//统计Z轴方向与当前视图方向在一条直线上的钢板
		CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;
		pStartParamPlate=(CLDSParamPlate*)pTower->FromPartHandle(pHorizBeamView->hStartJointPlate,CLS_PARAMPLATE);
		pEndParamPlate=(CLDSParamPlate*)pTower->FromPartHandle(pHorizBeamView->hEndJointPlate,CLS_PARAMPLATE);
		for(pPartItem=SonParts.GetFirst();pPartItem;pPartItem=SonParts.GetNext())
		{
			CLDSPlate *pPlate=(CLDSPlate*)pTower->FromPartHandle(pPartItem->hSonPart,CLS_PLATE);
			if(pPlate==NULL)
				continue;
			//排除十字插板附加钢板,避免重复添加检测线
			if(pStartParamPlate&&pStartParamPlate->m_iParamType==TYPE_XEND&&
				pStartParamPlate->IsInAttachPartList(pPlate->handle))
				continue;
			if(pEndParamPlate&&pEndParamPlate->m_iParamType==TYPE_XEND&&
				pEndParamPlate->IsInAttachPartList(pPlate->handle))
				continue;
			//cos0.5°= 0.99996192306417128873735516482698
			//double cosa=0.98480775301220805936674302458952;	//wht 10-08-15 使用EPS_COS精确度过高改为cos0.5°
			//生成视图时使用EPS_COS2判断是否共面，此处也应使用EPS_COS2判断,
			//否则会导致即未单独为某些钢板添加独立的视图，此处也未标注相关检测线 wht 11-07-18
			double tolerance=EPS_COS2;
			CHorizBeamTubeView* pHorizBeamView2=(pHorizBeamView->m_idViewType==CSectionView::CLS_HORIZ_BEAM_VIEW)?(CHorizBeamTubeView*)pHorizBeamView:NULL;

			if( (pPlate->m_hPartWeldParent==pHorizBeamView->m_hJointTube||pPlate->m_hPartWeldParent==pHorizBeamView->m_hMasterTube)&&
				(pHorizBeamView2==NULL||(pHorizBeamView2&&pHorizBeamView2->m_cSubType=='F')))
				tolerance=0.5;	//	//展开面法线与基面法线夹角小于30°即视为立面展开
			if(fabs(pPlate->ucs.axis_z*ucs.axis_z)>tolerance)	//EPS_COS EPS_COS2 wht 10-04-20
				PlateSet.append(pPlate);
			else if(pPlate->face_N>1)
			{
				for(int i=1;i<pPlate->face_N;i++)
				{
					GEPOINT vBendPlaneNormal=pPlate->GetHuoquFaceNorm(i-1);
					if(fabs(vBendPlaneNormal*ucs.axis_z)>EPS_COS2)	
					{
						PlateSet.append(pPlate);
						break;
					}
				}
			}
		}
		//2.2 遍历钢板集合查找并统计需要添加检测线的螺栓，并按上下侧分组
		//	  使用螺栓feature属性记录螺栓所在钢板句柄，同一钢板上的螺栓不用添加标注线
		BOLTSET upBoltSet;	//统计视图上侧需要添加检测线的螺栓
		BOLTSET downBoltSet;//统计视图下侧需要添加检测线的螺栓
		CLsRef *pLsRef=NULL;
		CLDSPlate *pPlate=NULL;
		CLDSLineTube* pWeldTube;
		UCS_STRU cs=ucs;
		cs.origin=pHorizTube->pStart->xArcliftPos.x<pHorizTube->pEnd->xArcliftPos.x?pHorizTube->Start():pHorizTube->End();
		//cs.axis_y=ucs.axis_z^pLineTube->ucs.axis_z;
		//cs.axis_x=cs.axis_y^ucs.axis_z;
		//normalize(cs.axis_x);
		//normalize(cs.axis_y);
		CHashSet<CLDSLineTube*> hashBoltToTubes;	//键值是待检测螺栓，数据是螺栓相对应的焊接钢管
		for(pPlate=PlateSet.GetFirst();pPlate;pPlate=PlateSet.GetNext())
		{
			CLDSBolt *pUpMinXBolt=NULL, *pUpMaxXBolt=NULL;	//当前视图坐标系下一块钢板中X坐标最小/最大的螺栓
			CLDSBolt *pDownMinXBolt=NULL, *pDownMaxXBolt=NULL;	//当前视图坐标系下一块钢板中X坐标最小/最大的螺栓
			CMaxDouble max_x_up,max_x_down;
			CMinDouble min_x_up,min_x_down;
			UCS_STRU cs2=cs;
			if(pPlate->m_hPartWeldParent==pHorizTube->handle)
			{	//永远以水平梁管左侧为长度基点
				pWeldTube=pHorizTube;
				if(pWeldTube->pStart->xArcliftPos.x<pWeldTube->pEnd->xArcliftPos.x)
					cs2.origin=pWeldTube->Start();
				else
					cs2.origin=pWeldTube->End();
				PART_ITEM* pPartItem=SonParts.GetValue(pPlate->handle);
				CDrawingAssistant* pResidePlane=pPartItem->iResidePlaneNo>0?this->hashAssistPlanes.GetValue(pPartItem->iResidePlaneNo):NULL;
				if(pResidePlane!=NULL&&pResidePlane->m_nStyle==0&&pResidePlane->ciSpreadDirection<=2&&pResidePlane->bSpreadOverRidge)
				{
					cs2.axis_y*=-1;
					cs2.axis_z=cs2.axis_x^cs2.axis_y;
				}
			}
			else if(pPlate->m_hPartWeldParent==pJointTube->handle)
			{
				pWeldTube=pJointTube;
				cs2.axis_x=pWeldTube->End()-pWeldTube->Start();
				normalize(cs2.axis_x);
				cs2.origin=(pHorizBeamView->cbJointState&0x0f)>0?pJointTube->End():pJointTube->Start();
				double justify_axisx=cs2.axis_x*cs.axis_x;
				if(justify_axisx<0)
					cs2.axis_x*=-1;
				if(pHorizBeamView->bMasterTubeOnLeft)
				{	//水平梁管在左侧，斜对接管的长度基点应从左侧端点再向左偏移水平梁管长度
					cs2.origin-=cs2.axis_x*pHorizTube->GetLength();
				}
				cs2.axis_y=pPlate->ucs.axis_z^cs2.axis_x;
				if(cs2.axis_y*cs.axis_y<0)
					cs2.axis_y*=-1;
				PART_ITEM* pPartItem=SonParts.GetValue(pPlate->handle);
				CDrawingAssistant* pResidePlane=pPartItem->iResidePlaneNo>0?this->hashAssistPlanes.GetValue(pPartItem->iResidePlaneNo):NULL;
				if(pResidePlane!=NULL&&pResidePlane->m_nStyle==0&&pResidePlane->ciSpreadDirection<=2&&pResidePlane->bSpreadOverRidge)
					cs2.axis_y*=-1;
				cs2.axis_z=cs2.axis_x^cs2.axis_y;
				if(cs2.axis_x.x<0)
				{
					cs2.axis_x*=-1;
					cs2.axis_y*=-1;
				}
				//正(前)视图中斜对接钢管不在当前绘图平面上，绘制时需要旋转展开，因此需要相应调整cs2.axis_z
				//if(pHorizBeamView->m_cSubType=='F')
				//	cs2.axis_z=cs2.axis_x^cs.axis_y;
				normalize(cs2.axis_x);
				normalize(cs2.axis_y);
				normalize(cs2.axis_z);
			}
			else //if(pPlate->m_hPartWeldParent!=pHorizTube->handle)
			{
				pWeldTube=(CLDSLineTube*)pTower->FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
				if(pWeldTube==NULL)
					continue;
					//pWeldTube=pHorizTube;
				cs2.origin=pWeldTube->Start();
				cs2.axis_x=pWeldTube->End()-pWeldTube->Start();
				cs2.axis_y=ucs.axis_z^cs2.axis_x;
				if(cs2.axis_y*cs.axis_y<0)
					cs2.axis_y*=-1;
				cs2.axis_x=cs.axis_y^ucs.axis_z;
				normalize(cs2.axis_x);
				normalize(cs2.axis_y);
			}
			for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
			{
				f3dPoint ls_pos;
				pPlate->GetBoltIntersPos(pLsRef->GetLsPtr(),ls_pos);//->ucs.origin;
				coord_trans(ls_pos,cs2,FALSE);
				//TODO:对于对接钢管时斜管上的螺栓区分上下侧还存在隐患 wjh-2016.11.21
				if(ls_pos.y>0)
				{
					if(max_x_up.Update(ls_pos.x)==ls_pos.x)
						pUpMaxXBolt=pLsRef->GetLsPtr();
					else if(pUpMaxXBolt&&fabs(max_x_up.Update(ls_pos.x)-ls_pos.x)<2&&cs2.axis_y*(pLsRef->GetLsPtr()->ucs.origin-pUpMaxXBolt->ucs.origin)>2)
					{
						max_x_up.number=ls_pos.x;
						pUpMaxXBolt=pLsRef->GetLsPtr();
					}

					if(min_x_up.Update(ls_pos.x)==ls_pos.x)
						pUpMinXBolt=pLsRef->GetLsPtr();
					else if(pUpMinXBolt&&fabs(min_x_up.Update(ls_pos.x)-ls_pos.x)<2&&cs2.axis_y*(pLsRef->GetLsPtr()->ucs.origin-pUpMinXBolt->ucs.origin)>2)
					{
						min_x_up.number=ls_pos.x;
						pUpMinXBolt=pLsRef->GetLsPtr();
					}
				}
				else if(ls_pos.y<0)
				{
					if(max_x_down.Update(ls_pos.x)==ls_pos.x)
						pDownMaxXBolt=pLsRef->GetLsPtr();
					if(min_x_down.Update(ls_pos.x)==ls_pos.x)
						pDownMinXBolt=pLsRef->GetLsPtr();
				}
			}
			if(pUpMinXBolt)	//用feature属性来表示两螺栓是否在同一钢板上
				pUpMinXBolt->feature=pPlate->handle;
			if(pUpMaxXBolt)	//用feature属性来表示两螺栓是否在同一钢板上
				pUpMaxXBolt->feature=pPlate->handle;
			if(pUpMaxXBolt==pUpMinXBolt)
				pUpMaxXBolt=NULL;
			if(pDownMinXBolt)	//用feature属性来表示两螺栓是否在同一钢板上
				pDownMinXBolt->feature=pPlate->handle;
			if(pDownMaxXBolt)	//用feature属性来表示两螺栓是否在同一钢板上
				pDownMaxXBolt->feature=pPlate->handle;
			if(pDownMaxXBolt==pDownMinXBolt)
				pDownMaxXBolt=NULL;

			if(pUpMaxXBolt)
				hashBoltToTubes.SetValue(pUpMaxXBolt->handle,pWeldTube);
			if(pUpMinXBolt)
				hashBoltToTubes.SetValue(pUpMinXBolt->handle,pWeldTube);
			if(pDownMaxXBolt)
				hashBoltToTubes.SetValue(pDownMaxXBolt->handle,pWeldTube);
			if(pDownMinXBolt)
				hashBoltToTubes.SetValue(pDownMinXBolt->handle,pWeldTube);

			CLDSBolt *pExistBolt=NULL;
			//将Y轴正方向的左右边缘螺栓插入到上侧检测线螺栓集合中
			for(pExistBolt=upBoltSet.GetFirst();pExistBolt;pExistBolt=upBoltSet.GetNext())
			{
				f3dPoint cur_ls_pos=pExistBolt->ucs.origin;
				coord_trans(cur_ls_pos,cs,FALSE);
				BOOL pushed=upBoltSet.push_stack();
				if(pUpMinXBolt!=NULL && pUpMinXBolt->handle!=pExistBolt->handle &&
					min_x_up.number<=cur_ls_pos.x)
				{
					upBoltSet.insert(pUpMinXBolt);
					pUpMinXBolt=NULL;	//已处理
				}
				upBoltSet.pop_stack(pushed);
				pushed=upBoltSet.push_stack();
				if(pUpMaxXBolt!=NULL && pUpMaxXBolt->handle!=pExistBolt->handle &&
					max_x_up.number<=cur_ls_pos.x)
				{
					upBoltSet.insert(pUpMaxXBolt);
					pUpMaxXBolt=NULL;	//已处理
				}
				upBoltSet.pop_stack(pushed);
				if(pUpMinXBolt==NULL && pUpMaxXBolt==NULL)
					break;
			}
			if(pExistBolt==NULL && pUpMinXBolt)
				upBoltSet.append(pUpMinXBolt);
			if(pExistBolt==NULL && pUpMaxXBolt)
				upBoltSet.append(pUpMaxXBolt);
			//将Y轴负方向的左右边缘螺栓插入到上侧检测线螺栓集合中
			for(pExistBolt=downBoltSet.GetFirst();pExistBolt;pExistBolt=downBoltSet.GetNext())
			{
				f3dPoint cur_ls_pos=pExistBolt->ucs.origin;
				coord_trans(cur_ls_pos,cs,FALSE);
				BOOL pushed=downBoltSet.push_stack();
				if(pDownMinXBolt!=NULL && pDownMinXBolt->handle!=pExistBolt->handle &&
					min_x_down.number<=cur_ls_pos.x)
				{
					downBoltSet.insert(pDownMinXBolt);
					pDownMinXBolt=NULL;	//已处理
				}
				downBoltSet.pop_stack(pushed);
				pushed=downBoltSet.push_stack();
				if(pDownMaxXBolt!=NULL && pDownMaxXBolt->handle!=pExistBolt->handle &&
					max_x_down.number<=cur_ls_pos.x)
				{
					downBoltSet.insert(pDownMaxXBolt);
					pDownMaxXBolt=NULL;	//已处理
				}
				downBoltSet.pop_stack(pushed);
				if(pDownMinXBolt==NULL && pDownMaxXBolt==NULL)
					break;
			}
			if(pExistBolt==NULL && pDownMinXBolt)
				downBoltSet.append(pDownMinXBolt);
			if(pExistBolt==NULL && pDownMaxXBolt)
				downBoltSet.append(pDownMaxXBolt);
		}
#ifdef __ALFA_TEST_VERSION_
		CLogErrorLife life;
		CLDSBolt* pBolt;
		for(pBolt=upBoltSet.GetFirst();pBolt;pBolt=upBoltSet.GetNext())
			logerr.Log("0x%X",pBolt->handle);
		logerr.Log("------------------------------");
		for(pBolt=downBoltSet.GetFirst();pBolt;pBolt=downBoltSet.GetNext())
			logerr.Log("0x%X",pBolt->handle);
#endif
		//3.添加检测线
		//3.1 添加上侧检测线
		AddCheckLinesByBoltSet(this,pHorizTube,upBoltSet,TRUE,&hashBoltToTubes);
		//3.2 添加下侧检测线
		AddCheckLinesByBoltSet(this,pHorizTube,downBoltSet,FALSE,&hashBoltToTubes);
	}
	else if(pDatumPart->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube *pTempTube=NULL;
		CLDSLineTube *pLineTube=(CLDSLineTube*)pDatumPart;
		pLineTube->BuildUCS();
		if(!IsMainView()&&m_iViewType!=VIEW_FRONT&&m_iViewType!=VIEW_USERDEF)
			return;
		if(fabs(pLineTube->ucs.axis_z*ucs.axis_z)>EPS_COS)
			return;	//端面图不添加检测线
		//1.添加钢管相贯检测线
		if(IsExistIntersectTube(pLineTube,pTower))
			return AppendIntersectTubeCheckLines(pLineTube,pTower);
		//2.添加钢管始终端端头和钢管中间的连接板之间的检测线(焊前检测线和焊后检测线)
		//2.1 查找需要添加检测线的钢板
		PLATESET PlateSet;	//统计Z轴方向与当前视图方向在一条直线上的钢板
		CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;
		pStartParamPlate=(CLDSParamPlate*)pTower->FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		pEndParamPlate=(CLDSParamPlate*)pTower->FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		for(pPartItem=SonParts.GetFirst();pPartItem;pPartItem=SonParts.GetNext())
		{
			CLDSPlate *pPlate=(CLDSPlate*)pTower->FromPartHandle(pPartItem->hSonPart,CLS_PLATE);
			if(pPlate==NULL)
				continue;
			pPlate->feature=0;	//清空feature,用来记录钢板展开方向
			//排除十字插板附加钢板,避免重复添加检测线
			if(pStartParamPlate&&pStartParamPlate->m_iParamType==TYPE_XEND&&
				pStartParamPlate->IsInAttachPartList(pPlate->handle))
				continue;
			if(pEndParamPlate&&pEndParamPlate->m_iParamType==TYPE_XEND&&
				pEndParamPlate->IsInAttachPartList(pPlate->handle))
				continue;
			if( pPlate->GetBaseNode()==NULL&&pPlate->designInfo.partList.GetNodeNum()==0&&
				pPlate->m_hPartWeldParent!=pLineTube->handle)
				continue;	//添加焊后检测线时，应排除焊接父构件不是钢管且无基准节点的钢板(如：爬梯连接板) wht 18-08-28
			//cos0.5°= 0.99996192306417128873735516482698
			//double cosa=0.98480775301220805936674302458952;	//wht 10-08-15 使用EPS_COS精确度过高改为cos0.5°
			//生成视图时使用EPS_COS2判断是否共面，此处也应使用EPS_COS2判断,
			//否则会导致即未单独为某些钢板添加独立的视图，此处也未标注相关检测线 wht 11-07-18
			if(fabs(pPlate->ucs.axis_z*ucs.axis_z)>EPS_COS2)	//EPS_COS EPS_COS2 wht 10-04-20
				PlateSet.append(pPlate);
			else if(pPlate->face_N>1)
			{
				for(int i=1;i<pPlate->face_N;i++)
				{
					GEPOINT vBendPlaneNormal=pPlate->GetHuoquFaceNorm(i-1);
					if(fabs(vBendPlaneNormal*ucs.axis_z)>EPS_COS2)	
					{
						PlateSet.append(pPlate);
						break;
					}
				}
			}
			else if(pPartItem->iResidePlaneNo>0)
			{
				CDrawingAssistant* pResidePlane=this->hashAssistPlanes.GetValue(pPartItem->iResidePlaneNo);
				if(pResidePlane&&pResidePlane->m_nStyle==0&&pResidePlane->ciSpreadDirection<=2)
				{
					pPlate->feature=pResidePlane->ciSpreadDirection;	//记录钢板展开方向 wht 18-03-12
					PlateSet.append(pPlate);
				}
			}
		}
		//2.2 遍历钢板集合查找并统计需要添加检测线的螺栓，并按上下侧分组
		//	  使用螺栓feature属性记录螺栓所在钢板句柄，同一钢板上的螺栓不用添加标注线
		BOLTSET upBoltSet;	//统计视图上侧需要添加检测线的螺栓
		BOLTSET downBoltSet;//统计视图下侧需要添加检测线的螺栓
		CLsRef *pLsRef=NULL;
		CLDSPlate *pPlate=NULL;
		UCS_STRU cs=ucs;
		cs.origin=pLineTube->Start();
		cs.axis_y=ucs.axis_z^pLineTube->ucs.axis_z;
		cs.axis_x=cs.axis_y^ucs.axis_z;
		normalize(cs.axis_x);
		normalize(cs.axis_y);
		CHashSet<CLDSLineTube*> hashBoltToTubes;	//键值是待检测螺栓，数据是螺栓相对应的焊接钢管
		for(pPlate=PlateSet.GetFirst();pPlate;pPlate=PlateSet.GetNext())
		{
			CLDSBolt *pUpMinXBolt=NULL, *pUpMaxXBolt=NULL;	//当前视图坐标系下一块钢板中X坐标最小/最大的螺栓
			CLDSBolt *pDownMinXBolt=NULL, *pDownMaxXBolt=NULL;	//当前视图坐标系下一块钢板中X坐标最小/最大的螺栓
			CMaxDouble max_x_up,max_x_down;
			CMinDouble min_x_up,min_x_down;
			UCS_STRU cs2=cs;
			if(pPlate->m_hPartWeldParent!=pLineTube->handle)
			{
				pTempTube=(CLDSLineTube*)pTower->FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
				if(pTempTube==NULL)
					pTempTube=pLineTube;
				cs2.origin=pTempTube->Start();
				cs2.axis_y=ucs.axis_z^pTempTube->ucs.axis_z;
				if(cs2.axis_y*cs.axis_y<0)
					cs2.axis_y*=-1;
				cs2.axis_x=cs.axis_y^ucs.axis_z;
				normalize(cs2.axis_x);
				normalize(cs2.axis_y);
			}
			else
				pTempTube=pLineTube;
			for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
			{
				f3dPoint ls_pos;
				pPlate->GetBoltIntersPos(pLsRef->GetLsPtr(),ls_pos);//->ucs.origin;
				coord_trans(ls_pos,cs2,FALSE);
				//TODO:对于对接钢管时斜管上的螺栓区分上下侧还存在隐患 wjh-2016.11.21
				bool bUp=(pPlate->feature==CSectionView::PART_ITEM::SPREAD_TO_UPPER)||(pPlate->feature==0&&ls_pos.y>0);
				bool bDown=(pPlate->feature==CSectionView::PART_ITEM::SPREAD_TO_UNDER)||(pPlate->feature==0&&ls_pos.y<0);
				if(bUp)
				{
					if(max_x_up.Update(ls_pos.x)==ls_pos.x)
						pUpMaxXBolt=pLsRef->GetLsPtr();
					else if(pUpMaxXBolt&&fabs(max_x_up.Update(ls_pos.x)-ls_pos.x)<2&&cs2.axis_y*(pLsRef->GetLsPtr()->ucs.origin-pUpMaxXBolt->ucs.origin)>2)
					{
						max_x_up.number=ls_pos.x;
						pUpMaxXBolt=pLsRef->GetLsPtr();
					}

					if(min_x_up.Update(ls_pos.x)==ls_pos.x)
						pUpMinXBolt=pLsRef->GetLsPtr();
					else if(pUpMinXBolt&&fabs(min_x_up.Update(ls_pos.x)-ls_pos.x)<2&&cs2.axis_y*(pLsRef->GetLsPtr()->ucs.origin-pUpMinXBolt->ucs.origin)>2)
					{
						min_x_up.number=ls_pos.x;
						pUpMinXBolt=pLsRef->GetLsPtr();
					}
				}
				else if(bDown)
				{
					if(max_x_down.Update(ls_pos.x)==ls_pos.x)
						pDownMaxXBolt=pLsRef->GetLsPtr();
					if(min_x_down.Update(ls_pos.x)==ls_pos.x)
						pDownMinXBolt=pLsRef->GetLsPtr();
				}
			}
			if(pUpMinXBolt)	//用feature属性来表示两螺栓是否在同一钢板上
				pUpMinXBolt->feature=pPlate->handle;
			if(pUpMaxXBolt)	//用feature属性来表示两螺栓是否在同一钢板上
				pUpMaxXBolt->feature=pPlate->handle;
			if(pUpMaxXBolt==pUpMinXBolt)
				pUpMaxXBolt=NULL;
			if(pDownMinXBolt)	//用feature属性来表示两螺栓是否在同一钢板上
				pDownMinXBolt->feature=pPlate->handle;
			if(pDownMaxXBolt)	//用feature属性来表示两螺栓是否在同一钢板上
				pDownMaxXBolt->feature=pPlate->handle;
			if(pDownMaxXBolt==pDownMinXBolt)
				pDownMaxXBolt=NULL;
			if(pTempTube&&pTempTube!=pLineTube)
			{
				if(pUpMaxXBolt)
					hashBoltToTubes.SetValue(pUpMaxXBolt->handle,pTempTube);
				if(pUpMinXBolt)
					hashBoltToTubes.SetValue(pUpMinXBolt->handle,pTempTube);
				if(pDownMaxXBolt)
					hashBoltToTubes.SetValue(pDownMaxXBolt->handle,pTempTube);
				if(pDownMinXBolt)
					hashBoltToTubes.SetValue(pDownMinXBolt->handle,pTempTube);
			}
			CLDSBolt *pExistBolt=NULL;
			//将Y轴正方向的左右边缘螺栓插入到上侧检测线螺栓集合中
			for(pExistBolt=upBoltSet.GetFirst();pExistBolt;pExistBolt=upBoltSet.GetNext())
			{
				f3dPoint cur_ls_pos=pExistBolt->ucs.origin;
				coord_trans(cur_ls_pos,cs,FALSE);
				BOOL pushed=upBoltSet.push_stack();
				if(pUpMinXBolt!=NULL && pUpMinXBolt->handle!=pExistBolt->handle &&
					min_x_up.number<=cur_ls_pos.x)
				{
					upBoltSet.insert(pUpMinXBolt);
					pUpMinXBolt=NULL;	//已处理
				}
				upBoltSet.pop_stack(pushed);
				pushed=upBoltSet.push_stack();
				if(pUpMaxXBolt!=NULL && pUpMaxXBolt->handle!=pExistBolt->handle &&
					max_x_up.number<=cur_ls_pos.x)
				{
					upBoltSet.insert(pUpMaxXBolt);
					pUpMaxXBolt=NULL;	//已处理
				}
				upBoltSet.pop_stack(pushed);
				if(pUpMinXBolt==NULL && pUpMaxXBolt==NULL)
					break;
			}
			if(pExistBolt==NULL && pUpMinXBolt)
				upBoltSet.append(pUpMinXBolt);
			if(pExistBolt==NULL && pUpMaxXBolt)
				upBoltSet.append(pUpMaxXBolt);
			//将Y轴负方向的左右边缘螺栓插入到上侧检测线螺栓集合中
			for(pExistBolt=downBoltSet.GetFirst();pExistBolt;pExistBolt=downBoltSet.GetNext())
			{
				f3dPoint cur_ls_pos=pExistBolt->ucs.origin;
				coord_trans(cur_ls_pos,cs,FALSE);
				BOOL pushed=downBoltSet.push_stack();
				if(pDownMinXBolt!=NULL && pDownMinXBolt->handle!=pExistBolt->handle &&
					min_x_down.number<=cur_ls_pos.x)
				{
					downBoltSet.insert(pDownMinXBolt);
					pDownMinXBolt=NULL;	//已处理
				}
				downBoltSet.pop_stack(pushed);
				pushed=downBoltSet.push_stack();
				if(pDownMaxXBolt!=NULL && pDownMaxXBolt->handle!=pExistBolt->handle &&
					max_x_down.number<=cur_ls_pos.x)
				{
					downBoltSet.insert(pDownMaxXBolt);
					pDownMaxXBolt=NULL;	//已处理
				}
				downBoltSet.pop_stack(pushed);
				if(pDownMinXBolt==NULL && pDownMaxXBolt==NULL)
					break;
			}
			if(pExistBolt==NULL && pDownMinXBolt)
				downBoltSet.append(pDownMinXBolt);
			if(pExistBolt==NULL && pDownMaxXBolt)
				downBoltSet.append(pDownMaxXBolt);
		}
		//3.添加检测线
		//3.1 添加上侧检测线
		AddCheckLinesByBoltSet(this,pLineTube,upBoltSet,TRUE,&hashBoltToTubes);
		//3.2 添加下侧检测线
		AddCheckLinesByBoltSet(this,pLineTube,downBoltSet,FALSE,&hashBoltToTubes);
	}
	else if(pDatumPart->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPart *pPart=NULL;
		CLDSPlate *pSonPlate=NULL;
		CLDSGroupLineAngle *pGroupLineAngle=NULL;
		CLDSPlate *pDatumPlate=(CLDSPlate*)pDatumPart;
		pPart=pTower->FromPartHandle(pDatumPlate->designInfo.m_hBasePart);
		if(pPart&&pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//为组合角钢填板添加默认的检测线
			pGroupLineAngle=(CLDSGroupLineAngle*)pPart;
			f3dPoint len_vec=pGroupLineAngle->Start()-pGroupLineAngle->End();
			normalize(len_vec);
			if(fabs(len_vec*ucs.axis_z)>EPS_COS2)
				return;	//端面图不需要检测线
			for(pPartItem=SonParts.GetFirst();pPartItem;pPartItem=SonParts.GetNext())
			{
				pSonPlate=(CLDSPlate*)pTower->FromPartHandle(pPartItem->hSonPart,CLS_PLATE);
				if(pSonPlate&&pSonPlate->designInfo.m_hBaseNode==pDatumPlate->designInfo.m_hBaseNode
					&&pSonPlate->designInfo.m_hBasePart==pDatumPlate->designInfo.m_hBasePart&&pSonPlate->handle!=pDatumPlate->handle)
					break;
			}
			if(pSonPlate)
			{	
				f3dPoint vec;
				double min_dist1=10000,min_dist2=10000;
				CLDSBolt *pBolt1=NULL,*pBolt2=NULL;
				for(CLsRef *pLsRef=pDatumPlate->GetFirstLsRef();pLsRef;pLsRef=pDatumPlate->GetNextLsRef())
				{
					vec=(*pLsRef)->ucs.origin-pSonPlate->ucs.origin;
					normalize(vec);
					if(vec*pSonPlate->ucs.axis_z>0)
					{
						f3dPoint base_pt=(*pLsRef)->ucs.origin;
						project_point(base_pt,pSonPlate->ucs.origin,pSonPlate->ucs.axis_z);
						double dist=DISTANCE((*pLsRef)->ucs.origin,base_pt);
						if(dist<min_dist1)
						{
							min_dist1=dist;
							pBolt1=pLsRef->GetLsPtr();
						}
					}
					else 
					{
						f3dPoint base_pt=(*pLsRef)->ucs.origin;
						project_point(base_pt,pSonPlate->ucs.origin,-pSonPlate->ucs.axis_z);
						double dist=DISTANCE((*pLsRef)->ucs.origin,base_pt);
						if(dist<min_dist2)
						{
							min_dist2=dist;
							pBolt2=pLsRef->GetLsPtr();
						}
					}
				}
				if(pBolt1)
				{
					pCheckLine= CheckLines.append();
					pCheckLine->m_bBeforeWeld=TRUE;
					pCheckLine->m_hStartPart=pBolt1->handle;
					pCheckLine->m_hEndPart=pSonPlate->handle;
				}
				if(pBolt2)
				{
					pCheckLine= CheckLines.append();
					pCheckLine->m_bBeforeWeld=TRUE;
					pCheckLine->m_hStartPart=pBolt2->handle;
					pCheckLine->m_hEndPart=pSonPlate->handle;
				}
			}
		}
	}
}

//根据视图类型查找当前视图描述的主构件
CLDSPart *CSectionView::GetMainPartHandle(IModel *pModel)
{
	PART_ITEM *pItem=NULL;
	//一个视图可能描述多块指定类型的构件 如上、下封板
	//取舍依据:将构件原点转换到视图坐标系下取Z坐标较大的构件为主构件
	CLDSPart *pPart=NULL,*pPrevPart=NULL,*pMainPart=NULL; 
	if(m_iViewType==VIEW_FL)
	{
		CLDSLineTube* pDrawTube=NULL;
		for(pItem=SonParts.GetFirst();pItem;pItem=SonParts.GetNext())
		{
			pPart=pModel->FromPartHandle(pItem->hSonPart);
			if(pDrawTube==NULL&&pPart->GetClassTypeId()==CLS_LINETUBE)
				pDrawTube=(CLDSLineTube*)pPart;
			else if(pMainPart==NULL&&pPart->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pPart)->IsFL())
				return (pMainPart=(CLDSParamPlate*)pPart);
		}
		for(pItem=SonParts.GetFirst();pItem&&pDrawTube;pItem=SonParts.GetNext())
		{
			pPart=pModel->FromPartHandle(pItem->hSonPart);
			if(pPart&&!pPart->IsPlate())
				continue;
			CSuperSmartPtr<CLDSGeneralPlate> pPlate=(CLDSGeneralPlate*)pPart;
			if(pPlate->GetBaseNode()!=pDrawTube->pStart&&pPlate->GetBaseNode()!=pDrawTube->pEnd)
				continue;
			if(pPlate->GetClassTypeId()==CLS_PLATE)
			{
				if(pPlate.pPlate->m_fInnerRadius<EPS)
					continue;
				return pPlate;
			}
			else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
			{
				double B=0;
				if(  pPlate.pParamPlate->m_iParamType!=TYPE_CIRRIBPLATE||
					!pPlate.pParamPlate->GetDesignItemValue('B',&B,NULL)||fabs(B-360)>EPS_COS)	//B:β,环向扇形角(外形参数)
					continue;
				return pPlate;
			}
		}
	}
	for(pItem=SonParts.GetFirst();pItem;pItem=SonParts.GetNext())
	{
		pPart=pModel->FromPartHandle(pItem->hSonPart);
		if(pPart==NULL)
			continue;
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
			int iParamType=pParamPlate->m_iParamType;
			if(iParamType==TYPE_CIRRIBPLATE)
				iParamType=TYPE_TUBERIBPLATE;	//跨越连板与环向肋板视同为同一类型
			if((m_iViewType==VIEW_CIRCOVERPLATE&&pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)//封板视图
				||(m_iViewType==VIEW_CIRRIBPLATE&&pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)//肋板视图
				||(m_iViewType==VIEW_TUBERIBPLATE&&iParamType==TYPE_TUBERIBPLATE)//跨越钢管连板视图
				||(m_iViewType==VIEW_FL&&(pParamPlate->IsFL()))	//法兰视图
				||(m_iViewType==VIEW_INSERTPLATE&&(pParamPlate->IsInsertPlate()))//插板视图
				||(m_iViewType==VIEW_NODEPLATE&&pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				||(m_iViewType==VIEW_NODEPLATE&&iParamType==TYPE_TUBERIBPLATE))
			{
				if(pMainPart)
				{	//将构件原点转换到视图坐标系下取Z坐标较大的构件为主构件
					f3dPoint main_pos=pMainPart->ucs.origin;
					f3dPoint cur_pos=pPart->ucs.origin;
					coord_trans(main_pos,ucs,FALSE);
					coord_trans(cur_pos,ucs,FALSE);
					if (main_pos.z<cur_pos.z)
						pMainPart=pPart;
				}
				else 
					pMainPart=pPart;
			}
		}
		else if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			if((m_iViewType==VIEW_NODEPLATE&&(pPlate->designInfo.m_hBaseNode>0x20||pMainPart==NULL))
				||(m_iViewType==VIEW_CIRPLATE&&pPlate->m_fInnerRadius>0)||m_iViewType==VIEW_FL)
				pMainPart=pPart;//VIEW_FL视图时也存在底脚普通钢板作法兰情况
		}
		else if(pPart->IsLinePart())
		{
			if( (m_iViewType==VIEW_FOOTNAILANGLE&&pPart->GetClassTypeId()==CLS_LINEANGLE)||
				(m_iViewType==VIEW_FOOTNAILSLOT&&pPart->GetClassTypeId()==CLS_LINESLOT))
				pMainPart=pPart;
		}
	}
	return pMainPart;
}

//添加指定构件到当前视图构件链表	wht 10-08-10
CSectionView::PART_ITEM *CSectionView::AddPart(long hSonPart,CLDSPart *pSonPart/*=NULL*/,int cFlag/*=0*/)
{
	CSectionView::PART_ITEM *pPartItem = NULL;
	if(pSonPart)
		hSonPart=pSonPart->handle;
	pPartItem = SonParts.Add(hSonPart);
	pPartItem->cFlag = cFlag;
	LoadTagInfo(pPartItem)->pCurPart = pSonPart;
	return pPartItem;
}
void CSectionView::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteString(sName);
	buffer.WriteInteger(m_bLocked);	//锁定视图
	buffer.WriteInteger(m_iViewType);//视图类型
	buffer.WriteInteger(index);		//视图索引
	buffer.WritePoint(ucs.origin);
	buffer.WritePoint(ucs.axis_x);
	buffer.WritePoint(ucs.axis_y);
	buffer.WritePoint(ucs.axis_z);
	if( version==0||
		(doc_type==1&&version>=4010200)||
		(doc_type==2&&version>=1010200)||
		(doc_type==4&&version>=1020003)||
		(doc_type==5&&version>=1020003))
		des_cs.ToBuffer(buffer,version,doc_type);
	BUFFERPOP stack(&buffer,SonParts.GetNodeNum());
	buffer.WriteInteger(SonParts.GetNodeNum());
	for(CSectionView::PART_ITEM *pItem=SonParts.GetFirst();pItem;pItem=SonParts.GetNext())
	{
		buffer.WriteInteger(pItem->hSonPart);
		buffer.WriteByte(pItem->cFlag);
		if( version==0||
		(doc_type==1&&version>=4010200)||
		(doc_type==2&&version>=1010200)||
		(doc_type==4&&version>=1020101)||
		(doc_type==5&&version>=1020101))
			buffer.WriteInteger(pItem->uiSpreadPlane);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of son part is wrong!");
#else
		CLDSObject::Log2File()->Log("子构件记录数出现错误!");
#endif

	buffer.WriteInteger(LocatePoints.GetNodeNum());
	for(CPartGroupLocatePoint *pLocatePoint=LocatePoints.GetFirst();pLocatePoint;pLocatePoint=LocatePoints.GetNext())
	{
		pLocatePoint->ToBuffer(buffer,version,doc_type);
	}
	buffer.WriteInteger(CheckLines.GetNodeNum());
	for(CCheckLineDimension *pCheckLine=CheckLines.GetFirst();pCheckLine;pCheckLine=CheckLines.GetNext())
	{
		pCheckLine->ToBuffer(buffer,version,doc_type);
	}
	buffer.WriteInteger(Angulars.GetNodeNum());
	for(CAngularDimension *pAngular=Angulars.GetFirst();pAngular;pAngular=Angulars.GetNext())
	{
		pAngular->ToBuffer(buffer,version,doc_type);
	}
	if( version==0||
	(doc_type==1&&version>=4010200)||
	(doc_type==2&&version>=1010200)||
	(doc_type==4&&version>=1020101)||
	(doc_type==5&&version>=1020101))
	{
		buffer.WriteInteger(hashAssistPlanes.GetNodeNum());
		for(CDrawingAssistant* pAssist=hashAssistPlanes.GetFirst();pAssist;pAssist=hashAssistPlanes.GetNext())
			pAssist->ToBuffer(buffer,version,doc_type);
	}
}

void CSectionView::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[500]="";
	fprintf(fp,"<视图信息 sName=\"%s\">\n",sName);
	if(m_bLocked==1)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<锁定视图 bLocked=\"%s\"/>\n",sText);
	fprintf(fp,"<视图类型 m_iViewType=\"%d\"/>\n",m_iViewType);
	fprintf(fp,"<视图索引 index=\"%d\"/>\n",index);
	fprintf(fp,"<原点坐标 origin=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.origin.x,ucs.origin.y,ucs.origin.z);
	fprintf(fp,"<X轴坐标 axis_x=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.axis_x.x,ucs.axis_x.y,ucs.axis_x.z);
	fprintf(fp,"<Y轴坐标 axis_y=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.axis_y.x,ucs.axis_y.y,ucs.axis_y.z);
	fprintf(fp,"<Z轴坐标 axis_z=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.axis_z.x,ucs.axis_z.y,ucs.axis_z.z);
	des_cs.ToXmlFile(fp,schema);
	fprintf(fp,"<子构件数量 SonPartsNum=\"%d\"/>\n",SonParts.GetNodeNum());
	for(CSectionView::PART_ITEM *pItem=SonParts.GetFirst();pItem;pItem=SonParts.GetNext())
		fprintf(fp,"<子构件信息 handle=\"0x%x\" Flag=\"%c\"/>\n",pItem->hSonPart,pItem->cFlag);
	fprintf(fp,"<LocatePoint num=\"%d\"/>\n",LocatePoints.GetNodeNum());
	for(CPartGroupLocatePoint *pLocatePoint=LocatePoints.GetFirst();pLocatePoint;pLocatePoint=LocatePoints.GetNext())
		pLocatePoint->ToXmlFile(fp,schema);
	fprintf(fp,"<CheckLines num=\"%d\"/>\n",CheckLines.GetNodeNum());
	for(CCheckLineDimension *pCheckLine=CheckLines.GetFirst();pCheckLine;pCheckLine=CheckLines.GetNext())
		pCheckLine->ToXmlFile(fp,schema);
	fprintf(fp,"<Angulars num=\"%d\"/>\n",Angulars.GetNodeNum());
	for(CAngularDimension *pAngular=Angulars.GetFirst();pAngular;pAngular=Angulars.GetNext())
		pAngular->ToXmlFile(fp,schema);
	fprintf(fp,"</视图信息>\n");
}
void CSectionView::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	buffer.ReadString(sName,51);
	buffer.ReadInteger(&m_bLocked);	//锁定视图
	/*#ifdef HAICHENGYINFENG	//海城银烽特殊需要塔脚板折叠板等
	#if defined(__TMA_FILE_)||defined(__TMA_)
	if(version==0||version>=4000024)//version!=NULL&&compareVersion(version,"4.0.0.24")>=0)
	#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(version==0||version>=1000078)//version!=NULL&&compareVersion(version,"1.0.0.78")>=0)
	#else
	if(version==0||version>=1000006)//version!=NULL&&compareVersion(version,"1.0.0.6")>=0)
	#endif
	#else*/
	/*#if defined(__TMA_FILE_)||defined(__TMA_)
	if(version==0||version>=4000024)//version!=NULL&&compareVersion(version,"4.0.0.24")>=0)
	#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(version==0||version>=1000079)//version!=NULL&&compareVersion(version,"1.0.0.79")>=0)
	#elif defined(__TDA_FILE_)
	if(version==0||version>=1000001)
	#else
	if(version==0||version>=1000006)//version!=NULL&&compareVersion(version,"1.0.0.6")>=0)
	#endif*/
	//#endif
	if( version==0||((doc_type==1&&version>=4000024)||
		(doc_type==2&&version>=1000079)||(doc_type==4&&version>=1000006)||doc_type==5))
	{	//视图类型
		int iType=0;
		buffer.ReadInteger(&iType);
		if(iType==1)
			m_iViewType=VIEW_MAIN;
		else if(iType==2)
			m_iViewType=VIEW_FRONT;
		else if(iType==3)
			m_iViewType=VIEW_NODEPLATE;
		else if(iType==4)
			m_iViewType=VIEW_CIRCOVERPLATE;
		else if(iType==5)
			m_iViewType=VIEW_CIRRIBPLATE;
		else if(iType==6)
			m_iViewType=VIEW_TUBERIBPLATE;
		else if(iType==7)
			m_iViewType=VIEW_FL;
		else if(iType==8)
			m_iViewType=VIEW_CIRPLATE;
		else if(iType==9)
			m_iViewType=VIEW_INSERTPLATE;
		else if(iType==10)
			m_iViewType=VIEW_FOOTNAILANGLE;
		else if(iType==11)
			m_iViewType=VIEW_FOOTNAILSLOT;
		else if(iType==12)
			m_iViewType=VIEW_FOOT;
		else if(iType==13)
			m_iViewType=VIEW_JOINSECTION;
		else if(iType==14)	//对接钢管简图
			m_iViewType=VIEW_JOINTTUBE;
		else if(iType==15)	//K节点相贯简图
			m_iViewType=VIEW_KNODESKETCH;
		else
			m_iViewType=VIEW_USERDEF;
		buffer.ReadInteger(&index);
	}
	buffer.ReadPoint(ucs.origin);
	buffer.ReadPoint(ucs.axis_x);
	buffer.ReadPoint(ucs.axis_y);
	buffer.ReadPoint(ucs.axis_z);
	if( version==0||
		(doc_type==1&&version>=4010200)||
		(doc_type==2&&version>=1010200)||
		(doc_type==4&&version>=1020003)||
		(doc_type==5&&version>=1020003))
	{
		des_cs.FromBuffer(buffer,version,doc_type);
	}
	else
	{
		CParaCS::PARAMETRIC_INFO param=des_cs.GetParamInfo();
		param.desOrg.datum_pos_style=0;
		param.desOrg.datum_pos=ucs.origin;
		param.desAxisX.norm_style=0;
		param.desAxisX.vector=ucs.axis_x;
		param.desAxisY.norm_style=0;
		param.desAxisY.vector=ucs.axis_y;
		param.desAxisZ.norm_style=0;
		param.desAxisZ.vector=ucs.axis_z;
		des_cs.SetParamInfo(param);
	}
	int j,nn;
	buffer.ReadInteger(&nn);
	for(j=0;j<nn;j++)
	{
		long hSonPart=0;
		buffer.ReadInteger(&hSonPart);
		CSectionView::PART_ITEM *pItem,item;
		if(hSonPart<=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Exist sub part whose handle is null in group weld part！");
#else
			logerr.Log("组焊件中存在零句柄子项构件！");
#endif
			pItem=&item;
		}
		else
			pItem=SonParts.Add(hSonPart);
		buffer.ReadByte(&pItem->cFlag);
		if( version==0||
		(doc_type==1&&version>=4010200)||
		(doc_type==2&&version>=1010200)||
		(doc_type==4&&version>=1020101)||
		(doc_type==5&&version>=1020101))
		buffer.ReadInteger(&pItem->uiSpreadPlane);
	}
	buffer.ReadInteger(&nn);
	for(j=0;j<nn;j++)
	{
		CPartGroupLocatePoint *pLocatePoint=LocatePoints.append();
		pLocatePoint->FromBuffer(buffer,version);
	}
	buffer.ReadInteger(&nn);
	for(j=0;j<nn;j++)
	{
		CCheckLineDimension *pCheckLine=CheckLines.append();
		pCheckLine->FromBuffer(buffer,version);
	}
	buffer.ReadInteger(&nn);
	for(j=0;j<nn;j++)
	{
		CAngularDimension *pAngular=Angulars.append();
		pAngular->FromBuffer(buffer,version);
	}
	if( version==0||
		(doc_type==1&&version>=4010200)||
		(doc_type==2&&version>=1010200)||
		(doc_type==4&&version>=1020101)||
		(doc_type==5&&version>=1020101))
	{
		buffer.ReadInteger(&nn);
		for(j=0;j<nn;j++)
		{
			CDrawingAssistant* pAssist=hashAssistPlanes.Add(0);
			pAssist->FromBuffer(buffer,version,doc_type);
			hashAssistPlanes.ModifyKeyId(hashAssistPlanes.GetCursorKey(),pAssist->iNo);
			if(BelongPartGroup())
			{	//实时值在序列化时不存储, 需要在此时根据设计参数重新计算更新
				pAssist->m_xFaceNorm.UpdateVector(BelongPartGroup()->BelongModel());
				pAssist->m_xLineStart.UpdatePos(BelongPartGroup()->BelongModel());
			}
		}
	}
	LocatePoints.Empty();//不存储,只在出图时临时生成,否则这部分代码修改后,必须在设计环境下重新统计组焊件
	CheckLines.Empty();	//不存储,只在出图时临时生成,否则这部分代码修改后,必须在设计环境下重新统计组焊件
	Angulars.Empty();	//不存储,只在出图时临时生成,否则这部分代码修改后,必须在设计环境下重新统计组焊件
}

void CSectionView::CopyProperty(CSectionView *pSrcView)
{
	if(pSrcView==NULL)
		return;
	strcpy(sName,pSrcView->sName);
	des_cs=pSrcView->des_cs;
	ucs=pSrcView->ucs;
	m_iViewType=pSrcView->m_iViewType;
	m_bLocked=pSrcView->m_bLocked;
	SonParts.Empty();
	for(PART_ITEM *pItem=pSrcView->SonParts.GetFirst();pItem;pItem=pSrcView->SonParts.GetNext())
		SonParts.SetValue(pItem->hSonPart,*pItem);
}
UCS_STRU CSectionView::UpdateUCS(IModel *pModel,CLDSPartGroup *pPartGroup)
{
	des_cs.SetModifiedState();
	if(des_cs.UpdateACS(pModel))
		ucs=des_cs.GetACS();
	else 
	{
		CXhChar100 sGroupName;
		if(pPartGroup)
			sGroupName.Copy(pPartGroup->sGroupName);
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("【%s】 group weld part 【%s】 view's UCS wrong calculates！",(char*)sGroupName,sName);
#else
		logerr.Log("【%s】组焊件【%s】视图坐标系计算错误！",(char*)sGroupName,sName);
#endif
	}
	return ucs;
}
//模型坐标与绘图坐标间的变换
GECS CSectionView::TransRotationsToCS(IXhEnumerator *pIter,bool bRotRightHand/*=true*/,bool bFowardIter/*=true*/)
{
	UINT i=0,count=pIter->Count;
	ROTATION* pRotation;
	DYN_ARRAY<ROTATION> rotations(pIter->Count);;
	for(pRotation=(ROTATION*)pIter->MoveFirst();pRotation;pRotation=(ROTATION*)pIter->MoveNext(),i++)
		rotations[bFowardIter?i:count-i-1]=*pRotation;
	GECS cs;
	cs.InitStdCS();
	for(i=0;i<count;i++)
	{
		GEPOINT axis=rotations[i].xRotAxisEnd-rotations[i].xRotAxisStart;
		rotate_point_around_axis(cs.origin,rotations[i].m_fRotAngle,rotations[i].xRotAxisStart,rotations[i].xRotAxisEnd);
		double sina=sin(rotations[i].m_fRotAngle),cosa=cos(rotations[i].m_fRotAngle);
		if(!bRotRightHand)
			sina*=-1;
		RotateVectorAroundVector(cs.axis_x,sina,cosa,axis);
		RotateVectorAroundVector(cs.axis_y,sina,cosa,axis);
		RotateVectorAroundVector(cs.axis_z,sina,cosa,axis);
	}
	return cs;
}
bool CSectionView::GetSpreadPlaneRotCS(int iAssistPlaneNo,GECS* pRotCS,IModel *pTower/*=NULL*/)
{
	ROTATION rotation;
	CXhSimpleList<ROTATION> rotations;
	CDrawingAssistant *pAssistPlane,*pParentPlane;
	if((pAssistPlane=iAssistPlaneNo>0?hashAssistPlanes.GetValue(iAssistPlaneNo):NULL)==NULL)
		return false;
	if(pAssistPlane->m_nStyle!=0&&pAssistPlane->m_nStyle!=3)
		return false;
	pAssistPlane->m_xFaceNorm.UpdateVector(pTower);
	pAssistPlane->m_xLineStart.UpdatePos(pTower);
	if(pAssistPlane->m_nStyle==0&&pAssistPlane->wStyleFlag&CDrawingAssistant::ENABLE_LINE_END)
		pAssistPlane->m_xLineEnd.UpdatePos(pTower);
	while(pAssistPlane!=NULL)
	{
		pParentPlane=NULL;
		rotation.xRotAxisStart=pAssistPlane->m_xLineStart.Position();
		if(pAssistPlane->m_nStyle==3)
		{	//旋转展开面是绕任意轴旋转时应重新计算旋转后基面的法线
			GEPOINT vRotAxisVec=pAssistPlane->m_xFaceNorm.vector;
			rotation.m_fRotAngle=pAssistPlane->m_fRotRadAng;
			rotation.xRotAxisEnd=rotation.xRotAxisStart+vRotAxisVec*1000;
		}
		else
		{
			GEPOINT vSpreadNormal,vAxisVec;
			vSpreadNormal=pAssistPlane->m_xFaceNorm.vector;
			if(pAssistPlane->wStyleFlag&CDrawingAssistant::ENABLE_LINE_END)
			{
				rotation.xRotAxisEnd  =pAssistPlane->m_xLineEnd.Position();
				vAxisVec=rotation.xRotAxisEnd-rotation.xRotAxisStart;
			}
			//project_vector(vSpreadNormal,vAxisVec);
			if((pParentPlane=pAssistPlane->iResidePlaneNo>0?hashAssistPlanes.GetValue(pAssistPlane->iResidePlaneNo):NULL)!=NULL)
			{
				pParentPlane->m_xFaceNorm.UpdateVector(pTower);
				pParentPlane->m_xLineStart.UpdatePos(pTower);
				if(pParentPlane->m_nStyle==0&&(pParentPlane->wStyleFlag&CDrawingAssistant::ENABLE_LINE_END))
					pParentPlane->m_xLineEnd.UpdatePos(pTower);
			}
			GEPOINT vBasePlaneAxisZ=ucs.axis_z;
			if(pParentPlane!=NULL&&pParentPlane->m_nStyle==0)
				vBasePlaneAxisZ=pParentPlane->m_xFaceNorm.vector;
			else if(pParentPlane!=NULL&&pParentPlane->m_nStyle==3)
			{	//寄生的旋转展开面是绕任意轴旋转时应重新计算旋转后基面的法线
				double cosa= cos(pParentPlane->m_fRotRadAng);
				double sina=-SQRT(1-cosa*cosa);	//反向旋转加负号
				RotateVectorAroundVector(vBasePlaneAxisZ,sina,cosa,pParentPlane->m_xFaceNorm.vector);
			}

			GEPOINT axis =vSpreadNormal^vBasePlaneAxisZ;
			rotation.m_fRotAngle=cal_angle_of_2vec(vSpreadNormal,vBasePlaneAxisZ);
			if(pAssistPlane->ciSpreadDirection==1||pAssistPlane->ciSpreadDirection==2)
			{
				if(pAssistPlane->bSpreadOverRidge&&rotation.m_fRotAngle<1.5707)	//Pi/2
					rotation.m_fRotAngle+=Pi;
				else if(!pAssistPlane->bSpreadOverRidge&&rotation.m_fRotAngle>1.5707)
					rotation.m_fRotAngle-=Pi;
			}
			if(pAssistPlane->ciSpreadDirection==0&&rotation.m_fRotAngle>1.5707963)	//Pi/2=3.1415926/2=1.5707963
				rotation.m_fRotAngle-=Pi;	//保证默认最小角度旋转展开
			if((pAssistPlane->wStyleFlag&CDrawingAssistant::ENABLE_LINE_END)!=0&&axis*vAxisVec<0)
				rotation.m_fRotAngle*=-1;
			else if(!(pAssistPlane->wStyleFlag&CDrawingAssistant::ENABLE_LINE_END))
			{
				normalize(axis);
				rotation.xRotAxisEnd=rotation.xRotAxisStart+axis*1000;
			}
		}
		ROTATION* pRot=rotations.AttachObject(rotation);
		pAssistPlane=pParentPlane;
	}
	//一系列工作平面旋转展开操作合成的旋转转换坐标系，trans from rcs相当于转换到dcs wjh-2017.10.18
	if(rotations.Count>0)
		*pRotCS=TransRotationsToCS(rotations.NewEnumerator());
	else
		return false;
	return true;
}
GEPOINT CSectionView::TransPToDCS(const double* coord3d,GECS* pFromRotPlaneCS/*=NULL*/)
{
	GEPOINT point(coord3d);
	if(pFromRotPlaneCS)
		point=pFromRotPlaneCS->TransPFromCS(point);
	coord_trans(point,ucs,FALSE,TRUE);
	return point;
}
GEPOINT CSectionView::TransPFromDCS(const double* coord3d,GECS* pToRotPlaneCS/*=NULL*/)
{
	GEPOINT point(coord3d);
	if(pToRotPlaneCS)
		point=pToRotPlaneCS->TransPFromCS(point);
	coord_trans(point,ucs,TRUE,TRUE);
	return point;
}
GEPOINT CSectionView::TransVToDCS(const double* vector3d,GECS* pFromRotPlaneCS/*=NULL*/)
{
	GEPOINT vec(vector3d);
	if(pFromRotPlaneCS)
		vec=pFromRotPlaneCS->TransVFromCS(vec);
	vector_trans(vec,ucs,FALSE,TRUE);
	return vec;
}
GEPOINT CSectionView::TransVFromDCS(const double* vector3d,GECS* pToRotPlaneCS/*=NULL*/)
{
	GEPOINT vec(vector3d);
	if(pToRotPlaneCS)
		vec=pToRotPlaneCS->TransVToCS(vec);
	vector_trans(vec,ucs,TRUE,TRUE);
	return vec;
}
bool IsOverlappedPlate(CLDSGeneralPlate* pPlate1,CLDSGeneralPlate* pPlate2)
{
	if(stricmp(pPlate1->GetPartNo(),pPlate2->GetPartNo())!=0)
		return false;	//只比较同编号构件
	GECS cs=pPlate1->ucs;
	for(PROFILE_VERTEX* pVertex=pPlate1->vertex_list.GetFirst();pVertex;pVertex=pPlate1->vertex_list.GetNext())
	{
		GEPOINT vertice=cs.TransPFromCS(pVertex->vertex);
		coord_trans(vertice,pPlate2->ucs,FALSE,TRUE);
		bool matched=false;
		for(PROFILE_VERTEX* pVertex2=pPlate2->vertex_list.GetFirst();pVertex&&!matched;pVertex=pPlate2->vertex_list.GetNext())
		{
			if(DISTANCE(vertice,pVertex2->vertex)<2)	//2mm显示误差
				matched=true;
		}
		if(!matched)	//未找到匹配顶点，说明非重叠钢板
			return false;
	}
	return true;
}
struct FILTER_ELBOWPLATE{
	char ciPosMode;	//'U':上侧；'D'：下侧；'F'：前侧
	double cosa;//肘板法线与基准钢管绘制法线的点积，用于辨识肘板是否在正交绘图平面上
	CLDSLineTube* pDatumTube;
	CLDSGeneralPlate* pElbowPlate;
};
struct TUBE_TOWERFOOT{
	CLDSLineTube *pDatumTube;
	CLDSPlate *pFootPlate;
	CLDSPlate *pConnectPlate1,*pConnectPlate2;
	PARTSET ribPlateSet;
	TUBE_TOWERFOOT(){pFootPlate=pConnectPlate1=pConnectPlate2=NULL;pDatumTube=NULL;}
};
BOOL RetrievedTubeTowerFoot(CSectionView *pView,TUBE_TOWERFOOT *pTubeTowerFoot)
{
	if(pView==NULL||pView->BelongPartGroup()==NULL||pView->BelongPartGroup()->BelongModel()==NULL)
		return FALSE;
	IModel *pModel=pView->BelongPartGroup()->BelongModel();
	CLDSLineTube *pDatumTube=(CLDSLineTube*)pModel->FromPartHandle(pView->BelongPartGroup()->handle,CLS_LINETUBE);
	if(pDatumTube==NULL)
		return FALSE;
	//1.塔脚底板
	CLDSPlate *pFootBasePlate=NULL;
	if(!pView->IsTubeTowerFootView(pModel,&pFootBasePlate))
		return FALSE;
	if(pFootBasePlate->m_hBaseRod!=pDatumTube->handle)
		return FALSE;
	//2.塔脚连接板及肋板
	if(pTubeTowerFoot)
	{
		PARTSET partSet;
		partSet.append(pFootBasePlate);
		partSet.append(pDatumTube);
		pTubeTowerFoot->pDatumTube=pDatumTube;
		pTubeTowerFoot->pFootPlate=pFootBasePlate;
		CLDSNode *pBaseNode=pFootBasePlate->GetBaseNode();
		for(CSectionView::PART_ITEM *pPartItem=pView->SonParts.GetFirst();pPartItem;pPartItem=pView->SonParts.GetNext())
		{
			if(TagPart(pPartItem)==NULL)
				pView->LoadTagInfo(pPartItem)->pCurPart=pModel->FromPartHandle(pPartItem->hSonPart);
			CLDSPart *pPart=TagPart(pPartItem);		
			if(pPart==NULL||!pPart->IsPlate())
				continue;
			CLDSGeneralPlate *pPlate=(CLDSGeneralPlate*)pPart;
			if(pPlate->GetBaseNode()!=pBaseNode)
				continue;
			if(pPlate->IsConnectPlate())
			{
				if(pTubeTowerFoot->pConnectPlate1==NULL)
					pTubeTowerFoot->pConnectPlate1=(CLDSPlate*)pPlate;
				else if(pTubeTowerFoot->pConnectPlate2==NULL)
					pTubeTowerFoot->pConnectPlate2=(CLDSPlate*)pPlate;
			}
			else 
			{
				pPlate->IntelliRecogWeldLine(&partSet);
				for(PROFILE_VERTEX *pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
				{
					if(pVertex->m_bWeldEdge&&pVertex->weldline.hWeldMotherPart==pFootBasePlate->handle)
					{
						pTubeTowerFoot->ribPlateSet.append(pPlate);
						break;
					}
				}
			}
		}
	}
	return TRUE;
}

BOOL RetrievedTubeTowerFoot(CSectionView *pView,CLDSLineTube **ppDatumTube,CLDSPlate **ppFootPlate,
							CLDSPlate **ppConnectPlate1,CLDSPlate **ppConnectPlate2,PARTSET *pRibPlateSet)
{
	TUBE_TOWERFOOT foot;
	BOOL bRetCode=RetrievedTubeTowerFoot(pView,&foot);
	if(bRetCode)
	{
		if(ppDatumTube)
			*ppDatumTube=foot.pDatumTube;
		if(ppFootPlate)
			*ppFootPlate=foot.pFootPlate;
		if(ppConnectPlate1)
			*ppConnectPlate1=foot.pConnectPlate1;
		if(ppConnectPlate2)
			*ppConnectPlate2=foot.pConnectPlate2;
		if(pRibPlateSet)
		{
			for(CLDSPart *pPart=foot.ribPlateSet.GetFirst();pPart;pPart=foot.ribPlateSet.GetNext())
				pRibPlateSet->append(pPart);
		}
	}
	return bRetCode;
}
int CSectionView::SmartFilterNeedDrawParts(IModel *pModel,PARTSET& partset)
{
	CSectionView::PART_ITEM* pItem;
	CParaCS::PARAMETRIC_INFO paramcs=des_cs.GetParamInfo();
	des_cs.UpdateACS(pModel);
	GECS acs=des_cs.GetACS();
	if(m_iViewType!=VIEW_MAIN && m_iViewType!=VIEW_FRONT)
	{
		CLDSGeneralPlate *pDatumPlate=NULL;
		GEPOINT location1,location2;
		for(pItem=SonParts.GetFirst();pItem;pItem=SonParts.GetNext())
		{
			if(TagPart(pItem)==NULL)
				LoadTagInfo(pItem)->pCurPart=pModel->FromPartHandle(pItem->hSonPart);
			CLDSPart *pPart=TagPart(pItem);
			if(pPart&&pPart->IsPlate()&&fabs(acs.axis_z*pPart->ucs.axis_z)>EPS_COS2)
			{	//水平绘制的钢板
				if(pDatumPlate==NULL)
				{
					pDatumPlate=(CLDSGeneralPlate*)pPart;
					location1=acs.TransPToCS(pDatumPlate->ucs.origin);
				}
				else if(pDatumPlate&&IsOverlappedPlate((CLDSGeneralPlate*)pPart,pDatumPlate))//pPart->GetPartNo().EqualNoCase(pDatumPlate->GetPartNo()))
				{
					//location2=acs.TransPToCS(pPart->ucs.origin);
					//if(DISTANCE(location1,location2)<100)	//绘制位置也相近　wjh-2016.4.8
						continue;	//避免俯视图中同一件号水平绘制的钢板重复绘制(角钢肋板、钢板肋板) wht 16-03-30
				}
			}
			bool bNeedAdd=true;
			if(pPart&&pPart->GetClassTypeId()==CLS_PLATE)
			{
				for(CLDSParamPlate *pPlate=(CLDSParamPlate*)pModel->EnumPartFirst(CLS_PARAMPLATE);pPlate;pPlate=(CLDSParamPlate*)pModel->EnumPartNext(CLS_PARAMPLATE))
				{
					if(pPlate->m_iParamType!=TYPE_XEND)
						continue;
					if(SonParts.GetValue(pPlate->handle)!=NULL)
						continue;	//十字插板主板在当前视图时不应该排除
					if( pPlate->AttachPart.GetValue(pPart->handle)||		//当前钢板为十字插板附加板
						pPlate->AttachPart.GetValue(((CLDSPlate*)pPart)->m_hBaseRod))	//当前钢板定位构件为十字插板附加板
					{
						bNeedAdd=false;
						break;
					}
				}
			}
			if(bNeedAdd&&TagPart(pItem))
				partset.append(TagPart(pItem));
		}
		return partset.GetNodeNum();	//目前暂仅支持在钢管主视图中筛选过滤出需要绘制的构件
	}
	/*else if(m_iViewType==VIEW_FOOT||m_iViewType==VIEW_FL)
	{
		CLDSGeneralPlate *pDatumPlate=NULL;
		GEPOINT location1,location2;
		for(pItem=SonParts.GetFirst();pItem;pItem=SonParts.GetNext())
		{
			if(TagPart(pItem)==NULL)
				LoadTagInfo(pItem)->pCurPart=pModel->FromPartHandle(pItem->hSonPart);
			CLDSPart *pPart=TagPart(pItem);
			if(pPart&&pPart->IsPlate()&&fabs(acs.axis_z*pPart->ucs.axis_z)>EPS_COS2)
			{	//水平绘制的钢板
				if(pDatumPlate==NULL)
				{
					pDatumPlate=(CLDSGeneralPlate*)pPart;
					location1=acs.TransPToCS(pDatumPlate->ucs.origin);
				}
				else if(pDatumPlate&&pPart->GetPartNo().EqualNoCase(pDatumPlate->GetPartNo()))
				{
					location2=acs.TransPToCS(pPart->ucs.origin);
					if(DISTANCE(location1,location2)<100)	//绘制位置也相近　wjh-2016.4.8
						continue;	//避免俯视图中同一件号水平绘制的钢板重复绘制(角钢肋板、钢板肋板) wht 16-03-30
				}
			}
			if(TagPart(pItem))
				partset.append(TagPart(pItem));
		}
		return partset.GetNodeNum();	//目前暂仅支持在钢管主视图中筛选过滤出需要绘制的构件
	}*/
	CLDSLineTube *pLineTube=NULL;
	if(paramcs.desAxisX.norm_style==5)
		pLineTube=(CLDSLineTube*)pModel->FromPartHandle(paramcs.desAxisX.hVicePart,CLS_LINETUBE);
	else if(paramcs.desAxisX.norm_style==0)	//自定义X轴基准方向
	{
		for(pItem=SonParts.GetFirst();pItem;pItem=SonParts.GetNext())
		{
			if(TagPart(pItem)==NULL)
				LoadTagInfo(pItem)->pCurPart=pModel->FromPartHandle(pItem->hSonPart);
			if(TagPart(pItem)&&TagPart(pItem)->GetClassTypeId()==CLS_LINETUBE)
			{
				pLineTube=(CLDSLineTube*)TagPart(pItem);
				f3dPoint axis_x=pLineTube->End()-pLineTube->Start();
				normalize(axis_x);
				if(fabs(ucs.axis_x*axis_x)>EPS_COS2)
					break;
				pLineTube=NULL;
			}
		}
	}
	if(pLineTube==NULL)
	{
		for(pItem=SonParts.GetFirst();pItem;pItem=SonParts.GetNext())
		{
			if(TagPart(pItem)==NULL)
				LoadTagInfo(pItem)->pCurPart=pModel->FromPartHandle(pItem->hSonPart);
			if(TagPart(pItem)!=NULL)
				partset.append(TagPart(pItem));
		}
		return partset.GetNodeNum();	//无法提取出X轴基准钢管
	}
	CSuperSmartPtr<CLDSPart> pStartPlate=pModel->FromPartHandle(pLineTube->m_tJointStart.hLinkObj);
	CSuperSmartPtr<CLDSPart> pEndPlate  =pModel->FromPartHandle(pLineTube->m_tJointEnd.hLinkObj);
	SCOPE_STRU draw_space;	//当前视图绘图坐标系下
	f3dLine axisline(pLineTube->Start(),pLineTube->End());
	axisline.startPt=acs.TransPToCS(axisline.startPt);
	axisline.endPt=acs.TransPToCS(axisline.endPt);
	draw_space.VerifyVertex(axisline.startPt);
	draw_space.VerifyVertex(axisline.endPt);
	double radius=pLineTube->GetWidth()*0.5;
	draw_space.fMinY-=radius;
	draw_space.fMaxY+=radius;
	draw_space.fMinZ-=radius;
	draw_space.fMaxZ+=radius;
	double justify;
	//1.提取该视图中需要绘制的杆件
	CHashSet<CLDSLinePart*> hashRodSet;
	CHashList<long> hashEndPlates;
	CHashStrList<CLDSPart*>hashSlotNails;	//键值"localposx#-localposy"
	for(pItem=SonParts.GetFirst();pItem;pItem=SonParts.GetNext())
	{
		CSuperSmartPtr<CLDSPart> pCurPart=LoadTagInfo(pItem)->pCurPart=pModel->FromPartHandle(pItem->hSonPart);
		if(pCurPart.IsNULL())
			continue;
		pCurPart->feature=0;
		if(!pCurPart->IsLinePart())
			continue;
		if(pCurPart->FuncType()==CLDSPart::FUNC_FOOTNAIL)
		{	//脚钉无论如何都致少绘制一颗，前后重位的脚钉只绘制前侧脚钉 wjh-2016.8.16
			if(pCurPart->GetClassTypeId()==CLS_LINESLOT)
			{
				CLDSLineSlot* pNailSlot=pCurPart.LineSlotPointer();
				GEPOINT vSlotNormal=pNailSlot->WorkPlaneNorm();	
				//需要特别注意槽钢的工作面法线与局部坐标系的Ｙ轴反向　wjh-2016.10.18
				double justify=vSlotNormal*acs.axis_z;//pCurPart->ucs.axis_y*acs.axis_z
				if(fabs(justify)<EPS_COS2)	//非平铺绘制
				{
					partset.append(pCurPart);
					continue;
				}
				else
				{
					GEPOINT localpos=acs.TransPToCS((pNailSlot->Start()+pNailSlot->End())*0.5);
					CXhChar50 poskeystr("%d#%d",ftoi(localpos.x*10),ftoi(localpos.y*10));
					CLDSPart** ppLegacyNail=hashSlotNails.GetValue(poskeystr);
					CLDSPart* pLegacyNail=ppLegacyNail!=NULL?*ppLegacyNail:NULL;
					if(justify>0)
					{
						pCurPart->feature=1;	//正面的脚钉无论如何都绘制
						if(pLegacyNail&&pLegacyNail->ucs.axis_y*acs.axis_z>=0)	//原有脚钉为背面脚钉，则改为不绘制
						{
							pLegacyNail->feature=0;
							hashDualSideSlotNails.SetValue(pCurPart->handle,true);
						}
					}
					else
					{
						if(pLegacyNail==NULL)
							pCurPart->feature=1;
						else if(((CLDSLineSlot*)pLegacyNail)->WorkPlaneNorm()*acs.axis_z>0)	//原有脚钉为正面脚钉，当前脚钉不绘制
						{
							hashDualSideSlotNails.SetValue(pLegacyNail->handle,true);
							continue;
						}
					}
					if(pCurPart->feature==1||ppLegacyNail==NULL)
						hashSlotNails.SetValue(poskeystr,pCurPart);
					continue;
				}
			}
			else
			{
				partset.append(pCurPart);
				pCurPart->feature=1;
			}
		}
		f3dLine rayline(pCurPart.LinePartPointer()->Start(),pCurPart.LinePartPointer()->End());
		f3dPoint rayvec=rayline.endPt-rayline.startPt;
		normalize(rayvec);
		rayline.startPt-=rayvec*pCurPart.LinePartPointer()->startOdd();
		rayline.endPt  +=rayvec*pCurPart.LinePartPointer()->endOdd();
		rayline.startPt = acs.TransPToCS(rayline.startPt);
		rayline.endPt = acs.TransPToCS(rayline.endPt);
		GEPOINT midpoint=(rayline.startPt+rayline.endPt)*0.5;
		if( pCurPart!=pLineTube&&midpoint.z<(axisline.startPt.z+axisline.endPt.z)*0.5&&
			midpoint.x>draw_space.fMinX&&midpoint.x<draw_space.fMaxX&&
			rayline.startPt.y<draw_space.fMaxY&&rayline.startPt.y>draw_space.fMinY&&
			rayline.endPt.y<draw_space.fMaxY&&rayline.endPt.y>draw_space.fMinY)
		{
			pCurPart->feature=1;
			continue;	//当前绘制杆件被基准主管挡在后面时，不绘制
		}
		partset.append(pCurPart);
		pCurPart->feature=1;	//已处理
		hashRodSet.SetValue(pCurPart->handle,pCurPart.LinePartPointer());
		if(pCurPart->GetClassTypeId()==CLS_LINETUBE)
		{
			//由于过往旧数据中m_tJointStart.type值存在错误，暂不使用该条件过滤 wjh-2014.3.30
			//if(pCurPart.LineTubePointer()->m_tJointStart.type>=2)
				hashEndPlates.SetValue(pCurPart.LineTubePointer()->m_tJointStart.hLinkObj,pCurPart.LineTubePointer()->m_tJointStart.hLinkObj);
			//if(pCurPart.LineTubePointer()->m_tJointEnd.type>=2)
				hashEndPlates.SetValue(pCurPart.LineTubePointer()->m_tJointEnd.hLinkObj,pCurPart.LineTubePointer()->m_tJointEnd.hLinkObj);
		}
	}
	for(CLDSPartPtr* ppSlotSnail=hashSlotNails.GetFirst();ppSlotSnail;ppSlotSnail=hashSlotNails.GetNext())
	{
		if((*ppSlotSnail)->feature==1)
			partset.append(*ppSlotSnail);
	}
	//2.提取该视图中需要绘制的法兰及端板
	for(pItem=SonParts.GetFirst();pItem;pItem=SonParts.GetNext())
	{
		if(TagPart(pItem)==NULL)
			continue;
		CSuperSmartPtr<CLDSPart> pCurPart=TagPart(pItem);
		if(pCurPart->feature!=0)
			continue;	//已筛选过的构件
		if(pCurPart->GetClassTypeId()!=CLS_PARAMPLATE ||
			(!pCurPart.ParamPlatePointer()->IsFL()&&!pCurPart.ParamPlatePointer()->IsInsertPlate()))
			continue;	//不是端板类型(法兰及插板)的参数化板
		pCurPart->feature=1;
		//由于过往部分文件因未知原因导致数据错误，钢管端头连接构件与法兰句柄不符因此应增加焊接父构件是否为待绘制构件的条件 wjh-2014.9.18
		if(hashEndPlates.GetValue(pCurPart->handle)==NULL&&hashRodSet.GetValue(pCurPart->m_hPartWeldParent)==NULL)
			continue;	//端板所依附的杆件不绘制该端板自然也不绘制
		if(pCurPart.ParamPlatePointer()->IsInsertPlate())
		{
			partset.append(pCurPart);
			continue;
		}
		justify=fabs(pCurPart->ucs.axis_z*acs.axis_z);
		if((m_iViewType==VIEW_MAIN||m_iViewType==VIEW_FRONT)&&justify>0.707)//EPS_COS2)
			continue;	//主视图中只绘制竖直方向的法兰(或底脚板)
		partset.append(pCurPart);
	}
	//3.筛选过滤法兰肘板
	FILTER_ELBOWPLATE* pFilterPlate;
	CMapList<FILTER_ELBOWPLATE> hashFilterElbowPlates;
	CXhSimpleList<RELA_RIBPLATE> listDebatablePlates;
	PARTSET plateset;	//不太确定是否需要绘制的肘板或肋板
	TUBE_TOWERFOOT foot;
	BOOL bTubeTowerFoot=RetrievedTubeTowerFoot(this,&foot);
	if(bTubeTowerFoot)
	{	//添加钢管塔脚连接板至肋板过滤链表，避免在节点板位置重复绘制肋板 wht 18-08-23
		for(int i=0;i<2;i++)
		{
			CLDSGeneralPlate *pPlate=(i==0)?foot.pConnectPlate1:foot.pConnectPlate2;
			if(pPlate==NULL)
				continue;
			GEPOINT vAxisY=foot.pDatumTube->GetPlateVertSpreadVec(pPlate);
			double cosa_y=vAxisY*acs.axis_y;
			double cosa_z=vAxisY*acs.axis_z;
			char ciMode=0;
			if(cosa_z>EPS_COS2)
				ciMode='f';
			else if(cosa_y>0.8)
				ciMode='u';
			else if(cosa_y<-0.8)
				ciMode='d';
			else
				continue;
			pFilterPlate=hashFilterElbowPlates.Add(foot.pDatumTube->handle,ciMode);
			pFilterPlate->ciPosMode=ciMode;
			pFilterPlate->pDatumTube=foot.pDatumTube;
			pFilterPlate->pElbowPlate=pPlate;
			if(ciMode=='f'||ciMode=='F')
				pFilterPlate->cosa=cosa_z;
			else
				pFilterPlate->cosa=cosa_y;
		}
	}
	for(pItem=SonParts.GetFirst();pItem;pItem=SonParts.GetNext())
	{
		if(TagPart(pItem)==NULL)
			continue;
		CSuperSmartPtr<CLDSPart> pCurPart=TagPart(pItem);
		if(pCurPart->feature!=0)
			continue;	//已筛选过的构件
		if(bTubeTowerFoot)
		{
			if(!foot.ribPlateSet.FromHandle(pCurPart->handle))
				continue;
			//钢管塔脚肋板绘制示意图 wht 18-08-24
			pItem->cFlag='W';
		}
		else if(  pCurPart->GetClassTypeId()!=CLS_PARAMPLATE ||	//由于历史原因法兰上的加劲板有时是TYPE_ELBOWPLATE(早期)，有时是TYPE_RIBPLATE
			(pCurPart.ParamPlatePointer()->m_iParamType!=TYPE_ELBOWPLATE&&pCurPart.ParamPlatePointer()->m_iParamType!=TYPE_RIBPLATE))
			continue;
		int iSubRibPlateType=pCurPart->GetClassTypeId()==CLS_PARAMPLATE?pCurPart.pParamPlate->GetRibPlateSubType():0;
		long hDatumTube=0,hDatumPlate=0;
		bool bFlElbowFilter=false;
		CLDSLineTube* pDatumTube=NULL;
		GECS dcs=acs;
		if(iSubRibPlateType==CLDSParamPlate::TYPE_RIB_ELBOW_PLATE)
		{
			CLDSParamPlate::RIBPLATE_ELBOW elbow;
			elbow.LoadParams(pCurPart.pParamPlate);
			hDatumPlate=abs(elbow.hBtmPlate.ToLong());
			hDatumTube=abs(elbow.hDatumTube.ToLong());
			bFlElbowFilter=true;	//不论hDatumPlate是否为基准钢管的端头hLinkObj,都要进行过滤 wjh-2018.3.12
			if((pDatumTube=(CLDSLineTube*)pModel->FromPartHandle(abs(hDatumTube),CLS_LINETUBE))!=NULL)
			{
				CDrawCoordTrans dcsx;
				if(GetDCS(pDatumTube->handle,dcsx))
				{
					dcs.axis_x=pDatumTube->End()-pDatumTube->Start();
					GEPOINT axis_z(0,0,1);
					GEPOINT axis_x=dcsx.TransVToDraw(dcs.axis_x);
					if(axis_x.x<0)
						axis_x*=-1;
					axis_x.z=0;
					normalize(axis_x);
					GEPOINT axis_y=axis_z^axis_x;
					//下面代码中用不到dcs.axis_x故暂时不转换 wjh-2018.3.5
					//dcs.axis_x=dcsx.TransVFromDraw(axis_x);
					dcs.axis_y=dcsx.TransVFromDraw(axis_y);
					dcs.axis_z=dcsx.TransVFromDraw(axis_z);
				}
			}
		}
		else if(iSubRibPlateType==CLDSParamPlate::TYPE_RIB_CORNER_PLATE)
		{
			CLDSParamPlate::RIBPLATE_CORNER corner;
			corner.LoadParams(pCurPart.pParamPlate);
			hDatumPlate=abs(corner.hDatumPlate.ToLong());
		}
		else if(bTubeTowerFoot)
			pDatumTube=foot.pDatumTube;
		pCurPart->feature=1;	//已进行筛选处理

		justify=fabs(pCurPart->ucs.axis_z*dcs.axis_z);
		if(justify<EPS2 || justify>0.8)//EPS_COS2)	//TODO:愿意可能是为了过滤掉众多非正交肘板,但现在发现一些较大的变异类型的肘板形肋板,就不受正交限制 wjh-2017.7.26
		{
			bool debated=false;
			if(iSubRibPlateType==CLDSParamPlate::TYPE_RIB_CORNER_PLATE)
				debated=true;
			else if(pDatumTube)
			{
				GEPOINT vAxisY=pDatumTube->GetPlateVertSpreadVec(pCurPart.pBasicPlate);
				double cosa_y=vAxisY*dcs.axis_y;
				double cosa_z=vAxisY*dcs.axis_z;
				char ciMode=0;
				if(cosa_z>EPS_COS2)
					ciMode='f';
				else if(cosa_y>0.8)
					ciMode='u';
				else if(cosa_y<-0.8)
					ciMode='d';
				else
					continue;
				long* phRelaEndPlate=0;
				if(bFlElbowFilter||(phRelaEndPlate=hashEndPlates.GetValue(abs(hDatumPlate)))!=NULL)
				{	//由于法兰肘板可在法兰图中描述，固须只须绘制最为正交的三个（上下及前面）与当前绘图平面最为平行或垂直的肘板 wjh-2018.3.5
					if(phRelaEndPlate!=NULL&&*phRelaEndPlate==pDatumTube->m_tJointStart.hLinkObj)
						ciMode=toupper(ciMode);	//始端侧用大写字母标识位置
					else
					{
						GEPOINT d2s=GEPOINT(pCurPart->ucs.origin)-pDatumTube->xPosStart;
						GEPOINT d2e=GEPOINT(pCurPart->ucs.origin)-pDatumTube->xPosEnd;
						if(d2s.mod2()<d2e.mod2())	//始端侧（距离始端更近说明位于始端侧）用大写字母标识位置
							ciMode=toupper(ciMode);
					}
					pFilterPlate=hashFilterElbowPlates.GetValue(pDatumTube->handle,ciMode);
					if(pFilterPlate==NULL)
					{
						pFilterPlate=hashFilterElbowPlates.Add(pDatumTube->handle,ciMode);
						pFilterPlate->ciPosMode=ciMode;
						pFilterPlate->pDatumTube=pDatumTube;
						pFilterPlate->pElbowPlate=pCurPart.pBasicPlate;
						if(ciMode=='f'||ciMode=='F')
							pFilterPlate->cosa=cosa_z;
						else
							pFilterPlate->cosa=cosa_y;
					}
					else if((ciMode=='f'||ciMode=='F')&&cosa_z>pFilterPlate->cosa)
					{
						pFilterPlate->ciPosMode=ciMode;
						pFilterPlate->pDatumTube=pDatumTube;
						pFilterPlate->pElbowPlate=pCurPart.pBasicPlate;
						pFilterPlate->cosa=cosa_z;
					}
					else if(fabs(cosa_y)>fabs(pFilterPlate->cosa))
					{
						pFilterPlate->pElbowPlate=pCurPart.pBasicPlate;
						pFilterPlate->cosa=cosa_y;
					}
				}
				else
					debated=true;
			}
			if(debated)
			{
				if(SonParts.GetValue(abs(hDatumPlate))==NULL)
					continue;	//如果基板根本就不在当前视图中,则认为不需要显示 wjh-2017.7.26
				RELA_RIBPLATE* pDebatePlate=listDebatablePlates.AttachObject();
				pDebatePlate->pPlate=pCurPart.pParamPlate;
				pDebatePlate->hDatumPlate=abs(hDatumPlate);
			}
		}
	}
	for(pFilterPlate=hashFilterElbowPlates.GetFirst();pFilterPlate;pFilterPlate=hashFilterElbowPlates.GetNext())
		partset.append(pFilterPlate->pElbowPlate);

	//4.筛选过滤脚钉及其余普通钢板
	for(pItem=SonParts.GetFirst();pItem;pItem=SonParts.GetNext())
	{
		if(TagPart(pItem)==NULL)
			continue;
		CSuperSmartPtr<CLDSPart> pCurPart=TagPart(pItem);
		if(pCurPart->feature!=0)
			continue;	//已筛选过的构件
		if(!pCurPart->IsPlate())
			continue;
		//槽钢脚钉已在上面代码中处理过，故此处仅处理踏脚板类型的脚钉 wjh-2016.8.16
		if(pCurPart->FuncType()==CLDSPart::FUNC_FOOTNAIL)
			partset.append(pCurPart);
		pCurPart->Create3dSolidModel();
		CLDSPart *pWeldParentPart=pModel->FromPartHandle(pCurPart->m_hPartWeldParent);
		bool bUnderlyingPlate=true,bTopFloatPlate=true;
		if( pItem->ciSpreadDirection==PART_ITEM::SPREAD_TO_UPPER||	//主管上侧
			pItem->ciSpreadDirection==PART_ITEM::SPREAD_TO_UNDER||	//主管下侧
			pItem->ciSpreadDirection==PART_ITEM::SPREAD_TO_TOP)		//主管前侧
			bUnderlyingPlate=bTopFloatPlate=false;
		else if(pItem->ciSpreadDirection==PART_ITEM::SPREAD_TO_BACK)//主管后侧
			bUnderlyingPlate=true;
		else if(pCurPart->m_hPartWeldParent==pLineTube->handle||
				(pWeldParentPart&&pWeldParentPart->m_hPartWeldParent==pLineTube->handle))
		{
			CDrawCoordTrans dcs;
			bool bSpreadPlane=(hashAssistPlanes.GetValue(pItem->iResidePlaneNo)!=NULL);
			bSpreadPlane=bSpreadPlane&&GetDCS(pItem->hSonPart,dcs);
			for(int i=0;i<pCurPart->pSolidBody->KeyPointNum();i++)
			{
				GEPOINT vertice=pCurPart->pSolidBody->GetKeyPointAt(i);
				if(bSpreadPlane)	//展开绘制构件需展开后再判断是否被钢管遮挡 wht 18-03-12
					vertice=dcs.TransToDraw(vertice,false);
				else
					vertice=acs.TransPToCS(vertice);
				if(bUnderlyingPlate&&vertice.z>axisline.startPt.z||fabs(vertice.y)>radius)
					bUnderlyingPlate=false;	//有一个点不在主管轴线之下，则表示钢板不位于绘图平面的背面
				if(bTopFloatPlate&&vertice.z<axisline.startPt.z+radius+10)
					bTopFloatPlate=false;//有一个点不漂浮于主管外壁之上，则表示该板不是漂浮板
				if(!bUnderlyingPlate&&!bTopFloatPlate)
					break;
			}
		}
		else	//TODO:对接钢管时，对接管上的连接板根据基准管判断会存在大问题，以后应改进根据旋转展开后的坐标进行判断 wjh-2017.11.30
			bUnderlyingPlate=bTopFloatPlate=false;
		pCurPart->feature=1;	//已进行筛选处理
		if(bUnderlyingPlate || bTopFloatPlate)
			continue;
		partset.append(pCurPart);
	}
	//5.处理之前疑而未决的肘板或肋板
	for(RELA_RIBPLATE* pDebatePlate=listDebatablePlates.EnumObjectFirst();pDebatePlate;pDebatePlate=listDebatablePlates.EnumObjectNext())
	{
		if(partset.FromHandle(pDebatePlate->hDatumPlate)==NULL)
			continue;	//基板不显示,其上的肘板或肋板也不显示
		partset.append(pDebatePlate->pPlate);	//由于法兰肘板可在法兰图中描述，固须较为严格的与当前绘图平面平行或垂直才绘制(但也不能用EPS,EPS_COS判别)
	}
	//6.移除主视图中的衬托板及套管 wht 18-03-10
	if(m_idViewType!=CLS_BUSH_PLATE_VIEW&&(IsMainView()||m_iViewType==VIEW_MAIN||m_iViewType==VIEW_FRONT))
	{
		for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(pPart->IsTube())
			{
				CLDSLineTube *pCurLineTube=(CLDSLineTube*)pPart;
				if(pCurLineTube->FuncType()==CLDSLineTube::FUNC_WIREBUSH||(pCurLineTube->pStart==NULL&&pCurLineTube->pEnd==NULL))
					partset.DeleteNode(pPart->handle);
			}
			else if(pPart->IsPlate()&&((CLDSGeneralPlate*)pPart)->IsBushWeldPlate())
				partset.DeleteNode(pPart->handle);
		}
	}
	//删除重复构件
	CHashSet<CLDSPart*> hashPartSet;
	for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(hashPartSet.GetValue(pPart->handle))
		{
			partset.DeleteNode(pPart->handle);	//DeleteNode会删除所有构件，需要重新添加pPart
			partset.append(pPart);
		}
		else
			hashPartSet.SetValue(pPart->handle,pPart);
	}
	return partset.GetNodeNum();
}
//////////////////////////////////////////////////////////////////////////
//CStdXPlatesView
//////////////////////////////////////////////////////////////////////////
CStdXPlatesView::CStdXPlatesView(int id) : CSectionView(id)
{
	_idViewType=CLS_STDX_PLATES_VIEW;
	m_cSubType='F';	//'F'正视图;'S':侧视图;'D':俯视图
	m_hBasePlate=0;	//通板句柄
	m_hFrontVertPlate=0;	//正面立板
	m_hBackVertPlate=0;	//侧面立板
	m_arrAngleHandle[0]=m_arrAngleHandle[1]=0;
	m_arrAngleHandle[2]=m_arrAngleHandle[3]=0;
}
void CStdXPlatesView::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	CSectionView::ToBuffer(buffer,version,doc_type);
	if(version==0||
		(doc_type==1&&version>=5000000)||
		(doc_type==2&&version>=2000000)||
		(doc_type==4&&version>=1030002)||
		(doc_type==5&&version>=1030002))
	{
		buffer.WriteByte(m_cSubType);
		buffer.WriteInteger(m_hBasePlate);
		buffer.WriteInteger(m_hFrontVertPlate);
		buffer.WriteInteger(m_hBackVertPlate);
		for(int i=0;i<4;i++)
			buffer.WriteInteger(m_arrAngleHandle[i]);
	}
}
void CStdXPlatesView::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	CSectionView::FromBuffer(buffer,version,doc_type);
	if(version==0||
		(doc_type==1&&version>=5000000)||
		(doc_type==2&&version>=2000000)||
		(doc_type==4&&version>=1030002)||
		(doc_type==5&&version>=1030002))
	{
		buffer.ReadByte(&m_cSubType);
		buffer.ReadInteger(&m_hBasePlate);
		buffer.ReadInteger(&m_hFrontVertPlate);
		buffer.ReadInteger(&m_hBackVertPlate);
		for(int i=0;i<4;i++)
			buffer.ReadInteger(&m_arrAngleHandle[i]);
	}
}
//////////////////////////////////////////////////////////////////////////
//CAngleFootView
//////////////////////////////////////////////////////////////////////////
CAngleFootView::CAngleFootView(int id) : CSectionView(id)
{
	_idViewType=CLS_ANGLE_FOOT_VIEW;
	m_cSubType='F';				//'F'正视图;'S':侧视图;'D':俯视图
	m_hFootPlate=m_hShoePlate=m_hRightShoePlate=m_hFrontShoePlate=m_hBackShoePlate=0;
	memset(m_arrAngleHandle,0,4*sizeof(long));	//关联角钢句柄	
	m_hDownLeftShoePlate=m_hDownRightShoePlate=m_hDownFrontShoePlate=m_hDownBackShoePlate=0;
	memset(m_arrDownAngleHandle,0,4*sizeof(long));	//关联角钢句柄	
}
void CAngleFootView::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	CSectionView::ToBuffer(buffer,version,doc_type);
	if(version==0||
		(doc_type==1&&version>=5000000)||
		(doc_type==2&&version>=2000000)||
		(doc_type==4&&version>=1030002)||
		(doc_type==5&&version>=1030002))
	{
		buffer.WriteByte(m_cSubType);
		buffer.WriteInteger(m_hFootPlate);
		buffer.WriteInteger(m_hShoePlate);
		buffer.WriteInteger(m_hRightShoePlate);
		buffer.WriteInteger(m_hFrontShoePlate);
		buffer.WriteInteger(m_hBackShoePlate);
		for(int i=0;i<4;i++)
			buffer.WriteInteger(m_arrAngleHandle[i]);
	}
	if(version==0||
		(doc_type==1&&version>=5000010)||
		(doc_type==2&&version>=2000005)||
		(doc_type==4&&version>=1030005)||
		(doc_type==5&&version>=1030005))
	{
		buffer.WriteInteger(m_hDownLeftShoePlate);
		buffer.WriteInteger(m_hDownRightShoePlate);
		buffer.WriteInteger(m_hDownFrontShoePlate);
		buffer.WriteInteger(m_hDownBackShoePlate);
		for(int i=0;i<4;i++)
			buffer.WriteInteger(m_arrDownAngleHandle[i]);
	}
}
void CAngleFootView::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	CSectionView::FromBuffer(buffer,version,doc_type);
	if(version==0||
		(doc_type==1&&version>=5000000)||
		(doc_type==2&&version>=2000000)||
		(doc_type==4&&version>=1030002)||
		(doc_type==5&&version>=1030002))
	{
		buffer.ReadByte(&m_cSubType);
		buffer.ReadInteger(&m_hFootPlate);
		buffer.ReadInteger(&m_hShoePlate);
		buffer.ReadInteger(&m_hRightShoePlate);
		buffer.ReadInteger(&m_hFrontShoePlate);
		buffer.ReadInteger(&m_hBackShoePlate);
		for(int i=0;i<4;i++)
			buffer.ReadInteger(&m_arrAngleHandle[i]);
	}
	if(version==0||
		(doc_type==1&&version>=5000010)||
		(doc_type==2&&version>=2000005)||
		(doc_type==4&&version>=1030005)||
		(doc_type==5&&version>=1030005))
	{
		buffer.ReadInteger(&m_hDownLeftShoePlate);
		buffer.ReadInteger(&m_hDownRightShoePlate);
		buffer.ReadInteger(&m_hDownFrontShoePlate);
		buffer.ReadInteger(&m_hDownBackShoePlate);
		for(int i=0;i<4;i++)
			buffer.ReadInteger(&m_arrDownAngleHandle[i]);
	}
}
//////////////////////////////////////////////////////////////////////////
//CDualTubeJointView
//////////////////////////////////////////////////////////////////////////
CDualTubeJointView::CDualTubeJointView(int id/*=0*/) : CSectionView(id)
{
	_idViewType=CLS_DUALTUBE_JOINT_VIEW;
	m_hMasterTube=m_hJointTube=0;
	m_hTemporyStartConnPlate=m_hTemporyEndConnPlate=m_cTemporyJointState=0;
	m_bMasterTubeOnLeft=false;	//默认在右侧
}
bool CDualTubeJointView::UpdateConnectState(IModel* pTower,CDualTubeJointView* pCopyFromDualTubeJointView/*=NULL*/)
{
	if(pCopyFromDualTubeJointView)
	{
		m_hTemporyStartConnPlate=pCopyFromDualTubeJointView->hStartJointPlate;
		m_hTemporyEndConnPlate=pCopyFromDualTubeJointView->hEndJointPlate;
		m_cTemporyJointState=pCopyFromDualTubeJointView->cbJointState;
		return true;
	}
	CLDSLineTube* pMasterTube=(CLDSLineTube*)(pTower!=NULL?pTower->FromPartHandle(m_hMasterTube):CLDSPart::_console->FromPartHandle(m_hMasterTube));
	CLDSLineTube* pJointTube=(CLDSLineTube*)(pTower!=NULL?pTower->FromPartHandle(m_hJointTube):CLDSPart::_console->FromPartHandle(m_hJointTube));
	if(pMasterTube==NULL||pJointTube==NULL)
		return false;
	if( pMasterTube->pStart==pJointTube->pStart)
		m_cTemporyJointState=0x00;
	else if(pMasterTube->pStart==pJointTube->pEnd)
		m_cTemporyJointState=0x01;
	else if(pMasterTube->pEnd==pJointTube->pStart)
		m_cTemporyJointState=0x10;
	else if(pMasterTube->pEnd==pJointTube->pEnd)
		m_cTemporyJointState=0x11;
	char cMasterDirect='X';
	long hStartPlate=0,hEndPlate=0;
	GEPOINT lenVec=pMasterTube->pEnd->xPreliftPos-pMasterTube->pStart->xPreliftPos;
	if(fabs(lenVec.x)>=fabs(lenVec.y)&&fabs(lenVec.x)>=fabs(lenVec.z))
	{
		cMasterDirect='X';
		if(pMasterTube->pEnd->xArcliftPos.x>pMasterTube->pStart->xArcliftPos.x)
		{	//水平钢管梁始->终为Ｘ轴正向
			if(cbJointState&0xf0)
			{	//水平钢管梁终端为对接点
				m_bMasterTubeOnLeft=true;	//水平基准钢管梁在整根曲梁的左侧
				m_hTemporyStartConnPlate=pMasterTube->m_tJointStart.hLinkObj;
				if(cbJointState&0x0f)//斜对接钢管梁终端为对接点
					m_hTemporyEndConnPlate=pJointTube->m_tJointStart.hLinkObj;
				else
					m_hTemporyEndConnPlate=pJointTube->m_tJointEnd.hLinkObj;
			}
			else
			{	//水平钢管梁始端为对接点
				m_bMasterTubeOnLeft=false;	//水平基准钢管梁在整根曲梁的右侧
				m_hTemporyEndConnPlate=pMasterTube->m_tJointEnd.hLinkObj;
				if(cbJointState&0x0f)//斜对接钢管梁终端为对接点
					m_hTemporyStartConnPlate=pJointTube->m_tJointStart.hLinkObj;
				else
					m_hTemporyStartConnPlate=pJointTube->m_tJointEnd.hLinkObj;
			}
		}
		else
		{	//水平钢管梁始->终为负Ｘ轴正向
			if(m_cTemporyJointState&0xf0)
			{	//水平钢管梁终端为对接点
				m_bMasterTubeOnLeft=false;	//水平基准钢管梁在整根曲梁的右侧
				m_hTemporyEndConnPlate=pMasterTube->m_tJointStart.hLinkObj;
				if(cbJointState&0x0f)//斜对接钢管梁终端为对接点
					m_hTemporyStartConnPlate=pJointTube->m_tJointStart.hLinkObj;
				else
					m_hTemporyStartConnPlate=pJointTube->m_tJointEnd.hLinkObj;
			}
			else
			{	//水平钢管梁始端为对接点
				m_bMasterTubeOnLeft=true;	//水平基准钢管梁在整根曲梁的左侧
				m_hTemporyStartConnPlate=pMasterTube->m_tJointEnd.hLinkObj;
				if(m_cTemporyJointState&0x0f)//斜对接钢管梁终端为对接点
					m_hTemporyEndConnPlate=pJointTube->m_tJointStart.hLinkObj;
				else
					m_hTemporyEndConnPlate=pJointTube->m_tJointEnd.hLinkObj;
			}
		}
	}
	else if(fabs(lenVec.y)>=fabs(lenVec.x)&&fabs(lenVec.y)>=fabs(lenVec.z))
	{	//Y轴向对接钢管默认应自前向后输出视图
		cMasterDirect='Y';
		if(pMasterTube->pEnd->xArcliftPos.y<pMasterTube->pStart->xArcliftPos.y)
		{	//Y钢管梁始->终为负Y轴正向
			if(m_cTemporyJointState&0xf0)
			{	//Y钢管梁终端为对接点
				m_bMasterTubeOnLeft=true;	//水平基准钢管梁在整根曲梁的左侧
				m_hTemporyStartConnPlate=pMasterTube->m_tJointStart.hLinkObj;
				if(cbJointState&0x0f)//斜对接钢管梁终端为对接点
					m_hTemporyEndConnPlate=pJointTube->m_tJointStart.hLinkObj;
				else
					m_hTemporyEndConnPlate=pJointTube->m_tJointEnd.hLinkObj;
			}
			else
			{	//Y钢管梁始端为对接点
				m_bMasterTubeOnLeft=false;	//水平基准钢管梁在整根曲梁的右侧
				m_hTemporyEndConnPlate=pMasterTube->m_tJointEnd.hLinkObj;
				if(cbJointState&0x0f)//斜对接钢管梁终端为对接点
					m_hTemporyStartConnPlate=pJointTube->m_tJointStart.hLinkObj;
				else
					m_hTemporyStartConnPlate=pJointTube->m_tJointEnd.hLinkObj;
			}
		}
		else
		{	//Y钢管梁始->终为正Y轴正向
			if(m_cTemporyJointState&0xf0)
			{	//Y钢管梁终端为对接点
				m_bMasterTubeOnLeft=true;	//水平基准钢管梁在整根曲梁的左侧
				m_hTemporyEndConnPlate=pMasterTube->m_tJointStart.hLinkObj;
				if(cbJointState&0x0f)//斜对接钢管梁终端为对接点
					m_hTemporyStartConnPlate=pJointTube->m_tJointStart.hLinkObj;
				else
					m_hTemporyStartConnPlate=pJointTube->m_tJointEnd.hLinkObj;
			}
			else
			{	//Y钢管梁始端为对接点
				m_bMasterTubeOnLeft=false;	//水平基准钢管梁在整根曲梁的右侧
				m_hTemporyStartConnPlate=pMasterTube->m_tJointEnd.hLinkObj;
				if(cbJointState&0x0f)//斜对接钢管梁终端为对接点
					m_hTemporyEndConnPlate=pJointTube->m_tJointStart.hLinkObj;
				else
					m_hTemporyEndConnPlate=pJointTube->m_tJointEnd.hLinkObj;
			}
		}
	}
	else //if(fabs(lenVec.z)>=fabs(lenVec.x)&&fabs(lenVec.z)>=fabs(lenVec.y))
	{	//Z轴向对接钢管默认应自上向下输出视图
		cMasterDirect='Z';
		if(pMasterTube->pEnd->xArcliftPos.z>pMasterTube->pStart->xArcliftPos.y)
		{	//Z钢管梁始->终为正Z轴正向
			if(m_cTemporyJointState&0xf0)
			{	//Z钢管梁终端为对接点
				m_bMasterTubeOnLeft=true;	//水平基准钢管梁在整根曲梁的左侧
				m_hTemporyStartConnPlate=pMasterTube->m_tJointStart.hLinkObj;
				if(cbJointState&0x0f)//斜对接钢管梁终端为对接点
					m_hTemporyEndConnPlate=pJointTube->m_tJointStart.hLinkObj;
				else
					m_hTemporyEndConnPlate=pJointTube->m_tJointEnd.hLinkObj;
			}
			else
			{	//Z钢管梁始端为对接点
				m_bMasterTubeOnLeft=false;	//Z基准钢管梁在整根曲梁的右侧
				m_hTemporyEndConnPlate=pMasterTube->m_tJointEnd.hLinkObj;
				if(cbJointState&0x0f)//斜对接钢管梁终端为对接点
					m_hTemporyStartConnPlate=pJointTube->m_tJointStart.hLinkObj;
				else
					m_hTemporyStartConnPlate=pJointTube->m_tJointEnd.hLinkObj;
			}
		}
		else
		{	//Z钢管梁始->终为负Z轴正向
			if(m_cTemporyJointState&0xf0)
			{	//Z钢管梁终端为对接点
				m_bMasterTubeOnLeft=false;	//Z基准钢管梁在整根曲梁的左侧
				m_hTemporyEndConnPlate=pMasterTube->m_tJointStart.hLinkObj;
				if(cbJointState&0x0f)//斜对接钢管梁终端为对接点
					m_hTemporyStartConnPlate=pJointTube->m_tJointStart.hLinkObj;
				else
					m_hTemporyStartConnPlate=pJointTube->m_tJointEnd.hLinkObj;
			}
			else
			{	//水平钢管梁始端为对接点
				m_bMasterTubeOnLeft=true;	//Z基准钢管梁在整根曲梁的右侧
				m_hTemporyStartConnPlate=pMasterTube->m_tJointEnd.hLinkObj;
				if(cbJointState&0x0f)//斜对接钢管梁终端为对接点
					m_hTemporyEndConnPlate=pJointTube->m_tJointStart.hLinkObj;
				else
					m_hTemporyEndConnPlate=pJointTube->m_tJointEnd.hLinkObj;
			}
		}
	}
	return true;
}
void CDualTubeJointView::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	CSectionView::ToBuffer(buffer,version,doc_type);
	if(version==0||
		(doc_type==1&&version>=5010700)||
		(doc_type==2&&version>=2000800)||
		(doc_type==4&&version>=1030800)||
		(doc_type==5&&version>=1030800))
	{
		buffer.WriteInteger(m_hMasterTube);
		buffer.WriteInteger(m_hJointTube);
	}
}
void CDualTubeJointView::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	CSectionView::FromBuffer(buffer,version,doc_type);
	if(version==0||
		(doc_type==1&&version>=5010700)||
		(doc_type==2&&version>=2000800)||
		(doc_type==4&&version>=1030800)||
		(doc_type==5&&version>=1030800))
	{
		buffer.ReadInteger(&m_hMasterTube);
		buffer.ReadInteger(&m_hJointTube);
	}
}
//////////////////////////////////////////////////////////////////////////
//CHorizBeamTubeView
//////////////////////////////////////////////////////////////////////////
CHorizBeamTubeView::CHorizBeamTubeView(int id/*=0*/) : CDualTubeJointView(id)
{
	_idViewType=CLS_HORIZ_BEAM_VIEW;
	m_cSubType=HORIZBEAM_FRONT_VIEW;
}
void CHorizBeamTubeView::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	CSectionView::ToBuffer(buffer,version,doc_type);
	if(version==0||
		(doc_type==1&&version>=5010700)||
		(doc_type==2&&version>=2000800)||
		(doc_type==4&&version>=1030800)||
		(doc_type==5&&version>=1030800))
	{
		buffer.WriteInteger(m_hMasterTube);
		buffer.WriteInteger(m_hJointTube);
	}
}
void CHorizBeamTubeView::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	CSectionView::FromBuffer(buffer,version,doc_type);
	if(version==0||
		(doc_type==1&&version>=5010700)||
		(doc_type==2&&version>=2000800)||
		(doc_type==4&&version>=1030800)||
		(doc_type==5&&version>=1030800))
	{
		buffer.ReadInteger(&m_hMasterTube);
		buffer.ReadInteger(&m_hJointTube);
	}
}
//////////////////////////////////////////////////////////////////////////
//CSlaveSectionView
//////////////////////////////////////////////////////////////////////////
CSlaveSectionView::CSlaveSectionView(int id) : CSectionView(id)
{
	_idViewType=CLS_SLAVE_VIEW;
	m_idHostView=0;
	m_hBaseNode=m_hRootRod=0;
	direction.Set(1,0,0);
	direct_offset=0;	//剖面图符实际标注位置与定位点间沿视线方向的偏移距离
	symbol_dist=10;		//剖面图符(成对)间的距离
	symbol.Copy("");	//剖面图符上的标注文字
}
void CSlaveSectionView::ToBuffer(CBuffer &buffer,long version,long doc_type/* =0 */)
{
	CSectionView::ToBuffer(buffer,version,doc_type);
	if(version==0||
		(doc_type==1&&version>=4010200)||
		(doc_type==2&&version>=1010200)||
		(doc_type==4&&version>=1020102)||
		(doc_type==5&&version>=1020102))
	{
		buffer.WriteInteger(m_idHostView);
		buffer.WritePoint(location);
		buffer.WritePoint(direction);
		buffer.WriteDouble(direct_offset);
		buffer.WriteDouble(symbol_dist);
		buffer.WriteString(symbol);
	}
	if(version==0||
		(doc_type==1&&version>=5020000)||
		(doc_type==2&&version>=2010000)||
		(doc_type==4&&version>=1030900)||
		(doc_type==5&&version>=1030900))
		buffer.WriteInteger(m_hRootRod);
}
void CSlaveSectionView::FromBuffer(CBuffer &buffer,long version/* =NULL */,long doc_type/* =0 */)
{
	CSectionView::FromBuffer(buffer,version,doc_type);
	if(version==0||
		(doc_type==1&&version>=4010200)||
		(doc_type==2&&version>=1010200)||
		(doc_type==4&&version>=1020102)||
		(doc_type==5&&version>=1020102))
	{
		buffer.ReadInteger(&m_idHostView);
		buffer.ReadPoint(location);
		buffer.ReadPoint(direction);
		buffer.ReadDouble(&direct_offset);
		buffer.ReadDouble(&symbol_dist);
		buffer.ReadString(symbol,symbol.GetLengthMax()+1);
	}
	if(version==0||
		(doc_type==1&&version>=5020000)||
		(doc_type==2&&version>=2010000)||
		(doc_type==4&&version>=1030900)||
		(doc_type==5&&version>=1030900))
		buffer.ReadInteger(&m_hRootRod);
}
//////////////////////////////////////////////////////////////////////////
//CSlaveNodeSectionView
//////////////////////////////////////////////////////////////////////////
CSlaveNodeSectionView::CSlaveNodeSectionView(int id) : CSlaveSectionView(id)
{
	_idViewType=CLS_SLAVE_NODESECT_VIEW;
	m_hBaseNode=m_hVertRod=0;
}
void CSlaveNodeSectionView::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	CSlaveSectionView::ToBuffer(buffer,version,doc_type);
	if(version==0||
		(doc_type==1&&version>=5010200)||
		(doc_type==2&&version>=2000700)||
		(doc_type==4&&version>=1030700)||
		(doc_type==5&&version>=1030700))
	{
		buffer.WriteInteger(m_hBaseNode);
		buffer.WriteInteger(m_hVertRod);
	}
}
void CSlaveNodeSectionView::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	CSlaveSectionView::FromBuffer(buffer,version,doc_type);
	if(version==0||
		(doc_type==1&&version>=5010200)||
		(doc_type==2&&version>=2000700)||
		(doc_type==4&&version>=1030700)||
		(doc_type==5&&version>=1030700))
	{
		buffer.ReadInteger(&m_hBaseNode);
		buffer.ReadInteger(&m_hVertRod);
	}
}
//计算法兰半径
static double GetFlR(CLDSParamPlate *pPlate)
{
	if(pPlate==NULL||!pPlate->IsFL())
		return 0;
	double R=0;
	if(pPlate->m_iParamType==TYPE_FL)
	{
		pPlate->GetDesignItemValue('W',&R);
		R*=0.5;
	}
	else if(pPlate->m_bStdPart)
	{
		CLDSStdPartModel stdPartModel;
		if(pPlate->GetStdPartModel(&stdPartModel))
		{
			if(pPlate->m_iParamType==TYPE_FLD||pPlate->m_iParamType==TYPE_FLP)
				R=stdPartModel.param.fl_param.D*0.5;		//对/平焊法兰
			else if(pPlate->m_iParamType==TYPE_FLG||pPlate->m_iParamType==TYPE_FLR)
				R=stdPartModel.param.fl_param2.fOuterD*0.5;	//刚/柔性法兰
		}
	}
	return R;
}
GEPOINT RotatePlaneNormalToDrawing(const double* vPlaneNorm,GECS cs)
{
	GEPOINT prjNormal(vPlaneNorm);
	prjNormal=prjNormal-cs.axis_x*(prjNormal*cs.axis_x);
	double rotAngle=Cal2dLineAng(0,0,prjNormal*cs.axis_y,prjNormal*cs.axis_z);
	double halfPi=0.5*Pi;
	if(rotAngle<=halfPi)
		rotAngle*=-rotAngle;
	else if(rotAngle<=1.5*Pi)
		rotAngle=Pi-rotAngle;
	else
		rotAngle=2*Pi-rotAngle;
	prjNormal=GEPOINT(vPlaneNorm);
	RotateVectorAroundVector(prjNormal,rotAngle,cs.axis_x);
	return prjNormal;
}
void CSlaveSectionView::InitSlaveSectionView(CSectionView *pHostView,IModel *pModel)
{
	if(pHostView==NULL)
		return;
	m_idHostView=pHostView->GetId();
	KEY2C keySymbol=BelongPartGroup()->GetIdleSectSymbol();
	symbol.Copy(keySymbol.Label());
	const int SYMBOL_OFFSET_DIST=10;	//标注点沿标注负方向的偏移距离
	//查找端面图主构件
	CLDSPart *pMainPart=GetMainPartHandle(pModel);
	if(pMainPart)
	{
		CLDSPart *pFatherPart=pModel->FromPartHandle(pMainPart->m_hPartWeldParent);
		if(pFatherPart==NULL)
			return;
		if(m_idViewType==CLS_SLAVE_NODESECT_VIEW)
		{
			CSlaveNodeSectionView *pSectView=(CSlaveNodeSectionView*)this;
			if(pSectView->m_hVertRod==0)
			{	//handle=98
				//TODO:logerr wjh-2018.3.7
			}
			if(pFatherPart->handle!=pSectView->m_hVertRod&&pSectView->m_hVertRod>0)
				pFatherPart=pModel->FromRodHandle(pSectView->m_hVertRod);
		}
		//1.剖视图符方向、标注位置、剖视图宽度初始值
		direction=-ucs.axis_z;
		location=pMainPart->ucs.origin;
		symbol_dist=1.2*pMainPart->GetWidth();			//偏移距离初始值为1.2*主构件宽度
		direct_offset=SYMBOL_OFFSET_DIST;
		//2.父构件为杆件时更新剖视图符参数
		if(pFatherPart->IsLinePart())
		{	
			symbol_dist=0.5*pFatherPart->GetWidth();
			if(pMainPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pMainParamPlate=(CLDSParamPlate*)pMainPart;
				if(pMainParamPlate->IsInsertPlate())
				{
					double L=0;
					pMainParamPlate->GetDesignItemValue('L',&L);
					if(ucs.axis_z*pMainParamPlate->ucs.axis_x>0)
						location+=ucs.axis_z*L;
					else
						location-=ucs.axis_z*L;
				}
				symbol_dist=max(symbol_dist,GetFlR((CLDSParamPlate*)pMainPart));

			}
			//主构件焊接父构件为杆件，且杆件方向与端面图Z轴方向共线
			CLDSLinePart *pLinePart=(CLDSLinePart*)pFatherPart;
			f3dPoint len_vec=pLinePart->End()-pLinePart->Start();
			normalize(len_vec);
			if(fabs(len_vec*direction)>EPS_COS2)
			{
				f3dPoint perp_pos=location;
				SnapPerp(&perp_pos,pLinePart->Start(),pLinePart->End(),location);
				location=perp_pos;
			}
		}
		//3.将剖视图符方向及标注位置转换到宿主视图坐标系下
		//  绘图模块中视图坐标系需进行调整，所以在这儿不进行坐标转换__wxc 2014.8.14
		//  剖视图方向、位置为主视图坐标系下的相对值 wht 15-03-12
		CDrawCoordTrans dcs;
		if(!pHostView->GetDCS(pFatherPart->handle,dcs))
		{
			direction=RotatePlaneNormalToDrawing(direction,pHostView->ucs);
			vector_trans(direction,pHostView->ucs,FALSE);	//转换到宿主视图坐标系下
			coord_trans(location,pHostView->ucs,FALSE);
		}
		else
		{
			direction=dcs.TransVToDraw(direction);
			location=dcs.TransToDraw(location);
		}
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("Failed to get the location of '%s' view,s sectional view sign",sName);
#else
		logerr.Log("'%s'视图的剖视图符位置提取失败",sName);
#endif
}
bool CSlaveSectionView::IsTubeSectionView(CLDSLineTube** ppSectTube/*=NULL*/)
{
	GEPOINT lenStdSectTubeVec;
	CLDSLineTube* pSectTube=NULL;
	PART_ITEM* pItem,*pRootItem;
	pRootItem=this->m_hRootRod>0?SonParts.GetValue(m_hRootRod):SonParts.GetValue(m_pPartGroup->handle);
	if(pRootItem)
		pSectTube=(CLDSLineTube*)m_pPartGroup->BelongModel()->FromPartHandle(pRootItem->hSonPart,CLS_LINETUBE);
	if(pSectTube&&pSectTube->pStart&&pSectTube->pEnd)
	{
		lenStdSectTubeVec=pSectTube->xPosEnd-pSectTube->xPosStart;
		normalize(lenStdSectTubeVec);
		if(fabs(lenStdSectTubeVec*ucs.axis_z)>0.8)	//此处在输出塔脚底板时主管与底板间夹角比较大，固不宜选EPS_COS wjh-2018.3.10
		{
			if(ppSectTube)
				*ppSectTube=pSectTube;
			return true;
		}
	}
	for(pItem=SonParts.GetFirst();pItem;pItem=SonParts.GetNext())
	{
		if(pItem->hSonPart==m_hRootRod||pItem->hSonPart==m_pPartGroup->handle)
			continue;
		if((pSectTube=(CLDSLineTube*)m_pPartGroup->BelongModel()->FromPartHandle(pItem->hSonPart,CLS_LINETUBE))!=NULL)
		{
			lenStdSectTubeVec=pSectTube->xPosEnd-pSectTube->xPosStart;
			normalize(lenStdSectTubeVec);
			if(fabs(lenStdSectTubeVec*ucs.axis_z)>0.8)
			{
				if(ppSectTube)
					*ppSectTube=pSectTube;
				return true;
			}
		}
	}
	if(ppSectTube)
		*ppSectTube=NULL;
	return false;
}

CLDSGeneralPlate* CSlaveSectionView::GetDatumPlateOfFLSectView()
{
	SmartPartPtr pDatumPlate;
	for(PART_ITEM* pItem=SonParts.GetFirst();pItem;pItem=SonParts.GetNext())
	{
		SmartPartPtr pPart=m_pPartGroup->BelongModel()->FromPartHandle(pItem->hSonPart,CLS_PLATE,CLS_PARAMPLATE);
		if(pPart.IsNULL()||fabs(pPart->ucs.axis_z*ucs.axis_z)<EPS_COS2)
			continue;
		LoadTagInfo(pItem)->pCurPart=pPart;
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			if(pPart.pPlate->m_fInnerRadius<EPS)
				continue;
			return pPart.pBasicPlate;
			//CLDSNode *pBaseNode=pPart.pPlate->GetBaseNode();
			//if(pBaseNode==pDatumNode)
			//{
				pDatumPlate=pPart;
			//	break;
			//}
		}
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			double B=0;
			if(pPart.pParamPlate->m_iParamType!=TYPE_CIRRIBPLATE||
				!pPart.pParamPlate->GetDesignItemValue('B',&B,NULL)||fabs(B-360)>EPS)	//B:β,环向扇形角(外形参数)
				continue;
			return pPart.pBasicPlate;
			//CLDSNode *pBaseNode=pPart.pParamPlate->GetBaseNode();
			//if(pBaseNode==pDatumNode)
			//{
			//	pDatumPlate=pPart;
			//	break;
			//}
		}
	}
	return NULL;
}

int CSlaveSectionView::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	int nRetCode=CSectionView::GetPropValueStr(id,valueStr,nMaxStrBufLen);
	if(nRetCode>0)
		return nRetCode;
	//
	CXhChar100 sText;
	if(GetPropID("m_idHostView")==id)
	{
		if(m_pPartGroup)
		{
			CSectionView *pHostView=m_pPartGroup->SonViews.GetValue(m_idHostView);
			if(pHostView)
				sText.Printf("%s(Id:%d)",pHostView->sName,pHostView->GetId());
		}
	}
	else if(GetPropID("location")==id)		//宿主视图绘图坐标系下的剖面图符定位点坐标
		sText.ConvertFromPoint(location);
	else if(GetPropID("location.x")==id)
	{
		sText.Printf("%f",location.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("location.y")==id)
	{
		sText.Printf("%f",location.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("location.z")==id)
	{
		sText.Printf("%f",location.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("direction")==id)		//宿主视图绘图坐标系下的剖面图符视线方向
		sText.ConvertFromPoint(direction);
	else if(GetPropID("direction.x")==id)
	{
		sText.Printf("%f",direction.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("direction.y")==id)
	{
		sText.Printf("%f",direction.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("direction.z")==id)
	{
		sText.Printf("%f",direction.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("direct_offset")==id)	//剖面图符实际标注位置与定位点间沿视线方向的偏移距离
	{
		sText.Printf("%f",direct_offset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("symbol_dist")==id)	//剖面图符(成对)间的距离
	{
		sText.Printf("%f",symbol_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("symbol")==id)		//剖面图符上的标注文字
		sText.Copy(symbol);
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
//////////////////////////////////////////////////////////////////////////
// CKaoMeAngleSectionView
CKaoMeAngleSectionView::CKaoMeAngleSectionView(int id/*=0*/) : CSectionView(id)
{
	_idViewType=CLS_KAOME_ANGLE_VIEW;
	m_hAngle=0;
	m_pAngle=NULL;
}
void CKaoMeAngleSectionView::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	CSectionView::ToBuffer(buffer,version,doc_type);
	if(version==0||
		(doc_type==1&&version>=5000200)||
		(doc_type==2&&version>=2000200)||
		(doc_type==4&&version>=1030200)||
		(doc_type==5&&version>=1030200))
	{
		buffer.WriteInteger(m_hAngle);
		//X肢螺栓记录数
		BUFFERPOP stack(&buffer,boltSetInWingX.GetNodeNum());
		buffer.WriteInteger(boltSetInWingX.GetNodeNum());
		for(CLDSBolt *pBolt=boltSetInWingX.GetFirst();pBolt;pBolt=boltSetInWingX.GetNext())
		{
			buffer.WriteInteger(pBolt->handle);
			stack.Increment();
		}
		if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
			CLDSObject::Log2File()->Log("The number record of bolts in X wing is wrong!");
#else 
			CLDSObject::Log2File()->Log("X肢螺栓记录数出现错误!");
#endif
		//Y肢螺栓记录数
		stack.Initialize(&buffer,boltSetInWingY.GetNodeNum());
		buffer.WriteInteger(boltSetInWingY.GetNodeNum());
		for(CLDSBolt *pBolt=boltSetInWingY.GetFirst();pBolt;pBolt=boltSetInWingY.GetNext())
		{
			buffer.WriteInteger(pBolt->handle);
			stack.Increment();
		}
		if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
			CLDSObject::Log2File()->Log("The number record of bolts in Y wing is wrong!");
#else 
			CLDSObject::Log2File()->Log("Y肢螺栓记录数出现错误!");
#endif
	}
}
void CKaoMeAngleSectionView::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	CSectionView::FromBuffer(buffer,version,doc_type);
	if(version==0||
		(doc_type==1&&version>=5000200)||
		(doc_type==2&&version>=2000200)||
		(doc_type==4&&version>=1030200)||
		(doc_type==5&&version>=1030200))
	{
		buffer.ReadInteger(&m_hAngle);
		CLDSBolt *pBolt=NULL;
		IModel *pModel=BelongPartGroup()->BelongModel();
		m_pAngle=(CLDSLineAngle*)pModel->FromPartHandle(m_hAngle,CLS_LINEANGLE);
		long hBolt=0,i=0,n=buffer.ReadInteger();
		boltSetInWingX.Empty();
		for(i=0;i<n;i++)
		{
			hBolt=buffer.ReadInteger();
			pBolt=(CLDSBolt*)pModel->FromPartHandle(hBolt,CLS_BOLT);
			if(pBolt)
				boltSetInWingX.append(pBolt);
		}
		n=buffer.ReadInteger();
		boltSetInWingY.Empty();
		for(i=0;i<n;i++)
		{
			hBolt=buffer.ReadInteger();
			pBolt=(CLDSBolt*)pModel->FromPartHandle(hBolt,CLS_BOLT);
			if(pBolt)
				boltSetInWingY.append(pBolt);
		}
	}
}
void CKaoMeAngleSectionView::InitView(const KAOMEANGLE &kmAngle)
{
	m_hAngle=kmAngle.hAngle;
	m_pAngle=kmAngle.pAngle;
	boltSetInWingX.Empty();
	CLDSBolt *pBolt=NULL;
	if(kmAngle.pBoltsInWingX)
	{
		for(pBolt=kmAngle.pBoltsInWingX->GetFirst();pBolt;pBolt=kmAngle.pBoltsInWingX->GetNext())
			boltSetInWingX.append(pBolt);
	}
	boltSetInWingY.Empty();
	if(kmAngle.pBoltsInWingY)
	{
		for(pBolt=kmAngle.pBoltsInWingY->GetFirst();pBolt;pBolt=kmAngle.pBoltsInWingY->GetNext())
			boltSetInWingY.append(pBolt);
	}
}
CLDSBolt* CKaoMeAngleSectionView::EnumFirstBolt(BOOL bInWingX)
{
	if(bInWingX)
		return boltSetInWingX.GetFirst();
	else
		return boltSetInWingY.GetFirst();
}
CLDSBolt* CKaoMeAngleSectionView::EnumNextBolt(BOOL bInWingX)
{
	if(bInWingX)
		return boltSetInWingX.GetNext();
	else
		return boltSetInWingY.GetNext();
}
//////////////////////////////////////////////////////////////////////////
// CAngleSpreadSectionView
CAngleSpreadSectionView::CAngleSpreadSectionView(int id/*=0*/) : CSectionView(id)
{
	_idViewType=CLS_ANGLE_SPREAD_VIEW;
}
//////////////////////////////////////////////////////////////////////////
// 
CTubeBushPlateView::CTubeBushPlateView(int id/*=0*/) : CSectionView(id)
{
	_idViewType=CLS_BUSH_PLATE_VIEW;
}
////////////////////////////////////////////////////////////////////////////////
//CLDSPartGroup
////////////////////////////////////////////////////////////////////////////////
static CSectionView* CreateSectionView(int iViewType,DWORD key,void* pContext)
{
	CSectionView *pView=NULL;
	switch(iViewType)
	{
	case CSectionView::CLS_BASIC_VIEW:
		pView = new CSectionView(key);
		break;
	case CSectionView::CLS_SLAVE_VIEW:
		pView = new CSlaveSectionView(key);
		break;
	case CSectionView::CLS_STDX_PLATES_VIEW:
		pView = new CStdXPlatesView(key);
		break;
	case CSectionView::CLS_ANGLE_FOOT_VIEW:
		pView = new CAngleFootView(key);
		break;
	case CSectionView::CLS_KAOME_ANGLE_VIEW:
		pView = new CKaoMeAngleSectionView(key);
		break;
	case CSectionView::CLS_ANGLE_SPREAD_VIEW:
		pView = new CAngleSpreadSectionView(key);
		break;
	case CSectionView::CLS_JOINT_TUBES_VIEW:
		pView = new CJointTubesView(key);
		break;
	case CSectionView::CLS_SLAVE_NODESECT_VIEW:
		pView = new CSlaveNodeSectionView(key);
		break;
	case CSectionView::CLS_HORIZ_BEAM_VIEW:
		pView = new CHorizBeamTubeView(key);
		break;
	case CSectionView::CLS_DUALTUBE_JOINT_VIEW:
		pView = new CDualTubeJointView(key);
		break;
	case CSectionView::CLS_BUSH_PLATE_VIEW:
		pView = new CTubeBushPlateView(key);
		break;
	default:
		pView = new CSectionView(key);
	}
	pView->SetBelongPartGroup((CLDSPartGroup*)pContext);
	//memcpy(&pView->GetViewClsType(),&iViewType,4);
	return pView;
}
static BOOL DestroyView(CSectionView* pView)
{
	switch(pView->GetViewTypeId())
	{
	case CSectionView::CLS_BASIC_VIEW:
		delete pView;
		break;
	case CSectionView::CLS_SLAVE_VIEW:
		delete (CSlaveSectionView*)pView;
		break;
	case  CSectionView::CLS_STDX_PLATES_VIEW:
		delete (CStdXPlatesView*)pView;
		break;
	case CSectionView::CLS_ANGLE_FOOT_VIEW:
		delete (CAngleFootView*)pView;
		break;
	case CKaoMeAngleSectionView::CLS_KAOME_ANGLE_VIEW:
		delete (CKaoMeAngleSectionView*)pView;
		break;
	case CSectionView::CLS_ANGLE_SPREAD_VIEW:
		delete (CAngleSpreadSectionView*)pView;
		break;
	case CSectionView::CLS_JOINT_TUBES_VIEW:
		delete (CJointTubesView*)pView;
		break;
	case CSectionView::CLS_SLAVE_NODESECT_VIEW:
		delete (CSlaveNodeSectionView*)pView;
		break;
	case CSectionView::CLS_HORIZ_BEAM_VIEW:
		delete (CHorizBeamTubeView*)pView;
		break;
	case CSectionView::CLS_DUALTUBE_JOINT_VIEW:
		delete (CDualTubeJointView*)pView;
		break;
	case CSectionView::CLS_BUSH_PLATE_VIEW:
		delete (CTubeBushPlateView*)pView;
		break;
	default:
		delete pView;
	}
	return TRUE;
}
CLDSPartGroup::CLDSPartGroup()
{
	handle=m_uStatMatCoef=0;
	strcpy(sFolderName,"");
	strcpy(sGroupName,"");
	m_uStatMatCoef=1;
	_iPartGroupType=0;
	m_hJointTube=0;
	m_bFootPartGroup=FALSE;
	SonViews.m_pContext = this;
	SonViews.CreateNewAtom=CreateSectionView;
	SonViews.DeleteAtom=DestroyView;
}
int CLDSPartGroup::get_iPartGroupType()	//组件类型
{
	return this->_iPartGroupType;
}
//为方便何时更改组焊类型，将此变量改为属性 wjh-2019.8.9
int CLDSPartGroup::set_iPartGroupType(int iValType)
{ //组件类型
	return this->_iPartGroupType=iValType;
}
void CLDSPartGroup::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteInteger(handle);
	buffer.WriteString(sFolderName);
	buffer.WriteString(sGroupName);
	buffer.WriteDword(m_uStatMatCoef);
	buffer.WritePoint(ucs.origin);
	buffer.WritePoint(ucs.axis_x);
	buffer.WritePoint(ucs.axis_y);
	buffer.WritePoint(ucs.axis_z);
	buffer.WriteInteger(m_iPartGroupType);	//标准组焊件
	buffer.WriteInteger(m_bFootPartGroup);	//塔脚板组焊件 wht 10-01-14
	BUFFERPOP stack(&buffer,SonViews.GetNodeNum());
	buffer.WriteInteger(SonViews.GetNodeNum());
	for(CSectionView *pView=SonViews.GetFirst();pView;pView=SonViews.GetNext())
	{
		if(version==0||
			(doc_type==1&&version>=4010200)||
			(doc_type==2&&version>=1010200)||
			(doc_type==4&&version>=1020200)||
			(doc_type==5&&version>=1020200))
			buffer.WriteInteger(pView->GetViewTypeId());
		pView->ToBuffer(buffer,version,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of son view is wrong!");
#else
		CLDSObject::Log2File()->Log("子视图记录数出现错误!");
#endif
}
void CLDSPartGroup::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[500]="";
	fprintf(fp,"<组合构件信息 handle=\"0x%x\" FolderName=\"%s\" GroupName=\"%s\">\n",handle,sFolderName,sGroupName);
	fprintf(fp,"<构件组数目 StatMatCoef=\"%d\"/>\n",m_uStatMatCoef);
	fprintf(fp,"<原点坐标 origin=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.origin.x,ucs.origin.y,ucs.origin.z);
	fprintf(fp,"<X轴坐标 axis_x=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.axis_x.x,ucs.axis_x.y,ucs.axis_x.z);
	fprintf(fp,"<Y轴坐标 axis_y=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.axis_y.x,ucs.axis_y.y,ucs.axis_y.z);
	fprintf(fp,"<Z轴坐标 axis_z=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.axis_z.x,ucs.axis_z.y,ucs.axis_z.z);
	if(m_iPartGroupType==GROUP_ENDCONN)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<标准组焊件 bStdPartGroup=\"%s\"/>\n",sText);
	if(m_bFootPartGroup==1)
		strcpy(sText,"true");
	else
		strcpy(sText,"false");
	fprintf(fp,"<塔脚板组焊件 bFootPartGroup=\"%s\"/>\n",sText);
	fprintf(fp,"<子视图数量 SonViewNum=\"%d\"/>\n",SonViews.GetNodeNum());
	for(CSectionView *pView=SonViews.GetFirst();pView;pView=SonViews.GetNext())
		pView->ToXmlFile(fp,schema);
	fprintf(fp,"</组合构件信息>\n");
}
//doc_tma1_lma2_tsa3_lds4_default0
void CLDSPartGroup::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	buffer.ReadInteger(&handle);
	buffer.ReadString(sFolderName,17);
	buffer.ReadString(sGroupName,51);
	buffer.ReadDword(&m_uStatMatCoef);
	buffer.ReadPoint(ucs.origin);
	buffer.ReadPoint(ucs.axis_x);
	buffer.ReadPoint(ucs.axis_y);
	buffer.ReadPoint(ucs.axis_z);
	buffer.ReadInteger(&_iPartGroupType);	//标准组焊件
	if(  version==0||doc_type==5||			//TDA
		(doc_type==1&&version>=4000025)||	//TMA V4.0.0.25
		(doc_type==2&&version>=1000080)||	//LMA V1.0.0.80
		(doc_type==4&&version>=1000007))	//LDS V1.0.0.7
	{
		buffer.ReadInteger(&m_bFootPartGroup);//塔脚组焊件 wht 10-01-14
		if(m_bFootPartGroup)
			m_iPartGroupType=GROUP_ANGLEFOOT;
	}
	int i,n;
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		int idViewType=CSectionView::CLS_BASIC_VIEW;
		if(version==0||
			(doc_type==1&&version>=4010200)||
			(doc_type==2&&version>=1010200)||
			(doc_type==4&&version>=1020200)||
			(doc_type==5&&version>=1020200))
			buffer.ReadInteger(&idViewType);
		CSectionView *pView=SonViews.Add(0,idViewType);
		pView->FromBuffer(buffer,version,doc_type);
	}
}

//通过旧视图信息生成需要的新视图 主要是在将节点板视图细分为环向封板视图，环向肋板，跨越连板时使用  wht 09-12-12
static CSectionView *AddNewViewByOldView(CLDSPartGroup *pPartGroup,CSectionView *pMainView,CSectionView *pOldView,
										 SECTION_VIEW_TYPE iViewType,int &view_index)
{	
	if(pPartGroup==NULL||pOldView==NULL)
		return NULL;
	CSlaveNodeSectionView *pSectionView=(CSlaveNodeSectionView*)pPartGroup->SonViews.Add(0,CSectionView::CLS_SLAVE_NODESECT_VIEW);
	pSectionView->index=view_index++;
	pSectionView->m_iViewType=iViewType;
	pSectionView->m_bLocked=TRUE;	//新添加的端面图必须锁定
	pSectionView->ucs=pOldView->ucs;
	pSectionView->des_cs.SetParamInfo(pOldView->des_cs.GetParamInfo());
	pSectionView->SonParts.Empty();
	//
	CXhChar100 sName;
	bool bSetOrg=false;
	for(CSectionView::PART_ITEM *pPartItem=pOldView->SonParts.GetFirst();pPartItem;pPartItem=pOldView->SonParts.GetNext())
	{
		CSuperSmartPtr<CLDSPart> pTempPart;
		if(TagPart(pPartItem)==NULL||(TagPart(pPartItem)->handle!=pPartItem->hSonPart))
			pTempPart=pPartGroup->BelongModel()->FromPartHandle(pPartItem->hSonPart);
		else
			pTempPart=TagPart(pPartItem);
		if(pTempPart.IsNULL())
			continue;
		int iPartType=pTempPart->GetClassTypeId();
		if(iViewType==VIEW_NODEPLATE)
		{	//节点板端面视图
			if(pTempPart->IsLinePart()&&pTempPart.pRod->pStart&&pTempPart.pRod->pEnd)
			{
				GEPOINT lenVec=pTempPart.pRod->End()-pTempPart.pRod->Start();
				normalize(lenVec);
				if(fabs(lenVec*pOldView->ucs.axis_z)>EPS_COS2&&fabs(lenVec*pOldView->ucs.axis_z)<EPS_COS)
					continue;	//该杆件非当前主截面杆件的支(管)杆件，跳过 wjh-2016.11.13	
			}
			else if(iPartType==CLS_PARAMPLATE)
			{	//TODO:值得商榷直接粗暴的把这三种类型过滤掉 wjh-2017.6.27
				int iType=pTempPart.ParamPlatePointer()->m_iParamType;
				if(iType==TYPE_TUBERIBPLATE||iType==TYPE_CIRRIBPLATE||iType==TYPE_CIRCOVERPLATE)
					continue;
			}
			else if(iPartType==CLS_PLATE)
			{	//TODO:值得商榷 wjh-2017.6.27
				if(pTempPart.PlatePointer()->m_fInnerRadius>0)
					continue;
#ifdef AFX_TARG_ENU_ENGLISH
				sName.Printf("%s Circular Plate End Plane View",(char*)pTempPart->GetPartNo());
#else
				sName.Printf("%s节点板端面视图",(char*)pTempPart->GetPartNo());
#endif
			}
			if(!bSetOrg&&pTempPart->IsPlate())
			{
				CLDSNode* pBaseNode=pTempPart.GeneralPlatePointer()->GetBaseNode();
				CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
				if(pBaseNode&&param.desOrg.datum_pos_style==1)
				{
					bSetOrg=true;
					long hRod=param.desOrg.des_para.RODEND.hRod;
					param.desOrg.datum_pos_style=2;
					param.desOrg.des_para.RODNODE.hRod=hRod;
					param.desOrg.des_para.RODNODE.hNode=pBaseNode->handle;
					pSectionView->des_cs.SetParamInfo(param);
					pSectionView->UpdateUCS(pTempPart->BelongModel(),pPartGroup);
					pSectionView->m_hBaseNode=pBaseNode->handle;
				}
			}
			pSectionView->AddPart(pTempPart->handle,pTempPart);
		}
		else if(iViewType==VIEW_CIRPLATE)
		{	//环向板端面图
			if(iPartType==CLS_PARAMPLATE)
			{
				int iType=pTempPart.ParamPlatePointer()->m_iParamType;
				if(iType==TYPE_TUBERIBPLATE||iType==TYPE_CIRRIBPLATE||iType==TYPE_CIRCOVERPLATE)
					continue;
			}
			if(pTempPart->GetClassTypeId()==CLS_PLATE)
			{
				if(pTempPart.PlatePointer()->m_fInnerRadius>0)
#ifdef AFX_TARG_ENU_ENGLISH
					sName.Printf("%s Circular Plate End Plane View",(char*)pTempPart->GetPartNo());
#else
					sName.Printf("%s环向板端面视图",(char*)pTempPart->GetPartNo());
#endif
			}
			if(!bSetOrg)
			{
				bSetOrg=true;
				CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
				param.desOrg.datum_pos_style=9;
				param.desOrg.des_para.hPart=pTempPart->handle;
				param.desOrg.datum_pos.Set();
				pSectionView->des_cs.SetParamInfo(param);
				pSectionView->des_cs.UpdateACS(pTempPart->BelongModel());
			}
			pSectionView->AddPart(pTempPart->handle,pTempPart);
		}
		else //if(iViewType==VIEW_TUBERIBPLATE||iViewType==VIEW_CIRCOVERPLATE)
		{	//环形封板视图|环形肋板视图
			if(iPartType!=CLS_PARAMPLATE)
				continue;
			int iParamType=pTempPart.ParamPlatePointer()->m_iParamType;
			if(iViewType==VIEW_TUBERIBPLATE&&iParamType==TYPE_CIRCOVERPLATE)
				continue;
			else if(iViewType==VIEW_CIRCOVERPLATE&&(iParamType==TYPE_TUBERIBPLATE||iParamType==TYPE_CIRRIBPLATE))
				continue;
#ifdef AFX_TARG_ENU_ENGLISH
			if(iParamType==TYPE_TUBERIBPLATE)
				sName.Printf("%s Across Linking Plate View",(char*)pTempPart->GetPartNo());
			else if(iParamType==TYPE_CIRRIBPLATE)
				sName.Printf("%s Circular Rib Plate View",(char*)pTempPart->GetPartNo());
			else if(iParamType==TYPE_CIRCOVERPLATE)
				sName.Printf("%s Circular Over Plate View",(char*)pTempPart->GetPartNo());
#else
			if(iParamType==TYPE_TUBERIBPLATE)
				sName.Printf("%s跨越连板视图",(char*)pTempPart->GetPartNo());
			else if(iParamType==TYPE_CIRRIBPLATE)
				sName.Printf("%s环向肋板视图",(char*)pTempPart->GetPartNo());
			else if(iParamType==TYPE_CIRCOVERPLATE)
				sName.Printf("%s环向封板视图",(char*)pTempPart->GetPartNo());
#endif
			if(!bSetOrg)
			{
				bSetOrg=true;
				CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
				param.desOrg.datum_pos_style=9;
				param.desOrg.des_para.hPart=pTempPart->handle;
				param.desOrg.datum_pos.Set();
				pSectionView->des_cs.SetParamInfo(param);
				pSectionView->des_cs.UpdateACS(pTempPart->BelongModel());
			}
			//添加环形板关联构件
			pSectionView->AddPart(pTempPart->handle,pTempPart);
			long hDatumTube=0,hRelPart1=0,hRelPart2=0;
			if(iViewType==VIEW_TUBERIBPLATE)
			{
				pTempPart.ParamPlatePointer()->GetDesignItemValue('E',&hRelPart1);
				pTempPart.ParamPlatePointer()->GetDesignItemValue('F',&hRelPart2);
			}
			else if(iViewType==VIEW_CIRCOVERPLATE)
			{
				pTempPart.ParamPlatePointer()->GetDesignItemValue('F',&hRelPart1);
				pTempPart.ParamPlatePointer()->GetDesignItemValue('G',&hRelPart2);
			}
			pTempPart.ParamPlatePointer()->GetDesignItemValue('C',&hDatumTube);
			CLDSPart* pDatumTube=pPartGroup->BelongModel()->FromPartHandle(hDatumTube);
			if(pDatumTube&&pOldView->SonParts.GetValue(hDatumTube))
				pSectionView->AddPart(hDatumTube,pDatumTube);
			CLDSPart* pRelPart1=pPartGroup->BelongModel()->FromPartHandle(hRelPart1);
			if(pRelPart1&&pOldView->SonParts.GetValue(hRelPart1))
				pSectionView->AddPart(hRelPart1,pRelPart1);
			CLDSPart* pRelPart2=pPartGroup->BelongModel()->FromPartHandle(hRelPart2);
			if(pRelPart2&&pOldView->SonParts.GetValue(hRelPart2))
				pSectionView->AddPart(hRelPart2,pRelPart2);
		}
		if(sName.GetLength()==0&&pTempPart->IsPlate()&&fabs(pTempPart->ucs.axis_z*pSectionView->ucs.axis_z)>EPS_COS2)
			sName.Copy(pTempPart->GetPartNo());
	}
	//识别与当前有效钢板的焊接依赖钢板未包在视图构件集的情况（焊接边的焊接依赖杆件非焊接父构件）
	/*for(CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)plateset.GetFirst();pPlate;pPlate=(CLDSGeneralPlate*)plateset.GetNext())
	{
		pPlate->IntelliRecogWeldLine(&partset);
		for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			if(pVertex->m_bWeldEdge&&pSectionView->SonParts.GetValue(pVertex->weldline.hWeldMotherPart)==NULL)
			{
				CLDSPart* pWeldPart=partset.FromHandle(pVertex->weldline.hWeldMotherPart);
				if(pWeldPart)
					pSectionView->AddPart(pWeldPart->handle,pWeldPart);
			}
		}
	}*/
	strcpy(pSectionView->sName,sName);
	//根据宿主视图初始化子视图参数
	pSectionView->InitSlaveSectionView(pMainView,pPartGroup->BelongModel());
	return pSectionView;
}

//添加默认的组焊件附加视图，在添加组焊件附加视图时为每个视图添加默认的标定点，检测线以及角度标注线
void AdjustViewNorm(f3dPoint &axis_z,CLDSPart *pDatumPart)
{
	if(pDatumPart==NULL)
		return;
	f3dPoint verfiy_vec;
	if(pDatumPart->layer(2)=='1'||pDatumPart->layer(2)=='2')
		verfiy_vec.Set(0,1,0);
	else if(pDatumPart->layer(2)=='3'||pDatumPart->layer(2)=='4')
		verfiy_vec.Set(0,-1,0);
	else if(pDatumPart->layer(2)=='Q'||pDatumPart->layer(2)=='q')
		verfiy_vec.Set(0,1,0);
	else if(pDatumPart->layer(2)=='H'||pDatumPart->layer(2)=='h')
		verfiy_vec.Set(0,-1,0);
	else if(pDatumPart->layer(2)=='Z'||pDatumPart->layer(2)=='z')
		verfiy_vec.Set(-1,0,0);
	else if(pDatumPart->layer(2)=='Y'||pDatumPart->layer(2)=='y')
		verfiy_vec.Set(1,0,0);
	else 
		return;
	if(axis_z*verfiy_vec<0)
		axis_z*=-1.0;
}
//1.添加对接相贯钢管视图	移至JointTubePartGroup.cpp
//BOOL CLDSPartGroup::AddJointTubeViews(CLDSLineTube *pRootTube,PARTSET &partset);

//2.四根支管相贯与主管的K节点类组焊件 wht 10-08-13
BOOL CLDSPartGroup::AddKNodeTransectViews(CLDSLineTube *pRootTube,PARTSET &partset)
{
	if(pRootTube==NULL||pRootTube->handle!=handle)
		return FALSE;
	TUBESET branchTubeSet;
	for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_LINETUBE)
			continue;
		CLDSLineTube *pBranchTube=(CLDSLineTube*)pPart;
		if((pBranchTube->m_tJointStart.type==1&&pBranchTube->m_tJointStart.hLinkObj==pRootTube->handle)
			||(pBranchTube->m_tJointEnd.type==1&&pBranchTube->m_tJointEnd.hLinkObj==pRootTube->handle))
			branchTubeSet.append(pBranchTube);
	}
	if(branchTubeSet.GetNodeNum()>=4)
	{
		//1.主视图
		int view_index=1;
		//CSectionView *pMainView = SonViews.append();
		CSectionView *pMainView = SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
		pMainView->index=view_index;	//视图索引
		view_index++;
		CParaCS::PARAMETRIC_INFO param = pMainView->des_cs.GetParamInfo();
		//基准点
		param.desOrg.datum_pos_style=1;
		param.desOrg.des_para.RODEND.hRod=pRootTube->handle;
		param.desOrg.des_para.RODEND.bIncOddEffect=TRUE;
		param.desOrg.des_para.RODEND.direction=0;
		//坐标轴
		param.m_eDatumPlane=CParaCS::DP_XY;
		param.desAxisX.norm_style=5;
		param.desAxisX.hVicePart=pRootTube->handle;
		param.desAxisX.direction=0;
		//
		CLDSLineTube *pFirstTube=branchTubeSet.GetFirst();
		param.desAxisY.norm_style=5;
		param.desAxisY.hVicePart=pFirstTube->handle;
		param.desAxisY.direction=0;
		pMainView->des_cs.SetParamInfo(param);
		pMainView->UpdateUCS(m_pModel,this);

		//调整主视图法线方向
		f3dPoint axis_z=pMainView->ucs.axis_z;
		AdjustViewNorm(axis_z,pRootTube);	
		if(axis_z*pMainView->ucs.axis_z<0)
		{
			//pMainView->des_cs.desAxisY.direction=1;
			param.desAxisY.direction=1;
			pMainView->des_cs.SetParamInfo(param);
			pMainView->UpdateUCS(m_pModel,this);
		}

#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(pMainView->sName,"Main View");
#else
		sprintf(pMainView->sName,"主视图");
#endif
		pMainView->m_iViewType=VIEW_MAIN;
		pMainView->AddPart(pRootTube->handle,pRootTube);
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			pMainView->AddPart(pPart->handle,pPart);
		//2.简图
		CSectionView *pView=NULL;
		//pView=SonViews.append();
		pView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
		pView->index=view_index;
		view_index++;
		pView->des_cs=pMainView->des_cs;
		pView->ucs=pMainView->ucs;
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(pView->sName,"Front Sketch");
#else
		sprintf(pView->sName,"正面简图");
#endif
		pView->m_iViewType=VIEW_KNODESKETCH;
		pView->AddPart(pRootTube->handle,pRootTube);
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(pPart->GetClassTypeId()!=CLS_PARAMPLATE)
				continue;
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
			if(!pParamPlate->IsFL())
				continue;
			//if(fabs(pParamPlate->ucs.axis_z*pView->ucs.axis_z)>EPS2)
			//	continue;
			pView->AddPart(pParamPlate->handle,pParamPlate);
		}
		//3.侧面视图
		//pView=SonViews.append();
		pView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
		pView->index=view_index;
		view_index++;
		param = pView->des_cs.GetParamInfo();
		param.desOrg=pMainView->des_cs.GetParamInfo().desOrg;
		param.m_eDatumPlane=CParaCS::DP_XZ;
		param.desAxisX=pMainView->des_cs.GetParamInfo().desAxisX;
		param.desAxisZ=pMainView->des_cs.GetParamInfo().desAxisY;
		pView->des_cs.SetParamInfo(param);
		//TODO:是不是有问题,替换成上一行代码 wjh 2012.11.20
		//pMainView->UpdateUCS(m_pModel,this);

#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(pView->sName,"Side View");
#else
		sprintf(pView->sName,"侧面视图");
#endif
		pView->m_iViewType=VIEW_FRONT;
		pView->AddPart(pRootTube->handle,pRootTube);
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(pPart->handle==pRootTube->handle)
				continue;
			CLDSLineTube *pTempTube=NULL;
			for(pTempTube=branchTubeSet.GetFirst();pTempTube;pTempTube=branchTubeSet.GetNext())
			{
				if(pTempTube->handle==pPart->handle)
					break;
			}
			if(pTempTube)
				continue;
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
				if(pParamPlate->IsFL()
					&&pParamPlate->m_hPartWeldParent!=pRootTube->handle)
					continue;
			}
			if(pPart->GetClassTypeId()==CLS_PLATE)
			{	
				if(fabs(pPart->ucs.axis_z*pView->ucs.axis_z)>EPS_COS2)
					pView->AddPart(pPart->handle,pPart);
			}
			else 
				pView->AddPart(pPart->handle,pPart);
		}
		//4.端面视图
		//pView=SonViews.append();
		pView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
		pView->index=view_index;
		view_index++;
		param = pView->des_cs.GetParamInfo();
		param.desOrg=pMainView->des_cs.GetParamInfo().desOrg;
		param.m_eDatumPlane=CParaCS::DP_ZX;
		param.desAxisZ=pMainView->des_cs.GetParamInfo().desAxisX;
		param.desAxisX=pMainView->des_cs.GetParamInfo().desAxisY;
		pView->des_cs.SetParamInfo(param);
		pView->UpdateUCS(m_pModel,this);
		
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(pView->sName,"End Plane View");
#else
		sprintf(pView->sName,"端面视图");
#endif
		pView->m_iViewType=VIEW_NODEPLATE;
		pView->AddPart(pRootTube->handle,pRootTube);
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
				if(pParamPlate->IsFL()||pParamPlate->m_iParamType==TYPE_ELBOWPLATE)
					continue;
			}
			CLDSLineTube *pTempTube=NULL;
			for(pTempTube=branchTubeSet.GetFirst();pTempTube;pTempTube=branchTubeSet.GetNext())
			{
				if(pTempTube->handle==pPart->handle)
					break;
			}
			if(pTempTube)
				continue;
			pView->AddPart(pPart->handle);
		}
		return TRUE;
	}
	return FALSE;
}
//3.添加基准构件为杆件的组焊件视图
//校正钢板工作平面(绘图)法线的正向，以使钢板尽量接尽其自然视角 wjh-2013.12.07
GEPOINT AdjustPlateWorkPlaneNorm(const double* axis_z,char cPosLayer)
{
	GEPOINT work_norm(axis_z);
	if(toupper(cPosLayer)=='Q'&&fabs(work_norm.y)>0.707&&work_norm.y<0)	//前视图钢板，法线应尽量显示为Y轴正向
		work_norm *=-1.0;
	else if(toupper(cPosLayer)=='H'&&fabs(work_norm.y)>0.707&&work_norm.y>0)	//前视图钢板，法线应尽量显示为Y轴正向
		work_norm *=-1.0;
	else if(toupper(cPosLayer)=='Z'&&fabs(work_norm.x)>0.707&&work_norm.x>0)	//前视图钢板，法线应尽量显示为Y轴正向
		work_norm *=-1.0;
	else if(toupper(cPosLayer)=='Y'&&fabs(work_norm.x)>0.707&&work_norm.x<0)	//前视图钢板，法线应尽量显示为Y轴正向
		work_norm *=-1.0;
	return work_norm;
}
//<DEVELOP_PROCESS_MARK nameId="CLDSPartGroup::ExtractSectionSegs">
//3.1 初始化组焊件工作面
//3.2 添加工作面对应的视图
//3.3 基准构件为钢管时需要添加钢管端面视图
//3.3.1 无主视图(如钢管两端均为法兰且钢管上无连接板的情况)时添加主视图
//3.3.2 添加钢管中间节点对应的端面视图
//提取SectionSegs
static void ExtractSectionSegs(CLDSLineTube *pLineTube,PARTSET &partset,ATOM_LIST<SECTION_SEG> &sectionSegList)
{	//1.遍历partset将钢板和指定类型的参数化板向基准钢板投影，计算构件对应的投影线段(保证投影线段有序)
	ATOM_LIST<SECTION_PART> sectionPartList;
	//统计汇总partset中所有钢管关联的法兰插板以及附属件
	CHashTable<long> hashPartHandleByHandle;
	hashPartHandleByHandle.CreateHashTable(partset.GetNodeNum());
	if(partset.FromHandle(pLineTube->handle)==NULL)
		partset.append(pLineTube);
	CLDSPart *pPart=NULL;
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(!pPart->IsTube())
			continue;
		CLDSLineTube *pTempTube=(CLDSLineTube*)pPart;
		for(int i=0;i<2;i++)
		{
			long hEndLj=pTempTube->m_tJointStart.hLinkObj;
			if(i==1)
				hEndLj=pTempTube->m_tJointEnd.hLinkObj;
			CLDSParamPlate *pEndLjPlate=(CLDSParamPlate*)pLineTube->BelongModel()->FromPartHandle(hEndLj,CLS_PARAMPLATE);
			if(pEndLjPlate)
			{
				if(!hashPartHandleByHandle.ContainsKey(pEndLjPlate->handle))
					hashPartHandleByHandle.SetValueAt(pEndLjPlate->handle,pEndLjPlate->handle);
				for(ATTACHPART_ITEM *pItem=pEndLjPlate->GetFirstAttachPart();pItem;pItem=pEndLjPlate->GetNextAttachPart())
				{
					if(!hashPartHandleByHandle.ContainsKey(pItem->h))
						hashPartHandleByHandle.SetValueAt(pItem->h,pItem->h);
				}
			}
		}
	}
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		ATOM_LIST<PROFILE_VERTEX> *pVertexList=NULL;
		ATOM_LIST<PROFILE_VERTEX> temVertexList;
		//法兰，插板附加板不在此处添加端面视图,存在由于添加特殊工艺将参数化肘板变为普通钢板情况 wjh-2017.11.17
		if(pPart->IsPlate()&&hashPartHandleByHandle.ContainsKey(pPart->handle))
			continue;
		if(pPart->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pPart)->m_fInnerRadius>0)
			continue;//塔脚法兰式底板会生成单独视图，不需要重复生成节点剖面图　wjh-2015.9.10
		else if(pPart->GetClassTypeId()==CLS_PLATE&&pPart->m_hPartWeldParent==pLineTube->handle)
		{	//连板焊接父构件必须为当前钢管，否则如十字插板的立板会引起混乱 wjh-2015.1.30
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			pVertexList=&pPlate->vertex_list;
		}
		else if(pPart->GetClassTypeId()==CLS_PLATE)
		{	//处理焊接父构件虽不是钢管，但确有一边焊到当前钢管的塔脚靴板型钢板 wjh-2016.10.16
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			pPlate->IntelliRecogWeldLine(&partset);
			for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			{
				if(pVertex->m_bWeldEdge&&pVertex->weldline.hWeldMotherPart==pLineTube->handle)
				{
					pVertexList=&pPlate->vertex_list;
					break;
				}
			}
		}
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
			//仅生成以下三种参数化板对应的端面图 wht 12-09-06
			if(pParamPlate->m_iParamType!=TYPE_CIRCOVERPLATE&&
				pParamPlate->m_iParamType!=TYPE_CIRRIBPLATE&&
				pParamPlate->m_iParamType!=TYPE_TUBERIBPLATE)
				continue;
			//法兰，插板附加板不在此处添加端面视图
			if(pParamPlate&&hashPartHandleByHandle.ContainsKey(pParamPlate->handle))
				continue;
			if(pParamPlate->vertex_list.GetNodeNum()<=3)
				pParamPlate->DesignPlate(false);
			pVertexList=&pParamPlate->vertex_list;
		}
		else if(pPart->GetClassTypeId()==CLS_LINETUBE&&
			(((CLDSLineTube*)pPart)->pStart==NULL||((CLDSLineTube*)pPart)->pEnd==NULL))
		{	//部分焊接钢板上的套管 wjh-2017.6.27
			CLDSLineTube* pTube=(CLDSLineTube*)pPart;
			pTube->BuildUCS();
			f3dPoint startPt=pTube->Start(),endPt=pTube->End();
			coord_trans(startPt,pTube->ucs,FALSE);
			coord_trans(endPt,pTube->ucs,FALSE);
			temVertexList.append(PROFILE_VERTEX(startPt));
			temVertexList.append(PROFILE_VERTEX(endPt));
			pVertexList=&temVertexList;
		}
		if(pVertexList==NULL)
			continue;
		int vertex_n=pVertexList->GetNodeNum();
		if(vertex_n<1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pLineTube->Log2File()->Log("0x%X part{%s}'s profile vertices are not enough (%d,minimal 3 vertices)",pPart->handle,(char*)pPart->GetPartNo(),vertex_n);
#else
			pLineTube->Log2File()->Log("0x%X构件{%s}缺少足够(%d,>2)的外形顶点数量",pPart->handle,(char*)pPart->GetPartNo(),vertex_n);
#endif
			continue;
		}
		SECTION_PART sectionPart;
		f3dLine datum_line(pLineTube->Start(),pLineTube->End());
		for(PROFILE_VERTEX *pVertex=pVertexList->GetFirst();pVertex;pVertex=pVertexList->GetNext())
		{
			f3dPoint vertex=pVertex->vertex,prjPt;
			coord_trans(vertex,pPart->ucs,TRUE);
			SnapPerp(&prjPt,datum_line,vertex);
			coord_trans(prjPt,pLineTube->ucs,FALSE);
			if(sectionPart.pPart==NULL)
			{
				sectionPart.prjLine.startPt=sectionPart.prjLine.endPt=prjPt;
				sectionPart.pPart=pPart;
			}
			else if(prjPt.z<sectionPart.prjLine.startPt.z)
				sectionPart.prjLine.startPt=prjPt;
			else if(prjPt.z>sectionPart.prjLine.endPt.z)
				sectionPart.prjLine.endPt=prjPt;
		}
		SECTION_PART *pSectionPart=NULL;
		for(pSectionPart=sectionPartList.GetFirst();pSectionPart;pSectionPart=sectionPartList.GetNext())
		{
			if(pSectionPart->prjLine.startPt.z>sectionPart.prjLine.startPt.z)
			{
				sectionPartList.insert(sectionPart);
				break;
			}
		}
		if(pSectionPart==NULL&&sectionPart.pPart)
			sectionPartList.append(sectionPart);
	}
	//2.根据指定的阈值将SectionParts划分到不同区间段
	sectionSegList.Empty();
	for(SECTION_PART *pTempSectionPart=sectionPartList.GetFirst();pTempSectionPart;pTempSectionPart=sectionPartList.GetNext())
	{
		long hKey=pTempSectionPart->pPart->handle;
		SECTION_SEG *pSectionSeg=sectionSegList.GetTail();
		if(pSectionSeg==NULL||!pSectionSeg->IsIncludeSectionPart(pTempSectionPart))
		{
			pSectionSeg=sectionSegList.append();
			pSectionSeg->m_cSectionType=SECTION_SEG::SECTION_TYPE_NODEPLATE;
			SECTION_PART *pNewSectionPart=pSectionSeg->sectionPartList.SetValue(hKey,*pTempSectionPart);
			pSectionSeg->m_pStartPart=pSectionSeg->m_pEndPart=pNewSectionPart;
		}
		else 
		{
			SECTION_PART *pNewSectionPart=pSectionSeg->sectionPartList.SetValue(hKey,*pTempSectionPart);
			if(pTempSectionPart->prjLine.endPt.z>pSectionSeg->m_pEndPart->prjLine.endPt.z)
				pSectionSeg->m_pEndPart=pNewSectionPart;
			if(pTempSectionPart->prjLine.startPt.z<pSectionSeg->m_pStartPart->prjLine.startPt.z)
				pSectionSeg->m_pStartPart=pNewSectionPart;
		}
		
		if(pTempSectionPart->pPart&&pTempSectionPart->pPart->IsPlate())
		{
			if(pTempSectionPart->pPart->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pTempSectionPart->pPart)->m_fInnerRadius>0)
				pSectionSeg->m_cSectionType|=SECTION_SEG::SECTION_TYPE_CIRPLATE;
			else if(pTempSectionPart->pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pTempSectionPart->pPart;
				if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
					pSectionSeg->m_cSectionType|=SECTION_SEG::SECTION_TYPE_CIRCOVERPLATE;
				else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
					pSectionSeg->m_cSectionType|=SECTION_SEG::SECTION_TYPE_CIRRIBPLATE;
				else if(pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
					pSectionSeg->m_cSectionType|=SECTION_SEG::SECTION_TYPE_TUBERIBPLATE;
			}
		}
	}
}
void InitSlaveSectionViewParaCS(CParaCS::PARAMETRIC_INFO& param,CLDSLineTube *pLineTube,CSectionView *pMainView)
{
	//原点参数
	param.desOrg.datum_pos_style=1;
	param.desOrg.des_para.RODEND.hRod=pLineTube->handle;
	param.desOrg.des_para.RODEND.bIncOddEffect=TRUE;
	param.desOrg.des_para.RODEND.direction=0;
	//坐标轴
	param.m_eDatumPlane=CParaCS::DP_ZX;
	param.desAxisZ.norm_style=5;
	param.desAxisZ.hVicePart=pLineTube->handle;
	param.desAxisZ.direction=1;
	GEPOINT lenVec=pLineTube->Start()-pLineTube->End();
	if(lenVec*pMainView->ucs.axis_x>0)
		param.desAxisZ.direction=0;
	if(pMainView&&pMainView->des_cs.IsDesAxisZ())
		param.desAxisX=pMainView->des_cs.GetParamInfo().desAxisZ;
	else
	{
		param.desAxisX.norm_style=4;
		param.desAxisX.hVicePart=pLineTube->handle;
		param.desAxisX.nearVector.Set(1,0,0);
	}
}
//</DEVELOP_PROCESS_MARK>

#if defined( __LDS_CONTEXT_)||defined(__DRAWING_CONTEXT_)
//<DEVELOP_PROCESS_MARK nameId="CLDSPartGroup::AddLineTubeMiddleNodeEndView">
static int compare_func3(const CIRPLATE_ITEM &unit1,const CIRPLATE_ITEM &unit2)
{
	if(unit1.m_fPrjLen>unit2.m_fPrjLen)
		return 1;
	else
		return -1;
}
void InitPartRegion(CIRPLATE_ITEM* pPartItem,UCS_STRU& tem_ucs)
{
	if(!pPartItem->m_pPart->IsPlate())
		return;
	CLDSGeneralPlate* pGeneralPlate=(CLDSGeneralPlate*)pPartItem->m_pPart;
	int n=pGeneralPlate->vertex_list.GetNodeNum();
	if(n<3)
		return;
	ARRAY_LIST<PROFILE_VERTEX*> verticePtrs(0,n);
	PROFILE_VERTEX* pVertex=NULL;
	for(pVertex=pGeneralPlate->vertex_list.GetFirst();pVertex;pVertex=pGeneralPlate->vertex_list.GetNext())
		verticePtrs.append(pVertex);
	ARRAY_LIST<f3dPoint> profileVertexList;
	f3dArcLine arcline;
	for(int i=0;i<n;i++)
	{
		PROFILE_VERTEX *pStartVertex=verticePtrs[i];
		PROFILE_VERTEX *pEndVertex=verticePtrs[(i+1)%n];
		f3dPoint f_start=pGeneralPlate->GetRealVertex(pStartVertex->vertex);
		f3dPoint f_end  =pGeneralPlate->GetRealVertex(pEndVertex->vertex);
		coord_trans(f_start,pGeneralPlate->ucs,TRUE);
		coord_trans(f_end,pGeneralPlate->ucs,TRUE);
		coord_trans(f_start,tem_ucs,FALSE);
		coord_trans(f_end,tem_ucs,FALSE);
		f_start.z=f_end.z=0;
		if(pStartVertex->type==0)
		{
			arcline=f3dArcLine(f3dLine(f_start,f_end));
			profileVertexList.append(arcline.Start());
		}
		else if(pStartVertex->type<3)	//圆弧
		{
			PROFILE_VERTEX arcStart=*pStartVertex;
			arcStart.vertex=f_start;
			arcStart.center=pStartVertex->center;
			coord_trans(arcStart.center,pGeneralPlate->ucs,TRUE);
			coord_trans(arcStart.center,tem_ucs,FALSE);
			arcStart.center.z=0;
			if(tem_ucs.axis_z*pGeneralPlate->ucs.axis_z<0)
			{
				if(arcStart.type==1)
					arcStart.sector_angle*=-1.0;
				else
					arcStart.radius*=-1.0;
			}
			arcStart.RetrieveArcLine(arcline,f_end,NULL);
			profileVertexList.append(arcline.Start());
			//将圆弧简化为多条线段
			int nSlices=4;//CalArcResolution(arcline.Radius(),arcline.SectorAngle(),1.0,5.0,6);
			double angle=arcline.SectorAngle()/nSlices;
			for(int i=1;i<nSlices;i++)
			{
				if(arcline.SectorAngle()<EPS)
				{
					if(pPartItem->m_pPart)
						pPartItem->m_pPart->Log2File()->Log("0x%X{%s}钢板的外形存在错误！",pPartItem->m_pPart->handle,pPartItem->m_pPart->Label);
					break;	//个别情况数据出错，会出现圆弧扇形角为零情况 wjh-2018.1.3
				}
				f3dPoint pos=arcline.PositionInAngle(angle*i);
				profileVertexList.append(pos);
			}
		}
		else if(pStartVertex->type==3)	//椭圆弧
		{
			PROFILE_VERTEX arcStart=*pStartVertex;
			arcStart.vertex=f_start;
			if(tem_ucs.axis_z*pGeneralPlate->ucs.axis_z<0)
				arcStart.radius*=-1.0;
			if(fabs(pGeneralPlate->ucs.axis_z*tem_ucs.axis_z)>EPS_COS)
				arcStart.RetrieveArcLine(arcline,f_end,&tem_ucs);
			else
				arcline=f3dArcLine(f3dLine(f_start,f_end));
			profileVertexList.append(arcline.Start());
		}
	}
	if(profileVertexList.GetSize()>0)
		pPartItem->m_xRegion.CreatePolygonRgn(profileVertexList.m_pData,profileVertexList.GetSize());
}
CSectionView *AddNewViewByLayerGroup(CLDSPartGroup *pPartGroup,CSectionView *pMainView,CLayerGroup *pLayerGroup,int &view_index,CLDSLineTube* pDatumZTube=NULL)
{
#if !defined( __LDS_CONTEXT_)&&!defined(__DRAWING_CONTEXT_)
	return NULL;
#else
	if(pPartGroup==NULL||pLayerGroup==NULL)
		return NULL;
	CLDSLineTube* pLineTube=pDatumZTube!=NULL?pDatumZTube:(CLDSLineTube*)pPartGroup->BelongModel()->FromPartHandle(pPartGroup->handle,CLS_LINETUBE);
	if(pLineTube==NULL)
		return NULL;
	CParaCS::PARAMETRIC_INFO paramCS;
	InitSlaveSectionViewParaCS(paramCS,pLineTube,pMainView);
	//
	CSlaveNodeSectionView *pSectionView=(CSlaveNodeSectionView*)pPartGroup->SonViews.Add(0,CSectionView::CLS_SLAVE_NODESECT_VIEW);
	pSectionView->index=view_index++;
	pSectionView->m_iViewType=VIEW_NODEPLATE;
	pSectionView->m_bLocked=TRUE;	//新添加的端面图必须锁定
	pSectionView->des_cs.SetParamInfo(paramCS);
	pSectionView->UpdateUCS(pPartGroup->BelongModel(),pPartGroup);
	pSectionView->SonParts.Empty();
	pSectionView->AddPart(pLineTube->handle,pLineTube);
	pSectionView->m_hVertRod=pLineTube->handle;
	CXhChar50 sName;
	pLayerGroup->GetGroupName(sName);
	sprintf(pSectionView->sName,"%s环板端面视图",(char*)sName);
	//
	bool bSetOrg=false;
	for(CLayerUnit** ppUnit=pLayerGroup->m_xLayerUnitPtrSet.GetFirst();ppUnit;ppUnit=pLayerGroup->m_xLayerUnitPtrSet.GetNext())
	{
		for(CIRPLATE_ITEM* pPartItem=(*ppUnit)->m_xPartSet.GetFirst();pPartItem;pPartItem=(*ppUnit)->m_xPartSet.GetNext())
		{
			if(pPartItem->m_pPart->GetClassTypeId()==CLS_PLATE&&!bSetOrg)
			{
				CLDSPlate* pPlate=(CLDSPlate*)pPartItem->m_pPart;
				CLDSNode* pBaseNode=pPlate->GetBaseNode();
				CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
				if(pBaseNode&&param.desOrg.datum_pos_style==1)
				{
					bSetOrg=true;
					long hRod=param.desOrg.des_para.RODEND.hRod;
					param.desOrg.datum_pos_style=2;
					param.desOrg.des_para.RODNODE.hRod=hRod;
					param.desOrg.des_para.RODNODE.hNode=pBaseNode->handle;
					pSectionView->des_cs.SetParamInfo(param);
					pSectionView->UpdateUCS(pPlate->BelongModel(),pPartGroup);
					pSectionView->m_hBaseNode=pBaseNode->handle;
				}
			}
			else if(pPartItem->m_pPart->GetClassTypeId()==CLS_PARAMPLATE&&!bSetOrg)
			{
				CLDSParamPlate* pParaPlate=(CLDSParamPlate*)pPartItem->m_pPart;
				bSetOrg=true;
				CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
				param.desOrg.datum_pos_style=9;
				param.desOrg.des_para.hPart=pParaPlate->handle;
				param.desOrg.datum_pos.Set();
				pSectionView->des_cs.SetParamInfo(param);
				pSectionView->des_cs.UpdateACS(pParaPlate->BelongModel());
			}
			pSectionView->AddPart(pPartItem->m_pPart->handle,pPartItem->m_pPart);
		}
	}
	//根据宿主视图初始化子视图参数
	pSectionView->InitSlaveSectionView(pMainView,pPartGroup->BelongModel());
	return pSectionView;
#endif
}
#endif

static BOOL PlateIsAlongTubeAxisVec(CLDSLineTube *pLineTube,CLDSGeneralPlate *pPlate)
{
	BOOL bAlongTubeRibPlate=FALSE;
	UCS_STRU tem_ucs=pLineTube->ucs;
	if( fabs(pPlate->ucs.axis_x*tem_ucs.axis_z)>EPS_COS||
		fabs(pPlate->ucs.axis_y*tem_ucs.axis_z)>EPS_COS)
		bAlongTubeRibPlate=TRUE;
	else 
	{
		PROFILE_VERTEX *pPreVertex=pPlate->vertex_list.GetTail(),*pVertex=NULL;
		for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			if(pPreVertex->m_bWeldEdge)
			{
				GEPOINT weld_vec=pPreVertex->vertex-pVertex->vertex;
				vector_trans(weld_vec,pPlate->ucs,TRUE);
				if(fabs(weld_vec*tem_ucs.axis_z)>EPS_COS)
				{
					bAlongTubeRibPlate=TRUE;
					break;
				}
			}
			pPreVertex=pVertex;
		}
	}
	return bAlongTubeRibPlate;
}
//基准构件为钢管，为每个节点添加端面视图 保证端面图与主视图匹配
//TODO: 基准杆件上剖切面节点不能直接根据GetBaseNode获取(尤其是钢管夹板时不对)，应考虑：
//将沿向钢板向基准杆件上投影，投影线重叠或距离在一定范围内时视同一个节点 wjh-2013.12.08
BOOL CLDSPartGroup::AddLineTubeMiddleNodeEndView(CLDSLineTube *pLineTube,CSectionView *pMainView,PARTSET &partset,int &view_index)
{
#if !defined( __LDS_CONTEXT_)&&!defined(__DRAWING_CONTEXT_)
	return NULL;
#else
	ATOM_LIST<SECTION_SEG> sectionSegList;
	//1.提取区间段
	ExtractSectionSegs(pLineTube,partset,sectionSegList);
	if(sectionSegList.GetNodeNum()<=0)
		return FALSE;
	//2.为每个区间段生成一组视图
	SECTION_SEG *pSectionSeg;
	//过滤掉重复的节间
	for(pSectionSeg=sectionSegList.GetFirst();pSectionSeg;pSectionSeg=sectionSegList.GetNext())
	{
		BOOL pushed=sectionSegList.push_stack();
		for(SECTION_SEG *pNextSectionSeg=sectionSegList.GetNext();pNextSectionSeg;pNextSectionSeg=sectionSegList.GetNext())
		{
			if(pSectionSeg->m_pDatumPart!=pNextSectionSeg->m_pDatumPart)
				continue;
			if(pSectionSeg->sectionPartList.GetNodeNum()!=pNextSectionSeg->sectionPartList.GetNodeNum())
				continue;
			SECTION_PART *pSectionPart;
			CHashStrList<SECTION_PART*> hashSectParts;
			CLDSPart* pNoneLabelPart=NULL;
			for(pSectionPart=pSectionSeg->sectionPartList.GetFirst();pSectionPart;pSectionPart=pSectionSeg->sectionPartList.GetNext())
			{
				if(strlen(pSectionPart->pPart->GetPartNo())>0)
					hashSectParts.SetValue(pSectionPart->pPart->GetPartNo(),pSectionPart);
				else
					pNoneLabelPart=pSectionPart->pPart;
			}
			bool matched=true;
			for(pSectionPart=pNextSectionSeg->sectionPartList.GetFirst();pSectionPart;pSectionPart=pNextSectionSeg->sectionPartList.GetNext())
			{
				if(strlen(pSectionPart->pPart->GetPartNo())==0&&pSectionPart->pPart==pNoneLabelPart)
					continue;	//个别情况下（变电构架梁)主管可能不编编号
				if(hashSectParts.GetValue(pSectionPart->pPart->GetPartNo())==NULL)	//未找到相应编号的构件
				{
					matched=false;
					break;
				}
			}
			if(matched)
				sectionSegList.DeleteCursor();
		}
		sectionSegList.pop_stack(pushed);
	}
	//
	int index=1;
	pLineTube->BuildUCS();
	UCS_STRU tem_ucs=pLineTube->ucs;
	f3dLine prj_line;
	prj_line.startPt=pLineTube->Start()-tem_ucs.axis_z*pLineTube->startOdd();
	prj_line.endPt=pLineTube->End()+tem_ucs.axis_z*pLineTube->endOdd();
	for(pSectionSeg=sectionSegList.GetFirst();pSectionSeg;pSectionSeg=sectionSegList.GetNext())
	{
		PARTSET assitPartSet;
		PARTSET alongTubeRibPlateSet;
		CHashList<long> hashHandleByHandle;
		ARRAY_LIST<CIRPLATE_ITEM> xCirPartArr;
		//提取钢管上的环向板,并按照环向板在钢管上的投影位置进行排序
		for(SECTION_PART *pSectionPart=pSectionSeg->sectionPartList.GetFirst();pSectionPart;pSectionPart=pSectionSeg->sectionPartList.GetNext())
		{	
			if(pSectionPart->pPart==NULL)
				continue;
			BOOL bCirPlate=FALSE;
			BOOL bAlongTubeRibPlate=FALSE;
			if(pSectionPart->pPart->GetClassTypeId()==CLS_PLATE)
			{
				CLDSPlate* pPlate=(CLDSPlate*)pSectionPart->pPart;
				f3dPoint vec=prj_line.endPt-prj_line.startPt;
				normalize(vec);
				if(pPlate->m_fInnerRadius>0||fabs(pPlate->ucs.axis_z*vec)>EPS_COS2)
					bCirPlate=TRUE;
				if( !pPlate->IsConnectPlate()&&pPlate->m_hPartWeldParent==pLineTube->handle)
					bAlongTubeRibPlate=PlateIsAlongTubeAxisVec(pLineTube,pPlate);
			}
			else if(pSectionPart->pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pSectionPart->pPart;
				if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE||
					pParamPlate->m_iParamType==TYPE_CIRRIBPLATE||
					pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
				{
					long B=0,C=0,E=0,F=0,G=0;
					if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
					{
						if(pParamPlate->GetDesignItemValue('F',&F)&&F>0)
							hashHandleByHandle.SetValue(F,F);
						if(pParamPlate->GetDesignItemValue('G',&G)&&G>0)
							hashHandleByHandle.SetValue(G,G);
					}
					else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE||pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
					{
						if(pParamPlate->GetDesignItemValue('E',&E)&&E>0)
							hashHandleByHandle.SetValue(E,E);
						if(pParamPlate->GetDesignItemValue('F',&F)&&F>0)
							hashHandleByHandle.SetValue(F,F);
						if(pParamPlate->GetDesignItemValue('C',&C)&&C>0)
							hashHandleByHandle.SetValue(C,C);
						if(pParamPlate->GetDesignItemValue('B',&B)&&B>0)
							hashHandleByHandle.SetValue(B,B);	
					}
					bCirPlate=TRUE;
				}
				if( pParamPlate->m_iParamType==TYPE_RIBPLATE&&pParamPlate->m_hPartWeldParent==pLineTube->handle)
					bAlongTubeRibPlate=PlateIsAlongTubeAxisVec(pLineTube,pParamPlate);
			}
			else if(pSectionPart->pPart->IsTube())
			{
				CLDSLineTube *pSonTube=(CLDSLineTube*)pSectionPart->pPart;
				if(pSonTube->FuncType()==CLDSLineTube::FUNC_WIREBUSH||(pSonTube->pStart==NULL&&pSonTube->pEnd==NULL))
					bAlongTubeRibPlate=TRUE;	//套管一般出现在衬托板或者挂线板上,剖视图中不需要添加套管 wht 18-03-14
			}
			if(bCirPlate)
			{
				f3dPoint perp;
				SnapPerp(&perp,prj_line,pSectionPart->pPart->ucs.origin);
				CIRPLATE_ITEM* pCirPlateItem=xCirPartArr.append();
				pCirPlateItem->m_pPart=pSectionPart->pPart;
				pCirPlateItem->m_xPrjPt=perp;
				pCirPlateItem->m_fPrjLen=DISTANCE(perp,prj_line.startPt);
			}
			else if(bAlongTubeRibPlate)
				alongTubeRibPlateSet.append(pSectionPart->pPart);
			else
				assitPartSet.append(pSectionPart->pPart);
		}
		//排除钢管沿向钢板中与环向板关联的钢板，并添加到assitPartSet中
		for(CLDSPart *pPart=alongTubeRibPlateSet.GetFirst();pPart;pPart=alongTubeRibPlateSet.GetNext())
		{
			if(hashHandleByHandle.GetValue(pPart->handle))
			{
				alongTubeRibPlateSet.DeleteCursor();
				assitPartSet.append(pPart);
			}
		}
		alongTubeRibPlateSet.Clean();
		if(xCirPartArr.GetSize()>0)
		{
			CQuickSort<CIRPLATE_ITEM>::QuickSort(xCirPartArr.m_pData,xCirPartArr.GetSize(),compare_func3);
			//将环板按照投影位置进行分层处理
			ATOM_LIST<CLayerUnit> layerUnitList;
			for(CIRPLATE_ITEM* pPartItem=xCirPartArr.GetFirst();pPartItem;pPartItem=xCirPartArr.GetNext())
			{
				CLayerUnit* pLayerUnit=NULL;
				for(pLayerUnit=layerUnitList.GetFirst();pLayerUnit;pLayerUnit=layerUnitList.GetNext())
				{
					if(pLayerUnit->IsInSameLayer(pPartItem->m_xPrjPt))
						break;
				}
				if(pLayerUnit==NULL)
				{
					pLayerUnit=layerUnitList.append();
					pLayerUnit->SetProjectionPt(pPartItem->m_xPrjPt);
				}
				CIRPLATE_ITEM* pItem=pLayerUnit->m_xPartSet.append();
				pItem->m_pPart=pPartItem->m_pPart;
				InitPartRegion(pItem,tem_ucs);
			}
			//根据环向构件之间的重叠度进行分组
			ATOM_LIST<CLayerGroup> layerGroupList;
			for(CLayerUnit* pLayerUnit=layerUnitList.GetFirst();pLayerUnit;pLayerUnit=layerUnitList.GetNext())
			{
				CLayerGroup* pGroup=NULL;
				for(pGroup=layerGroupList.GetFirst();pGroup;pGroup=layerGroupList.GetNext())
				{
					if(pGroup->m_bFinish)
						continue;
					if(pGroup->IsOverlapped(pLayerUnit))
					{	//有重叠
						if(pLayerUnit->m_bNeed)	//不完全重叠，需新建视图
							pGroup->m_bFinish=TRUE;
						else					//完全重叠的两个单元，不需重建视图，在同一视图显示	
							pGroup->m_xLayerUnitPtrSet.append(pLayerUnit);
					}
					else	//无重叠
					{	
						pGroup->m_xLayerUnitPtrSet.append(pLayerUnit);
						break;
					}
				}
				if(pGroup==NULL&&pLayerUnit->m_bNeed)
				{
					pGroup=layerGroupList.append();
					pGroup->m_bFinish=FALSE;
					pGroup->m_xLayerUnitPtrSet.append(pLayerUnit);
				}
			}
			//合并两端都有同一号法兰的分组
			CLayerGroup* pGroup=NULL,*pNextGroup=NULL;
			BOOL bDelete=FALSE;
			for(pGroup=layerGroupList.GetFirst();pGroup;pGroup=layerGroupList.GetNext())
			{
				if(!pGroup->IsCirCoverPlateGroup())
					continue;
				for(pNextGroup=layerGroupList.GetNext();pNextGroup;pNextGroup=layerGroupList.GetNext())
				{
					if(pNextGroup->IsCirCoverPlateGroup()&&pGroup->IsOverlapped(*pNextGroup->m_xLayerUnitPtrSet.GetFirst()))
					{
						pGroup->m_xLayerUnitPtrSet.append(*pNextGroup->m_xLayerUnitPtrSet.GetFirst());
						layerGroupList.DeleteCursor();
						bDelete=TRUE;
						break;
					}
				}
				if(bDelete)
					break;
			}
			if(bDelete)
				layerGroupList.Clean();
			//根据不同的分组添加相应的视图
			for(pGroup=layerGroupList.GetFirst();pGroup;pGroup=layerGroupList.GetNext(),index++)
			{	//根据不同的分组添加相应的视图
				CSlaveNodeSectionView* pSectNodeView=(CSlaveNodeSectionView*)AddNewViewByLayerGroup(this,pMainView,pGroup,view_index,pLineTube);
				if(pSectNodeView)
				{
					pSectNodeView->m_hVertRod=pLineTube->handle;
					for(CLDSPart* pPart=assitPartSet.GetFirst();pPart;pPart=assitPartSet.GetNext())
						pSectNodeView->AddPart(pPart->handle,pPart);
				}
			}
		}
		else
		{	//无环向板的情况
			CParaCS::PARAMETRIC_INFO paramCS;
			InitSlaveSectionViewParaCS(paramCS,pLineTube,pMainView);
			CSectionView sectionView;
			sectionView.des_cs.SetParamInfo(paramCS);
			sectionView.UpdateUCS(m_pModel,this);
			sectionView.AddPart(pLineTube->handle,pLineTube);
			for(SECTION_PART *pSectionPart=pSectionSeg->sectionPartList.GetFirst();pSectionPart;pSectionPart=pSectionSeg->sectionPartList.GetNext())
			{
				if(pSectionPart->pPart)
					sectionView.AddPart(pSectionPart->pPart->handle,pSectionPart->pPart);
			}
			CSlaveNodeSectionView* pSectNodeView=(CSlaveNodeSectionView*)AddNewViewByOldView(this,pMainView,&sectionView,VIEW_NODEPLATE,view_index);
			if(pSectNodeView)
				pSectNodeView->m_hVertRod=pLineTube->handle;
		}
	}
	return TRUE;
	for(pSectionSeg=sectionSegList.GetFirst();pSectionSeg;pSectionSeg=sectionSegList.GetNext())
	{
		CParaCS::PARAMETRIC_INFO paramCS;
		InitSlaveSectionViewParaCS(paramCS,pLineTube,pMainView);
		CSectionView sectionView;
		sectionView.des_cs.SetParamInfo(paramCS);
		sectionView.UpdateUCS(m_pModel,this);
		//填充视图构件集合
		sectionView.AddPart(pLineTube->handle,pLineTube);
		for(SECTION_PART *pSectionPart=pSectionSeg->sectionPartList.GetFirst();pSectionPart;pSectionPart=pSectionSeg->sectionPartList.GetNext())
		{
			if(pSectionPart->pPart)
				sectionView.AddPart(pSectionPart->pPart->handle,pSectionPart->pPart);
		}
		//根据分段节间类型添加视图
		CSlaveNodeSectionView* pSectNodeView=NULL;
		CSectionView::PART_ITEM* pPartItem=NULL;
		if(pSectionSeg->m_cSectionType&(SECTION_SEG::SECTION_TYPE_CIRRIBPLATE|SECTION_SEG::SECTION_TYPE_TUBERIBPLATE))
		{	//添加跨越连板(已与环向肋板合并为同一类型)视图 wjh-2016.10.16
			pSectNodeView=(CSlaveNodeSectionView*)AddNewViewByOldView(this,pMainView,&sectionView,VIEW_TUBERIBPLATE,view_index);
			if(pSectNodeView)
			{
				pSectNodeView->m_hVertRod=pLineTube->handle;
				for(pPartItem=pSectNodeView->SonParts.GetFirst();pPartItem;pPartItem=pSectNodeView->SonParts.GetNext())	
				{
					if(TagPart(pPartItem)==NULL)
						continue;
					SECTION_PART* pSectionPart=pSectionSeg->sectionPartList.GetValue(TagPart(pPartItem)->handle);
					if(pSectionPart)
						pSectionPart->m_bAddToView=TRUE;
				}
			}
		}
		if(pSectionSeg->m_cSectionType&SECTION_SEG::SECTION_TYPE_CIRCOVERPLATE)	//添加环向封板视图
		{
			pSectNodeView=(CSlaveNodeSectionView*)AddNewViewByOldView(this,pMainView,&sectionView,VIEW_CIRCOVERPLATE,view_index);
			if(pSectNodeView)
			{
				pSectNodeView->m_hVertRod=pLineTube->handle;
				for(pPartItem=pSectNodeView->SonParts.GetFirst();pPartItem;pPartItem=pSectNodeView->SonParts.GetNext())	
				{
					if(TagPart(pPartItem)==NULL)
						continue;
					SECTION_PART* pSectionPart=pSectionSeg->sectionPartList.GetValue(TagPart(pPartItem)->handle);
					if(pSectionPart)
						pSectionPart->m_bAddToView=TRUE;
				}
			}
		}
		if(pSectionSeg->m_cSectionType&SECTION_SEG::SECTION_TYPE_CIRPLATE)		//添加环向板端面图
		{
			pSectNodeView=(CSlaveNodeSectionView*)AddNewViewByOldView(this,pMainView,&sectionView,VIEW_CIRPLATE,view_index);
			if(pSectNodeView)
			{
				pSectNodeView->m_hVertRod=pLineTube->handle;
				for(pPartItem=pSectNodeView->SonParts.GetFirst();pPartItem;pPartItem=pSectNodeView->SonParts.GetNext())	
				{
					if(TagPart(pPartItem)==NULL)
						continue;
					SECTION_PART* pSectionPart=pSectionSeg->sectionPartList.GetValue(TagPart(pPartItem)->handle);
					if(pSectionPart)
						pSectionPart->m_bAddToView=TRUE;
				}
			}
		}
		//
		if(pSectionSeg->m_cSectionType==SECTION_SEG::SECTION_TYPE_NODEPLATE||pSectionSeg->StatSectionPartCount()>0)
		{
			sectionView.SonParts.Empty();
			sectionView.AddPart(pLineTube->handle,pLineTube);
			for(SECTION_PART *pSectionPart=pSectionSeg->sectionPartList.GetFirst();pSectionPart;pSectionPart=pSectionSeg->sectionPartList.GetNext())
			{
				if(pSectionPart->pPart&&!pSectionPart->m_bAddToView)
				{
					sectionView.AddPart(pSectionPart->pPart->handle,pSectionPart->pPart);
					CLDSPart* pWeldPart=BelongModel()->FromPartHandle(pSectionPart->pPart->m_hPartWeldParent);
					if(pWeldPart)
						sectionView.AddPart(pWeldPart->handle,pWeldPart);
				}
			}
			//
			pSectNodeView=(CSlaveNodeSectionView*)AddNewViewByOldView(this,pMainView,&sectionView,VIEW_NODEPLATE,view_index);
			if(pSectNodeView)
				pSectNodeView->m_hVertRod=pLineTube->handle;
		}
	}
	return TRUE;
#endif
}
//</DEVELOP_PROCESS_MARK>

//<DEVELOP_PROCESS_MARK nameId="CLDSPartGroup::AddLineTubeEndLjView">
//3.3.3 添加钢管端板视图(包括平面切割、法兰、插板、塔脚板)
BOOL CLDSPartGroup::AddLineTubeEndLjView(CLDSLineTube *pLineTube,CSectionView *pMainView,PARTSET &partset,int iStartView,int &view_index)
{
	int i=0;
	f3dPoint tube_vec=pLineTube->End()-pLineTube->Start();
	normalize(tube_vec);
	/* 目前终端切割面视图已融入到普通主视图中（通过旋转切割面实现) wjh-2016.10.15
	//1 为平面切割的主材钢管添加切割投影面视图
	f3dPoint cut_norm;
	for(i=0;i<2;i++)
	{
		TUBEJOINT *pTubeJoint=NULL;
		if(i==0)
			pTubeJoint=&pLineTube->m_tJointStart;
		else 
			pTubeJoint=&pLineTube->m_tJointEnd;
		if(pTubeJoint->cutPlaneNorm.IsZero()||fabs(pTubeJoint->cutPlaneNorm*tube_vec)>EPS_COS)
			continue;	//切割面法线方向为0或与钢管延伸方向平行
		if(i==1&&fabs(pTubeJoint->cutPlaneNorm*cut_norm)>EPS_COS)
			continue;	//避免始终端切割面法线相同同时重复添加视图的情况
		cut_norm=pTubeJoint->cutPlaneNorm;
		//初始化视图坐标系
		CParaCS::PARAMETRIC_INFO param;
		param.desOrg.datum_pos_style=1;	//基准点
		param.desOrg.des_para.RODEND.hRod=pLineTube->handle;
		param.desOrg.des_para.RODEND.bIncOddEffect=TRUE;
		param.desOrg.des_para.RODEND.direction=0;
		param.m_eDatumPlane=CParaCS::DP_XY;	//坐标轴
		param.desAxisX.norm_style=5;	//基准杆件上的特征方向
		param.desAxisX.hVicePart=pLineTube->handle;
		param.desAxisX.direction=0;
		param.desAxisY.norm_style=5;
		param.desAxisY.hVicePart=pLineTube->handle;
		if(i==0)
			param.desAxisY.direction=4;
		else
			param.desAxisY.direction=5;
		CParaCS des_cs;
		des_cs.SetParamInfo(param);
		des_cs.SetModifiedState();
		des_cs.UpdateACS(m_pModel);
		f3dPoint norm=des_cs.GetACS().axis_z;
		//判断此视图与已有视图有重复情况
		BOOL bHasSameView=FALSE;
		CSectionView* pSectionView=NULL;
		for(pSectionView=SonViews.GetFirst();pSectionView;pSectionView=SonViews.GetNext())
		{
			if(fabs(norm*pSectionView->ucs.axis_z)<EPS_COS)		//两法线共线
				continue;
			bHasSameView=TRUE;
			break;
		}
		if(bHasSameView)
			continue;
		//添加视图
		pSectionView = SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
		pSectionView->index=view_index++;	//视图索引
		pSectionView->m_iViewType=VIEW_FRONT;
		pSectionView->des_cs.SetParamInfo(param);
		if(i==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(pSectionView->sName,"Start cut plane project view");	//"始端对角面视图");
		else
			strcpy(pSectionView->sName,"End cut plane project view");	//"终端对角面视图");
#else
			strcpy(pSectionView->sName,"始端切割面投影视图");	//"始端对角面视图");
		else
			strcpy(pSectionView->sName,"终端切割面投影视图");	//"终端对角面视图");
#endif
		pSectionView->UpdateUCS(m_pModel,this);	//必须置于pSectionView->sName赋值之后，否则出错时警示信息无法正常显示视图名称 wjh-2016.8.7
		pSectionView->AddPart(pLineTube->handle,pLineTube);
		for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			pSectionView->AddPart(pPart->handle,pPart);
	}
	*/
	//2 添加钢管端头视图  法兰视图，插板视图
	char sPartNo[50]="";
	for(i=0;i<2;i++)
	{
		CLDSParamPlate *pEndLj=NULL;
		if(i==0&&pLineTube->m_tJointStart.hLinkObj>0x20)
			pEndLj=(CLDSParamPlate*)BelongModel()->FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		else if(i==1&&pLineTube->m_tJointEnd.hLinkObj>0x20)
			pEndLj=(CLDSParamPlate*)BelongModel()->FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		if(pEndLj==NULL)
			continue;
		if(i==1&&pEndLj->IsInsertPlate()&&strlen(sPartNo)>0&&strcmp(sPartNo,pEndLj->GetPartNo())==0)
			continue;	//始终端插板编号相同
		if(!pEndLj->IsFL()&&!pEndLj->IsInsertPlate())
			continue;	//只针对法兰及插板生成钢管端头视图 wht 12-09-06
		CSlaveSectionView *pSectionView = (CSlaveSectionView*)SonViews.Add(0,CSectionView::CLS_SLAVE_VIEW);
		pSectionView->m_bLocked=TRUE;
		if(i==0&&iStartView>0)
			pSectionView->index=iStartView;
		else
		{
			pSectionView->index=view_index;
			view_index++;
		}
		//端视图时插板的法线方向可能与钢管的延伸方向不垂直,重新设计板和板上螺栓 wh 16-10-20
		if(pEndLj->m_iParamType==TYPE_ROLLEND||pEndLj->m_iParamType==TYPE_UEND||pEndLj->m_iParamType==TYPE_XEND)
		{
			f3dPoint tube_line_vec=pLineTube->End()-pLineTube->Start();
			normalize(tube_line_vec);
			if(fabs(pEndLj->ucs.axis_z*tube_line_vec)>1e-6)
			{
				logerr.Log(CXhChar100("%s组焊件'端视图'中，0X%x(%s)插板法线方向与钢管的延伸方向不严格垂直",(char*)pLineTube->GetPartNo(),pEndLj->handle,(char*)pEndLj->GetPartNo()));
				//重新设计插板
				pEndLj->SetModified();
				pEndLj->DesignPlate();
				for(CLsRef *pLsRef=pEndLj->GetFirstLsRef();pLsRef;pLsRef=pEndLj->GetNextLsRef())
				{
					CLDSBolt *pBolt=pLsRef->GetLsPtr();
					pBolt->correct_worknorm();
					pBolt->correct_pos();
					pBolt->CalGuigeAuto();
				}
				for(ATTACHPART_ITEM *pItem=pEndLj->GetFirstAttachPart();pItem;pItem=pEndLj->GetNextAttachPart())
				{
					CLDSPart *pPlate=pEndLj->BelongModel()->FromPartHandle(pItem->h,CLS_PLATE,CLS_PARAMPLATE);
					if(!pPlate)
						continue;
					if(pEndLj->m_iParamType==TYPE_XEND&&(pPlate->m_hPartWeldParent!=pEndLj->handle&&pPlate->m_hPartWeldParent!=pLineTube->handle))
						continue;
					if(pPlate->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pPlate)->jdb_style==5)
						((CLDSPlate*)pPlate)->DesignSetupUcs();
				}
			}			
		}
		pSectionView->AddPart(pLineTube->handle,pLineTube);
		pSectionView->AddPart(pEndLj->handle,pEndLj);
		if(pEndLj->m_iParamType==TYPE_XEND||pEndLj->m_iParamType==TYPE_ROLLEND)
		{	//将十字插板附加构件添加到十字插板端面图中,槽型插板上也支持附加环件 wht 10-06-29
			ATTACHPART_ITEM *pItem=NULL;
			for(pItem=pEndLj->GetFirstAttachPart();pItem;pItem=pEndLj->GetNextAttachPart())
			{
				CLDSPlate *pAttachPlate=(CLDSPlate*)m_pModel->FromPartHandle(pItem->h,CLS_PLATE);
				if(pAttachPlate==NULL)
					continue;
				if(pAttachPlate->m_hPartWeldParent!=pEndLj->handle)
					continue;
				//仅添加焊接父构件为十字插板基板的附加板 wht 10-08-06
				pSectionView->AddPart(pItem->h);
			}
		}
		CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
		//基准点
		param.desOrg.datum_pos_style=9;
		param.desOrg.des_para.hPart=pEndLj->handle;

		param.m_eDatumPlane=CParaCS::DP_ZX;
		//X轴 调整端面图坐标系与主正视图保持一致
		if(pMainView&&pMainView->des_cs.IsDesAxisZ())
			param.desAxisX=pMainView->des_cs.GetParamInfo().desAxisZ;
		else
		{
			param.desAxisX.norm_style=4;
			param.desAxisX.hVicePart=pLineTube->handle;
			param.desAxisX.nearVector.Set(1,0,0);
		}
		if(pEndLj->IsFL())
		{
			//Z轴
			param.desAxisZ.norm_style=3;
			param.desAxisZ.hVicePart=pEndLj->handle;
			param.desAxisZ.direction=1;//调整视图坐标系方向为杆件指向法兰的负方向
			//宏源称法兰一律从左向右看视图	wjh-2017.12.1
			if(pEndLj->ucs.axis_z*pMainView->ucs.axis_z>0)
				param.desAxisZ.direction=0;
			if(i==0)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(pSectionView->sName,"Start Flange View");
			else 
				strcpy(pSectionView->sName,"End Flange View");
#else
				strcpy(pSectionView->sName,"始端法兰视图");
			else 
				strcpy(pSectionView->sName,"终端法兰视图");
#endif
			pSectionView->m_iViewType=VIEW_FL;
			//将法兰上的肘板添加到法兰视图中
			if(pEndLj->m_iParamType==TYPE_FL ||pEndLj->m_iParamType==TYPE_FLG || pEndLj->m_iParamType==TYPE_FLR)
			{
				ATTACHPART_ITEM *pAttachItem=NULL;
				for(pAttachItem=pEndLj->GetFirstAttachPart();pAttachItem;pAttachItem=pEndLj->GetNextAttachPart())
					pSectionView->AddPart(pAttachItem->h);
				for(CLDSPart* pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
				{	//添加有焊接关系，但不是附加肘板的普通钢板
					if(!pPart->IsPlate())
						continue;
					if(pEndLj->GetAttachPartList()->GetValue(pPart->handle)!=NULL)
						continue;
					CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pPart;
					for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
					{
						if(pVertex->m_bWeldEdge&&pVertex->weldline.hWeldMotherPart==pEndLj->handle)
						{
							pAttachItem=pEndLj->AttachPart.Add(pPlate->handle);
							pAttachItem->idClsType=pPlate->GetClassTypeId();
							strncpy(pAttachItem->sPartNo,pPlate->GetPartNo(),16);
							pSectionView->AddPart(pPlate->handle);
							break;
						}
					}
				}
			}
		}
		else if(pEndLj->m_iParamType==TYPE_ROLLEND||pEndLj->m_iParamType==TYPE_UEND||pEndLj->m_iParamType==TYPE_XEND)
		{	//Z轴
			param.desAxisZ.norm_style=4;
			param.desAxisZ.hVicePart=pEndLj->handle;
			if(i==0)	//始端
			{
				param.desAxisZ.nearVector.Set(1,0,0);
				param.desAxisZ.vector=pEndLj->ucs.axis_x;
				strcpy(sPartNo,pEndLj->GetPartNo());
			}
			else	//插板统一自左(始)->右看(终)，截面图
			{
				param.desAxisZ.vector=-pEndLj->ucs.axis_x;
				param.desAxisZ.nearVector.Set(-1,0,0);
			}
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pSectionView->sName,"%s Plug-In View",(char*)pEndLj->GetPartNo());
#else
			sprintf(pSectionView->sName,"%s插板视图",(char*)pEndLj->GetPartNo());
#endif
			pSectionView->m_iViewType=VIEW_INSERTPLATE;
			if(param.desAxisZ.vector*pMainView->ucs.axis_x>0)//TODO:这里是不是错误了？ wjh-2017.12.1
				param.desAxisX.ReverseDirection();	//端头插板截面视图法线与主视图X轴一致时（如终端右侧插板）X轴反向
		}
		pSectionView->des_cs.SetParamInfo(param);
		pSectionView->UpdateUCS(m_pModel,this);
		pSectionView->InitSlaveSectionView(pMainView,BelongModel());
	}
	//3. 添加(始)终端法兰视图(塔脚板) wht 12-09-06
	for(int i=0;i<2;i++)
	{
		f3dPoint datum_axis_z=tube_vec;
		CLDSNode *pDatumNode=NULL;
		TUBEJOINT *pTubeJoint=NULL;
		if(i==0)
		{
			pTubeJoint=&pLineTube->m_tJointStart;
			pDatumNode=pLineTube->pStart;
		}
		else 
		{
			pTubeJoint=&pLineTube->m_tJointEnd;
			pDatumNode=pLineTube->pEnd;
			datum_axis_z*=-1.0;
		}
		if(pTubeJoint==NULL||pTubeJoint->cutPlaneNorm.IsZero()||pDatumNode==NULL)
			continue;
		CLDSPart *pDatumPlate=NULL;
		for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(!pPart->IsPlate()||fabs(pPart->ucs.axis_z*pTubeJoint->cutPlaneNorm)<EPS_COS2)
				continue;
			if(pPart->GetClassTypeId()==CLS_PLATE)
			{
				CLDSPlate *pPlate=(CLDSPlate*)pPart;
				if(pPlate->m_fInnerRadius<EPS)
					continue;
				CLDSNode *pBaseNode=pPlate->GetBaseNode();
				if(pBaseNode==pDatumNode)
				{
					pDatumPlate=pPart;
					break;
				}
			}
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
				double B=0;
				if(pParamPlate->m_iParamType!=TYPE_CIRRIBPLATE||
					!pParamPlate->GetDesignItemValue('B',&B,NULL)||fabs(B-360)>EPS)	//B:β,环向扇形角(外形参数)
					continue;
				CLDSNode *pBaseNode=pParamPlate->GetBaseNode();
				if(pBaseNode==pDatumNode)
				{
					pDatumPlate=pPart;
					break;
				}
			}
		}
		if(pDatumPlate)
		{	
			//CSectionView *pSectionView = SonViews.append();
			CSlaveSectionView *pSectionView = (CSlaveSectionView*)SonViews.Add(0,CSectionView::CLS_SLAVE_VIEW);
			pSectionView->index=view_index;	//视图索引
			view_index++;
			pSectionView->m_iViewType=VIEW_FL;
			//
			CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
			//基准点(改为以底板中心点为基点，防止底脚板中心与杆件端点不在同一Z平面，进而造成内圆绘制不合理 wjh-2016.10.14）
			param.desOrg.datum_pos_style=9;
			param.desOrg.des_para.hPart=pDatumPlate->handle;//pLineTube->handle;
			param.desOrg.datum_pos.Set();
			//param.desOrg.des_para.RODEND.bIncOddEffect=TRUE;
			//param.desOrg.des_para.RODEND.direction=i;
			//坐标轴
			param.m_eDatumPlane=CParaCS::DP_ZX;
			param.desAxisZ.norm_style=3;
			param.desAxisZ.hVicePart=pDatumPlate->handle;
			if(datum_axis_z*pPart->ucs.axis_z<0)
				param.desAxisZ.direction=1;
			else 
				param.desAxisZ.direction=0;

			param.desAxisX.norm_style=4;
			param.desAxisX.hVicePart=pDatumPlate->handle;
			param.desAxisX.nearVector.Set(1,0,0);
			pSectionView->des_cs.SetParamInfo(param);
			//
			if(i==0)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(pSectionView->sName,"Start Base Flange View");
			else
				strcpy(pSectionView->sName,"End Base Flange View");
#else
				strcpy(pSectionView->sName,"始端底脚法兰视图");
			else
				strcpy(pSectionView->sName,"终端底脚法兰视图");
#endif
			//必须置于pSectionView->sName赋值之后，否则出错时警示信息无法正常显示视图名称 wjh-2016.8.7
			pSectionView->UpdateUCS(m_pModel,this);
			pSectionView->AddPart(pDatumPlate->handle,pDatumPlate);
			pSectionView->AddPart(pLineTube->handle,pLineTube);
			for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			{
				if(!pPart->IsPlate()||pDatumPlate->handle==pPart->handle)
					continue;
				if(pPart->GetClassTypeId()==CLS_PLATE)
				{
					CLDSPlate *pPlate=(CLDSPlate*)pPart;
					CLDSNode *pBaseNode=pPlate->GetBaseNode();
					if(pBaseNode!=pDatumNode)
						continue;
				}
				else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				{
					CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
					if(pParamPlate->m_iParamType!=TYPE_RIBPLATE&&pParamPlate->m_iParamType!=TYPE_ELBOWPLATE)
						continue;
					CLDSNode *pBaseNode=pParamPlate->GetBaseNode();
					if(pBaseNode!=pDatumNode)
						continue;
				}
				if(pPart->m_hPartWeldParent==pDatumPlate->handle||pPart->m_hPartWeldParent==pLineTube->handle)
					pSectionView->AddPart(pPart->handle,pPart);
			}
			//
			pSectionView->InitSlaveSectionView(pMainView,BelongModel());
		}
	}
	return TRUE;
}
//</DEVELOP_PROCESS_MARK>

//<DEVELOP_PROCESS_MARK nameId="CLDSPartGroup::AddFootNailView">
//3.3.4 为组焊件中脚钉底座添加端面图
BOOL CLDSPartGroup::AddFootNailView(CLDSLineTube *pLineTube,CSectionView *pMainView,PARTSET &partset,int &view_index)
{
	CHashTable<CSectionView*> hashViewByWeldParentHandle;
	hashViewByWeldParentHandle.CreateHashTable(partset.GetNodeNum());
	for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		CLDSBolt *pBolt=NULL;	//脚钉底座上的脚钉,某些情况会存在不带脚钉的脚钉底座
		CLDSLineSlot *pLineSlot=NULL;
		CLDSLineAngle *pLineAngle=NULL;
		if(pPart->GetClassTypeId()==CLS_LINESLOT)
		{	//存在脚钉底座
			pLineSlot=(CLDSLineSlot*)pPart;
			pLineSlot->BuildUCS();
			if(!pLineSlot->IsBaseSlot())
				continue;
		}
		else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{	//存在角钢脚钉
			pLineAngle=(CLDSLineAngle*)pPart;
			if(pLineAngle==NULL)
				continue;
			pLineAngle->getUCS(pLineAngle->ucs);
			if(!pLineAngle->m_bFootNail)
				continue;	//非角钢脚钉
		}
		if(pLineSlot==NULL&&pLineAngle==NULL)
			continue;
		//此处以脚钉底座/角钢角钢的焊接父构件为关键字
		CSectionView *pView=NULL;
		if(hashViewByWeldParentHandle.GetValueAt(pPart->m_hPartWeldParent,pView))
		{
			if(pView==NULL)
				continue;
			BOOL bIgnoreSlot=FALSE;	//不重复添加与视图中脚钉角度相同的脚钉
			BOOL bIgnoreAngle=FALSE;//不重复添加与视图中脚钉角度相同的脚钉
			CSectionView::PART_ITEM *pPartItem = NULL;
			for(pPartItem=pView->SonParts.GetFirst();pPartItem;pPartItem=pView->SonParts.GetNext())
			{
				if(TagPart(pPartItem)==NULL)
					continue;
				double cosa=0;
				if(pLineSlot&&TagPart(pPartItem)->GetClassTypeId()==CLS_LINESLOT)
				{	//槽钢的X轴为高度方向，Y轴即为宽度方向，Z轴为长度方向，故Y轴表示槽钢脚钉的射线方向
					cosa=pLineSlot->ucs.axis_y*TagPart(pPartItem)->ucs.axis_y;
					if(cosa>EPS_COS)
					{
						bIgnoreSlot=TRUE;
						break;
					}
				}
				else if(pLineAngle&&TagPart(pPartItem)->GetClassTypeId()==CLS_LINEANGLE)
				{
					cosa=pLineAngle->ucs.axis_z*TagPart(pPartItem)->ucs.axis_z;
					if(cosa>EPS_COS)
					{
						bIgnoreAngle=TRUE;
						break;
					}
				}
			}
			if((pLineSlot&&bIgnoreSlot)||(pLineAngle&&bIgnoreAngle))
				continue;
			if(pView->m_iViewType==VIEW_FOOTNAILSLOT&&pLineSlot)
			{
				pPartItem=pView->SonParts.Add(pLineSlot->handle);
				//pPartItem->hSonPart = pLineSlot->handle;
				pView->LoadTagInfo(pPartItem)->pCurPart = pLineSlot;
				pPartItem->cFlag = 0;
			}
			else if(pView->m_iViewType==VIEW_FOOTNAILANGLE&&pLineAngle)
			{
				pPartItem=pView->SonParts.Add(pLineAngle->handle);
				//pPartItem->hSonPart = pLineAngle->handle;
				pView->LoadTagInfo(pPartItem)->pCurPart = pLineAngle;
				pPartItem->cFlag = 0;
			}
		}
		else
		{
			CLDSPart *pFatherPart=BelongModel()->FromPartHandle(pPart->m_hPartWeldParent);
			if(pFatherPart==NULL)
				continue;
			CSlaveSectionView *pSectionView = (CSlaveSectionView*)SonViews.Add(0,CSectionView::CLS_SLAVE_VIEW);
			//CSectionView *pSectionView = SonViews.append();
			pSectionView->m_bLocked=TRUE;
			pSectionView->index=view_index;
			view_index++;
			if(pLineSlot)
			{
				pSectionView->m_iViewType=VIEW_FOOTNAILSLOT;
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(pSectionView->sName,"Foot Nail Base View");
#else
				strcpy(pSectionView->sName,"脚钉底座视图");
#endif
			}
			else
			{	
				pSectionView->m_iViewType=VIEW_FOOTNAILANGLE;
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(pSectionView->sName,"Angle Foot Nail View");
#else
				strcpy(pSectionView->sName,"角钢脚钉视图");
#endif
			}
			CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
			//基准点
			param.desOrg.datum_pos_style=1;
			param.desOrg.des_para.RODEND.hRod=pLineTube->handle;
			param.desOrg.des_para.RODEND.bIncOddEffect=TRUE;
			param.desOrg.des_para.RODEND.direction=0;
			//坐标轴
			param.m_eDatumPlane=CParaCS::DP_ZX;
			param.desAxisZ.norm_style=5;
			param.desAxisZ.hVicePart=pLineTube->handle;
			param.desAxisZ.direction=1;
			//调整端面图与主正视图匹配
			if(pMainView&&pMainView->des_cs.IsDesAxisZ())
				param.desAxisX=pMainView->des_cs.GetParamInfo().desAxisZ;
			else
			{
				param.desAxisX.norm_style=4;
				param.desAxisX.hVicePart=pLineTube->handle;
				param.desAxisX.nearVector.Set(1,0,0);
			}
			pSectionView->des_cs.SetParamInfo(param);
			pSectionView->UpdateUCS(m_pModel,this);

			pSectionView->AddPart(pLineTube->handle,pLineTube);
			pSectionView->AddPart(pPart->handle,pPart);
			pSectionView->InitSlaveSectionView(pMainView,BelongModel());
			hashViewByWeldParentHandle.SetValueAt(pFatherPart->handle,pSectionView);
		}
	}
	return TRUE;
}
//</DEVELOP_PROCESS_MARK>

//3.5 添加钢管抱杆视图(从塔中心向外看)
BOOL CLDSPartGroup::AddTubeBushingPlateView(CLDSLineTube *pLineTube,PARTSET &partset)
{
	//1.提取区间段
	ATOM_LIST<SECTION_SEG> sectionSegList;
	ExtractSectionSegs(pLineTube,partset,sectionSegList);
	//2、按节点将钢板(衬托板、连接板、封板)分类，并找出所有存在衬托板的节点
	NODESET validNodeSet;
	CHashList<PARTSET> hashPlatesByNodeHandle;
	for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(!pPart->IsPlate())
			continue;
		CLDSGeneralPlate *pPlate=(CLDSGeneralPlate*)pPart;
		CLDSNode *pBaseNode=pPart->BelongModel()->FromNodeHandle(pPlate->m_hBaseNode);
		if(pBaseNode==NULL)
			continue;
		BOOL bNeedAdd=FALSE;
		if(pPlate->GetClassTypeId()==CLS_PARAMPLATE&&
			(((CLDSParamPlate*)pPlate)->m_iParamType==TYPE_CIRCOVERPLATE||
			((CLDSParamPlate*)pPlate)->m_iParamType==TYPE_CIRRIBPLATE))
			bNeedAdd=TRUE;//封板肋板
		if(!bNeedAdd&&pPlate->GetClassTypeId()!=CLS_PLATE)
			continue;			//不是封板、衬托板和普通钢板
		if(pPlate->IsBushWeldPlate())
			validNodeSet.append(pBaseNode);
		PARTSET *pPartSetPtr=hashPlatesByNodeHandle.GetValue(pBaseNode->handle);
		if(pPartSetPtr==NULL)
			pPartSetPtr=hashPlatesByNodeHandle.Add(pBaseNode->handle);
		pPartSetPtr->append(pPlate);
	}
	//3.根据衬托板节点生成衬托板视图
	for(CLDSNode *pNode=validNodeSet.GetFirst();pNode;pNode=validNodeSet.GetNext())
	{
		PARTSET *pSet=hashPlatesByNodeHandle.GetValue(pNode->handle);
		if(pSet==NULL)
			continue;
		//2.1 找到衬托板
		CLDSGeneralPlate *pBushingPlate=NULL;
		for(CLDSPart *pPart=pSet->GetFirst();pPart;pPart=pSet->GetNext())
		{
			if(pPart==NULL||!pPart->IsPlate())
				continue;
			if(((CLDSGeneralPlate*)pPart)->IsBushWeldPlate())
			{
				pBushingPlate=(CLDSGeneralPlate*)pPart;
				break;
			}
		}
		//2.2 找到衬托板所在的区段
		SECTION_SEG *pSeg=NULL;
		for(pSeg=sectionSegList.GetFirst();pSeg;pSeg=sectionSegList.GetNext())
		{
			SECTION_PART *pSegPart=NULL;
			for(pSegPart=pSeg->sectionPartList.GetFirst();pSegPart;pSegPart=pSeg->sectionPartList.GetNext())
			{
				if(pSegPart->pPart->handle==pBushingPlate->handle)
					break;
			}
			if(pSegPart)
				break;
		}
		//2.3 添加视图
		CTubeBushPlateView *pSectionView = (CTubeBushPlateView*)SonViews.Add(0,CSectionView::CLS_BUSH_PLATE_VIEW);
		pSectionView->m_bLocked=TRUE;
		pSectionView->index=0;
		pSectionView->m_iViewType=VIEW_FRONT;
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(pSectionView->sName,"%s Bearing Plate View",(char*)pBushingPlate->Label);
#else
		sprintf(pSectionView->sName,"%s衬托板视图",(char*)pBushingPlate->Label);
#endif
		CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
		//基准点
		param.desOrg.datum_pos_style=1;
		param.desOrg.des_para.RODNODE.hNode=pBushingPlate->m_hBaseNode;
		param.desOrg.des_para.RODNODE.hRod=pLineTube->handle;
		param.desOrg.des_para.RODEND.direction=0;
		//坐标轴
		param.m_eDatumPlane=CParaCS::DP_XY;
		//X轴
		param.desAxisX.norm_style=5;
		param.desAxisX.hVicePart=pLineTube->handle;
		param.desAxisX.direction=0;
		//Y轴
		param.desAxisY.norm_style=3;
		param.desAxisY.hVicePart=pBushingPlate->handle;
		param.desAxisY.direction=0;
		pSectionView->des_cs.SetParamInfo(param);
		pSectionView->UpdateUCS(m_pModel,this);
		//修正坐标系保证Z轴放样与衬托板延展方向一致
		f3dPoint verfiy_vec=pLineTube->GetPlateVertSpreadVec(pBushingPlate);
		if(verfiy_vec*pSectionView->ucs.axis_z<0)
		{
			param.desAxisY.direction=1;
			pSectionView->des_cs.SetParamInfo(param);
			pSectionView->UpdateUCS(m_pModel,this);
		}
		pSectionView->AddPart(pLineTube->handle,pLineTube);
		if(pSeg)
		{
			for(SECTION_PART *pSegPart=pSeg->sectionPartList.GetFirst();pSegPart;pSegPart=pSeg->sectionPartList.GetNext())
			{
				if(pSegPart->pPart->GetClassTypeId()==CLS_PARAMPLATE)
				{
					CLDSParamPlate *pPlate=(CLDSParamPlate*)pSegPart->pPart;
					if(pPlate->IsFL()||pPlate->IsInsertPlate())
						continue;
				}
				pSectionView->AddPart(pSegPart->pPart->handle,pSegPart->pPart);
			}
		}
		else
		{
			for(CLDSPart *pPart=pSet->GetFirst();pPart;pPart=pSet->GetNext())
			{
				if(pPart==NULL||!pPart->IsPlate())
					continue;
				pSectionView->AddPart(pPart->handle,pPart);
			}
		}
	}
	return TRUE;
}

//<DEVELOP_PROCESS_MARK nameId="CLDSPartGroup::AddCommonRodPartGroupViews">
//提取工作面的展开信息
static int _LocalCompareWorkPlanePriorityFunc(const CPartGroupWorkPlane &pObj1,const CPartGroupWorkPlane &pObj2)
{
	if(pObj1.nRayTubeNum>pObj2.nRayTubeNum)
		return -1;
	else if(pObj1.nRayTubeNum<pObj2.nRayTubeNum)
		return 1;
	else if(pObj1.nRayTubeNum==pObj2.nRayTubeNum && pObj1.nPartNum>=pObj1.nPartNum)
		return -1;
	else if(pObj1.nRayTubeNum==pObj2.nRayTubeNum && pObj1.nPartNum==pObj1.nPartNum)
		return 0;
	else
		return 1;
}
static int compare_func2(const SECTION_PART &pPart1,const SECTION_PART &pPart2)
{
	if(pPart1.prjLine.startPt.x>pPart2.prjLine.startPt.x)
		return 1;
	else if(fabs(pPart1.prjLine.startPt.x-pPart2.prjLine.startPt.x)<EPS2)
		return 0;
	else
		return -1;
}

typedef struct AVER_NORMAL{
	UINT id;
	UINT uiStatCount;	//当前已统计构件法线的次数
	bool hasUpperParts,hasUnderParts;	//钢管左侧为上
	BYTE ciSpreadDirection;	//0.自动展开；1．展开至上侧；2.展开至下侧；3.展开至主管前侧4.展开至主管后侧
	double alfa;	//绕X轴以向上Y轴为起始边的旋转角度，弧度制
	double cosa;	//=cos(alfa),绝对值越大表示,越接近于当前绘图平面
	GEPOINT vCurrNormal;				//已单位化统计平均法线
	void SetKey(DWORD key){id=key;}
	UINT get_uiSerial(){return id&0x7fffffff;}
	UINT get_uiGroup(){return (id&0x80000000)>>31;}
	__declspec(property(get=get_uiSerial)) UINT uiSerial;
	__declspec(property(get=get_uiGroup)) UINT uiGroup;
	AVER_NORMAL(){uiStatCount=ciSpreadDirection=0;hasUpperParts=hasUnderParts=false;cosa=alfa=0;}
}*AVER_NORMALPTR;
bool bSpreadPartGroupMainViewInThreeTubeTower=false;
static void RetrieveWorkPlateSpreadInfo(CLDSLinePart* pRootLinePart,CLDSLinePart* pJointRod,CHashListEx<CPartGroupWorkPlane>& workplanelist,PARTSET& partset)
{
	CPartGroupWorkPlane* pWorkPlane=NULL;
	CLDSLineTube* pRootTube=NULL;
	if(pRootLinePart&&pRootLinePart->GetClassTypeId()==CLS_LINETUBE)
		pRootTube=(CLDSLineTube*)pRootLinePart;
	CHashList<long> hashEndPlates;
	if(pRootTube)
	{
		if(pRootTube->m_tJointStart.hLinkObj>0x20)
		{
			hashEndPlates.SetValue(pRootTube->m_tJointStart.hLinkObj,pRootTube->m_tJointStart.hLinkObj);
			CLDSParamPlate *pEndLjPlate=(CLDSParamPlate*)pRootTube->BelongModel()->FromPartHandle(pRootTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
			if(pEndLjPlate)
			{
				for(ATTACHPART_ITEM *pItem=pEndLjPlate->GetFirstAttachPart();pItem;pItem=pEndLjPlate->GetNextAttachPart())
				{
					if(!hashEndPlates.GetValue(pItem->h))
						hashEndPlates.SetValue(pItem->h,pItem->h);
				}
			}
		}
		if(pRootTube->m_tJointEnd.hLinkObj>0x20)
		{
			hashEndPlates.SetValue(pRootTube->m_tJointEnd.hLinkObj,pRootTube->m_tJointEnd.hLinkObj);
			CLDSParamPlate *pEndLjPlate=(CLDSParamPlate*)pRootTube->BelongModel()->FromPartHandle(pRootTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
			if(pEndLjPlate)
			{
				for(ATTACHPART_ITEM *pItem=pEndLjPlate->GetFirstAttachPart();pItem;pItem=pEndLjPlate->GetNextAttachPart())
				{
					if(!hashEndPlates.GetValue(pItem->h))
						hashEndPlates.SetValue(pItem->h,pItem->h);
				}
			}
		}
	}
	//1、初始化工作面内的优先级判断依据(所属工作面的支管数和板构件数)
	ARRAY_LIST<SECTION_PART> section;
	for(pWorkPlane=workplanelist.GetFirst();pWorkPlane;pWorkPlane=workplanelist.GetNext())
	{
		section.SetSize(0);
		for(CLDSPart* pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(pPart->GetClassTypeId()==CLS_LINETUBE)	
			{
				CLDSLineTube* pTube=(CLDSLineTube*)pPart;
				f3dPoint vec=pTube->End()-pTube->Start();
				normalize(vec);
				double cosa=fabs(vec*pWorkPlane->workPlaneNorm);
				if(cosa<EPS)
					pWorkPlane->nRayTubeNum++;
			}
			else if(pPart->IsPlate()&&hashEndPlates.GetValue(pPart->handle)==NULL&&
				(pRootLinePart==NULL||pPart->m_hPartWeldParent==pRootLinePart->handle)&&
				((CLDSGeneralPlate*)pPart)->IsConnectPlate())
			{
				double cosa=fabs(pPart->ucs.axis_z*pWorkPlane->workPlaneNorm);
				//if(cosa>0.999847695)	//coa1=0.999847695 板在工作面内
				if(cosa>0.99619469)		//COS(5°)=0.99619469809174553229501040247389
				{
					pWorkPlane->nPartNum++;
					SECTION_PART* pSectionPart=section.append();
					pSectionPart->pPart=pPart;
					pSectionPart->prjLine.startPt=pSectionPart->prjLine.endPt=pPart->ucs.origin;
				}
			}
		}
		if(section.GetSize()>0)
		{
			if(pWorkPlane->pTempSectionParts)
				delete []pWorkPlane->pTempSectionParts;
			pWorkPlane->pTempSectionParts=new SECTION_PART[section.GetSize()];
			memcpy(pWorkPlane->pTempSectionParts,section.m_pData,sizeof(SECTION_PART)*section.GetSize());
		}
	}
	//2、对工作面进行优先排序
	ARRAY_LIST<CPartGroupWorkPlane> workPlateArr;
	workPlateArr.SetSize(0,workplanelist.GetNodeNum());
	GEPOINT lenRootRodVec=pRootLinePart->End()-pRootLinePart->Start();
	normalize(lenRootRodVec);
	for(pWorkPlane=workplanelist.GetFirst();pWorkPlane;pWorkPlane=workplanelist.GetNext())
		pWorkPlane->CalYAxisVec(pRootLinePart->IsTube()?(CLDSLineTube*)pRootLinePart:NULL);
	for(pWorkPlane=workplanelist.GetFirst();pWorkPlane;pWorkPlane=workplanelist.GetNext())
	{
		if(pWorkPlane->pDatumPart->IsTube()&&pRootLinePart->IsTube())
		{
			GEPOINT lenBranchVec=((CLDSLineTube*)pWorkPlane->pDatumPart)->End()-((CLDSLineTube*)pWorkPlane->pDatumPart)->Start();
			normalize(lenBranchVec);
			if(fabs(lenBranchVec*lenRootRodVec)>0.94)	//cos(20°)
				pWorkPlane->bJointTubePlane=true;
		}
		workPlateArr.append(*pWorkPlane);
	}
	CQuickSort<CPartGroupWorkPlane>::QuickSort(workPlateArr.m_pData,workPlateArr.GetSize(),_LocalCompareWorkPlanePriorityFunc);
	//主视图应优先取接近前后左右的工作面 wht 18-03-22
	int iMainWorkPlane=-1;
	CPartGroupWorkPlane *pMainWorkPlane=NULL;
	GEPOINT mainNormArr[4]={GEPOINT(0,1,0),GEPOINT(0,-1,0),GEPOINT(-1,0,0),GEPOINT(1,0,0)};
	double maxCosa=0;
	for(int i=0;i<workPlateArr.GetSize();i++)
	{
		for(int j=0;j<4;j++)
		{
			double cosa=fabs(workPlateArr[i].workPlaneNorm*mainNormArr[j]);
			if(cosa>maxCosa)
			{
				maxCosa=cosa;
				pMainWorkPlane=&workPlateArr[i];
				iMainWorkPlane=i;
			}
		}
	}
	double cos75=0.25881904510252076234889883762405;
	if(pMainWorkPlane&&maxCosa<cos75)
	{
		iMainWorkPlane=-1;
		pMainWorkPlane=NULL;
	}
	if(pRootTube&&(workplanelist.GetNodeNum()==2||workplanelist.GetNodeNum()==3))
	{	//1.找到主视图
		int iMainPlane=0;
		if(workplanelist.GetNodeNum()==3)
		{
			double fCosa01=fabs(workPlateArr[0].vTempAxisY*workPlateArr[1].vTempAxisY);
			double fCosa02=fabs(workPlateArr[0].vTempAxisY*workPlateArr[2].vTempAxisY);
			double fCosa12=fabs(workPlateArr[1].vTempAxisY*workPlateArr[2].vTempAxisY);
			if( (fabs(fCosa12)>fabs(fCosa01)&&fabs(fCosa12)>fabs(fCosa02))||
				(fabs(fCosa01)>fabs(fCosa02)&&fabs(fCosa01)>fabs(fCosa12)))
				iMainPlane=1;
			else if((fabs(fCosa02)>fabs(fCosa01)&&fabs(fCosa02)>fabs(fCosa01)))
				iMainPlane=2;
		}
		if(pMainWorkPlane&&iMainWorkPlane>=0)
			iMainPlane=iMainWorkPlane;
		//2.以主视图为基准计算夹角
		CParaCS des_cs;
		des_cs.SetParamInfo(workPlateArr[iMainPlane].csParaInfo);
		des_cs.SetModifiedState();
		des_cs.UpdateACS(pRootTube?pRootTube->BelongModel():NULL);
		GECS acs=des_cs.GetACS();
		GEPOINT vDatumRodAxis=pRootTube->End()-pRootTube->Start();
		if(vDatumRodAxis*acs.axis_x<0)
			vDatumRodAxis*=-1;
		GEPOINT vDatumVertAxisY=acs.axis_z^vDatumRodAxis;
		normalize(vDatumRodAxis);
		normalize(vDatumVertAxisY);
		GECS cs(pRootTube->Start(),vDatumRodAxis,vDatumVertAxisY,acs.axis_z);
		AVER_NORMAL normalArr[3];
		for(int i=0;i<workPlateArr.GetSize();i++)
		{
			GEPOINT vPlateAxisY=workPlateArr[i].vTempAxisY;
			vPlateAxisY-=(vPlateAxisY*vDatumRodAxis)*vDatumRodAxis;
			normalArr[i].alfa=Cal2dLineAng(0,0,vPlateAxisY*cs.axis_y,vPlateAxisY*acs.axis_z);
			normalArr[i].cosa=cos(normalArr[i].alfa);
			normalArr[i].id=i;
		}
		//3.添加展开面
		workplanelist.Empty();
		CPartGroupWorkPlane *pMainPlane=workplanelist.SetValue(workPlateArr[iMainPlane].iNo,workPlateArr[iMainPlane]);
		//当前工作面内钢板延伸方向
		BYTE ciMainDirection=CSectionView::PART_ITEM::SPREAD_TO_UNDER;
		if(pMainPlane->vTempAxisY*acs.axis_y>0)
			ciMainDirection=CSectionView::PART_ITEM::SPREAD_TO_UPPER;
		//初始化展开面参数，最后如果不需要展开面参数时再清空
		pMainPlane->assistant.m_nStyle=0;
		pMainPlane->assistant2.m_nStyle=0;	//展开面
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(pMainPlane->assistant.name,"Extend Plane");
		strcpy(pMainPlane->assistant2.name,"Extend Plane 2");
#else
		strcpy(pMainPlane->assistant.name,"展开面");
		strcpy(pMainPlane->assistant2.name,"展开面2");
#endif
		const double ANGLE_THRESHOLD=Pi*0.25;
		if(workPlateArr.GetSize()==2)
		{	//
			int iOtherPlane=1-iMainPlane;
			double fOtherAngle=normalArr[iOtherPlane].alfa;
			//
			CPartGroupWorkPlane *pSpreadPlane=&workPlateArr[iOtherPlane];
			pSpreadPlane->csParaInfo.desOrg.UpdatePos(pMainPlane->pDatumPart->BelongModel());
			pSpreadPlane->csParaInfo.desAxisZ.UpdateVector(pMainPlane->pDatumPart->BelongModel());
			pMainPlane->assistant.iNo=pSpreadPlane->iNo;
			pMainPlane->assistant.m_xLineStart=pSpreadPlane->csParaInfo.desOrg;
			pMainPlane->assistant.m_xFaceNorm=pSpreadPlane->csParaInfo.desAxisZ;
			if(bSpreadPartGroupMainViewInThreeTubeTower||CLDSPart::CustomerSerial==8)	//CLDSPart::CustomerSerial=8.云南景云祥)
			{
				if( (fOtherAngle>=0&&fOtherAngle<ANGLE_THRESHOLD)||
					(fOtherAngle>=(2*Pi-ANGLE_THRESHOLD)&&fOtherAngle<Pi*2))
				{	//钢板在正负15度之内,展开方向与主面方向一致
					pMainPlane->assistant.ciSpreadDirection=ciMainDirection;
					pMainPlane->assistant.bSpreadOverRidge=false;
				}
				else 
				{	//其他情况均展开主工作面相反方向
					pMainPlane->assistant.ciSpreadDirection=3-ciMainDirection;
					if(normalArr[iOtherPlane].cosa>0)	//夹角大于90度跨越钢管轴线
						pMainPlane->assistant.bSpreadOverRidge=true;
					else
						pMainPlane->assistant.bSpreadOverRidge=false;
				}
			}
			else
			{
				pMainPlane->assistant.bSpreadOverRidge=false;
				if( (fOtherAngle>=0&&fOtherAngle<ANGLE_THRESHOLD)||(fOtherAngle>=(2*Pi-ANGLE_THRESHOLD)&&fOtherAngle<Pi*2))
					pMainPlane->assistant.ciSpreadDirection=ciMainDirection;
				else if(fOtherAngle>=ANGLE_THRESHOLD&&fOtherAngle<(Pi-ANGLE_THRESHOLD))
					pMainPlane->assistant.ciSpreadDirection=CSectionView::PART_ITEM::SPREAD_TO_TOP;
				else if(fOtherAngle>=(Pi-ANGLE_THRESHOLD)&&fOtherAngle<(Pi+ANGLE_THRESHOLD))
					pMainPlane->assistant.ciSpreadDirection=3-ciMainDirection;
				else if(fOtherAngle>=(Pi+ANGLE_THRESHOLD)&&fOtherAngle<(2*Pi-ANGLE_THRESHOLD))
					pMainPlane->assistant.ciSpreadDirection=CSectionView::PART_ITEM::SPREAD_TO_BACK;
				if(pMainPlane->assistant.ciSpreadDirection>2)
				{	//只有水平展开时添加展开面,展开面非水平展开时清空展开面 wht 18-03-13
					pMainPlane->assistant.iResidePlaneNo=0;
					pMainPlane->assistant.ciSpreadDirection=0;
					pMainPlane->assistant.m_xFaceNorm.norm_style=0;
					pMainPlane->assistant.m_xFaceNorm.vector.Set();
					pMainPlane->assistant.m_xFaceNorm.nearVector.Set();
					//
					pSpreadPlane=NULL;
					workplanelist.SetValue(workPlateArr[iOtherPlane].iNo,workPlateArr[iOtherPlane]);
				}
			}
			if(pSpreadPlane&&pSpreadPlane->pTempSectionParts)
			{
				delete []pSpreadPlane->pTempSectionParts;
				pSpreadPlane->pTempSectionParts=NULL;
			}
		}
		if(workPlateArr.GetSize()==3)
		{	//排序使各平面的alfa角由小至大
			AVER_NORMAL temparr[3]={normalArr[0],normalArr[1]};
			if(normalArr[0].alfa>normalArr[1].alfa)
			{
				temparr[0]=normalArr[1];
				temparr[1]=normalArr[0];
			}
			if(temparr[0].alfa>normalArr[2].alfa)
			{
				temparr[2]=temparr[1];
				temparr[1]=temparr[0];
				temparr[0]=normalArr[2];
			}
			else if(temparr[1].alfa>normalArr[2].alfa)
			{
				temparr[2]=temparr[1];
				temparr[1]=normalArr[2];
			}
			else
				temparr[2]=normalArr[2];
			normalArr[0]=temparr[0];
			normalArr[1]=temparr[1];
			normalArr[2]=temparr[2];
			int iDatumNorm=0;
			if(pMainWorkPlane&&iMainWorkPlane>0)
			{
				for(int i=0;i<3;i++)
				{
					if(normalArr[i].id==iMainWorkPlane)
					{
						iDatumNorm=i;
						break;
					}
				}
			}
			BYTE planeDirArr[3]={0,0,0};
			for(int i=0;i<3;i++)
			{
				if(iDatumNorm==i)
					continue;
				double fOtherAngle=fabs(normalArr[iDatumNorm].alfa-normalArr[i].alfa);
				if( (fOtherAngle>=0&&fOtherAngle<ANGLE_THRESHOLD)||(fOtherAngle>=(2*Pi-ANGLE_THRESHOLD)&&fOtherAngle<Pi*2))
					planeDirArr[i]=ciMainDirection;
				else if(fOtherAngle>=ANGLE_THRESHOLD&&fOtherAngle<(Pi-ANGLE_THRESHOLD))
					planeDirArr[i]=CSectionView::PART_ITEM::SPREAD_TO_TOP;
				else if(fOtherAngle>=(Pi-ANGLE_THRESHOLD)&&fOtherAngle<(Pi+ANGLE_THRESHOLD))
					planeDirArr[i]=3-ciMainDirection;
				else if(fOtherAngle>=(Pi+ANGLE_THRESHOLD)&&fOtherAngle<(2*Pi-ANGLE_THRESHOLD))
					planeDirArr[i]=CSectionView::PART_ITEM::SPREAD_TO_BACK;
			}
			int iAssistant=0;
			typedef CDrawingAssistant* CDrawingAssistantPtr;
			CDrawingAssistantPtr assistantPtrArr[2]={&pMainPlane->assistant,&pMainPlane->assistant2};
			for(int i=0;i<3;i++)
			{
				if(iDatumNorm==i)
					continue;
				int iWorkPlate=normalArr[i].id;
				if(planeDirArr[i]==0||planeDirArr[i]>2)
				{	//只有水平展开时添加展开面 wht 18-03-13
					workplanelist.SetValue(workPlateArr[iWorkPlate].iNo,workPlateArr[iWorkPlate]);
					continue;
				}
				CPartGroupWorkPlane *pSpreadPlane=&workPlateArr[iWorkPlate];
				pSpreadPlane->csParaInfo.desOrg.UpdatePos(pMainPlane->pDatumPart->BelongModel());
				pSpreadPlane->csParaInfo.desAxisZ.UpdateVector(pMainPlane->pDatumPart->BelongModel());
				CDrawingAssistant *pAssistant=assistantPtrArr[iAssistant];
				pAssistant->iNo=pSpreadPlane->iNo;
				pAssistant->m_xLineStart=pSpreadPlane->csParaInfo.desOrg;
				pAssistant->m_xFaceNorm=pSpreadPlane->csParaInfo.desAxisZ;
				pAssistant->bSpreadOverRidge=false;
				pAssistant->ciSpreadDirection=planeDirArr[i];
				iAssistant++;
				if(pSpreadPlane->pTempSectionParts)
				{
					delete []pSpreadPlane->pTempSectionParts;
					pSpreadPlane->pTempSectionParts=NULL;
				}
			}
		}
	}
	else
	{
		workplanelist.Empty();
		if(pMainWorkPlane&&iMainWorkPlane>=0)
			workplanelist.SetValue(pMainWorkPlane->iNo,*pMainWorkPlane);
		for(int i=0;i<workPlateArr.GetSize();i++)
		{
			if(pMainWorkPlane&&iMainWorkPlane==i)
				continue;
			workplanelist.SetValue(workPlateArr[i].iNo,workPlateArr[i]);
		}
	}
	/*
	workplanelist.Empty();
	for(int i=0;i<workPlateArr.GetSize();i++)
		workplanelist.SetValue(workPlateArr[i].iNo,workPlateArr[i]);
	//3、对工作面进行展开处理
	double cos30=0.866025403,cos60=0.5;
	for(CPartGroupWorkPlane *pMainPlane=workplanelist.GetFirst();pMainPlane;pMainPlane=workplanelist.GetNext())
	{
		CPartGroupWorkPlane *pSpreadPlane=NULL;
		f3dPoint mainAxisY= pMainPlane->vTempAxisY;
		int nPush=workplanelist.push_stack();
		long dwObsoleteItem=-1;
		double fAcrossCosa=0;
		for(pWorkPlane=workplanelist.GetNext();pWorkPlane;pWorkPlane=workplanelist.GetNext())
		{
			if(pWorkPlane->bJointTubePlane)
				continue;	//不能展开对接钢管对角焊视图
			double fCosa=-(mainAxisY*pWorkPlane->vTempAxisY);
			if(//(bSpreadPartGroupMainViewInThreeTubeTower&&fCosa>-cos30&&fCosa>fAcrossCosa)||
				(bSpreadPartGroupMainViewInThreeTubeTower&&fCosa>cos30&&fCosa>fAcrossCosa))
			{	//小于30°认为近似共面
				pSpreadPlane=pWorkPlane;
				fAcrossCosa=fCosa;
				dwObsoleteItem=workplanelist.GetCursorKey();
			}
		}
		if(pSpreadPlane)
		{
			pMainPlane->assistant.m_nStyle=0;	//展开面
			pMainPlane->assistant.iNo=pSpreadPlane->iNo;
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(pMainPlane->assistant.name,"Extend Plane");
#else
			strcpy(pMainPlane->assistant.name,"展开面");
#endif
			pSpreadPlane->csParaInfo.desOrg.UpdatePos(pMainPlane->pDatumPart->BelongModel());
			pSpreadPlane->csParaInfo.desAxisZ.UpdateVector(pMainPlane->pDatumPart->BelongModel());
			pMainPlane->assistant.m_xLineStart=pSpreadPlane->csParaInfo.desOrg;
			pMainPlane->assistant.m_xFaceNorm=pSpreadPlane->csParaInfo.desAxisZ;
			if(bSpreadPartGroupMainViewInThreeTubeTower)
			{	//展开后的目标方位1.主管上侧2.主管下侧3.主管前面4.主管背面
				CParaCS des_cs;
				des_cs.SetParamInfo(pMainPlane->csParaInfo);
				des_cs.SetModifiedState();
				des_cs.UpdateACS(pRootTube?pRootTube->BelongModel():NULL);
				if(pMainPlane->vTempAxisY*des_cs.GetACS().axis_y>0)
					pMainPlane->assistant.ciSpreadDirection=CSectionView::PART_ITEM::SPREAD_TO_UNDER;
				else //if(pMainPlane->vTempAxisY*des_cs.GetACS().axis_y>0)
					pMainPlane->assistant.ciSpreadDirection=CSectionView::PART_ITEM::SPREAD_TO_UPPER;
			}
			if(pSpreadPlane->pTempSectionParts)
			{
				delete []pSpreadPlane->pTempSectionParts;
				pSpreadPlane->pTempSectionParts=NULL;
			}
		}
		if(dwObsoleteItem>0)
			workplanelist.DeleteNode(dwObsoleteItem);
		workplanelist.pop_stack(nPush);
		if(pSpreadPlane)
			break;
	}*/
}
struct APPROPRIATE_NORMAL{
protected:
	CPartGroupWorkPlane *pAppropriatePlane;		//最迎合钢管基准面的工作面
	CPartGroupWorkPlane *pAppropriateOrthoPlane;//最迎合正侧面X/Y正交轴向的工作面
	double fMeetingDegreeOfNormal;	//与钢管基准面的迎合度
	double fMeetingDegreeOfOrtho;	//与正侧面的迎合度（优先正面）
public:
	APPROPRIATE_NORMAL(){
		pAppropriatePlane=pAppropriateOrthoPlane=NULL;
		fMeetingDegreeOfNormal=0;
		fMeetingDegreeOfOrtho =1;
	}
	void MeetingCheck(CPartGroupWorkPlane* pPlane,const double* vTubeNormal)
	{
		//const double COS1DEG =0.99985;	//cos( 1°)
		//const double COS2DEG =0.99939;	//cos( 2°)
		const double COS10DEG=0.98481;	//cos(10°)
		const double SIN01DEG=0.001745;	//sin(0.1°)
		static const double MeetingDegreeOfUp2Level=0.99939;//COS2DEG;

		const double* vPlaneNormal=pPlane->workPlaneNorm;
		double meetingOfNormal=vPlaneNormal[0]*vTubeNormal[0]+vPlaneNormal[1]*vTubeNormal[1]+vPlaneNormal[2]*vTubeNormal[2];
		if(meetingOfNormal>fMeetingDegreeOfNormal)
		{
			fMeetingDegreeOfNormal=meetingOfNormal;
			pAppropriatePlane=pPlane;
		}
		if(fMeetingDegreeOfNormal>=MeetingDegreeOfUp2Level)
			return;	//已经有一平面符合钢管基准面了，不需要再检测正侧面
		double meetingOfOrtho=1;
		//检测与正面的迎合度
		if(fabs(vPlaneNormal[0])<=SIN01DEG&&fabs(vPlaneNormal[2])>=COS10DEG)
			meetingOfOrtho=fabs(vPlaneNormal[0]);
		if(meetingOfOrtho<fMeetingDegreeOfOrtho)
		{
			fMeetingDegreeOfOrtho=meetingOfOrtho;
			pAppropriateOrthoPlane=pPlane;
		}
		//检测与侧面的迎合度
		meetingOfOrtho=1;
		if(fabs(vPlaneNormal[1])<=SIN01DEG&&fabs(vPlaneNormal[2])>=COS10DEG)
			meetingOfOrtho=fabs(vPlaneNormal[1])*1.1;
		if(meetingOfOrtho<fMeetingDegreeOfOrtho)
		{
			fMeetingDegreeOfOrtho=meetingOfOrtho;
			pAppropriateOrthoPlane=pPlane;
		}
	}
	CPartGroupWorkPlane* get_MainPlane(){
		static const double MeetingDegreeOfUp2Level=0.99939;//COS2DEG;
		if(fMeetingDegreeOfNormal>=MeetingDegreeOfUp2Level)
			return pAppropriatePlane;	//优先钢管工作面为主视图
		double fMeetingDegreeOfOrthoCosa=SQRT(1-fMeetingDegreeOfOrtho*fMeetingDegreeOfOrtho);
		if(fMeetingDegreeOfNormal>fMeetingDegreeOfOrthoCosa)
			return pAppropriatePlane;
		else
			return pAppropriateOrthoPlane;
	}
	__declspec(property(get=get_MainPlane)) CPartGroupWorkPlane* pMainPlane;
};
BOOL CLDSPartGroup::AdjustFrontViewWorkNormByInsertPlate(CLDSLinePart *pRootRod,PARTSET &partset)
{
	LOCALE_PROPITEM* pLocaleItem = gxLocalizer.GetLocaleItem("AdjustFrontView");
	if(pLocaleItem==NULL || !pLocaleItem->bVal)
		return FALSE;	//汇金通根据插板调整视图坐标系 2018-09-22
	if(pRootRod==NULL||!pRootRod->IsTube())
		return FALSE;
	CLDSLineTube *pLineTube=(CLDSLineTube*)pRootRod;
	CLDSParamPlate *pStartPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
	CLDSParamPlate *pEndPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
	if(pStartPlate==NULL&&pEndPlate==NULL)
		return FALSE;
	if(!((pStartPlate&&pStartPlate->m_iParamType==TYPE_ROLLEND)||(pEndPlate&&pEndPlate->m_iParamType==TYPE_ROLLEND)))
		return FALSE;
	CLDSParamPlate *pVerifyPlate=(pStartPlate&&pStartPlate->m_iParamType==TYPE_ROLLEND)?pStartPlate:pEndPlate;
	CSectionView *pMainView=NULL,*pFrontView=NULL;
	double max_dd=0;
	for(CSectionView *pView=SonViews.GetFirst();pView;pView=SonViews.GetNext())
	{
		if(pView->m_iViewType==VIEW_MAIN)
			pMainView=pView;
		else if(pView->m_iViewType==VIEW_FRONT)
		{
			double dd=fabs(pView->ucs.axis_z*pVerifyPlate->ucs.axis_y);
			if(pFrontView==NULL||dd>max_dd)
			{
				pFrontView=pView;
				max_dd=dd;
			}
		}	
	}
	if(pMainView==NULL)
		return FALSE;
	double dd=pMainView->ucs.axis_z*pVerifyPlate->ucs.axis_z;
	BYTE cDatumPlane=pMainView->des_cs.GetParamInfo().m_eDatumPlane;
	BOOL bDatumPlaneXZ=(cDatumPlane==CParaCS::DP_XZ||cDatumPlane==CParaCS::DP_ZX);
	//1.调整主视图坐标系保证主视图中槽型插板朝外
	if(bDatumPlaneXZ&&fabs(dd)>EPS_COS2)
	{
		if(dd>0)
		{	//翻转Z轴方向
			CParaCS::PARAMETRIC_INFO param=pMainView->des_cs.GetParamInfo();
			param.desAxisZ.ReverseDirection();
			pMainView->des_cs.SetParamInfo(param);
			pMainView->UpdateUCS(BelongModel(),this);
		}
	}
	else 
	{	//重新定义坐标系
		CParaCS::PARAMETRIC_INFO param=pMainView->des_cs.GetParamInfo();
		if(!bDatumPlaneXZ)
		{
			param.m_eDatumPlane=CParaCS::DP_ZX;
			param.desAxisX.norm_style=5;
			param.desAxisX.direction=0;
			param.desAxisX.hVicePart=pRootRod->handle;
		}
		param.desAxisZ.norm_style=3;	//构件工作法线
		param.desAxisZ.hVicePart=pVerifyPlate->handle;
		param.desAxisZ.direction=1;	//朝内
		pMainView->des_cs.SetParamInfo(param);
		pMainView->UpdateUCS(BelongModel(),this);
	}
	//2.调整正视图坐标系，保证正视图为主视图的仰视图
	if(pFrontView==NULL)
	{	//只有一个主视图时，如果存在脚钉需要添加插板仰视图 wht 18-09-27
		int nNailCount=pLineTube->GetFootNailCount();
		if(nNailCount>0)
		{
			pFrontView = SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
			pFrontView->m_iViewType=VIEW_FRONT;
			strcpy(pFrontView->sName,"仰视图");
			CParaCS::PARAMETRIC_INFO param=pMainView->des_cs.GetParamInfo();
			param.m_eDatumPlane=CParaCS::DP_XY;	//设置为非XZ模式，共用下面更新坐标系的代码设置坐标系 wht 18-09-27
			pFrontView->des_cs.SetParamInfo(param);
			pFrontView->AddPart(pRootRod->handle,pRootRod);
			for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
				pFrontView->AddPart(pPart->handle,pPart);
		}
	}
	if(pFrontView)
	{
		cDatumPlane=pFrontView->des_cs.GetParamInfo().m_eDatumPlane;
		bDatumPlaneXZ=(cDatumPlane==CParaCS::DP_XZ||cDatumPlane==CParaCS::DP_ZX);
		dd=pFrontView->ucs.axis_z*pVerifyPlate->ucs.axis_y;
		if(bDatumPlaneXZ&&fabs(dd)>EPS_COS2)
		{
			if(dd<0)
			{
				CParaCS::PARAMETRIC_INFO param=pMainView->des_cs.GetParamInfo();
				param.desAxisZ.ReverseDirection();
				pMainView->des_cs.SetParamInfo(param);
				pMainView->UpdateUCS(BelongModel(),this);
			}
		}
		else
		{	//重新定义坐标系
			CParaCS::PARAMETRIC_INFO param=pFrontView->des_cs.GetParamInfo();
			if(!bDatumPlaneXZ)
			{
				param.m_eDatumPlane=CParaCS::DP_ZX;
				param.desAxisX.norm_style=5;
				param.desAxisX.direction=0;
				param.desAxisX.hVicePart=pRootRod->handle;
			}
			param.desAxisZ.norm_style=4;	//构件上的相对方向
			param.desAxisZ.hVicePart=pVerifyPlate->handle;
			if(pVerifyPlate->ucs.axis_y*pMainView->ucs.axis_y>0)
				param.desAxisZ.nearVector.Set(0,-1,0);
			else
				param.desAxisZ.nearVector.Set(0,1,0);
			pFrontView->des_cs.SetParamInfo(param);
			pFrontView->UpdateUCS(BelongModel(),this);
		}
	}
	return TRUE;
}
BOOL CLDSPartGroup::AddCommonRodPartGroupViews(CLDSLinePart *pRootLinePart,PARTSET &partset)
{
	if(pRootLinePart==NULL||pRootLinePart->handle!=handle)
		return FALSE;
	GEPOINT lenRootStdVec=pRootLinePart->End()-pRootLinePart->Start();
	normalize(lenRootStdVec);	//单位化杆件长度方向的延伸轴线方向
	if(pRootLinePart->GetClassTypeId()==CLS_LINEANGLE)
		((CLDSLineAngle*)pRootLinePart)->getUCS(pRootLinePart->ucs);
	else if(pRootLinePart->GetClassTypeId()==CLS_LINEFLAT)
		((CLDSLineFlat*)pRootLinePart)->BuildUCS();
	else if(pRootLinePart->GetClassTypeId()==CLS_LINESLOT)
		((CLDSLineSlot*)pRootLinePart)->BuildUCS();
	else if(pRootLinePart->GetClassTypeId()==CLS_LINETUBE)
		((CLDSLineTube*)pRootLinePart)->BuildUCS();

	//添加普通组焊件的默认视图
	//1、提取组焊图工作面
	CPartGroupWorkPlane *pWorkPlane=NULL;
	CHashListEx<CPartGroupWorkPlane>workplanelist;
	ATOM_LIST<DIFF_PLANE_ERROR_LOG> listErrors;
	//1.提取区间段
	for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{	//应该去掉部分不需要的正视图 主要是由与钢管不垂直也不平行的一些钢板所形成的一些正视图
		//如某些跨越钢管连板，环向板，环向肋板，异性法兰等
		CPartGroupWorkPlane workplane;
		workplane.csParaInfo.desOrg.datum_pos_style=1;	//杆件端点为基准
		workplane.csParaInfo.desOrg.des_para.hPart=pRootLinePart->handle;
		workplane.csParaInfo.desOrg.des_para.RODEND.bIncOddEffect=TRUE;
		workplane.csParaInfo.desOrg.des_para.RODEND.direction=0;
		workplane.csParaInfo.desAxisX.norm_style=5;
		workplane.csParaInfo.desAxisX.hVicePart=pRootLinePart->handle;
		if((fabs(lenRootStdVec.y)+fabs(lenRootStdVec.z)<0.1)&&lenRootStdVec.x<0)
			workplane.csParaInfo.desAxisX.direction=1;	//接近平行于X轴时，按自左至右X轴正向顺序布置视图
		else
			workplane.csParaInfo.desAxisX.direction=0;
		if(pPart->m_hPartWeldParent!=pRootLinePart->handle)
			continue;	//只处理直接焊接于钢管上的构件
		if(pPart->IsPlate())
		{	//提取由钢管沿向钢板展开面形成的视图
			CSuperSmartPtr<CLDSGeneralPlate>pPlate=(CLDSGeneralPlate*)pPart;;
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&
				(((CLDSParamPlate*)pPart)->m_iParamType==TYPE_ELBOWPLATE||
				((CLDSParamPlate*)pPart)->m_iParamType==TYPE_RIBPLATE))
				continue;	//法兰肘板不单独生成组焊子视图
			
			if(!pPlate->IsConnectPlate())
				continue;	//非连接板不参预组焊子视图的提取，否则可能会遇到一些钢管竖向肋板影响正视图的生成 wjh-2016.10.9
			if(pPart->GetClassTypeId()==CLS_PLATE&&pPart->m_hPartWeldParent>0)
			{
				SmartPartPtr pDatumPlate=BelongModel()->FromPartHandle(pPart->m_hPartWeldParent);
				if(pDatumPlate.IsHasPtr()&&pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE&&
					pDatumPlate.pParamPlate->m_iParamType==TYPE_XEND)
					continue;	//十字插板的焊接立板不参预工作平面的筛查
			}
			/*if((pPlate->IsTubleFoldPlate()||pPlate->IsSplayFoldPlate())||pRootLinePart->GetClassTypeId()==CLS_LINETUBE)
			{
				if(pPlate.pPlate->designInfo.m_hBasePart!=pRootLinePart->handle)
					continue;

			}
			else */if(fabs(pPart->ucs.axis_z*lenRootStdVec)>EPS)
			{
				if(pRootLinePart->GetClassTypeId()==CLS_LINETUBE)
					continue;	//跳过非钢管沿向钢板（即保留钢管沿向钢板）
				else if(pRootLinePart->IsAngle()||pRootLinePart->GetClassTypeId()==CLS_LINESLOT)
					workplane.csParaInfo.desAxisX.direction=3;	//角钢-norm_x\槽钢肢法线方向(X轴方向)- wht 15-09-18
				//else if(pRootLinePart->GetClassTypeId()==CLS_LINEFLAT)
			}
			workplane.csParaInfo.desOrg.datum_pos_style=1;	//杆件端点为基准
			workplane.csParaInfo.desOrg.des_para.hPart=pRootLinePart->handle;
			workplane.csParaInfo.desOrg.des_para.RODEND.bIncOddEffect=TRUE;
			workplane.csParaInfo.desOrg.des_para.RODEND.direction=0;
			workplane.csParaInfo.m_eDatumPlane=CParaCS::DP_XZ;
			workplane.csParaInfo.desAxisZ.norm_style=3;	//构件工作法线
			workplane.csParaInfo.desAxisZ.hVicePart=pPart->handle;
			f3dPoint verfiy_norm=AdjustPlateWorkPlaneNorm(pPart->ucs.axis_z,pPart->layer(2));
			if(pPart->ucs.axis_z*verfiy_norm<0)
			{
				workplane.csParaInfo.desAxisZ.direction=1;	//朝内
				workplane.workPlaneNorm =-pPart->ucs.axis_z;
			}
			else
			{
				workplane.csParaInfo.desAxisZ.direction=0;	//朝外
				workplane.workPlaneNorm = pPart->ucs.axis_z;
			}
		}
		else if(pPart->GetClassTypeId()==CLS_LINETUBE)	
		{	//提取由其它焊接支管形成的交叉面视图
			CLDSLineTube *pBranchTube=(CLDSLineTube*)pPart;
			f3dPoint branch_tube_line_vec=pBranchTube->End()-pBranchTube->Start();
			workplane.workPlaneNorm=branch_tube_line_vec^lenRootStdVec;
			normalize(workplane.workPlaneNorm);
			workplane.csParaInfo.m_eDatumPlane=CParaCS::DP_XZ;
			workplane.csParaInfo.desAxisZ.norm_style=2;	//杆件交叉面法线
			workplane.csParaInfo.desAxisZ.hVicePart=pRootLinePart->handle;
			workplane.csParaInfo.desAxisZ.hCrossPart=pBranchTube->handle;
			if(pBranchTube->layer(2)=='Q')
				workplane.csParaInfo.desAxisZ.nearVector.Set(0, 1,0);
			else if(pBranchTube->layer(2)=='H')
				workplane.csParaInfo.desAxisZ.nearVector.Set(0,-1,0);
			else if(pBranchTube->layer(2)=='Z')
				workplane.csParaInfo.desAxisZ.nearVector.Set(-1,0,0);
			else if(pBranchTube->layer(2)=='Y')
				workplane.csParaInfo.desAxisZ.nearVector.Set( 1,0,0);
		}
		else if(pPart->IsLinePart()&&((CLDSLinePart*)pPart)->pStart!=NULL&&((CLDSLinePart*)pPart)->pEnd!=NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Now don't support weld branch rod which is no tube style 0X%X！",pPart->handle);
#else
			logerr.Log("目前不支持非钢管形式的焊接分支杆件0X%X！",pPart->handle);
#endif
			continue;
		}
		else	//不支持该类型构件的工作面检测
			continue;

		for(pWorkPlane=workplanelist.GetFirst();pWorkPlane;pWorkPlane=workplanelist.GetNext())
		{	//法线方向接近的工作平面没必要重复添加，现将误差范围调整为EPS_COS2
			double dd=fabs(pWorkPlane->workPlaneNorm*workplane.workPlaneNorm);
			//if(dd<0.999847695)//COS(1°)=0.999847695, 不严格执行共面是为了避免分支杆件工作面与板面有时不严格共面(但在工程误差范围内)的情况。
			if(dd<0.99619469)	//COS(5°)=0.99619469809174553229501040247389
				continue;
			if(dd<EPS_COS)
			{
				double diff=ACOS(dd)*DEGTORAD_COEF;
				if(fabs(diff)>0.2)
				{	//通过经验判断两板平面夹角超出0.2°，输出警示信息比较恰当 wjh-2016.6.19
					DIFF_PLANE_ERROR_LOG* pError=listErrors.append();
					pError->iPlaneNo=pWorkPlane->iNo;
					char sWorkNormInfo[100];
					if(pPart->IsPlate())
	#ifdef AFX_TARG_ENU_ENGLISH
						sprintf(sWorkNormInfo,"0X%X plate's working plane normal",pPart->handle);
					else
						sprintf(sWorkNormInfo,"The weld working plane formed by 0X%X part and datum rod 0X%X",pPart->handle,pRootLinePart->handle);
	#else
						sprintf(sWorkNormInfo,"0X%X钢板的工作面法线",pPart->handle);
					else
						sprintf(sWorkNormInfo,"由0X%X构件与基准杆件0X%X形成的组焊工作平面",pPart->handle,pRootLinePart->handle);
	#endif
					pError->error.Copy(sWorkNormInfo);
					pError->diff=diff;
					BYTE warninglevel=CLogFile::WARNING_LEVEL3_UNIMPORTANT;
					if(fabs(diff)>0.5)
						warninglevel=CLogFile::WARNING_LEVEL1_IMPORTANT;
					else if(fabs(diff)>0.35)
						warninglevel=CLogFile::WARNING_LEVEL2_COMMON;
					CXhChar50 sxPlaneName=pWorkPlane->sName;
					if(sxPlaneName.GetLength()==0)
						sxPlaneName.Copy("主视图");
					logerr.LevelLog(warninglevel,"%s组焊件'%s'中，%s与其归属工作平面不严格共面{%.1f°}!",sRootPartNo,(char*)sxPlaneName,sWorkNormInfo,diff);
				}
			}
			break;	//已存在此工作面法线方向
		}
		if(pWorkPlane==NULL)	//新的工作面法线方向
		{
			workplane.workPlaneOrg=pRootLinePart->Start();
			workplane.pDatumPart=pPart;
			//尽量保证描述前面构件或后面构件的视图作为主视图 wht 10-08-15
			/*CPartGroupWorkPlane *pFirstWorkPlane=workplanelist.GetFirst();
			char cFirstType,cCurType;
			if(pFirstWorkPlane&&pFirstWorkPlane->pDatumPart)
				cFirstType=toupper(pFirstWorkPlane->pDatumPart->layer(2));
			cCurType=toupper(pPart->layer(2));
			if(workplanelist.GetNodeNum()==0)
				strcpy(workplane.sName,"主视图");
			else
				sprintf(workplane.sName,"正视图%d",workplanelist.GetNodeNum());
			if((cFirstType!='Q'&&cCurType=='Q')||(cFirstType!='Q'&&cFirstType!='H'&&cCurType=='H'))
				workplanelist.insert(workplane);
			else
				workplanelist.append(workplane);*/
			pWorkPlane=workplanelist.SetValue(0,workplane,true);
			//pWorkPlane->iNo=workplanelist.GetNodeNum();
		}
	}
	//2、对近似共面的工作面进行展开处理
	GEPOINT lenRootRodVec=pRootLinePart->End()-pRootLinePart->Start();
	normalize(lenRootRodVec);
	double straightness=0;
	CLDSLinePart* pJointRod=NULL;
	CHashSet<CLDSLinePart*>hashBranchRods;
	for(SmartPartPtr pSmartPart=partset.GetFirst();pSmartPart.IsHasPtr();pSmartPart=partset.GetNext())
	{
		if(!pSmartPart->IsLinePart()||pSmartPart.IsEqualPtr(pRootLinePart))
			continue;
		if(pSmartPart.pRod->pStart==NULL||pSmartPart.pRod->pEnd==NULL)
			continue;
		hashBranchRods.SetValue(pSmartPart->handle,pSmartPart.pRod);
		GEPOINT lenBranchVec=pSmartPart.pRod->End()-pSmartPart.pRod->Start();
		normalize(lenBranchVec);
		double justify=fabs(lenBranchVec*lenRootRodVec);
		if(justify>0.94&&justify>straightness)	//cos(20°)
		{
			straightness=justify;
			pJointRod=pSmartPart.pRod;
		}
	}
	if(pJointRod)
		hashBranchRods.DeleteNode(pJointRod->handle);
	RetrieveWorkPlateSpreadInfo(pRootLinePart,pJointRod,workplanelist,partset);
	//2.1分析比对类同展开轴面，并删减（如三角塔正侧面轴面图相同，云南景云祥说只需保留一个） wjh-2015.10.12
	for(pWorkPlane=workplanelist.GetFirst();pWorkPlane;pWorkPlane=workplanelist.GetNext())
	{
		if(pWorkPlane->pTempSectionParts==NULL)
			continue;
		for(int i=0;i<pWorkPlane->nPartNum;i++)
		{
			SnapPerp(&pWorkPlane->pTempSectionParts[i].prjLine.startPt,pRootLinePart->Start(),pRootLinePart->End(),
				pWorkPlane->pTempSectionParts[i].prjLine.startPt);
			pWorkPlane->pTempSectionParts[i].prjLine.endPt=pWorkPlane->pTempSectionParts[i].prjLine.startPt;
		}
		CQuickSort<SECTION_PART>::QuickSort(pWorkPlane->pTempSectionParts,pWorkPlane->nPartNum,compare_func2);
	}
	for(pWorkPlane=workplanelist.GetFirst();pWorkPlane;pWorkPlane=workplanelist.GetNext())
	{
		if(pWorkPlane->pTempSectionParts==NULL)
			continue;
		BOOL pushed=workplanelist.push_stack();
		for(CPartGroupWorkPlane* pNextWorkPlane=workplanelist.GetNext();pNextWorkPlane;pNextWorkPlane=workplanelist.GetNext())
		{
			if(pWorkPlane->nPartNum!=pNextWorkPlane->nPartNum)
				continue;
			for(int i=0;i<pWorkPlane->nPartNum;i++)
			{
				if(stricmp(pWorkPlane->pTempSectionParts[i].pPart->GetPartNo(),pNextWorkPlane->pTempSectionParts[i].pPart->GetPartNo())!=0)
					break;
				if(fabs(pWorkPlane->pTempSectionParts[i].prjLine.startPt.x-pNextWorkPlane->pTempSectionParts[i].prjLine.startPt.x)>EPS2)
					break;
			}
			if(i==pWorkPlane->nPartNum)	//两张视图完全相同
			{
				delete []pNextWorkPlane->pTempSectionParts;
				workplanelist.DeleteCursor();
			}
		}
		if(pWorkPlane->pTempSectionParts)
			delete []pWorkPlane->pTempSectionParts;
		workplanelist.pop_stack(pushed);
	}
	//3、为每个工作面添加一个对应的视图
	f3dPoint main_norm;
	if(pRootLinePart->GetClassTypeId()==CLS_LINETUBE)
	{	//主视图法线一般取基准边所在面（基准面）的法线
		main_norm=pRootLinePart->ucs.axis_z^pRootLinePart->ucs.axis_x;
		normalize(main_norm);
	}
	int view_index=1,perp_view_hits=0;
	CSectionView *pMainView=NULL;	//记录主视图，调整端面视图与主视图一致时使用 wht 12-09-05
	CSectionView *pFirstBasicView=NULL;
	//优先从工作面中查找主视图

	APPROPRIATE_NORMAL selecter;
	CPartGroupWorkPlane* pMainPlane=NULL;
	for(pWorkPlane=workplanelist.GetFirst();pWorkPlane;pWorkPlane=workplanelist.GetNext())
	{
		CParaCS des_cs;
		des_cs.SetParamInfo(pWorkPlane->csParaInfo);
		des_cs.SetModifiedState();
		if(!des_cs.UpdateACS(m_pModel))
			continue;
		if(pMainPlane==NULL)	//先默认第一个有效工作面
			pMainPlane=pWorkPlane;
		pWorkPlane->workPlaneNorm=des_cs.GetACS().axis_z;
		selecter.MeetingCheck(pWorkPlane,main_norm);
	}
	if(selecter.pMainPlane!=NULL)
		pMainPlane=selecter.pMainPlane;
	if(pMainPlane!=NULL)//!main_norm.IsZero()&&fabs(des_cs.GetACS().axis_z*main_norm)>COS2DEG)//EPS_COS)
	{	//如果钢管基准面为工作平面时，尽量使其成为主视图
		int idSectionViewType=pMainPlane->bJointTubePlane?CSectionView::CLS_JOINT_TUBES_VIEW:CSectionView::CLS_BASIC_VIEW;
		if(!pRootLinePart->IsTube()||!pMainPlane->pDatumPart->IsTube())
			idSectionViewType=CSectionView::CLS_BASIC_VIEW;
		pMainView=SonViews.Add(0,idSectionViewType);
		pMainView->index=view_index++;	//视图索引
		CParaCS des_cs;
		des_cs.SetParamInfo(pMainPlane->csParaInfo);
		des_cs.UpdateACS(m_pModel);
		pMainView->ucs=des_cs.GetACS();
		pMainView->m_iViewType=VIEW_MAIN;
		pFirstBasicView=pMainView;
		pMainPlane->iMatchView=pMainView->index;	//记录工作面匹配的视图索引
	}
	//根据工作面添加其余视图，并初始化视图参数
	const double cosa1=0.99985;
	for(pWorkPlane=workplanelist.GetFirst();pWorkPlane;pWorkPlane=workplanelist.GetNext())
	{	//基准构件为钢管时不生成包含组焊件所有构件的端面图，构件太多标注会比较乱且无实际意义。 wht 09-11-24
		
		int idSectionViewType=pWorkPlane->bJointTubePlane?CSectionView::CLS_JOINT_TUBES_VIEW:CSectionView::CLS_BASIC_VIEW;
		if(!pRootLinePart->IsTube()||!pWorkPlane->pDatumPart->IsTube())
			idSectionViewType=CSectionView::CLS_BASIC_VIEW;
		CSectionView *pSectionView = SonViews.GetValue(pWorkPlane->iMatchView);
		if(pSectionView==NULL)
		{	
			pSectionView = SonViews.Add(0,idSectionViewType);
			pSectionView->index=view_index++;	//视图索引
			pWorkPlane->iMatchView=pSectionView->index;
		}
		pSectionView->des_cs.SetParamInfo(pWorkPlane->csParaInfo);
		if(idSectionViewType==CSectionView::CLS_JOINT_TUBES_VIEW)
		{
			CJointTubesView* pJointView=(CJointTubesView*)pSectionView;
			pJointView->m_hRootTube=pRootLinePart->handle;
			pJointView->m_hJointTube=pWorkPlane->pDatumPart->handle;
			pJointView->m_ciJointsViewType=0;	//0:展示对接钢管夹角的俯视图
			pJointView->UpdateJointViewType();	//更新m_ciJointsViewType,
		}
		if(pFirstBasicView==NULL)
			pFirstBasicView=pSectionView;
		//杆件垂向投影视图
		BOOL bViewCSUpdated;
		pSectionView->des_cs.SetModifiedState();
		if(bViewCSUpdated=pSectionView->des_cs.UpdateACS(m_pModel))
		{
			pSectionView->ucs=pSectionView->des_cs.GetACS();
			if(view_index>2&&(pSectionView->ucs.axis_z*pFirstBasicView->ucs.axis_y)<-EPS)
			{	//尽量使另一视图作为主视图的俯视图
				pWorkPlane->csParaInfo.desAxisZ.ReverseDirection();
				pSectionView->des_cs.SetParamInfo(pWorkPlane->csParaInfo);
				pSectionView->UpdateUCS(m_pModel,this);
			}
		}
		if(pMainView==NULL && (!bViewCSUpdated||	//视图坐标系更新失败
			(!main_norm.IsZero()&&fabs(pSectionView->ucs.axis_z*main_norm)>EPS_COS)))
		{	//如果钢管基准面为工作平面时，尽量使其成为主视图　wjh-2017.11.8
			pSectionView->m_iViewType=VIEW_MAIN;	
			pMainView=pSectionView;	//记录主视图
		}
		else if((pMainView==NULL||(pMainView!=pSectionView))&&idSectionViewType!=CSectionView::CLS_JOINT_TUBES_VIEW)
		{	//为了在之前防错报警中准确提示视图名称，故提前设定名称
			pSectionView->m_iViewType=VIEW_FRONT;
			perp_view_hits++;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		if(pSectionView->m_iViewType==VIEW_MAIN)
			strcpy(pSectionView->sName,"Main View");
		else if(idSectionViewType==CSectionView::CLS_JOINT_TUBES_VIEW)
			strcpy(pSectionView->sName,"Joint Tube View");
		else
		{
			strcpy(pSectionView->sName,"View");
			if(perp_view_hits>1)
				sprintf(pSectionView->sName,"Front View %d",perp_view_hits);
		}
#else
		if(pSectionView->m_iViewType==VIEW_MAIN)
			strcpy(pSectionView->sName,"主视图");
		else if(idSectionViewType==CSectionView::CLS_JOINT_TUBES_VIEW)
			strcpy(pSectionView->sName,"对接钢管简图");
		else
		{
			strcpy(pSectionView->sName,"正视图");
			if(perp_view_hits>1)
				sprintf(pSectionView->sName,"正视图%d",perp_view_hits);
		}
#endif
		if(!bViewCSUpdated)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("【%s】 group weld part 【%s】 view's UCS wrong calculates！",sGroupName,pSectionView->sName);
#else
			logerr.Log("【%s】组焊件【%s】视图坐标系计算错误！",sGroupName,pSectionView->sName);
#endif
		pWorkPlane->sName.Copy(pSectionView->sName);
		//
		pSectionView->AddPart(pRootLinePart->handle,pRootLinePart);
		if(pSectionView->m_idViewType==CSectionView::CLS_JOINT_TUBES_VIEW)
		{
			CJointTubesView* pJointView=(CJointTubesView*)pSectionView;
			pSectionView->AddPart(pJointView->m_hRootTube);
			pSectionView->AddPart(pJointView->m_hJointTube);
		}
		else if(pWorkPlane->assistant.m_xFaceNorm.vector.IsZero())
		{
			for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			{
				if(pPart==pRootLinePart)
					continue;
				else if(pPart==pJointRod)
				{
					pSectionView->AddPart(pJointRod->handle);
					continue;
				}
				pSectionView->AddPart(pPart->handle,pPart);
			}
			for(CLDSLinePart* pBranchRod=hashBranchRods.GetFirst();pBranchRod;pBranchRod=hashBranchRods.GetNext())
			{	//移除竖立绘制杆件上的附属构件
				if(pBranchRod->handle==pWorkPlane->pDatumPart->handle)
					continue;
				f3dPoint vec=pBranchRod->End()-pBranchRod->Start();
				normalize(vec);
				if(fabs(vec*pSectionView->ucs.axis_z)<EPS2)
					continue;
				PARTSET discardparts;
				BelongModel()->GetPartGroupPartSet(pBranchRod->handle,discardparts,0,true,pJointRod!=NULL?&pJointRod->handle:NULL);	//平基准钢管上的焊接子构件集合
				for(CLDSPart* pDiscardPart=discardparts.GetFirst();pDiscardPart;pDiscardPart=discardparts.GetNext())
					pSectionView->SonParts.DeleteNode(pDiscardPart->handle);
			}
		}
		else
		{
			//添加视图展开面及构件
			CDrawingAssistant* pAssist=pSectionView->hashAssistPlanes.Add(0);
			pAssist->m_nStyle=pWorkPlane->assistant.m_nStyle;
			pAssist->m_xLineStart=pWorkPlane->assistant.m_xLineStart;
			pAssist->m_xFaceNorm=pWorkPlane->assistant.m_xFaceNorm;
			pAssist->bSpreadOverRidge=pWorkPlane->assistant.bSpreadOverRidge;
			pAssist->iResidePlaneNo=pWorkPlane->assistant.iResidePlaneNo;
			pAssist->ciSpreadDirection=pWorkPlane->assistant.ciSpreadDirection;
			strcpy(pAssist->name,pWorkPlane->assistant.name);
			pAssist->m_xFaceNorm.UpdateVector(m_pModel);
			f3dPoint spreadNorm=pAssist->m_xFaceNorm.vector;
			CSectionView::PART_ITEM* pItem=NULL;
			for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			{
				if(pPart==pJointRod)
				{
					pItem=pSectionView->AddPart(pJointRod->handle);
					continue;
				}
				double fCosMain=0,fCosSpread=0;
				if(pPart->IsLinePart())
				{
					CLDSLinePart* pLinePart=(CLDSLinePart*)pPart;
					f3dPoint vec=pLinePart->End()-pLinePart->Start();
					normalize(vec);
					fCosSpread=fabs(vec*spreadNorm);
					fCosMain=fabs(vec*pSectionView->ucs.axis_z);
								
				}
				else if(pPart->IsPlate())
				{
					CLDSGeneralPlate* pBasicPlate=(CLDSGeneralPlate*)pPart;
					GEPOINT vPlateNormal=pPart->ucs.axis_z;
					if(pBasicPlate->IsTubeFoldPlate())
					{	//变电构架三角梁的底平面曲梁钢管上折叠板，两制弯面为工作连接面（基面仅是过渡面）
						vPlateNormal=pBasicPlate->GetHuoquFaceNorm(0)+pBasicPlate->GetHuoquFaceNorm(1);
						normalize(vPlateNormal);
					}
					else if(pBasicPlate->IsSplayFoldPlate())
					{	//变电构架三角梁顶梁八字板，取基面法线向钢管截面的投影为钢板名义工作面法线 wjh-2017.11.8
						vPlateNormal-=(vPlateNormal*lenRootStdVec)*lenRootStdVec;
						normalize(vPlateNormal);
					}
					fCosSpread=fabs(vPlateNormal*spreadNorm);
					fCosMain=fabs(vPlateNormal*pSectionView->ucs.axis_z);
				}
				//构件与展开面或基准面都接近垂直时，优先归属于基准面 wht 18-03-19
				double cos70=0.34202014332566873304409961468226;
				if(fCosSpread<fCosMain||fCosSpread<cos70||fCosMain<cos70)
				{
					if(fCosSpread<EPS2&&fCosSpread<(1-fCosMain)&&fCosMain>cos70)
					{	//钢板与基准面、展开面都接近时，应优先归属基准面
						pItem=pSectionView->AddPart(pPart->handle);
						pItem->iResidePlaneNo=pAssist->iNo;
						pItem->ciSpreadDirection=pAssist->ciSpreadDirection;
						pItem->cFlag='X';	//杆件正视图/板的截面图
					}
					else if(fCosMain>EPS_COS2||fCosSpread<cos70||fCosMain<cos70)
					{
						pItem=pSectionView->AddPart(pPart->handle);
						pItem->cFlag='Z';	//杆件截面图/板的正视图
						if(hashBranchRods.GetValue(pPart->handle))
						{	//移除竖立绘制杆件上的附属构件
							PARTSET discardparts;
							BelongModel()->GetPartGroupPartSet(pPart->handle,discardparts,0,true,pJointRod!=NULL?&pJointRod->handle:NULL);	//平基准钢管上的焊接子构件集合
							for(CLDSPart* pDiscardPart=discardparts.GetFirst();pDiscardPart;pDiscardPart=discardparts.GetNext())
								pSectionView->SonParts.DeleteNode(pDiscardPart->handle);
						}
					}
				}
				else if(fCosMain<fCosSpread)
				{
					if(fCosMain<EPS2&&fCosMain<(1-fCosSpread))
					{
						pItem=pSectionView->AddPart(pPart->handle);
						pItem->cFlag='X';	//杆件正视图/板的截面图
					}
					else if(fCosSpread>EPS_COS2)
					{
						pItem=pSectionView->AddPart(pPart->handle);
						pItem->iResidePlaneNo=pAssist->iNo;
						pItem->ciSpreadDirection=pAssist->ciSpreadDirection;
						pItem->cFlag='Z';	//杆件截面图/板的正视图
						if(hashBranchRods.GetValue(pPart->handle))
						{	//移除竖立绘制杆件上的附属构件
							PARTSET discardparts;
							BelongModel()->GetPartGroupPartSet(pPart->handle,discardparts,0,true,pJointRod!=NULL?&pJointRod->handle:NULL);	//平基准钢管上的焊接子构件集合
							for(CLDSPart* pDiscardPart=discardparts.GetFirst();pDiscardPart;pDiscardPart=discardparts.GetNext())
								pSectionView->SonParts.DeleteNode(pDiscardPart->handle);
						}
					}
				}		
			}
		}
		SmartPartPtr pEndPart;
		PARTSET endparts;
		CLDSLineTube* pRootTube=(CLDSLineTube*)pRootLinePart;
		CLDSLineTube* pJointTube=(CLDSLineTube*)pJointRod;
		if(pRootTube->m_tJointStart.hLinkObj>0x20)
		{
			pEndPart=BelongModel()->FromPartHandle(pRootTube->m_tJointStart.hLinkObj);
			if(pEndPart.IsHasPtr())
			{
				pSectionView->AddPart(pRootTube->m_tJointStart.hLinkObj,pEndPart);
				if(pEndPart->IsPlate())
					endparts.append(pEndPart);
			}
		}
		if(pRootTube->m_tJointEnd.hLinkObj>0x20)
		{
			pEndPart=BelongModel()->FromPartHandle(pRootTube->m_tJointEnd.hLinkObj);
			if(pEndPart.IsHasPtr())
			{
				pSectionView->AddPart(pRootTube->m_tJointEnd.hLinkObj);
				if(pEndPart->IsPlate())
					endparts.append(pEndPart);
			}
		}
		if(pJointTube&&pJointTube->m_tJointStart.hLinkObj>0x20)
		{
			pEndPart=BelongModel()->FromPartHandle(pJointTube->m_tJointStart.hLinkObj);
			if(pEndPart.IsHasPtr())
			{
				pSectionView->AddPart(pJointTube->m_tJointStart.hLinkObj);
				if(pEndPart->GetClassTypeId()==CLS_PARAMPLATE)
					endparts.append(pEndPart);
			}
		}
		if(pJointTube&&pJointTube->m_tJointEnd.hLinkObj>0x20)
		{
			pEndPart=BelongModel()->FromPartHandle(pJointTube->m_tJointEnd.hLinkObj);
			if(pEndPart.IsHasPtr())
			{
				pSectionView->AddPart(pJointTube->m_tJointEnd.hLinkObj);
				if(pEndPart->IsPlate())
					endparts.append(pEndPart);
			}
		}
		for(pEndPart=endparts.GetFirst();pEndPart.IsHasPtr();pEndPart=endparts.GetNext())
		{
			if(pEndPart->GetClassTypeId()!=CLS_PARAMPLATE)
				continue;
			for(ATTACHPART_ITEM* pAttachItem=pEndPart.pParamPlate->GetFirstAttachPart();pAttachItem;
				pAttachItem=pEndPart.pParamPlate->GetNextAttachPart())
			{
				CLDSPart* pSubPart=BelongModel()->FromPartHandle(pAttachItem->h);
				if(pSubPart==NULL||pRootLinePart==NULL||(pEndPart.pParamPlate->m_iParamType==TYPE_XEND&&
					(pSubPart->m_hPartWeldParent!=pEndPart->handle&&pSubPart->m_hPartWeldParent!=pRootLinePart->handle)))
					continue;
				pSectionView->AddPart(pAttachItem->h);
			}
		}
	}
	if(pMainView==NULL)
		pMainView=SonViews.GetFirst();
	if(pMainView)
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(pMainView->sName,"Main View");
#else
		sprintf(pMainView->sName,"主视图");
#endif
		pMainView->m_iViewType=VIEW_MAIN;	
	}
	for(DIFF_PLANE_ERROR_LOG *pErrLog=listErrors.GetFirst();pErrLog;pErrLog=listErrors.GetNext())
	{
		for(pWorkPlane=workplanelist.GetFirst();pWorkPlane;pWorkPlane=workplanelist.GetNext())
		{
			if(pErrLog->iPlaneNo==pWorkPlane->iNo)
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("In %s weld part group '%s',%s doesn't strictly coplanar with its reference working plane {%.1f°}!",sRootPartNo,(char*)pWorkPlane->sName,(char*)pErrLog->error,pErrLog->diff);
			else if(pErrLog->iPlaneNo==pWorkPlane->assistant.iNo)
				logerr.Log("In %s weld part group '%s',%s doesn't strictly coplanar with its reference extended working plane {%.1f°}!",sRootPartNo,(char*)pWorkPlane->sName,(char*)pErrLog->error,pErrLog->diff);
#else
				logerr.Log("%s组焊件'%s'中，%s与其归属工作平面不严格共面{%.1f°}!",sRootPartNo,(char*)pWorkPlane->sName,(char*)pErrLog->error,pErrLog->diff);
			else if(pErrLog->iPlaneNo==pWorkPlane->assistant.iNo)
				logerr.Log("%s组焊件'%s'中，%s与其归属的展开工作平面不严格共面{%.1f°}!",sRootPartNo,(char*)pWorkPlane->sName,(char*)pErrLog->error,pErrLog->diff);
#endif
			else
				continue;
			break;
		}
	}
	//3、基准构件为钢管时需要添加钢管端面视图
	if(pRootLinePart->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube *pLineTube=(CLDSLineTube*)pRootLinePart;
		pLineTube->BuildUCS();
		if(SonViews.GetNodeNum()<=0)
		{	//3.1 无主视图(如钢管两端均为法兰且钢管上无连接板的情况)
			CSectionView *pSectionView = SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
			pSectionView->index=view_index++;	//视图索引
			//
			CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
			//基准点
			param.desOrg.datum_pos_style=1;
			param.desOrg.des_para.RODEND.hRod=pLineTube->handle;
			param.desOrg.des_para.RODEND.bIncOddEffect=TRUE;
			param.desOrg.des_para.RODEND.direction=0;
			//坐标轴
			param.m_eDatumPlane=CParaCS::DP_XY;
			param.desAxisX.norm_style=5;
			param.desAxisX.hVicePart=pLineTube->handle;
			param.desAxisX.direction=0;
		
			param.desAxisY.norm_style=4;
			param.desAxisY.hVicePart=pLineTube->handle;
			param.desAxisY.nearVector.Set(1,0,0);
			pSectionView->des_cs.SetParamInfo(param);
			//
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pSectionView->sName,"Main View");
#else
			sprintf(pSectionView->sName,"主视图");
#endif
			//必须置于pSectionView->sName赋值之后，否则出错时警示信息无法正常显示视图名称 wjh-2016.8.7
			pSectionView->UpdateUCS(m_pModel,this);
			pSectionView->m_iViewType=VIEW_MAIN;
			pMainView=pSectionView;	//记录主视图

			pSectionView->AddPart(pRootLinePart->handle,pRootLinePart);
			for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
				pSectionView->AddPart(pPart->handle,pPart);
		}
		//修正主视图、正视图摆放方式
		AdjustFrontViewWorkNormByInsertPlate(pLineTube,partset);
		//预留始端视图索引位置
		int iStartView=-1;
		CLDSParamPlate *pStartPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		if(pStartPlate)
			iStartView=view_index++;
		// 端头视图提前是出于剖视图符顺序问题 wjh-2016.10.12
		//3.2 添加钢管端头视图(包括法兰视图、插板视图、平面切割视图)
		CLDSLineTube* pJointTube=(pJointRod&&pJointRod->IsTube())?(CLDSLineTube*)pJointRod:NULL;
		PARTSET partset2;
		if(pJointTube)
		{
			partset.Empty();
			BelongModel()->GetPartGroupPartSet(pLineTube->handle,partset ,0,true,&pJointRod->handle);
			BelongModel()->GetPartGroupPartSet(pJointRod->handle,partset2,0,true,&pLineTube->handle);
		}
		AddLineTubeEndLjView(pLineTube,pMainView,partset,iStartView,view_index);
		//3.3 添加钢管中间节点对应的端面视图
		AddLineTubeMiddleNodeEndView(pLineTube,pMainView,partset,view_index);
		//3.4 为组焊件中脚钉底座添加端面图
		AddFootNailView(pLineTube,pMainView,partset,view_index);
		if(pJointTube)
		{
			AddLineTubeEndLjView(pJointTube,pMainView,partset2,view_index,view_index);
			//3.3 添加钢管中间节点对应的端面视图
			AddLineTubeMiddleNodeEndView(pJointTube,pMainView,partset2,view_index);
			//3.4 为组焊件中脚钉底座添加端面图
			AddFootNailView(pJointTube,pMainView,partset2,view_index);
		}
		//3.5 添加钢管抱杆视图
		AddTubeBushingPlateView(pLineTube,partset);
		//3.5 支管端面图
		//TODO: 干什么用的？
		/*
		TUBESET tubeSet;	//支管集合
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(pPart->GetClassTypeId()==CLS_LINETUBE&&pPart->handle!=pRootLinePart->handle)
				tubeSet.append((CLDSLineTube*)pPart);
		}
		if(tubeSet.GetNodeNum()>0)
		{
			hashViewByWeldParentHandle.Empty();
			hashViewByWeldParentHandle.CreateHashTable(tubeSet.GetNodeNum());
			CLDSLineTube *pBranchTube=NULL;
			f3dLine datum_line(pLineTube->pStart->Position(true),pLineTube->pEnd->Position(true));
			for(pBranchTube=tubeSet.GetFirst();pBranchTube;pBranchTube=tubeSet.GetNext())
			{
				CLDSNode *pBaseNode=NULL;
				if(datum_line.PtInLine(pBranchTube->pStart->Position(true))>0)
					pBaseNode=pBranchTube->pStart;
				else if(datum_line.PtInLine(pBranchTube->pEnd->Position(true))>0)
					pBaseNode=pBranchTube->pEnd;
				else
					continue;
				if(pBaseNode==NULL)
					continue;
				CSectionView *pSectionView=NULL;
				if(hashViewByWeldParentHandle.GetValueAt(pBaseNode->handle,pSectionView))
				{
					CSectionView::PART_ITEM *pPartItem=pSectionView->SonParts.append();
					pPartItem->hSonPart = pBranchTube->handle;
					pPartItem->pCurPart = pBranchTube;
					pPartItem->cFlag = 0;
				}	
				else
				{
					CSectionView *pSectionView = SonViews.append();
					pSectionView->m_bLocked=TRUE;
					pSectionView->index=view_index;
					view_index++;
					pSectionView->m_iViewType=VIEW_BRACHTUBE;
					strcpy(pSectionView->sName,"支管视图");
					CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
					//基准点
					param.desOrg.datum_pos_style=2;	//杆件上节点为基准
					param.desOrg.des_para.RODNODE.hNode=pBaseNode->handle;
					param.desOrg.des_para.RODNODE.hRod=pRootLinePart->handle;
					//坐标轴
					param.m_eDatumPlane=CParaCS::DP_ZX;
					param.desAxisZ.norm_style=5;
					param.desAxisZ.hVicePart=pLineTube->handle;
					param.desAxisZ.direction=1;
					//调整端面图与主正视图匹配
					if(pMainView&&pMainView->des_cs.IsDesAxisZ())
						param.desAxisX=pMainView->des_cs.GetParamInfo().desAxisZ;
					else
					{
						param.desAxisX.norm_style=4;
						param.desAxisX.hVicePart=pLineTube->handle;
						param.desAxisX.nearVector.Set(1,0,0);
					}
					pSectionView->des_cs.SetParamInfo(param);
					pSectionView->UpdateUCS(m_pModel,this);
					
					pSectionView->AddPart(pRootLinePart->handle,pRootLinePart);
					pSectionView->AddPart(pBranchTube->handle,pBranchTube);
					hashViewByWeldParentHandle.SetValueAt(pBaseNode->handle,pSectionView);
				}
			}
		}*/
		//for(CSectionView *pSectionView=SonViews.GetFirst();pSectionView;pSectionView=SonViews.GetNext())
		//	pSectionView->AddDefaultAngulars(pRootLinePart,&Ta);//添加默认的角度标注线
	}
	return TRUE;
}


//</DEVELOP_PROCESS_MARK>
bool CLDSPartGroup::IdentifyDualTubeJoint(int *horiBeamOrGeneralDualTube)
{
	CLDSLineTube *pDatumTube=NULL,*pDatumTube2=NULL;
	if((pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(handle,CLS_LINETUBE))==NULL)
		return false;
	bool bTubeJointStart=true,bTubeJointEnd=true;
	if(pDatumTube->m_tJointStart.type!=1||pDatumTube->m_tJointStart.hLinkObj<0x20||pDatumTube->m_tJointStart.hViceLinkObj>0x20)
		bTubeJointStart=false;
	if(pDatumTube->m_tJointEnd.type!=1||pDatumTube->m_tJointEnd.hLinkObj<0x20||pDatumTube->m_tJointEnd.hViceLinkObj>0x20)
		bTubeJointEnd=false;
	if(bTubeJointStart==bTubeJointEnd)
		return false;
	if(bTubeJointStart)
		pDatumTube2=(CLDSLineTube*)BelongModel()->FromPartHandle(pDatumTube->m_tJointStart.hLinkObj,CLS_LINETUBE);
	else if(bTubeJointEnd)
		pDatumTube2=(CLDSLineTube*)BelongModel()->FromPartHandle(pDatumTube->m_tJointEnd.hLinkObj,CLS_LINETUBE);
	if(pDatumTube->pStart==NULL||pDatumTube->pEnd==NULL||pDatumTube2==NULL||pDatumTube2->pStart==NULL||pDatumTube2->pEnd==NULL)
		return false;
	if(bTubeJointStart&&(pDatumTube->pStart!=pDatumTube2->pStart&&pDatumTube->pStart!=pDatumTube2->pEnd))
		return false;
	if(bTubeJointEnd&&(pDatumTube->pEnd!=pDatumTube2->pStart&&pDatumTube->pEnd!=pDatumTube2->pEnd))
		return false;
	PARTSET partset,partset2;
	CHashSet<CLDSPart*> hashPartSet;
	BelongModel()->GetPartGroupPartSet(handle,partset);
	BelongModel()->GetPartGroupPartSet(pDatumTube2->handle,partset2);
	hashPartSet.SetValue(pDatumTube->handle,pDatumTube);
	hashPartSet.SetValue(pDatumTube2->handle,pDatumTube2);
	CLDSPart* pPart;
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		hashPartSet.SetValue(pPart->handle,pPart);
	for(pPart=partset2.GetFirst();pPart;pPart=partset2.GetNext())
		hashPartSet.SetValue(pPart->handle,pPart);
	for(pPart=hashPartSet.GetFirst();pPart;pPart=hashPartSet.GetNext())
	{
		if(!pPart->IsTube()||pPart==pDatumTube||pPart==pDatumTube2)
			continue;
		return false;	//不允许出现两根以上的的钢管
	}
	if(horiBeamOrGeneralDualTube!=NULL)	//普通两管相贯组焊图
		*horiBeamOrGeneralDualTube=GROUP_DUALTUBE_JOINT;
	GEPOINT lenStdVec =pDatumTube->pEnd->Position(false)-pDatumTube->pStart->Position(false);
	normalize(lenStdVec);
	GEPOINT lenStdVec2=pDatumTube2->pEnd->Position(false)-pDatumTube2->pStart->Position(false);
	normalize(lenStdVec2);
	m_hJointTube=pDatumTube2->handle;
	if(fabs(lenStdVec.z)<=0.02&&fabs(lenStdVec2.z)<=0.02)
	{
		GEPOINT vCrossNormal=lenStdVec^lenStdVec2;
		normalize(vCrossNormal);
		CSuperSmartPtr<CLDSPart> pSubPart;
		for(pPart=hashPartSet.GetFirst();pPart;pPart=hashPartSet.GetNext())
		{
			if(!pPart->IsPlate())
				continue;
			CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pPart;
			if(!pPlate->IsConnectPlate())
				continue;
			if(fabs(pPlate->ucs.axis_z*vCrossNormal)>EPS_COS2)
			{
				if(horiBeamOrGeneralDualTube)
					*horiBeamOrGeneralDualTube=GROUP_STATION_BTMTRIBEAM;
				return true;
			}
		}
	}
	return true;
}
//要求normal必须为单位化法线方向
GEPOINT AverageNormal(AVER_NORMAL* pCurrStatNormal,const double* normal)
{
	GEPOINT vNormal(normal);
	double UNDER=pCurrStatNormal->uiStatCount+1.0;
	if(pCurrStatNormal->uiStatCount==0)
		pCurrStatNormal->vCurrNormal=vNormal;
	else
		pCurrStatNormal->vCurrNormal=pCurrStatNormal->vCurrNormal*(pCurrStatNormal->uiStatCount/UNDER)+vNormal/UNDER;
	pCurrStatNormal->uiStatCount++;
	return pCurrStatNormal->vCurrNormal;
}
void CLDSPartGroup::UpdateDualTubeJointView(CDualTubeJointView *pMainView,CDualTubeJointView **ppMainView2,
											CLDSLineTube *pDatumTube,CLDSLineTube *pJointTube,PARTSET& partset1,PARTSET& partset2)
{
	CLDSPart* pPart;
	CSectionView::PART_ITEM* pItem;
	AVER_NORMAL* pAverageNormal;
	CHashListEx<AVER_NORMAL> hashWorkPlaneNormals;
	UINT uiMaxIdleId=1;
	for(CDrawingAssistant* pAssist=pMainView->hashAssistPlanes.GetFirst();pAssist;pAssist=pMainView->hashAssistPlanes.GetNext())
		uiMaxIdleId=max(uiMaxIdleId,pAssist->iNo+1);
	hashWorkPlaneNormals.SetMinmalId(uiMaxIdleId);
	CHorizBeamTubeView* pHorizBeamView=pMainView->m_idViewType==CSectionView::CLS_HORIZ_BEAM_VIEW?(CHorizBeamTubeView*)pMainView:NULL;

	double halfDia=pDatumTube->GetDiameter()*0.5;
	double halfJointDia=pJointTube->GetDiameter()*0.5;
	GEPOINT vDatumRodAxis=pDatumTube->End()-pDatumTube->Start();
	GEPOINT vJointRodAxis=pJointTube->End()-pJointTube->Start();
	if(vDatumRodAxis*pMainView->ucs.axis_x<0)
		vDatumRodAxis*=-1;
	if(vJointRodAxis*pMainView->ucs.axis_x<0)
		vJointRodAxis*=-1;
	GEPOINT vDatumVertAxisY=pMainView->ucs.axis_z^vDatumRodAxis;
	GEPOINT vJointVertAxisY=pMainView->ucs.axis_z^vJointRodAxis;
	normalize(vDatumRodAxis);
	normalize(vDatumVertAxisY);
	normalize(vJointRodAxis);
	normalize(vJointVertAxisY);
	GECS cs1(pDatumTube->Start(),vDatumRodAxis,vDatumVertAxisY,pMainView->ucs.axis_z);
	GECS cs2(pJointTube->Start(),vJointRodAxis,vJointVertAxisY,pMainView->ucs.axis_z);
	bool hasVertPlates[2]={false,false};
	for(pPart=partset1.GetFirst();pPart;pPart=partset1.GetNext())
	{
		pItem=pMainView->SonParts.Add(pPart->handle);
		GEPOINT vNormal;
		if(pPart->GetClassTypeId()!=CLS_PLATE)
			continue;
		CLDSPlate* pPlate=(CLDSPlate*)pPart;
		if(pPlate->face_N==2)
			vNormal=0.5*(pPlate->huoqufacenorm[0]+pPlate->ucs.axis_z);
		else if(pPlate->face_N==3)
			vNormal=0.5*(pPlate->huoqufacenorm[0]+pPlate->huoqufacenorm[1]);
		else //if(pPlate->face_N==1)
			vNormal=pPlate->ucs.axis_z;

		double ortho_justify=fabs(vNormal*pMainView->ucs.axis_z);
		if(ortho_justify>0.9848)
			continue;	//可近似视为位于当前绘图平面，不需要展开绘制
		if((vNormal*vDatumRodAxis)>0.1736)//EPS2)
			continue;	//非沿钢管轴向钢板
		if(ortho_justify<0.1736)
		{	//可近似视为与当前绘图平面垂直，不需要展开绘制
			hasVertPlates[0]=true;
			if(!pPlate->IsConnectPlate())
				continue;
		}
		for(pAverageNormal=hashWorkPlaneNormals.GetFirst();pAverageNormal;pAverageNormal=hashWorkPlaneNormals.GetNext())
		{
			if(fabs(pAverageNormal->vCurrNormal*vNormal)>0.9848)
				break;	//法线间夹角小于acosa(0.9848)可近似视为同一展开平面，大至<10°
		}
		if(pAverageNormal==NULL)
			pAverageNormal=hashWorkPlaneNormals.Add(0);
		f3dPoint xLocalOrg=cs1.origin,vLocalAxisY=cs1.axis_y,vPlateAxisY;
		SnapPerp(&xLocalOrg,pJointTube->Start(),pJointTube->End(),pPlate->ucs.origin);
		coord_trans(xLocalOrg,pPlate->ucs,FALSE,TRUE);
		vector_trans(vLocalAxisY,pPlate->ucs,FALSE,TRUE);
		if(fabs(vLocalAxisY.x)+fabs(vLocalAxisY.y)<EPS2)
		{	//钢板与当前绘图平面垂直时，取绘图平面法线作为上方参照方向
			vLocalAxisY=pMainView->ucs.axis_z;
			vector_trans(vLocalAxisY,pPlate->ucs,FALSE,TRUE);
		}
		bool upper=false;
		for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			vPlateAxisY.Set(pVertex->vertex.x-xLocalOrg.x,pVertex->vertex.y-xLocalOrg.y);
			double dist=vPlateAxisY*vLocalAxisY;
			if(dist>50)//halfDia)
			{
				upper=true;
				break;
			}
			else if(dist<-50)
				break;
		}
		if(pAverageNormal->vCurrNormal.IsZero())
		{	//该工作平面的第一次初始化，需要计算一下伸展方向相对于绘图平面Y轴的旋转角
			vector_trans(vPlateAxisY,pPlate->ucs,TRUE,TRUE);
			vPlateAxisY-=(vPlateAxisY*vDatumRodAxis)*vDatumRodAxis;
			pAverageNormal->alfa=Cal2dLineAng(0,0,vPlateAxisY*cs1.axis_y,vPlateAxisY*pMainView->ucs.axis_z);
		}
		pAverageNormal->hasUpperParts|= upper;
		pAverageNormal->hasUnderParts|=!upper;
		AverageNormal(pAverageNormal,vNormal);
		pItem->iResidePlaneNo=pAverageNormal->id;	//对接杆件钢管连接面
	}
	for(pPart=partset2.GetFirst();pPart;pPart=partset2.GetNext())
	{
		pItem=pMainView->SonParts.Add(pPart->handle);
		GEPOINT vNormal;
		if(pPart->GetClassTypeId()!=CLS_PLATE)
			continue;
		CLDSPlate* pPlate=(CLDSPlate*)pPart;
		if(pPlate->face_N==2)
			vNormal=0.5*(pPlate->huoqufacenorm[0]+pPlate->ucs.axis_z);
		else if(pPlate->face_N==3)
			vNormal=0.5*(pPlate->huoqufacenorm[0]+pPlate->huoqufacenorm[1]);
		else //if(pPlate->face_N==1)
			vNormal=pPlate->ucs.axis_z;

		double ortho_justify=fabs(vNormal*pMainView->ucs.axis_z);
		if(ortho_justify>0.9848)
			continue;	//可近似视为位于当前绘图平面，不需要展开绘制
		if((vNormal*vJointRodAxis)>0.1736)//EPS2)
			continue;	//非沿钢管轴向钢板
		if(ortho_justify<0.1736)
		{	//可近似视为与当前绘图平面垂直，不需要展开绘制
			hasVertPlates[1]=true;
			if(!pPlate->IsConnectPlate())
				continue;
		}
		for(pAverageNormal=hashWorkPlaneNormals.GetFirst();pAverageNormal;pAverageNormal=hashWorkPlaneNormals.GetNext())
		{
			if(pAverageNormal->uiGroup==0)
				continue;	//平直基准钢管上的旋转展开平面
			//TODO:这里可能会把两根对接钢管上的焊接板归属面搞混
			if(fabs(pAverageNormal->vCurrNormal*vNormal)>0.9848)
				break;	//法线间夹角小于acosa(0.9848)可近似视为同一展开平面，大至<10°
		}
		if(pAverageNormal==NULL)
		{
			pAverageNormal=hashWorkPlaneNormals.Add(0);
			pAverageNormal->id|=0x80000000;
		}
		f3dPoint vPlateAxisY,xLocalOrg=cs2.origin,vLocalAxisY=cs2.axis_y;
		SnapPerp(&xLocalOrg,pJointTube->Start(),pJointTube->End(),pPlate->ucs.origin);
		coord_trans(xLocalOrg,pPlate->ucs,FALSE,TRUE);
		vector_trans(vLocalAxisY,pPlate->ucs,FALSE,TRUE);
		bool upper=false;
		for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			vPlateAxisY.Set(pVertex->vertex.x-xLocalOrg.x,pVertex->vertex.y-xLocalOrg.y);
			double dist=vPlateAxisY*vLocalAxisY;
			if(dist>50)//halfDia)
			{
				upper=true;
				break;
			}
			else if(dist<-50)
				break;
		}
		if(pAverageNormal->vCurrNormal.IsZero())
		{	//该工作平面的第一次初始化，需要计算一下伸展方向相对于绘图平面Y轴的旋转角
			vector_trans(vPlateAxisY,pPlate->ucs,TRUE,TRUE);
			vPlateAxisY-=(vPlateAxisY*vJointRodAxis)*vJointRodAxis;
			pAverageNormal->alfa=Cal2dLineAng(0,0,vPlateAxisY*cs1.axis_y,vPlateAxisY*pMainView->ucs.axis_z);
		}
		pAverageNormal->hasUpperParts|= upper;
		pAverageNormal->hasUnderParts|=!upper;
		AverageNormal(pAverageNormal,vNormal);
		pItem->iResidePlaneNo=pAverageNormal->uiSerial;	//对接杆件钢管连接面
	}
	ARRAY_LIST<AVER_NORMAL*> arrayPlanes[2];
	arrayPlanes[0].SetSize(0,3);
	arrayPlanes[1].SetSize(0,3);
	for(pAverageNormal=hashWorkPlaneNormals.GetFirst();pAverageNormal;pAverageNormal=hashWorkPlaneNormals.GetNext())
	{
		arrayPlanes[pAverageNormal->uiGroup].append(pAverageNormal);
		pAverageNormal->cosa=cos(pAverageNormal->alfa);
	}
	for(int i=0;i<2;i++)
	{
		if(arrayPlanes[i].GetSize()==2)
		{	//双工作平面
			if(CLDSPart::CustomerSerial==7)	//CLDSPart::CustomerSerial=7.安徽宏源
			{	//安徽宏源要求两个工作面时，出两张图而不是上下展开合并为一张主视图
				if(ppMainView2!=NULL)
				{
					CDualTubeJointView *pMainView2=(CDualTubeJointView*)pMainView->BelongPartGroup()->SonViews.Add(0,pMainView->m_idViewType);
					long id=pMainView2->Id;
					CBuffer buffer;
					pMainView->ToBuffer(buffer);
					buffer.SeekToBegin();
					pMainView2->FromBuffer(buffer);
					//视图坐标系YZ轴翻转
					CParaCS::PARAMETRIC_INFO param=pMainView2->des_cs.GetParamInfo();
					if( param.m_eDatumPlane==CParaCS::DP_XZ||
						param.m_eDatumPlane==CParaCS::DP_ZX||
						param.m_eDatumPlane==CParaCS::DP_ZY||
						param.m_eDatumPlane==CParaCS::DP_YZ)
						param.desAxisZ.ReverseDirection();
					else if(param.m_eDatumPlane==CParaCS::DP_XY||param.m_eDatumPlane==CParaCS::DP_YX)
						param.desAxisY.ReverseDirection();
					pMainView2->des_cs.SetParamInfo(param);
					pMainView2->ucs.axis_y*=-1.0;
					pMainView2->ucs.axis_z*=-1.0;
					pMainView2->m_iViewType=VIEW_FRONT;
#ifdef AFX_TARG_ENU_ENGLISH
					strcpy(pMainView2->sName,"Front View");
#else
					strcpy(pMainView2->sName,"正视图");
#endif
					*ppMainView2=pMainView2;
					//从主视图中移除工作面1上的所有构件
					CHashList<long> hashHandles;
					double cos30=0.86602540378443864676372317075294;
					double maxCos=0;
					AVER_NORMAL *pDatumNormal=NULL;
					for(int i=0;i<arrayPlanes[0].GetSize();i++)
					{
						double cosa=fabs(arrayPlanes[0][i]->vCurrNormal*pMainView->ucs.axis_z);
						if(cosa>maxCos)
						{
							maxCos=cosa;
							pDatumNormal=arrayPlanes[0][i];
						}
					}
					for(CSectionView::PART_ITEM *pItem=pMainView->SonParts.GetFirst();pItem;pItem=pMainView->SonParts.GetNext())
					{
						CLDSPlate* pPlate=(CLDSPlate*)BelongModel()->FromPartHandle(pItem->hSonPart,CLS_PLATE);
						if(pPlate&&pDatumNormal&&fabs(pDatumNormal->vCurrNormal*pPlate->ucs.axis_z)<cos30)
						{
							pMainView->SonParts.DeleteNode(pItem->hSonPart);
							hashHandles.SetValue(pPlate->handle,pPlate->handle);
						}
					}
					CLDSPlate *pDatumPlate=NULL,*pMainDatumPlate=NULL;
					for(CSectionView::PART_ITEM *pItem=pMainView2->SonParts.GetFirst();pItem;pItem=pMainView2->SonParts.GetNext())
					{
						CLDSPlate *pPlate=(CLDSPlate*)BelongModel()->FromPartHandle(pItem->hSonPart,CLS_PLATE);
						if(pPlate&&pPlate->IsConnectPlate())
						{
							if(hashHandles.GetValue(pItem->hSonPart)==NULL)
							{
								pMainView2->SonParts.DeleteNode(pItem->hSonPart);
								if(pMainDatumPlate==NULL)
									pMainDatumPlate=pPlate;
							}
							else if(pDatumPlate==NULL&&fabs(pPlate->ucs.axis_z*pMainView2->ucs.axis_z)<0.707)
								pDatumPlate=pPlate;
						}
					}
					if(pDatumPlate)
					{	//需要调整第2个视图坐标系使钢板水平绘制
						CLDSLineTube *pTube=(pDatumPlate&&pDatumPlate->m_hPartWeldParent==pDatumPlate->handle)?pDatumTube:pJointTube;
						CParaCS::PARAMETRIC_INFO param=pMainView2->des_cs.GetParamInfo();
						//CParaCS::DP_XZ、CParaCS::DP_ZX、CParaCS::DP_ZY、CParaCS::DP_YZ方式需要更新Z坐标
						//CParaCS::DP_XY、CParaCS::DP_YX方式修改为CParaCS::DP_XZ
						param.desAxisZ.norm_style=3;
						param.desAxisZ.hVicePart=pDatumPlate->handle;
						param.desAxisZ.direction=0;
						if(param.m_eDatumPlane==CParaCS::DP_XY||param.m_eDatumPlane==CParaCS::DP_YX)
							param.m_eDatumPlane=CParaCS::DP_XZ;
						pMainView2->des_cs.SetParamInfo(param);
						f3dPoint main_spread_vec=pTube->GetPlateVertSpreadVec(pMainDatumPlate);
						f3dPoint spread_vec=pTube->GetPlateVertSpreadVec(pDatumPlate);
						GECS mainCS=pMainView->UpdateUCS(BelongModel(),this);
						GECS curCS=pMainView2->UpdateUCS(BelongModel(),this);
						vector_trans(main_spread_vec,mainCS,FALSE);
						vector_trans(spread_vec,curCS,FALSE);
						if(spread_vec*main_spread_vec>0)
						{
							param.desAxisZ.ReverseDirection();
							pMainView2->des_cs.SetParamInfo(param);
							pMainView2->UpdateUCS(BelongModel(),this);
						}
					}
					return;
				}
			}
			else
			{	//含景云祥在内其他厂家，CLDSPart::CustomerSerial=8.云南景云祥
				if(arrayPlanes[i][0]->cosa<0&&arrayPlanes[i][1]->cosa<0)
				{	//双工作平面均位于下侧，则优先将前侧钢板置为下侧板
					if(arrayPlanes[i][0]->alfa<arrayPlanes[i][1]->alfa)
					{
						arrayPlanes[i][0]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_UNDER;
						if(hasVertPlates[i]&&arrayPlanes[i][1]->alfa<Pi)
							arrayPlanes[i][1]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_TOP;
						else if(hasVertPlates[i]&&arrayPlanes[i][1]->alfa>=Pi)
							arrayPlanes[i][1]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_BACK;
						else
							arrayPlanes[i][1]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_UPPER;
					}
					else
					{
						arrayPlanes[i][1]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_UNDER;
						if(hasVertPlates[i]&&arrayPlanes[i][0]->alfa<Pi)
							arrayPlanes[i][0]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_TOP;
						else if(hasVertPlates[i]&&arrayPlanes[i][0]->alfa>=Pi)
							arrayPlanes[i][0]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_BACK;
						else
							arrayPlanes[i][0]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_UPPER;
					}
				}
				else if(arrayPlanes[i][0]->cosa>0&&arrayPlanes[i][1]->cosa>0)
				{	//双工作平面均位于上侧，则优先将前侧钢板置为上侧板
					if(arrayPlanes[i][0]->alfa<arrayPlanes[i][1]->alfa)
					{
						arrayPlanes[i][0]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_UPPER;
						if(hasVertPlates[i]&&arrayPlanes[i][1]->alfa<1.57)
							arrayPlanes[i][1]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_TOP;
						else if(hasVertPlates[i]&&arrayPlanes[i][1]->alfa>4.71)
							arrayPlanes[i][1]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_BACK;
						else
							arrayPlanes[i][1]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_UNDER;
					}
					else
					{
						arrayPlanes[i][1]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_UPPER;
						if(hasVertPlates[i]&&arrayPlanes[i][0]->alfa<1.57)
							arrayPlanes[i][0]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_TOP;
						else if(hasVertPlates[i]&&arrayPlanes[i][0]->alfa>4.71)
							arrayPlanes[i][0]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_BACK;
						else
							arrayPlanes[i][0]->ciSpreadDirection=CDrawingAssistant::SPREAD_TO_UNDER;
					}
				}
			}
		}
		if(arrayPlanes[i].GetSize()==3)
		{
			//排序使各平面的cosa由小至大
			AVER_NORMALPTR temparr[3]={arrayPlanes[i][0],arrayPlanes[i][1]};
			if(arrayPlanes[i][0]->cosa<arrayPlanes[i][1]->cosa)
			{
				temparr[0]=arrayPlanes[i][1];
				temparr[1]=arrayPlanes[i][0];
			}
			if(temparr[0]->cosa<arrayPlanes[i][2]->cosa)
			{
				temparr[2]=temparr[1];
				temparr[1]=temparr[0];
				temparr[0]=arrayPlanes[i][2];
			}
			else if(temparr[1]->cosa<arrayPlanes[i][2]->cosa)
			{
				temparr[2]=temparr[1];
				temparr[1]=arrayPlanes[i][2];
			}
			else
				temparr[2]=arrayPlanes[i][2];
			arrayPlanes[i][0]=temparr[0];
			arrayPlanes[i][1]=temparr[1];
			arrayPlanes[i][2]=temparr[2];
			if(fabs(arrayPlanes[i][0]->cosa)<0.2)
			{
				arrayPlanes[i][0]->ciSpreadDirection=3;
				if(arrayPlanes[i][0]->alfa>Pi)
					arrayPlanes[i][0]->ciSpreadDirection=4;
			}
			else
				arrayPlanes[i][0]->ciSpreadDirection=1;
			if(fabs(arrayPlanes[i][1]->cosa)<0.866)	//与绘图平面夹角不大于60度
			if(arrayPlanes[i][1]->alfa<Pi)
				arrayPlanes[i][0]->ciSpreadDirection=3;
			else
				arrayPlanes[i][0]->ciSpreadDirection=4;
			if(fabs(arrayPlanes[i][2]->cosa)<0.2)
			{
				arrayPlanes[i][2]->ciSpreadDirection=3;
				if(arrayPlanes[i][2]->alfa>Pi)
					arrayPlanes[i][2]->ciSpreadDirection=4;
			}
			else
				arrayPlanes[i][2]->ciSpreadDirection=2;	//下侧
		}
	}
	//upperSpreadPlane[2]={0,0},underSpreadPlane[2]={0,0},backSpreadPlane[2]={0,0},topSpreadPlane[2]={0,0};
	for(pAverageNormal=hashWorkPlaneNormals.GetFirst();pAverageNormal;pAverageNormal=hashWorkPlaneNormals.GetNext())
	{
		CDrawingAssistant* pAssist=pMainView->hashAssistPlanes.Add(pAverageNormal->uiSerial);
		if(pHorizBeamView&&pHorizBeamView->m_cSubType==CHorizBeamTubeView::HORIZBEAM_FRONT_VIEW&&(pAverageNormal->id&0x80000000)>0)
			pAssist->iResidePlaneNo=1;
		else
			pAssist->iResidePlaneNo=0;
		pAssist->ciSpreadDirection=pAverageNormal->ciSpreadDirection;
		if(pAverageNormal->ciSpreadDirection==CDrawingAssistant::SPREAD_TO_UNDER&&pAverageNormal->cosa>0)
			pAssist->bSpreadOverRidge=true;
		else if(pAverageNormal->ciSpreadDirection==CDrawingAssistant::SPREAD_TO_UPPER&&pAverageNormal->cosa<0)
			pAssist->bSpreadOverRidge=true;
		pAssist->iNo=pAverageNormal->uiSerial;
		pAssist->m_xLineStart.datum_pos_style=1;
		pAssist->m_xLineStart.des_para.RODEND.hRod=pAverageNormal->uiGroup==0?pDatumTube->handle:pJointTube->handle;
		pAssist->m_xLineStart.des_para.RODEND.direction=0;
		pAssist->m_xLineEnd.datum_pos_style=1;
		pAssist->m_xLineEnd.des_para.RODEND.hRod=pAverageNormal->uiGroup==0?pDatumTube->handle:pJointTube->handle;
		pAssist->m_xLineEnd.des_para.RODEND.direction=1;
		pAssist->m_xFaceNorm.norm_style=0;
		pAssist->wStyleFlag=CDrawingAssistant::ENABLE_LINE_END;//0x0001;
		if(fabs(pAverageNormal->vCurrNormal*pMainView->ucs.axis_z)<0.866)	//展开面法线与基面法线夹角小于30°即视为立面展开
		{
			GEPOINT vVertAxis=pAverageNormal->uiGroup==0?vDatumVertAxisY:vJointVertAxisY;
			GEPOINT vRodAxisTemp=pAverageNormal->vCurrNormal^vVertAxis;
			GEPOINT vNormal=pAverageNormal->vCurrNormal^vRodAxisTemp;
			normalize(vNormal);
			pAssist->m_xFaceNorm.vector=pAssist->m_xFaceNorm.nearVector=vNormal;
		}
		else
			pAssist->m_xFaceNorm.vector=pAssist->m_xFaceNorm.nearVector=pAverageNormal->vCurrNormal;
	}
}
BOOL CLDSPartGroup::AddHorizBeamTubeView(CLDSPart* pDatumPart,PARTSET& partset)
{
	CLDSPart* pPart;
	CHashSet<CLDSPart*> hashSumParts;
	CLDSLineTube *pDatumTube =(CLDSLineTube*)BelongModel()->FromPartHandle(handle,CLS_LINETUBE);
	CLDSLineTube *pJointTube =(CLDSLineTube*)BelongModel()->FromPartHandle(m_hJointTube,CLS_LINETUBE);

	if(pDatumTube==NULL||pJointTube==NULL)
		return FALSE;
	hashSumParts.SetValue(pDatumTube->handle,pDatumTube);
	hashSumParts.SetValue(pJointTube->handle,pJointTube);
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		hashSumParts.SetValue(pPart->handle,pPart);
	GEPOINT lenStdVec =pDatumTube->pEnd->xPreliftPos-pDatumTube->pStart->xPreliftPos;
	normalize(lenStdVec);
	//if(fabs(lenStdVec.z)>0.02)
	//	return false;
	GEPOINT lenStdVec2=pJointTube->pEnd->xPreliftPos-pJointTube->pStart->xPreliftPos;
	normalize(lenStdVec2);
	//if(fabs(lenStdVec2.z)>0.02)
	//	return false;
	if(fabs(lenStdVec.x) +fabs(lenStdVec.y) +fabs(lenStdVec.z)>fabs(lenStdVec2.x)+fabs(lenStdVec2.y)+fabs(lenStdVec2.z))
	{	//交换两根基准钢管，以保证pDatumTube更接近与坐标轴
		CLDSLineTube* pTmpTube=pDatumTube;
		pDatumTube=pJointTube;
		pJointTube=pTmpTube;
		GEPOINT stdvec=lenStdVec2;
		lenStdVec2=lenStdVec;
		lenStdVec=stdvec;
	}
	/*GEPOINT vCrossPlaneNormal=lenStdVec^lenStdVec2;
	normalize(vCrossPlaneNormal);
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(!pPart->IsPlate()||!((CLDSGeneralPlate*)pPart)->IsConnectPlate())
			continue;	//非连接钢板
		if(fabs(vCrossPlaneNormal*pPart->ucs.axis_z)>EPS_COS2)
			break;	//当前对接钢管交叉平面上存在连接板
	}
	//对接钢管交叉平面上无连接板存在，视同普通对接钢管视图
	bool bCrossPlaneNotWorkPlane=(pPart==NULL);
	if(bCrossPlaneNotWorkPlane)
	{	
		//TODO:未完待续
	}*/
	PARTSET partset1;	//直接焊接于pDatumTube平基准钢管上的焊接子构件集合
	PARTSET partset2;	//直接焊接于pJointTube斜对接钢管上的焊接子构件集合
	BelongModel()->GetPartGroupPartSet(pDatumTube->handle,partset1,0,true,&pJointTube->handle);	//平基准钢管上的焊接子构件集合
	//BelongModel()->GetPartGroupPartSet(pJointTube->handle,partset2,0,true,&pDatumTube->handle);	//仅获取另一根斜对接钢管上的焊接子构件
	BYTE cbJointState=0;
	if( pDatumTube->pStart==pJointTube->pStart)
		cbJointState=0x00;
	else if(pDatumTube->pStart==pJointTube->pEnd)
		cbJointState=0x01;
	else if(pDatumTube->pEnd==pJointTube->pStart)
		cbJointState=0x10;
	else if(pDatumTube->pEnd==pJointTube->pEnd)
		cbJointState=0x11;
	//1.增加正视图（将两根对接主管展开摆放至X轴水平位置
	CHorizBeamTubeView *pMainView = (CHorizBeamTubeView*)SonViews.Add(0,CSectionView::CLS_HORIZ_BEAM_VIEW);
	pMainView->m_cSubType=CHorizBeamTubeView::HORIZBEAM_FRONT_VIEW;
	pMainView->m_iViewType=VIEW_MAIN;
	pMainView->m_hMasterTube=pDatumTube->handle;
	pMainView->m_hJointTube=pJointTube->handle;
	CParaCS::PARAMETRIC_INFO csParaInfo;
	csParaInfo.desOrg.datum_pos_style=8;
	if(cbJointState&0x10)
		csParaInfo.desOrg.des_para.hNode=pDatumTube->pEnd->handle;
	else
		csParaInfo.desOrg.des_para.hNode=pDatumTube->pStart->handle;
	csParaInfo.desAxisX.norm_style=5;
	csParaInfo.desAxisX.hVicePart=pDatumTube->handle;
	csParaInfo.desAxisY.norm_style=0;
	if(fabs(lenStdVec.y)<fabs(lenStdVec.x))
	{
		csParaInfo.desAxisX.direction=lenStdVec.x>0?0:1;
		//csParaInfo.desAxisX.vector.Set(1,0,0);
	}
	else	//默认自前向后为视图X轴正向
	{
		csParaInfo.desAxisX.direction=lenStdVec.y<0?0:1;
		//csParaInfo.desAxisX.vector.Set(0,1,0);
	}
	csParaInfo.desAxisY.norm_style=2;
	csParaInfo.desAxisY.hVicePart=pDatumTube->handle;
	csParaInfo.desAxisY.hCrossPart=pJointTube->handle;
	csParaInfo.desAxisY.nearVector.Set(0,0,-1);
	//csParaInfo.desAxisZ.vector.Set(0,1,0);
	csParaInfo.m_eDatumPlane=CParaCS::DP_XY;	//X-Y定位
	pMainView->des_cs.SetParamInfo(csParaInfo);
	pMainView->UpdateUCS(m_pModel,this);

	CSectionView::PART_ITEM* pItem;
	pMainView->AddPart(pDatumTube->handle, pDatumTube);
	pMainView->AddPart(pJointTube->handle, pJointTube);
	for(pPart=partset1.GetFirst();pPart;pPart=partset1.GetNext())
		pItem=pMainView->SonParts.Add(pPart->handle);
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(pMainView->SonParts.GetValue(pPart->handle)==NULL)
			partset2.append(pPart);	//直接隶属于斜对接钢管上的子构件
	}
	CDrawingAssistant* pRotJointTubePlane= pMainView->hashAssistPlanes.Add(1);
	if(pRotJointTubePlane)
	{
		pRotJointTubePlane->iNo=1;
		pRotJointTubePlane->m_nStyle=3;
		pRotJointTubePlane->m_xLineStart.datum_pos_style=2;
		pRotJointTubePlane->m_xLineStart.des_para.RODNODE.hRod=pDatumTube->handle;
		if((cbJointState&0xf0)==0)
		{	//始端连接，计算终->始反射线方向
			pRotJointTubePlane->m_xLineStart.des_para.RODNODE.hNode=pDatumTube->pStart->handle;
			lenStdVec=pDatumTube->pStart->xArcliftPos-pDatumTube->pEnd->xArcliftPos;
		}
		else
		{
			pRotJointTubePlane->m_xLineStart.des_para.RODNODE.hNode=pDatumTube->pEnd->handle;
			lenStdVec=pJointTube->pEnd->xArcliftPos-pJointTube->pStart->xArcliftPos;
		}
		if((cbJointState&0x0f)==0)	//始端连接，计算始->终射线方向
			lenStdVec2=pJointTube->pEnd->xArcliftPos-pJointTube->pStart->xArcliftPos;
		else
			lenStdVec2=pJointTube->pStart->xArcliftPos-pJointTube->pEnd->xArcliftPos;
		normalize(lenStdVec);
		normalize(lenStdVec2);
		double fRotRadAngle=ACOS(lenStdVec*lenStdVec2);
		GEPOINT vRodAxis=lenStdVec2^lenStdVec;
		if(vRodAxis*pMainView->ucs.axis_y<0)
			pRotJointTubePlane->m_fRotRadAng=-fRotRadAngle;
		else
			pRotJointTubePlane->m_fRotRadAng= fRotRadAngle;
		pRotJointTubePlane->m_xFaceNorm.norm_style=2;
		pRotJointTubePlane->m_xFaceNorm.nearVector=pMainView->ucs.axis_y;
		pRotJointTubePlane->m_xFaceNorm.hVicePart=pDatumTube->handle;
		pRotJointTubePlane->m_xFaceNorm.hCrossPart=pJointTube->handle;
		pItem=pMainView->SonParts.GetValue(pJointTube->handle);
		pItem->iResidePlaneNo=1;
		for(pPart=partset2.GetFirst();pPart;pPart=partset2.GetNext())
		{
			pItem=pMainView->SonParts.Add(pPart->handle);
			pItem->iResidePlaneNo=1;
		}
	}
	CDualTubeJointView *pMainView2=NULL;;
	UpdateDualTubeJointView(pMainView,&pMainView2,pDatumTube,pJointTube,partset1,partset2);
	//2.增加俯视图（将两根对接钢管去除预拱后展开至俯视水平位置）
	CHorizBeamTubeView *pVertView = (CHorizBeamTubeView*)SonViews.Add(0,CSectionView::CLS_HORIZ_BEAM_VIEW);
	pVertView->m_cSubType=CHorizBeamTubeView::HORIZBEAM_VERTICAL_VIEW;
	pVertView->m_iViewType=VIEW_MAIN;
	pVertView->m_hMasterTube=pDatumTube->handle;
	pVertView->m_hJointTube=pJointTube->handle;
	pVertView->AddPart(pDatumTube->handle, pDatumTube);
	pVertView->AddPart(pJointTube->handle, pJointTube);
	csParaInfo.desAxisX.norm_style=5;
	csParaInfo.desAxisZ.norm_style=2;
	//if(fabs(lenStdVec.y)<fabs(lenStdVec.x))
	//	csParaInfo.desAxisX.vector.Set(1,0,0);
	//else
	//	csParaInfo.desAxisX.vector.Set(0,1,0);
	csParaInfo.desAxisZ.hVicePart=pDatumTube->handle;
	csParaInfo.desAxisZ.hCrossPart=pJointTube->handle;
	csParaInfo.desAxisZ.nearVector.Set(0,0,-1);
	csParaInfo.desAxisZ.vector.Set(0,0,-1);
	csParaInfo.m_eDatumPlane=CParaCS::DP_XZ;	//X-Z定位
	pVertView->des_cs.SetParamInfo(csParaInfo);
	//pMainView->hashAssistPlanes.Add()
	pVertView->UpdateUCS(m_pModel,this);

#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(pMainView->sName,"Front View");
	strcpy(pVertView->sName,"Vertical View");
#else
	strcpy(pMainView->sName,"主视图");
	strcpy(pVertView->sName,"俯视图");
#endif

	for(pItem=pMainView->SonParts.GetFirst();pItem;pItem=pMainView->SonParts.GetNext())
	{
		CSectionView::PART_ITEM* pNewItem=pVertView->SonParts.SetValue(pItem->hSonPart,*pItem);
		pNewItem->iResidePlaneNo=0;
		pNewItem->hSonPart=pItem->hSonPart;
	}
	UpdateDualTubeJointView(pVertView,NULL,pDatumTube,pJointTube,partset1,partset2);
	//3.添加其余端面图及节点剖面图
	//3.2 添加钢管端头视图(包括法兰视图、插板视图、平面切割视图)
	int viewIndex=0;
	AddLineTubeEndLjView(pDatumTube,pMainView,partset1,0,viewIndex);
	AddLineTubeEndLjView(pJointTube,pMainView,partset2,viewIndex,viewIndex);
	//3.3 添加钢管中间节点对应的端面视图
	AddLineTubeMiddleNodeEndView(pDatumTube,pMainView,partset1,viewIndex);
	AddLineTubeMiddleNodeEndView(pJointTube,pMainView,partset2,viewIndex);
	return TRUE;
}
//10、智能添加变电构架三角梁底部的收口梁组焊件视图
BOOL CLDSPartGroup::AddGeneralDualTubeJointView(CLDSPart* pDatumPart,PARTSET& partset)
{
	CLDSLineTube *pDatumTube =(CLDSLineTube*)BelongModel()->FromPartHandle(handle,CLS_LINETUBE);
	CLDSLineTube *pJointTube =(CLDSLineTube*)BelongModel()->FromPartHandle(m_hJointTube,CLS_LINETUBE);
	if(pDatumTube==NULL||pJointTube==NULL)
		return FALSE;

	CLDSPart* pPart;
	CHashSet<CLDSPart*> hashSumParts;
	hashSumParts.SetValue(pDatumTube->handle,pDatumTube);
	hashSumParts.SetValue(pJointTube->handle,pJointTube);
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		hashSumParts.SetValue(pPart->handle,pPart);
	GEPOINT lenStdVec =pDatumTube->pEnd->xPreliftPos-pDatumTube->pStart->xPreliftPos;
	GEPOINT lenStdVec2=pJointTube->pEnd->xPreliftPos-pJointTube->pStart->xPreliftPos;
	normalize(lenStdVec);
	normalize(lenStdVec2);
	if(fabs(lenStdVec.x) +fabs(lenStdVec.y) +fabs(lenStdVec.z)>fabs(lenStdVec2.x)+fabs(lenStdVec2.y)+fabs(lenStdVec2.z))
	{	//交换两根基准钢管，以保证pDatumTube更接近与坐标轴
		CLDSLineTube* pTmpTube=pDatumTube;
		pDatumTube=pJointTube;
		pJointTube=pTmpTube;
		GEPOINT stdvec=lenStdVec2;
		lenStdVec2=lenStdVec;
		lenStdVec=stdvec;
	}
	GEPOINT vCrossPlaneNormal=lenStdVec^lenStdVec2;
	normalize(vCrossPlaneNormal);
	PARTSET partset1;	//直接焊接于pDatumTube平基准钢管上的焊接子构件集合
	PARTSET partset2;	//直接焊接于pJointTube斜对接钢管上的焊接子构件集合
	BelongModel()->GetPartGroupPartSet(pDatumTube->handle,partset1,0,true,&pJointTube->handle);	//平基准钢管上的焊接子构件集合
	//BelongModel()->GetPartGroupPartSet(pJointTube->handle,partset2,0,true,&pDatumTube->handle);	//仅获取另一根斜对接钢管上的焊接子构件
	BYTE cbJointState=0;
	if( pDatumTube->pStart==pJointTube->pStart)
		cbJointState=0x00;
	else if(pDatumTube->pStart==pJointTube->pEnd)
		cbJointState=0x01;
	else if(pDatumTube->pEnd==pJointTube->pStart)
		cbJointState=0x10;
	else if(pDatumTube->pEnd==pJointTube->pEnd)
		cbJointState=0x11;
	//1.增加正视图（将两根对接主管展开摆放至X轴水平位置
	CDualTubeJointView *pMainView = (CDualTubeJointView*)SonViews.Add(0,CSectionView::CLS_DUALTUBE_JOINT_VIEW);
	pMainView->m_iViewType=VIEW_MAIN;
	pMainView->m_hMasterTube=pDatumTube->handle;
	pMainView->m_hJointTube=pJointTube->handle;
	CParaCS::PARAMETRIC_INFO csParaInfo;
	csParaInfo.desOrg.datum_pos_style=8;
	if(cbJointState&0x10)
		csParaInfo.desOrg.des_para.hNode=pDatumTube->pEnd->handle;
	else
		csParaInfo.desOrg.des_para.hNode=pDatumTube->pStart->handle;
	csParaInfo.desAxisX.norm_style=5;
	csParaInfo.desAxisX.hVicePart=pDatumTube->handle;
	csParaInfo.desAxisY.norm_style=0;
	GEPOINT vNearAxisY(0,0,-1);
	if(fabs(lenStdVec.x)>fabs(lenStdVec.y)&&fabs(lenStdVec.x)>fabs(lenStdVec.z))
		csParaInfo.desAxisX.direction=lenStdVec.x>0?0:1;
	else if(fabs(lenStdVec.y)>fabs(lenStdVec.x)&&fabs(lenStdVec.y)>fabs(lenStdVec.z))
		csParaInfo.desAxisX.direction=lenStdVec.y<0?0:1;
	else if(fabs(lenStdVec.z)>fabs(lenStdVec.x)&&fabs(lenStdVec.z)>fabs(lenStdVec.y))
	{
		csParaInfo.desAxisX.direction=lenStdVec.z>0?0:1;
		if(fabs(vCrossPlaneNormal.y)>fabs(vCrossPlaneNormal.x))
			vNearAxisY.Set(1,0,0);	//接近X-Z平面时绘图平面Y'轴优先为X轴正向
		else
			vNearAxisY.Set(0,-1,0);	//接近Y-Z平面时绘图平面Y'轴优先为Y轴负向
	}
	else
		vNearAxisY.Set(0,0,0);
	GEPOINT vAxisX=csParaInfo.desAxisX.direction==0?lenStdVec:-lenStdVec;
	GEPOINT vAxisY=vCrossPlaneNormal^vAxisX;
	if(vNearAxisY.IsZero())
		vNearAxisY=(cbJointState&0x01)?-lenStdVec2:lenStdVec2;	//vJointRayVec
	if(vAxisY*vNearAxisY<0)	//保证交叉平面正法线方向使对接斜管绘图时向上倾斜
		vCrossPlaneNormal*=-1;

	csParaInfo.desAxisZ.norm_style=2;
	csParaInfo.desAxisZ.hVicePart=pDatumTube->handle;
	csParaInfo.desAxisZ.hCrossPart=pJointTube->handle;
	csParaInfo.desAxisZ.nearVector=vCrossPlaneNormal;
	//csParaInfo.desAxisZ.vector.Set(0,1,0);
	csParaInfo.m_eDatumPlane=CParaCS::DP_XZ;	//X-Z定位
	pMainView->des_cs.SetParamInfo(csParaInfo);
	pMainView->UpdateUCS(m_pModel,this);

	CSectionView::PART_ITEM* pItem;
	pMainView->AddPart(pDatumTube->handle, pDatumTube);
	pMainView->AddPart(pJointTube->handle, pJointTube);
	for(pPart=partset1.GetFirst();pPart;pPart=partset1.GetNext())
		pItem=pMainView->SonParts.Add(pPart->handle);
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(pMainView->SonParts.GetValue(pPart->handle)==NULL)
			partset2.append(pPart);	//直接隶属于斜对接钢管上的子构件
	}
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(pMainView->sName,"Front View");
#else
	strcpy(pMainView->sName,"主视图");
#endif
	CDualTubeJointView *pMainView2=NULL;
	UpdateDualTubeJointView(pMainView,&pMainView2,pDatumTube,pJointTube,partset1,partset2);

	//3.添加其余端面图及节点剖面图
	//3.2 添加钢管端头视图(包括法兰视图、插板视图、平面切割视图)
	int viewIndex=0;
	AddLineTubeEndLjView(pDatumTube,pMainView,partset1,0,viewIndex);
	AddLineTubeEndLjView(pJointTube,pMainView,partset2,viewIndex,viewIndex);
	//3.3 添加钢管中间节点对应的端面视图
	AddLineTubeMiddleNodeEndView(pDatumTube,pMainView,partset1,viewIndex);
	AddLineTubeMiddleNodeEndView(pJointTube,pMainView,partset2,viewIndex);
	return TRUE;
}
//4.添加基准构件为钢板的组焊件视图
BYTE IdentifyWeldPlatesInfo(CLDSPlate *pRootPlate,PARTSET &partset,WELD_PLATES* weldinfo)
{
	CLDSGeneralPlate* pWeldPlate1=NULL,*pWeldPlate2=NULL;
	for(CSuperSmartPtr<CLDSPart> pPlate=partset.GetFirst();pPlate.IsHasPtr();pPlate=partset.GetNext())
	{
		if(!pPlate->IsPlate())
			continue;
		if(pWeldPlate1==NULL)
			pWeldPlate1=pPlate.GeneralPlatePointer();
		else if(pWeldPlate2==NULL)
		{
			if(fabs(pWeldPlate1->ucs.axis_z*pPlate->ucs.axis_z)>EPS_COS2)
			{
				if(pPlate->GetLsCount()>pWeldPlate1->GetLsCount())
					pWeldPlate1=pPlate.GeneralPlatePointer();
				continue;
			}
			pWeldPlate2=pPlate.GeneralPlatePointer();
		}
		else
		{	//拥有多块连接钢板时，应将钢板分成若干平面，并在同工作平面钢板中选取最恰当板作工作(绘图)面基准
			if(fabs(pWeldPlate1->ucs.axis_z*pPlate->ucs.axis_z)>EPS_COS2&&pPlate->GetLsCount()>pWeldPlate1->GetLsCount())
				pWeldPlate1=pPlate.GeneralPlatePointer();
			else if(fabs(pWeldPlate2->ucs.axis_z*pPlate->ucs.axis_z)>EPS_COS2&&pPlate->GetLsCount()>pWeldPlate2->GetLsCount())
				pWeldPlate2=pPlate.GeneralPlatePointer();
			//else	//暂时无法处理
			//	return false;
		}
	}
	if(pWeldPlate1&&pWeldPlate2==NULL)
	{
		weldinfo->cType=pWeldPlate1->GetLsCount()>0?WELD_PLATES::WITH_HOLE_PLATE:WELD_PLATES::WITH_NONEHOLE_PLATE;
		if(pRootPlate->GetFaceN()>1&&pRootPlate->GetFaceN()==pWeldPlate1->GetFaceN()&&pWeldPlate1->GetLsCount()>0)
			weldinfo->cType=WELD_PLATES::WITH_BUDDY_BENDPLATE;
		weldinfo->pBuddyPlate=pWeldPlate1;
	}
	else if(pWeldPlate1&&pWeldPlate2)
	{
		double area=pRootPlate->CalAreaOfNetWrap();
		double area1=pWeldPlate1->CalAreaOfNetWrap();
		double area2=pWeldPlate2->CalAreaOfNetWrap();
		if((pWeldPlate2->GetFaceN()==pRootPlate->GetFaceN()&&pWeldPlate1->GetFaceN()!=pRootPlate->GetFaceN())||
			fabs(area1-area)>fabs(area2-area)||pWeldPlate1->GetLsCount()==0&&pWeldPlate2->GetLsCount()>0)
		{
			weldinfo->pBuddyPlate=pWeldPlate2;
			weldinfo->pSectPlate=pWeldPlate1;
		}
		else
		{
			weldinfo->pBuddyPlate=pWeldPlate1;
			weldinfo->pSectPlate=pWeldPlate2;
		}
		weldinfo->cType=WELD_PLATES::WITH_TRIPPLE_PLATES;
	}
	else
		weldinfo->cType=0;
	return weldinfo->cType;
}
BOOL CLDSPartGroup::AddPlatePartGroupViews(CLDSPlate *pRootPlate,PARTSET &partset)
{
	if(pRootPlate==NULL||pRootPlate->handle!=handle)
		return FALSE;
	if(m_bFootPartGroup)
	{	//添加塔脚板默认视图
		CLDSLineAngle *pDatumLineAngle1=NULL,*pDatumLineAngle2=NULL;
		pDatumLineAngle1=(CLDSLineAngle*)m_pModel->FromPartHandle(pRootPlate->designInfo.m_hBasePart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		CDesignLjPartPara *pLjPartPara=NULL;
		for(pLjPartPara=pRootPlate->designInfo.partList.GetFirst();pLjPartPara;pLjPartPara=pRootPlate->designInfo.partList.GetNext())
		{
			if(pLjPartPara->hPart==pRootPlate->designInfo.m_hBasePart)
				continue;
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)m_pModel->FromPartHandle(pRootPlate->designInfo.m_hBasePart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pDatumLineAngle1==NULL)
				pDatumLineAngle1=pLineAngle;
			else if(pDatumLineAngle2==NULL)
				pDatumLineAngle2=pLineAngle;
		}
		f3dLine line;
		f3dArcLine arcline;
		if(pDatumLineAngle1==NULL&&pDatumLineAngle2==NULL)
			return FALSE;	//无基准杆件
		for(int i=0;i<6;i++)
		{
			if(i==5&&(pDatumLineAngle1==NULL||pDatumLineAngle2==NULL))
				continue;	//非牛蹄板无需添加仰视图
			//CSectionView *pSectionView = SonViews.append();
			CSectionView *pSectionView = SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
			UCS_STRU tm_ucs = pRootPlate->ucs;
			if(i==0)	
			{	//前面视图
				tm_ucs.axis_x.Set(1,0,0);
				tm_ucs.axis_y.Set(0,0,-1);
				tm_ucs.axis_z.Set(0,1,0);
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(pSectionView->sName,"%s","Front View");
#else
				sprintf(pSectionView->sName,"%s","前面视图");
#endif
			}
			else if(i==1)
			{	//右面视图
				tm_ucs.axis_x.Set(0,-1,0);
				tm_ucs.axis_y.Set(0,0,-1);
				tm_ucs.axis_z.Set(1,0,0);
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(pSectionView->sName,"%s","Right View");
#else
				sprintf(pSectionView->sName,"%s","右面视图");
#endif
			}
			else if(i==2)
			{	//后面视图
				tm_ucs.axis_x.Set(-1,0,0);
				tm_ucs.axis_y.Set(0,0,-1);
				tm_ucs.axis_z.Set(0,-1,0);
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(pSectionView->sName,"%s","Back View");
#else
				sprintf(pSectionView->sName,"%s","后面视图");
#endif
			}
			else if(i==3)
			{	//左面视图
				tm_ucs.axis_x.Set(0,1,0);
				tm_ucs.axis_y.Set(0,0,-1);
				tm_ucs.axis_z.Set(-1,0,0);
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(pSectionView->sName,"%s","Left View");
#else
				sprintf(pSectionView->sName,"%s","左面视图");
#endif
			}
			else if(i==4)
			{	//俯视图
				tm_ucs=pRootPlate->ucs;
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(pSectionView->sName,"%s","Downward View");
#else
				sprintf(pSectionView->sName,"%s","俯视图");
#endif
			}
			else if(i==5)
			{	//仰视图
				tm_ucs=pRootPlate->ucs;
				//仰视图坐标系需要将坐标原点定位在底板顶面  wht 10-01-14
				tm_ucs.origin=pRootPlate->ucs.origin+pRootPlate->ucs.axis_z*pRootPlate->GetThick();
				tm_ucs.axis_z*=-1.0;
				tm_ucs.axis_y*=-1.0;
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(pSectionView->sName,"%s","Upward View");
#else
				sprintf(pSectionView->sName,"%s","仰视图");
#endif
			}

			if((fabs(tm_ucs.axis_x.y)<EPS2||fabs(tm_ucs.axis_x.x)<EPS2)&&i<4)	
			{	//正/侧面图
				tm_ucs.axis_z.z=0;
				normalize(tm_ucs.axis_z);
				tm_ucs.axis_y=tm_ucs.axis_z^tm_ucs.axis_x;
				tm_ucs.axis_x=tm_ucs.axis_y^tm_ucs.axis_z;
				normalize(tm_ucs.axis_x);
				normalize(tm_ucs.axis_y);
			}
			pSectionView->ucs=tm_ucs;
			//坐标系设置参数
			CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
			param.desOrg.datum_pos_style=0;
			param.desOrg.datum_pos=tm_ucs.origin;
			param.m_eDatumPlane=CParaCS::DP_XY;
			param.desAxisX.norm_style=0;
			param.desAxisX.vector=tm_ucs.axis_x;
			param.desAxisY.norm_style=0;
			param.desAxisY.vector=tm_ucs.axis_y;
			param.desAxisZ.norm_style=0;
			param.desAxisZ.vector=tm_ucs.axis_z;
			pSectionView->des_cs.SetParamInfo(param);
			//
			pSectionView->AddPart(pRootPlate->handle,pRootPlate);
			for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			{
				if(i>3&&pDatumLineAngle1&&pDatumLineAngle2&&pPart->GetClassTypeId()==CLS_PLATE)
				{	//仰/俯视图 仅显示Z轴正方向的钢板
					PROFILE_VERTEX *pVertex=NULL;
					CLDSPlate *pTmpPlate=(CLDSPlate*)pPart;
					for(pVertex=pTmpPlate->vertex_list.GetFirst();pVertex;pVertex=pTmpPlate->vertex_list.GetNext())
					{
						f3dPoint vertex=pVertex->vertex;
						coord_trans(vertex,pTmpPlate->ucs,TRUE);
						coord_trans(vertex,pSectionView->ucs,FALSE);
						if(vertex.z<0)
							break;
					}
					if(pVertex)
						continue;
				}
				pSectionView->AddPart(pPart->handle,pPart);
			}
			//pSectionView->AddDefaultLocatePoints(pRootPlate,SuperiorTower());	//添加默认的标定点
			//pSectionView->AddDefaultCheckLines(pRootPlate,SuperiorTower());	//添加默认的检测线
			//pSectionView->AddDefaultAngulars(pRootPlate,SuperiorTower());		//添加默认的角度标注线
		}
	}
	else
	{
		/*		wxc-2016.1.6
			1、组合角钢填板组焊图已作为GROUP_STDX_PLATES进行处理，此处不再处理
			2、普通钢板组焊件适当添加正视图、侧视图和俯视图
		*/
		CParaCS::PARAMETRIC_INFO param,param2;
		CSectionView *pLeftView=NULL,*pDownwardView=NULL;
		CSectionView* pSectionView = SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
		pSectionView->m_iViewType=VIEW_FRONT;
		WELD_PLATES weldinfo;	//识别普通组焊钢板视图类型 wjh-2016.3.25
		BYTE identify_type=IdentifyWeldPlatesInfo(pRootPlate,partset,&weldinfo);
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(pSectionView->sName,"%s","Front View");
#else
		sprintf(pSectionView->sName,"%s","正视图");
#endif
		GEPOINT dcs_axis_x,dcs_axis_y;	//主视图绘图坐标系的X轴及Y轴方向（基准钢板局部相对坐标）
		//默认识别失败时的绘图平面基准轴定义
		if(pRootPlate->face_N<=1)
		{
			PROFILE_VERTEX* pVertex,*pPrevVertex=pRootPlate->vertex_list.GetTail();
			GEPOINT rolledgevec,rolledgevec_local;
			for(pVertex=pRootPlate->vertex_list.GetFirst();pVertex;pVertex=pRootPlate->vertex_list.GetNext())
			{
				if(pPrevVertex->m_bRollEdge)
				{
					rolledgevec=pPrevVertex->vertex-pVertex->vertex;
					normalize(rolledgevec);
					rolledgevec_local=rolledgevec;
					vector_trans(rolledgevec,pRootPlate->ucs,TRUE,TRUE);
					break;
				}
			}
			if(!rolledgevec.IsZero())
			{	//单面板如果有卷边，以卷边优先
				if(fabs(rolledgevec.z)>0.707)
					dcs_axis_y=rolledgevec.z>0 ? -rolledgevec_local : rolledgevec_local;
				else if(fabs(rolledgevec.x)>fabs(rolledgevec.y))
					dcs_axis_x=rolledgevec.x>0 ? rolledgevec_local : -rolledgevec_local;
				else if(fabs(rolledgevec.x)>fabs(rolledgevec.y))
					dcs_axis_x=rolledgevec.y<0 ? rolledgevec_local : -rolledgevec_local;
			}
			else if(fabs(pRootPlate->ucs.axis_z.z)>0.707)	//更接近水平的钢板
			{
				if(fabs(pRootPlate->ucs.axis_y.y)>fabs(pRootPlate->ucs.axis_y.x))	//前后沿向的钢板出俯视图
				{
					pRootPlate->ucs.axis_y.y>0 ? dcs_axis_y.Set(0,-1,0) : dcs_axis_y.Set(0,1,0);
					if(pRootPlate->ucs.axis_z.z>0)	
						dcs_axis_y*=-1;	//主视图实际是模型环境下的仰视图
				}
				else	//左右沿向的钢板出左视图
					pRootPlate->ucs.axis_y.x>0 ? dcs_axis_x.Set(0,1,0) : dcs_axis_x.Set(0,-1,0);
			}
			else if(fabs(pRootPlate->ucs.axis_x.z)>fabs(pRootPlate->ucs.axis_y.z))
				pRootPlate->ucs.axis_x.z>0?dcs_axis_y.Set(-1,0,0):dcs_axis_y.Set(1,0,0);
			else
				pRootPlate->ucs.axis_y.z>0?dcs_axis_y.Set(0,-1,0):dcs_axis_y.Set(0,1,0);
		}
		else //if(pDatumPlate->face_N>1)
		{
			GEPOINT huoquaxis=pRootPlate->HuoQuLine[0].endPt-pRootPlate->HuoQuLine[0].startPt;
			vector_trans(huoquaxis,pRootPlate->ucs,TRUE,TRUE);
			normalize(huoquaxis);
			if(fabs(huoquaxis.z)>0.707)	//增加俯视图
				dcs_axis_y=huoquaxis.z>0?-huoquaxis:huoquaxis;
			else if(fabs(huoquaxis.x)>fabs(huoquaxis.y))	//与X轴更接近
				dcs_axis_x=huoquaxis.x>0?huoquaxis:-huoquaxis;
			else
				dcs_axis_x=huoquaxis.y<0?huoquaxis:-huoquaxis;
			if(!dcs_axis_y.IsZero())
				vector_trans(dcs_axis_y,pRootPlate->ucs,FALSE,TRUE);
			if(!dcs_axis_x.IsZero())
				vector_trans(dcs_axis_x,pRootPlate->ucs,FALSE,TRUE);
		}
		param.desOrg.datum_pos_style=9;
		param.desOrg.des_para.hPart=pRootPlate->handle;
		param.desOrg.datum_pos.Set();
		param.desAxisX.norm_style=param.desAxisY.norm_style=param.desAxisZ.norm_style=4;
		param.desAxisX.hVicePart=param.desAxisY.hVicePart=param.desAxisZ.hVicePart=pRootPlate->handle;
		param.desAxisZ.nearVector.Set(0,0,1);
		if(dcs_axis_x.IsZero())
		{
			param.desAxisY.nearVector=dcs_axis_y;
			param.m_eDatumPlane=CParaCS::DP_ZY;
		}
		else
		{
			param.desAxisX.nearVector=dcs_axis_x;
			param.m_eDatumPlane=CParaCS::DP_ZX;
		}
		param.m_cDefStyle=0;
		param2=param;
		if(weldinfo.pBuddyPlate)
		{
			param2.desOrg.des_para.hPart=weldinfo.pBuddyPlate->handle;
			param2.desAxisX.hVicePart=param2.desAxisY.hVicePart=param2.desAxisZ.hVicePart=weldinfo.pBuddyPlate->handle;
		}
		if(weldinfo.cType==WELD_PLATES::WITH_NONEHOLE_PLATE||weldinfo.cType==WELD_PLATES::WITH_HOLE_PLATE)
		{	//焊有无孔肋板，此时一般遵循主板原有绘图坐标系
			if( weldinfo.cType==WELD_PLATES::WITH_HOLE_PLATE||	//焊有有孔连板，此时一般以两板焊缝线作为主视图工作平面的基准轴
				fabs(weldinfo.pBuddyPlate->ucs.axis_z*pRootPlate->ucs.axis_z)>0.01745)	//cos(90+/-1°)=0.01745
			{	//肋板与原钢板非垂直，此时取两板轴线方向为基准轴方向
				GEPOINT prjaxisvec=weldinfo.pBuddyPlate->ucs.axis_z^pRootPlate->ucs.axis_z;
				normalize(prjaxisvec);
				dcs_axis_x.Set();
				dcs_axis_y.Set();
				if(fabs(prjaxisvec.z)>0.707)	//增加俯视图
					dcs_axis_y=prjaxisvec.z>0?-prjaxisvec:prjaxisvec;
				else if(fabs(prjaxisvec.x)>fabs(prjaxisvec.y))	//与X轴更接近
					dcs_axis_x=prjaxisvec.x>0?prjaxisvec:-prjaxisvec;
				else
					dcs_axis_x=prjaxisvec.y<0?prjaxisvec:-prjaxisvec;
				if(!dcs_axis_x.IsZero())
				{
					vector_trans(dcs_axis_x,pRootPlate->ucs,FALSE,TRUE);
					dcs_axis_x.z=0;
					normalize(dcs_axis_x);
				}
				else
				{
					vector_trans(dcs_axis_y,pRootPlate->ucs,FALSE,TRUE);
					dcs_axis_y.z=0;
					normalize(dcs_axis_y);
				}
			}
			else
			{
				GEPOINT prjaxisvec=!dcs_axis_x.IsZero()?dcs_axis_x:dcs_axis_y;
				vector_trans(prjaxisvec,pRootPlate->ucs,TRUE,TRUE);
				if(fabs(prjaxisvec*weldinfo.pBuddyPlate->ucs.axis_z)<0.5)	//cos(30°)=0.5
				{	//交换投影基准轴('X'<->'Y')
					if(dcs_axis_x.IsZero())
					{
						dcs_axis_x.Set(dcs_axis_y.y,-dcs_axis_y.x);
						dcs_axis_y.Set();
					}
					else
					{
						dcs_axis_y.Set(-dcs_axis_x.y,dcs_axis_x.x);
						dcs_axis_x.Set();
					}
				}
			}
			if(!dcs_axis_x.IsZero())
			{	//X轴为主视图工作平面基准轴时，出侧(左)视图
				param.desAxisX.nearVector=dcs_axis_x;
				param.m_eDatumPlane=CParaCS::DP_ZX;
				pSectionView->des_cs.SetParamInfo(param);
				pSectionView->des_cs.UpdateACS(pRootPlate->BelongModel());
				pLeftView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
				param2=param;
				param2.desAxisX=param.desAxisZ;
				param2.desAxisZ=param.desAxisX;
				param2.desAxisZ.nearVector*=-1;
				pLeftView->des_cs.SetParamInfo(param2);
				if(weldinfo.cType==WELD_PLATES::WITH_HOLE_PLATE)
				{	//立板含有螺栓孔时应增加立板展开视图
					param2.desAxisY.nearVector=-pSectionView->des_cs.GetACS().axis_z;
					vector_trans(param2.desAxisY.nearVector,weldinfo.pBuddyPlate->ucs,FALSE,TRUE);
					param2.desAxisY.nearVector.z=0;
					if(weldinfo.pBuddyPlate->ucs.axis_z*pSectionView->des_cs.GetACS().axis_y>0)
						param2.desAxisZ.nearVector.Set(0,0, 1);
					else
						param2.desAxisZ.nearVector.Set(0,0,-1);
					param2.desAxisX.hVicePart=param2.desAxisY.hVicePart=param2.desAxisZ.hVicePart=weldinfo.pBuddyPlate->handle;
					param2.desOrg.des_para.hPart=weldinfo.pBuddyPlate->handle;
					pDownwardView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
					param2.m_eDatumPlane=CParaCS::DP_ZY;
					pDownwardView->des_cs.SetParamInfo(param2);
				}
			}
			else //if(!dcs_axis_y.IsZero())
			{	//Y轴为主视图工作平面基准轴时，出侧(左)视图
				param.desAxisY.nearVector=dcs_axis_y;
				param.m_eDatumPlane=CParaCS::DP_ZY;
				pSectionView->des_cs.SetParamInfo(param);
				pSectionView->des_cs.UpdateACS(pRootPlate->BelongModel());
				pDownwardView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
				param2=param;
				param2.desAxisY=param.desAxisZ;
				param2.desAxisZ=param.desAxisY;
				param2.desAxisY.nearVector*=-1;
				param2.m_eDatumPlane=CParaCS::DP_ZY;
				pDownwardView->des_cs.SetParamInfo(param2);
				if(weldinfo.cType==WELD_PLATES::WITH_HOLE_PLATE)
				{	//立板含有螺栓孔时应增加立板展开视图
					param2.desAxisX.nearVector=pSectionView->des_cs.GetACS().axis_z;
					vector_trans(param2.desAxisX.nearVector,weldinfo.pBuddyPlate->ucs,FALSE,TRUE);
					param2.desAxisX.nearVector.z=0;
					if(weldinfo.pBuddyPlate->ucs.axis_z*pSectionView->des_cs.GetACS().axis_x<0)
						param2.desAxisZ.nearVector.Set(0,0, 1);
					else
						param2.desAxisZ.nearVector.Set(0,0,-1);
					param2.desAxisX.hVicePart=param2.desAxisY.hVicePart=param2.desAxisZ.hVicePart=weldinfo.pBuddyPlate->handle;
					param2.desOrg.des_para.hPart=weldinfo.pBuddyPlate->handle;
					pLeftView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
					param2.m_eDatumPlane=CParaCS::DP_ZX;
					pLeftView->des_cs.SetParamInfo(param2);
				}
			}
		}
		else if(weldinfo.cType==WELD_PLATES::WITH_BUDDY_BENDPLATE||weldinfo.cType==WELD_PLATES::WITH_TRIPPLE_PLATES)
		{
			if(pRootPlate->face_N>1&&weldinfo.pBuddyPlate->GetFaceN()>1)
			{	//同为火曲板（都含螺栓孔），且火曲线共面
				GEPOINT huoquaxis =pRootPlate->HuoQuLine[0].endPt-pRootPlate->HuoQuLine[0].startPt;
				GEPOINT huoquaxis2=weldinfo.pBuddyPlate->GetHuoquLine(0).endPt-weldinfo.pBuddyPlate->GetHuoquLine(0).startPt;
				vector_trans(huoquaxis ,pRootPlate->ucs,TRUE,TRUE);
				vector_trans(huoquaxis2,weldinfo.pBuddyPlate->ucs,TRUE,TRUE);
				GEPOINT corneraxis=huoquaxis^huoquaxis2;
				normalize(corneraxis);
				if(fabs(corneraxis.z)>0.966)	//增加左视图 0.966=cos(15°)
				{	//可能是塔身变坡口处正侧面板,故火曲线位于同一水平面上
					dcs_axis_y.Set();
					dcs_axis_x=huoquaxis.x>0?huoquaxis:-huoquaxis;
					vector_trans(dcs_axis_x,pRootPlate->ucs,FALSE,TRUE);
					normalize(dcs_axis_x);
					param.desAxisX.nearVector.Set(dcs_axis_x.x,dcs_axis_x.y);
					param.m_eDatumPlane=CParaCS::DP_ZX;
					pSectionView->des_cs.SetParamInfo(param);
					pSectionView->des_cs.UpdateACS(pRootPlate->BelongModel());

					dcs_axis_x=huoquaxis2*pSectionView->des_cs.GetACS().axis_z>0?huoquaxis2:-huoquaxis2;
					vector_trans(dcs_axis_x,weldinfo.pBuddyPlate->ucs,FALSE,TRUE);
					normalize(dcs_axis_x);
					param2.desAxisX.nearVector.Set(dcs_axis_x.x,dcs_axis_x.y);
					param2.desAxisX.hVicePart=param2.desAxisY.hVicePart=param2.desAxisZ.hVicePart=weldinfo.pBuddyPlate->handle;
					param2.desOrg.des_para.hPart=weldinfo.pBuddyPlate->handle;
					if(weldinfo.pBuddyPlate->ucs.axis_z*pSectionView->des_cs.GetACS().axis_x<0)
						param2.desAxisZ.nearVector.Set(0,0, 1);
					else
						param2.desAxisZ.nearVector.Set(0,0,-1);
					param2.m_eDatumPlane=CParaCS::DP_ZX;
					pLeftView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
					pLeftView->des_cs.SetParamInfo(param2);
				}
				else
				{	//普通焊接在一起的双面板,应以竖立双面板的法线为视图判断依据
					pSectionView->des_cs.SetParamInfo(param);
					pSectionView->des_cs.UpdateACS(pRootPlate->BelongModel());
					double justify_x=pSectionView->des_cs.GetACS().axis_x*weldinfo.pBuddyPlate->ucs.axis_z;
					double justify_y=pSectionView->des_cs.GetACS().axis_y*weldinfo.pBuddyPlate->ucs.axis_z;
					if(fabs(justify_x)>=fabs(justify_y))
					{	//添加左视图
						dcs_axis_x.Set();
						dcs_axis_y=pSectionView->des_cs.GetACS().axis_y;
						vector_trans(dcs_axis_y,weldinfo.pBuddyPlate->ucs,FALSE,TRUE);
						normalize(dcs_axis_y);
						param2.desAxisY.nearVector.Set(dcs_axis_y.x,dcs_axis_y.y);
						param2.desAxisX.hVicePart=param2.desAxisY.hVicePart=param2.desAxisZ.hVicePart=weldinfo.pBuddyPlate->handle;
						param2.desOrg.des_para.hPart=weldinfo.pBuddyPlate->handle;
						if(weldinfo.pBuddyPlate->ucs.axis_z*pSectionView->des_cs.GetACS().axis_x<0)
							param2.desAxisZ.nearVector.Set(0,0, 1);
						else
							param2.desAxisZ.nearVector.Set(0,0,-1);
						param2.m_eDatumPlane=CParaCS::DP_ZY;
						pLeftView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
						pLeftView->des_cs.SetParamInfo(param2);
					}
					else
					{	//添加俯视图
						dcs_axis_y.Set();
						dcs_axis_x=pSectionView->des_cs.GetACS().axis_x;
						vector_trans(dcs_axis_x,weldinfo.pBuddyPlate->ucs,FALSE,TRUE);
						normalize(dcs_axis_x);
						param2.desAxisX.nearVector.Set(dcs_axis_x.x,dcs_axis_x.y);
						param2.desAxisX.hVicePart=param2.desAxisY.hVicePart=param2.desAxisZ.hVicePart=weldinfo.pBuddyPlate->handle;
						if(weldinfo.pBuddyPlate->ucs.axis_z*pSectionView->des_cs.GetACS().axis_y>0)
							param2.desAxisZ.nearVector.Set(0,0, 1);
						else
							param2.desAxisZ.nearVector.Set(0,0,-1);
						param2.desOrg.des_para.hPart=weldinfo.pBuddyPlate->handle;
						param2.m_eDatumPlane=CParaCS::DP_ZX;
						pDownwardView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
						pDownwardView->des_cs.SetParamInfo(param2);
					}
				}
			}
			else
			{	//普通三视图组焊板
				dcs_axis_x.Set();
				dcs_axis_y.Set();
				GEPOINT corneraxis=pRootPlate->ucs.axis_z^weldinfo.pBuddyPlate->ucs.axis_z;
				normalize(corneraxis);
				if(fabs(corneraxis.z)<0.707)
				{	//增加俯视图
					if(fabs(corneraxis.x)>fabs(corneraxis.y))
						dcs_axis_x=corneraxis.x>0?corneraxis:-corneraxis;
					else
						dcs_axis_x=corneraxis.y<0?corneraxis:-corneraxis;
					vector_trans(dcs_axis_x,pRootPlate->ucs,FALSE,TRUE);
					normalize(dcs_axis_x);
					param.desAxisX.nearVector.Set(dcs_axis_x.x,dcs_axis_x.y);
					param.m_eDatumPlane=CParaCS::DP_ZX;
					pSectionView->des_cs.SetParamInfo(param);
					pSectionView->des_cs.UpdateACS(pRootPlate->BelongModel());

					if(corneraxis*pSectionView->des_cs.GetACS().axis_x<0)
						corneraxis*=-1.0;
					param2.desAxisX.nearVector=corneraxis;
					vector_trans(param2.desAxisX.nearVector,weldinfo.pBuddyPlate->ucs,FALSE,TRUE);
					param2.desAxisX.nearVector.z=0;
					normalize(param2.desAxisX.nearVector);
					param2.desAxisX.hVicePart=param2.desAxisY.hVicePart=param2.desAxisZ.hVicePart=weldinfo.pBuddyPlate->handle;
					if(weldinfo.pBuddyPlate->ucs.axis_z*pSectionView->des_cs.GetACS().axis_y>0)
						param2.desAxisZ.nearVector.Set(0,0, 1);
					else
						param2.desAxisZ.nearVector.Set(0,0,-1);
					param2.desOrg.des_para.hPart=weldinfo.pBuddyPlate->handle;
					param2.m_eDatumPlane=CParaCS::DP_ZX;
					pDownwardView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
					pDownwardView->des_cs.SetParamInfo(param2);
				}
				else
				{	//增加左视图
					dcs_axis_y=corneraxis.z>0?-corneraxis:corneraxis;
					vector_trans(dcs_axis_y,pRootPlate->ucs,FALSE,TRUE);
					normalize(dcs_axis_y);
					param.desAxisY.nearVector.Set(dcs_axis_y.x,dcs_axis_y.y);
					param.m_eDatumPlane=CParaCS::DP_ZY;
					pSectionView->des_cs.SetParamInfo(param);
					pSectionView->des_cs.UpdateACS(pRootPlate->BelongModel());

					if(corneraxis*pSectionView->des_cs.GetACS().axis_y<0)
						corneraxis*=-1.0;
					param2.desAxisX.nearVector=pSectionView->des_cs.GetACS().axis_z;
					vector_trans(param2.desAxisX.nearVector,weldinfo.pBuddyPlate->ucs,FALSE,TRUE);
					param2.desAxisX.nearVector.z=0;
					normalize(param2.desAxisX.nearVector);
					if(pSectionView->des_cs.GetACS().axis_x*weldinfo.pBuddyPlate->ucs.axis_z<0)
						param2.desAxisZ.nearVector.Set(0,0, 1);
					else
						param2.desAxisZ.nearVector.Set(0,0,-1);
					param2.desAxisX.hVicePart=param2.desAxisY.hVicePart=param2.desAxisZ.hVicePart=weldinfo.pBuddyPlate->handle;
					param2.desOrg.des_para.hPart=weldinfo.pBuddyPlate->handle;
					param2.m_eDatumPlane=CParaCS::DP_ZX;
					pLeftView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
					pLeftView->des_cs.SetParamInfo(param2);
				}
			}
			if(weldinfo.cType==WELD_PLATES::WITH_TRIPPLE_PLATES)
			{	//三视图组焊板
				param2.desAxisZ.hVicePart=weldinfo.pSectPlate->handle;
				GEPOINT corneraxis=pRootPlate->ucs.axis_z^weldinfo.pSectPlate->ucs.axis_z;
				normalize(corneraxis);
				if(!dcs_axis_x.IsZero())
				{	//添加左视图
					if(pSectionView->des_cs.GetACS().axis_x*weldinfo.pSectPlate->ucs.axis_z>0)
						param2.desAxisZ.nearVector.Set(0,0,-1);
					else
						param2.desAxisZ.nearVector.Set(0,0, 1);
					if(corneraxis*pSectionView->des_cs.GetACS().axis_y<0)
						corneraxis*=-1.0;
					param2.desAxisY.nearVector=corneraxis;
					vector_trans(param2.desAxisY.nearVector,weldinfo.pSectPlate->ucs,FALSE,TRUE);
					param2.desAxisY.nearVector.z=0;
					normalize(param2.desAxisY.nearVector);
					param2.desAxisX.hVicePart=param2.desAxisY.hVicePart=param2.desAxisZ.hVicePart=weldinfo.pSectPlate->handle;
					param2.desOrg.des_para.hPart=weldinfo.pSectPlate->handle;
					pLeftView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
					param2.m_eDatumPlane=CParaCS::DP_ZY;
					pLeftView->des_cs.SetParamInfo(param2);
				}
				else
				{	//添加俯视图
					if(pSectionView->des_cs.GetACS().axis_y*weldinfo.pSectPlate->ucs.axis_z<0)
						param2.desAxisZ.nearVector.Set(0,0,-1);
					else
						param2.desAxisZ.nearVector.Set(0,0, 1);
					if(corneraxis*pSectionView->des_cs.GetACS().axis_x<0)
						corneraxis*=-1.0;
					param2.desAxisX.nearVector=corneraxis;
					vector_trans(param2.desAxisX.nearVector,weldinfo.pSectPlate->ucs,FALSE,TRUE);
					param2.desAxisX.nearVector.z=0;
					normalize(param2.desAxisX.nearVector);
					param2.desAxisX.hVicePart=param2.desAxisY.hVicePart=param2.desAxisZ.hVicePart=weldinfo.pSectPlate->handle;
					param2.desOrg.des_para.hPart=weldinfo.pSectPlate->handle;
					pDownwardView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
					param2.m_eDatumPlane=CParaCS::DP_ZX;
					pDownwardView->des_cs.SetParamInfo(param2);
				}
			}
		}
		pSectionView->des_cs.UpdateACS(pRootPlate->BelongModel());
		pSectionView->ucs=pSectionView->des_cs.GetACS();
		if(pLeftView)
		{
			pLeftView->des_cs.UpdateACS(pRootPlate->BelongModel());
			pLeftView->ucs=pLeftView->des_cs.GetACS();
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pLeftView->sName,"%s","Side View");
#else
			sprintf(pLeftView->sName,"%s","侧视图");
#endif
		}
		if(pDownwardView)
		{
			pDownwardView->des_cs.UpdateACS(pRootPlate->BelongModel());
			pDownwardView->ucs=pDownwardView->des_cs.GetACS();
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pDownwardView->sName,"%s","Downward View");
#else
			sprintf(pDownwardView->sName,"%s","俯视图");
#endif
		}
		pSectionView->AddPart(pRootPlate->handle,pRootPlate);
		if(pLeftView)
			pLeftView->AddPart(pRootPlate->handle,pRootPlate);
		if(pDownwardView)
			pDownwardView->AddPart(pRootPlate->handle,pRootPlate);
		for(CLDSPart* pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(pSectionView)
				pSectionView->AddPart(pPart->handle,pPart);
			if(pLeftView)
				pLeftView->AddPart(pPart->handle,pPart);
			if(pDownwardView)
				pDownwardView->AddPart(pPart->handle,pPart);
		}
		//添加靠模角钢视图
		CHashList<KAOMEANGLE> hashKaoMe;
		RetrieveKaoMeAngle(hashKaoMe);
		for(KAOMEANGLE *pKaoMeAngle=hashKaoMe.GetFirst();pKaoMeAngle;pKaoMeAngle=hashKaoMe.GetNext())
		{
			if(pKaoMeAngle->pBoltsInWingY==NULL||pKaoMeAngle->pBoltsInWingX==NULL)
				continue;
			if(pKaoMeAngle->pAngle==NULL)
				pKaoMeAngle->pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pKaoMeAngle->hAngle,CLS_LINEANGLE);
			if(pKaoMeAngle->pAngle==NULL)
				continue;
			CKaoMeAngleSectionView *pKmAngleView = (CKaoMeAngleSectionView*)SonViews.Add(0,CSectionView::CLS_KAOME_ANGLE_VIEW);
		#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pKmAngleView->sName,"weld mould angle-%s",(char*)pKaoMeAngle->pAngle->GetPartNo());
		#else
			sprintf(pKmAngleView->sName,"靠模角钢-%s",(char*)pKaoMeAngle->pAngle->GetPartNo());
		#endif
			param=pKmAngleView->des_cs.GetParamInfo();
			param.desOrg.datum_pos_style=9;
			param.desOrg.des_para.hPart=pRootPlate->handle;
			param.desOrg.datum_pos.Set();
			//
			param.m_eDatumPlane=CParaCS::DP_ZY;
			param.desAxisZ.norm_style=5;
			param.desAxisZ.hVicePart=pKaoMeAngle->pAngle->handle;
			param.desAxisZ.direction=0;
			param.desAxisY.norm_style=1;
			param.desAxisY.hVicePart=pKaoMeAngle->pAngle->handle;
			pKmAngleView->des_cs.SetParamInfo(param);
			pKmAngleView->UpdateUCS(m_pModel,this);
			pKmAngleView->InitView(*pKaoMeAngle);
			pKmAngleView->AddPart(pKaoMeAngle->pAngle->handle,pKaoMeAngle->pAngle);
			if(pKaoMeAngle->pPlateInWingX)
				pKmAngleView->AddPart(pKaoMeAngle->pPlateInWingX->handle,pKaoMeAngle->pPlateInWingX);
			if(pKaoMeAngle->pPlateInWingY)
				pKmAngleView->AddPart(pKaoMeAngle->pPlateInWingY->handle,pKaoMeAngle->pPlateInWingY);
			CLDSBolt *pBolt=NULL;
			for(pBolt=pKaoMeAngle->pBoltsInWingX->GetFirst();pBolt;pBolt=pKaoMeAngle->pBoltsInWingX->GetNext())
				pKmAngleView->AddPart(pBolt->handle,pBolt);
			for(pBolt=pKaoMeAngle->pBoltsInWingY->GetFirst();pBolt;pBolt=pKaoMeAngle->pBoltsInWingY->GetNext())
				pKmAngleView->AddPart(pBolt->handle,pBolt);
		}
	}
	return TRUE;
}
BOOL CLDSPartGroup::AddStdXPlatesViews(CLDSPart* pDatumPart,PARTSET& partset)
{
	STDX_PLATES_PARTGROUP stdXPlatesPartGroup;
	if(!IdentifyStdXPlates(&stdXPlatesPartGroup))
		return FALSE;
	CLDSPlate* platePtrArr[3]={stdXPlatesPartGroup.m_pBasePlate,stdXPlatesPartGroup.m_pBackVertPlate,stdXPlatesPartGroup.m_pBasePlate};
	for(int i=0;i<3;i++)
	{	
		CStdXPlatesView *pSectionView = (CStdXPlatesView*)SonViews.Add(0,CSectionView::CLS_STDX_PLATES_VIEW);
		pSectionView->m_hBasePlate=stdXPlatesPartGroup.m_pBasePlate->handle;
		pSectionView->m_hFrontVertPlate=stdXPlatesPartGroup.m_pFrontVertPlate->handle;
		pSectionView->m_hBackVertPlate=stdXPlatesPartGroup.m_pBackVertPlate->handle;
		for(int j=0;j<4;j++)
		{
			if(stdXPlatesPartGroup.m_arrAnglePtr[j]==NULL)
			{
				pSectionView->m_arrAngleHandle[j]=0;
				continue;
			}
			pSectionView->m_arrAngleHandle[j]=stdXPlatesPartGroup.m_arrAnglePtr[j]->handle;
			if(i==2)	//端面图需要添加角钢
				pSectionView->AddPart(stdXPlatesPartGroup.m_arrAnglePtr[j]->handle,stdXPlatesPartGroup.m_arrAnglePtr[j]);		
		}
		pSectionView->AddPart(stdXPlatesPartGroup.m_pBasePlate->handle,stdXPlatesPartGroup.m_pBasePlate);
		pSectionView->AddPart(stdXPlatesPartGroup.m_pFrontVertPlate->handle,stdXPlatesPartGroup.m_pFrontVertPlate);
		pSectionView->AddPart(stdXPlatesPartGroup.m_pBackVertPlate->handle,stdXPlatesPartGroup.m_pBackVertPlate);
		for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			pSectionView->AddPart(pPart->handle,pPart);
		//
		if(i==2)
		{	//在俯视图中添加需要生成靠模角钢的角钢,m_arrAngleHandle仅能记录一侧的关联角钢
			//接头位置上下侧角钢均需要生成靠模角钢，此处将另外一侧需要生成靠模角钢的角钢添加至俯视图 wht 15-10-30
			CHashList<KAOMEANGLE> hashKaoMe;
			RetrieveKaoMeAngle(hashKaoMe);
			for(KAOMEANGLE *pKaoMeAngle=hashKaoMe.GetFirst();pKaoMeAngle;pKaoMeAngle=hashKaoMe.GetNext())
			{
				if(pKaoMeAngle->pBoltsInWingY==NULL||pKaoMeAngle->pBoltsInWingX==NULL)
					continue;
				if(pKaoMeAngle->pAngle==NULL)
					pKaoMeAngle->pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pKaoMeAngle->hAngle,CLS_LINEANGLE);
				if( pKaoMeAngle->pAngle==NULL||
					pKaoMeAngle->pAngle->handle==pSectionView->m_arrAngleHandle[0]||
					pKaoMeAngle->pAngle->handle==pSectionView->m_arrAngleHandle[1]||
					pKaoMeAngle->pAngle->handle==pSectionView->m_arrAngleHandle[2]||
					pKaoMeAngle->pAngle->handle==pSectionView->m_arrAngleHandle[3])
					continue;
				pSectionView->AddPart(pKaoMeAngle->pAngle->handle,pKaoMeAngle->pAngle);
			}
		}
		//设置视图坐标系参数
		CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
		if(i==0)		//1.正视图(以基准钢板所在面为基准面)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pSectionView->sName,"%s","Front View");
#else
			sprintf(pSectionView->sName,"%s","正面图");
#endif
			pSectionView->m_cSubType='F';
		}
		else if(i==1)	//2.侧面图
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pSectionView->sName,"%s","Side View");
#else
			sprintf(pSectionView->sName,"%s","侧面图");
#endif
			pSectionView->m_cSubType='S';
		}
		else			//3.俯视图
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pSectionView->sName,"%s","Downward View");
#else
			sprintf(pSectionView->sName,"%s","俯视图");
#endif
			pSectionView->m_cSubType='D';
		}
		//坐标原点
		param.desOrg.datum_pos_style=9;
		param.desOrg.des_para.hPart=platePtrArr[i]->handle;
		param.desOrg.datum_pos.Set();
		//Z轴方向
		f3dPoint verify_vec(platePtrArr[i]->ucs.origin.x,platePtrArr[i]->ucs.origin.y,0);	//铁塔中心指向钢板的方向
		normalize(verify_vec);
		param.m_eDatumPlane=CParaCS::DP_ZY;
		param.desAxisZ.norm_style=3;	//构件法线方向
		param.desAxisZ.hVicePart=platePtrArr[i]->handle;
		if(verify_vec*platePtrArr[i]->ucs.axis_z>0)
			param.desAxisZ.direction=0;	//与法线同向
		else 
			param.desAxisZ.direction=1;	//与法线反向
		//Y轴方向
		verify_vec=stdXPlatesPartGroup.m_arrAnglePtr[0]->End()-stdXPlatesPartGroup.m_arrAnglePtr[0]->Start();
		normalize(verify_vec);
		param.desAxisY.norm_style=5;	//杆件特征方向
		param.desAxisY.hVicePart=stdXPlatesPartGroup.m_arrAnglePtr[0]->handle;
		if(verify_vec.z>0)
			param.desAxisY.direction=1;	//1.终->始 ;
		else 
			param.desAxisY.direction=0;	//0.始->终
		if(i==1)
		{	//侧视图(在正视图基础上从左向右看)
			CStdXPlatesView *pMainView=(CStdXPlatesView*)SonViews.GetFirst();
			if(pMainView->ucs.axis_x*platePtrArr[i]->ucs.axis_z<0)
				param.desAxisZ.direction=0;	//与法线同向
			else 
				param.desAxisZ.direction=1;	//与法线反向
		}
		else if(i==2)
		{	//俯视图
			CLDSVector tempVec=param.desAxisY;
			param.desAxisY=param.desAxisZ;
			param.desAxisZ=tempVec;
			if(param.desAxisY.direction==0)
				param.desAxisY.direction=1;
			else
				param.desAxisY.direction=0;
		}
		pSectionView->des_cs.SetParamInfo(param);
		pSectionView->UpdateUCS(m_pModel,this);
	}
	return TRUE;
}
BOOL CLDSPartGroup::AddAngleFootViews(CLDSPart* pDatumPart,PARTSET& partset)
{
	ANGLE_FOOT_PARTGROUP foot,footDown;	//foots[0]上层塔脚信息，foots[1]牛蹄板下层塔脚信息

	if(!IdentifyAngleFoot(&foot)||foot.m_arrAnglePtr[0]==NULL)
		return FALSE;
	f3dPoint verify_vec;
	bool ishoof=IdentifyAngleFoot(&footDown,false,foot.m_pShoePlate->ucs.axis_z);	//判断是否为牛蹄板
	CLDSGeneralPlate* platePtrArr[4]={foot.m_pShoePlate,foot.m_pBackShoePlate,foot.m_pShoePlate,foot.m_pShoePlate};
	for(int i=0;i<4;i++)
	{	//视图依次为正视图、侧(左)视图、俯视图与牛蹄板仰视图　wjh-2015.4.26
		if(i==3&&!ishoof)
			break;	//非牛蹄板不需要仰视图
		CAngleFootView *pSectionView = (CAngleFootView*)SonViews.Add(0,CSectionView::CLS_ANGLE_FOOT_VIEW);
		pSectionView->m_hFootPlate=foot.m_pFootPlate->handle;
		pSectionView->m_hShoePlate=foot.m_pShoePlate->handle;
		if(foot.m_pRightShoePlate)
			pSectionView->m_hRightShoePlate=foot.m_pRightShoePlate->handle;
		pSectionView->m_hFrontShoePlate=foot.m_pFrontShoePlate->handle;
		pSectionView->m_hBackShoePlate=foot.m_pBackShoePlate->handle;
		for(int j=0;j<4;j++)
		{
			if(foot.m_arrAnglePtr[j]==NULL)
			{
				pSectionView->m_arrAngleHandle[j]=0;
				continue;
			}
			pSectionView->m_arrAngleHandle[j]=foot.m_arrAnglePtr[j]->handle;
			if(i==2)	//端面图需要添加角钢
				pSectionView->AddPart(foot.m_arrAnglePtr[j]->handle,foot.m_arrAnglePtr[j]);		
		}
		if(ishoof)
		{
			if(footDown.m_pShoePlate)
				pSectionView->m_hDownLeftShoePlate=footDown.m_pShoePlate->handle;
			if(footDown.m_pRightShoePlate)
				pSectionView->m_hDownRightShoePlate=footDown.m_pRightShoePlate->handle;
			if(footDown.m_pFrontShoePlate)
				pSectionView->m_hDownFrontShoePlate=footDown.m_pFrontShoePlate->handle;
			if(footDown.m_pBackShoePlate)
				pSectionView->m_hDownBackShoePlate=footDown.m_pBackShoePlate->handle;
			for(int j=0;j<4;j++)
			{
				if(footDown.m_arrAnglePtr[j]==NULL)
				{
					pSectionView->m_arrDownAngleHandle[j]=0;
					continue;
				}
				pSectionView->m_arrDownAngleHandle[j]=footDown.m_arrAnglePtr[j]->handle;
				if(i==3)	//端面图需要添加角钢
					pSectionView->AddPart(footDown.m_arrAnglePtr[j]->handle,footDown.m_arrAnglePtr[j]);		
			}
		}
		pSectionView->AddPart(foot.m_pFootPlate->handle,foot.m_pFootPlate);
		for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(i<2)
				pSectionView->AddPart(pPart->handle,pPart);
			else
			{
				bool upperPlates=true;
				if(pPart->IsPlate())
				{
					f3dPoint geomcenter;
					CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pPart;
					for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
						geomcenter+=pVertex->vertex;
					geomcenter/=pPlate->vertex_list.GetNodeNum();
					coord_trans(geomcenter,pPlate->ucs,TRUE,TRUE);
					verify_vec=geomcenter-foot.m_pFootPlate->ucs.origin;
					if(verify_vec*foot.m_pFootPlate->ucs.axis_z<foot.m_pFootPlate->GetNormOffset()+foot.m_pFootPlate->GetThick()/2)
						upperPlates=false;	//下侧板
				}
				if(i==2&&upperPlates)	//俯视图只需要添加上侧板(靴板、肋板)
					pSectionView->AddPart(pPart->handle,pPart);
				else if(i==3&&!upperPlates)
					pSectionView->AddPart(pPart->handle,pPart);
			}
		}
		//设置视图坐标系参数
		CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
		if(i==0)		//1.正视图(以基准钢板所在面为基准面)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pSectionView->sName,"%s","Front View");
#else
			sprintf(pSectionView->sName,"%s","正面图");
#endif
			pSectionView->m_cSubType='F';
		}
		else if(i==1)	//2.侧面图
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pSectionView->sName,"%s","Side View");
#else
			sprintf(pSectionView->sName,"%s","侧面图");
#endif
			pSectionView->m_cSubType='S';
		}
		else if(i==2)		//3.俯视图
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pSectionView->sName,"%s","Top View");
#else
			sprintf(pSectionView->sName,"%s","俯视图");
#endif
			pSectionView->m_cSubType='D';	//Downward View
		}
		else if(i==3)		//4.仰视图
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pSectionView->sName,"%s","Bottom View");
#else
			sprintf(pSectionView->sName,"%s","仰视图");
#endif
			pSectionView->m_cSubType='U';	//Upward View
		}
		//坐标原点
		param.desOrg.datum_pos_style=9;
		param.desOrg.des_para.hPart=foot.m_pFootPlate->handle;
		param.desOrg.datum_pos.Set();
		if(i==3) 
			param.desOrg.datum_pos.Set(0,0,foot.m_pFootPlate->GetNormOffset());
		else //if(i==2)
			param.desOrg.datum_pos.Set(0,0,foot.m_pFootPlate->GetNormOffset()+foot.m_pFootPlate->GetThick());
		//Z轴方向
		verify_vec.Set(platePtrArr[i]->ucs.origin.x,platePtrArr[i]->ucs.origin.y,0);	//铁塔中心指向钢板的方向
		normalize(verify_vec);
		param.m_eDatumPlane=CParaCS::DP_YZ;
		param.desAxisZ.norm_style=3;	//构件法线方向
		param.desAxisZ.hVicePart=platePtrArr[i]->handle;
		if(verify_vec*platePtrArr[i]->ucs.axis_z>0)
			param.desAxisZ.direction=0;	//与法线同向
		else 
			param.desAxisZ.direction=1;	//与法线反向
		//Y轴方向
		verify_vec=foot.m_arrAnglePtr[0]->End()-foot.m_arrAnglePtr[0]->Start();
		normalize(verify_vec);
		param.desAxisY.norm_style=3;	//构件法线方向
		param.desAxisY.hVicePart=foot.m_pFootPlate->handle;
		if(foot.m_pFootPlate->ucs.axis_z.z>0)
			param.desAxisY.direction=1;	//与法线同向
		else 
			param.desAxisY.direction=0;	//与法线反向
		if(i==1)
		{	//侧视图(在正视图基础上从左向右看)
			CAngleFootView *pMainView=(CAngleFootView*)SonViews.GetFirst();
			if(pMainView->ucs.axis_x*platePtrArr[i]->ucs.axis_z<0)
				param.desAxisZ.direction=0;	//与法线同向
			else 
				param.desAxisZ.direction=1;	//与法线反向
		}
		else if(i>=2)
		{	//俯视图
			param.m_eDatumPlane=CParaCS::DP_ZY;
			CLDSVector tempVec=param.desAxisY;
			param.desAxisY=param.desAxisZ;
			param.desAxisZ=tempVec;
			if(i==3)
			{
				param.desAxisY.ReverseDirection();
				param.desAxisZ.ReverseDirection();
			}
			if(param.desAxisY.direction==0)
				param.desAxisY.direction=1;
			else
				param.desAxisY.direction=0;
		}
		pSectionView->des_cs.SetParamInfo(param);
		pSectionView->UpdateUCS(m_pModel,this);
	}
	return TRUE;
}
void CLDSPartGroup::RetrieveKaoMeAngle(CHashList<KAOMEANGLE> &hashKaoMe)
{
	PARTSET partset;
	IModel *pModel=BelongModel();
	pModel->GetPartGroupPartSet(handle,partset);
	CLDSPart* pPart;
	CLDSBolt* pBolt;
	//统计该组焊件上所有钢板上的连接螺栓集合
	BOLTSET boltset;
	CHashList<CLDSGeneralPlate*> hashPlatePtrByBoltHandle;
	if(partset.FromHandle(handle)==NULL)
	{
		pPart=pModel->FromPartHandle(handle);
		if(pPart)
			partset.append(pPart);
	}
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		CLsRefList* pLsRefList=NULL;
		if(!pPart->IsPlate() || (pPart->IsPlate() && (pLsRefList=pPart->GetLsRefList())==NULL))
			continue;
		for(CLsRef* pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
		{
			pBolt=pLsRef->GetLsPtr();
			boltset.append(pLsRef->GetLsPtr());
			hashPlatePtrByBoltHandle.SetValue(pBolt->handle,(CLDSGeneralPlate*)pPart);
		}
	}
	//遍历角钢查找关联角钢
	CLDSLineAngle* pAngle;
	for(pAngle=(CLDSLineAngle*)pModel->EnumPartFirst(CLS_LINEANGLE);pAngle;pAngle=(CLDSLineAngle*)pModel->EnumPartNext(CLS_LINEANGLE))
	{
		if(pAngle->pStart==NULL||pAngle->pEnd==NULL)
			continue;	//不考虑短角钢
		f3dPoint norm_x_wing=pAngle->get_norm_x_wing();
		f3dPoint norm_y_wing=pAngle->get_norm_y_wing();
		KAOMEANGLE* pKaoMe=hashKaoMe.GetValue(pAngle->handle);
		for(pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
		{
			if(pAngle->FindLsByHandle(pBolt->handle)==NULL)
				continue;	//该螺栓不在该角钢上
			double ddx=pBolt->get_norm()*norm_x_wing;
			double ddy=pBolt->get_norm()*norm_y_wing;
			if(pKaoMe==NULL)
			{
				pKaoMe=hashKaoMe.Add(pAngle->handle);
				pKaoMe->hAngle=pAngle->handle;
				pKaoMe->pAngle=pAngle;
			}
			CLDSGeneralPlate **ppPlate=hashPlatePtrByBoltHandle.GetValue(pBolt->handle);
			if( fabs(ddx)>fabs(ddy))	//X肢螺栓
			{
				if(pKaoMe->pBoltsInWingX==NULL)
					pKaoMe->pBoltsInWingX=new BOLTSET;
				pKaoMe->pBoltsInWingX->append(pBolt);
				if(ppPlate)
					pKaoMe->pPlateInWingX=*ppPlate;
			}
			else
			{
				if(pKaoMe->pBoltsInWingY==NULL)
					pKaoMe->pBoltsInWingY=new BOLTSET;
				pKaoMe->pBoltsInWingY->append(pBolt);
				if(ppPlate)
					pKaoMe->pPlateInWingY=*ppPlate;
			}
		}
	}
	for(KAOMEANGLE *pKaoMe=hashKaoMe.GetFirst();pKaoMe;pKaoMe=hashKaoMe.GetNext())
	{
		if(pKaoMe->pBoltsInWingX==NULL||pKaoMe->pBoltsInWingY==NULL)
			hashKaoMe.DeleteCursor();
	}
}
//7.添加主材为角钢的组焊件视图
BOOL CLDSPartGroup::AddAnglePartGroupViews(CLDSLineAngle *pLineAngle,PARTSET& partset)
{	
	int view_index=1;
	for(int i=0;i<2;i++)
	{	
		CSectionView *pSectionView = NULL;
		if(i==0)
			pSectionView=SonViews.Add(0,CSectionView::CLS_ANGLE_SPREAD_VIEW);
		else
			pSectionView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
		pSectionView->index=view_index++;
		pSectionView->AddPart(pLineAngle->handle,pLineAngle);
		for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			pSectionView->AddPart(pPart->handle,pPart);
		if(i==0)		//1.正视图(以基准钢板所在面为基准面)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pSectionView->sName,"%s","Front View");
#else
			sprintf(pSectionView->sName,"%s","正面图");
#endif
		}
		else //if(i==1)	//2.侧面图
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pSectionView->sName,"%s","Downward View");
#else
			sprintf(pSectionView->sName,"%s","俯视图");
#endif
		}
		//设置视图坐标系参数
		CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
		//坐标原点
		param.desOrg.datum_pos_style=9;
		param.desOrg.des_para.hPart=pLineAngle->handle;
		param.desOrg.datum_pos.Set();
		//Z轴方向
		param.m_eDatumPlane=CParaCS::DP_ZX;
		param.desAxisZ.norm_style=5;	//杆件特征方向
		param.desAxisZ.hVicePart=pLineAngle->handle;
		param.desAxisZ.direction=5;		//角钢norm_y-方向
		//X轴方向
		param.desAxisX.norm_style=5;
		param.desAxisX.hVicePart=pLineAngle->handle;
		param.desAxisX.direction=0;		//角钢从始->终方向
		if(i==1)
		{	//俯视图
			param.desAxisZ.direction=0;	//角钢从始->终方向
			param.desAxisX.direction=2;	//角钢norm_x+方向
		}
		pSectionView->des_cs.SetParamInfo(param);
		pSectionView->UpdateUCS(m_pModel,this);
	}
	return TRUE;
}
//添加含套管挂板组焊件视图
BOOL CLDSPartGroup::AddBushingPlateView(CLDSPart* pDatumPart,PARTSET& partset)
{
	if(pDatumPart->GetClassTypeId()!=CLS_PLATE)
		return FALSE;
	//添加正视图
	CLDSPlate* pDatumPlate=(CLDSPlate*)pDatumPart;
	CSectionView *pSectionView=NULL;
	GEPOINT dcs_axis_x,dcs_axis_y;	//主视图绘图坐标系的X轴及Y轴方向（基准钢板局部相对坐标）
	if(pDatumPlate->face_N<=1)
	{
		if(fabs(pDatumPart->ucs.axis_z.z)>0.707)	//更接近水平的钢板
		{
			if(fabs(pDatumPart->ucs.axis_y.y)>fabs(pDatumPart->ucs.axis_y.x))	//前后沿向的钢板出俯视图
			{
				pDatumPart->ucs.axis_y.y>0 ? dcs_axis_y.Set(0,-1,0) : dcs_axis_y.Set(0,1,0);
				if(pDatumPart->ucs.axis_z.z>0)	
					dcs_axis_y*=-1;	//主视图实际是模型环境下的仰视图
			}
			else	//左右沿向的钢板出左视图
			{
				pDatumPart->ucs.axis_y.x>0 ? dcs_axis_x.Set(1,0,0) : dcs_axis_x.Set(-1,0,0);
				dcs_axis_x.Set(1,0,0);
			}
		}
		else if(fabs(pDatumPart->ucs.axis_x.z)>fabs(pDatumPart->ucs.axis_y.z))
			pDatumPart->ucs.axis_x.z>0?dcs_axis_y.Set(-1,0,0):dcs_axis_y.Set(1,0,0);
		else
			pDatumPart->ucs.axis_y.z>0?dcs_axis_y.Set(0,-1,0):dcs_axis_y.Set(0,1,0);
	}
	else //if(pDatumPlate->face_N>1)
	{
		GEPOINT huoquaxis=pDatumPlate->HuoQuLine[0].endPt-pDatumPlate->HuoQuLine[0].startPt;
		vector_trans(huoquaxis,pDatumPlate->ucs,TRUE,TRUE);
		normalize(huoquaxis);
		if(fabs(huoquaxis.z)>0.707)	//增加俯视图
			dcs_axis_y=huoquaxis.z>0?-huoquaxis:huoquaxis;
		else if(fabs(huoquaxis.x)>fabs(huoquaxis.y))	//与X轴更接近
			dcs_axis_x=huoquaxis.x>0?huoquaxis:-huoquaxis;
		else
			dcs_axis_x=huoquaxis.y<0?huoquaxis:-huoquaxis;
		if(!dcs_axis_y.IsZero())
			vector_trans(dcs_axis_y,pDatumPlate->ucs,FALSE,TRUE);
		if(!dcs_axis_x.IsZero())
			vector_trans(dcs_axis_x,pDatumPlate->ucs,FALSE,TRUE);
	}
	for(int i=0;i<2;i++)
	{
		pSectionView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
		CParaCS::PARAMETRIC_INFO param=pSectionView->des_cs.GetParamInfo();
		param.desOrg.datum_pos_style=9;
		param.desOrg.des_para.hPart=pDatumPart->handle;
		param.desOrg.datum_pos.Set();
		param.desAxisX.norm_style=param.desAxisY.norm_style=param.desAxisZ.norm_style=4;
		param.desAxisX.hVicePart=param.desAxisY.hVicePart=param.desAxisZ.hVicePart=pDatumPart->handle;
		if(i==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pSectionView->sName,"%s","Front View");
#else
			sprintf(pSectionView->sName,"%s","正视图");
#endif
			param.desAxisZ.nearVector.Set(0,0,1);
			if(!dcs_axis_y.IsZero())
			{
				param.m_eDatumPlane=CParaCS::DP_ZY;
				param.desAxisY.nearVector=dcs_axis_y;
			}
			else //if(!dcs_axis_x.IsZero())
			{
				param.m_eDatumPlane=CParaCS::DP_ZX;
				param.desAxisX.nearVector=dcs_axis_x;
			}
			if(pDatumPlate->face_N>1)
			{	//添加挂孔所在的火曲展开面
				CDrawingAssistant* pAssist=pSectionView->hashAssistPlanes.Add(1);
				pAssist->m_xFaceNorm.norm_style=3;
				pAssist->m_xFaceNorm.hVicePart=pDatumPlate->handle;
				pAssist->m_xFaceNorm.reside_plane=1;	//第一个火曲面
				pAssist->m_xFaceNorm.vector=pDatumPlate->huoqufacenorm[0];
				f3dPoint huoquaxis_pick;	//必须进行一次投影，否则可能会由于拾取点过远导致误差 wjh-2016.3.25
				SnapPerp(&huoquaxis_pick,pDatumPlate->HuoQuLine[0].startPt,pDatumPlate->HuoQuLine[0].endPt,f3dPoint(0,0,0));
				pAssist->m_xLineStart.datum_pos_style=9;
				pAssist->m_xLineStart.des_para.hPart=pDatumPlate->handle;
				pAssist->m_xLineStart.datum_pos=huoquaxis_pick;
				pAssist->m_xLineStart.UpdatePos(pDatumPlate->BelongModel());
			}
		}
		else
		{
			if(!dcs_axis_x.IsZero())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(pSectionView->sName,"%s","Side View");
#else
				sprintf(pSectionView->sName,"%s","侧视图");
#endif
				param.desAxisZ.nearVector=-dcs_axis_x;
				param.desAxisX.nearVector.Set(0,0,1);
				param.m_eDatumPlane=CParaCS::DP_XZ;
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(pSectionView->sName,"%s","Downward View");
#else
				sprintf(pSectionView->sName,"%s","俯视图");
#endif
				param.desAxisZ.nearVector=-dcs_axis_y;
				param.desAxisY.nearVector.Set(0,0,-1);
				param.m_eDatumPlane=CParaCS::DP_ZY;
			}
		}
		pSectionView->des_cs.SetParamInfo(param);
		pSectionView->UpdateUCS(m_pModel,this);
		pSectionView->AddPart(pDatumPart->handle,pDatumPart);
		CLDSPart* pPart;
		CLDSBolt* pRelyBolt=NULL;
		CLDSLineTube* pBushTube=NULL;
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(pPart->IsTube()&&pPart->FuncType()==CLDSLineTube::FUNC_WIREBUSH)
			{
				pBushTube=(CLDSLineTube*)pPart;
				pRelyBolt=(CLDSBolt*)BelongModel()->FromPartHandle(pBushTube->bush.hBushRelyBolt,CLS_BOLT);
			}
		}
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			CSectionView::PART_ITEM* pItemPart=pSectionView->AddPart(pPart->handle,pPart);
			if(i==0&&pPart!=pDatumPart&&pDatumPlate->face_N>1&&pBushTube&&pRelyBolt)
			{	//挂线板正面视图中设定火曲面上的套管及环板至火曲展开面
				if(pPart==pBushTube&&pRelyBolt&&pRelyBolt->des_work_norm.reside_plane==1)
					pItemPart->iResidePlaneNo=1;
				else if(pPart->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pPart)->designInfo.m_hBasePart==pBushTube->handle)
					pItemPart->iResidePlaneNo=pRelyBolt->des_work_norm.reside_plane;
			}
		}
	}
	return TRUE;
}
static bool ValidateLineTubeJointErrors(CLDSLineTube* pTube)
{
	bool needCorrectStart=false,needCorrectEnd=false;
	if(pTube->m_tJointStart.type>=2)
	{
		CLDSParamPlate* pTipPlate=(CLDSParamPlate*)pTube->BelongModel()->FromPartHandle(pTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		if(pTipPlate&&pTipPlate->m_hPartWeldParent!=pTube->handle)
			needCorrectStart=true;
	}
	if(pTube->m_tJointEnd.type>=2)
	{
		CLDSParamPlate* pTipPlate=(CLDSParamPlate*)pTube->BelongModel()->FromPartHandle(pTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		if(pTipPlate&&pTipPlate->m_hPartWeldParent!=pTube->handle)
			needCorrectEnd=true;
	}
	if(needCorrectStart||needCorrectEnd)
	{
		bool startCorrected=false,endCorrected=false;
		for(CLDSParamPlate* pTipPlate=(CLDSParamPlate*)pTube->BelongModel()->EnumPartFirst(CLS_PARAMPLATE);pTipPlate;
			pTipPlate=(CLDSParamPlate*)pTube->BelongModel()->EnumPartNext(CLS_PARAMPLATE))
		{
			if(pTipPlate->m_hPartWeldParent!=pTube->handle)
				continue;
			else if((!pTipPlate->IsFL()&&!pTipPlate->IsInsertPlate()))
				continue;
			double distToStart=DISTANCE(pTipPlate->ucs.origin,pTube->Start());
			double distToEnd=DISTANCE(pTipPlate->ucs.origin,pTube->End());
			if(needCorrectStart)
			{
				if(distToStart<distToEnd)
				{
					pTube->m_tJointStart.hLinkObj=pTipPlate->handle;
					startCorrected=true;
				}
			}
			if(needCorrectEnd)
			{
				if(distToStart>distToEnd)
				{
					pTube->m_tJointEnd.hLinkObj=pTipPlate->handle;
					endCorrected=true;
				}
			}
		}
		return startCorrected||endCorrected;
	}
	else
		return false;
}
typedef CSlaveSectionView* CSlaveSectionViewPtr;
static int _LocalCompareSectionViews(const CSlaveSectionViewPtr& ppView1,const CSlaveSectionViewPtr& ppView2)
{
	if(ppView1->location.x>ppView2->location.x)
		return  1;
	else if(ppView1->location.x<ppView2->location.x)
		return -1;
	else
		return 0;
}
BOOL CLDSPartGroup::AddDefaultAttachViews()
{
	int i=0;
	PARTSET partset;
	CSuperSmartPtr<CLDSPart>pDatumPart=BelongModel()->FromPartHandle(handle);
	if(pDatumPart.IsNULL())
		return FALSE;
	BOOL bRetCode=TRUE;
	strncpy(sRootPartNo,pDatumPart->GetPartNo(),16);
	sRootPartNo[16]=0;
	SmartIdentifyGroupType();
	BelongModel()->GetPartGroupPartSet(handle,partset);
	if(m_iPartGroupType==GROUP_STATION_BTMTRIBEAM)
		bRetCode=AddHorizBeamTubeView(pDatumPart,partset);
	else if(m_iPartGroupType==GROUP_DUALTUBE_JOINT)
		bRetCode=AddGeneralDualTubeJointView(pDatumPart,partset);
	else if(pDatumPart->IsLinePart())
	{
		if(pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
			pDatumPart.LineAnglePointer()->getUCS(pDatumPart->ucs);
		else if(pDatumPart->GetClassTypeId()==CLS_LINEFLAT)
			pDatumPart.LineFlatPointer()->BuildUCS();
		else if(pDatumPart->GetClassTypeId()==CLS_LINESLOT)
			pDatumPart.LineSlotPointer()->BuildUCS();
		else if(pDatumPart->GetClassTypeId()==CLS_LINETUBE)
		{
			ValidateLineTubeJointErrors(pDatumPart.LineTubePointer());
			pDatumPart.LineTubePointer()->BuildUCS();
		}
		//添加特殊部位组焊件视图
		bRetCode=FALSE;
		if(!bRetCode&&pDatumPart->GetClassTypeId()==CLS_LINETUBE)	//对接相贯钢管视图
			bRetCode=AddJointTubeViews(pDatumPart.LineTubePointer(),partset);
		if(!bRetCode&&pDatumPart->GetClassTypeId()==CLS_LINETUBE)	//四根支管相贯与主管的K节点类组焊件
			bRetCode=AddKNodeTransectViews(pDatumPart.LineTubePointer(),partset);
		if(!bRetCode&&pDatumPart->IsAngle())
			bRetCode=AddAnglePartGroupViews(pDatumPart.LineAnglePointer(),partset);
		if(!bRetCode)	//普通杆件为基准的组焊件
			bRetCode=AddCommonRodPartGroupViews(pDatumPart.LinePartPointer(),partset);
	}
	else if(m_iPartGroupType==GROUP_STDX_PLATES)
		return AddStdXPlatesViews(pDatumPart,partset);
	else if(m_iPartGroupType==GROUP_ANGLEFOOT)
		return AddAngleFootViews(pDatumPart,partset);
	//套管挂板视图，现在已确认不需要添加（之前添加仅是为了基于该视图方便修改出套管挂板视图） wht 18-09-22
	else if(m_iPartGroupType==GROUP_BUSHING)
		return AddBushingPlateView(pDatumPart,partset);
#ifdef __COMPLEX_PART_INC_
	else if(pDatumPart->GetClassTypeId()==CLS_SPHERE&&m_iPartGroupType==GROUP_SPHERE)
	{
		CSpherePartGroup extrator(this);
		return extrator.AddSpherePartGroupViews(pDatumPart.SpherePointer(),partset);
	}
#endif
	else if(pDatumPart->GetClassTypeId()==CLS_PLATE)
		return AddPlatePartGroupViews(pDatumPart.PlatePointer(),partset);
	else
		return FALSE;
	if( m_iPartGroupType==GROUP_STATION_BTMTRIBEAM||m_iPartGroupType==GROUP_DUALTUBE_JOINT||
		pDatumPart->IsLinePart())
	{
		char szViewSymbol[2]="A";
		ARRAY_LIST<CSlaveSectionView*> arrayViews;
		for(CSectionView* pView=SonViews.GetFirst();pView;pView=SonViews.GetNext())
		{
			if(pView->IsSlaveView())
				arrayViews.append((CSlaveSectionView*)pView);
		}
		CQuickSort<CSlaveSectionView*>::QuickSort(arrayViews.m_pData,arrayViews.GetSize(),_LocalCompareSectionViews);
		for(i=0;i<arrayViews.GetSize();i++)
		{
			CSlaveSectionView* pSlaveView=arrayViews[i];
			szViewSymbol[0]='A'+i;
			pSlaveView->symbol.Copy(szViewSymbol);
		}
	}
	return bRetCode;
}
//识别是否为角钢塔脚板
bool CLDSPartGroup::IdentifyAngleFoot(ANGLE_FOOT_PARTGROUP *pAngleFoot/*=NULL*/,bool upper/*=true*/,const double* front_direction/*=NULL*/)	
{
	CLDSGeneralPlate *pDatumPlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(handle,CLS_PLATE,CLS_PARAMPLATE);
	if(pDatumPlate==NULL)
		return false;
	//1.提取塔脚板底板及底板基准角钢
	long hDatumAngle=0;
	long hKaoMoAngleArr[2]={0,0};
	CHashSet<CLDSLineAngle*> hashAngleSet;	//连接组合角钢的子角钢集合
	if(pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE)
	{	//参数化塔脚板
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pDatumPlate;
		if(pParamPlate->m_iParamType!=TYPE_BASEPLATE)
			return false;
		if(pParamPlate->GetDesignItemValue(KEY2C("L"),&hDatumAngle)==NULL)
			return false;
	}
	else //if(pDatumPlate->GetClassTypeId()==CLS_PLATE)
	{	//普通钢板塔脚板
		CLDSPlate *pPlate=(CLDSPlate*)pDatumPlate;
		if(fabs(pPlate->ucs.axis_z*f3dPoint(0,0,1))<EPS_COS)
			return false;
		hDatumAngle=pPlate->designInfo.m_hBasePart;
	}
	CLDSGroupLineAngle *pGroupAngle=NULL;
	CLDSLineAngle *pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(hDatumAngle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if(pAngle==NULL)
		return false;
	CLDSNode* pBaseNode=pDatumPlate->GetBaseNode();
	f3dPoint lenVec=pAngle->Start()-pAngle->End();
	if(pBaseNode==pAngle->pStart)
		lenVec=pAngle->End()-pAngle->Start();
	if((upper&&lenVec.z>0) || (!upper&&lenVec.z<0))
	{	//需要重新查找定位上（下）主材
		f3dPoint lenStdVec=lenVec.normalized();
		CLDSLineAngle* pAdaptAngle=NULL;
		for(CLDSLinePart* pRod=BelongModel()->EnumRodFirst();pRod;pRod=BelongModel()->EnumRodNext())
		{
			if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->m_bVirtualPart)
				continue;
			else if(!pAngle->ModuleTogetherWith(pRod))
				continue;
			if(!pRod->IsAngle())
				continue;
			if(pRod->pStart==pBaseNode)
				lenVec=pRod->End()-pRod->Start();
			else if(pRod->pEnd==pBaseNode)
				lenVec=pRod->Start()-pRod->End();
			else
				continue;
			if((upper&&lenVec.z>=-EPS) || (!upper&&lenVec.z<=EPS))
				continue;	//不是当前需要的向上(下)延伸主材角钢
			normalize(lenVec);
			if(pRod->Layer(1)=='Z')
			{	//找到当前侧的主材角钢作为塔脚当前侧主材角钢 wjh-2015.7.24
				pAdaptAngle=(CLDSLineAngle*)pRod;
				break;
			}
		}
		pAngle=pAdaptAngle;
	}
	if(pAngle==NULL)
		return false;	//未找到合适的向上(下)延伸主材
	if(pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		pGroupAngle=(CLDSGroupLineAngle*)pAngle;
	else if(pAngle->group_father_jg_h>0x20)
		pGroupAngle=(CLDSGroupLineAngle*)BelongModel()->FromPartHandle(pAngle->group_father_jg_h,CLS_GROUPLINEANGLE);
	if(pGroupAngle)
	{
		hDatumAngle=pGroupAngle->handle;
		for(int i=0;i<4;i++)
		{
			CLDSLineAngle *pSonAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
			if(pSonAngle==NULL||pSonAngle->m_bVirtualPart)
				continue;
			if(!hashAngleSet.GetValue(pSonAngle->handle))
				hashAngleSet.SetValue(pSonAngle->handle,pSonAngle);
			if(pAngle==pGroupAngle)
				pAngle=pSonAngle;	//保证pAngle指向单角钢
		}
		hKaoMoAngleArr[0]=pGroupAngle->son_jg_h[0];
		if(pGroupAngle->group_style==2)
			hKaoMoAngleArr[1]=pGroupAngle->son_jg_h[2];
		else
			hKaoMoAngleArr[1]=pGroupAngle->son_jg_h[1];
	}
	else if(pAngle)
	{
		hDatumAngle=hKaoMoAngleArr[0]=pAngle->handle;
		hashAngleSet.SetValue(pAngle->handle,pAngle);
	}
	f3dPoint wing_x_norm=pAngle->get_norm_x_wing();
	f3dPoint wing_y_norm=pAngle->get_norm_y_wing();
	f3dPoint wing_x_depth_org=pAngle->End()-wing_x_norm*pAngle->GetThick();
	f3dPoint wing_y_depth_org=pAngle->End()-wing_y_norm*pAngle->GetThick();
	//2.遍历组焊件子构件提取靴板信息
	typedef CLDSGeneralPlate* CLDSPlatePtr;
	int nPlateCount=0;
	CLDSPlatePtr platePtrArr[4]={NULL};		//0.贯通的靴板(也可能为左右两块) 1.正面立板 2.侧面立板
	PARTSET partset;
	BelongModel()->GetPartGroupPartSet(handle,partset);
	for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_BOLT||!pPart->IsPlate()||pPart->handle==hDatumAngle)
			continue;		//跳过螺栓、非钢板构件及塔脚板底板
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			CDesignLjPartPara *pLjPara=NULL;
			f3dPoint neutral=pPlate->ucs.origin+(pPlate->GetNormOffset()+pPlate->GetThick()*0.5)*pPlate->ucs.axis_z;
			for(pLjPara=pPlate->GetFirstLjPara();pLjPara;pLjPara=pPlate->GetNextLjPara())
			{
				if(hashAngleSet.GetValue(pLjPara->hPart)||pLjPara->hPart==hDatumAngle)
					break;	//钢板连接角钢包括底板基准角钢
			}
			if(pLjPara==NULL)
			{	//增加单角钢塔脚的前侧截断板有可能无连接角钢情况的处理　wjh-2015.4.23
				double ddx=fabs(wing_x_norm*pPlate->ucs.axis_z);
				double ddy=fabs(wing_y_norm*pPlate->ucs.axis_z);
				double depth_x=(neutral-wing_x_depth_org)*wing_x_norm;
				double depth_y=(neutral-wing_y_depth_org)*wing_y_norm;
				if( (ddx<EPS_COS2&&ddy<EPS_COS2)||
					(ddx>=EPS_COS2&&(depth_x<0||(depth_x>pAngle->GetThick()+pPlate->GetThick())))||
					(ddy>=EPS_COS2&&(depth_y<0||(depth_y>pAngle->GetThick()+pPlate->GetThick()))))
					continue;	//钢板中心基点超出正常靴板范围或法线与角钢肢不贴合，不是靴板
			}
		}
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
			if(pParamPlate->m_iParamType!=TYPE_SHOEPLATE)
				continue;
			long h=0;
			//判断靴板基准角钢与当前基准角钢是否一致
			if(!pParamPlate->GetDesignItemValue('D',&h)||(h!=hDatumAngle&&!hashAngleSet.GetValue(h)))
				continue;
		}
		if(nPlateCount>4)	
			return false;	//塔脚板最多有四块靴板
		if(nPlateCount==4)
		{
			Log2File()->Log("0X%X底板筛选出４个以上的靴板，数据有误!",pDatumPlate->handle);
			return false;
		}
		platePtrArr[nPlateCount++]=(CLDSGeneralPlate*)pPart;
	}
	if(platePtrArr[0]==NULL||platePtrArr[1]==NULL||platePtrArr[2]==NULL)
		return false;	//在组焊件构件集合中未找到三块钢板
	//3.从三块靴板中提取出贯通靴板、前侧靴板和后侧靴板(法线方向一致的两块板为前后侧截断的靴板其余一块为通板)
	CLDSGeneralPlate *pShoePlate=NULL,*pRightShoePlate=NULL,*pFrontShoePlate=NULL,*pBackShoePlate=NULL;
	GEPOINT directToFront(front_direction);
	if(!directToFront.IsZero())
	{
		normalize(directToFront);
		for(int i=0;i<4;i++)
		{
			if(platePtrArr[i]==NULL)
				continue;
			double dd=platePtrArr[i]->ucs.axis_z*directToFront;
			if(fabs(dd)>0.707)	//钢板法线与前侧基准方向夹角<45°才可能是基面钢板
			{
				if(pShoePlate==NULL)
					pShoePlate=platePtrArr[i];
				else
					pRightShoePlate=platePtrArr[i];
			}
			else
			{	//立板
				if(pFrontShoePlate==NULL)
					pFrontShoePlate=platePtrArr[i];
				else
					pBackShoePlate=platePtrArr[i];
			}
		}
	}
	else
	{
		double dd1=fabs(platePtrArr[0]->ucs.axis_z*platePtrArr[1]->ucs.axis_z);
		double dd2=fabs(platePtrArr[0]->ucs.axis_z*platePtrArr[2]->ucs.axis_z);
		double dd3=fabs(platePtrArr[1]->ucs.axis_z*platePtrArr[2]->ucs.axis_z);
		if(dd1>EPS_COS2&&dd2<EPS_COS2)
		{	//0、1法线相同为立板，2为基板
			pShoePlate=platePtrArr[2];
			pFrontShoePlate=platePtrArr[0];
			pBackShoePlate=platePtrArr[1];
		}
		else if(dd2>EPS_COS2&&dd3<EPS_COS2)
		{	//0、2法线相同为立板，1为基板
			pShoePlate=platePtrArr[1];
			pFrontShoePlate=platePtrArr[0];
			pBackShoePlate=platePtrArr[2];
		}
		else if(dd3>EPS_COS2&&dd1<EPS_COS2)
		{	//1、2法线相同为立板，0为基板
			pShoePlate=platePtrArr[0];
			pFrontShoePlate=platePtrArr[1];
			pBackShoePlate=platePtrArr[2];
		}
		else 
			return false;
		if(platePtrArr[3]&&fabs(platePtrArr[3]->ucs.axis_z*pShoePlate->ucs.axis_z)<EPS_COS2)
			return false;	//存在第四块靴板时，第四块靴板法线方向与pShoePlate必定一致
		else 
			pRightShoePlate=platePtrArr[3];
	}
	if(pAngleFoot)
	{	
		pAngleFoot->m_pFootPlate=pDatumPlate;			//塔脚底板
		pAngleFoot->m_pShoePlate=pShoePlate;			//贯通的靴板
		if(pRightShoePlate)							
			pAngleFoot->m_pRightShoePlate=pRightShoePlate;
		pAngleFoot->m_pFrontShoePlate=pFrontShoePlate;	//截断的靴板
		pAngleFoot->m_pBackShoePlate=pBackShoePlate;	//截断的靴板
		//3.1 前后靴板划分
		f3dPoint front_vec=pShoePlate->ucs.axis_z;
		//塔脚板自最早期的TMA开始都是底板X+Y-作为外侧的　wjh-2015.4.23
		f3dPoint verify_vec=pDatumPlate->ucs.axis_x-pDatumPlate->ucs.axis_y;
		if(!directToFront.IsZero())	//指定前侧基准方向
			verify_vec=directToFront;
		if(verify_vec*pShoePlate->ucs.axis_z<0)
			front_vec*=-1;
		f3dPoint geom_center;
		for(PROFILE_VERTEX *pVertex=pFrontShoePlate->vertex_list.GetFirst();pVertex;pVertex=pFrontShoePlate->vertex_list.GetNext())
		{
			f3dPoint vertex=pVertex->vertex;
			coord_trans(vertex,pFrontShoePlate->ucs,TRUE);
			geom_center+=vertex;
		}
		geom_center/=pFrontShoePlate->vertex_list.GetNodeNum();
		f3dPoint plate_vec1=geom_center-pShoePlate->ucs.origin;
		normalize(plate_vec1);
		if(plate_vec1*front_vec<0)
		{
			pAngleFoot->m_pFrontShoePlate=pBackShoePlate;
			pAngleFoot->m_pBackShoePlate=pFrontShoePlate;
		}
		//3.2 有4块靴板时需要确定左侧靴板和右侧靴板
		if(pRightShoePlate)
		{
			f3dPoint centerArr[2];
			CLDSGeneralPlate* shoePlatePtrArr[2]={pShoePlate,pRightShoePlate};
			for(int i=0;i<2;i++)
			{
				for(PROFILE_VERTEX *pVertex=shoePlatePtrArr[i]->vertex_list.GetFirst();pVertex;pVertex=pFrontShoePlate->vertex_list.GetNext())
					centerArr[i]+=pVertex->vertex;
				centerArr[i]/=shoePlatePtrArr[i]->vertex_list.GetNodeNum();
				coord_trans(centerArr[i],pFrontShoePlate->ucs,TRUE);
			}
			verify_vec=front_vec;
			RotateVectorAroundVector(verify_vec,Pi*0.5,f3dPoint(0,0,-1));
			f3dPoint temp_vec=centerArr[1]-centerArr[0];
			normalize(temp_vec);
			if(verify_vec*temp_vec<0)
			{
				pAngleFoot->m_pShoePlate=pRightShoePlate;
				pAngleFoot->m_pRightShoePlate=pShoePlate;
			}
		}
		//3.3 塔脚关联角钢
		int iAngleCount=0;
		for(int i=0;i<2;i++)
		{
			pAngleFoot->m_arrAnglePtr[i]=hashAngleSet.GetValue(hKaoMoAngleArr[i]);
			if(pAngleFoot->m_arrAnglePtr[i])
			{
				hashAngleSet.DeleteNode(hKaoMoAngleArr[i]);
				iAngleCount++;
			}
		}
		pAngleFoot->m_arrAnglePtr[2]=pAngleFoot->m_arrAnglePtr[3]=NULL;
		if(hashAngleSet.GetNodeNum()>2)
		{
			Log2File()->Log("0X%X底板筛选出４根以上的连接主材角钢，数据有误!",pDatumPlate->handle);
			return false;
		}
		for(CLDSLineAngle* pLineAngle=hashAngleSet.GetFirst();pLineAngle;pLineAngle=hashAngleSet.GetNext(),iAngleCount++)
			pAngleFoot->m_arrAnglePtr[iAngleCount]=pLineAngle;
	}
	return true;
}
//识别是否为组合角钢中标准十字连接组合焊板
bool CLDSPartGroup::IdentifyStdXPlates(STDX_PLATES_PARTGROUP *pStdXPlatesPartGroup/*=NULL*/)	
{
	PARTSET partset;
	CLDSPlate *pDatumPlate=(CLDSPlate*)BelongModel()->FromPartHandle(handle,CLS_PLATE);
	if(pDatumPlate==NULL)
		return false;
	typedef CLDSPlate* CLDSPlatePtr;
	BelongModel()->GetPartGroupPartSet(handle,partset);
	if(partset.FromHandle(pDatumPlate->handle)==NULL)
		partset.append(pDatumPlate);
	if(partset.GetNodeNum()!=3)
		return false;	//钢板个数大于3
	f3dPoint depth_org=pDatumPlate->ucs.origin+(pDatumPlate->GetNormOffset()+pDatumPlate->GetThick()*0.5)*pDatumPlate->ucs.axis_z;
	//1.组焊件由三块钢板构成并且钢板的定位杆件皆为组合角钢或子角钢
	int i=0;
	CLDSLineAngle *pSonAngle=NULL;
	CHashSet<CLDSLineAngle*> hashAngleSet;	//十字插板所连接的组合角钢的子角钢集合
	CLDSPlatePtr platePtrArr[3]={NULL};	//0.基板 1.正面立板 2.侧面立板
	long hSonAngle1=0,hSonAngle2=0;
	for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext(),i++)
	{
		if(pPart->GetClassTypeId()==CLS_BOLT)
			continue;		//跳过螺栓
		if(pPart->GetClassTypeId()!=CLS_PLATE)
			return false;	//非螺栓构件不为钢板	
		CLDSPlate* pPlate=platePtrArr[i]=(CLDSPlate*)pPart;
		CLDSLineAngle *pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(((CLDSPlate*)pPart)->designInfo.m_hBasePart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pAngle==NULL&&pPlate->designInfo.origin.datum_pos_style==1)	
			pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pPlate->designInfo.origin.des_para.RODEND.hRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pAngle==NULL&&pPlate->designInfo.origin.datum_pos_style==2)	
			pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pPlate->designInfo.origin.des_para.RODNODE.hRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pAngle==NULL)
		{
			for(pAngle=(CLDSLineAngle*)BelongModel()->EnumPartFirst(CLS_LINEANGLE);pAngle;pAngle=(CLDSLineAngle*)BelongModel()->EnumPartNext(CLS_LINEANGLE))
			{
				if(pAngle->m_bVirtualPart)
					continue;
				for(int iwing=0;iwing<2;iwing++)
				{
					double ddw=0;
					if(iwing==0)
						ddw=pAngle->vxWingNorm*pDatumPlate->ucs.axis_z;
					else
						ddw=pAngle->vyWingNorm*pDatumPlate->ucs.axis_z;
					if(fabs(ddw)>0.999)//EPS_COS)	//EPS_COS太严格了,当由于用户原因出现误差时会导致漏判 wjh-2016.3.28
					{
						char cWorkWing='X'+iwing;
						if(fabs(ddw)<EPS_COS)
							Log2File()->Log("0X%X钢板与角钢0X%X的%C肢贴合不紧密,虽识别为贴合面但可能导致组焊图不准!",pDatumPlate->handle,pAngle->handle,cWorkWing);
						f3dPoint depth_vec=pAngle->Start()-depth_org;
						if(ddw>0&&fabs(depth_vec*pDatumPlate->ucs.axis_z)<0.5)
							break;
						else //if(ddw<0&&fabs(depth_vec*pDatumPlate->ucs.axis_z-pDatumPlate->GetThick())<0.5)
							break;
					}
				}
				if(iwing<2)
					break;	//找到与当前基板贴合的角钢
			}
			if(pAngle==NULL)
				return false;	//未找到与当前基板贴合的合适角钢
		}
		else
		{	//需要验证基准角钢有一肢是否与基准板贴合在一起
			bool fitting=false;
			for(int iwing=0;iwing<2;iwing++)
			{
				double ddw=0;
				if(iwing==0)
					ddw=pAngle->vxWingNorm*pDatumPlate->ucs.axis_z;
				else
					ddw=pAngle->vyWingNorm*pDatumPlate->ucs.axis_z;
				if(fabs(ddw)>0.999)//EPS_COS)	//EPS_COS太严格了,当由于用户原因出现误差时会导致漏判 wjh-2016.3.28
				{
					char cWorkWing='X'+iwing;
					if(fabs(ddw)<EPS_COS)
						Log2File()->Log("0X%X钢板与角钢0X%X的%C肢贴合不紧密,虽识别为贴合面但可能导致组焊图不准!",pDatumPlate->handle,pAngle->handle,cWorkWing);
					f3dPoint depth_vec=pAngle->Start()-depth_org;
					if(ddw>0&&fabs(depth_vec*pDatumPlate->ucs.axis_z)<0.5)
						fitting=true;
					else //if(ddw<0&&fabs(depth_vec*pDatumPlate->ucs.axis_z-pDatumPlate->GetThick())<0.5)
						fitting=true;
				}
				if(fitting)
					break;
			}
			if(!fitting)
				return false;
		}
		CLDSGroupLineAngle* pGroupAngle=NULL;
		if(pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
			pGroupAngle=(CLDSGroupLineAngle*)pAngle;
		else if(pAngle->group_father_jg_h>0x20)
			pGroupAngle=(CLDSGroupLineAngle*)BelongModel()->FromPartHandle(pAngle->group_father_jg_h,CLS_GROUPLINEANGLE);
		if(pGroupAngle)
		{
			for(int i=0;i<4;i++)
			{
				pSonAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
				if(pSonAngle==NULL||pSonAngle->m_bVirtualPart)
					continue;
				if(!hashAngleSet.GetValue(pSonAngle->handle))
					hashAngleSet.SetValue(pSonAngle->handle,pSonAngle);
				if(pAngle==pGroupAngle)
					pAngle=pSonAngle;	//保证pAngle指向单角钢
			}
			hSonAngle1=pGroupAngle->son_jg_h[0];
			if(pGroupAngle->group_style==2)
				hSonAngle2=pGroupAngle->son_jg_h[2];
			else
				hSonAngle2=pGroupAngle->son_jg_h[1];
		}
		else if(!hashAngleSet.GetValue(pAngle->handle))
			hashAngleSet.SetValue(pAngle->handle,pAngle);
	}
	if(platePtrArr[0]==NULL||platePtrArr[1]==NULL||platePtrArr[2]==NULL)
		return false;	//在组焊件构件集合中未找到三块钢板
	//2.提取组焊件基板与立板，法线方向一致的两块板为立板其余一块为基板
	CLDSPlate *pBasePlate=NULL,*pFrontVertPlate=NULL,*pBackVertPlate=NULL;	//通板、正面立板、侧面立板
	double dd1=fabs(platePtrArr[0]->ucs.axis_z*platePtrArr[1]->ucs.axis_z);
	double dd2=fabs(platePtrArr[0]->ucs.axis_z*platePtrArr[2]->ucs.axis_z);
	double dd3=fabs(platePtrArr[1]->ucs.axis_z*platePtrArr[2]->ucs.axis_z);
	if(dd1>EPS_COS2&&dd2<EPS_COS2)
	{	//0、1法线相同为立板，2为基板
		pBasePlate=platePtrArr[2];
		pFrontVertPlate=platePtrArr[0];
		pBackVertPlate=platePtrArr[1];
	}
	else if(dd2>EPS_COS2&&dd3<EPS_COS2)
	{	//0、2法线相同为立板，1为基板
		pBasePlate=platePtrArr[1];
		pFrontVertPlate=platePtrArr[0];
		pBackVertPlate=platePtrArr[2];
	}
	else if(dd3>EPS_COS2&&dd1<EPS_COS2)
	{	//1、2法线相同为立板，0为基板
		pBasePlate=platePtrArr[0];
		pFrontVertPlate=platePtrArr[1];
		pBackVertPlate=platePtrArr[2];
	}
	else 
		return false;
	if(hashAngleSet.GetNodeNum()!=2&&hashAngleSet.GetNodeNum()!=4)
		return false;
	if(pStdXPlatesPartGroup)
	{
		pStdXPlatesPartGroup->m_pBasePlate=pBasePlate;
		pStdXPlatesPartGroup->m_pFrontVertPlate=pFrontVertPlate;
		pStdXPlatesPartGroup->m_pBackVertPlate=pBackVertPlate;
		//前后立板划分
		f3dPoint front_vec=pBasePlate->ucs.axis_z;
		//用verify_vec验证感觉没必要，且如下代码仅适用于塔身主材组合角钢情况，故此注释掉 wjh-2015.4.23
		//f3dPoint verify_vec(pBasePlate->ucs.origin.x,pBasePlate->ucs.origin.y,0);	//铁塔中心指向钢板的方向
		//normalize(verify_vec);
		//if(verify_vec*pBasePlate->ucs.axis_z<0)
		//	front_vec*=-1;
		f3dPoint geom_center;
		for(PROFILE_VERTEX *pVertex=pFrontVertPlate->vertex_list.GetFirst();pVertex;pVertex=pFrontVertPlate->vertex_list.GetNext())
		{
			f3dPoint vertex=pVertex->vertex;
			coord_trans(vertex,pFrontVertPlate->ucs,TRUE);
			geom_center+=vertex;
		}
		geom_center/=pFrontVertPlate->vertex_list.GetNodeNum();
		f3dPoint plate_vec1=geom_center-pBasePlate->ucs.origin;
		normalize(plate_vec1);
		if(plate_vec1*front_vec<0)
		{	//倒换前后面立板
			pStdXPlatesPartGroup->m_pFrontVertPlate=pBackVertPlate;
			pStdXPlatesPartGroup->m_pBackVertPlate=pFrontVertPlate;
		}
		i=0;
		pStdXPlatesPartGroup->m_arrAnglePtr[0]=hashAngleSet.GetValue(hSonAngle1);
		if(pStdXPlatesPartGroup->m_arrAnglePtr[0])
		{
			hashAngleSet.DeleteNode(hSonAngle1);
			i++;
		}
		pStdXPlatesPartGroup->m_arrAnglePtr[1]=hashAngleSet.GetValue(hSonAngle2);
		if(pStdXPlatesPartGroup->m_arrAnglePtr[1])
		{
			hashAngleSet.DeleteNode(hSonAngle2);
			i++;
		}
		pStdXPlatesPartGroup->m_arrAnglePtr[2]=pStdXPlatesPartGroup->m_arrAnglePtr[3]=NULL;
		if(hashAngleSet.GetNodeNum()>2)
		{
			Log2File()->Log("0X%X十字交插板筛选出４根以上的连接主材角钢，数据有误!",pDatumPlate->handle);
			return false;
		}
		for(CLDSLineAngle* pLineAngle=hashAngleSet.GetFirst();pLineAngle;pLineAngle=hashAngleSet.GetNext(),i++)
			pStdXPlatesPartGroup->m_arrAnglePtr[i]=pLineAngle;
	}
	return true;
}
//识别含套管的挂线板组焊件
bool CLDSPartGroup::IdentifyBushingPlate()
{
	CLDSPlate *pDatumPlate=(CLDSPlate*)BelongModel()->FromPartHandle(handle,CLS_PLATE);
	if(pDatumPlate==NULL)
		return false;
	PARTSET partset;
	BelongModel()->GetPartGroupPartSet(handle,partset);
	if(partset.FromHandle(pDatumPlate->handle)==NULL)
		partset.append(pDatumPlate);
	for(CLDSPart* pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(!pPart->IsTube())
			continue;
		CLDSLineTube* pTube=(CLDSLineTube*)pPart;
		if(pTube->m_cFuncType==CLDSLineTube::FUNC_WIREBUSH||(pTube->pStart == NULL && pTube->pEnd == NULL))
			return true;
	}
	return false;
}
int CLDSPartGroup::SmartIdentifyGroupType()	//智能识别组合类型
{
	CSuperSmartPtr<CLDSPart>pDatumPart=BelongModel()->FromPartHandle(handle);
	if(pDatumPart.IsHasPtr()&&pDatumPart->GetClassTypeId()==CLS_SPHERE)
		m_iPartGroupType=GROUP_SPHERE;
	else if(IdentifyStdXPlates())
		m_iPartGroupType=GROUP_STDX_PLATES;
	else if(IdentifyAngleFoot())
		m_iPartGroupType=GROUP_ANGLEFOOT;
	else if(IdentifyBushingPlate())
		m_iPartGroupType=GROUP_BUSHING;
	else if(IdentifyDualTubeJoint(&_iPartGroupType))
		m_iPartGroupType=m_iPartGroupType;//GROUP_DUALTUBE_JOINT或GROUP_STATION_BTMTRIBEAM
	else
		m_iPartGroupType=0;
	return m_iPartGroupType;
}
bool CLDSPartGroup::IsNeedSidePlateView()
{
	PARTSET partset;
	CLDSPlate *pDatumPlate=(CLDSPlate*)BelongModel()->FromPartHandle(handle,CLS_PLATE);
	if(pDatumPlate==NULL)
		return false;
	typedef CLDSPlate* CLDSPlatePtr;
	BelongModel()->GetPartGroupPartSet(handle,partset);
	if(partset.FromHandle(pDatumPlate->handle)==NULL)
		partset.append(pDatumPlate);
	//判断子板的法线方向与侧视图法线是否近似
	f3dPoint vector(-1,0,0);
	vector_trans(vector,pDatumPlate->ucs,TRUE);
	normalize(vector);
	for(CLDSPart* pSonPart=partset.GetFirst();pSonPart;pSonPart=partset.GetNext())
	{
		if(pSonPart->handle==pDatumPlate->handle)
			continue;
		if(!pSonPart->IsPlate())
			continue;
		CLDSGeneralPlate* pSonPlate=(CLDSGeneralPlate*)pSonPart;
		if(pSonPlate->GetLsRefList()->GetNodeNum()<=0)	//立板是否有螺栓
			continue;
		double fCos=fabs(pSonPlate->ucs.axis_z*vector);
		if(fCos>EPS_COS2)
			return true;
	}
	return false;
}
//prefer为优先推荐的键值类型，如'A1',则生成Ax系列的空闲剖视符号
KEY2C CLDSPartGroup::GetIdleSectSymbol(){return GetIdleSectSymbol(KEY2C());}
KEY2C CLDSPartGroup::GetIdleSectSymbol(KEY2C prefer)
{
	CHashList<KEY2C> hashKeys;
	BOOL pushed=SonViews.push_stack();
	for(CSectionView* pView=SonViews.GetFirst();pView;pView=SonViews.GetNext())
	{
		if(!pView->IsSlaveView())
			continue;
		KEY2C symbolkey(pView->Symbol);
		hashKeys.SetValue(symbolkey,symbolkey);
	}
	SonViews.pop_stack(pushed);
	if(prefer.wItem!=0&&hashKeys.GetValue(prefer)==NULL)
		return prefer;
	else
	{
		char symbol[3]="";
		int prefer_len=max(1,strlen(prefer.Label()));
		KEY2C maxkey("@");
		for(KEY2C *pKey=hashKeys.GetFirst();pKey;pKey=hashKeys.GetNext())
		{
			if(prefer_len==1)
			{
				symbol[0]=*pKey->Label()+1;
				if(hashKeys.GetValue(KEY2C(symbol))==NULL)
					return KEY2C(symbol);
			}
			else //if(prefer_len==2)
			{
				strncpy(symbol,pKey->Label(),2);
				symbol[1]+=1;
				if(hashKeys.GetValue(KEY2C(symbol))==NULL)
					return KEY2C(symbol);
			}
			if(maxkey.wItem<pKey->wItem)
				maxkey.wItem=pKey->wItem;
		}
		if(prefer_len==1)
			maxkey.wItem+=1;
		else
			*(maxkey.Label()+1)+=1;
		return maxkey; 
	}
}

BOOL CLDSPartGroup::GetDesignItemValue(WORD wKey, long *phPart,CHashTable<DESIGN_PLATE_ITEM*>*pTbl/*=NULL*/)
{
	DESIGN_PLATE_ITEM *pItem=NULL;
	if(pTbl)
		pTbl->GetValueAt(wKey,pItem);
	else
	{
		for(pItem=designItemList.GetFirst();pItem;pItem=designItemList.GetNext())
		{
			if(pItem->key.wItem==wKey)
				break;
		}
	}
	if(pItem)
	{
		if(pItem->dataType==0)
		{
			if(phPart)
				*phPart = ftoi(pItem->value.fValue);
			return TRUE;
		}
		else if(pItem->dataType==1)
		{
			if(phPart)
				*phPart = pItem->value.hPart;
			return TRUE;
		}
	}
	return FALSE;
}
char* CLDSPartGroup::GetDesignItemNotes(WORD wKey)
{
	for(DESIGN_PLATE_ITEM *pItem=designItemList.GetFirst();pItem;pItem=designItemList.GetNext())
	{
		if(pItem->key.wItem==wKey)
			return pItem->sNotes;
	}
	return 0;
}
DESIGN_PLATE_ITEM *CLDSPartGroup::GetDesignItem(WORD wKey)
{
	for(DESIGN_PLATE_ITEM *pItem=designItemList.GetFirst();pItem;pItem=designItemList.GetNext())
	{
		if(pItem->key.wItem==wKey)
			return pItem;	
	}
	return NULL;
}
BOOL CLDSPartGroup::IsHasDesignItemValue(WORD wKey)
{
	for(DESIGN_PLATE_ITEM *pItem=designItemList.GetFirst();pItem;pItem=designItemList.GetNext())
	{
		if(pItem->key.wItem==wKey)
			return TRUE;
	}
	return FALSE;
}
BOOL CLDSPartGroup::DeleteDesignItem(WORD wKey)
{
	for(DESIGN_PLATE_ITEM *pItem=designItemList.GetFirst();pItem;pItem=designItemList.GetNext())
	{
		if(pItem->key.wItem==wKey)
		{
			designItemList.DeleteCursor();
			return TRUE;
		}
	}
	return FALSE;
}
//BOOL CLDSPartGroup::SetDesignItemValue(WORD wKey,long lValue,char *sNotes=NULL,BOOL bModify=FALSE);

#endif