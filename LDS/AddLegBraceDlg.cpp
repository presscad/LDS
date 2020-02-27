// AddLegBraceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "XhCharString.h"
#include "AddLegBraceDlg.h"
#include "afxdialogex.h"


// CAddLegBraceDlg 对话框

IMPLEMENT_DYNAMIC(CAddLegBraceDlg, CDialogEx)

CAddLegBraceDlg::CAddLegBraceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddLegBraceDlg::IDD, pParent)
	, m_sLegScript(_T(""))
	, m_sNote(_T(""))
{

}

CAddLegBraceDlg::~CAddLegBraceDlg()
{
}

void CAddLegBraceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_REDIT_SCRIPT, m_sLegScript);
	DDX_Text(pDX, IDC_S_NOTE, m_sNote);
}


BEGIN_MESSAGE_MAP(CAddLegBraceDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BN_CREATE_SAMPLE_SCRIPT, &CAddLegBraceDlg::OnBnCreateSampleScript)
	ON_BN_CLICKED(IDC_IMPORT_LEG_BRACE_SCRIPT_FILE, &CAddLegBraceDlg::OnImportLegBraceScriptFile)
	ON_BN_CLICKED(IDC_EXPORT_LEG_BRACE_SCRIPT_FILE, &CAddLegBraceDlg::OnExportLegBraceScriptFile)
	ON_BN_CLICKED(IDC_BN_CONFIRM, &CAddLegBraceDlg::OnBnConfirm)
END_MESSAGE_MAP()


// CAddLegBraceDlg 消息处理程序
BOOL CAddLegBraceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_sNote = "脚本编辑：\n";
	m_sNote += " 通过按键Ctrl+Enter进行换行\n";
	m_sNote += " 通过Ctrl+C和Ctrl+V方式进行快速编辑\n\n";
	m_sNote += "脚本格式：\n";
	m_sNote += "<Height#> '#'后数据表示呼高号\n";
	m_sNote += "<level=>  '='后数据表示相邻接腿级差\n";
	m_sNote += "<Leg#>    '#'后数据表示接腿号\n";
	m_sNote += "<front=>  '='后数据表示正侧面布局\n";
	m_sNote += "<vplane=> '='后数据表示V面布局\n";
	m_sNote += "<number=> '='后数据表示等分节间数\n";
	m_sNote += "<spec=>   '='后数据表示接腿斜材规格\n";
	UpdateData(FALSE);
	return TRUE;
}

void CAddLegBraceDlg::OnOK()
{
}

void CAddLegBraceDlg::OnCancel()
{
	return CDialogEx::OnCancel();
}

void CAddLegBraceDlg::OnBnConfirm()
{
	UpdateData();
	CXhChar200 sFile("%sleg.lbt", theApp.execute_path);
	FILE* fp = fopen(sFile, "wt");
	if (fp == NULL)
	{
		AfxMessageBox("文件打开失败");
		return;
	}
	fprintf(fp, "%s", m_sLegScript);
	fclose(fp);
	return CDialog::OnOK();
}

void CAddLegBraceDlg::OnBnCreateSampleScript()
{
	m_sLegScript = "Height#1: level=1.5m\n";
	m_sLegScript+= " Leg#1:front=L1,vplane=V3A,number=6,spec=L70x5\n";
	m_sLegScript+= " Leg#2:front=L1,vplane=V3A,number=5,spec=L70x5\n";
	m_sLegScript+= " Leg#3:front=L1,vplane=V3A,number=4,spec=L70x5\n";
	m_sLegScript+= " Leg#4:front=L1,vplane=V3A,number=3,spec=L70x5\n";
	m_sLegScript+= " Leg#5:front=L1,vplane=V3A,number=2,spec=L70x5\n";
	m_sLegScript+= "Height#2: level=1.5m\n";
	m_sLegScript+= " Leg#1:front=L1,vplane=V3A,number=6,spec=L70x5\n";
	m_sLegScript+= " Leg#2:front=L1,vplane=V3A,number=5,spec=L70x5\n";
	m_sLegScript+= " Leg#3:front=L1,vplane=V3A,number=4,spec=L70x5\n";
	m_sLegScript+= " Leg#4:front=L1,vplane=V3A,number=3,spec=L70x5\n";
	m_sLegScript+= " Leg#5:front=L1,vplane=V3A,number=2,spec=L70x5";
	UpdateData(FALSE);
}


void CAddLegBraceDlg::OnImportLegBraceScriptFile()
{
	//读取布材方案，初始化设计信息
	CFileDialog fileDlg(TRUE, "接腿布材", "", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "接腿布材(*.lbt)|*.lbt|所有文件(*.*)|*.*||");
	if (fileDlg.DoModal() != IDOK)
		return;
	FILE* fp=fopen(fileDlg.GetPathName(),"rt");
	if(fp==NULL)
	{
		AfxMessageBox("文件打开失败");
		return;
	}
	m_sLegScript="";
	while(!feof(fp))
	{
		CXhChar200 lineText;
		if(fgets(lineText,200,fp)!=NULL)
			m_sLegScript+=lineText;
		else
			break;
	}
	fclose(fp);
	UpdateData(FALSE);
}

void CAddLegBraceDlg::OnExportLegBraceScriptFile()
{
	CFileDialog fileDlg(FALSE, "接腿布材", "LegDef", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "接腿布材(*.lbt)|*.lbt|所有文件(*.*)|*.*||");
	if (fileDlg.DoModal() != IDOK)
		return;
	UpdateData();
	FILE* fp=fopen(fileDlg.GetPathName(),"wt");
	if(fp==NULL)
	{
		AfxMessageBox("文件打开失败");
		return;
	}
	fprintf(fp,"%s",m_sLegScript);
	fclose(fp);
}
