#if !defined(AFX_LAYANGLEBOLTDLG_H__BE061B03_6422_4F58_B3E5_AE196B27C900__INCLUDED_)
#define AFX_LAYANGLEBOLTDLG_H__BE061B03_6422_4F58_B3E5_AE196B27C900__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayAngleBoltDlg.h : header file
//
#include "CallBackDialog.h"
#include "XhListCtrl.h"
#include "Tower.h"
#include "supergridctrl.h"
/////////////////////////////////////////////////////////////////////////////
// CLayAngleBoltDlg dialog
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

class CLayAngleBoltDlg : public CCallBackDialog
{
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
	static int PREV_BERSPACE_MODE,PREV_WINGSPACE_MODE;
	long m_hCurEditOddPart;
// Construction
public:	//�����Ʋ���
	long m_hPlate;
	SEGI m_xiPreferSegI;
	CLDSLinePart *m_pLinePart;
	CLDSNode *m_pNode;
	char m_ciSectType;			//0.ָ���ڵ㴦;1.�˼�ʼ����;2.�˼��ն���
	f3dPoint viewNorm;
	GEPOINT m_xWingOffsetVec, m_xLineVec;
	BOOL m_bRayEndFaceOffset;	//���߲Ķ�ͷƽ�ƴ�����ʱ��׼����˨Ĭ��ҲӦΪƽ��
public:	//�б��������Լ���Ա����
	CSuperGridCtrl m_xListCtrlLsSet;
	BOLTSET *m_pLegacyLsList;
	ATOM_LIST<CDesignLsPara> *m_pLsList;
	CHashList<CLDSBolt>hashTempPrevViewBolts;
	CListCtrl* m_pLsSetList;
	void DisplayItemInfo(int iCurSel);
	int AppendRecord(CListCtrl *pListCtrl,int iItem,CStringArray &str_arr);
	void AppendRecord(CString*str_arr,void* pLsPara/*=NULL*/);
	void RefreshListViewHeader();
	void InitDesignLsPara(CDesignLsPara& ls_para);
	void LayoutBolts(BOOL bUpdateData=TRUE);
	void PreviewBolt(CDesignLsPara *pLsPara);
	void IntelliUpdateBoltSpace();
	void UpdatePlateSpace();
	BOOL IsOverlapLegacyLs(CDesignLsPara *pLsPara);
	void WorkPlaneSketch();
	void DelWorkPlaneSketch();
public:
	f3dPoint face_norm_offset;
	double m_fHuoquWingOffsetDist;
	BOOL m_bEndLjJg;		//�Ƿ�Ϊ�����ӽǸ�
	//���Ӱ���ڶ��������߽Ǹ�,��׼�Ǹֲ���˨ʱ
	//��Ҫ������׼�϶�Զ�������˨λ��(ͶӰ�����߽���)
	BOOL m_bEndLjDatumJg;			//�����ӻ�׼�Ǹ� wht 10-01-14
	CLDSLineAngle *m_pEndLjRayJg;	//���������߽Ǹ����������׼�Ǹ�����˨��ƫ��λ�� 
	BOOL m_bEnable2RowsLs;
	BOOL m_bIncPlateProfilePara;
	CDesignThickPara m_tBoltNormOffset;
	ANGLE_SPACE_FLAG cbSpaceFlag;
	CLayAngleBoltDlg(CWnd* pParent = NULL);   // standard constructor
	int m_iCurSelOddRefRod;

// Dialog Data
	//{{AFX_DATA(CLayAngleBoltDlg)
	enum { IDD = IDD_LAY_ANGLE_BOLT_DLG };
	BOOL	m_bMirLayLs;
	int		m_nCrossAngleG;
	int		m_nG;
	int		m_nG1;
	int		m_nG2;
	int		m_nLsEndSpace;
	int		m_nLsNum;
	int		m_nLsSpace;
	int		m_iLsLayoutStyle;
	int		m_iBoltNorm;
	int		m_iRayDirection;
	CString	m_hBaseNode;
	CString	m_sRayAngleSize;
	int		m_iDatumPointStyle;
	int		m_iOffsetWing;
	CString	m_hCrossAngle;
	double	m_fWingOffsetDist;
	double	m_fLenOffsetDist;
	int		m_iBerSpaceMode;
	int		m_iWingSpaceMode;
	BOOL	m_bTwoEdgeProfile;
	BOOL	m_bUseSdtEdgeSpace;
	double	m_fBerSpace;
	double	m_fWingSpace;
	CString	m_sBoltSize;
	double	m_fOddment;
	CString	m_sExtraOdd;
	int 	m_iOddCalStyle;
	double	m_fAngleEndNormOffset;
	double	m_fPlateEdgeSpace;
	int		m_iBoltRows;
	int		m_nFirstLsJgSpace;
	int		m_iType;
	CXhListCtrl m_listOddRefRods;
	int		m_iHoleFuncType;//�׹�������
	BOOL	m_bPunchHole;	//���
	BOOL	m_bDrillHole;	//���
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayAngleBoltDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLayAngleBoltDlg)
	afx_msg void OnBnLayoutBolts();
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchangeCmbOddCalStyle();
	afx_msg void OnSelchangeCmbDatumPointStyle();
	afx_msg void OnKeydownListLsSet(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeECrossAngle();
	afx_msg void OnChangeCmbBoltSize();
	afx_msg void OnBnModifyBoltItem();
	afx_msg void OnLsLayoutStyle();
	afx_msg void OnSelchangeCmbLsRows();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnRdoOffsetWing();
	afx_msg void OnChangeOddment();
	afx_msg void OnItemchangedListLsSet(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListLsSet(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCbnSelchangeBerSpaceMode();
	afx_msg void OnCbnSelchangeWingSpaceMode();
	afx_msg void OnBnSelCrossAngle();
	afx_msg void OnRdoBoltNorm();
	afx_msg void OnSelchangeCmbType();
	afx_msg void OnItemchangedListOddRefRods(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkListOddRefRods(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRclickListOddRefRods(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKeydownListOddRefRods(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnChangeELsNum();
	afx_msg void OnChangeWingSpace();
	afx_msg void OnChangeBerSpace();
	afx_msg void OnBnSelBaseNode();
	afx_msg void OnBnClickedChkStdEdgeSpace();
	afx_msg void OnClickedChkPunchHole();
	afx_msg void OnClickedChkDrillHole();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void ContextMenu(CPoint point);
	void OnAddItem();
	void OnModifyItem();
	void OnDeleteItem();
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYANGLEBOLTDLG_H__BE061B03_6422_4F58_B3E5_AE196B27C900__INCLUDED_)
