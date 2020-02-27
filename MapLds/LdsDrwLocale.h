#pragma once
#include "XhLocale.h"

struct LDSDRW_LOCALE : public XHLOCALE{
	LDSDRW_LOCALE();
	bool bSplitAngleSketchDrawing;
public:
	virtual void InitCustomerSerial(UINT uiSerial);
};
extern LDSDRW_LOCALE gxDrwLocalizer;
