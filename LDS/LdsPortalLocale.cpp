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
	CLDSPart::CustomerSerial=uiCustomizeSerial;	//�ͻ����Ʊ�ʶ���
	if (uiCustomizeSerial == XHCID::CID_XiNanYuanPower)	//XHCID::CID_QingDao_DongFang=5
	{	//���ϵ������Ժ
		this->AddLocaleItemStr100("AppName", "���ϵ������Ժ���޹�˾������ά��ģ����");
		this->AddLocaleItemLong("M16.Color", RGB(255, 128, 64), "");	//ʮ�ֲ�弰�׽��麸������ͼ��������ͼ��
		this->AddLocaleItemLong("M24.Color", RGB(192, 192, 192), "");	//ʮ�ֲ�弰�׽��麸������ͼ��������ͼ��
		this->AddLocaleItemBool("UdfIntelliAngleJoint", true, "");	//�н�ͷ���ƹ���
	}
	else if (uiCustomizeSerial == XHCID::CID_AnHuiYuanPower)
	{	//����ʡ�������Ժ
		//this->AddLocaleItemStr100("AppName", "����ʡ�������Ժ������ά��ģ����");
		this->AddLocaleItemLong("M16.Color", RGB(255, 128, 64), "");	//ʮ�ֲ�弰�׽��麸������ͼ��������ͼ��
		this->AddLocaleItemLong("M24.Color", RGB(192, 192, 192), "");	//ʮ�ֲ�弰�׽��麸������ͼ��������ͼ��
		this->AddLocaleItemBool("ExportTurnLeftGimFile", true, "");
	}
}
