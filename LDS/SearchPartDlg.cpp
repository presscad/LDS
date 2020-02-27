//<LOCALE_TRANSLATE Confirm by hxr/>
// SearchPartDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "SearchPartDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "env_def.h"
#include "OutputInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CSearchPartDlg �Ի���
CSearchPartDlg *g_pSearchPartDlg;
IMPLEMENT_DYNAMIC(CSearchPartDlg, CDialog)

int CSearchPartDlg::nScrLocationX=0;
int CSearchPartDlg::nScrLocationY=0;
CSearchPartDlg::CSearchPartDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchPartDlg::IDD, pParent)
	, m_sSearchText(_T(""))
{
	m_bSearchResult=FALSE;
	m_bEnableWindowMoveListen=false;
	m_hDatumSelPart=0;
	m_bHideListCtrl=false;
	m_sPromptInfo="";
	//
#ifdef AFX_TARG_ENU_ENGLISH
	m_listCtrl.AddColumnHeader("handle");
	m_listCtrl.AddColumnHeader("number");
	m_listCtrl.AddColumnHeader("type");
#else
	m_listCtrl.AddColumnHeader("���");
	m_listCtrl.AddColumnHeader("���");
	m_listCtrl.AddColumnHeader("����");
#endif
}

CSearchPartDlg::~CSearchPartDlg()
{
}

void CSearchPartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CTRL, m_listCtrl);
	DDX_Text(pDX, IDC_E_PART_SEARCH_TEXT, m_sSearchText);
	DDX_Check(pDX, IDC_CHK_SEARCH_RESULT, m_bSearchResult);
	DDX_Control(pDX, IDC_TAB_SEARCH_TYPE, m_tabSearchType);
	DDX_Text(pDX, IDC_STATI_SEARCH_RESULT, m_sPromptInfo);
}


BEGIN_MESSAGE_MAP(CSearchPartDlg, CDialog)
	ON_WM_MOVE()
	ON_BN_CLICKED(ID_BTN_SEARCH, OnSearchPart)
	ON_BN_CLICKED(IDC_CHK_SEARCH_RESULT, OnExpandSearchResult)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CTRL, OnNMRClickListCtrl)
	ON_COMMAND(ID_SELECT_ITEM,OnSelectItem)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_CTRL, OnKeydownListCtrl)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CTRL, OnNMClickListCtrl)
	ON_BN_CLICKED(IDC_BN_CHECK_PARTNO, &CSearchPartDlg::OnBnCheckPartno)
	ON_BN_CLICKED(IDC_BN_UPDATE_SAME_LABEL_PARTS_RELA_PROPERTY, &CSearchPartDlg::OnBnClickedBnUpdateSameLabelPartsRelaProperty)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SEARCH_TYPE, &CSearchPartDlg::OnTcnSelchangeTabSearchType)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_E_PART_SEARCH_TEXT, &CSearchPartDlg::OnEnChangeEPartSearchText)
	ON_NOTIFY(NM_CLICK, IDC_TAB_SEARCH_TYPE, &CSearchPartDlg::OnNMClickTabSearchType)
	ON_BN_CLICKED(IDC_BN_SELECT_ALL, &CSearchPartDlg::OnBnClickedBnSelectAll)
END_MESSAGE_MAP()


// CSearchPartDlg ��Ϣ�������
BOOL CSearchPartDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	BOOL bReadPointI=FALSE;
#ifdef __TSA_
	bReadPointI=TRUE;
#elif defined(__LDS_)
	bReadPointI=TRUE;
#endif
	m_tabSearchType.DeleteAllItems(); 
#ifdef AFX_TARG_ENU_ENGLISH
	m_tabSearchType.InsertItem(0,"Comprehensive");
	m_tabSearchType.InsertItem(1,"Handle");
	m_tabSearchType.InsertItem(2,"Part Label");
	if(bReadPointI)
		m_tabSearchType.InsertItem(3,"Node Number");
#else
	m_tabSearchType.InsertItem(0,"�ۺ�");
	m_tabSearchType.InsertItem(1,"���");
	m_tabSearchType.InsertItem(2,"����");
	if(bReadPointI)
		m_tabSearchType.InsertItem(3,"���");
#endif
	m_tabSearchType.SetCurSel(0);
	long col_wide_arr[3]={65,65,65};
	m_listCtrl.InitListCtrl(col_wide_arr);
	HideListCtrl();
	CWnd* pCheckButton=GetDlgItem(IDC_BN_CHECK_PARTNO);
	if(pCheckButton)
		pCheckButton->ShowWindow(SW_HIDE);
	CWnd* pUpdtPropBtn=GetDlgItem(IDC_BN_UPDATE_SAME_LABEL_PARTS_RELA_PROPERTY);
	if(pUpdtPropBtn)
		pUpdtPropBtn->EnableWindow(FALSE);
	//
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n>0)
	{	//ѡ�й���
		CLDSPart *pPart=console.FromPartHandle(id_arr[0]);
		if(pPart&&strlen(pPart->GetPartNo())>0)
		{	//����ѡ��������Ĭ�ϼ��ţ������Ҷ�Ӧ����
			if(n>1)
			{
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(pPart->handle);
			}
			m_sSearchText=pPart->GetPartNo();
			InitListCtrl(id_arr[0]);
			m_tabSearchType.SetCurSel(2);
			if(pCheckButton)
				pCheckButton->ShowWindow(SW_SHOW);
			if(pUpdtPropBtn)
				pUpdtPropBtn->EnableWindow(TRUE);
			SpreadListCtrl();
			UpdateData(FALSE);
		}
	}
	//�ƶ����ڵ�����λ��
	CRect rect;
	CWnd::GetWindowRect(rect);
	int width = rect.Width();
	int height=rect.Height();
	rect.left=nScrLocationX;
	rect.top=nScrLocationY;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;
	MoveWindow(rect, TRUE);
	m_bEnableWindowMoveListen=true;
	return TRUE;
}

void CSearchPartDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}
void CSearchPartDlg::AddSearchResultObjToListCtrl(CLDSDbObject* pDbObj)
{
	if(pDbObj==NULL)
		return;
	CStringArray str_arr;
	str_arr.SetSize(3);
	str_arr[0].Format("0X%X",pDbObj->handle);
	/*if(hDatumSelPart==pDbObj->handle)
	{
		m_hDatumSelPart=hDatumSelPart;
		str_arr[0].AppendChar('*');
	}*/
	if(pDbObj->IsNode())
	{
#ifdef __PART_DESIGN_INC_
		str_arr[1].Format("%d",((CLDSNode*)pDbObj)->point_i); 
#else
		str_arr[1]=""; 
#endif
#ifdef AFX_TARG_ENU_ENGLISH
		str_arr[2]="Node";
#else
		str_arr[2]="�ڵ�";
#endif
	}
	else if(pDbObj->IsPart())
	{
		CLDSPart* pPart=(CLDSPart*)pDbObj;
		str_arr[1].Format("%s",(char*)pPart->GetPartNo()); 
		if(pPart->IsAngle())
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[2]="Angle";
		else if(pPart->IsPlate())
			str_arr[2]="Plate";
		else if(pPart->GetClassTypeId()==CLS_LINETUBE)
			str_arr[2]="Tube";
		else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
			str_arr[2]="Flat";
		else if(pPart->GetClassTypeId()==CLS_LINESLOT)
			str_arr[2]="Slot";
#else
			str_arr[2]="�Ǹ�";
		else if(pPart->IsPlate())
			str_arr[2]="�ְ�";
		else if(pPart->GetClassTypeId()==CLS_LINETUBE)
			str_arr[2]="�ֹ�";
		else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
			str_arr[2]="����";
		else if(pPart->GetClassTypeId()==CLS_LINESLOT)
			str_arr[2]="�۸�";
#endif
	}
	m_listCtrl.InsertItemRecord(-1,str_arr);    //�����¼
}
//from outputinfo.cpp
SCOPE_STRU GetPartScope(CLDSPart* pPart,SCOPE_STRU *pScope=NULL);
void CSearchPartDlg::SearchPart(BOOL bColseDialog/*=TRUE*/,int bySpecMode/*=-1*/)
{	
	SetPromptInfo("");
	m_listCtrl.DeleteAllItems();
	if(!bColseDialog)
		SpreadListCtrl();
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	int iCurSelMode=bySpecMode>=0?bySpecMode:m_tabSearchType.GetCurSel();
	int p=m_tabSearchType.GetCurSel();
	if(iCurSelMode==0)
	{	//�����ۺϲ���
		DBOBJSET objset;
		if(pLDSView->QuickFindPart(m_sSearchText,&objset))
		{
			for(CLDSDbObject* pObj=objset.GetFirst();pObj;pObj=objset.GetNext())
				AddSearchResultObjToListCtrl(pObj);
			if(bColseDialog)
			{
				if(objset.GetNodeNum()<=1)
					return CSearchPartDlg::OnOK();
				SpreadListCtrl();
				return;
			}
			else 
				return;
		}
		else
#ifdef AFX_TARG_ENU_ENGLISH
			SetPromptInfo("Fail to find the object of specified handle!");
#else
			SetPromptInfo("δ��������ָ������Ķ���!");
#endif
	}
	else if(iCurSelMode==1)
	{	//���������
		long hCurPart=0;
		sscanf(m_sSearchText,"%X",&hCurPart);
		CLDSNode *pNode=console.FromNodeHandle(hCurPart);
		if(pNode)
		{
			f3dPoint center;
			SCOPE_STRU scope;
			scope.VerifyVertex(pNode->Position()-f3dPoint(500,500,500));
			scope.VerifyVertex(pNode->Position()+f3dPoint(500,500,500));
			scope.centre(&center.x,&center.y,&center.z);
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(hCurPart);
			if(pLDSView->m_curAppType==TA_ASSEMBLE_ENV)
			{	
				g_pSolidOper->FocusTo(scope,g_sysPara.m_fMaxScaleScr2User);
				g_pSolidSet->SetOperType(OPER_ROTATE);	//���ò�������Ϊ��ת
				g_pSolidSet->SetRotOrg(center);
				g_pSolidSet->SetZoomStyle(ROT_CENTER);	//����Ϊָ����ת��������
				Invalidate(FALSE);
			}
			pLDSView->UpdatePropertyPage(pNode);
			if(bColseDialog)
				return CSearchPartDlg::OnOK();
			else
			{
				AddSearchResultObjToListCtrl(pNode);
				return;
			}
		}
		CLDSPart *pPart=console.FromPartHandle(hCurPart);
		if(pPart)
		{
			f3dPoint center;
			SCOPE_STRU scope=GetPartScope(pPart);
			scope.centre(&center.x,&center.y,&center.z);
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(hCurPart);
			if(pLDSView->m_curAppType==TA_ASSEMBLE_ENV)
			{	
				g_pSolidOper->FocusTo(scope,g_sysPara.m_fMaxScaleScr2User);
				g_pSolidSet->SetOperType(OPER_ROTATE);	//���ò�������Ϊ��ת
				g_pSolidSet->SetRotOrg(center);
				g_pSolidSet->SetZoomStyle(ROT_CENTER);	//����Ϊָ����ת��������
				Invalidate(FALSE);
			}
			pLDSView->UpdatePropertyPage(pPart);
			if(bColseDialog)
				return CSearchPartDlg::OnOK();
			else
			{
				AddSearchResultObjToListCtrl(pPart);
				return;
			}
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			SetPromptInfo("Fail to find the object of specified handle!");
#else
			SetPromptInfo("δ��������ָ������Ķ���!");
#endif
			return;
		}
	}
	else if(iCurSelMode==2)
	{	//�����Ų���
		InitListCtrl();
		if(m_listCtrl.GetItemCount()<=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			SetPromptInfo("Fail to find the object of specified part label!");
#else
			SetPromptInfo("δ��������ָ�����ŵĶ���!");
#endif
			return;
		}
		//Ĭ��ѡ�е�һ������
		m_listCtrl.SetFocus();
		m_listCtrl.SetItemState(0,LVNI_SELECTED,LVNI_SELECTED);
		SelectPart(0);
		SpreadListCtrl();
		UpdateData(FALSE);
	}
	else
	{	//���ݵ�Ų���
		long hNode=0;
		bool hasNonDigitLetter=false;	//���з�������ĸ
		CXhChar50 text(m_sSearchText);
		char* iter=(char*)text;
		while(iter!=0)
		{
			if(*iter==0)
				break;
			else if(*iter<'0'||*iter>'9')
			{
				hasNonDigitLetter=true;
				break;
			}
			iter++;
		}
		if(!hasNonDigitLetter)
			sscanf(m_sSearchText,"%d",&hNode);
		CLDSNode* pSelNode=NULL;
		if(hNode>0)
		{
			for(pSelNode=Ta.Node.GetFirst();pSelNode;pSelNode=Ta.Node.GetNext())
			{
				if(pSelNode->point_i==hNode)
					break;
			}
		}
		if(pSelNode)
		{
			g_pSolidDraw->ReleaseSnapStatus();
			SCOPE_STRU scope;
			scope.VerifyVertex(pSelNode->Position(false));
			scope.fMinX-=500;
			scope.fMaxX+=500;
			scope.fMinY-=500;
			scope.fMaxY+=500;
			scope.fMinZ-=500;
			scope.fMaxZ+=500;
			g_pSolidOper->FocusTo(scope,g_sysPara.m_fMaxScaleScr2User);
			f3dPoint center;
			g_pSolidSet->SetOperType(OPER_ROTATE);
			g_pSolidSet->SetZoomStyle(ROT_CENTER);
			scope.centre(&center.x,&center.y,&center.z);
			g_pSolidSet->SetRotOrg(center);
			Invalidate(FALSE);
			pLDSView->UpdatePropertyPage(pSelNode);
			if(bColseDialog)
				return CSearchPartDlg::OnOK();
			else
			{
				AddSearchResultObjToListCtrl(pSelNode);
				return;
			}
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			SetPromptInfo("Fail to find the object of specified node number!");
#else
			SetPromptInfo("δ����ָ����ŵĽڵ�!");
#endif
			return;
		}
	}
}
void CSearchPartDlg::OnSearchPart()
{
	UpdateData();
	SearchPart();
}
void CSearchPartDlg::OnExpandSearchResult()
{
	UpdateData();
	ResetDlgSize();
	UpdateData(FALSE);
}
void CSearchPartDlg::OnNMRClickListCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iCurSel = m_listCtrl.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); 
	if(iCurSel<0)
		return;
	CPoint point;
	GetCursorPos(&point);
	ContextMenu(this,point);
	*pResult = 0;
}
void CSearchPartDlg::OnSelectItem()
{ 
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	g_pSolidDraw->ReleaseSnapStatus();
	//ѡ��ͬ���Ź���
	CXhChar50 tem_str;
	BOOL selected=false;
	SCOPE_STRU scope;
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	CLDSDbObject* pDbObj=NULL;
	while (pos)
	{
		int nItem = m_listCtrl.GetNextSelectedItem(pos);
		m_listCtrl.GetItemText(nItem,0,tem_str,50);
		long hCurPart=0;
		sscanf(tem_str,"%X",&hCurPart);
		selected=g_pSolidDraw->SetEntSnapStatus(hCurPart);
		CLDSPart *pPart=console.FromPartHandle(hCurPart);
		if(nItem==0||m_listCtrl.GetSelectedCount()==1)
		{	//���ù�������ԭ��Ϊ��ת���ĵ㷽�㹹����λ
			f3dPoint org;
			if(pPart)
			{
				pDbObj=pPart;
				org=pPart->ucs.origin;
			}
			else
			{
				pDbObj=console.FromNodeHandle(hCurPart);
				if(pDbObj&&pDbObj->IsNode())
					org=((CLDSNode*)pDbObj)->ActivePosition();
			}
			g_pSolidSet->SetOperType(OPER_ROTATE);	//���ò�������Ϊ��ת
			g_pSolidSet->SetRotOrg(org);
			g_pSolidSet->SetZoomStyle(ROT_CENTER);	//����Ϊָ����ת��������
		}
		if(!selected&&pPart)
		{	//ʵ�崦��δ��ʾ״̬ʱ,����ʵ�岢ѡ�� wht 16-12-05
			g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
			g_pSolidDraw->SetEntSnapStatus(hCurPart);
		}
		scope=GetPartScope(pPart,&scope);
	}
	g_pSolidOper->FocusTo(scope,g_sysPara.m_fMaxScaleScr2User);
	if(!selected)
	{
		pLDSView->UpdatePropertyPage(pDbObj,FALSE,m_hWnd);
#ifdef AFX_TARG_ENU_ENGLISH
		//MessageBox("The object of specified handle is hidden currently!");
#else
		//MessageBox("ָ������Ķ���ǰ���ڷ���ʾ״̬��");
#endif
	}
	//Invalidate(FALSE);
	//CSearchPartDlg::OnOK();
}
void CSearchPartDlg::OnNMClickListCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if(m_listCtrl.GetSelectedCount()>1)
		OnSelectItem();		//��ѡ
	else
	{	//��ѡ
		int iCurSel = m_listCtrl.GetSelectedItem();
		if(iCurSel>=0)
			SelectPart(iCurSel);
		else	
		{	//δѡ�����񹹼�ȡ������ѡ��״̬
			g_pSolidDraw->ReleaseSnapStatus();
			CLDSView* pView=(CLDSView*)((CMainFrame*)theApp.m_pMainWnd)->GetLDSView();
			if(pView)
				pView->UpdatePropertyPage(NULL,FALSE,m_hWnd);
		}
	}
	*pResult = 0;
}
void CSearchPartDlg::OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_listCtrl.GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_UP)
		{	//��һ��UP��ѡ���������һλ����������������ڰ���UP��ʱ������
			//��ʱ����δ�ƶ��������ֶ�����ѡ�����λ��
			if(iCurSel>=1)
				iCurSel--;	
			if(iCurSel>=0)
				SelectPart(iCurSel);
		}
		else if(pLVKeyDow->wVKey==VK_DOWN)
		{	//��һ��DOWN��ѡ���������һλ����������������ڰ���DOWN��ʱ������
			//��ʱ����δ�ƶ��������ֶ�����ѡ�����λ��
			if(iCurSel<m_listCtrl.GetItemCount())
				iCurSel++;
			if(iCurSel>=0)
				SelectPart(iCurSel);
		}
	}
	*pResult = 0;
}
//
void CSearchPartDlg::SelectPart(int nCurSel)
{
	CXhChar50 tem_str;
	m_listCtrl.GetItemText(nCurSel,0,tem_str,50);
	long hCurPart=0;
	sscanf(tem_str,"%X",&hCurPart);
	CLDSDbObject *pDbObj=(CLDSDbObject*)console.FromHandle(hCurPart,TRUE);
	CLDSView* pView=(CLDSView*)((CMainFrame*)theApp.m_pMainWnd)->GetLDSView();
	if(pView)
		pView->UpdatePropertyPage(pDbObj,FALSE,m_hWnd);
	g_pSolidDraw->ReleaseSnapStatus();
	if(!g_pSolidDraw->SetEntSnapStatus(hCurPart))
	{
		if(pDbObj&&pDbObj->IsPart())
		{	//ʵ�崦��δ��ʾ״̬ʱ,����ʵ�岢ѡ�� wht 16-12-05
			g_pSolidDraw->NewSolidPart(((CLDSPart*)pDbObj)->GetSolidPartObject());
			g_pSolidDraw->SetEntSnapStatus(hCurPart);
		}
	}
	if(pDbObj&&(pDbObj->IsPart()||pDbObj->IsNode()))
	{
		SCOPE_STRU scope;
		if(pDbObj->IsPart())
		{
			scope=GetPartScope((CLDSPart*)pDbObj);
			if(g_sysPara.m_bDispPartACS&&pView->m_curAppType==TA_ASSEMBLE_ENV&&((CLDSPart*)pDbObj)->IsPlate())
			{
				g_pSolidDraw->BatchClearCS(4);
				g_pSolidDraw->AddCS(pDbObj->handle,((CLDSPart*)pDbObj)->ucs);
			}
		}
		else //if(pDbObj->IsNode())
		{
			CLDSNode *pNode=(CLDSNode*)pDbObj;
			scope.VerifyVertex(pNode->Position(false));
			scope.fMinX-=500;
			scope.fMaxX+=500;
			scope.fMinY-=500;
			scope.fMaxY+=500;
			scope.fMinZ-=500;
			scope.fMaxZ+=500;
		}
		g_pSolidOper->FocusTo(scope,g_sysPara.m_fMaxScaleScr2User);
		f3dPoint center;
		g_pSolidSet->SetOperType(OPER_ROTATE);
		g_pSolidSet->SetZoomStyle(ROT_CENTER);
		scope.centre(&center.x,&center.y,&center.z);
		g_pSolidSet->SetRotOrg(center);
		g_pSolidDraw->Draw();
	}
}
void CSearchPartDlg::ContextMenu(CWnd *pWnd, CPoint point)
{
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP); //���ز˵���
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
#ifdef AFX_TARG_ENU_ENGLISH
	pMenu->AppendMenu(MF_STRING,ID_SELECT_ITEM,"location");
#else
	pMenu->AppendMenu(MF_STRING,ID_SELECT_ITEM,"��λ");
#endif
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point .x, point .y,this);
}
void CSearchPartDlg::ResetDlgSize()
{
	if(m_bSearchResult)
		SpreadListCtrl();
	else
		HideListCtrl();
}
void CSearchPartDlg::InitListCtrl(long hDatumSelPart/*=0*/)
{
	m_listCtrl.DeleteAllItems();
	CStringArray str_arr;
	str_arr.SetSize(3);
	for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_BOLT||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE||
			pPart->GetClassTypeId()==CLS_POLYJOINT)//||pPart->m_bVirtualPart)
			continue;	//��˨�ޱ�Ų���Ԥ�����ż��
		//else if(pPart->IsLinePart())
		//{
		//	CLDSLinePart* pRod=(CLDSLinePart*)pPart;
		//	if(pRod->bHuoQuStart||pRod->bHuoQuEnd)
		//		continue;	//
		//}
		if(m_sSearchText.CompareNoCase(pPart->GetPartNo())!=0)
			continue;
		if(hDatumSelPart==0)
			hDatumSelPart=pPart->handle;
		str_arr[0].Format("0X%X",pPart->handle);
		if(hDatumSelPart==pPart->handle)
		{
			m_hDatumSelPart=hDatumSelPart;
			str_arr[0].AppendChar('*');
		}
		str_arr[1].Format("%s",pPart->Label); 
		if(pPart->IsAngle())
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[2]="Angle";
		else if(pPart->IsPlate())
			str_arr[2]="Plate";
		else if(pPart->GetClassTypeId()==CLS_LINETUBE)
			str_arr[2]="Tube";
		else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
			str_arr[2]="Flat";
		else if(pPart->GetClassTypeId()==CLS_LINESLOT)
			str_arr[2]="Slot";
#else
			str_arr[2]="�Ǹ�";
		else if(pPart->IsPlate())
			str_arr[2]="�ְ�";
		else if(pPart->GetClassTypeId()==CLS_LINETUBE)
			str_arr[2]="�ֹ�";
		else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
			str_arr[2]="����";
		else if(pPart->GetClassTypeId()==CLS_LINESLOT)
			str_arr[2]="�۸�";
#endif
		m_listCtrl.InsertItemRecord(-1,str_arr);    //�����¼
	}
}
void CSearchPartDlg::HideListCtrl()
{
	if(m_bHideListCtrl)
		return;	//��ǰ�����ʹ�������״̬�������ظ����� wjh-2015.4.12
	m_bHideListCtrl=true;
	CRect rc,list_rc;
	GetWindowRect(rc);
	GetDlgItem(IDC_LIST_CTRL)->GetWindowRect(list_rc);
	GetDlgItem(IDC_LIST_CTRL)->ShowWindow(SW_HIDE);
	((CButton*)GetDlgItem(IDC_CHK_SEARCH_RESULT))->SetCheck(FALSE);
	m_bSearchResult=FALSE;
	GetDlgItem(IDC_CHK_SEARCH_RESULT)->SetWindowText("+");
	rc.bottom-=list_rc.Height();
	MoveWindow(rc);
	CWnd *pSonWnd = GetDlgItem(ID_BTN_SEARCH);
	CRect rcSon;
	pSonWnd->GetWindowRect(rcSon);
	ScreenToClient(rcSon);
	rcSon.top-=list_rc.Height();
	rcSon.bottom-=list_rc.Height();
	pSonWnd->MoveWindow(rcSon);
}
void CSearchPartDlg::SpreadListCtrl()
{
	if(m_bHideListCtrl==false)
		return;		//��ǰ����չ��״̬�����ٴ�չ��
	CRect rc,list_rc;
	GetWindowRect(rc);
	GetDlgItem(IDC_LIST_CTRL)->GetWindowRect(list_rc);
	GetDlgItem(IDC_LIST_CTRL)->ShowWindow(TRUE);
	((CButton*)GetDlgItem(IDC_CHK_SEARCH_RESULT))->SetCheck(TRUE);
	m_bSearchResult=TRUE;
	GetDlgItem(IDC_CHK_SEARCH_RESULT)->SetWindowText("-");
	rc.bottom+=list_rc.Height();
	MoveWindow(rc);
	CWnd *pSonWnd = GetDlgItem(ID_BTN_SEARCH);
	CRect rcSon;
	pSonWnd->GetWindowRect(rcSon);
	ScreenToClient(rcSon);
	rcSon.top+=list_rc.Height();
	rcSon.bottom+=list_rc.Height();
	pSonWnd->MoveWindow(rcSon);
	m_bHideListCtrl=false;
}
//
void CSearchPartDlg::OnOK() 
{
	DestroyWindow();
}
void CSearchPartDlg::OnCancel() 
{
	DestroyWindow();	
}
BOOL CSearchPartDlg::Create()
{
	m_bHideListCtrl=false;
	return CDialog::Create(CSearchPartDlg::IDD);
}

void CSearchPartDlg::InitSearchPartDlg()
{
	m_bEnableWindowMoveListen=false;
	if(GetSafeHwnd()==0)
		Create();
	else
		OnInitDialog();
	UpdateData(FALSE);
}

void CSearchPartDlg::OnBnCheckPartno()
{
	PARTNO_CHK_CONTEXT context;
	context.sSpecifyPartNo.Copy(m_sSearchText);
	CMainFrame* pMainWnd=(CMainFrame*)theApp.m_pMainWnd;
	pMainWnd->CheckPartNoByScope(&context);
}
 
void CSearchPartDlg::OnNMClickTabSearchType(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData();
	int currSelI=m_tabSearchType.GetCurSel();
	int nextCurSelI=m_tabSearchType.GetCurFocus();
	if(m_sSearchText.GetLength()!=0)
		 SearchPart(FALSE,nextCurSelI);
	else 
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetPromptInfo("Please enter the search content!");
#else
		SetPromptInfo("������Ҫ��������!");
#endif
	}
	/* 
		���ܻ���Ϊ����ͬʱʧȥ�����ۺϲ���ʱĳЩ�����˫�����ѡ����ѡ�ۺ�)CLDSView::QuickFindPart�е�UpdatePropertyPage���û����
		CTabCtrl��ѡ��ҳ�л����ʸ�Ϊ�ֶ��л� wjh-2016.6.26
	*/
	if(currSelI!=nextCurSelI)
	{
		m_tabSearchType.SetCurSel(nextCurSelI);
		OnTcnSelchangeTabSearchType(pNMHDR,pResult);
	}
	*pResult = 0;
}
void CSearchPartDlg::OnTcnSelchangeTabSearchType(NMHDR *pNMHDR, LRESULT *pResult)
{
	CWnd* pCheckButton=GetDlgItem(IDC_BN_CHECK_PARTNO);
	CWnd* pUpdtPropBtn=GetDlgItem(IDC_BN_UPDATE_SAME_LABEL_PARTS_RELA_PROPERTY);
	CWnd* pSelectAllBtn=GetDlgItem(IDC_BN_SELECT_ALL);
	int	nItem=m_listCtrl.GetItemCount();
	if(pCheckButton)
	{
		if(m_tabSearchType.GetCurSel()==2&&m_bSearchResult&&nItem>0)	//����Ų���
			pCheckButton->ShowWindow(SW_SHOW);
		else
			pCheckButton->ShowWindow(SW_HIDE);
	}
	if(pUpdtPropBtn)
	{
		if(m_tabSearchType.GetCurSel()==2&&m_bSearchResult&&nItem>0)	//�����Ų���
			pUpdtPropBtn->EnableWindow(TRUE);
		else
			pUpdtPropBtn->EnableWindow(FALSE);
	}
	if(pSelectAllBtn)
	{
		if(m_tabSearchType.GetCurSel()==2&&m_bSearchResult&&nItem>0)	//����Ų���
			pSelectAllBtn->ShowWindow(SW_SHOW);
		else
			pSelectAllBtn->ShowWindow(SW_HIDE);
	}
	//else
	//	pCheckButton->ShowWindow(SW_HIDE);
	*pResult = 0;
}
void CSearchPartDlg::OnBnClickedBnUpdateSameLabelPartsRelaProperty()
{
	CLDSPart* pDatumPart=Ta.Parts.FromHandle(m_hDatumSelPart);
	if(pDatumPart==NULL)
		return;
	CLDSPart* pMirXPart=pDatumPart->GetMirPart(MIRMSG(1),pDatumPart->ucs.origin);
	CLDSPart* pMirYPart=pDatumPart->GetMirPart(MIRMSG(2),pDatumPart->ucs.origin);
	CLDSPart* pMirZPart=pDatumPart->GetMirPart(MIRMSG(3),pDatumPart->ucs.origin);
	int hits=0;
	for(int i=0;i<m_listCtrl.GetItemCount();i++)
	{
		CString sText=m_listCtrl.GetItemText(i,0);
		sText.Remove('*');
		long hPart=HexStrToLong(sText);
		if(hPart==m_hDatumSelPart)
			continue;
		bool rslt=false;
		CLDSPart* pPart=Ta.Parts.FromHandle(hPart);
		if(pMirXPart==pPart)
			rslt=pDatumPart->SyncMirPropTo(pPart,MIRMSG(1));
		else if(pMirYPart==pPart)
			rslt=pDatumPart->SyncMirPropTo(pPart,MIRMSG(2));
		else if(pMirZPart==pPart)
			rslt=pDatumPart->SyncMirPropTo(pPart,MIRMSG(3));
		else
		{
			RELATIVE_OBJECT* pRelaObj=pDatumPart->relativeObjs.GetValue(pPart->handle);
			if(pRelaObj)
				rslt=pDatumPart->SyncMirPropTo(pPart,pRelaObj->mirInfo);
			else
				rslt=pDatumPart->SyncMirPropTo(pPart);
		}
		if(rslt)
			hits++;
	}
	if(hits>0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetPromptInfo(CXhChar50("Update %d part!",hits));
#else
		SetPromptInfo(CXhChar50("�ɹ�����%d������!",hits));
#endif
	}
}
 HBRUSH CSearchPartDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	CWnd* pSearchResultWnd=GetDlgItem(IDC_STATI_SEARCH_RESULT);
	if(pSearchResultWnd->m_hWnd == pWnd->m_hWnd)
		pDC->SetTextColor(RGB(255,0,0));
	return hbr;
}
 void CSearchPartDlg::SetPromptInfo(const char* sValue)
 {
	 GetDlgItem(IDC_STATI_SEARCH_RESULT)->SetWindowText(sValue);
	 m_sPromptInfo=sValue;
 }

 void CSearchPartDlg::OnEnChangeEPartSearchText()
 {
	 SetPromptInfo("");
 }

 void CSearchPartDlg::OnBnClickedBnSelectAll()
 {
	 g_pSolidDraw->ReleaseSnapStatus();
	 //g_pSolidSet->SetDisplayType(DISP_SOLID);
	 for(int i=0;i<m_listCtrl.GetItemCount();i++)
	 {
		 long hCurPart=0;
		CString sText=m_listCtrl.GetItemText(i,0);
		sscanf(sText,"%X",&hCurPart);
		CLDSPart* pPart=console.FromPartHandle(hCurPart);
		if(pPart==NULL)
			continue;
 		g_pSolidDraw->SetEntSnapStatus(hCurPart);
 		g_pSolidSet->SetOperType(OPER_ROTATE);	//���ò�������Ϊ��ת
 		f3dPoint org=pPart->ucs.origin;
 		g_pSolidSet->SetRotOrg(org);
 		g_pSolidSet->SetZoomStyle(ROT_CENTER);	//����Ϊָ����ת��������
 	 }
	 //����������
	 CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
 	 pLDSView->UpdatePropertyPage();
	 OnOK();
 }
