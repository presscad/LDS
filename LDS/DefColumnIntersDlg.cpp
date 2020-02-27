// DefColumnIntersDlg.cpp : 实现文件
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "DefColumnIntersDlg.h"
#include "lds_part.h"
#include "env_def.h"
#include "DisplayPointProperty.h"
#include ".\defcolumnintersdlg.h"

// CDefColumnIntersDlg 对话框

IMPLEMENT_DYNAMIC(CDefColumnIntersDlg, CDialog)
CDefColumnIntersDlg::CDefColumnIntersDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDefColumnIntersDlg::IDD, pParent)
{
	m_idEventProp=0;
}

CDefColumnIntersDlg::~CDefColumnIntersDlg()
{
}

void CDefColumnIntersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);	
}


BEGIN_MESSAGE_MAP(CDefColumnIntersDlg, CDialog)
	
	ON_BN_CLICKED(ID_BTN_CONFIRM, OnBnClickedBtnConfirm)
END_MESSAGE_MAP()

// CDefColumnIntersDlg 消息处理程序
BOOL CDefColumnIntersDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.m_pObj=&m_columnPoint;
	m_propList.SetDividerScale(0.4);
	m_propList.SetModifyValueFunc(CDisplayPointProperty::ModifyDatumPointPropValue);
	m_propList.SetButtonClickFunc(CDisplayPointProperty::DatumPointButtonClick);
	//显示基准线信息
	if(m_columnPoint.des_para.COLUMN_INTERS.hLineTube>0X20)
	{
		CLDSLineTube* pTube=(CLDSLineTube*)Ta.FromPartHandle(m_columnPoint.des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE);
		f3dPoint axis_vec=pTube->End()-pTube->Start();
		normalize(axis_vec);
		m_columnPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec[0]=axis_vec.x;
		m_columnPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec[1]=axis_vec.y;
		m_columnPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec[2]=axis_vec.z;
		m_columnPoint.des_para.COLUMN_INTERS.column_r=pTube->GetWidth()*0.5;
	}
	if(m_bInernalStart)	//完成选择对象的后续操作
		CDisplayPointProperty::FinishSelectObjOper(&m_columnPoint,m_hPickObj,GetEventPropId());
	DisplayColumnPointProperty();
	if(m_bInernalStart)	
	{	//恢复选中项
		m_propList.Redraw();
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		m_propList.SetFocus();
		m_propList.SetCurSel(pItem->m_iIndex);	//设置选中项	
	}
	UpdateData(FALSE);
	return TRUE;
}
void CDefColumnIntersDlg::DisplayColumnPointProperty()
{
	m_propList.CleanTree();
	CPropTreeItem *pGroupItem,*pRoot=m_propList.GetRootItem();
	//柱面信息
	CItemInfo*lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Cylndrical Surface Information";
#else 
	lpInfo->m_strPropName = "柱面信息";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pGroupItem->m_idProp = CLDSPoint::GetPropID("des_para.COLUMN_INTERS.column");
	pGroupItem->m_bHideChildren=FALSE;
	//柱面点参数
	CDisplayPointProperty::UpdateDatumPointPropItem(&m_propList,pGroupItem,&m_columnPoint,FALSE);

	m_propList.Redraw();
}

void CDefColumnIntersDlg::OnOK()
{
	//屏蔽回车键，避免修改属性栏后，按回车时关闭对话框
}

void CDefColumnIntersDlg::OnBnClickedBtnConfirm()
{
	CDialog::OnOK();
}
