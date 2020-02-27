#pragma once


// CSpecZhiWanFaceVecDlg �Ի���

class CSpecZhiWanFaceVecDlg : public CDialog
{
	DECLARE_DYNAMIC(CSpecZhiWanFaceVecDlg)

public:
	CSpecZhiWanFaceVecDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSpecZhiWanFaceVecDlg();

// �Ի�������
	//{{AFX_DATA(CRollPlankEdgeDlg)
	enum { IDD = IDD_SPEC_ZHIWAN_FACE_VEC_DLG };
	// �����淨��X�����
	double m_fNormX;
	// �����淨��Y�����
	double m_fNormY;
	// �����淨��Z�����
	double m_fNormZ;
	// �����߷���X����
	double m_fVectorX;
	// �����߷���Y����
	double m_fVectorY;
	// �����߷���Z����
	double m_fVectorZ;
	//}}AFX_DATA

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnPasteNorm();
	afx_msg void OnBnPasteVec();
	afx_msg void OnBnClickedOk();
};
