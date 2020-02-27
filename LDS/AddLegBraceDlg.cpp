// AddLegBraceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "XhCharString.h"
#include "AddLegBraceDlg.h"
#include "afxdialogex.h"


// CAddLegBraceDlg �Ի���

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


// CAddLegBraceDlg ��Ϣ�������
BOOL CAddLegBraceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_sNote = "�ű��༭��\n";
	m_sNote += " ͨ������Ctrl+Enter���л���\n";
	m_sNote += " ͨ��Ctrl+C��Ctrl+V��ʽ���п��ٱ༭\n\n";
	m_sNote += "�ű���ʽ��\n";
	m_sNote += "<Height#> '#'�����ݱ�ʾ���ߺ�\n";
	m_sNote += "<level=>  '='�����ݱ�ʾ���ڽ��ȼ���\n";
	m_sNote += "<Leg#>    '#'�����ݱ�ʾ���Ⱥ�\n";
	m_sNote += "<front=>  '='�����ݱ�ʾ�����沼��\n";
	m_sNote += "<vplane=> '='�����ݱ�ʾV�沼��\n";
	m_sNote += "<number=> '='�����ݱ�ʾ�ȷֽڼ���\n";
	m_sNote += "<spec=>   '='�����ݱ�ʾ����б�Ĺ��\n";
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
		AfxMessageBox("�ļ���ʧ��");
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
	//��ȡ���ķ�������ʼ�������Ϣ
	CFileDialog fileDlg(TRUE, "���Ȳ���", "", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "���Ȳ���(*.lbt)|*.lbt|�����ļ�(*.*)|*.*||");
	if (fileDlg.DoModal() != IDOK)
		return;
	FILE* fp=fopen(fileDlg.GetPathName(),"rt");
	if(fp==NULL)
	{
		AfxMessageBox("�ļ���ʧ��");
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
	CFileDialog fileDlg(FALSE, "���Ȳ���", "LegDef", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "���Ȳ���(*.lbt)|*.lbt|�����ļ�(*.*)|*.*||");
	if (fileDlg.DoModal() != IDOK)
		return;
	UpdateData();
	FILE* fp=fopen(fileDlg.GetPathName(),"wt");
	if(fp==NULL)
	{
		AfxMessageBox("�ļ���ʧ��");
		return;
	}
	fprintf(fp,"%s",m_sLegScript);
	fclose(fp);
}
