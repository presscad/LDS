//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"

#include "MainFrm.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "Dlg.h"
#include "ScrTools.h"
#include "PartNoRuleDlg.h"
#include "CfgPartNoDlg.h"
#include "SortFunc.h"
#include "LocatePartByNoDlg.h"
#include "CheckPartNoDlg.h"
#include "PartNoListDlg.h"
#include "BriefWeldPart.h"
#include "OutputInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//快速排序法
int CompareFL(const CLDSPartPtr& pFl1,const CLDSPartPtr& pFl2)
{
	if(pFl1->ucs.origin.z>pFl2->ucs.origin.z)
		return 1;
	else if(pFl1->ucs.origin.z<pFl2->ucs.origin.z)
		return -1;
	else if(pFl1->ucs.origin.y>pFl2->ucs.origin.y)
		return -1;
	else if(pFl1->ucs.origin.y<pFl2->ucs.origin.y)
		return 1;
	else if(pFl1->ucs.origin.x>pFl2->ucs.origin.x)
		return 1;
	else if(pFl1->ucs.origin.x<pFl2->ucs.origin.x)
		return -1;
	else
	{
		if(pFl1->Layer(2)=='T'&&pFl2->Layer(2)!='T')
			return 1;
		else if(pFl1->Layer(2)!='T'&&pFl2->Layer(2)=='T')
			return -1;
	}
	return 0;
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
static SCOPE_STRU GetTaAtomScope(const CLDSPartPtr pObj)
{
	SCOPE_STRU scope,scope2;
	CLDSNode *pNode;
	CLDSLineTube *pTube;
	CLDSLineAngle *pCommJg;
	CLDSPlate *pPlank;
	CLDSParamPlate *pParamPlate;
	CJoint *pJoint;
	CFoot *pFoot;
	memset(&scope,0,sizeof(SCOPE_STRU));
	if(pObj==NULL)
		return scope;
	f3dPoint start,end;
	switch(pObj->GetClassTypeId())
	{
	/*case CLS_NODE:
		pNode = (CLDSNode*)pObj;
		scope.fMinX = scope.fMaxX = pNode->pos.x;
		scope.fMinY = scope.fMaxY = pNode->pos.y;
		scope.fMinZ = scope.fMaxZ = pNode->pos.z;
		break;*/
	case CLS_FOOT:
		pFoot = (CFoot*)pObj;
		pNode = console.FromNodeHandle(pFoot->base_node_handle);
		scope.fMinX = scope.fMaxX = pNode->Position(false).x;
		scope.fMinY = scope.fMaxY = pNode->Position(false).y;
		scope.fMinZ = scope.fMaxZ = pNode->Position(false).z;
		break;
	case CLS_LINEANGLE:
		pCommJg = (CLDSLineAngle*)pObj;
		if(pCommJg->pStart)
			start = pCommJg->pStart->Position(false);
		else
			start = pCommJg->Start();
		if(pCommJg->pEnd)
			end = pCommJg->pEnd->Position(false);
		else
			end = pCommJg->End();
		scope.fMinX = __min(start.x,end.x);
		scope.fMaxX = __max(start.x,end.x);
		scope.fMinY = __min(start.y,end.y);
		scope.fMaxY = __max(start.y,end.y);
		scope.fMinZ = __min(start.z,end.z);
		scope.fMaxZ = __max(start.z,end.z);
		break;
	case CLS_LINETUBE:
		pTube = (CLDSLineTube*)pObj;
		if(pTube->pStart)
			start = pTube->pStart->Position(false);
		else
			start = pTube->Start();
		if(pTube->pEnd)
			end = pTube->pEnd->Position(false);
		else
			end = pTube->End();
		scope.fMinX = __min(start.x,end.x);
		scope.fMaxX = __max(start.x,end.x);
		scope.fMinY = __min(start.y,end.y);
		scope.fMaxY = __max(start.y,end.y);
		scope.fMinZ = __min(start.z,end.z);
		scope.fMaxZ = __max(start.z,end.z);
		break;
	case CLS_PLATE:
		pPlank = (CLDSPlate*)pObj;
		scope.fMinX = scope.fMaxX = pPlank->ucs.origin.x;
		scope.fMinY = scope.fMaxY = pPlank->ucs.origin.y;
		scope.fMinZ = scope.fMaxZ = pPlank->ucs.origin.z;
		break;
	case CLS_PARAMPLATE:
		pParamPlate = (CLDSParamPlate*)pObj;
		scope.fMinX = scope.fMaxX = pParamPlate->ucs.origin.x;
		scope.fMinY = scope.fMaxY = pParamPlate->ucs.origin.y;
		scope.fMinZ = scope.fMaxZ = pParamPlate->ucs.origin.z;
		break;
	case CLS_JOINT:
		pJoint = (CJoint*)pObj;
		pNode = console.FromNodeHandle(pJoint->base_node_handle);
		scope.fMinX = scope.fMaxX = pNode->Position(false).x;
		scope.fMinY = scope.fMaxY = pNode->Position(false).y;
		scope.fMinZ = scope.fMaxZ = pNode->Position(false).z;
	default:
		if(pObj->IsLinePart())
		{
			CLDSLinePart* pRod=(CLDSLinePart*)pObj;
			if(pRod->pStart)
				start = pRod->pStart->Position(false);
			else
				start = pRod->Start();
			if(pRod->pEnd)
				end = pRod->pEnd->Position(false);
			else
				end = pRod->End();
			scope.fMinX = __min(start.x,end.x);
			scope.fMaxX = __max(start.x,end.x);
			scope.fMinY = __min(start.y,end.y);
			scope.fMaxY = __max(start.y,end.y);
			scope.fMinZ = __min(start.z,end.z);
			scope.fMaxZ = __max(start.z,end.z);
		}
		else
		{
			scope.fMinX = scope.fMaxX = pObj->ucs.origin.x;
			scope.fMinY = scope.fMaxY = pObj->ucs.origin.y;
			scope.fMinZ = scope.fMaxZ = pObj->ucs.origin.z;
		}
		break;
	}
	return scope;
}
//按面类型对塔构件进行排序，原则如下：
//由正面到侧面，最后断面
//面类型级别前正面0 后正面1；左面2 右面3；任意面4
static int GetFaceTypeGrade(const CLDSPartPtr pPart)
{
	char ch;	//标识字母
	//面类型级别前正面0 后正面1；左面2 右面3；任意面4
	int grade=4;
	if(pPart->IsLinePart())
	{
		ch = pPart->layer(2);
		if(ch=='1'||ch=='2'||ch=='Q')//前面
			grade = 0;
		else if(ch=='3'||ch=='4'||ch=='H')//后面
			grade = 1;
		else if(ch=='Z')//左侧面
			grade = 2;
		else if(ch=='Y')//右侧面
			grade = 3;
		else			//任意面
			grade = 4;
	}
	else if(pPart->IsPlate())
	{
		ch = pPart->layer(2);
		if(ch=='1')//前面
		{
			if(fabs(pPart->ucs.axis_z.y)>fabs(pPart->ucs.axis_z.x))
				grade = 0;	//前面
			else
				grade = 2;	//左侧面
		}
		else if(ch=='2')
		{
			if(fabs(pPart->ucs.axis_z.y)>fabs(pPart->ucs.axis_z.x))
				grade = 0;	//前面
			else
				grade = 3;	//右侧面
		}
		else if(ch=='3')
		{
			if(fabs(pPart->ucs.axis_z.y)>fabs(pPart->ucs.axis_z.x))
				grade = 1;	//后面
			else
				grade = 3;	//右侧面
		}
		else if(ch=='4')
		{
			if(fabs(pPart->ucs.axis_z.y)>fabs(pPart->ucs.axis_z.x))
				grade = 1;	//后面
			else
				grade = 2;	//左侧面
		}
		else if(ch=='Q')//前面
			grade = 0;
		else if(ch=='H')//后面
			grade = 1;
		else if(ch=='Z')//左侧面
			grade = 2;
		else if(ch=='Y')//右侧面
			grade = 3;
		else			//任意面
			grade = 4;
	}
	return grade;
}
//主要面构件级别高,但特征值低(如主材为0)
//-1表示第一构件级别高 0表示相同 1表示第二构件级别高
static int CompareFaceType(const CLDSPartPtr& pObj1,const CLDSPartPtr& pObj2)
{
	//面类型级别前正面0 后正面1；左面2 右面3；任意面4
	int grade1,grade2;
	grade1 = GetFaceTypeGrade(pObj1);
	grade2 = GetFaceTypeGrade(pObj2);
	if(grade1<grade2)//级别依次降低(级别值依次升高)
		return -1;
	else if(grade1>grade2)
		return 1;
	else
		return 0;
}
//若排序前已有序返回TRUE否则返回FALSE
static BOOL SortFaceType(CLDSPartPtr& pObj1,CLDSPartPtr& pObj2)
{
	//面类型级别前正面0 后正面1；左面2 右面3；任意面4
	int grade1,grade2;
	grade1 = GetFaceTypeGrade(pObj1);
	grade2 = GetFaceTypeGrade(pObj2);
	if(grade1>grade2)//级别依次降低(级别值依次升高)
	{
		swap_ta_atom(pObj1,pObj2);
		return FALSE;
	}
	return TRUE;
}
//塔构件类型排序,原则如下:
//主材角钢/钢管0;塔脚板2,其它构件1
static int GetTaAtomTypeGrade(const CLDSPartPtr pObj)
{
	if(pObj->IsLinePart()&&(((CLDSLinePart*)pObj)->pStart==NULL||((CLDSLinePart*)pObj)->pEnd==NULL))
		return 4;	//缺少端节点的杆件如短角钢视作普通钢板级别
	if(pObj->GetClassTypeId()==CLS_LINEANGLE)
	{
		CLDSLineAngle *pCommJg = (CLDSLineAngle*)pObj;
		char layertag;
		layertag = pCommJg->Layer(1);
		if(layertag=='Z')
			return 0;
		else if(layertag=='X')
			return 1;
		else if(layertag=='H')
			return 2;
		else
			return 3;
	}
	else if(pObj->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube *pTube = (CLDSLineTube*)pObj;
		char layertag;
		layertag = pTube->layer(1);
		if(layertag=='Z')
			return 0;
		else if(layertag=='X')
			return 1;
		else if(layertag=='H')
			return 2;
		else
			return 3;
	}
	else if(pObj->IsLinePart())
		return 3;
	else if(pObj->IsPlate())
	{
		if(pObj->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pObj)->jdb_style==4)
			return 7;	//螺栓垫板
		else if(pObj->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pObj;
			if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				return 5;	//环向封板
			else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE||pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
				return 6;	//环向肋板
			else if(pParamPlate->m_iParamType==TYPE_BASEPLATE)
				return 10;	//塔脚底板
			else if(pParamPlate->m_iParamType==TYPE_SHOEPLATE)
				return 11;	//塔脚靴板
			else if(pParamPlate->m_hPartWeldParent==TYPE_SHOEPLATE)
				return 8;	//塔脚靴板
		}
		return 4;	//普通钢板
	}
	else
		return 10;	//其余附属构件
}
//主材角钢/钢管0;塔脚板2,其它构件1
static double GetPlateSubTypeGrade(const CLDSPartPtr pObj)
{
	if(!pObj->IsPlate())
		return 0;	//缺少端节点的杆件如短角钢视作普通钢板级别
	CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pObj;
	if(pPlate->GetClassTypeId()==CLS_PLATE)
	{
		if(pObj->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pObj)->jdb_style==4)
			return 7;	//螺栓垫板
		else
			return 1.0/pPlate->GetThick();
	}
	else if(pObj->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pObj;
		if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			return 2;	//环向封板
		else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE||pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
			return 3;	//环向肋板
		else if(pParamPlate->m_iParamType==TYPE_BASEPLATE)
			return 4;	//塔脚底板
		else if(pParamPlate->m_iParamType==TYPE_SHOEPLATE)
			return 5;	//塔脚靴板
		else
			return 6;	//其余类型板,如塔脚肋板
	}
	return 0;
}
//优先级高的构件级别高,但特征值低(如主材为0)
//-1表示第一构件级别高 0表示相同 1表示第二构件级别高
static int CompareTaAtomType(const CLDSPartPtr& pObj1,const CLDSPartPtr& pObj2)
{
	int grade1,grade2;
	grade1 = GetTaAtomTypeGrade(pObj1);
	grade2 = GetTaAtomTypeGrade(pObj2);
	if(grade1<grade2)
		return -1;
	else if(grade1>grade2)
		return 1;
	else
		return 0;
}
//第一参数Z值小则返回正值,二者相等返回0,否则返回负值
static int CompareTaAtomUpDown(const CLDSPartPtr& pObj1,const CLDSPartPtr& pObj2)
{
	SCOPE_STRU scope1,scope2;
	scope1 = GetTaAtomScope(pObj1);
	scope2 = GetTaAtomScope(pObj2);
	double maxz1=ftoi(scope1.fMaxZ)/10;
	double maxz2=ftoi(scope2.fMaxZ)/10;
	
	//实践情况下，如果最你点相同，不应再取中点，否则遇到鸭嘴型横担上的正面辅材无法保证由近到远排序　wjh-2017.8.3
	//if(ftoi(maxz2-maxz1)==0 && pObj1->IsLinePart())
	//{	//杆件时,如果最低点一致则比中间点位置
	//	double midz1=(scope1.fMaxZ+scope1.fMinZ)*0.5;
	//	double midz2=(scope2.fMaxZ+scope2.fMinZ)*0.5;
	//	return ftoi(midz2-midz1);//Z越小应越靠后
	//}
	//else
		return ftoi(maxz2-maxz1);	//Z越小应越靠后
}
//第一参数X值小则返回负值,二者相等返回0,否则返回正值
static int CompareTaAtomL_R(const CLDSPartPtr& pObj1,const CLDSPartPtr& pObj2)
{
	SCOPE_STRU scope1,scope2;
	scope1 = GetTaAtomScope(pObj1);
	scope2 = GetTaAtomScope(pObj2);
	double minx1=scope1.fMinX;
	double minx2=scope2.fMinX;
	if(ftoi(minx2-minx1)==0 && pObj1->IsLinePart())
	{	//杆件时,如果最左点一致则比中间点位置
		double midx1=(scope1.fMinX+scope1.fMaxX)*0.5;
		double midx2=(scope2.fMinX+scope2.fMaxX)*0.5;
		return ftoi(midx1-midx2);//X越小应越靠前
	}
	else
		return ftoi(minx1-minx2);//X越小应越靠前
}
static int CompareRodPrior(const CLDSPartPtr&pObj1,const CLDSPartPtr&pObj2)
{
	if(pObj1->IsLinePart()&&!pObj2->IsLinePart())
		return -1;
	else if(!pObj1->IsLinePart()&&pObj2->IsLinePart())
		return 1;
	else
		return 0;
}
static BOOL ComplexCompareFunc(const CLDSPartPtr&pObj1,const CLDSPartPtr&pObj2)
{
	int k,ret=0;
	//1.比对段号，由小到大
	if(pObj1->iSeg>pObj2->iSeg)
		return 1;
	else if(pObj1->iSeg<pObj2->iSeg)
		return -1;
	long harr[4]={0x370,0x372,0,0};//,0x337,0x338};
	bool a=false,b=false;
	if(pObj1->handle==harr[0]||pObj1->handle==harr[1]||pObj1->handle==harr[2]||pObj1->handle==harr[3])
		a=true;
	if(pObj2->handle==harr[0]||pObj2->handle==harr[1]||pObj2->handle==harr[2]||pObj2->handle==harr[3])
		b=true;
	RULE_STRU rule_arr[4];
	for(k=0;k<4;k++)
		rule_arr[g_sysPara.arrRule[k].iOrder]=g_sysPara.arrRule[k];
	//横担优先由近到远，而塔身及腿，则优先由下到上　wjh-2017.8.3
	//优先主材编号放在第一位（与主材、斜材、横材、辅材排序分开)
	//其次是正(前后)侧(右左)面
	//再次塔身（由下向上）横担（由近到远）（主斜材取极值，横辅材取中值）
	//接着由斜横材辅材
	//2.同一段内永远都是主材优先
	bool bIsObj1Post=false,bIsObj2Post=false;
	if(pObj1->Layer(1)=='Z'&&pObj1->IsLinePart())
		bIsObj1Post=((CLDSLinePart*)pObj1)->pStart!=NULL&&((CLDSLinePart*)pObj1)->pEnd!=NULL;
	if(pObj2->Layer(1)=='Z'&&pObj2->IsLinePart())
		bIsObj2Post=((CLDSLinePart*)pObj2)->pStart!=NULL&&((CLDSLinePart*)pObj2)->pEnd!=NULL;
	if(bIsObj1Post&&!bIsObj2Post)
		return -1;	//同一段内永远都是主材优先
	else if(!bIsObj1Post&&bIsObj2Post)
		return 1;	//同一段内永远都是主材优先
	if(pObj1->Layer(0)=='T'&&pObj2->Layer(0)=='T')
	{
		//3.归属面,先正面后侧面,最后断面
		if((ret=CompareFaceType(pObj1,pObj2))!=0)
			return ret;
		//4.同一工作平面内杆件优先
		if((ret=CompareRodPrior(pObj1,pObj2))!=0)
			return ret;
		//5.两构件均在塔头时，以鸭嘴型横担为例一般以距离塔心距离由近到远规则（主材优先自下向上） wjh-2017.8.3
		CLDSLinePart* pRod1=pObj1->IsLinePart()?(CLDSLinePart*)pObj1:NULL;
		CLDSLinePart* pRod2=pObj2->IsLinePart()?(CLDSLinePart*)pObj2:NULL;
		double dist1,dist2;
		if(pRod1&&pRod2)
		{
			GEPOINT start1=pRod1->pStart!=NULL?pRod1->pStart->Position(false):pRod1->Start();
			GEPOINT end1=pRod1->pEnd!=NULL?pRod1->pEnd->Position(false):pRod1->End();
			GEPOINT start2=pRod2->pStart!=NULL?pRod2->pStart->Position(false):pRod2->Start();
			GEPOINT end2=pRod2->pEnd!=NULL?pRod2->pEnd->Position(false):pRod2->End();
			if(pRod1->Layer(1)=='Z'&&pRod2->Layer(1)=='Z')
			{
				if(max(start1.z,end1.z)<max(start2.z,end2.z))
					return 1;
				else if(max(start1.z,end1.z)>max(start2.z,end2.z))
					return -1;
			}
			if(pRod1->Layer(1)!='F')	//主斜材取极值，辅材取中值
				dist1=min(fabs(start1.x)+fabs(start1.y),fabs(end1.x)+fabs(end1.y))/10;
			else
				dist1=ftoi(fabs(pObj1->ucs.origin.x)+fabs(pObj1->ucs.origin.y))/10;
			if(pRod2->Layer(1)!='F')	//主斜材取极值，辅材取中值
				dist2=min(fabs(start2.x)+fabs(start2.y),fabs(end2.x)+fabs(end2.y))/10;
			else
				dist2=ftoi(fabs(start2.x+end2.x)+fabs(start2.y+end2.y))/10;
			if(dist1==dist2)
			{	//极值相同时取中值
				if(pRod1->Layer(1)!='F')	//主斜材取极值，辅材取中值
					dist1=ftoi(fabs(start1.x+end1.x)+fabs(start1.y+end1.y))/10;
				if(pRod2->Layer(1)!='F')	//主斜材取极值，辅材取中值
					dist2=ftoi(fabs(start2.x+end2.x)+fabs(start2.y+end2.y))/10;
			}
		}
		else
		{
			dist1=ftoi(fabs(pObj1->ucs.origin.x)+fabs(pObj1->ucs.origin.y))/10;
			dist2=ftoi(fabs(pObj2->ucs.origin.x)+fabs(pObj2->ucs.origin.y))/10;
		}
		if(dist1>dist2)
			return 1;
		else if(dist1<dist2)
			return -1;
		//6.自下至上
		if((ret=CompareTaAtomUpDown(pObj1,pObj2))!=0)
			return ret;	//自下至上
		//7.甄别钢板子类别并排序
		if(pObj1->IsPlate()&&pObj2->IsPlate())
		{	//进行钢板子类别甄别，并排序
			double fType1=GetPlateSubTypeGrade(pObj1);
			double fType2=GetPlateSubTypeGrade(pObj2);
			if(fType1>fType2)
				return 1;
			else if(fType1<fType2)
				return -1;
		}
		//8.根据构件类型排序（未来应与7进行整合）
		if((ret=CompareTaAtomType(pObj1,pObj2))!=0)
			return ret;	//构件类型,杆件普通钢板,环向封板,环向肋板,螺栓垫板,其余附属构件
		return 0;
	}
	else
	{
		for(k=0;k<4;k++)
		{
			if(!rule_arr[k].bActive)
				continue;
			if(rule_arr[k].name==MAIN_OTHER)
			{
				if((ret=CompareTaAtomType(pObj1,pObj2))!=0)
					return ret;	//构件类型,杆件普通钢板,环向封板,环向肋板,螺栓垫板,其余附属构件
			}
			else if(rule_arr[k].name==DOWN_UP)
			{
				if((ret=CompareTaAtomUpDown(pObj1,pObj2))!=0)
					return ret;	//自下至上
				if(pObj1->IsPlate()&&pObj2->IsPlate())
				{	//进行钢板子类别甄别
					double fType1=GetPlateSubTypeGrade(pObj1);
					double fType2=GetPlateSubTypeGrade(pObj2);
					if(fType1>fType2)
						return 1;
					else if(fType1<fType2)
						return -1;
				}
			}
			else if(rule_arr[k].name==LEFT_RIGHT)
			{
				if(pObj1->Layer(0)=='L'&&pObj2->Layer(0)=='L')
				{	//两构件均在接腿时一般以第一象限为基准故一般不遵循由左至右的规则 wjh-2017.4.13
					CLDSLinePart* pRod1=pObj1->IsLinePart()?(CLDSLinePart*)pObj1:NULL;
					CLDSLinePart* pRod2=pObj2->IsLinePart()?(CLDSLinePart*)pObj2:NULL;
					char cQuad1=0,cQuad2=0;
					if(pRod1&&pRod2)
					{
						cQuad1=CalPoleQuad(pRod1->Start(),pRod1->End());
						cQuad2=CalPoleQuad(pRod2->Start(),pRod2->End());
					}
					else
					{
						cQuad1=CalPointQuadPos(pObj1->ucs.origin)+'0';
						cQuad2=CalPointQuadPos(pObj2->ucs.origin)+'0';
					}
					if(cQuad1!=cQuad2)
						return cQuad1-cQuad2;
				}
				else if(pObj1->Layer(0)=='T'&&pObj2->Layer(0)=='T')
				{	//两构件均在塔头时，以鸭嘴型横担为例一般以距离塔心距离由近到远规则 wjh-2017.8.3
					CLDSLinePart* pRod1=pObj1->IsLinePart()?(CLDSLinePart*)pObj1:NULL;
					CLDSLinePart* pRod2=pObj2->IsLinePart()?(CLDSLinePart*)pObj2:NULL;
					double dist1,dist2;
					if(pRod1&&pRod2)
					{
						GEPOINT start1=pRod1->pStart!=NULL?pRod1->pStart->Position(false):pRod1->Start();
						GEPOINT end1=pRod1->pEnd!=NULL?pRod1->pEnd->Position(false):pRod1->End();
						GEPOINT start2=pRod2->pStart!=NULL?pRod2->pStart->Position(false):pRod2->Start();
						GEPOINT end2=pRod2->pEnd!=NULL?pRod2->pEnd->Position(false):pRod2->End();
						dist1=min(fabs(start1.x)+fabs(start1.y),fabs(end1.x)+fabs(end1.y));
						dist2=min(fabs(start2.x)+fabs(start2.y),fabs(end2.x)+fabs(end2.y));
						dist1=ftoi(dist1)/10;
						dist2=ftoi(dist2)/10;
						if(dist1==dist2)
						{
							dist1=ftoi(fabs(start1.x+end1.x)+fabs(start1.y+end1.y))/10;
							dist2=ftoi(fabs(start2.x+end2.x)+fabs(start2.y+end2.y))/10;
						}
					}
					else
					{
						dist1=ftoi(fabs(pObj1->ucs.origin.x)+fabs(pObj1->ucs.origin.y))/10;
						dist2=ftoi(fabs(pObj2->ucs.origin.x)+fabs(pObj2->ucs.origin.y))/10;
					}
					if(dist1>dist2)
						return 1;
					else if(dist1<dist2)
						return -1;
				}
				else if((ret=CompareTaAtomL_R(pObj1,pObj2))!=0)
					return ret;	//自左至右
			}
			else if(rule_arr[k].name==FRONT_SIDE)
			{
				if((ret=CompareFaceType(pObj1,pObj2))!=0)
					return ret;	//归属面,先正面后侧面,最后断面
			}
		}
		//杆件优先基本都是同一工作面内先杆件（角钢）后钢板（常利波提出建议）　wjh-2017.8.3
		if(g_sysPara.bFristJgCodeNo&&(ret=CompareTaAtomType(pObj1,pObj2))!=0)
			return ret;	//构件类型,杆件普通钢板,环向封板,环向肋板,螺栓垫板,其余附属构件
	}
	return 0;
}
static BOOL CheckPartSegI()
{
	CLDSNode *pNode;
	CLDSPart *pPart;
	CString sError;
	FILE *fp=fopen("PartSegI.txt","wt");
	int nHits=0;
	g_pSolidDraw->ReleaseSnapStatus();
	for(pNode=Ta.Node.GetFirst();pNode!=NULL;pNode=Ta.Node.GetNext())
	{
		if(pNode->iSeg.iSeg<1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sError.Format("The segment number of node 0x%X is smaller than 1，You need to specify the segment number",pNode->handle);
#else 
			sError.Format("节点0x%X的分段号小于1，需要指定分段号",pNode->handle);
#endif
			nHits++;
			fprintf(fp,"%s\n",sError);
			g_pSolidDraw->SetEntSnapStatus(pNode->handle);
		}
	}
	for(pPart=Ta.Parts.GetFirst();pPart!=NULL;pPart=Ta.Parts.GetNext())
	{
		if( pPart->m_bVirtualPart||pPart->GetClassTypeId()==CLS_POLYJOINT||
			pPart->GetClassTypeId()==CLS_ANCHORBOLT||pPart->GetClassTypeId()==CLS_FITTINGS)
			continue;
		if(pPart->iSeg.iSeg<1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sError.Format("The segment number of part 0x%X is smaller than 1，You need to specify the segment number",pPart->handle);
#else 
			sError.Format("构件0X%X的分段号小于1，需要指定分段号",pPart->handle);
#endif
			nHits++;
			fprintf(fp,"%s\n",sError);
			g_pSolidDraw->SetEntSnapStatus(pPart->handle);
		}
	}
	fclose(fp);
	if(nHits>0)
	{
		WinExec("notepad.exe PartSegI.txt",SW_SHOW);
		return FALSE;
	}
	else
		return TRUE;
}
CStringArray part_no_arr;
static BOOL FindPartNo(const char *sPartNo=NULL)
{
	CLDSPart *pPart;
	if(sPartNo)
	{
		if(strlen(sPartNo)==0)
			return TRUE;
		for(int i=0;i<part_no_arr.GetSize();i++)
		{
			if(part_no_arr[i].CompareNoCase(sPartNo)==0)
				return TRUE;	//此编号已被使用
		}
		return FALSE;
	}
	else
		part_no_arr.RemoveAll();
	int i;
	for(pPart=Ta.Parts.GetFirst();pPart!=NULL;pPart=Ta.Parts.GetNext())
	{
		if(strlen(pPart->GetPartNo())==0)
			continue;
		for(i=0;i<part_no_arr.GetSize();i++)
			if(part_no_arr[i].CompareNoCase(pPart->GetPartNo())==0)	//有此号
				break;
		if(i==part_no_arr.GetSize())
			part_no_arr.Add(pPart->GetPartNo());
	}
	for(CSupplementPart *pSuppPart=Ta.SuppPart.GetFirst();pSuppPart;pSuppPart=Ta.SuppPart.GetNext())
	{
		if(strlen(pSuppPart->GetPartNo())==0)
			continue;
		for(i=0;i<part_no_arr.GetSize();i++)
			if(part_no_arr[i].CompareNoCase(pSuppPart->GetPartNo())==0)	//有此号
				break;
		if(i==part_no_arr.GetSize())
			part_no_arr.Add(pSuppPart->GetPartNo());
	}
	/*for(pHuoQuJg=(CLDSPolyAngle*)Ta.Parts.GetFirst(CLS_POLYANGLE);pHuoQuJg!=NULL;pHuoQuJg=(CLDSPolyAngle*)Ta.Parts.GetNext(CLS_POLYANGLE))
	{
		if(strlen(pHuoQuJg->GetPartNo())==0)
			continue;
		for(int i=0;i<part_no_arr.GetSize();i++)
			if(part_no_arr[i].CompareNoCase(pHuoQuJg->GetPartNo())==0)	//有此号
				break;
		if(i==part_no_arr.GetSize())
			part_no_arr.Add(pHuoQuJg->GetPartNo());
	}*/
	return TRUE;
}
#endif
void CLDSView::OnCodePartNoRule() 
{
	CPartNoRuleDlg dlg;
	for(int i=0;i<4;i++)
		dlg.SetPartOrderRule(i,g_sysPara.arrRule[i].iOrder,g_sysPara.arrRule[i].bActive);
	dlg.m_bJgFirstCodeNo=g_sysPara.bFristJgCodeNo;
	if(dlg.DoModal()==IDOK)
	{
		for(int i=0;i<4;i++)
			dlg.GetPartOrderRule(i,&g_sysPara.arrRule[i].iOrder,&g_sysPara.arrRule[i].bActive);
		g_sysPara.bFristJgCodeNo= dlg.m_bJgFirstCodeNo;
	}
}

void CLDSView::OnClearPartNoI() 
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
#ifdef AFX_TARG_ENU_ENGLISH
	if(MessageBox("Really want to clear all the part label?","LDS",MB_YESNO)==IDYES)
#else 
	if(MessageBox("真的要清空所有构件编号吗?","LDS",MB_YESNO)==IDYES)
#endif
	{
		CLDSPart *pPart;
		//清零所有构件以前的编号
		CTmaPtrList<CLDSParamPlate,CLDSParamPlate*> xendset;
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&((((CLDSParamPlate*)pPart)->m_bStdPart)||
				(true/*codesetdlg.m_bReservFlNo*/&&((CLDSParamPlate*)pPart)->m_iParamType==TYPE_FL)))
			{	//标准构件或保留法兰编号情况下的普通法兰不可清空构件编号	wjh-2012.4.21
				if(((CLDSParamPlate*)pPart)->m_iParamType==TYPE_XEND)
					xendset.append((CLDSParamPlate*)pPart);
				pPart->feature=0;	//不可清空编号构件
			}
			else
				pPart->feature=1;	//可清空编号构件
		}
		for(CLDSParamPlate* pParamPlate=xendset.GetFirst();pParamPlate;pParamPlate=xendset.GetNext())
		{	//十字插板标准件的附件一般也应保留附属子件号
			if(pParamPlate->m_iParamType!=TYPE_XEND)
				continue;
			for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
			{
				pPart=console.FromPartHandle(pItem->h,pItem->idClsType);
				pPart->feature=0;
			}
		}
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(pPart->feature==0)	//不可清空编号的构件
				continue;
			pPart->SetPartNo("");
		}
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("All part number has been cleared!","LDS");
#else 
		MessageBox("所有构件编号已清空!","LDS");
#endif
	}
#endif
	
}
static int _LocalCompareWeldSonPartItem(const WELDSONPART& item1,const WELDSONPART& item2)
{
	return item1.iFuncType-item2.iFuncType;
}
void CLDSView::OnCodePartNo() 
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	int i,j,serial_no,n;
	/*if(theApp.m_bAnalysisWorkEnv)
	{
		theApp.m_bAnalysisWorkEnv=FALSE;
		LdsWorkEnvShift(FALSE);	//构件编号必须转换到绘图模式进行
	}*/
	BeginWaitCursor();
	long MaxCodeNo=0;
/*		构件编辑规则
	1.结构图中除螺栓、脚钉、垫圈外，所有构件均应有编号
	2.构件编号顺序:由下向上、由左到右、先主材后其它构件,
	  由正面到侧面,最后断面
	3.构件编号宜连续,不宜出现空号或编号后加A、B情况。
	4.构件编号时后两位用于表示编号中的流水号，其余位用于表示段号
*/
	n = Ta.Parts.GetNodeNum();
	//n+= Ta.HuoQuJg.GetNodeNum();
	if(n<=0)		//空塔，不需进行编号
		return;
	static CCheckPartNoDlg codesetdlg;
#ifdef AFX_TARG_ENU_ENGLISH
	codesetdlg.m_sCaption="Part Number";
#else 
	codesetdlg.m_sCaption="构件编号";
#endif
	codesetdlg.m_bDisplayFlNoCtrl=true;
	if(codesetdlg.DoModal()!=IDOK)
		return;
	//分解段号字符串
	CHashList<SEGI>hashSegTbl;
	GetSegNoHashTblBySegStr(codesetdlg.m_sSegStr,hashSegTbl);
	if(codesetdlg.m_sSegStr.Find('*')>=0)
	{
		if(!CheckPartSegI())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("No through part segment number check, cannot number part");
#else 
			AfxMessageBox("没有通过构件段号检查,不能进行构件编号");
#endif
			UpdatePropertyPage();	//更新属性框方便修改段号
			return;
		}
		//以下代码保证在未指定段号范围时，哈希表中具有所有涉及到的段号 wjh-2012.12.22
		for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(pPart->iSeg.iSeg!=0&&!pPart->m_bVirtualPart)
				hashSegTbl.SetValue(pPart->iSeg.iSeg,pPart->iSeg);
		}
	}
	//编号进度条
	CString PartNoRecord;
	CStringArray PartNoRecordArr;
	CLDSPartPtr* ta_atom_arr = new CLDSPartPtr[n];
	try
	{
		CLDSPart *pPart;
		//清零所有构件以前的编号
		CTmaPtrList<CLDSParamPlate,CLDSParamPlate*> xendset;
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&((((CLDSParamPlate*)pPart)->m_bStdPart)||
				(codesetdlg.m_bReservFlNo&&((CLDSParamPlate*)pPart)->m_iParamType==TYPE_FL)))
			{	//标准构件或保留法兰编号情况下的普通法兰不可清空构件编号	wjh-2012.4.21
				if(((CLDSParamPlate*)pPart)->m_iParamType==TYPE_XEND)
					xendset.append((CLDSParamPlate*)pPart);
				pPart->feature=0;	//不可清空编号构件
			}
			else
				pPart->feature=1;	//可清空编号构件
		}
		for(CLDSParamPlate* pParamPlate=xendset.GetFirst();pParamPlate;pParamPlate=xendset.GetNext())
		{	//十字插板标准件的附件一般也应保留附属子件号
			if(pParamPlate->m_iParamType!=TYPE_XEND)
				continue;
			for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
			{
				pPart=console.FromPartHandle(pItem->h,pItem->idClsType);
				pPart->feature=0;
			}
		}
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(pPart->feature==0 || pPart->m_bVirtualPart)	//不可清空编号的构件
				continue;
			if(hashSegTbl.GetNodeNum()>0&&hashSegTbl.GetValue(pPart->iSeg.iSeg)==NULL)
				continue;//不是当前段范围内的构件
			pPart->SetPartNo("");
		}
		POLYPARTLIST polylist;
		Ta.RetrievePolyPartList(polylist);
#ifdef AFX_TARG_ENU_ENGLISH
		PutMessage(this,"Tt is setting up the summary library of whole tower's parts!");
#else 
		PutMessage(this,"正在建立全塔的构件汇总库!");
#endif
		i = n = 0;
		PARTSET flset;
		//统计普通编号构件
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(pPart->IsShadowObject())
				continue;	//影射构件不参与自动编号 wht 13-04-28
			else if(pPart->GetClassTypeId()==CLS_BOLT||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE||pPart->m_bVirtualPart)
				continue;	//跳过螺栓、组合角钢、虚拟构件等不需统计的构件
			else if(pPart->IsLinePart()&&(((CLDSLinePart*)pPart)->bHuoQuStart||((CLDSLinePart*)pPart)->bHuoQuEnd))
				continue;	//火曲子构件由火曲构件进行统计
			else if(hashSegTbl.GetNodeNum()>0&&hashSegTbl.GetValue(pPart->iSeg.iSeg)==NULL)
				continue; //不是当前段范围内的构件
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pPart)->m_bStdPart)
				continue;	//标准构件不参与自动编号 wht 11-01-11
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pPart)->m_iParamType==TYPE_FL)
			{	//法兰单独编号
				flset.append(pPart);
				continue;
			}
			else	//未处理制弯构件
				ta_atom_arr[n]=pPart;
			n++;
		}
		//统计制弯构件
		CLDSPolyPart *pPolyPart;
		for(pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
		{
			if(hashSegTbl.GetNodeNum()>0&&hashSegTbl.GetValue(pPolyPart->iSeg.iSeg)==NULL)
				continue;	//不是当前段范围内的构件
			ta_atom_arr[n] = pPolyPart;
			n++;
		}
		//指定段号范围时,ta_atom_arr总数不能直接加上制弯构件的总数 wht 12-02-24
		//n+=polylist.GetNodeNum();
		//统计法兰
		CLDSPartPtr* fl_arr=new CLDSPartPtr[flset.GetNodeNum()];
		i=0;
		for(pPart=flset.GetFirst(),i=0;pPart;pPart=flset.GetNext(),i++)
			fl_arr[i]=pPart;
		CQuickSort<CLDSPartPtr>::QuickSort(fl_arr,flset.GetNodeNum(),CompareFL);
		serial_no=1;
		CXhChar16 sPartNo;
		for(i=0;i<flset.GetNodeNum();i++)
		{
			CLDSParamPlate *pFl=(CLDSParamPlate*)fl_arr[i];
			int str_len=strlen(pFl->GetPartNo());
			if(str_len==0)
			{	//未编号
				sPartNo.Printf("FL%d",serial_no);
				pFl->SetPartNo(sPartNo);
				for(j=i+1;j<flset.GetNodeNum();j++)
				{
					CLDSParamPlate *pCompareFl=(CLDSParamPlate*)fl_arr[j];
					if(strlen(pCompareFl->GetPartNo())>0)
						continue;
					if(DISTANCE(pFl->ucs.origin,pCompareFl->ucs.origin)<100&&!pFl->IsEqual(pCompareFl,codesetdlg.m_fScope))
					{	//有成对法兰，且不一致时以A,B号分别编号
						sPartNo.Append("A");
						pFl->SetPartNo(sPartNo);
						//
						CXhChar16 sCoupleFlPartNo("FL%dB*",serial_no);
						pCompareFl->SetPartNo(sCoupleFlPartNo);
						break;
					}
				}
				PartNoRecord.Format("%4X,%s,F,N,%s",pFl->handle,(char*)pFl->iSeg.ToString(),(char*)pFl->GetPartNo());
				PartNoRecordArr.Add(PartNoRecord);
				serial_no++;
			}
			sPartNo.Printf("%s",(char*)pFl->GetPartNo());
#ifndef __ENABLE_XHSTRING_LENGTH_PROPERTY_
			if(sPartNo.Length()>0&&!sPartNo.EndWith('*'))
#else
			if(sPartNo.Length>0&&!sPartNo.EndWith('*'))
#endif
				continue;
			else
			{	//以*号结束的法兰是成对生成而不是比较生成的编号需要与后续法兰进行构件比较
				if(str_len>0)
				{
					sPartNo.At(str_len-1)='\0';	//去掉'*'
					pFl->SetPartNo(sPartNo);	
					PartNoRecord.Format("%4X,%s,F,N,%s",pFl->handle,(char*)pFl->iSeg.ToString(),(char*)pFl->GetPartNo());
					PartNoRecordArr.Add(PartNoRecord);
				}
				for(j=i+1;j<flset.GetNodeNum();j++)
				{
					CLDSParamPlate *pCompareFl=(CLDSParamPlate*)fl_arr[j];
					if(strlen(pCompareFl->GetPartNo())>0)
						continue;	//已有编号
					if(pFl->IsEqual(pCompareFl,codesetdlg.m_fScope))
					{	//有成对法兰
						pCompareFl->SetPartNo(pFl->GetPartNo());
						PartNoRecord.Format("%4X,%s,F,R,%s",pCompareFl->handle,(char*)pCompareFl->iSeg.ToString(),(char*)pCompareFl->GetPartNo());
						PartNoRecordArr.Add(PartNoRecord);
					}
				}
			}
		}
		char sPrevPartNo[20]="";
		for(i=0;i<flset.GetNodeNum();i++)
		{
			CLDSParamPlate *pFl=(CLDSParamPlate*)fl_arr[i];
			int nRib=pFl->GetAttachPartCount();
			CLDSPart** rib_arr=new CLDSPart*[nRib];
			j=0;
			for(ATTACHPART_ITEM *pItem=pFl->GetFirstAttachPart();pItem;pItem=pFl->GetNextAttachPart())
			{
				rib_arr[j]=console.FromPartHandle(pItem->h);
				j++;
			}
			int iRib=2;	//１号板默认是指法兰底板，故肘板从２号开始编号
			for(j=0;j<nRib;j++)
			{
				if(rib_arr[j]==NULL)
					continue;
				else if(strlen(rib_arr[j]->GetPartNo())>0)
					continue;
				else
				{
					sPartNo.Printf("%s-%d",(char*)pFl->GetPartNo(),iRib);
					rib_arr[j]->SetPartNo(sPartNo);
				}
				if(stricmp(sPrevPartNo,rib_arr[j]->GetPartNo())==0)
					PartNoRecord.Format("%4X,%s,-,R,%s",rib_arr[j]->handle,(char*)rib_arr[j]->iSeg.ToString(),(char*)rib_arr[j]->GetPartNo());
				else
				{
					PartNoRecord.Format("%4X,%s,-,N,%s",rib_arr[j]->handle,(char*)rib_arr[j]->iSeg.ToString(),(char*)rib_arr[j]->GetPartNo());
					strcpy(sPrevPartNo,rib_arr[j]->GetPartNo());
				}
				PartNoRecordArr.Add(PartNoRecord);
				for(int k=j+1;k<nRib;k++)
				{
					if(rib_arr[k]==NULL)
						continue;
					else if(rib_arr[j]->IsEqual(rib_arr[k],TRUE,codesetdlg.m_fScope))
					{
						rib_arr[k]->SetPartNo(rib_arr[j]->GetPartNo());
						PartNoRecord.Format("%4X,%s,-,R,%s",rib_arr[k]->handle,(char*)rib_arr[k]->iSeg.ToString(),(char*)rib_arr[k]->GetPartNo());
						PartNoRecordArr.Add(PartNoRecord);
					}
				}
			}
			delete []rib_arr;
		}
		delete []fl_arr;

		//进行综合排序,
		CBubbleSort<CLDSPart*>::BubSort(ta_atom_arr,n,ComplexCompareFunc);
		//进行组焊件归并移位排序
		PLACEINDEX* pIndex;
		CHashListEx<PLACEINDEX> dynaObjIndex;
		for(i=0;i<n;i++)
		{
			pIndex=dynaObjIndex.Add(ta_atom_arr[i]->handle);
			pIndex->index=i;
			pIndex->pObj=ta_atom_arr[i];
		}
		CHashListEx<CBriefWeldPart> hashWeldParts;
		Ta.RetrieveBriefWeldPartGroups(hashWeldParts,codesetdlg.m_sSegStr);
		for(CBriefWeldPart* pWeldPart=hashWeldParts.GetFirst();pWeldPart;pWeldPart=hashWeldParts.GetNext())
		{
			if( pWeldPart->ciWeldType==CBriefWeldPart::WELD_ANGLEFOOT||
				pWeldPart->ciWeldType==CBriefWeldPart::WELD_STDX_PLATES||
				pWeldPart->ciWeldType==CBriefWeldPart::WELD_TUBECONNPLATES_INC_COVER)
			{
				WELDSONPART* pSonPart;
				ARRAY_LIST<WELDSONPART> arraySonParts(0,8);
				for(pSonPart=pWeldPart->EnumObjectFirst();pSonPart;pSonPart=pWeldPart->EnumObjectNext())
					arraySonParts.append(*pSonPart);
				CBubbleSort<WELDSONPART>::BubSort(arraySonParts.m_pData,arraySonParts.GetSize(),_LocalCompareWeldSonPartItem);
				for(i=0;i<arraySonParts.GetSize();i++)
				{
					if(i==0)
					{	
						if(arraySonParts[0].iFuncType<pWeldPart->pRootSonPart->iFuncType)
							PLACEINDEX::MoveBefore(&dynaObjIndex,arraySonParts[0].hPart,pWeldPart->pRootSonPart->hPart);
						else if(arraySonParts[0].iFuncType>pWeldPart->pRootSonPart->iFuncType)
							PLACEINDEX::MoveAfter(&dynaObjIndex,arraySonParts[0].hPart,pWeldPart->pRootSonPart->hPart);
					}
					else
					{
						if(arraySonParts[i].iFuncType>arraySonParts[i-1].iFuncType)
							PLACEINDEX::MoveAfter(&dynaObjIndex,arraySonParts[i].hPart,arraySonParts[i-1].hPart);
						//else if(arraySonParts[0].iFuncType==pWeldPart->pRootSonPart->iFuncType)
						//	continue;
					}
				}
			}
			//PLACEINDEX::MoveBefore
		}
		memset(ta_atom_arr,0,sizeof(CLDSPart*)*n);
		for(pIndex=dynaObjIndex.GetFirst();pIndex;pIndex=dynaObjIndex.GetNext())
			ta_atom_arr[pIndex->index]=(CLDSPartPtr)pIndex->pObj;
		for(i=0;i<n;i++)
		{
			if(ta_atom_arr[i]==NULL)
				logerr.Log("组焊件重新移位排序出错!");
		}

		char sPrompt[MAX_TEMP_CHAR_100+1];
		for(SEGI* pIseg=hashSegTbl.GetFirst();pIseg;pIseg=hashSegTbl.GetNext())
		{	
			serial_no = 1;
#ifdef AFX_TARG_ENU_ENGLISH
			_snprintf(sPrompt,MAX_TEMP_CHAR_100,"The %s section of part number is in progress",(char*)pIseg->ToString());
#else 
			_snprintf(sPrompt,MAX_TEMP_CHAR_100,"正在进行第%s分段的构件编号",(char*)pIseg->ToString());
#endif
			PutMessage(this,sPrompt);
			for(j=0;j<n;j++)
			{
				if(ta_atom_arr[j]->iSeg.iSeg!=pIseg->iSeg)	//段号判断
					continue;
				if(ta_atom_arr[j]->m_bVirtualPart)
					continue;
				if(strlen(ta_atom_arr[j]->GetPartNo())>0)				//是否已编号
					continue;
				while(FindPartNo(ta_atom_arr[j]->GetPartNo()))
				{
					if(serial_no<10)
						ta_atom_arr[j]->SetPartNo(CXhChar16("%s0%d",(char*)ta_atom_arr[j]->iSeg.ToString(),serial_no));
					else if(serial_no<100)
						ta_atom_arr[j]->SetPartNo(CXhChar16("%s%d",(char*)ta_atom_arr[j]->iSeg.ToString(),serial_no));
					else
						ta_atom_arr[j]->SetPartNo(CXhChar16("%s-%d",(char*)ta_atom_arr[j]->iSeg.ToString(),serial_no));
					serial_no++;
				}
				if(ta_atom_arr[j]->GetClassTypeId()==CLS_POLYPART)
				{	//制弯构件中所有子构件都拥有相同的编号
					pPolyPart=(CLDSPolyPart*)ta_atom_arr[j];
					CLDSLinePart *pLinePart;
					for(pLinePart=pPolyPart->segset.GetFirst();pLinePart;pLinePart=pPolyPart->segset.GetNext())
						pLinePart->SetPartNo(pPolyPart->GetPartNo());
					pLinePart=((CLDSPolyPart*)ta_atom_arr[j])->segset.GetFirst();
					PartNoRecord.Format("%4X,%s,L,N,%s",pLinePart->handle,(char*)pLinePart->iSeg.ToString(),(char*)pLinePart->GetPartNo());
				}
				else if(ta_atom_arr[j]->GetClassTypeId()==CLS_LINEANGLE)
					PartNoRecord.Format("%4X,%s,L,N,%s",ta_atom_arr[j]->handle,(char*)ta_atom_arr[j]->iSeg.ToString(),(char*)ta_atom_arr[j]->GetPartNo());
				else if(ta_atom_arr[j]->GetClassTypeId()==CLS_LINETUBE)
					PartNoRecord.Format("%4X,%s,P,N,%s",ta_atom_arr[j]->handle,(char*)ta_atom_arr[j]->iSeg.ToString(),(char*)ta_atom_arr[j]->GetPartNo());
				else if(ta_atom_arr[j]->GetClassTypeId()==CLS_PLATE||ta_atom_arr[j]->GetClassTypeId()==CLS_PARAMPLATE)
					PartNoRecord.Format("%4X,%s,-,N,%s",ta_atom_arr[j]->handle,(char*)ta_atom_arr[j]->iSeg.ToString(),(char*)ta_atom_arr[j]->GetPartNo());
				else //if(ta_atom_arr[j]->GetClassTypeId()==CLS_PLATE||ta_atom_arr[j]->GetClassTypeId()==CLS_PARAMPLATE)
					PartNoRecord.Format("%4X,%s,?,N,%s",ta_atom_arr[j]->handle,(char*)ta_atom_arr[j]->iSeg.ToString(),(char*)ta_atom_arr[j]->GetPartNo());
				PartNoRecordArr.Add(PartNoRecord);
				Ta.Parts.push_stack();
				for(CLDSPart *pEqualPart=Ta.Parts.GetFirst();pEqualPart;pEqualPart=Ta.Parts.GetNext())
				{
					if(pEqualPart==ta_atom_arr[j])
						continue;
					else if(pEqualPart->m_bVirtualPart)
						continue;
					else if(strlen(pEqualPart->GetPartNo())>0)
						continue;	//已有编号
					else if(strlen(pEqualPart->GetPartNo())==0&&pEqualPart->iSeg.iSeg==pIseg->iSeg
						&&ta_atom_arr[j]->IsEqual(pEqualPart,TRUE,codesetdlg.m_fScope))
					{
						pEqualPart->SetPartNo(ta_atom_arr[j]->GetPartNo());
						if(ta_atom_arr[j]->GetClassTypeId()==CLS_LINEANGLE)
							PartNoRecord.Format("%4X,%s,L,R,%s",pEqualPart->handle,(char*)pEqualPart->iSeg.ToString(),(char*)pEqualPart->GetPartNo());
						else if(ta_atom_arr[j]->GetClassTypeId()==CLS_LINETUBE)
							PartNoRecord.Format("%4X,%s,P,R,%s",pEqualPart->handle,(char*)pEqualPart->iSeg.ToString(),(char*)pEqualPart->GetPartNo());
						else if(ta_atom_arr[j]->GetClassTypeId()==CLS_PLATE||ta_atom_arr[j]->GetClassTypeId()==CLS_PARAMPLATE)
							PartNoRecord.Format("%4X,%s,-,R,%s",pEqualPart->handle,(char*)pEqualPart->iSeg.ToString(),(char*)pEqualPart->GetPartNo());
						else //if(ta_atom_arr[j]->GetClassTypeId()==CLS_PLATE||ta_atom_arr[j]->GetClassTypeId()==CLS_PARAMPLATE)
							PartNoRecord.Format("%4X,%s,?,R,%s",pEqualPart->handle,(char*)pEqualPart->iSeg.ToString(),(char*)pEqualPart->GetPartNo());
						PartNoRecordArr.Add(PartNoRecord);
					}
				}
				if(ta_atom_arr[j]->GetClassTypeId()==CLS_POLYPART)
				{	//制弯构件中所有子构件都拥有相同的编号
					pPolyPart=(CLDSPolyPart*)ta_atom_arr[j];
					CLDSLinePart *pLinePart;
					for(CLDSPolyPart *pEqualPolyPart=polylist.GetFirst();pEqualPolyPart;pEqualPolyPart=polylist.GetNext())
					{
						if(strlen(pEqualPolyPart->GetPartNo())==0&&pEqualPolyPart->iSeg.iSeg==pIseg->iSeg
							&&ta_atom_arr[j]->IsEqual(pEqualPolyPart,TRUE,codesetdlg.m_fScope))
						{
							pEqualPolyPart->SetPartNo(ta_atom_arr[j]->GetPartNo());
							for(pLinePart=pEqualPolyPart->segset.GetFirst();pLinePart;pLinePart=pEqualPolyPart->segset.GetNext())
								pLinePart->SetPartNo(pEqualPolyPart->GetPartNo());
							pLinePart=pEqualPolyPart->segset.GetFirst();
							if(ta_atom_arr[j]->GetClassTypeId()==CLS_LINEANGLE)
								PartNoRecord.Format("%4X,%s,L,R,%s",pLinePart->handle,(char*)pLinePart->iSeg.ToString(),(char*)pLinePart->GetPartNo());
							else if(ta_atom_arr[j]->GetClassTypeId()==CLS_LINETUBE)
								PartNoRecord.Format("%4X,%s,P,R,%s",pLinePart->handle,(char*)pLinePart->iSeg.ToString(),(char*)pLinePart->GetPartNo());
							else if(ta_atom_arr[j]->GetClassTypeId()==CLS_PLATE||ta_atom_arr[j]->GetClassTypeId()==CLS_PARAMPLATE)
								PartNoRecord.Format("%4X,%s,-,R,%s",pLinePart->handle,(char*)pLinePart->iSeg.ToString(),(char*)pLinePart->GetPartNo());
							else //if(ta_atom_arr[j]->GetClassTypeId()==CLS_PLATE||ta_atom_arr[j]->GetClassTypeId()==CLS_PARAMPLATE)
								PartNoRecord.Format("%4X,%s,?,R,%s",pLinePart->handle,(char*)pLinePart->iSeg.ToString(),(char*)pLinePart->GetPartNo());
							PartNoRecordArr.Add(PartNoRecord);
						}
					}
				}
				Ta.Parts.pop_stack();
			}
			
			if(serial_no>9999)	//构件编号上限 暂时调大点 wht 08-7-16
			{
				delete []ta_atom_arr;
				char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
				_snprintf(sError,199,"The first %s segment more than limits of 999, number failed!",(char*)pIseg->ToString());
#else 
				_snprintf(sError,199,"第%s段超过999个的构件数上限,编号失败!",(char*)pIseg->ToString());
#endif
				throw sError;
			}
			MaxCodeNo += serial_no;
		}
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		PutMessage(this,NULL);
		delete []ta_atom_arr;
		return;
	}
	delete []ta_atom_arr;
	EndWaitCursor();
#ifdef AFX_TARG_ENU_ENGLISH
	MessageBox("Complete tower's label");
#else 
	MessageBox("塔材编号完毕");
#endif
	PutMessage(this,NULL);
	m_pDoc->SetModifiedFlag();
	/*if(MessageBox("是否将编号过程保存下来，以便再次编号使用？","LDS",MB_YESNO)==IDYES)
	{
		CFileDialog file_dlg(FALSE,"no","LDS",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"编号过程文件(*.no)|*.no||");
		if(file_dlg.DoModal()==IDOK)
		{
			char file_name[MAX_PATH];
			_snprintf(file_name,MAX_PATH-1,"%s",file_dlg.GetPathName());
			FILE *fp=fopen(file_name,"wt");
			fprintf(fp,"构件句柄，段号，构件类型，编号类型，构件编号\n'L':角钢;'P':钢管;'-':钢板;'F'法兰; 'N':新编号;'R':重复上一编号;'S':指定为后面的编号\n");
			for(i=0;i<PartNoRecordArr.GetSize();i++)
				fprintf(fp,"%s\n",PartNoRecordArr[i]);
			fclose(fp);
		}
	}*/
#endif
}

void CLDSView::OnSegICheck() 
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	m_nPrevCommandID=ID_SEG_I_CHECK;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat Check Segment Number";
	if(CheckPartSegI())
		MessageBox("Through the detection of part's segment number ");
#else 
	m_sPrevCommandName="重复段号检查";
	if(CheckPartSegI())
		MessageBox("通过构件分段号检测");
#endif
	UpdatePropertyPage();	//更新属性框方便修改段号
#endif
}

static double scope=0.5;
CCheckPartNoDlg dlg;
void CLDSView::OnPartNoCheck() 
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	m_nPrevCommandID=ID_PART_NO_CHECK;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat Check Part Number";
#else 
	m_sPrevCommandName="重复构件号检查";
#endif
	//Input("校核误差范围",&scope,'f');
	dlg.m_fScope=scope;
	dlg.m_bDisplayFlNoCtrl=false;
	if(IDOK!=dlg.DoModal())
		return;
	scope=dlg.m_fScope;
	POLYPARTLIST polylist;
	Ta.RetrievePolyPartList(polylist);
	int i=0,nHits=0,n=Ta.Parts.GetNodeNum(),poly_n=polylist.GetNodeNum();
	CSuperSmartPtr<CLDSPart>pPart;//,pOthPart;
	FILE *fp;
	//分解段号字符串
	CHashList<SEGI>segTbl;
	GetSegNoHashTblBySegStr(dlg.m_sSegStr,segTbl);
	fp = fopen("NoCheck.txt","wt");
	CString sError,sCurIndex;
	CHashStrTable<CLDSPart*>partTbl;
	partTbl.CreateHashTable(n);
	g_pSolidDraw->ReleaseSnapStatus();
	CTaskOutputList itemList;
	try{
		for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
		{
			i++;
			//if(pPart->handle==0x9026||pPart->handle==0x9027)
				int b=-3;
			//else
			//	continue;
			if(pPart->GetClassTypeId()==CLS_BOLT||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE||
				pPart->GetClassTypeId()==CLS_POLYJOINT||pPart->m_bVirtualPart)
				continue;	//螺栓无编号不参预构件号检查
			else if(pPart->IsLinePart()&&(pPart.LinePartPointer()->bHuoQuStart||pPart.LinePartPointer()->bHuoQuEnd))
				continue;
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPart->dwPermission))
				continue;	//网络协同时，对此角钢没有拥有修改权限
			if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPart->iSeg.iSeg)==NULL)
				continue;	//检查是否在指定段号范围内
			else if(strlen(pPart->GetPartNo())<1)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(pPart->GetClassTypeId()==CLS_LINEANGLE)
					sError.Format("The angle's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_LINETUBE)
					sError.Format("The tube's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_LINESLOT)
					sError.Format("The U-steel's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_PLATE)
					sError.Format("The plate's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
					sError.Format("The parameter plate's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
					sError.Format("The flat angle's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_ARCANGLE)
					sError.Format("The circular angle's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_ARCFLAT)
					sError.Format("The circular flat angle's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_ARCSLOT)
					sError.Format("The circular U-steel's number of 0X%X inputs error,please check it!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_SPHERE)
					sError.Format("The sphere's number of 0X%X inputs error,please check it!",pPart->handle);
#else 
				if(pPart->GetClassTypeId()==CLS_LINEANGLE)
					sError.Format("0X%X角钢的构件号未正确输入,请查实!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_LINETUBE)
					sError.Format("0X%X钢管的构件号未正确输入,请查实!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_LINESLOT)
					sError.Format("0X%X槽钢的构件号未正确输入,请查实!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_PLATE)
					sError.Format("0X%X钢板的构件号未正确输入,请查实!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
					sError.Format("0X%X参数化钢板的构件号未正确输入,请查实!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
					sError.Format("0X%X扁钢的构件号未正确输入,请查实!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_ARCANGLE)
					sError.Format("0X%X环形角钢的构件号未正确输入,请查实!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_ARCFLAT)
					sError.Format("0X%X环形扁钢的构件号未正确输入,请查实!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_ARCSLOT)
					sError.Format("0X%X环形槽钢的构件号未正确输入,请查实!",pPart->handle);
				else if(pPart->GetClassTypeId()==CLS_SPHERE)
					sError.Format("0X%X球体的构件号未正确输入,请查实!",pPart->handle);
#endif
				if(strlen(sError)>0)
				{
					fprintf(fp,"%s\n",sError);
					nHits++;
					AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)pPart->GetPartNo(),pPart->handle,0,sError));
				}
				continue;
			}
			else
			{
				CLDSPart *pTmPart=NULL;
				//由于部分院标准件编号存在小写字母,所以必须克隆出一个编号字符串来升为大写 wjh-2012.3.18
				CXhChar50 upCasePartNo(pPart->GetPartNo());
				strupr(upCasePartNo);	//将构件编号中的字母变为大写 wht 09-09-27
				if(!partTbl.GetValueAt(upCasePartNo,pTmPart))	//该件号第一个构件
					partTbl.SetValueAt(upCasePartNo,pPart);
				else
				{
					CLDSParamPlate *pParamPlate=NULL;
					if(pTmPart->GetClassTypeId()==CLS_PARAMPLATE)
						pParamPlate=(CLDSParamPlate*)pTmPart;
					char error_info[200]="";
					if(pPart->GetClassTypeId()!=pTmPart->GetClassTypeId())
					{
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("The type of 0X%X and 0X%X is different,but they have same piece number！",pPart->handle,pTmPart->handle);
#else 
						sError.Format("0X%X构件与0X%X构件不是同一类型构件，但编了相同的构件号！",pPart->handle,pTmPart->handle);
#endif
						//g_pSolidDraw->SetEntSnapStatus(pPart->handle);
						//g_pSolidDraw->SetEntSnapStatus(pTmPart->handle);
						fprintf(fp,"%s\n",sError);
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)pPart->GetPartNo(),pPart->handle,pTmPart->handle,sError));
						nHits++;
					}
					//跳过标准构件
					/*else if(pPart->iSeg!=pTmPart->iSeg&&(pParamPlate==NULL||(pParamPlate&&!pParamPlate->m_bStdPart)))
					{	//编号相同的构件并不一定要位于同一个段(标准构件，法兰)，但多数情况编号相同就位于同一段
						if(pParamPlate&&pParamPlate->m_iParamType==TYPE_FL)
							sError.Format("0X%X法兰与0X%X法兰，但编了相同的构件号(%s)！",pPart->handle,pTmPart->handle,(char*)pPart->GetPartNo());
						else
							sError.Format("0X%X构件与0X%X构件，但编了相同的构件号(%s)！",pPart->handle,pTmPart->handle,(char*)pPart->GetPartNo());
						g_pSolidDraw->SetEntSnapStatus(pPart->handle);
						g_pSolidDraw->SetEntSnapStatus(pTmPart->handle);
						fprintf(fp,"%s\n",sError);
						nHits++;
					}*/
					else if(!pPart->IsEqual(pTmPart,TRUE,scope,error_info))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						if(pPart->GetClassTypeId()==CLS_LINEANGLE)
							sError.Format("Angle 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_LINETUBE)
							sError.Format("Tube 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_LINESLOT)
							sError.Format("U-steel 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_PLATE)
							sError.Format("Plane 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
							sError.Format("Parameter Plate 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
							sError.Format("Flat angle 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_ARCSLOT)
							sError.Format("Circular U-steel 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_ARCANGLE)
							sError.Format("Circular angle 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_ARCFLAT)
							sError.Format("Circular flat angle 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_SPHERE)
							sError.Format("Sphere 0X%X and 0X%X(%s)have same piece number",pPart->handle,pTmPart->handle,error_info);
#else 
						if(pPart->GetClassTypeId()==CLS_LINEANGLE)
							sError.Format("角钢0X%X与0X%X(%s)但编了相同的构件号",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_LINETUBE)
							sError.Format("钢管0X%X与0X%X(%s)但编了相同的构件号",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_LINESLOT)
							sError.Format("钢管0X%X与0X%X(%s)但编了相同的构件号",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_PLATE)
							sError.Format("钢板0X%X与0X%X(%s)但编了相同的构件号",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
							sError.Format("参数化钢板0X%X与0X%X(%s)但编了相同的构件号",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
							sError.Format("扁钢0X%X与0X%X(%s)但编了相同的构件号",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_ARCSLOT)
							sError.Format("环形槽钢0X%X与0X%X(%s)但编了相同的构件号",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_ARCANGLE)
							sError.Format("环形角钢0X%X与0X%X(%s)但编了相同的构件号",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_ARCFLAT)
							sError.Format("环形扁钢0X%X与0X%X(%s)但编了相同的构件号",pPart->handle,pTmPart->handle,error_info);
						else if(pPart->GetClassTypeId()==CLS_SPHERE)
							sError.Format("球体0X%X与0X%X(%s)但编了相同的构件号",pPart->handle,pTmPart->handle,error_info);
#endif
						//g_pSolidDraw->SetEntSnapStatus(pPart->handle);
						//g_pSolidDraw->SetEntSnapStatus(pTmPart->handle);
						fprintf(fp,"%s\n",sError);
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)pPart->GetPartNo(),pPart->handle,pTmPart->handle,sError));
						nHits++;
					}
				}
			}
			DisplayProcess((i*100)/(n+poly_n));
		}
		CLDSPolyPart *pPolyPart=NULL,*pOthPolyPart=NULL;
		CSuperSmartPtr<CLDSLinePart>pFirstLinePart;
		for(pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
		{
			i++;
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPolyPart->dwPermission))
				continue;	//网络协同时，对此角钢没有拥有修改权限
			if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPolyPart->iSeg.iSeg)==NULL)
				continue;	//检查是否在指点段号范围内
			pFirstLinePart=pPolyPart->segset.GetFirst();
			if(strlen(pPolyPart->GetPartNo())<1)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(pFirstLinePart->GetClassTypeId()==CLS_LINEANGLE)
					sError.Format("The angle's number of 0X%X inputs error,please check it!",pFirstLinePart->handle);
				else if(pFirstLinePart->GetClassTypeId()==CLS_LINETUBE)
					sError.Format("The tube's number of 0X%X inputs error,please check it!",pFirstLinePart->handle);
				else if(pFirstLinePart->GetClassTypeId()==CLS_LINESLOT)
					sError.Format("The U-steel's number of 0X%X inputs error,please check it!",pFirstLinePart->handle);
				else if(pFirstLinePart->GetClassTypeId()==CLS_LINEFLAT)
					sError.Format("The flat angle's number of 0X%X inputs error,please check it!",pFirstLinePart->handle);
#else 
				if(pFirstLinePart->GetClassTypeId()==CLS_LINEANGLE)
					sError.Format("0X%X角钢的构件号未正确输入,请查实!",pFirstLinePart->handle);
				else if(pFirstLinePart->GetClassTypeId()==CLS_LINETUBE)
					sError.Format("0X%X钢管的构件号未正确输入,请查实!",pFirstLinePart->handle);
				else if(pFirstLinePart->GetClassTypeId()==CLS_LINESLOT)
					sError.Format("0X%X槽钢的构件号未正确输入,请查实!",pFirstLinePart->handle);
				else if(pFirstLinePart->GetClassTypeId()==CLS_LINEFLAT)
					sError.Format("0X%X扁钢的构件号未正确输入,请查实!",pFirstLinePart->handle);
#endif
				else
					continue;
				fprintf(fp,"%s\n",sError);
				AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)pFirstLinePart->GetPartNo(),pFirstLinePart->handle,0,sError));
				nHits++;
				continue;
			}
			else
			{
				CLDSPart *pTmPart=NULL;
				CXhChar50 upCasePartNo(pPolyPart->GetPartNo());
				strupr(upCasePartNo);	//将构件编号中的字母变为大写 wht 09-09-27
				if(partTbl.GetValueAt(upCasePartNo,pTmPart))
				{	//检查火曲构件编号是否与其他构件编号冲突
					if(pTmPart->GetClassTypeId()!=pPolyPart->GetClassTypeId())
					{
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("The type of 0X%X and 0X%X is different,but they have same piece number！",pTmPart->handle,pFirstLinePart->handle,(char*)pTmPart->GetPartNo());
#else 
						sError.Format("0X%X构件与0X%X构件{%s}不是同一类型构件，但编了相同的构件号！",pTmPart->handle,pFirstLinePart->handle,(char*)pTmPart->GetPartNo());
#endif
						//g_pSolidDraw->SetEntSnapStatus(pFirstLinePart->handle);
						//g_pSolidDraw->SetEntSnapStatus(pTmPart->handle);
						fprintf(fp,"%s\n",sError);
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)pFirstLinePart->GetPartNo(),pFirstLinePart->handle,pTmPart->handle,sError));
						nHits++;
					}
				}
				polylist.push_stack();
				for(pOthPolyPart=polylist.GetNext();pOthPolyPart;pOthPolyPart=polylist.GetNext())
				{
					if(stricmp(pPolyPart->GetPartNo(),pOthPolyPart->GetPartNo())==0)
					{
						char error_info[50]="";
						CLDSLinePart *pOthFirstLinePart=pOthPolyPart->segset.GetFirst();
						if(pFirstLinePart->GetClassTypeId()!=pOthFirstLinePart->GetClassTypeId())
						{
#ifdef AFX_TARG_ENU_ENGLISH
							sError.Format("The type of 0X%X and 0X%X is different,but they have same piece number！",pFirstLinePart->handle,pOthFirstLinePart->handle,(char*)pFirstLinePart->GetPartNo());
#else 
							sError.Format("0X%X构件与0X%X构件{%s}不是同一类型构件，但编了相同的构件号！",pFirstLinePart->handle,pOthFirstLinePart->handle,(char*)pFirstLinePart->GetPartNo());
#endif
							//g_pSolidDraw->SetEntSnapStatus(pFirstLinePart->handle);
							//g_pSolidDraw->SetEntSnapStatus(pOthFirstLinePart->handle);
							fprintf(fp,"%s\n",sError);
							AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)pFirstLinePart->GetPartNo(),pFirstLinePart->handle,pOthFirstLinePart->handle,sError));
							nHits++;
						}
						else if(!pPolyPart->IsEqual(pOthPolyPart,scope,error_info))
						{
#ifdef AFX_TARG_ENU_ENGLISH
							if(pFirstLinePart->GetClassTypeId()==CLS_LINEANGLE)
								sError.Format("0X%X angle and 0X%X angle are different(%s)but they have same piece number",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
							else if(pFirstLinePart->GetClassTypeId()==CLS_LINETUBE)
								sError.Format("0X%X tube and 0X%X tube are different(%s)but they have same piece number",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
							else if(pFirstLinePart->GetClassTypeId()==CLS_LINESLOT)
								sError.Format("0X%X U-steel and 0X%X U-steel are different(%s)but they have same piece number",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
							else if(pFirstLinePart->GetClassTypeId()==CLS_LINEFLAT)
								sError.Format("0X%X flat angle and 0X%X flat angle are different(%s)but they have same piece number",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
#else 
							if(pFirstLinePart->GetClassTypeId()==CLS_LINEANGLE)
								sError.Format("0X%X角钢与0X%X角钢并不相同(%s)但编了相同的构件号",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
							else if(pFirstLinePart->GetClassTypeId()==CLS_LINETUBE)
								sError.Format("0X%X钢管与0X%X钢管并不相同(%s)但编了相同的构件号",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
							else if(pFirstLinePart->GetClassTypeId()==CLS_LINESLOT)
								sError.Format("0X%X槽钢与0X%X槽钢并不相同(%s)但编了相同的构件号",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
							else if(pFirstLinePart->GetClassTypeId()==CLS_LINEFLAT)
								sError.Format("0X%X扁钢与0X%X扁钢并不相同(%s)但编了相同的构件号",pFirstLinePart->handle,pOthFirstLinePart->handle,error_info);
#endif
							else
								continue;
							//g_pSolidDraw->SetEntSnapStatus(pFirstLinePart->handle);
							//g_pSolidDraw->SetEntSnapStatus(pOthFirstLinePart->handle);
							fprintf(fp,"%s\n",sError);
							AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)pFirstLinePart->GetPartNo(),pFirstLinePart->handle,pOthFirstLinePart->handle,sError));
							nHits++;
						}
					}
				}
				polylist.pop_stack();
			}
			DisplayProcess((i*100)/(n+poly_n));
		}
		DisplayProcess(100);
#ifdef AFX_TARG_ENU_ENGLISH
		fprintf(fp,"\n %d parts have problem！\n",nHits);
		sError.Format("%d parts have problem！",nHits);
#else 
		fprintf(fp,"\n总计有%d对构件比较有问题！\n",nHits);
		sError.Format("总计有%d对构件比较有问题！",nHits);
#endif
		fclose(fp);
		if(nHits>0)
			WinExec("notepad.exe NoCheck.txt",SW_SHOW);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox("Through part number check!");
#else 
			MessageBox("通过构件编号检查!");
#endif
		
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
#ifdef AFX_TARG_ENU_ENGLISH
		fprintf(fp,"\n %d parts have problem！\n",nHits);
#else 
		fprintf(fp,"\n总计有%d对构件比较有问题！\n",nHits);
#endif
		fclose(fp);
		if(nHits>0)
			WinExec("notepad.exe NoCheck.txt",SW_SHOW);
	}
#endif
}
//修正构件编号
/*void CLDSView::OnCorrectPartNo() 
{
	CLDSLineAngle *pJg,*pCheckJg;
	//CLDSPolyAngle *pHuoQuJg,*pCheckHuoQuJg;
	CLDSPlate *pPlank,*pCheckPlank;
	FILE *fp;
	fp = fopen("NoCorrect.txt","wt");
	int nHits=0;
	for(pJg=Ta.LineAngles.GetFirst();pJg;pJg=Ta.LineAngles.GetNext())
	{
		if(pJg->sub_type==TA_JG_EXPLODE)
			continue;
		else if(pJg->sub_type==TA_JG_VIRTUAL)
			continue;
		else if(pJg->m_bVirtualPart)
			continue;
		else if(strlen(pJg->GetPartNo())<3)
			continue;
		else
			pJg->Unlock();	//如果加锁则解锁
	}
	CString sError;
	for(pPlank=Ta.Plank.GetFirst();pPlank;pPlank=Ta.Plank.GetNext())
	{
		if(pPlank->vertex_list.GetNodeNum()<3)
			continue;
		if(strlen(pPlank->GetPartNo())<3)
			continue;
		else
			pPlank->Unlock();	//如果加锁则解锁
	}
	for(pJg=Ta.LineAngles.GetFirst();pJg;pJg=Ta.LineAngles.GetNext())
	{
		if(pJg->sub_type==TA_JG_EXPLODE)
			continue;
		else if(pJg->sub_type==TA_JG_VIRTUAL)
			continue;
		else if(pJg->m_bVirtualPart)
			continue;
		Ta.LineAngles.push_stack();
		pCheckJg=pJg;
		for(pJg=Ta.LineAngles.GetNext();pJg;pJg=Ta.LineAngles.GetNext())
		{
			if(pJg->huoqu_handle>0x20)	//火曲子角钢
				continue;
			if(strlen(pJg->GetPartNo())==0)
				continue;
			if(stricmp(pCheckJg->GetPartNo(),pJg->GetPartNo())==0)
			{
				if(!pCheckJg->IsEqual(pJg,1.0))
				{
					if( pJg->sPartNo[strlen(pCheckJg->GetPartNo())-1]<'0'
						||pJg->sPartNo[strlen(pCheckJg->GetPartNo())-1]>'9')
						pJg->sPartNo[strlen(pCheckJg->GetPartNo())-1]+=1;
					else
						strcat(pJg->sPartNo,"A");
					sError.Format("0X%X角钢编号:%s-->%s",pJg->handle,(char*)pCheckJg->GetPartNo(),(char*)pJg->GetPartNo());
					//AfxMessageBox(sError);
					g_pSolidDraw->SetEntSnapStatus(pJg->handle);
					g_pSolidDraw->SetEntSnapStatus(pCheckJg->handle);
					fprintf(fp,"%s\n",sError);
					nHits++;
				}
			}
		}
		Ta.LineAngles.pop_stack();
	}
	for(CLDSPolyAngle* pHuoQuJg=Ta.HuoQuJg.GetFirst();pHuoQuJg;pHuoQuJg=Ta.HuoQuJg.GetNext())
	{
		Ta.HuoQuJg.push_stack();
		CLDSPolyAngle* pCheckHuoQuJg=pHuoQuJg;
		for(pHuoQuJg=Ta.HuoQuJg.GetNext();pHuoQuJg;pHuoQuJg=Ta.HuoQuJg.GetNext())
		{
			if(stricmp(pCheckHuoQuJg->GetPartNo(),pHuoQuJg->GetPartNo())==0)
			{
				if(!pCheckHuoQuJg->IsEqual(pHuoQuJg,1.0))
				{
					char sPartNo[20],sCheckPartNo[20];
					strcpy(sPartNo,pHuoQuJg->seg_arr.GetFirst()->GetPartNo());
					strcpy(sCheckPartNo,pCheckHuoQuJg->seg_arr.GetFirst()->GetPartNo());
					if( sPartNo[strlen(sCheckPartNo)-1]<'0'
						||sPartNo[strlen(sCheckPartNo)-1]>'9')
						sPartNo[strlen(sCheckPartNo)-1]+=1;
					else
						strcat(sPartNo,"A");
					pHuoQuJg->SetPartNo(sPartNo);
					sError.Format("0X%X火曲角钢编号:%s-->%s",pHuoQuJg->handle,sCheckPartNo,sPartNo);
					//AfxMessageBox(sError);
					for(pJg=pHuoQuJg->seg_arr.GetFirst();pJg;pJg=pHuoQuJg->seg_arr.GetNext())
						g_pSolidDraw->SetEntSnapStatus(pJg->handle);
					for(pJg=pCheckHuoQuJg->seg_arr.GetFirst();pJg;pJg=pCheckHuoQuJg->seg_arr.GetNext())
						g_pSolidDraw->SetEntSnapStatus(pJg->handle);
					fprintf(fp,"%s\n",sError);
					nHits++;
				}
				else
					pCheckHuoQuJg->Lock();
			}
		}
		Ta.HuoQuJg.pop_stack();
	}
	for(pPlank=Ta.Plank.GetFirst();pPlank;pPlank=Ta.Plank.GetNext())
	{
		pCheckPlank=pPlank;
		if(pPlank->vertex_list.GetNodeNum()<3)
			continue;	//单螺栓连接板(其实无板,只有螺栓)
		if(strlen(pPlank->GetPartNo())==0)
			continue;
		Ta.Plank.push_stack();
		for(pPlank=Ta.Plank.GetNext();pPlank;pPlank=Ta.Plank.GetNext())
		{
			if(stricmp(pCheckPlank->GetPartNo(),pPlank->GetPartNo())==0)
			{
				if(!pCheckPlank->IsEqual(pPlank,1.0))
				{
					if( pPlank->sPartNo[strlen(pCheckPlank->sPartNo)-1]<'0'
						||pPlank->sPartNo[strlen(pCheckPlank->sPartNo)-1]>'9')
						pPlank->sPartNo[strlen(pCheckPlank->sPartNo)-1]+=1;
					else
						strcat(pPlank->sPartNo,"A");
					sError.Format("0X%X板编号:%s-->%s",pPlank->handle,pCheckPlank->sPartNo,pPlank->sPartNo);
					//AfxMessageBox(sError);
					g_pSolidDraw->SetEntSnapStatus(pPlank->base_node_handle);
					g_pSolidDraw->SetEntSnapStatus(pCheckPlank->base_node_handle);
					fprintf(fp,"%s\n",sError);
					nHits++;
				}
			}
		}
		Ta.Plank.pop_stack();
	}
	fclose(fp);
	if(nHits>0)
		WinExec("notepad.exe NoCorrect.txt",SW_SHOW);
	else
		MessageBox("构件编号不需修正!");
}*/

void CLDSView::OnLocatePartByNo() 
{
	if(m_curAppType!=TA_ASSEMBLE_ENV)
		return;	//构件编辑器中屏蔽该命令快捷键F2 wht 11-05-18
	m_nPrevCommandID=ID_LOCATE_PART_BY_NO;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat Locating Part";
#else 
	m_sPrevCommandName="重复定位构件";
#endif
	/*
	CString sPartNo,sText;
	Input("请输入构件编号:",&sPartNo,'s');
	CLDSPart *pPart;
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->m_bVirtualPart)
			continue;
		else if(sPartNo.CompareNoCase(pPart->GetPartNo())==0)
		{
			if(pPart->GetClassTypeId()==CLS_LINEANGLE)
				sText.Format("此构件为句柄为0X%X的角钢",pPart->handle);
			else if(pPart->GetClassTypeId()==CLS_LINETUBE)
				sText.Format("此构件为句柄为0X%X的钢管",pPart->handle);
			else if(pPart->GetClassTypeId()==CLS_LINESLOT)
				sText.Format("此构件为句柄为0X%X的槽钢",pPart->handle);
			else if(pPart->GetClassTypeId()==CLS_PLATE)
				sText.Format("此构件为句柄为0X%X的钢板",pPart->handle);
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				sText.Format("此构件为句柄为0X%X的参数化钢板",pPart->handle);
			MessageBox(sText);
			return;
		}
	}
	AfxMessageBox("未找到该构件!");
	*/
	//切换到实体显示状态
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n<=0)
		return;
	CLDSPart *pSelPart=console.FromPartHandle(id_arr[0]);
#ifndef __ENABLE_XHSTRING_LENGTH_PROPERTY_
	if(pSelPart==NULL||pSelPart->GetPartNo().Length()==0)
#else
	if(pSelPart==NULL||strlen(pSelPart->GetPartNo())==0)
#endif
		return;
	if(n>1)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pSelPart->handle);
	}
	//根据选中项设置默认件号，并查找对应构件
	CXhChar16 sPartNo=pSelPart->GetPartNo();
	for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if( pPart->GetClassTypeId()==CLS_BOLT||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE||
			pPart->GetClassTypeId()==CLS_POLYJOINT)//||pPart->m_bVirtualPart)
			continue;	//螺栓无编号不参预构件号检查
		if(pPart->handle==pSelPart->handle)
			continue;
		else if(stricmp(sPartNo,pPart->GetPartNo())==0)
		{
			g_pSolidDraw->SetEntSnapStatus(pPart->handle);
			g_pSolidSet->SetOperType(OPER_ROTATE);	//设置操作类型为旋转
			f3dPoint org=pPart->ucs.origin;
			g_pSolidSet->SetRotOrg(org);
			g_pSolidSet->SetZoomStyle(ROT_CENTER);	//设置为指定旋转中心类型
		}
	}
	UpdatePropertyPage();
}

//在AutoCad R14中进行绘图
void CLDSView::OnDrawTaInAcad() 
{
	m_nPrevCommandID=ID_DRAW_TA_IN_ACAD;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="重复Acad绘图";
#else 
	m_sPrevCommandName="Repeat Acad Draw";
#endif
#ifdef __LDS_TSA_	//LDS的内力分析模块
	HKEY hKey;
	char acad_path[MAX_PATH]="";
	DWORD dwDataType,dwLength=MAX_PATH;
	RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Xerofox\\LDS\\ACAD",0,KEY_READ,&hKey);
	if(RegQueryValueEx(hKey,_T("ACAD_PATH"),NULL,&dwDataType,(BYTE*)&acad_path[0],&dwLength)== ERROR_SUCCESS)
		m_sAcadPath = acad_path;
#else
		m_sAcadPath = AfxGetApp()->GetProfileString("ACAD","ACAD_PATH","");
#endif
	FILE* fp;
	CString file;
	file = m_sAcadPath.Left(m_sAcadPath.GetLength()-8);
	file+="Acad.rx";
	if((fp=fopen(file,"wt"))==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Failed to create mapping startup files,so it can't start drawing module!");
#else 
		AfxMessageBox("绘图启动文件创建失败,不能启动绘图模块!");
#endif
		return;
	}
	else
	{
#ifdef __LDS_
		fprintf(fp,"%sMapLds.arx\n",APP_PATH);
#elif defined(__TSA_)
		fprintf(fp,"%sMapTsa.arx\n",APP_PATH);
#elif defined(__LMA_)
		fprintf(fp,"%sMapLma.arx\n",APP_PATH);
#else
		fprintf(fp,"%sMapTma.arx\n",APP_PATH);
#endif
		fclose(fp);
	}
	WinExec(m_sAcadPath,SW_SHOW);
	::DeleteFile(file);
}
