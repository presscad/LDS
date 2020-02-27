// WindLoadSegDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "WindLoadSegDlg.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWindLoadSegDlg dialog

#if defined(__LDS_)||defined(__TSA_)
CWindLoadSegDlg::CWindLoadSegDlg(CWnd* pParent /*=NULL*/)
: CCallBackDialog(CWindLoadSegDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWindLoadSegDlg)
	m_fFrontCoef = 1.2;
	m_fSideCoef = 1.2;
	m_fBetaZ = 1.2;
	m_iCalStyle = 0;
	m_iMirStyle = 0;
	m_iSegType = 0;
	m_fCoefK = 1.0;
	m_fProfileCoef = 1.3;
	m_bSpecProfileCoef = FALSE;
	m_fBetaZForFoundation = 1.0;
	m_bSpecBtoAFront = FALSE;
	m_fBtoAFront = 1.0;
	m_fPrjOrgX = 0.0;
	m_fPrjOrgY = 0.0;
	m_fPrjOrgZ = 0.0;
	//}}AFX_DATA_INIT
	m_listNodeCtrl.AddColumnHeader("节点句柄");
	m_listNodeCtrl.AddColumnHeader("节点编号");
}


void CWindLoadSegDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWindLoadSegDlg)
	DDX_Control(pDX, IDC_LIST_NODE_CTRL, m_listNodeCtrl);
	DDX_Text(pDX, IDC_E_FRONT_COEF, m_fFrontCoef);
	DDX_Text(pDX, IDC_E_SIDE_COEF, m_fSideCoef);
	DDX_Text(pDX, IDC_E_WIND_BETA_Z, m_fBetaZ);
	DDX_CBIndex(pDX, IDC_CMB_CAL_STYLE, m_iCalStyle);
	DDX_CBIndex(pDX, IDC_CMB_MIR_STYLE, m_iMirStyle);
	DDX_CBIndex(pDX, IDC_CMB_SEG_TYPE, m_iSegType);
	DDX_Text(pDX, IDC_E_K_COEF, m_fCoefK);
	DDX_Text(pDX, IDC_E_PROFILE_COEF, m_fProfileCoef);
	DDX_Check(pDX, IDC_CHK_SPEC_PROFILE_COEF, m_bSpecProfileCoef);
	DDX_Text(pDX, IDC_E_WIND_BETA_Z_FOR_FOUNDATION, m_fBetaZForFoundation);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_E_BtoA_FRONT, m_fBtoAFront);
	DDX_Check(pDX, IDC_CHK_SPEC_BtoA_FRONT, m_bSpecBtoAFront);
	DDV_MinMaxDouble(pDX, m_fBtoAFront, 0.1, 10);
	DDX_Text(pDX, IDC_E_PRJ_ORG_X, m_fPrjOrgX);
	DDX_Text(pDX, IDC_E_PRJ_ORG_Y, m_fPrjOrgY);
	DDX_Text(pDX, IDC_E_PRJ_ORG_Z, m_fPrjOrgZ);
}


BEGIN_MESSAGE_MAP(CWindLoadSegDlg, CDialog)
	//{{AFX_MSG_MAP(CWindLoadSegDlg)
	ON_BN_CLICKED(IDC_BN_ADD_ITEM_RECORD, OnBnAddItemRecord)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_NODE_CTRL, OnKeydownListNodeCtrl)
	ON_BN_CLICKED(IDC_CHK_SPEC_PROFILE_COEF, OnChkSpecProfileCoef)
	ON_BN_CLICKED(IDC_CHK_SPEC_BtoA_FRONT, OnBnClickedChkSpecBtoAFront)
	ON_CBN_SELCHANGE(IDC_CMB_CAL_STYLE, OnSelchangeCmbCalStyle)
	ON_CBN_SELCHANGE(IDC_CMB_SEG_TYPE, OnSelchangeCmbSegType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindLoadSegDlg message handlers

BOOL CWindLoadSegDlg::OnInitDialog() 
{
	m_iSegType=m_pWindSeg->cType;		//标准段号
	m_iMirStyle=m_pWindSeg->cMirType;	//对称信息
	m_iCalStyle=m_pWindSeg->cCalStyle;	//计算方式
	m_fFrontCoef=m_pWindSeg->CqiFront;
	m_fSideCoef=m_pWindSeg->CqiSide;
	if(m_bSpecBtoAFront||m_pWindSeg->B_AFront>=0.1)
		m_fBtoAFront=m_pWindSeg->B_AFront;
	m_bSpecBtoAFront=m_pWindSeg->m_bSpecB_A;
	if(m_pWindSeg->cType!=0||m_pWindSeg->cCalStyle!=0)
		m_fCoefK=1.0;
	else
		m_fCoefK=m_pWindSeg->K;
	m_fPrjOrgX = m_pWindSeg->xProjPlaneOrg.x;
	m_fPrjOrgY = m_pWindSeg->xProjPlaneOrg.y;
	m_fPrjOrgZ = m_pWindSeg->xProjPlaneOrg.z;
	m_bSpecProfileCoef = !m_pWindSeg->m_bAutoCalProfileCoef;
	m_fProfileCoef = m_pWindSeg->m_fProfileModeCoef;
	m_fBetaZ=m_pWindSeg->BetaZ;
	m_fBetaZForFoundation=m_pWindSeg->BetaZForFoundation;
	
	CDialog::OnInitDialog();
	GetDlgItem(IDC_E_PROFILE_COEF)->EnableWindow(m_bSpecProfileCoef);
	GetDlgItem(IDC_CHK_SPEC_BtoA_FRONT)->EnableWindow(!m_bSpecProfileCoef);
	GetDlgItem(IDC_CMB_MIR_STYLE)->EnableWindow(m_iSegType==1);
	GetDlgItem(IDC_CMB_CAL_STYLE)->EnableWindow(m_iSegType==1);
	((CEdit*)GetDlgItem(IDC_E_BtoA_FRONT))->SetReadOnly(m_bSpecProfileCoef||!m_bSpecBtoAFront);
	GetDlgItem(IDC_E_K_COEF)->EnableWindow(m_pWindSeg->cType==0&&m_pWindSeg->cCalStyle==0);
	m_listNodeCtrl.InitListCtrl();
	CStringArray str_arr;
	str_arr.SetSize(2);
	if(m_bInernalStart)
	{
		CLDSNode *pNode;
		for(pNode=tempNodeSet.GetFirst();pNode;pNode=tempNodeSet.GetNext())
		{
			str_arr[0].Format("0X%X",pNode->handle);
			str_arr[1].Format("%d",pNode->point_i);
			int iCurSel = m_listNodeCtrl.InsertItemRecord(-1,str_arr);
			m_listNodeCtrl.SetItemData(iCurSel,pNode->handle);
		}
		if(m_hPickObj>0)
		{
			pNode=Ta.Node.FromHandle(m_hPickObj);
			if(pNode)
			{
				str_arr[0].Format("0X%X",m_hPickObj);
				str_arr[1].Format("%d",pNode->point_i);
				int iCurSel = m_listNodeCtrl.InsertItemRecord(-1,str_arr);
				m_listNodeCtrl.SetItemData(iCurSel,pNode->handle);
			}
		}
	}
	else if(!m_bInernalStart)
	{
		int n=m_pWindSeg->StatValidNodeCount(&Ta);
		for(int i=0;i<n;i++)
		{
			str_arr[0].Format("0X%X",m_pWindSeg->nodeArr[i]);
			str_arr[1].Format("%d",	m_pWindSeg->nodePtrArr[i]->point_i);
			int iCurSel = m_listNodeCtrl.InsertItemRecord(-1,str_arr);
			m_listNodeCtrl.SetItemData(iCurSel,m_pWindSeg->nodeArr[i]);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWindLoadSegDlg::OnBnAddItemRecord() 
{
	if(m_listNodeCtrl.GetItemCount()==8)
	{
		AfxMessageBox("系统暂只支持最大8个节点的自定义轮廓风压段!");
		return;
	}
	UpdateData();
	m_pWindSeg->cCalStyle=m_iCalStyle;
	m_pWindSeg->cMirType=m_iMirStyle;
	m_pWindSeg->cType=m_iSegType;
	m_bInernalStart=TRUE;
	m_iPickObjType=CLS_NODE;	//新添加节点
	m_bPauseBreakExit = TRUE;
	m_dwSelectPartsType = GetSingleWord(SELECTINDEX_NODE);
	m_sCmdPickPrompt = "请按逆时针顺序选择风压段轮廓节点，<回车键>确认";
	tempNodeSet.Empty();
	int n=m_listNodeCtrl.GetItemCount();
	for(int i=0;i<n;i++)
	{
		THANDLE h=m_listNodeCtrl.GetItemData(i);
		CLDSNode *pNode=Ta.Node.FromHandle(h);
		if(pNode)
			tempNodeSet.append(pNode);
	}
	CDialog::OnOK();
}

void CWindLoadSegDlg::OnOK() 
{
	CDialog::OnOK();
	m_pWindSeg->cType=(BYTE)m_iSegType;			//标准段号
	m_pWindSeg->cMirType=(BYTE)m_iMirStyle;		//对称信息
	m_pWindSeg->cCalStyle=(BYTE)m_iCalStyle;	//计算方式
	m_pWindSeg->CqiFront=m_fFrontCoef;
	m_pWindSeg->CqiSide=m_fSideCoef;
	m_pWindSeg->m_bSpecB_A=m_bSpecBtoAFront;
	if(m_bSpecBtoAFront)
		m_pWindSeg->B_AFront=m_fBtoAFront;
	m_pWindSeg->K=m_fCoefK;
	m_pWindSeg->m_bAutoCalProfileCoef = !m_bSpecProfileCoef;
	if(m_bSpecProfileCoef)
		m_pWindSeg->m_fProfileModeCoef=m_fProfileCoef;
	m_pWindSeg->BetaZ=m_fBetaZ;
	m_pWindSeg->BetaZForFoundation=m_fBetaZForFoundation;
	m_pWindSeg->xProjPlaneOrg.x = m_fPrjOrgX;
	m_pWindSeg->xProjPlaneOrg.y = m_fPrjOrgY;
	m_pWindSeg->xProjPlaneOrg.z = m_fPrjOrgZ;
	int n=m_listNodeCtrl.GetItemCount();
	memset(m_pWindSeg->nodeArr,0,sizeof(long)*8);
	memset(m_pWindSeg->nodePtrArr,0,sizeof(CLDSNode*)*8);
	for(int i=0;i<n;i++)
	{
		THANDLE h=m_listNodeCtrl.GetItemData(i);
		m_pWindSeg->nodeArr[i]=h;
	}
}

void CWindLoadSegDlg::OnKeydownListNodeCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	POSITION pos = m_listNodeCtrl.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_listNodeCtrl.GetNextSelectedItem(pos);
		/*if(pLVKeyDow->wVKey==VK_F3)
		{
			if(iCurSel>=0)
				DisplayItemInfo(iCurSel);
		}
		else */if(pLVKeyDow->wVKey==VK_DELETE)
		{
			if(iCurSel>=0)
				m_listNodeCtrl.DeleteItem(iCurSel);
		}
	}
	*pResult = 0;
}
void CWindLoadSegDlg::OnChkSpecProfileCoef() 
{
	UpdateData();
	GetDlgItem(IDC_E_PROFILE_COEF)->EnableWindow(m_bSpecProfileCoef);
	GetDlgItem(IDC_CHK_SPEC_BtoA_FRONT)->EnableWindow(!m_bSpecProfileCoef);
	((CEdit*)GetDlgItem(IDC_E_BtoA_FRONT))->SetReadOnly(m_bSpecProfileCoef||!m_bSpecBtoAFront);
}
void CWindLoadSegDlg::OnBnClickedChkSpecBtoAFront()
{
	UpdateData();
	((CEdit*)GetDlgItem(IDC_E_BtoA_FRONT))->SetReadOnly(m_bSpecProfileCoef||!m_bSpecBtoAFront);
}

void CWindLoadSegDlg::OnSelchangeCmbCalStyle() 
{
	UpdateData();
	GetDlgItem(IDC_E_K_COEF)->EnableWindow(m_iSegType==0&&m_iCalStyle==0);
	if(m_iSegType!=0||m_iCalStyle!=0)
		m_fCoefK=1.0;
	UpdateData(FALSE);
}

void CWindLoadSegDlg::OnSelchangeCmbSegType() 
{
	UpdateData();
	if(m_iSegType==0)
	{
		m_iCalStyle=0;
		m_iMirStyle=0;
	}
	else if(m_iSegType==1&&m_iCalStyle==0)
		m_iCalStyle=1;
	GetDlgItem(IDC_E_K_COEF)->EnableWindow(m_iSegType==0&&m_iCalStyle==0);
	GetDlgItem(IDC_CMB_MIR_STYLE)->EnableWindow(m_iSegType==1);
	GetDlgItem(IDC_CMB_CAL_STYLE)->EnableWindow(m_iSegType==1);
	if(m_iSegType!=0||m_iCalStyle!=0)
		m_fCoefK=1.0;
	UpdateData(FALSE);
}
#endif
