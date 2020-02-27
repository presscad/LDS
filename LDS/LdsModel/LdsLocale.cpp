#include "StdAfx.h"
#include "LdsLocale.h"
#include "ClientId.h"

LDS_LOCALE gxLocalizer;
void LDS_LOCALE::InitCustomerSerial(UINT uiSerial)
{
	XHLOCALE::InitCustomerSerial(uiSerial);
	if (uiSerial == XHCID::CID_GuangZhou_ZengLi)
	{	//��������
		this->AddLocaleItemBool("AdjustFrontView",TRUE);
	}
	else if (uiSerial == XHCID::CID_QingDao_HuiJinTong)
	{	//�ൺ���ͨ
		this->AddLocaleItemBool("AdjustFrontView",TRUE);
	}
	else if (uiSerial == XHCID::CID_XiNanYuanPower)	//XHCID::CID_QingDao_DongFang=5
	{	//���ϵ������Ժ
		this->AddLocaleItemStr100("M20Washer-ThickItems", "3,4");	//M20��˨�Ŀ��õ�Ȧ��ȱ�ѡ��
		this->AddLocaleItemLong("M20Washer-MaxCount", 2);			//M20��˨������Ȧ����
		this->AddLocaleItemLong("2IntersBolt-MinAnlgeWidth", 180);	//����㲼˫��˨����С�Ǹ�֫��
	}
}
