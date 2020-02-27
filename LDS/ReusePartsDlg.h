////////////////////////////////////////////////////
// CReusePartsDlg 对话框
#include "XhListCtrl.h"
struct REUSE_PART
{
	static const BYTE REUSE_TYPE_COPY	= 0;  //复制
	static const BYTE REUSE_TYPE_SHADOW	= 1;  //影射
	static const BYTE REUSE_TYPE_SHARE	= 2;  //共用
	CLDSPart* m_pPart;
	BYTE m_cReuseType;
	CFGWORD m_destCfgword;
	CXhChar16 GetReuseTypeName();
	REUSE_PART(CLDSPart* pPart=NULL){
		m_pPart=pPart;
		if(pPart)
			m_destCfgword=pPart->cfgword;
		m_cReuseType=0;};
	static CXhChar50 GetCfgWordStr(CFGWORD cfgword,bool bLegObj);
};
class  CReusePartsDlg: public CDialog
{
public:
	ATOM_LIST<REUSE_PART> m_reusePartList;
	DECLARE_DYNAMIC(CReusePartsDlg)
	void SetSelectedItemsReuseType(BYTE cType);
public:
	CReusePartsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CReusePartsDlg();
	CXhListCtrl m_listCtrl;
	BOOL UpdateReusePartList();
	virtual BOOL OnInitDialog();
// 对话框数据  
	enum { IDD = IDD_REUSE_PARTS_DLG };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRclickListReuseParts(NMHDR *pNMHDR, LRESULT *pResult); 
	afx_msg void OnSetToShareType();
	afx_msg void OnSetToCopyType();
	afx_msg void OnSetToShadowType();
	afx_msg void OnSetCfgword();
	afx_msg void OnNMClickListReuseParts(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownListReuseParts(NMHDR *pNMHDR, LRESULT *pResult);
};
