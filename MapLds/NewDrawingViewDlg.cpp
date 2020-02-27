//<LOCALE_TRANSLATE/>
// NewDrawingViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "NewDrawingViewDlg.h"
#include "Drawing.h"
#include "XhListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __SMART_DRAW_INC_
/////////////////////////////////////////////////////////////////////////////
// CNewDrawingViewDlg dialog

CNewDrawingViewDlg::CNewDrawingViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewDrawingViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewDrawingViewDlg)
	m_iType = 0;
	m_sName = _T("");
	m_sNo = _T("");
	m_iSeg = -2;
	//}}AFX_DATA_INIT
}


void CNewDrawingViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewDrawingViewDlg)
	DDX_Control(pDX, IDC_LIST_REMAIN_VIEW, m_listRight);
	DDX_Control(pDX, IDC_LIST_SELECT_VIEW, m_listLeft);
	DDX_Control(pDX, IDC_CMB_SEG, m_cmbSeg);
	DDX_CBIndex(pDX, IDC_CMB_TYPE, m_iType);
	DDX_Text(pDX, IDC_E_NAME, m_sName);
	DDX_Text(pDX, IDC_E_NO, m_sNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewDrawingViewDlg, CDialog)
	//{{AFX_MSG_MAP(CNewDrawingViewDlg)
	ON_BN_CLICKED(IDC_BN_DEL_VIEW, OnBnDelView)
	ON_BN_CLICKED(IDC_BN_ADD_VIEW, OnBnAddView)
	ON_LBN_DBLCLK(IDC_LIST_REMAIN_VIEW, OnDblclkListRemainView)
	ON_LBN_DBLCLK(IDC_LIST_SELECT_VIEW, OnDblclkListSelectView)
	ON_CBN_SELCHANGE(IDC_CMB_SEG, OnSelchangeCmbSeg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewDrawingViewDlg message handlers

BOOL CNewDrawingViewDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int i = 2;
#ifdef AFX_TARG_ENU_ENGLISH
	m_cmbSeg.InsertString(0,"general map");
	m_cmbSeg.SetItemData(0,-2);
	m_cmbSeg.InsertString(1,"force line map");
#else
	m_cmbSeg.InsertString(0,"总  图");
	m_cmbSeg.SetItemData(0,-2);
	m_cmbSeg.InsertString(1,"司令图");
#endif
	m_cmbSeg.SetItemData(1,-1);
	ATOM_LIST<SEGI> segNoList;
	int nSegN=Ta.GetSegNoList(segNoList);
	for(SEGI *pSegI=segNoList.GetFirst();pSegI;pSegI=segNoList.GetNext())
	{
		char sSegName[50]="";
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(sSegName,"%s segment",(char*)pSegI->ToString());
#else
		sprintf(sSegName,"%s段",(char*)pSegI->ToString());
#endif
		m_cmbSeg.InsertString(i,sSegName);
		m_cmbSeg.SetItemData(i,(DWORD)*pSegI);
		i++;
	}
	m_cmbSeg.SetCurSel(0);
	RefreshList(SEGI(-2));
	return TRUE;
}

void CNewDrawingViewDlg::RefreshList(SEGI iSeg)
{
	//CXhListBox *pViewRight=(CXhListBox*)GetDlgItem(IDC_LIST_REMAIN_VIEW);
	//将现有的drawingList图纸中的视图加进来
	for(CDrawing *pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
	{
		if(pDrawing->m_nSegI.iSeg==iSeg.iSeg)
		{
			for(CDrawingComponent *pComponent=pDrawing->GetFirstDrawingComponent();
				pComponent;pComponent=pDrawing->GetNextDrawingComponent())
			{
				DRAW_VIEW *pDrawView = m_drawViewList.append();
				pDrawView->dwDate = (DWORD)pComponent;
				pDrawView->iType = 0; //0.图纸列表中已存在的视图
				int iItem=m_listRight.AddString(pComponent->m_sName);
				char sText[50]="";
				m_listRight.GetText(iItem,sText);
				m_listRight.SetItemData(iItem,(DWORD)pDrawView);
			}
		}
	}
	//查找在塔中但不在drawingList中的视图
	for(CDisplayView *pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
	{
		if((stricmp(pView->defaultLayer,"SPQ")!=0||pView->filter.ThirdCharSet[0]=='?')
			&&(pView->m_iViewType!=0||stricmp(pView->defaultLayer,"SPY")!=0))
			continue;
		if(iSeg.iSeg==-2&&pView->m_iViewType!=0)
			continue;	//总图仅需要考虑图层视图
		bool bNewView = TRUE;
		if(iSeg.iSeg==-2 || iSeg.iSeg==-1)
		{
			for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
			{
				if(pDrawing->m_nSegI.iSeg!=iSeg.iSeg)
					continue;
				if(pDrawing->listComponents.GetValue(pView->handle-abs(iSeg.iSeg))!=NULL)
				{
					bNewView = FALSE;
					break;
				}
			}
			if(bNewView)
			{
				DRAW_VIEW *pDrawView = m_drawViewList.append();
				pDrawView->dwDate = (DWORD)pView;
				pDrawView->iType = 1;
				char sName[50];
				CLDSModule* pActiveModule = Ta.Module.FromHandle(Ta.m_hActiveModule);
				if(pActiveModule)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sName,"%s(%s activation)",pView->name,pActiveModule->description);
#else
					sprintf(sName,"%s(%s激活)",pView->name,pActiveModule->description);
#endif
				int iItem=m_listRight.AddString(sName);
				m_listRight.SetItemData(iItem,(DWORD)pDrawView);
			}
			if(pView->m_iViewType==0)
			{
				CLDSModule *pModule=NULL;
				for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
				{
					bNewView = TRUE;
					if(pModule->handle==Ta.m_hActiveModule)
						continue;
					for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
					{
						if(pDrawing->listComponents.GetValue(pView->handle+pModule->handle)!=NULL)
						{
							bNewView = FALSE;
							break;
						}
					}
					if(bNewView)
					{
						DRAW_VIEW *pDrawView = m_drawViewList.append();
						pDrawView->dwDate = (DWORD)pView;
						pDrawView->iType = 1;	//1.表示新加视图
						char sName[50];
						sprintf(sName,"%s(%s)",pView->name,pModule->description);
						int iItem=m_listRight.AddString(sName);
						m_listRight.SetItemData(iItem,(DWORD)pDrawView);
					}	
				}
			}
		}
		else
		{
			bNewView = TRUE;
			for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
			{
				if(pDrawing->m_nSegI.iSeg!=iSeg.iSeg)
					continue;
				if(pDrawing->listComponents.GetValue((pView->handle+iSeg.iSeg)%16+16)!=NULL)
				{
					bNewView = FALSE;
					break;
				}
			}
			if(bNewView)
			{
				DRAW_VIEW *pDrawView = m_drawViewList.append();
				pDrawView->dwDate = (DWORD)pView;
				pDrawView->iType = 1;	//1.表示新加视图
				int iItem=m_listRight.AddString(pView->name);
				m_listRight.SetItemData(iItem,(DWORD)pDrawView);
			}
		}
	}
}
void CNewDrawingViewDlg::OnBnAddView() 
{
	CXhListBox *pViewLeft=(CXhListBox*)GetDlgItem(IDC_LIST_SELECT_VIEW);
	CXhListBox *pViewRight=(CXhListBox*)GetDlgItem(IDC_LIST_REMAIN_VIEW);
	int nSel=m_listRight.GetSelCount();
	if(nSel==LB_ERR)	//无被选中项
		return;
	int *rgIndex=new int[nSel];
	nSel=m_listRight.GetSelItems(nSel,rgIndex);
	CString ss;
	CStringArray ss_arr;
	for(int i=0;i<nSel;i++)
	{
		m_listRight.GetText(rgIndex[i],ss);
		int iItem =m_listLeft.AddString(ss);
		DWORD dw = m_listRight.GetItemData(rgIndex[i]);
		m_listLeft.SetItemData(iItem,m_listRight.GetItemData(rgIndex[i]));
		ss_arr.Add(ss);
	}
	delete [] rgIndex;
	for(i=0;i<nSel;i++)
	{
		int ii=m_listRight.FindString(0,ss_arr[i]);
		if(ii!=LB_ERR)
			m_listRight.DeleteString(ii);
	}
}

void CNewDrawingViewDlg::OnBnDelView() 
{
	CXhListBox *pViewLeft=(CXhListBox*)GetDlgItem(IDC_LIST_SELECT_VIEW);
	CXhListBox *pViewRight=(CXhListBox*)GetDlgItem(IDC_LIST_REMAIN_VIEW);
	int nSel=m_listLeft.GetSelCount();
	if(nSel==LB_ERR)	//无被选中项
		return;
	int *rgIndex=new int[nSel];
	nSel=m_listLeft.GetSelItems(nSel,rgIndex);
	CString ss;
	CStringArray ss_arr;
	for(int i=0;i<nSel;i++)
	{
		m_listLeft.GetText(rgIndex[i],ss);
		int iItem =m_listRight.AddString(ss);
		m_listRight.SetItemData(iItem,m_listLeft.GetItemData(rgIndex[i]));
		ss_arr.Add(ss);
	}
	delete [] rgIndex;
	for(i=0;i<nSel;i++)
	{
		int ii=m_listLeft.FindString(0,ss_arr[i]);
		if(ii!=LB_ERR)
			m_listLeft.DeleteString(ii);
	}
}

void CNewDrawingViewDlg::OnDblclkListRemainView() 
{
	OnBnAddView();	
}

void CNewDrawingViewDlg::OnDblclkListSelectView() 
{
	OnBnDelView();
}

void CNewDrawingViewDlg::OnSelchangeCmbSeg() 
{
	UpdateData();
	CXhListBox *pViewRight=(CXhListBox*)GetDlgItem(IDC_LIST_REMAIN_VIEW);
	CXhListBox *pViewLeft=(CXhListBox*)GetDlgItem(IDC_LIST_SELECT_VIEW);
	while(m_listRight.GetCount()>0)
		m_listRight.DeleteString(0);
	while(m_listLeft.GetCount()>0)
		m_listLeft.DeleteString(0);
	m_iSeg = (int)m_cmbSeg.GetItemData(m_cmbSeg.GetCurSel());
	RefreshList(SEGI(m_iSeg));
}

void CNewDrawingViewDlg::OnOK() 
{
	UpdateData();
	//查找合适的插入位置，插入到同类图纸之后
	CDrawing *pDrawing=NULL,*pPrevDrawing=NULL;
	for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
	{
		if(pPrevDrawing==NULL)
			pPrevDrawing=pDrawing;
		else
		{	
			if(pPrevDrawing->m_nSegI.iSeg==m_iSeg&&pDrawing->m_nSegI.iSeg!=m_iSeg)
				break;
		}
		pPrevDrawing=pDrawing;
	}
	CDrawing *pNewDrawing=NULL;
	if(pDrawing)
		pNewDrawing=drawingList.insert(0);
	else 
		pNewDrawing=drawingList.Add(0);
	pNewDrawing->m_nSegI = SEGI(m_iSeg);
	pNewDrawing->m_sDrawingName.Printf("%s",m_sName);
	pNewDrawing->m_sDrawingNo.Printf("%s",m_sNo);
	pNewDrawing->m_xMapFrm.style = (BYTE)m_iType;
	pNewDrawing->m_hActiveModule=Ta.m_hActiveModule;
	pNewDrawing->m_pActiveModule=Ta.Module.FromHandle(Ta.m_hActiveModule);
	CListBox *pViewLeft=(CListBox*)GetDlgItem(IDC_LIST_SELECT_VIEW);
	for(int i=0;i<pViewLeft->GetCount();i++)
	{
		DRAW_VIEW *pDraw = (DRAW_VIEW *)pViewLeft->GetItemData(i);
		if(pDraw->iType == 1)
		{
			CDisplayView *pView = (CDisplayView *)pDraw->dwDate;
			if(pView->m_iViewType==0)
			{
				for(CLDSModule* pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
				{
					char sText[50]="";
					pViewLeft->GetText(i,sText);
					char sName[50];
					if(pModule == Ta.GetActiveModule())
#ifdef AFX_TARG_ENU_ENGLISH
						sprintf(sName,"%s(%s激活)",pView->name,pModule->description);
#else
						sprintf(sName,"%s(%s激活)",pView->name,pModule->description);
#endif
					else
						sprintf(sName,"%s(%s)",pView->name,pModule->description);
					if(strcmp(sName,sText)==0)
						pNewDrawing->AddComponentByView(pView,pModule);
				}
			}
			else
				pNewDrawing->AddComponentByView(pView,NULL);
		}
		else
		{
			CDrawingComponent *pComponent = (CDrawingComponent *)pDraw->dwDate;
			if(pComponent->ParentDrawing() != pNewDrawing)
			{
				long key = pComponent->ParentDrawing()->listComponents.GetKey(pComponent);
				CDrawingComponent *pNewComponent = pNewDrawing->AddComponent(key,pComponent->m_eClassType);
				pNewComponent->CopyProperty(pComponent);
				//归属图纸不允许赋值更改 wjh-2014.6.30
				//pNewComponent->m_pDrawing = pNewDrawing;
				pComponent->ParentDrawing()->DeleteDrawingComponent(key);//listComponents.DeleteNode(key);
			}
		}
	}
	CDialog::OnOK();
}
#endif