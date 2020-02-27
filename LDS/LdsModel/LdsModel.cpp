// LdsModel.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "stdlib.h"
#include "macro.h"
#include "Buffer.h"
#ifndef _LEGACY_LICENSE
#include "XhLdsLm.h"
#else
#include "Lic.h"
#endif
#include "Tower.h"
//#include "lds_part.h"

void CfgwordToBuffer(CFGWORD &cfgword,CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if( version==0||
		(doc_type==1&&version>=4020006)||	//TMA
		(doc_type==2&&version>=1020006)||	//LMA
		(doc_type==3&&version>=2000006)||	//TSA
		(doc_type==4&&version>=1020006)||	//LDS
		(doc_type==5&&version>=1020006))	//TDA
		buffer.Write(cfgword.flag.word,24);
	else
		buffer.Write(cfgword.flag.word,20);
}
void CfgwordFromBuffer(CFGWORD &cfgword,CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if( version==0||
		(doc_type==1&&version>=4020006)||	//TMA
		(doc_type==2&&version>=1020006)||	//LMA
		(doc_type==3&&version>=2000006)||	//TSA
		(doc_type==4&&version>=1020006)||	//LDS
		(doc_type==5&&version>=1020006))	//TDA
		buffer.Read(&cfgword.flag.word,24);
	else
		buffer.Read(&cfgword.flag.word,20);
}

void MirPartHandle(IModel *pTower,long *mir_h,long org_h,MIRMSG msg,COORD3D *pPickPos/*=NULL*/)
{
	*mir_h=0;
	CLDSDbObject *pObj=(CLDSDbObject*)pTower->FromHandle(org_h,TRUE);
	f3dPoint pickPos;
	if(pPickPos==NULL)	//调用MirPartHandle主要是用于对称关联引用杆件，故此时不宜采用严格对称规则，只要有部分对称即可　wjh-2014.5.4
		pPickPos=&pickPos;
	if(pObj)
	{	
		//杆件启用一端对称时从关联构件列表中找到的杆件可能是错误的	wht 10-01-15
		//所以对于杆件都应该使用GetMirPart()来得到对称杆件(GetMirPart()进行了相应的判断)
		CLDSPart *pLinePart=NULL;
		if(pObj->IsPart())
			pLinePart=(CLDSPart*)pObj;
		if(pLinePart==NULL||(pLinePart&&!pLinePart->IsLinePart()))
		{	//非杆件可以先通过关联构件查找对称杆件
			for(RELATIVE_OBJECT *pItem=pObj->relativeObjs.GetFirst();pItem;pItem=pObj->relativeObjs.GetNext())
			{	//先在关联构件列表中查找
				CLDSDbObject *pMirObj=NULL;
				if(pItem->mirInfo.axis_flag==msg.axis_flag&&
					pItem->mirInfo.rotate_angle==msg.rotate_angle&&
					pItem->mirInfo.origin==msg.origin)
				{
					pMirObj=(CLDSDbObject*)pTower->FromHandle(pItem->hObj,TRUE);
					if(pMirObj&&pMirObj->GetClassTypeId()==pObj->GetClassTypeId())
					{
						*mir_h=pMirObj->handle;
						return;
					}
				}
			}
		}
	}
	CLDSPart *pPart=pTower->FromPartHandle(org_h);
	if(pPart)
	{
		CLDSPart *pMirPart=NULL;
		CLDSLinePart *pLinePart=NULL;
		f3dPoint line_vec;
		double cosa=0;
		if(pPart->IsLinePart())
		{
			pLinePart=(CLDSLinePart*)pPart;
			line_vec=pLinePart->End()-pLinePart->Start();
			normalize(line_vec);
			cosa=fabs(line_vec*f3dPoint(1,0,0));
		}
		//在查找Z轴对称或旋转对称的杆件时不启用一端对称，横材也不能启用
		//过往不知为什么Z轴对称时不启用一端对称，现在发现做高低腿时主材Z轴对称也需要启用一端对称 wjh-2015.5.23
		//if(pLinePart&&msg.axis_flag!=4&&msg.axis_flag!=8&&cosa<EPS_COS2)
			pMirPart=pPart->GetMirPart(msg,*pPickPos);
		//else
		//	pMirPart=pPart->GetMirPart(msg);
		if(pMirPart)
			*mir_h=pMirPart->handle;
	}
	else
	{
		CLDSNode *pOrgNode=pTower->FromNodeHandle(org_h);
		if(pOrgNode)
		{
			CLDSNode *pMirNode=pOrgNode->GetMirNode(msg);
			if(pMirNode)
				*mir_h=pMirNode->handle;
		}
	}
}
f3dPoint SpreadFacePntToDatumFace(f3dPoint datum_face_norm,f3dLine axis,
					f3dPoint spread_face_norm,f3dPoint pick,BOOL bFromMuti/*=TRUE*/)
{
	double tem_angle;
	f3dPoint axis_vec,vice_vec;

	if(datum_face_norm*spread_face_norm<0)
		spread_face_norm *=-1;	//展开角应为锐角
	//cal_angle_of_2vec的返回在0~π之间的角度
	tem_angle = cal_angle_of_2vec(datum_face_norm,spread_face_norm);
	//两面法线叉积为其交线的矢量方向
	axis_vec = spread_face_norm^datum_face_norm;

	if(!bFromMuti)//反向旋转使多面板角钢恢复原来位置
		tem_angle = -tem_angle;
	Sub_Pnt(vice_vec,axis.endPt, axis.startPt);
	if(dot_prod(vice_vec,axis_vec)<0)
		tem_angle = -tem_angle;
	rotate_point_around_axis(pick,tem_angle,axis);

	return pick;
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL IsBerHuoQuJg(CLDSLineAngle *pHuoQuBaseJg, f3dPoint ray_jg_vec,int huoqu_x_wing0_y_wing1)
{
	if(pHuoQuBaseJg==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
		throw "Don't have this bending datum angle,data input error!";
#else
		throw "没有这个火曲基准角钢,数据输入错误!";
#endif
	f3dPoint wing_norm;
	if(huoqu_x_wing0_y_wing1==0)
		wing_norm = pHuoQuBaseJg->get_norm_y_wing();
	else
		wing_norm = pHuoQuBaseJg->get_norm_x_wing();
	
	if(wing_norm*ray_jg_vec>0)
		return TRUE;	//火曲线位于火曲基准角钢楞线一侧
	else
		return FALSE;	//火曲线位于火曲基准角钢肢一侧
}
BOOL IsPartsCollide(CLDSPart *pMainPart,CLDSPart *pVicePart,int nCollideVertexDist/*=-1*/)
{
	UCS_STRU ucs;
	if(pMainPart==NULL||pVicePart==NULL||pMainPart==pVicePart)
		return FALSE;
	if(nCollideVertexDist<0)
		nCollideVertexDist = 5;
	fBody body;
	CSuperSmartPtr<CLDSPart>pDatumPart=pMainPart;
	f3dPoint len_vec,ray_vec,work_norm;
	if(!pDatumPart->IsLinePart())	//基准构件不是线条型构件
		work_norm=pDatumPart->ucs.axis_z;
	else if(!pVicePart->IsLinePart())
		work_norm=pVicePart->ucs.axis_z;
	else
	{	//两构件都是线条型构件
		CLDSLinePart* pRayLinePart=(CLDSLinePart*)pVicePart;
		len_vec=pDatumPart.pRod->xPosEnd-pDatumPart.pRod->xPosStart;
		f3dPoint ray_len_vec=pRayLinePart->xPosEnd-pRayLinePart->xPosStart;
		double dfDatumRodLength=len_vec.mod();
		double dfRayRodLength=ray_len_vec.mod();
		if (dfDatumRodLength+pDatumPart.pRod->dfStartOdd+pDatumPart.pRod->dfEndOdd<10)
			return false;	//基准杆件长度已经过短不适宜进行正负头碰撞验算 wjh-2019.10.7
		if (dfRayRodLength+pRayLinePart->dfStartOdd+pRayLinePart->dfEndOdd<10)
			return false;	//射线杆件长度已经过短不适宜进行正负头碰撞验算 wjh-2019.10.7
		if(dfDatumRodLength>EPS)
			len_vec/=dfDatumRodLength;
		if(dfRayRodLength>EPS)
			ray_len_vec/=dfRayRodLength;
		work_norm=len_vec^ray_len_vec;
		normalize(work_norm);
	}
	//生成辅助构件的实体数据
	pVicePart->SetModified();
	bool bDisplayHole=pVicePart->m_bDisplayHole;
	pVicePart->m_bDisplayHole=false;
	pVicePart->Create3dSolidModel(FALSE);
	pVicePart->m_bDisplayHole=bDisplayHole;
	//生成基准构件的碰撞检测实体数据(有可能与基准构件的实际实体数据不一致)
	if(pMainPart->GetClassTypeId()==CLS_LINEANGLE)
	{	//主构件为角钢
		CLDSLineAngle *pMainAngle=(CLDSLineAngle*)pMainPart;
		//设置射线角钢检查线段
		double wing_wide = pMainAngle->GetWidth();
		double wing_thick = pMainAngle->GetThick();
		//设置主角钢检查边界面
		f3dPoint real_start_ber,real_end_ber;
		real_start_ber=pMainAngle->Start()-len_vec*(pMainAngle->startOdd()+nCollideVertexDist);
		real_end_ber  =pMainAngle->End()+len_vec*(pMainAngle->endOdd()+nCollideVertexDist);
		double ddx=work_norm*pMainAngle->get_norm_x_wing();
		double ddy=work_norm*pMainAngle->get_norm_y_wing();
		if(fabs(ddx)<fabs(ddy))	//X肢为检查面
		{
			body.vertex.append(real_start_ber+nCollideVertexDist*pMainAngle->get_norm_x_wing()+wing_wide*pMainAngle->get_norm_y_wing());
			body.vertex.append(real_end_ber+nCollideVertexDist*pMainAngle->get_norm_x_wing()+wing_wide*pMainAngle->get_norm_y_wing());
			body.vertex.append(real_end_ber+nCollideVertexDist*pMainAngle->get_norm_x_wing()+wing_wide*pMainAngle->GetWingVecX());
			body.vertex.append(real_start_ber+nCollideVertexDist*pMainAngle->get_norm_x_wing()+wing_wide*pMainAngle->GetWingVecX());
			
			body.vertex.append(real_start_ber+(wing_wide+nCollideVertexDist)*pMainAngle->GetWingVecY()+wing_wide*pMainAngle->get_norm_y_wing());
			body.vertex.append(real_end_ber+(wing_wide+nCollideVertexDist)*pMainAngle->GetWingVecY()+wing_wide*pMainAngle->get_norm_y_wing());
			body.vertex.append(real_end_ber+(wing_wide+nCollideVertexDist)*pMainAngle->GetWingVecY()-(wing_thick+nCollideVertexDist)*pMainAngle->get_norm_y_wing());
			body.vertex.append(real_start_ber+(wing_wide+nCollideVertexDist)*pMainAngle->GetWingVecY()-(wing_thick+nCollideVertexDist)*pMainAngle->get_norm_y_wing());
			
			body.vertex.append(pMainAngle->get_start_in_ber()-len_vec*(pMainAngle->startOdd()+nCollideVertexDist));	//8
			body.vertex.append(pMainAngle->get_end_in_ber()+len_vec*(pMainAngle->endOdd()+nCollideVertexDist));		//9
			f3dPolyFace* pFace=body.faceList.append();
			body.NewEdgeLine(pFace,1,0);
			body.NewEdgeLine(pFace,2);
			body.NewEdgeLine(pFace,3);
			body.NewEdgeLine(pFace,0);
			pFace=body.faceList.append();
			body.NewEdgeLine(pFace,5,4);
			body.NewEdgeLine(pFace,6);
			body.NewEdgeLine(pFace,7);
			body.NewEdgeLine(pFace,4);
			pFace=body.faceList.append();
			body.NewEdgeLine(pFace,7,6);
			body.NewEdgeLine(pFace,8);
			body.NewEdgeLine(pFace,6);
			pFace=body.faceList.append();
			body.NewEdgeLine(pFace,8,9);
			body.NewEdgeLine(pFace,7);
			body.NewEdgeLine(pFace,9);
			//始端面
			pFace=body.faceList.append();
			body.NewEdgeLine(pFace,4,0);
			body.NewEdgeLine(pFace,7);
			body.NewEdgeLine(pFace,3);
			body.NewEdgeLine(pFace,0);
			//终端面
			pFace=body.faceList.append();
			body.NewEdgeLine(pFace,2,1);
			body.NewEdgeLine(pFace,6);
			body.NewEdgeLine(pFace,5);
			body.NewEdgeLine(pFace,1);
		}
		else
		{
			body.vertex.append(real_start_ber+nCollideVertexDist*pMainAngle->get_norm_y_wing()+(wing_wide+nCollideVertexDist)*pMainAngle->get_norm_x_wing());
			body.vertex.append(real_end_ber+nCollideVertexDist*pMainAngle->get_norm_y_wing()+(wing_wide+nCollideVertexDist)*pMainAngle->get_norm_x_wing());
			body.vertex.append(real_end_ber+nCollideVertexDist*pMainAngle->get_norm_y_wing()+pMainAngle->GetWingVecY()*(wing_wide+nCollideVertexDist));
			body.vertex.append(real_start_ber+nCollideVertexDist*pMainAngle->get_norm_y_wing()+pMainAngle->GetWingVecY()*(wing_wide+nCollideVertexDist));
			
			body.vertex.append(real_start_ber+(wing_wide+nCollideVertexDist)*pMainAngle->GetWingVecX()+wing_wide*pMainAngle->get_norm_x_wing());
			body.vertex.append(real_end_ber+(wing_wide+nCollideVertexDist)*pMainAngle->GetWingVecX()+wing_wide*pMainAngle->get_norm_x_wing());
			body.vertex.append(real_end_ber+(wing_wide+nCollideVertexDist)*pMainAngle->GetWingVecX()-(wing_thick+nCollideVertexDist)*pMainAngle->get_norm_x_wing());
			body.vertex.append(real_start_ber+(wing_wide+nCollideVertexDist)*pMainAngle->GetWingVecX()-(wing_thick+nCollideVertexDist)*pMainAngle->get_norm_x_wing());
			
			body.vertex.append(pMainAngle->get_start_in_ber()-len_vec*(pMainAngle->startOdd()+nCollideVertexDist));
			body.vertex.append(pMainAngle->get_end_in_ber()+len_vec*(pMainAngle->endOdd()+nCollideVertexDist));
			f3dPolyFace* pFace=body.faceList.append();
			body.NewEdgeLine(pFace,1,0);
			body.NewEdgeLine(pFace,2);
			body.NewEdgeLine(pFace,3);
			body.NewEdgeLine(pFace,0);
			pFace=body.faceList.append();
			body.NewEdgeLine(pFace,5,4);
			body.NewEdgeLine(pFace,6);
			body.NewEdgeLine(pFace,7);
			body.NewEdgeLine(pFace,4);
			pFace=body.faceList.append();
			body.NewEdgeLine(pFace,7,6);
			body.NewEdgeLine(pFace,8);
			body.NewEdgeLine(pFace,6);
			pFace=body.faceList.append();
			body.NewEdgeLine(pFace,8,9);
			body.NewEdgeLine(pFace,7);
			body.NewEdgeLine(pFace,9);
			//始端面
			pFace=body.faceList.append();
			body.NewEdgeLine(pFace,3,0);
			body.NewEdgeLine(pFace,7);
			body.NewEdgeLine(pFace,4);
			body.NewEdgeLine(pFace,0);
			//终端面
			pFace=body.faceList.append();
			body.NewEdgeLine(pFace,5,1);
			body.NewEdgeLine(pFace,6);
			body.NewEdgeLine(pFace,2);
			body.NewEdgeLine(pFace,1);
		}
		CSolidBody solidbody;
		solidbody.ConvertFrom(&body);
		return pVicePart->pSolidBody->IsIntWith(&solidbody);
	}
	else if(pMainPart->GetClassTypeId()==CLS_LINETUBE)
	{	//主构件为钢管
		CLDSLineTube *pMainPipe=(CLDSLineTube*)pMainPart;
		f3dPoint axis_x=len_vec^work_norm;
		normalize(axis_x);
		double radius=pMainPipe->GetDiameter()*0.5+nCollideVertexDist;
		f3dPoint vertice=pMainPipe->Start()-len_vec*pMainPipe->startOdd()+axis_x*radius-work_norm*radius;
		f3dPoint vertice_left=vertice;
		//生成右侧面顶点
		body.vertex.append(vertice);
		vertice+=work_norm*(radius*2);
		body.vertex.append(vertice);
		vertice+=len_vec*pMainPipe->GetLength();
		body.vertex.append(vertice);
		vertice-=work_norm*(radius*2);
		body.vertex.append(vertice);
		//生成左侧面顶点
		vertice_left-=axis_x*(radius*2);
		body.vertex.append(vertice_left);
		vertice_left+=work_norm*(radius*2);
		body.vertex.append(vertice_left);
		vertice_left+=len_vec*pMainPipe->GetLength();
		body.vertex.append(vertice_left);
		vertice_left-=work_norm*(radius*2);
		body.vertex.append(vertice_left);
		//建立右侧面
		f3dPolyFace *pFace=body.faceList.append();
		body.NewEdgeLine(pFace,1,0);
		body.NewEdgeLine(pFace,2);
		body.NewEdgeLine(pFace,3);
		body.NewEdgeLine(pFace,0);
		//建立左侧面
		pFace=body.faceList.append();
		body.NewEdgeLine(pFace,5,4);
		body.NewEdgeLine(pFace,6);
		body.NewEdgeLine(pFace,7);
		body.NewEdgeLine(pFace,4);
		CSolidBody solidbody;
		solidbody.ConvertFrom(&body);
		return pVicePart->pSolidBody->IsIntWith(&solidbody);
	}
	else if(pMainPart->IsPlate())
	{
		double fOldThick=pMainPart->Thick;
		GEPOINT xOrg=pMainPart->ucs.origin;
		if(nCollideVertexDist>0)
		{
			pMainPart->Thick+=nCollideVertexDist*2;
			pMainPart->ucs.origin-=pMainPart->ucs.axis_z*nCollideVertexDist;
		}
		pMainPart->Create3dSolidModel(FALSE);
		BOOL intersed=pVicePart->pSolidBody->IsIntWith(pMainPart->pSolidBody);
		if(nCollideVertexDist>0)
		{
			pMainPart->Thick=fOldThick;
			pMainPart->ucs.origin=xOrg;
		}
		return intersed;
	}
	else
	{
		pMainPart->SetModified();
		pMainPart->Create3dSolidModel(FALSE);
		//TODO:怀疑以前代码有问题
		//return pVicePart->pBody->IsIntWith(&body);
		return pVicePart->pSolidBody->IsIntWith(pMainPart->pSolidBody);
	}
}
void CreateArcLine(f3dAtomLine *pAtomLine,f3dPoint &workNorm,PROFILE_VERTEX &profileVertex)
{
	if(profileVertex.type>=1)
	{
		double sec_angle;
		int sign=1;
		if(profileVertex.type==1||profileVertex.type==3)
		{	//指定圆弧夹角(自动计算圆弧夹角的情况现在已自动计算出精确值)
			sec_angle=profileVertex.sector_angle;
		}
		else if(profileVertex.type==2)
		{	//指定R
			if(profileVertex.radius<0)
				sign*=-1;
			double half_len = 0.5*DISTANCE(*pAtomLine->pStart,*pAtomLine->pEnd);
			if(half_len>fabs(profileVertex.radius))
				sec_angle = Pi;
			else
				sec_angle = 2*asin(half_len/fabs(profileVertex.radius));
		}
		int iFace=0;
		if(profileVertex.vertex.feature<10)
			iFace=profileVertex.vertex.feature-1;
		if(sec_angle<0)
		{
			sign=-sign;
			sec_angle=-sec_angle;
		}
		f3dPoint worknorm;
		if(iFace==0)
			worknorm=sign*workNorm;
		else if(iFace==1)
			worknorm=sign*workNorm;
		else //if(iFace==2)
			worknorm=sign*workNorm;
		if(profileVertex.type<3)
			pAtomLine->CreateArcLine(worknorm,sec_angle);
		else //椭圆弧
		{
			pAtomLine->CreateEllipseArcLine(profileVertex.center,profileVertex.column_norm,worknorm,profileVertex.radius);
			//椭圆时有可能需要调整始终端点位置
			f3dPoint start=pAtomLine->PositionInAngle(0);
			f3dPoint end = pAtomLine->PositionInAngle(pAtomLine->SectorAngle());
			pAtomLine->pStart->Set(start.x,start.y,start.z);
			pAtomLine->pEnd->Set(end.x,end.y,end.z);
		}
	}
}
#endif
CXhChar100 PointToString(const f3dPoint &point,bool b3dPt/*=TRUE*/)
{
	char sX[100]={""},sY[100]={""},sZ[100]={""};
	sprintf(sX,"%.1f",point.x);
	SimplifiedNumString(sX);
	sprintf(sY,"%.1f",point.y);
	SimplifiedNumString(sY);
	if(b3dPt)
	{
		sprintf(sZ,"%.1f",point.z);
		SimplifiedNumString(sZ);
		return CXhChar100("(%s,%s,%s)",sX,sY,sZ);
	}
	else 
		return CXhChar100("(%s,%s)",sX,sY);
}
