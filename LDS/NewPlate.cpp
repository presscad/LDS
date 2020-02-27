//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "CmdLineDlg.h"
#include "env_def.h"
#include "CoordDefDlg.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "DesignJinBanDlg.h"
#include "CircularPlateDlg.h"
#include "AngleProfileParaDlg.h"
#include "AddNewElbowPlateDlg.h"
#include "AddCircularPlateDlg.h"
#include "AddCircularCoverPlateDlg.h"
#include "RibPlateDesignDlg.h"
#include "DesignPlateDlg.h"
#include "DesignJdb.h"
#include "DesignGyHoleDlg.h"
#include "DesignSphereDlg.h"
#include "dlg.h"
#include "MirTaAtom.h"
#include "DesignTubeRibPlateDlg.h"
#include "DesignArcPartDlg.h"
#include "XInsertPlateIntersTubeDlg.h"
#include "DefFaceDlg.h"
#include "WaterShieldedPlateDlg.h"
#include "DesignConnectionPlateDlg.h"
#include "DesignOverlappendPlateDlg.h"
#include "CutPlateOptionDlg.h"
#include "DesignTubeBetweenPlateDlg.h"
#include "XhCharString.h"
#include "DesignTubeEndLjDlg.h"
#include "DesignSpecialFlDlg.h"
#include "MakePlateBendDlg.h"
#include "BendPlateByAngleDlg.h"
#include "DesignRibbedPlateDlg.h"
#include "DesignCircularPlateDlg.h"
#include "SortFunc.h"
#include "LogFile.h"
#include "SnapTypeVerify.h"
#include "LicFuncDef.h"
#include "btc.h"
#include "DesLineAngleRibPlateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
//extern BOOL g_bRedrawSolid;
void CLDSView::OnDesignLjPlate() 
{
	m_nPrevCommandID=ID_DESIGN_LJ_PLATE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName.Format("%s","Repeat connecting plate");
#else
	m_sPrevCommandName.Format("%s","重复连接板");
#endif
	Command("ConnectionPlate");
}
struct RODBOLTINFO{
	CLDSBolt* pBolt;
	CLDSPart* pPart;
	CLDSLinePart* pRod;
	RODBOLTINFO(){
		pBolt=NULL;
		pPart=pRod=NULL;
	}
};
struct CONN_NODEINFO{
	int hits;
	CLDSNode* pNode;
	CONN_NODEINFO(CLDSNode* pConnNode=NULL,int iConnHits=1)
	{
		hits=iConnHits;
		pNode=pConnNode;
	}
};
struct _LOCAL_CONNECT_PLANE{
	bool bTwoWorkPlane;
	bool m_bSelfCorrectBendline;
	CLDSVector designnorm,designbendnorm;
	CLDSPoint huoqustart,huoquend;
	CParaThick xBaseNormOffset,xBendNormOffset;
	GEPOINT vBasePlanePick,vBasePlaneNorm,vBendPlanePick,vBendPlaneNorm,vBendLineVec;
	_LOCAL_CONNECT_PLANE(){bTwoWorkPlane=false;m_bSelfCorrectBendline=true;}
};

void FindBoltRelatedParts(const CLDSBolt *pBolt,PARTSET &partSet);	//From BoltThroughThickDlg.cpp
BOOL RetrieveConnectPlateInfoByBolts(CHashSet<CLDSBolt*>& boltset,
	CLDSNode*& pBaseNode,CLDSLinePart*& pBaseRod,CHashSet<CLDSLinePart*>& rodset,_LOCAL_CONNECT_PLANE& workplane)
{
	CHashList<RODBOLTINFO> hashRodBolts;
	CHashList<CONN_NODEINFO> hashNodeConnHits;
	CHashList<CPtInLineCheck> hashLineChecks;
	LINEPARTSET boltRelatedRodSet;
	PARTSET partSet;
	//1.根据螺栓集合提取连接板所连接杆件的集合
	for(CLDSBolt* pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
	{
		RODBOLTINFO info;
		info.pBolt=pBolt;
		info.pPart=pBolt->BelongModel()->FromPartHandle(pBolt->des_base_pos.hPart);
		if(info.pPart&&info.pPart->IsLinePart())
			info.pRod=(CLDSLinePart*)info.pPart;
#ifdef __COMPLEX_PART_INC_
		else if(info.pPart&&info.pPart->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)info.pPart)->IsInsertPlate())
		{
			info.pRod=(CLDSLineTube*)info.pPart->BelongModel()->FromPartHandle(
				((CLDSParamPlate*)info.pPart)->m_hPartWeldParent,CLS_LINETUBE);
		}
#endif
		boltRelatedRodSet.Empty();
		if(info.pRod==NULL)
		{
			partSet.Empty();
			FindBoltRelatedParts(pBolt,partSet);
			for(CLDSPart *pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
			{
				if(pPart->IsLinePart())
					boltRelatedRodSet.append((CLDSLinePart*)pPart);
			}
		}
		else 
			boltRelatedRodSet.append(info.pRod);
		if(boltRelatedRodSet.GetNodeNum()<=0)
			continue;
		hashRodBolts.SetValue(pBolt->handle,info);
		for(CLDSLinePart *pRod=boltRelatedRodSet.GetFirst();pRod;pRod=boltRelatedRodSet.GetNext())
		{
			if(rodset.GetValue(pRod->handle)==NULL)
			{
				rodset.SetValue(pRod->handle,pRod);
				if(pRod->pStart)
				{
					CONN_NODEINFO *pHitsStart=hashNodeConnHits.GetValue(pRod->pStart->handle);
					if(pHitsStart)
						pHitsStart->hits++;
					else
						pHitsStart=hashNodeConnHits.SetValue(pRod->pStart->handle,CONN_NODEINFO(pRod->pStart));
				}
				if(pRod->pEnd)
				{
					CONN_NODEINFO *pHitsEnd=hashNodeConnHits.GetValue(pRod->pEnd->handle);
					if(pHitsEnd)
						pHitsEnd->hits++;
					else
						pHitsEnd=hashNodeConnHits.SetValue(pRod->pEnd->handle,CONN_NODEINFO(pRod->pEnd));
				}
				if(pRod->pStart!=NULL&&pRod->pEnd!=NULL)
					hashLineChecks.SetValue(pRod->handle,CPtInLineCheck(pRod->pStart->Position(false),pRod->pEnd->Position(false)));
			}
		}
	}
	//2.智能识别出最恰当的杆件作为钢板的定位基准杆件
	CONN_NODEINFO* pConnNode,*pMaxHitsConnNode=NULL;
	bool stronglyAdviseBaseRod=false;	//连接点位于基准杆件中间点时，强烈建议以此杆件为钢板的定位基准杆件
	for(pConnNode=hashNodeConnHits.GetNext();pConnNode;pConnNode=hashNodeConnHits.GetNext())
	{
		for(CPtInLineCheck* pLineCheck=hashLineChecks.GetFirst();pLineCheck;pLineCheck=hashLineChecks.GetNext())
		{
			double justify=pLineCheck->CheckPoint(pConnNode->pNode->Position(false));
			if(justify>EPS&&justify<1-EPS)
			{	//当前节点位于某一连接杆件的中间节点
				pConnNode->hits++;
				if(pBaseRod==NULL)
					pBaseRod=rodset.GetValue(hashLineChecks.GetCursorKey());
				stronglyAdviseBaseRod=true;
			}
		}
	}
	CLDSLinePart* pRod;
	for(pRod=rodset.GetFirst();pRod;pRod=rodset.GetNext())
	{
		pRod->feature=1;//默认均为单面板基准面连接杆件   
		CLDSLineAngle* pAngle=pRod->IsAngle()?(CLDSLineAngle*)pRod:NULL;
		//CLDSLineTube* pTube=pRod->GetClassTypeId()==CLS_LINETUBE?(CLDSLineTube*)pRod:NULL;
		if(pAngle!=NULL&&pAngle->pStart!=NULL&&pAngle->pEnd!=NULL&&
			(toupper(pAngle->Layer(1))=='Z'||toupper(pAngle->Layer(1))=='H'||//TODO:什么情况增加了'H',应单独提出一函数进行智能判断，以后应尽量减少对图层名的依赖
			pAngle->desStartPos.jgber_cal_style==1||pAngle->desEndPos.jgber_cal_style==1||	//优先指定角钢楞点坐标
			pAngle->desStartPos.spatialOperationStyle==0||pAngle->desStartPos.spatialOperationStyle>14))
		{
			if(pBaseRod==NULL||pRod->GetWidth()>pBaseRod->GetWidth())
				pBaseRod=pAngle;	//取肢宽较大角钢作为基准角钢
		}
	}
	for(pConnNode=hashNodeConnHits.GetFirst();pConnNode;pConnNode=hashNodeConnHits.GetNext())
	{
		if(pMaxHitsConnNode==NULL || pConnNode->hits>pMaxHitsConnNode->hits)
			pMaxHitsConnNode=pConnNode;
	}
	if(pMaxHitsConnNode==NULL&&rodset.GetNodeNum()<=0)	
		return FALSE;	//存在多根关联杆件但未找到合适连接节点时无法设计钢板 wht 16-11-09
	else if(pMaxHitsConnNode&&pMaxHitsConnNode->hits>1)
		pBaseNode=pMaxHitsConnNode->pNode;
	else if(pBaseRod==NULL)
		pBaseRod=rodset.GetFirst();
	if(pBaseNode&&pBaseRod==NULL)
		pBaseRod=rodset.GetValue(pBaseNode->hFatherPart);
	if(pBaseRod==NULL)
	{
		for(pRod=rodset.GetFirst();pRod;pRod=rodset.GetNext())
		{
			if(pBaseRod==NULL)
				pBaseRod=pRod;
			else if(pBaseNode&&(pRod->pStart==pBaseNode||pRod->pEnd==pBaseNode))
			{
				pBaseRod=pRod;
				break;
			}
			else if(pRod->GetWidth()>pBaseRod->GetWidth())
				pBaseRod=pRod;
		}
	}
	//3.定位连接平面
	if(pBaseRod)
	{
		CLDSLineAngle* pBaseAngle=pBaseRod->IsAngle()?(CLDSLineAngle*)pBaseRod:NULL;
		CLDSLineAngle *pBendPlaneBaseAngle=NULL;
		CLDSBolt *pBaseBolt1=NULL,*pBaseBolt2=NULL;
		if(pBaseAngle==NULL)
			workplane.bTwoWorkPlane=false;
		else
		{
			//3.1识别基准面法线
			for(pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
			{
				if(pBolt->des_base_pos.hPart==pBaseAngle->handle||pBaseAngle->FindLsByHandle(pBolt->handle))
				{
					workplane.designnorm.norm_style=1;
					workplane.designnorm.hVicePart=pBaseAngle->handle;
					if(fabs(pBaseAngle->vxWingNorm*pBolt->ucs.axis_z)>fabs(pBaseAngle->vyWingNorm*pBolt->ucs.axis_z))
					{
						workplane.designnorm.norm_wing=0;	//X肢为基准
						workplane.vBasePlaneNorm=pBaseAngle->vxWingNorm;
					}
					else
					{
						workplane.designnorm.norm_wing=1;	//Y肢为基准
						workplane.vBasePlaneNorm=pBaseAngle->vyWingNorm;
					}
					pBolt->des_base_pos.datumPoint.UpdatePos(pBaseAngle->BelongModel());
					workplane.vBasePlanePick=pBolt->des_base_pos.datumPoint.Position();
					pBaseBolt1=pBolt;
					break;
				}
			}
			//3.2识别制弯面法线
			if(workplane.vBasePlaneNorm.IsZero())
				return FALSE;
			double fCurrBendPlaneBaseAngleLength=0;
			for(pRod=rodset.GetFirst();pRod;pRod=rodset.GetNext())
			{
				if(pRod==pBaseAngle)
					continue;	//基准角钢
				double bendTolerance=max(50,pRod->size_wide/2);	//根据工程实践火曲容差给定固死值易判断失误，故改为与角钢肢宽相关 wjh-2016.3.16
				GEPOINT lenvec=pRod->End()-pRod->Start();
				double len=lenvec.mod();
				normalize(lenvec);
				double cosa=fabs(lenvec*workplane.vBasePlaneNorm);
				if(cosa*len<bendTolerance)
				{
					pRod->feature=1;
					if(cosa<EPS2)
					{	//杆件方向与基准面法线垂直,并且任何一肢法线方向与基准面法线方向均不一致
						if(pRod->IsAngle())
						{	//根据螺栓确定角钢当前工作肢
							CLDSLineAngle *pAngle=(CLDSLineAngle*)pRod;
							GEPOINT vWingNorm;
							CLsRef *pLsRef=NULL;
							for(pLsRef=pAngle->GetFirstLsRef();pLsRef;pLsRef=pAngle->GetNextLsRef())
							{
								if(boltset.GetValue(pLsRef->GetLsPtr()->handle))
								{
									if(fabs((*pLsRef)->get_norm()*pAngle->vxWingNorm)>fabs((*pLsRef)->get_norm()*pAngle->vyWingNorm))
										vWingNorm=pAngle->vxWingNorm;
									else
										vWingNorm=pAngle->vyWingNorm;
									break;
								}
							}
							//找到关联螺栓且工作肢法线与基准面法线不平行
							//EPS_COS2误差值太大,制弯度数较小时无法准确判断杆件所在面 wht 16-11-10
							const double cosa=0.99996192306417128873735516482698;	//0.5度
							if( pLsRef&&fabs(vWingNorm*workplane.vBasePlaneNorm)<cosa)
								pRod->feature=2;
						}
						//else if(pRod->IsTube()){} 暂时未处理射线杆件为钢管的情况
					}
					if(pRod->feature==1)
						continue;	//基准面内连接杆件
				}
				pRod->feature=2;//当前杆件为制弯面内的连接杆件
				double fCurrAngleLength=pRod->IsAngle()?pRod->GetLength():0;
				if(!pRod->IsAngle()||fCurrAngleLength<=fCurrBendPlaneBaseAngleLength)//workplane.vBendPlaneNorm.IsZero())
					continue;	//不是更恰当的制弯面角钢
				pBendPlaneBaseAngle=(CLDSLineAngle*)pRod;	//当前角钢更适合作为制弯面的基准角钢
				pBaseBolt2=NULL;//制弯角钢变更之后应重新查找基准螺栓
				fCurrBendPlaneBaseAngleLength=fCurrAngleLength;
				workplane.designbendnorm.norm_style=1;
				workplane.designbendnorm.hVicePart=pBendPlaneBaseAngle->handle;
				for(pBolt=boltset.GetFirst();pBaseBolt2==NULL&&pBolt;pBolt=boltset.GetNext())
				{
					if(pBolt->des_base_pos.hPart!=pBendPlaneBaseAngle->handle&&!pBendPlaneBaseAngle->FindLsByHandle(pBolt->handle))
						continue;
					if(fabs(pBendPlaneBaseAngle->vxWingNorm*pBolt->ucs.axis_z)>fabs(pBendPlaneBaseAngle->vyWingNorm*pBolt->ucs.axis_z))
					{
						workplane.vBendPlaneNorm=pBendPlaneBaseAngle->vxWingNorm;
						workplane.designbendnorm.norm_wing=0;	//X肢为基准
					}
					else
					{
						workplane.vBendPlaneNorm=pBendPlaneBaseAngle->vyWingNorm;
						workplane.designbendnorm.norm_wing=1;	//Y肢为基准
					}
					workplane.huoqustart.datum_pos_style=1;
					workplane.huoqustart.des_para.RODEND.hRod=pBendPlaneBaseAngle->handle;
					workplane.huoqustart.des_para.RODEND.wing_offset_style=4;
					workplane.huoqustart.des_para.RODEND.wing_offset_dist=0;
					workplane.huoqustart.des_para.RODEND.offset_wing=workplane.designbendnorm.norm_wing;	//偏移肢
					if(DISTANCE(pBolt->ucs.origin,pBendPlaneBaseAngle->Start())<DISTANCE(pBolt->ucs.origin,pBendPlaneBaseAngle->End()))
					{
						workplane.vBendPlanePick=pBendPlaneBaseAngle->Start();
						workplane.huoqustart.des_para.RODEND.direction=0;
					}
					else
					{
						workplane.vBendPlanePick=pBendPlaneBaseAngle->End();
						workplane.huoqustart.des_para.RODEND.direction=1;
					}
					pBaseBolt2=pBolt;
				}
			}
			if(stronglyAdviseBaseRod)
			{
				GEPOINT bendAxisVec=workplane.vBasePlaneNorm^workplane.vBendPlaneNorm;
				GEPOINT baseLenStdVec=pBaseRod->End()-pBaseRod->Start();
				normalize(bendAxisVec);
				normalize(baseLenStdVec);
				if(fabs(bendAxisVec*baseLenStdVec)>EPS_COS2)
				{
					workplane.designbendnorm.norm_style=2;
					workplane.designbendnorm.hVicePart=pBaseRod->handle;
					workplane.designbendnorm.hCrossPart=pRod->handle;
					workplane.designbendnorm.UpdateVector(pRod->BelongModel());
					workplane.vBendPlaneNorm=workplane.designbendnorm.vector;
					workplane.vBendPlanePick=pRod->End();
				}
				else
					stronglyAdviseBaseRod=false;
			}
		}
		if(pBaseAngle&&pBaseBolt1)
		{
			GEPOINT intersAngle,intersPlate;
			pBaseAngle->GetBoltIntersPos(pBaseBolt1,intersAngle);
			PARTSET partSet;
			FindBoltRelatedParts(pBaseBolt1,partSet);
			for(CLDSPart *pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
			{
				if(pPart->handle==pBaseAngle->handle||!pPart->IsPlate())
					continue;
				CLDSGeneralPlate* pThroughPlate=(CLDSGeneralPlate*)pPart;
				if(pThroughPlate)
				{
					pThroughPlate->GetBoltIntersPos(pBaseBolt1,intersPlate);
					if((intersPlate-intersAngle)*workplane.vBasePlaneNorm<-pBaseAngle->GetThick()/2)
						continue;	//板在角钢内皮
					workplane.xBaseNormOffset.AddThick(pThroughPlate->handle,TRUE,TRUE);
				}
			}
		}
		if(pBendPlaneBaseAngle!=NULL&&pBaseBolt2!=NULL)
		{
			GEPOINT intersAngle,intersPlate;
			pBendPlaneBaseAngle->GetBoltIntersPos(pBaseBolt2,intersAngle);
			PARTSET partSet;
			FindBoltRelatedParts(pBaseBolt2,partSet);
			for(CLDSPart *pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
			{
				if(pPart->handle==pBendPlaneBaseAngle->handle||!pPart->IsPlate())
					continue;
				CLDSGeneralPlate* pThroughPlate=(CLDSGeneralPlate*)pPart;
				if(pThroughPlate)
				{
					pBendPlaneBaseAngle->GetBoltIntersPos(pBaseBolt2,intersPlate);
					if((intersPlate-intersAngle)*workplane.vBendPlaneNorm<-pBendPlaneBaseAngle->GetThick()/2)
						continue;	//板在角钢内皮
					workplane.xBendNormOffset.AddThick(pThroughPlate->handle,TRUE,TRUE);
				}
			}
		}
		workplane.bTwoWorkPlane=!workplane.vBendPlaneNorm.IsZero();
		if(workplane.bTwoWorkPlane&&pBaseBolt1&&pBaseBolt2)
		{	//处理火曲面与基准面法线产生里外面悖论情况
			f3dPoint huoqu_pick,huoqu_vec,baseprjpick,bendprjpick;
			_Int3d2f(huoqu_pick,huoqu_vec,workplane.vBasePlanePick,workplane.vBasePlaneNorm,workplane.vBendPlanePick,workplane.vBendPlaneNorm);
			normalize(huoqu_vec);
			f3dPoint basepick=pBaseBolt1->ucs.origin;
			f3dPoint bendpick=pBaseBolt2->ucs.origin;
			project_point(basepick,workplane.vBasePlanePick,workplane.vBasePlaneNorm);
			project_point(bendpick,workplane.vBendPlanePick,workplane.vBendPlaneNorm);
			SnapPerp(&baseprjpick,huoqu_pick,huoqu_pick+huoqu_vec,basepick);
			SnapPerp(&bendprjpick,huoqu_pick,huoqu_pick+huoqu_vec,bendpick);
			f3dPoint extendvec1=basepick-baseprjpick;
			f3dPoint extendvec2=bendpick-bendprjpick;
			f3dPoint verify_axisvec1=extendvec1^workplane.vBasePlaneNorm;
			f3dPoint verify_axisvec2=extendvec2^workplane.vBendPlaneNorm;
			workplane.vBendLineVec=verify_axisvec1;
			if(verify_axisvec1*verify_axisvec2>0)	//应小于零，>0表示火曲面与基准面法线产生里外面悖论
			{
				if( workplane.designbendnorm.norm_style==1&&extendvec2*workplane.vBasePlaneNorm>0)
				{	//刘伟说苏州东方出L板时不考虑火曲变形,所以尽量让L板内曲
					workplane.designnorm.ReverseDirection();
					workplane.vBasePlaneNorm*=-1.0;
					workplane.vBendLineVec*=-1.0;
				}
				else
				{
					workplane.designbendnorm.ReverseDirection();
					workplane.vBendPlaneNorm*=-1.0;
				}
				workplane.vBendPlanePick=huoqu_pick;
				verify_axisvec2*=-1;
			}
			BOOL bBendAlongBaseAngle=stronglyAdviseBaseRod;	//是否延基准角钢制弯
			CLDSLineAngle *pBendBaseAngle=pBaseAngle;
			if(!bBendAlongBaseAngle)
			{	//从所有杆件中查找与制弯线方向一致的角钢 wht 16-11-10
			for(CLDSLinePart *pRod=rodset.GetFirst();pRod;pRod=rodset.GetNext())
			{
				if(!pRod->IsAngle())
					continue;
				GEPOINT base_angle_vec=pRod->End()-pRod->Start();
				normalize(base_angle_vec);
				double dd=base_angle_vec*huoqu_vec;
				if(fabs(base_angle_vec*huoqu_vec)>EPS_COS2)
				{	//基准角钢方向与制弯线方向一致
					CLDSLineAngle *pAngle=(CLDSLineAngle*)pRod;
					f3dLine base_angle_line(pAngle->Start(),pAngle->End());
					GEPOINT wing_vec=(workplane.designnorm.norm_wing==0)?pAngle->GetWingVecX():pAngle->GetWingVecY();
					if(wing_vec*extendvec2>0)	
					{	//火曲线在肢翼侧
						base_angle_line.startPt+=wing_vec*pAngle->GetWidth();
						base_angle_line.endPt+=wing_vec*pAngle->GetWidth();
					}
					double fDist=0;
					f3dPoint perp=huoqu_pick;
					SnapPerp(&perp,base_angle_line,huoqu_pick,&fDist);
					f3dPoint huoqu_pick_vec=huoqu_pick-perp;
					normalize(huoqu_pick_vec);
					if(huoqu_pick_vec*extendvec2<0||fDist<g_sysPara.HuoQuDist)
					{
						pBendBaseAngle=pAngle;
						bBendAlongBaseAngle=TRUE;
						break;
					}
				}
			}
			}
			if(bBendAlongBaseAngle&&pBendBaseAngle&&workplane.designnorm.norm_style==1)
			{
				workplane.huoqustart.datum_pos_style=workplane.huoquend.datum_pos_style=1;
				workplane.huoqustart.des_para.RODEND.hRod=workplane.huoquend.des_para.RODEND.hRod=pBendBaseAngle->handle;
				workplane.huoqustart.des_para.RODEND.direction=0;
				workplane.huoquend.des_para.RODEND.direction=1;
				workplane.huoqustart.des_para.RODEND.offset_wing=workplane.huoquend.des_para.RODEND.offset_wing=workplane.designnorm.norm_wing;
				workplane.huoqustart.des_para.RODEND.wing_offset_style=workplane.huoquend.des_para.RODEND.wing_offset_style=4;
				GEPOINT wing_vec;
				if(workplane.designnorm.norm_wing==0)
					wing_vec=pBendBaseAngle->GetWingVecX();
				else
					wing_vec=pBendBaseAngle->GetWingVecY();
				GEPOINT otherface_extendvec=(pBendBaseAngle->feature==1)?extendvec2:extendvec1;
				if(wing_vec*otherface_extendvec<0)	//火曲线在楞线侧
					workplane.huoqustart.des_para.RODEND.wing_offset_dist=workplane.huoquend.des_para.RODEND.wing_offset_dist=-g_sysPara.HuoQuDist;
				else
					workplane.huoqustart.des_para.RODEND.wing_offset_dist=workplane.huoquend.des_para.RODEND.wing_offset_dist=g_sysPara.HuoQuDist+pBendBaseAngle->size_wide;
				workplane.huoquend.des_para.RODEND.len_offset_dist=200;
				//由于底面火曲线不一定在角钢外皮上，所以最好还是自动纠正，如果确不合理可以由用户在钢板属性栏中进行调整 wjh-2016.12.20
				workplane.m_bSelfCorrectBendline=true;//false;
			}
			else 
				workplane.m_bSelfCorrectBendline=true;
		}
	}
	//选中一根角钢上的螺栓也应支持生成钢板
	return pBaseRod!=NULL && boltset.GetNodeNum()>=1 && rodset.GetNodeNum()>=1;
}
#include "KeypointLifeObj.h"
int CLDSView::ConnectionPlate()
{	//连接板
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr,promptStr;
	char sText[MAX_TEMP_CHAR_200+1]="";
	CLDSNode *pStartNode=NULL,*pEndNode=NULL;
	CLDSPlate plateTemp;
	CLDSPart *pPart=NULL;
	CLDSLinePart* pRod=NULL,*pBaseRod=NULL;
	CLDSNode *pBaseNode=NULL;
	CSuperSmartPtr<CLDSLinePart> pBaseLinePart,pSetupPlaneLinePart;
	CHashSet<CLDSLinePart*> rodset;
	CHashSet<CLDSBolt*> boltset;
	CCmdLockObject cmdlock(this);
	_LOCAL_CONNECT_PLANE workplane;
	try{
		long *id_arr,n=g_pSolidSnap->GetLastSelectEnts(id_arr);
		for(int i=0;i<n;i++)
		{
			CLDSBolt *pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[i],CLS_BOLT);
			if(pBolt==NULL)
				continue;
			boltset.SetValue(pBolt->handle,pBolt);
		}
		if(!RetrieveConnectPlateInfoByBolts(boltset,pBaseNode,pBaseRod,rodset,workplane))
			boltset.Empty();
		else
			pBaseLinePart=pBaseRod;
		if(boltset.GetNodeNum()==0)
		{
		//SELECT_ALL-0x03是剔除掉单线及节点(经常影响螺栓选取) wjh-2016.5.12
		GET_SCR_PART_PARA scr_part_para(SELECT_ALL-0x03,CLS_LINEPART,CLS_BOLT);
		scr_part_para.disp_type=DISP_SOLID;
#ifdef AFX_TARG_ENU_ENGLISH
		scr_part_para.cmdStr="please choose assembling location rod for connecting plates:";
		scr_part_para.cmdErrorFeedbackStr="No suitable parts selected,please choose assembling location rod for connecting plates again:";
#else
		scr_part_para.cmdStr="请选择连接板的装配定位杆件, 或直接选择全部连接螺栓:";
		scr_part_para.cmdErrorFeedbackStr="没有选中合适的构件，请重新选择连接板的装配定位杆件或全部连接螺栓:";
#endif
		scr_part_para.nResultObjs=2;
		while(1)
		{
			//设置命令行响应鼠标左键抬起的消息，同按回车键效果相同
			if(!GetPartsFromScr(scr_part_para,FALSE))
				return 0;
			CLDSDbObject *pObj=scr_part_para.pResultObjsArr[0];
			if(pObj&&pObj->IsPart()&&((CLDSPart*)pObj)->IsLinePart())
				pBaseLinePart =(CLDSLinePart*)pObj;
			for(int k=0;pBaseLinePart.IsNULL()&&k<scr_part_para.nResultObjs;k++)
			{
				pObj=scr_part_para.pResultObjsArr[k];
				if(pObj&&pObj->GetClassTypeId()==CLS_BOLT)
					boltset.SetValue(pObj->handle,(CLDSBolt*)pObj);
			}
			if(pBaseLinePart.IsHasPtr())
				boltset.Empty();
			else if(!RetrieveConnectPlateInfoByBolts(boltset,pBaseNode,pBaseRod,rodset,workplane))
				return 0;
			else	//用根据螺栓集合提取的装配定位杆件更新pBaseRod;
				pBaseLinePart=pBaseRod;
			break;
		}
		}

		if(boltset.GetNodeNum()==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select assembling location node of rod:","");
#else
		pCmdLine->FillCmdLine("请选择杆件上的装配定位节点:","");
#endif
		DWORD dwhObj=0,dwExportFlag=0;
		CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
		verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prPoint);
		CDisplayNodeAtFrontLife displayNode;
		displayNode.DisplayNodeAtFront();
		while(1)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;
			pBaseNode= console.FromNodeHandle(dwhObj);
			if(pBaseNode&&pBaseNode->IsNode())
				break;
		}
		g_pSolidDraw->SetEntSnapStatus(pBaseNode->handle);
		pCmdLine->FinishCmdLine(CXhChar16("节点0x%X",pBaseNode->handle));

		GET_SCR_PART_PARA scr_rod_para(GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_LINETUBE),CLS_LINEPART,0,2);
		scr_rod_para.disp_type=DISP_SOLID;
#ifdef AFX_TARG_ENU_ENGLISH
		scr_rod_para.cmdStr="Please choose all the rods connected with fundamental planes:";
		scr_rod_para.cmdErrorFeedbackStr="No suitable parts selected,Please choose all the rods connected with fundamental planes again:";
#else
		scr_rod_para.cmdStr="请选择基本面所连接的所有杆件:";
		scr_rod_para.cmdErrorFeedbackStr="没有选中合适的构件，请选择基本面所连接的所有杆件:";
#endif
		OpenWndSel();
		rodset.SetValue(pBaseLinePart->handle,pBaseLinePart);
		while(1)
		{
			if(GetPartsFromScr(scr_rod_para,FALSE))
			{
				if(scr_rod_para.nResultObjs==0)
				{
					scr_rod_para.nResultObjs=2;	//大于2代表多选
					continue;
				}
				for(int i=0;i<scr_rod_para.nResultObjs;i++)
				{
					if(scr_rod_para.pResultObjsArr[i]&&scr_rod_para.pResultObjsArr[i]->handle!=pBaseLinePart->handle)
						rodset.SetValue(scr_rod_para.pResultObjsArr[i]->handle,(CLDSLinePart*)scr_rod_para.pResultObjsArr[i]);
				}
				if(rodset.GetNodeNum()>1)
					break;
			}
			else
				return 0;
		}
		OnOperOther();	//取消开窗操作状态
		}
		f3dPoint baseLenStdVec=pBaseLinePart->End()-pBaseLinePart->Start();
		normalize(baseLenStdVec);

		for(pRod=rodset.GetFirst();pRod;pRod=rodset.GetNext())
		{
			f3dPoint rayLenVec=pRod->End()-pRod->Start();
			normalize(rayLenVec);
			if(fabs(baseLenStdVec*rayLenVec)>EPS_COS)
				continue;
			if(boltset.GetNodeNum()==0||pRod->feature==1)
				pSetupPlaneLinePart=pRod;
			break;
		}
		//创建HashTable
		CHashTable<CLDSPart*>partsetTable;
		partsetTable.CreateHashTable(100);
		CDesignLjPartPara desLjPartPara;
		if(pBaseLinePart.IsHasPtr())
		{	//依据输入的基准杆件确定装配坐标系
			if(pBaseNode)
				plateTemp.designInfo.m_hBaseNode = pBaseNode->handle;
			plateTemp.designInfo.m_hBasePart = pBaseLinePart->handle;
			if(pBaseNode&&pBaseLinePart->pEnd==pBaseNode)
				plateTemp.ucs.axis_y = pBaseLinePart->Start()-pBaseLinePart->End();
			else
				plateTemp.ucs.axis_y = pBaseLinePart->End()-pBaseLinePart->Start();
			normalize(plateTemp.ucs.axis_y);
			if(boltset.GetNodeNum()>0)
			{
				plateTemp.ucs.axis_z = workplane.vBasePlaneNorm;
				plateTemp.ucs.axis_x = plateTemp.ucs.axis_y^plateTemp.ucs.axis_z;
				normalize(plateTemp.ucs.axis_x);
			}
			else if(pSetupPlaneLinePart.IsHasPtr())
			{
				plateTemp.ucs.axis_x = pSetupPlaneLinePart->End()-pSetupPlaneLinePart->Start();
				plateTemp.ucs.axis_z = plateTemp.ucs.axis_x^plateTemp.ucs.axis_y;
				normalize(plateTemp.ucs.axis_z);
			}
			const double cosa=0.86602540378443864676372317075294;	//cos30
			double dd=plateTemp.ucs.axis_z*console.GetActiveView()->ucs.axis_z;
			if(fabs(dd)>cosa&&dd<0)
			{	//当前视图法线与钢板法线夹角小于30度时保证钢板法线与视图法线方向一致 wht 16-10-31
				plateTemp.ucs.axis_z*=-1.0;
				workplane.designnorm.ReverseDirection();
				workplane.designbendnorm.ReverseDirection();
			}
			if(pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
			{
				plateTemp.designInfo.norm.norm_style=2;	//杆件交叉面法线
				plateTemp.designInfo.norm.hVicePart=pBaseLinePart->handle;
				if(pSetupPlaneLinePart.IsHasPtr())
					plateTemp.designInfo.norm.hCrossPart=pSetupPlaneLinePart->handle;
				plateTemp.designInfo.norm.nearVector = plateTemp.ucs.axis_z;
			}
			else //if(pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE||pBaseLinePart->GetClassTypeId()==CLS_GROUP_LINEANGLE)
			{
				plateTemp.designInfo.norm.direction = 0;	//朝外
				plateTemp.designInfo.norm.norm_style = 1;	//角钢肢法线方向
				plateTemp.designInfo.norm.hVicePart = pBaseLinePart->handle;
				double ddx=pBaseLinePart.LineAnglePointer()->vxWingNorm*plateTemp.ucs.axis_z;
				double ddy=pBaseLinePart.LineAnglePointer()->vyWingNorm*plateTemp.ucs.axis_z;
				if(fabs(ddx)>fabs(ddy)&&fabs(ddx)>0.95)
				{
					plateTemp.designInfo.norm.norm_wing = 0;	//X肢法线
					if(ddx>0)
						plateTemp.ucs.axis_z = pBaseLinePart.LineAnglePointer()->vxWingNorm;
					else //if(ddx<0)
					{
						plateTemp.ucs.axis_z =-pBaseLinePart.LineAnglePointer()->vxWingNorm;
						plateTemp.designInfo.norm.direction =1;
					}
				}
				else if(fabs(ddy)>fabs(ddx)&&fabs(ddy)>0.95)
				{
					plateTemp.designInfo.norm.norm_wing = 1;	//Y肢法线
					if(ddy>0)
						plateTemp.ucs.axis_z = pBaseLinePart.LineAnglePointer()->vyWingNorm;
					else //if(ddy<0)
					{
						plateTemp.ucs.axis_z =-pBaseLinePart.LineAnglePointer()->vyWingNorm;
						plateTemp.designInfo.norm.direction =1;
					}
				}
				else
				{
					pCmdLine->FinishCmdLine();
					if(fabs(ddx)>fabs(ddy))
#ifdef AFX_TARG_ENU_ENGLISH
						pCmdLine->FillCmdLine("incorrect judgment of connecting plate's normal, please confirm the current connecting leg of the datum angle[X leg(X)/Y leg(Y)]<X>","");
					else
						pCmdLine->FillCmdLine("incorrect judgment of connecting plate's normal, please confirm the current connecting leg of the datum angle[X leg(X)/Y leg(Y)]<X>",""); 
#else
						pCmdLine->FillCmdLine("连板法线方向判断不准，请确认基准角钢当前的连接肢[X肢(X)/Y肢(Y)]<X>","");
					else
						pCmdLine->FillCmdLine("连板法线方向判断不准，请确认基准角钢当前的连接肢[X肢(X)/Y肢(Y)]<Y>","");
#endif
					while(1)
					{
						if(!pCmdLine->GetStrFromCmdLine(cmdStr))
							return 0;
						if((fabs(ddx)>fabs(ddy)&&cmdStr.GetLength()==0)||cmdStr.CompareNoCase("X")==0)
							plateTemp.designInfo.norm.norm_wing=0;	//X肢法线
						else if(cmdStr.GetLength()==0||cmdStr.CompareNoCase("Y")==0)
							plateTemp.designInfo.norm.norm_wing=1;	//Y肢法线
						else
						{
#ifdef AFX_TARG_ENU_ENGLISH
							pCmdLine->FillCmdLine("option key words needed","");
							pCmdLine->FinishCmdLine();
							if(fabs(ddx)>fabs(ddy))
								pCmdLine->FillCmdLine("incorrect judgment of connecting plate's normal, please confirm the current connecting leg of the datum angle[X leg(X)/Y leg(Y)]<X>","");
							else
								pCmdLine->FillCmdLine("incorrect judgment of connecting plate's normal, please confirm the current connecting leg of the datum angle[X leg(X)/Y leg(Y)]<X>","");
#else
							pCmdLine->FillCmdLine("需要选项关键字","");
							pCmdLine->FinishCmdLine();
							if(fabs(ddx)>fabs(ddy))
								pCmdLine->FillCmdLine("连板法线方向判断不准，请确认基准角钢当前的连接肢[X肢(X)/Y肢(Y)]<X>","");
							else
								pCmdLine->FillCmdLine("连板法线方向判断不准，请确认基准角钢当前的连接肢[X肢(X)/Y肢(Y)]<Y>","");
#endif
							continue;
						}
						break;
					}
				}
			}
			if(boltset.GetNodeNum()>0)
			{
				plateTemp.ucs.axis_z = workplane.vBasePlaneNorm;
				CLDSBolt *pDatumBolt=NULL;
				for(CLDSBolt *pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
				{
					if(fabs(pBolt->get_norm()*workplane.vBasePlaneNorm)>EPS_COS2)
					{
						if(pDatumBolt==NULL)
							pDatumBolt=pBolt;
						if( (pBolt->des_base_pos.datumPoint.datum_pos_style==1&&
							pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod==pBaseRod->handle)||
							(pBolt->des_base_pos.datumPoint.datum_pos_style==2&&
							pBolt->des_base_pos.datumPoint.des_para.RODNODE.hRod==pBaseRod->handle)||
							pBaseRod->FindLsByHandle(pBolt->handle))
						{
							pDatumBolt=pBolt;
							break;
						}
					}
				}
				if(pDatumBolt)
					plateTemp.designInfo.origin=pDatumBolt->des_base_pos.datumPoint;
			}
			else if(pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
			{
				plateTemp.designInfo.origin.datum_pos_style = 2;
				plateTemp.designInfo.origin.des_para.RODNODE.hNode = pBaseNode->handle;
				plateTemp.designInfo.origin.des_para.RODNODE.hRod =pBaseLinePart->handle;
			}
			else if(pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE||pBaseLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				plateTemp.designInfo.origin.datum_pos_style = 2;
				plateTemp.designInfo.origin.des_para.RODNODE.hRod = pBaseLinePart->handle;
				plateTemp.designInfo.origin.des_para.RODNODE.wing_offset_style = 0;
				plateTemp.designInfo.origin.des_para.RODNODE.hNode = pBaseNode->handle;
				if(plateTemp.designInfo.norm.norm_wing==0)
				{
					plateTemp.designInfo.origin.des_para.RODNODE.offset_wing=0;
					plateTemp.ucs.origin = pBaseLinePart.LineAnglePointer()->GetDatumPosWingX(pBaseNode);
				}
				else
				{
					plateTemp.designInfo.origin.des_para.RODNODE.offset_wing=1;
					plateTemp.ucs.origin = pBaseLinePart.LineAnglePointer()->GetDatumPosWingY(pBaseNode);
				}
				plateTemp.designInfo.iFaceType = 1;
			}
			if(workplane.vBasePlaneNorm.IsZero())
				workplane.vBasePlaneNorm=plateTemp.ucs.axis_z;
		}
		
		//{
		const BOOL bDisplayAngleProfileParaDlg=TRUE;
		for(pRod=rodset.GetFirst();pRod;pRod=rodset.GetNext())
		{
			pPart=pRod;
			if(!partsetTable.GetValueAt(pPart->handle,pPart))
			{		
				partsetTable.SetValueAt(pPart->handle,pPart);
				desLjPartPara.hPart=pPart->handle;
				if(boltset.GetNodeNum()==0)
					desLjPartPara.iFaceNo = 1;
				else	//通过连接螺栓生成的钢板
					desLjPartPara.iFaceNo = (short)pPart->feature;
				if(desLjPartPara.iFaceNo>plateTemp.designInfo.iFaceType)
					plateTemp.designInfo.iFaceType=desLjPartPara.iFaceNo;
				desLjPartPara.m_nClassTypeId = pPart->GetClassTypeId();
				desLjPartPara.start0_end1=CDesignJdb::LjPosInPart(pBaseLinePart,pBaseNode,pRod);
					
				if(pPart->GetClassTypeId()==CLS_LINEANGLE)
				{
					CLDSLineAngle *pAngle=(CLDSLineAngle*)pPart;
					f3dPoint vWorkNorm=pPart->feature==2?workplane.vBendPlaneNorm:workplane.vBasePlaneNorm;
					double ddx=vWorkNorm*pAngle->vxWingNorm;
					double ddy=vWorkNorm*pAngle->vyWingNorm;
					if(fabs(ddx)<fabs(ddy))
						desLjPartPara.angle.cur_wing_x0_y1=1;
					else
						desLjPartPara.angle.cur_wing_x0_y1=0;
					if( desLjPartPara.start0_end1==2||
						(pPart->handle==pBaseLinePart->handle&&
						 pBaseNode!=pBaseLinePart->pStart&&
						 pBaseNode!=pBaseLinePart->pEnd))
						desLjPartPara.angle.bTwoEdge=TRUE;
					else
						desLjPartPara.angle.bTwoEdge=FALSE;
					desLjPartPara.angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
					desLjPartPara.angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
					//端距默认值可能不满足螺栓最小边距，此处赋值为0，设计钢板时根据螺栓直径自动计算螺栓端距 wht 16-01-05
					desLjPartPara.end_space=0;
					if(bDisplayAngleProfileParaDlg)
					{
						g_pSolidDraw->ReleaseSnapStatus();
						g_pSolidDraw->SetEntSnapStatus(pPart->handle);
						CAngleProfileParaDlg angleProfileDlg;
						angleProfileDlg.m_pRod=pAngle;
						angleProfileDlg.m_pBoltSet=&boltset;
						angleProfileDlg.m_bTwoEdgeProfile=desLjPartPara.angle.bTwoEdge;
						angleProfileDlg.m_iBerSpaceMode=angleProfileDlg.m_iWingSpaceMode=ANGLE_SPACE_FLAG::SPACE_TOEDGE;
						if(angleProfileDlg.DoModal()==IDOK)
						{
							desLjPartPara.angle.cbSpaceFlag.SetBerSpaceStyle(angleProfileDlg.m_iBerSpaceMode);
							desLjPartPara.angle.cbSpaceFlag.SetWingSpaceStyle(angleProfileDlg.m_iWingSpaceMode);
							desLjPartPara.ber_space=angleProfileDlg.m_fBerSpace;
							desLjPartPara.wing_space=angleProfileDlg.m_fWingSpace;
							desLjPartPara.angle.bTwoEdge=angleProfileDlg.m_bTwoEdgeProfile;
							//端距默认值可能不满足螺栓最小边距，此处赋值为0，设计钢板时根据螺栓直径自动计算螺栓端距 wht 16-01-05
							desLjPartPara.end_space=0;	
						}
					}
				}
				plateTemp.designInfo.partList.SetValue(desLjPartPara.hPart,desLjPartPara);
			}
		}
		g_pSolidDraw->ReleaseSnapStatus();
		OnOperOther();	//取消开窗操作状态
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
		//}
		Ta.BeginUndoListen();
		int nThick=5;	//骏天说射线角钢肢厚小于4时，板厚取5mm wjh-2013.5.7
		for(CDesignLjPartPara *pPartPara=plateTemp.designInfo.partList.GetFirst();pPartPara;
		pPartPara=plateTemp.designInfo.partList.GetNext())
		{
			if(partsetTable.GetValueAt(pPart->handle,pPart))
			{
				if(!pBaseLinePart.IsEqualPtr(pPart)&&pPart->GetClassTypeId()==CLS_LINEANGLE)
				{
					if(((CLDSLineAngle*)pPart)->GetThick()>nThick)
						nThick=ftoi(((CLDSLineAngle*)pPart)->GetThick());
				}
			}
		}
		if(nThick%2>0)
			nThick+=1;	//尽量使用偶数板厚
		CLDSPlate *pPlate= (CLDSPlate*)console.AppendPart(CLS_PLATE);
		pPlate->Thick=nThick;
		if(boltset.GetNodeNum()>0)
		{
			for(CLDSBolt *pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
			{
				if(!workplane.bTwoWorkPlane&&fabs(pBolt->get_norm()*plateTemp.ucs.axis_z)<0.7071) //cos45=0.70710678118655
					continue;	//根据钢板法线过滤掉不垂直钢板的螺栓 wht 16-01-05
				//处理螺栓射线号与螺栓引用射线号间的映射
				CLsRef *pLsRef=pPlate->AppendLsRef(pBolt->GetLsRef());
				pBolt->AddL0Thick(pPlate->handle,TRUE);
				pBolt->CalGuigeAuto();
				pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			}
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("please select all the relative bolts of connecting plate:","");
#else
		pCmdLine->FillCmdLine("请选择连接板关联的所有螺栓:","");
#endif
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
		Invalidate(FALSE);
		OpenWndSel();	//开窗选择螺栓
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{	
			long *id_arr;
			int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
			for(int i=0;i<n;i++)
			{
				pPart=console.FromPartHandle(id_arr[i]);
				if(pPart&&pPart->GetClassTypeId()==CLS_BOLT)
				{
					CLDSBolt *pBolt=(CLDSBolt*)pPart;
					if(fabs(pBolt->get_norm()*plateTemp.ucs.axis_z)<0.7071) //cos45=0.70710678118655
						continue;	//根据钢板法线过滤掉不垂直钢板的螺栓 wht 16-01-05
					//处理螺栓射线号与螺栓引用射线号间的映射
					CLsRef *pLsRef=pPlate->AppendLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick(pPlate->handle,TRUE);
					pBolt->CalGuigeAuto();
					pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
		}
		else
		{
			pCmdLine->CancelCmdLine();	
			//命令中断，清除冗余数据
			if(pPlate)
			{
				console.DispPartSet.DeleteNode(pPlate->handle);
				g_pSolidDraw->DelEnt(pPlate->handle);
				console.DeletePart(pPlate->handle);
			}
			return 0;
		}
		OnOperOther();	//取消开窗操作状态
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
		}
		static CDesignConnectionPlateDlg ljb_dlg;
		if(pBaseLinePart.IsHasPtr())
			ljb_dlg.m_pLinePart=pBaseLinePart.LinePartPointer();
		//if(pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
		//	ljb_dlg.m_pLinePart=pBaseLinePart.LineTubePointer();
		//else if(pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE||pBaseLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		//	ljb_dlg.m_pLinePart=pBaseLinePart.LineAnglePointer();
		ljb_dlg.m_nPlateThick=nThick;
		double fWeldWidth=60,weld_start_high=0,weld_end_high=0,fEdgeSpace=0;
		BOOL bAdjustWeldLen=TRUE;
		ljb_dlg.m_iFaceN=plateTemp.designInfo.iFaceType-1;
		if(ljb_dlg.DoModal()==IDOK)
		{
			plateTemp.Thick=ljb_dlg.m_nPlateThick;
			plateTemp.cMaterial=CSteelMatLibrary::RecordAt(ljb_dlg.m_iPlateMaterial).cBriefMark;
			plateTemp.iSeg=SEGI(ljb_dlg.m_sSegI.GetBuffer());
			plateTemp.SetPartNo(ljb_dlg.m_sPartNo.GetBuffer(0));
			plateTemp.face_N=ljb_dlg.m_iFaceN+1;
			pPlate->designInfo.iWeldLineStyle=ljb_dlg.m_bSpecWeldScope;
			weld_start_high=ljb_dlg.m_fWeldStartHigh;
			weld_end_high=ljb_dlg.m_fWeldEndHigh;
			fWeldWidth=ljb_dlg.m_fWeldWidth;
			bAdjustWeldLen=ljb_dlg.m_bAdjustWeldLen;
			fEdgeSpace=ljb_dlg.m_fEdgeSpace;
		}
		else //命令中断，清除冗余数据
		{	
			if(pPlate)
			{
				console.DispPartSet.DeleteNode(pPlate->handle);
				g_pSolidDraw->DelEnt(pPlate->handle);
				console.DeletePart(pPlate->handle);
			}
			return 0;
		}
		pPlate->cMaterial = plateTemp.cMaterial;
		pPlate->Thick=ftoi(plateTemp.GetThick());
		pPlate->SetPartNo(plateTemp.GetPartNo());
		pPlate->iSeg = plateTemp.iSeg;
		pPlate->designInfo.iFaceType = plateTemp.designInfo.iFaceType;
		pPlate->face_N = plateTemp.face_N;
		if(pBaseNode)
			pPlate->CopyModuleInstanceInfo(pBaseNode);//调整钢板配材号与基准构件或基准节点配材号一致
		else if(pBaseRod) 
			pPlate->CopyModuleInstanceInfo(pBaseRod);
		pPlate->designInfo.m_hBaseNode=plateTemp.designInfo.m_hBaseNode;
		pPlate->designInfo.m_hBasePart=plateTemp.designInfo.m_hBasePart;
		pPlate->designInfo.m_bEnableFlexibleDesign=TRUE;
		pPlate->designInfo.origin = plateTemp.designInfo.origin;
		pPlate->designInfo.norm = plateTemp.designInfo.norm;
		if( pBaseLinePart.IsHasPtr()&&pBaseLinePart->IsAngle()&&boltset.GetNodeNum()>0)
		{
			if(pPlate->designInfo.norm.direction==1)
				workplane.xBaseNormOffset.AddThick(pPlate->handle,TRUE);
			if(pPlate->designInfo.origin.datum_pos_style==1)
				strncpy(pPlate->designInfo.origin.des_para.RODEND.norm_offset,(char*)workplane.xBaseNormOffset.ToThickStr(),50);
			else if(pPlate->designInfo.origin.datum_pos_style==2)
				strncpy(pPlate->designInfo.origin.des_para.RODNODE.norm_offset,(char*)workplane.xBaseNormOffset.ToThickStr(),50);
			else if(pPlate->designInfo.origin.datum_pos_style==3)
				strncpy(pPlate->designInfo.origin.des_para.AXIS_INTERS.norm_offset,(char*)workplane.xBaseNormOffset.ToThickStr(),50);
			else if(pPlate->designInfo.origin.datum_pos_style==0)
				pPlate->designInfo.origin.datum_pos+=pPlate->ucs.axis_z*workplane.xBaseNormOffset.Thick(&Ta);
		}
		if(workplane.bTwoWorkPlane)
		{
			if(workplane.designbendnorm.direction==1)
				workplane.xBendNormOffset.AddThick(pPlate->handle,TRUE);
			pPlate->designInfo.huoqufacenorm[0]=workplane.designbendnorm;
			pPlate->huoqufacenorm[0]=workplane.vBendPlaneNorm;
			if(workplane.huoqustart.datum_pos_style==1&&workplane.huoqustart.des_para.RODEND.hRod>0x20)
			{
				strncpy(workplane.huoqustart.des_para.RODEND.norm_offset,workplane.xBendNormOffset.ToThickStr(),50);
				workplane.huoqustart.UpdatePos(pPlate->BelongModel());
				pPlate->designInfo.huoquline_start[0]=workplane.huoqustart;
				if(workplane.huoquend.datum_pos_style==1)
				{
					workplane.huoquend.UpdatePos(pPlate->BelongModel());
					pPlate->designInfo.huoquline_end[0]=workplane.huoquend;
				}
				else
					pPlate->designInfo.huoquline_end[0].datum_pos=workplane.vBendPlanePick+workplane.vBendLineVec;
				pPlate->designInfo.huoquline_end[0].UpdatePos(&Ta);
			}
			else
			{
				if(workplane.designbendnorm.direction==0)	//朝外
					workplane.vBendPlanePick+=workplane.vBendPlaneNorm*workplane.xBendNormOffset.Thick(pPlate->BelongModel());
				else
					workplane.vBendPlanePick-=workplane.vBendPlaneNorm*workplane.xBendNormOffset.Thick(pPlate->BelongModel());
				pPlate->designInfo.huoquline_start[0].datum_pos_style=pPlate->designInfo.huoquline_end[0].datum_pos_style=0;
				pPlate->designInfo.huoquline_start[0].des_para.RODEND.bIncOddEffect=pPlate->designInfo.huoquline_end[0].des_para.RODEND.bIncOddEffect=TRUE;
				pPlate->designInfo.huoquline_start[0].datum_pos=workplane.vBendPlanePick;
				pPlate->designInfo.huoquline_end[0].datum_pos=workplane.vBendPlanePick+workplane.vBendLineVec;
				pPlate->designInfo.huoquline_start[0].UpdatePos(&Ta);
				pPlate->designInfo.huoquline_end[0].UpdatePos(&Ta);
			}
		}
		if(pBaseLinePart.IsHasPtr()&&pBaseLinePart->IsLinePart())
		{
			pPlate->ucs.axis_x.Set(0,0,0);
			pPlate->ucs.axis_y.Set(0,0,0);
		}
		if(pPlate->designInfo.origin.datum_pos_style==0)
			pPlate->ucs.origin=pPlate->designInfo.origin.Position();
		pPlate->DesignSetupUcs();
		//自动调整射线角钢的摆放位置和螺栓位置
		for(CDesignLjPartPara *pLjPart=plateTemp.designInfo.partList.GetFirst();pLjPart;pLjPart=plateTemp.designInfo.partList.GetNext())
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(pLjPart->hPart,CLS_LINEPART);
			if(pLinePart==NULL||(pLinePart&&!pLinePart->IsLinePart()))
				continue;
			if(pLinePart->GetClassTypeId()==CLS_LINETUBE&&(pLjPart->start0_end1==0||pLjPart->start0_end1==1))
				pLjPart->ber_space=fEdgeSpace;	//射线杆件为钢管时设置插板边距
			pPlate->designInfo.partList.SetValue(pLjPart->hPart,*pLjPart);
			if(pLinePart->handle==pBaseLinePart->handle)
				continue;	//跳过基准定位杆件
			if(pLjPart->start0_end1!=0&&pLjPart->start0_end1!=1)
				continue;	//跳过非始终端连接的射线杆件
			if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//调整卷边板上螺栓的法向偏移量
				CLDSLineTube *pLineTube=(CLDSLineTube*)pLinePart;
				CSuperSmartPtr<CLDSPart> pPart;
				if(pLjPart->start0_end1==0)	//始端连接
					pPart=console.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
				else if(pLjPart->start0_end1==1)//终端连接
					pPart=console.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
				if(pPart.IsNULL())
					continue;
				if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&pPart.ParamPlatePointer()->m_iParamType==TYPE_ROLLEND)
				{
					for(CLsRef *pLsRef=pPart.ParamPlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPart.ParamPlatePointer()->GetNextLsRef())
					{
						if((*pLsRef)->des_work_norm.direction==1)	//朝里(与板法线方向相反)
							(*pLsRef)->des_base_pos.norm_offset.AddThick(-pPlate->handle,TRUE);
					}
					//调整钢管摆放位置
					if(g_sysPara.nAutoAdjustTubePos==1||g_sysPara.nAutoAdjustTubePos==2)
					{
						double fWorkPlaneRotAngle=0;	//工作平面旋转角度
						double offset_dist=-0.5*(pPart.ParamPlatePointer()->GetThick()+pPlate->GetThick());
						double X=0;	//D与标准法兰相关参数同名,用X表示旋转角度  暂时如此处理 wht 10-01-21
						if(pPart.ParamPlatePointer()->m_bStdPart)
							pPart.ParamPlatePointer()->GetDesignItemValue('X',&fWorkPlaneRotAngle);
						else
							pPart.ParamPlatePointer()->GetDesignItemValue('D',&fWorkPlaneRotAngle);
						if(fabs(fabs(fWorkPlaneRotAngle)-180)<EPS)	//插板卷边方向相反与插板法线方向相反
							offset_dist*=-1.0;	
						if(g_sysPara.nAutoAdjustTubePos==2)	//钢管偏移
						{	//重新调整钢管法向偏移量 wht 10-07-27
							if(pLjPart->start0_end1==0)
								pLineTube->desStartPos.fNormOffset=offset_dist;
							else if(pLjPart->start0_end1==1)
								pLineTube->desEndPos.fNormOffset=offset_dist;
						}
						else if(g_sysPara.nAutoAdjustTubePos==1) //插板偏移
							pPart.ParamPlatePointer()->m_fNormOffset-=offset_dist;
						//到位钢管并重新设计插板
						pLineTube->SetModified();
						pLineTube->ClearFlag();
						pLineTube->CalPosition();
						pLineTube->CalWorkPlaneNorm();
						pLineTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
						if(pLjPart->start0_end1==0)
							pLineTube->DesignStartJointUCS(pPart.ParamPlatePointer());
						else if(pLjPart->start0_end1==1)
							pLineTube->DesignEndJointUCS(pPart.ParamPlatePointer());
						pPart.ParamPlatePointer()->DesignPlate();
						pPart.ParamPlatePointer()->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
					}
				}
			}
			else if(boltset.GetNodeNum()==0&&pLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{
				CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLinePart;
				f3dPoint verify_norm;
				//暂时仅处理单面板的情况
				verify_norm=pPlate->ucs.axis_z;
				double ddx=pLineAngle->get_norm_x_wing()*verify_norm;
				double ddy=pLineAngle->get_norm_y_wing()*verify_norm;
				CMultiOffsetPos *pPosDesign=NULL;
				if(pLjPart->start0_end1==0)
					pPosDesign=&pLineAngle->desStartPos;
				else if(pLjPart->start0_end1==1)
					pPosDesign=&pLineAngle->desEndPos;
				if(pPosDesign==NULL)
					continue;
				if(!pLjPart->angle.bTwoEdge)
				{	//共用板上的副基准角钢可通过<两端轮廓>属性甄别，并避免错误设定位置　wjh-2015.9.10
					if(fabs(ddx)>fabs(ddy))
					{	
						pPosDesign->wing_x_offset.gStyle=4;
						//基准杆件为钢管时里/外铁的摆放位置都需要调整
						if(pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
							pPosDesign->wing_x_offset.offsetDist=-0.5*pPlate->GetThick();
						//基准杆件为角钢时需调整外铁的摆放位置
						else if(pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE&&ddx<0)
							pPosDesign->wing_x_offset.offsetDist=-pPlate->GetThick();
					}
					else
					{
						pPosDesign->wing_y_offset.gStyle=4;
						//基准杆件为钢管时里/外铁的摆放位置都需要调整
						if(pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
							pPosDesign->wing_y_offset.offsetDist=-0.5*pPlate->GetThick();
						//基准杆件为角钢时需调整外铁的摆放位置
						else if(pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE&&ddy<0)
							pPosDesign->wing_y_offset.offsetDist=-pPlate->GetThick();
					}
					pLineAngle->ClearFlag();
					pLineAngle->CalPosition();
					//计算角钢位置后重新计算切角切肢信息
					if(pBaseLinePart.IsHasPtr()&&pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE)
						pLineAngle->CalCutAngleInfo(pBaseLinePart.LineAnglePointer(),NULL,(pLjPart->start0_end1==0),false);
				}
				pLineAngle->SetModified();
				pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
				if((fabs(ddx)>fabs(ddy)&&ddx<0)||(fabs(ddx)<fabs(ddy)&&ddy<0))
				{	//调整外铁上的螺栓的法向偏移量
					for(CLsRef *pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
						(*pLsRef)->des_base_pos.norm_offset.AddThick(-pPlate->handle,TRUE);
				}
			}
		}
		if(pBaseLinePart.IsHasPtr()&&pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
		{	//基准构件为钢管时应在设计钢板前设定焊接父构件，否则不能正确标出对活点
			pPlate->m_bWeldPart=TRUE;
			pPlate->m_hPartWeldParent=pBaseLinePart->handle;
			pPlate->designInfo.iProfileStyle0123=1;
			pPlate->m_fNormOffset = -0.5*pPlate->GetThick();
			if(pPlate->designInfo.iWeldLineStyle==1)
			{
				pPlate->designInfo.weld_start_high = weld_start_high;
				pPlate->designInfo.weld_end_high = weld_end_high;
			}
			pPlate->designInfo.weld_width = fWeldWidth;
			pPlate->DesignPlate();
		}
		else if(pBaseLinePart.IsHasPtr()&&pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{
			pPlate->designInfo.iProfileStyle0123=1;
			pPlate->m_fNormOffset=0;
			if(pPlate->face_N>=2)
			{
				pPlate->HuoQuLine[0].startPt=pPlate->designInfo.huoquline_start[0].Position();
				pPlate->HuoQuLine[0].endPt=pPlate->designInfo.huoquline_end[0].Position();
				coord_trans(pPlate->HuoQuLine[0].startPt,pPlate->ucs,FALSE,TRUE);
				coord_trans(pPlate->HuoQuLine[0].endPt,pPlate->ucs,FALSE,TRUE);
				pPlate->HuoQuLine[0].startPt.z=pPlate->HuoQuLine[0].endPt.z=0;
				pPlate->designInfo.m_bSelfCorrectBendline=workplane.m_bSelfCorrectBendline;
			}
			pPlate->DesignPlate();
			//找出不在钢板内的螺栓，将关联的杆件设置为两端轮廓
			int nSize=0;
			DYN_ARRAY<CLDSPlate::EDGEBOLT> boltInfoArr(pPlate->GetLsCount());
			if(!pPlate->ValidateProfile(1.0,boltInfoArr,&nSize)&&nSize>0)
			{
				for(CDesignLjPartPara *pLjPart=pPlate->designInfo.partList.GetFirst();pLjPart;pLjPart=pPlate->designInfo.partList.GetNext())
				{
					CLDSLineAngle *pAngle=(CLDSLineAngle*)console.FromPartHandle(pLjPart->hPart,CLS_LINEANGLE);
					if(pAngle==NULL)
						continue;
					int i=0;
					for(i=0;i<nSize;i++)
					{
						if(pAngle->FindLsByHandle(boltInfoArr[i].hBolt))
							break;
					}
					if(i<nSize)
						pLjPart->angle.bTwoEdge=TRUE;
				}
				pPlate->DesignPlate();
			}
		}
		else	//空板时，需要给一个默认数据
		{
			pPlate->ucs=plateTemp.ucs;
			PROFILE_VERTEX *pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(-50,-50,0));
			pVertex->vertex.feature=1;
			pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(50,-50,0));
			pVertex->vertex.feature=1;
			pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(50,50,0));
			pVertex->vertex.feature=1;
			pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(-50,50,0));
			pVertex->vertex.feature=1;
		}
		if(pBaseLinePart.IsHasPtr()&&pPlate->iSeg==0)
			pPlate->iSeg=pBaseLinePart->iSeg;
		
		//装配定位杆件为钢管时,连板与定位杆件间需要焊接,同一节点上的焊接连接板必须保持一致的焊缝线
		if(bAdjustWeldLen&&pBaseLinePart.IsHasPtr()&&pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
			CDesignJdb::AdjustPlateWeldLenOnSameNode(pBaseLinePart,pBaseNode);
		else
		{
			pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		}
		CLDSModule* pActiveModule=Ta.GetActiveModule();
		for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{	//重新设计并生成螺栓
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			PARTSET partset;
			GetBoltActiveRelaPartSet(pBolt,partset,pActiveModule);
			pBolt->des_base_pos.norm_offset.key_str=CLDSBolt::EmendZOffsetStr(pBolt,partset);
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();
			pBolt->SetModified();
			pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
		g_pSolidDraw->Draw();
		Ta.EndUndoListen();
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	return 0;
}


void CLDSView::OnDesignElbowPlate() 
{
	m_nPrevCommandID=ID_DESIGN_ELBOW_PLATE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat tube's elbow plate";
#else
	m_sPrevCommandName="重复钢管肘板";
#endif
	Command("ElbowPlate");
}

int CLDSView::DesignElbowPlate()
{	//肘板	
	f3dPoint plate_origin;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	UCS_STRU object_ucs,ucs;
	CString cmdStr,promptStr;
	CLDSPart *pPart=NULL;
	CLDSGeneralPlate* pDatumPlate=NULL;
	CLDSLineTube* pDatumTube=NULL;
	g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("ElbowPlate please choose elbow plate's location datum tube:","");
#else
	pCmdLine->FillCmdLine("ElbowPlate 请选择肘板的定位基准钢管:","");
#endif
	g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_LINETUBE));
	while(1)
	{
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			long *id_arr;
			if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
			{
				pPart=console.FromPartHandle(id_arr[0]);
				if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
				{
					pDatumTube=(CLDSLineTube*)pPart;
					break;
				}
			}
			if(pDatumTube==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("No suitable parts selected, please select elbow plate's location datum tube again:","");
#else
				pCmdLine->FillCmdLine("没有选中合适的构件，请重新选择肘板的定位基准钢管:","");
#endif
				g_pSolidDraw->ReleaseSnapStatus();
				continue;
			}
		}
		else
		{
			g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
			return 0;
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("please elbow plate's location datum plate:","");
#else
	pCmdLine->FillCmdLine("请选择肘板的定位基准钢板:","");
#endif
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE));
	while(1)
	{
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			long *id_arr;
			if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
			{
				pPart=console.FromPartHandle(id_arr[0]);
				if(pPart&&pPart->GetClassTypeId()==CLS_PLATE||pPart&&pPart->GetClassTypeId()==CLS_PARAMPLATE)
				{
					pDatumPlate=(CLDSGeneralPlate*)pPart;
					break;
				}
			}
			if(pDatumPlate==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("No suitable parts selected, please select elbow plate's location datum plate:","");
#else
				pCmdLine->FillCmdLine("没有选中合适的构件，请选择肘板的定位基准钢板:","");
#endif
				g_pSolidDraw->ReleaseSnapStatus();
				continue;
			}
		}
		else
		{
			g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
			return 0;
		}
	}
	g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	Ta.BeginUndoListen();
	//static CAddNewElbowPlateDlg elbowPlateDlg;
	//elbowPlateDlg.m_pDatumTube=pDatumTube;
	//elbowPlateDlg.m_pDatumPlate=pDatumPlate;
	CDesignRibbedPlateDlg elbowPlateDlg;
	elbowPlateDlg.m_pDatumPart=pDatumTube;
	elbowPlateDlg.m_pRelaPart1=pDatumPlate;
	if(elbowPlateDlg.DoModal()==IDOK)
	{
		m_pDoc->SetModifiedFlag();
		Ta.EndUndoListen();
		DesignElbowPlate();//Command("ElbowPlate");
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("command:","");
#else
		pCmdLine->FillCmdLine("命令:","");
#endif
	return 0;
}

void CLDSView::OnDesignRibPlate()	
{	//添加肋板
	m_nPrevCommandID=ID_DESIGN_RIB_PLATE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat rib plate";
#else
	m_sPrevCommandName="重复肋板";
#endif
	//Command("RibPlate");
	RibPlate();
}

//设计角钢肋板
CLDSParamPlate* CLDSView::DesignLineAngleRibPlate(CLDSLineAngle *pLineAngle,CLDSDbObject *pObj)
{
	CDesLineAngleRibPlateDlg lineAngleRibPlateDlg;
	lineAngleRibPlateDlg.m_pDatumAngle=pLineAngle;
	if(pObj->GetClassTypeId()==CLS_NODE)
		lineAngleRibPlateDlg.m_pDatumNode=(CLDSNode*)pObj;
	else if(pObj->GetClassTypeId()==CLS_BOLT)
		lineAngleRibPlateDlg.m_pDatumBolt=(CLDSBolt*)pObj;
	CLDSParamPlate* pRibPlate=NULL;
	if(lineAngleRibPlateDlg.DoModal()==IDOK)
		pRibPlate=lineAngleRibPlateDlg.CreateRibPlate();
	return pRibPlate;	
}

//设计钢管夹板
CLDSGeneralPlate* CLDSView::DesignTubeBetweenPlate(CLDSLineTube *pDatumTube1,CLDSLineTube *pDatumTube2)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	f3dPoint datumIntersPos;
	f3dLine datumLine1(pDatumTube1->pStart->Position(true),pDatumTube1->pEnd->Position(true)),
		datumLine2(pDatumTube2->pStart->Position(true),pDatumTube2->pEnd->Position(true));
	int ret=Int3dll(datumLine1,datumLine2,datumIntersPos);
	if(ret==0||ret==-1)
	{
		pCmdLine->CancelCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Two datum steel tubes have no intersection point, unable to generate tube between-plate!");
#else
		AfxMessageBox("两基准钢管无交点，无法生成钢管夹板！");
#endif
		return FALSE;
	}
	//选择钢管夹板的第一个定位节点
	CLDSNode *pDatumNode1=NULL,*pDatumNode2=NULL;
	CString cmdStr,errCmdStr;
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verify(OBJPROVIDER::LINESPACE,SNAP_POINT);
	verify.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
	if(pDatumNode1==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("TubeBetweenPlate Please select locating end node of the the first tube between-plate:");
#else
		cmdStr.Format("TubeBetweenPlate 请选择第一根钢管夹板定位端节点:");
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		CDisplayNodeAtFrontLife displayNode;
		displayNode.DisplayNodeAtFront();
		while(1)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verify)<0)
			{
				pCmdLine->CancelCmdLine();
				return NULL; 
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			pDatumNode1=Ta.Node.FromHandle(obj.hRelaObj);
			if(pDatumNode1==NULL||(pDatumNode1!=pDatumTube1->pStart&&pDatumNode1!=pDatumTube1->pEnd))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				errCmdStr.Format("TubeBetweenPlate node selected is not the end node of the first tube, please reselect locating end node of the the first tube between-plate::");
#else
				errCmdStr.Format("TubeBetweenPlate 选中节点不是第一根钢管的端节点,请重新选择第一根钢管夹板定位端节点:");
#endif
				pCmdLine->FillCmdLine(errCmdStr,"");
			}
			else
				break;
		}
	}
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pDatumNode1->handle));
	g_pSolidDraw->SetEntSnapStatus(pDatumNode1->handle);
	//选择钢管夹板的第二个定位节点
	if(pDatumNode2==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("TubeBetweenPlate Please select locating end node of the the first tube between-plate:");
#else
		cmdStr.Format("TubeBetweenPlate 请选择第二根钢管夹板定位端节点:");
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		CDisplayNodeAtFrontLife displayNode;
		displayNode.DisplayNodeAtFront();
		while(1)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verify)<0)
			{
				pCmdLine->CancelCmdLine();
				return NULL; 
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			pDatumNode2=Ta.Node.FromHandle(obj.hRelaObj);
			if(pDatumNode2==NULL||(pDatumNode2!=pDatumTube2->pStart&&pDatumNode2!=pDatumTube2->pEnd))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				errCmdStr.Format("TubeBetweenPlate node selected is not the end node of the second tube, please reselect locating end node of the the second tube between-plate:");
#else
				errCmdStr.Format("TubeBetweenPlate 选中节点不是第二根钢管的端节点,请重新选择第二根钢管夹板定位端节点:");
#endif
				pCmdLine->FillCmdLine(errCmdStr,"");
			}
			else
				break;
		}
	}
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pDatumNode2->handle));
	g_pSolidDraw->SetEntSnapStatus(pDatumNode2->handle);
	//
	CDesignTubeBetweenPlateDlg plate_dlg;
	if(plate_dlg.DoModal()!=IDOK)
	{
		pCmdLine->CancelCmdLine();
		return FALSE;
	}
	CUndoOperObject undo(&Ta,false);
	CLDSPlate *pBetweenPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	pBetweenPlate->CopyModuleInstanceInfo(pDatumNode1);	//调整钢板配材号与基准构件或基准节点配材号一致
	pBetweenPlate->designInfo.m_hBaseNode=pDatumNode1->handle;	//板的基准节点
	pBetweenPlate->jdb_style=3;	//钢管夹板
	pBetweenPlate->m_hPartWeldParent=pDatumTube1->handle;
	pBetweenPlate->Thick=ftoi(plate_dlg.m_fPlateThick);
	pBetweenPlate->cMaterial=CSteelMatLibrary::RecordAt(plate_dlg.m_iMaterial).cBriefMark;
	pBetweenPlate->SetPartNo(plate_dlg.m_sPartNo.GetBuffer(0));
	pBetweenPlate->iSeg=SEGI(plate_dlg.m_sSegI.GetBuffer());
	pBetweenPlate->m_fNormOffset=-0.5*pBetweenPlate->GetThick();
	pBetweenPlate->ucs.origin=datumIntersPos;
	f3dPoint gradient_line_vec,gradient_line_norm,gradient_line_norm1,gradient_line_norm2;
	if(pDatumNode1==pDatumTube1->pStart)
		pBetweenPlate->ucs.axis_x=datumLine1.startPt-datumLine1.endPt;
	else
		pBetweenPlate->ucs.axis_x=datumLine1.endPt-datumLine1.startPt;
	if(pDatumNode2==pDatumTube2->pStart)
		pBetweenPlate->ucs.axis_y=datumLine2.startPt-datumLine2.endPt;
	else
		pBetweenPlate->ucs.axis_y=datumLine2.endPt-datumLine2.startPt;
	gradient_line_vec=pBetweenPlate->ucs.axis_y;
	normalize(pBetweenPlate->ucs.axis_x);
	pBetweenPlate->ucs.axis_z=pBetweenPlate->ucs.axis_x^pBetweenPlate->ucs.axis_y;
	normalize(pBetweenPlate->ucs.axis_z);
	pBetweenPlate->ucs.axis_y=pBetweenPlate->ucs.axis_z^pBetweenPlate->ucs.axis_x;
	normalize(pBetweenPlate->ucs.axis_y);
	gradient_line_norm=gradient_line_vec^pBetweenPlate->ucs.axis_z;
	normalize(gradient_line_vec);
	normalize(gradient_line_norm);
	//两基准钢管实体是否存在交点
	gradient_line_norm1=pBetweenPlate->ucs.axis_y;
	gradient_line_norm2=gradient_line_norm;
	f3dPoint near_norm1,near_norm2,interPos;
	near_norm1=pDatumNode2->Position(true)-pDatumNode1->Position(true);
	near_norm2=pDatumNode1->Position(true)-pDatumNode2->Position(true);
	normalize(near_norm1);
	normalize(near_norm2);
	if(gradient_line_norm1*near_norm1<0)
		gradient_line_norm1*=-1.0;
	if(gradient_line_norm2*near_norm2<0)
		gradient_line_norm2*=-1.0;
	BOOL bHasInterPos=TRUE;
	bHasInterPos=plate_dlg.m_bHasInterPos;
	CLDSSphere *pBaseSphere=NULL;
	CLDSLineTube *pBaseTube=NULL;
	if(!bHasInterPos)
	{	//两基准钢管实体无交点
		long datum_handle=0;
		sscanf(plate_dlg.m_sTransectPartHandle,"%X",&datum_handle);
		CLDSPart *pDatumPart=console.FromPartHandle(datum_handle);
		if(pDatumPart&&pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			pBaseTube=(CLDSLineTube*)pDatumPart;
		else if(pDatumPart&&pDatumPart->GetClassTypeId()==CLS_SPHERE)
			pBaseSphere=(CLDSSphere*)pDatumPart;
	}
	else	
	{	//两基准钢管实体有交点
		pBaseTube=NULL;
		pBaseSphere=NULL;
	}
	f3dPoint inner_arc_start,inner_arc_end;
	if(pBaseTube)
	{	//钢管实体无交点时求两基准钢管与主钢管的内侧交点
		f3dPoint vertice;
		double radius=0.5*pBaseTube->GetDiameter();
		f3dPoint line_pick=pDatumTube1->Start()+0.5*pDatumTube1->GetDiameter()*gradient_line_norm1;
		project_point(line_pick,pBetweenPlate->ucs.origin,pBetweenPlate->ucs.axis_z);
		Int3dlc(&inner_arc_end,&vertice,line_pick,pBetweenPlate->ucs.axis_x,pBaseTube->Start(),pBaseTube->End(),radius,radius);
		f3dPoint vertice_vec=inner_arc_end-datumIntersPos;
		if(vertice_vec*pBetweenPlate->ucs.axis_x<0)
			inner_arc_end=vertice;
		line_pick=pDatumTube2->Start()+0.5*pDatumTube2->GetDiameter()*gradient_line_norm2;
		project_point(line_pick,pBetweenPlate->ucs.origin,pBetweenPlate->ucs.axis_z);
		Int3dlc(&inner_arc_start,&vertice,line_pick,gradient_line_vec,pBaseTube->Start(),pBaseTube->End(),radius,radius);
		vertice_vec=inner_arc_start-datumIntersPos;
		if(vertice_vec*gradient_line_vec<0)
			inner_arc_start=vertice;
		pBetweenPlate->m_hPartWeldParent=pBaseTube->handle;
	}
	else if(pBaseSphere)
	{	//钢管实体无交点时求两基准钢管与基准球体的内侧交点
		UCS_STRU ucs;
		ucs.origin=datumIntersPos;
		ucs.axis_x=pBetweenPlate->ucs.axis_x;
		ucs.axis_y=gradient_line_norm1;
		ucs.axis_z=ucs.axis_x^ucs.axis_y;
		normalize(ucs.axis_z);
		double r=pDatumTube1->GetDiameter()*0.5;
		double pos_x=sqrt(pBaseSphere->D*pBaseSphere->D*0.5*0.5-r*r);
		inner_arc_end.Set(pos_x,r,0);
		coord_trans(inner_arc_end,ucs,TRUE);
		ucs.axis_x=gradient_line_vec;
		ucs.axis_y=gradient_line_norm2;
		ucs.axis_z=ucs.axis_x^ucs.axis_y;
		normalize(ucs.axis_z);
		r=pDatumTube2->GetDiameter()*0.5;
		pos_x=sqrt(pBaseSphere->D*pBaseSphere->D*0.5*0.5-r*r);
		inner_arc_start.Set(pos_x,r,0);
		coord_trans(inner_arc_start,ucs,TRUE);
		pBetweenPlate->m_hPartWeldParent=pBaseSphere->handle;
	}
	double alfa=cal_angle_of_2vec(gradient_line_vec,pBetweenPlate->ucs.axis_x);
	double sina=sin(alfa);
	if(fabs(sina)<EPS)
		sina=1;		//以防出现除0
	pBetweenPlate->ucs.origin=datumIntersPos+
	pBetweenPlate->ucs.axis_x*(pDatumTube2->GetDiameter()*0.5/sina)+gradient_line_vec*(pDatumTube1->GetDiameter()*0.5/sina);
	f3dPoint vertice;
	if(pBaseTube||pBaseSphere)
	{  //钢管实体无交点时添加钢管夹板的第一个顶点
		coord_trans(inner_arc_start,pBetweenPlate->ucs,FALSE);
		coord_trans(inner_arc_end,pBetweenPlate->ucs,FALSE);
		inner_arc_start.z=inner_arc_end.z=0;	
		vertice=inner_arc_end;
	}
	vertice.feature=1;
	PROFILE_VERTEX *pVertex=pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertice));
	pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
	BOOL bSpecifyFirstEdgeLength=FALSE,bSpecifySecondEdgeLength=FALSE;
	double fFirstEdgeLen=0,fSecondEdgeLen=0;
	double fFirstFlWidth=0,fSecondFlWidth=0;
	//是否自动计算与钢管相连边的长度和宽度
	bSpecifyFirstEdgeLength=!plate_dlg.m_bAutoCalcFirst;
	bSpecifySecondEdgeLength=!plate_dlg.m_bAutoCalcSecond;
	if(bSpecifyFirstEdgeLength)
	{	//指定与第一根基准钢管相连边的长度
		fFirstEdgeLen=plate_dlg.m_fFirstEdgeLen;
		fFirstFlWidth=plate_dlg.m_fFirstEdgeWidth;
	}
	if(bSpecifySecondEdgeLength)
	{	//指定与第二根基准钢管相连边的长度
		fSecondEdgeLen=plate_dlg.m_fSecondEdgeLen;
		fSecondFlWidth=plate_dlg.m_fSecondEdgeWidth;
	}
	if(!bSpecifyFirstEdgeLength)
	{
		CLDSParamPlate *pFlPlate=NULL;
		if(pDatumNode1==pDatumTube1->pStart)
		{
			if(pDatumTube1->m_tJointStart.hLinkObj>0x20)
				pFlPlate=(CLDSParamPlate*)console.FromPartHandle(pDatumTube1->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
			f3dPoint vec=pDatumTube1->xPosStart-pBetweenPlate->ucs.origin;
			fFirstEdgeLen=vec*pBetweenPlate->ucs.axis_x+pDatumTube1->startOdd();
		}
		else
		{
			if(pDatumTube1->m_tJointEnd.hLinkObj>0x20)
				pFlPlate=(CLDSParamPlate*)console.FromPartHandle(pDatumTube1->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
			f3dPoint vec=pDatumTube1->xPosEnd-pBetweenPlate->ucs.origin;
			fFirstEdgeLen=vec*pBetweenPlate->ucs.axis_x+pDatumTube1->endOdd();	
		}		
		if(pFlPlate&&(pFlPlate->m_iParamType==TYPE_FL||
			pFlPlate->m_iParamType==TYPE_FLG||
			pFlPlate->m_iParamType==TYPE_FLR))
		{		
			f3dPoint vec=pFlPlate->ucs.origin-pBetweenPlate->ucs.origin;
			fFirstEdgeLen=vec*pBetweenPlate->ucs.axis_x;
			double W;
			pFlPlate->GetDesignItemValue('W', &W);
			fFirstFlWidth=0.5*(W-pDatumTube1->GetDiameter());
		}
		if(pBaseTube||pBaseSphere)	//两钢管实体无交点
			fFirstEdgeLen-=inner_arc_end.x;
	}
	if(!bSpecifySecondEdgeLength)
	{
		CLDSParamPlate *pFlPlate=NULL;
		if(pDatumNode2==pDatumTube2->pStart)
		{
			if(pDatumTube2->m_tJointStart.hLinkObj>0x20)
				pFlPlate=(CLDSParamPlate*)console.FromPartHandle(pDatumTube2->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
			f3dPoint vec=pDatumTube2->xPosStart-pBetweenPlate->ucs.origin;
			fSecondEdgeLen=vec*gradient_line_vec+pDatumTube2->startOdd();
		}
		else
		{
			if(pDatumTube2->m_tJointEnd.hLinkObj>0x20)
				pFlPlate=(CLDSParamPlate*)console.FromPartHandle(pDatumTube2->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
			f3dPoint vec=pDatumTube2->xPosEnd-pBetweenPlate->ucs.origin;
			fSecondEdgeLen=vec*gradient_line_vec+pDatumTube2->endOdd();
		}
		if(pFlPlate&&(pFlPlate->m_iParamType==TYPE_FL||
			pFlPlate->m_iParamType==TYPE_FLG||
			pFlPlate->m_iParamType==TYPE_FLR))
		{
			f3dPoint vec=pFlPlate->ucs.origin-pBetweenPlate->ucs.origin;
			fSecondEdgeLen=vec*gradient_line_vec;
			double W;
			pFlPlate->GetDesignItemValue('W', &W);
			fSecondFlWidth=(W-pDatumTube2->GetDiameter())*0.5;
		}
		if(pBaseTube||pBaseSphere)	//两钢管实体无交点
			fSecondEdgeLen-=inner_arc_start.mod();
	}
	f3dPoint vertex_arr[4];
	double between_angle=cal_angle_of_2vec(pBetweenPlate->ucs.axis_x,gradient_line_vec);
	vector_trans(gradient_line_vec,pBetweenPlate->ucs,FALSE);
	vector_trans(gradient_line_norm2,pBetweenPlate->ucs,FALSE);
	if(pBaseTube||pBaseSphere)//钢管实体无交点时钢管夹板的第二个顶点
		vertex_arr[0]=inner_arc_end.x+fFirstEdgeLen;
	else
		vertex_arr[0].Set(fFirstEdgeLen);
	if(fFirstFlWidth>0)
		vertex_arr[1].Set(vertex_arr[0].x,fFirstFlWidth);
	if(pBaseTube||pBaseSphere)//钢管实体无交点时钢管夹板的倒数第二个顶点
		vertex_arr[2]=inner_arc_start+gradient_line_vec*fSecondEdgeLen;
	else
		vertex_arr[2].Set(gradient_line_vec.x*fSecondEdgeLen,gradient_line_vec.y*fSecondEdgeLen);
	if(fSecondFlWidth>0)
		vertex_arr[3]=vertex_arr[2]+gradient_line_norm2*fSecondFlWidth;	
	f3dPoint inters_line_vec;
	if(between_angle<Pi/2)
	{
		//判断第一根钢管基准边是否过长
		if(fSecondFlWidth>0)
		{
			inters_line_vec=vertex_arr[3]-vertex_arr[2];
			normalize(inters_line_vec);
			if(vertex_arr[3].x-vertex_arr[3].y*inters_line_vec.x/inters_line_vec.y<fFirstEdgeLen)
			{	//第一根钢管基准边过长
				fFirstFlWidth=0;
				SnapPerp(&vertex_arr[0],f3dPoint(0,0,0),f3dPoint(100,0,0),vertex_arr[3]);
			}
		}
		//判断第二根钢管基准边是否过长
		if(fFirstFlWidth>0)
		{
			if(fFirstEdgeLen/gradient_line_vec.x<fSecondEdgeLen)
			{	//第二根钢管基准边过长
				fSecondFlWidth=0;
				SnapPerp(&vertex_arr[2],f3dPoint(0,0,0),gradient_line_vec*100,vertex_arr[1]);
			}
		}
	}
	//生成两小块夹板或一块夹板
	BOOL bOnlyOnePlate=TRUE;
	if(pBaseSphere)
		bOnlyOnePlate=!plate_dlg.m_iOnlyOnePlate;
	//钢管夹板连接射线杆件
	BOOL bLinkLinePart=FALSE;
	if(bOnlyOnePlate)	//仅在生成一整块钢管夹板时才处理连有射线杆件的情况
		bLinkLinePart=plate_dlg.m_bLinkLinePart;
	int n=0;	//初始化为0 否则当没有射线杆件的情况下可能出错
	f3dPoint vertex_array[100];
	if(bLinkLinePart)
	{
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select all the ray rods connected with tube between-plate:","");
#else
		pCmdLine->FillCmdLine("请选择钢管夹板上连接的所有射线杆件:","");
#endif
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_LINETUBE));
		Invalidate(FALSE);
		//OpenWndSel();	//开窗选择杆件
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			long *id_arr;
			n = g_pSolidSnap->GetLastSelectEnts(id_arr);
			for(int i=0;i<n;i++)
			{
				CDesignLjPartPara desLjPartPara;
				CLDSLineTube *pLineTube=NULL;
				CLDSParamPlate *pParamPlate=NULL;
				CLDSPart *pPart=console.FromPartHandle(id_arr[i]);
				if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
				{	//射线杆件为钢管
					pLineTube = (CLDSLineTube*)pPart;
					if(!pLineTube)
						continue;
					f3dPoint tube_vec = pLineTube->End()-pLineTube->Start();
					if(tube_vec*pBetweenPlate->ucs.axis_x>0)
					{	//钢管始端与板连接
						desLjPartPara.start0_end1=0;
						if(pLineTube->m_tJointStart.hLinkObj>0x20)
							pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
					}
					else
					{	//钢管终端与板连接
						desLjPartPara.start0_end1=1;
						if(pLineTube->m_tJointEnd.hLinkObj>0x20)
							pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
					}
					if(pParamPlate&&(pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND))
					{
						double L,H,N;
						pParamPlate->GetDesignItemValue('L',&L);
						pParamPlate->GetDesignItemValue('H',&H);
						pParamPlate->GetDesignItemValue('N',&N);
						if(pParamPlate->m_iParamType==TYPE_ROLLEND)
						{	
							//调整卷边板上螺栓的法向偏移量
							for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
							{
								if((*pLsRef)->des_work_norm.direction==0)	//朝外
									(*pLsRef)->des_base_pos.norm_offset.AddThick(-(int)pBetweenPlate->GetThick());
							}
							pLineTube->DesignEndJointUCS();	//重新设计ROLL板的坐标系
							pLineTube->DesignStartJointUCS();
							pParamPlate->SetModified();
							pParamPlate->DesignPlate();
							pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
							g_pSolidDraw->NewSolidPart(pParamPlate->GetSolidPartObject());
						}
						double minx=L,maxx=0;
						for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
						{
							f3dPoint pos = (*pLsRef)->ucs.origin;
							coord_trans(pos,pParamPlate->ucs,FALSE);
							if(pos.x<minx)
								minx=pos.x;
							if(pos.x>maxx)
								maxx=pos.x;
							//将参数板的螺栓引入到钢管夹板上
							pBetweenPlate->AppendLsRef(*pLsRef);
							pLsRef->GetLsPtr()->AddL0Thick(pBetweenPlate->handle,TRUE);
							pLsRef->GetLsPtr()->CalGuigeAuto();
							(*pLsRef)->SetModified();
							(*pLsRef)->Create3dSolidModel();
							g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
						}
						LSSPACE_STRU lsSpace;
						GetLsSpace(lsSpace,pParamPlate->GetFirstLsRef()->GetLsPtr()->get_d());
						vertex_array[2*i].x=vertex_array[2*i+1].x=minx-lsSpace.EndSpace; //此处控制钢管与板之间的距离
						vertex_array[2*i].y=0.5*H;
						vertex_array[2*i+1].y=-vertex_array[2*i].y;
						coord_trans(vertex_array[2*i],pParamPlate->ucs,TRUE);
						coord_trans(vertex_array[2*i+1],pParamPlate->ucs,TRUE);
						coord_trans(vertex_array[2*i],pBetweenPlate->ucs,FALSE);
						coord_trans(vertex_array[2*i+1],pBetweenPlate->ucs,FALSE);
						vertex_array[2*i].z=vertex_array[2*i+1].z=0;
						desLjPartPara.hPart=pPart->handle;
						desLjPartPara.iFaceNo=1;
						desLjPartPara.m_nClassTypeId=CLS_LINETUBE;
						pBetweenPlate->designInfo.partList.SetValue(pPart->handle,desLjPartPara);
					}
				}
				else if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
				{	//射线杆件为角钢
					CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPart;
					if(fabs(pBetweenPlate->ucs.axis_z*pLineAngle->get_norm_x_wing())>fabs(pBetweenPlate->ucs.axis_z*pLineAngle->get_norm_y_wing()))
					{	//X肢为当前肢
						desLjPartPara.angle.cur_wing_x0_y1=0;
						if(DISTANCE(pLineAngle->Start(),pBetweenPlate->ucs.origin)<DISTANCE(pLineAngle->End(),pBetweenPlate->ucs.origin))
						{	//始端连接
							desLjPartPara.start0_end1=0;
							pLineAngle->desStartPos.wing_x_offset.gStyle=4;
							pLineAngle->desStartPos.wing_x_offset.offsetDist=-pBetweenPlate->GetThick();
						}
						else
						{	//终端连接
							desLjPartPara.start0_end1=1;
							pLineAngle->desEndPos.wing_x_offset.gStyle=4;
							pLineAngle->desEndPos.wing_x_offset.offsetDist=-pBetweenPlate->GetThick();
						}
					}
					else
					{	//Y肢为当前肢
						desLjPartPara.angle.cur_wing_x0_y1=1;
						if(DISTANCE(pLineAngle->Start(),pBetweenPlate->ucs.origin)<DISTANCE(pLineAngle->End(),pBetweenPlate->ucs.origin))
						{	//始端连接
							desLjPartPara.start0_end1=0;
							pLineAngle->desStartPos.wing_y_offset.gStyle=4;
							pLineAngle->desStartPos.wing_y_offset.offsetDist=-pBetweenPlate->GetThick();
						}
						else
						{	//终端连接
							desLjPartPara.start0_end1=0;
							pLineAngle->desEndPos.wing_y_offset.gStyle=4;
							pLineAngle->desEndPos.wing_y_offset.offsetDist=-pBetweenPlate->GetThick();
						}
					}
					pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
					g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
					desLjPartPara.hPart=pPart->handle;
					desLjPartPara.iFaceNo=1;
					desLjPartPara.m_nClassTypeId=CLS_LINEANGLE;
					pBetweenPlate->designInfo.partList.SetValue(pPart->handle,desLjPartPara);
					g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("Please select all the relative bolts on the tube between-plate:","");
#else
					pCmdLine->FillCmdLine("请选择钢管夹板关联的所有螺栓:","");
#endif
					g_pSolidSet->SetDisplayType(DISP_SOLID);
					g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
					Invalidate(FALSE);
					OpenWndSel();	//开窗选择螺栓
					if(pCmdLine->GetStrFromCmdLine(cmdStr))
					{	
						long *id_arr;
						int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
						for(int i=0;i<n;i++)
						{
							CLDSPart *pPart=console.FromPartHandle(id_arr[i]);
							if(pPart&&pPart->GetClassTypeId()==CLS_BOLT)
							{
								pBetweenPlate->AppendLsRef(((CLDSBolt*)pPart)->GetLsRef());
								((CLDSBolt*)pPart)->AddL0Thick(pBetweenPlate->handle,TRUE);
								((CLDSBolt*)pPart)->CalGuigeAuto();
								pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
								g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
							}
						}
						g_pSolidDraw->ReleaseSnapStatus();
					}
					else
					{
						pCmdLine->CancelCmdLine();
						return FALSE;
					}
					OnOperOther();	//取消开窗操作状态
					g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
				}
			}
		}
		else
		{
			pCmdLine->CancelCmdLine();	
			//命令中断，清除冗余数据
			if(pBetweenPlate)
			{
				console.DispPartSet.DeleteNode(pBetweenPlate->handle);
				g_pSolidDraw->DelEnt(pBetweenPlate->handle);
				console.DeletePart(pBetweenPlate->handle);
			}
			return FALSE;
		}
		//OnOperOther();	//取消开窗选择
	}
	vertex_arr[0].feature=1;
	pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertex_arr[0]));
	if(fFirstFlWidth>0)
	{
		vertex_arr[1].feature=1;
		pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertex_arr[1]));
	}
	for(int i=0;i<2*n;i++)
	{	//按顶点坐标Y值从小到大依次加入钢管夹板顶点列表中
		for(int j=i+1;j<2*n;j++)
		{
			f3dPoint vertex;
			if(vertex_array[i].y>vertex_array[j].y)
			{
				vertex = vertex_array[i];
				vertex_array[i] = vertex_array[j];
				vertex_array[j] = vertex;
			}
		}
		if(vertex_array[i].x!=0&&vertex_array[i].y!=0)
		{	//跳过不合理顶点
			vertex_array[i].feature=1;
			pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertex_array[i]));
		}
	}
	
	if(bOnlyOnePlate)
	{	//生成一块板时才需要插入与第二根钢管相关的顶点
		if(fSecondFlWidth>0)
		{
			vertex_arr[3].feature=1;
			pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertex_arr[3]));
		}
		vertex_arr[2].feature=1;
		pVertex=pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertex_arr[2]));
		pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
	}
	UCS_STRU ucs;	//假定的坐标系
	BOOL bMirCreate=FALSE;
	if((pBaseTube||pBaseSphere)&&bOnlyOnePlate)
	{	//生成一整块钢管夹板
		if(pBaseTube)
		{
			f3dPoint len_vec=pBaseTube->End()-pBaseTube->Start();
			normalize(len_vec);
			//相贯钢管与两个基准钢管所在的面垂直
			if(fabs(len_vec*pBetweenPlate->ucs.axis_z)>EPS_COS2)
			{	//钢管实体无交点 添加圆弧起始点
				double R=pBaseTube->GetDiameter()*0.5;
				vertice=inner_arc_start;
				vertice.feature=1;
				pVertex=pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertice));
				if(fabs(pBetweenPlate->ucs.axis_z*pBaseTube->ucs.axis_z)>EPS_COS)	//圆弧
				{	//圆弧
					pVertex->type=2;	//指定半径圆弧,center为相对坐标 wht 12-06-15
					pVertex->center.Set();
					//pVertex->center=datumIntersPos;
					pVertex->radius=-R;
				}
				else
				{	//椭圆弧
					pVertex->type=3;
					pVertex->center=datumIntersPos;
					if(pBetweenPlate->ucs.axis_z*pBaseTube->ucs.axis_z>0)
						pVertex->column_norm=pBaseTube->ucs.axis_z;
					else
						pVertex->column_norm=-pBaseTube->ucs.axis_z;
					pVertex->radius=-R;
				}
				pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
			}
			else if(fabs(len_vec*pBetweenPlate->ucs.axis_z)<EPS)
			{	//相贯钢管与两个基准钢管共面
				vertice=inner_arc_start;
				vertice.feature=1;
				pVertex=pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertice));
				pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
			}
			else	//其他情况未处理
				return FALSE;
		}
		else if(pBaseSphere)
		{	//圆弧起点
			double R=pBaseSphere->D*0.5;
			vertice=inner_arc_start;
			vertice.feature=1;
			pVertex=pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertice));
			pVertex->type=1;
			pVertex->center=datumIntersPos;
			pVertex->radius=-R;
			PROFILE_VERTEX *pPreVertex=pBetweenPlate->vertex_list.GetFirst();
			double half_len = 0.5*DISTANCE(pVertex->vertex,pPreVertex->vertex);
			if(fabs(half_len/pVertex->radius)<=1)
				pVertex->sector_angle = 2*asin(half_len/pVertex->radius);
			pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
		}
	}
	else if(pBaseSphere&&!bOnlyOnePlate)
	{	//生成两小块钢管夹板的情况
		ucs=pBetweenPlate->ucs;
		CLDSNode *pBaseNode=console.FromNodeHandle(pBaseSphere->hDatumNode);
		if(pBaseNode)
			ucs.origin=pBaseNode->Position(true);
		else
			ucs.origin=pBaseSphere->ucs.origin;
		//求切点
		double fWidth=0;
		CString ss;
		double r=pBaseSphere->D*0.5;
		f2dPoint retPt,pickPt;
		f2dCircle cir(r,f2dPoint(0,0));
		PROFILE_VERTEX *pTailVertex=pBetweenPlate->vertex_list.GetTail();
		f3dPoint vertice=pTailVertex->vertex;
		coord_trans(vertice,pBetweenPlate->ucs,TRUE);
		coord_trans(vertice,ucs,FALSE);
		pickPt.Set(vertice.x,vertice.y+100);
		if(Tang2dpc(f2dPoint(vertice.x,vertice.y),cir,pickPt,retPt))					
			vertice.Set(retPt.x,retPt.y,0);
		else
		{
			pCmdLine->CancelCmdLine();	
			//命令中断，清除冗余数据
			if(pBetweenPlate)
			{
				console.DispPartSet.DeleteNode(pBetweenPlate->handle);
				g_pSolidDraw->DelEnt(pBetweenPlate->handle);
				console.DeletePart(pBetweenPlate->handle);
			}
			return FALSE;
		}
		//默认为相切宽度
		if(plate_dlg.m_bUseTangWidth)
			fWidth=vertice.y;
		else	
			fWidth=plate_dlg.m_fTangentWidth;	//使用用户输入的宽度
		f3dPoint vertex1,vertex2;
		if(fWidth==vertice.y)
			vertex1=vertice;
		else if(fWidth<vertice.y)
		{
			double x=sqrt(r*r-fWidth*fWidth);
			vertex1.Set(x,fWidth,0);
		}
		else if(fWidth>r)
		{
			vertex1=vertice;
			vertex2.Set(vertice.x,fWidth,0);
		}
		coord_trans(vertex1,ucs,TRUE);
		coord_trans(vertex2,ucs,TRUE);
		coord_trans(vertex1,pBetweenPlate->ucs,FALSE);
		coord_trans(vertex2,pBetweenPlate->ucs,FALSE);
		vertex1.feature=vertex2.feature=1;
		if(fWidth>vertice.y)	
			pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertex2));
		PROFILE_VERTEX *pVertex=pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertex1));
		pVertex->type=1;
		pVertex->center=ucs.origin;
		pVertex->radius=-r;
		PROFILE_VERTEX *pPreVertex=pBetweenPlate->vertex_list.GetFirst();
		double half_len = 0.5*DISTANCE(pVertex->vertex,pPreVertex->vertex);
		if(fabs(half_len/pVertex->radius)<=1)
			pVertex->sector_angle = 2*asin(half_len/pVertex->radius);
		pVertex->m_bWeldEdge=TRUE;
		//同是生成钢管外侧的钢板
		bMirCreate=plate_dlg.m_bMirCreate;
	}
	pBetweenPlate->m_hPartWeldParent=pDatumTube1->handle;
	//将与钢管夹板相关的构件添加到设计参数列表中
	CDesignLjPartPara desLjPartPara;
	desLjPartPara.hPart=pDatumTube1->handle;
	desLjPartPara.iFaceNo=1;
	desLjPartPara.m_nClassTypeId=CLS_LINETUBE;
	pBetweenPlate->designInfo.partList.SetValue(pDatumTube1->handle,desLjPartPara);
	desLjPartPara.hPart=pDatumTube2->handle;
	pBetweenPlate->designInfo.partList.SetValue(pDatumTube2->handle,desLjPartPara);
	if(pBaseTube)
	{
		desLjPartPara.hPart=pBaseTube->handle;
		pBetweenPlate->designInfo.partList.SetValue(pBaseTube->handle,desLjPartPara);
	}
	else if(pBaseSphere)
	{
		desLjPartPara.hPart=pBaseSphere->handle;
		desLjPartPara.m_nClassTypeId=CLS_SPHERE;
		pBetweenPlate->designInfo.partList.SetValue(pBaseSphere->handle,desLjPartPara);
	}
	pBetweenPlate->SetModified();
	if(pBaseSphere&&!bOnlyOnePlate&&bMirCreate)
	{
		CLDSPlate *pMirPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
		pMirPlate->CopyProperty(pBetweenPlate);
		f3dPoint origin=pBetweenPlate->ucs.origin;
		coord_trans(origin,ucs,FALSE);
		ucs.axis_y*=-1.0;
		ucs.axis_z*=-1.0;
		coord_trans(origin,ucs,TRUE);
		pMirPlate->ucs=ucs;
		pMirPlate->ucs.origin=origin;
		for(PROFILE_VERTEX *pVertex=pBetweenPlate->vertex_list.GetFirst();pVertex;pVertex=pBetweenPlate->vertex_list.GetNext())
			pMirPlate->vertex_list.append(*pVertex);
		CDesignLjPartPara desLjPartPara;
		//将与钢管夹板相关的构件添加到设计参数列表中
		desLjPartPara.hPart=pDatumTube1->handle;
		desLjPartPara.iFaceNo=1;
		desLjPartPara.m_nClassTypeId=CLS_LINETUBE;
		pMirPlate->designInfo.partList.SetValue(pDatumTube1->handle,desLjPartPara);
		desLjPartPara.hPart=pBaseSphere->handle;
		desLjPartPara.m_nClassTypeId=CLS_SPHERE;
		pMirPlate->designInfo.partList.SetValue(pBaseSphere->handle,desLjPartPara);
		pMirPlate->SetModified();
		pMirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pMirPlate->GetSolidPartObject());
	}
	m_pDoc->SetModifiedFlag();
	return pBetweenPlate;
}

bool InitRibPlateOffsetDistAndLen(CLDSGeneralPlate *pDatumPlate,CLDSParamPlate *pRibPlate,
							   ANCHOR_PARAM anchors[4],double distArr[4],double *pfLen)
{
	int nValidAnchor=0;
	for(int i=0;i<4;i++)
	{
		if(anchors[i].ciType!=1&&anchors[i].ciType!=2&&anchors[i].ciType!=3)
			break;
		else
			nValidAnchor++;
	}
	if(nValidAnchor==2)
	{	//
		if(anchors[0].ciType==anchors[1].ciType)
		{	
			if(anchors[0].ciType==3)
			{	//选中两个轮廓点
				GEPOINT vertex1(anchors[0].locate.vertex.x,anchors[0].locate.vertex.y),
						vertex2(anchors[1].locate.vertex.x,anchors[1].locate.vertex.y);
				if(pfLen)
					*pfLen=ftoi(DISTANCE(vertex1,vertex2));
				int n=pDatumPlate->vertex_list.GetNodeNum();
				int prev_id=(anchors[0].locate.vertex.idVertex-1+n-1)%n+1;	//前一轮廓点id
				int next_id=(anchors[0].locate.vertex.idVertex-1+1)%n+1;	//后一轮廓点id
				if(next_id==anchors[1].locate.vertex.idVertex)
					distArr[1]=-1*ftoi(pRibPlate->thick*2.5);
				else if(prev_id==anchors[1].locate.vertex.idVertex)
					distArr[1]=ftoi(pRibPlate->thick*2.5);
			}
			else if(anchors[0].ciType==2)
			{	//选中两个螺栓
				CLDSBolt *pBolt1=(CLDSBolt*)pDatumPlate->BelongModel()->FromPartHandle(anchors[0].locate.hBolt,CLS_BOLT);
				CLDSBolt *pBolt2=(CLDSBolt*)pDatumPlate->BelongModel()->FromPartHandle(anchors[1].locate.hBolt,CLS_BOLT);
				if(pBolt1)
				{
					LSSPACE_STRU lsSpace;
					GetLsSpace(lsSpace,pBolt1->get_d());
					distArr[0]=-1*lsSpace.EndSpace;
					distArr[1]=ftoi(lsSpace.EndSpace-pRibPlate->thick*0.5);
					if(pfLen&&pBolt2)
					{
						GEPOINT ls_pos1,ls_pos2;
						pDatumPlate->SpreadLsPos(pBolt1,ls_pos1);
						pDatumPlate->SpreadLsPos(pBolt2,ls_pos2);
						*pfLen=ftoi(DISTANCE(ls_pos1,ls_pos2))+2*lsSpace.EndSpace;
					}
					CDesignLjPartPara *pLjPara=NULL;
					if(pDatumPlate&&pDatumPlate->GetClassTypeId()==CLS_PLATE)
						pLjPara=((CLDSPlate*)pDatumPlate)->GetLjRodPara(pBolt1->des_base_pos.hPart);
					if(pLjPara&&pBolt2&&pLjPara->hPart==pBolt2->des_base_pos.hPart)
					{	//默认靠楞线侧
						CLDSLineAngle *pAngle=(CLDSLineAngle*)pDatumPlate->BelongModel()->FromPartHandle(pLjPara->hPart,CLS_LINEANGLE);
						if(pAngle)
						{
							GEPOINT work_wing_vec=pLjPara->angle.cur_wing_x0_y1==0?pAngle->GetWingVecX():pAngle->GetWingVecY();
							if(work_wing_vec*pRibPlate->ucs.axis_z>0)
								distArr[1]=ftoi(lsSpace.EndSpace-pRibPlate->thick*0.5)*-1;
						}
					}
				}
			}
		}
		else if((anchors[0].ciType==2||anchors[0].ciType==3)&&anchors[1].ciType==1)
		{	//选中定位螺栓或轮廓点，选择定向杆件
			CLDSLineAngle *pAngle=(CLDSLineAngle*)pDatumPlate->BelongModel()->FromPartHandle(anchors[1].locate.hVecRod,CLS_LINEANGLE);
			if(pAngle==NULL)
				return false;
			CDesignLjPartPara *pLjPara=NULL;
			if(pDatumPlate->IsPlate())
				pLjPara=((CLDSPlate*)pDatumPlate)->GetLjRodPara(pAngle->handle);
			if(pLjPara==NULL)
				return false;
			if(anchors[0].ciType==2)
			{	//选择定位螺栓
				GEPOINT ls_pos;
				CMinDouble minZBolt;
				CMaxDouble maxZBolt;
				for(CLsRef *pLsRef=pDatumPlate->GetFirstLsRef();pLsRef;pLsRef=pDatumPlate->GetNextLsRef())
				{
					if(pAngle->FindLsByHandle((*pLsRef)->handle)==NULL)
						continue;
					CLDSBolt* pBolt=pLsRef->GetLsPtr();
					pDatumPlate->SpreadLsPos(pBolt,ls_pos);
					coord_trans(ls_pos,pAngle->ucs,FALSE);
					minZBolt.Update(ls_pos.z,pBolt);
					maxZBolt.Update(ls_pos.z,pBolt);
				}
			}
			else if(anchors[0].ciType==3)
			{	//选择定位点

			}
		}
	}
	else if(nValidAnchor==3)
	{	//

	}
	else if(nValidAnchor==4)
	{	//

	}
	return true;
}
int CLDSView::RibPlate()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr,promptStr;
	CLDSNode *pStartNode=NULL,*pEndNode=NULL;
	CLDSPart *pPart=NULL;
	CLDSNode *pDatumNode=NULL;
	CLDSBolt *pDatumBolt=NULL;
	CSuperSmartPtr<CLDSPart> pDatumPart,pRelaPart1,pRelaPart2,pVecPart;
	CLogErrorLife errorlife;
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	long *id_arr=NULL,retcode=0;
	//切换到实体显示模式
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	g_pSolidDraw->ReleaseSnapStatus();
	//拾取肋板的定位基准构件
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,
		GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE)|
		GetSingleWord(SELECTINDEX_LINETUBE)|GetSingleWord(SELECTINDEX_LINEANGLE));
	DWORD dwhObj=0,dwExportFlag=0;
	pCmdLine->FillCmdLine("RibPlate 请选择肋板的底边定位基准构件(钢管、角钢或钢板):","");
	while(pDatumPart.IsNULL())
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<=0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		dwhObj=obj.hRelaObj;	//dwhObj不一定是选中对象的句柄标识，需要转换
		if(dwhObj==0&&g_pSolidSnap->GetLastSelectEnts(id_arr)>0)
			dwhObj=id_arr[0];
		pDatumPart=Ta.Parts.FromHandle(dwhObj);
	}
	if(pDatumPart->IsPlate())
		pCmdLine->FinishCmdLine(CXhChar16("钢板0x%X",pDatumPart->handle));
	else if(pDatumPart->GetClassTypeId()==CLS_LINETUBE)
		pCmdLine->FinishCmdLine(CXhChar16("钢管0x%X",pDatumPart->handle));
	else if(pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
		pCmdLine->FinishCmdLine(CXhChar16("角钢0x%X",pDatumPart->handle));
	else
		pCmdLine->FinishCmdLine(CXhChar16("构件0x%X",pDatumPart->handle));
	g_pSolidDraw->SetEntSnapStatus(pDatumPart->handle);
	//拾取肋板的关联构件1
	if(!pDatumPart->IsAngle())
	{
		CSysPromptObject prompt;
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,0);	//清空原有捕捉状态
		CDisplayNodeAtFrontLife showNodes;
		if(pDatumPart->IsTube())	//钢管
		{
			//prompt.SetPromptPicture();
			//加载符合条件的肋板简图及详图到提示框中
			//钢管竖板:	B.钢管+基准节点
			//  角肋板: E.钢管+辅助钢板(旧:钢管肘板)
			//			F.钢管+钢管(旧:钢管夹板)	
			//  槽肋板: H.钢管+2个辅助钢板
			verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prPoint);
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINETUBE)|
				GetSingleWord(SELECTINDEX_PARAMPLATE)|GetSingleWord(SELECTINDEX_PLATE));
			pCmdLine->FillCmdLine("RibPlate 请选择侧向靠接钢管、钢板或竖向肋板的定位节点:","");
			showNodes.DisplayNodeAtFront();
		}
		else if(pDatumPart->IsPlate())
		{
			//prompt.SetPromptPicture();
			//加载符合条件的肋板简图及详图到提示框中
			//钢板肋板: C.钢板+基准螺栓+方向构件
			//  角肋板: D.钢板+辅助钢板+螺栓定位
			//          E.钢管+辅助钢板(旧:钢管肘板)
			//  槽肋板: G.钢板+2个辅助钢板+螺栓定位
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINETUBE)|
				GetSingleWord(SELECTINDEX_PARAMPLATE)|GetSingleWord(SELECTINDEX_PLATE));
			pCmdLine->FillCmdLine("RibPlate 请选择侧向靠接钢管或钢板，无侧向靠接肋板请回车:","");
		}
		//g_pSolidDraw->ReleaseSnapStatus();
		while(true)
		{
			if((retcode=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;	//dwhObj不一定是选中对象的句柄标识，需要转换
			if(dwhObj==0&&g_pSolidSnap->GetLastSelectEnts(id_arr)>0)
				dwhObj=id_arr[0];
			if(dwhObj==pDatumPart->handle)
			{
				g_pSolidDraw->ReleaseSnapStatus();
				dwhObj=0;
			}
			pDatumNode=obj.provider==OBJPROVIDER::LINESPACE?Ta.Node.FromHandle(dwhObj):NULL;
			pRelaPart1=pDatumNode==NULL?Ta.Parts.FromHandle(dwhObj):NULL;
			//else if(dwhObj==0&&cmdValue.GetLength()>0)
			//	dwhObj=HexStrToLong(cmdValue);
			if(pDatumNode!=NULL || pRelaPart1.IsHasPtr() || 
				(pDatumPart->IsPlate()&&obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN))
				break;	//已选中依赖杆件或回车确认不需人依赖杆件
		}
		g_pSolidDraw->SetEntSnapStatus(pDatumPart->handle,false);
		if(pRelaPart1.IsHasPtr())
		{
			if(pRelaPart1->IsPlate())
				pCmdLine->FinishCmdLine(CXhChar16("钢板0x%X",pRelaPart1->handle));
			else if(pRelaPart1->GetClassTypeId()==CLS_LINETUBE)
				pCmdLine->FinishCmdLine(CXhChar16("钢管0x%X",pRelaPart1->handle));
			else
				pCmdLine->FinishCmdLine(CXhChar16("构件0x%X",pRelaPart1->handle));
			g_pSolidDraw->SetEntSnapStatus(pRelaPart1->handle);
		}
		else if(pDatumNode!=NULL)
		{
			pCmdLine->FinishCmdLine(CXhChar16("节点0x%X",pDatumNode->handle));
			g_pSolidDraw->SetEntSnapStatus(pDatumNode->handle);
		}
		verifier.SetVerifyFlag(OBJPROVIDER::LINESPACE,0);
	}
	//拾取肋板的关联构件2
	if(pRelaPart1.IsHasPtr()&&pRelaPart1->IsPlate()
		&&(pDatumPart->IsPlate()||pDatumPart->GetClassTypeId()==CLS_LINETUBE))
	{	//槽型肋板
		long* id_arr=NULL,retcode=0;
		DWORD dwhObj=0,dwExportFlag=0;
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,
			GetSingleWord(SELECTINDEX_PARAMPLATE)|GetSingleWord(SELECTINDEX_PLATE));
		pCmdLine->FillCmdLine("RibPlate 请选择槽型肋板的另一侧靠接钢板，单侧靠接角肋板请回车:","");
		while(true)
		{
			if((retcode=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;	//dwhObj不一定是选中对象的句柄标识，需要转换
			pRelaPart2=Ta.Parts.FromHandle(dwhObj);
			if(pRelaPart2.IsHasPtr() || obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
				break;	//已选中依赖杆件或回车确认不需人依赖杆件
		}
		if(pRelaPart2.IsHasPtr())
			pCmdLine->FinishCmdLine(CXhChar16("钢板0x%X",pRelaPart2->handle));
		else
			pCmdLine->FinishCmdLine("<回车>");
		g_pSolidDraw->ReleaseSnapStatus();
		if(pRelaPart2.IsHasPtr())
			g_pSolidDraw->SetEntSnapStatus(pRelaPart2->handle);
	}
	BOOL bRet = 0;
	if (pDatumPart)
	{	//
		CUndoOperObject undo(&Ta,true);
		SmartPlatePtr pRibPlate=RibPlateClassifyDesign(pDatumPart,pRelaPart1,pRelaPart2,pDatumNode);
		if (bRet=pRibPlate.blValid)
		{
			g_pSolidDraw->SetEntSnapStatus(pRibPlate->handle);
			UpdatePropertyPage(pRibPlate,FALSE);
			//选中Z向偏移量
			CTowerPropertyDlg *pPropertyDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
			if(pPropertyDlg&&pRibPlate->GetClassTypeId()==CLS_PARAMPLATE)
			{
				int iParamType=pRibPlate.ParamPlatePointer()->m_iParamType;
				if(iParamType!=TYPE_TUBESTAYWIREPLATE)
				{	//拉线板提示没有找名字为Z的属性ID wht 18-12-26
					CXhChar50 sKey(iParamType==TYPE_LINEANGLERIBPLATE?"D":"Z");
					CPropTreeItem *pItem=pPropertyDlg->GetPropertyList()->FindItemByPropId(CLDSParamPlate::GetPropID(sKey,iParamType),NULL);
					if(pItem)
						pPropertyDlg->GetPropertyList()->SelectItem(pItem->m_iIndex);
				}
			}
		}
		g_pSolidDraw->Draw();
	}
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:", "");
#else
	pCmdLine->FillCmdLine("命令:", "");
#endif
	g_pSolidDraw->ReleaseSnapStatus();
	if (bRet)
	{	//上一次肋板执行成功后，继续肋板命令操作
		cmdlock.ManualUnlock();
		Command("RibPlate");
	}
	return 0;
}

static bool CmdStrToNumber(const char* cmd_str,double *x,double *y=NULL,double *z=NULL)
{
	if(cmd_str==NULL||strlen(cmd_str)<=0)
		return false;
	CXhChar200 cmdstr=cmd_str;
	char* key=strtok(cmdstr,",，");
	if(x)
	{
		if(key==NULL)
			return false;
		else
			*x=atof(key);
	}
	key=strtok(NULL,",，");
	if(y)
	{
		if(key==NULL)
			return false;
		else
			*y=atof(key);
	}
	key=strtok(NULL,",，");
	if(z)
	{
		if(key==NULL)
			return false;
		else
			*z=atof(key);
	}
	return true;
}

//根据钢板件号查找对应钢板，初始化外形参数
static void InitPlateProfileParamByPartNo(CLDSParamPlate *pDestPlate)
{
	if(pDestPlate==NULL)
		return;
	CLDSParamPlate *pSrcPlate=NULL;
	if(pDestPlate->GetPartNo()>0)
	{
		CLDSObject *pObj=Ta.FromPartNo(pDestPlate->GetPartNo());
		if( pObj&&pObj->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pObj)->m_iParamType==pDestPlate->m_iParamType)
			pSrcPlate=(CLDSParamPlate*)pObj;
	}
	if(pSrcPlate&&pDestPlate->m_iParamType==TYPE_RIBPLATE)
	{	//TODO:待完善,需要参照零件库代码进行改进,目前仅支持角肋板 wht 16-10-28
		CLDSParamPlate::RIBPLATE_CORNER corner;
		corner.LoadParams(pSrcPlate,NULL);
		CXhChar16 paramKeyArr[8]={"E","G","F","H","I","MF","MG","MH"};
		double itemValueArr[8]={corner.A.value.fValue,corner.B.value.fValue,corner.C.value.fValue,
								corner.D.value.fValue,corner.E.value.fValue,corner.F.value.fValue,
								corner.G.value.fValue,corner.H.value.fValue};
		for(int i=0;i<8;i++)
			pDestPlate->SetDesignItemValue(KEY2C(paramKeyArr[i]),itemValueArr[i],NULL,PARAM_SHAPE);
		pDestPlate->SetPartNo(pSrcPlate->GetPartNo());
		pDestPlate->cMaterial=pSrcPlate->cMaterial;
		pDestPlate->Thick=pSrcPlate->Thick;
		if(pDestPlate->DesignPlate())
		{
			pDestPlate->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pDestPlate->GetSolidPartObject());
			g_pSolidDraw->AddCS(pDestPlate->handle,pDestPlate->ucs);
			g_pSolidDraw->Draw();
		}
	}
}

CLDSParamPlate* CLDSView::DesignSimpleRibPlateOnRod(CLDSLinePart* pDatumRod,CLDSNode* pDatumNode,CLDSBolt* pDatumBolt)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_POINT);
	DWORD dwhObj=0,dwExportFlag=0;
	CString cmdStr,valueStr;
	if (pDatumRod->IsAngle())
	{	//角钢肋板
		if(pDatumNode==NULL&&pDatumBolt==NULL)
		{
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_BOLT));
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("RibPlate Please select rib plate's location node or bolt int the datum angle:");
#else
			cmdStr.Format("RibPlate 请选择肋板在基准角钢上的定位节点或螺栓:");
#endif
			while(pDatumNode==NULL&&pDatumBolt==NULL)
			{
				pCmdLine->FillCmdLine(cmdStr,"");
				if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
				{
					pCmdLine->CancelCmdLine();
					return 0;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				dwhObj=obj.hRelaObj;	//dwhObj不一定是选中对象的句柄标识，需要转换
				pDatumNode=obj.provider==OBJPROVIDER::LINESPACE?Ta.Node.FromHandle(dwhObj):NULL;
				pDatumBolt=(pDatumNode==NULL)?(CLDSBolt*)Ta.Parts.FromHandle(dwhObj,CLS_BOLT):NULL;
			}
		}
		if (pDatumNode)
			return DesignLineAngleRibPlate((CLDSLineAngle*)pDatumRod,pDatumNode);
		else// if(pDatumBolt)
			return DesignLineAngleRibPlate((CLDSLineAngle*)pDatumRod,pDatumBolt);
	}
	else if(pDatumRod->IsTube())
	{
		if(pDatumNode==NULL)
		{
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("RibPlate Please select rib plate's location node in the datum rod:");
#else
			cmdStr.Format("RibPlate 请选择肋板在基准杆件上的定位节点:");
#endif
			CDisplayNodeAtFrontLife displayNode;
			displayNode.DisplayNodeAtFront();
			while(pDatumNode==NULL)
			{
				pCmdLine->FillCmdLine(cmdStr,"");
				if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
				{
					pCmdLine->CancelCmdLine();
					return 0;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				dwhObj=obj.hRelaObj;	//dwhObj不一定是选中对象的句柄标识，需要转换
				pDatumNode=Ta.Node.FromHandle(dwhObj);
			}
		}
		//设置肋板默认参数，生成肋板实体
		CLDSLineTube *pTube=(CLDSLineTube *) pDatumRod;
		CDesignRibbedPlateDlg ribDlg;
		ribDlg.m_pDatumPart=pDatumRod;
		ribDlg.m_pDatumNode=pDatumNode;
		ribDlg.m_xRibPlate.fLength = 200;
		ribDlg.m_xRibPlate.fBottomWidth=pTube->GetDiameter()*0.5;
		ribDlg.m_xRibPlate.tube.fOffsetDistM=0;
		ribDlg.m_xRibPlate.tube.fInitOffsetAngle =0;
		ribDlg.m_xRibPlate.tube.iDatumEdge=0;
		ribDlg.m_xRibPlate.tube.iWorkNormStyle=0;
		ribDlg.m_xRibPlate.tube.cLeftTopCornerType=ribDlg.m_xRibPlate.tube.cRightTopCornerType=1;
		ribDlg.m_xRibPlate.tube.fLeftTopCornerWidth=ribDlg.m_xRibPlate.tube.fLeftTopCornerHeight=30;
		ribDlg.m_xRibPlate.tube.fRightTopCornerWidth=ribDlg.m_xRibPlate.tube.fRightTopCornerHeight=30;
		/*根据李晓仲、刘伟反应，钢管轴向肋板命令直接通过肋板对话框进行设计更加方便
			不需要通过命令栏输入参数进行分布设计  wxc--2017.6.15
		*/
		CLDSParamPlate* pRibPlateOnTube=NULL;
		if(ribDlg.DoModal()==IDOK)
			pRibPlateOnTube=ribDlg.GetRibPlate();
		return pRibPlateOnTube;
	}
	return NULL;
}

#include "KeypointLifeObj.h"
CLDSParamPlate* CLDSView::DesignSimpleRibPlateOnPlate(CLDSGeneralPlate* pDatumPlate)
{	//角肋板	需要选择定位基准螺栓
	//钢板肋板  需要选择定位基准螺栓以及钢板延伸方向定位构件
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	ANCHOR_PARAM anchors[4];
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_BOLT));
	verifier.AddVerifyType(OBJPROVIDER::DRAWINGSPACE,IDbEntity::DbPoint);
	verifier.AddVerifyType(OBJPROVIDER::DRAWINGSPACE,IDbEntity::DbText);
	CPlateDisplayLifeObject drawPlate(pDatumPlate,1);
	CLDSBolt* pDatumBolt=NULL;
	CLDSPart* pVecPart=NULL;
	DWORD dwhObj=0,dwExportFlag=0;
	int retcode=0;
	IDrawing* pDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	int hits=0;
	while(hits<4)
	{
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_BOLT));
		if(hits==0)
#ifdef AFX_TARG_ENU_ENGLISH			
			pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
#else
			pCmdLine->FillCmdLine("请选择肋板的定位轮廓点或螺栓:","");
#endif
		else if(hits==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select rib plate's direction rod、vertex or bolt, press enter by default:","");
#else
			pCmdLine->FillCmdLine("请选择肋板的基面走向的基准杆件、标定轮廓点或螺栓, 回车取默认方向:","");
#endif
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART);
		}
		else if(hits==2)
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select rib plate's bending plane location vertex or bolt, press enter when no bending:","");
#else
			pCmdLine->FillCmdLine("请选择肋板的制弯面定位轮廓点或螺栓，无制弯面时回车:","");
#endif
		else if(hits==3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select rib plate's bending plane tip location vertex、 bolt or direction rod, press enter as default:","");
#else
			pCmdLine->FillCmdLine("请选择肋板的制弯面的末端定位轮廓点、螺栓或辅助定向杆件，回车自动识别:","");
#endif
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART);
		}
		int retcode=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier);
		if(retcode>0)
		{
			SELOBJ obj(dwhObj,dwExportFlag);
			if(obj.provider==1&&pDrawing)	//图纸图元
			{
				IDbEntity* pEnt=pDrawing->GetDbEntity(obj.idEnt);
				HIBERID hiberid=pEnt->GetHiberId();
				long hRelaPlate=hiberid.masterId;
				if(pEnt->GetDbEntType()==IDbEntity::DbPoint||pEnt->GetDbEntType()==IDbEntity::DbText)
				{
					anchors[hits].locate.vertex.idVertex=hiberid.HiberDownId(1);
					for(PROFILE_VERTEX* pVertex=pDatumPlate->vertex_list.GetFirst();pVertex&&hRelaPlate==pDatumPlate->handle;pVertex=pDatumPlate->vertex_list.GetNext())
					{
						if(pVertex->vertex.ID==anchors[hits].locate.vertex.idVertex)
						{
							anchors[hits].locate.vertex.x=pVertex->vertex.x;
							anchors[hits].locate.vertex.y=pVertex->vertex.y;
							anchors[hits].ciType=3;	//轮廓顶点
							break;
						}
					}
				}
				else if(pEnt->GetDbEntType()==IDbEntity::DbCircle)
				{
					anchors[hits].locate.hBolt=hiberid.HiberDownId(1);
					anchors[hits].ciType=2;	//选择图纸中的圆孔（代表螺栓）
				}
			}
			else if(obj.provider==2)	//实体模型
			{
				if(obj.iObjType==CLS_BOLT)
				{
					anchors[hits].ciType=2;	//选择实体模型中的螺栓
					anchors[hits].locate.hBolt=obj.hObj;
				}
				else
				{
					anchors[hits].ciType=1;	//选择实体模型中的杆件
					anchors[hits].locate.hVecRod=obj.hObj;
				}
				g_pSolidDraw->SetEntSnapStatus(obj.hObj);
			}
		}
		else if(retcode<0)
		{
			pCmdLine->CancelCmdLine();
			g_pSolidDraw->ReleaseSnapStatus();
			return 0;
		}
		//else if(retcode==0) 返回零表示未选到任何对象
		if(hits>=2&&anchors[hits].ciType==0)
		{
			pCmdLine->FinishCmdLine();
			break;
		}
		if(anchors[hits].ciType!=0||hits==1)
		{
			pCmdLine->FinishCmdLine();
			hits++;
		}
	}
	if(anchors[0].ciType==2)
		pDatumBolt=(CLDSBolt*)Ta.Parts.FromHandle(anchors[0].locate.hBolt,CLS_BOLT);
	if(anchors[1].ciType==1)
		pVecPart=Ta.Parts.FromHandle(anchors[1].locate.hVecRod);
	else if(anchors[1].ciType==2)
		pVecPart=Ta.Parts.FromHandle(anchors[1].locate.hBolt);
	if(anchors[2].ciType>1&&anchors[3].ciType>1)
	{	//制弯面连选两个定位点时，令最后选择的定位点为制弯面末点，另一点为辅助定向点
		ANCHOR_PARAM temp=anchors[2];
		anchors[2]=anchors[3];
		anchors[3]=temp;
	}
	//恢复基准钢板的实体显示
	g_pSolidDraw->NewSolidPart(pDatumPlate->GetSolidPartObject());
	if(pDrawing)
	{
		pDrawing->DeleteDbEntityByHiberMasterId(pDatumPlate->handle);
		g_pSolidDraw->Draw();
	}
	CDesignRibbedPlateDlg ribDlg;
	ribDlg.m_pDatumPart=pDatumPlate;
	ribDlg.m_pVecPart=pVecPart;
	ribDlg.m_pDatumBolt=pDatumBolt;
	//ribDlg.m_pDatumNode=pDatumNode;
	//采取直接生成以钢板为底板的普通肋板，以便改善用户体验性 wjh-2016.5.6
	if(anchors[0].ciType==3&&anchors[1].ciType==3&&anchors[2].ciType==3&&anchors[3].ciType==0)
		anchors[3]=anchors[1];	//选择三个钢板轮廓顶点时,自动识别为轮廓边上的等距线折弯肋板
	memcpy(ribDlg.m_xRibPlate.plate.anchors,anchors,sizeof(ANCHOR_PARAM)*4);
	ribDlg.m_xRibPlate.plate.bOnDatumPlateTopFace=TRUE;
	ribDlg.m_xRibPlate.m_cPlateType=3;
	ribDlg.m_xRibPlate.fBottomWidth=50;	//对应于板上焊接肋板的高度参数，故默认为50mm wjh-2016.6.18
	CLDSParamPlate* pRibPlate=ribDlg.CreateRibPlate();
	pRibPlate->iSeg=pDatumPlate->iSeg;
	ribDlg.SetRibPlate(pRibPlate);
	//根据用户选择参数初始化肋板参数
	double distArr[4]={0,0,0,0},fLen=0;
	if(InitRibPlateOffsetDistAndLen(pDatumPlate,pRibPlate,anchors,distArr,&fLen))
	{
		KEY2C keyArr[4]={KEY2C("X"),KEY2C("Z"),KEY2C("dx"),KEY2C("dz")};
		for(int i=0;i<4;i++)
		{
			if(distArr[i]!=0)
				pRibPlate->SetDesignItemValue(keyArr[i],distArr[i],"",PARAM_POS);
		}
		if(fLen>0)
			pRibPlate->SetDesignItemValue(KEY2C("G"),fLen);
		ribDlg.ReDesignPlate();
	}
	pDatumPlate->m_cPickedDisplayMode=0;
	return pRibPlate;
}
CLDSParamPlate* CLDSView::DesignElbowRibPlate(CLDSLineTube* pDatumTube,SmartPlatePtr pDatumPlate)
{
	//选中钢板和辅助板后，确定肋板类型：钢管肘板|钢管拉线板
	CString cmdStr,cmdValue;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH			
	pCmdLine->FillCmdLine("RibPlate Please select rib plate's type[Elbow (U)/钢管拉线板(D)]:","");
#else
	pCmdLine->FillCmdLine("RibPlate 选择肋板类型[钢管肘板(1)/钢管拉线板(2)] <1>:","");
#endif
	if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"1|2"))
		return NULL;
	if(cmdStr.GetLength()>0&&cmdStr[0]=='2')
	{	//设计钢管拉线板
		GEPOINT startPt=pDatumTube->Start(),endPt=pDatumTube->End(),pt=pDatumPlate->ucs.origin;
		GEPOINT vStdRodVec=startPt-endPt;
		Standarized(vStdRodVec);
		if(DISTANCE(startPt,pt)<DISTANCE(endPt,pt))
			vStdRodVec*=-1;
		char cRibToBasePlateSide=0;
		if(pDatumPlate->ucs.axis_z*vStdRodVec>0)
			cRibToBasePlateSide='U';
		else
			cRibToBasePlateSide='D';
		//
		CDesLineAngleRibPlateDlg rodRibDlg;
		rodRibDlg.m_pDatumTube=pDatumTube;
		rodRibDlg.m_pAssistPlate=pDatumPlate;
		rodRibDlg.stay_wire_para.bOnRelaPlateTopFace=cRibToBasePlateSide=='D'?FALSE:TRUE;
		CLDSParamPlate* pRibPlate=NULL;
		if(rodRibDlg.DoModal()==IDOK)
			pRibPlate=rodRibDlg.CreateRibPlate();
		return pRibPlate;
	}
	if((pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE&&pDatumPlate.pParamPlate->IsFL())||
		pDatumPlate->GetClassTypeId()==CLS_PLATE&&pDatumPlate.PlatePointer()->m_fInnerRadius>180)
	{	//根据立波建议，如果角肋板的依赖底板是法兰或者环板，则直接弹出肋板对话框设计钢管肘板更加便捷  wxc-2017.6.20
		GEPOINT startPt=pDatumTube->Start(),endPt=pDatumTube->End(),pt=pDatumPlate->ucs.origin;
		GEPOINT vStdRodVec=startPt-endPt;
		Standarized(vStdRodVec);
		if(DISTANCE(startPt,pt)<DISTANCE(endPt,pt))
			vStdRodVec*=-1;
		char cRibToBasePlateSide=0;
		if(pDatumPlate->ucs.axis_z*vStdRodVec>0)
			cRibToBasePlateSide='U';
		else
			cRibToBasePlateSide='D';
		//
		CDesignRibbedPlateDlg ribDlg;
		ribDlg.m_pDatumPart=pDatumTube;
		ribDlg.m_pRelaPart1=pDatumPlate;
		if(cRibToBasePlateSide=='D')
			ribDlg.m_xRibPlate.bOnRelaPlateTopFace=false;
		else
			ribDlg.m_xRibPlate.bOnRelaPlateTopFace=true;
		if(ribDlg.DoModal()==IDOK)
			return ribDlg.GetRibPlate();
		else
			return NULL;
	}
	//通过命令窗口初始化肘板的设计信息
	GEPOINT lineStart=pDatumTube->Start(),lineEnd=pDatumTube->End();
	GEPOINT vStdRodVec=lineEnd-lineStart;
	Standarized(vStdRodVec);
	lineStart-=vStdRodVec*pDatumTube->startOdd();
	lineEnd  +=vStdRodVec*pDatumTube->endOdd();
	coord_trans(lineStart,pDatumPlate->ucs,FALSE,TRUE);
	coord_trans(lineEnd,pDatumPlate->ucs,FALSE,TRUE);
	char cRibToBasePlateSide=0;
	if(lineStart.z>pDatumPlate->GetNormOffset()-20&&lineEnd.z>pDatumPlate->GetNormOffset()-20)
		cRibToBasePlateSide='U';	//肋板只能在钢板顶面（底面无钢管）
	else if(lineStart.z<pDatumPlate->GetNormOffset()+pDatumPlate->Thick+20&&lineEnd.z<pDatumPlate->GetNormOffset()+pDatumPlate->Thick+20)
		cRibToBasePlateSide='D';	//肋板只能在钢板底面（顶面无钢管）
	else //if(cRibToBasePlateSide==0)
	{
//#ifdef AFX_TARG_ENU_ENGLISH			
//		pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
//#else
//		pCmdLine->FillCmdLine("肋板位于钢板边的[顶面(U)/底板(D)] <U>:","");
//#endif
//		if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"U|D"))
//			return NULL;
//		if(cmdStr.GetLength()>0&&toupper(cmdStr[0])=='D')
//			cRibToBasePlateSide='D';
//		else
//			cRibToBasePlateSide='U';
		//
		if(pDatumPlate->ucs.axis_z*vStdRodVec>0)
			cRibToBasePlateSide='U';
		else
			cRibToBasePlateSide='D';
	}
	DWORD dwhObj=0,dwExportFlag=0;
	IDrawing* pDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	CPlateDisplayLifeObject drawplate(pDatumPlate,1);
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_BOLT));
	verifier.AddVerifyType(OBJPROVIDER::DRAWINGSPACE,IDbEntity::DbPoint);
	verifier.AddVerifyType(OBJPROVIDER::DRAWINGSPACE,IDbEntity::DbText);
	verifier.AddVerifyType(OBJPROVIDER::DRAWINGSPACE,IDbEntity::DbCircle);
#ifdef AFX_TARG_ENU_ENGLISH			
	pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
#else
	if(pDatumTube->m_bHasWeldRoad)
		pCmdLine->FillCmdLine("选择定位螺栓、顶点，或指定底边旋转角度(默认自基准边)、自焊道边(W)，或[底边走向(V)/定位拾取点(P)] <0>:","");
	else
		pCmdLine->FillCmdLine("选择定位螺栓、顶点，或指定底边旋转角度(自基准边)，或[底边走向(V)/定位拾取点(P)] <0>:","");
#endif
	CLDSBolt* pDatumBolt=NULL;
	ANCHOR_PARAM anchor,side_anchor;
	PEEKCMDLINE cmd_detect("W|V|P",pCmdLine);
	while(true)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier,PEEKCMDLINE::MatchKeyword,&cmd_detect)<0)
			return NULL;
		SELOBJ obj(dwhObj,dwExportFlag,pDrawing);
		dwhObj=obj.hRelaObj;
		if(obj.provider==OBJPROVIDER::SOLIDSPACE&&dwhObj>0)	//选择定位螺栓
		{
			pDatumBolt=(CLDSBolt*)Ta.Parts.FromHandle(dwhObj,CLS_BOLT);
			if(pDatumBolt)
			{
				anchor.locate.hBolt=dwhObj;
				anchor.ciType=2;
			}
		}
		if(obj.provider==OBJPROVIDER::DRAWINGSPACE&&dwhObj==pDatumPlate->handle)
		{	//直接拾取钢板轮廓顶点或螺栓孔
			anchor.ciType=3;
			IDbEntity* pEnt=pDrawing->GetDbEntity(obj.idEnt);
			HIBERID hiberid=pEnt->GetHiberId();
			if(pEnt->GetDbEntType()==IDbEntity::DbPoint||pEnt->GetDbEntType()==IDbEntity::DbText)
			{
				anchor.locate.vertex.idVertex=hiberid.HiberDownId(1);
				for(PROFILE_VERTEX* pVertex=pDatumPlate->vertex_list.GetFirst();pVertex&&dwhObj==pDatumPlate->handle;pVertex=pDatumPlate->vertex_list.GetNext())
				{
					if(pVertex->vertex.ID==anchor.locate.vertex.idVertex)
					{
						anchor.locate.vertex.x=pVertex->vertex.x;
						anchor.locate.vertex.y=pVertex->vertex.y;
						anchor.ciType=3;	//轮廓顶点
						break;
					}
				}
			}
			else if(pEnt->GetDbEntType()==IDbEntity::DbCircle)
			{
				anchor.locate.hBolt=hiberid.HiberDownId(1);
				anchor.ciType=2;	//选择图纸中的圆孔（代表螺栓）
			}
		}
		if(anchor.ciType==0&&obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
		{
			pCmdLine->PeekStrFromCmdLine(cmdStr);
			break;
		}
		else if(anchor.ciType>0)
			break;
	}
	drawplate.RestoreDisplayMode();
	int retcode=0;

	long nSelect,*id_arr=NULL;
	if(anchor.ciType==2)
		pDatumBolt=(CLDSBolt*)Ta.Parts.FromHandle(anchor.locate.hBolt,CLS_BOLT);
	else if((nSelect=g_pSolidSnap->GetLastSelectEnts(id_arr))>0)
		pDatumBolt=(CLDSBolt*)Ta.Parts.FromHandle(id_arr[0],CLS_BOLT);
	char cCmdType='E';	//自钢管基准边旋转指定角度定位底边
	char cSpecAxisType='0';
	SmartPlatePtr pLocationPlate;
	GEPOINT vSpecGlobalBtmEdgeVec,vSpecLocalBtmEdgeVecPick;
	double degree=0;
	if(cmdStr.GetLength()==0&&pDatumBolt)
		cCmdType='B';
	else if(cmdStr.GetLength()==0)
		degree=0;
	else if(cmdStr.GetLength()>1)
		degree=atof(cmdStr);
	else	//指定底边其它定位方式
		cCmdType=toupper(cmdStr[0]);
	pCmdLine->FinishCmdLine();
	verifier.ClearSnapFlag();
	if(anchor.ciType==3)
	{	//已选择了定位顶点
		cCmdType='P';
		vSpecLocalBtmEdgeVecPick.Set(anchor.locate.vertex.x,anchor.locate.vertex.y);
	}
	else if(cCmdType=='W')
	{
#ifdef AFX_TARG_ENU_ENGLISH			
		pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
#else
		pCmdLine->FillCmdLine("指定自钢管焊道边至肋板底边的旋转角度<0>:","");
#endif
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
			return NULL;
		degree=cmdStr.GetLength()>0?atof(cmdStr):0;
	}
	else if(cCmdType=='V')
	{	//指定全局坐标
#ifdef AFX_TARG_ENU_ENGLISH			
		pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
#else
		pCmdLine->FillCmdLine("指定底边的延伸参照方向，[0.自定义|1.X+|2.X-|3.Y+|4.Y-|6.Z+|6.Z-]<0>:","");
#endif
		if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"0|1|2|3|4|5|6"))
			return NULL;
		if(cmdStr.GetLength()>0)
			cSpecAxisType=cmdStr[0];
		if(cSpecAxisType<'0'||cSpecAxisType>'6')
			cSpecAxisType='0';
		if(cSpecAxisType=='0')
		{
#ifdef AFX_TARG_ENU_ENGLISH			
			pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
#else
			pCmdLine->FillCmdLine("指定底边延伸方向的参照方向(模型坐标系)坐标分量:","");
#endif
			CXhChar200 cmdstr=cmdStr;
			char* key=strtok(cmdstr,",");
			vSpecGlobalBtmEdgeVec.x=key!=NULL ? atof(key) : 0;
			key=strtok(NULL,",");
			vSpecGlobalBtmEdgeVec.y=key!=NULL ? atof(key) : 0;
			key=strtok(NULL,",");
			vSpecGlobalBtmEdgeVec.z=key!=NULL ? atof(key) : 0;
		}
		else
			vSpecGlobalBtmEdgeVec=CStdVector::GetVector(cSpecAxisType-'0');
	}
	else if(cCmdType=='P')
	{	//指定钢板上局部坐标
#ifdef AFX_TARG_ENU_ENGLISH			
		pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
#else
		pCmdLine->FillCmdLine((char*)CXhChar50("请输入底板局部坐标系下底边拾取点的坐标<X=%.0f,Y=%.0f>:",
			vSpecLocalBtmEdgeVecPick.x,vSpecLocalBtmEdgeVecPick.y),"");
#endif
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
			return NULL;
		CXhChar100 szCmdInput=cmdStr;
		char* key=strtok(szCmdInput,", ");
		if(key!=NULL)
			vSpecLocalBtmEdgeVecPick.x=atof(key);
		key=strtok(NULL,", ");
		if(key!=NULL)
			vSpecLocalBtmEdgeVecPick.y=atof(key);
	}
	//
//#ifdef AFX_TARG_ENU_ENGLISH			
//	pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
//#else
//	pCmdLine->FillCmdLine("指定与底板边缘的边距或不延伸至边(N) <0>:","");
//#endif
//	if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"N"))
//		return NULL;
	long margin=0;
//	if(cmdStr.GetLength()>0&&toupper(cmdStr[0])=='N')
//		margin=-1;
//	else if(cmdStr.GetLength()>0)
//		margin=atoi(cmdStr);

	CDesignRibbedPlateDlg ribDlg;
	ribDlg.m_pDatumPart=pDatumTube;
	ribDlg.m_pRelaPart1=pDatumPlate;
	ribDlg.m_xRibPlate.params.elbow.hDatumTube.Set( pDatumTube->handle);
	ribDlg.m_xRibPlate.params.elbow.margin.Set(margin);
	if(cRibToBasePlateSide=='D')
	{
		ribDlg.m_xRibPlate.bOnRelaPlateTopFace=false;
		ribDlg.m_xRibPlate.params.elbow.hBtmPlate.Set(-pDatumPlate->handle);
	}
	else
	{
		ribDlg.m_xRibPlate.bOnRelaPlateTopFace=true;
		ribDlg.m_xRibPlate.params.elbow.hBtmPlate.Set( pDatumPlate->handle);
	}
	if(cCmdType=='B')
	{	//螺栓定位
		ribDlg.m_xRibPlate.params.elbow.J.value.iValue=2;	//螺栓定位
		ribDlg.m_xRibPlate.params.elbow.hDatumBolt.Set(pDatumBolt->handle);
	}
	else if(cCmdType=='E')
	{	//自基准边旋转
		ribDlg.m_xRibPlate.params.elbow.J.value.iValue=0;
		ribDlg.m_xRibPlate.params.elbow.rotAngle.Set(degree);
	}
	else if(cCmdType=='W')
	{	//自焊道边旋转
		ribDlg.m_xRibPlate.params.elbow.J.value.iValue=1;
		ribDlg.m_xRibPlate.params.elbow.rotAngle.Set(degree);
	}
	else if(cCmdType=='V')
	{	//指定全局方向
		ribDlg.m_xRibPlate.params.elbow.J.value.iValue=3;
		ribDlg.m_xRibPlate.params.elbow.vAxisXVecX.Set(vSpecGlobalBtmEdgeVec.x);
		ribDlg.m_xRibPlate.params.elbow.vAxisXVecY.Set(vSpecGlobalBtmEdgeVec.y);
		ribDlg.m_xRibPlate.params.elbow.vAxisXVecZ.Set(vSpecGlobalBtmEdgeVec.z);
	}
	else if(cCmdType=='P')
	{	//指定底板上标识点(局部坐标)
		ribDlg.m_xRibPlate.params.elbow.J.value.iValue=4;
		ribDlg.m_xRibPlate.params.elbow.vAxisXVecX.Set(vSpecLocalBtmEdgeVecPick.x);
		ribDlg.m_xRibPlate.params.elbow.vAxisXVecY.Set(vSpecLocalBtmEdgeVecPick.y);
	}
	//
	ribDlg.DoModal();
	return ribDlg.GetRibPlate();
}

void SnapObjToAnchorParam(ANCHOR_PARAM &anchor,DWORD dwhObj,DWORD dwExportFlag,IDrawing *pDrawing)
{
	SELOBJ obj(dwhObj,dwExportFlag,pDrawing);
	if(obj.provider==OBJPROVIDER::SOLIDSPACE)	//选择定位螺栓
	{
		CLDSPart *pPart=Ta.Parts.FromHandle(obj.hRelaObj,CLS_BOLT,CLS_LINEPART);
		if(pPart&&pPart->GetClassTypeId()==CLS_BOLT)
		{
			anchor.locate.hBolt=pPart->handle;
			anchor.ciType=2;
		}
		else if(pPart&&pPart->IsLinePart())
		{
			anchor.locate.hVecRod=pPart->handle;
			anchor.ciType=1;
		}
	}
	else if(obj.provider==OBJPROVIDER::DRAWINGSPACE)
	{	//直接拾取钢板轮廓顶点或螺栓孔
		anchor.ciType=3;
		anchor.hRelaObj=obj.hRelaObj;
		IDbEntity* pEnt=pDrawing!=NULL?pDrawing->GetDbEntity(obj.idEnt):NULL;
		CLDSGeneralPlate *pPlate=(CLDSGeneralPlate*)console.FromPartHandle(obj.hRelaObj,CLS_PARAMPLATE,CLS_PLATE);
		if(pPlate&&pEnt)
		{
			HIBERID hiberid=pEnt->GetHiberId();
			if(pEnt->GetDbEntType()==IDbEntity::DbPoint||pEnt->GetDbEntType()==IDbEntity::DbText)
			{
				anchor.locate.vertex.idVertex=hiberid.HiberDownId(1);
				for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
				{
					if(pVertex->vertex.ID==anchor.locate.vertex.idVertex)
					{
						anchor.locate.vertex.x=pVertex->vertex.x;
						anchor.locate.vertex.y=pVertex->vertex.y;
						anchor.ciType=3;	//轮廓顶点
						break;
					}
				}
			}
			else if(pEnt->GetDbEntType()==IDbEntity::DbCircle)
			{
				anchor.locate.hBolt=hiberid.HiberDownId(1);
				anchor.ciType=2;	//选择图纸中的圆孔（代表螺栓）
			}
		}
	}
	else
	{
		anchor.ciType=0;
		anchor.locate.ciTriggerType=obj.ciTriggerType;
	}
}
bool CreateSketchPlaneByPlate(CLDSGeneralPlate* pPlate,btc::SKETCH_PLANE* pSketchPlane,
							  int iFaceNo=0,const double* distFromSketch=NULL);
CLDSParamPlate* CLDSView::DesignCornerRibPlate(SmartPlatePtr pDatumPlate,SmartPlatePtr pSidePlate)
{
	CString cmdStr,cmdValue;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CSysPromptObject prompt;
	prompt.SetPromptPicture(IDB_BMP_CORNER_RIB_PLATE2);
	DWORD dwhObj=0,dwExportFlag=0;
	IDrawing* pDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	CPlateDisplayLifeObject drawplate(pDatumPlate,1),drawSidePlate(pSidePlate,1);
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_BOLT));
	verifier.AddVerifyType(OBJPROVIDER::DRAWINGSPACE,IDbEntity::DbPoint);
	verifier.AddVerifyType(OBJPROVIDER::DRAWINGSPACE,IDbEntity::DbText);
	verifier.AddVerifyType(OBJPROVIDER::DRAWINGSPACE,IDbEntity::DbCircle);
	//1.选择定位螺栓、顶点或者输入定位拾取点
#ifdef AFX_TARG_ENU_ENGLISH			
	pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
#else
	pCmdLine->FillCmdLine("RibPlate 选择定位螺栓、顶点或输入底板坐标系下坐标<0,0>:","");
#endif
	ANCHOR_PARAM anchor;
	PEEKCMDLINE cmd_detect("V|P",pCmdLine);
	while(true)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier,PEEKCMDLINE::MatchKeyword,&cmd_detect)<0)
			return NULL;
		SnapObjToAnchorParam(anchor,dwhObj,dwExportFlag,pDrawing);
		if(anchor.ciType==0&&anchor.locate.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
		{
			pCmdLine->PeekStrFromCmdLine(cmdStr);
			break;
		}
		else if(anchor.ciType>0)
			break;
	}
	drawplate.RestoreDisplayMode();
	drawSidePlate.RestoreDisplayMode();
	//
	CLDSBolt *pDatumBolt=NULL;
	long nSelect,*id_arr=NULL;
	if(anchor.ciType==2)
		pDatumBolt=(CLDSBolt*)Ta.Parts.FromHandle(anchor.locate.hBolt,CLS_BOLT);
	else if((nSelect=g_pSolidSnap->GetLastSelectEnts(id_arr))>0)
		pDatumBolt=(CLDSBolt*)Ta.Parts.FromHandle(id_arr[0],CLS_BOLT);
	GEPOINT ptSpecOrg;
	if(anchor.ciType==3)
	{	//已选择了定位顶点
		ptSpecOrg.Set(anchor.locate.vertex.x,anchor.locate.vertex.y);
		if(anchor.hRelaObj==pSidePlate->handle)
		{
			coord_trans(ptSpecOrg,pSidePlate->ucs,TRUE);
			coord_trans(ptSpecOrg,pDatumPlate->ucs,FALSE);
			ptSpecOrg.z=0;
		}
	}
	if(cmdStr.GetLength()>=0)
		CmdStrToNumber(cmdStr,&ptSpecOrg.x,&ptSpecOrg.y);
	if(pDatumBolt)
		pCmdLine->FinishCmdLine(CXhChar50("%s0x%X",pDatumBolt->GetPartTypeName(),pDatumBolt->handle));
	else
		pCmdLine->FinishCmdLine(CXhChar50(GEPOINT(ptSpecOrg.x,ptSpecOrg.y)));
	//
	//2.根据定位点自动计算确定X轴方向及Z向偏移量初始值
	BOOL bOnDatumPlateTopFace=TRUE,bOnSidePlateTopFace=TRUE,bNeedInputVec=FALSE;
	//初始化肋板在基板(侧板)顶面还是底面
	for(int i=0;i<2;i++)
	{
		CLDSGeneralPlate *pPlate1=(i==0)?pSidePlate:pDatumPlate;
		CLDSGeneralPlate *pPlate2=(i==0)?pDatumPlate:pSidePlate;
		BOOL *pbTopFace=(i==0)?&(bOnDatumPlateTopFace):&(bOnSidePlateTopFace);
		CMaxDouble maxValue;
		CMinDouble minValue;
		for(PROFILE_VERTEX *pVertex=pPlate1->vertex_list.GetFirst();pVertex;pVertex=pPlate1->vertex_list.GetNext())
		{
			GEPOINT vertex=pVertex->vertex;
			coord_trans(vertex,pPlate1->ucs,TRUE);
			coord_trans(vertex,pPlate2->ucs,FALSE);
			maxValue.Update(vertex.z,pVertex);
			minValue.Update(vertex.z,pVertex);
		}
		if(maxValue.number*minValue.number<0)
		{	//根据定位点判断
			GEPOINT verfiy_vec=ptSpecOrg;
			if(pDatumBolt)
				verfiy_vec=pDatumBolt->ucs.origin;
			else
				coord_trans(verfiy_vec,pDatumPlate->ucs,TRUE);
			verfiy_vec-=pPlate2->ucs.origin;
			normalize(verfiy_vec);
			double dd=verfiy_vec*pPlate2->ucs.axis_z;
			*pbTopFace=(dd>0);
			//定位基准点为侧板螺栓(顶点)或无法根据定位点确认X轴方向时需要用户输入
			if(i==1)
			{
				bNeedInputVec=(fabs(dd)<EPS||anchor.hRelaObj==pSidePlate->handle||
							   (pDatumBolt&&pSidePlate->FindLsByHandle(pDatumBolt->handle)));
			}
		}
		else
			*pbTopFace=!(maxValue.number<0&&minValue.number<0);
	}
	//根据定位点
	double fOffsetZ=0;
	GEPOINT axis_x=bOnSidePlateTopFace?pSidePlate->ucs.axis_z:pSidePlate->ucs.axis_z*-1;
	GEPOINT axis_y=bOnDatumPlateTopFace?pDatumPlate->ucs.axis_z:pDatumPlate->ucs.axis_z*-1;
	GEPOINT axis_z=axis_x^axis_y;
	normalize(axis_z);
	if(pDatumBolt)
	{	//选择定位螺栓时查找最近的共线螺栓取距离一半,未找到时默认为半个螺栓间距
		CLDSGeneralPlate *pPlate=pDatumPlate->FindLsByHandle(pDatumBolt->handle)?pDatumPlate:pSidePlate;
		fOffsetZ=10000;	//初始值
		GEPOINT bolt_pos,temp_pos;
		pPlate->GetBoltIntersPos(pDatumBolt,bolt_pos);
		for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{
			if((*pLsRef)->handle==pDatumBolt->handle)
				continue;
			pPlate->GetBoltIntersPos(pLsRef->GetLsPtr(),temp_pos);
			GEPOINT bolt_vec=temp_pos-bolt_pos;
			normalize(bolt_vec);
			double dd=bolt_vec*axis_z;
			if(fabs(dd)<EPS_COS2)
				continue;
			double fDist=DISTANCE(temp_pos,bolt_pos)*0.5;
			if(fDist<fabs(fOffsetZ))
				fOffsetZ=fDist*(dd>0?1:-1);
		}
		if(fabs(fOffsetZ-10000)<EPS)
		{
			LSSPACE_STRU lsSpace;
			GetLsSpace(lsSpace,pDatumBolt->get_d());
			fOffsetZ=lsSpace.SingleRowSpace*0.5;
		}
	}
	else if(anchor.ciType==3)
	{	//轮廓点定位是默认为与Z轴方向接近的轮廓边中点
		CLDSGeneralPlate *pPlate=(anchor.hRelaObj==pDatumPlate->handle)?pDatumPlate:pSidePlate;
		int i=anchor.locate.vertex.idVertex-1,n=pPlate->vertex_list.GetNodeNum();
		PROFILE_VERTEX *pPrevVertex=&pPlate->vertex_list[(i+n-1)%n];
		PROFILE_VERTEX *pCurVertex=&pPlate->vertex_list[i%n];
		PROFILE_VERTEX *pNextVertex=&pPlate->vertex_list[(i+1)%n];
		GEPOINT prev_vec=pPrevVertex->vertex-pCurVertex->vertex,next_vec=pNextVertex->vertex-pCurVertex->vertex;
		normalize(prev_vec);
		normalize(next_vec);
		vector_trans(prev_vec,pPlate->ucs,TRUE);
		vector_trans(next_vec,pPlate->ucs,TRUE);
		double prev_dd=prev_vec*axis_z,next_dd=next_vec*axis_z;
		if(fabs(prev_dd)>fabs(next_dd))
			fOffsetZ=ftoi(DISTANCE(pPrevVertex->vertex,pCurVertex->vertex)*0.5)*((prev_dd>0)?1:-1);
		else
			fOffsetZ=ftoi(DISTANCE(pNextVertex->vertex,pCurVertex->vertex)*0.5)*((next_dd>0)?1:-1);
		if(pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE&&(pDatumPlate.ParamPlatePointer()->m_iParamType==TYPE_BASEPLATE))
			fOffsetZ=fOffsetZ>0?10:-10;
	}
	//3.根据参数创建肋板
	CDesignRibbedPlateDlg ribDlg;
	ribDlg.m_pDatumPart=pDatumPlate;
	ribDlg.m_pRelaPart1=pSidePlate;
	ribDlg.m_xRibPlate.bOnRelaPlateTopFace=bOnSidePlateTopFace;
	ribDlg.m_xRibPlate.plate.bOnDatumPlateTopFace=bOnDatumPlateTopFace;
	//原点
	ribDlg.m_xRibPlate.plate.bSpecOrg=(pDatumBolt==NULL);
	ribDlg.m_pDatumBolt=pDatumBolt;
	if(ribDlg.m_xRibPlate.plate.bSpecOrg)
	{
		ribDlg.m_xRibPlate.plate.fOriginX=ptSpecOrg.x;
		ribDlg.m_xRibPlate.plate.fOriginY=ptSpecOrg.y;
	}
	//X轴方向
	ribDlg.m_xRibPlate.plate.iAxisXType=5;
	ribDlg.m_xRibPlate.plate.bSpecAxisX=FALSE;
	//Y轴方向
	ribDlg.m_xRibPlate.plate.fGradientDegAngle=90;
	//Z项偏移量
	ribDlg.m_xRibPlate.plate.fOffsetDistZ=fOffsetZ;
	ribDlg.m_xRibPlate.nSegI=pDatumPlate->iSeg;
	ribDlg.m_xRibPlate.cMaterial=pDatumPlate->cMaterial;
	ribDlg.AutoCalRibPlateWidth();
	CLDSParamPlate *pRibPlate=ribDlg.CreateRibPlate();
	ribDlg.SetRibPlate(pRibPlate);
	//4.输入X轴方向
	char iAxisXType=5;
	GEPOINT vSpecGlobalBtmEdgeVec;
	if(bNeedInputVec)
	{
	#ifdef AFX_TARG_ENU_ENGLISH			
		pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
	#else
		pCmdLine->FillCmdLine("RibPlate 指定底边的延伸参照方向[0.X+|1.X-|2.Y+|3.Y-|4.输入|5.自动计算|6.侧板正面|7.侧板侧面]<6>:","");
	#endif
		if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"0|1|2|3|4|5|6|7"))
			return NULL;
		if(cmdStr.GetLength()>0)
			iAxisXType=cmdStr[0]-'0';
		else
			iAxisXType=6;
		if(iAxisXType==6||iAxisXType==7)
		{
			bOnSidePlateTopFace=(iAxisXType==6);
			iAxisXType=5;
		}
		else if(iAxisXType>5||iAxisXType<0)
			iAxisXType=4;
		if(iAxisXType==4)
		{
	#ifdef AFX_TARG_ENU_ENGLISH			
			pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
	#else
			pCmdLine->FillCmdLine("RibPlate 指定底边延伸方向的参照方向(模型坐标系)坐标分量:","");
	#endif
			CmdStrToNumber(cmdStr,&vSpecGlobalBtmEdgeVec.x,&vSpecGlobalBtmEdgeVec.y,&vSpecGlobalBtmEdgeVec.z);
		}
		else if(iAxisXType>=0&&iAxisXType<=3)
			vSpecGlobalBtmEdgeVec=CStdVector::GetVector('0'+iAxisXType+1);
	}
	//X轴方向发生变化后更新肋板
	//TODO:塔脚角肋板设计时默认显示不对(问题包括:默认Z向偏移,位置示意图,侧向钢板正面)
	if(iAxisXType!=ribDlg.m_xRibPlate.plate.iAxisXType)
	{
		ribDlg.m_xRibPlate.plate.iAxisXType=iAxisXType;
		if(iAxisXType>=0&&iAxisXType<=4)
		{
			vector_trans(vSpecGlobalBtmEdgeVec,pDatumPlate->ucs,FALSE);
			normalize(vSpecGlobalBtmEdgeVec);
			ribDlg.m_xRibPlate.plate.fXAxisX=vSpecGlobalBtmEdgeVec.x;
			ribDlg.m_xRibPlate.plate.fXAxisY=vSpecGlobalBtmEdgeVec.y;
			ribDlg.m_xRibPlate.plate.bSpecAxisX=TRUE;
		}
		else
			ribDlg.m_xRibPlate.plate.bSpecAxisX=FALSE;
		ribDlg.ReDesignPlate();
	}
	//3.输入肋板Y轴定位杆件
	double fGradientDegAngle=90;
	CLDSPart *pVecPart=NULL;
	anchor.ciType=0;
	anchor.hRelaObj=0;
	anchor.locate.hVecRod=0;
	verifier.ClearSnapFlag();
	verifier.AddVerifyType(OBJPROVIDER::SOLIDSPACE,CLS_LINEPART);
	verifier.AddVerifyType(OBJPROVIDER::SOLIDSPACE,CLS_BOLT);
#ifdef AFX_TARG_ENU_ENGLISH			
	pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
#else
	pCmdLine->FillCmdLine("RibPlate 选择Y轴定向螺栓、杆件或输入肋板倾斜角度<90>:","");
#endif
	while(true)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			return NULL;
		SnapObjToAnchorParam(anchor,dwhObj,dwExportFlag,pDrawing);
		if(anchor.ciType==0&&anchor.locate.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
		{
			pCmdLine->PeekStrFromCmdLine(cmdStr);
			break;
		}
		else if(anchor.ciType>0)
			break;
	}
	if(anchor.ciType==1||anchor.ciType==2)
		pVecPart=console.FromPartHandle(anchor.locate.hVecRod);
	if(pVecPart)
		pCmdLine->FinishCmdLine(CXhChar100("%s0x%X",pVecPart->GetPartTypeName(),pVecPart->handle));
	else 
		pCmdLine->FinishCmdLine(CXhChar100("%.f",fGradientDegAngle));
	//Y轴参数变化后更新钢板
	if(pVecPart||fabs(fGradientDegAngle-90)>EPS)
	{
		if(pVecPart&&(pVecPart->GetClassTypeId()==CLS_BOLT||pVecPart->IsLinePart()))
		{
			ribDlg.m_pVecPart=pVecPart;
			ribDlg.ModifyPlateParam('V',pVecPart->handle);
		}
		else
		{
			ribDlg.m_xRibPlate.plate.fGradientDegAngle=fGradientDegAngle;
			ribDlg.ModifyPlateParam(KEY2C("Ga"),fGradientDegAngle);
		}
		ribDlg.ReDesignPlate();
	}
	//根据刘伟要求，输完角肋板的定位参数后就直接退出，不需要通过命令栏输入基本信息等，否则太啰嗦 wxc-2018.1.24
	return pRibPlate;
}
CLDSParamPlate* CLDSView::DesignSlotRibPlate(SmartPartPtr pDatumPart,SmartPlatePtr pSidePlate1,SmartPlatePtr pSidePlate2)
{
	CDesignRibbedPlateDlg ribDlg;
	ribDlg.m_pDatumPart=pDatumPart;
	ribDlg.m_pRelaPart1=pSidePlate1;
	ribDlg.m_pRelaPart2=pSidePlate2;
	CLDSParamPlate* pRibPlate=ribDlg.DoModal()==IDOK?ribDlg.GetRibPlate():NULL;
	return pRibPlate;
}
CLDSGeneralPlate* CLDSView::RibPlateClassifyDesign(SmartPartPtr pDatumPart,SmartPartPtr pRelaPart1,SmartPartPtr pRelaPart2,CLDSNode* pDatumNode)
{
	CSuperSmartPtr<CLDSGeneralPlate>pRibPlate;
	if(pDatumPart->IsLinePart()&&pRelaPart1.IsNULL()&&pRelaPart2.IsNULL())	//杆件肋板：角钢肋板或钢管竖板
		return DesignSimpleRibPlateOnRod(pDatumPart.RodPointer(),pDatumNode,NULL);
	else if(pDatumPart->IsPlate()&&pRelaPart1.IsNULL()&&pRelaPart2.IsNULL())//钢板上的普通肋板
		return DesignSimpleRibPlateOnPlate(pDatumPart.GeneralPlatePointer());
	else if(pDatumPart->GetClassTypeId()==CLS_LINETUBE
		&&pRelaPart1.IsHasPtr()&&pRelaPart1->GetClassTypeId()==CLS_LINETUBE)//钢管夹板
		return DesignTubeBetweenPlate(pDatumPart.LineTubePointer(),pRelaPart1.LineTubePointer());
	else if((pDatumPart->GetClassTypeId()==CLS_LINETUBE&&pRelaPart1->IsPlate()&&pRelaPart2.IsNULL())||
		(pDatumPart->IsPlate()&&pRelaPart1->GetClassTypeId()==CLS_LINETUBE&&pRelaPart2.IsNULL()))
	{	//钢管肘板
		CLDSLineTube* pDatumTube=pDatumPart->IsTube()?pDatumPart.LineTubePointer():pRelaPart1.LineTubePointer();
		SmartPlatePtr pDatumPlate=pDatumPart->IsPlate()?pDatumPart.GeneralPlatePointer():pRelaPart1.GeneralPlatePointer();
		return DesignElbowRibPlate(pDatumTube,pDatumPlate);
	}
	else if(pDatumPart->IsPlate()&&pRelaPart1->IsPlate()&&pRelaPart2.IsNULL())//两钢板间的角肋板
		return DesignCornerRibPlate(pDatumPart.GeneralPlatePointer(),pRelaPart1.GeneralPlatePointer());
	else if((pDatumPart->GetClassTypeId()==CLS_LINETUBE||pDatumPart->IsPlate())&&
		pRelaPart1->IsPlate()&&pRelaPart2->IsPlate())//槽型肋板（含基准构件为钢管和钢板两种槽型肋板）
		return DesignSlotRibPlate(pDatumPart,pRelaPart1.GeneralPlatePointer(),pRelaPart2.GeneralPlatePointer());
	else
		return NULL;
	return pRibPlate;
}

void CLDSView::OnDesignOverlappedPlate()
{	//叠放板
	m_nPrevCommandID=ID_DESIGN_OVERLAPPED_PLATE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat plate";
#else
	m_sPrevCommandName="重复叠放板";
#endif
	Command("OverlappendPlate");
}
//叠放板命令已归入到复制构件命令中 wjh-2016.9.12
int CLDSView::OverlappendPlate()
{
/*	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr, promptStr;
	UCS_STRU object_ucs,ucs,plate_ucs;
	CSuperSmartPtr<CLDSPart> pBasePlate;
	//切换到实体显示模式
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("please select the datum (parametric) plate for the overlapped plates:","");
#else
	pCmdLine->FillCmdLine("请选择叠放板的基准(参数化)钢板:","");
#endif
	while(1)
	{
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			long *id_arr,id_num;
			id_num = g_pSolidSnap->GetLastSelectEnts(id_arr);
			if(id_num!=1)
				return 0;
			pBasePlate=console.FromPartHandle(id_arr[0]);
			if(pBasePlate.IsHasPtr()&&(pBasePlate->GetClassTypeId()==CLS_PLATE||pBasePlate->GetClassTypeId()==CLS_PARAMPLATE))
			{
				g_pSolidDraw->SetEntSnapStatus(pBasePlate->handle);
				break;
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("wrong parts selected,please choose again:","");
#else
				pCmdLine->FillCmdLine("选择了错误的构件，请重新选择:","");
#endif
				g_pSolidDraw->ReleaseSnapStatus();
			}
		}
		else
			return 0;
	}
	if(pBasePlate.IsHasPtr())
	{	//依据输入的基准钢板确定装配叠放板的参考坐标系(钢板坐标系)
		ucs = pBasePlate->ucs;
		g_pSolidSet->GetObjectUcs(object_ucs);
		coord_trans(ucs.origin,object_ucs,TRUE);
		vector_trans(ucs.axis_x,object_ucs,TRUE);
		vector_trans(ucs.axis_y,object_ucs,TRUE);
		vector_trans(ucs.axis_z,object_ucs,TRUE);
		g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
		//g_pSolidSet->SetDispWorkUcs();	
	}
	
	int offset_dist=0;
	if(pBasePlate->GetClassTypeId()==CLS_PLATE)
		offset_dist=(int)pBasePlate.PlatePointer()->GetThick();
	else if(pBasePlate->GetClassTypeId()==CLS_PARAMPLATE)
		offset_dist=pBasePlate.ParamPlatePointer()->thick;
	static CDesignOverlappendPlateDlg plate_dlg;
	plate_dlg.m_fBaseOffsetZ=offset_dist;
	if(plate_dlg.DoModal()!=IDOK)
		return 0;
	UCS_STRU tem_ucs;
	if(plate_dlg.m_bChooseWCS)	//绝对坐标系
		tem_ucs = theApp.env.mcs;
	else						//相对坐标系
		tem_ucs = pBasePlate->ucs;
	Ta.BeginUndoListen();
	if(pBasePlate->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pPlate= (CLDSPlate*)console.AppendPart(CLS_PLATE);
		pPlate->CopyProperty(pBasePlate.PlatePointer());
		pPlate->m_tInnerColumnNorm=pBasePlate.PlatePointer()->m_tInnerColumnNorm;	//内圆法线方向
		pPlate->m_fInnerRadius=pBasePlate.PlatePointer()->m_fInnerRadius;	//内圆半径
		pPlate->m_tInnerOrigin=pBasePlate.PlatePointer()->m_tInnerOrigin;	//内圆位置
		pPlate->designInfo.m_hBaseNode=pBasePlate.PlatePointer()->designInfo.m_hBaseNode;//板的基准节点
		pPlate->ucs=pBasePlate->ucs;
		pPlate->ucs.origin+=tem_ucs.axis_x*plate_dlg.m_fBaseOffsetX+tem_ucs.axis_y*plate_dlg.m_fBaseOffsetY+tem_ucs.axis_z*plate_dlg.m_fBaseOffsetZ;
		pPlate->cMaterial=CSteelMatLibrary::RecordAt(plate_dlg.m_iMaterial).cBriefMark;
		pPlate->iSeg=SEGI(plate_dlg.m_sSegI.GetBuffer());
		pPlate->SetPartNo(plate_dlg.m_sPartNo.GetBuffer(0));
		pPlate->Thick=plate_dlg.m_nThick;
		pPlate->face_N=pBasePlate.PlatePointer()->face_N;
		pPlate->jdb_style = pBasePlate.PlatePointer()->jdb_style;
		pPlate->HuoQuLine[0] = pBasePlate.PlatePointer()->HuoQuLine[0];
		pPlate->HuoQuLine[1] = pBasePlate.PlatePointer()->HuoQuLine[1];
		pPlate->huoqufacenorm[0] = pBasePlate.PlatePointer()->huoqufacenorm[0];
		pPlate->huoqufacenorm[1] = pBasePlate.PlatePointer()->huoqufacenorm[1];
		pPlate->top_point = pBasePlate.PlatePointer()->top_point;
		for(PROFILE_VERTEX *pVertex=pBasePlate.PlatePointer()->vertex_list.GetFirst();pVertex!=NULL;pVertex=pBasePlate.PlatePointer()->vertex_list.GetNext())
			pPlate->vertex_list.append(*pVertex);
		//for(CLsRef *pLsRef=pBasePlate.PlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pBasePlate.PlatePointer()->GetNextLsRef())
		//	pPlate->AppendLsRef((*pLsRef)->GetLsRef());
		pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	}
	else if(pBasePlate->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pPlate= (CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pPlate->CopyProperty(pBasePlate.ParamPlatePointer());
		pPlate->m_hPartWeldParent=pBasePlate->m_hPartWeldParent;
		pPlate->ucs=pBasePlate->ucs;
		//工作面法向偏移距离
		pPlate->ucs.origin+=tem_ucs.axis_x*plate_dlg.m_fBaseOffsetX+tem_ucs.axis_y*plate_dlg.m_fBaseOffsetY+tem_ucs.axis_z*plate_dlg.m_fBaseOffsetZ;
		pPlate->cMaterial=CSteelMatLibrary::RecordAt(plate_dlg.m_iMaterial).cBriefMark;
		pPlate->iSeg=SEGI(plate_dlg.m_sSegI.GetBuffer());
		pPlate->SetPartNo(plate_dlg.m_sPartNo.GetBuffer(0));
		pPlate->thick=plate_dlg.m_nThick;
		pPlate->DesignPlate();
		//for(CLsRef *pLsRef=pBasePlate.ParamPlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pBasePlate.ParamPlatePointer()->GetNextLsRef())
		//	pPlate->AppendLsRef((*pLsRef)->GetLsRef());
		pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	//g_pSolidSet->SetDispWorkUcs(FALSE);
	g_pSolidDraw->BatchClearCS(2);
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();*/
	return 0;
}

void CLDSView::OnCircularPlate()
{	//环向板
	m_nPrevCommandID=ID_CIRCULAR_PLATE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat circular plate";
#else
	m_sPrevCommandName="重复环向板";
#endif
	Command("CircularPlate");
}
	
BOOL CLDSView::FinishDesignCircularPlate(CLDSNode *pNode, CLDSLineTube *pLineTube, BOOL bEndPlate/*=FALSE*/,double manu_space/*=0*/)
{
#ifdef __COMPLEX_PART_INC_
	f3dLine baseline(pLineTube->pStart->Position(false),pLineTube->pEnd->Position(false));
	if(!baseline.PtInLine(pNode->Position(false)))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("all the nodes not on rods!");
#else
		AfxMessageBox("所选节点不在杆件上!");
#endif
		return FALSE;
	}
	baseline.startPt=pLineTube->pStart->Position(true);
	baseline.endPt=pLineTube->pEnd->Position(true);
	Ta.BeginUndoListen();
	//static CCircularPlateDlg plateDlg;
	//plateDlg.m_fInternalDiameter=0;
	//plateDlg.m_fExternalDiameter=pLineTube->GetDiameter();
	//if(bEndPlate)	//钢管封头板
	//	plateDlg.m_iDlgType = 1;
	//else 
	//	plateDlg.m_iDlgType = 0;
	CDesignCircularPlateDlg plateDlg;
	plateDlg.m_pDatumNode=pNode;
	plateDlg.m_pDatumTube=pLineTube;
	plateDlg.m_iPlateType=0;	//环向板
	plateDlg.para.R=0;
	CLDSPlate *pPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	pPlate->m_hPartWeldParent = pLineTube->handle;
	pPlate->CopyModuleInstanceInfo(pLineTube);	//调整钢板配材号与基准构件或基准节点配材号一致
	pPlate->designInfo.m_hBaseNode=pNode->handle;	//板的基准节点
	pPlate->designInfo.origin.datum_pos_style=2;
	pPlate->designInfo.origin.des_para.RODNODE.hNode=pNode->handle;
	pPlate->designInfo.origin.des_para.RODNODE.hRod=pLineTube->handle;
	CDesignLjPartPara *pLjPara=pPlate->designInfo.partList.Add(pLineTube->handle);
	pLjPara->hPart=pLineTube->handle;
	if(pNode==pLineTube->pStart)
	{
		pLjPara->start0_end1=0;	//始端连接
		if(bEndPlate)
			plateDlg.para.S=pLineTube->startOdd();
			//plateDlg.m_fNormalOffset=pLineTube->startOdd();
		pPlate->ucs.axis_z=pLineTube->Start()-pLineTube->End();
		normalize(pPlate->ucs.axis_z);
		pPlate->ucs.origin=pLineTube->Start();	//+pPlate->ucs.axis_z*pLineTube->startOdd();
	}
	else if(pNode==pLineTube->pEnd)
	{
		pLjPara->start0_end1=1;	//终端连接
		if(bEndPlate)
			plateDlg.para.S=pLineTube->endOdd();
			//plateDlg.m_fNormalOffset=pLineTube->endOdd();
		pPlate->ucs.axis_z=pLineTube->End()-pLineTube->Start();
		normalize(pPlate->ucs.axis_z);
		pPlate->ucs.origin=pLineTube->End();	//+pPlate->ucs.axis_z*pLineTube->endOdd();
	}
	else
	{
		pLjPara->start0_end1=2;	//中间连接
		pPlate->ucs.axis_z=pLineTube->End()-pLineTube->Start();
		normalize(pPlate->ucs.axis_z);
		SnapPerp(&pPlate->ucs.origin,baseline.startPt,baseline.endPt,pNode->xArcliftPos);
	}
	pPlate->ucs.axis_x=pLineTube->ucs.axis_x;
	pPlate->ucs.axis_y=pPlate->ucs.axis_z^pPlate->ucs.axis_x;
	f3dPoint columnnorm=pPlate->ucs.axis_z;	//用于椭圆弧中设定切割圆柱面的法线朝向
	UCS_STRU object_ucs,ucs=pPlate->ucs;
	g_pSolidSet->GetObjectUcs(object_ucs);
	coord_trans(ucs.origin,object_ucs,TRUE);
	vector_trans(ucs.axis_x,object_ucs,TRUE);
	vector_trans(ucs.axis_y,object_ucs,TRUE);
	vector_trans(ucs.axis_z,object_ucs,TRUE);
	g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
	//g_pSolidSet->SetDispWorkUcs();
	plateDlg.design_axis_z.vector = pPlate->ucs.axis_z;
	if(plateDlg.DoModal()!=IDOK)
	{
		//g_pSolidSet->SetDispWorkUcs(FALSE);
		g_pSolidDraw->BatchClearCS(2);
		//命令中断，清除冗余数据
		if(pPlate)
		{
			console.DispPartSet.DeleteNode(pPlate->handle);
			g_pSolidDraw->DelEnt(pPlate->handle);
			console.DeletePart(pPlate->handle);
		}
		return FALSE;
	}
	f3dPoint axis_z=plateDlg.design_axis_z.vector;
	normalize(axis_z);
	if(pPlate->ucs.axis_z!=axis_z)
	{
		f3dPoint rot_axis_vec=pPlate->ucs.axis_z^axis_z;
		double rot_ang=ACOS(pPlate->ucs.axis_z*axis_z);
		RotateVectorAroundVector(pPlate->ucs.axis_x,rot_ang,rot_axis_vec);
		RotateVectorAroundVector(pPlate->ucs.axis_y,rot_ang,rot_axis_vec);
		pPlate->ucs.axis_z=axis_z;
	}
	//g_pSolidSet->SetDispWorkUcs(FALSE);
	g_pSolidDraw->BatchClearCS(2);
	pPlate->SetPartNo(plateDlg.m_sPartNo);
	pPlate->iSeg=SEGI(plateDlg.m_sSeg.GetBuffer());
	pPlate->layer(2)=pLineTube->layer(2);	//调整环向板的图层名
	pPlate->Thick=plateDlg.thick;
	pPlate->m_fNormOffset = 0;//-pPlate->GetThick()*0.5;
	pPlate->cMaterial=plateDlg.m_cMaterial;
	//pPlate->ucs.origin+=pPlate->ucs.axis_z*plateDlg.m_fNormalOffset;
	pPlate->designInfo.origin.des_para.RODNODE.len_offset_dist=plateDlg.para.S;
	if(pNode==pLineTube->pStart)
		pPlate->designInfo.origin.des_para.RODNODE.direction=1;	//终-->始
	else 
		pPlate->designInfo.origin.des_para.RODNODE.direction=0;	//始-->终
	pPlate->designInfo.origin.UpdatePos(console.GetActiveModel());
	pPlate->ucs.origin=pPlate->designInfo.origin.Position();
	double start_rad_angle=plateDlg.para.A*RADTODEG_COEF;
	double sector_rad_angle=plateDlg.para.B*RADTODEG_COEF;
	int piece_sector_n=ftoi(plateDlg.para.B)/90;
	double piece_sector_rad_angle=sector_rad_angle/piece_sector_n;
	if(plateDlg.para.A!=0)
		RotateVectorAroundVector(pPlate->ucs.axis_x,start_rad_angle,pPlate->ucs.axis_z);
	pPlate->ucs.axis_y=pPlate->ucs.axis_z^pPlate->ucs.axis_x;
	double outter_radius = plateDlg.para.R+plateDlg.para.W;
	double inner_radius = plateDlg.para.R;
	PROFILE_VERTEX *pVertex;
	BOOL bFullCir=FALSE;
	if(fabs(plateDlg.para.B -360)<EPS)
		bFullCir=TRUE;
	if(plateDlg.para.R<EPS)
	{	//封头板 
		f3dPoint vertex_origin,vertex_start,vertex_end;
		int vertex_id=1;
		if(!bFullCir)
		{	//钢板为半圆时添加坐标原点 wht 10-08-06
			vertex_origin=pPlate->ucs.origin;
			vertex_origin.feature=1;
			vertex_origin.ID=vertex_id;
			pPlate->vertex_list.append(vertex_origin);
			vertex_id++;
		}
		vertex_start=pPlate->ucs.origin+pPlate->ucs.axis_x*outter_radius;
		vertex_start.feature=1;
		vertex_start.ID=vertex_id;
		pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(vertex_start,1));
		pVertex->sector_angle = piece_sector_rad_angle;
		pVertex->center=pPlate->ucs.origin;
		vertex_end=vertex_start;
		vertex_id++;
		for(int i=0;i<piece_sector_n;i++)
		{
			rotate_point_around_axis(vertex_end,piece_sector_rad_angle,pPlate->ucs.origin,pPlate->ucs.origin+pPlate->ucs.axis_z*100);
			vertex_end.feature=1;
			vertex_end.ID=vertex_id+i;
			if(i<piece_sector_n-1)
			{
				pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(vertex_end,1));
				pVertex->sector_angle = piece_sector_rad_angle;
			}
			else if(!bFullCir) //非整圆时需要添加以下顶点 wht 10-08-06
				pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(vertex_end,0));
			if(pVertex)
				pVertex->center=pPlate->ucs.origin;
		}
	}
	else
	{	//环形加劲板
		if(bFullCir)
		{	//全环
			pPlate->m_fInnerRadius = inner_radius;
			pPlate->m_tInnerColumnNorm=columnnorm;
			f3dPoint vertex_origin,vertex_outter_start,vertex_inner_end,vertex_outter_end;
			vertex_origin=pPlate->ucs.origin;
			/*vertex_origin.feature=1;
			vertex_inner_end=vertex_origin+pPlate->ucs.axis_x*inner_radius;
			vertex_inner_end.ID=1;
			vertex_inner_end.feature = 1;
			pPlate->vertex_list.append(PROFILE_VERTEX(vertex_inner_end));*/
			vertex_outter_start=pPlate->ucs.origin+pPlate->ucs.axis_x*outter_radius;
			vertex_outter_start.feature=1;
			vertex_outter_start.ID=2;
			pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(vertex_outter_start,1));
			pVertex->sector_angle = piece_sector_rad_angle;
			pVertex->center=pPlate->ucs.origin;
			vertex_outter_end=vertex_outter_start;
			for(int i=0;i<piece_sector_n;i++)
			{
				rotate_point_around_axis(vertex_outter_end,piece_sector_rad_angle,pPlate->ucs.origin,pPlate->ucs.origin+pPlate->ucs.axis_z*100);
				vertex_outter_end.feature=1;
				vertex_outter_end.ID=2+i;
				if(i<piece_sector_n-1)
				{
					pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(vertex_outter_end,1));//,TRUE);
					pVertex->sector_angle=piece_sector_rad_angle;
				}
				else
					pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(vertex_outter_end,0));//,TRUE);
				if(pVertex)
					pVertex->center=pPlate->ucs.origin;
			}
		}
		else
		{	//半环
			f3dPoint vertex_origin,vertex_start,vertex_end;
			vertex_origin=pPlate->ucs.origin;
			vertex_origin.feature=1;
			vertex_start=pPlate->ucs.origin+pPlate->ucs.axis_x*inner_radius;
			vertex_start.feature=1;
			vertex_start.ID=1;
			pPlate->vertex_list.append(PROFILE_VERTEX(vertex_start));
			vertex_start=pPlate->ucs.origin+pPlate->ucs.axis_x*outter_radius;
			vertex_start.feature=1;
			vertex_start.ID=2;
			pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(vertex_start,1));
			pVertex->sector_angle = piece_sector_rad_angle;
			pVertex->center=pPlate->ucs.origin;
			vertex_end=vertex_start;
			for(int i=0;i<piece_sector_n;i++)
			{
				rotate_point_around_axis(vertex_end,piece_sector_rad_angle,pPlate->ucs.origin,pPlate->ucs.origin+pPlate->ucs.axis_z*100);
				vertex_end.feature=1;
				vertex_end.ID=3+i;
				if(i==piece_sector_n-1)
					pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(vertex_end,0));
				else
				{
					pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(vertex_end,1));
					pVertex->sector_angle=piece_sector_rad_angle;
				}
				if(pVertex)
					pVertex->center=ucs.origin;
			}
			f3dPoint end_edge_vec=vertex_end-pPlate->ucs.origin;
			normalize(end_edge_vec);
			vertex_end=pPlate->ucs.origin+end_edge_vec*inner_radius;
			vertex_end.ID=pPlate->vertex_list.GetNodeNum();
			vertex_end.feature=1;
			double angle = -piece_sector_rad_angle;
			BOOL bEllipse=FALSE;
			if(fabs(columnnorm*pPlate->ucs.axis_z)<EPS_COS)	//圆弧
				bEllipse=TRUE;
			if(bEllipse)
			{
				pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(vertex_end,4));
				pVertex->column_norm = columnnorm;
				pVertex->radius=inner_radius;
			}
			else
				pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(vertex_end,1));
			if(pVertex)
			{
				pVertex->sector_angle = angle;
				pVertex->center=pPlate->ucs.origin;
			}
			for(i=0;i<piece_sector_n-1;i++)
			{
				rotate_point_around_axis(vertex_end,angle,pPlate->ucs.origin,pPlate->ucs.origin+pPlate->ucs.axis_z*100);
				vertex_end.feature=1;
				vertex_end.ID=pPlate->vertex_list.GetNodeNum();
				if(bEllipse)
				{
					pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(vertex_end,4));
					pVertex->column_norm = columnnorm;
					pVertex->radius=inner_radius;
				}
				else
					pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(vertex_end,1));
				if(pVertex)
				{
					pVertex->sector_angle = -piece_sector_rad_angle;
					pVertex->center=pPlate->ucs.origin;
				}
			}
		}
	}
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		coord_trans(pVertex->vertex,pPlate->ucs,FALSE);
	pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
#endif
	return TRUE;
}

#ifdef adadsdsdfsdfsdfsdfdsfffffffffff
int CLDSView::CircularPlate()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr,promptStr;
	int design_plate_type=0;//1:普通绕钢管的环向板2:环向封板3:环向肋板4:钢管肋板5:遮水板(即带流水槽的环向板)
	CLDSPart *pPart;
	CLDSNode *pBaseNode=NULL;
	CLDSLineTube* pBaseLineTube=NULL;
	CLDSLinePart* pAxisXRefLinePart=NULL;
	CLDSSphere *pDatumSphere=NULL;
	CLDSParamPlate *pParamPlate=NULL; //设计遮水板时用到
	CLDSPlate *pDatumPlate1=NULL,*pDatumPlate2=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("CircularPlate Please select ciruclar plate's assembling positioning tube or sphere:","");
#else
	pCmdLine->FillCmdLine("CircularPlate 请选择环向板的装配定位钢管或球体:","");
#endif
	g_pSolidDraw->ReleaseSnapStatus();
	DWORD dwOldSelPartFlag=g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_LINETUBE)|GetSingleWord(SELECTINDEX_SPHERE));
	while(1)
	{
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			long *id_arr;
			if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
			{
				pPart=console.FromPartHandle(id_arr[0]);
				if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
				{
					pBaseLineTube=(CLDSLineTube*)pPart;
					break;
				}
				else if(pPart&&pPart->GetClassTypeId()==CLS_SPHERE)
				{
					pDatumSphere=(CLDSSphere*)pPart;
					break;
				}
			}
			if(pBaseLineTube==NULL&&pDatumSphere==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("no proper parts selected，Please select ciruclar plate's assembling positioning tube or sphere:","");
#else
				pCmdLine->FillCmdLine("没有选中合适的构件，请选择环向板的装配定位钢管或球体:","");
#endif
				g_pSolidDraw->ReleaseSnapStatus();
				continue;
			}
		}
		else
		{
			if(g_pSolidSnap)
				g_pSolidSnap->SetSelectPartsType(dwOldSelPartFlag);
			return;
		}
	}
	//切换到单线图状态便于选择节点
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
	if(pDatumSphere==NULL)
	{	//基准构件为球时不用选择基准节点
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("please select the assembling location nodes on rod:","");
#else
		pCmdLine->FillCmdLine("请选择杆件上的装配定位节点:","");
#endif
		while(1)
		{
			f3dPoint* point;
			int ret = g_pSolidSnap->SnapPoint(point);
			if(ret<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			else if(ret>0)
			{
				pBaseNode=console.FromNodeHandle(point->ID);
				if(pBaseNode)
				{
					g_pSolidDraw->SetEntSnapStatus(point->ID);
					break;
				}
			}
		}
	}
	if(pDatumSphere==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("please select the first location datum plate for circular plate, if no, press enter directly:","");
	else if(pDatumSphere)	//when datum parts are sphere, a datum plate has to be selected
		pCmdLine->FillCmdLine("please choose the first piece of the datum locating steel plate:","");
#else
		pCmdLine->FillCmdLine("请选择环向板的第一块定位基准钢板,若没有直接回车:","");
	else if(pDatumSphere)	//基准构件为球时必须选择一块基准钢板
		pCmdLine->FillCmdLine("请选择环向板的第一块定位基准钢板:","");
#endif
	//切换到实体显示模式
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	g_pSolidDraw->ReleaseSnapStatus();
	dwOldSelPartFlag=g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_PLATE|SELECTINDEX_PARAMPLATE));
	while(1)
	{
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			long *id_arr;
			long id_num=g_pSolidSnap->GetLastSelectEnts(id_arr);
			if(pDatumSphere==NULL)
			{	//基准构件为球时不可能生成环向板
				if(id_num==0)
				{	//普通绕钢管的环向板或者遮水板
					design_plate_type=-1;
					break;
				}
			}
			if(id_num==1)
			{
				pPart=console.FromPartHandle(id_arr[0]);
				if(pPart&&pPart->GetClassTypeId()==CLS_PARAMPLATE)
				{
					pParamPlate=(CLDSParamPlate*)pPart;
					design_plate_type=5;	//遮水板
					break;
				}
				else if(pPart&&pPart->GetClassTypeId()==CLS_PLATE)
				{
					pDatumPlate1=(CLDSPlate*)pPart;
					if(pDatumPlate1->m_fInnerRadius>0&&fabs(pDatumPlate1->ucs.axis_z*pBaseLineTube->ucs.axis_z)>0)
						design_plate_type=5;	//遮水板
					break;
				}
			}
			if(pDatumPlate1==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("No proper parts selected, please select the first location datum plate for circular plate:","");
#else
				pCmdLine->FillCmdLine("没有选中合适的构件，请选择环向板的第一块定位基准钢板:","");
#endif
				g_pSolidDraw->ReleaseSnapStatus();
				continue;
			}
		}
		else
		{
			if(g_pSolidSnap)
				g_pSolidSnap->SetSelectPartsType(dwOldSelPartFlag);
			return 0;
		}
	}
	//跨越式钢管连板基准钢管
	CLDSLineTube *pDatumLineTube2=NULL;
	if(pDatumSphere==NULL)
	{	//基准构件为球时不必选择第2块基准钢板 
		if(design_plate_type!=1&&design_plate_type!=5&&design_plate_type!=-1)
		{	//普通环向板或遮水板都不需要选择第2块基准钢板
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("please select the second location datum plate (or tube) for circular plate< no choice permitted, press enter>:","");
#else
			pCmdLine->FillCmdLine("请选择环向板的第二块定位基准钢板(或钢管)<可不选,按回车确定>:","");
#endif
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_PLATE));
			while(1)
			{
				if(pCmdLine->GetStrFromCmdLine(cmdStr))
				{
					long *id_arr;
					if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
					{
						pPart=console.FromPartHandle(id_arr[0]);
						if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
						{
							pDatumLineTube2=(CLDSLineTube*)pPart;
							if(pDatumLineTube2->m_hPartWeldParent!=pDatumPlate1->m_hPartWeldParent)
							{
#ifdef AFX_TARG_ENU_ENGLISH
								pCmdLine->FillCmdLine("tube selected has the different welding parent parts with the first tube, please reselect the location datum tube for circular plate:","");
#else
								pCmdLine->FillCmdLine("所选钢管与第一块钢板焊接父构件不同，请重新选择环向板的定位基准钢管:","");
#endif
								pDatumLineTube2=NULL;	//防止错选数据错对下一步操作错误引导
								g_pSolidDraw->ReleaseSnapStatus();
								continue;
							}
							if(pDatumLineTube2->pStart->handle!=pBaseNode->handle&&pDatumLineTube2->pEnd->handle!=pBaseNode->handle)
							{
#ifdef AFX_TARG_ENU_ENGLISH
								pCmdLine->FillCmdLine("tube selected has no connection with datum nodes, please choose circular plate's location ray tube:","");
#else
								pCmdLine->FillCmdLine("所选钢管与基准节点无连接，请重新选择环向板的定位射线钢管:","");
#endif
								pDatumLineTube2=NULL;	//防止错选数据错对下一步操作错误引导
								g_pSolidDraw->ReleaseSnapStatus();
								continue;
							}
							break;
						}
						else if(pPart&&pPart->GetClassTypeId()==CLS_PLATE)
						{
							if(pPart->handle!=pDatumPlate1->handle&&pPart->m_hPartWeldParent==pDatumPlate1->m_hPartWeldParent)
								pDatumPlate2=(CLDSPlate*)pPart;
							else
							{
#ifdef AFX_TARG_ENU_ENGLISH
								pCmdLine->FillCmdLine("plate selected has the different welding parent parts with the first tube, please reselect circular plate's second location datum tube:","");
#else
								pCmdLine->FillCmdLine("所选钢板与第一块钢板焊接父构件不同，请重新选择环向板的第二块定位基准钢板:","");
#endif
								pDatumPlate2=NULL;	//防止错选数据错对下一步操作错误引导
								g_pSolidDraw->ReleaseSnapStatus();
								continue;
							}
							break;
						}
					}
					//仅有一块基准钢板的情况生成另一种环向肋板
					if(pDatumPlate2==NULL)	
						break;
				}
				else
				{
					g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
					return 0;
				}
			}
		}
	}
	if(pDatumSphere!=NULL)
		design_plate_type=3;//环向肋板
	if(pDatumLineTube2)
		design_plate_type=4;//跨越式钢管连板
	else if(design_plate_type==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select and confirm circular plate's reference rod in axis X direction, if no circular plate's axis X direction, be vertical with datum tube:","");
#else
		pCmdLine->FillCmdLine("请选择确认环向板X轴方向的参考杆件,若没有环向板的X轴方向将与基准钢管的轴向垂直:","");
#endif
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_LINETUBE));
		while(1)
		{
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				return 0;
			long *id_arr;
			long id_num=g_pSolidSnap->GetLastSelectEnts(id_arr);
			if(id_num==0)
				break;
			else if(id_num==1)
			{
				pPart=console.FromPartHandle(id_arr[0]);
				if(pPart&&pPart->IsLinePart())
				{
					pAxisXRefLinePart=(CLDSLinePart*)pPart;
					design_plate_type=4;//钢管肋板
					break;
				}
			}
			if(pAxisXRefLinePart==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("No proper parts selected, Please select circular plate's reference rod in axis X direction:","");
#else
				pCmdLine->FillCmdLine("没有选中合适的构件，请选择环向板X轴方向的参考杆件:","");
#endif
				g_pSolidDraw->ReleaseSnapStatus();
				continue;
			}
		}
	}
	//输入环向板加工间隙
	double manu_space=0;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("please input circular plate's process clearance<0>:");
#else
	cmdStr.Format("请输入环向板加工间隙<0>:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
			return 0;
		double space=atof(cmdStr.GetBuffer(1));
		if(cmdStr.GetLength()==0)
			manu_space=0;
		else if(space>0||fabs(space)<EPS)
			manu_space=space;
		else if(cmdStr.GetLength()>0)
		{
			pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("error in input，please input circular plate's process clearance<0>:");
#else
			cmdStr.Format("输入错误，请输入环向板加工间隙<0>:");
#endif
			pCmdLine->FillCmdLine(cmdStr,"");
			continue;
		}
		break;
	}*/
	static char cDefaultType;
	if(cDefaultType==0)
		cDefaultType='C';
	if(design_plate_type==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("please confirm the type of the circular plate,sealing plate(C)|rib plate(R)|cross tube connecting plate(S)<%C>:",cDefaultType);
#else
		cmdStr.Format("请确定环向板的类型,封板(C)|肋板(R)|跨越钢管连板(S)<%C>:",cDefaultType);
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		while(1)
		{
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				return 0;
			if(cmdStr.CompareNoCase("C")==0)
				cDefaultType='C';
			else if(cmdStr.CompareNoCase("R")==0)
				cDefaultType='R';
			else if(cmdStr.CompareNoCase("S")==0)
				cDefaultType='S';
			else if(cmdStr.GetLength()>0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("option key words needed","");
				pCmdLine->FinishCmdLine();
				cmdStr.Format("please confirm the type of the circular plate,sealing plate(C)|rib plate(R)|cross steel connecting plate(S)<%C>:",cDefaultType);
#else
				pCmdLine->FillCmdLine("需要选项关键字","");
				pCmdLine->FinishCmdLine();
				cmdStr.Format("请确定环向板的类型,封板(C)|肋板(R)|跨越钢管连板(S)<%C>:",cDefaultType);
#endif
				pCmdLine->FillCmdLine(cmdStr,"");
				continue;
			}
			break;
		}
		if(cDefaultType=='C')
			design_plate_type=2;//环向封板
		else if(cDefaultType=='R')
			design_plate_type=3;//环向肋板
		else if(cDefaultType=='S')
			design_plate_type=4;//跨越钢管的连板
	}
	else if(design_plate_type==-1)
	{
		cDefaultType='H';
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("please confirm the type of circular plate,circular plate(H)|flashing board(Z)<%C>:",cDefaultType);
#else
		cmdStr.Format("请确定环向板的类型,环向板(H)|遮水板(Z)<%C>:",cDefaultType);
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		while(1)
		{
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				return 0;
			if(cmdStr.CompareNoCase("H")==0)
				cDefaultType='H';
			else if(cmdStr.CompareNoCase("Z")==0)
				cDefaultType='Z';
			else if(cmdStr.GetLength()>0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("option key words needed","");
				pCmdLine->FinishCmdLine();
				cmdStr.Format("please confirm the type ofcircular plate,circular plate(H)|flashing board(Z)<%C>:",cDefaultType);
#else
				pCmdLine->FillCmdLine("需要选项关键字","");
				pCmdLine->FinishCmdLine();
				cmdStr.Format("请确定环向板的类型,环向板(H)|遮水板(Z)<%C>:",cDefaultType);
#endif
				pCmdLine->FillCmdLine(cmdStr,"");
				continue;
			}
			break;
		}
		if(cDefaultType=='H')
			design_plate_type=1;//环向板
		else if(cDefaultType=='Z')
			design_plate_type=5;//遮水板
	}
	
	BOOL bRet=FALSE;
	if(design_plate_type==1)		//环向板
		bRet=FinishDesignCircularPlate(pBaseNode,pBaseLineTube,FALSE,manu_space);
	else if(design_plate_type==2)	//环向封板
		bRet=FinishDesignCirCoverPlate(pBaseNode,pBaseLineTube,pDatumPlate1,pDatumPlate2,manu_space);
	else if(design_plate_type==3)	//环向肋板
	{
		if(pDatumSphere)	//球+基准钢板
			bRet=FinishDesignCirRibPlate(pDatumSphere,NULL,pDatumPlate1,NULL,manu_space);
		else if(pBaseLineTube)	//钢管+节点+一块或两块基准钢板
			bRet=FinishDesignCirRibPlate(pBaseLineTube,pBaseNode,pDatumPlate1,pDatumPlate2,manu_space);
	}
	else if(design_plate_type==4)	//钢管肋板
	{	//仅仅选择钢管肋板时才需要询问用户是否需要输入参考杆件 wht 10-08-27
		if(pDatumLineTube2==NULL)
		{	//第二个基准构件为钢管时无须选择X轴方向参照杆件 wht 10-10-26
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select and confirm circular plate's reference rod in axis X direction, if no circular plate's axis X direction, be vertical with datum tube:","");
#else
			pCmdLine->FillCmdLine("请选择确认环向板X轴方向的参考杆件,若没有环向板的X轴方向将与基准钢管的轴向垂直:","");
#endif
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_LINETUBE));
			while(1)
			{
				if(!pCmdLine->GetStrFromCmdLine(cmdStr))
					return 0;
				long *id_arr;
				long id_num=g_pSolidSnap->GetLastSelectEnts(id_arr);
				if(id_num==0)
					break;
				else if(id_num==1)
				{
					pPart=console.FromPartHandle(id_arr[0]);
					if(pPart&&pPart->IsLinePart())
					{
						pAxisXRefLinePart=(CLDSLinePart*)pPart;
						//design_plate_type=4;//钢管肋板
						break;
					}
				}
				if(pAxisXRefLinePart==NULL)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("No proper parts selected, please select circular plate's reference rod in axis X direction:","");
#else
					pCmdLine->FillCmdLine("没有选中合适的构件，请选择环向板X轴方向的参考杆件:","");
#endif
					g_pSolidDraw->ReleaseSnapStatus();
					continue;
				}
			}
			pCmdLine->FinishCmdLine();
		}
		if(pDatumLineTube2)
			bRet=FinishDesignTubeRibPlate(pBaseNode,pBaseLineTube,pDatumPlate1,pDatumLineTube2,pAxisXRefLinePart,manu_space);
		else 
			bRet=FinishDesignTubeRibPlate(pBaseNode,pBaseLineTube,pDatumPlate1,pDatumPlate2,pAxisXRefLinePart,manu_space);
	}
	else if(design_plate_type==5)	//遮水板
	{
		if(pParamPlate)
			bRet=FinishDesignWaterShieldedPlate(pBaseNode,pBaseLineTube,pParamPlate);
		else if(pDatumPlate1)
			bRet=FinishDesignWaterShieldedPlate(pBaseNode,pBaseLineTube,pDatumPlate1);
		else 
			bRet=FinishDesignWaterShieldedPlate(pBaseNode,pBaseLineTube,NULL);
	}
	m_pDoc->SetModifiedFlag();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	if(bRet)
		CircularPlate();	//Command("CircularPlate");
}
BOOL CLDSView::FinishDesignTubeRibPlate(CLDSNode *pDatumNode,CLDSLineTube *pDatumTube,CLDSPlate *pDatumPlate1,
										CLDSPart *pDatumPart2,CLDSLinePart *pAxisXRefLinePart,double manu_space/*=0*/)
{
	CLDSPart *pPart=NULL;
	CLDSLinePart *pSetUpPart=pAxisXRefLinePart;
	CDesignTubeRibPlateDlg dlg;
	dlg.m_pDatumNode=pDatumNode;
	dlg.m_pDatumTube=pDatumTube;
	dlg.m_pDatumPlate1=pDatumPlate1;
	//if(pDatumPart2&&pDatumPart2->GetClassTypeId()==CLS_PLATE)
	//	dlg.m_pRelaPart2=(CLDSPlate*)pDatumPart2;
	dlg.m_pDatumPart2=pDatumPart2;
	if(pSetUpPart)
	{
		dlg.m_pSetUpPart=pSetUpPart;
		dlg.m_bModifyZ=FALSE;
	}
	if(dlg.DoModal()!=IDOK)
		return FALSE;
	Ta.BeginUndoListen();
	CLDSParamPlate *pTubeRibPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pTubeRibPlate->m_iParamType=TYPE_TUBERIBPLATE;
	pTubeRibPlate->m_hPartWeldParent=pDatumTube->handle;
	pTubeRibPlate->CopyModuleInstanceInfo(pDatumTube);	//调整钢板配材号与基准构件或基准节点配材号一致
	pTubeRibPlate->thick=dlg.m_iThick;
	pTubeRibPlate->iSeg=dlg.m_iSegI;
	pTubeRibPlate->cMaterial = steelmat_tbl[dlg.m_iMaterial].cBriefMark;
	_snprintf(pTubeRibPlate->sPartNo,MAX_CHAR_GUIGE_16,"%s",dlg.m_sPartNo);
	pTubeRibPlate->ucs=pDatumTube->ucs;
	pTubeRibPlate->ucs.origin=pDatumNode->pos;
	pTubeRibPlate->ucs.axis_z.Set(dlg.m_fNormX,dlg.m_fNormY,dlg.m_fNormZ);
	normalize(pTubeRibPlate->ucs.axis_z);
	pTubeRibPlate->ucs.axis_y=pTubeRibPlate->ucs.axis_z^pTubeRibPlate->ucs.axis_x;
	normalize(pTubeRibPlate->ucs.axis_y);
	if(pSetUpPart)
	{
		pTubeRibPlate->ucs.axis_x=pSetUpPart->pEnd->pos-pSetUpPart->pStart->pos;
		normalize(pTubeRibPlate->ucs.axis_x);
		pTubeRibPlate->ucs.axis_y=pTubeRibPlate->ucs.axis_z^pTubeRibPlate->ucs.axis_x;
		normalize(pTubeRibPlate->ucs.axis_y);
		pTubeRibPlate->ucs.axis_z=pTubeRibPlate->ucs.axis_x^pTubeRibPlate->ucs.axis_y;
		normalize(pTubeRibPlate->ucs.axis_z);
#ifdef AFX_TARG_ENU_ENGLISH
		pTubeRibPlate->SetDesignItemValue('S',pSetUpPart->handle,"confirm tube rib plate's reference rod in axis X direction ",PARAM_POS,1);
	}
	else 
		pTubeRibPlate->SetDesignItemValue('S',0,"confirm tube rib plate's reference rod in axis X direction ",PARAM_POS,1);
	pTubeRibPlate->SetDesignItemValue('R',dlg.m_fParamR,"rib plate's Outside arc radius",PARAM_SHAPE);
	pTubeRibPlate->SetDesignItemValue('W',dlg.m_fParamW,"parameterW",PARAM_SHAPE);
	pTubeRibPlate->SetDesignItemValue('N',dlg.m_fParamN,"parameterN",PARAM_SHAPE);
	pTubeRibPlate->SetDesignItemValue('H',dlg.m_fParamH,"parameterH",PARAM_SHAPE);
#else
		pTubeRibPlate->SetDesignItemValue('S',pSetUpPart->handle,"确定钢管肋板X轴方向的参考杆件",PARAM_POS,1);
	}
	else 
		pTubeRibPlate->SetDesignItemValue('S',0,"确定钢管肋板X轴方向的参考杆件",PARAM_POS,1);
	pTubeRibPlate->SetDesignItemValue('R',dlg.m_fParamR,"肋板的外圆弧半径",PARAM_SHAPE);
	pTubeRibPlate->SetDesignItemValue('W',dlg.m_fParamW,"参数W",PARAM_SHAPE);
	pTubeRibPlate->SetDesignItemValue('N',dlg.m_fParamN,"参数N",PARAM_SHAPE);
	pTubeRibPlate->SetDesignItemValue('H',dlg.m_fParamH,"参数H",PARAM_SHAPE);
#endif
	double Y=-1,F=-1,A=-1;
	if(dlg.m_bDesignOuterPlate)
		Y=1;
	if(dlg.m_bTopArcConnection)
		A=1;
	if(dlg.m_bCirPlate)
		F=1;
	else	//非环形肋板是才需要添加该参数
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pTubeRibPlate->SetDesignItemValue('Y',Y,"positive value or zero shows that rib plate is on the outside of datum plate, otherwise on the inside");
		pTubeRibPlate->SetDesignItemValue('A',A,"positIve value or zero stands for arc connection on the rib plate vertex, otherwise straight line connection",PARAM_SHAPE);
	}
	pTubeRibPlate->SetDesignItemValue('F',F," positive value or zero indicates the rib plate is set on tube, otherwise the inside of the rib plate is semicircle arc",PARAM_SHAPE);
	pTubeRibPlate->SetDesignItemValue('D',dlg.m_fNormOffset,"rib plate's normal offset distance");
	pTubeRibPlate->SetDesignItemValue('P',pDatumNode->handle,"datum point that defines tube and rib plate's location",PARAM_POS,1);
	if(pDatumPlate1)
		pTubeRibPlate->SetDesignItemValue('B',pDatumPlate1->handle,"datum plate that defines tube rib plate's location",PARAM_POS,1);
	if(pDatumPart2)
		pTubeRibPlate->SetDesignItemValue('C',pDatumPart2->handle,"datum plate or tube that defines plate rib plate's location",PARAM_POS,1);
	//tooling gap wht 10-08-28
	pTubeRibPlate->SetDesignItemValue('V',manu_space,"processing clearance value");
#else
		pTubeRibPlate->SetDesignItemValue('Y',Y,"正值或零表示肋板在基准钢板的外侧，否则位于内侧");
		pTubeRibPlate->SetDesignItemValue('A',A,"正值或零表示肋板顶部为圆弧连接，否则肋板顶部为直线连接",PARAM_SHAPE);
	}
	pTubeRibPlate->SetDesignItemValue('F',F,"正值或零表示该板为套在钢管上的肋板，否则该板内侧为半圆弧",PARAM_SHAPE);
	pTubeRibPlate->SetDesignItemValue('D',dlg.m_fNormOffset,"肋板的法向偏移距离");
	pTubeRibPlate->SetDesignItemValue('P',pDatumNode->handle,"确定钢管肋板位置的基准点",PARAM_POS,1);
	if(pDatumPlate1)
		pTubeRibPlate->SetDesignItemValue('B',pDatumPlate1->handle,"确定钢管肋板位置的基准钢板",PARAM_POS,1);
	if(pDatumPart2)
		pTubeRibPlate->SetDesignItemValue('C',pDatumPart2->handle,"确定钢管肋板位置的基准钢板或钢管",PARAM_POS,1);
	//加工间隙 wht 10-08-28
	pTubeRibPlate->SetDesignItemValue('V',manu_space,"加工间隙值");
#endif
	pTubeRibPlate->DesignPlate();
	pTubeRibPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pTubeRibPlate->GetSolidPartObject());	
	if(dlg.m_bMirCreate)
	{
		CLDSParamPlate *pMirPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pMirPlate->CopyProperty(pTubeRibPlate);
		pMirPlate->CopyModuleInstanceInfo(pDatumTube);	//调整钢管肋板的图层名
		pMirPlate->m_hPartWeldParent=pDatumTube->handle;
#ifdef AFX_TARG_ENU_ENGLISH
		pMirPlate->SetDesignItemValue('W',dlg.m_fParamN,"parameter W",PARAM_SHAPE);
		pMirPlate->SetDesignItemValue('N',dlg.m_fParamW,"parameter N",PARAM_SHAPE);
		pMirPlate->SetDesignItemValue('Y',-Y,"positive value or zero shows that rib plate is on the outside of datum plate, otherwise, inside");
#else
		pMirPlate->SetDesignItemValue('W',dlg.m_fParamN,"参数W",PARAM_SHAPE);
		pMirPlate->SetDesignItemValue('N',dlg.m_fParamW,"参数N",PARAM_SHAPE);
		pMirPlate->SetDesignItemValue('Y',-Y,"正值或零表示肋板在基准钢板的外侧，否则位于内侧");
#endif
		pMirPlate->ucs=pTubeRibPlate->ucs;
		pMirPlate->DesignPlate();
		pMirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pMirPlate->GetSolidPartObject());
	}
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	return TRUE;
}
#endif

int CLDSView::CircularPlate()
{
#ifdef __COMPLEX_PART_INC_
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	DWORD dwhObj=0,dwExportFlag=0;
	g_pSolidDraw->ReleaseSnapStatus();
	//拾取环向板的定位基准构件
	SmartPartPtr pDatumPart;
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_SPHERE)|GetSingleWord(SELECTINDEX_LINETUBE));
	verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
	CString cmdStr,valueStr;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate Please select circular plate's assembling positioning tube or sphere:");
#else
	cmdStr.Format("CircularPlate 请选择环向板的装配定位钢管或球体:");
#endif
	while(pDatumPart.IsNULL())
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		pDatumPart=Ta.Parts.FromHandle(obj.hRelaObj,CLS_SPHERE,CLS_LINETUBE);
	}
	g_pSolidDraw->SetEntSnapStatus(pDatumPart->handle);
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pDatumPart->handle));
	//选择第一个定位依赖钢板|依赖角钢
	BOLTSET boltSet;
	SmartPartPtr pRelaPart1,pRelaPart2;
	verifier.ClearSnapFlag();
	verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE)|GetSingleWord(SELECTINDEX_LINEANGLE));
#ifdef AFX_TARG_ENU_ENGLISH
	if(pDatumPart->GetClassTypeId()==CLS_SPHERE)
		cmdStr.Format("CircularPlate Please select datum plate of the circular plate:");
	else
		cmdStr.Format("CircularPlate Please select the first location datum plate of the circular plate,if no,press enter directly:");
#else
	if(pDatumPart->GetClassTypeId()==CLS_SPHERE)
		cmdStr.Format("CircularPlate 请选择环向板的定位依赖钢板:");
	else
		cmdStr.Format("CircularPlate 请选择环向板的第一块定位依赖钢板或依赖角钢,若没有直接回车:");
#endif
	while(true)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		pRelaPart1=Ta.Parts.FromHandle(obj.hRelaObj,CLS_PLATE,CLS_PARAMPLATE,CLS_LINEANGLE);
		if(pDatumPart->GetClassTypeId()==CLS_SPHERE&&(pRelaPart1.IsNULL()||pRelaPart1->GetClassTypeId()==CLS_PARAMPLATE))	//球体上的环向板需要有定位基准钢板
			continue;
		break;
	}
	long hPartWeldParent1=0,hPartWeldParent2=0;
	if(pRelaPart1.IsHasPtr())
	{
		g_pSolidDraw->SetEntSnapStatus(pRelaPart1->handle);
		if(pRelaPart1->IsPlate())
			pCmdLine->FinishCmdLine(CXhChar16("钢板0x%X",pRelaPart1->handle));
		else if(pRelaPart1->IsAngle())
			pCmdLine->FinishCmdLine(CXhChar16("角钢0x%X",pRelaPart1->handle));
		hPartWeldParent1=pRelaPart1->m_hPartWeldParent;
	}
	if(pDatumPart->IsTube()&&pRelaPart1.IsHasPtr()&&pRelaPart1->IsPlate())
	{	//选择第二个定位依赖构件
		if(pRelaPart1->GetClassTypeId()==CLS_PLATE||(pRelaPart1->GetClassTypeId()==CLS_PARAMPLATE&&!pRelaPart1.ParamPlatePointer()->IsFL()))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("CircularPlate Please select the second location datum plate(or tube) of the circular plate, if no, press enter directly:");
#else
			cmdStr.Format("CircularPlate 请选择环向板的第二块定位基准钢板(或钢管),若没有直接回车:");
#endif
			verifier.ClearSnapFlag();
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE)|GetSingleWord(SELECTINDEX_LINETUBE));
			while(1)
			{
				pCmdLine->FillCmdLine(cmdStr,"");
				if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
				{
					pCmdLine->CancelCmdLine();
					return 0;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				pRelaPart2=Ta.Parts.FromHandle(obj.hRelaObj,CLS_LINETUBE,CLS_PLATE,CLS_PARAMPLATE);
				if(pRelaPart2.IsHasPtr()&&pRelaPart2->IsPlate())
					hPartWeldParent2=pRelaPart2->m_hPartWeldParent;	
				if(hPartWeldParent2>0X20&&hPartWeldParent2!=hPartWeldParent1)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					cmdStr.Format("CircularPlate parts selected has the different welding parent parts with the first tube, please reselect circular plate's location datum plate (or tube):");
#else
					cmdStr.Format("CircularPlate 所选构件与第一块钢板焊接父构件不同，请重新选择环向板的定位基准钢板(或钢管):");
#endif
					pCmdLine->FillCmdLine(cmdStr,"");
					pRelaPart2=NULL;
					continue;
				}
				break;
			}
		}
		if(pRelaPart2.IsHasPtr())
		{
			g_pSolidDraw->SetEntSnapStatus(pRelaPart2->handle);
			pCmdLine->FinishCmdLine(CXhChar16("0x%X",pRelaPart2->handle));
		}
		
	}
	else if(pRelaPart1.IsHasPtr()&&pRelaPart1->GetClassTypeId()==CLS_LINEANGLE)
	{	//选择环板依赖螺栓
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("CircularPlate please select all the relative bolts of circular plate:","");
#else
		pCmdLine->FillCmdLine("CircularPlate 请选择环向板关联螺栓:","");
#endif
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
		OpenWndSel();	//开窗选择螺栓
		while(1)
		{
			if(pCmdLine->GetStrFromCmdLine(cmdStr)==FALSE)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}	
			long *id_arr;
			int n=g_pSolidSnap->GetLastSelectEnts(id_arr);
			for(int i=0;i<n;i++)
			{
				CLDSBolt* pBolt=(CLDSBolt*)Ta.Parts.FromHandle(id_arr[i],CLS_BOLT);
				if(pBolt)
					boltSet.append(pBolt);
			}
			if(boltSet.GetNodeNum()>0)
				break;
		}
		OnOperOther();	//取消开窗操作状态
	}
	//选择环板基准节点
	CLDSNode *pDatumNode=NULL;
	if(pDatumPart->GetClassTypeId()==CLS_SPHERE)
		pDatumNode=Ta.Node.FromHandle(pDatumPart.SpherePointer()->hDatumNode);
	if(pDatumNode==NULL&&pRelaPart1.IsHasPtr()&&pRelaPart1->IsPlate())
		pDatumNode=Ta.Node.FromHandle(pRelaPart1.pBasicPlate->m_hBaseNode);
	if(pDatumNode==NULL&&pRelaPart2.IsHasPtr()&&pRelaPart2->IsPlate())
		pDatumNode=Ta.Node.FromHandle(pRelaPart2.pBasicPlate->m_hBaseNode);
	if(pDatumPart->GetClassTypeId()==CLS_LINETUBE && pDatumNode==NULL&&pRelaPart1.IsNULL())
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate Please select assembling location node on rods:");
#else
		cmdStr.Format("CircularPlate 请选择钢管上的装配定位节点:");
#endif
		verifier.ClearSnapFlag();
		verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
		verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
		CDisplayNodeAtFrontLife displayNode;
		displayNode.DisplayNodeAtFront();
		while(pDatumNode==NULL)
		{
			pCmdLine->FillCmdLine(cmdStr,"");
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			pDatumNode=Ta.Node.FromHandle(obj.hRelaObj);
		}
		g_pSolidDraw->SetEntSnapStatus(pDatumNode->handle);
		pCmdLine->FinishCmdLine(CXhChar16("节点0x%X",pDatumNode->handle));
	}
	//选择环向板设计类型
	int iPlateType=pRelaPart1.IsHasPtr()&&pRelaPart1->IsPlate()?2:0;	//0.普通环向板 1.封板 2.环向肋板 3.遮水板 4.走道板
	if(pRelaPart1.IsHasPtr()&&!pRelaPart2.IsHasPtr()&&pRelaPart1->GetClassTypeId()==CLS_PARAMPLATE&&pRelaPart1.ParamPlatePointer()->IsFL())
		iPlateType=3;	//选中法兰优先设计遮水板
	else if(pRelaPart1.IsHasPtr()&&pRelaPart2.IsNULL()&&pRelaPart1->GetClassTypeId()==CLS_PARAMPLATE
		&&pRelaPart1.ParamPlatePointer()->m_iParamType==TYPE_TUBESTAYWIREPLATE)
		iPlateType=1;	//选中拉线板优先设计加劲封板
	else if(pRelaPart1.IsHasPtr()&&pRelaPart1->GetClassTypeId()==CLS_LINEANGLE)
		iPlateType=4;
	/*根据李晓仲、刘伟反应，换向板命令直接通过对话框进行设计更加方便
			不需要通过命令栏输入参数进行分布设计  wxc--2017.6.15
	*/
	CUndoOperObject undo(&Ta,true);
	CLDSParamPlate* pCirPlate=NULL;
	CDesignCircularPlateDlg cirPlateDlg;
	if(iPlateType==0)
		cirPlateDlg.m_iPlateType=0;	//普通环向板
	else if(iPlateType==1)
		cirPlateDlg.idPlateType=CDesignCircularPlateDlg::TYPE_COVERPLATE2;	//环向封板
	else if(iPlateType==3)
		cirPlateDlg.idPlateType=CDesignCircularPlateDlg::TYPE_SHIELD_WATER;	//遮水板
	else if(iPlateType==4)
		cirPlateDlg.idPlateType=CDesignCircularPlateDlg::TYPE_AISLE_PLATE;	//走道板
	if(pDatumPart->GetClassTypeId()==CLS_SPHERE)
		cirPlateDlg.m_pDatumSphere=pDatumPart.SpherePointer();
	else if(pDatumPart->GetClassTypeId()==CLS_LINETUBE)
		cirPlateDlg.m_pDatumTube=pDatumPart.LineTubePointer();
	cirPlateDlg.m_pDatumNode=pDatumNode;
	if(pRelaPart1.IsHasPtr())
		cirPlateDlg.m_pRelaPart1=pRelaPart1.PartPointer();
	if(pRelaPart2.IsHasPtr())
		cirPlateDlg.m_pRelaPart2=pRelaPart2.PartPointer();
	for(CLDSBolt* pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
		cirPlateDlg.LsRefList.append(pBolt->GetLsRef());
	if(cirPlateDlg.DoModal()==IDOK)
		pCirPlate=cirPlateDlg.CreateCircularPlate();
	//
	g_pSolidDraw->ReleaseSnapStatus();
	if(pCirPlate)
	{
		g_pSolidDraw->SetEntSnapStatus(pCirPlate->handle);
		UpdatePropertyPage(pCirPlate,FALSE);
	}
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
#endif
	return 0;
}
//普通环向板
CLDSParamPlate* CLDSView::DesignSimpleCirRibPlate(CLDSLineTube* pDatumTube,CLDSNode* pDatumNode)
{
#ifdef __COMPLEX_PART_INC_
	if(pDatumTube==NULL || pDatumNode==NULL)
		return NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//设置环向板的默认参数，生成实体
	CDesignCircularPlateDlg cirPlateDlg;
	cirPlateDlg.m_pDatumNode=pDatumNode;
	cirPlateDlg.m_pDatumTube=pDatumTube;
	cirPlateDlg.m_iPlateType=0;
	cirPlateDlg.para.A=0;	//起始角度
	cirPlateDlg.para.V=1;	//加工间隙
	cirPlateDlg.InitCirPlatePara();
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(cirPlateDlg.m_pCirPlate->handle);
	//显示示意图
	CLDSView::PROMPT_LIFE lifeprompt;
	lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_PLATE);
	CString cmdStr,valueStr;
	//输入扇形夹角β
	double angle=cirPlateDlg.para.B;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's offset Angle<%.f>:",angle);
#else
	cmdStr.Format("CircularPlate 请输入环向板的扇形夹角β<%.f>:",angle);
#endif
	while(true)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(!pCmdLine->GetStrFromCmdLine(valueStr))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		if(valueStr.GetLength()==0 ||(CmdStrToNumber(valueStr,&angle)&&angle>0))
		{
			cirPlateDlg.para.B=angle;
			break;
		}
	}
	cirPlateDlg.ModifyPlateParam('B',cirPlateDlg.para.B);
	//输入初始偏移角度
	if(cirPlateDlg.para.B<360)
	{	
		cirPlateDlg.WorkPlaneSketch(1);
		angle=cirPlateDlg.para.A;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input circulaer plate's offset Angle<0>:");
#else
		cmdStr.Format("CircularPlate 请输入环向板的定位偏转角α<0>:");
#endif
		while(true)
		{
			pCmdLine->FillCmdLine(cmdStr,"");
			if(!pCmdLine->GetStrFromCmdLine(valueStr))
			{
				pCmdLine->CancelCmdLine();
				return NULL;
			}
			if(valueStr.GetLength()==0 ||CmdStrToNumber(valueStr,&angle))
			{
				cirPlateDlg.para.A=angle;
				break;
			}
		}
		cirPlateDlg.DelWorkPlaneSketch();
		cirPlateDlg.ModifyPlateParam('A',cirPlateDlg.para.A);
	}
	//输入沿轴向偏移量
	cirPlateDlg.WorkPlaneSketch(2);
	double S=cirPlateDlg.para.S;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's offset distance<%.0f>:",S);
#else
	cmdStr.Format("CircularPlate 请输入普通环向板的轴向偏移量<%.0f>:",S);
#endif
	while(true)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(!pCmdLine->GetStrFromCmdLine(valueStr))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		if(valueStr.GetLength()==0 ||CmdStrToNumber(valueStr,&S))
		{
			cirPlateDlg.para.S=S;
			break;
		}
	}
	cirPlateDlg.DelWorkPlaneSketch();
	cirPlateDlg.ModifyPlateParam('S',cirPlateDlg.para.S);
	//输入外环半径
	double R=cirPlateDlg.para.R;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's R<%.1f>:",R);
#else
	cmdStr.Format("CircularPlate 请输入普通环向板的外环半径<%.1f>:",R);
#endif
	while(true)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(!pCmdLine->GetStrFromCmdLine(valueStr))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		if(valueStr.GetLength()==0 ||(CmdStrToNumber(valueStr,&R)&&R>pDatumTube->GetDiameter()*0.5))
		{
			cirPlateDlg.para.R=R;
			break;
		}
	}
	cirPlateDlg.ModifyPlateParam('R',cirPlateDlg.para.R);
	//显示环向板对话框
	if(cirPlateDlg.DoModal()==IDOK)
		return cirPlateDlg.m_pCirPlate;
#endif
	return NULL;
}
//遮水板
CLDSParamPlate* CLDSView::DesignWaterShieldedPlate(CLDSLineTube* pDatumTube,CLDSNode* pDatumNode,CLDSParamPlate* pFl)
{
#ifdef __COMPLEX_PART_INC_
	if(pDatumTube==NULL || pDatumNode==NULL ||pFl==NULL)
		return NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//设置环向板的默认参数，生成实体
	CDesignCircularPlateDlg cirPlateDlg;
	cirPlateDlg.m_pDatumNode=pDatumNode;
	cirPlateDlg.m_pDatumTube=pDatumTube;
	cirPlateDlg.m_pRelaPart1=pFl;
	cirPlateDlg.m_iPlateType=6;
	cirPlateDlg.InitCirPlatePara();
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(cirPlateDlg.m_pCirPlate->handle);
	//显示示意图
	CLDSView::PROMPT_LIFE lifeprompt;
	lifeprompt.SetPromptPicture(IDB_BMP_SHIELDEDWATER_PLATE);
	CString cmdStr,valueStr;
	//确定定位角基准边
	cirPlateDlg.WorkPlaneSketch(1);
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate select circular plate's type:[0.base-line|1.weld-line]<0>:");
#else
	cmdStr.Format("CircularPlate 请选择遮水板的定位基准边[0.基准边|1.焊道线]<0>:");
#endif
	while(true)
	{
		pCmdLine->FillCmdLine(cmdStr,"");;
		if(!pCmdLine->GetStrFromCmdLine(valueStr,CCmdLineDlg::KEYWORD_AS_RETURN,"1|2"))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		if(valueStr.GetLength()==0||(valueStr[0]>='0'&&valueStr[0]<='1'))
		{
			if(valueStr.GetLength()==0||valueStr[0]=='0')
				cirPlateDlg.water.E=0;
			else
				cirPlateDlg.water.E=1;
			break;
		}
	}
	cirPlateDlg.DelWorkPlaneSketch();
	cirPlateDlg.ModifyPlateParam('E',cirPlateDlg.water.E);
	//输入法向偏移量
	cirPlateDlg.WorkPlaneSketch(3);
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's offset distance<%.0f>:",cirPlateDlg.water.S);
#else
	cmdStr.Format("CircularPlate 请输入遮水板的法向偏移距离<%.0f>:",cirPlateDlg.water.S);
#endif
	while(true)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(!pCmdLine->GetStrFromCmdLine(valueStr))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		if(valueStr.GetLength()==0 ||CmdStrToNumber(valueStr,&cirPlateDlg.water.S))
			break;
	}
	cirPlateDlg.DelWorkPlaneSketch();
	cirPlateDlg.ModifyPlateParam('S',cirPlateDlg.water.S);
	//输入外环直径
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's Diameter<%.0f>:",cirPlateDlg.water.W);
#else
	cmdStr.Format("CircularPlate 请输入遮水板的外环直径<%.0f>:",cirPlateDlg.water.W);
#endif
	while(true)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(!pCmdLine->GetStrFromCmdLine(valueStr))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		if(valueStr.GetLength()==0 ||CmdStrToNumber(valueStr,&cirPlateDlg.water.W))
			break;
	}
	cirPlateDlg.ModifyPlateParam('W',cirPlateDlg.water.W);
	//输入两流水槽之间的夹角
	double angle=cirPlateDlg.water.A;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's angle<%.0f>:",angle);
#else
	cmdStr.Format("CircularPlate 请输入遮水板的两流水槽的夹角<%.0f>:",angle);
#endif
	while(true)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(!pCmdLine->GetStrFromCmdLine(valueStr))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		if(valueStr.GetLength()==0 ||CmdStrToNumber(valueStr,&angle))
		{
			cirPlateDlg.water.A=angle;
			break;
		}
	}
	cirPlateDlg.ModifyPlateParam('A',cirPlateDlg.water.A);
	if(cirPlateDlg.water.A>0)	//两流水槽夹角>0,表示有流水槽
	{	//两流水槽角平分线与基准边的偏移角
		angle=cirPlateDlg.water.O;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input water-slot offet angle<%.0f>:",angle);
#else
		cmdStr.Format("CircularPlate 输入两流水槽角平分线与基准边的偏移角<%.0f>:",angle);
#endif
		while(true)
		{
			pCmdLine->FillCmdLine(cmdStr,"");
			if(!pCmdLine->GetStrFromCmdLine(valueStr))
			{
				pCmdLine->CancelCmdLine();
				return NULL;
			}
			if(valueStr.GetLength()==0 ||CmdStrToNumber(valueStr,&angle))
			{
				cirPlateDlg.water.O=angle;
				break;
			}
		}
		cirPlateDlg.ModifyPlateParam('O',cirPlateDlg.water.O);
		//输入流水槽长度、宽度、深度
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input water slot's Length(L),Width(W),Heigh(H)<%.f,%.f>:",cirPlateDlg.water.L,cirPlateDlg.water.K,cirPlateDlg.water.H);
#else
		cmdStr.Format("CircularPlate 输入流水槽的长(L),宽(W),高(H)<%.f,%.f,%.f>:",cirPlateDlg.water.L,cirPlateDlg.water.K,cirPlateDlg.water.H);
#endif
		while(true)
		{
			pCmdLine->FillCmdLine(cmdStr,"");
			if(!pCmdLine->GetStrFromCmdLine(valueStr))
			{
				pCmdLine->CancelCmdLine();
				return NULL;
			}
			if(valueStr.GetLength()==0||(CmdStrToNumber(valueStr,&cirPlateDlg.water.L,&cirPlateDlg.water.K,&cirPlateDlg.water.H)&&
				cirPlateDlg.water.L>0&&cirPlateDlg.water.K>0&&cirPlateDlg.water.H>0))
				break;
		}
		cirPlateDlg.ModifyPlateParam('L',cirPlateDlg.water.L);
		cirPlateDlg.ModifyPlateParam('K',cirPlateDlg.water.K);
		cirPlateDlg.ModifyPlateParam('H',cirPlateDlg.water.H);
	}
	//显示环向板对话框
	if(cirPlateDlg.DoModal()==IDOK)
		return cirPlateDlg.m_pCirPlate;
#endif
	return NULL;
}
//环向封板
CLDSParamPlate* CLDSView::DesignCircularCoverPlate(CLDSLineTube* pDatumTube,CLDSNode* pDatumNode,CLDSPlate* pRelaPlate1,CLDSPlate* pRelaPlate2/*=NULL*/)
{
#ifdef __COMPLEX_PART_INC_
	if(pDatumTube==NULL || pDatumNode==NULL ||pRelaPlate1==NULL)
		return NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//设置环向板的默认参数，生成实体
	CDesignCircularPlateDlg cirPlateDlg;
	cirPlateDlg.m_pDatumNode=pDatumNode;
	cirPlateDlg.m_pDatumTube=pDatumTube;
	cirPlateDlg.m_pRelaPart1=pRelaPlate1;
	cirPlateDlg.m_pRelaPart2=pRelaPlate2;
	cirPlateDlg.m_iPlateType=1;
	cirPlateDlg.para.S=1;		//默认设计上封板
	cirPlateDlg.InitCirPlatePara();
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(cirPlateDlg.m_pCirPlate->handle);
	//显示示意图
	CLDSView::PROMPT_LIFE lifeprompt;
	lifeprompt.SetPromptPicture((cirPlateDlg.para.nCirCoverPlateType==0)?IDB_BMP_CIRCULAR_COVERPLATE2:IDB_BMP_CIRCULAR_COVERPLATE3);
	CString cmdStr,valueStr;
	//确定封板类型
	if(pRelaPlate2==NULL)
	{	//简化封板
		cirPlateDlg.WorkPlaneSketch(5);
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input circulaer plate's position[0.Common|1.Y+|2.Y-]<0>:");
#else
		cmdStr.Format("CircularPlate 请确定简化封板的类型[0.普通|1.Y+侧|2.Y-侧]<0>:");
#endif
		while(true)
		{
			pCmdLine->FillCmdLine(cmdStr,"");
			if(!pCmdLine->GetStrFromCmdLine(valueStr,CCmdLineDlg::KEYWORD_AS_RETURN,"0|1|2"))
			{
				pCmdLine->CancelCmdLine();
				return NULL;
			}
			if(valueStr.GetLength()==0||valueStr[0]=='0'||valueStr[0]=='1'||valueStr[0]=='2')
			{
				if(valueStr.GetLength()==0||valueStr[0]=='0')
					cirPlateDlg.para.nCirCoverPlateType=0;
				else if(valueStr.GetLength()>0&&valueStr[0]=='1')
					cirPlateDlg.para.nCirCoverPlateType=1;	//Y+侧
				else
					cirPlateDlg.para.nCirCoverPlateType=-1;	//Y-侧
				break;
			}
		}
		cirPlateDlg.DelWorkPlaneSketch();
		cirPlateDlg.ModifyPlateParam('Y',cirPlateDlg.para.nCirCoverPlateType);
		lifeprompt.SetPromptPicture((cirPlateDlg.para.nCirCoverPlateType==0)?IDB_BMP_CIRCULAR_COVERPLATE2:IDB_BMP_CIRCULAR_COVERPLATE3);
	}
	//输入封板扇形夹角
	double angle=cirPlateDlg.para.B;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's angle<%.0f>:",angle);
#else
	cmdStr.Format("CircularPlate 请输入环向封板的扇形夹角<%.0f>:",angle);
#endif
	while(true)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(!pCmdLine->GetStrFromCmdLine(valueStr))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		//环向封板扇形角必须在0~360之内,否则为无效数字
		if(valueStr.GetLength()==0 ||(CmdStrToNumber(valueStr,&angle)&&angle>0&&angle<360))
		{
			cirPlateDlg.para.B=angle;
			break;
		}
	}
	cirPlateDlg.ModifyPlateParam('B',cirPlateDlg.para.B);
	if(cirPlateDlg.para.nCirCoverPlateType==0)
	{	//输入封板的定位旋转角
		cirPlateDlg.WorkPlaneSketch(4);
		angle=cirPlateDlg.para.A;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input circulaer plate's offset angle<%.0f>:",angle);
#else
		cmdStr.Format("CircularPlate 请输入环向封板的定位偏转角<%.0f>:",angle);
#endif
		while(true)
		{
			pCmdLine->FillCmdLine(cmdStr,"");
			if(!pCmdLine->GetStrFromCmdLine(valueStr))
			{
				pCmdLine->CancelCmdLine();
				return NULL;
			}
			if(valueStr.GetLength()==0 ||CmdStrToNumber(valueStr,&angle))
			{
				cirPlateDlg.para.A=angle;
				break;
			}
		}
		cirPlateDlg.DelWorkPlaneSketch();
		cirPlateDlg.ModifyPlateParam('A',cirPlateDlg.para.A);
	}
	else
	{
		//输入封板的参数N
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input circulaer plate's shape param N<%.0f>:",cirPlateDlg.para.N);
#else
		cmdStr.Format("CircularPlate 请输入环向封板的外形参数N<%.0f>:",cirPlateDlg.para.N);
#endif
		while(true)
		{
			pCmdLine->FillCmdLine(cmdStr,"");
			if(!pCmdLine->GetStrFromCmdLine(valueStr))
			{
				pCmdLine->CancelCmdLine();
				return NULL;
			}
			if(valueStr.GetLength()==0 ||CmdStrToNumber(valueStr,&cirPlateDlg.para.N))
				break;
		}
		cirPlateDlg.ModifyPlateParam('N',cirPlateDlg.para.N);
	}
	//输入封板的台口高度H和环形带宽
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's shape param H,param W<%.0f,%.0f>:",cirPlateDlg.para.H,cirPlateDlg.para.W);
#else
	cmdStr.Format("CircularPlate 请输入环向封板的台口高度(H),环形宽度(W)<%.0f,%.0f>:",cirPlateDlg.para.H,cirPlateDlg.para.W);
#endif
	while(true)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(!pCmdLine->GetStrFromCmdLine(valueStr))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		if(valueStr.GetLength()==0 ||(CmdStrToNumber(valueStr,&cirPlateDlg.para.H,&cirPlateDlg.para.W)&&cirPlateDlg.para.H>0&&cirPlateDlg.para.W>0))
			break;
	}
	cirPlateDlg.ModifyPlateParam('D',cirPlateDlg.para.H);
	cirPlateDlg.ModifyPlateParam('W',cirPlateDlg.para.W);
	//是否设计下封板
	int iType=0;
#ifdef AFX_TARG_ENU_ENGLISH
	if(cirPlateDlg.para.nCirCoverPlateType==0)
		cmdStr.Format("CircularPlate select relate plate design mode:[0.No|1.Down|2.Mir|3.Down & Mir]<0>:");
	else
		cmdStr.Format("CircularPlate select relate plate design mode:[0.No|1.Down]<0>:");
#else
	if(cirPlateDlg.para.nCirCoverPlateType==0)
		cmdStr.Format("CircularPlate 请选择关联封板设计模式:[0.无关联|1.下侧封板|2.对侧封板|3.下侧及对侧封板]<0>:");
	else
		cmdStr.Format("CircularPlate 请选择关联封板设计模式:[0.无关联|1.下侧封板]<0>:");
#endif
	while(true)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(!pCmdLine->GetStrFromCmdLine(valueStr,CCmdLineDlg::KEYWORD_AS_RETURN,"0|1|2|3"))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		if(valueStr.GetLength()==0||(valueStr[0]>='0'&&valueStr[0]<='3'))
		{
			if(valueStr.GetLength()>0)
				iType=atoi(valueStr);
			break;
		}
	}
	if(iType>0)
	{	//设计上下封板
		cirPlateDlg.ClearCirPlate();
		cirPlateDlg.m_bDisplayDlg=FALSE;
		cirPlateDlg.para.S=(iType==1||iType==3)?0:1;
		cirPlateDlg.para.bCreateMirPlate=(iType>1)?TRUE:FALSE;
		cirPlateDlg.m_pCirPlate=cirPlateDlg.CreateCircularPlate();
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(cirPlateDlg.m_pCirPlate->handle);
		return cirPlateDlg.m_pCirPlate;
	}
	//显示环向板对话框
	if(cirPlateDlg.DoModal()==IDOK)
		return cirPlateDlg.m_pCirPlate;
#endif
	return NULL;
}
//2WL肋板|2WA肋板|3WL肋板|3WA肋板
CLDSParamPlate* CLDSView::DesignCircularRibPlate(SmartPartPtr pDatumPart,CLDSNode* pDatumNode,CLDSGeneralPlate* pRelaPlate,SmartPartPtr pRelaPart2)
{
#ifdef __COMPLEX_PART_INC_
	if(pDatumPart.IsNULL() || pDatumNode==NULL ||pRelaPlate==NULL)
		return NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//设置环向板的默认参数，生成实体
	CDesignCircularPlateDlg cirPlateDlg;
	if(pDatumPart->GetClassTypeId()==CLS_SPHERE)
		cirPlateDlg.m_pDatumSphere=pDatumPart.SpherePointer();
	else if(pDatumPart->GetClassTypeId()==CLS_LINETUBE)
		cirPlateDlg.m_pDatumTube=pDatumPart.LineTubePointer();
	cirPlateDlg.m_pDatumNode=pDatumNode;
	cirPlateDlg.m_pRelaPart1=pRelaPlate;
	cirPlateDlg.m_pRelaPart2=(pRelaPart2.IsHasPtr())?pRelaPart2.PartPointer():NULL;
	if(pRelaPart2.IsNULL())
	{
		if(pRelaPlate->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pRelaPlate)->IsTransPlate())
		{	//关联基板为贯通连板
			cirPlateDlg.m_iPlateType=3;			//3WA肋板
			cirPlateDlg.para.nDHRelativeType=1;	//D、H为独立参数 即跨越连板
		}
		else
		{
			cirPlateDlg.m_iPlateType=4;		//2WL肋板
			cirPlateDlg.para.D=0;			//通过扇形夹角确定外形
		}
	}
	else if(pRelaPart2->GetClassTypeId()==CLS_LINETUBE)
	{
		cirPlateDlg.m_iPlateType=3;			//3WA肋板
		cirPlateDlg.para.nDHRelativeType=1; //D、H为独立参数 即跨越连板
	}
	else 
	{
		cirPlateDlg.m_iPlateType=2;			//3WL肋板
		cirPlateDlg.para.nDHRelativeType=2; //启用H参数
	}
	CString cmdStr,valueStr;
	if(cirPlateDlg.m_iPlateType==2 || cirPlateDlg.m_iPlateType==4)
	{	//选择自由边的类型：0.直线|1.圆弧
		int iEdgeType=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate select circulaer plate's free edge type[0.Line|1.Arc]<0>:");
#else
		cmdStr.Format("CircularPlate 选择环向肋板自由边的类型[0.直线|1.圆弧]<0>:");
#endif
		while(true)
		{
			pCmdLine->FillCmdLine(cmdStr,"");
			if(!pCmdLine->GetStrFromCmdLine(valueStr,CCmdLineDlg::KEYWORD_AS_RETURN,"0|1"))
			{
				pCmdLine->CancelCmdLine();
				return NULL;
			}
			if(valueStr.GetLength()==0 ||valueStr[0]=='0'||valueStr[0]=='1')
			{
				if(valueStr.GetLength()>0)
					iEdgeType=atoi(valueStr);
				break;
			}
		}	
		if(iEdgeType==1)
		{
			if(cirPlateDlg.m_iPlateType==2)
			{
				cirPlateDlg.m_iPlateType=3;
				cirPlateDlg.para.nDHRelativeType=0;
			}
			else if(cirPlateDlg.m_iPlateType==4)
				cirPlateDlg.m_iPlateType=5;
		}
	}
	cirPlateDlg.InitCirPlatePara();
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(cirPlateDlg.m_pCirPlate->handle);
	//显示示意图
	CLDSView::PROMPT_LIFE lifeprompt;
	if(cirPlateDlg.m_iPlateType==2)	//3WL肋板
		lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_RIBPLATE1);
	else if(cirPlateDlg.m_iPlateType==3)	//3WA肋板
		lifeprompt.SetPromptPicture((cirPlateDlg.para.nDHRelativeType==0)?IDB_BMP_CIRCULAR_RIBPLATE2:IDB_BMP_CIRCULAR_RIBPLATE5);
	else if(cirPlateDlg.m_iPlateType==4)	//2WL肋板
		lifeprompt.SetPromptPicture((cirPlateDlg.para.D>0)?IDB_BMP_CIRCULAR_RIBPLATE3:IDB_BMP_CIRCULAR_RIBPLATE7);
	else if(cirPlateDlg.m_iPlateType==5)	//2WA肋板
		lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_RIBPLATE4);
	//
	if(cirPlateDlg.m_iPlateType==3&&cirPlateDlg.para.nDHRelativeType==0)
	{	//输入环向肋板的外环半径
		cirPlateDlg.para.R;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input circulaer plate's outer-ring radius<%.1f>:",cirPlateDlg.para.R);
#else
		cmdStr.Format("CircularPlate 请输入环向封板的外环半径<%.1f>:",cirPlateDlg.para.R);
#endif
		while(true)
		{
			pCmdLine->FillCmdLine(cmdStr,"");
			if(!pCmdLine->GetStrFromCmdLine(valueStr))
			{
				pCmdLine->CancelCmdLine();
				return NULL;
			}
			if(valueStr.GetLength()==0 ||(CmdStrToNumber(valueStr,&cirPlateDlg.para.R)&&cirPlateDlg.para.R>0))
				break;
		}
		cirPlateDlg.ModifyPlateParam('R',cirPlateDlg.para.R);
	}
	else
	{	//输入焊缝长度W和台口高度
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input relative plate's weld length<%.f>,front height(H) <%.0f,%.0f>:",cirPlateDlg.para.W,cirPlateDlg.para.H);
#else
		cmdStr.Format("CircularPlate 输入环形肋板与关联钢板的焊缝长度(W)，台口高度(H) <%.0f,%.0f>:",cirPlateDlg.para.W,cirPlateDlg.para.H);
#endif
		while(true)
		{
			pCmdLine->FillCmdLine(cmdStr,"");
			if(!pCmdLine->GetStrFromCmdLine(valueStr))
			{
				pCmdLine->CancelCmdLine();
				return NULL;
			}
			if(valueStr.GetLength()==0||(CmdStrToNumber(valueStr,&cirPlateDlg.para.W,&cirPlateDlg.para.H) && cirPlateDlg.para.W>0))
			{
				cirPlateDlg.para.N=cirPlateDlg.para.W;
				cirPlateDlg.para.h=cirPlateDlg.para.H;
				break;
			}
		}
		cirPlateDlg.ModifyPlateParam('W',cirPlateDlg.para.W);
		cirPlateDlg.ModifyPlateParam('H',cirPlateDlg.para.H);
		if(cirPlateDlg.m_iPlateType<4)
		{
			cirPlateDlg.ModifyPlateParam('N',cirPlateDlg.para.N);
			cirPlateDlg.ModifyPlateParam('h',cirPlateDlg.para.h);
		}	
		if(cirPlateDlg.m_iPlateType==3)
		{//输入环形带宽D
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("CircularPlate input circulaer plate's outer wall distance<%.1f>:",cirPlateDlg.para.D);
#else
			cmdStr.Format("CircularPlate 请输入环向封板的环形带宽D<%.1f>:",cirPlateDlg.para.D);
#endif
			while(true)
			{
				pCmdLine->FillCmdLine(cmdStr,"");
				if(!pCmdLine->GetStrFromCmdLine(valueStr))
				{
					pCmdLine->CancelCmdLine();
					return NULL;
				}
				if(valueStr.GetLength()==0 ||(CmdStrToNumber(valueStr,&cirPlateDlg.para.D)&&cirPlateDlg.para.D>0))
					break;
			}
			cirPlateDlg.ModifyPlateParam('D',cirPlateDlg.para.D);
		}
		if(cirPlateDlg.m_iPlateType==4||cirPlateDlg.m_iPlateType==5)
		{	//输入扇形角β
			double angle=cirPlateDlg.para.B;
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("CircularPlate input circulaer plate's angle<%.0f>:",angle);
#else
			cmdStr.Format("CircularPlate 请输入环向肋板的扇形夹角β<%.0f>:",angle);
#endif
			while(true)
			{
				pCmdLine->FillCmdLine(cmdStr,"");
				if(!pCmdLine->GetStrFromCmdLine(valueStr))
				{
					pCmdLine->CancelCmdLine();
					return NULL;
				}
				if(valueStr.GetLength()==0 ||(CmdStrToNumber(valueStr,&angle)&&angle>0))
				{
					cirPlateDlg.para.B=angle;
					break;
				}
			}
			cirPlateDlg.ModifyPlateParam('B',cirPlateDlg.para.B);
		}
		if(cirPlateDlg.m_iPlateType==5)
		{	//倒角角度γ
			double angle=cirPlateDlg.para.gama;
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("CircularPlate input circulaer plate's outer wall distance<%.0f>:",angle);
#else
			cmdStr.Format("CircularPlate 请输入环向肋板的倒角角度γ<%.0f>:",angle);
#endif
			while(true)
			{
				pCmdLine->FillCmdLine(cmdStr,"");
				if(!pCmdLine->GetStrFromCmdLine(valueStr))
				{
					pCmdLine->CancelCmdLine();
					return NULL;
				}
				if(valueStr.GetLength()==0 ||(CmdStrToNumber(valueStr,&angle)&&angle>0))
				{
					cirPlateDlg.para.gama=angle;
					break;
				}
			}
			cirPlateDlg.ModifyPlateParam('y',cirPlateDlg.para.gama);
		}
	}
	//输入沿轴向偏移量
	cirPlateDlg.WorkPlaneSketch(2);
	double S=cirPlateDlg.para.S;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's offset distance<%.0f>:",S);
#else
	cmdStr.Format("CircularPlate 请输入环向肋板的轴向偏移量<%.0f>:",S);
#endif
	while(true)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(!pCmdLine->GetStrFromCmdLine(valueStr))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		if(valueStr.GetLength()==0 ||CmdStrToNumber(valueStr,&S))
		{
			cirPlateDlg.para.S=S;
			break;
		}
	}
	cirPlateDlg.DelWorkPlaneSketch();
	cirPlateDlg.ModifyPlateParam('S',cirPlateDlg.para.S);
	//显示环向板对话框
	if(cirPlateDlg.DoModal()==IDOK)
		return cirPlateDlg.m_pCirPlate;
#endif
	return NULL;
}

BOOL CLDSView::FastCirPlate(int plate_type)
{
#ifdef __COMPLEX_PART_INC_
	CLDSPlate *pDatumPlate1=NULL,*pDatumPlate2=NULL;
	long *id_arr=NULL;
	int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n==1||n==2)
	{
		pDatumPlate1=(CLDSPlate*)console.FromPartHandle(id_arr[0],CLS_PLATE);
		if(n==2)
			pDatumPlate2=(CLDSPlate*)console.FromPartHandle(id_arr[1],CLS_PLATE);
	}
	if(pDatumPlate1==NULL||(n==2&&pDatumPlate2==NULL))
		return FALSE;
	CLDSNode *pBaseNode=console.FromNodeHandle(pDatumPlate1->designInfo.m_hBaseNode);
	CLDSLineTube* pBaseTube=(CLDSLineTube*)console.FromPartHandle(pDatumPlate1->designInfo.m_hBasePart,CLS_LINETUBE);
	if(pBaseNode==NULL||pBaseTube==NULL)
		return FALSE; //保证所选钢板存在基准节点与基准钢管
	if(n==2&&(pBaseNode->handle!=pDatumPlate2->designInfo.m_hBaseNode
		||pBaseTube->handle!=pDatumPlate2->designInfo.m_hBasePart))
		return FALSE;	//保证所选的两块节点板基准节点与基准杆件都相同 
	//
	CDesignCircularPlateDlg cirPlateDlg;
	cirPlateDlg.m_pDatumNode=pBaseNode;			//基准节点
	cirPlateDlg.m_pDatumTube=pBaseTube;			//基准钢管
	if(pDatumPlate1)
		cirPlateDlg.m_pRelaPart1=pDatumPlate1;	//关联钢板1
	if(pDatumPlate2)
		cirPlateDlg.m_pRelaPart2=pDatumPlate2;	//关联钢板2
	if(plate_type==0)
		cirPlateDlg.m_iPlateType=1;		//封板
	else if(plate_type==1)
	{
		if(pDatumPlate1&&pDatumPlate2)
		{
			cirPlateDlg.m_iPlateType=2;	//3WL肋板
			cirPlateDlg.para.nDHRelativeType=2; //启用参数H
		}
		else if(pDatumPlate1)
			cirPlateDlg.m_iPlateType=4;	//2WL肋板
	}
	else //if(plate_type==2)
	{
		if(pDatumPlate1&&pDatumPlate2)
		{
			cirPlateDlg.m_iPlateType=3;	//3WA肋板
			cirPlateDlg.para.nDHRelativeType=1;
		}
	}
	if(cirPlateDlg.DoModal()==IDOK)
		cirPlateDlg.CreateCircularPlate();
	/*
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr;
	//输入环向板加工间隙
	double manu_space=0;
	cmdStr.Format("请输入环向板加工间隙<0>:");
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
			return FALSE;
		double space=atof(cmdStr.GetBuffer(1));
		if(cmdStr.GetLength()==0)
			manu_space=0;
		else if(space>0||fabs(space)<EPS)
			manu_space=space;
		else if(cmdStr.GetLength()>0)
		{
			pCmdLine->FinishCmdLine();
			cmdStr.Format("输入错误，请输入环向板加工间隙<0>:");
			pCmdLine->FillCmdLine(cmdStr,"");
			continue;
		}
		break;
	}
	g_pSolidDraw->ReleaseSnapStatus();
	pCmdLine->FillCmdLine("命令:","");
	pCmdLine->FinishCmdLine();
	if(plate_type==0)		//设计生成环向封板
		return FinishDesignCirCoverPlate(pBaseNode,pBaseTube,pDatumPlate1,pDatumPlate2,manu_space);
	else if(plate_type==1)	//设计生成环向肋板
		return FinishDesignCirRibPlate(pBaseTube,pBaseNode,pDatumPlate1,pDatumPlate2,manu_space);
	else if(plate_type==2)	//设计生成钢管肋板
	{
		CLDSLinePart *pAxisXRefLinePart=NULL;
		pCmdLine->FillCmdLine("请选择确认环向板X轴方向的参考杆件,若没有环向板的X轴方向将与基准钢管的轴向垂直:","");
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_LINETUBE));
		while(1)
		{
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				return FALSE;
			long *id_arr;
			long id_num=g_pSolidSnap->GetLastSelectEnts(id_arr);
			if(id_num==0)
				break;
			else if(id_num==1)
			{
				CLDSPart *pPart=console.FromPartHandle(id_arr[0]);
				if(pPart&&pPart->IsLinePart())
				{
					pAxisXRefLinePart=(CLDSLinePart*)pPart;
					break;
				}
			}
			if(pAxisXRefLinePart==NULL)
			{
				pCmdLine->FillCmdLine("没有选中合适的构件，请选择环向板X轴方向的参考杆件:","");
				g_pSolidDraw->ReleaseSnapStatus();
				continue;
			}
		}
		g_pSolidDraw->ReleaseSnapStatus();
		pCmdLine->FillCmdLine("命令:","");
		pCmdLine->FinishCmdLine();
		return FinishDesignTubeRibPlate(pBaseNode,pBaseTube,pDatumPlate1,pDatumPlate2,pAxisXRefLinePart,manu_space);
	}*/
#endif
	return FALSE;
}

//快速生成环向封板
void CLDSView::OnFastCirCoverPlate()
{
	FastCirPlate(0);	//设计生成环向封板
}

//快速生成环向肋板
void CLDSView::OnFastCirRibPlate()
{
	FastCirPlate(1);	//设计生成环向肋板
}

//快速生成钢管肋板
void CLDSView::OnFastTubeRibPlate()
{
	FastCirPlate(2);	//设计生成钢管肋板
}

static CDesignGyHoleDlg gyHoleDlg;
struct NEAR_VERTEX{
	PROFILE_VERTEX* pVertex;
	double dist;
	NEAR_VERTEX(PROFILE_VERTEX* _pVertex=NULL,double _dist=100000){pVertex=_pVertex;dist=_dist;}
};
BOOL CreateProcessHole(CSuperSmartPtr<CLDSPart> pHoriPlate,CSuperSmartPtr<CLDSPart>pVertPlate,
					   BOOL bDispDlg,BOOL bIncUpHole=TRUE,BOOL bIncDownHole=TRUE)
{
	if(pHoriPlate.IsNULL()||pVertPlate.IsNULL())
		return FALSE;
	CLDSLineTube *pDatumTube=NULL;
	if(pHoriPlate->m_hPartWeldParent)
	{
		CLDSPart *pPart = console.FromPartHandle(pHoriPlate->m_hPartWeldParent);
		if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
			pDatumTube = (CLDSLineTube*)pPart;
	}
	if(pDatumTube==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("welding parent rod of the selected plate is not tube, error in data！");
#else
		AfxMessageBox("所选中钢板的焊接父构件不是钢管，数据有误！");
#endif
		return FALSE;
	}
	//显示竖直板的坐标系用来区分内侧孔/外侧孔
	//GEPOINT base_pos=pVertPlate->ucs.origin;
	f3dPoint base_pos,edge_vec;
	Int3d2f(&base_pos,&edge_vec,pHoriPlate->ucs.origin,pHoriPlate->ucs.axis_z,pVertPlate->ucs.origin,pVertPlate->ucs.axis_z);
	GEPOINT vxEdgePick=base_pos,vEdgeVec=edge_vec;
	coord_trans(base_pos,pVertPlate->ucs,FALSE,TRUE);
	vector_trans(edge_vec,pVertPlate->ucs,FALSE,TRUE);
	edge_vec.z=0;
	normalize(edge_vec);
	BYTE ciMarkState=0;
	NEAR_VERTEX xNearest,xNear;
	for(PROFILE_VERTEX* pVertex=pVertPlate.pBasicPlate->vertex_list.GetFirst();pVertex;pVertex=pVertPlate.pBasicPlate->vertex_list.GetNext())
	{
		double dist=DistOf2dPtLine(pVertex->vertex,base_pos,base_pos+edge_vec);
		if(ciMarkState==0)
		{
			xNearest=xNear=NEAR_VERTEX(pVertex,dist);
			ciMarkState=1;
		}
		else //if(ciMarkState>=1)
		{
			if(xNearest.dist>dist)
			{
				xNear=xNearest;
				xNearest=NEAR_VERTEX(pVertex,dist);
			}
			else if(ciMarkState==1)
				xNear=NEAR_VERTEX(pVertex,dist);
			else if(ciMarkState==2&&xNear.dist>dist)
				xNear=NEAR_VERTEX(pVertex,dist);
			ciMarkState=2;
		}
	}
	base_pos=(xNear.pVertex->vertex+xNearest.pVertex->vertex)*0.5;
	coord_trans(base_pos,pVertPlate->ucs,TRUE,TRUE);
	SnapPerp(&base_pos,vxEdgePick,vxEdgePick+vEdgeVec,base_pos);
	btc::SKETCH_PLANE sketch,sketchAnti;//(base_pos,pVertPlate->ucs.axis_z,ucs.axis_x,100);
	sketch.CreateStdPlane(base_pos,pVertPlate->ucs.axis_z,pVertPlate->ucs.axis_x,300);
	sketchAnti.CreateStdPlane(base_pos,-pVertPlate->ucs.axis_z,pVertPlate->ucs.axis_x,300);
	CWorkPlaneSketchLifeObject workPlaneSketchLife;
	workPlaneSketchLife.WorkPlaneSketch(sketch,sketch.normal,"外侧",100);
	workPlaneSketchLife.WorkPlaneSketch(sketchAnti,sketchAnti.normal,"内侧",100);
	//如果布孔钢板是双面板，需判断流水孔位于哪个面(基面|火曲面)
	f3dPoint hori_face_pick=pHoriPlate->ucs.origin;
	f3dPoint hori_face_norm=pHoriPlate->ucs.axis_z;
	f3dPoint huoqu_norm=pHoriPlate.GeneralPlatePointer()->GetHuoquFaceNorm(0);
	int iPlane=0;
	if(!huoqu_norm.IsZero())
	{	//根据焊接边与焊接钢板的关系，确定位于哪个面
		PROFILE_VERTEX* pVertex=NULL,*pPrevVertex=pHoriPlate.GeneralPlatePointer()->vertex_list.GetTail();
		for(pVertex=pHoriPlate.GeneralPlatePointer()->vertex_list.GetFirst();pVertex;
			pPrevVertex=pVertex,pVertex=pHoriPlate.GeneralPlatePointer()->vertex_list.GetNext())
		{
			if(!pPrevVertex->m_bWeldEdge)
				continue;
			BOOL bWeldToVertPlate=FALSE;
			f3dPoint startPt,endPt;
			if(pHoriPlate->GetClassTypeId()==CLS_PLATE)
			{
				startPt=pHoriPlate.PlatePointer()->GetRealVertex(pPrevVertex->vertex);
				endPt=pHoriPlate.PlatePointer()->GetRealVertex(pVertex->vertex);
			}
			else
			{
				startPt=pHoriPlate.ParamPlatePointer()->SpreadTransV(pPrevVertex->vertex);
				endPt=pHoriPlate.ParamPlatePointer()->SpreadTransV(pVertex->vertex);
			}
			coord_trans(startPt,pHoriPlate->ucs,TRUE,TRUE);
			coord_trans(endPt,pHoriPlate->ucs,TRUE,TRUE);
			f3dPoint lineStdVec=endPt-startPt;
			normalize(lineStdVec);
			if(fabs(lineStdVec*pVertPlate->ucs.axis_z)<EPS2)
			{	//该边线与当前板基准面法线垂直
				coord_trans(startPt,pVertPlate->ucs,FALSE,TRUE);
				coord_trans(endPt,pVertPlate->ucs,FALSE,TRUE);
				f3dPoint middlePick=0.5*(startPt+endPt);
				if(pVertPlate.GeneralPlatePointer()->IsPtInPlate(middlePick))
					bWeldToVertPlate=TRUE;
			}
			if(bWeldToVertPlate&&pPrevVertex->vertex.feature%10==2&&pVertex->vertex.feature%10==2)
			{
				hori_face_norm=huoqu_norm;
				iPlane=1;
				break;
			}
		}
	}
	//
	long d=12;
	double s=20,h=30,D=13.5;
	BOOL bUp=TRUE,bDown=TRUE;
	int nVertPlateThick=(int)pVertPlate->GetThick();
	f3dPoint vert_face_pick;
	if(bDispDlg)
	{
		gyHoleDlg.m_bIncDownHole=bIncDownHole;
		gyHoleDlg.m_bIncUpHole=bIncUpHole;
		if(gyHoleDlg.DoModal()==IDOK)
		{
			s=gyHoleDlg.m_fParaS;
			h=gyHoleDlg.m_fParaH;
			bUp=gyHoleDlg.m_bIncUpHole;
			bDown=gyHoleDlg.m_bIncDownHole;
			d=atoi(gyHoleDlg.m_sBoltSize);
			D=gyHoleDlg.m_fHoleD;
		}
		else
			d=0;
	}
	else
	{
		s=gyHoleDlg.m_fParaS;
		h=gyHoleDlg.m_fParaH;
		bUp=gyHoleDlg.m_bIncUpHole;
		bDown=gyHoleDlg.m_bIncDownHole;
		d=atoi(gyHoleDlg.m_sBoltSize);
		D=gyHoleDlg.m_fHoleD;
	}
	if(D<=0)	//最关键的是孔径而不是虚拟螺栓直径　wjh-2018.1.29
		return FALSE;
	if(pHoriPlate->GetClassTypeId()==CLS_PARAMPLATE&&pHoriPlate.pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
	{
		long hPart=0;
		CLDSParamPlate::DESIGN_PARAM param;
		param.cValueType=3;
		param.value.hPart=pVertPlate->handle;
		WORD* pw_BGorBS=(WORD*)&param.value.uiAttribute;
		*pw_BGorBS	  =(WORD)h;
		*(pw_BGorBS+1)=(WORD)s;
		//	
		pHoriPlate.pParamPlate->SetDesignItemValue(KEY2C("BH"),D);	//流水孔直径,BH=0表示不生成流水孔
		if(pHoriPlate.pParamPlate->GetDesignItemValue('F',&hPart) && hPart==pVertPlate->handle)
			pHoriPlate.pParamPlate->SetDesignItem('F',param,"",PARAM_POS);
		else if(pHoriPlate.pParamPlate->GetDesignItemValue('G',&hPart) && hPart==pVertPlate->handle)
			pHoriPlate.pParamPlate->SetDesignItem('G',param,"",PARAM_POS);
		else
		{	//处理覆盖板
			char szKeyStr[3]="P0";
			CXhString keystr(szKeyStr,3);
			CLDSParamPlate::DESIGN_PARAM itemparam;
			int idle=-1;
			for(int i=0;i<4;i++)
			{
				keystr[1]='0'+i;
				BOOL bExist=pHoriPlate.pParamPlate->GetDesignItemValue(KEY2C(keystr),&itemparam);
				if(bExist && itemparam.hValue<0X20)
					bExist=FALSE;
				if(!bExist&&idle<0)
					idle=i;
				else if(bExist&&itemparam.hValue==pVertPlate->handle)
				{	//已存在，直接更新设计值
					idle=-1;
					pHoriPlate.pParamPlate->SetDesignItem(KEY2C(keystr),param,"",PARAM_POS);
					break;
				}
			}
			if(idle>=0&&idle<=3)
			{	//添加覆盖板设计信息
				keystr[1]='0'+idle;
				pHoriPlate.pParamPlate->SetDesignItem(KEY2C(keystr),param,"",PARAM_POS);
			}
		}
		//
		pHoriPlate->m_bDisplayHole=TRUE;
		pHoriPlate.pParamPlate->DesignPlate();
		pHoriPlate.pParamPlate->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pHoriPlate.pParamPlate->GetSolidPartObject());
		g_pSolidDraw->Draw();
		return TRUE;	//环向封板上的流水孔已转为虚拟孔CLDSPart::HOLE类型在CLDSParamPlate::DesignPlate时设计　wjh-2018.1.28
	}
	else if(pHoriPlate->GetClassTypeId()==CLS_PARAMPLATE&&
		(pHoriPlate.pParamPlate->m_iParamType==TYPE_CIRRIBPLATE||pHoriPlate.pParamPlate->m_iParamType==TYPE_TUBERIBPLATE))
	{
		long hPart=0;
		CLDSParamPlate::DESIGN_PARAM param;
		param.cValueType=3;
		param.value.hPart=pVertPlate->handle;
		WORD* pw_BGorBS=(WORD*)&param.value.uiAttribute;
		*pw_BGorBS	  =(WORD)h;
		*(pw_BGorBS+1)=(WORD)s;
		//	
		pHoriPlate.pParamPlate->SetDesignItemValue(KEY2C("BH"),D);	//流水孔直径,BH=0表示不生成流水孔
		if(pHoriPlate.pParamPlate->GetDesignItemValue('E',&hPart) && hPart==pVertPlate->handle)
			pHoriPlate.pParamPlate->SetDesignItem('E',param,"",PARAM_POS);
		else if(pHoriPlate.pParamPlate->GetDesignItemValue('F',&hPart) && hPart==pVertPlate->handle)
			pHoriPlate.pParamPlate->SetDesignItem('F',param,"",PARAM_POS);
		//
		pHoriPlate->m_bDisplayHole=TRUE;
		pHoriPlate.pParamPlate->DesignPlate();
		pHoriPlate.pParamPlate->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pHoriPlate.pParamPlate->GetSolidPartObject());
		g_pSolidDraw->Draw();
		return TRUE;
	}
	if(pVertPlate->GetClassTypeId()==CLS_PLATE)
		vert_face_pick=pVertPlate->ucs.origin+(pVertPlate.PlatePointer()->m_fNormOffset+nVertPlateThick*0.5)*pVertPlate->ucs.axis_z;
	else if(pVertPlate->GetClassTypeId()==CLS_PARAMPLATE)
	{
		if(pVertPlate.ParamPlatePointer()->m_iParamType==TYPE_FL||
			pVertPlate.ParamPlatePointer()->m_iParamType==TYPE_FLG||
			pVertPlate.ParamPlatePointer()->m_iParamType==TYPE_FLR)
			vert_face_pick=pVertPlate->ucs.origin+pVertPlate.ParamPlatePointer()->thick*pVertPlate->ucs.axis_z;
		else
			vert_face_pick=pVertPlate->ucs.origin;
	}
	f3dPoint inters,inters_pick1,inters_pick2,inters_line_vec;
	Int3d2f(&inters_pick1,&inters_line_vec,hori_face_pick,hori_face_norm,vert_face_pick,pVertPlate->ucs.axis_z);
	Int3dlc(&inters_pick1,&inters_pick2,inters_pick1,inters_line_vec,pDatumTube->Start(),pDatumTube->End(),pDatumTube->GetDiameter()*0.5,pDatumTube->GetDiameter()*0.5);
	f3dPoint test_vec1=inters_pick1-hori_face_pick;
	f3dPoint test_vec2=inters_pick2-hori_face_pick;
	f3dPoint verify_vec=pDatumTube->GetPlateVertSpreadVec(pVertPlate.GeneralPlatePointer());
	if(verify_vec.IsZero())
	{	//计算工艺孔大致位置(交点向外延伸h),判断是否在水平钢板内 wht 15-12-18
		f3dPoint middle_pt=(inters_pick1+inters_pick2)*0.5;
		f3dPoint extend_vec=inters_pick1-middle_pt;
		normalize(extend_vec);
		f3dPoint extend_inters=inters_pick1+extend_vec*h;
		coord_trans(extend_inters,pHoriPlate->ucs,FALSE);
		extend_inters.z=0;
		if(pHoriPlate.GeneralPlatePointer()->IsPtInPlate(extend_inters))
			verify_vec=inters_pick1-inters_pick2;
		else 
			verify_vec=inters_pick2-inters_pick1;
		normalize(verify_vec);
	}
	if(inters_line_vec*verify_vec<0)
		inters_line_vec*=-1.0;	
	normalize(inters_line_vec);
	CLDSBolt *pBolt=NULL;
	if(test_vec1*verify_vec>test_vec2*verify_vec)
		inters=inters_pick1;
	else
		inters=inters_pick2;
	
	//根据数据的S、H参数求得在交线方向的移动量 wht 10-08-25
	double tube_r=pDatumTube->GetDiameter()*0.5;
	double xie_bian=tube_r+D*0.5+h;
	double zhijiao_bian=D*0.5+s+nVertPlateThick*0.5;
	if(gyHoleDlg.m_bIncludeHoleR)
	{	//参数S、H包括孔半径 wht 10-08-25
		xie_bian=tube_r+h;
		zhijiao_bian=s+nVertPlateThick*0.5;
	}
	double offset_dist=sqrt(xie_bian*xie_bian-zhijiao_bian*zhijiao_bian);
	if(bUp)
	{
		pBolt = (CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->m_bVirtualPart=TRUE;
		pBolt->set_d(d);
		pBolt->CopyModuleInstanceInfo(pHoriPlate);	//调整螺栓配材号与基准构件配材号一致
		pBolt->ucs.axis_z=hori_face_norm;
		pBolt->ucs.origin=inters+inters_line_vec*(offset_dist-tube_r)+pVertPlate->ucs.axis_z*zhijiao_bian;
		pBolt->des_base_pos.datumPoint.datum_pos=pBolt->ucs.origin;
		pBolt->hole_d_increment=D-d;
		pBolt->des_base_pos.hPart=pHoriPlate->handle;
		pBolt->des_base_pos.datumPoint.datum_pos_style=9;
		pBolt->des_base_pos.datumPoint.des_para.hPart=pHoriPlate->handle;
		coord_trans(pBolt->des_base_pos.datumPoint.datum_pos,pHoriPlate->ucs,FALSE);
		pBolt->Create3dSolidModel();
		if(pHoriPlate->GetClassTypeId()==CLS_PLATE)
			pHoriPlate.PlatePointer()->AppendLsRef(pBolt->GetLsRef());
		else
			pHoriPlate.ParamPlatePointer()->AppendLsRef(pBolt->GetLsRef());
		if(g_sysPara.display.bDispVirtualPart)
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	if(bDown)
	{
		pBolt = (CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->m_bVirtualPart=TRUE;
		pBolt->set_d(d);
		pBolt->CopyModuleInstanceInfo(pHoriPlate);	//调整螺栓配材号与基准构件配材号一致
		pBolt->ucs.axis_z=hori_face_norm;
		pBolt->ucs.origin=inters+inters_line_vec*(offset_dist-tube_r)-pVertPlate->ucs.axis_z*zhijiao_bian;
		pBolt->des_base_pos.datumPoint.datum_pos=pBolt->ucs.origin;
		pBolt->hole_d_increment=D-d;
		pBolt->des_base_pos.hPart=pHoriPlate->handle;
		pBolt->des_base_pos.datumPoint.datum_pos_style=9;
		pBolt->des_base_pos.datumPoint.des_para.hPart=pHoriPlate->handle;
		coord_trans(pBolt->des_base_pos.datumPoint.datum_pos,pHoriPlate->ucs,FALSE);
		pBolt->Create3dSolidModel();
		if(pHoriPlate->GetClassTypeId()==CLS_PLATE)
			pHoriPlate.PlatePointer()->AppendLsRef(pBolt->GetLsRef());
		else
			pHoriPlate.ParamPlatePointer()->AppendLsRef(pBolt->GetLsRef());
		if(g_sysPara.display.bDispVirtualPart)
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	pHoriPlate->m_bDisplayHole=TRUE;
	pHoriPlate->SetModified();
	pHoriPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pHoriPlate->GetSolidPartObject());
	return TRUE;
}

BOOL CLDSView::FinishDesignCirCoverPlate(CLDSNode *pBaseNode,CLDSLineTube *pDatumTube,CLDSPlate *pPlate1,CLDSPlate *pPlate2,double manu_space/*=0*/)
{	//环向封板
#ifdef __COMPLEX_PART_INC_
	f3dPoint plate_origin;
	UCS_STRU object_ucs,ucs;
	CLDSPart *pPart=NULL;
	CSuperSmartPtr<CLDSPart> pFirstDatumPlate=pPlate1,pSecondDatumPlate=pPlate2;
	CLDSParamPlate *pCoverPlate=NULL, *pCoverPlate1=NULL;
	if(pDatumTube)
	{
		Ta.BeginUndoListen();
		pDatumTube->BuildUCS();
		//static CAddCircularCoverPlateDlg coverPlateDlg;
		//coverPlateDlg.pDatumPlate1=pPlate1;
		//coverPlateDlg.pDatumPlate2=pPlate2;
		CDesignCircularPlateDlg coverPlateDlg;
		coverPlateDlg.m_iPlateType=1;	//环向封板
		coverPlateDlg.m_pDatumNode=pBaseNode;
		coverPlateDlg.m_pDatumTube=pDatumTube;
		coverPlateDlg.m_pRelaPart1=pPlate1;
		coverPlateDlg.m_pRelaPart2=pPlate2;
		if(coverPlateDlg.DoModal()==IDOK)
		//if(coverPlateDlg.DoModal()==IDOK)
		{
			if(coverPlateDlg.para.S==0	//两个都设计时,在此处设计上封板
				||coverPlateDlg.para.S>0||coverPlateDlg.para.S<0)	//上下封板只设计一个时,在此处设计;
			{	
				pCoverPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
				pCoverPlate->m_iParamType = TYPE_CIRCOVERPLATE;
				pCoverPlate->SetPartNo(coverPlateDlg.m_sPartNo);	//编号
				pCoverPlate->iSeg = SEGI(coverPlateDlg.m_sSeg.GetBuffer());	//段号
				pCoverPlate->CopyModuleInstanceInfo(pDatumTube);	//调整钢板配材号与基准构件或基准节点配材号一致
				pCoverPlate->cMaterial = coverPlateDlg.m_cMaterial;//steelmat_tbl[coverPlateDlg.m_iMaterial].cBriefMark;	//材质
				pCoverPlate->thick = coverPlateDlg.thick;
				pCoverPlate->m_hPartWeldParent = pDatumTube->handle;
#ifdef AFX_TARG_ENU_ENGLISH
				pCoverPlate->SetDesignItemValue('A',coverPlateDlg.para.A,"Sealing plate rotation Angle");
				pCoverPlate->SetDesignItemValue('B',coverPlateDlg.para.B,"fan angle",PARAM_SHAPE);
				pCoverPlate->SetDesignItemValue('C',pDatumTube->handle,"location datum tube",PARAM_POS,1);
				pCoverPlate->SetDesignItemValue('E',pBaseNode->handle,"location datum node",PARAM_POS,1);
				pCoverPlate->SetDesignItemValue('F',pFirstDatumPlate->handle,"the first datum plate",PARAM_POS,1);
				if(pSecondDatumPlate.IsNULL())
					pCoverPlate->SetDesignItemValue('G',0,"the second datum plate",PARAM_POS,1);
				else
					pCoverPlate->SetDesignItemValue('G',pSecondDatumPlate->handle,"the second datum plate",PARAM_POS,1);
				pCoverPlate->SetDesignItemValue('D',coverPlateDlg.para.H,"ring sealing plate parameter H",PARAM_SHAPE);
				pCoverPlate->SetDesignItemValue('W',coverPlateDlg.para.W,"ring sealing plate parameter W",PARAM_SHAPE);
				if(coverPlateDlg.para.S>=0)
					pCoverPlate->SetDesignItemValue('S',-1,"design up sealing plate");
				else 
					pCoverPlate->SetDesignItemValue('S',1,"design down sealing plate");
				pCoverPlate->SetDesignItemValue('R',pDatumTube->GetDiameter()*0.5+manu_space,"circular plate inside arc radius (= + tube's radius + processing clearance)",PARAM_SHAPE);
#else
				pCoverPlate->SetDesignItemValue('A',coverPlateDlg.para.A,"封板旋转角度");
				pCoverPlate->SetDesignItemValue('B',coverPlateDlg.para.B,"扇形夹角",PARAM_SHAPE);
				pCoverPlate->SetDesignItemValue('C',pDatumTube->handle,"定位基准基准钢管",PARAM_POS,1);
				pCoverPlate->SetDesignItemValue('E',pBaseNode->handle,"定位基准节点",PARAM_POS,1);
				pCoverPlate->SetDesignItemValue('F',pFirstDatumPlate->handle,"第一块基准钢板",PARAM_POS,1);
				if(pSecondDatumPlate.IsNULL())
					pCoverPlate->SetDesignItemValue('G',0,"第二块基准钢板",PARAM_POS,1);
				else
					pCoverPlate->SetDesignItemValue('G',pSecondDatumPlate->handle,"第二块基准钢板",PARAM_POS,1);
				pCoverPlate->SetDesignItemValue('D',coverPlateDlg.para.H,"环向封板参数H",PARAM_SHAPE);
				pCoverPlate->SetDesignItemValue('W',coverPlateDlg.para.W,"环向封板参数W",PARAM_SHAPE);
				if(coverPlateDlg.para.S>=0)
					pCoverPlate->SetDesignItemValue('S',-1,"设计上封板");
				else 
					pCoverPlate->SetDesignItemValue('S',1,"设计下封板");
				pCoverPlate->SetDesignItemValue('R',pDatumTube->GetDiameter()*0.5+manu_space,"环向板内圆弧半径(=钢管半径+加工间隙)",PARAM_SHAPE);
#endif
				//if(coverPlateDlg.m_bMirSide)
				//	pCoverPlate->SetDesignItemValue('M',(double)-1,"M为-1时在贯通连板另外一侧加封板");
				//else
#ifdef AFX_TARG_ENU_ENGLISH
					pCoverPlate->SetDesignItemValue('M',(double)1,"when M is -1, add sealing plate on the other side of  through board");
#else
					pCoverPlate->SetDesignItemValue('M',(double)1,"M为-1时在贯通连板另外一侧加封板");
#endif
				pCoverPlate->DesignPlate();
				pCoverPlate->SetModified();
				pCoverPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pCoverPlate->GetSolidPartObject());
				g_pSolidDraw->Draw();
			}
			if(coverPlateDlg.para.S==0)
			{	//上下封一起设计时,在此处设计下封板
				pCoverPlate1=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
				pCoverPlate1->CopyProperty(pCoverPlate);
				pCoverPlate1->SetLayer(pDatumTube->layer()); //调整图层名
				pCoverPlate1->cfgword=pDatumTube->cfgword;
#ifdef AFX_TARG_ENU_ENGLISH
				pCoverPlate1->SetDesignItemValue('S',(double)1,"design down sealing plate");
#else
				pCoverPlate1->SetDesignItemValue('S',(double)1,"设计下封板");
#endif
				pCoverPlate1->m_hPartWeldParent = pDatumTube->handle;
				pCoverPlate1->DesignPlate();
				pCoverPlate1->SetModified();
				pCoverPlate1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pCoverPlate1->GetSolidPartObject());
				g_pSolidDraw->Draw();
			}
			g_pSolidSet->SetDisplayType(DISP_SOLID);
			Invalidate(FALSE);
			g_pSolidDraw->SetEntSnapStatus(pFirstDatumPlate->handle);
			BOOL bCreatHole=CreateProcessHole(pCoverPlate,pFirstDatumPlate,TRUE);
			if(bCreatHole)	//创建工艺孔
				CreateProcessHole(pCoverPlate1,pFirstDatumPlate,FALSE);
			if(pSecondDatumPlate.IsHasPtr()&&!pFirstDatumPlate.IsEqualPtr(pSecondDatumPlate))
			{
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(pSecondDatumPlate->handle);
				if(bCreatHole)
				{	//创建工艺孔
					CreateProcessHole(pCoverPlate,pSecondDatumPlate,FALSE);
					CreateProcessHole(pCoverPlate1,pSecondDatumPlate,FALSE);
				}
			}
			//g_bRedrawSolid=TRUE;
			g_pSolidDraw->Draw();
			g_pSolidDraw->ReleaseSnapStatus();
			m_pDoc->SetModifiedFlag();
			Ta.EndUndoListen();
			return TRUE;
		}
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("plate selected has no welding parent rod, can not meet the need to design ring sealing plate, desig interrupts!");
#else
		MessageBox("所选钢板无焊接父构件,不满足设计环向封板的条件,设计中断!");
#endif
#endif
	return FALSE;
}

BOOL CLDSView::FinishDesignCirRibPlate(CLDSPart *pDatumPart,CLDSNode *pBaseNode,CLDSPlate *pFirstDatumPlate,
									   CLDSPlate *pSecondDatumPlate,double manu_space/*=0*/)
{	//1.球+钢管夹板		
	//2.钢管+节点+一块基准钢板
	//3.钢管+节点+两块基准钢板
#ifdef __COMPLEX_PART_INC_
	CLDSLineTube *pDatumTube=NULL;
	CLDSSphere *pDatumSphere=NULL;
	if(pDatumPart==NULL||pFirstDatumPlate==NULL)
		return FALSE;
	if(pDatumPart->GetClassTypeId()==CLS_LINETUBE)
		pDatumTube=(CLDSLineTube*)pDatumPart;
	else if(pDatumPart->GetClassTypeId()==CLS_SPHERE)
		pDatumSphere=(CLDSSphere*)pDatumPart;
	if(pDatumTube==NULL&&pDatumSphere==NULL)
		return FALSE;
	CDesignCircularPlateDlg rib_dlg;
	rib_dlg.m_pDatumTube=pDatumTube;
	rib_dlg.m_pDatumSphere=pDatumSphere;
	rib_dlg.m_pDatumNode=pBaseNode;
	rib_dlg.m_pRelaPart1=pFirstDatumPlate;
	rib_dlg.m_pRelaPart2=pSecondDatumPlate;
	rib_dlg.m_iPlateType=2;
	rib_dlg.DoModal();
	/*static */CAddCircularPlateDlg cirRibDlg;
	if(pDatumTube&&pBaseNode)
	{	//钢管+节点+一块基准钢板或两块基准钢板
		pDatumTube->BuildUCS();
		cirRibDlg.m_pDatumNode=pBaseNode;
		cirRibDlg.m_pDatumTube=pDatumTube;	
		cirRibDlg.m_pDatumPlate1=pFirstDatumPlate;
		cirRibDlg.m_pDatumPlate2=pSecondDatumPlate;
		if(cirRibDlg.DoModal()==IDOK)
		{
			Ta.BeginUndoListen();
			CLDSParamPlate *pCirRibPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pCirRibPlate->m_iParamType = TYPE_CIRRIBPLATE;
			pCirRibPlate->SetPartNo(cirRibDlg.m_sPartNo.GetBuffer(0));	//编号
			pCirRibPlate->iSeg = SEGI(cirRibDlg.m_sSegI.GetBuffer());	//段号
			pCirRibPlate->SetLayer(pDatumTube->layer());	//调整环向板的图层名
			pCirRibPlate->cfgword=pDatumTube->cfgword;
			pCirRibPlate->cMaterial = CSteelMatLibrary::RecordAt(cirRibDlg.m_iMaterial).cBriefMark;	//材质
			pCirRibPlate->thick = (long)cirRibDlg.m_fPlateT;
			pCirRibPlate->m_hPartWeldParent = pDatumTube->handle;
			pCirRibPlate->cfgword=pDatumTube->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
#ifdef AFX_TARG_ENU_ENGLISH
			pCirRibPlate->SetDesignItemValue('A',pBaseNode->handle,"location datum node",PARAM_POS,1);
			pCirRibPlate->SetDesignItemValue('B',pFirstDatumPlate->handle,"the first datum plate",PARAM_POS,1);
#else
			pCirRibPlate->SetDesignItemValue('A',pBaseNode->handle,"定位基准节点",PARAM_POS,1);
			pCirRibPlate->SetDesignItemValue('B',pFirstDatumPlate->handle,"第一块基准钢板",PARAM_POS,1);
#endif
			double Y=-1;
			if(pSecondDatumPlate)	
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCirRibPlate->SetDesignItemValue('C',pSecondDatumPlate->handle,"the second datum plate",PARAM_POS,1);
				pCirRibPlate->SetDesignItemValue('W',cirRibDlg.m_fParaW,"circular plate parameter W",PARAM_SHAPE);
				pCirRibPlate->SetDesignItemValue('N',cirRibDlg.m_fParaN,"circular plate parameter N",PARAM_SHAPE);
				pCirRibPlate->SetDesignItemValue('D',cirRibDlg.m_fParaD,"circular plate parameter D, just enabling one of H and D is ok,D<0 indicates enabling H",PARAM_SHAPE);
				pCirRibPlate->SetDesignItemValue('H',cirRibDlg.m_fParaH,"circular plate parameter H, just enabling one of H and D is ok,H<0 indicates enabling D",PARAM_SHAPE);
#else
				pCirRibPlate->SetDesignItemValue('C',pSecondDatumPlate->handle,"第二块基准钢板",PARAM_POS,1);
				pCirRibPlate->SetDesignItemValue('W',cirRibDlg.m_fParaW,"环向肋板参数W",PARAM_SHAPE);
				pCirRibPlate->SetDesignItemValue('N',cirRibDlg.m_fParaN,"环向肋板参数N",PARAM_SHAPE);
				pCirRibPlate->SetDesignItemValue('D',cirRibDlg.m_fParaD,"环向肋板参数D,H与D启用一个即可 D<0表示启用H",PARAM_SHAPE);
				pCirRibPlate->SetDesignItemValue('H',cirRibDlg.m_fParaH,"环向肋板参数H,H与D启用一个即可 H<0表示启用D",PARAM_SHAPE);
#endif
				long O=-1;
				if(cirRibDlg.m_bCir)
					O=1;	//顶部圆弧连接
#ifdef AFX_TARG_ENU_ENGLISH
				pCirRibPlate->SetDesignItemValue('O',O,"positive value stands for arc connection on rib plate vertex; negative value stands for straight line connection");
#else
				pCirRibPlate->SetDesignItemValue('O',O,"正值表示肋板顶部为圆弧连接，负值表示肋板顶部为直线连接");
#endif
			}
			else
			{	
#ifdef AFX_TARG_ENU_ENGLISH
				pCirRibPlate->SetDesignItemValue('C',0,"the second datum plate",PARAM_POS,1);
				if(cirRibDlg.m_bDesignOuterPlate)
					Y=1;
				pCirRibPlate->SetDesignItemValue('Y',Y,"positive value or zero shows rib plate is set outside of datum plate, otherwise, inside");
				pCirRibPlate->SetDesignItemValue('W',cirRibDlg.m_fParaW,"circular rib plate parameter W",PARAM_SHAPE);
#else
				pCirRibPlate->SetDesignItemValue('C',0,"第二块基准钢板",PARAM_POS,1);
				if(cirRibDlg.m_bDesignOuterPlate)
					Y=1;
				pCirRibPlate->SetDesignItemValue('Y',Y,"正值或零表示肋板在基准钢板的外侧，否则位于内侧");
				pCirRibPlate->SetDesignItemValue('W',cirRibDlg.m_fParaW,"环向肋板参数W",PARAM_SHAPE);
#endif
				long O=-1;
				if(cirRibDlg.m_bCir)
				{	//顶部圆弧连接
					O=1;	//顶部圆弧连接
#ifdef AFX_TARG_ENU_ENGLISH
					pCirRibPlate->SetDesignItemValue('D',cirRibDlg.m_fParaD,"circular rib plate arc degree corresponding to βin map",PARAM_SHAPE);
					pCirRibPlate->SetDesignItemValue('N',cirRibDlg.m_fParaN,"circular rib plate parameter N",PARAM_SHAPE);
#else
					pCirRibPlate->SetDesignItemValue('D',cirRibDlg.m_fParaD,"环向肋板圆弧度数对应示意图中的β",PARAM_SHAPE);
					pCirRibPlate->SetDesignItemValue('N',cirRibDlg.m_fParaN,"环向肋板参数N",PARAM_SHAPE);
#endif
				}
				else
				{	//顶部非圆弧连接
#ifdef AFX_TARG_ENU_ENGLISH
					pCirRibPlate->SetDesignItemValue('D',cirRibDlg.m_fParaD,"circular rib plate parameter D",PARAM_SHAPE);
					pCirRibPlate->SetDesignItemValue('H',cirRibDlg.m_fParaH,"circular rib plate parameter H",PARAM_SHAPE);
				}
				pCirRibPlate->SetDesignItemValue('O',O,"positive value stands for arc connection on rib plate vertex; negative value stands for straight line connection");
			}
			
			pCirRibPlate->SetDesignItemValue('S',cirRibDlg.m_fParaS,"offset distance from circular plate to location node");
			pCirRibPlate->SetDesignItemValue('R',pDatumTube->GetDiameter()*0.5+manu_space,"circular plate inside arc radius(= tube radius + processing clearance)",PARAM_SHAPE);
#else
					pCirRibPlate->SetDesignItemValue('D',cirRibDlg.m_fParaD,"环向肋板参数D",PARAM_SHAPE);
					pCirRibPlate->SetDesignItemValue('H',cirRibDlg.m_fParaH,"环向肋板参数H",PARAM_SHAPE);
				}
				pCirRibPlate->SetDesignItemValue('O',O,"正值表示肋板顶部为圆弧连接，负值表示肋板顶部为直线连接");
			}
			
			pCirRibPlate->SetDesignItemValue('S',cirRibDlg.m_fParaS,"环向板到定位节点的偏移距离");
			pCirRibPlate->SetDesignItemValue('R',pDatumTube->GetDiameter()*0.5+manu_space,"环向板内圆弧半径(=钢管半径+加工间隙)",PARAM_SHAPE);
#endif
			pCirRibPlate->ucs.axis_z.Set(cirRibDlg.m_fNormX,cirRibDlg.m_fNormY,cirRibDlg.m_fNormZ);
			normalize(pCirRibPlate->ucs.axis_z);
			pCirRibPlate->DesignPlate();
			pCirRibPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pCirRibPlate->GetSolidPartObject());
			if(cirRibDlg.m_bMirCreate)
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
				pMirPlate->CopyProperty(pCirRibPlate);
				pMirPlate->SetLayer(pDatumTube->layer());
				pMirPlate->cfgword=pDatumTube->cfgword;
				pMirPlate->m_hPartWeldParent=pDatumTube->handle;
#ifdef AFX_TARG_ENU_ENGLISH
				pMirPlate->SetDesignItemValue('Y',-Y,"positive value or zero shows rib plate is set outside of datum plate, otherwise, inside");
#else
				pMirPlate->SetDesignItemValue('Y',-Y,"正值或零表示肋板在基准钢板的外侧，否则位于内侧");
#endif
				pMirPlate->ucs.axis_z=pCirRibPlate->ucs.axis_z;
				pMirPlate->DesignPlate();
				pMirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pMirPlate->GetSolidPartObject());
			}
			g_pSolidDraw->Draw();
			//判断基准钢板是否为贯通连板 wht 10-03-22
			if(pFirstDatumPlate&&pSecondDatumPlate)
			{
				if(pFirstDatumPlate->IsTransPlate()||pSecondDatumPlate->IsTransPlate())
				{	//贯通连板
#ifdef AFX_TARG_ENU_ENGLISH
					if(AfxMessageBox("Whether to generate simultaneously rib plate on the other side of the through board(Y/N)?",MB_YESNO)==IDYES)
#else
					if(AfxMessageBox("是否同时生成贯通板另一侧的肋板(Y/N)?",MB_YESNO)==IDYES)
#endif
					{	//同时生成贯通连板另一侧的肋板 
						CLDSParamPlate *pMirPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
						pMirPlate->CopyProperty(pCirRibPlate);
						pMirPlate->SetLayer(pDatumTube->layer());
						pMirPlate->cfgword=pDatumTube->cfgword;
						pMirPlate->m_hPartWeldParent=pDatumTube->handle;
#ifdef AFX_TARG_ENU_ENGLISH
						pMirPlate->SetDesignItemValue('B',pSecondDatumPlate->handle,"the first datum plate",PARAM_POS,1);
						pMirPlate->SetDesignItemValue('C',pFirstDatumPlate->handle,"the second datum plate",PARAM_POS,1);
						pMirPlate->SetDesignItemValue('W',cirRibDlg.m_fParaN,"circular rib plate parameter W",PARAM_SHAPE);
						pMirPlate->SetDesignItemValue('N',cirRibDlg.m_fParaW,"circular rib plate parameter N",PARAM_SHAPE);
#else
						pMirPlate->SetDesignItemValue('B',pSecondDatumPlate->handle,"第一块基准钢板",PARAM_POS,1);
						pMirPlate->SetDesignItemValue('C',pFirstDatumPlate->handle,"第二块基准钢板",PARAM_POS,1);
						pMirPlate->SetDesignItemValue('W',cirRibDlg.m_fParaN,"环向肋板参数W",PARAM_SHAPE);
						pMirPlate->SetDesignItemValue('N',cirRibDlg.m_fParaW,"环向肋板参数N",PARAM_SHAPE);
#endif
						pMirPlate->ucs.axis_z=pCirRibPlate->ucs.axis_z;
						pMirPlate->DesignPlate();
						pMirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
						g_pSolidDraw->NewSolidPart(pMirPlate->GetSolidPartObject());
					}
				}
			}
			g_pSolidDraw->Draw();
			m_pDoc->SetModifiedFlag();
			Ta.EndUndoListen();
		}
	}
	else if(pDatumSphere)
	{	//基准构件为球时不必选择装备定位节点
		pBaseNode=console.FromNodeHandle(pDatumSphere->hDatumNode);
		if(pBaseNode==NULL)
			return FALSE;
		cirRibDlg.m_pDatumNode=pBaseNode;
		cirRibDlg.m_pDatumTube=NULL;	
		cirRibDlg.m_pDatumPlate1=pFirstDatumPlate;
		cirRibDlg.m_pDatumPlate2=NULL;
		cirRibDlg.m_pDatumSphere=pDatumSphere;
		if(cirRibDlg.DoModal()==IDOK)
		{
			Ta.BeginUndoListen();
			CLDSParamPlate *pCirRibPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pCirRibPlate->m_iParamType = TYPE_CIRRIBPLATE;
			pCirRibPlate->SetPartNo(cirRibDlg.m_sPartNo.GetBuffer(0));	//编号
			pCirRibPlate->iSeg = SEGI(cirRibDlg.m_sSegI.GetBuffer());	//段号
			pCirRibPlate->SetLayer(pDatumSphere->layer());	//调整环向板的图层名
			pCirRibPlate->cfgword=pDatumSphere->cfgword;
			pCirRibPlate->cMaterial = CSteelMatLibrary::RecordAt(cirRibDlg.m_iMaterial).cBriefMark;	//材质
			pCirRibPlate->thick = (long)cirRibDlg.m_fPlateT;
			pCirRibPlate->m_hPartWeldParent = pDatumSphere->handle;
			pCirRibPlate->cfgword=pDatumTube->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
#ifdef AFX_TARG_ENU_ENGLISH
			pCirRibPlate->SetDesignItemValue('A',pBaseNode->handle,"location datum node",PARAM_POS,1);
			pCirRibPlate->SetDesignItemValue('B',pFirstDatumPlate->handle,"the first datum plate",PARAM_POS,1);
			double Y=-1;
			pCirRibPlate->SetDesignItemValue('C',0,"the second datum plate",PARAM_POS,1);
			if(cirRibDlg.m_bDesignOuterPlate)
				Y=1;
			pCirRibPlate->SetDesignItemValue('Y',Y,"positive value or zero shows rib plate is set outside of datum plate, otherwise, inside");
			pCirRibPlate->SetDesignItemValue('W',cirRibDlg.m_fParaW,"circular rib plate parameter W",PARAM_SHAPE);
			pCirRibPlate->SetDesignItemValue('D',cirRibDlg.m_fParaD,"circular rib plate parameter D",PARAM_SHAPE);
			pCirRibPlate->SetDesignItemValue('H',cirRibDlg.m_fParaH,"circular rib plate parameter H",PARAM_SHAPE);
			pCirRibPlate->SetDesignItemValue('S',cirRibDlg.m_fParaS,"offset distance from circular plate to location node");
			pCirRibPlate->SetDesignItemValue('R',pDatumSphere->D*0.5,"circular plate inside arc radius",PARAM_SHAPE);
#else
			pCirRibPlate->SetDesignItemValue('A',pBaseNode->handle,"定位基准节点",PARAM_POS,1);
			pCirRibPlate->SetDesignItemValue('B',pFirstDatumPlate->handle,"第一块基准钢板",PARAM_POS,1);
			double Y=-1;
			pCirRibPlate->SetDesignItemValue('C',0,"第二块基准钢板",PARAM_POS,1);
			if(cirRibDlg.m_bDesignOuterPlate)
				Y=1;
			pCirRibPlate->SetDesignItemValue('Y',Y,"正值或零表示肋板在基准钢板的外侧，否则位于内侧");
			pCirRibPlate->SetDesignItemValue('W',cirRibDlg.m_fParaW,"环向肋板参数W",PARAM_SHAPE);
			pCirRibPlate->SetDesignItemValue('D',cirRibDlg.m_fParaD,"环向肋板参数D",PARAM_SHAPE);
			pCirRibPlate->SetDesignItemValue('H',cirRibDlg.m_fParaH,"环向肋板参数H",PARAM_SHAPE);
			pCirRibPlate->SetDesignItemValue('S',cirRibDlg.m_fParaS,"环向板到定位节点的偏移距离");
			pCirRibPlate->SetDesignItemValue('R',pDatumSphere->D*0.5,"环向板内圆弧半径",PARAM_SHAPE);
#endif
			pCirRibPlate->ucs.axis_z.Set(cirRibDlg.m_fNormX,cirRibDlg.m_fNormY,cirRibDlg.m_fNormZ);
			normalize(pCirRibPlate->ucs.axis_z);
			pCirRibPlate->DesignPlate();
			pCirRibPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pCirRibPlate->GetSolidPartObject());
			if(cirRibDlg.m_bMirCreate)
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
				pMirPlate->CopyProperty(pCirRibPlate);
				pMirPlate->SetLayer(pDatumSphere->layer());
				pMirPlate->cfgword=pDatumSphere->cfgword;
				pMirPlate->m_hPartWeldParent=pDatumSphere->handle;
#ifdef AFX_TARG_ENU_ENGLISH
				pMirPlate->SetDesignItemValue('Y',-Y,"positive value or zero shows that rib plate is set outside of datum plate, otherwise, inside");
#else
				pMirPlate->SetDesignItemValue('Y',-Y,"正值或零表示肋板在基准钢板的外侧，否则位于内侧");
#endif
				pMirPlate->ucs.axis_z=pCirRibPlate->ucs.axis_z;
				pMirPlate->DesignPlate();
				pMirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pMirPlate->GetSolidPartObject());
			}
			g_pSolidDraw->Draw();
			m_pDoc->SetModifiedFlag();
			Ta.EndUndoListen();
		}
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("plate selected has no welding parent parts, cannot meet the requirements to design circular rib plate, design interrupt!");
#else
		MessageBox("所选钢板无焊接父构件,不满足设计环向肋板的条件,设计中断!");
#endif
		return FALSE;
	}
#endif
	return TRUE;
}

void CLDSView::OnInsertPlateStyleTubeInters() 
{
	m_nPrevCommandID=ID_INSERT_PLATE_STYLE_TUBE_INTERS;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat plate tube cross connection";
#else
	m_sPrevCommandName="重复插板式钢管交叉连接";
#endif
	Command("InsertPlateTubeInters");
}

CXInsertPlateIntersTubeDlg inters_tube_dlg;

//布置钢管插板上的螺栓和附加板
BOOL LayoutInsertPlateBoltAndAttachPlate(CLDSParamPlate *pParamPlate,char* sPartNoP,
										 char* sPartNoX,BOOL bUseLineAngle,char *sJgGuige)
{
	//if(nBoltNum%2)	//保证螺栓个数为偶数
	//	nBoltNum+=1;
	double A=0,N=0,M=0,R=0,L=0,H=0,S=0,W=0;
	if(pParamPlate->m_bStdPart)
	{	
		CLDSStdPartModel stdPartModel;
		if(!pParamPlate->GetStdPartModel(&stdPartModel))
			return FALSE;
		//生成标准构件上的螺栓
		stdPartModel.CreateBoltInPlate(pParamPlate);
		//绘制标准构件上的螺栓
		for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
			g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
		A=stdPartModel.param.insert_plate_param.A;
		N=stdPartModel.param.insert_plate_param.N;
		M=stdPartModel.param.insert_plate_param.M;
		R=stdPartModel.param.insert_plate_param.R;
		L=stdPartModel.param.insert_plate_param.L;
		H=stdPartModel.param.insert_plate_param.H;
		S=stdPartModel.param.insert_plate_param.S;
		W=stdPartModel.param.insert_plate_param.W;
	}
	else
	{
		pParamPlate->GetDesignItemValue('A',&A);
		pParamPlate->GetDesignItemValue('N',&N);
		pParamPlate->GetDesignItemValue('M',&M);
		pParamPlate->GetDesignItemValue('R',&R);
		pParamPlate->GetDesignItemValue('L',&L);
		pParamPlate->GetDesignItemValue('H',&H);
		pParamPlate->GetDesignItemValue('S',&S);
		pParamPlate->GetDesignItemValue('W',&W);
		//布置插板上的螺栓
		inters_tube_dlg.CreateBolt(pParamPlate);
	}
	
	double x=A+M;
	double y=R;
	double z=0.5*pParamPlate->GetThick();
	int i;
	CLDSPlate *pUpVertPlate=NULL,*pDownVertPlate=NULL; //Z正方向立板  Z负方向立板
	if(pParamPlate->m_iParamType==TYPE_XEND)
	{	//生成十字板上的附加板并且加入到附加构件列表中
		CString sPartNo;
		//十字插板需要布置立板
		//十字插板立板法线设计基准构件为基板			//wht 10-03-06
		//X-Y平面上的包板及螺栓法线设计基准构件为基板
		//X-Z平面上的包板及螺栓法线设计基准构件为对应的立板,立板法线修改后包板及螺栓法线能及时更新
		for(i=0;i<2;i++)
		{
			CLDSPlate *pVertPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
			pVertPlate->iSeg=pParamPlate->iSeg;
			pVertPlate->SetLayer(pParamPlate->layer()); //调整包板的图层名
			pVertPlate->cfgword=pParamPlate->cfgword;
			pVertPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
			pVertPlate->cMaterial = pParamPlate->cMaterial;
			pVertPlate->m_hPartWeldParent=pParamPlate->handle;//根据父构件的参数设计包板
			pVertPlate->Thick=ftoi(pParamPlate->GetThick());
			pVertPlate->SetPartNo(sPartNoP);
			pVertPlate->jdb_style=5;	//基准构件上相对位置不变的附加板
			//.................................................
			//此处有问题尽量不去使用jdb_style 稍后修改 
			//.................................................
			pVertPlate->designInfo.m_hBasePart=pParamPlate->handle;
			pVertPlate->designInfo.norm.norm_style=4;	//基准构件上的相对方向
			pVertPlate->designInfo.norm.hVicePart=pParamPlate->handle;
			pVertPlate->designInfo.origin.datum_pos_style=9;//基准构件上的相对坐标
			pVertPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
			pVertPlate->m_fNormOffset=-z;
			if(i==0)
			{	//Z正方向的立板
				pUpVertPlate=pVertPlate;	//记录Z正方向立板
				pVertPlate->designInfo.origin.datum_pos.Set(0,0,z);
				pVertPlate->designInfo.norm.nearVector.Set(0,-1,0);
				//引用相应的螺栓到立板上
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{
					if(pLsRef->start1_end2_mid0==31)//引入螺栓到Z正方向立板上
					{
						pVertPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pVertPlate->handle,TRUE);
					}
				}
			}
			else if(i==1)
			{	//Z负方向的立板
				pDownVertPlate=pVertPlate;	//记录Z负方向立板
				pVertPlate->designInfo.origin.datum_pos.Set(0,0,-z);
				pVertPlate->designInfo.norm.nearVector.Set(0,1,0);
				//引用相应的螺栓到立板上
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{
					if(pLsRef->start1_end2_mid0==41)//引入螺栓到Z负方向立板上
					{
						pVertPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pVertPlate->handle,TRUE);
					}
				}
			}
			pVertPlate->DesignSetupUcs();
			pVertPlate->vertex_list.append(PROFILE_VERTEX(0,0,0));
			pVertPlate->vertex_list.append(PROFILE_VERTEX(L,0,0));
			pVertPlate->vertex_list.append(PROFILE_VERTEX(L,0.5*H-z,0));
			pVertPlate->vertex_list.append(PROFILE_VERTEX(0,0.5*H-z,0));
			pVertPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pVertPlate->GetSolidPartObject());
			//将立板添加到附加构件列表中
			pParamPlate->AppendAttachPart(pVertPlate);
		}
		if(bUseLineAngle)
		{	//使用角钢代替包板
			double fAngleY=0.5*pParamPlate->GetThick();	//短角钢定位Y坐标
			double wing_wide=N+R-0.5*pParamPlate->GetThick();
			double wing_thick=W;
			if(sJgGuige)	//根据指定角钢规格生成角钢 wht 11-06-01
				restore_JG_guige(sJgGuige,wing_wide,wing_thick);
			for(i=0;i<4;i++)
			{	//生成四根短角钢代替八块包板
				CLDSLineAngle *pShortJg=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
				pShortJg->iSeg=pParamPlate->iSeg;
				pShortJg->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
				pShortJg->cMaterial = pParamPlate->cMaterial;
				pShortJg->SetLayer(pParamPlate->layer()); //调整包板的图层名
				pShortJg->cfgword=pParamPlate->cfgword;
				//角钢为螺栓连接，不应该有焊接父构件	wht 10-08-02
				//pShortJg->m_hPartWeldParent=pParamPlate->handle;//根据父构件的参数设计包板
				pShortJg->set_wing_thick(wing_thick);	//肢厚
				pShortJg->set_wing_wide(wing_wide);		//肢宽
				pShortJg->SetPartNo(sPartNoX);
				//位置参数
				pShortJg->desStartPos.SetToDatumPointStyle();	//直接指定基点
				pShortJg->desStartPos.datumPoint.datum_pos_style=9;	//指定构件上的相对坐标
				pShortJg->desStartPos.datumPoint.des_para.hPart=pParamPlate->handle;
				pShortJg->desEndPos.SetToDatumPointStyle();	//直接指定基点
				pShortJg->desEndPos.datumPoint.datum_pos_style=9;	//指定构件上的相对坐标
				pShortJg->desEndPos.datumPoint.des_para.hPart=pParamPlate->handle;
				//法线参数
				pShortJg->des_norm_x.spec_norm.norm_style=4;//指定构件上的相对方向
				pShortJg->des_norm_x.bByOtherWing=FALSE;
				pShortJg->des_norm_x.bSpecific=TRUE;
				pShortJg->des_norm_y.spec_norm.norm_style=4;//指定构件上的相对方向
				pShortJg->des_norm_y.bByOtherWing=FALSE;
				pShortJg->des_norm_y.bSpecific=TRUE;
				if(i==0)
				{	//X-Y平面: Y负Z上
					pShortJg->desStartPos.datumPoint.datum_pos.Set(x,-fAngleY,z);
					pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,-fAngleY,z);
					//X肢法线方向随Z正方向侧立板 Y肢法线方向随基板
					pShortJg->des_norm_x.spec_norm.hVicePart=pUpVertPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
					pShortJg->des_norm_y.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,-1);
					//引入螺栓
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y负方向一侧的螺栓 X-Z平面:Z正方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22
							||pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pShortJg->AppendMidLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
						}
					}
				}
				else if(i==1)
				{	//X-Y平面: Y正Z上
					pShortJg->desStartPos.datumPoint.datum_pos.Set(x,fAngleY,z);
					pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,fAngleY,z);
					//Y肢法线方向随Z负方向侧立板 X肢法线方向随基板
					pShortJg->des_norm_y.spec_norm.hVicePart=pUpVertPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
					pShortJg->des_norm_x.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
					//引入螺栓
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y正方向一侧的螺栓 X-Z平面:Z正方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12
							||pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pShortJg->AppendMidLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
						}
					}
				}
				else if(i==2)
				{	//X-Y平面: Y负Z下
					pShortJg->desStartPos.datumPoint.datum_pos.Set(x,-fAngleY,-z);
					pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,-fAngleY,-z);
					//X肢法线方向随Z负方向侧立板 Y肢法线方向随基板
					pShortJg->des_norm_y.spec_norm.hVicePart=pDownVertPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
					pShortJg->des_norm_x.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,1);
					//引入螺栓
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y负方向一侧的螺栓 X-Z平面:Z负方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22
							||pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
						{
							pShortJg->AppendMidLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
						}
					}
				} 
				else if(i==3)
				{	//X-Y平面: Y正Z下
					pShortJg->desStartPos.datumPoint.datum_pos.Set(x,fAngleY,-z);
					pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,fAngleY,-z);
					//Y肢法线方向随Z正方向侧立板 X肢法线方向随基板
					pShortJg->des_norm_x.spec_norm.hVicePart=pDownVertPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
					pShortJg->des_norm_y.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
					//引入螺栓
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y正方向一侧的螺栓 X-Z平面:Z负方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12
							||pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
						{
							pShortJg->AppendMidLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
						}
					}
				}
				pShortJg->ClearFlag();
				pShortJg->CalPosition();
				pShortJg->cal_x_wing_norm();
				pShortJg->cal_y_wing_norm();
				pShortJg->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pShortJg->GetSolidPartObject());
				//将包板添加到附加构件列表中
				pParamPlate->AppendAttachPart(pShortJg);
			}
		}
		else
		{	//包板为普通钢板
			for(i=0;i<8;i++)
			{
				CLDSPlate *pPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
				pPlate->iSeg=pParamPlate->iSeg;
				pPlate->SetLayer(pParamPlate->layer()); //调整包板的图层名
				pPlate->cfgword=pParamPlate->cfgword;
				pPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
				pPlate->cMaterial = pParamPlate->cMaterial;
				//包板为螺栓连接，不应该有焊接父构件	wht 10-08-02
				//pPlate->m_hPartWeldParent=pParamPlate->handle;//根据父构件的参数设计包板
				pPlate->Thick=ftoi(W);
				pPlate->SetPartNo(sPartNoX);
				pPlate->jdb_style=5;	//基准构件上相对位置不变的附加板
				pPlate->designInfo.m_hBasePart=pParamPlate->handle;
				pPlate->designInfo.norm.norm_style=4;	//基准构件上的相对方向
				pPlate->designInfo.origin.datum_pos_style=9;//基准构件上的相对坐标
				if(i==0)
				{	//X-Y平面: Y负Z上
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,-1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,-y,z);
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y负方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==1)
				{	//X-Y平面: Y负Z下
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,-1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,-y,-z);
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y负方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==2)
				{	//X-Y平面: Y正Z上
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,y,z);
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y正方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==3)
				{	//X-Y平面: Y正Z下
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,y,-z);
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y正方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==4)
				{	//X-Z平面: Y下Z正
					pPlate->designInfo.norm.hVicePart=pUpVertPlate->handle;			//上侧立板
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pUpVertPlate->handle;	//上侧立板
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,z);				//y=R z=半个板厚
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Z平面:Z正方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==5)
				{	//X-Z平面: Y上Z正
					pPlate->designInfo.norm.hVicePart=pUpVertPlate->handle;			//上侧立板
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pUpVertPlate->handle;	//上侧立板
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,-z);				//y=R z=半个板厚
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Z平面:Z正方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==6)
				{	//X-Z平面: Y下Z负
					pPlate->designInfo.norm.hVicePart=pDownVertPlate->handle;		//下侧立板
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pDownVertPlate->handle;//下侧立板
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,-z);				//y=R z=半个板厚
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Z平面:Z负方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==7)
				{	//X-Z平面: Y上Z负
					pPlate->designInfo.norm.hVicePart=pDownVertPlate->handle;		//下侧立板
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pDownVertPlate->handle;//下侧立板
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,z);				//y=R z=半个板厚
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Z平面:Z负方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				pPlate->DesignSetupUcs();
				pPlate->vertex_list.append(PROFILE_VERTEX(0,0,0));
				pPlate->vertex_list.append(PROFILE_VERTEX(S,0,0));
				pPlate->vertex_list.append(PROFILE_VERTEX(S,N,0));
				pPlate->vertex_list.append(PROFILE_VERTEX(0,N,0));
				pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				//将包板添加到附加构件列表中
				pParamPlate->AppendAttachPart(pPlate);
			}
		}
	}
	for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
	{	
		if(pParamPlate->m_iParamType==TYPE_XEND) 
		{
			//将螺栓引入立板/包板后调整螺栓法向偏移量
			(*pLsRef)->des_base_pos.norm_offset.AddThick(-(long)W);	
			//调整十字插板螺栓法线设计参数以及位置设计参数	wht 10-03-06
			if(pUpVertPlate&&pDownVertPlate)
			{	//X-Z平面:Z正方向一侧的螺栓
				if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
				{
					(*pLsRef)->des_work_norm.norm_style=4;	//基准构件上的相对方向
					(*pLsRef)->des_work_norm.nearVector.Set(0,0,1);
					(*pLsRef)->des_work_norm.hVicePart=pUpVertPlate->handle;
					//调整位置参数
					(*pLsRef)->des_base_pos.hPart=pUpVertPlate->handle;
					double wing_offset_dist=fabs((*pLsRef)->des_base_pos.wing_offset_dist);
					double len_offset_dist=fabs((*pLsRef)->des_base_pos.len_offset_dist);
					(*pLsRef)->des_base_pos.wing_offset_dist=wing_offset_dist-z; // z=半个板厚
					(*pLsRef)->des_base_pos.len_offset_dist=len_offset_dist;
				}
				//X-Z平面:Z负方向一侧的螺栓
				else if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
				{
					(*pLsRef)->des_work_norm.norm_style=4;	//基准构件上的相对方向
					(*pLsRef)->des_work_norm.nearVector.Set(0,0,1);
					(*pLsRef)->des_work_norm.hVicePart=pDownVertPlate->handle;
					//调整位置参数
					(*pLsRef)->des_base_pos.hPart=pDownVertPlate->handle;
					double wing_offset_dist=fabs((*pLsRef)->des_base_pos.wing_offset_dist);
					double len_offset_dist=fabs((*pLsRef)->des_base_pos.len_offset_dist);
					(*pLsRef)->des_base_pos.wing_offset_dist=wing_offset_dist-z; // z=半个板厚
					(*pLsRef)->des_base_pos.len_offset_dist=len_offset_dist;
				}
			}
		}
		(*pLsRef)->correct_pos();
		(*pLsRef)->CalGuigeAuto();
		(*pLsRef)->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
	}
	return TRUE;
}

//因CommonLjSmartDesign.obj使用该函数，不能直接添加参数，暂时重载实现 wht 11-06-01
BOOL LayoutInsertPlateBoltAndAttachPlate(CLDSParamPlate *pParamPlate,char* sPartNoP,
										 char* sPartNoX,BOOL bUseLineAngle)
{
	return LayoutInsertPlateBoltAndAttachPlate(pParamPlate,sPartNoP,sPartNoX,bUseLineAngle,NULL);	
}

int CLDSView::InsertPlateTubeInters()
{
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	g_pSolidDraw->ReleaseSnapStatus();
	//选择基准钢管
	CLDSLineTube *pDatumTube=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("InsertPlateTubeInters Please select datum tube in cross-connection:","");
#else
	pCmdLine->FillCmdLine("InsertPlateTubeInters 请选择插板式交叉连接的基准钢管:","");
#endif
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verify(OBJPROVIDER::SOLIDSPACE,GetSingleWord((SELECTINDEX_LINETUBE)));
	verify.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
	while(1)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verify)<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		pDatumTube=(CLDSLineTube*)Ta.FromPartHandle(obj.hRelaObj,CLS_LINETUBE);
		if(pDatumTube)
			break;
	}
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pDatumTube->handle));
	g_pSolidDraw->SetEntSnapStatus(pDatumTube->handle);
	//选择交叉节点;
	CLDSNode *pBaseNode=NULL;
	CLDSLineTube *pCrossTubeSeg1=NULL,*pCrossTubeSeg2=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("InsertPlateTubeInters Please select cross structure node for plate tube:","");
#else
	pCmdLine->FillCmdLine("InsertPlateTubeInters 请选择插板式钢管交叉构造节点:","");
#endif
	verify.ClearSnapFlag();
	verify.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
	verify.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
	{
	CDisplayNodeAtFrontLife displayNode;
	displayNode.DisplayNodeAtFront();
	while(1)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verify)<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		pBaseNode=console.FromNodeHandle(obj.hRelaObj);
		if(pBaseNode&&pBaseNode->m_cPosCalType==4)
		{	//交叉节点
			pCrossTubeSeg1=(CLDSLineTube*)console.FromPartHandle(pBaseNode->arrRelationPole[0],CLS_LINETUBE);
			pCrossTubeSeg2=(CLDSLineTube*)console.FromPartHandle(pBaseNode->arrRelationPole[1],CLS_LINETUBE);
			if(pCrossTubeSeg1&&pCrossTubeSeg2)
			{
				f3dLine baseline1(pCrossTubeSeg1->pStart->Position(false),pCrossTubeSeg1->pEnd->Position(false));
				f3dLine baseline2(pCrossTubeSeg2->pStart->Position(false),pCrossTubeSeg2->pEnd->Position(false));
				int ret1=baseline1.PtInLine(pBaseNode->Position(false));
				int ret2=baseline2.PtInLine(pBaseNode->Position(false));
				if(ret1==2&&ret2==1)
				{
					pDatumTube=pCrossTubeSeg1;
					pCrossTubeSeg1=pCrossTubeSeg2;	//让待搜索的被截断交叉构件始终赋值给pCrossTubeSeg2
				}
				else if(ret2==2&&ret1==1)
					pDatumTube=pCrossTubeSeg2;
				else if(ret2==1&&ret1==1)	
				{	//两根交叉斜材均被打断	须排除不合理的交叉材
					f3dPoint datum_line_vec=pDatumTube->End()-pDatumTube->Start();
					normalize(datum_line_vec);
					f3dPoint cross_vec1=pCrossTubeSeg1->End()-pCrossTubeSeg1->Start();
					normalize(cross_vec1);
					f3dPoint cross_vec2=pCrossTubeSeg2->End()-pCrossTubeSeg2->Start();
					normalize(cross_vec2);
					if(fabs(datum_line_vec*cross_vec1)>EPS_COS2&&fabs(datum_line_vec*cross_vec2)>EPS_COS2)
					{	//不合理返回
						pCrossTubeSeg1=pCrossTubeSeg2=NULL;
						return 0;
					}
					else if(fabs(datum_line_vec*cross_vec1)>EPS_COS2)
					{
						pCrossTubeSeg1=pCrossTubeSeg2;
						pCrossTubeSeg2=NULL;
					}
					else if(fabs(datum_line_vec*cross_vec2)>EPS_COS2)
						pCrossTubeSeg2=NULL;

				}
				if((ret1+ret2==3)||(ret1==1&&ret2==1))
				{	//初步满足条件，现在开始搜索被截断的交叉构件
					f3dLine baseline(pCrossTubeSeg1->pStart->Position(false),pCrossTubeSeg1->pEnd->Position(false));
					for(pCrossTubeSeg2=(CLDSLineTube*)Ta.Parts.GetFirst(CLS_LINETUBE);pCrossTubeSeg2;pCrossTubeSeg2=(CLDSLineTube*)Ta.Parts.GetNext(CLS_LINETUBE))
					{
						if((pCrossTubeSeg2->pStart==pBaseNode&&baseline.PtInLine(pCrossTubeSeg2->pEnd->Position(false))<0) ||
							(pCrossTubeSeg2->pEnd==pBaseNode&&baseline.PtInLine(pCrossTubeSeg2->pStart->Position(false))<0))
							break;	//插板式钢管交叉构造的全部条件满足,找到另一根被截断的交叉构件
					}
					if(pCrossTubeSeg2)
						break;
				}
			}
			//没有选择合适的交叉构造节点
			g_pSolidDraw->ReleaseSnapStatus();
			pBaseNode = NULL;
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("InsertPlateTubeInters node selected not the ideal plate tube cross structural node, please choose cross structure node again:","");
#else
			pCmdLine->FillCmdLine("InsertPlateTubeInters 所选节点不是合理的插板式钢管交叉构造节点,请重新选择交叉构造节点:","");
#endif
		}
	}
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pBaseNode->handle));
	g_pSolidDraw->SetEntSnapStatus(pBaseNode->handle);
	}
	//
	inters_tube_dlg.m_pCrossTube=pCrossTubeSeg1;
	inters_tube_dlg.m_pDatumTube=pDatumTube;
	if(inters_tube_dlg.DoModal()!=IDOK)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		return 0;
	}
	CUndoOperObject undo(&Ta,false);
	//一、计算截断的交叉材正负头
	double L=inters_tube_dlg.m_nDatumPlateLen;
	double M=inters_tube_dlg.m_nInsertPlateWeldLen;
	double H=inters_tube_dlg.m_nDatumPlateWidth;
	double N=inters_tube_dlg.m_fPlateParamN;
	double S=inters_tube_dlg.m_fPlateParamS;
	double A=inters_tube_dlg.m_fPlateParamA;
	double R=inters_tube_dlg.m_fPlateParamR;
	double W=inters_tube_dlg.m_fPlateParamW;
	f3dPoint datum_line_vec=pDatumTube->End()-pDatumTube->Start();
	f3dPoint cross_line_vec=pCrossTubeSeg1->End()-pCrossTubeSeg1->Start();
	if(pBaseNode==pCrossTubeSeg1->pEnd)
		cross_line_vec*=-1.0;	
	normalize(datum_line_vec);
	normalize(cross_line_vec);
	double cosa = fabs(datum_line_vec*cross_line_vec);
	double sina = sqrt(1-cosa*cosa);
	double oddment = inters_tube_dlg.m_nTubeOddment;
	//二、设计截断的交叉材的插板
	double fTubeNormOffset=0;
	BYTE ciJointType=0;
	CLDSParamPlate *pParamPlate1=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pParamPlate1->m_hPartWeldParent=pCrossTubeSeg1->handle;
	pParamPlate1->cfgword=pCrossTubeSeg1->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
	if(inters_tube_dlg.m_iInsertPlateType==0)
	{	//槽型端板
		pParamPlate1->m_iParamType=TYPE_ROLLEND;
		ciJointType=4;
	}
	else if(inters_tube_dlg.m_iInsertPlateType==1)
	{	//U型端板
		pParamPlate1->m_iParamType=TYPE_UEND;
		ciJointType=3;
	}
	else if(inters_tube_dlg.m_iInsertPlateType==2)
	{	//X型端板
		pParamPlate1->m_iParamType=TYPE_XEND;
		ciJointType=5;
	}
	pParamPlate1->cMaterial=CSteelMatLibrary::RecordAt(inters_tube_dlg.m_iDatumPlateMat).cBriefMark;
	pParamPlate1->iSeg=SEGI(inters_tube_dlg.m_sSegI.GetBuffer());
	//标准十字插板代号与基板编号不相同，	wht 10-01-24
	//为兼容以前版本只要编号与代号或者基准板编号相同则认为时同一个规格十字插板
	pParamPlate1->SetPartNo(inters_tube_dlg.m_sDatumPlatePartNo.GetBuffer(0));
	pParamPlate1->thick=inters_tube_dlg.m_nDatumPlateThick;	//厚度
	pParamPlate1->m_bStdPart=inters_tube_dlg.m_bStdPart;
	pParamPlate1->SetDesignItemValue('L',L,"",PARAM_SHAPE);	//插板长度
	pParamPlate1->SetDesignItemValue('M',M,"",PARAM_SHAPE);	//焊接长度
	pParamPlate1->SetDesignItemValue('H',H,"",PARAM_SHAPE);	//插板宽度
	pParamPlate1->SetDesignItemValue('D',0,"");	//基准X轴相对钢管X轴旋转角度
	if(inters_tube_dlg.m_iInsertPlateType==1)	//U型端板
	{
		pParamPlate1->SetDesignItemValue('W',W,"",PARAM_SHAPE);	//U型插板上下两面的间距
		pParamPlate1->SetDesignItemValue('N',N,"",PARAM_SHAPE);	//切角长度
		pParamPlate1->SetDesignItemValue('S',S,"",PARAM_SHAPE);	//切角宽度	
	}
	else if(inters_tube_dlg.m_iInsertPlateType==0)	//平面卷边端板
	{
		pParamPlate1->SetDesignItemValue('W',W,"",PARAM_SHAPE);	//卷边的高度
		pParamPlate1->SetDesignItemValue('N',N,"",PARAM_SHAPE);	//切角长度
		pParamPlate1->SetDesignItemValue('S',S,"",PARAM_SHAPE);	//切角宽度
		pParamPlate1->SetDesignItemValue('D',180,"");
		if(pParamPlate1->m_bStdPart)
			pParamPlate1->SetDesignItemValue('X',180,"");
		fTubeNormOffset=0.5*(pParamPlate1->Thick+inters_tube_dlg.m_nTransPlateThick);
	}
	else if(inters_tube_dlg.m_iInsertPlateType==2)	//X型插板
	{	
		pParamPlate1->SetDesignItemValue('W',W,"",PARAM_SHAPE);	//十字型插板上附加板的厚度
		pParamPlate1->SetDesignItemValue('N',N,"",PARAM_SHAPE);	//十字型插板上附加板的宽度
		pParamPlate1->SetDesignItemValue('S',S,"",PARAM_SHAPE);	//十字型插板上附加板的长度
		pParamPlate1->SetDesignItemValue('A',A,"",PARAM_SHAPE);	//十字型插板上附加板的定位参数A
		pParamPlate1->SetDesignItemValue('R',R,"",PARAM_SHAPE);	//十字型插板上附加板的定位参数R
	}
	if(pCrossTubeSeg1->pStart==pBaseNode)
	{	
		pCrossTubeSeg1->m_tJointStart.hLinkObj=pParamPlate1->handle;
		pCrossTubeSeg1->m_tJointStart.type=ciJointType;
		pCrossTubeSeg1->desStartPos.fNormOffset=fTubeNormOffset;
		pCrossTubeSeg1->SetStartOdd(oddment);
		pCrossTubeSeg1->DesignStartJoint();
	}
	else
	{
		pCrossTubeSeg1->m_tJointEnd.hLinkObj=pParamPlate1->handle;
		pCrossTubeSeg1->m_tJointEnd.type=ciJointType;
		pCrossTubeSeg1->desEndPos.fNormOffset=fTubeNormOffset;
		pCrossTubeSeg1->SetEndOdd(oddment);
		pCrossTubeSeg1->DesignEndJoint();
	}
	char sPartNoP[MAX_CHAR_GUIGE_16+1],sPartNoX[MAX_CHAR_GUIGE_16+1];
	//布置十字插板上的螺栓和附加板
	_snprintf(sPartNoP,MAX_CHAR_GUIGE_16,"%s",inters_tube_dlg.m_sWeldPlatePartNo);
	_snprintf(sPartNoX,MAX_CHAR_GUIGE_16,"%s",inters_tube_dlg.m_sWrapPlatePartNo);
	if(!LayoutInsertPlateBoltAndAttachPlate(pParamPlate1,sPartNoP,sPartNoX,
		inters_tube_dlg.m_bUseAngle,inters_tube_dlg.m_sJgGuige.GetBuffer(1)))
	{	//命令中断，清除冗余数据
		if(pParamPlate1)
		{
			console.DispPartSet.DeleteNode(pParamPlate1->handle);
			g_pSolidDraw->DelEnt(pParamPlate1->handle);
			console.DeletePart(pParamPlate1->handle);
		}
		g_pSolidDraw->ReleaseSnapStatus();
		return 0;
	}
	pCrossTubeSeg1->ClearFlag();
	pCrossTubeSeg1->CalPosition();
	if(pCrossTubeSeg1->pStart==pBaseNode)
		pCrossTubeSeg1->DesignStartJoint();
	else
		pCrossTubeSeg1->DesignEndJoint();
	pParamPlate1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pParamPlate1->GetSolidPartObject());
	
	CLDSParamPlate *pParamPlate2=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pParamPlate2->CopyProperty(pParamPlate1);
	pParamPlate2->m_hPartWeldParent=pCrossTubeSeg2->handle;
	if(pCrossTubeSeg2->pStart==pBaseNode)
	{
		pCrossTubeSeg2->m_tJointStart.hLinkObj=pParamPlate2->handle;
		pCrossTubeSeg2->m_tJointStart.type=ciJointType;
		pCrossTubeSeg2->desStartPos.fNormOffset=fTubeNormOffset;
		pCrossTubeSeg2->SetStartOdd(oddment);
		pCrossTubeSeg2->DesignStartJoint();
	}
	else
	{
		pCrossTubeSeg2->m_tJointEnd.hLinkObj=pParamPlate2->handle;
		pCrossTubeSeg2->m_tJointEnd.type=ciJointType;
		pCrossTubeSeg2->desEndPos.fNormOffset=fTubeNormOffset;
		pCrossTubeSeg2->SetEndOdd(oddment);
		pCrossTubeSeg2->DesignEndJoint();
	}
	//布置插板上的螺栓和附加板
	if(!LayoutInsertPlateBoltAndAttachPlate(pParamPlate2,sPartNoP,sPartNoX,
		inters_tube_dlg.m_bUseAngle,inters_tube_dlg.m_sJgGuige.GetBuffer(1)))
	{	//命令中断，清除冗余数据
		if(pParamPlate2)
		{
			console.DispPartSet.DeleteNode(pParamPlate2->handle);
			g_pSolidDraw->DelEnt(pParamPlate2->handle);
			console.DeletePart(pParamPlate2->handle);
		}
		g_pSolidDraw->ReleaseSnapStatus();
		return 0;
	}
	pCrossTubeSeg2->ClearFlag();
	pCrossTubeSeg2->CalPosition();
	if(pCrossTubeSeg2->pStart==pBaseNode)
		pCrossTubeSeg2->DesignStartJoint();
	else
		pCrossTubeSeg2->DesignEndJoint();
	pParamPlate2->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pParamPlate2->GetSolidPartObject());
	pCrossTubeSeg1->SetModified();
	pCrossTubeSeg2->SetModified();
	pCrossTubeSeg1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	pCrossTubeSeg2->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pCrossTubeSeg1->GetSolidPartObject());
	g_pSolidDraw->NewSolidPart(pCrossTubeSeg2->GetSolidPartObject());

	if(pParamPlate1->GetLsCount()<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("plate's bolt not arranged, design cannot be continued!");
#else
		AfxMessageBox("未布置插板螺栓，无法继续进行设计!");
#endif
		g_pSolidDraw->ReleaseSnapStatus();
		return 0;
	}
	//三、设计贯通交叉材的交叉面连接板
	LSSPACE_STRU LsSpace;
	int ls_d=atoi(inters_tube_dlg.m_sBoltSize);
	CLsRef *pFirstLsRef=pParamPlate1->GetFirstLsRef();
	if(pFirstLsRef)
		ls_d=(*pFirstLsRef)->get_d();
	GetLsSpace(LsSpace,ls_d);
	CLDSPlate *pTransPlate1=NULL, *pTransPlate2=NULL;
	pTransPlate1=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	pTransPlate1->SetPartNo(inters_tube_dlg.m_sTransPlatePartNo.GetBuffer(0));
	pTransPlate1->m_hPartWeldParent=pDatumTube->handle;
	pTransPlate1->cfgword=pDatumTube->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
	pTransPlate1->iSeg=SEGI(inters_tube_dlg.m_sSegI.GetBuffer());
	pTransPlate1->cMaterial=CSteelMatLibrary::RecordAt(inters_tube_dlg.m_iTransPlateMat).cBriefMark;
	pTransPlate1->Thick=inters_tube_dlg.m_nTransPlateThick;
	pTransPlate1->m_uStatMatCoef=1;
	pTransPlate1->m_fNormOffset=-0.5*inters_tube_dlg.m_nTransPlateThick;
	//计算连接板的坐标系
	CDesignLjPartPara *pLjPartPara=pTransPlate1->designInfo.partList.Add(pCrossTubeSeg1->handle);
	pLjPartPara->hPart=pCrossTubeSeg1->handle;
	pLjPartPara->iFaceNo=1;
	pLjPartPara->m_nClassTypeId=pCrossTubeSeg1->GetClassTypeId();
	if(pCrossTubeSeg1->pStart==pBaseNode)
		pLjPartPara->start0_end1=0;	//始端连接
	else if(pCrossTubeSeg1->pEnd==pBaseNode)
		pLjPartPara->start0_end1=1;//终端连接
	if(inters_tube_dlg.m_bTransPlate)
	{	//贯通整连板
		pLjPartPara=pTransPlate1->designInfo.partList.Add(pCrossTubeSeg2->handle);
		pLjPartPara->hPart=pCrossTubeSeg2->handle;
		pLjPartPara->iFaceNo=1;
		pLjPartPara->m_nClassTypeId=pCrossTubeSeg2->GetClassTypeId();
		if(pCrossTubeSeg2->pStart==pBaseNode)
			pLjPartPara->start0_end1=0;	//始端连接
		else if(pCrossTubeSeg2->pEnd==pBaseNode)
			pLjPartPara->start0_end1=1;//终端连接
	}
	pTransPlate1->designInfo.m_bEnableFlexibleDesign=TRUE;	//启动柔性设计
	pTransPlate1->designInfo.iProfileStyle0123=3;			//不进行外形设计
	pTransPlate1->designInfo.m_hBaseNode=pBaseNode->handle;
	pTransPlate1->designInfo.m_hBasePart=pDatumTube->handle;
	pTransPlate1->designInfo.origin.datum_pos_style=2;
	pTransPlate1->designInfo.origin.des_para.RODNODE.hRod=pDatumTube->handle;
	pTransPlate1->designInfo.origin.des_para.RODNODE.hNode=pBaseNode->handle;
	pTransPlate1->designInfo.origin.UpdatePos(console.GetActiveModel());
	pTransPlate1->designInfo.norm.norm_style=2;
	pTransPlate1->designInfo.norm.hVicePart=pDatumTube->handle;
	pTransPlate1->designInfo.norm.hCrossPart=pCrossTubeSeg1->handle;
	pTransPlate1->designInfo.norm.nearVector=pDatumTube->WorkPlaneNorm();
	pTransPlate1->designInfo.norm.UpdateVector(console.GetActiveModel());
	pTransPlate1->ucs.origin=pTransPlate1->designInfo.origin.Position();
	pTransPlate1->ucs.axis_z=pTransPlate1->designInfo.norm.vector;
	pTransPlate1->ucs.axis_y=datum_line_vec;
	normalize(pTransPlate1->ucs.axis_y);
	pTransPlate1->ucs.axis_x=pTransPlate1->ucs.axis_y^pTransPlate1->ucs.axis_z;
	normalize(pTransPlate1->ucs.axis_x);
	f3dPoint tube_seg1_vec;
	if(pCrossTubeSeg1->pStart==pBaseNode)
		tube_seg1_vec=pCrossTubeSeg1->End()-pCrossTubeSeg1->Start();
	else if(pCrossTubeSeg1->pEnd==pBaseNode)
		tube_seg1_vec=pCrossTubeSeg1->Start()-pCrossTubeSeg1->End();
	normalize(tube_seg1_vec);
	if(tube_seg1_vec*pTransPlate1->ucs.axis_x<0)
	{	//调整X轴方向指向第一段钢管
		pTransPlate1->ucs.axis_x*=-1.0;
		pTransPlate1->ucs.axis_y*=-1.0;
	}
		
	f3dPoint vertex_up,vertex_down,bolt_up,bolt_down,weld_vertex_start,weld_vertex_end;
	double minx=1000000,miny=0.5*H,maxy=-0.5*H;
	CLsRef *pLsRef;
	for(pLsRef=pParamPlate1->GetFirstLsRef();pLsRef;pLsRef=pParamPlate1->GetNextLsRef())
	{	//引用螺栓到贯通连板
		CLDSBolt *pBolt=pLsRef->GetLsPtr();
		//钢管正负头修改后螺栓位置和法线方向都有可能变化
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		if((pParamPlate1->m_iParamType==TYPE_XEND&&(pBolt->feature==12||pBolt->feature==22))
			||pParamPlate1->m_iParamType!=TYPE_XEND)
		{
			f3dPoint pos = (*pLsRef)->ucs.origin;
			coord_trans(pos,pParamPlate1->ucs,FALSE);
			if(pos.x<minx)
				minx=pos.x;
			if(pos.y<miny)
				miny=pos.y;
			if(pos.y>maxy)
				maxy=pos.y;
			pBolt->AddL0Thick(pTransPlate1->handle,TRUE);	//调整螺栓通厚	
			pTransPlate1->AppendLsRef(*pLsRef);
		}
	}
	if(inters_tube_dlg.m_bTransPlate)
	{	//贯通整连板
		for(pLsRef=pParamPlate2->GetFirstLsRef();pLsRef;pLsRef=pParamPlate2->GetNextLsRef())
		{	//引用螺栓到贯通连板
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			//钢管正负头修改后螺栓位置和法线方向都有可能变化
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			if((pParamPlate2->m_iParamType==TYPE_XEND&&(pBolt->feature==12||pBolt->feature==22))
				||pParamPlate2->m_iParamType!=TYPE_XEND)
			{
				pBolt->AddL0Thick(pTransPlate1->handle,TRUE);	//调整螺栓通厚
				pTransPlate1->AppendLsRef(pBolt->GetLsRef());
			}
		}
	}
	vertex_up.x=vertex_down.x=minx-LsSpace.EndSpace;
	vertex_up.y=0.5*H;
	vertex_down.y=-vertex_up.y;
	bolt_up.x=bolt_down.x=minx;
	bolt_up.y=maxy;
	bolt_down.y=miny;
	
	f3dPoint temp_vertex;
	PROFILE_VERTEX *pVertex=NULL, *pPrevVertex=NULL;
	if(inters_tube_dlg.m_bTransPlate)
	{	//贯通整连板
		//端板2上的顶点
		temp_vertex=vertex_up;
		coord_trans(temp_vertex,pParamPlate2->ucs,TRUE);
		coord_trans(temp_vertex,pTransPlate1->ucs,FALSE);
		temp_vertex.z=0;
		temp_vertex.feature = 1;
		pVertex=pTransPlate1->vertex_list.append(PROFILE_VERTEX(temp_vertex));
		temp_vertex=vertex_down;
		coord_trans(temp_vertex,pParamPlate2->ucs,TRUE);
		coord_trans(temp_vertex,pTransPlate1->ucs,FALSE);
		temp_vertex.z=0;
		temp_vertex.feature = 1;
		pVertex=pTransPlate1->vertex_list.append(PROFILE_VERTEX(temp_vertex));
	}
	coord_trans(vertex_up,pParamPlate1->ucs,TRUE);
	coord_trans(vertex_down,pParamPlate1->ucs,TRUE);
	coord_trans(bolt_up,pParamPlate1->ucs,TRUE);
	coord_trans(bolt_down,pParamPlate1->ucs,TRUE);
	//端板1上的顶点
	temp_vertex=vertex_up;
	coord_trans(temp_vertex,pTransPlate1->ucs,FALSE);
	temp_vertex.z=0;
	temp_vertex.feature = 1;
	pVertex=pTransPlate1->vertex_list.append(PROFILE_VERTEX(temp_vertex));
	temp_vertex=vertex_down;
	coord_trans(temp_vertex,pTransPlate1->ucs,FALSE);
	temp_vertex.z=0;
	temp_vertex.feature = 1;
	pVertex=pTransPlate1->vertex_list.append(PROFILE_VERTEX(temp_vertex));
	//由系统自动计算焊接区域外形顶点
	f3dPoint offset_vec=0.5*pDatumTube->GetDiameter()*pTransPlate1->ucs.axis_x;
	f3dLine prj_line(pDatumTube->Start()+offset_vec,pDatumTube->End()+offset_vec);
	f3dPoint seed_vertex[4],diffused_up_vec,diffused_down_vec;
	SnapPerp(&seed_vertex[0],prj_line,vertex_up);
	SnapPerp(&seed_vertex[1],prj_line,vertex_down);
	diffused_up_vec = diffused_down_vec = -cross_line_vec;
	//根据国内外经验应力扩散角均取30°,即每侧扩散30°，总计60°
	double diffuseAngle=30*RADTODEG_COEF;
	//计算上下两条扩散线方向的时候应该绕螺栓所在构件的Z轴旋转
	RotateVectorAroundVector(diffused_up_vec,diffuseAngle,pParamPlate1->ucs.axis_z);
	RotateVectorAroundVector(diffused_down_vec,-diffuseAngle,pParamPlate1->ucs.axis_z);
	f3dPoint diffused_upplane_norm = diffused_up_vec^pTransPlate1->ucs.axis_z;
	f3dPoint diffused_downplane_norm = diffused_down_vec^pTransPlate1->ucs.axis_z;
	Int3dlf(seed_vertex[2],prj_line,bolt_up,diffused_upplane_norm);
	Int3dlf(seed_vertex[3],prj_line,bolt_down,diffused_downplane_norm);
	//用扩散角中线纠正无效的扩散线所带来的板外形畸形
	f3dPoint midVertex,diffused_midplane_norm = cross_line_vec^pTransPlate1->ucs.axis_z;
	Int3dlf(midVertex,prj_line,0.5*(bolt_up+bolt_down),diffused_midplane_norm);
	double dd2=DISTANCE(seed_vertex[2],midVertex);
	double dd3=DISTANCE(seed_vertex[3],midVertex);
	offset_vec=seed_vertex[3]-seed_vertex[2];
	normalize(offset_vec);
	f3dPoint vec2=seed_vertex[2]-midVertex;
	normalize(vec2);
	f3dPoint vec3=seed_vertex[3]-midVertex;
	normalize(vec3);
	if(vec2*vec3>0)
	{	//扩散生成的两点在扩散线同一侧，较远的种子点肯定是另一侧种子顶点发生异变导致，故应取较短者并反号
		if(dd2<dd3)
			seed_vertex[3]=midVertex-vec3*dd2;
		else if(dd2>dd3)
			seed_vertex[2]=midVertex-vec2*dd3;
	}
	for(int i=0;i<4;i++)
	{
		if(i==0)
		{
			weld_vertex_start=weld_vertex_end=seed_vertex[0];
			f3dPoint start_vec=seed_vertex[1]-seed_vertex[0];
			if(start_vec*datum_line_vec<0)	//始端侧
				weld_vertex_start=seed_vertex[1];
			else
				weld_vertex_end=seed_vertex[1];
			i++;
		}
		else
		{
			f3dPoint extend_vec;
			//始端侧
			extend_vec = seed_vertex[i]-weld_vertex_start;
			if(extend_vec*datum_line_vec<0)
				weld_vertex_start=seed_vertex[i];
			//终端侧
			extend_vec=seed_vertex[i]-weld_vertex_end;
			if(extend_vec*datum_line_vec>0)
				weld_vertex_end=seed_vertex[i];
		}
	}

	coord_trans(weld_vertex_start,pTransPlate1->ucs,FALSE);
	coord_trans(weld_vertex_end,pTransPlate1->ucs,FALSE);
	//调整板外形 用射线杆件和基准杆件(锐角)之间的焊接点的-Y值取代另一焊接点的Y值
	if(cross_line_vec*datum_line_vec>0)
		weld_vertex_start.y=-weld_vertex_end.y;
	else //if(cross_line_vec*datum_line_vec<0)
		weld_vertex_end.y=-weld_vertex_start.y;
	if(fabs(weld_vertex_start.y)<inters_tube_dlg.m_nTransPlateWidth/2.0)
		weld_vertex_start.y=-inters_tube_dlg.m_nTransPlateWidth/2.0;
	if(fabs(weld_vertex_end.y)<inters_tube_dlg.m_nTransPlateWidth/2.0)
		weld_vertex_end.y=inters_tube_dlg.m_nTransPlateWidth/2.0;
	//焊缝宽造成的焊缝顶点 封板宽度==焊区宽度 wht 11-06-01
	f3dPoint weld_scope_start(weld_vertex_start.x+inters_tube_dlg.m_nCoverPlateWidth,weld_vertex_start.y,weld_vertex_start.z);
	f3dPoint weld_scope_end(weld_vertex_end.x+inters_tube_dlg.m_nCoverPlateWidth,weld_vertex_end.y,weld_vertex_end.z);
	weld_vertex_start.feature = weld_vertex_end.feature = weld_scope_start.feature = weld_scope_end.feature = 1;
	pVertex=pTransPlate1->vertex_list.append(PROFILE_VERTEX(weld_vertex_start));
	pVertex->vertex.ID=1;
	pVertex=pTransPlate1->vertex_list.append(PROFILE_VERTEX(weld_vertex_end));
	pVertex->vertex.ID=1;
	pVertex=pTransPlate1->vertex_list.append(PROFILE_VERTEX(weld_scope_start));
	pVertex=pTransPlate1->vertex_list.append(PROFILE_VERTEX(weld_scope_end));
	if(inters_tube_dlg.m_bTransPlate)
	{	//贯通整连板
		pVertex=pTransPlate1->vertex_list.append(PROFILE_VERTEX(f3dPoint(-weld_vertex_start.x,weld_vertex_start.y,weld_vertex_start.z)));
		pVertex->vertex.feature=1;
		pVertex=pTransPlate1->vertex_list.append(PROFILE_VERTEX(f3dPoint(-weld_vertex_end.x,weld_vertex_end.y,weld_vertex_end.z)));
		pVertex->vertex.feature=1;
		pVertex=pTransPlate1->vertex_list.append(PROFILE_VERTEX(f3dPoint(-weld_scope_start.x,weld_scope_start.y,weld_scope_start.z)));
		pVertex->vertex.feature=1;
		pVertex=pTransPlate1->vertex_list.append(PROFILE_VERTEX(f3dPoint(-weld_scope_end.x,weld_scope_end.y,weld_scope_end.z)));
		pVertex->vertex.feature=1;
	}
	else if(!inters_tube_dlg.m_bTransPlate)
	{
		pPrevVertex=pTransPlate1->vertex_list.GetTail();
		for(pVertex=pTransPlate1->vertex_list.GetFirst();pVertex;pVertex=pTransPlate1->vertex_list.GetNext())
		{
			if(pPrevVertex&&pPrevVertex->vertex.ID==1&&pVertex->vertex.ID==1)
			{
				pPrevVertex->m_bWeldEdge=1;	//焊缝线
				//填充焊接边对活点信息 //焊接父构件为钢管时才需要对活点
				pPrevVertex->local_point_vec=1;	//0.无对活点 1.对活点50-->100方向同焊接父杆件始-->终方向 2.与边始-->终方向相反 
				pPrevVertex->local_point_y=0;	//对活点Y坐标值
			}
			pPrevVertex=pVertex;
		}
	}

	pTransPlate1->DesignProfile();
	pTransPlate1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pTransPlate1->GetSolidPartObject());
	if(!inters_tube_dlg.m_bTransPlate)
	{	//非贯通整连板
		pTransPlate2=(CLDSPlate*)console.AppendPart(CLS_PLATE);
		pTransPlate2->CopyProperty(pTransPlate1);
		pTransPlate2->m_hPartWeldParent=pDatumTube->handle;
		pTransPlate2->cfgword=pDatumTube->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
		pLjPartPara=pTransPlate2->designInfo.partList.Add(pCrossTubeSeg2->handle);
		pLjPartPara->hPart=pCrossTubeSeg2->handle;
		pLjPartPara->iFaceNo=1;
		pLjPartPara->m_nClassTypeId=pCrossTubeSeg2->GetClassTypeId();
		if(pCrossTubeSeg2->pStart==pBaseNode)
			pLjPartPara->start0_end1=0;	//始端连接
		else if(pCrossTubeSeg2->pEnd==pBaseNode)
			pLjPartPara->start0_end1=1;//终端连接
		pTransPlate2->designInfo.m_bEnableFlexibleDesign=TRUE;	//启动柔性设计
		pTransPlate2->designInfo.iProfileStyle0123=3;			//不进行外形设计
		pTransPlate2->designInfo.m_hBaseNode=pBaseNode->handle;
		pTransPlate2->designInfo.m_hBasePart=pDatumTube->handle;
		pTransPlate2->designInfo.origin.datum_pos_style=2;
		pTransPlate2->designInfo.origin.des_para.RODNODE.hRod=pDatumTube->handle;
		pTransPlate2->designInfo.origin.des_para.RODNODE.hNode=pBaseNode->handle;
		pTransPlate2->designInfo.origin.UpdatePos(console.GetActiveModel());
		pTransPlate2->designInfo.norm.norm_style=2;
		pTransPlate2->designInfo.norm.hVicePart=pDatumTube->handle;
		pTransPlate2->designInfo.norm.hCrossPart=pCrossTubeSeg1->handle;
		pTransPlate2->designInfo.norm.nearVector=pDatumTube->WorkPlaneNorm();
		pTransPlate2->designInfo.norm.UpdateVector(console.GetActiveModel());
		pTransPlate2->ucs=pTransPlate1->ucs;
		pTransPlate2->ucs.axis_x*=-1.0;
		pTransPlate2->ucs.axis_y*=-1.0;
		for(pVertex=pTransPlate1->vertex_list.GetFirst();pVertex;pVertex=pTransPlate1->vertex_list.GetNext())
			pTransPlate2->vertex_list.append(*pVertex);
		for(pLsRef=pParamPlate2->GetFirstLsRef();pLsRef;pLsRef=pParamPlate2->GetNextLsRef())
		{	//引用螺栓到贯通连板
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			//钢管正负头修改后螺栓位置和法线方向都有可能变化
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			if((pParamPlate2->m_iParamType==TYPE_XEND&&(pBolt->feature==12||pBolt->feature==22))
				||pParamPlate2->m_iParamType!=TYPE_XEND)
			{
				pBolt->AddL0Thick(pTransPlate2->handle,TRUE);	//调整螺栓通厚
				pTransPlate2->AppendLsRef(pBolt->GetLsRef());
			}
		}
		pTransPlate2->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pTransPlate2->GetSolidPartObject());
	}
	//设计完贯穿板后，统一更新螺栓  wxc-2018.3.14
	for(int i=0;i<2;i++)
	{
		CLDSParamPlate* pParamPlate=NULL;
		if(i==0)
			pParamPlate=pParamPlate1;
		else
			pParamPlate=pParamPlate2;
		for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
		{	
			(*pLsRef)->correct_worknorm();
			(*pLsRef)->correct_pos();
			(*pLsRef)->CalGuigeAuto();
			(*pLsRef)->SetModified();
			(*pLsRef)->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
		}
	}
	//四、设计贯通交叉钢管上的垂向半连板
	double fOffRidus=pDatumTube->GetDiameter()*0.5+inters_tube_dlg.m_fVertPlateManuSpace;
	double fPlateLen=fabs(oddment)+M-minx+LsSpace.EndSpace-fOffRidus/sina; //计算垂向半连板参数W,N的值	 
	if(inters_tube_dlg.m_bCreateVertPlate)
	{
		CLDSParamPlate *pTubeRibPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pTubeRibPlate->m_iParamType=TYPE_TUBERIBPLATE;
		pTubeRibPlate->m_hPartWeldParent=pDatumTube->handle;
		pTubeRibPlate->cfgword=pDatumTube->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
		pTubeRibPlate->layer(2)=pDatumTube->layer(2);	//调整钢管肋板的图层名
		pTubeRibPlate->thick=inters_tube_dlg.m_nVertPlateThick;
		pTubeRibPlate->iSeg=SEGI(inters_tube_dlg.m_sSegI.GetBuffer());
		pTubeRibPlate->cMaterial = CSteelMatLibrary::RecordAt(inters_tube_dlg.m_iVertPlateMat).cBriefMark;
		pTubeRibPlate->SetPartNo(inters_tube_dlg.m_sVertPlatePartNo.GetBuffer(0));
		pTubeRibPlate->ucs=pDatumTube->ucs;
		SnapPerp(&pTubeRibPlate->ucs.origin,pDatumTube->Start(),pDatumTube->End(),pBaseNode->Position(true));
		pTubeRibPlate->ucs.axis_x=cross_line_vec;
		normalize(pTubeRibPlate->ucs.axis_x);
		pTubeRibPlate->ucs.axis_z=pParamPlate1->ucs.axis_y;
		pTubeRibPlate->ucs.axis_y=pTubeRibPlate->ucs.axis_z^pTubeRibPlate->ucs.axis_x;
		normalize(pTubeRibPlate->ucs.axis_y);
		pTubeRibPlate->ucs.axis_z=pTubeRibPlate->ucs.axis_x^pTubeRibPlate->ucs.axis_y;
		normalize(pTubeRibPlate->ucs.axis_z);
		//跨越连板关联方向 wht 11-05-17
		CONNECT_DIRECTION connectVec;
		connectVec.style=0;		//焊缝走向
		connectVec.serial=1;	//关联基板的焊缝走向
		connectVec.end_space=connectVec.high_space=(short)LsSpace.EndSpace;
		connectVec.direct.norm_style=5;	//杆件特征方向
		connectVec.direct.hVicePart=pCrossTubeSeg1->handle;
		if(pCrossTubeSeg1->pStart==pBaseNode)
			connectVec.direct.direction=0;	//始->终
		else 
			connectVec.direct.direction=1;	//终->始
		pTubeRibPlate->ConnDirectList.Empty();
		pTubeRibPlate->ConnDirectList.SetValue(1,connectVec);
		//关联构件
		pTubeRibPlate->SetDesignItemValue('C',pDatumTube->handle,"",PARAM_POS,1);	//定位基准钢管
		pTubeRibPlate->SetDesignItemValue('P',pBaseNode->handle,"",PARAM_POS,1);	//定位基准节点
		pTubeRibPlate->SetDesignItemValue('E',pTransPlate1->handle,"",PARAM_POS,1);	//确定钢管肋板位置的基准钢板
		if(pTransPlate2)	//连接板不是贯通钢板时有两块基准钢板
			pTubeRibPlate->SetDesignItemValue('F',pTransPlate2->handle,"",PARAM_POS,1);	//确定钢管肋板位置的基准钢板
		//外形参数
		pTubeRibPlate->SetDesignItemValue('V',inters_tube_dlg.m_fVertPlateManuSpace,"");//加工间隙 wht 10-08-28
		pTubeRibPlate->SetDesignItemValue('D',inters_tube_dlg.m_nVertPlateWidth,"",PARAM_SHAPE);	//环形带宽
		pTubeRibPlate->SetDesignItemValue('W',fPlateLen,"",PARAM_SHAPE);	//参数W
		pTubeRibPlate->SetDesignItemValue('N',fPlateLen,"",PARAM_SHAPE);	//参数N
		pTubeRibPlate->SetDesignItemValue('H',0.5*(H-pParamPlate1->GetThick()),"",PARAM_SHAPE);	//参数H
		double Y=-1,F=-1;
		pTubeRibPlate->SetDesignItemValue('Y',Y,"");	//半圆型环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)
		pTubeRibPlate->SetDesignItemValue('G',-1,"",PARAM_SHAPE);	//自由边类型(>0直线，<=0圆弧)
		pTubeRibPlate->SetDesignItemValue('S',0,"");	//肋板的法向偏移距离	
		pTubeRibPlate->DesignPlate();	//此处主要用于计算跨越连板坐标系
		//对称生成另外一块
		CLDSParamPlate *pMirPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pMirPlate->CopyProperty(pTubeRibPlate);
		pMirPlate->ConnDirectList.Empty();
		pMirPlate->ConnDirectList.SetValue(connectVec.serial,connectVec);
		pMirPlate->layer(2)=pDatumTube->layer(2);	//调整钢管肋板的图层名
		pMirPlate->m_hPartWeldParent=pDatumTube->handle;
		pMirPlate->SetDesignItemValue('W',fPlateLen,"",PARAM_SHAPE);	//参数W
		pMirPlate->SetDesignItemValue('N',fPlateLen,"",PARAM_SHAPE);	//参数N
		pMirPlate->SetDesignItemValue('Y',-Y,"");	//半圆型环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)
		pMirPlate->ucs=pTubeRibPlate->ucs;
		pMirPlate->DesignPlate();		//此处主要用于计算跨越连板坐标系
		//引入螺栓
		CLsRefList *pLsRefList=NULL;
		for(i=0;i<2;i++)
		{
			f3dPoint axis_z;
			BOOL bStdPart=FALSE;
			if(i==0)
			{
				axis_z=pParamPlate1->ucs.axis_z;
				pLsRefList=pParamPlate1->GetLsRefList();
				bStdPart=pParamPlate1->m_bStdPart;
			}
			else
			{
				axis_z=pParamPlate2->ucs.axis_z;
				pLsRefList=pParamPlate2->GetLsRefList();
				bStdPart=pParamPlate2->m_bStdPart;
			}
			if(pLsRefList==NULL)
				continue;
			for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
			{
				if(pLsRef->start1_end2_mid0==32)	//Z正方向一侧的螺栓
				{	//引入螺栓到Z正方向一侧的连板上
					if(pTubeRibPlate->ucs.axis_y*axis_z>0)	
					{
						pTubeRibPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pTubeRibPlate->handle,TRUE);
					}
					else if(pMirPlate->ucs.axis_y*axis_z>0)
					{
						pMirPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pMirPlate->handle,TRUE);
					}
				}
				else if(pLsRef->start1_end2_mid0==42) //Z负方向一侧的螺栓
				{	//引入螺栓到Z负方向一侧的连板上
					if(pTubeRibPlate->ucs.axis_y*axis_z<0)	
					{
						pTubeRibPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pTubeRibPlate->handle,TRUE);
					}
					else if(pMirPlate->ucs.axis_y*axis_z<0)	
					{
						pMirPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pMirPlate->handle,TRUE);
					}
				}
				(*pLsRef)->CalGuigeAuto();
				(*pLsRef)->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
			}
		}
		//引入螺栓以后再设计跨越连板，需要根据螺栓计算跨越连板外形 wht 11-05-17
		pTubeRibPlate->DesignPlate();
		pTubeRibPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pTubeRibPlate->GetSolidPartObject());
		pMirPlate->DesignPlate();
		pMirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pMirPlate->GetSolidPartObject());
	}
	//五、设计贯通交叉钢管上的环向封板
	double D=inters_tube_dlg.m_nCoverPlateWidth-inters_tube_dlg.m_nCoverPlateCutWidth;
	double BH=(inters_tube_dlg.m_iHoleType==2)?0.0:-1.0;
	CLDSParamPlate::DESIGN_PARAM param;
	param.cValueType=3;
	param.value.uiAttribute=ftoi(inters_tube_dlg.m_fHoleR)*10;
	param.value.hPart=pTransPlate1->handle;
	//X轴侧上封板
	CLDSParamPlate *pCoverPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pCoverPlate->m_iParamType = TYPE_CIRCOVERPLATE;
	pCoverPlate->SetPartNo(inters_tube_dlg.m_sCoverPlatePartNo.GetBuffer(0));	//编号
	pCoverPlate->iSeg = SEGI(inters_tube_dlg.m_sSegI.GetBuffer());	//段号
	pCoverPlate->layer(2)=pDatumTube->layer(2);
	pCoverPlate->cMaterial = CSteelMatLibrary::RecordAt(inters_tube_dlg.m_iCoverPlateMat).cBriefMark;	//材质
	pCoverPlate->thick = inters_tube_dlg.m_nCoverPlateThick;
	pCoverPlate->m_hPartWeldParent = pDatumTube->handle;
	pCoverPlate->cfgword=pDatumTube->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
	pCoverPlate->SetDesignItemValue('A',inters_tube_dlg.m_fCoverPlateOffsetAngle,"");	//封板旋转角度
	pCoverPlate->SetDesignItemValue('B',inters_tube_dlg.m_fCoverPlateSectorAngle,"",PARAM_SHAPE);	//扇形夹角
	pCoverPlate->SetDesignItemValue('C',pDatumTube->handle,"",PARAM_POS,1);	//定位基准基准钢管
	pCoverPlate->SetDesignItemValue('E',pBaseNode->handle,"",PARAM_POS,1);	//定位基准节点
	pCoverPlate->SetDesignItem('F',param,"",PARAM_POS,1);	//第一块基准钢板
	pCoverPlate->SetDesignItemValue(KEY2C("BH"),BH,"",PARAM_POS,1);
	pCoverPlate->SetDesignItemValue('G',0,"",PARAM_POS,1);	//第二块基准钢板
	pCoverPlate->SetDesignItemValue('D',D,"",PARAM_SHAPE);	//环向封板参数D
	pCoverPlate->SetDesignItemValue('W',inters_tube_dlg.m_nCoverPlateWidth,"",PARAM_SHAPE);	//环向封板参数W
	pCoverPlate->SetDesignItemValue('S',(double)-1,"");	//设计上封板
	//环向封板加工间隙值 wht 10-08-28
	pCoverPlate->SetDesignItemValue('R',pDatumTube->GetDiameter()*0.5+inters_tube_dlg.m_fCoverPlateManuSpace,"",PARAM_SHAPE);
	pCoverPlate->SetDesignItemValue('M',(double)0,"");	//M为-1时表示要设计贯通连板另外一侧的封板
	pCoverPlate->DesignPlate();
	pCoverPlate->SetModified();
	pCoverPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pCoverPlate->GetSolidPartObject());
	//X轴一侧下封板
	CLDSParamPlate *pCoverPlate1=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pCoverPlate1->CopyProperty(pCoverPlate);
	pCoverPlate1->SetDesignItemValue('S',(double)1,"");	//设计下封板
	pCoverPlate1->SetDesignItemValue('M',(double)0,"");	//M为-1时表示要设计贯通连板另外一侧的封板
	pCoverPlate1->m_hPartWeldParent = pDatumTube->handle;
	pCoverPlate1->DesignPlate();
	pCoverPlate1->SetModified();
	pCoverPlate1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pCoverPlate1->GetSolidPartObject());
	//X轴负方向一侧上封板或第二块连接板上的环向封板
	if(pTransPlate2)
		param.value.hPart=pTransPlate2->handle;
	CLDSParamPlate *pCoverPlate2=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pCoverPlate2->CopyProperty(pCoverPlate);
	pCoverPlate2->SetDesignItemValue('S',(double)-1,"");	//设计上封板
	if(inters_tube_dlg.m_bTransPlate)	//贯通整连板
		pCoverPlate2->SetDesignItemValue('M',(double)-1,"");	//M为-1时表示要设计贯通连板另外一侧的封板
	else if(pTransPlate2)				//第二块连接板上的环向封板
		pCoverPlate2->SetDesignItem('F',param,"",PARAM_POS,1);	//第一块基准钢板
	pCoverPlate2->m_hPartWeldParent = pDatumTube->handle;
	pCoverPlate2->DesignPlate();
	pCoverPlate2->SetModified();
	pCoverPlate2->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pCoverPlate2->GetSolidPartObject());
	//X轴负方向一侧下封板或第二块连接板上的环向封板
	CLDSParamPlate *pCoverPlate3=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pCoverPlate3->CopyProperty(pCoverPlate);
	pCoverPlate3->SetDesignItemValue('S',(double)1,"");	//设计下封板
	if(inters_tube_dlg.m_bTransPlate)	//贯通整连板
		pCoverPlate3->SetDesignItemValue('M',(double)-1,"");	//M为-1时表示要设计贯通连板另外一侧的封板
	else if(pTransPlate2)				//第二块连接板上的环向封板
		pCoverPlate3->SetDesignItem('F',param,"",PARAM_POS,1);	//第一块基准钢板
	pCoverPlate3->m_hPartWeldParent = pDatumTube->handle;
	pCoverPlate3->DesignPlate();
	pCoverPlate3->SetModified();
	pCoverPlate3->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pCoverPlate3->GetSolidPartObject());
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	g_pSolidDraw->Draw();
	//生成环向封板上的工艺孔
	if(inters_tube_dlg.m_iHoleType==1)
	{
		if(CreateProcessHole(pCoverPlate,pTransPlate1,TRUE))
		{
			CreateProcessHole(pCoverPlate1,pTransPlate1,FALSE);
			if(inters_tube_dlg.m_bTransPlate)	
			{	//贯通整连板
				CreateProcessHole(pCoverPlate2,pTransPlate1,FALSE);
				CreateProcessHole(pCoverPlate3,pTransPlate1,FALSE);
			}
			else if(pTransPlate2)
			{
				CreateProcessHole(pCoverPlate2,pTransPlate2,FALSE);
				CreateProcessHole(pCoverPlate3,pTransPlate2,FALSE);
			}
		}
	}
	//
	m_pDoc->SetModifiedFlag();
	g_pSolidDraw->ReleaseSnapStatus();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	return 0;
}

void CLDSView::OnGyHole()
{
	m_nPrevCommandID=ID_GY_HOLE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat water hole";
#else
	m_sPrevCommandName="重复流水孔";
#endif
	Command("NewProcessHole");
}

int CLDSView::NewProcessHole()
{	//工艺孔
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	g_pSolidDraw->ReleaseSnapStatus();
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE));
	CSuperSmartPtr<CLDSPart> pHoriPlate,pVertPlate;
	CString cmdStr,errCmdStr;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("NewProcessHole Please select the plate on which process holes are needs to be arranged:");
#else
	cmdStr.Format("NewProcessHole 请选择需要布置工艺孔的钢板:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(true)
	{
		if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		pHoriPlate=console.FromPartHandle(obj.hRelaObj,CLS_PLATE,CLS_PARAMPLATE);
		if(pHoriPlate.IsNULL())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			errCmdStr.Format("NewProcessHole No proper parts selected, please reselect the plate on which process holes to be arranged:");
#else
			errCmdStr.Format("NewProcessHole 没有选中合适的构件，请重新选择需要布置工艺孔的钢板:");
#endif
			pCmdLine->FillCmdLine(errCmdStr,"");
		}
		else
			break;
	}
	pCmdLine->FinishCmdLine(CXhChar16("0X%X",pHoriPlate->handle));
	g_pSolidDraw->SetEntSnapStatus(pHoriPlate->handle);
	//选择定位钢板
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("NewProcessHole Please select positioning plate which is vertical to the plate on which process holes to be arranged:");
#else
	cmdStr.Format("NewProcessHole 请选择与布孔钢板垂直的定位钢板:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(true)
	{
		if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		pVertPlate=console.FromPartHandle(obj.hRelaObj,CLS_PLATE,CLS_PARAMPLATE);
		if(pVertPlate.IsNULL())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			errCmdStr.Format("NewProcessHole No proper parts selected, please reselect the plate on which process holes to be arranged:");
#else
			errCmdStr.Format("NewProcessHole 没有选中合适的构件，请重新与布孔钢板垂直的定位钢板:");
#endif
			pCmdLine->FillCmdLine(errCmdStr,"");
		}
		else
			break;
	}
	pCmdLine->FinishCmdLine(CXhChar16("0X%X",pVertPlate->handle));
	g_pSolidDraw->SetEntSnapStatus(pVertPlate->handle);
	//
	CUndoOperObject undo(&Ta,false);
	CreateProcessHole(pHoriPlate,pVertPlate,TRUE);
	m_pDoc->SetModifiedFlag();
	g_pSolidDraw->Draw();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	return 0;
}

void CLDSView::OnTubeBetweenPlate() 
{
	m_nPrevCommandID=ID_TUBE_BETWEEN_PLATE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat tube between-plate";
#else
	m_sPrevCommandName="重复钢管夹板";
#endif
	Command("TubeBetweenPlate");
}
int CLDSView::TubeBetweenPlate() 
{	//设计钢管夹板
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	DWORD dwhObj=0,dwExportFlag=0;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	g_pSolidDraw->ReleaseSnapStatus();
	//选择第一根基准钢管
	CLDSLineTube *pDatumTube1=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("TubeBetweenPlate please select the design datum tube for the first between-plate:","");
#else
	pCmdLine->FillCmdLine("TubeBetweenPlate 请选择第一根夹板的设计基准钢管:","");
#endif
	CSnapTypeVerify verify(OBJPROVIDER::SOLIDSPACE,GetSingleWord((SELECTINDEX_LINETUBE)));
	verify.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
	while(1)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verify)<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		pDatumTube1=(CLDSLineTube*)Ta.FromPartHandle(obj.hRelaObj,CLS_LINETUBE);
		if(pDatumTube1)
			break;
	}
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pDatumTube1->handle));
	g_pSolidDraw->SetEntSnapStatus(pDatumTube1->handle);
	//选择第二根钢管
	CLDSLineTube *pDatumTube2=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("TubeBetweenPlate please select the design datum tube for the first between-plate:","");
#else
	pCmdLine->FillCmdLine("TubeBetweenPlate 请选择第二根夹板的设计基准钢管:","");
#endif
	while(1)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verify)<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		pDatumTube2=(CLDSLineTube*)Ta.FromPartHandle(obj.hRelaObj,CLS_LINETUBE);
		if(pDatumTube2)
			break;
	}
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pDatumTube2->handle));
	g_pSolidDraw->SetEntSnapStatus(pDatumTube2->handle);
	//设计钢管夹板
	CLDSGeneralPlate* pPlate=DesignTubeBetweenPlate(pDatumTube1,pDatumTube2);
	g_pSolidDraw->ReleaseSnapStatus();
	if(pPlate)
	{
		pPlate->SetModified();
		pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		g_pSolidDraw->Draw();
		g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
	}
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	return 0;
}

BOOL CLDSView::ExtendPlateToPlane(CLDSPlate *pPlate,f3dPoint face_pos,f3dPoint face_norm,
								  f3dPoint *pExtendVec/*=NULL*/,double fExtendLen/*=NULL*/,
								  CLDSPart *pWeldFatherPart/*=NULL*/)
{
	if(pPlate->face_N>1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Sorry,don't support extend multi plane plate now!");
#else
		AfxMessageBox("很抱歉,目前暂不支持延伸多面板!");
#endif
		return FALSE;
	}
	if(pPlate->vertex_list.GetNodeNum()<3)
		return FALSE;
	CUndoOperObject undo(&Ta);
	//假定的坐标系
	UCS_STRU ucs;
	ucs.origin=face_pos;
	ucs.axis_z=face_norm;
	ucs.axis_y=inters_vec(face_norm);
	ucs.axis_x=ucs.axis_y^ucs.axis_z;
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_x);
	normalize(ucs.axis_y);
	normalize(ucs.axis_z);
	f3dPoint tm_pnt;
	double min_z=100000,sec_min_z=100000;
	PROFILE_VERTEX *pVertex=NULL,*pMinVertex=NULL,*pSecMinVertex=NULL;
	int i=0,iMinVertex=0,iSecMinVertex=0;
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		tm_pnt=pPlate->GetRealVertex(pVertex->vertex);
		coord_trans(tm_pnt,pPlate->ucs,TRUE);
		coord_trans(tm_pnt,ucs,FALSE);
		if(fabs(tm_pnt.z)<min_z)
		{
			if(pMinVertex)
			{
				pSecMinVertex=pMinVertex;
				sec_min_z=min_z;
				iSecMinVertex=iMinVertex;
			}
			pMinVertex=pVertex;
			min_z=fabs(tm_pnt.z);
			iMinVertex=i;
		}
		if(fabs(tm_pnt.z)>min_z&&fabs(tm_pnt.z)<sec_min_z)	
		{
			pSecMinVertex=pVertex;
			sec_min_z=fabs(tm_pnt.z);
			iSecMinVertex=i;
		}
		i++;
	}
	if(pExtendVec)	
	{	//指定延伸方向 取在板的相对坐标系中X较小的点延延伸方向延伸
		f3dPoint extend_vec=*pExtendVec;
		if(pSecMinVertex->vertex.x<pMinVertex->vertex.x)
		{
			pVertex=pMinVertex;
			pMinVertex=pSecMinVertex;
			pSecMinVertex=pVertex;
			i=iMinVertex;
			iMinVertex=iSecMinVertex;
			iSecMinVertex=i;
		}
		f3dPoint vertice=pMinVertex->vertex;
		coord_trans(vertice,pPlate->ucs,TRUE);
		Int3dlf(vertice,vertice,extend_vec,face_pos,face_norm);
		coord_trans(vertice,pPlate->ucs,FALSE);
		pMinVertex->vertex.Set(vertice.x,vertice.y);
		pMinVertex->m_cPosDesignType=0;	//已经改变了顶点坐标，其定位类型改为用户自定义 wjh-2015.1.27
		pPlate->designInfo.iProfileStyle0123=3;	//已经改变了顶点坐标，其定位类型改为仅更新坐标
		if(fExtendLen>0)
		{
			f3dPoint verify_vec,inters_line_vec;
			inters_line_vec=pPlate->ucs.axis_z^face_norm;
			verify_vec=pSecMinVertex->vertex-pMinVertex->vertex;
			vector_trans(verify_vec,pPlate->ucs,TRUE);
			if(verify_vec*inters_line_vec<0)
				inters_line_vec*=-1.0;
			normalize(inters_line_vec);
			vector_trans(inters_line_vec,pPlate->ucs,FALSE);
			vertice+=inters_line_vec*fExtendLen;
			pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(vertice));
			pVertex->vertex.feature=pSecMinVertex->vertex.feature;
		}
	}
	else
	{
		f3dPoint inters_line_vec=pPlate->ucs.axis_z^face_norm;
		f3dPoint extend_vec=inters_line_vec^pPlate->ucs.axis_z;
		tm_pnt=pPlate->GetRealVertex(pMinVertex->vertex);
		coord_trans(tm_pnt,pPlate->ucs,TRUE);
		Int3dlf(tm_pnt,tm_pnt,extend_vec,face_pos,face_norm);
		coord_trans(tm_pnt,pPlate->ucs,FALSE);
		pMinVertex->vertex=tm_pnt;
		pMinVertex->vertex.feature=1;
		pMinVertex->m_cPosDesignType=0;	//已经改变了顶点坐标，其定位类型改为用户自定义 wjh-2015.1.27
		
		tm_pnt=pPlate->GetRealVertex(pSecMinVertex->vertex);
		coord_trans(tm_pnt,pPlate->ucs,TRUE);
		Int3dlf(tm_pnt,tm_pnt,extend_vec,face_pos,face_norm);
		coord_trans(tm_pnt,pPlate->ucs,FALSE);
		pSecMinVertex->vertex=tm_pnt;
		pSecMinVertex->vertex.feature=1;
		pSecMinVertex->m_cPosDesignType=0;	//已经改变了顶点坐标，其定位类型改为用户自定义 wjh-2015.1.27
		pPlate->designInfo.iProfileStyle0123=3;	//已经改变了顶点坐标，其定位类型改为仅更新坐标

		if(pWeldFatherPart)
		{	//设置焊接边属性
			if(iMinVertex<iSecMinVertex)
				pMinVertex->m_bWeldEdge=TRUE;
			else
				pSecMinVertex->m_bWeldEdge=TRUE;
		}
	}
	if(pWeldFatherPart)
		pPlate->m_hPartWeldParent=pWeldFatherPart->handle;
	pPlate->SetModified();
	pPlate->DesignProfile();
	pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return TRUE;
}

void CLDSView::OnExtendPlate() 
{
	m_nPrevCommandID=ID_EXTEND_PLATE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat Extend Plate";
#else
	m_sPrevCommandName="重复延伸钢板";
#endif
	Command("ExtendPlate");
}
int CLDSView::ExtendPlate() 
{	//延伸钢板到指定构件
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr,promptStr;
	CLDSPart *pPart=NULL;
	CLDSLinePart* pDatumLinePart=NULL;
	CLDSPlate *pExtendPlate=NULL;
	CSuperSmartPtr<CLDSPart>pCutPart;
	BOOL bSpecExtendVecPart=TRUE;	//是否指定延伸方向基准杆件
	//选择需要延伸的钢板
	CSnapTypeVerify verifier;
	DWORD dwhObj=0,dwExportFlag=0;
	long *id_arr,nCount=0;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
		pExtendPlate=(CLDSPlate*)console.FromPartHandle(id_arr[0],CLS_PLATE);
	if(pExtendPlate==NULL||(pExtendPlate&&pExtendPlate->GetClassTypeId()!=CLS_PLATE))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("ExtendPlate please select a plate needs extend:","");
#else
		pCmdLine->FillCmdLine("ExtendPlate 请选择需要延伸的钢板:","");
#endif
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE));
		while(1)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				return 0;
			SELOBJ obj(dwhObj,dwExportFlag);
			pExtendPlate=(CLDSPlate*)Ta.Parts.FromHandle(obj.hRelaObj);
			if(pExtendPlate!=NULL&&pExtendPlate->GetClassTypeId()==CLS_PLATE)
				break;
		}
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pExtendPlate->handle));
		g_pSolidDraw->SetEntSnapStatus(pExtendPlate->handle);
	}
	//选择延伸方向基准杆件或延伸目标位置基准构件
	BOOL bInPutPlane=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("ExtendPlate Please select the datum rod for extending direction or datum part for extending destination position,or directly press enter key to specify extending destination plane:","");
#else
	pCmdLine->FillCmdLine("ExtendPlate 请选择延伸方向基准杆件或延伸目标位置基准构件，或者直接按回车键指定延伸目标平面:","");
#endif
	verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART|GetSingleWord(SELECTINDEX_PARAMPLATE));
	while(1)
	{
		if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			return 0;
		SELOBJ obj(dwhObj,dwExportFlag);
		pPart=(CLDSPart*)console.FromPartHandle(obj.hRelaObj);
		if(pPart&&pPart->IsLinePart())
		{	//延伸方向基准构件
			pDatumLinePart=(CLDSLinePart*)pPart;
			bSpecExtendVecPart=TRUE;
			break;
		}
		else if(pPart&&(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE))
		{	//延伸目标位置基准构件
			pCutPart=pPart;
			bSpecExtendVecPart=FALSE;
			break;
		}
		else if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
		{	//指定延伸目标平面
			bInPutPlane=TRUE;
			break;
		}
	}
	long hSelPart=0;
	if(pPart)
	{
		hSelPart=pPart->handle;
		g_pSolidDraw->SetEntSnapStatus(hSelPart);
	}
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",hSelPart));
	if(bInPutPlane)
	{	//指定延伸目标平面
		g_pSolidDraw->ReleaseSnapStatus();
		static CDefFaceDlg	dlg;
		if(dlg.DoModal()!=IDOK)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		f3dPoint face_pos(dlg.m_fBasePosX,dlg.m_fBasePosY,dlg.m_fBasePosZ);
		f3dPoint face_norm(dlg.m_fFaceNormX,dlg.m_fFaceNormY,dlg.m_fFaceNormZ);
		face_pos+=face_norm*dlg.m_fBaseNormOffsetDist;
		if(is_zero_vector(face_norm))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Normal of specified plane is null,it's invalid plane!");
#else
			AfxMessageBox("指定面法线为空,为非法面!");
#endif
			pCmdLine->CancelCmdLine();
			return 0;
		}
		ExtendPlateToPlane(pExtendPlate,face_pos,face_norm);
		Command("ExtendPlate");
		return 0;
	}
	if(bSpecExtendVecPart)
	{	//如果指定延伸方向基准杆件 则需要继续指定延伸目标基准构件
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("ExtendPlate Please select the datum part for extending destination position:","");
#else
		pCmdLine->FillCmdLine("ExtendPlate 请选择延伸目标位置基准构件:","");
#endif
		verifier.ClearSnapFlag();
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE)|
			GetSingleWord(SELECTINDEX_LINETUBE));
		while(1)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				return 0;
			SELOBJ obj(dwhObj,dwExportFlag);
			pCutPart=console.FromPartHandle(obj.hRelaObj);
			if(pCutPart.IsHasPtr()&&(pCutPart->GetClassTypeId()==CLS_PLATE||pCutPart->GetClassTypeId()==CLS_PARAMPLATE||
				pCutPart->GetClassTypeId()==CLS_LINETUBE))
				break;
		}
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pCutPart->handle));
		g_pSolidDraw->SetEntSnapStatus(pCutPart->handle);
	}
	int iFaceNo=0;
	if(pCutPart->GetClassTypeId()==CLS_PLATE&&pCutPart.PlatePointer()->face_N>1)
	{
		char cMaxFaceNo='2';
		if(pCutPart.PlatePointer()->face_N==2)
			cMaxFaceNo='2';
		else if(pCutPart.PlatePointer()->face_N==3)
			cMaxFaceNo='3';
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("ExtendPlate Please input datum plate as extending destination plane's plane number,default as datum plane<0>:","");
#else
		pCmdLine->FillCmdLine("ExtendPlate 请输入基准钢板做为延伸目的平面的面号，默认为基准面<0>:","");
#endif
		while(1)
		{
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				return 0;
			if(cmdStr.GetLength()>0)
			{
				if(cmdStr[0]>='1'&&cmdStr[0]<cMaxFaceNo)
					iFaceNo = cmdStr[0]-'0';
				else
				{
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("ExtendPlate Inputed parameter is wrong,please input datum plate as cutting plane's plane number,default as datum plane again<0>:","");
#else
					pCmdLine->FillCmdLine("ExtendPlate 输入参数有误，请重新输入基准钢板做为切割面的面号,默认为基准面<0>:","");
#endif
					continue;
				}
			}
			else
				iFaceNo=0;
			break;
		}
	}
	f3dPoint extend_vec;//=pDatumLinePart->End()-pDatumLinePart->Start();
	f3dPoint extend_face_pick=pExtendPlate->vertex_list.GetFirst()->vertex;
	f3dPoint dest_face_norm,dest_face_pick=pCutPart->ucs.origin;
	coord_trans(extend_face_pick,pExtendPlate->ucs,TRUE);
	f3dPoint verify_vec=dest_face_pick-extend_face_pick;
	if(bSpecExtendVecPart)	//指定延伸方向基准杆件
	{
		extend_vec=pDatumLinePart->End()-pDatumLinePart->Start();
		if(verify_vec*extend_vec<0)
			extend_vec*=-1.0;
	}
	else
		extend_vec=verify_vec;	//未指定延伸方向 该变量仅用来找到正确的延伸面
	extend_vec-=(extend_vec*pExtendPlate->ucs.axis_z)*pExtendPlate->ucs.axis_z;
	if(pCutPart->GetClassTypeId()==CLS_PLATE)
	{	//目标构件为多面板时 还有问题
		dest_face_norm=pCutPart->ucs.axis_z;
		if(iFaceNo==1)
			dest_face_norm=pCutPart.PlatePointer()->huoqufacenorm[0];
		else if(iFaceNo==2)
			dest_face_norm=pCutPart.PlatePointer()->huoqufacenorm[1];
		dest_face_pick=pCutPart->ucs.origin+pCutPart.PlatePointer()->m_fNormOffset*pCutPart->ucs.axis_z;
		Int3dlf(dest_face_pick,extend_face_pick,extend_vec,dest_face_pick,dest_face_norm);
		extend_vec=dest_face_pick-extend_face_pick;
		if(pCutPart->ucs.axis_z*extend_vec>0)
		{
			dest_face_norm=-dest_face_norm;
			dest_face_pick=pCutPart->ucs.origin+pCutPart.PlatePointer()->m_fNormOffset*pCutPart->ucs.axis_z;
		}
		else
			dest_face_pick=pCutPart->ucs.origin+(pCutPart.PlatePointer()->m_fNormOffset+pCutPart.PlatePointer()->GetThick())*pCutPart->ucs.axis_z;
	}
	else if(pCutPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		if(pCutPart.ParamPlatePointer()->m_iParamType==TYPE_FL||
			pCutPart.ParamPlatePointer()->m_iParamType==TYPE_FLG||
			pCutPart.ParamPlatePointer()->m_iParamType==TYPE_FLR)
		{
			Int3dlf(dest_face_pick,extend_face_pick,extend_vec,pCutPart->ucs.origin,pCutPart->ucs.axis_z);
			extend_vec=dest_face_pick-extend_face_pick;
			if(pCutPart->ucs.axis_z*extend_vec>0)
			{
				dest_face_norm=-pCutPart->ucs.axis_z;
				dest_face_pick=pCutPart->ucs.origin;
			}
			else
			{
				dest_face_norm=pCutPart->ucs.axis_z;
				dest_face_pick=pCutPart->ucs.origin+pCutPart.ParamPlatePointer()->thick*pCutPart->ucs.axis_z;
			}
		}
		else if(pCutPart.ParamPlatePointer()->m_iParamType==TYPE_UEND)
		{
			double W;
			pCutPart.ParamPlatePointer()->GetDesignItemValue('W', &W);
			Int3dlf(dest_face_pick,extend_face_pick,extend_vec,pCutPart->ucs.origin,pCutPart->ucs.axis_z);
			extend_vec=dest_face_pick-extend_face_pick;
			if(pCutPart->ucs.axis_z*extend_vec>0)
			{
				dest_face_norm=-pCutPart->ucs.axis_z;
				dest_face_pick=pCutPart->ucs.origin-(W*0.5+pCutPart.ParamPlatePointer()->thick)*pCutPart->ucs.axis_z;
			}
			else
			{
				dest_face_norm=pCutPart->ucs.axis_z;
				dest_face_pick=pCutPart->ucs.origin+(W*0.5+pCutPart.ParamPlatePointer()->thick)*pCutPart->ucs.axis_z;
			}
		}
		else
		{
			Int3dlf(dest_face_pick,extend_face_pick,extend_vec,pCutPart->ucs.origin,pCutPart->ucs.axis_z);
			extend_vec=dest_face_pick-extend_face_pick;
			if(pCutPart->ucs.axis_z*extend_vec>0)
			{
				dest_face_norm=-pCutPart->ucs.axis_z;
				dest_face_pick=pCutPart->ucs.origin-(pCutPart.ParamPlatePointer()->thick*0.5)*pCutPart->ucs.axis_z;
			}
			else
			{
				dest_face_norm=pCutPart->ucs.axis_z;
				dest_face_pick=pCutPart->ucs.origin+(pCutPart.ParamPlatePointer()->thick*0.5)*pCutPart->ucs.axis_z;
			}
		}
	}
	else if(pCutPart->GetClassTypeId()==CLS_LINETUBE)
	{
		f3dPoint verify_pick;
		SnapPerp(&verify_pick,pCutPart.LineTubePointer()->Start(),pCutPart.LineTubePointer()->End(),extend_face_pick);
		verify_vec=verify_pick-extend_face_pick;
		if(verify_vec*extend_vec<0)
			extend_vec*=-1.0;
		f3dPoint inters1,inters2,inters_vec1,inters_vec2;
		int ret=Int3dlc(&inters1,&inters2,extend_face_pick,extend_vec,
			pCutPart.LineTubePointer()->Start(),pCutPart.LineTubePointer()->End(),
			pCutPart.LineTubePointer()->GetDiameter()*0.5,pCutPart.LineTubePointer()->GetDiameter()*0.5);
		if(ret<=0)
		{
			dest_face_pick=pCutPart->ucs.origin;
		}
		else if(ret==1)
			dest_face_pick=inters1;
		else //if(ret==2)
		{
			inters_vec1=inters1-pCutPart.LinePartPointer()->Start();
			inters_vec2=inters2-pCutPart.LinePartPointer()->Start();
			if(inters_vec1*extend_vec>inters_vec2*extend_vec)
				dest_face_pick=inters2;
			else
				dest_face_pick=inters1;
		}
		f3dPoint line_vec=pCutPart.LineTubePointer()->End()-pCutPart.LineTubePointer()->Start();
		dest_face_norm=extend_vec^line_vec;
		dest_face_norm=line_vec^dest_face_norm;
		normalize(dest_face_norm);
		if(dest_face_norm*extend_vec>0)
			dest_face_norm*=-1.0;
	}
	normalize(extend_vec);

	if(bSpecExtendVecPart)
	{	//指定延伸方向基准杆件
		Ta.BeginUndoListen();
		f3dPoint vertice,verify_point,lowest_point;
		PROFILE_VERTEX *pVertex,*pLowestVertex=NULL;
		double low_level,low_h;
		for(pVertex=pExtendPlate->vertex_list.GetFirst();pVertex;pVertex=pExtendPlate->vertex_list.GetNext())
		{
			verify_point+=pVertex->vertex;
			if(pDatumLinePart->GetClassTypeId()==CLS_LINETUBE&&pExtendPlate->m_hPartWeldParent==pDatumLinePart->handle)
			{	//延伸方向基准杆件为钢管且为延伸钢板的焊接父构件 则只在与钢管相连的边上找最低点
				CLDSLineTube *pLineTube=(CLDSLineTube*)pDatumLinePart;
				if(fabs(fabs(pVertex->vertex.x)-0.5*pLineTube->GetDiameter())>EPS2+1) //适当放大范围
					continue;
			}
			if(pLowestVertex==NULL)
			{
				vertice=pVertex->vertex;
				coord_trans(vertice,pExtendPlate->ucs,TRUE);
				verify_vec=vertice-dest_face_pick;
				low_level=verify_vec*dest_face_norm;
				pLowestVertex=pVertex;
			}
			else
			{
				vertice=pVertex->vertex;
				coord_trans(vertice,pExtendPlate->ucs,TRUE);
				verify_vec=vertice-dest_face_pick;
				double dd=verify_vec*dest_face_norm;
				double dd1=verify_vec.mod();
				if(dd<low_level)
				{
					low_level=dd;
					low_h=sqrt(dd1*dd1-dd*dd);
					pLowestVertex=pVertex;
				}
				else if(fabs(dd-low_level)<EPS2)
				{	//取距离延伸目的构件相等的点中离钢板依附构件近的点
					double dd2=sqrt(dd1*dd1-dd*dd);
					if(dd2<low_h)
						pLowestVertex=pVertex;
				}
			}
		}
		if(!pLowestVertex)
		{	//防止有未考虑到的情况 以后会去掉
			for(pVertex=pExtendPlate->vertex_list.GetFirst();pVertex;pVertex=pExtendPlate->vertex_list.GetNext())
			{	
				if(pLowestVertex==NULL)
				{
					vertice=pVertex->vertex;
					coord_trans(vertice,pExtendPlate->ucs,TRUE);
					verify_vec=vertice-dest_face_pick;
					low_level=verify_vec*dest_face_norm;
					pLowestVertex=pVertex;
				}
				else
				{
					vertice=pVertex->vertex;
					coord_trans(vertice,pExtendPlate->ucs,TRUE);
					verify_vec=vertice-dest_face_pick;
					double dd=verify_vec*dest_face_norm;
					double dd1=verify_vec.mod();
					if(dd<low_level)
					{
						low_level=dd;
						low_h=sqrt(dd1*dd1-dd*dd);
						pLowestVertex=pVertex;
					}
					else if(fabs(dd-low_level)<EPS2)
					{	//取距离延伸目的构件相等的点中离钢板依附构件近的点
						double dd2=sqrt(dd1*dd1-dd*dd);
						if(dd2<low_h)
							pLowestVertex=pVertex;
					}
				}
			}
		}
		
		lowest_point=vertice=pLowestVertex->vertex;
		coord_trans(vertice,pExtendPlate->ucs,TRUE);
		Int3dlf(vertice,vertice,extend_vec,dest_face_pick,dest_face_norm);
		coord_trans(vertice,pExtendPlate->ucs,FALSE);
		pLowestVertex->vertex.Set(vertice.x,vertice.y);
		pLowestVertex->m_cPosDesignType=0;	//已经改变了顶点坐标，其定位类型改为用户自定义 wjh-2015.1.27
		pExtendPlate->designInfo.iProfileStyle0123=3;	//已经改变了顶点坐标，其定位类型改为仅更新坐标
		pExtendPlate->designInfo.additional_vertex.append(pLowestVertex->vertex);
		static double extendEdgeLen=200;
		BOOL bSpecifyDestPartEdgeLength=TRUE;
		char sText[MAX_TEMP_CHAR_100+1]="";
		if(pCutPart->GetClassTypeId()==CLS_LINETUBE)
		{
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pCutPart->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("ExtendPlate Connecting edge of tube on destination position specifies length by user(Y), or extends to tube end automatic(N)<N>:","");
#else
			pCmdLine->FillCmdLine("ExtendPlate 目标位置钢管上的连接边是由用户指定长度(Y), 还是自动延伸至钢管端(N)<N>:","");
#endif
			while(1)
			{
				if(!pCmdLine->GetStrFromCmdLine(cmdStr))
					return 0;
				if(cmdStr.GetLength()>0)
				{
					if(toupper(cmdStr[0])=='N')
						bSpecifyDestPartEdgeLength=FALSE;
					else if(toupper(cmdStr[0])=='Y')
						bSpecifyDestPartEdgeLength=TRUE;
					else
					{
#ifdef AFX_TARG_ENU_ENGLISH
						pCmdLine->FillCmdLine("ExtendPlate Inputed parameter is wrong,please confirm again Connecting edge of tube on destination position specifies length by user(Y), or extends to tube end automatic(N)<N>>:","");
#else
						pCmdLine->FillCmdLine("ExtendPlate 输入参数有误，请重新确认目标位置钢管上的连接边是由用户指定长度(Y), 还是自动延伸至钢管端(N)<N>>:","");
#endif
						continue;
					}
				}
				else
					bSpecifyDestPartEdgeLength=FALSE;
				break;
			}
			if(bSpecifyDestPartEdgeLength)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				cmdStr.Format("ExtendPlate Please input the width of connecting edge after extending to destination position<%.0f>:",extendEdgeLen);
#else
				cmdStr.Format("ExtendPlate 请输入延伸到目标位置后的连接边宽度<%.0f>:",extendEdgeLen);
#endif
				pCmdLine->FillCmdLine(cmdStr,"");
				while(1)
				{
					if(!pCmdLine->GetStrFromCmdLine(cmdStr))
						return 0;
					if(cmdStr.GetLength()>0)
					{
						_snprintf(sText,MAX_TEMP_CHAR_100,"%s",cmdStr);
						extendEdgeLen = atof(sText);
					}
					break;
				}
			}
		}
		else if(pCutPart->GetClassTypeId()==CLS_PARAMPLATE&&
			(pCutPart.ParamPlatePointer()->m_iParamType==TYPE_FL||
			pCutPart.ParamPlatePointer()->m_iParamType==TYPE_FLG||
			pCutPart.ParamPlatePointer()->m_iParamType==TYPE_FLR||
			pCutPart.ParamPlatePointer()->m_iParamType==TYPE_CIRCOVERPLATE))
		{
			double W=0,R=0,r=0;
			if(pCutPart.ParamPlatePointer()->m_iParamType==TYPE_FL||
				pCutPart.ParamPlatePointer()->m_iParamType==TYPE_FLG||
				pCutPart.ParamPlatePointer()->m_iParamType==TYPE_FLR)
			{	//法兰
				pCutPart.ParamPlatePointer()->GetDesignItemValue('W',&W);
				pCutPart.ParamPlatePointer()->GetDesignItemValue('R',&R);
				r=R;
				CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(pCutPart->m_hPartWeldParent,CLS_LINETUBE);
				if(pLineTube)
					r=pLineTube->GetDiameter()*0.5;
				extendEdgeLen=0.5*W-r;
			}
			else //if(pCutPart.ParamPlatePointer()->m_iParamType==TYPE_CIRCOVERPLATE)
			{	//环向封板
				pCutPart.ParamPlatePointer()->GetDesignItemValue('W',&W);
				extendEdgeLen=W;
			}
			CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
			ss.Format("ExtendPlate Please input the width of connecting edge after extending to destination position,default as flange width<%s>:",(char*)CXhChar16(extendEdgeLen));
#else
			ss.Format("ExtendPlate 请输入延伸到目标位置后的连接边宽度,默认为法兰宽度<%s>:",(char*)CXhChar16(extendEdgeLen));
#endif
			pCmdLine->FillCmdLine(ss,"");
			while(1)
			{
				if(!pCmdLine->GetStrFromCmdLine(cmdStr))
					return 0;
				if(cmdStr.GetLength()>0)
				{
					_snprintf(sText,MAX_TEMP_CHAR_100,"%s",cmdStr);
					extendEdgeLen = atof(sText);
				}
				break;
			}
		}
		else
		{
			if(pCutPart->GetClassTypeId()==CLS_PLATE&&pCutPart.PlatePointer()->m_fInnerRadius>0)	//环向板
				extendEdgeLen=pCutPart.PlatePointer()->GetWidth()*0.5-pCutPart.PlatePointer()->m_fInnerRadius;
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("ExtendPlate Please input the width of connecting edge after extending to destination position<%.0f>:",extendEdgeLen);
#else
			cmdStr.Format("ExtendPlate 请输入延伸到目标位置后的连接边宽度<%.0f>:",extendEdgeLen);
#endif
			pCmdLine->FillCmdLine(cmdStr,"");
			while(1)
			{
				if(!pCmdLine->GetStrFromCmdLine(cmdStr))
					return 0;
				if(cmdStr.GetLength()>0)
				{
					sprintf(sText,"%s",cmdStr);
					extendEdgeLen = atof(sText);
				}
				break;
			}
		}
		//ExtendPlateToPlane(pExtendPlate,dest_face_pick,dest_face_norm,&extend_vec,extendEdgeLen);
		if(extendEdgeLen>0)
		{
			f3dPoint inters_line_vec;
			inters_line_vec=pExtendPlate->ucs.axis_z^dest_face_norm;
			coord_trans(verify_point,pExtendPlate->ucs,TRUE);
			coord_trans(lowest_point,pExtendPlate->ucs,TRUE);
			coord_trans(vertice,pExtendPlate->ucs,TRUE);
			verify_vec=verify_point-lowest_point;
			if(verify_vec*inters_line_vec<0)
				inters_line_vec*=-1.0;
			normalize(inters_line_vec);
			double W=0;
			CLDSParamPlate *pFlPlate=NULL;
			if(!bSpecifyDestPartEdgeLength)
			{	//延伸至钢管端
				f3dPoint line_vec=pCutPart.LineTubePointer()->End()-pCutPart.LineTubePointer()->Start();
				double oddment=0;
				if(line_vec*inters_line_vec<0)
				{	//钢板延伸至钢管始端
					if(pCutPart.LineTubePointer()->m_tJointStart.hLinkObj>0x20)
						pFlPlate=(CLDSParamPlate*)console.FromPartHandle(pCutPart.LineTubePointer()->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
					if(pFlPlate&&!pFlPlate->IsFL())
						pFlPlate=NULL;
					line_vec=pCutPart.LineTubePointer()->Start()-vertice;
					oddment=pCutPart.LinePartPointer()->startOdd();
				}
				else
				{	//钢板延伸至钢管终端
					if(pCutPart.LineTubePointer()->m_tJointEnd.hLinkObj>0x20)
						pFlPlate=(CLDSParamPlate*)console.FromPartHandle(pCutPart.LineTubePointer()->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
					if(pFlPlate&&!pFlPlate->IsFL())
						pFlPlate=NULL;
					line_vec=pCutPart.LineTubePointer()->End()-vertice;
					oddment=pCutPart.LinePartPointer()->endOdd();
				}
				if(pFlPlate!=NULL)
				{
					double H;
					pFlPlate->GetDesignItemValue('H', &H);
					pFlPlate->GetDesignItemValue('W', &W);
					extendEdgeLen = line_vec*inters_line_vec-pFlPlate->thick+H+oddment;
					W=(W-pCutPart.LineTubePointer()->GetDiameter())*0.5;
				}
			}
			vertice+=inters_line_vec*extendEdgeLen;
			coord_trans(vertice,pExtendPlate->ucs,FALSE);
			PROFILE_VERTEX *pPrevVertex=NULL;
			for(pVertex=pExtendPlate->vertex_list.GetFirst();pVertex;pVertex=pExtendPlate->vertex_list.GetNext())
			{
				if(pPrevVertex==NULL)
					pPrevVertex=pVertex;
				if(pPrevVertex==pLowestVertex)
				{
					pVertex=pExtendPlate->vertex_list.insert(PROFILE_VERTEX(vertice));
					vertice.feature=pPrevVertex->vertex.feature;
					pExtendPlate->designInfo.additional_vertex.append(vertice,TRUE);
					pVertex->vertex.feature=pPrevVertex->vertex.feature;
					//if(pCutPart->GetClassTypeId()==CLS_PARAMPLATE||pCutPart->GetClassTypeId()==CLS_PLATE)
					//	pVertex->m_bWeldEdge=TRUE;
					if(pFlPlate!=NULL)
					{
						vector_trans(inters_line_vec,pExtendPlate->ucs,FALSE);
						inters_line_vec.Set(-inters_line_vec.y,inters_line_vec.x);
						vertice+=inters_line_vec*W;
						pVertex=pExtendPlate->vertex_list.insert(PROFILE_VERTEX(vertice));
						vertice.feature=pPrevVertex->vertex.feature;
						pExtendPlate->designInfo.additional_vertex.append(vertice,TRUE);
						pVertex->vertex.feature=pPrevVertex->vertex.feature;
					}
					break;
				}
			}
			if(pVertex==NULL)
			{
				pVertex=pExtendPlate->vertex_list.append(PROFILE_VERTEX(vertice));
				vertice.feature=pPrevVertex->vertex.feature;
				pExtendPlate->designInfo.additional_vertex.append(vertice,TRUE);
				pVertex->vertex.feature=pPrevVertex->vertex.feature;
				//if(pCutPart->GetClassTypeId()==CLS_PARAMPLATE||pCutPart->GetClassTypeId()==CLS_PLATE)
				//	pVertex->m_bWeldEdge=TRUE;
				if(pFlPlate!=NULL)
				{
					vector_trans(inters_line_vec,pExtendPlate->ucs,FALSE);
					inters_line_vec.Set(-inters_line_vec.y,inters_line_vec.x);
					vertice+=inters_line_vec*W;
					pVertex=pExtendPlate->vertex_list.insert(PROFILE_VERTEX(vertice));
					vertice.feature=pPrevVertex->vertex.feature;
					pExtendPlate->designInfo.additional_vertex.append(vertice,TRUE);
					pVertex->vertex.feature=pPrevVertex->vertex.feature;
				}
			}
			pExtendPlate->DesignProfile();
		}
	}
	else
		ExtendPlateToPlane(pExtendPlate,dest_face_pick,dest_face_norm,NULL,0,pCutPart);
	pExtendPlate->SetModified();
	pExtendPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pExtendPlate->GetSolidPartObject());
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->Draw();
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	ExtendPlate();	//Command("ExtendPlate");
	return 0;
}

void CLDSView::OnMergeLjb() 
{
	m_nPrevCommandID=ID_MERGE_LJB;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat merge connecting plate";
#else
	m_sPrevCommandName="重复合并连接板";
#endif
	Command("MergePlates");
}
/* 王海涛编写代码，暂时不用，将来在开发合并连接板构造同火曲线不同火曲面板式可能会用到　wjh-2012.6.30
static int CompareVertex(const PROFILE_VERTEX &vertex1,const PROFILE_VERTEX &vertex2)
{
	double angle1=Cal2dLineAng(f2dPoint(0,0),f2dPoint(vertex1.rayVecX,vertex1.rayVecY));
	double angle2=Cal2dLineAng(f2dPoint(0,0),f2dPoint(vertex2.rayVecX,vertex2.rayVecY));
	if(angle1>angle2)
		return 1;
	else if(angle1<angle2)
		return -1;
	else 
		return 0;
}
static void SortVertex(PROFILE_VERTEX *pPntArr, int nCount)
{	
	int i=0;
	f3dPoint origin;
	//求多边形顶点的几何中心点origin
	for(i=0;i<nCount;i++)
		origin+=pPntArr[i].vertex;
	origin /=nCount;
	for (i=0;i<nCount;i++)
	{
		if (fabs(pPntArr[i].rayVecX)+fabs(pPntArr[i].rayVecY)<EPS)
		{	//单位化 
			f3dPoint ray_vec=f3dPoint(pPntArr[i].vertex.x,pPntArr[i].vertex.y,0)-f3dPoint(origin.x,origin.y,0);
			normalize(ray_vec);
			pPntArr[i].rayVecX = ray_vec.x;
			pPntArr[i].rayVecY = ray_vec.y;
		}
	}
	CQuickSort<PROFILE_VERTEX>::QuickSort(pPntArr,nCount,CompareVertex);
}
*/
//获取钢板在指定延伸方向上的最高点
f3dPoint GetPlateHighestVertex(CLDSPlate *pPlate,f3dPoint spread_vec);
//获取钢板指定延伸方向上的底边
typedef PROFILE_VERTEX* VERTEX_PTR;
BOOL GetPlateBottomEdge(CLDSPlate *pPlate,f3dPoint spread_vec,f3dPoint tube_len_vec,
						f3dLine &btm_line,VERTEX_PTR *ppStartVertex,BOOL *pbReverse);
//合并钢板轮廓标至vertex_list
f3dLine AddPlateVertexsToList(ATOM_LIST<PROFILE_VERTEX> &vertex_list,f3dPoint vertex_arr[4],UCS_STRU &plate_ucs,int iFaceNo,
							  CLDSPlate *pPlate,PROFILE_VERTEX *pStartVertex,BOOL bReserveOrder);
BOOL CreateFoldPlateByMergePlates(CLDSPlate *pPlate1,CLDSPlate *pPlate2)
{
	if( pPlate1==NULL||pPlate2==NULL||pPlate1->face_N!=1||pPlate2->face_N!=1||
		pPlate1->GetLsCount()==0||pPlate2->GetLsCount()==0)
		return FALSE;
	CLDSLineTube *pTube1=(CLDSLineTube*)pPlate1->BelongModel()->FromPartHandle(pPlate1->designInfo.m_hBasePart,CLS_LINETUBE);
	CLDSLineTube *pTube2=(CLDSLineTube*)pPlate2->BelongModel()->FromPartHandle(pPlate2->designInfo.m_hBasePart,CLS_LINETUBE);
	CLDSNode *pBaseNode=(CLDSNode*)pPlate1->BelongModel()->FromNodeHandle(pPlate1->designInfo.m_hBaseNode);
	if(pBaseNode==NULL)
		pBaseNode=(CLDSNode*)pPlate2->BelongModel()->FromNodeHandle(pPlate2->designInfo.m_hBaseNode);
	if(pTube1==NULL||pTube2==NULL||pBaseNode==NULL)
		return FALSE;
	//1.获取钢板延伸方向,比较延伸方向是否一致
	//TODO:通过CutJg.cpp中的RodJunctionDiscern()处理未交于同一节点的情况 wjh-2016.8.23
	GEPOINT cut_plane_norm,cut_plane_pick=pBaseNode->Position();
	GEPOINT tube_len_vec[2]={pTube1->ucs.axis_z,pTube2->ucs.axis_z};
	if(pTube1!=pTube2)
	{	//两钢板基准构件不同时一般为端连接情况，根据连接端判断连接方向
		if(pTube1->pEnd==pBaseNode||DISTANCE(pTube1->Start(),pPlate1->ucs.origin)>DISTANCE(pTube1->End(),pPlate1->ucs.origin))
			tube_len_vec[0]=-pTube1->ucs.axis_z;
		if(pTube2->pEnd==pBaseNode||DISTANCE(pTube2->Start(),pPlate2->ucs.origin)>DISTANCE(pTube2->End(),pPlate2->ucs.origin))
			tube_len_vec[1]=-pTube2->ucs.axis_z;
	}
	else
	{
		//TODO:是不是可以用pPlate1->GetBoltIntersPos函数取代
		f3dPoint ls_pos1=(*pPlate1->GetFirstLsRef())->ucs.origin;
		project_point(ls_pos1,pPlate1->ucs.origin,pPlate1->ucs.axis_z);
		f3dPoint ls_pos2=(*pPlate2->GetFirstLsRef())->ucs.origin;
		project_point(ls_pos2,pPlate2->ucs.origin,pPlate2->ucs.axis_z);
		f3dPoint verfiy_vec=ls_pos2-ls_pos1;
		normalize(verfiy_vec);
		if(tube_len_vec[0]*verfiy_vec>0)
			tube_len_vec[0]*=-1;
		else //if(tube_len_vec[1]*verfiy_vec<0)
			tube_len_vec[1]*=-1;
	}
	//
	f3dPoint spreadVecArr[2]={pTube1->GetPlateVertSpreadVec(pPlate1),pTube2->GetPlateVertSpreadVec(pPlate2)};
	if(spreadVecArr[0].IsZero()||spreadVecArr[1].IsZero())
		return FALSE;
	UCS_STRU tube_ucs_arr[2]={pTube1->ucs,pTube1->ucs};
	if(pTube1!=pTube2&&fabs(pTube1->ucs.axis_z*pTube2->ucs.axis_z)<EPS_COS)
	{
		tube_ucs_arr[0].axis_x=tube_len_vec[0];
		tube_ucs_arr[1].axis_x=-tube_len_vec[1];
		//
		tube_ucs_arr[0].axis_z=tube_ucs_arr[1].axis_x^tube_ucs_arr[0].axis_x;
		normalize(tube_ucs_arr[0].axis_z);
		tube_ucs_arr[1].axis_z=tube_ucs_arr[0].axis_z;
		for(int i=0;i<2;i++)
		{
			tube_ucs_arr[i].axis_y=tube_ucs_arr[i].axis_z^tube_ucs_arr[i].axis_x;
			normalize(tube_ucs_arr[i].axis_y);
		}
		//计算切割面法线
		cut_plane_norm=tube_len_vec[0]+tube_len_vec[1];
		RotateVectorAroundVector(cut_plane_norm,1,0,tube_ucs_arr[0].axis_z);
	}
	else 
		cut_plane_norm=pTube1->ucs.axis_z;
	f3dPoint spreadVec1=spreadVecArr[0],spreadVec2=spreadVecArr[1];
	vector_trans(spreadVec1,tube_ucs_arr[0],FALSE);
	vector_trans(spreadVec2,tube_ucs_arr[1],FALSE);
	if(spreadVec1*spreadVec2>EPS_COS)
		return FALSE;	//两钢板共面不需要合并为折叠板
	//2.根据待合并钢板构造基板
	//2.1 与截面求交计算交点，回退一个板厚获取两个轮廓点
	f3dLine btm_line_arr[2];
	f3dPoint vertex_arr[4];
	VERTEX_PTR keyVertexPtr[2]={NULL,NULL};
	BOOL reserveOrderArr[2]={FALSE,FALSE};
	f3dPoint plate_pick_arr[2]={pPlate1->ucs.origin+(pPlate1->ucs.axis_z*(pPlate1->m_fNormOffset+pPlate1->Thick*0.5)),
								pPlate2->ucs.origin+(pPlate2->ucs.axis_z*(pPlate2->m_fNormOffset+pPlate2->Thick*0.5))};
	for(int i=0;i<2;i++)
	{
		if(!GetPlateBottomEdge((i==0)?pPlate1:pPlate2,spreadVecArr[i],tube_len_vec[i],btm_line_arr[i],&keyVertexPtr[i],&reserveOrderArr[i]))
			return FALSE;
		int nRetCode=Int3dlf(vertex_arr[i],btm_line_arr[i],cut_plane_pick,cut_plane_norm);
		vertex_arr[i]+=tube_len_vec[i]*pPlate1->Thick;
	}
	//2.2 在1、2号钢板上分别求取一条最外侧射线角钢,计算基板法线方向
	GEPOINT planeNormArr[2]={};
	GEPOINT btm_edge_vec=vertex_arr[1]-vertex_arr[0];
	normalize(btm_edge_vec);
	for(int i=0;i<2;i++)
	{
		CMaxDouble maxAngle;
		CLDSPlate *pPlate=(i==0)?pPlate1:pPlate2;
		CDesignLjPartPara *pLjPara=NULL,*pNewLjPara=NULL;
		for(pLjPara=pPlate->GetFirstLjPara();pLjPara;pLjPara=pPlate->GetNextLjPara())
		{
			CLDSLinePart *pRod=(CLDSLinePart*)pPlate->BelongModel()->FromPartHandle(pLjPara->hPart,CLS_LINEPART);
			if(pRod==NULL||pRod->handle==pPlate->designInfo.m_hBasePart)
				continue;
			pLjPara->pCurLinePart=pRod;
			f3dPoint ray_vec=(pLjPara->start0_end1==0)?(pRod->End()-pRod->Start()):(pRod->Start()-pRod->End());
			normalize(ray_vec);
			double angle=cal_angle_of_2vec(tube_len_vec[i],ray_vec);
			maxAngle.Update(angle,pLjPara);
		}
		pLjPara=(CDesignLjPartPara*)maxAngle.m_pRelaObj;
		GEPOINT wing_norm,wing_vec;
		if(pLjPara&&pLjPara->pCurLinePart&&pLjPara->pCurLinePart->IsAngle())
		{
			CLDSLineAngle *pAngle=(CLDSLineAngle*)pLjPara->pCurLinePart;
			wing_norm=(pLjPara->angle.cur_wing_x0_y1==0)?pAngle->vxWingNorm:pAngle->vyWingNorm;
			wing_vec=(pLjPara->angle.cur_wing_x0_y1==0)?pAngle->GetWingVecX():pAngle->GetWingVecY();
			GEPOINT lenVecAngle=pAngle->End()-pAngle->Start();
			planeNormArr[i]=btm_edge_vec^lenVecAngle;
			normalize(planeNormArr[i]);
			if(planeNormArr[i]*pPlate1->ucs.axis_z<0)
				planeNormArr[i]=-planeNormArr[i];
		}
		else
			return FALSE;
	}
	//2.3 计算第三、四个轮廓点
	f3dPoint plate_norm=(planeNormArr[0]+planeNormArr[1])*0.5;
	f3dPoint huoqu_vec_arr[2]={};
	for(int i=0;i<2;i++)
	{
		f3dLine line;
		CLDSPlate *pPlate=(i==0)?pPlate1:pPlate2;
		line.startPt=GetPlateHighestVertex(pPlate,spreadVecArr[i]);
		line.endPt=line.startPt+tube_len_vec[i]*1000;
		line.startPt-=tube_len_vec[i]*1000;
		huoqu_vec_arr[i]=plate_norm^pPlate->ucs.axis_z;
		normalize(huoqu_vec_arr[i]);
		Int3dll(line.startPt,line.endPt,vertex_arr[i]-huoqu_vec_arr[i]*1000,vertex_arr[i]+huoqu_vec_arr[i]*1000,vertex_arr[2+(1-i)]);
	}
	//3.构造钢板
	UCS_STRU plate_ucs;
	plate_ucs.origin=0.5*(vertex_arr[1]+vertex_arr[0]);
	plate_ucs.axis_z=plate_norm;
	plate_ucs.axis_x=btm_edge_vec;
	plate_ucs.axis_y=plate_ucs.axis_z^plate_ucs.axis_x;
	normalize(plate_ucs.axis_y);
	plate_ucs.axis_x=plate_ucs.axis_y^plate_ucs.axis_z;
	normalize(plate_ucs.axis_x);
	if(plate_ucs.axis_y*spreadVecArr[0]<0)
	{	// 保证合并之后的钢板Y轴方向与原钢板延伸方向一致
		plate_ucs.axis_y*=-1;
		plate_ucs.axis_z*=-1;
	}
	//
	ATOM_LIST<PROFILE_VERTEX> vertex_list;
	for(int i=0;i<4;i++)
	{
		coord_trans(vertex_arr[i],plate_ucs,FALSE);
		vertex_arr[i].z=0;
		vertex_arr[i].feature=(i==1||i==2)?12:13;
	}
	//
	f3dLine top_edge_line1,top_edge_line2;
	vertex_list.append((PROFILE_VERTEX(vertex_arr[0])));
	vertex_list.append((PROFILE_VERTEX(vertex_arr[1])));
	//将第二块钢板轮廓点添加至轮廓点列表中
	top_edge_line1=AddPlateVertexsToList(vertex_list,vertex_arr,plate_ucs,2,pPlate2,keyVertexPtr[1],reserveOrderArr[1]);
	PROFILE_VERTEX *pTopVertex1=vertex_list.append((PROFILE_VERTEX(vertex_arr[2])));
	PROFILE_VERTEX *pTopVertex2=vertex_list.append((PROFILE_VERTEX(vertex_arr[3])));
	//将第一块钢板轮廓点添加至轮廓点列表中
	top_edge_line2=AddPlateVertexsToList(vertex_list,vertex_arr,plate_ucs,3,pPlate1,keyVertexPtr[0],reserveOrderArr[0]);
	BOOL bDeleteLine1End=FALSE,bDeleteLine2Start=FALSE;
	f3dLine top_edge_line(top_edge_line1.startPt,top_edge_line2.endPt);
	double fLine1EndState=DistOf2dPtLine(top_edge_line1.endPt,top_edge_line.startPt,top_edge_line.endPt);
	double fLine2StartState=DistOf2dPtLine(top_edge_line2.startPt,top_edge_line.startPt,top_edge_line.endPt);
	if( fLine1EndState<0&&fLine2StartState<0)	//在顶部轮廓边右侧
	{
		top_edge_line.startPt=top_edge_line1.endPt;
		top_edge_line.endPt=top_edge_line2.startPt;
	}
	else if(fLine2StartState<0)
	{
		top_edge_line.startPt=top_edge_line1.startPt;
		top_edge_line.endPt=top_edge_line2.startPt;
		bDeleteLine1End=TRUE;
	}
	else if(fLine1EndState<0)
	{
		top_edge_line.startPt=top_edge_line1.endPt;
		top_edge_line.endPt=top_edge_line2.endPt;
		bDeleteLine2Start=TRUE;
	}
	//删除与top_edge_line1.endPt\top_edge_line2.startPt对应的轮廓点
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if( pVertex->vertex.feature<10&&	//不删除火曲点
			((bDeleteLine1End&&pVertex->vertex.IsEqual(top_edge_line1.endPt))||
			 (bDeleteLine2Start&&pVertex->vertex.IsEqual(top_edge_line2.startPt))))
			vertex_list.DeleteCursor(FALSE);
	}
	vertex_list.Clean();
	//更新火曲线终点坐标
	int nRetCode=Int3dll(top_edge_line,f3dLine(vertex_arr[0],vertex_arr[3]),vertex_arr[3]);
	if(nRetCode!=0&&nRetCode!=-1)
		pTopVertex2->vertex=vertex_arr[3];
	nRetCode=Int3dll(top_edge_line,f3dLine(vertex_arr[1],vertex_arr[2]),vertex_arr[2]);
	if(nRetCode!=0&&nRetCode!=-1)
		pTopVertex1->vertex=vertex_arr[2];

	CUndoOperObject undo(&Ta);
	//4.设置钢板参数
	//4.1 更新钢板设计参数
	pPlate1->designInfo.iFaceType=6;
	pPlate1->jdb_style=1;
	pPlate1->designInfo.m_bEnableFlexibleDesign=TRUE;
	pPlate1->designInfo.iProfileStyle0123=1;
	//射线杆件
	for(CDesignLjPartPara *pLjPara=pPlate1->GetFirstLjPara();pLjPara;pLjPara=pPlate1->GetNextLjPara())
		pLjPara->iFaceNo=3;
	for(CDesignLjPartPara *pLjPara=pPlate2->GetFirstLjPara();pLjPara;pLjPara=pPlate2->GetNextLjPara())
	{
		CDesignLjPartPara *pNewLjPara=pPlate1->designInfo.partList.SetValue(pLjPara->hPart,*pLjPara);
		pNewLjPara->iFaceNo=2;
	}
	//法线、火曲面法线、火曲线
	for(int i=0;i<2;i++)
	{	//火曲面法线
		CLDSPlate *pPlate=(i==0)?pPlate2:pPlate1;
		pPlate1->designInfo.huoqufacenorm[i]=pPlate->designInfo.norm;
		pPlate1->designInfo.huoqufacenorm[i].vector=pPlate->ucs.axis_z;
		if(pPlate2->ucs.axis_z*plate_ucs.axis_z<0)
		{
			pPlate1->designInfo.huoqufacenorm[i].vector*=-1;
			if( pPlate1->designInfo.huoqufacenorm[i].norm_style==1||
				pPlate1->designInfo.huoqufacenorm[i].norm_style==3||
				pPlate1->designInfo.huoqufacenorm[i].norm_style==5)
			{
				if(pPlate1->designInfo.huoqufacenorm[i].direction%2==0)
					pPlate1->designInfo.huoqufacenorm[i].direction+=1;
				else
					pPlate1->designInfo.huoqufacenorm[i].direction-=1;
			}
			else if(pPlate1->designInfo.norm.norm_style==2||pPlate1->designInfo.norm.norm_style==4)
				pPlate1->designInfo.huoqufacenorm[i].nearVector*=-1;
		}
		pPlate1->huoqufacenorm[i]=pPlate1->designInfo.huoqufacenorm[i].vector;
		//火曲线
		pPlate1->designInfo.huoquline_start[i].datum_pos_style=0;
		pPlate1->designInfo.huoquline_start[i].datum_pos=vertex_arr[(i*2+1)%4];
		pPlate1->designInfo.huoquline_start[i].SetPosition(vertex_arr[(i*2+1)%4]);
		pPlate1->designInfo.huoquline_end[i].datum_pos_style=0;
		pPlate1->designInfo.huoquline_end[i].datum_pos=vertex_arr[(i*2+2)%4];
		pPlate1->designInfo.huoquline_end[i].SetPosition(vertex_arr[(i*2+2)%4]);
		pPlate1->HuoQuLine[i].startPt=vertex_arr[(i*2+1)%4];
		pPlate1->HuoQuLine[i].endPt=vertex_arr[(i*2+2)%4];
	}
	pPlate1->designInfo.norm.norm_style=0;
	pPlate1->designInfo.norm.vector=plate_ucs.axis_z;
	pPlate1->designInfo.origin.datum_pos_style=0;
	pPlate1->designInfo.origin.datum_pos=plate_ucs.origin;
	pPlate1->designInfo.origin.SetPosition(plate_ucs.origin);

	//4.2 更新钢板轮廓顶点
	pPlate1->vertex_list.Empty();
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		pPlate1->vertex_list.append(*pVertex);
	//添加螺栓引用
	for(CLsRef *pLsRef=pPlate2->GetFirstLsRef();pLsRef;pLsRef=pPlate2->GetNextLsRef())
		pPlate1->AppendLsRef(*pLsRef);
	pPlate1->ucs=plate_ucs;
	pPlate1->face_N=3;
	pPlate1->jdb_style=1;	//折叠板
	pPlate1->DesignPlate();
	pPlate1->SetModified();
	pPlate1->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pPlate1->GetSolidPartObject());
	//删除第二块钢板
	g_pSolidDraw->DelEnt(pPlate2->handle);
	console.DispPartSet.DeleteNode(pPlate2->handle);
	console.DeletePart(pPlate2->handle);
	//
	g_pSolidDraw->Draw();
	return TRUE;
}
int CLDSView::MergePlates()
{	//将两块连接板合并为一块连接钢板
	CString cmdStr,promptStr;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSPlate *pFirstPlate=NULL,*pSecondPlate=NULL;
	CUndoOperObject undo(&Ta,true);
	try
	{
		//选择需要合并的第一块钢板
		long *id_arr,nCount=0;
		CLDSPart* pPart=NULL;
		CSnapTypeVerify verifier;
		DWORD dwhObj=0,dwExportFlag=0;
		if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
		{
			pPart=console.FromPartHandle(id_arr[0],CLS_PLATE,CLS_PARAMPLATE);
			if(pPart&&pPart->GetClassTypeId()==CLS_PARAMPLATE)
				pFirstPlate=ConvertParamPlateToPlate((CLDSParamPlate*)pPart);
			else
				pFirstPlate=(CLDSPlate*)pPart;
		}
		if(pFirstPlate==NULL||(pFirstPlate&&pFirstPlate->GetClassTypeId()!=CLS_PLATE))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("MergePlates Please select the first connecting plate to be combined:","");			
#else		
			pCmdLine->FillCmdLine("MergePlates 请选择需要合并的第一块连接钢板:","");			
#endif
			verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(CLS_PARAMPLATE));
			while(true)
			{
				if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
					return 0;
				SELOBJ obj(dwhObj,dwExportFlag);
				pPart=Ta.Parts.FromHandle(obj.hRelaObj);
				if(pPart && pPart->IsPlate())
					break;
			}
			pCmdLine->FinishCmdLine(CXhChar16("0x%X",pPart->handle));
			g_pSolidDraw->SetEntSnapStatus(pPart->handle);
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				pFirstPlate=ConvertParamPlateToPlate((CLDSParamPlate*)pPart);
			else if(pPart!=NULL&&pPart->GetClassTypeId()==CLS_PLATE)
				pFirstPlate=(CLDSPlate*)pPart;
			if(pFirstPlate->vertex_list.GetNodeNum()<3)
			{
				g_pSolidDraw->ReleaseSnapStatus();
				return 0;
			}
		}
		//选择需要合并的第二块钢板
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("MergePlates Please select the second connecting plate to be combined:","");
#else
		pCmdLine->FillCmdLine("MergePlates 请选择需要合并的第二块连接钢板:","");		
#endif
		while(true)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				return 0;
			SELOBJ obj(dwhObj,dwExportFlag);
			pPart=Ta.Parts.FromHandle(obj.hRelaObj);
			if(pPart && pPart->IsPart())
				break;
		}
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pPart->handle));
		g_pSolidDraw->SetEntSnapStatus(pPart->handle);
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			pSecondPlate=ConvertParamPlateToPlate((CLDSParamPlate*)pPart);
		else if(pPart->GetClassTypeId()==CLS_PLATE)
			pSecondPlate=(CLDSPlate*)pPart;
		if(pSecondPlate->vertex_list.GetNodeNum()<3)
		{
			g_pSolidDraw->ReleaseSnapStatus();
			return 0;
		}
		if(pFirstPlate->face_N>2||pSecondPlate->face_N>2)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "connecting plates of over 2 planes can not be combined";
		else if(pFirstPlate->cMaterial!=pSecondPlate->cMaterial)
			throw "the two connecting plates are of different steel material, cannot be combined";
		else if(pFirstPlate->GetThick()!=pSecondPlate->GetThick())
			throw "the two connecting plates are of different thickness, cannot be merged";
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pFirstPlate->dwPermission))
			throw "No permission to modify the connecting plates！";
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pSecondPlate->dwPermission))
			throw "No permission to modify the connecting plates！";
#else
			throw "不能合并多于两面的连接钢板";
		else if(pFirstPlate->cMaterial!=pSecondPlate->cMaterial)
			throw "两连接板的板材质不同,无法合并";
		else if(pFirstPlate->GetThick()!=pSecondPlate->GetThick())
			throw "两连接板的板厚不同,无法合并";
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pFirstPlate->dwPermission))
			throw "没有此连接板的修改权限！";
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pSecondPlate->dwPermission))
			throw "没有此连接板的修改权限！";
#endif
#if defined(__COMPLEX_PART_INC_) && defined(__LMA_FILE_)
		if(VerifyValidFunction(LICFUNC::FUNC_IDENTITY_SUBSTATION)&&CreateFoldPlateByMergePlates(pFirstPlate,pSecondPlate))
			return TRUE;
		else
#endif
			MergePlates(pFirstPlate,pSecondPlate);
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
		return 0;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	g_pSolidDraw->ReleaseSnapStatus();
	return 0;
	//MergePlates(); //Command("MergePlates");
}
CLDSPlate* CLDSView::MergePlates(CLDSPlate *pFirstPlate,CLDSPlate *pSecondPlate)
{
	int i,n;
	CLsRef *pLsRef=NULL;
	f3dPoint vertice;
	CUndoOperObject undo(&Ta,true);
	try
	{
		CDesignLjPartPara *pLjPara=NULL;
		double dd = pFirstPlate->ucs.axis_z*pSecondPlate->ucs.axis_z;
		BOOL bOverturnSecPlate=(dd<0);	//翻转第二块钢板里外面 wht 16-09-28
		if(fabs(dd)<cos(RADTODEG_COEF*0.3))	//cos0.3°= 0.999986292247  
		{	//两板基准面不共面，合并两单面板成一双面板或合并一单面板与一双面板为三面板
			//1.求两钢板的交线(即火曲线)
			f3dPoint inters_pick,inters_line_vec;
			if(pFirstPlate->face_N<pSecondPlate->face_N)	//
			{	//保证第二块板为单面板，将第一块板单（双）面板作为将来合并后板的母板原型
				CLDSPlate *pPlate=pFirstPlate;
				pFirstPlate=pSecondPlate;
				pSecondPlate=pPlate;
			}
			else if(pFirstPlate->face_N>1&&pSecondPlate->face_N>1)
#ifdef AFX_TARG_ENU_ENGLISH
				throw " two multiple-plane plates with different basic plane can not be combined,";
#else
				throw "不能同时合并两块基本面不一致的多面板,";
#endif
			if(pFirstPlate->face_N!=pSecondPlate->face_N)	//折叠板
			{
				pFirstPlate->jdb_style=1;	//之前折叠板赋值为3是错的会与钢管夹板混淆，现重新修正为1，并增加IsFoldPlate()函数 wjh-2016.1.09
				pFirstPlate->designInfo.m_bEnableFlexibleDesign=FALSE;
			}
			f3dPoint extend_vec1,extend_vec2;
			f3dPoint face_pick1=pFirstPlate->ucs.origin+pFirstPlate->ucs.axis_z*pFirstPlate->m_fNormOffset;
			f3dPoint face_pick2=pSecondPlate->ucs.origin+pSecondPlate->ucs.axis_z*pSecondPlate->m_fNormOffset;
			Int3d2f(&inters_pick,&inters_line_vec,face_pick1,pFirstPlate->ucs.axis_z,
				face_pick2,pSecondPlate->ucs.axis_z);

			f3dPoint bend_axis_vec=pFirstPlate->ucs.axis_z^pSecondPlate->ucs.axis_z;
			if(bend_axis_vec*inters_line_vec<0)
				inters_line_vec*=-1.0;
			bend_axis_vec=inters_line_vec.normalized();	//算出弯曲旋转轴方向
			PROFILE_VERTEX *pVertex,*pPrevVertex;
			//计算钢板几何中心用来判断板延伸方向 wht 10-09-28
			//face_pick1.Set(); face_pick2.Set(); 此处不能清空拾取点，下边可能继续使用
			f3dPoint geom_center1,geom_center2; 
			for(pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
			{
				f3dPoint vertex=pVertex->vertex;
				coord_trans(vertex,pFirstPlate->ucs,TRUE);
				geom_center1+=vertex;
			}
			for(pVertex=pSecondPlate->vertex_list.GetFirst();pVertex;pVertex=pSecondPlate->vertex_list.GetNext())
			{
				f3dPoint vertex=pVertex->vertex;
				coord_trans(vertex,pSecondPlate->ucs,TRUE);
				geom_center2+=vertex;
			}
			geom_center1/=pFirstPlate->vertex_list.GetNodeNum();
			geom_center2/=pSecondPlate->vertex_list.GetNodeNum();
			extend_vec1=geom_center1-inters_pick;
			extend_vec2=geom_center2-inters_pick;
			normalize(extend_vec1); //此处应该单位化，下边要用来计算cos值
			normalize(extend_vec2);
			extend_vec1-=(extend_vec1*bend_axis_vec)*bend_axis_vec;	//转换为与弯曲旋转轴垂直的板面延伸方向
			extend_vec2-=(extend_vec2*bend_axis_vec)*bend_axis_vec;	//转换为与弯曲旋转轴垂直的板面延伸方向
			normalize(extend_vec1);
			normalize(extend_vec2);
			extend_vec1*=-1.0;
			f3dPoint verify_axis_vec=extend_vec1^extend_vec2;
			extend_vec1*=-1.0;
			//
			pFirstPlate->face_N++;
			pFirstPlate->designInfo.huoqufacenorm[pFirstPlate->face_N-2]=pSecondPlate->designInfo.norm;
			if(verify_axis_vec*bend_axis_vec<0)	//两连接板里外面产生悖论，需将第二块单面板里外面反面
			{
				bOverturnSecPlate=TRUE;
				UCS_STRU ucs=pSecondPlate->ucs;
				pSecondPlate->m_fNormOffset=-pSecondPlate->m_fNormOffset-pSecondPlate->GetThick();
				ucs.axis_z*=-1.0;
				ucs.axis_x*=-1.0;
				ATOM_LIST<PROFILE_VERTEX>vertex_list;
				pPrevVertex=pSecondPlate->vertex_list.GetFirst();
				for(pVertex=pSecondPlate->vertex_list.GetTail();pVertex;pVertex=pSecondPlate->vertex_list.GetPrev())
				{
					PROFILE_VERTEX *pNewVertex=vertex_list.append();
					*pNewVertex=*pVertex;
					pNewVertex->vertex=pPrevVertex->vertex;
					pNewVertex->vertex.x*=-1.0;
					pNewVertex->vertex.z*=-1.0;
					pPrevVertex=pVertex;
				}
				pSecondPlate->vertex_list.Empty();
				for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
					pSecondPlate->vertex_list.append(*pVertex);
				pSecondPlate->ucs=ucs;
				face_pick2=pSecondPlate->ucs.origin+pSecondPlate->ucs.axis_z*pSecondPlate->m_fNormOffset;
				Int3d2f(&inters_pick,&inters_line_vec,face_pick1,pFirstPlate->ucs.axis_z,
					face_pick2,pSecondPlate->ucs.axis_z);
				pFirstPlate->designInfo.huoqufacenorm[pFirstPlate->face_N-2].ReverseDirection();
			}
			pSecondPlate->ucs.origin+=pSecondPlate->ucs.axis_z*pSecondPlate->m_fNormOffset;
			if(pFirstPlate->face_N==2)
				pFirstPlate->designInfo.iFaceType=2;
			else if(pFirstPlate->face_N==3)
				pFirstPlate->designInfo.iFaceType=4;	//折叠板
			pFirstPlate->huoqufacenorm[pFirstPlate->face_N-2]=pSecondPlate->ucs.axis_z;
			pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].startPt=inters_pick-inters_line_vec*200;
			pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].endPt=inters_pick+inters_line_vec*200;
			coord_trans(pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].startPt,pFirstPlate->ucs,FALSE);
			coord_trans(pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].endPt,pFirstPlate->ucs,FALSE);
			pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].startPt.z=0;
			pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].endPt.z=0;
			//2-4为改进的合并连接板算法 wht 12-06-30
			//2.在第一、二块板上分别查找与火曲线最近的边
			f3dLine huoqu_line(inters_pick-inters_line_vec*200,inters_pick+inters_line_vec*200);
			f3dPoint startPerp[2],endPerp[2];	//绝对坐标
			PROFILE_VERTEX *pStartVertex[2]={NULL},*pEndVertex[2]={NULL};
			for(i=0;i<2;i++)
			{
				double min_dist=10000000;
				f3dPoint extend_vec=extend_vec1;
				CLDSPlate *pPlate=pFirstPlate;
				if(i==1)
				{
					pPlate=pSecondPlate;
					extend_vec=extend_vec2;
				}
				pPrevVertex=pPlate->vertex_list.GetTail();
				for (pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
				{
					double startDist=10000000,endDist=10000000;
					f3dPoint start,end;
					f3dPoint start_vertex=pPrevVertex->vertex;
					f3dPoint end_vertex=pVertex->vertex;
					coord_trans(start_vertex,pPlate->ucs,TRUE);
					coord_trans(end_vertex,pPlate->ucs,TRUE);
					SnapPerp(&start,huoqu_line,start_vertex,&startDist);
					SnapPerp(&end,huoqu_line,end_vertex,&endDist);
					startDist=(start_vertex-start)*extend_vec;
					endDist=(end_vertex-end)*extend_vec;
					if(startDist+endDist<min_dist)
					{
						pStartVertex[i]=pPrevVertex;
						pEndVertex[i]=pVertex;
						startPerp[i]=start;
						endPerp[i]=end;
						min_dist=startDist+endDist;
					}
					pPrevVertex=pVertex;
				}
			}
			if(pStartVertex[0]==NULL||pStartVertex[1]==NULL||pEndVertex[0]==NULL||pEndVertex[1]==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "edge near the  bending edge not found, plate combination failure!";
#else
				throw "未找到离火曲线较近的边，合并钢板失败！";
#endif
			//3.计算火曲顶点坐标(查找四个投影点中两个靠外的点作为火曲点)
			f3dPoint verfiy_vec=endPerp[0]-startPerp[0];
			int sign=1;	//默认火曲线方向与基面投影边方向一致
			if(verfiy_vec*inters_line_vec<0)
				sign=-1;
			double mindist=0,maxdist=0;
			f3dPoint minPerp=startPerp[0],maxPerp=startPerp[0];
			f3dPoint perp_arr[3]={endPerp[0],startPerp[1],endPerp[1]};
			for(int ii=0;ii<3;ii++)
			{
				double dist =(perp_arr[ii]-startPerp[0])*inters_line_vec*sign;
				if(dist<mindist)
				{
					minPerp=perp_arr[ii];
					mindist=dist;
				}
				else if(dist>maxdist)
				{
					maxPerp=perp_arr[ii];
					mindist=dist;
				}
			}
			pStartVertex[0]->vertex=minPerp;
			pEndVertex[0]->vertex=maxPerp;
			coord_trans(pStartVertex[0]->vertex,pFirstPlate->ucs,FALSE);
			coord_trans(pEndVertex[0]->vertex,pFirstPlate->ucs,FALSE);
			pStartVertex[0]->vertex.z=pEndVertex[0]->vertex.z=0;
			pStartVertex[0]->vertex.feature=10+pFirstPlate->face_N;
			pEndVertex[0]->vertex.feature=10+pFirstPlate->face_N;
			//4.将第二块板的顶点添加到第一块钢板顶点列表中
			//4.1生成第二块钢板上需要保留的轮廓点有序集合(从pEndVertex[1]的下一个点开始添加)
			BOOL bStart=FALSE;
			ATOM_LIST<PROFILE_VERTEX> vertex_list;
			int n=pSecondPlate->vertex_list.GetNodeNum();
			for(i=0;;i++)
			{
				pVertex=&pSecondPlate->vertex_list[i%n];
				if(pVertex==pEndVertex[1])
				{
					bStart=TRUE;
					continue;
				}
				if(bStart)
					vertex_list.append(*pVertex);
				if(vertex_list.GetNodeNum()==n-2)
					break;
			}
			//4.2将第二块板轮廓点添加到第一块钢板中
			for (pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
			{
				if(pVertex==pStartVertex[0])
				{
					pVertex=pFirstPlate->vertex_list.GetNext();
					PROFILE_VERTEX *pTepmVertex=NULL;
					for(pTepmVertex=vertex_list.GetFirst();pTepmVertex;pTepmVertex=vertex_list.GetNext())
					{
						f3dPoint vertice=pTepmVertex->vertex;
						coord_trans(vertice,pSecondPlate->ucs,TRUE);
						coord_trans(vertice,pFirstPlate->ucs,FALSE);
						vertice=pFirstPlate->GetSpreadVertex(vertice,pFirstPlate->face_N);
						vertice.z=0;
						vertice.feature=pFirstPlate->face_N;
						PROFILE_VERTEX *pNewVertex=NULL;
						if(pVertex==NULL)	//pStartVertext[0]为最后一个轮廓点
							pNewVertex=pFirstPlate->vertex_list.append(PROFILE_VERTEX(vertice));
						else 
							pNewVertex=pFirstPlate->vertex_list.insert(PROFILE_VERTEX(vertice));
						pNewVertex->m_bWeldEdge=pTepmVertex->m_bWeldEdge;
						pNewVertex->m_bRollEdge=pTepmVertex->m_bRollEdge;
						pNewVertex->manu_space=pTepmVertex->manu_space;
						if (pNewVertex->vertex == pTepmVertex->vertex)
							pNewVertex->m_bCanCut = FALSE;
						if (pTepmVertex->type!=0)
						{
							pNewVertex->type = pTepmVertex->type;
							if(pTepmVertex->type==2||pTepmVertex->type==3)
							{	
								pNewVertex->radius = pTepmVertex->radius;
								pNewVertex->center = pTepmVertex->center;
							}
							if(pTepmVertex->type==1)
								pNewVertex->sector_angle = pTepmVertex->sector_angle;
							if(pTepmVertex->type==3)	//椭圆弧
							{
								pNewVertex->center = pTepmVertex->center;
								pNewVertex->column_norm = pTepmVertex->column_norm;
							}
						}
					}
					break;
				}
			}
			//合并连接板旧算法
			/*
			f3dPoint temp_inters_pick=inters_pick;
			coord_trans(temp_inters_pick,pFirstPlate->ucs,FALSE);
			f3dPoint temp_vec=extend_vec1;
			vector_trans(temp_vec,pFirstPlate->ucs,FALSE);
			temp_vec.z = 0;
			pPrevVertex=pFirstPlate->vertex_list.GetTail();
			for (pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
			{
				if(pPrevVertex->m_bWeldEdge||pVertex->m_bWeldEdge)
					pVertex->m_bCanCut=FALSE;
				//去掉超出火曲线之外的不合理点
				if (pVertex->type != 0)
					pVertex->m_bCanCut = FALSE;
				else if(pVertex->type == 0)
				{
					f3dPoint vec1 = pVertex->vertex - temp_inters_pick;
					vec1.z = 0;
					normalize(vec1);
					if (vec1*temp_vec<0)
						pFirstPlate->vertex_list.DeleteCursor(FALSE);
				}
			}
			pFirstPlate->vertex_list.Clean();
			temp_inters_pick=inters_pick;
			coord_trans(temp_inters_pick,pSecondPlate->ucs,FALSE);
			temp_vec=extend_vec2;
			vector_trans(temp_vec,pSecondPlate->ucs,FALSE);
			temp_vec.z = 0;
			pPrevVertex=pSecondPlate->vertex_list.GetTail();
			for(pVertex=pSecondPlate->vertex_list.GetFirst();pVertex;pVertex=pSecondPlate->vertex_list.GetNext())
			{
				if(pPrevVertex->m_bWeldEdge||pVertex->m_bWeldEdge)
					pVertex->m_bCanCut=FALSE;
				PROFILE_VERTEX *pVertexTemp;//存储圆弧终点，添加圆弧终点
				if (pVertex->type != 0)
				{
					pVertex->m_bCanCut = FALSE;
					pVertexTemp = pSecondPlate->vertex_list.GetNext();
					coord_trans(pVertexTemp->vertex,pSecondPlate->ucs,TRUE);
					coord_trans(pVertexTemp->vertex,pFirstPlate->ucs,FALSE);
					pVertexTemp->vertex.feature=pFirstPlate->face_N;
					pVertexTemp->vertex.z=0;
					pVertexTemp->m_bCanCut = FALSE;
					pFirstPlate->vertex_list.append(*pVertexTemp);
					continue;
				}
				//去掉超出火曲线之外的不合理点
				else if(pVertex->type == 0)
				{
					f3dPoint vec2 = pVertex->vertex - temp_inters_pick;
					vec2.z = 0;
					normalize(vec2);
					if (vec2*temp_vec<0)
						continue;
				}
				f3dPoint vertice=pVertex->vertex;
				f3dPoint vector(pVertex->rayVecX,pVertex->rayVecY);
				coord_trans(vertice,pSecondPlate->ucs,TRUE);
				coord_trans(vertice,pFirstPlate->ucs,FALSE);
				coord_trans(vector,pSecondPlate->ucs,TRUE);
				coord_trans(vector,pFirstPlate->ucs,FALSE);
				vertice=pFirstPlate->GetSpreadVertex(vertice,pFirstPlate->face_N);
				vector = pFirstPlate->GetSpreadVertex(vector,pFirstPlate->face_N,false);
				vertice.feature=pFirstPlate->face_N;
				vertice.z=0;
				PROFILE_VERTEX *pNewVertex = pFirstPlate->vertex_list.append(PROFILE_VERTEX(vertice));
				pNewVertex->m_bWeldEdge=pVertex->m_bWeldEdge;
				pNewVertex->m_bRollEdge=pVertex->m_bRollEdge;
				pNewVertex->manu_space=pVertex->manu_space;
				if (pNewVertex->vertex == pVertex->vertex)
					pNewVertex->m_bCanCut = FALSE;
				if (pVertex->type!=0)
				{
					pNewVertex->type = pVertex->type;
					if(pVertex->type==2||pVertex->type==3)
					{	
						pNewVertex->radius = pVertex->radius;
						pNewVertex->center = pVertex->center;
					}
					if(pVertex->type==1)
						pNewVertex->sector_angle = pVertex->sector_angle;
					if(pVertex->type==3)	//椭圆弧
					{
						pNewVertex->center = pVertex->center;
						pNewVertex->column_norm = pVertex->column_norm;
					}
				}
				pNewVertex->rayVecX = 0;//vector.x;
				pNewVertex->rayVecY = 0;//vector.y;
			}
			*/
			//重新设计参数赋值（存在问题）
			/*pFirstPlate->designInfo.huoqufacenorm[pFirstPlate->face_N-2].vector = pFirstPlate->huoqufacenorm[pFirstPlate->face_N-2];
			pFirstPlate->designInfo.huoquline_start[pFirstPlate->face_N-2].datum_pos_style = 0;
			pFirstPlate->designInfo.huoquline_start[pFirstPlate->face_N-2].pos = pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].startPt;
			pFirstPlate->designInfo.huoquline_end[pFirstPlate->face_N-2].datum_pos_style = 0;	
			pFirstPlate->designInfo.huoquline_end[pFirstPlate->face_N-2].pos = pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].endPt;
			pFirstPlate->designInfo.huoquline_start[pFirstPlate->face_N-2].UpdatePos(console.GetActiveModel());
			pFirstPlate->designInfo.huoquline_end[pFirstPlate->face_N-2].UpdatePos(console.GetActiveModel());*/
			for(pLjPara=pSecondPlate->GetFirstLjPara();pLjPara;pLjPara=pSecondPlate->GetNextLjPara())
				pLjPara->iFaceNo++;	//附加面（第二面）上所有顶点面号＋１
		}
		else if(pFirstPlate->face_N==pSecondPlate->face_N)
		{
			for(i=0;i<pFirstPlate->face_N-1;i++)
			{
				if(pFirstPlate->huoqufacenorm[i]*pSecondPlate->huoqufacenorm[i]<EPS_COS)
#ifdef AFX_TARG_ENU_ENGLISH
					throw "bending plane normal of the two connecting plates are not consistent, cannot be combined";
#else
					throw "两连接板的火曲面法线不一致,无法合并";
#endif
			}
			if(dd<0)	//两连接板里外面产生悖论，需将第二块单面板里外面反面
			{
				UCS_STRU ucs=pSecondPlate->ucs;
				pSecondPlate->m_fNormOffset=-pSecondPlate->m_fNormOffset-pSecondPlate->GetThick();
				ucs.axis_z*=-1.0;
				ucs.axis_x*=-1.0;
				ATOM_LIST<PROFILE_VERTEX>vertex_list;
				PROFILE_VERTEX *pPrevVertex=pSecondPlate->vertex_list.GetFirst();
				for(PROFILE_VERTEX *pVertex=pSecondPlate->vertex_list.GetTail();pVertex;pVertex=pSecondPlate->vertex_list.GetPrev())
				{
					PROFILE_VERTEX *pNewVertex=vertex_list.append();
					*pNewVertex=*pVertex;
					pNewVertex->vertex=pPrevVertex->vertex;
					pNewVertex->vertex.x*=-1.0;
					pNewVertex->vertex.z*=-1.0;
					pPrevVertex=pVertex;
				}
			}
			int start_huo_qu_i=0,end_huo_qu_i=0;
			f3dLine huoqu_line;
			n = pFirstPlate->vertex_list.GetNodeNum();
			for(i=0;i<n;i++)
			{
				if(pFirstPlate->vertex_list[i].vertex.feature>10)
				{
					if(start_huo_qu_i==0)
					{
						start_huo_qu_i = i;
						huoqu_line.startPt = pFirstPlate->vertex_list[i].vertex;
					}
					else if(end_huo_qu_i==0)
					{
						end_huo_qu_i = i;
						huoqu_line.endPt = pFirstPlate->vertex_list[i].vertex;
					}
					else 
#ifdef AFX_TARG_ENU_ENGLISH
						throw "three bending points found, error in data";
#else
						throw "找到了三火曲点,数据错误";
#endif
				}
			}
			for (PROFILE_VERTEX *pCurPt = pSecondPlate->vertex_list.GetFirst();pCurPt;pCurPt = pSecondPlate->vertex_list.GetNext())
			{
				vertice = pCurPt->vertex;
				coord_trans(vertice,pSecondPlate->ucs,TRUE);
				coord_trans(vertice,pFirstPlate->ucs,FALSE);
				if(vertice.feature>10)
				{
					f3dPoint perp;
					SnapPerp(&perp,huoqu_line,vertice,NULL);
					if(huoqu_line.PtInLine(perp)>0)
						continue;	//火曲点不必添加
					else
					{
						//在火曲线末点外
						if(DISTANCE(huoqu_line.startPt,perp)>
							DISTANCE(huoqu_line.endPt,perp))
						{
							perp.feature = vertice.feature;
							pFirstPlate->vertex_list.DeleteAt(end_huo_qu_i);
							pFirstPlate->vertex_list.append(perp);
							end_huo_qu_i = pFirstPlate->vertex_list.GetNodeNum()-1;
						}
						else
						{
							perp.feature = vertice.feature;
							pFirstPlate->vertex_list.DeleteAt(start_huo_qu_i);
							pFirstPlate->vertex_list.append(perp);
							start_huo_qu_i = end_huo_qu_i-1;
							end_huo_qu_i = pFirstPlate->vertex_list.GetNodeNum()-1;
						}
					}
				}
				vertice.z = 0;
				vertice.feature = pCurPt->vertex.feature;
				PROFILE_VERTEX *pPt = pFirstPlate->vertex_list.append(PROFILE_VERTEX(vertice));
				if (pCurPt->type!=0)
				{
					pPt->m_bCanCut = FALSE;
					pPt->type = pCurPt->type;
					if(pCurPt->type==2||pCurPt->type==3)
					{	
						pPt->radius = pCurPt->radius;
						pPt->center = pCurPt->center;
					}
					if(pCurPt->type==1)
						pPt->sector_angle = pCurPt->sector_angle;
					if(pCurPt->type==3)	//椭圆弧
					{
						pPt->center = pCurPt->center;
						pPt->column_norm = pCurPt->column_norm;
					}
				}
			}
			//重新设计连接板的外形
			pFirstPlate->DesignProfile();
		}
		else
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Not support the combination of singular-plane plate and double-plane plate which shares the same baisc plane!";
#else
			throw "不支持基准面相同的单面板与双面板合并!";
#endif
		//螺栓引用
		for(pLsRef=pSecondPlate->GetFirstLsRef();pLsRef;pLsRef=pSecondPlate->GetNextLsRef())
		{
			//修正第二块钢板上的螺栓通厚
			(*pLsRef)->ReplaceReferenceHandle(CHandleReplacer(pSecondPlate->handle,pFirstPlate->handle),TRUE);
			pFirstPlate->AppendLsRef(*pLsRef);
		}
		//连接杆件列表	wht 10-03-02
		CHashList<CDesignLjPartPara> hashLjPartByHandle;
		for(pLjPara=pFirstPlate->GetFirstLjPara();pLjPara;pLjPara=pFirstPlate->GetNextLjPara())
		{
			if(hashLjPartByHandle.GetValue(pLjPara->hPart)==NULL)
				hashLjPartByHandle.SetValue(pLjPara->hPart,*pLjPara);		
		}
		for(pLjPara=pSecondPlate->GetFirstLjPara();pLjPara;pLjPara=pSecondPlate->GetNextLjPara())
		{
			if(hashLjPartByHandle.GetValue(pLjPara->hPart)==NULL)	//防止重复添加同一连接杆件
				pFirstPlate->designInfo.partList.SetValue(pLjPara->hPart,*pLjPara);
		}
		pFirstPlate->GetLsRefList()->ResetBoltRefRayNo(pFirstPlate);
		pFirstPlate->SetModified();
		if(pFirstPlate->face_N==2||pFirstPlate->face_N==3)
		{	
			pFirstPlate->designInfo.huoquline_start[pFirstPlate->face_N-2].datum_pos_style=1;
			pFirstPlate->designInfo.huoquline_start[pFirstPlate->face_N-2].des_para.RODEND.hRod=pSecondPlate->designInfo.m_hBasePart;
			pFirstPlate->designInfo.huoquline_start[pFirstPlate->face_N-2].des_para.RODEND.wing_offset_style=4;
			pFirstPlate->designInfo.huoquline_start[pFirstPlate->face_N-2].des_para.RODEND.wing_offset_dist=0;
			strcpy(pFirstPlate->designInfo.huoquline_start[pFirstPlate->face_N-2].des_para.RODEND.norm_offset,"");
			CLDSLineAngle *pBaseAngle=(CLDSLineAngle*)console.FromPartHandle(pSecondPlate->designInfo.m_hBasePart,CLS_LINEANGLE);
			if(pBaseAngle&&bOverturnSecPlate&&pFirstPlate->face_N==2)
			{	//两单面板合并生成双面板时,如果第二块钢板与第一块钢板里外面不同，
				//设置火曲面拾取点时需设置法向偏移量,否则合并完成之后重新设计火曲面位置发生错位 wht 16-09-28
				double dd[2]={pBaseAngle->vxWingNorm*pSecondPlate->ucs.axis_z,
							  pBaseAngle->vyWingNorm*pSecondPlate->ucs.axis_z};
				int wing_x0_y1=pFirstPlate->designInfo.huoquline_start[pFirstPlate->face_N-2].des_para.RODEND.offset_wing=fabs(dd[0])>fabs(dd[1])?0:1;
				if(dd[wing_x0_y1]<0&&fabs(pSecondPlate->m_fNormOffset)>0)
				{
					double fNormOffset=pSecondPlate->m_fNormOffset*-1;
					if(fabs(fNormOffset-pFirstPlate->Thick)<EPS)
						sprintf(pFirstPlate->designInfo.huoquline_start[pFirstPlate->face_N-2].des_para.RODEND.norm_offset,"0x%X",pFirstPlate->handle);
					else
						sprintf(pFirstPlate->designInfo.huoquline_start[pFirstPlate->face_N-2].des_para.RODEND.norm_offset,"%.f",fNormOffset);
				}
			}
			pFirstPlate->jdb_style=0;	//通过合并钢板命令生成的板均为普通钢板(螺栓垫板合并之后jdb_style应改为0)
			if( pFirstPlate->designInfo.m_bEnableFlexibleDesign&&
				pSecondPlate->designInfo.m_bEnableFlexibleDesign&&pSecondPlate->jdb_style==0&&
				pFirstPlate->designInfo.huoquline_start[0].UpdatePos(&Ta)&&
				pFirstPlate->designInfo.partList.GetNodeNum()>0&&
				pSecondPlate->designInfo.partList.GetNodeNum()>0)
			{	//通过合并钢板命令生成的钢板默认都不启用柔性设计 wht 16-12-02
				pFirstPlate->designInfo.m_bSelfCorrectBendline=true;
				pFirstPlate->designInfo.m_bEnableFlexibleDesign=false;
				pFirstPlate->DesignPlate();
			}
		}
		pFirstPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pFirstPlate->GetSolidPartObject());
		//更正火曲线
		/*	这部分代码是很早以前写的，主要用于确定原始火曲线的两端顶点位置便于绘图，与火曲线方向修正无关，现顶点修正工作已转移至各绘制代码处，
			因此这部分代码已无意义，而且还会对合并成的折叠板带来错误（两条火曲线，易找错）　wjh-2012.6.30
		BOOL bFirst = TRUE;
		for(PROFILE_VERTEX *pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
		{
			if (pVertex->vertex.feature>10)
			{
				if (bFirst)
				{
					pFirstPlate->HuoQuLine[0].startPt = pVertex->vertex;
					bFirst = FALSE;
				}
				else
					pFirstPlate->HuoQuLine[0].endPt = pVertex->vertex;
			}
		}
		*/
		//检查是否有与要合并的钢板相关的参数化板
		CLDSParamPlate* pParamPlate=NULL;
		PARTSET param_plate_set;	//参数板集合大于零时询问用户是否重新设计相关参数板
		for(pParamPlate=(CLDSParamPlate*)Ta.Parts.GetFirst(CLS_PARAMPLATE);pParamPlate;pParamPlate=(CLDSParamPlate*)Ta.Parts.GetNext(CLS_PARAMPLATE))
		{
			long hDatumNode=0, hFirstPlate=0, hSecondPlate=0;
			if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
			{
				pParamPlate->GetDesignItemValue('B',&hFirstPlate);
				pParamPlate->GetDesignItemValue('C',&hSecondPlate);
			}
			else if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			{
				pParamPlate->GetDesignItemValue('F',&hFirstPlate);
				pParamPlate->GetDesignItemValue('G',&hSecondPlate);
			}
			else if(pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
			{
				pParamPlate->GetDesignItemValue('E',&hFirstPlate);	//第一块基准钢板由B改为E wht 11-05-13
				pParamPlate->GetDesignItemValue('F',&hSecondPlate);	//第二块基准钢板由C改为F wht 11-05-13
			}
			if((hFirstPlate>0&&hFirstPlate==pSecondPlate->handle)||(hSecondPlate>0&&hSecondPlate==pSecondPlate->handle))
				param_plate_set.append(pParamPlate);
		}
		BOOL bAdjustPlate=FALSE;
		if(param_plate_set.GetNodeNum()>0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(AfxMessageBox("Parametric plate relative to combined plates needs to be redesigned, Y(convert into normal plate manually or adjust parameters)/N (adjust parameters automatically)?",MB_YESNO)!=IDYES)
#else
			if(AfxMessageBox("与合并钢板相关的参数化板需要重新设计,Y(手动转换为普通钢板或调整参数)/N(自动调整参数)？",MB_YESNO)!=IDYES)
#endif
				bAdjustPlate=TRUE;
		}
		if(bAdjustPlate)
		{	//用第一块钢板的句柄替换第二块钢板的句柄 主要指的参数板的设计参数
			CLDSPart *pPart=NULL;
			for(pPart=param_plate_set.GetFirst();pPart;pPart=param_plate_set.GetNext())
			{
				pParamPlate=NULL;
				if(pPart&&pPart->GetClassTypeId()==CLS_PARAMPLATE)
					pParamPlate=(CLDSParamPlate*)pPart;
				if(pParamPlate==NULL)
					continue;
				long hDatumNode=0, hFirstPlate=0, hSecondPlate=0;
				if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
				{	//环向肋板
					//第一块基准钢板由B改为E 第二块基准钢板由C改为F wht 11-05-13
					pParamPlate->GetDesignItemValue('E',&hFirstPlate);
					pParamPlate->GetDesignItemValue('F',&hSecondPlate);
					if(hFirstPlate==pSecondPlate->handle)
#ifdef AFX_TARG_ENU_ENGLISH
						pParamPlate->SetDesignItemValue('E',pFirstPlate->handle,"the first positioning datum plate",PARAM_POS,1);
					if(hSecondPlate==pSecondPlate->handle)
						pParamPlate->SetDesignItemValue('F',pFirstPlate->handle,"the second positioning datum plate",PARAM_POS,1);
#else
						pParamPlate->SetDesignItemValue('E',pFirstPlate->handle,"第一块定位基准钢板",PARAM_POS,1);
					if(hSecondPlate==pSecondPlate->handle)
						pParamPlate->SetDesignItemValue('F',pFirstPlate->handle,"第二块定位基准钢板",PARAM_POS,1);
#endif
				}
				else if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				{	//环向封板
					pParamPlate->GetDesignItemValue('F',&hFirstPlate);
					pParamPlate->GetDesignItemValue('G',&hSecondPlate);
					UCS_STRU old_ucs=pParamPlate->ucs;	//记录修改参数前的坐标系
					if(hFirstPlate==pSecondPlate->handle)
#ifdef AFX_TARG_ENU_ENGLISH
						pParamPlate->SetDesignItemValue('F',pFirstPlate->handle,"the first positioning datum plate",PARAM_POS,1);
					if(hSecondPlate==pSecondPlate->handle)
						pParamPlate->SetDesignItemValue('G',pFirstPlate->handle,"the second positioning datum plate",PARAM_POS,1);
#else
						pParamPlate->SetDesignItemValue('F',pFirstPlate->handle,"第一块定位基准钢板",PARAM_POS,1);
					if(hSecondPlate==pSecondPlate->handle)
						pParamPlate->SetDesignItemValue('G',pFirstPlate->handle,"第二块定位基准钢板",PARAM_POS,1);
#endif
					/*
					//修改参数后重新设计根据新旧坐标系X轴之间的夹角调整参数
					pParamPlate->SetDesignItemValue('A',0,"封板旋转角度");
					pParamPlate->DesignPlate();
					//计算两坐标系X轴之间的夹角
					vector_trans(old_ucs.axis_x,pParamPlate->ucs,FALSE);
					double offset_angle=Cal2dLineAng(f3dPoint(0,0,0),old_ucs.axis_x)*DEGTORAD_COEF;
					if(offset_angle>180)
						offset_angle-=180;
					if(offset_angle>eps)
						pParamPlate->SetDesignItemValue('A',offset_angle,"封板旋转角度");
					*/
				}
				else if(pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
				{	//跨越式钢管连板
					//第一块基准钢板由B改为E 第二块基准钢板由C改为F wht 11-05-13
					pParamPlate->GetDesignItemValue('E',&hFirstPlate);
					pParamPlate->GetDesignItemValue('F',&hSecondPlate);
					if(hFirstPlate==pSecondPlate->handle)
					{
						if(hSecondPlate==pFirstPlate->handle)
#ifdef AFX_TARG_ENU_ENGLISH
							pParamPlate->SetDesignItemValue('E',0,"the first positioning datum plate",PARAM_POS,1);
						else
							pParamPlate->SetDesignItemValue('E',pFirstPlate->handle,"the first positioning datum plate",PARAM_POS,1);
#else
							pParamPlate->SetDesignItemValue('E',0,"第一块定位基准钢板",PARAM_POS,1);
						else
							pParamPlate->SetDesignItemValue('E',pFirstPlate->handle,"第一块定位基准钢板",PARAM_POS,1);
#endif
					}
					if(hSecondPlate==pSecondPlate->handle)
					{
						if(hFirstPlate==pFirstPlate->handle)
#ifdef AFX_TARG_ENU_ENGLISH
							pParamPlate->SetDesignItemValue('F',0,"the second  positioning datum plate",PARAM_POS,1);
						else
							pParamPlate->SetDesignItemValue('F',pFirstPlate->handle,"the second positioning datum plate",PARAM_POS,1);
#else
							pParamPlate->SetDesignItemValue('F',0,"第二块定位基准钢板",PARAM_POS,1);
						else
							pParamPlate->SetDesignItemValue('F',pFirstPlate->handle,"第二块定位基准钢板",PARAM_POS,1);
#endif
					}
				}
				g_pSolidDraw->SetEntSnapStatus(pParamPlate->handle);
				pParamPlate->DesignPlate();
				pParamPlate->SetModified();
				pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pParamPlate->GetSolidPartObject());
			}
		}
		g_pSolidDraw->Draw();
		//删除第二块钢板
		g_pSolidDraw->DelEnt(pSecondPlate->handle);
		console.DispPartSet.DeleteNode(pSecondPlate->handle);
		console.DeletePart(pSecondPlate->handle);
		return pFirstPlate;
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
		return NULL;
	}
}

void CLDSView::OnDesignSphere()
{
	m_nPrevCommandID=ID_DESIGN_SPHERE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat nodes sphere";
#else
	m_sPrevCommandName="重复节点球体";
#endif
	Command("DesignSphere");
}

int CLDSView::DesignSphere()	
{	//添加球体
	if(!PRODUCT_FUNC::IsHasColumnRayRod())//该软件授权不支持生成柱面射线杆件功能!
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The software license does not support the function to generate spherical node!");
#else
		AfxMessageBox("该软件授权不支持生成球形节点功能!");
#endif
		return 0;
	}
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	UCS_STRU ucs;
	CString cmdStr,promptStr;
	CLDSNode *pBaseNode=NULL;
	//切换到单线图状态便于选择节点
	g_pSolidSet->SetDisplayType(DISP_LINE);
	Invalidate(FALSE);
	g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select sphere's assembling locating node:","");
#else
	pCmdLine->FillCmdLine("请选择球体的装配定位节点:","");
#endif
	while(1)
	{
		f3dPoint* point;
		int ret = g_pSolidSnap->SnapPoint(point);
		if(ret<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		else if(ret>0)
		{
			pBaseNode=console.FromNodeHandle(point->ID);
			if(pBaseNode)
			{
				g_pSolidDraw->SetEntSnapStatus(point->ID);
				break;
			}
			else
			{
				pBaseNode = NULL;
				pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("Please choose sphere's assembling location node again:","");
#else
				pCmdLine->FillCmdLine("请重新选择球体的装配定位节点:","");
#endif
				g_pSolidDraw->ReleaseSnapStatus();
				continue;
			}
		}
	}
	static CDesignSphereDlg dlg;
	double t=0;
	char guige[100],mark;
	dlg.m_sSegNo=pBaseNode->iSeg.ToString();
	if(dlg.DoModal()!=IDOK)
		return 0;
	LoadDefaultUCS(&ucs);
	Ta.BeginUndoListen();
	CLDSSphere *pSphere=(CLDSSphere*)console.AppendPart(CLS_SPHERE);
	pSphere->cMaterial = CSteelMatLibrary::RecordAt(dlg.m_iSphereMat).cBriefMark;
	pSphere->ucs=ucs;
	pSphere->SetLayer(pBaseNode->layer());
	pSphere->hDatumNode=pBaseNode->handle;
	pSphere->ucs.origin=pBaseNode->Position(true);
	pSphere->iSeg=SEGI(dlg.m_sSegNo.GetBuffer()); 
	pSphere->SetPartNo(dlg.m_sPartNo.GetBuffer(0));
	_snprintf(guige,MAX_CHAR_GUIGE_16,"%s",dlg.m_sSphereGuige);
	sscanf(guige,"%lf%c%lf",&pSphere->D,&mark,&t);
	pSphere->d=pSphere->D-2*t;
	//切换到实体显示状态
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	g_pSolidDraw->ReleaseSnapStatus();
	pSphere->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pSphere->GetSolidPartObject());
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	CMirMsgDlg mir_dlg;
	if(mir_dlg.DoModal()!=IDOK)
		return 0;
	MirTaAtom(pSphere,mir_dlg.mirmsg);
	g_pSolidDraw->Draw();
	return 0;
}

void CLDSView::OnNewArcPart() 
{
	m_nPrevCommandID=ID_NEW_ARCPART;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat ring rod";
#else
	m_sPrevCommandName="重复环形杆件";
#endif
	Command("ArcPart");
}

int CLDSView::NewArcPart()
{
	CString cmdStr;
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	long retcode=0,*id_arr=NULL;
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_POINT);
	double radius=500;
	GEPOINT center,worknorm;
	CLDSNode* pStartNode=NULL,*pEndNode=NULL,*pCenterPickNode=NULL,*pPlanePickNode=NULL;
	pCmdLine->FillCmdLine("ArcPart 指定弧形杆件的起点:","");
	CLDSArcPart arcparam;
	while(pStartNode==NULL)
	{
		if((retcode=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			return FALSE;
		SELOBJ obj(dwhObj,dwExportFlag);
		dwhObj=obj.hRelaObj;
		if(dwhObj==0&&g_pSolidSnap->GetLastSelectEnts(id_arr)>0)
			dwhObj=id_arr[0];
		pStartNode=Ta.Node.FromHandle(dwhObj);
	}
	if(pStartNode)
		pCmdLine->FinishCmdLine(CXhChar16("起点<0x%X>",pStartNode->handle));
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(pStartNode->handle);
	PEEKCMDLINE cmd_detect("N",pCmdLine);
	pCmdLine->FillCmdLine("ArcPart 指定弧形杆件的终点或[平面法线(N)]:","");
	while(pEndNode==NULL)
	{
		if((retcode=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier,PEEKCMDLINE::MatchKeyword,&cmd_detect))<0)
			return FALSE;
		SELOBJ obj(dwhObj,dwExportFlag);
		dwhObj=obj.hRelaObj;
		pEndNode=Ta.Node.FromHandle(dwhObj);
		if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
			break;
	}
	if(pEndNode)
	{	//指定终点
		pCmdLine->FinishCmdLine(CXhChar16("终点<0x%X>",pEndNode->handle));
		arcparam.type=0;	//根据圆弧始末端点创建圆弧
		GEPOINT arcvec=pEndNode->Position()-pStartNode->Position();
		double halfarclen=arcvec.mod()*0.5;
		pCmdLine->FillCmdLine("ArcPart 指定弧形杆件的圆弧半径或[圆心拾取点(C)]:","");
		cmd_detect.InitKeywords("C");
		while(pCenterPickNode==NULL)
		{
			if((retcode=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier,PEEKCMDLINE::MatchKeyword,&cmd_detect))<0)
				return FALSE;
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;
			pCenterPickNode=Ta.Node.FromHandle(dwhObj);
			if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN&&pCmdLine->PeekStrFromCmdLine(cmdStr))
				break;
		}
		if(pCenterPickNode)
			pCmdLine->FinishCmdLine(CXhChar16("圆心拾取点<0x%X>",pCenterPickNode->handle));
		else
			pCmdLine->FinishCmdLine();
		//if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"C"))
		//	return FALSE;
		if(pCenterPickNode|| toupper(cmdStr[0])=='C')
		{	//指定圆心
			while(true)
			{
				if(pCenterPickNode==NULL)
				{
					pCmdLine->FillCmdLine("ArcPart 指定弧形杆件的圆心拾取点坐标:","");
					if(!pCmdLine->GetStrFromCmdLine(cmdStr))
						return FALSE;
					CXhChar200 coordstr=cmdStr;
					char* key=strtok(coordstr,",");
					center.x=(key!=NULL)?atof(key):0;
					key=strtok(NULL,",");
					center.y=(key!=NULL)?atof(key):0;
					key=strtok(NULL,",");
					center.z=(key!=NULL)?atof(key):0;
					arcparam.center=center;
				}
				else
				{
					arcparam.pCenterPickNode=pCenterPickNode;
					center=pCenterPickNode->Position();
				}
				GEPOINT vec1=center-(GEPOINT)pStartNode->Position();
				GEPOINT vec2=center-(GEPOINT)pEndNode->Position();
				double dist1=vec1.mod();
				double dist2=vec2.mod();
				if(vec1.IsZero()||vec2.IsZero())
				{
					pCmdLine->FillCmdLine("指定圆心拾取点无效,请重新指定:","");
					arcparam.pCenterPickNode=NULL;
					continue;
				}
				vec1/=dist1;
				vec2/=dist2;
				if(fabs(dist1-dist2)<1&&(arcvec*vec1)>0&&(arcvec*vec2)<0)
					arcparam.radius=ftoi(dist1);
				else if(fabs(vec1*vec2)>EPS_COS)
				{
					pCmdLine->FillCmdLine("指定圆心拾取点无效,请重新指定:","");
					arcparam.pCenterPickNode=NULL;
					continue;
				}
				worknorm=vec1^vec2;
				break;
			}
			pCmdLine->FillCmdLine((char*)CXhChar50("ArcPart 指定弧形杆件的圆弧半径<%.0f>:",arcparam.radius),"");
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				return FALSE;
			if(cmdStr.GetLength()>0)
				arcparam.radius=atof(cmdStr);
			normalize(worknorm);
			int iVecType=CStdVector::GetVectorType(worknorm);
			pCmdLine->FillCmdLine((char*)CXhChar100("弧形杆件工作平面法线[0.自定义|1.X+|2.X-|3.Y+|4.Y-|6.Z+|6.Z-] <%d>:",iVecType),"");
			if(pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"0|1|2|3|4|5|6")<0)
				return FALSE;
			arcparam.worknorm=worknorm;
			if(cmdStr.GetLength()>0&&cmdStr[0]>'0'&&cmdStr[0]<='6')
				worknorm=CStdVector::GetVector(cmdStr[0]-'0');
			if(!worknorm.IsEqual(arcparam.worknorm[0],arcparam.worknorm[1],arcparam.worknorm[2]))
			{
				arcparam.pPlanePickNode=NULL;
				arcparam.worknorm=worknorm;
			}
		}
		else
		{	//指定圆弧半径
			radius =atof(cmdStr);
			while(radius<halfarclen)
			{
				pCmdLine->FillCmdLine("ArcPart 指定弧形杆件的半径小于弦长的一半, 请重新指定:","");
				if(!pCmdLine->GetStrFromCmdLine(cmdStr))
					return FALSE;
				radius =atof(cmdStr);
			}
			arcparam.radius=radius;
			pCmdLine->FillCmdLine("ArcPart 指定弧形杆件工作平面内的任一拾取点或[平面法线(N)]:","");
			cmd_detect.InitKeywords("N");
			while(pPlanePickNode==NULL)
			{
				if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier,PEEKCMDLINE::MatchKeyword,&cmd_detect)<0)
					return FALSE;
				SELOBJ obj(dwhObj,dwExportFlag);
				dwhObj=obj.hRelaObj;
				pPlanePickNode=Ta.Node.FromHandle(dwhObj);
				if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN&&pCmdLine->PeekStrFromCmdLine(cmdStr))
					break;
			}
			arcparam.pPlanePickNode=pPlanePickNode;
			while(worknorm.IsZero())
			{
				if(pPlanePickNode==NULL)
				{
					pCmdLine->FinishCmdLine("N");
					int iVecType=CStdVector::GetVectorType(worknorm);
					pCmdLine->FillCmdLine((char*)CXhChar100("弧形杆件工作平面法线[0.自定义|1.X+|2.X-|3.Y+|4.Y-|6.Z+|6.Z-] <6>:"),"");
					if(pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"0|1|2|3|4|5|6")<0)
						return FALSE;
					arcparam.worknorm=worknorm;
					if(cmdStr.GetLength()>0&&cmdStr[0]>'0'&&cmdStr[0]<='6')
						arcparam.worknorm=worknorm=CStdVector::GetVector(cmdStr[0]-'0');
				}
				else
				{
					f3dPoint vec1=pPlanePickNode->Position()-pStartNode->Position();
					f3dPoint vec2=pPlanePickNode->Position()-pEndNode->Position();
					worknorm=vec1^vec2;
					Standarized(worknorm);
					arcparam.worknorm=worknorm;
					pCmdLine->FinishCmdLine(CXhChar16("平面拾取点<0x%X>",pPlanePickNode->handle));
				}
			}
			if(pCenterPickNode==NULL)
			{
				pCmdLine->FillCmdLine("ArcPart 指定弧形杆件的圆心拾取点<X=0,Y=0,Z=0>:","");
				while(pCenterPickNode==NULL)
				{
					if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
						return FALSE;
					SELOBJ obj(dwhObj,dwExportFlag);
					dwhObj=obj.hRelaObj;
					pCenterPickNode=Ta.Node.FromHandle(dwhObj);
					pCmdLine->PeekStrFromCmdLine(cmdStr);
					if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
						break;
				}
				if(pCenterPickNode!=NULL)
				{
					arcparam.pCenterPickNode=pCenterPickNode;
					pCmdLine->FinishCmdLine(CXhChar16("节点<0x%X>",pCenterPickNode->handle));
				}
				else
				{
					CXhChar200 coordstr=cmdStr;
					char* key=strtok(coordstr,",");
					center.x=(key!=NULL)?atof(key):0;
					key=strtok(NULL,",");
					center.y=(key!=NULL)?atof(key):0;
					key=strtok(NULL,",");
					center.z=(key!=NULL)?atof(key):0;
					arcparam.center=center;
					pCmdLine->FinishCmdLine();
				}
			}
		}
	}
	else
	{	//指定平面法线
		pCmdLine->FinishCmdLine("N");
		pCmdLine->FillCmdLine("弧形杆件工作平面法线[0.自定义|1.X+|2.X-|3.Y+|4.Y-|6.Z+|6.Z-] <6>:","");
		if(pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"0|1|2|3|4|5|6")<0)
			return FALSE;
		arcparam.worknorm=worknorm;
		if(cmdStr.GetLength()>0&&cmdStr[0]>'0'&&cmdStr[0]<='6')
			arcparam.worknorm=worknorm=CStdVector::GetVector(cmdStr[0]-'0');
		pCmdLine->FillCmdLine("ArcPart 指定弧形杆件的扇形角 <60>:","");
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
			return FALSE;
		if(cmdStr.GetLength()>0)
			arcparam.sector_angle=atof(cmdStr)*RADTODEG_COEF;
		else
			arcparam.sector_angle=Pi*2/3;
		arcparam.type=2;
	}
	static char cDefaultType;
	if(cDefaultType==0)
		cDefaultType='B';
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("ArcPart rod type[angle(L)/flat(B)/slot(C)]<%C>:",cDefaultType);
#else
	cmdStr.Format("ArcPart 杆件类型[角钢(L)/扁钢(B)/槽钢(C)]<%C>:",cDefaultType);
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"L|B|C"))
		return FALSE;
	if(cmdStr.GetLength()>0)
		cDefaultType=toupper(cmdStr[0]);
	static CDesignArcPartDlg arcPartDlg;
	if(pEndNode)
	{
		if(cDefaultType=='B')
			arcPartDlg.m_iPartType=2;
		else if(cDefaultType=='C')
			arcPartDlg.m_iPartType=1;
		else //if(cDefaultType=='L')
			arcPartDlg.m_iPartType=0;
		arcparam.pStart=pStartNode;
		arcparam.pEnd  =pEndNode;
		arcparam.DesignArcBaseLine();
		arcPartDlg.m_fStartAngle=0;
		arcPartDlg.m_fArcAngle=arcparam.baseline.SectorAngle()*DEGTORAD_COEF;
		arcPartDlg.m_fWorkNormX=arcparam.worknorm.x;
		arcPartDlg.m_fWorkNormY=arcparam.worknorm.y;
		arcPartDlg.m_fWorkNormZ=arcparam.worknorm.z;
		arcPartDlg.m_fStartPosX=pStartNode->Position('X');
		arcPartDlg.m_fStartPosY=pStartNode->Position('Y');
		arcPartDlg.m_fStartPosZ=pStartNode->Position('Z');
		arcPartDlg.m_fCenterX=arcparam.center.x;
		arcPartDlg.m_fCenterY=arcparam.center.y;
		arcPartDlg.m_fCenterZ=arcparam.center.z;
	}
	if(arcPartDlg.DoModal()!=IDOK)
		return FALSE;

	PEN_STRU pen;
	if(g_sysPara.iColorSchema==1)		//TSA配色方案
		pen=g_pSolidSet->SetPen(g_sysPara.crTsaMode.crPrimaryLine,PS_SOLID,1);
	else if(g_sysPara.iColorSchema==2)	//材质配色方案
		pen=g_pSolidSet->SetPen(g_sysPara.crMaterialMode.crLine,PS_SOLID,1);
	else								//构件配色方案
		pen=g_pSolidSet->SetPen(g_sysPara.crPartMode.crLine,PS_SOLID,1);
	CLDSArcPart *pArcPart = NULL;
	CUndoOperObject undo(&Ta,true);
	if(arcPartDlg.m_iPartType==0)
	{	//角钢
		CLDSArcAngle *pArcAngle=(CLDSArcAngle*)console.AppendPart(CLS_ARCANGLE);
		pArcPart=pArcAngle;
		restore_JG_guige(arcPartDlg.m_sPartGuiGe,pArcAngle->wing_wide,pArcAngle->wing_thick);
		pArcAngle->offset_h=arcPartDlg.m_fOffsetH;
		pArcAngle->offset_v=arcPartDlg.m_fOffsetV;
		pArcAngle->m_siLayoutStyle=(short)arcPartDlg.m_iLayoutStyle;	//摆放方式
	}
	else if(arcPartDlg.m_iPartType==1)
	{	//槽钢
		CLDSArcSlot *pArcSlot=(CLDSArcSlot*)console.AppendPart(CLS_ARCSLOT);
		pArcPart = pArcSlot;
		_snprintf(pArcSlot->spec,MAX_CHAR_GUIGE_16,"%s",arcPartDlg.m_sPartGuiGe);
		//根据规格查找槽钢
		SLOT_PARA_TYPE *pSlotPara=FindSlotType(CXhChar50(arcPartDlg.m_sPartGuiGe));
		pArcSlot->height=pSlotPara->h;	//高度
		pArcSlot->wide=pSlotPara->b;	//宽度
		pArcSlot->thick=pSlotPara->d;	//厚度
		pArcSlot->m_siLayoutStyle=(short)arcPartDlg.m_iLayoutStyle;	//摆放方式	
		pArcSlot->SetPartNo(arcPartDlg.m_sPartNo.GetBuffer(0));
		pArcSlot->iSeg=SEGI(arcPartDlg.m_sSegI.GetBuffer());
		pArcSlot->offset_h=arcPartDlg.m_fOffsetH;
		pArcSlot->offset_v=arcPartDlg.m_fOffsetV;
	}
	else if(arcPartDlg.m_iPartType==2)
	{	//扁钢
		CLDSArcFlat *pArcFlat=(CLDSArcFlat*)console.AppendPart(CLS_ARCFLAT);
		pArcPart=pArcFlat;
		restore_JG_guige(arcPartDlg.m_sPartGuiGe,pArcFlat->wide,pArcFlat->thick);
		pArcFlat->offset_h=arcPartDlg.m_fOffsetH;
		pArcFlat->offset_v=arcPartDlg.m_fOffsetV;
		pArcFlat->m_siLayoutStyle=0;	//摆放方式
	}
	/*else if(arcPartDlg.m_iPartType==3)
	{	//钢管
		
	}*/
	if(pArcPart==NULL)
		return FALSE;

	pArcPart->cMaterial=CSteelMatLibrary::RecordAt(arcPartDlg.m_iMaterial).cBriefMark;
	pArcPart->SetPartNo(arcPartDlg.m_sPartNo);
	pArcPart->iSeg=SEGI(arcPartDlg.m_sSegI);

	pArcPart->pStart=pStartNode;
	pArcPart->pEnd  =pEndNode;
	pArcPart->offset_h=arcPartDlg.m_fOffsetH;
	pArcPart->offset_v=arcPartDlg.m_fOffsetV;
	if(pEndNode==NULL)
	{	//计算中性层半径
		f3dPoint arcStart = arcPartDlg.m_arcStart.Position();
		f3dPoint arcCenter = arcPartDlg.m_center.Position();
		pArcPart->radius=DISTANCE(arcStart,arcCenter);
		double startAngle=arcPartDlg.m_fStartAngle*RADTODEG_COEF;
		pArcPart->DesignArcBaseLine(arcPartDlg.m_center,arcPartDlg.m_arcStart,arcPartDlg.m_norm,startAngle,arcPartDlg.m_fArcAngle*RADTODEG_COEF);
	}
	else
	{
		pArcPart->type=arcparam.type;
		pArcPart->radius=arcparam.radius;
		pArcPart->des_arc_norm=arcparam.des_arc_norm;
		pArcPart->center=arcparam.center;
		pArcPart->pCenterPickNode=arcparam.pCenterPickNode;
		pArcPart->pPlanePickNode=arcparam.pPlanePickNode;
		pArcPart->DesignArcBaseLine();
	}
	NewArcLinePart(pArcPart);
		
	m_pDoc->SetModifiedFlag();
	g_pSolidSet->SetPen(pen);
	g_pSolidDraw->Draw();
	return 0;
}

void CLDSView::OnRotatePartAroundAxis()
{	//刘伟说该命令主要用于塔脚倾斜肋板，不能去　wjh-2016.12.16
	m_nPrevCommandID=ID_ROTATE_PART_AROUND_AXIS;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat rotating coordinate system";
#else
	m_sPrevCommandName="重复旋转坐标系";
#endif
	Command("RotatePartAroundAxis");
}
int CLDSView::RotatePartAroundAxis()
{
	CString cmdStr;
	double fAngle = 0;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//选择需要旋转的构件
	CLDSPlate *pPlate=NULL;	//要旋转的钢板
	//用来确定旋转轴的钢板可以为参数化钢板
	CLDSPlate *pRefPlate=NULL;
	CLDSParamPlate *pRefParamPlate=NULL;	
	long *id_arr,nCount=0;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
		pPlate=(CLDSPlate*)console.FromPartHandle(id_arr[0],CLS_PLATE);
	if(pPlate==NULL||(pPlate&&pPlate->GetClassTypeId()!=CLS_PLATE))
	{	
		CSnapTypeVerify verifier;
		DWORD dwhObj=0,dwExportFlag=0;
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("RotatePartAroundAxis please choose plate to be rotated:","");
#else
		pCmdLine->FillCmdLine("RotatePartAroundAxis 请选择要进行旋转的钢板:","");
#endif
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE));
		while(true)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				return 0;
			SELOBJ obj(dwhObj,dwExportFlag);
			pPlate=(CLDSPlate*)Ta.Parts.FromHandle(obj.hRelaObj);
			if(pPlate&&pPlate->GetClassTypeId()==CLS_PLATE)
				break;
		}
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pPlate->handle));
		g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
	}
	UCS_STRU object_ucs,ucs,part_ucs;
	ucs = pPlate->ucs;
	g_pSolidSet->GetObjectUcs(object_ucs);
	coord_trans(ucs.origin,object_ucs,TRUE);
	vector_trans(ucs.axis_x,object_ucs,TRUE);
	vector_trans(ucs.axis_y,object_ucs,TRUE);
	vector_trans(ucs.axis_z,object_ucs,TRUE);
	g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
	//g_pSolidSet->SetDispWorkUcs();
	g_pSolidDraw->Draw();
	
	int nFlag = -1;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("RotatePartAroundAxis Please specify the rotating axis when rotating plate<0.axis X|1.axis Y|2.axis Z|3.Specify reference plate>[3]:","");
#else
	pCmdLine->FillCmdLine("RotatePartAroundAxis 请指定旋转钢板时的旋转轴<0.X轴|1.Y轴|2.Z轴|3.指定参考钢板>[3]:","");
#endif
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
			return 0;
		if(cmdStr.GetLength()==0)
			nFlag = 3;
		else 
			nFlag = atoi(cmdStr);
		if(nFlag<0||nFlag>3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("RotatePartAroundAxis option key words needed","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("RotatePartAroundAxis Please specify centre axis when rotating coordinate system[X/Y/Z]<X>:","");
#else
			pCmdLine->FillCmdLine("RotatePartAroundAxis 需要选项关键字","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("RotatePartAroundAxis 请指定旋转坐标系时的中心轴[X/Y/Z]<X>:","");
#endif
			continue;
		}
		else
			break;
	}
	if(nFlag==3)
	{
		CSnapTypeVerify verifier;
		DWORD dwhObj=0,dwExportFlag=0;
		CLDSPart* pPart=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("RotatePartAroundAxis Please select the plate used to determine the rotating axis:","");
#else
		pCmdLine->FillCmdLine("RotatePartAroundAxis 请选择用来确定旋转轴的钢板:","");
#endif
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE));
		while(true)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				return 0;
			SELOBJ obj(dwhObj,dwExportFlag);
			pPart=Ta.Parts.FromHandle(obj.hRelaObj);
			if(pPart && pPart->IsPlate())
				break;
		}
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pPart->handle));
		g_pSolidDraw->SetEntSnapStatus(pPart->handle);
		if(pPart->GetClassTypeId()==CLS_PLATE)
			pRefPlate=(CLDSPlate*)pPart;
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			pRefParamPlate=(CLDSParamPlate*)pPart;				
	}
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("RotatePartAroundAxis Please input rotating angle<90>:","");
#else
	pCmdLine->FillCmdLine("RotatePartAroundAxis 请输入旋转角度<90>:","");
#endif
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
			return 0;
		if(cmdStr.CompareNoCase("90")==0 || cmdStr.GetLength()==0)
		{
			fAngle=90;
			break;
		}
		if(cmdStr.GetLength()>0)
		{
			fAngle=atof(cmdStr);
			break;
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("RotatePartAroundAxis option key words needed","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("RotatePartAroundAxis Please input rotating angle<90>:","");
#else
			pCmdLine->FillCmdLine("RotatePartAroundAxis 需要选项关键字","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("RotatePartAroundAxis 请输入旋转角度<90>:","");
#endif
			continue;
		}
	}
	fAngle*=RADTODEG_COEF;
	part_ucs=pPlate->ucs;
	if(nFlag==0)
	{	//绕X轴旋转
		RotateVectorAroundVector(part_ucs.axis_y, fAngle, part_ucs.axis_x);
		normalize(part_ucs.axis_y);
		part_ucs.axis_z = part_ucs.axis_x^part_ucs.axis_y;
		normalize(part_ucs.axis_z);
	}
	else if(nFlag==1)
	{	//绕Y轴旋转
		RotateVectorAroundVector(part_ucs.axis_x, fAngle, part_ucs.axis_y);
		normalize(part_ucs.axis_x);
		part_ucs.axis_z = part_ucs.axis_x^part_ucs.axis_y;
		normalize(part_ucs.axis_z);
	}
	else if(nFlag==2)
	{	//绕Z轴旋转
		RotateVectorAroundVector(part_ucs.axis_y, fAngle, part_ucs.axis_z);
		normalize(part_ucs.axis_y);
		part_ucs.axis_x = part_ucs.axis_y^part_ucs.axis_z;
		normalize(part_ucs.axis_x);
	}
	else if(pRefParamPlate||pRefPlate)
	{	//以两钢板相交线为旋转轴
		UCS_STRU ref_ucs;
		double offset_dist=0;
		f3dPoint face1_pick,face2_pick,inters_pick,inters_line_vec;
		if(pRefParamPlate)
			ref_ucs=pRefParamPlate->ucs;
		else if(pRefPlate)
			ref_ucs=pRefPlate->ucs;
		//判断要旋转的钢板在基准钢板顶面还是底面一侧
		if(pPlate&&pPlate->vertex_list.GetNodeNum()>0)
		{
			f3dPoint vertice=pPlate->vertex_list.GetFirst()->vertex;
			coord_trans(vertice,pPlate->ucs,TRUE);
			coord_trans(vertice,ref_ucs,FALSE);
			if(vertice.z>0)
			{
				if(pRefParamPlate)
					face1_pick=ref_ucs.origin+0.5*ref_ucs.axis_z*pRefParamPlate->GetThick();
				else if(pRefPlate)
					face1_pick=ref_ucs.origin+ref_ucs.axis_z*(pRefPlate->GetThick()+pRefPlate->m_fNormOffset);
			}
			else
			{
				if(pRefParamPlate)
					face1_pick=ref_ucs.origin-0.5*ref_ucs.axis_z*pRefParamPlate->GetThick();
				else if(pRefPlate)
					face1_pick=ref_ucs.origin+ref_ucs.axis_z*pRefPlate->m_fNormOffset;
			}
		}
		face2_pick=pPlate->ucs.origin;
		if(((int)pPlate->m_fNormOffset)==0)	//此处之前好像有笔误 wjh-2019.8.26
			face2_pick=pPlate->ucs.origin+pPlate->ucs.axis_z*(0.5*pPlate->GetThick());
		int nRetCode=Int3d2f(&inters_pick,&inters_line_vec,face2_pick,
			pPlate->ucs.axis_z,face1_pick,ref_ucs.axis_z);
		if(nRetCode==1)
		{	//更正钢板坐标系,以旋转轴为X轴
			f3dPoint perp;
			f3dLine axis_line;
			axis_line.startPt=inters_pick+inters_line_vec*100;
			axis_line.endPt=inters_pick-inters_line_vec*100;
			part_ucs.axis_x=inters_line_vec;
			if(pPlate->ucs.axis_x*part_ucs.axis_x<0)
				part_ucs.axis_x*=-1.0;
			normalize(part_ucs.axis_x);
			part_ucs.axis_y=part_ucs.axis_z^part_ucs.axis_x;
			normalize(part_ucs.axis_y);
			part_ucs.axis_x=part_ucs.axis_y^part_ucs.axis_z;
			normalize(part_ucs.axis_x);
			//将坐标原点投影到相交线上
			SnapPerp(&perp,axis_line,part_ucs.origin);
			part_ucs.origin=perp;
			//调整钢板坐标系后将钢板轮廓点坐标转换到新坐标系下
			PROFILE_VERTEX *pVertex=NULL;
			for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			{
				f3dPoint vertex=pVertex->vertex;
				coord_trans(vertex,pPlate->ucs,TRUE);
				coord_trans(vertex,part_ucs,FALSE);
				pVertex->vertex=vertex;
			}
			//将调整后的坐标系赋值给钢板坐标系
			pPlate->ucs=part_ucs;
			//绕X轴旋转指定角度
			RotateVectorAroundVector(part_ucs.axis_y, fAngle, part_ucs.axis_x);
			normalize(part_ucs.axis_y);
			part_ucs.axis_z = part_ucs.axis_x^part_ucs.axis_y;
			normalize(part_ucs.axis_z);
		}
		else
		{
			g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("rotating plate failure!");
#else
			AfxMessageBox("旋转钢板失败!");
#endif
			return 0;
		}
	}
	BOOL bCreateNewPart=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("RotatePartAroundAxis generate new parts through rotating coordinate system(Y/N)<N>:","");
#else
	pCmdLine->FillCmdLine("RotatePartAroundAxis 通过旋转坐标系生成新构件(Y/N)<N>:","");
#endif
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
			return 0;
		if(cmdStr.GetLength()>0)
		{
			if(toupper(cmdStr[0])=='N')
				bCreateNewPart=FALSE;
			else if(toupper(cmdStr[0])=='Y')
				bCreateNewPart=TRUE;
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("RotatePartAroundAxis Input parameter is wrong, please confirm again whether to generate new parts by rotating coordinate system(Y/N):","");
#else
				pCmdLine->FillCmdLine("RotatePartAroundAxis 输入参数有误，请重新确认是否通过旋转坐标系生成新构件(Y/N):","");
#endif
				continue;
			}
		}
		else
			bCreateNewPart=FALSE;
		break;
	}
	Ta.BeginUndoListen();
	if(pPlate&&bCreateNewPart)
	{	//通过旋转构件坐标系生成新构件
		CLDSPlate *pNewPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
		long hNew=pNewPlate->handle;
		pPlate->CloneTo(*pNewPlate);
		pNewPlate->handle=hNew;
		pNewPlate->EmptyLsRef();		//清空螺栓引用
		pNewPlate->relativeObjs.Empty();//清空关联构件	
		pNewPlate->ucs=part_ucs;
		pNewPlate->SetModified();
		pNewPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pNewPlate->GetSolidPartObject());
	}
	else
	{
		pPlate->ucs=part_ucs;
		pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	}

	//g_pSolidSet->SetDispWorkUcs(FALSE);
	g_pSolidDraw->BatchClearCS(2);
	ucs = part_ucs;
	g_pSolidSet->GetObjectUcs(object_ucs);
	coord_trans(ucs.origin,object_ucs,TRUE);
	vector_trans(ucs.axis_x,object_ucs,TRUE);
	vector_trans(ucs.axis_y,object_ucs,TRUE);
	vector_trans(ucs.axis_z,object_ucs,TRUE);
	g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
	//g_pSolidSet->SetDispWorkUcs();
	g_sysPara.m_bDispWorkUcs = TRUE;	//更新系统设置对话框
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	RotatePartAroundAxis();	//Command("RotatePartAroundAxis");
	return 0;
}

void CLDSView::OnCutPlateByPlane() 
{
	m_nPrevCommandID=ID_CUT_PLATE_BY_PLANE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat cutting plate";
#else
	m_sPrevCommandName="重复切割钢板";
#endif
	Command("CutPlate");
}
//<DEVELOP_PROCESS_MARK nameId="CLDSView::CutPlateByPlate">
//TODO:将钢板切割钢板这一块另存一个CutPlate.cpp文件,另根据情况可集成为一个操作类CCutPlateOper
struct  CUTSECTOR_3D
{
	f3dPoint corner;
	f3dPoint start_vec;
	f3dPoint end_vec;
	f3dPoint rot_axis;
};

struct EDGE_INTERS_STATE{
	short stateVertexStart,stateVertexEnd;	//0.扇形区外;1.扇形区内;2.扇形区边界上;3.交点为扇形角点
	short intersStartEdge;	//与扇形起始边相交(有效交点),0.无交点;1.交点为内点;2.交点为直线端点;3.交点为扇形角点
	short intersEndEdge;	//与扇形终止边相交(有效交点),0.无交点;1.交点为内点;2.交点为直线端点;3.交点为扇形角点
};
class CCutSector
{
public:
	static const int LINE_RIGHT =-1;	//角点在线的右侧
	static const int ON_LINE	= 0;	//角点在线上或直线定义有误
	static const int LINE_LEFT	= 1;	//角点在线的左侧
public:
	f2dPoint corner,edgeVecStart,edgeVecEnd;	//角点，起始方向，终止方向
	EDGE_INTERS_STATE CalEdgeIntersState(f2dPoint lineStart,f2dPoint lineEnd,f2dPoint& inters1,f2dPoint& inters2);
	//返回LINE_LEFT或LINE_RIGHT、ON_LINE
	int JustifyConerReferToLine(f2dPoint lineStart,f2dPoint lineEnd);
	//判断空间点是否扇形有效区域内,0.在扇形区域外;1.在扇形区域内;2.在扇形边界上;3.在扇形角点上
	int PositionType(f2dPoint vertex);
};
EDGE_INTERS_STATE CCutSector::CalEdgeIntersState(f2dPoint lineStart,f2dPoint lineEnd,f2dPoint& inters1,f2dPoint& inters2)
{
	f2dRay start_ray,end_ray;
	start_ray.basePt=end_ray.basePt=corner;
	start_ray.vec=edgeVecStart;
	end_ray.vec=edgeVecEnd;

	EDGE_INTERS_STATE state;
	state.stateVertexStart=PositionType(lineStart);
	state.stateVertexEnd=PositionType(lineEnd);
	state.intersEndEdge;
	int result=Int2dlr(f2dLine(lineStart,lineEnd),start_ray,inters1.x,inters1.y);
	if(result>0)
		state.intersStartEdge=result;
	else
		state.intersStartEdge=0;
	result=Int2dlr(f2dLine(lineStart,lineEnd),end_ray,inters2.x,inters2.y);
	if(result>0)
		state.intersEndEdge=result;
	else
		state.intersEndEdge=0;
	//统一直线端点在角点情况或角点在直线段内情况
	if(state.stateVertexStart==3||state.intersEndEdge==3)
		state.intersStartEdge=3;
	if(state.stateVertexEnd==3||state.intersStartEdge==3)
		state.intersEndEdge=3;
	return state;
}
int CCutSector::JustifyConerReferToLine(f2dPoint lineStart,f2dPoint lineEnd)
{
	double  iRet=DistOf2dPtLine(corner,lineStart,lineEnd);
	if(iRet>0)
		return CCutSector::LINE_LEFT;
	else if(iRet<0)
		return CCutSector::LINE_RIGHT;
	else
		return CCutSector::ON_LINE;
}
//顶点是否完全落在扇形区域内（不含在扇形边界上顶点）,0.在扇形区域外;1.在扇形区域内;2.在扇形边界上;3.在扇形角点上
int CCutSector::PositionType(f2dPoint vertex)
{
	UCS_2dSTRU ucs;
	ucs.origin=corner;
	ucs.axis_x.Set(edgeVecStart.x,edgeVecStart.y);
	ucs.axis_y.Set(-edgeVecStart.y,edgeVecStart.x);
	f2dPoint endPos=edgeVecEnd;
	vector_trans(endPos,ucs,FALSE);
	double max_angle=Cal2dLineAng(0,0,endPos.x,endPos.y)*DEGTORAD_COEF;
	coord_trans(vertex,ucs,FALSE);
	double angle=Cal2dLineAng(0,0,vertex.x,vertex.y)*DEGTORAD_COEF;
	if(angle==-1)
		return 3;	//在扇形角点上
	if(angle<-EPS||angle>max_angle+EPS)
		return 0;	//不在扇形区域内
	else if(angle>EPS&&angle<max_angle-EPS)
		return 1;	//确定完全在扇形区域内（不含边界）
	else
		return 2;	//在扇形边界上
	return TRUE;
}
//通过切割钢板初始化三维切割扇形（角点，初始方向，旋转方向，旋转角度）
static void Init3DCutSectorByPlate(CUTSECTOR_3D& sector_3d,CLDSPlate* pCutPlate,BOOL bTopFace,double fMinDist);
//根据三维扇形区域与单面板所在面相交确定二维扇形区域（角点，起始方向，终止方向）
static void InitCutSectorBy3DSector(CCutSector& cutSector,CUTSECTOR_3D sector_3d,UCS_STRU ucs);
//通过二维扇形区域切割单面板
static int CutPlateBySector(CLDSPlate* pPlate,CCutSector cutSector,BOOL bPrompt/* =TRUE */);
//将双面板拆分成两个单面板
static BOOL Split2PlanePlate(CLDSPlate* pPlate,CLDSPlate *plate_arr);
//合并两单面为双面板
static BOOL Combine2PlanePlate(CLDSPlate* pPlate,CLDSPlate* plate_arr);
//通过指定双面板切割钢板
BOOL CLDSView::CutPlateByPlate(CLDSPlate *pPlate,CLDSPlate *pDatumPlate,
								 BOOL bTopFace,double fMinDist,BOOL bPrompt/* =TRUE */,BOOL bSetWeldEdge/*=TRUE*/)
{
	if(pPlate->face_N==3)
	{
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Sorry, three-plane plate can not be cut temporarily!");
#else
			AfxMessageBox("很抱歉,目前暂不能分割三面板!");
#endif
		return FALSE;
	}
	if(pDatumPlate->face_N==3)
	{
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Sorry, three-plane plate is not supported temporarily to be cut into plates!");
#else
			AfxMessageBox("很抱歉,目前暂不支持使用三面板切割钢板!");
#endif
		return FALSE;
	}
	if(pPlate->vertex_list.GetNodeNum()<3||pDatumPlate->vertex_list.GetNodeNum()<3)
		return FALSE;
	if(pDatumPlate->face_N==1)	//基准钢板为单面板时直接调用通过平面切割钢板函数即可
	{
		f3dPoint face_pick,face_norm;
		face_norm=pDatumPlate->ucs.axis_z;
		face_pick=pDatumPlate->ucs.origin+face_norm*pDatumPlate->m_fNormOffset;
		if(bTopFace)
		{
			face_norm=-pDatumPlate->ucs.axis_z;
			face_pick-=face_norm*pDatumPlate->GetThick();
		}
		return CutPlateByPlane(pPlate,face_pick,face_norm,fMinDist,bPrompt,bSetWeldEdge);
	}
	CUTSECTOR_3D sector_3d;
	Init3DCutSectorByPlate(sector_3d,pDatumPlate,bTopFace,fMinDist);	//根据切割双面板确定三维切割扇形
	if(pPlate->face_N==1)	//被切割板pPlate是单面板	
	{
		CCutSector cutSector;
		InitCutSectorBy3DSector(cutSector,sector_3d,pPlate->ucs);	//根据三维扇形和单面板所在面确定切割二维扇形
		if(CutPlateBySector(pPlate,cutSector,bPrompt)<2)	//通过扇形区域切割单面板
			return FALSE;
		pPlate->SetModified();
		pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		g_pSolidDraw->Draw();
		return TRUE;
	}
	//被切割板是双面板,将双面板pPlate分成两个单面板进行处理
	CLDSPlate plate_arr[2];
	if(!Split2PlanePlate(pPlate,plate_arr))	//拆分双面板为两个单面板
		return FALSE;
	int iRet,iRetDaum=-1,iRetHuoqu=-1;
	for(int i=0;i<2;i++)
	{
		CCutSector cutSector;
		InitCutSectorBy3DSector(cutSector,sector_3d,plate_arr[i].ucs);	//根据三维扇形和单面板所在面确定切割二维扇形
		iRet=CutPlateBySector(&plate_arr[i],cutSector,FALSE);	//通过扇形区域切割单面板
		if(i==0)
			iRetDaum=iRet;
		else
			iRetHuoqu=iRet;
	}
	if(iRetDaum==-1||iRetHuoqu==-1)
		return FALSE;
	else if(iRetDaum==0&&iRetHuoqu==0)
	{
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("All plates are to be cut off!");
#else
			AfxMessageBox("钢板被全部切割掉!");
#endif
		return FALSE;
	}
	else if(iRetDaum==1&&iRetHuoqu==1)
	{
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("plates are not cut!");
#else
			AfxMessageBox("钢板没有被切割!");
#endif
		return FALSE;
	}
	else
	{
		if(!Combine2PlanePlate(pPlate,plate_arr))
			return FALSE;
	}
	pPlate->SetModified();
	pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return TRUE;
}
//根据输入夹角面的法线，计算沿面法线进行等距偏移量时，等距角点的实际偏移矢量　wjh-2013.6.29
f3dPoint EquidistantCornerOffset(f3dPoint base_norm,f3dPoint other_norm,double offset_thick=1.0)
{
	f3dPoint offset_vec;
	if(!normalize(base_norm)||!normalize(other_norm))
		return offset_vec;
	f3dPoint bisector=base_norm+other_norm;
	if(!normalize(bisector))
		return offset_vec;
	double cosa=bisector*base_norm;	//实际bisector*base_norm值理论上应该恒大于等0
	if(cosa>0)
		offset_vec=bisector*(offset_thick/cosa);
	return offset_vec;
}
#ifdef __ALFA_TEST_VERSION_
#include "LogFile.h"
void Test_EquidistantCornerOffset()
{
	logerr.ClearContents();
	for(int i=0;i<8;i++)
	{
		double angle=45*(i-1)*RADTODEG_COEF;
		f3dPoint other_norm(cos(angle),sin(angle),0);
		f3dPoint offset=EquidistantCornerOffset(f3dPoint(0,1,0),other_norm);
		logerr.Log("%d#%.3f,%.3f,%.3f",i*45+45,offset.x,offset.y,offset.z);
	}
	logerr.ShowToScreen();
}
#endif
void Init3DCutSectorByPlate(CUTSECTOR_3D& sector_3d,CLDSPlate* pCutPlate,BOOL bTopFace,double fMinDist)
{
	if(pCutPlate->face_N!=2)
		return ;
	//1、获取火曲信息
	f3dPoint huoqu_start,huoqu_end,huoqu_offset_vec;
	PROFILE_VERTEX* pPrevVertex=pCutPlate->vertex_list.GetTail();
	for(PROFILE_VERTEX* pVertex=pCutPlate->vertex_list.GetFirst();pVertex;pVertex=pCutPlate->vertex_list.GetNext())
	{
		if(pVertex->vertex.feature>10)
		{
			if(pPrevVertex->vertex.feature==1)
				huoqu_start=pVertex->vertex;
			else if(pPrevVertex->vertex.feature==2)
				huoqu_end=pVertex->vertex;
		}
		pPrevVertex=pVertex;
	}
	coord_trans(huoqu_start,pCutPlate->ucs,TRUE);
	coord_trans(huoqu_end,pCutPlate->ucs,TRUE);
	f3dPoint huoqu_vec=huoqu_end-huoqu_start;
	normalize(huoqu_vec);
	if(bTopFace)
		huoqu_offset_vec=EquidistantCornerOffset(pCutPlate->ucs.axis_z,pCutPlate->huoqufacenorm[0],pCutPlate->GetThick()+fMinDist);	
	else
		huoqu_offset_vec=EquidistantCornerOffset(pCutPlate->ucs.axis_z,pCutPlate->huoqufacenorm[0],-fMinDist);	
	huoqu_start+=pCutPlate->ucs.axis_z*pCutPlate->m_fNormOffset+huoqu_offset_vec;
	huoqu_end+=pCutPlate->ucs.axis_z*pCutPlate->m_fNormOffset+huoqu_offset_vec;
	//2、根据火曲线确定三维扇形区域
	sector_3d.corner=huoqu_start;	//扇形角点
	sector_3d.rot_axis=huoqu_vec;	//旋转轴方向
	if(bTopFace)	//顶面切割时旋转轴方向改变
		sector_3d.rot_axis*=-1;
	f3dPoint datum_plane_vec=pCutPlate->ucs.axis_z^huoqu_vec;
	normalize(datum_plane_vec);
	sector_3d.start_vec=datum_plane_vec;	//起始方向
	f3dPoint huoqu_plane_vec=pCutPlate->huoqufacenorm[0]^(-huoqu_vec);
	normalize(huoqu_plane_vec);
	sector_3d.end_vec=huoqu_plane_vec;	//终止方向
}
void InitCutSectorBy3DSector(CCutSector& cutSector,CUTSECTOR_3D sector_3d,UCS_STRU ucs)
{
	f3dPoint corner,inters,start,start_vec,end,end_vec;
	//扇形角点
	Int3dlf(corner,sector_3d.corner,sector_3d.rot_axis,ucs.origin,ucs.axis_z);
	coord_trans(corner,ucs,FALSE);
	corner.z=0;
	cutSector.corner.Set(corner.x,corner.y);
	//起始方向
	start=sector_3d.corner+sector_3d.start_vec*100;
	Int3dlf(inters,start,sector_3d.rot_axis,ucs.origin,ucs.axis_z);
	coord_trans(inters,ucs,FALSE);
	inters.z=0;
	start_vec=inters-corner;
	normalize(start_vec);
	//终止方向
	end=sector_3d.corner+sector_3d.end_vec*100;
	Int3dlf(inters,end,sector_3d.rot_axis,ucs.origin,ucs.axis_z);
	coord_trans(inters,ucs,FALSE);
	inters.z=0;
	end_vec=inters-corner;
	normalize(end_vec);
	if(ucs.axis_z*sector_3d.rot_axis<0)
	{
		f3dPoint tmp_vec=start_vec;
		start_vec=end_vec;
		end_vec=tmp_vec;
	}
	cutSector.edgeVecStart.Set(start_vec.x,start_vec.y);
	cutSector.edgeVecEnd.Set(end_vec.x,end_vec.y);
}
//通过扇形区域切割单面板:-1切割失败 0全部切割 1没有切割 2切割成功 
int CutPlateBySector(CLDSPlate* pPlate,CCutSector cutSector,BOOL bPrompt/* =TRUE */)
{
	if(pPlate->face_N!=1 || pPlate->vertex_list.GetNodeNum()<3)
		return -1;
	bool cornerInserted=false;
	bool bVertexModify=false;
	//1 根据扇形区域取舍钢板顶点，并计算新生成顶点
	int iFaceNo=10;
	PROFILE_VERTEX *pCurVertex=NULL,*pPrevVertex=NULL,*pInsertVertex=NULL;
	pPrevVertex=pPlate->vertex_list.GetTail();
	for(pCurVertex=pPlate->vertex_list.GetFirst();pCurVertex;pCurVertex=pPlate->vertex_list.GetNext())
	{
		if(pPrevVertex->vertex.feature<=0)
			return -1;	//板轮廓点面号有误
		else if(pPrevVertex->vertex.feature<iFaceNo)
			iFaceNo=pCurVertex->vertex.feature;
		f2dPoint inters_start,inters_end;
		//1.1计算轮廓边直线与扇形间相对位置及相交状态,	0.扇形区外;1.扇形区内;2.扇形区边界上;3.交点为扇形角点
		EDGE_INTERS_STATE state=cutSector.CalEdgeIntersState(pPrevVertex->vertex,pCurVertex->vertex,inters_start,inters_end);
		//1.2根据位置及相交状态决定原轮廓点存留及插入新交点
		if(state.stateVertexStart==1)	//顶点在有效扇形区域，最终被删除
			pPrevVertex->vertex.feature*=-1;
			// 0.无交点;1.交点为内点;2.交点为直线端点;3.交点为扇形角点
		if(state.intersStartEdge==2)
		{	//轮廓边直线与扇形始边交点为直线端点
			double diff_start=fabs(pPrevVertex->vertex.x-inters_start.x)+fabs(pPrevVertex->vertex.y-inters_start.y);
			double diff_end=fabs(pCurVertex->vertex.x-inters_start.x)+fabs(pCurVertex->vertex.y-inters_start.y);
			if(diff_start<diff_end)	//与轮廓边直线始端顶点重合
				pPrevVertex->vertex.feature=abs(pPrevVertex->vertex.feature)%100+100;
			else
				pCurVertex->vertex.feature=abs(pCurVertex->vertex.feature)%100+100;
		}
		if(state.intersEndEdge==2)
		{	//轮廓边直线与扇形始边交点为直线端点
			double diff_start=fabs(pPrevVertex->vertex.x-inters_end.x)+fabs(pPrevVertex->vertex.y-inters_end.y);
			double diff_end=fabs(pCurVertex->vertex.x-inters_end.x)+fabs(pCurVertex->vertex.y-inters_end.y);
			if(diff_start<diff_end)	//与轮廓边直线始端顶点重合
				pPrevVertex->vertex.feature=abs(pPrevVertex->vertex.feature)%100+200;
			else
				pCurVertex->vertex.feature=abs(pCurVertex->vertex.feature)%100+200;
		}
		if(state.intersStartEdge==3||state.intersEndEdge==3)
		{	//角点在当前轮廓边上
			if((state.stateVertexStart==1&&state.stateVertexEnd==0)||(state.stateVertexStart==0&&state.stateVertexEnd==1))
			{	//轮廓边始顶点在扇形区内，会被切割，而终点则在扇形区外保留
				pInsertVertex=pPlate->vertex_list.insert(*pPrevVertex);
				pInsertVertex->vertex.Set(cutSector.corner.x,cutSector.corner.y);
				pInsertVertex->vertex.feature=min(ftoi(pPrevVertex->vertex.feature),ftoi(pCurVertex->vertex.feature));
				pInsertVertex->vertex.feature=abs(pInsertVertex->vertex.feature);	//+300表示为新插入与扇形角点;
				cornerInserted=true;
			}
		}
		else if(state.intersStartEdge==1&&state.intersEndEdge!=1)
		{	//与始边有内交点,与终边无内交点
			pInsertVertex=pPlate->vertex_list.insert(*pPrevVertex);
			pInsertVertex->vertex.Set(inters_start.x,inters_start.y);
			pInsertVertex->vertex.feature=min(ftoi(pPrevVertex->vertex.feature),ftoi(pCurVertex->vertex.feature));
			pInsertVertex->vertex.feature=abs(pInsertVertex->vertex.feature)+100;	//+100表示为新插入与扇形起始边内相交的顶点;
		}
		else if(state.intersStartEdge!=1&&state.intersEndEdge==1)
		{	//与始边无内交点,与终边有内交点
			pInsertVertex=pPlate->vertex_list.insert(*pPrevVertex);
			pInsertVertex->vertex.Set(inters_end.x,inters_end.y);
			pInsertVertex->vertex.feature=min(ftoi(pPrevVertex->vertex.feature),ftoi(pCurVertex->vertex.feature));
			pInsertVertex->vertex.feature=abs(pInsertVertex->vertex.feature)+200;	//+200表示为新插入与扇形终止边内相交的顶点;
		}
		else if(state.intersStartEdge==1&&state.intersEndEdge==1)
		{	//与始边和终边都有内交点
			f2dPoint inters_vec(inters_end.x-inters_start.x,inters_end.y-inters_start.y);
			f2dPoint edge_vec(pCurVertex->vertex.x-pPrevVertex->vertex.x,pCurVertex->vertex.y-pPrevVertex->vertex.y);
			PROFILE_VERTEX *pInsertVertex1=pPlate->vertex_list.insert(*pPrevVertex);
			PROFILE_VERTEX *pInsertVertex2=pPlate->vertex_list.insert(*pPrevVertex);
			if(inters_vec.x*edge_vec.x+inters_vec.y*edge_vec.y<0)	//两交点先后顺序与原轮廓边不一致
			{
				pInsertVertex1->vertex.Set(inters_end.x,inters_end.y);
				pInsertVertex1->vertex.feature=abs(pInsertVertex->vertex.feature)+200;	//+200表示为新插入与扇形终止边内相交的顶点;
				pInsertVertex2->vertex.Set(inters_start.x,inters_start.y);
				pInsertVertex2->vertex.feature=abs(pInsertVertex->vertex.feature)+100;	//+100表示为新插入与扇形起始边内相交的顶点;
			}
			else
			{
				pInsertVertex1->vertex.Set(inters_start.x,inters_start.y);
				pInsertVertex1->vertex.feature=abs(pInsertVertex->vertex.feature)+100;	//+100表示为新插入与扇形起始边内相交的顶点;
				pInsertVertex2->vertex.Set(inters_end.x,inters_end.y);
				pInsertVertex2->vertex.feature=abs(pInsertVertex->vertex.feature)+200;	//+200表示为新插入与扇形终止边内相交的顶点;
			}
			pInsertVertex1->vertex.feature=pInsertVertex2->vertex.feature=min(ftoi(pPrevVertex->vertex.feature),ftoi(pCurVertex->vertex.feature));
		}
		pPrevVertex=pCurVertex;	//记录前一个顶点，用来判断轮廓边类型
	}
	//2 根据新轮廓顶点的feature值,插入扇形角点
	pPrevVertex=pPlate->vertex_list.GetTail();
	while(pPrevVertex&&pPrevVertex->vertex.feature<=0)
		pPrevVertex=pPlate->vertex_list.GetPrev();
	if(pPrevVertex==NULL)
		return 0;	//该板被全部切割掉
	int prevInsertVertexEdge=pPrevVertex->vertex.feature/100;
	pPrevVertex=pPlate->vertex_list.GetTail();
	for(pCurVertex=pPlate->vertex_list.GetFirst();pCurVertex;pCurVertex=pPlate->vertex_list.GetNext())
	{
		if(pCurVertex->vertex.feature>=100||pCurVertex->vertex.feature<0)
			bVertexModify=true;
		if(pCurVertex->vertex.feature<0)
		{
			pPlate->vertex_list.DeleteCursor();
			pPrevVertex=pCurVertex;
			continue;
		}
		int insertVertexEdge=pCurVertex->vertex.feature/100;
		if(prevInsertVertexEdge>0&&insertVertexEdge>0&&prevInsertVertexEdge!=insertVertexEdge)
		{
			if(cornerInserted)
				return -1;	//切割失败,不支持将一块单面板分割为两块的情况
			pInsertVertex=pPlate->vertex_list.insert(*pPrevVertex);
			pInsertVertex->vertex.Set(cutSector.corner.x,cutSector.corner.y);
			//不能从pPrevVertex->vertex.feature获取，因pPrevVertex有可能是制弯点
			pInsertVertex->vertex.feature=iFaceNo;
			cornerInserted=true;
		}
		prevInsertVertexEdge=insertVertexEdge;
		pPrevVertex=pCurVertex;
	}
	for(pCurVertex=pPlate->vertex_list.GetFirst();pCurVertex;pCurVertex=pPlate->vertex_list.GetNext())
		pCurVertex->vertex.feature%=100;
	//3 对所有顶点进行全取或全舍的特殊情况进行处理
	if(pPlate->vertex_list.GetNodeNum()<3)
	{
		pPlate->vertex_list.Empty();
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("All plates are to be cut off!");
#else
			AfxMessageBox("钢板被全部切割掉!");
#endif
		return 0;
	}

	//4.删除扇形边与轮廓重叠时造成的重叠折线
	pPrevVertex=pPlate->vertex_list.GetTail();
	for(pCurVertex=pPlate->vertex_list.GetFirst();pCurVertex;pCurVertex=pPlate->vertex_list.GetNext())
	{
		BOOL pushed=pPlate->vertex_list.push_stack();
		PROFILE_VERTEX* pNextVertex=pPlate->vertex_list.GetNext();
		if(pNextVertex==NULL)
			pNextVertex=pPlate->vertex_list.GetFirst();
		pPlate->vertex_list.pop_stack(pushed);

		double prev_deltax=pCurVertex->vertex.x-pPrevVertex->vertex.x;
		double prev_deltay=pCurVertex->vertex.y-pPrevVertex->vertex.y;
		double curr_deltax=pNextVertex->vertex.x-pCurVertex->vertex.x;
		double curr_deltay=pNextVertex->vertex.y-pCurVertex->vertex.y;
		if(fabs(prev_deltay*curr_deltax-prev_deltax*curr_deltay)<EPS)	//共线
		{
			if(prev_deltax*curr_deltax+prev_deltay*curr_deltay<0)
				pPlate->vertex_list.DeleteCursor();	//重叠折线，删除中间顶点
		}
		pPrevVertex=pCurVertex;
	}
	pPlate->vertex_list.Clean();
	if(!bVertexModify)	//所有外形顶点没有经过任何处理
	{
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("plates are not cut!");
#else
			AfxMessageBox("钢板没有进行切割!");
#endif
		return 1;
	}
	//5 去除不在板范围内的螺栓引用
	for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())	//螺栓引用
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		f3dPoint ls_pos=pBolt->ucs.origin;
		coord_trans(ls_pos,pPlate->ucs,FALSE);
		ls_pos.z=0;
		if(cutSector.PositionType(f2dPoint(ls_pos.x,ls_pos.y))>0)	//在扇形区域内或边界上
			pPlate->GetLsRefList()->DeleteCursor();
	}
	pPlate->GetLsRefList()->Clean();
	return 2;
}
//</DEVELOP_PROCESS_MARK>
//<DEVELOP_PROCESS_MARK_TEST nameId="CLDSView::CutPlateByPlate">
#ifdef __ALFA_TEST_VERSION_
#include "DxfFile.h"
void Test_CutPlateBySector()
{
	logerr.ClearContents();
	CCutSector sector[7];
	CDxfFile dxf;
	f3dPoint start,end,corner_offset;
	PROFILE_VERTEX* pVertex=NULL;
	dxf.OpenFile("d:\\testCutPlate.dxf");
	for(int i=0;i<7;i++)
	{
		sector[i].corner.Set(0,0);
		sector[i].edgeVecStart.Set(1,0);
		sector[i].edgeVecEnd.Set(cos((i+1)*Pi/4.0),sin((i+1)*Pi/4.0));
		corner_offset=f3dPoint(1,0,0)*400*i;
		start=corner_offset+f3dPoint(1,0,0)*30;
		end=corner_offset+f3dPoint(cos((i+1)*Pi/4.0),sin((i+1)*Pi/4.0),0)*30;
		for(int j=0;j<6;j++)
		{
			CLDSPlate plate;
			plate.ucs.origin.Set(0,0,0);
			plate.ucs.axis_x.Set(1,0,0);
			plate.ucs.axis_y.Set(0,1,0);
			plate.ucs.axis_z.Set(0,0,1);
			f3dPoint vertex,vertex_offset;
			if(j==0)
			{
				vertex.Set(20,15,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(-20,10,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(-25,0,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(-25,-20,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(20,-20,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
			}
			else if(j==1)
			{
				vertex.Set(20,0,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(20,30,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(0,15,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(-25,30,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(-20,0,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
			}
			else if(j==2)
			{
				vertex.Set(0,0,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(-30,-20,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(30,-25,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
			}
			else if(j==3)
			{
				vertex.Set(0,-30,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(30,-30,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(10,-10,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(25,10,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(30,30,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(0,20,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
			}
			else if(j==4)
			{
				vertex.Set(0,0,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(0,30,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(-20,30,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(-25,-30,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
			}
			else if(j==5)
			{
				vertex.Set(5,0,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(20,20,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(-20,20,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(-5,0,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(-20,-20,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;
				vertex.Set(20,-20,0);
				pVertex=plate.vertex_list.append(PROFILE_VERTEX(vertex));
				pVertex->vertex.feature=1;

			}
			//切割前后图形绘制
			vertex_offset=f3dPoint(0,-1,0)*200*j;
			dxf.NewLine(corner_offset+vertex_offset,start+vertex_offset);
			dxf.NewLine(corner_offset+vertex_offset,end+vertex_offset);
			PROFILE_VERTEX* pPrevVertex=plate.vertex_list.GetTail();
			for(PROFILE_VERTEX* pCurVertex=plate.vertex_list.GetFirst();pCurVertex;pCurVertex=plate.vertex_list.GetNext())
			{
				dxf.NewLine(pPrevVertex->vertex+corner_offset+vertex_offset,pCurVertex->vertex+corner_offset+vertex_offset);
				pPrevVertex=pCurVertex;
			}
			//-1切割失败 0全部切割 1没有切割 2切割成功 
			int result=CutPlateBySector(&plate,sector[i],FALSE);
			if(result==-1)
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("%d-%d cut failure",i,j);
			else if(result==0)
				logerr.Log("%d-%d All plates are to be cut off",i,j);
			else if(result==1)
				logerr.Log("%d-%d plates are not cut",i,j);
#else
				logerr.Log("%d-%d切割失败",i,j);
			else if(result==0)
				logerr.Log("%d-%d钢板被全部切割",i,j);
			else if(result==1)
				logerr.Log("%d-%d钢板没有被切割",i,j);
#endif
			else
			{	
				f3dPoint org_offset=f3dPoint(1,0,0)*70;
				pPrevVertex=plate.vertex_list.GetTail();
				for(pCurVertex=plate.vertex_list.GetFirst();pCurVertex;pCurVertex=plate.vertex_list.GetNext())
				{
					dxf.NewLine(pPrevVertex->vertex+corner_offset+vertex_offset+org_offset,pCurVertex->vertex+corner_offset+vertex_offset+org_offset);
					pPrevVertex=pCurVertex;
				}
			}
		}
	}
	dxf.CloseFile();
	logerr.ShowToScreen();
}
#endif
//</DEVELOP_PROCESS_MARK_TEST>
//将双面板拆分成两个单面板
BOOL Split2PlanePlate(CLDSPlate* pPlate,CLDSPlate* plate_arr)
{
	if(pPlate->face_N!=2)
		return FALSE;
	//1、获取火曲信息
	CLDSPlate* pDatumPlate=&plate_arr[0];
	CLDSPlate* pHuoquPlate=&plate_arr[1];
	f3dPoint huoqu_start,huoqu_end,huoqu_vec;
	huoqu_start=pPlate->HuoQuLine[0].startPt;
	huoqu_end=pPlate->HuoQuLine[0].endPt;
	coord_trans(huoqu_start,pPlate->ucs,TRUE);
	coord_trans(huoqu_end,pPlate->ucs,TRUE);
	huoqu_vec=huoqu_end-huoqu_start;
	normalize(huoqu_vec);
	//2、确定两单面板坐标系
	pDatumPlate->ucs.origin=huoqu_start;
	pDatumPlate->ucs.axis_x=huoqu_vec;
	pDatumPlate->ucs.axis_z=pPlate->ucs.axis_z;
	pDatumPlate->ucs.axis_y=pDatumPlate->ucs.axis_z^pDatumPlate->ucs.axis_x;
	normalize(pDatumPlate->ucs.axis_y);
	if(!is_ucs_valid(pDatumPlate->ucs))
		return FALSE;
	pHuoquPlate->ucs.origin=huoqu_end;
	pHuoquPlate->ucs.axis_x=huoqu_vec;
	pHuoquPlate->ucs.axis_z=pPlate->huoqufacenorm[0];
	pHuoquPlate->ucs.axis_y=pHuoquPlate->ucs.axis_z^pHuoquPlate->ucs.axis_x;
	normalize(pHuoquPlate->ucs.axis_y);
	pHuoquPlate->ucs.axis_x=pHuoquPlate->ucs.axis_y^pHuoquPlate->ucs.axis_z;
	normalize(pHuoquPlate->ucs.axis_z);
	if(!is_ucs_valid(pHuoquPlate->ucs))
		return FALSE;
	//3、初始化两单面板的外形顶点
	f3dPoint datum_vertex,huoqu_vertex;
	for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		huoqu_vertex=datum_vertex=pPlate->GetRealVertex(pVertex->vertex);
		if(pVertex->vertex.feature==1 || pVertex->vertex.feature>10)	//将顶点加入到基准面板;	
		{	
			coord_trans(datum_vertex,pPlate->ucs,TRUE);
			coord_trans(datum_vertex,pDatumPlate->ucs,FALSE);
			datum_vertex.z=0;
			pVertex=pDatumPlate->vertex_list.append(*pVertex);
			pVertex->vertex.Set(datum_vertex.x,datum_vertex.y,datum_vertex.z);
		}
		if(pVertex->vertex.feature==2 || pVertex->vertex.feature>10)	//将顶点加入到火曲面板
		{
			coord_trans(huoqu_vertex,pPlate->ucs,TRUE);
			coord_trans(huoqu_vertex,pHuoquPlate->ucs,FALSE);
			huoqu_vertex.z=0;
			pVertex=pHuoquPlate->vertex_list.append(*pVertex);
			pVertex->vertex.Set(huoqu_vertex.x,huoqu_vertex.y,huoqu_vertex.z);
		}
	}
	//3、获取两单面板上的螺栓引用
	for(CLsRef* pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		if(fabs(pBolt->ucs.axis_z*pPlate->ucs.axis_z.normalized())
			>fabs(pBolt->ucs.axis_z*pPlate->huoqufacenorm[0].normalized()))
			pDatumPlate->GetLsRefList()->append(*pLsRef);
		else
			pHuoquPlate->GetLsRefList()->append(*pLsRef);
	}
	return TRUE;
}
//合并两单面为双面板
BOOL Combine2PlanePlate(CLDSPlate* pPlate,CLDSPlate *plate_arr)
{
	int datumVertexsNum=plate_arr[0].vertex_list.GetNodeNum();
	int huoquVertexsNum=plate_arr[1].vertex_list.GetNodeNum();
	if(pPlate->face_N!=2||(datumVertexsNum==0&&huoquVertexsNum==0))
		return FALSE;
	ATOM_LIST<PROFILE_VERTEX> vertex_list;
	pPlate->vertex_list.Empty();
	if(datumVertexsNum==0&&huoquVertexsNum>3)	//仅保留火曲面
	{	//将火曲面顶点坐标转换到绝对坐标系下，保存到vertex_list
		pPlate->ucs=plate_arr[1].ucs;
		for(PROFILE_VERTEX* pVertex=plate_arr[1].vertex_list.GetFirst();pVertex;pVertex=plate_arr[1].vertex_list.GetNext())
		{	
			pVertex->vertex.feature=1;
			pPlate->vertex_list.append(*pVertex);
		}
	}
	else
	{
		PROFILE_VERTEX *pVertex=NULL;
		if(datumVertexsNum>3&&huoquVertexsNum==0)	//仅保留基准面
		{	//将基准面顶点坐标转换到绝对坐标系下，保存到vertex_list
			for(pVertex=plate_arr[0].vertex_list.GetFirst();pVertex;pVertex=plate_arr[0].vertex_list.GetNext())
			{	
				coord_trans(pVertex->vertex,plate_arr[0].ucs,TRUE);
				pVertex->vertex.feature=1;
				pPlate->vertex_list.append(*pVertex);
			}
		}
		else	//基准面和火曲面都存在的情况
		{
			ARRAY_LIST<PROFILE_VERTEX> vertex_arr0,vertex_arr1;
			vertex_arr0.SetSize(0,plate_arr[0].vertex_list.GetNodeNum());
			vertex_arr1.SetSize(0,plate_arr[1].vertex_list.GetNodeNum());
			for(pVertex=plate_arr[0].vertex_list.GetFirst();pVertex;pVertex=plate_arr[0].vertex_list.GetNext())
				vertex_arr0.append(*pVertex);
			for(pVertex=plate_arr[1].vertex_list.GetFirst();pVertex;pVertex=plate_arr[1].vertex_list.GetNext())
				vertex_arr1.append(*pVertex);
			int i,huoqustart_index0=-1,huoqustart_index1=-1;
			for(i=0;i<vertex_arr0.GetSize();i++)
			{
				huoqustart_index0=(i-1+vertex_arr0.GetSize())%vertex_arr0.GetSize();
				if(vertex_arr0[huoqustart_index0].vertex.feature>10&&vertex_arr0[i].vertex.feature>10)
					break;
			}
			for(i=0;i<vertex_arr1.GetSize();i++)
			{
				huoqustart_index1=(i-1+vertex_arr1.GetSize())%vertex_arr1.GetSize();
				if(vertex_arr1[huoqustart_index1].vertex.feature>10&&vertex_arr1[i].vertex.feature>10)
					break;
			}
			if(huoqustart_index0<0||huoqustart_index1<0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Cutting a double-plane plate into two singular-plane plates is not supported temporarily");
#else
				AfxMessageBox("暂不支持将一块双面板分割为两块单面板情况");
#endif
				return FALSE;
			}
			//插入基准面普通顶点
			for(i=2;i<vertex_arr0.GetSize();i++)
			{
				int index=(i+huoqustart_index0)%vertex_arr0.GetSize();
				pVertex=vertex_list.append(vertex_arr0[index]);
				coord_trans(pVertex->vertex,plate_arr[0].ucs,TRUE);
			}
			//插入制弯线始顶点
			pVertex=vertex_list.append(vertex_arr0[huoqustart_index0]);
			coord_trans(pVertex->vertex,plate_arr[0].ucs,TRUE);
			//插入制弯面顶点
			for(i=2;i<vertex_arr1.GetSize();i++)
			{
				int index=(i+huoqustart_index1)%vertex_arr1.GetSize();
				pVertex=vertex_list.append(vertex_arr1[index]);
				coord_trans(pVertex->vertex,plate_arr[1].ucs,TRUE);
			}
			//插入制弯线终顶点
			pVertex=vertex_list.append(vertex_arr0[(huoqustart_index0+1)%vertex_arr0.GetSize()]);
			coord_trans(pVertex->vertex,plate_arr[0].ucs,TRUE);
		}
		//将vertex_list中顶点转回原始板基准面坐标系下，填充原始双面板的顶点列表
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			coord_trans(pVertex->vertex,pPlate->ucs,FALSE);
			pVertex->vertex=pPlate->GetSpreadVertex(pVertex->vertex,pVertex->vertex.feature);
			pVertex->vertex.z=0;
			pPlate->vertex_list.append(*pVertex);
		}
	}
	//添充双面板的螺栓引用
	pPlate->EmptyLsRef();
	if(plate_arr[0].GetLsCount()>0)
	{
		for(CLsRef* pLsRef=plate_arr[0].GetFirstLsRef();pLsRef;pLsRef=plate_arr[0].GetNextLsRef())
			pPlate->AppendLsRef(*pLsRef);
	}
	if(plate_arr[1].GetLsCount()>0)
	{
		for(CLsRef* pLsRef=plate_arr[1].GetFirstLsRef();pLsRef;pLsRef=plate_arr[1].GetNextLsRef())
			pPlate->AppendLsRef(*pLsRef);
	}
	return TRUE;
}
/*static int GetFaceIndex(UCS_STRU tm_ucs,f3dPoint vertex,BOOL bTopFace)
{	//将vertex转换到tm_ucs相对坐标系下,若z坐标小于0表示处于基准面,反之处于火曲面;bTopFace==TRUE,正好相反
	int iUcsIndex=0;	//使用第一个坐标系
	f3dPoint tm_pnt=vertex;
	coord_trans(tm_pnt,tm_ucs,FALSE);
	if(bTopFace&&tm_pnt.z<0)
		iUcsIndex=1;	//使用第二个坐标系
	else if(!bTopFace&&tm_pnt.z>0)
		iUcsIndex=1;	//使用第二个坐标系
	return iUcsIndex;
}*/
//通过指定的钢板切割钢板(自动判断钢板切割面)
//1.双面板切割单面板 2.双面板切割双面板
//BOOL CLDSView::CutPlateByPlate(CLDSPlate *pPlate,CLDSPlate *pDatumPlate,
//							   BOOL bTopFace,double fMinDist,BOOL bPrompt/*=TRUE*/)
/*{
	if(pPlate->face_N==3)
	{
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Sorry, three-plane plate can not be cut temporarily!");
#else
			AfxMessageBox("很抱歉,目前暂不能分割三面板!");
#endif
		return FALSE;
	}
	if(pDatumPlate->face_N==3)
	{
		if(bPrompt)
			AfxMessageBox("很抱歉,目前暂不支持使用三面板切割钢板!");
		return FALSE;
	}
	if(pPlate->vertex_list.GetNodeNum()<3||pDatumPlate->vertex_list.GetNodeNum()<3)
		return FALSE;
	//基准钢板为单面板时,直接调用通过平面切割钢板函数即可
	if(pDatumPlate->face_N==1)	
		return CutPlateByPlane(pPlate,pDatumPlate->ucs.origin,pDatumPlate->ucs.axis_z,fMinDist,bPrompt);
	//下面是双面板切割钢板的情况
	//1、分别为基准面和火曲面建立坐标系
	int i=0;
	UCS_STRU ucs[2];
	f3dPoint axis_z[2];	//相对坐标系下的方向
	f3dPoint face_pos[2],face_norm[2];	//钢板基准面(第二面)基准点以及法线
	f3dPoint datum_pos=pDatumPlate->HuoQuLine[0].startPt;
	coord_trans(datum_pos,pDatumPlate->ucs,TRUE);
	f3dPoint huoqu_offset_vec=pDatumPlate->ucs.axis_z+pDatumPlate->huoqufacenorm[0];
	normalize(huoqu_offset_vec);
	for(i=0;i<2;i++)
	{
		face_norm[i]=pDatumPlate->ucs.axis_z;
		if(i==1) //第二面
			face_norm[i]=pDatumPlate->huoqufacenorm[0];	
		face_pos[i]=datum_pos+face_norm[i]*pDatumPlate->m_fNormOffset;
		if(bTopFace)
		{	//顶面负法线方向切割,拾取点(之前在底面)沿所在法线负方向偏移一个间距后到顶面上
			face_norm[i]*=-1.0;
			double cosa=fabs(huoqu_offset_vec*pDatumPlate->ucs.axis_z);
			double huoqu_offset_dist=pDatumPlate->GetThick()*(1.0/cosa);
			face_pos[i]-=face_norm[i]*huoqu_offset_dist;
		}
		//假定的坐标系	
		ucs[i].origin=face_pos[i];
		ucs[i].axis_z=face_norm[i];
		ucs[i].axis_y=inters_vec(face_norm[i]);
		ucs[i].axis_x=ucs[i].axis_y^ucs[i].axis_z;
		ucs[i].axis_y=ucs[i].axis_z^ucs[i].axis_x;
		normalize(ucs[i].axis_x);
		normalize(ucs[i].axis_y);
		normalize(ucs[i].axis_z);
		axis_z[i]=face_norm[i];
		vector_trans(axis_z[i],pPlate->ucs,FALSE);
	}
	//定制相对坐标系用来判断切割钢板时使用哪个基准面 
	UCS_STRU tm_ucs;
	tm_ucs.origin=ucs[0].origin;
	tm_ucs.axis_x=ucs[0].axis_z+ucs[1].axis_z;
	normalize(tm_ucs.axis_x);
	tm_ucs.axis_y=ucs[0].axis_z^tm_ucs.axis_x;
	normalize(tm_ucs.axis_y);
	tm_ucs.axis_z=tm_ucs.axis_x^tm_ucs.axis_y;
	normalize(tm_ucs.axis_z);
	
	int old_face_N=pPlate->face_N,face_N=1;
	if(pPlate->face_N==2)
	{	//1.切割以后的板为单面板 2.切割以后的板仍然为双面板
		f3dPoint huoqu_start=pPlate->HuoQuLine[0].startPt;
		f3dPoint huoqu_end=pPlate->HuoQuLine[0].endPt;
		coord_trans(huoqu_start,pPlate->ucs,TRUE);
		coord_trans(huoqu_end,pPlate->ucs,TRUE);
		coord_trans(huoqu_start,ucs[GetFaceIndex(tm_ucs,huoqu_start,bTopFace)],FALSE);
		coord_trans(huoqu_end,ucs[GetFaceIndex(tm_ucs,huoqu_end,bTopFace)],FALSE);
		if(huoqu_start.z>=0&&huoqu_end.z>=0)
			face_N=1;	//切割以后的板为单面板
		else
			face_N=2;	//切割以后的板为双面板
	}
	f3dPoint tm_pnt;
	PROFILE_VERTEX *pVertex=NULL,*pPrevVertex=pPlate->vertex_list.GetTail();
	f3dPoint vertice_prev=pPlate->GetRealVertex(pPrevVertex->vertex);
	coord_trans(vertice_prev,pPlate->ucs,TRUE);
	//
	int cursor_face_i,first_face_i,prev_face_i;
	first_face_i=prev_face_i=pPrevVertex->vertex.feature;
	double prev_z,cursor_z;
	tm_pnt=vertice_prev;
	coord_trans(tm_pnt,ucs[GetFaceIndex(tm_ucs,tm_pnt,bTopFace)],FALSE);
	prev_z=tm_pnt.z;
	if(prev_z>0)
		pPrevVertex->vertex.feature*=-1;	//最终要被删除掉的顶点
	int iFaceIndex=0;	//切割面索引
	f3dPoint vertice_cursor;
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		cursor_face_i=pVertex->vertex.feature;
		vertice_cursor=pPlate->GetRealVertex(pVertex->vertex);
		coord_trans(vertice_cursor,pPlate->ucs,TRUE);
		tm_pnt=vertice_cursor;
		iFaceIndex=GetFaceIndex(tm_ucs,tm_pnt,bTopFace);	//切割面索引
		coord_trans(tm_pnt,ucs[iFaceIndex],FALSE);
		cursor_z=tm_pnt.z;
		if(cursor_z>0&&pVertex->vertex.feature>0)	//最终要被删除掉的顶点
			pVertex->vertex.feature*=-1;
		if(prev_z*cursor_z>=0)	//在同侧不需计算交点
		{
			prev_z=cursor_z;
			vertice_prev=vertice_cursor;
			prev_face_i=cursor_face_i;
			pPrevVertex=pVertex;//记录前一个顶点，用来判断当前轮廓边类型 
			continue;
		}
		else	//分在同一切割面的两侧，需要计算线面交点，一般不会出现两个连续点分在不同切割面的两侧
		{
			f3dPoint inters,vertice;
			if(pPrevVertex&&pPrevVertex->type>0)
			{	//圆弧边 目前出现圆弧边的钢板都为单面板  wht 09-11-11
				f3dPoint inters_pick,inters_line_vec;
				f3dPoint face_pick1=pPlate->ucs.origin, face_norm1=pPlate->ucs.axis_z;
				if(Int3d2f(&inters_pick,&inters_line_vec,face_pick1,face_norm1,face_pos[iFaceIndex],face_norm[iFaceIndex])==1)
				{
					f3dLine inters_line;
					inters_line.startPt=inters_pick-inters_line_vec*100000;
					inters_line.endPt=inters_pick+inters_line_vec*100000;
					coord_trans(inters_line.startPt,pPlate->ucs,FALSE);
					coord_trans(inters_line.endPt,pPlate->ucs,FALSE);
					inters_line.startPt.z=inters_line.endPt.z=0;
					//生成圆弧线
					f3dArcLine arcLine;
					int sign=1;
					if(pPrevVertex->type==1)
					{	//圆弧
						if(pPrevVertex->sector_angle<0)
							sign=-1;
						arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),fabs(pPrevVertex->sector_angle));
					}
					else if(pPrevVertex->type==2)	//指定圆弧R
					{	
						if(pPrevVertex->radius<0)
							sign=-1;
						arcLine.CreateMethod3(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),sign*pPrevVertex->radius,pPrevVertex->center);
						//不能用CreateMethod2,否则丢失了拾取点信息，对于大于180°的弧是错的。 wht 12-06-14
						//arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),pPrevVertex->sector_angle);
					}
					else if(pPrevVertex->type==3)	//椭圆弧
					{
						f3dPoint center = pPrevVertex->center;
						f3dPoint col_axis=pPrevVertex->column_norm;
						coord_trans(center,pPlate->ucs,FALSE);
						vector_trans(col_axis,pPlate->ucs,FALSE);
						if(pPrevVertex->radius<0)
							sign=-1;
						arcLine.CreateEllipse(center,pPrevVertex->vertex,pVertex->vertex,col_axis,f3dPoint(0,0,sign),sign*pPrevVertex->radius);
					}
					if(arcLine.SectorAngle()>0)
					{
						f2dArc arc;
						arc.angle=arcLine.SectorAngle();
						if(arcLine.WorkNorm().z>0)
							arc.startPt = arcLine.Start();
						else
							arc.startPt = arcLine.End();
						arc.centre = arcLine.Center();
						f3dPoint pos1,pos2;
						f2dLine line;
						line.startPt.Set(inters_line.startPt.x,inters_line.startPt.y);
						line.endPt.Set(inters_line.endPt.x,inters_line.endPt.y);
						int nRet=Int2dla(line,arc,pos1.x,pos1.y,pos2.x,pos2.y);
						if(nRet>0)	//暂不处理直线与圆弧有两个交点的情况
						{
							if(pPrevVertex->type==3)
							{	//椭圆弧
								double radius=fabs(pPrevVertex->radius);
								f3dPoint inters1,inters2;
								f3dPoint pick=inters_pick,vec=inters_line_vec;
								int nRetCode=Int3dlc(&inters1,&inters2,pick,vec,pPrevVertex->center-pPrevVertex->column_norm*1000,
									pPrevVertex->center+pPrevVertex->column_norm*1000,radius,radius);
								if(nRetCode>0)
								{
									coord_trans(inters1,pPlate->ucs,FALSE);
									coord_trans(inters2,pPlate->ucs,FALSE);
									inters1.z=inters2.z=0;
									if(nRetCode==2&&DISTANCE(pos1,inters1)>DISTANCE(pos1,inters2))
										pos1=inters2;
									else
										pos1=inters1;
								}
							}
							PROFILE_VERTEX *pInsertVertex=pPlate->vertex_list.insert(PROFILE_VERTEX(pos1));
							/*
							double offset_angle=fMinDist*(1.0/pPrevVertex->radius);//新生成的钢板与基准钢板间的距离
							//arcLine.WorkNorm().z>0==>逆时针 cursor_z<0==>起点
							if(cursor_z*arcLine.WorkNorm().z>0)
								offset_angle*=-1.0;
							rotate_point_around_axis(pos1,offset_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
							*/
						/*	double arc_r=arcLine.Radius();	
							if(fabs(arc_r)>EPS)
							{	//圆弧半径小于等于0时为异常情况 wht 11-05-27
								if(cursor_z<0)	//当前顶点不会被删除
								{	
									pInsertVertex->type=pPrevVertex->type;
									pInsertVertex->center=pPrevVertex->center;
									pInsertVertex->radius=pPrevVertex->radius;
									pInsertVertex->column_norm=pPrevVertex->column_norm;
									pInsertVertex->m_bWeldEdge=pPrevVertex->m_bWeldEdge;
									double half_len = 0.5*DISTANCE(pVertex->vertex,pos1);
									//使用计算得出的圆弧半径且考虑扇形角正负号 wht 11-05-30
									if(fabs(half_len/arc_r)<=1&&pPrevVertex->type==1)	
										pInsertVertex->sector_angle = 2*asin(half_len/arc_r)*sign;
								}
								else //当前顶点为被删除顶点，更新前一个顶点数据
								{
									double sector_angle=0;
									double half_len = 0.5*DISTANCE(pPrevVertex->vertex,pos1);
									//使用计算得出的圆弧半径且考虑扇形角正负号 wht 11-05-30
									if(fabs(half_len/arc_r)<=1)
										sector_angle = 2*asin(half_len/arc_r);
									if(pPrevVertex->type==1)
										pPrevVertex->sector_angle=sign*sector_angle;
								}
							}
						}
					}
				}
			}
			else
			{	//直线边
				Int3dlf(inters,f3dLine(vertice_prev,vertice_cursor),face_pos[iFaceIndex],face_norm[iFaceIndex]);
				coord_trans(inters,pPlate->ucs,FALSE);
				vertice=inters;
				if(abs(prev_face_i)==abs(cursor_face_i)&&abs(cursor_face_i)<10)
					vertice.feature=cursor_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(cursor_face_i));	//相交边全部落在某一面内
				else if(abs(prev_face_i)==abs(cursor_face_i)&&abs(cursor_face_i)>10)
					vertice=pPlate->GetSpreadVertex(inters,1);//相交边整好全部是火曲线点,这一情况一般不可能
				else if(abs(cursor_face_i)<10&&abs(prev_face_i)>10)
					vertice.feature=cursor_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(cursor_face_i));	//相交边一端是火曲点 一端是普通点
				else if(abs(prev_face_i)<10&&abs(cursor_face_i)>10)
					vertice.feature=prev_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(prev_face_i));	//相交边一端是火曲点 一端时普通点
				vertice.z=0;	//pVertex->vertex.z;
				vertice-=axis_z[iFaceIndex]*fMinDist; //新生成的钢板与基准钢板间的距离
				PROFILE_VERTEX *pInsertVertex=pPlate->vertex_list.insert(vertice);
				if(cursor_z<0)	//当前顶点不会被删除
				{
					pInsertVertex->m_bWeldEdge=pPrevVertex->m_bWeldEdge;	//焊缝线
					pInsertVertex->m_bRollEdge=pPrevVertex->m_bRollEdge;	//卷边
					pInsertVertex->manu_space=pPrevVertex->manu_space;		//加工间隙
				}
				else if(pInsertVertex->vertex.feature<0)
					pInsertVertex->vertex.feature*=-1;	//保证新插入的顶点不被删除掉
			}
			prev_z=cursor_z;
			pPrevVertex=pVertex;//记录前一个顶点，用来判断轮廓边类型
			vertice_prev=vertice_cursor;
			prev_face_i=cursor_face_i;
		}
		//对于用双面板去切割任意板的一条边的情况不做处理(此情况一般不发生)
	}
	if(face_N==1)
	{	//判断切割面是否与被切割钢板相交,现在仅仅处理单面板的情况
		int iDelVertex=0;
		SCOPE_STRU scope;
		for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			scope.VerifyVertex(pVertex->vertex);
		for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			if(pVertex->vertex.feature<0)
			{
				if(iDelVertex==0&&old_face_N==1)
				{	//将第一应该被切除的顶点修改为双面板火曲线与被切割钢板的交点 wht 11-01-20
					f3dPoint inters_pick,inters_vec,inters_pt;
					if(Int3d2f(&inters_pick,&inters_vec,face_pos[0],face_norm[0],face_pos[1],face_norm[1])==1)
					{	//基准面与火曲面交线
						f3dPoint inters_pt;
						int nRetCode=Int3dlf(inters_pt,inters_pick,inters_vec,pPlate->ucs.origin,pPlate->ucs.axis_z);
						coord_trans(inters_pt,pPlate->ucs,FALSE);
						inters_pt.z=0;
						if(nRetCode&&scope.IsIncludePoint(inters_pt))
						{	//交点在被切割钢板区域内，修改第一个被删除点坐标为火曲线与钢板交点
							pVertex->vertex=inters_pt;
							pVertex->vertex.feature*=-1;
							pVertex->m_bCanCut=FALSE;	//可能为凹点，故设置为不允许设计外形时删除该顶点
						}
					}
				}
				iDelVertex++;
			}
		}
		if(iDelVertex==pPlate->vertex_list.GetNodeNum()||iDelVertex==0)
		{	//板上所有顶点都在切割面的一侧，无法完成切割
			if(iDelVertex==pPlate->vertex_list.GetNodeNum())
			{	//钢板上所有顶点都在切割面一侧且所有点都应被删除时将feature修改为正值 wht 10-12-01
				for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
				{	
					if(pVertex->vertex.feature<0)
						pVertex->vertex.feature*=-1;
				}
			}
			if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Cut plane has no joints with plate, unable to finish cutting!");
#else
				AfxMessageBox("切割面与钢板不相交，无法完成切割!");
#endif
			return FALSE;
		}
	}
	//去除被切割掉的非火曲顶点
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		if(face_N==1)
		{
			if(pVertex->vertex.feature<0)	//应该被切割掉的顶点
				pPlate->vertex_list.DeleteCursor();
		}
		else 
		{
			if(pVertex->vertex.feature<0&&abs(pVertex->vertex.feature)<10)	//应该被切割掉的非火曲点
				pPlate->vertex_list.DeleteCursor();
		}
	}
	//如果第一个顶点就被切割掉的话,上面的去除工作就会遗漏检测一个顶点
	pVertex=pPlate->vertex_list.GetFirst();
	if(pVertex&&pVertex->vertex.feature<0&&abs(pVertex->vertex.feature)<10)
		pPlate->vertex_list.DeleteCursor();
	
	if(pPlate->vertex_list.GetNodeNum()<3)
	{
		pPlate->vertex_list.Empty();
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("All plates are to be cut off!");
#else
			AfxMessageBox("钢板被全部切割掉!");
#endif
		return TRUE;
	}
	//重新设计被切割掉的火曲顶点
	int n=pPlate->vertex_list.GetNodeNum();
	pPlate->face_N=face_N;
	if(pPlate->vertex_list.GetFirst()->vertex.feature==2&&pPlate->face_N==1)
	{	//切割后新生成的单面板为原来双面板的第二面时调整板的坐标系
		pPlate->ucs.axis_z=pPlate->huoqufacenorm[0].normalized();
		pPlate->ucs.axis_x=pPlate->ucs.axis_y^pPlate->ucs.axis_z;
		normalize(pPlate->ucs.axis_x);
	}
	for(i=0;i<n;i++)
	{
		pVertex=&pPlate->vertex_list[i];
		if(face_N==1)
			pVertex->vertex.feature=face_N;
		if(pVertex->vertex.feature<-10)	//重新设计火曲点
		{
			pVertex->vertex.feature=abs(pVertex->vertex.feature);
			f3dPoint *pPrevPoint=&pPlate->vertex_list[(i+n-1)%n].vertex;
			f3dPoint *pNextPoint=&pPlate->vertex_list[(i+1)%n].vertex;
			pPrevPoint->z=pNextPoint->z=0;	//Z坐标归零否则求交可能失败 wht 11-01-19
			//1.1计算新生成双面板的火曲点
			f3dPoint inters,startPos,endPos,datum_startPos,datum_endPos;
			BOOL bHashHuoquLineInters=FALSE;
			BOOL bNeedCalHuoquInters=TRUE;
			datum_startPos=pDatumPlate->HuoQuLine[0].startPt;
			datum_endPos=pDatumPlate->HuoQuLine[0].endPt;
			coord_trans(datum_startPos,pDatumPlate->ucs,TRUE);
			coord_trans(datum_endPos,pDatumPlate->ucs,TRUE);
			startPos=pPlate->HuoQuLine[0].startPt;
			endPos=pPlate->HuoQuLine[0].endPt;
			coord_trans(startPos,pPlate->ucs,TRUE);
			coord_trans(endPos,pPlate->ucs,TRUE);
			if(Int3dpl(f3dLine(startPos,endPos),f3dLine(datum_startPos,datum_endPos),inters)==1)
			{	
				bHashHuoquLineInters=TRUE;	//两条火曲线有交点(底面切割有效)
				bNeedCalHuoquInters=FALSE;
			}
			if(bTopFace)
			{	//顶面切割时,基准面火曲线需偏移到顶面切割面上
				f3dPoint huoqu_offset_vec=face_norm[0]+face_norm[1];
				normalize(huoqu_offset_vec);
				double cosa=fabs(huoqu_offset_vec*face_norm[0]);
				double huoqu_offset_dist=pDatumPlate->GetThick()*(1.0/cosa);
				datum_startPos -= huoqu_offset_vec*huoqu_offset_dist;
				datum_endPos -= huoqu_offset_vec*huoqu_offset_dist;
				if(!bHashHuoquLineInters&&Int3dpl(f3dLine(startPos,endPos),f3dLine(datum_startPos,datum_endPos),inters)==1)
				{	
					bHashHuoquLineInters=TRUE;	//两火曲线无交点时，基准面火曲线偏移之后需重新计算是否与被切割钢板火曲线有交点
					bNeedCalHuoquInters=FALSE;
				}
				else
					bNeedCalHuoquInters=TRUE;
			}
			if(bNeedCalHuoquInters)
			{	//重新计算火曲点时,通过火曲线与切割面相交求火曲点,
				//先用火曲线与基准切割面求交,查看交点落在哪个面上,如果在火曲面上需要重新计算火曲点
				Int3dlf(inters,f3dLine(startPos,endPos),face_pos[0],face_norm[0]);
				iFaceIndex=GetFaceIndex(tm_ucs,inters,bTopFace);
				if(iFaceIndex==1)
					Int3dlf(inters,f3dLine(startPos,endPos),face_pos[1],face_norm[1]);
			}
			//1.2更新火曲点坐标
			coord_trans(inters,pPlate->ucs,FALSE);
			pVertex->vertex.Set(inters.x,inters.y,0);
			//1.3两条火曲线没有交点,此时会增加一个点(凸凹点)
			if(!bHashHuoquLineInters)
			{	
				f3dPoint temp_pick,temp_norm;
				temp_norm=pPlate->ucs.axis_z;
				temp_pick=pPlate->HuoQuLine[0].startPt;
				coord_trans(temp_pick,pPlate->ucs,TRUE);
				UCS_STRU tmp_ucs;
				tmp_ucs.origin=temp_pick;
				tmp_ucs.axis_x=pPlate->ucs.axis_z+pPlate->huoqufacenorm[0];
				normalize(tmp_ucs.axis_x);
				tmp_ucs.axis_y=pPlate->ucs.axis_z^tmp_ucs.axis_x;
				normalize(tm_ucs.axis_y);
				tmp_ucs.axis_z=tmp_ucs.axis_x^tmp_ucs.axis_y;
				normalize(tmp_ucs.axis_z);
				Int3dlf(inters,f3dLine(datum_startPos,datum_endPos),temp_pick,temp_norm);
				int iFace1=GetFaceIndex(tmp_ucs,inters,FALSE);
				if(iFace1==1)
				{
					temp_norm=pPlate->huoqufacenorm[0];
					Int3dlf(inters,f3dLine(datum_startPos,datum_endPos),temp_pick,temp_norm);
				}
				coord_trans(inters,pPlate->ucs,FALSE);
				inters.z=0;
				if(iFace1+1==pPrevPoint->feature)
				{
					PROFILE_VERTEX *pInsertVertex=pInsertVertex=pPlate->vertex_list.insert(inters,i);
					pInsertVertex->vertex.feature=pPrevPoint->feature;
					pInsertVertex->m_bCanCut=FALSE;	//可能为凹点，故设置为不允许设计外形时删除该顶点
				}
				else if(iFace1+1==pNextPoint->feature)
				{
					PROFILE_VERTEX *pInsertVertex=pInsertVertex=pPlate->vertex_list.insert(inters);
					pInsertVertex->vertex.feature=pNextPoint->feature;
					pInsertVertex->m_bCanCut=FALSE;	//可能为凹点，故设置为不允许设计外形时删除该顶点
				}
			}
			//基准钢板为单面板时直接调用通过平面切割钢板函数即可
			//else if(pDatumPlate->face_N==1)
			//{
			//	Int3dpl(f3dLine(*pPrevPoint,*pNextPoint),pPlate->HuoQuLine[0],inters);
			//	pVertex->vertex.Set(inters.x,inters.y,inters.z);
			//}
		}
	}
	//去除不在板范围内的螺栓引用
	for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		f3dPoint ls_pos=(*pLsRef)->ucs.origin;
		coord_trans(ls_pos,ucs[iFaceIndex],FALSE);
		if(ls_pos.z>0)
			pPlate->GetLsRefList()->DeleteCursor();
	}
	//移除不在板范围内的射线杆件	wht 10-11-30
	CDesignLjPartPara *pLjPara=NULL;
	if(pPlate->jdb_style!=4)
	{	//螺栓垫板中第一个连接构件其实是切割角钢，不能剔除
		for(pLjPara=pPlate->GetFirstLjPara();pLjPara;pLjPara=pPlate->GetNextLjPara())
		{
			//获得射线杆件螺栓引用列表
			CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(pLjPara->hPart,CLS_LINEPART);
			if(pLinePart==NULL)
				continue;
			CLsRefList *pLsRefList=NULL;
			if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//钢管
				CLDSParamPlate *pParamPlate=NULL;
				pParamPlate=((CLDSLineTube*)pLinePart)->GetParamPlate(pLjPara->start0_end1);
				if(pParamPlate)
					pLsRefList=pParamPlate->GetLsRefList();
			}
			else 
				pLsRefList=pLinePart->GetLsRefList();
			//螺栓引用列表为空或者该列表中的不包括钢板上的螺栓，则删除该射线杆件
			if(pLsRefList==NULL)
				pPlate->DeleteLjPara(pLjPara->hPart);
			else
			{
				CLsRef *pLsRef=NULL;
				for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{
					if(pLsRefList->FromHandle(pLsRef->GetLsPtr()->handle))
						break;	//螺栓引用列表中包括钢板上的螺栓，该射线杆件为钢板上的射线杆件
				}
				if(pLsRef==NULL)
					pPlate->DeleteLjPara(pLjPara->hPart);
			}
		}
		pPlate->GetLsRefList()->Clean();
	}
	pPlate->SetModified();
	pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return TRUE;
}*/

BOOL CLDSView::CutPlateByPlane(CLDSPlate *pPlate,f3dPoint face_pos,f3dPoint face_norm,double fMinDist,BOOL bPrompt/*=TRUE*/,BOOL bSetWeldEdge/*=TRUE*/)
{
	if(pPlate->face_N==3)
	{
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Sorry, three-plane plate can not be cut temporarily!");
#else
			AfxMessageBox("很抱歉,目前暂不能分割三面板!");
#endif
		return FALSE;
	}
	if(pPlate->vertex_list.GetNodeNum()<3)
		return FALSE;
	//后续判断时有的地方考虑了间隙值fMinDist有的未考虑，故统一考虑到face_pos，并清零fMinDist中 wjh-2019.9.19
	face_pos-=fMinDist*face_norm;
	fMinDist=0;
	//假定的坐标系
	UCS_STRU ucs;
	ucs.origin=face_pos;
	ucs.axis_z=face_norm;
	ucs.axis_y=inters_vec(face_norm);
	ucs.axis_x=ucs.axis_y^ucs.axis_z;
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_x);
	normalize(ucs.axis_y);
	normalize(ucs.axis_z);
	f3dPoint axis_z;
	axis_z=face_norm;
	vector_trans(axis_z,pPlate->ucs,FALSE);
	//以钢板中间面为基准进行切割(否则切割生成的两块钢板因切割基准不一致导致外形不同) wht 16-12-04
	double fNormOffset=pPlate->GetNormOffset()+0.5*pPlate->GetThick();	//底面到中间面的偏移量
	PROFILE_VERTEX *pVertex=NULL,*pPrevVertex=pPlate->vertex_list.GetFirst();
	f3dPoint vertice_first,vertice_prev=pPlate->GetRealVertex(pPrevVertex->vertex);
	coord_trans(vertice_prev,pPlate->ucs,TRUE);
	if(fabs(fNormOffset)>EPS)
	{
		if(pPrevVertex->vertex.feature==3)
			vertice_prev+=pPlate->huoqufacenorm[1]*fNormOffset;
		else if(pPrevVertex->vertex.feature==2)
			vertice_prev+=pPlate->huoqufacenorm[0]*fNormOffset;
		else //if(pPrevVertex->vertex.feature==1||pPrevVertex->vertex.feature>10)
			vertice_prev+=pPlate->ucs.axis_z*fNormOffset;
	}
	f3dPoint tm_pnt,vertice_cursor;
	double first_z,prev_z,cursor_z;
	int cursor_face_i,first_face_i,prev_face_i,face_N=1;
	if(pPlate->face_N==2)
	{	//1.切割以后的板为单面板 2.切割以后的板仍然为双面板
		PROFILE_VERTEX *pHuoquStart=NULL,*pHuoquEnd=NULL;
		//此处使用火曲线始终端坐标判断切割后是否为双面板,直接使用HuoQuLine[0]判断可能不准确,
		//使用GetBendlineVertexies()函数获取真正的火曲线轮廓点		wht 16-10-08
		pPlate->GetBendlineVertexies(&pHuoquStart,&pHuoquEnd);
		f3dPoint huoqu_start=pHuoquStart?pHuoquStart->vertex:pPlate->HuoQuLine[0].startPt;
		f3dPoint huoqu_end=pHuoquEnd?pHuoquEnd->vertex:pPlate->HuoQuLine[0].endPt;
		coord_trans(huoqu_start,pPlate->ucs,TRUE);
		coord_trans(huoqu_start,ucs,FALSE);
		coord_trans(huoqu_end,pPlate->ucs,TRUE);
		coord_trans(huoqu_end,ucs,FALSE);
		if(huoqu_start.z>0&&huoqu_end.z>0)
			face_N=1;	//切割以后的板为单面板
		else
			face_N=2;	//切割以后的板为双面板
	}
	first_face_i=prev_face_i=pPrevVertex->vertex.feature;
	tm_pnt=vertice_first=vertice_prev;
	coord_trans(tm_pnt,ucs,FALSE);
	first_z=prev_z=tm_pnt.z;
	if(prev_z>0)
		pPrevVertex->vertex.feature*=-1;	//最终要被删除掉的顶点
	for(pVertex=pPlate->vertex_list.GetNext();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		cursor_face_i=pVertex->vertex.feature;
		vertice_cursor=pPlate->GetRealVertex(pVertex->vertex);
		coord_trans(vertice_cursor,pPlate->ucs,TRUE);
		if(fabs(fNormOffset)>EPS)
		{
			if(pVertex->vertex.feature==3)
				vertice_cursor+=pPlate->huoqufacenorm[1]*fNormOffset;
			else if(pVertex->vertex.feature==2)
				vertice_cursor+=pPlate->huoqufacenorm[0]*fNormOffset;
			else //if(pVertex->vertex.feature==1||pVertex->vertex.feature>10)
				vertice_cursor+=pPlate->ucs.axis_z*fNormOffset;
		}
		tm_pnt=vertice_cursor;
		coord_trans(tm_pnt,ucs,FALSE);
		cursor_z=tm_pnt.z;
		if(cursor_z>0)			//最终要被删除掉的顶点
			pVertex->vertex.feature*=-1;
		if(prev_z*cursor_z>=0)	//在同侧不需计算交点
		{
			prev_z=cursor_z;
			vertice_prev=vertice_cursor;
			prev_face_i=cursor_face_i;
			pPrevVertex=pVertex;//记录前一个顶点，用来判断当前轮廓边类型 
			continue;
		}
		else	//分在切割面的两侧，需要计算线面交点
		{
			f3dPoint inters,vertice;
			if(pPrevVertex&&pPrevVertex->type>0)
			{	//圆弧边 目前出现圆弧边的钢板都为单面板  wht 09-11-11
				f3dPoint inters_pick,inters_line_vec;
				f3dPoint face_pick1=pPlate->ucs.origin, face_norm1=pPlate->ucs.axis_z;
				if(fabs(fNormOffset)>EPS)
					face_pick1+=pPlate->ucs.axis_z*fNormOffset;
				if(Int3d2f(&inters_pick,&inters_line_vec,face_pick1,face_norm1,face_pos,face_norm)==1)
				{
					f3dLine inters_line;
					inters_line.startPt=inters_pick-inters_line_vec*100000;
					inters_line.endPt=inters_pick+inters_line_vec*100000;
					coord_trans(inters_line.startPt,pPlate->ucs,FALSE);
					coord_trans(inters_line.endPt,pPlate->ucs,FALSE);
					inters_line.startPt.z=inters_line.endPt.z=0;
					//生成圆弧线
					f3dArcLine arcLine;
					int sign=1;
					if(pPrevVertex->type==1)
					{	//圆弧
						if(pPrevVertex->sector_angle<0)
							sign=-1;
						arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),fabs(pPrevVertex->sector_angle));
					}
					else if(pPrevVertex->type==2)	//指定圆弧R
					{	
						if(pPrevVertex->radius<0)
							sign=-1;
						arcLine.CreateMethod3(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),sign*pPrevVertex->radius,pPrevVertex->center);
						//不能用CreateMethod2,否则丢失了拾取点信息，对于大于180°的弧是错的。 wht 12-06-14
						//arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),pPrevVertex->sector_angle);
					}
					else if(pPrevVertex->type==3)	//椭圆弧
					{
						f3dPoint center = pPrevVertex->center;
						f3dPoint col_axis=pPrevVertex->column_norm;
						coord_trans(center,pPlate->ucs,FALSE);
						vector_trans(col_axis,pPlate->ucs,FALSE);
						if(pPrevVertex->radius<0)
							sign=-1;
						arcLine.CreateEllipse(center,pPrevVertex->vertex,pVertex->vertex,col_axis,f3dPoint(0,0,sign),sign*pPrevVertex->radius);
					}
					if(arcLine.SectorAngle()>0)
					{
						f2dArc arc;
						arc.angle=arcLine.SectorAngle();
						if(arcLine.WorkNorm().z>0)
							arc.startPt = arcLine.Start();
						else
							arc.startPt = arcLine.End();
						arc.centre = arcLine.Center();
						f3dPoint pos1,pos2;
						f2dLine line;
						line.startPt.Set(inters_line.startPt.x,inters_line.startPt.y);
						line.endPt.Set(inters_line.endPt.x,inters_line.endPt.y);
						int nRet=Int2dla(line,arc,pos1.x,pos1.y,pos2.x,pos2.y);
						if(nRet>0)	//暂不处理直线与圆弧有两个交点的情况
						{
							if(pPrevVertex->type==3)
							{	//椭圆弧
								double radius=fabs(pPrevVertex->radius);
								f3dPoint inters1,inters2;
								f3dPoint pick=inters_pick,vec=inters_line_vec;
								int nRetCode=Int3dlc(&inters1,&inters2,pick,vec,pPrevVertex->center-pPrevVertex->column_norm*1000,
									pPrevVertex->center+pPrevVertex->column_norm*1000,radius,radius);
								if(nRetCode>0)
								{
									coord_trans(inters1,pPlate->ucs,FALSE);
									coord_trans(inters2,pPlate->ucs,FALSE);
									inters1.z=inters2.z=0;
									if(nRetCode==2&&DISTANCE(pos1,inters1)>DISTANCE(pos1,inters2))
										pos1=inters2;
									else
										pos1=inters1;
								}
							}
							PROFILE_VERTEX *pInsertVertex=pPlate->vertex_list.insert(PROFILE_VERTEX(pos1));
							/*
							double offset_angle=fMinDist*(1.0/pPrevVertex->radius);//新生成的钢板与基准钢板间的距离
							//arcLine.WorkNorm().z>0==>逆时针 cursor_z<0==>起点
							if(cursor_z*arcLine.WorkNorm().z>0)
								offset_angle*=-1.0;
							rotate_point_around_axis(pos1,offset_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
							*/
							//pInsertVertex->radius 此处应使用圆弧半径(type==1时pInsertVertex->radius==0) wht 11-05-27
							double arc_r=arcLine.Radius();	
							if(fabs(arc_r)>EPS)
							{	//圆弧半径小于等于0时为异常情况 wht 11-05-27
								if(cursor_z<0)	//当前顶点不会被删除
								{	
									pInsertVertex->type=pPrevVertex->type;
									pInsertVertex->center=pPrevVertex->center;
									pInsertVertex->radius=pPrevVertex->radius;
									pInsertVertex->column_norm=pPrevVertex->column_norm;
									pInsertVertex->m_bWeldEdge=pPrevVertex->m_bWeldEdge;
									double half_len = 0.5*DISTANCE(pVertex->vertex,pos1);
									//使用计算得出的圆弧半径且考虑扇形角正负号 wht 11-05-30
									if(fabs(half_len/arc_r)<=1&&pPrevVertex->type==1)
										pInsertVertex->sector_angle = 2*asin(half_len/arc_r)*sign;
								}
								else //当前顶点为被删除顶点，更新前一个顶点数据
								{
									double sector_angle=0;
									double half_len = 0.5*DISTANCE(pPrevVertex->vertex,pos1);
									//使用计算得出的圆弧半径且考虑扇形角正负号 wht 11-05-30
									if(fabs(half_len/arc_r)<=1)
										sector_angle = 2*asin(half_len/arc_r);
									if(pPrevVertex->type==1)
										pPrevVertex->sector_angle=sign*sector_angle;
									if(bSetWeldEdge)
										pInsertVertex->m_bWeldEdge=true;	//设切割边为焊缝线
								}
							}
						}
					}
				}
			}
			else
			{	//直线边
				Int3dlf(inters,f3dLine(vertice_prev,vertice_cursor),face_pos,face_norm);
				coord_trans(inters,pPlate->ucs,FALSE);
				vertice=inters;
				if(abs(prev_face_i)==abs(cursor_face_i)&&abs(cursor_face_i)<10)
					vertice.feature=cursor_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(cursor_face_i));	//相交边全部落在某一面内
				else if(abs(prev_face_i)==abs(cursor_face_i)&&abs(cursor_face_i)>10)
					vertice=pPlate->GetSpreadVertex(inters,1);//相交边整好全部是火曲线点,这一情况一般不可能
				else if(abs(cursor_face_i)<10&&abs(prev_face_i)>10)
					vertice.feature=cursor_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(cursor_face_i));	//相交边一端是火曲点 一端是普通点
				else if(abs(prev_face_i)<10&&abs(cursor_face_i)>10)
					vertice.feature=prev_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(prev_face_i));	//相交边一端是火曲点 一端时普通点
				if(vertice.feature>1)
					vertice=pPlate->GetSpreadVertex(vertice,vertice.feature);
				vertice.z=0;	//pVertex->vertex.z;
				/*防止因顶点过近导致难以发现的对称板外形校核不匹配错误 wjh-2015.5.15
				  如果保留的顶点坐标与计算的交点坐标过近，则用交点坐标更新保留的顶点坐标
				  pVertex->vertex.feature>0 保留的顶点为当前顶点   pVertex
				  pVertex->vertex.feature<0 保留的顶点为前一个顶点 pPrevVertex   wxc-2017.1.23
				*/
				PROFILE_VERTEX *pInsertVertex;
				if(pVertex->vertex.feature>0 && DISTANCE(vertice,pVertex->vertex)<0.5)
				{	
					pInsertVertex=pVertex;
					pInsertVertex->vertex.Set(vertice.x,vertice.y);
				}
				else if(pVertex->vertex.feature<0 && DISTANCE(vertice,pPrevVertex->vertex)<0.5)
				{
					pInsertVertex=pPrevVertex;
					pInsertVertex->vertex.Set(vertice.x,vertice.y);
				}
				else
					pInsertVertex=pPlate->vertex_list.insert(vertice);
				if(cursor_z<0)	//当前顶点不会被删除
				{
					pInsertVertex->m_bWeldEdge=pPrevVertex->m_bWeldEdge;	//焊缝线
					pInsertVertex->m_bRollEdge=pPrevVertex->m_bRollEdge;	//卷边
					pInsertVertex->manu_space=pPrevVertex->manu_space;		//加工间隙
				}
				else if(bSetWeldEdge)
					pInsertVertex->m_bWeldEdge=true;	//设新切割边为焊缝线
			}
			prev_z=cursor_z;
			pPrevVertex=pVertex;//记录前一个顶点，用来判断轮廓边类型
			vertice_prev=vertice_cursor;
			prev_face_i=cursor_face_i;
		}
	}
	if(first_z*cursor_z<0)	//在首末顶点间进行了切割
	{
		f3dPoint inters,vertice;
		pPrevVertex=pPlate->vertex_list.GetTail();
		pVertex=pPlate->vertex_list.GetFirst();
		if(pVertex&&pPrevVertex&&pPrevVertex->type>0)
		{	//圆弧边 目前出现圆弧边的钢板都为单面板  wht 09-11-11
			f3dPoint inters_pick,inters_line_vec;
			f3dPoint face_pick1=pPlate->ucs.origin, face_norm1=pPlate->ucs.axis_z;
			if(Int3d2f(&inters_pick,&inters_line_vec,face_pick1,face_norm1,face_pos,face_norm)==1)
			{
				f3dLine inters_line;
				inters_line.startPt=inters_pick-inters_line_vec*100000;
				inters_line.endPt=inters_pick+inters_line_vec*100000;
				coord_trans(inters_line.startPt,pPlate->ucs,FALSE);
				coord_trans(inters_line.endPt,pPlate->ucs,FALSE);
				inters_line.startPt.z=inters_line.endPt.z=0;
				//生成圆弧线
				f3dArcLine arcLine;
				int sign=1;
				if(pPrevVertex->type==1)
				{	//圆弧
					if(pPrevVertex->sector_angle<0)
						sign=-1;
					arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),fabs(pPrevVertex->sector_angle));
				}
				else if(pPrevVertex->type==2)	//指定圆弧R
				{	
					if(pPrevVertex->radius<0)
						sign=-1;
					arcLine.CreateMethod3(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),sign*pPrevVertex->radius,pPrevVertex->center);
					//不能用CreateMethod2,否则丢失了拾取点信息，对于大于180°的弧是错的。 wht 12-06-14
					//arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),pPrevVertex->sector_angle);
				}
				else if(pPrevVertex->type==3)	//椭圆弧
				{
					f3dPoint center = pPrevVertex->center;
					f3dPoint col_axis=pPrevVertex->column_norm;
					coord_trans(center,pPlate->ucs,FALSE);
					vector_trans(col_axis,pPlate->ucs,FALSE);
					if(pPrevVertex->radius<0)
						sign=-1;
					arcLine.CreateEllipse(center,pPrevVertex->vertex,pVertex->vertex,col_axis,f3dPoint(0,0,sign),sign*pPrevVertex->radius);
				}
				if(arcLine.SectorAngle()>0)
				{
					f2dArc arc;
					arc.angle=arcLine.SectorAngle();
					if(arcLine.WorkNorm().z>0)
						arc.startPt = arcLine.Start();
					else
						arc.startPt = arcLine.End();
					arc.centre = arcLine.Center();
					f3dPoint pos1,pos2;
					f2dLine line;
					line.startPt.Set(inters_line.startPt.x,inters_line.startPt.y);
					line.endPt.Set(inters_line.endPt.x,inters_line.endPt.y);
					int nRet=Int2dla(line,arc,pos1.x,pos1.y,pos2.x,pos2.y);
					if(nRet>0)	//暂不处理直线与圆弧有两个交点的情况
					{
						if(pPrevVertex->type==3)
						{	//椭圆弧
							double radius=fabs(pPrevVertex->radius);
							f3dPoint inters1,inters2;
							f3dPoint pick=inters_pick,vec=inters_line_vec;
							int nRetCode=Int3dlc(&inters1,&inters2,pick,vec,pPrevVertex->center-pPrevVertex->column_norm*1000,
								pPrevVertex->center+pPrevVertex->column_norm*1000,radius,radius);
							if(nRetCode>0)
							{
								coord_trans(inters1,pPlate->ucs,FALSE);
								coord_trans(inters2,pPlate->ucs,FALSE);
								inters1.z=inters2.z=0;
								if(nRetCode==2&&DISTANCE(pos1,inters1)>DISTANCE(pos1,inters2))
									pos1=inters2;
								else
									pos1=inters1;
							}
						}
						//pInsertVertex->radius 此处应使用圆弧半径(type==1时pInsertVertex->radius==0) wht 11-05-27
						double arc_r=arcLine.Radius();	
						if(fabs(arc_r)>EPS)
						{	//圆弧半径小于等于0时为异常情况 wht 11-05-27
							PROFILE_VERTEX *pInsertVertex=pPlate->vertex_list.insert(PROFILE_VERTEX(pos1));
							if(first_z<0)	//当前顶点不会被删除
							{	
								pInsertVertex->type=pPrevVertex->type;
								pInsertVertex->center=pPrevVertex->center;
								pInsertVertex->radius=pPrevVertex->radius;
								pInsertVertex->column_norm=pPrevVertex->column_norm;
								pInsertVertex->m_bWeldEdge=pPrevVertex->m_bWeldEdge;
								double half_len = 0.5*DISTANCE(pVertex->vertex,pos1);
								//使用计算得出的圆弧半径且考虑扇形角正负号 wht 11-05-30
								if(fabs(half_len/arc_r)<=1&&pPrevVertex->type==1)
									pInsertVertex->sector_angle = 2*asin(half_len/arc_r)*sign;
							}
							else //当前顶点为被删除顶点，更新前一个顶点数据
							{	 
								double sector_angle=0;
								double half_len = 0.5*DISTANCE(pPrevVertex->vertex,pos1);
								//使用计算得出的圆弧半径且考虑扇形角正负号 wht 11-05-30
								if(fabs(half_len/arc_r)<=1)
									sector_angle = 2*asin(half_len/arc_r);
								if(pPrevVertex->type==1)
									pPrevVertex->sector_angle=sign*sector_angle;
							}
						}
					}
				}
			}
		}
		else
		{
			Int3dlf(inters,f3dLine(vertice_prev,vertice_first),face_pos,face_norm);
			coord_trans(inters,pPlate->ucs,FALSE);
			vertice=inters;
			if(abs(prev_face_i)==abs(first_face_i)&&abs(first_face_i)<10)
				vertice.feature=first_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(first_face_i));	//相交边全部落在某一面内
			else if(abs(prev_face_i)==abs(first_face_i)&&abs(first_face_i)>10)
				vertice=pPlate->GetSpreadVertex(inters,1);//相交边整好全部是火曲线点,这一情况一般不可能
			else if(abs(first_face_i)<10&&abs(prev_face_i)>10)
				vertice.feature=first_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(first_face_i));	//相交边一端是火曲点 一端是普通点
			else if(abs(prev_face_i)<10&&abs(first_face_i)>10)
				vertice.feature=prev_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(prev_face_i));	//相交边一端是火曲点 一端时普通点	
			if(vertice.feature>1)
				vertice=pPlate->GetSpreadVertex(vertice,vertice.feature);
			vertice.z=0;	//pPlate->vertex_list.GetFirst()->vertex.z;
			PROFILE_VERTEX *pInsertVertex;
			if(pVertex->vertex.feature>0 && DISTANCE(vertice,pVertex->vertex)<0.5)
			{	
				pInsertVertex=pVertex;
				pInsertVertex->vertex.Set(vertice.x,vertice.y);
			}
			else if(pVertex->vertex.feature<0 && DISTANCE(vertice,pPrevVertex->vertex)<0.5)
			{	
				pInsertVertex=pPrevVertex;
				pInsertVertex->vertex.Set(vertice.x,vertice.y);
			}
			else
				pInsertVertex=pPlate->vertex_list.insert(vertice);
			if(first_z<0)	//当前顶点不会被删除
			{
				pInsertVertex->m_bWeldEdge=pPrevVertex->m_bWeldEdge;	//焊缝线
				pInsertVertex->m_bRollEdge=pPrevVertex->m_bRollEdge;	//卷边
				pInsertVertex->manu_space=pPrevVertex->manu_space;		//加工间隙
			}
			else if(bSetWeldEdge)
				pInsertVertex->m_bWeldEdge=true;	//设新切割边为焊缝线
		}
	}
	if(face_N==1)
	{	//判断切割面是否与被切割钢板相交,现在仅仅处理单面板的情况
		int iDelVertex=0;
		for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			if(pVertex->vertex.feature<0)
				iDelVertex++;
		}
		if(iDelVertex==pPlate->vertex_list.GetNodeNum()||iDelVertex==0)
		{	//板上所有顶点都在切割面的一侧，无法完成切割
			if(iDelVertex==pPlate->vertex_list.GetNodeNum())
			{	//钢板上所有顶点都在切割面一侧且所有点都应被删除时将feature修改为正值 wht 10-12-01
				for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
				{	
					if(pVertex->vertex.feature<0)
						pVertex->vertex.feature*=-1;
				}
			}
			if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Cut plane has no joints with plate 0X%X, unable to finish cutting!",pPlate->handle);
#else
				logerr.Log("切割面与钢板0X%X不相交，无法完成切割!",pPlate->handle);
#endif
			return FALSE;
		}
	}
	//去除被切割掉的非火曲顶点
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		if(face_N==1)
		{
			if(pVertex->vertex.feature<0)	//应该被切割掉的顶点
				pPlate->vertex_list.DeleteCursor();
		}
		else 
		{
			if(pVertex->vertex.feature<0&&abs(pVertex->vertex.feature)<10)	//应该被切割掉的非火曲点
				pPlate->vertex_list.DeleteCursor();
		}
	}
	//如果第一个顶点就被切割掉的话,上面的去除工作就会遗漏检测一个顶点
	pVertex=pPlate->vertex_list.GetFirst();
	if(pVertex&&pVertex->vertex.feature<0&&abs(pVertex->vertex.feature)<10)
		pPlate->vertex_list.DeleteCursor();
	
	if(pPlate->vertex_list.GetNodeNum()<3)
	{
		pPlate->vertex_list.Empty();
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("All plates are to be cut off!");
#else
			AfxMessageBox("钢板被全部切割掉!");
#endif
		return TRUE;
	}
	//重新设计被切割掉的火曲顶点
	int i,n=pPlate->vertex_list.GetNodeNum();
	pPlate->face_N=face_N;
	if(pPlate->vertex_list.GetFirst()->vertex.feature==2&&pPlate->face_N==1)
	{	//切割后新生成的单面板为原来双面板的第二面时调整板的坐标系
		pPlate->ucs.axis_z=pPlate->huoqufacenorm[0].normalized();
		pPlate->ucs.axis_x=pPlate->ucs.axis_y^pPlate->ucs.axis_z;
		normalize(pPlate->ucs.axis_x);
	}
	for(i=0;i<n;i++)
	{
		pVertex=&pPlate->vertex_list[i];
		if(face_N==1)
			pVertex->vertex.feature=face_N;
		if(pVertex->vertex.feature<-10)	//重新设计火曲点
		{
			pVertex->vertex.feature=abs(pVertex->vertex.feature);
			f3dPoint *pPrevPoint=&pPlate->vertex_list[(i+n-1)%n].vertex;
			f3dPoint *pNextPoint=&pPlate->vertex_list[(i+1)%n].vertex;
			pPrevPoint->z=pNextPoint->z=0;	//Z坐标归零否则求交可能失败 wht 11-01-19
			f3dPoint inters,inters2;
			Int3dpl(f3dLine(*pPrevPoint,*pNextPoint),pPlate->HuoQuLine[0],inters);
			GEPOINT xCutPlanePick=pPlate->ucs.TransPToCS(face_pos);
			GEPOINT xCutPlaneNorm=pPlate->ucs.TransVToCS(face_norm);
			Int3dlf(inters2,pPlate->HuoQuLine[0],xCutPlanePick,xCutPlaneNorm);
			if ((inters-inters2)*xCutPlaneNorm>0)
				inters=inters2;	//火曲线与切割面交点落在被切割区域 wjh-2019.9.19
			pVertex->vertex.Set(inters.x,inters.y,inters.z);
		}
	}
	//去除不在板范围内的螺栓引用
	for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		f3dPoint ls_pos=(*pLsRef)->ucs.origin;
		coord_trans(ls_pos,ucs,FALSE);
		if(ls_pos.z>0)
			pPlate->GetLsRefList()->DeleteCursor();
	}
	//移除不在板范围内的射线杆件	wht 10-11-30
	CDesignLjPartPara *pLjPara=NULL;
	if(pPlate->jdb_style!=4)
	{	//螺栓垫板中第一个连接构件其实是切割角钢，不能剔除
		for(pLjPara=pPlate->GetFirstLjPara();pLjPara;pLjPara=pPlate->GetNextLjPara())
		{
			//获得射线杆件螺栓引用列表
			CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(pLjPara->hPart,CLS_LINEPART);
			if(pLinePart==NULL)
				continue;
			CLsRefList *pLsRefList=NULL;
			if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//钢管
				CLDSParamPlate *pParamPlate=NULL;
				pParamPlate=((CLDSLineTube*)pLinePart)->GetParamPlate(pLjPara->start0_end1);
				if(pParamPlate)
					pLsRefList=pParamPlate->GetLsRefList();
			}
			else 
				pLsRefList=pLinePart->GetLsRefList();
			//螺栓引用列表为空或者该列表中的不包括钢板上的螺栓，则删除该射线杆件
			if(pLsRefList==NULL)
				pPlate->DeleteLjPara(pLjPara->hPart);
			else
			{
				CLsRef *pLsRef=NULL;
				for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{
					if(pLsRefList->FromHandle(pLsRef->GetLsPtr()->handle))
						break;	//螺栓引用列表中包括钢板上的螺栓，该射线杆件为钢板上的射线杆件
				}
				if(pLsRef==NULL)
					pPlate->DeleteLjPara(pLjPara->hPart);
			}
		}
		pPlate->GetLsRefList()->Clean();
	}
	pPlate->SetModified();
	pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return TRUE;
}

//对称切割钢板
//返回值为切割截断后钢板的句柄（如果是分割则返回新增的另一块钢板句柄） wjh-2015.5.14
long CLDSView::FinishCutPlate(CLDSPlate *pSrcPlate,CLDSPart *pDatumPart,
							  CCutPlateOptionDlg &cut_plate_dlg,BOOL bReCalCutFace)
{
	if(pSrcPlate==NULL||pDatumPart==NULL)
		return 0;
	f3dPoint face_pos,face_norm;
	if(bReCalCutFace)
	{	//1.重新计算切割面法线以及切割面基准点
		if(pDatumPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate *pDatumPlate=(CLDSPlate*)pDatumPart;
			face_norm=pDatumPlate->ucs.axis_z;
			face_pos=pDatumPlate->ucs.origin+pDatumPlate->ucs.axis_z*pDatumPlate->m_fNormOffset;
			if(cut_plate_dlg.m_iTopFace==0)
			{	//顶面负法线方向切割
				face_norm*=-1.0;
				face_pos+=pDatumPlate->ucs.axis_z*pDatumPlate->GetThick();
			}
		}
		else if(pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pDatumPart;
			double W=0;
			if(pParamPlate->m_iParamType==TYPE_FL||
				pParamPlate->m_iParamType==TYPE_FLG||
				pParamPlate->m_iParamType==TYPE_FLR||
				pParamPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
				face_pos=pParamPlate->ucs.origin;
			else if(pParamPlate->m_iParamType==TYPE_UEND)
			{
				pParamPlate->GetDesignItemValue('W',&W);
				face_pos=pParamPlate->ucs.origin-pParamPlate->ucs.axis_z*(0.5*W+pParamPlate->thick);
			}
			else 
				face_pos=pParamPlate->ucs.origin-pParamPlate->ucs.axis_z*(0.5*pParamPlate->thick);
			if(cut_plate_dlg.m_iTopFace==0)
			{	//顶面负法线方向切割
				face_norm=-pParamPlate->ucs.axis_z;
				face_pos+=pParamPlate->ucs.axis_z*cut_plate_dlg.m_fTopBtmSpace;
			}
			else //底面正法线方向切割
				face_norm=pDatumPart->ucs.axis_z;
		}
		else
		{
			face_pos=pDatumPart->ucs.origin;
			if(cut_plate_dlg.m_iTopFace==0)
			{	//顶面负法线方向切割
				face_norm=-pDatumPart->ucs.axis_z;
				face_pos+=pDatumPart->ucs.axis_z*pDatumPart->GetThick();
			}
			else //底面正法线方向切割
				face_pos=pDatumPart->ucs.axis_z;
		}
	}
	else
	{
		face_pos.Set(cut_plate_dlg.m_fCutFacePickX,cut_plate_dlg.m_fCutFacePickY,cut_plate_dlg.m_fCutFacePickZ);
		face_norm.Set(cut_plate_dlg.m_fCutFaceNormX,cut_plate_dlg.m_fCutFaceNormY,cut_plate_dlg.m_fCutFaceNormZ);
	}
	Ta.BeginUndoListen();
	pSrcPlate->designInfo.m_bEnableFlexibleDesign=FALSE;	//关闭柔性化设计
	if(cut_plate_dlg.m_bWeld)	//设置焊接父构件
		pSrcPlate->m_hPartWeldParent=pDatumPart->handle;
	BOOL bFinishCutPlate=FALSE;
	CLDSPlate *pNewPlate=NULL;
	if(cut_plate_dlg.m_bSaveAnotherPlate)
	{	//克隆钢板
		pNewPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
		long hNew=pNewPlate->handle;	//保存钢板句柄
		pSrcPlate->CloneTo(*pNewPlate);	//克隆钢板 wht 11-01-07
		pNewPlate->handle=hNew;			//更新钢板句柄
		if(cut_plate_dlg.m_bWeld)
		{	//设置焊接父构件
			pSrcPlate->m_hPartWeldParent=pDatumPart->handle;
			pNewPlate->m_hPartWeldParent=pDatumPart->handle;
			pNewPlate->m_bWeldPart=pSrcPlate->m_bWeldPart=TRUE;
		}
		pNewPlate->designInfo.iWeldLineStyle=pSrcPlate->designInfo.iWeldLineStyle=2;
		pNewPlate->relativeObjs.Empty();//清空关联构件列表
		pNewPlate->EmptyLsRef();		//清空螺栓引用
		//复制螺栓引用
		for(CLsRef *pLsRef=pSrcPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcPlate->GetNextLsRef())
			pNewPlate->AppendLsRef(*pLsRef);
		pNewPlate->SetModified();
		pNewPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pNewPlate->GetSolidPartObject());
		g_pSolidDraw->Draw();
		
		if(cut_plate_dlg.m_bWeld)	//设置焊接父构件
			pNewPlate->m_hPartWeldParent=pDatumPart->handle;
		//基准钢板为双面板且选择了双面切割 wht 11-01-20
		if(pDatumPart->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pDatumPart)->face_N==2&&cut_plate_dlg.m_iFaceNo==2)
			bFinishCutPlate=CutPlateByPlate(pSrcPlate,(CLDSPlate*)pDatumPart,cut_plate_dlg.m_iTopFace==0,cut_plate_dlg.m_fOffsetDist,TRUE,cut_plate_dlg.m_bWeld);
		else
			bFinishCutPlate=CutPlateByPlane(pSrcPlate,face_pos,face_norm,cut_plate_dlg.m_fOffsetDist,TRUE,cut_plate_dlg.m_bWeld);
		//更新切割面基准点以及切割面法线方向
		face_pos+=face_norm*cut_plate_dlg.m_fTopBtmSpace;
		face_norm*=-1.0;
		if(bFinishCutPlate)
		{	//基准钢板为双面板且选择了双面切割 wht 11-01-20
			if(pDatumPart->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pDatumPart)->face_N==2&&cut_plate_dlg.m_iFaceNo==2)
				bFinishCutPlate=CutPlateByPlate(pNewPlate,(CLDSPlate*)pDatumPart,cut_plate_dlg.m_iTopFace!=0,cut_plate_dlg.m_fOffsetDist,TRUE,cut_plate_dlg.m_bWeld);
			else
				bFinishCutPlate=CutPlateByPlane(pNewPlate,face_pos,face_norm,cut_plate_dlg.m_fOffsetDist,TRUE,cut_plate_dlg.m_bWeld);
		}
		if(!bFinishCutPlate&&pNewPlate)
		{	//切割面与钢板不相交,无法完成切割,删除已复制的钢板
			console.DispPartSet.DeleteNode(pNewPlate->handle);
			g_pSolidDraw->DelEnt(pNewPlate->handle);
			console.DeletePart(pNewPlate->handle);
			Ta.EndUndoListen();
			return 0;
		}
	}
	else	//切割钢板
	{
		//基准钢板为双面板且选择了双面切割 wht 11-01-20
		if(pDatumPart->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pDatumPart)->face_N==2&&cut_plate_dlg.m_iFaceNo==2)
			bFinishCutPlate=CutPlateByPlate(pSrcPlate,(CLDSPlate*)pDatumPart,cut_plate_dlg.m_iTopFace==0,cut_plate_dlg.m_fOffsetDist,TRUE,cut_plate_dlg.m_bWeld);
		else
			bFinishCutPlate=CutPlateByPlane(pSrcPlate,face_pos,face_norm,cut_plate_dlg.m_fOffsetDist,TRUE,cut_plate_dlg.m_bWeld);
	}
	Ta.EndUndoListen();
	if(!bFinishCutPlate)
		return 0;
	else if(pNewPlate)
		return pNewPlate->handle;
	else
		return pSrcPlate->handle;
}

int CLDSView::CutPlate()
{	//切割钢板,保留切割生成的两块或者一块
	CString cmdStr,errCmdStr;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSPlate *pSrcPlate=NULL,*pNewPlate=NULL;
	PARTSET cutPlateSet;
	CSuperSmartPtr<CLDSPart>pDatumPlate;
	try
	{
		CUndoOperObject undo(&Ta,true);
		//选择需要切割的钢板
		long *id_arr,nCount=0;
		if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
		{
			pSrcPlate=(CLDSPlate*)console.FromPartHandle(id_arr[0],CLS_PLATE);
			cutPlateSet.append(pSrcPlate);
		}
		if(pSrcPlate==NULL||(pSrcPlate&&pSrcPlate->GetClassTypeId()!=CLS_PLATE))
		{
			GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECTINDEX_PLATE),CLS_PLATE);
			scr_part_para.disp_type=DISP_SOLID;
#ifdef AFX_TARG_ENU_ENGLISH
			scr_part_para.cmdStr="CutPlate Please choose plates to be cut:";
			scr_part_para.cmdErrorFeedbackStr="No proper parts selected, please plates to be cut:";
#else
			scr_part_para.cmdStr="CutPlate 请选择需要切割的钢板:";
			scr_part_para.cmdErrorFeedbackStr="没有选中合适的构件，请重新选择需要切割的钢板:";
#endif
			scr_part_para.nResultObjs=2;
			if(!GetPartsFromScr(scr_part_para))
				return 0; 
			for(int k=0;k<scr_part_para.nResultObjs;k++)
			{
				if(scr_part_para.pResultObjsArr[k]&&scr_part_para.pResultObjsArr[k]->GetClassTypeId()==CLS_PLATE)
					cutPlateSet.append((CLDSPart*)scr_part_para.pResultObjsArr[k]);
			}
			if(scr_part_para.nResultObjs>0)
			{
				if(scr_part_para.pResultObjsArr[0]&&scr_part_para.pResultObjsArr[0]->GetClassTypeId()==CLS_PLATE)
					pSrcPlate=(CLDSPlate*)scr_part_para.pResultObjsArr[0];
			}
			if(pSrcPlate==NULL)
				return 0;
		}
		//选择用来切割钢板的基准钢板或直接输入切割面
		BOOL bInputPlane=FALSE;	//输入切割面
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CutPlate Please select datum plate to be used for cutting plate, or press enter to input cutting plane directly:");
		errCmdStr.Format("CutPlate Two plates are parallel, please reselect the datum plate used to cut plate, or press enter to input cutting plane directly:");
#else
		cmdStr.Format("CutPlate 请选择用来切割钢板的基准钢板,或直接按回车键输入切割面:");
		errCmdStr.Format("CutPlate 两钢板平行，请重新选择用来切割钢板的基准钢板，或直接按回车键输入切割面:");
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		DWORD dwhObj=0,dwExportFlag=0;
		CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE));
		while(true)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				return 0;
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;
			if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
			{	//未选择构件，则指定切割面
				bInputPlane=TRUE;
				break;
			}	
			CLDSPart* pPart=console.FromPartHandle(dwhObj);
			if((pPart && pPart->GetClassTypeId()==CLS_PLATE)||(pPart && pPart->GetClassTypeId()==CLS_PARAMPLATE))
			{
				pDatumPlate=pPart;
				if(fabs(pDatumPlate->ucs.axis_z*pSrcPlate->ucs.axis_z)>EPS_COS)
					pCmdLine->FillCmdLine(errCmdStr,"");
				else
					break;
			}
			else
				pCmdLine->FillCmdLine(cmdStr,"");
		}
		long hDatumPlate=0;
		if(pDatumPlate.IsHasPtr())
			hDatumPlate=pDatumPlate->handle;
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",hDatumPlate));
		g_pSolidDraw->SetEntSnapStatus(hDatumPlate);
		//切割面基准点以及切割面法线
		if(bInputPlane)
		{	//输入切割面
			g_pSolidDraw->ReleaseSnapStatus();
			static CDefFaceDlg	def_face_dlg;
			def_face_dlg.m_bCutPlate=TRUE;	//切割钢板
			if(def_face_dlg.DoModal()!=IDOK)
			{	
				pCmdLine->CancelCmdLine();
				return 0;
			}
			f3dPoint face_pos,face_norm;
			face_pos.Set(def_face_dlg.m_fBasePosX,def_face_dlg.m_fBasePosY,def_face_dlg.m_fBasePosZ);
			face_norm.Set(def_face_dlg.m_fFaceNormX,def_face_dlg.m_fFaceNormY,def_face_dlg.m_fFaceNormZ);
			face_pos+=face_norm*def_face_dlg.m_fBaseNormOffsetDist;
			//用指定面切割钢板
			if(is_zero_vector(face_norm))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Specified cut plane's normals is empty, illegal cut plane!");
#else
				AfxMessageBox("指定分割面法线为空,为非法分割面!");
#endif
				pCmdLine->CancelCmdLine();
				return 0;
			}
			for(pSrcPlate=(CLDSPlate*)cutPlateSet.GetFirst();pSrcPlate;pSrcPlate=(CLDSPlate*)cutPlateSet.GetNext())
			{
				if(pSrcPlate->face_N>3)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("over-three-plane connecting plate can't be cut");
#else
					AfxMessageBox("不能分割多于三面的连接钢板");
#endif
					continue;
				}
				else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pSrcPlate->dwPermission))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("No permission to modify the connecting plate!");
#else
					AfxMessageBox("没有此连接板的修改权限！");
#endif
					continue;
				}
				//指定切割面切割钢板时支持保留另一面的钢板 wht 11-05-30
				Ta.BeginUndoListen();
				pSrcPlate->designInfo.m_bEnableFlexibleDesign=FALSE;	//关闭柔性化设计
				if(def_face_dlg.m_bSaveAnotherPlate)
				{
					BOOL bFinishCutPlate=FALSE;
					//克隆钢板
					CLDSPlate *pNewPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
					long hNew=pNewPlate->handle;	//保存钢板句柄
					pSrcPlate->CloneTo(*pNewPlate);	//克隆钢板 wht 11-01-07
					pNewPlate->handle=hNew;			//更新钢板句柄
					pNewPlate->relativeObjs.Empty();//清空关联构件列表
					pNewPlate->EmptyLsRef();		//清空螺栓引用
					//复制螺栓引用
					for(CLsRef *pLsRef=pSrcPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcPlate->GetNextLsRef())
						pNewPlate->AppendLsRef(*pLsRef);
					pNewPlate->SetModified();
					pNewPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
					g_pSolidDraw->NewSolidPart(pNewPlate->GetSolidPartObject());
					g_pSolidDraw->Draw();
					
					bFinishCutPlate=CutPlateByPlane(pSrcPlate,face_pos,face_norm,0,TRUE,FALSE);
					//更新切割面基准点以及切割面法线方向
					face_pos-=face_norm*def_face_dlg.m_fBaseNormOffsetDist*2;
					face_norm*=-1.0;
					if(bFinishCutPlate)
						bFinishCutPlate=CutPlateByPlane(pNewPlate,face_pos,face_norm,0,TRUE,FALSE);
					if(!bFinishCutPlate&&pNewPlate)
					{	//切割面与钢板不相交,无法完成切割,删除已复制的钢板
						console.DispPartSet.DeleteNode(pNewPlate->handle);
						g_pSolidDraw->DelEnt(pNewPlate->handle);
						console.DeletePart(pNewPlate->handle);
						Ta.EndUndoListen();
						return 0;
					}
				}
				else	//切割钢板
					CutPlateByPlane(pSrcPlate,face_pos,face_norm,0,TRUE,FALSE);
				Ta.EndUndoListen();
			}
		}
		else
		{
			static CCutPlateOptionDlg cut_plate_dlg;
			if(pDatumPlate.IsHasPtr()&&pDatumPlate->GetClassTypeId()==CLS_PLATE)
				cut_plate_dlg.m_pDatumPart=pDatumPlate.PlatePointer();
			else if(pDatumPlate.IsHasPtr()&&pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE)
				cut_plate_dlg.m_pDatumPart=pDatumPlate.ParamPlatePointer();
			//显示切割钢板坐标系 用来判断顶/底面
			//显示竖直板的坐标系用来区分内侧孔/外侧孔
			CWorkPlaneSketchLifeObject workPlaneSketchLife;
			workPlaneSketchLife.WorkPlaneSketch(pDatumPlate->ucs.origin,pDatumPlate->ucs.axis_z,"切割板顶面");
			workPlaneSketchLife.WorkPlaneSketch(pDatumPlate->ucs.origin,-1*pDatumPlate->ucs.axis_z,"切割板底面");
			if(cut_plate_dlg.DoModal()!=IDOK)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			f3dPoint face_norm(cut_plate_dlg.m_fCutFaceNormX,cut_plate_dlg.m_fCutFaceNormY,cut_plate_dlg.m_fCutFaceNormZ);
			if(is_zero_vector(face_norm))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Specified cut plane's normals is empty, illegal cut plane!");
#else
				AfxMessageBox("指定分割面法线为空,为非法分割面!");
#endif
				pCmdLine->CancelCmdLine();
				return 0;
			}
			long hNewCutPlate=0;
			for(pSrcPlate=(CLDSPlate*)cutPlateSet.GetFirst();pSrcPlate;pSrcPlate=(CLDSPlate*)cutPlateSet.GetNext())
			{
				if(pSrcPlate->face_N>3)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("over-three-plane connecting plate can't be cut");
#else
					AfxMessageBox("不能分割多于三面的连接钢板");
#endif
					continue;
				}
				else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pSrcPlate->dwPermission))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("No permission to modify the connecting plate!");
#else
					AfxMessageBox("没有此连接板的修改权限！");
#endif
					continue;
				}
				hNewCutPlate=FinishCutPlate(pSrcPlate,pDatumPlate,cut_plate_dlg,FALSE);
				if(!cut_plate_dlg.m_bMirCut)
				{
					if(cut_plate_dlg.m_bSaveAnotherPlate)
					{
						if(hNewCutPlate)
#ifdef AFX_TARG_ENU_ENGLISH
							MessageBox("plate cutting succeeded!");
						else
							MessageBox("plate cutting failed!");
#else
							MessageBox("钢板分割成功!");
						else
							MessageBox("钢板分割失败!");
#endif
					}
					else 
					{
						if(hNewCutPlate)
#ifdef AFX_TARG_ENU_ENGLISH
							MessageBox("plate cutting succeed!ed");
						else
							MessageBox("plate cutting failed!");
#else
							MessageBox("钢板切割成功!");
						else
							MessageBox("钢板切割失败!");
#endif
					}
				}
			}
			GEPOINT srcPlaneNorm=pDatumPlate->ucs.axis_z;
			pSrcPlate=(CLDSPlate*)cutPlateSet.GetFirst();
			if(cut_plate_dlg.m_bMirCut&&pSrcPlate&&cutPlateSet.GetNodeNum()==1)
			{	//对称切割钢板 wht 11-01-07	
				CMirMsgDlg mir_dlg;
				CLDSPartPtr plateArr[8]={NULL},plateArr2[8]={NULL};
				plateArr[0]=pSrcPlate;
				plateArr2[0]=Ta.Parts.FromHandle(hNewCutPlate);
				long retCodeArr[7]={0};	//用来记录对称切割结果
				if(mir_dlg.DoModal()==IDOK)
				{
					CLDSPlate *src_mirplate[4]={NULL};
					CLDSPart *datum_part[4]={NULL};
					src_mirplate[0]=pSrcPlate;
					datum_part[0]=pDatumPlate;
					int flag_arr[4]={0,1,2,4};
					for(int i=1;i<4;i++)
					{
						if(mir_dlg.mirmsg.axis_flag&flag_arr[i])
						{
							MIRMSG submir=mir_dlg.mirmsg.GetSubMirItem(i);
							GEPOINT mirPlaneNorm=submir.MirVector(srcPlaneNorm);
							datum_part[i]=pDatumPlate->GetMirPart(submir);
							src_mirplate[i]=(CLDSPlate*)pSrcPlate->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(i));
							if(src_mirplate[i]&&datum_part[i])
							{
								retCodeArr[i-1]=FinishCutPlate(src_mirplate[i],datum_part[i],cut_plate_dlg,TRUE);
								if(mirPlaneNorm*datum_part[i]->ucs.axis_z>0)
								{
									plateArr[i]=src_mirplate[i];
									plateArr2[i]=Ta.Parts.FromHandle(retCodeArr[i-1]);
								}
								else
								{
									plateArr[i]=Ta.Parts.FromHandle(retCodeArr[i-1]);
									plateArr2[i]=src_mirplate[i];
								}
							}
						}
					}
					if(mir_dlg.mirmsg.axis_flag&8)
					{
						mir_dlg.mirmsg.axis_flag=8;
						for(int i=0;i<4;i++)
						{
							if(src_mirplate[i]==NULL||datum_part[i]==NULL)
								continue;
							CLDSPart *pMirPart=NULL;
							CLDSPlate *pMirPlate=NULL;
							GEPOINT mirPlaneNorm=srcPlaneNorm;
							if(i>0)
							{
								MIRMSG submir=mir_dlg.mirmsg;
								submir.axis_flag=8+MIRMSG(i).axis_flag;
								mirPlaneNorm=submir.MirVector(srcPlaneNorm);
							}
							for(int j=0;j<mir_dlg.mirmsg.array_num;j++)
							{
								if(j==0)
								{
									pMirPart=datum_part[i]->GetMirPart(mir_dlg.mirmsg);
									pMirPlate=(CLDSPlate*)src_mirplate[i]->GetMirPart(mir_dlg.mirmsg);
								}
								else
								{
									pMirPart=pMirPart->GetMirPart(mir_dlg.mirmsg);
									pMirPlate=(CLDSPlate*)pMirPlate->GetMirPart(mir_dlg.mirmsg);
								}
								mirPlaneNorm=mir_dlg.mirmsg.MirVector(mirPlaneNorm);
								if(pMirPart&&pMirPlate)
								{
									retCodeArr[3+i]=FinishCutPlate(pMirPlate,pMirPart,cut_plate_dlg,TRUE);
									if(mirPlaneNorm*pMirPart->ucs.axis_z>0)
									{
										plateArr[i+4]=pMirPlate;
										plateArr2[i+4]=Ta.Parts.FromHandle(retCodeArr[3+i]);
									}
									else
									{
										plateArr[i+4]=Ta.Parts.FromHandle(retCodeArr[3+i]);
										plateArr2[i+4]=pMirPlate;
									}
								}
								else
									break;
							}
						}
					}
				}
				int i;
				for(i=0;i<7;i++)
				{
					if(!retCodeArr[i])
						break;
				}
				int mapinfo[8][8]={
					 0, 1, 2, 4, 8, 9,10,12,
					 1, 0, 4, 2, 9, 8,12,10,
					 2, 4, 0, 1,10,12, 8, 9,
					 4, 2, 1, 0,12,10, 9, 8,
					 8, 9,12,10, 0, 1, 4, 2,
					 9, 8,10,12, 1, 0, 2, 4,
					12,10, 8, 9, 4, 2, 0, 1,
					10,12, 9, 8, 2, 4, 1, 0,
				};
				for(int j=0;j<8;j++)
				{
					int k;
					RELATIVE_OBJECT* pRelaObj;
					if(plateArr[j]!=NULL)
					{
						for(pRelaObj=plateArr[j]->relativeObjs.GetFirst();pRelaObj;pRelaObj=plateArr[j]->relativeObjs.GetNext())
						{
							CLDSPart* pRelaPart=plateArr[j]->BelongModel()->FromPartHandle(pRelaObj->hObj);
							if(pRelaPart)
							{
								pRelaPart->relativeObjs.DeleteNode(plateArr[j]->handle);
								plateArr[j]->relativeObjs.DeleteNode(pRelaPart->handle);
							}
						}
						plateArr[j]->relativeObjs.Empty();
						for(k=0;k<8;k++)
						{
							if(j==k || plateArr[k]==NULL)
								continue;
							pRelaObj=plateArr[j]->relativeObjs.Add(plateArr[k]->handle);
							pRelaObj->mirInfo.axis_flag=mapinfo[j][k];
							if(pRelaObj->mirInfo.axis_flag&0x08)
								pRelaObj->mirInfo.rotate_angle=90;
							if(j>3&&k<4)
								pRelaObj->mirInfo.rotate_angle=-90;
						}
					}
					if(plateArr2[j]!=NULL)
					{
						for(pRelaObj=plateArr2[j]->relativeObjs.GetFirst();pRelaObj;pRelaObj=plateArr2[j]->relativeObjs.GetNext())
						{
							CLDSPart* pRelaPart=plateArr2[j]->BelongModel()->FromPartHandle(pRelaObj->hObj);
							if(pRelaPart)
							{
								pRelaPart->relativeObjs.DeleteNode(plateArr2[j]->handle);
								plateArr2[j]->relativeObjs.DeleteNode(pRelaPart->handle);
							}
						}
						plateArr2[j]->relativeObjs.Empty();
						for(k=0;k<8;k++)
						{
							if(j==k || plateArr2[k]==NULL)
								continue;
							RELATIVE_OBJECT* pRelaObj=plateArr2[j]->relativeObjs.Add(plateArr2[k]->handle);
							pRelaObj->mirInfo.axis_flag=mapinfo[j][k];
							if(pRelaObj->mirInfo.axis_flag&0x08)
								pRelaObj->mirInfo.rotate_angle=90;
							if(j>3&&k<4)
								pRelaObj->mirInfo.rotate_angle=-90;
						}
					}
				}
				if(i<7)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox("Symmetric cutting plate succeeded!");
				else
					MessageBox("symmetric cutting plate not fully successfully!");
#else
					MessageBox("对称切割钢板成功!");
				else
					MessageBox("对称切割钢板未完全完成!");
#endif
			}
		}
		Invalidate(FALSE);
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
		return 0;
	}
	g_pSolidDraw->ReleaseSnapStatus();
	CutPlate();	//Command("CutPlate");
	return 0;
}
//<DEVELOP_PROCESS_MARK nameId="CLDSView::OnCutPlateByColumnFace">
BOOL IsCanCutPlateByColumnFace(CLDSDbObject *pDbObj)
{
	if(pDbObj&&pDbObj->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pDbObj;
		if( pParamPlate->m_iParamType==TYPE_BASEPLATE||
			pParamPlate->m_iParamType==TYPE_ELBOWPLATE||
			pParamPlate->m_iParamType==TYPE_RIBPLATE)
			return TRUE;
		else
			return FALSE;
	}
	else if(pDbObj&&pDbObj->GetClassTypeId()==CLS_PLATE)
		return TRUE;
	else
		return FALSE;
}
void CLDSView::OnCutPlateByColumnFace() 
{	//圆柱面切割钢板
	m_nPrevCommandID=ID_CUT_PLATE_BY_COLUMN_FACE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat cylindrical-plane cut plate";
#else
	m_sPrevCommandName="重复圆柱面切割钢板";
#endif
	CString cmdStr;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSGeneralPlate *pGeneralPlate=NULL;
	CLDSLineTube *pCutTube=NULL;
	CLDSSphere *pSphere=NULL;
	CLogErrorLife loglife;
	try
	{
		PARTSET plateSet;
		long *id_arr,i,n=g_pSolidSnap->GetLastSelectEnts(id_arr);
		for(i=0;i<n;i++)
		{
			pGeneralPlate=(CLDSGeneralPlate*)console.FromPartHandle(id_arr[i],CLS_PLATE,CLS_PARAMPLATE);
			if(IsCanCutPlateByColumnFace(pGeneralPlate))
				plateSet.append(pGeneralPlate);
		}
		if(plateSet.GetNodeNum()==0)
		{
			GET_SCR_PART_PARA part_para(GetSingleWord(SELECTINDEX_PLATE|SELECTINDEX_PARAMPLATE),CLS_PLATE,CLS_PARAMPLATE);
			part_para.disp_type=DISP_SOLID;
#ifdef AFX_TARG_ENU_ENGLISH
			part_para.cmdStr="CutByColumnFace please select plate to be cut:";
			part_para.cmdErrorFeedbackStr="No proper parts selected, please reselect plate to be cut:";
#else
			part_para.cmdStr="CutByColumnFace 请选择需要切割的钢板:";
			part_para.cmdErrorFeedbackStr="没有选中合适的构件，请重新选择需要切割的钢板:";
#endif
			if(!GetPartsFromScr(part_para))
				return; 
			CLDSDbObject *pObj=part_para.pResultObjsArr[0];
			if(IsCanCutPlateByColumnFace(pObj))
				plateSet.append((CLDSGeneralPlate*)pObj);
			else
				return;
		}
		//选择要圆柱切割面的基准钢管
		CLDSPart* pPart=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("CutByColumnFace Please select cylindrical-plane datum tube or sphere used to cut plate:","");
#else
		pCmdLine->FillCmdLine("CutByColumnFace 请选择用于切割钢板的圆曲面基准钢管或球体:","");
#endif
		DWORD dwhObj=0,dwExportFlag=0;
		CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINETUBE)|GetSingleWord(SELECTINDEX_SPHERE));
		while(true)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				return;
			SELOBJ obj(dwhObj,dwExportFlag);
			pPart=console.FromPartHandle(obj.hRelaObj);
			if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
			{
				pCutTube=(CLDSLineTube*)pPart;
				break;
			}
			else if(pPart&&pPart->GetClassTypeId()==CLS_SPHERE)
			{
				pSphere=(CLDSSphere*)pPart;
				break;
			}
		}	
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pPart->handle));
		g_pSolidDraw->SetEntSnapStatus(pPart->handle);
		double fMinDist=1;
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("CutByColumnFace Please enter the clearance value between the newly generated plate and cutting surface<1>:","");
#else
		pCmdLine->FillCmdLine("CutByColumnFace 请输入切割后生成的新钢板与切割曲面的间隙值<1>:","");
#endif
		while(1)
		{
			if(pCmdLine->GetStrFromCmdLine(cmdStr))
			{
				if(cmdStr.GetLength()>0)
					fMinDist=atoi(cmdStr);
				else
					fMinDist=1;
				if(fMinDist>0)
					break;
			}
			else
				return;
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("CutByColumnFace error in input, Please enter the clearance value between the newly generated plate and cutting surface<1>:","");
#else
			pCmdLine->FillCmdLine("CutByColumnFace 输入错误，请重新输入切割后生成的新钢板与切割曲面的间隙值<1>:","");
#endif
		}
		double cut_radius;
		f3dPoint column_ucsaxis;	//待切割钢板相对坐标系下的圆柱面圆心点及圆柱方向
		f3dPoint column_center,column_norm;
		f3dLine column_ucsline,column_line;
		if(pCutTube!=NULL)
		{
			cut_radius=pCutTube->GetDiameter()*0.5+fMinDist;
			column_norm=pCutTube->End()-pCutTube->Start();
			column_line.startPt=pCutTube->Start();
			column_line.endPt=pCutTube->End();
		}
		pCmdLine->FinishCmdLine();
		normalize(column_norm);
		CUndoOperObject undo(&Ta,true);
		CLDSPlate *pPlate=NULL;
		for(pGeneralPlate=(CLDSGeneralPlate*)plateSet.GetFirst();pGeneralPlate;pGeneralPlate=(CLDSGeneralPlate*)plateSet.GetNext())
		{
			if(pGeneralPlate->GetClassTypeId()==CLS_PLATE)
				pPlate=(CLDSPlate*)pGeneralPlate;
			else if(pGeneralPlate->GetClassTypeId()==CLS_PARAMPLATE)
				pPlate=ConvertParamPlateToPlate((CLDSParamPlate*)pGeneralPlate);
			if(pPlate==NULL)
				continue;
			if(pPlate->GetFaceN()>2)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("plate 0X%X{%s} is not singular-plane plate, cutting over-two-plane plate with circular Surface Cutting is not supported temporarily", pPlate->handle,(char*)pPlate->GetPartNo());
#else
				logerr.Log("钢板0X%X{%s}非单面板，暂时不支持曲面切割多于二面的连接钢板",pPlate->handle,(char*)pPlate->GetPartNo());
#endif
				continue;
			}
			else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPlate->dwPermission))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("plate 0X%X{%s} No permission to modify the connecting plate!",pPlate->handle,(char*)pPlate->GetPartNo());
#else
				logerr.Log("钢板0X%X{%s}没有此连接板的修改权限！",pPlate->handle,(char*)pPlate->GetPartNo());
#endif
				continue;
			}
			if(pSphere!=NULL)
			{
				column_norm=pPlate->ucs.axis_z;
				f3dPoint sphere_org=column_center=pSphere->ucs.origin;
				f3dPoint prjpos=pPlate->ucs.origin+pPlate->ucs.axis_z*pPlate->m_fNormOffset;
				project_point(column_center,prjpos,pPlate->ucs.axis_z);
				f3dPoint prjvec=sphere_org-column_center;
				double prjdist=prjvec*pPlate->ucs.axis_z;
				if(prjdist>=0&&prjdist<=pPlate->GetThick())
					cut_radius=pSphere->D*0.5;	//球心在钢板底面之上顶面之下,此时以球径为切割圆曲面半径
				else
				{	//球心在钢板之外
					if(prjdist>0)
					{	//球心在钢板顶面之上
						prjdist-=pPlate->GetThick();
						column_center+=pPlate->ucs.axis_z*pPlate->GetThick();
					}
					//else	//球心钢板底面之下
					double R2=0.25*pSphere->D*pSphere->D-prjdist*prjdist;
					if(R2>0)
						cut_radius=sqrt(R2);
					else
					{
#ifdef AFX_TARG_ENU_ENGLISH
						logerr.Log("the distance between plate 0X%X{%s} and selected sphere is too far,  Circular Surface Cutting  failure!",pPlate->handle,(char*)pPlate->GetPartNo());
#else
						logerr.Log("钢板0X%X{%s}与所选球体距离过远不相交，曲面切割失败！",pPlate->handle,(char*)pPlate->GetPartNo());
#endif
						continue;
					}
				}
				column_line.startPt=column_center;
				column_line.endPt =column_center+column_norm*100;
				cut_radius+=fMinDist;
			}
			f3dPoint center_pick,elipse_center_pick;	//椭圆拾取点为绝对坐标，且不应该向板底面进行投影（即不能截取相对Z坐标） wjh 2011.11.27
			Int3dlf(center_pick,column_line.startPt,column_norm,pPlate->ucs.origin+pPlate->ucs.axis_z*pPlate->m_fNormOffset,pPlate->ucs.axis_z);
			elipse_center_pick=center_pick;
			pPlate->designInfo.iProfileStyle0123=3;	//关闭柔性化设计
			column_ucsaxis=column_norm;
			column_ucsline=column_line;
			normalize(pPlate->ucs.axis_z);
			normalize(pPlate->huoqufacenorm[0]);
			normalize(pPlate->huoqufacenorm[1]);
			vector_trans(column_ucsaxis,pPlate->ucs,FALSE);
			coord_trans(column_ucsline.startPt,pPlate->ucs,FALSE);
			coord_trans(column_ucsline.endPt,pPlate->ucs,FALSE);
			coord_trans(center_pick,pPlate->ucs,FALSE);
			//center_pick.z=0;
			PROFILE_VERTEX *pPrevVertex,*pVertex;
			//计算顶点表中所有顶点是否需要被切割掉
			BOOL bWholeCir=TRUE;	//圆柱面在钢板中间切割出一个整圆
			SCOPE_STRU scope;
			for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			{
				scope.VerifyVertex(pVertex->vertex);	//钢板轮廓区
				double dd=DistOf3dPtLine(pVertex->vertex,column_ucsline.startPt,column_ucsline.endPt);
				if(dd<cut_radius)
				{
					pVertex->xTag.liTagValue=0;	//0:表示需要被切割掉
					bWholeCir=FALSE;	//非整圆
				}
				else
					pVertex->xTag.liTagValue=1;	//1:表示需要保留
			}
			scope.fMinZ=center_pick.z-1;	//消除法向坐标分量影响
			scope.fMaxZ=center_pick.z+1;
			if(!scope.IsIncludePoint(center_pick))
				bWholeCir=FALSE;	//非整圆
			long hWeldParentPart=(pCutTube!=NULL)?pCutTube->handle:pSphere->handle;
			if(pPlate->m_hPartWeldParent!=0)
				pPlate->m_hPartWeldParent=hWeldParentPart;

			// 为排除新生成顶点尤其是PROFILE_VERTEX::type取值对过往顶点的干扰，须重建vertex_list
			// 而不是在原链表上进行修改 wjh-2013.11.03
			f3dPoint inters1,inters2;
			PROFILE_VERTEX* pNewVertex;
			ATOM_LIST<PROFILE_VERTEX> vertex_list;
			pPrevVertex =pPlate->vertex_list.GetTail();
			for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			{
				if(pVertex->xTag.liTagValue==0)
				{
					if(pPrevVertex->xTag.liTagValue==1)
					{	//需要删除当前顶点, 且前一边需要插入新顶点
						int ret=Int3dlc(&inters1,&inters2,pPrevVertex->vertex,pVertex->vertex-pPrevVertex->vertex,
							column_ucsline.startPt,column_ucsline.endPt,cut_radius,cut_radius);
						if(ret<=0)
							continue;
						if(ret==2)
						{
							f3dPoint edge_vec=pVertex->vertex-pPrevVertex->vertex;
							normalize(edge_vec);
							f3dPoint line_vec1=inters1-pPrevVertex->vertex;
							f3dPoint line_vec2=inters2-pPrevVertex->vertex;
							double edge_len = DISTANCE(pPrevVertex->vertex,pVertex->vertex);
							double dd1 = line_vec1*edge_vec;
							double dd2=line_vec2*edge_vec;
							if(dd1<0||dd1>edge_len||(dd2>0&&dd2<dd1))
								inters1=inters2;
						}
						pNewVertex=vertex_list.append(PROFILE_VERTEX(inters1));
						pNewVertex->vertex.feature=pPrevVertex->vertex.feature;
						pNewVertex->type=2;
						pNewVertex->center=center_pick;
						pNewVertex->xTag.liTagValue=pVertex->xTag.liTagValue;	//记录被删除节点的删除标志位的状态
						pNewVertex->radius=-cut_radius;	//圆柱面切割的都是内圆弧
						pNewVertex->m_bWeldEdge=TRUE;	//被圆柱面切割时工艺上一般都需要焊接
						pNewVertex->weldline.hWeldMotherPart=hWeldParentPart;
						if((pPrevVertex->vertex.feature==1&&fabs(pPlate->ucs.axis_z*column_norm)<EPS_COS) ||
							(pPrevVertex->vertex.feature==2&&fabs(pPlate->huoqufacenorm[0]*column_norm)<EPS_COS))
						{
							pNewVertex->type=3;
							pNewVertex->column_norm=column_norm;
							pNewVertex->center=elipse_center_pick;	//column_line.startPt;
						}
					}
					if(pVertex->vertex.feature>10)	//火曲顶点　
					{	//需要将当前顶点(火曲点)更新坐标
						int iFace=pVertex->vertex.feature%10;
						f3dPoint huoqu_inters1,huoqu_inters2;
						PROFILE_VERTEX* pOtherHuoquVertex=NULL;
						BOOL pushed=pPlate->vertex_list.push_stack();
						for(pOtherHuoquVertex=pPlate->vertex_list.GetFirst();pOtherHuoquVertex;pOtherHuoquVertex=pPlate->vertex_list.GetNext())
						{
							if(pOtherHuoquVertex!=pVertex&&pOtherHuoquVertex->vertex.feature==pVertex->vertex.feature)
								break;
						}
						pPlate->vertex_list.pop_stack(pushed);
						f3dPoint huoqu_vec;
						double huoqu_len=0;
						if(pOtherHuoquVertex)
						{
							huoqu_vec=pOtherHuoquVertex->vertex-pVertex->vertex;
							huoqu_len=huoqu_vec.mod();
						}
						else
						{
							huoqu_vec=pPlate->HuoQuLine[iFace-2].endPt-pPlate->HuoQuLine[iFace-2].startPt;
							huoqu_len=huoqu_vec.mod();
						}

						if(huoqu_len>EPS)
							huoqu_vec/=huoqu_len;
						int ret=Int3dlc(&huoqu_inters1,&huoqu_inters2,pPlate->HuoQuLine[iFace-2].startPt,
							huoqu_vec,column_ucsline.startPt,column_ucsline.endPt,cut_radius,cut_radius);
						if(ret<=0)
							continue;	//TODO: logerr.Log();
						if(ret==2)
						{
							f3dPoint verify_vec1=huoqu_inters1-pVertex->vertex;
							f3dPoint verify_vec2=huoqu_inters2-pVertex->vertex;
							double dd1=verify_vec1*huoqu_vec;
							double dd2=verify_vec2*huoqu_vec;
							if(dd1<0||dd1>huoqu_len||(dd2>0&&dd2<dd1))
								huoqu_inters1=huoqu_inters2;
						}
						pNewVertex=vertex_list.append(*pVertex);
						pNewVertex->vertex.Set(huoqu_inters1.x,huoqu_inters1.y,huoqu_inters1.z);
						pNewVertex->type=2;
						pNewVertex->radius=-cut_radius;	//圆柱面切割的都是内圆弧
						pNewVertex->m_bWeldEdge=TRUE;		//被圆柱面切割时工艺上一般都需要焊接
						pNewVertex->weldline.hWeldMotherPart=hWeldParentPart;
						pNewVertex->center=center_pick;	
						pushed=pPlate->vertex_list.push_stack();
						PROFILE_VERTEX* pNextVertex=pPlate->vertex_list.GetNext();
						if(pNextVertex==NULL)
							pNextVertex=pPlate->vertex_list.GetFirst();
						pPlate->vertex_list.pop_stack(pushed);
						if((pNextVertex->vertex.feature==1&&fabs(pPlate->ucs.axis_z*column_norm)<EPS_COS) ||
							(pNextVertex->vertex.feature==2&&fabs(pPlate->huoqufacenorm[0]*column_norm)<EPS_COS))
						{
							pNewVertex->type=3;
							pNewVertex->column_norm=column_norm;
							pNewVertex->center=elipse_center_pick;	//column_line.startPt;
						}
					}
				}
				else
				{	//保留当前顶点，同时当前一顶点被切割时插入前一边切割点
					if(pPrevVertex->xTag.liTagValue==0)
					{	//前一边确定需要插入一个新顶点
						int ret=Int3dlc(&inters1,&inters2,pPrevVertex->vertex,pVertex->vertex-pPrevVertex->vertex,
							column_ucsline.startPt,column_ucsline.endPt,cut_radius,cut_radius);
						if(ret<=0)
							continue;
						else if(ret==2)
						{
							f3dPoint edge_vec=pVertex->vertex-pPrevVertex->vertex;
							normalize(edge_vec);
							f3dPoint line_vec1=inters1-pPrevVertex->vertex;
							f3dPoint line_vec2=inters2-pPrevVertex->vertex;
							double edge_len = DISTANCE(pVertex->vertex,pPrevVertex->vertex);
							double dd1 = line_vec1*edge_vec;
							double dd2 = line_vec2*edge_vec;
							if(dd1<0||dd1>edge_len||(dd2>0&&dd2<dd1))
								inters1=inters2;
						}
						pNewVertex=vertex_list.append(*pPrevVertex);
						pNewVertex->vertex.Set(inters1.x,inters1.y,0);
						if(pVertex->vertex.feature>10)	//火曲点
							pNewVertex->vertex.feature=pVertex->vertex.feature;
					}
					else //if(pPrevVertex->center.feature==1)
					{	//检测前一边是否存在中间与圆柱面相交的情况
						int ret=0;
						if(pPrevVertex->type>0)
						{
							f3dArcLine arcline;
							pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,&pPlate->ucs);
							inters1.z=inters2.z=0;
							if(pPrevVertex->type<3)
								ret=Int2dcc(inters1,inters2,arcline.Center(),arcline.Radius(),center_pick,cut_radius);
							else
							{
								GEPOINT long_axis=arcline.ColumnNorm();
								double cosa=fabs(arcline.ColumnNorm()*arcline.WorkNorm());
								project_vector(long_axis,arcline.WorkNorm());
								long_axis*=(arcline.Radius()/(cosa*long_axis.mod()));
								ret=Int2dce(inters1,inters2,NULL,NULL,center_pick,cut_radius,arcline.Center(),arcline.Radius(),long_axis);
							}
							if(ret==2)
							{
								double ang[2],start_ang =Cal2dLineAng(f3dPoint(arcline.Center()),f3dPoint(arcline.Start()));
								bool valid_arr[2]={false,false};
								for(int i=0;i<2;i++)
								{
									if(i==0)
										ang[i]=Cal2dLineAng(f3dPoint(arcline.Center()),inters1);
									else
										ang[i]=Cal2dLineAng(f3dPoint(arcline.Center()),inters2);
									if(arcline.WorkNorm().z<0)
									{	//圆弧为反向内凹圆弧
										if(ang[i]>EPS)
											ang[i]=2*Pi-ang[i];
										if(start_ang>EPS)
											start_ang=2*Pi-start_ang;
									}
									if(ang[i]<start_ang||ang[i]>start_ang+arcline.SectorAngle())
										ret--;
									if(i==0&&ret==1)	//首顶点无效
										inters1=inters2;
								}
								/*if(ret==2&&dd2<dd1)
								{
									f3dPoint pt=inters1;
									inters1=inters2;
									inters2=inters1;
								}*/
							}
						}
						else
						{
							ret=Int3dlc(&inters1,&inters2,pPrevVertex->vertex,pVertex->vertex-pPrevVertex->vertex,
								column_ucsline.startPt,column_ucsline.endPt,cut_radius,cut_radius);
							if(ret==2)
							{
								f3dPoint edge_vec=pVertex->vertex-pPrevVertex->vertex;
								normalize(edge_vec);
								f3dPoint line_vec1=inters1-pPrevVertex->vertex;
								f3dPoint line_vec2=inters2-pPrevVertex->vertex;
								double edge_len = DISTANCE(pPrevVertex->vertex,pVertex->vertex);
								double dd1 = line_vec1*edge_vec;
								double dd2 = line_vec2*edge_vec;
								if(dd1<0||dd1>edge_len)
								{
									inters1=inters2;
									ret--;
								}
								if(dd2<0||dd2>edge_len)
									ret--;
								if(ret==2&&dd2<dd1)
								{
									f3dPoint pt=inters1;
									inters1=inters2;
									inters2=inters1;
								}
							}
						}
						if(ret==2)
						{	//存在前一边是否存在中间与圆柱面相交的情况
							bWholeCir=FALSE;	//非整圆
							//下面两行顺序不能反，因为后插入顶点实际插在先插入顶点的前面
							PROFILE_VERTEX *pInsertVertex1=vertex_list.append(PROFILE_VERTEX(inters1));
							PROFILE_VERTEX *pInsertVertex2=vertex_list.append(PROFILE_VERTEX(inters2));
							pInsertVertex1->vertex.feature=pInsertVertex2->vertex.feature=pPrevVertex->vertex.feature;
							pInsertVertex1->type=2;
							pInsertVertex1->center=center_pick;
							pInsertVertex1->xTag.liTagValue=pVertex->xTag.liTagValue;	//记录被删除节点的删除标志位的状态
							pInsertVertex2->xTag.liTagValue=pVertex->xTag.liTagValue;	//记录被删除节点的删除标志位的状态
							pInsertVertex1->radius=-cut_radius;	//圆柱面切割的都是内圆弧
							pInsertVertex1->m_bWeldEdge=TRUE;	//被圆柱面切割时工艺上一般都需要焊接
							pInsertVertex1->weldline.hWeldMotherPart=hWeldParentPart;
							if(fabs(column_norm*pPlate->ucs.axis_z)<EPS_COS)
							{	//椭圆弧
								pInsertVertex1->type=3;
								pInsertVertex1->center=elipse_center_pick;
								pInsertVertex1->column_norm=column_norm;	//column_line.startPt;
							}
							pPrevVertex=pVertex;
							bWholeCir=FALSE;
						}
					}
					vertex_list.append(*pVertex);
				}
				pPrevVertex=pVertex;
			}
			pPlate->vertex_list.Empty();
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
				pPlate->vertex_list.append(*pVertex);
			if(bWholeCir)
			{	//整圆	//暂时仅支持单面板
				pPlate->m_tInnerOrigin=center_pick;		//内圆环位置
				pPlate->m_fInnerRadius=cut_radius;
				pPlate->m_tInnerColumnNorm=column_norm;
			}
			pPlate->SetModified();
			pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		}
		m_pDoc->SetModifiedFlag();
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("command:","");
#else
		pCmdLine->FillCmdLine("命令:","");
#endif
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("command:","");
#else
		pCmdLine->FillCmdLine("命令:","");
#endif
		return;
	}
}
//</DEVELOP_PROCESS_MARK>
void CLDSView::OnBoltPad()
{
	m_nPrevCommandID=ID_BOLT_PAD;
#ifdef AFX_TARG_ENU_ENGLISH
 	m_sPrevCommandName="repeat bolts gasket";
#else
	m_sPrevCommandName="重复螺栓垫板";
#endif
	Command("BoltPad");
}
static int CompareThickItemByZOffset(const THICK_ITEMTAG& item1,const THICK_ITEMTAG& item2)
{
	if(item1.zOffset>item2.zOffset)
		return 1;
	else if(item1.zOffset<item2.zOffset)
		return -1;
	else
		return 0;
}
//自动计算垫板厚度
static double CalBoltPadThick(CLDSBolt* pBolt)
{
	double fThick=0;
	if(pBolt==NULL)
		return fThick;
	PARTSET partset;
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	pView->GetBoltActiveRelaPartSet(pBolt,partset,NULL);
	f3dPoint datumPosition=pBolt->ucs.origin;
	f3dPoint worknorm=pBolt->ucs.axis_z;
	ARRAY_LIST<THICK_ITEMTAG> items;
	items.SetSize(0,partset.GetNodeNum());
	for(CLDSPart* pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		f3dPoint inters;
		if(pPart->GetBoltIntersPos(pBolt,inters,NULL,CLDSPart::BOLTBTM_PLANE))
		{
			THICK_ITEMTAG* pItem=items.append();
			pItem->val=pPart->handle;
			pItem->pPart=pPart;
			pItem->through=ftoi(pPart->GetThick());
			f3dPoint zoffset_vec=inters-datumPosition;
			pItem->zOffset=zoffset_vec*worknorm;
		}
	}
	CQuickSort<THICK_ITEMTAG>::QuickSort(items.m_pData,items.GetSize(),CompareThickItemByZOffset);
	THICK_ITEMTAG preItem=items[0];
	for(int i=1;i<items.GetSize();i++)
	{
		double fFullDist=items[i].zOffset-preItem.zOffset;
		double fSpacing=fFullDist-preItem.through;
		if(fSpacing<3)
		{
			preItem=items[i];
			continue;
		}
		if(fSpacing>fThick)
			fThick=fSpacing;	
		preItem=items[i];
	}
	return fThick;
}
CLDSPlate* CLDSView::DesignBoltPad(CSuperSmartPtr<CLDSPart>pDatumPart,CLDSLineAngle *pCutAngle,
								   BOLTSET &boltSet,double fPlatethick/* =-1 */,double fNormoffset/* =-1 */)
{
	try{
		if(pDatumPart.IsNULL()||boltSet.GetNodeNum()<=0||
			(!pDatumPart->IsPlate()&&!pDatumPart->IsAngle()))
			return NULL;
		CLDSBolt *pBolt=boltSet.GetFirst();
		if(pBolt==NULL)
			return NULL;
		LSSPACEEX space;
		IBoltLibrary::GetLsSpaceEx(pBolt->d,&space);
		int end_space=space.WasherEdgeSpace>0?space.WasherEdgeSpace:space.EndSpace;
		//
		CLDSPlate *pBoltPad= (CLDSPlate*)console.AppendPart(CLS_PLATE);	
		pBoltPad->SetLayer(pDatumPart->layer());
		pBoltPad->cfgword=pDatumPart->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
		pBoltPad->iSeg=pDatumPart->iSeg;
		pBoltPad->jdb_style=4;	//螺栓垫板
		pBoltPad->gasketEdgeSpace.berSpace=end_space;
		pBoltPad->gasketEdgeSpace.wingSpace=end_space;
		pBoltPad->gasketEdgeSpace.endSpaceS=end_space;
		pBoltPad->gasketEdgeSpace.endSpaceE=end_space;
		pBoltPad->designInfo.m_bEnableFlexibleDesign=TRUE;	//启用柔性设计
		for(CLDSBolt *pLs=boltSet.GetFirst();pLs;pLs=boltSet.GetNext())
			pBoltPad->AppendLsRef(pLs->GetLsRef());
		//
		if(pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			int x0_y1=2;
			f3dPoint inter_pos;
			if(pDatumPart.LineAnglePointer()->IsLsInWing(pBolt,&x0_y1))
			{	//确定垫板的Z轴方向
				if(x0_y1==0)
					pBoltPad->ucs.axis_z=pDatumPart.LineAnglePointer()->get_norm_x_wing();
				else if(x0_y1==1)
					pBoltPad->ucs.axis_z=pDatumPart.LineAnglePointer()->get_norm_y_wing();
				//填充垫板法线设计信息
				pBoltPad->designInfo.norm.norm_style=1;
				pBoltPad->designInfo.norm.norm_wing=x0_y1;
				pBoltPad->designInfo.norm.direction=0;	//朝外
				pBoltPad->designInfo.norm.hVicePart=pDatumPart->handle;
			}
			//定义垫板坐标系
			pBoltPad->ucs.axis_x=pDatumPart.LineAnglePointer()->End()-pDatumPart.LineAnglePointer()->Start();
			pBoltPad->cDatumAxis='X';
			normalize(pBoltPad->ucs.axis_x);
			pDatumPart.LineAnglePointer()->getUCS(pDatumPart->ucs);
			SnapPerp(&pBoltPad->ucs.origin,pDatumPart->ucs.origin,pDatumPart->ucs.origin+pBoltPad->ucs.axis_x*1000,pBolt->ucs.origin);
			pBoltPad->designInfo.m_hBasePart=pDatumPart->handle;
			if(pCutAngle)
			{
				if(boltSet.GetNodeNum()==1)//仅有一个螺栓的时候要保证，螺栓垫板的X轴垂直切割基准角钢
				{
					f3dPoint axis_y=pCutAngle->End()-pCutAngle->Start();
					pBoltPad->ucs.axis_x=pBoltPad->ucs.axis_z^axis_y;
					normalize(pBoltPad->ucs.axis_x);
				}
				//选择了切割垫板的基准角钢，则调整X轴方向指向切割基准角钢
				f3dPoint vec=inter_pos-pBoltPad->ucs.origin;
				if(pBoltPad->ucs.axis_x*vec<0)
					pBoltPad->ucs.axis_x*=-1.0;
				//存储基准角钢和切割基准角钢重新设计时用
				pBoltPad->designInfo.m_hBasePart=pDatumPart->handle;
				CDesignLjPartPara *pDesPara=pBoltPad->designInfo.partList.Add(pCutAngle->handle);
				pDesPara->hPart=pCutAngle->handle;
				pDesPara->m_nClassTypeId=CLS_LINEANGLE;
				//该杆件用于切割螺栓垫板，对称时不调整杆件位置 wht 11-01-11
				pDesPara->m_bAdjustLjEndPos=FALSE;	
			}
			pBoltPad->ucs.axis_y=pBoltPad->ucs.axis_z^pBoltPad->ucs.axis_x;
			normalize(pBoltPad->ucs.axis_y);
		}
		else //if(pDatumPart->GetClassTypeId()==CLS_PLATE||pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			pBoltPad->designInfo.m_hBasePart=pDatumPart->handle;
			pBoltPad->designInfo.norm.norm_style=4;
			pBoltPad->designInfo.norm.hVicePart=pDatumPart->handle;
			pBoltPad->ucs.axis_z=pDatumPart->ucs.axis_z;
			pBoltPad->designInfo.norm.nearVector.Set(0,0,1);
			//定义垫板坐标系
			pBoltPad->ucs.axis_x=pDatumPart->ucs.axis_x;
			normalize(pBoltPad->ucs.axis_x);
			double normoffset=0;
			if(pDatumPart->GetClassTypeId()==CLS_PLATE)
				normoffset=pDatumPart.PlatePointer()->m_fNormOffset;
			else if(pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
				normoffset=pDatumPart.ParamPlatePointer()->GetNormOffset()+pDatumPart->GetThick();
			pBoltPad->ucs.origin=pBolt->ucs.origin;
			project_point(pBoltPad->ucs.origin,
				pDatumPart->ucs.origin+pDatumPart->ucs.axis_z*normoffset,pDatumPart->ucs.axis_z);
			pBoltPad->designInfo.origin.datum_pos_style=9;
			pBoltPad->designInfo.origin.des_para.hPart=pDatumPart->handle;
			pBoltPad->designInfo.origin.datum_pos=pBoltPad->ucs.origin;
			coord_trans(pBoltPad->designInfo.origin.datum_pos,pDatumPart->ucs,FALSE,TRUE);
			pBoltPad->ucs.axis_y=pBoltPad->ucs.axis_z^pBoltPad->ucs.axis_x;
			normalize(pBoltPad->ucs.axis_y);
		}
		g_pSolidDraw->ReleaseSnapStatus();
		if(fNormoffset>-1&&fPlatethick>-1)
		{
			pBoltPad->m_fNormOffset=fNormoffset;
			pBoltPad->Thick=FindPlankThick(fPlatethick);
			pBoltPad->cMaterial=pDatumPart->cMaterial;
		}
		else
		{	//
			static CDesignPlateDlg plateParaDlg;
			if(pCutAngle)
				plateParaDlg.m_nThick=(int)pCutAngle->GetThick();
			else
				plateParaDlg.m_nThick=FindPlankThick(CalBoltPadThick(boltSet.GetFirst()));
			plateParaDlg.m_pDatumPart=pDatumPart.PartPointer();
			plateParaDlg.m_pBoltPad=pBoltPad;
			if(plateParaDlg.DoModal()!=IDOK||plateParaDlg.m_iDesStyle==1)
			{	//取消设计或设计垫圈时要删除提前生成的垫板
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->DelEnt(pBoltPad->handle);
				console.DeletePart(pBoltPad->handle);
				console.DispPartSet.DeleteNode(pBoltPad->handle);
				return NULL;
			}
			pBoltPad->m_fNormOffset=plateParaDlg.m_fNormOffset;
			pBoltPad->Thick=plateParaDlg.m_nThick;
			pBoltPad->cMaterial=CSteelMatLibrary::RecordAt(plateParaDlg.m_iMaterial).cBriefMark;
			pBoltPad->SetPartNo(plateParaDlg.m_sPartNo);
			//根据输入件号，提取归属段号
			SEGI iSeg;
			if(ParsePartNo(plateParaDlg.m_sPartNo,&iSeg,NULL))
				pBoltPad->iSeg=iSeg;
		}
		//添加螺栓引用,修正螺栓通厚
		for(CLsRef *pLsRef=pBoltPad->GetFirstLsRef();pLsRef;pLsRef=pBoltPad->GetNextLsRef())
		{	//处理螺栓射线号与螺栓引用射线号间的映射
			int nPush=pBoltPad->GetLsRefList()->push_stack();
			pBolt=pLsRef->GetLsPtr();
			pBolt->AddL0Thick(pBoltPad->handle,TRUE); 
			pBolt->CorrectBoltZnL0Para();
			pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			pBoltPad->GetLsRefList()->pop_stack(nPush);
		}
		pBoltPad->CalStdProfile();
		if(pCutAngle&&pBoltPad->GetLsCount()>1)	//仅仅对多螺栓垫板进行切割处理
			CutBoltPadByAngle(pBoltPad,pCutAngle);
		pBoltPad->SetModified();
		pBoltPad->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pBoltPad->GetSolidPartObject());
		g_pSolidDraw->Draw();
		m_pDoc->SetModifiedFlag();
		return pBoltPad;
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	return NULL;
}
BOOL CLDSView::CutBoltPadByAngle(CLDSPlate* pBoltPad,CLDSLineAngle* pCutDatumJg/*=NULL*/)
{
	if(pBoltPad->jdb_style!=4||pBoltPad->designInfo.iProfileStyle0123==3)
		return FALSE;
	CLDSLineAngle* pCutAngle=pCutDatumJg;
	if(pCutAngle==NULL)
	{	//根据垫板的连接杆件获取切割基准角钢
		CDesignLjPartPara *pLjPara=pBoltPad->GetFirstLjPara();	//切割垫板的基准角钢
		if(pLjPara==NULL)
			return FALSE;
		pCutAngle=(CLDSLineAngle*)console.FromPartHandle(pLjPara->hPart,CLS_LINEANGLE);
	}
	if(pCutAngle&&pBoltPad->GetLsCount()>1)	//只有多孔垫板需要进行切角
	{
		f3dPoint face_pos,face_norm;
		pCutAngle->getUCS(pCutAngle->ucs);	//获取角钢坐标系，防止单线模式下角钢坐标原点为(0,0,0)
		face_pos=pCutAngle->ucs.origin;
		if(fabs(pCutAngle->get_norm_x_wing()*pBoltPad->ucs.axis_z)>fabs(pCutAngle->get_norm_y_wing()*pBoltPad->ucs.axis_z))
			face_norm=pCutAngle->get_norm_y_wing();	//1.Y肢法线为初始切割面法线
		else if(fabs(pCutAngle->get_norm_y_wing()*pBoltPad->ucs.axis_z)>fabs(pCutAngle->get_norm_x_wing()*pBoltPad->ucs.axis_z))
			face_norm=pCutAngle->get_norm_x_wing();	//2.X肢法线为初始切割面法线
		//实际垫板切割时基本上都是用角钢肢翼侧切割的(因为只有射线角钢从肢翼侧伸主基准角钢时才需要垫板)
		face_pos=pCutAngle->ucs.origin-face_norm*(pCutAngle->GetWidth()+g_sysPara.VertexDist);
		CutPlateByPlane(pBoltPad,face_pos,face_norm,0,FALSE,FALSE);
		//切割完垫板后，需对垫板的轮廓点提取计算类型
		pBoltPad->RetrieveGasketVertexCalType();
		return TRUE;
	}
	return FALSE;
}
int CLDSView::BoltPad()
{
	CLDSBolt *pBolt=NULL;
	BOLTSET selectedBoltSet,boltSet;
	long *id_arr=NULL,num=g_pSolidSnap->GetLastSelectEnts(id_arr);
	for(int i=0;i<num;i++)
	{
		pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[i],CLS_BOLT);
		if(pBolt)
			selectedBoltSet.append(pBolt);
	}
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	g_pSolidDraw->ReleaseSnapStatus();
	//选择垫板的基准构件
	CSuperSmartPtr<CLDSPart>pDatumPart;
	CString cmdStr,errCmdStr;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("BoltPad Please select datum parts (angle or plate) which need to add gasket:");
	errCmdStr.Format("BoltPad No proper parts selected, please select datum parts which need to add gasket:");
#else
	cmdStr.Format("BoltPad 请选择要添加垫板的基准构件(角钢或钢板):");
	errCmdStr.Format("BoltPad 没有选中合适的构件，请重新选择要添加垫板的基准构件(角钢或钢板):");
#endif
	DWORD dwFlag,dwhObj=0,dwExportFlag=0;
	dwFlag=GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE);
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,dwFlag);
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		pDatumPart=console.FromPartHandle(obj.hRelaObj);
		if(pDatumPart.IsHasPtr()&&(pDatumPart->IsPlate()||pDatumPart->IsAngle()))
			break;
		pCmdLine->FillCmdLine(errCmdStr,"");
	}
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pDatumPart->handle));
	CLsRefList *pLsRefList=pDatumPart->GetLsRefList();
	if(pLsRefList==NULL)
		return 0;
	//选择垫板关联螺栓
	CUndoOperObject undo(&Ta);
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("BoltPad Please select all the bolts relative to gasket:");
	errCmdStr.Format("BoltPad number of effective bolt is zero, not all the selected bolts are on the datum angle, please select again:");
#else
	cmdStr.Format("BoltPad 请选择垫板关联的所有螺栓:");
	errCmdStr.Format("BoltPad 有效螺栓数为0，可能选择的螺栓并不全在基准角钢上，请重新选择:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
	try{
		OpenWndSel();	//开窗选择螺栓
		while(1)
		{
			boltSet.Empty();
			g_pSolidDraw->ReleaseSnapStatus();
			for(pBolt=selectedBoltSet.GetFirst();pBolt;pBolt=selectedBoltSet.GetNext())
				g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
			//进行螺栓捕捉
			if(pCmdLine->GetStrFromCmdLine(cmdStr)==FALSE)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			num=g_pSolidSnap->GetLastSelectEnts(id_arr);
			for(int i=0;i<num;i++)
			{
				pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[i],CLS_BOLT);
				if(pBolt && pLsRefList->FromHandle(pBolt->handle))	//螺栓在基准构件上
					boltSet.append(pBolt);
			}
			if(boltSet.GetNodeNum()>0)
				break;
			pCmdLine->FillCmdLine(errCmdStr,"");
		}
		OnOperOther();	//取消开窗操作状态
		int x0_y1=0;
		if(pDatumPart->IsAngle()&&pDatumPart.LineAnglePointer()->IsLsInWing(boltSet.GetFirst(),&x0_y1)==FALSE)
			return 0;
		//选择切割螺栓垫板的基准角钢
		CLDSLineAngle *pLineAngle=NULL;
		if(pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("BoltPad Please select the datum angle which need to cut gasket, if no, press enter directly:");
			errCmdStr.Format("BoltPad the Please select the datum angle which needs to cut gasket, if no, press enter directly");
#else
			cmdStr.Format("BoltPad 请选择要切割垫板的基准角钢，若没有直接回车:");
			errCmdStr.Format("BoltPad 所选角钢可能与垫板基准角钢平行，请重新选择要切割垫板的基准角钢，若没有直接回车");
#endif
			verifier.ClearSnapFlag();
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE));
			pCmdLine->FillCmdLine(cmdStr,"");
			while(1)
			{	
				if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				{
					pCmdLine->CancelCmdLine();
					return 0;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				pLineAngle=(CLDSLineAngle*)console.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE);
				if(pLineAngle)
				{
					f3dPoint axis_z=pDatumPart.LineAnglePointer()->get_norm_x_wing();
					if(x0_y1==1)
						axis_z=pDatumPart.LineAnglePointer()->get_norm_y_wing();
					f3dLine line,datum_line;
					datum_line.startPt=pDatumPart.LineAnglePointer()->Start();
					datum_line.endPt=pDatumPart.LineAnglePointer()->End();
					project_point(datum_line.startPt,pDatumPart.LineAnglePointer()->Start(),axis_z);
					project_point(datum_line.endPt,pDatumPart.LineAnglePointer()->Start(),axis_z);
					line.startPt=pLineAngle->Start();
					line.endPt=pLineAngle->End();
					project_point(line.startPt,pDatumPart.LineAnglePointer()->Start(),axis_z);
					project_point(line.endPt,pDatumPart.LineAnglePointer()->Start(),axis_z);
					int ret=Int3dll(line,datum_line,axis_z);
					if(ret!=0&&ret!=-1)
						break;
					else
						pCmdLine->FillCmdLine(errCmdStr,"");
				}
				else
					break;
			}
			pCmdLine->FinishCmdLine(CXhChar16("0x%X",pLineAngle==NULL?0:pLineAngle->handle));
			if(pLineAngle)
				g_pSolidDraw->SetEntSnapStatus(pLineAngle->handle);
		}
		//设计螺栓垫板
		DesignBoltPad(pDatumPart,pLineAngle,boltSet);
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	BoltPad();	//Command("BoltPad");
	return 0;
}
//遮水板
BOOL CLDSView::FinishDesignWaterShieldedPlate(CLDSNode *pBaseNode,CLDSLineTube *pDatumTube,CLDSPart *pPlate)
{
	if(pBaseNode==NULL||pDatumTube==NULL)
		return FALSE;
	f3dLine baseline(pDatumTube->pStart->Position(false),pDatumTube->pEnd->Position(false));
	if(!baseline.PtInLine(pBaseNode->Position(false)))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The selected node is not on the rod!");
#else
		AfxMessageBox("所选节点不在杆件上!");
#endif
		return FALSE;
	}
	baseline.startPt=pDatumTube->pStart->Position(true);
	baseline.endPt=pDatumTube->pEnd->Position(true);
	if(pPlate&&pPlate->GetClassTypeId()!=CLS_PLATE&&pPlate->GetClassTypeId()!=CLS_PARAMPLATE)
		return FALSE;
	if(pPlate&&pPlate->m_hPartWeldParent!=pDatumTube->handle)
		return FALSE;
	double W=0;
	static CWaterShieldedPlateDlg plateDlg;
	plateDlg.m_fInternalDiameter=0;
	plateDlg.m_fExternalDiameter=pDatumTube->GetDiameter();
	if(pPlate&&pPlate->GetClassTypeId()==CLS_PARAMPLATE&&
		(((CLDSParamPlate*)pPlate)->m_iParamType==TYPE_FL||
		((CLDSParamPlate*)pPlate)->m_iParamType==TYPE_FLG||
		((CLDSParamPlate*)pPlate)->m_iParamType==TYPE_FLR))
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPlate;
		pParamPlate->GetDesignItemValue('W',&W);
		plateDlg.m_fExternalDiameter=W;
		plateDlg.m_fWaterSlotAngle=360.0/pParamPlate->GetLsCount();
	}
	UCS_STRU ucs;
	if(pBaseNode==pDatumTube->pStart)
	{
		ucs.axis_z=pDatumTube->Start()-pDatumTube->End();
		normalize(ucs.axis_z);
		ucs.origin=pDatumTube->Start();
		plateDlg.m_fNormalOffset=pDatumTube->startOdd();
	}
	else if(pBaseNode==pDatumTube->pEnd)
	{
		ucs.axis_z=pDatumTube->End()-pDatumTube->Start();
		normalize(ucs.axis_z);
		ucs.origin=pDatumTube->End();
		plateDlg.m_fNormalOffset=pDatumTube->endOdd();
	}
	else
	{
		ucs.axis_z=pDatumTube->End()-pDatumTube->Start();
		normalize(ucs.axis_z);
		SnapPerp(&ucs.origin,baseline,pBaseNode->Position(true));
	}
	if(pPlate)
		plateDlg.m_fNormalOffset+=pPlate->GetThick()*0.5;
	ucs.axis_x=pDatumTube->ucs.axis_x;
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	plateDlg.m_fNormX = ucs.axis_z.x;
	plateDlg.m_fNormY = ucs.axis_z.y;
	plateDlg.m_fNormZ = ucs.axis_z.z;
	if(plateDlg.DoModal()!=IDOK)
		return FALSE;
	Ta.BeginUndoListen();
	CLDSParamPlate *pPlateAttachLaunder=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pPlateAttachLaunder->m_hPartWeldParent = pDatumTube->handle;
	pPlateAttachLaunder->cfgword=pDatumTube->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
	pPlateAttachLaunder->ucs=ucs;
	pPlateAttachLaunder->SetPartNo(plateDlg.m_sPartNo.GetBuffer(0));
	pPlateAttachLaunder->iSeg=SEGI(plateDlg.m_sSegI.GetBuffer());
	pPlateAttachLaunder->layer(2)=pDatumTube->layer(2);	//调整环向板的图层名
	pPlateAttachLaunder->thick=plateDlg.m_nPlateThick;
	pPlateAttachLaunder->cMaterial=CSteelMatLibrary::RecordAt(plateDlg.m_iMaterial).cBriefMark;
	pPlateAttachLaunder->m_iParamType=TYPE_WATERSHIELDEDPLATE;
	f3dPoint axis_z(plateDlg.m_fNormX,plateDlg.m_fNormY,plateDlg.m_fNormZ);
	normalize(axis_z);
	if(pPlateAttachLaunder->ucs.axis_z!=axis_z)
	{
		f3dPoint rot_axis_vec=pPlateAttachLaunder->ucs.axis_z^axis_z;
		double rot_ang=ACOS(pPlateAttachLaunder->ucs.axis_z*axis_z);
		RotateVectorAroundVector(pPlateAttachLaunder->ucs.axis_x,rot_ang,rot_axis_vec);
		RotateVectorAroundVector(pPlateAttachLaunder->ucs.axis_y,rot_ang,rot_axis_vec);
		pPlateAttachLaunder->ucs.axis_z=axis_z;
	}
	pPlateAttachLaunder->ucs.origin+=pPlateAttachLaunder->ucs.axis_z*plateDlg.m_fNormalOffset;
#ifdef AFX_TARG_ENU_ENGLISH
	pPlateAttachLaunder->SetDesignItemValue('B',pBaseNode->handle,"datum node",PARAM_POS,1);
	pPlateAttachLaunder->SetDesignItemValue('C',pDatumTube->handle,"datum tube",PARAM_POS,1);
	pPlateAttachLaunder->SetDesignItemValue('D',plateDlg.m_fInternalDiameter,"diameter of inner ring",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('W',plateDlg.m_fExternalDiameter,"diameter of inner ring",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('S',plateDlg.m_fNormalOffset,"Normal Direction Offset distance");
	pPlateAttachLaunder->SetDesignItemValue('K',plateDlg.m_fWaterSlotWidth,"width of water tank",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('H',plateDlg.m_fWaterSlotDepth,"depth of water tank",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('L',plateDlg.m_fWaterSlotLen,"length of water tank",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('A',plateDlg.m_fWaterSlotAngle,"angle between two water tanks",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('O',plateDlg.m_fOffsetAngle,"angle offset between two water tanks angle bisector and datum edge",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('E',plateDlg.m_iDatumLine,"datum edge of positioning angle:0.datum edge1.weld line");
#else
	pPlateAttachLaunder->SetDesignItemValue('B',pBaseNode->handle,"基准节点",PARAM_POS,1);
	pPlateAttachLaunder->SetDesignItemValue('C',pDatumTube->handle,"基准钢管",PARAM_POS,1);
	pPlateAttachLaunder->SetDesignItemValue('D',plateDlg.m_fInternalDiameter,"内环直径",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('W',plateDlg.m_fExternalDiameter,"外环直径",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('S',plateDlg.m_fNormalOffset,"法向偏移距离");
	pPlateAttachLaunder->SetDesignItemValue('K',plateDlg.m_fWaterSlotWidth,"流水槽宽度",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('H',plateDlg.m_fWaterSlotDepth,"流水槽深度",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('L',plateDlg.m_fWaterSlotLen,"流水槽长度",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('A',plateDlg.m_fWaterSlotAngle,"两流水槽之间的夹角",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('O',plateDlg.m_fOffsetAngle,"两流水槽角平分线与基准边之间的偏移角",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('E',plateDlg.m_iDatumLine,"定位角基准边:0.基准边1.焊道线");
#endif
	pPlateAttachLaunder->DesignPlate();
	pPlateAttachLaunder->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pPlateAttachLaunder->GetSolidPartObject());
	CLsRefList *pLsRefList=NULL;
	if(pPlate&&pPlate->GetClassTypeId()==CLS_PLATE)
		pLsRefList=((CLDSPlate*)pPlate)->GetLsRefList();
	else if(pPlate&&pPlate->GetClassTypeId()==CLS_PARAMPLATE)
		pLsRefList=((CLDSParamPlate*)pPlate)->GetLsRefList();
	if(pLsRefList)
	{	//引入螺栓
		for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
		{
			pPlateAttachLaunder->AppendLsRef(*pLsRef);
			(*pLsRef)->CalGuigeAuto();
			(*pLsRef)->SetModified();
			(*pLsRef)->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
		}
	}
	g_pSolidDraw->Draw();
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	return TRUE;
}

// 典型连接设计 自动处理十字插板立板非90度的情况
// 可用来设计：一、变坡位置(1.使用异性法兰 2.双面板 3.钢管相贯)
//			   二、塔脚板
//			   三、塔身横隔面 横担相连的位置 			
//			   四、生成普通的节点板  
// 附带肋板，封板一同设计
void CLDSView::OnClassicConnectDesign() 
{

	//一.
}

void CLDSView::DesignTubeEndLj(char cType)
{
	CLDSPart *pPart=NULL;
	CLDSNode *pBaseNode=NULL;
	CLDSLineTube *pLineTube=NULL;
	//创建HashTable
	CXhChar100 sText;
	CString cmdStr;
	CHashTable<CLDSPart*>partsetTable;
	partsetTable.CreateHashTable(100);
	g_pSolidDraw->ReleaseSnapStatus();
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please tube which needs end design:","");
#else
	pCmdLine->FillCmdLine("请选择需要进行端头设计的钢管:","");
#endif
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINETUBE));
	verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prLine);
	while(1)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
		{
			pCmdLine->CancelCmdLine();
			return;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		dwhObj=obj.hRelaObj;		
		if(dwhObj>0x20)
		{
			SmartRodPtr pRod=console.FromRodHandle(dwhObj);
			if(pRod.IsHasPtr()&&pRod->IsTube())
			{
				double scaleOfS2E=GetPickPosScaleOnRodS2E(pRod);
				if(scaleOfS2E<0.3&&pRod->pStart)
					pBaseNode=pRod->pStart;
				else if(scaleOfS2E>0.7&&pRod->pEnd)
					pBaseNode=pRod->pEnd;
				pLineTube=pRod.pTube;
				break;
			}
		}
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select tube which need to end connection design:","");
#else 
		pCmdLine->FillCmdLine("没有选中合适的钢管，请重新选择需要进行端头设计的钢管:","");
#endif
	}
	g_pSolidDraw->SetEntSnapStatus(pLineTube->handle);
	pCmdLine->FinishCmdLine(CXhChar16("0X%X",pLineTube->handle));
	if(pBaseNode==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select current tube end node:","");
#else
		pCmdLine->FillCmdLine("请选择钢管的的端头节点:","");
#endif
		while(1)
		{
			f3dPoint* point;
			int ret = g_pSolidSnap->SnapPoint(point);
			if(ret<0)
			{
				pCmdLine->CancelCmdLine();
				return;
			}
			else if(ret>0)
			{
				pBaseNode=console.FromNodeHandle(point->ID);
				if(pBaseNode)
				{
					g_pSolidDraw->SetEntSnapStatus(point->ID);
					break;
				}
			}
		}
		pCmdLine->FinishCmdLine(CXhChar16("0X%X",pBaseNode->handle));
	}
	if(pBaseNode==NULL||pLineTube==NULL)
		return;
	static CDesignTubeEndLjDlg endLjDlg;
	if(cType=='C'||cType=='U'||cType=='X'||cType=='O')
	{
		endLjDlg.m_bSpecEndLjStyle=TRUE;
		if(cType=='O')
			endLjDlg.m_iTubeEndLjStyle=2;
		else if(cType=='U')
			endLjDlg.m_iTubeEndLjStyle=3;
		else if(cType=='C')
			endLjDlg.m_iTubeEndLjStyle=4;
		else if(cType=='X')
			endLjDlg.m_iTubeEndLjStyle=5;
	}
	else
		endLjDlg.m_bSpecEndLjStyle=FALSE;
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(pLineTube->handle,TRUE);
	if(pLineTube->pStart==pBaseNode)
		endLjDlg.m_bStartEnd=TRUE;
	else
		endLjDlg.m_bStartEnd=FALSE;
	//预判钢管端连接类型
	if(FindTubeInLine(pLineTube,endLjDlg.m_bStartEnd)&&pBaseNode->m_cPosCalType!=4)
		endLjDlg.m_iTubeEndLjStyle=2;	//法兰
	endLjDlg.m_pBaseNode=pBaseNode;
	endLjDlg.m_pCurLineTube=pLineTube;
	{
		CUndoOperObject undo(&Ta,true);
		if(endLjDlg.DoModal()!=IDOK)
			return;
	}
	if(endLjDlg.m_iTubeEndLjStyle==2)
		cType='O';
	else if(endLjDlg.m_iTubeEndLjStyle==3)
		cType='U';
	else if(endLjDlg.m_iTubeEndLjStyle==4)
		cType='C';
	else if(endLjDlg.m_iTubeEndLjStyle==5)
		cType='X';

	DesignTubeEndLj(cType);
}
int CLDSView::DesignTubeEndLjTypeD()	//钢管端连接
{
	DesignTubeEndLj('D');
	return 0;
}
int CLDSView::DesignTubeEndLjTypeO()	//法兰
{
	DesignTubeEndLj('O');
	return 0;
}
int CLDSView::DesignTubeEndLjTypeU()	//U型插板
{
	DesignTubeEndLj('U');
	return 0;
}
int CLDSView::DesignTubeEndLjTypeC()	//C型插板
{
	DesignTubeEndLj('C');
	return 0;
}
int CLDSView::DesignTubeEndLjTypeX()
{
	DesignTubeEndLj('X');	//X钢管端连接
	return 0;
}
int CLDSView::DesignTubeEndLjTypeTJ()//设计塔脚
{
	DesignSpecialFl();
	return 0;
}

typedef CLDSPlate*	PLATEPTR;
static BOOL DesignRayPoleAndPlate(CLDSLineTube *pBaseLineTube,CDesignTubeEndLjDlg *pEndLjDlg,
								  LINEPARTSET &linePartSet,LINEPARTSET *pMidLinePart,
								  PLATEPTR &pNodePlate1,PLATEPTR &pNodePlate2)
{
	CLDSParamPlate *pSrcParamPlate=NULL;
	CLDSLineTube *pLineTube1=NULL,*pLineTube2=NULL;
	CLDSLinePart *pRayLinePart1=linePartSet.GetFirst();
	CLDSLinePart *pRayLinePart2=linePartSet.GetNext();
	if(pRayLinePart1==NULL)
		return FALSE;
	CLDSLinePart *pHorizontalPole1=NULL,*pHorizontalPole2=NULL;
	f3dPoint base_line_vec=(pBaseLineTube->End()-pBaseLineTube->Start()).normalized();
	if(pMidLinePart)
	{
		CLDSLinePart *pLinePart=NULL;
		f3dPoint len_vec1,len_vec2,axis_z1,axis_z2;
		len_vec1=(pRayLinePart1->End()-pRayLinePart1->Start()).normalized();
		if(pRayLinePart2)
			len_vec2=(pRayLinePart2->End()-pRayLinePart2->Start()).normalized();
		axis_z1=len_vec1^base_line_vec;
		axis_z2=len_vec2^base_line_vec;
		normalize(axis_z1);
		normalize(axis_z2);
		for(pLinePart=pMidLinePart->GetFirst();pLinePart;pLinePart=pMidLinePart->GetNext())
		{
			if(!pLinePart->IsTube())
				continue;
			f3dPoint len_vec=(pLinePart->End()-pLinePart->Start()).normalized();
			f3dPoint axis_z=len_vec^base_line_vec;
			normalize(axis_z);
			if(fabs(axis_z*axis_z1)>EPS_COS2&&pHorizontalPole1==NULL)
				pHorizontalPole1=pLinePart;
			if(fabs(axis_z*axis_z2)>EPS_COS2&&pHorizontalPole2==NULL)
				pHorizontalPole2=pLinePart;
			if(pHorizontalPole1&&pHorizontalPole2)
				break;
		}
	}
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(pRayLinePart1->handle,TRUE);
	BOOL bSameDesign=FALSE;
	if(pRayLinePart2)
	{
		g_pSolidDraw->SetEntSnapStatus(pRayLinePart2->handle,TRUE);
		f3dPoint ray_line_vec1=(pRayLinePart1->End()-pRayLinePart1->Start()).normalized();
		if(pEndLjDlg->m_pBaseNode==pRayLinePart1->pEnd)
			ray_line_vec1*=-1.0;
		f3dPoint ray_line_vec2=(pRayLinePart2->End()-pRayLinePart2->Start()).normalized();
		if(pEndLjDlg->m_pBaseNode==pRayLinePart2->pEnd)
			ray_line_vec2*=-1.0;
		double fAngle1=cal_angle_of_2vec(base_line_vec,ray_line_vec1);
		double fAngle2=cal_angle_of_2vec(base_line_vec,ray_line_vec2);
		if(fabs(fAngle1-fAngle2)<EPS2&&fabs(pRayLinePart1->GetThick()-pRayLinePart2->GetThick())<EPS2
			&&fabs(pRayLinePart1->GetWidth()-pRayLinePart2->GetWidth())<EPS2)
			bSameDesign=TRUE;
	}
	g_pSolidDraw->ReleaseSnapStatus();
	if(pRayLinePart1->GetClassTypeId()==CLS_LINETUBE)
	{
		g_pSolidDraw->SetEntSnapStatus(pRayLinePart1->handle);
		pLineTube1=(CLDSLineTube*)pRayLinePart1;
		pEndLjDlg->m_pCurLineTube=pLineTube1;
		if(pEndLjDlg->DoModal()==IDOK)
		{
			pSrcParamPlate=pEndLjDlg->GetParamPlate();
			if(pEndLjDlg->m_bDesignNodePlate)
			{
				CLDSLineTube *pLineTube=NULL;
				if(pHorizontalPole1&&pHorizontalPole1->IsTube())
					pLineTube=(CLDSLineTube*)pHorizontalPole1;
				pNodePlate1=pEndLjDlg->CreateNodePlate(pBaseLineTube,NULL,NULL,pLineTube);
			}
		}
		else
			return FALSE;
		if(pSrcParamPlate->m_iParamType==TYPE_XEND)
			bSameDesign=FALSE;//暂时关掉十字插板时，正侧面自动参照设计功能,否则会由于法线的参数化设计导致错误 wjh 2011-12-8
	}
	else if(pRayLinePart1->GetClassTypeId()==CLS_LINEANGLE)
	{	//射线杆件为角钢时，设计节点板
		g_pSolidDraw->SetEntSnapStatus(pRayLinePart1->handle);
		CLDSLineAngle* pRayLineAngle=(CLDSLineAngle*)pRayLinePart1;
		if(pEndLjDlg->m_bDesignNodePlate)
			pNodePlate1=pEndLjDlg->CreateNodePlate(pBaseLineTube,pRayLineAngle);
	}
	g_pSolidDraw->ReleaseSnapStatus();
	if(pRayLinePart2&&pRayLinePart2->GetClassTypeId()==CLS_LINEANGLE)
	{	//射线杆件为角钢时，设计节点板
		g_pSolidDraw->SetEntSnapStatus(pRayLinePart2->handle);
		CLDSLineAngle* pRayLineAngle=(CLDSLineAngle*)pRayLinePart2;
		if(pEndLjDlg->m_bDesignNodePlate)
			pNodePlate2=pEndLjDlg->CreateNodePlate(pBaseLineTube,pRayLineAngle);
	}
	if(pSrcParamPlate&&pLineTube1&&pRayLinePart2&&
		pRayLinePart2->GetClassTypeId()==CLS_LINETUBE)
	{
		g_pSolidDraw->SetEntSnapStatus(pRayLinePart2->handle);
		pLineTube2=(CLDSLineTube*)pRayLinePart2;
		CTubeEndPosPara *pCurDesEnd=&pLineTube2->desEndPos;
		TUBEJOINT *pCurJoint=&pLineTube2->m_tJointEnd;
		if(pLineTube2->pStart==pEndLjDlg->m_pBaseNode)
		{
			pCurJoint=&pLineTube2->m_tJointStart;
			pCurDesEnd=&pLineTube2->desStartPos;
		}
		CLDSParamPlate *pExistPlate=(CLDSParamPlate*)console.FromPartHandle(pCurJoint->hLinkObj,CLS_PARAMPLATE);
		if(bSameDesign&&pExistPlate==NULL)
		{
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pSrcParamPlate->CloneTo(*pParamPlate);
			if(pSrcParamPlate->workNorm.hVicePart==pLineTube1->handle)
				pParamPlate->workNorm.hVicePart=pLineTube2->handle;
			//清空附加构件链表
			pParamPlate->AttachPart.Empty();
			pParamPlate->SetLayer(pLineTube2->layer());	//图层名
			pParamPlate->cfgword=pLineTube2->cfgword;
			//异形法兰射线杆件 卷边板U型板自动调整钢管工作面法线方向
			if(pEndLjDlg->m_bSpecialFlRayPole&&(pSrcParamPlate->m_iParamType==TYPE_ROLLEND
				||pSrcParamPlate->m_iParamType==TYPE_UEND))
			{	
				pLineTube2->m_iNormCalStyle=0;	//直接指定工作面法线
				pLineTube2->SetWorkPlaneNorm(f3dPoint(0,0,1));
			}
			double fOddment=pLineTube1->endOdd();	
			pCurDesEnd->fNormOffset=pLineTube1->desEndPos.fNormOffset;	//钢管法线偏移量
			if(pLineTube1->pStart==pEndLjDlg->m_pBaseNode)
			{
				fOddment=pLineTube1->startOdd();
				pCurDesEnd->fNormOffset=pLineTube1->desStartPos.fNormOffset;
			}
			pParamPlate->m_hPartWeldParent=pLineTube2->handle;
			int type=0;
			if(pParamPlate->IsFL())
				type=2;
			else if(pParamPlate->m_iParamType==TYPE_UEND)
				type=3;
			else if(pParamPlate->m_iParamType==TYPE_ROLLEND)
				type=4;
			else if(pParamPlate->m_iParamType==TYPE_XEND)
				type=5;
			pLineTube2->CalPosition();	//计算钢管位置
			if(pLineTube2->pStart==pEndLjDlg->m_pBaseNode)
			{
				pLineTube2->SetStartOdd(fOddment);
				pLineTube2->m_tJointStart.type=type;
				pLineTube2->m_tJointStart.hLinkObj=pParamPlate->handle;
				pLineTube2->DesignStartJointUCS(pParamPlate);
			}
			else
			{
				pLineTube2->SetEndOdd(fOddment);
				pLineTube2->m_tJointEnd.type=type;
				pLineTube2->m_tJointEnd.hLinkObj=pParamPlate->handle;
				pLineTube2->DesignEndJointUCS(pParamPlate);
			}
			pParamPlate->DesignPlate();
			pParamPlate->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pParamPlate->GetSolidPartObject());
			pLineTube2->SetModified();
			pLineTube2->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pLineTube2->GetSolidPartObject());
			//
			CLsRef *pLsRef=NULL;
			pParamPlate->EmptyLsRef();	//清空螺栓引用
			for(pLsRef=pSrcParamPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcParamPlate->GetNextLsRef())
			{
				CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
				pLsRef->GetLsPtr()->CloneTo(*pBolt);
				pBolt->SetLayer(pParamPlate->layer());
				//pBolt->layer(2)=pLineTube2->layer(2);	//图层名
				pBolt->cfgword=pParamPlate->cfgword;	//调整螺栓配材号与基准构件配材号一致
				pBolt->ReplaceReferenceHandle(CHandleReplacer(pSrcParamPlate->handle,pParamPlate->handle));
				pBolt->SetModified();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pBolt->CalGuigeAuto();
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				CLsRef *pNewLsRef=pParamPlate->AppendLsRef(pBolt->GetLsRef());
				if(pParamPlate->m_iParamType==TYPE_XEND)
				{
					pBolt->EmptyL0DesignPara();	//清空通厚设计参数
					pBolt->des_base_pos.norm_offset.EmptyThick();	//清空法向偏移量设计参数
					pBolt->des_base_pos.norm_offset.AddThick(ftol(-pParamPlate->GetThick()*0.5));
					pNewLsRef->start1_end2_mid0=pLsRef->start1_end2_mid0;
					//基板范围内的螺栓添加通厚参数
					if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
						pBolt->AddL0Thick(pParamPlate->handle,TRUE,TRUE);
				}
			}
			//
			if(pParamPlate->m_iParamType==TYPE_XEND)
			{	//生成十字插板附加钢板
				pEndLjDlg->m_pCurLineTube=pLineTube2;
				//螺栓为克隆螺栓时，布置插板时需明确指出，否则参数将被调整 wht 11-05-27
				pEndLjDlg->LayoutXEndSubPlate(pParamPlate,TRUE);
				if(pEndLjDlg->m_bCreateFillPlate)	//设置十字插板填板 wht 11-05-26
					pEndLjDlg->DesignXEndFillPlate(pParamPlate);
			}
			//设计节点板
			if(pNodePlate1)
			{
				CLDSLineTube *pLineTube=NULL;
				if(pHorizontalPole2&&pHorizontalPole2->IsTube())
					pLineTube=(CLDSLineTube*)pHorizontalPole2;
				pNodePlate2=pEndLjDlg->CreateNodePlate(pBaseLineTube,pLineTube2,pParamPlate,pLineTube);
			}
			for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
			{
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				if(pNodePlate1&&pNodePlate2)
					pBolt->ReplaceReferenceHandle(CHandleReplacer(pNodePlate1->handle,pNodePlate2->handle));
			}
		}
		else
		{
			pEndLjDlg->m_pCurLineTube=pLineTube2;
			if(pEndLjDlg->DoModal()==IDOK&&pEndLjDlg->m_bDesignNodePlate)
			{
				CLDSLineTube *pLineTube=NULL;
				if(pHorizontalPole2&&pHorizontalPole2->IsTube())
					pLineTube=(CLDSLineTube*)pHorizontalPole2;	
				pNodePlate2=pEndLjDlg->CreateNodePlate(pBaseLineTube,NULL,NULL,pLineTube);
			}
			else
				return FALSE;
		}
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

static void DesignMainTubeEndLj(CLDSNode *pBaseNode,CLDSLineTube *pBaseLineTube,CLDSPart *pDatumPlate)
{
	if(pDatumPlate==NULL||pBaseNode==NULL||pBaseLineTube==NULL)
		return;
	TUBEJOINT *pCurJoint=NULL;
	f3dPoint tube_len_vec=pBaseLineTube->End()-pBaseLineTube->Start();
	normalize(tube_len_vec);
	pCurJoint=&pBaseLineTube->m_tJointEnd;
	f3dPoint cur_pos=pBaseLineTube->End();
	if(pBaseNode==pBaseLineTube->pStart)
	{
		tube_len_vec*=-1;
		pCurJoint=&pBaseLineTube->m_tJointStart;
		cur_pos=pBaseLineTube->Start();
	}
	f3dPoint face_pos=pDatumPlate->ucs.origin;
	f3dPoint face_norm=pDatumPlate->ucs.axis_z;
	if(pDatumPlate->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pPlate=(CLDSPlate*)pDatumPlate;
		face_pos+=(pPlate->ucs.axis_z*pPlate->m_fNormOffset);
		if(tube_len_vec*face_norm<0)
		{
			face_norm*=-1.0;
			face_pos+=(pPlate->ucs.axis_z*pPlate->GetThick());
		}
	}
	else if(pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE) 
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pDatumPlate;
		if(tube_len_vec*face_norm<0)
		{
			face_pos+=pDatumPlate->ucs.axis_z*pDatumPlate->GetThick()*0.5;
			face_norm*=-1.0;
		}
		else
			face_pos-=pDatumPlate->ucs.axis_z*pDatumPlate->GetThick()*0.5;
	}
	if(pCurJoint)
	{	//平面切割钢管
		pCurJoint->type=0;
		pCurJoint->cutPlanePos=face_pos;
		pCurJoint->cutPlaneNorm=face_norm;
		if(!face_norm.IsZero())
		{	//如果为平面切割自动计算一个最小的负头
			f3dPoint inters,temp_vec;
			Int3dlf(inters,cur_pos,tube_len_vec,face_pos,face_norm);
			double min_dist=DISTANCE(inters,cur_pos);
			double angle=cal_angle_of_2vec(tube_len_vec,face_norm);
			double tana=fabs(tan(angle));
			//交点==>当前设计端位置
			temp_vec=cur_pos-inters;
			normalize(temp_vec);
			if(temp_vec*tube_len_vec>0)	//需要设置负头
				min_dist*=-1.0;
			min_dist+=(pBaseLineTube->GetDiameter()*0.5*tana);
			if(pBaseNode==pBaseLineTube->pStart)
				pBaseLineTube->SetStartOdd(int(min_dist+1));
			else 
				pBaseLineTube->SetEndOdd(int(min_dist+1));
		}
	}
	pBaseLineTube->SetModified();
	pBaseLineTube->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pBaseLineTube->GetSolidPartObject());
	g_pSolidDraw->Draw();
}
//调整钢板节点板外形---将节点板延伸至环向板或者切割钢板
static void AdjustNodePlateProfile(CLDSView *pView,CLDSNode *pBaseNode,CLDSLineTube *pBaseLineTube,
							CLDSGeneralPlate *pDatumPlate,CLDSPlate *pNodePlate1,CLDSPlate *pNodePlate2)
{	
	if(pDatumPlate==NULL||pView==NULL||pBaseNode==NULL||pBaseLineTube==NULL)
		return;
	f3dPoint tube_len_vec=pBaseLineTube->End()-pBaseLineTube->Start();
	normalize(tube_len_vec);
	if(pBaseNode==pBaseLineTube->pStart)
		tube_len_vec*=-1;
	f3dPoint face_pos=pDatumPlate->ucs.origin;
	f3dPoint face_norm=pDatumPlate->ucs.axis_z;
	double fWidth=0;
	if(pDatumPlate->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pPlate=(CLDSPlate*)pDatumPlate;
		face_pos+=(pPlate->ucs.axis_z*pPlate->m_fNormOffset);
		if(tube_len_vec*face_norm<0)
		{
			face_norm*=-1.0;
			face_pos+=(pPlate->ucs.axis_z*pPlate->GetThick());
		}
		if(!pPlate->m_tInnerColumnNorm.IsZero())
		{	//处理椭圆弧上下平面间的错位差异
			GEPOINT linepick=pPlate->m_tInnerOrigin;
			linepick.z+=pPlate->m_fNormOffset;
			coord_trans(linepick,pPlate->ucs,TRUE,TRUE);
			Int3dlf(face_pos,linepick,tube_len_vec,face_pos,face_norm);
		}
		fWidth=0.5*pDatumPlate->GetWidth()-pPlate->m_fInnerRadius;
	}
	else if(pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE) 
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pDatumPlate;
		if(tube_len_vec*face_norm<0)
		{
			face_pos+=pDatumPlate->ucs.axis_z*pDatumPlate->GetThick()*0.5;
			face_norm*=-1.0;
		}
		else
			face_pos-=pDatumPlate->ucs.axis_z*pDatumPlate->GetThick()*0.5;
		double r=0;
		f3dPoint col_axis;
		pParamPlate->GetInnerCirRadius(r,col_axis);
		fWidth=0.5*pDatumPlate->GetWidth()-r;
	}
	for(int i=0;i<2;i++)
	{
		CLDSPlate *pCurPlate=NULL;
		if(i==0)
			pCurPlate=pNodePlate1;
		else 
			pCurPlate=pNodePlate2;
		if(pCurPlate==NULL)
			continue;
		//切割钢板失败，需要延伸钢板
		if(!pView->CutPlateByPlane(pCurPlate,face_pos,face_norm,0,FALSE))
			pView->ExtendPlateToPlane(pCurPlate,face_pos,face_norm,&tube_len_vec,fWidth);
		//调整钢板与切割面相交边的长度pDatumPlate
		//查找钢板与切割面相交边
		PROFILE_VERTEX *pVertex=NULL,*pWeldStartVertex=NULL,*pWeldEndVertex=NULL;
		CMinDouble btm_start,btm_end;
		UCS_STRU datumcs=pDatumPlate->ucs;
		datumcs.origin=face_pos;
		for(pVertex=pCurPlate->vertex_list.GetFirst();pVertex;pVertex=pCurPlate->vertex_list.GetNext())
		{
			f3dPoint vertex=pVertex->vertex;
			coord_trans(vertex,pCurPlate->ucs,TRUE);
			//以钢板中性层为求交基准
			double offsetToMidPlane=pCurPlate->GetNormOffset()+pCurPlate->Thick*0.5;
			if(fabs(offsetToMidPlane)>EPS)
				vertex+=offsetToMidPlane*pCurPlate->ucs.axis_z;
			coord_trans(vertex,datumcs,FALSE);
			btm_start.Update(fabs(vertex.z),pVertex,1);
			if(btm_start.m_pRelaObj!=pVertex)
				btm_end.Update(fabs(vertex.z),pVertex,1);
		}
		if(btm_start.IsInited()&&btm_end.IsInited()&&fabs(btm_start.number-btm_end.number)<1)
		{	//找到与基准底板相贴合的最低边
			pWeldStartVertex=(PROFILE_VERTEX*)btm_start.m_pRelaObj;
			pWeldEndVertex=(PROFILE_VERTEX*)btm_end.m_pRelaObj;
			PROFILE_VERTEX *pPrevVertex=pDatumPlate->vertex_list.GetTail();
			bool inters_finded[2]={false};
			GEPOINT inters_arr[4];
			f3dLine L(pWeldStartVertex->vertex,pWeldEndVertex->vertex);
			f3dPoint edge_vec=L.endPt-L.startPt;
			edge_vec.z=0;
			normalize(edge_vec);
			f3dLine line3d(L.startPt,L.endPt);
			coord_trans(line3d.startPt,pCurPlate->ucs,TRUE);
			coord_trans(line3d.endPt,pCurPlate->ucs,TRUE);
			vector_trans(edge_vec,pCurPlate->ucs,TRUE);
			coord_trans(line3d.startPt,datumcs,FALSE);
			coord_trans(line3d.endPt,datumcs,FALSE);
			vector_trans(edge_vec,datumcs,FALSE);
			bool bDatumWeldStart=false;
			if(line3d.startPt.mod2()>line3d.endPt.mod2())
				bDatumWeldStart=true;
			line3d.startPt-=edge_vec*10000;
			line3d.endPt+=edge_vec*10000;

			f2dLine L1(line3d.startPt,line3d.endPt);
			for(pVertex=pDatumPlate->vertex_list.GetFirst();pVertex;pVertex=pDatumPlate->vertex_list.GetNext())
			{
				double inters_x,inters_y;
				int rslt=0;
				if(pPrevVertex->type==0)
				{
					if(Int2dll(L1,f2dLine(pPrevVertex->vertex,pVertex->vertex),inters_x,inters_y)>0)
					{
						if(!inters_finded[0])
						{
							inters_finded[0]=true;
							inters_arr[0].Set(inters_x,inters_y);
						}
						else if(!inters_finded[1])
						{
							inters_finded[1]=true;
							inters_arr[1].Set(inters_x,inters_y);
						}
					}
				}
				else
				{
					f3dArcLine arcline;
					pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,&datumcs);
					f2dArc arc;
					arc.centre=arcline.Center();
					arc.angle=arcline.SectorAngle();
					if(arcline.WorkNorm().z>0)
						arc.startPt=pPrevVertex->vertex;
					else
						arc.startPt=pVertex->vertex;
					double inters_x2,inters_y2;
					rslt=Int2dla(L1,arc,inters_x,inters_y,inters_x2,inters_y2);
					for(int iii=0;iii<rslt;iii++)
					{
						if(!inters_finded[0])
						{
							inters_finded[0]=true;
							inters_arr[0].Set(inters_x,inters_y);
						}
						else if(!inters_finded[1])
						{
							inters_finded[1]=true;
							inters_arr[1].Set(inters_x,inters_y);
						}
					}
				}
				if(inters_finded[0]&&inters_finded[1])
					break;
				pPrevVertex=pVertex;
			}
			f2dCircle circle;
			GEPOINT column_norm;
			if(pDatumPlate->GetClassTypeId()==CLS_PLATE)
			{
				circle.radius=((CLDSPlate*)pDatumPlate)->m_fInnerRadius;
				column_norm=((CLDSPlate*)pDatumPlate)->m_tInnerColumnNorm;
			}
			else if(!((CLDSParamPlate*)pDatumPlate)->GetInnerCirRadius(circle.radius,column_norm))
				circle.radius=0;
			int inters_count=0;
			if(circle.radius>EPS)
			{
				if(!column_norm.IsZero())
					vector_trans(column_norm,datumcs,FALSE,TRUE);
				if(fabs(column_norm.x)+fabs(column_norm.y)<EPS)
					inters_count=Int2dlc(L1,circle,inters_arr[2].x,inters_arr[2].y,inters_arr[3].x,inters_arr[3].y);
				else	//椭圆
				{
					GEPOINT arc_start(1,1,0);
					double cosa=fabs(column_norm.z);
					GEPOINT long_axis(column_norm.x,column_norm.y);
					normalize(long_axis);
					long_axis*=(circle.radius/(cosa>0?cosa:1));
					inters_count=Int2dle(inters_arr[2],inters_arr[3],line3d.startPt,line3d.endPt,GEPOINT(0,0,0),arc_start,circle.radius,Pi+Pi,long_axis);
				}
			}
			f3dPoint rayEdgeVec;
			if(bDatumWeldStart)
			{
				rayEdgeVec=-edge_vec;
				f3dPoint inters;
				if(inters_arr[0]*rayEdgeVec>inters_arr[1]*rayEdgeVec)
					inters=inters_arr[0];
				else
					inters=inters_arr[1];
				GEPOINT intersI=inters_arr[2];
				if( inters_count==2&&
					DISTANCE(inters,inters_arr[2])>DISTANCE(inters,inters_arr[3]))
						intersI=inters_arr[3];
				coord_trans(inters,datumcs,TRUE);
				coord_trans(inters,pCurPlate->ucs,FALSE);
				pWeldStartVertex->vertex.Set(inters.x,inters.y);
				if(inters_count>0)
				{
					coord_trans(intersI,datumcs,TRUE);
					coord_trans(intersI,pCurPlate->ucs,FALSE);
					pWeldEndVertex->vertex.Set(intersI.x,intersI.y);
				}
			}
			else
			{
				rayEdgeVec= edge_vec;
				f3dPoint inters;
				if(inters_arr[0]*rayEdgeVec>inters_arr[1]*rayEdgeVec)
					inters=inters_arr[0];
				else
					inters=inters_arr[1];
				GEPOINT intersI=inters_arr[2];
				if( inters_count==2&&
					DISTANCE(inters,inters_arr[2])>DISTANCE(inters,inters_arr[3]))
					intersI=inters_arr[3];
				if(inters_count>0)
				{
					coord_trans(intersI,datumcs,TRUE);
					coord_trans(intersI,pCurPlate->ucs,FALSE);
					pWeldStartVertex->vertex.Set(intersI.x,intersI.y);
				}
				coord_trans(inters,datumcs,TRUE);
				coord_trans(inters,pCurPlate->ucs,FALSE);
				pWeldEndVertex->vertex.Set(inters.x,inters.y);
			}
			pCurPlate->SetModified();
		}
		pCurPlate->designInfo.m_bEnableFlexibleDesign=FALSE;
	}
	if(pNodePlate1)
	{
		pNodePlate1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pNodePlate1->GetSolidPartObject());
	}
	if(pNodePlate2)
	{
		pNodePlate2->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pNodePlate2->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
}

typedef CLDSPart*  PART_PTR;
static int ComparePlateByAxisX(const PART_PTR &pPart1,const PART_PTR &pPart2)
{
	if(pPart1==NULL||pPart2==NULL)
		return 0;
	double angle1=Cal2dLineAng(0,0,pPart1->ucs.axis_x.x,pPart1->ucs.axis_x.y);
	double angle2=Cal2dLineAng(0,0,pPart2->ucs.axis_x.x,pPart2->ucs.axis_x.y);
	if(angle1>angle2)
		return 1;
	else if(angle1<angle2)
		return -1;
	return 0;
}
//特殊法兰设计(塔脚水平法兰、变坡处异形法兰)
void CLDSView::OnDesignSpecialFl() 
{	
	m_nPrevCommandID=ID_DESIGN_SPECIAL_FL;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat tower foot && Changing slope design";
#else
	m_sPrevCommandName="重复塔脚&&变坡设计";
#endif
	DesignSpecialFl();
}

//将【塔脚&变坡设计】分为两个命令 wht 11-05-20
//塔脚设计
void CLDSView::OnDesignTubeTowerFoot() 
{
	m_nPrevCommandID=ID_DESIGN_TUBE_TOWER_FOOT;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat tower foot design";
#else
	m_sPrevCommandName="重复塔脚设计";
#endif
	DesignSpecialFl();
}
//变坡设计 
void CLDSView::OnDesignTubeTowerSlope() 
{
	m_nPrevCommandID=ID_DESIGN_TUBE_TOWER_SLOPE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat changing slope design";
#else
	m_sPrevCommandName="重复变坡设计";
#endif
	DesignSpecialFl(TRUE);
}

void CLDSView::DesignSpecialFl(BOOL bDesignSlope/*=FALSE*/)
{
#ifdef __COMPLEX_PART_INC_
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return;
	CString cmdStr,errCmdStr;
	DWORD dwhObj=0,dwExportFlag=0;
	g_pSolidDraw->ReleaseSnapStatus();
	//选择基准节点
	CLDSNode *pBaseNode=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("%s please select datum node:",bDesignSlope?"SpecialFl":"TubeFoot");
#else
	cmdStr.Format("%s 请选择基准节点:",bDesignSlope?"SpecialFl":"TubeFoot");
#endif
	CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_POINT);
	verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
	{//控制凸出显示节点的生命周期
	CDisplayNodeAtFrontLife displayNode;
	displayNode.DisplayNodeAtFront();
	while(pBaseNode==NULL)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
		{
			pCmdLine->CancelCmdLine();
			return;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		pBaseNode=Ta.Node.FromHandle(obj.hRelaObj);
	}
	}
	pCmdLine->FinishCmdLine(CXhChar50("节点0X%X",pBaseNode->handle));
	g_pSolidDraw->SetEntSnapStatus(pBaseNode->handle);
	//选择基准钢管
	CLDSLineTube *pUpBaseTube=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("%s please select datum tube:",bDesignSlope?"SpecialFl":"TubeFoot");
#else
	cmdStr.Format("%s 请选择基准钢管:",bDesignSlope?"SpecialFl":"TubeFoot");
#endif
	verifier.ClearSnapFlag();
	verifier.SetVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
	verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINETUBE));
	while(pUpBaseTube==NULL)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
		{
			pCmdLine->CancelCmdLine();
			return;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		pUpBaseTube=(CLDSLineTube*)Ta.FromPartHandle(obj.hRelaObj,CLS_LINETUBE);
	}
	pCmdLine->FinishCmdLine(CXhChar50("钢管0X%X",pUpBaseTube->handle));
	g_pSolidDraw->SetEntSnapStatus(pUpBaseTube->handle);
	//选择变坡出第二根基准钢管
	CLDSLineTube *pDownBaseTube=NULL;
	if(bDesignSlope)
	{	//变坡设计
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("SpecialFl please second the second datum tube:");
#else
		cmdStr.Format("SpecialFl 请选择第二根依赖基准钢管:");
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		while(pDownBaseTube==NULL)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			pDownBaseTube=(CLDSLineTube*)Ta.FromPartHandle(obj.hRelaObj,CLS_LINETUBE);
		}
		pCmdLine->FinishCmdLine(CXhChar50("钢管0X%X",pDownBaseTube->handle));
		g_pSolidDraw->SetEntSnapStatus(pDownBaseTube->handle);
	}
	//切换到实体显示模式
	DISPLAY_TYPE displayType;
	g_pSolidSet->GetDisplayType(&displayType);
	if(displayType!=DISP_SOLID)
	{
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		Invalidate(FALSE);
	}
	//调整两基准钢管位置
	CLDSLineTube* pDatumTube=pUpBaseTube;
	double fMidZ=0.5*(pUpBaseTube->Start().z+pUpBaseTube->End().z);
	if(fMidZ-pBaseNode->Position(true).z>0)
	{
		CLDSLineTube *pTempTube=pDownBaseTube;
		pDownBaseTube=pUpBaseTube;
		pUpBaseTube=pTempTube;
	}
	//根据节点查找射线杆件
	LINEPARTSET UpLinePartSet,horiRodSet,DownLinePartSet;
	CLDSLinePart *pLinePart=NULL,*pRayLinePart1=NULL,*pRayLinePart2=NULL;
	for(pLinePart=Ta.Parts.GetFirstLinePart();pLinePart;pLinePart=Ta.Parts.GetNextLinePart())
	{
		if(pDownBaseTube&&pLinePart->handle==pDownBaseTube->handle)
			continue;
		if(pUpBaseTube&&pLinePart->handle==pUpBaseTube->handle)
			continue;
		f3dPoint line_vec;
		if(pLinePart->pStart==pBaseNode)
			line_vec=pLinePart->pEnd->Position(true)-pLinePart->pStart->Position(true);
		else if(pLinePart->pEnd==pBaseNode)
			line_vec=pLinePart->pStart->Position(true)-pLinePart->pEnd->Position(true);
		else
			continue;
		normalize(line_vec);
		double cosa=f3dPoint(0,0,1)*line_vec;
		if(fabs(cosa)<EPS2) //横隔面射线杆件
			horiRodSet.append(pLinePart);
		else if(cosa<0)	//上侧射线杆件
			UpLinePartSet.append(pLinePart);
		else			//下侧射线杆件
			DownLinePartSet.append(pLinePart);
	}
	static CDesignTubeEndLjDlg endLjDlg;
	endLjDlg.m_pBaseNode=pBaseNode;
	endLjDlg.m_bDesignNodePlate=TRUE;
	CUndoOperObject undo(&Ta,true);
	if((pUpBaseTube&&pDownBaseTube==NULL)||(pDownBaseTube&&pUpBaseTube==NULL))
	{
		//设计钢管插板
		CLDSParamPlate *pSrcParamPlate=NULL;
		CLDSLineTube *pLineTube1=NULL,*pLineTube2=NULL;
		CLDSPlate *pNodePlate[2]={NULL,NULL};
		//设计节点板
		CLDSLineTube *pBaseLineTube=NULL;
		if(pUpBaseTube)
		{
			pBaseLineTube=pUpBaseTube;
			DesignRayPoleAndPlate(pUpBaseTube,&endLjDlg,UpLinePartSet,NULL,pNodePlate[0],pNodePlate[1]);
		}
		else if(pDownBaseTube)
		{
			pBaseLineTube=pDownBaseTube;
			DesignRayPoleAndPlate(pDownBaseTube,&endLjDlg,DownLinePartSet,NULL,pNodePlate[0],pNodePlate[1]);
		}
		if(pBaseLineTube==NULL)
			return;
		//设计环向板，节点板，环向封板，环向肋板，修正节点板
		CDesignSpecialFlDlg specialFlDlg;
		specialFlDlg.m_pRayLinePart[0]=UpLinePartSet.GetFirst();
		specialFlDlg.m_pRayLinePart[1]=UpLinePartSet.GetNext();
		specialFlDlg.m_pNodePlate[0]=pNodePlate[0];
		specialFlDlg.m_pNodePlate[1]=pNodePlate[1];
		specialFlDlg.m_pBaseNode=pBaseNode;
		specialFlDlg.m_pCurLineTube=pBaseLineTube;
		if(specialFlDlg.DoModal()==IDOK)
		{	//调整钢板节点板外形---将节点板延伸至环向板或者切割钢板
			CLDSPlate *pDatumPlate=specialFlDlg.m_pCirPlate;
			AdjustNodePlateProfile(this,pBaseNode,pBaseLineTube,pDatumPlate,pNodePlate[0],pNodePlate[1]);
		}
	}
	else if(horiRodSet.GetNodeNum()<2)
		AfxMessageBox("水平连接的横材杆件少于2根");
	else if(horiRodSet.GetNodeNum()>=2&&pUpBaseTube&&pDownBaseTube)
	{	//变坡处异形法兰连接
		//1>.生成横材插板
		CLDSPlate *pNodePlate[4]={NULL,NULL};
		endLjDlg.m_bDesignNodePlate=FALSE;
		endLjDlg.m_bSpecialFlRayPole=TRUE;	//异形法兰连接的射线杆件
		DesignRayPoleAndPlate(pDownBaseTube,&endLjDlg,horiRodSet,NULL,pNodePlate[0],pNodePlate[1]);
		//2>.生成上下侧斜材钢管端头插板
		//3>.生成上下侧节点板,如果横材为十字插板
		//   则引入相关螺栓重新计算外形,调整立板法线同所在坡面法线一致
		endLjDlg.m_bDesignNodePlate=TRUE;
		endLjDlg.m_bSpecialFlRayPole=FALSE;	//异形法兰连接的射线杆件
		DesignRayPoleAndPlate(pUpBaseTube,&endLjDlg,UpLinePartSet,&horiRodSet,pNodePlate[0],pNodePlate[1]);
		DesignRayPoleAndPlate(pDownBaseTube,&endLjDlg,DownLinePartSet,&horiRodSet,pNodePlate[2],pNodePlate[3]);
		//4>.生成异形法兰,并环向布置螺栓
		CDesignCircularPlateDlg cirPlateDlg;
		cirPlateDlg.m_pDatumNode=pBaseNode;
		cirPlateDlg.m_pDatumTube=pDatumTube;
		cirPlateDlg.m_bReadOnlyPlateType=TRUE;
		cirPlateDlg.idPlateType=CDesignCircularPlateDlg::TYPE_SPECIAL_FL;
		cirPlateDlg.design_axis_z.vector.Set(0,0,1);
		cirPlateDlg.design_axis_z.norm_style=0;
		for(CLDSLinePart *pLinePart=horiRodSet.GetFirst();pLinePart;pLinePart=horiRodSet.GetNext())
			cirPlateDlg.AddRelaPole(pLinePart);
		if(cirPlateDlg.DoModal()!=IDOK)
			return;
		CLDSParamPlate *pParamPlate=cirPlateDlg.CreateCircularPlate(NULL);
		//5>.使用异形法兰切割节点板
		AdjustNodePlateProfile(this,pBaseNode,pUpBaseTube,pParamPlate,pNodePlate[0],pNodePlate[1]);
		AdjustNodePlateProfile(this,pBaseNode,pDownBaseTube,pParamPlate,pNodePlate[2],pNodePlate[3]);
		//6>.平面切割基准钢管
		DesignMainTubeEndLj(pBaseNode,pUpBaseTube,pParamPlate);
		DesignMainTubeEndLj(pBaseNode,pDownBaseTube,pParamPlate);
		/* 实际使用过程中下属附属板还是由用户自行手动添加更方便 wjh-2018.3.1
		//7>.在上侧节点板上生成上封板，下侧节点板上生成下封板
		CDesignRibbedPlateDlg ribPlateDlg,elbowPlateDlg;
		CLDSParamPlate *pCoverPlate[2]={NULL};
		CLDSLineTube *pBaseTube[2]={pUpBaseTube,pDownBaseTube};
		for(int i=0;i<2;i++)
		{
			if(pNodePlate[2*i]==NULL||pNodePlate[2*i+1]==NULL)
				continue;
			CDesignCircularPlateDlg coverPlateDlg;
			coverPlateDlg.m_pDatumNode=pBaseNode;
			coverPlateDlg.m_pDatumTube=pBaseTube[i];
			coverPlateDlg.m_pRelaPart1=pNodePlate[2*i];
			coverPlateDlg.m_pRelaPart2=pNodePlate[2*i+1];
			//coverPlateDlg.m_iPlateType=1;	//封板
			coverPlateDlg.idPlateType=CDesignCircularPlateDlg::TYPE_COVERPLATE2;
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pNodePlate[2*i]->handle);
			g_pSolidDraw->SetEntSnapStatus(pNodePlate[2*i+1]->handle);
			if(i==0)
				coverPlateDlg.para.S=1;	//上侧封板
			else 
				coverPlateDlg.para.S=-1;	//下侧封板
			if(coverPlateDlg.DoModal()!=IDOK)
				return;
			coverPlateDlg.CreateCircularPlate(NULL);
			pCoverPlate[i]=coverPlateDlg.m_pCirPlate;
			//8>.以封板+异形法兰为基准在节点板相对位置生成两块肋板
			ribPlateDlg.m_pDatumPart=pBaseTube[i];
			ribPlateDlg.m_pRelaPart1=pParamPlate;
			ribPlateDlg.m_pRelaPart2=pCoverPlate[i];
			g_pSolidDraw->ReleaseSnapStatus();
			if(pParamPlate)
				g_pSolidDraw->SetEntSnapStatus(pParamPlate->handle);
			if(pCoverPlate[i])
				g_pSolidDraw->SetEntSnapStatus(pCoverPlate[i]->handle);
			if(pParamPlate==NULL||pCoverPlate[i]==NULL)
				continue;
			if(ribPlateDlg.DoModal()!=IDOK)
				return;
			CLDSPart *PlatePtrArr[4]={NULL};
			PlatePtrArr[0]=pNodePlate[2*i];
			PlatePtrArr[1]=pNodePlate[2*i+1];
			PlatePtrArr[2]=ribPlateDlg.GetRibPlate();
			PlatePtrArr[3]=ribPlateDlg.ribPlateSet.GetFirst();
			CBubbleSort<CLDSPart*>::BubSort(PlatePtrArr,4,ComparePlateByAxisX);
			//9>.以节点板及肋板为基准在两两之间生成环向肋板
			CDesignCircularPlateDlg xCirPlateDlg;
			//xCirPlateDlg.m_iPlateType=3;	//3WA环向肋板
			xCirPlateDlg.idPlateType=CDesignCircularPlateDlg::TYPE_3WA_2;
			xCirPlateDlg.m_pDatumNode=pBaseNode;
			xCirPlateDlg.m_pDatumTube=pBaseTube[i];
			xCirPlateDlg.m_pRelaPart1=PlatePtrArr[0];
			xCirPlateDlg.m_pRelaPart2=PlatePtrArr[1];
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(PlatePtrArr[0]->handle);
			g_pSolidDraw->SetEntSnapStatus(PlatePtrArr[1]->handle);
			if(xCirPlateDlg.DoModal()!=IDOK)
				return;
			xCirPlateDlg.CreateCircularPlate(NULL);
			CLDSParamPlate *pScrCirRibPlate=xCirPlateDlg.m_pCirPlate;
			for(int j=1;j<4;j++)
			{
				if(PlatePtrArr[j]==NULL || PlatePtrArr[(j+1)%4]==NULL)
					continue;
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
				pMirPlate->CopyProperty(pScrCirRibPlate);
				pMirPlate->CopyModuleInstanceInfo(pDownBaseTube);
				pMirPlate->m_hPartWeldParent = pDownBaseTube->handle;
				pMirPlate->SetDesignItemValue('E',PlatePtrArr[j]->handle,NULL,PARAM_POS,1);
				pMirPlate->SetDesignItemValue('F',PlatePtrArr[(j+1)%4]->handle,NULL,PARAM_POS,1);
				pMirPlate->DesignPlate();
				pMirPlate->SetModified();
				pMirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pMirPlate->GetSolidPartObject());
			}
			//10>.以肋板和异形法兰为基准生成加劲肋板
			elbowPlateDlg.m_pDatumPart=pBaseTube[i];
			elbowPlateDlg.m_pRelaPart1=pParamPlate;
			elbowPlateDlg.m_pRelaPart2=pScrCirRibPlate;
			g_pSolidDraw->ReleaseSnapStatus();
			if(pParamPlate)
				g_pSolidDraw->SetEntSnapStatus(pParamPlate->handle);
			if(pScrCirRibPlate)
				g_pSolidDraw->SetEntSnapStatus(pScrCirRibPlate->handle);
			if(elbowPlateDlg.DoModal()!=IDOK)
				return;
		}*/
		/*
		//设计异形法兰,环向板，封板，肋板，肘板
		CDesignSpecialFlDlg specialFlDlg;
		specialFlDlg.m_pBaseNode=pBaseNode;
		specialFlDlg.m_pCurLineTube=pUpBaseTube;
		specialFlDlg.m_pCoupleTube=pDownBaseTube;
		specialFlDlg.m_pRayLinePart[0]=UpLinePartSet.GetFirst();
		specialFlDlg.m_pRayLinePart[1]=UpLinePartSet.GetNext();
		specialFlDlg.m_pRayLinePart[2]=DownLinePartSet.GetFirst();
		specialFlDlg.m_pRayLinePart[3]=DownLinePartSet.GetNext();
		specialFlDlg.m_pNodePlate[0]=pNodePlate[0];
		specialFlDlg.m_pNodePlate[1]=pNodePlate[1];
		specialFlDlg.m_pNodePlate[2]=pNodePlate[2];
		specialFlDlg.m_pNodePlate[3]=pNodePlate[3];
		specialFlDlg.m_pLinePartSet=&horiRodSet;
		if(specialFlDlg.DoModal()==IDOK)
		{	//调整钢板节点板外形---将节点板延伸至环向板或者切割钢板
			CLDSPlate *pDatumPlate=specialFlDlg.GetCirPlate(0);
			if(specialFlDlg.m_iLjStype>=1)
			{
				AdjustNodePlateProfile(this,pBaseNode,pUpBaseTube,pDatumPlate,pNodePlate[0],pNodePlate[1]);
				if(specialFlDlg.m_iLjStype>1)
					pDatumPlate=specialFlDlg.GetCirPlate(1);
				AdjustNodePlateProfile(this,pBaseNode,pDownBaseTube,pDatumPlate,pNodePlate[2],pNodePlate[3]);
			}
		}*/
	}
	//切换到单线显示模式
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	pCmdLine->FinishCmdLine();
	g_pSolidDraw->Draw();
#endif
}

//制弯钢板
void CLDSView::OnMakePlateBend() 
{
	m_nPrevCommandID=ID_MAKE_PLATE_BEND;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="bending plate";
#else
	m_sPrevCommandName="制弯钢板";
#endif
	BendPlate();
}

int CLDSView::BendPlate()
{
	f3dPoint vertice;
	CString cmdStr,promptStr;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSPlate *pDatumPlate=NULL;
	//选择基准钢板
	long *id_arr,nCount=0;
	CSnapTypeVerify verifier;
	DWORD dwhObj=0,dwExportFlag=0;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
		pDatumPlate=(CLDSPlate*)console.FromPartHandle(id_arr[0],CLS_PLATE);
	if(pDatumPlate==NULL||(pDatumPlate&&pDatumPlate->GetClassTypeId()!=CLS_PLATE))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("MakePlateBend please select to-be-bent datum plate:","");
#else
		pCmdLine->FillCmdLine("MakePlateBend 请选择需要制弯的基准钢板:","");
#endif
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE));
		while(true)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				return 0;
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;
			pDatumPlate=(CLDSPlate*)Ta.Parts.FromHandle(dwhObj);
			if(pDatumPlate!=NULL&&pDatumPlate->GetClassTypeId()==CLS_PLATE)
				break;
		}
		if(pDatumPlate->vertex_list.GetNodeNum()<3)
			return FALSE;	
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pDatumPlate->handle));
		g_pSolidDraw->SetEntSnapStatus(pDatumPlate->handle);
	}
	if(pDatumPlate==NULL)
		return FALSE;
	if(pDatumPlate->face_N>=3)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("the plate selected is three-plane plate, unable to continue bending!");
#else
		AfxMessageBox("所选钢板为三面板已无法继续进行制弯!");
#endif
		return FALSE;
	}
	Ta.BeginUndoListen();
	//显示制弯钢板坐标系
	UCS_STRU object_ucs,ucs;
	ucs = pDatumPlate->ucs;
	g_pSolidSet->GetObjectUcs(object_ucs);
	coord_trans(ucs.origin,object_ucs,TRUE);
	vector_trans(ucs.axis_x,object_ucs,TRUE);
	vector_trans(ucs.axis_y,object_ucs,TRUE);
	vector_trans(ucs.axis_z,object_ucs,TRUE);
	g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
	//g_pSolidSet->SetDispWorkUcs();
	Invalidate(FALSE);
	//选择制弯基准角钢
	CLDSLineAngle *pDatumLineAngle=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("MakePlateBend Please select bending datum angle, if no datum angle, specify plane to bend plate<no selection is permitted,press enter to determine>:","");
#else
	pCmdLine->FillCmdLine("MakePlateBend 请选择火曲基准角钢,无基准角钢时通过指定平面火曲钢板<可不选,按回车确定>:","");
#endif
	verifier.ClearSnapFlag();
	verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE));
	while(1)
	{
 		if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
 			return 0;
 		SELOBJ obj(dwhObj,dwExportFlag);
 		dwhObj=obj.hRelaObj;
 		if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
 			break;
 		pDatumLineAngle=(CLDSLineAngle*)console.FromPartHandle(dwhObj);
 		if(pDatumLineAngle!=NULL&&pDatumLineAngle->GetClassTypeId()==CLS_LINEANGLE)
 			break;
	}
	long hDatumAngle=0;
	if(pDatumLineAngle)
		hDatumAngle=pDatumLineAngle->handle;
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",hDatumAngle));
	g_pSolidDraw->SetEntSnapStatus(hDatumAngle);
	//
	g_pSolidDraw->ReleaseSnapStatus();
	SCOPE_STRU scope;
	BOOL bBendPlate=FALSE;
	f3dPoint huoqu_norm;	//火曲面法线
	PROFILE_VERTEX *pPrevVertex=NULL,*pVertex=NULL,*pHuoquStartVertex=NULL,*pHuoquEndVertex=NULL;
	f3dPoint huoquline_vec;
	if(pDatumLineAngle==NULL)
	{
		CMakePlateBendDlg bendDlg;
		bendDlg.m_pPlate=pDatumPlate;
		if(bendDlg.DoModal()==IDOK)
		{
			f3dPoint inter_pick,inter_line_vec;
			int nRetCode=Int3d2f(&inter_pick,&inter_line_vec,bendDlg.huoqu_pos,
				bendDlg.huoqu_norm,bendDlg.datum_plane_pos,bendDlg.datum_plane_norm);
			normalize(inter_line_vec);
			if(nRetCode!=1)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("specified plane has no connection with datum plate!");
#else
				AfxMessageBox("指定平面与基准钢板不相交!");
#endif
				return FALSE;
			}	
			//计算两面交线与基准钢板轮廓边的交点
			f3dLine inters_line;
			coord_trans(inter_pick,pDatumPlate->ucs,FALSE);
			vector_trans(inter_line_vec,pDatumPlate->ucs,FALSE);
			inter_pick.z=inter_line_vec.z=0;
			inters_line.startPt=inter_pick-inter_line_vec*20000;
			inters_line.endPt=inter_pick+inter_line_vec*20000;
			//
			if(pDatumPlate->face_N==2)
			{
				if(inters_line.PtInLine(pDatumPlate->HuoQuLine[0].startPt)
					&&inters_line.PtInLine(pDatumPlate->HuoQuLine[0].endPt))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("error in bending circular plane input, intersecting line of two planes coincides with the above bending curve if datum plate!");
#else
					AfxMessageBox("输入的火曲平面有误，两平面交线与基准钢板前一条火曲线重合!");
#endif
					return FALSE;
				}
			}
			pPrevVertex=pDatumPlate->vertex_list.GetTail();
			for(pVertex=pDatumPlate->vertex_list.GetFirst();pVertex;pVertex=pDatumPlate->vertex_list.GetNext())
			{
				if(pHuoquStartVertex&&pHuoquEndVertex)
					break;
				f3dPoint inter_vertex,inter_vertex2;
				if(pPrevVertex->type==0)
				{
					f3dLine line;
					line.startPt=pVertex->vertex;
					line.endPt=pPrevVertex->vertex;
					if(Int3dll(line,inters_line,inter_vertex)>0)
					{
						if(pHuoquStartVertex==NULL)
							pHuoquStartVertex=pDatumPlate->vertex_list.insert(PROFILE_VERTEX(inter_vertex));
						else //if(pEndVertex==NULL)
							pHuoquEndVertex=pDatumPlate->vertex_list.insert(PROFILE_VERTEX(inter_vertex));
					}
				}
				else if(pPrevVertex->type<4)
				{
					int sign=1;
					f3dArcLine arcLine;
					if(pPrevVertex->type==1)//指定圆弧夹角(自动计算圆弧夹角的情况现在已自动计算出精确值)
					{
						if(pPrevVertex->sector_angle>0)
							sign=-1;
						arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),pPrevVertex->sector_angle);
					}
					else if(pPrevVertex->type==2)
					{	//指定R
						if(pPrevVertex->radius<0)
							sign=-1;
						arcLine.CreateMethod3(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),sign*pPrevVertex->radius,pPrevVertex->center);
						//不能用CreateMethod2,否则丢失了拾取点信息，对于大于180°的弧是错的。 wht 12-06-14
						//arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),sector_angle);
					}
					else if(pPrevVertex->type==3)	//椭圆弧
					{
						f3dPoint center = pPrevVertex->center;
						f3dPoint col_axis=pPrevVertex->column_norm;
						coord_trans(center,pDatumPlate->ucs,FALSE);
						vector_trans(col_axis,pDatumPlate->ucs,FALSE);
						if(pPrevVertex->radius<0)
							sign=-1;
						arcLine.CreateEllipse(center,pPrevVertex->vertex,pVertex->vertex,col_axis,f3dPoint(0,0,sign),sign*pPrevVertex->radius);
					}
					f2dArc arc;
					arc.angle=arcLine.SectorAngle();
					arc.centre=arcLine.Center();
					if(arcLine.WorkNorm().z>0)
						arc.startPt=arcLine.Start();
					else
						arc.startPt=arcLine.End();
					f2dLine huoqu_line2d;
					huoqu_line2d.startPt.Set(inters_line.startPt.x,inters_line.startPt.y);
					huoqu_line2d.endPt.Set(inters_line.endPt.x,inters_line.endPt.y);
					int nRetCode=Int2dla(huoqu_line2d,arc,inter_vertex.x,inter_vertex.y,inter_vertex2.x,inter_vertex2.y);
					if(nRetCode>0)
					{	//此处暂未考虑直线与(椭)圆弧有两交点时的交点取舍问题
						if(pPrevVertex->type==3)
						{	//椭圆弧
							double radius=fabs(pPrevVertex->radius);
							f3dPoint inters1,inters2;
							f3dPoint pick=inter_pick,vec=inter_line_vec;
							coord_trans(pick,pDatumPlate->ucs,TRUE);
							vector_trans(vec,pDatumPlate->ucs,TRUE);
							int nRet=Int3dlc(&inters1,&inters2,pick,vec,pPrevVertex->center-pPrevVertex->column_norm*1000,
								pPrevVertex->center+pPrevVertex->column_norm*1000,radius,radius);
							if(nRet>0)
							{
								coord_trans(inters1,pDatumPlate->ucs,FALSE);
								coord_trans(inters2,pDatumPlate->ucs,FALSE);
								inters1.z=inters2.z=0;
								if(nRet==2&&DISTANCE(inter_vertex,inters1)>DISTANCE(inter_vertex,inters2))
									inter_vertex=inters2;
								else
									inter_vertex=inters1;
							}
						}
						PROFILE_VERTEX *pArcVertex=pDatumPlate->vertex_list.insert(*pPrevVertex);
						pArcVertex->vertex=inter_vertex;
						if(pArcVertex->type==1)
						{	//指定圆弧夹角
							int sign=1;
							if(pPrevVertex->sector_angle<0)
								sign=-1;
							double half_len = 0.5*DISTANCE(pPrevVertex->vertex,inter_vertex);
							if(half_len>=fabs(pPrevVertex->radius))
								pPrevVertex->sector_angle = sign*Pi;
							else
								pPrevVertex->sector_angle = sign*2*asin(half_len/fabs(pPrevVertex->radius));
							pArcVertex->sector_angle=sign*(fabs(arc.angle)-fabs(pPrevVertex->sector_angle));
						}
						if(pHuoquStartVertex==NULL)
							pHuoquStartVertex=pArcVertex;
						else //if(pEndVertex==NULL)
							pHuoquEndVertex=pArcVertex;
					}
				}
				pPrevVertex=pVertex;
			}
			if(pHuoquStartVertex==NULL||pHuoquEndVertex==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Two plane's intersecting line has no intersection point with the outline edge of the datum plate!");
#else
				AfxMessageBox("两平面交线与基准钢板轮廓边无交点!");
#endif
				return FALSE;
			}
			//纠正火曲线方向，保证输入的火曲面为第2或3火曲面 
			huoquline_vec=pHuoquEndVertex->vertex-pHuoquStartVertex->vertex;
			normalize(huoquline_vec);
			vector_trans(huoquline_vec,pDatumPlate->ucs,TRUE);
			f3dPoint verfiy_vec=bendDlg.datum_plane_norm^bendDlg.huoqu_norm;
			if(huoquline_vec*verfiy_vec>0)
			{
				pVertex=pHuoquEndVertex;
				pHuoquEndVertex=pHuoquStartVertex;
				pHuoquStartVertex=pVertex;
				huoquline_vec*=-1.0;
			}
			scope.VerifyVertex(pHuoquStartVertex->vertex); 
			scope.VerifyVertex(pHuoquEndVertex->vertex);
			pDatumPlate->face_N++;
			if(pDatumPlate->face_N==3)
				pDatumPlate->jdb_style=1;//之前折叠板赋值为3是错的会与钢管夹板混淆，现重新修正为1，并增加IsFoldPlate()函数 wjh-2016.1.09
			pDatumPlate->HuoQuLine[pDatumPlate->face_N-2].startPt=pHuoquStartVertex->vertex;
			pDatumPlate->HuoQuLine[pDatumPlate->face_N-2].endPt=pHuoquEndVertex->vertex;
			pDatumPlate->huoqufacenorm[pDatumPlate->face_N-2]=bendDlg.huoqu_norm;
			huoqu_norm=bendDlg.huoqu_norm;	//记录火曲面法线
			bBendPlate=TRUE;
		}
	}
	else 
	{	//根据基准角钢制弯钢板
		CBendPlateByAngleDlg bendDlg;
		bendDlg.m_hBendLineAngle=pDatumLineAngle->handle;
		if(bendDlg.DoModal()==IDOK)
		{
			double ddx=pDatumPlate->ucs.axis_z*pDatumLineAngle->get_norm_x_wing();
			double ddy=pDatumPlate->ucs.axis_z*pDatumLineAngle->get_norm_y_wing();
			f3dPoint offset_vec,pick_vec,huoqu_start,huoqu_end;
			huoqu_start=pDatumLineAngle->Start();
			huoqu_end  =pDatumLineAngle->End();
			if(fabs(ddx)>fabs(ddy))
			{	//X肢为当前连接肢
				offset_vec=pDatumLineAngle->GetWingVecX();
				pick_vec=pDatumLineAngle->get_norm_y_wing();
				if(bendDlg.m_iDatumEdge==0)	//制弯线在楞线侧
					offset_vec*=-1.0;
				else	//制弯线在肢翼侧
					pick_vec*=-1.0;
			}
			else
			{	//Y肢为当前连接肢
				offset_vec=pDatumLineAngle->GetWingVecY();
				pick_vec=pDatumLineAngle->get_norm_x_wing();
				if(bendDlg.m_iDatumEdge==0)	//制弯线在楞线侧
					offset_vec*=-1.0;
				else	//制弯线在肢翼侧
					pick_vec*=-1.0;
			}
			if(bendDlg.m_iDatumEdge==1)
			{	//肢翼侧
				huoqu_start+=offset_vec*pDatumLineAngle->GetWidth();
				huoqu_end+=offset_vec*pDatumLineAngle->GetWidth();
			}
			huoqu_norm=pDatumPlate->ucs.axis_z;
			huoquline_vec=huoqu_end-huoqu_start;
			GEPOINT verify_norm=offset_vec^huoquline_vec;
			if(verify_norm*pDatumPlate->ucs.axis_z<0)
				huoquline_vec*=-1.0;
			double rad_ang=bendDlg.m_fBendAngle*RADTODEG_COEF;
			if(RotateVectorAroundVector(huoqu_norm,rad_ang,huoquline_vec))
			{
				pDatumPlate->face_N++;
				pDatumPlate->huoqufacenorm[pDatumPlate->face_N-2]=huoqu_norm;
				if(pDatumPlate->face_N==3)
					pDatumPlate->jdb_style=1;//之前折叠板赋值为3是错的会与钢管夹板混淆，现重新修正为1，并增加IsFoldPlate()函数 wjh-2016.1.09
				//计算制弯线位置
				huoqu_start+=offset_vec*bendDlg.m_fBendClearSpace;
				huoqu_end+=offset_vec*bendDlg.m_fBendClearSpace;
				coord_trans(huoqu_start,pDatumPlate->ucs,FALSE);
				coord_trans(huoqu_end,pDatumPlate->ucs,FALSE);
				huoqu_start.z=huoqu_end.z=0;
				pDatumPlate->HuoQuLine[pDatumPlate->face_N-2].startPt=huoqu_start;
				pDatumPlate->HuoQuLine[pDatumPlate->face_N-2].endPt=huoqu_end;
				//计算制弯线在板轮廓上的连接制弯点
				f2dLine edge_line,huoquline2d;
				huoquline2d.startPt=huoqu_start;
				huoquline2d.endPt=huoqu_end;
				pHuoquStartVertex=pHuoquEndVertex=NULL;
				pPrevVertex=pDatumPlate->vertex_list.GetTail();
				for(pVertex=pDatumPlate->vertex_list.GetFirst();pVertex;pPrevVertex=pVertex,pVertex=pDatumPlate->vertex_list.GetNext())
				{
					int prev_min_face=0,cur_min_face=0;
					if(pPrevVertex->vertex.feature<10)
						prev_min_face=pPrevVertex->vertex.feature;
					else
						prev_min_face=min(pPrevVertex->vertex.feature%10,pPrevVertex->vertex.feature/10);
					if(pVertex->vertex.feature<10)
						cur_min_face=pVertex->vertex.feature;
					else
						cur_min_face=min(pVertex->vertex.feature%10,pVertex->vertex.feature/10);
					edge_line.startPt=pPrevVertex->vertex;
					edge_line.endPt=pVertex->vertex;
					//pPrevVertex=pVertex;
					if(prev_min_face>1||cur_min_face>1)	//不是基面上的轮廓边
						continue;
					else
					{
						f3dPoint inters;
						if(Int2dpl(huoquline2d,edge_line,inters.x,inters.y)>0)
						{	//两直线有交点
							double justify;	//交点有效性判断
							if(fabs(edge_line.endPt.x-edge_line.startPt.x)>eps)
								justify=(inters.x-edge_line.startPt.x)/(edge_line.endPt.x-edge_line.startPt.x);
							else
								justify=(inters.y-edge_line.startPt.y)/(edge_line.endPt.y-edge_line.startPt.y);
							if(justify>=-eps&&justify<=eps)		//前一顶点为制弯点
								pPrevVertex->vertex.feature=10+pDatumPlate->face_N;
							else if(justify>eps&&justify<1-eps)	//交点为轮廓边的内点
							{
								PROFILE_VERTEX vertex;
								vertex.vertex=inters;
								PROFILE_VERTEX *pInsertVertex=pDatumPlate->vertex_list.insert(vertex);
								pInsertVertex->vertex.feature=10+pDatumPlate->face_N;
								if(pHuoquStartVertex==NULL)
									pHuoquStartVertex=pInsertVertex;
								else if(pHuoquEndVertex==NULL)
								{
									pHuoquEndVertex=pInsertVertex;
									break;	//已找到并生成制弯线的两个匹配轮廓顶点
								}
							}
							else if(justify>=1-eps&&justify<=1+eps)	//当前顶点为制弯点
							{
								pVertex->vertex.feature=10+pDatumPlate->face_N;
								if(pHuoquStartVertex==NULL)
									pHuoquStartVertex=pVertex;
								else if(pHuoquEndVertex==NULL)
								{
									pHuoquEndVertex=pVertex;
									break;	//已找到并生成制弯线的两个匹配轮廓顶点
								}
							}
						}
					}
				}
				if(pHuoquStartVertex&&pHuoquEndVertex)
					bBendPlate=TRUE;
				else
					bBendPlate=FALSE;	//板外形有问题，无法找到与制弯线相匹配的轮廓顶点
			}
			//转换至钢板局部坐标系以便后续火曲线方向的验证 wjh-2016.4.28
			vector_trans(huoquline_vec,pDatumPlate->ucs,FALSE,TRUE);
		}
		else
			return FALSE;
	}
	if(bBendPlate)
	{	//制弯钢板
		GEPOINT verify_huoquvec=pHuoquEndVertex->vertex-pHuoquStartVertex->vertex;
		if(huoquline_vec*verify_huoquvec<0)
		{
			PROFILE_VERTEX* pTempVertex=pHuoquEndVertex;
			pHuoquEndVertex=pHuoquStartVertex;
			pHuoquStartVertex=pTempVertex;
		}
		pDatumPlate->designInfo.m_bEnableFlexibleDesign=FALSE;	//关闭柔性设计
		BOOL bStart=FALSE;
		pVertex=pDatumPlate->vertex_list.GetFirst();
		while(1)
		{
			if(bStart)	//开始火曲面
			{
				if(pVertex==pHuoquEndVertex)
				{
					pVertex->vertex.feature=pDatumPlate->face_N+10;
					break;		//终止火曲面
				}
				if(pVertex->vertex.feature==1)
					pVertex->vertex.feature=pDatumPlate->face_N;
			}
			else		//查找火曲面开始点
			{
				if(pVertex==pHuoquStartVertex)
				{
					pVertex->vertex.feature=pDatumPlate->face_N+10;
					bStart=TRUE;
				}
			}
			pVertex=pDatumPlate->vertex_list.GetNext();
			if(pVertex==NULL)
				pVertex=pDatumPlate->vertex_list.GetFirst();
		}
		//TODO:下面代码为2010年海涛暂时拷贝过来准备处理圆弧边的,一直未完善 wjh-2015.2.7
		/*int n=pDatumPlate->vertex_list.GetNodeNum();
		for(int i=0;i<pDatumPlate->vertex_list.GetNodeNum();i++)
		{
			PROFILE_VERTEX *pPrevVertex=&pDatumPlate->vertex_list[(i-1+n)%n];
			PROFILE_VERTEX *pStartVertex=&pDatumPlate->vertex_list[i];
			PROFILE_VERTEX *pEndVertex=&pDatumPlate->vertex_list[(i+1)%n];
			PROFILE_VERTEX *pNextVertex=&pDatumPlate->vertex_list[(i+2)%n];
			f3dLine prev_line(pPrevVertex->vertex,pStartVertex->vertex);
			f3dLine next_line(pEndVertex->vertex,pNextVertex->vertex);
			coord_trans(prev_line.startPt,pDatumPlate->ucs,TRUE);
			coord_trans(prev_line.endPt,pDatumPlate->ucs,TRUE);
			coord_trans(next_line.startPt,pDatumPlate->ucs,TRUE);
			coord_trans(next_line.endPt,pDatumPlate->ucs,TRUE);
			f3dPoint line_vec=prev_line.endPt-prev_line.startPt;
			f3dPoint start_vertex,end_vertex,inters1,inters2,vec1,vec2;
			if(pPrevVertex->type==0&&pStartVertex->type==3)
			{	//前一个轮廓边为直线时才可自动修正椭圆弧顶点 wht 10-06-23
				int n=Int3dlc(&inters1,&inters2,prev_line.startPt,line_vec,
					pStartVertex->center,pStartVertex->center+pStartVertex->column_norm*100,
					fabs(pStartVertex->radius),fabs(pStartVertex->radius));
				if(n==1)
					start_vertex=inters1;
				else if(n==2)
				{
					vec1=inters1-prev_line.endPt;
					vec2=inters2-prev_line.endPt;
					if(vec1*line_vec>vec2*line_vec)
						start_vertex = inters2;
					else
						start_vertex = inters1;
				}
				else
					break;
				coord_trans(start_vertex,pDatumPlate->ucs,FALSE);
				pStartVertex->vertex.Set(start_vertex.x,start_vertex.y,start_vertex.z);
			}
			if(pEndVertex->type==0&&pStartVertex->type==3)
			{	//后一个轮廓边为直线时才可自动修正椭圆弧顶点
				line_vec=next_line.startPt-next_line.endPt;
				int n=Int3dlc(&inters1,&inters2,next_line.endPt,line_vec,
					pStartVertex->center,pStartVertex->center+pStartVertex->column_norm*100,
					fabs(pStartVertex->radius),fabs(pStartVertex->radius));
				if(n==1)
					end_vertex = inters1;
				else if(n==2)
				{
					vec1=inters1-next_line.startPt;
					vec2=inters2-next_line.startPt;
					if(vec1*line_vec>vec2*line_vec)
						end_vertex = inters2;
					else
						end_vertex = inters1;
				}
				else
					break;	//
				coord_trans(end_vertex,pDatumPlate->ucs,FALSE);
				pEndVertex->vertex.Set(end_vertex.x,end_vertex.y,end_vertex.z);
			}
		}*/
		//完成火曲钢板后，更新新生成的火曲面上螺栓的法线
		f3dLine huoquline;
		CLsRef *pLsRef=NULL;
		for(pLsRef=pDatumPlate->GetFirstLsRef();pLsRef;pLsRef=pDatumPlate->GetNextLsRef())
		{
			f3dPoint ls_pos=(*pLsRef)->ucs.origin;
			//将螺栓位置转换到钢板坐标系下
			coord_trans(ls_pos,pDatumPlate->ucs,FALSE);
			ls_pos.z=0;	//Z坐标归零 否则不能正确判断螺栓是否位于火曲面上
			if(DistOf2dPtLine(pHuoquStartVertex->vertex,pHuoquEndVertex->vertex,ls_pos)<0)
			{	//螺栓在火曲线右侧,说明螺栓为火曲面上的螺栓,须更新螺栓法线方向
				if((*pLsRef)->des_base_pos.datumPoint.datum_pos_style==0)
				{	//位置由用户指定的螺栓须更新螺栓孔坐标
					(*pLsRef)->des_base_pos.cLocationStyle=true;
					(*pLsRef)->des_base_pos.reside_plane=(BYTE)pDatumPlate->face_N-1;
					(*pLsRef)->des_base_pos.hPart=pDatumPlate->handle;
					(*pLsRef)->des_base_pos.len_offset_dist=ls_pos.x;
					(*pLsRef)->des_base_pos.wing_offset_dist=ls_pos.y;
				}
				(*pLsRef)->set_norm(huoqu_norm);
				(*pLsRef)->des_work_norm.norm_style=3;	//构件工作法线
				(*pLsRef)->des_work_norm.hVicePart=pDatumPlate->handle;		//基准板
				(*pLsRef)->des_work_norm.reside_plane=(BYTE)pDatumPlate->face_N-1;//构件工作法线
				(*pLsRef)->des_work_norm.vector=huoqu_norm;
				(*pLsRef)->correct_worknorm();
				(*pLsRef)->correct_pos();
				(*pLsRef)->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
			}
		}
		pDatumPlate->SetModified();
		pDatumPlate->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pDatumPlate->GetSolidPartObject());
		g_pSolidDraw->Draw();
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Enough matched bending vertex are found, plate bending failure!");
#else
		AfxMessageBox("没有找到匹配到足够的制弯顶点,钢板制弯失败!");
#endif
	//设置不显示工作坐标系
	//g_pSolidSet->SetDispWorkUcs(FALSE);
	g_pSolidDraw->BatchClearCS(2);
	Invalidate(FALSE);
	Ta.EndUndoListen();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	return 1;
}

void CLDSView::OnBasePlate()
{
	Command("BasePlate");
}

void CLDSView::OnAngleShoePlate()
{
	Command("ShoePlate");
}
#else
void CLDSView::OnDesignLjPlate(){;}
void CLDSView::OnDesignElbowPlate(){;} 
void CLDSView::OnDesignRibPlate(){;}
void CLDSView::OnDesignOverlappedPlate(){;}
void CLDSView::OnCircularPlate(){;}
void CLDSView::OnInsertPlateStyleTubeInters(){;}
void CLDSView::OnGyHole(){;}
void CLDSView::OnTubeBetweenPlate(){;}
void CLDSView::OnExtendPlate(){;}
void CLDSView::OnMergeLjb(){;}
void CLDSView::OnDesignSphere(){;}
void CLDSView::OnNewArcPart(){;}
void CLDSView::OnRotatePartAroundAxis(){;}
void CLDSView::OnCutPlateByPlane(){;}
void CLDSView::OnCutPlateByColumnFace(){;}
void CLDSView::OnBoltPad(){;}
void CLDSView::OnClassicConnectDesign(){;}
void CLDSView::OnDesignSpecialFl(){;}
void CLDSView::OnMakePlateBend(){;}
void CLDSView::OnFastCirCoverPlate(){;}
void CLDSView::OnFastCirRibPlate(){;}
void CLDSView::OnFastTubeRibPlate(){;}
void CLDSView::OnBasePlate(){;}
void CLDSView::OnAngleShoePlate(){;}
#endif
