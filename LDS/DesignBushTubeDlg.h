#pragma once
#include "resource.h"
#include "PropertyList.h"
#include "CallBackDialog.h"
#include "PropListItem.h"
#include "Markup.h"
//
class CBushTube
{
public:
	char cMaterial;		//材质
	CXhChar50 sPartNo;	//编号
	double fLength;		//长度
	double fOuterD;		//外径
	double fInnerD;		//内径
	double fThick;		//厚度
	long hResidePlate;	//所在挂线钢板
	long hRelyBolt;		//依赖螺栓
	BOOL bRingPlate;	//是否生成套板
	struct RING_PLATE
	{
		char cMaterial;
		CXhChar50 sPartNo;
		double fThick;
		double fInnerD;
		double fOuterD;
		RING_PLATE(){fThick=10;fInnerD=30;fOuterD=50;cMaterial=CSteelMatLibrary::Q235BriefMark();}
	}ring_plate;
public:
	CBushTube(){
		cMaterial=CSteelMatLibrary::Q235BriefMark();
		fLength=40;
		fOuterD=30;
		fInnerD=20;
		fThick=10;
		hResidePlate=hRelyBolt=0;
	}
	//
	void CloneProp(CBushTube* pSrcBush);
	bool ImportFootDataFromXML(CMarkup xml,const char* sElemName);
	BOOL ToXmlTemplFile();
	BOOL FromXmlTemplFile();
	//属性栏
	DECLARE_PROP_FUNC(CBushTube);
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);
};
// DesignBushTubeDlg 对话框
//////////////////////////////////////////////////////////////////////////
class CDesignBushTubeDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesignBushTubeDlg)
	//
	static bool m_bValidBushParam;
	static CBushTube m_xBushParam;
public:
	CDesignBushTubeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDesignBushTubeDlg();
public:
	CBushTube m_xBush;
// 对话框数据
	enum { IDD = IDD_DESIGN_BUSHTUBE_DLG };
	CPropertyList	m_propList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	void OnImportData();
public:
	void DisplayPropertyList();
	void FinishSelectObjOper();		//完成选择对象的后续操作
};
