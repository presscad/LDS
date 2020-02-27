#include "StdAfx.h"
#include "LdsLocale.h"
#include "ClientId.h"

LDS_LOCALE gxLocalizer;
void LDS_LOCALE::InitCustomerSerial(UINT uiSerial)
{
	XHLOCALE::InitCustomerSerial(uiSerial);
	if (uiSerial == XHCID::CID_GuangZhou_ZengLi)
	{	//广州增立
		this->AddLocaleItemBool("AdjustFrontView",TRUE);
	}
	else if (uiSerial == XHCID::CID_QingDao_HuiJinTong)
	{	//青岛汇金通
		this->AddLocaleItemBool("AdjustFrontView",TRUE);
	}
	else if (uiSerial == XHCID::CID_XiNanYuanPower)	//XHCID::CID_QingDao_DongFang=5
	{	//西南电力设计院
		this->AddLocaleItemStr100("M20Washer-ThickItems", "3,4");	//M20螺栓的可用垫圈厚度备选项
		this->AddLocaleItemLong("M20Washer-MaxCount", 2);			//M20螺栓的最大垫圈数量
		this->AddLocaleItemLong("2IntersBolt-MinAnlgeWidth", 180);	//交叉点布双螺栓的最小角钢肢宽
	}
}
