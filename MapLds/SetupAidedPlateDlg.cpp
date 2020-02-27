//<LOCALE_TRANSLATE/>
// SetupAidedPlateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tower.h"
#include "SetupAidedPlateDlg.h"
#include "RxTools.h"
#include "GlobalFunc.h"
#include "SysPara.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupAidedPlateDlg dialog


CSetupAidedPlateDlg::CSetupAidedPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupAidedPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupAidedPlateDlg)
	m_fRadius = 0.0;
	m_fStartPlateHalfT = 0.0;
	m_sSetupAidedPlatePartNo = _T("");
	m_fEndPlateHalfT = 0.0;
	m_fAngle = 0.0;
	m_sAidedPlateFilePath = _T("");
	m_iProfileStyle = 0;
	m_fWidth = 0.0;
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("No.");
	m_arrHeaderLabel.Add("radius");
	m_arrHeaderLabel.Add("width");
	m_arrHeaderLabel.Add("angle");
	m_arrHeaderLabel.Add("T1");
	m_arrHeaderLabel.Add("T2");
	m_arrHeaderLabel.Add("type");
#else
	m_arrHeaderLabel.Add("编号");
	m_arrHeaderLabel.Add("半径");
	m_arrHeaderLabel.Add("宽度");
	m_arrHeaderLabel.Add("夹角");
	m_arrHeaderLabel.Add("T1");
	m_arrHeaderLabel.Add("T2");
	m_arrHeaderLabel.Add("样式");
#endif
	listViewComparer.SetMaxColumnCount(7);
}


void CSetupAidedPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupAidedPlateDlg)
	DDX_Text(pDX, IDC_E_RADIUS, m_fRadius);
	DDX_Text(pDX, IDC_E_START_PLATE_HALF_T, m_fStartPlateHalfT);
	DDX_Text(pDX, IDC_E_SETUP_AIDED_PLATE_PARTNO, m_sSetupAidedPlatePartNo);
	DDX_Text(pDX, IDC_E_END_PLATE_HALF_T, m_fEndPlateHalfT);
	DDX_Text(pDX, IDC_E_ANGLE, m_fAngle);
	DDX_Text(pDX, IDC_E_AIDED_PLATE_FILE_PATH, m_sAidedPlateFilePath);
	DDX_CBIndex(pDX, IDC_CMB_PROFILE_STYLE, m_iProfileStyle);
	DDX_Text(pDX, IDC_E_WIDTH, m_fWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupAidedPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CSetupAidedPlateDlg)
	ON_BN_CLICKED(IDC_BTN_REBUILD_PARTSET, OnRebuildPartset)
	ON_BN_CLICKED(IDC_BTN_MODIFY_SETUP_AIDED_PLATE, OnModifySetupAidedPlate)
	ON_BN_CLICKED(IDC_BTN_BROWSE_FILE, OnBrowseFile)
	ON_BN_CLICKED(IDC_BTN_ADD_SETUP_AIDED_PLATE, OnAddSetupAidedPlate)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_CTRL, OnColumnclickListCtrl)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_CTRL, OnKeydownListCtrl)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CTRL, OnDblclkListCtrl)
	ON_EN_KILLFOCUS(IDC_E_AIDED_PLATE_FILE_PATH, OnKillfocusAidedPlateFilePath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupAidedPlateDlg message handlers

void CSetupAidedPlateDlg::SetupAidedPlateSerialize(CString sFileName, BOOL bWrite)
{
	FILE *fp;
	char file_path[MAX_PATH],line_txt[100],sText[50],sPartNo[16];
	int  iTmpStyle = 0;
	sprintf(file_path,"%s",sFileName);
	if(strlen(file_path)<=0)
		return;
	int iRow;
	try
	{
		if(bWrite)	//导出
		{
			if((fp=fopen(file_path,"wt"))==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Can not open the setup aided plate library file,the setup aided plate library file remain unchanged!";
#else
				throw "打不开工装辅助钢板库,工装辅助钢板库保持不变！";
#endif
			int nCount=m_pListCtrl->GetItemCount();
			fprintf(fp,"%d \n",nCount);
			m_nPlateNum=0;	//清空数组
			for(iRow=0;iRow<nCount;iRow++)
			{
				//编号，半径，夹角，起始钢板半个厚度，终止钢板半个厚度
				m_pListCtrl->GetItemText(iRow,0,sPartNo,16);
				m_pListCtrl->GetItemText(iRow,1,sText,50);
				m_fRadius=atof(sText);
				m_pListCtrl->GetItemText(iRow,2,sText,50);
				m_fWidth=atof(sText);
				m_pListCtrl->GetItemText(iRow,3,sText,50);
				m_fAngle=atof(sText);
				m_pListCtrl->GetItemText(iRow,4,sText,50);
				m_fStartPlateHalfT=atof(sText);
				m_pListCtrl->GetItemText(iRow,5,sText,50);
				m_fEndPlateHalfT=atof(sText);
				m_pListCtrl->GetItemText(iRow,6,sText,50);
#ifdef AFX_TARG_ENU_ENGLISH
				if(memicmp(sText,"straight line",2)==0)
#else
				if(memicmp(sText,"直线",2)==0)
#endif
					iTmpStyle = 0;
				else
					iTmpStyle = 1;
				sprintf(line_txt,"%s\n",sPartNo);
				fputs(line_txt,fp);
				sprintf(line_txt,"%lf %lf %lf %lf %lf %d\n",m_fRadius,m_fAngle,m_fStartPlateHalfT,m_fEndPlateHalfT,m_fWidth,iTmpStyle);
				fputs(line_txt,fp);
				strcpy(aided_plate[m_nPlateNum].sPartNo, sPartNo);
				aided_plate[m_nPlateNum].radius=m_fRadius;
				aided_plate[m_nPlateNum].angle=m_fAngle;
				aided_plate[m_nPlateNum].start_half_t=m_fStartPlateHalfT;
				aided_plate[m_nPlateNum].end_half_t=m_fEndPlateHalfT;
				aided_plate[m_nPlateNum].profile_style=iTmpStyle;
				aided_plate[m_nPlateNum].width=m_fWidth;
				m_nPlateNum++;
			}
		}
		else		//导入
		{
			if((fp=fopen(file_path,"rt"))==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Can not open the setup aided plate library file,the setup aided plate library file remain unchanged!";
#else
				throw "打不开工装辅助钢板库,工装辅助钢板库保持不变！";
#endif
			int nCount=0;
			fscanf(fp,"%d",&nCount);
			fgets(line_txt,100,fp);
			m_nPlateNum=0;	//清空数组
			m_pListCtrl->DeleteAllItems();
			for(iRow=0;iRow<2*nCount;iRow++)
			{
				CString tempStr;
				CStringArray str_arr;
				fgets(sPartNo,15,fp);
				//去掉字符串结尾的换行符
				int str_len=strlen(sPartNo);
				if(sPartNo[str_len-1]=='\n')
					sPartNo[str_len-1]='\0';
				tempStr.Format("%s",sPartNo);
				str_arr.Add(tempStr);
				m_sSetupAidedPlatePartNo=sPartNo;
				fgets(line_txt,100,fp);
				sscanf(line_txt,"%lf%lf%lf%lf%lf%d",&m_fRadius,&m_fAngle,&m_fStartPlateHalfT,&m_fEndPlateHalfT,&m_fWidth,&iTmpStyle);
				tempStr.Format("%f",m_fRadius);
				SimplifiedNumString(tempStr);
				str_arr.Add(tempStr);
				tempStr.Format("%f",m_fWidth);
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
				m_iProfileStyle=iTmpStyle;
				if(iTmpStyle == 0)
#ifdef AFX_TARG_ENU_ENGLISH
					tempStr="straight line";
				else
					tempStr="arc";
#else
					tempStr="直线";
				else
					tempStr="圆弧";
#endif
				SimplifiedNumString(tempStr);
				str_arr.Add(tempStr);
				AppendRecord(-1,str_arr);
				iRow++;
				strcpy(aided_plate[m_nPlateNum].sPartNo, sPartNo);
				aided_plate[m_nPlateNum].radius=m_fRadius;
				aided_plate[m_nPlateNum].angle=m_fAngle;
				aided_plate[m_nPlateNum].start_half_t=m_fStartPlateHalfT;
				aided_plate[m_nPlateNum].end_half_t=m_fEndPlateHalfT;
				aided_plate[m_nPlateNum].profile_style=iTmpStyle;
				aided_plate[m_nPlateNum].width=m_fWidth;
				m_nPlateNum++;
			}
		}
		fclose(fp);
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
}

BOOL CSetupAidedPlateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	m_pListCtrl->DeleteAllItems();
	m_sAidedPlateFilePath=sys.general_set.m_sSetupAidedPlate;
	if(m_sAidedPlateFilePath.GetLength()<=0)
	{
		char file_name[200]="";
		GetAppPath(file_name);
		strcat(file_name,"AidedPlate.sap");
		m_sAidedPlateFilePath.Format("%s",file_name);
	}
	//导入指定的工装辅助钢板库文件
	SetupAidedPlateSerialize(m_sAidedPlateFilePath,FALSE);
	UpdateData(FALSE);
	return TRUE;
}
void CSetupAidedPlateDlg::RefreshListViewHeader()
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
#ifdef AFX_TARG_ENU_ENGLISH
		if(i==0) 
			lvc.cx=60;
		else if(i==1)
			lvc.cx = 60;
		else if(i==3)
			lvc.cx=55;
		else if(i==4||i==5)
			lvc.cx=60;
		else
			lvc.cx = 65;
#else 
		if(i==0) 
			lvc.cx=50;
		else if(i==1)
			lvc.cx = 48;
		else if(i==3)
			lvc.cx=47;
		else if(i==4||i==5)
			lvc.cx=28;
		else
			lvc.cx = 40;
#endif
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}

int CSetupAidedPlateDlg::AppendRecord(int iItem, CStringArray &str_arr)
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

void CSetupAidedPlateDlg::OnRebuildPartset() 
{
	UpdateData();
	//导入指定的工装辅助钢板库文件
	SetupAidedPlateSerialize(m_sAidedPlateFilePath,FALSE);
}

void CSetupAidedPlateDlg::OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	if(pLVKeyDow->wVKey==VK_DELETE)
	{
		POSITION pos=m_pListCtrl->GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
			m_pListCtrl->DeleteItem(iCurSel);
			pos=m_pListCtrl->GetFirstSelectedItemPosition();
		}
	}
	*pResult = 0;
}

void CSetupAidedPlateDlg::OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CSetupAidedPlateDlg::OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	listViewComparer.m_pListCtrl = m_pListCtrl;
	listViewComparer.m_iColumn = pNMListView->iSubItem;
	listViewComparer.sortOrderAscending[pNMListView->iSubItem]=!listViewComparer.sortOrderAscending[pNMListView->iSubItem];
	m_pListCtrl->SortItems(CompareItemProc,(DWORD)&listViewComparer);
	*pResult = 0;
}

void CSetupAidedPlateDlg::OnOK() 
{
	UpdateData();
	//按当前显示顺序导出指定的工装辅助钢板库文件
	SetupAidedPlateSerialize(m_sAidedPlateFilePath,TRUE);
	CDialog::OnOK();
}

void CSetupAidedPlateDlg::ModifyItem(int iCurSel)
{
	char sText[50];
	m_pListCtrl->GetItemText(iCurSel,0,sText,50);
	m_sSetupAidedPlatePartNo.Format("%s",sText);
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
	if(stricmp(sText,"straight line")==0)
#else
	if(stricmp(sText,"直线")==0)
#endif
		m_iProfileStyle=0;
	else
		m_iProfileStyle=1;
	UpdateData(FALSE);
}

void CSetupAidedPlateDlg::OnAddSetupAidedPlate() 
{
	UpdateData();
	CString tempStr;
	CStringArray str_arr;
	int nCount=m_pListCtrl->GetItemCount();
	for(int i=0;i<nCount;i++)
	{
		char sText[50];
		CString sPartNo;
		m_pListCtrl->GetItemText(i,0,sText,50);
		sPartNo.Format("%s",sText);
		if(m_sSetupAidedPlatePartNo.CompareNoCase(sPartNo)==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("The same number aided plate has already existed，can't repeat to add!");
#else
			AfxMessageBox("已存在相同编号的辅助钢板，不能重复添加!");
#endif
			return;
		}
	}
	tempStr.Format("%s",m_sSetupAidedPlatePartNo);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fRadius);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fWidth);
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
	AppendRecord(-1,str_arr);
}

void CSetupAidedPlateDlg::OnModifySetupAidedPlate() 
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
				char sText[50];
				CString sPartNo;
				m_pListCtrl->GetItemText(i,0,sText,50);
				sPartNo.Format("%s",sText);
				if(m_sSetupAidedPlatePartNo.CompareNoCase(sPartNo)==0)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("The same number aided plate has already existed，can't repeat to add!");
#else
					AfxMessageBox("已存在相同编号的辅助钢板，不能重复添加!");
#endif
					return;
				}
			}
			//更新工装辅助钢板列表
			CString ss;
			m_pListCtrl->SetItemText(iCurSel,0,m_sSetupAidedPlatePartNo);
			ss.Format("%f",m_fRadius);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,1,ss);
			ss.Format("%f",m_fWidth);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,2,ss);
			ss.Format("%f",m_fAngle);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,3,ss);
			ss.Format("%f",m_fStartPlateHalfT);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,4,ss);
			ss.Format("%f",m_fEndPlateHalfT);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,5,ss);
			if(m_iProfileStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
				ss="straight line";
			else
				ss="arc";
#else
				ss="直线";
			else
				ss="圆弧";
#endif
			m_pListCtrl->SetItemText(iCurSel,6,ss);
		}
	}
}


void CSetupAidedPlateDlg::OnBrowseFile() 
{	//指定工装辅助钢板库文件路径
	UpdateData();
	CFileDialog dlg(TRUE,"sap","AidedPlate",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"setup aided plate(*.sap)|*.sap||");
#else
		"工装辅助钢板(*.sap)|*.sap||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	m_sAidedPlateFilePath = dlg.GetPathName();
	//导入指定的工装辅助钢板库文件
	SetupAidedPlateSerialize(m_sAidedPlateFilePath,FALSE);
	UpdateData(FALSE);
}

void CSetupAidedPlateDlg::OnKillfocusAidedPlateFilePath() 
{
	UpdateData();
	//导入指定的工装辅助钢板库文件
	SetupAidedPlateSerialize(m_sAidedPlateFilePath,FALSE);
}
