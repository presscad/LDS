//<LOCALE_TRANSLATE Confirm by hxr/>
// ExtendItemInfo.cpp: implementation of the COutputInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "lds.h"
#include "OutputInfo.h"
#include "env_def.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "CheckPartNoDlg.h"
#include "GlobalFunc.h"
#include "ArrayList.h"
#include "EmendBoltGasket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COutputInfo* COutputInfoAgent::Append(long hRelaObj1,long hRelaObj2,char* format,...)
{
	if(ListAppendFunc==NULL)
		return NULL;
	COutputInfo* pInfo=(*ListAppendFunc)(m_pContext);
	pInfo->m_uType=m_uType;
	pInfo->m_hRelaObj1=hRelaObj1;
	pInfo->m_hRelaObj2=hRelaObj2;
	va_list marker;
	va_start(marker, format);
	char lineText[201];
	memset(lineText,0,201);
	_vsnprintf(lineText,200,format,marker);
	int str_len=strlen(lineText);
	pInfo->m_sDescription.Format("%s",lineText);
	return pInfo;
}
static CTaskOutputList _localListTemporyWarningOutput;
bool ReflectErrorMsgTo(int iWarningType,long hRelaObj[2],const char* errmsg)
{
	if(errmsg==NULL)	//为空表示清空当前临时任务信息链表
		_localListTemporyWarningOutput.Empty();
	else
	{
		COutputInfo* pOutput=_localListTemporyWarningOutput.AppendSimpleOutput();
		pOutput->m_uType=iWarningType;
		pOutput->m_hRelaObj1=hRelaObj[0];
		pOutput->m_hRelaObj2=hRelaObj[1];
		pOutput->m_sDescription=errmsg;
	}
	return true;
}
void StartTaskOutputPage()
{
	CMainFrame* pMainWnd=((CMainFrame*)theApp.m_pMainWnd);
	COutputInfoDlg *pTaskListDlg = pMainWnd->GetOutputInfoPage();
	if(!g_sysPara.dock.pageOutput.bDisplay)
		pMainWnd->ShowDockpageOutput();
	pTaskListDlg->InitOutputInfoList(_localListTemporyWarningOutput);
	char sTitle[50]="";
#ifdef AFX_TARG_ENU_ENGLISH
	if(_localListTemporyWarningOutput.GetNodeNum()>0)
		sprintf(sTitle,"%d task",_localListTemporyWarningOutput.GetNodeNum());
	else 
		sprintf(sTitle,"0 task");
#else 
	if(_localListTemporyWarningOutput.GetNodeNum()>0)
		sprintf(sTitle,"%d项任务",_localListTemporyWarningOutput.GetNodeNum());
	else 
		sprintf(sTitle,"0项任务");
#endif
	pTaskListDlg->SetInfoTitle(sTitle);
	pMainWnd->ActivateDockpage(RUNTIME_CLASS(COutputInfoDlg));
}

CTaskLogErrorLife::CTaskLogErrorLife(CLogFile* pLogFile/*=NULL*/)
	: CLogErrorLife(pLogFile,ReflectErrorMsgTo,StartTaskOutputPage){;}

COutputInfo::COutputInfo()
{
	m_idOutputType=1;
	m_uType=(OUTPUT_INFO_TYPE)0;//类型
#ifdef __PART_DESIGN_INC_
	m_ciElemDiffType=0;
	memset(compare.strbuf,0,16);
#endif
	m_hRelaObj1=0;				//关联对象id1
	m_hRelaObj2=0;				//关联对象id2
	m_sDescription="";			//描述信息
	state=false;
	tolerance=0.5;
}

COutputInfo::~COutputInfo()
{

}
bool COutputInfo::CopyFrom(IBasicOutput* pOutput)
{
	if(GetTypeId()!=pOutput->GetTypeId())
		return false;
	COutputInfo* pCopySrc=(COutputInfo*)pOutput;
	m_hRelaObj1=pCopySrc->m_hRelaObj1;		//关联对象id1
	m_hRelaObj2=pCopySrc->m_hRelaObj2;		//关联对象id2
	m_uType=pCopySrc->m_uType;
	state  = pCopySrc->state;
	tolerance=pCopySrc->tolerance;		//外形校核时的允许误差范围,默认0.5mm
	m_sDescription	=pCopySrc->m_sDescription;	//描述信息
	collisionPt		=pCopySrc->collisionPt;	//实体碰撞点
#ifdef __PART_DESIGN_INC_
	m_ciElemDiffType=pCopySrc->m_ciElemDiffType;
	compare=pCopySrc->compare;
#endif
	return true;
}
SCOPE_STRU GetPartScope(CLDSPart* pPart,SCOPE_STRU *pScope=NULL)
{
	SCOPE_STRU scope;
	if(pScope==NULL)
		pScope=&scope;
	if(pPart->IsLinePart())
	{
		CLDSLinePart* pRod=(CLDSLinePart*)pPart;
		if(pRod->pStart&&pRod->pEnd)
		{
			pScope->VerifyVertex(pRod->pStart->Position(false));
			pScope->VerifyVertex(pRod->pEnd->Position(false));
		}
		else
		{
			pScope->VerifyVertex(pRod->Start());
			pScope->VerifyVertex(pRod->End());
		}
	}
	else if(pPart->pSolidBody&&pPart->pSolidBody->KeyPointNum()>0)
	{
		for(int i=0;i<pPart->pSolidBody->KeyPointNum();i++)
		{
			f3dPoint vertex=pPart->pSolidBody->GetKeyPointAt(i);
			pScope->VerifyVertex(vertex);
		}
	}
	else
		pScope->VerifyVertex(pPart->ucs.origin);
	if(pScope)
		return *pScope;
	else
		return scope;
}
//选中关联对象
void COutputInfo::SelectRelationObj()				
{
	g_pSolidDraw->ReleaseSnapStatus();
	BOOL obj1_selected=false,obj2_selected=false;
	if(m_hRelaObj1>0x20)
		obj1_selected=g_pSolidDraw->SetEntSnapStatus(m_hRelaObj1);
	if(m_hRelaObj2>0x20)
		obj2_selected=g_pSolidDraw->SetEntSnapStatus(m_hRelaObj2);
	SCOPE_STRU scope;
	CLDSDbObject *pRelaObj1=NULL;
	CLDSPart *pPart=NULL;
	CLDSModule* pModule=Ta.GetActiveModule();
#ifdef __PART_DESIGN_INC_
	if(m_uType==INFO_COMPARE_REPORT_ELEMS)
	{
		CLDSNode* pStart=Ta.Node.FromHandle(m_hRelaObj1);
		CLDSNode* pEnd=Ta.Node.FromHandle(m_hRelaObj2);
		if(pStart)
			scope.VerifyVertex(pStart->Position(false));
		if(pEnd)
			scope.VerifyVertex(pEnd->Position(false));
		if((pRelaObj1=pStart)==NULL)
			pRelaObj1=pEnd;
		CElemInfo* pElemInfo=Ta.Elems.GetValue(m_hRelaObj1,m_hRelaObj2);
		if(pElemInfo)
		{
			CLDSLinePart* pRod=Ta.FromRodHandle(pElemInfo->m_hParentRod);
			if(pRod&&!pModule->IsSonPart(pRod))
			{
				for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
				{
					if(!pRod->IsLegObj()&&pModule->IsSonPart(pRod))
					{
						Ta.m_hActiveModule=pModule->handle;
						break;
					}
					else if(pRod->cfgword.IsHasBodyNo(pModule->GetBodyNo()))
					{
						Ta.m_hActiveModule=pModule->handle;
						if(!pModule->IsSonPart(pRod))
						{
							char iQuad=pRod->Layer(2)-'1';
							if(iQuad<0||iQuad>3)
								iQuad=0;
							for(int iLeg=1;iLeg<=192;iLeg++)
							{
								if(pRod->cfgword.IsHasNo(iLeg))
								{
									pModule->m_arrActiveQuadLegNo[iQuad]=iLeg;
									break;
								}
							}
						}
						break;
					}
					g_pSolidDraw->BuildDisplayList(NULL);	
				}
			}
			g_pSolidDraw->SetEntSnapStatus(pElemInfo->Id|0x80000000);
		}
	}
	else
#endif
	{
	//if(m_uType!=INFO_EMEND_CROSSRODINFO)
		pPart = Ta.Parts.FromHandle(m_hRelaObj1);	//选中第一个构件时设置旋转中心
	//else
		CLDSPart* pPart2 = Ta.Parts.FromHandle(m_hRelaObj2);	//选中第一个构件时设置旋转中心
	if((pRelaObj1=pPart)==NULL)
		pRelaObj1=pPart2;
	if(pPart||pPart2)
	{
		if(pPart)
			scope=GetPartScope(pPart);
		if(pPart2)
		{
			SCOPE_STRU scope2=GetPartScope(pPart2);
			double ff1=scope.wide()+scope.high()+scope.depth();
			double ff2=scope2.wide()+scope2.high()+scope2.depth();
			if(ff1<ff2)
				scope=scope2;
		}
	}
	else
	{
		CLDSNode *pNode=Ta.Node.FromHandle(m_hRelaObj1);
		if(pNode==NULL)
			return;
		scope.VerifyVertex(pNode->Position(false));
		scope.fMinX-=500;
		scope.fMaxX+=500;
		scope.fMinY-=500;
		scope.fMaxY+=500;
		scope.fMinZ-=500;
		scope.fMaxZ+=500;
	}
	}
	g_pSolidOper->FocusTo(scope,g_sysPara.m_fMaxScaleScr2User);
	f3dPoint center;
	g_pSolidSet->SetOperType(OPER_ROTATE);
	g_pSolidSet->SetZoomStyle(ROT_CENTER);
	scope.centre(&center.x,&center.y,&center.z);
	if(IsCollisionOutputType())
		center=collisionPt;
	g_pSolidSet->SetRotOrg(center);
	g_pSolidDraw->Draw();
	//更新属性栏
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if(obj1_selected||obj2_selected)
		pView->UpdatePropertyPage();
	else if(pRelaObj1)
	{
		pView->UpdatePropertyPage(pRelaObj1);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The object of specified handle is hidden currently!");
#else
		AfxMessageBox("指定句柄的对象当前处于非显示状态！");
#endif
	}
}
bool COutputInfo::IsCollisionOutputType()
{
	if( INFO_CHK_COLLIDE_RODS==m_uType||INFO_CHK_COLLIDE_RODBOLT==m_uType||
		INFO_CHK_COLLIDE_PLATEBOLT==m_uType||INFO_CHK_COLLIDE_BOLTS==m_uType||
		INFO_CHK_COLLIDE_RODPLATE==m_uType||INFO_CHK_COLLIDE_PLATES==m_uType||
		INFO_ONEKEY_CUTANGLE_CHECK==m_uType)
		return true;
	else
		return false;
}
const char* COutputInfo::GetTypeName()
{
	return GetTypeName(m_uType,m_idOutputType);	
}
const char* COutputInfo::GetTypeName(int uType,int m_idOutputType/*=1*/)
{
	const char* sTypeName="";
	if(m_idOutputType==3)
	{	//垫板(圈)增补修订任务输出
		if(uType==ERROR_ISOLATED_BOLT)
			return "冗余孤立螺栓";	//无关联构件的孤立螺栓, 处理方式为:忽略或确认清除
		else if(uType==ERROR_VIRTUAL_HOLE_LIKELY||uType==ONE_PART_BOLT)
			return "单构件连接的假螺栓";//目前有一个关联构件的非脚钉疑似虚拟孔，处理方式为：确认设为虚拟螺栓孔
		else if(uType==ERROR_VIRTUAL_WIREHOLE_LIKELY)
			return "挂线孔假螺栓";//目前有一个关联构件的非脚钉疑似虚拟孔，处理方式为：确认设为虚拟螺栓孔
		else if(uType==ERROR_MISSING_CROSSGASKET)
			return "交叉点处垫板缺失";	//交叉点处垫板缺失，处理方式为：添加垫板或垫圈
		else if(uType==ERROR_MISSING_ENDGASKET)
			return "端头垫板(圈)缺失";
		else if(uType==ERROR_GASKET_THICK)
			return "垫板(圈)厚度错误";	//垫板或垫圈厚度错误，处理方式为：重新修订垫板或垫圈厚度
		else if(uType==ERROR_PART_POS)
			return "端头构件位置错误";	//构件位置错误
		else if(uType==ERROR_BOLTPAD_CUTANGLE)
			return "端头垫板需切角";
		else if(uType==ERROR_GASKET_PROFILE)
			return "近似切角垫板统一化处理";
		else if(uType==ERROR_REDUNDANT_GASKET)
			return "冗余垫板";
		else
			return "未知垫板修正错误";
	}
	//1.智能修正
	if(uType==INFO_EMEND_SINGULAR_BOLTLO)		//螺栓通厚修订
#ifdef AFX_TARG_ENU_ENGLISH
		sTypeName="bolts LO modification";
	else if(uType==INFO_EMEND_CROSSRODINFO)		//update Cross-bracing Members computation information
		sTypeName="update Cross-bracing Members computation information";
	else if(uType==INFO_EMEND_MIRRODINFO)		//modify relative symmetry information
		sTypeName="modify relative symmetry information";
	else if(uType==INFO_EMEND_BOLTGASKET)		//add modify bolt's washers
		sTypeName="add modify bolt's washers";
	else if(uType==INFO_EMEND_LAYER)			//modify object's layer name
		sTypeName="modify object's layer name";
	else if(uType==INFO_EMEND_TUBEENDJOINTLJ)	//check modifying tube end connecting information
		sTypeName="check modifying tube end connecting information";
#else
		sTypeName="螺栓通厚修订";
	else if(uType==INFO_EMEND_CROSSRODINFO)		//更新交叉材计算信息
		sTypeName="更新交叉材计算信息";
	else if(uType==INFO_EMEND_MIRRODINFO)		//修正关联对称信息
		sTypeName="修正关联对称信息";
	else if(uType==INFO_EMEND_BOLTGASKET)		//增补修正螺栓垫片或垫圈
		sTypeName="增补修正螺栓垫板(圈)";
	else if(uType==INFO_EMEND_LAYER)			//修正对象图层名称
		sTypeName="修正对象图层名称";
	else if(uType==INFO_EMEND_TUBEENDJOINTLJ)	//校核修正钢管端头连接信息
		sTypeName="修正钢管端头连接信息";
#endif
	//2.常规校核
	//编号前检查	
	else if(uType==INFO_CHK_MIRROD)				//对比对称杆件的规格材质，以便下一步进行构件编号
#ifdef AFX_TARG_ENU_ENGLISH
		sTypeName="symmetric rods information";
	else if(uType==INFO_CHK_MIRROD_LENGTH)		//datum length of symmetric rods
		sTypeName="datum length of symmetric rods";
	else if(uType==INFO_CHK_MIRPLATE)			//symmetric plates check
		sTypeName="symmetric plates information check";
	else if(uType==INFO_CHK_BOLTOVERLAP)		//overlapped bolts check
		sTypeName="overlapped bolts check";
	else if(uType==INFO_CHK_HOLEMISSING)		//hole missing check
		sTypeName="hole missing check";
	else if(uType==INFO_CHK_REDUNDANTBOLT)		//redundant bolts check
		sTypeName="redundant bolts check";
	//bolts legality check
	else if(uType==INFO_CHK_BOLT_ROLLINGSPACE)	//angle rolling margin
		sTypeName="angle rolling margin";
	else if(uType==INFO_CHK_BOLT_CUTANGLESPACE)	//angle cutting margin
		sTypeName="angle cutting margin";
	else if(uType==INFO_CHK_BOLT_ANGLE_ENDSPACE)//end distance of angle bolts
		sTypeName="end distance of angle bolts";
	else if(uType==INFO_CHK_BOLT_ANGLE_FORGEERR)//angle flat leg conflicts with bolt's hole positon
		sTypeName="angle flat leg conflicts with bolt's hole positon";
	else if(uType==INFO_CHK_BOLT_PLATE_ENDSPACE)//connecting bolts' margin
		sTypeName="connecting bolts' margin";
	else if(uType==INFO_CHK_BOLT_HUOQULINE_SPACE)//connecting bolts' margin
		sTypeName="connecting bolts' margin";
	else if(uType==INFO_CHK_BOLT_L0)			//bolts LO range
		sTypeName="bolts LO range";
	else if(uType==INFO_CHK_BOLT_SPEC)			//bolts specification
		sTypeName="bolts specification";
	else if(uType==INFO_CHK_BOLT_GRADE)			//bolt level does not agree with specification library
		sTypeName="bolt level does not agree with specification library";
	//data completeness check,data completeness test(including parts which can not be shown by wrong solid data, plate parts more often)
	else if(uType==INFO_ROD_LOST_STARTNODE)		//nodes of rod's starting end missing
		sTypeName="nodes of rod's starting end missing";
	else if(uType==INFO_ROD_LOST_ENDNODE)		//nodes of rod's ending end missing
		sTypeName="nodes on rod's ending end missing";
	else if(uType==INFO_NODE_LOST_FATHERROD)	//nodes of parent rod missing
		sTypeName="nodes of parent rod missing";
	else if(uType==INFO_NODE_LOST_ATTACHNODE)	//depend node
		sTypeName="node lack of depend node";
	else if(uType==INFO_NODE_LOST_ATTACHROD)	//depend rod
		sTypeName="node lack of depend rod";
	else if(uType==INFO_NODE_LOST_ATTACHPLANE)	//depend plane
		sTypeName="node lack of depend plane";
	else if(uType==INFO_NODE_LOST_ATTACHPOINT)	//depend datum point
		sTypeName="node lack of depend datum point";
	else if(uType==INFO_PART_LOST_MATERIAL)		//part lack of steel material information
		sTypeName="part lack of steel material information";
	else if(uType==INFO_PART_INVALID_MATERIAL)	//part's steel materil information incorrect
		sTypeName="part's steel material information incorrect";
	else if(uType==INFO_ANGLE_LOST_LSLIST)		//angle lack of bolts chain table
		sTypeName="angle lack of bolts chain table";
	else if(uType==INFO_ANGLESTART_LOST_BENDLINE)	//angle starting lack of bending datum angle
		sTypeName="angle starting lack of bending datum angle";
	else if(uType==INFO_ANGLESTART_LOST_PRJLINE)	//angle starting lack of Collinear position datum angle
		sTypeName="angle starting lack of Collinear position datum angle";
	else if(uType==INFO_ANGLEEND_LOST_BENDLINE)	//angle ending lack of bending datum angle
		sTypeName="angle ending lack of bending datum angle";
	else if(uType==INFO_ANGLEEND_LOST_PRJLINE)	//angle ending lack of Collinear position datum angle
		sTypeName="angle ending lack of Collinear position datum angle";
	else if(uType==INFO_ANGLEWINGX_LOST_REF_ROD)//lack of referrence rod during calculating angle leg X normal
		sTypeName="angle leg X normal";
	else if(uType==INFO_ANGLEWINGY_LOST_REF_ROD)//lack of referrence rod during calculating angle leg Y normal
		sTypeName="angle leg Y normal";
	else if(uType==INFO_PLATE_LOST_DATUM_REF_NODE)		//handle of datum node is empty
		sTypeName="handle of datum node is empty";
	else if(uType==INFO_PLATE_LOST_DATUM_POS_DES_PARA)	//datum point parameters
		sTypeName="datum point parameters";
	else if(uType==INFO_PLATE_LOST_NORM_DES_PARA)	//datum plane normal parameters
		sTypeName="datum plane normal parameters";
	else if(uType==INFO_PLATE_LOST_LINK_REF_ROD)	//connecting rods
		sTypeName="plate connecting rod's information";
	else if(uType==INFO_BOLT_LOST_DATUM_REF_NODE)	//bolts datum node lost
		sTypeName="bolts datum node lost";
	else if(uType==INFO_BOLT_LOST_DATUM_REF_PART)	//bolts datum part lost
		sTypeName="bolts datum part lost";
	else if(uType==INFO_BOLT_LOST_NORM_DES_PARA)	//bolts normal design parameter lost 
		sTypeName="bolts normal design parameter";
	//merge overlap nodes and rods
	else if(uType==INFO_EMEND_OVERLAPNODE)			//merge bolts normal design parameters(modification)
		sTypeName="bolts normal design parameter";
	else if(uType==INFO_EMEND_OVERLAPROD)			//merge overlap rods(modification)
		sTypeName="pverlap rods";
	//check legality of part solid data 
	else if(uType==INFO_BOLT_NORM_ZERO)			//bolt's normal is empty
		sTypeName="bolt's normal is empty";
	else if(uType==INFO_PART_LOCATION_ABNORMAL)			//bolt's normal is empty
		sTypeName="Part's location is abnormal!";
	else if(uType==INFO_INVALID_PART_UCS)		//coordinate system is illegal
		sTypeName="coordinate system is illegal";
	else if(uType==INFO_INVALID_PROFILE)		//illegal appearance, the number of solid vertex is too small, all the length,width and thickness are zero
		sTypeName="invalid appearance";
	//
	else if(uType==INFO_CHK_MODELINTEGRITY)		//check the match relationship between modle inner rod's nodes
		sTypeName="check the match relationship between module inner rod's nodes";
	else if(uType==INFO_CHK_BOLTCONNECT)		//connecting tense of rod's bolts 
		sTypeName="connecting tense of rod's bolts ";
	else if(uType==INFO_CHK_MODELSTABILITY)		//System stability check 
		sTypeName="System stability check";
	//3.check Number
	else if(uType==INFO_CHK_NULLPARTNO)			//No-Number parts
		sTypeName="No-Number parts";
	else if(uType==INFO_CHK_PARTTYPE_DIFFER)	//part type are different
		sTypeName="part type are different";
	else if(uType==INFO_CHK_RODINFO)			//basic information of rods of the same No. 
		sTypeName="basic information of rods of the same No. ";
	else if(uType==INFO_CHK_RODBOLTINFO)		//quantity, specifications and location information of the bolts with the same Number
		sTypeName="rod's bolts information";
	else if(uType==INFO_CHK_ANGLECUT)			//angle cut information(including reverse contrast)
		sTypeName="angle cut and leg cut information of angle";
	else if(uType==INFO_CHK_PLATEINFO)			//thickness, steel material, bending planes, rolling edges of the plates with the same Number
		sTypeName="basic information of plate";
	else if(uType==INFO_CHK_PLATEPROFILEANDBOLT)//plate's bolt information, appearance and corresponding hole position, reverse exchangability
		sTypeName="plate bolts and appearance information";
	//4.collision check
	else if(uType==INFO_CHK_COLLIDE_RODS)		//collision between rod and rod(region can be specified)
		sTypeName="collision between rod and rod";
	else if(uType==INFO_CHK_COLLIDE_RODBOLT)	//collision between rod and bolt(region can be specified)
		sTypeName="collision between rod and bolt";
	else if(uType==INFO_CHK_COLLIDE_PLATEBOLT)	//collision between plate and bolt(region can be specified)
		sTypeName="collision between plate and bolt";
	else if(uType==INFO_CHK_COLLIDE_BOLTS)		//collision between bolt and bolt(region can be specified)
		sTypeName="collision between bolt and bolt";
	else if(uType==INFO_CHK_COLLIDE_RODPLATE)	//collision between rod and connecting plate(region can be specified)
		sTypeName="collision between rod and connecting plate";
	else if(uType==INFO_CHK_COLLIDE_PLATES)		//collision between connecting plate and connecting plate(region can be specified)
		sTypeName="collision between connecting plate and connecting plate";
	//5.查找统计
	else if(uType==INFO_STAT_BOLTRELAPARTS)		//inquire parts relative to bolts
		sTypeName="inquire parts relative to bolts";
	else if(uType==INFO_STAT_RODLENGTH)			//rod statistics in the length region
		sTypeName="rod statistics in the length region";
	else if(uType==INFO_STAT_RODSIZE)			//specified specification rods statistics
		sTypeName="specified specification rods statistics";
	else if(uType==INFO_ONEKEY_CUTANGLE_CHECK)
		sTypeName="one key cut angle check";
	else if(uType==INFO_COMPARE_REPORT_WARNNING||uType==INFO_COMPARE_REPORT_NODES||
		uType==INFO_COMPARE_REPORT_ELEMS||uType==INFO_COMPARE_REPORT_RODS)
		sTypeName="computation report comparison";
#else
		sTypeName="对称杆件信息检查";
	else if(uType==INFO_CHK_MIRROD_LENGTH)		//对称杆件基准长度
		sTypeName="对称杆件基准长度";
	else if(uType==INFO_CHK_MIRPLATE)			//对称钢板检测
		sTypeName="对称钢板信息检查";
	else if(uType==INFO_CHK_BOLTOVERLAP)		//重叠螺栓检测
		sTypeName="重叠螺栓检查";
	else if(uType==INFO_CHK_HOLEMISSING)		//漏孔检测
		sTypeName="漏孔检查";
	else if(uType==INFO_CHK_REDUNDANTBOLT)		//冗余螺栓检测
		sTypeName="冗余螺栓检查";
	//螺栓合法性校核
	else if(uType==INFO_CHK_BOLT_ROLLINGSPACE)	//角钢轧制边距
		sTypeName="角钢轧制边距";
	else if(uType==INFO_CHK_BOLT_CUTANGLESPACE)	//角钢切角边距
		sTypeName="角钢切角边距";
	else if(uType==INFO_CHK_BOLT_ANGLE_ENDSPACE)//角钢螺栓端距
		sTypeName="角钢螺栓端距";
	else if(uType==INFO_CHK_BOLT_ANGLE_FORGEERR)//角钢压扁与螺栓孔位冲突
		sTypeName="角钢压扁肢与螺栓冲突";
	else if(uType==INFO_CHK_BOLT_PLATE_ENDSPACE)//连板螺栓边距
		sTypeName="连板螺栓边距";
	else if(uType==INFO_CHK_BOLT_HUOQULINE_SPACE)//连板螺栓边距
		sTypeName="连板螺栓边距(到火曲线)";
	else if(uType==INFO_CHK_BOLT_L0)			//螺栓通厚范围
		sTypeName="螺栓通厚范围";
	else if(uType==INFO_CHK_BOLT_SPEC)			//螺栓规格
		sTypeName="螺栓规格";
	else if(uType==INFO_CHK_BOLT_GRADE)			//螺栓级别与规格库是否相符
		sTypeName="螺栓级别与规格库不符";
	//数据完整性校核，数据完整性校验(含错误实体数据无法显示的构件, 多为板构件)
	else if(uType==INFO_ROD_LOST_STARTNODE)		//杆件始端节点丢失
		sTypeName="杆件始端节点丢失";
	else if(uType==INFO_ROD_LOST_ENDNODE)		//杆件终端节点丢失
		sTypeName="杆件终端节点丢失";
	else if(uType==INFO_NODE_LOST_FATHERROD)	//节点父杆件丢失
		sTypeName="节点父杆件丢失";
	else if(uType==INFO_NODE_LOST_ATTACHNODE)	//依附节点
		sTypeName="节点缺少依附节点";
	else if(uType==INFO_NODE_LOST_ATTACHROD)	//依附杆件
		sTypeName="节点缺少依附杆件";
	else if(uType==INFO_NODE_LOST_ATTACHPLANE)	//依附平面
		sTypeName="节点缺少依附平面";
	else if(uType==INFO_NODE_LOST_ATTACHPOINT)	//依附基准点
		sTypeName="节点缺少依附基准点";
	else if(uType==INFO_PART_LOST_MATERIAL)		//构件缺少材质信息
		sTypeName="构件缺少材质信息";
	else if(uType==INFO_PART_INVALID_MATERIAL)	//构件材质信息不正确
		sTypeName="构件材质信息不正确";
	else if(uType==INFO_ANGLE_LOST_LSLIST)		//角钢缺少螺栓链表
		sTypeName="角钢缺少螺栓链表";
	else if(uType==INFO_ANGLESTART_LOST_BENDLINE)	//角钢始端缺少火曲基准角钢
		sTypeName="角钢始端缺少火曲基准角钢";
	else if(uType==INFO_ANGLESTART_LOST_PRJLINE)	//角钢始端缺少共线位置基准角钢
		sTypeName="角钢始端缺少共线位置基准角钢";
	else if(uType==INFO_ANGLEEND_LOST_BENDLINE)	//角钢终端缺少火曲基准角钢
		sTypeName="角钢终端缺少火曲基准角钢";
	else if(uType==INFO_ANGLEEND_LOST_PRJLINE)	//角钢终端缺少共线位置基准角钢
		sTypeName="角钢终端缺少共线位置基准角钢";
	else if(uType==INFO_ANGLEWINGX_LOST_REF_ROD)//角钢X肢法线计算过程中缺少参照杆件
		sTypeName="角钢X肢法线";
	else if(uType==INFO_ANGLEWINGY_LOST_REF_ROD)//角钢Y肢法线计算过程中缺少参照杆件
		sTypeName="角钢Y肢法线";
	else if(uType==INFO_PLATE_LOST_DATUM_REF_NODE)		//基准节点句柄为空
		sTypeName="基准节点句柄为空";
	else if(uType==INFO_PLATE_LOST_DATUM_POS_DES_PARA)	//基准点参数
		sTypeName="基准点参数";
	else if(uType==INFO_PLATE_LOST_NORM_DES_PARA)	//基准面法线参数
		sTypeName="基准面法线参数";
	else if(uType==INFO_PLATE_LOST_LINK_REF_ROD)	//连接杆件
		sTypeName="钢板连接杆件信息";
	else if(uType==INFO_BOLT_LOST_DATUM_REF_NODE)	//螺栓丢失基准节点
		sTypeName="螺栓丢失基准节点";
	else if(uType==INFO_BOLT_LOST_DATUM_REF_PART)	//螺栓丢失基准构件
		sTypeName="螺栓丢失基准构件";
	else if(uType==INFO_BOLT_LOST_NORM_DES_PARA)	//螺栓丢失法线设计参数
		sTypeName="螺栓丢失法线设计参数";
	//合并重复节点及重复杆件
	else if(uType==INFO_EMEND_OVERLAPNODE)			//合并重复节点(修正)
		sTypeName="重复节点";
	else if(uType==INFO_EMEND_OVERLAPROD)			//合并重复杆件(修正)
		sTypeName="重复杆件";
	//构件实体数据合法性检查
	else if(uType==INFO_BOLT_NORM_ZERO)			//螺栓法线为空
		sTypeName="螺栓法线为空";
	else if(uType==INFO_PART_LOCATION_ABNORMAL)	//螺栓法线为空
		sTypeName="构件位置异常";
	else if(uType==INFO_INVALID_PART_UCS)		//坐标系不合法
		sTypeName="坐标系不合法";
	else if(uType==INFO_INVALID_PROFILE)		//非法外形,实体顶点数过少、长度或宽度或厚度为0
		sTypeName="非法外形";
	//
	else if(uType==INFO_CHK_MODELINTEGRITY)		//模型内杆件节点的匹配关系检查
		sTypeName="模型内杆件节点的匹配关系检查";
	else if(uType==INFO_CHK_BOLTCONNECT)		//杆件上螺栓连接强度
		sTypeName="杆件上螺栓连接强度";
	else if(uType==INFO_CHK_MODELSTABILITY)		//体系稳定性检查
		sTypeName="体系稳定性检查";
	//3.编号校核
	else if(uType==INFO_CHK_NULLPARTNO)			//空编号构件
		sTypeName="空编号构件";
	else if(uType==INFO_CHK_PARTTYPE_DIFFER)	//构件类型不同
		sTypeName="构件类型不同";
	else if(uType==INFO_CHK_RODINFO)			//同编号杆件基本信息
		sTypeName="杆件基本信息";
	else if(uType==INFO_CHK_RODBOLTINFO)		//同编号杆件上螺栓数量、规格及位置信息
		sTypeName="杆件螺栓信息";
	else if(uType==INFO_CHK_ANGLECUT)			//角钢上切角信息(含颠倒对比)
		sTypeName="角钢压扁切角切肢信息";
	else if(uType==INFO_CHK_PLATEINFO)			//同编号钢板厚度、材质、火曲面数、卷边
		sTypeName="钢板基本信息";
	else if(uType==INFO_CHK_PLATEPROFILEANDBOLT)//钢板螺栓信息、外形及相对孔位，颠倒互换性、
		sTypeName="钢板螺栓及外形信息";
	//4.碰撞校核
	else if(uType==INFO_CHK_COLLIDE_RODS)		//杆件与杆件碰撞(可指定区域)
		sTypeName="杆件与杆件碰撞";
	else if(uType==INFO_CHK_COLLIDE_RODBOLT)	//杆件与螺栓碰撞(可指定区域)
		sTypeName="杆件与螺栓碰撞";
	else if(uType==INFO_CHK_COLLIDE_PLATEBOLT)	//钢板与螺栓碰撞(可指定区域)
		sTypeName="钢板与螺栓碰撞";
	else if(uType==INFO_CHK_COLLIDE_BOLTS)		//螺栓与螺栓碰撞(可指定区域)
		sTypeName="螺栓与螺栓碰撞";
	else if(uType==INFO_CHK_COLLIDE_RODPLATE)	//杆件与连板碰撞(可指定区域)
		sTypeName="杆件与连板碰撞";
	else if(uType==INFO_CHK_COLLIDE_PLATES)		//连板与连板碰撞(可指定区域)
		sTypeName="连板与连板碰撞";
	//5.查找统计
	else if(uType==INFO_STAT_BOLTRELAPARTS)		//查询螺栓关联构件
		sTypeName="查询螺栓关联构件";
	else if(uType==INFO_STAT_RODLENGTH)			//长度区间内的杆件统计
		sTypeName="长度区间内的杆件统计";
	else if(uType==INFO_STAT_RODSIZE)			//指定规格的杆件统计
		sTypeName="指定规格的杆件统计";
	else if(uType==INFO_ONEKEY_CUTANGLE_CHECK)
		sTypeName="一键切角检测";
	else if(uType==INFO_COMPARE_REPORT_WARNNING||uType==INFO_COMPARE_REPORT_NODES||
		uType==INFO_COMPARE_REPORT_ELEMS||uType==INFO_COMPARE_REPORT_RODS)
		sTypeName="计算报告比对";
#endif
	return sTypeName;
}
CString COutputInfo::GetKeyStr()
{
	CString keyStr;
	keyStr.Format("%d+%ld+%ld",m_uType,m_hRelaObj1,m_hRelaObj2);
	return keyStr;
}
bool ReCalBoltGuigeAndGrade(COutputInfo *pOutputInfo,bool bGrade)
{
	CLDSBolt* pBolt=(CLDSBolt*)Ta.Parts.FromHandle(pOutputInfo->m_hRelaObj1,CLS_BOLT);
	return pBolt->CalGuigeAuto(bGrade);
}
bool COutputInfo::Recheck()
{
	char sError[100]="";
	CLDSPart *pPart=Ta.Parts.FromHandle(m_hRelaObj1);
	CLDSPart *pOtherPart=Ta.Parts.FromHandle(m_hRelaObj2);
	if(m_uType==INFO_CHK_MIRROD || m_uType==INFO_CHK_MIRROD_LENGTH)			//对比对称杆件的规格材质及基准长度，以便下一步进行构件编号
	{
		if(pPart&&pOtherPart&&pPart->IsLinePart()&&pOtherPart->IsLinePart())
		{
			DWORD dwStyleFlag=ChkStyleFlag();
			if(!((CLDSLinePart*)pPart)->IsEqualWith((CLDSLinePart*)pOtherPart,0.5,sError,NULL,&dwStyleFlag))
				m_sDescription.Format("%s",sError);
			else
				state=true;
		}
	}
#ifdef __COMMON_PART_INC_
	else if(m_uType==INFO_CHK_MIRPLATE)		//对称钢板检测
	{
		if(pPart&&pOtherPart&&pPart->GetClassTypeId()==CLS_PLATE&&pOtherPart->GetClassTypeId()==CLS_PLATE)
		{
			if(!((CLDSPlate*)pPart)->IsEqual((CLDSPlate*)pOtherPart,0.5,sError))
#ifdef AFX_TARG_ENU_ENGLISH
				m_sDescription.Format("0x%X and 0x%X%s",pPart->handle,pOtherPart->handle,sError);
#else
				m_sDescription.Format("0x%X与0x%X%s",pPart->handle,pOtherPart->handle,sError);
#endif
			else
				state=true;
		}
	}
	else if(m_uType==INFO_CHK_BOLTOVERLAP)	//重叠螺栓检测
	{
		if(pPart&&pOtherPart&&pPart->GetClassTypeId()==CLS_BOLT&&pOtherPart->GetClassTypeId()==CLS_BOLT)
		{
			CLDSBolt *pBolt=(CLDSBolt*)pPart;
			if(DISTANCE(pPart->ucs.origin,pOtherPart->ucs.origin)<pBolt->get_d())
#ifdef AFX_TARG_ENU_ENGLISH
				m_sDescription.Format("0x%X and 0x%X bolts postion is too near，conflict occur！",pPart->handle,pOtherPart->handle);
#else
				m_sDescription.Format("0x%X与0x%X螺栓位置过近，产生冲突！",pPart->handle,pOtherPart->handle);
#endif
			else 
				state=true;
		}
		else 
		{
			state=true;
			return false;
		}
	}
	else if(m_uType==INFO_CHK_HOLEMISSING)	//漏孔检测
	{
		if(pPart&&pPart->GetClassTypeId()==CLS_BOLT)
		{
			CLDSBolt *pBolt=(CLDSBolt*)pPart;
			pBolt->feature=0;
			for(CLDSPart *pTempPart=Ta.Parts.GetFirst();pTempPart;pTempPart=Ta.Parts.GetNext())
			{
				CLsRefList *pLsRefList=pTempPart->GetLsRefList();
				if(pLsRefList==NULL)
					continue;
				if(pLsRefList->FromHandle(pBolt->handle))
					pBolt->feature++;
			}
			if(pBolt->feature==1&&!pBolt->IsFootNail())
#ifdef AFX_TARG_ENU_ENGLISH
				m_sDescription.Format("0x%X bolt connects only one part ，maybe lack of bolt hole！",pBolt->handle);
#else
				m_sDescription.Format("0x%X螺栓只连接了一个构件，可能缺少螺栓孔！",pBolt->handle);
#endif
			else
				state=true;
		}
		else if(pPart==NULL)
			return false;
	}
	else if(m_uType==INFO_CHK_REDUNDANTBOLT)	//冗余螺栓检测
	{
		if(pPart&&pPart->GetClassTypeId()==CLS_BOLT)
		{
			CLDSBolt *pBolt=(CLDSBolt*)pPart;
			pBolt->feature=0;
			for(CLDSPart *pTempPart=Ta.Parts.GetFirst();pTempPart;pTempPart=Ta.Parts.GetNext())
			{
				CLsRefList *pLsRefList=pTempPart->GetLsRefList();
				if(pLsRefList==NULL)
					continue;
				if(pLsRefList->FromHandle(pBolt->handle))
					pBolt->feature++;
			}
			if(pBolt->feature==0)
#ifdef AFX_TARG_ENU_ENGLISH
				m_sDescription.Format("0x%X bolt does not connect any part, maybe redundant bolt！",pBolt->handle);
#else
				m_sDescription.Format("0x%X螺栓未连接任何构件，可能为多余螺栓！",pBolt->handle);
#endif
			else
				state=true;
		}
		else if(pPart==NULL)
			return false;	
	}
#endif
#ifdef __COMMON_PART_INC_
	else if(m_uType==INFO_CHK_NULLPARTNO)		//空编号构件
	{
		if(pPart&&strlen(pPart->GetPartNo())<=0)
#ifdef AFX_TARG_ENU_ENGLISH
			m_sDescription.Format("0x%X%s part's No. not input correctly, please check!",pPart->handle,pPart->GetPartTypeName());
#else
			m_sDescription.Format("0x%X%s的构件号未正确输入,请查实!",pPart->handle,pPart->GetPartTypeName());
#endif
		else 
			state=true;
	}	
	else if(m_uType==INFO_CHK_BOLT_SPEC||m_uType==INFO_CHK_BOLT_GRADE)		//空编号构件
		state=ReCalBoltGuigeAndGrade(this,m_uType==INFO_CHK_BOLT_GRADE);
	else if(m_uType==INFO_CHK_RODBOLTINFO||m_uType==INFO_CHK_PLATEPROFILEANDBOLT)
	{
		CLDSPartPtr pPartArr[2];
		pPartArr[0]=Ta.Parts.FromHandle(m_hRelaObj1);
		pPartArr[1]=Ta.Parts.FromHandle(m_hRelaObj2);
		DWORD styleFlag=CHKPARTNO_RODBOLTINFO;
		if(m_uType==INFO_CHK_PLATEPROFILEANDBOLT)
			styleFlag=CHK_PLATE_PROFILEANDBOLTINFO;
		if(pPartArr[0]&&pPartArr[1])
			state=pPartArr[0]->IsEqual(pPartArr[1],TRUE,tolerance,NULL,NULL,&styleFlag);
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("This check type not support now!");
#else
		logerr.Log("暂不支持此类型的重新校核");
#endif
		return false;
	}
#endif
	return state;
}
int COutputInfo::CorrectErrorCmdInfo(AUTOCORRECT_CMDINFO &cmdInfo)
{
	cmdInfo.safeType=-1;
	if(state)	//已解决任务不需要再修正
		return -1;
	/*if(m_uType==INFO_CHK_ANGLECUT)
	{
		cmdInfo.safeType=2;
		cmdInfo.nativeName.Copy("统一切角信息");
	}
	else */if(m_uType==INFO_EMEND_OVERLAPNODE)
	{
		cmdInfo.safeType=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("merge overlapped nodes");
#else
		cmdInfo.nativeName.Copy("合并重叠节点");
#endif
	}
	else if(m_uType==INFO_EMEND_OVERLAPROD)
	{
		cmdInfo.safeType=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("merge overlapped rods");
#else
		cmdInfo.nativeName.Copy("合并重叠杆件");
#endif
	}
	else if(m_uType==INFO_CHK_REDUNDANTBOLT)
	{
		cmdInfo.safeType=1;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("clear redundant bolts");
#else
		cmdInfo.nativeName.Copy("清除冗余螺栓");
#endif
	}
	else if(m_uType==INFO_CHK_BOLTOVERLAP)
	{
		cmdInfo.safeType=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("merge overlapped bolts");
#else
		cmdInfo.nativeName.Copy("合并重叠螺栓");
#endif
	}
	else if(m_uType==INFO_BOLT_NORM_ZERO||m_uType==INFO_PART_LOCATION_ABNORMAL)
	{
		cmdInfo.safeType=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("clear wrong parts");
#else
		cmdInfo.nativeName.Copy("清除错误构件");
#endif
	}
	else if(m_uType==INFO_INVALID_PART_UCS)
	{
		cmdInfo.safeType=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("clear wrong parts");
#else
		cmdInfo.nativeName.Copy("清除错误构件");
#endif
	}
	else if(m_uType==INFO_INVALID_PROFILE)
	{
		cmdInfo.safeType=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("clear wrong parts");
#else
		cmdInfo.nativeName.Copy("清除错误构件");
#endif
	}
	else if(m_uType==INFO_CHK_RODBOLTINFO||m_uType==INFO_CHK_PLATEPROFILEANDBOLT||m_uType==INFO_CHK_MIRPLATE)
	{
		cmdInfo.safeType=1;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("design again");
#else
		cmdInfo.nativeName.Copy("重新设计");
#endif
	}
	else if(m_uType==INFO_CHK_BOLT_SPEC||m_uType==INFO_CHK_BOLT_GRADE)
	{
		cmdInfo.safeType=1;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdInfo.nativeName.Copy("modify");
#else
		cmdInfo.nativeName.Copy("修正");
#endif
	}
	return cmdInfo.safeType;
}
//统一角钢切角信息
bool UnifyAngleCutInfo(COutputInfo *pOutputInfo)
{
	return false;
}
//合并重叠节点
//pNode:合并后需要保留的节点;pOthNode:合并后需要删除的节点
bool MergeOverlapNodes(CLDSNode *pReserveNode,CLDSNode *pMergeNode,COutputInfo *pOutputInfo,bool updateRelaRodSolid)
{
	CLDSNode *pNode=pReserveNode, *pOthNode=pMergeNode;	//此行代码纯粹为了提升可阅读性添加 wjh-2017.12.16
	if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pOthNode->dwPermission))
	{
		if(pOutputInfo)
#ifdef AFX_TARG_ENU_ENGLISH
			pOutputInfo->m_sDescription.Format("When network coordinates，no permission to modify node 0X%X authority，merge forbidden",pOthNode->handle);
		else
			logerr.Log("When network coordinates,no permission to modify node 0X%X authority,merge forbidden",pOthNode->handle);
#else
			pOutputInfo->m_sDescription.Format("网络协同时，对0X%X节点没有拥有修改权限，无法合并",pOthNode->handle);
		else
			logerr.Log("网络协同时，对0X%X节点没有拥有修改权限，无法合并",pOthNode->handle);
#endif
		return false;	//网络协同时，对此
	}
	if(pNode==NULL||pOthNode==NULL)
		return false;	//其中至少一个节点已被删除掉

	if(!theApp.m_bCooperativeWork||pOthNode->dwPermission==pNode->dwPermission)
	{	//可以合并
		if(pOutputInfo)
#ifdef AFX_TARG_ENU_ENGLISH
			pOutputInfo->m_sDescription.Format("merge node 0X%X and node &0X%X-->node 0X%X\n",pNode->handle,pOthNode->handle,pNode->handle);
		else
			logerr.Log("merge node 0X%X and node &0X%X-->node 0X%X\n",pNode->handle,pOthNode->handle,pNode->handle);
#else
			pOutputInfo->m_sDescription.Format("合并0X%X节点&0X%X节点-->节点0X%X\n",pNode->handle,pOthNode->handle,pNode->handle);
		else
		{
			CXhChar16 nodestr1,nodestr2;
			nodestr1.Printf("0X%X",pNode->handle);
			if(pNode->pointI>0)
				nodestr1.Append(CXhChar16("{%d}",pNode->pointI));
			nodestr2.Printf("0X%X",pOthNode->handle);
			if(pOthNode->pointI>0)
				nodestr2.Append(CXhChar16("{%d}",pOthNode->pointI));
			logerr.Log("合并%s节点&%s节点-->节点%s\n",(char*)nodestr1,(char*)nodestr2,(char*)nodestr1);
		}
#endif
		if(pOthNode->hFatherPart!=0&&pNode->hFatherPart==0)
		{	//保留节点如缺失父杆件时，应借用待删除节点的父杆件 wjh-2013.6.6
			pNode->hFatherPart=pOthNode->hFatherPart;
			pNode->SetModified();
		}
		else if(pOthNode->hFatherPart>0&&pNode->hFatherPart>0)
		{
			CLDSLinePart* pFatherRod=Ta.FromRodHandle(pNode->hFatherPart);
			if(pFatherRod==NULL)
			{
				pNode->hFatherPart=pOthNode->hFatherPart;
				pNode->SetModified();
			}
		}
		CLDSLinePart* pRod;
		LINEPARTSET relaRods;
		for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			if(pRod->pStart==pOthNode||pRod->pEnd==pOthNode)
				relaRods.append(pRod);
		}
		Ta.ReplaceReferenceHandle(CHandleReplacer(pOthNode->handle,pNode->handle),CLS_NODE);
		if(updateRelaRodSolid)
		{
			CAngleMoveOperObject moveangle;
			for(pRod=relaRods.GetFirst();pRod;pRod=relaRods.GetNext())
			{
				pRod->ClearFlag();
				pRod->CalPosition();
				pRod->Create3dSolidModel();
				if(pRod->is_visible)
					g_pSolidDraw->NewSolidPart(pRod->GetSolidPartObject());
			}
		}
		pNode->cfgword.AddSpecWord(pOthNode->cfgword);	//防止合并后出现配材号在某一呼高缺失情况
		console.DispNodeSet.DeleteNode(pOthNode->handle);
		console.DeleteNode(pOthNode->handle);
		g_pSolidDraw->DelEnt(pOthNode->handle);
		return true;
	}
	else //if(theApp.m_bCooperativeWork&&pOthNode->dwPermission!=pNode->dwPermission)
	{
		if(pOutputInfo)
#ifdef AFX_TARG_ENU_ENGLISH
			pOutputInfo->m_sDescription.Format("0X%X & 0X%X-->different authority，unable to merge\n",pNode->handle,pOthNode->handle);
		else
			logerr.Log("0X%X & 0X%X-->different authority，unable to merge\n",pNode->handle,pOthNode->handle);
#else
			pOutputInfo->m_sDescription.Format("0X%X & 0X%X-->权限不同，无法合并\n",pNode->handle,pOthNode->handle);
		else
			logerr.Log("0X%X & 0X%X-->权限不同，无法合并\n",pNode->handle,pOthNode->handle);
#endif
		return false;
	}
}
bool MergeOverlapNodes(CLDSNode *pReserveNode,CLDSNode *pMergeNode,COutputInfo *pOutputInfo=NULL)
{
	return MergeOverlapNodes(pReserveNode,pMergeNode,pOutputInfo,true);
}
bool MergeOverlapNodes(COutputInfo *pOutputInfo)
{
	CLDSNode *pNode=Ta.Node.FromHandle(pOutputInfo->m_hRelaObj1);
	CLDSNode *pMergeNode=Ta.Node.FromHandle(pOutputInfo->m_hRelaObj2);
	return MergeOverlapNodes(pNode,pMergeNode,pOutputInfo);
}
//合并重叠杆件
bool MergeOverlapRods(COutputInfo *pOutputInfo)
{
	CLDSLinePart *pPole=(CLDSLinePart*)Ta.Parts.FromHandle(pOutputInfo->m_hRelaObj1,CLS_LINEPART);
	CLDSLinePart *pOthPole=(CLDSLinePart*)Ta.Parts.FromHandle(pOutputInfo->m_hRelaObj2,CLS_LINEPART);
	if(pPole==NULL||pOthPole==NULL)
		return false;
	//当前构件应合并到基准构件
#ifdef AFX_TARG_ENU_ENGLISH
	pOutputInfo->m_sDescription.Format("0X%X rod and 0X%X rod overlap,merge into 0X%X！\n",pPole->handle,pOthPole->handle,pPole->handle);
#else
	pOutputInfo->m_sDescription.Format("0X%X杆件与0X%X杆件有重叠,合并为0X%X！\n",pPole->handle,pOthPole->handle,pPole->handle);
#endif
	Ta.ReplaceReferenceHandle(CHandleReplacer(pOthPole->handle,pPole->handle));
	console.DispPartSet.DeleteNode(pOthPole->handle);
	pPole->cfgword.AddSpecWord(pOthPole->cfgword);
	console.DeletePart(pOthPole->handle);
	return true;
}
#ifdef __COMMON_PART_INC_
//合并重叠螺栓
bool MergeOverlapBolts(COutputInfo *pOutputInfo)
{
	if(pOutputInfo->SolvedState())
		return true;
	long hFromBolt=max(pOutputInfo->m_hRelaObj1,pOutputInfo->m_hRelaObj2);
	long hDestBolt=min(pOutputInfo->m_hRelaObj1,pOutputInfo->m_hRelaObj2);
	CLDSBolt* pFromBolt=(CLDSBolt*)Ta.Parts.FromHandle(hFromBolt,CLS_BOLT);
	CLDSBolt* pDestBolt=(CLDSBolt*)Ta.Parts.FromHandle(hDestBolt,CLS_BOLT);
	if(pFromBolt==NULL||pDestBolt==NULL)
		return false;
	bool failed=false;
	for(CSuperSmartPtr<CLDSPart>pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
	{
		CLsRefList* pLsRefList=pPart->GetLsRefList();
		if(pLsRefList)
		{
			CLsRef *pLsRef=pLsRefList->FromHandle(hFromBolt);
			bool canMerge=true;
			if(theApp.m_bCooperativeWork&&pLsRef!=NULL)
			{
				if(pPart->IsLinePart())
				{
					if(pLsRef->start1_end2_mid0==0&&!theApp.IsHasModifyPerm(pPart.LinePartPointer()->dwPermission))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						pOutputInfo->m_sDescription.Format("not own the authority to modify 0X%X rod's basic information,unable to merge bolts!",pPart->handle);
#else
						pOutputInfo->m_sDescription.Format("不拥有对0X%X杆件基本信息的修改权限,无法合并螺栓!",pPart->handle);
#endif
						canMerge=false;
					}
					else if(pLsRef->start1_end2_mid0==1&&!theApp.IsHasModifyPerm(pPart.LinePartPointer()->dwStartPermission))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						pOutputInfo->m_sDescription.Format("not own the authority to modify 0X%X rod's starting end,unable to merge bolts!",pPart->handle);
#else
						pOutputInfo->m_sDescription.Format("不拥有对0X%X杆件始端的修改权限,无法合并螺栓!",pPart->handle);
#endif
						canMerge=false;
					}
					else if(pLsRef->start1_end2_mid0==2&&!theApp.IsHasModifyPerm(pPart.LinePartPointer()->dwEndPermission))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						pOutputInfo->m_sDescription.Format("not own the authority to modify 0X%X rod's ending,unable to merge bolts!",pPart->handle);
#else
						pOutputInfo->m_sDescription.Format("不拥有对0X%X杆件终端的修改权限,无法合并螺栓!",pPart->handle);
#endif
						canMerge=false;
					}
				}
				else if(!theApp.IsHasModifyPerm(pPart->dwPermission))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					pOutputInfo->m_sDescription.Format("not own the authority to modify 0X%X parts,unable to merge bolts!",pPart->handle);
#else
					pOutputInfo->m_sDescription.Format("不拥有对0X%X构件的修改权限,无法合并螺栓!",pPart->handle);
#endif
					canMerge=false;	//协同模式下不拥有对此构件的权限
				}
			}
			if(canMerge)
				pLsRefList->MergeBolts(pFromBolt,pDestBolt);
			else
				failed=true;
#ifdef AFX_TARG_ENU_ENGLISH
			pOutputInfo->m_sDescription.Format("bolt 0X%X & 0X%X -->bolt 0X%X!",hFromBolt,hDestBolt,hDestBolt);
#else
			pOutputInfo->m_sDescription.Format("螺栓0X%X & 0X%X -->螺栓0X%X!",hFromBolt,hDestBolt,hDestBolt);
#endif
		}
	}

	if(!failed)
	{
		console.DispPartSet.DeleteNode(pFromBolt->handle);
		g_pSolidDraw->DelEnt(pFromBolt->handle);
		console.DeletePart(pFromBolt->handle);
		return true;
	}
	else
		return false;
}
//清除冗余螺栓
bool RemoveRedundantBolt(COutputInfo *pOutputInfo)
{
	console.DispPartSet.DeleteNode(pOutputInfo->m_hRelaObj1);
	console.DeletePart(pOutputInfo->m_hRelaObj1);
	return true;
}
//清除错误构件
bool RemoveInvalidPart(COutputInfo *pOutputInfo)
{
	console.DispPartSet.DeleteNode(pOutputInfo->m_hRelaObj1);
	console.DeletePart(pOutputInfo->m_hRelaObj1);
	return true;
}
//重新设计,然后再检查
bool RedesignPartAndCheck(COutputInfo *pOutputInfo)
{
	CSuperSmartPtr<CLDSPart> pPartArr[2];
	pPartArr[0]=Ta.Parts.FromHandle(pOutputInfo->m_hRelaObj1);
	pPartArr[1]=Ta.Parts.FromHandle(pOutputInfo->m_hRelaObj2);
	for(int i=0;i<2;i++)
	{
		CSuperSmartPtr<CLDSPart> pPart1=pPartArr[i];
		if(pPart1.IsHasPtr()&&pPart1->GetClassTypeId()==CLS_PLATE)
		{
			pPart1.PlatePointer()->DesignPlate();
			for(CLsRef *pLsRef=pPart1.PlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPart1.PlatePointer()->GetNextLsRef())
			{
				pLsRef->GetLsPtr()->correct_worknorm();
				pLsRef->GetLsPtr()->correct_pos();
			}
		}
		else if(pPart1.IsHasPtr()&&pPart1->GetClassTypeId()==CLS_PARAMPLATE)
			pPart1.ParamPlatePointer()->DesignPlate();
		else if(pPart1.IsHasPtr()&&pPart1->GetClassTypeId()==CLS_LINEANGLE)
		{
			pPart1.LineAnglePointer()->cal_x_wing_norm();
			pPart1.LineAnglePointer()->cal_y_wing_norm();
			pPart1.LineAnglePointer()->CalPosition();
			for(CLsRef *pLsRef=pPart1.LineAnglePointer()->GetFirstLsRef();pLsRef;pLsRef=pPart1.LineAnglePointer()->GetNextLsRef())
			{
				pLsRef->GetLsPtr()->correct_worknorm();
				pLsRef->GetLsPtr()->correct_pos();
			}
		}
	}
	DWORD styleFlag=pOutputInfo->ChkStyleFlag();
	return pPartArr[0]->IsEqual(pPartArr[1],TRUE,pOutputInfo->tolerance,NULL,NULL,&styleFlag);
}
#endif
DWORD COutputInfo::ChkStyleFlag()
{
	if(m_uType==INFO_CHK_MIRROD)
		return CHK_MIRROD_BASICINFO;
	else if(m_uType==INFO_CHK_MIRROD_LENGTH)
		return CHK_MIRROD_LENGTHINFO;
	else
		return 0;
}
bool COutputInfo::CorrectError()
{
	/*if(m_uType==INFO_CHK_ANGLECUT)
		state=UnifyAngleCutInfo(this);	//统一角钢切角信息
	else */if(m_uType==INFO_EMEND_OVERLAPNODE)
		state=MergeOverlapNodes(this);	//合并重叠节点
	else if(m_uType==INFO_EMEND_OVERLAPROD)
		state=MergeOverlapRods(this);	//合并重叠杆件
#ifdef __COMMON_PART_INC_
	else if(m_uType==INFO_CHK_REDUNDANTBOLT)
		state=RemoveRedundantBolt(this);//清除冗余螺栓
	else if(m_uType==INFO_CHK_BOLTOVERLAP)
		state=MergeOverlapBolts(this);	//合并重叠螺栓
	else if(m_uType==INFO_BOLT_NORM_ZERO||m_uType==INFO_PART_LOCATION_ABNORMAL)
		state=RemoveInvalidPart(this);	//清除错误构件
	else if(m_uType==INFO_INVALID_PART_UCS)
		state=RemoveInvalidPart(this);	//清除错误构件
	else if(m_uType==INFO_INVALID_PROFILE)
		state=RemoveInvalidPart(this);	//清除错误构件
	else if(m_uType==INFO_CHK_RODBOLTINFO||m_uType==INFO_CHK_PLATEPROFILEANDBOLT||m_uType==INFO_CHK_MIRPLATE)
		state=RedesignPartAndCheck(this);//重新设计,然后再检查
	else if(m_uType==INFO_CHK_BOLT_SPEC||m_uType==INFO_CHK_BOLT_GRADE)
		state=ReCalBoltGuigeAndGrade(this,m_uType==INFO_CHK_BOLT_GRADE);
#endif
	return state;
}

//////////////////////////////////////////////////////////////////////////
//CCheckPartNoInfo
//////////////////////////////////////////////////////////////////////////
CCheckPartNoInfo::CCheckPartNoInfo()
{
	m_idOutputType=2;
	m_uType=INFO_CHK_MIRROD;	//类型
	m_hRelaObj1=0;				//关联对象id1
	m_hRelaObj2=0;				//关联对象id2
	m_sDescription="";			//描述信息
	m_sPartNo="";	
}
CCheckPartNoInfo::~CCheckPartNoInfo()
{

}
CCheckPartNoInfo::CCheckPartNoInfo(CString sPartNo,long hObj1,long hObj2,CString sDescription)
{
	m_idOutputType=2;
	m_uType=INFO_CHK_MIRROD;	//类型
	//
	m_sPartNo=sPartNo;			//件号
	m_hRelaObj1=hObj1;		//关联对象id1
	m_hRelaObj2=hObj2;		//关联对象id2
	m_sDescription=sDescription;//描述信息
}

//重新检查当前件号构件
bool CCheckPartNoInfo::Recheck()
{
#ifdef __COMMON_PART_INC_
	CLDSPart *pPart1=Ta.Parts.FromHandle(m_hRelaObj1);
	CLDSPart *pPart2=Ta.Parts.FromHandle(m_hRelaObj2);
	if(pPart1==NULL&&pPart2==NULL)
		return false;
	DWORD dwStyleFlag=ChkStyleFlag();
	state = pPart1->IsEqual(pPart2,TRUE,tolerance,NULL,NULL,&dwStyleFlag);
#endif
	/*ATOM_LIST<CCheckPartNoInfo> itemList;
	PartNoCheck(itemList,"*",tolerance,pPart1->sPartNo);*/
	return state;
}

void AppendCheckLabelOutputInfo(CTaskOutputList* pItemList,CCheckPartNoInfo& checklabel)
{
	CCheckPartNoInfo* pCheckLabel=(CCheckPartNoInfo*)pItemList->Add(0,2);
	*pCheckLabel=checklabel;
}
void CCheckPartNoInfo::PartNoCheck(CTaskOutputList &itemList,CString sSegStr,
								   double fScope,char *sPartNo/*=NULL*/)
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	int nSumProgress=0;
	//分解段号字符串
	CHashList<SEGI>segTbl;
	GetSegNoHashTblBySegStr(sSegStr,segTbl);
	//1.遍历塔构构件，将每个件号对应的构件存入哈希表,件号为NULL的的构件均存于key为"NULL"的链表中
	typedef ARRAY_LIST<CLDSPart*> PART_PTR_ARR;
	typedef ARRAY_LIST<CLDSPolyPart*>POLYPART_PTR_ARR;
	CHashStrList<PART_PTR_ARR> partsHasListPartNo;
	CSuperSmartPtr<CLDSPart> pPart;
	for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
	{
		if(sPartNo==NULL)
		{
			if(pPart->GetClassTypeId()==CLS_BOLT||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE||
				pPart->GetClassTypeId()==CLS_POLYJOINT||pPart->m_bVirtualPart)
				continue;	//螺栓无编号不参预构件号检查
			else if(pPart->IsLinePart()&&(pPart.LinePartPointer()->bHuoQuStart||pPart.LinePartPointer()->bHuoQuEnd))
				continue;	//
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPart->dwPermission))
				continue;	//网络协同时，对此角钢没有拥有修改权限
			if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPart->iSeg.iSeg)==NULL)
				continue;	//检查是否在指定段号范围内
		}
		else if(stricmp(sPartNo,pPart->GetPartNo())!=0)
			continue;		//检查指定编号构件
		CString sKey="NULL";//件号为空的构件用NULL作为键值
		if(strlen(pPart->GetPartNo())>0)
			sKey=pPart->GetPartNo();
		PART_PTR_ARR *pPartArr=partsHasListPartNo.GetValue(sKey.GetBuffer(0));
		if(pPartArr==NULL)
			pPartArr=partsHasListPartNo.Add(sKey.GetBuffer(0));
		pPartArr->append(pPart);
		nSumProgress++;
	}
	//2.重新统计制弯构件并遍历,生成同上哈希表
	POLYPARTLIST polylist;
	Ta.RetrievePolyPartList(polylist);
	CHashStrList<POLYPART_PTR_ARR> polyPartsHasListPartNo;
	CLDSPolyPart *pPolyPart=NULL;
	for(pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
	{
		if(sPartNo==NULL)
		{
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPolyPart->dwPermission))
				continue;	//网络协同时，对此构件没有拥有修改权限
			if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPolyPart->iSeg.iSeg)==NULL)
				continue;	//检查是否在指点段号范围内
			CLDSLinePart *pFirstPole=pPolyPart->segset.GetFirst();
			if(pFirstPole->GetClassTypeId()!=CLS_LINEANGLE
				&&pFirstPole->GetClassTypeId()!=CLS_LINETUBE
				&&pFirstPole->GetClassTypeId()!=CLS_LINESLOT
				&&pFirstPole->GetClassTypeId()!=CLS_LINEFLAT)
				continue;
		}
		else if(stricmp(sPartNo,pPolyPart->GetPartNo())!=0)
			continue;		//检查指定编号构件
		CString sKey="NULL";//件号为空的构件用NULL作为键值
		if(strlen(pPolyPart->GetPartNo())>0)
			sKey=pPolyPart->GetPartNo();
		POLYPART_PTR_ARR *pPolyPartArr=polyPartsHasListPartNo.GetValue(sKey.GetBuffer(0));
		if(pPolyPartArr==NULL)
			pPolyPartArr=polyPartsHasListPartNo.Add(sKey.GetBuffer(0));
		pPolyPartArr->append(pPolyPart);
		nSumProgress++;
	}
	try{
		int i=0;
		CString sError;
		//3.对普通构件进行件号检查
		for(PART_PTR_ARR *pPartArr=partsHasListPartNo.GetFirst();pPartArr;pPartArr=partsHasListPartNo.GetNext())
		{
			if(stricmp(partsHasListPartNo.GetCursorKey(),"NULL")==0)
			{	//件号为空的构件
				for(CLDSPart **ppPart=pPartArr->GetFirst();ppPart;ppPart=pPartArr->GetNext())
				{
					CString sPartName=(*ppPart)->GetPartTypeName();
#ifdef AFX_TARG_ENU_ENGLISH
					sError.Format("%s0x%X part No. not input correctly,please check!",sPartName,(*ppPart)->handle);
#else
					sError.Format("%s0x%X的构件号未正确输入,请查实!",sPartName,(*ppPart)->handle);
#endif
					AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)(*ppPart)->GetPartNo(),(*ppPart)->handle,0,sError));
					//显示进度条
					i++;
					DisplayProcess((i*100)/nSumProgress);
				}
			}
			else if(pPartArr->GetSize()>1)
			{	//件号非空且同件号构件数大于1时进行比对
				CLDSPart **ppFirstPart=pPartArr->GetFirst();
				CString sFirstPartName=(*ppFirstPart)->GetPartTypeName();
				long hFirstPart=(*ppFirstPart)->handle;
				CString sPartNo=(*ppFirstPart)->GetPartNo();
				i++;
				for(CLDSPart **ppPart=pPartArr->GetFirst();ppPart;ppPart=pPartArr->GetNext())
				{
					char error_info[200]="";
					long hPart=(*ppPart)->handle;
					if((*ppFirstPart)->GetClassTypeId()!=(*ppPart)->GetClassTypeId())
					{
						CString sPartName=(*ppPart)->GetPartTypeName();
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("%s0x%X and %s0x%X are not of the same type parts，but coded with the same Number！",
#else
						sError.Format("%s0x%X与%s0x%X不是同一类型构件，但编了相同的构件号！",
#endif
							sFirstPartName,hFirstPart,sPartName,hPart);
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo(sPartNo,hFirstPart,hPart,sError));
					}
					else if(!(*ppFirstPart)->IsEqual(*ppPart,TRUE,fScope,error_info))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("%s0X%X and 0X%X(%s)are coded with the same Number",sFirstPartName,hFirstPart,hPart,error_info);
#else
						sError.Format("%s0X%X与0X%X(%s)但编了相同的构件号",sFirstPartName,hFirstPart,hPart,error_info);
#endif
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo(sPartNo,hFirstPart,hPart,sError));
					}
					//显示进度条
					i++;
					DisplayProcess((i*100)/nSumProgress);
				}
			}
		}
		//4.对制弯构件进行件号检查
		POLYPART_PTR_ARR *pPolyPartArr=NULL;
		for(pPolyPartArr=polyPartsHasListPartNo.GetFirst();pPolyPartArr;pPolyPartArr=polyPartsHasListPartNo.GetNext())
		{
			if(stricmp(polyPartsHasListPartNo.GetCursorKey(),"NULL")==0)
			{	//件号为空的构件
				for(CLDSPolyPart **ppPart=pPolyPartArr->GetFirst();ppPart;ppPart=pPolyPartArr->GetNext())
				{
					CString sPartName=(*ppPart)->GetPartTypeName();
#ifdef AFX_TARG_ENU_ENGLISH
					sError.Format("%s0x%X Part No not input correctly,please check!",sPartName,(*ppPart)->handle);
#else
					sError.Format("%s0x%X的构件号未正确输入,请查实!",sPartName,(*ppPart)->handle);
#endif
					AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo((char*)(*ppPart)->GetPartNo(),(*ppPart)->handle,0,sError));
					//显示进度条
					i++;
					DisplayProcess((i*100)/nSumProgress);
				}
			}
			else if(pPolyPartArr->GetSize()>1)
			{	//件号非空且同件号构件数大于1时进行比对
				CLDSPolyPart **ppFirstPart=pPolyPartArr->GetFirst();
				CString sFirstPartName=(*ppFirstPart)->GetPartTypeName();
				long hFirstPart=(*ppFirstPart)->handle;
				CString sPartNo=(*ppFirstPart)->GetPartNo();
				i++;
				if(PART_PTR_ARR *pPartArr=partsHasListPartNo.GetValue(sPartNo.GetBuffer(0)))
				{	//检查火曲构件编号是否与其他构件编号冲突
					CLDSPart **ppTempPart=pPartArr->GetFirst();
					if(ppTempPart&&(*ppTempPart)->GetClassTypeId()!=(*ppFirstPart)->GetClassTypeId())
					{
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("0X%X Part and 0X%X part{%s}are not of the same type，but coded with the same Number！",(*ppTempPart)->handle,hFirstPart,sPartNo);
#else
						sError.Format("0X%X构件与0X%X构件{%s}不是同一类型构件，但编了相同的构件号！",(*ppTempPart)->handle,hFirstPart,sPartNo);
#endif
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo(sPartNo,hFirstPart,(*ppTempPart)->handle,sError));
					}
				}
				for(CLDSPolyPart **ppPart=pPolyPartArr->GetFirst();ppPart;ppPart=pPolyPartArr->GetNext())
				{
					char error_info[200]="";
					long hPart=(*ppPart)->handle;
					if((*ppFirstPart)->GetClassTypeId()!=(*ppPart)->GetClassTypeId())
					{
						CString sPartName=(*ppPart)->GetPartTypeName();
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("0X%X Part and 0X%X part{%s} are not of the same type，but coded with the same Number！",
#else
						sError.Format("%s0x%X与%s0x%X不是同一类型构件，但编了相同的构件号！",
#endif
							sFirstPartName,hFirstPart,sPartName,hPart);
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo(sPartNo,hFirstPart,hPart,sError));
					}
					else if(!(*ppFirstPart)->IsEqual(*ppPart,fScope,error_info))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("%s0X%X and 0X%X(%s) are coded with the same Number",sFirstPartName,hFirstPart,hPart,error_info);
#else
						sError.Format("%s0X%X与0X%X(%s)但编了相同的构件号",sFirstPartName,hFirstPart,hPart,error_info);
#endif
						AppendCheckLabelOutputInfo(&itemList,CCheckPartNoInfo(sPartNo,hFirstPart,hPart,sError));
					}
					//显示进度条
					i++;
					DisplayProcess((i*100)/nSumProgress);
				}
			}
		}
		DisplayProcess(100);
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
#endif
}
//////////////////////////////////////////////////////////////////////////
//CCheckGasketErrorInfo
//////////////////////////////////////////////////////////////////////////
#include "VertexsLifeObject.h"
CCheckGasketErrorInfo::CCheckGasketErrorInfo()
{
	m_idOutputType=3;
	m_uType=INFO_EMEND_BOLTGASKET;	//类型
	m_hRelaObj1=0;				//关联对象id1
	m_hRelaObj2=0;				//关联对象id2
	m_sDescription="";			//描述信息
	siGasketThick=0;
	memset((long*)relaObjIds.pPresetObjs,0,sizeof(long)*relaObjIds.CountOfPreset);
}
CCheckGasketErrorInfo::~CCheckGasketErrorInfo()
{

}
long CCheckGasketErrorInfo::get_hRelaBolt(){
	return m_hRelaObj1;
}
long CCheckGasketErrorInfo::set_hRelaBolt(long hBolt){return m_hRelaObj1=hBolt;}
long CCheckGasketErrorInfo::get_hRelaGasket(){
	return m_hRelaObj2;
}
long CCheckGasketErrorInfo::set_hRelaGasket(long hGasket){return m_hRelaObj2=hGasket;}
CLDSLineAngle *GetBoltGasketCutAngle(CLDSBolt* pBolt);	//defined in EmendBoltGasket.cpp
bool CCheckGasketErrorInfo::CorrectError()
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if(m_uType==ERROR_ISOLATED_BOLT)
	{	//冗余孤立螺栓
		return state=console.DeletePart(this->hRelaBolt)!=FALSE;
	}
	else if(m_uType==ERROR_VIRTUAL_HOLE_LIKELY||m_uType==ERROR_VIRTUAL_WIREHOLE_LIKELY)
	{	//目前有一个关联构件的非脚钉疑似虚拟孔
		CLDSBolt* pBolt=(CLDSBolt*)console.FromPartHandle(this->hRelaBolt);
		if(pBolt)
		{
			state=true;
			pBolt->m_bVirtualPart=true;
			g_pSolidDraw->DelEnt(pBolt->handle);
			pBolt->SetModified(TRUE,FALSE);
			CLDSPart* pRelaPart=console.FromPartHandle(this->m_hRelaObj2);
			if(pRelaPart)
			{	//重新生成虚拟孔的实体显示
				pRelaPart->SetSolidModified(TRUE);
				pRelaPart->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pRelaPart->GetSolidPartObject());
				g_pSolidDraw->Draw();
			}
			return true;
		}
		return false;
	}
	else if(m_uType==ERROR_MISSING_CROSSGASKET)
	{	//交叉点处垫板缺失，处理方式为：添加垫板或垫圈
		CLDSBolt* pBolt=(CLDSBolt*)console.FromPartHandle(this->hRelaBolt);
		int type=CLDSBolt::CalPadPara(pBolt->get_d(),abs(siGasketThick));
		if(type==DIANQUAN_STRU::DIANBAN)
		{
			pBolt->DianQuan.N=0;
			CLDSPlate* pGasketPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
			pGasketPlate->wiType=CLDSPlate::WASHER_PLATE;
			//添加垫板定位信息
			pGasketPlate->designInfo.m_hBasePart=anchor.hAnchorPart;
			//添加垫板包括的螺栓集合
			ARRAY_LIST<CLDSBolt*> bolts(0,relaObjIds.Count);
			for(UINT i=0;i<this->relaObjIds.Count;i++)
			{
				CLDSBolt* pRelaBolt=(CLDSBolt*)console.FromPartHandle(relaObjIds.At(i),CLS_BOLT);
				if(pRelaBolt)
				{
					pGasketPlate->AppendLsRef(pRelaBolt->GetLsRef());
					bolts.append(pRelaBolt);
				}
			}
			if(bolts.GetSize()>0)
			{
				CLDSBolt::CorrectBoltZnL0Para(bolts.m_pData,bolts.GetSize());
				for(int j=0;j<bolts.GetSize();j++)
				{
					bolts[j]->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(bolts[j]->GetSolidPartObject());
				}
			}
			if(pGasketPlate->DesignPlate())
			{
				pGasketPlate->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pGasketPlate->GetSolidPartObject());
			}
		}
		else if(type==DIANQUAN_STRU::DIANQUAN)
		{	//添加垫圈
			pBolt->DianQuan.thick=CLDSPart::library->GetBestMatchWasherThick(pBolt->get_d(),siGasketThick);
			pBolt->DianQuan.N=abs(siGasketThick)/pBolt->DianQuan.thick;
			pBolt->DianQuan.offset=this->anchor.washerOffset;
		}
	}
	else if(m_uType==ERROR_MISSING_ENDGASKET)
	{	//端连接处垫板缺失，处理方式为：添加垫板或垫圈
		CLDSBolt* pBolt=(CLDSBolt*)console.FromPartHandle(this->hRelaBolt);
		int type=CLDSBolt::CalPadPara(pBolt->get_d(),abs(siGasketThick),&pBolt->DianQuan);
		if(type==DIANQUAN_STRU::DIANBAN)
		{
			pBolt->DianQuan.N=0;
			CLDSPart* pDatumPart=Ta.Parts.FromHandle(anchor.hAnchorPart);
			CLDSLineAngle* pCutAngle=GetBoltGasketCutAngle(pBolt);
			//添加垫板包括的螺栓集合
			CLDSBolt* pRelaBolt;
			BOLTSET boltset;
			for(UINT i=0;i<this->relaObjIds.Count;i++)
			{
				pRelaBolt=(CLDSBolt*)console.FromPartHandle(relaObjIds.At(i),CLS_BOLT);
				if(pRelaBolt)
					boltset.append(pRelaBolt);
			}
			CLDSPlate* pGasketPlate=pView->DesignBoltPad(pDatumPart,pCutAngle,boltset,siGasketThick,anchor.gasketOffset);
			if(pGasketPlate)
			{
				this->state=true;
				for(pRelaBolt=boltset.GetFirst();pRelaBolt;pRelaBolt=boltset.GetNext())
				{
					pRelaBolt->CorrectBoltZnL0Para();
					pRelaBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pRelaBolt->GetSolidPartObject());
				}
				//添加垫板定位信息
				/*pGasketPlate->designInfo.m_hBasePart=anchor.hAnchorPart;
				pGasketPlate->Thick=this->siGasketThick;
				if(pGasketPlate->DesignPlate())*/
				pGasketPlate->wiType=CLDSPlate::WASHER_PLATE;
				pGasketPlate->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pGasketPlate->GetSolidPartObject());
			}
		}
		else if(type==DIANQUAN_STRU::DIANQUAN)
		{	//添加垫圈
			pBolt->CalPadPara(this->siGasketThick);
			pBolt->DianQuan.offset=this->anchor.washerOffset;
			state=true;
		}
		if(state)
		{
			CLDSBolt::CorrectBoltZnL0Para(&pBolt);
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
	}
	else if(m_uType==ERROR_GASKET_THICK)
	{	//垫板或垫圈厚度错误，处理方式为：重新修订垫板或垫圈厚度
		CLDSBolt* pBolt=(CLDSBolt*)console.FromPartHandle(this->hRelaBolt);
		CLDSPlate* pGasket=hRelaGasket>0?(CLDSPlate*)console.FromPartHandle(hRelaGasket,CLS_PLATE):NULL;
		bool oldstate=state;
		state=true;
		if(pGasket)
			pGasket->Thick=abs(siGasketThick);
		else if(pBolt)
			pBolt->CalPadPara(siGasketThick);
		else
			state=oldstate;
		if(state)
		{
			CLDSBolt::CorrectBoltZnL0Para(&pBolt);
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
	}
	else if(m_uType==ERROR_PART_POS)
	{	//构件位置错误
		SmartPartPtr pErrPart=console.FromPartHandle(this->relaObjIds[0]);
		if(pErrPart.IsNULL())
			return false;
		if(pErrPart->GetClassTypeId()==CLS_PLATE)
			pErrPart.pPlate->DesignPlate();
		else if(pErrPart->GetClassTypeId()==CLS_PARAMPLATE)
			pErrPart.pParamPlate->DesignPlate();
		else if(pErrPart->IsLinePart())
			pErrPart.pRod->CalPosition();
		else
			return false;
		return true;
	}
	else if(m_uType==ERROR_BOLTPAD_CUTANGLE)
	{	//垫板缺少切角处理
		CLDSPlate* pGasketPlate=(CLDSPlate*)console.FromPartHandle(this->hRelaGasket,CLS_PLATE);
		CLDSLineAngle* pCutAngle=(CLDSLineAngle*)console.FromPartHandle(this->relaObjIds[0],CLS_LINEANGLE);
		CVertexsLifeObject manageVertexs(pGasketPlate->vertex_list);
		if(pView&&pView->CutBoltPadByAngle(pGasketPlate,pCutAngle))	//垫板进行切角处理
		{
			return true;
		}
		else	//未发生外形改变
		{
			manageVertexs.DisableBackup();
			return false;
		}
	}
	else if(m_uType==ERROR_GASKET_PROFILE)
	{	//垫板外形不合适，处理方式为：修正垫板外形
		PLATESET gaskets;
		CLDSPlate* pPlate;
		for(UINT i=0;i<this->relaObjIds.Count;i++)
		{
			if((pPlate=(CLDSPlate*)console.FromPartHandle(relaObjIds[i],CLS_PLATE))!=NULL)
				gaskets.append(pPlate);
		}
		CHashList<CBoltPadProperty> hashPlateProps;
		ATOM_LIST<SAMESCHEMA_BOLTPAD> sameSchemaBoltPadList;
		for(pPlate=gaskets.GetFirst();pPlate;pPlate=gaskets.GetNext())
		{
			CBoltPadProperty *pPadProp = hashPlateProps.Add(pPlate->handle);
			pPadProp->InitProperty(pPlate);
		}
		CBoltPadProperty datumprop;
		//1 获取最小外形顶点坐标
		CBoltPadProperty *pBoltPadProp=NULL,*pDatumProperty=&datumprop;//pSimBoltPad->pDatumProperty;
		CXhChar16 sPartNo;
		for(pPlate=gaskets.GetFirst();pPlate;pPlate=gaskets.GetNext())
		{
			if(pPlate->handle==pDatumProperty->m_pBoltPad->handle)
				continue;
			pBoltPadProp=hashPlateProps.GetValue(pPlate->handle);
			pBoltPadProp->UpdateMinProfileVertex(pDatumProperty);
			if(sPartNo.GetLength()<=0&&strlen(pPlate->Label)>0)
				sPartNo.Copy(pPlate->Label);
		}
		//2 调整外形顶点坐标
		for(pPlate=gaskets.GetFirst();pPlate;pPlate=gaskets.GetNext())
		{
			pBoltPadProp=hashPlateProps.GetValue(pPlate->handle);
			//if(pBoltPadProp==NULL)
			//	continue;
			if(sPartNo.GetLength()>0)
				pPlate->SetPartNo(sPartNo);
			BOOL bRetCode = pBoltPadProp->EmendProfileVertexs(pDatumProperty);
		}
		return true;
	}
	else if(m_uType==ERROR_REDUNDANT_GASKET)
	{	//冗余垫板，处理方式为：清除垫板
		return console.DeletePart(hRelaGasket)==TRUE;
	}
	return state;
}
// 返回值>=0表示支持错误修正功能
int CCheckGasketErrorInfo::CorrectErrorCmdInfo(AUTOCORRECT_CMDINFO &cmdInfo)
{
	cmdInfo.safeType=-1;
	if(state)	//已解决任务不需要再修正
		return -1;
	if(m_uType==ERROR_ISOLATED_BOLT)
	{	//冗余孤立螺栓
		cmdInfo.safeType=1;
		cmdInfo.nativeName.Copy("清除");
	}
	else if(m_uType==ERROR_VIRTUAL_HOLE_LIKELY||m_uType==ERROR_VIRTUAL_WIREHOLE_LIKELY)
	{	//目前有一个关联构件的非脚钉疑似虚拟孔
		cmdInfo.safeType=1;
		cmdInfo.nativeName.Copy("设为仅有孔");
	}
	else if(m_uType==ERROR_MISSING_CROSSGASKET)
	{	//交叉点处垫板缺失，处理方式为：添加垫板或垫圈
		cmdInfo.safeType=0;	//交叉点处垫板厚度由于受组合角钢影响可能需要人为调整
		cmdInfo.nativeName.Copy("添加垫板或垫圈");
	}
	else if(m_uType==ERROR_MISSING_ENDGASKET)
	{	//端连接处垫板缺失，处理方式为：添加垫板或垫圈
		cmdInfo.safeType=0;
		cmdInfo.nativeName.Copy("添加垫板或垫圈");
	}
	else if(m_uType==ERROR_GASKET_THICK)
	{	//垫板或垫圈厚度错误，处理方式为：重新修订垫板或垫圈厚度
		cmdInfo.safeType=0;
		cmdInfo.nativeName.Copy("更正垫板(圈)厚度");
	}
	else if(m_uType==ERROR_PART_POS)
	{	//构件位置错误
		cmdInfo.safeType=0;
		return -1;
	}
	else if(m_uType==ERROR_BOLTPAD_CUTANGLE)
	{	//垫板缺少切角处理
		cmdInfo.safeType=0;
		cmdInfo.nativeName.Copy("垫板切角");
	}
	else if(m_uType==ERROR_GASKET_PROFILE)
	{	//垫板外形不合适，处理方式为：修正垫板外形
		cmdInfo.safeType=0;
		cmdInfo.nativeName.Copy("标准化分组");
	}
	else if(m_uType==ERROR_REDUNDANT_GASKET)
	{	//冗余垫板，处理方式为：清除垫板
		cmdInfo.safeType=0;
		cmdInfo.nativeName.Copy("清除");
	}
	else
		return -1;
	PRESET_ARRAY4<int> a;
	return cmdInfo.safeType;
}
bool CCheckGasketErrorInfo::Recheck()
{
	return false;
}
bool CCheckGasketErrorInfo::CopyFrom(IBasicOutput* pOutput)
{
	COutputInfo::CopyFrom(pOutput);
	CCheckGasketErrorInfo* pCopyFrom=(CCheckGasketErrorInfo*)pOutput;
	siGasketThick=pCopyFrom->siGasketThick;//垫板垫圈厚度(负值表示垫板自基点沿螺栓工作朝向向下）
	anchor=pCopyFrom->anchor;
	if(relaObjIds.Count!=pCopyFrom->relaObjIds.Count)
	{
		relaObjIds.Clear();
		for(UINT i=0;i<pCopyFrom->relaObjIds.Count;i++)
			relaObjIds.Append(pCopyFrom->relaObjIds.At(i)); 
	}
	else
	{
		for(UINT i=0;i<pCopyFrom->relaObjIds.Count;i++)
			relaObjIds.Set(i,pCopyFrom->relaObjIds.At(i)); 
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
CHashStrList<COutputInfo> markInfoList;