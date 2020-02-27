// RsltBufferDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include <direct.h>
#include "LDSDoc.h"
#include "Tower.h"
#include "PostProcessReport.h"
#include "env_def.h"
#include "XhCharString.h"
#include ".\MainFrm.h"
#include ".\RsltBufferDlg.h"
#include ".\ExportRptFileOptionDlg.h"
#include ".\PostProcess.h"
#include "SortFunc.h"
#include "RevisionOperDlg.h"


// CRsltBufferDlg 对话框
#if defined(__LDS_)||defined(__TSA_)

IMPLEMENT_DYNAMIC(CRsltBufferDlg, CDialog)
CRsltBufferDlg::CRsltBufferDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRsltBufferDlg::IDD, pParent)
	, m_sActiveRsltItemKeyStr(_T(""))
	, m_sCompareRsltItemKeyStr(_T(""))
	, m_sCtrlCaseStr(_T(""))
{
	m_nPointI=0;
}

CRsltBufferDlg::~CRsltBufferDlg()
{
}

void CRsltBufferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_CMB_ACTIVE_RSLT_ITEM_TITLE, m_sActiveRsltItemKeyStr);
	DDX_CBString(pDX, IDC_CMB_COMPARE_RSLT_ITEM_TITLE, m_sCompareRsltItemKeyStr);
	DDX_Text(pDX, IDC_E_CTRL_CASES_STR, m_sCtrlCaseStr);
	DDV_MaxChars(pDX, m_sCtrlCaseStr, 200);
	DDX_Text(pDX, IDC_E_EQUATION_NODE, m_nPointI);
}


BEGIN_MESSAGE_MAP(CRsltBufferDlg, CDialog)
	ON_BN_CLICKED(ID_SAVE_NEW_ITEM, OnBnClickedSaveNewItem)
	ON_BN_CLICKED(ID_DEL_RSLT_ITEM, OnBnClickedDelRsltItem)
	ON_BN_CLICKED(IDC_BN_NODE_EQUATION, OnBnNodeEquation)
	ON_BN_CLICKED(ID_EXPORT_RSLT_RPT_FILE, OnBnClickedExportRsltRptFile)
	ON_BN_CLICKED(ID_CMP_RSLT_RPT, OnBnClickedCmpRsltRpt)
	ON_CBN_SELCHANGE(IDC_CMB_MODULE_NO, OnCbnSelchangeCmbModuleNo)
	ON_BN_CLICKED(IDC_BN_RETRIEVE_CTRL_CASES, OnBnClickedBnRetrieveCtrlCases)
	ON_BN_CLICKED(IDC_BN_EXPORT_ANALYZE_PROC_FILE, OnBnClickedBnExportAnalyzeProcFile)
	ON_CBN_SELCHANGE(IDC_CMB_LEG_QUAD1, OnCbnSelchangeCmbLegQuad1)
	ON_BN_CLICKED(ID_READ_RSLT_ITEM, OnBnClickedReadRsltItem)
END_MESSAGE_MAP()


// CRsltBufferDlg 消息处理程序

BOOL CRsltBufferDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CComboBox* pCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_ACTIVE_RSLT_ITEM_TITLE);
	CComboBox* pCompareCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_COMPARE_RSLT_ITEM_TITLE);
	pCmbBox->ResetContent();
	pCompareCmbBox->ResetContent();
	int i,i2;
	i=0;
	while ((i2=m_cmbRsltItems.Find('|',i)) != -1)
	{
		pCmbBox->AddString(m_cmbRsltItems.Mid(i,i2-i));
		pCompareCmbBox->AddString(m_cmbRsltItems.Mid(i,i2-i));
		i=i2+1;
	}
	if(i<m_cmbRsltItems.GetLength())
	{
		pCmbBox->AddString(m_cmbRsltItems.Right(m_cmbRsltItems.GetLength()-i));
		pCompareCmbBox->AddString(m_cmbRsltItems.Right(m_cmbRsltItems.GetLength()-i));
	}

	pCmbBox->SetWindowText(m_sActiveRsltItemKeyStr);
	pCompareCmbBox->SetWindowText(m_sCompareRsltItemKeyStr);
	//pListBox->SetWindowText(m_sActiveRsltItemKeyStr);
	CButton *pBut=(CButton *)GetDlgItem(ID_SAVE_NEW_ITEM);
	if(m_curBitmap1.LoadBitmap(IDB_BMP_SAVE_BUTTON))
	{
		pBut->SetBitmap(m_curBitmap1);
	}
	pBut=(CButton *)GetDlgItem(ID_DEL_RSLT_ITEM);
	if(m_curBitmap2.LoadBitmap(IDB_BMP_DELETE_BUTTON))
	{
		pBut->SetBitmap(m_curBitmap2);
	}
	pBut=(CButton *)GetDlgItem(ID_READ_RSLT_ITEM);
	if(m_curBitmap3.LoadBitmap(IDB_BMP_READ_BUTTON))
	{
		pBut->SetBitmap(m_curBitmap3);
	}
	CComboBox* pCmb=(CComboBox*)GetDlgItem(IDC_CMB_MODULE_NO);
	bool inited=false;
	for(CLDSModule* pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext()){
		int iSel=pCmb->AddString(CXhChar16("%d",pModule->iNo));
		if(!inited){
			pCmb->SetCurSel(iSel);
			inited=true;
			InitQuadLegs(pModule);
		}
	}

	pCmb=(CComboBox*)GetDlgItem(IDC_CMB_CASE_NO);
	for(CWorkStatus* pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		pCmb->AddString(CXhChar16("%d",pStatus->iNo));
	pCmb->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CRsltBufferDlg::InitQuadLegs(CLDSModule* pModule)
{
	typedef CComboBox* CComboBoxPtr;
	CComboBox* pCmbArr[4];
	pCmbArr[0]=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD1);
	pCmbArr[1]=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD2);
	pCmbArr[2]=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD3);
	pCmbArr[3]=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD4);
	for(int j=0;j<4;j++)
		pCmbArr[j]->ResetContent();
	QUAD_LEG quadLeg;
	bool inited=false;
	for(int i=1;i<=192;i++)
	{
		if(pModule->m_dwLegCfgWord.IsHasNo(i)){
			pCmbArr[0]->AddString(CXhChar16("%C",(i-1)%8+'A'));
			pCmbArr[1]->AddString(CXhChar16("%C",(i-1)%8+'A'));
			pCmbArr[2]->AddString(CXhChar16("%C",(i-1)%8+'A'));
			pCmbArr[3]->AddString(CXhChar16("%C",(i-1)%8+'A'));
		}
	}
	pCmbArr[0]->SetCurSel(0);
	pCmbArr[1]->SetCurSel(0);
	pCmbArr[2]->SetCurSel(0);
	pCmbArr[3]->SetCurSel(0);
}

void CRsltBufferDlg::OnCbnSelchangeCmbLegQuad1()
{
	CComboBox* pCmbArr[4];
	pCmbArr[0]=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD1);
	pCmbArr[1]=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD2);
	pCmbArr[2]=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD3);
	pCmbArr[3]=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD4);
	for(int i=1;i<4;i++)
		pCmbArr[i]->SetCurSel(pCmbArr[0]->GetCurSel());
}

void CRsltBufferDlg::OnCbnSelchangeCmbModuleNo()
{
	CString sModuleNo;
	CComboBox* pCmb=(CComboBox*)GetDlgItem(IDC_CMB_MODULE_NO);
	pCmb->GetWindowText(sModuleNo);
	int iModuleNo=atoi(sModuleNo);
	for(CLDSModule* pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		if(pModule->iNo==iModuleNo)
		{
			InitQuadLegs(pModule);
			return;
		}
	}
}

void CRsltBufferDlg::OnBnClickedSaveNewItem()
{
	UpdateData();
	if(Ta.IsHasRsltBuffer(m_sActiveRsltItemKeyStr.GetBuffer())&&AfxMessageBox("已存在该名称的计算结果，继续将覆盖原计算结果!",MB_YESNO)!=IDYES)
		return;
	if(!Ta.IsHasRsltBuffer(m_sActiveRsltItemKeyStr.GetBuffer()))
	{
		CComboBox* pCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_ACTIVE_RSLT_ITEM_TITLE);
		CComboBox* pCompareCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_COMPARE_RSLT_ITEM_TITLE);
		pCmbBox->AddString(m_sActiveRsltItemKeyStr);
		pCompareCmbBox->AddString(m_sActiveRsltItemKeyStr);
	}
	
	CBuffer buffer;
	postreport.ToBuffer(buffer);	//将当前激活计算结果报告写入到一个数据缓存块中
	if(Ta.WriteRsltBuffer(m_sActiveRsltItemKeyStr,buffer))
	{
		postreport.m_sRsltItem.Copy(m_sActiveRsltItemKeyStr);
		MessageBox(CXhChar50("计算结果[%s]保存成功",m_sActiveRsltItemKeyStr));
	}
	else
		AfxMessageBox(CXhChar50("计算结果[%s]保存失败",m_sActiveRsltItemKeyStr));
}

void CRsltBufferDlg::OnBnClickedReadRsltItem()
{
	UpdateData();
	if(m_sActiveRsltItemKeyStr.Compare("***")!=0)
	{
		CBuffer buffer;
		if(!Ta.ReadRsltBuffer(m_sActiveRsltItemKeyStr,buffer))
		{
			AfxMessageBox(CXhChar50("计算结果[%s]读取失败",m_sActiveRsltItemKeyStr));
			return;
		}
		BeginWaitCursor();
		buffer.SeekToBegin();
		postreport.FromBuffer(buffer);
		postreport.m_sRsltItem.Copy(m_sActiveRsltItemKeyStr);
		EndWaitCursor();
		MessageBox(CXhChar50("计算结果[%s]已读入",m_sActiveRsltItemKeyStr));
	}
	if(theApp.m_pMainWnd)
	{
		CRevisionOperDlg *pRevision=((CMainFrame*)theApp.m_pMainWnd)->GetRevisionOperPage();
		pRevision->UpdateRevision(postreport);
	}
}

void CRsltBufferDlg::OnBnClickedDelRsltItem()
{
	UpdateData();
	if(Ta.RemoveRsltBuffer(m_sActiveRsltItemKeyStr.GetBuffer()))
	{
		CComboBox* pCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_ACTIVE_RSLT_ITEM_TITLE);
		CComboBox* pCompareCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_COMPARE_RSLT_ITEM_TITLE);
		if(pCmbBox->GetCurSel()>=0)
		{
			pCmbBox->DeleteString(pCmbBox->GetCurSel());
			for(int i=0;i<pCompareCmbBox->GetCount();i++)
			{
				CString rString;
				pCompareCmbBox->GetLBText(i,rString);
				if(rString.CompareNoCase(m_sActiveRsltItemKeyStr)==0)
				{
					pCompareCmbBox->DeleteString(i);
					break;
				}
			}
			MessageBox(CXhChar50("计算结果[%s]删除成功!",m_sActiveRsltItemKeyStr));
		}
	}
	else
		AfxMessageBox("已存计算结果中未找到当前项，删除失败!");
}

void CRsltBufferDlg::OnBnClickedExportRsltRptFile()
{
	UpdateData();

	static CExportRptFileOptionDlg rpt_dlg;
	rpt_dlg.m_iEfficType=g_sysPara.iEfficacyCeofType;
	if(rpt_dlg.DoModal()==IDOK)
	{
		CBuffer buffer;
		if( m_sActiveRsltItemKeyStr.Compare("***")!=0 && 
			!postreport.m_sRsltItem.EqualNoCase(m_sActiveRsltItemKeyStr))
		{
			if(!Ta.ReadRsltBuffer(m_sActiveRsltItemKeyStr,buffer))
			{
				AfxMessageBox(CXhChar50("计算结果[%s]读取失败",m_sActiveRsltItemKeyStr));
				return;
			}
			buffer.SeekToBegin();
			postreport.FromBuffer(buffer);
			postreport.m_sRsltItem.Copy(m_sActiveRsltItemKeyStr);
		}
		//结果文件目录生成
		char file_path[MAX_PATH], file_name[MAX_PATH];
		CString ss=theApp.GetLDSDoc()->GetPathName();
		_snprintf(file_path,MAX_PATH-1,"%s",ss);
		GetSysPath(file_path,file_path,file_name);
		strcat(file_path,file_name);
		strcat(file_path,"\\");
		strcat(file_path,file_name);
		g_sysPara.iEfficacyCeofType=rpt_dlg.m_iEfficType;
		if(rpt_dlg.m_iFileLanguage == 0)
			CreateDetailRptFileFromReportByCN(file_path,rpt_dlg.m_iDocFileType);
		else if(rpt_dlg.m_iFileLanguage == 1)
			CreateDetailRptFileFromReportByEN(file_path,rpt_dlg.m_iDocFileType);
	}
}

void CRsltBufferDlg::OnBnClickedCmpRsltRpt()
{
	UpdateData();
	if(m_sActiveRsltItemKeyStr.CompareNoCase(m_sCompareRsltItemKeyStr)==0)
	{
		AfxMessageBox("对比的是同一计算结果!");
		return;
	}
	CBuffer buffer;
	bool rslt1=false,rslt2=false;
	if(m_sActiveRsltItemKeyStr.Find('*')>=0)
		rslt1=true;	//当前默认值
	else if(!postreport.m_sRsltItem.EqualNoCase(m_sActiveRsltItemKeyStr))
	{
		if(rslt1=Ta.ReadRsltBuffer(m_sActiveRsltItemKeyStr,buffer))
		{
			buffer.SeekToBegin();
			postreport.FromBuffer(buffer);
			postreport.m_sRsltItem.Copy(m_sActiveRsltItemKeyStr);
		}
	}
	else	//当前计算结果集，不需要再次读取
		rslt1=true;
	if(rslt1)
		rslt2=Ta.ReadRsltBuffer(m_sCompareRsltItemKeyStr.GetBuffer(),buffer);
	if(rslt1&&rslt2)
	{
		CPostProcessReport legacyreport;
		buffer.SeekToBegin();
		legacyreport.FromBuffer(buffer);
		CLogFile rpt("D:\\FemTestReport.txt");
		postreport.CompareWithLegacyReport(legacyreport,rpt);
		CDialog::OnOK();
	}
	else if(!rslt1)
		AfxMessageBox(CXhChar50("计算结果[%s]读取失败",m_sActiveRsltItemKeyStr));
	else //if(!rslt2)
		AfxMessageBox(CXhChar50("计算结果[%s]读取失败",m_sCompareRsltItemKeyStr));
}

void CRsltBufferDlg::OnBnNodeEquation()
{
	UpdateData();
	if( m_sActiveRsltItemKeyStr.Compare("***")!=0 &&
		!postreport.m_sRsltItem.EqualNoCase(m_sActiveRsltItemKeyStr))
	{
		CBuffer buffer;
		if(!Ta.ReadRsltBuffer(m_sActiveRsltItemKeyStr,buffer))
		{
			AfxMessageBox(CXhChar50("计算结果[%s]读取失败",m_sActiveRsltItemKeyStr));
			return;
		}
		buffer.SeekToBegin();
		postreport.FromBuffer(buffer);
		postreport.m_sRsltItem.Copy(m_sActiveRsltItemKeyStr);
	}
	//分析过程文件生成
	CString sModuleNo,sCaseNo,sQuadLeg1,sQuadLeg2,sQuadLeg3,sQuadLeg4;
	GetDlgItem(IDC_CMB_MODULE_NO)->GetWindowText(sModuleNo);
	GetDlgItem(IDC_CMB_CASE_NO)->GetWindowText(sCaseNo);
	GetDlgItem(IDC_CMB_LEG_QUAD1)->GetWindowText(sQuadLeg1);
	GetDlgItem(IDC_CMB_LEG_QUAD2)->GetWindowText(sQuadLeg2);
	GetDlgItem(IDC_CMB_LEG_QUAD3)->GetWindowText(sQuadLeg3);
	GetDlgItem(IDC_CMB_LEG_QUAD4)->GetWindowText(sQuadLeg4);
	long iModuleNo=atoi(sModuleNo);
	char cEiffel=sCaseNo[sCaseNo.GetLength()-1];
	SUBID iCaseNo;
	if(cEiffel>='A'&&cEiffel<='Z')
		sCaseNo.Remove(cEiffel);
	else
		cEiffel=0;
	if(cEiffel>0)
		iCaseNo=SUBID(atoi(sCaseNo),cEiffel-'A'+1);
	else
		iCaseNo=SUBID(atoi(sCaseNo));
	QUAD_LEG quadLeg;
	quadLeg.arrQuad[0]=sQuadLeg1[0]-'A'+1;
	quadLeg.arrQuad[1]=sQuadLeg2[0]-'A'+1;
	quadLeg.arrQuad[2]=sQuadLeg3[0]-'A'+1;
	quadLeg.arrQuad[3]=sQuadLeg4[0]-'A'+1;

	char file_path[MAX_PATH], file_name[MAX_PATH];
	CString ss=theApp.GetLDSDoc()->GetPathName();
	_snprintf(file_path,MAX_PATH-1,"%s",ss);
	GetSysPath(file_path,file_path,file_name);
	_chdir(file_path);	//确保有该子目录
	CXhChar200 subdir("%s_worktemp",file_name);
	_mkdir(subdir);
	strcat(file_path,subdir);
	strcat(file_path,"\\");
	CXhChar50 proc_str;
	if(iCaseNo.Group()>0)
		proc_str.Printf("%d-%d%C-%C%C%C%C%d",iModuleNo,iCaseNo.Id(),iCaseNo.Group()+'@',	//'@'='A'-1
		sQuadLeg1[0],sQuadLeg2[0],sQuadLeg3[0],sQuadLeg4[0],m_nPointI);
	else
		proc_str.Printf("%d-%d-%C%C%C%C#%d",iModuleNo,iCaseNo.Id(),
		sQuadLeg1[0],sQuadLeg2[0],sQuadLeg3[0],sQuadLeg4[0],m_nPointI);
	strcat(file_path,proc_str);
	if(postreport.CreateNodeEquation(file_path,iModuleNo,quadLeg.dwQuadLeg,iCaseNo,m_nPointI))
		WinExec(CXhChar200("notepad.exe %s.bal",file_path),SW_SHOW);
}
void CRsltBufferDlg::OnBnClickedBnExportAnalyzeProcFile()
{
	UpdateData();
	if( m_sActiveRsltItemKeyStr.Compare("***")!=0 &&
		!postreport.m_sRsltItem.EqualNoCase(m_sActiveRsltItemKeyStr))
	{
		CBuffer buffer;
		if(!Ta.ReadRsltBuffer(m_sActiveRsltItemKeyStr,buffer))
		{
			AfxMessageBox(CXhChar50("计算结果[%s]读取失败",m_sActiveRsltItemKeyStr));
			return;
		}
		buffer.SeekToBegin();
		postreport.FromBuffer(buffer);
		postreport.m_sRsltItem.Copy(m_sActiveRsltItemKeyStr);
	}
	//分析过程文件生成
	CString sModuleNo,sCaseNo,sQuadLeg1,sQuadLeg2,sQuadLeg3,sQuadLeg4;
	GetDlgItem(IDC_CMB_MODULE_NO)->GetWindowText(sModuleNo);
	GetDlgItem(IDC_CMB_CASE_NO)->GetWindowText(sCaseNo);
	GetDlgItem(IDC_CMB_LEG_QUAD1)->GetWindowText(sQuadLeg1);
	GetDlgItem(IDC_CMB_LEG_QUAD2)->GetWindowText(sQuadLeg2);
	GetDlgItem(IDC_CMB_LEG_QUAD3)->GetWindowText(sQuadLeg3);
	GetDlgItem(IDC_CMB_LEG_QUAD4)->GetWindowText(sQuadLeg4);
	long iModuleNo=atoi(sModuleNo);
	char cEiffel=sCaseNo[sCaseNo.GetLength()-1];
	SUBID iCaseNo;
	if(cEiffel>='A'&&cEiffel<='Z')
		sCaseNo.Remove(cEiffel);
	else
		cEiffel=0;
	if(cEiffel>0)
		iCaseNo=SUBID(atoi(sCaseNo),cEiffel-'A'+1);
	else
		iCaseNo=SUBID(atoi(sCaseNo));
	QUAD_LEG quadLeg;
	quadLeg.arrQuad[0]=sQuadLeg1[0]-'A'+1;
	quadLeg.arrQuad[1]=sQuadLeg2[0]-'A'+1;
	quadLeg.arrQuad[2]=sQuadLeg3[0]-'A'+1;
	quadLeg.arrQuad[3]=sQuadLeg4[0]-'A'+1;

	char file_path[MAX_PATH], file_name[MAX_PATH];
	CString ss=theApp.GetLDSDoc()->GetPathName();
	_snprintf(file_path,MAX_PATH-1,"%s",ss);
	GetSysPath(file_path,file_path,file_name);
	_chdir(file_path);	//确保有该子目录
	CXhChar200 subdir("%s_worktemp",file_name);
	_mkdir(subdir);
	strcat(file_path,subdir);
	strcat(file_path,"\\");
	CXhChar50 proc_str;
	if(iCaseNo.Group()>0)
		proc_str.Printf("%d-%d%C-%C%C%C%C",iModuleNo,iCaseNo.Id(),iCaseNo.Group()+'@',	//'@'='A'-1
			sQuadLeg1[0],sQuadLeg2[0],sQuadLeg3[0],sQuadLeg4[0]);
	else
		proc_str.Printf("%d-%d-%C%C%C%C",iModuleNo,iCaseNo.Id(),
			sQuadLeg1[0],sQuadLeg2[0],sQuadLeg3[0],sQuadLeg4[0]);
	strcat(file_path,proc_str);
	if(postreport.CreateAnalyzeFile(file_path,iModuleNo,quadLeg.dwQuadLeg,iCaseNo))
		WinExec(CXhChar200("notepad.exe %s.anl",file_path),SW_SHOW);
}
struct REACT_FORCE{
	long hNode;
	double fMaxForce,fMinForce;
	CWorkStatus *pMaxForceStatus,*pMinForceStatus;
	REACT_FORCE(){hNode=0;fMaxForce=fMinForce=0;pMaxForceStatus=pMinForceStatus=NULL;}
};
void CRsltBufferDlg::OnBnClickedBnRetrieveCtrlCases()
{
	UpdateData();
	if( m_sActiveRsltItemKeyStr.Compare("***")!=0 && 
		!postreport.m_sRsltItem.EqualNoCase(m_sActiveRsltItemKeyStr))
	{
		CBuffer buffer;
		if(!Ta.ReadRsltBuffer(m_sActiveRsltItemKeyStr.GetBuffer(),buffer))
		{
			AfxMessageBox(CXhChar50("计算结果[%s]读取失败",m_sActiveRsltItemKeyStr));
			return;
		}
		buffer.SeekToBegin();
		postreport.FromBuffer(buffer);
		postreport.m_sRsltItem.Copy(m_sActiveRsltItemKeyStr);
	}
	REACT_FORCE* pReactForce;
	CHashList<REACT_FORCE>hashCtrlReactCase;
	//基础反作用力,提取最大上拨力及下压力发生工况号
	for(BASENODE_REACTION* pReaction=postreport.Reaction.GetFirst();pReaction;pReaction=postreport.Reaction.GetNext())
	{
		pReactForce=hashCtrlReactCase.GetValue(pReaction->pNode->handle);
		if(pReactForce==NULL)
			pReactForce=hashCtrlReactCase.Add(pReaction->pNode->handle);
		if(pReaction->reaction_force_design.z>pReactForce->fMaxForce)
		{
			pReactForce->pMaxForceStatus=pReaction->pStatus;
			pReactForce->fMaxForce=pReaction->reaction_force_design.z;
		}
		if(pReaction->reaction_force_design.z<pReactForce->fMinForce)
		{
			pReactForce->pMinForceStatus=pReaction->pStatus;
			pReactForce->fMinForce=pReaction->reaction_force_design.z;
		}
	}
	CHashList<long> hashCtrlCase;	//以荷载工况的标识序号为键值
	for(pReactForce=hashCtrlReactCase.GetFirst();pReactForce;pReactForce=hashCtrlReactCase.GetNext())
	{
		if(pReactForce->pMaxForceStatus)
			hashCtrlCase.SetValue(pReactForce->pMaxForceStatus->iNo,pReactForce->pMaxForceStatus->iNo);
		if(pReactForce->pMinForceStatus)
			hashCtrlCase.SetValue(pReactForce->pMinForceStatus->iNo,pReactForce->pMinForceStatus->iNo);
	}

	// 应力计算时将长杆打碎后的全塔杆件（一个计算操作内的集合），键值为单元标识CElement::id
	for(CElement* pElem=postreport.hashElems.GetFirst();pElem;pElem=postreport.hashElems.GetNext())
	{
		CWorkStatus *pStatus;
		if(pElem->MaxTension.hWorkStatus.Id()>0)
		{
			if((pStatus=Ta.WorkStatus.FromHandle(pElem->MaxTension.hWorkStatus.Id()))!=NULL)
				hashCtrlCase.SetValue(pStatus->iNo,pStatus->iNo);
		}
		if(pElem->MaxCompression.hWorkStatus.Id()>0)
		{
			if((pStatus=Ta.WorkStatus.FromHandle(pElem->MaxCompression.hWorkStatus.Id()))!=NULL)
				hashCtrlCase.SetValue(pStatus->iNo,pStatus->iNo);
		}
		if(pElem->MaxCompStable.hWorkStatus.Id()>0)
		{
			if((pStatus=Ta.WorkStatus.FromHandle(pElem->MaxCompStable.hWorkStatus.Id()))!=NULL)
				hashCtrlCase.SetValue(pStatus->iNo,pStatus->iNo);
		}
	}
	ARRAY_LIST<long> arrSerialI(0,hashCtrlCase.GetNodeNum());
	for(long *pStatusNo=hashCtrlCase.GetFirst();pStatusNo;pStatusNo=hashCtrlCase.GetNext())
		arrSerialI.append(*pStatusNo);
	CQuickSort<long>::QuickSort(arrSerialI.m_pData,arrSerialI.GetSize(),SimpleCompareFunc<long>);
	ATOM_LIST<SEGI>listSegI;
	for(int i=0;i<arrSerialI.GetSize();i++)
		listSegI.append(SEGI(arrSerialI[i]));
	m_sCtrlCaseStr=CTower::SegNoListToString(listSegI);
	UpdateData(FALSE);
	//CEdit* pEdit=(CEdit*)GetDlgItem(IDC_E_CTRL_CASES_STR);
	//pEdit->SetSel(0,-1,TRUE);
}
#endif
