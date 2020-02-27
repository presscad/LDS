#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "TtaDataFile.h"
#include "SortFunc.h"
#include "UnifyModelLayer.h"
#include "CorrectAngleLayout.h"
#include "TowerTreeDlg.h"
#include "MainFrm.h"
#include "FoundationPropDlg.h"
#include "Rapid3dModelDlg.h"
#include "./IntelliAgent/IntelliAgent.h"
#include "UI.h"
#include "DesignQuadSection.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#ifndef __TIMER_COUNT_
//#define __TIMER_COUNT_
//#endif
//#define __TIMER_COUNT_
#ifdef __TIMER_COUNT_
#include "TimerCount.h"
#endif
//统一四角塔塔身图层名
void CLDSView::IntelliCorrectModelObjsLayer()
{
	//杆塔结构类型，四角塔，三角塔
	//统一图层视图（增加俯视图及三角塔1-2（前）、1-3（右后）、2-3（左后））
	CUnifyModelLayer unify;
	CLogErrorLife loglife;
	unify.UnifyObjsLayer();
}
void CLDSView::OnIntelliCorrectModelObjsLayer(){IntelliCorrectModelObjsLayer();}
//是否为长杆件隶属度
static double CalLongRodFactor(double length)
{
	if(length>4000)
		return 1;	//长杆件，可认为是主框架杆件
	else if(length<2000)	//4000000=2000*2000
		return 0;	//短杆件，不被认为是框架主杆件
	else
	{
		double MID_LENGTH=3000;	//3000=(2000+4000)/2;
		double LENGTH_SPAN=2000;//4000-2000;
		double HALF_LENGTH_SPAN=LENGTH_SPAN*0.5;
		if(length<=MID_LENGTH)
		{
			double len_dist=length-2000;
			double factor=len_dist/2000;
			return 2*factor*factor;
		}
		else //if(length>MID_LENGTH)
		{
			double len_dist=length-MID_LENGTH;
			double factor=len_dist/HALF_LENGTH_SPAN;
			double root=sqrt(factor);
			return 0.5+root*0.5;
		}
	}
}
//是否为正交杆件的隶属度
static double CalRodOrthoFactor(double dx,double dz)
{
	if(fabs(dz)<1)	//水平杆件
		return 1.0;
	else if(fabs(dx)<1)
		return 1.0;	//竖直杆件
	if(fabs(dx)>fabs(dz))
		return 1.0-fabs(dz/dx);
	else //if(fabs(dx)<fabs(dz))
		return 1.0-fabs(dx/dz);
}
double SimilarRatioToAxisXYZ(const double* vec3d)
{
	GEPOINT vec(vec3d);
	normalize(vec);
	vec.x=abs(vec.x);
	vec.y=abs(vec.y);
	vec.z=abs(vec.z);
	if(vec.x>=vec.y&&vec.x>=vec.z)
		return vec.x;
	if(vec.y>=vec.x&&vec.y>=vec.z)
		return vec.y;
	if(vec.z>=vec.x&&vec.z>=vec.y)
		return vec.z;
	return 0;
}
int LayerPriorIndex(const char* layer)
{
	int index=0;
	if(toupper(layer[0])=='S'||toupper(layer[0])=='L')
		index=100;
	if(layer[1]=='Z')
		index+=20;	//塔身杆件优先于头部杆件
	else if(layer[1]=='H'||layer[1]=='X')
		index+=10;	//前后左右面杆件优先于隔面杆件
	else if(isdigit(layer[2])!=0)
		index+=2;
	else if(layer[2]!='G')
		index+=1;
	return index;
}
int CompareLayerPriorIndex(const char* layer1,const char* layer2)
{
	return LayerPriorIndex(layer1)-LayerPriorIndex(layer2);
}
bool CUnifyModelLayer::TestRodPosition(CLDSLinePart* pRod,int* piPosTypeToTrunk)
{
	if(this->listSlopes.GetNodeNum()==0||pRod->pStart==NULL||pRod->pEnd==NULL)
		return false;
	SLOPE* pSlope=NULL;
	for(pSlope=listSlopes.GetFirst();pSlope;pSlope=listSlopes.GetNext())
	{
		bool blTip4=pSlope->bottom.z>=pRod->pEnd->xOriginalPos.z-EPS2;
		if( pSlope->top.z<=pRod->pStart->xOriginalPos.z+EPS2&&pSlope->bottom.z>=pRod->pStart->xOriginalPos.z-EPS2&&
			pSlope->top.z<=pRod->pEnd->xOriginalPos.z+EPS2&&pSlope->bottom.z>=pRod->pEnd->xOriginalPos.z-EPS2)
			break;
	}
	if(pSlope==NULL)
	{
		if(piPosTypeToTrunk)
			*piPosTypeToTrunk=-1;
		return false;
	}
	GEPOINT lenStdPostVec=pSlope->bottom-pSlope->top;
	if(lenStdPostVec.z<0)
		lenStdPostVec=pSlope->top-pSlope->bottom;
	normalize(lenStdPostVec);
	GEPOINT vFrontPanelNormal(0,lenStdPostVec.z,-lenStdPostVec.y);
	GEPOINT vRightPanelNormal(lenStdPostVec.z,0,-lenStdPostVec.y);
	normalize(vFrontPanelNormal);
	normalize(vRightPanelNormal);
	int iPosToTrunkType=0;
	if( pSlope->taper.IsIncludePoint(pRod->pStart->xOriginalPos,1)&&
		pSlope->taper.IsIncludePoint(pRod->pEnd->xOriginalPos,1))
		iPosToTrunkType=0;	//杆件在塔身坡面或塔身内部
	else
	{
		//判断杆件是否在右侧
		GEPOINT xPanelOrg=pSlope->top;
		GEPOINT xPanelNormal=vRightPanelNormal;
		double dfTipStartZ=(pRod->pStart->xOriginalPos-xPanelOrg)*xPanelNormal;
		double dfTipEndZ  =(pRod->pEnd->xOriginalPos-xPanelOrg)*xPanelNormal;
		const double MINSPACE_TOLERANCE=50;
		if(dfTipStartZ>-EPS2&&dfTipEndZ>-EPS2&&dfTipStartZ+dfTipEndZ>MINSPACE_TOLERANCE)
			iPosToTrunkType='Y';
		if(iPosToTrunkType==0)
		{	//判断杆件是否在左侧
			xPanelOrg.Set(-pSlope->top.x,pSlope->top.y,pSlope->top.z);
			xPanelNormal.Set(-vRightPanelNormal.x,0,vFrontPanelNormal.z);
			dfTipStartZ=(pRod->pStart->xOriginalPos-xPanelOrg)*xPanelNormal;
			dfTipEndZ  =(pRod->pEnd->xOriginalPos-xPanelOrg)*xPanelNormal;
			if(dfTipStartZ>-EPS2&&dfTipEndZ>-EPS2&&dfTipStartZ+dfTipEndZ>MINSPACE_TOLERANCE)
				iPosToTrunkType='L';
		}
		if(iPosToTrunkType==0)
		{	//判断杆件是否在前面
			xPanelOrg=pSlope->top;
			xPanelNormal=vFrontPanelNormal;
			dfTipStartZ=(pRod->pStart->xOriginalPos-xPanelOrg)*xPanelNormal;
			dfTipEndZ  =(pRod->pEnd->xOriginalPos-xPanelOrg)*xPanelNormal;
			if(dfTipStartZ>-EPS2&&dfTipEndZ>-EPS2&&dfTipStartZ+dfTipEndZ>MINSPACE_TOLERANCE)
				iPosToTrunkType='Q';
		}
		if(iPosToTrunkType==0)
		{	//判断杆件是否在后面
			xPanelOrg.Set(pSlope->top.x,-pSlope->top.y,pSlope->top.z);
			xPanelNormal.Set(0,-vFrontPanelNormal.y,vFrontPanelNormal.z);
			dfTipStartZ=(pRod->pStart->xOriginalPos-xPanelOrg)*xPanelNormal;
			dfTipEndZ  =(pRod->pEnd->xOriginalPos-xPanelOrg)*xPanelNormal;
			if(dfTipStartZ>-EPS2&&dfTipEndZ>-EPS2&&dfTipStartZ+dfTipEndZ>MINSPACE_TOLERANCE)
				iPosToTrunkType='H';
		}
	}
	if(piPosTypeToTrunk)
		*piPosTypeToTrunk=iPosToTrunkType;
	return true;
}
static bool VerifyIterRod(CLDSLinePart* pRod){return pRod->feature==0;}
void CUnifyModelLayer::CorrectColinearJoints(COLINEAR_TOLERANCE* pTolerance/*=NULL*/,int liInitProcessPercent/*=0*/)
{
	COLINEAR_TOLERANCE default_tolerance;
	if(pTolerance==NULL)
		pTolerance=&default_tolerance;
	int liLeastPercent=100-liInitProcessPercent;
	CSuperSmartPtr<CLDSLinePart> pRod;
	for(pRod=Ta.EnumRodFirst();pRod.IsHasPtr();pRod=Ta.EnumRodNext())
	{
		pRod->UnifiedCfgword();
		pRod->feature=0;
	}
	DisplayProcess(liInitProcessPercent,"处理共线杆件位置");
	int i=0,n=0;
	for(pRod=Ta.EnumRodFirst();pRod.IsHasPtr();pRod=Ta.EnumRodNext())
		n++;
	for(pRod=Ta.EnumRodFirst();pRod.IsHasPtr();pRod=Ta.EnumRodNext(),i++)
	{
		DisplayProcess(liInitProcessPercent+i*liLeastPercent/n,"处理共线杆件位置");
		if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->feature==1)
			continue;
		pRod->feature=1;
		LINEPARTSET rodset;
		bool bStartIsStartNode,bEndIsStartNode;
		Ta.SearchSortedColinearRods(pRod,rodset,&bStartIsStartNode,&bEndIsStartNode,pTolerance->distance,pTolerance->tangent,VerifyIterRod);	//一般为共线点
		if(rodset.GetNodeNum()<=1)
			continue;
		CLDSLinePart* pStartRod=rodset.GetFirst();
		CLDSLinePart* pEndRod=rodset.GetTail();
		CLDSNode* pStartTipNode=pStartRod->pStart;
		CLDSNode* pEndTipNode=pEndRod->pEnd;
		if(!bStartIsStartNode)
			pStartTipNode=pStartRod->pEnd;
		CLDSNode *pCurrNode=bStartIsStartNode?pStartRod->pEnd:pStartRod->pStart;
		if(bEndIsStartNode)
			pEndTipNode=pEndRod->pStart;
		SmartRodPtr pPrevRod=rodset.GetFirst(),pCurrRod;
		pPrevRod->feature=1;
		bool blFinalSegment = false;	//是否为共线杆件集合中最后一段首尾相连的共线杆件分段
		do {
			blFinalSegment = false;
			pPrevRod = pStartRod;
			//////////////////////////////////////////////////
			{//检测分段共线情况（共线杆件集合被分类若干段，互相不连续）
				BOOL pushed=rodset.push_stack();
				CLDSNode* pJointNode=pCurrNode;		//此时pCurrNode为当前共线分段的首个首尾衔接端节点
				CLDSLinePart* pSegPrevRod=pPrevRod;	//此时pPrevRod为当前共线分段的首根杆件
				int iPrevTrunkPosType=0,nSegRodCount=1;
				TestRodPosition(pSegPrevRod,&iPrevTrunkPosType);
				for (pCurrRod = rodset.GetNext(); pCurrRod.IsHasPtr(); pCurrRod = rodset.GetNext())
				{
					if(pJointNode!=pCurrRod->pStart&&pJointNode!=pCurrRod->pEnd)
					{	//此时pCurrRod已与之前共线杆件不能首尾衔接，前一杆件即为当前共线分段的末尾杆件
						pEndRod = pSegPrevRod;	//连接中断
						//blFinalSegment = false;	//因还有共线杆件未处理完，故当前提取出的共线分段非最后的分段
						break;
					}
					int iTrunkPosType=0;
					TestRodPosition(pCurrRod,&iTrunkPosType);
					if(iTrunkPosType!=iPrevTrunkPosType)
					{
						pEndRod = pSegPrevRod;	//连接中断
						break;	//类型不同可能表示连续杆件穿过了塔身，必须在塔身处断开
					}
					iPrevTrunkPosType=iTrunkPosType;
					nSegRodCount++;
					if(pJointNode == pCurrRod->pStart)
					{
						bEndIsStartNode = false;
						pJointNode=pCurrRod->pEnd;
					}
					else //if(pJointNode == pCurrRod->pEnd)
					{
						bEndIsStartNode = true;
						pJointNode=pCurrRod->pStart;
					}
					pSegPrevRod = pCurrRod;
				}
				if (blFinalSegment = pCurrRod.IsNULL())
					pEndRod = pSegPrevRod;
				rodset.pop_stack(pushed);
				if(nSegRodCount==1)
				{	//共线段中的孤立杆件分段
					if(pCurrRod.blValid&&pCurrRod->pStart==pJointNode)
					{
						pJointNode=pCurrRod->pEnd;
						bStartIsStartNode=true;
					}
					else if(pCurrRod.blValid&&pCurrRod->pEnd==pJointNode)
					{
						pJointNode=pCurrRod->pStart;
						bStartIsStartNode=false;
					}
				}
			}
			//////////////////////////////////////////////////
			pCurrNode=bStartIsStartNode?pStartRod->pEnd:pStartRod->pStart;//NULL;
			int iPrevTrunkPosType=0;
			TestRodPosition(pPrevRod,&iPrevTrunkPosType);
			for(pCurrRod=rodset.GetNext();pCurrRod.IsHasPtr();pCurrRod=rodset.GetNext())
			{
				pCurrRod->feature=1;
				if(pCurrNode!=pCurrRod->pStart&&pCurrNode!=pCurrRod->pEnd)
					break;	//连接中断
				int iTrunkPosType=0;
				TestRodPosition(pCurrRod,&iTrunkPosType);
				if(iTrunkPosType!=iPrevTrunkPosType)
					break;	//类型不同可能表示连续杆件穿过了塔身，必须在塔身处断开
				iPrevTrunkPosType=iTrunkPosType;
				//共线杆件间的中间端节点父杆件应该是其中一根（交叉点除外)
				if( pCurrNode->m_cPosCalType!=CLDSNode::COORD_INTERSECTION&&
					pCurrNode->hFatherPart!=pPrevRod->handle&&pCurrNode->hFatherPart!=pCurrRod->handle)
				{	//端节点父杆件已是共线杆件中的一根，不需要额外调整
					if(pPrevRod->GetWidth()<pCurrRod->GetWidth())
						pCurrNode->hFatherPart=pCurrRod->handle;
					else
						pCurrNode->hFatherPart=pPrevRod->handle;
					this->SetNodeRevision(pCurrNode->handle,REVISION::PARENT_ROD);
				}
				if(pCurrNode->hFatherPart==pCurrRod->handle)
				{
					if(pCurrRod->pStart==pCurrNode)
					{
						if(pCurrRod->IsAngle())
						{
							pCurrRod.pAngle->desStartPos.jgber_cal_style=0;
							if(pCurrRod.pAngle->desStartPos.spatialOperationStyle!=16&&pCurrRod.pAngle->desStartPos.spatialOperationStyle!=17)
								pCurrRod.pAngle->desStartPos.spatialOperationStyle=16;
							pCurrRod.pAngle->desStartPos.hDatumLine=0;
							pCurrRod.pAngle->desStartPos.datumLineStart.datum_pos_style=1;
							pCurrRod.pAngle->desStartPos.datumLineStart.des_para.RODEND.hRod=pStartRod->handle;
							pCurrRod.pAngle->desStartPos.datumLineStart.des_para.RODEND.direction=bStartIsStartNode?0:1;
							pCurrRod.pAngle->desStartPos.datumLineStart.des_para.RODEND.SetWingOffset(0,4);
							pCurrRod.pAngle->desStartPos.datumLineEnd.datum_pos_style=1;
							pCurrRod.pAngle->desStartPos.datumLineEnd.des_para.RODEND.hRod=pEndRod->handle;
							pCurrRod.pAngle->desStartPos.datumLineEnd.des_para.RODEND.direction=bEndIsStartNode?0:1;
							pCurrRod.pAngle->desStartPos.datumLineEnd.des_para.RODEND.SetWingOffset(0,4);
							this->SetRodRevision(pCurrRod->handle,REVISION::POS_START);
						}
						else if(pCurrRod->IsTube())
						{
							pCurrRod.pTube->desStartPos.endPosType=2;
							pCurrRod.pTube->desStartPos.hDatumStartPart=pStartRod->handle;
							pCurrRod.pTube->desStartPos.bDatumStartStartEnd=bStartIsStartNode;
							pCurrRod.pTube->desStartPos.hDatumEndPart=pEndRod->handle;
							pCurrRod.pTube->desStartPos.bDatumEndStartEnd=bEndIsStartNode;
							this->SetRodRevision(pCurrRod->handle,REVISION::POS_START);
						}
					}
					else
					{
						if(pCurrRod->IsAngle())
						{
							pCurrRod.pAngle->desEndPos.jgber_cal_style=0;
							if(pCurrRod.pAngle->desEndPos.spatialOperationStyle!=16&&pCurrRod.pAngle->desEndPos.spatialOperationStyle!=17)
								pCurrRod.pAngle->desEndPos.spatialOperationStyle=16;
							pCurrRod.pAngle->desEndPos.hDatumLine=0;
							pCurrRod.pAngle->desEndPos.datumLineStart.datum_pos_style=1;
							pCurrRod.pAngle->desEndPos.datumLineStart.des_para.RODEND.hRod=pStartRod->handle;
							pCurrRod.pAngle->desEndPos.datumLineStart.des_para.RODEND.direction=bStartIsStartNode?0:1;
							pCurrRod.pAngle->desEndPos.datumLineStart.des_para.RODEND.SetWingOffset(0,4);
							pCurrRod.pAngle->desEndPos.datumLineEnd.datum_pos_style=1;
							pCurrRod.pAngle->desEndPos.datumLineEnd.des_para.RODEND.hRod=pEndRod->handle;
							pCurrRod.pAngle->desEndPos.datumLineEnd.des_para.RODEND.direction=bEndIsStartNode?0:1;
							pCurrRod.pAngle->desEndPos.datumLineEnd.des_para.RODEND.SetWingOffset(0,4);
							this->SetRodRevision(pCurrRod->handle,REVISION::POS_END);
						}
						else if(pCurrRod->IsTube())
						{
							pCurrRod.pTube->desEndPos.endPosType=2;
							pCurrRod.pTube->desEndPos.hDatumStartPart=pStartRod->handle;
							pCurrRod.pTube->desEndPos.bDatumStartStartEnd=bStartIsStartNode;
							pCurrRod.pTube->desEndPos.hDatumEndPart=pEndRod->handle;
							pCurrRod.pTube->desEndPos.bDatumEndStartEnd=bEndIsStartNode;
							this->SetRodRevision(pCurrRod->handle,REVISION::POS_END);
						}
					}
					if(pPrevRod->pStart==pCurrNode)
					{
						if(pPrevRod->IsAngle())
						{
							pPrevRod.pAngle->desStartPos.spatialOperationStyle=1;
							pPrevRod.pAngle->desStartPos.wing_x_offset.SetOffset(0,4);
							pPrevRod.pAngle->desStartPos.wing_y_offset.SetOffset(0,4);
							this->SetRodRevision(pPrevRod->handle,REVISION::POS_START);
						}
						else if(pPrevRod->IsTube())
						{
							pPrevRod.pTube->desStartPos.endPosType=1;
							pPrevRod.pTube->desStartPos.datum_pos_style=0;
							this->SetRodRevision(pPrevRod->handle,REVISION::POS_START);
						}
					}
					else
					{
						if(pPrevRod->IsAngle())
						{
							pPrevRod.pAngle->desEndPos.spatialOperationStyle=1;
							pPrevRod.pAngle->desEndPos.wing_x_offset.SetOffset(0,4);
							pPrevRod.pAngle->desEndPos.wing_y_offset.SetOffset(0,4);
							this->SetRodRevision(pPrevRod->handle,REVISION::POS_END);
						}
						else if(pPrevRod->IsTube())
						{
							pPrevRod.pTube->desEndPos.endPosType=1;
							pPrevRod.pTube->desEndPos.datum_pos_style=0;
							this->SetRodRevision(pPrevRod->handle,REVISION::POS_END);
						}
					}
				}
				else //if(pPrevRod->GetWidth()>pCurrRod->GetWidth())
				{
					if(pPrevRod->pStart==pCurrNode)
					{
						if(pPrevRod->IsAngle())
						{
							pPrevRod.pAngle->desStartPos.jgber_cal_style=0;
							if(pPrevRod.pAngle->desStartPos.spatialOperationStyle!=16&&pPrevRod.pAngle->desStartPos.spatialOperationStyle!=17)
								pPrevRod.pAngle->desStartPos.spatialOperationStyle=16;
							pPrevRod.pAngle->desStartPos.hDatumLine=0;
							pPrevRod.pAngle->desStartPos.datumLineStart.datum_pos_style=1;
							pPrevRod.pAngle->desStartPos.datumLineStart.des_para.RODEND.hRod=pStartRod->handle;
							pPrevRod.pAngle->desStartPos.datumLineStart.des_para.RODEND.direction=bStartIsStartNode?0:1;
							pPrevRod.pAngle->desStartPos.datumLineStart.des_para.RODEND.SetWingOffset(0,4);
							pPrevRod.pAngle->desStartPos.datumLineEnd.datum_pos_style=1;
							pPrevRod.pAngle->desStartPos.datumLineEnd.des_para.RODEND.hRod=pEndRod->handle;
							pPrevRod.pAngle->desStartPos.datumLineEnd.des_para.RODEND.direction=bEndIsStartNode?0:1;
							pPrevRod.pAngle->desStartPos.datumLineEnd.des_para.RODEND.SetWingOffset(0,4);
							this->SetRodRevision(pPrevRod->handle,REVISION::POS_START);
						}
						else if(pPrevRod->IsTube())
						{
							pPrevRod.pTube->desStartPos.endPosType=2;
							pPrevRod.pTube->desStartPos.hDatumStartPart=pStartRod->handle;
							pPrevRod.pTube->desStartPos.bDatumStartStartEnd=bStartIsStartNode;
							pPrevRod.pTube->desStartPos.hDatumEndPart=pEndRod->handle;
							pPrevRod.pTube->desStartPos.bDatumEndStartEnd=bEndIsStartNode;
							this->SetRodRevision(pPrevRod->handle,REVISION::POS_START);
						}
					}
					else
					{
						if(pPrevRod->IsAngle())
						{
							pPrevRod.pAngle->desEndPos.jgber_cal_style=0;
							if(pPrevRod.pAngle->desEndPos.spatialOperationStyle!=16&&pPrevRod.pAngle->desEndPos.spatialOperationStyle!=17)
								pPrevRod.pAngle->desEndPos.spatialOperationStyle=16;
							pPrevRod.pAngle->desEndPos.hDatumLine=0;
							pPrevRod.pAngle->desEndPos.datumLineStart.datum_pos_style=1;
							pPrevRod.pAngle->desEndPos.datumLineStart.des_para.RODEND.hRod=pStartRod->handle;
							pPrevRod.pAngle->desEndPos.datumLineStart.des_para.RODEND.direction=bStartIsStartNode?0:1;
							pPrevRod.pAngle->desEndPos.datumLineStart.des_para.RODEND.SetWingOffset(0,4);
							pPrevRod.pAngle->desEndPos.datumLineEnd.datum_pos_style=1;
							pPrevRod.pAngle->desEndPos.datumLineEnd.des_para.RODEND.hRod=pEndRod->handle;
							pPrevRod.pAngle->desEndPos.datumLineEnd.des_para.RODEND.direction=bEndIsStartNode?0:1;
							pPrevRod.pAngle->desEndPos.datumLineEnd.des_para.RODEND.SetWingOffset(0,4);
							this->SetRodRevision(pPrevRod->handle,REVISION::POS_END);
						}
						else if(pPrevRod->IsTube())
						{
							pPrevRod.pTube->desEndPos.endPosType=2;
							pPrevRod.pTube->desEndPos.hDatumStartPart=pStartRod->handle;
							pPrevRod.pTube->desEndPos.bDatumStartStartEnd=bStartIsStartNode;
							pPrevRod.pTube->desEndPos.hDatumEndPart=pEndRod->handle;
							pPrevRod.pTube->desEndPos.bDatumEndStartEnd=bEndIsStartNode;
							this->SetRodRevision(pPrevRod->handle,REVISION::POS_END);
						}
					}
					if(pCurrRod->pStart==pCurrNode)
					{
						if(pCurrRod->IsAngle())
						{
							pCurrRod.pAngle->desStartPos.spatialOperationStyle=1;
							pCurrRod.pAngle->desStartPos.wing_x_offset.SetOffset(0,4);
							pCurrRod.pAngle->desStartPos.wing_y_offset.SetOffset(0,4);
						}
						else if(pCurrRod->IsTube())
						{
							pCurrRod.pTube->desStartPos.endPosType=1;
							pCurrRod.pTube->desStartPos.datum_pos_style=0;
						}
						this->SetRodRevision(pCurrRod->handle,REVISION::POS_START);
					}
					else
					{
						if(pCurrRod->IsAngle())
						{
							pCurrRod.pAngle->desEndPos.spatialOperationStyle=1;
							pCurrRod.pAngle->desEndPos.wing_x_offset.SetOffset(0,4);
							pCurrRod.pAngle->desEndPos.wing_y_offset.SetOffset(0,4);
						}
						else if(pCurrRod->IsTube())
						{
							pCurrRod.pTube->desEndPos.endPosType=1;
							pCurrRod.pTube->desEndPos.datum_pos_style=0;
						}
						this->SetRodRevision(pCurrRod->handle,REVISION::POS_END);
					}
				}
				//更新下一个共线段首尾衔接端点
				if(pCurrNode==pCurrRod->pStart)
					pCurrNode=pCurrRod->pEnd;
				else if(pCurrNode==pCurrRod->pEnd)
					pCurrNode=pCurrRod->pStart;
				pPrevRod=pCurrRod;
			}
			//查找下一共线分段杆件集合（至少2根及2根以上杆件）
			while (pCurrRod.blValid)
			{	//根据下一段前两根杆件判断下一共线段的起始端点
				pStartRod = pPrevRod = pCurrRod;
				BOOL pushed = rodset.push_stack();
				pCurrRod = rodset.GetNext();
				rodset.pop_stack(pushed);
				if (pCurrRod.blValid)
				{
					if (pCurrRod->pStart == pPrevRod->pStart || pCurrRod->pEnd == pPrevRod->pStart)
					{
						bStartIsStartNode = false;
						pCurrNode=pPrevRod->pStart;
						break;
					}
					else if (pCurrRod->pStart == pPrevRod->pEnd || pCurrRod->pEnd == pPrevRod->pEnd)
					{
						bStartIsStartNode = true;
						pCurrNode=pPrevRod->pEnd;
						break;
					}
					else	//共线杆件集合中的单根独立分段杆件，应跳过不处理
						pCurrRod=rodset.GetNext();
				}
				else
					blFinalSegment = true;
			}
		}while (!blFinalSegment&&pCurrRod.blValid);
	}
	DisplayProcess(100,"处理共线杆件位置");
}
//CorrectNodeFatherRodInfo()函数是从自CommandFileProcess.cpp中原装移植过来的 wjh-2015.12.10
int CorrectNodeFatherRodInfo(NODESET* pNodeSet=NULL,COLINEAR_TOLERANCE* pTolerance=NULL,bool bCorrectColinear=true)
{
	CUnifyModelLayer unify;
	return unify.CorrectNodeFatherRodInfo(pNodeSet,pTolerance,bCorrectColinear);
}
int CUnifyModelLayer::CorrectNodeFatherRodInfo(NODESET* pNodeSet/*=NULL*/,COLINEAR_TOLERANCE* pTolerance/*=NULL*/,bool bCorrectColinear/*=true*/)
{	//修正节点父杆件配材号
	COLINEAR_TOLERANCE default_tolerance;
	if(pTolerance==NULL)
		pTolerance=&default_tolerance;
	CLogErrorLife life;
	CHashList<CPtInLineCheck> lineChecks;
	CSuperSmartPtr<CLDSLinePart> pRod;
	for(pRod=Ta.EnumRodFirst();pRod.IsHasPtr();pRod=Ta.EnumRodNext())
		pRod->UnifiedCfgword();
	CLDSNode* pNode;
	NODESET nodes;
	if(pNodeSet==NULL)
	{
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			nodes.append(pNode);
		pNodeSet=&nodes;
	}
	if(bCorrectColinear)
		DisplayProcess(0,"处理共线杆件位置");
	else
		DisplayProcess(0,"补充节点依赖杆件信息");

	int i=0,n=pNodeSet->Count;
	for(pNode=pNodeSet->GetFirst();pNode;pNode=pNodeSet->GetNext(),i++)
	{
		BITFLAG* pRevFlag=this->hashObjRevision.GetValue(pNode->handle);
		if (pRevFlag!=NULL&&pRevFlag->IsHasFlag(REVISION::PARENT_ROD))
			continue;	//之前已设定过该节点父杆件
		CPtInLineCheck* pCheck=NULL;
		pNode->UnifiedCfgword();	//补全各接腿号占位
		if(bCorrectColinear)
			DisplayProcess(i*10/n,"处理共线杆件位置");
		else
			DisplayProcess(i*100/n,"补充节点依赖杆件信息");
		if(pNode->m_cPosCalType==5&&pNode->Layer(0)=='S'&&(pNode->Position().x==0||pNode->Position().y==0))
		{	//塔身横材上的比例等分节点，焊接父杆件优先考虑横材
			CLDSNode* pDependNode1=Ta.FromNodeHandle(pNode->arrRelationNode[0]);
			CLDSNode* pDependNode2=Ta.FromNodeHandle(pNode->arrRelationNode[1]);
			if(pRod.IsHasPtr()&&pRod->pStart&&pRod->pEnd&&((pRod->pStart==pDependNode1&&pRod->pEnd==pDependNode2)||
				(pRod->pStart==pDependNode2&&pRod->pEnd==pDependNode1)))
				continue;
			pRod=Ta.CheckOverLappedLinePart(pDependNode1->Position(),pDependNode2->Position(),pNode);
			if(pRod.IsHasPtr())
			{
				pNode->hFatherPart=pRod->handle;
				continue;
			}
		}
		else if(pRod.IsHasPtr()&&pRod->pStart&&pRod->pEnd&&pNode->cfgword.IsEqual(pRod->UnifiedCfgword()))
		{	//节点配材号与父杆件配材号一致时，应进一步判断节点是否为父杆件的内节点
			if((pCheck=lineChecks.GetValue(pRod->handle))==NULL)
			{
				pCheck=lineChecks.Add(pRod->handle);
				pCheck->SetLine(pRod->pStart->Position(false),pRod->pEnd->Position(false));
			}
			double rslt=pCheck->CheckPoint(pNode->Position(false),pTolerance->distance,pTolerance->tangent);
			if(rslt>0.0&&rslt<1.0)
				continue;	//节点与其父杆件配材号一致，且为父件的内点，不需要进一步智能修正
			else if((rslt==0.0||rslt==1.0)&&pRod->Layer(0)!='T'&&pRod->Layer(1)=='Z')
				continue;	//节点为塔身（腿）部主材时，尽量不要再次调整节点的父杆件，否则可能导致在节点上下关联父杆件间反复调换 wjh-2015.12.12
		}
		//进一步智能修正更为合理的节点父杆件
		CSuperSmartPtr<CLDSLinePart> pParentRod=pRod=Ta.FromRodHandle(pNode->hFatherPart);
		double perfectRodSimilarity=0.0;
		CLDSLinePart* pPerfectParentRod=NULL;
		for(pRod=Ta.EnumRodFirst();pRod.IsHasPtr();pRod=Ta.EnumRodNext())
		{
			if(pRod->pStart==NULL||pRod->pEnd==NULL)
				continue;
			if(pRod->GetClassTypeId()==CLS_LINEANGLE&&pRod.LineAnglePointer()->group_father_jg_h>0x20)
				continue;	//一般不以组合角钢子角钢作为节点的父杆件
			if(!pNode->cfgword.IsEqual(pRod->UnifiedCfgword()))
				continue;	//配材号一致
			if((pCheck=lineChecks.GetValue(pRod->handle))==NULL)
			{
				pCheck=lineChecks.Add(pRod->handle);
				pCheck->SetLine(pRod->pStart->Position(false),pRod->pEnd->Position(false));
			}
			double rslt=pCheck->CheckPoint(pNode->Position(false),pTolerance->distance,pTolerance->tangent);
			if(rslt>0.0&&rslt<1.0)
				break;
			if(pRod->pStart!=pNode&&pRod->pEnd!=pNode)
				continue;
			GEPOINT vRayVec=pRod->pEnd->xOriginalPos-pRod->pStart->xOriginalPos;
			double similarity=CalRodOrthoFactor(vRayVec.x,vRayVec.z);	//SimilarRatioToAxisXYZ(pRod->End()-pRod->Start())
			if(pPerfectParentRod==NULL)
			{
				pPerfectParentRod=pRod;
				perfectRodSimilarity=similarity;
				continue;
			}
			int layer_priority=CompareLayerPriorIndex(pPerfectParentRod->layer(),pRod->layer());
			if(layer_priority>0)
				continue;	//当前杆件图层优先级更低跳过
			else if(layer_priority<0)
				pPerfectParentRod=pRod;
			else if(similarity-perfectRodSimilarity>0.01)
				pPerfectParentRod=pRod;
			else if(fabs(similarity-perfectRodSimilarity)<0.01&&pPerfectParentRod!=NULL&&pRod->GetWidth()>pPerfectParentRod->GetWidth())
				pPerfectParentRod=pRod;
			else
				continue;
			perfectRodSimilarity=similarity;
		}
		if(pRod.IsHasPtr())
			pNode->hFatherPart=pRod->handle;
		else if(pPerfectParentRod!=NULL)
			pNode->hFatherPart=pPerfectParentRod->handle;
		else if(!pNode->cfgword.IsNull()&&(pParentRod.IsNULL()||!pNode->cfgword.IsEqual(pParentRod->UnifiedCfgword())))
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"Can't find 0x%X node's reasonable parent rod ,its parent rod remains unchanged!",pNode->handle);
#else	//cfgword为空时，视为虚拟辅助节点，能否找到合理父杆件不重要 wjh-2015.12.12
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"未找到0x%X节点的合理父杆件,节点父杆件保持不变!",pNode->handle);
#endif
	}
	//需要考虑两段杆件共线时中间连接节点父杆件应是其中一根情况
	if(bCorrectColinear)
		CorrectColinearJoints(pTolerance,10);
	DisplayProcess(100,"处理共线杆件位置");
	return 0;
}
#ifdef __RAPID_ROUGH_3DMODEL_
#include "DesignMultiLeg.h"
int CLDSView::CorrectNodeFatherRodCfgWord()	//修正节点父杆件配材号
{
	return CorrectNodeFatherRodInfo();
}
class ROD_MEMBERS{
public:
	char cMaterial;
	GEPOINT zeroPoint;	//杆件长度延伸方向的零基准点
	GEPOINT lenStdVec;	//始->终单位化杆单元延伸方向
	SIZESPEC size;
	LINEPARTSET members;
	CFGWORD cfgword;
public:
	ROD_MEMBERS(){cMaterial='S';}
	~ROD_MEMBERS(){members.Empty();}
	void InitRodMembers(CLDSLinePart* pRodMember)
	{
		cMaterial=pRodMember->cMaterial;
		size=pRodMember->GetSizeSpec();
		zeroPoint=(pRodMember->pStart->Position(false)+pRodMember->pEnd->Position(false))*0.5;
		lenStdVec=pRodMember->pEnd->Position(false)-f3dPoint(zeroPoint);
		normalize(lenStdVec);
		cfgword=pRodMember->cfgword;
		if(pRodMember->IsBodyObj())
			cfgword=cfgword.ToBodyWord();
		members.append(pRodMember);
	}
	//返回pickCoords点是否在杆件线上
	bool IsColinear(const double* pickCoords,double tolerance=0.01,double tolerance_tangent=1)
	{
		GEPOINT verify_vec=GEPOINT(pickCoords)-zeroPoint;
		GEPOINT prj_vec=(verify_vec*lenStdVec)*lenStdVec;
		GEPOINT vert_vec=verify_vec-prj_vec;
		double tolerance_square=tolerance*tolerance;
		double tangent_square=tolerance_tangent*tolerance_tangent;
		//绝对距离误差
		bool bColinear1 = vert_vec.mod2()<=tolerance_square;//绝对距离误差
		double diff2=vert_vec.mod2()/prj_vec.mod2();
		bool bColinear2 = diff2<=tangent_square;			//相对正切误差
		return bColinear1&&bColinear2;
	}
};
struct DISCRETE_ELEM
{
	CLDSLinePart *pRod;	//受力杆件
	GEPOINT lenStdVec;	//始->终单位化杆单元延伸方向
	bool reverseStart2End;
	double fDistance;	//受力单元到基准点距离
	DISCRETE_ELEM(){pRod=NULL;fDistance=0;reverseStart2End=false;}
	DISCRETE_ELEM(CLDSLinePart *pPart,ROD_MEMBERS* pCombineRod)
	{
		pRod=pPart;
		reverseStart2End=false;
		if(pPart->pStart==NULL||pPart->pEnd==NULL)
			return;
		GEPOINT middlePt=(pPart->pStart->Position(false)+pPart->pEnd->Position(false))*0.5;
		GEPOINT verfiy_vec=middlePt-pCombineRod->zeroPoint;
		fDistance=verfiy_vec.mod();
		if(verfiy_vec*pCombineRod->lenStdVec<0)
			fDistance*=-1.0;
		lenStdVec=pPart->pEnd->Position(false)-pPart->pStart->Position(false);
		normalize(lenStdVec);
		if(lenStdVec*pCombineRod->lenStdVec<0)
			reverseStart2End=true;
		else
			reverseStart2End=false;
	}
};

int CompareDistanceFunc(const DISCRETE_ELEM& item1,const DISCRETE_ELEM& item2)
{
	if(item1.fDistance>item2.fDistance)
		return 1;
	else if(item1.fDistance<item2.fDistance)
		return -1;
	else
		return 0;
}
BOOL FinishComposLinePart(LINEPARTSET &partset,CLDSNode *pStart,CLDSNode *pEnd,BOOL bMechanicalPersist=FALSE,BOOL bFullMode=TRUE);
void CombineDiscreteRodMembers(BOOL bMechanicalPersist=FALSE,BOOL bCorrectNodeFatherRodInfo=TRUE,COLINEAR_TOLERANCE* pTolerance=NULL,CUnifyModelLayer::SLOPEPTR* ppVerifySlopes=NULL,int nSlopeCount=0)
{
	ROD_MEMBERS *pCombineRod=NULL;
	ATOM_LIST<ROD_MEMBERS> listRods;
	CUndoOperObject undo(&Ta);
	DisplayProcess(0,"合并离散杆件单元");
	CLogErrorLife loglife;
	int i=0,n=Ta.Parts.GetNodeNum();
	COLINEAR_TOLERANCE tolerance;
	if(pTolerance==NULL)
		pTolerance=&tolerance;
	//1.判断统材号是否相同、杆件是否共线
	CLDSLinePart* pRod;
	for(pRod = Ta.EnumRodFirst();pRod; pRod=Ta.EnumRodNext())
		pRod->UnifiedCfgword();
	for(pRod = Ta.EnumRodFirst();pRod; pRod=Ta.EnumRodNext(),i++)
	{
		char cPartMaterial = pRod->cMaterial;
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		if(pRod->blGroupSonAngle)//GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pRod)->group_father_jg_h>0x20)
			continue;	//跳过组合角钢子角钢
		SIZESPEC size;
		for(pCombineRod=listRods.GetFirst();pCombineRod;pCombineRod=listRods.GetNext())
		{
			if( cPartMaterial==pCombineRod->cMaterial&&
				pCombineRod->cfgword.IsEqual(pRod->cfgword)&&pCombineRod->size.IsEqual(pRod->GetSizeSpec())&&
				pCombineRod->IsColinear(pRod->pStart->xPreliftPos,pTolerance->distance,pTolerance->tangent)&&
				pCombineRod->IsColinear(pRod->pEnd->xPreliftPos  ,pTolerance->distance,pTolerance->tangent))
				break;
		}
		if(pCombineRod==NULL)
		{
			pCombineRod=listRods.append();
			pCombineRod->InitRodMembers(pRod);
		}
		else
			pCombineRod->members.append(pRod);
		DisplayProcess(i*10/100,"合并离散杆件单元");
	}
	//2.把共线的受力杆件按照有序放到集合中
	ATOM_LIST<PARTSET> needMergePartsList;
	ARRAY_LIST<DISCRETE_ELEM> elemArr;
	DISCRETE_ELEM *pElem;
	ARRAY_LIST<ROD_MEMBERS*> preMergeMembers(0,500);
	for(pCombineRod=listRods.GetFirst();pCombineRod;pCombineRod=listRods.GetNext())
	{	//2.1 把不需要合并的放到一个partSet中
		if(pCombineRod->members.GetNodeNum()<=1)
			continue;
		preMergeMembers.append(pCombineRod);
	}
	i=0;
	n=preMergeMembers.Size();
	for(i=0;i<n;i++)
	{
		pCombineRod=preMergeMembers[i];
		//2.2 把要合并的集合排序
		elemArr.SetSize(0,pCombineRod->members.GetNodeNum());
		for(CLDSLinePart *pMemberRod = (CLDSLinePart *)pCombineRod->members.GetFirst();pMemberRod;pMemberRod=(CLDSLinePart *)pCombineRod->members.GetNext())
			elemArr.append(DISCRETE_ELEM(pMemberRod,pCombineRod));
		CQuickSort<DISCRETE_ELEM>::QuickSort(elemArr.m_pData,elemArr.GetSize(),CompareDistanceFunc);
		//2.3 把排好的放到集合中
		bool finished=false;
		DISCRETE_ELEM *pHeadElem=NULL,*pTailElem=elemArr.GetTail();
		DISCRETE_ELEM *pPrevElem=elemArr.GetFirst();
		int tranverse_hits=0;	//穿越塔身主柱分界线次数（第二次穿越后，应调转始末端方向)
		do
		{
			pCombineRod->members.Empty();
			pHeadElem=pPrevElem;
			pCombineRod->members.append(pHeadElem->pRod);
			for (pElem=elemArr.GetNext();pElem;pElem=elemArr.GetNext())
			{
				CLDSNode* pMergeNode;
				if(!pPrevElem->reverseStart2End)
					pMergeNode=pPrevElem->pRod->pEnd;
				else
					pMergeNode=pPrevElem->pRod->pStart;
				if( (!pElem->reverseStart2End && pElem->pRod->pStart!=pMergeNode)||		
					(pElem->reverseStart2End  && pElem->pRod->pEnd!=pMergeNode))
				{
					pTailElem=pPrevElem;
					//可能是共线的非连续杆件，如横隔面上对角线方向的断续辅材杆件,wjh-2015.12.10
					pPrevElem=pElem;
					break;
				}
				if(ppVerifySlopes!=NULL)
				{
					GEPOINT checkpoint=pMergeNode->xOriginalPos;
					if(checkpoint.x<0)
						checkpoint.x*=-1;
					if(checkpoint.y<0)
						checkpoint.y*=-1;
					GEPOINT vCompareLenStdVec=pCombineRod->lenStdVec;
					if(vCompareLenStdVec.z<0)
						vCompareLenStdVec*=-1.0;
					if(vCompareLenStdVec.x<0)
						vCompareLenStdVec.x=-vCompareLenStdVec.x;
					if(vCompareLenStdVec.y<0)
						vCompareLenStdVec.y=-vCompareLenStdVec.y;
					bool bTranversePostLine=false;
					for(int j=0;!bTranversePostLine&&j<nSlopeCount;j++)
					{
						if(fabs(vCompareLenStdVec*ppVerifySlopes[j]->lenStdVec)>EPS_COS2)
							continue;	//接近共线
						if(ppVerifySlopes[j]->TestInSlopeLine(checkpoint,NULL,pTolerance->distance))
							bTranversePostLine=true;
					}
					if(bTranversePostLine)
					{
						tranverse_hits++;
						pTailElem=pPrevElem;
						//可能是共线的非连续杆件，如横隔面上对角线方向的断续辅材杆件,wjh-2015.12.10
						pPrevElem=pElem;
						break;
					}
				}
				pCombineRod->members.append(pElem->pRod);
				pPrevElem=pTailElem=pElem;
			}
			if(pCombineRod->members.GetNodeNum()>1)
			{
				CLDSNode* pStartNode=NULL,*pEndNode=NULL;
				if(pHeadElem->reverseStart2End)
					pStartNode=pHeadElem->pRod->pEnd;
				else
					pStartNode=pHeadElem->pRod->pStart;
				if(pTailElem->reverseStart2End)
					pEndNode=pTailElem->pRod->pStart;
				else
					pEndNode=pTailElem->pRod->pEnd;
				if(tranverse_hits>=2)
				{	//第二次穿越塔身主柱分界线后应调转始末端方向
					CLDSNode* pNode=pStartNode;
					pStartNode=pEndNode;
					pEndNode=pNode;
					LINEPARTSET tmprodset;
					for(pRod=pCombineRod->members.GetTail();pRod;pRod=pCombineRod->members.GetPrev())
						tmprodset.append(pRod);
					pCombineRod->members.Empty();
					for(pRod=tmprodset.GetFirst();pRod;pRod=tmprodset.GetNext())
						pCombineRod->members.append(pRod);
				}
				FinishComposLinePart(pCombineRod->members,pStartNode,pEndNode,bMechanicalPersist, (g_pSolidSet->eDisplayMode == DISP_SOLID));
			}
		}while(pElem!=NULL);
		DisplayProcess(10+i*90/n,"合并离散杆件单元!");
	}
	//由于原模型中存在很多节点父杆件不正确情况，在合并杆件后需要重新修正节点的父杆件
	if(bCorrectNodeFatherRodInfo)
		CorrectNodeFatherRodInfo();
	DisplayProcess(100);
	g_pSolidDraw->Draw();
}
bool CorrectSingleAngleWingNormParams(CLDSLinePart* pRod)
{
	if(pRod->pStart==NULL||pRod->pEnd==NULL||!pRod->IsAngle())
		return false;
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pRod;
	if(pAngle->group_father_jg_h>0x20)
		return false;	//组合角钢的子角钢不单独进行修正
	bool needCorrectX=false,needCorrectY=false;
	while(true)
	{
		if(pAngle->des_norm_x.bSpecific&&pAngle->vxWingNorm.IsZero())
			needCorrectX=true;	//指定法线方向为空值
		else if((pAngle->des_norm_x.hViceJg==0||pAngle->des_norm_x.hViceJg==pAngle->handle)&&
			!pAngle->des_norm_x.bByOtherWing&&!pAngle->des_norm_x.bSpecific)
			needCorrectX=true;	//参照面法线，但参照杆件为空或自参照
		if(pAngle->des_norm_y.bSpecific&&pAngle->vyWingNorm.IsZero())
			needCorrectY=true;	//指定法线方向为空值
		else if((pAngle->des_norm_y.hViceJg==0||pAngle->des_norm_y.hViceJg==pAngle->handle)&&
			!pAngle->des_norm_y.bByOtherWing&&!pAngle->des_norm_y.bSpecific)
			needCorrectY=true;	//参照面法线，但参照杆件为空或自参照
		else if(pAngle->des_norm_x.bByOtherWing&&pAngle->des_norm_y.bByOtherWing)
			needCorrectY=true;	//两肢法线互相依附
		else if(fabs(pAngle->vxWingNorm*pAngle->vyWingNorm)>EPS_COS)	//两肢法向相同或相反
			needCorrectY=true;
		if(!needCorrectX&&!needCorrectY)
		{
			if(pAngle->vxWingNorm.IsZero()||pAngle->vyWingNorm.IsZero())
			{
				pAngle->cal_x_wing_norm(true);
				pAngle->cal_y_wing_norm();
			}
			break;	//不需要修正
		}

		GEPOINT vRefRodVec,vWingNorm;
		GEPOINT vCurrRodVec;
		if(pRod->pStart&&pRod->pEnd)
			vCurrRodVec=pRod->pEnd->Position(false)-pRod->pStart->Position(false);
		else
			vCurrRodVec=pRod->End()-pRod->Start();
		normalize(vCurrRodVec);
		CLDSLinePart* pTestDependRod=NULL;
		if(pAngle->pStart!=NULL && pAngle->pStart->hFatherPart!=pAngle->handle)
			pTestDependRod=Ta.FromRodHandle(pAngle->pStart->hFatherPart);
		if(pTestDependRod==NULL && pAngle->pEnd!=NULL && pAngle->pEnd->hFatherPart!=pAngle->handle)
			pTestDependRod=Ta.FromRodHandle(pAngle->pEnd->hFatherPart);
		if(pTestDependRod)
		{
			if(pTestDependRod->pStart&&pTestDependRod->pEnd)
				vRefRodVec=pTestDependRod->pEnd->Position(false)-pTestDependRod->pStart->Position(false);
			else
				vRefRodVec=pTestDependRod->End()-pTestDependRod->Start();
			normalize(vRefRodVec);
			vWingNorm=vCurrRodVec^vRefRodVec;
			if(vWingNorm.IsZero())	//参照杆件共线时视同找不到参照杆件
				pTestDependRod=NULL;
		}
		if(pTestDependRod==NULL)
		{
			if(needCorrectX)
			{
				GEPOINT norm=CalAxisYZByX(vCurrRodVec,1);
				pAngle->set_norm_x_wing(norm);
			}
			if(needCorrectY)
			{
				GEPOINT norm=pAngle->vxWingNorm^vCurrRodVec;
				pAngle->set_norm_y_wing(norm);
			}
			break;	//缺少参照杆件，只能随意算角钢肢法线
		}
		if(needCorrectX)
		{
			pAngle->des_norm_x.hViceJg=pTestDependRod->handle;
			pAngle->des_norm_x.bSpecific=FALSE;
			pAngle->des_norm_x.bByOtherWing=FALSE;
			pAngle->cal_x_wing_norm(true);
		}
		if(needCorrectY)
		{
			if(!pAngle->des_norm_x.bByOtherWing)
				pAngle->des_norm_y.bByOtherWing=TRUE;	//X与Y肢不能同时依赖
			else
			{
				pAngle->des_norm_y.hViceJg=pTestDependRod->handle;
				pAngle->des_norm_y.bByOtherWing=FALSE;
			}
			pAngle->des_norm_y.bSpecific=FALSE;
		}
		if(needCorrectX||needCorrectY)
			pAngle->cal_y_wing_norm(true);
		break;
	}
	if(needCorrectX||needCorrectY)
	{
		if(pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
			((CLDSGroupLineAngle*)pAngle)->UpdateSonJgWingNorm();
		pAngle->CalPosition();
		pAngle->SetModified();
	}
	return true;
}
void CorrectInvalidAngleWingNormParams()
{
	CLdsPartListStack stack(&Ta.Parts);
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		CorrectSingleAngleWingNormParams(pRod);
}
static bool UnifyColinearRodsPosParams(LINEPARTSET& rodset,CLDSNode* pStartTipNode,CLDSNode* pEndTipNode)
{
	CLDSLinePart* pStartRod=rodset.GetFirst();
	CLDSLinePart* pEndRod=rodset.GetTail();
	if(pStartRod==NULL||pEndRod==NULL||rodset.GetNodeNum()==1)
		return false;
	bool bStartIsStartNode=pStartRod->pStart==pStartTipNode ? true : false;
	bool bEndIsStartNode  =pEndRod->pStart  ==  pEndTipNode ? true : false;

	for(CLDSLinePart* pInternalRod=rodset.GetFirst();pInternalRod;pInternalRod=rodset.GetNext())
	{
		if(!pInternalRod->IsAngle())
			continue;
		CLDSLineAngle* pInternalAngle=(CLDSLineAngle*)pInternalRod;
		if(pInternalAngle->pStart==pStartTipNode&&pInternalAngle||pInternalAngle->pStart==pEndTipNode)
		{
			if(pInternalAngle->Layer(0)=='T'&&pInternalAngle->desStartPos.spatialOperationStyle>=16)
			{	//塔头角钢由塔身控制点改为两肢坐标控制
				pInternalAngle->desStartPos.spatialOperationStyle=15;	//改为两肢坐标控制
				pInternalAngle->desStartPos.wing_x_offset.gStyle=pInternalAngle->desStartPos.wing_y_offset.gStyle=0;
			}
			else if(pInternalAngle->desStartPos.spatialOperationStyle==16)
			{
				pInternalAngle->desStartPos.spatialOperationStyle=18;
				pInternalAngle->desStartPos.wing_x_offset.gStyle=pInternalAngle->desStartPos.wing_y_offset.gStyle=0;
			}
			else if(pInternalAngle->desStartPos.spatialOperationStyle==17)
			{
				pInternalAngle->desStartPos.spatialOperationStyle=19;
				pInternalAngle->desStartPos.wing_x_offset.gStyle=pInternalAngle->desStartPos.wing_y_offset.gStyle=0;
			}
		}
		else if(pInternalAngle->pStart->hFatherPart==pInternalAngle->handle)
		{
			if(pInternalAngle->Layer(0)=='T')
				pInternalAngle->desStartPos.spatialOperationStyle=16;
			else
			{	//塔身及腿部设为平推
				pInternalAngle->desStartPos.spatialOperationStyle=17;
				pInternalAngle->desStartPos.face_offset_norm.Set(0,0,1);
			}
			pInternalAngle->desStartPos.datumLineStart.datum_pos_style=pInternalAngle->desStartPos.datumLineEnd.datum_pos_style=1;
			pInternalAngle->desStartPos.datumLineStart.des_para.RODEND.wing_offset_style=pInternalAngle->desStartPos.datumLineEnd.des_para.RODEND.wing_offset_style=4;
			pInternalAngle->desStartPos.datumLineStart.des_para.RODEND.wing_offset_dist=pInternalAngle->desStartPos.datumLineEnd.des_para.RODEND.wing_offset_dist=0;
			pInternalAngle->desStartPos.datumLineStart.des_para.RODEND.hRod=pStartRod->handle;
			pInternalAngle->desStartPos.datumLineStart.des_para.RODEND.direction=!bStartIsStartNode;
			pInternalAngle->desStartPos.datumLineEnd.des_para.RODEND.hRod=pEndRod->handle;
			pInternalAngle->desStartPos.datumLineEnd.des_para.RODEND.direction=!bEndIsStartNode;
		}
		else
		{
			pInternalAngle->desStartPos.spatialOperationStyle=1;
			pInternalAngle->desStartPos.wing_x_offset.gStyle=pInternalAngle->desStartPos.wing_y_offset.gStyle=4;
			pInternalAngle->desStartPos.wing_x_offset.offsetDist=pInternalAngle->desStartPos.wing_y_offset.offsetDist=0;
		}
		if(pInternalAngle->pEnd==pStartTipNode&&pInternalAngle||pInternalAngle->pEnd==pEndTipNode)
		{
			if(pInternalAngle->Layer(0)=='T'&&pInternalAngle->desEndPos.spatialOperationStyle>=16)
			{	//塔头角钢由塔身控制点改为两肢坐标控制
				pInternalAngle->desEndPos.spatialOperationStyle=15;
				pInternalAngle->desEndPos.wing_x_offset.gStyle=pInternalAngle->desEndPos.wing_y_offset.gStyle=0;
			}
			else if(pInternalAngle->desEndPos.spatialOperationStyle==16)
			{
				pInternalAngle->desEndPos.spatialOperationStyle=18;
				pInternalAngle->desEndPos.wing_x_offset.gStyle=pInternalAngle->desEndPos.wing_y_offset.gStyle=0;
			}
			else if(pInternalAngle->desEndPos.spatialOperationStyle==17)
			{
				pInternalAngle->desEndPos.spatialOperationStyle=19;
				pInternalAngle->desEndPos.wing_x_offset.gStyle=pInternalAngle->desEndPos.wing_y_offset.gStyle=0;
			}
		}
		else if(pInternalAngle->pEnd->hFatherPart==pInternalAngle->handle)
		{
			if(pInternalAngle->Layer(0)=='T')	//头部设为点线投影
				pInternalAngle->desEndPos.spatialOperationStyle=16;
			else
			{	//塔身及腿部设为平推
				pInternalAngle->desEndPos.spatialOperationStyle=17;
				pInternalAngle->desEndPos.face_offset_norm.Set(0,0,1);
			}
			pInternalAngle->desEndPos.spatialOperationStyle=16;
			pInternalAngle->desEndPos.datumLineStart.datum_pos_style=pInternalAngle->desEndPos.datumLineEnd.datum_pos_style=1;
			pInternalAngle->desEndPos.datumLineStart.des_para.RODEND.wing_offset_style=pInternalAngle->desEndPos.datumLineEnd.des_para.RODEND.wing_offset_style=4;
			pInternalAngle->desEndPos.datumLineStart.des_para.RODEND.wing_offset_dist=pInternalAngle->desEndPos.datumLineEnd.des_para.RODEND.wing_offset_dist=0;
			pInternalAngle->desEndPos.datumLineStart.des_para.RODEND.hRod=pStartRod->handle;
			pInternalAngle->desEndPos.datumLineStart.des_para.RODEND.direction=!bStartIsStartNode;
			pInternalAngle->desEndPos.datumLineEnd.des_para.RODEND.hRod=pEndRod->handle;
			pInternalAngle->desEndPos.datumLineEnd.des_para.RODEND.direction=!bEndIsStartNode;
		}
		else
		{
			pInternalAngle->desEndPos.spatialOperationStyle=1;
			pInternalAngle->desEndPos.wing_x_offset.gStyle=pInternalAngle->desStartPos.wing_y_offset.gStyle=4;
			pInternalAngle->desEndPos.wing_x_offset.offsetDist=pInternalAngle->desStartPos.wing_y_offset.offsetDist=0;
		}
	}
	return true;
}
void CorrectSelfLocationAnglePosParams(COLINEAR_TOLERANCE* pTolerance=NULL)
{
	COLINEAR_TOLERANCE default_tolerance;
	if(pTolerance==NULL)
		pTolerance=&default_tolerance;
	CLdsPartListStack stack(&Ta.Parts);
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
 		if(pRod->pStart==NULL||pRod->pEnd==NULL||!pRod->IsAngle())
			continue;
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pRod;
		LINEPARTSET rodset;
		bool bStartIsStartNode=true,bEndIsStartNode=false;
		bool bColinearUnified=false;
		bool bStartPosSelfDependent=pAngle->IsPosSelfDependent(true);
		bool bEndPosSelfDependent=pAngle->IsPosSelfDependent(false);
		if (pAngle->pStart->hFatherPart == pAngle->handle)
		{	//判断端节点父杆件是否为当前角钢，设此条件用以避免将未修正杆件的摆放位置的搭接定位筛选出去 wjh-2019.6.28
			if (pAngle->Layer(0) == 'T'&&pAngle->desStartPos.spatialOperationStyle >= 18)
				pAngle->desStartPos.spatialOperationStyle = 15;
			else if ((pAngle->desStartPos.spatialOperationStyle == 16 || pAngle->desStartPos.spatialOperationStyle == 17) && bStartPosSelfDependent)
			{	//点线投影或平推时的投影线端点依赖当前端楞点
				if (pAngle->Layer(0) == 'T')
					pAngle->desStartPos.spatialOperationStyle = 15;
				else
					pAngle->desStartPos.spatialOperationStyle = 18;
			}
			else if ((pAngle->desStartPos.spatialOperationStyle >= 18 && pAngle->Layer(0) == 'T') ||	//头部杆件楞点为塔身控制点
				pAngle->pStart->hFatherPart == pAngle->handle&&pAngle->desStartPos.datum_jg_h == 0 &&
				(pAngle->desStartPos.spatialOperationStyle >= 1 && pAngle->desStartPos.spatialOperationStyle <= 14))
			{	//始端楞点定位出现自依赖，需要修正
				if (pAngle->pStart->m_cPosCalType == 0 && pAngle->Layer(0) == 'T')
					pAngle->desStartPos.spatialOperationStyle = 15;
				else if (pAngle->pStart->m_cPosCalType == 0)
				{
					pAngle->desStartPos.spatialOperationStyle = 19;
					pAngle->desStartPos.face_offset_norm.Set(0, 0, 1);
				}
				else //if(pAngle->pStart->m_cPosCalType>=1&&pAngle->pStart->m_cPosCalType<=3)
				{	//一般为共线点
					Ta.SearchSortedColinearRods(pAngle, rodset, &bStartIsStartNode, &bEndIsStartNode, pTolerance->distance, pTolerance->tangent);	//一般为共线点
					CLDSLinePart* pStartRod = rodset.GetFirst();
					CLDSLinePart* pEndRod = rodset.GetTail();
					CLDSNode* pStartTipNode = pStartRod->pStart;
					CLDSNode* pEndTipNode = pEndRod->pEnd;
					if (!bStartIsStartNode)
						pStartTipNode = pStartRod->pEnd;
					if (bEndIsStartNode)
						pEndTipNode = pEndRod->pStart;
					if (!(bColinearUnified = UnifyColinearRodsPosParams(rodset, pStartTipNode, pEndTipNode)) && bStartPosSelfDependent)
						pAngle->desStartPos.spatialOperationStyle = 15;
				}
				if (pAngle->desStartPos.spatialOperationStyle == 15)
					pAngle->desStartPos.wing_x_offset.gStyle = pAngle->desStartPos.wing_y_offset.gStyle = 0;
			}
		}
		if (pAngle->pEnd->hFatherPart == pAngle->handle)
		{
			if (pAngle->Layer(0) == 'T'&&pAngle->desEndPos.spatialOperationStyle >= 18)
				pAngle->desEndPos.spatialOperationStyle = 15;
			else if ((pAngle->desEndPos.spatialOperationStyle == 16 || pAngle->desEndPos.spatialOperationStyle == 17) && pAngle->IsPosSelfDependent(false))
			{	//点线投影或平推时的投影线端点依赖当前端楞点
				if (pAngle->Layer(0) == 'T')
					pAngle->desEndPos.spatialOperationStyle = 15;
				else
					pAngle->desEndPos.spatialOperationStyle = 18;
			}
			else if ((pAngle->desEndPos.spatialOperationStyle >= 18 && pAngle->Layer(0) == 'T') ||	//头部杆件楞点为塔身控制点
				pAngle->pEnd->hFatherPart == pAngle->handle&&pAngle->desEndPos.datum_jg_h == 0 &&
				(pAngle->desEndPos.spatialOperationStyle >= 1 && pAngle->desEndPos.spatialOperationStyle <= 14))
			{	//终端楞点定位出现自依赖，需要修正
				if (pAngle->pEnd->m_cPosCalType == 0 && pAngle->Layer(0) == 'T')
					pAngle->desEndPos.spatialOperationStyle = 15;
				else if (pAngle->pEnd->m_cPosCalType == 0)
				{
					pAngle->desEndPos.spatialOperationStyle = 19;
					pAngle->desEndPos.face_offset_norm.Set(0, 0, 1);
				}
				else //if(pAngle->pEnd->m_cPosCalType>=1&&pAngle->pEnd->m_cPosCalType<=3)
				{	//一般为共线点
					if (rodset.GetNodeNum() == 0)	//尚未进行共线杆件搜索
						Ta.SearchSortedColinearRods(pAngle, rodset, &bStartIsStartNode, &bEndIsStartNode, pTolerance->distance, pTolerance->tangent);
					CLDSLinePart* pStartRod = rodset.GetFirst();
					CLDSLinePart* pEndRod = rodset.GetTail();
					CLDSNode* pStartTipNode = pStartRod->pStart;
					CLDSNode* pEndTipNode = pEndRod->pEnd;
					if (!bStartIsStartNode)
						pStartTipNode = pStartRod->pEnd;
					if (bEndIsStartNode)
						pEndTipNode = pEndRod->pStart;
					if (!bColinearUnified)
						bColinearUnified = UnifyColinearRodsPosParams(rodset, pStartTipNode, pEndTipNode);
					if (!bColinearUnified&&bEndPosSelfDependent)
						pAngle->desEndPos.spatialOperationStyle = 15;
				}
				if (pAngle->desEndPos.spatialOperationStyle == 15)
					pAngle->desEndPos.wing_x_offset.gStyle = pAngle->desEndPos.wing_y_offset.gStyle = 0;
			}
		}
	}
}

void CUnifyModelLayer::CorrectPostLineNodes(COLINEAR_TOLERANCE* pTolerance/*=NULL*/)	//修正位于塔身角线上的无任何依赖节点
{
	CLDSNode* pNode;
	CUnifyModelLayer::SLOPE *pSlope,*pPrevSlope=NULL;
	CXhSimpleList<CUnifyModelLayer::SLOPE> listTmpSlopes;
	if((pPrevSlope=listSlopes.GetFirst())==NULL)
	{
		logerr.Log("未提取到塔身坡段信息，主材位置修正失败!");
		return;
	}
	
	GEPOINT xSlopStart,xSlopeEnd;
	COLINEAR_TOLERANCE tolerance;
	if(pTolerance==NULL)
		pTolerance=&tolerance;
	pPrevSlope=listTmpSlopes.Append(*pPrevSlope);
	for(pSlope=listSlopes.GetNext();pSlope;pSlope=listSlopes.GetNext())
	{
		GEPOINT hvSect=pSlope->top-pPrevSlope->bottom;
		hvSect-=(hvSect*pPrevSlope->lenStdVec)*pPrevSlope->lenStdVec;
		double distance=hvSect.mod();
		distance*=pPrevSlope->length/(pPrevSlope->length+pSlope->length);
		if(distance<pTolerance->distance)
			pPrevSlope->MergeWithUpperSlope(pSlope);
		else
			pPrevSlope=listTmpSlopes.Append(*pSlope);
	}
	CHashSet<CLDSNodePtr> hashPostNodes;
	BOOL pushed=Ta.Node.push_stack();
	for(pNode=Ta.EnumNodeFirst();pNode;pNode=Ta.EnumNodeNext())
	{
		if(pNode->xOriginalPos.x<=EPS||pNode->xOriginalPos.y<=EPS)
			continue;	//只检测第一象限节点
		if( pNode->m_cPosCalType==CLDSNode::COORD_INDEPENDENT||(
			pNode->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED&&(
			pNode->arrRelationNode[0]==pNode->handle||pNode->arrRelationNode[1]==pNode->handle)))
		{
			for(pSlope=listSlopes.GetFirst();pSlope;pSlope=listSlopes.GetNext())
			{
				if(pNode->xOriginalPos.z<=pSlope->top.z-1||pNode->xOriginalPos.z>pSlope->bottom.z+1)
					continue;
				if(pSlope->TestInSlopeLine(pNode->xOriginalPos,NULL,pTolerance->distance))
					hashPostNodes.SetValue(pNode->handle,pNode);
			}
		}
	}
	Ta.Node.pop_stack(pushed);

	if(listSlopes.GetNodeNum()!=listTmpSlopes.Count)
	{
		listSlopes.Empty();
		for(pSlope=listTmpSlopes.EnumObjectFirst();pSlope;pSlope=listTmpSlopes.EnumObjectNext())
			listSlopes.append(*pSlope);
	}
	PRESET_ARRAY32<CLDSNodePtr> znodes;
	for(pSlope=listSlopes.GetFirst();pSlope;pSlope=listSlopes.GetNext())
	{
		long hTopNode=pSlope->bTopOnStart?pSlope->pTopRod->pStart->handle:pSlope->pTopRod->pEnd->handle;
		long hBtmNode=pSlope->bBtmOnStart?pSlope->pBtmRod->pStart->handle:pSlope->pBtmRod->pEnd->handle;
		for(pNode=hashPostNodes.GetFirst();pNode;pNode=hashPostNodes.GetNext())
		{
			if(pNode->xOriginalPos.z<pSlope->top.z+10||pNode->xOriginalPos.z>pSlope->bottom.z-10)
				continue;
			pNode->m_cPosCalType=CLDSNode::COORD_Z_SPECIFIED;
			pNode->arrRelationNode[0]=hTopNode;
			pNode->arrRelationNode[1]=hBtmNode;
			pNode->CalPosition();
			pNode->SyncMirProp("pos");
		}
	}
}
//修正塔身躯干杆件方位
void CUnifyModelLayer::CorrectTrunkBodyPosition()
{
	//初始化坡段基准线
	for(CUnifyModelLayer::SLOPE* pSlope=listSlopes.GetFirst();pSlope;pSlope=listSlopes.GetNext())
	{
		pSlope->length = DISTANCE(pSlope->top, pSlope->bottom);
		if(pSlope->pDatumLine!=NULL)
			continue;
		if(pSlope->pTopRod&&pSlope->pBtmRod)
		{
			/*CLDSPoint* pStartPoint = Ta.Point.append();
			CLDSPoint* pEndPoint = Ta.Point.append();
			pStartPoint->datum_pos_style = pEndPoint->datum_pos_style = 1;
			pStartPoint->des_para.RODEND.hRod = pSlope->pTopRod->handle;
			pEndPoint->des_para.RODEND.hRod = pSlope->pBtmRod->handle;
			if (pSlope->bTopOnStart)
				pStartPoint->des_para.RODEND.direction = 0;
			else
				pStartPoint->des_para.RODEND.direction = 1;
			if (pSlope->bBtmOnStart)
				pEndPoint->des_para.RODEND.direction = 0;
			else
				pEndPoint->des_para.RODEND.direction = 1;
			pStartPoint->des_para.RODEND.SetWingOffset(0, 4);
			pEndPoint->des_para.RODEND.SetWingOffset(0, 4);*/
			//坡段基准线
			pSlope->pDatumLine = Ta.Line.append();
			strcpy(pSlope->pDatumLine->description, "坡段");
			//pSlope->pDatumLine->hPointArr[0] = pStartPoint->handle;
			//pSlope->pDatumLine->hPointArr[1] = pEndPoint->handle;
			//用简易参数化点可避免反复多次添加基准点的问题 wjh-2020.1.20
			pSlope->pDatumLine->hPointArr[0] = HANDLEPOINT(pSlope->pTopRod->handle,pSlope->bTopOnStart?'S':'E');
			pSlope->pDatumLine->hPointArr[1] = HANDLEPOINT(pSlope->pBtmRod->handle,pSlope->bBtmOnStart?'S':'E');
		}
	}
	//提取1象限塔身主材并统一修正配材号
	CLDSLinePart* pRod;
	LINEPARTSET postRodSet;
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{	//清零１象限塔身主材节点父杆件
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		if(pRod->chLayer1st=='T' || pRod->Layer(1)!='Z' || pRod->Layer(2)!='1')
			continue;
		pRod->ClearFlag();
		if(pRod->IsAngle())
		{
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pRod;
			if(pAngle->group_father_jg_h>0x20)
				continue;
		}
		pRod->pStart->hFatherPart=0;
		pRod->pEnd->hFatherPart=0;
		pRod->UnifiedCfgword();
		pRod->pStart->UnifiedCfgword();
		pRod->pEnd->UnifiedCfgword();
		//记录1象限塔身主材
		postRodSet.append(pRod);
	}
	//补充塔身主材端节点的父杆件
	CHashSet<CLDSNode*>hashPostNodes;
	for (pRod = postRodSet.GetFirst(); pRod; pRod = postRodSet.GetNext())
	{	//在配材号相同的前提下，优先将节点父杆件设为下侧连接主材杆件
		CLDSLineAngle* pAngle=pRod->IsAngle()?(CLDSLineAngle*)pRod:NULL;
		if (pRod->pEnd->Position('Z', false) > pRod->pStart->Position('Z', false))
		{	
			if (pRod->pStart->hFatherPart==0 && pRod->cfgword.IsEqual(pRod->pStart->cfgword))
			{
				if(pAngle&&pAngle->desStartPos.spatialOperationStyle>=1&&pAngle&&pAngle->desStartPos.spatialOperationStyle<=14)
					pAngle->desStartPos.spatialOperationStyle=17;	//提前更改避免后续到位时报警示信息 wjh-2019.12.15
				pRod->pStart->hFatherPart = pRod->handle;
			}
		}
		else
		{
			if (pRod->pEnd->hFatherPart==0 && pRod->cfgword.IsEqual(pRod->pEnd->cfgword))
			{
				if(pAngle&&pAngle->desEndPos.spatialOperationStyle>=1&&pAngle&&pAngle->desEndPos.spatialOperationStyle<=14)
					pAngle->desEndPos.spatialOperationStyle=17;	//提前更改避免后续到位时报警示信息 wjh-2019.12.15
				pRod->pEnd->hFatherPart = pRod->handle;
			}
		}
	}
	for (pRod = postRodSet.GetFirst(); pRod; pRod = postRodSet.GetNext())
	{	//补充前面环节未能设定下侧主材为父杆件的节点（必须分批次，置于上一修正环节之后进行） wjh-2019.11.1
		if (pRod->pStart->hFatherPart == 0)
			pRod->pStart->hFatherPart = pRod->handle;
		if (pRod->pEnd->hFatherPart == 0)
			pRod->pEnd->hFatherPart = pRod->handle;
		//记录1象限塔身主材始末端节点
		hashPostNodes.SetValue(pRod->pStart->handle, pRod->pStart);
		hashPostNodes.SetValue(pRod->pEnd->handle, pRod->pEnd);
	}
	for (CLDSNode* pNode = hashPostNodes.GetFirst(); pNode; pNode = hashPostNodes.GetNext())
	{
		CLDSLinePart* pFatherRod = Ta.FromRodHandle(pNode->hFatherPart);
		if (pFatherRod == NULL)
			continue;
		CLDSNode* pMirNodeX = pNode->GetMirNode(MIRMSG(1));
		CLDSNode* pMirNodeY = pNode->GetMirNode(MIRMSG(2));
		CLDSNode* pMirNodeZ = pNode->GetMirNode(MIRMSG(3));
		CLDSLinePart* pMirRodX = (CLDSLinePart*)pFatherRod->GetMirPart(MIRMSG(1), pNode->Position(false));
		CLDSLinePart* pMirRodY = (CLDSLinePart*)pFatherRod->GetMirPart(MIRMSG(2), pNode->Position(false));
		CLDSLinePart* pMirRodZ = (CLDSLinePart*)pFatherRod->GetMirPart(MIRMSG(3), pNode->Position(false));
		if (pMirNodeX&&pMirRodX)
			pMirNodeX->hFatherPart = pMirRodX->handle;
		if (pMirNodeY&&pMirRodY)
			pMirNodeY->hFatherPart = pMirRodY->handle;
		if (pMirNodeZ&&pMirRodZ)
			pMirNodeZ->hFatherPart = pMirRodZ->handle;
	}
	//重置塔身主材方向
	int i=0,nPostRod=postRodSet.GetNodeNum()*2;
	DisplayProcess(0,"重置塔身主材方向和位置");
	for(CLDSLinePart* pRod=postRodSet.GetFirst();pRod;pRod=postRodSet.GetNext(),i++)
	{
		DisplayProcess(i * 100 / nPostRod, "重置塔身主材方向和位置");
		if (!pRod->IsAngle())
			continue;
		CLDSLineAngle* pAngle = (CLDSLineAngle*)pRod;
		CLDSLineAngle* pMirAngleX = (CLDSLineAngle*)pRod->GetMirPart(MIRMSG(1), pRod->pStart->Position(false));
		CLDSLineAngle* pMirAngleY = (CLDSLineAngle*)pRod->GetMirPart(MIRMSG(2), pRod->pStart->Position(false));
		CLDSLineAngle* pMirAngleZ = (CLDSLineAngle*)pRod->GetMirPart(MIRMSG(3), pRod->pStart->Position(false));
		if (pMirAngleY)
		{	//前侧工作面
			pAngle->des_norm_x.hViceJg = pMirAngleY->handle;
			pAngle->des_norm_x.near_norm.Set(0, 1, 0);
			pMirAngleY->des_norm_y.hViceJg = pAngle->handle;
			pMirAngleY->des_norm_y.near_norm.Set(0, 1, 0);
			pMirAngleY->des_norm_y.bSpecific = pMirAngleY->des_norm_y.bByOtherWing = pAngle->des_norm_x.bSpecific = pAngle->des_norm_x.bByOtherWing = FALSE;
		}
		if (pMirAngleX)
		{	//右侧工作面
			pAngle->des_norm_y.hViceJg = pMirAngleX->handle;
			pAngle->des_norm_y.near_norm.Set(1, 0, 0);
			pMirAngleX->des_norm_x.hViceJg = pAngle->handle;
			pMirAngleX->des_norm_x.near_norm.Set(1, 0, 0);
			pMirAngleX->des_norm_x.bSpecific = pMirAngleX->des_norm_x.bByOtherWing = pAngle->des_norm_y.bSpecific = pAngle->des_norm_y.bByOtherWing = FALSE;
		}
		if (pMirAngleZ&&pMirAngleY)
		{	//左侧工作面
			pMirAngleZ->des_norm_y.hViceJg = pMirAngleY->handle;
			pMirAngleY->des_norm_x.hViceJg = pMirAngleZ->handle;
			pMirAngleZ->des_norm_y.near_norm = pMirAngleY->des_norm_x.near_norm = f3dPoint(-1, 0, 0);
			pMirAngleY->des_norm_x.bSpecific = pMirAngleY->des_norm_x.bByOtherWing = pMirAngleZ->des_norm_y.bSpecific = pMirAngleZ->des_norm_y.bByOtherWing = FALSE;
		}
		if (pMirAngleZ&&pMirAngleX)
		{	//后侧工作面
			pMirAngleZ->des_norm_x.hViceJg = pMirAngleX->handle;
			pMirAngleX->des_norm_y.hViceJg = pMirAngleZ->handle;
			pMirAngleX->des_norm_y.near_norm = pMirAngleZ->des_norm_x.near_norm = f3dPoint(0, -1, 0);
			pMirAngleX->des_norm_y.bSpecific = pMirAngleX->des_norm_y.bByOtherWing = pMirAngleZ->des_norm_x.bSpecific = pMirAngleZ->des_norm_x.bByOtherWing = FALSE;
		}
		bool blReverseStart2End=pAngle->xPosEnd.z<pAngle->xPosStart.z;
		CLDSLineAngle* xarrAngles[4]={ pAngle,pMirAngleX,pMirAngleY,pMirAngleZ };
		for (int i = 0; i < 4; i++)
		{
			if(xarrAngles[i]==NULL)
				continue;
			xarrAngles[i]->ClearFlag();
			if(blReverseStart2End)
			{	//始->终方向朝上(Z轴负向)与默认假定反向，需调整X/Y肢方向
				DESIGN_JGWING_NORM xDesTmpNorm=xarrAngles[i]->des_norm_x;
				xarrAngles[i]->des_norm_x=xarrAngles[i]->des_norm_y;
				xarrAngles[i]->des_norm_y=xDesTmpNorm;
			}
			xarrAngles[i]->cal_x_wing_norm();
			xarrAngles[i]->cal_y_wing_norm();
			if (xarrAngles[i]->GetClassTypeId() == CLS_GROUPLINEANGLE)
				((CLDSGroupLineAngle*)xarrAngles[i])->UpdateSonJgWingNorm();
		}
	}
	//重置塔身主材位置
	for(CLDSLinePart* pRod=postRodSet.GetFirst();pRod;pRod=postRodSet.GetNext(),i++)
	{
		DisplayProcess(i * 100 / nPostRod, "重置塔身主材方向和位置");
		if(!pRod->IsAngle())
			continue;
		CLDSLineAngle* pAngle = (CLDSLineAngle*)pRod;
		CUnifyModelLayer::SLOPE* pSlope = NULL;
		for(pSlope=listSlopes.GetFirst();pSlope;pSlope=listSlopes.GetNext())
		{
			double startdist2top=0,enddist2top=0;
			if( pSlope->TestInSlopeLine(pAngle->pStart->Position(false),&startdist2top,xTolerance.distance)&&
				pSlope->TestInSlopeLine(pAngle->pEnd->Position(false),&enddist2top,xTolerance.distance))
			{
				if(startdist2top>EPS||startdist2top<-pSlope->length-EPS)
					continue;
				if(enddist2top>EPS||enddist2top<-pSlope->length-EPS)
					continue;
				break;
			}
		}
		if(pSlope==NULL)
			continue;
		long hRodPointS = pSlope->pDatumLine->hPointArr[0];
		long hRodPointE = pSlope->pDatumLine->hPointArr[1];
		for (int i = 0; i < 4; i++)
		{
			BYTE cFaceOffsetDatumLine = (i == 0 || i == 3) ? 'X' : 'Y';
			BYTE ciMirType = 0;
			CLDSLineAngle* pDestAngle = pAngle;
			if (i > 0)
			{
				pDestAngle = (CLDSLineAngle*)pAngle->GetMirPart(MIRMSG(i), pRod->pStart->Position(false));
				if (i == 1)
					ciMirType = 'X';
				else if (i == 2)
					ciMirType = 'Y';
				else if (i == 3)
					ciMirType = 'Z';
			}
			if(pDestAngle ==NULL)
				continue;
			for (int ii = 0; ii < 2; ii++)
			{
				CLDSNode* pDesNode = (ii == 0) ? pDestAngle->pStart : pDestAngle->pEnd;
				CMultiOffsetPos* pPosDesPara = (ii == 0) ? &pDestAngle->desStartPos : &pDestAngle->desEndPos;
				long hDatumJg = pPosDesPara->datum_jg_h;
				*pPosDesPara = (ii == 0) ? pAngle->desStartPos : pAngle->desEndPos;
				pPosDesPara->datum_jg_h = hDatumJg;
				if (pDestAngle->handle!=pDesNode->hFatherPart&&(pDestAngle->handle!=hRodPointS&&pDestAngle->handle!=hRodPointE))
				{	//向楞线投影
					pPosDesPara->spatialOperationStyle = 1;
					pPosDesPara->wing_x_offset.SetOffset(0, 4);
					pPosDesPara->wing_y_offset.SetOffset(0, 4);
				}
				else if (
					(fabs(pDesNode->xOriginalPos.z-pSlope->top.z)<EPS2||fabs(pDesNode->xOriginalPos.z-pSlope->bottom.z)<EPS2)&&
					(hRodPointS == pDestAngle->handle)||(hRodPointE==pDestAngle->handle))
				{	//塔身控制点平推(两肢控制)
					//存在节点父杆件非当前角钢情况此时如果为简单定位容易将控制点定位方式冲掉 wjh-2020.1.20
					pPosDesPara->jgber_cal_style=0;
					pPosDesPara->spatialOperationStyle = 19;	
					pPosDesPara->cFaceOffsetDatumLine = cFaceOffsetDatumLine;
					pPosDesPara->wing_x_offset.SetOffset(0, 0);
					pPosDesPara->wing_y_offset.SetOffset(0, 0);
				}
				else
				{	//向指定线平推
					pPosDesPara->spatialOperationStyle = 17;
					pPosDesPara->hDatumLine = pSlope->pDatumLine->handle;
					pPosDesPara->cFaceOffsetDatumLine = cFaceOffsetDatumLine;
					pPosDesPara->cMirTransType = ciMirType;
					pPosDesPara->wing_x_offset.SetOffset(0, 4);
					pPosDesPara->wing_y_offset.SetOffset(0, 4);
				}
			}
			pDestAngle->CalPosition(true);
			if (pDestAngle->GetClassTypeId() == CLS_GROUPLINEANGLE)
				((CLDSGroupLineAngle*)pDestAngle)->UpdateSonJgPos();
		}
	}
	DisplayProcess(100,"重置塔身主材方向和位置");
}
void CUnifyModelLayer::CorrectTGroupAnglePosition()
{
	SmartRodPtr pRod;
	for (pRod=Ta.EnumRodFirst();pRod.blValid;pRod=Ta.EnumRodNext())
	{
		if (pRod->pStart==NULL||pRod->pEnd==NULL||!pRod->IsAngle()||pRod.pAngle->size_cSubClassType!='T')
			continue;
		BITFLAG* pFlag=hashObjRevision.GetValue(pRod->handle);
		if (pFlag!=NULL&&pFlag->IsHasFlag(REVISION::LAYOUT))
			continue;	//之前可能因为其余模板或AI渠道修正过该角钢肢朝向，以之前修正的为准 wjh-2019.9.4
		//ciEndAnchorWing==1"X肢定位";
		//ciEndAnchorWing==2"Y肢定位";
		//ciEndAnchorWing==3"双肢定位";
		BYTE ciStartAnchorWing=pRod.pAngle->ciStartAnchorWing;
		BYTE ciEndAnchorWing=pRod.pAngle->ciEndAnchorWing;
		BYTE ciWorkWing=ciStartAnchorWing;
		if ((ciWorkWing==0||ciWorkWing>2)&&(ciEndAnchorWing==1||ciEndAnchorWing==2))
			ciWorkWing=ciEndAnchorWing;
		else if (ciWorkWing!=ciEndAnchorWing&&(ciEndAnchorWing==1||ciEndAnchorWing==2))
			ciWorkWing=0;
		if (ciWorkWing == 3 && toupper(pRod.pAngle->Layer(1)) == 'Z')
		{	//两肢定位的组合角钢，前后面方向作为角钢的工作肢方向
			GEPOINT xStartPoint = pRod.pAngle->pStart->xPreliftPos;
			GEPOINT xEndPoint = pRod.pAngle->pEnd->xPreliftPos;
			BYTE ciQuad = 0;
			if (xStartPoint.x >= 0 && xStartPoint.y >= 0 && xEndPoint.x >= 0 && xEndPoint.y >= 0)
				ciQuad=1;
			else if (xStartPoint.x <= 0 && xStartPoint.y >= 0 && xEndPoint.x <= 0 && xEndPoint.y >= 0)
				ciQuad=2;
			else if (xStartPoint.x >= 0 && xStartPoint.y <= 0 && xEndPoint.x >= 0 && xEndPoint.y <= 0)
				ciQuad=3;
			else if (xStartPoint.x <= 0 && xStartPoint.y <= 0 && xEndPoint.x <= 0 && xEndPoint.y <= 0)
				ciQuad=4;
			if (ciQuad >= 1 || ciQuad <= 4)
			{
				GEPOINT vWorkNorm(0,1,0);
				if (fabs(vWorkNorm*pRod.pAngle->vxWingNorm) > fabs(vWorkNorm*pRod.pAngle->vyWingNorm))
					ciWorkWing = 1;
				else
					ciWorkWing = 2;
			}
		}
		if (ciWorkWing==1)		//T-X型组合角钢，1号子角钢为特征子角钢
			pRod.pGroupAngle->ciMasterIndex=0;
		else if(ciWorkWing==2)	//T-Y型组合角钢，2号子角钢为特征子角钢
			pRod.pGroupAngle->ciMasterIndex=1;
		pRod.pGroupAngle->ClearFlag();
		pRod.pGroupAngle->cal_x_wing_norm();
		pRod.pGroupAngle->cal_y_wing_norm();
		pRod.pGroupAngle->CalPosition();
		pRod.pGroupAngle->UpdateSonJgPos();
		pRod.pGroupAngle->UpdateSonJgWingNorm();
	}
}
//修正塔头&横担杆件方位
bool CorrectSingleAngleLayout(CLDSLineAngle* pJg,bool updatePartSolid=true);	//defined 
void IntelliDesignTrunkArmBendPlate(CIntelliModelDesigner& unify)
{
	CSuperSmartPtr<CLDSLinePart>pRod;
	CLDSLineAngle* pAngle;
	//塔身侧面的横担主材与塔身间的火曲钢板
	for(pRod=Ta.EnumRodFirst();pRod.IsHasPtr();pRod=Ta.EnumRodNext())
	{
		if(pRod->Layer(0)!='T'||!pRod->IsAngle()||pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;	//只需要重置角钢构件的位置
		pAngle=pRod.LineAnglePointer();
		long hStartDependRod=pAngle->pStart->hFatherPart;
		long hEndDependRod=pAngle->pEnd->hFatherPart;
		if(pAngle->desStartPos.datum_jg_h>0x20)
			hStartDependRod=pAngle->desStartPos.datum_jg_h;
		if(pAngle->desEndPos.datum_jg_h>0x20)
			hEndDependRod=pAngle->desEndPos.datum_jg_h;
		CLDSLineAngle* pStartDependAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(hStartDependRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		CLDSLineAngle* pEndDependAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(hEndDependRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		LDSNORMAL datumvec,rayvec,bendPlaneNorm;
		rayvec.vSolid=pAngle->End()-pAngle->Start();
		rayvec.vLine=pAngle->pEnd->Position(false)-pAngle->pStart->Position(false);
		if(pStartDependAngle&&pStartDependAngle->Layer(0)=='S'&&pStartDependAngle->Layer(1)=='Z')
		{
			CorrectSingleAngleLayout(pAngle,false);
			datumvec.vSolid=pStartDependAngle->End()-pStartDependAngle->Start();
			datumvec.vLine=pStartDependAngle->pEnd->Position(false)-pStartDependAngle->pStart->Position(false);
			bendPlaneNorm.vSolid=datumvec.vSolid^rayvec.vSolid;
			bendPlaneNorm.vLine=datumvec.vLine^rayvec.vLine;
			normalize(bendPlaneNorm.vSolid);
			normalize(bendPlaneNorm.vLine);
			if( !pAngle->desStartPos.IsInHuoQuLine()||
				(pAngle->desStartPos.huoqu_jg_h!=0&&pAngle->desStartPos.huoqu_jg_h!=hStartDependRod))
			{
				double ddx=fabs(bendPlaneNorm.vSolid*pStartDependAngle->get_norm_x_wing());
				double ddy=fabs(bendPlaneNorm.vSolid*pStartDependAngle->get_norm_y_wing());
				if(ddx<EPS_COS2&&ddy<EPS_COS2)
					continue;	//从几何空间上判断，未连接到任何一个基准角钢肢上，极可能为隔面斜辅材
			}
			char cWorkWing=0;
			if(pAngle->desStartPos.IsInDatumJgWingX())
				cWorkWing='X';
			else if(pAngle->desStartPos.IsInDatumJgWingY())
				cWorkWing='Y';
			else if(pStartDependAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2==1)
				cWorkWing='X';
			else if(pStartDependAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2==2)
				cWorkWing='Y';
			else
			{
				if(fabs(bendPlaneNorm.vSolid*pStartDependAngle->get_norm_x_wing())>0.816)	//cos(30°)=0.816
					cWorkWing='X';
				else if(fabs(bendPlaneNorm.vSolid*pStartDependAngle->get_norm_y_wing())>0.816)
					cWorkWing='Y';
			}
			if(cWorkWing=='X'||cWorkWing=='Y')
			{
				CUnifyModelLayer::PLATE plate(pAngle->pStart->handle,pStartDependAngle->handle,cWorkWing);
				if(unify.hashPlates.GetValue(plate.StrKey()))
					continue;	//该节点处当前工作面连接板已设计
				else
				{
					plate.hBendlineAngle=pStartDependAngle->handle;
					plate.bendPlaneNorm=bendPlaneNorm.vLine;
					unify.DesignPlate(plate);
					//if(unify.DesignPlate(plate)>0)
					//	fprintf(fp,"0X%X\n",plate.hPlate);
				}
			}
			else
				continue;
		}
		else if(pEndDependAngle&&pEndDependAngle->Layer(0)=='S'&&pEndDependAngle->Layer(1)=='Z')
		{
			CorrectSingleAngleLayout(pAngle,false);
			datumvec.vSolid=pEndDependAngle->End()-pEndDependAngle->Start();
			datumvec.vLine=pEndDependAngle->pEnd->Position(false)-pEndDependAngle->pStart->Position(false);
			bendPlaneNorm.vSolid=datumvec.vSolid^rayvec.vSolid;
			bendPlaneNorm.vLine=datumvec.vLine^rayvec.vLine;
			normalize(bendPlaneNorm.vSolid);
			normalize(bendPlaneNorm.vLine);
			if( !pAngle->desEndPos.IsInHuoQuLine()||
				(pAngle->desEndPos.huoqu_jg_h!=0&&pAngle->desEndPos.huoqu_jg_h!=hEndDependRod))
			{
				double ddx=fabs(bendPlaneNorm.vSolid*pEndDependAngle->get_norm_x_wing());
				double ddy=fabs(bendPlaneNorm.vSolid*pEndDependAngle->get_norm_y_wing());
				if(ddx<EPS_COS2&&ddy<EPS_COS2)
					continue;	//从几何空间上判断，未连接到任何一个基准角钢肢上，极可能为隔面斜辅材
			}
			char cWorkWing=0;
			if(pAngle->desEndPos.IsInDatumJgWingX())
				cWorkWing='X';
			else if(pAngle->desEndPos.IsInDatumJgWingY())
				cWorkWing='Y';
			else if(pEndDependAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2==1)
				cWorkWing='X';
			else if(pEndDependAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2==2)
				cWorkWing='Y';
			else
			{
				if(fabs(bendPlaneNorm.vSolid*pEndDependAngle->get_norm_x_wing())>0.816)	//cos(30°)=0.816
					cWorkWing='X';
				else if(fabs(bendPlaneNorm.vSolid*pEndDependAngle->get_norm_y_wing())>0.816)
					cWorkWing='Y';
			}
			if(cWorkWing=='X'||cWorkWing=='Y')
			{
				CUnifyModelLayer::PLATE plate(pAngle->pEnd->handle,pEndDependAngle->handle,cWorkWing);
				if(unify.hashPlates.GetValue(plate.StrKey()))
					continue;	//该节点处当前工作面连接板已设计
				else
				{
					plate.hBendlineAngle=pEndDependAngle->handle;
					plate.bendPlaneNorm=bendPlaneNorm.vLine;
					unify.DesignPlate(plate);
				}
			}
			else
				continue;
		}
	}
}

#include "GenRough3dModelDlg.h"
#include "IntelliAgent\IntelliModel.h"
#include "TowerStructDlg.h"
void CLDSView::OnTrunkModelTree()
{
	CTowerStructDlg dlg;
	gxModelTree.SetBelongTower(&Ta);
	if(dlg.DoModal()!=IDOK)
		return;
}
void CLDSView::OnSetFoundationParams()
{
	CFoundationPropDlg dlg;
	dlg.m_pFoundation=&Ta.m_xFoundation;
	if(dlg.DoModal()!=IDOK)
		return;
	for(CModelHeightNode *pHeight=gxModelTree.hashHeights.GetFirst();pHeight;pHeight=gxModelTree.hashHeights.GetNext())
	{
		for(int i=0;i<24;i++)
		{
			if(pHeight->xarrSubLegs[i].fLegHeight<=0)
				continue;
			//pHeight->xarrSubLegs[i].foundationPara=Ta.m_xPreferFoundation;
		}
	}
}
void ExecuteEmendMirRodInfo(DWORD dwParam);	//defined in DataCenter.cpp
void CLDSView::RelayoutRodModel()
{
	//修正模型图层名
	static CGenRough3dModelDlg setdlg;
	if(setdlg.DoModal()!=IDOK)
		return;
	CLogErrorLife loglife;
	CUndoOperObject undo(&Ta,true);
#ifdef __TIMER_COUNT_
	timer.Start();
#endif
	COLINEAR_TOLERANCE tolerance(setdlg.m_fTolerance,setdlg.m_fToleranceTangent);
	static CIntelliModelDesigner intelli;
	intelli.hashPlates.Empty();
	intelli.hashPlanes.Empty();
	intelli.listSlopes.Empty();
	//合并重复节点，否则后面的合并离散共线杆件可能会遗漏
	intelli.MergeModelOverlapNodes();
#ifdef __TIMER_COUNT_
	DWORD dwStartTick=timer.Relay(10);
#endif
	//1.智能初置杆件方位（合并离散杆元，更新图层名，初置杆件方位）
	if(setdlg.m_bCombineColinearRods)
	{
		intelli.RecognizeSlopes(tolerance.distance);
		int i=0,nSlopeCount=intelli.listSlopes.GetNodeNum();
		//DYN_ARRAY<CUnifyModelLayer::SLOPEPTR> arrVerifySlopes(nSlopeCount);
		CUnifyModelLayer::SLOPEPTR xarrPresetSlopes[4]={NULL,NULL,NULL,NULL};
		ARRAY_LIST<CUnifyModelLayer::SLOPEPTR> arrVerifySlopes(xarrPresetSlopes,4);
		for(CUnifyModelLayer::SLOPE* pSlope=intelli.listSlopes.GetFirst();pSlope;pSlope=intelli.listSlopes.GetNext(),i++)
			arrVerifySlopes.append(pSlope);//[i]=pSlope;
		CombineDiscreteRodMembers(setdlg.m_bMechanicPersist,!setdlg.m_bIntelliSetRodPosition,&tolerance,arrVerifySlopes.m_pData,nSlopeCount);//合并同规格(统材)共线杆单元
	}
	intelli.UnifyObjsLayer(tolerance);	//必须置于CombineDiscreteRodMembers之后，否则会因合并杆件导致intelli中部分句柄失效 wjh-2015.12.28
#ifdef __TIMER_COUNT_
	dwStartTick=timer.Relay(1);
#endif
	if(setdlg.m_bIntelliSetRodPosition)
	{
		CIntelliAgent experts(&Ta,setdlg.m_nMinKeyLength);	//初始化experts代码在intelli.UnifyObjsLayer()之后，是因为UnifyObjsLayer可能会修改模型对象的图层名 wjh-2018.5.26
		intelli.SetIntelliExperts(&experts);
#ifdef __TIMER_COUNT_
		dwStartTick=timer.Relay(2,dwStartTick);
#endif
		//补充完善节点的关联对称信息，以便修正mod中的节点坐标计算类型 wjh-2019.5.24
		for (CLDSNode *pNode = Ta.Node.GetFirst(); pNode; pNode = Ta.Node.GetNext())
		{
			BOOL pushed=Ta.Node.push_stack();
			this->FinishRetrieveRelaMirObject(pNode);
			Ta.Node.pop_stack(pushed);
		}
		intelli.CorrectPostLineNodes(&tolerance);
		intelli.CorrectColinearJoints(&tolerance);	//修正共线杆件中间端节点处的连接方式
#ifdef __TIMER_COUNT_
		dwStartTick=timer.Relay(3,dwStartTick);
#endif
		DisplayProcess(0,"识别特殊腿身复合型接腿");
		CTowerModelTree modeltree;
		modeltree.SetBelongTower(&Ta);
		intelli.RetrievedTowerModeTree(modeltree);
		for (CModelHeightNode* pHeightNode=modeltree.hashHeights.GetFirst();pHeightNode;pHeightNode=modeltree.hashHeights.GetNext())
		{
			for (WORD k=0;k<pHeightNode->xarrSubLegs.Count;k++)
			{
				CModelSubLegNode* pSubLeg=pHeightNode->xarrSubLegs.GetAt(k);
				if (!pSubLeg->CorrectPantsKneeJointPosition())
					break;	//非大裤衩型腿身复合接腿
				BITFLAG* pRevFlag;
				//处理大裤衩结构的V斜材顶部端节点父杆件
				pSubLeg->pants.pVLeftTopNode->hFatherPart=pSubLeg->pants.pLeftHoriAngle->handle;
				pSubLeg->pants.pVRightTopNode->hFatherPart=pSubLeg->pants.pRightHoriAngle->handle;
				pRevFlag=intelli.hashObjRevision.Add(pSubLeg->pants.pVLeftTopNode->handle);
				pRevFlag->SetFlagDword(CUnifyModelLayer::REVISION::PARENT_ROD);//设定过该节点父杆件
				pRevFlag=intelli.hashObjRevision.Add(pSubLeg->pants.pVRightTopNode->handle);
				pRevFlag->SetFlagDword(CUnifyModelLayer::REVISION::PARENT_ROD);//设定过该节点父杆件
				for (int mir=1;mir<4;mir++)
				{
					MIRMSG mirmsg(mir);
					CLDSLinePart* pMirParentRod=pSubLeg->pants.pLeftHoriAngle->GetMirRod(mirmsg);
					CLDSNode* pMirNode=pSubLeg->pants.pVLeftTopNode->GetMirNode(mirmsg);
					if (pMirNode&&pMirParentRod)
					{
						pMirNode->hFatherPart=pMirParentRod->handle;
						pRevFlag=intelli.hashObjRevision.Add(pMirNode->handle);
						pRevFlag->SetFlagDword(CUnifyModelLayer::REVISION::PARENT_ROD);//设定过该节点父杆件
					}
					pMirParentRod=pSubLeg->pants.pRightHoriAngle->GetMirRod(mirmsg);
					pMirNode=pSubLeg->pants.pVRightTopNode->GetMirNode(mirmsg);
					if (pMirNode&&pMirParentRod)
					{
						pMirNode->hFatherPart=pMirParentRod->handle;
						pRevFlag=intelli.hashObjRevision.Add(pMirNode->handle);
						pRevFlag->SetFlagDword(CUnifyModelLayer::REVISION::PARENT_ROD);//设定过该节点父杆件
					}
				}
				if (pSubLeg->pants.pLeftHoriAngle->IsBodyObj())
					break;
			}
		}
		DisplayProcess(5,"补充节点依赖杆件信息");
		intelli.CorrectNodeFatherRodInfo(NULL,&tolerance,false);	//必须置于intelli.UnifyObjsLayer()之后，否则会因图层名不对导致部分节点父杆件修正失败 wjh-2015.12.28
		DisplayProcess(50,"补充杆件对称关联信息");
		ExecuteEmendMirRodInfo(0);	//补充对象对称关联信息
		//重置节点坐标，以便于后期模板识别
		for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			pNode->CalPosition(true);
		//修正交叉点的依赖杆件
		intelli.CorrectDependRodsOfIntersNode();
		//修正塔身躯干杆件方位
		intelli.CorrectTrunkBodyPosition();
		CorrectSelfLocationAnglePosParams(&tolerance);
		CorrectInvalidAngleWingNormParams();
#ifdef __TIMER_COUNT_
		dwStartTick=timer.Relay(4,dwStartTick);
#endif
		//修正塔头及横担杆件方位
		intelli.CorrectHeadArmRodPosition();
#ifdef __TIMER_COUNT_
		dwStartTick=timer.Relay(5,dwStartTick);
#endif
		CCorrectModelAngleLayout correct;
		//提取塔身斜材、横材及横隔面布置
		correct.CorrectDiaphragm();			//修正横隔面角钢位置
		correct.CorrectBodyDiagonalRodPos();//修正塔身斜材角钢位置
		CDesignQuadSection quadsects;
		quadsects.DesignQuadSects(&Ta,false);
#ifdef __TIMER_COUNT_
		dwStartTick=timer.Relay(6,dwStartTick);
#endif
		//提取塔身辅材并重置方向与位置
		correct.CorrectAuxRodPosition();
#ifdef __TIMER_COUNT_
		dwStartTick=timer.Relay(7,dwStartTick);
#endif
		//提取头部工作平面,并设置图层
		//提取塔腿V面并重置方向与位置
		correct.CorrectVPanelRodPos();
#ifdef __TIMER_COUNT_
		dwStartTick=timer.Relay(8,dwStartTick);
#endif
		//重置T型组合角钢的默认摆放方式
		intelli.CorrectTGroupAnglePosition();
	}
	//启用角钢推荐心距
	if (setdlg.m_bUsePerfectAngleG)
	{
		//提取塔身及接腿主材角钢（以第一象限为主）
		LINEPARTSET postRodSet;
		for (CLDSLinePart* pRod = Ta.EnumRodFirst(); pRod; pRod = Ta.EnumRodNext())
		{
			SmartRodPtr pSmartRod = pRod;
			if (pRod->pStart == NULL || pRod->pEnd == NULL || pRod->m_bVirtualPart)
				continue;	//短角钢或虚拟构件
			if(pRod->Layer(0) == 'T' || pRod->Layer(1) != 'Z' || pRod->Layer(2) != '1')
				continue;	//非1象限塔身主材
			if (!pSmartRod->IsAngle() || pSmartRod.pAngle->group_father_jg_h > 0x20)
				continue;	//非角钢或组合角钢只处理虚拟组合角钢不处理子角钢
			postRodSet.append(pRod);
		}
		//重置角钢心距
		for (CLDSLinePart* pPostRod=postRodSet.GetFirst();pPostRod;pPostRod=postRodSet.GetNext())
		{
			if (pPostRod == NULL || pPostRod->GetClassTypeId() != CLS_LINEANGLE)
				continue;
			//获取推荐心距
			CLDSLineAngle* pAngle = (CLDSLineAngle*)pPostRod;
			JGZJ jgzj = CLDSLineAngle::GetAngleZJ(pAngle->size_wide);
			WORD wPreferAngleG = (pAngle->size_wide < 140) ? (WORD)jgzj.g : (WORD)jgzj.g1;
			//更新角钢心距
			pAngle->xWingXZhunJu = pAngle->xWingYZhunJu = jgzj;
			if (jgzj.g!=wPreferAngleG)
			{	//设定特殊心距
				pAngle->xWingXZhunJu.g = pAngle->xWingYZhunJu.g = wPreferAngleG;
				pAngle->m_bEnableTeG = true;
				pAngle->SyncMirProp("m_bEnableTeG");	//同步更新四个角上对称的主材角钢心距
			}
		}
#ifdef __TIMER_COUNT_
		dwStartTick = timer.Relay(12, dwStartTick);
#endif
	}
	//全塔到位更新节点&杆件(角钢)位置
	CLDSNode* pNode;
	CLDSLinePart* pRod;
	GEPOINT xOldPos,xOldEndPos,vxMove;
	NODESET nodeSet,moveNodeSet;
	LINEPARTSET rodSet,moveRodSet;
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		nodeSet.append(pNode);
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		rodSet.append(pRod);
	for(int loopI=0;loopI<5;loopI++)
	{
		moveNodeSet.Empty();
		moveRodSet.Empty();
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{
			xOldPos=pNode->xOriginalPos;
			pNode->CalPosition(true);
			vxMove=pNode->xOriginalPos-xOldPos;
			if(fabs(vxMove.x)+fabs(vxMove.y)+fabs(vxMove.z)>EPS2)
				moveNodeSet.append(pNode);
		}
		for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			xOldPos=pRod->xPosStart;
			xOldEndPos=pRod->xPosEnd;
			pRod->CalPosition(true);
			vxMove=pRod->xPosStart-xOldPos;
			if(fabs(vxMove.x)+fabs(vxMove.y)+fabs(vxMove.z)>EPS2)
				moveRodSet.append(pRod);
			else
			{
				vxMove=pRod->xPosEnd-xOldEndPos;
				if(fabs(vxMove.x)+fabs(vxMove.y)+fabs(vxMove.z)>EPS2)
					moveRodSet.append(pRod);
			}
		}
		nodeSet.Empty();
		rodSet.Empty();
		for(pNode=moveNodeSet.GetFirst();pNode;pNode=moveNodeSet.GetNext())
			nodeSet.append(pNode);
		for(pRod=moveRodSet.GetFirst();pRod;pRod=moveRodSet.GetNext())
			rodSet.append(pRod);
		if(moveNodeSet.Count==0&&moveRodSet.Count==0)
			break;	//杆件节点位置已收敛
	}
#ifdef __TIMER_COUNT_
	dwStartTick=timer.Relay(9,dwStartTick);
#endif
	RedrawAll();
#ifdef __TIMER_COUNT_
	timer.Relay(11,dwStartTick);
	timer.End();
	for(DWORD* pdwTickCount=timer.hashTicks.GetFirst();pdwTickCount;pdwTickCount=timer.hashTicks.GetNext())
	{
		UINT uiTickIdentity=timer.hashTicks.GetCursorKey();
		logerr.Log("timer#%2d=%.3f\n",uiTickIdentity,(*pdwTickCount)*0.001);
	}
	logerr.Log("Summary time cost =%.3f\n",(timer.GetEndTicks()-timer.GetStartTicks())*0.001);
#endif
	if(!logerr.IsHasContents())
		MessageBox("命令执行成功");
	if(!PRODUCT_FUNC::IsHasInternalTest())
		logerr.ClearContents();
	//刷新左侧视图区激活当前活动视图
	CTowerTreeDlg *pTreeView = (CTowerTreeDlg*)((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage();
	pTreeView->RefreshTreeCtrl();
}
void CLDSView::OnIntelliRelayoutRodModel()
{
	RelayoutRodModel();
}
DWORD _LocalPlateHashFunc(const char *key, DWORD nHashSize)	//根据输入的关键字key计算相应的Hash映像值
{
	const char* pIterChar=key;
	int hits=0,count=0;
	while(*pIterChar!=NULL&&count<100)	//count<100防错处理
	{
		if(*pIterChar=='-')
			hits++;
		if(hits==3)
			break;	//后续部分不参预哈希计算
		pIterChar++;
		count++;
	}
	//传统字符串快速哈希算法
	CXhChar100 keycopy;
	keycopy.NCopy(key,count);
	ULONG hashkey=0,keylen=count;//(int)strlen(keycopy);
	//key为空时默认以"CHashStrTable:NULL-KEY"作为键值,
	//否则清空链表，再次用空字符串做键值查询时返回无效值 wht 16-12-05
	char sNullKey[]="CHashStrTable:NULL-KEY";
	const char *sRealKey=keylen>0?key:sNullKey;
	if(keylen<=0)
		keylen=(int)strlen(sRealKey);
	union KEY_VALUE
	{
		BYTE key[4]; //0.A1 1.A2 2.A3 3.A4
		DWORD dwKey; //A4A3A2A1 从高到低
	}key_value,value;
	memset(key_value.key,0,sizeof(BYTE)*4);
	for(ULONG i=0;i<keylen;i++)
	{
		ULONG nRemainder=i%3;
		if(nRemainder==0)
			memset(value.key,0,sizeof(BYTE)*4);
		value.key[nRemainder]=sRealKey[i];
		if(nRemainder==2||i==keylen-1)
			key_value.dwKey+=value.dwKey;
	}
	key_value.key[3]=(BYTE)keylen;
	hashkey=key_value.dwKey;
	return hashkey%nHashSize;
	//if(_siBodyOrLegNo==0)
	//	return CXhChar50("%X-%X-%C", hDatumAngle, hNode, cAngleWorkWing);
	//else if(_siBodyOrLegNo>0)
	//	return CXhChar50("%X-%X-%C-L%d", hDatumAngle, hNode, cAngleWorkWing, _siBodyOrLegNo);
	//else
	//	return CXhChar50("%X-%X-%C-B%d", hDatumAngle, hNode, cAngleWorkWing,-_siBodyOrLegNo);
	return 0;
}
bool  _LocalPlateKeyMatchFunc(const char* key, const char* szPartialMatchWithKey)	//判断key是否与withKey匹配，个别情况withKey更精准，允许key与withKey部分匹配
{
	const char* pIterChar=key,*pIterCharWith=szPartialMatchWithKey;
	int hits=0,count=0;
	while(*pIterChar!=NULL&&*pIterCharWith!=NULL&&count<100)	//count<100防错处理
	{
		if(*pIterChar!=*pIterCharWith)
			return false;
		pIterChar++;
		pIterCharWith++;
		count++;
		if(*pIterChar==0)
			return true;	//已经验证key为szPartialMatchWithKey的前半部分
		if(*pIterCharWith==0)
			return false;	//szPartialMatchWithKey为key的一部分
	}
	return false;
}
//#ifndef __TMP_LOG_PROC_
//#define __TMP_LOG_PROC_
//#endif

void CLDSView::OnUpdateBaseFootInfo()
{
	BASEFOOT basefoot;
	basefoot.CopyFromFoundationParams(&Ta.m_xFoundation);
	CTowerMultiLegDesigner designer(&Ta);
	designer.AddOrUpdateBaseFoot(&basefoot,true);	//pFoot
	this->RedrawAll();
}

void ExecuteEmendBoltL0Para(DWORD dwParam=0); //defined in DataCenter.cpp
void CLDSView::OnGenerateRough3DModel()
{
	CRapid3dModelDlg modelDlg;
	if(modelDlg.DoModal()!=IDOK)
		return;
#ifdef __TIMER_COUNT_
	DWORD dwPrevTicks=timer.Start();
#endif
	static CIntelliModelDesigner intelli;
	CLogErrorLife loglife(&logerr);
	intelli.RetrieveModelLegacyConnections(&Ta);
#ifdef __TIMER_COUNT_
	/*CUnifyModelLayer::JOINTINFO* pJointInfo;
	int i = 1;
	for (pJointInfo = intelli.hashJoints.GetFirst(); pJointInfo; pJointInfo = intelli.hashJoints.GetNext(),i++)
	{
		logerr.Log("Joint %3d#0x%X-0x%X", i,pJointInfo->hCoupleAngle, pJointInfo->hMainAngle);
	}*/
	dwPrevTicks=timer.Relay(1);
#endif
	intelli.m_bEnablePromptUI = false;
	intelli.hashPlates.SetHashFunc(_LocalPlateHashFunc);
	intelli.hashPlates.SetKeyMatchFunc(_LocalPlateKeyMatchFunc);
	//1.设计多呼高多接腿的连接
	//1.1设计多呼高多接腿的塔脚板
	CUndoOperObject undo(&Ta, true);
	CTowerMultiLegDesigner designer;
	CStackVariant<char> stackprop0(&CLDSBolt::BOLTL0_CALMODE, CLDSBolt::L0CAL_BY_SUMM_THICK);
	CStackVariant<char> stackprop0i(&CLDSBolt::BOLTL0_PREFER_MODE,CLDSBolt::L0CAL_INC_GAP_THICK);
	CStackVariant<bool> stackprop1(&UI::blEnableIntermediateUpdateUI, false);
	CStackVariant<bool> stackprop1a(&UI::blDisableIntermediateMessageBox,true);
	designer.DesignAllSubLegs(&Ta, 'B',&intelli);
#ifdef __TIMER_COUNT_
	dwPrevTicks=timer.Relay(2, dwPrevTicks);
#endif
	//1.2设计多呼高多接腿的腿身连接板
	designer.DesignAllSubLegs(&Ta, '1', &intelli);
#ifdef __TIMER_COUNT_
	dwPrevTicks=timer.Relay(3, dwPrevTicks);
#endif
	//1.3设计多呼高多接腿的横材中间八字板
	designer.DesignAllSubLegs(&Ta, '2', &intelli);
#ifdef __TIMER_COUNT_
	dwPrevTicks=timer.Relay(4);
#endif
	//1.4设计多呼高多接腿的V面板
	designer.DesignAllSubLegs(&Ta, 'V', &intelli);
#ifdef __TIMER_COUNT_
	dwPrevTicks=timer.Relay(5,dwPrevTicks);
#endif
	//2.设计多呼高塔身
	designer.DesignTrunkPostShiftJoints(&Ta, &intelli);
#ifdef __TIMER_COUNT_
	dwPrevTicks = timer.Relay(6);
#endif
	//2.1设计塔身K点连接板
	//2.2设计塔身横担连接板
	//2.3设计塔身变坡及塔头瓶口连接板
	//2.4设计辅材端点连接
	//2.5设计塔身水平横材中间支撑板连接
	//2.6设计塔身交叉点连接
	//2.7设计塔身横隔角连接
	//3.设计塔头横担
#ifdef __TMP_LOG_PROC_
	logerr.Log("共用段接身换接点设计完毕");
#endif
	intelli.DesignAllNodeConnections(true);
	//2.1设计四角横隔面
	CDesignQuadSection quadsects;
	quadsects.DesignQuadSects(&Ta,true,&intelli);
#ifdef __TIMER_COUNT_
	dwPrevTicks=timer.Relay(7, dwPrevTicks);
#endif
	//3.1智能重置杆件方位（修正头部展开工作面图层名，重置（含火曲面）杆件方位）
	//3.2智能连接设计（含接头、变坡口、对接点双面板、普通以角钢为火曲基准线的双面板、单面板及单螺栓连接设计、塔脚设计）
	//工作面类型：塔身坡面;横隔面;V面;
	//4.统一螺栓朝向及修正螺栓通厚及穿插位置
	stackprop0.PopStack();
	CStackVariant<bool> stackprop2(&CLDSBolt::m_bEnableActiveHeightFilterOnL0InternalCal, false);
	CStackVariant<char> stackprop3(&CLDSBolt::BOLTL0_CALMODE, CLDSBolt::L0CAL_INC_GAP_THICK);
	//需要调用ExecuteEmendBoltL0Para(),否则单螺栓CorrectBoltZnL0Para()速度太慢 wjh-2019.7.15
	ExecuteEmendBoltL0Para();
	for(CLDSBolt* pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
	{
		CLdsPartListStack stackparts(&Ta);
		//pBolt->CorrectBoltZnL0Para();
		if(IsNeedReverseBoltDirection(pBolt))
		{
			for(CLDSModule* pHeight=Ta.Module.GetFirst();pHeight;pHeight=Ta.Module.GetNext())
			{
				if(!pHeight->IsSonPart(pBolt))
					continue;
				ReverseBoltDirection(pBolt,pHeight);
				break;	//找到任一包含当前螺栓的呼高即何反向螺栓
			}
		}
	}
#ifdef __TIMER_COUNT_
	timer.Relay(8,dwPrevTicks);
	timer.End();
	if (PRODUCT_FUNC::IsHasInternalTest())
	{
		for (DWORD* pdwTickCount = timer.hashTicks.GetFirst(); pdwTickCount; pdwTickCount = timer.hashTicks.GetNext())
		{
			UINT uiTickIdentity = timer.hashTicks.GetCursorKey();
			logerr.Log("timer#%2d=%.3f\n", uiTickIdentity, (*pdwTickCount)*0.001);
		}
		logerr.Log("Summary time cost =%.3f\n", (timer.GetEndTicks() - timer.GetStartTicks())*0.001);
	}
#endif

}
#endif
