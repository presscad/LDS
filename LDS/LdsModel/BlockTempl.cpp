//<LOCALE_TRANSLATE BY hxr /> 2015-5-12
#include "stdafx.h"
#include "Tower.h"	//主要是为了借用类别枚举值，如CLS_LINEANGLE
#include ".\block.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "BlockTempl.h"
static int SplitSizeSpecStr(const char* sizespec,CXhChar16* parrItemStr, int max_items_count=3)
{
	CXhChar200 sizestr(sizespec);
	char* pszIter=(char*)sizestr;
	char* prevtoken=pszIter-1,*iter=pszIter;
	//const char* typestr="L-[Φφ";
	WORD wKeyTube =KEY2C("Φ");
	WORD wKeyTube2=KEY2C("φ");
	WORD wType=0,index=0;
	if(*iter=='L'||*iter=='-'||*iter=='[')
	{
		wType=*iter;
		prevtoken=iter;
		if(parrItemStr!=NULL&&max_items_count>0)
			parrItemStr[0].Printf("%C",*iter);
		index++;
		pszIter++;
	}
	else if(*iter<0)
	{
		KEY2C wKey(iter);
		if(wKey==wKeyTube||wKey==wKeyTube2)
			wType=wKeyTube;	//钢管
		if(parrItemStr!=NULL&&max_items_count>0)
			parrItemStr[0].Copy("Φ");
		pszIter+=2;
		index++;
		iter++;
		prevtoken=iter;
	}
	else
		iter-=1;

	CXhChar16 itemstr;
	do{
		iter++;
		if(*iter=='X'||*iter=='x'||*iter=='*'||*iter==0)
		{
			int nitemstr=iter-(prevtoken+1);
			itemstr.NCopy(&pszIter,nitemstr,true);
			if(index<max_items_count&&parrItemStr!=NULL)
				parrItemStr[index]=itemstr;
			if(*pszIter>0)
				pszIter++;
			else
				pszIter+=2;
			prevtoken=iter;
			index++;
		}
	}while(*iter!=0&&iter-(char*)sizestr<200);
	return index;
}
/*void Test_SplitSizeSpecStr()
{
	CXhChar16 items[4];
	int n=SplitSizeSpecStr("299X7",items,4);
	n=SplitSizeSpecStr("299X7",items,4);
	n=SplitSizeSpecStr("299X7",items,4);
	n=SplitSizeSpecStr("Φ299X7",items,4);
	n=SplitSizeSpecStr("L299X7",items,4);
	n=SplitSizeSpecStr("-180X16",items,4);
	n=SplitSizeSpecStr("-180*16",items,4);
}*/
//GetWidthThickFromStr代替之前的restore_JG_guige函数 wjh-2018.7.18
bool GetWidthThickFromStr(const char* sizestr,double& fWidth,double& fThick)
{
	CXhChar16 items[5];
	if(SplitSizeSpecStr(sizestr,items,5)<2)
		return false;
	fWidth=atof(items[0]);
	fThick=atof(items[1]);
	return true;
}
static KEY4C Key4C(BYTE char0,BYTE char1=0,BYTE char2=0,BYTE char3=0)
{
	KEY4C k4;
	BYTE* pbyte=(BYTE*)&k4;
	pbyte[0]=char0;
	pbyte[1]=char1;
	pbyte[2]=char2;
	pbyte[3]=char3;
	return k4;
}
///////////////////////////////////////////////////////////////////////////
CLDSNode* CSubstationTemplate::InsertNewScaleNode(CParaBlockModel* pBlockModel,double fScale,CLDSNode* pStart,CLDSNode* pEnd)
{
	if(fScale<=0 || fScale>=1)
		return NULL;
	f3dPoint startPt=pStart->Position(false);
	f3dPoint endPt=pEnd->Position(false);
	f3dPoint pos=startPt+(endPt-startPt)*fScale;
	CLDSNode *pNode=pBlockModel->AppendNode();
	pNode->SetPosition(pos);
	pNode->layer(0)='S';
	if(m_iTempType==CParaBlockModelTemplate::CROSSARM_TEMPLATE)
		pNode->layer(0)='T';			//横担属于头部
	pNode->layer(1)='P';
	if(m_iTempType==CParaBlockModelTemplate::SUBSTATION_TEMPLATE)
	{
		if(pos.y>=0)
			pNode->layer(2)='Q';
		else //if(pos.y<0)
			pNode->layer(2)='H';
	}
	else if(pos.x>=0&&pos.y>=0)
		pNode->layer(2)='1';
	else if(pos.x<=0&&pos.y>=0)
		pNode->layer(2)='2';
	else if(pos.x>=0&&pos.y<=0)
		pNode->layer(2)='3';
	else //if(pos.x<=0&&pos.y<=0)
		pNode->layer(2)='4';
	pNode->hFatherPart=pStart->hFatherPart;
	pNode->m_cPosCalType=5;	//比例等分点
	pNode->attach_scale=fScale;
	pNode->arrRelationNode[0]=pStart->handle;
	pNode->arrRelationNode[1]=pEnd->handle;
	return pNode;
}
CLDSLinePart* CSubstationTemplate::InsertNewPole(CParaBlockModel* pBlockModel,CLDSNode* pStart,CLDSNode* pEnd,int iPoleType,
													int layStyle,f3dPoint norm,long hRefRod/*=0*/)
{
	if(pStart==NULL || pEnd==NULL)
		return NULL;
	long hMinNode=min(pStart->handle,pEnd->handle);
	long hMaxNode=max(pStart->handle,pEnd->handle);
	if(hashmapRods.GetValue(hMinNode,hMaxNode)!=NULL)	//已生成
		return *hashmapRods.GetValue(hMinNode,hMaxNode);
	CLDSLinePart* pPole=(CLDSLinePart*)pBlockModel->AppendPart(iPoleType);
	hashmapRods.SetValue(hMinNode,hMaxNode,pPole);
	pPole->pStart=pStart;
	pPole->pEnd=pEnd;
	pPole->SetStart(pStart->Position(false));	
	pPole->SetEnd(pEnd->Position(false));
	pPole->layer(0)='S';
	if(m_iTempType==CParaBlockModelTemplate::CROSSARM_TEMPLATE)
		pPole->layer(0)='T';
	pPole->layer(1)='X';
	if(m_iTempType==CParaBlockModelTemplate::SUBSTATION_TEMPLATE)
	{
		if(pPole->pStart->Layer(2)==pPole->pEnd->Layer(2))
			pPole->layer(2)=pPole->pStart->Layer(2);
		else
			pPole->layer(2)='G';
	}
	else
		pPole->layer(2)=CalPoleQuad(pPole->pStart->Position(false),pPole->pEnd->Position(false));
	CLDSLinePart* pWorkplaneRefRod=NULL;
	if(hRefRod>0X20)
		pWorkplaneRefRod=(CLDSLinePart*)pBlockModel->FromPartHandle(hRefRod);
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
	if(pPole->GetClassTypeId()==CLS_LINEANGLE)
	{
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pPole;
		pAngle->dwLayStyle=layStyle+1;
		if(layStyle==0)
		{
			if(norm.IsZero())
				pAngle->des_norm_x.near_norm.Set(0,-1,0);
			else
				pAngle->des_norm_x.near_norm.Set(-norm.x,-norm.y,-norm.z);
			if(pWorkplaneRefRod)
				pAngle->des_norm_x.hViceJg=pWorkplaneRefRod->handle;
			else if(pAngle->pStart->hFatherPart!=pAngle->handle)
				pAngle->des_norm_x.hViceJg=pAngle->pStart->hFatherPart;
			else if(pAngle->pEnd->hFatherPart!=pAngle->handle)
				pAngle->des_norm_x.hViceJg=pAngle->pEnd->hFatherPart;
			pAngle->des_norm_y.bByOtherWing=TRUE;
			pAngle->desStartPos.wing_x_offset.gStyle=pAngle->desEndPos.wing_x_offset.gStyle=4;
			pAngle->desStartPos.wing_x_offset.offsetDist=pAngle->desEndPos.wing_x_offset.offsetDist=0;
			pAngle->desStartPos.wing_y_offset.gStyle=pAngle->desEndPos.wing_y_offset.gStyle=0;
		}
		else if(layStyle==1)
		{
			if(norm.IsZero())
				pAngle->des_norm_y.near_norm.Set(0,1,0);
			else
				pAngle->des_norm_y.near_norm=norm;
			if(pWorkplaneRefRod)
				pAngle->des_norm_y.hViceJg=pWorkplaneRefRod->handle;
			else if(pAngle->pStart->hFatherPart!=pAngle->handle)
				pAngle->des_norm_y.hViceJg=pAngle->pStart->hFatherPart;
			else if(pAngle->pEnd->hFatherPart!=pAngle->handle)
				pAngle->des_norm_y.hViceJg=pAngle->pEnd->hFatherPart;
			pAngle->des_norm_x.bByOtherWing=TRUE;
			pAngle->desStartPos.wing_y_offset.gStyle=pAngle->desEndPos.wing_y_offset.gStyle=4;
			pAngle->desStartPos.wing_y_offset.offsetDist=pAngle->desEndPos.wing_y_offset.offsetDist=0;
			pAngle->desStartPos.wing_x_offset.gStyle=pAngle->desEndPos.wing_x_offset.gStyle=0;
		}
		else if(layStyle==2)
		{
			if(norm.IsZero())
				pAngle->des_norm_x.near_norm.Set(0,1,0);
			else
				pAngle->des_norm_x.near_norm=norm;
			if(pWorkplaneRefRod)
				pAngle->des_norm_x.hViceJg=pWorkplaneRefRod->handle;
			else if(pAngle->pStart->hFatherPart!=pAngle->handle)
				pAngle->des_norm_x.hViceJg=pAngle->pStart->hFatherPart;
			else if(pAngle->pEnd->hFatherPart!=pAngle->handle)
				pAngle->des_norm_x.hViceJg=pAngle->pEnd->hFatherPart;
			pAngle->des_norm_y.bByOtherWing=TRUE;
			pAngle->desStartPos.wing_x_offset.gStyle=pAngle->desEndPos.wing_x_offset.gStyle=4;
			pAngle->desStartPos.wing_x_offset.offsetDist=pAngle->desEndPos.wing_x_offset.offsetDist=0;
			pAngle->desStartPos.wing_y_offset.gStyle=pAngle->desEndPos.wing_y_offset.gStyle=0;
		}
		else
		{
			if(norm.IsZero())
				pAngle->des_norm_y.near_norm.Set(0,-1,0);
			else
				pAngle->des_norm_y.near_norm.Set(-norm.x,-norm.y,-norm.z);
			if(pWorkplaneRefRod)
				pAngle->des_norm_y.hViceJg=pWorkplaneRefRod->handle;
			else if(pAngle->pStart->hFatherPart!=pAngle->handle)
				pAngle->des_norm_y.hViceJg=pAngle->pStart->hFatherPart;
			else if(pAngle->pEnd->hFatherPart!=pAngle->handle)
				pAngle->des_norm_y.hViceJg=pAngle->pEnd->hFatherPart;
			pAngle->des_norm_x.bByOtherWing=TRUE;
			pAngle->desStartPos.wing_y_offset.gStyle=pAngle->desEndPos.wing_y_offset.gStyle=4;
			pAngle->desStartPos.wing_y_offset.offsetDist=pAngle->desEndPos.wing_y_offset.offsetDist=0;
			pAngle->desStartPos.wing_x_offset.gStyle=pAngle->desEndPos.wing_x_offset.gStyle=0;
		}
	}
	else if(pPole->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube* pTube=(CLDSLineTube*)pPole;
		if(!norm.IsZero())
		{	//根据共面参照点计算钢管的工作面
			pTube->m_iNormCalStyle=2;
			pTube->pickWorkPlane[0]=norm.x;
			pTube->pickWorkPlane[1]=norm.y;
			pTube->pickWorkPlane[2]=norm.z;
			pTube->CalWorkPlaneNorm();
		}
		else if(pWorkplaneRefRod)
			pTube->hWorkNormRefPart=pWorkplaneRefRod->handle;
	}
#endif
	return pPole;
}
const BYTE CSubstationTemplate::STROKE_SHUFLAG=0x01;
const BYTE CSubstationTemplate::STROKE_PIEFLAG=0x02;
const BYTE CSubstationTemplate::STROKE_NAFLAG =0x04;
BYTE CSubstationTemplate::MirLayoutStyle(int ciLayoutStyle){
	return 3-ciLayoutStyle;
}
void CSubstationTemplate::CreateSegmentParts(CParaBlockModel* pBlockModel,CXhChar16 sLayer,KEY4C keyFStart,KEY4C keyFEnd,
										KEY4C keyBStart,KEY4C keyBEnd,KEY4C* pKeyTStart/*=NULL*/,KEY4C* pKeyTEnd/*=NULL*/,
										BYTE cbStrokeFlag/*=0xff*/,bool rightSide/*=false*/)
{
	CParaModelNode* pPreFNode=listParaNodes.GetValue(keyFStart);
	CParaModelNode* pCurFNode=listParaNodes.GetValue(keyFEnd);
	CParaModelNode* pPreBNode=listParaNodes.GetValue(keyBStart);
	CParaModelNode* pCurBNode=listParaNodes.GetValue(keyBEnd);
	if(pCurFNode==NULL||pCurFNode->TagInfo()==NULL ||
		pCurBNode==NULL||pCurBNode->TagInfo()==NULL ||
		pPreFNode==NULL||pPreFNode->TagInfo()==NULL ||
		pPreBNode==NULL||pPreBNode->TagInfo()==NULL)
	{
		pBlockModel->LogError("缺少合理依附节点");
		return;
	}
	DESIGN_PARAM_ITEM *pItem=NULL;
	pItem=pBlockModel->GetParamItem(KEY4C("PLT"));
	int iPoleType=pItem->value.iType;
	if(iPoleType==1)
		iPoleType=CLS_LINEANGLE;
	else
		iPoleType=CLS_LINETUBE;
	CLDSNode* pFSNode=pPreFNode->TagInfo()->pModelNode;
	CLDSNode* pFENode=pCurFNode->TagInfo()->pModelNode;
	CLDSNode* pBSNode=pPreBNode->TagInfo()->pModelNode;
	CLDSNode* pBENode=pCurBNode->TagInfo()->pModelNode;
	CLDSNode* pTSNode=NULL,*pTENode=NULL;
	BOOL bTBeamSeg=FALSE;
	if(pKeyTStart&&pKeyTEnd)
	{
		CParaModelNode* pPreTNode=listParaNodes.GetValue(*pKeyTStart);
		CParaModelNode* pCurTNode=listParaNodes.GetValue(*pKeyTEnd);
		if(pPreTNode==NULL || pPreTNode->TagInfo()==NULL||
			pCurTNode==NULL|| pCurTNode->TagInfo()==NULL)
		{
			pBlockModel->LogError("缺少合理依附节点");
			return;
		}
		pTSNode=pPreTNode->TagInfo()->pModelNode;
		pTENode=pCurTNode->TagInfo()->pModelNode;
		bTBeamSeg=TRUE;
	}
	//根据布局字符串提取等分节间数，生成节点
	CHashStrList<CLDSNode*> keySegmentNodeHash;
	int nSeg=0;
	for(int i=0;i<sLayer.GetLength();i++)
	{
		if(sLayer[i]=='/'||sLayer[i]=='\\'||toupper(sLayer[i])=='X')
			nSeg++;
	}
	nSeg=max(nSeg,1);
	CLDSNode* pNode=NULL;
	for(int i=1;i<nSeg;i++)
	{	
		double fScale=(double(i)/nSeg);
		pNode=InsertNewScaleNode(pBlockModel,fScale,pFSNode,pFENode);
		keySegmentNodeHash.SetValue(CXhChar16("F%d",i),pNode);
		pNode=InsertNewScaleNode(pBlockModel,fScale,pBSNode,pBENode);
		keySegmentNodeHash.SetValue(CXhChar16("B%d",i),pNode);
		if(bTBeamSeg)
		{
			pNode=InsertNewScaleNode(pBlockModel,fScale,pTSNode,pTENode);
			keySegmentNodeHash.SetValue(CXhChar16("T%d",i),pNode);
		}
	}
	keySegmentNodeHash.SetValue(CXhChar16("F%d",nSeg),pFENode);
	keySegmentNodeHash.SetValue(CXhChar16("B%d",nSeg),pBENode);
	keySegmentNodeHash.SetValue(CXhChar16("T%d",nSeg),pTENode);
	//生成节间杆件
	CLDSNodePtr* ppNode;
	int segIndex=0;
	for(int i=0;i<sLayer.GetLength();i++)
	{
		if(sLayer[i]=='/'||toupper(sLayer[i])=='X'||sLayer[i]=='\\')
		{
			ppNode=keySegmentNodeHash.GetValue(CXhChar16("F%d",segIndex+1));
			pFENode=ppNode!=NULL?*ppNode:NULL;
			ppNode=keySegmentNodeHash.GetValue(CXhChar16("B%d",segIndex+1));
			pBENode=ppNode!=NULL?*ppNode:NULL;
			if(bTBeamSeg)
			{
				ppNode=keySegmentNodeHash.GetValue(CXhChar16("T%d",segIndex+1));
				pTENode=ppNode!=NULL?*ppNode:NULL;
			}
			segIndex++;
		}
		else
		{
			pFENode=pFSNode;
			pBENode=pBSNode;
			pTENode=pTSNode;
		}
		CLDSLinePart* pRod=NULL;
		CLDSLineAngle* pJg=NULL;
		CXhChar16 sGuige="63X5";
		double wide=0,thick=0;
		if(bTBeamSeg)
		{
			if(sLayer[i]=='/' && (cbStrokeFlag&STROKE_PIEFLAG))
			{
				pRod=InsertNewPole(pBlockModel,pTENode,pFSNode,iPoleType,0,f3dPoint(0,1,0),pFSNode->hFatherPart);
				pRod->layer(2)='Q';
				GetWidthThickFromStr(sGuige,wide,thick);
				pRod->size_wide = wide;
				pRod->size_thick = thick;
				//
				pRod=InsertNewPole(pBlockModel,pTENode,pBSNode,iPoleType,3,f3dPoint(0,-1,0),pBSNode->hFatherPart);
				pRod->layer(2)='H';
				GetWidthThickFromStr(sGuige,wide,thick);
				pRod->size_wide = wide;
				pRod->size_thick = thick;
			}
			if(sLayer[i]=='\\' && (cbStrokeFlag&STROKE_NAFLAG))
			{	
				pRod=InsertNewPole(pBlockModel,pTSNode,pFENode,iPoleType,3,f3dPoint(0,1,0),pFENode->hFatherPart);
				pRod->layer(2)='Q';
				GetWidthThickFromStr(sGuige,wide,thick);
				pRod->size_wide = wide;
				pRod->size_thick = thick;
				//
				pRod=InsertNewPole(pBlockModel,pTSNode,pBENode,iPoleType,0,f3dPoint(0,-1,0),pBENode->hFatherPart);
				pRod->layer(2)='H';
				GetWidthThickFromStr(sGuige,wide,thick);
				pRod->size_wide = wide;
				pRod->size_thick = thick;
			}
			if(toupper(sLayer[i])=='X' && (cbStrokeFlag&STROKE_NAFLAG))
			{
				pRod=InsertNewPole(pBlockModel,pTENode,pFSNode,iPoleType,rightSide?1:0,f3dPoint(0,1,0),pFSNode->hFatherPart);
				pRod->layer(2)='Q';
				GetWidthThickFromStr(sGuige,wide,thick);
				pRod->size_wide = wide;
				pRod->size_thick = thick;
				//
				pRod=InsertNewPole(pBlockModel,pTENode,pBSNode,iPoleType,rightSide?2:3,f3dPoint(0,-1,0),pBSNode->hFatherPart);
				pRod->layer(2)='H';
				GetWidthThickFromStr(sGuige,wide,thick);
				pRod->size_wide = wide;
				pRod->size_thick = thick;
				//
				pRod=InsertNewPole(pBlockModel,pTSNode,pFENode,iPoleType,rightSide?3:2,f3dPoint(0,1,0),pFENode->hFatherPart);
				pRod->layer(2)='Q';
				GetWidthThickFromStr(sGuige,wide,thick);
				pRod->size_wide = wide;
				pRod->size_thick = thick;
				//
				pRod=InsertNewPole(pBlockModel,pTSNode,pBENode,iPoleType,rightSide?0:1,f3dPoint(0,-1,0),pBENode->hFatherPart);
				pRod->layer(2)='H';
				GetWidthThickFromStr(sGuige,wide,thick);
				pRod->size_wide = wide;
				pRod->size_thick = thick;
			}
			if(sLayer[i]=='|' && (cbStrokeFlag&STROKE_SHUFLAG))
			{
				pRod=InsertNewPole(pBlockModel,pTENode,pFENode,iPoleType,rightSide?0:3,f3dPoint(0,1,0),pFENode->hFatherPart);
				pRod->layer(2)='Q';
				GetWidthThickFromStr(sGuige,wide,thick);
				pRod->size_wide = wide;
				pRod->size_thick = thick;
				//
				pRod=InsertNewPole(pBlockModel,pTENode,pBENode,iPoleType,rightSide?3:0,f3dPoint(0,-1,0),pBENode->hFatherPart);
				pRod->layer(2)='H';
				GetWidthThickFromStr(sGuige,wide,thick);
				pRod->size_wide = wide;
				pRod->size_thick = thick;
			}
		}
		else
		{
			if(sLayer[i]=='/' && (cbStrokeFlag&STROKE_PIEFLAG))
			{
				pRod=InsertNewPole(pBlockModel,pBENode,pFSNode,iPoleType,rightSide?0:3,f3dPoint(0,0,-1));
				pRod->layer(2)='G';
				GetWidthThickFromStr(sGuige,wide,thick);
				pRod->size_wide = wide;
				pRod->size_thick = thick;
			}
			if(sLayer[i]=='\\' && (cbStrokeFlag&STROKE_NAFLAG))
			{	
				pRod=InsertNewPole(pBlockModel,pFENode,pBSNode,iPoleType,rightSide?1:2,f3dPoint(0,0,-1));
				pRod->layer(2)='G';
				GetWidthThickFromStr(sGuige,wide,thick);
				pRod->size_wide = wide;
				pRod->size_thick = thick;
			}
			if(toupper(sLayer[i])=='X' && (cbStrokeFlag&STROKE_NAFLAG))
			{
				if(!rightSide)
					pRod=InsertNewPole(pBlockModel,pBENode,pFSNode,iPoleType,3,f3dPoint(0,0,-1));
				else
					pRod=InsertNewPole(pBlockModel,pFSNode,pBENode,iPoleType,1,f3dPoint(0,0,-1));
				pRod->layer(2)='G';
				GetWidthThickFromStr(sGuige,wide,thick);
				pRod->size_wide = wide;
				pRod->size_thick = thick;
				//
				if(!rightSide)
					pRod=InsertNewPole(pBlockModel,pFENode,pBSNode,iPoleType,2,f3dPoint(0,0,-1));
				else
					pRod=InsertNewPole(pBlockModel,pBSNode,pFENode,iPoleType,0,f3dPoint(0,0,-1));
				pRod->layer(2)='G';
				GetWidthThickFromStr(sGuige,wide,thick);
				pRod->size_wide = wide;
				pRod->size_thick = thick;
			}
			if(sLayer[i]=='|' && (cbStrokeFlag&STROKE_SHUFLAG))
			{	
				pRod=InsertNewPole(pBlockModel,pBENode,pFENode,iPoleType,rightSide?3:0,f3dPoint(0,0,-1));
				pRod->layer(2)='G';
				GetWidthThickFromStr(sGuige,wide,thick);
				pRod->size_wide = wide;
				pRod->size_thick = thick;
			}
		}
		pFSNode=pFENode;
		pBSNode=pBENode;
		pTSNode=pTENode;
	}
}
void CSubstationTemplate::RebuildModel(CParaBlockModel* pBlockModel)	//创建变电构架的几何模型
{
	CParaBlockModelTemplate::RebuildModel(pBlockModel);
	if(pBlockModel->GetParamItem(KEY4C("BTYP"))==NULL)
		return;	//构架梁类型为空，表示仅生成人字住
	DESIGN_PARAM_ITEM *pItem=pBlockModel->GetParamItem(KEY4C("NUM"));	//跨数
	int nNum=(int)pItem->value.fValue;
	TRIBEAM_LABELS labels;
	for(int i=0;i<nNum;i++)
	{
		char key='A'+i;
		//分配节点模板标识
		labels.nodes.CLF=KEY4C(CXhChar16("A%C",key));
		labels.nodes.CLB=KEY4C(CXhChar16("B%C",key));
		labels.nodes.CRF=KEY4C(CXhChar16("C%C",key));
		labels.nodes.CRB=KEY4C(CXhChar16("D%C",key));
		labels.nodes.SLF=KEY4C(CXhChar16("E%C",key));
		labels.nodes.SLB=KEY4C(CXhChar16("F%C",key));
		labels.nodes.SRF=KEY4C(CXhChar16("G%C",key));
		labels.nodes.SRB=KEY4C(CXhChar16("H%C",key));
		labels.nodes.SMF=KEY4C(CXhChar16("I%C",key));
		labels.nodes.SMB=KEY4C(CXhChar16("J%C",key));
		labels.nodes.TL =KEY4C(CXhChar16("K%C",key));
		labels.nodes.TR =KEY4C(CXhChar16("L%C",key));
		labels.nodes.TM =KEY4C(CXhChar16("Q%C",key));
		labels.nodes.clf=KEY4C(CXhChar16("a%C",key));
		labels.nodes.clb=KEY4C(CXhChar16("b%C",key));
		labels.nodes.crf=KEY4C(CXhChar16("c%C",key));
		labels.nodes.crb=KEY4C(CXhChar16("d%C",key));
		labels.nodes.slf=KEY4C(CXhChar16("e%C",key));
		labels.nodes.slb=KEY4C(CXhChar16("f%C",key));
		labels.nodes.srf=KEY4C(CXhChar16("g%C",key));
		labels.nodes.srb=KEY4C(CXhChar16("h%C",key));
		labels.nodes.tl =KEY4C(CXhChar16("k%C",key));
		labels.nodes.tr =KEY4C(CXhChar16("q%C",key));
		//
		ARRAY_LIST<CXhChar16>* pLayArr=NULL;
		CXhChar16 *sLayer=NULL;
		KEY4C keyPreF,keyPreB,keyPreT,keyCurF,keyCurB,keyCurT;
		CHashStrList<KEY4C> keyStrHash;
		int iLStart=0,iRStart=0;
		//左侧曲梁拉花布材
		pLayArr=hashSectionLayouts.GetValue(KEY4C("LCBR"));
		if(pLayArr==NULL)
		{
			pBlockModel->LogError("左侧曲梁缺少布局类型参数");
			return;
		}
		keyPreF=labels.nodes.CLF;
		keyPreB=labels.nodes.CLB;
		for(int j=0;j<pLayArr->GetSize();j++)
		{
			if(j<pLayArr->GetSize()-1)
			{
				keyCurF=KEY4C(labels.nodes.clf.pair.wItem,j+1);
				keyCurB=KEY4C(labels.nodes.clb.pair.wItem,j+1);
			}
			else
			{
				keyCurF=labels.nodes.SLF;
				keyCurB=labels.nodes.SLB;
			}
			sLayer=pLayArr->GetByIndex(j);
			CreateSegmentParts(pBlockModel,CXhChar16(*sLayer),keyPreF,keyCurF,keyPreB,keyCurB);
			//
			keyPreF=keyCurF;
			keyPreB=keyCurB;
			keyStrHash.SetValue(CXhChar16("LF%d",iLStart),keyCurF);
			keyStrHash.SetValue(CXhChar16("LB%d",iLStart),keyCurB);
			iLStart++;
		}
		//直梁左侧拉花布材
		pLayArr=hashSectionLayouts.GetValue(KEY4C("LSBR"));
		if(pLayArr==NULL)
		{
			pBlockModel->LogError("直梁左侧缺少布局类型参数");
			return;
		}
		keyPreF=labels.nodes.SLF;
		keyPreB=labels.nodes.SLB;
		for(int j=0;j<pLayArr->GetSize();j++)
		{
			if(j<pLayArr->GetSize()-1)
			{
				keyCurF=KEY4C(labels.nodes.slf.pair.wItem,j+1);
				keyCurB=KEY4C(labels.nodes.slb.pair.wItem,j+1);
			}
			else
			{
				keyCurF=labels.nodes.SMF;
				keyCurB=labels.nodes.SMB;
			}
			sLayer=pLayArr->GetByIndex(j);
			CreateSegmentParts(pBlockModel,CXhChar16(*sLayer),keyPreF,keyCurF,keyPreB,keyCurB);
			//
			keyPreF=keyCurF;
			keyPreB=keyCurB;
			keyStrHash.SetValue(CXhChar16("LF%d",iLStart),keyCurF);
			keyStrHash.SetValue(CXhChar16("LB%d",iLStart),keyCurB);
			iLStart++;
		}
		keyStrHash.SetValue(CXhChar16("RF%d",iRStart),labels.nodes.SMF);
		keyStrHash.SetValue(CXhChar16("RB%d",iRStart),labels.nodes.SMB);
		iRStart++;
		//直梁右侧拉花布材
		pLayArr=hashSectionLayouts.GetValue(KEY4C("RSBR"));
		if(pLayArr==NULL)
		{
			pBlockModel->LogError("缺直梁右侧少布局类型参数");
			return;
		}
		for(int j=pLayArr->GetSize()-1;j>=0;j--)
		{
			if(j>0)
			{
				keyCurF=KEY4C(labels.nodes.srf.pair.wItem,j);
				keyCurB=KEY4C(labels.nodes.srb.pair.wItem,j);
			}
			else
			{
				keyCurF=labels.nodes.SRF;
				keyCurB=labels.nodes.SRB;
			}
			sLayer=pLayArr->GetByIndex(pLayArr->GetSize()-1-j);
			CreateSegmentParts(pBlockModel,CXhChar16(*sLayer),keyPreF,keyCurF,keyPreB,keyCurB,NULL,NULL,0xff,true);
			//
			keyPreF=keyCurF;
			keyPreB=keyCurB;
			keyStrHash.SetValue(CXhChar16("RF%d",iRStart),keyCurF);
			keyStrHash.SetValue(CXhChar16("RB%d",iRStart),keyCurB);
			iRStart++;
		}
		//右侧曲梁拉花布材
		pLayArr=hashSectionLayouts.GetValue(KEY4C("RCBR"));
		if(pLayArr==NULL)
		{
			pBlockModel->LogError("右侧曲梁缺少布局类型参数");
			return;
		}
		for(int j=pLayArr->GetSize()-1;j>=0;j--)
		{
			if(j>0)
			{
				keyCurF=KEY4C(labels.nodes.crf.pair.wItem,j);
				keyCurB=KEY4C(labels.nodes.crb.pair.wItem,j);
			}
			else
			{
				keyCurF=labels.nodes.CRF;
				keyCurB=labels.nodes.CRB;
			}
			sLayer=pLayArr->GetByIndex(pLayArr->GetSize()-1-j);
			CreateSegmentParts(pBlockModel,CXhChar16(*sLayer),keyPreF,keyCurF,keyPreB,keyCurB,NULL,NULL,0xff,true);
			//
			keyPreF=keyCurF;
			keyPreB=keyCurB;
			keyStrHash.SetValue(CXhChar16("RF%d",iRStart),keyCurF);
			keyStrHash.SetValue(CXhChar16("RB%d",iRStart),keyCurB);
			iRStart++;
		}
		//左侧顶梁拉花布材
		pLayArr=hashSectionLayouts.GetValue(KEY4C("LTBR"));
		if(pLayArr==NULL)
		{
			pBlockModel->LogError("横梁左侧缺少布局类型参数");
			return;
		}
		KEY4C* pLabelKey=NULL;
		keyPreT=labels.nodes.TL;
		//最左侧端节间
		{
			keyPreF=labels.nodes.CLF;
			keyPreB=labels.nodes.CLB;
			if((pLabelKey=keyStrHash.GetValue("LF0"))==NULL)
				continue;
			keyCurF=*pLabelKey;
			if((pLabelKey=keyStrHash.GetValue("LB0"))==NULL)
				continue;
			keyCurB=*pLabelKey;
			CreateSegmentParts(pBlockModel,m_sLeftFrontLayout,keyPreF,keyCurF,keyPreB,keyCurB,&keyPreT,&keyPreT,STROKE_PIEFLAG|STROKE_SHUFLAG);
		}
		for(int j=0;j<pLayArr->GetSize();j++)
		{
			if(j<pLayArr->GetSize()-1)
				keyCurT=KEY4C(labels.nodes.tl.pair.wItem,j+1);
			else
				keyCurT=labels.nodes.TM;
			if((pLabelKey=keyStrHash.GetValue(CXhChar16("LF%d",j)))==NULL)
				continue;
			keyPreF=*pLabelKey;
			if((pLabelKey=keyStrHash.GetValue(CXhChar16("LB%d",j)))==NULL)
				continue;
			keyPreB=*pLabelKey;
			if((pLabelKey=keyStrHash.GetValue(CXhChar16("LF%d",j+1)))==NULL)
				continue;
			keyCurF=*pLabelKey;
			if((pLabelKey=keyStrHash.GetValue(CXhChar16("LB%d",j+1)))==NULL)
				continue;
			keyCurB=*pLabelKey;
			sLayer=pLayArr->GetByIndex(j);
			CreateSegmentParts(pBlockModel,CXhChar16(*sLayer),keyPreF,keyCurF,keyPreB,keyCurB,&keyPreT,&keyCurT);
			keyPreT=keyCurT;
		}
		//右侧顶梁拉花布材
		pLayArr=hashSectionLayouts.GetValue(KEY4C("RTBR"));
		if(pLayArr==NULL)
		{
			pBlockModel->LogError("横梁右侧缺少布局类型参数");
			return;
		}
		keyPreT=labels.nodes.TM;
		for(int j=0;j<pLayArr->GetSize();j++)
		{
			if(j<pLayArr->GetSize()-1)
				keyCurT=KEY4C(labels.nodes.tr.pair.wItem,(WORD)(pLayArr->GetSize()-j-1));
			else
				keyCurT=labels.nodes.TR;
			keyPreF=*keyStrHash.GetValue(CXhChar16("RF%d",j));
			keyPreB=*keyStrHash.GetValue(CXhChar16("RB%d",j));
			keyCurF=*keyStrHash.GetValue(CXhChar16("RF%d",j+1));
			keyCurB=*keyStrHash.GetValue(CXhChar16("RB%d",j+1));
			sLayer=pLayArr->GetByIndex(j);
			CreateSegmentParts(pBlockModel,CXhChar16(*sLayer),keyPreF,keyCurF,keyPreB,keyCurB,&keyPreT,&keyCurT,0xff,true);
			keyPreT=keyCurT;
		}
		//最右侧端节间
		{
			keyPreF=keyCurF;
			keyPreB=keyCurB;
			keyCurF=labels.nodes.CRF;
			keyCurB=labels.nodes.CRB;
			CreateSegmentParts(pBlockModel,m_sRightFrontLayout,keyPreF,keyCurF,keyPreB,keyCurB,&labels.nodes.TR,&labels.nodes.TR,STROKE_NAFLAG|STROKE_SHUFLAG,true);
		}
	}
}
///////////////////////////////////////////////////////////////////////////
//普通单榀变电架构模板
//////////////////////////////////////////////////////////////////////////
static void InitTriBeamSubstationTemp(IParaBlockContext* pContext)
{

}
static void InitSquBeamSubStationTemp(IParaBlockContext* pContext)
{

}
static bool ParseSubstationTemplate(IParaBlockContext* pContext)
{
	CParaModelNode* pParaNode=NULL;
	CParaModelRod* pParaRod=NULL;
	CParaModelParamPlate* pParaPlate=NULL;
	DESIGN_PARAM_ITEM* pBeamTypeItem=NULL,*pPostTypeItem=NULL,*pItem=NULL;
	int nNum=0,post_type=0,beam_type=0;
	pItem=pContext->GetParamItem(KEY4C("NUM"));	//跨数
	nNum=(int)pItem->value.fValue;
	pPostTypeItem=pContext->GetParamItem(KEY4C("PTYP"));	//构架柱类型
	pBeamTypeItem=pContext->GetParamItem(KEY4C("BTYP"));	//构架梁类型
	if(pPostTypeItem==NULL && pBeamTypeItem==NULL)
		return false;
	if(pPostTypeItem)
		post_type=pPostTypeItem->value.iType;
	if(pBeamTypeItem)
		beam_type=pBeamTypeItem->value.iType;
	//梁柱传力横杆
	DESIGN_PARAM_ITEM* pItem_BW=NULL,*pItem_PTW=NULL;
	if(pPostTypeItem&&pBeamTypeItem)
	{
		if(post_type==0)
			pItem_PTW=pContext->GetParamItem(KEY4C("PTW"));
		if(beam_type==0)
			pItem_BW=pContext->GetParamItem(KEY4C("LCBW"));
		else if(beam_type==1)
			pItem_BW=pContext->GetParamItem(KEY4C("BW"));
	}
	CParaModelRod *pTransmitRod=NULL;
	if(pPostTypeItem&&post_type==0)		//人字架
	{
		BOOL bStartSup=FALSE,bEndSup=FALSE;
		pItem=pContext->GetParamItem(KEY4C("PSS"));
		bStartSup=pItem->value.iType;
		pItem=pContext->GetParamItem(KEY4C("PBS"));
		bEndSup=pItem->value.iType;
		HUMANPOST_LABELS labels;
		for(int i=0;i<=nNum;i++)
		{
			char key='A'+i;
			//分配节点模板标识
			labels.nodes.PTF=KEY4C(CXhChar16("P%c1",key));
			labels.nodes.PBF=KEY4C(CXhChar16("P%c2",key));
			labels.nodes.PTB=KEY4C(CXhChar16("P%c3",key));
			labels.nodes.PBB=KEY4C(CXhChar16("P%c4",key));
			labels.nodes.PMF=KEY4C(CXhChar16("P%c5",key));
			labels.nodes.PMB=KEY4C(CXhChar16("P%c6",key));
			labels.nodes.PTL=KEY4C(CXhChar16("P%c7",key));
			labels.nodes.PTMU=KEY4C(CXhChar16("P%c8",key));
			labels.nodes.PTMD=KEY4C(CXhChar16("P%c9",key));
			labels.nodes.PBL=KEY4C(CXhChar16("P%c10",key));
			labels.nodes.PML=KEY4C(CXhChar16("P%c11",key));
			//分配杆件模板标识
			labels.rods.RT=KEY4C(CXhChar16("T%C",key));
			labels.rods.RT0=KEY4C(CXhChar16("T%C0",key));
			labels.rods.RPF=KEY4C(CXhChar16("L%c1",key));
			labels.rods.RPB=KEY4C(CXhChar16("L%c2",key));
			labels.rods.RH0=KEY4C(CXhChar16("L%c3",key));
			labels.rods.RPL=KEY4C(CXhChar16("L%c4",key));
			labels.rods.RHF=KEY4C(CXhChar16("L%c5",key));
			labels.rods.RHB=KEY4C(CXhChar16("L%c6",key));
			//分配钢板模板标识
			labels.plates.BT1=KEY4C(CXhChar16("B%c0",key));
			labels.plates.BT2=KEY4C(CXhChar16("B%c1",key));
			labels.plates.BEL=KEY4C(CXhChar16("B%c2",key));
			labels.plates.BER=KEY4C(CXhChar16("B%c3",key));
			labels.plates.BLY=KEY4C(CXhChar16("B%c4",key));
			labels.plates.BSW=KEY4C(CXhChar16("B%c5",key));
			labels.plates.BSL=KEY4C(CXhChar16("B%c6",key));
			labels.plates.BSR=KEY4C(CXhChar16("B%c7",key));
			labels.plates.BMF=KEY4C(CXhChar16("B%c8",key));
			labels.plates.BMR=KEY4C(CXhChar16("B%c9",key));
			labels.plates.BML=KEY4C(CXhChar16("B%c10",key));
			//节点列表之前侧顶部节点
			if(pBeamTypeItem && pItem_PTW->value.fValue>pItem_BW->value.fValue)
			{	//人字架顶部宽度大于横梁的梁柱衔接口时，杆件以人字架顶部开口节点为端节点
				pTransmitRod=pContext->AddParaRod(labels.rods.RT);
			}
			else
				pTransmitRod=NULL;
			//顶柱上节点
			pParaNode=pContext->AddParaNode(labels.nodes.PTMU);
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN",i);
			strcpy(pParaNode->posy_expression,"0");
			strcpy(pParaNode->posz_expression,"-200");
			//顶柱下节点
			pParaNode=pContext->AddParaNode(labels.nodes.PTMD);
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN",i);
			strcpy(pParaNode->posy_expression,"0");
			strcpy(pParaNode->posz_expression,"H-PH");
			//添加顶柱
			pParaRod=pContext->AddParaRod(labels.rods.RT0);
			pParaRod->keyStartNode=labels.nodes.PTMU;
			pParaRod->keyEndNode=labels.nodes.PTMD;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			pParaRod->pickWorkPlane.Set(1000,0,0);
			pParaRod->m_iElemType = CParaModelRod::FEMELEM_BEAM;
			strcpy(pParaRod->m_sGuige,"299X6");
			//前侧顶部节点
			pParaNode=pContext->AddParaNode(labels.nodes.PTF);
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN",i);
			strcpy(pParaNode->posy_expression,"PTW/2");
			strcpy(pParaNode->posz_expression,"H-PH");
			if(pTransmitRod)
			{
				pTransmitRod->keyStartNode=labels.nodes.PTF;
				pParaNode->keyLabelFatherRod=pTransmitRod->keyLabel;
			}
			//前侧底部节点
			pParaNode=pContext->AddParaNode(labels.nodes.PBF);	
			pParaNode->keyLabelFatherRod=labels.rods.RPF;
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN",i);
			strcpy(pParaNode->posy_expression,"PBW/2");
			strcpy(pParaNode->posz_expression,"H");
			//后侧顶部节点
			pParaNode=pContext->AddParaNode(labels.nodes.PTB);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;		
			sprintf(pParaNode->posx_expression,"%d*LEN",i);
			strcpy(pParaNode->posy_expression,"-PTW/2");
			strcpy(pParaNode->posz_expression,"H-PH");
			if(pTransmitRod)
			{
				pTransmitRod->keyEndNode=labels.nodes.PTB;
				pParaNode->keyLabelFatherRod=pTransmitRod->keyLabel;
			}
			//后侧底部节点
			pParaNode=pContext->AddParaNode(labels.nodes.PBB);		
			pParaNode->keyLabelFatherRod=labels.rods.RPB;
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN",i);
			strcpy(pParaNode->posy_expression,"-PBW/2");
			strcpy(pParaNode->posz_expression,"H");
			//横连杆前侧节点
			pParaNode=pContext->AddParaNode(labels.nodes.PMF);	
			pParaNode->keyLabelFatherRod=labels.rods.RPF;
			pParaNode->attach_type=CParaModelNode::COORD_Z_SPECIFIED;
			pParaNode->attachNodeArr[0]=labels.nodes.PTF;
			pParaNode->attachNodeArr[1]=labels.nodes.PBF;
			strcpy(pParaNode->posz_expression,"HV");
			//中间横连杆后侧节点
			pParaNode=pContext->AddParaNode(labels.nodes.PMB);	
			pParaNode->keyLabelFatherRod=labels.rods.RPB;
			pParaNode->attach_type=CParaModelNode::COORD_Z_SPECIFIED;
			pParaNode->attachNodeArr[0]=labels.nodes.PTB;
			pParaNode->attachNodeArr[1]=labels.nodes.PBB;
			strcpy(pParaNode->posz_expression,"HV");
			if((i==0 && bStartSup) || (i==nNum && bEndSup))
			{
				//端撑顶部节点
				pParaNode=pContext->AddParaNode(labels.nodes.PTL);	
				pParaNode->keyLabelFatherRod=labels.rods.RT;
				pParaNode->attach_type=CParaModelNode::INDEPENDENT;
				if(i==0)
					strcpy(pParaNode->posx_expression,"-TSW");
				else
					sprintf(pParaNode->posx_expression,"%d*LEN+TSW",nNum);
				strcpy(pParaNode->posz_expression,"H-PH");
				//端撑底部节点
				pParaNode=pContext->AddParaNode(labels.nodes.PBL);	
				pParaNode->keyLabelFatherRod=labels.rods.RPL;
				pParaNode->attach_type=CParaModelNode::INDEPENDENT;
				if(i==0)
					strcpy(pParaNode->posx_expression,"-BSD");
				else
					sprintf(pParaNode->posx_expression,"%d*LEN+BSD",nNum);
				strcpy(pParaNode->posz_expression,"H");
				//端撑中间连杆节点
				pParaNode=pContext->AddParaNode(labels.nodes.PML);	
				pParaNode->keyLabelFatherRod=labels.rods.RPL;
				pParaNode->attach_type=CParaModelNode::COORD_Z_SPECIFIED;
				pParaNode->attachNodeArr[0]=labels.nodes.PTL;
				pParaNode->attachNodeArr[1]=labels.nodes.PBL;
				strcpy(pParaNode->posz_expression,"HV");
			}
			//杆件列表之前侧杆件
			pParaRod=pContext->AddParaRod(labels.rods.RPF);
			pParaRod->keyStartNode=labels.nodes.PTF;
			pParaRod->keyEndNode=labels.nodes.PBF;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			pParaRod->keyRefPole=labels.rods.RPB;
			pParaRod->m_iElemType = CParaModelRod::FEMELEM_BEAM;
			strcpy(pParaRod->m_sGuige,"299X6");
			//后侧杆件
			pParaRod=pContext->AddParaRod(labels.rods.RPB);
			pParaRod->keyStartNode=labels.nodes.PTB;
			pParaRod->keyEndNode=labels.nodes.PBB;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			pParaRod->keyRefPole=labels.rods.RPF;
			pParaRod->m_iElemType = CParaModelRod::FEMELEM_BEAM;
			strcpy(pParaRod->m_sGuige,"299X6");
			//连接杆件
			pParaRod=pContext->AddParaRod(labels.rods.RH0);
			pParaRod->keyStartNode=labels.nodes.PMF;
			pParaRod->keyEndNode=labels.nodes.PMB;
			pParaRod->keyRefPole=labels.rods.RHF;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			strcpy(pParaRod->m_sGuige,"299X6");
			if((i==0&&bStartSup) || (i==nNum&&bEndSup))
			{
				pParaRod=pContext->AddParaRod(labels.rods.RPL);
				pParaRod->keyStartNode=labels.nodes.PTL;
				pParaRod->keyEndNode=labels.nodes.PBL;
				pParaRod->m_idPartClsType=CLS_LINETUBE;
				pParaRod->m_iElemType = CParaModelRod::FEMELEM_BEAM;
				strcpy(pParaRod->m_sGuige,"299X6");
				//
				pParaRod=pContext->AddParaRod(labels.rods.RHF);
				pParaRod->keyStartNode=labels.nodes.PMF;
				pParaRod->keyEndNode=labels.nodes.PML;
				pParaRod->m_idPartClsType=CLS_LINEANGLE;
				strcpy(pParaRod->m_sGuige,"140X12");
				pParaRod->eyeSideNorm.Set(0,0,-1);
				pParaRod->layout_style=(i==0)?3:0;
				pParaRod=pContext->AddParaRod(labels.rods.RHB);
				pParaRod->keyStartNode=labels.nodes.PMB;
				pParaRod->keyEndNode=labels.nodes.PML;
				pParaRod->m_idPartClsType=CLS_LINEANGLE;
				strcpy(pParaRod->m_sGuige,"140X12");
				pParaRod->eyeSideNorm.Set(0,0,-1);
				pParaRod->layout_style=(i==0)?0:3;
			}
			//钢板列表
			if((i==0&&bStartSup) || (i==nNum&&bEndSup))
			{
				//端撑中间连接板
				pParaPlate=pContext->AddParaPlate(labels.plates.BML);
				pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
				pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_D_LINKPLATE;
				pParaPlate->keyDatumNode=labels.nodes.PML;
				pParaPlate->keyDatumPart=labels.rods.RPL;
				pParaPlate->keyAssistPart=labels.rods.RHF;
				pParaPlate->keyAssistPart2=labels.rods.RHB;
				pParaPlate->workNorm.vector.Set(0,0,-1);
				//前侧立柱连接板
				pParaPlate=pContext->AddParaPlate(labels.plates.BMF);
				pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
				pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_L_NODEPLATE;
				pParaPlate->keyDatumNode=labels.nodes.PMF;
				pParaPlate->keyDatumPart=labels.rods.RH0;
				pParaPlate->keyAssistPart=labels.rods.RHF;
				pParaPlate->workNorm.vector.Set(0,0,-1);
				//后侧立柱连接板
				pParaPlate=pContext->AddParaPlate(labels.plates.BMR);
				pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
				pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_L_NODEPLATE;
				pParaPlate->keyDatumNode=labels.nodes.PMB;
				pParaPlate->keyDatumPart=labels.rods.RH0;
				pParaPlate->keyAssistPart=labels.rods.RHB;
				pParaPlate->workNorm.vector.Set(0,0,-1);
				//端撑顶板
				pParaPlate=pContext->AddParaPlate(labels.plates.BT1);
				pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
				pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_D_TOPPLATE;
				pParaPlate->keyDatumNode=labels.nodes.PTL;
				pParaPlate->keyDatumPart=labels.rods.RPL;
				pParaPlate->workNorm.vector.Set(0,0,0);
				pParaPlate->feature=1;			//0.方形 1.圆形
				//端撑X+端板
				pParaPlate=pContext->AddParaPlate(labels.plates.BEL);
				pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
				pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_D_ENDPLATE;
				pParaPlate->keyDatumPart=labels.plates.BT1;
				pParaPlate->workNorm.vector.Set(1,0,0);
				//端撑X-端板
				pParaPlate=pContext->AddParaPlate(labels.plates.BER);
				pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
				pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_D_ENDPLATE;
				pParaPlate->keyDatumPart=labels.plates.BT1;
				pParaPlate->workNorm.vector.Set(-1,0,0);
				//端撑Y通向插板
				pParaPlate=pContext->AddParaPlate(labels.plates.BSW);
				pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
				pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_D_CHAPLATE;
				pParaPlate->keyDatumPart=labels.plates.BT1;
				pParaPlate->workNorm.vector.Set(0,1,1);
				//端撑X+插板
				pParaPlate=pContext->AddParaPlate(labels.plates.BSL);
				pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
				pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_D_CHAPLATE;
				pParaPlate->keyDatumPart=labels.plates.BT1;
				pParaPlate->keyAssistPart=labels.plates.BSW;
				pParaPlate->workNorm.vector.Set(1,0,0);
				//端撑X-插板
				pParaPlate=pContext->AddParaPlate(labels.plates.BSR);
				pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
				pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_D_CHAPLATE;
				pParaPlate->keyDatumPart=labels.plates.BT1;
				pParaPlate->keyAssistPart=labels.plates.BSW;
				pParaPlate->workNorm.vector.Set(-1,0,0);
				//立柱顶板
				pParaPlate=pContext->AddParaPlate(labels.plates.BT2);
				pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
				pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_L_TOPPLATE;
				pParaPlate->keyDatumNode=labels.nodes.PTMD;
				pParaPlate->workNorm.vector.Set(0,0,-1);
				pParaPlate->feature=0;			//0.方形 1.圆形
				//立柱插板
				pParaPlate=pContext->AddParaPlate(labels.plates.BLY);
				pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
				pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_L_JIAPLATE;
				pParaPlate->keyDatumPart=labels.plates.BT2;
				pParaPlate->keyAssistPart=labels.plates.BEL;
			}
			else
			{
				//立柱顶板
				pParaPlate=pContext->AddParaPlate(labels.plates.BT2);
				pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
				pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_L_TOPPLATE;
				pParaPlate->keyDatumNode=labels.nodes.PTMD;
				pParaPlate->workNorm.vector.Set(0,0,-1);
				pParaPlate->feature=0;			//0.方形 1.圆形
				//立柱插板
				pParaPlate=pContext->AddParaPlate(labels.plates.BLY);
				pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
				pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_L_JIAPLATE;
				pParaPlate->keyDatumPart=labels.plates.BT2;
			}
		}
	}
	else
	{

	}
	if(pBeamTypeItem&&beam_type==0)		//三角梁
	{
		pItem=pContext->GetParamItem(KEY4C("PLT"));
		int m_iPoleType=pItem->value.iType;
		if(m_iPoleType==1)
			m_iPoleType=CLS_LINEANGLE;
		else
			m_iPoleType=CLS_LINETUBE;
		int nLCBSection=0,nRCBSection=0,nSBSection=0,nTBSection=0;
		pItem=pContext->GetParamItem(KEY4C("LCBN"));
		nLCBSection=pItem->value.iType;
		pItem=pContext->GetParamItem(KEY4C("RCBN"));
		nRCBSection=pItem->value.iType;
		pItem=pContext->GetParamItem(KEY4C("LSBN"));
		nSBSection=pItem->value.iType;
		pItem=pContext->GetParamItem(KEY4C("LTBN"));
		nTBSection=pItem->value.iType;
		TRIBEAM_LABELS labels;
		for(int i=1;i<=nNum;i++)
		{
			char key='A'+i-1;
			//分配节点模板标识
			labels.nodes.CLF=KEY4C(CXhChar16("A%C",key));//弯口（梁柱衔接口）节点
			labels.nodes.clf=KEY4C(CXhChar16("a%C",key));//弯口（梁柱衔接口）节点
			labels.nodes.CLB=KEY4C(CXhChar16("B%C",key));
			labels.nodes.clb=KEY4C(CXhChar16("b%C",key));//弯口（梁柱衔接口）节点
			labels.nodes.CRF=KEY4C(CXhChar16("C%C",key));
			labels.nodes.crf=KEY4C(CXhChar16("c%C",key));
			labels.nodes.CRB=KEY4C(CXhChar16("D%C",key));
			labels.nodes.crb=KEY4C(CXhChar16("d%C",key));
			labels.nodes.SLF=KEY4C(CXhChar16("E%C",key));//直口节点
			labels.nodes.slf=KEY4C(CXhChar16("e%C",key));//直口节点
			labels.nodes.SLB=KEY4C(CXhChar16("F%C",key));
			labels.nodes.slb=KEY4C(CXhChar16("f%C",key));
			labels.nodes.SRF=KEY4C(CXhChar16("G%C",key));
			labels.nodes.srf=KEY4C(CXhChar16("g%C",key));
			labels.nodes.SRB=KEY4C(CXhChar16("H%C",key));
			labels.nodes.srb=KEY4C(CXhChar16("h%C",key));
			labels.nodes.SMF=KEY4C(CXhChar16("I%C",key));
			labels.nodes.SMB=KEY4C(CXhChar16("J%C",key));
			labels.nodes.TL =KEY4C(CXhChar16("K%C",key));//顶梁节点
			labels.nodes.tl =KEY4C(CXhChar16("k%C",key));//顶梁节点
			labels.nodes.TR =KEY4C(CXhChar16("L%C",key));
			labels.nodes.NLF=KEY4C(CXhChar16("M%C",key));//用于预拱的节点
			labels.nodes.NLB=KEY4C(CXhChar16("N%C",key));
			labels.nodes.NRF=KEY4C(CXhChar16("O%C",key));
			labels.nodes.NRB=KEY4C(CXhChar16("P%C",key));
			//2016-6-5日新增
			labels.nodes.TM =KEY4C(CXhChar16("Q%C",key));//顶梁中间节点
			labels.nodes.tr =KEY4C(CXhChar16("q%C",key));//顶梁右侧节点
			//分配杆件模板标识
			labels.rods.RTL=KEY4C(CXhChar16("T%C1",key));
			labels.rods.RTR=KEY4C(CXhChar16("T%C2",key));
			labels.rods.RBLF=KEY4C(CXhChar16("L%CA",key));	//左弯梁前
			labels.rods.RBLB=KEY4C(CXhChar16("L%CB",key));	//左弯梁后
			labels.rods.LBMF=KEY4C(CXhChar16("LL%CC",key));	//中直梁左前
			labels.rods.LBMB=KEY4C(CXhChar16("LL%CD",key));	//中直梁左后
			labels.rods.RBMF=KEY4C(CXhChar16("RL%CC",key));	//中直梁右前
			labels.rods.RBMB=KEY4C(CXhChar16("RL%CD",key));	//中直梁右后
			labels.rods.RBRF=KEY4C(CXhChar16("L%CE",key));	//右弯梁前
			labels.rods.RBRB=KEY4C(CXhChar16("L%CF",key));	//右弯梁后
			labels.rods.LRBB=KEY4C(CXhChar16("LL%CG",key));	//脊背梁
			labels.rods.RRBB=KEY4C(CXhChar16("RL%CG",key));	//脊背梁
			labels.rods.RTLF=KEY4C(CXhChar16("L%CH",key));
			labels.rods.RTLB=KEY4C(CXhChar16("L%CL",key));
			labels.rods.RTRF=KEY4C(CXhChar16("L%CM",key));
			labels.rods.RTRB=KEY4C(CXhChar16("L%CN",key));
			//分配钢板模板标识
			labels.plates.BTLF=KEY4C(CXhChar16("S%c0",key));
			labels.plates.BTLB=KEY4C(CXhChar16("S%c1",key));
			labels.plates.BTRF=KEY4C(CXhChar16("S%c2",key));
			labels.plates.BTRB=KEY4C(CXhChar16("S%c3",key));
			labels.plates.BPLF=KEY4C(CXhChar16("S%c4",key));
			labels.plates.BPLB=KEY4C(CXhChar16("S%c5",key));
			labels.plates.BPRF=KEY4C(CXhChar16("S%c6",key));
			labels.plates.BPRB=KEY4C(CXhChar16("S%c7",key));
			labels.plates.RPLF=KEY4C(CXhChar16("S%c8",key));
			labels.plates.RPLB=KEY4C(CXhChar16("S%c9",key));
			labels.plates.RPRF=KEY4C(CXhChar16("S%c10",key));
			labels.plates.RPRB=KEY4C(CXhChar16("S%c11",key));
			//if(pItem_PTW->value.fValue<=pItem_BW->value.fValue)
			//{	//人字架顶部宽度小于横梁的梁柱衔接口时，杆件以横梁衔接开口节点为端节点
			//	pTransmitRod=pContext->AddParaRod(labels.rods.RTL);
			//}
			//else
				pTransmitRod=NULL;
			//节点列表之左侧曲梁前侧始端节点
			pParaNode=pContext->AddParaNode(labels.nodes.CLF);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN+DL",i-1);
			strcpy(pParaNode->posy_expression,"LCBW/2");
			pParaNode->sPreferLayer[2]='Q';
			//if(pTransmitRod)
			//{
			//	pTransmitRod->keyStartNode=pParaNode->keyLabel;
			//	pParaNode->keyLabelFatherRod=pTransmitRod->keyLabel;
			//}
			//左前节点---用于预拱的四个节点
			pParaNode=pContext->AddParaNode(labels.nodes.NLF);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN+DL",i-1);
			strcpy(pParaNode->posy_expression,"SBW/2");
			pParaNode->sPreferLayer[2]='Q';
			//左后节点
			pParaNode=pContext->AddParaNode(labels.nodes.NLB);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN+DL",i-1);
			strcpy(pParaNode->posy_expression,"-SBW/2");
			pParaNode->sPreferLayer[2]='H';
			//右前节点
			pParaNode=pContext->AddParaNode(labels.nodes.NRF);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN-DL",i);
			strcpy(pParaNode->posy_expression,"SBW/2");
			pParaNode->sPreferLayer[2]='Q';
			//右后节点
			pParaNode=pContext->AddParaNode(labels.nodes.NRB);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN-DL",i);
			strcpy(pParaNode->posy_expression,"-SBW/2");
			pParaNode->sPreferLayer[2]='H';
			//前侧直梁节间对称节点
			pParaNode=pContext->AddParaNode(labels.nodes.SMF);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN+DL+LCBL+LSBL",i-1);
			strcpy(pParaNode->posy_expression,"SBW/2");
			pParaNode->sPreferLayer[2]='Q';
			//后侧直梁节间对称节点
			pParaNode=pContext->AddParaNode(labels.nodes.SMB);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN+DL+LCBL+LSBL",i-1);
			strcpy(pParaNode->posy_expression,"-SBW/2");
			pParaNode->sPreferLayer[2]='H';
			//左侧曲梁前侧终端节点(前侧直梁始端节点)
			pParaNode=pContext->AddParaNode(labels.nodes.SLF);	
			pParaNode->attach_type=CParaModelNode::COORD_OFFSET;
			pParaNode->ciOffsetType='X';
			pParaNode->attachNodeArr[0]=labels.nodes.NLF;
			pParaNode->attachNodeArr[1]=labels.nodes.NLF;
			pParaNode->attachNodeArr[2]=labels.nodes.SMF;
			strcpy(pParaNode->offset_expression,"LCBL");
			pParaNode->sPreferLayer[2]='Q';
			//左侧曲梁后侧始端节点
			pParaNode=pContext->AddParaNode(labels.nodes.CLB);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN+DL",i-1);
			strcpy(pParaNode->posy_expression,"-LCBW/2");
			pParaNode->sPreferLayer[2]='H';
			//if(pTransmitRod)
			//{
			//	pTransmitRod->keyEndNode=pParaNode->keyLabel;
			//	pParaNode->keyLabelFatherRod=pTransmitRod->keyLabel;
			//}
			//左侧曲梁后侧终端节点(后侧直梁始端节点)
			pParaNode=pContext->AddParaNode(labels.nodes.SLB);	
			pParaNode->attach_type=CParaModelNode::COORD_OFFSET;
			pParaNode->ciOffsetType='X';
			pParaNode->attachNodeArr[0]=labels.nodes.NLB;
			pParaNode->attachNodeArr[1]=labels.nodes.NLB;
			pParaNode->attachNodeArr[2]=labels.nodes.SMB;
			strcpy(pParaNode->offset_expression,"LCBL");
			pParaNode->sPreferLayer[2]='H';
			//右侧曲梁前侧始端节点(前侧直梁终端节点)
			pParaNode=pContext->AddParaNode(labels.nodes.SRF);		
			pParaNode->attach_type=CParaModelNode::COORD_OFFSET;
			pParaNode->ciOffsetType='X';
			pParaNode->attachNodeArr[0]=labels.nodes.NRF;
			pParaNode->attachNodeArr[1]=labels.nodes.NRF;
			pParaNode->attachNodeArr[2]=labels.nodes.SMF;
			strcpy(pParaNode->offset_expression,"RCBL");
			pParaNode->sPreferLayer[2]='Q';
			//生成右侧梁柱传导杆件
			//if(pItem_PTW->value.fValue<=pItem_BW->value.fValue && i==nNum)
			//{	//人字架顶部宽度小于横梁的梁柱衔接口时，杆件以横梁衔接开口节点为端节点
			//	pTransmitRod=pContext->AddParaRod(labels.rods.RTR);
			//}
			//else
				pTransmitRod=NULL;
			//右侧曲梁前侧终端节点
			pParaNode=pContext->AddParaNode(labels.nodes.CRF);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN-DL",i);
			strcpy(pParaNode->posy_expression,"RCBW/2");
			pParaNode->sPreferLayer[2]='Q';
			if(pTransmitRod)
				pTransmitRod->keyStartNode=pParaNode->keyLabel;
			//右侧曲梁后侧始端节点(后侧直梁终端节点)
			pParaNode=pContext->AddParaNode(labels.nodes.SRB);	
			pParaNode->attach_type=CParaModelNode::COORD_OFFSET;
			pParaNode->ciOffsetType='X';
			pParaNode->attachNodeArr[0]=labels.nodes.NRB;
			pParaNode->attachNodeArr[1]=labels.nodes.NRB;
			pParaNode->attachNodeArr[2]=labels.nodes.SMB;
			strcpy(pParaNode->offset_expression,"RCBL");
			pParaNode->sPreferLayer[2]='H';
			//右侧曲梁后侧终端节点
			pParaNode=pContext->AddParaNode(labels.nodes.CRB);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN-DL",i);
			strcpy(pParaNode->posy_expression,"-RCBW/2");
			pParaNode->sPreferLayer[2]='H';
			if(pTransmitRod)
				pTransmitRod->keyEndNode=pParaNode->keyLabel;
			if(nLCBSection>0)
			{	//顶梁始端节点
				pParaNode=pContext->AddParaNode(labels.nodes.TL);	
				pParaNode->attach_type=CParaModelNode::INDEPENDENT;
				sprintf(pParaNode->posx_expression,"%d*LEN+DL+LCBF[0]",i-1);
				strcpy(pParaNode->posz_expression,"-TBH");
				pParaNode->sPreferLayer[2]='1';
				//顶梁中间节点
				pParaNode=pContext->AddParaNode(labels.nodes.TM);	
				pParaNode->attach_type=CParaModelNode::INDEPENDENT;
				sprintf(pParaNode->posx_expression,"%d*LEN+DL+LCBL+LSBL",i-1);
				strcpy(pParaNode->posz_expression,"-TBH");
				pParaNode->sPreferLayer[2]='1';
				//顶梁终端节点
				pParaNode=pContext->AddParaNode(labels.nodes.TR);	
				pParaNode->attach_type=CParaModelNode::INDEPENDENT;
				sprintf(pParaNode->posx_expression,"%d*LEN-DL-RCBF[RCBN-1]",i);
				strcpy(pParaNode->posz_expression,"-TBH");
				pParaNode->sPreferLayer[2]='1';
			}
			else
			{	//顶梁始端节点
				pParaNode=pContext->AddParaNode(labels.nodes.TL);	
				pParaNode->attach_type=CParaModelNode::INDEPENDENT;
				sprintf(pParaNode->posx_expression,"%d*LEN+DL+SBF[0]",i-1);
				strcpy(pParaNode->posz_expression,"-TBH");
				//顶梁中间节点
				pParaNode=pContext->AddParaNode(labels.nodes.TM);	
				pParaNode->attach_type=CParaModelNode::INDEPENDENT;
				sprintf(pParaNode->posx_expression,"%d*LEN+DL+LSBL",i-1);
				strcpy(pParaNode->posz_expression,"-TBH");
				//顶梁终端节点
				pParaNode=pContext->AddParaNode(labels.nodes.TR);	
				pParaNode->attach_type=CParaModelNode::INDEPENDENT;
				sprintf(pParaNode->posx_expression,"%d*LEN-DL-SBF[0]",i);
				strcpy(pParaNode->posz_expression,"-TBH");

			}
			if(nLCBSection>0)	//左侧曲梁重复节点
			{	
				//左侧曲梁前重复节点
				pParaNode=pContext->AddParaNode(labels.nodes.clf);	
				pParaNode->keyRepeatParam=KEY4C("LCBN");		//重复信息
				pParaNode->attach_type=CParaModelNode::COORD_OFFSET;//SCALE;
				pParaNode->ciOffsetType='X';
				pParaNode->wiOffsetByPrevNode=1;
				pParaNode->attachNodeArr[0]=labels.nodes.CLF;	//索引号0xffff表示前一索引点
				pParaNode->attachNodeArr[1]=labels.nodes.CLF;
				pParaNode->attachNodeArr[2]=labels.nodes.SLF;
				strcpy(pParaNode->offset_expression,"LCBF[i-1]");
				pParaNode->sPreferLayer[2]='Q';
				//左侧曲梁后重复节点
				pParaNode=pContext->AddParaNode(labels.nodes.clb);	
				pParaNode->keyRepeatParam=KEY4C("LCBN");		//重复信息
				pParaNode->attach_type=CParaModelNode::COORD_OFFSET;//SCALE;
				pParaNode->ciOffsetType='X';
				pParaNode->wiOffsetByPrevNode=1;
				pParaNode->attachNodeArr[0]=labels.nodes.CLB;
				pParaNode->attachNodeArr[1]=labels.nodes.CLB;
				pParaNode->attachNodeArr[2]=labels.nodes.SLB;
				strcpy(pParaNode->offset_expression,"LCBF[i-1]");
				pParaNode->sPreferLayer[2]='H';
			}
			if(nSBSection>0)	//直梁重复节点
			{
				//前直梁左侧侧重复节点
				pParaNode=pContext->AddParaNode(labels.nodes.slf);	
				pParaNode->keyRepeatParam=KEY4C("LSBN");		//重复信息
				pParaNode->attach_type=CParaModelNode::COORD_OFFSET;
				pParaNode->wiOffsetByPrevNode=1;
				pParaNode->attachNodeArr[0]=labels.nodes.SLF;
				pParaNode->attachNodeArr[1]=labels.nodes.SLF;
				pParaNode->attachNodeArr[2]=labels.nodes.SMF;
				strcpy(pParaNode->offset_expression,"LSBF[i-1]");//i*0.5*BSL/BSN");//i*(0.5*LEN-BCL)/BSN");
				pParaNode->sPreferLayer[2]='Q';
				//后直梁左侧重复节点
				pParaNode=pContext->AddParaNode(labels.nodes.slb);	
				pParaNode->keyRepeatParam=KEY4C("LSBN");		//重复信息
				pParaNode->attach_type=CParaModelNode::COORD_OFFSET;
				pParaNode->wiOffsetByPrevNode=1;
				pParaNode->attachNodeArr[0]=labels.nodes.SLB;
				pParaNode->attachNodeArr[1]=labels.nodes.SLB;
				pParaNode->attachNodeArr[2]=labels.nodes.SMB;
				strcpy(pParaNode->offset_expression,"LSBF[i-1]");//i*0.5*BSL/BSN");//i*(0.5*LEN-BCL)/BSN");
				pParaNode->sPreferLayer[2]='H';
				//前直梁右侧重复节点
				pParaNode=pContext->AddParaNode(labels.nodes.srf);	
				pParaNode->keyRepeatParam=KEY4C("RSBN");		//重复信息
				pParaNode->attach_type=CParaModelNode::COORD_OFFSET;
				pParaNode->wiOffsetByPrevNode=1;
				pParaNode->attachNodeArr[0]=labels.nodes.SRF;
				pParaNode->attachNodeArr[1]=labels.nodes.SRF;
				pParaNode->attachNodeArr[2]=labels.nodes.SMF;
				strcpy(pParaNode->offset_expression,"RSBF[RSBN-i]");//i*0.5*BSL/BSN");//i*(0.5*LEN-BCR)/BSN");
				pParaNode->sPreferLayer[2]='Q';
				//后直梁右侧重复节点
				pParaNode=pContext->AddParaNode(labels.nodes.srb);	
				pParaNode->keyRepeatParam=KEY4C("RSBN");		//重复信息
				pParaNode->attach_type=CParaModelNode::COORD_OFFSET;
				pParaNode->wiOffsetByPrevNode=1;
				pParaNode->attachNodeArr[0]=labels.nodes.SRB;
				pParaNode->attachNodeArr[1]=labels.nodes.SRB;
				pParaNode->attachNodeArr[2]=labels.nodes.SMB;
				strcpy(pParaNode->offset_expression,"RSBF[RSBN-i]");//i*0.5*BSL/BSN");//"i*(0.5*LEN-BCR)/BSN");
				pParaNode->sPreferLayer[2]='H';
			}
			if(nRCBSection>0)	//右侧曲梁重复节点
			{
				//右侧曲梁前重复节点
				pParaNode=pContext->AddParaNode(labels.nodes.crf);	
				pParaNode->keyRepeatParam=KEY4C("RCBN");		//重复信息
				pParaNode->attach_type=CParaModelNode::COORD_OFFSET;//SCALE;
				pParaNode->ciOffsetType='X';
				pParaNode->wiOffsetByPrevNode=1;
				pParaNode->attachNodeArr[0]=labels.nodes.CRF;
				pParaNode->attachNodeArr[1]=labels.nodes.CRF;
				pParaNode->attachNodeArr[2]=labels.nodes.SRF;
				strcpy(pParaNode->offset_expression,"RCBF[RCBN-i]");//i*0.5*BSL/BSN");//"i*(0.5*LEN-BCR)/BSN");
				pParaNode->sPreferLayer[2]='Q';
				//右侧曲梁后重复节点
				pParaNode=pContext->AddParaNode(labels.nodes.crb);	
				pParaNode->keyRepeatParam=KEY4C("RCBN");		//重复信息
				pParaNode->attach_type=CParaModelNode::COORD_OFFSET;//SCALE;
				pParaNode->ciOffsetType='X';
				pParaNode->wiOffsetByPrevNode=1;
				pParaNode->attachNodeArr[0]=labels.nodes.CRB;
				pParaNode->attachNodeArr[1]=labels.nodes.CRB;
				pParaNode->attachNodeArr[2]=labels.nodes.SRB;
				strcpy(pParaNode->offset_expression,"RCBF[RCBN-i]");//i*0.5*BSL/BSN");//"i*(0.5*LEN-BCR)/BSN");
				pParaNode->sPreferLayer[2]='H';
			}
			if(nTBSection>0)	//顶梁重复节点
			{
				//顶梁左侧重复节点
				pParaNode=pContext->AddParaNode(labels.nodes.tl);	
				pParaNode->keyRepeatParam=KEY4C("LTBN");		//重复信息
				pParaNode->attach_type=CParaModelNode::COORD_OFFSET;//SCALE;
				pParaNode->wiOffsetByPrevNode=1;
				pParaNode->attachNodeArr[0]=labels.nodes.TL;
				pParaNode->attachNodeArr[1]=labels.nodes.TL;
				pParaNode->attachNodeArr[2]=labels.nodes.TM;
				strcpy(pParaNode->offset_expression,"LTBF[i-1]");//i*(LEN-BCL/BCN-BCR/BRN)/BTN");
				pParaNode->sPreferLayer[2]='1';
				//顶梁右侧重复节点
				pParaNode=pContext->AddParaNode(labels.nodes.tr);	
				pParaNode->keyRepeatParam=KEY4C("RTBN");		//重复信息
				pParaNode->attach_type=CParaModelNode::COORD_OFFSET;
				pParaNode->wiOffsetByPrevNode=1;
				pParaNode->attachNodeArr[0]=labels.nodes.TR;
				pParaNode->attachNodeArr[1]=labels.nodes.TR;
				pParaNode->attachNodeArr[2]=labels.nodes.TM;
				strcpy(pParaNode->offset_expression,"RTBF[RTBN-i]");//"i*(LEN-BCL/BCN-BCR/BRN)/BTN");
				pParaNode->sPreferLayer[2]='1';
			}
			//杆件列表之左侧前曲梁杆件
			pParaRod=pContext->AddParaRod(labels.rods.RBLF);		
			pParaRod->keyStartNode=labels.nodes.CLF;
			pParaRod->keyEndNode=labels.nodes.SLF;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			pParaRod->keyRefPole=labels.rods.RBLB;
			pParaRod->sPreferLayer[2]='Q';
			strcpy(pParaRod->m_sGuige,"180X5");
			//左侧后曲梁杆件
			pParaRod=pContext->AddParaRod(labels.rods.RBLB);		
			pParaRod->keyStartNode=labels.nodes.CLB;
			pParaRod->keyEndNode=labels.nodes.SLB;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			pParaRod->keyRefPole=labels.rods.RBLF;
			pParaRod->sPreferLayer[2]='H';
			strcpy(pParaRod->m_sGuige,"180X5");
			//前直梁杆件
			pParaRod=pContext->AddParaRod(labels.rods.LBMF);		
			pParaRod->keyStartNode=labels.nodes.SLF;
			pParaRod->keyEndNode=labels.nodes.SMF;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			pParaRod->keyRefPole=labels.rods.LBMB;
			pParaRod->sPreferLayer[2]='Q';
			strcpy(pParaRod->m_sGuige,"180X5");
			//后直梁杆件
			pParaRod=pContext->AddParaRod(labels.rods.LBMB);		
			pParaRod->keyStartNode=labels.nodes.SLB;
			pParaRod->keyEndNode=labels.nodes.SMB;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			pParaRod->keyRefPole=labels.rods.LBMF;
			pParaRod->sPreferLayer[2]='H';
			strcpy(pParaRod->m_sGuige,"180X5");
			//前直梁杆件
			pParaRod=pContext->AddParaRod(labels.rods.RBMF);		
			pParaRod->keyStartNode=labels.nodes.SRF;
			pParaRod->keyEndNode=labels.nodes.SMF;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			pParaRod->keyRefPole=labels.rods.RBMB;
			pParaRod->sPreferLayer[2]='Q';
			strcpy(pParaRod->m_sGuige,"180X5");
			//后直梁杆件
			pParaRod=pContext->AddParaRod(labels.rods.RBMB);		
			pParaRod->keyStartNode=labels.nodes.SRB;
			pParaRod->keyEndNode=labels.nodes.SMB;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			pParaRod->keyRefPole=labels.rods.RBMF;
			pParaRod->sPreferLayer[2]='H';
			strcpy(pParaRod->m_sGuige,"180X5");
			//右侧前曲梁杆件
			pParaRod=pContext->AddParaRod(labels.rods.RBRF);		
			pParaRod->keyStartNode=labels.nodes.CRF;
			pParaRod->keyEndNode=labels.nodes.SRF;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			pParaRod->keyRefPole=labels.rods.RBRB;
			pParaRod->sPreferLayer[2]='Q';
			strcpy(pParaRod->m_sGuige,"180X5");
			//右侧后曲梁杆件
			pParaRod=pContext->AddParaRod(labels.rods.RBRB);		
			pParaRod->keyStartNode=labels.nodes.CRB;
			pParaRod->keyEndNode=labels.nodes.SRB;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			pParaRod->keyRefPole=labels.rods.RBRF;
			pParaRod->sPreferLayer[2]='H';
			strcpy(pParaRod->m_sGuige,"180X5");
			//左侧顶梁杆件
			pParaRod=pContext->AddParaRod(labels.rods.LRBB);		
			pParaRod->keyStartNode=labels.nodes.TM;
			pParaRod->keyEndNode=labels.nodes.TL;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			pParaRod->pickWorkPlane.Set(0,0,100);	//pickWorkPlane是拾取点不是方向
			pParaRod->sPreferLayer[2]='1';
			strcpy(pParaRod->m_sGuige,"180X5");
			//右侧顶梁杆件
			pParaRod=pContext->AddParaRod(labels.rods.RRBB);		
			pParaRod->keyStartNode=labels.nodes.TM;
			pParaRod->keyEndNode=labels.nodes.TR;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			pParaRod->pickWorkPlane.Set(0,0,100);	//pickWorkPlane是拾取点不是方向
			pParaRod->sPreferLayer[2]='1';
			strcpy(pParaRod->m_sGuige,"180X5");
			/** 这部末端拉杆现已包含在拉花布材类型中，此处代码存在会与拉花布材冲突　wjh-2016.10.6
			//左前侧顶梁拉杆
			pParaRod=pContext->AddParaRod(labels.rods.RTLF,m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.TL;
			pParaRod->keyEndNode=labels.nodes.CLF;
			pParaRod->m_idPartClsType=m_iPoleType;
			pParaRod->eyeSideNorm.Set(0,1,0);
			pParaRod->layout_style=0;	//外铁，吐水(口朝下)
			//左后侧顶梁拉杆
			pParaRod=pContext->AddParaRod(labels.rods.RTLB,m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.TL;
			pParaRod->keyEndNode=labels.nodes.CLB;
			pParaRod->m_idPartClsType=m_iPoleType;
			pParaRod->eyeSideNorm.Set(0,0,-1);
			pParaRod->layout_style=3;	//外铁，吐水(口朝下)
			//右前侧顶梁拉杆
			pParaRod=pContext->AddParaRod(labels.rods.RTRF,m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.TR;
			pParaRod->keyEndNode=labels.nodes.CRF;
			pParaRod->m_idPartClsType=m_iPoleType;
			pParaRod->eyeSideNorm.Set(0,1,0);
			pParaRod->layout_style=3;	//外铁，吐水(口朝下)
			//右后侧顶梁拉杆
			pParaRod=pContext->AddParaRod(labels.rods.RTRB,m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.TR;
			pParaRod->keyEndNode=labels.nodes.CRB;
			pParaRod->m_idPartClsType=m_iPoleType;
			pParaRod->eyeSideNorm.Set(0,0,-1);
			pParaRod->layout_style=0;	//外铁，吐水(口朝下)
			*/
			//梁上钢板列表--左侧曲梁前侧底板
			pParaPlate=pContext->AddParaPlate(labels.plates.BPLF);
			pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
			pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_Q_BASEPLATE;
			pParaPlate->keyDatumNode=labels.nodes.CLF;
			pParaPlate->workNorm.vector.Set(0,0,-1);
			strcpy(pParaPlate->off_expression,"PH-H");
			pParaPlate->feature=0;			//0.方形 1.圆形

			pParaPlate=pContext->AddParaPlate(labels.plates.BTLF);	//
			pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
			pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_Q_TOPPLATE;
			pParaPlate->keyDatumNode=labels.nodes.CLF;
			pParaPlate->keyAssistPart=labels.plates.BPLF;
			pParaPlate->workNorm.vector.Set(-1,0,0);
			pParaPlate->feature=0;			//0.方形 1.圆形
			
			pParaPlate=pContext->AddParaPlate(labels.plates.RPLF);
			pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
			pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_Q_RIBPLATE;
			pParaPlate->keyDatumPart=labels.plates.BPLF;
			pParaPlate->keyAssistPart=labels.plates.BTLF;
			//左侧曲梁后侧底板
			pParaPlate=pContext->AddParaPlate(labels.plates.BPLB);
			pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
			pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_Q_BASEPLATE;
			pParaPlate->keyDatumNode=labels.nodes.CLB;
			pParaPlate->workNorm.vector.Set(0,0,-1);
			strcpy(pParaPlate->off_expression,"PH-H");
			pParaPlate->feature=0;			//0.方形 1.圆形

			pParaPlate=pContext->AddParaPlate(labels.plates.BTLB);
			pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
			pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_Q_TOPPLATE;
			pParaPlate->keyDatumNode=labels.nodes.CLB;
			pParaPlate->keyAssistPart=labels.plates.BPLB;
			pParaPlate->workNorm.vector.Set(-1,0,0);
			pParaPlate->feature=0;			//0.方形 1.圆形
			
			pParaPlate=pContext->AddParaPlate(labels.plates.RPLB);
			pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
			pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_Q_RIBPLATE;
			pParaPlate->keyDatumPart=labels.plates.BPLB;
			pParaPlate->keyAssistPart=labels.plates.BTLB;
			//右侧曲梁前侧底板
			pParaPlate=pContext->AddParaPlate(labels.plates.BPRF);
			pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
			pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_Q_BASEPLATE;
			pParaPlate->keyDatumNode=labels.nodes.CRF;
			pParaPlate->workNorm.vector.Set(0,0,-1);
			strcpy(pParaPlate->off_expression,"PH-H");
			pParaPlate->feature=0;			//0.方形 1.圆形

			pParaPlate=pContext->AddParaPlate(labels.plates.BTRF);
			pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
			pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_Q_TOPPLATE;
			pParaPlate->keyDatumNode=labels.nodes.CRF;
			pParaPlate->keyAssistPart=labels.plates.BPRF;
			pParaPlate->workNorm.vector.Set(1,0,0);
			pParaPlate->feature=0;			//0.方形 1.圆形
			
			pParaPlate=pContext->AddParaPlate(labels.plates.RPRF);
			pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
			pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_Q_RIBPLATE;
			pParaPlate->keyDatumPart=labels.plates.BPRF;
			pParaPlate->keyAssistPart=labels.plates.BTRF;
			//右侧曲梁后侧底板
			pParaPlate=pContext->AddParaPlate(labels.plates.BPRB);
			pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
			pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_Q_BASEPLATE;
			pParaPlate->keyDatumNode=labels.nodes.CRB;
			pParaPlate->workNorm.vector.Set(0,0,-1);
			strcpy(pParaPlate->off_expression,"PH-H");
			pParaPlate->feature=0;			//0.方形 1.圆形

			pParaPlate=pContext->AddParaPlate(labels.plates.BTRB);
			pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
			pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_Q_TOPPLATE;
			pParaPlate->keyDatumNode=labels.nodes.CRB;
			pParaPlate->keyAssistPart=labels.plates.BPRB;
			pParaPlate->workNorm.vector.Set(1,0,0);
			pParaPlate->feature=0;			//0.方形 1.圆形
			
			pParaPlate=pContext->AddParaPlate(labels.plates.RPRB);
			pParaPlate->m_idPartClsType=CLS_PARAMPLATE;
			pParaPlate->m_iParamType=CParaModelParamPlate::TYPE_Q_RIBPLATE;
			pParaPlate->keyDatumPart=labels.plates.BPRB;
			pParaPlate->keyAssistPart=labels.plates.BTRB;
		}
	}
	else if(pBeamTypeItem)		//四方梁
	{
		pItem=pContext->GetParamItem(KEY4C("BRT"));
		int m_iPoleType=pItem->value.iType;
		if(m_iPoleType==1)
			m_iPoleType=CLS_LINEANGLE;
		else
			m_iPoleType=CLS_LINETUBE;
		SQUAREBEAM_LABELS labels;
		for(int i=1;i<=nNum;i++)
		{
			char key='A'+i-1;
			//节点模版标识
			labels.nodes.PLFD=KEY4C(CXhChar16("A%c",key));	//左方梁前侧底部节点
			labels.nodes.PLFU=KEY4C(CXhChar16("B%c",key));	//左方梁前侧顶部节点
			labels.nodes.PLBD=KEY4C(CXhChar16("C%c",key));	//左方梁后侧底部节点
			labels.nodes.PLBU=KEY4C(CXhChar16("D%c",key));	//左方梁后侧顶部节点
			labels.nodes.PRFD=KEY4C(CXhChar16("E%c",key));	//右方梁前侧底部节点
			labels.nodes.PRFU=KEY4C(CXhChar16("F%c",key));	//右方梁前侧顶部节点
			labels.nodes.PRBD=KEY4C(CXhChar16("G%c",key));	//右方梁后侧底部节点
			labels.nodes.PRBU=KEY4C(CXhChar16("H%c",key));	//右方梁后侧顶部节点
			labels.nodes.pfd=KEY4C(CXhChar16("a%c",key));	//前侧底部重复节点
			labels.nodes.pfu=KEY4C(CXhChar16("b%c",key));	//前侧顶部重复节点
			labels.nodes.pbd=KEY4C(CXhChar16("c%c",key));	//后侧底部重复节点
			labels.nodes.pbu=KEY4C(CXhChar16("d%c",key));	//后侧顶部重复节点
			//杆件模版标识
			labels.rods.RFD=KEY4C(CXhChar16("L%cA",key));	//前侧底部杆件
			labels.rods.RFU=KEY4C(CXhChar16("L%cB",key));	//前侧顶部杆件
			labels.rods.RBD=KEY4C(CXhChar16("L%cC",key));	//后侧底部杆件
			labels.rods.RBU=KEY4C(CXhChar16("L%cD",key));	//后侧顶部杆件
			labels.rods.RLD=KEY4C(CXhChar16("L%cE",key));	//方梁左侧传力横杆
			labels.rods.RRD=KEY4C(CXhChar16("L%cF",key));	//方梁右侧传力横杆
			labels.rods.RLU=KEY4C(CXhChar16("L%cG",key));	//左侧顶部横杆
			labels.rods.RRU=KEY4C(CXhChar16("L%cH",key));	//右侧顶部横杆
			labels.rods.RLF=KEY4C(CXhChar16("L%cI",key));	//左侧前拉杆
			labels.rods.RLB=KEY4C(CXhChar16("L%cJ",key));	//左侧后拉杆
			labels.rods.RRF=KEY4C(CXhChar16("L%cK",key));	//右侧前拉杆
			labels.rods.RRB=KEY4C(CXhChar16("L%cL",key));	//右侧后拉杆
			if(pPostTypeItem && pItem_PTW->value.fValue<=pItem_BW->value.fValue)
			{	//人字架顶部宽度小于横梁的梁柱衔接口时，杆件以横梁衔接开口节点为端节点
				pTransmitRod=pContext->AddParaRod(labels.rods.RLD);
			}
			else
				pTransmitRod=NULL;
			//节点列表之左侧方梁底部前节点
			pParaNode=pContext->AddParaNode(labels.nodes.PLFD);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN",i-1);
			strcpy(pParaNode->posy_expression,"BW/2");
			if(pTransmitRod)
			{
				pTransmitRod->keyStartNode=pParaNode->keyLabel;
				pParaNode->keyLabelFatherRod=pTransmitRod->keyLabel;
			}
			//左侧方梁底部后节点
			pParaNode=pContext->AddParaNode(labels.nodes.PLBD);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN",i-1);
			strcpy(pParaNode->posy_expression,"-BW/2");
			if(pTransmitRod)
			{
				pTransmitRod->keyEndNode=pParaNode->keyLabel;
				pParaNode->keyLabelFatherRod=pTransmitRod->keyLabel;
			}
			//左侧方梁顶部前节点
			pParaNode=pContext->AddParaNode(labels.nodes.PLFU);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN",i-1);
			strcpy(pParaNode->posy_expression,"BW/2");
			strcpy(pParaNode->posz_expression,"-BH");
			//左侧方梁顶部后节点
			pParaNode=pContext->AddParaNode(labels.nodes.PLBU);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN",i-1);
			strcpy(pParaNode->posy_expression,"-BW/2");
			strcpy(pParaNode->posz_expression,"-BH");
			//生成右侧传力横杆
			if(pPostTypeItem&& pItem_PTW->value.fValue<=pItem_BW->value.fValue && i==nNum)
			{	//人字架顶部宽度小于横梁的梁柱衔接口时，杆件以横梁衔接开口节点为端节点
				pTransmitRod=pContext->AddParaRod(labels.rods.RRD);
			}
			else
				pTransmitRod=NULL;
			//右侧方梁底部前节点
			pParaNode=pContext->AddParaNode(labels.nodes.PRFD);		
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN+LEN",i-1);
			strcpy(pParaNode->posy_expression,"BW/2");
			if(pTransmitRod)
				pTransmitRod->keyStartNode=pParaNode->keyLabel;
			//右侧方梁底部后节点
			pParaNode=pContext->AddParaNode(labels.nodes.PRBD);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN+LEN",i-1);
			strcpy(pParaNode->posy_expression,"-BW/2");
			if(pTransmitRod)
				pTransmitRod->keyEndNode=pParaNode->keyLabel;
			//右侧方梁顶部前节点
			pParaNode=pContext->AddParaNode(labels.nodes.PRFU);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN+LEN",i-1);
			strcpy(pParaNode->posy_expression,"BW/2");
			strcpy(pParaNode->posz_expression,"-BH");
			//右侧方梁顶部后节点
			pParaNode=pContext->AddParaNode(labels.nodes.PRBU);	
			pParaNode->attach_type=CParaModelNode::INDEPENDENT;
			sprintf(pParaNode->posx_expression,"%d*LEN+LEN",i-1);
			strcpy(pParaNode->posy_expression,"-BW/2");
			strcpy(pParaNode->posz_expression,"-BH");
			//方梁底部前重复节点
			pParaNode=pContext->AddParaNode(labels.nodes.pfd);
			pParaNode->attach_type=CParaModelNode::SCALE;
			pParaNode->keyRepeatParam=KEY4C("BIN");
			pParaNode->attachNodeArr[0]=labels.nodes.PLFD;
			pParaNode->attachNodeArr[1]=labels.nodes.PRFD;
			//方梁底部后重复节点
			pParaNode=pContext->AddParaNode(labels.nodes.pbd);
			pParaNode->attach_type=CParaModelNode::SCALE;
			pParaNode->keyRepeatParam=KEY4C("BIN");
			pParaNode->attachNodeArr[0]=labels.nodes.PLBD;
			pParaNode->attachNodeArr[1]=labels.nodes.PRBD;
			//方梁顶部前重复节点
			pParaNode=pContext->AddParaNode(labels.nodes.pfu);
			pParaNode->attach_type=CParaModelNode::SCALE;
			pParaNode->keyRepeatParam=KEY4C("BIN");
			pParaNode->attachNodeArr[0]=labels.nodes.PLFU;
			pParaNode->attachNodeArr[1]=labels.nodes.PRFU;
			//方梁顶部后重复节点
			pParaNode=pContext->AddParaNode(labels.nodes.pbu);
			pParaNode->attach_type=CParaModelNode::SCALE;
			pParaNode->keyRepeatParam=KEY4C("BIN");
			pParaNode->attachNodeArr[0]=labels.nodes.PLBU;
			pParaNode->attachNodeArr[1]=labels.nodes.PRBU;
			//杆件列表之方梁底部前杆件
			pParaRod=pContext->AddParaRod(labels.rods.RFD);		
			pParaRod->keyStartNode=labels.nodes.PLFD;
			pParaRod->keyEndNode=labels.nodes.PRFD;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			//方梁底部后杆件
			pParaRod=pContext->AddParaRod(labels.rods.RBD);		
			pParaRod->keyStartNode=labels.nodes.PLBD;
			pParaRod->keyEndNode=labels.nodes.PRBD;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			//方梁顶部前杆件
			pParaRod=pContext->AddParaRod(labels.rods.RFU);		
			pParaRod->keyStartNode=labels.nodes.PLFU;
			pParaRod->keyEndNode=labels.nodes.PRFU;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			//方梁顶部后杆件
			pParaRod=pContext->AddParaRod(labels.rods.RBU);		
			pParaRod->keyStartNode=labels.nodes.PLBU;
			pParaRod->keyEndNode=labels.nodes.PRBU;
			pParaRod->m_idPartClsType=CLS_LINETUBE;
			//左侧方梁前拉杆
			pParaRod=pContext->AddParaRod(labels.rods.RLF,m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.PLFD;
			pParaRod->keyEndNode=labels.nodes.PLFU;
			pParaRod->m_idPartClsType=m_iPoleType;
			//左侧方梁后拉杆件
			pParaRod=pContext->AddParaRod(labels.rods.RLB,m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.PLBD;
			pParaRod->keyEndNode=labels.nodes.PLBU;
			pParaRod->m_idPartClsType=m_iPoleType;
			//左侧方梁上横杆
			pParaRod=pContext->AddParaRod(labels.rods.RLU,m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.PLFU;
			pParaRod->keyEndNode=labels.nodes.PLBU;
			pParaRod->m_idPartClsType=m_iPoleType;
			if(i==nNum)
			{
				//右前方梁前拉杆
				pParaRod=pContext->AddParaRod(labels.rods.RRF,m_iPoleType);		
				pParaRod->keyStartNode=labels.nodes.PRFD;
				pParaRod->keyEndNode=labels.nodes.PRFU;
				pParaRod->m_idPartClsType=m_iPoleType;
				//后侧方梁后拉杆
				pParaRod=pContext->AddParaRod(labels.rods.RRB,m_iPoleType);		
				pParaRod->keyStartNode=labels.nodes.PRBD;
				pParaRod->keyEndNode=labels.nodes.PRBU;
				pParaRod->m_idPartClsType=m_iPoleType;
				//右前方梁上横杆
				pParaRod=pContext->AddParaRod(labels.rods.RRU,m_iPoleType);		
				pParaRod->keyStartNode=labels.nodes.PRFU;
				pParaRod->keyEndNode=labels.nodes.PRBU;
				pParaRod->m_idPartClsType=m_iPoleType;
			}
			//方梁底部重复杆件
			pParaRod=pContext->AddParaRod(Key4C(key,0x10,m_iPoleType));		
			pParaRod->keyStartNode=labels.nodes.pfd;
			pParaRod->keyEndNode=labels.nodes.pbd;
			strcpy(pParaRod->indexParaStart,"i");
			strcpy(pParaRod->indexParaEnd,"i+1");
			strcpy(pParaRod->exist_logicexpr,"1>0");
			pParaRod->m_idPartClsType=m_iPoleType;
			pParaRod=pContext->AddParaRod(Key4C(key,0x11),m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.pfd;
			pParaRod->keyEndNode=labels.nodes.pbd;
			strcpy(pParaRod->indexParaStart,"i+1");
			strcpy(pParaRod->indexParaEnd,"i");
			strcpy(pParaRod->exist_logicexpr,"1>0");
			pParaRod->m_idPartClsType=m_iPoleType;
			pParaRod=pContext->AddParaRod(Key4C(key,0x12),m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.pfd;
			pParaRod->keyEndNode=labels.nodes.pbd;
			strcpy(pParaRod->indexParaStart,"i+1");
			strcpy(pParaRod->indexParaEnd,"i+1");
			strcpy(pParaRod->exist_logicexpr,"i+1<BIN");
			pParaRod->m_idPartClsType=m_iPoleType;
			//方梁顶部重复杆件
			pParaRod=pContext->AddParaRod(Key4C(key,0x20),m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.pfu;
			pParaRod->keyEndNode=labels.nodes.pbu;
			strcpy(pParaRod->indexParaStart,"i");
			strcpy(pParaRod->indexParaEnd,"i+1");
			strcpy(pParaRod->exist_logicexpr,"1>0");
			pParaRod->m_idPartClsType=m_iPoleType;
			pParaRod=pContext->AddParaRod(Key4C(key,0x21),m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.pfu;
			pParaRod->keyEndNode=labels.nodes.pbu;
			strcpy(pParaRod->indexParaStart,"i+1");
			strcpy(pParaRod->indexParaEnd,"i");
			strcpy(pParaRod->exist_logicexpr,"1>0");
			pParaRod->m_idPartClsType=m_iPoleType;
			pParaRod=pContext->AddParaRod(Key4C(key,0x22),m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.pfu;
			pParaRod->keyEndNode=labels.nodes.pbu;
			strcpy(pParaRod->indexParaStart,"i+1");
			strcpy(pParaRod->indexParaEnd,"i+1");
			strcpy(pParaRod->exist_logicexpr,"i+1<BIN");
			pParaRod->m_idPartClsType=m_iPoleType;
			//方梁前侧重复杆件
			pParaRod=pContext->AddParaRod(Key4C(key,0x30),m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.pfu;
			pParaRod->keyEndNode=labels.nodes.pfd;
			strcpy(pParaRod->indexParaStart,"i");
			strcpy(pParaRod->indexParaEnd,"i+1");
			strcpy(pParaRod->exist_logicexpr,"1>0");
			pParaRod->m_idPartClsType=m_iPoleType;
			pParaRod=pContext->AddParaRod(Key4C(key,0x31),m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.pfu;
			pParaRod->keyEndNode=labels.nodes.pfd;
			strcpy(pParaRod->indexParaStart,"i+1");
			strcpy(pParaRod->indexParaEnd,"i");
			strcpy(pParaRod->exist_logicexpr,"1>0");
			pParaRod->m_idPartClsType=m_iPoleType;
			pParaRod=pContext->AddParaRod(Key4C(key,0x32),m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.pfu;
			pParaRod->keyEndNode=labels.nodes.pfd;
			strcpy(pParaRod->indexParaStart,"i+1");
			strcpy(pParaRod->indexParaEnd,"i+1");
			strcpy(pParaRod->exist_logicexpr,"i+1<BIN");
			pParaRod->m_idPartClsType=m_iPoleType;
			//方梁后侧重复杆件
			pParaRod=pContext->AddParaRod(Key4C(key,0x40),m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.pbd;
			pParaRod->keyEndNode=labels.nodes.pbu;
			strcpy(pParaRod->indexParaStart,"i");
			strcpy(pParaRod->indexParaEnd,"i+1");
			strcpy(pParaRod->exist_logicexpr,"1>0");
			pParaRod->m_idPartClsType=m_iPoleType;
			pParaRod=pContext->AddParaRod(Key4C(key,0x41),m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.pbd;
			pParaRod->keyEndNode=labels.nodes.pbu;
			strcpy(pParaRod->indexParaStart,"i+1");
			strcpy(pParaRod->indexParaEnd,"i");
			strcpy(pParaRod->exist_logicexpr,"1>0");
			pParaRod->m_idPartClsType=m_iPoleType;
			pParaRod=pContext->AddParaRod(Key4C(key,0x42),m_iPoleType);		
			pParaRod->keyStartNode=labels.nodes.pbd;
			pParaRod->keyEndNode=labels.nodes.pbu;
			strcpy(pParaRod->indexParaStart,"i+1");
			strcpy(pParaRod->indexParaEnd,"i+1");
			strcpy(pParaRod->exist_logicexpr,"i+1<BIN");
			pParaRod->m_idPartClsType=m_iPoleType;
		}
	}
	return true;
}
static bool AssertSubstationParamsValid(IParaBlockContext* pContext)
{
	DESIGN_PARAM_ITEM* pItem;
	bool result=true;
	if((pItem=pContext->GetParamItem(KEY4C("NUM")))==NULL)
	{	//跨数
#ifdef	AFX_TARG_ENU_ENGLISH
		pContext->LogError("Lack parameter 'NUM'");
#else
		pContext->LogError("缺少‘NUM’参数");
#endif
		result = false;
	}
	if((pItem=pContext->GetParamItem(KEY4C("LEN")))==NULL)
	{	//跨度
#ifdef	AFX_TARG_ENU_ENGLISH
		pContext->LogError("Lack parameter 'LEN'");
#else
		pContext->LogError("缺少'LEN'参数");
#endif
		result = false;
	}
	bool hasHumonPost=true;
	if((pItem=pContext->GetParamItem(KEY4C("PTYP")))==NULL)
	{	//构架柱类型 0.人字架
//#ifdef	AFX_TARG_ENU_ENGLISH
//		pContext->LogError("Lack parameter 'PTYP'");
//#else
//		pContext->LogError("缺少'PTYP'参数");
//#endif
		hasHumonPost = false;
	}
	if(hasHumonPost)
	{
		int iPostType=pItem!=NULL?pItem->value.iType:0;
		if(iPostType==0)
		{
			if((pItem=pContext->GetParamItem(KEY4C("PSS")))==NULL)
			{
#ifdef	AFX_TARG_ENU_ENGLISH
				pContext->LogError("Lack parameter 'PSS'");
#else
				pContext->LogError("缺少'PSS'参数");
#endif
				result = false;
			}
			if((pItem=pContext->GetParamItem(KEY4C("PBS")))==NULL)
			{
#ifdef	AFX_TARG_ENU_ENGLISH
				pContext->LogError("Lack parameter 'PBS'");
#else
				pContext->LogError("缺少'PBS'参数");
#endif
				result = false;
			}	
			if((pItem=pContext->GetParamItem(KEY4C("PBW")))==NULL)
			{
#ifdef	AFX_TARG_ENU_ENGLISH
				pContext->LogError("Lack parameter 'PBW'");
#else
				pContext->LogError("缺少'PBW'参数");
#endif
				result = false;
			}
			if((pItem=pContext->GetParamItem(KEY4C("PTW")))==NULL)
			{	//人字架顶部开口
#ifdef	AFX_TARG_ENU_ENGLISH
				pContext->LogError("Lack parameter 'PTW'");
#else
				pContext->LogError("缺少'PTW'参数");
#endif
				result = false;
			}
		}
	}
	bool hasBeam=true;
	if((pItem=pContext->GetParamItem(KEY4C("BTYP")))==NULL)
	{	//构架横梁类型 0.三角梁 1.四方梁 
//#ifdef	AFX_TARG_ENU_ENGLISH
//		pContext->LogError("Lack parameter 'BTYP'");
//#else
//		pContext->LogError("缺少'BTYP'参数");
//#endif
		hasBeam = false;
	}
	if(hasBeam)
	{
		int iBeamType=pItem!=NULL?pItem->value.iType:0;
		if(iBeamType==0)
		{
			if((pItem=pContext->GetParamItem(KEY4C("LCBW")))==NULL)
			{	//三角梁弯口宽度
#ifdef	AFX_TARG_ENU_ENGLISH
				pContext->LogError("Lack parameter 'LCBW'");
#else
				pContext->LogError("缺少'BCW'参数");
#endif
				result = false;
			}
			if((pItem=pContext->GetParamItem(KEY4C("LCBN")))==NULL)
			{	//对称曲梁节间数
#ifdef	AFX_TARG_ENU_ENGLISH
				pContext->LogError("Lack parameter 'LCBN'");
#else
				pContext->LogError("缺少'BCN'参数");
#endif
				result = false;
			}
			if((pItem=pContext->GetParamItem(KEY4C("LSBN")))==NULL)
			{	//对称直梁节间数
#ifdef	AFX_TARG_ENU_ENGLISH
				pContext->LogError("Lack parameter 'LSBN'");
#else
				pContext->LogError("缺少'BSN'参数");
#endif
				result = false;
			}
			if((pItem=pContext->GetParamItem(KEY4C("LTBN")))==NULL)
			{	//顶梁节间数
#ifdef	AFX_TARG_ENU_ENGLISH
				pContext->LogError("Lack parameter 'LTBN'");
#else
				pContext->LogError("缺少'BTN'参数");
#endif
				result = false;
			}
		}
		else if(iBeamType==1)
		{
			if((pItem=pContext->GetParamItem(KEY4C("BW")))==NULL)
			{
#ifdef	AFX_TARG_ENU_ENGLISH
				pContext->LogError("Lack parameter 'BW'");
#else
				pContext->LogError("缺少'BW'参数");
#endif
				result = false;
			}
			if((pItem=pContext->GetParamItem(KEY4C("BH")))==NULL)
			{
#ifdef	AFX_TARG_ENU_ENGLISH
				pContext->LogError("Lack parameter 'BH'");
#else
				pContext->LogError("缺少'BH'参数");
#endif
				result = false;
			}
			if((pItem=pContext->GetParamItem(KEY4C("BIN")))==NULL)
			{
#ifdef	AFX_TARG_ENU_ENGLISH
				pContext->LogError("Lack parameter 'BIN'");
#else
				pContext->LogError("缺少'BIN'参数");
#endif
				result = false;
			}
			if((pItem=pContext->GetParamItem(KEY4C("BRT")))==NULL)
			{
#ifdef	AFX_TARG_ENU_ENGLISH
				pContext->LogError("Lack parameter 'BRT'");
#else
				pContext->LogError("缺少'BRT'参数");
#endif
				result = false;
			}
		}
	}
	return result;
}
//////////////////////////////////////////////////////////////////////////
//标准横担模版
//////////////////////////////////////////////////////////////////////////
static void InitDownTriaCrossArmTemp(IParaBlockContext* pContext)
{
	DESIGN_PARAM_ITEM* pParam=NULL;
	//纵向开口尺寸
	pParam=pContext->AddParaItem(KEY4C("D"));
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=500;
#ifdef	AFX_TARG_ENU_ENGLISH
	pParam->sLabel.Copy("Vertical Opening Size");
	pParam->sNotes.Copy("Vertical opening size of cross arm's bottom plane");
#else
	pParam->sLabel.Copy("纵向开口尺寸");
	pParam->sNotes.Copy("横担下平面纵向开口尺寸");
#endif
	//横向距离
	pParam=pContext->AddParaItem(KEY4C("W"));
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=3000;
#ifdef	AFX_TARG_ENU_ENGLISH
	pParam->sLabel.Copy("Horizontal Distance");
	pParam->sNotes.Copy("Horizontal distance from tower center to hanging point");
#else
	pParam->sLabel.Copy("横向距离");
	pParam->sNotes.Copy("塔中心至挂点的横向距离");
#endif
	//水平节间数
	pParam=pContext->AddParaItem(KEY4C("N"));
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
	pParam->value.iType=3;
#ifdef	AFX_TARG_ENU_ENGLISH
	pParam->sLabel.Copy("Horizontal Internodes");
	pParam->sNotes.Copy("Internodes of cross arm's bottom plane");
#else
	pParam->sLabel.Copy("水平节间数");
	pParam->sNotes.Copy("横担下平面的节间数");
#endif
	//斜向节间数
	pParam=pContext->AddParaItem(KEY4C("M"));
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
	pParam->value.iType=3;
#ifdef	AFX_TARG_ENU_ENGLISH
	pParam->sLabel.Copy("Inclined Internodes");
	pParam->sNotes.Copy("Internodes of cross arm's top inclined plane");
#else
	pParam->sLabel.Copy("斜向节间数");
	pParam->sNotes.Copy("横担上斜面的节间数");
#endif
}
static void InitUpTriaCrossArmTemp(IParaBlockContext* pContext)
{
	DESIGN_PARAM_ITEM* pParam=NULL;
	//纵向开口尺寸
	pParam=pContext->AddParaItem(KEY4C("D"));
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=500;
#ifdef	AFX_TARG_ENU_ENGLISH
	pParam->sLabel.Copy("Vertical Opening Size");
	pParam->sNotes.Copy("Vertical opening size of cross arm's bottom plane");
#else
	pParam->sLabel.Copy("纵向开口尺寸");
	pParam->sNotes.Copy("横担上平面纵向开口尺寸");
#endif
	//横向距离
	pParam=pContext->AddParaItem(KEY4C("W"));
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE;
	pParam->value.fValue=3000;
#ifdef	AFX_TARG_ENU_ENGLISH
	pParam->sLabel.Copy("Horizontal Distance");
	pParam->sNotes.Copy("Horizontal distance from tower center to hanging point");
#else
	pParam->sLabel.Copy("横向距离");
	pParam->sNotes.Copy("塔中心至挂点的横向距离");
#endif
	//水平节间数
	pParam=pContext->AddParaItem(KEY4C("N"));
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
	pParam->value.iType=3;
#ifdef	AFX_TARG_ENU_ENGLISH
	pParam->sLabel.Copy("Horizontal Internodes");
	pParam->sNotes.Copy("Internodes of cross arm's bottom plane");
#else
	pParam->sLabel.Copy("水平节间数");
	pParam->sNotes.Copy("横担上平面的节间数");
#endif
	//斜向节间数
	pParam=pContext->AddParaItem(KEY4C("M"));
	pParam->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
	pParam->value.iType=3;
#ifdef	AFX_TARG_ENU_ENGLISH
	pParam->sLabel.Copy("Inclined Internodes");
	pParam->sNotes.Copy("Internodes of cross arm's top inclined plane");
#else
	pParam->sLabel.Copy("斜向节间数");
	pParam->sNotes.Copy("横担下斜面的节间数");
#endif
}
static bool ParseDownTriaCrossArmTemp(IParaBlockContext* pContext)
{
	CParaModelNode* pParaNode=NULL;
	CParaModelRod* pParaRod=NULL;
	CParaModelAngle* pParaAngle=NULL;
	DESIGN_PARAM_ITEM* pItem=NULL;
	double fLen=0,fWidth=0;
	pItem=pContext->GetParamItem(KEY4C("D"));
	fWidth=pItem->value.fValue;
	pItem=pContext->GetParamItem(KEY4C("W"));
	fLen=pItem->value.fValue;
	//分配节点模板标识
	CROSSARM_LABELS labels;
	labels.nodes.KN1=KEY4C("P1");
	labels.nodes.KN2=KEY4C("P2");
	labels.nodes.KN3=KEY4C("P3");
	labels.nodes.KN4=KEY4C("P4");
	labels.nodes.KNF=KEY4C("P5");
	labels.nodes.KNB=KEY4C("P6");
	labels.nodes.kfd=KEY4C("Pa");
	labels.nodes.kfu=KEY4C("Pb");
	labels.nodes.kbd=KEY4C("Pc");
	labels.nodes.kbu=KEY4C("Pd");
	//分配杆件模板标识
	labels.rods.RFD=KEY4C("L1");
	labels.rods.RFU=KEY4C("L2");
	labels.rods.RBD=KEY4C("L3");
	labels.rods.RBU=KEY4C("L4");
	labels.rods.RW=KEY4C("LT");
	labels.rods.rld=KEY4C("la");
	labels.rods.rlu=KEY4C("lb");
	labels.rods.rlf=KEY4C("lc");
	labels.rods.rlb=KEY4C("ld");
	//节点列表
	//关键点1
	CLDSNode* pKeyNode=pContext->GetKeyNode(1);
	if(pKeyNode==NULL)
	{
#ifdef	AFX_TARG_ENU_ENGLISH
		pContext->LogError("Failed to get key point!");
#else
		pContext->LogError("获取关键点失败!");
#endif
		return false;
	}
	pParaNode=pContext->AddParaNode(labels.nodes.KN1);
	pParaNode->SetTagInfo(pKeyNode);	//将关键节点暂时挂在Tag标签上
	//关键点2
	pKeyNode=pContext->GetKeyNode(2);
	if(pKeyNode==NULL)
	{
#ifdef	AFX_TARG_ENU_ENGLISH
		pContext->LogError("Failed to get key point!");
#else
		pContext->LogError("获取关键点失败!");
#endif
		return false;
	}
	pParaNode=pContext->AddParaNode(labels.nodes.KN2);
	pParaNode->SetTagInfo(pKeyNode);	//将关键节点暂时挂在Tag标签上
	//关键点3
	pKeyNode=pContext->GetKeyNode(3);
	if(pKeyNode==NULL)
	{
#ifdef	AFX_TARG_ENU_ENGLISH
		pContext->LogError("Failed to get key point!");
#else
		pContext->LogError("获取关键点失败!");
#endif
		return false;
	}
	pParaNode=pContext->AddParaNode(labels.nodes.KN3);
	pParaNode->SetTagInfo(pKeyNode);	//将关键节点暂时挂在Tag标签上
	//关键点4
	pKeyNode=pContext->GetKeyNode(4);
	if(pKeyNode==NULL)
	{
#ifdef	AFX_TARG_ENU_ENGLISH
		pContext->LogError("Failed to get key point!");
#else
		pContext->LogError("获取关键点失败!");
#endif
		return false;
	}
	pParaNode=pContext->AddParaNode(labels.nodes.KN4);
	pParaNode->SetTagInfo(pKeyNode);	//将关键节点暂时挂在Tag标签上
	//横担前侧挂点
	pKeyNode=pContext->GetKeyNode(2);
	pParaNode=pContext->AddParaNode(labels.nodes.KNF);
	pParaNode->keyLabelFatherRod=labels.rods.RFD;
	pParaNode->attach_type=CParaModelNode::INDEPENDENT;
	sprintf(pParaNode->posx_expression,"%f",fLen);
	SimplifiedNumString(pParaNode->posx_expression);
	sprintf(pParaNode->posy_expression,"%f",fWidth*0.5);
	SimplifiedNumString(pParaNode->posy_expression);
	sprintf(pParaNode->posz_expression,"%f",pKeyNode->Position().z);
	SimplifiedNumString(pParaNode->posz_expression);
	//横担后侧挂点
	pParaNode=pContext->AddParaNode(labels.nodes.KNB);
	pParaNode->keyLabelFatherRod=labels.rods.RBD;
	pParaNode->attach_type=CParaModelNode::INDEPENDENT;
	sprintf(pParaNode->posx_expression,"%f",fLen);
	SimplifiedNumString(pParaNode->posx_expression);
	sprintf(pParaNode->posy_expression,"%f",-fWidth*0.5);
	SimplifiedNumString(pParaNode->posy_expression);
	sprintf(pParaNode->posz_expression,"%f",pKeyNode->Position().z);
	SimplifiedNumString(pParaNode->posz_expression);
	//横担前下侧重复节点
	pParaNode=pContext->AddParaNode(labels.nodes.kfd);
	pParaNode->keyLabelFatherRod=labels.rods.RFD;
	pParaNode->keyRepeatParam=KEY4C("N");		//重复信息
	pParaNode->attach_type=CParaModelNode::SCALE;
	pParaNode->attachNodeArr[0]=labels.nodes.KN2;
	pParaNode->attachNodeArr[1]=labels.nodes.KNF;
	//横担后下侧重复节点
	pParaNode=pContext->AddParaNode(labels.nodes.kbd);
	pParaNode->keyLabelFatherRod=labels.rods.RBD;
	pParaNode->keyRepeatParam=KEY4C("N");		//重复信息
	pParaNode->attach_type=CParaModelNode::SCALE;
	pParaNode->attachNodeArr[0]=labels.nodes.KN4;
	pParaNode->attachNodeArr[1]=labels.nodes.KNB;
	//横担前上侧重复节点
	pParaNode=pContext->AddParaNode(labels.nodes.kfu);
	pParaNode->keyLabelFatherRod=labels.rods.RFU;
	pParaNode->keyRepeatParam=KEY4C("M");
	pParaNode->attach_type=CParaModelNode::SCALE;
	pParaNode->attachNodeArr[0]=labels.nodes.KN1;
	pParaNode->attachNodeArr[1]=labels.nodes.KNF;
	//横担后上侧重复节点
	pParaNode=pContext->AddParaNode(labels.nodes.kbu);
	pParaNode->keyLabelFatherRod=labels.rods.RBU;
	pParaNode->keyRepeatParam=KEY4C("M");
	pParaNode->attach_type=CParaModelNode::SCALE;
	pParaNode->attachNodeArr[0]=labels.nodes.KN3;
	pParaNode->attachNodeArr[1]=labels.nodes.KNB;
	//杆件列表之前侧水平杆件
	pParaAngle=(CParaModelAngle*)pContext->AddParaRod(labels.rods.RFD,CLS_LINEANGLE);
	pParaAngle->keyStartNode=labels.nodes.KN2;
	pParaAngle->keyEndNode=labels.nodes.KNF;
	pParaAngle->m_idPartClsType=CLS_LINEANGLE;
	pParaAngle->m_iElemType = CParaModelRod::FEMELEM_BEAM;
	pParaAngle->m_bMajorMat=TRUE;
	pParaAngle->layout_style=1;		//里铁喝水	
	pParaAngle->eyeSideNorm.Set(0,1,0);
	pParaAngle->desEnd.cMainCoordCtrlWing='Y';
	pParaAngle->desEnd.coordCtrlType=0;	//X-Z主控
	pParaAngle->desEnd.cTouchLineType=3;	//控制点
	pParaAngle->desEnd.face_offset_norm.Set(1,0,0);
	pParaAngle->desEnd.XOrCtrlWing.operStyle=0;
	pParaAngle->desEnd.YOrOtherWing.operStyle=2;
	//前侧斜向杆件
	pParaRod=pContext->AddParaRod(labels.rods.RFU);
	pParaRod->keyStartNode=labels.nodes.KN1;
	pParaRod->keyEndNode=labels.nodes.KNF;
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->m_iElemType = CParaModelRod::FEMELEM_BEAM;
	pParaRod->m_bMajorMat=TRUE;
	pParaRod->layout_style=2;		//里铁吐水	
	pParaRod->eyeSideNorm.Set(0,1,0);
	//后侧水平杆件
	pParaRod=pContext->AddParaRod(labels.rods.RBD);
	pParaRod->keyStartNode=labels.nodes.KN4;
	pParaRod->keyEndNode=labels.nodes.KNB;
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->m_iElemType = CParaModelRod::FEMELEM_BEAM;
	pParaRod->m_bMajorMat=TRUE;
	pParaRod->layout_style=0;		//外铁喝水	
	pParaRod->eyeSideNorm.Set(0,1,0);
	//后侧斜向杆件
	pParaRod=pContext->AddParaRod(labels.rods.RBU);
	pParaRod->keyStartNode=labels.nodes.KN3;
	pParaRod->keyEndNode=labels.nodes.KNB;
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->m_iElemType = CParaModelRod::FEMELEM_BEAM;
	pParaRod->m_bMajorMat=TRUE;
	pParaRod->layout_style=3;		//外铁吐水	
	pParaRod->eyeSideNorm.Set(0,1,0);
	//传力杆件
	pParaRod=pContext->AddParaRod(labels.rods.RW);
	pParaRod->keyStartNode=labels.nodes.KNF;
	pParaRod->keyEndNode=labels.nodes.KNB;
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->m_iElemType = CParaModelRod::FEMELEM_BEAM;
	pParaRod->layout_style=2;		//里铁吐水	
	pParaRod->eyeSideNorm.Set(1,0,0);
	//水平节间重复杆件
	/*pParaRod=pContext->AddParaRod(Key4C('A',0x01));
	pParaRod->keyStartNode=labels.nodes.kfd;
	pParaRod->keyEndNode=labels.nodes.kbd;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i+1");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=1;*/
	/*pParaRod=pContext->AddParaRod(Key4C('A',0x02));		
	pParaRod->keyStartNode=labels.nodes.kbd;
	pParaRod->keyEndNode=labels.nodes.kfd;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i+1");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=0;*/
	//斜平面节间重复杆件
	/*pParaRod=pContext->AddParaRod(Key4C('B',0x01));		
	pParaRod->keyStartNode=labels.nodes.kfu;
	pParaRod->keyEndNode=labels.nodes.kbu;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i+1");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=2;*/
	/*pParaRod=pContext->AddParaRod(Key4C('B',0x02));
	pParaRod->keyStartNode=labels.nodes.kbu;
	pParaRod->keyEndNode=labels.nodes.kfu;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i+1");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=2;*/
	//前三角面重复杆件
	/*pParaRod=pContext->AddParaRod(Key4C('C',0x01));
	pParaRod->keyStartNode=labels.nodes.kfd;
	pParaRod->keyEndNode=labels.nodes.kfu;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i+1");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=2;*/
	/*pParaRod=pContext->AddParaRod(Key4C('C',0x02));
	pParaRod->keyStartNode=labels.nodes.kfd;
	pParaRod->keyEndNode=labels.nodes.kfu;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=2;*/
	//后三角面重复杆件
	/*pParaRod=pContext->AddParaRod(Key4C('D',0x01));
	pParaRod->keyStartNode=labels.nodes.kbd;
	pParaRod->keyEndNode=labels.nodes.kbu;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i+1");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=2;*/
	/*pParaRod=pContext->AddParaRod(Key4C('D',0x02));
	pParaRod->keyStartNode=labels.nodes.kbd;
	pParaRod->keyEndNode=labels.nodes.kbu;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=2;*/
	return true;
}
static bool ParseUpTriaCrossArmTemp(IParaBlockContext* pContext)
{
	CParaModelNode* pParaNode=NULL;
	CParaModelRod* pParaRod=NULL;
	DESIGN_PARAM_ITEM* pItem=NULL;
	double fLen=0,fWidth=0;
	pItem=pContext->GetParamItem(KEY4C("D"));
	fWidth=pItem->value.fValue;
	pItem=pContext->GetParamItem(KEY4C("W"));
	fLen=pItem->value.fValue;
	//分配节点模板标识
	CROSSARM_LABELS labels;
	labels.nodes.KN1=KEY4C("P1");
	labels.nodes.KN2=KEY4C("P2");
	labels.nodes.KN3=KEY4C("P3");
	labels.nodes.KN4=KEY4C("P4");
	labels.nodes.KNF=KEY4C("P5");
	labels.nodes.KNB=KEY4C("P6");
	labels.nodes.kfd=KEY4C("Pa");
	labels.nodes.kfu=KEY4C("Pb");
	labels.nodes.kbd=KEY4C("Pc");
	labels.nodes.kbu=KEY4C("Pd");
		//分配杆件模板标识
	labels.rods.RFD=KEY4C("L1");
	labels.rods.RFU=KEY4C("L2");
	labels.rods.RBD=KEY4C("L3");
	labels.rods.RBU=KEY4C("L4");
	labels.rods.RW=KEY4C("LT");
	labels.rods.rld=KEY4C("la");
	labels.rods.rlu=KEY4C("lb");
	labels.rods.rlf=KEY4C("lc");
	labels.rods.rlb=KEY4C("ld");
	//节点列表
	//关键点1
	CLDSNode* pKeyNode=pContext->GetKeyNode(1);
	if(pKeyNode==NULL)
	{
#ifdef	AFX_TARG_ENU_ENGLISH
		pContext->LogError("Failed to get key point!");
#else
		pContext->LogError("获取关键点失败!");
#endif
		return false;
	}
	pParaNode=pContext->AddParaNode(labels.nodes.KN1);
	pParaNode->SetTagInfo(pKeyNode);	//将关键节点暂时挂在Tag标签上
	//关键点2
	pKeyNode=pContext->GetKeyNode(2);
	if(pKeyNode==NULL)
	{
#ifdef	AFX_TARG_ENU_ENGLISH
		pContext->LogError("Failed to get key point!");
#else
		pContext->LogError("获取关键点失败!");
#endif
		return false;
	}
	pParaNode=pContext->AddParaNode(labels.nodes.KN2);
	pParaNode->SetTagInfo(pKeyNode);	//将关键节点暂时挂在Tag标签上
	//关键点3
	pKeyNode=pContext->GetKeyNode(3);
	if(pKeyNode==NULL)
	{
#ifdef	AFX_TARG_ENU_ENGLISH
		pContext->LogError("Failed to get key point!");
#else
		pContext->LogError("获取关键点失败!");
#endif
		return false;
	}
	pParaNode=pContext->AddParaNode(labels.nodes.KN3);
	pParaNode->SetTagInfo(pKeyNode);	//将关键节点暂时挂在Tag标签上
	//关键点4
	pKeyNode=pContext->GetKeyNode(4);
	if(pKeyNode==NULL)
	{
#ifdef	AFX_TARG_ENU_ENGLISH
		pContext->LogError("Failed to get key point!");
#else
		pContext->LogError("获取关键点失败!");
#endif
		return false;
	}
	pParaNode=pContext->AddParaNode(labels.nodes.KN1);
	pParaNode->SetTagInfo(pKeyNode);	//将关键节点暂时挂在Tag标签上
	//横担前侧挂点
	pKeyNode=pContext->GetKeyNode(1);
	pParaNode=pContext->AddParaNode(labels.nodes.KNF);
	pParaNode->keyLabelFatherRod=labels.rods.RFU;
	pParaNode->attach_type=CParaModelNode::INDEPENDENT;
	sprintf(pParaNode->posx_expression,"%f",fLen);
	SimplifiedNumString(pParaNode->posx_expression);
	sprintf(pParaNode->posy_expression,"%f",fWidth*0.5);
	SimplifiedNumString(pParaNode->posy_expression);
	sprintf(pParaNode->posz_expression,"%f",pKeyNode->Position().z);
	SimplifiedNumString(pParaNode->posz_expression);
	//横担后侧挂点
	pParaNode=pContext->AddParaNode(labels.nodes.KNB);
	pParaNode->keyLabelFatherRod=labels.rods.RBU;
	pParaNode->attach_type=CParaModelNode::INDEPENDENT;
	sprintf(pParaNode->posx_expression,"%f",fLen);
	SimplifiedNumString(pParaNode->posx_expression);
	sprintf(pParaNode->posy_expression,"%f",-fWidth*0.5);
	SimplifiedNumString(pParaNode->posy_expression);
	sprintf(pParaNode->posz_expression,"%f",pKeyNode->Position().z);
	SimplifiedNumString(pParaNode->posz_expression);
	//横担前下侧重复节点
	pParaNode=pContext->AddParaNode(labels.nodes.kfd);
	pParaNode->keyLabelFatherRod=labels.rods.RFD;
	pParaNode->keyRepeatParam=KEY4C("M");		//重复信息
	pParaNode->attach_type=CParaModelNode::SCALE;
	pParaNode->attachNodeArr[0]=labels.nodes.KN2;
	pParaNode->attachNodeArr[1]=labels.nodes.KNF;
	//横担后下侧重复节点
	pParaNode=pContext->AddParaNode(labels.nodes.kbd);
	pParaNode->keyLabelFatherRod=labels.rods.RBD;
	pParaNode->keyRepeatParam=KEY4C("M");		//重复信息
	pParaNode->attach_type=CParaModelNode::SCALE;
	pParaNode->attachNodeArr[0]=labels.nodes.KN4;
	pParaNode->attachNodeArr[1]=labels.nodes.KNB;
	//横担前上侧重复节点
	pParaNode=pContext->AddParaNode(labels.nodes.kfu);
	pParaNode->keyLabelFatherRod=labels.rods.RFU;
	pParaNode->keyRepeatParam=KEY4C("N");
	pParaNode->attach_type=CParaModelNode::SCALE;
	pParaNode->attachNodeArr[0]=labels.nodes.KN1;
	pParaNode->attachNodeArr[1]=labels.nodes.KNF;
	//横担后上侧重复节点
	pParaNode=pContext->AddParaNode(labels.nodes.kbu);
	pParaNode->keyLabelFatherRod=labels.rods.RBU;
	pParaNode->keyRepeatParam=KEY4C("N");
	pParaNode->attach_type=CParaModelNode::SCALE;
	pParaNode->attachNodeArr[0]=labels.nodes.KN3;
	pParaNode->attachNodeArr[1]=labels.nodes.KNB;
	//杆件列表之前侧斜向杆件
	pParaRod=pContext->AddParaRod(labels.rods.RFD);
	pParaRod->keyStartNode=labels.nodes.KN2;
	pParaRod->keyEndNode=labels.nodes.KNF;
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->m_iElemType = CParaModelRod::FEMELEM_BEAM;
	pParaRod->m_bMajorMat=TRUE;
	pParaRod->layout_style=1;		//里铁喝水
	pParaRod->eyeSideNorm.Set(0,1,0);
	//前侧水平杆件
	pParaRod=pContext->AddParaRod(labels.rods.RFU);
	pParaRod->keyStartNode=labels.nodes.KN1;
	pParaRod->keyEndNode=labels.nodes.KNF;
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->m_iElemType = CParaModelRod::FEMELEM_BEAM;
	pParaRod->m_bMajorMat=TRUE;
	pParaRod->layout_style=2;		//里铁吐水
	pParaRod->eyeSideNorm.Set(0,1,0);
	//后侧斜向杆件
	pParaRod=pContext->AddParaRod(labels.rods.RBD);
	pParaRod->keyStartNode=labels.nodes.KN4;
	pParaRod->keyEndNode=labels.nodes.KNB;
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->m_iElemType = CParaModelRod::FEMELEM_BEAM;
	pParaRod->m_bMajorMat=TRUE;
	pParaRod->layout_style=0;		//外铁喝水
	pParaRod->eyeSideNorm.Set(0,1,0);
	//后侧水平杆件
	pParaRod=pContext->AddParaRod(labels.rods.RBU);
	pParaRod->keyStartNode=labels.nodes.KN3;
	pParaRod->keyEndNode=labels.nodes.KNB;
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->m_iElemType = CParaModelRod::FEMELEM_BEAM;
	pParaRod->m_bMajorMat=TRUE;
	pParaRod->layout_style=3;		//外铁吐水
	pParaRod->eyeSideNorm.Set(0,1,0);
	//传力杆件
	pParaRod=pContext->AddParaRod(labels.rods.RW);
	pParaRod->keyStartNode=labels.nodes.KNF;
	pParaRod->keyEndNode=labels.nodes.KNB;
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->m_iElemType = CParaModelRod::FEMELEM_BEAM;
	pParaRod->layout_style=2;
	pParaRod->eyeSideNorm.Set(1,0,0);
	//水平节间重复杆件
	pParaRod=pContext->AddParaRod(Key4C('A',0x01));
	pParaRod->keyStartNode=labels.nodes.kfu;
	pParaRod->keyEndNode=labels.nodes.kbu;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i+1");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=1;
	/*pParaRod=pContext->AddParaRod(Key4C('A',0x02));		
	pParaRod->keyStartNode=labels.nodes.kbu;
	pParaRod->keyEndNode=labels.nodes.kfu;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i+1");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=0;*/
	//斜平面节间重复杆件
	pParaRod=pContext->AddParaRod(Key4C('B',0x01));		
	pParaRod->keyStartNode=labels.nodes.kfd;
	pParaRod->keyEndNode=labels.nodes.kbd;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i+1");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=2;
	/*pParaRod=pContext->AddParaRod(Key4C('B',0x02));
	pParaRod->keyStartNode=labels.nodes.kbd;
	pParaRod->keyEndNode=labels.nodes.kfd;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i+1");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=2;*/
	//前三角面重复杆件
	pParaRod=pContext->AddParaRod(Key4C('C',0x01));
	pParaRod->keyStartNode=labels.nodes.kfu;
	pParaRod->keyEndNode=labels.nodes.kfd;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i+1");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=2;
	/*pParaRod=pContext->AddParaRod(Key4C('C',0x02));
	pParaRod->keyStartNode=labels.nodes.kfu;
	pParaRod->keyEndNode=labels.nodes.kfd;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=2;*/
	//后三角面重复杆件
	pParaRod=pContext->AddParaRod(Key4C('D',0x01));
	pParaRod->keyStartNode=labels.nodes.kbu;
	pParaRod->keyEndNode=labels.nodes.kbd;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i+1");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=2;
	/*pParaRod=pContext->AddParaRod(Key4C('D',0x02));
	pParaRod->keyStartNode=labels.nodes.kbu;
	pParaRod->keyEndNode=labels.nodes.kbd;
	strcpy(pParaRod->indexParaStart,"i");
	strcpy(pParaRod->indexParaEnd,"i");
	strcpy(pParaRod->exist_logicexpr,"1>0");
	pParaRod->m_idPartClsType=CLS_LINEANGLE;
	pParaRod->eyeSideNorm.Set(0,0,-1);
	pParaRod->layout_style=2;*/
	return true;
}
static bool AssertStdCrossArmParamsValid(IParaBlockContext* pContext)
{
	DESIGN_PARAM_ITEM* pItem;
	bool result=true;
	if((pItem=pContext->GetParamItem(KEY4C("D")))==NULL)
	{	//纵向开口尺寸
#ifdef	AFX_TARG_ENU_ENGLISH
		pContext->LogError("Lack parameter 'D'");
#else
		pContext->LogError("缺少‘D’参数");
#endif
		result = false;
	}
	if((pItem=pContext->GetParamItem(KEY4C("W")))==NULL)
	{	//横向尺寸
#ifdef	AFX_TARG_ENU_ENGLISH
		pContext->LogError("Lack parameter 'W'");
#else
		pContext->LogError("缺少‘W’参数");
#endif
		result = false;
	}
	if((pItem=pContext->GetParamItem(KEY4C("N")))==NULL)
	{	//节间数
		pContext->LogError("缺少‘N’参数");
		result = false;
	}
	if((pItem=pContext->GetParamItem(KEY4C("M")))==NULL)
	{	//节间数
#ifdef	AFX_TARG_ENU_ENGLISH
		pContext->LogError("Lack parameter 'M'");
#else
		pContext->LogError("缺少‘M’参数");
#endif
		result = false;
	}
	return result;
}
///////////////////////////////////////////////////////////////////////////
//各类型模版的操作
//////////////////////////////////////////////////////////////////////////
//初始化模版参数
void InitTemplatePara(IParaBlockContext* pContext, DWORD iTempType,DWORD templateIndex=0)
{
	if(iTempType==0)		//变电构架
	{
		if(templateIndex==0)		//人字架+三角梁
			InitTriBeamSubstationTemp(pContext);
		else if(templateIndex==1)	//人字架+四方梁
			InitSquBeamSubStationTemp(pContext);
	}
	else if(iTempType==1)
	{
		if(templateIndex==0)		//下三角形横担
			InitDownTriaCrossArmTemp(pContext);
		else if(templateIndex==1)	//上三角形横担
			InitUpTriaCrossArmTemp(pContext);
	}
}
//解析模版参数
bool ParseTemplatePara(IParaBlockContext* pContext, DWORD iTempType,DWORD templateIndex=0)
{
	if(iTempType==0)		//变电构架
		return ParseSubstationTemplate(pContext);
	else if(iTempType==1)	//标准横担
	{
		if(templateIndex==0)
			return ParseDownTriaCrossArmTemp(pContext);
		else
			return ParseUpTriaCrossArmTemp(pContext);
	}
	else
		return false;
}
//检测模版参数的有效性
bool NativeAssertParamsValid(IParaBlockContext* pContext, DWORD iTempType,DWORD templateIndex=0)
{
	if(iTempType==0)		//变电构架
		return AssertSubstationParamsValid(pContext);
	else if(iTempType==1)	//标准横担
		return AssertStdCrossArmParamsValid(pContext);
	else
		return false;
}

