// LDSDoc.h : interface of the CLDSDoc class
//
/////////////////////////////////////////////////////////////////////////////

//#if !defined(AFX_LDSDOC_H__2B6108E5_011C_475F_8139_2A16659F8B4E__INCLUDED_)
//#define AFX_LDSDOC_H__2B6108E5_011C_475F_8139_2A16659F8B4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "TowerType.h"
class CLDSDoc : public CDocument
{
	bool m_bStartAutoSaveThread;
protected: // create from serialization only
	CLDSDoc();
	DECLARE_DYNCREATE(CLDSDoc)

// Attributes
public:
	UINT m_nTimer;
	long m_idServerFile;
	int m_iMaxBakSerial;//当前的文件存储序号 默认值为0
// Operations
public:
	CView* GetView(const CRuntimeClass* pClass);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLDSDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLDSDoc();
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	void AddPartGroupSumMatTbl(CTowerType &mat_tbl,LPDISPATCH pSheet,BOOL bDispSpecPart=FALSE);
	void AddGuangZhouZengLiPartGroupSumMatTbl(CTowerType &mat_tbl,LPDISPATCH pSheet);
	void AddWeiFangChangAnPartGroupSumMatTbl(CTowerType &mat_tbl,LPDISPATCH pSheet); //潍坊长安组焊明细表 wht 10-09-30
	void AddLsSumMatTbl(CTowerType &mat_tbl, LPDISPATCH pSheet,int iStyle);//,BOOL bDispSpecPart=FALSE);
	void AddLsSumMatTblBySeg(CTowerType &mat_tbl, LPDISPATCH pSheet);	//按段统计螺栓
	void AddBoltSumTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet);
	void AddCommonPartSpecTblSheet(CTowerType & mat_tbl, LPDISPATCH pSheet, BOOL bOrgMat, CString segStr, LDSPART_TYPE part_type = TA_OTHER, int iPartSortType = 0);
	void AddZheJiangShengDaPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr);
	void AddShanXiJinNengPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr);
	void AddChengDuPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr);
	void AddChangShuPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr);
	void AddGuangZhouZengLiPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr);
	void AddHuZhouFeiJianPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr);
	void AddZheJiangYongDaPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr);
	void AddWeiFangChangAnPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr);
	void AddSlotAndWeldCodeTblSheet(LPDISPATCH pSheet,int nRuleType);	//开槽组焊代号类型表
	void AddPoleTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet);
	//钢管特殊工艺表 
	//nTableType 表格类型:0.平面相贯 1.圆柱相贯 2.一字槽 3.十字槽 4.U型槽 
	//					  5.T型槽 6.两端开槽其中包括U型 7.两端开槽不包括U型槽 
	void AddTubeSpecialProcossSumTbl(CTowerType &mat_tbl,LPDISPATCH pWorkSheet,int nTableType=0);
	void AddWorkPartsSheet(CTowerType &mat_tbl,LPDISPATCH pSheet);
	void AddManuPartSheet(CTowerType &mat_tbl,LPDISPATCH pSheet);
	void AddBoltSheet(CTowerType &mat_tbl,LPDISPATCH pSheet);
	void AddWorkFLSheet(CTowerType &mat_tbl,LPDISPATCH pSheet);
#endif
	void AddManuModuleSheet(LPDISPATCH pSheet);
	void AddBodySheet(LPDISPATCH pSheet);
	void UpdatePermList();
	void UpdateDataFile(){OnUpdateDataFile();}
	bool IsStartAutoSaveThread(){return m_bStartAutoSaveThread;}
	void ResetAutoSaveTimer();
	void AutoSaveBakFile();
	BOOL SaveDataFile();
	void SaveFile(){OnFileSave();}//自动存盘时使用
#ifdef __CONNECT_REMOTE_SERVER_
	void SerializeTowerFileBuffer(CBuffer &buffer,BOOL bStoring);
#endif
	CString m_sDocType;	//仅为测试资源为什么改变用 wjh-2012.4.29
	CString m_sProductVersion;
	CString m_sFileVersion;
	//CString m_sDocType;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
#ifndef __TSA_FILE_
	void ImportTsaFile(LPCTSTR lpszPathName);
#endif
	void ExportTxtSumMatData(LPCTSTR lpszPathName);
	void ExportExcelSumMatData();
#ifdef __SMART_DRAW_INC_
	void CheckModelByCommandDrawFile(LPCTSTR lpszPathName);
	void ImportCommandDrawingFile(LPCTSTR lpszPathName);
#endif
#ifdef __PART_DESIGN_INC_
	void ExportCommandDrawingFile(LPCTSTR lpszPathName);//输出司令图信息文件
#endif
#ifdef __RAPID_ROUGH_3DMODEL_
	void Export3dsFile(LPCTSTR lpszPathName);
	void ExportStlFile(LPCTSTR lpszPathName);
	void ExportGimFile(LPCTSTR lpszPathName);		//输出国网移交文件
#endif


// Generated message map functions
protected:
	//{{AFX_MSG(CLDSDoc)
	afx_msg void OnFileProperty();
	afx_msg void OnReloadFile();
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenInNewWindow();
	afx_msg void OnCheckModelByCmdDrawFile();
	afx_msg void OnUpdateDataFile();
	afx_msg void OnExportExcelFile();
	afx_msg void OnExportSpecPartExcelFile();
	afx_msg void OnExportTxtPartData();
	afx_msg void OnExportTxtLsData();
	afx_msg void OnExportTxtPartData2();
	afx_msg void OnExportTxtPartData3();
	afx_msg void OnExportTxtPartData4();
	afx_msg void OnExportJgExcelFile();
	afx_msg void OnExportTubeSpecialProcessTable();
	afx_msg void OnExportExcelFile2();
	afx_msg void OnExportExcelFile3();
	afx_msg void OnExportSwapInfoFile();
	afx_msg void OnExportFLExcelFile();
	afx_msg void OnFileSaveAs();
	afx_msg void OnReadTtaFile();
	afx_msg void OnTowerGimProperty();
	//afx_msg void OnAddBraceElemDataFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//#endif // !defined(AFX_LDSDOC_H__2B6108E5_011C_475F_8139_2A16659F8B4E__INCLUDED_)
