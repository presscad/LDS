// AddSetupAidedPlateDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "lds.h"
#include "env_def.h"
#include "AddSetupAidedPlateDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef __ANGLE_PART_INC_

/////////////////////////////////////////////////////////////////////////////
// CAddSetupAidedPlateDlg dialog


CAddSetupAidedPlateDlg::CAddSetupAidedPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddSetupAidedPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddSetupAidedPlateDlg)
	m_fAngle = 0.0;
	m_sEndPlateHandle = _T("0X0");
	m_fStartPlateHalfT = 0.0;
	m_fEndPlateHalfT = 0.0;
	m_sStartPlateHandle = _T("0X0");
	m_sPlatePartNo = _T("");
	m_iProfileStyle = 0;
	m_fWidth = 100.0;
	m_fRadius = 0.0;
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("Label Code");
	m_arrHeaderLabel.Add("Radius");
	m_arrHeaderLabel.Add("Width");
	m_arrHeaderLabel.Add("Angle");
	m_arrHeaderLabel.Add("T1");
	m_arrHeaderLabel.Add("T2");
	m_arrHeaderLabel.Add("Style");
#else 
	m_arrHeaderLabel.Add("���");
	m_arrHeaderLabel.Add("�뾶");
	m_arrHeaderLabel.Add("���");
	m_arrHeaderLabel.Add("�н�");
	m_arrHeaderLabel.Add("T1");
	m_arrHeaderLabel.Add("T2");
	m_arrHeaderLabel.Add("��ʽ");
#endif
	listViewComparer.SetMaxColumnCount(7);
	m_bPauseBreakExit = FALSE;
	m_nPlateNum=0;
	m_tmpHandle=0;
	DisplayTmpParts.SetDocModifiedFlagAddress(&Ta.m_bDataModified);
}


void CAddSetupAidedPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddSetupAidedPlateDlg)
	DDX_Text(pDX, IDC_E_ANGLE, m_fAngle);
	DDX_Text(pDX, IDC_E_END_PLATE, m_sEndPlateHandle);
	DDX_Text(pDX, IDC_E_START_PLATE_HALF_T, m_fStartPlateHalfT);
	DDX_Text(pDX, IDC_E_END_PLATE_HALF_T, m_fEndPlateHalfT);
	DDX_Text(pDX, IDC_E_START_PLATE, m_sStartPlateHandle);
	DDX_Text(pDX, IDC_E_SETUP_AIDED_PLATE_PARTNO, m_sPlatePartNo);
	DDX_CBIndex(pDX, IDC_CMB_PROFILE_STYLE, m_iProfileStyle);
	DDX_Text(pDX, IDC_E_WIDTH, m_fWidth);
	DDX_Text(pDX, IDC_E_RADIUS, m_fRadius);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddSetupAidedPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CAddSetupAidedPlateDlg)
	ON_BN_CLICKED(IDC_BTN_ADD_SETUP_AIDED_PLATE, OnAddSetupAidedPlate)
	ON_BN_CLICKED(IDC_BTN_AUTO_STAT, OnAutoStat)
	ON_BN_CLICKED(IDC_BTN_EXPORT_LIST, OnExportList)
	ON_BN_CLICKED(IDC_BTN_IMPORT_LIST, OnImportList)
	ON_BN_CLICKED(IDC_BTN_SELECT_PART, OnSelectPart)
	ON_BN_CLICKED(IDC_BTN_MODIFY_SETUP_AIDED_PLATE, OnModifySetupAidedPlate)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SETUP_AIDED_PLATE, OnDblclkListSetupAidedPlate)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_SETUP_AIDED_PLATE, OnKeydownListSetupAidedPlate)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_SETUP_AIDED_PLATE, OnColumnclickListSetupAidedPlate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddSetupAidedPlateDlg message handlers

void CAddSetupAidedPlateDlg::SetupAidedPlateSerialize(CString sFileName, BOOL bWrite)
{
	FILE *fp;
	char file_path[MAX_PATH+1],line_txt[MAX_TEMP_CHAR_100+1],sText[MAX_TEMP_CHAR_50+1],sPartNo[MAX_CHAR_GUIGE_16+1];
	_snprintf(file_path,MAX_PATH,"%s",sFileName);
	if(strlen(file_path)<=0)
		return;
	int iRow;
	try
	{
		if(bWrite)	//����
		{
			if((fp=fopen(file_path,"wt"))==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "�򲻿���װ�����ְ��,��װ�����ְ�Ᵽ�ֲ��䣡";
#else 
				throw "�򲻿���װ�����ְ��,��װ�����ְ�Ᵽ�ֲ��䣡";
#endif
			m_nPlateNum=0;	//�������
			int nCount=m_pListCtrl->GetItemCount();
			fprintf(fp,"%d \n",nCount);
			for(iRow=0;iRow<nCount;iRow++)
			{
				//��ţ��뾶���нǣ���ʼ�ְ�����ȣ���ֹ�ְ������
				m_pListCtrl->GetItemText(iRow,0,sPartNo,MAX_CHAR_GUIGE_16);
				m_pListCtrl->GetItemText(iRow,1,sText,MAX_TEMP_CHAR_50);
				m_fRadius=atof(sText);
				m_pListCtrl->GetItemText(iRow,2,sText,MAX_TEMP_CHAR_50);
				m_fWidth=atof(sText);
				m_pListCtrl->GetItemText(iRow,3,sText,MAX_TEMP_CHAR_50);
				m_fAngle=atof(sText);
				m_pListCtrl->GetItemText(iRow,4,sText,MAX_TEMP_CHAR_50);
				m_fStartPlateHalfT=atof(sText);
				m_pListCtrl->GetItemText(iRow,5,sText,MAX_TEMP_CHAR_50);
				m_fEndPlateHalfT=atof(sText);
				m_pListCtrl->GetItemText(iRow,6,sText,MAX_TEMP_CHAR_50);
#ifdef AFX_TARG_ENU_ENGLISH
				if(strcmp(sText,"Internal line")==0)
#else 
				if(strcmp(sText,"�ڲ�ֱ��")==0)
#endif
					m_iProfileStyle=0;
				else
					m_iProfileStyle=1;
				_snprintf(line_txt,MAX_TEMP_CHAR_100,"%s\n",sPartNo);
				fputs(line_txt,fp);
				_snprintf(line_txt,MAX_TEMP_CHAR_100,"%lf %lf %lf %lf %lf %d\n",m_fRadius,m_fAngle,m_fStartPlateHalfT,m_fEndPlateHalfT,m_fWidth,m_iProfileStyle);
				fputs(line_txt,fp);
				strncpy(aided_plate[m_nPlateNum].sPartNo, m_sPlatePartNo,15);
				aided_plate[m_nPlateNum].radius=m_fRadius;
				aided_plate[m_nPlateNum].angle=m_fAngle;
				aided_plate[m_nPlateNum].start_half_t=m_fStartPlateHalfT;
				aided_plate[m_nPlateNum].end_half_t=m_fEndPlateHalfT;
				aided_plate[m_nPlateNum].profile_style=m_iProfileStyle;
				aided_plate[m_nPlateNum].width=m_fWidth;
				m_nPlateNum++;
			}
		}
		else		//����
		{
			if((fp=fopen(file_path,"rt"))==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "�򲻿���װ�����ְ��,��װ�����ְ�Ᵽ�ֲ��䣡";
#else 
				throw "�򲻿���װ�����ְ��,��װ�����ְ�Ᵽ�ֲ��䣡";
#endif
			m_nPlateNum=0;	//�������
			m_pListCtrl->DeleteAllItems();
			int nCount=0;
			fscanf(fp,"%d",&nCount);
			fgets(line_txt,100,fp);
			for(iRow=0;iRow<2*nCount;iRow++)
			{
				CString tempStr;
				CStringArray str_arr;
				fgets(sPartNo,16,fp);
				//ȥ���ַ�����β�Ļ��з�
				int str_len=strlen(sPartNo);
				if(sPartNo[str_len-1]=='\n')
					sPartNo[str_len-1]='\0';
				tempStr.Format("%s",sPartNo);
				str_arr.Add(tempStr);
				fgets(line_txt,100,fp);
				sscanf(line_txt,"%lf%lf%lf%lf%lf%d",&m_fRadius,&m_fAngle,&m_fStartPlateHalfT,&m_fEndPlateHalfT,&m_fWidth,&m_iProfileStyle);
				tempStr.Format("%f",m_fRadius);
				SimplifiedNumString(tempStr);
				str_arr.Add(tempStr);
				tempStr.Format("%.2f",m_fAngle);
				SimplifiedNumString(tempStr);
				str_arr.Add(tempStr);
				tempStr.Format("%f",m_fStartPlateHalfT);
				SimplifiedNumString(tempStr);
				str_arr.Add(tempStr);
				tempStr.Format("%f",m_fEndPlateHalfT);
				SimplifiedNumString(tempStr);
				str_arr.Add(tempStr);
				tempStr.Format("%f",m_fWidth);
				SimplifiedNumString(tempStr);
				str_arr.Add(tempStr);
#ifdef AFX_TARG_ENU_ENGLISH
				if(m_iProfileStyle==0)
					tempStr="Internal line";
				else
					tempStr="Internal arc";
#else 
				if(m_iProfileStyle==0)
					tempStr="�ڲ�ֱ��";
				else
					tempStr="�ڲ�Բ��";
#endif
				SimplifiedNumString(tempStr);
				str_arr.Add(tempStr);
				AppendRecord(-1,str_arr);
				iRow++;
				strncpy(aided_plate[m_nPlateNum].sPartNo, sPartNo,15);
				aided_plate[m_nPlateNum].radius=m_fRadius;
				aided_plate[m_nPlateNum].angle=m_fAngle;
				aided_plate[m_nPlateNum].start_half_t=m_fStartPlateHalfT;
				aided_plate[m_nPlateNum].end_half_t=m_fEndPlateHalfT;
				aided_plate[m_nPlateNum].profile_style=m_iProfileStyle;
				aided_plate[m_nPlateNum].width=m_fWidth;
				m_nPlateNum++;
			}
		}
		fclose(fp);
	}
	catch(char* sError)
	{
#if defined(__LDS_FILE_)||defined(__LDS_)
		MessageBox(sError,"LDS",MB_OK);
#elif defined(__LMA_FILE_)||defined(__LMA_)
		MessageBox(sError,"LMA",MB_OK);
#elif defined(__TMA_FILE_)||defined(__TMA_)
		MessageBox(sError,"TMA",MB_OK);
#elif defined(__TSA_FILE_)||defined(__TSA_)
		MessageBox(sError,"TSA",MB_OK);
#elif defined(__TDA_FILE_)||defined(__TDA_)
		MessageBox(sError,"TDA",MB_OK);
#endif
	}
}

BOOL CAddSetupAidedPlateDlg::OnInitDialog() 
{
	
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_SETUP_AIDED_PLATE);
	if(m_pListCtrl)
		m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	if(m_bPauseBreakExit)
	{
		for(int i=0;i<m_nPlateNum;i++)
		{
			CString tempStr;
			CStringArray str_arr;
			tempStr.Format("%s",aided_plate[i].sPartNo);
			str_arr.Add(tempStr);
			tempStr.Format("%f",aided_plate[i].radius);
			SimplifiedNumString(tempStr);
			str_arr.Add(tempStr);
			tempStr.Format("%f",aided_plate[i].width);
			SimplifiedNumString(tempStr);
			str_arr.Add(tempStr);
			tempStr.Format("%.2f",aided_plate[i].angle);
			SimplifiedNumString(tempStr);
			str_arr.Add(tempStr);
			tempStr.Format("%f",aided_plate[i].start_half_t);
			SimplifiedNumString(tempStr);
			str_arr.Add(tempStr);
			tempStr.Format("%f",aided_plate[i].end_half_t);
			SimplifiedNumString(tempStr);
			str_arr.Add(tempStr);
			AppendRecord(-1,str_arr);
		}
		m_bPauseBreakExit=FALSE;
	}
	CDialog::OnInitDialog();
	return TRUE;
}

void CAddSetupAidedPlateDlg::RefreshListViewHeader()
{
	LV_COLUMN lvc;
	int i, NUM_COLUMNS, nColumnCount;
	CHeaderCtrl *pHeader = m_pListCtrl->GetHeaderCtrl();
	nColumnCount = pHeader->GetItemCount();

	// Delete all of the columns.
	for (i=0;i < nColumnCount;i++)
	{
	   m_pListCtrl->DeleteColumn(0);
	}
	if(m_arrHeaderLabel.GetSize()>0)
		NUM_COLUMNS = m_arrHeaderLabel.GetSize();
	else
		NUM_COLUMNS = 0;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(i = 0; i<NUM_COLUMNS; i++)
	{
		lvc.iSubItem = i;
		lvc.pszText = m_arrHeaderLabel[i].GetBuffer(1);//_gszColumnLabel[i];
		if(i<1)
			lvc.cx = 55;
		else if(i==1||i==2)
			lvc.cx = 40;
		else if(i==3)
			lvc.cx = 55;
		else if(i==4||i==5)
			lvc.cx = 30;	
		else
			lvc.cx = 70;
		
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}

int CAddSetupAidedPlateDlg::AppendRecord(int iItem, CStringArray &str_arr)
{
	//insert items
	LV_ITEM lvi;
	if(iItem==-1)
		iItem=m_pListCtrl->GetItemCount();

	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = str_arr[0].GetBuffer(1);//_gszItem[i][0];
	iItem = m_pListCtrl->InsertItem(&lvi);
	//set item text for additional columns
	int nCount = m_arrHeaderLabel.GetSize();
	nCount = __min(nCount,str_arr.GetSize());
	for(int j = 1; j<nCount; j++)
	{
		str_arr[j].TrimLeft();
		str_arr[j].TrimRight();
		m_pListCtrl->SetItemText(iItem,j,str_arr[j]);
	}
	m_pListCtrl->RedrawItems(iItem,iItem);
	return iItem;
}

void CAddSetupAidedPlateDlg::OnOK() 
{
	if(m_pListCtrl->GetItemCount()>0)
		OnExportList();
	CDialog::OnOK();
}

void CAddSetupAidedPlateDlg::OnDblclkListSetupAidedPlate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
			ModifyItem(iCurSel);
	}
	*pResult = 0;
}

void CAddSetupAidedPlateDlg::OnColumnclickListSetupAidedPlate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	listViewComparer.m_pListCtrl = m_pListCtrl;
	listViewComparer.m_iColumn = pNMListView->iSubItem;
	listViewComparer.sortOrderAscending[pNMListView->iSubItem]=!listViewComparer.sortOrderAscending[pNMListView->iSubItem];
	m_pListCtrl->SortItems(CompareItemProc,(DWORD)&listViewComparer);
	*pResult = 0;
}

void CAddSetupAidedPlateDlg::OnKeydownListSetupAidedPlate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_F3)
		{
			if(iCurSel>=0)
				ModifyItem(iCurSel);
		}
		else if(pLVKeyDow->wVKey==VK_DELETE)
		{
			while(iCurSel>=0)
			{
				m_pListCtrl->DeleteItem(iCurSel);
				pos=m_pListCtrl->GetFirstSelectedItemPosition();
				if(pos==NULL)
					break;
				iCurSel=m_pListCtrl->GetNextSelectedItem(pos);
			}
		}
	}
	*pResult = 0;
}

void CAddSetupAidedPlateDlg::ModifyItem(int iCurSel)
{
	char sText[51];
	m_pListCtrl->GetItemText(iCurSel,0,sText,50);
	m_sPlatePartNo.Format("%s",sText);
	m_pListCtrl->GetItemText(iCurSel,1,sText,50);
	m_fRadius=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,2,sText,50);
	m_fWidth=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,3,sText,50);
	m_fAngle=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,4,sText,50);
	m_fStartPlateHalfT=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,5,sText,50);
	m_fEndPlateHalfT=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,6,sText,50);
#ifdef AFX_TARG_ENU_ENGLISH
	if(strcmp(sText,"Internal line")==0)
#else 
	if(strcmp(sText,"�ڲ�ֱ��")==0)
#endif
		m_iProfileStyle=0;
	else
		m_iProfileStyle=1;
	UpdateData(FALSE);
}

void CAddSetupAidedPlateDlg::OnAddSetupAidedPlate() 
{
	UpdateData();
	CString tempStr;
	CStringArray str_arr;
	int nCount=m_pListCtrl->GetItemCount();
	for(int i=0;i<nCount;i++)
	{
		char sText[51];
		CString sPartNo;
		m_pListCtrl->GetItemText(i,0,sText,50);
		sPartNo.Format("%s",sText);
		if(m_sPlatePartNo.CompareNoCase(sPartNo)==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
#if defined(__LDS_FILE_)||defined(__LDS_)
			MessageBox("The label code of ancillary plate has existed that we can't add again!","LDS",MB_OK);
#elif defined(__LMA_FILE_)||defined(__LMA_)
			MessageBox("The label code of ancillary plate has existed that we can't add again!","LMA",MB_OK);
#elif defined(__TMA_FILE_)||defined(__TMA_)
			MessageBox("The label code of ancillary plate has existed that we can't add again!","TMA",MB_OK);
#endif
#else 
#if defined(__LDS_FILE_)||defined(__LDS_)
			MessageBox("�Ѵ�����ͬ��ŵĸ����ְ壬�����ظ����!","LDS",MB_OK);
#elif defined(__LMA_FILE_)||defined(__LMA_)
			MessageBox("�Ѵ�����ͬ��ŵĸ����ְ壬�����ظ����!","LMA",MB_OK);
#elif defined(__TMA_FILE_)||defined(__TMA_)
			MessageBox("�Ѵ�����ͬ��ŵĸ����ְ壬�����ظ����!","TMA",MB_OK);
#endif
#endif
			return;
		}
	}
/*
	long startHandle;
	sscanf(m_sStartPlateHandle,"%X",&startHandle);
	long endHandle;
	sscanf(m_sEndPlateHandle,"%X",&endHandle);
	CLDSPlate* pStartPlate = (CLDSPlate*)console.FromPartHandle(startHandle,CLS_PLATE);
	CLDSPlate* pEndPlate = (CLDSPlate*)console.FromPartHandle(startHandle,CLS_PLATE);
	CLDSLineTube* pBaseLine = (CLDSLineTube*)console.FromPartHandle(pStartPlate->m_hPartWeldParent);
	this->DisplayTmpModel(pStartPlate,pEndPlate,pBaseLine,m_tmpHandle);
	m_tmpHandle++;
*/
	tempStr.Format("%s",m_sPlatePartNo);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fRadius);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fWidth);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%.2f",m_fAngle);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fStartPlateHalfT);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fEndPlateHalfT);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
#ifdef AFX_TARG_ENU_ENGLISH
	if(m_iProfileStyle==0)
		tempStr="Internal line";
	else
		tempStr="Internal arc";
#else 
	if(m_iProfileStyle==0)
		tempStr="�ڲ�ֱ��";
	else
		tempStr="�ڲ�Բ��";
#endif
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	AppendRecord(-1,str_arr);
	strncpy(aided_plate[m_nPlateNum].sPartNo, m_sPlatePartNo,15);
	aided_plate[m_nPlateNum].radius=m_fRadius;
	aided_plate[m_nPlateNum].angle=m_fAngle;
	aided_plate[m_nPlateNum].start_half_t=m_fStartPlateHalfT;
	aided_plate[m_nPlateNum].end_half_t=m_fEndPlateHalfT;
	aided_plate[m_nPlateNum].profile_style=m_iProfileStyle;
	aided_plate[m_nPlateNum].width=m_fWidth;
	m_nPlateNum++;
}
struct NODE_PLATE
{
	CLDSNode* BaseNode;//��׼�ڵ�
	int  nNum;//�ڵ�����
	CLDSPlate* plateArr[20];//�ڵ��ϰ��ָ��
};
//�Զ�ͳ�ƹ�װ�����ְ�
void CAddSetupAidedPlateDlg::OnAutoStat() 
{
	CString tempStr;
	CStringArray str_arr;
	CLDSPlate* pStartPlate=NULL;
	CLDSPlate* pEndPlate=NULL;
	CLDSPlate* tmpPlate=NULL;
	//����HashTable
	CHashTable<NODE_PLATE*>partsetTable;
	partsetTable.CreateHashTable(500);
	long iBaseNodeArr[500];  
	int i=0;
	int NodeArrLen=0;
	UpdateData();
	int iPlaterStyle = m_iProfileStyle;
	for(CLDSPartGroup *pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
	{
		CLDSLineTube *pDatumTube=(CLDSLineTube*)console.FromPartHandle(pPartGroup->handle,CLS_LINETUBE);
		if(pDatumTube==NULL)
			continue;	//�麸����׼�����Ǹֹ�
		PARTSET partset;
		partset.Empty();
		Ta.GetPartGroupPartSet(pPartGroup->handle,partset);
		i=0;
		NodeArrLen = 0;
		CHashTable<CLDSPlate*>ExistPartsetTable;
		ExistPartsetTable.CreateHashTable(500);
		for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(pPart->GetClassTypeId()!=CLS_PLATE)
				continue;	//�Ǹְ�
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			if(pPlate->m_fInnerRadius>0)
				continue;	//�����
			if(pPlate->m_hPartWeldParent<0X20)
				continue;	//�޺��Ӹ�����
			CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
			if(pLineTube==NULL)
				continue;	//���Ӹ��������Ǹֹ�
			f3dPoint tube_vec=pLineTube->End()-pLineTube->Start();
			normalize(tube_vec);
			if(fabs(pPlate->ucs.axis_z*tube_vec)>EPS2)
				continue;	//�Ǹֹ�����ְ�
			if(pPlate->designInfo.m_hBaseNode<0X20)
				continue;	//�ְ��޻�׼�ڵ�
			if(pLineTube->GetDiameter()*0.5<10)
				continue;	//��������
			NODE_PLATE *tmpNodePlate;
			if(partsetTable.GetValueAt(pPlate->designInfo.m_hBaseNode,tmpNodePlate))
			{
				if(tmpNodePlate->plateArr[0]->m_hPartWeldParent != pPlate->m_hPartWeldParent)
					continue;//���Ӹ�������ͬ
				tmpNodePlate->plateArr[tmpNodePlate->nNum] = pPlate;
				tmpNodePlate->nNum++;
				tmpNodePlate->BaseNode = pPlate->GetBaseNode();
			}
			else
			{
				NODE_PLATE *NodePlate = new NODE_PLATE;
				for(int h=0;h<20;h++)
					NodePlate->plateArr[i]=NULL;
				NodePlate->BaseNode=pPlate->GetBaseNode();
				NodePlate->nNum = 1;
				NodePlate->plateArr[0] = pPlate;
				partsetTable.SetValueAt(pPlate->designInfo.m_hBaseNode,NodePlate);
				NodeArrLen++;
				iBaseNodeArr[i] = pPlate->designInfo.m_hBaseNode;
				i++;
			}
		}
		if(NodeArrLen<=0)
			continue;
		CLDSPlate *pPlate=NULL;
		for(i=0;i<NodeArrLen;i++)
		{
			NODE_PLATE *tmpNodePlate;
			if(partsetTable.GetValueAt(iBaseNodeArr[i],tmpNodePlate))
			{
				for(int z = 0;z<tmpNodePlate->nNum;z++)
				{
					double fAngle=0.0;
					pPlate = tmpNodePlate->plateArr[z];
					CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
					pStartPlate = NULL;
					pEndPlate = NULL;
					CLDSPlate *pTempPlate=NULL;
					for(int j = 0;j<tmpNodePlate->nNum;j++)
					{
						pTempPlate = tmpNodePlate->plateArr[j];
						if(pTempPlate->handle == pPlate->handle)
							continue;//�����Լ�
						if(pTempPlate->m_hPartWeldParent != pPlate->m_hPartWeldParent)
							continue;//���Ӹ���������ͬ
						PROFILE_VERTEX *pVertex=NULL;
						f3dPoint axis_x,axis_x1,axis_x2,vertice;
						pLineTube->BuildUCS();
						axis_x1=pPlate->ucs.axis_z^pLineTube->ucs.axis_z;
						axis_x2=pTempPlate->ucs.axis_z^pLineTube->ucs.axis_z;
						if(pPlate->vertex_list.GetNodeNum()>0)
						{
							for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
							{
								vertice=pVertex->vertex;
								coord_trans(vertice,pPlate->ucs,TRUE);
								axis_x+=vertice;
							}
							//���ݰ�����ĵ���������췽��
							axis_x/=pPlate->vertex_list.GetNodeNum();
							project_point(axis_x,pPlate->ucs.origin,pPlate->ucs.axis_z);
							axis_x-=pPlate->ucs.origin;
							normalize(axis_x);
						}
						if(axis_x1*axis_x<0)
							axis_x1*=-1.0;
						if(pTempPlate->vertex_list.GetNodeNum()>0)
						{
							for(pVertex=pTempPlate->vertex_list.GetFirst();pVertex;pVertex=pTempPlate->vertex_list.GetNext())
							{
								vertice=pVertex->vertex;
								coord_trans(vertice,pTempPlate->ucs,TRUE);
								axis_x+=vertice;
							}
							//���ݰ�����ĵ���������췽��
							axis_x/=pTempPlate->vertex_list.GetNodeNum();
							project_point(axis_x,pTempPlate->ucs.origin,pTempPlate->ucs.axis_z);
							axis_x-=pTempPlate->ucs.origin;
							normalize(axis_x);
						}
						if(axis_x2*axis_x<0)
							axis_x2*=-1.0;
						if(fabs(fAngle)<0.5)//�Ƿ��һ��ƥ��
							fAngle=cal_angle_of_2vec(axis_x1,axis_x2)*DEGTORAD_COEF;
						else if(fabs(fAngle) < fabs(cal_angle_of_2vec(axis_x1,axis_x2)*DEGTORAD_COEF))
							continue;
						m_fAngle=fAngle=fabs(cal_angle_of_2vec(axis_x1,axis_x2)*DEGTORAD_COEF);
						if(fabs(fAngle-180)<1||fabs(fAngle)<5)//С����Ⱥͽӽ�180�ȵĲ�����
							continue;
						m_fRadius=pLineTube->GetDiameter()*0.5;
						//�ж�ǰ��,����ֹܻ�׼�߽���Ϊ��ʼ��
						pLineTube->BuildUCS();
						if(pLineTube->ucs.axis_x*axis_x1>pLineTube->ucs.axis_x*axis_x2)
						{
							m_fStartPlateHalfT=pPlate->GetThick()*0.5;
							m_fEndPlateHalfT=pTempPlate->GetThick()*0.5;
							pStartPlate = pPlate;
							pEndPlate = pTempPlate;
						}
						else
						{
							m_fStartPlateHalfT=pTempPlate->GetThick()*0.5;
							m_fEndPlateHalfT=pPlate->GetThick()*0.5;
							pStartPlate = pTempPlate;
							pEndPlate = pPlate;
						}
					}
					//�ж�����Ƿ��Ѿ�����
					if(pStartPlate&&pEndPlate&&ExistPartsetTable.GetValueAt(pStartPlate->handle,tmpPlate))
					{
						if(stricmp(tmpPlate->GetPartNo(),pEndPlate->GetPartNo())==0)
						{
							pStartPlate = NULL;
							pEndPlate = NULL;
						}
					}
					else if(pStartPlate&&pEndPlate)
							ExistPartsetTable.SetValueAt(pStartPlate->handle,pEndPlate);
					if(pEndPlate!=NULL||pStartPlate!=NULL)
					{
						m_sPlatePartNo.Format("%sA",(char*)pPlate->GetPartNo());
						m_iProfileStyle = iPlaterStyle;
						DisplayTmpModel(pStartPlate,pEndPlate,pDatumTube,m_tmpHandle);
						m_tmpHandle++;
						str_arr.RemoveAll();
						str_arr.Add(m_sPlatePartNo);
						tempStr.Format("%f",m_fRadius);
						SimplifiedNumString(tempStr);
						str_arr.Add(tempStr);
						tempStr.Format("%f",m_fWidth);
						SimplifiedNumString(tempStr);
						str_arr.Add(tempStr);
						tempStr.Format("%.2f",m_fAngle);
						SimplifiedNumString(tempStr);
						str_arr.Add(tempStr);
						tempStr.Format("%f",m_fStartPlateHalfT);
						SimplifiedNumString(tempStr);
						str_arr.Add(tempStr);
						tempStr.Format("%f",m_fEndPlateHalfT);
						SimplifiedNumString(tempStr);
						str_arr.Add(tempStr);
#ifdef AFX_TARG_ENU_ENGLISH
						if(m_iProfileStyle==0)
							tempStr="Internal line";
						else
							tempStr="Internal arc";
#else 
						if(m_iProfileStyle==0)
							tempStr="�ڲ�ֱ��";
						else
							tempStr="�ڲ�Բ��";
#endif
						SimplifiedNumString(tempStr);
						str_arr.Add(tempStr);
						AppendRecord(-1,str_arr);
					}
				}
			}
			else
				continue;//��������
		}
	}
}
/*
//�Զ�ͳ�ƹ�װ�����ְ�
void CAddSetupAidedPlateDlg::OnAutoStat() 
{
	CString tempStr;
	CStringArray str_arr;
	CLDSPlate* pStartPlate=NULL;
	CLDSPlate* pEndPlate=NULL;
	//����HashTable
	CHashTable<char*>partsetTable;
	partsetTable.CreateHashTable(500);
	UpdateData();
	for(CLDSPartGroup *pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
	{
		CLDSLineTube *pDatumTube=(CLDSLineTube*)console.FromPartHandle(pPartGroup->handle,CLS_LINETUBE);
		if(pDatumTube==NULL)
			continue;	//�麸����׼�����Ǹֹ�
		PARTSET partset;
		partset.Empty();
		Ta.GetPartGroupPartSet(pPartGroup->handle,partset);
		for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(pPart->GetClassTypeId()!=CLS_PLATE)
				continue;	//�Ǹְ�
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			if(pPlate->m_fInnerRadius>0)
				continue;	//�����
			if(pPlate->m_hPartWeldParent<0X20)
				continue;	//�޺��Ӹ�����
			CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
			if(pLineTube==NULL)
				continue;	//���Ӹ��������Ǹֹ�
			f3dPoint tube_vec=pLineTube->End()-pLineTube->Start();
			normalize(tube_vec);
			if(fabs(pPlate->ucs.axis_z*tube_vec)>EPS2)
				continue;	//�Ǹֹ�����ְ�
			if(pPlate->designInfo.m_hBaseNode<0X20)
				continue;	//�ְ��޻�׼�ڵ�
			if(pLineTube->GetDiameter()*0.5<10)
				continue;	//��������
			CLDSPlate *pTempPlate=NULL;
			if(partsetTable.GetValueAt(pPlate->designInfo.m_hBaseNode,pTempPlate))
			{
				if(pTempPlate==NULL)
					continue;	//�Ƿ�����
				if(pTempPlate->m_hPartWeldParent!=pPlate->m_hPartWeldParent)
					continue;	//���Ӹ���������ͬ	
				//���㹤װ�����ְ���ز�������ӵ��б���
				m_sPlatePartNo.Format("%s",(char*)pPlate->GetPartNo());
				m_fRadius=pLineTube->GetDiameter()*0.5;
				
				f3dPoint tmp_axis_x,axis_x1,axis_x2,tmpPoint,axis_x,axis_y;
				/*
				tmpPoint = pTempPlate->GetGeomCenter();
				tmp_axis_x = tmpPoint - pTempPlate->GetBaseNode()->pos;
				if(axis_x1*tmp_axis_x<0)
					axis_x1*=-1;
				axis_x2 = pPlate->ucs.axis_x;
				tmpPoint = pPlate->GetGeomCenter();
				tmp_axis_x = tmpPoint - pPlate->GetBaseNode()->pos;
				if(axis_x2*tmp_axis_x<0)
				axis_x2*=-1;
				*/
			/*	for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
				{
					f3dPoint vertice=pVertex->vertex;
					coord_trans(vertice,pPlate->ucs,TRUE);
					tmp_axis_x+=vertice;
				}
				//���ݰ�����ĵ���������췽��
				axis_x1 = pPlate->ucs.axis_x;
				tmp_axis_x/=pPlate->vertex_list.GetNodeNum();
				project_point(axis_x1,pPlate->ucs.origin,pPlate->ucs.axis_z);
				tmp_axis_x-=pPlate->ucs.origin;
				normalize(axis_x);
				if(axis_x1*tmp_axis_x<0)
					axis_x1*=-1;

				for(pVertex=pTempPlate->vertex_list.GetFirst();pVertex;pVertex=pTempPlate->vertex_list.GetNext())
				{
					f3dPoint vertice=pVertex->vertex;
					coord_trans(vertice,pTempPlate->ucs,TRUE);
					tmp_axis_x+=vertice;
				}
				//���ݰ�����ĵ���������췽��
				axis_x2 = pTempPlate->ucs.axis_x;
				tmp_axis_x/=pTempPlate->vertex_list.GetNodeNum();
				tmp_axis_x-=pTempPlate->ucs.origin;
				normalize(axis_x);
				if(axis_x2*tmp_axis_x<0)
					axis_x2*=-1;

				normalize(axis_x1);
				normalize(axis_x2);
				axis_x=axis_x1+axis_x2;
				axis_y = pLineTube->ucs.axis_z^axis_x;
				normalize(axis_y);
				if(axis_x1*axis_y<0)
				{
					pStartPlate = pTempPlate;
					m_fStartPlateHalfT = pTempPlate->GetThick()*0.5;
					pEndPlate = pPlate;
					m_fEndPlateHalfT = pPlate->GetThick()*0.5;
				}
				else
				{
					pStartPlate = pPlate;
					m_fStartPlateHalfT = pPlate->GetThick()*0.5;
					pEndPlate = pTempPlate;
					m_fEndPlateHalfT = pTempPlate->GetThick()*0.5;
				}
				m_fAngle=cal_angle_of_2vec(axis_x1,axis_x2)*DEGTORAD_COEF;
				m_fRadius=pLineTube->GetDiameter()*0.5;
				if(fabs(m_fAngle-180)>0.5&&fabs(m_fAngle)>0.5)
					continue;//0��180�������
		
				BOOL bRet=FALSE;
				int nCount=m_pListCtrl->GetItemCount();
				for(int i=0;i<nCount;i++)
				{
					char sText[50];
					double r,start_t,end_t,angle;
					CString sPartNo;
					m_pListCtrl->GetItemText(i,0,sText,50);
					sPartNo.Format("%s",sText);
					m_pListCtrl->GetItemText(i,1,sText,50);
					r=atof(sText);
					m_pListCtrl->GetItemText(i,2,sText,50);
					angle=atof(sText);
					m_pListCtrl->GetItemText(i,3,sText,50);
					start_t=atof(sText);
					m_pListCtrl->GetItemText(i,4,sText,50);
					end_t=atof(sText);
					if(fabs(r-m_fRadius)<EPS&&fabs(angle-m_fAngle)<EPS2&&
						((fabs(start_t-m_fStartPlateHalfT)<EPS&&fabs(end_t-m_fEndPlateHalfT)<EPS)||
						(fabs(start_t-m_fEndPlateHalfT)<EPS&&fabs(end_t-m_fStartPlateHalfT)<EPS)))
					{
						bRet=TRUE;
						break;
					}
					else if(sPartNo.CompareNoCase(pPlate->GetPartNo())==0)
						m_sPlatePartNo.Format("%sA",(char*)pPlate->GetPartNo());	
				}
				if(bRet)	//�б��д�����ͬ��ŵĹ�װ�����ְ�
					continue;
				DisplayTmpModel(pStartPlate,pEndPlate,pDatumTube);
				str_arr.RemoveAll();
				str_arr.Add(m_sPlatePartNo);
				tempStr.Format("%f",m_fRadius);
				SimplifiedNumString(tempStr);
				str_arr.Add(tempStr);
				tempStr.Format("%f",m_fAngle);
				SimplifiedNumString(tempStr);
				str_arr.Add(tempStr);
				tempStr.Format("%f",m_fStartPlateHalfT);
				SimplifiedNumString(tempStr);
				str_arr.Add(tempStr);
				tempStr.Format("%f",m_fEndPlateHalfT);
				SimplifiedNumString(tempStr);
				str_arr.Add(tempStr);
				if(m_iProfileStyle==0)
					tempStr="�ڲ�ֱ��";
				else
					tempStr="�ڲ�Բ��";
				SimplifiedNumString(tempStr);
				str_arr.Add(tempStr);
				//�нǽӽ�0���180�㲻��Ҫ��װ�����ְ� ��ȷ��0.5
				if(fabs(m_fAngle-180)>0.5&&fabs(m_fAngle)>0.5)
					AppendRecord(-1,str_arr);
			}
			else
				partsetTable.SetValueAt(pPlate->designInfo.m_hBaseNode,(char)(pPlate));
		}
	}
	UpdateData(FALSE);
}
*/
void CAddSetupAidedPlateDlg::OnExportList() 
{
	CString sFileName;
	char *sName="sap";
#ifdef AFX_TARG_ENU_ENGLISH
	char *sFileType="��װ�����ְ��ļ�(*.sap)|*.sap||";
#else 
	char *sFileType="��װ�����ְ��ļ�(*.sap)|*.sap||";
#endif
	CFileDialog dlg(TRUE,sName,"AidedPlate",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		SetupAidedPlateSerialize(sFileName, TRUE);
	}
}

void CAddSetupAidedPlateDlg::OnImportList() 
{
	CString sFileName;
	char *sName="sap";
#ifdef AFX_TARG_ENU_ENGLISH
	char *sFileType="��װ�����ְ��ļ�(*.sap)|*.sap||";
#else 
	char *sFileType="��װ�����ְ��ļ�(*.sap)|*.sap||";
#endif
	CFileDialog dlg(TRUE,sName,"AidedPlate",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		SetupAidedPlateSerialize(sFileName, FALSE);
	}
}

void CAddSetupAidedPlateDlg::OnSelectPart() 
{
	UpdateData();
	m_bPauseBreakExit=TRUE;
	CDialog::OnOK();
}

void CAddSetupAidedPlateDlg::OnModifySetupAidedPlate() 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		UpdateData();
		if(iCurSel>=0)
		{
			int nCount=m_pListCtrl->GetItemCount();
			for(int i=0;i<nCount;i++)
			{
				if(i==iCurSel)
					continue;
				char sText[51];
				CString sPartNo;
				m_pListCtrl->GetItemText(i,0,sText,50);
				sPartNo.Format("%s",sText);
				if(m_sPlatePartNo.CompareNoCase(sPartNo)==0)
				{
#ifdef AFX_TARG_ENU_ENGLISH
				#if defined(__LDS_FILE_)||defined(__LDS_)
					MessageBox("The label code of ancillary plate has existed that we can't add again!","LDS",MB_OK);
				#elif defined(__LMA_FILE_)||defined(__LMA_)
					MessageBox("The label code of ancillary plate has existed that we can't add again!","LMA",MB_OK);
				#elif defined(__TMA_FILE_)||defined(__TMA_)
					MessageBox("The label code of ancillary plate has existed that we can't add again!","TMA",MB_OK);
				#endif
#else 
				#if defined(__LDS_FILE_)||defined(__LDS_)
					MessageBox("�Ѵ�����ͬ��ŵĸ����ְ壬�����ظ����!","LDS",MB_OK);
				#elif defined(__LMA_FILE_)||defined(__LMA_)
					MessageBox("�Ѵ�����ͬ��ŵĸ����ְ壬�����ظ����!","LMA",MB_OK);
				#elif defined(__TMA_FILE_)||defined(__TMA_)
					MessageBox("�Ѵ�����ͬ��ŵĸ����ְ壬�����ظ����!","TMA",MB_OK);
				#endif
#endif
					return;
				}
			}
			//���¹�װ�����ְ��б�
			CString ss;
			m_pListCtrl->SetItemText(iCurSel,0,m_sPlatePartNo);
			ss.Format("%f",m_fRadius);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,1,ss);
			ss.Format("%f",m_fWidth);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,2,ss);
			ss.Format("%.2f",m_fAngle);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,3,ss);
			ss.Format("%f",m_fStartPlateHalfT);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,4,ss);
			ss.Format("%f",m_fEndPlateHalfT);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,5,ss);
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_iProfileStyle==0)
				ss="Internal line";
			else
				ss="Internal arc";
#else 
			if(m_iProfileStyle==0)
				ss="�ڲ�ֱ��";
			else
				ss="�ڲ�Բ��";
#endif
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,6,ss);
		}
	}
}
void CAddSetupAidedPlateDlg::DisplayTmpModel(CLDSPlate* pStartPlate,CLDSPlate* pEndPlate,CLDSLineTube* pBaseTube,int TmpHandle)
{
	CLDSPlate* pDisplayPlate;
	f3dPoint offset_vector;//���ƫ�Ʒ���
	f3dPoint datum_pos;
	f3dPoint TmpPoint;
	PROFILE_VERTEX* pPlateVertex;
	f3dLine  rotateAxis;
	double   fScale=0.8,fScale1=0.7;//����ϵ��
	double   fAngle,fTan,fCos;//���ְ�нǺ�һ�������ֵ
	f3dPoint axisStart,axisEnd;//���ְ�����췽��
	//double   r;//�ֹܰ뾶
	f3dPoint vector_x;//�ְ�X�᷽��

	if(pStartPlate==NULL||pEndPlate==NULL||pBaseTube==NULL)
		return;
	pDisplayPlate = (CLDSPlate*)DisplayTmpParts.append(CLS_PLATE,FALSE);
	pDisplayPlate->handle = Ta.handle+(long)TmpHandle;
	//pDisplayPlate = (CLDSPlate*)console.AppendPart(CLS_PLATE,TRUE);
	//r = pBaseTube->GetDiameter()*0.5;
	pBaseTube->BuildUCS();
	datum_pos = pStartPlate->GetBaseNode()->Position(true);
	f3dPoint axis_x;
	PROFILE_VERTEX* pVertex;
	for(pVertex=pStartPlate->vertex_list.GetFirst();pVertex;pVertex=pStartPlate->vertex_list.GetNext())
	{
		f3dPoint vertice=pVertex->vertex;
		coord_trans(vertice,pStartPlate->ucs,TRUE);
		axis_x+=vertice;
	}
	//���ݰ�����ĵ���������췽��
	axisStart = pStartPlate->ucs.axis_z^pBaseTube->ucs.axis_z;
	axis_x/=pStartPlate->vertex_list.GetNodeNum();
	project_point(axis_x,pStartPlate->ucs.origin,pStartPlate->ucs.axis_z);
	axis_x-=pStartPlate->ucs.origin;
	normalize(axis_x);
	normalize(axisStart);
	if(axisStart*axis_x<0)
		axisStart*=-1;
	
	for(pVertex=pEndPlate->vertex_list.GetFirst();pVertex;pVertex=pEndPlate->vertex_list.GetNext())
	{
		f3dPoint vertice=pVertex->vertex;
		coord_trans(vertice,pEndPlate->ucs,TRUE);
		axis_x+=vertice;
	}
	//���ݰ�����ĵ���������췽��
	axisEnd = pEndPlate->ucs.axis_z^pBaseTube->ucs.axis_z;
	axis_x/=pEndPlate->vertex_list.GetNodeNum();
	project_point(axis_x,pEndPlate->ucs.origin,pEndPlate->ucs.axis_z);
	axis_x-=pEndPlate->ucs.origin;
	normalize(axis_x);
	normalize(axisEnd);
	if(axisEnd*axis_x<0)
		axisEnd*=-1;
	/*
	//��ʼ�ְ�����췽��
	axisStart = pStartPlate->ucs.axis_x;
	TmpPoint = pStartPlate->GetGeomCenter();
	coord_trans(TmpPoint,pStartPlate->ucs,TRUE);
	axis_x = TmpPoint - datum_pos;
	normalize(axis_x);
	normalize(axisStart);
	if(axis_x*axisStart<0)
		axisStart*=-1;
	//�����ְ�����췽��
	axisEnd = pEndPlate->ucs.axis_x;
	TmpPoint = pEndPlate->GetGeomCenter();
	coord_trans(TmpPoint,pEndPlate->ucs,TRUE);
	axis_x = TmpPoint - datum_pos;
	normalize(axis_x);
	normalize(axisEnd);
	if(axis_x*axisEnd<0)
		axisEnd*=-1;
	*/
	//�����ְ�ļн�
	fAngle = cal_angle_of_2vec(axisStart,axisEnd);
	fTan = tan(fAngle*0.5);
	fCos = cos(fAngle*0.5);
	m_fAngle = fAngle*DEGTORAD_COEF;//�����ְ�ĽǶ�
	if(fAngle>(Pi*0.7))
		m_iProfileStyle=1;
	//��������ϵ
	pDisplayPlate->ucs.origin = datum_pos;
	pDisplayPlate->ucs.axis_x=axisStart+axisEnd;
	pDisplayPlate->ucs.axis_z = pBaseTube->ucs.axis_z;
	normalize(pDisplayPlate->ucs.axis_x);
	normalize(pDisplayPlate->ucs.axis_z);
	pDisplayPlate->ucs.axis_y = pDisplayPlate->ucs.axis_z^pDisplayPlate->ucs.axis_x;
	normalize(pDisplayPlate->ucs.axis_y);
	//����һ����ת��
	/*
	rotateAxis.startPt = pBaseTube->Start();
	coord_trans(rotateAxis.startPt, pDisplayPlate->ucs, FALSE);
	//rotateAxis.startPt.z = 0;
	rotateAxis.endPt = pBaseTube->End();
	coord_trans(rotateAxis.endPt, pDisplayPlate->ucs, FALSE);
	//rotateAxis.endPt.z = 0;
	*/
	rotateAxis.startPt.Set(0,0,100);
	rotateAxis.endPt.Set(0,0,-100);
	//�ְ�����
	pDisplayPlate->face_N=1;
	vector_x.Set(1,0,0);
	//��������µ�ԭ��
	datum_pos.Set(0,0,0);
	if(m_iProfileStyle == 0)
	{
		offset_vector = pDisplayPlate->ucs.axis_x;
		vector_trans(offset_vector, pDisplayPlate->ucs, FALSE);
		rotate_point_around_axis(offset_vector,-Pi*0.5,rotateAxis);
		normalize(offset_vector);
		//��һ������
		TmpPoint = datum_pos + m_fRadius*vector_x;
		pPlateVertex = pDisplayPlate->vertex_list.append();
		pPlateVertex->vertex = TmpPoint + m_fRadius*fTan*offset_vector;
		pPlateVertex->vertex-= m_fEndPlateHalfT/fCos*offset_vector;//��ȥ���
		pPlateVertex->vertex.z=0;
		pPlateVertex->vertex.feature = 1;
		//�ڶ���
		TmpPoint = datum_pos + (m_fRadius+m_fWidth)*fScale*vector_x;
		pPlateVertex = pDisplayPlate->vertex_list.append();
		pPlateVertex->vertex = TmpPoint +(m_fRadius+m_fWidth)*fTan*fScale*offset_vector;
		pPlateVertex->vertex-= m_fEndPlateHalfT/fCos*offset_vector;//��ȥ���
		pPlateVertex->vertex.z = 0;
		pPlateVertex->vertex.feature = 1;
		//������
		TmpPoint = datum_pos + (m_fRadius+m_fWidth)*vector_x;
		pPlateVertex = pDisplayPlate->vertex_list.append();
		pPlateVertex->vertex = TmpPoint+(m_fRadius+m_fWidth)*fTan*fScale1*offset_vector;
		pPlateVertex->vertex-= m_fEndPlateHalfT/fCos*offset_vector;//��ȥ���
		pPlateVertex->vertex.z = 0;
		pPlateVertex->vertex.feature = 1;
		//�ԳƵĵ�
		offset_vector*=-1;
		//���ĸ�����
		TmpPoint = datum_pos + (m_fRadius+m_fWidth)*vector_x;
		pPlateVertex = pDisplayPlate->vertex_list.append();
		pPlateVertex->vertex = TmpPoint+(m_fRadius+m_fWidth)*fTan*fScale1*offset_vector;
		pPlateVertex->vertex-= m_fStartPlateHalfT/fCos*offset_vector;//��ȥ���
		pPlateVertex->vertex.z = 0;
		pPlateVertex->vertex.feature = 1;
		//�����
		TmpPoint = datum_pos + (m_fRadius+m_fWidth)*fScale*vector_x;
		pPlateVertex = pDisplayPlate->vertex_list.append();
		pPlateVertex->vertex = TmpPoint +(m_fRadius+m_fWidth)*fTan*fScale*offset_vector;
		pPlateVertex->vertex-= m_fStartPlateHalfT/fCos*offset_vector;//��ȥ���
		pPlateVertex->vertex.z = 0;
		pPlateVertex->vertex.feature = 1;
		//������
		TmpPoint = datum_pos + m_fRadius*vector_x;
		pPlateVertex = pDisplayPlate->vertex_list.append();
		pPlateVertex->vertex = TmpPoint + m_fRadius*fTan*offset_vector;
		pPlateVertex->vertex-= m_fStartPlateHalfT/fCos*offset_vector;//��ȥ���
		pPlateVertex->vertex.z = 0;
		pPlateVertex->vertex.feature = 1;
	}
	else
	{	//������ʼ�ְ�ͽ����ְ� 
		vector_trans(axisStart, pDisplayPlate->ucs, FALSE);
		normalize(axisStart);
		vector_trans(axisEnd, pDisplayPlate->ucs, FALSE);
		normalize(axisEnd);
		f3dPoint tmp_offset;
		f3dPoint vector_y;
		vector_y.Set(0,1,0);
		if(axisStart*vector_y>0)
		{
			tmp_offset = axisEnd;
			axisEnd = axisStart;
			axisStart = tmp_offset;
			double tmp = m_fStartPlateHalfT;
			m_fStartPlateHalfT = m_fEndPlateHalfT;
			m_fEndPlateHalfT = tmp;
		}
		//���ƫ�Ʒ���
		offset_vector = axisStart;
		rotate_point_around_axis(offset_vector,Pi*0.5,rotateAxis);
		if(offset_vector*vector_x<0)
			offset_vector*=-1;
		normalize(offset_vector);
		//��һ��
		pPlateVertex = pDisplayPlate->vertex_list.append();
		pPlateVertex->vertex = m_fRadius*axisStart;
		pPlateVertex->vertex +=m_fStartPlateHalfT*offset_vector;
		pPlateVertex->vertex.z = 0;
		pPlateVertex->vertex.feature = 1;
		//�ڶ���
		pPlateVertex = pDisplayPlate->vertex_list.append();
		pPlateVertex->vertex = (m_fRadius+m_fWidth)*axisStart;
		pPlateVertex->vertex +=m_fStartPlateHalfT*offset_vector;
		pPlateVertex->vertex.z = 0;
		pPlateVertex->vertex.feature = 1;
		pPlateVertex->type = 2;
		pPlateVertex->center.Set();
		pPlateVertex->radius = m_fRadius+m_fWidth;
		//���ƫ�Ʒ���
		offset_vector = axisEnd;
		rotate_point_around_axis(offset_vector,Pi*0.5,rotateAxis);
		if(offset_vector*vector_x<0)
			offset_vector*=-1;
		normalize(offset_vector);
		//������
		pPlateVertex = pDisplayPlate->vertex_list.append();
		pPlateVertex->vertex = (m_fRadius+m_fWidth)*axisEnd;
		pPlateVertex->vertex+=m_fEndPlateHalfT*offset_vector;
		pPlateVertex->vertex.z = 0;
		pPlateVertex->vertex.feature = 1;
		//���ĸ�
		pPlateVertex = pDisplayPlate->vertex_list.append();
		pPlateVertex->vertex = m_fRadius*axisEnd;
		pPlateVertex->vertex+=m_fEndPlateHalfT*offset_vector;
		pPlateVertex->vertex.z=0;
		pPlateVertex->vertex.feature = 1;
		pPlateVertex->type = 2;
		pPlateVertex->center.Set();
		pPlateVertex->radius = -m_fRadius;
	}
	//������ɫ
	COLORREF color;
	pDisplayPlate->m_bUserSpecColor = TRUE;
	color=RGB(150,200,100);
	pDisplayPlate->crMaterial = color;
	//��ʾ
	pDisplayPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pDisplayPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
}
#endif