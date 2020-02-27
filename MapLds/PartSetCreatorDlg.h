#if !defined(AFX_PARTSETCREATORDLG_H__3FD092D2_B330_4534_8B08_9CFCA55A6962__INCLUDED_)
#define AFX_PARTSETCREATORDLG_H__3FD092D2_B330_4534_8B08_9CFCA55A6962__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartSetCreatorDlg.h : header file
//
#include "resource.h"
#include "ListViewComparer.h"
#include "ProcessPart.h"
#include "MemberProperty.h"
#include "SegI.h"
/////////////////////////////////////////////////////////////////////////////
// CPartSetCreatorDlg dialog
#ifdef __COMMON_PART_INC_
class CPartSetCreatorDlg : public CDialog
{
// Construction
	POLYPARTLIST polypartlist;
	CStringKeyHashList<DWORD> m_xItemPtrHashPartNo;
	CListViewComparer listViewComparer;
	CSuperHashList<CProcessPart> hashProcessParts;//记录导入的中性文件
	CSuperHashStrList<CProcessPart> hashProcessPartByLabel;	//记录从TMA附加缓存中读取的CProcessPart
	CHashStrList<CProcessAngle> hashProcessAngleBySrcLabel;	//记录根据带脚钉角钢生成的无脚钉角钢
	void AddProcessPart();//添加中性文件构件
	void AddNewProcessAngle(int iInsertPos=-1);		//去除脚钉之后新生成的角钢
	void AddPartset(IModel *pModel,CHashList<SEGI> &segTbl,CBlockModel *pEmbededBlock=NULL,CBlockReference *pBlockRef=NULL);
	void AddBlockRefsToPartSet(CHashList<SEGI> &segTbl);
	void ConfirmCreate(bool bOnlySelecteItems);
	BOOL GetSelectOnlyAngleOrPlate();   //  判断是否只选择角钢或则钢板
public:
	static int PATTERN_DRAW_SCALE;
	static int PART_DRAW_SCALE;
	struct STDPART_ITEM{
	private:
		CLDSPart *_pPart;
		CProcessPart *_pProcessPart;
	public:
		CProcessAngle m_xReverseAngle;		//反转后的角钢
	public:
		int num;
		BOOL bFlag;	//标志位,目前仅在更新钢板构件是使用 wht 10-11-12
		BOOL bReverseAngle;
		CXhChar16 sLabelWithOutNail;		//要生成的不带脚钉角钢的件号 wht 16-05-27
		STDPART_ITEM(){_pPart=NULL;_pProcessPart=NULL;bReverseAngle=FALSE;}
		STDPART_ITEM::STDPART_ITEM(const STDPART_ITEM &srcItem)
		{
			Clone(srcItem);
		}
		STDPART_ITEM& STDPART_ITEM::operator =(const STDPART_ITEM &srcItem)
		{
			Clone(srcItem);
			return *this;
		}
		void STDPART_ITEM::Clone(const STDPART_ITEM &srcItem)
		{
			_pPart=srcItem._pPart;
			_pProcessPart=srcItem._pProcessPart;
		    num=srcItem.num;
			bFlag=srcItem.bFlag;
			bReverseAngle=srcItem.bReverseAngle;
			CBuffer buffer;
			(const_cast<STDPART_ITEM*>(&srcItem))->m_xReverseAngle.ToPPIBuffer(buffer);
			buffer.SeekToBegin();
			m_xReverseAngle.FromPPIBuffer(buffer);
		}
		static const BYTE TYPE_LINEANGLE	= 1;	//角钢
		static const BYTE TYPE_BOLT			= 2;	//螺栓
		static const BYTE TYPE_PLATE		= 3;	//钢板
		static const BYTE TYPE_LINETUBE		= 4;	//钢管
		static const BYTE TYPE_LINEFLAT		= 5;	//扁铁
		static const BYTE TYPE_LINESLOT		= 6;	//槽钢
		static const BYTE TYPE_SPHERE		= 7;	//球壳
		DECLARE_READONLY_PROPERTY(BYTE,m_cPartType)
		{
			if(_pPart)
			{
				if(_pPart->GetClassTypeId()==CLS_LINEANGLE||_pPart->GetClassTypeId()==CLS_POLYPART)
					return TYPE_LINEANGLE;
				else if(_pPart->GetClassTypeId()==CLS_LINETUBE)
					return TYPE_LINETUBE;
				else if(_pPart->GetClassTypeId()==CLS_LINEFLAT)
					return TYPE_LINEFLAT;
				else if(_pPart->GetClassTypeId()==CLS_LINESLOT)
					return TYPE_LINESLOT;
				else if(_pPart->GetClassTypeId()==CLS_PLATE||_pPart->GetClassTypeId()==CLS_PARAMPLATE)
					return TYPE_PLATE;
			}
			else if(_pProcessPart)
				return _pProcessPart->m_cPartType;
			return 0;
		}
		CLDSPart* GetPart()
		{
			if(_pPart)
			{
				if(bReverseAngle&&_pPart->IsAngle())
					return NULL;
				else
					return _pPart->MotherPart();
			}
			else
				return NULL;
		}
		CProcessPart* GetProcessPart()
		{
			if( bReverseAngle&&
				((_pPart&&_pPart->IsAngle())||
				(_pProcessPart&&_pProcessPart->IsAngle())))
				return &m_xReverseAngle;
			else
				return _pProcessPart;
		}
		void SetPart(CLDSPart* pPart)
		{
			_pPart=pPart;
			_pProcessPart=NULL;
		}
		void SetProcessPart(CProcessPart* pProcessPart)
		{
			_pPart=NULL;
			_pProcessPart=pProcessPart;
		}
		CXhChar16 GetPartNo()
		{
			if(GetProcessPart())
				return GetProcessPart()->GetPartNo();
			else if(GetPart())
				return GetPart()->GetPartNo();
			else
				return CXhChar16();
		}
		SEGI GetSegI()
		{
			if(GetProcessPart())
				return GetProcessPart()->GetSegI();
			else if(GetPart())
				return GetPart()->iSeg;
			else
				return SEGI(""); //错误
		}
	};
	ATOM_LIST<STDPART_ITEM> partList;
	CPartSetCreatorDlg(CWnd* pParent = NULL);   // standard constructor
	CStringArray m_arrHeaderLabel;
	CListCtrl *m_pListCtrl;
	BOOL m_bBatchPlot;	//是否为批量生成
	void RefreshListViewHeader();
	int AppendRecord(int iItem, CStringArray &str_arr);
	BOOL RebuildPartset();
// Dialog Data
	//{{AFX_DATA(CPartSetCreatorDlg)
	enum { IDD = IDD_PART_SET_CREATOR_DLG };
	BOOL	m_bIncAngle;
	BOOL	m_bIncPlate;
	BOOL	m_bIncTransectTube;
	CString	m_sSegStr;
	CString	m_sPartNo;
	CString	m_sDrawScale;
	BOOL	m_bIncStdPart;
	int		m_nTaNum;
	BOOL	m_bIncTube;
	BOOL	m_bCreateTubeBrief;
	BOOL	m_bAutoArrangePlate;
	CString	m_sPartNum;
	BOOL	m_bIncSlot;
	BOOL	m_bAutoCreateFile;
	BOOL	m_bCreateJgCutAngleBriefMap;
	BOOL    m_bIncArcPart;
	BOOL	m_bProcessPart;
	BOOL	m_bPatternDrawing;
private:
	BOOL    _bGroupBySeg;
	BOOL    _bOneSegOneFile;
public:
	DECLARE_PROPERTY(BOOL,m_bGroupBySeg)
	DECLARE_PROPERTY(BOOL,m_bOneSegOneFile)
	CString m_sCreate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartSetCreatorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartSetCreatorDlg)
	afx_msg void OnBnRebuildPartset();
	virtual BOOL OnInitDialog();
	afx_msg void OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdatePartType();
	afx_msg void OnIncTransectTube();
	afx_msg void OnIncTube();
	afx_msg void OnCreateTubeBrief();
	afx_msg void OnAutoArrangePlate();
	afx_msg void OnAddProcessPartFile();
	afx_msg void OnBnClickedChkPatternDraw();
	afx_msg void OnRclickCmdListCommon(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedChkGroupBySegi();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnCreateSelectedItems();
	afx_msg void OnReverseAngle();
	afx_msg void OnNewItem();

};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTSETCREATORDLG_H__3FD092D2_B330_4534_8B08_9CFCA55A6962__INCLUDED_)
