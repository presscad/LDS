//<LOCALE_TRANSLATE BY hxr /> 2015-04-24
// AddView.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AddViewDlg.h"
#include "Drawing.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __SMART_DRAW_INC_
/////////////////////////////////////////////////////////////////////////////
// CAddViewDlg dialog


CAddViewDlg::CAddViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddViewDlg::IDD, pParent)
{
	m_pDrawing=NULL;
}


void CAddViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddViewDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddViewDlg, CDialog)
	//{{AFX_MSG_MAP(CAddViewDlg)
	ON_BN_CLICKED(IDC_BN_ADD_VIEW, OnBnAddView)
	ON_BN_CLICKED(IDC_BN_DEL_VIEW, OnBnDelView)
	ON_LBN_DBLCLK(IDC_LIST_SELECT_VIEW, OnDblclkListSelectView)
	ON_LBN_DBLCLK(IDC_LIST_REMAIN_VIEW, OnDblclkListRemainView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddViewDlg message handlers

BOOL CAddViewDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CListBox *pViewLeft=(CListBox*)GetDlgItem(IDC_LIST_SELECT_VIEW);
	CListBox *pViewRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_VIEW);
	for(CDrawing *pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
	{
		if(pDrawing->m_nSegI.iSeg==m_pDrawing->m_nSegI.iSeg)
		{
			if(pDrawing == m_pDrawing)
			{
				for(CDrawingComponent *pComponent=pDrawing->GetFirstDrawingComponent();
					pComponent;pComponent=pDrawing->GetNextDrawingComponent())
				{
					int iItem=pViewLeft->AddString(pComponent->m_sName);
					DRAW_VIEW *pDrawView = itemInfoList.append(DRAW_VIEW(0,(DWORD)pComponent,(DWORD)pComponent->m_pModule));
					pViewLeft->SetItemData(iItem,(DWORD)pDrawView);
				}
			}
			else
			{
				for(CDrawingComponent *pComponent=pDrawing->GetFirstDrawingComponent();
					pComponent;pComponent=pDrawing->GetNextDrawingComponent())
				{
					int iItem=pViewRight->AddString(pComponent->m_sName);
					DRAW_VIEW *pDrawView = itemInfoList.append(DRAW_VIEW(0,(DWORD)pComponent,(DWORD)pComponent->m_pModule));
					pViewRight->SetItemData(iItem,(DWORD)pDrawView);
				}
			}
		}
	}
	SEGI iSeg = m_pDrawing->m_nSegI;
	for(CDisplayView *pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
	{
		if(pView->m_iViewType==0)
		{	//图层视图
			if((stricmp(pView->defaultLayer,"SPQ")!=0||pView->filter.ThirdCharSet[0]=='?')
				&&stricmp(pView->defaultLayer,"SPY")!=0
				&&stricmp(pView->defaultLayer,"SPZ")!=0
				&&stricmp(pView->defaultLayer,"SPH")!=0)
				continue;
		}
		if(iSeg.iSeg==-2&&pView->m_iViewType!=0)
			continue;	//总图不用考虑定制视图
		BOOL bHasView = FALSE;	//当前图纸中是否已存在该视图
		if(pView->m_iViewType==0)
		{
			if(iSeg.iSeg==-2 || iSeg.iSeg==-1)
			{
				//主体图
				for(CDrawing* pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
				{
					if(pDrawing->m_nSegI.iSeg!=iSeg.iSeg)
						continue;
					if(pDrawing->listComponents.GetValue(pView->handle-abs(iSeg.iSeg)))
					{	//查找段号与当前段号一致的图纸中是否存在该视图
						bHasView = TRUE;
						break;
					}
				}
				if(!bHasView)
				{
					CLDSModule* pActiveModule = Ta.Module.FromHandle(Ta.m_hActiveModule);
					if(pActiveModule)
					{
						char sName[50]="";
#ifdef AFX_TARG_ENU_ENGLISH
						sprintf(sName,"%s(%s activate)",pView->name,pActiveModule->description);
#else
						sprintf(sName,"%s(%s激活)",pView->name,pActiveModule->description);
#endif
						int iItem=pViewRight->AddString(sName);
						DRAW_VIEW *pDrawView = itemInfoList.append(DRAW_VIEW(1,(DWORD)pView,(DWORD)pActiveModule));
						pViewRight->SetItemData(iItem,(DWORD)pDrawView);
					}
				}
				//呼高图
				CLDSModule *pModule=NULL;
				for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
				{
					bHasView = FALSE;
					if(pModule->handle==Ta.m_hActiveModule)
						continue;
					for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
					{
						if(pDrawing->m_nSegI.iSeg!=iSeg.iSeg)
							continue;
						if(pDrawing->listComponents.GetValue(pView->handle+pModule->handle))
						{	
							bHasView = TRUE;
							break;
						}
					}
					if(!bHasView)
					{
						char sName[50];
						sprintf(sName,"%s(%s)",pView->name,pModule->description);
						int iItem=pViewRight->AddString(sName);
						DRAW_VIEW *pDrawView = itemInfoList.append(DRAW_VIEW(1,(DWORD)pView,(DWORD)pModule));
						pViewRight->SetItemData(iItem,(DWORD)pDrawView);
					}	
				}
			}
			else
			{
				bHasView = FALSE;
				for(CDrawing* pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
				{
					if(pDrawing->m_nSegI.iSeg!=iSeg.iSeg)
						continue;
					if(pDrawing->listComponents.GetValue((pView->handle+iSeg.iSeg)%16+16))
					{
						bHasView = TRUE;
						break;
					}
				}
				if(!bHasView)
				{
					int iItem=pViewRight->AddString(pView->name);
					DRAW_VIEW *pDrawView = itemInfoList.append(DRAW_VIEW(1,(DWORD)pView,0));
					pViewRight->SetItemData(iItem,(DWORD)pDrawView);
				}
			}
		}
		else if(pView->m_iViewType==1)
		{	//定制视图
			if(pView->m_iViewType==1&&iSeg.iSeg>0
				&&!(pView->GetSegI().iSeg==iSeg.iSeg||(SEGI(pView->folderName)).iSeg==iSeg.iSeg))
				continue;	//结构图非本段定制视图不显示
			for(CDrawing* pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
			{
				if(pDrawing->m_nSegI.iSeg!=iSeg.iSeg)
					continue;
				if(pDrawing->listComponents.GetValue(pView->handle))
				{	//查找段号与当前段号一致的图纸中是否存在该视图
					bHasView = TRUE;
					break;
				}
			}
			if(!bHasView)
			{
				int iItem=pViewRight->AddString(pView->name);
				DRAW_VIEW *pDrawView = itemInfoList.append(DRAW_VIEW(1,(DWORD)pView,0));
				pViewRight->SetItemData(iItem,(DWORD)pDrawView);
			}
		}
	}
	return TRUE;  
}

void CAddViewDlg::OnBnAddView() 
{
	CListBox *pViewLeft=(CListBox*)GetDlgItem(IDC_LIST_SELECT_VIEW);
	CListBox *pViewRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_VIEW);
	int nSel=pViewRight->GetSelCount();
	if(nSel==LB_ERR)	//无被选中项
		return;
	int *rgIndex=new int[nSel];
	nSel=pViewRight->GetSelItems(nSel,rgIndex);
	CString ss;
	CStringArray ss_arr;
	for(int i=0;i<nSel;i++)
	{
		pViewRight->GetText(rgIndex[i],ss);
		int iItem =pViewLeft->AddString(ss);
		DWORD dw = (DWORD)pViewRight->GetItemData(rgIndex[i]);
		pViewLeft->SetItemData(iItem,pViewRight->GetItemData(rgIndex[i]));
		ss_arr.Add(ss);
	}
	delete [] rgIndex;
	for(i=0;i<nSel;i++)
	{
		int ii=pViewRight->FindString(0,ss_arr[i]);
		if(ii!=LB_ERR)
			pViewRight->DeleteString(ii);
	}
}

void CAddViewDlg::OnBnDelView() 
{
	CListBox *pViewLeft=(CListBox*)GetDlgItem(IDC_LIST_SELECT_VIEW);
	CListBox *pViewRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_VIEW);
	int nSel=pViewLeft->GetSelCount();
	if(nSel==LB_ERR)	//无被选中项
		return;
	int *rgIndex=new int[nSel];
	nSel=pViewLeft->GetSelItems(nSel,rgIndex);
	CString ss;
	CStringArray ss_arr;
	for(int i=0;i<nSel;i++)
	{
		pViewLeft->GetText(rgIndex[i],ss);
		int iItem =pViewRight->AddString(ss);
		pViewRight->SetItemData(iItem,pViewLeft->GetItemData(rgIndex[i]));
		ss_arr.Add(ss);
	}
	delete [] rgIndex;
	for(i=0;i<nSel;i++)
	{
		int ii=pViewLeft->FindString(0,ss_arr[i]);
		if(ii!=LB_ERR)
			pViewLeft->DeleteString(ii);
	}
}

void CAddViewDlg::OnOK() 
{
	UpdateData();
	CListBox *pViewRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_VIEW);
	for(int i=0;i<pViewRight->GetCount();i++)
	{
		DRAW_VIEW *pDraw = (DRAW_VIEW *)pViewRight->GetItemData(i);
		if(pDraw->iType == 1)
			continue;
		else
		{
			CDrawingComponent *pComponent = (CDrawingComponent *)pDraw->dwView;
			if(pComponent->ParentDrawing() == m_pDrawing)
			{
				long key = m_pDrawing->listComponents.GetKey(pComponent);
				m_pDrawing->listComponents.DeleteNode(key);
			}
		}
	}
	CListBox *pViewLeft=(CListBox*)GetDlgItem(IDC_LIST_SELECT_VIEW);
	for(i=0;i<pViewLeft->GetCount();i++)
	{
		DRAW_VIEW *pDraw = (DRAW_VIEW *)pViewLeft->GetItemData(i);
		if(pDraw->iType == 1)
		{	//新增视图
			CDisplayView *pView = (CDisplayView *)pDraw->dwView;
			CLDSModule *pModule = (CLDSModule*)pDraw->dwModule;
			CDrawingComponent *pNewComponet=m_pDrawing->AddComponentByView(pView,pModule);
			if(pNewComponet)
			{	//新增视图默认放置于同类视图之后
				int i=0,iSameTypeCom=-1,iCurCom=m_pDrawing->listComponents.GetNodeNum()-1;
				CDrawingComponent *pCom=NULL;
				for(pCom=m_pDrawing->GetFirstDrawingComponent();pCom;pCom=m_pDrawing->GetNextDrawingComponent())
				{
					if(pCom!=pNewComponet&&pCom->m_eClassType==pNewComponet->m_eClassType)
					{
						if(pCom->m_eClassType==DRAW_COM_STRUVIEW)
						{	//结构图需要判断关联视图类型
							if(pCom->m_pAttachView&&pNewComponet->m_pAttachView
								&&pCom->m_pAttachView->m_iViewType==pNewComponet->m_pAttachView->m_iViewType)
								iSameTypeCom=i;
						}
						else 
							iSameTypeCom=i;	
					}
					i++;
				}
				if(iSameTypeCom!=-1&&iCurCom>iSameTypeCom)
				{	//调整当前视图位置至同类视图之类 wht 11-07-26
					for(i=0;i<(iCurCom-iSameTypeCom-1);i++)	
						m_pDrawing->listComponents.Move(pNewComponet,1);
				}
			}
		}
		else
		{	//原有视图移动位置
			CDrawingComponent *pComponent = (CDrawingComponent *)pDraw->dwView;
			if(pComponent->ParentDrawing() != m_pDrawing)
			{
				long key = pComponent->ParentDrawing()->listComponents.GetKey(pComponent);
				CDrawingComponent *pNewComponent = m_pDrawing->AddComponent(key,pComponent->m_eClassType);
				pNewComponent->CopyProperty(pComponent);
				//归属图纸不允许赋值更改 wjh-2014.6.30
				//pNewComponent->m_pDrawing = m_pDrawing;
				pComponent->ParentDrawing()->listComponents.DeleteNode(key);
			}
		}
	}
	CDialog::OnOK();
}

void CAddViewDlg::OnDblclkListSelectView() 
{
	OnBnDelView();	
}

void CAddViewDlg::OnDblclkListRemainView() 
{	
	OnBnAddView();
}
#endif