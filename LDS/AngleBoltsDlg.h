#pragma once

#include "supergridctrl.h"
#include "RememberLocationDlg.h"
// CAngleBoltsDlg 对话框

struct RODSECTION{
	char ciSectType;	//0.指定节点处;1.杆件始端面;2.杆件终端面
	CLDSNode* pSectNode;
	RODSECTION(CLDSNode* _pSectNode = NULL) {
		if (pSectNode = _pSectNode)
			ciSectType = 0;
		else
			ciSectType = 1;
	}
};

class CAngleBoltsDlg : public CRememberLocationDlg
{
	DECLARE_DYNAMIC(CAngleBoltsDlg)
public:
	CLDSLinePart* m_pBaseRod,*m_pCrossRod;
	RODSECTION section;
	GEPOINT m_xWingOffsetVec,m_xLineVec;
	int m_iOffsetWing;	//0.X肢 1.Y肢
	int	m_iRayDirection;//射线方向 0.始->终 1.终->始
	int	m_iBoltNorm;	//螺栓朝向 0.朝外 1.朝里
	int	m_iLsLayStyle;	//螺栓排列方式
	int m_iDatumPtStyle;	//定位方式 1.节点在心线投影 2.节点在心线平推 3.角钢心线交点
	BOOL m_bMirLayLs;	//对称布孔
	f3dPoint viewNorm;	//视图法向
	f3dPoint face_norm_offset;	//平推面法向
	CDesignThickPara m_tBoltNormOffset;
	ATOM_LIST<CDesignLsPara> *m_pLsList;
	CHashList<CLDSBolt> m_xHashPrevViewBolts;
	//
	void InitDesignLsPara(CDesignLsPara& ls_para);
	void LayoutBolts();
	void PreviewBolt(CDesignLsPara *pLsPara);
	void UpdateContrlStatus();
	void UpdateLsSpaceData();
	void AppendRecord(CString*str_arr,void* pLsPara=NULL);
	void RefreshLsListCtrl();
	void WorkPlaneSketch();
	void DelWorkPlaneSketch();
	BOOL IsEnableSwitchG(int* g1=NULL,int* g2=NULL);
public:
	CAngleBoltsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAngleBoltsDlg();

// 对话框数据
	enum { IDD = IDD_ANGLE_BOLT_DLG };
	CString m_sBaseAngleSize;
	CString m_sRayAngleSize;
	CString m_sDatumPtText;
	CString m_sDatumPtText2;
	int		m_nG;
	int		m_nG1;
	int		m_nG2;
	int		m_nCrossAngleG;
	BOOL	m_bProject;		//节点在心线投影上
	BOOL	m_bPlaneOffset;	//节点在心线平推上
	CString	m_sBoltSize;	//螺栓规格
	double  m_fHoleD;		//孔径
	int		m_nLsSpace;		//螺栓间距
	int		m_nOffset0;		//首螺栓偏移
	int		m_nLsNum;		//螺栓数量
	int		m_iBoltRows;	//螺栓排数 0.单排 1.多排
	int		m_iHoleFuncType;//孔功用类型
	BOOL	m_bPunchHole;	//冲孔
	BOOL	m_bDrillHole;	//钻孔
	CSuperGridCtrl m_xListCtrlLsSet;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedChkProject();
	afx_msg void OnBnClickedChkPlaneOffset();
	afx_msg void OnBnTwoEdgeLayBolts();
	afx_msg void OnBnSingleLayBolts();
	afx_msg void OnBnSwitchG();
	afx_msg void OnChangeCmbBoltSize();
	afx_msg void OnSelchangeCmbLsRows();
	afx_msg void OnChangeELsNum();
	afx_msg void OnClickedChkPunchHole();
	afx_msg void OnClickedChkDrillHole();
	afx_msg void OnClickListLsSet(NMHDR* pNMHDR, LRESULT* pResult);
};
