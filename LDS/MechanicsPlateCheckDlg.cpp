// InputWirePointDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include ".\MechanicsPlateCheckDlg.h"

#ifdef __PART_DESIGN_INC_
//////////////////////////////////////////////////////////////////////////
//回调函数
//////////////////////////////////////////////////////////////////////////
// CMechanicsPlateCheckDlg 对话框

int CMechanicsPlateCheckDlg::nScrLocationX = 0;
int CMechanicsPlateCheckDlg::nScrLocationY = 0;
CMechanicsPlateCheckDlg::CMechanicsPlateCheckDlg(CWnd* pParent /*=nullptr*/)
	: CCallBackDialog(IDD_MECHANICS_CHECK_DLG, pParent)
	,m_bConnStrengthCheck(FALSE)
	,m_bBoltBlockShearCheck(FALSE)
	,m_bEdgeStableCheck(FALSE)
	,m_fRodAxisForce(0)
	,m_iAxisForceMode(0)
	,m_sRayAngleLabel(_T(""))
{
	operangle.dfAxisNByBolts=0;
	operangle.pOperAngle=NULL;
}

CMechanicsPlateCheckDlg::~CMechanicsPlateCheckDlg()
{
}

void CMechanicsPlateCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkTubeDlg)
	DDX_Check(pDX,IDC_CHK_PLATE_CONN_STRENGTH,m_bConnStrengthCheck);
	DDX_Check(pDX,IDC_CHK_CONN_ANGLE_BLOCK_SHEAR,m_bBoltBlockShearCheck);
	DDX_Check(pDX,IDC_CHK_PLATE_EDGE_STABLE,m_bEdgeStableCheck);
	DDX_Radio(pDX,IDC_RDO_AXIS_FORCE_CALMODE,m_iAxisForceMode);
	DDX_Text(pDX,IDC_E_AXIS_LOAD_FORCE,m_fRodAxisForce);
	DDX_Text(pDX,IDC_S_RAY_ANGLE_SIZE,m_sRayAngleLabel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMechanicsPlateCheckDlg, CDialog)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_RDO_AXIS_FORCE_CALMODE,&CMechanicsPlateCheckDlg::OnBnRdoAxisForceCalmode)
	ON_BN_CLICKED(IDC_RADIO12,&CMechanicsPlateCheckDlg::OnBnRdoAxisForceCalmode)
END_MESSAGE_MAP()


// CMechanicsPlateCheckDlg 消息处理程序
BOOL CMechanicsPlateCheckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//
	if (operangle.pOperAngle!=NULL)
		m_sRayAngleLabel.Format("角钢0x%X{%s}",operangle.pOperAngle->handle,operangle.pOperAngle->Label);
	this->m_fRodAxisForce=operangle.dfAxisNByBolts;
	//移动对话框到左上角
	CRect rect;
	GetWindowRect(rect);
	int width = rect.Width();
	int height = rect.Height();
	rect.left = nScrLocationX;
	rect.top = nScrLocationY;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	MoveWindow(rect, TRUE);
	m_bEnableWindowMoveListen = TRUE;
	UpdateData(FALSE);
	return TRUE;
}

void CMechanicsPlateCheckDlg::OnOK()
{
	CDialog::OnOK();
}

void CMechanicsPlateCheckDlg::OnMove(int x, int y)
{
	CCallBackDialog::OnMove(x, y);
	if (m_bEnableWindowMoveListen)
	{
		nScrLocationX = x;
		nScrLocationY = y;
	}
}

void CMechanicsPlateCheckDlg::OnBnRdoAxisForceCalmode()
{
	UpdateData();
	CEdit* pForceEdit=(CEdit*)GetDlgItem(IDC_E_AXIS_LOAD_FORCE);
	if (pForceEdit)
		pForceEdit->SetReadOnly(this->m_iAxisForceMode!=0);
	if (m_iAxisForceMode==1)
	{
		this->m_fRodAxisForce=operangle.dfAxisNByBolts;
		UpdateData(false);
	}
}

#endif
