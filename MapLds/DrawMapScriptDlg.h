#if !defined(AFX_DRAWMAPSCRIPTDLG_H__1757B2B0_437C_4ED5_B48D_04FB29C14DE2__INCLUDED_)
#define AFX_DRAWMAPSCRIPTDLG_H__1757B2B0_437C_4ED5_B48D_04FB29C14DE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DrawMapScriptDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDrawMapScriptDlg dialog
#include "StdArx.h"
#include "resource.h"
#include "PropertyList.h"
#include "supergridctrl.h"
#include "Drawing.h"
#include "CADCallBackDlg.h"
#include "Drawing.h"
#include "XhListCtrl.h"

#ifdef __SMART_DRAW_INC_
class CDrawMapScriptDlg : public CCADCallBackDlg
{
// Construction
	int m_nMaxCmdDrawingSerialNo;
	BOOL m_bSuperListCtrlInUsing; //�жϵ�ǰListCtrl���� wht 12-02-22
	int m_iOperType;	//�������� 0.����(���ϵ�ֹͣ) 1.��һ�� 2.��һ�� 3.����˼����� 4.��Ӹ����� 5.���������
	void ExecutableCommand(int iOperType);	//ִ��ָ������ //3.����˼����� 4.��Ӹ����� 5.���������
	void DisplaySuperCtrlCmdList();
	void FinishSelectObjOper();				//���ѡ�����ĺ�������
	void EnableAllItem(BOOL bFlag);			//����\����������
	void EnableSelectedItem(BOOL bFlag);	//����\����ѡ����
	void DisplayCommonCtrlCmdList(int iGroup=0);//ʹ����ͨListView��ʾ�����б�
	int InsertCmdToListCtrl(int iGroup,CDrawingCommand *pCmd,int iInsertIndex);
public:
	CDrawMapScriptDlg(CWnd* pParent = NULL);   // standard constructor
	~CDrawMapScriptDlg();
	int GetOperType() const {return m_iOperType;}	//�õ���������
	CSuperGridCtrl::CTreeItem * GetFirstExecutableItem();	//�õ���һ����ִ�е�������
	CSuperGridCtrl::CTreeItem * GetLastExecutedItem();		//�õ����һ����ִ�еĵ�������
	int GetCurCmdGroup(){return m_tabCmdType.GetCurSel();}	//�õ���ǰ������
	void SelectEntObj(int nResultEnt=1);					//ѡ�����ڵ����
	void UpdateSpreadLinePos();				//����չ����λ��
	void RefreshCommandList(int iListGroup);//ˢ�������б�
	void UpdatePartSpecPropertyList(CPropTreeItem *pParentItem,CDrawingCommand *pCmd,
		BOOL bSonItem,BOOL bUpdate=FALSE);
	void UpdateRodBoltOrFlInfoPropList(CPropTreeItem *pParentItem,
		CDrawingCommand *pCmd,BOOL bUpdate=FALSE);
// Dialog Data
	//{{AFX_DATA(CDrawMapScriptDlg)
	enum { IDD = IDD_DRAW_MAP_SCRIPT_DLG };
	CTabCtrl	m_tabCmdType;
	CPropertyList	m_propList;
	CSuperGridCtrl	m_listCmdSuperCtrl;
	CXhListCtrl		m_listCmdCommonCtrl;
	CString	m_sScriptName;
	CString	m_sKeyWord;
	//}}AFX_DATA
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawMapScriptDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnOK();
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
public:
	CDrawing *m_pDrawing;				//��ǰͼֽ
	CDrawingComponent *m_pComponent;	//��ǰ��ͼ

	CHashList<CDrawingCommand*> hashRodBoltOrFlInfoList;	//�˼���˨������Ϣ��ע�����б�
	CHashList<CDrawingCommand*> hashPartSpecList;	//������ϸ�����б�
	CHashList<CDrawingCommand*> hashOddmentList;	//����ͷ�����б�
	CHashList<CDrawingCommand*> hashFootNailSpaceList;//�Ŷ���������б�
	CHashList<CDrawingCommand*> hashFillPlateSpaceList;//����������б�
	CDrawingCommand *m_pCurCmd;			//��ǰ����
	ATOM_LIST<CDrawingCommand*> cmdList;//��ǰѡ�����Ӧ�������б�
	CDrawingSpreadLine *m_pSpreadLine;	//��ǰչ����
	int m_iOldGroup;		//��¼ǰһ��������
	long m_idEventProp;		//��¼�����жϵ�����ID,�ָ�����ʱʹ��
	BOOL m_bCallByDrawingDlg;	//ͨ����ͼ�Ի������
	BOOL m_bPdmodeIs34;			//������չ����ʾģʽΪ34 wht 11-07-15
public:
	BOOL IsSpreadLinePropGroup(int iCurSel);
	void DisplayPropertyList(int iCurSel=0);
	void HideWnd(BOOL bRestart);	//���ش���֧����ĳЩ�����󵯳�
	//���Թ���
	static CStringKeyHashTable<long>propHashtable;	//�����ַ�����ϣ��
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	int GetPropValueStr(long id, char *valueStr);	//��������ID�õ�����ֵ
	void InitPropHashtable();						//��ʼ�����Թ�ϣ��
	int GetPropStr(long id, char *propStr);			//�õ������ַ���
	static long GetPropID(char* propStr,...);		//�õ�����ID
	int GetPropValueStr(CDrawingCommand *pCmd,long id,char *valueStr);//�õ�ָ������ָ��ID�������ַ���
	BOOL IsPropEqual(long idProp,int iSonCmdType=-1);//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDrawMapScriptDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSelchangeTabCmdType(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeKeyWord();
	afx_msg void OnEnableAllItem();
	afx_msg void OnDisableAllItem();
	afx_msg void OnEnableSelectedItem();
	afx_msg void OnDisableSelectedItem();
	afx_msg void OnItemchangedCmdListCommon(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkCmdListCommon(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickCmdListCommon(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMoveBefore();
	afx_msg void OnMoveAfter();
	afx_msg void OnAdjustDrawingSerialNo();
	afx_msg void OnAddItem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWMAPSCRIPTDLG_H__1757B2B0_437C_4ED5_B48D_04FB29C14DE2__INCLUDED_)
