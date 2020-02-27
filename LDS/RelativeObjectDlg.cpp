//<LOCALE_TRANSLATE BY hxr /> 2015-1-9
// RelativeObjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "RelativeObjectDlg.h"
#include "LDSView.h"
#include "env_def.h"
#include "dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRelativeObjectDlg dialog
static BOOL DblclkListSyncDetails(CXhListCtrl* pListCtrl,int iItem,int iSubItem)
{
	CRelativeObjectDlg *pDlg=(CRelativeObjectDlg*)pListCtrl->GetParent();
	if(pDlg==NULL)
		return FALSE;
	PROPLIST_TYPE* pListProps=pDlg->m_pWorkObj->GetSyncPropList();
	if(pListProps==NULL)
		return FALSE;
	long idItem=pListCtrl->GetItemData(iItem);
	PROPLIST_ITEM* pItem=pListProps->GetPropItem(idItem);
	if(pItem==NULL)
		return FALSE;
	if(pDlg->mpcl.GetBitState((WORD)(pItem->id-1)))
	{
		pDlg->mpcl.SetBitState((WORD)(pItem->id-1),false);
		pListCtrl->SetItemText(iItem,1,"");
	}
	else
	{
		pDlg->mpcl.SetBitState((WORD)(pItem->id-1),true);
		pListCtrl->SetItemText(iItem,1,"√");
	}
	return TRUE;
}

CRelativeObjectDlg::CRelativeObjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRelativeObjectDlg::IDD, pParent)
	, m_bSyncMirPropDetails(FALSE)
{
	//{{AFX_DATA_INIT(CRelativeObjectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pWorkObj = NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	m_listCtrl.AddColumnHeader("Handle",58);
	m_listCtrl.AddColumnHeader("Mirror Style",166);
	m_listCtrl.AddColumnHeader("Rotate Angle",100);
	m_listCtrl.AddColumnHeader("Mirror Datum Point",145);
	m_listCtrl.AddColumnHeader("Part No.",72);
	m_listCtrl.AddColumnHeader("Exclude",70);
	m_listSyncDetails.AddColumnHeader("Property Name",200);
	m_listSyncDetails.AddColumnHeader("State",60);
#else 
	m_listCtrl.AddColumnHeader("句柄",50);
	m_listCtrl.AddColumnHeader("对称方式",70);
	m_listCtrl.AddColumnHeader("旋转角度",70);
	m_listCtrl.AddColumnHeader("对称基点",100);
	m_listCtrl.AddColumnHeader("构件编号",70);
	m_listCtrl.AddColumnHeader("排除",50);
	m_listSyncDetails.AddColumnHeader("属性名称",200);
	m_listSyncDetails.AddColumnHeader("状态",60);
#endif
	m_listCtrl.m_bEnableSortItems=TRUE;
	m_listCtrl.SetColumnSortType(4,SORTTYPE_PARTNO);
}


void CRelativeObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRelativeObjectDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHK_SYNC_MIR_PROP_DETAILS, m_bSyncMirPropDetails);
	DDX_Control(pDX, IDC_LIST_SYNC_PROP_DETAILS, m_listSyncDetails);
	DDX_Control(pDX, IDC_LIST_RELATIVE_OBJECT, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CRelativeObjectDlg, CDialog)
	//{{AFX_MSG_MAP(CRelativeObjectDlg)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RELATIVE_OBJECT, OnClickListRelativeObject)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RELATIVE_OBJECT, OnDblclkListRelativeObject)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_RELATIVE_OBJECT, OnKeydownListRelativeObject)
	ON_BN_CLICKED(ID_SYNCHRONIZE_RELATIVE_OBJ, &OnBnClickedSynchronizeRelativeObj)
	ON_BN_CLICKED(IDC_CHK_SYNC_MIR_PROP_DETAILS, &CRelativeObjectDlg::OnBnChkSyncMirPropDetails)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SYNC_PROP_DETAILS, &CRelativeObjectDlg::OnNMRClickListSyncPropDetails)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_RELATIVE_OBJECT, &CRelativeObjectDlg::OnNMRClickListRelativeObject)
	ON_COMMAND(ID_NEW_ITEM, OnSetSyncItem)
	ON_COMMAND(ID_DELETE_ITEM, OnRevokeSyncItem)
	ON_COMMAND(ID_SET_ACTIVE_ITEM, OnSetSyncDefault)
	ON_COMMAND(ID_ELIMINATE_ITEM, OnEliminateItem)
	ON_COMMAND(ID_CANCEL_ITEM,OnCancelEliminateItem)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRelativeObjectDlg message handlers

void CRelativeObjectDlg::DeleteItem(int nCurSel) 
{
	char tem_str[100];
	m_listCtrl.GetItemText(nCurSel,0,tem_str,100);
	long hCurPart=0;
	sscanf(tem_str,"%X",&hCurPart);
	if(m_pWorkObj==NULL)
		return;
	RELATIVE_OBJECT *pRelaObj=m_pWorkObj->relativeObjs.GetValue(hCurPart);
	if(pRelaObj)
	{
		CLDSDbObject *pObj=(CLDSDbObject*)console.FromHandle(pRelaObj->hObj,TRUE);
		if(pObj!=NULL)
			pObj->feature=10;	//暂做删除标记,等用户最终确认时再一并删除
	}
	m_listCtrl.DeleteItem(nCurSel);
}

void CRelativeObjectDlg::ModifyMirInfo(int iCurSel) 
{	//修改关联构件对称信息
	CMirMsgDlg mirmsgDlg;
	char tem_str[100];
	m_listCtrl.GetItemText(iCurSel,0,tem_str,100);
	long hCurPart=0;
	sscanf(tem_str,"%X",&hCurPart);
	if(m_pWorkObj==NULL)
		return;
	for(RELATIVE_OBJECT *pRelaObj=m_pWorkObj->relativeObjs.GetFirst();pRelaObj;pRelaObj=m_pWorkObj->relativeObjs.GetNext())
	{
		if(pRelaObj->hObj!=hCurPart)
			continue;
		mirmsgDlg.mirmsg = pRelaObj->mirInfo;
		if(IDOK==mirmsgDlg.DoModal())
		{
			CString tempStr;
			pRelaObj->mirInfo = mirmsgDlg.mirmsg;
			if(pRelaObj->mirInfo.axis_flag&1)
#ifdef AFX_TARG_ENU_ENGLISH
				tempStr.Format("X Mirror");
			else if(pRelaObj->mirInfo.axis_flag&2)
				tempStr.Format("Y Mirror");
			else if(pRelaObj->mirInfo.axis_flag&4)
				tempStr.Format("Z Mirror");
			else if(pRelaObj->mirInfo.axis_flag&8)
				tempStr.Format("Rotate Mirror");
			if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&1))
				tempStr.Format("X Mirror|Rotate Mirror");		//X轴对称后再旋转对称
			else if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&2))
				tempStr.Format("Y Mirror|Rotate Mirror");		//Y轴对称后再旋转对称
			else if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&4))
				tempStr.Format("Z Mirror|Rotate Mirror");		//Z轴对称后再旋转对称
#else 
				tempStr.Format("X轴对称");
			else if(pRelaObj->mirInfo.axis_flag&2)
				tempStr.Format("Y轴对称");
			else if(pRelaObj->mirInfo.axis_flag&4)
				tempStr.Format("Z轴对称");
			else if(pRelaObj->mirInfo.axis_flag&8)
				tempStr.Format("旋转对称");

			if((pRelaObj->mirInfo.axis_flag&8)==0)
				pRelaObj->mirInfo.rotate_angle=0;
			if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&1))
				tempStr.Format("X轴对称|旋转对称");		//X轴对称后再旋转对称
			else if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&2))
				tempStr.Format("Y轴对称|旋转对称");		//Y轴对称后再旋转对称
			else if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&4))
				tempStr.Format("Z轴对称|旋转对称");		//Z轴对称后再旋转对称
#endif
			m_listCtrl.SetItemText(iCurSel,1,tempStr);	
			if(pRelaObj->mirInfo.axis_flag&8)
				tempStr.Format("%.2f",pRelaObj->mirInfo.rotate_angle);
			else 
				tempStr="0";
			m_listCtrl.SetItemText(iCurSel,2,tempStr);	
			CXhChar100 coordStr;
			coordStr.ConvertFromPoint(pRelaObj->mirInfo.origin);
			m_listCtrl.SetItemText(iCurSel,3,coordStr);	
		}
		break;
	}
}

void CRelativeObjectDlg::OnDblclkListRelativeObject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_listCtrl.GetNextSelectedItem(pos);
		if(iCurSel>=0)
			ModifyMirInfo(iCurSel);
	}
	*pResult = 0;
}

void CRelativeObjectDlg::OnKeydownListRelativeObject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_listCtrl.GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_F3)
		{
			if(iCurSel>=0)
				ModifyMirInfo(iCurSel);
		}
		else if(pLVKeyDow->wVKey==VK_DELETE)
		{
			while(iCurSel>=0)
			{
				DeleteItem(iCurSel);
				pos=m_listCtrl.GetFirstSelectedItemPosition();
				if(pos==NULL)
					break;
				iCurSel=m_listCtrl.GetNextSelectedItem(pos);
			}
		}
	}
	*pResult = 0;
}

BOOL CRelativeObjectDlg::OnInitDialog() 
{
	if(m_pWorkObj==NULL)
		return FALSE;
	if(m_pWorkObj->GetClassTypeId()==CLS_NODE)
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Relative Node");
	else
		SetWindowText("Relative Part");
#else 
		SetWindowText("关联节点");
	else
		SetWindowText("关联构件");
#endif
	if(!CDialog::OnInitDialog())
		return FALSE;
	m_listCtrl.InitListCtrl();
	for(RELATIVE_OBJECT *pRelaObj=m_pWorkObj->relativeObjs.GetFirst();pRelaObj;pRelaObj=m_pWorkObj->relativeObjs.GetNext())
	{
		CLDSDbObject *pRelativeObj=(CLDSDbObject*)console.FromHandle(pRelaObj->hObj,TRUE);
		if(pRelativeObj==NULL)	//不存在该构件
		{
			m_pWorkObj->relativeObjs.DeleteCursor();
			continue;
		}
		pRelativeObj->feature=0;	//清零状态，以便启用后续的删除标记　wjh-2015.2.15
		CString tempStr;
		CStringArray str_arr;
		tempStr.Format("0X%X",pRelaObj->hObj);
		str_arr.Add(tempStr);
		if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&1))
#ifdef AFX_TARG_ENU_ENGLISH
			tempStr.Format("X Mirror|Rotate Mirror");		//X轴对称后再旋转对称
		else if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&2))
			tempStr.Format("Y Mirror|Rotate Mirror");		//Y轴对称后再旋转对称
		else if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&4))
			tempStr.Format("Z Mirror|Rotate Mirror");		//Z轴对称后再旋转对称
		else if(pRelaObj->mirInfo.axis_flag&1)
			tempStr.Format("X Mirror");
		else if(pRelaObj->mirInfo.axis_flag&2)
			tempStr.Format("Y Mirror");
		else if(pRelaObj->mirInfo.axis_flag&4)
			tempStr.Format("Z Mirror");
		else if(pRelaObj->mirInfo.axis_flag&8)
			tempStr.Format("Rotate Mirror");
#else 
			tempStr.Format("X轴对称|旋转对称");		//X轴对称后再旋转对称
		else if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&2))
			tempStr.Format("Y轴对称|旋转对称");		//Y轴对称后再旋转对称
		else if((pRelaObj->mirInfo.axis_flag&8)&&(pRelaObj->mirInfo.axis_flag&4))
			tempStr.Format("Z轴对称|旋转对称");		//Z轴对称后再旋转对称
		else if(pRelaObj->mirInfo.axis_flag&1)
			tempStr.Format("X轴对称");
		else if(pRelaObj->mirInfo.axis_flag&2)
			tempStr.Format("Y轴对称");
		else if(pRelaObj->mirInfo.axis_flag&4)
			tempStr.Format("Z轴对称");
		else if(pRelaObj->mirInfo.axis_flag&8)
			tempStr.Format("旋转对称");
#endif
		else
			tempStr="";
		str_arr.Add(tempStr);
		if(pRelaObj->mirInfo.axis_flag&8)
			tempStr.Format("%d",pRelaObj->mirInfo.rotate_angle);
		else 
			tempStr.Format("0",0);
		str_arr.Add(tempStr);
		CXhChar100 coordStr;
		coordStr.ConvertFromPoint(pRelaObj->mirInfo.origin);
		str_arr.Add(coordStr);
		if(!pRelativeObj->IsPart())
			str_arr.Add("");
		else
			str_arr.Add(((CLDSPart*)pRelativeObj)->GetPartNo());
		str_arr.Add("");
		//不加下面两行代码将无法实现按列排序　wjh-2015.4.12
		int iInsert=m_listCtrl.InsertItemRecord(-1,str_arr);
		m_listCtrl.SetItemData(iInsert,iInsert);
	}
	m_pWorkObj->relativeObjs.Clean();
	m_listSyncDetails.InitListCtrl();
	m_listSyncDetails.SetDblclkFunc(DblclkListSyncDetails);
	GetWindowRect(m_rcOriginal);
	OnBnChkSyncMirPropDetails();
	PROPLIST_TYPE* pListProps=m_pWorkObj->GetSyncPropList();
	if(pListProps!=NULL)
	{
		CStringArray str_arr;
		str_arr.SetSize(2);
		mpcl=m_pWorkObj->mpcl;
		if(mpcl.Size()<=4&&mpcl.IsZero())
			mpcl=m_pWorkObj->GetDefaultMPCL();
		for(PROPLIST_ITEM* pItem=pListProps->EnumFirst();pItem;pItem=pListProps->EnumNext())
		{
			str_arr[0]=(char*)pItem->name;
			if(mpcl.GetBitState((WORD)(pItem->id-1)))
				str_arr[1]="√";
			else
				str_arr[1]="";
			int iCurr=m_listSyncDetails.InsertItemRecord(-1,str_arr);
			if(iCurr>=0)
				m_listSyncDetails.SetItemData(iCurr,pItem->id);
		}
	};
	return TRUE;
}

void CRelativeObjectDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
	RECT rc;
	GetWindowRect(&rc);
	int height=m_rcOriginal.Height();
	int width=m_rcOriginal.Width();
	m_rcOriginal.left=rc.left;
	m_rcOriginal.right=rc.left+width;
	m_rcOriginal.top=rc.top;
	m_rcOriginal.bottom=rc.top+height;
}

void CRelativeObjectDlg::OnOK()
{
	char tem_str[100];
	if(mpcl==m_pWorkObj->GetDefaultMPCL())
		m_pWorkObj->mpcl.ReleaseBlockBuffer();
	else
		m_pWorkObj->mpcl=mpcl;
	for(RELATIVE_OBJECT *pRelaObj=m_pWorkObj->relativeObjs.GetFirst();pRelaObj;pRelaObj=m_pWorkObj->relativeObjs.GetNext())
	{
		CLDSDbObject *pObj=(CLDSDbObject*)console.FromHandle(pRelaObj->hObj,TRUE);
		if(pObj==NULL)
			continue;
		if(pObj->feature==10)
		{	//清除已删除的关联构件与当前构件的关联关系
			m_pWorkObj->relativeObjs.DeleteNode(pRelaObj->hObj);

			for(RELATIVE_OBJECT *pOtherRelaObj=pObj->relativeObjs.GetFirst();pOtherRelaObj;pOtherRelaObj=pObj->relativeObjs.GetNext())
			{
				if(pOtherRelaObj->hObj==m_pWorkObj->handle)
				{
					pObj->relativeObjs.DeleteCursor(TRUE);
					break;
				}
			}
		}
	}
	m_pWorkObj->ResetRelativeObjs();
	//关闭窗口前设置选中当前构件
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(m_pWorkObj->handle,TRUE);
	m_pWorkObj->relativeObjs.Empty();
	int i,n=m_listCtrl.GetItemCount();
	for(i=0;i<n;i++)
	{
		m_listCtrl.GetItemText(i,0,tem_str,100);
		long hCurPart=0;
		sscanf(tem_str,"%X",&hCurPart);
		RELATIVE_OBJECT *pRelaObj=m_pWorkObj->relativeObjs.Add(hCurPart);
		m_listCtrl.GetItemText(i,1,tem_str,100);
#ifdef AFX_TARG_ENU_ENGLISH
		if(strcmp(tem_str,"X Mirror")==0)
			pRelaObj->mirInfo.axis_flag=1;
		else if(strcmp(tem_str,"Y Mirror")==0)
			pRelaObj->mirInfo.axis_flag=2;
		else if(strcmp(tem_str,"Z Mirror")==0)
			pRelaObj->mirInfo.axis_flag=4;
		else if(strcmp(tem_str,"Rotate Mirror")==0)
			pRelaObj->mirInfo.axis_flag=8;
		else if(strcmp(tem_str,"X Mirror|Rotate Mirror")==0)
			pRelaObj->mirInfo.axis_flag=1|8;
		else if(strcmp(tem_str,"Y Mirror|Rotate Mirror")==0)
			pRelaObj->mirInfo.axis_flag=2|8;
		else if(strcmp(tem_str,"Z Mirror|Rotate Mirror")==0)
#else 
		if(strcmp(tem_str,"X轴对称")==0)
			pRelaObj->mirInfo.axis_flag=1;
		else if(strcmp(tem_str,"Y轴对称")==0)
			pRelaObj->mirInfo.axis_flag=2;
		else if(strcmp(tem_str,"Z轴对称")==0)
			pRelaObj->mirInfo.axis_flag=4;
		else if(strcmp(tem_str,"旋转对称")==0)
			pRelaObj->mirInfo.axis_flag=8;
		else if(strcmp(tem_str,"X轴对称|旋转对称")==0)
			pRelaObj->mirInfo.axis_flag=1|8;
		else if(strcmp(tem_str,"Y轴对称|旋转对称")==0)
			pRelaObj->mirInfo.axis_flag=2|8;
		else if(strcmp(tem_str,"Z轴对称|旋转对称")==0)
#endif
			pRelaObj->mirInfo.axis_flag=4|8;
		m_listCtrl.GetItemText(i,2,tem_str,100);
		pRelaObj->mirInfo.rotate_angle=atoi(tem_str);
		/*m_listCtrl.GetItemText(i,3,tem_str,100);
		pRelaObj->mirInfo.origin.x=atof(tem_str);
		m_listCtrl.GetItemText(i,4,tem_str,100);
		pRelaObj->mirInfo.origin.y=atof(tem_str);
		m_listCtrl.GetItemText(i,5,tem_str,100);
		pRelaObj->mirInfo.origin.z=atof(tem_str);*/
		//更新关联构件的同步属性信息	wxc-15.7.29
		CLDSDbObject* pRelaDbObj=Ta.FromNodeHandle(hCurPart);
		if(pRelaDbObj==NULL)
			pRelaDbObj=Ta.Parts.FromHandle(hCurPart);
		if(mpcl==pRelaDbObj->GetDefaultMPCL())
			pRelaDbObj->mpcl.ReleaseBlockBuffer();
		else
			pRelaDbObj->mpcl=mpcl;
	}
	CDialog::OnOK();
}

void CRelativeObjectDlg::SelectPart(int iCurSel) 
{	//选中构件
	char tem_str[100];
	m_listCtrl.GetItemText(iCurSel,0,tem_str,100);
	long hCurPart=0;
	sscanf(tem_str,"%X",&hCurPart);
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(hCurPart);
}

void CRelativeObjectDlg::OnClickListRelativeObject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_listCtrl.GetNextSelectedItem(pos);
		if(iCurSel>=0)
			SelectPart(iCurSel);
	}
	*pResult = 0;
}

void CRelativeObjectDlg::OnCancel() 
{	//关闭窗口前设置选中当前构件
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(m_pWorkObj->handle,TRUE);
	CDialog::OnCancel();
}
void CRelativeObjectDlg::OnBnClickedSynchronizeRelativeObj()
{
	if(m_pWorkObj)
	{
		//将同步属性信息临时赋值到所属构件中，完成同步更新操作后再还原 wxc-15.7.29
		MEMORY_BLOCK mcl,mir_mcl;
		mcl=m_pWorkObj->mpcl;
		m_pWorkObj->mpcl=mpcl;
		CLDSPlate *pWorkPlate=NULL;
		if(m_pWorkObj->GetClassTypeId()==CLS_PLATE)
			pWorkPlate=(CLDSPlate*)m_pWorkObj;
		for(int i=0;i<m_listCtrl.GetItemCount();i++)
		{
			CString exlusive=m_listCtrl.GetItemText(i,5);
			if(exlusive.Compare("×")==0)
				continue;
			CString hstr=m_listCtrl.GetItemText(i,0);
			long h=HexStrToLong(hstr);
			CLDSDbObject* pRelaDbObj=Ta.FromNodeHandle(h);
			if(pRelaDbObj==NULL)
				pRelaDbObj=Ta.Parts.FromHandle(h);
			RELATIVE_OBJECT* pRela=m_pWorkObj->relativeObjs.GetValue(h);
			if(pRelaDbObj&&pRela)
			{
				mir_mcl=pRelaDbObj->mpcl;
				pRelaDbObj->mpcl=mpcl;
				m_pWorkObj->SyncMirPropTo(pRelaDbObj,pRela->mirInfo);
				pRelaDbObj->mpcl=mir_mcl;
				//TODO:不清楚之前为什么添加以下两行代码，加了又为什么放在后面而不是同步之前,故此暂时注释 wjh-2019.3.27
				//if(pWorkPlate&&pWorkPlate->IsSyncMirPropWith(pRelaDbObj,"Profile"))
				//	pWorkPlate->DesignPlate();
			}
			//
			m_pWorkObj->mpcl=mcl;
		}
		if(pWorkPlate)
		{
			pWorkPlate->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pWorkPlate->GetSolidPartObject());
			g_pSolidDraw->Draw();
		}
	}
}

void CRelativeObjectDlg::OnBnChkSyncMirPropDetails()
{
	UpdateData();
	if(m_bSyncMirPropDetails)
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_CHK_SYNC_MIR_PROP_DETAILS)->SetWindowText("   Property synchronization update range->>");
	else
		GetDlgItem(IDC_CHK_SYNC_MIR_PROP_DETAILS)->SetWindowText("-V-Property synchronization update range-V-");
#else 
		GetDlgItem(IDC_CHK_SYNC_MIR_PROP_DETAILS)->SetWindowText("   属性同步更新范围->>");
	else
		GetDlgItem(IDC_CHK_SYNC_MIR_PROP_DETAILS)->SetWindowText("-V-属性同步更新范围-V-");
#endif
	if(m_bSyncMirPropDetails)
		MoveWindow(m_rcOriginal);
	else
	{
		CRect rcCtrl;
		CWnd* pListCtrl=GetDlgItem(IDC_LIST_SYNC_PROP_DETAILS);
		pListCtrl->GetWindowRect(rcCtrl);
		int height=rcCtrl.bottom-rcCtrl.top;
		MoveWindow(m_rcOriginal.left,m_rcOriginal.top,m_rcOriginal.Width(),m_rcOriginal.Height()-rcCtrl.Height()-4);
	}
	GetDlgItem(IDC_LIST_SYNC_PROP_DETAILS)->EnableWindow(m_bSyncMirPropDetails);
	int nCmdShow=m_bSyncMirPropDetails ? SW_SHOW : SW_HIDE;
	GetDlgItem(IDC_LIST_SYNC_PROP_DETAILS)->ShowWindow(nCmdShow);
}

//0.取消同步 1.设置同步 2.恢复默认值
void UpdateSelectedItemsState(CXhListCtrl *pListCtrl,BYTE cOper)
{
	CRelativeObjectDlg *pDlg=(CRelativeObjectDlg*)pListCtrl->GetParent();
	if(pDlg==NULL)
		return;
	PROPLIST_TYPE* pListProps=pDlg->m_pWorkObj->GetSyncPropList();
	if(pListProps==NULL)
		return;
	//
	bool bState=(cOper==0)?false:true;
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iItem=pListCtrl->GetNextSelectedItem(pos);
		long idItem=pListCtrl->GetItemData(iItem);
		PROPLIST_ITEM* pItem=pListProps->GetPropItem(idItem);
		if(pItem==NULL)
			continue;
		if(2==cOper)
			bState=pDlg->m_pWorkObj->GetDefaultMPCL().GetBitState((WORD)(pItem->id-1));
		pDlg->mpcl.SetBitState((WORD)(pItem->id-1),bState);
		if(!bState)
			pListCtrl->SetItemText(iItem,1,"");
		else 
			pListCtrl->SetItemText(iItem,1,"√");
	}
}

void CRelativeObjectDlg::OnSetSyncItem()
{
	UpdateSelectedItemsState(&m_listSyncDetails,1);
}
void CRelativeObjectDlg::OnRevokeSyncItem()
{
	UpdateSelectedItemsState(&m_listSyncDetails,0);
}
void CRelativeObjectDlg::OnSetSyncDefault()
{
	UpdateSelectedItemsState(&m_listSyncDetails,2);
}
void CRelativeObjectDlg::OnNMRClickListSyncPropDetails(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	//2.获取当前鼠标位置
	CPoint point;
	GetCursorPos(&point);
	//3.获取ListCtrl对应的矩形区域
	CRect rect;
	m_listSyncDetails.GetWindowRect(&rect);
	//4.在ListCtrl范围内点击右键时显示右键菜单
	if(rect.PtInRect(point))
	{
		CMenu popMenu;
		popMenu.LoadMenu(IDR_ITEM_CMD_POPUP); //加载菜单项
		popMenu.GetSubMenu(0)->DeleteMenu(0,MF_BYPOSITION);
#ifdef AFX_TARG_ENU_ENGLISH
		popMenu.GetSubMenu(0)->AppendMenu(MF_STRING,ID_NEW_ITEM,"Synchronous");
		popMenu.GetSubMenu(0)->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Cancel Synchronous");
		popMenu.GetSubMenu(0)->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"Default");
#else 
		popMenu.GetSubMenu(0)->AppendMenu(MF_STRING,ID_NEW_ITEM,"同步");
		popMenu.GetSubMenu(0)->AppendMenu(MF_STRING,ID_DELETE_ITEM,"取消同步");
		popMenu.GetSubMenu(0)->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"默认");
#endif
		//弹出右键菜单
		popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
			point.x,point.y,this);
	}
	*pResult = 0;
}
void CRelativeObjectDlg::OnEliminateItem()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iItem=m_listCtrl.GetNextSelectedItem(pos);
		if(iItem<0)
			continue;
		char sMark[4]="";
		m_listCtrl.GetItemText(iItem,5,sMark,4);
		if(stricmp(sMark,"×")==0)
			continue;
		m_listCtrl.SetItemText(iItem,5,"×");
	}
}
void CRelativeObjectDlg::OnCancelEliminateItem()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iItem=m_listCtrl.GetNextSelectedItem(pos);
		if(iItem<0)
			continue;
		char sMark[4]="";
		m_listCtrl.GetItemText(iItem,5,sMark,4);
		if(stricmp(sMark,"×")==0)
			m_listCtrl.SetItemText(iItem,5,"");
	}
}
void CRelativeObjectDlg::OnNMRClickListRelativeObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CPoint point;
	GetCursorPos(&point);
	CRect rect;
	m_listCtrl.GetWindowRect(&rect);
	if(rect.PtInRect(point))
	{
		CMenu popMenu;
		popMenu.LoadMenu(IDR_ITEM_CMD_POPUP); //加载菜单项
		popMenu.GetSubMenu(0)->DeleteMenu(0,MF_BYPOSITION);
#ifdef AFX_TARG_ENU_ENGLISH
		popMenu.GetSubMenu(0)->AppendMenu(MF_STRING,ID_ELIMINATE_ITEM,"Exclude");
		popMenu.GetSubMenu(0)->AppendMenu(MF_STRING,ID_CANCEL_ITEM,"Cancel Exclude");
#else 
		popMenu.GetSubMenu(0)->AppendMenu(MF_STRING,ID_ELIMINATE_ITEM,"排除");
		popMenu.GetSubMenu(0)->AppendMenu(MF_STRING,ID_CANCEL_ITEM,"取消排除");
#endif
		//弹出右键菜单
		popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
			point.x,point.y,this);
	}
	*pResult = 0;
}
