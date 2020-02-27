#pragma once

#include "supergridctrl.h"
#include "afxcmn.h"

// CAngleJointsDesignForm 对话框
#ifdef __PART_DESIGN_INC_
class CAngleJointsDesignForm : public CDialogEx
{
	DECLARE_DYNAMIC(CAngleJointsDesignForm)

	CSuperGridCtrl	m_listCtrlJoints;
public:
	CAngleJointsDesignForm(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAngleJointsDesignForm();

// 对话框数据
	enum { IDD = IDD_ANGLE_JOINTS_DESIGN_FORM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	void RefreshListJoints();
	void RefreshSpecJointItem(long iItem,ANGLE_JOINT* pAngleJoint);
public:
	CHashList<ANGLE_JOINT> hashAngleJoints;
	virtual BOOL OnInitDialog();
	void Destroy();
	void DisplayWindow();
	BOOL HideWindow();
	BOOL RelayoutWnd();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBnRedesignAll();
	afx_msg void OnBnClickedBnDesignSelectJoints();
	afx_msg void OnBnClickedBnDesignAllJoints();
};
#endif
