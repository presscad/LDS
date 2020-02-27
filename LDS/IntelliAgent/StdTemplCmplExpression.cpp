#include "stdafx.h"
#include "f_ent.h"
#include "XhCharString.h"

#ifdef __RAPID_ROUGH_3DMODEL_
#include "StdTemplCmplExpression.h"
////////////////////////////////////////////////////////////////////////////////
STDTEMPL_EXPRESSION_FUNC_NODE::STDTEMPL_EXPRESSION_FUNC_NODE()
{
	id=STDTEMPL_CMPL_EXPRESSION::LOCALEFUNC_NODE;
	memcpy(name,"Node",4);
}
bool STDTEMPL_EXPRESSION_FUNC_NODE::Calc(EXPRESSION_VALUE* pRsltValue,IXhArray<CMPL_EXPRESSION_WORD>* pExprParams,CMPL_EXPRESSION* pExpr)	//ÔËËã
{
	STDTEMPL_CMPL_EXPRESSION* pStdTemplExpr=(STDTEMPL_CMPL_EXPRESSION*)pExpr;
	CMPL_EXPRESSION_WORD& exprword=pExprParams->At(0);
	STDTEMPL::MATCHNODE* pMatchNode=pStdTemplExpr->pStdTempl->hashMatchNodes.GetValue(exprword.value.liValue);
	pRsltValue->ciValType=3;
	pRsltValue->pObj=pMatchNode;
	return pMatchNode!=NULL;
}
STDTEMPL_EXPRESSION_FUNC_ROD::STDTEMPL_EXPRESSION_FUNC_ROD()
{
	id=STDTEMPL_CMPL_EXPRESSION::LOCALEFUNC_ROD;
	memcpy(name,"Rod",3);
}
bool STDTEMPL_EXPRESSION_FUNC_ROD::Calc(EXPRESSION_VALUE* pRsltValue,IXhArray<CMPL_EXPRESSION_WORD>* pExprParams,CMPL_EXPRESSION* pExpr)	//ÔËËã
{
	STDTEMPL_CMPL_EXPRESSION* pStdTemplExpr=(STDTEMPL_CMPL_EXPRESSION*)pExpr;
	CMPL_EXPRESSION_WORD& exprword=pExprParams->At(0);
	STDTEMPL::MATCHROD* pMatchRod=pStdTemplExpr->pStdTempl->hashMatchRods.GetValue(exprword.value.liValue);
	pRsltValue->ciValType=3;
	pRsltValue->pObj=pMatchRod;
	return pMatchRod!=NULL;
}
static STDTEMPL_EXPRESSION_FUNC_NODE _locale_expr_func_node;
static STDTEMPL_EXPRESSION_FUNC_ROD  _locale_expr_func_rod;
static  STDTEMPL_CMPL_EXPRESSION::CMPL_EXPRESSION_NODE _locale_expr_object_node;
static STDTEMPL_CMPL_EXPRESSION::CMPL_EXPRESSION_ROD  _locale_expr_object_rod;
//#include "ExpressionCmplSysFunc.h"
STDTEMPL_CMPL_EXPRESSION::CMPL_EXPRESSION_NODE::CMPL_EXPRESSION_NODE()
{
	id=0;
}
int STDTEMPL_CMPL_EXPRESSION::CMPL_EXPRESSION_NODE::GetObjPropId(const char* szPropKey)
{
	if(*szPropKey=='x'||*szPropKey=='X')
		return 1;
	else if(*szPropKey=='y'||*szPropKey=='Y')
		return 2;
	else if(*szPropKey=='z'||*szPropKey=='Z')
		return 3;
	else
		return 0;
}
STDTEMPL_CMPL_EXPRESSION::CMPL_EXPRESSION_ROD::CMPL_EXPRESSION_ROD()
{
	id=1;
}
int STDTEMPL_CMPL_EXPRESSION::CMPL_EXPRESSION_ROD::GetObjPropId(const char* szPropKey)
{
	return 0;
}
STDTEMPL_CMPL_EXPRESSION::STDTEMPL_CMPL_EXPRESSION(STDTEMPL* pStdTempl/*=NULL*/)
{
	m_pContextStdTempl=pStdTempl;
	InitLocaleFunctions();
	presetObjTypes[0]=&_locale_expr_object_node;
	presetObjTypes[1]=&_locale_expr_object_rod;
}
void STDTEMPL_CMPL_EXPRESSION::InitLocaleFunctions()
{
	localefunctions.functions.SetSize(3);//[];
	localefunctions.functions[LOCALEFUNC_NODE-MIN_LOCALE_FUNCID]=&_locale_expr_func_node;
	localefunctions.functions[LOCALEFUNC_ROD -MIN_LOCALE_FUNCID]=&_locale_expr_func_rod;
	localefunctions.hashFuncsByLabel.SetValue("Node",LOCALEFUNC_NODE);
	localefunctions.hashFuncsByLabel.SetValue("Rod",LOCALEFUNC_ROD);
}
CMPL_EXPRESSION_OBJ* STDTEMPL_CMPL_EXPRESSION::FromExpressionObj(int idObjType)
{
	if(idObjType<2)
		return presetObjTypes[idObjType];
	else
		return NULL;
}
UINT STDTEMPL_CMPL_EXPRESSION::FromFunctionName(const char* func_label)
{
	CMPL_EXPRESSION_FUNC* pSysFunc=FromSysFunction(func_label);
	if(pSysFunc!=NULL)
		return pSysFunc->Id;
	UINT *pidFunc=localefunctions.hashFuncsByLabel.GetValue(func_label);
	if(pidFunc!=NULL)
		return (pidFunc!=NULL)?*pidFunc:0;
	else
		return CMPL_EXPRESSION::FromFunctionName(func_label);
}
CMPL_EXPRESSION_FUNC* STDTEMPL_CMPL_EXPRESSION::FromFunction(UINT idFunc)
{
	CMPL_EXPRESSION_FUNC* pSysFunc=FromSysFunction(idFunc);
	if(pSysFunc!=NULL)
		return pSysFunc;
	else
		return localefunctions.functions[idFunc-MIN_LOCALE_FUNCID];
}
bool STDTEMPL_CMPL_EXPRESSION::GetExprObjProperty(void* pExprObj,int iExprObjType,int idObjProp,EXPRESSION_VALUE* pRsltValue)
{
	EXPRESSION_VALUE value;
	if(iExprObjType==1)		//STDTEMPL::MATCHNODE
	{
		STDTEMPL::MATCHNODE* pMatchNode=(STDTEMPL::MATCHNODE*)pExprObj;
		if(iExprObjType==1)
		{
			value.fValue=pMatchNode->pIntelliNode->xPosition.x;
			value.ciValType=0;
		}
		else if(iExprObjType==2)
		{
			value.fValue=pMatchNode->pIntelliNode->xPosition.y;
			value.ciValType=0;
		}
		else if(iExprObjType==3)
		{
			value.fValue=pMatchNode->pIntelliNode->xPosition.z;
			value.ciValType=0;
		}
		else
			return false;
	}
	else
		return false;
	if(pRsltValue)
		*pRsltValue=value;
	return true;
}
#include "../env_def.h"
#include ".\IntelliAgent.h"
#include "StdTemplExpression.h"
void TestStdTemplCmplExpression()
{
	STDTEMPL zb2templ;
	STDTEMPL::MATCHNODE* pMatchNode=zb2templ.hashMatchNodes.Add(3);
	CIntelliAgent experts(&Ta);
	pMatchNode->pIntelliNode=experts.FromNodeHandle(0x53);
	pMatchNode=zb2templ.hashMatchNodes.Add(5);
	pMatchNode->pIntelliNode=experts.FromNodeHandle(0x2c);
	pMatchNode=zb2templ.hashMatchNodes.Add(9);
	pMatchNode->pIntelliNode=experts.FromNodeHandle(0x4f);
	pMatchNode=zb2templ.hashMatchNodes.Add(19);
	pMatchNode->pIntelliNode=experts.FromNodeHandle(0x4b);
	STDTEMPL_CMPL_EXPRESSION expression(&zb2templ);
	//expression.CompileExprStr("Abs(Node(3).x-Node(5).x)<100&&Abs(Node(9).x-Node(19).x)<100");
	//expression.CompileExprStr("Abs(Node#3.x-Node#5.x)<100&&Abs(Node#9.x-Node#19.x)<100");
	//expression.CompileExprStr("Abs(Node(3).x-Node(5).x)-100",0);
	//bool successed=expression.Calc();
	STDTEMPL_EXPRESSION expr(&zb2templ);
	double val=expr.SolveExpression("Abs(Node_x(3)-Node_x(5))-100");
	int wait=3;
}

#endif