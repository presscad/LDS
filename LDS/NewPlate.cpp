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
	m_sPrevCommandName.Format("%s","�ظ����Ӱ�");
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
	//1.������˨������ȡ���Ӱ������Ӹ˼��ļ���
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
	//2.����ʶ�����ǡ���ĸ˼���Ϊ�ְ�Ķ�λ��׼�˼�
	CONN_NODEINFO* pConnNode,*pMaxHitsConnNode=NULL;
	bool stronglyAdviseBaseRod=false;	//���ӵ�λ�ڻ�׼�˼��м��ʱ��ǿ�ҽ����Դ˸˼�Ϊ�ְ�Ķ�λ��׼�˼�
	for(pConnNode=hashNodeConnHits.GetNext();pConnNode;pConnNode=hashNodeConnHits.GetNext())
	{
		for(CPtInLineCheck* pLineCheck=hashLineChecks.GetFirst();pLineCheck;pLineCheck=hashLineChecks.GetNext())
		{
			double justify=pLineCheck->CheckPoint(pConnNode->pNode->Position(false));
			if(justify>EPS&&justify<1-EPS)
			{	//��ǰ�ڵ�λ��ĳһ���Ӹ˼����м�ڵ�
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
		pRod->feature=1;//Ĭ�Ͼ�Ϊ������׼�����Ӹ˼�   
		CLDSLineAngle* pAngle=pRod->IsAngle()?(CLDSLineAngle*)pRod:NULL;
		//CLDSLineTube* pTube=pRod->GetClassTypeId()==CLS_LINETUBE?(CLDSLineTube*)pRod:NULL;
		if(pAngle!=NULL&&pAngle->pStart!=NULL&&pAngle->pEnd!=NULL&&
			(toupper(pAngle->Layer(1))=='Z'||toupper(pAngle->Layer(1))=='H'||//TODO:ʲô���������'H',Ӧ�������һ�������������жϣ��Ժ�Ӧ�������ٶ�ͼ����������
			pAngle->desStartPos.jgber_cal_style==1||pAngle->desEndPos.jgber_cal_style==1||	//����ָ���Ǹ��������
			pAngle->desStartPos.spatialOperationStyle==0||pAngle->desStartPos.spatialOperationStyle>14))
		{
			if(pBaseRod==NULL||pRod->GetWidth()>pBaseRod->GetWidth())
				pBaseRod=pAngle;	//ȡ֫��ϴ�Ǹ���Ϊ��׼�Ǹ�
		}
	}
	for(pConnNode=hashNodeConnHits.GetFirst();pConnNode;pConnNode=hashNodeConnHits.GetNext())
	{
		if(pMaxHitsConnNode==NULL || pConnNode->hits>pMaxHitsConnNode->hits)
			pMaxHitsConnNode=pConnNode;
	}
	if(pMaxHitsConnNode==NULL&&rodset.GetNodeNum()<=0)	
		return FALSE;	//���ڶ�������˼���δ�ҵ��������ӽڵ�ʱ�޷���Ƹְ� wht 16-11-09
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
	//3.��λ����ƽ��
	if(pBaseRod)
	{
		CLDSLineAngle* pBaseAngle=pBaseRod->IsAngle()?(CLDSLineAngle*)pBaseRod:NULL;
		CLDSLineAngle *pBendPlaneBaseAngle=NULL;
		CLDSBolt *pBaseBolt1=NULL,*pBaseBolt2=NULL;
		if(pBaseAngle==NULL)
			workplane.bTwoWorkPlane=false;
		else
		{
			//3.1ʶ���׼�淨��
			for(pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
			{
				if(pBolt->des_base_pos.hPart==pBaseAngle->handle||pBaseAngle->FindLsByHandle(pBolt->handle))
				{
					workplane.designnorm.norm_style=1;
					workplane.designnorm.hVicePart=pBaseAngle->handle;
					if(fabs(pBaseAngle->vxWingNorm*pBolt->ucs.axis_z)>fabs(pBaseAngle->vyWingNorm*pBolt->ucs.axis_z))
					{
						workplane.designnorm.norm_wing=0;	//X֫Ϊ��׼
						workplane.vBasePlaneNorm=pBaseAngle->vxWingNorm;
					}
					else
					{
						workplane.designnorm.norm_wing=1;	//Y֫Ϊ��׼
						workplane.vBasePlaneNorm=pBaseAngle->vyWingNorm;
					}
					pBolt->des_base_pos.datumPoint.UpdatePos(pBaseAngle->BelongModel());
					workplane.vBasePlanePick=pBolt->des_base_pos.datumPoint.Position();
					pBaseBolt1=pBolt;
					break;
				}
			}
			//3.2ʶ�������淨��
			if(workplane.vBasePlaneNorm.IsZero())
				return FALSE;
			double fCurrBendPlaneBaseAngleLength=0;
			for(pRod=rodset.GetFirst();pRod;pRod=rodset.GetNext())
			{
				if(pRod==pBaseAngle)
					continue;	//��׼�Ǹ�
				double bendTolerance=max(50,pRod->size_wide/2);	//���ݹ���ʵ�������ݲ��������ֵ���ж�ʧ�󣬹ʸ�Ϊ��Ǹ�֫����� wjh-2016.3.16
				GEPOINT lenvec=pRod->End()-pRod->Start();
				double len=lenvec.mod();
				normalize(lenvec);
				double cosa=fabs(lenvec*workplane.vBasePlaneNorm);
				if(cosa*len<bendTolerance)
				{
					pRod->feature=1;
					if(cosa<EPS2)
					{	//�˼��������׼�淨�ߴ�ֱ,�����κ�һ֫���߷������׼�淨�߷������һ��
						if(pRod->IsAngle())
						{	//������˨ȷ���Ǹֵ�ǰ����֫
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
							//�ҵ�������˨�ҹ���֫�������׼�淨�߲�ƽ��
							//EPS_COS2���ֵ̫��,���������Сʱ�޷�׼ȷ�жϸ˼������� wht 16-11-10
							const double cosa=0.99996192306417128873735516482698;	//0.5��
							if( pLsRef&&fabs(vWingNorm*workplane.vBasePlaneNorm)<cosa)
								pRod->feature=2;
						}
						//else if(pRod->IsTube()){} ��ʱδ�������߸˼�Ϊ�ֹܵ����
					}
					if(pRod->feature==1)
						continue;	//��׼�������Ӹ˼�
				}
				pRod->feature=2;//��ǰ�˼�Ϊ�������ڵ����Ӹ˼�
				double fCurrAngleLength=pRod->IsAngle()?pRod->GetLength():0;
				if(!pRod->IsAngle()||fCurrAngleLength<=fCurrBendPlaneBaseAngleLength)//workplane.vBendPlaneNorm.IsZero())
					continue;	//���Ǹ�ǡ����������Ǹ�
				pBendPlaneBaseAngle=(CLDSLineAngle*)pRod;	//��ǰ�Ǹָ��ʺ���Ϊ������Ļ�׼�Ǹ�
				pBaseBolt2=NULL;//����Ǹֱ��֮��Ӧ���²��һ�׼��˨
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
						workplane.designbendnorm.norm_wing=0;	//X֫Ϊ��׼
					}
					else
					{
						workplane.vBendPlaneNorm=pBendPlaneBaseAngle->vyWingNorm;
						workplane.designbendnorm.norm_wing=1;	//Y֫Ϊ��׼
					}
					workplane.huoqustart.datum_pos_style=1;
					workplane.huoqustart.des_para.RODEND.hRod=pBendPlaneBaseAngle->handle;
					workplane.huoqustart.des_para.RODEND.wing_offset_style=4;
					workplane.huoqustart.des_para.RODEND.wing_offset_dist=0;
					workplane.huoqustart.des_para.RODEND.offset_wing=workplane.designbendnorm.norm_wing;	//ƫ��֫
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
						continue;	//���ڽǸ���Ƥ
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
						continue;	//���ڽǸ���Ƥ
					workplane.xBendNormOffset.AddThick(pThroughPlate->handle,TRUE,TRUE);
				}
			}
		}
		workplane.bTwoWorkPlane=!workplane.vBendPlaneNorm.IsZero();
		if(workplane.bTwoWorkPlane&&pBaseBolt1&&pBaseBolt2)
		{	//������������׼�淨�߲���������������
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
			if(verify_axisvec1*verify_axisvec2>0)	//ӦС���㣬>0��ʾ���������׼�淨�߲������������
			{
				if( workplane.designbendnorm.norm_style==1&&extendvec2*workplane.vBasePlaneNorm>0)
				{	//��ΰ˵���ݶ�����L��ʱ�����ǻ�������,���Ծ�����L������
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
			BOOL bBendAlongBaseAngle=stronglyAdviseBaseRod;	//�Ƿ��ӻ�׼�Ǹ�����
			CLDSLineAngle *pBendBaseAngle=pBaseAngle;
			if(!bBendAlongBaseAngle)
			{	//�����и˼��в����������߷���һ�µĽǸ� wht 16-11-10
			for(CLDSLinePart *pRod=rodset.GetFirst();pRod;pRod=rodset.GetNext())
			{
				if(!pRod->IsAngle())
					continue;
				GEPOINT base_angle_vec=pRod->End()-pRod->Start();
				normalize(base_angle_vec);
				double dd=base_angle_vec*huoqu_vec;
				if(fabs(base_angle_vec*huoqu_vec)>EPS_COS2)
				{	//��׼�Ǹַ����������߷���һ��
					CLDSLineAngle *pAngle=(CLDSLineAngle*)pRod;
					f3dLine base_angle_line(pAngle->Start(),pAngle->End());
					GEPOINT wing_vec=(workplane.designnorm.norm_wing==0)?pAngle->GetWingVecX():pAngle->GetWingVecY();
					if(wing_vec*extendvec2>0)	
					{	//��������֫���
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
				if(wing_vec*otherface_extendvec<0)	//�����������߲�
					workplane.huoqustart.des_para.RODEND.wing_offset_dist=workplane.huoquend.des_para.RODEND.wing_offset_dist=-g_sysPara.HuoQuDist;
				else
					workplane.huoqustart.des_para.RODEND.wing_offset_dist=workplane.huoquend.des_para.RODEND.wing_offset_dist=g_sysPara.HuoQuDist+pBendBaseAngle->size_wide;
				workplane.huoquend.des_para.RODEND.len_offset_dist=200;
				//���ڵ�������߲�һ���ڽǸ���Ƥ�ϣ�������û����Զ����������ȷ������������û��ڸְ��������н��е��� wjh-2016.12.20
				workplane.m_bSelfCorrectBendline=true;//false;
			}
			else 
				workplane.m_bSelfCorrectBendline=true;
		}
	}
	//ѡ��һ���Ǹ��ϵ���˨ҲӦ֧�����ɸְ�
	return pBaseRod!=NULL && boltset.GetNodeNum()>=1 && rodset.GetNodeNum()>=1;
}
#include "KeypointLifeObj.h"
int CLDSView::ConnectionPlate()
{	//���Ӱ�
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
		//SELECT_ALL-0x03���޳������߼��ڵ�(����Ӱ����˨ѡȡ) wjh-2016.5.12
		GET_SCR_PART_PARA scr_part_para(SELECT_ALL-0x03,CLS_LINEPART,CLS_BOLT);
		scr_part_para.disp_type=DISP_SOLID;
#ifdef AFX_TARG_ENU_ENGLISH
		scr_part_para.cmdStr="please choose assembling location rod for connecting plates:";
		scr_part_para.cmdErrorFeedbackStr="No suitable parts selected,please choose assembling location rod for connecting plates again:";
#else
		scr_part_para.cmdStr="��ѡ�����Ӱ��װ�䶨λ�˼�, ��ֱ��ѡ��ȫ��������˨:";
		scr_part_para.cmdErrorFeedbackStr="û��ѡ�к��ʵĹ�����������ѡ�����Ӱ��װ�䶨λ�˼���ȫ��������˨:";
#endif
		scr_part_para.nResultObjs=2;
		while(1)
		{
			//������������Ӧ������̧�����Ϣ��ͬ���س���Ч����ͬ
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
			else	//�ø�����˨������ȡ��װ�䶨λ�˼�����pBaseRod;
				pBaseLinePart=pBaseRod;
			break;
		}
		}

		if(boltset.GetNodeNum()==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select assembling location node of rod:","");
#else
		pCmdLine->FillCmdLine("��ѡ��˼��ϵ�װ�䶨λ�ڵ�:","");
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
		pCmdLine->FinishCmdLine(CXhChar16("�ڵ�0x%X",pBaseNode->handle));

		GET_SCR_PART_PARA scr_rod_para(GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_LINETUBE),CLS_LINEPART,0,2);
		scr_rod_para.disp_type=DISP_SOLID;
#ifdef AFX_TARG_ENU_ENGLISH
		scr_rod_para.cmdStr="Please choose all the rods connected with fundamental planes:";
		scr_rod_para.cmdErrorFeedbackStr="No suitable parts selected,Please choose all the rods connected with fundamental planes again:";
#else
		scr_rod_para.cmdStr="��ѡ������������ӵ����и˼�:";
		scr_rod_para.cmdErrorFeedbackStr="û��ѡ�к��ʵĹ�������ѡ������������ӵ����и˼�:";
#endif
		OpenWndSel();
		rodset.SetValue(pBaseLinePart->handle,pBaseLinePart);
		while(1)
		{
			if(GetPartsFromScr(scr_rod_para,FALSE))
			{
				if(scr_rod_para.nResultObjs==0)
				{
					scr_rod_para.nResultObjs=2;	//����2�����ѡ
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
		OnOperOther();	//ȡ����������״̬
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
		//����HashTable
		CHashTable<CLDSPart*>partsetTable;
		partsetTable.CreateHashTable(100);
		CDesignLjPartPara desLjPartPara;
		if(pBaseLinePart.IsHasPtr())
		{	//��������Ļ�׼�˼�ȷ��װ������ϵ
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
			{	//��ǰ��ͼ������ְ巨�߼н�С��30��ʱ��֤�ְ巨������ͼ���߷���һ�� wht 16-10-31
				plateTemp.ucs.axis_z*=-1.0;
				workplane.designnorm.ReverseDirection();
				workplane.designbendnorm.ReverseDirection();
			}
			if(pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
			{
				plateTemp.designInfo.norm.norm_style=2;	//�˼������淨��
				plateTemp.designInfo.norm.hVicePart=pBaseLinePart->handle;
				if(pSetupPlaneLinePart.IsHasPtr())
					plateTemp.designInfo.norm.hCrossPart=pSetupPlaneLinePart->handle;
				plateTemp.designInfo.norm.nearVector = plateTemp.ucs.axis_z;
			}
			else //if(pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE||pBaseLinePart->GetClassTypeId()==CLS_GROUP_LINEANGLE)
			{
				plateTemp.designInfo.norm.direction = 0;	//����
				plateTemp.designInfo.norm.norm_style = 1;	//�Ǹ�֫���߷���
				plateTemp.designInfo.norm.hVicePart = pBaseLinePart->handle;
				double ddx=pBaseLinePart.LineAnglePointer()->vxWingNorm*plateTemp.ucs.axis_z;
				double ddy=pBaseLinePart.LineAnglePointer()->vyWingNorm*plateTemp.ucs.axis_z;
				if(fabs(ddx)>fabs(ddy)&&fabs(ddx)>0.95)
				{
					plateTemp.designInfo.norm.norm_wing = 0;	//X֫����
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
					plateTemp.designInfo.norm.norm_wing = 1;	//Y֫����
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
						pCmdLine->FillCmdLine("���巨�߷����жϲ�׼����ȷ�ϻ�׼�Ǹֵ�ǰ������֫[X֫(X)/Y֫(Y)]<X>","");
					else
						pCmdLine->FillCmdLine("���巨�߷����жϲ�׼����ȷ�ϻ�׼�Ǹֵ�ǰ������֫[X֫(X)/Y֫(Y)]<Y>","");
#endif
					while(1)
					{
						if(!pCmdLine->GetStrFromCmdLine(cmdStr))
							return 0;
						if((fabs(ddx)>fabs(ddy)&&cmdStr.GetLength()==0)||cmdStr.CompareNoCase("X")==0)
							plateTemp.designInfo.norm.norm_wing=0;	//X֫����
						else if(cmdStr.GetLength()==0||cmdStr.CompareNoCase("Y")==0)
							plateTemp.designInfo.norm.norm_wing=1;	//Y֫����
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
							pCmdLine->FillCmdLine("��Ҫѡ��ؼ���","");
							pCmdLine->FinishCmdLine();
							if(fabs(ddx)>fabs(ddy))
								pCmdLine->FillCmdLine("���巨�߷����жϲ�׼����ȷ�ϻ�׼�Ǹֵ�ǰ������֫[X֫(X)/Y֫(Y)]<X>","");
							else
								pCmdLine->FillCmdLine("���巨�߷����жϲ�׼����ȷ�ϻ�׼�Ǹֵ�ǰ������֫[X֫(X)/Y֫(Y)]<Y>","");
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
				else	//ͨ��������˨���ɵĸְ�
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
					//�˾�Ĭ��ֵ���ܲ�������˨��С�߾࣬�˴���ֵΪ0����Ƹְ�ʱ������˨ֱ���Զ�������˨�˾� wht 16-01-05
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
							//�˾�Ĭ��ֵ���ܲ�������˨��С�߾࣬�˴���ֵΪ0����Ƹְ�ʱ������˨ֱ���Զ�������˨�˾� wht 16-01-05
							desLjPartPara.end_space=0;	
						}
					}
				}
				plateTemp.designInfo.partList.SetValue(desLjPartPara.hPart,desLjPartPara);
			}
		}
		g_pSolidDraw->ReleaseSnapStatus();
		OnOperOther();	//ȡ����������״̬
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
		//}
		Ta.BeginUndoListen();
		int nThick=5;	//����˵���߽Ǹ�֫��С��4ʱ�����ȡ5mm wjh-2013.5.7
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
			nThick+=1;	//����ʹ��ż�����
		CLDSPlate *pPlate= (CLDSPlate*)console.AppendPart(CLS_PLATE);
		pPlate->Thick=nThick;
		if(boltset.GetNodeNum()>0)
		{
			for(CLDSBolt *pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
			{
				if(!workplane.bTwoWorkPlane&&fabs(pBolt->get_norm()*plateTemp.ucs.axis_z)<0.7071) //cos45=0.70710678118655
					continue;	//���ݸְ巨�߹��˵�����ֱ�ְ����˨ wht 16-01-05
				//������˨���ߺ�����˨�������ߺż��ӳ��
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
		pCmdLine->FillCmdLine("��ѡ�����Ӱ������������˨:","");
#endif
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
		Invalidate(FALSE);
		OpenWndSel();	//����ѡ����˨
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
						continue;	//���ݸְ巨�߹��˵�����ֱ�ְ����˨ wht 16-01-05
					//������˨���ߺ�����˨�������ߺż��ӳ��
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
			//�����жϣ������������
			if(pPlate)
			{
				console.DispPartSet.DeleteNode(pPlate->handle);
				g_pSolidDraw->DelEnt(pPlate->handle);
				console.DeletePart(pPlate->handle);
			}
			return 0;
		}
		OnOperOther();	//ȡ����������״̬
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
		else //�����жϣ������������
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
			pPlate->CopyModuleInstanceInfo(pBaseNode);//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
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
				if(workplane.designbendnorm.direction==0)	//����
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
		//�Զ��������߽Ǹֵİڷ�λ�ú���˨λ��
		for(CDesignLjPartPara *pLjPart=plateTemp.designInfo.partList.GetFirst();pLjPart;pLjPart=plateTemp.designInfo.partList.GetNext())
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(pLjPart->hPart,CLS_LINEPART);
			if(pLinePart==NULL||(pLinePart&&!pLinePart->IsLinePart()))
				continue;
			if(pLinePart->GetClassTypeId()==CLS_LINETUBE&&(pLjPart->start0_end1==0||pLjPart->start0_end1==1))
				pLjPart->ber_space=fEdgeSpace;	//���߸˼�Ϊ�ֹ�ʱ���ò��߾�
			pPlate->designInfo.partList.SetValue(pLjPart->hPart,*pLjPart);
			if(pLinePart->handle==pBaseLinePart->handle)
				continue;	//������׼��λ�˼�
			if(pLjPart->start0_end1!=0&&pLjPart->start0_end1!=1)
				continue;	//������ʼ�ն����ӵ����߸˼�
			if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//������߰�����˨�ķ���ƫ����
				CLDSLineTube *pLineTube=(CLDSLineTube*)pLinePart;
				CSuperSmartPtr<CLDSPart> pPart;
				if(pLjPart->start0_end1==0)	//ʼ������
					pPart=console.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
				else if(pLjPart->start0_end1==1)//�ն�����
					pPart=console.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
				if(pPart.IsNULL())
					continue;
				if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&pPart.ParamPlatePointer()->m_iParamType==TYPE_ROLLEND)
				{
					for(CLsRef *pLsRef=pPart.ParamPlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPart.ParamPlatePointer()->GetNextLsRef())
					{
						if((*pLsRef)->des_work_norm.direction==1)	//����(��巨�߷����෴)
							(*pLsRef)->des_base_pos.norm_offset.AddThick(-pPlate->handle,TRUE);
					}
					//�����ֹܰڷ�λ��
					if(g_sysPara.nAutoAdjustTubePos==1||g_sysPara.nAutoAdjustTubePos==2)
					{
						double fWorkPlaneRotAngle=0;	//����ƽ����ת�Ƕ�
						double offset_dist=-0.5*(pPart.ParamPlatePointer()->GetThick()+pPlate->GetThick());
						double X=0;	//D���׼������ز���ͬ��,��X��ʾ��ת�Ƕ�  ��ʱ��˴��� wht 10-01-21
						if(pPart.ParamPlatePointer()->m_bStdPart)
							pPart.ParamPlatePointer()->GetDesignItemValue('X',&fWorkPlaneRotAngle);
						else
							pPart.ParamPlatePointer()->GetDesignItemValue('D',&fWorkPlaneRotAngle);
						if(fabs(fabs(fWorkPlaneRotAngle)-180)<EPS)	//����߷����෴���巨�߷����෴
							offset_dist*=-1.0;	
						if(g_sysPara.nAutoAdjustTubePos==2)	//�ֹ�ƫ��
						{	//���µ����ֹܷ���ƫ���� wht 10-07-27
							if(pLjPart->start0_end1==0)
								pLineTube->desStartPos.fNormOffset=offset_dist;
							else if(pLjPart->start0_end1==1)
								pLineTube->desEndPos.fNormOffset=offset_dist;
						}
						else if(g_sysPara.nAutoAdjustTubePos==1) //���ƫ��
							pPart.ParamPlatePointer()->m_fNormOffset-=offset_dist;
						//��λ�ֹܲ�������Ʋ��
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
				//��ʱ�������������
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
				{	//���ð��ϵĸ���׼�Ǹֿ�ͨ��<��������>������𣬲���������趨λ�á�wjh-2015.9.10
					if(fabs(ddx)>fabs(ddy))
					{	
						pPosDesign->wing_x_offset.gStyle=4;
						//��׼�˼�Ϊ�ֹ�ʱ��/�����İڷ�λ�ö���Ҫ����
						if(pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
							pPosDesign->wing_x_offset.offsetDist=-0.5*pPlate->GetThick();
						//��׼�˼�Ϊ�Ǹ�ʱ����������İڷ�λ��
						else if(pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE&&ddx<0)
							pPosDesign->wing_x_offset.offsetDist=-pPlate->GetThick();
					}
					else
					{
						pPosDesign->wing_y_offset.gStyle=4;
						//��׼�˼�Ϊ�ֹ�ʱ��/�����İڷ�λ�ö���Ҫ����
						if(pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
							pPosDesign->wing_y_offset.offsetDist=-0.5*pPlate->GetThick();
						//��׼�˼�Ϊ�Ǹ�ʱ����������İڷ�λ��
						else if(pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE&&ddy<0)
							pPosDesign->wing_y_offset.offsetDist=-pPlate->GetThick();
					}
					pLineAngle->ClearFlag();
					pLineAngle->CalPosition();
					//����Ǹ�λ�ú����¼����н���֫��Ϣ
					if(pBaseLinePart.IsHasPtr()&&pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE)
						pLineAngle->CalCutAngleInfo(pBaseLinePart.LineAnglePointer(),NULL,(pLjPart->start0_end1==0),false);
				}
				pLineAngle->SetModified();
				pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
				if((fabs(ddx)>fabs(ddy)&&ddx<0)||(fabs(ddx)<fabs(ddy)&&ddy<0))
				{	//���������ϵ���˨�ķ���ƫ����
					for(CLsRef *pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
						(*pLsRef)->des_base_pos.norm_offset.AddThick(-pPlate->handle,TRUE);
				}
			}
		}
		if(pBaseLinePart.IsHasPtr()&&pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
		{	//��׼����Ϊ�ֹ�ʱӦ����Ƹְ�ǰ�趨���Ӹ���������������ȷ����Ի��
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
			//�ҳ����ڸְ��ڵ���˨���������ĸ˼�����Ϊ��������
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
		else	//�հ�ʱ����Ҫ��һ��Ĭ������
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
		
		//װ�䶨λ�˼�Ϊ�ֹ�ʱ,�����붨λ�˼�����Ҫ����,ͬһ�ڵ��ϵĺ������Ӱ���뱣��һ�µĺ�����
		if(bAdjustWeldLen&&pBaseLinePart.IsHasPtr()&&pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
			CDesignJdb::AdjustPlateWeldLenOnSameNode(pBaseLinePart,pBaseNode);
		else
		{
			pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		}
		CLDSModule* pActiveModule=Ta.GetActiveModule();
		for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{	//������Ʋ�������˨
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
	pCmdLine->FillCmdLine("����:","");
#endif
	return 0;
}


void CLDSView::OnDesignElbowPlate() 
{
	m_nPrevCommandID=ID_DESIGN_ELBOW_PLATE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat tube's elbow plate";
#else
	m_sPrevCommandName="�ظ��ֹ����";
#endif
	Command("ElbowPlate");
}

int CLDSView::DesignElbowPlate()
{	//���	
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
	pCmdLine->FillCmdLine("ElbowPlate ��ѡ�����Ķ�λ��׼�ֹ�:","");
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
				pCmdLine->FillCmdLine("û��ѡ�к��ʵĹ�����������ѡ�����Ķ�λ��׼�ֹ�:","");
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
	pCmdLine->FillCmdLine("��ѡ�����Ķ�λ��׼�ְ�:","");
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
				pCmdLine->FillCmdLine("û��ѡ�к��ʵĹ�������ѡ�����Ķ�λ��׼�ְ�:","");
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
		pCmdLine->FillCmdLine("����:","");
#endif
	return 0;
}

void CLDSView::OnDesignRibPlate()	
{	//����߰�
	m_nPrevCommandID=ID_DESIGN_RIB_PLATE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat rib plate";
#else
	m_sPrevCommandName="�ظ��߰�";
#endif
	//Command("RibPlate");
	RibPlate();
}

//��ƽǸ��߰�
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

//��Ƹֹܼа�
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
		AfxMessageBox("����׼�ֹ��޽��㣬�޷����ɸֹܼа壡");
#endif
		return FALSE;
	}
	//ѡ��ֹܼа�ĵ�һ����λ�ڵ�
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
		cmdStr.Format("TubeBetweenPlate ��ѡ���һ���ֹܼа嶨λ�˽ڵ�:");
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
				errCmdStr.Format("TubeBetweenPlate ѡ�нڵ㲻�ǵ�һ���ֹܵĶ˽ڵ�,������ѡ���һ���ֹܼа嶨λ�˽ڵ�:");
#endif
				pCmdLine->FillCmdLine(errCmdStr,"");
			}
			else
				break;
		}
	}
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pDatumNode1->handle));
	g_pSolidDraw->SetEntSnapStatus(pDatumNode1->handle);
	//ѡ��ֹܼа�ĵڶ�����λ�ڵ�
	if(pDatumNode2==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("TubeBetweenPlate Please select locating end node of the the first tube between-plate:");
#else
		cmdStr.Format("TubeBetweenPlate ��ѡ��ڶ����ֹܼа嶨λ�˽ڵ�:");
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
				errCmdStr.Format("TubeBetweenPlate ѡ�нڵ㲻�ǵڶ����ֹܵĶ˽ڵ�,������ѡ��ڶ����ֹܼа嶨λ�˽ڵ�:");
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
	pBetweenPlate->CopyModuleInstanceInfo(pDatumNode1);	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
	pBetweenPlate->designInfo.m_hBaseNode=pDatumNode1->handle;	//��Ļ�׼�ڵ�
	pBetweenPlate->jdb_style=3;	//�ֹܼа�
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
	//����׼�ֹ�ʵ���Ƿ���ڽ���
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
	{	//����׼�ֹ�ʵ���޽���
		long datum_handle=0;
		sscanf(plate_dlg.m_sTransectPartHandle,"%X",&datum_handle);
		CLDSPart *pDatumPart=console.FromPartHandle(datum_handle);
		if(pDatumPart&&pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			pBaseTube=(CLDSLineTube*)pDatumPart;
		else if(pDatumPart&&pDatumPart->GetClassTypeId()==CLS_SPHERE)
			pBaseSphere=(CLDSSphere*)pDatumPart;
	}
	else	
	{	//����׼�ֹ�ʵ���н���
		pBaseTube=NULL;
		pBaseSphere=NULL;
	}
	f3dPoint inner_arc_start,inner_arc_end;
	if(pBaseTube)
	{	//�ֹ�ʵ���޽���ʱ������׼�ֹ������ֹܵ��ڲཻ��
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
	{	//�ֹ�ʵ���޽���ʱ������׼�ֹ����׼������ڲཻ��
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
		sina=1;		//�Է����ֳ�0
	pBetweenPlate->ucs.origin=datumIntersPos+
	pBetweenPlate->ucs.axis_x*(pDatumTube2->GetDiameter()*0.5/sina)+gradient_line_vec*(pDatumTube1->GetDiameter()*0.5/sina);
	f3dPoint vertice;
	if(pBaseTube||pBaseSphere)
	{  //�ֹ�ʵ���޽���ʱ��Ӹֹܼа�ĵ�һ������
		coord_trans(inner_arc_start,pBetweenPlate->ucs,FALSE);
		coord_trans(inner_arc_end,pBetweenPlate->ucs,FALSE);
		inner_arc_start.z=inner_arc_end.z=0;	
		vertice=inner_arc_end;
	}
	vertice.feature=1;
	PROFILE_VERTEX *pVertex=pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertice));
	pVertex->m_bWeldEdge=TRUE;	//���ں�������ʼ��
	BOOL bSpecifyFirstEdgeLength=FALSE,bSpecifySecondEdgeLength=FALSE;
	double fFirstEdgeLen=0,fSecondEdgeLen=0;
	double fFirstFlWidth=0,fSecondFlWidth=0;
	//�Ƿ��Զ�������ֹ������ߵĳ��ȺͿ��
	bSpecifyFirstEdgeLength=!plate_dlg.m_bAutoCalcFirst;
	bSpecifySecondEdgeLength=!plate_dlg.m_bAutoCalcSecond;
	if(bSpecifyFirstEdgeLength)
	{	//ָ�����һ����׼�ֹ������ߵĳ���
		fFirstEdgeLen=plate_dlg.m_fFirstEdgeLen;
		fFirstFlWidth=plate_dlg.m_fFirstEdgeWidth;
	}
	if(bSpecifySecondEdgeLength)
	{	//ָ����ڶ�����׼�ֹ������ߵĳ���
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
		if(pBaseTube||pBaseSphere)	//���ֹ�ʵ���޽���
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
		if(pBaseTube||pBaseSphere)	//���ֹ�ʵ���޽���
			fSecondEdgeLen-=inner_arc_start.mod();
	}
	f3dPoint vertex_arr[4];
	double between_angle=cal_angle_of_2vec(pBetweenPlate->ucs.axis_x,gradient_line_vec);
	vector_trans(gradient_line_vec,pBetweenPlate->ucs,FALSE);
	vector_trans(gradient_line_norm2,pBetweenPlate->ucs,FALSE);
	if(pBaseTube||pBaseSphere)//�ֹ�ʵ���޽���ʱ�ֹܼа�ĵڶ�������
		vertex_arr[0]=inner_arc_end.x+fFirstEdgeLen;
	else
		vertex_arr[0].Set(fFirstEdgeLen);
	if(fFirstFlWidth>0)
		vertex_arr[1].Set(vertex_arr[0].x,fFirstFlWidth);
	if(pBaseTube||pBaseSphere)//�ֹ�ʵ���޽���ʱ�ֹܼа�ĵ����ڶ�������
		vertex_arr[2]=inner_arc_start+gradient_line_vec*fSecondEdgeLen;
	else
		vertex_arr[2].Set(gradient_line_vec.x*fSecondEdgeLen,gradient_line_vec.y*fSecondEdgeLen);
	if(fSecondFlWidth>0)
		vertex_arr[3]=vertex_arr[2]+gradient_line_norm2*fSecondFlWidth;	
	f3dPoint inters_line_vec;
	if(between_angle<Pi/2)
	{
		//�жϵ�һ���ֹܻ�׼���Ƿ����
		if(fSecondFlWidth>0)
		{
			inters_line_vec=vertex_arr[3]-vertex_arr[2];
			normalize(inters_line_vec);
			if(vertex_arr[3].x-vertex_arr[3].y*inters_line_vec.x/inters_line_vec.y<fFirstEdgeLen)
			{	//��һ���ֹܻ�׼�߹���
				fFirstFlWidth=0;
				SnapPerp(&vertex_arr[0],f3dPoint(0,0,0),f3dPoint(100,0,0),vertex_arr[3]);
			}
		}
		//�жϵڶ����ֹܻ�׼���Ƿ����
		if(fFirstFlWidth>0)
		{
			if(fFirstEdgeLen/gradient_line_vec.x<fSecondEdgeLen)
			{	//�ڶ����ֹܻ�׼�߹���
				fSecondFlWidth=0;
				SnapPerp(&vertex_arr[2],f3dPoint(0,0,0),gradient_line_vec*100,vertex_arr[1]);
			}
		}
	}
	//������С��а��һ��а�
	BOOL bOnlyOnePlate=TRUE;
	if(pBaseSphere)
		bOnlyOnePlate=!plate_dlg.m_iOnlyOnePlate;
	//�ֹܼа��������߸˼�
	BOOL bLinkLinePart=FALSE;
	if(bOnlyOnePlate)	//��������һ����ֹܼа�ʱ�Ŵ����������߸˼������
		bLinkLinePart=plate_dlg.m_bLinkLinePart;
	int n=0;	//��ʼ��Ϊ0 ����û�����߸˼�������¿��ܳ���
	f3dPoint vertex_array[100];
	if(bLinkLinePart)
	{
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select all the ray rods connected with tube between-plate:","");
#else
		pCmdLine->FillCmdLine("��ѡ��ֹܼа������ӵ��������߸˼�:","");
#endif
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_LINETUBE));
		Invalidate(FALSE);
		//OpenWndSel();	//����ѡ��˼�
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
				{	//���߸˼�Ϊ�ֹ�
					pLineTube = (CLDSLineTube*)pPart;
					if(!pLineTube)
						continue;
					f3dPoint tube_vec = pLineTube->End()-pLineTube->Start();
					if(tube_vec*pBetweenPlate->ucs.axis_x>0)
					{	//�ֹ�ʼ���������
						desLjPartPara.start0_end1=0;
						if(pLineTube->m_tJointStart.hLinkObj>0x20)
							pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
					}
					else
					{	//�ֹ��ն��������
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
							//������߰�����˨�ķ���ƫ����
							for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
							{
								if((*pLsRef)->des_work_norm.direction==0)	//����
									(*pLsRef)->des_base_pos.norm_offset.AddThick(-(int)pBetweenPlate->GetThick());
							}
							pLineTube->DesignEndJointUCS();	//�������ROLL�������ϵ
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
							//�����������˨���뵽�ֹܼа���
							pBetweenPlate->AppendLsRef(*pLsRef);
							pLsRef->GetLsPtr()->AddL0Thick(pBetweenPlate->handle,TRUE);
							pLsRef->GetLsPtr()->CalGuigeAuto();
							(*pLsRef)->SetModified();
							(*pLsRef)->Create3dSolidModel();
							g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
						}
						LSSPACE_STRU lsSpace;
						GetLsSpace(lsSpace,pParamPlate->GetFirstLsRef()->GetLsPtr()->get_d());
						vertex_array[2*i].x=vertex_array[2*i+1].x=minx-lsSpace.EndSpace; //�˴����Ƹֹ����֮��ľ���
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
				{	//���߸˼�Ϊ�Ǹ�
					CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPart;
					if(fabs(pBetweenPlate->ucs.axis_z*pLineAngle->get_norm_x_wing())>fabs(pBetweenPlate->ucs.axis_z*pLineAngle->get_norm_y_wing()))
					{	//X֫Ϊ��ǰ֫
						desLjPartPara.angle.cur_wing_x0_y1=0;
						if(DISTANCE(pLineAngle->Start(),pBetweenPlate->ucs.origin)<DISTANCE(pLineAngle->End(),pBetweenPlate->ucs.origin))
						{	//ʼ������
							desLjPartPara.start0_end1=0;
							pLineAngle->desStartPos.wing_x_offset.gStyle=4;
							pLineAngle->desStartPos.wing_x_offset.offsetDist=-pBetweenPlate->GetThick();
						}
						else
						{	//�ն�����
							desLjPartPara.start0_end1=1;
							pLineAngle->desEndPos.wing_x_offset.gStyle=4;
							pLineAngle->desEndPos.wing_x_offset.offsetDist=-pBetweenPlate->GetThick();
						}
					}
					else
					{	//Y֫Ϊ��ǰ֫
						desLjPartPara.angle.cur_wing_x0_y1=1;
						if(DISTANCE(pLineAngle->Start(),pBetweenPlate->ucs.origin)<DISTANCE(pLineAngle->End(),pBetweenPlate->ucs.origin))
						{	//ʼ������
							desLjPartPara.start0_end1=0;
							pLineAngle->desStartPos.wing_y_offset.gStyle=4;
							pLineAngle->desStartPos.wing_y_offset.offsetDist=-pBetweenPlate->GetThick();
						}
						else
						{	//�ն�����
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
					pCmdLine->FillCmdLine("��ѡ��ֹܼа������������˨:","");
#endif
					g_pSolidSet->SetDisplayType(DISP_SOLID);
					g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
					Invalidate(FALSE);
					OpenWndSel();	//����ѡ����˨
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
					OnOperOther();	//ȡ����������״̬
					g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
				}
			}
		}
		else
		{
			pCmdLine->CancelCmdLine();	
			//�����жϣ������������
			if(pBetweenPlate)
			{
				console.DispPartSet.DeleteNode(pBetweenPlate->handle);
				g_pSolidDraw->DelEnt(pBetweenPlate->handle);
				console.DeletePart(pBetweenPlate->handle);
			}
			return FALSE;
		}
		//OnOperOther();	//ȡ������ѡ��
	}
	vertex_arr[0].feature=1;
	pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertex_arr[0]));
	if(fFirstFlWidth>0)
	{
		vertex_arr[1].feature=1;
		pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertex_arr[1]));
	}
	for(int i=0;i<2*n;i++)
	{	//����������Yֵ��С�������μ���ֹܼа嶥���б���
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
		{	//������������
			vertex_array[i].feature=1;
			pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertex_array[i]));
		}
	}
	
	if(bOnlyOnePlate)
	{	//����һ���ʱ����Ҫ������ڶ����ֹ���صĶ���
		if(fSecondFlWidth>0)
		{
			vertex_arr[3].feature=1;
			pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertex_arr[3]));
		}
		vertex_arr[2].feature=1;
		pVertex=pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertex_arr[2]));
		pVertex->m_bWeldEdge=TRUE;	//���ں�������ʼ��
	}
	UCS_STRU ucs;	//�ٶ�������ϵ
	BOOL bMirCreate=FALSE;
	if((pBaseTube||pBaseSphere)&&bOnlyOnePlate)
	{	//����һ����ֹܼа�
		if(pBaseTube)
		{
			f3dPoint len_vec=pBaseTube->End()-pBaseTube->Start();
			normalize(len_vec);
			//���ֹ���������׼�ֹ����ڵ��洹ֱ
			if(fabs(len_vec*pBetweenPlate->ucs.axis_z)>EPS_COS2)
			{	//�ֹ�ʵ���޽��� ���Բ����ʼ��
				double R=pBaseTube->GetDiameter()*0.5;
				vertice=inner_arc_start;
				vertice.feature=1;
				pVertex=pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertice));
				if(fabs(pBetweenPlate->ucs.axis_z*pBaseTube->ucs.axis_z)>EPS_COS)	//Բ��
				{	//Բ��
					pVertex->type=2;	//ָ���뾶Բ��,centerΪ������� wht 12-06-15
					pVertex->center.Set();
					//pVertex->center=datumIntersPos;
					pVertex->radius=-R;
				}
				else
				{	//��Բ��
					pVertex->type=3;
					pVertex->center=datumIntersPos;
					if(pBetweenPlate->ucs.axis_z*pBaseTube->ucs.axis_z>0)
						pVertex->column_norm=pBaseTube->ucs.axis_z;
					else
						pVertex->column_norm=-pBaseTube->ucs.axis_z;
					pVertex->radius=-R;
				}
				pVertex->m_bWeldEdge=TRUE;	//���ں�������ʼ��
			}
			else if(fabs(len_vec*pBetweenPlate->ucs.axis_z)<EPS)
			{	//���ֹ���������׼�ֹܹ���
				vertice=inner_arc_start;
				vertice.feature=1;
				pVertex=pBetweenPlate->vertex_list.append(PROFILE_VERTEX(vertice));
				pVertex->m_bWeldEdge=TRUE;	//���ں�������ʼ��
			}
			else	//�������δ����
				return FALSE;
		}
		else if(pBaseSphere)
		{	//Բ�����
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
			pVertex->m_bWeldEdge=TRUE;	//���ں�������ʼ��
		}
	}
	else if(pBaseSphere&&!bOnlyOnePlate)
	{	//������С��ֹܼа�����
		ucs=pBetweenPlate->ucs;
		CLDSNode *pBaseNode=console.FromNodeHandle(pBaseSphere->hDatumNode);
		if(pBaseNode)
			ucs.origin=pBaseNode->Position(true);
		else
			ucs.origin=pBaseSphere->ucs.origin;
		//���е�
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
			//�����жϣ������������
			if(pBetweenPlate)
			{
				console.DispPartSet.DeleteNode(pBetweenPlate->handle);
				g_pSolidDraw->DelEnt(pBetweenPlate->handle);
				console.DeletePart(pBetweenPlate->handle);
			}
			return FALSE;
		}
		//Ĭ��Ϊ���п��
		if(plate_dlg.m_bUseTangWidth)
			fWidth=vertice.y;
		else	
			fWidth=plate_dlg.m_fTangentWidth;	//ʹ���û�����Ŀ��
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
		//ͬ�����ɸֹ����ĸְ�
		bMirCreate=plate_dlg.m_bMirCreate;
	}
	pBetweenPlate->m_hPartWeldParent=pDatumTube1->handle;
	//����ֹܼа���صĹ�����ӵ���Ʋ����б���
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
		//����ֹܼа���صĹ�����ӵ���Ʋ����б���
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
			{	//ѡ������������
				GEPOINT vertex1(anchors[0].locate.vertex.x,anchors[0].locate.vertex.y),
						vertex2(anchors[1].locate.vertex.x,anchors[1].locate.vertex.y);
				if(pfLen)
					*pfLen=ftoi(DISTANCE(vertex1,vertex2));
				int n=pDatumPlate->vertex_list.GetNodeNum();
				int prev_id=(anchors[0].locate.vertex.idVertex-1+n-1)%n+1;	//ǰһ������id
				int next_id=(anchors[0].locate.vertex.idVertex-1+1)%n+1;	//��һ������id
				if(next_id==anchors[1].locate.vertex.idVertex)
					distArr[1]=-1*ftoi(pRibPlate->thick*2.5);
				else if(prev_id==anchors[1].locate.vertex.idVertex)
					distArr[1]=ftoi(pRibPlate->thick*2.5);
			}
			else if(anchors[0].ciType==2)
			{	//ѡ��������˨
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
					{	//Ĭ�Ͽ����߲�
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
		{	//ѡ�ж�λ��˨�������㣬ѡ����˼�
			CLDSLineAngle *pAngle=(CLDSLineAngle*)pDatumPlate->BelongModel()->FromPartHandle(anchors[1].locate.hVecRod,CLS_LINEANGLE);
			if(pAngle==NULL)
				return false;
			CDesignLjPartPara *pLjPara=NULL;
			if(pDatumPlate->IsPlate())
				pLjPara=((CLDSPlate*)pDatumPlate)->GetLjRodPara(pAngle->handle);
			if(pLjPara==NULL)
				return false;
			if(anchors[0].ciType==2)
			{	//ѡ��λ��˨
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
			{	//ѡ��λ��

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
	//�л���ʵ����ʾģʽ
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	g_pSolidDraw->ReleaseSnapStatus();
	//ʰȡ�߰�Ķ�λ��׼����
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,
		GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE)|
		GetSingleWord(SELECTINDEX_LINETUBE)|GetSingleWord(SELECTINDEX_LINEANGLE));
	DWORD dwhObj=0,dwExportFlag=0;
	pCmdLine->FillCmdLine("RibPlate ��ѡ���߰�ĵױ߶�λ��׼����(�ֹܡ��Ǹֻ�ְ�):","");
	while(pDatumPart.IsNULL())
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<=0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		dwhObj=obj.hRelaObj;	//dwhObj��һ����ѡ�ж���ľ����ʶ����Ҫת��
		if(dwhObj==0&&g_pSolidSnap->GetLastSelectEnts(id_arr)>0)
			dwhObj=id_arr[0];
		pDatumPart=Ta.Parts.FromHandle(dwhObj);
	}
	if(pDatumPart->IsPlate())
		pCmdLine->FinishCmdLine(CXhChar16("�ְ�0x%X",pDatumPart->handle));
	else if(pDatumPart->GetClassTypeId()==CLS_LINETUBE)
		pCmdLine->FinishCmdLine(CXhChar16("�ֹ�0x%X",pDatumPart->handle));
	else if(pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
		pCmdLine->FinishCmdLine(CXhChar16("�Ǹ�0x%X",pDatumPart->handle));
	else
		pCmdLine->FinishCmdLine(CXhChar16("����0x%X",pDatumPart->handle));
	g_pSolidDraw->SetEntSnapStatus(pDatumPart->handle);
	//ʰȡ�߰�Ĺ�������1
	if(!pDatumPart->IsAngle())
	{
		CSysPromptObject prompt;
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,0);	//���ԭ�в�׽״̬
		CDisplayNodeAtFrontLife showNodes;
		if(pDatumPart->IsTube())	//�ֹ�
		{
			//prompt.SetPromptPicture();
			//���ط����������߰��ͼ����ͼ����ʾ����
			//�ֹ�����:	B.�ֹ�+��׼�ڵ�
			//  ���߰�: E.�ֹ�+�����ְ�(��:�ֹ����)
			//			F.�ֹ�+�ֹ�(��:�ֹܼа�)	
			//  ���߰�: H.�ֹ�+2�������ְ�
			verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prPoint);
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINETUBE)|
				GetSingleWord(SELECTINDEX_PARAMPLATE)|GetSingleWord(SELECTINDEX_PLATE));
			pCmdLine->FillCmdLine("RibPlate ��ѡ����򿿽Ӹֹܡ��ְ�������߰�Ķ�λ�ڵ�:","");
			showNodes.DisplayNodeAtFront();
		}
		else if(pDatumPart->IsPlate())
		{
			//prompt.SetPromptPicture();
			//���ط����������߰��ͼ����ͼ����ʾ����
			//�ְ��߰�: C.�ְ�+��׼��˨+���򹹼�
			//  ���߰�: D.�ְ�+�����ְ�+��˨��λ
			//          E.�ֹ�+�����ְ�(��:�ֹ����)
			//  ���߰�: G.�ְ�+2�������ְ�+��˨��λ
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINETUBE)|
				GetSingleWord(SELECTINDEX_PARAMPLATE)|GetSingleWord(SELECTINDEX_PLATE));
			pCmdLine->FillCmdLine("RibPlate ��ѡ����򿿽Ӹֹܻ�ְ壬�޲��򿿽��߰���س�:","");
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
			dwhObj=obj.hRelaObj;	//dwhObj��һ����ѡ�ж���ľ����ʶ����Ҫת��
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
				break;	//��ѡ�������˼���س�ȷ�ϲ����������˼�
		}
		g_pSolidDraw->SetEntSnapStatus(pDatumPart->handle,false);
		if(pRelaPart1.IsHasPtr())
		{
			if(pRelaPart1->IsPlate())
				pCmdLine->FinishCmdLine(CXhChar16("�ְ�0x%X",pRelaPart1->handle));
			else if(pRelaPart1->GetClassTypeId()==CLS_LINETUBE)
				pCmdLine->FinishCmdLine(CXhChar16("�ֹ�0x%X",pRelaPart1->handle));
			else
				pCmdLine->FinishCmdLine(CXhChar16("����0x%X",pRelaPart1->handle));
			g_pSolidDraw->SetEntSnapStatus(pRelaPart1->handle);
		}
		else if(pDatumNode!=NULL)
		{
			pCmdLine->FinishCmdLine(CXhChar16("�ڵ�0x%X",pDatumNode->handle));
			g_pSolidDraw->SetEntSnapStatus(pDatumNode->handle);
		}
		verifier.SetVerifyFlag(OBJPROVIDER::LINESPACE,0);
	}
	//ʰȡ�߰�Ĺ�������2
	if(pRelaPart1.IsHasPtr()&&pRelaPart1->IsPlate()
		&&(pDatumPart->IsPlate()||pDatumPart->GetClassTypeId()==CLS_LINETUBE))
	{	//�����߰�
		long* id_arr=NULL,retcode=0;
		DWORD dwhObj=0,dwExportFlag=0;
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,
			GetSingleWord(SELECTINDEX_PARAMPLATE)|GetSingleWord(SELECTINDEX_PLATE));
		pCmdLine->FillCmdLine("RibPlate ��ѡ������߰����һ�࿿�Ӹְ壬���࿿�ӽ��߰���س�:","");
		while(true)
		{
			if((retcode=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;	//dwhObj��һ����ѡ�ж���ľ����ʶ����Ҫת��
			pRelaPart2=Ta.Parts.FromHandle(dwhObj);
			if(pRelaPart2.IsHasPtr() || obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
				break;	//��ѡ�������˼���س�ȷ�ϲ����������˼�
		}
		if(pRelaPart2.IsHasPtr())
			pCmdLine->FinishCmdLine(CXhChar16("�ְ�0x%X",pRelaPart2->handle));
		else
			pCmdLine->FinishCmdLine("<�س�>");
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
			//ѡ��Z��ƫ����
			CTowerPropertyDlg *pPropertyDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
			if(pPropertyDlg&&pRibPlate->GetClassTypeId()==CLS_PARAMPLATE)
			{
				int iParamType=pRibPlate.ParamPlatePointer()->m_iParamType;
				if(iParamType!=TYPE_TUBESTAYWIREPLATE)
				{	//���߰���ʾû��������ΪZ������ID wht 18-12-26
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
	pCmdLine->FillCmdLine("����:", "");
#endif
	g_pSolidDraw->ReleaseSnapStatus();
	if (bRet)
	{	//��һ���߰�ִ�гɹ��󣬼����߰��������
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
	char* key=strtok(cmdstr,",��");
	if(x)
	{
		if(key==NULL)
			return false;
		else
			*x=atof(key);
	}
	key=strtok(NULL,",��");
	if(y)
	{
		if(key==NULL)
			return false;
		else
			*y=atof(key);
	}
	key=strtok(NULL,",��");
	if(z)
	{
		if(key==NULL)
			return false;
		else
			*z=atof(key);
	}
	return true;
}

//���ݸְ���Ų��Ҷ�Ӧ�ְ壬��ʼ�����β���
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
	{	//TODO:������,��Ҫ��������������иĽ�,Ŀǰ��֧�ֽ��߰� wht 16-10-28
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
	{	//�Ǹ��߰�
		if(pDatumNode==NULL&&pDatumBolt==NULL)
		{
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_BOLT));
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("RibPlate Please select rib plate's location node or bolt int the datum angle:");
#else
			cmdStr.Format("RibPlate ��ѡ���߰��ڻ�׼�Ǹ��ϵĶ�λ�ڵ����˨:");
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
				dwhObj=obj.hRelaObj;	//dwhObj��һ����ѡ�ж���ľ����ʶ����Ҫת��
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
			cmdStr.Format("RibPlate ��ѡ���߰��ڻ�׼�˼��ϵĶ�λ�ڵ�:");
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
				dwhObj=obj.hRelaObj;	//dwhObj��һ����ѡ�ж���ľ����ʶ����Ҫת��
				pDatumNode=Ta.Node.FromHandle(dwhObj);
			}
		}
		//�����߰�Ĭ�ϲ����������߰�ʵ��
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
		/*���������١���ΰ��Ӧ���ֹ������߰�����ֱ��ͨ���߰�Ի��������Ƹ��ӷ���
			����Ҫͨ������������������зֲ����  wxc--2017.6.15
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
{	//���߰�	��Ҫѡ��λ��׼��˨
	//�ְ��߰�  ��Ҫѡ��λ��׼��˨�Լ��ְ����췽��λ����
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
			pCmdLine->FillCmdLine("��ѡ���߰�Ķ�λ���������˨:","");
#endif
		else if(hits==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select rib plate's direction rod��vertex or bolt, press enter by default:","");
#else
			pCmdLine->FillCmdLine("��ѡ���߰�Ļ�������Ļ�׼�˼����궨���������˨, �س�ȡĬ�Ϸ���:","");
#endif
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART);
		}
		else if(hits==2)
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select rib plate's bending plane location vertex or bolt, press enter when no bending:","");
#else
			pCmdLine->FillCmdLine("��ѡ���߰�������涨λ���������˨����������ʱ�س�:","");
#endif
		else if(hits==3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select rib plate's bending plane tip location vertex�� bolt or direction rod, press enter as default:","");
#else
			pCmdLine->FillCmdLine("��ѡ���߰���������ĩ�˶�λ�����㡢��˨��������˼����س��Զ�ʶ��:","");
#endif
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART);
		}
		int retcode=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier);
		if(retcode>0)
		{
			SELOBJ obj(dwhObj,dwExportFlag);
			if(obj.provider==1&&pDrawing)	//ͼֽͼԪ
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
							anchors[hits].ciType=3;	//��������
							break;
						}
					}
				}
				else if(pEnt->GetDbEntType()==IDbEntity::DbCircle)
				{
					anchors[hits].locate.hBolt=hiberid.HiberDownId(1);
					anchors[hits].ciType=2;	//ѡ��ͼֽ�е�Բ�ף�������˨��
				}
			}
			else if(obj.provider==2)	//ʵ��ģ��
			{
				if(obj.iObjType==CLS_BOLT)
				{
					anchors[hits].ciType=2;	//ѡ��ʵ��ģ���е���˨
					anchors[hits].locate.hBolt=obj.hObj;
				}
				else
				{
					anchors[hits].ciType=1;	//ѡ��ʵ��ģ���еĸ˼�
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
		//else if(retcode==0) �������ʾδѡ���κζ���
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
	{	//��������ѡ������λ��ʱ�������ѡ��Ķ�λ��Ϊ������ĩ�㣬��һ��Ϊ���������
		ANCHOR_PARAM temp=anchors[2];
		anchors[2]=anchors[3];
		anchors[3]=temp;
	}
	//�ָ���׼�ְ��ʵ����ʾ
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
	//��ȡֱ�������Ըְ�Ϊ�װ����ͨ�߰壬�Ա�����û������� wjh-2016.5.6
	if(anchors[0].ciType==3&&anchors[1].ciType==3&&anchors[2].ciType==3&&anchors[3].ciType==0)
		anchors[3]=anchors[1];	//ѡ�������ְ���������ʱ,�Զ�ʶ��Ϊ�������ϵĵȾ��������߰�
	memcpy(ribDlg.m_xRibPlate.plate.anchors,anchors,sizeof(ANCHOR_PARAM)*4);
	ribDlg.m_xRibPlate.plate.bOnDatumPlateTopFace=TRUE;
	ribDlg.m_xRibPlate.m_cPlateType=3;
	ribDlg.m_xRibPlate.fBottomWidth=50;	//��Ӧ�ڰ��Ϻ����߰�ĸ߶Ȳ�������Ĭ��Ϊ50mm wjh-2016.6.18
	CLDSParamPlate* pRibPlate=ribDlg.CreateRibPlate();
	pRibPlate->iSeg=pDatumPlate->iSeg;
	ribDlg.SetRibPlate(pRibPlate);
	//�����û�ѡ�������ʼ���߰����
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
	//ѡ�иְ�͸������ȷ���߰����ͣ��ֹ����|�ֹ����߰�
	CString cmdStr,cmdValue;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH			
	pCmdLine->FillCmdLine("RibPlate Please select rib plate's type[Elbow (U)/�ֹ����߰�(D)]:","");
#else
	pCmdLine->FillCmdLine("RibPlate ѡ���߰�����[�ֹ����(1)/�ֹ����߰�(2)] <1>:","");
#endif
	if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"1|2"))
		return NULL;
	if(cmdStr.GetLength()>0&&cmdStr[0]=='2')
	{	//��Ƹֹ����߰�
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
	{	//�����������飬������߰�������װ��Ƿ������߻��壬��ֱ�ӵ����߰�Ի�����Ƹֹ������ӱ��  wxc-2017.6.20
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
	//ͨ������ڳ�ʼ�����������Ϣ
	GEPOINT lineStart=pDatumTube->Start(),lineEnd=pDatumTube->End();
	GEPOINT vStdRodVec=lineEnd-lineStart;
	Standarized(vStdRodVec);
	lineStart-=vStdRodVec*pDatumTube->startOdd();
	lineEnd  +=vStdRodVec*pDatumTube->endOdd();
	coord_trans(lineStart,pDatumPlate->ucs,FALSE,TRUE);
	coord_trans(lineEnd,pDatumPlate->ucs,FALSE,TRUE);
	char cRibToBasePlateSide=0;
	if(lineStart.z>pDatumPlate->GetNormOffset()-20&&lineEnd.z>pDatumPlate->GetNormOffset()-20)
		cRibToBasePlateSide='U';	//�߰�ֻ���ڸְ嶥�棨�����޸ֹܣ�
	else if(lineStart.z<pDatumPlate->GetNormOffset()+pDatumPlate->Thick+20&&lineEnd.z<pDatumPlate->GetNormOffset()+pDatumPlate->Thick+20)
		cRibToBasePlateSide='D';	//�߰�ֻ���ڸְ���棨�����޸ֹܣ�
	else //if(cRibToBasePlateSide==0)
	{
//#ifdef AFX_TARG_ENU_ENGLISH			
//		pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
//#else
//		pCmdLine->FillCmdLine("�߰�λ�ڸְ�ߵ�[����(U)/�װ�(D)] <U>:","");
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
		pCmdLine->FillCmdLine("ѡ��λ��˨�����㣬��ָ���ױ���ת�Ƕ�(Ĭ���Ի�׼��)���Ժ�����(W)����[�ױ�����(V)/��λʰȡ��(P)] <0>:","");
	else
		pCmdLine->FillCmdLine("ѡ��λ��˨�����㣬��ָ���ױ���ת�Ƕ�(�Ի�׼��)����[�ױ�����(V)/��λʰȡ��(P)] <0>:","");
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
		if(obj.provider==OBJPROVIDER::SOLIDSPACE&&dwhObj>0)	//ѡ��λ��˨
		{
			pDatumBolt=(CLDSBolt*)Ta.Parts.FromHandle(dwhObj,CLS_BOLT);
			if(pDatumBolt)
			{
				anchor.locate.hBolt=dwhObj;
				anchor.ciType=2;
			}
		}
		if(obj.provider==OBJPROVIDER::DRAWINGSPACE&&dwhObj==pDatumPlate->handle)
		{	//ֱ��ʰȡ�ְ������������˨��
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
						anchor.ciType=3;	//��������
						break;
					}
				}
			}
			else if(pEnt->GetDbEntType()==IDbEntity::DbCircle)
			{
				anchor.locate.hBolt=hiberid.HiberDownId(1);
				anchor.ciType=2;	//ѡ��ͼֽ�е�Բ�ף�������˨��
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
	char cCmdType='E';	//�Ըֹܻ�׼����תָ���Ƕȶ�λ�ױ�
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
	else	//ָ���ױ�������λ��ʽ
		cCmdType=toupper(cmdStr[0]);
	pCmdLine->FinishCmdLine();
	verifier.ClearSnapFlag();
	if(anchor.ciType==3)
	{	//��ѡ���˶�λ����
		cCmdType='P';
		vSpecLocalBtmEdgeVecPick.Set(anchor.locate.vertex.x,anchor.locate.vertex.y);
	}
	else if(cCmdType=='W')
	{
#ifdef AFX_TARG_ENU_ENGLISH			
		pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
#else
		pCmdLine->FillCmdLine("ָ���Ըֹܺ��������߰�ױߵ���ת�Ƕ�<0>:","");
#endif
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
			return NULL;
		degree=cmdStr.GetLength()>0?atof(cmdStr):0;
	}
	else if(cCmdType=='V')
	{	//ָ��ȫ������
#ifdef AFX_TARG_ENU_ENGLISH			
		pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
#else
		pCmdLine->FillCmdLine("ָ���ױߵ�������շ���[0.�Զ���|1.X+|2.X-|3.Y+|4.Y-|6.Z+|6.Z-]<0>:","");
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
			pCmdLine->FillCmdLine("ָ���ױ����췽��Ĳ��շ���(ģ������ϵ)�������:","");
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
	{	//ָ���ְ��Ͼֲ�����
#ifdef AFX_TARG_ENU_ENGLISH			
		pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
#else
		pCmdLine->FillCmdLine((char*)CXhChar50("������װ�ֲ�����ϵ�µױ�ʰȡ�������<X=%.0f,Y=%.0f>:",
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
//	pCmdLine->FillCmdLine("ָ����װ��Ե�ı߾����������(N) <0>:","");
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
	{	//��˨��λ
		ribDlg.m_xRibPlate.params.elbow.J.value.iValue=2;	//��˨��λ
		ribDlg.m_xRibPlate.params.elbow.hDatumBolt.Set(pDatumBolt->handle);
	}
	else if(cCmdType=='E')
	{	//�Ի�׼����ת
		ribDlg.m_xRibPlate.params.elbow.J.value.iValue=0;
		ribDlg.m_xRibPlate.params.elbow.rotAngle.Set(degree);
	}
	else if(cCmdType=='W')
	{	//�Ժ�������ת
		ribDlg.m_xRibPlate.params.elbow.J.value.iValue=1;
		ribDlg.m_xRibPlate.params.elbow.rotAngle.Set(degree);
	}
	else if(cCmdType=='V')
	{	//ָ��ȫ�ַ���
		ribDlg.m_xRibPlate.params.elbow.J.value.iValue=3;
		ribDlg.m_xRibPlate.params.elbow.vAxisXVecX.Set(vSpecGlobalBtmEdgeVec.x);
		ribDlg.m_xRibPlate.params.elbow.vAxisXVecY.Set(vSpecGlobalBtmEdgeVec.y);
		ribDlg.m_xRibPlate.params.elbow.vAxisXVecZ.Set(vSpecGlobalBtmEdgeVec.z);
	}
	else if(cCmdType=='P')
	{	//ָ���װ��ϱ�ʶ��(�ֲ�����)
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
	if(obj.provider==OBJPROVIDER::SOLIDSPACE)	//ѡ��λ��˨
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
	{	//ֱ��ʰȡ�ְ������������˨��
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
						anchor.ciType=3;	//��������
						break;
					}
				}
			}
			else if(pEnt->GetDbEntType()==IDbEntity::DbCircle)
			{
				anchor.locate.hBolt=hiberid.HiberDownId(1);
				anchor.ciType=2;	//ѡ��ͼֽ�е�Բ�ף�������˨��
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
	//1.ѡ��λ��˨������������붨λʰȡ��
#ifdef AFX_TARG_ENU_ENGLISH			
	pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
#else
	pCmdLine->FillCmdLine("RibPlate ѡ��λ��˨�����������װ�����ϵ������<0,0>:","");
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
	{	//��ѡ���˶�λ����
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
	//2.���ݶ�λ���Զ�����ȷ��X�᷽��Z��ƫ������ʼֵ
	BOOL bOnDatumPlateTopFace=TRUE,bOnSidePlateTopFace=TRUE,bNeedInputVec=FALSE;
	//��ʼ���߰��ڻ���(���)���滹�ǵ���
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
		{	//���ݶ�λ���ж�
			GEPOINT verfiy_vec=ptSpecOrg;
			if(pDatumBolt)
				verfiy_vec=pDatumBolt->ucs.origin;
			else
				coord_trans(verfiy_vec,pDatumPlate->ucs,TRUE);
			verfiy_vec-=pPlate2->ucs.origin;
			normalize(verfiy_vec);
			double dd=verfiy_vec*pPlate2->ucs.axis_z;
			*pbTopFace=(dd>0);
			//��λ��׼��Ϊ�����˨(����)���޷����ݶ�λ��ȷ��X�᷽��ʱ��Ҫ�û�����
			if(i==1)
			{
				bNeedInputVec=(fabs(dd)<EPS||anchor.hRelaObj==pSidePlate->handle||
							   (pDatumBolt&&pSidePlate->FindLsByHandle(pDatumBolt->handle)));
			}
		}
		else
			*pbTopFace=!(maxValue.number<0&&minValue.number<0);
	}
	//���ݶ�λ��
	double fOffsetZ=0;
	GEPOINT axis_x=bOnSidePlateTopFace?pSidePlate->ucs.axis_z:pSidePlate->ucs.axis_z*-1;
	GEPOINT axis_y=bOnDatumPlateTopFace?pDatumPlate->ucs.axis_z:pDatumPlate->ucs.axis_z*-1;
	GEPOINT axis_z=axis_x^axis_y;
	normalize(axis_z);
	if(pDatumBolt)
	{	//ѡ��λ��˨ʱ��������Ĺ�����˨ȡ����һ��,δ�ҵ�ʱĬ��Ϊ�����˨���
		CLDSGeneralPlate *pPlate=pDatumPlate->FindLsByHandle(pDatumBolt->handle)?pDatumPlate:pSidePlate;
		fOffsetZ=10000;	//��ʼֵ
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
	{	//�����㶨λ��Ĭ��Ϊ��Z�᷽��ӽ����������е�
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
	//3.���ݲ��������߰�
	CDesignRibbedPlateDlg ribDlg;
	ribDlg.m_pDatumPart=pDatumPlate;
	ribDlg.m_pRelaPart1=pSidePlate;
	ribDlg.m_xRibPlate.bOnRelaPlateTopFace=bOnSidePlateTopFace;
	ribDlg.m_xRibPlate.plate.bOnDatumPlateTopFace=bOnDatumPlateTopFace;
	//ԭ��
	ribDlg.m_xRibPlate.plate.bSpecOrg=(pDatumBolt==NULL);
	ribDlg.m_pDatumBolt=pDatumBolt;
	if(ribDlg.m_xRibPlate.plate.bSpecOrg)
	{
		ribDlg.m_xRibPlate.plate.fOriginX=ptSpecOrg.x;
		ribDlg.m_xRibPlate.plate.fOriginY=ptSpecOrg.y;
	}
	//X�᷽��
	ribDlg.m_xRibPlate.plate.iAxisXType=5;
	ribDlg.m_xRibPlate.plate.bSpecAxisX=FALSE;
	//Y�᷽��
	ribDlg.m_xRibPlate.plate.fGradientDegAngle=90;
	//Z��ƫ����
	ribDlg.m_xRibPlate.plate.fOffsetDistZ=fOffsetZ;
	ribDlg.m_xRibPlate.nSegI=pDatumPlate->iSeg;
	ribDlg.m_xRibPlate.cMaterial=pDatumPlate->cMaterial;
	ribDlg.AutoCalRibPlateWidth();
	CLDSParamPlate *pRibPlate=ribDlg.CreateRibPlate();
	ribDlg.SetRibPlate(pRibPlate);
	//4.����X�᷽��
	char iAxisXType=5;
	GEPOINT vSpecGlobalBtmEdgeVec;
	if(bNeedInputVec)
	{
	#ifdef AFX_TARG_ENU_ENGLISH			
		pCmdLine->FillCmdLine("Please select rib plate's location vertex or bolt:","");
	#else
		pCmdLine->FillCmdLine("RibPlate ָ���ױߵ�������շ���[0.X+|1.X-|2.Y+|3.Y-|4.����|5.�Զ�����|6.�������|7.������]<6>:","");
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
			pCmdLine->FillCmdLine("RibPlate ָ���ױ����췽��Ĳ��շ���(ģ������ϵ)�������:","");
	#endif
			CmdStrToNumber(cmdStr,&vSpecGlobalBtmEdgeVec.x,&vSpecGlobalBtmEdgeVec.y,&vSpecGlobalBtmEdgeVec.z);
		}
		else if(iAxisXType>=0&&iAxisXType<=3)
			vSpecGlobalBtmEdgeVec=CStdVector::GetVector('0'+iAxisXType+1);
	}
	//X�᷽�����仯������߰�
	//TODO:���Ž��߰����ʱĬ����ʾ����(�������:Ĭ��Z��ƫ��,λ��ʾ��ͼ,����ְ�����)
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
	//3.�����߰�Y�ᶨλ�˼�
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
	pCmdLine->FillCmdLine("RibPlate ѡ��Y�ᶨ����˨���˼��������߰���б�Ƕ�<90>:","");
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
	//Y������仯����¸ְ�
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
	//������ΰҪ��������߰�Ķ�λ�������ֱ���˳�������Ҫͨ�����������������Ϣ�ȣ�����̫���� wxc-2018.1.24
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
	if(pDatumPart->IsLinePart()&&pRelaPart1.IsNULL()&&pRelaPart2.IsNULL())	//�˼��߰壺�Ǹ��߰��ֹ�����
		return DesignSimpleRibPlateOnRod(pDatumPart.RodPointer(),pDatumNode,NULL);
	else if(pDatumPart->IsPlate()&&pRelaPart1.IsNULL()&&pRelaPart2.IsNULL())//�ְ��ϵ���ͨ�߰�
		return DesignSimpleRibPlateOnPlate(pDatumPart.GeneralPlatePointer());
	else if(pDatumPart->GetClassTypeId()==CLS_LINETUBE
		&&pRelaPart1.IsHasPtr()&&pRelaPart1->GetClassTypeId()==CLS_LINETUBE)//�ֹܼа�
		return DesignTubeBetweenPlate(pDatumPart.LineTubePointer(),pRelaPart1.LineTubePointer());
	else if((pDatumPart->GetClassTypeId()==CLS_LINETUBE&&pRelaPart1->IsPlate()&&pRelaPart2.IsNULL())||
		(pDatumPart->IsPlate()&&pRelaPart1->GetClassTypeId()==CLS_LINETUBE&&pRelaPart2.IsNULL()))
	{	//�ֹ����
		CLDSLineTube* pDatumTube=pDatumPart->IsTube()?pDatumPart.LineTubePointer():pRelaPart1.LineTubePointer();
		SmartPlatePtr pDatumPlate=pDatumPart->IsPlate()?pDatumPart.GeneralPlatePointer():pRelaPart1.GeneralPlatePointer();
		return DesignElbowRibPlate(pDatumTube,pDatumPlate);
	}
	else if(pDatumPart->IsPlate()&&pRelaPart1->IsPlate()&&pRelaPart2.IsNULL())//���ְ��Ľ��߰�
		return DesignCornerRibPlate(pDatumPart.GeneralPlatePointer(),pRelaPart1.GeneralPlatePointer());
	else if((pDatumPart->GetClassTypeId()==CLS_LINETUBE||pDatumPart->IsPlate())&&
		pRelaPart1->IsPlate()&&pRelaPart2->IsPlate())//�����߰壨����׼����Ϊ�ֹܺ͸ְ����ֲ����߰壩
		return DesignSlotRibPlate(pDatumPart,pRelaPart1.GeneralPlatePointer(),pRelaPart2.GeneralPlatePointer());
	else
		return NULL;
	return pRibPlate;
}

void CLDSView::OnDesignOverlappedPlate()
{	//���Ű�
	m_nPrevCommandID=ID_DESIGN_OVERLAPPED_PLATE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat plate";
#else
	m_sPrevCommandName="�ظ����Ű�";
#endif
	Command("OverlappendPlate");
}
//���Ű������ѹ��뵽���ƹ��������� wjh-2016.9.12
int CLDSView::OverlappendPlate()
{
/*	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr, promptStr;
	UCS_STRU object_ucs,ucs,plate_ucs;
	CSuperSmartPtr<CLDSPart> pBasePlate;
	//�л���ʵ����ʾģʽ
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("please select the datum (parametric) plate for the overlapped plates:","");
#else
	pCmdLine->FillCmdLine("��ѡ����Ű�Ļ�׼(������)�ְ�:","");
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
				pCmdLine->FillCmdLine("ѡ���˴���Ĺ�����������ѡ��:","");
#endif
				g_pSolidDraw->ReleaseSnapStatus();
			}
		}
		else
			return 0;
	}
	if(pBasePlate.IsHasPtr())
	{	//��������Ļ�׼�ְ�ȷ��װ����Ű�Ĳο�����ϵ(�ְ�����ϵ)
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
	if(plate_dlg.m_bChooseWCS)	//��������ϵ
		tem_ucs = theApp.env.mcs;
	else						//�������ϵ
		tem_ucs = pBasePlate->ucs;
	Ta.BeginUndoListen();
	if(pBasePlate->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pPlate= (CLDSPlate*)console.AppendPart(CLS_PLATE);
		pPlate->CopyProperty(pBasePlate.PlatePointer());
		pPlate->m_tInnerColumnNorm=pBasePlate.PlatePointer()->m_tInnerColumnNorm;	//��Բ���߷���
		pPlate->m_fInnerRadius=pBasePlate.PlatePointer()->m_fInnerRadius;	//��Բ�뾶
		pPlate->m_tInnerOrigin=pBasePlate.PlatePointer()->m_tInnerOrigin;	//��Բλ��
		pPlate->designInfo.m_hBaseNode=pBasePlate.PlatePointer()->designInfo.m_hBaseNode;//��Ļ�׼�ڵ�
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
		//�����淨��ƫ�ƾ���
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
	pCmdLine->FillCmdLine("����:","");
#endif
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();*/
	return 0;
}

void CLDSView::OnCircularPlate()
{	//�����
	m_nPrevCommandID=ID_CIRCULAR_PLATE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat circular plate";
#else
	m_sPrevCommandName="�ظ������";
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
		AfxMessageBox("��ѡ�ڵ㲻�ڸ˼���!");
#endif
		return FALSE;
	}
	baseline.startPt=pLineTube->pStart->Position(true);
	baseline.endPt=pLineTube->pEnd->Position(true);
	Ta.BeginUndoListen();
	//static CCircularPlateDlg plateDlg;
	//plateDlg.m_fInternalDiameter=0;
	//plateDlg.m_fExternalDiameter=pLineTube->GetDiameter();
	//if(bEndPlate)	//�ֹܷ�ͷ��
	//	plateDlg.m_iDlgType = 1;
	//else 
	//	plateDlg.m_iDlgType = 0;
	CDesignCircularPlateDlg plateDlg;
	plateDlg.m_pDatumNode=pNode;
	plateDlg.m_pDatumTube=pLineTube;
	plateDlg.m_iPlateType=0;	//�����
	plateDlg.para.R=0;
	CLDSPlate *pPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	pPlate->m_hPartWeldParent = pLineTube->handle;
	pPlate->CopyModuleInstanceInfo(pLineTube);	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
	pPlate->designInfo.m_hBaseNode=pNode->handle;	//��Ļ�׼�ڵ�
	pPlate->designInfo.origin.datum_pos_style=2;
	pPlate->designInfo.origin.des_para.RODNODE.hNode=pNode->handle;
	pPlate->designInfo.origin.des_para.RODNODE.hRod=pLineTube->handle;
	CDesignLjPartPara *pLjPara=pPlate->designInfo.partList.Add(pLineTube->handle);
	pLjPara->hPart=pLineTube->handle;
	if(pNode==pLineTube->pStart)
	{
		pLjPara->start0_end1=0;	//ʼ������
		if(bEndPlate)
			plateDlg.para.S=pLineTube->startOdd();
			//plateDlg.m_fNormalOffset=pLineTube->startOdd();
		pPlate->ucs.axis_z=pLineTube->Start()-pLineTube->End();
		normalize(pPlate->ucs.axis_z);
		pPlate->ucs.origin=pLineTube->Start();	//+pPlate->ucs.axis_z*pLineTube->startOdd();
	}
	else if(pNode==pLineTube->pEnd)
	{
		pLjPara->start0_end1=1;	//�ն�����
		if(bEndPlate)
			plateDlg.para.S=pLineTube->endOdd();
			//plateDlg.m_fNormalOffset=pLineTube->endOdd();
		pPlate->ucs.axis_z=pLineTube->End()-pLineTube->Start();
		normalize(pPlate->ucs.axis_z);
		pPlate->ucs.origin=pLineTube->End();	//+pPlate->ucs.axis_z*pLineTube->endOdd();
	}
	else
	{
		pLjPara->start0_end1=2;	//�м�����
		pPlate->ucs.axis_z=pLineTube->End()-pLineTube->Start();
		normalize(pPlate->ucs.axis_z);
		SnapPerp(&pPlate->ucs.origin,baseline.startPt,baseline.endPt,pNode->xArcliftPos);
	}
	pPlate->ucs.axis_x=pLineTube->ucs.axis_x;
	pPlate->ucs.axis_y=pPlate->ucs.axis_z^pPlate->ucs.axis_x;
	f3dPoint columnnorm=pPlate->ucs.axis_z;	//������Բ�����趨�и�Բ����ķ��߳���
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
		//�����жϣ������������
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
	pPlate->layer(2)=pLineTube->layer(2);	//����������ͼ����
	pPlate->Thick=plateDlg.thick;
	pPlate->m_fNormOffset = 0;//-pPlate->GetThick()*0.5;
	pPlate->cMaterial=plateDlg.m_cMaterial;
	//pPlate->ucs.origin+=pPlate->ucs.axis_z*plateDlg.m_fNormalOffset;
	pPlate->designInfo.origin.des_para.RODNODE.len_offset_dist=plateDlg.para.S;
	if(pNode==pLineTube->pStart)
		pPlate->designInfo.origin.des_para.RODNODE.direction=1;	//��-->ʼ
	else 
		pPlate->designInfo.origin.des_para.RODNODE.direction=0;	//ʼ-->��
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
	{	//��ͷ�� 
		f3dPoint vertex_origin,vertex_start,vertex_end;
		int vertex_id=1;
		if(!bFullCir)
		{	//�ְ�Ϊ��Բʱ�������ԭ�� wht 10-08-06
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
			else if(!bFullCir) //����Բʱ��Ҫ������¶��� wht 10-08-06
				pVertex=pPlate->vertex_list.append(PROFILE_VERTEX(vertex_end,0));
			if(pVertex)
				pVertex->center=pPlate->ucs.origin;
		}
	}
	else
	{	//���μӾ���
		if(bFullCir)
		{	//ȫ��
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
		{	//�뻷
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
			if(fabs(columnnorm*pPlate->ucs.axis_z)<EPS_COS)	//Բ��
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
	int design_plate_type=0;//1:��ͨ�ƸֹܵĻ����2:������3:�����߰�4:�ֹ��߰�5:��ˮ��(������ˮ�۵Ļ����)
	CLDSPart *pPart;
	CLDSNode *pBaseNode=NULL;
	CLDSLineTube* pBaseLineTube=NULL;
	CLDSLinePart* pAxisXRefLinePart=NULL;
	CLDSSphere *pDatumSphere=NULL;
	CLDSParamPlate *pParamPlate=NULL; //�����ˮ��ʱ�õ�
	CLDSPlate *pDatumPlate1=NULL,*pDatumPlate2=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("CircularPlate Please select ciruclar plate's assembling positioning tube or sphere:","");
#else
	pCmdLine->FillCmdLine("CircularPlate ��ѡ������װ�䶨λ�ֹܻ�����:","");
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
				pCmdLine->FillCmdLine("no proper parts selected��Please select ciruclar plate's assembling positioning tube or sphere:","");
#else
				pCmdLine->FillCmdLine("û��ѡ�к��ʵĹ�������ѡ������װ�䶨λ�ֹܻ�����:","");
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
	//�л�������ͼ״̬����ѡ��ڵ�
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
	if(pDatumSphere==NULL)
	{	//��׼����Ϊ��ʱ����ѡ���׼�ڵ�
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("please select the assembling location nodes on rod:","");
#else
		pCmdLine->FillCmdLine("��ѡ��˼��ϵ�װ�䶨λ�ڵ�:","");
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
		pCmdLine->FillCmdLine("��ѡ�����ĵ�һ�鶨λ��׼�ְ�,��û��ֱ�ӻس�:","");
	else if(pDatumSphere)	//��׼����Ϊ��ʱ����ѡ��һ���׼�ְ�
		pCmdLine->FillCmdLine("��ѡ�����ĵ�һ�鶨λ��׼�ְ�:","");
#endif
	//�л���ʵ����ʾģʽ
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
			{	//��׼����Ϊ��ʱ���������ɻ����
				if(id_num==0)
				{	//��ͨ�ƸֹܵĻ���������ˮ��
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
					design_plate_type=5;	//��ˮ��
					break;
				}
				else if(pPart&&pPart->GetClassTypeId()==CLS_PLATE)
				{
					pDatumPlate1=(CLDSPlate*)pPart;
					if(pDatumPlate1->m_fInnerRadius>0&&fabs(pDatumPlate1->ucs.axis_z*pBaseLineTube->ucs.axis_z)>0)
						design_plate_type=5;	//��ˮ��
					break;
				}
			}
			if(pDatumPlate1==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("No proper parts selected, please select the first location datum plate for circular plate:","");
#else
				pCmdLine->FillCmdLine("û��ѡ�к��ʵĹ�������ѡ�����ĵ�һ�鶨λ��׼�ְ�:","");
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
	//��Խʽ�ֹ������׼�ֹ�
	CLDSLineTube *pDatumLineTube2=NULL;
	if(pDatumSphere==NULL)
	{	//��׼����Ϊ��ʱ����ѡ���2���׼�ְ� 
		if(design_plate_type!=1&&design_plate_type!=5&&design_plate_type!=-1)
		{	//��ͨ��������ˮ�嶼����Ҫѡ���2���׼�ְ�
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("please select the second location datum plate (or tube) for circular plate< no choice permitted, press enter>:","");
#else
			pCmdLine->FillCmdLine("��ѡ�����ĵڶ��鶨λ��׼�ְ�(��ֹ�)<�ɲ�ѡ,���س�ȷ��>:","");
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
								pCmdLine->FillCmdLine("��ѡ�ֹ����һ��ְ庸�Ӹ�������ͬ��������ѡ�����Ķ�λ��׼�ֹ�:","");
#endif
								pDatumLineTube2=NULL;	//��ֹ��ѡ���ݴ����һ��������������
								g_pSolidDraw->ReleaseSnapStatus();
								continue;
							}
							if(pDatumLineTube2->pStart->handle!=pBaseNode->handle&&pDatumLineTube2->pEnd->handle!=pBaseNode->handle)
							{
#ifdef AFX_TARG_ENU_ENGLISH
								pCmdLine->FillCmdLine("tube selected has no connection with datum nodes, please choose circular plate's location ray tube:","");
#else
								pCmdLine->FillCmdLine("��ѡ�ֹ����׼�ڵ������ӣ�������ѡ�����Ķ�λ���߸ֹ�:","");
#endif
								pDatumLineTube2=NULL;	//��ֹ��ѡ���ݴ����һ��������������
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
								pCmdLine->FillCmdLine("��ѡ�ְ����һ��ְ庸�Ӹ�������ͬ��������ѡ�����ĵڶ��鶨λ��׼�ְ�:","");
#endif
								pDatumPlate2=NULL;	//��ֹ��ѡ���ݴ����һ��������������
								g_pSolidDraw->ReleaseSnapStatus();
								continue;
							}
							break;
						}
					}
					//����һ���׼�ְ�����������һ�ֻ����߰�
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
		design_plate_type=3;//�����߰�
	if(pDatumLineTube2)
		design_plate_type=4;//��Խʽ�ֹ�����
	else if(design_plate_type==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select and confirm circular plate's reference rod in axis X direction, if no circular plate's axis X direction, be vertical with datum tube:","");
#else
		pCmdLine->FillCmdLine("��ѡ��ȷ�ϻ����X�᷽��Ĳο��˼�,��û�л�����X�᷽�����׼�ֹܵ�����ֱ:","");
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
					design_plate_type=4;//�ֹ��߰�
					break;
				}
			}
			if(pAxisXRefLinePart==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("No proper parts selected, Please select circular plate's reference rod in axis X direction:","");
#else
				pCmdLine->FillCmdLine("û��ѡ�к��ʵĹ�������ѡ�����X�᷽��Ĳο��˼�:","");
#endif
				g_pSolidDraw->ReleaseSnapStatus();
				continue;
			}
		}
	}
	//���뻷���ӹ���϶
	double manu_space=0;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("please input circular plate's process clearance<0>:");
#else
	cmdStr.Format("�����뻷���ӹ���϶<0>:");
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
			cmdStr.Format("error in input��please input circular plate's process clearance<0>:");
#else
			cmdStr.Format("������������뻷���ӹ���϶<0>:");
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
		cmdStr.Format("��ȷ������������,���(C)|�߰�(R)|��Խ�ֹ�����(S)<%C>:",cDefaultType);
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
				pCmdLine->FillCmdLine("��Ҫѡ��ؼ���","");
				pCmdLine->FinishCmdLine();
				cmdStr.Format("��ȷ������������,���(C)|�߰�(R)|��Խ�ֹ�����(S)<%C>:",cDefaultType);
#endif
				pCmdLine->FillCmdLine(cmdStr,"");
				continue;
			}
			break;
		}
		if(cDefaultType=='C')
			design_plate_type=2;//������
		else if(cDefaultType=='R')
			design_plate_type=3;//�����߰�
		else if(cDefaultType=='S')
			design_plate_type=4;//��Խ�ֹܵ�����
	}
	else if(design_plate_type==-1)
	{
		cDefaultType='H';
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("please confirm the type of circular plate,circular plate(H)|flashing board(Z)<%C>:",cDefaultType);
#else
		cmdStr.Format("��ȷ������������,�����(H)|��ˮ��(Z)<%C>:",cDefaultType);
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
				pCmdLine->FillCmdLine("��Ҫѡ��ؼ���","");
				pCmdLine->FinishCmdLine();
				cmdStr.Format("��ȷ������������,�����(H)|��ˮ��(Z)<%C>:",cDefaultType);
#endif
				pCmdLine->FillCmdLine(cmdStr,"");
				continue;
			}
			break;
		}
		if(cDefaultType=='H')
			design_plate_type=1;//�����
		else if(cDefaultType=='Z')
			design_plate_type=5;//��ˮ��
	}
	
	BOOL bRet=FALSE;
	if(design_plate_type==1)		//�����
		bRet=FinishDesignCircularPlate(pBaseNode,pBaseLineTube,FALSE,manu_space);
	else if(design_plate_type==2)	//������
		bRet=FinishDesignCirCoverPlate(pBaseNode,pBaseLineTube,pDatumPlate1,pDatumPlate2,manu_space);
	else if(design_plate_type==3)	//�����߰�
	{
		if(pDatumSphere)	//��+��׼�ְ�
			bRet=FinishDesignCirRibPlate(pDatumSphere,NULL,pDatumPlate1,NULL,manu_space);
		else if(pBaseLineTube)	//�ֹ�+�ڵ�+һ��������׼�ְ�
			bRet=FinishDesignCirRibPlate(pBaseLineTube,pBaseNode,pDatumPlate1,pDatumPlate2,manu_space);
	}
	else if(design_plate_type==4)	//�ֹ��߰�
	{	//����ѡ��ֹ��߰�ʱ����Ҫѯ���û��Ƿ���Ҫ����ο��˼� wht 10-08-27
		if(pDatumLineTube2==NULL)
		{	//�ڶ�����׼����Ϊ�ֹ�ʱ����ѡ��X�᷽����ո˼� wht 10-10-26
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select and confirm circular plate's reference rod in axis X direction, if no circular plate's axis X direction, be vertical with datum tube:","");
#else
			pCmdLine->FillCmdLine("��ѡ��ȷ�ϻ����X�᷽��Ĳο��˼�,��û�л�����X�᷽�����׼�ֹܵ�����ֱ:","");
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
						//design_plate_type=4;//�ֹ��߰�
						break;
					}
				}
				if(pAxisXRefLinePart==NULL)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("No proper parts selected, please select circular plate's reference rod in axis X direction:","");
#else
					pCmdLine->FillCmdLine("û��ѡ�к��ʵĹ�������ѡ�����X�᷽��Ĳο��˼�:","");
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
	else if(design_plate_type==5)	//��ˮ��
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
	pCmdLine->FillCmdLine("����:","");
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
	pTubeRibPlate->CopyModuleInstanceInfo(pDatumTube);	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
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
		pTubeRibPlate->SetDesignItemValue('S',pSetUpPart->handle,"ȷ���ֹ��߰�X�᷽��Ĳο��˼�",PARAM_POS,1);
	}
	else 
		pTubeRibPlate->SetDesignItemValue('S',0,"ȷ���ֹ��߰�X�᷽��Ĳο��˼�",PARAM_POS,1);
	pTubeRibPlate->SetDesignItemValue('R',dlg.m_fParamR,"�߰����Բ���뾶",PARAM_SHAPE);
	pTubeRibPlate->SetDesignItemValue('W',dlg.m_fParamW,"����W",PARAM_SHAPE);
	pTubeRibPlate->SetDesignItemValue('N',dlg.m_fParamN,"����N",PARAM_SHAPE);
	pTubeRibPlate->SetDesignItemValue('H',dlg.m_fParamH,"����H",PARAM_SHAPE);
#endif
	double Y=-1,F=-1,A=-1;
	if(dlg.m_bDesignOuterPlate)
		Y=1;
	if(dlg.m_bTopArcConnection)
		A=1;
	if(dlg.m_bCirPlate)
		F=1;
	else	//�ǻ����߰��ǲ���Ҫ��Ӹò���
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
		pTubeRibPlate->SetDesignItemValue('Y',Y,"��ֵ�����ʾ�߰��ڻ�׼�ְ����࣬����λ���ڲ�");
		pTubeRibPlate->SetDesignItemValue('A',A,"��ֵ�����ʾ�߰嶥��ΪԲ�����ӣ������߰嶥��Ϊֱ������",PARAM_SHAPE);
	}
	pTubeRibPlate->SetDesignItemValue('F',F,"��ֵ�����ʾ�ð�Ϊ���ڸֹ��ϵ��߰壬����ð��ڲ�Ϊ��Բ��",PARAM_SHAPE);
	pTubeRibPlate->SetDesignItemValue('D',dlg.m_fNormOffset,"�߰�ķ���ƫ�ƾ���");
	pTubeRibPlate->SetDesignItemValue('P',pDatumNode->handle,"ȷ���ֹ��߰�λ�õĻ�׼��",PARAM_POS,1);
	if(pDatumPlate1)
		pTubeRibPlate->SetDesignItemValue('B',pDatumPlate1->handle,"ȷ���ֹ��߰�λ�õĻ�׼�ְ�",PARAM_POS,1);
	if(pDatumPart2)
		pTubeRibPlate->SetDesignItemValue('C',pDatumPart2->handle,"ȷ���ֹ��߰�λ�õĻ�׼�ְ��ֹ�",PARAM_POS,1);
	//�ӹ���϶ wht 10-08-28
	pTubeRibPlate->SetDesignItemValue('V',manu_space,"�ӹ���϶ֵ");
#endif
	pTubeRibPlate->DesignPlate();
	pTubeRibPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pTubeRibPlate->GetSolidPartObject());	
	if(dlg.m_bMirCreate)
	{
		CLDSParamPlate *pMirPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pMirPlate->CopyProperty(pTubeRibPlate);
		pMirPlate->CopyModuleInstanceInfo(pDatumTube);	//�����ֹ��߰��ͼ����
		pMirPlate->m_hPartWeldParent=pDatumTube->handle;
#ifdef AFX_TARG_ENU_ENGLISH
		pMirPlate->SetDesignItemValue('W',dlg.m_fParamN,"parameter W",PARAM_SHAPE);
		pMirPlate->SetDesignItemValue('N',dlg.m_fParamW,"parameter N",PARAM_SHAPE);
		pMirPlate->SetDesignItemValue('Y',-Y,"positive value or zero shows that rib plate is on the outside of datum plate, otherwise, inside");
#else
		pMirPlate->SetDesignItemValue('W',dlg.m_fParamN,"����W",PARAM_SHAPE);
		pMirPlate->SetDesignItemValue('N',dlg.m_fParamW,"����N",PARAM_SHAPE);
		pMirPlate->SetDesignItemValue('Y',-Y,"��ֵ�����ʾ�߰��ڻ�׼�ְ����࣬����λ���ڲ�");
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
	//ʰȡ�����Ķ�λ��׼����
	SmartPartPtr pDatumPart;
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_SPHERE)|GetSingleWord(SELECTINDEX_LINETUBE));
	verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
	CString cmdStr,valueStr;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate Please select circular plate's assembling positioning tube or sphere:");
#else
	cmdStr.Format("CircularPlate ��ѡ������װ�䶨λ�ֹܻ�����:");
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
	//ѡ���һ����λ�����ְ�|�����Ǹ�
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
		cmdStr.Format("CircularPlate ��ѡ�����Ķ�λ�����ְ�:");
	else
		cmdStr.Format("CircularPlate ��ѡ�����ĵ�һ�鶨λ�����ְ�������Ǹ�,��û��ֱ�ӻس�:");
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
		if(pDatumPart->GetClassTypeId()==CLS_SPHERE&&(pRelaPart1.IsNULL()||pRelaPart1->GetClassTypeId()==CLS_PARAMPLATE))	//�����ϵĻ������Ҫ�ж�λ��׼�ְ�
			continue;
		break;
	}
	long hPartWeldParent1=0,hPartWeldParent2=0;
	if(pRelaPart1.IsHasPtr())
	{
		g_pSolidDraw->SetEntSnapStatus(pRelaPart1->handle);
		if(pRelaPart1->IsPlate())
			pCmdLine->FinishCmdLine(CXhChar16("�ְ�0x%X",pRelaPart1->handle));
		else if(pRelaPart1->IsAngle())
			pCmdLine->FinishCmdLine(CXhChar16("�Ǹ�0x%X",pRelaPart1->handle));
		hPartWeldParent1=pRelaPart1->m_hPartWeldParent;
	}
	if(pDatumPart->IsTube()&&pRelaPart1.IsHasPtr()&&pRelaPart1->IsPlate())
	{	//ѡ��ڶ�����λ��������
		if(pRelaPart1->GetClassTypeId()==CLS_PLATE||(pRelaPart1->GetClassTypeId()==CLS_PARAMPLATE&&!pRelaPart1.ParamPlatePointer()->IsFL()))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("CircularPlate Please select the second location datum plate(or tube) of the circular plate, if no, press enter directly:");
#else
			cmdStr.Format("CircularPlate ��ѡ�����ĵڶ��鶨λ��׼�ְ�(��ֹ�),��û��ֱ�ӻس�:");
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
					cmdStr.Format("CircularPlate ��ѡ�������һ��ְ庸�Ӹ�������ͬ��������ѡ�����Ķ�λ��׼�ְ�(��ֹ�):");
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
	{	//ѡ�񻷰�������˨
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("CircularPlate please select all the relative bolts of circular plate:","");
#else
		pCmdLine->FillCmdLine("CircularPlate ��ѡ����������˨:","");
#endif
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
		OpenWndSel();	//����ѡ����˨
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
		OnOperOther();	//ȡ����������״̬
	}
	//ѡ�񻷰��׼�ڵ�
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
		cmdStr.Format("CircularPlate ��ѡ��ֹ��ϵ�װ�䶨λ�ڵ�:");
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
		pCmdLine->FinishCmdLine(CXhChar16("�ڵ�0x%X",pDatumNode->handle));
	}
	//ѡ������������
	int iPlateType=pRelaPart1.IsHasPtr()&&pRelaPart1->IsPlate()?2:0;	//0.��ͨ����� 1.��� 2.�����߰� 3.��ˮ�� 4.�ߵ���
	if(pRelaPart1.IsHasPtr()&&!pRelaPart2.IsHasPtr()&&pRelaPart1->GetClassTypeId()==CLS_PARAMPLATE&&pRelaPart1.ParamPlatePointer()->IsFL())
		iPlateType=3;	//ѡ�з������������ˮ��
	else if(pRelaPart1.IsHasPtr()&&pRelaPart2.IsNULL()&&pRelaPart1->GetClassTypeId()==CLS_PARAMPLATE
		&&pRelaPart1.ParamPlatePointer()->m_iParamType==TYPE_TUBESTAYWIREPLATE)
		iPlateType=1;	//ѡ�����߰�������ƼӾ����
	else if(pRelaPart1.IsHasPtr()&&pRelaPart1->GetClassTypeId()==CLS_LINEANGLE)
		iPlateType=4;
	/*���������١���ΰ��Ӧ�����������ֱ��ͨ���Ի��������Ƹ��ӷ���
			����Ҫͨ������������������зֲ����  wxc--2017.6.15
	*/
	CUndoOperObject undo(&Ta,true);
	CLDSParamPlate* pCirPlate=NULL;
	CDesignCircularPlateDlg cirPlateDlg;
	if(iPlateType==0)
		cirPlateDlg.m_iPlateType=0;	//��ͨ�����
	else if(iPlateType==1)
		cirPlateDlg.idPlateType=CDesignCircularPlateDlg::TYPE_COVERPLATE2;	//������
	else if(iPlateType==3)
		cirPlateDlg.idPlateType=CDesignCircularPlateDlg::TYPE_SHIELD_WATER;	//��ˮ��
	else if(iPlateType==4)
		cirPlateDlg.idPlateType=CDesignCircularPlateDlg::TYPE_AISLE_PLATE;	//�ߵ���
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
	pCmdLine->FillCmdLine("����:","");
#endif
#endif
	return 0;
}
//��ͨ�����
CLDSParamPlate* CLDSView::DesignSimpleCirRibPlate(CLDSLineTube* pDatumTube,CLDSNode* pDatumNode)
{
#ifdef __COMPLEX_PART_INC_
	if(pDatumTube==NULL || pDatumNode==NULL)
		return NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//���û�����Ĭ�ϲ���������ʵ��
	CDesignCircularPlateDlg cirPlateDlg;
	cirPlateDlg.m_pDatumNode=pDatumNode;
	cirPlateDlg.m_pDatumTube=pDatumTube;
	cirPlateDlg.m_iPlateType=0;
	cirPlateDlg.para.A=0;	//��ʼ�Ƕ�
	cirPlateDlg.para.V=1;	//�ӹ���϶
	cirPlateDlg.InitCirPlatePara();
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(cirPlateDlg.m_pCirPlate->handle);
	//��ʾʾ��ͼ
	CLDSView::PROMPT_LIFE lifeprompt;
	lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_PLATE);
	CString cmdStr,valueStr;
	//�������μнǦ�
	double angle=cirPlateDlg.para.B;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's offset Angle<%.f>:",angle);
#else
	cmdStr.Format("CircularPlate �����뻷�������μнǦ�<%.f>:",angle);
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
	//�����ʼƫ�ƽǶ�
	if(cirPlateDlg.para.B<360)
	{	
		cirPlateDlg.WorkPlaneSketch(1);
		angle=cirPlateDlg.para.A;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input circulaer plate's offset Angle<0>:");
#else
		cmdStr.Format("CircularPlate �����뻷���Ķ�λƫת�Ǧ�<0>:");
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
	//����������ƫ����
	cirPlateDlg.WorkPlaneSketch(2);
	double S=cirPlateDlg.para.S;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's offset distance<%.0f>:",S);
#else
	cmdStr.Format("CircularPlate ��������ͨ����������ƫ����<%.0f>:",S);
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
	//�����⻷�뾶
	double R=cirPlateDlg.para.R;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's R<%.1f>:",R);
#else
	cmdStr.Format("CircularPlate ��������ͨ�������⻷�뾶<%.1f>:",R);
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
	//��ʾ�����Ի���
	if(cirPlateDlg.DoModal()==IDOK)
		return cirPlateDlg.m_pCirPlate;
#endif
	return NULL;
}
//��ˮ��
CLDSParamPlate* CLDSView::DesignWaterShieldedPlate(CLDSLineTube* pDatumTube,CLDSNode* pDatumNode,CLDSParamPlate* pFl)
{
#ifdef __COMPLEX_PART_INC_
	if(pDatumTube==NULL || pDatumNode==NULL ||pFl==NULL)
		return NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//���û�����Ĭ�ϲ���������ʵ��
	CDesignCircularPlateDlg cirPlateDlg;
	cirPlateDlg.m_pDatumNode=pDatumNode;
	cirPlateDlg.m_pDatumTube=pDatumTube;
	cirPlateDlg.m_pRelaPart1=pFl;
	cirPlateDlg.m_iPlateType=6;
	cirPlateDlg.InitCirPlatePara();
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(cirPlateDlg.m_pCirPlate->handle);
	//��ʾʾ��ͼ
	CLDSView::PROMPT_LIFE lifeprompt;
	lifeprompt.SetPromptPicture(IDB_BMP_SHIELDEDWATER_PLATE);
	CString cmdStr,valueStr;
	//ȷ����λ�ǻ�׼��
	cirPlateDlg.WorkPlaneSketch(1);
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate select circular plate's type:[0.base-line|1.weld-line]<0>:");
#else
	cmdStr.Format("CircularPlate ��ѡ����ˮ��Ķ�λ��׼��[0.��׼��|1.������]<0>:");
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
	//���뷨��ƫ����
	cirPlateDlg.WorkPlaneSketch(3);
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's offset distance<%.0f>:",cirPlateDlg.water.S);
#else
	cmdStr.Format("CircularPlate ��������ˮ��ķ���ƫ�ƾ���<%.0f>:",cirPlateDlg.water.S);
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
	//�����⻷ֱ��
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's Diameter<%.0f>:",cirPlateDlg.water.W);
#else
	cmdStr.Format("CircularPlate ��������ˮ����⻷ֱ��<%.0f>:",cirPlateDlg.water.W);
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
	//��������ˮ��֮��ļн�
	double angle=cirPlateDlg.water.A;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's angle<%.0f>:",angle);
#else
	cmdStr.Format("CircularPlate ��������ˮ�������ˮ�۵ļн�<%.0f>:",angle);
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
	if(cirPlateDlg.water.A>0)	//����ˮ�ۼн�>0,��ʾ����ˮ��
	{	//����ˮ�۽�ƽ�������׼�ߵ�ƫ�ƽ�
		angle=cirPlateDlg.water.O;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input water-slot offet angle<%.0f>:",angle);
#else
		cmdStr.Format("CircularPlate ��������ˮ�۽�ƽ�������׼�ߵ�ƫ�ƽ�<%.0f>:",angle);
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
		//������ˮ�۳��ȡ���ȡ����
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input water slot's Length(L),Width(W),Heigh(H)<%.f,%.f>:",cirPlateDlg.water.L,cirPlateDlg.water.K,cirPlateDlg.water.H);
#else
		cmdStr.Format("CircularPlate ������ˮ�۵ĳ�(L),��(W),��(H)<%.f,%.f,%.f>:",cirPlateDlg.water.L,cirPlateDlg.water.K,cirPlateDlg.water.H);
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
	//��ʾ�����Ի���
	if(cirPlateDlg.DoModal()==IDOK)
		return cirPlateDlg.m_pCirPlate;
#endif
	return NULL;
}
//������
CLDSParamPlate* CLDSView::DesignCircularCoverPlate(CLDSLineTube* pDatumTube,CLDSNode* pDatumNode,CLDSPlate* pRelaPlate1,CLDSPlate* pRelaPlate2/*=NULL*/)
{
#ifdef __COMPLEX_PART_INC_
	if(pDatumTube==NULL || pDatumNode==NULL ||pRelaPlate1==NULL)
		return NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//���û�����Ĭ�ϲ���������ʵ��
	CDesignCircularPlateDlg cirPlateDlg;
	cirPlateDlg.m_pDatumNode=pDatumNode;
	cirPlateDlg.m_pDatumTube=pDatumTube;
	cirPlateDlg.m_pRelaPart1=pRelaPlate1;
	cirPlateDlg.m_pRelaPart2=pRelaPlate2;
	cirPlateDlg.m_iPlateType=1;
	cirPlateDlg.para.S=1;		//Ĭ������Ϸ��
	cirPlateDlg.InitCirPlatePara();
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(cirPlateDlg.m_pCirPlate->handle);
	//��ʾʾ��ͼ
	CLDSView::PROMPT_LIFE lifeprompt;
	lifeprompt.SetPromptPicture((cirPlateDlg.para.nCirCoverPlateType==0)?IDB_BMP_CIRCULAR_COVERPLATE2:IDB_BMP_CIRCULAR_COVERPLATE3);
	CString cmdStr,valueStr;
	//ȷ���������
	if(pRelaPlate2==NULL)
	{	//�򻯷��
		cirPlateDlg.WorkPlaneSketch(5);
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input circulaer plate's position[0.Common|1.Y+|2.Y-]<0>:");
#else
		cmdStr.Format("CircularPlate ��ȷ���򻯷�������[0.��ͨ|1.Y+��|2.Y-��]<0>:");
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
					cirPlateDlg.para.nCirCoverPlateType=1;	//Y+��
				else
					cirPlateDlg.para.nCirCoverPlateType=-1;	//Y-��
				break;
			}
		}
		cirPlateDlg.DelWorkPlaneSketch();
		cirPlateDlg.ModifyPlateParam('Y',cirPlateDlg.para.nCirCoverPlateType);
		lifeprompt.SetPromptPicture((cirPlateDlg.para.nCirCoverPlateType==0)?IDB_BMP_CIRCULAR_COVERPLATE2:IDB_BMP_CIRCULAR_COVERPLATE3);
	}
	//���������μн�
	double angle=cirPlateDlg.para.B;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's angle<%.0f>:",angle);
#else
	cmdStr.Format("CircularPlate �����뻷��������μн�<%.0f>:",angle);
#endif
	while(true)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(!pCmdLine->GetStrFromCmdLine(valueStr))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		//���������νǱ�����0~360֮��,����Ϊ��Ч����
		if(valueStr.GetLength()==0 ||(CmdStrToNumber(valueStr,&angle)&&angle>0&&angle<360))
		{
			cirPlateDlg.para.B=angle;
			break;
		}
	}
	cirPlateDlg.ModifyPlateParam('B',cirPlateDlg.para.B);
	if(cirPlateDlg.para.nCirCoverPlateType==0)
	{	//������Ķ�λ��ת��
		cirPlateDlg.WorkPlaneSketch(4);
		angle=cirPlateDlg.para.A;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input circulaer plate's offset angle<%.0f>:",angle);
#else
		cmdStr.Format("CircularPlate �����뻷����Ķ�λƫת��<%.0f>:",angle);
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
		//������Ĳ���N
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input circulaer plate's shape param N<%.0f>:",cirPlateDlg.para.N);
#else
		cmdStr.Format("CircularPlate �����뻷��������β���N<%.0f>:",cirPlateDlg.para.N);
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
	//�������̨�ڸ߶�H�ͻ��δ���
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's shape param H,param W<%.0f,%.0f>:",cirPlateDlg.para.H,cirPlateDlg.para.W);
#else
	cmdStr.Format("CircularPlate �����뻷�����̨�ڸ߶�(H),���ο��(W)<%.0f,%.0f>:",cirPlateDlg.para.H,cirPlateDlg.para.W);
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
	//�Ƿ�����·��
	int iType=0;
#ifdef AFX_TARG_ENU_ENGLISH
	if(cirPlateDlg.para.nCirCoverPlateType==0)
		cmdStr.Format("CircularPlate select relate plate design mode:[0.No|1.Down|2.Mir|3.Down & Mir]<0>:");
	else
		cmdStr.Format("CircularPlate select relate plate design mode:[0.No|1.Down]<0>:");
#else
	if(cirPlateDlg.para.nCirCoverPlateType==0)
		cmdStr.Format("CircularPlate ��ѡ�����������ģʽ:[0.�޹���|1.�²���|2.�Բ���|3.�²༰�Բ���]<0>:");
	else
		cmdStr.Format("CircularPlate ��ѡ�����������ģʽ:[0.�޹���|1.�²���]<0>:");
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
	{	//������·��
		cirPlateDlg.ClearCirPlate();
		cirPlateDlg.m_bDisplayDlg=FALSE;
		cirPlateDlg.para.S=(iType==1||iType==3)?0:1;
		cirPlateDlg.para.bCreateMirPlate=(iType>1)?TRUE:FALSE;
		cirPlateDlg.m_pCirPlate=cirPlateDlg.CreateCircularPlate();
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(cirPlateDlg.m_pCirPlate->handle);
		return cirPlateDlg.m_pCirPlate;
	}
	//��ʾ�����Ի���
	if(cirPlateDlg.DoModal()==IDOK)
		return cirPlateDlg.m_pCirPlate;
#endif
	return NULL;
}
//2WL�߰�|2WA�߰�|3WL�߰�|3WA�߰�
CLDSParamPlate* CLDSView::DesignCircularRibPlate(SmartPartPtr pDatumPart,CLDSNode* pDatumNode,CLDSGeneralPlate* pRelaPlate,SmartPartPtr pRelaPart2)
{
#ifdef __COMPLEX_PART_INC_
	if(pDatumPart.IsNULL() || pDatumNode==NULL ||pRelaPlate==NULL)
		return NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//���û�����Ĭ�ϲ���������ʵ��
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
		{	//��������Ϊ��ͨ����
			cirPlateDlg.m_iPlateType=3;			//3WA�߰�
			cirPlateDlg.para.nDHRelativeType=1;	//D��HΪ�������� ����Խ����
		}
		else
		{
			cirPlateDlg.m_iPlateType=4;		//2WL�߰�
			cirPlateDlg.para.D=0;			//ͨ�����μн�ȷ������
		}
	}
	else if(pRelaPart2->GetClassTypeId()==CLS_LINETUBE)
	{
		cirPlateDlg.m_iPlateType=3;			//3WA�߰�
		cirPlateDlg.para.nDHRelativeType=1; //D��HΪ�������� ����Խ����
	}
	else 
	{
		cirPlateDlg.m_iPlateType=2;			//3WL�߰�
		cirPlateDlg.para.nDHRelativeType=2; //����H����
	}
	CString cmdStr,valueStr;
	if(cirPlateDlg.m_iPlateType==2 || cirPlateDlg.m_iPlateType==4)
	{	//ѡ�����ɱߵ����ͣ�0.ֱ��|1.Բ��
		int iEdgeType=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate select circulaer plate's free edge type[0.Line|1.Arc]<0>:");
#else
		cmdStr.Format("CircularPlate ѡ�����߰����ɱߵ�����[0.ֱ��|1.Բ��]<0>:");
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
	//��ʾʾ��ͼ
	CLDSView::PROMPT_LIFE lifeprompt;
	if(cirPlateDlg.m_iPlateType==2)	//3WL�߰�
		lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_RIBPLATE1);
	else if(cirPlateDlg.m_iPlateType==3)	//3WA�߰�
		lifeprompt.SetPromptPicture((cirPlateDlg.para.nDHRelativeType==0)?IDB_BMP_CIRCULAR_RIBPLATE2:IDB_BMP_CIRCULAR_RIBPLATE5);
	else if(cirPlateDlg.m_iPlateType==4)	//2WL�߰�
		lifeprompt.SetPromptPicture((cirPlateDlg.para.D>0)?IDB_BMP_CIRCULAR_RIBPLATE3:IDB_BMP_CIRCULAR_RIBPLATE7);
	else if(cirPlateDlg.m_iPlateType==5)	//2WA�߰�
		lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_RIBPLATE4);
	//
	if(cirPlateDlg.m_iPlateType==3&&cirPlateDlg.para.nDHRelativeType==0)
	{	//���뻷���߰���⻷�뾶
		cirPlateDlg.para.R;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input circulaer plate's outer-ring radius<%.1f>:",cirPlateDlg.para.R);
#else
		cmdStr.Format("CircularPlate �����뻷������⻷�뾶<%.1f>:",cirPlateDlg.para.R);
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
	{	//���뺸�쳤��W��̨�ڸ߶�
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CircularPlate input relative plate's weld length<%.f>,front height(H) <%.0f,%.0f>:",cirPlateDlg.para.W,cirPlateDlg.para.H);
#else
		cmdStr.Format("CircularPlate ���뻷���߰�������ְ�ĺ��쳤��(W)��̨�ڸ߶�(H) <%.0f,%.0f>:",cirPlateDlg.para.W,cirPlateDlg.para.H);
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
		{//���뻷�δ���D
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("CircularPlate input circulaer plate's outer wall distance<%.1f>:",cirPlateDlg.para.D);
#else
			cmdStr.Format("CircularPlate �����뻷����Ļ��δ���D<%.1f>:",cirPlateDlg.para.D);
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
		{	//�������νǦ�
			double angle=cirPlateDlg.para.B;
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("CircularPlate input circulaer plate's angle<%.0f>:",angle);
#else
			cmdStr.Format("CircularPlate �����뻷���߰�����μнǦ�<%.0f>:",angle);
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
		{	//���ǽǶȦ�
			double angle=cirPlateDlg.para.gama;
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("CircularPlate input circulaer plate's outer wall distance<%.0f>:",angle);
#else
			cmdStr.Format("CircularPlate �����뻷���߰�ĵ��ǽǶȦ�<%.0f>:",angle);
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
	//����������ƫ����
	cirPlateDlg.WorkPlaneSketch(2);
	double S=cirPlateDlg.para.S;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CircularPlate input circulaer plate's offset distance<%.0f>:",S);
#else
	cmdStr.Format("CircularPlate �����뻷���߰������ƫ����<%.0f>:",S);
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
	//��ʾ�����Ի���
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
		return FALSE; //��֤��ѡ�ְ���ڻ�׼�ڵ����׼�ֹ�
	if(n==2&&(pBaseNode->handle!=pDatumPlate2->designInfo.m_hBaseNode
		||pBaseTube->handle!=pDatumPlate2->designInfo.m_hBasePart))
		return FALSE;	//��֤��ѡ������ڵ���׼�ڵ����׼�˼�����ͬ 
	//
	CDesignCircularPlateDlg cirPlateDlg;
	cirPlateDlg.m_pDatumNode=pBaseNode;			//��׼�ڵ�
	cirPlateDlg.m_pDatumTube=pBaseTube;			//��׼�ֹ�
	if(pDatumPlate1)
		cirPlateDlg.m_pRelaPart1=pDatumPlate1;	//�����ְ�1
	if(pDatumPlate2)
		cirPlateDlg.m_pRelaPart2=pDatumPlate2;	//�����ְ�2
	if(plate_type==0)
		cirPlateDlg.m_iPlateType=1;		//���
	else if(plate_type==1)
	{
		if(pDatumPlate1&&pDatumPlate2)
		{
			cirPlateDlg.m_iPlateType=2;	//3WL�߰�
			cirPlateDlg.para.nDHRelativeType=2; //���ò���H
		}
		else if(pDatumPlate1)
			cirPlateDlg.m_iPlateType=4;	//2WL�߰�
	}
	else //if(plate_type==2)
	{
		if(pDatumPlate1&&pDatumPlate2)
		{
			cirPlateDlg.m_iPlateType=3;	//3WA�߰�
			cirPlateDlg.para.nDHRelativeType=1;
		}
	}
	if(cirPlateDlg.DoModal()==IDOK)
		cirPlateDlg.CreateCircularPlate();
	/*
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr;
	//���뻷���ӹ���϶
	double manu_space=0;
	cmdStr.Format("�����뻷���ӹ���϶<0>:");
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
			cmdStr.Format("������������뻷���ӹ���϶<0>:");
			pCmdLine->FillCmdLine(cmdStr,"");
			continue;
		}
		break;
	}
	g_pSolidDraw->ReleaseSnapStatus();
	pCmdLine->FillCmdLine("����:","");
	pCmdLine->FinishCmdLine();
	if(plate_type==0)		//������ɻ�����
		return FinishDesignCirCoverPlate(pBaseNode,pBaseTube,pDatumPlate1,pDatumPlate2,manu_space);
	else if(plate_type==1)	//������ɻ����߰�
		return FinishDesignCirRibPlate(pBaseTube,pBaseNode,pDatumPlate1,pDatumPlate2,manu_space);
	else if(plate_type==2)	//������ɸֹ��߰�
	{
		CLDSLinePart *pAxisXRefLinePart=NULL;
		pCmdLine->FillCmdLine("��ѡ��ȷ�ϻ����X�᷽��Ĳο��˼�,��û�л�����X�᷽�����׼�ֹܵ�����ֱ:","");
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
				pCmdLine->FillCmdLine("û��ѡ�к��ʵĹ�������ѡ�����X�᷽��Ĳο��˼�:","");
				g_pSolidDraw->ReleaseSnapStatus();
				continue;
			}
		}
		g_pSolidDraw->ReleaseSnapStatus();
		pCmdLine->FillCmdLine("����:","");
		pCmdLine->FinishCmdLine();
		return FinishDesignTubeRibPlate(pBaseNode,pBaseTube,pDatumPlate1,pDatumPlate2,pAxisXRefLinePart,manu_space);
	}*/
#endif
	return FALSE;
}

//�������ɻ�����
void CLDSView::OnFastCirCoverPlate()
{
	FastCirPlate(0);	//������ɻ�����
}

//�������ɻ����߰�
void CLDSView::OnFastCirRibPlate()
{
	FastCirPlate(1);	//������ɻ����߰�
}

//�������ɸֹ��߰�
void CLDSView::OnFastTubeRibPlate()
{
	FastCirPlate(2);	//������ɸֹ��߰�
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
		AfxMessageBox("welding parent rod of the selected plate is not tube, error in data��");
#else
		AfxMessageBox("��ѡ�иְ�ĺ��Ӹ��������Ǹֹܣ���������");
#endif
		return FALSE;
	}
	//��ʾ��ֱ�������ϵ���������ڲ��/����
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
	workPlaneSketchLife.WorkPlaneSketch(sketch,sketch.normal,"���",100);
	workPlaneSketchLife.WorkPlaneSketch(sketchAnti,sketchAnti.normal,"�ڲ�",100);
	//������׸ְ���˫��壬���ж���ˮ��λ���ĸ���(����|������)
	f3dPoint hori_face_pick=pHoriPlate->ucs.origin;
	f3dPoint hori_face_norm=pHoriPlate->ucs.axis_z;
	f3dPoint huoqu_norm=pHoriPlate.GeneralPlatePointer()->GetHuoquFaceNorm(0);
	int iPlane=0;
	if(!huoqu_norm.IsZero())
	{	//���ݺ��ӱ��뺸�Ӹְ�Ĺ�ϵ��ȷ��λ���ĸ���
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
			{	//�ñ����뵱ǰ���׼�淨�ߴ�ֱ
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
	if(D<=0)	//��ؼ����ǿ׾�������������˨ֱ����wjh-2018.1.29
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
		pHoriPlate.pParamPlate->SetDesignItemValue(KEY2C("BH"),D);	//��ˮ��ֱ��,BH=0��ʾ��������ˮ��
		if(pHoriPlate.pParamPlate->GetDesignItemValue('F',&hPart) && hPart==pVertPlate->handle)
			pHoriPlate.pParamPlate->SetDesignItem('F',param,"",PARAM_POS);
		else if(pHoriPlate.pParamPlate->GetDesignItemValue('G',&hPart) && hPart==pVertPlate->handle)
			pHoriPlate.pParamPlate->SetDesignItem('G',param,"",PARAM_POS);
		else
		{	//�����ǰ�
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
				{	//�Ѵ��ڣ�ֱ�Ӹ������ֵ
					idle=-1;
					pHoriPlate.pParamPlate->SetDesignItem(KEY2C(keystr),param,"",PARAM_POS);
					break;
				}
			}
			if(idle>=0&&idle<=3)
			{	//��Ӹ��ǰ������Ϣ
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
		return TRUE;	//�������ϵ���ˮ����תΪ�����CLDSPart::HOLE������CLDSParamPlate::DesignPlateʱ��ơ�wjh-2018.1.28
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
		pHoriPlate.pParamPlate->SetDesignItemValue(KEY2C("BH"),D);	//��ˮ��ֱ��,BH=0��ʾ��������ˮ��
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
	{	//���㹤�տ״���λ��(������������h),�ж��Ƿ���ˮƽ�ְ��� wht 15-12-18
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
	
	//�������ݵ�S��H��������ڽ��߷�����ƶ��� wht 10-08-25
	double tube_r=pDatumTube->GetDiameter()*0.5;
	double xie_bian=tube_r+D*0.5+h;
	double zhijiao_bian=D*0.5+s+nVertPlateThick*0.5;
	if(gyHoleDlg.m_bIncludeHoleR)
	{	//����S��H�����װ뾶 wht 10-08-25
		xie_bian=tube_r+h;
		zhijiao_bian=s+nVertPlateThick*0.5;
	}
	double offset_dist=sqrt(xie_bian*xie_bian-zhijiao_bian*zhijiao_bian);
	if(bUp)
	{
		pBolt = (CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->m_bVirtualPart=TRUE;
		pBolt->set_d(d);
		pBolt->CopyModuleInstanceInfo(pHoriPlate);	//������˨��ĺ����׼������ĺ�һ��
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
		pBolt->CopyModuleInstanceInfo(pHoriPlate);	//������˨��ĺ����׼������ĺ�һ��
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
{	//������
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
		coverPlateDlg.m_iPlateType=1;	//������
		coverPlateDlg.m_pDatumNode=pBaseNode;
		coverPlateDlg.m_pDatumTube=pDatumTube;
		coverPlateDlg.m_pRelaPart1=pPlate1;
		coverPlateDlg.m_pRelaPart2=pPlate2;
		if(coverPlateDlg.DoModal()==IDOK)
		//if(coverPlateDlg.DoModal()==IDOK)
		{
			if(coverPlateDlg.para.S==0	//���������ʱ,�ڴ˴�����Ϸ��
				||coverPlateDlg.para.S>0||coverPlateDlg.para.S<0)	//���·��ֻ���һ��ʱ,�ڴ˴����;
			{	
				pCoverPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
				pCoverPlate->m_iParamType = TYPE_CIRCOVERPLATE;
				pCoverPlate->SetPartNo(coverPlateDlg.m_sPartNo);	//���
				pCoverPlate->iSeg = SEGI(coverPlateDlg.m_sSeg.GetBuffer());	//�κ�
				pCoverPlate->CopyModuleInstanceInfo(pDatumTube);	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
				pCoverPlate->cMaterial = coverPlateDlg.m_cMaterial;//steelmat_tbl[coverPlateDlg.m_iMaterial].cBriefMark;	//����
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
				pCoverPlate->SetDesignItemValue('A',coverPlateDlg.para.A,"�����ת�Ƕ�");
				pCoverPlate->SetDesignItemValue('B',coverPlateDlg.para.B,"���μн�",PARAM_SHAPE);
				pCoverPlate->SetDesignItemValue('C',pDatumTube->handle,"��λ��׼��׼�ֹ�",PARAM_POS,1);
				pCoverPlate->SetDesignItemValue('E',pBaseNode->handle,"��λ��׼�ڵ�",PARAM_POS,1);
				pCoverPlate->SetDesignItemValue('F',pFirstDatumPlate->handle,"��һ���׼�ְ�",PARAM_POS,1);
				if(pSecondDatumPlate.IsNULL())
					pCoverPlate->SetDesignItemValue('G',0,"�ڶ����׼�ְ�",PARAM_POS,1);
				else
					pCoverPlate->SetDesignItemValue('G',pSecondDatumPlate->handle,"�ڶ����׼�ְ�",PARAM_POS,1);
				pCoverPlate->SetDesignItemValue('D',coverPlateDlg.para.H,"���������H",PARAM_SHAPE);
				pCoverPlate->SetDesignItemValue('W',coverPlateDlg.para.W,"���������W",PARAM_SHAPE);
				if(coverPlateDlg.para.S>=0)
					pCoverPlate->SetDesignItemValue('S',-1,"����Ϸ��");
				else 
					pCoverPlate->SetDesignItemValue('S',1,"����·��");
				pCoverPlate->SetDesignItemValue('R',pDatumTube->GetDiameter()*0.5+manu_space,"�������Բ���뾶(=�ֹܰ뾶+�ӹ���϶)",PARAM_SHAPE);
#endif
				//if(coverPlateDlg.m_bMirSide)
				//	pCoverPlate->SetDesignItemValue('M',(double)-1,"MΪ-1ʱ�ڹ�ͨ��������һ��ӷ��");
				//else
#ifdef AFX_TARG_ENU_ENGLISH
					pCoverPlate->SetDesignItemValue('M',(double)1,"when M is -1, add sealing plate on the other side of  through board");
#else
					pCoverPlate->SetDesignItemValue('M',(double)1,"MΪ-1ʱ�ڹ�ͨ��������һ��ӷ��");
#endif
				pCoverPlate->DesignPlate();
				pCoverPlate->SetModified();
				pCoverPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pCoverPlate->GetSolidPartObject());
				g_pSolidDraw->Draw();
			}
			if(coverPlateDlg.para.S==0)
			{	//���·�һ�����ʱ,�ڴ˴�����·��
				pCoverPlate1=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
				pCoverPlate1->CopyProperty(pCoverPlate);
				pCoverPlate1->SetLayer(pDatumTube->layer()); //����ͼ����
				pCoverPlate1->cfgword=pDatumTube->cfgword;
#ifdef AFX_TARG_ENU_ENGLISH
				pCoverPlate1->SetDesignItemValue('S',(double)1,"design down sealing plate");
#else
				pCoverPlate1->SetDesignItemValue('S',(double)1,"����·��");
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
			if(bCreatHole)	//�������տ�
				CreateProcessHole(pCoverPlate1,pFirstDatumPlate,FALSE);
			if(pSecondDatumPlate.IsHasPtr()&&!pFirstDatumPlate.IsEqualPtr(pSecondDatumPlate))
			{
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(pSecondDatumPlate->handle);
				if(bCreatHole)
				{	//�������տ�
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
		MessageBox("��ѡ�ְ��޺��Ӹ�����,��������ƻ����������,����ж�!");
#endif
#endif
	return FALSE;
}

BOOL CLDSView::FinishDesignCirRibPlate(CLDSPart *pDatumPart,CLDSNode *pBaseNode,CLDSPlate *pFirstDatumPlate,
									   CLDSPlate *pSecondDatumPlate,double manu_space/*=0*/)
{	//1.��+�ֹܼа�		
	//2.�ֹ�+�ڵ�+һ���׼�ְ�
	//3.�ֹ�+�ڵ�+�����׼�ְ�
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
	{	//�ֹ�+�ڵ�+һ���׼�ְ�������׼�ְ�
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
			pCirRibPlate->SetPartNo(cirRibDlg.m_sPartNo.GetBuffer(0));	//���
			pCirRibPlate->iSeg = SEGI(cirRibDlg.m_sSegI.GetBuffer());	//�κ�
			pCirRibPlate->SetLayer(pDatumTube->layer());	//����������ͼ����
			pCirRibPlate->cfgword=pDatumTube->cfgword;
			pCirRibPlate->cMaterial = CSteelMatLibrary::RecordAt(cirRibDlg.m_iMaterial).cBriefMark;	//����
			pCirRibPlate->thick = (long)cirRibDlg.m_fPlateT;
			pCirRibPlate->m_hPartWeldParent = pDatumTube->handle;
			pCirRibPlate->cfgword=pDatumTube->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
#ifdef AFX_TARG_ENU_ENGLISH
			pCirRibPlate->SetDesignItemValue('A',pBaseNode->handle,"location datum node",PARAM_POS,1);
			pCirRibPlate->SetDesignItemValue('B',pFirstDatumPlate->handle,"the first datum plate",PARAM_POS,1);
#else
			pCirRibPlate->SetDesignItemValue('A',pBaseNode->handle,"��λ��׼�ڵ�",PARAM_POS,1);
			pCirRibPlate->SetDesignItemValue('B',pFirstDatumPlate->handle,"��һ���׼�ְ�",PARAM_POS,1);
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
				pCirRibPlate->SetDesignItemValue('C',pSecondDatumPlate->handle,"�ڶ����׼�ְ�",PARAM_POS,1);
				pCirRibPlate->SetDesignItemValue('W',cirRibDlg.m_fParaW,"�����߰����W",PARAM_SHAPE);
				pCirRibPlate->SetDesignItemValue('N',cirRibDlg.m_fParaN,"�����߰����N",PARAM_SHAPE);
				pCirRibPlate->SetDesignItemValue('D',cirRibDlg.m_fParaD,"�����߰����D,H��D����һ������ D<0��ʾ����H",PARAM_SHAPE);
				pCirRibPlate->SetDesignItemValue('H',cirRibDlg.m_fParaH,"�����߰����H,H��D����һ������ H<0��ʾ����D",PARAM_SHAPE);
#endif
				long O=-1;
				if(cirRibDlg.m_bCir)
					O=1;	//����Բ������
#ifdef AFX_TARG_ENU_ENGLISH
				pCirRibPlate->SetDesignItemValue('O',O,"positive value stands for arc connection on rib plate vertex; negative value stands for straight line connection");
#else
				pCirRibPlate->SetDesignItemValue('O',O,"��ֵ��ʾ�߰嶥��ΪԲ�����ӣ���ֵ��ʾ�߰嶥��Ϊֱ������");
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
				pCirRibPlate->SetDesignItemValue('C',0,"�ڶ����׼�ְ�",PARAM_POS,1);
				if(cirRibDlg.m_bDesignOuterPlate)
					Y=1;
				pCirRibPlate->SetDesignItemValue('Y',Y,"��ֵ�����ʾ�߰��ڻ�׼�ְ����࣬����λ���ڲ�");
				pCirRibPlate->SetDesignItemValue('W',cirRibDlg.m_fParaW,"�����߰����W",PARAM_SHAPE);
#endif
				long O=-1;
				if(cirRibDlg.m_bCir)
				{	//����Բ������
					O=1;	//����Բ������
#ifdef AFX_TARG_ENU_ENGLISH
					pCirRibPlate->SetDesignItemValue('D',cirRibDlg.m_fParaD,"circular rib plate arc degree corresponding to ��in map",PARAM_SHAPE);
					pCirRibPlate->SetDesignItemValue('N',cirRibDlg.m_fParaN,"circular rib plate parameter N",PARAM_SHAPE);
#else
					pCirRibPlate->SetDesignItemValue('D',cirRibDlg.m_fParaD,"�����߰�Բ��������Ӧʾ��ͼ�еĦ�",PARAM_SHAPE);
					pCirRibPlate->SetDesignItemValue('N',cirRibDlg.m_fParaN,"�����߰����N",PARAM_SHAPE);
#endif
				}
				else
				{	//������Բ������
#ifdef AFX_TARG_ENU_ENGLISH
					pCirRibPlate->SetDesignItemValue('D',cirRibDlg.m_fParaD,"circular rib plate parameter D",PARAM_SHAPE);
					pCirRibPlate->SetDesignItemValue('H',cirRibDlg.m_fParaH,"circular rib plate parameter H",PARAM_SHAPE);
				}
				pCirRibPlate->SetDesignItemValue('O',O,"positive value stands for arc connection on rib plate vertex; negative value stands for straight line connection");
			}
			
			pCirRibPlate->SetDesignItemValue('S',cirRibDlg.m_fParaS,"offset distance from circular plate to location node");
			pCirRibPlate->SetDesignItemValue('R',pDatumTube->GetDiameter()*0.5+manu_space,"circular plate inside arc radius(= tube radius + processing clearance)",PARAM_SHAPE);
#else
					pCirRibPlate->SetDesignItemValue('D',cirRibDlg.m_fParaD,"�����߰����D",PARAM_SHAPE);
					pCirRibPlate->SetDesignItemValue('H',cirRibDlg.m_fParaH,"�����߰����H",PARAM_SHAPE);
				}
				pCirRibPlate->SetDesignItemValue('O',O,"��ֵ��ʾ�߰嶥��ΪԲ�����ӣ���ֵ��ʾ�߰嶥��Ϊֱ������");
			}
			
			pCirRibPlate->SetDesignItemValue('S',cirRibDlg.m_fParaS,"����嵽��λ�ڵ��ƫ�ƾ���");
			pCirRibPlate->SetDesignItemValue('R',pDatumTube->GetDiameter()*0.5+manu_space,"�������Բ���뾶(=�ֹܰ뾶+�ӹ���϶)",PARAM_SHAPE);
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
				pMirPlate->SetDesignItemValue('Y',-Y,"��ֵ�����ʾ�߰��ڻ�׼�ְ����࣬����λ���ڲ�");
#endif
				pMirPlate->ucs.axis_z=pCirRibPlate->ucs.axis_z;
				pMirPlate->DesignPlate();
				pMirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pMirPlate->GetSolidPartObject());
			}
			g_pSolidDraw->Draw();
			//�жϻ�׼�ְ��Ƿ�Ϊ��ͨ���� wht 10-03-22
			if(pFirstDatumPlate&&pSecondDatumPlate)
			{
				if(pFirstDatumPlate->IsTransPlate()||pSecondDatumPlate->IsTransPlate())
				{	//��ͨ����
#ifdef AFX_TARG_ENU_ENGLISH
					if(AfxMessageBox("Whether to generate simultaneously rib plate on the other side of the through board(Y/N)?",MB_YESNO)==IDYES)
#else
					if(AfxMessageBox("�Ƿ�ͬʱ���ɹ�ͨ����һ����߰�(Y/N)?",MB_YESNO)==IDYES)
#endif
					{	//ͬʱ���ɹ�ͨ������һ����߰� 
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
						pMirPlate->SetDesignItemValue('B',pSecondDatumPlate->handle,"��һ���׼�ְ�",PARAM_POS,1);
						pMirPlate->SetDesignItemValue('C',pFirstDatumPlate->handle,"�ڶ����׼�ְ�",PARAM_POS,1);
						pMirPlate->SetDesignItemValue('W',cirRibDlg.m_fParaN,"�����߰����W",PARAM_SHAPE);
						pMirPlate->SetDesignItemValue('N',cirRibDlg.m_fParaW,"�����߰����N",PARAM_SHAPE);
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
	{	//��׼����Ϊ��ʱ����ѡ��װ����λ�ڵ�
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
			pCirRibPlate->SetPartNo(cirRibDlg.m_sPartNo.GetBuffer(0));	//���
			pCirRibPlate->iSeg = SEGI(cirRibDlg.m_sSegI.GetBuffer());	//�κ�
			pCirRibPlate->SetLayer(pDatumSphere->layer());	//����������ͼ����
			pCirRibPlate->cfgword=pDatumSphere->cfgword;
			pCirRibPlate->cMaterial = CSteelMatLibrary::RecordAt(cirRibDlg.m_iMaterial).cBriefMark;	//����
			pCirRibPlate->thick = (long)cirRibDlg.m_fPlateT;
			pCirRibPlate->m_hPartWeldParent = pDatumSphere->handle;
			pCirRibPlate->cfgword=pDatumTube->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
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
			pCirRibPlate->SetDesignItemValue('A',pBaseNode->handle,"��λ��׼�ڵ�",PARAM_POS,1);
			pCirRibPlate->SetDesignItemValue('B',pFirstDatumPlate->handle,"��һ���׼�ְ�",PARAM_POS,1);
			double Y=-1;
			pCirRibPlate->SetDesignItemValue('C',0,"�ڶ����׼�ְ�",PARAM_POS,1);
			if(cirRibDlg.m_bDesignOuterPlate)
				Y=1;
			pCirRibPlate->SetDesignItemValue('Y',Y,"��ֵ�����ʾ�߰��ڻ�׼�ְ����࣬����λ���ڲ�");
			pCirRibPlate->SetDesignItemValue('W',cirRibDlg.m_fParaW,"�����߰����W",PARAM_SHAPE);
			pCirRibPlate->SetDesignItemValue('D',cirRibDlg.m_fParaD,"�����߰����D",PARAM_SHAPE);
			pCirRibPlate->SetDesignItemValue('H',cirRibDlg.m_fParaH,"�����߰����H",PARAM_SHAPE);
			pCirRibPlate->SetDesignItemValue('S',cirRibDlg.m_fParaS,"����嵽��λ�ڵ��ƫ�ƾ���");
			pCirRibPlate->SetDesignItemValue('R',pDatumSphere->D*0.5,"�������Բ���뾶",PARAM_SHAPE);
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
				pMirPlate->SetDesignItemValue('Y',-Y,"��ֵ�����ʾ�߰��ڻ�׼�ְ����࣬����λ���ڲ�");
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
		MessageBox("��ѡ�ְ��޺��Ӹ�����,��������ƻ����߰������,����ж�!");
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
	m_sPrevCommandName="�ظ����ʽ�ֹܽ�������";
#endif
	Command("InsertPlateTubeInters");
}

CXInsertPlateIntersTubeDlg inters_tube_dlg;

//���øֹܲ���ϵ���˨�͸��Ӱ�
BOOL LayoutInsertPlateBoltAndAttachPlate(CLDSParamPlate *pParamPlate,char* sPartNoP,
										 char* sPartNoX,BOOL bUseLineAngle,char *sJgGuige)
{
	//if(nBoltNum%2)	//��֤��˨����Ϊż��
	//	nBoltNum+=1;
	double A=0,N=0,M=0,R=0,L=0,H=0,S=0,W=0;
	if(pParamPlate->m_bStdPart)
	{	
		CLDSStdPartModel stdPartModel;
		if(!pParamPlate->GetStdPartModel(&stdPartModel))
			return FALSE;
		//���ɱ�׼�����ϵ���˨
		stdPartModel.CreateBoltInPlate(pParamPlate);
		//���Ʊ�׼�����ϵ���˨
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
		//���ò���ϵ���˨
		inters_tube_dlg.CreateBolt(pParamPlate);
	}
	
	double x=A+M;
	double y=R;
	double z=0.5*pParamPlate->GetThick();
	int i;
	CLDSPlate *pUpVertPlate=NULL,*pDownVertPlate=NULL; //Z����������  Z����������
	if(pParamPlate->m_iParamType==TYPE_XEND)
	{	//����ʮ�ְ��ϵĸ��Ӱ岢�Ҽ��뵽���ӹ����б���
		CString sPartNo;
		//ʮ�ֲ����Ҫ��������
		//ʮ�ֲ�����巨����ƻ�׼����Ϊ����			//wht 10-03-06
		//X-Yƽ���ϵİ��弰��˨������ƻ�׼����Ϊ����
		//X-Zƽ���ϵİ��弰��˨������ƻ�׼����Ϊ��Ӧ������,���巨���޸ĺ���弰��˨�����ܼ�ʱ����
		for(i=0;i<2;i++)
		{
			CLDSPlate *pVertPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
			pVertPlate->iSeg=pParamPlate->iSeg;
			pVertPlate->SetLayer(pParamPlate->layer()); //���������ͼ����
			pVertPlate->cfgword=pParamPlate->cfgword;
			pVertPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
			pVertPlate->cMaterial = pParamPlate->cMaterial;
			pVertPlate->m_hPartWeldParent=pParamPlate->handle;//���ݸ������Ĳ�����ư���
			pVertPlate->Thick=ftoi(pParamPlate->GetThick());
			pVertPlate->SetPartNo(sPartNoP);
			pVertPlate->jdb_style=5;	//��׼���������λ�ò���ĸ��Ӱ�
			//.................................................
			//�˴������⾡����ȥʹ��jdb_style �Ժ��޸� 
			//.................................................
			pVertPlate->designInfo.m_hBasePart=pParamPlate->handle;
			pVertPlate->designInfo.norm.norm_style=4;	//��׼�����ϵ���Է���
			pVertPlate->designInfo.norm.hVicePart=pParamPlate->handle;
			pVertPlate->designInfo.origin.datum_pos_style=9;//��׼�����ϵ��������
			pVertPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
			pVertPlate->m_fNormOffset=-z;
			if(i==0)
			{	//Z�����������
				pUpVertPlate=pVertPlate;	//��¼Z����������
				pVertPlate->designInfo.origin.datum_pos.Set(0,0,z);
				pVertPlate->designInfo.norm.nearVector.Set(0,-1,0);
				//������Ӧ����˨��������
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{
					if(pLsRef->start1_end2_mid0==31)//������˨��Z������������
					{
						pVertPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pVertPlate->handle,TRUE);
					}
				}
			}
			else if(i==1)
			{	//Z�����������
				pDownVertPlate=pVertPlate;	//��¼Z����������
				pVertPlate->designInfo.origin.datum_pos.Set(0,0,-z);
				pVertPlate->designInfo.norm.nearVector.Set(0,1,0);
				//������Ӧ����˨��������
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{
					if(pLsRef->start1_end2_mid0==41)//������˨��Z������������
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
			//��������ӵ����ӹ����б���
			pParamPlate->AppendAttachPart(pVertPlate);
		}
		if(bUseLineAngle)
		{	//ʹ�ýǸִ������
			double fAngleY=0.5*pParamPlate->GetThick();	//�̽Ǹֶ�λY����
			double wing_wide=N+R-0.5*pParamPlate->GetThick();
			double wing_thick=W;
			if(sJgGuige)	//����ָ���Ǹֹ�����ɽǸ� wht 11-06-01
				restore_JG_guige(sJgGuige,wing_wide,wing_thick);
			for(i=0;i<4;i++)
			{	//�����ĸ��̽Ǹִ���˿����
				CLDSLineAngle *pShortJg=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
				pShortJg->iSeg=pParamPlate->iSeg;
				pShortJg->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
				pShortJg->cMaterial = pParamPlate->cMaterial;
				pShortJg->SetLayer(pParamPlate->layer()); //���������ͼ����
				pShortJg->cfgword=pParamPlate->cfgword;
				//�Ǹ�Ϊ��˨���ӣ���Ӧ���к��Ӹ�����	wht 10-08-02
				//pShortJg->m_hPartWeldParent=pParamPlate->handle;//���ݸ������Ĳ�����ư���
				pShortJg->set_wing_thick(wing_thick);	//֫��
				pShortJg->set_wing_wide(wing_wide);		//֫��
				pShortJg->SetPartNo(sPartNoX);
				//λ�ò���
				pShortJg->desStartPos.SetToDatumPointStyle();	//ֱ��ָ������
				pShortJg->desStartPos.datumPoint.datum_pos_style=9;	//ָ�������ϵ��������
				pShortJg->desStartPos.datumPoint.des_para.hPart=pParamPlate->handle;
				pShortJg->desEndPos.SetToDatumPointStyle();	//ֱ��ָ������
				pShortJg->desEndPos.datumPoint.datum_pos_style=9;	//ָ�������ϵ��������
				pShortJg->desEndPos.datumPoint.des_para.hPart=pParamPlate->handle;
				//���߲���
				pShortJg->des_norm_x.spec_norm.norm_style=4;//ָ�������ϵ���Է���
				pShortJg->des_norm_x.bByOtherWing=FALSE;
				pShortJg->des_norm_x.bSpecific=TRUE;
				pShortJg->des_norm_y.spec_norm.norm_style=4;//ָ�������ϵ���Է���
				pShortJg->des_norm_y.bByOtherWing=FALSE;
				pShortJg->des_norm_y.bSpecific=TRUE;
				if(i==0)
				{	//X-Yƽ��: Y��Z��
					pShortJg->desStartPos.datumPoint.datum_pos.Set(x,-fAngleY,z);
					pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,-fAngleY,z);
					//X֫���߷�����Z����������� Y֫���߷��������
					pShortJg->des_norm_x.spec_norm.hVicePart=pUpVertPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
					pShortJg->des_norm_y.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,-1);
					//������˨
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨ X-Zƽ��:Z������һ�����˨
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22
							||pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pShortJg->AppendMidLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
						}
					}
				}
				else if(i==1)
				{	//X-Yƽ��: Y��Z��
					pShortJg->desStartPos.datumPoint.datum_pos.Set(x,fAngleY,z);
					pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,fAngleY,z);
					//Y֫���߷�����Z����������� X֫���߷��������
					pShortJg->des_norm_y.spec_norm.hVicePart=pUpVertPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
					pShortJg->des_norm_x.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
					//������˨
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨ X-Zƽ��:Z������һ�����˨
						if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12
							||pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pShortJg->AppendMidLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
						}
					}
				}
				else if(i==2)
				{	//X-Yƽ��: Y��Z��
					pShortJg->desStartPos.datumPoint.datum_pos.Set(x,-fAngleY,-z);
					pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,-fAngleY,-z);
					//X֫���߷�����Z����������� Y֫���߷��������
					pShortJg->des_norm_y.spec_norm.hVicePart=pDownVertPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
					pShortJg->des_norm_x.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,1);
					//������˨
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨ X-Zƽ��:Z������һ�����˨
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22
							||pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
						{
							pShortJg->AppendMidLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
						}
					}
				} 
				else if(i==3)
				{	//X-Yƽ��: Y��Z��
					pShortJg->desStartPos.datumPoint.datum_pos.Set(x,fAngleY,-z);
					pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,fAngleY,-z);
					//Y֫���߷�����Z����������� X֫���߷��������
					pShortJg->des_norm_x.spec_norm.hVicePart=pDownVertPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
					pShortJg->des_norm_y.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
					//������˨
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨ X-Zƽ��:Z������һ�����˨
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
				//��������ӵ����ӹ����б���
				pParamPlate->AppendAttachPart(pShortJg);
			}
		}
		else
		{	//����Ϊ��ͨ�ְ�
			for(i=0;i<8;i++)
			{
				CLDSPlate *pPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
				pPlate->iSeg=pParamPlate->iSeg;
				pPlate->SetLayer(pParamPlate->layer()); //���������ͼ����
				pPlate->cfgword=pParamPlate->cfgword;
				pPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
				pPlate->cMaterial = pParamPlate->cMaterial;
				//����Ϊ��˨���ӣ���Ӧ���к��Ӹ�����	wht 10-08-02
				//pPlate->m_hPartWeldParent=pParamPlate->handle;//���ݸ������Ĳ�����ư���
				pPlate->Thick=ftoi(W);
				pPlate->SetPartNo(sPartNoX);
				pPlate->jdb_style=5;	//��׼���������λ�ò���ĸ��Ӱ�
				pPlate->designInfo.m_hBasePart=pParamPlate->handle;
				pPlate->designInfo.norm.norm_style=4;	//��׼�����ϵ���Է���
				pPlate->designInfo.origin.datum_pos_style=9;//��׼�����ϵ��������
				if(i==0)
				{	//X-Yƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,-1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,-y,z);
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==1)
				{	//X-Yƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,-1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,-y,-z);
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==2)
				{	//X-Yƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,y,z);
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨
						if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==3)
				{	//X-Yƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,y,-z);
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨
						if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==4)
				{	//X-Zƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pUpVertPlate->handle;			//�ϲ�����
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pUpVertPlate->handle;	//�ϲ�����
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,z);				//y=R z=������
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Zƽ��:Z������һ�����˨
						if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==5)
				{	//X-Zƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pUpVertPlate->handle;			//�ϲ�����
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pUpVertPlate->handle;	//�ϲ�����
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,-z);				//y=R z=������
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Zƽ��:Z������һ�����˨
						if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==6)
				{	//X-Zƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pDownVertPlate->handle;		//�²�����
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pDownVertPlate->handle;//�²�����
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,-z);				//y=R z=������
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Zƽ��:Z������һ�����˨
						if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==7)
				{	//X-Zƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pDownVertPlate->handle;		//�²�����
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pDownVertPlate->handle;//�²�����
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,z);				//y=R z=������
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Zƽ��:Z������һ�����˨
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
				//��������ӵ����ӹ����б���
				pParamPlate->AppendAttachPart(pPlate);
			}
		}
	}
	for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
	{	
		if(pParamPlate->m_iParamType==TYPE_XEND) 
		{
			//����˨��������/����������˨����ƫ����
			(*pLsRef)->des_base_pos.norm_offset.AddThick(-(long)W);	
			//����ʮ�ֲ����˨������Ʋ����Լ�λ����Ʋ���	wht 10-03-06
			if(pUpVertPlate&&pDownVertPlate)
			{	//X-Zƽ��:Z������һ�����˨
				if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
				{
					(*pLsRef)->des_work_norm.norm_style=4;	//��׼�����ϵ���Է���
					(*pLsRef)->des_work_norm.nearVector.Set(0,0,1);
					(*pLsRef)->des_work_norm.hVicePart=pUpVertPlate->handle;
					//����λ�ò���
					(*pLsRef)->des_base_pos.hPart=pUpVertPlate->handle;
					double wing_offset_dist=fabs((*pLsRef)->des_base_pos.wing_offset_dist);
					double len_offset_dist=fabs((*pLsRef)->des_base_pos.len_offset_dist);
					(*pLsRef)->des_base_pos.wing_offset_dist=wing_offset_dist-z; // z=������
					(*pLsRef)->des_base_pos.len_offset_dist=len_offset_dist;
				}
				//X-Zƽ��:Z������һ�����˨
				else if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
				{
					(*pLsRef)->des_work_norm.norm_style=4;	//��׼�����ϵ���Է���
					(*pLsRef)->des_work_norm.nearVector.Set(0,0,1);
					(*pLsRef)->des_work_norm.hVicePart=pDownVertPlate->handle;
					//����λ�ò���
					(*pLsRef)->des_base_pos.hPart=pDownVertPlate->handle;
					double wing_offset_dist=fabs((*pLsRef)->des_base_pos.wing_offset_dist);
					double len_offset_dist=fabs((*pLsRef)->des_base_pos.len_offset_dist);
					(*pLsRef)->des_base_pos.wing_offset_dist=wing_offset_dist-z; // z=������
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

//��CommonLjSmartDesign.objʹ�øú���������ֱ����Ӳ�������ʱ����ʵ�� wht 11-06-01
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
	//ѡ���׼�ֹ�
	CLDSLineTube *pDatumTube=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("InsertPlateTubeInters Please select datum tube in cross-connection:","");
#else
	pCmdLine->FillCmdLine("InsertPlateTubeInters ��ѡ����ʽ�������ӵĻ�׼�ֹ�:","");
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
	//ѡ�񽻲�ڵ�;
	CLDSNode *pBaseNode=NULL;
	CLDSLineTube *pCrossTubeSeg1=NULL,*pCrossTubeSeg2=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("InsertPlateTubeInters Please select cross structure node for plate tube:","");
#else
	pCmdLine->FillCmdLine("InsertPlateTubeInters ��ѡ����ʽ�ֹܽ��湹��ڵ�:","");
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
		{	//����ڵ�
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
					pCrossTubeSeg1=pCrossTubeSeg2;	//�ô������ı��ضϽ��湹��ʼ�ո�ֵ��pCrossTubeSeg2
				}
				else if(ret2==2&&ret1==1)
					pDatumTube=pCrossTubeSeg2;
				else if(ret2==1&&ret1==1)	
				{	//��������б�ľ������	���ų�������Ľ����
					f3dPoint datum_line_vec=pDatumTube->End()-pDatumTube->Start();
					normalize(datum_line_vec);
					f3dPoint cross_vec1=pCrossTubeSeg1->End()-pCrossTubeSeg1->Start();
					normalize(cross_vec1);
					f3dPoint cross_vec2=pCrossTubeSeg2->End()-pCrossTubeSeg2->Start();
					normalize(cross_vec2);
					if(fabs(datum_line_vec*cross_vec1)>EPS_COS2&&fabs(datum_line_vec*cross_vec2)>EPS_COS2)
					{	//��������
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
				{	//�����������������ڿ�ʼ�������ضϵĽ��湹��
					f3dLine baseline(pCrossTubeSeg1->pStart->Position(false),pCrossTubeSeg1->pEnd->Position(false));
					for(pCrossTubeSeg2=(CLDSLineTube*)Ta.Parts.GetFirst(CLS_LINETUBE);pCrossTubeSeg2;pCrossTubeSeg2=(CLDSLineTube*)Ta.Parts.GetNext(CLS_LINETUBE))
					{
						if((pCrossTubeSeg2->pStart==pBaseNode&&baseline.PtInLine(pCrossTubeSeg2->pEnd->Position(false))<0) ||
							(pCrossTubeSeg2->pEnd==pBaseNode&&baseline.PtInLine(pCrossTubeSeg2->pStart->Position(false))<0))
							break;	//���ʽ�ֹܽ��湹���ȫ����������,�ҵ���һ�����ضϵĽ��湹��
					}
					if(pCrossTubeSeg2)
						break;
				}
			}
			//û��ѡ����ʵĽ��湹��ڵ�
			g_pSolidDraw->ReleaseSnapStatus();
			pBaseNode = NULL;
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("InsertPlateTubeInters node selected not the ideal plate tube cross structural node, please choose cross structure node again:","");
#else
			pCmdLine->FillCmdLine("InsertPlateTubeInters ��ѡ�ڵ㲻�Ǻ���Ĳ��ʽ�ֹܽ��湹��ڵ�,������ѡ�񽻲湹��ڵ�:","");
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
	//һ������ضϵĽ��������ͷ
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
	//������ƽضϵĽ���ĵĲ��
	double fTubeNormOffset=0;
	BYTE ciJointType=0;
	CLDSParamPlate *pParamPlate1=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pParamPlate1->m_hPartWeldParent=pCrossTubeSeg1->handle;
	pParamPlate1->cfgword=pCrossTubeSeg1->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
	if(inters_tube_dlg.m_iInsertPlateType==0)
	{	//���Ͷ˰�
		pParamPlate1->m_iParamType=TYPE_ROLLEND;
		ciJointType=4;
	}
	else if(inters_tube_dlg.m_iInsertPlateType==1)
	{	//U�Ͷ˰�
		pParamPlate1->m_iParamType=TYPE_UEND;
		ciJointType=3;
	}
	else if(inters_tube_dlg.m_iInsertPlateType==2)
	{	//X�Ͷ˰�
		pParamPlate1->m_iParamType=TYPE_XEND;
		ciJointType=5;
	}
	pParamPlate1->cMaterial=CSteelMatLibrary::RecordAt(inters_tube_dlg.m_iDatumPlateMat).cBriefMark;
	pParamPlate1->iSeg=SEGI(inters_tube_dlg.m_sSegI.GetBuffer());
	//��׼ʮ�ֲ�����������Ų���ͬ��	wht 10-01-24
	//Ϊ������ǰ�汾ֻҪ�������Ż��߻�׼������ͬ����Ϊʱͬһ�����ʮ�ֲ��
	pParamPlate1->SetPartNo(inters_tube_dlg.m_sDatumPlatePartNo.GetBuffer(0));
	pParamPlate1->thick=inters_tube_dlg.m_nDatumPlateThick;	//���
	pParamPlate1->m_bStdPart=inters_tube_dlg.m_bStdPart;
	pParamPlate1->SetDesignItemValue('L',L,"",PARAM_SHAPE);	//��峤��
	pParamPlate1->SetDesignItemValue('M',M,"",PARAM_SHAPE);	//���ӳ���
	pParamPlate1->SetDesignItemValue('H',H,"",PARAM_SHAPE);	//�����
	pParamPlate1->SetDesignItemValue('D',0,"");	//��׼X����Ըֹ�X����ת�Ƕ�
	if(inters_tube_dlg.m_iInsertPlateType==1)	//U�Ͷ˰�
	{
		pParamPlate1->SetDesignItemValue('W',W,"",PARAM_SHAPE);	//U�Ͳ����������ļ��
		pParamPlate1->SetDesignItemValue('N',N,"",PARAM_SHAPE);	//�нǳ���
		pParamPlate1->SetDesignItemValue('S',S,"",PARAM_SHAPE);	//�нǿ��	
	}
	else if(inters_tube_dlg.m_iInsertPlateType==0)	//ƽ���߶˰�
	{
		pParamPlate1->SetDesignItemValue('W',W,"",PARAM_SHAPE);	//��ߵĸ߶�
		pParamPlate1->SetDesignItemValue('N',N,"",PARAM_SHAPE);	//�нǳ���
		pParamPlate1->SetDesignItemValue('S',S,"",PARAM_SHAPE);	//�нǿ��
		pParamPlate1->SetDesignItemValue('D',180,"");
		if(pParamPlate1->m_bStdPart)
			pParamPlate1->SetDesignItemValue('X',180,"");
		fTubeNormOffset=0.5*(pParamPlate1->Thick+inters_tube_dlg.m_nTransPlateThick);
	}
	else if(inters_tube_dlg.m_iInsertPlateType==2)	//X�Ͳ��
	{	
		pParamPlate1->SetDesignItemValue('W',W,"",PARAM_SHAPE);	//ʮ���Ͳ���ϸ��Ӱ�ĺ��
		pParamPlate1->SetDesignItemValue('N',N,"",PARAM_SHAPE);	//ʮ���Ͳ���ϸ��Ӱ�Ŀ��
		pParamPlate1->SetDesignItemValue('S',S,"",PARAM_SHAPE);	//ʮ���Ͳ���ϸ��Ӱ�ĳ���
		pParamPlate1->SetDesignItemValue('A',A,"",PARAM_SHAPE);	//ʮ���Ͳ���ϸ��Ӱ�Ķ�λ����A
		pParamPlate1->SetDesignItemValue('R',R,"",PARAM_SHAPE);	//ʮ���Ͳ���ϸ��Ӱ�Ķ�λ����R
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
	//����ʮ�ֲ���ϵ���˨�͸��Ӱ�
	_snprintf(sPartNoP,MAX_CHAR_GUIGE_16,"%s",inters_tube_dlg.m_sWeldPlatePartNo);
	_snprintf(sPartNoX,MAX_CHAR_GUIGE_16,"%s",inters_tube_dlg.m_sWrapPlatePartNo);
	if(!LayoutInsertPlateBoltAndAttachPlate(pParamPlate1,sPartNoP,sPartNoX,
		inters_tube_dlg.m_bUseAngle,inters_tube_dlg.m_sJgGuige.GetBuffer(1)))
	{	//�����жϣ������������
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
	//���ò���ϵ���˨�͸��Ӱ�
	if(!LayoutInsertPlateBoltAndAttachPlate(pParamPlate2,sPartNoP,sPartNoX,
		inters_tube_dlg.m_bUseAngle,inters_tube_dlg.m_sJgGuige.GetBuffer(1)))
	{	//�����жϣ������������
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
		AfxMessageBox("δ���ò����˨���޷������������!");
#endif
		g_pSolidDraw->ReleaseSnapStatus();
		return 0;
	}
	//������ƹ�ͨ����ĵĽ��������Ӱ�
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
	pTransPlate1->cfgword=pDatumTube->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
	pTransPlate1->iSeg=SEGI(inters_tube_dlg.m_sSegI.GetBuffer());
	pTransPlate1->cMaterial=CSteelMatLibrary::RecordAt(inters_tube_dlg.m_iTransPlateMat).cBriefMark;
	pTransPlate1->Thick=inters_tube_dlg.m_nTransPlateThick;
	pTransPlate1->m_uStatMatCoef=1;
	pTransPlate1->m_fNormOffset=-0.5*inters_tube_dlg.m_nTransPlateThick;
	//�������Ӱ������ϵ
	CDesignLjPartPara *pLjPartPara=pTransPlate1->designInfo.partList.Add(pCrossTubeSeg1->handle);
	pLjPartPara->hPart=pCrossTubeSeg1->handle;
	pLjPartPara->iFaceNo=1;
	pLjPartPara->m_nClassTypeId=pCrossTubeSeg1->GetClassTypeId();
	if(pCrossTubeSeg1->pStart==pBaseNode)
		pLjPartPara->start0_end1=0;	//ʼ������
	else if(pCrossTubeSeg1->pEnd==pBaseNode)
		pLjPartPara->start0_end1=1;//�ն�����
	if(inters_tube_dlg.m_bTransPlate)
	{	//��ͨ������
		pLjPartPara=pTransPlate1->designInfo.partList.Add(pCrossTubeSeg2->handle);
		pLjPartPara->hPart=pCrossTubeSeg2->handle;
		pLjPartPara->iFaceNo=1;
		pLjPartPara->m_nClassTypeId=pCrossTubeSeg2->GetClassTypeId();
		if(pCrossTubeSeg2->pStart==pBaseNode)
			pLjPartPara->start0_end1=0;	//ʼ������
		else if(pCrossTubeSeg2->pEnd==pBaseNode)
			pLjPartPara->start0_end1=1;//�ն�����
	}
	pTransPlate1->designInfo.m_bEnableFlexibleDesign=TRUE;	//�����������
	pTransPlate1->designInfo.iProfileStyle0123=3;			//�������������
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
	{	//����X�᷽��ָ���һ�θֹ�
		pTransPlate1->ucs.axis_x*=-1.0;
		pTransPlate1->ucs.axis_y*=-1.0;
	}
		
	f3dPoint vertex_up,vertex_down,bolt_up,bolt_down,weld_vertex_start,weld_vertex_end;
	double minx=1000000,miny=0.5*H,maxy=-0.5*H;
	CLsRef *pLsRef;
	for(pLsRef=pParamPlate1->GetFirstLsRef();pLsRef;pLsRef=pParamPlate1->GetNextLsRef())
	{	//������˨����ͨ����
		CLDSBolt *pBolt=pLsRef->GetLsPtr();
		//�ֹ�����ͷ�޸ĺ���˨λ�úͷ��߷����п��ܱ仯
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
			pBolt->AddL0Thick(pTransPlate1->handle,TRUE);	//������˨ͨ��	
			pTransPlate1->AppendLsRef(*pLsRef);
		}
	}
	if(inters_tube_dlg.m_bTransPlate)
	{	//��ͨ������
		for(pLsRef=pParamPlate2->GetFirstLsRef();pLsRef;pLsRef=pParamPlate2->GetNextLsRef())
		{	//������˨����ͨ����
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			//�ֹ�����ͷ�޸ĺ���˨λ�úͷ��߷����п��ܱ仯
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			if((pParamPlate2->m_iParamType==TYPE_XEND&&(pBolt->feature==12||pBolt->feature==22))
				||pParamPlate2->m_iParamType!=TYPE_XEND)
			{
				pBolt->AddL0Thick(pTransPlate1->handle,TRUE);	//������˨ͨ��
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
	{	//��ͨ������
		//�˰�2�ϵĶ���
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
	//�˰�1�ϵĶ���
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
	//��ϵͳ�Զ����㺸���������ζ���
	f3dPoint offset_vec=0.5*pDatumTube->GetDiameter()*pTransPlate1->ucs.axis_x;
	f3dLine prj_line(pDatumTube->Start()+offset_vec,pDatumTube->End()+offset_vec);
	f3dPoint seed_vertex[4],diffused_up_vec,diffused_down_vec;
	SnapPerp(&seed_vertex[0],prj_line,vertex_up);
	SnapPerp(&seed_vertex[1],prj_line,vertex_down);
	diffused_up_vec = diffused_down_vec = -cross_line_vec;
	//���ݹ����⾭��Ӧ����ɢ�Ǿ�ȡ30��,��ÿ����ɢ30�㣬�ܼ�60��
	double diffuseAngle=30*RADTODEG_COEF;
	//��������������ɢ�߷����ʱ��Ӧ������˨���ڹ�����Z����ת
	RotateVectorAroundVector(diffused_up_vec,diffuseAngle,pParamPlate1->ucs.axis_z);
	RotateVectorAroundVector(diffused_down_vec,-diffuseAngle,pParamPlate1->ucs.axis_z);
	f3dPoint diffused_upplane_norm = diffused_up_vec^pTransPlate1->ucs.axis_z;
	f3dPoint diffused_downplane_norm = diffused_down_vec^pTransPlate1->ucs.axis_z;
	Int3dlf(seed_vertex[2],prj_line,bolt_up,diffused_upplane_norm);
	Int3dlf(seed_vertex[3],prj_line,bolt_down,diffused_downplane_norm);
	//����ɢ�����߾�����Ч����ɢ���������İ����λ���
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
	{	//��ɢ���ɵ���������ɢ��ͬһ�࣬��Զ�����ӵ�϶�����һ�����Ӷ��㷢����䵼�£���Ӧȡ�϶��߲�����
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
			if(start_vec*datum_line_vec<0)	//ʼ�˲�
				weld_vertex_start=seed_vertex[1];
			else
				weld_vertex_end=seed_vertex[1];
			i++;
		}
		else
		{
			f3dPoint extend_vec;
			//ʼ�˲�
			extend_vec = seed_vertex[i]-weld_vertex_start;
			if(extend_vec*datum_line_vec<0)
				weld_vertex_start=seed_vertex[i];
			//�ն˲�
			extend_vec=seed_vertex[i]-weld_vertex_end;
			if(extend_vec*datum_line_vec>0)
				weld_vertex_end=seed_vertex[i];
		}
	}

	coord_trans(weld_vertex_start,pTransPlate1->ucs,FALSE);
	coord_trans(weld_vertex_end,pTransPlate1->ucs,FALSE);
	//���������� �����߸˼��ͻ�׼�˼�(���)֮��ĺ��ӵ��-Yֵȡ����һ���ӵ��Yֵ
	if(cross_line_vec*datum_line_vec>0)
		weld_vertex_start.y=-weld_vertex_end.y;
	else //if(cross_line_vec*datum_line_vec<0)
		weld_vertex_end.y=-weld_vertex_start.y;
	if(fabs(weld_vertex_start.y)<inters_tube_dlg.m_nTransPlateWidth/2.0)
		weld_vertex_start.y=-inters_tube_dlg.m_nTransPlateWidth/2.0;
	if(fabs(weld_vertex_end.y)<inters_tube_dlg.m_nTransPlateWidth/2.0)
		weld_vertex_end.y=inters_tube_dlg.m_nTransPlateWidth/2.0;
	//�������ɵĺ��춥�� �����==������� wht 11-06-01
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
	{	//��ͨ������
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
				pPrevVertex->m_bWeldEdge=1;	//������
				//��亸�ӱ߶Ի����Ϣ //���Ӹ�����Ϊ�ֹ�ʱ����Ҫ�Ի��
				pPrevVertex->local_point_vec=1;	//0.�޶Ի�� 1.�Ի��50-->100����ͬ���Ӹ��˼�ʼ-->�շ��� 2.���ʼ-->�շ����෴ 
				pPrevVertex->local_point_y=0;	//�Ի��Y����ֵ
			}
			pPrevVertex=pVertex;
		}
	}

	pTransPlate1->DesignProfile();
	pTransPlate1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pTransPlate1->GetSolidPartObject());
	if(!inters_tube_dlg.m_bTransPlate)
	{	//�ǹ�ͨ������
		pTransPlate2=(CLDSPlate*)console.AppendPart(CLS_PLATE);
		pTransPlate2->CopyProperty(pTransPlate1);
		pTransPlate2->m_hPartWeldParent=pDatumTube->handle;
		pTransPlate2->cfgword=pDatumTube->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		pLjPartPara=pTransPlate2->designInfo.partList.Add(pCrossTubeSeg2->handle);
		pLjPartPara->hPart=pCrossTubeSeg2->handle;
		pLjPartPara->iFaceNo=1;
		pLjPartPara->m_nClassTypeId=pCrossTubeSeg2->GetClassTypeId();
		if(pCrossTubeSeg2->pStart==pBaseNode)
			pLjPartPara->start0_end1=0;	//ʼ������
		else if(pCrossTubeSeg2->pEnd==pBaseNode)
			pLjPartPara->start0_end1=1;//�ն�����
		pTransPlate2->designInfo.m_bEnableFlexibleDesign=TRUE;	//�����������
		pTransPlate2->designInfo.iProfileStyle0123=3;			//�������������
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
		{	//������˨����ͨ����
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			//�ֹ�����ͷ�޸ĺ���˨λ�úͷ��߷����п��ܱ仯
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			if((pParamPlate2->m_iParamType==TYPE_XEND&&(pBolt->feature==12||pBolt->feature==22))
				||pParamPlate2->m_iParamType!=TYPE_XEND)
			{
				pBolt->AddL0Thick(pTransPlate2->handle,TRUE);	//������˨ͨ��
				pTransPlate2->AppendLsRef(pBolt->GetLsRef());
			}
		}
		pTransPlate2->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pTransPlate2->GetSolidPartObject());
	}
	//�����ᴩ���ͳһ������˨  wxc-2018.3.14
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
	//�ġ���ƹ�ͨ����ֹ��ϵĴ��������
	double fOffRidus=pDatumTube->GetDiameter()*0.5+inters_tube_dlg.m_fVertPlateManuSpace;
	double fPlateLen=fabs(oddment)+M-minx+LsSpace.EndSpace-fOffRidus/sina; //���㴹����������W,N��ֵ	 
	if(inters_tube_dlg.m_bCreateVertPlate)
	{
		CLDSParamPlate *pTubeRibPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pTubeRibPlate->m_iParamType=TYPE_TUBERIBPLATE;
		pTubeRibPlate->m_hPartWeldParent=pDatumTube->handle;
		pTubeRibPlate->cfgword=pDatumTube->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		pTubeRibPlate->layer(2)=pDatumTube->layer(2);	//�����ֹ��߰��ͼ����
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
		//��Խ����������� wht 11-05-17
		CONNECT_DIRECTION connectVec;
		connectVec.style=0;		//��������
		connectVec.serial=1;	//��������ĺ�������
		connectVec.end_space=connectVec.high_space=(short)LsSpace.EndSpace;
		connectVec.direct.norm_style=5;	//�˼���������
		connectVec.direct.hVicePart=pCrossTubeSeg1->handle;
		if(pCrossTubeSeg1->pStart==pBaseNode)
			connectVec.direct.direction=0;	//ʼ->��
		else 
			connectVec.direct.direction=1;	//��->ʼ
		pTubeRibPlate->ConnDirectList.Empty();
		pTubeRibPlate->ConnDirectList.SetValue(1,connectVec);
		//��������
		pTubeRibPlate->SetDesignItemValue('C',pDatumTube->handle,"",PARAM_POS,1);	//��λ��׼�ֹ�
		pTubeRibPlate->SetDesignItemValue('P',pBaseNode->handle,"",PARAM_POS,1);	//��λ��׼�ڵ�
		pTubeRibPlate->SetDesignItemValue('E',pTransPlate1->handle,"",PARAM_POS,1);	//ȷ���ֹ��߰�λ�õĻ�׼�ְ�
		if(pTransPlate2)	//���Ӱ岻�ǹ�ͨ�ְ�ʱ�������׼�ְ�
			pTubeRibPlate->SetDesignItemValue('F',pTransPlate2->handle,"",PARAM_POS,1);	//ȷ���ֹ��߰�λ�õĻ�׼�ְ�
		//���β���
		pTubeRibPlate->SetDesignItemValue('V',inters_tube_dlg.m_fVertPlateManuSpace,"");//�ӹ���϶ wht 10-08-28
		pTubeRibPlate->SetDesignItemValue('D',inters_tube_dlg.m_nVertPlateWidth,"",PARAM_SHAPE);	//���δ���
		pTubeRibPlate->SetDesignItemValue('W',fPlateLen,"",PARAM_SHAPE);	//����W
		pTubeRibPlate->SetDesignItemValue('N',fPlateLen,"",PARAM_SHAPE);	//����N
		pTubeRibPlate->SetDesignItemValue('H',0.5*(H-pParamPlate1->GetThick()),"",PARAM_SHAPE);	//����H
		double Y=-1,F=-1;
		pTubeRibPlate->SetDesignItemValue('Y',Y,"");	//��Բ�ͻ�����λ��(>0��׼�����;=0�Զ�;<0��׼���ڲ�)
		pTubeRibPlate->SetDesignItemValue('G',-1,"",PARAM_SHAPE);	//���ɱ�����(>0ֱ�ߣ�<=0Բ��)
		pTubeRibPlate->SetDesignItemValue('S',0,"");	//�߰�ķ���ƫ�ƾ���	
		pTubeRibPlate->DesignPlate();	//�˴���Ҫ���ڼ����Խ��������ϵ
		//�Գ���������һ��
		CLDSParamPlate *pMirPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pMirPlate->CopyProperty(pTubeRibPlate);
		pMirPlate->ConnDirectList.Empty();
		pMirPlate->ConnDirectList.SetValue(connectVec.serial,connectVec);
		pMirPlate->layer(2)=pDatumTube->layer(2);	//�����ֹ��߰��ͼ����
		pMirPlate->m_hPartWeldParent=pDatumTube->handle;
		pMirPlate->SetDesignItemValue('W',fPlateLen,"",PARAM_SHAPE);	//����W
		pMirPlate->SetDesignItemValue('N',fPlateLen,"",PARAM_SHAPE);	//����N
		pMirPlate->SetDesignItemValue('Y',-Y,"");	//��Բ�ͻ�����λ��(>0��׼�����;=0�Զ�;<0��׼���ڲ�)
		pMirPlate->ucs=pTubeRibPlate->ucs;
		pMirPlate->DesignPlate();		//�˴���Ҫ���ڼ����Խ��������ϵ
		//������˨
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
				if(pLsRef->start1_end2_mid0==32)	//Z������һ�����˨
				{	//������˨��Z������һ���������
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
				else if(pLsRef->start1_end2_mid0==42) //Z������һ�����˨
				{	//������˨��Z������һ���������
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
		//������˨�Ժ�����ƿ�Խ���壬��Ҫ������˨�����Խ�������� wht 11-05-17
		pTubeRibPlate->DesignPlate();
		pTubeRibPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pTubeRibPlate->GetSolidPartObject());
		pMirPlate->DesignPlate();
		pMirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pMirPlate->GetSolidPartObject());
	}
	//�塢��ƹ�ͨ����ֹ��ϵĻ�����
	double D=inters_tube_dlg.m_nCoverPlateWidth-inters_tube_dlg.m_nCoverPlateCutWidth;
	double BH=(inters_tube_dlg.m_iHoleType==2)?0.0:-1.0;
	CLDSParamPlate::DESIGN_PARAM param;
	param.cValueType=3;
	param.value.uiAttribute=ftoi(inters_tube_dlg.m_fHoleR)*10;
	param.value.hPart=pTransPlate1->handle;
	//X����Ϸ��
	CLDSParamPlate *pCoverPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pCoverPlate->m_iParamType = TYPE_CIRCOVERPLATE;
	pCoverPlate->SetPartNo(inters_tube_dlg.m_sCoverPlatePartNo.GetBuffer(0));	//���
	pCoverPlate->iSeg = SEGI(inters_tube_dlg.m_sSegI.GetBuffer());	//�κ�
	pCoverPlate->layer(2)=pDatumTube->layer(2);
	pCoverPlate->cMaterial = CSteelMatLibrary::RecordAt(inters_tube_dlg.m_iCoverPlateMat).cBriefMark;	//����
	pCoverPlate->thick = inters_tube_dlg.m_nCoverPlateThick;
	pCoverPlate->m_hPartWeldParent = pDatumTube->handle;
	pCoverPlate->cfgword=pDatumTube->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
	pCoverPlate->SetDesignItemValue('A',inters_tube_dlg.m_fCoverPlateOffsetAngle,"");	//�����ת�Ƕ�
	pCoverPlate->SetDesignItemValue('B',inters_tube_dlg.m_fCoverPlateSectorAngle,"",PARAM_SHAPE);	//���μн�
	pCoverPlate->SetDesignItemValue('C',pDatumTube->handle,"",PARAM_POS,1);	//��λ��׼��׼�ֹ�
	pCoverPlate->SetDesignItemValue('E',pBaseNode->handle,"",PARAM_POS,1);	//��λ��׼�ڵ�
	pCoverPlate->SetDesignItem('F',param,"",PARAM_POS,1);	//��һ���׼�ְ�
	pCoverPlate->SetDesignItemValue(KEY2C("BH"),BH,"",PARAM_POS,1);
	pCoverPlate->SetDesignItemValue('G',0,"",PARAM_POS,1);	//�ڶ����׼�ְ�
	pCoverPlate->SetDesignItemValue('D',D,"",PARAM_SHAPE);	//���������D
	pCoverPlate->SetDesignItemValue('W',inters_tube_dlg.m_nCoverPlateWidth,"",PARAM_SHAPE);	//���������W
	pCoverPlate->SetDesignItemValue('S',(double)-1,"");	//����Ϸ��
	//������ӹ���϶ֵ wht 10-08-28
	pCoverPlate->SetDesignItemValue('R',pDatumTube->GetDiameter()*0.5+inters_tube_dlg.m_fCoverPlateManuSpace,"",PARAM_SHAPE);
	pCoverPlate->SetDesignItemValue('M',(double)0,"");	//MΪ-1ʱ��ʾҪ��ƹ�ͨ��������һ��ķ��
	pCoverPlate->DesignPlate();
	pCoverPlate->SetModified();
	pCoverPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pCoverPlate->GetSolidPartObject());
	//X��һ���·��
	CLDSParamPlate *pCoverPlate1=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pCoverPlate1->CopyProperty(pCoverPlate);
	pCoverPlate1->SetDesignItemValue('S',(double)1,"");	//����·��
	pCoverPlate1->SetDesignItemValue('M',(double)0,"");	//MΪ-1ʱ��ʾҪ��ƹ�ͨ��������һ��ķ��
	pCoverPlate1->m_hPartWeldParent = pDatumTube->handle;
	pCoverPlate1->DesignPlate();
	pCoverPlate1->SetModified();
	pCoverPlate1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pCoverPlate1->GetSolidPartObject());
	//X�Ḻ����һ���Ϸ���ڶ������Ӱ��ϵĻ�����
	if(pTransPlate2)
		param.value.hPart=pTransPlate2->handle;
	CLDSParamPlate *pCoverPlate2=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pCoverPlate2->CopyProperty(pCoverPlate);
	pCoverPlate2->SetDesignItemValue('S',(double)-1,"");	//����Ϸ��
	if(inters_tube_dlg.m_bTransPlate)	//��ͨ������
		pCoverPlate2->SetDesignItemValue('M',(double)-1,"");	//MΪ-1ʱ��ʾҪ��ƹ�ͨ��������һ��ķ��
	else if(pTransPlate2)				//�ڶ������Ӱ��ϵĻ�����
		pCoverPlate2->SetDesignItem('F',param,"",PARAM_POS,1);	//��һ���׼�ְ�
	pCoverPlate2->m_hPartWeldParent = pDatumTube->handle;
	pCoverPlate2->DesignPlate();
	pCoverPlate2->SetModified();
	pCoverPlate2->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pCoverPlate2->GetSolidPartObject());
	//X�Ḻ����һ���·���ڶ������Ӱ��ϵĻ�����
	CLDSParamPlate *pCoverPlate3=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pCoverPlate3->CopyProperty(pCoverPlate);
	pCoverPlate3->SetDesignItemValue('S',(double)1,"");	//����·��
	if(inters_tube_dlg.m_bTransPlate)	//��ͨ������
		pCoverPlate3->SetDesignItemValue('M',(double)-1,"");	//MΪ-1ʱ��ʾҪ��ƹ�ͨ��������һ��ķ��
	else if(pTransPlate2)				//�ڶ������Ӱ��ϵĻ�����
		pCoverPlate3->SetDesignItem('F',param,"",PARAM_POS,1);	//��һ���׼�ְ�
	pCoverPlate3->m_hPartWeldParent = pDatumTube->handle;
	pCoverPlate3->DesignPlate();
	pCoverPlate3->SetModified();
	pCoverPlate3->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pCoverPlate3->GetSolidPartObject());
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	g_pSolidDraw->Draw();
	//���ɻ������ϵĹ��տ�
	if(inters_tube_dlg.m_iHoleType==1)
	{
		if(CreateProcessHole(pCoverPlate,pTransPlate1,TRUE))
		{
			CreateProcessHole(pCoverPlate1,pTransPlate1,FALSE);
			if(inters_tube_dlg.m_bTransPlate)	
			{	//��ͨ������
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
	pCmdLine->FillCmdLine("����:","");
#endif
	return 0;
}

void CLDSView::OnGyHole()
{
	m_nPrevCommandID=ID_GY_HOLE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat water hole";
#else
	m_sPrevCommandName="�ظ���ˮ��";
#endif
	Command("NewProcessHole");
}

int CLDSView::NewProcessHole()
{	//���տ�
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
	cmdStr.Format("NewProcessHole ��ѡ����Ҫ���ù��տ׵ĸְ�:");
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
			errCmdStr.Format("NewProcessHole û��ѡ�к��ʵĹ�����������ѡ����Ҫ���ù��տ׵ĸְ�:");
#endif
			pCmdLine->FillCmdLine(errCmdStr,"");
		}
		else
			break;
	}
	pCmdLine->FinishCmdLine(CXhChar16("0X%X",pHoriPlate->handle));
	g_pSolidDraw->SetEntSnapStatus(pHoriPlate->handle);
	//ѡ��λ�ְ�
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("NewProcessHole Please select positioning plate which is vertical to the plate on which process holes to be arranged:");
#else
	cmdStr.Format("NewProcessHole ��ѡ���벼�׸ְ崹ֱ�Ķ�λ�ְ�:");
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
			errCmdStr.Format("NewProcessHole û��ѡ�к��ʵĹ������������벼�׸ְ崹ֱ�Ķ�λ�ְ�:");
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
	pCmdLine->FillCmdLine("����:","");
#endif
	return 0;
}

void CLDSView::OnTubeBetweenPlate() 
{
	m_nPrevCommandID=ID_TUBE_BETWEEN_PLATE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat tube between-plate";
#else
	m_sPrevCommandName="�ظ��ֹܼа�";
#endif
	Command("TubeBetweenPlate");
}
int CLDSView::TubeBetweenPlate() 
{	//��Ƹֹܼа�
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	DWORD dwhObj=0,dwExportFlag=0;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	g_pSolidDraw->ReleaseSnapStatus();
	//ѡ���һ����׼�ֹ�
	CLDSLineTube *pDatumTube1=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("TubeBetweenPlate please select the design datum tube for the first between-plate:","");
#else
	pCmdLine->FillCmdLine("TubeBetweenPlate ��ѡ���һ���а����ƻ�׼�ֹ�:","");
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
	//ѡ��ڶ����ֹ�
	CLDSLineTube *pDatumTube2=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("TubeBetweenPlate please select the design datum tube for the first between-plate:","");
#else
	pCmdLine->FillCmdLine("TubeBetweenPlate ��ѡ��ڶ����а����ƻ�׼�ֹ�:","");
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
	//��Ƹֹܼа�
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
	pCmdLine->FillCmdLine("����:","");
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
		AfxMessageBox("�ܱ�Ǹ,Ŀǰ�ݲ�֧����������!");
#endif
		return FALSE;
	}
	if(pPlate->vertex_list.GetNodeNum()<3)
		return FALSE;
	CUndoOperObject undo(&Ta);
	//�ٶ�������ϵ
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
	{	//ָ�����췽�� ȡ�ڰ���������ϵ��X��С�ĵ������췽������
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
		pMinVertex->m_cPosDesignType=0;	//�Ѿ��ı��˶������꣬�䶨λ���͸�Ϊ�û��Զ��� wjh-2015.1.27
		pPlate->designInfo.iProfileStyle0123=3;	//�Ѿ��ı��˶������꣬�䶨λ���͸�Ϊ����������
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
		pMinVertex->m_cPosDesignType=0;	//�Ѿ��ı��˶������꣬�䶨λ���͸�Ϊ�û��Զ��� wjh-2015.1.27
		
		tm_pnt=pPlate->GetRealVertex(pSecMinVertex->vertex);
		coord_trans(tm_pnt,pPlate->ucs,TRUE);
		Int3dlf(tm_pnt,tm_pnt,extend_vec,face_pos,face_norm);
		coord_trans(tm_pnt,pPlate->ucs,FALSE);
		pSecMinVertex->vertex=tm_pnt;
		pSecMinVertex->vertex.feature=1;
		pSecMinVertex->m_cPosDesignType=0;	//�Ѿ��ı��˶������꣬�䶨λ���͸�Ϊ�û��Զ��� wjh-2015.1.27
		pPlate->designInfo.iProfileStyle0123=3;	//�Ѿ��ı��˶������꣬�䶨λ���͸�Ϊ����������

		if(pWeldFatherPart)
		{	//���ú��ӱ�����
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
	m_sPrevCommandName="�ظ�����ְ�";
#endif
	Command("ExtendPlate");
}
int CLDSView::ExtendPlate() 
{	//����ְ嵽ָ������
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr,promptStr;
	CLDSPart *pPart=NULL;
	CLDSLinePart* pDatumLinePart=NULL;
	CLDSPlate *pExtendPlate=NULL;
	CSuperSmartPtr<CLDSPart>pCutPart;
	BOOL bSpecExtendVecPart=TRUE;	//�Ƿ�ָ�����췽���׼�˼�
	//ѡ����Ҫ����ĸְ�
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
		pCmdLine->FillCmdLine("ExtendPlate ��ѡ����Ҫ����ĸְ�:","");
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
	//ѡ�����췽���׼�˼�������Ŀ��λ�û�׼����
	BOOL bInPutPlane=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("ExtendPlate Please select the datum rod for extending direction or datum part for extending destination position,or directly press enter key to specify extending destination plane:","");
#else
	pCmdLine->FillCmdLine("ExtendPlate ��ѡ�����췽���׼�˼�������Ŀ��λ�û�׼����������ֱ�Ӱ��س���ָ������Ŀ��ƽ��:","");
#endif
	verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART|GetSingleWord(SELECTINDEX_PARAMPLATE));
	while(1)
	{
		if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			return 0;
		SELOBJ obj(dwhObj,dwExportFlag);
		pPart=(CLDSPart*)console.FromPartHandle(obj.hRelaObj);
		if(pPart&&pPart->IsLinePart())
		{	//���췽���׼����
			pDatumLinePart=(CLDSLinePart*)pPart;
			bSpecExtendVecPart=TRUE;
			break;
		}
		else if(pPart&&(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE))
		{	//����Ŀ��λ�û�׼����
			pCutPart=pPart;
			bSpecExtendVecPart=FALSE;
			break;
		}
		else if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
		{	//ָ������Ŀ��ƽ��
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
	{	//ָ������Ŀ��ƽ��
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
			AfxMessageBox("ָ���淨��Ϊ��,Ϊ�Ƿ���!");
#endif
			pCmdLine->CancelCmdLine();
			return 0;
		}
		ExtendPlateToPlane(pExtendPlate,face_pos,face_norm);
		Command("ExtendPlate");
		return 0;
	}
	if(bSpecExtendVecPart)
	{	//���ָ�����췽���׼�˼� ����Ҫ����ָ������Ŀ���׼����
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("ExtendPlate Please select the datum part for extending destination position:","");
#else
		pCmdLine->FillCmdLine("ExtendPlate ��ѡ������Ŀ��λ�û�׼����:","");
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
		pCmdLine->FillCmdLine("ExtendPlate �������׼�ְ���Ϊ����Ŀ��ƽ�����ţ�Ĭ��Ϊ��׼��<0>:","");
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
					pCmdLine->FillCmdLine("ExtendPlate ����������������������׼�ְ���Ϊ�и�������,Ĭ��Ϊ��׼��<0>:","");
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
	if(bSpecExtendVecPart)	//ָ�����췽���׼�˼�
	{
		extend_vec=pDatumLinePart->End()-pDatumLinePart->Start();
		if(verify_vec*extend_vec<0)
			extend_vec*=-1.0;
	}
	else
		extend_vec=verify_vec;	//δָ�����췽�� �ñ����������ҵ���ȷ��������
	extend_vec-=(extend_vec*pExtendPlate->ucs.axis_z)*pExtendPlate->ucs.axis_z;
	if(pCutPart->GetClassTypeId()==CLS_PLATE)
	{	//Ŀ�깹��Ϊ�����ʱ ��������
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
	{	//ָ�����췽���׼�˼�
		Ta.BeginUndoListen();
		f3dPoint vertice,verify_point,lowest_point;
		PROFILE_VERTEX *pVertex,*pLowestVertex=NULL;
		double low_level,low_h;
		for(pVertex=pExtendPlate->vertex_list.GetFirst();pVertex;pVertex=pExtendPlate->vertex_list.GetNext())
		{
			verify_point+=pVertex->vertex;
			if(pDatumLinePart->GetClassTypeId()==CLS_LINETUBE&&pExtendPlate->m_hPartWeldParent==pDatumLinePart->handle)
			{	//���췽���׼�˼�Ϊ�ֹ���Ϊ����ְ�ĺ��Ӹ����� ��ֻ����ֹ������ı�������͵�
				CLDSLineTube *pLineTube=(CLDSLineTube*)pDatumLinePart;
				if(fabs(fabs(pVertex->vertex.x)-0.5*pLineTube->GetDiameter())>EPS2+1) //�ʵ��Ŵ�Χ
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
				{	//ȡ��������Ŀ�Ĺ�����ȵĵ�����ְ������������ĵ�
					double dd2=sqrt(dd1*dd1-dd*dd);
					if(dd2<low_h)
						pLowestVertex=pVertex;
				}
			}
		}
		if(!pLowestVertex)
		{	//��ֹ��δ���ǵ������ �Ժ��ȥ��
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
					{	//ȡ��������Ŀ�Ĺ�����ȵĵ�����ְ������������ĵ�
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
		pLowestVertex->m_cPosDesignType=0;	//�Ѿ��ı��˶������꣬�䶨λ���͸�Ϊ�û��Զ��� wjh-2015.1.27
		pExtendPlate->designInfo.iProfileStyle0123=3;	//�Ѿ��ı��˶������꣬�䶨λ���͸�Ϊ����������
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
			pCmdLine->FillCmdLine("ExtendPlate Ŀ��λ�øֹ��ϵ����ӱ������û�ָ������(Y), �����Զ��������ֹܶ�(N)<N>:","");
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
						pCmdLine->FillCmdLine("ExtendPlate �����������������ȷ��Ŀ��λ�øֹ��ϵ����ӱ������û�ָ������(Y), �����Զ��������ֹܶ�(N)<N>>:","");
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
				cmdStr.Format("ExtendPlate ���������쵽Ŀ��λ�ú�����ӱ߿��<%.0f>:",extendEdgeLen);
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
			{	//����
				pCutPart.ParamPlatePointer()->GetDesignItemValue('W',&W);
				pCutPart.ParamPlatePointer()->GetDesignItemValue('R',&R);
				r=R;
				CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(pCutPart->m_hPartWeldParent,CLS_LINETUBE);
				if(pLineTube)
					r=pLineTube->GetDiameter()*0.5;
				extendEdgeLen=0.5*W-r;
			}
			else //if(pCutPart.ParamPlatePointer()->m_iParamType==TYPE_CIRCOVERPLATE)
			{	//������
				pCutPart.ParamPlatePointer()->GetDesignItemValue('W',&W);
				extendEdgeLen=W;
			}
			CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
			ss.Format("ExtendPlate Please input the width of connecting edge after extending to destination position,default as flange width<%s>:",(char*)CXhChar16(extendEdgeLen));
#else
			ss.Format("ExtendPlate ���������쵽Ŀ��λ�ú�����ӱ߿��,Ĭ��Ϊ�������<%s>:",(char*)CXhChar16(extendEdgeLen));
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
			if(pCutPart->GetClassTypeId()==CLS_PLATE&&pCutPart.PlatePointer()->m_fInnerRadius>0)	//�����
				extendEdgeLen=pCutPart.PlatePointer()->GetWidth()*0.5-pCutPart.PlatePointer()->m_fInnerRadius;
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("ExtendPlate Please input the width of connecting edge after extending to destination position<%.0f>:",extendEdgeLen);
#else
			cmdStr.Format("ExtendPlate ���������쵽Ŀ��λ�ú�����ӱ߿��<%.0f>:",extendEdgeLen);
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
			{	//�������ֹܶ�
				f3dPoint line_vec=pCutPart.LineTubePointer()->End()-pCutPart.LineTubePointer()->Start();
				double oddment=0;
				if(line_vec*inters_line_vec<0)
				{	//�ְ��������ֹ�ʼ��
					if(pCutPart.LineTubePointer()->m_tJointStart.hLinkObj>0x20)
						pFlPlate=(CLDSParamPlate*)console.FromPartHandle(pCutPart.LineTubePointer()->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
					if(pFlPlate&&!pFlPlate->IsFL())
						pFlPlate=NULL;
					line_vec=pCutPart.LineTubePointer()->Start()-vertice;
					oddment=pCutPart.LinePartPointer()->startOdd();
				}
				else
				{	//�ְ��������ֹ��ն�
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
	m_sPrevCommandName="�ظ��ϲ����Ӱ�";
#endif
	Command("MergePlates");
}
/* �����α�д���룬��ʱ���ã������ڿ����ϲ����Ӱ幹��ͬ�����߲�ͬ�������ʽ���ܻ��õ���wjh-2012.6.30
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
	//�����ζ���ļ������ĵ�origin
	for(i=0;i<nCount;i++)
		origin+=pPntArr[i].vertex;
	origin /=nCount;
	for (i=0;i<nCount;i++)
	{
		if (fabs(pPntArr[i].rayVecX)+fabs(pPntArr[i].rayVecY)<EPS)
		{	//��λ�� 
			f3dPoint ray_vec=f3dPoint(pPntArr[i].vertex.x,pPntArr[i].vertex.y,0)-f3dPoint(origin.x,origin.y,0);
			normalize(ray_vec);
			pPntArr[i].rayVecX = ray_vec.x;
			pPntArr[i].rayVecY = ray_vec.y;
		}
	}
	CQuickSort<PROFILE_VERTEX>::QuickSort(pPntArr,nCount,CompareVertex);
}
*/
//��ȡ�ְ���ָ�����췽���ϵ���ߵ�
f3dPoint GetPlateHighestVertex(CLDSPlate *pPlate,f3dPoint spread_vec);
//��ȡ�ְ�ָ�����췽���ϵĵױ�
typedef PROFILE_VERTEX* VERTEX_PTR;
BOOL GetPlateBottomEdge(CLDSPlate *pPlate,f3dPoint spread_vec,f3dPoint tube_len_vec,
						f3dLine &btm_line,VERTEX_PTR *ppStartVertex,BOOL *pbReverse);
//�ϲ��ְ���������vertex_list
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
	//1.��ȡ�ְ����췽��,�Ƚ����췽���Ƿ�һ��
	//TODO:ͨ��CutJg.cpp�е�RodJunctionDiscern()����δ����ͬһ�ڵ����� wjh-2016.8.23
	GEPOINT cut_plane_norm,cut_plane_pick=pBaseNode->Position();
	GEPOINT tube_len_vec[2]={pTube1->ucs.axis_z,pTube2->ucs.axis_z};
	if(pTube1!=pTube2)
	{	//���ְ��׼������ͬʱһ��Ϊ������������������Ӷ��ж����ӷ���
		if(pTube1->pEnd==pBaseNode||DISTANCE(pTube1->Start(),pPlate1->ucs.origin)>DISTANCE(pTube1->End(),pPlate1->ucs.origin))
			tube_len_vec[0]=-pTube1->ucs.axis_z;
		if(pTube2->pEnd==pBaseNode||DISTANCE(pTube2->Start(),pPlate2->ucs.origin)>DISTANCE(pTube2->End(),pPlate2->ucs.origin))
			tube_len_vec[1]=-pTube2->ucs.axis_z;
	}
	else
	{
		//TODO:�ǲ��ǿ�����pPlate1->GetBoltIntersPos����ȡ��
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
		//�����и��淨��
		cut_plane_norm=tube_len_vec[0]+tube_len_vec[1];
		RotateVectorAroundVector(cut_plane_norm,1,0,tube_ucs_arr[0].axis_z);
	}
	else 
		cut_plane_norm=pTube1->ucs.axis_z;
	f3dPoint spreadVec1=spreadVecArr[0],spreadVec2=spreadVecArr[1];
	vector_trans(spreadVec1,tube_ucs_arr[0],FALSE);
	vector_trans(spreadVec2,tube_ucs_arr[1],FALSE);
	if(spreadVec1*spreadVec2>EPS_COS)
		return FALSE;	//���ְ干�治��Ҫ�ϲ�Ϊ�۵���
	//2.���ݴ��ϲ��ְ幹�����
	//2.1 ������󽻼��㽻�㣬����һ������ȡ����������
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
	//2.2 ��1��2�Ÿְ��Ϸֱ���ȡһ����������߽Ǹ�,������巨�߷���
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
	//2.3 ����������ĸ�������
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
	//3.����ְ�
	UCS_STRU plate_ucs;
	plate_ucs.origin=0.5*(vertex_arr[1]+vertex_arr[0]);
	plate_ucs.axis_z=plate_norm;
	plate_ucs.axis_x=btm_edge_vec;
	plate_ucs.axis_y=plate_ucs.axis_z^plate_ucs.axis_x;
	normalize(plate_ucs.axis_y);
	plate_ucs.axis_x=plate_ucs.axis_y^plate_ucs.axis_z;
	normalize(plate_ucs.axis_x);
	if(plate_ucs.axis_y*spreadVecArr[0]<0)
	{	// ��֤�ϲ�֮��ĸְ�Y�᷽����ԭ�ְ����췽��һ��
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
	//���ڶ���ְ�������������������б���
	top_edge_line1=AddPlateVertexsToList(vertex_list,vertex_arr,plate_ucs,2,pPlate2,keyVertexPtr[1],reserveOrderArr[1]);
	PROFILE_VERTEX *pTopVertex1=vertex_list.append((PROFILE_VERTEX(vertex_arr[2])));
	PROFILE_VERTEX *pTopVertex2=vertex_list.append((PROFILE_VERTEX(vertex_arr[3])));
	//����һ��ְ�������������������б���
	top_edge_line2=AddPlateVertexsToList(vertex_list,vertex_arr,plate_ucs,3,pPlate1,keyVertexPtr[0],reserveOrderArr[0]);
	BOOL bDeleteLine1End=FALSE,bDeleteLine2Start=FALSE;
	f3dLine top_edge_line(top_edge_line1.startPt,top_edge_line2.endPt);
	double fLine1EndState=DistOf2dPtLine(top_edge_line1.endPt,top_edge_line.startPt,top_edge_line.endPt);
	double fLine2StartState=DistOf2dPtLine(top_edge_line2.startPt,top_edge_line.startPt,top_edge_line.endPt);
	if( fLine1EndState<0&&fLine2StartState<0)	//�ڶ����������Ҳ�
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
	//ɾ����top_edge_line1.endPt\top_edge_line2.startPt��Ӧ��������
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if( pVertex->vertex.feature<10&&	//��ɾ��������
			((bDeleteLine1End&&pVertex->vertex.IsEqual(top_edge_line1.endPt))||
			 (bDeleteLine2Start&&pVertex->vertex.IsEqual(top_edge_line2.startPt))))
			vertex_list.DeleteCursor(FALSE);
	}
	vertex_list.Clean();
	//���»������յ�����
	int nRetCode=Int3dll(top_edge_line,f3dLine(vertex_arr[0],vertex_arr[3]),vertex_arr[3]);
	if(nRetCode!=0&&nRetCode!=-1)
		pTopVertex2->vertex=vertex_arr[3];
	nRetCode=Int3dll(top_edge_line,f3dLine(vertex_arr[1],vertex_arr[2]),vertex_arr[2]);
	if(nRetCode!=0&&nRetCode!=-1)
		pTopVertex1->vertex=vertex_arr[2];

	CUndoOperObject undo(&Ta);
	//4.���øְ����
	//4.1 ���¸ְ���Ʋ���
	pPlate1->designInfo.iFaceType=6;
	pPlate1->jdb_style=1;
	pPlate1->designInfo.m_bEnableFlexibleDesign=TRUE;
	pPlate1->designInfo.iProfileStyle0123=1;
	//���߸˼�
	for(CDesignLjPartPara *pLjPara=pPlate1->GetFirstLjPara();pLjPara;pLjPara=pPlate1->GetNextLjPara())
		pLjPara->iFaceNo=3;
	for(CDesignLjPartPara *pLjPara=pPlate2->GetFirstLjPara();pLjPara;pLjPara=pPlate2->GetNextLjPara())
	{
		CDesignLjPartPara *pNewLjPara=pPlate1->designInfo.partList.SetValue(pLjPara->hPart,*pLjPara);
		pNewLjPara->iFaceNo=2;
	}
	//���ߡ������淨�ߡ�������
	for(int i=0;i<2;i++)
	{	//�����淨��
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
		//������
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

	//4.2 ���¸ְ���������
	pPlate1->vertex_list.Empty();
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		pPlate1->vertex_list.append(*pVertex);
	//�����˨����
	for(CLsRef *pLsRef=pPlate2->GetFirstLsRef();pLsRef;pLsRef=pPlate2->GetNextLsRef())
		pPlate1->AppendLsRef(*pLsRef);
	pPlate1->ucs=plate_ucs;
	pPlate1->face_N=3;
	pPlate1->jdb_style=1;	//�۵���
	pPlate1->DesignPlate();
	pPlate1->SetModified();
	pPlate1->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pPlate1->GetSolidPartObject());
	//ɾ���ڶ���ְ�
	g_pSolidDraw->DelEnt(pPlate2->handle);
	console.DispPartSet.DeleteNode(pPlate2->handle);
	console.DeletePart(pPlate2->handle);
	//
	g_pSolidDraw->Draw();
	return TRUE;
}
int CLDSView::MergePlates()
{	//���������Ӱ�ϲ�Ϊһ�����Ӹְ�
	CString cmdStr,promptStr;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSPlate *pFirstPlate=NULL,*pSecondPlate=NULL;
	CUndoOperObject undo(&Ta,true);
	try
	{
		//ѡ����Ҫ�ϲ��ĵ�һ��ְ�
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
			pCmdLine->FillCmdLine("MergePlates ��ѡ����Ҫ�ϲ��ĵ�һ�����Ӹְ�:","");			
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
		//ѡ����Ҫ�ϲ��ĵڶ���ְ�
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("MergePlates Please select the second connecting plate to be combined:","");
#else
		pCmdLine->FillCmdLine("MergePlates ��ѡ����Ҫ�ϲ��ĵڶ������Ӹְ�:","");		
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
			throw "No permission to modify the connecting plates��";
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pSecondPlate->dwPermission))
			throw "No permission to modify the connecting plates��";
#else
			throw "���ܺϲ�������������Ӹְ�";
		else if(pFirstPlate->cMaterial!=pSecondPlate->cMaterial)
			throw "�����Ӱ�İ���ʲ�ͬ,�޷��ϲ�";
		else if(pFirstPlate->GetThick()!=pSecondPlate->GetThick())
			throw "�����Ӱ�İ��ͬ,�޷��ϲ�";
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pFirstPlate->dwPermission))
			throw "û�д����Ӱ���޸�Ȩ�ޣ�";
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pSecondPlate->dwPermission))
			throw "û�д����Ӱ���޸�Ȩ�ޣ�";
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
	pCmdLine->FillCmdLine("����:","");
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
		BOOL bOverturnSecPlate=(dd<0);	//��ת�ڶ���ְ������� wht 16-09-28
		if(fabs(dd)<cos(RADTODEG_COEF*0.3))	//cos0.3��= 0.999986292247  
		{	//�����׼�治���棬�ϲ���������һ˫����ϲ�һ�������һ˫���Ϊ�����
			//1.�����ְ�Ľ���(��������)
			f3dPoint inters_pick,inters_line_vec;
			if(pFirstPlate->face_N<pSecondPlate->face_N)	//
			{	//��֤�ڶ����Ϊ����壬����һ��嵥��˫�������Ϊ�����ϲ�����ĸ��ԭ��
				CLDSPlate *pPlate=pFirstPlate;
				pFirstPlate=pSecondPlate;
				pSecondPlate=pPlate;
			}
			else if(pFirstPlate->face_N>1&&pSecondPlate->face_N>1)
#ifdef AFX_TARG_ENU_ENGLISH
				throw " two multiple-plane plates with different basic plane can not be combined,";
#else
				throw "����ͬʱ�ϲ���������治һ�µĶ����,";
#endif
			if(pFirstPlate->face_N!=pSecondPlate->face_N)	//�۵���
			{
				pFirstPlate->jdb_style=1;	//֮ǰ�۵��帳ֵΪ3�Ǵ�Ļ���ֹܼа����������������Ϊ1��������IsFoldPlate()���� wjh-2016.1.09
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
			bend_axis_vec=inters_line_vec.normalized();	//���������ת�᷽��
			PROFILE_VERTEX *pVertex,*pPrevVertex;
			//����ְ弸�����������жϰ����췽�� wht 10-09-28
			//face_pick1.Set(); face_pick2.Set(); �˴��������ʰȡ�㣬�±߿��ܼ���ʹ��
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
			normalize(extend_vec1); //�˴�Ӧ�õ�λ�����±�Ҫ��������cosֵ
			normalize(extend_vec2);
			extend_vec1-=(extend_vec1*bend_axis_vec)*bend_axis_vec;	//ת��Ϊ��������ת�ᴹֱ�İ������췽��
			extend_vec2-=(extend_vec2*bend_axis_vec)*bend_axis_vec;	//ת��Ϊ��������ת�ᴹֱ�İ������췽��
			normalize(extend_vec1);
			normalize(extend_vec2);
			extend_vec1*=-1.0;
			f3dPoint verify_axis_vec=extend_vec1^extend_vec2;
			extend_vec1*=-1.0;
			//
			pFirstPlate->face_N++;
			pFirstPlate->designInfo.huoqufacenorm[pFirstPlate->face_N-2]=pSecondPlate->designInfo.norm;
			if(verify_axis_vec*bend_axis_vec<0)	//�����Ӱ������������ۣ��轫�ڶ��鵥��������淴��
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
				pFirstPlate->designInfo.iFaceType=4;	//�۵���
			pFirstPlate->huoqufacenorm[pFirstPlate->face_N-2]=pSecondPlate->ucs.axis_z;
			pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].startPt=inters_pick-inters_line_vec*200;
			pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].endPt=inters_pick+inters_line_vec*200;
			coord_trans(pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].startPt,pFirstPlate->ucs,FALSE);
			coord_trans(pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].endPt,pFirstPlate->ucs,FALSE);
			pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].startPt.z=0;
			pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].endPt.z=0;
			//2-4Ϊ�Ľ��ĺϲ����Ӱ��㷨 wht 12-06-30
			//2.�ڵ�һ��������Ϸֱ���������������ı�
			f3dLine huoqu_line(inters_pick-inters_line_vec*200,inters_pick+inters_line_vec*200);
			f3dPoint startPerp[2],endPerp[2];	//��������
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
				throw "δ�ҵ�������߽Ͻ��ıߣ��ϲ��ְ�ʧ�ܣ�";
#endif
			//3.���������������(�����ĸ�ͶӰ������������ĵ���Ϊ������)
			f3dPoint verfiy_vec=endPerp[0]-startPerp[0];
			int sign=1;	//Ĭ�ϻ����߷��������ͶӰ�߷���һ��
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
			//4.���ڶ����Ķ�����ӵ���һ��ְ嶥���б���
			//4.1���ɵڶ���ְ�����Ҫ���������������򼯺�(��pEndVertex[1]����һ���㿪ʼ���)
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
			//4.2���ڶ������������ӵ���һ��ְ���
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
						if(pVertex==NULL)	//pStartVertext[0]Ϊ���һ��������
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
							if(pTepmVertex->type==3)	//��Բ��
							{
								pNewVertex->center = pTepmVertex->center;
								pNewVertex->column_norm = pTepmVertex->column_norm;
							}
						}
					}
					break;
				}
			}
			//�ϲ����Ӱ���㷨
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
				//ȥ������������֮��Ĳ������
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
				PROFILE_VERTEX *pVertexTemp;//�洢Բ���յ㣬���Բ���յ�
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
				//ȥ������������֮��Ĳ������
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
					if(pVertex->type==3)	//��Բ��
					{
						pNewVertex->center = pVertex->center;
						pNewVertex->column_norm = pVertex->column_norm;
					}
				}
				pNewVertex->rayVecX = 0;//vector.x;
				pNewVertex->rayVecY = 0;//vector.y;
			}
			*/
			//������Ʋ�����ֵ���������⣩
			/*pFirstPlate->designInfo.huoqufacenorm[pFirstPlate->face_N-2].vector = pFirstPlate->huoqufacenorm[pFirstPlate->face_N-2];
			pFirstPlate->designInfo.huoquline_start[pFirstPlate->face_N-2].datum_pos_style = 0;
			pFirstPlate->designInfo.huoquline_start[pFirstPlate->face_N-2].pos = pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].startPt;
			pFirstPlate->designInfo.huoquline_end[pFirstPlate->face_N-2].datum_pos_style = 0;	
			pFirstPlate->designInfo.huoquline_end[pFirstPlate->face_N-2].pos = pFirstPlate->HuoQuLine[pFirstPlate->face_N-2].endPt;
			pFirstPlate->designInfo.huoquline_start[pFirstPlate->face_N-2].UpdatePos(console.GetActiveModel());
			pFirstPlate->designInfo.huoquline_end[pFirstPlate->face_N-2].UpdatePos(console.GetActiveModel());*/
			for(pLjPara=pSecondPlate->GetFirstLjPara();pLjPara;pLjPara=pSecondPlate->GetNextLjPara())
				pLjPara->iFaceNo++;	//�����棨�ڶ��棩�����ж�����ţ���
		}
		else if(pFirstPlate->face_N==pSecondPlate->face_N)
		{
			for(i=0;i<pFirstPlate->face_N-1;i++)
			{
				if(pFirstPlate->huoqufacenorm[i]*pSecondPlate->huoqufacenorm[i]<EPS_COS)
#ifdef AFX_TARG_ENU_ENGLISH
					throw "bending plane normal of the two connecting plates are not consistent, cannot be combined";
#else
					throw "�����Ӱ�Ļ����淨�߲�һ��,�޷��ϲ�";
#endif
			}
			if(dd<0)	//�����Ӱ������������ۣ��轫�ڶ��鵥��������淴��
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
						throw "�ҵ�����������,���ݴ���";
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
						continue;	//�����㲻�����
					else
					{
						//�ڻ�����ĩ����
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
					if(pCurPt->type==3)	//��Բ��
					{
						pPt->center = pCurPt->center;
						pPt->column_norm = pCurPt->column_norm;
					}
				}
			}
			//����������Ӱ������
			pFirstPlate->DesignProfile();
		}
		else
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Not support the combination of singular-plane plate and double-plane plate which shares the same baisc plane!";
#else
			throw "��֧�ֻ�׼����ͬ�ĵ������˫���ϲ�!";
#endif
		//��˨����
		for(pLsRef=pSecondPlate->GetFirstLsRef();pLsRef;pLsRef=pSecondPlate->GetNextLsRef())
		{
			//�����ڶ���ְ��ϵ���˨ͨ��
			(*pLsRef)->ReplaceReferenceHandle(CHandleReplacer(pSecondPlate->handle,pFirstPlate->handle),TRUE);
			pFirstPlate->AppendLsRef(*pLsRef);
		}
		//���Ӹ˼��б�	wht 10-03-02
		CHashList<CDesignLjPartPara> hashLjPartByHandle;
		for(pLjPara=pFirstPlate->GetFirstLjPara();pLjPara;pLjPara=pFirstPlate->GetNextLjPara())
		{
			if(hashLjPartByHandle.GetValue(pLjPara->hPart)==NULL)
				hashLjPartByHandle.SetValue(pLjPara->hPart,*pLjPara);		
		}
		for(pLjPara=pSecondPlate->GetFirstLjPara();pLjPara;pLjPara=pSecondPlate->GetNextLjPara())
		{
			if(hashLjPartByHandle.GetValue(pLjPara->hPart)==NULL)	//��ֹ�ظ����ͬһ���Ӹ˼�
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
			{	//�������ϲ�����˫���ʱ,����ڶ���ְ����һ��ְ������治ͬ��
				//���û�����ʰȡ��ʱ�����÷���ƫ����,����ϲ����֮��������ƻ�����λ�÷�����λ wht 16-09-28
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
			pFirstPlate->jdb_style=0;	//ͨ���ϲ��ְ��������ɵİ��Ϊ��ͨ�ְ�(��˨���ϲ�֮��jdb_styleӦ��Ϊ0)
			if( pFirstPlate->designInfo.m_bEnableFlexibleDesign&&
				pSecondPlate->designInfo.m_bEnableFlexibleDesign&&pSecondPlate->jdb_style==0&&
				pFirstPlate->designInfo.huoquline_start[0].UpdatePos(&Ta)&&
				pFirstPlate->designInfo.partList.GetNodeNum()>0&&
				pSecondPlate->designInfo.partList.GetNodeNum()>0)
			{	//ͨ���ϲ��ְ��������ɵĸְ�Ĭ�϶�������������� wht 16-12-02
				pFirstPlate->designInfo.m_bSelfCorrectBendline=true;
				pFirstPlate->designInfo.m_bEnableFlexibleDesign=false;
				pFirstPlate->DesignPlate();
			}
		}
		pFirstPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pFirstPlate->GetSolidPartObject());
		//����������
		/*	�ⲿ�ִ����Ǻ�����ǰд�ģ���Ҫ����ȷ��ԭʼ�����ߵ����˶���λ�ñ��ڻ�ͼ��������߷��������޹أ��ֶ�������������ת���������ƴ��봦��
			����ⲿ�ִ����������壬���һ���Ժϲ��ɵ��۵�������������������ߣ����Ҵ���wjh-2012.6.30
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
		//����Ƿ�����Ҫ�ϲ��ĸְ���صĲ�������
		CLDSParamPlate* pParamPlate=NULL;
		PARTSET param_plate_set;	//�����弯�ϴ�����ʱѯ���û��Ƿ����������ز�����
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
				pParamPlate->GetDesignItemValue('E',&hFirstPlate);	//��һ���׼�ְ���B��ΪE wht 11-05-13
				pParamPlate->GetDesignItemValue('F',&hSecondPlate);	//�ڶ����׼�ְ���C��ΪF wht 11-05-13
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
			if(AfxMessageBox("��ϲ��ְ���صĲ���������Ҫ�������,Y(�ֶ�ת��Ϊ��ͨ�ְ���������)/N(�Զ���������)��",MB_YESNO)!=IDYES)
#endif
				bAdjustPlate=TRUE;
		}
		if(bAdjustPlate)
		{	//�õ�һ��ְ�ľ���滻�ڶ���ְ�ľ�� ��Ҫָ�Ĳ��������Ʋ���
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
				{	//�����߰�
					//��һ���׼�ְ���B��ΪE �ڶ����׼�ְ���C��ΪF wht 11-05-13
					pParamPlate->GetDesignItemValue('E',&hFirstPlate);
					pParamPlate->GetDesignItemValue('F',&hSecondPlate);
					if(hFirstPlate==pSecondPlate->handle)
#ifdef AFX_TARG_ENU_ENGLISH
						pParamPlate->SetDesignItemValue('E',pFirstPlate->handle,"the first positioning datum plate",PARAM_POS,1);
					if(hSecondPlate==pSecondPlate->handle)
						pParamPlate->SetDesignItemValue('F',pFirstPlate->handle,"the second positioning datum plate",PARAM_POS,1);
#else
						pParamPlate->SetDesignItemValue('E',pFirstPlate->handle,"��һ�鶨λ��׼�ְ�",PARAM_POS,1);
					if(hSecondPlate==pSecondPlate->handle)
						pParamPlate->SetDesignItemValue('F',pFirstPlate->handle,"�ڶ��鶨λ��׼�ְ�",PARAM_POS,1);
#endif
				}
				else if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				{	//������
					pParamPlate->GetDesignItemValue('F',&hFirstPlate);
					pParamPlate->GetDesignItemValue('G',&hSecondPlate);
					UCS_STRU old_ucs=pParamPlate->ucs;	//��¼�޸Ĳ���ǰ������ϵ
					if(hFirstPlate==pSecondPlate->handle)
#ifdef AFX_TARG_ENU_ENGLISH
						pParamPlate->SetDesignItemValue('F',pFirstPlate->handle,"the first positioning datum plate",PARAM_POS,1);
					if(hSecondPlate==pSecondPlate->handle)
						pParamPlate->SetDesignItemValue('G',pFirstPlate->handle,"the second positioning datum plate",PARAM_POS,1);
#else
						pParamPlate->SetDesignItemValue('F',pFirstPlate->handle,"��һ�鶨λ��׼�ְ�",PARAM_POS,1);
					if(hSecondPlate==pSecondPlate->handle)
						pParamPlate->SetDesignItemValue('G',pFirstPlate->handle,"�ڶ��鶨λ��׼�ְ�",PARAM_POS,1);
#endif
					/*
					//�޸Ĳ�����������Ƹ����¾�����ϵX��֮��ļнǵ�������
					pParamPlate->SetDesignItemValue('A',0,"�����ת�Ƕ�");
					pParamPlate->DesignPlate();
					//����������ϵX��֮��ļн�
					vector_trans(old_ucs.axis_x,pParamPlate->ucs,FALSE);
					double offset_angle=Cal2dLineAng(f3dPoint(0,0,0),old_ucs.axis_x)*DEGTORAD_COEF;
					if(offset_angle>180)
						offset_angle-=180;
					if(offset_angle>eps)
						pParamPlate->SetDesignItemValue('A',offset_angle,"�����ת�Ƕ�");
					*/
				}
				else if(pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
				{	//��Խʽ�ֹ�����
					//��һ���׼�ְ���B��ΪE �ڶ����׼�ְ���C��ΪF wht 11-05-13
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
							pParamPlate->SetDesignItemValue('E',0,"��һ�鶨λ��׼�ְ�",PARAM_POS,1);
						else
							pParamPlate->SetDesignItemValue('E',pFirstPlate->handle,"��һ�鶨λ��׼�ְ�",PARAM_POS,1);
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
							pParamPlate->SetDesignItemValue('F',0,"�ڶ��鶨λ��׼�ְ�",PARAM_POS,1);
						else
							pParamPlate->SetDesignItemValue('F',pFirstPlate->handle,"�ڶ��鶨λ��׼�ְ�",PARAM_POS,1);
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
		//ɾ���ڶ���ְ�
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
	m_sPrevCommandName="�ظ��ڵ�����";
#endif
	Command("DesignSphere");
}

int CLDSView::DesignSphere()	
{	//�������
	if(!PRODUCT_FUNC::IsHasColumnRayRod())//�������Ȩ��֧�������������߸˼�����!
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The software license does not support the function to generate spherical node!");
#else
		AfxMessageBox("�������Ȩ��֧���������νڵ㹦��!");
#endif
		return 0;
	}
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	UCS_STRU ucs;
	CString cmdStr,promptStr;
	CLDSNode *pBaseNode=NULL;
	//�л�������ͼ״̬����ѡ��ڵ�
	g_pSolidSet->SetDisplayType(DISP_LINE);
	Invalidate(FALSE);
	g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select sphere's assembling locating node:","");
#else
	pCmdLine->FillCmdLine("��ѡ�������װ�䶨λ�ڵ�:","");
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
				pCmdLine->FillCmdLine("������ѡ�������װ�䶨λ�ڵ�:","");
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
	//�л���ʵ����ʾ״̬
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
	m_sPrevCommandName="�ظ����θ˼�";
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
	pCmdLine->FillCmdLine("ArcPart ָ�����θ˼������:","");
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
		pCmdLine->FinishCmdLine(CXhChar16("���<0x%X>",pStartNode->handle));
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(pStartNode->handle);
	PEEKCMDLINE cmd_detect("N",pCmdLine);
	pCmdLine->FillCmdLine("ArcPart ָ�����θ˼����յ��[ƽ�淨��(N)]:","");
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
	{	//ָ���յ�
		pCmdLine->FinishCmdLine(CXhChar16("�յ�<0x%X>",pEndNode->handle));
		arcparam.type=0;	//����Բ��ʼĩ�˵㴴��Բ��
		GEPOINT arcvec=pEndNode->Position()-pStartNode->Position();
		double halfarclen=arcvec.mod()*0.5;
		pCmdLine->FillCmdLine("ArcPart ָ�����θ˼���Բ���뾶��[Բ��ʰȡ��(C)]:","");
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
			pCmdLine->FinishCmdLine(CXhChar16("Բ��ʰȡ��<0x%X>",pCenterPickNode->handle));
		else
			pCmdLine->FinishCmdLine();
		//if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"C"))
		//	return FALSE;
		if(pCenterPickNode|| toupper(cmdStr[0])=='C')
		{	//ָ��Բ��
			while(true)
			{
				if(pCenterPickNode==NULL)
				{
					pCmdLine->FillCmdLine("ArcPart ָ�����θ˼���Բ��ʰȡ������:","");
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
					pCmdLine->FillCmdLine("ָ��Բ��ʰȡ����Ч,������ָ��:","");
					arcparam.pCenterPickNode=NULL;
					continue;
				}
				vec1/=dist1;
				vec2/=dist2;
				if(fabs(dist1-dist2)<1&&(arcvec*vec1)>0&&(arcvec*vec2)<0)
					arcparam.radius=ftoi(dist1);
				else if(fabs(vec1*vec2)>EPS_COS)
				{
					pCmdLine->FillCmdLine("ָ��Բ��ʰȡ����Ч,������ָ��:","");
					arcparam.pCenterPickNode=NULL;
					continue;
				}
				worknorm=vec1^vec2;
				break;
			}
			pCmdLine->FillCmdLine((char*)CXhChar50("ArcPart ָ�����θ˼���Բ���뾶<%.0f>:",arcparam.radius),"");
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				return FALSE;
			if(cmdStr.GetLength()>0)
				arcparam.radius=atof(cmdStr);
			normalize(worknorm);
			int iVecType=CStdVector::GetVectorType(worknorm);
			pCmdLine->FillCmdLine((char*)CXhChar100("���θ˼�����ƽ�淨��[0.�Զ���|1.X+|2.X-|3.Y+|4.Y-|6.Z+|6.Z-] <%d>:",iVecType),"");
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
		{	//ָ��Բ���뾶
			radius =atof(cmdStr);
			while(radius<halfarclen)
			{
				pCmdLine->FillCmdLine("ArcPart ָ�����θ˼��İ뾶С���ҳ���һ��, ������ָ��:","");
				if(!pCmdLine->GetStrFromCmdLine(cmdStr))
					return FALSE;
				radius =atof(cmdStr);
			}
			arcparam.radius=radius;
			pCmdLine->FillCmdLine("ArcPart ָ�����θ˼�����ƽ���ڵ���һʰȡ���[ƽ�淨��(N)]:","");
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
					pCmdLine->FillCmdLine((char*)CXhChar100("���θ˼�����ƽ�淨��[0.�Զ���|1.X+|2.X-|3.Y+|4.Y-|6.Z+|6.Z-] <6>:"),"");
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
					pCmdLine->FinishCmdLine(CXhChar16("ƽ��ʰȡ��<0x%X>",pPlanePickNode->handle));
				}
			}
			if(pCenterPickNode==NULL)
			{
				pCmdLine->FillCmdLine("ArcPart ָ�����θ˼���Բ��ʰȡ��<X=0,Y=0,Z=0>:","");
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
					pCmdLine->FinishCmdLine(CXhChar16("�ڵ�<0x%X>",pCenterPickNode->handle));
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
	{	//ָ��ƽ�淨��
		pCmdLine->FinishCmdLine("N");
		pCmdLine->FillCmdLine("���θ˼�����ƽ�淨��[0.�Զ���|1.X+|2.X-|3.Y+|4.Y-|6.Z+|6.Z-] <6>:","");
		if(pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"0|1|2|3|4|5|6")<0)
			return FALSE;
		arcparam.worknorm=worknorm;
		if(cmdStr.GetLength()>0&&cmdStr[0]>'0'&&cmdStr[0]<='6')
			arcparam.worknorm=worknorm=CStdVector::GetVector(cmdStr[0]-'0');
		pCmdLine->FillCmdLine("ArcPart ָ�����θ˼������ν� <60>:","");
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
	cmdStr.Format("ArcPart �˼�����[�Ǹ�(L)/���(B)/�۸�(C)]<%C>:",cDefaultType);
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
	if(g_sysPara.iColorSchema==1)		//TSA��ɫ����
		pen=g_pSolidSet->SetPen(g_sysPara.crTsaMode.crPrimaryLine,PS_SOLID,1);
	else if(g_sysPara.iColorSchema==2)	//������ɫ����
		pen=g_pSolidSet->SetPen(g_sysPara.crMaterialMode.crLine,PS_SOLID,1);
	else								//������ɫ����
		pen=g_pSolidSet->SetPen(g_sysPara.crPartMode.crLine,PS_SOLID,1);
	CLDSArcPart *pArcPart = NULL;
	CUndoOperObject undo(&Ta,true);
	if(arcPartDlg.m_iPartType==0)
	{	//�Ǹ�
		CLDSArcAngle *pArcAngle=(CLDSArcAngle*)console.AppendPart(CLS_ARCANGLE);
		pArcPart=pArcAngle;
		restore_JG_guige(arcPartDlg.m_sPartGuiGe,pArcAngle->wing_wide,pArcAngle->wing_thick);
		pArcAngle->offset_h=arcPartDlg.m_fOffsetH;
		pArcAngle->offset_v=arcPartDlg.m_fOffsetV;
		pArcAngle->m_siLayoutStyle=(short)arcPartDlg.m_iLayoutStyle;	//�ڷŷ�ʽ
	}
	else if(arcPartDlg.m_iPartType==1)
	{	//�۸�
		CLDSArcSlot *pArcSlot=(CLDSArcSlot*)console.AppendPart(CLS_ARCSLOT);
		pArcPart = pArcSlot;
		_snprintf(pArcSlot->spec,MAX_CHAR_GUIGE_16,"%s",arcPartDlg.m_sPartGuiGe);
		//���ݹ����Ҳ۸�
		SLOT_PARA_TYPE *pSlotPara=FindSlotType(CXhChar50(arcPartDlg.m_sPartGuiGe));
		pArcSlot->height=pSlotPara->h;	//�߶�
		pArcSlot->wide=pSlotPara->b;	//���
		pArcSlot->thick=pSlotPara->d;	//���
		pArcSlot->m_siLayoutStyle=(short)arcPartDlg.m_iLayoutStyle;	//�ڷŷ�ʽ	
		pArcSlot->SetPartNo(arcPartDlg.m_sPartNo.GetBuffer(0));
		pArcSlot->iSeg=SEGI(arcPartDlg.m_sSegI.GetBuffer());
		pArcSlot->offset_h=arcPartDlg.m_fOffsetH;
		pArcSlot->offset_v=arcPartDlg.m_fOffsetV;
	}
	else if(arcPartDlg.m_iPartType==2)
	{	//���
		CLDSArcFlat *pArcFlat=(CLDSArcFlat*)console.AppendPart(CLS_ARCFLAT);
		pArcPart=pArcFlat;
		restore_JG_guige(arcPartDlg.m_sPartGuiGe,pArcFlat->wide,pArcFlat->thick);
		pArcFlat->offset_h=arcPartDlg.m_fOffsetH;
		pArcFlat->offset_v=arcPartDlg.m_fOffsetV;
		pArcFlat->m_siLayoutStyle=0;	//�ڷŷ�ʽ
	}
	/*else if(arcPartDlg.m_iPartType==3)
	{	//�ֹ�
		
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
	{	//�������Բ�뾶
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
{	//��ΰ˵��������Ҫ����������б�߰壬����ȥ��wjh-2016.12.16
	m_nPrevCommandID=ID_ROTATE_PART_AROUND_AXIS;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat rotating coordinate system";
#else
	m_sPrevCommandName="�ظ���ת����ϵ";
#endif
	Command("RotatePartAroundAxis");
}
int CLDSView::RotatePartAroundAxis()
{
	CString cmdStr;
	double fAngle = 0;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//ѡ����Ҫ��ת�Ĺ���
	CLDSPlate *pPlate=NULL;	//Ҫ��ת�ĸְ�
	//����ȷ����ת��ĸְ����Ϊ�������ְ�
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
		pCmdLine->FillCmdLine("RotatePartAroundAxis ��ѡ��Ҫ������ת�ĸְ�:","");
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
	pCmdLine->FillCmdLine("RotatePartAroundAxis ��ָ����ת�ְ�ʱ����ת��<0.X��|1.Y��|2.Z��|3.ָ���ο��ְ�>[3]:","");
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
			pCmdLine->FillCmdLine("RotatePartAroundAxis ��Ҫѡ��ؼ���","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("RotatePartAroundAxis ��ָ����ת����ϵʱ��������[X/Y/Z]<X>:","");
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
		pCmdLine->FillCmdLine("RotatePartAroundAxis ��ѡ������ȷ����ת��ĸְ�:","");
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
	pCmdLine->FillCmdLine("RotatePartAroundAxis ��������ת�Ƕ�<90>:","");
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
			pCmdLine->FillCmdLine("RotatePartAroundAxis ��Ҫѡ��ؼ���","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("RotatePartAroundAxis ��������ת�Ƕ�<90>:","");
#endif
			continue;
		}
	}
	fAngle*=RADTODEG_COEF;
	part_ucs=pPlate->ucs;
	if(nFlag==0)
	{	//��X����ת
		RotateVectorAroundVector(part_ucs.axis_y, fAngle, part_ucs.axis_x);
		normalize(part_ucs.axis_y);
		part_ucs.axis_z = part_ucs.axis_x^part_ucs.axis_y;
		normalize(part_ucs.axis_z);
	}
	else if(nFlag==1)
	{	//��Y����ת
		RotateVectorAroundVector(part_ucs.axis_x, fAngle, part_ucs.axis_y);
		normalize(part_ucs.axis_x);
		part_ucs.axis_z = part_ucs.axis_x^part_ucs.axis_y;
		normalize(part_ucs.axis_z);
	}
	else if(nFlag==2)
	{	//��Z����ת
		RotateVectorAroundVector(part_ucs.axis_y, fAngle, part_ucs.axis_z);
		normalize(part_ucs.axis_y);
		part_ucs.axis_x = part_ucs.axis_y^part_ucs.axis_z;
		normalize(part_ucs.axis_x);
	}
	else if(pRefParamPlate||pRefPlate)
	{	//�����ְ��ཻ��Ϊ��ת��
		UCS_STRU ref_ucs;
		double offset_dist=0;
		f3dPoint face1_pick,face2_pick,inters_pick,inters_line_vec;
		if(pRefParamPlate)
			ref_ucs=pRefParamPlate->ucs;
		else if(pRefPlate)
			ref_ucs=pRefPlate->ucs;
		//�ж�Ҫ��ת�ĸְ��ڻ�׼�ְ嶥�滹�ǵ���һ��
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
		if(((int)pPlate->m_fNormOffset)==0)	//�˴�֮ǰ�����б��� wjh-2019.8.26
			face2_pick=pPlate->ucs.origin+pPlate->ucs.axis_z*(0.5*pPlate->GetThick());
		int nRetCode=Int3d2f(&inters_pick,&inters_line_vec,face2_pick,
			pPlate->ucs.axis_z,face1_pick,ref_ucs.axis_z);
		if(nRetCode==1)
		{	//�����ְ�����ϵ,����ת��ΪX��
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
			//������ԭ��ͶӰ���ཻ����
			SnapPerp(&perp,axis_line,part_ucs.origin);
			part_ucs.origin=perp;
			//�����ְ�����ϵ�󽫸ְ�����������ת����������ϵ��
			PROFILE_VERTEX *pVertex=NULL;
			for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			{
				f3dPoint vertex=pVertex->vertex;
				coord_trans(vertex,pPlate->ucs,TRUE);
				coord_trans(vertex,part_ucs,FALSE);
				pVertex->vertex=vertex;
			}
			//�������������ϵ��ֵ���ְ�����ϵ
			pPlate->ucs=part_ucs;
			//��X����תָ���Ƕ�
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
			AfxMessageBox("��ת�ְ�ʧ��!");
#endif
			return 0;
		}
	}
	BOOL bCreateNewPart=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("RotatePartAroundAxis generate new parts through rotating coordinate system(Y/N)<N>:","");
#else
	pCmdLine->FillCmdLine("RotatePartAroundAxis ͨ����ת����ϵ�����¹���(Y/N)<N>:","");
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
				pCmdLine->FillCmdLine("RotatePartAroundAxis �����������������ȷ���Ƿ�ͨ����ת����ϵ�����¹���(Y/N):","");
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
	{	//ͨ����ת��������ϵ�����¹���
		CLDSPlate *pNewPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
		long hNew=pNewPlate->handle;
		pPlate->CloneTo(*pNewPlate);
		pNewPlate->handle=hNew;
		pNewPlate->EmptyLsRef();		//�����˨����
		pNewPlate->relativeObjs.Empty();//��չ�������	
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
	g_sysPara.m_bDispWorkUcs = TRUE;	//����ϵͳ���öԻ���
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("����:","");
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
	m_sPrevCommandName="�ظ��и�ְ�";
#endif
	Command("CutPlate");
}
//<DEVELOP_PROCESS_MARK nameId="CLDSView::CutPlateByPlate">
//TODO:���ְ��и�ְ���һ�����һ��CutPlate.cpp�ļ�,���������ɼ���Ϊһ��������CCutPlateOper
struct  CUTSECTOR_3D
{
	f3dPoint corner;
	f3dPoint start_vec;
	f3dPoint end_vec;
	f3dPoint rot_axis;
};

struct EDGE_INTERS_STATE{
	short stateVertexStart,stateVertexEnd;	//0.��������;1.��������;2.�������߽���;3.����Ϊ���νǵ�
	short intersStartEdge;	//��������ʼ���ཻ(��Ч����),0.�޽���;1.����Ϊ�ڵ�;2.����Ϊֱ�߶˵�;3.����Ϊ���νǵ�
	short intersEndEdge;	//��������ֹ���ཻ(��Ч����),0.�޽���;1.����Ϊ�ڵ�;2.����Ϊֱ�߶˵�;3.����Ϊ���νǵ�
};
class CCutSector
{
public:
	static const int LINE_RIGHT =-1;	//�ǵ����ߵ��Ҳ�
	static const int ON_LINE	= 0;	//�ǵ������ϻ�ֱ�߶�������
	static const int LINE_LEFT	= 1;	//�ǵ����ߵ����
public:
	f2dPoint corner,edgeVecStart,edgeVecEnd;	//�ǵ㣬��ʼ������ֹ����
	EDGE_INTERS_STATE CalEdgeIntersState(f2dPoint lineStart,f2dPoint lineEnd,f2dPoint& inters1,f2dPoint& inters2);
	//����LINE_LEFT��LINE_RIGHT��ON_LINE
	int JustifyConerReferToLine(f2dPoint lineStart,f2dPoint lineEnd);
	//�жϿռ���Ƿ�������Ч������,0.������������;1.������������;2.�����α߽���;3.�����νǵ���
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
	//ͳһֱ�߶˵��ڽǵ������ǵ���ֱ�߶������
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
//�����Ƿ���ȫ�������������ڣ����������α߽��϶��㣩,0.������������;1.������������;2.�����α߽���;3.�����νǵ���
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
		return 3;	//�����νǵ���
	if(angle<-EPS||angle>max_angle+EPS)
		return 0;	//��������������
	else if(angle>EPS&&angle<max_angle-EPS)
		return 1;	//ȷ����ȫ�����������ڣ������߽磩
	else
		return 2;	//�����α߽���
	return TRUE;
}
//ͨ���и�ְ��ʼ����ά�и����Σ��ǵ㣬��ʼ������ת������ת�Ƕȣ�
static void Init3DCutSectorByPlate(CUTSECTOR_3D& sector_3d,CLDSPlate* pCutPlate,BOOL bTopFace,double fMinDist);
//������ά���������뵥����������ཻȷ����ά�������򣨽ǵ㣬��ʼ������ֹ����
static void InitCutSectorBy3DSector(CCutSector& cutSector,CUTSECTOR_3D sector_3d,UCS_STRU ucs);
//ͨ����ά���������и���
static int CutPlateBySector(CLDSPlate* pPlate,CCutSector cutSector,BOOL bPrompt/* =TRUE */);
//��˫����ֳ����������
static BOOL Split2PlanePlate(CLDSPlate* pPlate,CLDSPlate *plate_arr);
//�ϲ�������Ϊ˫���
static BOOL Combine2PlanePlate(CLDSPlate* pPlate,CLDSPlate* plate_arr);
//ͨ��ָ��˫����и�ְ�
BOOL CLDSView::CutPlateByPlate(CLDSPlate *pPlate,CLDSPlate *pDatumPlate,
								 BOOL bTopFace,double fMinDist,BOOL bPrompt/* =TRUE */,BOOL bSetWeldEdge/*=TRUE*/)
{
	if(pPlate->face_N==3)
	{
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Sorry, three-plane plate can not be cut temporarily!");
#else
			AfxMessageBox("�ܱ�Ǹ,Ŀǰ�ݲ��ָܷ������!");
#endif
		return FALSE;
	}
	if(pDatumPlate->face_N==3)
	{
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Sorry, three-plane plate is not supported temporarily to be cut into plates!");
#else
			AfxMessageBox("�ܱ�Ǹ,Ŀǰ�ݲ�֧��ʹ��������и�ְ�!");
#endif
		return FALSE;
	}
	if(pPlate->vertex_list.GetNodeNum()<3||pDatumPlate->vertex_list.GetNodeNum()<3)
		return FALSE;
	if(pDatumPlate->face_N==1)	//��׼�ְ�Ϊ�����ʱֱ�ӵ���ͨ��ƽ���и�ְ庯������
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
	Init3DCutSectorByPlate(sector_3d,pDatumPlate,bTopFace,fMinDist);	//�����и�˫���ȷ����ά�и�����
	if(pPlate->face_N==1)	//���и��pPlate�ǵ����	
	{
		CCutSector cutSector;
		InitCutSectorBy3DSector(cutSector,sector_3d,pPlate->ucs);	//������ά���κ͵����������ȷ���и��ά����
		if(CutPlateBySector(pPlate,cutSector,bPrompt)<2)	//ͨ�����������и���
			return FALSE;
		pPlate->SetModified();
		pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		g_pSolidDraw->Draw();
		return TRUE;
	}
	//���и����˫���,��˫���pPlate�ֳ������������д���
	CLDSPlate plate_arr[2];
	if(!Split2PlanePlate(pPlate,plate_arr))	//���˫���Ϊ���������
		return FALSE;
	int iRet,iRetDaum=-1,iRetHuoqu=-1;
	for(int i=0;i<2;i++)
	{
		CCutSector cutSector;
		InitCutSectorBy3DSector(cutSector,sector_3d,plate_arr[i].ucs);	//������ά���κ͵����������ȷ���и��ά����
		iRet=CutPlateBySector(&plate_arr[i],cutSector,FALSE);	//ͨ�����������и���
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
			AfxMessageBox("�ְ屻ȫ���и��!");
#endif
		return FALSE;
	}
	else if(iRetDaum==1&&iRetHuoqu==1)
	{
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("plates are not cut!");
#else
			AfxMessageBox("�ְ�û�б��и�!");
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
//��������н���ķ��ߣ��������淨�߽��еȾ�ƫ����ʱ���Ⱦ�ǵ��ʵ��ƫ��ʸ����wjh-2013.6.29
f3dPoint EquidistantCornerOffset(f3dPoint base_norm,f3dPoint other_norm,double offset_thick=1.0)
{
	f3dPoint offset_vec;
	if(!normalize(base_norm)||!normalize(other_norm))
		return offset_vec;
	f3dPoint bisector=base_norm+other_norm;
	if(!normalize(bisector))
		return offset_vec;
	double cosa=bisector*base_norm;	//ʵ��bisector*base_normֵ������Ӧ�ú���ڵ�0
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
	//1����ȡ������Ϣ
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
	//2�����ݻ�����ȷ����ά��������
	sector_3d.corner=huoqu_start;	//���νǵ�
	sector_3d.rot_axis=huoqu_vec;	//��ת�᷽��
	if(bTopFace)	//�����и�ʱ��ת�᷽��ı�
		sector_3d.rot_axis*=-1;
	f3dPoint datum_plane_vec=pCutPlate->ucs.axis_z^huoqu_vec;
	normalize(datum_plane_vec);
	sector_3d.start_vec=datum_plane_vec;	//��ʼ����
	f3dPoint huoqu_plane_vec=pCutPlate->huoqufacenorm[0]^(-huoqu_vec);
	normalize(huoqu_plane_vec);
	sector_3d.end_vec=huoqu_plane_vec;	//��ֹ����
}
void InitCutSectorBy3DSector(CCutSector& cutSector,CUTSECTOR_3D sector_3d,UCS_STRU ucs)
{
	f3dPoint corner,inters,start,start_vec,end,end_vec;
	//���νǵ�
	Int3dlf(corner,sector_3d.corner,sector_3d.rot_axis,ucs.origin,ucs.axis_z);
	coord_trans(corner,ucs,FALSE);
	corner.z=0;
	cutSector.corner.Set(corner.x,corner.y);
	//��ʼ����
	start=sector_3d.corner+sector_3d.start_vec*100;
	Int3dlf(inters,start,sector_3d.rot_axis,ucs.origin,ucs.axis_z);
	coord_trans(inters,ucs,FALSE);
	inters.z=0;
	start_vec=inters-corner;
	normalize(start_vec);
	//��ֹ����
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
//ͨ�����������и���:-1�и�ʧ�� 0ȫ���и� 1û���и� 2�и�ɹ� 
int CutPlateBySector(CLDSPlate* pPlate,CCutSector cutSector,BOOL bPrompt/* =TRUE */)
{
	if(pPlate->face_N!=1 || pPlate->vertex_list.GetNodeNum()<3)
		return -1;
	bool cornerInserted=false;
	bool bVertexModify=false;
	//1 ������������ȡ��ְ嶥�㣬�����������ɶ���
	int iFaceNo=10;
	PROFILE_VERTEX *pCurVertex=NULL,*pPrevVertex=NULL,*pInsertVertex=NULL;
	pPrevVertex=pPlate->vertex_list.GetTail();
	for(pCurVertex=pPlate->vertex_list.GetFirst();pCurVertex;pCurVertex=pPlate->vertex_list.GetNext())
	{
		if(pPrevVertex->vertex.feature<=0)
			return -1;	//���������������
		else if(pPrevVertex->vertex.feature<iFaceNo)
			iFaceNo=pCurVertex->vertex.feature;
		f2dPoint inters_start,inters_end;
		//1.1����������ֱ�������μ����λ�ü��ཻ״̬,	0.��������;1.��������;2.�������߽���;3.����Ϊ���νǵ�
		EDGE_INTERS_STATE state=cutSector.CalEdgeIntersState(pPrevVertex->vertex,pCurVertex->vertex,inters_start,inters_end);
		//1.2����λ�ü��ཻ״̬����ԭ����������������½���
		if(state.stateVertexStart==1)	//��������Ч�����������ձ�ɾ��
			pPrevVertex->vertex.feature*=-1;
			// 0.�޽���;1.����Ϊ�ڵ�;2.����Ϊֱ�߶˵�;3.����Ϊ���νǵ�
		if(state.intersStartEdge==2)
		{	//������ֱ��������ʼ�߽���Ϊֱ�߶˵�
			double diff_start=fabs(pPrevVertex->vertex.x-inters_start.x)+fabs(pPrevVertex->vertex.y-inters_start.y);
			double diff_end=fabs(pCurVertex->vertex.x-inters_start.x)+fabs(pCurVertex->vertex.y-inters_start.y);
			if(diff_start<diff_end)	//��������ֱ��ʼ�˶����غ�
				pPrevVertex->vertex.feature=abs(pPrevVertex->vertex.feature)%100+100;
			else
				pCurVertex->vertex.feature=abs(pCurVertex->vertex.feature)%100+100;
		}
		if(state.intersEndEdge==2)
		{	//������ֱ��������ʼ�߽���Ϊֱ�߶˵�
			double diff_start=fabs(pPrevVertex->vertex.x-inters_end.x)+fabs(pPrevVertex->vertex.y-inters_end.y);
			double diff_end=fabs(pCurVertex->vertex.x-inters_end.x)+fabs(pCurVertex->vertex.y-inters_end.y);
			if(diff_start<diff_end)	//��������ֱ��ʼ�˶����غ�
				pPrevVertex->vertex.feature=abs(pPrevVertex->vertex.feature)%100+200;
			else
				pCurVertex->vertex.feature=abs(pCurVertex->vertex.feature)%100+200;
		}
		if(state.intersStartEdge==3||state.intersEndEdge==3)
		{	//�ǵ��ڵ�ǰ��������
			if((state.stateVertexStart==1&&state.stateVertexEnd==0)||(state.stateVertexStart==0&&state.stateVertexEnd==1))
			{	//������ʼ�������������ڣ��ᱻ�и���յ������������Ᵽ��
				pInsertVertex=pPlate->vertex_list.insert(*pPrevVertex);
				pInsertVertex->vertex.Set(cutSector.corner.x,cutSector.corner.y);
				pInsertVertex->vertex.feature=min(ftoi(pPrevVertex->vertex.feature),ftoi(pCurVertex->vertex.feature));
				pInsertVertex->vertex.feature=abs(pInsertVertex->vertex.feature);	//+300��ʾΪ�²��������νǵ�;
				cornerInserted=true;
			}
		}
		else if(state.intersStartEdge==1&&state.intersEndEdge!=1)
		{	//��ʼ�����ڽ���,���ձ����ڽ���
			pInsertVertex=pPlate->vertex_list.insert(*pPrevVertex);
			pInsertVertex->vertex.Set(inters_start.x,inters_start.y);
			pInsertVertex->vertex.feature=min(ftoi(pPrevVertex->vertex.feature),ftoi(pCurVertex->vertex.feature));
			pInsertVertex->vertex.feature=abs(pInsertVertex->vertex.feature)+100;	//+100��ʾΪ�²�����������ʼ�����ཻ�Ķ���;
		}
		else if(state.intersStartEdge!=1&&state.intersEndEdge==1)
		{	//��ʼ�����ڽ���,���ձ����ڽ���
			pInsertVertex=pPlate->vertex_list.insert(*pPrevVertex);
			pInsertVertex->vertex.Set(inters_end.x,inters_end.y);
			pInsertVertex->vertex.feature=min(ftoi(pPrevVertex->vertex.feature),ftoi(pCurVertex->vertex.feature));
			pInsertVertex->vertex.feature=abs(pInsertVertex->vertex.feature)+200;	//+200��ʾΪ�²�����������ֹ�����ཻ�Ķ���;
		}
		else if(state.intersStartEdge==1&&state.intersEndEdge==1)
		{	//��ʼ�ߺ��ձ߶����ڽ���
			f2dPoint inters_vec(inters_end.x-inters_start.x,inters_end.y-inters_start.y);
			f2dPoint edge_vec(pCurVertex->vertex.x-pPrevVertex->vertex.x,pCurVertex->vertex.y-pPrevVertex->vertex.y);
			PROFILE_VERTEX *pInsertVertex1=pPlate->vertex_list.insert(*pPrevVertex);
			PROFILE_VERTEX *pInsertVertex2=pPlate->vertex_list.insert(*pPrevVertex);
			if(inters_vec.x*edge_vec.x+inters_vec.y*edge_vec.y<0)	//�������Ⱥ�˳����ԭ�����߲�һ��
			{
				pInsertVertex1->vertex.Set(inters_end.x,inters_end.y);
				pInsertVertex1->vertex.feature=abs(pInsertVertex->vertex.feature)+200;	//+200��ʾΪ�²�����������ֹ�����ཻ�Ķ���;
				pInsertVertex2->vertex.Set(inters_start.x,inters_start.y);
				pInsertVertex2->vertex.feature=abs(pInsertVertex->vertex.feature)+100;	//+100��ʾΪ�²�����������ʼ�����ཻ�Ķ���;
			}
			else
			{
				pInsertVertex1->vertex.Set(inters_start.x,inters_start.y);
				pInsertVertex1->vertex.feature=abs(pInsertVertex->vertex.feature)+100;	//+100��ʾΪ�²�����������ʼ�����ཻ�Ķ���;
				pInsertVertex2->vertex.Set(inters_end.x,inters_end.y);
				pInsertVertex2->vertex.feature=abs(pInsertVertex->vertex.feature)+200;	//+200��ʾΪ�²�����������ֹ�����ཻ�Ķ���;
			}
			pInsertVertex1->vertex.feature=pInsertVertex2->vertex.feature=min(ftoi(pPrevVertex->vertex.feature),ftoi(pCurVertex->vertex.feature));
		}
		pPrevVertex=pCurVertex;	//��¼ǰһ�����㣬�����ж�����������
	}
	//2 ���������������featureֵ,�������νǵ�
	pPrevVertex=pPlate->vertex_list.GetTail();
	while(pPrevVertex&&pPrevVertex->vertex.feature<=0)
		pPrevVertex=pPlate->vertex_list.GetPrev();
	if(pPrevVertex==NULL)
		return 0;	//�ð屻ȫ���и��
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
				return -1;	//�и�ʧ��,��֧�ֽ�һ�鵥���ָ�Ϊ��������
			pInsertVertex=pPlate->vertex_list.insert(*pPrevVertex);
			pInsertVertex->vertex.Set(cutSector.corner.x,cutSector.corner.y);
			//���ܴ�pPrevVertex->vertex.feature��ȡ����pPrevVertex�п����������
			pInsertVertex->vertex.feature=iFaceNo;
			cornerInserted=true;
		}
		prevInsertVertexEdge=insertVertexEdge;
		pPrevVertex=pCurVertex;
	}
	for(pCurVertex=pPlate->vertex_list.GetFirst();pCurVertex;pCurVertex=pPlate->vertex_list.GetNext())
		pCurVertex->vertex.feature%=100;
	//3 �����ж������ȫȡ��ȫ�������������д���
	if(pPlate->vertex_list.GetNodeNum()<3)
	{
		pPlate->vertex_list.Empty();
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("All plates are to be cut off!");
#else
			AfxMessageBox("�ְ屻ȫ���и��!");
#endif
		return 0;
	}

	//4.ɾ�����α��������ص�ʱ��ɵ��ص�����
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
		if(fabs(prev_deltay*curr_deltax-prev_deltax*curr_deltay)<EPS)	//����
		{
			if(prev_deltax*curr_deltax+prev_deltay*curr_deltay<0)
				pPlate->vertex_list.DeleteCursor();	//�ص����ߣ�ɾ���м䶥��
		}
		pPrevVertex=pCurVertex;
	}
	pPlate->vertex_list.Clean();
	if(!bVertexModify)	//�������ζ���û�о����κδ���
	{
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("plates are not cut!");
#else
			AfxMessageBox("�ְ�û�н����и�!");
#endif
		return 1;
	}
	//5 ȥ�����ڰ巶Χ�ڵ���˨����
	for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())	//��˨����
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		f3dPoint ls_pos=pBolt->ucs.origin;
		coord_trans(ls_pos,pPlate->ucs,FALSE);
		ls_pos.z=0;
		if(cutSector.PositionType(f2dPoint(ls_pos.x,ls_pos.y))>0)	//�����������ڻ�߽���
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
			//�и�ǰ��ͼ�λ���
			vertex_offset=f3dPoint(0,-1,0)*200*j;
			dxf.NewLine(corner_offset+vertex_offset,start+vertex_offset);
			dxf.NewLine(corner_offset+vertex_offset,end+vertex_offset);
			PROFILE_VERTEX* pPrevVertex=plate.vertex_list.GetTail();
			for(PROFILE_VERTEX* pCurVertex=plate.vertex_list.GetFirst();pCurVertex;pCurVertex=plate.vertex_list.GetNext())
			{
				dxf.NewLine(pPrevVertex->vertex+corner_offset+vertex_offset,pCurVertex->vertex+corner_offset+vertex_offset);
				pPrevVertex=pCurVertex;
			}
			//-1�и�ʧ�� 0ȫ���и� 1û���и� 2�и�ɹ� 
			int result=CutPlateBySector(&plate,sector[i],FALSE);
			if(result==-1)
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("%d-%d cut failure",i,j);
			else if(result==0)
				logerr.Log("%d-%d All plates are to be cut off",i,j);
			else if(result==1)
				logerr.Log("%d-%d plates are not cut",i,j);
#else
				logerr.Log("%d-%d�и�ʧ��",i,j);
			else if(result==0)
				logerr.Log("%d-%d�ְ屻ȫ���и�",i,j);
			else if(result==1)
				logerr.Log("%d-%d�ְ�û�б��и�",i,j);
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
//��˫����ֳ����������
BOOL Split2PlanePlate(CLDSPlate* pPlate,CLDSPlate* plate_arr)
{
	if(pPlate->face_N!=2)
		return FALSE;
	//1����ȡ������Ϣ
	CLDSPlate* pDatumPlate=&plate_arr[0];
	CLDSPlate* pHuoquPlate=&plate_arr[1];
	f3dPoint huoqu_start,huoqu_end,huoqu_vec;
	huoqu_start=pPlate->HuoQuLine[0].startPt;
	huoqu_end=pPlate->HuoQuLine[0].endPt;
	coord_trans(huoqu_start,pPlate->ucs,TRUE);
	coord_trans(huoqu_end,pPlate->ucs,TRUE);
	huoqu_vec=huoqu_end-huoqu_start;
	normalize(huoqu_vec);
	//2��ȷ�������������ϵ
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
	//3����ʼ�������������ζ���
	f3dPoint datum_vertex,huoqu_vertex;
	for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		huoqu_vertex=datum_vertex=pPlate->GetRealVertex(pVertex->vertex);
		if(pVertex->vertex.feature==1 || pVertex->vertex.feature>10)	//��������뵽��׼���;	
		{	
			coord_trans(datum_vertex,pPlate->ucs,TRUE);
			coord_trans(datum_vertex,pDatumPlate->ucs,FALSE);
			datum_vertex.z=0;
			pVertex=pDatumPlate->vertex_list.append(*pVertex);
			pVertex->vertex.Set(datum_vertex.x,datum_vertex.y,datum_vertex.z);
		}
		if(pVertex->vertex.feature==2 || pVertex->vertex.feature>10)	//��������뵽�������
		{
			coord_trans(huoqu_vertex,pPlate->ucs,TRUE);
			coord_trans(huoqu_vertex,pHuoquPlate->ucs,FALSE);
			huoqu_vertex.z=0;
			pVertex=pHuoquPlate->vertex_list.append(*pVertex);
			pVertex->vertex.Set(huoqu_vertex.x,huoqu_vertex.y,huoqu_vertex.z);
		}
	}
	//3����ȡ��������ϵ���˨����
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
//�ϲ�������Ϊ˫���
BOOL Combine2PlanePlate(CLDSPlate* pPlate,CLDSPlate *plate_arr)
{
	int datumVertexsNum=plate_arr[0].vertex_list.GetNodeNum();
	int huoquVertexsNum=plate_arr[1].vertex_list.GetNodeNum();
	if(pPlate->face_N!=2||(datumVertexsNum==0&&huoquVertexsNum==0))
		return FALSE;
	ATOM_LIST<PROFILE_VERTEX> vertex_list;
	pPlate->vertex_list.Empty();
	if(datumVertexsNum==0&&huoquVertexsNum>3)	//������������
	{	//�������涥������ת������������ϵ�£����浽vertex_list
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
		if(datumVertexsNum>3&&huoquVertexsNum==0)	//��������׼��
		{	//����׼�涥������ת������������ϵ�£����浽vertex_list
			for(pVertex=plate_arr[0].vertex_list.GetFirst();pVertex;pVertex=plate_arr[0].vertex_list.GetNext())
			{	
				coord_trans(pVertex->vertex,plate_arr[0].ucs,TRUE);
				pVertex->vertex.feature=1;
				pPlate->vertex_list.append(*pVertex);
			}
		}
		else	//��׼��ͻ����涼���ڵ����
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
				AfxMessageBox("�ݲ�֧�ֽ�һ��˫���ָ�Ϊ���鵥������");
#endif
				return FALSE;
			}
			//�����׼����ͨ����
			for(i=2;i<vertex_arr0.GetSize();i++)
			{
				int index=(i+huoqustart_index0)%vertex_arr0.GetSize();
				pVertex=vertex_list.append(vertex_arr0[index]);
				coord_trans(pVertex->vertex,plate_arr[0].ucs,TRUE);
			}
			//����������ʼ����
			pVertex=vertex_list.append(vertex_arr0[huoqustart_index0]);
			coord_trans(pVertex->vertex,plate_arr[0].ucs,TRUE);
			//���������涥��
			for(i=2;i<vertex_arr1.GetSize();i++)
			{
				int index=(i+huoqustart_index1)%vertex_arr1.GetSize();
				pVertex=vertex_list.append(vertex_arr1[index]);
				coord_trans(pVertex->vertex,plate_arr[1].ucs,TRUE);
			}
			//�����������ն���
			pVertex=vertex_list.append(vertex_arr0[(huoqustart_index0+1)%vertex_arr0.GetSize()]);
			coord_trans(pVertex->vertex,plate_arr[0].ucs,TRUE);
		}
		//��vertex_list�ж���ת��ԭʼ���׼������ϵ�£����ԭʼ˫���Ķ����б�
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			coord_trans(pVertex->vertex,pPlate->ucs,FALSE);
			pVertex->vertex=pPlate->GetSpreadVertex(pVertex->vertex,pVertex->vertex.feature);
			pVertex->vertex.z=0;
			pPlate->vertex_list.append(*pVertex);
		}
	}
	//���˫������˨����
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
{	//��vertexת����tm_ucs�������ϵ��,��z����С��0��ʾ���ڻ�׼��,��֮���ڻ�����;bTopFace==TRUE,�����෴
	int iUcsIndex=0;	//ʹ�õ�һ������ϵ
	f3dPoint tm_pnt=vertex;
	coord_trans(tm_pnt,tm_ucs,FALSE);
	if(bTopFace&&tm_pnt.z<0)
		iUcsIndex=1;	//ʹ�õڶ�������ϵ
	else if(!bTopFace&&tm_pnt.z>0)
		iUcsIndex=1;	//ʹ�õڶ�������ϵ
	return iUcsIndex;
}*/
//ͨ��ָ���ĸְ��и�ְ�(�Զ��жϸְ��и���)
//1.˫����и��� 2.˫����и�˫���
//BOOL CLDSView::CutPlateByPlate(CLDSPlate *pPlate,CLDSPlate *pDatumPlate,
//							   BOOL bTopFace,double fMinDist,BOOL bPrompt/*=TRUE*/)
/*{
	if(pPlate->face_N==3)
	{
		if(bPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Sorry, three-plane plate can not be cut temporarily!");
#else
			AfxMessageBox("�ܱ�Ǹ,Ŀǰ�ݲ��ָܷ������!");
#endif
		return FALSE;
	}
	if(pDatumPlate->face_N==3)
	{
		if(bPrompt)
			AfxMessageBox("�ܱ�Ǹ,Ŀǰ�ݲ�֧��ʹ��������и�ְ�!");
		return FALSE;
	}
	if(pPlate->vertex_list.GetNodeNum()<3||pDatumPlate->vertex_list.GetNodeNum()<3)
		return FALSE;
	//��׼�ְ�Ϊ�����ʱ,ֱ�ӵ���ͨ��ƽ���и�ְ庯������
	if(pDatumPlate->face_N==1)	
		return CutPlateByPlane(pPlate,pDatumPlate->ucs.origin,pDatumPlate->ucs.axis_z,fMinDist,bPrompt);
	//������˫����и�ְ�����
	//1���ֱ�Ϊ��׼��ͻ����潨������ϵ
	int i=0;
	UCS_STRU ucs[2];
	f3dPoint axis_z[2];	//�������ϵ�µķ���
	f3dPoint face_pos[2],face_norm[2];	//�ְ��׼��(�ڶ���)��׼���Լ�����
	f3dPoint datum_pos=pDatumPlate->HuoQuLine[0].startPt;
	coord_trans(datum_pos,pDatumPlate->ucs,TRUE);
	f3dPoint huoqu_offset_vec=pDatumPlate->ucs.axis_z+pDatumPlate->huoqufacenorm[0];
	normalize(huoqu_offset_vec);
	for(i=0;i<2;i++)
	{
		face_norm[i]=pDatumPlate->ucs.axis_z;
		if(i==1) //�ڶ���
			face_norm[i]=pDatumPlate->huoqufacenorm[0];	
		face_pos[i]=datum_pos+face_norm[i]*pDatumPlate->m_fNormOffset;
		if(bTopFace)
		{	//���渺���߷����и�,ʰȡ��(֮ǰ�ڵ���)�����ڷ��߸�����ƫ��һ�����󵽶�����
			face_norm[i]*=-1.0;
			double cosa=fabs(huoqu_offset_vec*pDatumPlate->ucs.axis_z);
			double huoqu_offset_dist=pDatumPlate->GetThick()*(1.0/cosa);
			face_pos[i]-=face_norm[i]*huoqu_offset_dist;
		}
		//�ٶ�������ϵ	
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
	//�����������ϵ�����ж��и�ְ�ʱʹ���ĸ���׼�� 
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
	{	//1.�и��Ժ�İ�Ϊ����� 2.�и��Ժ�İ���ȻΪ˫���
		f3dPoint huoqu_start=pPlate->HuoQuLine[0].startPt;
		f3dPoint huoqu_end=pPlate->HuoQuLine[0].endPt;
		coord_trans(huoqu_start,pPlate->ucs,TRUE);
		coord_trans(huoqu_end,pPlate->ucs,TRUE);
		coord_trans(huoqu_start,ucs[GetFaceIndex(tm_ucs,huoqu_start,bTopFace)],FALSE);
		coord_trans(huoqu_end,ucs[GetFaceIndex(tm_ucs,huoqu_end,bTopFace)],FALSE);
		if(huoqu_start.z>=0&&huoqu_end.z>=0)
			face_N=1;	//�и��Ժ�İ�Ϊ�����
		else
			face_N=2;	//�и��Ժ�İ�Ϊ˫���
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
		pPrevVertex->vertex.feature*=-1;	//����Ҫ��ɾ�����Ķ���
	int iFaceIndex=0;	//�и�������
	f3dPoint vertice_cursor;
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		cursor_face_i=pVertex->vertex.feature;
		vertice_cursor=pPlate->GetRealVertex(pVertex->vertex);
		coord_trans(vertice_cursor,pPlate->ucs,TRUE);
		tm_pnt=vertice_cursor;
		iFaceIndex=GetFaceIndex(tm_ucs,tm_pnt,bTopFace);	//�и�������
		coord_trans(tm_pnt,ucs[iFaceIndex],FALSE);
		cursor_z=tm_pnt.z;
		if(cursor_z>0&&pVertex->vertex.feature>0)	//����Ҫ��ɾ�����Ķ���
			pVertex->vertex.feature*=-1;
		if(prev_z*cursor_z>=0)	//��ͬ�಻����㽻��
		{
			prev_z=cursor_z;
			vertice_prev=vertice_cursor;
			prev_face_i=cursor_face_i;
			pPrevVertex=pVertex;//��¼ǰһ�����㣬�����жϵ�ǰ���������� 
			continue;
		}
		else	//����ͬһ�и�������࣬��Ҫ�������潻�㣬һ�㲻�����������������ڲ�ͬ�и��������
		{
			f3dPoint inters,vertice;
			if(pPrevVertex&&pPrevVertex->type>0)
			{	//Բ���� Ŀǰ����Բ���ߵĸְ嶼Ϊ�����  wht 09-11-11
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
					//����Բ����
					f3dArcLine arcLine;
					int sign=1;
					if(pPrevVertex->type==1)
					{	//Բ��
						if(pPrevVertex->sector_angle<0)
							sign=-1;
						arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),fabs(pPrevVertex->sector_angle));
					}
					else if(pPrevVertex->type==2)	//ָ��Բ��R
					{	
						if(pPrevVertex->radius<0)
							sign=-1;
						arcLine.CreateMethod3(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),sign*pPrevVertex->radius,pPrevVertex->center);
						//������CreateMethod2,����ʧ��ʰȡ����Ϣ�����ڴ���180��Ļ��Ǵ�ġ� wht 12-06-14
						//arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),pPrevVertex->sector_angle);
					}
					else if(pPrevVertex->type==3)	//��Բ��
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
						if(nRet>0)	//�ݲ�����ֱ����Բ����������������
						{
							if(pPrevVertex->type==3)
							{	//��Բ��
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
							double offset_angle=fMinDist*(1.0/pPrevVertex->radius);//�����ɵĸְ����׼�ְ��ľ���
							//arcLine.WorkNorm().z>0==>��ʱ�� cursor_z<0==>���
							if(cursor_z*arcLine.WorkNorm().z>0)
								offset_angle*=-1.0;
							rotate_point_around_axis(pos1,offset_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
							*/
						/*	double arc_r=arcLine.Radius();	
							if(fabs(arc_r)>EPS)
							{	//Բ���뾶С�ڵ���0ʱΪ�쳣��� wht 11-05-27
								if(cursor_z<0)	//��ǰ���㲻�ᱻɾ��
								{	
									pInsertVertex->type=pPrevVertex->type;
									pInsertVertex->center=pPrevVertex->center;
									pInsertVertex->radius=pPrevVertex->radius;
									pInsertVertex->column_norm=pPrevVertex->column_norm;
									pInsertVertex->m_bWeldEdge=pPrevVertex->m_bWeldEdge;
									double half_len = 0.5*DISTANCE(pVertex->vertex,pos1);
									//ʹ�ü���ó���Բ���뾶�ҿ������ν������� wht 11-05-30
									if(fabs(half_len/arc_r)<=1&&pPrevVertex->type==1)	
										pInsertVertex->sector_angle = 2*asin(half_len/arc_r)*sign;
								}
								else //��ǰ����Ϊ��ɾ�����㣬����ǰһ����������
								{
									double sector_angle=0;
									double half_len = 0.5*DISTANCE(pPrevVertex->vertex,pos1);
									//ʹ�ü���ó���Բ���뾶�ҿ������ν������� wht 11-05-30
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
			{	//ֱ�߱�
				Int3dlf(inters,f3dLine(vertice_prev,vertice_cursor),face_pos[iFaceIndex],face_norm[iFaceIndex]);
				coord_trans(inters,pPlate->ucs,FALSE);
				vertice=inters;
				if(abs(prev_face_i)==abs(cursor_face_i)&&abs(cursor_face_i)<10)
					vertice.feature=cursor_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(cursor_face_i));	//�ཻ��ȫ������ĳһ����
				else if(abs(prev_face_i)==abs(cursor_face_i)&&abs(cursor_face_i)>10)
					vertice=pPlate->GetSpreadVertex(inters,1);//�ཻ������ȫ���ǻ����ߵ�,��һ���һ�㲻����
				else if(abs(cursor_face_i)<10&&abs(prev_face_i)>10)
					vertice.feature=cursor_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(cursor_face_i));	//�ཻ��һ���ǻ����� һ������ͨ��
				else if(abs(prev_face_i)<10&&abs(cursor_face_i)>10)
					vertice.feature=prev_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(prev_face_i));	//�ཻ��һ���ǻ����� һ��ʱ��ͨ��
				vertice.z=0;	//pVertex->vertex.z;
				vertice-=axis_z[iFaceIndex]*fMinDist; //�����ɵĸְ����׼�ְ��ľ���
				PROFILE_VERTEX *pInsertVertex=pPlate->vertex_list.insert(vertice);
				if(cursor_z<0)	//��ǰ���㲻�ᱻɾ��
				{
					pInsertVertex->m_bWeldEdge=pPrevVertex->m_bWeldEdge;	//������
					pInsertVertex->m_bRollEdge=pPrevVertex->m_bRollEdge;	//���
					pInsertVertex->manu_space=pPrevVertex->manu_space;		//�ӹ���϶
				}
				else if(pInsertVertex->vertex.feature<0)
					pInsertVertex->vertex.feature*=-1;	//��֤�²���Ķ��㲻��ɾ����
			}
			prev_z=cursor_z;
			pPrevVertex=pVertex;//��¼ǰһ�����㣬�����ж�����������
			vertice_prev=vertice_cursor;
			prev_face_i=cursor_face_i;
		}
		//������˫���ȥ�и�������һ���ߵ������������(�����һ�㲻����)
	}
	if(face_N==1)
	{	//�ж��и����Ƿ��뱻�и�ְ��ཻ,���ڽ��������������
		int iDelVertex=0;
		SCOPE_STRU scope;
		for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			scope.VerifyVertex(pVertex->vertex);
		for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			if(pVertex->vertex.feature<0)
			{
				if(iDelVertex==0&&old_face_N==1)
				{	//����һӦ�ñ��г��Ķ����޸�Ϊ˫���������뱻�и�ְ�Ľ��� wht 11-01-20
					f3dPoint inters_pick,inters_vec,inters_pt;
					if(Int3d2f(&inters_pick,&inters_vec,face_pos[0],face_norm[0],face_pos[1],face_norm[1])==1)
					{	//��׼��������潻��
						f3dPoint inters_pt;
						int nRetCode=Int3dlf(inters_pt,inters_pick,inters_vec,pPlate->ucs.origin,pPlate->ucs.axis_z);
						coord_trans(inters_pt,pPlate->ucs,FALSE);
						inters_pt.z=0;
						if(nRetCode&&scope.IsIncludePoint(inters_pt))
						{	//�����ڱ��и�ְ������ڣ��޸ĵ�һ����ɾ��������Ϊ��������ְ彻��
							pVertex->vertex=inters_pt;
							pVertex->vertex.feature*=-1;
							pVertex->m_bCanCut=FALSE;	//����Ϊ���㣬������Ϊ�������������ʱɾ���ö���
						}
					}
				}
				iDelVertex++;
			}
		}
		if(iDelVertex==pPlate->vertex_list.GetNodeNum()||iDelVertex==0)
		{	//�������ж��㶼���и����һ�࣬�޷�����и�
			if(iDelVertex==pPlate->vertex_list.GetNodeNum())
			{	//�ְ������ж��㶼���и���һ�������е㶼Ӧ��ɾ��ʱ��feature�޸�Ϊ��ֵ wht 10-12-01
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
				AfxMessageBox("�и�����ְ岻�ཻ���޷�����и�!");
#endif
			return FALSE;
		}
	}
	//ȥ�����и���ķǻ�������
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		if(face_N==1)
		{
			if(pVertex->vertex.feature<0)	//Ӧ�ñ��и���Ķ���
				pPlate->vertex_list.DeleteCursor();
		}
		else 
		{
			if(pVertex->vertex.feature<0&&abs(pVertex->vertex.feature)<10)	//Ӧ�ñ��и���ķǻ�����
				pPlate->vertex_list.DeleteCursor();
		}
	}
	//�����һ������ͱ��и���Ļ�,�����ȥ�������ͻ���©���һ������
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
			AfxMessageBox("�ְ屻ȫ���и��!");
#endif
		return TRUE;
	}
	//������Ʊ��и���Ļ�������
	int n=pPlate->vertex_list.GetNodeNum();
	pPlate->face_N=face_N;
	if(pPlate->vertex_list.GetFirst()->vertex.feature==2&&pPlate->face_N==1)
	{	//�и�������ɵĵ����Ϊԭ��˫���ĵڶ���ʱ�����������ϵ
		pPlate->ucs.axis_z=pPlate->huoqufacenorm[0].normalized();
		pPlate->ucs.axis_x=pPlate->ucs.axis_y^pPlate->ucs.axis_z;
		normalize(pPlate->ucs.axis_x);
	}
	for(i=0;i<n;i++)
	{
		pVertex=&pPlate->vertex_list[i];
		if(face_N==1)
			pVertex->vertex.feature=face_N;
		if(pVertex->vertex.feature<-10)	//������ƻ�����
		{
			pVertex->vertex.feature=abs(pVertex->vertex.feature);
			f3dPoint *pPrevPoint=&pPlate->vertex_list[(i+n-1)%n].vertex;
			f3dPoint *pNextPoint=&pPlate->vertex_list[(i+1)%n].vertex;
			pPrevPoint->z=pNextPoint->z=0;	//Z�����������󽻿���ʧ�� wht 11-01-19
			//1.1����������˫���Ļ�����
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
				bHashHuoquLineInters=TRUE;	//�����������н���(�����и���Ч)
				bNeedCalHuoquInters=FALSE;
			}
			if(bTopFace)
			{	//�����и�ʱ,��׼���������ƫ�Ƶ������и�����
				f3dPoint huoqu_offset_vec=face_norm[0]+face_norm[1];
				normalize(huoqu_offset_vec);
				double cosa=fabs(huoqu_offset_vec*face_norm[0]);
				double huoqu_offset_dist=pDatumPlate->GetThick()*(1.0/cosa);
				datum_startPos -= huoqu_offset_vec*huoqu_offset_dist;
				datum_endPos -= huoqu_offset_vec*huoqu_offset_dist;
				if(!bHashHuoquLineInters&&Int3dpl(f3dLine(startPos,endPos),f3dLine(datum_startPos,datum_endPos),inters)==1)
				{	
					bHashHuoquLineInters=TRUE;	//���������޽���ʱ����׼�������ƫ��֮�������¼����Ƿ��뱻�и�ְ�������н���
					bNeedCalHuoquInters=FALSE;
				}
				else
					bNeedCalHuoquInters=TRUE;
			}
			if(bNeedCalHuoquInters)
			{	//���¼��������ʱ,ͨ�����������и����ཻ�������,
				//���û��������׼�и�����,�鿴���������ĸ�����,����ڻ���������Ҫ���¼��������
				Int3dlf(inters,f3dLine(startPos,endPos),face_pos[0],face_norm[0]);
				iFaceIndex=GetFaceIndex(tm_ucs,inters,bTopFace);
				if(iFaceIndex==1)
					Int3dlf(inters,f3dLine(startPos,endPos),face_pos[1],face_norm[1]);
			}
			//1.2���»���������
			coord_trans(inters,pPlate->ucs,FALSE);
			pVertex->vertex.Set(inters.x,inters.y,0);
			//1.3����������û�н���,��ʱ������һ����(͹����)
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
					pInsertVertex->m_bCanCut=FALSE;	//����Ϊ���㣬������Ϊ�������������ʱɾ���ö���
				}
				else if(iFace1+1==pNextPoint->feature)
				{
					PROFILE_VERTEX *pInsertVertex=pInsertVertex=pPlate->vertex_list.insert(inters);
					pInsertVertex->vertex.feature=pNextPoint->feature;
					pInsertVertex->m_bCanCut=FALSE;	//����Ϊ���㣬������Ϊ�������������ʱɾ���ö���
				}
			}
			//��׼�ְ�Ϊ�����ʱֱ�ӵ���ͨ��ƽ���и�ְ庯������
			//else if(pDatumPlate->face_N==1)
			//{
			//	Int3dpl(f3dLine(*pPrevPoint,*pNextPoint),pPlate->HuoQuLine[0],inters);
			//	pVertex->vertex.Set(inters.x,inters.y,inters.z);
			//}
		}
	}
	//ȥ�����ڰ巶Χ�ڵ���˨����
	for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		f3dPoint ls_pos=(*pLsRef)->ucs.origin;
		coord_trans(ls_pos,ucs[iFaceIndex],FALSE);
		if(ls_pos.z>0)
			pPlate->GetLsRefList()->DeleteCursor();
	}
	//�Ƴ����ڰ巶Χ�ڵ����߸˼�	wht 10-11-30
	CDesignLjPartPara *pLjPara=NULL;
	if(pPlate->jdb_style!=4)
	{	//��˨����е�һ�����ӹ�����ʵ���и�Ǹ֣������޳�
		for(pLjPara=pPlate->GetFirstLjPara();pLjPara;pLjPara=pPlate->GetNextLjPara())
		{
			//������߸˼���˨�����б�
			CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(pLjPara->hPart,CLS_LINEPART);
			if(pLinePart==NULL)
				continue;
			CLsRefList *pLsRefList=NULL;
			if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//�ֹ�
				CLDSParamPlate *pParamPlate=NULL;
				pParamPlate=((CLDSLineTube*)pLinePart)->GetParamPlate(pLjPara->start0_end1);
				if(pParamPlate)
					pLsRefList=pParamPlate->GetLsRefList();
			}
			else 
				pLsRefList=pLinePart->GetLsRefList();
			//��˨�����б�Ϊ�ջ��߸��б��еĲ������ְ��ϵ���˨����ɾ�������߸˼�
			if(pLsRefList==NULL)
				pPlate->DeleteLjPara(pLjPara->hPart);
			else
			{
				CLsRef *pLsRef=NULL;
				for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{
					if(pLsRefList->FromHandle(pLsRef->GetLsPtr()->handle))
						break;	//��˨�����б��а����ְ��ϵ���˨�������߸˼�Ϊ�ְ��ϵ����߸˼�
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
			AfxMessageBox("�ܱ�Ǹ,Ŀǰ�ݲ��ָܷ������!");
#endif
		return FALSE;
	}
	if(pPlate->vertex_list.GetNodeNum()<3)
		return FALSE;
	//�����ж�ʱ�еĵط������˼�϶ֵfMinDist�е�δ���ǣ���ͳһ���ǵ�face_pos��������fMinDist�� wjh-2019.9.19
	face_pos-=fMinDist*face_norm;
	fMinDist=0;
	//�ٶ�������ϵ
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
	//�Ըְ��м���Ϊ��׼�����и�(�����и����ɵ�����ְ����и��׼��һ�µ������β�ͬ) wht 16-12-04
	double fNormOffset=pPlate->GetNormOffset()+0.5*pPlate->GetThick();	//���浽�м����ƫ����
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
	{	//1.�и��Ժ�İ�Ϊ����� 2.�и��Ժ�İ���ȻΪ˫���
		PROFILE_VERTEX *pHuoquStart=NULL,*pHuoquEnd=NULL;
		//�˴�ʹ�û�����ʼ�ն������ж��и���Ƿ�Ϊ˫���,ֱ��ʹ��HuoQuLine[0]�жϿ��ܲ�׼ȷ,
		//ʹ��GetBendlineVertexies()������ȡ�����Ļ�����������		wht 16-10-08
		pPlate->GetBendlineVertexies(&pHuoquStart,&pHuoquEnd);
		f3dPoint huoqu_start=pHuoquStart?pHuoquStart->vertex:pPlate->HuoQuLine[0].startPt;
		f3dPoint huoqu_end=pHuoquEnd?pHuoquEnd->vertex:pPlate->HuoQuLine[0].endPt;
		coord_trans(huoqu_start,pPlate->ucs,TRUE);
		coord_trans(huoqu_start,ucs,FALSE);
		coord_trans(huoqu_end,pPlate->ucs,TRUE);
		coord_trans(huoqu_end,ucs,FALSE);
		if(huoqu_start.z>0&&huoqu_end.z>0)
			face_N=1;	//�и��Ժ�İ�Ϊ�����
		else
			face_N=2;	//�и��Ժ�İ�Ϊ˫���
	}
	first_face_i=prev_face_i=pPrevVertex->vertex.feature;
	tm_pnt=vertice_first=vertice_prev;
	coord_trans(tm_pnt,ucs,FALSE);
	first_z=prev_z=tm_pnt.z;
	if(prev_z>0)
		pPrevVertex->vertex.feature*=-1;	//����Ҫ��ɾ�����Ķ���
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
		if(cursor_z>0)			//����Ҫ��ɾ�����Ķ���
			pVertex->vertex.feature*=-1;
		if(prev_z*cursor_z>=0)	//��ͬ�಻����㽻��
		{
			prev_z=cursor_z;
			vertice_prev=vertice_cursor;
			prev_face_i=cursor_face_i;
			pPrevVertex=pVertex;//��¼ǰһ�����㣬�����жϵ�ǰ���������� 
			continue;
		}
		else	//�����и�������࣬��Ҫ�������潻��
		{
			f3dPoint inters,vertice;
			if(pPrevVertex&&pPrevVertex->type>0)
			{	//Բ���� Ŀǰ����Բ���ߵĸְ嶼Ϊ�����  wht 09-11-11
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
					//����Բ����
					f3dArcLine arcLine;
					int sign=1;
					if(pPrevVertex->type==1)
					{	//Բ��
						if(pPrevVertex->sector_angle<0)
							sign=-1;
						arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),fabs(pPrevVertex->sector_angle));
					}
					else if(pPrevVertex->type==2)	//ָ��Բ��R
					{	
						if(pPrevVertex->radius<0)
							sign=-1;
						arcLine.CreateMethod3(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),sign*pPrevVertex->radius,pPrevVertex->center);
						//������CreateMethod2,����ʧ��ʰȡ����Ϣ�����ڴ���180��Ļ��Ǵ�ġ� wht 12-06-14
						//arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),pPrevVertex->sector_angle);
					}
					else if(pPrevVertex->type==3)	//��Բ��
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
						if(nRet>0)	//�ݲ�����ֱ����Բ����������������
						{
							if(pPrevVertex->type==3)
							{	//��Բ��
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
							double offset_angle=fMinDist*(1.0/pPrevVertex->radius);//�����ɵĸְ����׼�ְ��ľ���
							//arcLine.WorkNorm().z>0==>��ʱ�� cursor_z<0==>���
							if(cursor_z*arcLine.WorkNorm().z>0)
								offset_angle*=-1.0;
							rotate_point_around_axis(pos1,offset_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
							*/
							//pInsertVertex->radius �˴�Ӧʹ��Բ���뾶(type==1ʱpInsertVertex->radius==0) wht 11-05-27
							double arc_r=arcLine.Radius();	
							if(fabs(arc_r)>EPS)
							{	//Բ���뾶С�ڵ���0ʱΪ�쳣��� wht 11-05-27
								if(cursor_z<0)	//��ǰ���㲻�ᱻɾ��
								{	
									pInsertVertex->type=pPrevVertex->type;
									pInsertVertex->center=pPrevVertex->center;
									pInsertVertex->radius=pPrevVertex->radius;
									pInsertVertex->column_norm=pPrevVertex->column_norm;
									pInsertVertex->m_bWeldEdge=pPrevVertex->m_bWeldEdge;
									double half_len = 0.5*DISTANCE(pVertex->vertex,pos1);
									//ʹ�ü���ó���Բ���뾶�ҿ������ν������� wht 11-05-30
									if(fabs(half_len/arc_r)<=1&&pPrevVertex->type==1)
										pInsertVertex->sector_angle = 2*asin(half_len/arc_r)*sign;
								}
								else //��ǰ����Ϊ��ɾ�����㣬����ǰһ����������
								{
									double sector_angle=0;
									double half_len = 0.5*DISTANCE(pPrevVertex->vertex,pos1);
									//ʹ�ü���ó���Բ���뾶�ҿ������ν������� wht 11-05-30
									if(fabs(half_len/arc_r)<=1)
										sector_angle = 2*asin(half_len/arc_r);
									if(pPrevVertex->type==1)
										pPrevVertex->sector_angle=sign*sector_angle;
									if(bSetWeldEdge)
										pInsertVertex->m_bWeldEdge=true;	//���и��Ϊ������
								}
							}
						}
					}
				}
			}
			else
			{	//ֱ�߱�
				Int3dlf(inters,f3dLine(vertice_prev,vertice_cursor),face_pos,face_norm);
				coord_trans(inters,pPlate->ucs,FALSE);
				vertice=inters;
				if(abs(prev_face_i)==abs(cursor_face_i)&&abs(cursor_face_i)<10)
					vertice.feature=cursor_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(cursor_face_i));	//�ཻ��ȫ������ĳһ����
				else if(abs(prev_face_i)==abs(cursor_face_i)&&abs(cursor_face_i)>10)
					vertice=pPlate->GetSpreadVertex(inters,1);//�ཻ������ȫ���ǻ����ߵ�,��һ���һ�㲻����
				else if(abs(cursor_face_i)<10&&abs(prev_face_i)>10)
					vertice.feature=cursor_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(cursor_face_i));	//�ཻ��һ���ǻ����� һ������ͨ��
				else if(abs(prev_face_i)<10&&abs(cursor_face_i)>10)
					vertice.feature=prev_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(prev_face_i));	//�ཻ��һ���ǻ����� һ��ʱ��ͨ��
				if(vertice.feature>1)
					vertice=pPlate->GetSpreadVertex(vertice,vertice.feature);
				vertice.z=0;	//pVertex->vertex.z;
				/*��ֹ�򶥵�����������Է��ֵĶԳư�����У�˲�ƥ����� wjh-2015.5.15
				  ��������Ķ������������Ľ���������������ý���������±����Ķ�������
				  pVertex->vertex.feature>0 �����Ķ���Ϊ��ǰ����   pVertex
				  pVertex->vertex.feature<0 �����Ķ���Ϊǰһ������ pPrevVertex   wxc-2017.1.23
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
				if(cursor_z<0)	//��ǰ���㲻�ᱻɾ��
				{
					pInsertVertex->m_bWeldEdge=pPrevVertex->m_bWeldEdge;	//������
					pInsertVertex->m_bRollEdge=pPrevVertex->m_bRollEdge;	//���
					pInsertVertex->manu_space=pPrevVertex->manu_space;		//�ӹ���϶
				}
				else if(bSetWeldEdge)
					pInsertVertex->m_bWeldEdge=true;	//�����и��Ϊ������
			}
			prev_z=cursor_z;
			pPrevVertex=pVertex;//��¼ǰһ�����㣬�����ж�����������
			vertice_prev=vertice_cursor;
			prev_face_i=cursor_face_i;
		}
	}
	if(first_z*cursor_z<0)	//����ĩ�����������и�
	{
		f3dPoint inters,vertice;
		pPrevVertex=pPlate->vertex_list.GetTail();
		pVertex=pPlate->vertex_list.GetFirst();
		if(pVertex&&pPrevVertex&&pPrevVertex->type>0)
		{	//Բ���� Ŀǰ����Բ���ߵĸְ嶼Ϊ�����  wht 09-11-11
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
				//����Բ����
				f3dArcLine arcLine;
				int sign=1;
				if(pPrevVertex->type==1)
				{	//Բ��
					if(pPrevVertex->sector_angle<0)
						sign=-1;
					arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),fabs(pPrevVertex->sector_angle));
				}
				else if(pPrevVertex->type==2)	//ָ��Բ��R
				{	
					if(pPrevVertex->radius<0)
						sign=-1;
					arcLine.CreateMethod3(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),sign*pPrevVertex->radius,pPrevVertex->center);
					//������CreateMethod2,����ʧ��ʰȡ����Ϣ�����ڴ���180��Ļ��Ǵ�ġ� wht 12-06-14
					//arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),pPrevVertex->sector_angle);
				}
				else if(pPrevVertex->type==3)	//��Բ��
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
					if(nRet>0)	//�ݲ�����ֱ����Բ����������������
					{
						if(pPrevVertex->type==3)
						{	//��Բ��
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
						//pInsertVertex->radius �˴�Ӧʹ��Բ���뾶(type==1ʱpInsertVertex->radius==0) wht 11-05-27
						double arc_r=arcLine.Radius();	
						if(fabs(arc_r)>EPS)
						{	//Բ���뾶С�ڵ���0ʱΪ�쳣��� wht 11-05-27
							PROFILE_VERTEX *pInsertVertex=pPlate->vertex_list.insert(PROFILE_VERTEX(pos1));
							if(first_z<0)	//��ǰ���㲻�ᱻɾ��
							{	
								pInsertVertex->type=pPrevVertex->type;
								pInsertVertex->center=pPrevVertex->center;
								pInsertVertex->radius=pPrevVertex->radius;
								pInsertVertex->column_norm=pPrevVertex->column_norm;
								pInsertVertex->m_bWeldEdge=pPrevVertex->m_bWeldEdge;
								double half_len = 0.5*DISTANCE(pVertex->vertex,pos1);
								//ʹ�ü���ó���Բ���뾶�ҿ������ν������� wht 11-05-30
								if(fabs(half_len/arc_r)<=1&&pPrevVertex->type==1)
									pInsertVertex->sector_angle = 2*asin(half_len/arc_r)*sign;
							}
							else //��ǰ����Ϊ��ɾ�����㣬����ǰһ����������
							{	 
								double sector_angle=0;
								double half_len = 0.5*DISTANCE(pPrevVertex->vertex,pos1);
								//ʹ�ü���ó���Բ���뾶�ҿ������ν������� wht 11-05-30
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
				vertice.feature=first_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(first_face_i));	//�ཻ��ȫ������ĳһ����
			else if(abs(prev_face_i)==abs(first_face_i)&&abs(first_face_i)>10)
				vertice=pPlate->GetSpreadVertex(inters,1);//�ཻ������ȫ���ǻ����ߵ�,��һ���һ�㲻����
			else if(abs(first_face_i)<10&&abs(prev_face_i)>10)
				vertice.feature=first_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(first_face_i));	//�ཻ��һ���ǻ����� һ������ͨ��
			else if(abs(prev_face_i)<10&&abs(first_face_i)>10)
				vertice.feature=prev_face_i;	//vertice=pPlate->GetSpreadVertex(inters,abs(prev_face_i));	//�ཻ��һ���ǻ����� һ��ʱ��ͨ��	
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
			if(first_z<0)	//��ǰ���㲻�ᱻɾ��
			{
				pInsertVertex->m_bWeldEdge=pPrevVertex->m_bWeldEdge;	//������
				pInsertVertex->m_bRollEdge=pPrevVertex->m_bRollEdge;	//���
				pInsertVertex->manu_space=pPrevVertex->manu_space;		//�ӹ���϶
			}
			else if(bSetWeldEdge)
				pInsertVertex->m_bWeldEdge=true;	//�����и��Ϊ������
		}
	}
	if(face_N==1)
	{	//�ж��и����Ƿ��뱻�и�ְ��ཻ,���ڽ��������������
		int iDelVertex=0;
		for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			if(pVertex->vertex.feature<0)
				iDelVertex++;
		}
		if(iDelVertex==pPlate->vertex_list.GetNodeNum()||iDelVertex==0)
		{	//�������ж��㶼���и����һ�࣬�޷�����и�
			if(iDelVertex==pPlate->vertex_list.GetNodeNum())
			{	//�ְ������ж��㶼���и���һ�������е㶼Ӧ��ɾ��ʱ��feature�޸�Ϊ��ֵ wht 10-12-01
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
				logerr.Log("�и�����ְ�0X%X���ཻ���޷�����и�!",pPlate->handle);
#endif
			return FALSE;
		}
	}
	//ȥ�����и���ķǻ�������
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		if(face_N==1)
		{
			if(pVertex->vertex.feature<0)	//Ӧ�ñ��и���Ķ���
				pPlate->vertex_list.DeleteCursor();
		}
		else 
		{
			if(pVertex->vertex.feature<0&&abs(pVertex->vertex.feature)<10)	//Ӧ�ñ��и���ķǻ�����
				pPlate->vertex_list.DeleteCursor();
		}
	}
	//�����һ������ͱ��и���Ļ�,�����ȥ�������ͻ���©���һ������
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
			AfxMessageBox("�ְ屻ȫ���и��!");
#endif
		return TRUE;
	}
	//������Ʊ��и���Ļ�������
	int i,n=pPlate->vertex_list.GetNodeNum();
	pPlate->face_N=face_N;
	if(pPlate->vertex_list.GetFirst()->vertex.feature==2&&pPlate->face_N==1)
	{	//�и�������ɵĵ����Ϊԭ��˫���ĵڶ���ʱ�����������ϵ
		pPlate->ucs.axis_z=pPlate->huoqufacenorm[0].normalized();
		pPlate->ucs.axis_x=pPlate->ucs.axis_y^pPlate->ucs.axis_z;
		normalize(pPlate->ucs.axis_x);
	}
	for(i=0;i<n;i++)
	{
		pVertex=&pPlate->vertex_list[i];
		if(face_N==1)
			pVertex->vertex.feature=face_N;
		if(pVertex->vertex.feature<-10)	//������ƻ�����
		{
			pVertex->vertex.feature=abs(pVertex->vertex.feature);
			f3dPoint *pPrevPoint=&pPlate->vertex_list[(i+n-1)%n].vertex;
			f3dPoint *pNextPoint=&pPlate->vertex_list[(i+1)%n].vertex;
			pPrevPoint->z=pNextPoint->z=0;	//Z�����������󽻿���ʧ�� wht 11-01-19
			f3dPoint inters,inters2;
			Int3dpl(f3dLine(*pPrevPoint,*pNextPoint),pPlate->HuoQuLine[0],inters);
			GEPOINT xCutPlanePick=pPlate->ucs.TransPToCS(face_pos);
			GEPOINT xCutPlaneNorm=pPlate->ucs.TransVToCS(face_norm);
			Int3dlf(inters2,pPlate->HuoQuLine[0],xCutPlanePick,xCutPlaneNorm);
			if ((inters-inters2)*xCutPlaneNorm>0)
				inters=inters2;	//���������и��潻�����ڱ��и����� wjh-2019.9.19
			pVertex->vertex.Set(inters.x,inters.y,inters.z);
		}
	}
	//ȥ�����ڰ巶Χ�ڵ���˨����
	for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		f3dPoint ls_pos=(*pLsRef)->ucs.origin;
		coord_trans(ls_pos,ucs,FALSE);
		if(ls_pos.z>0)
			pPlate->GetLsRefList()->DeleteCursor();
	}
	//�Ƴ����ڰ巶Χ�ڵ����߸˼�	wht 10-11-30
	CDesignLjPartPara *pLjPara=NULL;
	if(pPlate->jdb_style!=4)
	{	//��˨����е�һ�����ӹ�����ʵ���и�Ǹ֣������޳�
		for(pLjPara=pPlate->GetFirstLjPara();pLjPara;pLjPara=pPlate->GetNextLjPara())
		{
			//������߸˼���˨�����б�
			CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(pLjPara->hPart,CLS_LINEPART);
			if(pLinePart==NULL)
				continue;
			CLsRefList *pLsRefList=NULL;
			if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//�ֹ�
				CLDSParamPlate *pParamPlate=NULL;
				pParamPlate=((CLDSLineTube*)pLinePart)->GetParamPlate(pLjPara->start0_end1);
				if(pParamPlate)
					pLsRefList=pParamPlate->GetLsRefList();
			}
			else 
				pLsRefList=pLinePart->GetLsRefList();
			//��˨�����б�Ϊ�ջ��߸��б��еĲ������ְ��ϵ���˨����ɾ�������߸˼�
			if(pLsRefList==NULL)
				pPlate->DeleteLjPara(pLjPara->hPart);
			else
			{
				CLsRef *pLsRef=NULL;
				for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{
					if(pLsRefList->FromHandle(pLsRef->GetLsPtr()->handle))
						break;	//��˨�����б��а����ְ��ϵ���˨�������߸˼�Ϊ�ְ��ϵ����߸˼�
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

//�Գ��и�ְ�
//����ֵΪ�и�ضϺ�ְ�ľ��������Ƿָ��򷵻���������һ��ְ����� wjh-2015.5.14
long CLDSView::FinishCutPlate(CLDSPlate *pSrcPlate,CLDSPart *pDatumPart,
							  CCutPlateOptionDlg &cut_plate_dlg,BOOL bReCalCutFace)
{
	if(pSrcPlate==NULL||pDatumPart==NULL)
		return 0;
	f3dPoint face_pos,face_norm;
	if(bReCalCutFace)
	{	//1.���¼����и��淨���Լ��и����׼��
		if(pDatumPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate *pDatumPlate=(CLDSPlate*)pDatumPart;
			face_norm=pDatumPlate->ucs.axis_z;
			face_pos=pDatumPlate->ucs.origin+pDatumPlate->ucs.axis_z*pDatumPlate->m_fNormOffset;
			if(cut_plate_dlg.m_iTopFace==0)
			{	//���渺���߷����и�
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
			{	//���渺���߷����и�
				face_norm=-pParamPlate->ucs.axis_z;
				face_pos+=pParamPlate->ucs.axis_z*cut_plate_dlg.m_fTopBtmSpace;
			}
			else //���������߷����и�
				face_norm=pDatumPart->ucs.axis_z;
		}
		else
		{
			face_pos=pDatumPart->ucs.origin;
			if(cut_plate_dlg.m_iTopFace==0)
			{	//���渺���߷����и�
				face_norm=-pDatumPart->ucs.axis_z;
				face_pos+=pDatumPart->ucs.axis_z*pDatumPart->GetThick();
			}
			else //���������߷����и�
				face_pos=pDatumPart->ucs.axis_z;
		}
	}
	else
	{
		face_pos.Set(cut_plate_dlg.m_fCutFacePickX,cut_plate_dlg.m_fCutFacePickY,cut_plate_dlg.m_fCutFacePickZ);
		face_norm.Set(cut_plate_dlg.m_fCutFaceNormX,cut_plate_dlg.m_fCutFaceNormY,cut_plate_dlg.m_fCutFaceNormZ);
	}
	Ta.BeginUndoListen();
	pSrcPlate->designInfo.m_bEnableFlexibleDesign=FALSE;	//�ر����Ի����
	if(cut_plate_dlg.m_bWeld)	//���ú��Ӹ�����
		pSrcPlate->m_hPartWeldParent=pDatumPart->handle;
	BOOL bFinishCutPlate=FALSE;
	CLDSPlate *pNewPlate=NULL;
	if(cut_plate_dlg.m_bSaveAnotherPlate)
	{	//��¡�ְ�
		pNewPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
		long hNew=pNewPlate->handle;	//����ְ���
		pSrcPlate->CloneTo(*pNewPlate);	//��¡�ְ� wht 11-01-07
		pNewPlate->handle=hNew;			//���¸ְ���
		if(cut_plate_dlg.m_bWeld)
		{	//���ú��Ӹ�����
			pSrcPlate->m_hPartWeldParent=pDatumPart->handle;
			pNewPlate->m_hPartWeldParent=pDatumPart->handle;
			pNewPlate->m_bWeldPart=pSrcPlate->m_bWeldPart=TRUE;
		}
		pNewPlate->designInfo.iWeldLineStyle=pSrcPlate->designInfo.iWeldLineStyle=2;
		pNewPlate->relativeObjs.Empty();//��չ��������б�
		pNewPlate->EmptyLsRef();		//�����˨����
		//������˨����
		for(CLsRef *pLsRef=pSrcPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcPlate->GetNextLsRef())
			pNewPlate->AppendLsRef(*pLsRef);
		pNewPlate->SetModified();
		pNewPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pNewPlate->GetSolidPartObject());
		g_pSolidDraw->Draw();
		
		if(cut_plate_dlg.m_bWeld)	//���ú��Ӹ�����
			pNewPlate->m_hPartWeldParent=pDatumPart->handle;
		//��׼�ְ�Ϊ˫�����ѡ����˫���и� wht 11-01-20
		if(pDatumPart->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pDatumPart)->face_N==2&&cut_plate_dlg.m_iFaceNo==2)
			bFinishCutPlate=CutPlateByPlate(pSrcPlate,(CLDSPlate*)pDatumPart,cut_plate_dlg.m_iTopFace==0,cut_plate_dlg.m_fOffsetDist,TRUE,cut_plate_dlg.m_bWeld);
		else
			bFinishCutPlate=CutPlateByPlane(pSrcPlate,face_pos,face_norm,cut_plate_dlg.m_fOffsetDist,TRUE,cut_plate_dlg.m_bWeld);
		//�����и����׼���Լ��и��淨�߷���
		face_pos+=face_norm*cut_plate_dlg.m_fTopBtmSpace;
		face_norm*=-1.0;
		if(bFinishCutPlate)
		{	//��׼�ְ�Ϊ˫�����ѡ����˫���и� wht 11-01-20
			if(pDatumPart->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pDatumPart)->face_N==2&&cut_plate_dlg.m_iFaceNo==2)
				bFinishCutPlate=CutPlateByPlate(pNewPlate,(CLDSPlate*)pDatumPart,cut_plate_dlg.m_iTopFace!=0,cut_plate_dlg.m_fOffsetDist,TRUE,cut_plate_dlg.m_bWeld);
			else
				bFinishCutPlate=CutPlateByPlane(pNewPlate,face_pos,face_norm,cut_plate_dlg.m_fOffsetDist,TRUE,cut_plate_dlg.m_bWeld);
		}
		if(!bFinishCutPlate&&pNewPlate)
		{	//�и�����ְ岻�ཻ,�޷�����и�,ɾ���Ѹ��Ƶĸְ�
			console.DispPartSet.DeleteNode(pNewPlate->handle);
			g_pSolidDraw->DelEnt(pNewPlate->handle);
			console.DeletePart(pNewPlate->handle);
			Ta.EndUndoListen();
			return 0;
		}
	}
	else	//�и�ְ�
	{
		//��׼�ְ�Ϊ˫�����ѡ����˫���и� wht 11-01-20
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
{	//�и�ְ�,�����и����ɵ��������һ��
	CString cmdStr,errCmdStr;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSPlate *pSrcPlate=NULL,*pNewPlate=NULL;
	PARTSET cutPlateSet;
	CSuperSmartPtr<CLDSPart>pDatumPlate;
	try
	{
		CUndoOperObject undo(&Ta,true);
		//ѡ����Ҫ�и�ĸְ�
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
			scr_part_para.cmdStr="CutPlate ��ѡ����Ҫ�и�ĸְ�:";
			scr_part_para.cmdErrorFeedbackStr="û��ѡ�к��ʵĹ�����������ѡ����Ҫ�и�ĸְ�:";
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
		//ѡ�������и�ְ�Ļ�׼�ְ��ֱ�������и���
		BOOL bInputPlane=FALSE;	//�����и���
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CutPlate Please select datum plate to be used for cutting plate, or press enter to input cutting plane directly:");
		errCmdStr.Format("CutPlate Two plates are parallel, please reselect the datum plate used to cut plate, or press enter to input cutting plane directly:");
#else
		cmdStr.Format("CutPlate ��ѡ�������и�ְ�Ļ�׼�ְ�,��ֱ�Ӱ��س��������и���:");
		errCmdStr.Format("CutPlate ���ְ�ƽ�У�������ѡ�������и�ְ�Ļ�׼�ְ壬��ֱ�Ӱ��س��������и���:");
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
			{	//δѡ�񹹼�����ָ���и���
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
		//�и����׼���Լ��и��淨��
		if(bInputPlane)
		{	//�����и���
			g_pSolidDraw->ReleaseSnapStatus();
			static CDefFaceDlg	def_face_dlg;
			def_face_dlg.m_bCutPlate=TRUE;	//�и�ְ�
			if(def_face_dlg.DoModal()!=IDOK)
			{	
				pCmdLine->CancelCmdLine();
				return 0;
			}
			f3dPoint face_pos,face_norm;
			face_pos.Set(def_face_dlg.m_fBasePosX,def_face_dlg.m_fBasePosY,def_face_dlg.m_fBasePosZ);
			face_norm.Set(def_face_dlg.m_fFaceNormX,def_face_dlg.m_fFaceNormY,def_face_dlg.m_fFaceNormZ);
			face_pos+=face_norm*def_face_dlg.m_fBaseNormOffsetDist;
			//��ָ�����и�ְ�
			if(is_zero_vector(face_norm))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Specified cut plane's normals is empty, illegal cut plane!");
#else
				AfxMessageBox("ָ���ָ��淨��Ϊ��,Ϊ�Ƿ��ָ���!");
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
					AfxMessageBox("���ָܷ������������Ӹְ�");
#endif
					continue;
				}
				else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pSrcPlate->dwPermission))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("No permission to modify the connecting plate!");
#else
					AfxMessageBox("û�д����Ӱ���޸�Ȩ�ޣ�");
#endif
					continue;
				}
				//ָ���и����и�ְ�ʱ֧�ֱ�����һ��ĸְ� wht 11-05-30
				Ta.BeginUndoListen();
				pSrcPlate->designInfo.m_bEnableFlexibleDesign=FALSE;	//�ر����Ի����
				if(def_face_dlg.m_bSaveAnotherPlate)
				{
					BOOL bFinishCutPlate=FALSE;
					//��¡�ְ�
					CLDSPlate *pNewPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
					long hNew=pNewPlate->handle;	//����ְ���
					pSrcPlate->CloneTo(*pNewPlate);	//��¡�ְ� wht 11-01-07
					pNewPlate->handle=hNew;			//���¸ְ���
					pNewPlate->relativeObjs.Empty();//��չ��������б�
					pNewPlate->EmptyLsRef();		//�����˨����
					//������˨����
					for(CLsRef *pLsRef=pSrcPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcPlate->GetNextLsRef())
						pNewPlate->AppendLsRef(*pLsRef);
					pNewPlate->SetModified();
					pNewPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
					g_pSolidDraw->NewSolidPart(pNewPlate->GetSolidPartObject());
					g_pSolidDraw->Draw();
					
					bFinishCutPlate=CutPlateByPlane(pSrcPlate,face_pos,face_norm,0,TRUE,FALSE);
					//�����и����׼���Լ��и��淨�߷���
					face_pos-=face_norm*def_face_dlg.m_fBaseNormOffsetDist*2;
					face_norm*=-1.0;
					if(bFinishCutPlate)
						bFinishCutPlate=CutPlateByPlane(pNewPlate,face_pos,face_norm,0,TRUE,FALSE);
					if(!bFinishCutPlate&&pNewPlate)
					{	//�и�����ְ岻�ཻ,�޷�����и�,ɾ���Ѹ��Ƶĸְ�
						console.DispPartSet.DeleteNode(pNewPlate->handle);
						g_pSolidDraw->DelEnt(pNewPlate->handle);
						console.DeletePart(pNewPlate->handle);
						Ta.EndUndoListen();
						return 0;
					}
				}
				else	//�и�ְ�
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
			//��ʾ�и�ְ�����ϵ �����ж϶�/����
			//��ʾ��ֱ�������ϵ���������ڲ��/����
			CWorkPlaneSketchLifeObject workPlaneSketchLife;
			workPlaneSketchLife.WorkPlaneSketch(pDatumPlate->ucs.origin,pDatumPlate->ucs.axis_z,"�и�嶥��");
			workPlaneSketchLife.WorkPlaneSketch(pDatumPlate->ucs.origin,-1*pDatumPlate->ucs.axis_z,"�и�����");
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
				AfxMessageBox("ָ���ָ��淨��Ϊ��,Ϊ�Ƿ��ָ���!");
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
					AfxMessageBox("���ָܷ������������Ӹְ�");
#endif
					continue;
				}
				else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pSrcPlate->dwPermission))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("No permission to modify the connecting plate!");
#else
					AfxMessageBox("û�д����Ӱ���޸�Ȩ�ޣ�");
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
							MessageBox("�ְ�ָ�ɹ�!");
						else
							MessageBox("�ְ�ָ�ʧ��!");
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
							MessageBox("�ְ��и�ɹ�!");
						else
							MessageBox("�ְ��и�ʧ��!");
#endif
					}
				}
			}
			GEPOINT srcPlaneNorm=pDatumPlate->ucs.axis_z;
			pSrcPlate=(CLDSPlate*)cutPlateSet.GetFirst();
			if(cut_plate_dlg.m_bMirCut&&pSrcPlate&&cutPlateSet.GetNodeNum()==1)
			{	//�Գ��и�ְ� wht 11-01-07	
				CMirMsgDlg mir_dlg;
				CLDSPartPtr plateArr[8]={NULL},plateArr2[8]={NULL};
				plateArr[0]=pSrcPlate;
				plateArr2[0]=Ta.Parts.FromHandle(hNewCutPlate);
				long retCodeArr[7]={0};	//������¼�Գ��и���
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
					MessageBox("�Գ��и�ְ�ɹ�!");
				else
					MessageBox("�Գ��и�ְ�δ��ȫ���!");
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
{	//Բ�����и�ְ�
	m_nPrevCommandID=ID_CUT_PLATE_BY_COLUMN_FACE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat cylindrical-plane cut plate";
#else
	m_sPrevCommandName="�ظ�Բ�����и�ְ�";
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
			part_para.cmdStr="CutByColumnFace ��ѡ����Ҫ�и�ĸְ�:";
			part_para.cmdErrorFeedbackStr="û��ѡ�к��ʵĹ�����������ѡ����Ҫ�и�ĸְ�:";
#endif
			if(!GetPartsFromScr(part_para))
				return; 
			CLDSDbObject *pObj=part_para.pResultObjsArr[0];
			if(IsCanCutPlateByColumnFace(pObj))
				plateSet.append((CLDSGeneralPlate*)pObj);
			else
				return;
		}
		//ѡ��ҪԲ���и���Ļ�׼�ֹ�
		CLDSPart* pPart=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("CutByColumnFace Please select cylindrical-plane datum tube or sphere used to cut plate:","");
#else
		pCmdLine->FillCmdLine("CutByColumnFace ��ѡ�������и�ְ��Բ�����׼�ֹܻ�����:","");
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
		pCmdLine->FillCmdLine("CutByColumnFace �������и�����ɵ��¸ְ����и�����ļ�϶ֵ<1>:","");
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
			pCmdLine->FillCmdLine("CutByColumnFace ������������������и�����ɵ��¸ְ����и�����ļ�϶ֵ<1>:","");
#endif
		}
		double cut_radius;
		f3dPoint column_ucsaxis;	//���и�ְ��������ϵ�µ�Բ����Բ�ĵ㼰Բ������
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
				logerr.Log("�ְ�0X%X{%s}�ǵ���壬��ʱ��֧�������и���ڶ�������Ӹְ�",pPlate->handle,(char*)pPlate->GetPartNo());
#endif
				continue;
			}
			else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPlate->dwPermission))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("plate 0X%X{%s} No permission to modify the connecting plate!",pPlate->handle,(char*)pPlate->GetPartNo());
#else
				logerr.Log("�ְ�0X%X{%s}û�д����Ӱ���޸�Ȩ�ޣ�",pPlate->handle,(char*)pPlate->GetPartNo());
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
					cut_radius=pSphere->D*0.5;	//�����ڸְ����֮�϶���֮��,��ʱ����Ϊ�и�Բ����뾶
				else
				{	//�����ڸְ�֮��
					if(prjdist>0)
					{	//�����ڸְ嶥��֮��
						prjdist-=pPlate->GetThick();
						column_center+=pPlate->ucs.axis_z*pPlate->GetThick();
					}
					//else	//���ĸְ����֮��
					double R2=0.25*pSphere->D*pSphere->D-prjdist*prjdist;
					if(R2>0)
						cut_radius=sqrt(R2);
					else
					{
#ifdef AFX_TARG_ENU_ENGLISH
						logerr.Log("the distance between plate 0X%X{%s} and selected sphere is too far,  Circular Surface Cutting  failure!",pPlate->handle,(char*)pPlate->GetPartNo());
#else
						logerr.Log("�ְ�0X%X{%s}����ѡ��������Զ���ཻ�������и�ʧ�ܣ�",pPlate->handle,(char*)pPlate->GetPartNo());
#endif
						continue;
					}
				}
				column_line.startPt=column_center;
				column_line.endPt =column_center+column_norm*100;
				cut_radius+=fMinDist;
			}
			f3dPoint center_pick,elipse_center_pick;	//��Բʰȡ��Ϊ�������꣬�Ҳ�Ӧ�����������ͶӰ�������ܽ�ȡ���Z���꣩ wjh 2011.11.27
			Int3dlf(center_pick,column_line.startPt,column_norm,pPlate->ucs.origin+pPlate->ucs.axis_z*pPlate->m_fNormOffset,pPlate->ucs.axis_z);
			elipse_center_pick=center_pick;
			pPlate->designInfo.iProfileStyle0123=3;	//�ر����Ի����
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
			//���㶥��������ж����Ƿ���Ҫ���и��
			BOOL bWholeCir=TRUE;	//Բ�����ڸְ��м��и��һ����Բ
			SCOPE_STRU scope;
			for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			{
				scope.VerifyVertex(pVertex->vertex);	//�ְ�������
				double dd=DistOf3dPtLine(pVertex->vertex,column_ucsline.startPt,column_ucsline.endPt);
				if(dd<cut_radius)
				{
					pVertex->xTag.liTagValue=0;	//0:��ʾ��Ҫ���и��
					bWholeCir=FALSE;	//����Բ
				}
				else
					pVertex->xTag.liTagValue=1;	//1:��ʾ��Ҫ����
			}
			scope.fMinZ=center_pick.z-1;	//���������������Ӱ��
			scope.fMaxZ=center_pick.z+1;
			if(!scope.IsIncludePoint(center_pick))
				bWholeCir=FALSE;	//����Բ
			long hWeldParentPart=(pCutTube!=NULL)?pCutTube->handle:pSphere->handle;
			if(pPlate->m_hPartWeldParent!=0)
				pPlate->m_hPartWeldParent=hWeldParentPart;

			// Ϊ�ų������ɶ���������PROFILE_VERTEX::typeȡֵ�Թ�������ĸ��ţ����ؽ�vertex_list
			// ��������ԭ�����Ͻ����޸� wjh-2013.11.03
			f3dPoint inters1,inters2;
			PROFILE_VERTEX* pNewVertex;
			ATOM_LIST<PROFILE_VERTEX> vertex_list;
			pPrevVertex =pPlate->vertex_list.GetTail();
			for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			{
				if(pVertex->xTag.liTagValue==0)
				{
					if(pPrevVertex->xTag.liTagValue==1)
					{	//��Ҫɾ����ǰ����, ��ǰһ����Ҫ�����¶���
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
						pNewVertex->xTag.liTagValue=pVertex->xTag.liTagValue;	//��¼��ɾ���ڵ��ɾ����־λ��״̬
						pNewVertex->radius=-cut_radius;	//Բ�����и�Ķ�����Բ��
						pNewVertex->m_bWeldEdge=TRUE;	//��Բ�����и�ʱ������һ�㶼��Ҫ����
						pNewVertex->weldline.hWeldMotherPart=hWeldParentPart;
						if((pPrevVertex->vertex.feature==1&&fabs(pPlate->ucs.axis_z*column_norm)<EPS_COS) ||
							(pPrevVertex->vertex.feature==2&&fabs(pPlate->huoqufacenorm[0]*column_norm)<EPS_COS))
						{
							pNewVertex->type=3;
							pNewVertex->column_norm=column_norm;
							pNewVertex->center=elipse_center_pick;	//column_line.startPt;
						}
					}
					if(pVertex->vertex.feature>10)	//�������㡡
					{	//��Ҫ����ǰ����(������)��������
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
						pNewVertex->radius=-cut_radius;	//Բ�����и�Ķ�����Բ��
						pNewVertex->m_bWeldEdge=TRUE;		//��Բ�����и�ʱ������һ�㶼��Ҫ����
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
				{	//������ǰ���㣬ͬʱ��ǰһ���㱻�и�ʱ����ǰһ���и��
					if(pPrevVertex->xTag.liTagValue==0)
					{	//ǰһ��ȷ����Ҫ����һ���¶���
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
						if(pVertex->vertex.feature>10)	//������
							pNewVertex->vertex.feature=pVertex->vertex.feature;
					}
					else //if(pPrevVertex->center.feature==1)
					{	//���ǰһ���Ƿ�����м���Բ�����ཻ�����
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
									{	//Բ��Ϊ�����ڰ�Բ��
										if(ang[i]>EPS)
											ang[i]=2*Pi-ang[i];
										if(start_ang>EPS)
											start_ang=2*Pi-start_ang;
									}
									if(ang[i]<start_ang||ang[i]>start_ang+arcline.SectorAngle())
										ret--;
									if(i==0&&ret==1)	//�׶�����Ч
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
						{	//����ǰһ���Ƿ�����м���Բ�����ཻ�����
							bWholeCir=FALSE;	//����Բ
							//��������˳���ܷ�����Ϊ����붥��ʵ�ʲ����Ȳ��붥���ǰ��
							PROFILE_VERTEX *pInsertVertex1=vertex_list.append(PROFILE_VERTEX(inters1));
							PROFILE_VERTEX *pInsertVertex2=vertex_list.append(PROFILE_VERTEX(inters2));
							pInsertVertex1->vertex.feature=pInsertVertex2->vertex.feature=pPrevVertex->vertex.feature;
							pInsertVertex1->type=2;
							pInsertVertex1->center=center_pick;
							pInsertVertex1->xTag.liTagValue=pVertex->xTag.liTagValue;	//��¼��ɾ���ڵ��ɾ����־λ��״̬
							pInsertVertex2->xTag.liTagValue=pVertex->xTag.liTagValue;	//��¼��ɾ���ڵ��ɾ����־λ��״̬
							pInsertVertex1->radius=-cut_radius;	//Բ�����и�Ķ�����Բ��
							pInsertVertex1->m_bWeldEdge=TRUE;	//��Բ�����и�ʱ������һ�㶼��Ҫ����
							pInsertVertex1->weldline.hWeldMotherPart=hWeldParentPart;
							if(fabs(column_norm*pPlate->ucs.axis_z)<EPS_COS)
							{	//��Բ��
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
			{	//��Բ	//��ʱ��֧�ֵ����
				pPlate->m_tInnerOrigin=center_pick;		//��Բ��λ��
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
		pCmdLine->FillCmdLine("����:","");
#endif
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("command:","");
#else
		pCmdLine->FillCmdLine("����:","");
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
	m_sPrevCommandName="�ظ���˨���";
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
//�Զ���������
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
		pBoltPad->cfgword=pDatumPart->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		pBoltPad->iSeg=pDatumPart->iSeg;
		pBoltPad->jdb_style=4;	//��˨���
		pBoltPad->gasketEdgeSpace.berSpace=end_space;
		pBoltPad->gasketEdgeSpace.wingSpace=end_space;
		pBoltPad->gasketEdgeSpace.endSpaceS=end_space;
		pBoltPad->gasketEdgeSpace.endSpaceE=end_space;
		pBoltPad->designInfo.m_bEnableFlexibleDesign=TRUE;	//�����������
		for(CLDSBolt *pLs=boltSet.GetFirst();pLs;pLs=boltSet.GetNext())
			pBoltPad->AppendLsRef(pLs->GetLsRef());
		//
		if(pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			int x0_y1=2;
			f3dPoint inter_pos;
			if(pDatumPart.LineAnglePointer()->IsLsInWing(pBolt,&x0_y1))
			{	//ȷ������Z�᷽��
				if(x0_y1==0)
					pBoltPad->ucs.axis_z=pDatumPart.LineAnglePointer()->get_norm_x_wing();
				else if(x0_y1==1)
					pBoltPad->ucs.axis_z=pDatumPart.LineAnglePointer()->get_norm_y_wing();
				//����巨�������Ϣ
				pBoltPad->designInfo.norm.norm_style=1;
				pBoltPad->designInfo.norm.norm_wing=x0_y1;
				pBoltPad->designInfo.norm.direction=0;	//����
				pBoltPad->designInfo.norm.hVicePart=pDatumPart->handle;
			}
			//����������ϵ
			pBoltPad->ucs.axis_x=pDatumPart.LineAnglePointer()->End()-pDatumPart.LineAnglePointer()->Start();
			pBoltPad->cDatumAxis='X';
			normalize(pBoltPad->ucs.axis_x);
			pDatumPart.LineAnglePointer()->getUCS(pDatumPart->ucs);
			SnapPerp(&pBoltPad->ucs.origin,pDatumPart->ucs.origin,pDatumPart->ucs.origin+pBoltPad->ucs.axis_x*1000,pBolt->ucs.origin);
			pBoltPad->designInfo.m_hBasePart=pDatumPart->handle;
			if(pCutAngle)
			{
				if(boltSet.GetNodeNum()==1)//����һ����˨��ʱ��Ҫ��֤����˨����X�ᴹֱ�и��׼�Ǹ�
				{
					f3dPoint axis_y=pCutAngle->End()-pCutAngle->Start();
					pBoltPad->ucs.axis_x=pBoltPad->ucs.axis_z^axis_y;
					normalize(pBoltPad->ucs.axis_x);
				}
				//ѡ�����и���Ļ�׼�Ǹ֣������X�᷽��ָ���и��׼�Ǹ�
				f3dPoint vec=inter_pos-pBoltPad->ucs.origin;
				if(pBoltPad->ucs.axis_x*vec<0)
					pBoltPad->ucs.axis_x*=-1.0;
				//�洢��׼�Ǹֺ��и��׼�Ǹ��������ʱ��
				pBoltPad->designInfo.m_hBasePart=pDatumPart->handle;
				CDesignLjPartPara *pDesPara=pBoltPad->designInfo.partList.Add(pCutAngle->handle);
				pDesPara->hPart=pCutAngle->handle;
				pDesPara->m_nClassTypeId=CLS_LINEANGLE;
				//�ø˼������и���˨��壬�Գ�ʱ�������˼�λ�� wht 11-01-11
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
			//����������ϵ
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
			{	//ȡ����ƻ���Ƶ�ȦʱҪɾ����ǰ���ɵĵ��
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
			//����������ţ���ȡ�����κ�
			SEGI iSeg;
			if(ParsePartNo(plateParaDlg.m_sPartNo,&iSeg,NULL))
				pBoltPad->iSeg=iSeg;
		}
		//�����˨����,������˨ͨ��
		for(CLsRef *pLsRef=pBoltPad->GetFirstLsRef();pLsRef;pLsRef=pBoltPad->GetNextLsRef())
		{	//������˨���ߺ�����˨�������ߺż��ӳ��
			int nPush=pBoltPad->GetLsRefList()->push_stack();
			pBolt=pLsRef->GetLsPtr();
			pBolt->AddL0Thick(pBoltPad->handle,TRUE); 
			pBolt->CorrectBoltZnL0Para();
			pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			pBoltPad->GetLsRefList()->pop_stack(nPush);
		}
		pBoltPad->CalStdProfile();
		if(pCutAngle&&pBoltPad->GetLsCount()>1)	//�����Զ���˨�������и��
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
	{	//���ݵ������Ӹ˼���ȡ�и��׼�Ǹ�
		CDesignLjPartPara *pLjPara=pBoltPad->GetFirstLjPara();	//�и���Ļ�׼�Ǹ�
		if(pLjPara==NULL)
			return FALSE;
		pCutAngle=(CLDSLineAngle*)console.FromPartHandle(pLjPara->hPart,CLS_LINEANGLE);
	}
	if(pCutAngle&&pBoltPad->GetLsCount()>1)	//ֻ�ж�׵����Ҫ�����н�
	{
		f3dPoint face_pos,face_norm;
		pCutAngle->getUCS(pCutAngle->ucs);	//��ȡ�Ǹ�����ϵ����ֹ����ģʽ�½Ǹ�����ԭ��Ϊ(0,0,0)
		face_pos=pCutAngle->ucs.origin;
		if(fabs(pCutAngle->get_norm_x_wing()*pBoltPad->ucs.axis_z)>fabs(pCutAngle->get_norm_y_wing()*pBoltPad->ucs.axis_z))
			face_norm=pCutAngle->get_norm_y_wing();	//1.Y֫����Ϊ��ʼ�и��淨��
		else if(fabs(pCutAngle->get_norm_y_wing()*pBoltPad->ucs.axis_z)>fabs(pCutAngle->get_norm_x_wing()*pBoltPad->ucs.axis_z))
			face_norm=pCutAngle->get_norm_x_wing();	//2.X֫����Ϊ��ʼ�и��淨��
		//ʵ�ʵ���и�ʱ�����϶����ýǸ�֫����и��(��Ϊֻ�����߽Ǹִ�֫���������׼�Ǹ�ʱ����Ҫ���)
		face_pos=pCutAngle->ucs.origin-face_norm*(pCutAngle->GetWidth()+g_sysPara.VertexDist);
		CutPlateByPlane(pBoltPad,face_pos,face_norm,0,FALSE,FALSE);
		//�и��������Ե�����������ȡ��������
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
	//ѡ����Ļ�׼����
	CSuperSmartPtr<CLDSPart>pDatumPart;
	CString cmdStr,errCmdStr;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("BoltPad Please select datum parts (angle or plate) which need to add gasket:");
	errCmdStr.Format("BoltPad No proper parts selected, please select datum parts which need to add gasket:");
#else
	cmdStr.Format("BoltPad ��ѡ��Ҫ��ӵ��Ļ�׼����(�Ǹֻ�ְ�):");
	errCmdStr.Format("BoltPad û��ѡ�к��ʵĹ�����������ѡ��Ҫ��ӵ��Ļ�׼����(�Ǹֻ�ְ�):");
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
	//ѡ���������˨
	CUndoOperObject undo(&Ta);
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("BoltPad Please select all the bolts relative to gasket:");
	errCmdStr.Format("BoltPad number of effective bolt is zero, not all the selected bolts are on the datum angle, please select again:");
#else
	cmdStr.Format("BoltPad ��ѡ���������������˨:");
	errCmdStr.Format("BoltPad ��Ч��˨��Ϊ0������ѡ�����˨����ȫ�ڻ�׼�Ǹ��ϣ�������ѡ��:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
	try{
		OpenWndSel();	//����ѡ����˨
		while(1)
		{
			boltSet.Empty();
			g_pSolidDraw->ReleaseSnapStatus();
			for(pBolt=selectedBoltSet.GetFirst();pBolt;pBolt=selectedBoltSet.GetNext())
				g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
			//������˨��׽
			if(pCmdLine->GetStrFromCmdLine(cmdStr)==FALSE)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			num=g_pSolidSnap->GetLastSelectEnts(id_arr);
			for(int i=0;i<num;i++)
			{
				pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[i],CLS_BOLT);
				if(pBolt && pLsRefList->FromHandle(pBolt->handle))	//��˨�ڻ�׼������
					boltSet.append(pBolt);
			}
			if(boltSet.GetNodeNum()>0)
				break;
			pCmdLine->FillCmdLine(errCmdStr,"");
		}
		OnOperOther();	//ȡ����������״̬
		int x0_y1=0;
		if(pDatumPart->IsAngle()&&pDatumPart.LineAnglePointer()->IsLsInWing(boltSet.GetFirst(),&x0_y1)==FALSE)
			return 0;
		//ѡ���и���˨���Ļ�׼�Ǹ�
		CLDSLineAngle *pLineAngle=NULL;
		if(pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("BoltPad Please select the datum angle which need to cut gasket, if no, press enter directly:");
			errCmdStr.Format("BoltPad the Please select the datum angle which needs to cut gasket, if no, press enter directly");
#else
			cmdStr.Format("BoltPad ��ѡ��Ҫ�и���Ļ�׼�Ǹ֣���û��ֱ�ӻس�:");
			errCmdStr.Format("BoltPad ��ѡ�Ǹֿ��������׼�Ǹ�ƽ�У�������ѡ��Ҫ�и���Ļ�׼�Ǹ֣���û��ֱ�ӻس�");
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
		//�����˨���
		DesignBoltPad(pDatumPart,pLineAngle,boltSet);
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	BoltPad();	//Command("BoltPad");
	return 0;
}
//��ˮ��
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
		AfxMessageBox("��ѡ�ڵ㲻�ڸ˼���!");
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
	pPlateAttachLaunder->cfgword=pDatumTube->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
	pPlateAttachLaunder->ucs=ucs;
	pPlateAttachLaunder->SetPartNo(plateDlg.m_sPartNo.GetBuffer(0));
	pPlateAttachLaunder->iSeg=SEGI(plateDlg.m_sSegI.GetBuffer());
	pPlateAttachLaunder->layer(2)=pDatumTube->layer(2);	//����������ͼ����
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
	pPlateAttachLaunder->SetDesignItemValue('B',pBaseNode->handle,"��׼�ڵ�",PARAM_POS,1);
	pPlateAttachLaunder->SetDesignItemValue('C',pDatumTube->handle,"��׼�ֹ�",PARAM_POS,1);
	pPlateAttachLaunder->SetDesignItemValue('D',plateDlg.m_fInternalDiameter,"�ڻ�ֱ��",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('W',plateDlg.m_fExternalDiameter,"�⻷ֱ��",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('S',plateDlg.m_fNormalOffset,"����ƫ�ƾ���");
	pPlateAttachLaunder->SetDesignItemValue('K',plateDlg.m_fWaterSlotWidth,"��ˮ�ۿ��",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('H',plateDlg.m_fWaterSlotDepth,"��ˮ�����",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('L',plateDlg.m_fWaterSlotLen,"��ˮ�۳���",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('A',plateDlg.m_fWaterSlotAngle,"����ˮ��֮��ļн�",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('O',plateDlg.m_fOffsetAngle,"����ˮ�۽�ƽ�������׼��֮���ƫ�ƽ�",PARAM_SHAPE);
	pPlateAttachLaunder->SetDesignItemValue('E',plateDlg.m_iDatumLine,"��λ�ǻ�׼��:0.��׼��1.������");
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
	{	//������˨
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

// ����������� �Զ�����ʮ�ֲ�������90�ȵ����
// ��������ƣ�һ������λ��(1.ʹ�����Է��� 2.˫��� 3.�ֹ����)
//			   �������Ű�
//			   ������������ �ᵣ������λ�� 			
//			   �ġ�������ͨ�Ľڵ��  
// �����߰壬���һͬ���
void CLDSView::OnClassicConnectDesign() 
{

	//һ.
}

void CLDSView::DesignTubeEndLj(char cType)
{
	CLDSPart *pPart=NULL;
	CLDSNode *pBaseNode=NULL;
	CLDSLineTube *pLineTube=NULL;
	//����HashTable
	CXhChar100 sText;
	CString cmdStr;
	CHashTable<CLDSPart*>partsetTable;
	partsetTable.CreateHashTable(100);
	g_pSolidDraw->ReleaseSnapStatus();
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please tube which needs end design:","");
#else
	pCmdLine->FillCmdLine("��ѡ����Ҫ���ж�ͷ��Ƶĸֹ�:","");
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
		pCmdLine->FillCmdLine("û��ѡ�к��ʵĸֹܣ�������ѡ����Ҫ���ж�ͷ��Ƶĸֹ�:","");
#endif
	}
	g_pSolidDraw->SetEntSnapStatus(pLineTube->handle);
	pCmdLine->FinishCmdLine(CXhChar16("0X%X",pLineTube->handle));
	if(pBaseNode==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select current tube end node:","");
#else
		pCmdLine->FillCmdLine("��ѡ��ֹܵĵĶ�ͷ�ڵ�:","");
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
	//Ԥ�иֹܶ���������
	if(FindTubeInLine(pLineTube,endLjDlg.m_bStartEnd)&&pBaseNode->m_cPosCalType!=4)
		endLjDlg.m_iTubeEndLjStyle=2;	//����
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
int CLDSView::DesignTubeEndLjTypeD()	//�ֹܶ�����
{
	DesignTubeEndLj('D');
	return 0;
}
int CLDSView::DesignTubeEndLjTypeO()	//����
{
	DesignTubeEndLj('O');
	return 0;
}
int CLDSView::DesignTubeEndLjTypeU()	//U�Ͳ��
{
	DesignTubeEndLj('U');
	return 0;
}
int CLDSView::DesignTubeEndLjTypeC()	//C�Ͳ��
{
	DesignTubeEndLj('C');
	return 0;
}
int CLDSView::DesignTubeEndLjTypeX()
{
	DesignTubeEndLj('X');	//X�ֹܶ�����
	return 0;
}
int CLDSView::DesignTubeEndLjTypeTJ()//�������
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
			bSameDesign=FALSE;//��ʱ�ص�ʮ�ֲ��ʱ���������Զ�������ƹ���,��������ڷ��ߵĲ�������Ƶ��´��� wjh 2011-12-8
	}
	else if(pRayLinePart1->GetClassTypeId()==CLS_LINEANGLE)
	{	//���߸˼�Ϊ�Ǹ�ʱ����ƽڵ��
		g_pSolidDraw->SetEntSnapStatus(pRayLinePart1->handle);
		CLDSLineAngle* pRayLineAngle=(CLDSLineAngle*)pRayLinePart1;
		if(pEndLjDlg->m_bDesignNodePlate)
			pNodePlate1=pEndLjDlg->CreateNodePlate(pBaseLineTube,pRayLineAngle);
	}
	g_pSolidDraw->ReleaseSnapStatus();
	if(pRayLinePart2&&pRayLinePart2->GetClassTypeId()==CLS_LINEANGLE)
	{	//���߸˼�Ϊ�Ǹ�ʱ����ƽڵ��
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
			//��ո��ӹ�������
			pParamPlate->AttachPart.Empty();
			pParamPlate->SetLayer(pLineTube2->layer());	//ͼ����
			pParamPlate->cfgword=pLineTube2->cfgword;
			//���η������߸˼� ��߰�U�Ͱ��Զ������ֹܹ����淨�߷���
			if(pEndLjDlg->m_bSpecialFlRayPole&&(pSrcParamPlate->m_iParamType==TYPE_ROLLEND
				||pSrcParamPlate->m_iParamType==TYPE_UEND))
			{	
				pLineTube2->m_iNormCalStyle=0;	//ֱ��ָ�������淨��
				pLineTube2->SetWorkPlaneNorm(f3dPoint(0,0,1));
			}
			double fOddment=pLineTube1->endOdd();	
			pCurDesEnd->fNormOffset=pLineTube1->desEndPos.fNormOffset;	//�ֹܷ���ƫ����
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
			pLineTube2->CalPosition();	//����ֹ�λ��
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
			pParamPlate->EmptyLsRef();	//�����˨����
			for(pLsRef=pSrcParamPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcParamPlate->GetNextLsRef())
			{
				CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
				pLsRef->GetLsPtr()->CloneTo(*pBolt);
				pBolt->SetLayer(pParamPlate->layer());
				//pBolt->layer(2)=pLineTube2->layer(2);	//ͼ����
				pBolt->cfgword=pParamPlate->cfgword;	//������˨��ĺ����׼������ĺ�һ��
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
					pBolt->EmptyL0DesignPara();	//���ͨ����Ʋ���
					pBolt->des_base_pos.norm_offset.EmptyThick();	//��շ���ƫ������Ʋ���
					pBolt->des_base_pos.norm_offset.AddThick(ftol(-pParamPlate->GetThick()*0.5));
					pNewLsRef->start1_end2_mid0=pLsRef->start1_end2_mid0;
					//���巶Χ�ڵ���˨���ͨ�����
					if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
						pBolt->AddL0Thick(pParamPlate->handle,TRUE,TRUE);
				}
			}
			//
			if(pParamPlate->m_iParamType==TYPE_XEND)
			{	//����ʮ�ֲ�帽�Ӹְ�
				pEndLjDlg->m_pCurLineTube=pLineTube2;
				//��˨Ϊ��¡��˨ʱ�����ò��ʱ����ȷָ������������������� wht 11-05-27
				pEndLjDlg->LayoutXEndSubPlate(pParamPlate,TRUE);
				if(pEndLjDlg->m_bCreateFillPlate)	//����ʮ�ֲ����� wht 11-05-26
					pEndLjDlg->DesignXEndFillPlate(pParamPlate);
			}
			//��ƽڵ��
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
	{	//ƽ���и�ֹ�
		pCurJoint->type=0;
		pCurJoint->cutPlanePos=face_pos;
		pCurJoint->cutPlaneNorm=face_norm;
		if(!face_norm.IsZero())
		{	//���Ϊƽ���и��Զ�����һ����С�ĸ�ͷ
			f3dPoint inters,temp_vec;
			Int3dlf(inters,cur_pos,tube_len_vec,face_pos,face_norm);
			double min_dist=DISTANCE(inters,cur_pos);
			double angle=cal_angle_of_2vec(tube_len_vec,face_norm);
			double tana=fabs(tan(angle));
			//����==>��ǰ��ƶ�λ��
			temp_vec=cur_pos-inters;
			normalize(temp_vec);
			if(temp_vec*tube_len_vec>0)	//��Ҫ���ø�ͷ
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
//�����ְ�ڵ������---���ڵ�����������������и�ְ�
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
		{	//������Բ������ƽ���Ĵ�λ����
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
		//�и�ְ�ʧ�ܣ���Ҫ����ְ�
		if(!pView->CutPlateByPlane(pCurPlate,face_pos,face_norm,0,FALSE))
			pView->ExtendPlateToPlane(pCurPlate,face_pos,face_norm,&tube_len_vec,fWidth);
		//�����ְ����и����ཻ�ߵĳ���pDatumPlate
		//���Ҹְ����и����ཻ��
		PROFILE_VERTEX *pVertex=NULL,*pWeldStartVertex=NULL,*pWeldEndVertex=NULL;
		CMinDouble btm_start,btm_end;
		UCS_STRU datumcs=pDatumPlate->ucs;
		datumcs.origin=face_pos;
		for(pVertex=pCurPlate->vertex_list.GetFirst();pVertex;pVertex=pCurPlate->vertex_list.GetNext())
		{
			f3dPoint vertex=pVertex->vertex;
			coord_trans(vertex,pCurPlate->ucs,TRUE);
			//�Ըְ����Բ�Ϊ�󽻻�׼
			double offsetToMidPlane=pCurPlate->GetNormOffset()+pCurPlate->Thick*0.5;
			if(fabs(offsetToMidPlane)>EPS)
				vertex+=offsetToMidPlane*pCurPlate->ucs.axis_z;
			coord_trans(vertex,datumcs,FALSE);
			btm_start.Update(fabs(vertex.z),pVertex,1);
			if(btm_start.m_pRelaObj!=pVertex)
				btm_end.Update(fabs(vertex.z),pVertex,1);
		}
		if(btm_start.IsInited()&&btm_end.IsInited()&&fabs(btm_start.number-btm_end.number)<1)
		{	//�ҵ����׼�װ������ϵ���ͱ�
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
				else	//��Բ
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
//���ⷨ�����(����ˮƽ���������´����η���)
void CLDSView::OnDesignSpecialFl() 
{	
	m_nPrevCommandID=ID_DESIGN_SPECIAL_FL;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat tower foot && Changing slope design";
#else
	m_sPrevCommandName="�ظ�����&&�������";
#endif
	DesignSpecialFl();
}

//��������&������ơ���Ϊ�������� wht 11-05-20
//�������
void CLDSView::OnDesignTubeTowerFoot() 
{
	m_nPrevCommandID=ID_DESIGN_TUBE_TOWER_FOOT;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat tower foot design";
#else
	m_sPrevCommandName="�ظ��������";
#endif
	DesignSpecialFl();
}
//������� 
void CLDSView::OnDesignTubeTowerSlope() 
{
	m_nPrevCommandID=ID_DESIGN_TUBE_TOWER_SLOPE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat changing slope design";
#else
	m_sPrevCommandName="�ظ��������";
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
	//ѡ���׼�ڵ�
	CLDSNode *pBaseNode=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("%s please select datum node:",bDesignSlope?"SpecialFl":"TubeFoot");
#else
	cmdStr.Format("%s ��ѡ���׼�ڵ�:",bDesignSlope?"SpecialFl":"TubeFoot");
#endif
	CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_POINT);
	verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
	{//����͹����ʾ�ڵ����������
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
	pCmdLine->FinishCmdLine(CXhChar50("�ڵ�0X%X",pBaseNode->handle));
	g_pSolidDraw->SetEntSnapStatus(pBaseNode->handle);
	//ѡ���׼�ֹ�
	CLDSLineTube *pUpBaseTube=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("%s please select datum tube:",bDesignSlope?"SpecialFl":"TubeFoot");
#else
	cmdStr.Format("%s ��ѡ���׼�ֹ�:",bDesignSlope?"SpecialFl":"TubeFoot");
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
	pCmdLine->FinishCmdLine(CXhChar50("�ֹ�0X%X",pUpBaseTube->handle));
	g_pSolidDraw->SetEntSnapStatus(pUpBaseTube->handle);
	//ѡ����³��ڶ�����׼�ֹ�
	CLDSLineTube *pDownBaseTube=NULL;
	if(bDesignSlope)
	{	//�������
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("SpecialFl please second the second datum tube:");
#else
		cmdStr.Format("SpecialFl ��ѡ��ڶ���������׼�ֹ�:");
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
		pCmdLine->FinishCmdLine(CXhChar50("�ֹ�0X%X",pDownBaseTube->handle));
		g_pSolidDraw->SetEntSnapStatus(pDownBaseTube->handle);
	}
	//�л���ʵ����ʾģʽ
	DISPLAY_TYPE displayType;
	g_pSolidSet->GetDisplayType(&displayType);
	if(displayType!=DISP_SOLID)
	{
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		Invalidate(FALSE);
	}
	//��������׼�ֹ�λ��
	CLDSLineTube* pDatumTube=pUpBaseTube;
	double fMidZ=0.5*(pUpBaseTube->Start().z+pUpBaseTube->End().z);
	if(fMidZ-pBaseNode->Position(true).z>0)
	{
		CLDSLineTube *pTempTube=pDownBaseTube;
		pDownBaseTube=pUpBaseTube;
		pUpBaseTube=pTempTube;
	}
	//���ݽڵ�������߸˼�
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
		if(fabs(cosa)<EPS2) //��������߸˼�
			horiRodSet.append(pLinePart);
		else if(cosa<0)	//�ϲ����߸˼�
			UpLinePartSet.append(pLinePart);
		else			//�²����߸˼�
			DownLinePartSet.append(pLinePart);
	}
	static CDesignTubeEndLjDlg endLjDlg;
	endLjDlg.m_pBaseNode=pBaseNode;
	endLjDlg.m_bDesignNodePlate=TRUE;
	CUndoOperObject undo(&Ta,true);
	if((pUpBaseTube&&pDownBaseTube==NULL)||(pDownBaseTube&&pUpBaseTube==NULL))
	{
		//��Ƹֹܲ��
		CLDSParamPlate *pSrcParamPlate=NULL;
		CLDSLineTube *pLineTube1=NULL,*pLineTube2=NULL;
		CLDSPlate *pNodePlate[2]={NULL,NULL};
		//��ƽڵ��
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
		//��ƻ���壬�ڵ�壬�����壬�����߰壬�����ڵ��
		CDesignSpecialFlDlg specialFlDlg;
		specialFlDlg.m_pRayLinePart[0]=UpLinePartSet.GetFirst();
		specialFlDlg.m_pRayLinePart[1]=UpLinePartSet.GetNext();
		specialFlDlg.m_pNodePlate[0]=pNodePlate[0];
		specialFlDlg.m_pNodePlate[1]=pNodePlate[1];
		specialFlDlg.m_pBaseNode=pBaseNode;
		specialFlDlg.m_pCurLineTube=pBaseLineTube;
		if(specialFlDlg.DoModal()==IDOK)
		{	//�����ְ�ڵ������---���ڵ�����������������и�ְ�
			CLDSPlate *pDatumPlate=specialFlDlg.m_pCirPlate;
			AdjustNodePlateProfile(this,pBaseNode,pBaseLineTube,pDatumPlate,pNodePlate[0],pNodePlate[1]);
		}
	}
	else if(horiRodSet.GetNodeNum()<2)
		AfxMessageBox("ˮƽ���ӵĺ�ĸ˼�����2��");
	else if(horiRodSet.GetNodeNum()>=2&&pUpBaseTube&&pDownBaseTube)
	{	//���´����η�������
		//1>.���ɺ�Ĳ��
		CLDSPlate *pNodePlate[4]={NULL,NULL};
		endLjDlg.m_bDesignNodePlate=FALSE;
		endLjDlg.m_bSpecialFlRayPole=TRUE;	//���η������ӵ����߸˼�
		DesignRayPoleAndPlate(pDownBaseTube,&endLjDlg,horiRodSet,NULL,pNodePlate[0],pNodePlate[1]);
		//2>.�������²�б�ĸֹܶ�ͷ���
		//3>.�������²�ڵ��,������Ϊʮ�ֲ��
		//   �����������˨���¼�������,�������巨��ͬ�������淨��һ��
		endLjDlg.m_bDesignNodePlate=TRUE;
		endLjDlg.m_bSpecialFlRayPole=FALSE;	//���η������ӵ����߸˼�
		DesignRayPoleAndPlate(pUpBaseTube,&endLjDlg,UpLinePartSet,&horiRodSet,pNodePlate[0],pNodePlate[1]);
		DesignRayPoleAndPlate(pDownBaseTube,&endLjDlg,DownLinePartSet,&horiRodSet,pNodePlate[2],pNodePlate[3]);
		//4>.�������η���,����������˨
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
		//5>.ʹ�����η����и�ڵ��
		AdjustNodePlateProfile(this,pBaseNode,pUpBaseTube,pParamPlate,pNodePlate[0],pNodePlate[1]);
		AdjustNodePlateProfile(this,pBaseNode,pDownBaseTube,pParamPlate,pNodePlate[2],pNodePlate[3]);
		//6>.ƽ���и��׼�ֹ�
		DesignMainTubeEndLj(pBaseNode,pUpBaseTube,pParamPlate);
		DesignMainTubeEndLj(pBaseNode,pDownBaseTube,pParamPlate);
		/* ʵ��ʹ�ù��������������廹�����û������ֶ���Ӹ����� wjh-2018.3.1
		//7>.���ϲ�ڵ���������Ϸ�壬�²�ڵ���������·��
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
			//coverPlateDlg.m_iPlateType=1;	//���
			coverPlateDlg.idPlateType=CDesignCircularPlateDlg::TYPE_COVERPLATE2;
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pNodePlate[2*i]->handle);
			g_pSolidDraw->SetEntSnapStatus(pNodePlate[2*i+1]->handle);
			if(i==0)
				coverPlateDlg.para.S=1;	//�ϲ���
			else 
				coverPlateDlg.para.S=-1;	//�²���
			if(coverPlateDlg.DoModal()!=IDOK)
				return;
			coverPlateDlg.CreateCircularPlate(NULL);
			pCoverPlate[i]=coverPlateDlg.m_pCirPlate;
			//8>.�Է��+���η���Ϊ��׼�ڽڵ�����λ�����������߰�
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
			//9>.�Խڵ�弰�߰�Ϊ��׼������֮�����ɻ����߰�
			CDesignCircularPlateDlg xCirPlateDlg;
			//xCirPlateDlg.m_iPlateType=3;	//3WA�����߰�
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
			//10>.���߰�����η���Ϊ��׼���ɼӾ��߰�
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
		//������η���,����壬��壬�߰壬���
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
		{	//�����ְ�ڵ������---���ڵ�����������������и�ְ�
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
	//�л���������ʾģʽ
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("����:","");
#endif
	pCmdLine->FinishCmdLine();
	g_pSolidDraw->Draw();
#endif
}

//����ְ�
void CLDSView::OnMakePlateBend() 
{
	m_nPrevCommandID=ID_MAKE_PLATE_BEND;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="bending plate";
#else
	m_sPrevCommandName="����ְ�";
#endif
	BendPlate();
}

int CLDSView::BendPlate()
{
	f3dPoint vertice;
	CString cmdStr,promptStr;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSPlate *pDatumPlate=NULL;
	//ѡ���׼�ְ�
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
		pCmdLine->FillCmdLine("MakePlateBend ��ѡ����Ҫ����Ļ�׼�ְ�:","");
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
		AfxMessageBox("��ѡ�ְ�Ϊ��������޷�������������!");
#endif
		return FALSE;
	}
	Ta.BeginUndoListen();
	//��ʾ����ְ�����ϵ
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
	//ѡ�������׼�Ǹ�
	CLDSLineAngle *pDatumLineAngle=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("MakePlateBend Please select bending datum angle, if no datum angle, specify plane to bend plate<no selection is permitted,press enter to determine>:","");
#else
	pCmdLine->FillCmdLine("MakePlateBend ��ѡ�������׼�Ǹ�,�޻�׼�Ǹ�ʱͨ��ָ��ƽ������ְ�<�ɲ�ѡ,���س�ȷ��>:","");
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
	f3dPoint huoqu_norm;	//�����淨��
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
				AfxMessageBox("ָ��ƽ�����׼�ְ岻�ཻ!");
#endif
				return FALSE;
			}	
			//�������潻�����׼�ְ������ߵĽ���
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
					AfxMessageBox("����Ļ���ƽ��������ƽ�潻�����׼�ְ�ǰһ���������غ�!");
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
					if(pPrevVertex->type==1)//ָ��Բ���н�(�Զ�����Բ���нǵ�����������Զ��������ȷֵ)
					{
						if(pPrevVertex->sector_angle>0)
							sign=-1;
						arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),pPrevVertex->sector_angle);
					}
					else if(pPrevVertex->type==2)
					{	//ָ��R
						if(pPrevVertex->radius<0)
							sign=-1;
						arcLine.CreateMethod3(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),sign*pPrevVertex->radius,pPrevVertex->center);
						//������CreateMethod2,����ʧ��ʰȡ����Ϣ�����ڴ���180��Ļ��Ǵ�ġ� wht 12-06-14
						//arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),sector_angle);
					}
					else if(pPrevVertex->type==3)	//��Բ��
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
					{	//�˴���δ����ֱ����(��)Բ����������ʱ�Ľ���ȡ������
						if(pPrevVertex->type==3)
						{	//��Բ��
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
						{	//ָ��Բ���н�
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
				AfxMessageBox("��ƽ�潻�����׼�ְ��������޽���!");
#endif
				return FALSE;
			}
			//���������߷��򣬱�֤����Ļ�����Ϊ��2��3������ 
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
				pDatumPlate->jdb_style=1;//֮ǰ�۵��帳ֵΪ3�Ǵ�Ļ���ֹܼа����������������Ϊ1��������IsFoldPlate()���� wjh-2016.1.09
			pDatumPlate->HuoQuLine[pDatumPlate->face_N-2].startPt=pHuoquStartVertex->vertex;
			pDatumPlate->HuoQuLine[pDatumPlate->face_N-2].endPt=pHuoquEndVertex->vertex;
			pDatumPlate->huoqufacenorm[pDatumPlate->face_N-2]=bendDlg.huoqu_norm;
			huoqu_norm=bendDlg.huoqu_norm;	//��¼�����淨��
			bBendPlate=TRUE;
		}
	}
	else 
	{	//���ݻ�׼�Ǹ�����ְ�
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
			{	//X֫Ϊ��ǰ����֫
				offset_vec=pDatumLineAngle->GetWingVecX();
				pick_vec=pDatumLineAngle->get_norm_y_wing();
				if(bendDlg.m_iDatumEdge==0)	//�����������߲�
					offset_vec*=-1.0;
				else	//��������֫���
					pick_vec*=-1.0;
			}
			else
			{	//Y֫Ϊ��ǰ����֫
				offset_vec=pDatumLineAngle->GetWingVecY();
				pick_vec=pDatumLineAngle->get_norm_x_wing();
				if(bendDlg.m_iDatumEdge==0)	//�����������߲�
					offset_vec*=-1.0;
				else	//��������֫���
					pick_vec*=-1.0;
			}
			if(bendDlg.m_iDatumEdge==1)
			{	//֫���
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
					pDatumPlate->jdb_style=1;//֮ǰ�۵��帳ֵΪ3�Ǵ�Ļ���ֹܼа����������������Ϊ1��������IsFoldPlate()���� wjh-2016.1.09
				//����������λ��
				huoqu_start+=offset_vec*bendDlg.m_fBendClearSpace;
				huoqu_end+=offset_vec*bendDlg.m_fBendClearSpace;
				coord_trans(huoqu_start,pDatumPlate->ucs,FALSE);
				coord_trans(huoqu_end,pDatumPlate->ucs,FALSE);
				huoqu_start.z=huoqu_end.z=0;
				pDatumPlate->HuoQuLine[pDatumPlate->face_N-2].startPt=huoqu_start;
				pDatumPlate->HuoQuLine[pDatumPlate->face_N-2].endPt=huoqu_end;
				//�����������ڰ������ϵ����������
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
					if(prev_min_face>1||cur_min_face>1)	//���ǻ����ϵ�������
						continue;
					else
					{
						f3dPoint inters;
						if(Int2dpl(huoquline2d,edge_line,inters.x,inters.y)>0)
						{	//��ֱ���н���
							double justify;	//������Ч���ж�
							if(fabs(edge_line.endPt.x-edge_line.startPt.x)>eps)
								justify=(inters.x-edge_line.startPt.x)/(edge_line.endPt.x-edge_line.startPt.x);
							else
								justify=(inters.y-edge_line.startPt.y)/(edge_line.endPt.y-edge_line.startPt.y);
							if(justify>=-eps&&justify<=eps)		//ǰһ����Ϊ�����
								pPrevVertex->vertex.feature=10+pDatumPlate->face_N;
							else if(justify>eps&&justify<1-eps)	//����Ϊ�����ߵ��ڵ�
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
									break;	//���ҵ������������ߵ�����ƥ����������
								}
							}
							else if(justify>=1-eps&&justify<=1+eps)	//��ǰ����Ϊ�����
							{
								pVertex->vertex.feature=10+pDatumPlate->face_N;
								if(pHuoquStartVertex==NULL)
									pHuoquStartVertex=pVertex;
								else if(pHuoquEndVertex==NULL)
								{
									pHuoquEndVertex=pVertex;
									break;	//���ҵ������������ߵ�����ƥ����������
								}
							}
						}
					}
				}
				if(pHuoquStartVertex&&pHuoquEndVertex)
					bBendPlate=TRUE;
				else
					bBendPlate=FALSE;	//�����������⣬�޷��ҵ�����������ƥ�����������
			}
			//ת�����ְ�ֲ�����ϵ�Ա���������߷������֤ wjh-2016.4.28
			vector_trans(huoquline_vec,pDatumPlate->ucs,FALSE,TRUE);
		}
		else
			return FALSE;
	}
	if(bBendPlate)
	{	//����ְ�
		GEPOINT verify_huoquvec=pHuoquEndVertex->vertex-pHuoquStartVertex->vertex;
		if(huoquline_vec*verify_huoquvec<0)
		{
			PROFILE_VERTEX* pTempVertex=pHuoquEndVertex;
			pHuoquEndVertex=pHuoquStartVertex;
			pHuoquStartVertex=pTempVertex;
		}
		pDatumPlate->designInfo.m_bEnableFlexibleDesign=FALSE;	//�ر��������
		BOOL bStart=FALSE;
		pVertex=pDatumPlate->vertex_list.GetFirst();
		while(1)
		{
			if(bStart)	//��ʼ������
			{
				if(pVertex==pHuoquEndVertex)
				{
					pVertex->vertex.feature=pDatumPlate->face_N+10;
					break;		//��ֹ������
				}
				if(pVertex->vertex.feature==1)
					pVertex->vertex.feature=pDatumPlate->face_N;
			}
			else		//���һ����濪ʼ��
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
		//TODO:�������Ϊ2010�꺣����ʱ��������׼������Բ���ߵ�,һֱδ���� wjh-2015.2.7
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
			{	//ǰһ��������Ϊֱ��ʱ�ſ��Զ�������Բ������ wht 10-06-23
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
			{	//��һ��������Ϊֱ��ʱ�ſ��Զ�������Բ������
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
		//��ɻ����ְ�󣬸��������ɵĻ���������˨�ķ���
		f3dLine huoquline;
		CLsRef *pLsRef=NULL;
		for(pLsRef=pDatumPlate->GetFirstLsRef();pLsRef;pLsRef=pDatumPlate->GetNextLsRef())
		{
			f3dPoint ls_pos=(*pLsRef)->ucs.origin;
			//����˨λ��ת�����ְ�����ϵ��
			coord_trans(ls_pos,pDatumPlate->ucs,FALSE);
			ls_pos.z=0;	//Z������� ��������ȷ�ж���˨�Ƿ�λ�ڻ�������
			if(DistOf2dPtLine(pHuoquStartVertex->vertex,pHuoquEndVertex->vertex,ls_pos)<0)
			{	//��˨�ڻ������Ҳ�,˵����˨Ϊ�������ϵ���˨,�������˨���߷���
				if((*pLsRef)->des_base_pos.datumPoint.datum_pos_style==0)
				{	//λ�����û�ָ������˨�������˨������
					(*pLsRef)->des_base_pos.cLocationStyle=true;
					(*pLsRef)->des_base_pos.reside_plane=(BYTE)pDatumPlate->face_N-1;
					(*pLsRef)->des_base_pos.hPart=pDatumPlate->handle;
					(*pLsRef)->des_base_pos.len_offset_dist=ls_pos.x;
					(*pLsRef)->des_base_pos.wing_offset_dist=ls_pos.y;
				}
				(*pLsRef)->set_norm(huoqu_norm);
				(*pLsRef)->des_work_norm.norm_style=3;	//������������
				(*pLsRef)->des_work_norm.hVicePart=pDatumPlate->handle;		//��׼��
				(*pLsRef)->des_work_norm.reside_plane=(BYTE)pDatumPlate->face_N-1;//������������
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
		AfxMessageBox("û���ҵ�ƥ�䵽�㹻�����䶥��,�ְ�����ʧ��!");
#endif
	//���ò���ʾ��������ϵ
	//g_pSolidSet->SetDispWorkUcs(FALSE);
	g_pSolidDraw->BatchClearCS(2);
	Invalidate(FALSE);
	Ta.EndUndoListen();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("����:","");
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
