// AngleJointsDesignForm.cpp : ʵ���ļ�
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "AngleJointsDesignForm.h"
#include "afxdialogex.h"
#include "Tower.h"
#include "GlobalFunc.h"
#include "FLSizeOptimizor.h"
#include "SortFunc.h"

// CAngleJointsDesignForm �Ի���
#ifdef __PART_DESIGN_INC_
IMPLEMENT_DYNAMIC(CAngleJointsDesignForm, CDialogEx)

CAngleJointsDesignForm::CAngleJointsDesignForm(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAngleJointsDesignForm::IDD, pParent)
{

}

CAngleJointsDesignForm::~CAngleJointsDesignForm()
{
	DestroyWindow();
}
void CAngleJointsDesignForm::DisplayWindow()
{
	if(GetSafeHwnd()==NULL)
		Create(IDD);
	ShowWindow(SW_SHOW);
	RefreshListJoints();
}
BOOL CAngleJointsDesignForm::HideWindow()
{
	if(GetSafeHwnd()==NULL)
		return FALSE;
	ShowWindow(SW_HIDE);
	return TRUE;
}

void CAngleJointsDesignForm::Destroy()
{
	DestroyWindow();
	delete this;
}

void CAngleJointsDesignForm::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_JOINTS_DESIGN_RSLT, m_listCtrlJoints);
}


BEGIN_MESSAGE_MAP(CAngleJointsDesignForm, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BN_REDESIGN_ALL, &CAngleJointsDesignForm::OnBnClickedBnRedesignAll)
	ON_BN_CLICKED(IDC_BN_DESIGN_SELECT_JOINTS, &CAngleJointsDesignForm::OnBnClickedBnDesignSelectJoints)
	ON_BN_CLICKED(IDC_BN_DESIGN_ALL_JOINTS, &CAngleJointsDesignForm::OnBnClickedBnDesignAllJoints)
END_MESSAGE_MAP()


// CAngleJointsDesignForm ��Ϣ�������

static int FireCompareItem(const CSuperGridCtrl::CSuperGridCtrlItemPtr& pItem1,const CSuperGridCtrl::CSuperGridCtrlItemPtr& pItem2,DWORD lPara)
{
	COMPARE_FUNC_EXPARA* pExPara=(COMPARE_FUNC_EXPARA*)lPara;
	int iSubItem=0;
	BOOL bAscending=true;
	if(pExPara)
	{
		iSubItem=pExPara->iSubItem;
		bAscending=pExPara->bAscending;
	}
	CString sText1=pItem1->m_lpNodeInfo->GetSubItemText(iSubItem);
	CString sText2=pItem2->m_lpNodeInfo->GetSubItemText(iSubItem);
	int result=0;
	CXhChar50 label1,label2;
	label1.Copy(sText1.GetString());
	label2.Copy(sText2.GetString());
	char material_order[26]={0};
	material_order['S'-'A']=0;
	material_order['H'-'A']=1;
	material_order['G'-'A']=2;
	material_order['P'-'A']=3;
	material_order['T'-'A']=4;
	if(iSubItem==0)	//��ͷ���
	{
		long i,arrPointI[6]={0};
		char* key=NULL;
		for(i=0;i<6;i++)
		{
			if(i==0)
				key=strtok(label1,":-");
			else if(i==3)
				key=strtok(label2,":-");
			else
				key=strtok(NULL,":-");
			arrPointI[i]=atoi(key);
		}
		if(arrPointI[0]!=arrPointI[3])
			result=arrPointI[0] - arrPointI[3];
		else if(arrPointI[1]!=arrPointI[4])
			result=arrPointI[1] - arrPointI[4];
		else if(arrPointI[2]!=arrPointI[5])
			result=arrPointI[2] - arrPointI[5];
	}
	else if(iSubItem==1||iSubItem==2||iSubItem==4||iSubItem==6)
	{
		double wide[2]={0},thick[2]={0};
		char material[2]={'S','S'};
		label1.Replace('L',' ');
		label2.Replace('L',' ');
		char* key=NULL;
		for(int i=0;i<2;i++)
		{
			if(i==0)
				key=strtok(label1,"X*");
			else
				key=strtok(label2,"X*");
			if(key==NULL)
				wide[i]=0;
			else
				wide[i]=atof(key);
			key=strtok(NULL,"X*");
			if(key==NULL)
				thick[i]=material[i]=0;
			else
			{
				if(isdigit(key[strlen(key)-1])==0)	//��ĸ
				{
					material[i]=key[strlen(key)-1];
					key[strlen(key)-1]=0;
				}
				thick[i]=atof(key);
			}
		}
		if(wide[0]!=wide[1])
			result=(int)(wide[0]-wide[1]);
		else if(thick[0]!=thick[1])
			result=(int)(thick[0]-thick[1]);
		else if(material[0]!=material[1])
			result=(int)(material_order[min(material[0]-'A',25)]-material_order[min(material[1]-'A',25)]);
	}
	else
		result=sText1.Compare(sText2);
	if(!bAscending)
		result*=-1;
	return result;
}
static int FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem,CString& sTextValue)
{
	ANGLE_JOINT *pAngleJoint=Ta.hashAngleJoint.GetValue(pSelItem->m_idProp);
	CString sTextValueTemp="";
	CString strAngleSpecs=MakeAngleSetString();
	if(iSubItem==3)
	{
		pListCtrl->SetSubItemText(pSelItem,3,sTextValue);
#ifdef AFX_TARG_ENU_ENGLISH
		if(sTextValue.CompareNoCase("�ڰ��������")==0)
#else 
		if(sTextValue.CompareNoCase("�ڰ��������")==0)
#endif
		{
			pAngleJoint->cJointType=0;
			pSelItem->m_lpNodeInfo->SetSubItemReadOnly(4,FALSE);
			pSelItem->m_lpNodeInfo->SetSubItemReadOnly(5,FALSE);
			pSelItem->m_lpNodeInfo->SetSubItemReadOnly(6,TRUE);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(sTextValue.CompareNoCase("�����")==0)
#else 
		else if(sTextValue.CompareNoCase("�����")==0)
#endif
		{
			pAngleJoint->cJointType=1;
			pSelItem->m_lpNodeInfo->SetSubItemReadOnly(4,TRUE);
			pSelItem->m_lpNodeInfo->SetSubItemReadOnly(5,TRUE);
			pSelItem->m_lpNodeInfo->SetSubItemReadOnly(6,FALSE);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		if(sTextValue.CompareNoCase("�������")==0)
#else 
		if(sTextValue.CompareNoCase("�������")==0)
#endif
		{
			pAngleJoint->cJointType=2;
			pSelItem->m_lpNodeInfo->SetSubItemReadOnly(4,FALSE);
			pSelItem->m_lpNodeInfo->SetSubItemReadOnly(5,TRUE);
			pSelItem->m_lpNodeInfo->SetSubItemReadOnly(6,FALSE);
		}

		JG_JOINT_PARA* pJointPara=FindJgJointPara(pAngleJoint->smallAngle.width,pAngleJoint->smallAngle.thick);
		if (pJointPara==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox(NULL,"There is no matching data that please input again��","Warning Message",MB_OK);
#else 
			MessageBox(NULL,"û��ƥ���������Ϣ�����������룡","��ʾ",MB_OK);
#endif
			return 0;
		}
		pAngleJoint->innerAngle.width=pJointPara->fInnerJgWidth;
		pAngleJoint->innerAngle.thick=pJointPara->fInnerJgThick;
		if(pAngleJoint->cJointType==1||pAngleJoint->cJointType==2)
		{
			pAngleJoint->outer.angle.width = pJointPara->fOuterJgWidth;
			pAngleJoint->outer.angle.thick = pJointPara->fOuterJgThick;
		}
		else if(pAngleJoint->cJointType==0)
		{
			pAngleJoint->outer.plate.width = pJointPara->fOuterPlateWidth;
			pAngleJoint->outer.plate.thick = pJointPara->fOuterPlateThick;
		}
		//�ڰ��Ǹֹ��
		if(pAngleJoint->cJointType!=0&&pAngleJoint->cJointType!=2)
			pListCtrl->SetSubItemText(pSelItem,4,"");	//���ڰ���
		else
		{
			sTextValueTemp="";
			sTextValueTemp=pAngleJoint->GetInnerPartSpec();
			pSelItem->m_lpNodeInfo->SetListItemsStr(4,strAngleSpecs);
			pListCtrl->SetSubItemText(pSelItem,4,sTextValueTemp);
		}
		//����ְ���
		if(pAngleJoint->cJointType!=0)
			pListCtrl->SetSubItemText(pSelItem,5,"");	//������ְ�
		else
		{
			sTextValueTemp="";
			sTextValueTemp=pAngleJoint->GetOutterPartSpec();
			pSelItem->m_lpNodeInfo->SetListItemsStr(5,strAngleSpecs);
			pListCtrl->SetSubItemText(pSelItem,5,sTextValueTemp);
		}
		//����Ǹֹ��
		if(pAngleJoint->cJointType!=1&&pAngleJoint->cJointType!=2)
			pListCtrl->SetSubItemText(pSelItem,6,"");	//�������
		else
		{
			sTextValueTemp="";
			sTextValueTemp=pAngleJoint->GetOutterPartSpec();
			pSelItem->m_lpNodeInfo->SetListItemsStr(6,strAngleSpecs);
			pListCtrl->SetSubItemText(pSelItem,6,sTextValueTemp);	//�������
		}
	}
	//�ڰ��Ǹֹ��
	else if(iSubItem==4)
	{
		pListCtrl->SetSubItemText(pSelItem,4,sTextValue);
		char *pTemp = (LPSTR)(LPCTSTR)sTextValue;
		restore_JG_guige((char*)pTemp,pAngleJoint->innerAngle.width,pAngleJoint->innerAngle.thick);
	}
	//����ְ���
	else if(iSubItem==5)
	{
		pListCtrl->SetSubItemText(pSelItem,5,sTextValue);
		AfxExtractSubString(sTextValueTemp,sTextValue,0,'*');
		sTextValueTemp.Replace("-","");
		pAngleJoint->outer.plate.thick=atof(sTextValueTemp);
		AfxExtractSubString(sTextValueTemp,sTextValue,1,'*');
		sTextValueTemp.Delete(sTextValueTemp.GetLength()-1,1);
		pAngleJoint->outer.plate.width=atof(sTextValueTemp);
	}
	//����Ǹֹ��
	else if(iSubItem==6)
	{
		pListCtrl->SetSubItemText(pSelItem,6,sTextValue);
		char *pTemp = (LPSTR)(LPCTSTR)sTextValue;
		restore_JG_guige((char*)pTemp,pAngleJoint->outer.angle.width,pAngleJoint->outer.angle.thick);
	}
	pAngleJoint->DesignCheckAreaAndBoltForceScale();
	//������֫������˨��Ϣ
	sTextValueTemp="";
	sTextValueTemp=pAngleJoint->GetBoltInfo();
	pListCtrl->SetSubItemText(pSelItem,7,sTextValueTemp);
	//������
	sTextValueTemp="";
	sTextValueTemp.Format("%.2f",pAngleJoint->bolt.holeout_coef);
	pListCtrl->SetSubItemText(pSelItem,8,sTextValueTemp);
	//��ͷ/���������
	sTextValueTemp="";
	sTextValueTemp.Format("%.2f",pAngleJoint->pTagInfo->scaleAreaJ2A2);
	pListCtrl->SetSubItemText(pSelItem,9,sTextValueTemp);
	//��˨/����������
	sTextValueTemp="";
	sTextValueTemp.Format("%.2f",pAngleJoint->pTagInfo->scaleForceB2A2);
	pListCtrl->SetSubItemText(pSelItem,10,sTextValueTemp);
	pListCtrl->RedrawItems(3,10);
	return 0;
}
BOOL CAngleJointsDesignForm::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	RelayoutWnd();
#ifdef AFX_TARG_ENU_ENGLISH
	m_listCtrlJoints.AddColumnHeader("Joint Label Code",140);
	m_listCtrlJoints.AddColumnHeader("Small Angle Specification",80);
	m_listCtrlJoints.AddColumnHeader("Large Angle specification",80);
	m_listCtrlJoints.AddColumnHeader("Joint Part Style",100);
	m_listCtrlJoints.AddColumnHeader("�ڰ������",80);
	m_listCtrlJoints.AddColumnHeader("�������",80);
	m_listCtrlJoints.AddColumnHeader("��������",80);
	m_listCtrlJoints.AddColumnHeader("Bolt Information",100);
	m_listCtrlJoints.AddColumnHeader("������",60);
	m_listCtrlJoints.AddColumnHeader("Area Comparative",60);
	m_listCtrlJoints.AddColumnHeader("Internal Force Comparative",60);
#else 
	m_listCtrlJoints.AddColumnHeader("��ͷ���",140);
	m_listCtrlJoints.AddColumnHeader("С�Ǹֹ��",80);
	m_listCtrlJoints.AddColumnHeader("��Ǹֹ��",80);
	m_listCtrlJoints.AddColumnHeader("��ͷ����",100);
	m_listCtrlJoints.AddColumnHeader("�ڰ������",80);
	m_listCtrlJoints.AddColumnHeader("�������",80);
	m_listCtrlJoints.AddColumnHeader("��������",80);
	m_listCtrlJoints.AddColumnHeader("��˨��Ϣ",100);
	m_listCtrlJoints.AddColumnHeader("������",60);
	m_listCtrlJoints.AddColumnHeader("�����",60);
	m_listCtrlJoints.AddColumnHeader("������",60);
#endif
	m_listCtrlJoints.EnableSortItems();
	m_listCtrlJoints.SetCompareItemFunc(FireCompareItem);
	//m_listCtrlJoints.SetContextMenuFunc(FireContextMenu);
	m_listCtrlJoints.SetModifyValueFunc(FireValueModify);
	m_listCtrlJoints.InitListCtrl(NULL);
	RefreshListJoints();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
void CAngleJointsDesignForm::RefreshSpecJointItem(long iItem,ANGLE_JOINT* pAngleJoint)
{
	CXhChar50 itemText;
	CSuperGridCtrl::CTreeItem* pAttachItem=m_listCtrlJoints.GetTreeItem(iItem);
	//��ͷ���
	m_listCtrlJoints.SetSubItemText(pAttachItem,0,pAngleJoint->GetJointNo());
	int column=1;
	//����(С)���
	itemText.Printf("L%s*%s%C",
		(char*)CXhChar16(pAngleJoint->smallAngle.width),
		(char*)CXhChar16(pAngleJoint->smallAngle.thick),pAngleJoint->smallAngle.cMaterial);
	m_listCtrlJoints.SetSubItemText(pAttachItem,column++,itemText);
	//����(��)���
	itemText.Printf("L%s*%s%C",
		(char*)CXhChar16(pAngleJoint->bigAngle.width),
		(char*)CXhChar16(pAngleJoint->bigAngle.thick),pAngleJoint->bigAngle.cMaterial);
	m_listCtrlJoints.SetSubItemText(pAttachItem,column++,itemText);
	//��ͷ����
#ifdef AFX_TARG_ENU_ENGLISH
	if(pAngleJoint->cJointType==0)
		itemText.Copy("�ڰ��������");
	else if(pAngleJoint->cJointType==1)
		itemText.Copy("�����");
	else //if(pAngleJoint->cJointType==2)
		itemText.Copy("�������");
#else 
	if(pAngleJoint->cJointType==0)
		itemText.Copy("�ڰ��������");
	else if(pAngleJoint->cJointType==1)
		itemText.Copy("�����");
	else //if(pAngleJoint->cJointType==2)
		itemText.Copy("�������");
#endif
	m_listCtrlJoints.SetSubItemText(pAttachItem,column++,itemText);
	//�ڰ��Ǹֹ��
	if(pAngleJoint->cJointType!=0&&pAngleJoint->cJointType!=2)
		itemText.Copy("");	//���ڰ���
	else
		itemText =pAngleJoint->GetInnerPartSpec();
	m_listCtrlJoints.SetSubItemText(pAttachItem,column++,itemText);
	//����ְ���
	if(pAngleJoint->cJointType!=0)
		itemText.Copy("");	//������ְ�
	else
		itemText =pAngleJoint->GetOutterPartSpec();
	m_listCtrlJoints.SetSubItemText(pAttachItem,column++,itemText);
	//����Ǹֹ��
	if(pAngleJoint->cJointType!=1&&pAngleJoint->cJointType!=2)
		itemText.Copy("");	//�������
	else
		itemText =pAngleJoint->GetOutterPartSpec();
	m_listCtrlJoints.SetSubItemText(pAttachItem,column++,itemText);
	//������֫������˨��Ϣ
	itemText=pAngleJoint->GetBoltInfo();
	m_listCtrlJoints.SetSubItemText(pAttachItem,column++,itemText);
	//������
	itemText.Printf("%.2f",pAngleJoint->bolt.holeout_coef);
	m_listCtrlJoints.SetSubItemText(pAttachItem,column++,itemText);
	//��ͷ/���������
	itemText.Printf("%.2f",pAngleJoint->pTagInfo->scaleAreaJ2A2);
	m_listCtrlJoints.SetSubItemText(pAttachItem,column++,itemText);
	//��˨/����������
	itemText.Printf("%.2f",pAngleJoint->pTagInfo->scaleForceB2A2);
	m_listCtrlJoints.SetSubItemText(pAttachItem,column++,itemText);
	m_listCtrlJoints.RedrawItems(iItem,iItem);
}
void CAngleJointsDesignForm::RefreshListJoints()
{
	if(m_listCtrlJoints.GetItemCount()>0)
		m_listCtrlJoints.DeleteAllItems();
	CSuperGridCtrl::CTreeItem* pItem=NULL;
	CXhChar50 itemText;
	for(ANGLE_JOINT* pAngleJoint=hashAngleJoints.GetFirst();pAngleJoint;pAngleJoint=hashAngleJoints.GetNext())
	{
		if(!pAngleJoint->VerifyLinkAngles())
			continue;
		pAngleJoint->DesignCheckAreaAndBoltForceScale();
		CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
		lpRoot->SetSubItemColor(0,RGB(192,192,192));
		lpRoot->SetSubItemColor(1,RGB(192,192,192));
		lpRoot->SetSubItemColor(2,RGB(192,192,192));
		itemText.Copy("");
		//��ͷ���
		itemText=pAngleJoint->GetJointNo();
		lpRoot->AddSubItemText(CString(itemText),TRUE);
		pItem = m_listCtrlJoints.InsertRootItem(lpRoot);
		pItem->m_idProp=pAngleJoint->Id();
		pItem->m_lpNodeInfo->SetSubItemReadOnly(0,TRUE);
		int column=1;
		//����(С)���
		itemText.Printf("L%s*%s%C",
			(char*)CXhChar16(pAngleJoint->smallAngle.width),
			(char*)CXhChar16(pAngleJoint->smallAngle.thick),pAngleJoint->smallAngle.cMaterial);
		CString strAngleSpecs=MakeAngleSetString();
		pItem->m_lpNodeInfo->SetListItemsStr(column,strAngleSpecs);
		pItem->m_lpNodeInfo->SetSubItemReadOnly(column,TRUE);
		m_listCtrlJoints.SetSubItemText(pItem,column,itemText);
		lpRoot->SetControlType(column++,GCT_CMB_LIST);
		//����(��)���
		itemText.Printf("L%s*%s%C",
			(char*)CXhChar16(pAngleJoint->bigAngle.width),
			(char*)CXhChar16(pAngleJoint->bigAngle.thick),pAngleJoint->bigAngle.cMaterial);
		pItem->m_lpNodeInfo->SetListItemsStr(column,strAngleSpecs);
		pItem->m_lpNodeInfo->SetSubItemReadOnly(column,TRUE);
		m_listCtrlJoints.SetSubItemText(pItem,column,itemText);
		lpRoot->SetControlType(column++,GCT_CMB_LIST);
		//��ͷ����
#ifdef AFX_TARG_ENU_ENGLISH
		lpRoot->SetListItemsStr(column,"�ڰ��������|�����|�������");
		if(pAngleJoint->cJointType==0)
			itemText.Copy("�ڰ��������");
		else if(pAngleJoint->cJointType==1)
			itemText.Copy("�����");
		else //if(pAngleJoint->cJointType==2)
			itemText.Copy("�������");
#else 
		lpRoot->SetListItemsStr(column,"�ڰ��������|�����|�������");
		if(pAngleJoint->cJointType==0)
			itemText.Copy("�ڰ��������");
		else if(pAngleJoint->cJointType==1)
			itemText.Copy("�����");
		else //if(pAngleJoint->cJointType==2)
			itemText.Copy("�������");
#endif
		m_listCtrlJoints.SetSubItemText(pItem,column,itemText);
		lpRoot->SetControlType(column++,GCT_CMB_LIST);
		//�ڰ��Ǹֹ��
		if(pAngleJoint->cJointType!=0&&pAngleJoint->cJointType!=2)
		{
			itemText.Copy("");	//���ڰ���
			pItem->m_lpNodeInfo->SetSubItemReadOnly(column,TRUE);
		}
		else
		{
			itemText=pAngleJoint->GetInnerPartSpec();
			pItem->m_lpNodeInfo->SetSubItemReadOnly(column,FALSE);
			pItem->m_lpNodeInfo->SetListItemsStr(column,strAngleSpecs);
		}
		m_listCtrlJoints.SetSubItemText(pItem,column,itemText);
			pItem->m_lpNodeInfo->SetControlType(column++,GCT_CMB_LIST);
		//����ְ���
		if(pAngleJoint->cJointType!=0)
		{
			itemText.Copy("");	//������ְ�
			pItem->m_lpNodeInfo->SetSubItemReadOnly(column,TRUE);
		}
		else
		{
			itemText=pAngleJoint->GetOutterPartSpec();
			pItem->m_lpNodeInfo->SetSubItemReadOnly(column,FALSE);
		}
		m_listCtrlJoints.SetSubItemText(pItem,column++,itemText);
		//����Ǹֹ��
		if(pAngleJoint->cJointType!=1&&pAngleJoint->cJointType!=2)
		{
			itemText.Copy("");	//�������
			pItem->m_lpNodeInfo->SetSubItemReadOnly(column,TRUE);
		}
		else
		{
			itemText=pAngleJoint->GetOutterPartSpec();
			pItem->m_lpNodeInfo->SetSubItemReadOnly(column,FALSE);
			pItem->m_lpNodeInfo->SetListItemsStr(column,strAngleSpecs);
		}
		m_listCtrlJoints.SetSubItemText(pItem,column,itemText);
			pItem->m_lpNodeInfo->SetControlType(column++,GCT_CMB_LIST);
		//������֫������˨��Ϣ
		itemText=pAngleJoint->GetBoltInfo();
		pItem->m_lpNodeInfo->SetSubItemReadOnly(column,TRUE);
		m_listCtrlJoints.SetSubItemText(pItem,column++,itemText);
		//������
		itemText.Printf("%.2f",pAngleJoint->bolt.holeout_coef);
		pItem->m_lpNodeInfo->SetSubItemReadOnly(column,TRUE);
		m_listCtrlJoints.SetSubItemText(pItem,column++,itemText);
		//��ͷ/���������
		itemText.Printf("%.2f",pAngleJoint->pTagInfo->scaleAreaJ2A2);
		pItem->m_lpNodeInfo->SetSubItemReadOnly(column,TRUE);
		m_listCtrlJoints.SetSubItemText(pItem,column++,itemText);
		//��˨/����������
		itemText.Printf("%.2f",pAngleJoint->pTagInfo->scaleForceB2A2);
		pItem->m_lpNodeInfo->SetSubItemReadOnly(column,TRUE);
		m_listCtrlJoints.SetSubItemText(pItem,column++,itemText);
	}
	m_listCtrlJoints.Redraw();
}

BOOL CAngleJointsDesignForm::RelayoutWnd()
{
	RECT rc,rcCtrl;
	GetClientRect(&rc);
	rcCtrl=rc;
	if(m_listCtrlJoints.GetSafeHwnd()!=NULL)
	{
		rcCtrl.top=rcCtrl.left=8;
		rcCtrl.right=rc.right-8;
		rcCtrl.bottom=rc.bottom-40;
		m_listCtrlJoints.MoveWindow(&rcCtrl);
	}
	CWnd* pBtnOkCancel=GetDlgItem(IDCANCEL);
	if(pBtnOkCancel!=NULL&&pBtnOkCancel->GetSafeHwnd()!=NULL)
	{
		pBtnOkCancel->GetClientRect(&rcCtrl);
		int height=rcCtrl.bottom-rcCtrl.top;
		int width=rcCtrl.right;
		rcCtrl.bottom=rc.bottom-10;
		rcCtrl.top=rcCtrl.bottom-height;
		rcCtrl.right=rc.right-8;
		rcCtrl.left=rcCtrl.right-width;
		pBtnOkCancel->MoveWindow(&rcCtrl);
		InvalidateRect(&rcCtrl,FALSE);
	}
	pBtnOkCancel=GetDlgItem(IDOK);
	if(pBtnOkCancel!=NULL&&pBtnOkCancel->GetSafeHwnd()!=NULL)
	{
		pBtnOkCancel->GetClientRect(&rcCtrl);
		int height=rcCtrl.bottom-rcCtrl.top;
		int width=rcCtrl.right;
		rcCtrl.bottom=rc.bottom-10;
		rcCtrl.top=rcCtrl.bottom-height;
		rcCtrl.right=rc.right-8-(width+10);
		rcCtrl.left=rcCtrl.right-width;
		pBtnOkCancel->MoveWindow(&rcCtrl);
		InvalidateRect(&rcCtrl,FALSE);
	}
	CWnd* pBtnReDesign=GetDlgItem(IDC_BN_DESIGN_SELECT_JOINTS);
	if(pBtnReDesign!=NULL&&pBtnReDesign->GetSafeHwnd()!=NULL)
	{
		pBtnReDesign->GetClientRect(&rcCtrl);
		int height=rcCtrl.bottom-rcCtrl.top;
		int width=rcCtrl.right;
		rcCtrl.bottom=rc.bottom-10;
		rcCtrl.top=rcCtrl.bottom-height;
		rcCtrl.right=rc.right-8-(width+10)*2;
		rcCtrl.left=rcCtrl.right-width;
		pBtnReDesign->MoveWindow(&rcCtrl);
		InvalidateRect(&rcCtrl,FALSE);
	}
	pBtnReDesign=GetDlgItem(IDC_BN_DESIGN_ALL_JOINTS);
	if(pBtnReDesign!=NULL&&pBtnReDesign->GetSafeHwnd()!=NULL)
	{
		pBtnReDesign->GetClientRect(&rcCtrl);
		int height=rcCtrl.bottom-rcCtrl.top;
		int width=rcCtrl.right;
		rcCtrl.bottom=rc.bottom-10;
		rcCtrl.top=rcCtrl.bottom-height;
		rcCtrl.right=rc.right-8-(width+10)*3;
		rcCtrl.left=rcCtrl.right-width;
		pBtnReDesign->MoveWindow(&rcCtrl);
		InvalidateRect(&rcCtrl,FALSE);
	}
	pBtnReDesign=GetDlgItem(IDC_BN_REDESIGN_ALL);
	if(pBtnReDesign!=NULL&&pBtnReDesign->GetSafeHwnd()!=NULL)
	{
		pBtnReDesign->GetClientRect(&rcCtrl);
		int height=rcCtrl.bottom-rcCtrl.top;
		int width=rcCtrl.right;
		rcCtrl.bottom=rc.bottom-10;
		rcCtrl.top=rcCtrl.bottom-height;
		rcCtrl.right=rc.right-8-(width+10)*4;
		rcCtrl.left=rcCtrl.right-width;
		pBtnReDesign->MoveWindow(&rcCtrl);
		InvalidateRect(&rcCtrl,FALSE);
	}
	return TRUE;
}

void CAngleJointsDesignForm::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	RelayoutWnd();
}


void CAngleJointsDesignForm::OnBnClickedBnRedesignAll()
{
	ATOM_LIST<CJointCouple> listCouple;
	CJointCouple::StatJointLinkPart(&listCouple,CLS_LINEANGLE);//���ɽǸֽ�ͷƥ���
	CAngleJointDesigner::GenAngleJointSet(&listCouple,&Ta.hashAngleJoint);
	hashAngleJoints.Empty();
	for(ANGLE_JOINT* pJoint=Ta.hashAngleJoint.GetFirst();pJoint;pJoint=Ta.hashAngleJoint.GetNext())
	{
		ANGLE_JOINT* pNewJoint=hashAngleJoints.Add(pJoint->Id());
		*pNewJoint=*pJoint;
		pNewJoint->pTagInfo=NULL;
	}
	RefreshListJoints();
}

void CAngleJointsDesignForm::OnBnClickedBnDesignSelectJoints()
{
	POSITION pos=m_listCtrlJoints.GetFirstSelectedItemPosition();
	while(pos!=NULL){
		int iItem=m_listCtrlJoints.GetNextSelectedItem(pos);
		ANGLE_JOINT* pJoint=NULL;
		CSuperGridCtrl::CTreeItem* pItem=m_listCtrlJoints.GetTreeItem(iItem);
		if(pItem!=NULL)
			pJoint=hashAngleJoints.GetValue(pItem->m_idProp);
		if(pJoint!=NULL&&iItem>=0)
		{
			CAngleJointDesigner::SelectAngleJoint(pJoint,pJoint->cJointType);
			RefreshSpecJointItem(iItem,pJoint);
		}
	};
}

void CAngleJointsDesignForm::OnBnClickedBnDesignAllJoints()
{
	for(ANGLE_JOINT* pJoint=hashAngleJoints.GetFirst();pJoint;pJoint=hashAngleJoints.GetNext())
		CAngleJointDesigner::SelectAngleJoint(pJoint,pJoint->cJointType);
	RefreshListJoints();
}
#endif