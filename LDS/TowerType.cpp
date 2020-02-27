//<LOCALE_TRANSLATE Confirm by hxr/>
// TowerType.cpp: implementation of the CTowerType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TowerType.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "SortFunc.h"
#include "ComparePartNoString.h"
#include "env_def.h"
#include "LibraryQuery.h"
#include "lds_part.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

SCOPE_STRU GetRectFromPoly(fPtList &vertex_list)
{
	int nn;
	CRgn rgn;
	f3dPoint *pPoint;
	SCOPE_STRU scope;
	scope.ClearScope();
	nn = vertex_list.GetNodeNum();
	for(pPoint=vertex_list.GetFirst();pPoint!=NULL;pPoint=vertex_list.GetNext())
		scope.VerifyVertex(*pPoint);
	return scope;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLsLine::CLsLine()
{
	g=0;
	pLsList = new ATOM_LIST<PART_LS*>();
}
CLsLine::~CLsLine()
{
	if(pLsList)
		delete pLsList;
}
PART_LS* CLsLine::AppendPartLsAndSort(PART_LS* pPartLs)
{
	PART_LS* pTmPartLs=NULL;
	for(pTmPartLs=GetFirstPartLs();pTmPartLs;pTmPartLs=GetNextPartLs())
	{
		if(pTmPartLs->x>pPartLs->x)
		{
			pLsList->insert(pPartLs);
			break;
		}
	}
	if(pTmPartLs==NULL)
		pLsList->append(pPartLs);
	return pPartLs;
}
PART_LS* CLsLine::GetFirstPartLs()
{
	PART_LSPTR *pLsPartPtr=pLsList->GetFirst();
	if(pLsPartPtr)
		return *pLsPartPtr;
	else
		return NULL;
}
PART_LS* CLsLine::GetNextPartLs()
{
	PART_LSPTR *pLsPartPtr=pLsList->GetNext();
	if(pLsPartPtr)
		return *pLsPartPtr;
	else
		return NULL;
}
PART_LS* CLsLine::GetPrevPartLs()
{
	PART_LSPTR *pLsPartPtr=pLsList->GetPrev();
	if(pLsPartPtr)
		return *pLsPartPtr;
	else
		return NULL;
}
PART_LS* CLsLine::GetTailPartLs()
{
	PART_LSPTR *pLsPartPtr=pLsList->GetTail();
	if(pLsPartPtr)
		return *pLsPartPtr;
	else
		return NULL;
}
PART_LS* CLsLineSet::AppendPartLsAndSort(PART_LS *pPartLs,double epsilon)
{
	CLsLine *pLsLine;
	for(pLsLine=lslineset.GetFirst();pLsLine;pLsLine=lslineset.GetNext())
	{
		if(fabs(pPartLs->y-pLsLine->g)<epsilon)
		{
			pLsLine->AppendPartLsAndSort(pPartLs);
			break;
		}
	}
	if(pLsLine==NULL)
	{
		pLsLine=lslineset.append();
		pLsLine->g = pPartLs->y;
		pLsLine->AppendPartLsAndSort(pPartLs);
	}
	return pPartLs;
}
struct LSLINE_ORDER{int iOrder;double g;};
static int compare_lsline_order_func(const LSLINE_ORDER& order1,const LSLINE_ORDER& order2)
{
	if(order1.g>order2.g)
		return 1;
	else if(order1.g<order2.g)
		return -1;
	else
		return 0;
}
int PART::GetProcessNotes(char *notes,BOOL bIncKaiHeAngle/*=FALSE*/)
{
	char ss[300]="";
#ifdef AFX_TARG_ENU_ENGLISH
	if(bZhiWan)			//是否需要制弯
		sprintf(ss,"%s blending",ss);
	if(bWeldPart)		//whether weld
		sprintf(ss,"%s weld",ss);
	if(bNeedFillet)		//slope neck
		sprintf(ss,"%s slope neck",ss);
	if(startProcess.type==PROCESSTYPE_FLD&&endProcess.type==PROCESSTYPE_FLD)
		sprintf(ss,"%s two end slope neck",ss);
	else if(startProcess.type==PROCESSTYPE_FLD||endProcess.type==PROCESSTYPE_FLD)
		sprintf(ss,"%s one end slope neck",ss);
	if(bCutAngle)		//whether cut angle
		sprintf(ss,"%s angle cut",ss);
	if(bCutBer)			//Whether cut back
		sprintf(ss,"%s cut back",ss);
	if(bCutRoot)		//whether cut root
		sprintf(ss,"%s cut root",ss);
	if(bKaiJiao)		//是否需要开角
	{
		if(bIncKaiHeAngle)	//包括开角度数 wht 11-05-24
			sprintf(ss,"%s open angle(%.1f°)",ss,wing_angle-90);
		else 
			sprintf(ss,"%s open angle",ss);
	}
	if(bHeJiao)			//是否需要合角
	{
		if(bIncKaiHeAngle)	//包括合角度数 wht 11-05-24
			sprintf(ss,"%s close angle(%.1f°)",ss,90-wing_angle);
		else 
			sprintf(ss,"%s close angle",ss);
	}
	if(nPushFlat>0)		//whether push flat
	{
		if(nPushFlat==3)
			sprintf(ss,"%s middle push flat",ss);
		else
			sprintf(ss,"%s push flat",ss);
	}
	if(bRollEdge)
		sprintf(ss,"%s roll edge",ss);
	if(bHasFootNail)
		sprintf(ss,"%s has footnail",ss);
#else
	if(bZhiWan)			//是否需要制弯
		sprintf(ss,"%s 制弯",ss);
	if(bWeldPart)		//是否需要焊接
		sprintf(ss,"%s 焊接",ss);
	if(bNeedFillet)		//需要坡口
		sprintf(ss,"%s 坡口",ss);
	if(startProcess.type==PROCESSTYPE_FLD&&endProcess.type==PROCESSTYPE_FLD)
		sprintf(ss,"%s 两端坡口",ss);
	else if(startProcess.type==PROCESSTYPE_FLD||endProcess.type==PROCESSTYPE_FLD)
		sprintf(ss,"%s 一端坡口",ss);
	if(bCutAngle)		//是否切角
		sprintf(ss,"%s 切角",ss);
	if(bCutBer)			//是否需要铲背
		sprintf(ss,"%s 铲背",ss);
	if(bCutRoot)		//是否需要清根
		sprintf(ss,"%s 清根",ss);
	if(bKaiJiao)		//是否需要开角
	{
		if(bIncKaiHeAngle)	//包括开角度数 wht 11-05-24
			sprintf(ss,"%s 开角(%.1f°)",ss,wing_angle-90);
		else 
			sprintf(ss,"%s 开角",ss);
	}
	if(bHeJiao)			//是否需要合角
	{
		if(bIncKaiHeAngle)	//包括合角度数 wht 11-05-24
			sprintf(ss,"%s 合角(%.1f°)",ss,90-wing_angle);
		else 
			sprintf(ss,"%s 合角",ss);
	}
	if(nPushFlat>0)		//是否压扁
	{
		if(nPushFlat==3)
			sprintf(ss,"%s 中间压扁",ss);
		else
			sprintf(ss,"%s 压扁",ss);
	}
	if(bRollEdge)
		sprintf(ss,"%s 卷边%.0fmm",ss,fRollHeight);
	if(bHasFootNail)
		sprintf(ss,"%s 带脚钉",ss);
	if(bDianBan)
		sprintf(ss,"%s 垫块",ss);
#endif
	if(notes)
		strcpy(notes,ss);
	return strlen(ss);
}

//获取开口槽类型
//nRuleType 开槽类型编号规则
//nRuleType==0 代号：LRM(左右中) 
//				M位: (钢管中间连接方式) 0.无连接 1.节点板 2.相贯钢管 3.节点板和相贯钢管
//			  L,R位: (钢管两端开槽方式) 1.槽型插板 2.U型插板 3.T型插板 4.X型插板 5.FL 6.相贯 7.平面切割
//nRuleType==1 代号：LMR(左中右) 
//				M位: (钢管中间连接方式) 0.无连接 1.节点板 2.相贯钢管
//			  L,R位: (钢管两端开槽方式) 1.槽型插板 2.U型插板 3.T型插板 4.X型插板 5.FL
int PART::GetNotchType(char *type,int nRuleType)
{
	int nType=-1;
	//平面切割 可能连接有水平法兰、异形法兰或对接相贯
	if(startProcess.type>0&&startProcess.type<8
		&&endProcess.type>0&&endProcess.type<8)
	{	//两端开槽
		if(startProcess.type==endProcess.type)
		{	//两端开槽类型一致
			if(startProcess.type==PROCESSTYPE_CSLOT)
				nType=11;	//支管 两头C型插板
			else if(startProcess.type==PROCESSTYPE_USLOT)
				nType=22;	//支管 两头U型插板
			else if(startProcess.type==PROCESSTYPE_TSLOT)
				nType=33;	//支管 两头T型插板
			else if(startProcess.type==PROCESSTYPE_XSLOT)
				nType=44;	//支管 两头X型插板
			else if(startProcess.IsFL())
				nType=55;	//支管 两头法兰
			else if(nRuleType==0)
			{	//LRM
				if(startProcess.type==PROCESSTYPE_TRANSECT)
					nType=66;	//支管 两头相贯
				else if(startProcess.type==PROCESSTYPE_PLANECUT)
					nType=77;	//支管 两头平面切割
			}
		}
		else if(startProcess.type==PROCESSTYPE_CSLOT||endProcess.type==PROCESSTYPE_CSLOT)
		{	//两端连接不同 一端为槽型插板
			TUBE_PROCESS *pAnotherProcess=&endProcess;
			if(endProcess.type==PROCESSTYPE_CSLOT)
				pAnotherProcess=&startProcess;
			if(pAnotherProcess->type==PROCESSTYPE_USLOT)
				nType=12;	//支管 一端C型插板一端U型插板
			else if(pAnotherProcess->type==PROCESSTYPE_TSLOT)
				nType=13;	//支管 一端C型插板一端T型插板
			else if(pAnotherProcess->type==PROCESSTYPE_XSLOT)
				nType=14;	//支管 一端C型插板一端X型插板
			else if(pAnotherProcess->IsFL())
				nType=15;	//支管 一端C型插板一端法兰
			else if(nRuleType==0)
			{	//LRM
				if(pAnotherProcess->type==PROCESSTYPE_TRANSECT)
					nType=16;	//支管 一端C型插板一端相贯
				else if(pAnotherProcess->type==PROCESSTYPE_PLANECUT)
					nType=17;	//支管 一端C型插板一端平面切割
			}
		}
		else if(startProcess.type==PROCESSTYPE_USLOT||endProcess.type==PROCESSTYPE_USLOT)
		{	//两端连接不同 一端为U型插板
			TUBE_PROCESS *pAnotherProcess=&endProcess;
			if(endProcess.type==PROCESSTYPE_USLOT)
				pAnotherProcess=&startProcess;
			if(pAnotherProcess->type==PROCESSTYPE_TSLOT)
				nType=23;	//支管 一端U型插板一端T型插板
			else if(pAnotherProcess->type==PROCESSTYPE_XSLOT)
				nType=24;	//支管 一端U型插板一端X型插板
			else if(pAnotherProcess->IsFL())
				nType=25;	//支管 一端U型插板一端法兰
			else if(nRuleType==0)
			{	//LRM
				if(pAnotherProcess->type==PROCESSTYPE_TRANSECT)
					nType=26;	//支管 一端U型插板一端相贯
				else if(pAnotherProcess->type==PROCESSTYPE_PLANECUT)
					nType=27;	//支管 一端U型插板一端平面切割
			}
		}
		else if(startProcess.type==PROCESSTYPE_TSLOT||endProcess.type==PROCESSTYPE_TSLOT)
		{	//两端连接不同 一端为T型插板
			TUBE_PROCESS *pAnotherProcess=&endProcess;
			if(endProcess.type==PROCESSTYPE_TSLOT)
				pAnotherProcess=&startProcess;
			if(pAnotherProcess->type==PROCESSTYPE_XSLOT)
				nType=34;	//支管 一端T型插板一端X型插板
			else if(pAnotherProcess->IsFL())
				nType=35;	//支管 一端T型插板一端法兰
			else if(nRuleType==0)
			{	//LRM
				if(pAnotherProcess->type==PROCESSTYPE_TRANSECT)
					nType=36;	//支管 一端T型插板一端相贯
				else if(pAnotherProcess->type==PROCESSTYPE_PLANECUT)
					nType=37;	//支管 一端T型插板一端平面切割
			}
		}
		else if(startProcess.type==PROCESSTYPE_XSLOT||endProcess.type==PROCESSTYPE_XSLOT)
		{	//两端连接不同 一端为X型插板
			TUBE_PROCESS *pAnotherProcess=&endProcess;
			if(endProcess.type==PROCESSTYPE_USLOT)
				pAnotherProcess=&startProcess;
			if(pAnotherProcess->IsFL())
				nType=45;	//支管 一端X型插板一端法兰
			else if(nRuleType==0)
			{	//LRM
				if(pAnotherProcess->type==PROCESSTYPE_TRANSECT)
					nType=46;	//支管 一端X型插板一端相贯
				else if(pAnotherProcess->type==PROCESSTYPE_PLANECUT)
					nType=47;	//支管 一端X型插板一端平面切割
			}
		}
		else if(startProcess.IsFL()||endProcess.IsFL())
		{
			TUBE_PROCESS *pAnotherProcess=&endProcess;
			if(endProcess.IsFL())
				pAnotherProcess=&startProcess;
			if(nRuleType==0)
			{	//LRM
				if(pAnotherProcess->type==PROCESSTYPE_TRANSECT)
					nType=56;	//支管 一端法兰一端相贯
				else if(pAnotherProcess->type==PROCESSTYPE_PLANECUT)
					nType=57;	//支管 一端法兰一端平面切割
			}
		}
		else if(startProcess.type==PROCESSTYPE_TRANSECT||endProcess.type==PROCESSTYPE_TRANSECT)
		{
			TUBE_PROCESS *pAnotherProcess=&endProcess;
			if(endProcess.type==PROCESSTYPE_TRANSECT)
				pAnotherProcess=&startProcess;
			if(nRuleType==0)
			{	//LRM
				if(pAnotherProcess->type==PROCESSTYPE_PLANECUT)
					nType=67;	//支管 一端相贯一端平面切割
			}
		}
		if(nType>0)
		{
			if(nRuleType==0)
			{	//LRM
				if(bHasBranchTube&&bHasNodePlate)
					nType=nType*10+3;	//支管 两头插板(法兰)中间相贯管以及节点板
				else if(bHasBranchTube)
					nType=nType*10+2;	//支管 两头插板(法兰)中间相贯管
				else if(bHasNodePlate)
					nType=nType*10+1;	//支管 两头插板(法兰)中间节点板
			}
			else if(nRuleType==1)
			{
				int nGeWei=0,nShiWei=0;
				nGeWei=nType%10;
				nShiWei=nType-nGeWei;
				if(bHasNodePlate)
					nType=(nShiWei+1)*10+nGeWei;	//支管 两头插板(法兰)中间节点板
				else if(bHasBranchTube)
					nType=(nShiWei+2)*10+nGeWei;	//支管 两头插板(法兰)中间相贯管
				else 
					nType=nShiWei*10+nGeWei;

			}
		}
	}
	//8		连板角钢焊件
	//9		扶手焊件
	//A		爬梯焊件
	//B		其他焊件
	if(type)
		sprintf(type,"%d",nType);
	return nType;
}

//是否需要割开口槽
BOOL PART::IsTubeNotch()
{
	if((startProcess.type>2&&startProcess.type<7)
		||(endProcess.type>2&&endProcess.type<7))
		return TRUE;
	return FALSE;
}
//是否需要割相贯线
BOOL PART::IsTubeTransect()
{
	if(startProcess.type==1||startProcess.type==2
		||endProcess.type==1||endProcess.type==2)
		return TRUE;
	return FALSE;
}
//得到指定孔径的螺栓孔数 bSpecifyScope==TRUE; 指定范围内的螺栓个数
//						 bSpecifyScope==FALSE; 直径=min_d的螺栓个数
int PART::GetLsHoleNum(double min_d,double max_d/*=-1*/,BOOL bHoleD/*=TRUE*/,
					BOOL bSpecifyScope/*=TRUE*/)
{
	int nLsNum=0;
	if(min_d==-1&&max_d==-1)
		return 0;
	for(int i=0;i<nMSumLs;i++)
	{
		double fHoleD=ls_arr[i].d;
		if(bHoleD)
		fHoleD+=ls_arr[i].hole_d_increment;
		if(!bSpecifyScope)
		{	//获得指定直径的螺栓个数
			if(fabs(fHoleD-min_d)<EPS)
				nLsNum++;
		}
		else 
		{	//获得指定直径
			if(min_d==-1&&(max_d>fHoleD||fabs(max_d-fHoleD)<EPS))
				nLsNum++;
			else if(max_d==-1&&(fHoleD>min_d||fabs(max_d-fHoleD)<EPS))
				nLsNum++;
			else if((fHoleD>min_d&&fHoleD<max_d)
				||fabs(fHoleD-min_d)<EPS||fabs(fHoleD-max_d)<EPS)
				nLsNum++;
		}
	}
	return nLsNum;
}
//构件编号
char* PART::GetPartNo(int nMatCharPosType/* =0 */,char cMatSeparator/*=0*/)
{
	CXhChar16 szMaterial("%c",cMaterial);
	if(cQualityLevel!=0)
		szMaterial.Append(cQualityLevel);
	if(nMatCharPosType==1&&cMaterial!=CSteelMatLibrary::Q235BriefMark())
	{
		if(cMatSeparator==0)		//无间隔
			sprintf(sPartNoIncMat,"%s%s",(char*)szMaterial,sPartNo);
		else if(cMatSeparator==' ')	//空格
			sprintf(sPartNoIncMat,"%s %s",(char*)szMaterial,sPartNo);
		else
			sprintf(sPartNoIncMat,"%s%c%s",(char*)szMaterial,cMatSeparator,sPartNo);
	}
	else if(nMatCharPosType==2&&cMaterial!=CSteelMatLibrary::Q235BriefMark()) 
	{	
		if(cMatSeparator==0)		//无间隔
			sprintf(sPartNoIncMat,"%s%s",sPartNo,(char*)szMaterial);
		else if(cMatSeparator==' ')	//空格
			sprintf(sPartNoIncMat,"%s %s",sPartNo,(char*)szMaterial);
		else
			sprintf(sPartNoIncMat,"%s%c%s",sPartNo,cMatSeparator,(char*)szMaterial);
	}
	else
		StrCopy(sPartNoIncMat,sPartNo,17);
	return sPartNoIncMat;
}
//螺栓孔字符串
typedef struct tagHOLE_INFO
{
	int d;					//螺栓直径
	double hole_d_increment;	//孔增量
	int nCount;				//该直径螺栓个数
}HOLE_INFO;
int PART::GetLsHoleStr(char *hole_str, BOOL bIncUnit/*=TRUE*/)
{
	char sHoleStr[500]="";
	if(nMSumLs<=0)
	{
		if(hole_str)
			strcpy(hole_str,"");
		return 0;
	}
	ATOM_LIST<HOLE_INFO> holeList;
	HOLE_INFO *pHoleInfo=NULL;
	for(int i=0;i<nMSumLs;i++)
	{
		for(pHoleInfo=holeList.GetFirst();pHoleInfo;pHoleInfo=holeList.GetNext())
		{
			if(fabs(pHoleInfo->d+pHoleInfo->hole_d_increment-ls_arr[i].d-ls_arr[i].hole_d_increment)<EPS)
			{
				pHoleInfo->nCount++;
				break;
			}
		}
		if(pHoleInfo==NULL)
		{
			pHoleInfo=holeList.append();
			pHoleInfo->d=ls_arr[i].d;
			pHoleInfo->hole_d_increment=ls_arr[i].hole_d_increment;
			pHoleInfo->nCount=1;
		}
	}
	int nCount=0;
	for(pHoleInfo=holeList.GetFirst();pHoleInfo;pHoleInfo=holeList.GetNext())
	{
		char sCurHole[50]="",sD[50]="";
		sprintf(sD,"%f",pHoleInfo->d+pHoleInfo->hole_d_increment);
		SimplifiedNumString(sD);
		if(bIncUnit)
			sprintf(sCurHole,"%smm*%d",sD,pHoleInfo->nCount);
		else 
			sprintf(sCurHole,"%s*%d",sD,pHoleInfo->nCount);
		strcat(sHoleStr,sCurHole);
		if(nCount<holeList.GetNodeNum()-1)
			strcat(sHoleStr,",");
		nCount++;
	}
	if(hole_str)
		strcpy(hole_str,sHoleStr);
	return strlen(sHoleStr);
}

int PART::GetLsStr(char *ls_str)
{
	CLsLineSet set;
	int i,n;
	if(nMSumLs<=0)
	{
		if(ls_str)
			strcpy(ls_str,"");
		return 0;
	}
	for(i=0;i<nMSumLs;i++)
		set.AppendPartLsAndSort(&ls_arr[i],0.5);
	n = set.lslineset.GetNodeNum();
	LSLINE_ORDER *order_arr=new LSLINE_ORDER[n];
	for(i=0;i<n;i++)
	{
		order_arr[i].iOrder=i;
		order_arr[i].g = set.lslineset[i].g;
	}
	CBubbleSort<LSLINE_ORDER>::BubSort(order_arr,n,compare_lsline_order_func,false);
	char ss[3600]="";
	if(set.lslineset[order_arr[0].iOrder].g>0)
		strcpy(ss,"X: ");
	bool bInitWingYLs=false;
	for(i=0;i<n;i++)
	{
		CLsLine *pLsLine;
		PART_LS *pPartLs;
		char cLsDia='0';
		if(set.lslineset[order_arr[i].iOrder].g>0)
		{
			pLsLine=&set.lslineset[order_arr[i].iOrder];
			sprintf(ss,"%s  [%.0f]",ss,pLsLine->g);
			for(pPartLs=pLsLine->GetFirstPartLs();pPartLs;pPartLs=pLsLine->GetNextPartLs())
			{
				double x = fto_halfi(pPartLs->x);
				char ch;
				if(pPartLs->d==12)
					ch='!';
				else if(pPartLs->d==16)
					ch='*';
				else if(pPartLs->d==20)
					ch='#';
				else if(pPartLs->d==24)
					ch='$';
				else
					ch='?';
				if(cLsDia!=ch)
					cLsDia=ch;
				else
					ch=' ';
				if((x-(int)x)!=0)
					sprintf(ss,"%s  %C%.1f",ss,ch,x);
				else
					sprintf(ss,"%s  %C%.0f",ss,ch,x);
			}
		}
		else
		{
			if(!bInitWingYLs)
			{
				if(strlen(ss)>0)
					sprintf(ss,"%s    \nY: ",ss);
				else
					strcpy(ss,"Y: ");
				bInitWingYLs=TRUE;
			}
			pLsLine=&set.lslineset[order_arr[i].iOrder];
			sprintf(ss,"%s  [%.0f]",ss,-pLsLine->g);
			for(pPartLs=pLsLine->GetFirstPartLs();pPartLs;pPartLs=pLsLine->GetNextPartLs())
			{
				double x = fto_halfi(pPartLs->x);
				char ch;
				if(pPartLs->d==12)
					ch='!';
				else if(pPartLs->d==16)
					ch='*';
				else if(pPartLs->d==20)
					ch='#';
				else if(pPartLs->d==24)
					ch='$';
				else
					ch='?';
				if(cLsDia!=ch)
					cLsDia=ch;
				else
					ch=' ';
				if((x-(int)x)!=0)
					sprintf(ss,"%s  %C%.1f",ss,ch,x);
				else
					sprintf(ss,"%s  %C%.0f",ss,ch,x);
			}
		}
	}

	delete []order_arr;
	if(ls_str)
		strcpy(ls_str,ss);
	return strlen(ss);
}

long CLsRecord::GetLsPadCount()		//获取螺栓垫圈数量 
{
	if(typels0_pad1==1)
		return nPart;
	else
		return 0;
}
long CLsRecord::GetCommonLsCount()		//获取普通螺栓数量
{
	if(typels0_pad1==1)
		return 0;
	else if(bFootNail)
		return 0;
	else
		return nPart-nTwoCap-nBurglarproof;	//是双帽就不是防盗螺栓
}
long CLsRecord::GetTwoCapLsCount()			//获取双帽螺栓数量
{
	if(typels0_pad1==1)
		return 0;
	else
		return nTwoCap;
}
long CLsRecord::GetCommonNailCount()		//获取普通脚钉数量
{
	if(typels0_pad1==1)
		return 0;
	else if(bFootNail)
		return nPart-nTwoCap;
	else
		return 0;
}
long CLsRecord::GetBurglarproofLsCount()	//获取防盗螺栓数量
{
	if(typels0_pad1==1)
		return 0;
	else if(bFootNail)
		return 0;
	else
		return nBurglarproof;
}
long CLsRecord::GetBurglarproofNailCount()	//获取防盗脚钉数量
{
	if(typels0_pad1==1)
		return 0;
	else if(bFootNail)
		return nBurglarproof;
	else
		return 0;
}
int CLsRecord::AddPart(BITFLAG flagBolt,int add_num/*=1*/)
{
	nPart+=add_num;
	if(flagBolt.IsHasFlag(CLDSBolt::TWO_CAP))
		nTwoCap+=add_num;
	if(flagBolt.IsHasFlag(CLDSBolt::ANTI_THEFT))
		nBurglarproof+=add_num;
	if(flagBolt.IsHasFlag(CLDSBolt::ANTI_LOOSE))
		nSpringPad+=add_num;
	double unit_weight=0;
	if(typels0_pad1==1)
		unit_weight=FindPadWei(L,d);
	else
	{	//根据螺栓实际功用,获取螺栓规格系列号
		CLsFamily *pLsFamily=CLsLibrary::FromHandle(this->hFamily);
		if(pLsFamily==NULL)
			pLsFamily=CLsLibrary::DefaultFamily();
		long hLsFamily=pLsFamily->GetFamilyHandle();
		if(flagBolt.IsHasFlag(CLDSBolt::ANTI_LOOSE) && pLsFamily->m_hMappingAntiLooseFamily>0)
			hLsFamily=pLsFamily->m_hMappingAntiLooseFamily;	//防松螺栓系列
		else if(flagBolt.IsHasFlag(CLDSBolt::ANTI_THEFT) && pLsFamily->m_hMappingAntiTheftFamily>0)
			hLsFamily=pLsFamily->m_hMappingAntiTheftFamily;	//防盗螺栓系列
		LS_XING_HAO *pXingHao=globalLibrary.FindLsGuiGeByValidLen(d,L,hLsFamily);
		if(pXingHao)
		{
			L0_downlim=pXingHao->down_lim;
			L0_uplim=pXingHao->up_lim;
			no_thread_len=pXingHao->no_thread_len;
			unit_weight=pXingHao->weight;
		}
	}
	//计算总重量时应考虑统材系数
	sum_weight+=unit_weight*add_num;
	return nPart;
}
//0.|普通|1.防松|2.防盗
double CLsRecord::GetUnitWeight(BYTE ciLsFlag/*=0*/)
{
	CLsFamily *pLsFamily=CLsLibrary::FromHandle(this->hFamily);
	if(pLsFamily==NULL)
		pLsFamily=CLsLibrary::DefaultFamily();
	long hLsFamily=pLsFamily->GetFamilyHandle();
	if(ciLsFlag==1 && pLsFamily->m_hMappingAntiLooseFamily>0)
		hLsFamily=pLsFamily->m_hMappingAntiLooseFamily;	//防松螺栓系列
	else if(ciLsFlag==2 && pLsFamily->m_hMappingAntiTheftFamily>0)
		hLsFamily=pLsFamily->m_hMappingAntiTheftFamily;	//防盗螺栓系列
	return globalLibrary.GetLsWeight(d,L,hLsFamily);
}
void CStatModule::GetIncSegStr(CString &ss)
{
	//1,对段号进行排序
	int n=segNoHashList.GetNodeNum();
	SEGI *seg_i_arr = new SEGI[n];
	int i=0;
	for(SEGI *pSegI=segNoHashList.GetFirst();pSegI;pSegI=segNoHashList.GetNext())
	{
		if(i>0)
		{
			int j;
			for(j=0;j<i;j++)
			{
				if(*pSegI<seg_i_arr[j])
				{
					memmove(&seg_i_arr[j+1],&seg_i_arr[j],(n-j-1)*sizeof(int));
					seg_i_arr[j]=*pSegI;
					break;
				}
			}
			if(j==i)
				seg_i_arr[i]=*pSegI;
		}
		else
			seg_i_arr[i]=*pSegI;
		i++;
	}
	//2,得到段号字符串
	ss.Empty();	//清空原字符串
	CXhChar200 sSegStr;
	SEGI startSegI=-1,endSegI=-1;
	for(int i=0;i<n;i++)
	{
		int segI = seg_i_arr[i].iSeg;
		if(startSegI==-1)
			startSegI = segI;
		else if(endSegI==-1&&segI==startSegI+1)
			endSegI = segI;
		else if(endSegI>-1&&segI==endSegI+1)
			endSegI = segI;
		else 
		{
			if(sSegStr.GetLength()>0)
				sSegStr.Append(",");
			if(startSegI<endSegI)	//存在区间段
				sSegStr.Printf("%s%s-%s",(char*)sSegStr,(char*)startSegI.ToString(),(char*)endSegI.ToString());
			else if(startSegI>-1)
				sSegStr.Printf("%s%s",(char*)sSegStr,(char*)startSegI.ToString());
			startSegI = segI;
			endSegI = -1;
		}
	}
	//将最后一段添加到字符串中
	if(sSegStr.GetLength()>0)
		sSegStr.Append(",");
	if(startSegI<endSegI)	//存在区间段
		sSegStr.Printf("%s%s-%s",(char*)sSegStr,(char*)startSegI.ToString(),(char*)endSegI.ToString());
	else if(startSegI>-1)
		sSegStr.Printf("%s%s",(char*)sSegStr,(char*)startSegI.ToString());
	ss.Format("%s",(char*)sSegStr);
	//
	delete []seg_i_arr;
}

int CTowerType::CustomerSerial=0;	//客户定制标识序号
void CTowerType::SummarizeOneBolt(CLDSBolt *pBolt,char *limit_str)
{
	if (pBolt->IsShadowObject())
		pBolt->ShadowDesign();
	CString sGrade,sGuiGe;
	if(pBolt->m_bVirtualPart)	//副本忽略掉
		return;
	CLsFamily *pFamily = CLsLibrary::FromHandle(pBolt->m_hFamily);
	if (pFamily == NULL)
		pFamily = CLsLibrary::DefaultFamily();
	short hFamily = (short)pFamily->GetFamilyHandle();
	if(m_bStatLsBySegI)	//按段统计螺栓
	{
		CLsRecord *pLsRec=NULL;
		SEGI seg_arr[100]={SEGI()};
		int seg_n=0;
		seg_arr[0]=pBolt->iSeg;	//赋值基础统材段号
		for(pLsRec=ls_recordset.GetFirst();pLsRec;pLsRec=ls_recordset.GetNext())
		{
			if(!pBolt->IsStatSeg(pLsRec->iSeg))	//考虑螺栓所从属的段号
				continue;
			else if( pLsRec->typels0_pad1==0&&stricmp(pLsRec->grade,pBolt->Grade())==0&&
				pLsRec->d==pBolt->get_d()&&pLsRec->L==pBolt->get_L()&&pLsRec->hFamily== hFamily)
			{
				pLsRec->AddPart(pBolt->dwSafeFlag,pBolt->m_uStatMatCoef);
				break;
			}
		}
		if(pLsRec==NULL)
		{	//新记录
			pLsRec=ls_recordset.append();
			pLsRec->hFamily= hFamily;
			pLsRec->d=pBolt->get_d();
			pLsRec->L=(long)pBolt->get_L();
			strcpy(pLsRec->grade,pBolt->Grade());
			pLsRec->iSeg=seg_arr[0];	//段号属性
			pLsRec->bFootNail=pBolt->IsFootNail();
			pLsRec->AddPart(pBolt->dwSafeFlag,pBolt->m_uStatMatCoef);
			//处理螺栓多段统计时多条分段记录问题
			seg_n=pBolt->GetStatSegArr(seg_arr,100);
			for(int i=1;i<seg_n;i++)
			{
				CLsRecord *pAddLsRec=ls_recordset.append();
				*pAddLsRec=*pLsRec;
				pAddLsRec->iSeg=seg_arr[i];
			}
		}
		//垫圈和垫板的统计
		if(pBolt->DianQuan.N>0)
		{	//螺栓上带有垫圈
			for(pLsRec=ls_recordset.GetFirst();pLsRec;pLsRec=ls_recordset.GetNext())
			{
				if(pLsRec->typels0_pad1==0)
					continue;
				else if( m_bStatLsBySegI&&pLsRec->iSeg.iSeg!=pBolt->iSeg.iSeg)	//考虑螺栓所从属的段号
					continue;
				else if(pLsRec->d==pBolt->get_d()&&pLsRec->L==pBolt->DianQuan.thick)
				{
					pLsRec->AddPart(0,pBolt->DianQuan.N);
					break;
				}
				else
					continue;
			}
			if(pLsRec==NULL)
			{
				pLsRec=ls_recordset.append();
				pLsRec->typels0_pad1=1;
				pLsRec->iSeg=seg_arr[0];	//段号属性
				pLsRec->d=pBolt->get_d();
				pLsRec->L=pBolt->DianQuan.thick;
				//pLsRec->unit_weight=FindPadWei(pLsRec->L,pLsRec->d);
				pLsRec->AddPart((DWORD)0,pBolt->DianQuan.N);
				//处理螺栓多段统计时多条分段记录问题
				if(seg_n<1)
					seg_n=pBolt->GetStatSegArr(seg_arr,100);
				for(int i=1;i<seg_n;i++)
				{
					CLsRecord *pAddLsRec=ls_recordset.append();
					*pAddLsRec=*pLsRec;
					pAddLsRec->iSeg=seg_arr[i];
				}
			}
		}
	}
	else	//按呼高统计螺栓
	{
		for(int iNo=FindAdaptNo(limit_str);iNo>0;iNo=FindAdaptNo(NULL))
		{
			CLsRecord *pLsRec=NULL;
			for(pLsRec=ls_recordset.GetFirst();pLsRec;pLsRec=ls_recordset.GetNext())
			{
				if( pLsRec->typels0_pad1==0&&pLsRec->iModuleNo==iNo&&stricmp(pLsRec->grade,pBolt->Grade())==0&&
					pLsRec->d==pBolt->get_d()&&pLsRec->L==pBolt->get_L()&&pLsRec->hFamily== hFamily)
				{
					pLsRec->AddPart(pBolt->dwSafeFlag,pBolt->m_uStatMatCoef);
					break;
				}
			}
			if(pLsRec==NULL)
			{	//新记录
				pLsRec=ls_recordset.append();
				pLsRec->iModuleNo=iNo;
				int iOrder=1;
				CLDSModule *pModule=&m_pTower->Module[iNo-1];
				strcpy(pLsRec->sModule,pModule->description);
				pLsRec->hFamily= hFamily;
				pLsRec->d=pBolt->get_d();
				pLsRec->L=(long)pBolt->get_L();
				strcpy(pLsRec->grade,pBolt->Grade());
				pLsRec->bFootNail=pBolt->IsFootNail();
				pLsRec->AddPart(pBolt->dwSafeFlag,pBolt->m_uStatMatCoef);
			}
			//垫圈和垫板的统计
			if(pBolt->DianQuan.N>0)
			{	//螺栓上带有垫圈
				for(pLsRec=ls_recordset.GetFirst();pLsRec;pLsRec=ls_recordset.GetNext())
				{
					if(pLsRec->typels0_pad1==0)
						continue;
					else if(pLsRec->iModuleNo==iNo&&pLsRec->d==pBolt->get_d()&&pLsRec->L==pBolt->DianQuan.thick)
					{
						pLsRec->AddPart((DWORD)0,pBolt->DianQuan.N);
						break;
					}
					else
						continue;
				}
				if(pLsRec==NULL)
				{
					pLsRec=ls_recordset.append();
					CLDSModule *pModule=&m_pTower->Module[iNo-1];
					strcpy(pLsRec->sModule,pModule->description);
					pLsRec->iModuleNo=iNo;
					pLsRec->typels0_pad1=1;
					pLsRec->iSeg=pBolt->iSeg;
					pLsRec->d=pBolt->get_d();
					pLsRec->L=pBolt->DianQuan.thick;
					//pLsRec->unit_weight=FindPadWei(pLsRec->L,pLsRec->d);
					pLsRec->AddPart((DWORD)0,pBolt->DianQuan.N);
				}
			}
		}
	}
}
void CTowerType::SummarizeParts()
{
	CLDSPart *pPart;
	POLYPARTLIST polylist;
	if(m_pModel->IsTowerModel())
		((CTower*)m_pModel)->RetrievePolyPartList(polylist);
	int i=0;
	for(pPart=m_pModel->EnumPartFirst();pPart!=NULL;pPart=m_pModel->EnumPartNext(),i++)
	{
		if(pPart->m_bVirtualPart)	//隐含角钢不统计
			continue;
		else if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			continue;	//材料汇总时不统计组合角钢(单线)重量 wht 11-03-11
		else if(pPart->IsLinePart())
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
			if(pLinePart->bHuoQuStart||pLinePart->bHuoQuEnd)
				continue;	//火曲子构件由火曲构件进行统计
		}
		BOOL pushed=m_pModel->PushPartStack();
		SummarizeOnePart(pPart);
		m_pModel->PopPartStack();
	}
	for(CLDSPolyPart *pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
		SummarizeOnePart(pPolyPart);
}
void CTowerType::SummarizeOneLineAngle(CLDSLineAngle *pLineAngle,char *limit_str)
{
	PART *pRec=NULL;
	//焊接父构件
	for(pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
	{
		if(stricmp(pRec->GetPartNo(),pLineAngle->GetPartNo())==0)
		{
			if(!pRec->bCutRoot&&pLineAngle->m_bCutRoot)
				pRec->bCutRoot=pLineAngle->m_bCutRoot;
			if(!pRec->bCutBer&&pLineAngle->m_bCutBer)
				pRec->bCutBer=pLineAngle->m_bCutBer;
			//存在合法焊接父构件或焊接属性为TRUE
			if(!pRec->bWeldPart&&pLineAngle->IsWeldPart())
				pRec->bWeldPart=TRUE;
			pRec->AddPart(pLineAngle->m_uStatMatCoef);
			if(strlen(pRec->sNotes)==0&&strlen(pLineAngle->sNotes)>0)
				strcpy(pRec->sNotes,pLineAngle->sNotes);
			SummarizePartByModule(limit_str,pRec->iSeg);
			break;
		}
	}
	if(pRec==NULL)
	{
		pRec=part_recordset.append();
		pRec->SetPartNo(pLineAngle->GetPartNo());
		pRec->part_type=STEELTYPE_ANGLE;
		pRec->wide = pLineAngle->GetWidth();
		pRec->thick = pLineAngle->GetThick();
		pRec->length=pLineAngle->GetLength();
		pRec->y_wing_wide=pLineAngle->GetWidthWingY();
		pLineAngle->GetSpecification(pRec->sSpec,FALSE);
		//sprintf(pRec->sSpec,"L%.f*%.f",pRec->wide,pRec->thick);
		pRec->cMaterial=pLineAngle->cMaterial;
		pRec->cQualityLevel=pLineAngle->m_cQualityLevel;
		pRec->iSeg=pLineAngle->iSeg;
		pRec->fPieceNetWeight = pRec->fPieceMinWeight = pLineAngle->GetWeight();//pLineAngle->GetLength()*GetJgUnitLenWei(pLineAngle->GetWidth(),pLineAngle->GetThick())/1000;
		pRec->nM16Ls=pLineAngle->GetLsCount(16);
		pRec->nM20Ls=pLineAngle->GetLsCount(20);
		pRec->nM24Ls=pLineAngle->GetLsCount(24);
		pRec->nMXLs=pLineAngle->GetLsCount()-pRec->nM16Ls-pRec->nM20Ls-pRec->nM24Ls;
		pRec->bZhiWan=pLineAngle->bHuoQuStart||pLineAngle->bHuoQuEnd;
		pRec->bHasFootNail=pLineAngle->IsHasFootNail();
		//pRec->bCutAngle=pLineAngle->IsCutAngleOrWing();
		if(pLineAngle->IsCutAngleOrWing())
		{
			pRec->bCutAngle=1;//TRUE;
			if( (pLineAngle->cut_wing[0]>0&&pLineAngle->cut_wing_para[0][1]>0)||
				(pLineAngle->cut_wing[1]>0&&pLineAngle->cut_wing_para[1][1]>0))	//背切肢
				pRec->bCutAngle=2;	//BC模式切两刀（常熟提供格式要求）
		}
		else
			pRec->bCutAngle=0;//FALSE;
		pRec->wing_angle=pLineAngle->GetDecWingAngle();
		pRec->bCutRoot=pLineAngle->m_bCutRoot;
		pRec->bCutBer=pLineAngle->m_bCutBer;
		//存在合法焊接父构件或焊接属性为TRUE
		if(pLineAngle->IsWeldPart())
			pRec->bWeldPart=TRUE;
		if(pLineAngle->start_push_flat_x1_y2>0&&pLineAngle->end_push_flat_x1_y2>0)
			pRec->nPushFlat=2;	//pRec->bPushFlat=TRUE;
		else if(pLineAngle->start_push_flat_x1_y2>0||pLineAngle->end_push_flat_x1_y2>0)
			pRec->nPushFlat=1;	
		else if(pLineAngle->mid_push_flat_x1_y2>0)
			pRec->nPushFlat=3;
		else 
			pRec->nPushFlat=0;
		//使用用户设置的开合角阈值判断角钢是否需要进行开合角 wht 11-05-06
		if(pLineAngle->GetDecWingAngle()-90>globalLibrary.GetJgKaiHeJiaoThreshold()&&pLineAngle->GetKaiHeWingX0Y1()!=-1)
			pRec->bKaiJiao=TRUE;
		else if(90-pLineAngle->GetDecWingAngle()>globalLibrary.GetJgKaiHeJiaoThreshold()&&pLineAngle->GetKaiHeWingX0Y1()!=-1)
			pRec->bHeJiao=TRUE;
		if(strlen(pLineAngle->sNotes)>0)
			strcpy(pRec->sNotes,pLineAngle->sNotes);
		pRec->cut_wing[0] = pLineAngle->cut_wing[0];
		pRec->cut_wing[1] = pLineAngle->cut_wing[1];
		pRec->cut_wing_para[0][0] = (int)pLineAngle->cut_wing_para[0][0];
		pRec->cut_wing_para[0][1] = (int)pLineAngle->cut_wing_para[0][1];
		pRec->cut_wing_para[0][2] = (int)pLineAngle->cut_wing_para[0][2];
		pRec->cut_wing_para[1][0] = (int)pLineAngle->cut_wing_para[1][0];
		pRec->cut_wing_para[1][1] = (int)pLineAngle->cut_wing_para[1][1];
		pRec->cut_wing_para[1][2] = (int)pLineAngle->cut_wing_para[1][2];
		pRec->cut_angle[0][0] = (int)pLineAngle->cut_angle[0][0];
		pRec->cut_angle[1][0] = (int)pLineAngle->cut_angle[1][0];
		pRec->cut_angle[2][0] = (int)pLineAngle->cut_angle[2][0];
		pRec->cut_angle[3][0] = (int)pLineAngle->cut_angle[3][0];
		pRec->cut_angle[0][1] = (int)pLineAngle->cut_angle[0][1];
		pRec->cut_angle[1][1] = (int)pLineAngle->cut_angle[1][1];
		pRec->cut_angle[2][1] = (int)pLineAngle->cut_angle[2][1];
		pRec->cut_angle[3][1] = (int)pLineAngle->cut_angle[3][1];
		
		pRec->nMSumLs = pLineAngle->GetLsCount();
		UCS_STRU ucs;
		pLineAngle->getUCS(ucs);
		pRec->ls_arr = new PART_LS[pRec->nMSumLs];
		int i=0;
		for(CLsRef *pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
		{
			int X0_Y1;
			pLineAngle->IsLsInWing(pLsRef->GetLsPtr(),&X0_Y1);
			double g = pLineAngle->GetLsG(pLsRef);
			f3dPoint pt = (*pLsRef)->ucs.origin;
			coord_trans(pt,ucs,FALSE);
			pRec->ls_arr[i].d = (*pLsRef)->get_d();
			pRec->ls_arr[i].hole_d_increment = (*pLsRef)->hole_d_increment;
			pRec->ls_arr[i].x = pLineAngle->startOdd()+pt.z;
			if(X0_Y1==0)	//X肢
				pRec->ls_arr[i].y = -g;
			else if(X0_Y1==1)	//Y肢
				pRec->ls_arr[i].y = g;
			else	//错误数据
				pRec->ls_arr[i].y = 0;
			i++;
		}
		pRec->AddPart(pLineAngle->m_uStatMatCoef);
		SummarizePartByModule(limit_str,pRec->iSeg);
	}
}
void CTowerType::SummarizeOneLineTube(CLDSLineTube *pTube,char *limit_str)
{
	PART *pRec=NULL;
	//焊接父构件
	CLDSPart *pFatherPart=m_pModel->FromPartHandle(pTube->m_hPartWeldParent);
	BOOL bHasNodePlate=FALSE,bHasBranchTube=FALSE;
	BOOL bPush=m_pModel->PushPartStack();
	for(CLDSPart *pSonPart=m_pModel->EnumPartFirst();pSonPart;pSonPart=m_pModel->EnumPartNext())
	{
		if(!bHasBranchTube&&pSonPart->GetClassTypeId()==CLS_LINETUBE)
		{
			CLDSLineTube *pTempTube=(CLDSLineTube*)pSonPart;
			if((pTempTube->m_tJointStart.type==1&&(pTempTube->m_tJointStart.hLinkObj==pTube->handle
				||pTempTube->m_tJointStart.hViceLinkObj==pTube->handle))
				||(pTempTube->m_tJointEnd.type==1&&(pTempTube->m_tJointEnd.hLinkObj==pTube->handle
				||pTempTube->m_tJointEnd.hViceLinkObj==pTube->handle)))
				bHasBranchTube=TRUE;
		}
		if(!bHasNodePlate&&pSonPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate *pTempPlate=(CLDSPlate*)pSonPart;
			if(pTempPlate->m_hPartWeldParent!=pTube->handle
				||pTempPlate->designInfo.m_hBasePart!=pTube->handle)
				continue;
			CLDSNode *pBaseNode=m_pModel->FromNodeHandle(pTempPlate->designInfo.m_hBaseNode);
			if(pBaseNode==NULL)
				continue;
			if(pBaseNode->handle==pTube->pStart->handle
				||pBaseNode->handle==pTube->pEnd->handle
				||pBaseNode->hFatherPart==pTube->handle)
				bHasNodePlate=TRUE;
		}
		if(bHasNodePlate&&bHasBranchTube)
			break;
	}
	if(bPush)
		m_pModel->PopPartStack();
	for(pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
	{
		if(stricmp(pRec->GetPartNo(),pTube->GetPartNo())==0)
		{
			//存在合法焊接父构件或焊接属性为TRUE
			if(!pRec->bWeldPart&&pFatherPart)
			{
				pRec->bWeldPart=TRUE;
				pRec->sub_type=TUBE_BRANCH;	//支管
			}
			else 
				pRec->sub_type=TUBE_MAIN;	//主管
			if(!pRec->bHasNodePlate&&bHasNodePlate)
				pRec->bHasNodePlate=TRUE;
			if(!pRec->bHasBranchTube&&bHasBranchTube)
				pRec->bHasBranchTube=TRUE;
			pRec->AddPart(pTube->m_uStatMatCoef);
			if(strlen(pRec->sNotes)==0&&strlen(pTube->sNotes)>0)
				strcpy(pRec->sNotes,pTube->sNotes);
			SummarizePartByModule(limit_str,pRec->iSeg);
			break;
		}
	}
	if(pRec==NULL)
	{
		pRec=part_recordset.append();
		pRec->SetPartNo(pTube->GetPartNo());
		pRec->part_type=STEELTYPE_TUBE;
		pRec->wide = pTube->GetDiameter();
		pRec->thick = pTube->GetThick();
		pRec->length=pTube->GetLength();
		//sprintf(pRec->sSpec,"Φ%.f*%.f",pRec->wide,pRec->thick);
		pTube->GetSpecification(pRec->sSpec,FALSE);
		pRec->cMaterial=pTube->cMaterial;
		pRec->cQualityLevel=pTube->m_cQualityLevel;
		pRec->iSeg=pTube->iSeg;
		pRec->fPieceNetWeight = pRec->fPieceMinWeight = pTube->GetLength()*Pi*pTube->GetThick()*(pTube->GetDiameter()-pTube->GetThick())*7.85e-6;
		pRec->nM16Ls=pTube->GetLsCount(16);
		pRec->nM20Ls=pTube->GetLsCount(20);
		pRec->nM24Ls=pTube->GetLsCount(24);
		pRec->nMXLs=pTube->GetLsCount()-pRec->nM16Ls-pRec->nM20Ls-pRec->nM24Ls;
		pRec->bZhiWan=pTube->bHuoQuStart||pTube->bHuoQuEnd;
		pRec->bHasFootNail=pTube->IsHasFootNail();
		//存在合法焊接父构件或焊接属性为TRUE
		if(pFatherPart)
		{
			pRec->bWeldPart=TRUE;
			pRec->sub_type=TUBE_BRANCH;	//支管
		}
		else 
			pRec->sub_type=TUBE_MAIN;	//主管
		pRec->bHasNodePlate=bHasNodePlate;		//连接有节点板
		pRec->bHasBranchTube=bHasBranchTube;	//连接有支管
		if(strlen(pTube->sNotes)>0)
			strcpy(pRec->sNotes,pTube->sNotes);
		CLDSParamPlate *pStartParamPlate=NULL, *pEndParamPlate=NULL;
		pStartParamPlate=(CLDSParamPlate*)pTube->BelongModel()->FromPartHandle(pTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		pEndParamPlate=(CLDSParamPlate*)pTube->BelongModel()->FromPartHandle(pTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		//统计钢管始端加工工艺
		if((pTube->m_tJointStart.type==0&&!pTube->m_tJointStart.cutPlaneNorm.IsZero())	//平面切割
			||(pTube->m_tJointStart.type==1&&pTube->m_tJointStart.hViceLinkObj<0&&pTube->m_tJointStart.hLinkObj>0x20))	//对接相贯
		{	//平面相贯
			f3dPoint tube_vec=pTube->End()-pTube->Start();
			normalize(tube_vec);
			double cosa=fabs(tube_vec*pTube->m_tJointStart.cutPlaneNorm);
			pRec->startProcess.type=PROCESSTYPE_PLANECUT;		//平面相贯
			pRec->startProcess.L=pTube->GetLength();			//支管长度
			pRec->startProcess.A=90.0-ACOS(cosa)*DEGTORAD_COEF;	//钢管心线与切割面之间的夹角
		}
		else if(pTube->m_tJointStart.type==1&&pTube->m_tJointStart.hLinkObj>0x20&&pTube->m_tJointStart.hViceLinkObj>=0)	
		{	//钢管(圆柱)相贯
			CLDSLineTube *pMainTube=(CLDSLineTube*)pTube->BelongModel()->FromPartHandle(pTube->m_tJointStart.hLinkObj,CLS_LINETUBE);
			if(pMainTube)
			{
				f3dPoint tube_vec=pTube->End()-pTube->Start();
				f3dPoint main_tube_vec=pMainTube->End()-pMainTube->Start();
				normalize(tube_vec);
				normalize(main_tube_vec);
				double cosa=fabs(tube_vec*main_tube_vec);
				pRec->startProcess.type=PROCESSTYPE_TRANSECT;	//圆柱相贯
				pRec->startProcess.L=pTube->GetLength();		//支管长度
				pRec->startProcess.A=ACOS(cosa)*DEGTORAD_COEF;	//支管与主管之间的夹角
				pMainTube->GetSpecification(pRec->startProcess.guige,FALSE);//主管规格
			}
		}
		else if(pStartParamPlate&&pStartParamPlate->m_iParamType==TYPE_UEND) 
		{	//U型开口槽
			double M=0,W=0,thick=0,fNormOffset=0;
			pStartParamPlate->GetDesignItemValue('M',&M);
			pStartParamPlate->GetDesignItemValue('W',&W);
			thick=pStartParamPlate->GetThick();
			fNormOffset=pStartParamPlate->m_fNormOffset;
			pRec->startProcess.type=PROCESSTYPE_USLOT;	//U型开口槽
			pRec->startProcess.L=M;						//开口槽长度L
			pRec->startProcess.A=W+2*thick;				//开口槽宽度A
			pRec->startProcess.B=0.5*W+thick;			//开口槽内圆弧半径B
			pRec->startProcess.fNormOffset=fNormOffset;	//开口槽法向偏移量
		}
		else if(pStartParamPlate&&pStartParamPlate->m_iParamType==TYPE_ROLLEND) 
		{	//一字型开口槽
			double M=0,thick=0,fNormOffset=0;
			pStartParamPlate->GetDesignItemValue('M',&M);
			thick=pStartParamPlate->GetThick();
			fNormOffset=pStartParamPlate->m_fNormOffset;
			pRec->startProcess.type=PROCESSTYPE_CSLOT;	//一字型开口槽
			pRec->startProcess.L=M;						//开口槽长度L
			pRec->startProcess.A=thick;					//开口槽宽度A
			pRec->startProcess.fNormOffset=fNormOffset;	//开口槽法向偏移量
		}
		else if(pStartParamPlate&&pStartParamPlate->m_iParamType==TYPE_XEND) 
		{	//十字型开口槽
			double M=0,thick=0,fNormOffset=0;
			pStartParamPlate->GetDesignItemValue('M',&M);
			thick=pStartParamPlate->GetThick();
			fNormOffset=pStartParamPlate->m_fNormOffset;
			pRec->startProcess.type=PROCESSTYPE_XSLOT;	//十字型开口槽
			pRec->startProcess.L=M;						//开口槽长度L
			pRec->startProcess.A=thick;					//开口槽宽度A
			pRec->startProcess.fNormOffset=fNormOffset;	//开口槽法向偏移量
		}
		else if(pStartParamPlate&&pStartParamPlate->IsFL())
		{
			if(pStartParamPlate->m_iParamType==TYPE_FLD)
				pRec->startProcess.type=PROCESSTYPE_FLD;
			else if(pStartParamPlate->m_iParamType==TYPE_FLP)
				pRec->startProcess.type=PROCESSTYPE_FLP;
			else if(pStartParamPlate->m_iParamType==TYPE_FLG)
				pRec->startProcess.type=PROCESSTYPE_FLG;
			else if(pStartParamPlate->m_iParamType==TYPE_FLR)
				pRec->startProcess.type=PROCESSTYPE_FLR;
			else //if(pStartParamPlate->m_iParamType==TYPE_FL)
				pRec->startProcess.type=PROCESSTYPE_FL;
		}
		else
			pRec->startProcess.type=PROCESSTYPE_NONE; //无加工工艺
		//统计钢管终端加工工艺
		if((pTube->m_tJointEnd.type==0&&!pTube->m_tJointEnd.cutPlaneNorm.IsZero())	//平面切割
			||(pTube->m_tJointEnd.type==1&&pTube->m_tJointEnd.hViceLinkObj<0&&pTube->m_tJointEnd.hLinkObj>0x20))	//对接相贯
		{	//平面相贯
			f3dPoint tube_vec=pTube->End()-pTube->Start();
			normalize(tube_vec);
			double cosa=fabs(tube_vec*pTube->m_tJointEnd.cutPlaneNorm);
			pRec->endProcess.type=PROCESSTYPE_PLANECUT;		//平面相贯
			pRec->endProcess.L=pTube->GetLength();			//支管长度
			pRec->endProcess.A=90.0-ACOS(cosa)*DEGTORAD_COEF;//钢管心线与切割面之间的夹角
		}
		else if(pTube->m_tJointEnd.type==1&&pTube->m_tJointEnd.hLinkObj>0x20&&pTube->m_tJointEnd.hViceLinkObj>=0)	
		{	//钢管(圆柱)相贯
			CLDSLineTube *pMainTube=(CLDSLineTube*)pTube->BelongModel()->FromPartHandle(pTube->m_tJointEnd.hLinkObj,CLS_LINETUBE);
			if(pMainTube)
			{
				f3dPoint tube_vec=pTube->End()-pTube->Start();
				f3dPoint main_tube_vec=pMainTube->End()-pMainTube->Start();
				normalize(tube_vec);
				normalize(main_tube_vec);
				double cosa=fabs(tube_vec*main_tube_vec);
				pRec->endProcess.type=PROCESSTYPE_TRANSECT;	//圆柱相贯
				pRec->endProcess.L=pTube->GetLength();		//支管长度
				pRec->endProcess.A=ACOS(cosa)*DEGTORAD_COEF;//支管与主管之间的夹角
				pMainTube->GetSpecification(pRec->endProcess.guige,FALSE);//主管规格
			}
		}
		else if(pEndParamPlate&&pEndParamPlate->m_iParamType==TYPE_UEND)
		{	//U型开口槽
			double M=0,W=0,thick=0,fNormOffset=0;
			pEndParamPlate->GetDesignItemValue('M',&M);
			pEndParamPlate->GetDesignItemValue('W',&W);
			thick=pEndParamPlate->GetThick();
			fNormOffset=pEndParamPlate->m_fNormOffset;
			pRec->endProcess.type=PROCESSTYPE_USLOT;//U型开口槽
			pRec->endProcess.L=M;					//开口槽长度L
			pRec->endProcess.A=W+2*thick;			//开口槽宽度A
			pRec->endProcess.B=0.5*W+thick;			//开口槽内圆弧半径B
			pRec->endProcess.fNormOffset=fNormOffset;//开口槽法向偏移量
		}
		else if(pEndParamPlate&&pEndParamPlate->m_iParamType==TYPE_ROLLEND)
		{	//一字型开口槽
			double M=0,thick=0,fNormOffset=0;
			pEndParamPlate->GetDesignItemValue('M',&M);
			thick=pEndParamPlate->GetThick();
			fNormOffset=pEndParamPlate->m_fNormOffset;
			pRec->endProcess.type=PROCESSTYPE_CSLOT;//一字型开口槽
			pRec->endProcess.L=M;					//开口槽长度L
			pRec->endProcess.A=thick;				//开口槽宽度A
			pRec->endProcess.fNormOffset=fNormOffset;//开口槽法向偏移量
		}
		else if(pEndParamPlate&&pEndParamPlate->m_iParamType==TYPE_XEND)
		{	//十字型开口槽
			double M=0,thick=0,fNormOffset=0;
			pEndParamPlate->GetDesignItemValue('M',&M);
			thick=pEndParamPlate->GetThick();
			fNormOffset=pEndParamPlate->m_fNormOffset;
			pRec->endProcess.type=PROCESSTYPE_XSLOT;//十字型开口槽
			pRec->endProcess.L=M;					//开口槽长度L
			pRec->endProcess.A=thick;				//开口槽宽度A
			pRec->endProcess.fNormOffset=fNormOffset;//开口槽法向偏移量
		}
		else if(pEndParamPlate&&pEndParamPlate->IsFL())
		{
			if(pEndParamPlate->m_iParamType==TYPE_FLD)
				pRec->endProcess.type=PROCESSTYPE_FLD;
			else if(pEndParamPlate->m_iParamType==TYPE_FLP)
				pRec->endProcess.type=PROCESSTYPE_FLP;
			else if(pEndParamPlate->m_iParamType==TYPE_FLG)
				pRec->endProcess.type=PROCESSTYPE_FLG;
			else if(pEndParamPlate->m_iParamType==TYPE_FLR)
				pRec->endProcess.type=PROCESSTYPE_FLR;
			else //if(pEndParamPlate->m_iParamType==TYPE_FL)
				pRec->endProcess.type=PROCESSTYPE_FL;
		}
		else
			pRec->endProcess.type=PROCESSTYPE_NONE; //无加工工艺
		pRec->AddPart(pTube->m_uStatMatCoef);
		SummarizePartByModule(limit_str,pRec->iSeg);
	}
}
void CTowerType::SummarizeOneLineSlot(CLDSLineSlot *pLineSlot,char *limit_str)
{	
	PART *pRec=NULL;
	//焊接父构件
	CLDSPart *pFatherPart=m_pModel->FromPartHandle(pLineSlot->m_hPartWeldParent);
	for(pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
	{
		if(stricmp(pRec->GetPartNo(),pLineSlot->GetPartNo())==0)
		{	
			//存在合法焊接父构件或焊接属性为TRUE
			if(!pRec->bWeldPart&&pFatherPart)
				pRec->bWeldPart=TRUE;
			pRec->AddPart(pLineSlot->m_uStatMatCoef);
			if(strlen(pRec->sNotes)==0&&strlen(pLineSlot->sNotes)>0)
				strcpy(pRec->sNotes,pLineSlot->sNotes);
			SummarizePartByModule(limit_str,pRec->iSeg);
			break;
		}
	}
	if(pRec==NULL)
	{
		pRec=part_recordset.append();
		pRec->SetPartNo(pLineSlot->GetPartNo());
		pRec->part_type=STEELTYPE_SLOT;
		pRec->wide = pLineSlot->GetWidth();
		pRec->thick = pLineSlot->GetThick();
		pRec->length=pLineSlot->GetLength();
		if(strlen(pLineSlot->spec)<=0)
			sprintf(pRec->sSpec,"[%.f",pLineSlot->size_height*0.1);
		else
			sprintf(pRec->sSpec,"[%s",pLineSlot->spec);	//槽钢规格 wht 10-08-26
		pRec->cMaterial=pLineSlot->cMaterial;
		pRec->cQualityLevel=pLineSlot->m_cQualityLevel;
		pRec->iSeg=pLineSlot->iSeg;
		pRec->fPieceNetWeight = pRec->fPieceMinWeight = pLineSlot->GetLength()*
			(pLineSlot->GetThick()*(pLineSlot->GetHeight()+2*(pLineSlot->GetWidth()-pLineSlot->GetThick())))*7.85e-6;
		pRec->nM16Ls=pLineSlot->GetLsCount(16);
		pRec->nM20Ls=pLineSlot->GetLsCount(20);
		pRec->nM24Ls=pLineSlot->GetLsCount(24);
		pRec->nMXLs=pLineSlot->GetLsCount()-pRec->nM16Ls-pRec->nM20Ls-pRec->nM24Ls;
		pRec->bZhiWan=pLineSlot->bHuoQuStart||pLineSlot->bHuoQuEnd;
		pRec->bHasFootNail=pLineSlot->IsHasFootNail();
		if(pFatherPart)
			pRec->bWeldPart=TRUE;
		if(strlen(pLineSlot->sNotes)>0)
			strcpy(pRec->sNotes,pLineSlot->sNotes);

		pRec->nMSumLs = pLineSlot->GetLsCount();
		UCS_STRU ucs;
		ucs=pLineSlot->BuildUCS();
		pRec->ls_arr = new PART_LS[pRec->nMSumLs];
		int i=0;
		for(CLsRef *pLsRef=pLineSlot->GetFirstLsRef();pLsRef;pLsRef=pLineSlot->GetNextLsRef())
		{	//槽钢上的螺栓信息
			f3dPoint pt = (*pLsRef)->ucs.origin;
			coord_trans(pt,ucs,FALSE);
			pRec->ls_arr[i].d = (*pLsRef)->get_d();
			pRec->ls_arr[i].hole_d_increment = (*pLsRef)->hole_d_increment;
			pRec->ls_arr[i].x = pLineSlot->startOdd()+pt.z;
			pRec->ls_arr[i].y = 0;
			i++;
		}
		pRec->AddPart(pLineSlot->m_uStatMatCoef);
		SummarizePartByModule(limit_str,pRec->iSeg);
	}
}
void CTowerType::SummarizeOneLineFlat(CLDSLineFlat *pLineFlat,char *limit_str)
{
	PART *pRec=NULL;
	CLDSPart *pFatherPart=m_pModel->FromPartHandle(pLineFlat->m_hPartWeldParent);
	for(pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
	{
		if(stricmp(pRec->GetPartNo(),pLineFlat->GetPartNo())==0)
		{	
			//存在合法焊接父构件或焊接属性为TRUE
			if(!pRec->bWeldPart&&pFatherPart)
				pRec->bWeldPart=TRUE;
			pRec->AddPart(pLineFlat->m_uStatMatCoef);
			if(strlen(pRec->sNotes)==0&&strlen(pLineFlat->sNotes)>0)
				strcpy(pRec->sNotes,pLineFlat->sNotes);
			SummarizePartByModule(limit_str,pRec->iSeg);
			break;
		}
	}
	if(pRec==NULL)
	{
		pRec=part_recordset.append();
		pRec->SetPartNo(pLineFlat->GetPartNo());
		pRec->part_type=STEELTYPE_FLAT;
		pRec->cMaterial=pLineFlat->cMaterial;
		pRec->cQualityLevel=pLineFlat->m_cQualityLevel;
		pRec->iSeg=pLineFlat->iSeg;
		pRec->wide = pLineFlat->GetWidth();
		pRec->thick = pLineFlat->GetThick();
		pRec->length=pLineFlat->GetLength();
		sprintf(pRec->sSpec,"-%.fx%.f",pRec->wide,pRec->thick);
		pRec->fPieceNetWeight= CalSteelPlankWei(pRec->length*pRec->wide*pRec->thick);
		pRec->fPieceMinWeight=pRec->fPieceNetWeight;
		pRec->nM16Ls=pLineFlat->GetLsCount(16);
		pRec->nM20Ls=pLineFlat->GetLsCount(20);
		pRec->nM24Ls=pLineFlat->GetLsCount(24);
		pRec->nMXLs=pLineFlat->GetLsCount()-pRec->nM16Ls-pRec->nM20Ls-pRec->nM24Ls;
		pRec->bZhiWan=pLineFlat->bHuoQuStart||pLineFlat->bHuoQuEnd;

		if(pFatherPart)
			pRec->bWeldPart=TRUE;
		if(strlen(pLineFlat->sNotes)>0)
			strcpy(pRec->sNotes,pLineFlat->sNotes);

		pRec->nMSumLs = pLineFlat->GetLsCount();
		UCS_STRU ucs;
		ucs=pLineFlat->BuildUCS();
		pRec->ls_arr = new PART_LS[pRec->nMSumLs];
		int i=0;
		for(CLsRef *pLsRef=pLineFlat->GetFirstLsRef();pLsRef;pLsRef=pLineFlat->GetNextLsRef())
		{	//扁铁上的螺栓信息
			f3dPoint pt = (*pLsRef)->ucs.origin;
			coord_trans(pt,ucs,FALSE);
			pRec->ls_arr[i].d = (*pLsRef)->get_d();
			pRec->ls_arr[i].hole_d_increment = (*pLsRef)->hole_d_increment;
			pRec->ls_arr[i].x = pLineFlat->startOdd()+pt.z;
			pRec->ls_arr[i].y = 0;
			i++;
		}
		pRec->AddPart(pLineFlat->m_uStatMatCoef);
		SummarizePartByModule(limit_str,pRec->iSeg);
	}
}
void CTowerType::SummarizeOneParamPlate(CLDSParamPlate *pPlate,char *limit_str)
{
	if (pPlate->IsShadowObject())
		pPlate->ShadowDesign();
	if(pPlate->vertex_list.GetNodeNum()<3)
		pPlate->DesignPlate();
	if(pPlate->vertex_list.GetNodeNum()>2)
	{	//有板实体
		PART *pRec=NULL;
		//焊接父构件
		BOOL bWeldPart=FALSE;
		CLDSPart *pFatherPart=m_pModel->FromPartHandle(pPlate->m_hPartWeldParent);
		if(pFatherPart==NULL)
		{	//判断该构件是否存在焊接子构件，存在则认为该构件亦为焊接件
			CLDSPart *pSonPart=NULL;
			BOOL bPush=m_pModel->PushPartStack();
			for(pSonPart=m_pModel->EnumPartFirst();pSonPart;pSonPart=m_pModel->EnumPartNext())
			{
				if(pSonPart->m_hPartWeldParent==pPlate->handle)
				{
					bWeldPart=TRUE;
					break;
				}
			}
			if(bPush)
				m_pModel->PopPartStack();
		}
		else
			bWeldPart=TRUE;
		for(pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
		{
			if(stricmp(pRec->GetPartNo(),pPlate->GetPartNo())==0&&pRec->iSeg.iSeg==pPlate->iSeg)
			{
				if(!pRec->bWeldPart&&bWeldPart)
					pRec->bWeldPart=TRUE;
				pRec->AddPart(pPlate->m_uStatMatCoef);
				if(strlen(pRec->sNotes)==0&&strlen(pPlate->sNotes)>0)
					strcpy(pRec->sNotes,pPlate->sNotes);
				SummarizePartByModule(limit_str,pRec->iSeg);
				break;
			}
		}
		if(pRec==NULL)
		{
			pRec=part_recordset.append();
			pRec->iSeg=pPlate->iSeg;
			pRec->SetPartNo(pPlate->GetPartNo());
			pRec->wide = pPlate->GetWidth();
			pRec->length = pPlate->GetLength();
			pRec->thick = pPlate->GetThick();
			pRec->cMaterial=pPlate->cMaterial;
			pRec->cQualityLevel=pPlate->m_cQualityLevel;
			pRec->min_area=pRec->wide*pRec->length;
			pRec->real_area=pRec->wide*pRec->length;
			pRec->fPieceMinWeight = CalSteelPlankWei(pRec->min_area*pRec->thick);
			pRec->fPieceNetWeight = CalSteelPlankWei(pRec->real_area*pPlate->GetThick());
			sprintf(pRec->sSpec,"-%.fx%.f",pRec->thick,pRec->wide);
			pRec->part_type=STEELTYPE_PLATE;
			if(pPlate->m_iParamType==TYPE_ROLLEND)
				pRec->sub_type=PARAMPLATE_C;		//槽型插板
			else if(pPlate->m_iParamType==TYPE_UEND)
				pRec->sub_type=PARAMPLATE_U;		//U型插板
			else if(pPlate->m_iParamType==TYPE_XEND)
				pRec->sub_type=PARAMPLATE_X;		//X型插板
			else if(pPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
				pRec->sub_type=PARAMPLATE_WATER;	//遮水板
			else if(pPlate->IsFL())
			{
				double D=0,B=0;
				pRec->sub_type=PARAMPLATE_FL;		//法兰
				if(pPlate->m_iParamType==TYPE_FL||pPlate->m_iParamType==TYPE_FLG||
					pPlate->m_iParamType==TYPE_FLR)
				{
					pPlate->GetDesignItemValue('W',&D);
					pPlate->GetDesignItemValue('R',&B);
					B*=2;
				}
				else if(pPlate->m_iParamType==TYPE_FLD||pPlate->m_iParamType==TYPE_FLP)
				{
					pPlate->GetDesignItemValue('D',&D);
					pPlate->GetDesignItemValue('B',&B);
				}
				if(CustomerSerial==3)
				{	//根据武晓要求及图纸规范，法兰规格统一为“-厚度*外径”	wxc-2016.12.19
					sprintf(pRec->sSpec,"-%.fx%.f",pRec->thick,D);
				}
				else
					sprintf(pRec->sSpec,"Φ%.f/%.f",D,B);
			}
			pRec->nM16Ls=pPlate->GetLsCount(16);
			pRec->nM20Ls=pPlate->GetLsCount(20);
			pRec->nM24Ls=pPlate->GetLsCount(24);
			pRec->nMXLs=pPlate->GetLsCount()-pRec->nM16Ls-pRec->nM20Ls-pRec->nM24Ls;
			pRec->nMSumLs=pPlate->GetLsCount();	//螺栓总数
			pRec->ls_arr = new PART_LS[pRec->nMSumLs];
			int i=0;
			for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
			{
				f3dPoint pt = (*pLsRef)->ucs.origin;
				coord_trans(pt,pPlate->ucs,FALSE);
				pRec->ls_arr[i].d = (*pLsRef)->get_d();
				pRec->ls_arr[i].hole_d_increment = (*pLsRef)->hole_d_increment;
				pRec->ls_arr[i].x = pt.x;
				pRec->ls_arr[i].y = pt.y;
				i++;
			}
			if(bWeldPart)
				pRec->bWeldPart=TRUE;
			if(bWeldPart&&pFatherPart!=NULL)
				pRec->bNeedFillet=TRUE;	//仅有焊接父构件的钢板需要坡口工艺 wjh-2016.12-22
			else
				pRec->bNeedFillet=FALSE;
			pRec->nEdgeNum=(short)pPlate->vertex_list.GetNodeNum();
			//计算钢板的周长
			pRec->fWeldEdgeLen=0;	//焊接边长度
			PROFILE_VERTEX *pVertex=NULL,*pPreVertex=pPlate->vertex_list.GetTail();
			for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			{
				if(pPreVertex)
				{
					double fEdgeLen=DISTANCE(pPreVertex->vertex,pVertex->vertex);
					if(pPreVertex->type==1||pPreVertex->type==2||pPreVertex->type==3)
					{	//圆弧边
						int sign=1;
						f3dArcLine arcLine;
						if(pPreVertex->type==1)
						{	//圆弧
							if(pPreVertex->sector_angle<0)
								sign=-1;
							arcLine.CreateMethod2(pPreVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),fabs(pPreVertex->sector_angle));
						}
						else if(pPreVertex->type==2)	//指定圆弧R
						{	
							if(pPreVertex->radius<0)
								sign=-1;
							arcLine.CreateMethod3(pPreVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),sign*pPreVertex->radius,pPreVertex->center);
							//不能用CreateMethod2,否则丢失了拾取点信息，对于大于180°的弧是错的。 wht 12-06-14
							//arcLine.CreateMethod2(pPreVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),pPreVertex->sector_angle);
						}
						else //if(pPreVertex->type==3)	//椭圆弧
						{
							f3dPoint center = pPreVertex->center;
							f3dPoint col_axis=pPreVertex->column_norm;
							coord_trans(center,pPlate->ucs,FALSE);
							vector_trans(col_axis,pPlate->ucs,FALSE);
							if(pPreVertex->radius<0)
								sign=-1;
							arcLine.CreateEllipse(center,pPreVertex->vertex,pVertex->vertex,col_axis,f3dPoint(0,0,sign),sign*pPreVertex->radius);
						}
						fEdgeLen=arcLine.Length();
					}
					pRec->fPerimeter+=fEdgeLen;
					//焊接边长度 wht 10-10-12
					if(pPreVertex->m_bWeldEdge)
						pRec->fWeldEdgeLen+=fEdgeLen;
				}
				pPreVertex=pVertex;
			}
			if(strlen(pPlate->sNotes)>0)
				strcpy(pRec->sNotes,pPlate->sNotes);
			pRec->AddPart(pPlate->m_uStatMatCoef);
			SummarizePartByModule(limit_str,pRec->iSeg);
		}
	}
 }
void CTowerType::SummarizeOnePart(CLDSPart *pPart,CTower *pTower/*=NULL*/)
{	
	if(m_pModel==NULL)
	{
		m_pModel=pTower;
		m_pTower=pTower;
	}
	if(m_pTower==NULL)
		return;
	//暂未考虑制弯件统材
	CString sPureGuiGe,cMaterial;
	CLDSPart *pStatDatumPart=pPart;
	if(pPart->GetClassTypeId()==CLS_POLYPART)
	{
		CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
		pStatDatumPart=pPolyPart->segset.GetFirst();
	}
	else if(pPart->IsLinePart())
	{
		CLDSLinePart* pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->bHuoQuStart||pLinePart->bHuoQuEnd)
			return;	//制弯子构件在制弯件中统计不单独统计
	}
	limit_str[0]='\0';
	int iNo=1;
	for(CLDSModule *pModule=m_pTower->Module.GetFirst();pModule;pModule=m_pTower->Module.GetNext(),iNo++)
	{
		if(pModule->IsSonPart(pStatDatumPart))
		{
			char ss[4];
			if(strlen(limit_str)>0)	//已有数据
				sprintf(ss,",%d",iNo);
			else					//还无数据
				sprintf(ss,"%d",iNo);
			strcat(limit_str,ss);
		}
	}
	if(pPart->GetClassTypeId()==CLS_BOLT) 
		SummarizeOneBolt((CLDSBolt*)pPart,limit_str);
	else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		SummarizeOneLineAngle((CLDSLineAngle*)pPart,limit_str);
	else if(pPart->GetClassTypeId()==CLS_LINETUBE)
		SummarizeOneLineTube((CLDSLineTube*)pPart,limit_str);
	else if(pPart->GetClassTypeId()==CLS_LINESLOT)
		SummarizeOneLineSlot((CLDSLineSlot*)pPart,limit_str);
	else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
		SummarizeOneLineFlat((CLDSLineFlat*)pPart,limit_str);
	else if(pPart->GetClassTypeId()==CLS_PLATE)
		SummarizeOnePlate((CLDSPlate*)pPart,limit_str);
	else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		SummarizeOneParamPlate((CLDSParamPlate*)pPart,limit_str);
	else if(pPart->GetClassTypeId()==CLS_POLYPART)
		SummarizeOnePolyPart((CLDSPolyPart*)pPart,limit_str);
}
void CTowerType::SummarizeOnePolyPart(CLDSPolyPart *pPolyPart,char *limit_str)
{
	PART *pRec=NULL;
	CLDSLinePart *pFirstLinePart=pPolyPart->segset.GetFirst();
	for(pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
	{
		if(stricmp(pRec->GetPartNo(),pFirstLinePart->GetPartNo())==0)
		{
			pRec->AddPart(pFirstLinePart->m_uStatMatCoef);
			SummarizePartByModule(limit_str,pRec->iSeg);
			break;
		}
	}
	if(pRec==NULL)
	{
		pRec=part_recordset.append();
		pRec->SetPartNo(pFirstLinePart->GetPartNo());
		pRec->part_type=STEELTYPE_ANGLE;
		pRec->wide = pFirstLinePart->GetWidth();
		pRec->thick = pFirstLinePart->GetThick();
		pRec->length=pPolyPart->GetLength();
		pFirstLinePart->GetSpecification(pRec->sSpec,FALSE);
		pRec->cMaterial=pFirstLinePart->cMaterial;
		pRec->cQualityLevel=pFirstLinePart->m_cQualityLevel;
		pRec->iSeg=pFirstLinePart->iSeg;
		pRec->fPieceNetWeight = pRec->fPieceMinWeight = TruncateDouble(pPolyPart->GetWeight(),2);
		pRec->bZhiWan=true;
		if(pFirstLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{
			for(CLDSLineAngle *pSegAngle=(CLDSLineAngle*)pPolyPart->segset.GetFirst();pSegAngle;pSegAngle=(CLDSLineAngle*)pPolyPart->segset.GetNext())
			{
				if(!pRec->bCutAngle&&pSegAngle->IsCutAngleOrWing())
					pRec->bCutAngle=TRUE;
				if(!pRec->bCutRoot&&pSegAngle->m_bCutRoot)
					pRec->bCutRoot=TRUE;
				if(!pRec->bCutBer&&pSegAngle->m_bCutBer)
					pRec->bCutBer=TRUE;
				if(!pRec->bWeldPart&&pSegAngle->IsWeldPart())
					pRec->bWeldPart=TRUE;	//焊接
				//使用用户设置的开合角阈值判断角钢是否需要进行开合角 wht 11-05-06
				if(pSegAngle->GetDecWingAngle()-90>globalLibrary.GetJgKaiHeJiaoThreshold()&&pSegAngle->GetKaiHeWingX0Y1()!=-1)
					pRec->bKaiJiao=TRUE;
				else if(90-pSegAngle->GetDecWingAngle()>globalLibrary.GetJgKaiHeJiaoThreshold()&&pSegAngle->GetKaiHeWingX0Y1()!=-1)
					pRec->bHeJiao=TRUE;
				if(strlen(pRec->sNotes)==0&&strlen(pSegAngle->sNotes)>0)
					strcpy(pRec->sNotes,pSegAngle->sNotes);
				pRec->nMSumLs+=pSegAngle->GetLsCount();
				pRec->nM16Ls+=pSegAngle->GetLsCount(16);
				pRec->nM20Ls+=pSegAngle->GetLsCount(20);
				pRec->nM24Ls+=pSegAngle->GetLsCount(24);
			}
			pRec->nMXLs=pRec->nMSumLs-pRec->nM16Ls-pRec->nM20Ls-pRec->nM24Ls;
			pRec->wing_angle=90;
			pRec->ls_arr = new PART_LS[pRec->nMSumLs];
			BOLTHOLE_INFO *pBoltHole;
			CLDSPolyPart::AnaylyzeAngleBolts(pPolyPart);
			int i=0,nWingXLsCount=CLDSPolyPart::boltListX.GetNodeNum(),nWingYLsCount=CLDSPolyPart::boltListY.GetNodeNum();
			for(pBoltHole=CLDSPolyPart::boltListX.GetFirst();pBoltHole;pBoltHole=CLDSPolyPart::boltListX.GetNext())
			{
				pRec->ls_arr[i].d = ftoi(pBoltHole->bolt_d);
				pRec->ls_arr[i].x = pBoltHole->centre.x;
				pRec->ls_arr[i].y =-pBoltHole->centre.y;
				pRec->ls_arr[i].hole_d_increment = 1.5;
				i++;
			}
			for(pBoltHole=CLDSPolyPart::boltListY.GetFirst();pBoltHole;pBoltHole=CLDSPolyPart::boltListY.GetNext())
			{
				pRec->ls_arr[i].d = ftoi(pBoltHole->bolt_d);
				pRec->ls_arr[i].x = pBoltHole->centre.x;
				pRec->ls_arr[i].y = pBoltHole->centre.y;
				pRec->ls_arr[i].hole_d_increment = 1.5;
				i++;
			}
		}
		pRec->AddPart(pFirstLinePart->m_uStatMatCoef);
		SummarizePartByModule(limit_str,pRec->iSeg);
	}
}
void CTowerType::SummarizeOnePlate(CLDSPlate *pPlate,char *limit_str)
{
	try{
		if (pPlate->IsShadowObject())
			pPlate->ShadowDesign();
		if(((CLDSPlate*)(pPlate->MotherObject()))->vertex_list.GetNodeNum()>2)
		{	//有板实体
			PART *pRec=NULL;
			//焊接父构件
			BOOL bWeldPart=FALSE;
			CLDSPart *pFatherPart=pPlate->BelongModel()->FromPartHandle(pPlate->m_hPartWeldParent);
			if(pFatherPart==NULL)
			{	//判断该构件是否存在焊接子构件，存在则认为该构件亦为焊接件
				CLDSPart *pSonPart=NULL;
				BOOL bPush=m_pModel->PushPartStack();
				for(pSonPart=m_pModel->EnumPartFirst();pSonPart;pSonPart=m_pModel->EnumPartNext())
				{
					if(pSonPart->m_hPartWeldParent==pPlate->handle)
					{
						bWeldPart=TRUE;
						break;
					}
				}
				if(bPush)
					m_pModel->PopPartStack();
			}
			else
				bWeldPart=TRUE;
			for(pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
			{
				if(stricmp(pRec->GetPartNo(),pPlate->GetPartNo())==0)
				{
					if(!pRec->bWeldPart&&(pPlate->m_bWeldPart||bWeldPart))
						pRec->bWeldPart=TRUE;
					if(!pRec->bNeedFillet&&pPlate->m_bNeedFillet)
						pRec->bNeedFillet=pPlate->m_bNeedFillet;
					pRec->AddPart(pPlate->m_uStatMatCoef);
					if(strlen(pRec->sNotes)==0&&strlen(pPlate->sNotes)>0)
						strcpy(pRec->sNotes,pPlate->sNotes);
					SummarizePartByModule(limit_str,pRec->iSeg);
					break;
				}
			}
			if(pRec==NULL)
			{
				pRec=part_recordset.append();
				pRec->iSeg=pPlate->iSeg;
				pRec->SetPartNo(pPlate->GetPartNo());
				pRec->wide = pPlate->GetWidth();
				pRec->length = pPlate->GetLength();
				pRec->thick = pPlate->GetThick();
				sprintf(pRec->sSpec,"-%.fx%.f",pPlate->GetThick(),pRec->wide);
				pRec->cMaterial=pPlate->cMaterial;
				pRec->cQualityLevel=pPlate->m_cQualityLevel;
				pRec->min_area=pPlate->CalAreaOfMinWrapRect(NULL);
				pRec->real_area=pPlate->CalAreaOfNetWrap();
				pRec->fPieceMinWeight = CalSteelPlankWei(pRec->min_area*pRec->thick);
				pRec->fPieceNetWeight = CalSteelPlankWei(pRec->real_area*pPlate->GetThick());
				pRec->part_type=STEELTYPE_PLATE;
				if(ftoi(pPlate->m_fInnerRadius)>0)
				{
					pRec->sub_type=PLATE_FL;	//底脚法兰
					if(CustomerSerial!=3)
						sprintf(pRec->sSpec,"Φ%.f/%.f",pRec->wide,pPlate->m_fInnerRadius*2);
				}
				pRec->nM16Ls=pPlate->GetLsCount(16);
				pRec->nM20Ls=pPlate->GetLsCount(20);
				pRec->nM24Ls=pPlate->GetLsCount(24);
				pRec->nMXLs=pPlate->GetLsCount()-pRec->nM16Ls-pRec->nM20Ls-pRec->nM24Ls;
				pRec->nMSumLs=pPlate->GetLsCount();	//螺栓总数
				pRec->ls_arr = new PART_LS[pRec->nMSumLs];
				int i=0;
				for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{
					f3dPoint pt = (*pLsRef)->ucs.origin;
					coord_trans(pt,pPlate->ucs,FALSE);
					pRec->ls_arr[i].d = (*pLsRef)->get_d();
					pRec->ls_arr[i].hole_d_increment = (*pLsRef)->hole_d_increment;
					pRec->ls_arr[i].x = pt.x;
					pRec->ls_arr[i].y = pt.y;
					i++;
				}
				if(pRec->bWeldPart||bWeldPart)
					pRec->bWeldPart=TRUE;
				pRec->bNeedFillet=pPlate->m_bNeedFillet;
				pRec->nEdgeNum=(short)pPlate->vertex_list.GetNodeNum();
				//计算钢板的周长
				PROFILE_VERTEX *pVertex=NULL,*pPreVertex=pPlate->vertex_list.GetTail();
				for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
				{
					if(pPreVertex)
					{
						double fEdgeLen=DISTANCE(pPreVertex->vertex,pVertex->vertex);
						if(pPreVertex->type==1||pPreVertex->type==2||pPreVertex->type==3)
						{	//圆弧边
							int sign=1;
							f3dArcLine arcLine;
							if(pPreVertex->type==1)
							{	//圆弧
								if(pPreVertex->sector_angle<0)
									sign=-1;
								arcLine.CreateMethod2(pPreVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),fabs(pPreVertex->sector_angle));
							}
							else if(pPreVertex->type==2)	//指定圆弧R
							{	
								if(pPreVertex->radius<0)
									sign=-1;
								arcLine.CreateMethod3(pPreVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),sign*pPreVertex->radius,pPreVertex->center);
								//不能用CreateMethod2,否则丢失了拾取点信息，对于大于180°的弧是错的。 wht 12-06-14
								//arcLine.CreateMethod2(pPreVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),pPreVertex->sector_angle);
							}
							else //if(pPreVertex->type==3)	//椭圆弧
							{
								f3dPoint center = pPreVertex->center;
								f3dPoint col_axis=pPreVertex->column_norm;
								coord_trans(center,pPlate->ucs,FALSE);
								vector_trans(col_axis,pPlate->ucs,FALSE);
								if(pPreVertex->radius<0)
									sign=-1;
								arcLine.CreateEllipse(center,pPreVertex->vertex,pVertex->vertex,col_axis,f3dPoint(0,0,sign),sign*pPreVertex->radius);
							}
							fEdgeLen=arcLine.Length();
						}
						pRec->fPerimeter+=fEdgeLen;
						//焊接边长度 wht 10-10-12
						if(pPreVertex->m_bWeldEdge)
							pRec->fWeldEdgeLen+=fEdgeLen;
					}
					pPreVertex=pVertex;
				}
				if(pPlate->face_N>1)
					pRec->bZhiWan=TRUE;
				if(pPlate->wiType==CLDSGeneralPlate::WASHER_PLATE)	//螺栓垫板
					pRec->bDianBan=TRUE;
				if(fabs(pPlate->GetRollEdgeHeight())>0)	//钢板卷边高度值允许是负值     wxc----2015.5.29
				{
					pRec->fRollHeight=pPlate->GetRollEdgeHeight();
					pRec->bRollEdge=pRec->bZhiWan=TRUE;	//卷边一定需要制弯 wjh-2016.12.22
				}
				if(strlen(pPlate->sNotes)>0)
					strcpy(pRec->sNotes,pPlate->sNotes);
				pRec->AddPart(pPlate->m_uStatMatCoef);
				SummarizePartByModule(limit_str,pRec->iSeg);
			}
		}
	}
	catch(char* sError)
	{
		char ss[200];
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(ss,"%s the handle of error part is 0X%X.",sError,pPlate->handle);
#else
		sprintf(ss,"%s出错构件句柄为0X%X。",sError,pPlate->handle);
#endif
		AfxMessageBox(ss);
	}
}
void CTowerType::SummarizeSuppPart()	//统计附加构件
{
	if(m_pTower==NULL)
		return;
	for(CSupplementPart *pSuppPart=m_pTower->SuppPart.GetFirst();pSuppPart;pSuppPart=m_pTower->SuppPart.GetNext())
	{
		limit_str[0]='\0';
		int iNo=1;
		for(CLDSModule *pModule=m_pTower->Module.GetFirst();pModule;pModule=m_pTower->Module.GetNext(),iNo++)
		{
			if(pSuppPart->cfgword.IsHasBodyNo(pModule->GetBodyNo()))
			{
				char ss[4];
				if(strlen(limit_str)>0)	//已有数据
					sprintf(ss,",%d",iNo);
				else					//还无数据
					sprintf(ss,"%d",iNo);
				strcat(limit_str,ss);
			}
		}
		if(pSuppPart->part_type==STEELTYPE_ANGLE)	//角钢
		{
			PART *pRec=NULL;
			for(pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
			{
				if(stricmp(pRec->GetPartNo(),pSuppPart->GetPartNo())==0)
				{
					pRec->AddPart(pSuppPart->m_uStatMatCoef);
					if(strlen(pRec->sNotes)==0&&strlen(pSuppPart->sNotes)>0)
						strcpy(pRec->sNotes,pSuppPart->sNotes);
					SummarizePartByModule(limit_str,pRec->iSeg);
					break;
				}
			}
			if(pRec==NULL)
			{
				pRec=part_recordset.append();
				pRec->SetPartNo(pSuppPart->GetPartNo());
				pRec->part_type=STEELTYPE_ANGLE;
				pRec->sSpec.Copy(pSuppPart->guige);
				pRec->cMaterial=pSuppPart->cMaterial;
				pRec->cQualityLevel=pSuppPart->m_cQualityLevel;
				pRec->iSeg=pSuppPart->iSeg;
				char ss[100];
				strcpy(ss,pRec->sSpec);
				char *key=strtok(ss,"LXSH ");
				if(key)
				{
					pRec->wide = atof(key);
					key=strtok(NULL,"LXSH ");
					if(key)
						pRec->thick = atof(key);
				}
				restore_JG_guige(pRec->sSpec,pRec->wide,pRec->thick);
				pRec->length=pSuppPart->length;
				pRec->fPieceNetWeight = pRec->fPieceMinWeight = pSuppPart->piece_weight;
				if(strlen(pSuppPart->sNotes)>0)
					strcpy(pRec->sNotes,pSuppPart->sNotes);
				pRec->AddPart(pSuppPart->nQuantity);
				SummarizePartByModule(limit_str,pRec->iSeg);
			}
		}
		else if(pSuppPart->part_type==STEELTYPE_PLATE)	//钢板
		{
			PART *pRec=NULL;
			for(pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
			{
				if(stricmp(pRec->GetPartNo(),pSuppPart->GetPartNo())==0)
				{
					pRec->AddPart(pSuppPart->m_uStatMatCoef);
					if(strlen(pRec->sNotes)==0&&strlen(pSuppPart->sNotes)>0)
						strcpy(pRec->sNotes,pSuppPart->sNotes);
					SummarizePartByModule(limit_str,pRec->iSeg);
					break;
				}
			}
			if(pRec==NULL)
			{
				pRec=part_recordset.append();
				pRec->SetPartNo(pSuppPart->GetPartNo());
				pRec->part_type=STEELTYPE_PLATE;	//连接板
				pRec->sSpec.Copy(pSuppPart->guige);
				pRec->cMaterial=pSuppPart->cMaterial;
				pRec->cQualityLevel=pSuppPart->m_cQualityLevel;
				pRec->iSeg=pSuppPart->iSeg;
				pRec->length=pSuppPart->length;
				pRec->fPieceNetWeight = pRec->fPieceMinWeight = pSuppPart->piece_weight;
				if(strlen(pSuppPart->sNotes)>0)
					strcpy(pRec->sNotes,pSuppPart->sNotes);
				pRec->AddPart(pSuppPart->nQuantity);
				SummarizePartByModule(limit_str,pRec->iSeg);
			}
		}
		else	//其它构件
		{
			PART *pRec=NULL;
			for(pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
			{
				if(stricmp(pRec->GetPartNo(),pSuppPart->GetPartNo())==0)
				{
					pRec->AddPart(pSuppPart->m_uStatMatCoef);
					if(strlen(pRec->sNotes)==0&&strlen(pSuppPart->sNotes)>0)
						strcpy(pRec->sNotes,pSuppPart->sNotes);
					SummarizePartByModule(limit_str,pRec->iSeg);
					break;
				}
			}
			if(pRec==NULL)
			{
				pRec=part_recordset.append();
				pRec->SetPartNo(pSuppPart->GetPartNo());
				pRec->part_type=TA_OTHER;	//其它构件
				pRec->sSpec.Copy(pSuppPart->guige);
				pRec->cMaterial=pSuppPart->cMaterial;
				pRec->cQualityLevel=pSuppPart->m_cQualityLevel;
				pRec->iSeg=pSuppPart->iSeg;
				pRec->length=pSuppPart->length;
				pRec->fPieceNetWeight = pRec->fPieceMinWeight = pSuppPart->piece_weight;
				if(strlen(pSuppPart->sNotes)>0)
					strcpy(pRec->sNotes,pSuppPart->sNotes);
				pRec->AddPart(pSuppPart->nQuantity);
				SummarizePartByModule(limit_str,pRec->iSeg);
			}
		}
	}
}

int compare_partref_partno(const PARTREF& partref1,const PARTREF& partref2)
{
	return ComparePartNoString((char*)partref1.sPartNo,(char*)partref2.sPartNo);
}
typedef CLDSPartGroup* CLDSPartGroupPtr;
int compare_partgroup_partno(const CLDSPartGroupPtr& partref1,const CLDSPartGroupPtr& partref2)
{
	return ComparePartNoString((char*)partref1->sRootPartNo,(char*)partref2->sRootPartNo);
}
void CTowerType::SummarizePartGroup()	//统计组焊件
{
	CLDSPartGroup *pPartGroup=NULL;
	ARRAY_LIST<CLDSPartGroupPtr> weldPartArr;
	weldPartArr.SetSize(0,100);
	for(pPartGroup=m_pModel->EnumPartGroupFirst();pPartGroup;pPartGroup=m_pModel->EnumPartGroupNext())
	{
		CLDSPart *pRootPart=m_pModel->FromPartHandle(pPartGroup->handle);
		if(pRootPart==NULL)
			continue;
		strcpy(pPartGroup->sRootPartNo,pRootPart->GetPartNo());
		weldPartArr.append(pPartGroup);
	}
	CQuickSort<CLDSPartGroupPtr>::QuickSort(weldPartArr.m_pData,weldPartArr.GetSize(),compare_partgroup_partno);
	for(int index=0;index<weldPartArr.GetSize();index++)
	{
		pPartGroup=weldPartArr[index];
		CLDSPart *pRootPart=m_pModel->FromPartHandle(pPartGroup->handle);
		if(pRootPart==NULL)
			continue;
		PARTSET partset;
		m_pModel->GetPartGroupPartSet(pPartGroup->handle,partset);
		partset.append(pRootPart);
		CStatWeldPart *pWeldPart=pWeldPart=partgroupset.append();
		pWeldPart->iSeg=pRootPart->iSeg;	//组焊件段号
		pWeldPart->num=(short)pPartGroup->m_uStatMatCoef;
		strcpy(pWeldPart->sName,pPartGroup->sGroupName);
		strcpy(pWeldPart->sPartNo,pRootPart->GetPartNo());
		for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			limit_str[0]='\0';
			int iNo=1;
			for(CLDSModule *pModule=m_pTower->Module.GetFirst();pModule;pModule=m_pTower->Module.GetNext(),iNo++)
			{
				if(pPart->cfgword.IsHasBodyNo(pModule->GetBodyNo()))
				{
					char ss[4];
					if(strlen(limit_str)>0)	//已有数据
						sprintf(ss,",%d",iNo);
					else					//还无数据
						sprintf(ss,"%d",iNo);
					strcat(limit_str,ss);
				}
			}
			PARTREF *pRec=NULL;
			for(pRec=pWeldPart->GetPartRefArrPtr()->GetFirst();pRec;pRec=pWeldPart->GetPartRefArrPtr()->GetNext())
			{
				if(stricmp(pRec->sPartNo,pPart->GetPartNo())==0)
				{
					pRec->num++;	//此处不用考虑统材系数问题，只需统计一个组焊件中的构件个数
					//SummarizePartByModule(limit_str,pRec->iSeg);
					break;
				}
			}
			if(pRec==NULL)
			{
				pRec=pWeldPart->GetPartRefArrPtr()->append();
				strcpy(pRec->sPartNo,pPart->GetPartNo());
				pRec->num=1;
				//SummarizePartByModule(limit_str,pRec->iSeg);
			}
		}
		//将组焊件中的构件按构件编号排序
		int i=0,part_n=pWeldPart->GetPartRefArrPtr()->GetNodeNum();
		ARRAY_LIST<PARTREF> part_arr;
		part_arr.SetSize(0,part_n);
		PARTREF *pRec=NULL;
		for(pRec=pWeldPart->GetPartRefArrPtr()->GetFirst();pRec;pRec=pWeldPart->GetPartRefArrPtr()->GetNext())
			part_arr.append(*pRec);
		CBubbleSort<PARTREF>::BubSort(part_arr.m_pData,part_arr.GetSize(),compare_partref_partno);
		i=0;
		for(pRec=pWeldPart->GetPartRefArrPtr()->GetFirst();pRec;pRec=pWeldPart->GetPartRefArrPtr()->GetNext())
		{
			*pRec=part_arr[i];
			i++;
		}
	}

	//将组焊件按编号排序
	/*int i=0,partgroup_n=partgroupset.GetNodeNum();
	CStatWeldPart *partgroup_arr=new CStatWeldPart[partgroup_n];
	i=0;
	CStatWeldPart *pRec=NULL;
	for(pRec=partgroupset.GetFirst();pRec;pRec=partgroupset.GetNext())
	{
		partgroup_arr[i]=*pRec;
		i++;
	}
	for(i=0;i<partgroup_n-1;i++)
	{
		BOOL bSort=TRUE;
		for(int j=0;j<partgroup_n-i-1;j++)
		{
			if(stricmp(partgroup_arr[j].sPartNo,partgroup_arr[j+1].sPartNo)>0)
			{
				CStatWeldPart tm;
				tm=partgroup_arr[j];
				partgroup_arr[j]=partgroup_arr[j+1];
				partgroup_arr[j+1]=tm;
			}
			else
				continue;
		}
		if(bSort)	//已有序
			break;
	}
	i=0;
	for(pRec=partgroupset.GetFirst();pRec;pRec=partgroupset.GetNext())
	{
		*pRec=partgroup_arr[i];
		i++;
	}
	delete []partgroup_arr;*/
}
BOOL CTowerType::RunSummarize(CTower* pTower,bool bIncLsSegI/*=false*/)	//进行材料统计
{
	if(pTower==NULL)
		return FALSE;
	m_pModel = pTower;
	m_pTower = pTower;
	m_bStatLsBySegI=bIncLsSegI;
	// 进行分段号统计
	segIList.Empty();	//清空原有分段号
	int nSegN=m_pTower->GetSegNoList(segIList);
	if(nSegN<=0)
		return FALSE;
	// 进行加工呼高模型统计
	moduleset.Empty();
	int iNo=1;
	for(CLDSModule *pModule=m_pTower->Module.GetFirst();pModule;pModule=m_pTower->Module.GetNext(),iNo++)
	{
		CStatModule *pStatModule=moduleset.append();
		pStatModule->iNo=iNo;
		pStatModule->m_fBurglarLsStartZ = pModule->m_fBurglarLsStartZ;
		pStatModule->m_fBurglarLsEndZ = pModule->m_fBurglarLsEndZ;
		strcpy(pStatModule->description,pModule->description);
	}

	SummarizeParts();		//统计构件集合内的材料
	SummarizeSuppPart();	//统计附加构件的材料
	SummarizePartGroup();	//统计组焊件
	SummarizeBlockRefs();	//统计部件引用
	return TRUE;
}
BOOL CTowerType::SummarizeFLs(CTower* pTower)
{
	if(pTower==NULL)
		return FALSE;
	m_pModel = pTower;
	m_pTower = pTower;
	segIList.Empty();	//清空原有分段号
	int nSegN=m_pTower->GetSegNoList(segIList);
	if(nSegN<=0)
		return FALSE;
	for(CLDSPart* pPart=m_pModel->EnumPartFirst();pPart!=NULL;pPart=m_pModel->EnumPartNext())
	{
		if(pPart->m_bVirtualPart)
			continue;
		else if(pPart->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		CLDSParamPlate* pFL=(CLDSParamPlate*)pPart;
		if(pFL->m_iParamType!=TYPE_FL)
			continue;
		CFlRecord* pFLRec=NULL;
		for(pFLRec=fl_recordset.GetFirst();pFLRec;pFLRec=fl_recordset.GetNext())
		{
			if(stricmp(pFLRec->GetPartNo(),pFL->GetPartNo())==0&&pFLRec->iSeg.iSeg==pFL->iSeg)
			{
				pFLRec->AddPart(pFL->m_uStatMatCoef);
				if(strlen(pFLRec->sNotes)==0&&strlen(pFL->sNotes)>0)
					strcpy(pFLRec->sNotes,pFL->sNotes);
				break;
			}
		}
		if(pFLRec==NULL)
		{
			pFLRec=fl_recordset.append();
			pFLRec->AddPart();
			pFLRec->iSeg=pFL->iSeg;
			pFLRec->SetPartNo(pFL->GetPartNo());
			pFLRec->thick=pFL->GetThick();
			strcpy(pFLRec->sNotes,pFL->sNotes);
			double R,D;
			pFL->GetDesignItemValue('R', &R);	//法兰内径
			pFL->GetDesignItemValue('W', &D);	//法兰外径
			pFLRec->d=R*2;
			pFLRec->D=D;
			pFLRec->nLsNum=pFL->GetLsRefList()->GetNodeNum();
			if(pFLRec->nLsNum>0)
			{
				CLsRef* pLsRef=pFL->GetFirstLsRef();
				CLDSBolt* pBolt=pLsRef->GetLsPtr();
				pFLRec->ls_d=pBolt->get_d();
				//计算螺栓布孔直径
				f3dPoint centre(pBolt->ucs.origin);
				coord_trans(centre,pFL->ucs,FALSE);
				centre.z=0;
				pFLRec->K=centre.mod()*2;
			}
			else
			{
				pFLRec->ls_d=0;
				pFLRec->K=0;
			}
		}
	}
	return TRUE;
}
void CTowerType::SortFL()
{
	int i=0,fl_n=fl_recordset.GetNodeNum();
	CFlRecord *fl_arr=new CFlRecord[fl_n];
	for(CFlRecord *pRec=fl_recordset.GetFirst();pRec;pRec=fl_recordset.GetNext())
	{
		fl_arr[i]=*pRec;
		i++;
	}
	for(i=0;i<fl_n-1;i++)
	{
		BOOL bSort=TRUE;
		for(int j=0;j<fl_n-i-1;j++)
		{
			if(fl_arr[j].iSeg.iSeg>fl_arr[j+1].iSeg.iSeg)
			{
				CFlRecord tm;
				tm=fl_arr[j];
				fl_arr[j]=fl_arr[j+1];
				fl_arr[j+1]=tm;
				bSort=FALSE;
			}
			else if(fl_arr[j].iSeg.iSeg==fl_arr[j+1].iSeg.iSeg)
			{
				if(ComparePartNoString(fl_arr[j].GetPartNo(),fl_arr[j+1].GetPartNo())>0)
				{
					CFlRecord tm;
					tm=fl_arr[j];
					fl_arr[j]=fl_arr[j+1];
					fl_arr[j+1]=tm;
					bSort=FALSE;
				}
			}
			else
				continue;
		}
		if(bSort)	//已有序
			break;
	}
	i=0;
	for(CFlRecord *pRec=fl_recordset.GetFirst();pRec;pRec=fl_recordset.GetNext())
	{
		*pRec=fl_arr[i];
		i++;
	}
	delete []fl_arr;
}
void CTowerType::SortPart(int partno0_guige1_onlyguige2/*=0*/,BOOL bDifferMat/*=TRUE*/,BOOL bAscending/*=TRUE*/)
{
	int i=0,part_n=part_recordset.GetNodeNum();
	PART *part_arr=new PART[part_n];
	i=0;
	PART *pRec=NULL;
	for(pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
	{
		part_arr[i]=*pRec;
		i++;
	}
	if(partno0_guige1_onlyguige2==0)
	{	//按构件编号进行排序
		for(i=0;i<part_n-1;i++)
		{
			BOOL bSort=TRUE;
			for(int j=0;j<part_n-i-1;j++)
			{
				if(part_arr[j].iSeg.iSeg>part_arr[j+1].iSeg.iSeg)
				{
					PART tm;
					tm=part_arr[j];
					part_arr[j]=part_arr[j+1];
					part_arr[j+1]=tm;
					bSort=FALSE;
				}
				else if(part_arr[j].iSeg.iSeg==part_arr[j+1].iSeg.iSeg)
				{
					if(ComparePartNoString(part_arr[j].GetPartNo(),part_arr[j+1].GetPartNo())>0)
					{
						PART tm;
						tm=part_arr[j];
						part_arr[j]=part_arr[j+1];
						part_arr[j+1]=tm;
						bSort=FALSE;
					}
				}
				else
					continue;
			}
			if(bSort)	//已有序
				break;
		}
	}
	else if(partno0_guige1_onlyguige2==1)
	{	//按规格进行排序(可以分为是否区分材质两种)
		for(i=0;i<part_n-1;i++)
		{
			BOOL bSort=TRUE;
			for(int j=0;j<part_n-i-1;j++)
			{
				if(part_arr[j].iSeg.iSeg>part_arr[j+1].iSeg.iSeg)
				{
					PART tm;
					tm=part_arr[j];
					part_arr[j]=part_arr[j+1];
					part_arr[j+1]=tm;
					bSort=FALSE;
				}
				else if(part_arr[j].iSeg.iSeg==part_arr[j+1].iSeg.iSeg)
				{
					if(part_arr[j].part_type>part_arr[j+1].part_type)
					{	//区分塔材类型
						PART tm;
						tm=part_arr[j];
						part_arr[j]=part_arr[j+1];
						part_arr[j+1]=tm;
						bSort=FALSE;
					}
					else if(part_arr[j].part_type==part_arr[j+1].part_type)
					{
						if(bDifferMat&&part_arr[j].cMaterial<part_arr[j+1].cMaterial)
						{	//区分材质
							PART tm;
							tm=part_arr[j];
							part_arr[j]=part_arr[j+1];
							part_arr[j+1]=tm;
							bSort=FALSE;
						}
						else if(!bDifferMat||part_arr[j].cMaterial==part_arr[j+1].cMaterial)
						{	//材质相同或不区分材质
							char ch;
							double guige_para1=0,guige_para2=0,guige_para3=0,guige_para4=0;
							sscanf(part_arr[j].sSpec,"%c%lf%c%lf",&ch,&guige_para1,&ch,&guige_para2);
							sscanf(part_arr[j+1].sSpec,"%c%lf%c%lf",&ch,&guige_para3,&ch,&guige_para4);
							if(guige_para1>guige_para3)
							{
								PART tm;
								tm=part_arr[j];
								part_arr[j]=part_arr[j+1];
								part_arr[j+1]=tm;
								bSort=FALSE;
							}
							else if(guige_para1==guige_para3&&guige_para2>guige_para4)
							{
								PART tm;
								tm=part_arr[j];
								part_arr[j]=part_arr[j+1];
								part_arr[j+1]=tm;
								bSort=FALSE;
							}
							else if(guige_para1==guige_para3&&guige_para2==guige_para4&&part_arr[j].length>part_arr[j+1].length)
							{
								PART tm;
								tm=part_arr[j];
								part_arr[j]=part_arr[j+1];
								part_arr[j+1]=tm;
								bSort=FALSE;
							}
							else if(guige_para1==guige_para3&&guige_para2==guige_para4&&fabs(part_arr[j].length-part_arr[j+1].length)<0.1)
							{
								//if(stricmp(part_arr[j].sPartNo,part_arr[j+1].sPartNo)>0)
								if(ComparePartNoString(part_arr[j].GetPartNo(),part_arr[j+1].GetPartNo())>0)
								{
									PART tm;
									tm=part_arr[j];
									part_arr[j]=part_arr[j+1];
									part_arr[j+1]=tm;
									bSort=FALSE;
								}
							}
						}
						else
							continue;
					}
				}
			}
			if(bSort)	//已有序
				break;
		}
	}
	else if (partno0_guige1_onlyguige2 == 2)
	{
		for (i = 0; i < part_n - 1; i++)
		{
			BOOL bSort = TRUE;
			for (int j = 0; j < part_n - i - 1; j++)
			{
				if (part_arr[j].part_type > part_arr[j + 1].part_type)
				{	//区分塔材类型
					PART tm;
					tm = part_arr[j];
					part_arr[j] = part_arr[j + 1];
					part_arr[j + 1] = tm;
					bSort = FALSE;
				}
				else if (part_arr[j].part_type == part_arr[j + 1].part_type)
				{
					if (bDifferMat&&part_arr[j].cMaterial < part_arr[j + 1].cMaterial)
					{	//区分材质
						PART tm;
						tm = part_arr[j];
						part_arr[j] = part_arr[j + 1];
						part_arr[j + 1] = tm;
						bSort = FALSE;
					}
					else if (!bDifferMat || part_arr[j].cMaterial == part_arr[j + 1].cMaterial)
					{	//材质相同或不区分材质
						char ch;
						double guige_para1 = 0, guige_para2 = 0, guige_para3 = 0, guige_para4 = 0;
						sscanf(part_arr[j].sSpec, "%c%lf%c%lf", &ch, &guige_para1, &ch, &guige_para2);
						sscanf(part_arr[j + 1].sSpec, "%c%lf%c%lf", &ch, &guige_para3, &ch, &guige_para4);
						if (guige_para1 > guige_para3)
						{
							PART tm;
							tm = part_arr[j];
							part_arr[j] = part_arr[j + 1];
							part_arr[j + 1] = tm;
							bSort = FALSE;
						}
						else if (guige_para1 == guige_para3 && guige_para2 > guige_para4)
						{
							PART tm;
							tm = part_arr[j];
							part_arr[j] = part_arr[j + 1];
							part_arr[j + 1] = tm;
							bSort = FALSE;
						}
						else if (guige_para1 == guige_para3 && guige_para2 == guige_para4 && part_arr[j].length > part_arr[j + 1].length)
						{
							PART tm;
							tm = part_arr[j];
							part_arr[j] = part_arr[j + 1];
							part_arr[j + 1] = tm;
							bSort = FALSE;
						}
						else if (guige_para1 == guige_para3 && guige_para2 == guige_para4 && fabs(part_arr[j].length - part_arr[j + 1].length) < 0.1)
						{
							if (stricmp(part_arr[j].GetPartNo(), part_arr[j + 1].GetPartNo()) > 0)
							{
								PART tm;
								tm = part_arr[j];
								part_arr[j] = part_arr[j + 1];
								part_arr[j + 1] = tm;
								bSort = FALSE;
							}
						}
					}
					else
						continue;
				}
			}
			if (bSort)	//已有序
				break;
		}
	}
	i=0;
	for(pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
	{
		*pRec=part_arr[i];
		i++;
	}
	delete []part_arr;
}
void CTowerType::SortLs()
{
	int i=0,ls_n=ls_recordset.GetNodeNum();
	CLsRecord *ls_arr=new CLsRecord[ls_n];
	i=0;
	CLsRecord *pRec=NULL;
	for(pRec=ls_recordset.GetFirst();pRec;pRec=ls_recordset.GetNext())
	{
		ls_arr[i]=*pRec;
		i++;
	}
	//按规格进行排序(可以分为是否区分材质两种)
	for(i=0;i<ls_n-1;i++)
	{
		BOOL bSort=TRUE;
		for(int j=0;j<ls_n-i-1;j++)
		{
			if(ls_arr[j].iModuleNo>ls_arr[j+1].iModuleNo)
			{
				CLsRecord tm=ls_arr[j];
				ls_arr[j]=ls_arr[j+1];
				ls_arr[j+1]=tm;
				bSort=FALSE;
			}
			else if(ls_arr[j].iModuleNo==ls_arr[j+1].iModuleNo)
			{
				if(m_bStatLsBySegI&&ls_arr[j].iSeg.iSeg>ls_arr[j+1].iSeg.iSeg)
				{
					CLsRecord tm=ls_arr[j];
					ls_arr[j]=ls_arr[j+1];
					ls_arr[j+1]=tm;
					bSort=FALSE;
				}
				else if(m_bStatLsBySegI&&ls_arr[j].iSeg.iSeg<ls_arr[j+1].iSeg.iSeg)
					continue;
				else if(stricmp(ls_arr[j].grade,ls_arr[j+1].grade)>0)
				{	//区分级别类型
					CLsRecord tm=ls_arr[j];
					ls_arr[j]=ls_arr[j+1];
					ls_arr[j+1]=tm;
					bSort=FALSE;
				}
				else if(stricmp(ls_arr[j].grade,ls_arr[j+1].grade)==0)
				{
					if(ls_arr[j].d>ls_arr[j+1].d)
					{	//区分直径
						CLsRecord tm=ls_arr[j];
						ls_arr[j]=ls_arr[j+1];
						ls_arr[j+1]=tm;
						bSort=FALSE;
					}
					else if(ls_arr[j].d==ls_arr[j+1].d)
					{	//直径相同区分有效长度
						if(ls_arr[j].L>ls_arr[j+1].L)
						{
							CLsRecord tm=ls_arr[j];
							ls_arr[j]=ls_arr[j+1];
							ls_arr[j+1]=tm;
							bSort=FALSE;
						}
					}
				}
			}
		}
		if(bSort)	//已有序
			break;
	}

	i=0;
	for(pRec=ls_recordset.GetFirst();pRec;pRec=ls_recordset.GetNext())
	{
		*pRec=ls_arr[i];
		i++;
	}
	delete []ls_arr;
}


bool CTowerType::SummarizePartByModule(char* limit_str, SEGI iSeg)
{
	for(long iNo=FindAdaptNo(limit_str);iNo>0;iNo=FindAdaptNo(NULL))
	{
		for(CStatModule *pModule=moduleset.GetFirst();pModule;pModule=moduleset.GetNext())
		{
			if(pModule->iNo==iNo)
			{
				pModule->segNoHashList.SetValue(iSeg.iSeg,iSeg);
				break;
			}
		}
	}
	return true;
}
bool CTowerType::GetStatMatRecordJg(char cMaterial,ATOM_LIST<PARTSPEC_RECORD>& matrecList)	//索引值以0为基数
{
	matrecList.Empty();
	for(PART *pPart=part_recordset.GetFirst();pPart;pPart=part_recordset.GetNext())
	{
		if(pPart->part_type!=STEELTYPE_ANGLE || pPart->cMaterial!=cMaterial)
			continue;
		char spec[24];
		sprintf(spec,"L%.0fX%.0f",pPart->wide,pPart->thick);
		PARTSPEC_RECORD *pRec;
		double add_weight=pPart->fPieceMinWeight*pPart->GetPartNum();
		for(pRec=matrecList.GetFirst();pRec;pRec=matrecList.GetNext())
		{
			if(stricmp(pRec->guige,spec)==0)
			{
				double *pSegWeight=pRec->segWeightList.GetFirst();
				for(SEGI *pSegI=segIList.GetFirst();pSegI;pSegI=segIList.GetNext())
				{
					if((*pSegI).iSeg==pPart->iSeg.iSeg)
					{
						*pSegWeight+=add_weight;
						break;
					}
					pSegWeight=pRec->segWeightList.GetNext();
				}
				double *pBodyWeight=pRec->bodyWeightList.GetFirst();
				for(CStatModule *pStatModule=moduleset.GetFirst();pStatModule;pStatModule=moduleset.GetNext())
				{
					if(pStatModule->segNoHashList.GetValue(pPart->iSeg.iSeg)!=NULL)
						*pBodyWeight+=add_weight;
					pBodyWeight=pRec->bodyWeightList.GetNext();
				}
				break;
			}
		}
		if(pRec==NULL)
		{
			pRec=matrecList.append();
			strcpy(pRec->guige,spec);
			for(SEGI *pSegI=segIList.GetFirst();pSegI;pSegI=segIList.GetNext())
			{
				if((*pSegI).iSeg==pPart->iSeg.iSeg)
					pRec->segWeightList.append(add_weight);
				else
					pRec->segWeightList.append(0);
			}
			for(CStatModule *pStatModule=moduleset.GetFirst();pStatModule;pStatModule=moduleset.GetNext())
			{
				if(pStatModule->segNoHashList.GetValue(pPart->iSeg.iSeg)!=NULL)
					pRec->bodyWeightList.append(add_weight);
				else
					pRec->bodyWeightList.append(0);
			}
		}
	}
	return true;
}
bool CTowerType::GetStatMatRecordPlate(char cMaterial,ATOM_LIST<PARTSPEC_RECORD>& matrecList)
{
	matrecList.Empty();
	for(PART *pPart=part_recordset.GetFirst();pPart;pPart=part_recordset.GetNext())
	{
		if(pPart->part_type!=STEELTYPE_PLATE || pPart->cMaterial!=cMaterial)
			continue;
		char spec[24];
		sprintf(spec,"-%.0f",pPart->thick);
		PARTSPEC_RECORD *pRec;
		double add_weight=pPart->fPieceMinWeight*pPart->GetPartNum();
		for(pRec=matrecList.GetFirst();pRec;pRec=matrecList.GetNext())
		{
			if(stricmp(pRec->guige,spec)==0)
			{
				double *pSegWeight=pRec->segWeightList.GetFirst();
				for(SEGI *pSegI=segIList.GetFirst();pSegI;pSegI=segIList.GetNext())
				{
					if((*pSegI).iSeg==pPart->iSeg.iSeg)
					{
						*pSegWeight+=add_weight;
						break;
					}
					pSegWeight=pRec->segWeightList.GetNext();
				}
				double *pBodyWeight=pRec->bodyWeightList.GetFirst();
				for(CStatModule *pStatModule=moduleset.GetFirst();pStatModule;pStatModule=moduleset.GetNext())
				{
					if(pStatModule->segNoHashList.GetValue(pPart->iSeg.iSeg)!=NULL)
						*pBodyWeight+=add_weight;
					pBodyWeight=pRec->bodyWeightList.GetNext();
				}
				break;
			}
		}
		if(pRec==NULL)
		{
			pRec=matrecList.append();
			strcpy(pRec->guige,spec);
			for(SEGI *pSegI=segIList.GetFirst();pSegI;pSegI=segIList.GetNext())
			{
				if((*pSegI).iSeg==pPart->iSeg.iSeg)
					pRec->segWeightList.append(add_weight);
				else
					pRec->segWeightList.append(0);
			}
			for(CStatModule *pStatModule=moduleset.GetFirst();pStatModule;pStatModule=moduleset.GetNext())
			{
				if(pStatModule->segNoHashList.GetValue(pPart->iSeg.iSeg)!=NULL)
					pRec->bodyWeightList.append(add_weight);
				else
					pRec->bodyWeightList.append(0);
			}
		}
	}
	return true;
}

bool CTowerType::GetStatMatRecordLs(char* grade,ATOM_LIST<PARTSPEC_RECORD>& matrecList)
{
	matrecList.Empty();
	for(CLsRecord *pLsRec=ls_recordset.GetFirst();pLsRec;pLsRec=ls_recordset.GetNext())
	{
		if(pLsRec->typels0_pad1!=0 || pLsRec->bFootNail || stricmp(pLsRec->grade,grade)!=0)
			continue;	//垫圈或脚钉
		/*char spec[24];
		sprintf(spec,"M%dX%d",pLsRec->d,pLsRec->L);
		PARTSPEC_RECORD *pRec=NULL;
		double add_weight=pLsRec->unit_weight*pLsRec->nPart;
		for(pRec=matrecList.GetFirst();pRec;pRec=matrecList.GetNext())
		{
			if(stricmp(pRec->guige,spec)==0)
			{
				double *pSegWeight=pRec->segWeightList.GetFirst();
				for(SEGI *pSegI=segIList.GetFirst();pSegI;pSegI=segIList.GetNext())
				{
					if(*pSegI==pLsRec->iSeg)
					{
						*pSegWeight+=add_weight;
						break;
					}
					pSegWeight=pRec->segWeightList.GetNext();
				}
				double *pBodyWeight=pRec->bodyWeightList.GetFirst();
				for(CStatModule *pStatModule=moduleset.GetFirst();pStatModule;pStatModule=moduleset.GetNext())
				{
					if(pStatModule->seg_flag_arr[pLsRec->iSeg])
						*pBodyWeight+=add_weight;
					pBodyWeight=pRec->bodyWeightList.GetNext();
				}
				break;
			}
		}
		if(pRec==NULL)
		{
			pRec=matrecList.append();
			strcpy(pRec->guige,spec);
			for(SEGI *pSegI=segIList.GetFirst();pSegI;pSegI=segIList.GetNext())
			{
				if(*pSegI==pLsRec->iSeg)
					pRec->segWeightList.append(add_weight);
				else
					pRec->segWeightList.append(0);
			}
			for(CStatModule *pStatModule=moduleset.GetFirst();pStatModule;pStatModule=moduleset.GetNext())
			{
				if(pStatModule->seg_flag_arr[pLsRec->iSeg])
					pRec->bodyWeightList.append(add_weight);
				else
					pRec->bodyWeightList.append(0);
			}
		}*/
	}
	return true;
}

bool CTowerType::GetStatMatRecordNail(char* grade,ATOM_LIST<PARTSPEC_RECORD>& matrecList)		//索引值以0为基数
{
	matrecList.Empty();
	for(CLsRecord *pLsRec=ls_recordset.GetFirst();pLsRec;pLsRec=ls_recordset.GetNext())
	{
		if(pLsRec->typels0_pad1!=0 || !pLsRec->bFootNail)
			continue;	//垫圈或螺栓
		char spec[24];
		sprintf(spec,"M%dX%d",pLsRec->d,pLsRec->L);
		PARTSPEC_RECORD *pRec;
		double add_weight=pLsRec->sum_weight;//pLsRec->unit_weight*pLsRec->nPart;
		for(pRec=matrecList.GetFirst();pRec;pRec=matrecList.GetNext())
		{
			if(stricmp(pRec->guige,spec)==0)
			{
				double *pSegWeight=pRec->segWeightList.GetFirst();
				for(SEGI *pSegI=segIList.GetFirst();pSegI;pSegI=segIList.GetNext())
				{
					if((*pSegI).iSeg==pLsRec->iSeg.iSeg)
					{
						*pSegWeight+=add_weight;
						break;
					}
					pSegWeight=pRec->segWeightList.GetNext();
				}
				double *pBodyWeight=pRec->bodyWeightList.GetFirst();
				for(CStatModule *pStatModule=moduleset.GetFirst();pStatModule;pStatModule=moduleset.GetNext())
				{
					if(pStatModule->segNoHashList.GetValue(pLsRec->iSeg.iSeg)!=NULL)
						*pBodyWeight+=add_weight;
					pBodyWeight=pRec->bodyWeightList.GetNext();
				}
				break;
			}
		}
		if(pRec==NULL)
		{
			pRec=matrecList.append();
			strcpy(pRec->guige,spec);
			for(SEGI *pSegI=segIList.GetFirst();pSegI;pSegI=segIList.GetNext())
			{
				if((*pSegI).iSeg==pLsRec->iSeg.iSeg)
					pRec->segWeightList.append(add_weight);
				else
					pRec->segWeightList.append(0);
			}
			for(CStatModule *pStatModule=moduleset.GetFirst();pStatModule;pStatModule=moduleset.GetNext())
			{
				if(pStatModule->segNoHashList.GetValue(pLsRec->iSeg.iSeg)!=NULL)
					pRec->bodyWeightList.append(add_weight);
				else
					pRec->bodyWeightList.append(0);
			}
		}
	}
	return true;
}
bool CTowerType::GetStatMatRecordWasher(ATOM_LIST<PARTSPEC_RECORD>& matrecList)		//索引值以0为基数
{
	matrecList.Empty();
	for(CLsRecord *pLsRec=ls_recordset.GetFirst();pLsRec;pLsRec=ls_recordset.GetNext())
	{
		if(pLsRec->typels0_pad1!=1)
			continue;	//螺栓或脚钉
		char spec[24];
		sprintf(spec,"M%dX%d",pLsRec->d,pLsRec->L);
		PARTSPEC_RECORD *pRec;
		double add_weight=pLsRec->sum_weight;//pLsRec->unit_weight*pLsRec->nPart;
		for(pRec=matrecList.GetFirst();pRec;pRec=matrecList.GetNext())
		{
			if(stricmp(pRec->guige,spec)==0)
			{
				double *pSegWeight=pRec->segWeightList.GetFirst();
				for(SEGI *pSegI=segIList.GetFirst();pSegI;pSegI=segIList.GetNext())
				{
					if((*pSegI).iSeg==pLsRec->iSeg.iSeg)
					{
						*pSegWeight+=add_weight;
						break;
					}
					pSegWeight=pRec->segWeightList.GetNext();
				}
				double *pBodyWeight=pRec->bodyWeightList.GetFirst();
				for(CStatModule *pStatModule=moduleset.GetFirst();pStatModule;pStatModule=moduleset.GetNext())
				{
					if(pStatModule->segNoHashList.GetValue(pLsRec->iSeg.iSeg)!=NULL)
						*pBodyWeight+=add_weight;
					pBodyWeight=pRec->bodyWeightList.GetNext();
				}
				break;
			}
		}
		if(pRec==NULL)
		{
			pRec=matrecList.append();
			strcpy(pRec->guige,spec);
			for(SEGI *pSegI=segIList.GetFirst();pSegI;pSegI=segIList.GetNext())
			{
				if((*pSegI).iSeg==pLsRec->iSeg.iSeg)
					pRec->segWeightList.append(add_weight);
				else
					pRec->segWeightList.append(0);
			}
			for(CStatModule *pStatModule=moduleset.GetFirst();pStatModule;pStatModule=moduleset.GetNext())
			{
				if(pStatModule->segNoHashList.GetValue(pLsRec->iSeg.iSeg)!=NULL)
					pRec->bodyWeightList.append(add_weight);
				else
					pRec->bodyWeightList.append(0);
			}
		}
	}
	return true;
}
bool CTowerType::GetSegStatMatRecordPart(SEGI iSeg,ATOM_LIST<SEGPARTNO_RECORD>& matrecList)
{
	matrecList.Empty();
	SortPart(0,FALSE);
	for(PART *pPart=part_recordset.GetFirst();pPart;pPart=part_recordset.GetNext())
	{
		if(pPart->iSeg.iSeg!=iSeg.iSeg)
			continue;	// 不是当前统计段
		SEGPARTNO_RECORD *pRec=matrecList.append();
		char steelmat[20];
		if(pPart->cMaterial=='S')
		{
			if(pPart->part_type==STEELTYPE_ANGLE)	//角钢
				sprintf(pRec->guige,"L%.0fX%.0f",pPart->wide,pPart->thick);
			else if(pPart->part_type==STEELTYPE_PLATE)//钢板
				sprintf(pRec->guige,"-%.0fX%.0f",pPart->thick,pPart->wide);
		}
		else
		{
			QuerySteelMatMark(pPart->cMaterial,steelmat);
			if(pPart->part_type==STEELTYPE_ANGLE)	//角钢
				sprintf(pRec->guige,"%sL%.0fX%.0f",steelmat,pPart->wide,pPart->thick);
			else if(pPart->part_type==STEELTYPE_PLATE)//钢板
				sprintf(pRec->guige,"%s-%.0fX%.0f",steelmat,pPart->thick,pPart->wide);
		}
		pRec->length = pPart->length;
		pRec->N = pPart->GetPartNum();
		if(CLDSPlate::m_iPlankWeightStatType==2)
			pRec->unit_weight = pPart->fPieceNetWeight;
		else
			pRec->unit_weight = pPart->fPieceMinWeight;
		pPart->GetProcessNotes(pRec->sNotes);
	}
	return true;
}
bool CTowerType::GetSegStatMatRecordLs(SEGI iSeg,ATOM_LIST<SEGLS_SPEC_RECORD>& matrecList)
{
	matrecList.Empty();
	SortLs();
	for(CLsRecord *pLsRec=ls_recordset.GetFirst();pLsRec;pLsRec=ls_recordset.GetNext())
	{
		if(pLsRec->iSeg.iSeg!=iSeg.iSeg || pLsRec->typels0_pad1!=0 || pLsRec->bFootNail)
			continue;	//不是当前统计段,或统计类型
		SEGLS_SPEC_RECORD *pSegLsRec= matrecList.append();
		sprintf(pSegLsRec->guige,"M%dX%d",pLsRec->d,pLsRec->L);
		//pSegLsRec->unit_weight = pLsRec->unit_weight;
		pSegLsRec->d=pLsRec->d;
		pSegLsRec->L=pLsRec->L;
		pSegLsRec->N = pLsRec->GetPartNum();
		strcpy(pSegLsRec->grade,pLsRec->grade);
		strcpy(pSegLsRec->sNotes,"");
	}
	return true;
}
bool CTowerType::GetSegStatMatRecordNail(SEGI iSeg,ATOM_LIST<SEGLS_SPEC_RECORD>& matrecList)
{
	matrecList.Empty();
	SortLs();
	for(CLsRecord *pLsRec=ls_recordset.GetFirst();pLsRec;pLsRec=ls_recordset.GetNext())
	{
		if(pLsRec->iSeg.iSeg!=iSeg.iSeg || pLsRec->typels0_pad1!=0 || !pLsRec->bFootNail)
			continue;	//不是当前统计段,或统计类型
		SEGLS_SPEC_RECORD *pSegLsRec= matrecList.append();
		sprintf(pSegLsRec->guige,"M%dX%d",pLsRec->d,pLsRec->L);
		//pSegLsRec->unit_weight = pLsRec->unit_weight;
		pSegLsRec->d=pLsRec->d;
		pSegLsRec->L=pLsRec->L;
		pSegLsRec->N = pLsRec->GetPartNum();
		strcpy(pSegLsRec->grade,pLsRec->grade);
		strcpy(pSegLsRec->sNotes,"");
	}
	return true;
}
bool CTowerType::GetSegStatMatRecordWasher(SEGI iSeg,ATOM_LIST<SEGLS_SPEC_RECORD>& matrecList)
{
	matrecList.Empty();
	SortLs();
	for(CLsRecord *pLsRec=ls_recordset.GetFirst();pLsRec;pLsRec=ls_recordset.GetNext())
	{
		if(pLsRec->iSeg.iSeg!=iSeg.iSeg || pLsRec->typels0_pad1!=1)
			continue;	//不是当前统计段,或统计类型
		SEGLS_SPEC_RECORD *pSegLsRec= matrecList.append();
		sprintf(pSegLsRec->guige,"M%dX%d",pLsRec->d,pLsRec->L);
		//pSegLsRec->unit_weight = pLsRec->unit_weight;
		pSegLsRec->d=pLsRec->d;
		pSegLsRec->L=pLsRec->L;
		pSegLsRec->N = pLsRec->GetPartNum();
		strcpy(pSegLsRec->grade,pLsRec->grade);
		strcpy(pSegLsRec->sNotes,"");
	}
	return true;
}

PART *CTowerType::GetPartFromPartNo(char *sPartNo)	//主要用于通过组焊件的PARTREF查找相关PART
{
	PART *pRec=NULL;
	for(pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
	{
		if(stricmp(pRec->GetPartNo(),sPartNo)==0)
			break;
	}
	return pRec;
}
void CTowerType::SummarizeBlockRefs()
{
	if(m_pTower==NULL)
		return;
	for(CBlockReference *pBlockRef=m_pTower->BlockRef.GetFirst();pBlockRef;pBlockRef=m_pTower->BlockRef.GetNext())
		SummarizeOneBlockRef(pBlockRef);
}
void CTowerType::SummarizeOneBlockRef(CBlockReference *pBlockRef)
{
	if(m_pTower==NULL)
		return;
	if(pBlockRef->IsExploded())
		return;	//打碎后的部件引用，会将对应的部件添加到对应的塔中，所以不用根据部件进行材料统计
	CBlockModel *pBlock = m_pTower->Block.FromHandle(pBlockRef->m_hBlock);
	if(pBlock==NULL)
		return;
	if(pBlock->IsEmbeded())
	{

	}
	else 
	{
		//根据部件引用段号及配材号更新部件内构件的段号及配材号
		pBlockRef->UpdateBlockSegNoAndCfgWord();
		IModel *pOldModel=m_pModel;
		m_pModel=pBlock;
		SummarizeParts();		//统计构件集合内的材料
		SummarizePartGroup();	//统计组焊件
		m_pModel=pOldModel;
	}
}
#endif