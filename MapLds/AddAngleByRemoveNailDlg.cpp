// AddAngleByRemoveNailDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AddAngleByRemoveNailDlg.h"

static BOOL FirePartItemValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,int iSubItem,CString& valueStr)
{
	CPartSetCreatorDlg::STDPART_ITEM *pPartItem=(CPartSetCreatorDlg::STDPART_ITEM*)pItem->m_idProp;
	if(pPartItem==NULL)
		return FALSE;
	if(iSubItem==1)
		pPartItem->sLabelWithOutNail.Copy(valueStr);
	return TRUE;
}

// CAddAngleByRemoveNailDlg 对话框

IMPLEMENT_DYNAMIC(CAddAngleByRemoveNailDlg, CDialog)

CAddAngleByRemoveNailDlg::CAddAngleByRemoveNailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddAngleByRemoveNailDlg::IDD, pParent)
{
	m_pPartItemSet=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	m_xAngleListCtrl.AddColumnHeader("Part Label");
	m_xAngleListCtrl.AddColumnHeader("Part Label Without Nail",100);
	m_xAngleListCtrl.AddColumnHeader("Spec",100);
	m_xAngleListCtrl.AddColumnHeader("Quantity");
	m_xAngleListCtrl.AddColumnHeader("Description");
#else
	m_xAngleListCtrl.AddColumnHeader("构件编号");
	m_xAngleListCtrl.AddColumnHeader("不带脚钉编号",100);
	m_xAngleListCtrl.AddColumnHeader("规格",100);
	m_xAngleListCtrl.AddColumnHeader("件数",60);
	m_xAngleListCtrl.AddColumnHeader("描述");
#endif
}

CAddAngleByRemoveNailDlg::~CAddAngleByRemoveNailDlg()
{
}

void CAddAngleByRemoveNailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ANGLE_LIST, m_xAngleListCtrl);
}


BEGIN_MESSAGE_MAP(CAddAngleByRemoveNailDlg, CDialog)
END_MESSAGE_MAP()


// CAddAngleByRemoveNailDlg 消息处理程序
BOOL CAddAngleByRemoveNailDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(m_pPartItemSet==NULL)
		return FALSE;
	m_xAngleListCtrl.InitListCtrl();
	m_xAngleListCtrl.SetModifyValueFunc(FirePartItemValueModify);
	CXhChar50 sTemp;
	CListCtrlItemInfo* lpInfo=NULL;
	CPartSetCreatorDlg::STDPART_ITEM *pPartItem=NULL;
	for(pPartItem=m_pPartItemSet->GetFirst();pPartItem;pPartItem=m_pPartItemSet->GetNext())
	{
		CLDSPart *pPart=pPartItem->GetPart();
		if(pPart==NULL||!pPart->IsAngle()||!((CLDSLineAngle*)pPart)->IsHasFootNail())
			continue;
		lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,pPart->GetPartNo(),TRUE);
		lpInfo->SetSubItemText(1,pPartItem->sLabelWithOutNail);
		pPart->GetSpecification(sTemp);
		lpInfo->SetSubItemText(2,sTemp,TRUE);
		lpInfo->SetSubItemText(3,CXhChar50("%d",pPartItem->num),TRUE);
		sTemp.Empty();
		((CLDSLineAngle*)pPart)->GetProcessStr(sTemp,50);
		lpInfo->SetSubItemText(4,sTemp,TRUE);
		CSuperGridCtrl::CTreeItem* pItem=m_xAngleListCtrl.InsertRootItem(lpInfo);
		pItem->m_idProp=(long)pPartItem;
	}
	return TRUE;
}
