//<LOCALE_TRANSLATE Confirm by hxr/>
// TaskListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "OutputInfo.h"
#include "OutputInfoDlg.h"
#include "MainFrm.h"
#include "Variant.h"
#include "ExcelOper.h"
#include "PostProcessReport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputInfoDlg dialog

IMPLEMENT_DYNCREATE(COutputInfoDlg, CDialog)
COutputInfoDlg::COutputInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COutputInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COutputInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_listOutputInfoCtrl.AddColumnHeader("state");
	m_listOutputInfoCtrl.AddColumnHeader("type");
	m_listOutputInfoCtrl.AddColumnHeader("description");
#else
	m_listOutputInfoCtrl.AddColumnHeader("״̬");
	m_listOutputInfoCtrl.AddColumnHeader("����");
	m_listOutputInfoCtrl.AddColumnHeader("˵��");
#endif
}


void COutputInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COutputInfoDlg)
	DDX_Control(pDX, IDC_OUTPUT_INFO_LIST, m_listOutputInfoCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COutputInfoDlg, CDialog)
	//{{AFX_MSG_MAP(COutputInfoDlg)
	ON_WM_SIZE()
	ON_COMMAND(ID_RECHECK, OnRecheck)
	ON_COMMAND(ID_CORRECT_ERRORITEM, OnCorrectErrorItem)
	ON_COMMAND(ID_EXPORT_RESULT, OnExportToTextFile)
	ON_COMMAND(ID_EXPORT_EXCEL_FILE, OnExportToExcelFile)
	ON_COMMAND(ID_MARK_NORMAL,OnMarkNormal)
	ON_COMMAND(ID_CANCEL_MARK,OnCancelMark)
	ON_COMMAND(ID_AUTO_CUT_JG,OnAutoCutJg)
	ON_NOTIFY(NM_CLICK, IDC_OUTPUT_INFO_LIST, OnClickOutputInfoList)
	ON_NOTIFY(NM_DBLCLK, IDC_OUTPUT_INFO_LIST, OnDblclkOutputInfoList)
	ON_NOTIFY(NM_RCLICK, IDC_OUTPUT_INFO_LIST, OnRclickOutputInfoList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputInfoDlg message handlers

BOOL COutputInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	RelayoutWnd();
	
	m_listOutputInfoCtrl.DeleteAllItems();
	long col_wide_arr[3]={40,100,400};
	m_listOutputInfoCtrl.InitListCtrl(col_wide_arr);
	m_listOutputInfoCtrl.SetExtendedStyle(m_listOutputInfoCtrl.GetExtendedStyle()&~LVS_EX_HEADERDRAGDROP);
	return TRUE;
}

void COutputInfoDlg::SetInfoTitle(CString sTitle)
{
	m_sPaneTopTitle=sTitle;
}
void COutputInfoDlg::RelayoutWnd()
{
	RECT rcFull;
	GetClientRect(&rcFull);
	CWnd *pWnd = GetDlgItem(IDC_OUTPUT_INFO_LIST);
	if(pWnd)
		pWnd->MoveWindow(&rcFull);
}

void COutputInfoDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	RelayoutWnd();
	//���µڶ����п�
	CRect rect;
	GetClientRect(&rect);
	CListCtrl *pOutputInfoListCtrl = (CListCtrl*)GetDlgItem(IDC_OUTPUT_INFO_LIST);
	if(pOutputInfoListCtrl)
		pOutputInfoListCtrl->SetColumnWidth(2,rect.Width()-pOutputInfoListCtrl->GetColumnWidth(0)-30);
}

void COutputInfoDlg::ClearOutputInfoList()
{
	m_xOutputInfoList.Empty();
	m_listOutputInfoCtrl.DeleteAllItems();
#ifdef AFX_TARG_ENU_ENGLISH
	GetParent()->SetWindowText("Task list");
#else
	GetParent()->SetWindowText("�����б�");
#endif
	SetInfoTitle("");
	/*CWnd *pWnd=GetParent();
	if(pWnd->IsKindOf(RUNTIME_CLASS(CDockPageBar)))
	{	//����״̬ʱĬ�ϸ�������CMainFrame
		CDockPageBar *pDockWnd=(CDockPageBar*)pWnd;
		pDockWnd->SetTitle("�����б�",RUNTIME_CLASS(COutputInfoDlg));
	}*/
}

void COutputInfoDlg::InitOutputInfoList(CTaskOutputList &itemList)
{
	m_xOutputInfoList.Empty();
	m_listOutputInfoCtrl.DeleteAllItems();
	CHashList<COutputInfo*> hashSelObjId;
	for(IBasicOutput *pInfo=itemList.GetFirst();pInfo;pInfo=itemList.GetNext())
	{
		if(pInfo->m_hRelaObj1>0)
			hashSelObjId.SetValue(pInfo->m_hRelaObj1,(COutputInfo*)pInfo);
		if(pInfo->m_hRelaObj2>0)
			hashSelObjId.SetValue(pInfo->m_hRelaObj2,(COutputInfo*)pInfo);
		COutputInfo* pNewOutput=(COutputInfo*)m_xOutputInfoList.Add(0,pInfo->GetTypeId());
		pNewOutput->CopyFrom(pInfo);
	}
	int i=0;
	DYN_ARRAY<long> arrSelObjIds(hashSelObjId.GetNodeNum());
	for(COutputInfo** ppInfo=hashSelObjId.GetFirst();ppInfo;ppInfo=hashSelObjId.GetNext(),i++)
		arrSelObjIds[i]=hashSelObjId.GetCursorKey();
	g_pSolidDraw->SetEntSnapStatus((long*)arrSelObjIds,hashSelObjId.GetNodeNum());
	UpdateOutputInfoList();
}

void COutputInfoDlg::AddOutputInfo(IBasicOutput* pItemInfo)
{
	COutputInfo *pOutputInfo=m_xOutputInfoList.AppendSimpleOutput();
	pOutputInfo->m_hRelaObj1=pItemInfo->m_hRelaObj1;
	pOutputInfo->m_hRelaObj2=pItemInfo->m_hRelaObj2;
	pOutputInfo->m_sDescription=pItemInfo->szMessage;
}

void COutputInfoDlg::UpdateOutputInfoList()
{
	m_listOutputInfoCtrl.DeleteAllItems();
	CStringArray str_arr;
	str_arr.SetSize(3);
	for(IBasicOutput *pItem=m_xOutputInfoList.GetFirst();pItem;pItem=m_xOutputInfoList.GetNext())
	{
		if(((COutputInfo*)pItem)->SolvedState())
			str_arr[0] = "��";
		else if(markInfoList.GetValue(pItem->GetKeyStr())!=NULL)
			str_arr[0] = "��";
		else
			str_arr[0] = "";
		str_arr[1] = pItem->GetTypeName();
		str_arr[2] = pItem->szMessage;
		int iItem = m_listOutputInfoCtrl.InsertItemRecord(-1,str_arr);
		m_listOutputInfoCtrl.SetItemData(iItem,(DWORD)pItem);
	}
	m_listOutputInfoCtrl.AutoAdjustColumnWidth();
}

BOOL COutputInfoDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)   
	{   
		//if(pMsg->wParam==VK_ESCAPE)
		//	return FALSE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
void COutputInfoDlg::OnExportToTextFile()
{
	CLogFile logoutput(CXhChar200("%soutput.txt",theApp.szWorkPath));
	CLogErrorLife life(&logoutput);
	int i=1;
	POSITION pos = m_listOutputInfoCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = m_listOutputInfoCtrl.GetNextSelectedItem(pos);
		COutputInfo *pItemInfo = (COutputInfo*)m_listOutputInfoCtrl.GetItemData(iCurSel);
		CXhChar500 lineText("%4d#%s",i,pItemInfo->m_sDescription);
		logoutput.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,(char*)lineText);
		i++;
	}
}
void COutputInfoDlg::OnExportToExcelFile()
{
#ifdef __PART_DESIGN_INC_
	LPDISPATCH pDisp;
	LPUNKNOWN pUnk;
	CLSID clsid;
	_Application excel_app;
	CExcelOperObject::ExcelCLSIDFromProgID(clsid);
	if(::GetActiveObject(clsid, NULL, &pUnk) == S_OK) 
	{
		VERIFY(pUnk->QueryInterface(IID_IDispatch,(void**) &pDisp) == S_OK);
		excel_app.AttachDispatch(pDisp);
		pUnk->Release();
	} 
	else if(!excel_app.CreateDispatch(clsid)) 
		AfxMessageBox("Excel program not found");
	
	CWnd* pWnd = CWnd::FindWindow("XLMAIN", NULL);
	if (pWnd != NULL) 
	{
		pWnd->ShowWindow(SW_SHOWNORMAL);
		pWnd->UpdateWindow();
		pWnd->BringWindowToTop();
	}

	LPDISPATCH pWorkbooks;
	Sheets excel_sheets;
	Workbooks excel_workbooks;
	_Workbook excel_workbook;
	_Worksheet excel_sheet;
	Range excel_range;
	LPDISPATCH pRange;
	Borders borders;
	Border border;
	FontLegacy font;
	//char cRangeInfo[500];//ÿ�е�����
	long  i=0,iRow=0,iLineNum=0;//�к�
	COleVariant covTrue((short)TRUE),covFalse((short)FALSE);
	excel_app.SetSheetsInNewWorkbook(2);	//�����ɹ�������Ĭ�Ϲ������������
	VERIFY(pWorkbooks = excel_app.GetWorkbooks());
	excel_workbooks.AttachDispatch(pWorkbooks);
	
	LPDISPATCH pWorkbook = excel_workbooks.Add(); // Save the pointer for
	excel_workbook.AttachDispatch(pWorkbook);
	LPDISPATCH pWorksheets = excel_workbook.GetWorksheets();
	ASSERT(pWorksheets != NULL);
	excel_sheets.AttachDispatch(pWorksheets);
	CXhChar16 cell_start;
	cell_start.Printf("%s","A1");
	CXhChar16 cell_end;
	cell_end.Printf("%s","A1");
	CXhChar100 cell,ss;
	long iSheet=1;
	LPDISPATCH pWorksheet;
	CElement* pElem=NULL,*pTtaElem=NULL;
	CHashList<CElement>hashTtaElems;
	//�ؽ�TTA�ȶԱ���
	for(pElem=postreport.hashElems.GetFirst(),i=0;pElem;pElem=postreport.hashElems.GetNext(),i++)
		hashTtaElems.SetValue(pElem->id,*pElem);
	CLDSModule* pModule;
	CWorkStatus* pCase;
	CHashSet<CLDSModule*>  hashModuleBySerial;
	CHashSet<CWorkStatus*> hashCaseBySerial;
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		hashModuleBySerial.SetValue(pModule->iNo,pModule);
	for(pCase=Ta.WorkStatus.GetFirst();pCase;pCase=Ta.WorkStatus.GetNext())
		hashCaseBySerial.SetValue(pCase->iNo,pCase);
	for(COutputInfo *pOutput=(COutputInfo*)m_xOutputInfoList.GetFirst();pOutput;pOutput=(COutputInfo*)m_xOutputInfoList.GetNext())
	{
		pTtaElem=hashTtaElems.GetValue(pOutput->m_idElem);
		if(pTtaElem==NULL)
			continue;
		if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_BOTH_COMPRESSION)
		{	//1.ͬʱ��ѹ����
			pTtaElem->MaxCompStable.bothComp=pOutput->compare.bValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_TENSION_REDUCTION_COEF)
		{	//2.����ǿ���ۼ�ϵ��
			pTtaElem->design_info.fCoefTensStrengthDecrease=pOutput->compare.fValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_COMPRESSION_REDUCTION_COEF)
		{	//3.��ѹǿ���ۼ�ϵ��
			pTtaElem->design_info.fCoefCompStrengthDecrease=pOutput->compare.fValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_BLOCK_SHEAR_STATE)
		{	//4.����ܿ�״̬
			pTtaElem->design_info.bBlockShearActivated=pOutput->compare.bValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_STABLE_COEF)
		{	//5.ѹ��ϵ��
			pTtaElem->design_info.fCoefStablity=pOutput->compare.fValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_STABLE_mN)
		{	//6.ѹ���ȶ�ǿ���ۼ�ϵ��
			pTtaElem->design_info.mN=pOutput->compare.fValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_LAMDA_L)
		{	//7.���㳤��
			pTtaElem->design_info.lamdaL=pOutput->compare.fValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_GYRATION)
		{	//8.��ת�뾶
			pTtaElem->design_info.lamdaR=pOutput->compare.fValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_LAMDA)
		{	//9.��ϸ��
			pTtaElem->design_info.Lo_r=pOutput->compare.fValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_MAX_TENSION)
		{	//10.�������,N
			pTtaElem->MaxTension.N=pOutput->compare.fValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_MAX_TENSION_CASE)
		{	//11.�����������
			char* key=strtok(pOutput->compare.strbuf,"-");
			if(key)
			{
				pModule=hashModuleBySerial.GetValue(atoi(key));
				if(pModule)
					pTtaElem->MaxTension.hModule=pModule->handle;
			}
			if(key)
				key=strtok(NULL,"-");
			if(key)
			{
				pCase=hashCaseBySerial.GetValue(atoi(key));
				if(pCase)
					pTtaElem->MaxTension.hWorkStatus=pCase->handle;
			}
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_MAX_COMPRESSION)
		{	//12.���ѹ��,N
			pTtaElem->MaxCompression.N=pOutput->compare.fValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_MAX_COMPRESSION_CASE)
		{	//13.���ѹ������
			char* key=strtok(pOutput->compare.strbuf,"-");
			if(key)
			{
				pModule=hashModuleBySerial.GetValue(atoi(key));
				if(pModule)
					pTtaElem->MaxCompression.hModule=pModule->handle;
			}
			if(key)
				key=strtok(NULL,"-");
			if(key)
			{
				pCase=hashCaseBySerial.GetValue(atoi(key));
				if(pCase)
					pTtaElem->MaxCompression.hWorkStatus=pCase->handle;
			}
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_MAX_STABLE_N)
		{	//14.���ѹ����,N
			pTtaElem->MaxCompStable.N=pOutput->compare.fValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_MAX_STABLE_SIGEMA)
		{	//15.���ѹ��Ӧ��,MPa
			pTtaElem->MaxCompStable.sigema=pOutput->compare.fValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_MAX_STABLE_CASE)
		{	//16.���ѹ��������
			char* key=strtok(pOutput->compare.strbuf,"-");
			if(key)
			{
				pModule=hashModuleBySerial.GetValue(atoi(key));
				if(pModule)
					pTtaElem->MaxCompStable.hModule=pModule->handle;
			}
			if(key)
				key=strtok(NULL,"-");
			if(key)
			{
				pCase=hashCaseBySerial.GetValue(atoi(key));
				if(pCase)
					pTtaElem->MaxCompStable.hWorkStatus=pCase->handle;
			}
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_DANGEROUS_CASE)
		{	//17.�ܿس�������
			pTtaElem->design_info.iDangerousLoadCase=pOutput->compare.iValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_MAX_WORK_SIGEMA)
		{	//18.�����Ӧ��,MPa
			pTtaElem->design_info.workSigema=pOutput->compare.fValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_BEARLOAD_COEF)
		{	//19.����ϵ��
			pTtaElem->design_info.fCoefOfBearLoad=pOutput->compare.fValue;
		}
		else if(pOutput->m_ciElemDiffType==COutputInfo::ELEM_DIFF_OUTSIDE_MISS)
		{	//100.��絼�뵥Ԫȱʧ
			hashTtaElems.DeleteNode(pOutput->m_idElem);
		}
	}
	//��һҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("�����ȶ�");
	//1.�趨������
	//1.1 "���"�б���
	//strcpy(cell_start,"A1");
	//strcpy(cell_end,"A2");
	pRange = excel_sheet.GetRange(COleVariant("A1"),COleVariant("A2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	//excel_range.Merge(COleVariant((BYTE)1));	//����ϲ�
	excel_range.SetValue(COleVariant("No."));
	excel_range.SetColumnWidth(COleVariant(3.0));
	excel_range.ReleaseDispatch();
	//1.2 "��Ԫ��ʶ"�б���
	//strcpy(cell_start,"B1");
	//strcpy(cell_end,"B2");
	pRange = excel_sheet.GetRange(COleVariant("B1"),COleVariant("B2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	//excel_range.Merge(COleVariant((BYTE)1));	//����ϲ�
	excel_range.SetColumnWidth(COleVariant(7.0));
	excel_range.SetValue(COleVariant("��Ԫ��ʶ"));
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.ReleaseDispatch();
	//1.3 "����"�б���
	pRange = excel_sheet.GetRange(COleVariant("C1"),COleVariant("C2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	//excel_range.Merge(COleVariant((BYTE)1));	//����ϲ�
	excel_range.SetValue(COleVariant("����"));
	excel_range.ReleaseDispatch();
	//1.4 "���"�б���
	pRange = excel_sheet.GetRange(COleVariant("D1"),COleVariant("D2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	excel_range.SetValue(COleVariant("���"));
	excel_range.ReleaseDispatch();
	//1.5 "�������N+"�б���
	pRange = excel_sheet.GetRange(COleVariant("E1"),COleVariant("F1"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));	//����ϲ�
	excel_range.SetValue(COleVariant("�������N+"));
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.ReleaseDispatch();
		//����
	pRange = excel_sheet.GetRange(COleVariant("E2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("TTA"));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("F2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("LDS"));
	excel_range.ReleaseDispatch();
	//1.6 "���ѹ��N-"�б���
	pRange = excel_sheet.GetRange(COleVariant("G1"),COleVariant("H1"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));	//����ϲ�
	excel_range.SetValue(COleVariant("���ѹ��N-"));
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.ReleaseDispatch();
		//����
	pRange = excel_sheet.GetRange(COleVariant("G2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("TTA"));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("H2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("LDS"));
	excel_range.ReleaseDispatch();
	//1.7 ">N+"�б���
	pRange = excel_sheet.GetRange(COleVariant("I1"),COleVariant("I2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.SetValue(COleVariant(">N+"));
	excel_range.ReleaseDispatch();
	//1.7 ">N-"�б���
	pRange = excel_sheet.GetRange(COleVariant("J1"),COleVariant("J2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.SetValue(COleVariant(">N-"));
	excel_range.ReleaseDispatch();
	//1.8 "��ע"�б���
	pRange = excel_sheet.GetRange(COleVariant("K1"),COleVariant("K2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.SetValue(COleVariant("��ע"));
	excel_range.ReleaseDispatch();
	//��������
	int index=0;
	CVariant2dArray mapPage1(postreport.hashElems.GetNodeNum(),11);//���ݵĶ�ά����
	//�趨�����
	for(pElem=postreport.hashElems.GetFirst(),i=0;pElem;pElem=postreport.hashElems.GetNext(),i++)
	{
		//0.�趨��Ԫ���
		mapPage1.SetValueAt(i,0,COleVariant(i+1));
		//1.�趨��Ԫ��ʶ
		int minPointI=min(pElem->pStart->pointI,pElem->pEnd->pointI);
		int bigPointI=max(pElem->pStart->pointI,pElem->pEnd->pointI);
		mapPage1.SetValueAt(i,1,COleVariant(CXhChar16("%d-%d",minPointI,bigPointI)));
		pTtaElem=hashTtaElems.GetValue(pElem->id);
		//2.�趨��Ԫ����
		char elemtype[5]={0};
		if(pElem->pOrgPole)
			elemtype[0]=pElem->pOrgPole->Layer(1);
		mapPage1.SetValueAt(i,2,COleVariant(elemtype));
		//3.�趨��Ԫ���
		CXhChar16 sizestr("%gX%g",pElem->GetWidth(),pElem->GetThick());
		sizestr.InsertBefore(pElem->pOrgPole->GetBriefPartTypeMark(),0);
		sizestr.Append(pElem->pOrgPole->cMaterial);
		mapPage1.SetValueAt(i,3,COleVariant(sizestr));
		//4.TTA�������
		CXhChar16 szMaxForce;
		if(pTtaElem)
			szMaxForce.Printf("%.2f",pTtaElem->MaxTension.N*0.001);
		mapPage1.SetValueAt(i,4,COleVariant(szMaxForce));
		//5.LDS�������
		szMaxForce.Printf("%.2f",pElem->MaxTension.N*0.001);
		mapPage1.SetValueAt(i,5,COleVariant(szMaxForce));
		//6.TTA���ѹ��
		if(pTtaElem)
			szMaxForce.Printf("%.2f",pTtaElem->MaxCompression.N*0.001);
		else
			szMaxForce.Empty();
		mapPage1.SetValueAt(i,6,COleVariant(szMaxForce));
		//7.LDS���ѹ��
		szMaxForce.Printf("%.2f",pElem->MaxCompression.N*0.001);
		mapPage1.SetValueAt(i,7,COleVariant(szMaxForce));
		//8.>N+
		if(pTtaElem==NULL)
			mapPage1.SetValueAt(i,8,COleVariant("N/A"));
		else
		{
			double diffcoef=0;
			double maxN=max(pElem->MaxTension.N,pTtaElem->MaxTension.N);
			if(maxN>1)
			{
				diffcoef=fabs((pElem->MaxTension.N-pTtaElem->MaxTension.N)/maxN);
				if(diffcoef<0.0001)
					diffcoef=0;
			}
			mapPage1.SetValueAt(i,8,COleVariant(diffcoef));
		}
		//9.>N-
		if(pTtaElem==NULL)
			mapPage1.SetValueAt(i,9,COleVariant("N/A"));
		else
		{
			double diffcoef=0;
			double maxN=max(fabs(pElem->MaxCompression.N),fabs(pTtaElem->MaxCompression.N));
			if(maxN>1)
			{
				diffcoef=fabs((pElem->MaxCompression.N-pTtaElem->MaxCompression.N)/maxN);
				if(diffcoef<0.0001)
					diffcoef=0;
			}
			mapPage1.SetValueAt(i,9,COleVariant(diffcoef));
		}
		//10.��ע
		if(pTtaElem==NULL)
			mapPage1.SetValueAt(i,10,COleVariant("TTA��Ԫȱʧ"));
		else
			mapPage1.SetValueAt(i,10,COleVariant(""));
	}
	//�趨��Ԫ��ʶ��Ϊ�ı�������Ὣ"1-5"������ģ�ͷ�Χ�Զ�ת��������
	cell_end.Printf("B%d",postreport.hashElems.GetNodeNum()+2);
	pRange = excel_sheet.GetRange(COleVariant("B3"),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetNumberFormatLocal(COleVariant("@"));
	excel_range.ReleaseDispatch();
	//����SetRangeValue�����������趨����Ϊ�ı�����ܵ��ã�������Ȼ���ܱ��ⲿ���ı��Զ�תΪ���ڣ�������ʾ����wjh-2017.5.15
	cell_end[0]='K';
	CExcelOper::SetRangeValue(excel_sheet,"A3",cell_end,mapPage1.var);
	//�趨I�м�J��Ϊ��ʾ�ٷֱ�ģʽ
	cell_end[0]='J';
	pRange = excel_sheet.GetRange(COleVariant("I3"),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetNumberFormatLocal(COleVariant("0.0%"));
	excel_range.ReleaseDispatch();
	//�趨�����ָ�Ϊ9
	cell_end[0]='J';
	pRange = excel_sheet.GetRange(COleVariant("A1"),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	//font.SetBold(covTrue);
	font.SetSize(COleVariant(9.0));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	//������
	excel_sheet.DetachDispatch();
	pWorksheet->Release();

	//�ڶ�ҳ
	pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
	iSheet++;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("��Ԫ����ȶ�");

	//1.�趨������
	//1.1 "���"�б���
	//strcpy(cell_start,"A1");
	//strcpy(cell_end,"A2");
	pRange = excel_sheet.GetRange(COleVariant("A1"),COleVariant("A2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	//excel_range.Merge(COleVariant((BYTE)1));	//����ϲ�
	excel_range.SetValue(COleVariant("No."));
	excel_range.SetColumnWidth(COleVariant(3.0));
	excel_range.ReleaseDispatch();
	//1.2 "��Ԫ��ʶ"�б���
	//strcpy(cell_start,"B1");
	//strcpy(cell_end,"B2");
	pRange = excel_sheet.GetRange(COleVariant("B1"),COleVariant("B2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	//excel_range.Merge(COleVariant((BYTE)1));	//����ϲ�
	excel_range.SetColumnWidth(COleVariant(7.0));
	excel_range.SetValue(COleVariant("��Ԫ��ʶ"));
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.ReleaseDispatch();
	//1.3 "����"�б���
	pRange = excel_sheet.GetRange(COleVariant("C1"),COleVariant("C2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	//excel_range.Merge(COleVariant((BYTE)1));	//����ϲ�
	excel_range.SetValue(COleVariant("����"));
	excel_range.ReleaseDispatch();
	//1.4 "���"�б���
	pRange = excel_sheet.GetRange(COleVariant("D1"),COleVariant("D2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	excel_range.SetValue(COleVariant("���"));
	excel_range.ReleaseDispatch();
	//1.5 "����Ӧ��"�б���
	pRange = excel_sheet.GetRange(COleVariant("E1"),COleVariant("F1"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));	//����ϲ�
	excel_range.SetValue(COleVariant("����Ӧ��"));
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.ReleaseDispatch();
		//����
	pRange = excel_sheet.GetRange(COleVariant("E2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("TTA"));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("F2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("LDS"));
	excel_range.ReleaseDispatch();
	//1.6 "m"ǿ���ۼ�ϵ���б���
	pRange = excel_sheet.GetRange(COleVariant("G1"),COleVariant("H1"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));	//����ϲ�
	excel_range.SetValue(COleVariant("m"));
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.ReleaseDispatch();
		//����
	pRange = excel_sheet.GetRange(COleVariant("G2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("TTA"));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("H2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("LDS"));
	excel_range.ReleaseDispatch();
	//1.7 "��"ѹ��ϵ���б���
	pRange = excel_sheet.GetRange(COleVariant("I1"),COleVariant("J1"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));	//����ϲ�
	excel_range.SetValue(COleVariant("��"));
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.ReleaseDispatch();
		//����
	pRange = excel_sheet.GetRange(COleVariant("I2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("TTA"));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("J2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("LDS"));
	excel_range.ReleaseDispatch();
	//1.8 "K*lo/r"�б���
	pRange = excel_sheet.GetRange(COleVariant("K1"),COleVariant("L1"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));	//����ϲ�
	excel_range.SetValue(COleVariant("K*lo/r"));
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.ReleaseDispatch();
		//����
	pRange = excel_sheet.GetRange(COleVariant("K2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("TTA"));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("L2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("LDS"));
	excel_range.ReleaseDispatch();
	//1.9 "����ϵ��"�б���
	pRange = excel_sheet.GetRange(COleVariant("M1"),COleVariant("N1"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));	//����ϲ�
	excel_range.SetValue(COleVariant("����ϵ��"));
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.ReleaseDispatch();
		//����
	pRange = excel_sheet.GetRange(COleVariant("M2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("TTA"));
	excel_range.ReleaseDispatch();
	pRange = excel_sheet.GetRange(COleVariant("N2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("LDS"));
	excel_range.ReleaseDispatch();
	//1.9 ">m"�б���
	pRange = excel_sheet.GetRange(COleVariant("O1"),COleVariant("O2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.SetValue(COleVariant(">m"));
	excel_range.ReleaseDispatch();
	//1.10 ">��"�б���
	pRange = excel_sheet.GetRange(COleVariant("P1"),COleVariant("P2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.SetValue(COleVariant(">��"));
	excel_range.ReleaseDispatch();
	//1.11 ">��"�б���
	pRange = excel_sheet.GetRange(COleVariant("Q1"),COleVariant("Q2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.SetValue(COleVariant(">��"));
	excel_range.ReleaseDispatch();
	//1.12 ">��"�б���
	pRange = excel_sheet.GetRange(COleVariant("R1"),COleVariant("R2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.SetValue(COleVariant(">��"));
	excel_range.ReleaseDispatch();
	//1.13 "��ע"�б���
	pRange = excel_sheet.GetRange(COleVariant("S1"),COleVariant("S2"));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));	//����ϲ�
	excel_range.SetHorizontalAlignment(COleVariant((short)-4108));	//xlCenter:-4108;xlRight:-4152
	excel_range.SetValue(COleVariant("��ע"));
	excel_range.ReleaseDispatch();
	//��������
	CVariant2dArray mapPage2(postreport.hashElems.GetNodeNum(),19);//���ݵĶ�ά����
	//�趨�����
	for(pElem=postreport.hashElems.GetFirst(),i=0;pElem;pElem=postreport.hashElems.GetNext(),i++)
	{
		//0.�趨��Ԫ���
		mapPage2.SetValueAt(i,0,COleVariant(i+1));
		//1.�趨��Ԫ��ʶ
		int minPointI=min(pElem->pStart->pointI,pElem->pEnd->pointI);
		int bigPointI=max(pElem->pStart->pointI,pElem->pEnd->pointI);
		mapPage2.SetValueAt(i,1,COleVariant(CXhChar16("%d-%d",minPointI,bigPointI)));
		pTtaElem=hashTtaElems.GetValue(pElem->id);
		//2.�趨��Ԫ����
		char elemtype[5]={0};
		if(pElem->pOrgPole)
			elemtype[0]=pElem->pOrgPole->Layer(1);
		mapPage2.SetValueAt(i,2,COleVariant(elemtype));
		//3.�趨��Ԫ���
		CXhChar16 sizestr("%gX%g",pElem->GetWidth(),pElem->GetThick());
		sizestr.InsertBefore(pElem->pOrgPole->GetBriefPartTypeMark(),0);
		sizestr.Append(pElem->pOrgPole->cMaterial);
		mapPage2.SetValueAt(i,3,COleVariant(sizestr));
		//4.TTA����Ӧ��
		double lds_mN=1.0,tta_mN=1.0;
		//double A=pElem->pOrgPole->GetArea();
		//double An=A-pElem->design_info.hole_out*pElem->design_info.hole_d*pElem->pOrgPole->GetThick();
		if(pElem->design_info.iDangerousLoadCase==2)		//T
			lds_mN=pElem->design_info.fCoefTensStrengthDecrease;
		else if(pElem->design_info.iDangerousLoadCase==3)	//C
			lds_mN=pElem->design_info.fCoefCompStrengthDecrease;
		else //if(pElem->design_info.iDangerousLoadCase==4)	//S
			lds_mN=pElem->design_info.mN;
		if(pTtaElem)
		{
			if(pTtaElem->design_info.iDangerousLoadCase==2)		//T
				tta_mN=pTtaElem->design_info.fCoefTensStrengthDecrease;
			else if(pTtaElem->design_info.iDangerousLoadCase==3)	//C
				tta_mN=pTtaElem->design_info.fCoefCompStrengthDecrease;
			else //if(pElem->design_info.iDangerousLoadCase==4)	//S
				tta_mN=pTtaElem->design_info.mN;
		}
		CXhChar16 szSigema;
		if(pTtaElem)
			szSigema.Printf("%.2f",pTtaElem->design_info.workSigema);
		mapPage2.SetValueAt(i,4,COleVariant(szSigema));
		//5.LDS����Ӧ��
		szSigema.Printf("%.2f",pElem->design_info.workSigema);
		mapPage2.SetValueAt(i,5,COleVariant(szSigema));
		//6.TTAǿ���ۼ�ϵ��
		if(pTtaElem)
			szSigema.Printf("%.2f",tta_mN);
		else
			szSigema.Empty();
		mapPage2.SetValueAt(i,6,COleVariant(szSigema));
		//7.LDSǿ���ۼ�ϵ��
		szSigema.Printf("%.2f",lds_mN);
		mapPage2.SetValueAt(i,7,COleVariant(szSigema));
		//8.TTAѹ��ϵ��
		if(pTtaElem)
			szSigema.Printf("%.2f",pTtaElem->design_info.fCoefStablity);
		else
			szSigema.Empty();
		mapPage2.SetValueAt(i,8,COleVariant(szSigema));
		//9.LDSѹ��ϵ��
		szSigema.Printf("%.2f",pElem->design_info.fCoefStablity);
		mapPage2.SetValueAt(i,9,COleVariant(szSigema));
		//10.TTA��ϸ��
		if(pTtaElem)
			szSigema.Printf("%.2f",pTtaElem->design_info.Lo_r);
		else
			szSigema.Empty();
		mapPage2.SetValueAt(i,10,COleVariant(szSigema));
		//11.LDS��ϸ��
		szSigema.Printf("%.2f",pElem->design_info.Lo_r);
		mapPage2.SetValueAt(i,11,COleVariant(szSigema));
		//12.TTA����ϵ��
		if(pTtaElem)
			szSigema.Printf("%.2f",pTtaElem->design_info.fCoefOfBearLoad);
		else
			szSigema.Empty();
		mapPage2.SetValueAt(i,12,COleVariant(szSigema));
		//13.LDS����ϵ��
		szSigema.Printf("%.2f",pElem->design_info.fCoefOfBearLoad);
		mapPage2.SetValueAt(i,13,COleVariant(szSigema));
		//14.>m
		if(pTtaElem==NULL)
			mapPage2.SetValueAt(i,14,COleVariant("N/A"));
		else
		{
			double diffcoef=0;
			double maxN=max(lds_mN,tta_mN);
			if(maxN>1)
			{
				diffcoef=fabs((lds_mN-tta_mN)/maxN);
				if(diffcoef<0.0001)
					diffcoef=0;
			}
			mapPage2.SetValueAt(i,14,COleVariant(diffcoef));
		}
		//15.>��
		if(pTtaElem==NULL)
			mapPage2.SetValueAt(i,15,COleVariant("N/A"));
		else
		{
			double diffcoef=0;
			double maxN=max(fabs(pElem->design_info.workSigema),fabs(pTtaElem->design_info.workSigema));
			if(maxN>0.1)
			{
				diffcoef=fabs((fabs(pElem->design_info.workSigema)-fabs(pTtaElem->design_info.workSigema))/maxN);
				if(diffcoef<0.0001)
					diffcoef=0;
			}
			mapPage2.SetValueAt(i,15,COleVariant(diffcoef));
		}
		//16.>��
		if(pTtaElem==NULL)
			mapPage2.SetValueAt(i,16,COleVariant("N/A"));
		else
		{
			double diffcoef=0;
			double maxN=max(fabs(pElem->design_info.fCoefStablity),fabs(pTtaElem->design_info.fCoefStablity));
			if(maxN>1)
			{
				diffcoef=fabs((pElem->design_info.fCoefStablity-pTtaElem->design_info.fCoefStablity)/maxN);
				if(diffcoef<0.0001)
					diffcoef=0;
			}
			mapPage2.SetValueAt(i,16,COleVariant(diffcoef));
		}
		//17.>��
		if(pTtaElem==NULL)
			mapPage2.SetValueAt(i,17,COleVariant("N/A"));
		else
		{
			double diffcoef=0;
			double maxN=max(fabs(pElem->design_info.Lo_r),fabs(pTtaElem->design_info.Lo_r));
			if(maxN>1)
			{
				diffcoef=fabs((pElem->design_info.Lo_r-pTtaElem->design_info.Lo_r)/maxN);
				if(diffcoef<0.0001)
					diffcoef=0;
			}
			mapPage2.SetValueAt(i,17,COleVariant(diffcoef));
		}
		//18.��ע
		if(pTtaElem==NULL)
			mapPage2.SetValueAt(i,18,COleVariant("TTA��Ԫȱʧ"));
		else
			mapPage2.SetValueAt(i,18,COleVariant(""));
	}
	//�趨��Ԫ��ʶ��Ϊ�ı�������Ὣ"1-5"������ģ�ͷ�Χ�Զ�ת��������
	cell_end.Printf("B%d",postreport.hashElems.GetNodeNum()+2);
	pRange = excel_sheet.GetRange(COleVariant("B3"),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetNumberFormatLocal(COleVariant("@"));
	excel_range.ReleaseDispatch();
	//����SetRangeValue�����������趨����Ϊ�ı�����ܵ��ã�������Ȼ���ܱ��ⲿ���ı��Զ�תΪ���ڣ�������ʾ����wjh-2017.5.15
	cell_end[0]='S';//.Printf("S%d",postreport.hashElems.GetNodeNum()+2);
	CExcelOper::SetRangeValue(excel_sheet,"A3",cell_end,mapPage2.var);
	//�趨I�м�J��Ϊ��ʾ�ٷֱ�ģʽ
	cell_end[0]='R';
	pRange = excel_sheet.GetRange(COleVariant("O3"),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.SetNumberFormatLocal(COleVariant("0.0%"));
	excel_range.ReleaseDispatch();
	//�趨�����ָ�Ϊ9
	cell_end[0]='S';
	pRange = excel_sheet.GetRange(COleVariant("A1"),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	//font.SetBold(covTrue);
	font.SetSize(COleVariant(9.0));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	//������


	//char save_file_name[100];
	//strcpy(save_file_name,file_name);
	//strcat(save_file_name,"_Diff.xls");
	//DeleteFile(save_file_name);
	//COleVariant covOptional((long)DISP_E_PARAMNOTFOUND,   VT_ERROR);  
	//excel_workbook.SaveAs(COleVariant(save_file_name),covOptional,covOptional, covOptional,covOptional,covOptional,0,covOptional,covOptional,covOptional,covOptional);
	//�ͷ�
	excel_sheets.ReleaseDispatch();
	excel_workbook.ReleaseDispatch();
	excel_workbooks.ReleaseDispatch();
	excel_app.ReleaseDispatch();
	//excel_app.Quit();
#endif
}
void COutputInfoDlg::OnCorrectErrorItem()
{
	CWaitCursor waitCursor;
	POSITION pos = m_listOutputInfoCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = m_listOutputInfoCtrl.GetNextSelectedItem(pos);
		COutputInfo *pItemInfo = (COutputInfo*)m_listOutputInfoCtrl.GetItemData(iCurSel);
		if(pItemInfo&&!pItemInfo->SolvedState())
		{
			if(pItemInfo->CorrectError())	//����ͨ��������Ŀ
				m_listOutputInfoCtrl.SetItemText(iCurSel,0,"��");
			m_listOutputInfoCtrl.SetItemText(iCurSel,2,pItemInfo->m_sDescription);	//��������
		}
	}
	g_pSolidDraw->Draw();
}

void COutputInfoDlg::OnRecheck()
{
	POSITION pos = m_listOutputInfoCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = m_listOutputInfoCtrl.GetNextSelectedItem(pos);
		COutputInfo *pItemInfo = (COutputInfo*)m_listOutputInfoCtrl.GetItemData(iCurSel);
		if(pItemInfo)
		{
			if(pItemInfo->Recheck())
			{
				m_listOutputInfoCtrl.SetItemText(iCurSel,0,"��");
#ifdef AFX_TARG_ENU_ENGLISH
				m_listOutputInfoCtrl.SetItemText(iCurSel,2,"Passed");	//update content
#else
				m_listOutputInfoCtrl.SetItemText(iCurSel,2,"ͨ������У��");	//��������
#endif
			}
			else
			{
				m_listOutputInfoCtrl.SetItemText(iCurSel,0,"��");
				m_listOutputInfoCtrl.SetItemText(iCurSel,2,pItemInfo->m_sDescription);	//��������
			}
		}
	}
	/*char sTitle[50]="";
	if(m_listOutputInfoCtrl.GetItemCount()>0)
		sprintf(sTitle,"�����б� - %d������",m_listOutputInfoCtrl.GetItemCount());
	else 
		sprintf(sTitle,"�����б�");
	((CMainFrame*)AfxGetMainWnd())->SetBottomWorkspaceTitle(sTitle);*/
}
void COutputInfoDlg::OnMarkNormal()
{
	POSITION pos = m_listOutputInfoCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = m_listOutputInfoCtrl.GetNextSelectedItem(pos);
		COutputInfo *pItemInfo = (COutputInfo*)m_listOutputInfoCtrl.GetItemData(iCurSel);
		if(pItemInfo)
		{
			CString  keyStr=pItemInfo->GetKeyStr();
			if(markInfoList.GetValue(keyStr.GetBuffer())==NULL)
			{
				markInfoList.SetValue(keyStr.GetBuffer(),*pItemInfo);
				m_listOutputInfoCtrl.SetItemText(iCurSel,0,"��");
			}
		}
	}
}
void COutputInfoDlg::OnCancelMark()
{
	POSITION pos = m_listOutputInfoCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = m_listOutputInfoCtrl.GetNextSelectedItem(pos);
		COutputInfo *pItemInfo = (COutputInfo*)m_listOutputInfoCtrl.GetItemData(iCurSel);
		if(pItemInfo)
		{
			CString keyStr=pItemInfo->GetKeyStr();
			if(markInfoList.GetValue(keyStr.GetBuffer())!=NULL)
			{
				markInfoList.DeleteNode(keyStr.GetBuffer());
				m_listOutputInfoCtrl.SetItemText(iCurSel,0,"");
			}
		}
	}
}
//<DEVELOP_PROCESS_MARK nameId="COutputInfoDlg.OnAutoCutJg">
#include "CutJg.h"
void COutputInfoDlg::OnAutoCutJg()
{
	CLogErrorLife loglife;
	CUndoOperObject undo(&Ta,true);
	int nCutDist=0;
	POSITION pos = m_listOutputInfoCtrl.GetFirstSelectedItemPosition();
	if(m_listOutputInfoCtrl.GetSelectedCount()>1)
	{
		nCutDist=CLDSPart::library->GetVertexDist();
		nCutDist=InputCutAngleCollideDist(nCutDist);
	}
	bool needredraw=false;
	while(pos!=NULL)
	{
		int iCurSel = m_listOutputInfoCtrl.GetNextSelectedItem(pos);
		COutputInfo *pItemInfo = (COutputInfo*)m_listOutputInfoCtrl.GetItemData(iCurSel);
		int iKey=iCurSel+1;
		ANGLE_CUT_INFO* pJgCutInfo=cutJg.hashAngleCuts.GetValue(iKey);
		if(pJgCutInfo==NULL)
			continue;

		if((nCutDist>0&&CCutJg::FinishCutAngle(pJgCutInfo,NULL,nCutDist,FALSE))||
			(nCutDist==0&&CCutJg::FinishCutAngle(pJgCutInfo,InputCutAngleCollideDist,nCutDist,FALSE)))
		{
			CString  keyStr=pItemInfo->GetKeyStr();
			if(markInfoList.GetValue(keyStr.GetBuffer())==NULL)
			{
				//markInfoList.SetValue(keyStr.GetBuffer(),*pItemInfo);
#ifdef AFX_TARG_ENU_ENGLISH
				m_listOutputInfoCtrl.SetItemText(iCurSel,0,"completed");
#else
				m_listOutputInfoCtrl.SetItemText(iCurSel,0,"���");
#endif
				needredraw=true;
			}
		}
		else
		{
			CString keyStr=pItemInfo->GetKeyStr();
			if(markInfoList.GetValue(keyStr.GetBuffer())==NULL)
			{
				//markInfoList.SetValue(keyStr.GetBuffer(),*pItemInfo);
#ifdef AFX_TARG_ENU_ENGLISH
				m_listOutputInfoCtrl.SetItemText(iCurSel,0,"failed");
#else
				m_listOutputInfoCtrl.SetItemText(iCurSel,0,"ʧ��");
#endif
			}
		}
	}
	if(needredraw)
		g_pSolidDraw->Draw();
}
//</DEVELOP_PROCESS_MARK>

void COutputInfoDlg::OnClickOutputInfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	*pResult = 0;
}

void COutputInfoDlg::OnDblclkOutputInfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if( GetFocus() != this) 
		SetFocus();
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	LVHITTESTINFO ht;
	ht.pt = pNMListView->ptAction;
	m_listOutputInfoCtrl.SubItemHitTest(&ht);
	if(ht.iItem<0)
		return;	
	COutputInfo *pItemInfo = (COutputInfo*)m_listOutputInfoCtrl.GetItemData(ht.iItem);
	if(pItemInfo==NULL)
		return;
	pItemInfo->SelectRelationObj();	
	*pResult = 0;
}

void COutputInfoDlg::OnRclickOutputInfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_listOutputInfoCtrl.GetSelectedCount()==0)
		return;
	CPoint point;
	GetCursorPos(&point);
	CRect rect;
	m_listOutputInfoCtrl.GetWindowRect(&rect);
	if(rect.PtInRect(point))
		ContextMenu(this,point);
	*pResult = 0;
}
//<DEVELOP_PROCESS_MARK nameId="COutputInfoDlg.ContextMenu">
void COutputInfoDlg::ContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint scr_point = point;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	POSITION pos=m_listOutputInfoCtrl.GetFirstSelectedItemPosition();
	AUTOCORRECT_CMDINFO mutualCmdInfo;
	mutualCmdInfo.safeType=-1;
	COutputInfo *pItem=NULL;
	while(pos)
	{
		int iSel=m_listOutputInfoCtrl.GetNextSelectedItem(pos);
		pItem=(COutputInfo*)m_listOutputInfoCtrl.GetItemData(iSel);
		AUTOCORRECT_CMDINFO cmdInfo;
		int safeType=pItem->CorrectErrorCmdInfo(cmdInfo);
		if(safeType<0)
			continue;	//��֧���Զ��������������Ӱ��˵�������ʾ
		if(mutualCmdInfo.safeType<0)
		{
			mutualCmdInfo.safeType=safeType;
			mutualCmdInfo.nativeName.Copy(cmdInfo.nativeName);
		}
		else if(mutualCmdInfo.safeType==0&&safeType==0&&stricmp(mutualCmdInfo.nativeName,cmdInfo.nativeName)!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			mutualCmdInfo.nativeName.Copy("automatically modify");
#else
			mutualCmdInfo.nativeName.Copy("�Զ�����");
#endif
		else if(stricmp(mutualCmdInfo.nativeName,cmdInfo.nativeName)!=0)
			mutualCmdInfo.nativeName.Copy("");	//�޷�ͳһ������Ĳ˵���ʾ
	}
	if(pItem->m_uType==INFO_ONEKEY_CUTANGLE_CHECK)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_AUTO_CUT_JG,"cutting angle automatically");
		pMenu->AppendMenu(MF_STRING,ID_MARK_NORMAL,"normal mark");
		pMenu->AppendMenu(MF_STRING,ID_CANCEL_MARK,"cancel mark");
#else
		pMenu->AppendMenu(MF_STRING,ID_AUTO_CUT_JG,"�Զ��н�");
		pMenu->AppendMenu(MF_STRING,ID_MARK_NORMAL,"�������");
		pMenu->AppendMenu(MF_STRING,ID_CANCEL_MARK,"ȡ�����");
#endif
	}
	else
	{
		if(mutualCmdInfo.safeType>=0&&mutualCmdInfo.nativeName.GetLength()>0)
			pMenu->AppendMenu(MF_STRING,ID_CORRECT_ERRORITEM,mutualCmdInfo.nativeName);
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_RESULT,"Export to text file");
		pMenu->AppendMenu(MF_SEPARATOR,ID_SEPARATOR,"");
		pMenu->AppendMenu(MF_STRING,ID_RECHECK,"check again");
		pMenu->AppendMenu(MF_STRING,ID_MARK_NORMAL,"normal mark");
		pMenu->AppendMenu(MF_STRING,ID_CANCEL_MARK,"cancel mark");
#else
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_RESULT,"��������±�");
#ifdef __PART_DESIGN_INC_
		for(COutputInfo* pOutput=(COutputInfo*)m_xOutputInfoList.GetFirst();pOutput;pOutput=(COutputInfo*)m_xOutputInfoList.GetNext())
		{
			if(pOutput->m_ciElemDiffType==0)
				continue;
			pMenu->AppendMenu(MF_STRING,ID_EXPORT_EXCEL_FILE,"�����Excel");
			break;	//��Ԫ�������ȶ����ʱ���������Excel�������� wjh-2017.3.15
		}
#endif
		pMenu->AppendMenu(MF_SEPARATOR,ID_SEPARATOR,"");
		pMenu->AppendMenu(MF_STRING,ID_RECHECK,"����У��");
		pMenu->AppendMenu(MF_STRING,ID_MARK_NORMAL,"�������");
		pMenu->AppendMenu(MF_STRING,ID_CANCEL_MARK,"ȡ�����");
#endif
	}
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}
//</DEVELOP_PROCESS_MARK>
void COutputInfoDlg::OnOK()
{	//ʵ��OnOK����������Ի����ڼ���״̬ʱ��Enter����رնԻ���

}
void COutputInfoDlg::OnCancel() 
{	//ʵ��OnCancel����������Ի����ڼ���״̬ʱ��ECS����رնԻ���
}