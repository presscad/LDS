// ModifyPartNoSegIDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ModifyPartNoSegIDlg.h"

// CModifyPartNoSegIDlg 对话框

IMPLEMENT_DYNAMIC(CModifyPartNoSegIDlg, CDialog)
CModifyPartNoSegIDlg::CModifyPartNoSegIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyPartNoSegIDlg::IDD, pParent)
	, m_sNewSegI(_T(""))
{
}

CModifyPartNoSegIDlg::~CModifyPartNoSegIDlg()
{
}

void CModifyPartNoSegIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_SEGI, m_cmbSegI);
	DDX_Text(pDX, IDC_E_NEW_SEGI, m_sNewSegI);
}


BEGIN_MESSAGE_MAP(CModifyPartNoSegIDlg, CDialog)
END_MESSAGE_MAP()


// CModifyPartNoSegIDlg 消息处理程序

BOOL CModifyPartNoSegIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cmbSegI.Clear();
	ATOM_LIST<SEGI> segNoList;
	int nSegNum=Ta.GetSegNoList(segNoList);
	CString sItem;
	for(SEGI *pSegI=segNoList.GetFirst();pSegI;pSegI=segNoList.GetNext())
	{
		sItem=pSegI->ToString();
		int iItem=m_cmbSegI.AddString(sItem);
		m_cmbSegI.SetItemData(iItem,(DWORD)pSegI->iSeg);
	}
	if(m_cmbSegI.GetCount()>0)
		m_cmbSegI.SetCurSel(0);
	return TRUE;  
}

void CModifyPartNoSegIDlg::OnOK()
{
	UpdateData();
	int iCurSel=m_cmbSegI.GetCurSel();
	if(iCurSel>=0)
		m_oldSegI = SEGI(m_cmbSegI.GetItemData(iCurSel));
	else 
		m_oldSegI = SEGI(-1);
	m_newSegI = SEGI(m_sNewSegI.GetBuffer());

	CDialog::OnOK();
}