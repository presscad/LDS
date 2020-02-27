#pragma once

// CCheckPartDataDlg 对话框

class CCheckPartDataDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCheckPartDataDlg)
	SmartPartPtr m_pSelPart;
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
public:
	BOOL m_bChkPartNo;
	BOOL m_bChkCount;
	BOOL m_bChkMaterial;
	BOOL m_bChkGuige;
	BOOL m_bChkLength;
	BOOL m_bChkM16Num;
	BOOL m_bChkM20Num;
	BOOL m_bChkM24Num;
	BOOL m_bChkElseNum;
	BOOL m_bChkHole;
	//基本信息
	BYTE m_ciPartType;
	int  m_nCount;			//单基数量
	CString m_sMaterial;	//材质
	CString	m_sGuige;		//规格
	CString m_sLen;			//长度
	CString m_sHandle;		//句柄
	CString m_sPartNo;		//件号
	CString	m_sPartType;	//类型
	//螺栓信息
	int m_nM16;			//M16连接螺栓数量
	int m_nM20;			//M20连接螺栓数量
	int m_nM24;			//M24连接螺栓数量
	int m_nElse;		//其余非常规孔径连接螺栓数量
	int m_nHole;		//特殊功能孔（如挂线孔、安装孔等虚拟螺栓）数量
public:
	CCheckPartDataDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCheckPartDataDlg();
	void SetPart(CLDSPart* pPart);
	void InitSelPartInfo();
	void CreatePartCheckData();
// 对话框数据
	enum { IDD = IDD_CHECK_PART_DATA_DLG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMove(int x, int y);
	afx_msg void OnCbnSelchangeCmbMaterial();
	afx_msg void OnCbnSelchangeCmbGuige();
};
