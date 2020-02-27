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
	char cMaterial;		//����
	CXhChar50 sPartNo;	//���
	double fLength;		//����
	double fOuterD;		//�⾶
	double fInnerD;		//�ھ�
	double fThick;		//���
	long hResidePlate;	//���ڹ��߸ְ�
	long hRelyBolt;		//������˨
	BOOL bRingPlate;	//�Ƿ������װ�
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
	//������
	DECLARE_PROP_FUNC(CBushTube);
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);
};
// DesignBushTubeDlg �Ի���
//////////////////////////////////////////////////////////////////////////
class CDesignBushTubeDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesignBushTubeDlg)
	//
	static bool m_bValidBushParam;
	static CBushTube m_xBushParam;
public:
	CDesignBushTubeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDesignBushTubeDlg();
public:
	CBushTube m_xBush;
// �Ի�������
	enum { IDD = IDD_DESIGN_BUSHTUBE_DLG };
	CPropertyList	m_propList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	void OnImportData();
public:
	void DisplayPropertyList();
	void FinishSelectObjOper();		//���ѡ�����ĺ�������
};
