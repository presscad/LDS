#pragma once


// CSpreadVectorDlg �Ի���

class CSpreadVectorDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSpreadVectorDlg)
	DYN_ARRAY<GEPOINT> m_arrPlaneVertex;
public:
	CSpreadVectorDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSpreadVectorDlg();

// �Ի�������
	enum { IDD = IDD_SPREAD_VECTOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBnReversalVec();
	virtual BOOL OnInitDialog();
public:
	double m_fVectorX;
	double m_fVectorY;
	double m_fVectorZ;
	void SetPlaneNormal(GEPOINT normal);
	GEPOINT GetPlaneNormal();
	__declspec( property(get=GetPlaneNormal,put=SetPlaneNormal)) GEPOINT vPlaneNormal;
	void InitPlaneVertex(GEPOINT *pVertexArr,int count);
	void InitPlaneVertex(f3dPoint *pVertexArr,int count);
};
