// BetweenSpaceDlg.cpp : 实现文件
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "XhCharString.h"
#include "MultiLineInputDlg.h"
// CMultiLineInputDlg 对话框

IMPLEMENT_DYNAMIC(CMultiLineInputDlg, CDialog)
CMultiLineInputDlg::CMultiLineInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMultiLineInputDlg::IDD, pParent)
{
	m_sRichSpaceValue=_T("");
	m_sSpaceValue=_T("");
}

CMultiLineInputDlg::~CMultiLineInputDlg()
{
}

void CMultiLineInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICH_SPACE, m_xRichCtrl);
	DDX_Text(pDX, IDC_RICH_SPACE, m_sRichSpaceValue);
}


BEGIN_MESSAGE_MAP(CMultiLineInputDlg, CDialog)
	ON_BN_CLICKED(ID_CONFIRM_INPUT, OnBnClickedConfirmInput)
END_MESSAGE_MAP()


// CMultiLineInputDlg 消息处理程序
BOOL CMultiLineInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_xRichCtrl.SetEventMask(m_xRichCtrl.GetEventMask()|ENM_KEYEVENTS|ENM_MOUSEEVENTS);
	CXhChar100 tem_str="";
	CString sValue="";
	tem_str.Printf("%s",m_sSpaceValue);
	for(char* key=strtok(tem_str,",");key;key=strtok(NULL,","))
	{
		sValue.Format("%s\n",key);
		m_sRichSpaceValue+=sValue;
	}
	UpdateData(FALSE);
	return TRUE;
}
void CMultiLineInputDlg::OnOK()
{

}
BOOL IsFigure(CString sText)
{
	int nLen = sText.GetLength();
	for(int i=0;i<nLen;i++)
	{
		if(!isdigit(sText[i]))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Please input correct data!");
#else 
			AfxMessageBox("请输入正确的数据!");
#endif
			return FALSE; 
		}
	}
	return TRUE;
}
void CMultiLineInputDlg::OnBnClickedConfirmInput()
{
	UpdateData();
	CXhChar100 tem_str="";
	CString sText="";
	CString sValue="";
	strcpy(tem_str,m_sRichSpaceValue);
	int uStrLen = strlen(tem_str);
	for(int i=0;i<uStrLen;i++)
	{
		if(tem_str[i] == '\n'&&i<uStrLen-1)
		{
			sText.Trim();
			if(IsFigure(sText))
			{
				sValue+=sText;
				sValue+=",";
				sText="";
			}
		}
		else
			sText+=tem_str[i];
		if(i == uStrLen-1)
		{
			sText.Trim();
			if(IsFigure(sText))
				sValue+=sText;
		}
	}
	m_sRichSpaceValue.Format("%s",sValue);
	UpdateData(FALSE);
	CDialog::OnOK();
}
