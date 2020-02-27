// CrossArmDesignDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "CrossArmDesignDlg.h"
#include "afxdialogex.h"
#include "GlobalFunc.h"

//////////////////////////////////////////////////////////////////////////
static BOOL ModifyItemValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CCrossArmDesignDlg* pDlg=(CCrossArmDesignDlg*)pPropList->GetParent();
	if(pItem->m_idProp==1)	//�ؼ�������
	{
		pDlg->m_nKeyNodeNum=atoi(valueStr);
		pDlg->ChangeKeyNodeNum();
		if(pDlg->m_nKeyNodeNum==2)
		{
			pDlg->m_iKeyNodeMir=1;
			pPropList->SetItemPropValue(2,"X��");
		}
		else if(pDlg->m_nKeyNodeNum==4)
		{
			pDlg->m_iKeyNodeMir=0;
			pPropList->SetItemPropValue(2,"�޶Գ�");
		}
	}
	else if(pItem->m_idProp==2)	//�ؼ���Գ�
	{
		if(valueStr.CompareNoCase("�޶Գ�")==0)
			pDlg->m_iKeyNodeMir=0;
		else if(valueStr.CompareNoCase("X��")==0)
			pDlg->m_iKeyNodeMir=1;
		else if(valueStr.CompareNoCase("Y��")==0)
			pDlg->m_iKeyNodeMir=2;
		else if(valueStr.CompareNoCase("XYZ��")==0)
			pDlg->m_iKeyNodeMir=3;
		else
			return FALSE;
	}
	else if(pItem->m_idProp==3)	//ģ����Գ�
	{
		if(valueStr.CompareNoCase("�޶Գ�")==0)
			pDlg->m_iStdFaceMir=0;
		else if(valueStr.CompareNoCase("X��")==0)
			pDlg->m_iStdFaceMir=1;
		else if(valueStr.CompareNoCase("Y��")==0)
			pDlg->m_iStdFaceMir=2;
		else if(valueStr.CompareNoCase("Z��")==0)
			pDlg->m_iStdFaceMir=3;
		else if(valueStr.CompareNoCase("����")==0)
			pDlg->m_iStdFaceMir=4;
		else if(valueStr.CompareNoCase("Z��_����")==0)
			pDlg->m_iStdFaceMir=5;
		else
			return FALSE;
	}
	else if(pItem->m_idProp!=0)
	{
		DESIGN_PARAM_ITEM* pParam=pDlg->m_pModel->GetParamItem(pItem->m_idProp);
		if(pParam==NULL)
			return FALSE;
		if(pParam->dataType==DESIGN_PARAM_ITEM::TYPE_INTEGER)
			pParam->value.iType=atoi(valueStr);
		else if(pParam->dataType==DESIGN_PARAM_ITEM::TYPE_DOUBLE)
			pParam->value.fValue=atoi(valueStr);
	}
	return TRUE;
}
// CCrossArmDesignDlg �Ի���

IMPLEMENT_DYNAMIC(CCrossArmDesignDlg, CDialog)

CCrossArmDesignDlg::CCrossArmDesignDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCrossArmDesignDlg::IDD, pParent)
{
	m_iCrossArmType=0;
	m_nKeyNodeNum=2;
	m_iKeyNodeMir=1;
	m_iStdFaceMir=0;
	m_sKeyNodeOrder="";
}

CCrossArmDesignDlg::~CCrossArmDesignDlg()
{
}

void CCrossArmDesignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_CBIndex(pDX, IDC_CMB_CROSEE_ARM_TYPE,m_iCrossArmType);
}


BEGIN_MESSAGE_MAP(CCrossArmDesignDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_CMB_CROSEE_ARM_TYPE, OnChangeCmbCrossArmType)
END_MESSAGE_MAP()


// CCrossArmDesignDlg ��Ϣ�������
BOOL CCrossArmDesignDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_CROSEE_ARM_TYPE);
	pCMB->SetCurSel(m_iCrossArmType);
	m_pModel->m_pTemplate->m_iTempIndex=m_iCrossArmType;
	m_pModel->InitTemplateParams();
	//
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	DisplayPropertyList();
	ChangeKeyNodeNum();
	//
	//GetWindowRect(&m_rectInitWndRgn);
	//CWnd *pWnd = GetDlgItem(IDC_S_MAP);
	//pWnd->GetWindowRect(&m_rectInitDrawRgn);	//��ͼ��Ч��������������
	return TRUE;
}
void CCrossArmDesignDlg::ChangeKeyNodeNum() 
{	//�˴����ؼ�����ʾ˳������ģ��ʾ��ͼһ�£��Ժ��������� wxc 14-11-19
	m_sKeyNodeOrder="�ؼ���ѡ��˳��:\n\n";
	CString ss;
	for(int i=0;i<m_nKeyNodeNum;i++)
	{
		if(i==0)
			ss="\tK1";
		else if(i==1)
			ss="     K2";
		else if(i==2)
			ss="     K3";
		else
			ss="     K4";
		m_sKeyNodeOrder+=ss;
	}
	GetDlgItem(IDC_S_KEY_NODE_ORDER)->SetWindowText(m_sKeyNodeOrder);
}
int CCrossArmDesignDlg::GetPropValueStr(int idProp,char* valueStr)
{
	char sText[100]="";
	if(idProp==1)	//�ؼ�������
		sprintf(sText,"%d",m_nKeyNodeNum);
	else if(idProp==2)	//�ؼ���Գ�
	{
		if(m_nKeyNodeNum==2)
			m_iKeyNodeMir=1;
		else //if(m_nKeyNodeNum==4)
			m_iKeyNodeMir=0;
		//
		if(m_iKeyNodeMir==0)
			strcpy(sText,"�޶Գ�");
		else if(m_iKeyNodeMir==1)
			strcpy(sText,"X��");
		else if(m_iKeyNodeMir==2)
			strcpy(sText,"Y��");
		else if(m_iKeyNodeMir==3)
			strcpy(sText,"XYZ��");
		else
			return 0;
	}
	else if(idProp==3)	//ģ����Գ�
	{
		if(m_iStdFaceMir==0)
			strcpy(sText,"�޶Գ�");
		else if(m_iStdFaceMir==1)
			strcpy(sText,"X��");
		else if(m_iStdFaceMir==2)
			strcpy(sText,"Y��");
		else if(m_iStdFaceMir==3)
			strcpy(sText,"Z��");
		else if(m_iStdFaceMir==4)
			strcpy(sText,"����");
		else if(m_iStdFaceMir==5)
			strcpy(sText,"Z��_����");
		else
			return 0;
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
void CCrossArmDesignDlg::DisplayPropertyList()
{
	m_propList.CleanTree();
	//�����������ص�����
	m_propList.SetModifyValueFunc(ModifyItemValue);
	char sText[100]="";
	CPropTreeItem *pItem,*pGroupItem,*pRoot=m_propList.GetRootItem();
	//ģ�����
	CItemInfo* lpInfo = new CItemInfo();
	lpInfo->m_strPropName = "ģ�����";
	lpInfo->m_strPropHelp = "ģ�����";
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp=0;
	pGroupItem->m_bHideChildren=FALSE;
	for(DESIGN_PARAM_ITEM* pParam=m_pModel->listParams.GetFirst();pParam;pParam=m_pModel->listParams.GetNext())
	{
		lpInfo=new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName=pParam->sLabel;
		lpInfo->m_strPropHelp=pParam->sNotes;
		pItem=m_propList.InsertItem(pGroupItem,lpInfo,-1);
		pItem->m_idProp=pParam->key;
		if(pParam->dataType==DESIGN_PARAM_ITEM::TYPE_DOUBLE)
		{
			pItem->m_lpNodeInfo->m_strPropValue.Format("%1f",pParam->value.fValue);
			SimplifiedNumString(pItem->m_lpNodeInfo->m_strPropValue);
		}
		else if(pParam->dataType==DESIGN_PARAM_ITEM::TYPE_INTEGER)
			pItem->m_lpNodeInfo->m_strPropValue.Format("%d",pParam->value.iType);
	}
	//װ����Ϣ
	lpInfo = new CItemInfo();
	lpInfo->m_strPropName = "װ����Ϣ";
	lpInfo->m_strPropHelp = "װ����Ϣ";
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp=0;
	pGroupItem->m_bHideChildren=FALSE;
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbItems="2|4";
	lpInfo->m_strPropName = "�ؼ�������";
	lpInfo->m_strPropHelp = "�ؼ�������";
	pItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pItem->m_idProp=1;
	if(GetPropValueStr(pItem->m_idProp,sText)>0)
		pItem->m_lpNodeInfo->m_strPropValue=sText;

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbItems= "�޶Գ�|X��|Y��|XYZ��";	//|Z��
	lpInfo->m_strPropName = "�ؼ���Գ�";
	lpInfo->m_strPropHelp = "�ؼ���Գ�";
	pItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pItem->m_idProp=2;
	if(GetPropValueStr(pItem->m_idProp,sText)>0)
		pItem->m_lpNodeInfo->m_strPropValue=sText;
	pItem->SetReadOnly(TRUE);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbItems= "�޶Գ�|X��|Y��|Z��|����|Z��_����";
	lpInfo->m_strPropName = "ģ����Գ�";
	lpInfo->m_strPropHelp = "ģ����Գ�";
	pItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pItem->m_idProp=3;
	if(GetPropValueStr(pItem->m_idProp,sText)>0)
		pItem->m_lpNodeInfo->m_strPropValue=sText;
	m_propList.Redraw();
}
void CCrossArmDesignDlg::UpdateFaceDetailGraph()	//��ͼ������ʾ��ͼ
{

}
void CCrossArmDesignDlg::OnChangeCmbCrossArmType()
{
	UpdateData();
	m_pModel->listParams.Empty();
	CComboBox* pCMB=(CComboBox*)GetDlgItem(IDC_CMB_CROSEE_ARM_TYPE);
	if(pCMB->GetCurSel()==0)		//�������κᵣ
		m_pModel->m_pTemplate->m_iTempIndex=0;	
	else if(pCMB->GetCurSel()==1)	//�������κᵣ
		m_pModel->m_pTemplate->m_iTempIndex=1;	
	m_pModel->InitTemplateParams();	//��ʼ���ᵣģ�����
	UpdateFaceDetailGraph();		//���ƺᵣʾ��ͼ
	DisplayPropertyList();
}

