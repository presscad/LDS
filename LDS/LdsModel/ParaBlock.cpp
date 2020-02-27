//<LOCALE_TRANSLATE BY hxr /> 2015-5-12
#include "stdafx.h"
#include "Tower.h"
#include "block.h"
#include "SolidBodyBuffer.h"
#include "Expression.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// KEY4C Construction/Destruction
//////////////////////////////////////////////////////////////////////
/* 该类源码已移至lds_part.cpp中
static char pack_str[5];
KEY4C::KEY4C(char* keyStr){
	dwItem=0;
	if(keyStr==NULL)
		return;
	else //if(keyStr!=NULL)
	{
		for(size_t i=0;i<strlen(keyStr)&&i<4;i++)
			label[i]=keyStr[i];
	}
}
char* KEY4C::Label()//返回指针是临时字符串指针
{
	strncpy(pack_str,label,4);
	pack_str[4]=0;
	return pack_str;
}
*/
//////////////////////////////////////////////////////////////////////
// CParaModelNode Construction/Destruction
//////////////////////////////////////////////////////////////////////
CParaModelNode::CParaModelNode()
{
	pTagInfo=NULL;
	memset(sPreferLayer,0,3);
	strcpy(posx_expression,"0");
	strcpy(posy_expression,"0");
	strcpy(posz_expression,"0");
	strcpy(scale_expression,"");
	strcpy(exist_logicexpr,"");
	keyRepeatParam.dwItem=0;	//>0时表示该节点为节间中的重复节点，重复参数为"i"；其值为重复节点所归属的节点重复轴线的标识
	/*
		0-坐标无任何依赖的节点;
		1-杆件上X坐标值不变点;
		2-杆件上Y坐标值不变点;
		3-杆件上Z坐标值不变点;
		4-两杆件交叉交点;
		5-杆件上的比例等分点;
	*/
	attach_type=0;	
	memset(attachNodeArr,0,sizeof(short)*4);
	ciOffsetType=0;	//默认偏移点偏移量为延长度方向
	attach_scale=0;	//正值表示小数形式的比例位置值,负值表示分子.分母
	wiOffsetByPrevNode=0xffff;
}
CParaModelNode::~CParaModelNode()
{
	if(pTagInfo)
		pTagInfo->DetachObject();
}
//////////////////////////////////////////////////////////////////////
// CParaModelNode Construction/Destruction
//////////////////////////////////////////////////////////////////////
CParaModelPart::CParaModelPart()
{
	//memset(sPreferLayer,0,3);
	memset(this,0,sizeof(CParaModelPart));
	m_idPartClsType=CLS_LINEANGLE;
	strcpy(exist_logicexpr,"1=0");
}
CParaModelRod::CParaModelRod()
{
	pTagInfo=NULL;
	start_joint_type=end_joint_type=JOINT_AUTOJUSTIFY;
	start_force_type=end_force_type=0;
	nIterStep = 1;
	layout_style=0;
	callen_type=0;
	m_iElemType=0;	//表示自动判断有限单元的承力类型
	m_bMajorMat=FALSE;
	eyeSideNorm.Set(0,0,0);
}
CParaModelRod::~CParaModelRod()
{
	if(pTagInfo)
		pTagInfo->DetachObject();
}
//////////////////////////////////////////////////////////////////////
// CParaModelAngle Construction/Destruction
//////////////////////////////////////////////////////////////////////
CParaModelAngle::CParaModelAngle()
{
	desStart.cTouchLine=0;			//0.X肢心线;1.Y肢心线;2.楞线;3.控制点
	desStart.cTouchLineType=0;		//0.心线;1.火曲线;2.偏移准线
	desStart.cFaceOffsetDatumLine=0;  //平推模式下的自身平推基准线
	desStart.face_offset_norm.Set(0,0,1);	//平推面法线
	desStart.coordCtrlType=0;	//0:X-Z主控;1:Y-Z主控;2.X-Y主控(仅在spatialOperationStyle=15时有效)
	desStart.cMainCoordCtrlWing='X';//角钢主控肢：'X'肢心线上关键点;'Y'肢心线上关键点;其余楞线上关键点
	desEnd=desStart;
}
//////////////////////////////////////////////////////////////////////////
//CParaModelParamPlate
CParaModelParamPlate::CParaModelParamPlate()
{
	pTagInfo=NULL;
	m_iParamType=1;
	strcpy(off_expression,"");
	feature=0;
}
CParaModelParamPlate::~CParaModelParamPlate()
{
	if(pTagInfo)
		pTagInfo->DetachObject();
}
//////////////////////////////////////////////////////////////////////
// DESIGN_PARAM_ITEM Construction/Destruction
//////////////////////////////////////////////////////////////////////
static EXPRESSION_VAR ConvertParamItemToExprVar(DESIGN_PARAM_ITEM *pItem)
{
	EXPRESSION_VAR var;
	if(pItem->dataType==DESIGN_PARAM_ITEM::TYPE_DOUBLE)
		var.fValue=pItem->value.fValue;
	else if(pItem->dataType==DESIGN_PARAM_ITEM::TYPE_INTEGER)
		var.fValue=pItem->value.iType;
	else if(pItem->dataType==DESIGN_PARAM_ITEM::TYPE_DOUBLE_ARR)
	{
		var.uiUpperBound=pItem->value.uiUpperBound;
		var.arrValues=pItem->value.arrValues;
	}
	strncpy(var.variableStr,pItem->key.Label(),8);
	return var;
}
//////////////////////////////////////////////////////////////////////
// CParaBlockModelTemplate Construction/Destruction
//////////////////////////////////////////////////////////////////////
void SolveNodePosExpression(CParaModelNode *pFaceNode,CExpression &expression,BOOL bIgnorePosExpression=FALSE)
{
	/*EXPRESSION_VAR *pVar;
	if(!bIgnorePosExpression&&strlen(pFaceNode->posx_expression)>0)
		pFaceNode->actual_pos.x=expression.SolveExpression(pFaceNode->posx_expression);
	pVar=expression.varList.append();
	pVar->fValue=pFaceNode->actual_pos.x;
	_snprintf(pVar->variableStr,8,"%s_X",pFaceNode->keyLabel.Label());
	if(!bIgnorePosExpression&&strlen(pFaceNode->posy_expression)>0)
		pFaceNode->actual_pos.y=expression.SolveExpression(pFaceNode->posy_expression);
	pVar=expression.varList.append();
	pVar->fValue=pFaceNode->actual_pos.y;
	_snprintf(pVar->variableStr,8,"%s_Y",pFaceNode->keyLabel.Label());
	if(!bIgnorePosExpression&&strlen(pFaceNode->posz_expression)>0)
		pFaceNode->actual_pos.z=expression.SolveExpression(pFaceNode->posz_expression);
	pVar=expression.varList.append();
	pVar->fValue=pFaceNode->actual_pos.z;
	_snprintf(pVar->variableStr,8,"%s_Z",pFaceNode->keyLabel.Label());*/
}
static void LoadParaModelNodeInfo(CParaModelNode *pItem,DWORD key){pItem->keyLabel=key;}
static void LoadParaModelRodInfo(CParaModelRod *pItem,DWORD key){
	pItem->keyLabel=key;
	pItem->m_idPartClsType=CLS_LINETUBE;
}
static void LoadRaraModelPlateInfo(CParaModelParamPlate *pItem,DWORD key){pItem->keyLabel=key;}
static CParaModelRod* CreateNewParaRod(int idClsType,DWORD key,void* pContext)	//必设回调函数
{
	CParaModelRod* pRod;
	switch(idClsType)
	{
	case CLS_LINEANGLE:
		pRod=new CParaModelAngle();
		break;
	case CLS_LINETUBE:
	default:
		pRod=new CParaModelRod();
		break;
	}
	pRod->keyLabel=key;
	return pRod;
}
static BOOL DeleteParaRod(CParaModelRod *pRod)				//必设回调函数
{
	if(pRod->IsAngleObject())
		delete (CParaModelAngle*)pRod;
	else
		delete pRod;
	return TRUE;
}
//long (*AllocateUniqueIdFunc)();						//可选择设定回调函数
CParaBlockModelTemplate::CParaBlockModelTemplate()
{
	m_iTempType=0;
	m_iTempIndex=1;
	listRepeatedParaNodes.LoadDefaultObjectInfo=listParaNodes.LoadDefaultObjectInfo=LoadParaModelNodeInfo;
	listParaRods.CreateNewAtom=listRepeatedParaRods.CreateNewAtom=CreateNewParaRod;
	listParaRods.DeleteAtom=listRepeatedParaRods.DeleteAtom = DeleteParaRod;
	listRepeatedParaRods.LoadDefaultObjectInfo=listParaRods.LoadDefaultObjectInfo=LoadParaModelRodInfo;
	listParaPlates.LoadDefaultObjectInfo=LoadRaraModelPlateInfo;
}
CParaBlockModelTemplate::~CParaBlockModelTemplate()
{
}
//从listParaNodes及listRepetedParaNodes中查找参数化节点模板
CParaModelNode* CParaBlockModelTemplate::GetParaNode(DWORD key)
{
	CParaModelNode* pParaNode=listParaNodes.GetValue(key);
	if(pParaNode==NULL)
		pParaNode=listRepeatedParaNodes.GetValue(key);
	return pParaNode;
}
//从listParaRods及listRepetedParaRods中查找参数化节点模板
CParaModelRod* CParaBlockModelTemplate::GetParaRod(DWORD key)
{
	CParaModelRod* pParaRod=listParaRods.GetValue(key);
	if(pParaRod==NULL)
		pParaRod=listRepeatedParaRods.GetValue(key);
	return pParaRod;
}
//
CParaModelParamPlate* CParaBlockModelTemplate::GetParaPlate(DWORD key)
{
	return listParaPlates.GetValue(key);
}
//当CParaModelNode已经有挂接对象时执行TagInfo(...), 否则创建新挂接数据对象后返回
TAG_NODE_INFO* CParaBlockModelTemplate::LoadTagInfo(CParaModelNode* pItem)
{
	if(pItem->TagInfo()!=NULL)
		return pItem->TagInfo();
	TAG_NODE_INFO* pTagInfo=listTagNodeInfo.append();
	pItem->SetTagInfo(pTagInfo);
	pTagInfo->AttachObject(pItem);
	return pTagInfo;
}
//当CParaModelNode已经有挂接对象时执行TagInfo(...), 否则创建新挂接数据对象后返回
TAG_ROD_INFO* CParaBlockModelTemplate::LoadTagInfo(CParaModelRod* pItem)
{
	if(pItem->TagInfo()!=NULL)
		return pItem->TagInfo();
	TAG_ROD_INFO* pTagInfo=listTagRodInfo.append();
	pItem->SetTagInfo(pTagInfo);
	pTagInfo->AttachObject(pItem);
	return pTagInfo;
}
//
TAG_PLATE_INFO* CParaBlockModelTemplate::LoadTagInfo(CParaModelParamPlate* pItem)
{
	if(pItem->TagInfo()!=NULL)
		return pItem->TagInfo();
	TAG_PLATE_INFO* pTagInfo=listTagPlateInfo.append();
	pItem->SetTagInfo(pTagInfo);
	pTagInfo->AttachObject(pItem);
	return pTagInfo;
}
void CParaBlockModelTemplate::FillMirInfo(CParaBlockModel* pModel)
{
	RELATIVE_OBJECT relaObj;
	//匹配节点对称信息
	CLDSNode* pNode=NULL,*pCheckNode=NULL;
	for(pNode=pModel->Node.GetFirst();pNode;pNode=pModel->Node.GetNext())
	{
		for(pCheckNode=pModel->Node.GetNext();pCheckNode;pCheckNode=pModel->Node.GetNext())
		{
			if(fabs(pNode->Position(false).x-pCheckNode->Position(false).x)<EPS&&fabs(pNode->Position(false).y+pCheckNode->Position(false).y)<EPS)
				relaObj.mirInfo.axis_flag=1;	//Y坐标反号是Ｘ轴对称
			else if(fabs(pNode->Position(false).x+pCheckNode->Position(false).x)<EPS&&fabs(pNode->Position(false).y-pCheckNode->Position(false).y)<EPS)
				relaObj.mirInfo.axis_flag=2;	//X坐标反号是Ｙ轴对称
			else if(fabs(pNode->Position(false).x+pCheckNode->Position(false).x)<EPS&&fabs(pNode->Position(false).y+pCheckNode->Position(false).y)<EPS)
				relaObj.mirInfo.axis_flag=4;	//X&Y坐标反号是Ｚ轴对称
			else
				continue;	//无对称
			relaObj.hObj=pCheckNode->handle;
			pNode->AppendRelativeObj(relaObj);
			relaObj.hObj=pNode->handle;
			pCheckNode->AppendRelativeObj(relaObj);
		}
	}
	//匹配杆件对称信息
	CLDSLinePart* pLinePart=NULL,*pCheckLinePart=NULL;
	for(pLinePart=(CLDSLinePart*)pModel->Parts.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pModel->Parts.GetNext())
	{
		for(pCheckLinePart=(CLDSLinePart*)pModel->Parts.GetNext();pCheckLinePart;pCheckLinePart=(CLDSLinePart*)pModel->Parts.GetNext())
		{
			if( fabs(pLinePart->pStart->Position(false).x-pCheckLinePart->pStart->Position(false).x)<EPS&&
				fabs(pLinePart->pStart->Position(false).y+pCheckLinePart->pStart->Position(false).y)<EPS&&
				fabs(pLinePart->pEnd->Position(false).x-pCheckLinePart->pEnd->Position(false).x)<EPS&&
				fabs(pLinePart->pEnd->Position(false).y+pCheckLinePart->pEnd->Position(false).y)<EPS)
				relaObj.mirInfo.axis_flag=1;	//Y坐标反号是Ｘ轴对称
			else if( fabs(pLinePart->pStart->Position(false).x+pCheckLinePart->pStart->Position(false).x)<EPS&&
				fabs(pLinePart->pStart->Position(false).y-pCheckLinePart->pStart->Position(false).y)<EPS&&
				fabs(pLinePart->pEnd->Position(false).x+pCheckLinePart->pEnd->Position(false).x)<EPS&&
				fabs(pLinePart->pEnd->Position(false).y-pCheckLinePart->pEnd->Position(false).y)<EPS)
				relaObj.mirInfo.axis_flag=2;	//X坐标反号是Ｙ轴对称
			else if( fabs(pLinePart->pStart->Position(false).x+pCheckLinePart->pStart->Position(false).x)<EPS&&
				fabs(pLinePart->pStart->Position(false).y+pCheckLinePart->pStart->Position(false).y)<EPS&&
				fabs(pLinePart->pEnd->Position(false).x+pCheckLinePart->pEnd->Position(false).x)<EPS&&
				fabs(pLinePart->pEnd->Position(false).y+pCheckLinePart->pEnd->Position(false).y)<EPS)
				relaObj.mirInfo.axis_flag=4;	//X&Y坐标反号是Ｚ轴对称
			else
				continue;	//无对称
			relaObj.hObj=pCheckLinePart->handle;
			pLinePart->AppendRelativeObj(relaObj);
			relaObj.hObj=pLinePart->handle;
			pCheckLinePart->AppendRelativeObj(relaObj);
		}
	}
}

CLDSNode* CParaBlockModelTemplate::InsertNewNode(CParaBlockModel* pBlockModel,CParaModelNode* pParaNode,f3dPoint pos)
{
	CLDSNode *pNode=pBlockModel->AppendNode();
	pNode->SetPosition(pos);
	pNode->layer(0)='S';
	if(m_iTempType==CParaBlockModelTemplate::CROSSARM_TEMPLATE)
		pNode->layer(0)='T';			//横担属于头部
	pNode->layer(1)='P';
	if(pos.x>=0&&pos.y>=0)
		pNode->layer(2)='1';
	else if(pos.x<=0&&pos.y>=0)
		pNode->layer(2)='2';
	else if(pos.x>=0&&pos.y<=0)
		pNode->layer(2)='3';
	else //if(pos.x<=0&&pos.y<=0)
		pNode->layer(2)='4';
	for(int i=0;i<3;i++)
	{	
		if(pParaNode->sPreferLayer[i]!=0&&pParaNode->sPreferLayer[i]!='?'&&pParaNode->sPreferLayer[i]!='*')
			pNode->layer(i)=pParaNode->sPreferLayer[i];
	}
	LoadTagInfo(pParaNode)->pModelNode=pNode;
	return pNode;
}
static char restore_JG_guige(const char* guige, double &wing_wide, double &wing_thick)
{
	char mark,material;
	sscanf(guige,"%lf%c%lf%c",&wing_wide,&mark,&wing_thick,&material);
	material  = (char)toupper(material);//角钢材质A3(S)或Q345(H)
	return material;
}
CLDSLinePart* CParaBlockModelTemplate::InsertNewPole(CParaBlockModel* pBlockModel,CParaModelRod *pParaRod)
{
	CParaModelNode* pParaStart=listParaNodes.GetValue(pParaRod->keyStartNode);
	CParaModelNode* pParaEnd=listParaNodes.GetValue(pParaRod->keyEndNode);
	CLDSLinePart* pPole=(CLDSLinePart*)pBlockModel->AppendPart(pParaRod->m_idPartClsType);
	pPole->SetStart(pParaStart->TagInfo()->pModelNode->Position(false));	
	pPole->SetEnd(pParaEnd->TagInfo()->pModelNode->Position(false));
	pPole->pStart=pParaStart->TagInfo()->pModelNode;
	pPole->pEnd=pParaEnd->TagInfo()->pModelNode;
	pPole->layer(0)='S';
	if(m_iTempType==CParaBlockModelTemplate::CROSSARM_TEMPLATE)
		pPole->layer(0)='T';
	pPole->layer(1)='X';
	if(pParaRod->m_bMajorMat)
		pPole->layer(1)='Z';
	pPole->layer(2)=CalPoleQuad(pPole->pStart->Position(false),pPole->pEnd->Position(false));
	for(int i=0;i<3;i++)
	{	
		if(pParaRod->sPreferLayer[i]!=0&&pParaRod->sPreferLayer[i]!='?'&&pParaRod->sPreferLayer[i]!='*')
			pPole->layer(i)=pParaRod->sPreferLayer[i];
	}
	CParaModelRod* pWorkplaneParaRod=NULL;
	if(pParaRod->keyRefPole.dwItem!=0)
		pWorkplaneParaRod=listParaRods.GetValue(pParaRod->keyRefPole);
	CLDSLinePart* pWorkplaneRefRod=pWorkplaneParaRod!=NULL&&pWorkplaneParaRod->TagInfo()!=NULL?pWorkplaneParaRod->TagInfo()->pModelRod:NULL;
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
	if(pPole->GetClassTypeId()==CLS_LINEANGLE)
	{
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pPole;
		if(pParaRod->m_sGuige.GetLength()>0)
		{
			double wing_wide,wing_thick;
			restore_JG_guige(pParaRod->m_sGuige,wing_wide,wing_thick);
			pAngle->set_wing_wide(wing_wide);
			pAngle->set_wing_thick(wing_thick);
		}
		pAngle->dwLayStyle=pParaRod->layout_style+1;
		if(pParaRod->layout_style==0)
		{
			if(pParaRod->eyeSideNorm.IsZero())
				pAngle->des_norm_x.near_norm.Set(0,-1,0);
			else
				pAngle->des_norm_x.near_norm.Set(-pParaRod->eyeSideNorm.x,-pParaRod->eyeSideNorm.y,-pParaRod->eyeSideNorm.z);
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
		else if(pParaRod->layout_style==1)
		{
			if(pParaRod->eyeSideNorm.IsZero())
				pAngle->des_norm_y.near_norm.Set(0,1,0);
			else
				pAngle->des_norm_y.near_norm=pParaRod->eyeSideNorm;
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
		else if(pParaRod->layout_style==2)
		{
			if(pParaRod->eyeSideNorm.IsZero())
				pAngle->des_norm_x.near_norm.Set(0,1,0);
			else
				pAngle->des_norm_x.near_norm=pParaRod->eyeSideNorm;
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
			if(pParaRod->eyeSideNorm.IsZero())
				pAngle->des_norm_y.near_norm.Set(0,-1,0);
			else
				pAngle->des_norm_y.near_norm.Set(-pParaRod->eyeSideNorm.x,-pParaRod->eyeSideNorm.y,-pParaRod->eyeSideNorm.z);
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
		if(pParaRod->m_sGuige.GetLength()>0)
		{
			double wide=0,thick=0;
			restore_JG_guige(pParaRod->m_sGuige,wide,thick);
			pTube->size_wide = wide;
			pTube->size_thick = thick;
		}
		if(pWorkplaneRefRod)
		{
			pTube->m_iNormCalStyle=1;
			pTube->hWorkNormRefPart=pWorkplaneRefRod->handle;
		}
		else if(pParaRod->keyRefPole==0)//&&!pParaRod->pickWorkPlane.IsZero())
		{	//根据共面参照点计算钢管的工作面
			pTube->m_iNormCalStyle=2;
			pTube->pickWorkPlane[0]=pParaRod->pickWorkPlane.x;
			pTube->pickWorkPlane[1]=pParaRod->pickWorkPlane.y;
			pTube->pickWorkPlane[2]=pParaRod->pickWorkPlane.z;
			pTube->CalWorkPlaneNorm();
		}
	}
#endif
#ifdef __PART_DESIGN_INC_
	if(pParaRod->start_force_type==0)
		pPole->start_force_type=CENTRIC_FORCE;
	else
		pPole->start_force_type=ECCENTRIC_FORCE;
	if(pParaRod->end_force_type==0)
		pPole->end_force_type=CENTRIC_FORCE;
	else
		pPole->end_force_type=ECCENTRIC_FORCE;;
	if(pParaRod->start_joint_type==0)
		pPole->start_joint_type=JOINT_HINGE;
	else if(pParaRod->start_joint_type==1)
		pPole->start_joint_type=JOINT_RIGID;
	else
		pPole->start_joint_type=JOINT_AUTOJUSTIFY;
	if(pParaRod->end_joint_type==0)
		pPole->end_joint_type=JOINT_HINGE;
	else if(pParaRod->end_joint_type==1)
		pPole->end_joint_type=JOINT_RIGID;
	else
		pPole->end_joint_type=JOINT_AUTOJUSTIFY;;
	pPole->m_iElemType=pParaRod->m_iElemType;
#endif
	LoadTagInfo(pParaRod)->pModelRod=pPole;
	return pPole;
}
CLDSParamPlate* CParaBlockModelTemplate::InsertNewPlate(CParaBlockModel* pBlockModel,CParaModelParamPlate* pParaPlate,CExpression& expression)
{
	if(pParaPlate->m_iParamType==CParaModelParamPlate::TYPE_D_TOPPLATE||pParaPlate->m_iParamType==CParaModelParamPlate::TYPE_L_TOPPLATE
		||pParaPlate->m_iParamType==CParaModelParamPlate::TYPE_Q_BASEPLATE||pParaPlate->m_iParamType==CParaModelParamPlate::TYPE_Q_TOPPLATE)
	{
		CParaModelNode* pDatumNode=listParaNodes.GetValue(pParaPlate->keyDatumNode);
		if(pDatumNode==NULL || pDatumNode->TagInfo()->pModelNode==NULL)
			return NULL;
		CLDSNode* pBaseNode=pDatumNode->TagInfo()->pModelNode;
		CLDSLinePart* pBaseRod=NULL;
		CParaModelRod* pDatumRod=listParaRods.GetValue(pParaPlate->keyDatumPart);
		if(pDatumRod)
			pBaseRod=pDatumRod->TagInfo()->pModelRod;
		//设计基板
		CLDSParamPlate* pBasePlate=(CLDSParamPlate*)pBlockModel->AppendPart(CLS_PARAMPLATE);
		pBasePlate->m_iParamType=TYPE_BASEPLATE;
		pBasePlate->m_bDisplayHole=true;
		pBasePlate->thick=20;
		f3dPoint origin=pDatumNode->TagInfo()->pModelNode->Position(true);
		pBasePlate->design.origin.SetPosition(origin);
		pBasePlate->SetDesignItemValue('A',pBaseNode->handle,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);
		pBasePlate->SetDesignItemValue('L',0,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);
		if(pBaseRod)
			pBasePlate->SetDesignItemValue('L',pBaseRod->handle,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);
		long iAxisZType=CStdVector::GetVectorType(pParaPlate->workNorm.vector);
		pBasePlate->SetDesignItemValue('Z',iAxisZType,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_INTEGER);
		double fNormOff=expression.SolveExpression(pParaPlate->off_expression);
		if(fabs(fNormOff)>0)
			pBasePlate->SetDesignItemValue('N',fNormOff+pBasePlate->thick,NULL,PARAM_POS);	//法线偏移
		else
			pBasePlate->SetDesignItemValue('C',1,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_INTEGER);	//节点在板的位置
		if(pParaPlate->feature==1&&pBaseRod&&pBaseRod->IsTube())
		{	//圆形顶板
			pBasePlate->SetDesignItemValue('S',1,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER);		//底板外形
			pBasePlate->SetDesignItemValue('O',1,NULL,PARAM_POS);	//基板原点位置
			double d=((CLDSLineTube*)pBaseRod)->GetDiameter()+200;
			pBasePlate->SetDesignItemValue(KEY2C("R"),d*0.5,NULL,PARAM_SHAPE);	//半径
		}
		else
		{	//方形顶板
			double ff[4]={400,400,800,800};
			if(pParaPlate->m_iParamType==CParaModelParamPlate::TYPE_Q_BASEPLATE)
				ff[0]=ff[1]=ff[2]=ff[3]=200;
			else if(pParaPlate->m_iParamType==CParaModelParamPlate::TYPE_Q_TOPPLATE)
			{
				ff[0]=ff[1]=ff[2]=ff[3]=200;
				CParaModelParamPlate* pAssistPlate=listParaPlates.GetValue(pParaPlate->keyAssistPart);
				if(pAssistPlate && pAssistPlate->TagInfo()->pModelPlate)
				{
					double fValue=0;
					pAssistPlate->TagInfo()->pModelPlate->GetDesignItemValue('N',&fValue);
					ff[3]=fabs(fValue)-pAssistPlate->TagInfo()->pModelPlate->thick;
				}
			}
			pBasePlate->SetDesignItemValue('S',0,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER);		//底板外形
			pBasePlate->SetDesignItemValue(KEY2C("XA"),ff[0],NULL,PARAM_SHAPE);
			pBasePlate->SetDesignItemValue(KEY2C("XB"),ff[1],NULL,PARAM_SHAPE);
			pBasePlate->SetDesignItemValue(KEY2C("YA"),ff[2],NULL,PARAM_SHAPE);
			pBasePlate->SetDesignItemValue(KEY2C("YB"),ff[3],NULL,PARAM_SHAPE);
			for(int i=0;i<4;i++)
			{
				pBasePlate->SetDesignItemValue(KEY2C(CXhChar16("X%d",i)),0,NULL,PARAM_SHAPE);
				pBasePlate->SetDesignItemValue(KEY2C(CXhChar16("Y%d",i)),0,NULL,PARAM_SHAPE);
			}
		}
		return pBasePlate;
	}
	else if(pParaPlate->m_iParamType==CParaModelParamPlate::TYPE_D_ENDPLATE)	//端撑连接板
	{
		CParaModelParamPlate* pDatumPlate=listParaPlates.GetValue(pParaPlate->keyDatumPart);
		if(pDatumPlate==NULL || pDatumPlate->TagInfo()->pModelPlate==NULL)
			return NULL;
		CLDSParamPlate* pBasePlate=(CLDSParamPlate*)pDatumPlate->TagInfo()->pModelPlate;
		CLDSParamPlate* pBracePlate =(CLDSParamPlate*)pBlockModel->AppendPart(CLS_PARAMPLATE);
		pBracePlate->m_hPartWeldParent=pBasePlate->handle;
		pBracePlate->CopyModuleInstanceInfo(pBasePlate);
		pBracePlate->m_iParamType=TYPE_STATIONCONNPLATE;
		pBracePlate->thick=20;
		//
		pBracePlate->SetDesignItemValue(KEY2C("LP"),pBasePlate->handle,NULL,0,DESIGN_PARAM_ITEM::TYPE_HANDLE);
		if(pParaPlate->workNorm.vector.IsEqual(f3dPoint(1,0,0)))
			pBracePlate->SetDesignItemValue('I',0,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_INTEGER);
		else
			pBracePlate->SetDesignItemValue('I',1,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_INTEGER);

		pBracePlate->SetDesignItemValue('D',50,NULL,PARAM_POS);
		pBracePlate->SetDesignItemValue('H',500,NULL,PARAM_SHAPE);
		pBracePlate->SetDesignItemValue('C',60,NULL,PARAM_SHAPE);
		pBracePlate->SetDesignItemValue('r',60,NULL,PARAM_SHAPE);
		pBracePlate->SetDesignItemValue('R',150,NULL,PARAM_SHAPE);
		//填充基板的附加板信息
		ATTACHPART_ITEM* pAttachItem=pBasePlate->AttachPart.Add(pBracePlate->handle);
		pAttachItem->idClsType=pBracePlate->GetClassTypeId();
		return pBracePlate;
	}
	else if(pParaPlate->m_iParamType==CParaModelParamPlate::TYPE_D_CHAPLATE)		//插板
	{
		CParaModelParamPlate* pDatumPlate=listParaPlates.GetValue(pParaPlate->keyDatumPart);
		CParaModelParamPlate* pAssistPlate=listParaPlates.GetValue(pParaPlate->keyAssistPart);
		if(pDatumPlate==NULL || pDatumPlate->TagInfo()->pModelPlate==NULL)
			return NULL;
		CLDSParamPlate* pBasePlate=NULL,*pDependPlate=NULL;
		pBasePlate=(CLDSParamPlate*)pDatumPlate->TagInfo()->pModelPlate;
		if(pAssistPlate)
			pDependPlate=(CLDSParamPlate*)pAssistPlate->TagInfo()->pModelPlate;
		CLDSParamPlate* pSpilePlate =(CLDSParamPlate*)pBlockModel->AppendPart(CLS_PARAMPLATE);
		pSpilePlate->m_hPartWeldParent=pBasePlate->handle;
		pSpilePlate->CopyModuleInstanceInfo(pBasePlate);
		pSpilePlate->m_iParamType=TYPE_STATIONCROSSPLATE;
		pSpilePlate->thick=20;
		//
		pSpilePlate->SetDesignItemValue(KEY2C("LP"),pBasePlate->handle,NULL,0,DESIGN_PARAM_ITEM::TYPE_HANDLE);
		if(pParaPlate->workNorm.vector.IsEqual(f3dPoint(0,1,1)))	//Y通向插板
			pSpilePlate->SetDesignItemValue('I',3,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_INTEGER);
		else if(pParaPlate->workNorm.vector.IsEqual(f3dPoint(1,0,0)))	//X+向插板
		{	
			pSpilePlate->SetDesignItemValue('I',1,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_INTEGER);
			if(pDependPlate)
				pSpilePlate->SetDesignItemValue(KEY2C("LA"),pDependPlate->handle,NULL,0,DESIGN_PARAM_ITEM::TYPE_HANDLE);
		}
		else if(pParaPlate->workNorm.vector.IsEqual(f3dPoint(-1,0,0)))	//X-向插板
		{
			pSpilePlate->SetDesignItemValue('I',2,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_INTEGER);
			if(pDependPlate)
				pSpilePlate->SetDesignItemValue(KEY2C("LA"),pDependPlate->handle,NULL,0,DESIGN_PARAM_ITEM::TYPE_HANDLE);
		}
		pSpilePlate->SetDesignItemValue('H',500,NULL,PARAM_SHAPE);
		//填充基板的附加板信息
		ATTACHPART_ITEM* pAttachItem=pBasePlate->AttachPart.Add(pSpilePlate->handle);
		pAttachItem->idClsType=pSpilePlate->GetClassTypeId();
		return pSpilePlate;
	}
	else if(pParaPlate->m_iParamType==CParaModelParamPlate::TYPE_D_LINKPLATE)	//端撑连接板
	{
		CParaModelNode* pDatumNode=listParaNodes.GetValue(pParaPlate->keyDatumNode);
		CParaModelRod* pDatumRod=listParaRods.GetValue(pParaPlate->keyDatumPart);
		if(pDatumNode==NULL || pDatumNode->TagInfo()->pModelNode==NULL||
			pDatumRod==NULL || pDatumRod->TagInfo()->pModelRod==NULL||pDatumRod->TagInfo()->pModelRod->GetClassTypeId()!=CLS_LINETUBE)
			return NULL;
		CParaModelRod* pRod1=listParaRods.GetValue(pParaPlate->keyAssistPart);
		CParaModelRod* pRod2=listParaRods.GetValue(pParaPlate->keyAssistPart2);
		if(pRod1==NULL||pRod1->TagInfo()->pModelRod==NULL||pRod1->TagInfo()->pModelRod->GetClassTypeId()!=CLS_LINEANGLE||
			pRod2==NULL||pRod2->TagInfo()->pModelRod==NULL||pRod2->TagInfo()->pModelRod->GetClassTypeId()!=CLS_LINEANGLE)
			return NULL;
		CLDSNode* pBaseNode=pDatumNode->TagInfo()->pModelNode;
		CLDSLineTube* pBaseTube=(CLDSLineTube*)pDatumRod->TagInfo()->pModelRod;
		CLDSLineAngle* pAssistJg1=(CLDSLineAngle*)pRod1->TagInfo()->pModelRod;
		CLDSLineAngle* pAssistJg2=(CLDSLineAngle*)pRod2->TagInfo()->pModelRod;
		CLDSParamPlate* pMidLinkPlate =(CLDSParamPlate*)pBlockModel->AppendPart(CLS_PARAMPLATE);
		pMidLinkPlate->m_iParamType=TYPE_STATIONLINKPLATE;
		pMidLinkPlate->m_bDisplayHole=true;
		pMidLinkPlate->thick=20;
		//初始化设计参数
		pMidLinkPlate->SetDesignItemValue('A',pBaseTube->handle,NULL,PARAM_POS,1);
		pMidLinkPlate->SetDesignItemValue('B',pBaseNode->handle,NULL,PARAM_POS,1);
		pMidLinkPlate->SetDesignItemValue('C',pAssistJg1->handle,NULL,PARAM_POS,1);
		pMidLinkPlate->SetDesignItemValue('D',pAssistJg2->handle,NULL,PARAM_POS,1);
		long iAxisZType=CStdVector::GetVectorType(pParaPlate->workNorm.vector);
		pMidLinkPlate->SetDesignItemValue('Z',iAxisZType,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_INTEGER);
		pMidLinkPlate->SetDesignItemValue(KEY2C("OW"),0,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_INTEGER);	//肢翼侧偏移量
		pMidLinkPlate->SetDesignItemValue(KEY2C("OB"),0,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_INTEGER);	//楞线侧偏移量
	}
	else if(pParaPlate->m_iParamType==CParaModelParamPlate::TYPE_L_NODEPLATE)	//立柱连接板
	{
		CParaModelNode* pDatumNode=listParaNodes.GetValue(pParaPlate->keyDatumNode);
		CParaModelRod* pDatumRod=listParaRods.GetValue(pParaPlate->keyDatumPart);
		if(pDatumNode==NULL || pDatumNode->TagInfo()->pModelNode==NULL||
			pDatumRod==NULL || pDatumRod->TagInfo()->pModelRod==NULL||pDatumRod->TagInfo()->pModelRod->GetClassTypeId()!=CLS_LINETUBE)
			return NULL;
		CParaModelRod* pRod1=listParaRods.GetValue(pParaPlate->keyAssistPart);
		if(pRod1==NULL||pRod1->TagInfo()->pModelRod==NULL||pRod1->TagInfo()->pModelRod->GetClassTypeId()!=CLS_LINEANGLE)
			return NULL;
		CLDSNode* pBaseNode=pDatumNode->TagInfo()->pModelNode;
		CLDSLineTube* pBaseTube=(CLDSLineTube*)pDatumRod->TagInfo()->pModelRod;
		CLDSLineAngle* pAssistJg=(CLDSLineAngle*)pRod1->TagInfo()->pModelRod;
		CLDSParamPlate* pNodePlate =(CLDSParamPlate*)pBlockModel->AppendPart(CLS_PARAMPLATE);
		pNodePlate->m_iParamType=TYPE_STATIONNODEPLATE;
		pNodePlate->m_bDisplayHole=true;
		pNodePlate->thick=20;
		//初始化设计参数
		pNodePlate->SetDesignItemValue('A',pBaseNode->handle,NULL,PARAM_POS,1);
		pNodePlate->SetDesignItemValue('B',pBaseTube->handle,NULL,PARAM_POS,1);
		pNodePlate->SetDesignItemValue('C',pAssistJg->handle,NULL,PARAM_POS,1);
		long iAxisZType=CStdVector::GetVectorType(pParaPlate->workNorm.vector);
		pNodePlate->SetDesignItemValue('Z',iAxisZType,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_INTEGER);
	}
	else if(pParaPlate->m_iParamType==CParaModelParamPlate::TYPE_L_JIAPLATE)	//夹板
	{
		CParaModelParamPlate* pDatumPlate=listParaPlates.GetValue(pParaPlate->keyDatumPart);
		CParaModelParamPlate* pAssistPlate=listParaPlates.GetValue(pParaPlate->keyAssistPart);
		if(pDatumPlate==NULL || pDatumPlate->TagInfo()->pModelPlate==NULL)
			return NULL;
		CLDSParamPlate* pBasePlate=NULL,*pEndPlate=NULL;
		pBasePlate=(CLDSParamPlate*)pDatumPlate->TagInfo()->pModelPlate;
		if(pAssistPlate)
			pEndPlate=(CLDSParamPlate*)pAssistPlate->TagInfo()->pModelPlate;
		CLDSParamPlate* pPlywoodPlate =(CLDSParamPlate*)pBlockModel->AppendPart(CLS_PARAMPLATE);
		pPlywoodPlate->m_iParamType=TYPE_STATIONSHOEPLATE;	//基板
		pPlywoodPlate->m_bDisplayHole = true;
		pPlywoodPlate->thick=20;
		pPlywoodPlate->CopyModuleInstanceInfo(pBasePlate);
		//
		pPlywoodPlate->SetDesignItemValue(KEY2C("LB"),pBasePlate->handle,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);
		if(pEndPlate&&pEndPlate->m_iParamType==TYPE_STATIONCONNPLATE)
		{
			pPlywoodPlate->SetDesignItemValue(KEY2C("LE"),pEndPlate->handle,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);
			pPlywoodPlate->SetDesignItemValue('R',200,NULL,PARAM_SHAPE);	//半径
		}
		else
		{
			pPlywoodPlate->SetDesignItemValue(KEY2C("LE"),0,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);
			pPlywoodPlate->SetDesignItemValue('H',500,NULL,PARAM_SHAPE);	//板高
		}
		pPlywoodPlate->SetDesignItemValue('A',50,NULL,PARAM_SHAPE);		//根高
		pPlywoodPlate->SetDesignItemValue('B',150,NULL,PARAM_SHAPE);	//倒角偏移距
		pPlywoodPlate->SetDesignItemValue(KEY2C("CR"),150,NULL,PARAM_SHAPE);		//倒角半径
		//填充基板的附加板信息
		ATTACHPART_ITEM* pAttachItem=pBasePlate->AttachPart.Add(pPlywoodPlate->handle);
		pAttachItem->idClsType=pPlywoodPlate->GetClassTypeId();
		return pPlywoodPlate;
	}
	else if(pParaPlate->m_iParamType==CParaModelParamPlate::TYPE_Q_RIBPLATE)
	{
		CParaModelParamPlate* pDatumPlate=listParaPlates.GetValue(pParaPlate->keyDatumPart);
		CParaModelParamPlate* pAssistPlate=listParaPlates.GetValue(pParaPlate->keyAssistPart);
		if(pDatumPlate==NULL || pDatumPlate->TagInfo()->pModelPlate==NULL||
		   pAssistPlate==NULL ||pAssistPlate->TagInfo()->pModelPlate==NULL)
			return NULL;
		CLDSParamPlate* pBasePlate=pDatumPlate->TagInfo()->pModelPlate;
		CLDSParamPlate* pEndPlate=pAssistPlate->TagInfo()->pModelPlate;
		double fLen1=0,fLen2=0;
		pEndPlate->GetDesignItemValue(KEY2C("YA"),&fLen1);
		pEndPlate->GetDesignItemValue(KEY2C("YB"),&fLen2);
		CLDSParamPlate* pRibPlate =(CLDSParamPlate*)pBlockModel->AppendPart(CLS_PARAMPLATE);
		pRibPlate->m_iParamType=TYPE_RIBPLATE;
		pRibPlate->m_bDisplayHole = true;
		pRibPlate->thick=20;
		pRibPlate->CopyModuleInstanceInfo(pBasePlate);
		//初始化设计参数
		pRibPlate->SetDesignItemValue('A',pBasePlate->handle,NULL,PARAM_POS,1);
		pRibPlate->SetDesignItemValue('C',pEndPlate->handle,NULL,PARAM_POS,1);	
		pRibPlate->SetDesignItemValue(KEY2C("Ox"),0,NULL,PARAM_POS,0);
		pRibPlate->SetDesignItemValue(KEY2C("Oy"),0,NULL,PARAM_POS,0);
		pRibPlate->SetDesignItemValue('E',fLen1+fLen2,NULL,PARAM_SHAPE);
		pRibPlate->SetDesignItemValue('F',50,NULL,PARAM_SHAPE);
		pRibPlate->SetDesignItemValue('G',180,NULL,PARAM_SHAPE);
		pRibPlate->SetDesignItemValue('H',50,NULL,PARAM_SHAPE);
		pRibPlate->SetDesignItemValue('e',0,NULL,PARAM_SHAPE,1);
		pRibPlate->SetDesignItemValue('I',40,NULL,PARAM_SHAPE);
	}
	return NULL;
}
bool AssignAnglePos(CMultiOffsetPos& desBerPos,CParaModelAngle::POS desTemplPos)
{
	if(desTemplPos.cTouchLineType==0)
	{
		if(desTemplPos.cTouchLine==0)
		{
			if(desTemplPos.face_offset_norm.IsZero())	//投影
				desBerPos.spatialOperationStyle=2;	//向X肢心线投影
			else
			{
				desBerPos.spatialOperationStyle=5;	//向X肢心线平推
				desBerPos.face_offset_norm=desTemplPos.face_offset_norm;
				desBerPos.wing_x_offset=desTemplPos.XOrCtrlWing;
				desBerPos.wing_y_offset=desTemplPos.YOrOtherWing;
			}
		}
		else if(desTemplPos.cTouchLine==1)
		{
			if(desTemplPos.face_offset_norm.IsZero())	//投影
				desBerPos.spatialOperationStyle=3;	//向X肢心线投影
			else
			{
				desBerPos.spatialOperationStyle=6;	//向X肢心线平推
				desBerPos.face_offset_norm=desTemplPos.face_offset_norm;
				desBerPos.wing_x_offset=desTemplPos.XOrCtrlWing;
				desBerPos.wing_y_offset=desTemplPos.YOrOtherWing;
			}
		}
		else if(desTemplPos.cTouchLine==2)
		{
			if(desTemplPos.face_offset_norm.IsZero())	//投影
				desBerPos.spatialOperationStyle=1;	//向X肢心线投影
			else
			{
				desBerPos.spatialOperationStyle=4;	//向X肢心线平推
				desBerPos.face_offset_norm=desTemplPos.face_offset_norm;
				desBerPos.wing_x_offset=desTemplPos.XOrCtrlWing;
				desBerPos.wing_y_offset=desTemplPos.YOrOtherWing;
			}
		}
		else
			return false;
	}
	else if(desTemplPos.cTouchLineType==1)
	{
		//KEY4C labelHuoquRod;
		if(desTemplPos.cTouchLine==0)
		{
			if(desTemplPos.face_offset_norm.IsZero())	//投影
				desBerPos.spatialOperationStyle=8;	//向X肢心线投影
			else
			{
				desBerPos.spatialOperationStyle=11;	//向X肢心线火曲平推
				desBerPos.face_offset_norm=desTemplPos.face_offset_norm;
				desBerPos.wing_x_offset=desTemplPos.XOrCtrlWing;
				desBerPos.wing_y_offset=desTemplPos.YOrOtherWing;
			}
		}
		else if(desTemplPos.cTouchLine==1)
		{
			if(desTemplPos.face_offset_norm.IsZero())	//投影
				desBerPos.spatialOperationStyle=9;	//向X肢心线投影
			else
			{
				desBerPos.spatialOperationStyle=12;	//向Y肢心线火曲线平推
				desBerPos.face_offset_norm=desTemplPos.face_offset_norm;
				desBerPos.wing_x_offset=desTemplPos.XOrCtrlWing;
				desBerPos.wing_y_offset=desTemplPos.YOrOtherWing;
			}
		}
		else if(desTemplPos.cTouchLine==2)
		{
			if(desTemplPos.face_offset_norm.IsZero())	//投影
				desBerPos.spatialOperationStyle=7;	//向X肢心线投影
			else
			{
				desBerPos.spatialOperationStyle=10;	//向X肢楞线火曲线平推
				desBerPos.face_offset_norm=desTemplPos.face_offset_norm;
				desBerPos.wing_x_offset=desTemplPos.XOrCtrlWing;
				desBerPos.wing_y_offset=desTemplPos.YOrOtherWing;
			}
		}
		else
			return false;
	}
	else if(desTemplPos.cTouchLineType==3)
	{
		if(desTemplPos.cMainCoordCtrlWing=='X'||desTemplPos.cMainCoordCtrlWing=='Y')	//投影
		{
			desBerPos.spatialOperationStyle=15;	//两肢坐标控制
			desBerPos.coordCtrlType=desTemplPos.coordCtrlType;	//0:X-Z主控;1:Y-Z主控;2.X-Y主控(仅在spatialOperationStyle=15时有效)
			desBerPos.cMainCoordCtrlWing=desTemplPos.cMainCoordCtrlWing;
			desBerPos.cFaceOffsetDatumLine=desTemplPos.cFaceOffsetDatumLine;
			desBerPos.ctrlWing=desTemplPos.XOrCtrlWing;
			desBerPos.otherWing=desTemplPos.YOrOtherWing;
		}
		else
		{
			if(desTemplPos.face_offset_norm.IsZero())
				desBerPos.spatialOperationStyle=18;
			else
			{
				desBerPos.spatialOperationStyle=19;
				desBerPos.face_offset_norm=desTemplPos.face_offset_norm;
			}
			desBerPos.wing_x_offset=desTemplPos.XOrCtrlWing;
			desBerPos.wing_y_offset=desTemplPos.YOrOtherWing;
		}
	}
	else
		return false;
	return true;
}
void CParaBlockModelTemplate::RebuildModel(CParaBlockModel* pBlockModel)
{
	//将double数组插入到设计参数中
	DESIGN_PARAM_ITEM *pItem=NULL;
	ARRAY_LIST<double>* pArray=NULL;
	pArray=pBlockModel->hashArrayParamValues.GetValue(KEY4C("LCBF"));
	if(pArray)
	{
		pItem=pBlockModel->listParams.Add(KEY4C("LCBF"));
		pItem->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE_ARR;
		pItem->value.arrValues=pArray->m_pData;
		pItem->value.uiUpperBound=pArray->GetSize();
	}
	pArray=pBlockModel->hashArrayParamValues.GetValue(KEY4C("RCBF"));
	if(pArray)
	{
		pItem=pBlockModel->listParams.Add(KEY4C("RCBF"));
		pItem->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE_ARR;
		pItem->value.arrValues=pArray->m_pData;
		pItem->value.uiUpperBound=pArray->GetSize();
	}
	pArray=pBlockModel->hashArrayParamValues.GetValue(KEY4C("LSBF"));
	if(pArray)
	{
		pItem=pBlockModel->listParams.Add(KEY4C("LSBF"));
		pItem->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE_ARR;
		pItem->value.arrValues=pArray->m_pData;
		pItem->value.uiUpperBound=pArray->GetSize();
	}
	pArray=pBlockModel->hashArrayParamValues.GetValue(KEY4C("RSBF"));
	if(pArray)
	{
		pItem=pBlockModel->listParams.Add(KEY4C("RSBF"));
		pItem->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE_ARR;
		pItem->value.arrValues=pArray->m_pData;
		pItem->value.uiUpperBound=pArray->GetSize();
	}
	pArray=pBlockModel->hashArrayParamValues.GetValue(KEY4C("LTBF"));
	if(pArray)
	{
		pItem=pBlockModel->listParams.Add(KEY4C("LTBF"));
		pItem->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE_ARR;
		pItem->value.arrValues=pArray->m_pData;
		pItem->value.uiUpperBound=pArray->GetSize();
	}
	pArray=pBlockModel->hashArrayParamValues.GetValue(KEY4C("RTBF"));
	if(pArray)
	{
		pItem=pBlockModel->listParams.Add(KEY4C("RTBF"));
		pItem->dataType=DESIGN_PARAM_ITEM::TYPE_DOUBLE_ARR;
		pItem->value.arrValues=pArray->m_pData;
		pItem->value.uiUpperBound=pArray->GetSize();
	}
	//设计参数及表达式处理
	CExpression expression;
	expression.varList.Empty();
	for(pItem=pBlockModel->listParams.GetFirst();pItem;pItem=pBlockModel->listParams.GetNext())
		expression.varList.Append(ConvertParamItemToExprVar(pItem));
	listTagNodeInfo.Empty();
	listTagRodInfo.Empty();
	listTagPlateInfo.Empty();
	pBlockModel->Empty();		//清空模型
	listRepeatedParaNodes.Empty();
	listRepeatedParaRods.Empty();
	//1 节点插入操作
	CLDSNode* pNode=NULL;
	EXPRESSION_VAR* pIndexVar=NULL;
	CParaModelNode* pParaNode=NULL,*pRepeatNode=NULL,*pPrevRepeatNode=NULL;
	for(pParaNode=listParaNodes.GetFirst();pParaNode;pParaNode=listParaNodes.GetNext())
	{
		int repeat_num=0;	//默认不重复
		KEY4C key=KEY4C(pParaNode->keyLabel);
		if(pParaNode->keyRepeatParam.dwItem>0)	//重复插入循环节点
		{
			pItem=pBlockModel->listParams.GetValue(pParaNode->keyRepeatParam);
			if(pItem)
				repeat_num=pItem->value.iType;
		}
		int i=1;
		CParaModelNode* pTemplParaNode=pParaNode;
		pRepeatNode=NULL;
		do
		{
			if(repeat_num>0)
			{
				if(pIndexVar==NULL)
					pIndexVar=expression.varList.Append();
				pIndexVar->fValue=i;
				strcpy(pIndexVar->variableStr,"i");
				if(pParaNode->exist_logicexpr[0]!='\0'&&!expression.SolveLogicalExpression(pParaNode->exist_logicexpr))
				{
					i++;
					continue;
				}
				pPrevRepeatNode=pRepeatNode;
				pRepeatNode=listRepeatedParaNodes.Add(KEY4C(key.pair.wItem,i));
				*pRepeatNode=*pParaNode;
				pRepeatNode->keyLabel=KEY4C(key.pair.wItem,i);
				pRepeatNode->SetTagInfo(NULL);
				if(pTemplParaNode->scale_expression[0]=='\0'&&pParaNode->attach_type==CParaModelNode::SCALE)
					sprintf(pRepeatNode->scale_expression,"%d/%d",i,repeat_num);
				pParaNode=pRepeatNode;
				i++;
			}
			f3dPoint start,end,pos;
			if(pParaNode->attach_type==CParaModelNode::COORD_X_SPECIFIED)
			{	//杆件上X坐标值不变点
				CParaModelNode* pDependParaStartNode=listParaNodes.GetValue(pParaNode->attachNodeArr[0]);
				CParaModelNode* pDependParaEndNode=listParaNodes.GetValue(pParaNode->attachNodeArr[1]);
				if(pDependParaStartNode&&pDependParaStartNode->TagInfo()!=NULL)
					start=pDependParaStartNode->TagInfo()->pModelNode->Position(false);
				else
#ifdef	AFX_TARG_ENU_ENGLISH
					throw "Nodes of standard plane lack of reasonable relative node,failed to insert standard plane!";
#else
					throw "标准面中的节点缺少合理依附节点，插入标准面失败！";
#endif
				if(pDependParaEndNode&&pDependParaEndNode->TagInfo()!=NULL)
					end=pDependParaEndNode->TagInfo()->pModelNode->Position(false);
				else
#ifdef	AFX_TARG_ENU_ENGLISH
					throw "Nodes of standard plane lack of reasonable relative node,failed to insert standard plane!";
#else
					throw "标准面中的节点缺少合理依附节点，插入标准面失败！";
#endif
				pos.x=expression.SolveExpression(pParaNode->posx_expression);
				pos.y=start.y+(pos.x-start.x)*(end.y-start.y)/(end.x-start.x);
				pos.z=start.z+(pos.x-start.x)*(end.z-start.z)/(end.x-start.x);
				pNode=InsertNewNode(pBlockModel,pParaNode,pos);
				pNode->m_cPosCalType=1;	//X坐标不变点
				pNode->arrRelationNode[0]=pDependParaStartNode->TagInfo()->pModelNode->handle;
				pNode->arrRelationNode[1]=pDependParaEndNode->TagInfo()->pModelNode->handle;
			}
			else if(pParaNode->attach_type==CParaModelNode::COORD_Y_SPECIFIED)
			{	//杆件上Y坐标值不变点
				CParaModelNode* pDependParaStartNode=listParaNodes.GetValue(pParaNode->attachNodeArr[0]);
				CParaModelNode* pDependParaEndNode=listParaNodes.GetValue(pParaNode->attachNodeArr[1]);
				if(pDependParaStartNode&&pDependParaStartNode->TagInfo()!=NULL)
					start=pDependParaStartNode->TagInfo()->pModelNode->Position(false);
				else
#ifdef	AFX_TARG_ENU_ENGLISH
					throw "Nodes of standard plane lack of reasonable relative node,failed to insert standard plane!";
#else
					throw "标准面中的节点缺少合理依附节点，插入标准面失败！";
#endif
				if(pDependParaEndNode&&pDependParaEndNode->TagInfo()!=NULL)
					end=pDependParaEndNode->TagInfo()->pModelNode->Position(false);
				else
#ifdef	AFX_TARG_ENU_ENGLISH
					throw "Nodes of standard plane lack of reasonable relative node,failed to insert standard plane!";
#else
					throw "标准面中的节点缺少合理依附节点，插入标准面失败！";
#endif
				pos.y=expression.SolveExpression(pParaNode->posy_expression);
				pos.x=start.x+(pos.y-start.y)*(end.x-start.x)/(end.y-start.y);
				pos.z=start.z+(pos.y-start.y)*(end.z-start.z)/(end.y-start.y);
				pNode=InsertNewNode(pBlockModel,pParaNode,pos);
				pNode->m_cPosCalType=2;	//Y坐标不变点
				pNode->arrRelationNode[0]=pDependParaStartNode->TagInfo()->pModelNode->handle;
				pNode->arrRelationNode[1]=pDependParaEndNode->TagInfo()->pModelNode->handle;
			}
			else if(pParaNode->attach_type==CParaModelNode::COORD_Z_SPECIFIED)
			{	//杆件上Z坐标值不变点
				CParaModelNode* pDependParaStartNode=listParaNodes.GetValue(pParaNode->attachNodeArr[0]);
				CParaModelNode* pDependParaEndNode=listParaNodes.GetValue(pParaNode->attachNodeArr[1]);
				if(pDependParaStartNode&&pDependParaStartNode->TagInfo()!=NULL)
					start=pDependParaStartNode->TagInfo()->pModelNode->Position(false);
				else
#ifdef	AFX_TARG_ENU_ENGLISH
					throw "Nodes of standard plane lack of reasonable relative node,failed to insert standard plane!";
#else
					throw "标准面中的节点缺少合理依附节点，插入标准面失败！";
#endif
				if(pDependParaEndNode&&pDependParaEndNode->TagInfo()!=NULL)
					end=pDependParaEndNode->TagInfo()->pModelNode->Position(false);
				else
#ifdef	AFX_TARG_ENU_ENGLISH
					throw "Nodes of standard plane lack of reasonable relative node,failed to insert standard plane!";
#else
					throw "标准面中的节点缺少合理依附节点，插入标准面失败！";
#endif
				pos.z=expression.SolveExpression(pParaNode->posz_expression);
				pos.x=start.x+(pos.z-start.z)*(end.x-start.x)/(end.z-start.z);
				pos.y=start.y+(pos.z-start.z)*(end.y-start.y)/(end.z-start.z);
				pNode=InsertNewNode(pBlockModel,pParaNode,pos);
				pNode->m_cPosCalType=3;	//Z坐标不变点
				pNode->arrRelationNode[0]=pDependParaStartNode->TagInfo()->pModelNode->handle;
				pNode->arrRelationNode[1]=pDependParaEndNode->TagInfo()->pModelNode->handle;
			}
			else if(pParaNode->attach_type==CParaModelNode::SCALE)
			{	//比例等分点
				CParaModelNode* pDependParaStartNode=listParaNodes.GetValue(pParaNode->attachNodeArr[0]);
				CParaModelNode* pDependParaEndNode=listParaNodes.GetValue(pParaNode->attachNodeArr[1]);
				if(pDependParaStartNode&&pDependParaStartNode->TagInfo()!=NULL)
					start=pDependParaStartNode->TagInfo()->pModelNode->Position(false);
				else
#ifdef	AFX_TARG_ENU_ENGLISH
					throw "Nodes of standard plane lack of reasonable relative node,failed to insert standard plane!";
#else
					throw "标准面中的节点缺少合理依附节点，插入标准面失败！";
#endif
				if(pDependParaEndNode&&pDependParaEndNode->TagInfo()!=NULL)
					end=pDependParaEndNode->TagInfo()->pModelNode->Position(false);
				else
#ifdef	AFX_TARG_ENU_ENGLISH
					throw "Nodes of standard plane lack of reasonable relative node,failed to insert standard plane!";
#else
					throw "标准面中的节点缺少合理依附节点，插入标准面失败！";
#endif
				double scale=expression.SolveExpression(pParaNode->scale_expression);
				pos=start+(end-start)*scale;
				if(scale==0)
					pNode=pDependParaStartNode->TagInfo()->pModelNode;
				else if(scale==1)
					pNode=pDependParaEndNode->TagInfo()->pModelNode;
				else
				{
					pNode=InsertNewNode(pBlockModel,pParaNode,pos);
					pNode->m_cPosCalType=5;	//比例等分点
					pNode->attach_scale=scale;
					pNode->arrRelationNode[0]=pDependParaStartNode->TagInfo()->pModelNode->handle;
					pNode->arrRelationNode[1]=pDependParaEndNode->TagInfo()->pModelNode->handle;
				}
			}
			else if(pParaNode->attach_type==CParaModelNode::INTERSECTION)
			{
				CParaModelNode* pDependParaStartNode1=listParaNodes.GetValue(pParaNode->attachNodeArr[0]);
				CParaModelNode* pDependParaEndNode1=listParaNodes.GetValue(pParaNode->attachNodeArr[1]);
				CParaModelNode* pDependParaStartNode2=listParaNodes.GetValue(pParaNode->attachNodeArr[2]);
				CParaModelNode* pDependParaEndNode2=listParaNodes.GetValue(pParaNode->attachNodeArr[3]);
				if(pDependParaStartNode1&&pDependParaStartNode1->TagInfo()!=NULL)
					start=pDependParaStartNode1->TagInfo()->pModelNode->Position(false);
				else
#ifdef	AFX_TARG_ENU_ENGLISH
					throw "Nodes of standard plane lack of reasonable relative node,failed to insert standard plane!";
				if(pDependParaEndNode1&&pDependParaEndNode1->TagInfo()!=NULL)
					end=pDependParaEndNode1->TagInfo()->pModelNode->Position(false);
				else
					throw "Nodes of standard plane lack of reasonable relative node,failed to insert standard plane!";
				if(pDependParaStartNode2&&pDependParaStartNode2->TagInfo()!=NULL)
					start=pDependParaStartNode2->TagInfo()->pModelNode->Position(false);
				else
					throw "Nodes of standard plane lack of reasonable relative node,failed to insert standard plane!";
				if(pDependParaEndNode2&&pDependParaEndNode2->TagInfo()!=NULL)
					end=pDependParaEndNode2->TagInfo()->pModelNode->Position(false);
				else
					throw "Nodes of standard plane lack of reasonable relative node,failed to insert standard plane!";
#else
					throw "标准面中的节点缺少合理依附节点，插入标准面失败！";
				if(pDependParaEndNode1&&pDependParaEndNode1->TagInfo()!=NULL)
					end=pDependParaEndNode1->TagInfo()->pModelNode->Position(false);
				else
					throw "标准面中的节点缺少合理依附节点，插入标准面失败！";
				if(pDependParaStartNode2&&pDependParaStartNode2->TagInfo()!=NULL)
					start=pDependParaStartNode2->TagInfo()->pModelNode->Position(false);
				else
					throw "标准面中的节点缺少合理依附节点，插入标准面失败！";
				if(pDependParaEndNode2&&pDependParaEndNode2->TagInfo()!=NULL)
					end=pDependParaEndNode2->TagInfo()->pModelNode->Position(false);
				else
					throw "标准面中的节点缺少合理依附节点，插入标准面失败！";
#endif
				pNode=InsertNewNode(pBlockModel,pParaNode,pos);
				pNode->m_cPosCalType=4;	//两杆件交叉交点
				pNode->arrRelationNode[0]=pDependParaStartNode1->TagInfo()->pModelNode->handle;
				pNode->arrRelationNode[1]=pDependParaEndNode1->TagInfo()->pModelNode->handle;
				pNode->arrRelationNode[2]=pDependParaStartNode2->TagInfo()->pModelNode->handle;
				pNode->arrRelationNode[3]=pDependParaEndNode2->TagInfo()->pModelNode->handle;
			}
			else if(pParaNode->attach_type==CParaModelNode::COORD_OFFSET)
			{
				CParaModelNode* pDatumNode=listParaNodes.GetValue(pParaNode->attachNodeArr[0]);
				if(i-1>pParaNode->wiOffsetByPrevNode)
				{
					if(pPrevRepeatNode!=NULL)
						pDatumNode=pPrevRepeatNode;
				}
				CParaModelNode* pOffsetSNode=listParaNodes.GetValue(pParaNode->attachNodeArr[1]);
				CParaModelNode* pOffsetENod=listParaNodes.GetValue(pParaNode->attachNodeArr[2]);
				f3dPoint datumPt,vec;
				if(pDatumNode&&pDatumNode->TagInfo()!=NULL)
					datumPt=pDatumNode->TagInfo()->pModelNode->Position(false);
				else
#ifdef	AFX_TARG_ENU_ENGLISH
					throw "Nodes of standard plane lack of reasonable relative node,failed to insert standard plane!";
				if(pOffsetSNode&&pOffsetSNode->TagInfo()!=NULL)
					start=pOffsetSNode->TagInfo()->pModelNode->Position(false);
				else
					throw "Nodes of standard plane lack of reasonable relative node,failed to insert standard plane!";
				if(pOffsetENod&&pOffsetENod->TagInfo()!=NULL)
					end=pOffsetENod->TagInfo()->pModelNode->Position(false);
				else
					throw "Nodes of standard plane lack of reasonable relative node,failed to insert standard plane!";
#else
					throw "标准面中的节点缺少合理依附节点，插入标准面失败！";
				if(pOffsetSNode&&pOffsetSNode->TagInfo()!=NULL)
					start=pOffsetSNode->TagInfo()->pModelNode->Position(false);
				else
					throw "标准面中的节点缺少合理依附节点，插入标准面失败！";
				if(pOffsetENod&&pOffsetENod->TagInfo()!=NULL)
					end=pOffsetENod->TagInfo()->pModelNode->Position(false);
				else
					throw "标准面中的节点缺少合理依附节点，插入标准面失败！";
#endif
				double attach_offset=expression.SolveExpression(pParaNode->offset_expression);
				Sub_Pnt(vec,end,start);
				normalize(vec);
				if(pParaNode->ciOffsetType=='X')
					pos=datumPt+vec*(attach_offset/fabs(vec.x));
				else if(pParaNode->ciOffsetType=='Y')
					pos=datumPt+vec*(attach_offset/fabs(vec.y));
				else if(pParaNode->ciOffsetType=='Z')
					pos=datumPt+vec*(attach_offset/fabs(vec.z));
				else
					pos= datumPt+vec*attach_offset;
				pNode=InsertNewNode(pBlockModel,pParaNode,pos);
				pNode->m_cPosCalType=7;		//沿杆件自某节点的偏移点
				pNode->m_cPosCalViceType=pParaNode->ciOffsetType;//
				pNode->attach_offset=attach_offset;
				pNode->arrRelationNode[0]=pDatumNode->TagInfo()->pModelNode->handle;
				pNode->arrRelationNode[1]=pOffsetSNode->TagInfo()->pModelNode->handle;
				pNode->arrRelationNode[2]=pOffsetENod->TagInfo()->pModelNode->handle;
			}
			else //if(pParaNode->attach_type==CParaModelNode::INDEPENDENT)
			{	
				if(pParaNode->TagInfo()!=NULL)
				{	
					//取出关键节点，重新添加标签
					CLDSNode* pNode=(CLDSNode*)pParaNode->TagInfo();
					pParaNode->SetTagInfo(NULL);
					LoadTagInfo(pParaNode)->pModelNode=pNode;
				}
				else
				{
					pos.x=expression.SolveExpression(pParaNode->posx_expression);
					pos.y=expression.SolveExpression(pParaNode->posy_expression);
					pos.z=expression.SolveExpression(pParaNode->posz_expression);
					pNode=InsertNewNode(pBlockModel,pParaNode,pos);
					pNode->m_cPosCalType=0;	//无坐标依赖项
				}
			}
		}while(i<repeat_num);
	}
	//设定重复节点的首末节点影射
	for(pParaNode=listParaNodes.GetFirst();pParaNode;pParaNode=listParaNodes.GetNext())
	{
		int repeat_num=0;	//默认不重复
		KEY4C key=KEY4C(pParaNode->keyLabel);
		if(pParaNode->keyRepeatParam.dwItem>0)	//重复插入循环节点
		{
			pItem=pBlockModel->listParams.GetValue(pParaNode->keyRepeatParam);
			if(pItem)
				repeat_num=pItem->value.iType;
		}
		if( repeat_num<=1 || 
			(pParaNode->attach_type!=CParaModelNode::SCALE&&pParaNode->attach_type!=CParaModelNode::COORD_OFFSET))
			continue;
		BOOL pushed=listParaNodes.push_stack();
		CParaModelNode* pDependencyStart=listParaNodes.GetValue(pParaNode->attachNodeArr[0]);
		CParaModelNode* pDependencyEnd=listParaNodes.GetValue(pParaNode->attachNodeArr[1]);
		if(pParaNode->attach_type==CParaModelNode::COORD_OFFSET)
			pDependencyEnd=listParaNodes.GetValue(pParaNode->attachNodeArr[2]);
		if(pDependencyStart!=NULL)
		{
			pRepeatNode=listRepeatedParaNodes.Add(pParaNode->keyLabel.ClonePrior(0));
			LoadTagInfo(pRepeatNode)->pModelNode=pDependencyStart->TagInfo()->pModelNode;
		}
		if(pDependencyEnd!=NULL)
		{
			pRepeatNode=listRepeatedParaNodes.Add(pParaNode->keyLabel.ClonePrior(repeat_num));
			LoadTagInfo(pRepeatNode)->pModelNode=pDependencyEnd->TagInfo()->pModelNode;
		}
		listParaNodes.pop_stack(pushed);
	}
	for(pRepeatNode=listRepeatedParaNodes.GetFirst();pRepeatNode;pRepeatNode=listRepeatedParaNodes.GetNext())
		listParaNodes.SetValue(pRepeatNode->keyLabel,*pRepeatNode);

	//2 杆件插入操作
	CLDSLinePart *pPole=NULL;
	CParaModelRod *pParaRod=NULL,*pRepeatRod=NULL;
	////插入主控杆件
	//for(CLDSPart* pPart=pBlockModel->keyPartSet.GetFirst();pPart;pPart=pBlockModel->keyPartSet.GetNext())
	//{	
	//	CLDSLinePart* pNewPart=(CLDSLinePart*)pBlockModel->AppendPart(pPart->GetClassTypeId(),FALSE);
	//	pNewPart->handle=pPart->handle;
	//	pNewPart->CopyProperty(pPart);
	//	pNewPart->pStart
	//}
	//插入新生成杆件
	for(pParaRod=listParaRods.GetFirst();pParaRod;pParaRod=listParaRods.GetNext())
	{
		BOOL bRepeatPole=FALSE;
		CParaModelNode* pParaStartNode=listParaNodes.GetValue(pParaRod->keyStartNode);
		CParaModelNode* pParaEndNode=listParaNodes.GetValue(pParaRod->keyEndNode);
		int repeat_num1=0,repeat_num2=0;
		if(pParaStartNode->keyRepeatParam.dwItem>0||pParaEndNode->keyRepeatParam.dwItem>0)
		{
			pItem=pBlockModel->listParams.GetValue(pParaStartNode->keyRepeatParam);
			repeat_num1=pItem->value.iType;
			pItem=pBlockModel->listParams.GetValue(pParaEndNode->keyRepeatParam);
			repeat_num2=pItem->value.iType;
			bRepeatPole=TRUE;
		}
		if(bRepeatPole)		//重复杆件
		{
			int index=1;
			int repeat_num=(repeat_num1<repeat_num2)?repeat_num1:repeat_num2;
			CParaModelNode* pStartNode=NULL,*pEndNode=NULL;
			for(int i=0;i<=repeat_num;i++)
			{
				if(pIndexVar==NULL)
					break;	//缺少索引变量参数
				pIndexVar->fValue=i;
				BOOL bInsertPole=expression.SolveLogicalExpression(pParaRod->exist_logicexpr);
				int indexStart=ftoi(expression.SolveExpression(pParaRod->indexParaStart));
				int indexEnd=ftoi(expression.SolveExpression(pParaRod->indexParaEnd));
				if(bInsertPole)
				{
					KEY4C keyStart=pParaRod->keyStartNode.ClonePrior(indexStart);
					KEY4C keyEnd  =pParaRod->keyEndNode.ClonePrior(indexEnd);
					pStartNode=listParaNodes.GetValue(keyStart);
					pEndNode=listParaNodes.GetValue(keyEnd);
					if(pStartNode==NULL || pEndNode==NULL)
						continue;
					pRepeatRod=listRepeatedParaRods.Add(pParaRod->keyLabel.ClonePrior(i));
					*pRepeatRod=*pParaRod;
					pRepeatRod->keyStartNode=pStartNode->keyLabel;
					pRepeatRod->keyEndNode=pEndNode->keyLabel;
					InsertNewPole(pBlockModel,pRepeatRod);
				}
			}
		}
		else	//非重复杆件
			InsertNewPole(pBlockModel,pParaRod);
	}
	for(pRepeatRod=listRepeatedParaRods.GetFirst();pRepeatRod;pRepeatRod=listRepeatedParaRods.GetNext())
	{
		pParaRod=listParaRods.Add(pRepeatRod->keyLabel,pRepeatRod->m_idPartClsType,FALSE);
		*pParaRod=*pRepeatRod;
	}
	for(pParaRod=listParaRods.GetFirst();pParaRod;pParaRod=listParaRods.GetNext())
	{
		if(pParaRod->TagInfo()->pModelRod->GetClassTypeId()!=CLS_LINETUBE)
			continue;
		CLDSLineTube* pTube=(CLDSLineTube*)pParaRod->TagInfo()->pModelRod;
		if(pTube->hWorkNormRefPart==0&&pParaRod->keyRefPole!=0)
		{
			CParaModelRod* pKeyRefParaRod=listParaRods.GetValue(pParaRod->keyRefPole);
			if(pKeyRefParaRod&&pKeyRefParaRod->TagInfo()->pModelRod)
			{
				pTube->m_iNormCalStyle=1;
				pTube->hWorkNormRefPart=pKeyRefParaRod->TagInfo()->pModelRod->handle;
			}
		}
	}
	//3 节点杆件关系信息填写(节点父杆件信息及依附杆件信息)
	for(pNode=pBlockModel->Node.GetFirst();pNode;pNode=pBlockModel->Node.GetNext())
	{
		if(pNode->m_cPosCalType==4)	//交叉点
		{
			pNode->arrRelationPole[0]=pNode->arrRelationPole[1]=0;
			for(CLDSPart* pPart=pBlockModel->Parts.GetFirst();pPart;pPart=pBlockModel->Parts.GetNext())
			{
				if(!pPart->IsLinePart())
					continue;
				pPole=(CLDSLinePart*)pPart;
				if((pPole->pStart->handle==pNode->arrRelationNode[0]&&pPole->pEnd->handle==pNode->arrRelationNode[1])||
					(pPole->pStart->handle==pNode->arrRelationNode[1]&&pPole->pEnd->handle==pNode->arrRelationNode[0]))
					pNode->arrRelationPole[0]=pPole->handle;
				else if((pPole->pStart->handle==pNode->arrRelationNode[2]&&pPole->pEnd->handle==pNode->arrRelationNode[3])||
					(pPole->pStart->handle==pNode->arrRelationNode[3]&&pPole->pEnd->handle==pNode->arrRelationNode[2]))
					pNode->arrRelationPole[1]=pPole->handle;
				if(pNode->arrRelationPole[0]>0X20 && pNode->arrRelationPole[1]>0X20)
					break;
			}
		}
		else if(pNode->m_cPosCalType!=0)	//非无任何依赖节点
		{
			for(CLDSPart* pPart=pBlockModel->Parts.GetFirst();pPart;pPart=pBlockModel->Parts.GetNext())
			{
				if(!pPart->IsLinePart())
					continue;
				pPole=(CLDSLinePart*)pPart;
				if((pPole->pStart->handle==pNode->arrRelationNode[0]&&pPole->pEnd->handle==pNode->arrRelationNode[1])||
					(pPole->pStart->handle==pNode->arrRelationNode[1]&&pPole->pEnd->handle==pNode->arrRelationNode[0]))
				{
					pNode->arrRelationPole[0]=pPole->handle;
					break;
				}
			}
		}
		if(pNode->hFatherPart<0x20)
		{	//无父杆件信息
			CLDSLinePart *pViceFatherPole=NULL;
			for(CLDSPart* pPart=pBlockModel->Parts.GetFirst();pPart;pPart=pBlockModel->Parts.GetNext())
			{
				if(!pPart->IsLinePart())
					continue;
				pPole=(CLDSLinePart*)pPart;
				f3dLine line(pPole->pStart->Position(false),pPole->pEnd->Position(false));
				int ret=line.PtInLine(pNode->Position(false));
				if(ret==2)		//点在杆件直线内
				{
					pNode->hFatherPart=pPole->handle;
					pNode->layer(2)=pPole->layer(2);
					break;
				}
				else if(ret==1&&pViceFatherPole==NULL)	//准父杆件(节点为杆件端节点)
					pViceFatherPole=pPole;
			}
			if(pNode->hFatherPart<0x20 && pViceFatherPole)	//未找到父杆件，则以第一根准父杆件为父杆件
			{
				pNode->hFatherPart=pViceFatherPole->handle;
				pNode->layer(2)=pViceFatherPole->layer(2);
			}
		}
	}
	//4 计算杆件肢法线方向及位置
	for(pPole=pBlockModel->EnumRodFirst();pPole;pPole=pBlockModel->EnumRodNext())
	{
		if(pPole->GetClassTypeId()!=CLS_LINEANGLE)
			continue;
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pPole;
		if(pAngle->LayStyle==1&&pAngle->des_norm_x.hViceJg==0)
		{
			if(pAngle->pStart->hFatherPart!=pAngle->handle)
				pAngle->des_norm_x.hViceJg=pAngle->pStart->hFatherPart;
			else if(pAngle->pEnd->hFatherPart!=pAngle->handle)
				pAngle->des_norm_x.hViceJg=pAngle->pEnd->hFatherPart;
		}
		else if(pAngle->LayStyle==2&&pAngle->des_norm_y.hViceJg==0)
		{
			if(pAngle->pStart->hFatherPart!=pAngle->handle)
				pAngle->des_norm_y.hViceJg=pAngle->pStart->hFatherPart;
			else if(pAngle->pEnd->hFatherPart!=pAngle->handle)
				pAngle->des_norm_y.hViceJg=pAngle->pEnd->hFatherPart;
		}
		else if(pAngle->LayStyle==3&&pAngle->des_norm_x.hViceJg==0)
		{
			if(pAngle->pStart->hFatherPart!=pAngle->handle)
				pAngle->des_norm_x.hViceJg=pAngle->pStart->hFatherPart;
			else if(pAngle->pEnd->hFatherPart!=pAngle->handle)
				pAngle->des_norm_x.hViceJg=pAngle->pEnd->hFatherPart;
		}
		else if(pAngle->des_norm_y.hViceJg==0)
		{
			if(pAngle->pStart->hFatherPart!=pAngle->handle)
				pAngle->des_norm_y.hViceJg=pAngle->pStart->hFatherPart;
			else if(pAngle->pEnd->hFatherPart!=pAngle->handle)
				pAngle->des_norm_y.hViceJg=pAngle->pEnd->hFatherPart;
		}
		pAngle->cal_x_wing_norm();
		pAngle->cal_y_wing_norm();
		pAngle->CalPosition();
	}
	for(pParaRod=listParaRods.GetFirst();pParaRod;pParaRod=listParaRods.GetNext())
	{
		TAG_ROD_INFO* pTagInfo=pParaRod->TagInfo();
		if(pTagInfo==NULL || pTagInfo->pModelRod==NULL)
			continue;
		if(pParaRod->IsAngleObject()&&pTagInfo->pModelRod!=NULL)
		{
			CParaModelAngle* pParaAngle=(CParaModelAngle*)pParaRod;
			CLDSLineAngle* pCurJg=(CLDSLineAngle*)pTagInfo->pModelRod;
			AssignAnglePos(pCurJg->desStartPos,pParaAngle->desStart);
			AssignAnglePos(pCurJg->desEndPos,pParaAngle->desEnd);
		}
	}
	//2.5杆件信息（计算长度类型及受力类型等）填写
#ifdef __PART_DESIGN_INC_
	for(pParaRod=listParaRods.GetFirst();pParaRod;pParaRod=listParaRods.GetNext())
	{
		TAG_ROD_INFO* pTagInfo=pParaRod->TagInfo();
		if(pTagInfo==NULL || pTagInfo->pModelRod==NULL)
			continue;
		pPole=pTagInfo->pModelRod;
		pPole->CalLenCoef.iTypeNo=pParaRod->callen_type;
		if(pParaRod->start_force_type==0)
			pPole->start_force_type=CENTRIC_FORCE;
		else
			pPole->start_force_type=ECCENTRIC_FORCE;
		if(pParaRod->end_force_type==0)
			pPole->end_force_type=CENTRIC_FORCE;
		else
			pPole->end_force_type=ECCENTRIC_FORCE;
		CLDSLinePart *pTmPole=NULL;
		/*if(strlen(pParaRod->sLamdaRefPole)>0 &&
		poleSetTbl.GetValueAt(pParaRod->sLamdaRefPole,pTmPole))
		pPole->CalLenCoef.hHorizPole=pTmPole->handle;
		if(strlen(pParaRod->sLamdaRefPole)>0 && 
		poleSetTbl.GetValueAt(pParaRod->sLamdaRefPole,pTmPole))
		pPole->CalLenCoef.hRefPole=pTmPole->handle;*/
	}
#endif
	FillMirInfo(pBlockModel);
	//赋值节点的父杆件句柄
	for(pParaNode=listParaNodes.GetFirst();pParaNode;pParaNode=listParaNodes.GetNext())
	{
		pParaRod=listParaRods.GetValue(pParaNode->keyLabelFatherRod);
		if(pParaRod&&pParaNode->TagInfo()&&pParaRod->TagInfo())
			pParaNode->TagInfo()->pModelNode->hFatherPart=pParaRod->TagInfo()->pModelRod->handle;
	}
	//特定板的插入
	CParaModelParamPlate* pParaPlate=NULL;
	for(pParaPlate=listParaPlates.GetFirst();pParaPlate;pParaPlate=listParaPlates.GetNext())
	{
		CLDSParamPlate* pPlate=InsertNewPlate(pBlockModel,pParaPlate,expression);
		LoadTagInfo(pParaPlate)->pModelPlate=pPlate;
	}
}
void CParaBlockModelTemplate::ResetPosition(CParaBlockModel* pBlockModel)
{
	//TODO: 未实现

	;
}
bool CParaBlockModelTemplate::MechanicsDesign(CParaBlockModel* pBlockModel)
{
	//TODO: 未实现
	return false;
}
bool CParaBlockModelTemplate::StructuralDesign(CParaBlockModel* pBlockModel)
{
	//TODO: 未实现
	return false;
}

//////////////////////////////////////////////////////////////////////
// CParaBlockModel Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*CParaBlockModel::InitHumanPost();
CParaBlockModel::InitTriBeam();
CParaBlockModel::InitSquareBeam();*/

void LoadBlockDesignParamInfo(DESIGN_PARAM_ITEM *pItem,DWORD key)
{
	pItem->key=key;
}
CParaBlockModel::CParaBlockModel()
{
	m_pTemplate=NULL;
	listParams.LoadDefaultObjectInfo=LoadBlockDesignParamInfo;
}
CParaBlockModel::~CParaBlockModel()
{
	;
}
CLDSNode* CParaBlockModel::GetKeyNode(DWORD index)
{
	if(keyNodeSet.GetNodeNum()<=0)
		return NULL;
	return keyNodeSet[index-1];
}
DESIGN_PARAM_ITEM* CParaBlockModel::AddParaItem(DWORD key,bool glbalParams/*=false*/)
{
	if(glbalParams)
		listGlobalParams.AttachObject(key);
	return listParams.Add(key);
}
DESIGN_PARAM_ITEM* CParaBlockModel::GetParamItem(DWORD key)
{
	return listParams.GetValue(key);
}
CParaModelNode* CParaBlockModel::AddParaNode(DWORD key,bool glbalControlled/*=false*/)
{
	if(m_pTemplate==NULL)
		return NULL;
	listControlNodes.AttachObject(key);
	return m_pTemplate->listParaNodes.Add(key);
}
CParaModelRod* CParaBlockModel::AddParaRod(DWORD key,int idClsPartType/*=0*/)
{
	if(m_pTemplate==NULL)
		return NULL;
	return m_pTemplate->listParaRods.Add(key,idClsPartType);
}
CParaModelParamPlate* CParaBlockModel::AddParaPlate(DWORD key)
{
	if(m_pTemplate==NULL)
		return NULL;
	return m_pTemplate->listParaPlates.Add(key);
}
CParaModelNode* CParaBlockModel::GetParaNode(DWORD key)
{
	if(m_pTemplate==NULL)
		return NULL;
	return m_pTemplate->listParaNodes.GetValue(key);
}
CParaModelRod* CParaBlockModel::GetParaRod(DWORD key)
{
	if(m_pTemplate==NULL)
		return NULL;
	return m_pTemplate->listParaRods.GetValue(key);
}
CParaModelParamPlate* CParaBlockModel::GetParaPlate(DWORD key)
{
	if(m_pTemplate==NULL)
		return NULL;
	return m_pTemplate->listParaPlates.GetValue(key);
}
//#include "LogFile.h"
void CParaBlockModel::LogError(char* error)
{
	if(CLDSObject::log2file)
		CLDSObject::log2file->Log(error);
}
//From BlockTempl.cpp
void InitTemplatePara(IParaBlockContext* pContext, DWORD iTempType,DWORD templateIndex=0);
bool ParseTemplatePara(IParaBlockContext* pContext, DWORD iTempType,DWORD templateIndex=0);
bool NativeAssertParamsValid(IParaBlockContext* pContext, DWORD iTempType,DWORD templateIndex=0);
//当前将模版参数先固化在代码中，以后将参数导出到XML文档中
void CParaBlockModel::InitTemplateParams()
{
	InitTemplatePara(this,m_pTemplate->m_iTempType,m_pTemplate->m_iTempIndex);
}
void CParaBlockModel::RebuildModel()
{
	if(AssertParamsValid())
	{
		m_pTemplate->listParaNodes.Empty();
		m_pTemplate->listParaRods.Empty();
		m_pTemplate->listParaPlates.Empty();
		ParseTemplatePara(this,m_pTemplate->m_iTempType);
		m_pTemplate->RebuildModel(this);
	}
}
void CParaBlockModel::BuildAcs()
{
	if(m_pTemplate->m_iTempType==0)
	{	//变电构架
		DESIGN_PARAM_ITEM* pItem=GetParamItem(KEY4C("LEN"));
		DESIGN_PARAM_ITEM* pDlItem=GetParamItem(KEY4C("DL"));
		DESIGN_PARAM_ITEM* pLCBeamItem=GetParamItem(KEY4C("LCBL"));
		DESIGN_PARAM_ITEM* pLSBeamItem=GetParamItem(KEY4C("LSBL"));
		double fLen=0;
		if(pDlItem && pLCBeamItem && pLSBeamItem)
			fLen=pDlItem->value.fValue+pLCBeamItem->value.fValue+pLSBeamItem->value.fValue;
		else
			fLen=pItem->value.fValue*0.5;
		acs.origin.Set(fLen,0,0);
		acs.axis_x.Set(1,0,0);
		acs.axis_y.Set(0,1,0);
		acs.axis_z.Set(0,0,1);
	}
	else
	{

	}
}
ARRAY_LIST<double>* CParaBlockModel::AppendDoubleArray(DWORD key)
{
	ARRAY_LIST<double>* pArray=hashArrayParamValues.GetValue(key);
	if(pArray==NULL)
	{
		pArray=listArrayParamValues.append();
		hashArrayParamValues.SetValue(key,pArray);
	}
	return pArray;
}
bool CParaBlockModel::AssertParamsValid()
{
	//调用各模板中应支持的部件参数合法性校验
	if(m_pTemplate)
		return NativeAssertParamsValid(this,m_pTemplate->m_iTempType);
	else
#ifdef	AFX_TARG_ENU_ENGLISH
		LogError("Lack relative template object");
#else
		LogError("缺少关联模板对象");
#endif
	return false;
}
void CParaBlockModel::ResetPosition()
{
	if(m_pTemplate)
		m_pTemplate->ResetPosition(this);
}
bool CParaBlockModel::MechanicsDesign(CParaBlockModel* pBlockModel)
{
	if(m_pTemplate)
		return m_pTemplate->MechanicsDesign(this);
	else
		return false;
}
bool CParaBlockModel::StructuralDesign(CParaBlockModel* pBlockModel)
{
	if(m_pTemplate)
		return m_pTemplate->StructuralDesign(this);
	else
		return false;
}
