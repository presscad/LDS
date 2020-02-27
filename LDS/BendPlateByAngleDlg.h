#if !defined(AFX_BENDPLATEBYANGLEDLG_H__B405F65B_AF2A_4A0F_92B1_18C705C6119A__INCLUDED_)
#define AFX_BENDPLATEBYANGLEDLG_H__B405F65B_AF2A_4A0F_92B1_18C705C6119A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BendPlateByAngleDlg.h : header file
//
#include "propertylist.h"
#include "PropListItem.h"
#include "XhListCtrl.h"
#include "RememberLocationDlg.h"
//////////////////////////////////////////////////////////////////////////
//DESIGN_GUAXIAN_HOLE
class DESIGN_GUAXIAN_HOLE
{
public: 
	struct GUAXIANHOLE_BASE_INFO	
	{	//���߿���Ϣ
		DWORD serial;			//���
		int hole_d;				//�׾�
		f3dPoint ls_pos;		//���߿�λ��
		double fDistX;			//�������ϵ��Xƫ����
		double fDistY;			//��������µ�Yƫ����
		double fRadius;			//��Բ�뾶
		double lengthPosToEdge;	//���߿��������ߵ�ͶӰλ��
		BOOL bDrillHole;		//���
		//
		BOOL bHasTube;			//�ڳ��׹�
		struct TUBEINFO{
			char sPartNo[16];	//�׹ܱ��
			char guige[16];		//�׹ܹ��
			char cMaterial;		//�׹ܲ���
			TUBEINFO(){
				memset(sPartNo,0,16);
				memset(guige,0,16);
				cMaterial=CSteelMatLibrary::Q235BriefMark();;
			}
		}tube;
		//
		BOOL bWeldCirClePank;	//�⺸����
		struct CIRPLATEINFO{
			char sPartNo[16];	//������
			char cMaterial;		//�������
			double fOffsetDist;	//�߶Ȳ�
			CIRPLATEINFO(){
				memset(sPartNo,0,16);
				cMaterial=CSteelMatLibrary::Q235BriefMark();
				fOffsetDist=0;
			}
		}cir_plate;
	public:
		GUAXIANHOLE_BASE_INFO();
		DWORD Serial() const{return serial;}
		void SetKey(DWORD key){serial=key;}
	};
	int m_iEdgeShapeType;		//���߿׵�������� 0.��͹Բ��|1.����Բ��|2.�ڰ���Բ��
	PROFILE_VERTEX* pStartVertex,*pEndVertex;
	//ARRAY_LIST<GUAXIANHOLE_BASE_INFO> guaXianHoleInfoArr;
	CHashListEx<GUAXIANHOLE_BASE_INFO> guaXianHoleHashList;
public:
	DESIGN_GUAXIAN_HOLE();
};
/////////////////////////////////////////////////////////////////////////////
// CBendPlateByAngleDlg dialog
class CBendPlateByAngleDlg : public CRememberLocationDlg
{
// Construction
public:
	int		m_iBendLineStyle;		//�����߶�λ��ʽ��0.�Ǹֱ�Ե|1.�ֶ�ָ��
	long    m_hBendLineAngle;		//�����������Ǹ�
	int		m_iDatumEdge;			//0.���߲�|1.֫���
	double	m_fBendAngle;			//�����Ƕ�
	double	m_fBendClearSpace;		//�����߼�϶
	DESIGN_GUAXIAN_HOLE design_guaxian_hole;
	CBendPlateByAngleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBendPlateByAngleDlg)
	enum { IDD = IDD_BEND_PLATE_BY_ANGLE_DLG };
	double m_fOriginX;		// ��λԭ��X����
	double m_fOriginY;		// ��λԭ��Y����
	int m_iIndex;		// ���߿����
	int m_iHoleD;		// �׾�
	double m_fRadius;	// ���߿���Ե�뾶
	double m_fPosX;		// �������ϵ�µĹ��߿�X����
	double m_fPosY;		// �������ϵ�µĹ��߿�Y����
	CString m_sTubePartNo;		// �׹ܱ��
	CString m_sCirPlatePartNo;	// ������
	double m_fOffsetDist;		// ����߲�
	int m_iTubeMaterial;		//�׹ܲ���
	int m_iTubeGuiGe;			//�׹ܹ��
	int m_iPlateMaterial;		//Բ�����
	BOOL m_bDrillHole;			// �Ƿ����
	BOOL m_bHaveTube;			// �ڳ��׹�
	BOOL m_bHaveCirPlate;		// �⺸����
	int m_iEdgeShapeType;		//�������
	CPropertyList m_xPropList;
	CXhListCtrl m_listCtrlRayAngles;
	CXhListCtrl m_listCtrlGuaXianHoles;	
	//}}AFX_DATA
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBendPlateByAngleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
// Implementation
public:
	void RefreshCtrlStatue();
	void DisplayPropertyList();
	//���Թ���
	void InitPropHashtable();
	PROPLIST_ITEM* AddPropItem(char* keyStr,PROPLIST_ITEM item);
	int GetPropValueStr(long id, char *valueStr);	//��������ID�õ�����ֵ
	int GetPropStr(long id, char *propStr);			//�õ������ַ���
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//�����ַ�����ϣ��
	static long GetPropID(char* propStr);			//�õ�����ID
	CItemInfo* AppendPropItemInfo(const char* propKey,int controlType=0,long* pPropId=NULL,BOOL bUpdateValueText=TRUE);
	CPropTreeItem* InsertItem(CPropTreeItem *pParent, const char* propKey,int controlType=0,BOOL bUpdateValueText=TRUE, int iInsertIndex=-1, BOOL bUpdate=0);
	CPropTreeItem* InsertOrUpdateItem(CPropTreeItem *pParent, const char* propKey,int controlType=0,BOOL bUpdateValueText=TRUE, int iInsertIndex=-1, BOOL bUpdate=0);
protected:

	// Generated message map functions
	//{{AFX_MSG(CBendPlateByAngleDlg)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnBnClickedBnAddGuaxianHole();
	afx_msg void OnBnClickedChkHaveTube();
	afx_msg void OnBnClickedChkHaveCirplate();
	afx_msg void OnNMDblclkListGuaXianHoles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListGuaXianHoles(NMHDR *pNMHDR, LRESULT *pResult);
	void ContextMenu(CWnd *pWnd, CPoint point);
	afx_msg void OnDeleteItem();
	afx_msg void OnBnClickedBnSelPosBolt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BENDPLATEBYANGLEDLG_H__B405F65B_AF2A_4A0F_92B1_18C705C6119A__INCLUDED_)
