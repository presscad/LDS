#pragma once
#include "LinkLabel.h"

// CActiveHeightLegsDlg �Ի���

class CActiveHeightLegsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CActiveHeightLegsDlg)

public:
	CActiveHeightLegsDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CActiveHeightLegsDlg();
public:	//�����Ի���ǰ�ĺ����������
	short m_siInitLegHeight;	//A�����ȵ�����ڻ�׼���ߵĸ߲�(��λmm)��һ��Ϊ0�����ٴ���+1.5m���
	short m_siLegLevelDiff;		//���ڵȼ����ȵĸ߲�(��λmm, >0)
	DWORD m_dwIncLegItemFlag;	//��ǰ���߰����ļ��ȷ�Χ
public:	//�Ի���رպ�Ӵ����ϻ�ȡ���û��趨������ͬʱҲΪ���������
	char xarrActiveQuadLegs[4];	//1,2,3,4���޵ļ�����ȼ��ַ�

// �Ի�������
	enum { IDD = IDD_ACTIVE_HEIGHT_LEGS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnActivate();
public:
	virtual BOOL OnInitDialog();
public:
	CColorLabel m_ctrlMasterLegLabel;
	int m_iOrderOfLegQuadA2;
	int m_iOrderOfLegQuadB4;
	int m_iOrderOfLegQuadC3;
	int m_iOrderOfLegQuadD1;
	double m_fLegLevelDiff;	//���ڼ��ȸ߶ȼ���
	CString m_sHeightName;
	CString m_sLegItems;
	CString m_sLegLevelDiff;
	afx_msg void OnCbnSelchangeCmbLegQuadA2();
};
