//<LOCALE_TRANSLATE/>
// PlankPartParaExportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PlankPartParaExportDlg.h"
#include "Buffer.h"
#include "SysPara.h"
#include "RxTools.h"
#include "MenuFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlankPartParaExportDlg dialog
BOOL CPlankPartParaExportDlg::m_bMarkVertex=FALSE;
BOOL CPlankPartParaExportDlg::m_bMartWeld=FALSE;
CString CPlankPartParaExportDlg::m_sExportPartInfoKeyStr;
ATOM_LIST<CPlankPartParaExportDlg::TABLE_ROW> CPlankPartParaExportDlg::m_xTblRowList;

CPlankPartParaExportDlg::CPlankPartParaExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlankPartParaExportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlankPartParaExportDlg)
	m_bPartMat = FALSE;
	m_bPartNo = FALSE;
	m_bPieceWeight = FALSE;
	m_bPlankThick = FALSE;
	m_bSumWeight = FALSE;
	m_bTaType = FALSE;
	m_bPartSize = FALSE;
	m_bVertexMark = FALSE;
	m_bWeldMark = FALSE;
	m_bPrjName = FALSE;
	m_bTowerTypeAlias = FALSE;
	m_bLsNum = FALSE;
	m_sEditLine = _T("");
	m_bStampNo = FALSE;
	m_bPartNum = FALSE;
	m_bPieceArea = FALSE;
	m_bTaNum = FALSE;
	m_bEdgeNum = FALSE;
	m_bWeldLength = FALSE;
	m_bTrialAssembly = FALSE;
	m_bTolerance = FALSE;
	m_bTaskNo = FALSE;
	m_bSumPartNum = FALSE;
	m_bWeldPart = FALSE;
	m_bHuoQu = FALSE;
	m_bPromptEveryTime=sys.part_map.plate.bShowPlateCfgInfoDlg;
	//}}AFX_DATA_INIT
}


void CPlankPartParaExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlankPartParaExportDlg)
	DDX_Check(pDX, IDC_CHK_PART_MAT, m_bPartMat);
	DDX_Check(pDX, IDC_CHK_PART_NO, m_bPartNo);
	DDX_Check(pDX, IDC_CHK_PIECE_WEIGHT, m_bPieceWeight);
	DDX_Check(pDX, IDC_CHK_PLANK_THICK, m_bPlankThick);
	DDX_Check(pDX, IDC_CHK_SUM_WEIGHT, m_bSumWeight);
	DDX_Check(pDX, IDC_CHK_TA_TYPE, m_bTaType);
	DDX_Check(pDX, IDC_CHK_PART_SIZE, m_bPartSize);
	DDX_Check(pDX, IDC_CHK_VERTEX_MARK, m_bVertexMark);
	DDX_Check(pDX, IDC_CHK_WELD_MARK, m_bWeldMark);
	DDX_Check(pDX, IDC_CHK_PRJ_NAME, m_bPrjName);
	DDX_Check(pDX, IDC_CHK_TOWER_ALIAS, m_bTowerTypeAlias);
	DDX_Check(pDX, IDC_CHK_LS_NUM, m_bLsNum);
	DDX_Text(pDX, IDC_E_EDIT_LINE, m_sEditLine);
	DDV_MaxChars(pDX, m_sEditLine, 200);
	DDX_Check(pDX, IDC_CHK_STAMP_NO, m_bStampNo);
	DDX_Check(pDX, IDC_CHK_PART_NUM, m_bPartNum);
	DDX_Check(pDX, IDC_CHK_PIECE_AREA, m_bPieceArea);
	DDX_Check(pDX, IDC_CHK_TA_NUM, m_bTaNum);
	DDX_Check(pDX, IDC_CHE_EDGE_NUM, m_bEdgeNum);
	DDX_Check(pDX, IDC_CHK_WELD_LENGTH ,m_bWeldLength);
#ifndef AFX_TARG_ENU_ENGLISH
	DDX_Check(pDX, IDC_CHK_TRIAL_ASSEMBLY, m_bTrialAssembly);
	DDX_Check(pDX, IDC_CHK_TOLERANCE, m_bTolerance);
	DDX_Check(pDX, IDC_CHK_TASK_NO, m_bTaskNo);
	DDX_Check(pDX, IDC_CHK_SUM_PART_NUM, m_bSumPartNum);
	DDX_Check(pDX, IDC_CHK_WELD_PART, m_bWeldPart);
	DDX_Check(pDX, IDC_CHK_HUOQU, m_bHuoQu);
	DDX_Check(pDX, IDC_CHK_SUPPORT_TABLE, m_bSupportTable);
	DDX_Check(pDX, IDC_CHK_SAVE_CONFIG_FILE, m_bPromptEveryTime);
	DDX_Control(pDX, IDC_LIST_TBL_ROW, m_listCtrTblRow);
#endif
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlankPartParaExportDlg, CDialog)
	//{{AFX_MSG_MAP(CPlankPartParaExportDlg)
	ON_BN_CLICKED(IDC_BN_ADD_EDIT_LINE, OnBnAddEditLine)
	ON_BN_CLICKED(IDC_BN_DEL_CURRENT_LINE, OnBnDelCurrentLine)
	ON_BN_CLICKED(IDC_BN_CLEAR_EDIT_LINE, OnBnClearEditLine)
	ON_BN_CLICKED(IDC_CHK_PRJ_NAME, OnChkPrjName)
	ON_BN_CLICKED(IDC_CHK_TOWER_ALIAS, OnChkPrjCode)
	ON_BN_CLICKED(IDC_CHK_TA_TYPE, OnChkTaType)
	ON_BN_CLICKED(IDC_CHK_PART_NO, OnChkPartNo)
	ON_BN_CLICKED(IDC_CHK_PART_MAT, OnChkPartMat)
	ON_BN_CLICKED(IDC_CHK_PLANK_THICK, OnChkPlankThick)
	ON_BN_CLICKED(IDC_CHK_PART_SIZE, OnChkPartSize)
	ON_BN_CLICKED(IDC_CHK_PIECE_WEIGHT, OnChkPieceWeight)
	ON_BN_CLICKED(IDC_CHK_SUM_WEIGHT, OnChkSumWeight)
	ON_BN_CLICKED(IDC_CHK_LS_NUM, OnChkLsNum)
	ON_BN_CLICKED(IDC_CHK_STAMP_NO, OnChkStampNo)
	ON_BN_CLICKED(IDC_CHK_PART_NUM, OnChkPartNum)
	ON_BN_CLICKED(IDC_CHK_PIECE_AREA, OnChkPieceArea)
	ON_BN_CLICKED(IDC_CHK_TA_NUM, OnChkTaNum)
	ON_BN_CLICKED(IDC_CHE_EDGE_NUM,OnChkEdgeNum)
	ON_BN_CLICKED(IDC_CHK_WELD_LENGTH,OnChkWeldLength)
#ifndef AFX_TARG_ENU_ENGLISH
	ON_BN_CLICKED(IDC_CHK_TRIAL_ASSEMBLY, OnChkTrialAssembly)
	ON_BN_CLICKED(IDC_CHK_TOLERANCE, OnChkTolerance)
	ON_BN_CLICKED(IDC_CHK_TASK_NO, OnChkTaskNo)
	ON_BN_CLICKED(IDC_CHK_SUM_PART_NUM, OnChkSumPartNum)
	ON_BN_CLICKED(IDC_CHK_WELD_PART, OnChkWeldPart)
	ON_BN_CLICKED(IDC_CHK_HUOQU, OnChkHuoQu)
	ON_BN_CLICKED(IDC_CHK_SUPPORT_TABLE, OnChkSupportTable)
	ON_BN_CLICKED(IDC_BN_ADD_ROW, OnBnAddRow)
	ON_BN_CLICKED(IDC_BN_DEL_ROW, OnBnDelRow)
	ON_BN_CLICKED(IDC_BN_MOVE_UP, OnBnMoveUp)
	ON_BN_CLICKED(IDC_BN_MOVE_DOWN, OnBnMoveDown)
#endif
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlankPartParaExportDlg message handlers

void CPlankPartParaExportDlg::UpdateCheckStateByName(const char *sName,BOOL bValue)
{
	if(sName==NULL||strlen(sName)<=0)
		return;
#ifdef AFX_TARG_ENU_ENGLISH
	if(stricmp(sName,"Project Name")==0)
		m_bPrjName=bValue;
	else if(stricmp(sName,"tower type alias")==0)
		m_bTowerTypeAlias=bValue;
	else if(stricmp(sName,"material")==0)
		m_bPartMat=bValue;
	else if(stricmp(sName,"part number")==0)
		m_bPartNo=bValue;
	else if(stricmp(sName,"part amount")==0)
		m_bPartNum=bValue;
	else if(stricmp(sName,"thickness")==0)
		m_bPlankThick=bValue;
	else if(stricmp(sName,"piece weight")==0)
		m_bPieceWeight=bValue;
	else if(stricmp(sName,"sum weight")==0)
		m_bSumWeight=bValue;
	else if(stricmp(sName,"tower type")==0)
		m_bTaType=bValue;
	else if(stricmp(sName,"size")==0)
		m_bPartSize=bValue;
	else if(stricmp(sName,"area")==0)
		m_bPieceArea=bValue;
	else if(stricmp(sName,"bolt amount")==0)
		m_bLsNum=bValue;
	else if(stricmp(sName,"stamp number")==0)
		m_bStampNo=bValue;
	else if(stricmp(sName,"basic number")==0)
		m_bTaNum=bValue;
	else if(stricmp(sName,"edge number")==0)
		m_bEdgeNum=bValue;
	else if(stricmp(sName,"the length of weld seam")==0)
		m_bWeldLength =bValue;
#else
	if(stricmp(sName,"工程名称")==0)
		m_bPrjName=bValue;
	else if(stricmp(sName,"塔型代号")==0)
		m_bTowerTypeAlias=bValue;
	else if(stricmp(sName,"材质")==0)
		m_bPartMat=bValue;
	else if(stricmp(sName,"件号")==0)
		m_bPartNo=bValue;
	else if(stricmp(sName,"件数")==0)
		m_bPartNum=bValue;
	else if(stricmp(sName,"厚度")==0)
		m_bPlankThick=bValue;
	else if(stricmp(sName,"单重")==0)
		m_bPieceWeight=bValue;
	else if(stricmp(sName,"总重")==0)
		m_bSumWeight=bValue;
	else if(stricmp(sName,"塔型")==0)
		m_bTaType=bValue;
	else if(stricmp(sName,"规格")==0)
		m_bPartSize=bValue;
	else if(stricmp(sName,"面积")==0)
		m_bPieceArea=bValue;
	else if(stricmp(sName,"螺栓数")==0)
		m_bLsNum=bValue;
	else if(stricmp(sName,"钢印号")==0)
		m_bStampNo=bValue;
	else if(stricmp(sName,"基数")==0)
		m_bTaNum=bValue;
	else if(stricmp(sName,"边数")==0)
		m_bEdgeNum=bValue;
	else if(stricmp(sName,"焊缝边长")==0)
		m_bWeldLength =bValue;
	else if(stricmp(sName,"试组装")==0)
		m_bTrialAssembly=bValue;
	else if(stricmp(sName,"公差")==0)
		m_bTolerance=bValue;
	else if(stricmp(sName,"任务单号")==0)
		m_bTaskNo=bValue;
	else if(stricmp(sName,"总件数")==0)
		m_bSumPartNum=bValue;
	else if(stricmp(sName,"焊接")==0)
		m_bWeldPart=bValue;
	else if(stricmp(sName,"制弯")==0)
		m_bHuoQu=bValue;
#endif
	UpdateData(FALSE);
}
void CPlankPartParaExportDlg::UpdateCheckBoxEnableState()
{
	GetDlgItem(IDC_CHK_PART_MAT)->EnableWindow(!m_bPartMat);
	GetDlgItem(IDC_CHK_PART_NO)->EnableWindow(!m_bPartNo);
	GetDlgItem(IDC_CHK_PART_NUM)->EnableWindow(!m_bPartNum);
	GetDlgItem(IDC_CHK_PIECE_AREA)->EnableWindow(!m_bPieceArea);
	GetDlgItem(IDC_CHK_PIECE_WEIGHT)->EnableWindow(!m_bPieceWeight);
	GetDlgItem(IDC_CHK_PLANK_THICK)->EnableWindow(!m_bPlankThick);
	GetDlgItem(IDC_CHK_SUM_WEIGHT)->EnableWindow(!m_bSumWeight);
	GetDlgItem(IDC_CHK_TA_TYPE)->EnableWindow(!m_bTaType);
	GetDlgItem(IDC_CHK_PART_SIZE)->EnableWindow(!m_bPartSize);
	GetDlgItem(IDC_CHK_PRJ_NAME)->EnableWindow(!m_bPrjName);
	GetDlgItem(IDC_CHK_TOWER_ALIAS)->EnableWindow(!m_bTowerTypeAlias);
	GetDlgItem(IDC_CHK_LS_NUM)->EnableWindow(!m_bLsNum);
	GetDlgItem(IDC_CHK_STAMP_NO)->EnableWindow(!m_bStampNo);
	GetDlgItem(IDC_CHK_TA_NUM)->EnableWindow(!m_bTaNum);
	GetDlgItem(IDC_CHE_EDGE_NUM)->EnableWindow(!m_bEdgeNum);
	GetDlgItem(IDC_CHK_WELD_LENGTH)->EnableWindow(!m_bWeldLength);
#ifndef AFX_TARG_ENU_ENGLISH
	GetDlgItem(IDC_CHK_TRIAL_ASSEMBLY)->EnableWindow(!m_bTrialAssembly);
	GetDlgItem(IDC_CHK_TOLERANCE)->EnableWindow(!m_bTolerance);
	GetDlgItem(IDC_CHK_TASK_NO)->EnableWindow(!m_bTaskNo);
	GetDlgItem(IDC_CHK_SUM_PART_NUM)->EnableWindow(!m_bSumPartNum);
	GetDlgItem(IDC_CHK_WELD_PART)->EnableWindow(!m_bWeldPart);
	GetDlgItem(IDC_CHK_HUOQU)->EnableWindow(!m_bHuoQu);
#endif
}

static int FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem,CString& sTextValue)
{
#ifndef AFX_TARG_ENU_ENGLISH
	CPlankPartParaExportDlg::TABLE_ROW *pRow=(CPlankPartParaExportDlg::TABLE_ROW*)pSelItem->m_idProp;
	if(pRow&&iSubItem==0)
		pRow->sDisplayName=sTextValue;
	return TRUE;
#else
	return FALSE;
#endif
}

BOOL CPlankPartParaExportDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_bPartMat = m_bPartNo = m_bPartNum = m_bPieceArea = m_bPieceWeight = m_bPlankThick = m_bSumWeight = FALSE;
	m_bTaType = m_bPartSize = m_bPrjName = m_bTowerTypeAlias = m_bLsNum = m_bStampNo = m_bTaNum = m_bEdgeNum =m_bWeldLength = FALSE;
	m_bTrialAssembly = m_bTolerance = m_bTaskNo = FALSE;
	m_sEditLine="";
	m_bWeldMark=m_bMartWeld;
#ifndef AFX_TARG_ENU_ENGLISH
	m_listCtrTblRow.EmptyColumnHeader();
	m_listCtrTblRow.AddColumnHeader("显示名称");
	m_listCtrTblRow.AddColumnHeader("属性");
	long col_wide_arr[2]={75,55};
	m_listCtrTblRow.InitListCtrl(col_wide_arr);
	m_listCtrTblRow.SetModifyValueFunc(FireValueModify);
#endif
	if(sys.part_map.plate.bUseDimStyleConfigFile)
	{
		LoadDimStyleFromFile(sys.part_map.plate.sPlateConfigFilePath);
#ifndef AFX_TARG_ENU_ENGLISH
		m_bSupportTable=m_xTblRowList.GetNodeNum()>0;
#endif
	}
	char *token,key_str[100];
	sprintf(key_str,"%s",m_sExportPartInfoKeyStr);
	CListBox *pListBox=(CListBox*)GetDlgItem(IDC_LIST_EXPORT_PART_INFO);
	for(token=strtok(key_str,"\n");token;token=strtok(NULL,"\n"))
		pListBox->AddString(token);
	for(int i=0;i<pListBox->GetCount();i++)
	{
		pListBox->GetText(i,key_str);
		for(token=strtok(key_str,"&");token;token=strtok(NULL,"&"))
		{
			UpdateCheckStateByName(token,TRUE);
		}
	}
#ifndef AFX_TARG_ENU_ENGLISH
	for(TABLE_ROW *pRow=m_xTblRowList.GetFirst();pRow;pRow=m_xTblRowList.GetNext())
	{
		CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
		lpInfo->AddSubItemText(pRow->sDisplayName);
		lpInfo->AddSubItemText(pRow->sRowName,TRUE);
		CSuperGridCtrl::CTreeItem *pItem=m_listCtrTblRow.InsertRootItem(lpInfo);
		pItem->m_idProp=(DWORD)pRow;
		UpdateCheckStateByName(pRow->sRowName,TRUE);
	}
	GetDlgItem(IDC_CHK_TRIAL_ASSEMBLY)->EnableWindow(!m_bTrialAssembly);
	GetDlgItem(IDC_CHK_TOLERANCE)->EnableWindow(!m_bTolerance);
	GetDlgItem(IDC_CHK_TASK_NO)->EnableWindow(!m_bTaskNo);
	if(sys.part_map.plate.bDisplaypPartInfoTbl)
		GetDlgItem(IDC_CHK_SUPPORT_TABLE)->ShowWindow(SW_SHOW);
	else 
	{
		GetDlgItem(IDC_CHK_SUPPORT_TABLE)->ShowWindow(SW_HIDE);
		m_bSupportTable=FALSE;
	}
#endif
	UpdateCheckBoxEnableState();
	UpdateData(FALSE);
#ifndef AFX_TARG_ENU_ENGLISH
	OnChkSupportTable();
#endif
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPlankPartParaExportDlg::OnBnAddEditLine() 
{
	UpdateData();
	if(m_sEditLine.GetLength()>0)
	{
		CListBox *pListBox=(CListBox*)GetDlgItem(IDC_LIST_EXPORT_PART_INFO);
		pListBox->AddString(m_sEditLine);
		m_sEditLine="";
		UpdateData(FALSE);
	}
}

void CPlankPartParaExportDlg::OnBnDelCurrentLine() 
{
	CListBox *pListBox=(CListBox*)GetDlgItem(IDC_LIST_EXPORT_PART_INFO);
	if(pListBox->GetCurSel()>=0)
	{
		CString ss;
		pListBox->GetText(pListBox->GetCurSel(),ss);
		char key_str[200];
		strcpy(key_str,ss);
		for(char *token=strtok(key_str,"&");token;token=strtok(NULL,"&"))
		{
			UpdateCheckStateByName(token,FALSE);
		}
		UpdateData(FALSE);
		OnBnClearEditLine();
		pListBox->DeleteString(pListBox->GetCurSel());
	}
}

void CPlankPartParaExportDlg::OnBnClearEditLine() 
{
	m_sEditLine="";
	UpdateCheckBoxEnableState();
	UpdateData(FALSE);
}

void CPlankPartParaExportDlg::OnOK() 
{
	CListBox *pListBox=(CListBox*)GetDlgItem(IDC_LIST_EXPORT_PART_INFO);
	m_sExportPartInfoKeyStr="";
	for(int i=0;i<pListBox->GetCount();i++)
	{
		CString ss;
		pListBox->GetText(i,ss);
		if(i==0)
			m_sExportPartInfoKeyStr=ss;
		else
			m_sExportPartInfoKeyStr+="\n"+ss;
	}
#ifndef AFX_TARG_ENU_ENGLISH
	UpdateData();
	m_bMarkVertex=m_bVertexMark;
	m_bMartWeld=m_bWeldMark;
	sys.part_map.plate.bShowPlateCfgInfoDlg=m_bPromptEveryTime;
	sys.part_map.plate.bUseDimStyleConfigFile=SaveDimStyleToFile(sys.part_map.plate.sPlateConfigFilePath);
#endif
	CDialog::OnOK();
}

void CPlankPartParaExportDlg::OnChkPrjName() 
{
	GetDlgItem(IDC_CHK_PRJ_NAME)->EnableWindow(FALSE);
	if(this->m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&Project Name";
	else
		m_sEditLine="Project Name";
#else
		m_sEditLine+="&工程名称";
	else
		m_sEditLine="工程名称";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}

void CPlankPartParaExportDlg::OnChkPrjCode() 
{
	GetDlgItem(IDC_CHK_TOWER_ALIAS)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&tower type alias";
	else
		m_sEditLine="tower type alias";
#else
		m_sEditLine+="&塔型代号";
	else
		m_sEditLine="塔型代号";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}

void CPlankPartParaExportDlg::OnChkTaType() 
{
	GetDlgItem(IDC_CHK_TA_TYPE)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&tower type";
	else
		m_sEditLine="tower type";
#else
		m_sEditLine+="&塔型";
	else
		m_sEditLine="塔型";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}

void CPlankPartParaExportDlg::OnChkPartNo() 
{
	GetDlgItem(IDC_CHK_PART_NO)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&part number";
	else
		m_sEditLine="part number";
#else
		m_sEditLine+="&件号";
	else
		m_sEditLine="件号";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}

void CPlankPartParaExportDlg::OnChkPartMat() 
{
	GetDlgItem(IDC_CHK_PART_MAT)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&material";
	else
		m_sEditLine="material";
#else
		m_sEditLine+="&材质";
	else
		m_sEditLine="材质";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}

void CPlankPartParaExportDlg::OnChkPlankThick() 
{
	GetDlgItem(IDC_CHK_PLANK_THICK)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&thickness";
	else
		m_sEditLine="thickness";
#else
		m_sEditLine+="&厚度";
	else
		m_sEditLine="厚度";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}

void CPlankPartParaExportDlg::OnChkPartSize() 
{
	GetDlgItem(IDC_CHK_PART_SIZE)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&spec";
	else
		m_sEditLine="spec";
#else
		m_sEditLine+="&规格";
	else
		m_sEditLine="规格";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}

void CPlankPartParaExportDlg::OnChkPieceArea() 
{
	GetDlgItem(IDC_CHK_PIECE_AREA)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&area";
	else
		m_sEditLine="area";
#else
		m_sEditLine+="&面积";
	else
		m_sEditLine="面积";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}

void CPlankPartParaExportDlg::OnChkPieceWeight() 
{
	GetDlgItem(IDC_CHK_PIECE_WEIGHT)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&piece weight";
	else
		m_sEditLine="piece weight";
#else
		m_sEditLine+="&单重";
	else
		m_sEditLine="单重";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}

void CPlankPartParaExportDlg::OnChkSumWeight() 
{
	GetDlgItem(IDC_CHK_SUM_WEIGHT)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&sum weight";
	else
		m_sEditLine="sum weight";
#else
		m_sEditLine+="&总重";
	else
		m_sEditLine="总重";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}

void CPlankPartParaExportDlg::OnChkLsNum() 
{
	GetDlgItem(IDC_CHK_LS_NUM)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&bolt amount";
	else
		m_sEditLine="bolt amount";
#else
		m_sEditLine+="&螺栓数";
	else
		m_sEditLine="螺栓数";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}

void CPlankPartParaExportDlg::OnChkStampNo() 
{
	GetDlgItem(IDC_CHK_STAMP_NO)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&stamp number";
	else
		m_sEditLine="stamp number";
#else
		m_sEditLine+="&钢印号";
	else
		m_sEditLine="钢印号";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}

void CPlankPartParaExportDlg::OnChkPartNum() 
{
	GetDlgItem(IDC_CHK_PART_NUM)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&part amount";
	else
		m_sEditLine="part amount";
#else
		m_sEditLine+="&件数";
	else
		m_sEditLine="件数";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}

void CPlankPartParaExportDlg::OnChkTaNum() 
{
	GetDlgItem(IDC_CHK_TA_NUM)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&basic number";
	else
		m_sEditLine="basic number";
#else
		m_sEditLine+="&基数";
	else
		m_sEditLine="基数";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}
void CPlankPartParaExportDlg::OnChkEdgeNum()
{
	GetDlgItem(IDC_CHE_EDGE_NUM)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&edge number";
	else
		m_sEditLine="edge number";
#else
		m_sEditLine+="&边数";
	else
		m_sEditLine="边数";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}
void CPlankPartParaExportDlg::OnChkWeldLength()
{
	GetDlgItem(IDC_CHK_WELD_LENGTH)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sEditLine+="&the length of weld seam";
	else
		m_sEditLine="the length of weld seam";
#else
		m_sEditLine+="&焊缝边长";
	else
		m_sEditLine="焊缝边长";
#endif
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}
#ifndef AFX_TARG_ENU_ENGLISH
void CPlankPartParaExportDlg::OnChkTrialAssembly()
{
	GetDlgItem(IDC_CHK_TRIAL_ASSEMBLY)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
		m_sEditLine+="&试组装";
	else
		m_sEditLine="试组装";
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}
void CPlankPartParaExportDlg::OnChkTolerance()
{
	GetDlgItem(IDC_CHK_TOLERANCE)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
		m_sEditLine+="&公差";
	else
		m_sEditLine="公差";
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}
void CPlankPartParaExportDlg::OnChkTaskNo()
{
	GetDlgItem(IDC_CHK_TASK_NO)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
		m_sEditLine+="&任务单号";
	else
		m_sEditLine="任务单号";
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}
void CPlankPartParaExportDlg::OnChkSumPartNum()
{
	GetDlgItem(IDC_CHK_SUM_PART_NUM)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
		m_sEditLine+="&总件数";
	else
		m_sEditLine="总件数";
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}
void CPlankPartParaExportDlg::OnChkWeldPart()
{
	GetDlgItem(IDC_CHK_WELD_PART)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
		m_sEditLine+="&焊接";
	else
		m_sEditLine="焊接";
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}
void CPlankPartParaExportDlg::OnChkHuoQu()
{
	GetDlgItem(IDC_CHK_HUOQU)->EnableWindow(FALSE);
	if(m_sEditLine.GetLength()>0)
		m_sEditLine+="&制弯";
	else
		m_sEditLine="制弯";
	GetDlgItem(IDC_E_EDIT_LINE)->SetWindowText(m_sEditLine);
}
void CPlankPartParaExportDlg::OnChkSupportTable()
{
	UpdateData();
	CRect rect,btnRect;
	GetWindowRect(rect);
	if(m_bSupportTable)
		rect.right=rect.left+485;
	else
		rect.right=rect.left+321;
	MoveWindow(rect);
	GetDlgItem(IDOK)->GetWindowRect(btnRect);
	ScreenToClient(btnRect);
	int nBtnWidth=btnRect.Width();
	btnRect.left=(int)((rect.Width()-nBtnWidth)*0.6);
	btnRect.right=btnRect.left+nBtnWidth;
	GetDlgItem(IDOK)->MoveWindow(btnRect);
	//
	if(!m_bSupportTable)
	{
		for(TABLE_ROW *pRow=m_xTblRowList.GetFirst();pRow;pRow=m_xTblRowList.GetNext())
			UpdateCheckStateByName(pRow->sRowName,FALSE);
		m_xTblRowList.Empty();
		m_listCtrTblRow.DeleteAllItems();
		UpdateCheckBoxEnableState();
	}
}
void CPlankPartParaExportDlg::OnBnAddRow()
{
	if(!m_bSupportTable)
		return;
	UpdateData();
	char *token,key_str[100];
	sprintf(key_str,"%s",m_sEditLine);
	for(token=strtok(key_str,"&");token;token=strtok(NULL,"&"))
	{
		TABLE_ROW *pRow=m_xTblRowList.append();
		pRow->sRowName.Copy(token);
		if(pRow->sRowName.EqualNoCase("件号"))
			pRow->sDisplayName.Copy("零部件号");
		else if(pRow->sRowName.EqualNoCase("件数"))
			pRow->sDisplayName.Copy("单段数量");
		else if(pRow->sRowName.EqualNoCase("总件数"))
			pRow->sDisplayName.Copy("加工数量");
		else if(pRow->sRowName.EqualNoCase("螺栓数"))
			pRow->sDisplayName.Copy("孔数");
		else if(pRow->sRowName.EqualNoCase("焊接"))
			pRow->sDisplayName.Copy("点焊");
		else if(pRow->sRowName.EqualNoCase("制弯"))
			pRow->sDisplayName.Copy("弯曲");
		else 
			pRow->sDisplayName.Copy(token);
		CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
		lpInfo->AddSubItemText(pRow->sDisplayName);
		lpInfo->AddSubItemText(pRow->sRowName,TRUE);
		CSuperGridCtrl::CTreeItem *pItem=m_listCtrTblRow.InsertRootItem(lpInfo);
		pItem->m_idProp=(DWORD)pRow;
	}
	m_sEditLine="";
	UpdateData(FALSE);
}
void CPlankPartParaExportDlg::OnBnDelRow()
{
	if(!m_bSupportTable)
		return;
	int iCurSel=m_listCtrTblRow.GetSelectedItem();
	CSuperGridCtrl::CTreeItem *pCurItem=m_listCtrTblRow.GetTreeItem(iCurSel);
	if(pCurItem)
	{
		TABLE_ROW *pCurRow=(TABLE_ROW*)pCurItem->m_idProp;
		UpdateCheckStateByName(pCurRow->sRowName,FALSE);
		for(TABLE_ROW *pRow=m_xTblRowList.GetFirst();pRow;pRow=m_xTblRowList.GetNext())
		{
			if(pRow==pCurRow)
			{
				m_xTblRowList.DeleteCursor(TRUE);
				break;
			}
		}
		m_listCtrTblRow.DeleteItem(iCurSel);
		CSuperGridCtrl::CTreeItem *pNextItem=NULL;
		if(iCurSel<m_listCtrTblRow.GetItemCount()-1)
			pNextItem=m_listCtrTblRow.GetTreeItem(iCurSel+1);
		else if(iCurSel>0)
			pNextItem=m_listCtrTblRow.GetTreeItem(iCurSel-1);
		if(pNextItem)
			m_listCtrTblRow.SelectItem(pNextItem);
		OnBnClearEditLine();
		UpdateData(FALSE);
	}
}
void SwapTreeItem(CSuperGridCtrl *pListCtrl,CSuperGridCtrl::CTreeItem *pItem1,CSuperGridCtrl::CTreeItem *pItem2)
{
	CPlankPartParaExportDlg::TABLE_ROW *pRow1=(CPlankPartParaExportDlg::TABLE_ROW*)pItem1->m_idProp;
	CPlankPartParaExportDlg::TABLE_ROW *pRow2=(CPlankPartParaExportDlg::TABLE_ROW*)pItem2->m_idProp;
	pItem1->m_idProp=(DWORD)pRow2;
	pListCtrl->SetSubItemText(pItem1,0,pRow2->sDisplayName);
	pListCtrl->SetSubItemText(pItem1,1,pRow2->sRowName);
	//
	pItem2->m_idProp=(DWORD)pRow1;
	pListCtrl->SetSubItemText(pItem2,0,pRow1->sDisplayName);
	pListCtrl->SetSubItemText(pItem2,1,pRow1->sRowName);
}
void CPlankPartParaExportDlg::OnBnMoveUp()
{
	if(!m_bSupportTable)
		return;
	int iCurSel=m_listCtrTblRow.GetSelectedItem();
	if(iCurSel>0)
	{
		CSuperGridCtrl::CTreeItem *pPrevItem=m_listCtrTblRow.GetTreeItem(iCurSel-1);
		SwapTreeItem(&m_listCtrTblRow,m_listCtrTblRow.GetTreeItem(iCurSel),pPrevItem);
		m_listCtrTblRow.SetItemState(-1,0,LVIS_SELECTED|LVIS_FOCUSED);
		m_listCtrTblRow.SelectItem(pPrevItem);
	}
}
void CPlankPartParaExportDlg::OnBnMoveDown()
{
	if(!m_bSupportTable)
		return;
	int iCurSel=m_listCtrTblRow.GetSelectedItem();
	if(iCurSel<m_listCtrTblRow.GetItemCount()-1)
	{
		CSuperGridCtrl::CTreeItem *pNextItem=m_listCtrTblRow.GetTreeItem(iCurSel+1);
		SwapTreeItem(&m_listCtrTblRow,m_listCtrTblRow.GetTreeItem(iCurSel),pNextItem);
		m_listCtrTblRow.SetItemState(-1,0,LVIS_SELECTED|LVIS_FOCUSED);
		m_listCtrTblRow.SelectItem(pNextItem);
	}
}
#endif

BOOL CPlankPartParaExportDlg::SaveDimStyleToFile(const char* file_path)
{
	CBuffer buffer;
	char *token,key_str[100];
	sprintf(key_str,"%s",m_sExportPartInfoKeyStr);
	BUFFERPOP stack(&buffer,0);
	buffer.WriteInteger(0);
	for(token=strtok(key_str,"\n");token;token=strtok(NULL,"\n"))
	{
		buffer.WriteString(token);
		stack.Increment();
	}
	stack.VerifyAndRestore();
	//
	stack.Initialize(&buffer,m_xTblRowList.GetNodeNum());
	buffer.WriteInteger(m_xTblRowList.GetNodeNum());
	for(TABLE_ROW *pRow=m_xTblRowList.GetFirst();pRow;pRow=m_xTblRowList.GetNext())
	{
		buffer.WriteString(pRow->sDisplayName);
		buffer.WriteString(pRow->sRowName);
		stack.Increment();
	}
	stack.VerifyAndRestore();
	//
	buffer.WriteInteger(m_bMartWeld);
	buffer.WriteInteger(m_bMarkVertex);
	//
	FILE *fp=fopen(file_path,"wb");
	if(fp==NULL)
		return FALSE;
	long version=2;
	long buf_size=buffer.GetLength();
	fwrite(&version,4,1,fp);
	fwrite(&buf_size,4,1,fp);
	fwrite(buffer.GetBufferPtr(),buf_size,1,fp);
	fclose(fp);
	return TRUE;
}
BOOL CPlankPartParaExportDlg::LoadDimStyleFromFile(const char* file_path)
{
	FILE *fp=fopen(file_path,"rb");
	if(fp==NULL)
		return FALSE;
	long version=0,buf_size=0;
	fread(&version,4,1,fp);
	fread(&buf_size,4,1,fp);
	CBuffer buffer;
	buffer.Write(NULL,buf_size);
	fread(buffer.GetBufferPtr(),buf_size,1,fp);
	fclose(fp);
	//
	m_sExportPartInfoKeyStr.Empty();
	buffer.SeekToBegin();
	CXhChar200 sRow;
	int n=buffer.ReadInteger();
	for(int i=0;i<n;i++)
	{
		buffer.ReadString(sRow);
		if(m_sExportPartInfoKeyStr.GetLength()>0)
			m_sExportPartInfoKeyStr+="\n";
		m_sExportPartInfoKeyStr.Append(sRow);
	}
	//
	m_xTblRowList.Empty();
	n=buffer.ReadInteger();
	for(int i=0;i<n;i++)
	{
		TABLE_ROW *pRow=m_xTblRowList.append();
		buffer.ReadString(pRow->sDisplayName);
		buffer.ReadString(pRow->sRowName);
	}
	//
	if(version>=2)
	{
		buffer.ReadInteger(&m_bMartWeld);
		buffer.ReadInteger(&m_bMarkVertex);
	}
	return TRUE;
}