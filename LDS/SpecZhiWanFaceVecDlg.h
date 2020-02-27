#pragma once


// CSpecZhiWanFaceVecDlg 对话框

class CSpecZhiWanFaceVecDlg : public CDialog
{
	DECLARE_DYNAMIC(CSpecZhiWanFaceVecDlg)

public:
	CSpecZhiWanFaceVecDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSpecZhiWanFaceVecDlg();

// 对话框数据
	//{{AFX_DATA(CRollPlankEdgeDlg)
	enum { IDD = IDD_SPEC_ZHIWAN_FACE_VEC_DLG };
	// 制弯面法线X向分量
	double m_fNormX;
	// 制弯面法线Y向分量
	double m_fNormY;
	// 制弯面法线Z向分量
	double m_fNormZ;
	// 制弯线方向X分量
	double m_fVectorX;
	// 制弯线方向Y分量
	double m_fVectorY;
	// 制弯线方向Z分量
	double m_fVectorZ;
	//}}AFX_DATA

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnPasteNorm();
	afx_msg void OnBnPasteVec();
	afx_msg void OnBnClickedOk();
};
