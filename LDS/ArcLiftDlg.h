#pragma once
#include "afxwin.h"
#include "PropertyList.h"
#include "CallBackDialog.h"
// CArcLiftDlg �Ի���

#ifdef __ANGLE_PART_INC_
class CArcLiftDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CArcLiftDlg)
	CPropertyList m_propList;
public:
	CLDSArcLift m_xArcLift;
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
public:
	CArcLiftDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CArcLiftDlg();

	void DisplayArcLiftProperty();
	void FinishSelectObjOper();		//���ѡ�����ĺ�������
	void CopyCoordinate(long idProp);
	//���Թ���
	static CStringKeyHashTable<long>propHashtable;	//�����ַ�����ϣ��
	CString GetPropValueStr(long id);		//��������ID�õ�����ֵ
	void InitArcLiftPropHashtable();
	static long GetPropID(char* propStr);	//�õ�����ID
// �Ի�������
	enum { IDD = IDD_ARC_LIFT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnConfirm();
};
#endif