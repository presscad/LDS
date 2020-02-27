#if defined(__BP_ONLY_)||defined(__CARD_IMG_VIEWER_)
#pragma once

#include "Resource.h"
#include "supergridctrl.h"
#include "BPSModel.h"
#include "ImageFile.h"
#include "ximajpg.h"
#include "ximapng.h"
#include "ServerTowerType.h"

// CUploadProcessCardDlg �Ի���

class CUploadProcessCardDlg : public CDialog
{
	DECLARE_DYNAMIC(CUploadProcessCardDlg)
	ARRAY_LIST<CAngleProcessInfo> m_xJgList;
	BITMAP image;	//�洢ʾ��ͼ
	CImageFile *m_pImageFile;
	CImageFileJpeg m_xJpegImage;
	CImageFilePng m_xPngImage;
	CServerManuTask *m_pTask;
	CServerTowerType *m_pTowerType;
	CImageFile *ReadImageFile(const char* image_file);
	void InitTaskTowerTypes();
public:
	CUploadProcessCardDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CUploadProcessCardDlg();

	void UpdateJgInfoList();
	void RefreshListCtrl();
	void RefreshPicture();
	void RefreshPrompt();
// �Ի�������
	enum { IDD = IDD_UPLOAD_PROCESS_CARD_DLG };
	CSuperGridCtrl m_xListCtrl;
	CComboBox m_cmbTask;
	CComboBox m_cmbTowerType;
	CString m_sCardPath;
	int m_nRecord;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeCmbTask();
	afx_msg void OnCbnSelchangeCmbTowerType();
};
#endif