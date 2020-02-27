#pragma once
#include "XhLocale.h"

struct LDSPORTAL_LOCALE : public XHLOCALE{
	LDSPORTAL_LOCALE();
public:
	virtual void InitCustomerSerial(UINT uiSerial);
};
extern LDSPORTAL_LOCALE gxPortalLocalizer;
