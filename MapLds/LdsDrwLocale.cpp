#include "StdAfx.h"
#include "ClientId.h"
#include "lds_part.h"
#include ".\LdsDrwLocale.h"
#include ".\PlankDrawing.h"
#include ".\SysPara.h"

#if defined(_DEBUG)&&!defined(_DISABLE_DEBUG_NEW_)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////
LDSDRW_LOCALE gxDrwLocalizer;
LDSDRW_LOCALE::LDSDRW_LOCALE()
{
	bSplitAngleSketchDrawing=false;
}
void LDSDRW_LOCALE::InitCustomerSerial(UINT uiCustomizeSerial)
{
	XHLOCALE::InitCustomerSerial(uiCustomizeSerial);
	CLDSPart::CustomerSerial=uiCustomizeSerial;	//�ͻ����Ʊ�ʶ���
	CPlankDrawing::PREFER_STYLEFLAG.SetMaskBitCount(0);
	if(uiCustomizeSerial==XHCID::CID_QingDao_DongFang)	//XHCID::CID_QingDao_DongFang=5
	{	//�ൺ����
		this->AddLocaleItemStr100("ProcessCard.HasSymbol","��");
		this->AddLocaleItemStr100("WeldLabelPrefix", "Z");	//ʮ�ֲ�弰�׽��麸������ͼ��������ͼ��
		this->AddLocaleItemBool("CardCodeLabelAddSegI",true);
		this->AddLocaleItemBool("WeldMasterMap.DrawSectSymbol",true);	//ʮ�ֲ�弰�׽��麸������ͼ��������ͼ��
		this->AddLocaleItemBool("StdXPlates.blDimVertBoltSpace",true);	//��עʮ�ֲ��������˨���
		//�趨�麸��ϸ����ʽ
		sys.part_group_map.part_table.bDispPartGroupNo = true;	//�麸���
		sys.part_group_map.part_table.bDispPartNo = true;		//�麸�ӹ������
		sys.part_group_map.part_table.bDispPartGuiGe = true;	//�麸�ӹ������
		sys.part_group_map.part_table.bDispPartLen = false;		//�麸�ӹ�������
		sys.part_group_map.part_table.bDispPartNum = true;		//�麸�ӹ�������
		sys.part_group_map.part_table.bDispPartWeight = false;	//�麸�ӹ�������
		sys.part_group_map.part_table.bDispNotes = false;
		sys.part_group_map.part_table.bDispPartGroupNum = true;	//�麸������
		sys.part_group_map.part_table.bDispInternal = false;	//������
		sys.part_group_map.part_table.bDispAppearance = false;	//������
		sys.general_set.iMatCharPosType = 0;	//�ൺ�������տ���Ų���Ҫ��Ӳ��ʷ�
		this->AddLocaleItemStr100("WeldBOM.AddPartStampLabel", \
			"DF-%TowerType%-%PartLabel%-%material%-%PartNum%");	//����ӹ�����ӡ��
		sys.welding.bZoomAdaptiveWithScale=sys.part_group_map.part_table.bZoomInProcessCard=false;
		//�趨��˨��ע��ʽ
		this->AddLocaleItemBool("DimRayLsSpace", true);			//��ע��˨���
		this->AddLocaleItemBool("DimLsRayAngle", true);			//��ע��˨���߽Ƕ�
		//this->AddLocaleItemBool("DimSpaceToWeldEdge", true);	//��ע��˨�����ӱߵ���С����
		this->AddLocaleItemBool("DimSpaceToEdgeLine", false);	//��ע����������˨�������ߵľ���
		this->AddLocaleItemBool("DimFeatueLsSpace", false);		//��ע����������˨���
	}
	else if( uiCustomizeSerial==XHCID::CID_YunNan_DongDian||
		uiCustomizeSerial==XHCID::CID_YunNan_ZhanYe||
		uiCustomizeSerial==XHCID::CID_YunNan_JianYuan)
	{
		this->AddLocaleItemBool("WeldSubPartDrawWeldingSymbol",true);
	}
	else if( uiCustomizeSerial==XHCID::CID_ZheJiangShengDa)
	{	//�㽭ʢ��뽨��Ҫ��TMA����麸��ʱ�����˨����ע wjh-2020.1.8
		this->AddLocaleItemBool("DimRayLsSpace", true);			//��ע��˨���
	}
	else if(uiCustomizeSerial==2)
	{	//������ͨ���ӣ����������£�
		//�Ǹֹ��տ��нǸֲ�ͼ�ֳ�����ͼ������ͼ(�򲻳��ã�����������趨��ֱ��ͨ��Ȩ�޿���)
		bSplitAngleSketchDrawing=true;
		//�ֿ���ʾ�����麸ͼ������ͼ������ͼ(�ݲ���������趨��ֱ��ͨ��Ȩ�޿��ƣ�
		//��˨�׵����ӱߵ���С��ֱ�����ע
		CPlankDrawing::PREFER_STYLEFLAG.SetFlagDword(CPlankDrawing::DimBoltSpaceToWeldEdge);
		//����˨�׾��Ŀ׾���ע�������Ƿ�ͼ����ע��
		CPlankDrawing::PREFER_STYLEFLAG.SetFlagDword(CPlankDrawing::DimHoleDiameterAlways);
	}
	else if(uiCustomizeSerial==3)
	{	//�����߰���˵��Ҫ������ֶι�����ϸ��ʱ��������ʮ�ֲ���ϵĸ��������������ʾ��� wjh-2017.5.26
		sys.stru_map.bMergeAttachPlatesToBasePlateInBOM=FALSE;
	}
}
