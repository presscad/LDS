#pragma once
#include "TowerTreeCtrl.h"
#include "env_def.h"

// CRemoteServerDlg �Ի���

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
	static const int IMG_TOWERPARTSET		=10;  //ȫ��������
	static const int IMG_TOWERPARTSET_SYNC  =11;  //��ͬ��������
	static const int IMG_TOWERPART_SEGMENT	=12;  //�����ֶ�
	static const int IMG_ANGLEPART			=13;  //������¼(��ͬ��)
	static const int IMG_ANGLEPART_NEW		=14;  //������¼(��ͬ��)
	static const int IMG_ANGLEPART_SYNCING	=15;  //������¼(��ͬ��)
	static const int IMG_ANGLEPART_UPLOAD	=16;  //������¼(���ϴ�)
	static const int IMG_ANGLEPART_DOWNLOAD	=17;  //������¼(������)
	static const int IMG_PLATEPART			=18;  //������¼(����ģ��ʵʱ���µĹ���)
	static const int IMG_PLATEPART_NEW		=19;  //������¼(����ģ��ʵʱ���µĹ���)
	static const int IMG_PLATEPART_SYNCING	=20;  //������¼(����ģ��ʵʱ���µĹ���)
	static const int IMG_PLATEPART_UPLOAD	=21;  //������¼(�ϴ�)
	static const int IMG_PLATEPART_DOWNLOAD	=22;  //������¼(����)
	static const int IMG_PARTSET_UPLOAD		=24;  //���ϴ�����
	static const int IMG_PARTSET_DOWNLOAD	=25;  //�����ؼ���
public:
	static const int SERVER_CONTENT				= 1;  //�ļ�
	static const int SERVER_TOWERTYPESET1		= 2;  //�ڷ�������
	static const int SERVER_TOWERTYPESET2		= 3;  //����������
	static const int SERVER_TOWERTYPESET3		= 4;  //�浵����
	static const int SERVER_TOWERFILE			= 5;  //�ļ�
	static const int SERVER_TOWERFILEGROUP		= 6;  //�ļ���
	static const int SERVER_TOWERFILEASSEMBLY	= 7;  //������¼
	static const int SERVER_TOWERPARTSET		= 8;  //������
	static const int SERVER_TOWERPARTSET_SYNC	= 9;  //������(��ͬ��)
	static const int SERVER_TOWERPART_SEGMENT	=10;  //�����ֶ�
	static const int SERVER_TOWERPART			=11;  //������¼
	static const int SERVER_TOWERPART_SYNCING	=12;  //������¼����ͬ����
	static const int SERVER_PARTSET_UPLOAD		=13;  //�����������ϴ���
	static const int SERVER_PARTSET_DOWNLOAD	=14;  //�������������أ�
	static const int SERVER_PARTSET_JUSTIFY		=15;  //�����������жϣ�
	static const int SERVER_TOWERTYPE			=16;  //����
	//
	CHashStrList<CServerTowerPart> hashProcessParts;
	CHashStrList<CServerTowerPart> hashSyncTowerParts;
	CServerTowerFile* m_pServerTowerFile;
	CServerTowerFileGroup* m_pTowerFileGroup;
	DECLARE_DYNCREATE(CRemoteServerDlg)
public:
	CRemoteServerDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRemoteServerDlg();
// �Ի�������
	enum { IDD = IDD_REMOTE_SERVER_DLG };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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