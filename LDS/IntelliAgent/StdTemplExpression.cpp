#include "stdafx.h"
#include "f_ent.h"

#ifdef __RAPID_ROUGH_3DMODEL_
#include "StdTemplExpression.h"
////////////////////////////////////////////////////////////////////////////////
class EXPRESSION_FUNC_NODE_X : public EXPRESSION_FUNC
{
	bool Calc(EXPRESSION_VALUE* pRsltValue,const char* param_str,CExpression* pContextExpr)
	{
		STDTEMPL_EXPRESSION* pStdTemplExpr=(STDTEMPL_EXPRESSION*)pContextExpr;
		WORD widNode=(WORD)pContextExpr->SolveExpression(param_str);
		STDTEMPL::MATCHNODE* pMatchNode=pStdTemplExpr->pStdTempl->hashMatchNodes.GetValue(widNode);
		if(pMatchNode==NULL)
			return false;
		if(pRsltValue)
		{
			pRsltValue->ciValType=0;
			pRsltValue->fValue=pMatchNode->pIntelliNode->xPosition.x;
		}
		return true;
	}
};
class EXPRESSION_FUNC_NODE_Y : public EXPRESSION_FUNC
{
	bool Calc(EXPRESSION_VALUE* pRsltValue,const char* param_str,CExpression* pContextExpr)
	{
		STDTEMPL_EXPRESSION* pStdTemplExpr=(STDTEMPL_EXPRESSION*)pContextExpr;
		WORD widNode=(WORD)pContextExpr->SolveExpression(param_str);
		STDTEMPL::MATCHNODE* pMatchNode=pStdTemplExpr->pStdTempl->hashMatchNodes.GetValue(widNode);
		if(pMatchNode==NULL)
			return false;
		if(pRsltValue)
		{
			pRsltValue->ciValType=0;
			pRsltValue->fValue=pMatchNode->pIntelliNode->xPosition.y;
		}
		return true;
	}
};
class EXPRESSION_FUNC_ABS : public EXPRESSION_FUNC
{
	bool Calc(EXPRESSION_VALUE* pRsltValue,const char* param_str,CExpression* pContextExpr)
	{
		double lfValue=pContextExpr->SolveExpression(param_str);
		pRsltValue->fValue=lfValue<0?-lfValue:lfValue;
		return true;
	}
};
static EXPRESSION_FUNC_NODE_X _locale_expr_funcobj_nodex;
static EXPRESSION_FUNC_NODE_Y _locale_expr_funcobj_nodey;
static EXPRESSION_FUNC_ABS	  _locale_expr_funcobj_abs;
STDTEMPL_EXPRESSION::STDTEMPL_EXPRESSION(STDTEMPL* pStdTempl/*=NULL*/)
{
	m_pContextStdTempl=pStdTempl;
}
EXPRESSION_FUNC* STDTEMPL_EXPRESSION::FromFunctionName(const char* func_label)
{
	if(stricmp(func_label,"NODE_X")==0)
		return &_locale_expr_funcobj_nodex;
	else if(stricmp(func_label,"NODE_Y")==0)
		return &_locale_expr_funcobj_nodey;
	else if(stricmp(func_label,"ABS")==0)
		return &_locale_expr_funcobj_abs;
	return NULL;
}
#include "../env_def.h"
#include ".\IntelliAgent.h"
void TestStdTemplExpression()
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
	//STDTEMPL_CMPL_EXPRESSION expression(&zb2templ);
	//expression.CompileExprStr("Abs(Node(3).x-Node(5).x)<100&&Abs(Node(9).x-Node(19).x)<100");
	//expression.CompileExprStr("Abs(Node#3.x-Node#5.x)<100&&Abs(Node#9.x-Node#19.x)<100");
	//expression.CompileExprStr("Abs(Node(3).x-Node(5).x)-100",0);
	//bool successed=expression.Calc();
	STDTEMPL_EXPRESSION expr(&zb2templ);
	double val=expr.SolveExpression("Abs(Node_x(3)-Node_x(5))-100");
	int wait=3;
}

#endif