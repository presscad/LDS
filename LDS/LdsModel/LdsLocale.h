#pragma once
#include "XhLocale.h"

struct LDS_LOCALE : public XHLOCALE{
	virtual void InitCustomerSerial(UINT uiSerial);
};
extern LDS_LOCALE gxLocalizer;
