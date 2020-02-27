#pragma once

#ifdef __PART_DESIGN_INC_
// CCalPerfectSectDlg 对话框

class CCalPerfectSectsDlg : public CDialog
{
	DECLARE_DYNAMIC(CCalPerfectSectsDlg)
public:
	CLDSLinePart* m_pRod;
	STEELMAT *m_pSteel;
	double m_fR;			//内圆弧半径
	double m_fRx;			//回转半径(惯性半径)
	double m_fGrossArea;	//毛面积
	double m_fNetArea;		//净面积
	double m_fLsCoef;		//螺栓减孔系数
	int m_nBoltD;			//螺栓直径
	int m_nSectNum;			//节间数
	char m_cSectionType;	//
public:
	void OptimizeSectCounts();
	double VerityCompStableStress();
public:
	CCalPerfectSectsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCalPerfectSectsDlg();

// 对话框数据
	enum { IDD = IDD_CAL_PERFECT_SECTS_DLG };

public:
	CString	m_sLsGuiGe;		//螺栓规格
	CString m_sStress;		//压稳应力
	CString m_sSectNum;		//节间数
	CString m_sLsCoef;		//螺栓减孔系数
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChangeCmbLsGuige();
	afx_msg void OnChangeCmbSectNum();
	afx_msg void OnChangeEHoleOut();
};
#endif