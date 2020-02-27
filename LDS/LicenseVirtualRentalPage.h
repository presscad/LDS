#pragma once


// CLicenseVirtualRentalPage �Ի���
#include "afxdialogex.h"
#include "LinkLabel.h"
#define __WXPAY_RENTAL_ONLINE_
//#include "afxwin.h"
#include "PromptDlg.h"
#include "LicenseManager.h"
#include "PayQRCodeDlg.h"

class CLicenseVirtualRentalPage : public CDialogEx
{
	DECLARE_DYNAMIC(CLicenseVirtualRentalPage)
protected:
	CPayQRCodeDlg qrCodeDlg;
	//����֧�����
	int m_nTimer;
	int m_nTimerElapse;		//��λΪ����
	static const int MAX_WAIT_PAY_TIME	= 180000;	//������
	int m_nDeathLine;		//��λΪ����
	bool m_bPaySuccess;		//֧���ɹ�
	//
	CComboBox m_cmbRentalPeriod;
#ifdef __WXPAY_RENTAL_ONLINE_
	void StopListenPayResult();
	ATOM_LIST<CProductRental> rentalList;
public:
	ATOM_LIST<CProductRental>* GetRentalListPtr(){return &rentalList;}
	bool IsPaySuccess(){return m_bPaySuccess;}
	void StartListenPayResult();
	void ProcessPayResult(int nIdEvent);
	bool DownloadAndImportAuthFile(bool bPromptMsg=true);
	CProductRental* GetCurRentalPeriod();
	void InitProductRentalComboBox(CComboBox *pComboBox,ATOM_LIST<CProductRental> *pRentalList=NULL,int iCurSel=-1);
	bool TestDownloadAndImportAuthFile();
#endif
	static int TestLogin(bool bNeedInitAplInfo=false,bool bIncFuncUseFlag=false);
	bool ImportProductAuthorizeFile(const char* auth_file, bool bPromptMsg = true);
	int GetCurRentalPeriodIndex();

	DWORD m_dwLicenseApplyTimeStamp;
	CLinkLabel m_ctrlNoCloseMsg;
public:
	CLicenseVirtualRentalPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLicenseVirtualRentalPage();

// �Ի�������
	enum { IDD = IDD_LICENSE_VIRTUAL_RENTAL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnShiftToEndLicense();
	afx_msg void OnBnShiftToSrvLicense();
	afx_msg void OnBnCreateIdentityFile();
	afx_msg void OnBnImportAuthorizeFile();
	afx_msg void OnBnClickedBnApplyLicenseOnline();
public:
	int m_iRentalType;
	int m_iDogType;
	void UpdateBtnStateByDogType(int iDogType);
};
