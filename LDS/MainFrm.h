// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__7DA70090_573A_40CE_96C7_DCE49CAD5E31__INCLUDED_)
#define AFX_MAINFRM_H__7DA70090_573A_40CE_96C7_DCE49CAD5E31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "sizecbar.h"
//#include "AutoHideBar.h"
#include "MouseStatusBar.h"
#include "CirStack.h"
#include "LDSView.h"
#include "TowerTreeDlg.h"
#include "TowerPropertyDlg.h"
#include "TowerPartGroupDlg.h"
#include "CmdLineDlg.h"
#include "PartNoListDlg.h"
#include "OutputInfoDlg.h"
#include "LogViewerDlg.h"
#include "DialogPanel.h"
#include "LDSMFCToolBar.h"
#include "LDSMFCMenuBar.h"
#include "DataCheckDlg.h"
#ifdef __CONNECT_REMOTE_SERVER_
#include "RemoteServerDlg.h"
#endif
class CMD_GROUP{
public:
	BYTE serial;	//命令分组标识号
	int nMaxWarnings;
	BOOL bOutputToNotepad;	//输出到记事本
	double tolerance;		//误差(deviation)允许范围
	CHashList<bool> cmdItems;//值为默认激活状态
	CMD_GROUP()
	{
		serial=0;
		tolerance=0.5;
		nMaxWarnings=100;
		bOutputToNotepad=FALSE;
	}
};

struct ANALYZE_SCOPE
{
	int m_iCalMode;
	BOOL m_bUpdateBoltInfo;
	bool m_bVerifyFoundationMode;
	bool m_bVerifyFoundationByStdLoad;
	CXhChar200 csModule,csStatus;
	CFGWORD cfgwordBody,cfgwordLeg;
};
#ifdef __PART_DESIGN_INC_
class CRevisionOperDlg;
#endif
class CMainFrame : public CFrameWndEx
{
protected: // create from serialization only
	CMainFrame();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	DECLARE_DYNCREATE(CMainFrame)
	CMenu m_NewMenu;
	HMENU m_hMenuDefault;

// Attributes
public:
	//数据中心菜单
	CHashList<CMDCFG_ITEM> cmdItems;
	CMD_GROUP cmdRoutineChk,cmdPartNoChk,cmdCollisionChk,cmdCorrectErros;
	CCirStack<long>prev_part;
	CCirStack<long>next_part;
	ANALYZE_SCOPE m_xAnalyzeScope;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual void HtmlHelp(DWORD_PTR dwData, UINT nCmd = 0x000F);
	//virtual void WinHelpInternal(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMouseStatusBar m_wndStatusBar;
	CLDSMFCToolBar	m_wndToolBar;
	CLDSMFCToolBar	m_wndModelling;
	CLDSMFCToolBar	m_wndToolKit;
	CLDSMFCToolBar	m_wndPartDesignBar;
	CLDSMFCToolBar	m_wndGLBar;
	CLDSMFCToolBar	m_wndPartEditor;
	CLDSMFCToolBar	m_wndPartSearch;
	CLDSMFCToolBar  m_wndViewBar;
	CLDSMFCToolBar	m_wndDataCheckBar;
	CLDSMFCMenuBar		m_wndMenuBar;
	CMFCToolBarImages	m_userImages;
	CDialogPanel	m_towerPropertyView;
	CDialogPanel	m_towerTreeView;
	CDialogPanel	m_cmdLineView;
	CDialogPanel	m_revisionOperView;
	CDialogPanel	m_logView;
	CDialogPanel	m_outputInfoView;
	CDialogPanel	m_towerPartGroupView;
	CDialogPanel	m_partNoListView;
	CDialogPanel    m_dataCheckView;
#ifdef __CONNECT_REMOTE_SERVER_
	CDialogPanel    m_remoteServerView;
#endif
	CMFCToolBarEditBoxButton *m_editButton;
// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnClose();
	afx_msg void OnReleaseNotes();
	afx_msg void OnUpgradeSoftware();
	afx_msg void OnCheckVersion();
	afx_msg void OnSendMsgToOther();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPartEnv();
	afx_msg void OnUpdatePartEnv(CCmdUI* pCmdUI);
	afx_msg void OnAppExitPartApp();
	afx_msg void OnPartEditorToolBarDropDown(NMHDR *pnm,LRESULT *plr);
	afx_msg void OnDockpageModel();
	afx_msg void OnUpdateDockpageModel(CCmdUI* pCmdUI);
	afx_msg void OnDockpageProperty();
	afx_msg void OnUpdateDockpageProperty(CCmdUI* pCmdUI);
	afx_msg void OnDockpageCmdline();
	afx_msg void OnUpdateDockpageCmdline(CCmdUI* pCmdUI);
	afx_msg void OnDockpageOutput();
	afx_msg void OnUpdateDockpageServer(CCmdUI* pCmdUI);
	afx_msg void OnDockpageServer();
	afx_msg void OnUpdateDockpageOutput(CCmdUI* pCmdUI);
	afx_msg void OnDockdataCheck();
	afx_msg void OnUpdateDockdataCheck(CCmdUI* pCmdUI);
	afx_msg void OnDockpageRevision();
	afx_msg void OnUpdateDockpageRevision(CCmdUI* pCmdUI);
	afx_msg void OnDockpageLogger();
	afx_msg void OnUpdateDockpageLogger(CCmdUI* pCmdUI);
	afx_msg void OnDockpageWeldPart();
	afx_msg void OnUpdateDockpageWeldPart(CCmdUI* pCmdUI);
	afx_msg void OnDockpagePartLabel();
	afx_msg void OnUpdateDockpagePartLabel(CCmdUI* pCmdUI);
	afx_msg void OnSolidModeDisplay();
	afx_msg void OnUpdateSolidModeDisplay(CCmdUI* pCmdUI);
	afx_msg void OnLineModeEnv();
	afx_msg void OnUpdateLineModeEnv(CCmdUI* pCmdUI);
	afx_msg void OnSolidModeEnv();
	afx_msg void OnUpdateSolidModeEnv(CCmdUI* pCmdUI);
	afx_msg void OnUndo();
	afx_msg void OnRedo();
	afx_msg void OnUpdateUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRedo(CCmdUI* pCmdUI);
	afx_msg void OnCodeNodeI();
	afx_msg void OnDesWireNode();
	afx_msg void OnImportLoadFile();
	afx_msg void OnExportLoadFile();
	afx_msg void OnDisplayDeformTower();
	afx_msg void OnModelFreqs();
	afx_msg void OnModelDesignCal();
	afx_msg void OnTrussStableCheck();
	afx_msg void OnInsertStdFacePanel();
	afx_msg void OnUpdateDisplayDeformTower(CCmdUI* pCmdUI);
	afx_msg void OnBnBendPartCheck();
	afx_msg void OnImportVibrationMode();
	afx_msg void OnDisplayVibrationMode();
	afx_msg void OnUpdateDisplayVibrationMode(CCmdUI* pCmdUI);
	afx_msg void OnAnalysisWorkEnv();
	afx_msg void OnUpdateAnalysisWorkEnv(CCmdUI* pCmdUI);
	afx_msg void OnFemElemEnv();
	afx_msg void OnUpdateFemElemEnv(CCmdUI* pCmdUI);
	afx_msg void OnKeyboardMap();
	afx_msg void OnSysParameters();
	afx_msg void OnStubAngleCheck();
	afx_msg void OnFootDesignCal();
	afx_msg void OnModelEqCheck();
	afx_msg void OnExportRptFile();
	afx_msg void OnResultBuffer();
	afx_msg void OnCompareWithTtaResults();
	afx_msg void OnCorrectErrors();
	afx_msg void OnRoutineCheck();
	afx_msg void OnPartNoCheck();
	afx_msg void OnCollisionCheck();
	afx_msg LRESULT OnToolbarReset(WPARAM,LPARAM);
	afx_msg void OnModelWireLoadCal();
	afx_msg void OnClickEditBox();
	afx_msg void OnQuickSearchPart();
	afx_msg void OnDrawing2dEnv();
	afx_msg void OnUpdateDrawing2dEnv(CCmdUI *pCmdUI);
	afx_msg void OnLicenseCenter();
	afx_msg void OnUpdateStatusBar(CCmdUI *pCmdUI);
	afx_msg void OnShiftUiOfMechanics();
	afx_msg void OnShiftUiOfRapid3dModel();
	afx_msg void OnShiftUiOfFinal3dModel();
	afx_msg void OnShiftUiOfDrawingModel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL CreateDockingWindows();
public:
	void UpdateGlToolBarCmdUI();
	BOOL CreateWireLoadProcess( HANDLE hClientPipeRead, HANDLE hClientPipeWrite );
	void ModelWireLoadCal(){OnModelWireLoadCal();}
	void ModelDesignCal(int iSelModuleNo=0,bool bEqCheck=false);
	void ModelCheck(bool bEqCheck=false);
	void ModelOptimize();
	void ModelFreqsAnalyze(int iElemProcessMode = 0);	//0.梁杆自动判断;1.全部梁元计算
	bool SetPaneTextById(UINT idPaneItemId,const char* sPaneText,BOOL bUpdate=TRUE);
	void SetPaneText(int nIndex, LPCTSTR lpszNewText, BOOL bUpdate = TRUE );
	void LoadNewMenu(UINT idMenu);
	void SavePreWorkUIState();
	void ShiftUiOfMechanics();
	void ShiftUiOfRapid3dModel();
	void ShiftUiOfFinal3dModel();
	void ShiftUiOfDrawingModel();
	void ShiftToPartEditor();
	void ExitPartApp();
	void SetBottomWorkspaceTitle(LPCSTR sTitle);		//设置底部工作区标题	
	void ActivateDockpage(CRuntimeClass *pRuntimeClass);
	void ModifyDockpageStatus(CRuntimeClass *pRuntimeClass, BOOL bShow);
	void ShowTaskInfoList(CTaskOutputList &infoList);
	void Undo();
	void InitDataCenterCmdItems();
	void CheckPartNoByScope(void* pContext);
	//根据配置文件更新命令组命令顺序及激活状态
	void UpdateDataCenterCmdGroupPara();
	void UpdateAllPages();
	void StartInfoPrompt();
	void KillInfoPrompt();
	UINT m_nTimerID;
	CLDSDoc *m_pDoc;
	BOOL m_bDisplayDeformedTower;

	CTowerTreeDlg* GetTowerTreePage() { return (CTowerTreeDlg*)m_towerTreeView.GetDlgPtr(); }
	void ShowDockpageOutput();
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CTowerPartGroupDlg *GetTowerPartGroupPage() { return (CTowerPartGroupDlg*)m_towerPartGroupView.GetDlgPtr(); }
	COutputInfoDlg *GetOutputInfoPage() { return (COutputInfoDlg*)m_outputInfoView.GetDlgPtr(); }
#ifdef __SMART_DRAW_INC_
	CPartNoListDlg* GetPartNoListPage() { return (CPartNoListDlg*)m_partNoListView.GetDlgPtr(); }
#endif
#endif
#ifdef __PART_DESIGN_INC_
	CLogViewerDlg* GetLogViewerPage() { return (CLogViewerDlg*)m_logView.GetDlgPtr(); }
	CRevisionOperDlg* GetRevisionOperPage() { return (CRevisionOperDlg*)m_revisionOperView.GetDlgPtr();	}
#endif
#ifdef __TMA_
	CDataCheckDlg* GetDataCheckPage() { return (CDataCheckDlg*)m_dataCheckView.GetDlgPtr(); }
#endif
#ifdef __CONNECT_REMOTE_SERVER_
	CRemoteServerDlg* GetRemoteServerPage() { return (CRemoteServerDlg*)m_remoteServerView.GetDlgPtr(); }
#endif
	CCmdLineDlg* GetCmdLinePage() { return (CCmdLineDlg*)m_cmdLineView.GetDlgPtr(); }
	CTowerPropertyDlg* GetTowerPropertyPage() { return (CTowerPropertyDlg*)m_towerPropertyView.GetDlgPtr(); }
	CLDSView* GetLDSView() { return (CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));}
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__7DA70090_573A_40CE_96C7_DCE49CAD5E31__INCLUDED_)
