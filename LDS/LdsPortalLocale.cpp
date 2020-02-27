#include "StdAfx.h"
#include "ClientId.h"
#include "lds_part.h"
#include ".\LdsPortalLocale.h"

LDSPORTAL_LOCALE gxPortalLocalizer;
LDSPORTAL_LOCALE::LDSPORTAL_LOCALE()
{
	;
}
void LDSPORTAL_LOCALE::InitCustomerSerial(UINT uiCustomizeSerial)
{
	XHLOCALE::InitCustomerSerial(uiCustomizeSerial);
	CLDSPart::CustomerSerial=uiCustomizeSerial;	//客户定制标识序号
	if (uiCustomizeSerial == XHCID::CID_XiNanYuanPower)	//XHCID::CID_QingDao_DongFang=5
	{	//西南电力设计院
		this->AddLocaleItemStr100("AppName", "西南电力设计院有限公司铁塔三维建模程序");
		this->AddLocaleItemLong("M16.Color", RGB(255, 128, 64), "");	//十字插板及底脚组焊中主视图绘制剖视图符
		this->AddLocaleItemLong("M24.Color", RGB(192, 192, 192), "");	//十字插板及底脚组焊中主视图绘制剖视图符
		this->AddLocaleItemBool("UdfIntelliAngleJoint", true, "");	//有接头定制规则
	}
	else if (uiCustomizeSerial == XHCID::CID_AnHuiYuanPower)
	{	//安徽省电力设计院
		//this->AddLocaleItemStr100("AppName", "安徽省电力设计院铁塔三维建模程序");
		this->AddLocaleItemLong("M16.Color", RGB(255, 128, 64), "");	//十字插板及底脚组焊中主视图绘制剖视图符
		this->AddLocaleItemLong("M24.Color", RGB(192, 192, 192), "");	//十字插板及底脚组焊中主视图绘制剖视图符
		this->AddLocaleItemBool("ExportTurnLeftGimFile", true, "");
	}
}
