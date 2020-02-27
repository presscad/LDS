// CheckDataDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "CheckPartDataDlg.h"
#include "afxdialogex.h"
#include "OriginalData.h"
#include "SnapTypeVerify.h"
#include "OriginalData.h"
#include "Query.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//
void AddPlateRecord(CComboBox *pCMB)
{
	pCMB->AddString("-2");
	pCMB->AddString("-4");
	pCMB->AddString("-5");
	pCMB->AddString("-6");
	pCMB->AddString("-8");
	pCMB->AddString("-10");
	pCMB->AddString("-12");
	pCMB->AddString("-14");
	pCMB->AddString("-16");
	pCMB->AddString("-18");
	pCMB->AddString("-20");

}
// CCheckPartDataDlg 对话框
IMPLEMENT_DYNAMIC(CCheckPartDataDlg, CDialogEx)

int CCheckPartDataDlg::nScrLocationX=0;
int CCheckPartDataDlg::nScrLocationY=0;
CCheckPartDataDlg::CCheckPartDataDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCheckPartDataDlg::IDD, pParent)
{
	m_bChkPartNo=TRUE;
	m_bChkGuige=TRUE;
	m_bChkLength=TRUE;
	m_bChkCount=TRUE;
	m_bChkMaterial=TRUE;
	m_bChkM16Num=FALSE;
	m_bChkM20Num=FALSE;
	m_bChkM24Num=FALSE;
	m_bChkElseNum=FALSE;
	m_bChkHole=FALSE;
	m_nM16=m_nM20=m_nM24=m_nElse=m_nHole=0;	
	m_nCount=0;
	m_ciPartType=0;
	m_bEnableWindowMoveListen=false;
}

CCheckPartDataDlg::~CCheckPartDataDlg()
{
}

void CCheckPartDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_PART_NO, m_bChkPartNo);
	DDX_Check(pDX, IDC_CHK_MATERIAL, m_bChkMaterial);
	DDX_Check(pDX, IDC_CHK_GUIGE, m_bChkGuige);
	DDX_Check(pDX, IDC_CHK_LENGTH, m_bChkLength);
	DDX_Check(pDX, IDC_CHK_COUNT, m_bChkCount);
	DDX_Check(pDX, IDC_CHK_M16_NUM, m_bChkM16Num);
	DDX_Check(pDX, IDC_CHK_M20_NUM, m_bChkM20Num);
	DDX_Check(pDX, IDC_CHK_M24_NUM, m_bChkM24Num);
	DDX_Check(pDX, IDC_CHK_ELSE_NUM, m_bChkElseNum);
	DDX_Check(pDX, IDC_CHK_HOLE, m_bChkHole);
	DDX_Text(pDX, IDC_E_HANDLE, m_sHandle);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_PARTTYPE, m_sPartType);
	DDX_CBString(pDX, IDC_CMB_MATERIAL, m_sMaterial);
	DDX_CBString(pDX, IDC_CMB_GUIGE, m_sGuige);
	DDX_Text(pDX, IDC_E_LENGTH, m_sLen);
	DDX_Text(pDX, IDC_E_COUNT, m_nCount);
	DDX_Text(pDX, IDC_E_M16_NUM, m_nM16);
	DDX_Text(pDX, IDC_E_M20_NUM, m_nM20);
	DDX_Text(pDX, IDC_E_M24_NUM, m_nM24);
	DDX_Text(pDX, IDC_E_ELSE_NUM, m_nElse);
	DDX_Text(pDX, IDC_E_HOLE, m_nHole);
}

BEGIN_MESSAGE_MAP(CCheckPartDataDlg, CDialogEx)
	ON_WM_MOVE()
	ON_CBN_SELCHANGE(IDC_CMB_MATERIAL, &CCheckPartDataDlg::OnCbnSelchangeCmbMaterial)
	ON_CBN_SELCHANGE(IDC_CMB_GUIGE, &CCheckPartDataDlg::OnCbnSelchangeCmbGuige)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
BOOL CCheckPartDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	((CEdit*)GetDlgItem(IDC_E_PART_NO))->SetReadOnly(TRUE);
	//构件材质
	CComboBox* pCMBMate = (CComboBox*)GetDlgItem(IDC_CMB_MATERIAL);
	pCMBMate->AddString("Q235");
	pCMBMate->AddString("Q345");
	pCMBMate->AddString("Q420");
	GetDlgItem(IDC_CHK_HOLE)->EnableWindow(FALSE);
	InitSelPartInfo();
	//移动窗口到合适位置
	CRect rect;
	CWnd::GetWindowRect(rect);
	int width = rect.Width();
	int height=rect.Height();
	rect.left=nScrLocationX;
	rect.top=nScrLocationY;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;
	MoveWindow(rect, TRUE);
	m_bEnableWindowMoveListen=true;
	UpdateData(FALSE);
	return TRUE;
}
void CCheckPartDataDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}
void CCheckPartDataDlg::OnOK()
{
	UpdateData();
	if(m_bChkPartNo==FALSE)
	{
		MessageBox("请优先确认件号信息！");
		return;
	}
	CreatePartCheckData();
	return CDialog::OnOK();
}
void CCheckPartDataDlg::OnCancel()
{
	return CDialog::OnCancel();
}
void CCheckPartDataDlg::OnCbnSelchangeCmbMaterial()
{
	UpdateData();
	if(m_pSelPart.IsNULL() || strlen(m_pSelPart->GetPartNo())<=0)
		return;
	m_pSelPart->cMaterial=QuerySteelBriefMatMark(CXhChar100(m_sMaterial));
	m_pSelPart->SyncMirProp();	//更新关联对象
	//更新属性栏
	CLDSView* pView=(CLDSView*)((CMainFrame*)theApp.m_pMainWnd)->GetLDSView();
	pView->UpdatePropertyPage(m_pSelPart);
}
void CCheckPartDataDlg::OnCbnSelchangeCmbGuige()
{
	CComboBox* pCmbGuige=(CComboBox*)GetDlgItem(IDC_CMB_GUIGE);
	int iCurSel=pCmbGuige->GetCurSel();
	pCmbGuige->GetLBText(iCurSel,m_sGuige);
	if(m_pSelPart.IsNULL() || strlen(m_pSelPart->GetPartNo())<=0)
		return;
	double fThick=0,fWidth=0;
	if(m_pSelPart->IsAngle())
	{
		restore_JG_guige(m_sGuige,fWidth,fThick);
		m_pSelPart.LineAnglePointer()->set_wing_wide(fWidth);
		m_pSelPart.LineAnglePointer()->set_wing_thick(fThick);
	}
	else if(m_pSelPart->IsPlate())
	{
		fThick=fabs(atof(m_sGuige));
		m_pSelPart.GeneralPlatePointer()->Thick=fThick;
	}
	else if(m_pSelPart->GetClassTypeId()==CLS_LINETUBE)
	{
		restore_JG_guige(m_sGuige,fWidth,fThick);
		m_pSelPart.LineTubePointer()->size_wide=fWidth;
		m_pSelPart.LineTubePointer()->size_thick=fThick;
	}
	else if(m_pSelPart->GetClassTypeId()==CLS_LINEFLAT)
	{
		restore_JG_guige(m_sGuige,fWidth,fThick);
		m_pSelPart.LineFlatPointer()->size_wide=fWidth;
		m_pSelPart.LineFlatPointer()->size_thick=fThick;
	}
	m_pSelPart->SyncMirProp();	//更新关联对象
	//更新属性栏
	CLDSView* pView=(CLDSView*)((CMainFrame*)theApp.m_pMainWnd)->GetLDSView();
	pView->UpdatePropertyPage(m_pSelPart);
}
//////////////////////////////////////////////////////////////////////////
//生成构件的校审数据
void CCheckPartDataDlg::CreatePartCheckData()
{
	CDataCheckModel::PART_CHECK_ITEM* pPartCheckItem=xDataCheckModel.AddPartCheckItem(m_sPartNo);
	pPartCheckItem->m_dwFlag=0;
	pPartCheckItem->drawing.m_sPartNo=m_sPartNo;
	if(strcmp(m_sPartType,"角钢")==0)
		pPartCheckItem->drawing.ciPartType=1;
	else if(strcmp(m_sPartType,"钢板")==0)
		pPartCheckItem->drawing.ciPartType=3;
	else if(strcmp(m_sPartType,"钢管")==0)
		pPartCheckItem->drawing.ciPartType=4;
	else if(strcmp(m_sPartType,"扁铁")==0)
		pPartCheckItem->drawing.ciPartType=5;
	else if(strcmp(m_sPartType,"槽钢")==0)
		pPartCheckItem->drawing.ciPartType=6;
	else
		pPartCheckItem->drawing.ciPartType=100;
	if(m_bChkMaterial)
	{
		pPartCheckItem->drawing.cMaterial=m_pSelPart->cMaterial;
		pPartCheckItem->m_dwFlag|=MARK_CMAT;
	}
	if(m_bChkGuige)
	{
		pPartCheckItem->drawing.m_fThick=m_pSelPart->GetThick();
		pPartCheckItem->m_dwFlag|=MARK_SIZE;
		if(pPartCheckItem->drawing.ciPartType!=3)
		{
			pPartCheckItem->drawing.m_fWidth=m_pSelPart.LinePartPointer()->GetWidth();
			pPartCheckItem->m_dwFlag|=MARK_SIZE;
		}
		else
			pPartCheckItem->drawing.m_fWidth=0;
	}
	if(m_bChkLength)
	{
		pPartCheckItem->drawing.m_fLength=atof(m_sLen);
		pPartCheckItem->m_dwFlag|=MARK_LENGTH;
	}
	if(m_bChkCount)
	{
		pPartCheckItem->drawing.nCount=m_nCount;
		pPartCheckItem->m_dwFlag|=MARK_COUNT;
	}
	if(m_bChkM16Num)
	{
		pPartCheckItem->drawing.nM16Bolt=m_nM16;
		pPartCheckItem->m_dwFlag|=MARK_M16BOLT;
	}
	if(m_bChkM20Num)
	{
		pPartCheckItem->drawing.nM20Bolt=m_nM20;
		pPartCheckItem->m_dwFlag|=MARK_M20BOLT;
	}
	if(m_bChkM24Num)
	{
		pPartCheckItem->drawing.nM24Bolt=m_nM24;
		pPartCheckItem->m_dwFlag|=MARK_M24BOLT;
	}
	if(m_bChkElseNum)
	{
		pPartCheckItem->drawing.nElseBolt=m_nElse;
		pPartCheckItem->m_dwFlag|=MARK_ELSEBOLT;
	}
	if(m_bChkHole)
	{
		pPartCheckItem->drawing.nElseBolt=m_nHole;
		pPartCheckItem->m_dwFlag|=MARK_HOLE;
	}
}
void CCheckPartDataDlg::SetPart(CLDSPart* pPart)
{
	if(pPart!=NULL)
		m_pSelPart=pPart;
}
void CCheckPartDataDlg::InitSelPartInfo()
{
	if(m_pSelPart.IsNULL() || strlen(m_pSelPart->GetPartNo())<=0)
		return;
	CComboBox* pCmbGuige=(CComboBox*)GetDlgItem(IDC_CMB_GUIGE);
	pCmbGuige->ResetContent();
	if(m_pSelPart->IsAngle())
		AddJgRecord(pCmbGuige);
	else if(m_pSelPart->IsPlate())
		AddPlateRecord(pCmbGuige);
	else if(m_pSelPart->GetClassTypeId()==CLS_LINETUBE)
		AddTubeRecord(pCmbGuige);	
	//
	m_sHandle.Format("0x%X",m_pSelPart->handle);
	m_sPartNo.Format("%s",(char*)m_pSelPart->GetPartNo());
	CDataCheckModel::PART_CHECK_ITEM* pPartCheckItem=xDataCheckModel.AddPartCheckItem(m_sPartNo);
	//材质属性校审项
	char matStr[20];
	if(pPartCheckItem->m_dwFlag&CDataCheckModel::PART_DATA::PROP_MATERIAL)
	{
		m_bChkMaterial=TRUE;
		QuerySteelMatMark(pPartCheckItem->drawing.cMaterial,matStr);
	}
	else
	{
		m_bChkMaterial=FALSE;
		QuerySteelMatMark(m_pSelPart->cMaterial,matStr);
		pPartCheckItem->drawing.cMaterial=m_pSelPart->cMaterial;
	}
	m_sMaterial.Format("%s",matStr);
	//规格属性校审项
	if(pPartCheckItem->m_dwFlag&CDataCheckModel::PART_DATA::PROP_SIZE)
		m_bChkGuige=TRUE;
	else
	{
		m_bChkGuige=FALSE;
		pPartCheckItem->drawing.m_fWidth=m_pSelPart->GetWidth();
		pPartCheckItem->drawing.m_fThick=m_pSelPart->GetThick();
	}
	if(pPartCheckItem->drawing.ciPartType==CDataCheckModel::PART_DATA::TYPE_ANGLE)
		m_sGuige.Format("%gX%g",pPartCheckItem->drawing.m_fWidth,pPartCheckItem->drawing.m_fThick);
	else if(pPartCheckItem->drawing.ciPartType==CDataCheckModel::PART_DATA::TYPE_PLATE)
		m_sGuige.Format("%g",pPartCheckItem->drawing.m_fThick);
	else
		m_sGuige.Format("%gX%g",pPartCheckItem->drawing.m_fWidth,pPartCheckItem->drawing.m_fThick);
	//长度属性校审项
	if(pPartCheckItem->m_dwFlag&CDataCheckModel::PART_DATA::PROP_LENGTH)
		m_bChkLength=TRUE;
	else
	{
		m_bChkLength=FALSE;
		pPartCheckItem->drawing.m_fLength=m_pSelPart->GetLength();
	}
	GetDlgItem(IDC_CHK_LENGTH)->EnableWindow(m_pSelPart->IsLinePart());
	GetDlgItem(IDC_E_LENGTH)->EnableWindow(m_pSelPart->IsLinePart());
	if(m_pSelPart->IsLinePart())
		m_sLen.Format("%.1f",pPartCheckItem->drawing.m_fLength);
	else
		m_sLen="";
	//数量属性校审项
	m_nCount=0;
	for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_BOLT||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE||
			pPart->GetClassTypeId()==CLS_POLYJOINT)
			continue;	//螺栓无编号不参预构件号检查
		if(m_sPartNo.CompareNoCase(pPart->GetPartNo())!=0)
			continue;
		m_nCount++;
	}
	if(pPartCheckItem->m_dwFlag&CDataCheckModel::PART_DATA::PROP_LENGTH)
	{
		m_bChkCount=TRUE;
		m_nCount=pPartCheckItem->drawing.nCount;
	}
	else
	{
		m_bChkCount=FALSE;
		pPartCheckItem->drawing.nCount=m_nCount;
	}
	//

	m_nM16=m_pSelPart->GetLsCount(16);
	m_nM20=m_pSelPart->GetLsCount(20);
	m_nM24=m_pSelPart->GetLsCount(24);
	m_nElse=m_pSelPart->GetLsCount()-m_nM16-m_nM20-m_nM24;
	//
	if(m_pSelPart->IsAngle())
	{
		m_sPartType="角钢";
		m_ciPartType=CDataCheckModel::PART_DATA::TYPE_ANGLE;
	}
	else if(m_pSelPart->IsPlate())
	{
		m_sPartType="钢板";
		m_ciPartType=CDataCheckModel::PART_DATA::TYPE_PLATE;
	}
	else if(m_pSelPart->GetClassTypeId()==CLS_LINETUBE)
	{
		m_sPartType="钢管";
		m_ciPartType=4;
	}
	else if(m_pSelPart->GetClassTypeId()==CLS_LINEFLAT)
	{
		m_sPartType="扁铁";
		m_ciPartType=5;
	}
	else if(m_pSelPart->GetClassTypeId()==CLS_LINESLOT)
	{
		m_sPartType="槽钢";
		m_ciPartType=6;
	}
	else
		m_sPartType="其他";
}