// LineTube.cpp: implementation of the CLDSSlot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tower.h"
#include "CreateFace.h"
#include "LogFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CTubeEndPosPara::IsInDatumJgBer()		//基点是否在基准角钢楞线上
{
	if(datum_pos_style==0||datum_pos_style==3)
		return TRUE;
	else
		return FALSE;
}
BOOL CTubeEndPosPara::IsInDatumJgWingX()
{
	if(datum_pos_style==1||datum_pos_style==4)
		return TRUE;
	else
		return FALSE;
}
BOOL CTubeEndPosPara::IsInDatumJgWingY()
{
	if(datum_pos_style==2||datum_pos_style==5)
		return TRUE;
	else
		return FALSE;
}
void CTubeEndPosPara::FromBuffer(CBuffer &buffer, long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	buffer.ReadInteger(&endPosType);
	if(	   doc_type==5
		||(doc_type==1&&(version==0||version>=4000030))	//TMA
		||(doc_type==2&&(version==0||version>=1000085))	//LMA
		||(doc_type==3&&(version==0||version>=1070002))	//TSA
		||(doc_type==4&&(version==0||version>=1000013)))//LDS
		buffer.ReadByte(&datum_pos_style);
	buffer.ReadInteger(&hDatumPart);
	buffer.ReadInteger(&hDatumStartPart);
	buffer.ReadInteger(&hDatumEndPart);
	buffer.ReadInteger(&bDatumEndStartEnd);
	buffer.ReadInteger(&bDatumStartStartEnd);
	if(	   doc_type==5
		||(doc_type==1&&(version==0||version>=4000029))	//TMA
		||(doc_type==2&&(version==0||version>=1000084))	//LMA
		||(doc_type==4&&(version==0||version>=1000011)))//LDS
	{	//平推面法线 10000084
		buffer.ReadBooleanThin(&bUseFaceOffsetNorm);
		if(bUseFaceOffsetNorm)
		{
			buffer.ReadDouble(&face_offset_norm.x);
			buffer.ReadDouble(&face_offset_norm.y);
			buffer.ReadDouble(&face_offset_norm.z);
		}	
	}
	buffer.ReadDouble(&fEccentricDist);
	if(  version==0||doc_type==5||	//TDA
		(doc_type==1&&version>=4000103)|| //TMA V4.0.1.3
		(doc_type==2&&version>=1000103)|| //LMA V1.0.1.3
		(doc_type==3&&version>=1070303)|| //TSA V1.7.3.3
		(doc_type==4&&version>=1000303))  //LDS V1.0.3.3
		buffer.ReadDouble(&fLenVecOffsetDist); //基点杆件长度方向偏移值 wht 11-04-23
	buffer.ReadDouble(&fNormOffset);
}
void CTubeEndPosPara::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteInteger(endPosType);
	buffer.WriteByte(datum_pos_style);
	buffer.WriteInteger(hDatumPart);
	buffer.WriteInteger(hDatumStartPart);
	buffer.WriteInteger(hDatumEndPart);
	buffer.WriteInteger(bDatumEndStartEnd);
	buffer.WriteInteger(bDatumStartStartEnd);
	//平推面法线 10000084
	if(face_offset_norm.IsZero())
		bUseFaceOffsetNorm=false;
	else
		bUseFaceOffsetNorm=true;
	buffer.WriteBooleanThin(bUseFaceOffsetNorm);
	if(bUseFaceOffsetNorm)
	{
		buffer.WriteDouble(face_offset_norm.x);
		buffer.WriteDouble(face_offset_norm.y);
		buffer.WriteDouble(face_offset_norm.z);
	}
	buffer.WriteDouble(fEccentricDist);
	buffer.WriteDouble(fLenVecOffsetDist); //1.0.1.3
	buffer.WriteDouble(fNormOffset);
}

bool CTubeEndPosPara::ReplaceReferenceHandle(CHandleReplacer& replacer,double* pickpos_coord)
{
	bool replaced=false;
	if(!replacer.SelectPerfectPartHandle(pickpos_coord))
		return false;
	replaced|=replacer.VerifyAndReplace(&hDatumPart);
	replaced|=replacer.VerifyAndReplace(&hDatumStartPart);
	replaced|=replacer.VerifyAndReplace(&hDatumEndPart);
	return replaced;
}
long CTubeEndPosPara::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	if(hDatumPart>0)
		pListHandlers->Append(HANDLE_ADDR(&hDatumPart));
	if (hDatumStartPart>0)
		pListHandlers->Append(HANDLE_ADDR(&hDatumStartPart));
	if (hDatumEndPart>0)
		pListHandlers->Append(HANDLE_ADDR(&hDatumEndPart));
	return 3;
}

// 钢管
BOOL CLDSLineTube::bDisplaySolidTransectFace=FALSE;
BYTE CLDSLineTube::ciFlTubeOddment=1;	//端板为法兰时,正负头计算原则默认为到法兰端面 wjh-2015.3.14
CLDSLineTube::CLDSLineTube(void)
{
	size_idClassType=m_nClassTypeId = CLS_LINETUBE;
	//strcpy(m_sClassName,"CLDSLineTube");
	size_wide = 114;	//直径
	size_thick = 4;
	cMaterial = 'S';
	pSolidBody = NULL;
	m_uSlices = 0;
	m_uDisplaySlices = 0;
	is_visible=TRUE;
	m_uStatMatCoef = 1;
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
	hWorkNormRefPart = 0;
	m_bHasWeldRoad = FALSE;
	m_iWeldRoadStyle = 0;			//默认为直接给定
	m_iNormCalStyle = 1;			//默认为杆件指定
	m_bCutSlot = false;				//默认不需要开槽处理
	memset(&desStartPos,0,sizeof(CTubeEndPosPara));
	memset(&desEndPos,0,sizeof(CTubeEndPosPara));
	memset(&m_tJointStart,0,sizeof(TUBEJOINT));
	memset(&m_tJointEnd,0,sizeof(TUBEJOINT));
	hTransTube=0;		//贯穿此钢管的钢管
	hWorkNormRefPart=0;	//计算工作面法线时的参照杆件，如果为0，则以始端节点或终端节点的父杆件为参照杆件
	nearWorkPlaneNorm.Set(0,0,0);
}
CLDSLineTube::~CLDSLineTube()
{
	if(pSolidBody)
	{
		delete pSolidBody;
		pSolidBody=NULL;
	}
}
//是否存在因钢管发生变动而需要更新实体的构件
bool CLDSLineTube::IsHasTubeRelaModifiedParts()
{
	return _localHashModifiedAngles.GetNodeNum()>0||_localHashModifiedBolts.GetNodeNum()>0||_localHashModifiedPlates.GetNodeNum()>0;
}
//记录需要更新实体的钢管
CLDSLineTube* CLDSLineTube::AppendModifiedTube(CLDSLineTube* pModifiedTube)
{
	if(pModifiedTube==NULL)
		return NULL;
	return _localHashModifiedTubes.SetValue(pModifiedTube->handle,pModifiedTube);
}
//遍历第一根发生变化后需要更新实体的钢管
CLDSLineTube* CLDSLineTube::EnumFirstModifiedTube()
{
	return _localHashModifiedTubes.GetFirst();
}
//遍历下一根发生变化后需要更新实体的钢管
CLDSLineTube* CLDSLineTube::EnumNextModifiedTube()
{
	return _localHashModifiedTubes.GetNext();
}
//
int CLDSLineTube::GatherOddRelaParts(BYTE start1end2all3/*=3*/)
{
	int hits=0;
	CLDSParamPlate *pParamPlate=NULL;
	if(start1end2all3&0x01 && m_tJointStart.type>=2 && m_tJointStart.type<=10)
		pParamPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(m_tJointStart.hLinkObj,CLS_PARAMPLATE);
	if(start1end2all3&0x02 && m_tJointEnd.type>=2 && m_tJointEnd.type<=10)	
		pParamPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
	if(pParamPlate==NULL)
		return hits;
	//添加钢管随动的关联钢板和关联螺栓
	CHashSet<CLDSBolt*> hashRelaBolt;
	CHashSet<CLDSGeneralPlate*> hashRelaPlate;
	hashRelaPlate.SetValue(pParamPlate->handle,pParamPlate);
	if(pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND||pParamPlate->m_iParamType==TYPE_XEND)
	{	//将U型板或卷边板附带板的原点坐标转化为相对坐标，等重新设计完以后再转换为绝对坐标
		for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
		{
			CLDSGeneralPlate *pPlate=(CLDSPlate*)BelongModel()->FromPartHandle(pItem->h,CLS_PLATE,CLS_PARAMPLATE);
			if(pPlate && hashRelaPlate.GetValue(pPlate->handle)==NULL)
				hashRelaPlate.SetValue(pPlate->handle,pPlate);
		}
		//遍历普通钢板，根据其连接杆件设计参数查找关联板
		BOOL pushed=BelongModel()->PushPartStack();
		for(CLDSPlate* pPlate=(CLDSPlate*)BelongModel()->EnumPartFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)BelongModel()->EnumPartNext(CLS_PLATE))
		{
			if(pPlate==NULL || pPlate->GetLjRodPara(this->handle)==NULL)
				continue;
			if(hashRelaPlate.GetValue(pPlate->handle)==NULL)
				hashRelaPlate.SetValue(pPlate->handle,pPlate);
		}
		BelongModel()->PopPartStack(pushed);
	}
	else if(pParamPlate->IsFL()||pParamPlate->IsInsertPlate())
	{	//重新设计U型板和卷边板时将其附带的封头板和加劲板一起重新设计
		for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
		{
			CLDSGeneralPlate *pPlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(pItem->h,CLS_PLATE,CLS_PARAMPLATE);
			if(pPlate && hashRelaPlate.GetValue(pPlate->handle)==NULL)
				hashRelaPlate.SetValue(pPlate->handle,pPlate);
		}
		//遍历肘板，根据其设计参数查找关联板
		BOOL pushed=BelongModel()->PushPartStack();
		for(CLDSParamPlate* pPlate=(CLDSParamPlate*)BelongModel()->EnumPartFirst(CLS_PARAMPLATE);pPlate;pPlate=(CLDSParamPlate*)BelongModel()->EnumPartNext(CLS_PARAMPLATE))
		{
			if(pPlate==NULL || hashRelaPlate.GetValue(pPlate->handle))
				continue;
			long hDatumPlate=0;
			if(pPlate->m_iParamType==TYPE_RIBPLATE)
				pPlate->GetDesignItemValue('C',&hDatumPlate);
			else if(pPlate->m_iParamType==TYPE_ELBOWPLATE)
				pPlate->GetDesignItemValue('L',&hDatumPlate);
			else
				continue;
			if(abs(hDatumPlate)==pParamPlate->handle)
				hashRelaPlate.SetValue(pPlate->handle,pPlate);
		}
		BelongModel()->PopPartStack(pushed);
	}
	//
	for(CLDSGeneralPlate* pPlate=hashRelaPlate.GetFirst();pPlate;pPlate=hashRelaPlate.GetNext())
	{
		_localHashModifiedPlates.SetValue(pPlate->handle,pPlate);
		//
		for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(pBolt && hashRelaBolt.GetValue(pBolt->handle)==NULL)
			{
				hashRelaBolt.SetValue(pBolt->handle,pBolt);
				_localHashModifiedBolts.SetValue(pBolt->handle,pBolt);
			}
		}
	}
	return hits;
}
//计算钢管正负头，但不修改钢管正负头，返回计算得到的正负头值	wht 10-04-07
double CLDSLineTube::CalOddment(BOOL bStart,CLDSParamPlate *pParamPlate,bool bReturnOddToFL/*=false*/)
{
	double fOldOddment=0;	//记录旧的正负头
	DESIGNODDMENT *pCurDesOddment=NULL;
	TUBEJOINT* pTubeJoint = NULL;
	if(bStart)
	{	//始端
		fOldOddment=start_oddment;
		pCurDesOddment=&desStartOdd;
		pTubeJoint = &m_tJointStart;
	}
	else
	{	//终端
		fOldOddment=end_oddment;
		pCurDesOddment=&desEndOdd;
		pTubeJoint = &m_tJointEnd;
	}
	if (pCurDesOddment->m_iOddCalStyle == 0)
	{	//根据碰撞计算正负头
		CLDSPart *pPartArr[2] = { NULL };
		pPartArr[0] = BelongModel()->FromPartHandle(pCurDesOddment->m_hRefPart1);
		pPartArr[1] = BelongModel()->FromPartHandle(pCurDesOddment->m_hRefPart2);
		if (pPartArr[0] == NULL && pPartArr[1] == NULL)
			return fOldOddment;	//无参照杆件，无需计算
		if (pPartArr[0] && pPartArr[1] && (pPartArr[0]->handle == pPartArr[1]->handle))
			pPartArr[1] = NULL;	//避免重复计算
		double H = GetDiameter(), extra_odd = 0, L = 0, M = 0, A = 0, S = 0, N = 0;
		if (pParamPlate && (pParamPlate->m_iParamType == TYPE_ROLLEND
			|| pParamPlate->m_iParamType == TYPE_XEND || pParamPlate->m_iParamType == TYPE_UEND))
		{	//端头有参数化插板
			if (pParamPlate->m_bStdPart)
			{	//标准构件
				CLDSStdPartModel stdPartModel;
				if (pParamPlate->GetStdPartModel(&stdPartModel))
				{
					L = stdPartModel.param.insert_plate_param.L;
					M = stdPartModel.param.insert_plate_param.M;
					A = stdPartModel.param.insert_plate_param.A;
					S = stdPartModel.param.insert_plate_param.S;
					H = stdPartModel.param.insert_plate_param.H;
					N = stdPartModel.param.insert_plate_param.N;
				}
			}
			else
			{	//非标准构件
				pParamPlate->GetDesignItemValue('L', &L);
				pParamPlate->GetDesignItemValue('M', &M);
				pParamPlate->GetDesignItemValue('A', &A);
				pParamPlate->GetDesignItemValue('S', &S);
				pParamPlate->GetDesignItemValue('H', &H);
				pParamPlate->GetDesignItemValue('N', &N);
			}
			if (pParamPlate->m_iParamType == TYPE_UEND || pParamPlate->m_iParamType == TYPE_ROLLEND)
				extra_odd = L - M;	//始端存在U/C型插板正头至少应减去(L-M)
			else if (pParamPlate->m_iParamType == TYPE_XEND)
				extra_odd = A + S;	//十字插板
		}
		else
			pParamPlate = NULL;
		f3dPoint datum_line_vec = End() - Start();
		normalize(datum_line_vec);
		if (bStart)
			start_oddment = 0;
		else
			end_oddment = 0;
		for (int i = 0; i < 2; i++)
		{	//计算正负头
			if (pPartArr[i] == NULL)
				continue;
			if (pPartArr[i]->GetClassTypeId() == CLS_LINETUBE || pPartArr[i]->GetClassTypeId() == CLS_LINEANGLE)
			{	//钢管端有插板
				CLDSLinePart *pLinePart = (CLDSLinePart*)pPartArr[i];
				f3dPoint cross_line_vec = pLinePart->End() - pLinePart->Start();
				normalize(cross_line_vec);
				//α角为当前角钢射线与参照杆件轴线的垂线间的夹角
				double sina = fabs(datum_line_vec*cross_line_vec);
				double cosa = sqrt(1 - sina * sina);
				double offset_dist = 0;//因钢管外径或插板外形宽度引起的负头部分
				if (cosa < EPS)
					continue;
				if (pParamPlate)
				{
					if (pParamPlate->m_iParamType == TYPE_UEND || pParamPlate->m_iParamType == TYPE_ROLLEND)
					{	//基准钢管和交叉钢管不垂直且基准钢管端存在槽型插板或者U型插板时
						//β角槽型插板倒角边与插端顶边间的夹角
						//应考虑插板切角后，生成的两个点那个距离碰撞钢管较近
						//判断依据：根据垂直与基准钢管的方向和碰撞钢管的延伸方向之间的角度sina，
						//          插板上切角后形成的斜边与垂直与基准钢管的方向之间的角度sinb
						//			1.sina=sinb 切角生成的两点到碰撞钢管的距离相等
						//			2.sina>sinb 切角后生的点中离钢管心线较远的点离碰撞钢管近
						//			3.sina<sinb 切角后生的点中离钢管心线较近的点离碰撞钢管近
						double sinb = N / sqrt(N*N + S * S);
						if (sina >= sinb)
							offset_dist = H * 0.5*sina / cosa - N;
						else //if(sina<sinb)
							offset_dist = (H*0.5 - S)*sina / cosa;
					}
					else
						offset_dist = H * 0.5*sina / cosa;
				}
				else	//因钢管外径引起的负头部分
					offset_dist = GetDiameter()*0.5*sina / cosa;
				double pole_width = 0;
				if (pLinePart->GetClassTypeId() == CLS_LINETUBE)
				{
					CLDSLineTube *pLineTube = (CLDSLineTube*)pLinePart;
					pole_width = pLineTube->GetDiameter()*0.5;
					CLDSParamPlate *pTempPlate = NULL;
					if (i == 1 && pStart&&pEnd&&pLineTube->pStart&&pLineTube->pEnd)
					{
						long hCurNode = pEnd->handle;
						if (bStart)
							hCurNode = pStart->handle;
						if (hCurNode == pLineTube->pStart->handle)
							pTempPlate = (CLDSParamPlate*)BelongModel()->FromPartHandle(pLineTube->m_tJointStart.hLinkObj, CLS_PARAMPLATE);
						else if (hCurNode == pLineTube->pEnd->handle)
							pTempPlate = (CLDSParamPlate*)BelongModel()->FromPartHandle(pLineTube->m_tJointEnd.hLinkObj, CLS_PARAMPLATE);
					}
					if (pTempPlate && (pTempPlate->m_iParamType == TYPE_ROLLEND
						|| pTempPlate->m_iParamType == TYPE_UEND || pTempPlate->m_iParamType == TYPE_XEND))
					{	//碰撞参照杆件2,碰撞检测端连接有插板时取插板宽度与钢管直径中的较大值 wht 10-11-11
						double fPlateWidth = 0;
						pTempPlate->GetDesignItemValue('H', &fPlateWidth);
						pole_width = max(fPlateWidth, pLineTube->GetDiameter())*0.5;
					}
				}
				else if (pLinePart->GetClassTypeId() == CLS_LINEANGLE)
				{	//碰撞基准杆件为角钢
					CLDSLineAngle *pLineAngle = (CLDSLineAngle*)pLinePart;
					JGZJ jgzj;
					getjgzj(jgzj, pLineAngle->GetWidth());
					pole_width = pLineAngle->GetWidth() - jgzj.g;
				}
				double oddment = -(pole_width + pCurDesOddment->m_fCollideDist) / cosa - offset_dist - extra_odd;
				oddment = fto_halfi(oddment / 10) * 10;
				if (bStart)
				{
					if (oddment < start_oddment)
						start_oddment = oddment;
				}
				else
				{
					if (oddment < end_oddment)
						end_oddment = oddment;
				}
			}
			else
			{
				BOOL bCollide = TRUE;
				while (bCollide)
				{
					bCollide = FALSE;
					for (int i = 0; i < 2; i++)
					{
						if (pPartArr[i] && pPartArr[i]->GetClassTypeId() != CLS_SPHERE)
						{
							if (IsPartsCollide(pPartArr[i], this, ftoi(pCurDesOddment->m_fCollideDist)))
							{
								if (bStart)
									start_oddment -= 5;
								else
									end_oddment -= 5;
								bCollide = TRUE;
								break;
							}
						}
						else if (pPartArr[i] && pPartArr[i]->GetClassTypeId() == CLS_SPHERE)
						{
							CLDSSphere *pSphere = (CLDSSphere*)pPartArr[i];
							double r = GetDiameter()*0.5 - GetThick();
							double R = pSphere->D*0.5;
							if (R < r)
								break;
							else
							{
								double oddment = sqrt(R*R - r * r) + pCurDesOddment->m_fCollideDist;
								if (bStart)
								{
									if (oddment > fabs(start_oddment))
										start_oddment = -(double)((int)oddment);
								}
								else
								{
									if (oddment > fabs(end_oddment))
										end_oddment = -(double)((int)oddment);
								}
								break;
							}
						}
					}
				}
			}
		}
	}
	else if (pCurDesOddment->m_iOddCalStyle == 1)
	{	//根据切割|相贯计算正负头
		GEPOINT line_vec;
		if (bStart)
			line_vec = Start() - End();
		else
			line_vec = End() - Start();
		normalize(line_vec);
		if (pTubeJoint->type == 0)		//平面切割
		{
			if (!pTubeJoint->cutPlaneNorm.IsZero())
			{
				f3dPoint norm_vec = pTubeJoint->cutPlaneNorm^line_vec;
				f3dPoint vert_vec = norm_vec ^ line_vec;
				f3dPoint cut_vec = norm_vec ^ pTubeJoint->cutPlaneNorm;
				normalize(vert_vec);
				normalize(cut_vec);
				double sina = fabs(cut_vec*vert_vec);
				double cosa = fabs(cut_vec*line_vec);
				f3dPoint inters, pt = bStart ? Start() : End();
				if (sina > EPS&&Int3dlf(inters, pt, line_vec, pTubeJoint->cutPlanePos, pTubeJoint->cutPlaneNorm) > 0)
				{	//得到有效交点
					f3dPoint odd_vec = inters - pt;
					double oddment= odd_vec * line_vec + GetDiameter()*0.5*cosa / sina;
					if(bStart)
						start_oddment = oddment;
					else
						end_oddment = oddment;
				}
			}
		}
		else if (pTubeJoint->type == 1 && pTubeJoint->hLinkObj > 0x20)	//圆柱面钢管相贯
		{
			CLDSLineTube *pLinkTube = (CLDSLineTube*)BelongModel()->FromPartHandle(pTubeJoint->hLinkObj, CLS_LINETUBE);
			int nSlices = m_uSlices;
			if (nSlices <= 0)
			{
				int n = ftoi(Pi*GetDiameter() / 2.5);
				nSlices = max(n, 6);
				nSlices = min(n, 288);
			}
			double slice_angle = 2 * Pi / nSlices;
			double oddment = 0;
			UCS_STRU cs;
			cs.origin = bStart ? Start() : End();
			cs.axis_z = line_vec;
			f3dPoint link_vec = pLinkTube->End() - pLinkTube->Start();
			normalize(link_vec);
			if (fabs(link_vec*cs.axis_z) < EPS_COS)
			{	//非平行共线钢管
				cs.axis_x = link_vec ^ cs.axis_z;
				normalize(cs.axis_x);
				cs.axis_y = cs.axis_z^cs.axis_x;
				double radius = GetDiameter() / 2;
				double link_radius = pLinkTube->GetDiameter()*0.5;
				double sina = sin(slice_angle);	//扇片角正弦
				double cosa = cos(slice_angle);	//扇片角余弦
				double Pi2 = Pi * 2;
				f3dPoint inters1, inters2, prev_pick, pick(radius, 0, 0);
				f3dPoint axis_start = pLinkTube->Start();
				f3dPoint axis_end = pLinkTube->End();
				coord_trans(axis_start, cs, FALSE, TRUE);
				coord_trans(axis_end, cs, FALSE, TRUE);
				for (double angle = 0; angle < Pi2; angle += slice_angle)
				{	//沿当前钢管外圆扫描一圈，检测出最大正头值
					if (angle == 0)
					{
						prev_pick = pick;
						int num = Int3dlc(&inters1, &inters2, pick, f3dPoint(0, 0, 1), axis_start, axis_end, link_radius, link_radius);
						if (num <= 0)	//无有效交点
							continue;
						else if (num == 1)
							oddment = inters1.z;
						else if (num == 2)	//两交点中正头小者为有效交点
							oddment = min(inters1.z, inters2.z);
					}
					else
					{	//由上一顶点逆时针旋转slice_angle度
						pick.x = prev_pick.x*cosa - prev_pick.y*sina;
						pick.y = prev_pick.y*cosa + prev_pick.x*sina;
						prev_pick = pick;
						int num = Int3dlc(&inters1, &inters2, pick, f3dPoint(0, 0, 1), axis_start, axis_end, link_radius, link_radius);
						if (num <= 0)	//无有效交点
							continue;
						else if (num == 1)
						{
							if (inters1.z > oddment)	//比原有最大正头检测值大
								oddment = inters1.z;
						}
						else if (num == 2)	//两交点中正头小者为有效交点
						{
							double odd = min(inters1.z, inters2.z);
							if (odd > oddment)
								oddment = odd;
						}
					}
				}
				if (bStart)
					start_oddment = oddment;
				else
					end_oddment = oddment;
			}
		}
	}
	double fRetOddment=0;
	if(bStart)
	{
		fRetOddment=startOdd(bReturnOddToFL);	//记录计算得到的正负头
		start_oddment=fOldOddment;	//恢复钢管始端正负头
	}
	else
	{
		fRetOddment=endOdd(bReturnOddToFL);	//记录计算得到的正负头
		end_oddment=fOldOddment;	//恢复钢管终端正负头
	}
	return fRetOddment;
}

double CLDSLineTube::startOdd(bool bToFL/*=false*/) const
{
	CLDSParamPlate* pFL=NULL;
	if(!bToFL)
		return start_oddment;
	if(m_tJointStart.type==2||m_tJointStart.type==6)
		pFL=(CLDSParamPlate*)BelongModel()->FromPartHandle(m_tJointStart.hLinkObj,CLS_PARAMPLATE);
	if(pFL==NULL)
		return start_oddment;
	double H=0;
	CLDSStdPartModel stdFlModel;
	if(!pFL->m_bStdPart)
		pFL->GetDesignItemValue('H',&H);
	else if(pFL->GetStdPartModel(&stdFlModel))
	{
		if(pFL->m_iParamType==TYPE_FLD)
			H=stdFlModel.param.fl_param.H;
		else if(pFL->m_iParamType==TYPE_FLP)
			H=stdFlModel.param.fl_param.H1;
		else if(pFL->m_iParamType==TYPE_FLG||pFL->m_iParamType==TYPE_FLR)
			H=stdFlModel.param.fl_param2.fH;
	}
	return start_oddment+H;
}
double CLDSLineTube::endOdd(bool bToFL/*=false*/) const
{
	CLDSParamPlate* pFL=NULL;
	if(!bToFL)
		return end_oddment;
	if(m_tJointEnd.type==2||m_tJointEnd.type==6)
		pFL=(CLDSParamPlate*)BelongModel()->FromPartHandle(m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
	if(pFL==NULL)
		return end_oddment;
	double H=0;
	CLDSStdPartModel stdFlModel;
	if(!pFL->m_bStdPart)
		pFL->GetDesignItemValue('H',&H);
	else if(pFL->GetStdPartModel(&stdFlModel))
	{
		if(pFL->m_iParamType==TYPE_FLD)
			H=stdFlModel.param.fl_param.H;
		else if(pFL->m_iParamType==TYPE_FLP)
			H=stdFlModel.param.fl_param.H1;
		else if(pFL->m_iParamType==TYPE_FLG||pFL->m_iParamType==TYPE_FLR)
			H=stdFlModel.param.fl_param2.fH;
	}
	return end_oddment+H;
}
void CLDSLineTube::CalStartOddment(char ciAutoCalMode/*=0*/,int liInitOddment/*=0*/)
{	
	if(desStartOdd.m_iOddCalStyle==2&&ciAutoCalMode==0)
		return;
	//根据碰撞计算正负头
	CLDSParamPlate *pParamPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(m_tJointStart.hLinkObj,CLS_PARAMPLATE);
	start_oddment=CalOddment(TRUE,pParamPlate);
	DesignStartJointUCS();
}

void CLDSLineTube::CalEndOddment(char ciAutoCalMode/*=0*/,int liInitOddment/*=0*/)
{
	if(desEndOdd.m_iOddCalStyle==2&&ciAutoCalMode==0)	
		return;	
	//根据碰撞计算正负头
	CLDSParamPlate *pParamPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
	end_oddment=CalOddment(FALSE,pParamPlate);
	DesignEndJointUCS();
}
double CLDSLineTube::GetLength()
{
	f3dPoint vec;
	if(GetClassTypeId()==CLS_LINEPART&&pStart&&pEnd)
		vec=pEnd->Position(false)-pStart->Position(false);
	else
		vec=End()-Start();
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if(bHuoQuStart)
		start_oddment=0;
	if(bHuoQuEnd)
		end_oddment=0;
	double start_odd=start_oddment;
	double end_odd=end_oddment;
	f3dPoint line_vec;
	double PRECISION=0.5;	//计算钢管相贯时单侧长度值的精度，暂定为0.5mm
	double HALF_PRECISION=0.25;
	double fSamplingLength=2.5;
	int nSlices=m_uSlices;
	if(nSlices<=0)
	{
		int n = ftoi(Pi*GetDiameter()/fSamplingLength);
		nSlices=max(n,6);
		nSlices=min(n,288);
	}
	double slice_angle=2*Pi/nSlices;
	if(m_tJointStart.type==0)		//平面切割
	{
		line_vec=Start()-End();
		normalize(line_vec);
		if(!m_tJointStart.cutPlaneNorm.IsZero())
		{
			f3dPoint norm_vec=m_tJointStart.cutPlaneNorm^line_vec;
			f3dPoint vert_vec=norm_vec^line_vec;
			f3dPoint cut_vec=norm_vec^m_tJointStart.cutPlaneNorm;
			normalize(vert_vec);
			normalize(cut_vec);
			double sina=fabs(cut_vec*vert_vec);
			double cosa=fabs(cut_vec*line_vec);
			f3dPoint inters;
			if(sina>EPS&&Int3dlf(inters,Start(),line_vec,m_tJointStart.cutPlanePos,m_tJointStart.cutPlaneNorm)>0)
			{	//得到有效交点
				f3dPoint odd_vec=inters-Start();
				start_odd=odd_vec*line_vec+GetDiameter()*0.5*cosa/sina;
				start_odd=min(start_odd,start_oddment);
			}
		}
	}
	else if(m_tJointStart.type==1&&m_tJointStart.hLinkObj>0x20)	//圆柱面钢管相贯
	{
		CLDSLineTube *pLinkTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_tJointStart.hLinkObj,CLS_LINETUBE);
		if(pLinkTube&&pLinkTube->GetDiameter()>GetDiameter())
		{
			UCS_STRU cs;
			cs.origin=Start();
			line_vec=Start()-End();
			normalize(line_vec);
			cs.axis_z=line_vec;
			f3dPoint link_vec=pLinkTube->End()-pLinkTube->Start();
			normalize(link_vec);
			if(fabs(link_vec*cs.axis_z)<EPS_COS)
			{	//非平行共线钢管
				cs.axis_x=link_vec^cs.axis_z;
				normalize(cs.axis_x);
				cs.axis_y=cs.axis_z^cs.axis_x;
				double radius=GetDiameter()/2;
				double link_radius=pLinkTube->GetDiameter()*0.5;
				double sina = sin(slice_angle);	//扇片角正弦
				double cosa = cos(slice_angle);	//扇片角余弦
				double Pi2=Pi*2;
				f3dPoint inters1,inters2,prev_pick,pick(radius,0,0);
				f3dPoint axis_start=pLinkTube->Start();
				f3dPoint axis_end=pLinkTube->End();
				coord_trans(axis_start,cs,FALSE,TRUE);
				coord_trans(axis_end,cs,FALSE,TRUE);
				for(double angle=0;angle<Pi2;angle+=slice_angle)
				{	//沿当前钢管外圆扫描一圈，检测出最大正头值
					if(angle==0)
					{
						prev_pick=pick;
						int num=Int3dlc(&inters1,&inters2,pick,f3dPoint(0,0,1),axis_start,axis_end,link_radius,link_radius);
						if(num<=0)	//无有效交点
							continue;
						else if(num==1)
							start_odd=inters1.z;
						else if(num==2)	//两交点中正头小者为有效交点
							start_odd=min(inters1.z,inters2.z);
					}
					else
					{	//由上一顶点逆时针旋转slice_angle度
						pick.x=prev_pick.x*cosa-prev_pick.y*sina;
						pick.y=prev_pick.y*cosa+prev_pick.x*sina;
						prev_pick=pick;
						int num=Int3dlc(&inters1,&inters2,pick,f3dPoint(0,0,1),axis_start,axis_end,link_radius,link_radius);
						if(num<=0)	//无有效交点
							continue;
						else if(num==1)
						{
							if(inters1.z>start_odd)	//比原有最大正头检测值大
								start_odd=inters1.z;
						}
						else if(num==2)	//两交点中正头小者为有效交点
						{
							double odd=min(inters1.z,inters2.z);
							if(odd>start_odd)
								start_odd=odd;
						}
					}
				}
				if(start_odd>start_oddment)
					start_odd=start_oddment;	//最大正头检测值与原有正头间取小值
			}
		}
	}
	if(m_tJointEnd.type==0)		//平面切割
	{
		if(line_vec.IsZero())
		{
			line_vec=End()-Start();
			normalize(line_vec);
		}
		else
			line_vec*=-1.0;	//反向
		if(!m_tJointEnd.cutPlaneNorm.IsZero())
		{
			f3dPoint norm_vec=m_tJointEnd.cutPlaneNorm^line_vec;
			f3dPoint vert_vec=norm_vec^line_vec;
			f3dPoint cut_vec=norm_vec^m_tJointEnd.cutPlaneNorm;
			normalize(vert_vec);
			normalize(cut_vec);
			double sina=fabs(cut_vec*vert_vec);
			double cosa=fabs(cut_vec*line_vec);
			f3dPoint inters;
			if(sina>EPS&&Int3dlf(inters,End(),line_vec,m_tJointEnd.cutPlanePos,m_tJointEnd.cutPlaneNorm)>0)
			{	//得到有效交点
				f3dPoint odd_vec=inters-End();
				end_odd=odd_vec*line_vec+GetDiameter()*0.5*cosa/sina;
				end_odd=min(end_odd,end_oddment);
			}
		}
	}
	else if(m_tJointEnd.type==1&&m_tJointEnd.hLinkObj>0x20)	//圆柱面钢管相贯
	{
		CLDSLineTube *pLinkTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_tJointEnd.hLinkObj,CLS_LINETUBE);
		if(pLinkTube)
		{
			UCS_STRU cs;
			cs.origin=End();
			line_vec=End()-Start();
			normalize(line_vec);
			cs.axis_z=line_vec;
			f3dPoint link_vec=pLinkTube->End()-pLinkTube->Start();
			normalize(link_vec);
			if(fabs(link_vec*cs.axis_z)<EPS_COS)
			{	//非平行共线钢管
				cs.axis_x=link_vec^cs.axis_z;
				normalize(cs.axis_x);
				cs.axis_y=cs.axis_z^cs.axis_x;
				double radius=GetDiameter()/2;
				double link_radius=pLinkTube->GetDiameter()*0.5;
				double sina = sin(slice_angle);	//扇片角正弦
				double cosa = cos(slice_angle);	//扇片角余弦
				double Pi2=Pi*2;
				f3dPoint inters1,inters2,prev_pick,pick(radius,0,0);
				f3dPoint axis_start=pLinkTube->Start();
				f3dPoint axis_end=pLinkTube->End();
				coord_trans(axis_start,cs,FALSE,TRUE);
				coord_trans(axis_end,cs,FALSE,TRUE);
				for(double angle=0;angle<Pi2;angle+=slice_angle)
				{	//沿当前钢管外圆扫描一圈，检测出最大正头值
					if(angle==0)
					{
						prev_pick=pick;
						int num=Int3dlc(&inters1,&inters2,pick,f3dPoint(0,0,1),axis_start,axis_end,link_radius,link_radius);
						if(num<=0)	//无有效交点
							continue;
						else if(num==1)
							end_odd=inters1.z;
						else if(num==2)	//两交点中正头小者为有效交点
							end_odd=min(inters1.z,inters2.z);
					}
					else
					{	//由上一顶点逆时针旋转slice_angle度
						pick.x=prev_pick.x*cosa-prev_pick.y*sina;
						pick.y=prev_pick.y*cosa+prev_pick.x*sina;
						prev_pick=pick;
						int num=Int3dlc(&inters1,&inters2,pick,f3dPoint(0,0,1),axis_start,axis_end,link_radius,link_radius);
						if(num<=0)	//无有效交点
							continue;
						else if(num==1)
						{
							if(inters1.z>end_odd)	//比原有最大正头检测值大
								end_odd=inters1.z;
						}
						else if(num==2)	//两交点中正头小者为有效交点
						{
							double odd=min(inters1.z,inters2.z);
							if(odd>end_odd)
								end_odd=odd;
						}
					}
				}
				if(end_odd>end_oddment)
					end_odd=end_oddment;	//最大正头检测值与原有正头间取小值
			}
		}
	}
	double len = vec.mod()+start_odd+end_odd;
	return len;
#else
	return vec.mod();
#endif
}
bool CLDSLineTube::IntelliRecogWeldLine()
{
	if(m_tJointStart.hLinkObj>0&&m_tJointEnd.hLinkObj>0)
		return false;	//已设计两端连接，无需再另行设定相贯
	CHashList<CPtInLineCheck> hashLineChecks;
	BOOL pushed=BelongModel()->PushPartStack();
	for(CLDSPart* pPart=BelongModel()->EnumPartFirst(CLS_LINETUBE);pPart;pPart=BelongModel()->EnumPartNext(CLS_LINETUBE))
	{
		CLDSLineTube* pMotherTube=(CLDSLineTube*)pPart;
		CPtInLineCheck* pLineCheck=hashLineChecks.GetValue(pPart->handle);
		if(pLineCheck==NULL)
		{
			f3dPoint lineStdVec=End()-Start();
			normalize(lineStdVec);
			CPtInLineCheck lineCheck(Start()-lineStdVec*startOdd(),End()+lineStdVec*endOdd());
			pLineCheck=hashLineChecks.SetValue(handle,lineCheck);
		}
		if(m_tJointStart.hLinkObj==0)
		{
			bool onLine=false;
			double scale=pLineCheck->CheckPointEx(Start(),&onLine);
			if(scale>0&&scale<1&&onLine)
			{
				m_tJointStart.type=1;
				m_tJointStart.hLinkObj=pMotherTube->handle;
			}
		}
		if(m_tJointEnd.hLinkObj==0)
		{
			bool onLine=false;
			double scale=pLineCheck->CheckPointEx(End(),&onLine);
			if(scale>0&&scale<1&&onLine)
			{
				m_tJointEnd.type=1;
				m_tJointEnd.hLinkObj=pMotherTube->handle;
			}
		}
	}
	BelongModel()->PopPartStack(pushed);
	return true;
}
bool CLDSLineTube::IsWeldWith(double* lineStartCoord,double* lineEndCoord)
{
	f3dPoint lineStart(lineStartCoord),lineEnd(lineEndCoord);
	f3dPoint line_vec=lineEnd-lineStart;
	normalize(line_vec);
	f3dPoint axis_z=End()-Start();
	normalize(axis_z);
	if(fabs(line_vec*axis_z)<EPS_COS)
		return false;	//非轴(纵)向延伸贴边
	f3dPoint vec=0.5*(lineStart+lineEnd)-Start();
	double length=DISTANCE(Start(),End());
	double poslen=vec*axis_z;
	if(poslen<-startOdd()||poslen>length+endOdd())
		return false;	//超出钢管轴向范围
	f3dPoint prjvec=poslen*axis_z;
	f3dPoint vertvec=vec-prjvec;
	double dd=vertvec.mod();
	if(fabs(dd-size.wide*0.5)<1.5)
		return true;//拾取点在钢管外壁上
	return false;
}
bool CLDSLineTube::IsWeldWith(f3dArcLine& arcline)
{
	GEPOINT axis_z=End()-Start();
	CPtInLineCheck lineCheck(Start(),End());
	double justify=lineCheck.CheckPoint(arcline.Center());
	GEPOINT columnnorm=arcline.ColumnNorm();
	if(columnnorm.IsZero())
		columnnorm=arcline.WorkNorm();
	if(fabs(columnnorm*lineCheck.LineStdVec())<EPS_COS)
		return false;
	if(justify>=0&&justify<=1&&fabs(size.wide*0.5-arcline.Radius())<2)
		return true;
	return false;
}

//返回钢管上脚钉的个数 wht 11-04-23
int CLDSLineTube::GetFootNailCount()
{
	PARTSET partSet;
	BelongModel()->GetPartGroupPartSet(handle,partSet);
	int nFootNail=0;
	CLDSPart *pTempPart=NULL;
	for(pTempPart=partSet.GetFirst();pTempPart;pTempPart=partSet.GetNext())
	{	//角钢脚钉
		if(pTempPart->GetClassTypeId()==CLS_LINEANGLE
			&&((CLDSLineAngle*)pTempPart)->m_bFootNail)
			nFootNail++;
		else if(pTempPart->GetClassTypeId()==CLS_LINESLOT)
		{	//槽钢无始终端节点且仅有槽钢上有且仅有一颗螺栓并且为脚钉
			CLDSLineSlot *pLineSlot=(CLDSLineSlot*)pTempPart;
			if(pLineSlot->pStart==NULL&&pLineSlot->pEnd==NULL
				&&pLineSlot->GetLsCount()==1&&pLineSlot->GetFirstLsRef()->GetLsPtr()->IsFootNail())
				nFootNail++;
		}
	}
	return nFootNail;
}

bool CLDSLineTube::IsEqual(CLDSLineTube* pMirTube,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
						   int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{
	if(pChkStyleFlag==NULL||(*pChkStyleFlag)&CHKPARTNO_RODINFO)
	{
		bool bRet=true;
		if(fabs(GetLength()-pMirTube->GetLength())>epsilon)
		{
			if(error_info)
				sprintf(error_info,"{%s}长度不同",(char*)sPartNo);
			bRet=false;
		}
		else if(GetDiameter()-pMirTube->GetDiameter()!=0)
		{
			if(error_info)
				sprintf(error_info,"{%s}直径不同",(char*)sPartNo);
			bRet=false;
		}
		else if(GetThick()-pMirTube->GetThick()!=0)
		{
			if(error_info)
				sprintf(error_info,"{%s}厚度不同",(char*)sPartNo);
			bRet=false;
		}
		else if(cMaterial!=pMirTube->cMaterial)
		{
			if(error_info)
				sprintf(error_info,"{%s}材质不同",(char*)sPartNo);
			bRet=false;
		}
		if(!bRet)
		{
			if(pErrorInfo)
				*pErrorInfo=CHKPARTNO_RODINFO;
			return false;
		}
	}
	if(pChkStyleFlag==NULL||(*pChkStyleFlag)&CHKPARTNO_RODBOLTINFO)
	{
		if(GetFootNailCount()!=pMirTube->GetFootNailCount())
		{
			if(error_info)
				sprintf(error_info,"{%s}脚钉个数不同",(char*)sPartNo);
			if(pErrorInfo)
				*pErrorInfo=CHKPARTNO_RODBOLTINFO;
			return false;
		}
	}
	return true;
}

double CLDSLineTube::CalWeldRoadAngle()
{
	BuildUCS();
	f3dPoint weldRoadVec=nearWeldRoadVec;
	if(weldRoadVec.IsZero())
	{
		weldRoadVec.Set(ucs.axis_z.x,ucs.axis_z.y);
		f3dPoint verify_vec(Start().x+End().x,Start().y+End().y);
		if(weldRoadVec*verify_vec>0)
			weldRoadVec*=-1;
		if(weldRoadVec.x!=0&&weldRoadVec.y!=0)
		{	//不是X轴或Y轴向平行的杆件
			if(weldRoadVec.y<0)
				weldRoadVec.y=-fabs(weldRoadVec.x);
			else
				weldRoadVec.y=fabs(weldRoadVec.x);
		}
		nearWeldRoadVec=weldRoadVec;
	}
	normalize(weldRoadVec);
	vector_trans(weldRoadVec,ucs,FALSE);
	return Cal2dLineAng(0,0,weldRoadVec.x,weldRoadVec.y);
}

void CLDSLineTube::CalWeldRoadVec(double fAngle)
{
	BuildUCS();
	double fRadain = fAngle*RADTODEG_COEF;
	f3dPoint tmpWeldRoadVec = ucs.axis_x;
	RotateVectorAroundVector(tmpWeldRoadVec,fRadain,ucs.axis_z);
	nearWeldRoadVec=tmpWeldRoadVec;
}

int CLDSLineTube::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	char sText[100]="";
	CXhString szText(sText,100);
	if(GetPropID("basicInfo")==0)
		strcpy(sText,"");
	else if(GetPropID("handle")==id)
		sprintf(sText,"0X%X",handle);
	else if(GetPropID("layer")==id)
		strcpy(sText,layer());
	else if(GetPropID("cMaterial")==id)	
	{
		char matStr[20];
		if(QuerySteelMatMark(cMaterial,matStr))
			sprintf(sText,"%s",matStr);
	}
	else if(GetPropID("m_cQualityLevel")==id)
	{
		if(isalpha(m_cQualityLevel))
			sprintf(sText,"%C",m_cQualityLevel);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Default");
#else
			strcpy(sText,"默认");
#endif
	}
	else if(GetPropID("specification")==id)
	{
		char s_d[200],s_t[200];
		sprintf(s_d,"%f",GetDiameter());
		sprintf(s_t,"%f",GetThick());
		SimplifiedNumString(s_d);
		SimplifiedNumString(s_t);
		sprintf(sText,"%sX%s", s_d,s_t); 
	}
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("sPartNo")==id)
		sprintf(sText,"%s",(char*)GetPartNo());
	else if(GetPropID("cfgword")==id)
		//return MakeWordToString(cfgword,valueStr);
	{
		if(layer(0)=='L')	//接腿构件
			return cfgword.GetLegScopeStr(valueStr,50,true);
		else
			return cfgword.GetBodyScopeStr(valueStr);
	}
	else if(GetPropID("relativeObjs")==id)
	{
		for(RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
		{
			if(strlen(sText)==0)
				_snprintf(sText,99,"0X%X",pRelaObj->hObj);
			else
				_snprintf(sText,99,"%s,0X%X",sText,pRelaObj->hObj);
		}
	}
	else if(GetPropID("shadowInfo")==0)
		strcpy(sText,"");
	else if(GetPropID("m_hMotherObj")==id)
	{
		CLDSLineTube *pMotherLineTube=(CLDSLineTube*)MotherObject(false);
		if(pMotherLineTube!=this)
			sprintf(sText,"0X%X",pMotherLineTube->handle);
		else 
			strcpy(sText,"0X0");
	}
	else if(GetPropID("m_hBlockRef")==id)
	{
		CBlockParameter blockPara;
		if (GetBlockParameter(blockPara))
			sprintf(sText,"0X%X",blockPara.m_pBlockRef->handle);
		else 
			strcpy(sText,"0X0");
	}
	else if(GetPropID("m_hBlock")==id)
		strcpy(sText,GetBlockIdName());
	else if(GetPropID("m_uStatMatCoef")==id)
		sprintf(sText,"%d",m_uStatMatCoef);
	else if(GetPropID("m_hPartWeldParent")==id)
		sprintf(sText,"0X%X",m_hPartWeldParent);
	else if(GetPropID("m_bDisplayHole")==id)
	{
		if(m_bDisplayHole)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
	}
	else if(GetPropID("m_bVirtualPart")==id)
	{
		if(m_bVirtualPart)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
	}
	else if(GetPropID("m_hArcLift")==id)
	{
		CLDSArcLift *pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
		if(pLift)
		{
			CString sLiftName;
			sLiftName.Format("%s(%d)",pLift->name,pLift->iNo);
			strncpy(sText,sLiftName,50);
		}
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"None");
#else
			strcpy(sText,"无预拱");
#endif
	}
	else if(GetPropID("length")==id)
	{
		sprintf(sText,"%f",GetLength());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bUserSpecColor")==id)
	{
		if(m_bUserSpecColor)
			sprintf(sText,"是");
		else
			sprintf(sText,"否");
	}
	else if(GetPropID("crMaterial")==id)
		sprintf(sText,"RGB%X",crMaterial);
	else if(GetPropID("sNotes")==id)
		strcpy(sText,sNotes);
	else if(GetPropID("m_bHasWeldRoad")==id)
	{
		if(m_bHasWeldRoad)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
	}
	else if(GetPropID("nearWeldRoadVec.x")==id)
	{
		sprintf(sText,"%f",nearWeldRoadVec.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("weldRoadAngle")==id)
	{
		//返回值为弧度需要转为角度并且在0-360度之间
		double fAngle = this->CalWeldRoadAngle()*DEGTORAD_COEF;
		int iTmp=0;
		if(fAngle>0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360;
		}
		else if(fAngle<0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360+360;
		}
		sprintf(sText,"%f",fAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("nearWeldRoadVec.y")==id)
	{
		sprintf(sText,"%f",nearWeldRoadVec.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_iWeldRoadStyle")==id)
	{
		if(m_iWeldRoadStyle==0)
			strcpy(sText,"0.直接指定");
		else //if (m_iWeldRoadStyle==1)
			strcpy(sText,"1.角度定位");
	}
	else if(GetPropID("nearWeldRoadVec.y")==id)
	{
		sprintf(sText,"%f",nearWeldRoadVec.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("nearWeldRoadVec.z")==id)
	{
		sprintf(sText,"%f",nearWeldRoadVec.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_iNormCalStyle")==id)
	{
		if(m_iNormCalStyle==0)
			strcpy(sText,"0.直接指定");
		else if(m_iNormCalStyle==1)
			strcpy(sText,"1.参照杆件");
		else if(m_iNormCalStyle==2)
			strcpy(sText,"2.共面参照点");
	}
	else if(GetPropID("hWorkNormRefPart")==id)
		sprintf(sText,"0X%X",hWorkNormRefPart);
	else if(GetPropID("workPlaneNorm.x")==id)
	{
		sprintf(sText,"%f",workPlaneNorm.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("workPlaneNorm.y")==id)
	{
		sprintf(sText,"%f",workPlaneNorm.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("workPlaneNorm.z")==id)
	{
		sprintf(sText,"%f",workPlaneNorm.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("pickWorkPlane.x")==id)
	{
		sprintf(sText,"%f",pickWorkPlane[0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("pickWorkPlane.y")==id)
	{
		sprintf(sText,"%f",pickWorkPlane[1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("pickWorkPlane.z")==id)
	{
		sprintf(sText,"%f",pickWorkPlane[2]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cutPlaneNorm.x")==id)
	{
		sprintf(sText,"%f",cutPlaneNorm.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cutPlaneNorm.y")==id)
	{
		sprintf(sText,"%f",cutPlaneNorm.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cutPlaneNorm.z")==id)
	{
		sprintf(sText,"%f",cutPlaneNorm.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cutPlaneOrigin.x")==id)
	{
		sprintf(sText,"%f",cutPlaneOrigin.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cutPlaneOrigin.y")==id)
	{
		sprintf(sText,"%f",cutPlaneOrigin.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cutPlaneOrigin.z")==id)
	{
		sprintf(sText,"%f",cutPlaneOrigin.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("hStart")==id)
	{
		if(pStart)
			sprintf(sText,"0X%X",pStart->handle);
		else
			strcpy(sText,"0X0");
	}
	else if(GetPropID("oddStart")==id)
	{
		sprintf(sText,"%f",startOdd(ciFlTubeOddment==1));
		SimplifiedNumString(sText);
	}
	else if (GetPropID("oddStart.m_iOddCalStyle") == id)
	{
		if (desStartOdd.m_iOddCalStyle == 0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText, "0.Collide");
		else if (desStartOdd.m_iOddCalStyle == 1)
			strcpy(sText, "1.Bolt Check");
		else //if(desStartOdd.m_iOddCalStyle==2)
			strcpy(sText, "2.Specify");
#else
			strcpy(sText, "0.碰撞干涉");
		else if (desStartOdd.m_iOddCalStyle == 1)
			strcpy(sText, "1.相贯检测");
		else //if(desStartOdd.m_iOddCalStyle==2)
			strcpy(sText, "2.指定数值");
#endif
	}
	else if (GetPropID("oddStart.m_fCollideDist") == id)
	{
		sprintf(sText, "%f", desStartOdd.m_fCollideDist);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("oddStart.m_hRefPart1") == id)
		sprintf(sText, "0X%X", desStartOdd.m_hRefPart1);
	else if (GetPropID("oddStart.m_hRefPart2") == id)
		sprintf(sText, "0X%X", desStartOdd.m_hRefPart2);
	else if(GetPropID("jointStart")==id)
	{
		if(m_tJointStart.hLinkObj>0x20)
		{
			if(m_tJointStart.type>1)
			{
				CLDSPart *pPart=BelongModel()->FromPartHandle(m_tJointStart.hLinkObj,CLS_PARAMPLATE);
				if(pPart&&pPart->GetClassTypeId()==CLS_PARAMPLATE)
				{
					CLDSParamPlate* pLinkPlate=(CLDSParamPlate*)pPart;
					CXhChar16 sPartNo=pPart->Label;
					if(pLinkPlate&&pLinkPlate->m_bStdPart&&pLinkPlate->m_iParamType==TYPE_XEND)
					{	//标准十字插板，显示插板代号
						CLDSStdPartModel stdPartModel;
						pLinkPlate->GetStdPartModel(&stdPartModel);
						szText.Printf("%s(0x%X)",stdPartModel.param.insert_plate_param.codeName,pPart->handle);
					}
					else if(sPartNo.GetLength()>1)
						szText.Printf("%s(0x%X)",(char*)sPartNo,pPart->handle);
					else
						sprintf(sText,"0X%X",pPart->handle);
				}
				else
					sprintf(sText,"无连接");
			}
			else if(m_tJointStart.type!=0)
				sprintf(sText,"0X%X",m_tJointStart.hLinkObj);
			else	//平面切割且有端头连接构件
				sprintf(sText,"0X%X /=",m_tJointStart.hLinkObj);
		}
		else if(!m_tJointStart.cutPlaneNorm.IsZero())
			sprintf(sText,"平面切割");
		else
			sprintf(sText,"无连接");
	}
	else if(GetPropID("startPos.x")==id)
	{
		sprintf(sText,"%f",Start().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("startPos.y")==id)
	{
		sprintf(sText,"%f",Start().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("startPos.z")==id)
	{
		sprintf(sText,"%f",Start().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.endPosType")==id)
	{
		if(desStartPos.endPosType==0)
			strcpy(sText,"节点即基点");
		else if(desStartPos.endPosType==1)
			strcpy(sText,"搭接连接");
		else if(desStartPos.endPosType==2)
			strcpy(sText,"点线投影");
		else if(desStartPos.endPosType==3)
			strcpy(sText,"心线交点");
		else //if(desStartPos.endPosType==4)
			strcpy(sText,"指定坐标");
	}
	else if(GetPropID("desStartPos.datum_pos_style")==id)
	{
		if(desStartPos.datum_pos_style==1)
			strcpy(sText,"X肢心线投影");
		else if(desStartPos.datum_pos_style==2)
			strcpy(sText,"Y肢心线投影");
		else if(desStartPos.datum_pos_style==3)
			strcpy(sText,"向楞线平推");
		else if(desStartPos.datum_pos_style==4)
			strcpy(sText,"X肢心线平推");
		else if(desStartPos.datum_pos_style==5)
			strcpy(sText,"Y肢心线平推");
		else //if(desStartPos.datum_pos_style==0)
			strcpy(sText,"向楞线投影");
	}
	else if(GetPropID("desStartPos.hDatumPart")==id)
		sprintf(sText,"0X%X", desStartPos.hDatumPart);
	else if(GetPropID("desStartPos.fNormOffset")==id)
	{
		sprintf(sText,"%f",desStartPos.fNormOffset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.fEccentricDist")==id)
	{
		sprintf(sText,"%f",desStartPos.fEccentricDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.fLenVecOffsetDist")==id)
	{
		sprintf(sText,"%f",desStartPos.fLenVecOffsetDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.hDatumStartPart")==id)
		sprintf(sText,"0X%X", desStartPos.hDatumStartPart); 
	else if(GetPropID("desStartPos.bDatumStartStartEnd")==id)
	{
		if(desStartPos.bDatumStartStartEnd)
			strcpy(sText,"始端");
		else //if(!desStartPos.bDatumStartStartEnd)
			strcpy(sText,"终端");
	}
	else if(GetPropID("desStartPos.hDatumEndPart")==id)
		sprintf(sText,"0X%X", desStartPos.hDatumEndPart); 
	else if(GetPropID("desStartPos.bDatumEndStartEnd")==id)
	{
		if(desStartPos.bDatumEndStartEnd)
			strcpy(sText,"始端");
		else //if(!desStartPos.bDatumEndStartEnd)
			strcpy(sText,"终端");
	}
	else if(GetPropID("hEnd")==id)
	{
		if(pEnd)
			sprintf(sText,"0X%X",pEnd->handle);
		else
			strcpy(sText,"0X0");
	}
	else if(GetPropID("oddEnd")==id)
	{
		sprintf(sText,"%f",endOdd(ciFlTubeOddment==1));
		SimplifiedNumString(sText);
	}
	else if (GetPropID("oddEnd.m_iOddCalStyle") == id)
	{
		if (desEndOdd.m_iOddCalStyle == 0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText, "0.Collide");
		else if (desEndOdd.m_iOddCalStyle == 1)
			strcpy(sText, "1.Bolt Check");
		else //if(desEndOdd.m_iOddCalStyle==2)
			strcpy(sText, "2.Specify");
#else
			strcpy(sText, "0.碰撞干涉");
		else if (desEndOdd.m_iOddCalStyle == 1)
			strcpy(sText, "1.相贯检测");
		else //if(desEndOdd.m_iOddCalStyle==2)
			strcpy(sText, "2.指定数值");
#endif
	}
	else if (GetPropID("oddEnd.m_fCollideDist") == id)
	{
		sprintf(sText, "%f", desEndOdd.m_fCollideDist);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("oddEnd.m_hRefPart1") == id)
		sprintf(sText, "0X%X", desEndOdd.m_hRefPart1);
	else if (GetPropID("oddEnd.m_hRefPart2") == id)
		sprintf(sText, "0X%X", desEndOdd.m_hRefPart2);
	else if(GetPropID("jointEnd")==id)
	{
		if(m_tJointEnd.hLinkObj>0x20)
		{
			if(m_tJointEnd.type>1)
			{
				CLDSPart *pPart=BelongModel()->FromPartHandle(m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
				if(pPart&&pPart->GetClassTypeId()==CLS_PARAMPLATE)
				{
					CLDSParamPlate* pLinkPlate=(CLDSParamPlate*)pPart;
					CXhChar16 sPartNo=pPart->Label;
					if(pLinkPlate&&pLinkPlate->m_bStdPart&&pLinkPlate->m_iParamType==TYPE_XEND)
					{	//标准十字插板,显示插板代号
						CLDSStdPartModel stdPartModel;
						pLinkPlate->GetStdPartModel(&stdPartModel);
						szText.Printf("%s(0x%X)",stdPartModel.param.insert_plate_param.codeName,pPart->handle);
					}
					else if(sPartNo.GetLength()>1)
						szText.Printf("%s(0x%X)",(char*)sPartNo,pPart->handle);
					else
						sprintf(sText,"0X%X",pPart->handle);
				}
				else
					sprintf(sText,"无连接");
			}
			else if(m_tJointEnd.type!=0)
				sprintf(sText,"0X%X",m_tJointEnd.hLinkObj);
			else	//平面切割且有端头连接构件
				sprintf(sText,"0X%X /=",m_tJointEnd.hLinkObj);
		}
		else if(!m_tJointEnd.cutPlaneNorm.IsZero())
			sprintf(sText,"平面切割");
		else
			sprintf(sText,"无连接");
	}
	else if(GetPropID("endPos.x")==id)
	{
		sprintf(sText,"%f",End().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("endPos.y")==id)
	{
		sprintf(sText,"%f",End().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("endPos.z")==id)
	{
		sprintf(sText,"%f",End().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.endPosType")==id)
	{
		if(desEndPos.endPosType==0)
			strcpy(sText,"节点即基点");
		else if(desEndPos.endPosType==1)
			strcpy(sText,"搭接连接");
		else if(desEndPos.endPosType==2)
			strcpy(sText,"点线投影");
		else if(desEndPos.endPosType==3)
			strcpy(sText,"心线交点");
		else //if(desEndPos.endPosType==4)
			strcpy(sText,"指定坐标");
	}
	else if(GetPropID("desEndPos.datum_pos_style")==id)
	{
		if(desEndPos.datum_pos_style==0)
			strcpy(sText,"向楞线投影");
		else if(desEndPos.datum_pos_style==1)
			strcpy(sText,"X肢心线投影");
		else if(desEndPos.datum_pos_style==2)
			strcpy(sText,"Y肢心线投影");
		else if(desEndPos.datum_pos_style==3)
			strcpy(sText,"向楞线平推");
		else if(desEndPos.datum_pos_style==4)
			strcpy(sText,"X肢心线平推");
		else if(desEndPos.datum_pos_style==5)
			strcpy(sText,"Y肢心线平推");
	}
	else if(GetPropID("desEndPos.hDatumPart")==id)
		sprintf(sText,"0X%X", desEndPos.hDatumPart);
	else if(GetPropID("desEndPos.fNormOffset")==id)
	{
		sprintf(sText,"%f",desEndPos.fNormOffset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.fEccentricDist")==id)
	{
		sprintf(sText,"%f",desEndPos.fEccentricDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.fLenVecOffsetDist")==id)
	{
		sprintf(sText,"%f",desEndPos.fLenVecOffsetDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.hDatumStartPart")==id)
		sprintf(sText,"0X%X", desEndPos.hDatumStartPart); 
	else if(GetPropID("desEndPos.bDatumStartStartEnd")==id)
	{
		if(desEndPos.bDatumStartStartEnd)
			strcpy(sText,"始端");
		else //if(!desEndPos.bDatumStartStartEnd)
			strcpy(sText,"终端");
	}
	else if(GetPropID("desEndPos.hDatumEndPart")==id)
		sprintf(sText,"0X%X", desEndPos.hDatumEndPart); 
	else if(GetPropID("desEndPos.bDatumEndStartEnd")==id)
	{
		if(desEndPos.bDatumEndStartEnd)
			strcpy(sText,"始端");
		else //if(!desEndPos.bDatumEndStartEnd)
			strcpy(sText,"终端");
	}
#if defined(__TSA_)||defined(__LDS_)
	else if(GetPropID("size.idClassType")==id)
	{
		if(size_idClassType==CLS_LINEANGLE)
			strcpy(sText, "等肢角钢");
		else if(size_idClassType==CLS_GROUPLINEANGLE)
		{
			if(size_cSubClassType=='T')
				strcpy(sText, "T型组合");
			else if(size_cSubClassType=='D')
				strcpy(sText, "对角组合");
			else if(size_cSubClassType=='X')
				strcpy(sText, "十字组合");
		}
		else if(size_idClassType==CLS_LINETUBE)
			strcpy(sText, "钢管");
		else if(size_idClassType==CLS_LINEFLAT)
			strcpy(sText, "扁钢");
		else if(size_idClassType==CLS_LINESLOT)
			strcpy(sText, "槽钢");
		else if(size_idClassType==0)
			strcpy(sText, "用户定制");
	}
	else if(GetPropID("m_uStatMatNo")==id)
		sprintf(sText,"%d",m_uStatMatNo);
	else if(GetPropID("m_fWeightRaiseCoef")==id)
	{
		sprintf(sText,"%f",m_fWeightRaiseCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_iElemType")==id)	//自动判断单元类型
	{
		if(m_iElemType==0)
			strcpy(sText,"0.自动判断");
		else if(m_iElemType==2)
			strcpy(sText,"2.经典梁");
		else if(m_iElemType==3)
			strcpy(sText,"3.柔索");
		else
			strcpy(sText,"1.二力杆");
	}
	else if(GetPropID("m_bNeedSelMat")==id)
	{
		if(m_bNeedSelMat)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
	}
	else if(GetPropID("m_bNeedSelSize")==id)
	{
		if(m_bNeedSelSize)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
	}
	else if(GetPropID("CalLenCoef.iTypeNo")==id)
	{
		if(CalLenCoef.iTypeNo==0)
			strcpy(sText,"0.智能判断");
		else if(CalLenCoef.iTypeNo==1)
			strcpy(sText,"1.V斜材(跨脚材连接)");
		else if(CalLenCoef.iTypeNo==2)
			strcpy(sText,"2.V斜材(V面连接)");
		else if(CalLenCoef.iTypeNo==3)
			strcpy(sText,"3.V斜材3节间带横连杆");
		else if(CalLenCoef.iTypeNo==4)
			strcpy(sText,"4.V斜材4节间带横连杆");
		else if(CalLenCoef.iTypeNo==5)
			strcpy(sText,"5.指定计算长度");
	}
	else if(GetPropID("CalLenCoef.pHorizPole")==id)
	{
		if(CalLenCoef.hHorizPole>0x20)
			sprintf(sText,"0X%X",CalLenCoef.hHorizPole);
		else
			strcpy(sText,"0X0");
	}
	else if(GetPropID("CalLenCoef.pRefPole")==id)
	{
		if(CalLenCoef.hRefPole>0x20)
			sprintf(sText,"0X%X",CalLenCoef.hRefPole);
		else
			strcpy(sText,"0X0");
	}
	else if(GetPropID("CalLenCoef.spec_callenMinR")==id)
	{
		sprintf(sText,"%f",CalSpecLamdaLenMinR());
		SimplifiedNumString(sText);
	}
	/*else if(GetPropID("CalLenCoef.spec_callenParaR")==id)
	{
		sprintf(sText,"%f",CalSpecLamdaLenParaR());
		SimplifiedNumString(sText);
	}*/
	else if(GetPropID("start_joint_type")==id)
	{
		if(start_joint_type==JOINT_HINGE)
			strcpy(sText,"铰接");
		else if(start_joint_type==JOINT_RIGID)
			strcpy(sText,"刚接");
		else //if(start_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"自动判断");
	}
	else if(GetPropID("start_force_type")==id)
	{
		if(start_force_type==CENTRIC_FORCE)
			strcpy(sText,"中心受力");
		else //if(start_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"偏心受力");
	}
	else if(GetPropID("connectStart.grade")==id)
		strcpy(sText,connectStart.grade);
	else if(GetPropID("connectStart.d")==id)
		sprintf(sText,"M%d",connectStart.d);
	else if(GetPropID("connectStart.N")==id)
		sprintf(sText,"%d",connectStart.wnConnBoltN);
	else if(GetPropID("connectStart.bearloadcoef")==id)
	{
		sprintf(sText,"%f",connectStart.bearloadcoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("end_joint_type")==id)
	{
		if(end_joint_type==JOINT_HINGE)
			strcpy(sText,"铰接");
		else if(end_joint_type==JOINT_RIGID)
			strcpy(sText,"刚接");
		else //if(end_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"自动判断");
	}
	else if(GetPropID("end_force_type")==id)
	{
		if(end_force_type==CENTRIC_FORCE)
			strcpy(sText,"中心受力");
		else //if(end_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"偏心受力");
	}
	else if(GetPropID("connectEnd.grade")==id)
		strcpy(sText,connectEnd.grade);
	else if(GetPropID("connectEnd.d")==id)
		sprintf(sText,"M%d",connectEnd.d);
	else if(GetPropID("connectEnd.N")==id)
		sprintf(sText,"%d",connectEnd.wnConnBoltN);
	else if(GetPropID("connectEnd.bearloadcoef")==id)
	{
		sprintf(sText,"%f",connectEnd.bearloadcoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bAutoCalHoleOutCoef")==id)
	{
		if(m_bAutoCalHoleOutCoef)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
	}
	else if(GetPropID("hole_out")==id)
	{
		sprintf(sText,"%f",hole_out);
		SimplifiedNumString(sText);
	}
#endif
	else if(GetPropID("bush.height")==id)
	{
		sprintf(sText,"%d",bush.height);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("bush.hBushResidePlate")==id)
		sprintf(sText,"0X%X",bush.hBushResidePlate);
	else if(GetPropID("bush.hBushRelyBolt")==id)
		sprintf(sText,"0X%X",bush.hBushRelyBolt);
#ifdef AFX_TARG_ENU_ENGLISH
	if(strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("0X%X Tube's property##d string value out of length",handle,id);
#else
	if(strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("钢管0X%X属性#%d字符串超出长度限制",handle,id);
#endif
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}

BOOL CLDSLineTube::IsPropEqual(HASHOBJSET &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSObject *pFirstObj = selectObjs.GetFirst();
	CLDSLineTube *pLineTube=NULL, *pFirstLineTube = (CLDSLineTube*)pFirstObj;
	pFirstLineTube->GetPropValueStr(idProp,valueStr);
	for(pLineTube=(CLDSLineTube*)selectObjs.GetNext();pLineTube;pLineTube=(CLDSLineTube*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pLineTube->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

BOOL CLDSLineTube::GetObjPropReadOnlyDefaultState(long id)
{
	return GetObjPropReadOnlyDefaultState(id,ShadowMode());
}
BOOL CLDSLineTube::GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode)
{
	if (shadow_mode==CLDSObject::SHADOWMODE_COMMON)
	{
		if(GetPropID("layer")==id)
			return FALSE;
		else if(GetPropID("cMaterial")==id)
			return FALSE;
		else if(GetPropID("m_cQualityLevel")==id)
			return FALSE;
		else if(GetPropID("m_hPartWeldParent")==id)
			return FALSE;
		else if(GetPropID("specification")==id)
			return FALSE;
		else if(GetPropID("sPartNo")==id)
			return FALSE;
		else if(GetPropID("cfgword")==id)
			return FALSE;
		else if(GetPropID("m_uStatMatCoef")==id)
			return FALSE;
		else if(GetPropID("sNotes")==id)
			return FALSE;
		else if(GetPropID("relativeObjs")==id)
			return FALSE;
		else
			return TRUE;
	}
	else if(shadow_mode==CLDSObject::SHADOWMODE_BLOCK)
	{
		if(GetPropID("layer")==id)
			return FALSE;
		else if(GetPropID("m_hPartWeldParent")==id)
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if(GetPropID("handle")==id)
			return TRUE;
		else if(GetPropID("length")==id)
			return TRUE;
		else
			return FALSE;
	}
}

void CLDSLineTube::CalStartPos(CLDSArcLift *pLift/*=NULL*/)
{
	if(!LockStart())
		return;	//已到位
	if(pStart==NULL)
		return;
#ifdef __COMPLEX_PART_INC_
	if(pLift==NULL&&m_hArcLift>0x20)
		pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
#else
	pLift=NULL;
#endif
	if(IsStartPosLocked())
		return;	//始端位置已被锁定（可能是火曲点）
	SetModified();
	if(desStartPos.endPosType==0)
		SetStart(pStart->Position(true)+workPlaneNorm*desStartPos.fNormOffset);
	else if(desStartPos.endPosType==1)
	{	//搭接连接
		long hDatumPart=0;
		if(desStartPos.hDatumPart>0x20)
			hDatumPart=desStartPos.hDatumPart;
		else if(pStart)
			hDatumPart=pStart->hFatherPart;
		CLDSLinePart *pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(hDatumPart,CLS_LINETUBE,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pDatumLinePart==NULL)
		{
			logerr.Log("计算钢管0x%X始端位置时,未找到可用的基准杆件，计算失败！",handle);
			return;
		}
		f3dPoint startPos;
		if(pDatumLinePart->GetClassTypeId()==CLS_LINETUBE)
		{
			CLDSLineTube *pDatumTube=(CLDSLineTube*)pDatumLinePart;
			f3dLine datumLine(pDatumTube->Start(),pDatumTube->End());
			f3dPoint datum_vec=datumLine.endPt-datumLine.startPt;
			f3dPoint current_vec=End()-Start();
			f3dPoint datumPos,plane_norm,eccentricOffsetVec;
			plane_norm=datum_vec^current_vec;
			normalize(plane_norm);
			if(plane_norm*workPlaneNorm<0)
				plane_norm=-plane_norm;
			SnapPerp(&datumPos,datumLine,pStart->Position(true));
			eccentricOffsetVec=plane_norm^datum_vec;
			if(eccentricOffsetVec*current_vec<0)
				eccentricOffsetVec*=-1.0;
			normalize(eccentricOffsetVec);
			/*f3dPoint endPos=datumPos+eccentricOffsetVec*desStartPos.fEccentricDist+
			workPlaneNorm*desStartPos.fNormOffset;//以前是plane_norm*desStartPos.fNormOffset*/
			startPos=datumPos;
			if(fabs(desStartPos.fEccentricDist)>EPS)
			{	//偏心距
				if(desStartPos.face_offset_norm.IsZero()) //未指定平推面法线
					startPos+=eccentricOffsetVec*desStartPos.fEccentricDist;
				else
				{	//指定平推面法线
					datumLine.startPt+=eccentricOffsetVec*desStartPos.fEccentricDist;
					datumLine.endPt+=eccentricOffsetVec*desStartPos.fEccentricDist;
					f3dPoint face_norm=desStartPos.face_offset_norm;
					if(pLift)
					{
						bool bInRightSide=true;
						if(pLift->m_cRotAxis=='Y'&&datumPos.x<0)
							bInRightSide=false;
						else if(pLift->m_cRotAxis=='X'&&datumPos.y>0)
							bInRightSide=false;
						face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
					}
					Int3dlf(startPos,datumLine,datumPos,face_norm);
				}
			}
		}
		else if(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE||pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)pDatumLinePart;
			if(desStartPos.datum_pos_style==0)	//以父角钢楞线为基准
				startPos = pDatumLineAngle->GetDatumPosBer(pStart);
			else if(desStartPos.datum_pos_style==1)	//以父角钢X支为基准
				startPos = pDatumLineAngle->GetDatumPosWingX(pStart);
			else if(desStartPos.datum_pos_style==2)	//以父角钢Y支为基准
				startPos = pDatumLineAngle->GetDatumPosWingY(pStart);
			else if(desStartPos.datum_pos_style==3)	//以父角钢楞线为基准平推
				startPos = pDatumLineAngle->GetDatumPosBerByFaceOffset(pStart,desStartPos.face_offset_norm);
			else if(desStartPos.datum_pos_style==4)	//以父角钢X肢为基准平推
				startPos = pDatumLineAngle->GetDatumPosWingXByFaceOffset(pStart,desStartPos.face_offset_norm);
			else //if(desStartPos.datum_pos_style==5)//以父角钢Y肢为基准平推
				startPos = pDatumLineAngle->GetDatumPosWingYByFaceOffset(pStart,desStartPos.face_offset_norm);
			//基准杆件为角钢时生成双面板时可能需要调整偏心距(与角钢定位方式中的投影楞线、X肢、Y肢火曲线作用相同) wht 10-07-08
			if(fabs(desStartPos.fEccentricDist)>EPS)
			{	//偏心距
				f3dPoint eccentricOffsetVec;
				if(desStartPos.datum_pos_style==1||desStartPos.datum_pos_style==4)
					eccentricOffsetVec=pDatumLineAngle->GetWingVecX();
				else if(desStartPos.datum_pos_style==2||desStartPos.datum_pos_style==5)
					eccentricOffsetVec=pDatumLineAngle->GetWingVecY();
				else
				{
					if(fabs(workPlaneNorm*pDatumLineAngle->get_norm_x_wing())>fabs(workPlaneNorm*pDatumLineAngle->get_norm_y_wing()))
						eccentricOffsetVec=pDatumLineAngle->GetWingVecX();
					else
						eccentricOffsetVec=pDatumLineAngle->GetWingVecY();
				}
				//投影 沿基准角钢肢延伸方向，偏移desStartPos.fEccentricDist
				if(desStartPos.datum_pos_style<3)
					startPos+=eccentricOffsetVec*desStartPos.fEccentricDist;
				else if(desStartPos.face_offset_norm.IsZero()) //平推且指定平推面法线
				{	
					f3dPoint datumPos;
					f3dLine datumLine(pDatumLineAngle->Start(),pDatumLineAngle->End());
					SnapPerp(&datumPos,datumLine,pStart->Position(true));
					double offset_dist=0;
					JGZJ jgzj;
					if(desStartPos.datum_pos_style==4)
					{	//以父角钢X肢为基准平推
						jgzj=pDatumLineAngle->xWingXZhunJu;	
						eccentricOffsetVec=pDatumLineAngle->GetWingVecX();
					}
					else if(desStartPos.datum_pos_style==5)
					{	//以父角钢Y肢为基准平推
						jgzj=pDatumLineAngle->xWingYZhunJu;
						eccentricOffsetVec=pDatumLineAngle->GetWingVecY();
					}
					if(!pDatumLineAngle->m_bEnableTeG)
						getjgzj(jgzj,pDatumLineAngle->GetWidth());
					if(desStartPos.datum_pos_style>4)
						offset_dist=jgzj.g+desStartPos.fEccentricDist;
					else	//以父角钢楞线为基准平推
						offset_dist=desStartPos.fEccentricDist;
					datumLine.startPt+=eccentricOffsetVec*offset_dist;
					datumLine.endPt+=eccentricOffsetVec*offset_dist;
					f3dPoint face_norm=desStartPos.face_offset_norm;
					if(pLift)
					{
						bool bInRightSide=true;
						if(pLift->m_cRotAxis=='Y'&&datumPos.x<0)
							bInRightSide=false;
						else if(pLift->m_cRotAxis=='X'&&datumPos.y>0)
							bInRightSide=false;
						face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
					}
					Int3dlf(startPos,datumLine,datumPos,face_norm);
				}
			}
		}
		else
		{
			logerr.Log("计算钢管(0x%X)始端位置时,搭接连接基准杆件目前仅支持角钢和钢管！",handle);
			return;
		}
		f3dPoint len_offset_vec=pDatumLinePart->End()-pDatumLinePart->Start();
		normalize(len_offset_vec);
		//考虑钢管始端沿所搭接基准杆件长度方向的偏移距离 wht 11-04-23
		startPos+=len_offset_vec*desStartPos.fLenVecOffsetDist;	
		startPos+=workPlaneNorm*desStartPos.fNormOffset;//以前是plane_norm*desEndPos.fNormOffset
		SetStart(startPos);
	}
	else if(desStartPos.endPosType==2)
	{	//点线投影
		CLDSLineTube *pStartDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desStartPos.hDatumStartPart,CLS_LINETUBE);
		CLDSLineTube *pEndDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desStartPos.hDatumEndPart,CLS_LINETUBE);
		if(pStartDatumTube==NULL||pEndDatumTube==NULL)
			return;
		f3dPoint datumPosStart,datumPosEnd,pos;
		if(desStartPos.bDatumStartStartEnd)
			datumPosStart=pStartDatumTube->Start();
		else
			datumPosStart=pStartDatumTube->End();
		if(desStartPos.bDatumEndStartEnd)
			datumPosEnd=pEndDatumTube->Start();
		else
			datumPosEnd=pEndDatumTube->End();
		//下面代码被之后三行代码代替以解决V面上交叉材为钢管时,因钢管偏心导致单线节(交)点非实体钢管轴心交点,进而引起跨越连板位置错误 wjh-2013.10.13
		//SnapPerp(&pos,datumPosStart,datumPosEnd,pStart->Position(true));
		f3dPoint init_pos=pStart->Position(true);
		pStart->GetIntersZhun(&init_pos);
		SnapPerp(&pos,datumPosStart,datumPosEnd,init_pos);
		pos+=workPlaneNorm*desStartPos.fNormOffset;//考虑法向偏移量
		SetStart(pos);
	}
	//增加心线交点以及指定坐标两种定位方式 wht 10-10-19
	else if(desStartPos.endPosType==3)
	{	//心线交点(两杆件交点)
		CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desStartPos.hDatumPart,CLS_LINETUBE);		//基准钢管
		CLDSLineTube *pCrossTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desStartPos.hDatumStartPart,CLS_LINETUBE);//交叉钢管
		if(pDatumTube==NULL||pCrossTube==NULL)
		{
			logerr.Log("计算钢管0x%X始端位置时,交叉杆件句柄不可用,计算失败！",handle);
			return;
		}
		f3dPoint inters_pt;
		f3dLine datum_line,cross_line;
		datum_line.startPt=pDatumTube->Start();
		datum_line.endPt=pDatumTube->End();
		cross_line.startPt=pCrossTube->Start();
		cross_line.endPt=pCrossTube->End();
		//int ret=Int3dll(datum_line,cross_line,inters_pt);
		int ret=Int3dDifferPlaneLL(datum_line.startPt,datum_line.endPt,cross_line.startPt,cross_line.endPt,inters_pt);
		if(ret!=0)
		{
			inters_pt+=workPlaneNorm*desStartPos.fNormOffset;//考虑法向偏移量 wht 11-07-01
			SetStart(inters_pt);
		}
		else	//未求得合法交点 
		{
			logerr.Log("计算钢管0x%X始端位置时,0x%X与0x%X平行或重合或定义错误，交点计算错误！",handle,pDatumTube->handle,pCrossTube->handle);
			return;
		}
	}
	else if(desStartPos.endPosType==4)//直接指定坐标
		return;
}
void CLDSLineTube::CalEndPos(CLDSArcLift *pLift/*=NULL*/)
{
	if(!LockEnd())
		return;	//已到位
	if(pEnd==NULL)
		return;
#ifdef __COMPLEX_PART_INC_
	if(pLift==NULL&&m_hArcLift>0x20)
		pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
#else
	pLift=NULL;
#endif
	if(IsEndPosLocked())
		return;	//始端位置已被锁定（可能是火曲点）
	SetModified();
	if(desEndPos.endPosType==0)
		SetEnd(pEnd->Position(true)+workPlaneNorm*desEndPos.fNormOffset);
	else if(desEndPos.endPosType==1)
	{	//搭接连接
		long hDatumPart=0;
		if(desEndPos.hDatumPart>0x20)
			hDatumPart=desEndPos.hDatumPart;
		else if(pEnd)
			hDatumPart=pEnd->hFatherPart;
		CLDSLinePart *pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(hDatumPart,CLS_LINETUBE,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pDatumLinePart==NULL)
		{
			logerr.Log("计算钢管0x%X终端位置时,未找到可用的基准杆件，计算失败！",handle);
			return;
		}
		f3dPoint endPos;
		if(pDatumLinePart->GetClassTypeId()==CLS_LINETUBE)
		{
			CLDSLineTube *pDatumTube=(CLDSLineTube*)pDatumLinePart;
			f3dLine datumLine(pDatumTube->Start(),pDatumTube->End());
			f3dPoint datum_vec=datumLine.endPt-datumLine.startPt;
			f3dPoint current_vec=Start()-End();
			f3dPoint datumPos,plane_norm,eccentricOffsetVec;
			plane_norm=datum_vec^current_vec;
			normalize(plane_norm);
			if(plane_norm*workPlaneNorm<0)
				plane_norm = -plane_norm;
			SnapPerp(&datumPos,datumLine,pEnd->Position(true));
			eccentricOffsetVec=plane_norm^datum_vec;
			if(eccentricOffsetVec*current_vec<0)
				eccentricOffsetVec*=-1.0;
			normalize(eccentricOffsetVec);
			/*
			f3dPoint endPos=datumPos+eccentricOffsetVec*desEndPos.fEccentricDist+
			workPlaneNorm*desEndPos.fNormOffset;//以前是plane_norm*desEndPos.fNormOffset
			*/
			endPos=datumPos;
			if(fabs(desEndPos.fEccentricDist)>EPS)
			{	//偏心距
				if(desEndPos.face_offset_norm.IsZero()) //未指定平推面法线
					endPos+=eccentricOffsetVec*desEndPos.fEccentricDist;
				else
				{	//指定平推面法线
					datumLine.startPt+=eccentricOffsetVec*desEndPos.fEccentricDist;
					datumLine.endPt+=eccentricOffsetVec*desEndPos.fEccentricDist;
					f3dPoint face_norm=desEndPos.face_offset_norm;
					if(pLift)
					{
						bool bInRightSide=true;
						if(pLift->m_cRotAxis=='Y'&&datumPos.x<0)
							bInRightSide=false;
						else if(pLift->m_cRotAxis=='X'&&datumPos.y>0)
							bInRightSide=false;
						face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
					}
					Int3dlf(endPos,datumLine,datumPos,face_norm);
				}
			}
		}
		else if(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE||pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)pDatumLinePart;
			if(desEndPos.datum_pos_style==0)	//以父角钢楞线为基准
				endPos = pDatumLineAngle->GetDatumPosBer(pEnd);
			else if(desEndPos.datum_pos_style==1)	//以父角钢X支为基准
				endPos = pDatumLineAngle->GetDatumPosWingX(pEnd);
			else if(desEndPos.datum_pos_style==2)	//以父角钢Y支为基准
				endPos = pDatumLineAngle->GetDatumPosWingY(pEnd);
			else if(desEndPos.datum_pos_style==3)	//以父角钢楞线为基准平推
				endPos = pDatumLineAngle->GetDatumPosBerByFaceOffset(pEnd,desEndPos.face_offset_norm);
			else if(desEndPos.datum_pos_style==4)	//以父角钢X肢为基准平推
				endPos = pDatumLineAngle->GetDatumPosWingXByFaceOffset(pEnd,desEndPos.face_offset_norm);
			else //if(desEndPos.datum_pos_style==5)//以父角钢Y肢为基准平推
				endPos = pDatumLineAngle->GetDatumPosWingYByFaceOffset(pEnd,desEndPos.face_offset_norm);
			//基准杆件为角钢时生成双面板时可能需要调整偏心距(与角钢定位方式中的投影楞线、X肢、Y肢火曲线作用相同) wht 10-07-08
			if(fabs(desEndPos.fEccentricDist)>EPS)
			{	//偏心距
				f3dPoint eccentricOffsetVec;
				if(desEndPos.datum_pos_style==1||desEndPos.datum_pos_style==4)
					eccentricOffsetVec=pDatumLineAngle->GetWingVecX();
				else if(desEndPos.datum_pos_style==2||desEndPos.datum_pos_style==5)
					eccentricOffsetVec=pDatumLineAngle->GetWingVecY();
				else
				{
					if(fabs(workPlaneNorm*pDatumLineAngle->get_norm_x_wing())>fabs(workPlaneNorm*pDatumLineAngle->get_norm_y_wing()))
						eccentricOffsetVec=pDatumLineAngle->GetWingVecX();
					else
						eccentricOffsetVec=pDatumLineAngle->GetWingVecY();
				}
				//投影 沿基准角钢肢延伸方向，偏移desEndPos.fEccentricDist
				if(desEndPos.datum_pos_style<3)
					endPos+=eccentricOffsetVec*desEndPos.fEccentricDist;
				else if(desEndPos.face_offset_norm.IsZero()) //平推且指定平推面法线
				{	
					f3dPoint datumPos;
					f3dLine datumLine(pDatumLineAngle->Start(),pDatumLineAngle->End());
					SnapPerp(&datumPos,datumLine,pStart->Position(true));
					double offset_dist=0;
					JGZJ jgzj;
					if(desEndPos.datum_pos_style==4)
					{	//以父角钢X肢为基准平推
						jgzj=pDatumLineAngle->xWingXZhunJu;	
						eccentricOffsetVec=pDatumLineAngle->GetWingVecX();
					}
					else if(desEndPos.datum_pos_style==5)
					{	//以父角钢Y肢为基准平推
						jgzj=pDatumLineAngle->xWingYZhunJu;
						eccentricOffsetVec=pDatumLineAngle->GetWingVecY();
					}
					if(!pDatumLineAngle->m_bEnableTeG)
						getjgzj(jgzj,pDatumLineAngle->GetWidth());
					if(desStartPos.datum_pos_style>4)
						offset_dist=jgzj.g+desEndPos.fEccentricDist;
					else	//以父角钢楞线为基准平推
						offset_dist=desEndPos.fEccentricDist;
					datumLine.startPt+=eccentricOffsetVec*offset_dist;
					datumLine.endPt+=eccentricOffsetVec*offset_dist;
					f3dPoint face_norm=desEndPos.face_offset_norm;
					if(pLift)
					{
						bool bInRightSide=true;
						if(pLift->m_cRotAxis=='Y'&&datumPos.x<0)
							bInRightSide=false;
						else if(pLift->m_cRotAxis=='X'&&datumPos.y>0)
							bInRightSide=false;
						face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
					}
					Int3dlf(endPos,datumLine,datumPos,face_norm);
				}
			}
		}
		else
		{
			logerr.Log("计算钢管0x%X终端位置时,搭接连接基准杆件目前仅支持角钢和钢管！",handle);
			return;
		}
		f3dPoint len_offset_vec=pDatumLinePart->End()-pDatumLinePart->Start();
		normalize(len_offset_vec);
		//考虑钢管终端沿所搭接基准杆件长度方向的偏移距离 wht 11-04-23
		endPos+=len_offset_vec*desEndPos.fLenVecOffsetDist;	
		endPos+=workPlaneNorm*desEndPos.fNormOffset;//以前是plane_norm*desEndPos.fNormOffset
		SetEnd(endPos);
	}
	else if(desEndPos.endPosType==2)
	{	//点线投影
		CLDSLineTube *pStartDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desEndPos.hDatumStartPart,CLS_LINETUBE);
		CLDSLineTube *pEndDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desEndPos.hDatumEndPart,CLS_LINETUBE);
		if(pStartDatumTube==NULL||pEndDatumTube==NULL)
			return;
		if(pStartDatumTube!=this)
			pStartDatumTube->CalPosition();
		if(pEndDatumTube!=this)
			pEndDatumTube->CalPosition();
		f3dPoint datumPosStart,datumPosEnd,pos;
		if(desEndPos.bDatumStartStartEnd)
			datumPosStart=pStartDatumTube->Start();
		else
			datumPosStart=pStartDatumTube->End();
		if(desEndPos.bDatumEndStartEnd)
			datumPosEnd=pEndDatumTube->Start();
		else
			datumPosEnd=pEndDatumTube->End();
		//下面代码被之后三行代码代替以解决V面上交叉材为钢管时,因钢管偏心导致单线节(交)点非实体钢管轴心交点,进而引起跨越连板位置错误 wjh-2013.10.13
		//SnapPerp(&pos,datumPosStart,datumPosEnd,pEnd->Position(true));
		f3dPoint init_pos=pEnd->Position(true);
		pEnd->GetIntersZhun(&init_pos);
		SnapPerp(&pos,datumPosStart,datumPosEnd,init_pos);
		pos+=workPlaneNorm*desEndPos.fNormOffset;//考虑法向偏移量
		SetEnd(pos);
	}
	//增加心线交点以及指定坐标两种定位方式 wht 10-10-19
	else if(desEndPos.endPosType==3)
	{	//心线交点(两杆件交点)
		CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desEndPos.hDatumPart,CLS_LINETUBE);		//基准钢管
		CLDSLineTube *pCrossTube=(CLDSLineTube*)BelongModel()->FromPartHandle(desEndPos.hDatumStartPart,CLS_LINETUBE);//交叉钢管
		if(pDatumTube==NULL||pCrossTube==NULL)
		{
			logerr.Log("计算钢管0x%X终端位置时,交叉杆件句柄不可用,计算失败！",handle);
			return;
		}
		f3dPoint inters_pt;
		f3dLine datum_line,cross_line;
		datum_line.startPt=pDatumTube->Start();
		datum_line.endPt=pDatumTube->End();
		cross_line.startPt=pCrossTube->Start();
		cross_line.endPt=pCrossTube->End();
		//int ret=Int3dll(datum_line,cross_line,inters_pt);
		int ret=Int3dDifferPlaneLL(datum_line.startPt,datum_line.endPt,cross_line.startPt,cross_line.endPt,inters_pt);
		if(ret!=0)
		{
			inters_pt+=workPlaneNorm*desEndPos.fNormOffset;//考虑法向偏移量 wht 11-07-01
			SetEnd(inters_pt);
		}
		else	//未求得合法交点 
		{
			logerr.Log("计算钢管0x%X终端位置时,0x%X与0x%X平行或重合或定义错误，交点计算错误！",handle,pDatumTube->handle,pCrossTube->handle);
			return;
		}
	}
	else if(desEndPos.endPosType==4)//直接指定坐标
		return;
}

void CLDSLineTube::CalPosition(bool bIgnoreRepeatFlag/*=FALSE*/)
{
	if(IsShadowObject())
	{
		CalShadowRodPos();
		return;
	}
	if(bIgnoreRepeatFlag)
		ClearFlag();
	if(FuncType()==FUNC_WIREBUSH||pStart==NULL||pEnd==NULL)
	{	//挂线孔套管
		CLDSBolt* pRelyBolt=(CLDSBolt*)BelongModel()->FromPartHandle(bush.hBushRelyBolt,CLS_BOLT);
		CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(bush.hBushResidePlate,CLS_PLATE,CLS_PARAMPLATE);
		if(pPlate==NULL)
		{
			Log2File()->Log("套管0x%X定位基准钢板错误!",handle);
			return;
		}
		if(bush.height<1)
		{
			Log2File()->Log("0x%X套管长度为零!",handle);
			return;
		}
		m_iNormCalStyle=0;	//直接指定基准面法线
		GEPOINT workPlaneNorm=pPlate->ucs.axis_z;
		GEPOINT workPlanePick=pPlate->ucs.origin+pPlate->ucs.axis_z*pPlate->GetNormOffset();
		GEPOINT location;
		if(pRelyBolt)
		{	//有基准螺栓时，根据螺栓计算套管始终端
			if(pRelyBolt->des_base_pos.hPart!=bush.hBushResidePlate)
			{
				Log2File()->Log("套管0x%X螺栓定位基准构件不是套管的焊接依赖钢板",handle);
				return;
			}
			BYTE reside_plane=pRelyBolt->des_base_pos.reside_plane;
			if(pRelyBolt->des_base_pos.reside_plane>=pPlate->GetFaceN())
				reside_plane=0;
			if(reside_plane==0)	//钢板基面上的螺栓
			{
				workPlaneNorm=pPlate->ucs.axis_z;
				workPlanePick=pPlate->ucs.origin+pPlate->ucs.axis_z*pPlate->GetNormOffset();
			}
			else //if(pRelyBolt->des_base_pos.reside_plane<pPlate->face_N)
			{
				workPlaneNorm=pPlate->GetHuoquFaceNorm(reside_plane-1);
				f3dLine huoquline=pPlate->GetHuoquLine(reside_plane-1);
				f3dPoint inters;	//必须进行一次投影，否则可能会由于拾取点过远导致误差 wjh-2016.3.22
				SnapPerp(&inters,huoquline.startPt,huoquline.endPt,f3dPoint(0,0,0));
				workPlanePick=inters;
				workPlanePick.z+=pPlate->GetNormOffset();
				coord_trans(workPlanePick,pPlate->ucs,TRUE,TRUE);
			}
			if(workPlaneNorm.IsZero())
			{
				Log2File()->Log("0x%X套管的工作面法线为空!",handle);
				return;
			}
			Int3dlf(location,pRelyBolt->ucs.origin,pRelyBolt->ucs.axis_z,workPlanePick,workPlaneNorm);
			double odd=0.5*(bush.height-pPlate->GetThick());
			baseline.SetStart(location.x-odd*workPlaneNorm.x,location.y-odd*workPlaneNorm.y,location.z-odd*workPlaneNorm.z);
			odd+=pPlate->GetThick();
			baseline.SetEnd(location.x+odd*workPlaneNorm.x,location.y+odd*workPlaneNorm.y,location.z+odd*workPlaneNorm.z);
		}
		else
		{	//直接指定套管定位点
			xBushLocation.UpdatePos();
			location=xBushLocation.datum_pos;
			Int3dlf(location,location,workPlaneNorm,workPlanePick,workPlaneNorm);
			double odd=0.5*bush.height;
			baseline.SetStart(location.x-odd*workPlaneNorm.x,location.y-odd*workPlaneNorm.y,location.z-odd*workPlaneNorm.z);
			baseline.SetEnd(location.x+odd*workPlaneNorm.x,location.y+odd*workPlaneNorm.y,location.z+odd*workPlaneNorm.z);
		}
		SetModified();
		return;
	}
	CLDSArcLift *pLift=NULL;
	if(m_hArcLift>0x20)
		pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
	CalStartPos(pLift);
	CalEndPos(pLift);
}

BOOL CLDSLineTube::CreateRoundBar3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{	//钢筋实体生成
	fBody body;
	fBody *pBody=&body;
	COLORREF tube_color=GetColor(CLS_LINETUBE,3,cMaterial);
	if(m_bUserSpecColor)	//用户指定构件颜色
		tube_color=crMaterial;
	int slices;
	if(m_uDisplaySlices>0)
		slices= m_uDisplaySlices;
	else
		slices= CalArcResolution(GetDiameter()*0.5,2*Pi,scale_of_user2scr,sample_len,smoothness);
	slices=min(slices,200);	//防止后面临时建立的outter_down_vertex_arr数组越界
	double coshalf_alfa = cos(Pi/slices);	//半角余弦
	double D=GetDiameter()/coshalf_alfa;
	double high=DISTANCE(Start(),End())+startOdd()+endOdd();

	BuildUCS();
	double outer_radius = D*0.5;
	fPtList inters_list;
	f3dPoint outter_down_vertex_arr[200];
	int i;
	GetCircleSimuPolyVertex(outer_radius,outter_down_vertex_arr,slices);
	UCS_STRU tmucs=ucs;
	tmucs.origin=Start()-ucs.axis_z*startOdd();
	for(i=0;i<slices;i++)
	{
		f3dPoint *pOutterDown,*pOutterUp;
		pOutterDown=pBody->vertex.append(outter_down_vertex_arr[i]);	//底面外圆弧点
		coord_trans(*pOutterDown,tmucs,TRUE,TRUE);
		outter_down_vertex_arr[i].z=high;
		pOutterUp=pBody->vertex.append(outter_down_vertex_arr[i]);	//顶面外圆弧点
		coord_trans(*pOutterUp,tmucs,TRUE,TRUE);
	}
	f3dPoint down_center,up_center;
	down_center.Set(0,0,0);
	up_center.Set(0,0,high);
	f3dPoint *pCenterDown,*pCenterUp;
	pCenterDown=pBody->vertex.append(down_center);	//底面中心
	coord_trans(*pCenterDown,tmucs,TRUE,TRUE);
	pCenterUp=pBody->vertex.append(up_center);	//顶面中心
	coord_trans(*pCenterUp,tmucs,TRUE,TRUE);
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);

	f3dPoint axis_pick_start,axis_pick_end;
	//钢管底面创建
	f3dPolyFace *pFace;
	COLORREF crCutFaceOrPolyJoint=GetColor(CLS_POLYJOINT,-1,' ');
	for(i=0;i<slices;i++)
	{
		//由于不共面，所以整个底面拆分成若干三角面
		pFace=pBody->faceList.append();
		pFace->material = tube_color;
		createFace.NewOutterEdgeLine(pFace,slices*2+0,i*2);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*2);
		createFace.NewOutterEdgeLine(pFace,i*2);
	}
	//钢管顶面创建
	for(i=0;i<slices;i++)
	{
		//由于不共面，所以整个底面拆分成若干三角面
		pFace=pBody->faceList.append();
		pFace->material = tube_color;
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*2+1,i*2+1);
		createFace.NewOutterEdgeLine(pFace,slices*2+1);
		createFace.NewOutterEdgeLine(pFace,i*2+1);
	}
	//钢管内/外侧面创建
	for(i=0;i<slices;i++)
	{
		//钢管外侧面创建
		pFace = pBody->faceList.append();
		pFace->material = tube_color;
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*2+0,i*2);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*2+1);
		createFace.NewOutterEdgeLine(pFace,i*2+1);
		createFace.NewOutterEdgeLine(pFace,i*2);
	}
	pSolidBody->ConvertFrom(pBody);
	SetModified(FALSE);	//防止重复生成
	return TRUE;
}

BOOL CLDSLineTube::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	if(CreateShadowPart3dSolidModel(bDisplayHole,scale_of_user2scr,sample_len,smoothness))
		return TRUE;

	if(pSolidBody!=NULL&& !IsSolidModified())
		return TRUE;
	else
	{
		if(pSolidBody == NULL)
			pSolidBody = new CSolidBody();
		else
			pSolidBody->Empty();
	}
	SetModified(FALSE);
	if (GetThick()<=0||GetThick()>=GetDiameter()*0.5)
		return CreateRoundBar3dSolidModel(bDisplayHole,scale_of_user2scr,sample_len,smoothness);
	fBody body;
	fBody *pBody=&body;
	COLORREF tube_color=GetColor(CLS_LINETUBE,3,cMaterial);
	if(m_bUserSpecColor)	//用户指定构件颜色
		tube_color=crMaterial;
	int slices;
	if(m_uDisplaySlices>0)
		slices= m_uDisplaySlices;
	else
		slices= CalArcResolution(GetDiameter()*0.5,2*Pi,scale_of_user2scr,sample_len,smoothness);
	slices=min(slices,200);	//防止后面临时建立的outter_down_vertex_arr数组越界
	//slices=4;
	double coshalf_alfa = cos(Pi/slices);	//半角余弦
	double D=GetDiameter()/coshalf_alfa,T=GetThick()/coshalf_alfa;
	double high=DISTANCE(Start(),End())+startOdd()+endOdd();
	
	BuildUCS();
	double outer_radius = D*0.5;
	double inner_radius = outer_radius-T;
	BOOL bStartCutPlane=FALSE,bStartCutPlaneFinished=FALSE;
	BOOL bEndCutPlane=FALSE,bEndCutPlaneFinished=FALSE;
	if(m_tJointStart.type==0||(m_tJointStart.type==1&&m_tJointStart.hViceLinkObj<0))
	{
		f3dPoint inters;
		bStartCutPlane=TRUE;
		if(Int3dlf(inters,ucs.origin,ucs.axis_z,m_tJointStart.cutPlanePos,m_tJointStart.cutPlaneNorm)>0)
		{
			f3dPoint oddVec=inters-Start();
			double odd=startOdd()+oddVec*ucs.axis_z;
			double cosa=fabs(m_tJointStart.cutPlaneNorm*ucs.axis_z);
			double sina=sqrt(1-cosa*cosa);
			double oddlen=inner_radius*sina/cosa;
			if(oddlen<=odd+EPS)
				bStartCutPlaneFinished=TRUE;
		}
	}
	if(m_tJointEnd.type==0||(m_tJointEnd.type==1&&m_tJointEnd.hViceLinkObj<0))
	{
		f3dPoint inters;
		bEndCutPlane=TRUE;
		if(Int3dlf(inters,ucs.origin,ucs.axis_z,m_tJointEnd.cutPlanePos,m_tJointEnd.cutPlaneNorm)>0)
		{
			f3dPoint oddVec=inters-End();
			double odd=endOdd()-oddVec*ucs.axis_z;
			double cosa=fabs(m_tJointEnd.cutPlaneNorm*ucs.axis_z);
			double sina=sqrt(1-cosa*cosa);
			double oddlen=inner_radius*sina/cosa;
			if(oddlen<=odd+EPS)
				bEndCutPlaneFinished=TRUE;
		}
	}
	BOOL bViceCutStartPlane = FALSE,bViceCutPlaneFinished=TRUE;
	if(!cutPlaneNorm.IsZero() && cutPlaneNorm*ucs.axis_z<0)
		bViceCutStartPlane = TRUE;
	fPtList inters_list;
	f3dPoint outter_down_vertex_arr[200];
	int i;
	GetCircleSimuPolyVertex(outer_radius,outter_down_vertex_arr,slices);
	UCS_STRU tmucs=ucs;
	tmucs.origin=Start()-ucs.axis_z*startOdd();
	double scaleRI=inner_radius/outer_radius;
	for(i=0;i<slices;i++)
	{
		f3dPoint *pOutterDown,*pOutterUp,*pInnerDown,*pInnerUp;
		pOutterDown=pBody->vertex.append(outter_down_vertex_arr[i]);	//底面外圆弧点
		coord_trans(*pOutterDown,tmucs,TRUE,TRUE);
		outter_down_vertex_arr[i].z=high;
		pOutterUp=pBody->vertex.append(outter_down_vertex_arr[i]);	//顶面外圆弧点
		coord_trans(*pOutterUp,tmucs,TRUE,TRUE);
		outter_down_vertex_arr[i].x *= scaleRI;
		outter_down_vertex_arr[i].y *= scaleRI;
		outter_down_vertex_arr[i].z=0;
		pInnerDown=pBody->vertex.append(outter_down_vertex_arr[i]);	//底面内圆弧点
		coord_trans(*pInnerDown,tmucs,TRUE,TRUE);
		outter_down_vertex_arr[i].z=high;
		pInnerUp=pBody->vertex.append(outter_down_vertex_arr[i]);	//顶面内圆弧点
		coord_trans(*pInnerUp,tmucs,TRUE,TRUE);
		if(bStartCutPlaneFinished)
		{
			Int3dlf(*pOutterDown,*pOutterDown,ucs.axis_z,m_tJointStart.cutPlanePos,m_tJointStart.cutPlaneNorm);
			Int3dlf(*pInnerDown,*pInnerDown,ucs.axis_z,m_tJointStart.cutPlanePos,m_tJointStart.cutPlaneNorm);
		}
		if(bEndCutPlaneFinished)
		{
			Int3dlf(*pOutterUp,*pOutterUp,ucs.axis_z,m_tJointEnd.cutPlanePos,m_tJointEnd.cutPlaneNorm);
			Int3dlf(*pInnerUp,*pInnerUp,ucs.axis_z,m_tJointEnd.cutPlanePos,m_tJointEnd.cutPlaneNorm);
		}
		if(!cutPlaneNorm.IsZero()&&bViceCutPlaneFinished)
		{
			f3dPoint inters,verify_vec;
			if(bViceCutStartPlane)
			{
				if(Int3dlf(inters,*pOutterDown,ucs.axis_z,cutPlaneOrigin,cutPlaneNorm)>0)
				{
					verify_vec=inters-*pOutterDown;
					if(verify_vec*ucs.axis_z>0)
					{
						inters_list.append(inters);
						if(Int3dlf(inters,*pInnerDown,ucs.axis_z,cutPlaneOrigin,cutPlaneNorm)>0)
						{
							verify_vec=inters-*pInnerDown;
							if(verify_vec*ucs.axis_z<0)
								bViceCutPlaneFinished=FALSE;
							else
								inters_list.append(inters);
						}
						else
							bViceCutPlaneFinished=FALSE;
					}
					else
						bViceCutPlaneFinished=FALSE;
				}
				else
					bViceCutPlaneFinished=FALSE;
			}
			else
			{
				if(Int3dlf(inters,*pOutterUp,ucs.axis_z,cutPlaneOrigin,cutPlaneNorm)>0)
				{
					verify_vec=inters-*pOutterUp;
					if(verify_vec*ucs.axis_z<0)
					{
						inters_list.append(inters);
						if(Int3dlf(inters,*pInnerUp,ucs.axis_z,cutPlaneOrigin,cutPlaneNorm)>0)
						{
							verify_vec=inters-*pInnerUp;
							if(verify_vec*ucs.axis_z>0)
								bViceCutPlaneFinished=FALSE;
							else
								inters_list.append(inters);
						}
						else
							bViceCutPlaneFinished=FALSE;
					}
					else
						bViceCutPlaneFinished=FALSE;
				}
				else
					bViceCutPlaneFinished=FALSE;
			}
		}
	}
	if(bViceCutPlaneFinished)
	{	//附加切割面全部切割管一端
		BOOL bFirstVertex=TRUE;
		for(f3dPoint *pFinalVertex=inters_list.GetFirst();pFinalVertex;pFinalVertex=inters_list.GetNext())
		{
			f3dPoint *pVertex=NULL;
			//外侧
			if(bFirstVertex)
			{
				pVertex=pBody->vertex.GetFirst();
				bFirstVertex=FALSE;
			}
			else
				pVertex=pBody->vertex.GetNext();
			if(!bViceCutStartPlane)	//切割终端(即Up端)
				pVertex=pBody->vertex.GetNext();
			pVertex->Set(pFinalVertex->x,pFinalVertex->y,pFinalVertex->z);
			if(bViceCutStartPlane)	//切割始端(即Down端)
				pVertex=pBody->vertex.GetNext();
			//内侧
			pFinalVertex=inters_list.GetNext();
			pVertex=pBody->vertex.GetNext();
			if(!bViceCutStartPlane)	//切割终端(即Up端)
				pVertex=pBody->vertex.GetNext();
			pVertex->Set(pFinalVertex->x,pFinalVertex->y,pFinalVertex->z);
			if(bViceCutStartPlane)	//切割始端(即Down端)
				pVertex=pBody->vertex.GetNext();
		}
	}
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);

	f3dPoint axis_pick_start,axis_pick_end;
	//钢管底面创建
	f3dPolyFace *pFace;
	COLORREF crCutFaceOrPolyJoint=GetColor(CLS_POLYJOINT,-1,' ');
	for(i=0;i<slices;i++)
	{
		//由于不共面，所以整个底面拆分成若干三角面
		pFace=pBody->faceList.append();
		if(bStartCutPlaneFinished)
			pFace->material=crCutFaceOrPolyJoint;
		
		else	//始端标记为黄色, 与单线始端标记一致 wjh-2013.8.4
			pFace->material = RGB(255,255,0);
		createFace.NewOutterEdgeLine(pFace,i*4+2,i*4);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4);
		createFace.NewOutterEdgeLine(pFace,i*4);
		pFace->poly_norm=-ucs.axis_z;
		pFace=pBody->faceList.append();
		if(bStartCutPlaneFinished)
			pFace->material=crCutFaceOrPolyJoint;
		else	//始端标记为黄色, 与单线始端标记一致 wjh-2013.8.4
			pFace->material = RGB(255,255,0);
		createFace.NewOutterEdgeLine(pFace,i*4+2,((i+1)%slices)*4);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+2);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4);
		pFace->poly_norm=-ucs.axis_z;
	}
	//钢管顶面创建
	for(i=0;i<slices;i++)
	{
		//由于不共面，所以整个底面拆分成若干三角面
		pFace=pBody->faceList.append();
		if(bEndCutPlaneFinished)
			pFace->material=crCutFaceOrPolyJoint;
		else
			pFace->material = tube_color;
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+1,i*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4+3);
		createFace.NewOutterEdgeLine(pFace,i*4+1);
		pFace->poly_norm=ucs.axis_z;
		pFace=pBody->faceList.append();
		if(bEndCutPlaneFinished)
			pFace->material=crCutFaceOrPolyJoint;
		else
			pFace->material = tube_color;
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+3,((i+1)%slices)*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4+3);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+1);
		pFace->poly_norm=ucs.axis_z;
	}
	//钢管内/外侧面创建
	for(i=0;i<slices;i++)
	{
		//钢管外侧面创建
		pFace = pBody->faceList.append();
		pFace->material = tube_color;
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4,i*4);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4);
		f3dPoint vertex1=createFace.VertexAt(((i+1)%slices)*4);
		f3dPoint vertex2=createFace.VertexAt(((i+1)%slices)*4+1);
		f3dPoint vertex3=createFace.VertexAt(i*4+1);
		f3dPoint vec1=vertex3-vertex2;
		f3dPoint vec2=vertex1-vertex2;
		pFace->poly_norm=vec1^vec2;
		normalize(pFace->poly_norm);
		//钢管内侧面创建
		pFace = pBody->faceList.append();
		pFace->material = tube_color;
		createFace.NewOutterEdgeLine(pFace,i*4+2,((i+1)%slices)*4+2);
		createFace.NewOutterEdgeLine(pFace,i*4+3);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+3);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+2);
		
		vertex1=createFace.VertexAt(i*4+2);
		vertex2=createFace.VertexAt(i*4+3);
		vertex3=createFace.VertexAt(((i+1)%slices)*4+3);
		vec1=vertex3-vertex2;
		vec2=vertex1-vertex2;
		pFace->poly_norm=vec1^vec2;
		normalize(pFace->poly_norm);
	}
	if(bStartCutPlane&&!bStartCutPlaneFinished)
		pBody->CutByPlane(m_tJointStart.cutPlanePos,m_tJointStart.cutPlaneNorm,crCutFaceOrPolyJoint);
	else if(CLDSLineTube::bDisplaySolidTransectFace&&m_tJointStart.type==1&&m_tJointStart.hViceLinkObj>=0)	//始端是圆柱面相贯线
	{
		CLDSLineTube *pLinkTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_tJointStart.hLinkObj,CLS_LINETUBE);
		if(pLinkTube)
		{
			if(pLinkTube->m_uDisplaySlices>0)
				slices= pLinkTube->m_uDisplaySlices;
			else
				slices= CalArcResolution(pLinkTube->GetDiameter()*0.5,2*Pi,scale_of_user2scr,sample_len);
			f3dPoint extend_vec;
			fPtList band_plane_vertex_list;
			double length=pLinkTube->GetCutCylinderFace(slices,band_plane_vertex_list,extend_vec);
			pBody->CutByBandPlaneList(band_plane_vertex_list,extend_vec,length,crCutFaceOrPolyJoint);
		}
		CLDSLineTube *pViceLinkTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_tJointStart.hViceLinkObj,CLS_LINETUBE);
		if(pViceLinkTube)
		{
			if(pViceLinkTube->m_uDisplaySlices>0)
				slices= pViceLinkTube->m_uDisplaySlices;
			else
				slices= CalArcResolution(pViceLinkTube->GetDiameter()*0.5,2*Pi,scale_of_user2scr,sample_len);
			f3dPoint extend_vec;
			fPtList band_plane_vertex_list;
			double length=pViceLinkTube->GetCutCylinderFace(slices,band_plane_vertex_list,extend_vec);
			pBody->CutByBandPlaneList(band_plane_vertex_list,extend_vec,length,crCutFaceOrPolyJoint);
		}
	}
	if(bEndCutPlane&&!bEndCutPlaneFinished)
		pBody->CutByPlane(m_tJointEnd.cutPlanePos,m_tJointEnd.cutPlaneNorm,crCutFaceOrPolyJoint);
	else if(CLDSLineTube::bDisplaySolidTransectFace&&m_tJointEnd.type==1&&m_tJointEnd.hViceLinkObj>=0)	//终端是圆柱面相贯线
	{ 
		CLDSLineTube *pLinkTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_tJointEnd.hLinkObj,CLS_LINETUBE);
		if(pLinkTube)
		{
			if(pLinkTube->m_uDisplaySlices>0)
				slices= pLinkTube->m_uDisplaySlices;
			else
				slices= CalArcResolution(pLinkTube->GetDiameter()*0.5,2*Pi,scale_of_user2scr,sample_len);
			f3dPoint extend_vec;
			fPtList band_plane_vertex_list;
			double length=pLinkTube->GetCutCylinderFace(slices,band_plane_vertex_list,extend_vec);
			pBody->CutByBandPlaneList(band_plane_vertex_list,extend_vec,length,crCutFaceOrPolyJoint);
		}
		CLDSLineTube *pViceLinkTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_tJointEnd.hViceLinkObj,CLS_LINETUBE);
		if(pViceLinkTube)
		{
			if(pViceLinkTube->m_uDisplaySlices>0)
				slices= pViceLinkTube->m_uDisplaySlices;
			else
				slices= CalArcResolution(pViceLinkTube->GetDiameter()*0.5,2*Pi,scale_of_user2scr,sample_len);
			f3dPoint extend_vec;
			fPtList band_plane_vertex_list;
			double length=pViceLinkTube->GetCutCylinderFace(slices,band_plane_vertex_list,extend_vec);
			pBody->CutByBandPlaneList(band_plane_vertex_list,extend_vec,length,crCutFaceOrPolyJoint);
		}
	}
	if(!cutPlaneNorm.IsZero()&&!bViceCutPlaneFinished)
		pBody->CutByPlane(cutPlaneOrigin,cutPlaneNorm,crCutFaceOrPolyJoint);
	if(CLDSLineTube::bDisplayLineVecMark&&pStart!=NULL&&pEnd!=NULL)
	{	//套管等非常规连接杆件，不需要会制焊缝与起始边标记　wjh-2016.1.09
		double half_t=0.5*size_thick;
		f3dPoint vertex_bottom,vertex_top,vertex_start,radius_vec;
		vertex_start = Start()-ucs.axis_z*startOdd()+ucs.axis_x*outer_radius;
		vertex_bottom=vertex_start+ucs.axis_x*half_t+ucs.axis_z*2;
		f3dPoint vertex_bottom_left =vertex_bottom+(ucs.axis_y-ucs.axis_x)*GetThick();
		f3dPoint vertex_bottom_right=vertex_bottom-(ucs.axis_y+ucs.axis_x)*GetThick();
		f3dPoint vertex_up=vertex_bottom+ucs.axis_z*(high-4);
		f3dPoint vertex_up_left=vertex_bottom_left+ucs.axis_z*(high-4);
		f3dPoint vertex_up_right=vertex_bottom_right +ucs.axis_z*(high-4);
		for(i=0;i<2;i++)
		{
			if(i==1)
			{
				if(m_bHasWeldRoad)
				{
					double weldRoadAngle=CalWeldRoadAngle();
					if(weldRoadAngle==0)
						break;	//焊道线计算失败
					rotate_point_around_axis(vertex_bottom,weldRoadAngle,Start(),End());
					rotate_point_around_axis(vertex_bottom_left,weldRoadAngle,Start(),End());
					rotate_point_around_axis(vertex_bottom_right,weldRoadAngle,Start(),End());
					rotate_point_around_axis(vertex_up,weldRoadAngle,Start(),End());
					rotate_point_around_axis(vertex_up_left,weldRoadAngle,Start(),End());
					rotate_point_around_axis(vertex_up_right,weldRoadAngle,Start(),End());
				}
				else	//该钢管没有焊道线
					break;
			}
			int iMarkStartIndex=pBody->vertex.GetNodeNum();
			if(m_tJointStart.type==0&&!m_tJointStart.cutPlaneNorm.IsZero())
			{
				f3dPoint inters[3],vec;
				Int3dlf(inters[0],vertex_bottom,ucs.axis_z,m_tJointStart.cutPlanePos,m_tJointStart.cutPlaneNorm);
				Int3dlf(inters[1],vertex_bottom_left,ucs.axis_z,m_tJointStart.cutPlanePos,m_tJointStart.cutPlaneNorm);
				Int3dlf(inters[2],vertex_bottom_right,ucs.axis_z,m_tJointStart.cutPlanePos,m_tJointStart.cutPlaneNorm);
				vec=inters[0]-vertex_bottom;
				if(vec*ucs.axis_z>0)
					vertex_bottom=inters[0];
				vec=inters[1]-vertex_bottom_left;
				if(vec*ucs.axis_z>0)
					vertex_bottom_left=inters[1];
				vec=inters[2]-vertex_bottom_right;
				if(vec*ucs.axis_z>0)
					vertex_bottom_right=inters[2];
			}
			if(m_tJointEnd.type==0&&!m_tJointEnd.cutPlaneNorm.IsZero())
			{
				f3dPoint inters[3],vec;
				Int3dlf(inters[0],vertex_up,ucs.axis_z,m_tJointEnd.cutPlanePos,m_tJointEnd.cutPlaneNorm);
				Int3dlf(inters[1],vertex_up_left,ucs.axis_z,m_tJointEnd.cutPlanePos,m_tJointEnd.cutPlaneNorm);
				Int3dlf(inters[2],vertex_up_right,ucs.axis_z,m_tJointEnd.cutPlanePos,m_tJointEnd.cutPlaneNorm);
				vec=inters[0]-vertex_up;
				if(vec*ucs.axis_z<0)
					vertex_up=inters[0];
				vec=inters[1]-vertex_up_left;
				if(vec*ucs.axis_z<0)
					vertex_up_left=inters[1];
				vec=inters[2]-vertex_up_right;
				if(vec*ucs.axis_z<0)
					vertex_up_right=inters[2];
			}
			pBody->vertex.append(vertex_bottom);
			pBody->vertex.append(vertex_bottom_left);
			pBody->vertex.append(vertex_bottom_right);
			pBody->vertex.append(vertex_up);
			pBody->vertex.append(vertex_up_left);
			pBody->vertex.append(vertex_up_right);
			//CutByPlane,CutByBandPlaneList的的点无法添加所以重新Init
			createFace.InitVertexList(pBody);
			//标志起始基准面
			pFace=pBody->faceList.append();
			if(i==0)	//起始边线
				pFace->material=RGB(255,0,0);
			else		//焊道线
				pFace->material=RGB(0,0,0);
			createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+3,iMarkStartIndex);
			createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+5);
			createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+2);
			createFace.NewOutterEdgeLine(pFace,iMarkStartIndex);
			//标志旋转方向面
			pFace=pBody->faceList.append();
			if(i==0)	//起始边线
				pFace->material=RGB(0,255,0);
			else		//焊道线
				pFace->material=RGB(0,0,0);
			createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+1,iMarkStartIndex);
			createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+4);
			createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+3);
			createFace.NewOutterEdgeLine(pFace,iMarkStartIndex);
			//标志上端面
			pFace=pBody->faceList.append();
			if(i==0)	//起始边线
				pFace->material=RGB(0,0,255);
			else		//焊道线
				pFace->material=tube_color;
			createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+2,iMarkStartIndex);
			createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+1);
			createFace.NewOutterEdgeLine(pFace,iMarkStartIndex);
			//标志下端面
			pFace=pBody->faceList.append();
			if(i==0)	//起始边线
				pFace->material=RGB(0,0,255);
			else		//焊道线
				pFace->material=tube_color;
			createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+4,iMarkStartIndex+3);
			createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+5);
			createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+3);
		}
	}
	pSolidBody->ConvertFrom(pBody);
	SetModified(FALSE);	//防止重复生成
	return TRUE;
}
//当前构件是否可以在指定绘图坐标系(负Z轴为视线方向)下遮掩住另一构件
bool CLDSLineTube::IsCanHide(CLDSPart* pPart,const double* dcs_norm)
{
	pPart->Create3dSolidModel();
	if(pPart->pSolidBody==NULL)
		return true;
	f3dLine axisline=f3dLine(Start(),End());
	f3dPoint axisvec=axisline.endPt-axisline.startPt;
	double length=axisvec.mod()+startOdd()+endOdd();
	normalize(axisvec);
	axisline.startPt-=axisvec*startOdd();
	axisline.endPt+=axisvec*endOdd();
	GECS sect_cs;	//钢管绘制短形
	sect_cs.axis_x=axisvec;
	sect_cs.axis_y=f3dPoint(dcs_norm)^axisvec;
	if(!normalize(sect_cs.axis_y))
		return true;	//视线方向沿钢管轴线方向
	sect_cs.axis_z=sect_cs.axis_x^sect_cs.axis_y;
	if(!normalize(sect_cs.axis_z))
		return true;
	if(sect_cs.axis_z*f3dPoint(dcs_norm)<0)
	{
		sect_cs.axis_y*=-1.0;
		sect_cs.axis_z*=-1.0;
	}
	sect_cs.origin=axisline.startPt-0.5*size.wide*sect_cs.axis_y;	//原点设在矩形左下角
	for(int i=0;i<pPart->pSolidBody->KeyPointNum();i++)
	{
		GEPOINT vertice=pPart->pSolidBody->GetKeyPointAt(i);
		GEPOINT P=sect_cs.TransPToCS(vertice);
		if(P.z>0)
			return false;
		double cosa=f3dPoint(dcs_norm)*sect_cs.axis_z;
		P=vertice+f3dPoint(dcs_norm)*(-P.z/cosa);
		P=sect_cs.TransPToCS(P);
		if(P.x<0||P.x>length||P.y<0||P.y>size.wide)
			return false;
	}
	return true;
}
bool CLDSLineTube::GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* cReserved/*=NULL*/,int iIntersPlane/*=0*/)
{
	f3dPoint perp;
	if(SnapPerp(&perp,Start(),End(),pBolt->ucs.origin)&&inters_coords)
	{
		memcpy(inters_coords,(double*)perp,24);
		return true;
	}
	else if(inters_coords)
		memcpy(inters_coords,(double*)pBolt->ucs.origin,24);
	return false;
}
void CLDSLineTube::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	int h=0;
	buffer.ReadInteger(&h);
	pStart=BelongModel()->FromNodeHandle(h);
	buffer.ReadInteger(&h);
	pEnd=BelongModel()->FromNodeHandle(h);
	pLsRefList->FromBuffer(buffer,BelongModel(),version,doc_type);

	if( version==0||
		(doc_type==1&&version>=4020005)||	//TMA(V4.2.0.5)
		(doc_type==2&&version>=1020005)||	//LMA(V1.2.0.5)
		(doc_type==3&&version>=2000005)||	//TSA(V2.0.0.5)
		(doc_type==4&&version>=1020005)||	//LDS(V1.2.0.5)
		(doc_type==5&&version>=1020005))	//TDA(V1.2.0.5)
	{
		buffer.ReadDword(&dwPermission);
		buffer.ReadDword(&dwStartPermission);
		buffer.ReadDword(&dwEndPermission);
		CfgwordFromBuffer(cfgword,buffer,version,doc_type);
		BelongModel()->EvolveCfgword(cfgword,version,doc_type);
		buffer.ReadInteger(&m_uDisplaySlices);
		buffer.ReadInteger(&m_uSlices);
		buffer.ReadInteger(&m_uStatMatCoef);
		buffer.ReadInteger(&_hPartWeldParent);
		buffer.ReadWord(&m_iNormCalStyle);
		buffer.ReadDouble(&pickWorkPlane[0]);
		buffer.ReadDouble(&pickWorkPlane[1]);
		buffer.ReadDouble(&pickWorkPlane[2]);
		if( version==0||
			(doc_type==1&&version>=5010001)||	//TMA(V5,1,0,1)
			(doc_type==2&&version>=2000500)||	//LMA(V2,0,5,0)
			(doc_type==4&&version>=1030500)||	//LDS(V1,3,5,0)
			(doc_type==5&&version>=1030500))	//TDA(V1,3,5,0)
			buffer.ReadInteger(&hTransTube);	//一直以来漏记这个属性　wjh-2016.4.12
		buffer.ReadInteger(&hWorkNormRefPart);
		buffer.ReadDouble(&workPlaneNorm.x);
		buffer.ReadDouble(&workPlaneNorm.y);
		buffer.ReadDouble(&workPlaneNorm.z);
		buffer.ReadDouble(&cutPlaneOrigin.x);
		buffer.ReadDouble(&cutPlaneOrigin.y);
		buffer.ReadDouble(&cutPlaneOrigin.z);
		buffer.ReadDouble(&cutPlaneNorm.x);
		buffer.ReadDouble(&cutPlaneNorm.y);
		buffer.ReadDouble(&cutPlaneNorm.z);
		if(	version==0||
			(doc_type==1&&version>=5010301)||	//TMA V5.1.3.1
			(doc_type==2&&version>=2000601)||	//LMA V2.0.6.1
			//(doc_type==3&&version>=2000100)||	//TSA V2.0.1.0
			(doc_type==4&&version>=1030601)||	//LDS V1.3.6.1
			(doc_type==5&&version>=1030601))	//TDA V1.3.6.1
			buffer.ReadBooleanThin(&_bLiveNeedSupplyPart);
		//buffer.ReadString(sPartNo);
		buffer.ReadString(layer_tag,4);
		f3dPoint start,end;
		buffer.ReadDouble(&start.x);
		buffer.ReadDouble(&start.y);
		buffer.ReadDouble(&start.z);
		buffer.ReadDouble(&end.x);	
		buffer.ReadDouble(&end.y);	
		buffer.ReadDouble(&end.z);	
		baseline.SetStart(start);
		baseline.SetEnd(end);
		m_xStartBeforeLift=start;	
		m_xEndBeforeLift  =end;		
		buffer.ReadDouble(&start_oddment);
		buffer.ReadInteger(&desStartOdd.m_iOddCalStyle);
		buffer.ReadInteger(&desStartOdd.m_hRefPart1);
		buffer.ReadInteger(&desStartOdd.m_hRefPart2);
		buffer.ReadDouble(&desStartOdd.m_fCollideDist);

		buffer.ReadDouble(&end_oddment);
		buffer.ReadInteger(&desEndOdd.m_iOddCalStyle);
		buffer.ReadInteger(&desEndOdd.m_hRefPart1);
		buffer.ReadInteger(&desEndOdd.m_hRefPart2);
		buffer.ReadDouble(&desEndOdd.m_fCollideDist);
		//钢管端头设计
		buffer.ReadInteger(&m_tJointStart.type);
		buffer.ReadInteger(&m_tJointStart.hLinkObj);
		buffer.ReadInteger(&m_tJointStart.hViceLinkObj);
		buffer.ReadInteger(&m_tJointStart.bEndPlate);
		buffer.ReadInteger(&m_tJointStart.bVertPlate);
		buffer.ReadInteger(&m_tJointStart.typeEndPlate);
		buffer.ReadDouble(&m_tJointStart.m_fAxisXRotAngle);
		buffer.ReadDouble(&m_tJointStart.m_fWorkPlaneRotAngle);
		buffer.ReadDouble(&m_tJointStart.cutPlanePos.x);
		buffer.ReadDouble(&m_tJointStart.cutPlanePos.y);
		buffer.ReadDouble(&m_tJointStart.cutPlanePos.z);
		buffer.ReadDouble(&m_tJointStart.cutPlaneNorm.x);
		buffer.ReadDouble(&m_tJointStart.cutPlaneNorm.y);
		buffer.ReadDouble(&m_tJointStart.cutPlaneNorm.z);

		buffer.ReadInteger(&m_tJointEnd.type);
		buffer.ReadInteger(&m_tJointEnd.hLinkObj);
		buffer.ReadInteger(&m_tJointEnd.hViceLinkObj);
		buffer.ReadInteger(&m_tJointEnd.bEndPlate);
		buffer.ReadInteger(&m_tJointEnd.bVertPlate);
		buffer.ReadInteger(&m_tJointEnd.typeEndPlate);
		buffer.ReadDouble(&m_tJointEnd.m_fAxisXRotAngle);
		buffer.ReadDouble(&m_tJointEnd.m_fWorkPlaneRotAngle);
		buffer.ReadDouble(&m_tJointEnd.cutPlanePos.x);
		buffer.ReadDouble(&m_tJointEnd.cutPlanePos.y);
		buffer.ReadDouble(&m_tJointEnd.cutPlanePos.z);
		buffer.ReadDouble(&m_tJointEnd.cutPlaneNorm.x);
		buffer.ReadDouble(&m_tJointEnd.cutPlaneNorm.y);
		buffer.ReadDouble(&m_tJointEnd.cutPlaneNorm.z);
		
		//buffer.ReadInteger(&desStartPos.endPosType);
		//buffer.ReadByte(&desStartPos.datum_pos_style);
		//buffer.ReadInteger(&desStartPos.hDatumPart);
		//buffer.ReadInteger(&desStartPos.hDatumStartPart);
		//buffer.ReadInteger(&desStartPos.hDatumEndPart);
		//buffer.ReadInteger(&desStartPos.bDatumEndStartEnd);
		//buffer.ReadInteger(&desStartPos.bDatumStartStartEnd);
		//buffer.ReadDouble(&desStartPos.fEccentricDist);
		//buffer.ReadDouble(&desStartPos.fLenVecOffsetDist);
		//buffer.ReadDouble(&desStartPos.fNormOffset);
		//
		//buffer.ReadInteger(&desEndPos.endPosType);
		//buffer.ReadByte(&desEndPos.datum_pos_style);
		//buffer.ReadInteger(&desEndPos.hDatumPart);
		//buffer.ReadInteger(&desEndPos.hDatumStartPart);
		//buffer.ReadInteger(&desEndPos.hDatumEndPart);
		//buffer.ReadInteger(&desEndPos.bDatumEndStartEnd);
		//buffer.ReadInteger(&desEndPos.bDatumStartStartEnd);
		//buffer.ReadDouble(&desEndPos.fEccentricDist);
		//buffer.ReadDouble(&desEndPos.fLenVecOffsetDist);
		//buffer.ReadDouble(&desEndPos.fNormOffset);
		buffer.ReadInteger(&m_bLockStartPos);
		buffer.ReadInteger(&m_bLockEndPos);
		buffer.ReadString(sNotes,51);
		buffer.ReadDouble(&nearWeldRoadVec.x);
		buffer.ReadDouble(&nearWeldRoadVec.y);
		buffer.ReadDouble(&nearWeldRoadVec.z);
#ifdef __PART_DESIGN_INC_
		if(doc_type==3||doc_type==4)
		{
			buffer.ReadInteger(&m_bAutoCalHoleOutCoef);	//自动计算螺栓减孔数，否则为指定螺栓减孔数
			buffer.ReadDouble(&hole_out);			//杆件上最多螺栓减孔数
			buffer.ReadInteger(&CalLenCoef.iTypeNo);
			buffer.ReadInteger(&CalLenCoef.hHorizPole);
			buffer.ReadInteger(&CalLenCoef.hRefPole);
			buffer.ReadInteger(&CalLenCoef.minR.hStartNode);
			buffer.ReadInteger(&CalLenCoef.minR.hEndNode);
			buffer.ReadDouble(&CalLenCoef.minR.coef);
			buffer.ReadWord(&m_iElemType);			
			buffer.ReadInteger(&m_bNeedSelSize);	//需要选择规格
			buffer.ReadInteger(&m_bNeedSelMat);		//需要选择材质

			buffer.ReadDouble(&connectStart.maxTension);
			buffer.ReadDouble(&connectStart.maxCompression);
			buffer.ReadDouble(&connectStart.maxMoment);
			buffer.ReadDouble(&connectEnd.maxTension);
			buffer.ReadDouble(&connectEnd.maxCompression);
			buffer.ReadDouble(&connectEnd.maxMoment);

			buffer.ReadInteger(&connectStart.d);   
			buffer.Read(connectStart.grade,4);   	
			buffer.Read(&connectStart.uiLegacyN,4); 
			int iConnType=0;
			buffer.ReadInteger(&iConnType);
			connectStart.m_iConnectType=(BYTE)iConnType;
			buffer.ReadString(connectStart.flNo);
			buffer.ReadInteger(&connectEnd.d);   	
			buffer.Read(connectEnd.grade,4);   	
			buffer.Read(&connectEnd.uiLegacyN,4);   	
			buffer.ReadInteger(&iConnType);
			connectEnd.m_iConnectType=(BYTE)iConnType;
			buffer.ReadString(connectEnd.flNo);
		}
#endif
		int n=0;
		buffer.ReadInteger(&n);
		for(int j=0;j<n;j++)
		{
			RELATIVE_OBJECT *pMirObj=relativeObjs.Add(buffer.ReadInteger());
			buffer.ReadByte(&pMirObj->mirInfo.axis_flag);
			buffer.ReadPoint(pMirObj->mirInfo.origin);
			if((pMirObj->mirInfo.axis_flag&0x08)>0)
			{
				buffer.ReadWord(&pMirObj->mirInfo.rotate_angle);
				buffer.ReadByte(&pMirObj->mirInfo.array_num);
			}
			if(pMirObj->mirInfo.axis_flag&0x10)	//镜像
				buffer.ReadPoint(pMirObj->mirInfo.mir_norm);
		}
	}
}
void CLDSLineTube::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(pStart)
		buffer.WriteInteger(pStart->handle);	// 起点句柄
	else
		buffer.WriteInteger(0);
	if(pEnd)
		buffer.WriteInteger(pEnd->handle);
	else
		buffer.WriteInteger(0);
	pLsRefList->ToBuffer(buffer,version,doc_type);

	buffer.WriteDword(dwPermission);
	buffer.WriteDword(dwStartPermission);
	buffer.WriteDword(dwEndPermission);
	buffer.Write(cfgword.flag.word,24);
	buffer.WriteInteger(m_uDisplaySlices);
	buffer.WriteInteger(m_uSlices);
	buffer.WriteInteger(m_uStatMatCoef);
	buffer.WriteInteger(m_hPartWeldParent);
	buffer.WriteWord(m_iNormCalStyle);	//1000083
	buffer.WriteDouble(pickWorkPlane[0]);
	buffer.WriteDouble(pickWorkPlane[1]);
	buffer.WriteDouble(pickWorkPlane[2]);
	if( version==0||
		(doc_type==1&&version>=5010001)||	//TMA(V5,1,0,1)
		(doc_type==2&&version>=2000500)||	//LMA(V2,0,5,0)
		(doc_type==4&&version>=1030500)||	//LDS(V1,3,5,0)
		(doc_type==5&&version>=1030500))	//TDA(V1,3,5,0)
		buffer.WriteInteger(hTransTube);	//一直以来漏记这个属性　wjh-2016.4.12
	buffer.WriteInteger(hWorkNormRefPart);
	buffer.WriteDouble(workPlaneNorm.x);	// 主工作平面法线方向.
	buffer.WriteDouble(workPlaneNorm.y);	// 主工作平面法线方向.
	buffer.WriteDouble(workPlaneNorm.z);	// 主工作平面法线方向.
	buffer.WriteDouble(cutPlaneOrigin.x);
	buffer.WriteDouble(cutPlaneOrigin.y);
	buffer.WriteDouble(cutPlaneOrigin.z);
	buffer.WriteDouble(cutPlaneNorm.x);
	buffer.WriteDouble(cutPlaneNorm.y);
	buffer.WriteDouble(cutPlaneNorm.z);
	if(	version==0||
		(doc_type==1&&version>=5010301)||	//TMA V5.1.3.1
		(doc_type==2&&version>=2000601)||	//LMA V2.0.6.1
		//(doc_type==3&&version>=2000100)||	//TSA V2.0.1.0
		(doc_type==4&&version>=1030601)||	//LDS V1.3.6.1
		(doc_type==5&&version>=1030601))	//TDA V1.3.6.1
		buffer.WriteBooleanThin(_bLiveNeedSupplyPart);
	//buffer.WriteString(sPartNo);			//原始构件编号
	buffer.WriteString(layer());			// 本钢管所在图层.
	buffer.WriteDouble(baseline.Start().x);
	buffer.WriteDouble(baseline.Start().y);
	buffer.WriteDouble(baseline.Start().z);
	buffer.WriteDouble(baseline.End().x);
	buffer.WriteDouble(baseline.End().y);	
	buffer.WriteDouble(baseline.End().z);	
	//始端正头计算参数
	buffer.WriteDouble(start_oddment);
	buffer.WriteInteger(desStartOdd.m_iOddCalStyle);
	buffer.WriteInteger(desStartOdd.m_hRefPart1);
	buffer.WriteInteger(desStartOdd.m_hRefPart2);
	buffer.WriteDouble(desStartOdd.m_fCollideDist);
	//终端正头计算参数
	buffer.WriteDouble(end_oddment);
	buffer.WriteInteger(desEndOdd.m_iOddCalStyle);
	buffer.WriteInteger(desEndOdd.m_hRefPart1);
	buffer.WriteInteger(desEndOdd.m_hRefPart2);
	buffer.WriteDouble(desEndOdd.m_fCollideDist);
	//
	buffer.WriteInteger(m_tJointStart.type);
	buffer.WriteInteger(m_tJointStart.hLinkObj);
	buffer.WriteInteger(m_tJointStart.hViceLinkObj);
	buffer.WriteInteger(m_tJointStart.bEndPlate);
	buffer.WriteInteger(m_tJointStart.bVertPlate);
	buffer.WriteInteger(m_tJointStart.typeEndPlate);
	buffer.WriteDouble(m_tJointStart.m_fAxisXRotAngle);
	buffer.WriteDouble(m_tJointStart.m_fWorkPlaneRotAngle);
	buffer.WriteDouble(m_tJointStart.cutPlanePos.x);
	buffer.WriteDouble(m_tJointStart.cutPlanePos.y);
	buffer.WriteDouble(m_tJointStart.cutPlanePos.z);
	buffer.WriteDouble(m_tJointStart.cutPlaneNorm.x);
	buffer.WriteDouble(m_tJointStart.cutPlaneNorm.y);
	buffer.WriteDouble(m_tJointStart.cutPlaneNorm.z);
	//
	buffer.WriteInteger(m_tJointEnd.type);
	buffer.WriteInteger(m_tJointEnd.hLinkObj);
	buffer.WriteInteger(m_tJointEnd.hViceLinkObj);
	buffer.WriteInteger(m_tJointEnd.bEndPlate);
	buffer.WriteInteger(m_tJointEnd.bVertPlate);
	buffer.WriteInteger(m_tJointEnd.typeEndPlate);
	buffer.WriteDouble(m_tJointEnd.m_fAxisXRotAngle);
	buffer.WriteDouble(m_tJointEnd.m_fWorkPlaneRotAngle);
	buffer.WriteDouble(m_tJointEnd.cutPlanePos.x);
	buffer.WriteDouble(m_tJointEnd.cutPlanePos.y);
	buffer.WriteDouble(m_tJointEnd.cutPlanePos.z);
	buffer.WriteDouble(m_tJointEnd.cutPlaneNorm.x);
	buffer.WriteDouble(m_tJointEnd.cutPlaneNorm.y);
	buffer.WriteDouble(m_tJointEnd.cutPlaneNorm.z);
	//
	//buffer.WriteInteger(desStartPos.endPosType);
	//buffer.WriteByte(desStartPos.datum_pos_style);
	//buffer.WriteInteger(desStartPos.hDatumPart);
	//buffer.WriteInteger(desStartPos.hDatumStartPart);
	//buffer.WriteInteger(desStartPos.hDatumEndPart);
	//buffer.WriteInteger(desStartPos.bDatumEndStartEnd);
	//buffer.WriteInteger(desStartPos.bDatumStartStartEnd);
	//if(desStartPos.face_offset_norm.IsZero())
	//	desStartPos.bUseFaceOffsetNorm=false;
	//else
	//	desStartPos.bUseFaceOffsetNorm=true;
	
	//buffer.WriteDouble(desStartPos.fEccentricDist);
	//buffer.WriteDouble(desStartPos.fLenVecOffsetDist); //1.0.1.3
	//buffer.WriteDouble(desStartPos.fNormOffset);
	//buffer.WriteInteger(desEndPos.endPosType);
	//buffer.WriteByte(desEndPos.datum_pos_style);
	//buffer.WriteInteger(desEndPos.hDatumPart);
	//buffer.WriteInteger(desEndPos.hDatumStartPart);
	//buffer.WriteInteger(desEndPos.hDatumEndPart);
	//buffer.WriteInteger(desEndPos.bDatumEndStartEnd);
	//buffer.WriteInteger(desEndPos.bDatumStartStartEnd);
	//if(desEndPos.face_offset_norm.IsZero())
	//	desEndPos.bUseFaceOffsetNorm=false;
	//else
	//	desEndPos.bUseFaceOffsetNorm=true;
	//buffer.WriteDouble(desEndPos.fEccentricDist);
	//buffer.WriteDouble(desEndPos.fLenVecOffsetDist); //1.0.1.3
	//buffer.WriteDouble(desEndPos.fNormOffset);
	buffer.WriteInteger(m_bLockStartPos);
	buffer.WriteInteger(m_bLockEndPos);
	buffer.WriteString(sNotes);
	buffer.WriteDouble(nearWeldRoadVec.x);
	buffer.WriteDouble(nearWeldRoadVec.y);
	buffer.WriteDouble(nearWeldRoadVec.z);
#ifdef __PART_DESIGN_INC_
	buffer.WriteInteger(m_bAutoCalHoleOutCoef);	//自动计算螺栓减孔数，否则为指定螺栓减孔数
	buffer.WriteDouble(hole_out);			//杆件上最多螺栓减孔数
	buffer.WriteInteger(CalLenCoef.iTypeNo);
	buffer.WriteInteger(CalLenCoef.hHorizPole);
	buffer.WriteInteger(CalLenCoef.hRefPole);
	buffer.WriteInteger(CalLenCoef.minR.hStartNode);
	buffer.WriteInteger(CalLenCoef.minR.hEndNode);
	buffer.WriteDouble(CalLenCoef.minR.coef);
	buffer.WriteWord(m_iElemType);			//0表示自动判断;1表示二力杆桁架;2表示经典梁单元;3表示柔索单元
	buffer.WriteInteger(m_bNeedSelSize);	//需要选择规格
	buffer.WriteInteger(m_bNeedSelMat);		//需要选择材质
	
	buffer.WriteDouble(connectStart.maxTension);
	buffer.WriteDouble(connectStart.maxCompression);
	buffer.WriteDouble(connectStart.maxMoment);
	buffer.WriteDouble(connectEnd.maxTension);
	buffer.WriteDouble(connectEnd.maxCompression);
	buffer.WriteDouble(connectEnd.maxMoment);
		
	buffer.WriteInteger(connectStart.d);   
	buffer.Write(connectStart.grade,4);   	
	buffer.Write(&connectStart.uiLegacyN,4);   	
	buffer.WriteInteger(connectStart.m_iConnectType);
	buffer.WriteString(connectStart.flNo);
	buffer.WriteInteger(connectEnd.d);   	
	buffer.Write(connectEnd.grade,4);   	
	buffer.Write(&connectEnd.uiLegacyN,4);   	
	buffer.WriteInteger(connectEnd.m_iConnectType);
	buffer.WriteString(connectEnd.flNo);
#endif
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for(RELATIVE_OBJECT *pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pMirObj->hObj);
		buffer.WriteByte(pMirObj->mirInfo.axis_flag);
		buffer.WritePoint(pMirObj->mirInfo.origin);
		if((pMirObj->mirInfo.axis_flag&8)>0)
		{
			buffer.WriteWord(pMirObj->mirInfo.rotate_angle);
			buffer.WriteByte(pMirObj->mirInfo.array_num);
		}
		if(pMirObj->mirInfo.axis_flag&0x10)	//镜像
			buffer.WritePoint(pMirObj->mirInfo.mir_norm);
	}
}
void CLDSLineTube::FromBuffer(CBuffer &buffer, long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropFromBuffer(buffer,version,doc_type);		
		return;
	}
	DWORD dw;
	bool bPrevDeflateVersion=false;	//文件瘦身之前的版本
	if( (doc_type==1&&version>0&&version<4000027)||	//TMA
		(doc_type==2&&version>0&&version<1000082)||	//LMA
		(doc_type==4&&version>0&&version<1000009))	//LDS
		bPrevDeflateVersion=true;
	buffer.ReadDword(&dwPermission);	
	buffer.ReadDword(&dwStartPermission);
	buffer.ReadDword(&dwEndPermission);
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	BelongModel()->EvolveCfgword(cfgword,version,doc_type);
	if( version==0||
	   (version>=5000003&&doc_type==1)||(version>=2000003&&doc_type==2)||
	   (version>=1030003&&doc_type==4)||(version>=1030003&&doc_type==5))
	   mpcl.FromBuffer(&buffer,version,doc_type);
	if(doc_type==4&&(version==0||version>=1000204))	//LDS
	{
		buffer.ReadInteger(&size.idClassType);
		buffer.ReadByte(&size.cSubClassType);
	}
	buffer.ReadDouble(&size.wide);
	buffer.ReadDouble(&size.thick);
	buffer.ReadByte(&_material);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.ReadByte(&_cQualityLevel);
	buffer.ReadInteger(&m_uDisplaySlices);
	buffer.ReadInteger(&m_uSlices);
	buffer.ReadInteger(&m_uStatMatCoef);
	buffer.ReadInteger(&_hPartWeldParent);
	if( version==0||
		(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
		buffer.ReadByte(&_cFuncType);
	if((doc_type==1&&(version==0||version>=4000021))	//TMA
		||(doc_type==2&&(version==0||version>=1000076))//LMA
		||doc_type==4||doc_type==5)
	{
		if(bPrevDeflateVersion)
		{
			buffer.ReadInteger(&dw);
			m_bUserSpecColor=(dw!=0);
		}
		else
			buffer.ReadBooleanThin(&m_bUserSpecColor);
		if(m_bUserSpecColor)
			buffer.ReadDword(&crMaterial);
	}
	long h;
	buffer.ReadInteger(&h);
	//if(SuperiorTower()->hashObjs.GetHashTableSize()>0)
	//	SuperiorTower()->hashObjs.GetValueAt(h,(CLDSObject*&)pStart);
	//else
		pStart=BelongModel()->FromNodeHandle(h);	// 起点句柄
	buffer.ReadInteger(&h);
	//if(SuperiorTower()->hashObjs.GetHashTableSize()>0)
	//	SuperiorTower()->hashObjs.GetValueAt(h,(CLDSObject*&)pEnd);
	//else
		pEnd  =BelongModel()->FromNodeHandle(h);	// 终点句柄

	if(doc_type==5	//TDA
		||(doc_type==1&&(version==0||version>=4000028))	//TMA
		||(doc_type==2&&(version==0||version>=1000083))	//LMA
		||(doc_type==4&&(version==0||version>=1000010)))//LDS
		buffer.ReadWord(&m_iNormCalStyle);
	buffer.ReadDouble(&pickWorkPlane[0]);
	buffer.ReadDouble(&pickWorkPlane[1]);
	buffer.ReadDouble(&pickWorkPlane[2]);
	if( version==0||
		(doc_type==1&&version>=5010001)||	//TMA(V5,1,0,1)
		(doc_type==2&&version>=2000500)||	//LMA(V2,0,5,0)
		(doc_type==4&&version>=1030500)||	//LDS(V1,3,5,0)
		(doc_type==5&&version>=1030500))	//TDA(V1,3,5,0)
		buffer.ReadInteger(&hTransTube);	//一直以来漏记这个属性　wjh-2016.4.12
	buffer.ReadInteger(&hWorkNormRefPart);
	buffer.ReadDouble(&workPlaneNorm.x);	// 主工作平面法线方向.
	buffer.ReadDouble(&workPlaneNorm.y);	// 主工作平面法线方向.
	buffer.ReadDouble(&workPlaneNorm.z);	// 主工作平面法线方向.
	buffer.ReadDouble(&cutPlaneOrigin.x);
	buffer.ReadDouble(&cutPlaneOrigin.y);
	buffer.ReadDouble(&cutPlaneOrigin.z);
	buffer.ReadDouble(&cutPlaneNorm.x);
	buffer.ReadDouble(&cutPlaneNorm.y);
	buffer.ReadDouble(&cutPlaneNorm.z);
	if(	version==0||
		(doc_type==1&&version>=5010301)||	//TMA V5.1.3.1
		(doc_type==2&&version>=2000601)||	//LMA V2.0.6.1
		//(doc_type==3&&version>=2000100)||	//TSA V2.0.1.0
		(doc_type==4&&version>=1030601)||	//LDS V1.3.6.1
		(doc_type==5&&version>=1030601))	//TDA V1.3.6.1
		buffer.ReadBooleanThin(&_bLiveNeedSupplyPart);
	if(bPrevDeflateVersion)
	{
		buffer.ReadInteger(&dw);
		m_bVirtualPart=(dw!=0);	//	隐含钢管
	}
	else
		buffer.ReadBooleanThin(&_bVirtualPart);
	buffer.ReadInteger(&_iSeg.iSeg);
	buffer.ReadString(sPartNo);				//原始构件编号
	buffer.ReadString(layer(),4);				// 本钢管所在图层.
	f3dPoint start,end;
	buffer.ReadDouble(&start.x);// 实际楞线起点
	buffer.ReadDouble(&start.y);// 实际楞线起点
	buffer.ReadDouble(&start.z);// 实际楞线起点
	buffer.ReadDouble(&end.x);	// 实际楞线终点
	buffer.ReadDouble(&end.y);	// 实际楞线终点
	buffer.ReadDouble(&end.z);	// 实际楞线终点
	baseline.SetStart(start);
	baseline.SetEnd(end);
	m_xStartBeforeLift=start;	//赋默认值
	m_xEndBeforeLift  =end;		//赋默认值
#ifdef __COMMON_PART_INC_		//含钢管构件产品才支持自动圆弧预拱LMA/LDS/TDA V1.3.4.0
	if( version==0||
		(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
	{
		buffer.ReadInteger(&m_hArcLift);
		buffer.ReadPoint(m_xStartBeforeLift);
		buffer.ReadPoint(m_xEndBeforeLift);
	}
#endif
	buffer.ReadDouble(&start_oddment);
	//始端正头计算参数
	buffer.ReadInteger(&desStartOdd.m_iOddCalStyle);
	buffer.ReadInteger(&desStartOdd.m_hRefPart1);
	buffer.ReadInteger(&desStartOdd.m_hRefPart2);
	buffer.ReadDouble(&desStartOdd.m_fCollideDist);
	//终端正头计算参数
	buffer.ReadDouble(&end_oddment);   // 角钢始终端正负头.
	buffer.ReadInteger(&desEndOdd.m_iOddCalStyle);
	buffer.ReadInteger(&desEndOdd.m_hRefPart1);
	buffer.ReadInteger(&desEndOdd.m_hRefPart2);
	buffer.ReadDouble(&desEndOdd.m_fCollideDist);
	buffer.ReadInteger(&m_tJointStart.type);
	buffer.ReadInteger(&m_tJointStart.hLinkObj);
	buffer.ReadInteger(&m_tJointStart.hViceLinkObj);
	buffer.ReadInteger(&m_tJointStart.bEndPlate);
	buffer.ReadInteger(&m_tJointStart.bVertPlate);
	buffer.ReadInteger(&m_tJointStart.typeEndPlate);
	buffer.ReadDouble(&m_tJointStart.m_fAxisXRotAngle);
	buffer.ReadDouble(&m_tJointStart.m_fWorkPlaneRotAngle);
	buffer.ReadDouble(&m_tJointStart.cutPlanePos.x);
	buffer.ReadDouble(&m_tJointStart.cutPlanePos.y);
	buffer.ReadDouble(&m_tJointStart.cutPlanePos.z);
	buffer.ReadDouble(&m_tJointStart.cutPlaneNorm.x);
	buffer.ReadDouble(&m_tJointStart.cutPlaneNorm.y);
	buffer.ReadDouble(&m_tJointStart.cutPlaneNorm.z);
	buffer.ReadInteger(&m_tJointEnd.type);
	buffer.ReadInteger(&m_tJointEnd.hLinkObj);
	if(m_tJointStart.hLinkObj<0)
		m_tJointStart.hLinkObj = 0;
	if(m_tJointEnd.hLinkObj<0)
		m_tJointEnd.hLinkObj=0;
	buffer.ReadInteger(&m_tJointEnd.hViceLinkObj);
	buffer.ReadInteger(&m_tJointEnd.bEndPlate);
	buffer.ReadInteger(&m_tJointEnd.bVertPlate);
	buffer.ReadInteger(&m_tJointEnd.typeEndPlate);
	buffer.ReadDouble(&m_tJointEnd.m_fAxisXRotAngle);
	buffer.ReadDouble(&m_tJointEnd.m_fWorkPlaneRotAngle);
	buffer.ReadDouble(&m_tJointEnd.cutPlanePos.x);
	buffer.ReadDouble(&m_tJointEnd.cutPlanePos.y);
	buffer.ReadDouble(&m_tJointEnd.cutPlanePos.z);
	buffer.ReadDouble(&m_tJointEnd.cutPlaneNorm.x);
	buffer.ReadDouble(&m_tJointEnd.cutPlaneNorm.y);
	buffer.ReadDouble(&m_tJointEnd.cutPlaneNorm.z);
	//原始终端定位信息读取代码已移至 CTubeEndPosPara::FromBuffer中 wjh-2015.2.7
	desStartPos.FromBuffer(buffer,version,doc_type);
	desEndPos.FromBuffer(buffer,version,doc_type);
	buffer.ReadInteger(&feature);		//备用(可用于为本角钢标志用户临时自定义的属性)
	if(bPrevDeflateVersion)
	{
		buffer.ReadInteger(&dw);//显示螺栓孔及切割面
		m_bDisplayHole=(dw!=0);
	}
	else
		buffer.ReadBooleanThin(&m_bDisplayHole);

	//设计信息
	buffer.ReadInteger(&m_bLockStartPos);
	buffer.ReadInteger(&m_bLockEndPos);
	buffer.ReadString(sNotes,51);
	buffer.ReadInteger(&_bHasWeldRoad);
	buffer.ReadDouble(&nearWeldRoadVec.x);
	buffer.ReadDouble(&nearWeldRoadVec.y);
	buffer.ReadDouble(&nearWeldRoadVec.z);
#ifdef __LDS_FILE_
	if(doc_type==4&&(version==0||version>=1000012))
	{
		buffer.ReadInteger(&m_bAutoCalHoleOutCoef);	//自动计算螺栓减孔数，否则为指定螺栓减孔数
		buffer.ReadDouble(&hole_out);			//杆件上最多螺栓减孔数
		int force_type=0,joint_type=0;
		buffer.ReadInteger(&force_type);
		if(force_type==CENTRIC_FORCE)
			start_force_type=CENTRIC_FORCE;
		else //if(force_type==ECCENTRIC_FORCE)
			start_force_type=ECCENTRIC_FORCE;
		buffer.ReadInteger(&force_type);
		if(force_type==CENTRIC_FORCE)
			end_force_type=CENTRIC_FORCE;
		else //if(force_type==ECCENTRIC_FORCE)
			end_force_type=ECCENTRIC_FORCE;
		buffer.ReadInteger(&joint_type);
		if(joint_type==JOINT_HINGE)			//0：完全铰接JOINT_HINGE
			start_joint_type=JOINT_HINGE;
		else if(joint_type==JOINT_RIGID)	//1：部分刚接JOINT_RIGID（限制除扭转外的转动约束）
			start_joint_type=JOINT_RIGID;
		else								//2：自动判断JOINT_AUTOJUSTIFY
			start_joint_type=JOINT_AUTOJUSTIFY;
		buffer.ReadInteger(&joint_type);
		if(joint_type==JOINT_HINGE)
			end_joint_type=JOINT_HINGE;
		else if(joint_type==JOINT_RIGID)
			end_joint_type=JOINT_RIGID;
		else
			end_joint_type=JOINT_AUTOJUSTIFY;
		buffer.ReadInteger(&CalLenCoef.iTypeNo);
		buffer.ReadInteger(&CalLenCoef.hHorizPole);
		buffer.ReadInteger(&CalLenCoef.hRefPole);
		if(version==0||(doc_type==3&&version>=1070101)||(doc_type==4&&version>=1000101))
		{
			buffer.ReadInteger(&CalLenCoef.minR.hStartNode);
			buffer.ReadInteger(&CalLenCoef.minR.hEndNode);
			buffer.ReadDouble(&CalLenCoef.minR.coef);
		}
		else
		{
			CalLenCoef.minR.hStartNode=CalLenCoef.minR.hEndNode=CalLenCoef.paraR.hStartNode=CalLenCoef.paraR.hEndNode=0;
			buffer.ReadDouble(&CalLenCoef.minR.coef);
		}
		buffer.ReadWord(&m_iElemType);			//0表示自动判断;1表示二力杆桁架;2表示经典梁单元;3表示柔索单元
		buffer.ReadDword(&_uStatMatNo);   //统材组
		buffer.ReadInteger(&m_bNeedSelSize);	//需要选择规格
		buffer.ReadInteger(&m_bNeedSelMat);		//需要选择材质
		if(version>=1000003&&version<1000203)
		{
			BOOL bAnalysisInc;
			buffer.ReadInteger(&bAnalysisInc);
		}
		buffer.ReadInteger(&force_type);		//塔材选材方式
		sel_mat_method=AUX_POLE_SEL_METHOD(force_type);
		buffer.ReadDouble(&bear_load_coef);		//辅助材支撑所支撑杆件内力的
		buffer.ReadDouble(&m_fWeightRaiseCoef);	//重力放大系数，考虑联板、螺栓等对重力的放大作用
		if(	version==0|| (doc_type==4&&version>=1030800))	//LDS V1.3.8.0
			buffer.ReadByte(&m_cbRodFlag);
//#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
		if(version==0||(doc_type==3&&version>=1070306)||(doc_type==4&&version>=1000306))
		{
			buffer.ReadDouble(&connectStart.maxTension);
			buffer.ReadDouble(&connectStart.maxCompression);
			buffer.ReadDouble(&connectStart.maxMoment);
			buffer.ReadDouble(&connectEnd.maxTension);
			buffer.ReadDouble(&connectEnd.maxCompression);
			buffer.ReadDouble(&connectEnd.maxMoment);
		}
//#endif
		// --------- 3. 其它数据: 螺栓信息和杆件类型 -----------
		if(version==0||(doc_type==3&&version>=1070201)||(doc_type==4&&version>=1000201))
		{
			buffer.ReadInteger(&connectStart.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
			buffer.Read(connectStart.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
			buffer.Read(&connectStart.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
			int iConnType=0;
			buffer.ReadInteger(&iConnType);
			connectStart.m_iConnectType=iConnType;
			buffer.ReadString(connectStart.flNo);
			buffer.ReadInteger(&connectEnd.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
			buffer.Read(connectEnd.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
			buffer.Read(&connectEnd.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
			buffer.ReadInteger(&iConnType);
			connectEnd.m_iConnectType=iConnType;
			buffer.ReadString(connectEnd.flNo);
		}
	}
#endif
	//关联构件
	int i,nn=0;
	buffer.ReadInteger(&nn);
	RELATIVE_OBJECT *pRelaObj=NULL;
	relativeObjs.Empty();
	for(i=0;i<nn;i++)
	{
		pRelaObj=relativeObjs.Add(buffer.ReadInteger());
		if(bPrevDeflateVersion)
		{
			buffer.ReadDword(&dw);
			pRelaObj->mirInfo.axis_flag=(BYTE)dw;
			buffer.ReadInteger(&dw);
			pRelaObj->mirInfo.array_num=(BYTE)dw;
			buffer.ReadPoint(pRelaObj->mirInfo.origin);
			double ff;
			buffer.ReadDouble(&ff);
			pRelaObj->mirInfo.rotate_angle=(short)ff;
		}
		else
		{
			buffer.ReadByte(&pRelaObj->mirInfo.axis_flag);
			bool bSpecOrigin;
			buffer.ReadBooleanThin(&bSpecOrigin);
			if(bSpecOrigin)
				buffer.ReadPoint(pRelaObj->mirInfo.origin);
			if(pRelaObj->mirInfo.axis_flag&0x08)
			{
				buffer.ReadByte(&pRelaObj->mirInfo.array_num);
				buffer.ReadWord(&pRelaObj->mirInfo.rotate_angle);
			}
		}
	}
	//读取钢管螺栓引用 wht 12-03-19
	if(version==0 
		||(doc_type==1&&version>=4000112)	//TMA
		||(doc_type==2&&version>=1000112)	//LMA
		||(doc_type==3&&version>=1070312)	//TSA
		||(doc_type==4&&version>=1000312)	//LDS
		||(doc_type==5&&version>=1000002))	//TDA
		pLsRefList->FromBuffer(buffer,m_pModel,version,doc_type);
	SetModified();
}

void CLDSLineTube::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropToBuffer(buffer,version,doc_type);
		return;
	}
	buffer.WriteDword(dwPermission);
	buffer.WriteDword(dwStartPermission);
	buffer.WriteDword(dwEndPermission);
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	if( version==0||
	   (version>=5000003&&doc_type==1)||(version>=2000003&&doc_type==2)||
	   (version>=1030003&&doc_type==4)||(version>=1030003&&doc_type==5))
	   mpcl.ToBuffer(&buffer,version,doc_type);
#ifdef __LDS_FILE_
	if(doc_type==3||doc_type==4)
	{
	buffer.WriteInteger(size_idClassType);
	buffer.WriteByte(size_cSubClassType);
	}
#endif
	buffer.WriteDouble(size_wide);
	buffer.WriteDouble(size_thick);
	buffer.WriteByte(cMaterial);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.WriteByte(m_cQualityLevel);
	buffer.WriteInteger(m_uDisplaySlices);
	buffer.WriteInteger(m_uSlices);
	buffer.WriteInteger(m_uStatMatCoef);
	buffer.WriteInteger(m_hPartWeldParent);
	if( version==0||
		(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
		buffer.WriteByte(m_cFuncType);
	buffer.WriteBooleanThin(m_bUserSpecColor);
	if(m_bUserSpecColor)
		buffer.WriteDword(crMaterial);
	if(pStart)
		buffer.WriteInteger(pStart->handle);	// 起点句柄
	else
		buffer.WriteInteger(0);
	if(pEnd)
		buffer.WriteInteger(pEnd->handle);
	else
		buffer.WriteInteger(0);

	buffer.WriteWord(m_iNormCalStyle);	//1000083
	buffer.WriteDouble(pickWorkPlane[0]);
	buffer.WriteDouble(pickWorkPlane[1]);
	buffer.WriteDouble(pickWorkPlane[2]);
	if( version==0||
		(doc_type==1&&version>=5010001)||	//TMA(V5,1,0,1)
		(doc_type==2&&version>=2000500)||	//LMA(V2,0,5,0)
		(doc_type==4&&version>=1030500)||	//LDS(V1,3,5,0)
		(doc_type==5&&version>=1030500))	//TDA(V1,3,5,0)
		buffer.WriteInteger(hTransTube);	//一直以来漏记这个属性　wjh-2016.4.12
	buffer.WriteInteger(hWorkNormRefPart);
	buffer.WriteDouble(workPlaneNorm.x);	// 主工作平面法线方向.
	buffer.WriteDouble(workPlaneNorm.y);	// 主工作平面法线方向.
	buffer.WriteDouble(workPlaneNorm.z);	// 主工作平面法线方向.
	buffer.WriteDouble(cutPlaneOrigin.x);
	buffer.WriteDouble(cutPlaneOrigin.y);
	buffer.WriteDouble(cutPlaneOrigin.z);
	buffer.WriteDouble(cutPlaneNorm.x);
	buffer.WriteDouble(cutPlaneNorm.y);
	buffer.WriteDouble(cutPlaneNorm.z);
	if(	version==0||
		(doc_type==1&&version>=5010301)||	//TMA V5.1.3.1
		(doc_type==2&&version>=2000601)||	//LMA V2.0.6.1
		//(doc_type==3&&version>=2000100)||	//TSA V2.0.1.0
		(doc_type==4&&version>=1030601)||	//LDS V1.3.6.1
		(doc_type==5&&version>=1030601))	//TDA V1.3.6.1
		buffer.WriteBooleanThin(_bLiveNeedSupplyPart);
	buffer.WriteBooleanThin(_bVirtualPart);	//	隐含钢管
	buffer.WriteInteger(iSeg);
	buffer.WriteString(sPartNo);			//原始构件编号
	buffer.WriteString(layer());			// 本钢管所在图层.
	buffer.WriteDouble(baseline.Start().x);// 实际楞线起点
	buffer.WriteDouble(baseline.Start().y);// 实际楞线起点
	buffer.WriteDouble(baseline.Start().z);// 实际楞线起点
	buffer.WriteDouble(baseline.End().x);	// 实际楞线终点
	buffer.WriteDouble(baseline.End().y);	// 实际楞线终点
	buffer.WriteDouble(baseline.End().z);	// 实际楞线终点
#ifdef __COMMON_PART_INC_		//含钢管构件产品才支持自动圆弧预拱LMA/LDS/TDA V1.3.4.0
	if( version==0||
		(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
	{
		buffer.WriteInteger(m_hArcLift);
		buffer.WritePoint(m_xStartBeforeLift);
		buffer.WritePoint(m_xEndBeforeLift);
	}
#endif
	//始端正头计算参数
	buffer.WriteDouble(start_oddment);
	buffer.WriteInteger(desStartOdd.m_iOddCalStyle);
	buffer.WriteInteger(desStartOdd.m_hRefPart1);
	buffer.WriteInteger(desStartOdd.m_hRefPart2);
	buffer.WriteDouble(desStartOdd.m_fCollideDist);
	//终端正头计算参数
	buffer.WriteDouble(end_oddment);   // 角钢始终端正负头.
	buffer.WriteInteger(desEndOdd.m_iOddCalStyle);
	buffer.WriteInteger(desEndOdd.m_hRefPart1);
	buffer.WriteInteger(desEndOdd.m_hRefPart2);
	buffer.WriteDouble(desEndOdd.m_fCollideDist);
	//
	buffer.WriteInteger(m_tJointStart.type);
	buffer.WriteInteger(m_tJointStart.hLinkObj);
	buffer.WriteInteger(m_tJointStart.hViceLinkObj);
	buffer.WriteInteger(m_tJointStart.bEndPlate);
	buffer.WriteInteger(m_tJointStart.bVertPlate);
	buffer.WriteInteger(m_tJointStart.typeEndPlate);
	buffer.WriteDouble(m_tJointStart.m_fAxisXRotAngle);
	buffer.WriteDouble(m_tJointStart.m_fWorkPlaneRotAngle);
	buffer.WriteDouble(m_tJointStart.cutPlanePos.x);
	buffer.WriteDouble(m_tJointStart.cutPlanePos.y);
	buffer.WriteDouble(m_tJointStart.cutPlanePos.z);
	buffer.WriteDouble(m_tJointStart.cutPlaneNorm.x);
	buffer.WriteDouble(m_tJointStart.cutPlaneNorm.y);
	buffer.WriteDouble(m_tJointStart.cutPlaneNorm.z);
	//
	buffer.WriteInteger(m_tJointEnd.type);
	buffer.WriteInteger(m_tJointEnd.hLinkObj);
	buffer.WriteInteger(m_tJointEnd.hViceLinkObj);
	buffer.WriteInteger(m_tJointEnd.bEndPlate);
	buffer.WriteInteger(m_tJointEnd.bVertPlate);
	buffer.WriteInteger(m_tJointEnd.typeEndPlate);
	buffer.WriteDouble(m_tJointEnd.m_fAxisXRotAngle);
	buffer.WriteDouble(m_tJointEnd.m_fWorkPlaneRotAngle);
	buffer.WriteDouble(m_tJointEnd.cutPlanePos.x);
	buffer.WriteDouble(m_tJointEnd.cutPlanePos.y);
	buffer.WriteDouble(m_tJointEnd.cutPlanePos.z);
	buffer.WriteDouble(m_tJointEnd.cutPlaneNorm.x);
	buffer.WriteDouble(m_tJointEnd.cutPlaneNorm.y);
	buffer.WriteDouble(m_tJointEnd.cutPlaneNorm.z);
	//原始终端定位信息写入代码已移至 CTubeEndPosPara::ToBuffer中 wjh-2015.2.7
	desStartPos.ToBuffer(buffer,version,doc_type);
	desEndPos.ToBuffer(buffer,version,doc_type);
	buffer.WriteInteger(feature);		//备用(可用于为本角钢标志用户临时自定义的属性)
	buffer.WriteBooleanThin(m_bDisplayHole);//显示螺栓孔及切割面
	buffer.WriteInteger(m_bLockStartPos);
	buffer.WriteInteger(m_bLockEndPos);
	buffer.WriteString(sNotes);
	buffer.WriteInteger(m_bHasWeldRoad);	//焊道线
	buffer.WriteDouble(nearWeldRoadVec.x);
	buffer.WriteDouble(nearWeldRoadVec.y);
	buffer.WriteDouble(nearWeldRoadVec.z);
#ifdef __LDS_FILE_
	if(doc_type==3||doc_type==4)
	{
	buffer.WriteInteger(m_bAutoCalHoleOutCoef);	//自动计算螺栓减孔数，否则为指定螺栓减孔数
	buffer.WriteDouble(hole_out);			//杆件上最多螺栓减孔数
	buffer.WriteInteger(start_force_type);
	buffer.WriteInteger(end_force_type);
	buffer.WriteInteger(start_joint_type);
	buffer.WriteInteger(end_joint_type);
	buffer.WriteInteger(CalLenCoef.iTypeNo);
	buffer.WriteInteger(CalLenCoef.hHorizPole);
	buffer.WriteInteger(CalLenCoef.hRefPole);
	buffer.WriteInteger(CalLenCoef.minR.hStartNode);
	buffer.WriteInteger(CalLenCoef.minR.hEndNode);
	buffer.WriteDouble(CalLenCoef.minR.coef);
	buffer.WriteWord(m_iElemType);			//0表示自动判断;1表示二力杆桁架;2表示经典梁单元;3表示柔索单元
	buffer.WriteDword(m_uStatMatNo);		//统材组号
	buffer.WriteInteger(m_bNeedSelSize);	//需要选择规格
	buffer.WriteInteger(m_bNeedSelMat);		//需要选择材质
	buffer.WriteInteger(sel_mat_method);	//塔材选材方式
	buffer.WriteDouble(bear_load_coef);		//辅助材支撑所支撑杆件内力的
	buffer.WriteDouble(m_fWeightRaiseCoef);	//重力放大系数，考虑联板、螺栓等对重力的放大作用
	if(	version==0|| (doc_type==4&&version>=1030800))	//LDS V1.3.8.0
		buffer.WriteByte(m_cbRodFlag);
//#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	buffer.WriteDouble(connectStart.maxTension);
	buffer.WriteDouble(connectStart.maxCompression);
	buffer.WriteDouble(connectStart.maxMoment);
	buffer.WriteDouble(connectEnd.maxTension);
	buffer.WriteDouble(connectEnd.maxCompression);
	buffer.WriteDouble(connectEnd.maxMoment);
//#endif
	// --------- 3. 其它数据: 螺栓信息和杆件类型 -----------
	buffer.WriteInteger(connectStart.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Write(connectStart.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Write(&connectStart.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.WriteInteger(connectStart.m_iConnectType);
	buffer.WriteString(connectStart.flNo);
	buffer.WriteInteger(connectEnd.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Write(connectEnd.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Write(&connectEnd.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.WriteInteger(connectEnd.m_iConnectType);
	buffer.WriteString(connectEnd.flNo);
	}
#endif
	//关联构件列表
	RELATIVE_OBJECT *pRelaObj=NULL;
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for(pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pRelaObj->hObj);
		buffer.WriteByte(pRelaObj->mirInfo.axis_flag);
		bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
		buffer.WriteBooleanThin(bSpecOrigin);
		if(bSpecOrigin)
			buffer.WritePoint(pRelaObj->mirInfo.origin);
		if(pRelaObj->mirInfo.axis_flag&0x08)
		{
			buffer.WriteByte(pRelaObj->mirInfo.array_num);
			buffer.WriteWord(pRelaObj->mirInfo.rotate_angle);
		}
	}
	//钢管螺栓区 wht 12-03-19
	pLsRefList->ToBuffer(buffer,version,doc_type);
}
void CLDSLineTube::FromCdfBuffer(CBuffer &buffer,long version/*=0*/,bool bReadForCheck/*=false*/)
{
	CLDSLinePart::FromCdfBuffer(buffer,version,bReadForCheck);
	buffer.ReadInteger(&m_uSlices);
	long hStart=0,hEnd=0;
	buffer.ReadInteger(&hStart);
	buffer.ReadInteger(&hEnd);
	pStart=BelongModel()->FromNodeHandle(hStart);	// 起点句柄
	pEnd=BelongModel()->FromNodeHandle(hEnd);		// 终点句柄

	buffer.ReadWord(&m_iNormCalStyle);	//1000083
	buffer.ReadPoint(pickWorkPlane);
	buffer.ReadInteger(&hWorkNormRefPart);
	buffer.ReadPoint(workPlaneNorm);	// 主工作平面法线方向.
	GEPOINT posStart,posEnd;
	buffer.ReadPoint(posStart);// 实际楞线起点
	buffer.ReadPoint(posEnd);	// 实际楞线终点
	baseline.SetStart(posStart);
	baseline.SetEnd(posEnd);
	//原始终端定位信息写入代码已移至 CTubeEndPosPara::ToBuffer中 wjh-2015.2.7
	desStartPos.FromBuffer(buffer);//,version,doc_type);
	desEndPos.FromBuffer(buffer);//,version,doc_type);
	//buffer.WriteInteger(m_bHasWeldRoad);	//焊道线
	//buffer.WriteDouble(nearWeldRoadVec.x);
	//buffer.WriteDouble(nearWeldRoadVec.y);
	//buffer.WriteDouble(nearWeldRoadVec.z);
}
void CLDSLineTube::ToCdfBuffer(CBuffer &buffer,long version/*=0*/)
{
	CLDSLinePart::ToCdfBuffer(buffer,version);
	buffer.WriteInteger(m_uSlices);
	if(pStart)
		buffer.WriteInteger(pStart->handle);	// 起点句柄
	else
		buffer.WriteInteger(0);
	if(pEnd)
		buffer.WriteInteger(pEnd->handle);
	else
		buffer.WriteInteger(0);

	buffer.WriteWord(m_iNormCalStyle);	//1000083
	buffer.WritePoint(pickWorkPlane);
	buffer.WriteInteger(hWorkNormRefPart);
	buffer.WritePoint(workPlaneNorm);	// 主工作平面法线方向.
	buffer.WritePoint(baseline.Start());// 实际楞线起点
	buffer.WritePoint(baseline.End());	// 实际楞线终点
	//原始终端定位信息写入代码已移至 CTubeEndPosPara::ToBuffer中 wjh-2015.2.7
	desStartPos.ToBuffer(buffer);//,version,doc_type);
	desEndPos.ToBuffer(buffer);//,version,doc_type);
	//buffer.WriteInteger(m_bHasWeldRoad);	//焊道线
	//buffer.WriteDouble(nearWeldRoadVec.x);
	//buffer.WriteDouble(nearWeldRoadVec.y);
	//buffer.WriteDouble(nearWeldRoadVec.z);
}

void CLDSLineTube::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//已调用过)
		return;
	_console->AddToObjSetBuf(this);
	if(!bAddRelaObj)
		return;
	bAddRelaObj=bAddSubLevelRelaObj;
	if(pStart)
		pStart->AddToConsoleObjBuf(bAddRelaObj);
	if(pEnd)
		pEnd->AddToConsoleObjBuf(bAddRelaObj);

	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		pLsRef->GetLsPtr()->AddToConsoleObjBuf(bAddRelaObj);
}
//自协同中心通过管道读取数据
void CLDSLineTube::FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
{
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer))
	{
		IndependentShadowPropFromBuffer(buffer);
		return;
	}
	long h=0;
	buffer.ReadDword(&dwPermission);
	buffer.ReadDword(&dwStartPermission);
	buffer.ReadDword(&dwEndPermission);
	mpcl.FromBuffer(&buffer);

	if(dwPermission&dwUpdateWord)
	{
		buffer.Read(&cfgword.flag.word,24);
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.ReadInteger(&size.idClassType);
		buffer.ReadByte(&size.cSubClassType);
#endif
		buffer.ReadDouble(&size.wide);
		buffer.ReadDouble(&size.thick);
		buffer.ReadByte(&_material);
		buffer.ReadByte(&_cQualityLevel);
		buffer.ReadInteger(&m_uDisplaySlices);
		buffer.ReadInteger(&m_uSlices);
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)||defined(__TSA_FILE_)
		buffer.ReadInteger(&_uStatMatNo);
#endif
		buffer.ReadInteger(&m_uStatMatCoef);
		buffer.ReadInteger(&_hPartWeldParent);
		buffer.ReadByte(&_cFuncType);
		buffer.ReadBooleanThin(&m_bUserSpecColor);	//是否为指定构件颜色
		buffer.ReadDword(&crMaterial);				//构件颜色

		buffer.ReadWord(&m_iNormCalStyle);
		buffer.ReadDouble(&pickWorkPlane[0]);
		buffer.ReadDouble(&pickWorkPlane[1]);
		buffer.ReadDouble(&pickWorkPlane[2]);
		buffer.ReadInteger(&hTransTube);	//一直以来漏记这个属性　wjh-2016.4.12
		buffer.ReadInteger(&hWorkNormRefPart);
		buffer.ReadDouble(&workPlaneNorm.x);	// 主工作平面法线方向.
		buffer.ReadDouble(&workPlaneNorm.y);	// 主工作平面法线方向.
		buffer.ReadDouble(&workPlaneNorm.z);	// 主工作平面法线方向.
		buffer.ReadDouble(&cutPlaneOrigin.x);
		buffer.ReadDouble(&cutPlaneOrigin.y);
		buffer.ReadDouble(&cutPlaneOrigin.z);
		buffer.ReadDouble(&cutPlaneNorm.x);
		buffer.ReadDouble(&cutPlaneNorm.y);
		buffer.ReadDouble(&cutPlaneNorm.z);
		buffer.ReadBooleanThin(&_bVirtualPart);	//	隐含角钢
		buffer.ReadInteger(&_iSeg.iSeg);
		buffer.ReadString(sPartNo);	//原始构件编号
		buffer.ReadInteger(&m_hArcLift);		//预拱项 wht 16-01-07
		buffer.ReadString(layer(),4);					// 本角钢所在图层.
		buffer.ReadBooleanThin(&m_bDisplayHole);//显示螺栓孔及切割面
		buffer.ReadInteger(&_bHasWeldRoad);
		buffer.ReadDouble(&nearWeldRoadVec.x);
		buffer.ReadDouble(&nearWeldRoadVec.y);
		buffer.ReadDouble(&nearWeldRoadVec.z);
		buffer.ReadString(sNotes,51);
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)||defined(__TSA_FILE_)
		buffer.ReadInteger(&m_bAutoCalHoleOutCoef);	//自动计算螺栓减孔数，否则为指定螺栓减孔数
		buffer.ReadDouble(&hole_out);				//杆件上最多螺栓减孔数
		//
		buffer.ReadInteger(&CalLenCoef.iTypeNo);
		buffer.ReadInteger(&CalLenCoef.hHorizPole);
		buffer.ReadInteger(&CalLenCoef.hRefPole);
		buffer.ReadInteger(&CalLenCoef.minR.hStartNode);
		buffer.ReadInteger(&CalLenCoef.minR.hEndNode);
		buffer.ReadDouble(&CalLenCoef.minR.coef);
		buffer.ReadWord(&m_iElemType);			//0表示自动判断;1表示二力杆桁架;2表示经典梁单元;3表示柔索单元
		buffer.ReadDword(&_uStatMatNo);		//统材组号
		buffer.ReadInteger(&m_bNeedSelSize);	//需要选择规格
		buffer.ReadInteger(&m_bNeedSelMat);		//需要选择材质
		buffer.ReadInteger(&h);					//塔材选材方式
		sel_mat_method=AUX_POLE_SEL_METHOD(h);
		buffer.ReadDouble(&bear_load_coef);		//辅助材支撑所支撑杆件内力的
		buffer.ReadDouble(&m_fWeightRaiseCoef);	//重力放大系数，考虑联板、螺栓等对重力的放大作用
		buffer.ReadByte(&m_cbRodFlag);
#endif
		//关联构件
		int i,nn=0;
		buffer.ReadInteger(&nn);
		RELATIVE_OBJECT *pRelaObj=NULL;
		relativeObjs.Empty();
		for(i=0;i<nn;i++)
		{
			pRelaObj=relativeObjs.Add(buffer.ReadInteger());
			buffer.ReadByte(&pRelaObj->mirInfo.axis_flag);
			bool bSpecOrigin;
			buffer.ReadBooleanThin(&bSpecOrigin);
			if(bSpecOrigin)
				buffer.ReadPoint(pRelaObj->mirInfo.origin);
			if(pRelaObj->mirInfo.axis_flag&0x08)
			{
				buffer.ReadByte(&pRelaObj->mirInfo.array_num);
				buffer.ReadWord(&pRelaObj->mirInfo.rotate_angle);
			}
		}
		//钢管螺栓区 wht 12-03-19
		pLsRefList->FromBuffer(buffer,m_pModel);
	}
	if(dwStartPermission&dwUpdateWord)
	{
		buffer.ReadInteger(&h);
		//if(SuperiorTower()->hashObjs.GetHashTableSize()>0)
		//	SuperiorTower()->hashObjs.GetValueAt(h,(CLDSObject*&)pStart);
		//else
			pStart=BelongModel()->FromNodeHandle(h);	// 起点句柄
		f3dPoint start;
		buffer.ReadDouble(&start.x);// 实际楞线起点
		buffer.ReadDouble(&start.y);// 实际楞线起点
		buffer.ReadDouble(&start.z);// 实际楞线起点
		baseline.SetStart(start);
		buffer.ReadPoint(m_xStartBeforeLift);
		buffer.ReadDouble(&start_oddment);
		//始端正头计算参数
		buffer.ReadInteger(&desStartOdd.m_iOddCalStyle);
		buffer.ReadInteger(&desStartOdd.m_hRefPart1);
		buffer.ReadInteger(&desStartOdd.m_hRefPart2);
		buffer.ReadDouble(&desStartOdd.m_fCollideDist);
		buffer.ReadInteger(&m_tJointStart.type);
		buffer.ReadInteger(&m_tJointStart.hLinkObj);
		buffer.ReadInteger(&m_tJointStart.hViceLinkObj);
		buffer.ReadInteger(&m_tJointStart.bEndPlate);
		buffer.ReadInteger(&m_tJointStart.bVertPlate);
		buffer.ReadInteger(&m_tJointStart.typeEndPlate);
		buffer.ReadDouble(&m_tJointStart.m_fAxisXRotAngle);
		buffer.ReadDouble(&m_tJointStart.m_fWorkPlaneRotAngle);
		buffer.ReadDouble(&m_tJointStart.cutPlanePos.x);
		buffer.ReadDouble(&m_tJointStart.cutPlanePos.y);
		buffer.ReadDouble(&m_tJointStart.cutPlanePos.z);
		buffer.ReadDouble(&m_tJointStart.cutPlaneNorm.x);
		buffer.ReadDouble(&m_tJointStart.cutPlaneNorm.y);
		buffer.ReadDouble(&m_tJointStart.cutPlaneNorm.z);
		//
		buffer.ReadInteger(&desStartPos.endPosType);
		buffer.ReadByte(&desStartPos.datum_pos_style);
		buffer.ReadInteger(&desStartPos.hDatumPart);
		buffer.ReadInteger(&desStartPos.hDatumStartPart);
		buffer.ReadInteger(&desStartPos.hDatumEndPart);
		buffer.ReadInteger(&desStartPos.bDatumEndStartEnd);
		buffer.ReadInteger(&desStartPos.bDatumStartStartEnd);
		buffer.ReadDouble(&desStartPos.face_offset_norm.x);
		buffer.ReadDouble(&desStartPos.face_offset_norm.y);
		buffer.ReadDouble(&desStartPos.face_offset_norm.z);
		buffer.ReadDouble(&desStartPos.fEccentricDist);
		buffer.ReadDouble(&desStartPos.fLenVecOffsetDist);// wht 11-04-23
		buffer.ReadDouble(&desStartPos.fNormOffset);
		//设计信息
		buffer.ReadInteger(&m_bLockStartPos);
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		WORD w;
		buffer.ReadWord(&w);
		if(w==0)
			start_joint_type=JOINT_HINGE;
		else if(w==1)
			start_joint_type=JOINT_RIGID;
		else
			start_joint_type=JOINT_AUTOJUSTIFY;
		buffer.ReadWord(&w);
		if(w==0)
			start_force_type=CENTRIC_FORCE;
		else
			start_force_type=ECCENTRIC_FORCE;
		buffer.ReadDouble(&connectStart.maxTension);
		buffer.ReadDouble(&connectStart.maxCompression);
		buffer.ReadDouble(&connectStart.maxMoment);
		//螺栓信息
		buffer.ReadInteger(&connectStart.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Read(&connectStart.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Read(&connectStart.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		int iConnType=0;
		buffer.ReadInteger(&iConnType);
		connectStart.m_iConnectType=iConnType;
		buffer.ReadString(connectStart.flNo);
#endif
	}
	if(dwEndPermission&dwUpdateWord)
	{
		buffer.ReadInteger(&h);
		//if(SuperiorTower()->hashObjs.GetHashTableSize()>0)
		//	SuperiorTower()->hashObjs.GetValueAt(h,(CLDSObject*&)pEnd);
		//else
			pEnd  =BelongModel()->FromNodeHandle(h);	// 终点句柄
		f3dPoint end;
		buffer.ReadDouble(&end.x);	// 实际楞线终点
		buffer.ReadDouble(&end.y);	// 实际楞线终点
		buffer.ReadDouble(&end.z);	// 实际楞线终点
		baseline.SetEnd(end);
		buffer.ReadPoint(m_xEndBeforeLift);
		//终端正头计算参数
		buffer.ReadDouble(&end_oddment);   // 角钢始终端正负头.
		buffer.ReadInteger(&desEndOdd.m_iOddCalStyle);
		buffer.ReadInteger(&desEndOdd.m_hRefPart1);
		buffer.ReadInteger(&desEndOdd.m_hRefPart2);
		buffer.ReadDouble(&desEndOdd.m_fCollideDist);
		buffer.ReadInteger(&m_tJointEnd.type);
		buffer.ReadInteger(&m_tJointEnd.hLinkObj);
		if(m_tJointStart.hLinkObj<0)
			m_tJointStart.hLinkObj = 0;
		if(m_tJointEnd.hLinkObj<0)
			m_tJointEnd.hLinkObj=0;
		buffer.ReadInteger(&m_tJointEnd.hViceLinkObj);
		buffer.ReadInteger(&m_tJointEnd.bEndPlate);
		buffer.ReadInteger(&m_tJointEnd.bVertPlate);
		buffer.ReadInteger(&m_tJointEnd.typeEndPlate);
		buffer.ReadDouble(&m_tJointEnd.m_fAxisXRotAngle);
		buffer.ReadDouble(&m_tJointEnd.m_fWorkPlaneRotAngle);
		buffer.ReadDouble(&m_tJointEnd.cutPlanePos.x);
		buffer.ReadDouble(&m_tJointEnd.cutPlanePos.y);
		buffer.ReadDouble(&m_tJointEnd.cutPlanePos.z);
		buffer.ReadDouble(&m_tJointEnd.cutPlaneNorm.x);
		buffer.ReadDouble(&m_tJointEnd.cutPlaneNorm.y);
		buffer.ReadDouble(&m_tJointEnd.cutPlaneNorm.z);
		//
		buffer.ReadInteger(&desEndPos.endPosType);
		buffer.ReadByte(&desEndPos.datum_pos_style);
		buffer.ReadInteger(&desEndPos.hDatumPart);
		buffer.ReadInteger(&desEndPos.hDatumStartPart);
		buffer.ReadInteger(&desEndPos.hDatumEndPart);
		buffer.ReadInteger(&desEndPos.bDatumEndStartEnd);
		buffer.ReadInteger(&desEndPos.bDatumStartStartEnd);
		buffer.ReadDouble(&desEndPos.face_offset_norm.x);
		buffer.ReadDouble(&desEndPos.face_offset_norm.y);
		buffer.ReadDouble(&desEndPos.face_offset_norm.z);
		buffer.ReadDouble(&desEndPos.fEccentricDist);
		buffer.ReadDouble(&desEndPos.fLenVecOffsetDist);// wht 11-04-23
		buffer.ReadDouble(&desEndPos.fNormOffset);
		//设计信息
		buffer.ReadInteger(&m_bLockEndPos);
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		WORD w;
		buffer.ReadWord(&w);
		if(w==0)
			end_joint_type=JOINT_HINGE;
		else if(w==1)
			end_joint_type=JOINT_RIGID;
		else
			end_joint_type=JOINT_AUTOJUSTIFY;
		buffer.ReadWord(&w);
		if(w==0)
			end_force_type=CENTRIC_FORCE;
		else
			end_force_type=ECCENTRIC_FORCE;
		buffer.ReadDouble(&connectEnd.maxTension);
		buffer.ReadDouble(&connectEnd.maxCompression);
		buffer.ReadDouble(&connectEnd.maxMoment);
		//螺栓信息
		buffer.ReadInteger(&connectEnd.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Read(&connectEnd.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Read(&connectEnd.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		int iConnType=0;
		buffer.ReadInteger(&iConnType);
		connectEnd.m_iConnectType=iConnType;
		buffer.ReadString(connectEnd.flNo);
#endif
	}
	SetModified();
}
//通过管道向协同中心写入数据
void CLDSLineTube::ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdToBuffer(buffer))
	{
		IndependentShadowPropToBuffer(buffer);
		return;
	}
	buffer.WriteDword(dwPermission);
	buffer.WriteDword(dwStartPermission);
	buffer.WriteDword(dwEndPermission);
	mpcl.ToBuffer(&buffer);

	if(dwPermission&dwUpdateWord)
	{
		buffer.Write(cfgword.flag.word,24);
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.WriteInteger(size_idClassType);
		buffer.WriteByte(size_cSubClassType);
#endif
		buffer.WriteDouble(size_wide);
		buffer.WriteDouble(size_thick);
		buffer.WriteByte(cMaterial);
		buffer.WriteByte(m_cQualityLevel);
		buffer.WriteInteger(m_uDisplaySlices);
		buffer.WriteInteger(m_uSlices);
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.WriteInteger(m_uStatMatNo);
#endif
		buffer.WriteInteger(m_uStatMatCoef);
		buffer.WriteInteger(m_hPartWeldParent);
		buffer.WriteByte(m_cFuncType);
		buffer.WriteBooleanThin(m_bUserSpecColor);	//是否为指定构件颜色
		buffer.WriteDword(crMaterial);				//构件颜色
		
		buffer.WriteWord(m_iNormCalStyle);	//1000083
		buffer.WriteDouble(pickWorkPlane[0]);
		buffer.WriteDouble(pickWorkPlane[1]);
		buffer.WriteDouble(pickWorkPlane[2]);
		buffer.WriteInteger(hTransTube);	//一直以来漏记这个属性　wjh-2016.4.12
		buffer.WriteInteger(hWorkNormRefPart);
		buffer.WriteDouble(workPlaneNorm.x);	// 主工作平面法线方向.
		buffer.WriteDouble(workPlaneNorm.y);	// 主工作平面法线方向.
		buffer.WriteDouble(workPlaneNorm.z);	// 主工作平面法线方向.
		buffer.WriteDouble(cutPlaneOrigin.x);
		buffer.WriteDouble(cutPlaneOrigin.y);
		buffer.WriteDouble(cutPlaneOrigin.z);
		buffer.WriteDouble(cutPlaneNorm.x);
		buffer.WriteDouble(cutPlaneNorm.y);
		buffer.WriteDouble(cutPlaneNorm.z);
		buffer.WriteBooleanThin(_bVirtualPart);	//	隐含钢管
		buffer.WriteInteger(iSeg);
		buffer.WriteString(sPartNo);				//原始构件编号
		buffer.WriteInteger(m_hArcLift);			//预拱项 wht 16-01-07
		buffer.WriteString(layer());				//构件所在图层.
		buffer.WriteBooleanThin(m_bDisplayHole);	//显示螺栓孔及切割面
		buffer.WriteInteger(m_bHasWeldRoad);		//焊道线
		buffer.WriteDouble(nearWeldRoadVec.x);
		buffer.WriteDouble(nearWeldRoadVec.y);
		buffer.WriteDouble(nearWeldRoadVec.z);
		buffer.WriteString(sNotes);
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.WriteInteger(m_bAutoCalHoleOutCoef);	//自动计算螺栓减孔数，否则为指定螺栓减孔数
		buffer.WriteDouble(hole_out);			//杆件上最多螺栓减孔数
		//
		buffer.WriteInteger(CalLenCoef.iTypeNo);
		buffer.WriteInteger(CalLenCoef.hHorizPole);
		buffer.WriteInteger(CalLenCoef.hRefPole);
		buffer.WriteInteger(CalLenCoef.minR.hStartNode);
		buffer.WriteInteger(CalLenCoef.minR.hEndNode);
		buffer.WriteDouble(CalLenCoef.minR.coef);
		buffer.WriteWord(m_iElemType);			//0表示自动判断;1表示二力杆桁架;2表示经典梁单元;3表示柔索单元
		buffer.WriteDword(m_uStatMatNo);		//统材组号
		buffer.WriteInteger(m_bNeedSelSize);	//需要选择规格
		buffer.WriteInteger(m_bNeedSelMat);		//需要选择材质
		buffer.WriteInteger(sel_mat_method);	//塔材选材方式
		buffer.WriteDouble(bear_load_coef);		//辅助材支撑所支撑杆件内力的
		buffer.WriteDouble(m_fWeightRaiseCoef);	//重力放大系数，考虑联板、螺栓等对重力的放大作用
		buffer.WriteByte(m_cbRodFlag);
#endif
		//关联构件列表
		RELATIVE_OBJECT *pRelaObj=NULL;
		buffer.WriteInteger(relativeObjs.GetNodeNum());
		for(pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
		{
			buffer.WriteInteger(pRelaObj->hObj);
			buffer.WriteByte(pRelaObj->mirInfo.axis_flag);
			bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
			buffer.WriteBooleanThin(bSpecOrigin);
			if(bSpecOrigin)
				buffer.WritePoint(pRelaObj->mirInfo.origin);
			if(pRelaObj->mirInfo.axis_flag&0x08)
			{
				buffer.WriteByte(pRelaObj->mirInfo.array_num);
				buffer.WriteWord(pRelaObj->mirInfo.rotate_angle);
			}
		}
		//钢管螺栓区 wht 12-03-19
		pLsRefList->ToBuffer(buffer);
	}
	if(dwStartPermission&dwUpdateWord)
	{
		if(pStart)
			buffer.WriteInteger(pStart->handle);	// 起点句柄
		else
			buffer.WriteInteger(0);
		buffer.WriteDouble(baseline.Start().x);// 实际楞线起点
		buffer.WriteDouble(baseline.Start().y);// 实际楞线起点
		buffer.WriteDouble(baseline.Start().z);// 实际楞线起点
		buffer.WritePoint(m_xStartBeforeLift);
		//始端正头计算参数
		buffer.WriteDouble(start_oddment);
		buffer.WriteInteger(desStartOdd.m_iOddCalStyle);
		buffer.WriteInteger(desStartOdd.m_hRefPart1);
		buffer.WriteInteger(desStartOdd.m_hRefPart2);
		buffer.WriteDouble(desStartOdd.m_fCollideDist);
		//
		buffer.WriteInteger(m_tJointStart.type);
		buffer.WriteInteger(m_tJointStart.hLinkObj);
		buffer.WriteInteger(m_tJointStart.hViceLinkObj);
		buffer.WriteInteger(m_tJointStart.bEndPlate);
		buffer.WriteInteger(m_tJointStart.bVertPlate);
		buffer.WriteInteger(m_tJointStart.typeEndPlate);
		buffer.WriteDouble(m_tJointStart.m_fAxisXRotAngle);
		buffer.WriteDouble(m_tJointStart.m_fWorkPlaneRotAngle);
		buffer.WriteDouble(m_tJointStart.cutPlanePos.x);
		buffer.WriteDouble(m_tJointStart.cutPlanePos.y);
		buffer.WriteDouble(m_tJointStart.cutPlanePos.z);
		buffer.WriteDouble(m_tJointStart.cutPlaneNorm.x);
		buffer.WriteDouble(m_tJointStart.cutPlaneNorm.y);
		buffer.WriteDouble(m_tJointStart.cutPlaneNorm.z);
		//
		buffer.WriteInteger(desStartPos.endPosType);
		buffer.WriteByte(desStartPos.datum_pos_style);
		buffer.WriteInteger(desStartPos.hDatumPart);
		buffer.WriteInteger(desStartPos.hDatumStartPart);
		buffer.WriteInteger(desStartPos.hDatumEndPart);
		buffer.WriteInteger(desStartPos.bDatumEndStartEnd);
		buffer.WriteInteger(desStartPos.bDatumStartStartEnd);
		buffer.WriteDouble(desStartPos.face_offset_norm.x);
		buffer.WriteDouble(desStartPos.face_offset_norm.y);
		buffer.WriteDouble(desStartPos.face_offset_norm.z);
		buffer.WriteDouble(desStartPos.fEccentricDist);
		buffer.WriteDouble(desStartPos.fLenVecOffsetDist); // wht 11-04-23
		buffer.WriteDouble(desStartPos.fNormOffset);
		buffer.WriteInteger(m_bLockStartPos);
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.WriteWord((WORD)start_force_type);
		buffer.WriteWord((WORD)start_joint_type);
		buffer.WriteDouble(connectStart.maxTension);
		buffer.WriteDouble(connectStart.maxCompression);
		buffer.WriteDouble(connectStart.maxMoment);
		//螺栓信息
		buffer.WriteInteger(connectStart.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Write(connectStart.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Write(&connectStart.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.WriteInteger(connectStart.m_iConnectType);
		buffer.WriteString(connectStart.flNo);
#endif
	}
	if(dwEndPermission&dwUpdateWord)
	{
		if(pEnd)
			buffer.WriteInteger(pEnd->handle);
		else
			buffer.WriteInteger(0);
		buffer.WriteDouble(baseline.End().x);	// 实际楞线终点
		buffer.WriteDouble(baseline.End().y);	// 实际楞线终点
		buffer.WriteDouble(baseline.End().z);	// 实际楞线终点
		buffer.WritePoint(m_xEndBeforeLift);
		//终端正头计算参数
		buffer.WriteDouble(end_oddment);   // 角钢始终端正负头.
		buffer.WriteInteger(desEndOdd.m_iOddCalStyle);
		buffer.WriteInteger(desEndOdd.m_hRefPart1);
		buffer.WriteInteger(desEndOdd.m_hRefPart2);
		buffer.WriteDouble(desEndOdd.m_fCollideDist);
		//
		buffer.WriteInteger(m_tJointEnd.type);
		buffer.WriteInteger(m_tJointEnd.hLinkObj);
		buffer.WriteInteger(m_tJointEnd.hViceLinkObj);
		buffer.WriteInteger(m_tJointEnd.bEndPlate);
		buffer.WriteInteger(m_tJointEnd.bVertPlate);
		buffer.WriteInteger(m_tJointEnd.typeEndPlate);
		buffer.WriteDouble(m_tJointEnd.m_fAxisXRotAngle);
		buffer.WriteDouble(m_tJointEnd.m_fWorkPlaneRotAngle);
		buffer.WriteDouble(m_tJointEnd.cutPlanePos.x);
		buffer.WriteDouble(m_tJointEnd.cutPlanePos.y);
		buffer.WriteDouble(m_tJointEnd.cutPlanePos.z);
		buffer.WriteDouble(m_tJointEnd.cutPlaneNorm.x);
		buffer.WriteDouble(m_tJointEnd.cutPlaneNorm.y);
		buffer.WriteDouble(m_tJointEnd.cutPlaneNorm.z);
		//
		buffer.WriteInteger(desEndPos.endPosType);
		buffer.WriteByte(desEndPos.datum_pos_style);
		buffer.WriteInteger(desEndPos.hDatumPart);
		buffer.WriteInteger(desEndPos.hDatumStartPart);
		buffer.WriteInteger(desEndPos.hDatumEndPart);
		buffer.WriteInteger(desEndPos.bDatumEndStartEnd);
		buffer.WriteInteger(desEndPos.bDatumStartStartEnd);
		buffer.WriteDouble(desEndPos.face_offset_norm.x);
		buffer.WriteDouble(desEndPos.face_offset_norm.y);
		buffer.WriteDouble(desEndPos.face_offset_norm.z);
		buffer.WriteDouble(desEndPos.fEccentricDist);
		buffer.WriteDouble(desEndPos.fLenVecOffsetDist); // wht 11-04-23
		buffer.WriteDouble(desEndPos.fNormOffset);
		buffer.WriteInteger(m_bLockEndPos);
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.WriteWord((WORD)end_force_type);
		buffer.WriteWord((WORD)end_joint_type);
		buffer.WriteDouble(connectEnd.maxTension);
		buffer.WriteDouble(connectEnd.maxCompression);
		buffer.WriteDouble(connectEnd.maxMoment);
		//螺栓信息
		buffer.WriteInteger(connectEnd.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Write(connectEnd.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Write(&connectEnd.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.WriteInteger(connectEnd.m_iConnectType);
		buffer.WriteString(connectEnd.flNo);
#endif
	}
}

f3dPoint CLDSLineTube::GetPlateVertSpreadVec(CLDSGeneralPlate *pPlate,char ciPlane/*=0*/)	//获取钢板平面上垂直于钢管轴线的延展方向
{
	f3dPoint line_start=Start();
	f3dPoint line_end=End();
	coord_trans(line_start,pPlate->ucs,FALSE);
	coord_trans(line_end,pPlate->ucs,FALSE);
	GEPOINT vVertSpreadVec;
	if(ciPlane>0)
	{
		GEPOINT vPlaneNormal=pPlate->GetHuoquFaceNorm(ciPlane-1);
		GEPOINT lenTubeVec=End()-Start();
		vVertSpreadVec=vPlaneNormal^lenTubeVec;
		if(!normalize(vVertSpreadVec))
			ciPlane=0;	//火曲面与钢管轴向垂直,取基面延伸方向
	}
	PROFILE_VERTEX* pFirstVertex=pPlate->vertex_list.GetFirst();
	if(fabs(line_start.x-line_end.x)+fabs(line_start.y-line_end.y)<EPS && pFirstVertex)
	{	//钢管与钢板垂直
		f3dPoint vert_vec=pFirstVertex->vertex-line_start;
		normalize(vert_vec);
		vector_trans(vert_vec,pPlate->ucs,TRUE);	//延展方向转换到绝对坐标系
		if(ciPlane==0)
			return vert_vec;
		else if(vVertSpreadVec*vert_vec<0)
			vVertSpreadVec*=-1;
		return vVertSpreadVec;
	}
	int side_style=0;	//0x01左侧;0x02右侧;0x03两侧
	for(PROFILE_VERTEX *pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		short side=SideOfPtLine(pVertex->vertex,line_start,line_end);
		if(side>0)		//点在轴线左侧
			side_style|=0x01;
		else if(side<0)	//点在轴线右侧
			side_style|=0x02;
		if(side_style==3)
			break;
	}
	if(side_style==0||side_style==3)	//原始数据有误，无法区分在哪一侧
		return f3dPoint(0,0,0);
	f3dPoint line_vec=line_end-line_start;
	f3dPoint vert_vec(-line_vec.y,line_vec.x);	//逆时针转90度获取基准轴线左侧垂直延展方向
	normalize(vert_vec);
	if(side_style==2)
		vert_vec*=-1.0;
	vector_trans(vert_vec,pPlate->ucs,TRUE);	//延展方向转换到绝对坐标系
	if(ciPlane==0)
		return vert_vec;
	else if(vVertSpreadVec*vert_vec<0)
		vVertSpreadVec*=-1;
	return vVertSpreadVec;
}
f3dPoint CLDSLineTube::WorkPlaneNorm()
{
	//纠正工作面法线方向
	/*f3dPoint axis_z = baseline.End()-baseline.Start();
	if(axis_z.mod()>EPS)
	{
		f3dPoint axis_y = axis_z^workPlaneNorm;
		workPlaneNorm = axis_y^axis_z;
	}
	normalize(workPlaneNorm);*/
	return workPlaneNorm;
}

void CLDSLineTube::CalWorkPlaneNorm()
{
	if(m_iNormCalStyle==1)
	{	//根据参照杆件计算工作面法线
		CLDSLinePart *pDatumLinePart=NULL;
		//1.指定了工作面参照杆件
		if(hWorkNormRefPart!=0)	
			pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(hWorkNormRefPart,CLS_LINEPART);
		if(hWorkNormRefPart!=0&&pDatumLinePart==NULL)
			logerr.Log("0X%X钢管的工作面法线参照杆件0X%X杆件丢失!",handle,hWorkNormRefPart);
		//2.未指定工作面参照杆件时，以始端搭接杆件为工作面参照杆件
		if(pDatumLinePart==NULL&&pStart&&pStart->hFatherPart>=0x20&&pStart->hFatherPart!=handle)	//始端搭接在其它杆件上
		{	
			if(pStart->hFatherPart>=0x20)
				pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(pStart->hFatherPart,CLS_LINEPART);
			else if(desStartPos.hDatumPart>=0x20)
				pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(desStartPos.hDatumPart,CLS_LINEPART);
		}
		//3.未指定工作面参照杆件且始端无搭接杆件时，以终端搭接杆件为工作面参照杆件
		if(pDatumLinePart==NULL&&pEnd&&pEnd->hFatherPart>=0x20&&pEnd->hFatherPart!=handle)	//终端搭接在其它杆件上
		{	
			if(pEnd->hFatherPart>=0x20)
				pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(pEnd->hFatherPart,CLS_LINEPART);
			else if(desEndPos.hDatumPart>=0x20)
				pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(desEndPos.hDatumPart,CLS_LINEPART);
		}
		if(pDatumLinePart)
		{	//存在工作面法线参照杆件
			/*f3dPoint datum_vec=pDatumLinePart->baseline.End()-pDatumLinePart->baseline.Start();
			normalize(datum_vec);
			if(fabs(datum_vec*current_vec)>EPS_COS)
			{	//基准钢管与当前钢管平行
			datum_vec=pDatumLinePart->End()-End();
			normalize(datum_vec);
			}
			workPlaneNorm=datum_vec^current_vec;*/
			workPlaneNorm=CalFaceNorm(Start(),End(),pDatumLinePart->Start(),pDatumLinePart->End());
			normalize(workPlaneNorm);
		}
		else if(fabs(pickWorkPlane[0])+fabs(pickWorkPlane[1])+fabs(pickWorkPlane[2])>eps)//!nearWorkPlaneNorm.IsZero())
		{	//无合理工作面法线参照杆件，以近似工作面法线计算工作面法线
			f3dPoint current_vec;
			current_vec=Start()-End();
			normalize(current_vec);
			workPlaneNorm=f3dPoint(pickWorkPlane);
			f3dPoint vec=workPlaneNorm^current_vec;
			workPlaneNorm=current_vec^vec;
			normalize(workPlaneNorm);
			logerr.Log("0X%X钢管无合理工作面法线参照杆件，以近似工作面法线计算工作面法线!",handle);
		}
		if(f3dPoint(pickWorkPlane)*workPlaneNorm<0)
			workPlaneNorm=-workPlaneNorm;
		SetModified();
	}
	else if(m_iNormCalStyle==2)
	{	//根据参照点计算钢管工作面法线 wjh-2015.8.7
		f3dPoint axis_vec=End()-Start();
		normalize(axis_vec);
		f3dPoint hv=f3dPoint(pickWorkPlane)-Start();
		hv-=(hv*axis_vec)*axis_vec;	//获取垂直于钢管轴线的垂直投影矢量
		normalize(hv);
		workPlaneNorm=axis_vec^hv;
		if(workPlaneNorm.IsZero())
			Log2File()->Log("0X%X钢管根据参照点计算的工作面法线无效!",handle);
	}

}

UCS_STRU CLDSLineTube::BuildUCS()
{
	ucs.axis_z=End()-Start();
	if(m_cFuncType==FUNC_WIREBUSH&&pStart==NULL&&pEnd==NULL)
		ucs.origin=0.5*(Start()+End());
	else
		ucs.origin=Start();
	if(!normalize(workPlaneNorm))
		workPlaneNorm=inters_vec(ucs.axis_z);
	ucs.axis_y=workPlaneNorm;
	normalize(ucs.axis_z);
	ucs.axis_x=ucs.axis_y^ucs.axis_z;
	normalize(ucs.axis_x);
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	return ucs;
}


BOOL CLDSLineTube::DesignStartJointUCS(CLDSParamPlate *pParamPlate/*=NULL*/)
{
	if(pParamPlate==NULL)
	{
		if(m_tJointStart.type>=2)
			pParamPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(m_tJointStart.hLinkObj,CLS_PARAMPLATE);
	}
	if(pParamPlate==NULL)
		return FALSE;
	if(pParamPlate->m_bStdPart)
		return pParamPlate->DesignStdPartUcs();	//设计标准构件的坐标系
	BuildUCS();
	pParamPlate->SetModified();
	if(pParamPlate->m_iParamType==TYPE_FL)		//法兰
	{
		double D=0,H=0;
		pParamPlate->GetDesignItemValue('D',&D);
		pParamPlate->GetDesignItemValue('H',&H);
		D=D*RADTODEG_COEF;
		pParamPlate->ucs.axis_z=Start()-End();
		normalize(pParamPlate->ucs.axis_z);
		pParamPlate->ucs.origin=Start()+(start_oddment+H-pParamPlate->thick)*pParamPlate->ucs.axis_z;
		pParamPlate->ucs.axis_x=ucs.axis_x;
		double rot_ang=0;
		long J=0;
		if(!pParamPlate->GetDesignItemValue('J',&J))
			J=feature;	//读取钢管中以前变量m_bAttachPartByWeldRoad的值 现在该变量已去掉
		if(J!=0)//m_bHasWeldRoad&&  m_bAttachPartByWeldRoad)	//以焊道为基准作为角度定位起始边
			rot_ang=CalWeldRoadAngle();
		if((D+rot_ang)!=0)
			RotateVectorAroundVector(pParamPlate->ucs.axis_x,D+rot_ang,ucs.axis_z);
		pParamPlate->ucs.axis_y=pParamPlate->ucs.axis_z^pParamPlate->ucs.axis_x;
		normalize(pParamPlate->ucs.axis_y);
	}
	else if(pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND||
		pParamPlate->m_iParamType==TYPE_VERTPLATE||pParamPlate->m_iParamType==TYPE_XEND)
	{
		double M=0;
		pParamPlate->GetDesignItemValue('M',&M);
		pParamPlate->ucs.axis_x=Start()-End();
		normalize(pParamPlate->ucs.axis_x);
		pParamPlate->ucs.origin=Start()+(M-start_oddment)*ucs.axis_z;
		if(pParamPlate->m_iParamType==TYPE_XEND)
		{
			if(pParamPlate->workNorm.UpdateVector(BelongModel()))
				pParamPlate->ucs.axis_z=pParamPlate->workNorm.vector;
			else
				pParamPlate->ucs.axis_z=workPlaneNorm;
		}
		else
		{
			double D=0;
			pParamPlate->GetDesignItemValue('D',&D);
			D=D*RADTODEG_COEF;
			pParamPlate->ucs.axis_z=workPlaneNorm;
			if(D!=0)
				RotateVectorAroundVector(pParamPlate->ucs.axis_z,D,ucs.axis_z);
		}
		normalize(pParamPlate->ucs.axis_z);
		pParamPlate->ucs.axis_y=pParamPlate->ucs.axis_z^pParamPlate->ucs.axis_x;
		pParamPlate->ucs.axis_z=pParamPlate->ucs.axis_x^pParamPlate->ucs.axis_y;
		normalize(pParamPlate->ucs.axis_y);
		normalize(pParamPlate->ucs.axis_z);
	}
	pParamPlate->ucs.origin+=pParamPlate->ucs.axis_z*pParamPlate->m_fNormOffset;
	return TRUE;
}
BOOL CLDSLineTube::DesignEndJointUCS(CLDSParamPlate *pParamPlate/*=NULL*/)
{
	if(pParamPlate==NULL)
	{
		if(m_tJointEnd.type>=2)
			pParamPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
	}
	if(pParamPlate==NULL)
		return FALSE;
	if(pParamPlate->m_bStdPart)
		return pParamPlate->DesignStdPartUcs();	//设计标准构件的坐标系
	BuildUCS();
	pParamPlate->SetModified();
	if(pParamPlate->m_iParamType==TYPE_FL)		//法兰
	{
		double D=0,H=0;
		pParamPlate->GetDesignItemValue('D',&D);
		pParamPlate->GetDesignItemValue('H',&H);
		D=D*RADTODEG_COEF;
		pParamPlate->ucs.axis_z=End()-Start();
		normalize(pParamPlate->ucs.axis_z);
		pParamPlate->ucs.origin=End()+(end_oddment+H-pParamPlate->thick)*pParamPlate->ucs.axis_z;
		pParamPlate->ucs.axis_x=ucs.axis_x;
		double rot_ang=0;
		long J=0;
		if(!pParamPlate->GetDesignItemValue('J',&J))
			J=feature;	//读取钢管中以前变量m_bAttachPartByWeldRoad的值 现在该变量已去掉
		if(J!=0)	//以焊道为基准作为角度定位起始边	//m_bHasWeldRoad&&
			rot_ang=CalWeldRoadAngle();
		if((D+rot_ang)!=0)
			RotateVectorAroundVector(pParamPlate->ucs.axis_x,D+rot_ang,ucs.axis_z);
		pParamPlate->ucs.axis_y=pParamPlate->ucs.axis_z^pParamPlate->ucs.axis_x;
		normalize(pParamPlate->ucs.axis_y);
	}
	else if(pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND||
		pParamPlate->m_iParamType==TYPE_VERTPLATE||pParamPlate->m_iParamType==TYPE_XEND)
	{
		double M=0;
		pParamPlate->GetDesignItemValue('M',&M);
		pParamPlate->ucs.axis_x=End()-Start();
		normalize(pParamPlate->ucs.axis_x);
		pParamPlate->ucs.origin=End()+(end_oddment-M)*ucs.axis_z;
		if(pParamPlate->m_iParamType==TYPE_XEND)
		{
			if(pParamPlate->workNorm.UpdateVector(BelongModel()))
				pParamPlate->ucs.axis_z=pParamPlate->workNorm.vector;
			else
				pParamPlate->ucs.axis_z=workPlaneNorm;
		}
		else
		{
			double D=0;
			pParamPlate->GetDesignItemValue('D',&D);
			D=D*RADTODEG_COEF;
			pParamPlate->ucs.axis_z=workPlaneNorm;
			if(D!=0)
				RotateVectorAroundVector(pParamPlate->ucs.axis_z,D,ucs.axis_z);
		}
		normalize(pParamPlate->ucs.axis_z);
		pParamPlate->ucs.axis_y=pParamPlate->ucs.axis_z^pParamPlate->ucs.axis_x;
		pParamPlate->ucs.axis_z=pParamPlate->ucs.axis_x^pParamPlate->ucs.axis_y;
		normalize(pParamPlate->ucs.axis_y);
		normalize(pParamPlate->ucs.axis_z);
	}
	pParamPlate->ucs.origin+=pParamPlate->ucs.axis_z*pParamPlate->m_fNormOffset;
	return TRUE;
}
BOOL CLDSLineTube::DesignStartJoint()
{
	if(m_tJointStart.hLinkObj==0)
		return FALSE;
	if(m_tJointStart.type>=2)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		if(pParamPlate==NULL)
			return FALSE;
		DesignStartJointUCS(pParamPlate);
		pParamPlate->DesignPlate();
	}
	return TRUE;
}
BOOL CLDSLineTube::DesignEndJoint()
{
	if(m_tJointEnd.hLinkObj==0)
		return FALSE;
	if(m_tJointEnd.type>=2)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		if(pParamPlate==NULL)
			return FALSE;
		DesignEndJointUCS(pParamPlate);
		pParamPlate->DesignPlate();
	}
	return TRUE;
}

double CLDSLineTube::GetCutCylinderFace(int slices,fPtList &vertex_list, f3dPoint &extend_vec)
{
	double slice_angle = 2*Pi/slices;
	double cosa = cos(slice_angle*0.5);

	BuildUCS();
	f3dPoint vertex_bottom,vertex_start;
	extend_vec=ucs.axis_z;
	// 0.5*d/cosa目的是用显示圆多边形与圆内切
	vertex_start = Start()-ucs.axis_z*startOdd()+ucs.axis_x*0.5*GetDiameter()/cosa;
	for(int i=0;i<slices;i++)
	{
		vertex_bottom=vertex_start;
		if(i==0)
			vertex_list.append(vertex_bottom);	//底面外圆弧点
		else
		{
			rotate_point_around_axis(vertex_bottom,slice_angle*i,Start(),End());
			vertex_list.append(vertex_bottom);
		}
	}
	return GetLength();
}

void CLDSLineTube::CopyProperty(CLDSDbObject* pObj)
{
    SetLayer(pObj->layer());						// 本角钢所在图层.
	iSeg=pObj->iSeg;
	cfgword=pObj->cfgword;
	is_highlight=pObj->is_highlight;
	is_visible=pObj->is_visible;
	is_data_modified = TRUE;
	if(!pObj->IsPart())
		return;
	CLDSPart* pPart=(CLDSPart*)pObj;
	size_wide = pPart->GetWidth();
	size_thick = pPart->GetThick();
	m_bVirtualPart = pPart->m_bVirtualPart;
	//start_oddment = pTube->start_oddment;	//实践证明正负头对称不好，不方便
	//end_oddment = pTube->end_oddment;
	sPartNo.Copy(pPart->GetPartNo());
	m_uStatMatCoef = pPart->m_uStatMatCoef;
	cMaterial = pPart->cMaterial;
	cbVirginQualityLevel = pPart->cbVirginQualityLevel;
#ifdef __LDS_
	if(!pPart->IsLinePart())
		return;
	CLDSLinePart* pRod=(CLDSLinePart*)pObj;
	m_uStatMatNo=pRod->m_uStatMatNo;
	m_bNeedSelSize=pRod->m_bNeedSelSize;
	start_force_type=pRod->start_force_type;
	end_force_type=pRod->end_force_type;
	start_joint_type=pRod->start_joint_type;
	end_joint_type=pRod->end_joint_type;
	CalLenCoef.iTypeNo=pRod->CalLenCoef.iTypeNo;
	m_cbRodFlag=pRod->m_cbRodFlag;
#endif
	if(pObj->GetClassTypeId()!=CLS_LINETUBE)
		return;
	CLDSLineTube *pTube=(CLDSLineTube*)pObj;
    size_wide = pTube->GetDiameter();
	m_cFuncType=pTube->m_cFuncType;
	if(m_cFuncType==CLDSLineTube::FUNC_WIREBUSH)
		bush.height=pTube->bush.height;
	m_uDisplaySlices = pTube->m_uDisplaySlices;						//隐含角钢
	m_uSlices=pTube->m_uSlices;
	if(!IsStartPosLocked())
		desStartPos.endPosType=pTube->desStartPos.endPosType;
	if(!IsEndPosLocked())
		desEndPos.endPosType=pTube->desEndPos.endPosType;
	m_bHasWeldRoad=pTube->m_bHasWeldRoad;
}

/* 已转移至CLDSLinePart::GetDatumPos（）中, wjh-2014.5.28
//wht 08-02-21 
f3dPoint CLDSLineTube::GetDatumPosBer(CLDSNode *pNode)
{
	try{
		f3dPoint datum_point;
		if(pNode==pStart)
		{	//始端节点
			return Start();
		}
		else if(pNode==pEnd)
		{	//终端节点
			return End();
		}
		else
		{	
			f3dPoint perp;
			SnapPerp(&perp,Start(),End(),pNode->Position(true));
			return perp;
			//中间节点中除交叉点，其余情况节点父角钢均应为当前角钢
			//return f3dPoint(0,0,0);
		}
	}
	catch(CPtrException &e)
	{
		AfxMessageBox(e.sError);
		return f3dPoint(0,0,0);
	}
}
*/
void CLDSLineTube::GetProcessStr(char *notes, size_t max_len)
{
	//得到特殊处理字符串 如压扁等
}

double CLDSLineTube::GetStartActualOdd()
{
	f3dPoint axis_vec=Start()-End();
	if(m_tJointStart.type==0&&(m_tJointStart.cutPlaneNorm*axis_vec)!=0)
	{
		normalize(axis_vec);
		f3dPoint offset_vec=m_tJointStart.cutPlaneNorm-(axis_vec*m_tJointStart.cutPlaneNorm)*axis_vec;
		normalize(offset_vec);
		f3dPoint line_pick1=Start()+offset_vec*GetDiameter()*0.5;
		f3dPoint line_pick2=Start()-offset_vec*GetDiameter()*0.5;
		f3dPoint inters1,inters2;
		Int3dlf(inters1,line_pick1,axis_vec,m_tJointStart.cutPlanePos,m_tJointStart.cutPlaneNorm);
		Int3dlf(inters2,line_pick2,axis_vec,m_tJointStart.cutPlanePos,m_tJointStart.cutPlaneNorm);
		double dd1=(inters1-line_pick1)*axis_vec;
		double dd2=(inters2-line_pick2)*axis_vec;
		double dd=max(dd1,dd2);
		if(dd<startOdd())
			return dd;
		else
			return startOdd();
	}
	else if(m_tJointStart.type==1&&m_tJointStart.hLinkObj>0)
	{
		CLDSLineTube *pTransTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_tJointStart.hLinkObj,CLS_LINETUBE);
		if(pTransTube&&pTransTube->GetDiameter()>GetDiameter())	//粗判
		{
			double radius=GetDiameter()*0.5,trans_radius=pTransTube->GetDiameter()*0.5;
			UCS_STRU tmucs;
			tmucs.origin=Start();
			tmucs.axis_z=Start()-End();
			normalize(tmucs.axis_z);
			f3dPoint trans_axis=pTransTube->End()-pTransTube->Start();
			tmucs.axis_x=trans_axis^tmucs.axis_z;
			normalize(tmucs.axis_x);
			tmucs.axis_y=tmucs.axis_z^tmucs.axis_x;
			f3dPoint trans_pos=pTransTube->End();
			coord_trans(trans_pos,tmucs,FALSE);
			if(fabs(trans_pos.x)>(trans_radius-radius))
				return startOdd();
			f3dPoint line_pick1=tmucs.origin-tmucs.axis_x*radius;
			f3dPoint line_pick2=tmucs.origin+tmucs.axis_x*radius;
			f3dPoint inters1,inters2;
			Int3dlc(&inters1,&inters2,line_pick1,tmucs.axis_z,
				pTransTube->Start(),pTransTube->End(),trans_radius,trans_radius);
			double d1=(inters1-line_pick1)*tmucs.axis_z;
			double d2=(inters2-line_pick2)*tmucs.axis_z;
			double dd1=min(d1,d2);
			Int3dlc(&inters1,&inters2,line_pick2,tmucs.axis_z,
				pTransTube->Start(),pTransTube->End(),trans_radius,trans_radius);
			d1=(inters1-line_pick1)*tmucs.axis_z;
			d2=(inters2-line_pick2)*tmucs.axis_z;
			double dd2=min(d1,d2);
			double dd=max(dd1,dd2);
			if(dd<startOdd())
				return dd;
			else
				return startOdd();
		}
		else
			return startOdd();
	}
	else
		return startOdd();
}

double CLDSLineTube::GetArea()	//单位为mm2
{
	return Pi*(GetDiameter()*size_thick-size_thick*size_thick);
}
int CLDSLineTube::GetSpecification(char *spec,BOOL bIncMat/*=TRUE*/)
{
	char steelmark[20]="";
	char tmp_spec[200],tmp_spec2[200];
	if(bIncMat&&cMaterial!=CSteelMatLibrary::Q235BriefMark())
		QuerySteelMatMark(cMaterial,steelmark);
	sprintf(tmp_spec,"%sΦ%f",steelmark,GetDiameter());
	SimplifiedNumString(tmp_spec);
	if (size_thick>0&&size_thick<GetDiameter()*0.5)
	{
		_snprintf(tmp_spec2,199,"%sx%f",tmp_spec,size_thick);
		SimplifiedNumString(tmp_spec2);
	}
	else
		strcpy(tmp_spec2,tmp_spec);
	if(spec)
		strcpy(spec,tmp_spec2);
	return strlen(tmp_spec2);
}
double CLDSLineTube::GetWeight(BOOL bSolidLength/*=TRUE*/)
{
	double theroy_weight=0;
	if(library)
		theroy_weight=library->GetWeightOf1Meter(size_wide,size_thick,0,size_idClassType);
	else
		theroy_weight=GetArea()*7.85e-3;
	double length;
	if(bSolidLength||pStart==NULL||pEnd==NULL)
		length=GetLength();
	else
		length=DISTANCE(pStart->Position(false),pEnd->Position(false));
	return length*0.001*theroy_weight;
}

double CLDSLineTube::GetEndActualOdd()
{
	f3dPoint axis_vec=End()-Start();
	if(m_tJointEnd.type==0&&(m_tJointEnd.cutPlaneNorm*axis_vec)!=0)
	{
		normalize(axis_vec);
		f3dPoint offset_vec=m_tJointEnd.cutPlaneNorm-(axis_vec*m_tJointEnd.cutPlaneNorm)*axis_vec;
		normalize(offset_vec);
		f3dPoint line_pick1=End()+offset_vec*GetDiameter()*0.5;
		f3dPoint line_pick2=End()-offset_vec*GetDiameter()*0.5;
		f3dPoint inters1,inters2;
		Int3dlf(inters1,line_pick1,axis_vec,m_tJointEnd.cutPlanePos,m_tJointEnd.cutPlaneNorm);
		Int3dlf(inters2,line_pick2,axis_vec,m_tJointEnd.cutPlanePos,m_tJointEnd.cutPlaneNorm);
		double dd1=(inters1-line_pick1)*axis_vec;
		double dd2=(inters2-line_pick2)*axis_vec;
		double dd=max(dd1,dd2);
		if(dd<endOdd())
			return dd;
		else
			return endOdd();
	}
	else if(m_tJointEnd.type==1&&m_tJointEnd.hLinkObj>0)
	{
		CLDSLineTube *pTransTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_tJointEnd.hLinkObj,CLS_LINETUBE);
		if(pTransTube&&pTransTube->GetDiameter()>GetDiameter())	//粗判
		{
			double radius=GetDiameter()*0.5,trans_radius=pTransTube->GetDiameter()*0.5;
			UCS_STRU tmucs;
			tmucs.origin=End();
			tmucs.axis_z=End()-Start();
			normalize(tmucs.axis_z);
			f3dPoint trans_axis=pTransTube->End()-pTransTube->Start();
			tmucs.axis_x=trans_axis^tmucs.axis_z;
			normalize(tmucs.axis_x);
			tmucs.axis_y=tmucs.axis_z^tmucs.axis_x;
			f3dPoint trans_pos=pTransTube->End();
			coord_trans(trans_pos,tmucs,FALSE);
			if(fabs(trans_pos.x)>(trans_radius-radius))
				return endOdd();
			f3dPoint line_pick1=tmucs.origin-tmucs.axis_x*radius;
			f3dPoint line_pick2=tmucs.origin+tmucs.axis_x*radius;
			f3dPoint inters1,inters2;
			Int3dlc(&inters1,&inters2,line_pick1,tmucs.axis_z,
				pTransTube->Start(),pTransTube->End(),trans_radius,trans_radius);
			double d1=(inters1-line_pick1)*tmucs.axis_z;
			double d2=(inters2-line_pick2)*tmucs.axis_z;
			double dd1=min(d1,d2);
			Int3dlc(&inters1,&inters2,line_pick2,tmucs.axis_z,
				pTransTube->Start(),pTransTube->End(),trans_radius,trans_radius);
			d1=(inters1-line_pick1)*tmucs.axis_z;
			d2=(inters2-line_pick2)*tmucs.axis_z;
			double dd2=min(d1,d2);
			double dd=max(dd1,dd2);
			if(dd<endOdd())
				return dd;
			else
				return endOdd();
		}
		else
			return startOdd();
	}
	else
		return endOdd();
}

//获取钢管端头参数化板 wht 10-11-30
CLDSParamPlate *CLDSLineTube::GetParamPlate(int start0_end1/*=0*/)
{
	CLDSParamPlate *pParamPlate=NULL;
	long hPlate=0;
	if(start0_end1==0)
		hPlate=m_tJointStart.hLinkObj;
	else if(start0_end1==1)
		hPlate=m_tJointEnd.hLinkObj;
	if(hPlate>0x20)
		pParamPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(hPlate,CLS_PARAMPLATE);
	return pParamPlate;
}
void CLDSLineTube::TransToCS(GECS& cs)
{
	baseline.CoordTrans(UCS_STRU(cs),false);
	if(m_iNormCalStyle==2)
	{
		GEPOINT pick=cs.TransPToCS(pickWorkPlane);
		pickWorkPlane[0]=pick.x;
		pickWorkPlane[1]=pick.y;
		pickWorkPlane[2]=pick.z;
	}
	SetModified();
}
void CLDSLineTube::Offset(const double* offset)
{
	GEPOINT offset3d(offset);
	CLDSLinePart::Offset(offset);
	//TODO:设计参数更新
}
bool CLDSLineTube::ShadowDesign()
{	
	if(!IsShadowObject())
		return false;
	CLDSLineTube *pMotherTube = (CLDSLineTube*)MotherObject(false);
	size=pMotherTube->size;
	cMaterial=pMotherTube->cMaterial;
	connectStart=pMotherTube->connectStart;
	connectEnd=pMotherTube->connectEnd;
	connectMid=pMotherTube->connectMid;
	start_oddment=pMotherTube->startOdd();
	end_oddment=pMotherTube->endOdd();
	//
	CalPosition();
	if(m_iShadowObjType==SHADOW_EMBEDDED||m_iShadowObjType==SHADOW_EXTERBLK)
	{
		CBlockParameter blockPara;
		if(!GetBlockParameter(blockPara))
			return false;
		UCS_STRU cs=blockPara.m_pBlock->TransToACS(blockPara.m_pBlockRef->GetACS());
		vector_trans(nearWeldRoadVec,cs,TRUE,TRUE);
		vector_trans(pickWorkPlane,cs,TRUE,TRUE);
		vector_trans(workPlaneNorm,cs,TRUE,TRUE);
		vector_trans(cutPlaneNorm,cs,TRUE,TRUE);
		coord_trans(cutPlaneOrigin,cs,TRUE,TRUE);
	}
	return true;
}
bool CLDSLineTube::IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr)
{
	if(pObj==NULL||propKeyStr==NULL||pObj->GetClassTypeId()!=m_nClassTypeId)
		return false;
	CLDSLineTube* pTube=(CLDSLineTube*)pObj;
	MEMORY_BLOCK cl=mpcl;
	if(cl.IsZero())
		cl=MPCL;
	MEMORY_BLOCK mir_cl=pTube->mpcl;
	if(mir_cl.IsZero())
		mir_cl=MPCL;
	PROPLIST_ITEM* pItem=propMirSync.GetPropItem(propKeyStr);
	if(pItem!=NULL)
		return cl.GetBitState((WORD)pItem->id-1)&&mir_cl.GetBitState((WORD)pItem->id-1);
	else
	{
		Log2File()->Log("%s同步对称属性项缺失",propKeyStr);
		return false;
	}
}
//从MirTaAtom.cpp文件的CreateSingleMirTubeTransectFace函数移植过来　wjh-2015.3.7
BOOL CLDSLineTube::SyncMirTransectFaceTo(CLDSLineTube* pDestMirTube,MIRMSG mirmsg)
{
	CLDSLineTube *pSrcLinkTube=NULL,*pSrcViceTube=NULL,*pLinkTube=NULL,*pViceTube=NULL;
	CLDSNode *pMirStartNode=NULL,*pMirEndNode=NULL;
	BOOL bStartToEnd=FALSE;	//对称钢管的终止节点对应源钢管的起始节点
	if(pDestMirTube==NULL)
		return FALSE;
	BOOL bStartTransect=FALSE,bEndTransect=FALSE;
	if(m_tJointStart.type==1&&m_tJointStart.hLinkObj)
		bStartTransect=TRUE;
	if(m_tJointEnd.type==1&&m_tJointEnd.hLinkObj)
		bEndTransect=TRUE;
	if(pDestMirTube->handle==handle)//对称钢管为本身
	{
		if(bStartTransect&&bEndTransect)//源钢管两端都已经存在钢管相贯 无法对称
			return FALSE;
		bStartToEnd=TRUE;
	}
	pMirStartNode=pStart->GetMirNode(mirmsg);
	pMirEndNode=pEnd->GetMirNode(mirmsg);
	if(!pMirStartNode||!pMirEndNode)
		return FALSE;
	if(pMirStartNode->handle==pDestMirTube->pEnd->handle)
		bStartToEnd=TRUE;
	if(pDestMirTube)
	{
		if(bStartToEnd)	//头对尾 尾对头
		{
			if(bStartTransect&&(pDestMirTube->m_tJointEnd.hLinkObj&&pDestMirTube->m_tJointEnd.type!=1))
				return FALSE;
			if(bEndTransect&&(pDestMirTube->m_tJointStart.hLinkObj&&pDestMirTube->m_tJointEnd.type!=1))
				return FALSE;
		}
		else	//头对头 尾对尾
		{
			if(bStartTransect&&(pDestMirTube->m_tJointStart.hLinkObj&&pDestMirTube->m_tJointEnd.type!=1))
				return FALSE;
			if(bEndTransect&&(pDestMirTube->m_tJointEnd.hLinkObj&&pDestMirTube->m_tJointEnd.type!=1))
				return FALSE;
		}
	}
	if(bStartTransect)
	{
		if(bStartToEnd)//对称钢管的终止节点对应源钢管的起始节点
			pDestMirTube->m_tJointEnd.type=m_tJointStart.type;
		else
			pDestMirTube->m_tJointStart.type=m_tJointStart.type;
		//对称hLinkObj
		pSrcLinkTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_tJointStart.hLinkObj,CLS_LINETUBE);
		if(pSrcLinkTube)
		{
			pLinkTube=(CLDSLineTube*)pSrcLinkTube->GetMirPart(mirmsg);
			if(!pLinkTube)
				return FALSE;
			if(bStartToEnd)//对称钢管的终止节点对应源钢管的起始节点
				pDestMirTube->m_tJointEnd.hLinkObj=pLinkTube->handle;
			else
				pDestMirTube->m_tJointStart.hLinkObj=pLinkTube->handle;
		}
		else
		{
			if(bStartToEnd)//对称钢管的终止节点对应源钢管的起始节点
				pDestMirTube->m_tJointEnd.hLinkObj=0;
			else
				pDestMirTube->m_tJointStart.hLinkObj=0;
		}
		//对称hViceLinkObj
		if(m_tJointStart.hViceLinkObj>0)	//单侧双管相贯
		{
			pSrcViceTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_tJointStart.hViceLinkObj,CLS_LINETUBE);
			if(pSrcViceTube)
			{
				pViceTube=(CLDSLineTube*)pSrcViceTube->GetMirPart(mirmsg);
				if(!pViceTube)
					return FALSE;
				if(bStartToEnd)//对称钢管的终止节点对应源钢管的起始节点
					pDestMirTube->m_tJointEnd.hViceLinkObj=pViceTube->handle;
				else
					pDestMirTube->m_tJointStart.hViceLinkObj=pViceTube->handle;
			}
		}
		else //对接相贯或普通相贯
		{
			if(bStartToEnd)//对称钢管的终止节点对应源钢管的起始节点
				pDestMirTube->m_tJointEnd.hViceLinkObj=m_tJointStart.hViceLinkObj;
			else
				pDestMirTube->m_tJointStart.hViceLinkObj=m_tJointStart.hViceLinkObj;
			if(m_tJointStart.hViceLinkObj<0)
			{	//对接相贯
				if((bStartToEnd&&pLinkTube->pStart->handle==pDestMirTube->pEnd->handle)||(!bStartToEnd&&pLinkTube->pStart->handle==pDestMirTube->pStart->handle))
				{	//始端对接相贯
					pLinkTube->m_tJointStart.type=1;
					pLinkTube->m_tJointStart.hLinkObj=pDestMirTube->handle;
					pLinkTube->m_tJointStart.hViceLinkObj=-1;
				}
				else if((bStartToEnd&&pLinkTube->pEnd->handle==pDestMirTube->pEnd->handle)||(!bStartToEnd&&pLinkTube->pEnd->handle==pDestMirTube->pStart->handle))
				{	//终端对接相贯
					pLinkTube->m_tJointEnd.type=1;
					pLinkTube->m_tJointEnd.hLinkObj=pDestMirTube->handle;
					pLinkTube->m_tJointEnd.hViceLinkObj=-1;
				}
				pLinkTube->m_hPartWeldParent=pDestMirTube->handle;
				pLinkTube->SetModified();
				pLinkTube->Create3dSolidModel();
				//g_pSolidDraw->NewSolidPart(pLinkTube->GetSolidPartObject());
			}
		}
		pDestMirTube->m_hPartWeldParent=pLinkTube->handle;
	}
	//没切割参数时同样应该更新对称关联对象的属性 wjh-2015.8.5
	else if(m_tJointStart.type==0)// && !m_tJointStart.cutPlaneNorm.IsZero())
	{	//始端平面切割
		f3dPoint mir_pos=GetMirPos(m_tJointStart.cutPlanePos,mirmsg);
		f3dPoint mir_norm=GetMirVector(m_tJointStart.cutPlaneNorm,mirmsg);
		if(bStartToEnd)//对称钢管的终止节点对应源钢管的起始节点
		{
			pDestMirTube->m_tJointEnd.type=m_tJointStart.type;
			pDestMirTube->m_tJointEnd.cutPlanePos=mir_pos;
			pDestMirTube->m_tJointEnd.cutPlaneNorm=mir_norm;
		}
		else
		{	
			pDestMirTube->m_tJointStart.type=m_tJointStart.type;
			pDestMirTube->m_tJointStart.cutPlanePos=mir_pos;
			pDestMirTube->m_tJointStart.cutPlaneNorm=mir_norm;
		}
	}
	if(bEndTransect)
	{
		pSrcLinkTube=pSrcViceTube=pLinkTube=pViceTube=NULL;	//清空临时变量
		if(bStartToEnd)//对称钢管的终止节点对应源钢管的起始节点
			pDestMirTube->m_tJointStart.type=m_tJointEnd.type;
		else
			pDestMirTube->m_tJointEnd.type=m_tJointEnd.type;
		//对称hLinkObj
		pSrcLinkTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_tJointEnd.hLinkObj,CLS_LINETUBE);
		if(pSrcLinkTube)
		{
			pLinkTube=(CLDSLineTube*)pSrcLinkTube->GetMirPart(mirmsg);
			if(!pLinkTube)
				return FALSE;
			if(bStartToEnd)//对称钢管的终止节点对应源钢管的起始节点
				pDestMirTube->m_tJointStart.hLinkObj=pLinkTube->handle;
			else
				pDestMirTube->m_tJointEnd.hLinkObj=pLinkTube->handle;
		}
		else
		{
			if(bStartToEnd)//对称钢管的终止节点对应源钢管的起始节点
				pDestMirTube->m_tJointEnd.hLinkObj=pLinkTube->handle;
			else
				pDestMirTube->m_tJointStart.hLinkObj=pLinkTube->handle;
		}
		//对称hViceLinkObj
		if(m_tJointEnd.hViceLinkObj>0)	//单侧双管相贯
		{
			pSrcViceTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_tJointEnd.hViceLinkObj,CLS_LINETUBE);
			if(pSrcViceTube)
			{
				pViceTube=(CLDSLineTube*)pSrcViceTube->GetMirPart(mirmsg);
				if(!pViceTube)
					return FALSE;
				if(bStartToEnd)//对称钢管的终止节点对应源钢管的起始节点
					pDestMirTube->m_tJointStart.hViceLinkObj=pViceTube->handle;
				else
					pDestMirTube->m_tJointEnd.hViceLinkObj=pViceTube->handle;
			}
		}
		else //对接相贯或普通相贯
		{
			if(bStartToEnd)//对称钢管的终止节点对应源钢管的起始节点
				pDestMirTube->m_tJointStart.hViceLinkObj=m_tJointEnd.hViceLinkObj;
			else
				pDestMirTube->m_tJointEnd.hViceLinkObj=m_tJointEnd.hViceLinkObj;
			if(m_tJointEnd.hViceLinkObj<0)
			{	//对接相贯
				if((bStartToEnd&&pLinkTube->pStart->handle==pDestMirTube->pStart->handle)||(!bStartToEnd&&pLinkTube->pStart->handle==pDestMirTube->pEnd->handle))
				{	//始端对接相贯
					pLinkTube->m_tJointStart.type=1;
					pLinkTube->m_tJointStart.hLinkObj=pDestMirTube->handle;
					pLinkTube->m_tJointStart.hViceLinkObj=-1;
				}
				else if((bStartToEnd&&pLinkTube->pEnd->handle==pDestMirTube->pStart->handle)||(!bStartToEnd&&pLinkTube->pEnd->handle==pDestMirTube->pEnd->handle))
				{	//终端对接相贯
					pLinkTube->m_tJointEnd.type=1;
					pLinkTube->m_tJointEnd.hLinkObj=pDestMirTube->handle;
					pLinkTube->m_tJointEnd.hViceLinkObj=-1;
				}
				pLinkTube->m_hPartWeldParent=pDestMirTube->handle;
				pLinkTube->SetModified();
				pLinkTube->Create3dSolidModel();
				//g_pSolidDraw->NewSolidPart(pLinkTube->GetSolidPartObject());
			}
		}
		pDestMirTube->m_hPartWeldParent=pLinkTube->handle;
	}
	//没切割参数时同样应该更新对称关联对象的属性 wjh-2015.8.5
	else if(m_tJointEnd.type==0)// && !m_tJointEnd.cutPlaneNorm.IsZero())
	{	//平面切割情况
		f3dPoint mir_pos=GetMirPos(m_tJointEnd.cutPlanePos,mirmsg);
		f3dPoint mir_norm=GetMirVector(m_tJointEnd.cutPlaneNorm,mirmsg);
		if(bStartToEnd)//对称钢管的终止节点对应源钢管的起始节点
		{
			pDestMirTube->m_tJointStart.type=m_tJointEnd.type;
			pDestMirTube->m_tJointStart.cutPlanePos=mir_pos;
			pDestMirTube->m_tJointStart.cutPlaneNorm=mir_norm;
		}
		else
		{	
			pDestMirTube->m_tJointEnd.type=m_tJointEnd.type;
			pDestMirTube->m_tJointEnd.cutPlanePos=mir_pos;
			pDestMirTube->m_tJointEnd.cutPlaneNorm=mir_norm;
		}
	}
	pDestMirTube->SetModified();
	pDestMirTube->Create3dSolidModel();
	//g_pSolidDraw->NewSolidPart(pMirTube->GetSolidPartObject());
	//g_pSolidDraw->Draw();
	return TRUE;
}

bool CLDSLineTube::SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)
{
	if(pDestObj==NULL||pDestObj->GetClassTypeId()!=m_nClassTypeId)
		return false;
	CLDSLineTube* pTube=(CLDSLineTube*)pDestObj;
	if((propKeyStr==NULL||strcmp(propKeyStr,"iSeg")==0)&&IsSyncMirPropWith(pDestObj,"iSeg"))
	{	//分段号
		pTube->iSeg=iSeg;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cMaterial")==0)&&IsSyncMirPropWith(pDestObj,"cMaterial"))
	{	//分段号
		pTube->cMaterial=cMaterial;
		pTube->cbVirginQualityLevel=cbVirginQualityLevel;
		pTube->SetModified(TRUE,FALSE);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"specification")==0)&&IsSyncMirPropWith(pDestObj,"specification"))
	{	//分段号
		pTube->size=size;
		pTube->SetModified(TRUE,TRUE);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cfgword")==0)&&IsSyncMirPropWith(pDestObj,"cfgword"))
	{	//配材号
		pTube->cfgword=cfgword;
	}
	if((propKeyStr==NULL ||strcmp(propKeyStr,"oddStart")==0)&&IsSyncMirPropWith(pDestObj,"oddStart"))
	{	//始端正负头
		pTube->SetStartOdd(startOdd());
	}
	if((propKeyStr==NULL ||strcmp(propKeyStr,"oddEnd")==0)&&IsSyncMirPropWith(pDestObj,"oddEnd"))
	{	//终端正负头
		pTube->SetEndOdd(endOdd());
	}
	return true;
}
bool CLDSLineTube::SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)
{
	if(pDestObj==NULL||pDestObj->GetClassTypeId()!=m_nClassTypeId)
		return false;
	CLDSLineTube* pTube=(CLDSLineTube*)pDestObj;
#ifdef __PART_DESIGN_INC_
	pTube->m_cbRodFlag=m_cbRodFlag;
#endif
	if((propKeyStr==NULL||strcmp(propKeyStr,"layer")==0)&&IsSyncMirPropWith(pDestObj,"layer"))
	{	//图层名同步对称
		MirObjLayer(layer(),pTube->layer(),mirmsg);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"iSeg")==0)&&IsSyncMirPropWith(pDestObj,"iSeg"))
	{	//分段号
		pTube->iSeg=iSeg;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cMaterial")==0)&&IsSyncMirPropWith(pDestObj,"cMaterial"))
	{	//分段号
		pTube->cMaterial=cMaterial;
		pTube->cbVirginQualityLevel=cbVirginQualityLevel;
		pTube->SetModified(TRUE,FALSE);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"specification")==0)&&IsSyncMirPropWith(pDestObj,"specification"))
	{	//分段号
		pTube->size=size;
		pTube->SetModified(TRUE,TRUE);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cfgword")==0)&&IsSyncMirPropWith(pDestObj,"cfgword"))
	{	//配材号
		pTube->cfgword=cfgword;
	}
	if((propKeyStr==NULL ||strcmp(propKeyStr,"oddStart")==0)&&IsSyncMirPropWith(pDestObj,"oddStart"))
	{	//始端正负头
		pTube->SetStartOdd(startOdd());
	}
	if((propKeyStr==NULL ||strcmp(propKeyStr,"oddEnd")==0)&&IsSyncMirPropWith(pDestObj,"oddEnd"))
	{	//终端正负头
		pTube->SetEndOdd(endOdd());
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"TransectFace")==0)&&IsSyncMirPropWith(pDestObj,"TransectFace"))
	{	//相贯信息
		if(!SyncMirTransectFaceTo(pTube,mirmsg))
			return false;
	}
	return true;
}
bool CLDSLineTube::SyncMirProp(const char* propKeyStr/*=NULL*/,BYTE ciMirAll0X1Y2Z3/*=0*/,DWORD dwPropFilterFlag/*=0xffffffff*/)//propKeyStr表示同步对称全部属性
{
	MIRMSG xSpecMirInfo(ciMirAll0X1Y2Z3);
	for(RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		if(ciMirAll0X1Y2Z3!=0&&!pRelaObj->mirInfo.IsSameMirMsg(xSpecMirInfo))
			continue;
		CLDSPart* pPart=BelongModel()->FromPartHandle(pRelaObj->hObj);
		SyncMirPropTo(pPart,pRelaObj->mirInfo,propKeyStr,dwPropFilterFlag);
	}
	return true;
}

#ifdef __LDS_CONTEXT_
bool CLDSLineTube::CloneToBomPart(BOMPART* pBomPart)
{	
	if(pBomPart->cPartType!=BOMPART::TUBE)
		return false;
	PART_TUBE *pBomTube=(PART_TUBE*)pBomPart;
	CLDSPart::CloneToBomPart(pBomPart);	//克隆基本属性
	//克隆钢管特有属性
	//焊接父构件
	CLDSPart *pFatherPart=m_pModel->FromPartHandle(m_hPartWeldParent);
	BOOL bWeldPart=pFatherPart?TRUE:FALSE;
	BOOL bHasNodePlate=FALSE,bHasBranchTube=FALSE,bHasFootNail=FALSE;
	BOOL bPush=m_pModel->PushPartStack();
	for(CLDSPart *pSonPart=m_pModel->EnumPartFirst();pSonPart;pSonPart=m_pModel->EnumPartNext())
	{
		if(!bWeldPart&&pSonPart->m_hPartWeldParent==handle)
			bWeldPart=TRUE;
		if(!bHasBranchTube&&pSonPart->GetClassTypeId()==CLS_LINETUBE)
		{
			CLDSLineTube *pTempTube=(CLDSLineTube*)pSonPart;
			if((pTempTube->m_tJointStart.type==1&&(pTempTube->m_tJointStart.hLinkObj==handle
				||pTempTube->m_tJointStart.hViceLinkObj==handle))
				||(pTempTube->m_tJointEnd.type==1&&(pTempTube->m_tJointEnd.hLinkObj==handle
				||pTempTube->m_tJointEnd.hViceLinkObj==handle)))
				bHasBranchTube=TRUE;
		}
		if(!bHasNodePlate&&pSonPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate *pTempPlate=(CLDSPlate*)pSonPart;
			if(pTempPlate->m_hPartWeldParent!=handle
				||pTempPlate->designInfo.m_hBasePart!=handle)
				continue;
			CLDSNode *pBaseNode=m_pModel->FromNodeHandle(pTempPlate->designInfo.m_hBaseNode);
			if(pBaseNode==NULL)
				continue;
			if(pBaseNode->handle==pStart->handle
				||pBaseNode->handle==pEnd->handle
				||pBaseNode->hFatherPart==handle)
				bHasNodePlate=TRUE;
		}
		if(!bHasFootNail&&(pSonPart->GetClassTypeId()==CLS_LINESLOT||pSonPart->GetClassTypeId()==CLS_LINEANGLE))
		{
			if(pSonPart->m_hPartWeldParent!=handle)
				continue;
			if((pSonPart->GetClassTypeId()==CLS_LINESLOT&&((CLDSLineSlot*)pSonPart)->IsBaseSlot())||
				(pSonPart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pSonPart)->m_bFootNail))
				bHasFootNail=TRUE;
		}
		if(bHasNodePlate&&bHasBranchTube)
			break;
	}
	if(bPush)
		m_pModel->PopPartStack();
	//存在合法焊接父构件或焊接属性为TRUE
	if(pFatherPart)
		pBomTube->siSubType=BOMPART::SUB_TYPE_TUBE_BRANCH;	//支管
	else 
		pBomTube->siSubType=BOMPART::SUB_TYPE_TUBE_MAIN;	//主管
	pBomTube->bHasNodePlate=bHasNodePlate;		//连接有节点板
	pBomTube->bHasBranchTube=bHasBranchTube;	//连接有支管
	pBomTube->bHasFootNail=bHasFootNail;		//含有脚钉
	pBomTube->bWeldPart=bWeldPart;				//焊接件
	//始终端工艺信息
	TUBEJOINT *pTubeJoint=NULL;
	PART_TUBE::TUBE_PROCESS *pTubeProcess=NULL;
	CLDSParamPlate *pParamPlate=NULL;
	for(int i=0;i<2;i++)
	{
		pTubeJoint=(i==0)?&m_tJointStart:&m_tJointEnd;
		pTubeProcess=(i==0)?&pBomTube->startProcess:&pBomTube->endProcess;
		pParamPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(pTubeJoint->hLinkObj,CLS_PARAMPLATE);
		if(pParamPlate==NULL)
			continue;
		//统计钢管始\终端加工工艺
		if((pTubeJoint->type==0&&!pTubeJoint->cutPlaneNorm.IsZero())						//平面切割
			||(pTubeJoint->type==1&&pTubeJoint->hViceLinkObj<0&&pTubeJoint->hLinkObj>0x20))	//对接相贯
		{	//平面相贯
			f3dPoint tube_vec=End()-Start();
			normalize(tube_vec);
			double cosa=fabs(tube_vec*pTubeJoint->cutPlaneNorm);
			pTubeProcess->type=PART_TUBE::TUBE_PROCESS::PROCESSTYPE_PLANECUT;		//平面相贯
			pTubeProcess->L=GetLength();			//支管长度
			pTubeProcess->A=90.0-ACOS(cosa)*DEGTORAD_COEF;	//钢管心线与切割面之间的夹角
		}
		else if(pTubeJoint->type==1&&pTubeJoint->hLinkObj>0x20&&pTubeJoint->hViceLinkObj>=0)	
		{	//钢管(圆柱)相贯
			CLDSLineTube *pMainTube=(CLDSLineTube*)BelongModel()->FromPartHandle(pTubeJoint->hLinkObj,CLS_LINETUBE);
			if(pMainTube)
			{
				f3dPoint tube_vec=End()-Start();
				f3dPoint main_tube_vec=pMainTube->End()-pMainTube->Start();
				normalize(tube_vec);
				normalize(main_tube_vec);
				double cosa=fabs(tube_vec*main_tube_vec);
				pTubeProcess->type=PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TRANSECT;	//圆柱相贯
				pTubeProcess->L=GetLength();		//支管长度
				pTubeProcess->A=ACOS(cosa)*DEGTORAD_COEF;	//支管与主管之间的夹角
				pMainTube->GetSpecification(pTubeProcess->sSpec,FALSE);//主管规格
			}
		}
		else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_UEND) 
		{	//U型开口槽
			double M=0,W=0,thick=0,fNormOffset=0;
			pParamPlate->GetDesignItemValue('M',&M);
			pParamPlate->GetDesignItemValue('W',&W);
			thick=pParamPlate->GetThick();
			fNormOffset=pParamPlate->m_fNormOffset;
			pTubeProcess->type=PART_TUBE::TUBE_PROCESS::PROCESSTYPE_USLOT;	//U型开口槽
			pTubeProcess->L=M;						//开口槽长度L
			pTubeProcess->A=W+2*thick;				//开口槽宽度A
			pTubeProcess->B=0.5*W+thick;			//开口槽内圆弧半径B
			pTubeProcess->fNormOffset=fNormOffset;	//开口槽法向偏移量
		}
		else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_ROLLEND) 
		{	//一字型开口槽
			double M=0,thick=0,fNormOffset=0;
			pParamPlate->GetDesignItemValue('M',&M);
			thick=pParamPlate->GetThick();
			fNormOffset=pParamPlate->m_fNormOffset;
			pTubeProcess->type=PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT;	//一字型开口槽
			pTubeProcess->L=M;						//开口槽长度L
			pTubeProcess->A=thick;					//开口槽宽度A
			pTubeProcess->fNormOffset=fNormOffset;	//开口槽法向偏移量
		}
		else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND) 
		{	//十字型开口槽
			double M=0,thick=0,fNormOffset=0;
			pParamPlate->GetDesignItemValue('M',&M);
			thick=pParamPlate->GetThick();
			fNormOffset=pParamPlate->m_fNormOffset;
			pTubeProcess->type=PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT;	//十字型开口槽
			pTubeProcess->L=M;						//开口槽长度L
			pTubeProcess->A=thick;					//开口槽宽度A
			pTubeProcess->fNormOffset=fNormOffset;	//开口槽法向偏移量
		}
		else if(pParamPlate&&pParamPlate->IsFL())
		{
			if(pParamPlate->m_iParamType==TYPE_FLD)
				pTubeProcess->type=PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD;
			else if(pParamPlate->m_iParamType==TYPE_FLP)
				pTubeProcess->type=PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLP;
			else if(pParamPlate->m_iParamType==TYPE_FLG)
				pTubeProcess->type=PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLG;
			else if(pParamPlate->m_iParamType==TYPE_FLR)
				pTubeProcess->type=PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLR;
			else //if(pStartParamPlate->m_iParamType==TYPE_FL)
				pTubeProcess->type=PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FL;
		}
		else
			pTubeProcess->type=PART_TUBE::TUBE_PROCESS::PROCESSTYPE_NONE; //无加工工艺
	}
	return true;
}
#endif
#endif