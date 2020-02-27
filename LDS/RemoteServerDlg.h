#pragma once
#include "TowerTreeCtrl.h"
#include "env_def.h"

// CRemoteServerDlg 对话框

#ifdef __CONNECT_REMOTE_SERVER_
class CServerTowerPart;
class CServerTowerFile;
class CServerTowerFileGroup;
class CRemoteServerDlg : public CDialogEx
{
	static const int IMG_MACHINE_SERVER		= 4;
	static const int IMG_MACHINE_SERVER_OFFLINE	= 5;
	static const int IMG_MACHINE_LOCAL		= 6;
	static const int IMG_TOWER_TYPE			= 7;
	static const int IMG_FILE_GROUP			= 8;
	static const int IMG_TOWER_FILE			= 9;
	static const int IMG_TOWERPARTSET		=10;  //全部构件集
	static const int IMG_TOWERPARTSET_SYNC  =11;  //需同步构件集
	static const int IMG_TOWERPART_SEGMENT	=12;  //构件分段
	static const int IMG_ANGLEPART			=13;  //构件记录(已同步)
	static const int IMG_ANGLEPART_NEW		=14;  //构件记录(需同步)
	static const int IMG_ANGLEPART_SYNCING	=15;  //构件记录(需同步)
	static const int IMG_ANGLEPART_UPLOAD	=16;  //构件记录(需上传)
	static const int IMG_ANGLEPART_DOWNLOAD	=17;  //构件记录(需下载)
	static const int IMG_PLATEPART			=18;  //构件记录(本地模型实时更新的工件)
	static const int IMG_PLATEPART_NEW		=19;  //构件记录(本地模型实时更新的工件)
	static const int IMG_PLATEPART_SYNCING	=20;  //构件记录(本地模型实时更新的工件)
	static const int IMG_PLATEPART_UPLOAD	=21;  //构件记录(上传)
	static const int IMG_PLATEPART_DOWNLOAD	=22;  //构件记录(下载)
	static const int IMG_PARTSET_UPLOAD		=24;  //待上传集合
	static const int IMG_PARTSET_DOWNLOAD	=25;  //待下载集合
public:
	static const int SERVER_CONTENT				= 1;  //文件
	static const int SERVER_TOWERTYPESET1		= 2;  //在放样塔型
	static const int SERVER_TOWERTYPESET2		= 3;  //在试组塔型
	static const int SERVER_TOWERTYPESET3		= 4;  //存档塔型
	static const int SERVER_TOWERFILE			= 5;  //文件
	static const int SERVER_TOWERFILEGROUP		= 6;  //文件组
	static const int SERVER_TOWERFILEASSEMBLY	= 7;  //构件记录
	static const int SERVER_TOWERPARTSET		= 8;  //构件集
	static const int SERVER_TOWERPARTSET_SYNC	= 9;  //构件集(需同步)
	static const int SERVER_TOWERPART_SEGMENT	=10;  //构件分段
	static const int SERVER_TOWERPART			=11;  //构件记录
	static const int SERVER_TOWERPART_SYNCING	=12;  //构件记录（需同步）
	static const int SERVER_PARTSET_UPLOAD		=13;  //构件集（需上传）
	static const int SERVER_PARTSET_DOWNLOAD	=14;  //构件集（需下载）
	static const int SERVER_PARTSET_JUSTIFY		=15;  //构件集（待判断）
	static const int SERVER_TOWERTYPE			=16;  //塔型
	//
	CHashStrList<CServerTowerPart> hashProcessParts;
	CHashStrList<CServerTowerPart> hashSyncTowerParts;
	CServerTowerFile* m_pServerTowerFile;
	CServerTowerFileGroup* m_pTowerFileGroup;
	DECLARE_DYNCREATE(CRemoteServerDlg)
public:
	CRemoteServerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRemoteServerDlg();
// 对话框数据
	enum { IDD = IDD_REMOTE_SERVER_DLG };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
private:
	ATOM_LIST<TREEITEM_INFO>itemInfoList;
	HTREEITEM m_hServerContentItem;
	HTREEITEM m_hPartSetItem,m_hSyncPartSetItem;
	CImageList m_ModelImages;
	void DeleteItem(HTREEITEM hItem);
	CServerTowerFile* SaveOrUpdateTowerFile(HTREEITEM hItem,TREEITEM_INFO *pItemInfo,const char* sOriginalFileName,const char *sFileNotes,BOOL bDisplayDlg);
public:
	CTowerTreeCtrl m_treeCtrl;
	virtual BOOL OnInitDialog();
	CTowerTreeCtrl* GetTreeCtrl();
public:
	void RefreshServerContent(HTREEITEM hTowerTypeGroupItem);
	void RefreshProcessPartSet(HTREEITEM hPartSetItem);
	void RefreshSyncPartSet(HTREEITEM hPartSetItem);
	void ContextMenu(CWnd *pWnd, CPoint point);
	void RefreshTreeCtrl();
	void RetrievedSyncPartSet();
	void RetrievedPartSet();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDblclkTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickTreeCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnConnectServer();
	afx_msg void OnDisconnectServer();
	afx_msg void OnDownloadItem();
	afx_msg void OnUploadItem();
	afx_msg void OnMappingWithCurrentModel();
	afx_msg void OnSaveAsTowerFile();
	afx_msg void OnNewItem();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDeleteItem();
	afx_msg void OnEditItemPropItem();
	afx_msg void OnSetActiveItem(); 
	afx_msg void OnTvnItemexpandingTreeCtrl(NMHDR *pNMHDR, LRESULT *pResult);
};
#endif