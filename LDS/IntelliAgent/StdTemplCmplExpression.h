#pragma once
#include "HashTable.h"
#include "List.h"
#include "ArrayList.h"
#include "f_ent.h"
#include "XhCharString.h"
#include "ExpressionCmpl.h"
#include "IntelliModel.h"
#ifdef __RAPID_ROUGH_3DMODEL_
struct STDTEMPL_EXPRESSION_FUNC_NODE : public CMPL_EXPRESSION_FUNC
{
	STDTEMPL_EXPRESSION_FUNC_NODE();
	virtual long GetRsltValType(){return 400;}	//返回节点对象
	virtual bool Calc(EXPRESSION_VALUE* pRsltValue,IXhArray<CMPL_EXPRESSION_WORD>* pExprParams,CMPL_EXPRESSION* pExpr);	//运算
};
struct STDTEMPL_EXPRESSION_FUNC_ROD : public CMPL_EXPRESSION_FUNC
{
	STDTEMPL_EXPRESSION_FUNC_ROD();
	virtual long GetRsltValType(){return 401;}	//返回杆件对象
	virtual bool Calc(EXPRESSION_VALUE* pRsltValue,IXhArray<CMPL_EXPRESSION_WORD>* pExprParams,CMPL_EXPRESSION* pExpr);	//运算
};
class STDTEMPL_CMPL_EXPRESSION : public CMPL_EXPRESSION
{
public:
	struct CMPL_EXPRESSION_NODE : public CMPL_EXPRESSION_OBJ
	{
		CMPL_EXPRESSION_NODE();
		virtual int GetObjPropId(const char* szPropKey);
	};
	struct CMPL_EXPRESSION_ROD : public CMPL_EXPRESSION_OBJ
	{
		CMPL_EXPRESSION_ROD();
		virtual int GetObjPropId(const char* szPropKey);
	};
protected:
	STDTEMPL* m_pContextStdTempl;
	CMPL_EXPRESSION_OBJPTR presetObjTypes[2];
public:
	static const UINT MIN_LOCALE_FUNCID	= 1001;
	static const UINT LOCALEFUNC_NODE	= 1001;	//MIN_LOCALE_FUNCID+0
	static const UINT LOCALEFUNC_ROD	= 1002;	//MIN_LOCALE_FUNCID+1
public:
	STDTEMPL_CMPL_EXPRESSION(STDTEMPL* pStdTempl=NULL);
	STDTEMPL* get_pStdTempl(){return m_pContextStdTempl;}
	__declspec(property(get=get_pStdTempl)) STDTEMPL* pStdTempl;
	void InitLocaleFunctions();
	STDTEMPL* InitStdTemplContainer(STDTEMPL* pStdTempl){m_pContextStdTempl=pStdTempl;}
	virtual CMPL_EXPRESSION_OBJ* FromExpressionObj(int idObjType);
	virtual UINT FromFunctionName(const char* func_label);
	virtual CMPL_EXPRESSION_FUNC* FromFunction(UINT idFunc);
	virtual bool GetExprObjProperty(void* pExprObj,int iExprObjType,int idObjProp,EXPRESSION_VALUE* pRsltValue);
};
#endif