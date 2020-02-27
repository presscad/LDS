#pragma once

#ifdef __RAPID_ROUGH_3DMODEL_
#include "XhCharString.h"
#include "Expression.h"
#include "IntelliModel.h"

class STDTEMPL_EXPRESSION : public CExpression
{
protected:
	virtual EXPRESSION_FUNC* FromFunctionName(const char* func_label);
	STDTEMPL* m_pContextStdTempl;
public:
	STDTEMPL_EXPRESSION(STDTEMPL* pStdTempl=NULL);
	STDTEMPL* get_pStdTempl(){return m_pContextStdTempl;}
	__declspec(property(get=get_pStdTempl)) STDTEMPL* pStdTempl;
};
#endif