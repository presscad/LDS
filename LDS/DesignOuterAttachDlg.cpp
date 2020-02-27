// DesignOuterAttachDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignOuterAttachDlg.h"
#include "afxdialogex.h"
#include "GlobalFunc.h"
#include "Query.h"
#include "LicFuncDef.h"


// CDesignOuterAttachDlg 对话框

IMPLEMENT_DYNAMIC(CDesignOuterAttachDlg, CDialogEx)


CDesignOuterAttachDlg::CDesignOuterAttachDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesignOuterAttachDlg::IDD, pParent)
	, m_sSpec(_T(""))
	, m_iMaterial(0)
	, m_sPartNo(_T(""))
	, m_sSegI(_T(""))
	, m_nWorkWingOfDepend(0)
	, m_nOutterAttachStyle(0)
{
}

CDesignOuterAttachDlg::~CDesignOuterAttachDlg()
{
}

void CDesignOuterAttachDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_CMB_SHORT_JG_SPEC, m_sSpec);
	DDX_CBIndex(pDX, IDC_CMB_SHORT_JG_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_SHORT_JG_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_SHORT_JG_SEG_I, m_sSegI);
	DDX_CBIndex(pDX, IDC_CMB_ATTACH_WING, m_nWorkWingOfDepend);
	DDX_CBIndex(pDX, IDC_CMB_OUTTER_ATTACH_STYLE, m_nOutterAttachStyle);
}


BEGIN_MESSAGE_MAP(CDesignOuterAttachDlg, CDialog)
	ON_EN_CHANGE(IDC_E_SHORT_JG_PART_NO, &CDesignOuterAttachDlg::OnChangePartNo)
	ON_CBN_SELCHANGE(IDC_CMB_OUTTER_ATTACH_STYLE, &CDesignOuterAttachDlg::OnCbnSelchangeCmbOutterAttachStyle)
	ON_CBN_SELCHANGE(IDC_CMB_ATTACH_WING, &CDesignOuterAttachDlg::OnCbnSelchangeCmbAttachWing)
END_MESSAGE_MAP()


// CDesignOuterAttachDlg 消息处理程序

BOOL CDesignOuterAttachDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_sSegI=info.iSeg.ToString();
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_SHORT_JG_SPEC);
	AddJgRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_SHORT_JG_MATERIAL);
	AddSteelMatRecord(pCMB);
	m_iMaterial=CSteelMatLibrary::GetMatIndex(info.cMaterial);
	pCMB->SetCurSel(m_iMaterial);
	GetDlgItem(IDC_CMB_ATTACH_WING)->EnableWindow(FALSE);
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_E_SHORT_JG_PART_NO);
	pEdit->SetFocus();
	CComboBox *pStyleCMB=(CComboBox*)GetDlgItem(IDC_CMB_OUTTER_ATTACH_STYLE);
	int n=pStyleCMB->GetCount();
	for(int i=0;i<n;i++)
		pStyleCMB->DeleteString(0);
	if(m_nWorkWingOfDepend==0)
	{	//工作肢为双肢
		pStyleCMB->AddString("0.外包");
		pStyleCMB->AddString("1.内包");
	}
	else
	{	//工作肢为单肢
		pStyleCMB->AddString("0.沿向顺贴");
		pStyleCMB->AddString("1.沿向倒贴");
		pStyleCMB->AddString("2.垂向朝外贴");
		pStyleCMB->AddString("3.垂向朝里贴");
		pStyleCMB->AddString("4.交叉角钢");
	}
	UpdateData(FALSE);
	OnCbnSelchangeCmbOutterAttachStyle();
	return TRUE;
}

void CDesignOuterAttachDlg::SetDatumAngle(CLDSLineAngle *pDatumAngle)
{
	m_pDatumAngle = pDatumAngle;
	for(int i=0;i<CSteelMatLibrary::GetCount();i++)
	{
		if(CSteelMatLibrary::RecordAt(i).cBriefMark==pDatumAngle->cMaterial)
		{
			m_iMaterial=i;
			break;
		}
	}
	CXhChar16 spec;
	pDatumAngle->GetAngleSimpleSpec(spec);
	m_sSpec.Format("%s",spec);
}

void CDesignOuterAttachDlg::OnOK()
{
	CDialog::OnOK();
	UpdateOuterAttachAngleInfo();
}
void CDesignOuterAttachDlg::UpdateOuterAttachAngleInfo(BOOL bInernalEnable/*=TRUE*/)
{
	if(bInernalEnable)
	{	//对话框内部调用
		info.cMaterial=CSteelMatLibrary::RecordAt(m_iMaterial).cBriefMark;
		info.iSeg=SEGI(m_sSegI);
		info.spec.Copy(m_sSpec);
	}
	info.sPartNo.Copy(m_sPartNo);
	info.iWorkWingOfDepend = m_nWorkWingOfDepend;
	if(m_nWorkWingOfDepend==0)
	{	//双肢设计
		info.bLocateOutBer=(m_nOutterAttachStyle==0)?TRUE:FALSE;
	}
	else
	{	//单肢设计
		if(m_nOutterAttachStyle==0)
		{	//顺帖
			info.nRotateAngle=0;
			info.iWorkWingOfSelf=3-m_nWorkWingOfDepend;
		}
		else if(m_nOutterAttachStyle==1)
		{	//倒贴
			info.nRotateAngle=0;
			info.iWorkWingOfSelf=m_nWorkWingOfDepend;
		}
		else if(m_nOutterAttachStyle==2)
		{	//
			info.nRotateAngle = 90;
			info.iWorkWingOfSelf=3-m_nWorkWingOfDepend;
		}
		else if(m_nOutterAttachStyle==3)
		{	//
			info.nRotateAngle=90;
			info.iWorkWingOfSelf=m_nWorkWingOfDepend;
		}
		else if	(m_nOutterAttachStyle==5)
		{
			f3dPoint datumAngleVecX=info.pDatumJg->get_norm_x_wing();
			f3dPoint datumAngleVecY=info.pDatumJg->get_norm_y_wing();
			if(abs(datumAngleVecX*info.pDatumJg1->get_norm_x_wing())>EPS_COS2
				||abs(datumAngleVecX*info.pDatumJg1->get_norm_y_wing())>EPS_COS2)
				info.iWorkWingOfDepend=2;
			else if(abs(datumAngleVecY*info.pDatumJg1->get_norm_x_wing())>EPS_COS2
				||abs(datumAngleVecY*info.pDatumJg1->get_norm_y_wing())>EPS_COS2)
				info.iWorkWingOfDepend=1;
			info.iWorkWingOfSelf=info.iWorkWingOfDepend==1?2:1;
			info.nRotateAngle=-90;
			//
		}
	}
}

void CDesignOuterAttachDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::OnCancel();
}

void CDesignOuterAttachDlg::OnChangePartNo()
{
	UpdateData();
	SEGI iSeg;
	if(ParsePartNo(m_sPartNo.GetBuffer(),&iSeg,NULL))
		m_sSegI=iSeg.ToString();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNo);
	if(pPart)
	{	//根据找到的BOM信息，更新该钢板的基本信息(段号、规格、材质)
		if(pPart->cPartType==BOMPART::ANGLE)
		{
			m_iMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
			m_sSpec.Format("%.fx%.f",pPart->wide,pPart->thick);
		}
		//else
		//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	UpdateData(FALSE);
}

void CDesignOuterAttachDlg::OnCbnSelchangeCmbOutterAttachStyle()
{
	int nOutterAttachStyle=m_nOutterAttachStyle;
	UpdateData();
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_ATTACH_STYLE);
	m_CurJointBmp.Detach();
	if(m_nWorkWingOfDepend==0)
	{
		if(m_nOutterAttachStyle==0)
			m_CurJointBmp.LoadBitmap(IDB_BMP_OUTTER_ATTACH_STYLE4);
		else if(m_nOutterAttachStyle==1)
			m_CurJointBmp.LoadBitmap(IDB_BMP_OUTTER_ATTACH_STYLE5);
	}
	else
	{
		if (m_nOutterAttachStyle == 0)
			m_CurJointBmp.LoadBitmap(IDB_BMP_OUTTER_ATTACH_STYLE);
		else if(m_nOutterAttachStyle == 1)
			m_CurJointBmp.LoadBitmap(IDB_BMP_OUTTER_ATTACH_STYLE1);
		else if(m_nOutterAttachStyle == 2)
			m_CurJointBmp.LoadBitmap(IDB_BMP_OUTTER_ATTACH_STYLE2);
		else if(m_nOutterAttachStyle == 3)
			m_CurJointBmp.LoadBitmap(IDB_BMP_OUTTER_ATTACH_STYLE3);
	}
	
	pPic->SetBitmap(m_CurJointBmp);
	UpdateData(FALSE);
	if(nOutterAttachStyle!=m_nOutterAttachStyle&&DesOuterAttachShortJg)
	{
		UpdateOuterAttachAngleInfo();
		DesOuterAttachShortJg(m_pShortSegJg,&info);
	}
}


void CDesignOuterAttachDlg::OnCbnSelchangeCmbAttachWing()
{
	int nWorkWingOfDepend=m_nWorkWingOfDepend;
	UpdateData();
	if(nWorkWingOfDepend!=m_nWorkWingOfDepend&&DesOuterAttachShortJg)
	{
		UpdateOuterAttachAngleInfo();
		DesOuterAttachShortJg(m_pShortSegJg,&info);	
	}
}
