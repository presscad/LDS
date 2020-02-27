//<LOCALE_TRANSLATE Confirm by hxr/>
// SteelMatLibDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "SteelMatLibDlg.h"
#include "PartLib.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "XHcharString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSteelMatLibDlg dialog

static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CSteelMatLibDlg *pSteelMatLibDlg=(CSteelMatLibDlg*)pListCtrl->GetParent();
	if(pSteelMatLibDlg==NULL)
		return FALSE;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	int nCurRow = pListCtrl->GetSelectedItem();
	int nCurCol = pListCtrl->GetCurSubItem();
	CRect rectItem;
	pListCtrl->GetItemRect(nCurRow,rectItem,LVIR_BOUNDS);
	if(!rectItem.PtInRect(point))
		return FALSE;	//鼠标不在当前选中项上
	if (nCurCol>=0)
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_PASTE_ITEM,"paste");
	if (nCurCol==0)
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"delete this spec");
#else
		pMenu->AppendMenu(MF_STRING,ID_PASTE_ITEM,"粘贴");
	if (nCurCol==0)
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除此规格");
#endif
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pSteelMatLibDlg);
	return TRUE;
}

static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
							int iSubItem,CString& sTextValue)
{
	CSteelMatLibDlg *pSteelMatLibDlg=(CSteelMatLibDlg*)pListCtrl->GetParent();
	if(pSteelMatLibDlg->m_iContentsType!=0&&(sTextValue==""||sTextValue=="0"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The data can not be empty");
#else
		AfxMessageBox("数据不能为空");
#endif
		return FALSE;
	}
	CString oldText = pSelItem->m_lpNodeInfo->GetSubItemText(iSubItem);
	if (oldText.CompareNoCase(sTextValue)==0)
		return FALSE;
	else if(pSelItem->m_idProp<=0)
		return FALSE;
#if defined(__LDS_)||defined(__TSA_)
	else if(pSteelMatLibDlg->m_iContentsType==1&&pSelItem->m_idProp<=(int)g_sysPara.InterpBetaZArr.Size())	//风振系数
	{
		if(iSubItem==1)	//
			g_sysPara.InterpBetaZArr[pSelItem->m_idProp-1].val_des=atof(sTextValue);
		else if(iSubItem==2)
			g_sysPara.InterpBetaZArr[pSelItem->m_idProp-1].val_std=atof(sTextValue);
		return TRUE;
	}
	else if(pSteelMatLibDlg->m_iContentsType==3&&pSelItem->m_idProp<=(int)g_sysPara.InterpIceLoadCoefArr.Size()+1)	//杆塔覆冰重力荷载系数表
	{
		//TODO: 随后两行代码加得有点不合理　wjh-2014.1.6
		//if(pSelItem->m_idProp==g_sysPara.InterpIceLoadCoefArr.Size()+1)
		//	g_sysPara.InterpIceLoadCoefArr.Resize(pSelItem->m_idProp);
		if(iSubItem==0)	//
			g_sysPara.InterpIceLoadCoefArr[pSelItem->m_idProp-1].pivot_key=atof(sTextValue);
		if(iSubItem==1)	//
			g_sysPara.InterpIceLoadCoefArr[pSelItem->m_idProp-1].val_des=atof(sTextValue);
		return TRUE;
	}
#endif
	else if(pSteelMatLibDlg->m_iContentsType==2&&pSelItem->m_idProp<10)	//角钢螺栓选择表
	{
		int i=1;
		BOLT_SELCOND *pPrevSelCond=NULL,*pNextSelCond=NULL;
		for(BOLT_SELCOND *pSelCond=Ta.listSelBoltCond.GetFirst();pSelCond;pSelCond=Ta.listSelBoltCond.GetNext(),i++)
		{
			if(i==pSelItem->m_idProp)
			{
				pSelCond->grade[4]=0;
				if(iSubItem==0)
					strncpy(pSelCond->grade,sTextValue,4);
				else if(iSubItem==1)
				{
					char dia_str[10];
					memset(dia_str,0,10);
					strncpy(dia_str,sTextValue,9);
					pSelCond->d=atoi(&dia_str[1]);
				}
				else if(iSubItem==2)
				{
					int lim=atoi(sTextValue);
					if(pPrevSelCond&&lim<=pPrevSelCond->upWidthLim)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("The latter wing width limit will be larger than the former!");
#else
						AfxMessageBox("后一肢宽界限应比前一值大!");
#endif
						return FALSE;
					}
					pSelCond->upWidthLim=lim;
					CString wideStr,nextWideStr;
					pNextSelCond=Ta.listSelBoltCond.GetNext();
					if(pPrevSelCond==NULL)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						wideStr.Format("wing width≤%d",lim);
						if(pNextSelCond)
							nextWideStr.Format("%d＜wing width≤%d",lim,pNextSelCond->upWidthLim);
#else
						wideStr.Format("肢宽≤%d",lim);
						if(pNextSelCond)
							nextWideStr.Format("%d＜肢宽≤%d",lim,pNextSelCond->upWidthLim);
#endif
					}
					else
					{
#ifdef AFX_TARG_ENU_ENGLISH
						wideStr.Format("%d＜wing width≤%d",pPrevSelCond->upWidthLim,lim);
						if(pNextSelCond)
							nextWideStr.Format("%d＜wing width≤%d",lim,pNextSelCond->upWidthLim);
#else
						wideStr.Format("%d＜肢宽≤%d",pPrevSelCond->upWidthLim,lim);
						if(pNextSelCond)
							nextWideStr.Format("%d＜肢宽≤%d",lim,pNextSelCond->upWidthLim);
#endif
					}
					pListCtrl->SetItemText(pSelItem->m_idProp-1,3,wideStr);
					if(pNextSelCond)
						pListCtrl->SetItemText(pSelItem->m_idProp,3,nextWideStr);
				}
				return TRUE;
			}
			else
				pPrevSelCond=pSelCond;
		}
	}
	int iMat=pSelItem->m_idProp/10-1;	//材料类型索引
	int iGroup=pSelItem->m_idProp%10;	//材料属性行号
	STEELMAT mat=CSteelMatLibrary::RecordAt(iMat);
	if(iGroup==0)	//材料基本信息行
	{
		if(iSubItem==1)	//简化标识符
		{
			char cPrevBriefMark=mat.cBriefMark;
			char cNowBriefMark=sTextValue[0];
			for(int i=0;i<CSteelMatLibrary::GetCount();i++)
			{
				if(cNowBriefMark==CSteelMatLibrary::RecordAt(i).cBriefMark)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("the simplified symbol of inputed material is repeated with existing materials");
#else
					AfxMessageBox("输入材料简化标识符与已有材料重复！");
#endif
					return FALSE;
				}
			}
			if(cNowBriefMark!=cPrevBriefMark)
			{
				for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
				{
					if(pPart->cMaterial==cPrevBriefMark)
						pPart->cMaterial=cNowBriefMark;
				}
			}
			mat.cBriefMark=cNowBriefMark;
			if(sTextValue.GetLength()>1)
				sTextValue=sTextValue.Left(1);
		}
		else if(iSubItem==2)	//默认材料质量等级
		{
			char cQualityLevel=0;
			if(sTextValue.IsEmpty()||!isalpha(sTextValue[0]))
			{
				cQualityLevel=0;
				if(!sTextValue.IsEmpty())
					AfxMessageBox("质量等级必须是字母或为空!");
				sTextValue="";
			}
			else
			{
				if(sTextValue.GetLength()>1)
					sTextValue=sTextValue.Left(1);
				cQualityLevel=sTextValue[0];
			}
			if(cQualityLevel!=mat.cPreferQualityLevel)
			{
				for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
				{
					if(pPart->IsBolt()||pPart->cMaterial!=mat.cBriefMark)
						continue;	//螺栓或不是当前材质
					if(pPart->m_cQualityLevel==mat.cPreferQualityLevel)
						pPart->m_cQualityLevel=cQualityLevel;
				}
			}
			mat.cPreferQualityLevel=cQualityLevel;
		}
		else if(iSubItem==3)	//弹性模量
			mat.E=atof(sTextValue);
		else if(iSubItem==4)	//价格系数
			mat.price_coef=atof(sTextValue);
	}
	else //if(iGroup>0)
	{
		if(iSubItem==1&&iGroup<4)		//厚度分组界限
		{
			//更新材料厚度分组界限值
			mat.t_lim[iGroup-1]=atoi(sTextValue);
			//更新当前行材料厚度分组名称
			CString sText;
			if(iGroup==1)	//第一组
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Format("group1(0＜t≤%d)",mat.t_lim[iGroup-1]);
			else //if(iGroup<=3)
				sText.Format("group%d(%d＜t≤%d)",iGroup,mat.t_lim[iGroup-2],mat.t_lim[iGroup-1]);
#else
				sText.Format("分组1(0＜t≤%d)",mat.t_lim[iGroup-1]);
			else //if(iGroup<=3)
				sText.Format("分组%d(%d＜t≤%d)",iGroup,mat.t_lim[iGroup-2],mat.t_lim[iGroup-1]);
#endif
			pListCtrl->SetSubItemText(pSelItem,0,sText);
			//更新下一行材料厚度分组名称
			CSuperGridCtrl::CTreeItem *pFindItem=pListCtrl->FindTreeItemById(pSelItem->m_idProp+1);
			if(pFindItem)
			{
				if(iGroup<3)
#ifdef AFX_TARG_ENU_ENGLISH
					sText.Format("group%d(%d＜t≤%d)",iGroup+1,mat.t_lim[iGroup-1],mat.t_lim[iGroup]);
				else
					sText.Format("group%d(%d＜t)",iGroup+1,mat.t_lim[iGroup-1]);
#else
					sText.Format("分组%d(%d＜t≤%d)",iGroup+1,mat.t_lim[iGroup-1],mat.t_lim[iGroup]);
				else
					sText.Format("分组%d(%d＜t)",iGroup+1,mat.t_lim[iGroup-1]);
#endif
				pListCtrl->SetSubItemText(pFindItem,0,sText);
			}
		}
		else if(iSubItem==2&&iGroup<=4)	//分组屈服强度
			mat.intensity[iGroup-1]=atof(sTextValue);
		else if(iSubItem==3&&iGroup<=4)	//分组拉压设计强度
			mat.intensity[4+iGroup-1]=atof(sTextValue);
		else if(iSubItem==4&&iGroup<=4)	//分组剪切强度
			mat.intensity[8+iGroup-1]=atof(sTextValue);
		else if(iSubItem==5&&iGroup<=4)	//分组孔壁承压强度
			mat.intensity[12+iGroup-1]=atof(sTextValue);
	}
	CSteelMatLibrary::SetRecordAt(iMat,mat);
	pListCtrl->SetSubItemText(pSelItem,iSubItem,sTextValue);
	return TRUE;
}

CSteelMatLibDlg::CSteelMatLibDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSteelMatLibDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSteelMatLibDlg)
	//}}AFX_DATA_INIT
	m_iContentsType=0;
}


void CSteelMatLibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSteelMatLibDlg)
	DDX_Control(pDX, IDC_LIST_STEEL_MAT_LIB, m_listSteelMat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSteelMatLibDlg, CDialog)
	//{{AFX_MSG_MAP(CSteelMatLibDlg)
	ON_COMMAND(ID_PASTE_ITEM, OnPasteData)
	ON_COMMAND(ID_DELETE_ITEM, OnDelData)
	ON_BN_CLICKED(IDC_BN_IMPORT, OnBnImport)
	ON_BN_CLICKED(IDC_BN_EXPORT, OnBnExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSteelMatLibDlg message handlers

void CSteelMatLibDlg::OnPasteData()
{
	m_listSteelMat.PasteDataFromClipboard(7,NULL,TRUE);
}

void CSteelMatLibDlg::OnDelData()
{
	int iCurItem = m_listSteelMat.GetSelectedItem();
	int nSumItem = m_listSteelMat.GetItemCount();
	if(nSumItem<2||nSumItem==iCurItem+1)
		return;
	for (int i = iCurItem;i<nSumItem;i++)
	{
		CSuperGridCtrl::CTreeItem* pItem = m_listSteelMat.GetTreeItem(i);
		pItem->m_idProp-=1;
	}
	m_listSteelMat.DeleteItem(iCurItem);
	m_listSteelMat.UpdateData();
}

BOOL CSteelMatLibDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_iContentsType==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Material Library Type");
		m_listSteelMat.AddColumnHeader("material name",150);
	#ifdef __PART_DESIGN_INC_
		m_listSteelMat.AddColumnHeader("identifier/group limit",170);	//mm
		m_listSteelMat.AddColumnHeader("E/fy(MPa)",90);	//yield strengthfy(MPa)
		m_listSteelMat.AddColumnHeader("price coefficient/f(MPa)",185);	//tension-compression strengthf(MPa)
		m_listSteelMat.AddColumnHeader("fv(MPa)",80);	//抗剪强度fv(MPa)
		m_listSteelMat.AddColumnHeader("fc(MPa)",80);	//孔壁承压强度fc(MPa)
	#else
		m_listSteelMat.AddColumnHeader("Identifier",170);	//mm
		m_listSteelMat.AddColumnHeader("Quality",80);
	#endif
#else
		SetWindowText("材料类型库");
		m_listSteelMat.AddColumnHeader("材料名称",150);
	#ifdef __PART_DESIGN_INC_
		m_listSteelMat.AddColumnHeader("标识符/分组界限",110);	//mm
		m_listSteelMat.AddColumnHeader("质量等级/fy(MPa)",120);	//屈服强度fy(MPa)
		m_listSteelMat.AddColumnHeader("E/f(MPa)",90);			//拉压强度f(MPa)
		m_listSteelMat.AddColumnHeader("价格系数/fv(MPa)",120);	//抗剪强度fv(MPa)
		m_listSteelMat.AddColumnHeader("fc(MPa)",80);	//孔壁承压强度fc(MPa)
	#else
		m_listSteelMat.AddColumnHeader("标识符",110);	//mm
		m_listSteelMat.AddColumnHeader("质量等级",80);
	#endif
#endif
		m_listSteelMat.InitListCtrl();
		m_listSteelMat.SetContextMenuFunc(FireContextMenu);
	}
#ifdef __PART_DESIGN_INC_
	else if(m_iContentsType==1)	//风振系数插值表
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("wind vibration coefficient interpolation table");
		m_listSteelMat.AddColumnHeader("height of interpolation points(m)");
		m_listSteelMat.AddColumnHeader("wind vibration coefficient values(tower)");	//mm
		m_listSteelMat.AddColumnHeader("wind vibration coefficient values(base)");	//yield strengthfy fy(MPa)
#else
		SetWindowText("风振系数插值表");
		m_listSteelMat.AddColumnHeader("插值点高度(m)");
		m_listSteelMat.AddColumnHeader("风振系数取值(杆塔)");	//mm
		m_listSteelMat.AddColumnHeader("风振系数取值(基础)");	//屈服强度fy(MPa)
#endif
		long col_wide_arr[3]={150,200,200};
		m_listSteelMat.InitListCtrl(col_wide_arr);
		//GetDlgItem(IDC_BN_EXPORT)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_BN_IMPORT)->ShowWindow(SW_HIDE);
	}
	else if(m_iContentsType==3)	//杆塔覆冰重力荷载系数表
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Tower ice gravity load coefficient tableTower ice gravity load coefficient table");
		m_listSteelMat.AddColumnHeader("The thickness of the ice(mm)");
		m_listSteelMat.AddColumnHeader("tower ice load increase coefficient");	
#else
		SetWindowText("杆塔覆冰重力荷载系数表");
		m_listSteelMat.AddColumnHeader("覆冰厚度(mm)");
		m_listSteelMat.AddColumnHeader("杆塔覆冰荷载增大系数");	
#endif
		long col_wide_arr[2]={200,200};
		m_listSteelMat.InitListCtrl(col_wide_arr);
		//GetDlgItem(IDC_BN_EXPORT)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_BN_IMPORT)->ShowWindow(SW_HIDE);
	}
#endif
	else if(m_iContentsType==2)	//螺栓选择条件表
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Angle steel bolt selection table");
		m_listSteelMat.AddColumnHeader("bolt grade");	//bolt grade
		m_listSteelMat.AddColumnHeader("bolt size");	//bolt size
		m_listSteelMat.AddColumnHeader("wing width limit");
		m_listSteelMat.AddColumnHeader("wing range");	//mm
#else
		SetWindowText("角钢螺栓选择表");
		m_listSteelMat.AddColumnHeader("螺栓级别");	//螺栓级别
		m_listSteelMat.AddColumnHeader("螺栓直径");	//螺栓规格
		m_listSteelMat.AddColumnHeader("肢宽界限值");
		m_listSteelMat.AddColumnHeader("肢宽范围");	//mm
#endif
		long col_wide_arr[4]={150,150,150,150};
		m_listSteelMat.InitListCtrl(col_wide_arr);
		GetDlgItem(IDC_BN_EXPORT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BN_IMPORT)->ShowWindow(SW_HIDE);
	}
	m_listSteelMat.SetModifyValueFunc(FireValueModify);
	Refresh();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void MatTypeSerialize(CString sFileName,BOOL bSave)
{
	FILE *fp=NULL;
	CXhChar200 lineText;
	if(bSave)	//保存
	{
		fp=fopen(sFileName,"wt");
		fprintf(fp,"%d\n",CSteelMatLibrary::GetCount());	//材料类型数量
		for(int i=0;i<CSteelMatLibrary::GetCount();i++)
		{
			STEELMAT mat=CSteelMatLibrary::RecordAt(i);
			lineText.Printf("%s %C %d %d %d %.0f %f",mat.mark,mat.cBriefMark,
				mat.t_lim[0],mat.t_lim[1],mat.t_lim[2],
				mat.E,mat.price_coef);
			SimplifiedNumString(lineText);
			lineText.Append(mat.cPreferQualityLevel,' ');
			fprintf(fp,"%s\n",(char*)lineText);
			for(int j=0;j<4;j++)
			{
				fprintf(fp,"%.0f %.0f %.0f %.0f\n",
					mat.intensity[j*4+0],mat.intensity[j*4+1],
					mat.intensity[j*4+2],mat.intensity[j*4+3]);
			}
		}
	}
	else	//读取
	{
		fp=fopen(sFileName,"rt");
		fgets(lineText,200,fp);
		int steelmat_N=4;
		sscanf(lineText,"%d",&steelmat_N);	//材料类型数量
		if(steelmat_N>20)
			steelmat_N=20;
		CSteelMatLibrary::SetCount(steelmat_N);
		for(int i=0;i<steelmat_N;i++)
		{
			STEELMAT mat=CSteelMatLibrary::RecordAt(i);
			char brief_mark[20]="";
			fgets(lineText,200,fp);
			CXhChar50 xarrWords[8];
			int count=DELIMITER_STR::ParseWordsFromStr(lineText,", \t",xarrWords,8);
			if(count>0)
				StrCopy(mat.mark,xarrWords[0],8);
			if(count>1)
				brief_mark[0]=xarrWords[1].At(0);
			if(count>2)
				mat.t_lim[0]=atoi(xarrWords[2]);
			if(count>3)
				mat.t_lim[1]=atoi(xarrWords[3]);
			if(count>4)
				mat.t_lim[2]=atoi(xarrWords[4]);
			if(count>5)
				mat.E=atof(xarrWords[5]);
			if(count>6)
				mat.price_coef=atof(xarrWords[6]);
			if(count>7)
				mat.cPreferQualityLevel=xarrWords[7].At(0);
			//sscanf(lineText,"%s %C %d %d %d %lf %lf",mat.mark,brief_mark,
			//	&mat.t_lim[0],&mat.t_lim[1],&mat.t_lim[2],
			//	&mat.E,&mat.price_coef);
			mat.cBriefMark=brief_mark[0];
			for(int j=0;j<4;j++)
			{
				fgets(lineText,200,fp);
				sscanf(lineText,"%lf %lf %lf %lf",
					&mat.intensity[j*4+0],&mat.intensity[j*4+1],
					&mat.intensity[j*4+2],&mat.intensity[j*4+3]);
			}
			CSteelMatLibrary::SetRecordAt(i,mat);
		}
	}
	if(fp!=NULL)
		fclose(fp);
}
void CSteelMatLibDlg::OnBnImport() 
{
	CString sFileName;
	if(this->m_iContentsType==0)
	{
		CFileDialog dlg(TRUE,"mat","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"material type file(*.mat)|*.mat||");
#else
			"材料类型文件(*.mat)|*.mat||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			MatTypeSerialize(sFileName,FALSE);	
		}
	}
#ifdef __PART_DESIGN_INC_
	else if(this->m_iContentsType==1)
	{
		CFileDialog dlg(TRUE,"mat","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"tower wind vibration coefficient interpolation table(*.wbz)|*.wbz||");
#else
			"杆塔风振系数插值表(*.wbz)|*.wbz||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			FILE* fp=fopen(dlg.GetPathName(),"rt");
			CXhChar100 lineTxt;
			DWORD i=0,N=0;
			while(!feof(fp)){
				fgets(lineTxt,100,fp);
				if(i==0&&lineTxt.GetLength()>0)
				{
					sscanf(lineTxt,"%d",&N);
					g_sysPara.InterpBetaZArr.Resize(N);
					i++;
				}
				else if(lineTxt.GetLength()>0)
				{
					sscanf(lineTxt,"%lf%lf%lf",&g_sysPara.InterpBetaZArr[i-1].pivot_key,
						&g_sysPara.InterpBetaZArr[i-1].val_des,&g_sysPara.InterpBetaZArr[i-1].val_std);
					if(i==g_sysPara.InterpBetaZArr.Size())
						break;
					i++;
				}
			}
			fclose(fp);
		}
	}
	else if(this->m_iContentsType==3)
	{
		CFileDialog dlg(TRUE,"mat","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"Tower ice gravity load coefficient tableTower ice gravity load coefficient table(*.iil)|*.iil||");
#else
			"杆塔覆冰重力荷载系数表(*.iil)|*.iil||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			FILE* fp=fopen(dlg.GetPathName(),"rt");
			CXhChar100 lineTxt;
			DWORD i=0,N=0;
			while(!feof(fp)){
				fgets(lineTxt,100,fp);
				if(i==0&&lineTxt.GetLength()>0)
				{
					sscanf(lineTxt,"%d",&N);
					g_sysPara.InterpIceLoadCoefArr.Resize(N);
					i++;
				}
				else if(lineTxt.GetLength()>0)
				{
					sscanf(lineTxt,"%lf%lf",&g_sysPara.InterpIceLoadCoefArr[i-1].pivot_key,
						&g_sysPara.InterpIceLoadCoefArr[i-1].val_des);
					if(i==g_sysPara.InterpIceLoadCoefArr.Size())
						break;
					i++;
				}
			}
			fclose(fp);
		}
	}
#endif
	Refresh();
}

void CSteelMatLibDlg::OnBnExport() 
{
	CString sFileName;
	if(this->m_iContentsType==0)
	{
		CFileDialog dlg(FALSE,"mat","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"material type file(*.mat)|*.mat||");
#else
			"材料类型文件(*.mat)|*.mat||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			MatTypeSerialize(sFileName,TRUE);	
		}
	}
#ifdef __PART_DESIGN_INC_
	else if(this->m_iContentsType==1)
	{
		CFileDialog dlg(FALSE,"mat","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"tower wind vibration coefficient interpolation table(*.wbz)|*.wbz||");
#else
			"杆塔风振系数插值表(*.wbz)|*.wbz||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			FILE* fp=fopen(sFileName,"wt");
			fprintf(fp,"%d\n",g_sysPara.InterpBetaZArr.Size());
			for(DWORD i=0;i<g_sysPara.InterpBetaZArr.Size();i++)
				fprintf(fp,"%8.3f %8.3f %8.3f\n",g_sysPara.InterpBetaZArr[i].pivot_key,
				g_sysPara.InterpBetaZArr[i].val_des,g_sysPara.InterpBetaZArr[i].val_std);
			fclose(fp);
		}
	}
	else if(this->m_iContentsType==3)
	{
		CFileDialog dlg(FALSE,"mat","Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
			"Tower ice gravity load coefficient tableTower ice gravity load coefficient table(*.iil)|*.iil||");
#else
			"杆塔覆冰重力荷载系数表(*.iil)|*.iil||");
#endif
		if(dlg.DoModal()==IDOK)
		{
			sFileName = dlg.GetPathName();
			FILE* fp=fopen(sFileName,"wt");
			fprintf(fp,"%d\n",g_sysPara.InterpIceLoadCoefArr.Size());
			for(DWORD i=0;i<g_sysPara.InterpIceLoadCoefArr.Size();i++)
				fprintf(fp,"%8.3f %8.3f\n",g_sysPara.InterpIceLoadCoefArr[i].pivot_key,
				g_sysPara.InterpIceLoadCoefArr[i].val_des);
			fclose(fp);
		}
	}
#endif
}

BOOL CSteelMatLibDlg::Refresh()
{
	CString sText,sError;
	try
	{
		if (m_listSteelMat.GetItemCount()>0)
			m_listSteelMat.DeleteAllItems();
		//CSuperGridCtrl::CTreeItem* pItemArr[100]={NULL};
		CSuperGridCtrl::CTreeItem* pItem=NULL,*pSubItem=NULL;
		CString itemText;
		if(m_iContentsType==0)
		{
			for (int i=0;i<CSteelMatLibrary::GetCount();i++)
			{
				STEELMAT mat=CSteelMatLibrary::RecordAt(i);
				CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
				lpRoot->SetSubItemColor(0,RGB(192,192,192));
				
				itemText.Format("%s",mat.mark);
				lpRoot->AddSubItemText(itemText,TRUE);
				//lpRoot->SetControlType(0,GCT_EDIT);
				pItem = m_listSteelMat.InsertRootItem(lpRoot);
				pItem->m_idProp=(i+1)*10;
				itemText.Format("%C",mat.cBriefMark);
				lpRoot->AddSubItemText(itemText,FALSE);
				itemText.Format("%C",mat.cPreferQualityLevel);
				lpRoot->AddSubItemText(itemText,FALSE);
#ifdef __PART_DESIGN_INC_
				itemText.Format("%f",mat.E);
				SimplifiedNumString(itemText);
				lpRoot->AddSubItemText(itemText,FALSE);
				itemText.Format("%f",mat.price_coef);
				SimplifiedNumString(itemText);
				lpRoot->AddSubItemText(itemText,FALSE);
				
				int prevThick=0;
				for(int j=0;j<4;j++)
				{
					//强度分组名称
					CListCtrlItemInfo *lpSubItemInfo=new CListCtrlItemInfo();
					lpSubItemInfo->SetSubItemColor(0,RGB(192,192,192));
					if(j<3)
#ifdef AFX_TARG_ENU_ENGLISH
						itemText.Format("group%d(%d＜t≤%d)",j+1,prevThick,mat.t_lim[j]);
					else
						itemText.Format("group%d(%d＜t)",j+1,prevThick);
#else
						itemText.Format("分组%d(%d＜t≤%d)",j+1,prevThick,mat.t_lim[j]);
					else
						itemText.Format("分组%d(%d＜t)",j+1,prevThick);
#endif
					prevThick=mat.t_lim[j];
					lpSubItemInfo->AddSubItemText(itemText,TRUE);
					//lpRoot->SetControlType(0,GCT_EDIT);
					pSubItem = m_listSteelMat.InsertItem(pItem,lpSubItemInfo);
					pSubItem->m_idProp=(i+1)*10+1+j;
					//分组厚度界限
					if(j<3)
					{
						itemText.Format("%d",mat.t_lim[j]);
						m_listSteelMat.SetSubItemText(pSubItem,1,itemText);
						pSubItem->m_lpNodeInfo->SetSubItemReadOnly(1,FALSE);
					}
					else
						pSubItem->m_lpNodeInfo->SetSubItemReadOnly(1,TRUE);
					//屈服强度
					itemText.Format("%f",mat.intensity[j]);
					SimplifiedNumString(itemText);
					m_listSteelMat.SetSubItemText(pSubItem,2,itemText);
					pSubItem->m_lpNodeInfo->SetSubItemReadOnly(2,FALSE);
					//拉压强度
					itemText.Format("%f",mat.intensity[4+j]);
					SimplifiedNumString(itemText);
					m_listSteelMat.SetSubItemText(pSubItem,3,itemText);
					pSubItem->m_lpNodeInfo->SetSubItemReadOnly(3,FALSE);
					//抗剪强度
					itemText.Format("%f",mat.intensity[8+j]);
					SimplifiedNumString(itemText);
					m_listSteelMat.SetSubItemText(pSubItem,4,itemText);
					pSubItem->m_lpNodeInfo->SetSubItemReadOnly(4,FALSE);
					//孔壁承压强度
					itemText.Format("%f",mat.intensity[12+j]);
					SimplifiedNumString(itemText);
					m_listSteelMat.SetSubItemText(pSubItem,5,itemText);
					pSubItem->m_lpNodeInfo->SetSubItemReadOnly(5,FALSE);
				}
#endif
			}
		}
#if defined(__LDS_)||defined(__TSA_)
		else if(m_iContentsType==1)
		{
			for (DWORD i=0;i<g_sysPara.InterpBetaZArr.Size();i++)
			{
				CListCtrlItemInfo* lpItemInfo= new CListCtrlItemInfo();
				lpItemInfo->SetSubItemColor(0,RGB(192,192,192));
				
				itemText.Format("%f",g_sysPara.InterpBetaZArr[i].pivot_key);
				SimplifiedNumString(itemText);
				lpItemInfo->AddSubItemText(itemText,TRUE);
				pItem = m_listSteelMat.InsertRootItem(lpItemInfo);
				pItem->m_idProp=i+1;
				itemText.Format("%f",g_sysPara.InterpBetaZArr[i].val_des);
				SimplifiedNumString(itemText);
				lpItemInfo->AddSubItemText(itemText,FALSE);
				itemText.Format("%f",g_sysPara.InterpBetaZArr[i].val_std);
				SimplifiedNumString(itemText);
				lpItemInfo->AddSubItemText(itemText,FALSE);
			}
		}
		else if(m_iContentsType==3)
		{
			DWORD n=(int)g_sysPara.InterpIceLoadCoefArr.Size();
			for(DWORD i=0;i<n;i++)
			{
				CListCtrlItemInfo* lpItemInfo= new CListCtrlItemInfo();
				itemText.Format("%f",g_sysPara.InterpIceLoadCoefArr[i].pivot_key);
				SimplifiedNumString(itemText);
				lpItemInfo->AddSubItemText(itemText,TRUE);
				pItem = m_listSteelMat.InsertRootItem(lpItemInfo);
				pItem->m_idProp=i+1;

				itemText.Format("%f",g_sysPara.InterpIceLoadCoefArr[i].val_des);
				lpItemInfo->AddSubItemText(itemText,FALSE);
				lpItemInfo->SetSubItemReadOnly(0,FALSE);
			}
			//TODO: 为什么要加下面这段代码，如此会导致界面上可以直接添加记录，但不能删除记录的结果　wjh-2014.1.6
			/*
			CListCtrlItemInfo* lpItemInfo= new CListCtrlItemInfo();
			itemText.Format(" ");
			SimplifiedNumString(itemText);
			lpItemInfo->AddSubItemText(itemText,TRUE);
			pItem = m_listSteelMat.InsertRootItem(lpItemInfo);
			pItem->m_idProp=n+1;
			itemText.Format(" ");
			SimplifiedNumString(itemText);
			lpItemInfo->AddSubItemText(itemText,FALSE);
			lpItemInfo->SetSubItemReadOnly(0,FALSE);
			*/
		}
#endif
		else //if(m_iContentsType==2)
		{
			BOLT_SELCOND *pPrevSelBolt=NULL,*pSelBolt=NULL;
			int i=1;
			for (pSelBolt=Ta.listSelBoltCond.GetFirst();pSelBolt;pSelBolt=Ta.listSelBoltCond.GetNext())
			{
				CListCtrlItemInfo* lpItemInfo= new CListCtrlItemInfo();
				lpItemInfo->SetSubItemColor(3,RGB(192,192,192));
				
				itemText.Format("%s",pSelBolt->grade);
				lpItemInfo->AddSubItemText(itemText,FALSE);
				lpItemInfo->SetControlType(0,GCT_CMB_LIST);
				lpItemInfo->SetListItemsStr(0,"4.8|6.8|8.8|10.9");
				pItem = m_listSteelMat.InsertRootItem(lpItemInfo);
				pItem->m_idProp=i;
				itemText.Format("M%d",pSelBolt->d);
				lpItemInfo->AddSubItemText(itemText,FALSE);
				lpItemInfo->SetControlType(1,GCT_CMB_LIST);
				lpItemInfo->SetListItemsStr(1,"M12|M16|M20|M24");
				itemText.Format("%d",pSelBolt->upWidthLim);
				lpItemInfo->AddSubItemText(itemText,FALSE);
				if(pPrevSelBolt==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					itemText.Format("≤%d",pSelBolt->upWidthLim);
				else
					itemText.Format("%d＜wing width≤%d",pPrevSelBolt->upWidthLim,pSelBolt->upWidthLim);
#else
					itemText.Format("≤%d",pSelBolt->upWidthLim);
				else
					itemText.Format("%d＜肢宽≤%d",pPrevSelBolt->upWidthLim,pSelBolt->upWidthLim);
#endif
				lpItemInfo->AddSubItemText(itemText,TRUE);
				i++;
				pPrevSelBolt=pSelBolt;
			}
		}
		m_listSteelMat.Redraw();
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	return TRUE;
}
