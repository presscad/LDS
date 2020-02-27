#pragma once
#include "env_def.h"
#include "supergridctrl.h"
#include "TowerTreeCtrl.h"
#include "DataCompare.h"

#if defined(__TMA_)||defined(__LMA_)
// CDataCompareDlg �Ի���
enum TREEITEMTYPE{	
	DRAWING_BOM_GROUP=1,	//�ⲿ����ͼֽBOM����
	DRAWING_BOM_FILE,		//�ⲿ����ͼֽBOM����
	CURRENT_MODEL,			//��ǰT/LMA��άģ����ȡ���ݽڵ�
	DATA_COMPARE,			//���ݱȶԽڵ�
};
class CDataCompareDlg : public CDialog
{
	DECLARE_DYNAMIC(CDataCompareDlg)
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
	ATOM_LIST<TREEITEM_INFO>itemInfoList;
	HTREEITEM m_hRootItem,m_hExterDataGroupItem,m_hModelItem;
public:
	CDataCompareDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDataCompareDlg();

// �Ի�������
	enum { IDD = IDD_DATA_CMP_DLG };
	int		m_nRecordNum;
	double	m_fTolerance,m_fWeightTolernace;
	CString m_sSeg;
	CSuperGridCtrl m_xListCtrl;
	CTowerTreeCtrl m_xTreeCtrl;
	CTabCtrl m_xTabCtrl;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMRClickTreeCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeContrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnImportExterBomFile();
	afx_msg void OnCompareData();
	afx_msg void OnExportCompResult();
	afx_msg void OnRetrieveData();
	afx_msg void OnDeleteItem();
	afx_msg void OnEnChangeESeg();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChangeCompareTolerance();
public:
	BOOL Create();
	void InitDataCmpDlg();
	void ContextMenu(CWnd *pWnd, CPoint point);
	void RefreshTreeCtrl();
	void RefreshListCtrl();
	TREEITEM_INFO* GetCurItem();
	afx_msg void OnBnClickedBtnDataCompare();
	afx_msg void OnBnClickedBtnImportData();
	afx_msg void OnBnClickedBtnExportResult();
	afx_msg void OnBnClickedBtnLoadModelData();
};
extern CDataCompareDlg *g_pDataCompareDlg;
#endif