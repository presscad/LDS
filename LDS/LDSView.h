// LDSView.h : interface of the CLDSView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LDSVIEW_H__20AB67FD_535B_4270_B4FD_28BEF9A3A45A__INCLUDED_)
#define AFX_LDSVIEW_H__20AB67FD_535B_4270_B4FD_28BEF9A3A45A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Lds.h"
#include "lds_part.h"
#include "env_def.h"
#include "LDSDoc.h"
#include "ldsptr_list.h"
#include "Tower.h"
#include "CommandDefOper.h"
#include "PromptDlg.h"
#include "CutPlateOptionDlg.h"
#include "Block.h"
#include "KeypointLifeObj.h"
#if defined(__LDS_)||defined(__TSA_)||defined(__TDA_)
#include "DrawEngine.h"
#endif
#include "BasicOutput.h"

struct GET_SCR_PART_PARA
{	
public:
	int cls_id1;
	int cls_id2;
	DISPLAY_TYPE disp_type;
	CString cmdStr;
	CString cmdErrorFeedbackStr;
	DWORD dwSelectPartType;
	CLDSDbObject **pResultObjsArr;
	int nResultObjs;
	GET_SCR_PART_PARA(){
		dwSelectPartType=0;
		nResultObjs=0;
		pResultObjsArr=NULL;
	}
	//CLDSDbObject** pResultObjsArr=NULL
	//��֧���ⲿ���������ʼ��pResultObjsArr,pResultObjsArr�ڴ����ڲ����� wht 11-05-27
	GET_SCR_PART_PARA(DWORD dwSelectPartTypeFlag,int cls_id1=0,int cls_id2=0,int nResultParts=0){
		dwSelectPartType=dwSelectPartTypeFlag;
		this->cls_id1=cls_id1;
		this->cls_id2=cls_id2;
		this->nResultObjs=nResultParts;
		this->pResultObjsArr=NULL;
		//this->pResultObjsArr=pResultObjsArr;
	}
	~GET_SCR_PART_PARA()
	{	//�������������ͷ�ѡ�񹹼�ʱ������ڴ� wht 11-05-27
		if(pResultObjsArr)
			delete []pResultObjsArr;
		pResultObjsArr=NULL;
	}
};
bool GetPartsFromScr(GET_SCR_PART_PARA &part_para,BOOL bCaptureLButtonUpMsg=FALSE);//Ĭ�������в���Ӧ������̧�����Ϣ
//CAngleMoveOperObject�Ƶ�keypointLiftObj.h  ���йض��������������������ͳһ����  wxc-2017.8.4
struct SORTEDNODE_ITEM{CLDSNode *pNode;double distToStart;};
typedef ATOM_LIST<SORTEDNODE_ITEM> CNodeList;
class CLDSView : public CView
{
	TASK_TYPE m_curTask;		//��ǰ��������
	int m_nObjectSnapedCounts;	//��ǰ�Ѳ�׽����ͼԪ��
	IDrawSolid* m_pDrawSolid;
	typedef int (CLDSView::*SYSCMD_FUNC_PTR)();
public:
	struct SYS_CMD{
		static const DWORD CMD_NEED_CMDINPUT		= 0x01;
		static const DWORD CMD_ANALYSIS_EXCLUSIVE	= 0x02;
		static const DWORD CMD_BASICDRAW	= 0x04;	//0x04+0x08
		static const DWORD CMD_SMARTDRAW	= 0x08;
		//static const DWORD CMD_SMARTDRAW	= 0x08;
		CXhChar50 cmdStr;
		char briefCmdStr[4];
		CXhChar50 descript;	//����
		DWORD dwTypeFlag;
		SYSCMD_FUNC_PTR execute;
		SYS_CMD(){
			execute=NULL;
			dwTypeFlag=0;
			briefCmdStr[0]=briefCmdStr[1]=briefCmdStr[2]=briefCmdStr[3]=0;
		}
		SYS_CMD(SYSCMD_FUNC_PTR execute_func,const char* cmd,const char* briefcmd=NULL,DWORD typeFlag=0){
			cmdStr.Copy(cmd);
			if(briefcmd!=NULL)
				strncpy(briefCmdStr,briefcmd,3);
			else
				briefCmdStr[0]=briefCmdStr[1]=briefCmdStr[2]=0;
			briefCmdStr[3]=0;
			dwTypeFlag=typeFlag;
			execute=execute_func;
		}
	};
private:
	//��ͼ����envionment
	struct DISPLAY_ENV{
		int eViewType;
		long m_hView;
		long m_hHeightModule;
		GECS object_ucs;
		char envpool[2048];
	};
	// �洢Undo��Redo�Ĳ�����¼
	CCirBufferStack UndoViewBuffStack;
	bool FromViewBuffer(CBuffer& buffer,DISPLAY_ENV& enviornment,NODESET* pDisplayNodeSet,PARTSET* pDisplayPartSet);
	void CurrentViewToBuffer(CBuffer& buffer);
public:
	// ִ����ͼѹջ����,������ѹջ�ڴ��С
	UINT PushView(void);
	// ִ��Undo����
	void UndoView(void);
	// ִ��Redo����
	void RedoView(void);
	// ����ִ��Undo����
	BOOL CanUndoView(void);
	// ����ִ��Redo����
	BOOL CanRedoView(void);
	// �Ƿ����µ�Undo��Ϣ��Ҫ�洢
	BOOL IsViewStateModified();
protected: // create from serialization only
	CLDSView();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	DECLARE_DYNCREATE(CLDSView)
#ifdef __GGG_
	CDrawEngine *m_pDraw2d;
#endif
// Attributes
public:
	CLDSDoc* GetDocument();
	VIEW_DIRECT m_eViewFlag;
	BOOL m_bInputCharByCmdLine;
	CHashSet<CLDSObject*>selectObjs;
	CHashListEx<PROP_OBJ> hashSelectObjs;	//δ����ȡ��selectObjs,��ʱ����������һЩ���ߡ��ҵ����ʱʵ����ʾ����
	CTmaPtrList<SUB_OBJ,SUB_OBJ*>selectSubObjs;	//���ڴ�Ű������ͼԪ��Ϣ
	ATOM_LIST<SYS_CMD> listSysCmds;
	CBlockModel* m_pSelectBlock;
	//��¼�����༭������ӵĸ������븨����,�˳������༭��ʱ��� wht 16-11-10
	ATOM_LIST<GEPOINT> m_xAssistPointList;
	ATOM_LIST<GELINE> m_xAssistLineList;
private:
	//��ǰѡ�е����й����ı��ݣ�������ʾʱʹ�� selectObjs<=selectObjsBackup wht 10-11-12
	CTmaPtrList<CLDSObject,CLDSObject*>selectObjsBackup;
	void UpdatePlateProfileDisplayStates();
// Operations
public: 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLDSView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	HINSTANCE m_hResHandle;
	APPLICATION_TYPE m_curAppType;
	void OpenWndSel();
	void OperOther();
	long EntPickEvent(long ID);
	void ExportBmpFile(int left,int bottom,int right,int top);
	void DisconnectSolidSrcComp();	//��DrawSolid���Դ����������
	void ConnectSolidSrcComp();		//����DrawSolid���Դ�������
	void AutoDaoWei();
	BOOL LockFunc(BOOL bPromptMsg=TRUE);
	BOOL IsCmdLocked(){	return pFuncStatus->IsLocked();}//�жϺ����Ƿ�������������������������
	void ReleaseFunc();
	virtual ~CLDSView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
// Generated message map functions
protected:
	//{{AFX_MSG(CLDSView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnShareJdbDesign();
	afx_msg void OnMergeLjb();
	afx_msg void OnCutPlateByPlane();
	afx_msg void OnRotatePartAroundAxis();
	afx_msg void OnCutJg();
	afx_msg void OnSegICheck();
	afx_msg void OnPartNoCheck();
	afx_msg void OnLocatePartByNo();
	afx_msg void OnDrawTaInAcad();
	afx_msg void OnLinePartExtendVec();
	afx_msg void OnTaDaoWei();
	afx_msg void OnEditFindPart();
	afx_msg void OnHidePart();
	afx_msg void OnInverseSelectObjs();
	afx_msg void OnCheckPartData();
	afx_msg void OnDisplayPartWorkUcs();
	afx_msg void OnEditSelectAll();
	afx_msg void OnClearDesignResult();
	afx_msg void OnExplodeHuoqu();
	afx_msg void OnHandShortJg();
	afx_msg void OnOneKeyCutAngleCheck();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSendPartEditor();
	afx_msg void OnSendPpeEditor();
	afx_msg void OnUpdateSendPartEditor(CCmdUI* pCmdUI);
	afx_msg void OnAssembleCs();
	afx_msg void OnWorkCs();
	afx_msg void OnAttachPartTbl();
	afx_msg void OnImportPart();
	afx_msg void OnFeatureProp();
	afx_msg void OnNewLs();
	afx_msg void OnSpecWcsOrgin();
	afx_msg void OnSpecAxisXVertex();
	afx_msg void OnDelPartFeature();
	afx_msg void OnMeasureVertexDist();
	afx_msg void OnInVertex();
	afx_msg void OnFoldPlank();
	afx_msg void OnReCalHuoquPos();
	afx_msg void OnClearNail();
	afx_msg void OnCodePartNoRule();
	afx_msg void OnCodePartNo();
	afx_msg void OnInsertJgnodeToView();
	afx_msg void OnRemoveObjsFromView();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnClearPartNoI();
	afx_msg void OnCheckJgLsValid();
	afx_msg void OnSetNcDriver();
	afx_msg void OnGenJgNcFile();
	afx_msg void OnGenPartProcessInfoFile();
	afx_msg void OnDisplayOrHideAllHole();
	afx_msg void OnGenPlankNcData();
	afx_msg void OnMoveDbObjPosition();
	afx_msg void ModifyPartPos(CLDSPart* pPart);
	afx_msg void OnImportPlankProfile();
	afx_msg void OnMakeEndBend();
	afx_msg void OnAutoCalPlankProfile();
	afx_msg void OnBendPlank();
	afx_msg void OnConvertToGroupJg();
	afx_msg void OnAddFillPlank();
	afx_msg void OnDesignGuaXianHole();
	afx_msg void OnDesignBushTube();
	afx_msg void OnSpecLsRayNo();
	afx_msg void OnUpdateHidePart(CCmdUI* pCmdUI);
	afx_msg void OnDaoweiSelectPart();
	afx_msg void OnConvertToSingleJg();
	afx_msg void OnAutoCorrectJgLaystyle();
	afx_msg void OnSolidCollideCheck();
	afx_msg void OnCheckPlankLsValid();
	afx_msg void OnRollPlankEdge();
	afx_msg void OnHandoverPermission();
	afx_msg void OnReturnToAssembleEnv();
	afx_msg void OnRemoveKaiheJiao();
	afx_msg void OnPrevPart();
	afx_msg void OnNextPart();
	afx_msg void OnUpdatePrevPart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNextPart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewAttachVerticalPlate(CCmdUI* pCmdUI);
	afx_msg void OnSpecCutFaceCutJg();
	afx_msg void OnAngleFaceOffsetOddDiff();
	afx_msg void OnFileNew();
	afx_msg void OnFileNewTowerModel();
	afx_msg void OnFileNewSubstation();
	afx_msg void OnSaveAsNewView();
	afx_msg void OnLinkLinePart();
	afx_msg void OnOpenWndSel();
	afx_msg void OnDefCtrlPt();
	afx_msg void OnDefOffsetNode();
	afx_msg void OnDefIntersNode();
	afx_msg void OnDefHorizonNode();
	afx_msg void OnDefAxisValFixNode();
	afx_msg void OnDefZhuJgPoly();
	afx_msg void OnMakeJgBend();
	afx_msg void OnCal1jgNorm();
	afx_msg void OnGlOpenWindow();
	afx_msg void OnUpdateGlOpenWindow(CCmdUI* pCmdUI);
	afx_msg void OnGlAllZoom();
	afx_msg void OnGlZoom();
	afx_msg void OnGlPan();
	afx_msg void OnUpdateGlPan(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGlZoom(CCmdUI* pCmdUI);
	afx_msg void OnGlRotated();
	afx_msg void OnUpdateGlRotated(CCmdUI* pCmdUI);
	afx_msg void OnResetView();
	afx_msg void OnOperOther();
	afx_msg void OnOperControlCenter();
	afx_msg void OnNodeCenter();
	afx_msg void OnRedrawAll();
	afx_msg void OnSpreadFace();
	afx_msg void OnSpreadNodeFace();
	afx_msg void OnSelectNodeFace();
	afx_msg void OnDesignJdb();
	afx_msg void OnCircularPlate();
	afx_msg void OnGyHole();
	afx_msg void OnXieIntersPtDesign();
	afx_msg void OnSingleXieNodeDesign();
	afx_msg void OnAllSingleXieNodeDesign();
	afx_msg void OnOneBoltDesign();
	afx_msg void OnSpecNodeOneBoltDesign();
	afx_msg void OnLayJgEndLs();
	afx_msg void OnCommonJgJoint();
	afx_msg void OnLayFillPlank();
	afx_msg void OnLayoutFootnail();
	afx_msg void OnFootPlank();
	afx_msg void OnPartPermSwitch();
	afx_msg void OnUpdatePartPermSwitch(CCmdUI* pCmdUI);
	afx_msg void OnRefreshBurglarLs();
	afx_msg void OnDisplayCoordAxis();
	afx_msg void OnUpdateDisplayCoordAxis(CCmdUI* pCmdUI);
	afx_msg void OnExportBmpFile();
	afx_msg void OnDisplayFrameLine();
	afx_msg void OnUpdateDisplayFrameLine(CCmdUI* pCmdUI);
	afx_msg void OnDisplayLineVec();
	afx_msg void OnUpdateDisplayLineVec(CCmdUI* pCmdUI);
	afx_msg void OnDisplayLineNode();
	afx_msg void OnUpdateDisplayLineNode(CCmdUI* pCmdUI);
	afx_msg void OnViewJgKeyPoint();
	afx_msg void OnToolKit();
	afx_msg void OnMeasure();
	afx_msg void OnCalJgWingZhunIntersPt();
	afx_msg void OnDesHoofPlank();
	afx_msg void OnCalWorkFaceNorm();
	afx_msg void OnRevisionBoltL0nZpos();
	afx_msg void OnRevisionBoltSpace();
	afx_msg void OnAutoDesignSelectedParts();
	afx_msg void OnObjectProperty();
	afx_msg void OnDefDivideScaleNode();
	afx_msg void OnDesignLjPlate();
	afx_msg void OnDesignRibPlate();
	afx_msg void OnDesignOverlappedPlate();
	afx_msg void OnBreakLinePart();
	afx_msg void OnMirrorCreate();
	afx_msg void OnDesignSphere();
	afx_msg void OnUpdateMirCreateTransectFace(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMirrorCreate(CCmdUI* pCmdUI);
	afx_msg void OnRecreateSpecPartSolidmodel();
	afx_msg void OnDesignLinepartOddment();
	afx_msg void OnDesignElbowPlate();
	afx_msg void OnImportBoltRefToPart();
	afx_msg void OnLookBoltInPart();
	afx_msg void OnImportLinePartToPlate();
	afx_msg void OnLookLinePartInPlate();
	afx_msg void OnRetrievePartSegI();
	afx_msg void OnCreateBoltInPart();
	afx_msg void OnAddRelativeObject();
	afx_msg void OnUpdateAddRelativeObject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCreateBolt(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImportBoltRef(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLookBolt(CCmdUI* pCmdUI);
	afx_msg void OnComposeLinepart();
	afx_msg void OnTubeBetweenPlate();
	afx_msg void OnExtendPlate();
	afx_msg void OnSpecWcsOrginByGlobalCs();
	afx_msg void OnNewArcPart();
	afx_msg void OnGroupEditPartLayer();
	afx_msg void OnBoltValidCheck();
	afx_msg void OnBoltPunchHole();
	afx_msg void OnBoltPad();
	afx_msg void OnInsertPlateStyleTubeInters();
	afx_msg void OnConvertToPlate();
	afx_msg void OnCutPlateByColumnFace();
	afx_msg void OnDivideMainLinePart();
	afx_msg void OnAdjustPlateWeldLen();
	afx_msg void OnCompareLinepartPos();
	afx_msg void OnTurnAngleClockwise();
	afx_msg void OnTurnAngleAnticlockwise();
	afx_msg void OnLookDesignParam();
	afx_msg void OnCheckBoltValid();
	afx_msg void OnSetupAidedPlate();
	afx_msg void OnAffirmCurCommand();
	afx_msg void OnCancelCurCommand();
	afx_msg void OnDelSpecifyPart();
	afx_msg void OnIntelligentConnectDesign();
	afx_msg void OnStdComTmplRepository();
	afx_msg void OnConnectFromTemplate();
	afx_msg void OnExportConnectionTemplate();
	afx_msg void OnCopyToClipboard();
	afx_msg void OnClassicConnectDesign();
	afx_msg void OnTurnAngleInoutSide();
	afx_msg void OnTurnAngleWing();
	afx_msg void OnImportExistLs();
	afx_msg void OnDesignSpecialFl();
	afx_msg void OnDesignSimpleLadder();
	afx_msg void OnDefAllIntersNode();
	afx_msg void OnUpdateDisplayAllHole(CCmdUI* pCmdUI);
	afx_msg void OnMakePlateBend();
	afx_msg void OnFastCirCoverPlate();
	afx_msg void OnFastCirRibPlate();
	afx_msg void OnFastTubeRibPlate();
	afx_msg void OnAddMirRelativeInfo();
	afx_msg void OnCheckRelativeMirInfo();
	afx_msg void OnValidateCrossrodBearcompInfo();
	afx_msg void OnShadowCreate();
	afx_msg void OnOuterAttachAngle();
	afx_msg void OnDesignTubeTowerFoot();
	afx_msg void OnDesignTubeTowerSlope();
	afx_msg void OnBillOfFL();
	afx_msg void OnAngleJointDesignCal();
	afx_msg void OnReplaceBoltDatumpoint();
	afx_msg void OnMoveBoltSet();
	afx_msg void OnUnifyBoltNorm();
	afx_msg void OnPlateProfileToRect();
	afx_msg void OnCopyPartSet();
	afx_msg void OnSelectRelaBolts();
	afx_msg void OnAddRelaBoltsToView();
	afx_msg void OnSetPartSpreadFace();
	afx_msg void OnCorrectBoltL0Para();
	afx_msg void OnConvertToTube();
	afx_msg void OnOperShankSection();
	afx_msg void OnAddLegAndBraceRods();
	afx_msg void OnAddGtmBraceElemPanels();
	afx_msg void OnSetArcLift();
	afx_msg void OnSaveAsNewBlock();
	afx_msg void OnExplodeBlockRef();
	afx_msg void OnLogViewer();
	afx_msg void OnModifyPartNoSegI();
	afx_msg void OnDesignColumnRayRod();
	afx_msg void OnStartWingXCutAngle();
	afx_msg void OnStartWingYCutAngle();
	afx_msg void OnEndWingXCutAngle();
	afx_msg void OnEndWingYCutAngle();
	afx_msg void OnWingXCutAngle();
	afx_msg void OnWingYCutAngle();
	afx_msg void OnUpgradeCtrlNode();
	afx_msg void OnCalPerfectSectCount();
	afx_msg void OnMergeNodes();
	afx_msg void OnRelayoutScaleNodes();
#ifdef __ANGLE_PART_INC_
	afx_msg void OnBranchNodes();
	afx_msg void OnLabelInput();
	afx_msg void OnWasherRevision();
#endif
	afx_msg void OnSyncMirObjProp();
	afx_msg void OnRemoveRelaObjects();
	afx_msg void OnUpdateRemoveRelaObjects(CCmdUI *pCmdUI);
	afx_msg void OnIntelliCorrectModelObjsLayer();
	afx_msg void OnUnifyAngleBoltsParamG();
	afx_msg void OnHandleReplace();
	afx_msg void OnReuseParts();
	afx_msg void OnCreateFacePanel();
	afx_msg void OnPPEStarter();
	afx_msg void OnSwapBoltG();
	afx_msg void OnDataCompare();
	afx_msg void OnMechanicsCheck();
	afx_msg void OnOptimizePlate();
	afx_msg void OnAddAssistLine();
	afx_msg void OnNextView();
	afx_msg void OnPrevView();
	afx_msg void OnUpdatePrevView(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNextView(CCmdUI *pCmdUI);
	afx_msg void OnBasePlate();
	afx_msg void OnAngleShoePlate();
	afx_msg void OnRevisionMode();
	afx_msg void OnUpdateRevisionMode(CCmdUI *pCmdUI);
	afx_msg void OnShiftView(UINT cmdID);
	afx_msg void OnUpdateShiftView(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void GetBoltActiveRelaPartSet(CLDSBolt* pBolt,PARTSET& partset,CLDSModule* pActiveModule);
	void DelSpecPart(long h);
	void NotifyObjStateModified(long* id_arr,long count=1,UINT message=1);	//message=1��ʾɾ��
	int RedrawAll();
	int RenderAll();
	double GetPickPosScaleOnRodS2E(CLDSLinePart* pRod);
	void SpecWcsOrgin();
	void SpecAxisXVertex();
	void SpecAxisYVertex();
	BOOL FinishViewKeyPoint(CLDSDbObject *pObj1, CLDSDbObject *pObj2);
	void LayJgEndLs();
	void LayFillPlank();
	int ModifyShankSection();
	void PPEStarter(PARTSET *pPartSet=NULL,bool bWaitReturn=false);
	void IntelliCorrectModelObjsLayer();
	void UnifyAngleBoltsParamG();
	bool IsNeedReverseBoltDirection(CLDSBolt* pBolt,CLDSPart* pDatumPart=NULL);
	bool ReverseBoltDirection(CLDSBolt* pBolt,CLDSModule* pModule=NULL);
	void RelayoutRodModel();
	void ClearSelectRodNail(LINEPARTSET& xLinePartSet);
private:
	LRESULT ObjectSnappedProcess(WPARAM ID, LPARAM ent_type);	//��׽��Ϣ����
	LRESULT ObjectSelectProcess(WPARAM ID, LPARAM ent_type);	//ѡȡ��Ϣ����
	LRESULT CmdLineAffirmed(WPARAM wParam, LPARAM lParam);		//����������õ�ȷ��
	LRESULT CmdLineCanceled(WPARAM wParam, LPARAM lParam);		//���������ȡ��
	BOOL DefMultiLeg(CLDSNode *pOriginNode,CLDSLineAngle *pOriginJg);
	
	void FinishSpeadNodeFace(THANDLE *id_arr, int node_n );
	void FinishCalAngleFaceOffsetOddDiff(CLDSDbObject *pTowerAtom);
	BOOL DesignShortAngle();
	BOOL FinishOuterAttachAngle(CLDSLineAngle* pAngle=NULL,CLDSNode* pNode=NULL);
	BOOL FinishCalLinePartExtendVec(CLDSDbObject *pObj);
	BOOL FinishCalWorkFaceNorm(
		CLDSDbObject *pObj1,CLDSDbObject *pObj2);
	BOOL FinishDesignJoint(CLDSDbObject *pNodeAtom,
			CLDSDbObject *pJgAtom1,CLDSDbObject *pJgAtom2);
	BOOL FinishDesignJdb(CLDSNode *pSelNode) ;
	int  DesignNodePlate();	//ͨ�������е���FinishDesignJdb
	void OnMirBoltOnlyConnect() ;
	void FinishMirBoltOnlyConnect() ;
	BOOL FinishDelSingleBolt(CLDSDbObject *pObj1,CLDSDbObject *pObj2);
	BOOL FinishVerticalDivideNode(CLDSDbObject* pObj);
	BOOL FinishDefShareBody(int *id_arr, int nSelect);
	int FinishSpreadFace();
	BOOL FinishSpreadFaceByLinePart(CLDSLinePart* pLinePart);
	BOOL FinishSpreadFaceByNodes(CLDSNode* pNode1,CLDSNode* pNode2);
	BOOL FinishViewFootPlank(CLDSDbObject *pObj);
	BOOL FinishViewMainJgJoint(CLDSDbObject *pObj);
	BOOL FinishMeasure(CLDSDbObject * pObj1,CLDSDbObject *pObj2=NULL,CLDSDbObject *pObj3=NULL,CLDSDbObject *pObj4=NULL);
	BOOL FinishFootPlank(CLDSDbObject  *pObj1,CLDSDbObject *pObj2);
	BOOL FinishHoofPlank(CLDSDbObject  *pObj1,CLDSDbObject *pObj2,CLDSDbObject *pObj3);
	BOOL FinishDefOffsetNode();
	BOOL FinishDefHorizonNode();
	int FinishDefDivideScaleNode();
	int FinishLinkPart();
	BOOL FinishDefScaleNodeByLinePart(CLDSLinePart *pLinePart=NULL);
	BOOL FinishDefScaleNodeByNodeSection(CLDSNode *pSelNode1=NULL,CLDSNode *pSelNode2=NULL);
	BOOL FinishDefScaleNodeByIntersNodeSection(CLDSNode *pSelNode=NULL);
	BOOL FinishDefAxisValFixNode();
	BOOL FinishViewProperty(CLDSDbObject *pObj);
	BOOL FinishLinkTsaPole(CLDSNode *pStartNode, CLDSNode *pEndNode);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
public:
	bool AppendBoltRefToPart(CLDSPart *DatumPart,CLDSBolt* pBolt,int style=0);
	void ImportBoltRefToPart(CLDSPart *pPart,int style=0);
	void ImportLinePartToPlate(CLDSPlate *pPlate);
	void ShiftApplication(int app_type);
	BOOL FinishDesignCircularPlate(CLDSNode *pNode,CLDSLineTube *pLineTube, BOOL bEndPlate=FALSE,double manu_space=0);
private:
	int  LayoutAngleBolts();
	int  AngleBolts();
#ifdef __ANGLE_PART_INC_
	bool CopySingularPlate(CLDSPlate* pPlate,CLDSNode* pDestNode);
#endif
	void CopyBoltSimplified(BOLTSET* pBoltSet=NULL);		//������ļ��׸�����˨���������
	void CopyNodes(NODESET& nodeset);		//���ƽڵ�
	void CopyOrdinaryParts(PARTSET& partset);
	BOOL FinishMeasureVertexDist(fAtom *pAtom1,fAtom*pAtom2);
	BOOL FinishSpecWCS(fAtom *pAtom);
	BOOL FinishViewPartFeature(fAtom *pAtom);
	BOOL FinishViewLine(fAtom *pAtom);
	void LayoutSingleRodBolts(CLDSLinePart* pCurRod,BYTE ciCurWorkWing,CLDSNode* pBaseNode,CLDSLineAngle* pCrossAngle=NULL);
	void LayoutSingleAngleBolts(CLDSLineAngle *pLineAngle,CLDSNode *pBaseNode,CLDSLineAngle *pBackToBackJgX=NULL,CLDSLineAngle *pBackToBackJgY=NULL);
	void LayoutSlotOrFlatBolts(CLDSLinePart *pLinePart,CLDSNode *pBaseNode);
	void LayoutTubeBolts(CLDSLineTube* pLineTube,CLDSNode* pBaseNode);
	int  DesignOddment();
	BOOL FinishAddFillPlank(CLDSDbObject *pFirObj,CLDSDbObject *pSecObj=NULL,CLDSDbObject *pThirObj=NULL);
	BOOL FinishReCalHuoQuPos(fAtom *pAtom);
	BOOL FinishInsertPlankVertex(fAtom *pFeatAtom);
	BOOL FinishRollPlankEdge(fAtom *pAtom);
	BOOL FinishViewCir(fAtom *pAtom);
	BOOL FinishViewLsCir(fAtom *pAtom);
	BOOL FinishViewVertex(fAtom *pAtom);
	BOOL FinishDelPartFeature(fAtom *pFeatAtom);
	BOOL FinishDefPoly(f3dPoint *vertex_arr,int nCount);
	void FinishBendPlank(fAtom *pAtomStart, fAtom *pAtomEnd);
	void CalDefaultAssembleCS(CLDSDbObject *pObj);
	int  CommonJgJoint();
	int  LayoutFootnail();
	int  LayoutPedalPlate();
	int  DesignOneBoltEndAngle();
	int  TubeBetweenPlate();
	int  ExtendPlate();
	int  RollPlankEdge();
	BOOL FinishCal1JgNorm(CLDSDbObject *pObj1,CLDSDbObject *pObj2);
	void FinishCal2PoleIntersPt(CLDSDbObject *pObj1,CLDSDbObject *pObj2);
	BOOL ExtendJg(CLDSLineAngle *pJg, BOOL bExtendStart=FALSE);
	BOOL FinishCalCutJg(CLDSDbObject *pObj1,CLDSDbObject *pObj2);
	CLDSLineAngle* FinishLinkAngle(CLDSNode *pStartNode, CLDSNode *pEndNode);
	BOOL FinishLinkTube(CLDSNode *pStartNode, CLDSNode *pEndNode);
	BOOL FinishLinkSlot(CLDSNode *pStartNode, CLDSNode *pEndNode);
	BOOL FinishLinkFlat(CLDSNode *pStartNode, CLDSNode *pEndNode);
	BOOL FinishDesignCirRibPlate(CLDSPart *pDatumPart,CLDSNode *pBaseNode,CLDSPlate *pFirstDatumPlate,CLDSPlate *pSecondDatumPlate,double manu_space=0);
	BOOL FinishDesignCirCoverPlate(CLDSNode *pNode,CLDSLineTube *pLineTube,CLDSPlate *pPlate1,CLDSPlate *pPlate2,double manu_space=0);
	BOOL FinishDesignTubeRibPlate(CLDSNode *pNode,CLDSLineTube *pLineTube,CLDSPlate *pPlate1,CLDSPart *pPart2,CLDSLinePart *pAxisXRefLinePart,double manu_space=0);
	BOOL FinishDesignWaterShieldedPlate(CLDSNode *pBaseNode,CLDSLineTube *pDatumTube,CLDSPart *pPlate);
	BOOL FinishTurnAngleClockwise(CLDSLineAngle *pLineAngle);
	BOOL FinishTurnAngleAnticlockwise(CLDSLineAngle *pLineAngle);
	BOOL FinishTurnAngle(CLDSLineAngle *pLineAngle,int nTurnType);//0Ϊ��������ת1Ϊ����֫��ת
	BOOL FinishJgOpenCut(CLDSLineAngle* pLineAngle, CLDSFitting* pFitting);
	long FinishCutPlate(CLDSPlate *pSrcPlate,CLDSPart *pDatumPart,CCutPlateOptionDlg &cut_plate_dlg,BOOL bReCalCutFace);
	CLDSParamPlate* DesignLineAngleRibPlate(CLDSLineAngle *pLineAngle,CLDSDbObject *pObj);			//��ƽǸ��߰�
	CLDSParamPlate* DesignSimpleRibPlateOnRod(CLDSLinePart* pDatumRod,CLDSNode* pDatumNode,CLDSBolt* pDatumBolt);
	CLDSParamPlate* DesignSimpleRibPlateOnPlate(CLDSGeneralPlate* pDatumPlate);
	CLDSParamPlate* DesignElbowRibPlate(CLDSLineTube* pDatumTube,SmartPlatePtr pDatumPlate);
	CLDSParamPlate* DesignCornerRibPlate(SmartPlatePtr pDatumPlate,SmartPlatePtr pSidePlate);
	CLDSParamPlate* DesignSlotRibPlate(SmartPartPtr pDatumPart,SmartPlatePtr pSidePlate1,SmartPlatePtr pSidePlate2);
	CLDSGeneralPlate* RibPlateClassifyDesign(SmartPartPtr pDatumPart,SmartPartPtr pRelaPart1,SmartPartPtr pRelaPart2,CLDSNode* pDatumNode);
	CLDSGeneralPlate* DesignTubeBetweenPlate(CLDSLineTube *pDatumTube1,CLDSLineTube *pDatumTube2);	//��Ƹֹܼа�
	//�������
	CLDSParamPlate* DesignSimpleCirRibPlate(CLDSLineTube* pDatumTube,CLDSNode* pDatumNode);
	CLDSParamPlate* DesignWaterShieldedPlate(CLDSLineTube* pDatumTube,CLDSNode* pDatumNode,CLDSParamPlate* pFl);
	CLDSParamPlate* DesignCircularCoverPlate(CLDSLineTube* pDatumTube,CLDSNode* pDatumNode,CLDSPlate* pRelaPlate1,CLDSPlate* pRelaPlate2=NULL);
	CLDSParamPlate* DesignCircularRibPlate(SmartPartPtr pDatumPart,CLDSNode* pDatumNode,CLDSGeneralPlate* pRelaPlate,SmartPartPtr pRelaPart2);
#endif
	BOOL FinishBreakLinePart(CSuperSmartPtr<CLDSLinePart> pLinePart,CNodeList *pNodeList,LINEPARTSET &partset);
	CLDSLineAngle* ConvertToAngle(CLDSLineTube *pTube);
#ifdef __GGG_
	BOOL FinishBreakConeTube(CLDSConeTube* pLinePart,CNodeList *pNodeList,CONETUBESET &partset);
#endif
private:
	UINT m_nPrevCommandID;		//���ڼ�¼ǰһ��ִ�������ID
	CString m_sPrevCommandName;	//���ڼ�¼ǰһ��ִ�����������
	CString m_sAcadPath;		//AutoCADִ��·��
	CPoint m_ptLBDownPos;
	BOOL m_bStartOpenWndSelTest;
	BOOL m_bEnableOpenWndSelectTest;
	long m_nMaxY;
	long m_nMaxX;
	::CEvent FuncRun;
	CLDSDoc* m_pDoc;
	CSingleLock *pFuncStatus;
	DBOBJSET m_setSnapObjects;
	CArray<fAtom*,fAtom*>m_snap_feat_atom_arr;

	BEGIN_INTERFACE_PART(EntPickEventSink, IDispatch)
		INIT_INTERFACE_PART(CLDSView, EntSelEventSink)
		STDMETHOD(GetTypeInfoCount)(unsigned int*);
		STDMETHOD(GetTypeInfo)(unsigned int, LCID, ITypeInfo**);
		STDMETHOD(GetIDsOfNames)(REFIID, LPOLESTR*, unsigned int, LCID, DISPID*);
		STDMETHOD(Invoke)(DISPID, REFIID, LCID, unsigned short, DISPPARAMS*,
						  VARIANT*, EXCEPINFO*, unsigned int*);
	END_INTERFACE_PART(EntPickEventSink)
private:
	DWORD m_dwCookie;
public:
	void VibrationAnimate(double phasePositionCoef);
	int CalDistOfSolid();
	int DistMeasureOnDispLine();
	int DistMeasure();
	int CalAngle();
	void SaveCursorView();
	void EmptySnapObjects(){m_setSnapObjects.Empty();}	//��գ���ֹRedo/Undo�Ȳ�������ָ��ʧЧ�����쳣��wjh-2018.1.3
	//CSuperDataSource m_dataSource;
	COleDataSource m_dataSource;
	int DefControlNode();
	int DefOffsetNode();
	int DefIntersNode();
	int DefHorizonNode();
	int DefFixCoordNode();
	void DefAllIntersNode();
	int RenderMode();
	BOOL BreakLinePart();
#ifdef __PART_DESIGN_INC_
	int TtaFile();
	int TowerFile();
#endif
#ifdef __COMMON_PART_INC_
	int StdComTmpl();
#endif
#ifdef __GGG_
	BOOL BreakConeTube();
#endif
	BOOL DivideMainLinePart();
	BOOL FindPart();
	BOOL QuickFindPart(const char* sSearchText,DBOBJSET* pFindedObjSet=NULL);
	BOOL DeletePart();
	BOOL HidePart();
	int ConnectionPlate();
	void DisplayDbObjectProperty(BOOL bRebuild);
	void DisplayNodeProperty(BOOL bRebuild);
	void DisplayLinePartProperty(BOOL bRebuild);
	void DisplayFemElementProperty(BOOL bRebuild);
	void LdsWorkEnvShift(WORKENV_TYPE env_type);
#ifndef __TSA_
	BOOL ComposeLinePart();
	BOOL DelSpecClsPart();
	int  RotatePartAroundAxis();
	BOOL DisplayPartWorkUcs();
	int  InsertPlateTubeInters();
	CLDSPlate* ConvertParamPlateToPlate(CLDSParamPlate *pParamPlate);
	int  MergeMultiNodes();
	int  MergeNodes();
#ifdef __ANGLE_PART_INC_
	int  OffsetRayRodEndNode(LINEPARTSET& linePartSet);	//�ֶ�ƫ�Ʋ�����߸˼��˽ڵ�
	int  BranchNodes();
	int  LabelInput();
	int  WasherRevision();
	int  StdPanel();
	int  OpenCutAngle();
#endif
	int  DataOperation();
	int  MergePlates();
	CLDSPlate* MergePlates(CLDSPlate *pFirstPlate,CLDSPlate *pSecondPlate);
	int  BendPlate(); //
	int  DesignSphere();
	int  CheckModel();	//У��ѡ�й���
	int  JgKeyPoint();	//�鿴�ؼ���
	int  AddLegAndBraceRods();
	int  AddGtmBraceElemPanels();
	//ͨ��ָ���ĸְ��и�ְ�(�Զ��жϸְ��и���) 1.˫����и��� 2.˫����и�˫��� wht 11-01-20
	BOOL CutPlateByPlate(CLDSPlate *pPlate,CLDSPlate *pDatumPlate,BOOL bTopFace,double fMinDist,BOOL bPrompt=TRUE,BOOL bSetWeldEdge=TRUE);
	BOOL CutPlateByPlane(CLDSPlate *pPlate,f3dPoint face_pos,f3dPoint face_norm,double fMinDist=0,BOOL bPrompt=TRUE,BOOL bSetWeldEdge=TRUE);
	BOOL ExtendPlateToPlane(CLDSPlate *pPlate,f3dPoint face_pos,f3dPoint face_norm,
							f3dPoint *pExtendVec=NULL,double fExtendLen=NULL,CLDSPart *pWeldFatherPart=NULL);
	int CutPlate();
	int DesignElbowPlate();
	int OverlappendPlate();
	int BoltPad();
	CLDSPlate* DesignBoltPad(CSuperSmartPtr<CLDSPart>pDatumPart,CLDSLineAngle *pCutAngle,BOLTSET &boltSet,double fPlatethick=-1,double fNormoffset=-1);
	BOOL CutBoltPadByAngle(CLDSPlate* pBoltPad,CLDSLineAngle* pCutAngle=NULL);
	int RibPlate();
	int NewArcPart();
	int CircularPlate();
	int DesignColumnRayRod();
	BOOL FastCirPlate(int plate_type);
	int  NewProcessHole();
	BOOL FindBoltRelationPart();
	BOOL ReplaceBoltDatumPoint();
	BOOL AlignBoltsAlongLenVec();
	void DisplayPartProperty(BOOL bRebuild);
	void DisplayLineAngleProperty(BOOL bRebuild);
	void DisplayLineAnglePositionSubSectionProperty(int iStartSectionType,int iEndSectionType);
	void DisplayLineAngleDesignSubSectionProperty(int iSectionType);
	void DisplayTubeProperty(BOOL bRebuild);//TMA���׹�Ҳ���õ��ֹܡ�wjh-2016.1.08
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
	void DisplayConeTubeProperty(BOOL bRebuild);
	void DisplaySlotArmProperty(BOOL bRebuild);
#endif
	void DisplayPlateProperty(BOOL bRebuild);
	void DisplayPlateNeedlessRebuildProperty();
	void DisplaySphereProperty(BOOL bRebuild);
	void DisplayParamPlateProperty(BOOL bRebuild);
	void DisplayBoltProperty(BOOL bRebuild);
	void DisplayLineSlotProperty(BOOL bRebuild);
	void DisplayLineFlatProperty(BOOL bRebuild);
#ifdef __H_SHAPE_STEEL_
	void DisplayLineHShapeProperty(BOOL bRebuild);
#endif
	void DisplayArcPartProperty(BOOL bRebuild);
	void DisplayArcAngleProperty(BOOL bRebuild);
	void DisplayArcFlatProperty(BOOL bRebuild);
	void DisplayArcSlotProperty(BOOL bRebuild);
	void DisplayPolyJointProperty(BOOL bRebuild);	//�������ε�
	void SolidCollideCheck(SCOPE_STRU *pScope=NULL);
	void DisplayBlockModelProperty(BOOL bRebuild);
	void DisplayBlockReferenceProperty(BOOL bRebuild);
	void DisplayProfileVertexProperty(BOOL bRebuild,CLDSGeneralPlate* pBelongPlate=NULL);
	void DisplayWirePointProperty(BOOL bRebuild);
	void DisplayFittingProperty(BOOL bRebuild);
	void DisplayAnchorBoltProperty(BOOL bRebuild);
	void DisplayFoundationProperty(BOOL bRebuild);
	void DisplayHoleWallProperty(BOOL bRebuild);
#endif
	void DisplayWorkViewProperty();
	void DisplaySysSettingProperty(BOOL bRebuild);
	void DisplaySysSettingDisplaySubSectionProperty(int iSectionType);
	void DisplaySysSettingDesignSubSectionProperty(int iSectionType);
	void DisplaySysSettingDesignCodeSubSectionProperty(int iSectionType);
	void DisplaySysSettingDrawingSubSectionProperty(int iSectionType);
	void UpdatePropertyPage(CLDSObject *pObj=NULL,BOOL bFilterDisplay=FALSE,HWND hFocusWnd=NULL);
	int ShrinkObjHandle();	//���������
	int CorrectNodeFatherRodCfgWord();	//�����ڵ㸸�˼���ĺ�
	int MultiLegSchema();	//ת������ģ�͵ĺ��߽�����ĺ�ռλģʽ
	int CopyLeg();
	int NewManuTowerModel();	//�½���������ģ��
	int ShareJdbDesign();
	int ConnectFromTemplate();
	int ExportConnectionTemplate();
	int ExportConnecTempData(BYTE ciMode=0);
	int ExportFootTemplate();	//��������ģ��
	int DesignFoot();			//�������
	int DesignOldFoot();		//��ƾ�ʽ����
	int DesignHoof();			//���ţ���
	int DesignBasePlate();		//��Ƶװ�
	int DesignShoePlate();		//���ѥ��
	int DesignFitting();		//��ƽ��
	int NewDesignTowerModel();	//�½��������ģ��
	int NewSubstationModel();	//�½���繹��ģ��
	int NewFile3();
	void InitSysCmdList();
	BOOL Command(CString cmdStr);
	int DesignOneBolt();				//��Ƶ���˨����
	int DesignSpecNodeOneBolt();		//������Ƶ���˨����
	void DesignTubeEndLj(char cType);	//�ֹܶ�����
	int DesignTubeEndLjTypeD();	//�ֹܶ�����
	int DesignTubeEndLjTypeO();	//����
	int DesignTubeEndLjTypeU();	//U�Ͳ��
	int DesignTubeEndLjTypeC();	//C�Ͳ��
	int DesignTubeEndLjTypeX();	//X�ֹܶ�����
	int DesignTubeEndLjTypeTJ();//�������
	int FilletPlateCorner();	//�ְ嵹��
	int InsertPlankVertex();	//����ְ�������
	int InsertPlankBolt();		//����ְ���˨��
	int DeletePlankFeature();	//ɾ���ְ���������(�����㡢��˨��)
	void DesignSpecialFl(BOOL bDesignSlope=FALSE);	//������ⷨ��
	int DesignSimpleLadder();
	int DesignTubeWalkWay();
	int DesignTubeSafeHolder();
	int MigrateSlotNailParams();
	void ExplodeBlockRef(CBlockReference *pBlockRef);
	bool IntegrateBlockRef(CBlockReference *pBlockRef);
	BOOL FinishRetrieveRelaMirObject(CLDSDbObject* pSrcObject);
	void CheckRelativeMirInfo(CLDSDbObject *pObject,int* bad_hits=NULL,CTaskOutputList* pInfoList=NULL);
	BOOL EmendMirRodInfo(CLDSDbObject* pObject);
	//���ݾ������ģ�ͳ����Դ���ʾУ�� wjh-2017.1.20
	int RoutingWarningCheck();
	int MechanicsCheck();
	int OptimizePlate();
#ifdef __COMMON_PART_INC_
	int AddAnchorBolts();
	int CutOpenByFitting();
#endif
	//���Թ���
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static CHashSet<PROPLIST_ITEM*>hashPropItemById;
	static CXhChar100 HELPLINK_URL;
	UPDATE_PROPLISTITEM_FUNC(CLDSView,propHashtable)  //������������º���
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	CString GetPropValueStr(long id);		//��������ID�õ�����ֵ
	static void InitWorkViewPropHashtable();
	static long GetPropID(char* propStr);	//�õ�����ID
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
public:	//��̬���ߺ���
	static void RedrawAngleFrameDrawing(CLDSLineAngle* pAngle,BYTE cbDrawMode=0,UCS_STRU *pObjectCS=NULL,bool removeOldEntities=true);
	static void RedrawPlateProfileDrawing(CLDSGeneralPlate* pPlate,UCS_STRU *pObjectCS=NULL,bool removeOldEntities=true,BYTE cbMode=0x03);
	static void RedrawFlatFrameDrawing(CLDSLineFlat* pFlat,BYTE cbDrawMode=0,UCS_STRU *pObjectCS=NULL,bool removeOldEntities=true);
	static void RedrawSlotFrameDrawing(CLDSLineSlot* pSlot,BYTE cbDrawMode=0,UCS_STRU *pObjectCS=NULL,bool removeOldEntities=true);
	static void AppendSizeCoordCheckItem(void* pCoordSize);	//CDataCheckModel::SIZE_DATA
	//
	static IDbLine* AppendDbLine(f3dLine line,PEN_STRU pen,HIBERID hiberId,f3dPoint up_vec,bool bSelect=false);
	//����Ϊ�ڲ����Թ���ʱ����ʱִ�к���
	int InternalTest();
	//����Ϊ�ɺ����������´���ԱȲ���
#ifdef __ALFA_TEST_VERSION_
public:
	int Test();
#endif
	//��ʾ��
	static CSysPromptObject prompt;
	class PROMPT_LIFE{
		bool m_bDisableAutoCloseOnUse;
		bool m_bAutoClose;
	public:
		PROMPT_LIFE(bool disableAutoCloseOnUse=false);
		~PROMPT_LIFE();
		void Prompt(const char* promptstr);
		void SetPromptPicture(long bitmapId);
		void SetPromptPicture(const char* bmpfile);
		void DisableAutoClose(){m_bAutoClose=false;}
	};
	afx_msg void OnSizeDimension();
	afx_msg void OnAlignBoltAlongLenvec();
#if defined(__RAPID_ROUGH_3DMODEL_)
	afx_msg void OnTrunkModelTree();
	afx_msg void OnIntelliRelayoutRodModel();
	afx_msg void OnSetFoundationParams();
	afx_msg void OnGenerateRough3DModel();
	afx_msg void OnUpdateBaseFootInfo();
#endif
#ifdef __COMMON_PART_INC_
	afx_msg void OnAddAnchorBolts();
	afx_msg void OnCutOpenByFitting();
#endif
};
class CCmdLockObject{
	UINT m_uiLockCount;
	CLDSView* m_pView;
public:
	CCmdLockObject(CLDSView* pView,BOOL bPromptMsg=TRUE){
		m_pView=pView;
		if(pView->LockFunc(bPromptMsg))
			m_uiLockCount=1;
		else
			m_uiLockCount=0;
	}
	bool LockSuccessed(){return m_uiLockCount>0;}
	~CCmdLockObject(){
		if(m_uiLockCount>0)
			m_pView->ReleaseFunc();
	}
	bool ManualUnlock(){
		if(!LockSuccessed())
			return false;
		m_pView->ReleaseFunc();
		m_uiLockCount=0;
		return true;
	};
};
struct DRAWCONTEXT{
protected:
	short idContextClsType;
public:
	static const short VIEW_LDS = 0;	//pContextָ��CLDSView
	static const short VIEW_ENV = 1;	//pContext��OBJECT_UCS
	virtual short ContextClsType(){return idContextClsType;}
};
struct OBJECTCS_CONTEXT : public DRAWCONTEXT{
	UCS_STRU object_ucs;
	OBJECTCS_CONTEXT(UCS_STRU* pObjectCS=NULL)
	{
		idContextClsType=VIEW_ENV;
		if(pObjectCS)
			object_ucs=*pObjectCS;
		else
			LoadDefaultUCS(&object_ucs);
	}
};
struct LDSVIEW_CONTEXT : public DRAWCONTEXT{
	CLDSView* pView;
	LDSVIEW_CONTEXT(CLDSView* pLdsView=NULL){
		idContextClsType=VIEW_LDS;
		pView=pLdsView;
	}
};
#ifndef _DEBUG  // debug version in LDSView.cpp
inline CLDSDoc* CLDSView::GetDocument()
   { return (CLDSDoc*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LDSVIEW_H__20AB67FD_535B_4270_B4FD_28BEF9A3A45A__INCLUDED_)
