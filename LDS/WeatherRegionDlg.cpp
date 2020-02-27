// WeatherRegionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "GlobalFunc.h"
#include "WeatherRegionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWeatherRegionDlg dialog

static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
							int iSubItem,CString& sTextValue)
{
	CWeatherRegionDlg *pWeatherDlg=(CWeatherRegionDlg*)pListCtrl->GetParent();
	pListCtrl->SetSubItemText(pSelItem,iSubItem,sTextValue);
	pWeatherDlg->m_bDataChange = TRUE;
	if(pSelItem->m_idProp==1)
	{	
		//�������
		pWeatherDlg->weatherRegion.m_nHighestT = atoi(sTextValue.GetBuffer(1));
	}
	else if(pSelItem->m_idProp==2)
	{	//�������
		pWeatherDlg->weatherRegion.m_nLowestT = atoi(sTextValue.GetBuffer(1));
	}
	else if(pSelItem->m_idProp==3)
	{	//ƽ������
		pWeatherDlg->weatherRegion.m_nAverageT = atoi(sTextValue.GetBuffer(1));
	}
	else if(pSelItem->m_idProp==4)
	{	//���
		if (iSubItem==1)
			pWeatherDlg->weatherRegion.m_nMaxWindT = atoi(sTextValue.GetBuffer(1));
		else
			pWeatherDlg->weatherRegion.m_fMaxWindV = atof(sTextValue.GetBuffer(1));
	}
	else if(pSelItem->m_idProp==5)
	{	//����
		if (iSubItem==1)
			pWeatherDlg->weatherRegion.m_nMaxIceT = atoi(sTextValue.GetBuffer(1));
		else if(iSubItem==2)
			pWeatherDlg->weatherRegion.m_fMaxWindV = atof(sTextValue.GetBuffer(1));
		else
			pWeatherDlg->weatherRegion.m_fMaxIceThick = atof(sTextValue.GetBuffer(1));
	}
	else if(pSelItem->m_idProp==6)
	{	//��װ
		if (iSubItem==1)
			pWeatherDlg->weatherRegion.m_nSetupT = atoi(sTextValue.GetBuffer(1));
		else if(iSubItem==2)
			pWeatherDlg->weatherRegion.m_fSetupWindV = atof(sTextValue.GetBuffer(1));
	}
	return TRUE;
}
CWeatherRegionDlg::CWeatherRegionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWeatherRegionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWeatherRegionDlg)
	m_sRegionCode = _T("���͢�");
	//}}AFX_DATA_INIT
	m_listCtrl.AddColumnHeader("������");
	m_listCtrl.AddColumnHeader("�¶�(��C)");
	m_listCtrl.AddColumnHeader("����(m/s)");
	m_listCtrl.AddColumnHeader("����(mm)");
}


void CWeatherRegionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWeatherRegionDlg)
	DDX_Control(pDX, IDC_LIST_WEATHER_PARAS, m_listCtrl);
	DDX_CBString(pDX, IDC_CMB_REGION_CODE, m_sRegionCode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWeatherRegionDlg, CDialog)
	//{{AFX_MSG_MAP(CWeatherRegionDlg)
	ON_CBN_SELCHANGE(IDC_CMB_REGION_CODE, OnSelchangeCmbRegionCode)
	ON_CBN_EDITCHANGE(IDC_CMB_REGION_CODE, OnEditchangeCmbRegionCode)
	ON_BN_CLICKED(IDC_BN_SAVE_WEATHER_REGION, OnBnSaveWeatherRegion)
	ON_BN_CLICKED(IDC_BN_DEL_WEATHER_REGION, OnBnDelWeatherRegion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWeatherRegionDlg message handlers

BOOL CWeatherRegionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_bDataChange = FALSE;
	char file_path[MAX_PATH];
	GetSysPath(file_path);
	strcat(file_path,"wireload.d");
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_REGION_CODE);
	if(db.OpenDbFile(file_path))
	{
		WEATHER_REGION wr;
		BOOL finded=db.QueryFirstWeatherRegion(&wr);
		while(finded)
		{
			pCMB->AddString(wr.weatherCode);
			finded=db.QueryNextWeatherRegion(&wr);
		}
	}
	pCMB->SetWindowText(weatherRegion.weatherCode);
	long col_wide_arr[6]={120,80,80,80};
	m_listCtrl.InitListCtrl(col_wide_arr);
	CSuperGridCtrl::CTreeItem* pItemArr[6]={NULL};
	//�������
	CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
	lpRoot->AddSubItemText("�������",TRUE);
	pItemArr[0] = m_listCtrl.InsertRootItem(lpRoot);
	//�������
	lpRoot = new CListCtrlItemInfo();
	lpRoot->AddSubItemText("�������",TRUE);
	pItemArr[1] = m_listCtrl.InsertRootItem(lpRoot);
	//ƽ������
	lpRoot = new CListCtrlItemInfo();
	lpRoot->AddSubItemText("ƽ������",TRUE);
	pItemArr[2] = m_listCtrl.InsertRootItem(lpRoot);
	//���
	lpRoot = new CListCtrlItemInfo();
	lpRoot->AddSubItemText("���",TRUE);
	pItemArr[3] = m_listCtrl.InsertRootItem(lpRoot);
	//����
	lpRoot = new CListCtrlItemInfo();
	lpRoot->AddSubItemText("����",TRUE);
	pItemArr[4] = m_listCtrl.InsertRootItem(lpRoot);
	//��װ
	lpRoot = new CListCtrlItemInfo();
	lpRoot->AddSubItemText("��װ",TRUE);
	pItemArr[5] = m_listCtrl.InsertRootItem(lpRoot);
	for(int i=0;i<6;i++)
		pItemArr[i]->m_idProp=i+1;
	m_listCtrl.SetModifyValueFunc(FireValueModify);
	if (strlen(weatherRegion.weatherCode) != 0 
		|| weatherRegion.m_fMaxIceWindV != 0
		|| weatherRegion.m_fMaxIceThick != 0
		|| weatherRegion.m_fMaxWindV != 0
		|| weatherRegion.m_nAverageT != 0
		|| weatherRegion.m_nHighestT != 0
		|| weatherRegion.m_nLowestT != 0
		|| weatherRegion.m_nMaxIceT != 0
		|| weatherRegion.m_nMaxWindT != 0
		|| weatherRegion.m_nSetupT != 0
		|| weatherRegion.m_fSetupWindV != 0)
	{
		CString itemText;
		//�������
		itemText.Format("%d",weatherRegion.m_nHighestT);
		m_listCtrl.SetSubItemText(pItemArr[0],1,itemText);
		pItemArr[0]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
		//�������
		itemText.Format("%d",weatherRegion.m_nLowestT);
		m_listCtrl.SetSubItemText(pItemArr[1],1,itemText);
		pItemArr[1]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
		//ƽ������
		itemText.Format("%d",weatherRegion.m_nAverageT);
		m_listCtrl.SetSubItemText(pItemArr[2],1,itemText);
		pItemArr[2]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
		//���
		itemText.Format("%d",weatherRegion.m_nMaxWindT);
		m_listCtrl.SetSubItemText(pItemArr[3],1,itemText);
		pItemArr[3]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
		itemText.Format("%f",weatherRegion.m_fMaxWindV);
		SimplifiedNumString(itemText);
		m_listCtrl.SetSubItemText(pItemArr[3],2,itemText);
		pItemArr[3]->m_lpNodeInfo->SetSubItemReadOnly(2,FALSE);
		//����
		itemText.Format("%d",weatherRegion.m_nMaxIceT);
		m_listCtrl.SetSubItemText(pItemArr[4],1,itemText);
		pItemArr[4]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
		itemText.Format("%f",weatherRegion.m_fMaxIceWindV);
		SimplifiedNumString(itemText);
		m_listCtrl.SetSubItemText(pItemArr[4],2,itemText);
		pItemArr[4]->m_lpNodeInfo->SetSubItemReadOnly(2,FALSE);
		itemText.Format("%f",weatherRegion.m_fMaxIceThick);
		SimplifiedNumString(itemText);
		m_listCtrl.SetSubItemText(pItemArr[4],3,itemText);
		pItemArr[4]->m_lpNodeInfo->SetSubItemReadOnly(3,FALSE);
		//��װ
		itemText.Format("%d",weatherRegion.m_nSetupT);
		m_listCtrl.SetSubItemText(pItemArr[5],1,itemText);
		pItemArr[5]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
		itemText.Format("%f",weatherRegion.m_fSetupWindV);
		SimplifiedNumString(itemText);
		m_listCtrl.SetSubItemText(pItemArr[5],2,itemText);
		pItemArr[5]->m_lpNodeInfo->SetSubItemReadOnly(2,FALSE);
	}
	else
	{
		CXhChar200 regionCode;
		regionCode.Printf("%s",m_sRegionCode);
		if(db.QueryWeatherRegion(regionCode,&weatherRegion))
		{
			CString itemText;
			//�������
			itemText.Format("%d",weatherRegion.m_nHighestT);
			m_listCtrl.SetSubItemText(pItemArr[0],1,itemText);
			pItemArr[0]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
			//�������
			itemText.Format("%d",weatherRegion.m_nLowestT);
			m_listCtrl.SetSubItemText(pItemArr[1],1,itemText);
			pItemArr[1]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
			//ƽ������
			itemText.Format("%d",weatherRegion.m_nAverageT);
			m_listCtrl.SetSubItemText(pItemArr[2],1,itemText);
			pItemArr[2]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
			//���
			itemText.Format("%d",weatherRegion.m_nMaxWindT);
			m_listCtrl.SetSubItemText(pItemArr[3],1,itemText);
			pItemArr[3]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
			itemText.Format("%f",weatherRegion.m_fMaxWindV);
			SimplifiedNumString(itemText);
			m_listCtrl.SetSubItemText(pItemArr[3],2,itemText);
			pItemArr[3]->m_lpNodeInfo->SetSubItemReadOnly(2,FALSE);
			//����
			itemText.Format("%d",weatherRegion.m_nMaxIceT);
			m_listCtrl.SetSubItemText(pItemArr[4],1,itemText);
			pItemArr[4]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
			itemText.Format("%f",weatherRegion.m_fMaxIceWindV);
			SimplifiedNumString(itemText);
			m_listCtrl.SetSubItemText(pItemArr[4],2,itemText);
			pItemArr[4]->m_lpNodeInfo->SetSubItemReadOnly(2,FALSE);
			itemText.Format("%f",weatherRegion.m_fMaxIceThick);
			SimplifiedNumString(itemText);
			m_listCtrl.SetSubItemText(pItemArr[4],3,itemText);
			pItemArr[4]->m_lpNodeInfo->SetSubItemReadOnly(3,FALSE);
			//��װ
			itemText.Format("%d",weatherRegion.m_nSetupT);
			m_listCtrl.SetSubItemText(pItemArr[5],1,itemText);
			pItemArr[5]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
			itemText.Format("%f",weatherRegion.m_fSetupWindV);
			SimplifiedNumString(itemText);
			m_listCtrl.SetSubItemText(pItemArr[5],2,itemText);
			pItemArr[5]->m_lpNodeInfo->SetSubItemReadOnly(2,FALSE);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWeatherRegionDlg::OnSelchangeCmbRegionCode() 
{
	if (m_bDataChange)
		m_bDataChange = FALSE;
	CXhChar200 regionCode;
	CComboBox* pCMB = (CComboBox*)GetDlgItem(IDC_CMB_REGION_CODE);
	int iCurSel = pCMB->GetCurSel();
	pCMB->GetLBText(iCurSel,m_sRegionCode);
	regionCode.Printf("%s",m_sRegionCode);
	UpdateWeatherInfoList(regionCode);
}

void CWeatherRegionDlg::OnEditchangeCmbRegionCode() 
{
	if (m_bDataChange == TRUE)
		return;
	UpdateData();
	CXhChar200 regionCode;
	regionCode.Printf("%s",m_sRegionCode);
	UpdateWeatherInfoList(regionCode);
}

BOOL CWeatherRegionDlg::UpdateWeatherInfoList(char* regionCode)
{
	if(db.QueryWeatherRegion(regionCode,&weatherRegion))
	{
		CString itemText;
		CSuperGridCtrl::CTreeItem* pItemArr[6];
		for(int i=0;i<6;i++)
			pItemArr[i]=m_listCtrl.FindTreeItemById(i+1);
		//�������
		itemText.Format("%d",weatherRegion.m_nHighestT);
		m_listCtrl.SetSubItemText(pItemArr[0],1,itemText);
		pItemArr[0]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
		//�������
		itemText.Format("%d",weatherRegion.m_nLowestT);
		m_listCtrl.SetSubItemText(pItemArr[1],1,itemText);
		pItemArr[1]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
		//ƽ������
		itemText.Format("%d",weatherRegion.m_nAverageT);
		m_listCtrl.SetSubItemText(pItemArr[2],1,itemText);
		pItemArr[2]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
		//���
		itemText.Format("%d",weatherRegion.m_nMaxWindT);
		m_listCtrl.SetSubItemText(pItemArr[3],1,itemText);
		pItemArr[3]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
		itemText.Format("%f",weatherRegion.m_fMaxWindV);
		SimplifiedNumString(itemText);
		m_listCtrl.SetSubItemText(pItemArr[3],2,itemText);
		pItemArr[3]->m_lpNodeInfo->SetSubItemReadOnly(2,FALSE);
		//����
		itemText.Format("%d",weatherRegion.m_nMaxIceT);
		m_listCtrl.SetSubItemText(pItemArr[4],1,itemText);
		pItemArr[4]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
		itemText.Format("%f",weatherRegion.m_fMaxIceWindV);
		SimplifiedNumString(itemText);
		m_listCtrl.SetSubItemText(pItemArr[4],2,itemText);
		pItemArr[4]->m_lpNodeInfo->SetSubItemReadOnly(2,FALSE);
		itemText.Format("%f",weatherRegion.m_fMaxIceThick);
		SimplifiedNumString(itemText);
		m_listCtrl.SetSubItemText(pItemArr[4],3,itemText);
		pItemArr[4]->m_lpNodeInfo->SetSubItemReadOnly(3,FALSE);
		//��װ
		itemText.Format("%d",weatherRegion.m_nSetupT);
		m_listCtrl.SetSubItemText(pItemArr[5],1,itemText);
		pItemArr[5]->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
		itemText.Format("%f",weatherRegion.m_fSetupWindV);
		SimplifiedNumString(itemText);
		m_listCtrl.SetSubItemText(pItemArr[5],2,itemText);
		pItemArr[5]->m_lpNodeInfo->SetSubItemReadOnly(2,FALSE);
		return TRUE;
	}
	else
		return FALSE;
}

void CWeatherRegionDlg::OnBnSaveWeatherRegion() 
{
	// TODO: Add your control notification handler code here
	char code[100] = "";
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_REGION_CODE);
	pCMB->GetWindowText(code,100);
	int i = pCMB->GetCount();
	for (int j=0;j<i;j++)
	{
		char str[100] = "";
		pCMB->GetLBText(j,str);
		if (strcmp(str,code)==0)
		{
			if (IDOK==MessageBox("�����Ѵ����Ƿ��滻","��ʾ",IDOK))
				db.SetWeatherRegion(code,weatherRegion);
			return;
		}		
	}
	db.SetWeatherRegion(code,weatherRegion);
	pCMB->AddString(code);
}

void CWeatherRegionDlg::OnBnDelWeatherRegion() 
{
	// TODO: Add your control notification handler code here
	char code[100] = "";
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_REGION_CODE);
	pCMB->GetWindowText(code,100);
	db.DelWeatherRegion(code);
	pCMB->DeleteString(pCMB->GetCurSel());
	if (pCMB->GetCount()>0)
	{
		char sCode[100] = "";
		pCMB->SetCurSel(0);
		pCMB->GetWindowText(sCode,100);
		UpdateWeatherInfoList(sCode);
	}	
}

void CWeatherRegionDlg::OnOK() 
{
	CString itemText;
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_REGION_CODE);
	pCMB->GetWindowText(itemText);
	_snprintf(weatherRegion.weatherCode,49,"%s",itemText);
	CSuperGridCtrl::CTreeItem* pItemArr[6];
	for(int i=0;i<6;i++)
		pItemArr[i]=m_listCtrl.FindTreeItemById(i+1);
	//�������
	itemText=m_listCtrl.GetItemText(0,1);
	weatherRegion.m_nHighestT=atoi(itemText);
	//�������
	itemText=m_listCtrl.GetItemText(1,1);
	weatherRegion.m_nLowestT=atoi(itemText);
	//ƽ������
	itemText=m_listCtrl.GetItemText(2,1);
	weatherRegion.m_nAverageT=atoi(itemText);
	//���
	itemText=m_listCtrl.GetItemText(3,1);
	weatherRegion.m_nMaxWindT=atoi(itemText);
	itemText=m_listCtrl.GetItemText(3,2);
	weatherRegion.m_fMaxWindV=atof(itemText);
	//����
	itemText=m_listCtrl.GetItemText(4,1);
	weatherRegion.m_nMaxIceT=atoi(itemText);
	itemText=m_listCtrl.GetItemText(4,2);
	weatherRegion.m_fMaxIceWindV=atof(itemText);
	itemText=m_listCtrl.GetItemText(4,3);
	weatherRegion.m_fMaxIceThick=atof(itemText);
	//��װ
	itemText=m_listCtrl.GetItemText(5,1);
	weatherRegion.m_nSetupT=atoi(itemText);
	itemText=m_listCtrl.GetItemText(5,2);
	weatherRegion.m_fSetupWindV=atof(itemText);

	CDialog::OnOK();
}
