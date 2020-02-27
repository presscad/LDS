//<LOCALE_TRANSLATE/>
// SystemSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include ".\SystemSettingDlg.h"
#ifdef __DRAWING_CONTEXT_
#include "SysPara.h"
#include "AddNewLayerDlg.h"
#include "XhCharString.h"
#include "MenuFunc.h"
#include "RxTools.h"
#include "SpecialSettingDlg.h"
#include "NewMapFrameDlg.h"
#include "PropertyListOper.h"
#include "PlankPartParaExportDlg.h"
#include "LicFuncDef.h"
#include "LocalFeatureDef.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//�ص���������ʱ���ܺ�������
CString MakeMapFrameSizeSetString();	//��̬����ͼ��ߴ����͵�����������
CString MakeLayerSetString();			//��̬����ר��ͼ������������
CXhChar500 MakeFontNameStr();			//��̬����������������������
BOOL GetBitMap(const char* bmp_file,CBitmap& xBitMap);
BOOL ButtonClickSystemSetting(CPropertyList *pPropList,CPropTreeItem* pItem);
BOOL ModifySystemSettingValue(CPropertyList	*pPropList,CPropTreeItem *pItem, CString &valueStr);
BOOL FireSystemSettingHelpStr(CPropertyList* pPropList,CPropTreeItem* pItem);
//CSystemSettingDlg��̬��Ա��������
ARRAY_LIST<long> CSystemSettingDlg::m_xSpecialLayerIdArr;
int CSystemSettingDlg::m_iCurrentTabPage=0;
MAP_FRAME_SIZE* CSystemSettingDlg::m_pCurMapFrm=NULL;
/////////////////////////////////////////////////////////////////////////////
// CSystemSettingDlg dialog
CSystemSettingDlg::CSystemSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSystemSettingDlg::IDD, pParent)
{
	m_iDistType=0;
	m_nWorkPaneW=0;
	m_nMargin=8;
	m_pCurMapFrm=sys.m_hashMapFrameList.GetValue("A0");
}
void CSystemSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemSettingDlg)
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Control(pDX, IDC_TAB_GROUP, m_ctrlPropGroup);
	//}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(CSystemSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CSystemSettingDlg)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_GROUP, OnSelchangeTabGroup)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemSettingDlg message handlers
BOOL CSystemSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//��ʼ��TabCtrl
	m_ctrlPropGroup.DeleteAllItems();
#ifdef AFX_TARG_ENU_ENGLISH
	m_ctrlPropGroup.InsertItem(0,"General");
	m_ctrlPropGroup.InsertItem(1,"Line Map");
	m_ctrlPropGroup.InsertItem(2,"Structure Map ");
	m_ctrlPropGroup.InsertItem(3,"Part Map");
	m_ctrlPropGroup.InsertItem(4,"Weld Group Map");
	m_ctrlPropGroup.InsertItem(5,"Dimension");
	m_ctrlPropGroup.InsertItem(6,"Layer");
#else
	m_ctrlPropGroup.InsertItem(0,"����");
	m_ctrlPropGroup.InsertItem(1,"����ͼ");
	m_ctrlPropGroup.InsertItem(2,"�ṹͼ");
	m_ctrlPropGroup.InsertItem(3,"����ͼ");
	m_ctrlPropGroup.InsertItem(4,"�麸ͼ");
	m_ctrlPropGroup.InsertItem(5,"��ע");
	m_ctrlPropGroup.InsertItem(6,"ͼ��");
#endif
	m_ctrlPropGroup.SetCurSel(m_iCurrentTabPage);
	//��ʼ��ListCtrl
	m_propList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.m_iPropGroup=m_iCurrentTabPage;
	//
	CWnd *pWnd = GetDlgItem(IDC_WORK_PANEL);
	CRect rect;
	pWnd->GetWindowRect(&rect);
	m_nWorkPaneW=rect.Width();
#ifndef __BP_ONLY_
	m_xGraphManager.InitDrawingEnv(pWnd->GetSafeHwnd(),GetSafeHwnd());
#endif
	//
	Relayout();
	DisplaySystemSetting();
	UpdateData(FALSE);
	return TRUE;
}
void CSystemSettingDlg::RelayoutWnd()
{
	CRect rect,rcTop;
	GetWindowRect(&rect);
	int nWndH=rect.Height(),nWndW=rect.Width();
	int nClientW=0,nClientH=0;
	CWnd *pListCtrlWnd = GetDlgItem(IDC_LIST_BOX);
	if(pListCtrlWnd)
		pListCtrlWnd->GetWindowRect(&rcTop);
	nClientW=rcTop.Width();
	if(m_iDistType==1)
	{
		BITMAP bmInfo;
		m_bBitMap.GetBitmap(&bmInfo);
		nClientW+=bmInfo.bmWidth;
		nClientH=bmInfo.bmHeight;
	}
	else if(m_iDistType==2)
		nClientW+=m_nWorkPaneW;
	rect.right=rect.left+nClientW+4*m_nMargin;
	if(nClientH>(nWndH-2*m_nMargin))
		rect.bottom =rect.top+nClientH+2*m_nMargin;
	MoveWindow(rect,TRUE);
}
void CSystemSettingDlg::Relayout(BOOL bMoveMainWnd/*=TRUE*/)
{
	if(bMoveMainWnd)
		RelayoutWnd();
	CRect rcClient,rcHeader,rcTop,rcBtm,rcOk,rcClose;
	GetClientRect(rcClient);
	if(m_ctrlPropGroup.GetSafeHwnd())
		m_ctrlPropGroup.GetWindowRect(rcHeader);
	ScreenToClient(&rcHeader);
	CWnd *pTopWnd = GetDlgItem(IDC_LIST_BOX);
	if(pTopWnd)
		pTopWnd->GetWindowRect(&rcTop);
	ScreenToClient(&rcTop);
	CWnd *pBtmWnd = GetDlgItem(IDC_E_PROP_HELP_STR);
	if(pBtmWnd)
		pBtmWnd->GetWindowRect(&rcBtm);
	ScreenToClient(&rcBtm);
	CWnd *pOkBtn = GetDlgItem(IDOK);
	if(pOkBtn)
		pOkBtn->GetWindowRect(&rcOk);
	ScreenToClient(&rcOk);
	CWnd *pCloseBtn = GetDlgItem(IDCANCEL);
	if(pCloseBtn)
		pCloseBtn->GetWindowRect(&rcClose);
	ScreenToClient(&rcClose);
	//
	int nWidth=rcTop.Width(),nHeight=rcHeader.Height();
	rcHeader.left =rcTop.left =rcBtm.left =m_nMargin;
	rcHeader.right=rcTop.right=rcBtm.right=m_nMargin+nWidth;
	rcHeader.top	=m_nMargin;
	rcHeader.bottom =m_nMargin+nHeight;
	if(m_ctrlPropGroup.GetSafeHwnd())
		m_ctrlPropGroup.MoveWindow(&rcHeader);
	nHeight=rcOk.Height();
	rcOk.bottom =rcClose.bottom =rcClient.bottom-3;
	rcOk.top	=rcClose.top	=rcOk.bottom-nHeight;
	if(pOkBtn && pCloseBtn)
	{
		pOkBtn->MoveWindow(&rcOk);
		pCloseBtn->MoveWindow(&rcClose);
	}
	nHeight=rcBtm.Height();
	rcBtm.bottom =rcOk.top-3;
	rcBtm.top	 =rcBtm.bottom-nHeight;
	if(pBtmWnd)
		pBtmWnd->MoveWindow(&rcBtm);
	rcTop.top	 =rcHeader.bottom+1;
	rcTop.bottom =rcBtm.top-1;
	if(pTopWnd)
		pTopWnd->MoveWindow(&rcTop);
	//ʾ��ͼ����
	CWnd* pWorkPanel=GetDlgItem(IDC_WORK_PANEL);
	if(pWorkPanel)
	{
		if(m_iDistType==0)
			pWorkPanel->ShowWindow(FALSE);
		else
		{
			CRect rcPanel;
			rcPanel.left=rcTop.right+2;
			rcPanel.right=rcClient.right-4;
			rcPanel.top=rcHeader.top;
			rcPanel.bottom=rcOk.bottom;
			pWorkPanel->MoveWindow(rcPanel);
			pWorkPanel->ShowWindow(TRUE);
		}
	}
}
void CSystemSettingDlg::SetPromptPicture(long bitmapId)
{
	if(m_bBitMap.GetSafeHandle())
		m_bBitMap.DeleteObject();
	if(!m_bBitMap.LoadBitmap(bitmapId))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Please make sure the bitmap resources exist!");
#else 
		AfxMessageBox("��ȷ��λͼ��Դ����!");
#endif
		return;
	}
	m_iDistType=1;
	Relayout();
	Invalidate();
}
void CSystemSettingDlg::SetPromptPicture(const char* bmpfile)
{
	if(m_bBitMap.GetSafeHandle())
		m_bBitMap.DeleteObject();
	if(!GetBitMap(bmpfile,m_bBitMap))
		return;
	m_iDistType=1;
	Relayout();
	Invalidate();
}
void CSystemSettingDlg::SetOtherOper()
{
	m_iDistType=0;
	Relayout();
}
void CSystemSettingDlg::SetDrawingPara(DRAWING_PARA &draw_para)
{
	m_iDistType=2;
#ifndef __BP_ONLY_
	m_xGraphManager.InitDrawingPara(draw_para);
	g_pSolidDraw->BuildDisplayList(&m_xGraphManager);
#endif
	Relayout();
	Invalidate();
}
void CSystemSettingDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	Relayout(FALSE);
}
void CSystemSettingDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if(m_iDistType==1)
	{
		CRect rcPanel;
		CWnd *pWorkPanel=GetDlgItem(IDC_WORK_PANEL);
		pWorkPanel->GetClientRect(&rcPanel);
		CDC*  pDC=pWorkPanel->GetDC();	// ��ͼ�ؼ����豸������
		pDC->FillSolidRect(0,0,rcPanel.Width(),rcPanel.Height(),RGB(196,196,196));
		BITMAP bmInfo;
		m_bBitMap.GetBitmap(&bmInfo);
		CDC     memeroyDC;
		memeroyDC.CreateCompatibleDC(pDC);
		CBitmap* pOldMap=memeroyDC.SelectObject(&m_bBitMap);
		pDC->BitBlt(0,0,bmInfo.bmWidth,bmInfo.bmHeight,&memeroyDC,0,0,SRCCOPY);
		memeroyDC.SelectObject(pOldMap);
	}
#ifndef __BP_ONLY_
	else if(m_iDistType==2)
		g_pSolidDraw->Draw();
#endif
}
void CSystemSettingDlg::OnOK() 
{
	//�رնԻ���ʱ��ʼ����������,����ĳЩ�޸Ŀ���δ���ĵ�sys������ȥ
	//�����ʹ��ǰδ����sys.fonts.InitSpecialProp();�ᵼ������ʧЧ wht 11-04-28
	sys.fonts.InitSpecialProp(0);
	//ȷ������
	SysSetExportDefault(FALSE);
	sys.InitDrawingEnv();
	CDialog::OnOK();
}
void CSystemSettingDlg::OnCancel() 
{
	CDialog::OnCancel();
}
void CSystemSettingDlg::OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iCurSel = m_ctrlPropGroup.GetCurSel();
	m_iCurrentTabPage=m_propList.m_iPropGroup=iCurSel;
	m_propList.Redraw();
	*pResult = 0;
}
BOOL CSystemSettingDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	CXhChar200 helpStr=g_sHelpFilePath;
	if(HELPLINK_URL.GetLength()>0)
		helpStr.Append(CXhChar100("::/%s",(char*)HELPLINK_URL));
	CPropTreeItem* pPropItem=m_propList.GetSelectedItem();
	PROPLIST_ITEM* pPropHelpItem=NULL;
	if(pPropItem)
		pPropHelpItem=hashPropItemById.GetValue(pPropItem->m_idProp);
	if(pPropHelpItem!=NULL&&pPropHelpItem->Url.GetLength()>0)
		helpStr.Printf("%s::/%s",g_sHelpFilePath,(char*)pPropHelpItem->Url);
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle,helpStr,HH_DISPLAY_TOPIC,NULL)!= NULL;
	else
		return TRUE;
}
#endif
void CSystemSettingDlg::InitEnglishPropHashtable()
{
	int id = 1;
	//����
	AddPropItem("General",PROPLIST_ITEM(id++,"General"));
	AddPropItem("general_set.LineMapScale",PROPLIST_ITEM(id++,"line map scale 1:","line map drawing scale"));
	AddPropItem("general_set.ForceLineScale",PROPLIST_ITEM(id++,"force line map scale 1:",""));
	AddPropItem("general_set.StruMapScale",PROPLIST_ITEM(id++,"Structure map scale  1:",""));
	AddPropItem("general_set.StruMapScale.iBoltSymbolStyle",PROPLIST_ITEM(id++,"Structure map bolt hole processing mode","","0.round hole|1.symbol|2.scale symbol|3.scale diameter symbol"));
	AddPropItem("general_set.StruMapScale.fBoltSymbolFactor",PROPLIST_ITEM(id++,"scale factor","Scale map symbol rate factor/scale diameter map symbol scale factor"));
	AddPropItem("general_set.PartMapScale",PROPLIST_ITEM(id++,"part map scale  1:"));
	AddPropItem("general_set.PartMapScale.iBoltSymbolStyle",PROPLIST_ITEM(id++,"part map bolt hole processing mode","","0.round hole|1.symbol|2.scale symbol|3.scale diameter symbol"));
	AddPropItem("general_set.PartMapScale.fBoltSymbolFactor",PROPLIST_ITEM(id++,"scale factor","Scale map symbol rate factor/scale diameter map symbol scale factor"));
	AddPropItem("general_set.WeldMapScale",PROPLIST_ITEM(id++,"Weld group map scale 1:"));
	AddPropItem("general_set.WeldMapScale.iBoltSymbolStyle",PROPLIST_ITEM(id++,"Weld group map bolt hole processing mode","","0.round hole|1.symbol|2.scale symbol|3.scale diameter symbol"));
	AddPropItem("general_set.WeldMapScale.fBoltSymbolFactor",PROPLIST_ITEM(id++,"scale factor","Scale map symbol rate factor/scale diameter map symbol scale factor"));
	AddPropItem("general_set.iMatCharPosType",PROPLIST_ITEM(id++,"steel Material characters location","","0.not needed|1.before part number|2.after part number"));
	AddPropItem("general_set.cMatCharSeparator",PROPLIST_ITEM(id++,"steel Material Separator","","no|spacing"));
	AddPropItem("general_set.fHoleAugmentInNC",PROPLIST_ITEM(id++,"Hole Augment value"));
	AddPropItem("general_set.fWeldLineSymbolHigh",PROPLIST_ITEM(id++,"Weld line height"));
	AddPropItem("general_set.fWeldLineSymbolSpace",PROPLIST_ITEM(id++,"Weld line clearance"));
	AddPropItem("general_set.iDimPrecision",PROPLIST_ITEM(id++,"size accuracy","","1.0mm|0.5mm|0.1mm"));
	CXhChar500 strHelp("column widths are different for different style of parts list, adjust bolts list with parts list to ensure the overall width agreement.");
	AddPropItem("general_set.iPartBOMStyle",PROPLIST_ITEM(id++,"Parts list style",strHelp,"0.Standard Style|1.Broaden Style"));
	AddPropItem("general_set.iSumMatBOMStyle",PROPLIST_ITEM(id++,"Material summary table style","","0.Style 1|1.Style 2"));
	strHelp.Copy("dimension parts list, when using the part specification table to simplify marking, the variable control the number of part specifications to simplify the specification in the table. Maximum number is 26");
	AddPropItem("nBriefGuigeN",PROPLIST_ITEM(id++,"Simplify spec number",strHelp));
	AddPropItem("bDrawMainPoleJointLine",PROPLIST_ITEM(id++,"line map segment mark line","","YES|NO"));
	AddPropItem("nJointLineOffset",PROPLIST_ITEM(id++,"offset of segment mark line"));
	AddPropItem("nJointLineLen",PROPLIST_ITEM(id++,"length of segment mark line"));
	//���տ�·�������з�ʽ
	AddPropItem("ProcessCartAndLayout",PROPLIST_ITEM(id++,"Part process card and its arrangement"));
#ifdef __DRAWING_CONTEXT_
	if(sys.general_set.bCardLayoutBaseRow)
		strHelp.Copy("max columns in a row");	
	else
		strHelp.Copy("max rows in a column");
#endif
	AddPropItem("general_set.nPartRowsN",PROPLIST_ITEM(id++,strHelp));
	AddPropItem("general_set.nRowHigh",PROPLIST_ITEM(id++,"row height of parts map"));
	AddPropItem("general_set.nColumnWidth",PROPLIST_ITEM(id++,"column width of parts map"));
	AddPropItem("general_set.bCardLayoutBaseRow",PROPLIST_ITEM(id++,"process card arrangement style","","0.by the reference of row|1.by the reference of column"));
	AddPropItem("general_set.bHasFrame",PROPLIST_ITEM(id++,"Gutter","","YES|NO"));
	AddPropItem("general_set.bProcessCardTbl",PROPLIST_ITEM(id++,"Process card frame","","YES|NO"));
	AddPropItem("general_set.m_sJgCard",PROPLIST_ITEM(id++,"Angle process card"));
	AddPropItem("general_set.m_sSlotCard",PROPLIST_ITEM(id++,"U-steel process card"));
	AddPropItem("general_set.m_sTubeCard",PROPLIST_ITEM(id++,"Tube process card"));
	AddPropItem("general_set.m_sPlankCard",PROPLIST_ITEM(id++,"Plate process card"));
	AddPropItem("general_set.m_sWeldPartGroupCard",PROPLIST_ITEM(id++,"Weld parts process card"));
	AddPropItem("general_set.m_sProcessPartStatCard",PROPLIST_ITEM(id++,"Process part statistics card"));
	AddPropItem("MapArxPath",PROPLIST_ITEM(id++,"map modular path"));				
	AddPropItem("general_set.m_sTMAMapArxPath",PROPLIST_ITEM(id++,"TMA map modular path"));
	AddPropItem("general_set.m_sLMAMapArxPath",PROPLIST_ITEM(id++,"LMA map modular path"));
	AddPropItem("general_set.m_sLDSMapArxPath",PROPLIST_ITEM(id++,"LDS map modular path"));
	AddPropItem("general_set.m_sTSAMapArxPath",PROPLIST_ITEM(id++,"TSA map modular path"));
	//���ֻ��ƽ���������
	AddPropItem("digitalHandver",PROPLIST_ITEM(id++,"Digital Handver"));
	AddPropItem("general_set.lCrossarmCr",PROPLIST_ITEM(id++,"Left Crossarm Color"));
	AddPropItem("general_set.rCrossarmCr",PROPLIST_ITEM(id++,"Right Crossarm Color"));
	//��ͼ
	AddPropItem("LineMap",PROPLIST_ITEM(id++,"line map"));
	AddPropItem("LineMap.Font",PROPLIST_ITEM(id++,"font set"));
	//˾��ͼ
	AddPropItem("ForceLineMap",PROPLIST_ITEM(id++,"force line map"));
	AddPropItem("ForceLineMap.Font",PROPLIST_ITEM(id++,"font set"));
	//�ṹͼ
	AddPropItem("StruMap",PROPLIST_ITEM(id++,"structure map"));
	AddPropItem("stru_map.Font",PROPLIST_ITEM(id++,"font set"));
	AddPropItem("stru_map.bExportByEnglish",PROPLIST_ITEM(id++,"Construction Drawing Language","","0.Chinese|1.English"));
	AddPropItem("stru_map.fMinWingWide",PROPLIST_ITEM(id++,"Min angle leg width for drawing"));
	AddPropItem("stru_map.fMinPartThick",PROPLIST_ITEM(id++,"Min part thickness for drawing"));
	AddPropItem("stru_map.bDimSpecialLsSpace",PROPLIST_ITEM(id++,"dimension special bolts space","","YES|NO"));
	AddPropItem("stru_map.bDimSpecialJgZhun",PROPLIST_ITEM(id++,"dimension angle's special center distance","","YES|NO"));
	AddPropItem("stru_map.bDimStruJgLen",PROPLIST_ITEM(id++,"dimension angle length","Whether to dimension angle length in the structure map","YES|NO"));
	AddPropItem("stru_map.bDrawCutAngleAndWingInfo",PROPLIST_ITEM(id++,"Draw cut leg information","","YES|NO"));
	AddPropItem("stru_map.bOnlyDrawHorizontalPlate",PROPLIST_ITEM(id++,"just draw plate horizontally shown","��","YES|NO"));
	AddPropItem("stru_map.bUsePolyLineDrawEdge",PROPLIST_ITEM(id++,"draw outline edge with poly line","","Yes|No"));
	AddPropItem("stru_map.fPolyLineWidth",PROPLIST_ITEM(id++,"poly line width","poly line width"));
	AddPropItem("stru_map.PoleAxisLineOutSpreadLen",PROPLIST_ITEM(id++,"extending of rod heart line",""));
	AddPropItem("stru_map.fPoleAxisLineStartOutSpreadLen",PROPLIST_ITEM(id++,"Starting"));
	AddPropItem("stru_map.fPoleAxisLineEndOutSpreadLen",PROPLIST_ITEM(id++,"Ending"));
	AddPropItem("stru_map.bReadAdditionalBuffer",PROPLIST_ITEM(id++,"Read additional buffer","","YES|NO"));
	//ͼ������
	AddPropItem("m_pCurMapFrm.style",PROPLIST_ITEM(id++,"frame spec","frame specifications��"));
	AddPropItem("m_pCurMapFrm.nLength",PROPLIST_ITEM(id++,"frame length","frame length"));
	AddPropItem("m_pCurMapFrm.nWidth",PROPLIST_ITEM(id++,"frame width","frame width��"));
	AddPropItem("m_pCurMapFrm.nLeftMargin",PROPLIST_ITEM(id++,"left side space"));
	AddPropItem("m_pCurMapFrm.nOtherMargin",PROPLIST_ITEM(id++,"Other side space"));
	AddPropItem("m_pCurMapFrm.nBottomRightFixLen",PROPLIST_ITEM(id++,"fixed length of lower right side"));
	AddPropItem("m_pCurMapFrm.nBottomRightFixWidth",PROPLIST_ITEM(id++,"fixed height of lower right side"));
	//�麸ͼ
	AddPropItem("PartGroupMap",PROPLIST_ITEM(id++,"Weld group map"));
	AddPropItem("part_group_map.Font",PROPLIST_ITEM(id++,"font set"));
	AddPropItem("part_group_map.fMinFontSize",PROPLIST_ITEM(id++,"Min font height","Generate weld map, font scaling but must ensure the minimum height."));
	AddPropItem("part_group_map.fMinDrawThick",PROPLIST_ITEM(id++,"The thickness of the minimum value"));
	AddPropItem("part_group_map.nKmJgGIncrement",PROPLIST_ITEM(id++,"Explorator Angle G Increment","Explorator Angle G Increment"));
	AddPropItem("part_group_map.bDrawWeldLineSymbol",PROPLIST_ITEM(id++,"Drawing weld line","","YES|NO"));
	AddPropItem("part_group_map.iPlateLocateDrawStyle",PROPLIST_ITEM(id++,"Drawing steel sheet on the marauder","","0.Don't draw|1.Three vertical bar|2.The pennant"));
	AddPropItem("part_group_map.iLocateSymboleHeight",PROPLIST_ITEM(id++,"Drawing steel sheet height on the marauder"));
	AddPropItem("part_group_map.bAutoDimPartNo",PROPLIST_ITEM(id++,"automatically dimension parts No","","YES|NO"));
	AddPropItem("part_group_map.bAutoMatchView",PROPLIST_ITEM(id++,"automatically combine weld part views","","YES|NO"));
	AddPropItem("part_group_map.bModulatePartGroupLen",PROPLIST_ITEM(id++,"automatically adjust weld map length","","YES|NO"));
	AddPropItem("part_group_map.fPartGroupDistThreshold",PROPLIST_ITEM(id++,"scale threshold","scale threshold"));
	AddPropItem("part_group_map.fPartGroupDistZoomCoef",PROPLIST_ITEM(id++,"scale coefficient","weld map scale coefficient"));
	AddPropItem("part_group_map.bDimAbsoluteDist",PROPLIST_ITEM(id++,"locating point absolute size","","YES|NO"));
	AddPropItem("part_group_map.bDimAfterWeldCheckLine",PROPLIST_ITEM(id++,"size detection after welding","","YES|NO"));
	AddPropItem("part_group_map.bDimBeforeWeldLocatSpace",PROPLIST_ITEM(id++,"assembly location dimension before welding","","YES|NO"));
	AddPropItem("part_group_map.bDimPlatePoleRayAndAngle",PROPLIST_ITEM(id++,"ray material center line and angle","","YES|NO"));
	AddPropItem("part_group_map.bSmartFilterRedundantPart",PROPLIST_ITEM(id++,"automatically screen views and display parts","","YES|NO"));
	AddPropItem("part_group_map.bAngularDimension",PROPLIST_ITEM(id++,"dimension angle line","","YES|NO"));
	AddPropItem("part_group_map.bCreatePartGroupBOM",PROPLIST_ITEM(id++,"generate weld parts list","","YES|NO"));
	AddPropItem("welding.textsize.bZoomWithScale",PROPLIST_ITEM(id++,"font can zoom","","YES|NO"));
	AddPropItem("welding.textsize.wiScale",PROPLIST_ITEM(id++,"base zoom scale 1��"));
	//������ϸ��
	AddPropItem("part_group_map.part_table.bZoomInProcessCard",PROPLIST_ITEM(id++,"zoom in process card","","YES|NO"));
	AddPropItem("part_group_map.part_table.bDispPartGroupNo",PROPLIST_ITEM(id++,"weld part number","","YES|NO"));
	AddPropItem("part_group_map.part_table.bDispPartGroupNum",PROPLIST_ITEM(id++,"weld parts quantity","","YES|NO"));
	AddPropItem("part_group_map.part_table.bDispPartNo",PROPLIST_ITEM(id++,"part number","","YES|NO"));
	AddPropItem("part_group_map.part_table.bDispPartNum",PROPLIST_ITEM(id++,"part quantity","","YES|No"));
	AddPropItem("part_group_map.part_table.bDispPartGuiGe",PROPLIST_ITEM(id++,"part spec","","YES|NO"));
	AddPropItem("part_group_map.part_table.bDispPartLen",PROPLIST_ITEM(id++,"part length","","YES|NO"));
	AddPropItem("part_group_map.part_table.bDispPartWeight",PROPLIST_ITEM(id++,"weld parts group weight","","YES|NO"));
	AddPropItem("part_group_map.part_table.bDispNotes",PROPLIST_ITEM(id++,"notes","","YES|NO"));
	AddPropItem("part_group_map.part_table.bDispInternal",PROPLIST_ITEM(id++,"weld seam level - inner","","YES|NO"));
	AddPropItem("part_group_map.part_table.bDispAppearance",PROPLIST_ITEM(id++,"weld level - appearance","","YES|NO"));
	//����ͼ
	AddPropItem("PartMap",PROPLIST_ITEM(id++,"part map"));
	AddPropItem("part_map.iExportDeformedProfile",PROPLIST_ITEM(id++,"consider bending deformation effect","","0.Do not consider all|1.Consider all|2.Consider the details|3.Consider component diagram"));
	AddPropItem("part_map.bDimDeformedProfileText",PROPLIST_ITEM(id++,"dimension blending deformation words","","YES|NO"));
	AddPropItem("part_map.bDimPlaneAngle",PROPLIST_ITEM(id++,"dimension blending part angle between the both sides","","YES|NO"));
	AddPropItem("part_map.iDimAngleType",PROPLIST_ITEM(id++,"blending angle type","","0.Acute Angle|1.Obtuse Angle|2.Automatically"));
	AddPropItem("part_map.fMinHuoquAngle",PROPLIST_ITEM(id++,"min huoqu angle",""));
	AddPropItem("part_map.bDimHuoquType",PROPLIST_ITEM(id++,"dimension part blending type","","YES|NO"));
	AddPropItem("part_map.bUseZhengFanQun",PROPLIST_ITEM(id++,"dimension blending parts by front and reverse blending","��","YES|NO"));
	AddPropItem("part_map.iPartMapStyle",PROPLIST_ITEM(id++,"process mark symbols","","0.Style 1|1.Style 2"));
	//�Ǹֹ���ͼ����
	AddPropItem("AnglePartMap",PROPLIST_ITEM(id++,"angle part map"));
	AddPropItem("AnglePartMap.Font",PROPLIST_ITEM(id++,"font set"));
	AddPropItem("part_map.angle.bOneCardMultiPart",PROPLIST_ITEM(id++,"more than one part in a card","","Yes|No"));
	AddPropItem("part_map.angle.iJgZoomSchema",PROPLIST_ITEM(id++,"angle scaling scheme","","0.No zoom 1:1 drawing|1.Use part map scale|2.Zoom width and length to the same scale|3.Zoom length and width respectively"));
	AddPropItem("part_map.angle.ciJgLenDrawMode",PROPLIST_ITEM(id++,"angle len deal mode","","0.Extend To Right|1.Extrusion To middle"));
	AddPropItem("part_map.angle.bMaxExtendAngleLength",PROPLIST_ITEM(id++,"extend drawing length as far as possible","","0.No| 1.Yes"));
	AddPropItem("part_map.angle.iJgGDimStyle",PROPLIST_ITEM(id++,"dimension position of angle center distance","","0.Starting|1.Middle|2.Automatically judge"));
	AddPropItem("part_map.angle.nMaxBoltNumStartDimG",PROPLIST_ITEM(id++,"Max bolt quantity","Max bolt quantity"));
	AddPropItem("part_map.angle.iLsSpaceDimStyle",PROPLIST_ITEM(id++,"bolt space dimension style","","0.X axis direction|1.Y axis direction|2.Automatic judgment|3.Not dimension|4.Only dimension size line"));
	AddPropItem("part_map.angle.nMaxBoltNumAlongX",PROPLIST_ITEM(id++,"Max bolt quantity",""));
	AddPropItem("part_map.angle.bDimCutAngle",PROPLIST_ITEM(id++,"dimension cut angle size","","Yes|No"));
	AddPropItem("part_map.angle.bDimCutAngleMap",PROPLIST_ITEM(id++,"show cutting angle view","","Yes|No"));
	AddPropItem("part_map.angle.bDimPushFlatMap",PROPLIST_ITEM(id++,"dimension flat","","Yes|No"));
	AddPropItem("part_map.angle.bDimKaiHe",PROPLIST_ITEM(id++,"dimension open-close","","Yes|No"));
	AddPropItem("part_map.angle.bDimKaiheSumLen",PROPLIST_ITEM(id++,"dimension total length of open-close region","","Yes|No"));
	AddPropItem("part_map.angle.bDimKaiheAngle",PROPLIST_ITEM(id++,"dimension open-close degree","dimension open-close degree��","Yes|No"));
	AddPropItem("part_map.angle.bDimKaiheSegLen",PROPLIST_ITEM(id++,"dimension segment length of open-close region","","Yes|No"));
	AddPropItem("part_map.angle.bDimKaiheScopeMap",PROPLIST_ITEM(id++,"dimension identifier of open-close region","","Yes|No"));
	AddPropItem("part_map.angle.bJgUseSimpleLsMap",PROPLIST_ITEM(id++,"simplify bolt symbol","","Yes|No"));
	AddPropItem("part_map.angle.bDimLsAbsoluteDist",PROPLIST_ITEM(id++,"dimension bolt absolute size","","Yes|No"));
	AddPropItem("part_map.angle.bMergeLsAbsoluteDist",PROPLIST_ITEM(id++,"merge adjacent isometric bolt absolute size","","Yes|No"));
	AddPropItem("part_map.angle.bDimRibPlatePartNo",PROPLIST_ITEM(id++,"dimension angle rib plate number","","Yes|No"));
	AddPropItem("part_map.angle.bDimRibPlateMaterial",PROPLIST_ITEM(id++,"dimension angle rib plate Mat","","Yes|No"));
	AddPropItem("part_map.angle.bDimRibPlateSetUpPos",PROPLIST_ITEM(id++,"dimension angle rib plate assembly position","","Yes|No"));
	AddPropItem("part_map.angle.iCutAngleDimType",PROPLIST_ITEM(id++,"cutting angle dimension style","","0.Style 1|1.Styles 2"));
	AddPropItem("part_map.angle.fKaiHeJiaoThreshold",PROPLIST_ITEM(id++,"open-close threshold (��)"));
	AddPropItem("part_map.angle.fKaiHeJiaoMinDeita",PROPLIST_ITEM(id++,"Min security clearance of open-close"));
	//�ְ幹��ͼ����
	AddPropItem("PlateMap",PROPLIST_ITEM(id++,"plate part map"));
	AddPropItem("PlateMap.Font",PROPLIST_ITEM(id++,"font set"));
	AddPropItem("part_map.plate.bDimArcEdgeR",PROPLIST_ITEM(id++,"DimArcEdgeR","","Yes|No"));
	AddPropItem("part_map.plate.bDimArcAngle",PROPLIST_ITEM(id++,"DimArcAngle","","Yes|No"));
	AddPropItem("part_map.plate.bDimPlateRayLs",PROPLIST_ITEM(id++,"plate ray bolts space","","Yes|No"));
	AddPropItem("part_map.plate.bDimPlateBoltAngle",PROPLIST_ITEM(id++,"plate bolt angle","","Yes|No"));
	AddPropItem("part_map.plate.bDimBoltEdgeSpace",PROPLIST_ITEM(id++,"plate bolt angle","","Yes|No"));
	AddPropItem("part_map.plate.bDimDiffRayLsSpace",PROPLIST_ITEM(id++,"plate bolt angle","","Yes|No"));
	AddPropItem("part_map.plate.bDimLsToHuoquLine",PROPLIST_ITEM(id++,"plate bolt angle","","��|��"));
	AddPropItem("part_map.plate.bDimLsToWeldEdge",PROPLIST_ITEM(id++,"plate bolt angle","","��|��"));
	AddPropItem("part_map.plate.bDimPlateEdgeLen",PROPLIST_ITEM(id++,"dimension plate length","","Yes|No"));
	AddPropItem("part_map.plate.bDrawInnerEdge",PROPLIST_ITEM(id++,"draw inter edge","","Yes|No"));
	AddPropItem("part_map.plate.fInnerEdgeOffset",PROPLIST_ITEM(id++,"inner edge offset","inner edge offset"));
	AddPropItem("part_map.plate.bAdjustPlateUcs",PROPLIST_ITEM(id++,"Adjust text dimension in horizontal direction","","Yes|No"));
	AddPropItem("part_map.plate.bDimCoord",PROPLIST_ITEM(id++,"dimension coordinate","","Yes|No"));
	AddPropItem("part_map.plate.bPlateUseSimpleLsMap",PROPLIST_ITEM(id++,"simplifyd bolt symbol","","Yes|No"));
	AddPropItem("part_map.plate.bZoomDimTextSizeByWidth",PROPLIST_ITEM(id++,"automatically calculate dimension text height","","Yes|No"));
	AddPropItem("part_map.plate.fTextSizeZoomCoef",PROPLIST_ITEM(id++,"text height scale coefficient"));
	AddPropItem("part_map.plate.bDrawSlopeCutLine",PROPLIST_ITEM(id++,"draw the weld groove line","","Yes|No"));
	AddPropItem("part_map.plate.bDrawDatumLine",PROPLIST_ITEM(id++,"draw the datum line","","Yes|No"));
	AddPropItem("part_map.plate.iZhiWanLineType",PROPLIST_ITEM(id++,"zhi wan line type","","0.Outer|1.Inner"));
	AddPropItem("part_map.plate.fInnerOffThreshold",PROPLIST_ITEM(id++,"offset Threshold",""));
	AddPropItem("part_map.plate.iCutArcsType",PROPLIST_ITEM(id++,"weld corner cutting arc style","","0.No cutting|1.Cutting angle|2.Cutting arc"));
	AddPropItem("part_map.plate.bMarkHuoquPos",PROPLIST_ITEM(id++,"Mark HuoquLine","","Yes|No"));
	AddPropItem("part_map.plate.bDrawHuoQuPrjMap",PROPLIST_ITEM(id++,"Draw Huoqu Map","","Yes|No"));
	AddPropItem("part_map.plate.bDrawHuoQuMarkLine",PROPLIST_ITEM(id++,"Draw Huoqu Feature Line","","Yes|No"));
	AddPropItem("part_map.plate.bDisplaypPartInfoTbl",PROPLIST_ITEM(id++,"Display Part Info","","Yes|No"));
	AddPropItem("part_map.plate.nPartInfoTblColumnWidth",PROPLIST_ITEM(id++,"Column Width"));
	AddPropItem("part_map.plate.bUseDimStyleConfigFile",PROPLIST_ITEM(id++,"Use Config File","","Yes|No"));
	//�ֹܹ���ͼ
	AddPropItem("part_map.tube",PROPLIST_ITEM(id++,"tube part map"));
	AddPropItem("part_map.tube.fSlotWidthProcessError",PROPLIST_ITEM(id++,"slot width error value"));
	AddPropItem("part_map.tube.fSlotLenProcessError",PROPLIST_ITEM(id++,"slot length error value"));
	AddPropItem("part_map.tube.fReelTubeThresholdD",PROPLIST_ITEM(id++,"reelpipe diameter threshold"));
	//��ע--�������
	AddPropItem("PartNoDim",PROPLIST_ITEM(id++,"parts number"));
	AddPropItem("dim_map.PartNo.iPartNoFrameStyle",PROPLIST_ITEM(id++,"ordinary parts number frame type","","0.Circle|1.Waist round rectangle|2.Ordinary rectangular|3.Automatic judgment"));
	AddPropItem("dim_map.PartNo.iFlPartNoFrameStyle",PROPLIST_ITEM(id++,"Flange numbered box type","","0.Circle|1.Waist round rectangle|2.Ordinary rectangular|3.Automatic judgment"));
	AddPropItem("dim_map.PartNo.iStdInsertPlatePartNoFrameStyle",PROPLIST_ITEM(id++,"Standard board number frame type","","0.Circle|1.Waist round rectangle|2.Ordinary rectangular|3.Automatic judgment"));
	AddPropItem("dim_map.PartNo.fPartNoCirD",PROPLIST_ITEM(id++,"number frame length"));
	AddPropItem("dim_map.PartNo.fMainPoleNo",PROPLIST_ITEM(id++,"main pole number"));
	AddPropItem("dim_map.PartNo.fMainPoleNoOuterCirD",PROPLIST_ITEM(id++,"outer circle diameter"));
	AddPropItem("dim_map.PartNo.fMainPoleNoInnerCirD",PROPLIST_ITEM(id++,"inner circle diameter"));
	AddPropItem("dim_map.PartNo.iMirMsg",PROPLIST_ITEM(id++,"symmetry mode","","0.X Axis Mirror|1.Y Axis Mirror|2.No Mirror"));
	AddPropItem("dim_map.PartNo.fPartNoTextSize",PROPLIST_ITEM(id++,"parts number text height"));
	AddPropItem("dim_map.PartNo.fPartNoMargin",PROPLIST_ITEM(id++,"text frame clearance"));
	AddPropItem("dim_map.PartNo.bDrawRefLine",PROPLIST_ITEM(id++,"draw indication line","","Yes|No"));
	AddPropItem("dim_map.fSegITextSize",PROPLIST_ITEM(id++,"segment number text height"));
	AddPropItem("dim_map.fSegICirD",PROPLIST_ITEM(id++,"segment number frame length"));
	//����߶ȱ�ע��
	AddPropItem("PartNo",PROPLIST_ITEM(id++,"PartNoText"));
	AddPropItem("PartNoMargin",PROPLIST_ITEM(id++,"PartNoMargin"));
	AddPropItem("DimText",PROPLIST_ITEM(id++,"DimTextHeigh"));
	AddPropItem("DimAngleText",PROPLIST_ITEM(id++,"DimAngleTextH"));
	AddPropItem("HeaderText",PROPLIST_ITEM(id++,"HeaderTextH"));
	AddPropItem("CommonText",PROPLIST_ITEM(id++,"CommonTextH"));
	AddPropItem("NumericText",PROPLIST_ITEM(id++,"NumericTextH"));
	AddPropItem("CutawayViewSymbolText",PROPLIST_ITEM(id++,"CutawayViewSymbolText"));
	AddPropItem("CutawayViewSymbolText1",PROPLIST_ITEM(id++,"CutawayViewSymbolText1"));
	AddPropItem("SimPolyText",PROPLIST_ITEM(id++,"SimPolyText"));
	AddPropItem("SimPolyText1",PROPLIST_ITEM(id++,"SimPolyText1"));
	AddPropItem("LsGuigeText",PROPLIST_ITEM(id++,"LsGuigeText"));
	AddPropItem("JgOddmentText",PROPLIST_ITEM(id++,"JgOddmentText"));
	AddPropItem("PlateThickText",PROPLIST_ITEM(id++,"PlateThickText"));
	AddPropItem("PartGuigeText",PROPLIST_ITEM(id++,"PartGuigeText"));
	AddPropItem("OtherText",PROPLIST_ITEM(id++,"OtherText"));
	//�ߴ��ע
	AddPropItem("setMapSize",PROPLIST_ITEM(id++,"dimension style"));
	AddPropItem("dim_map.sArrowType",PROPLIST_ITEM(id++,"arrow type","","Solid closed|Hollow closed|Closed|Architectural symbol|Tilt|30 degree|Right angle"));
	AddPropItem("dim_map.fDimTextSize",PROPLIST_ITEM(id++,"length text height"));
	AddPropItem("dim_map.fDimGap",PROPLIST_ITEM(id++,"dimension clearance",""));
	AddPropItem("dim_map.fDimAngleTextSize",PROPLIST_ITEM(id++,"angle text height"));
	AddPropItem("dim_map.fDimArrowSize",PROPLIST_ITEM(id++,"arrow length"));
	//���ϱ��ע
	AddPropItem("setMaterMap",PROPLIST_ITEM(id++,"material table dimension"));
	AddPropItem("dim_map.fHeaderTextSize",PROPLIST_ITEM(id++,"header text height"));
	AddPropItem("dim_map.fCommonTextSize",PROPLIST_ITEM(id++,"title height"));
	AddPropItem("dim_map.fNumericTextSize",PROPLIST_ITEM(id++,"content text height"));
	AddPropItem("dim_map.sFontName",PROPLIST_ITEM(id++,"font name"));
	//����ͼ����ע
	AddPropItem("CutawayViewSymbol",PROPLIST_ITEM(id++,"cutaway view symbol"));
	AddPropItem("dim_map.fCutawayViewSymbolTextSize",PROPLIST_ITEM(id++,"text height"));
	AddPropItem("dim_map.fCutawayViewSymbolLen",PROPLIST_ITEM(id++,"length"));
	AddPropItem("dim_map.fCutawayViewPolyLineWidth",PROPLIST_ITEM(id++,"multi-line width"));
	//�����α�ע
	AddPropItem("SimPolyLayer",PROPLIST_ITEM(id++,"similar polygon"));
	AddPropItem("dim_map.fSimPolyTextSize",PROPLIST_ITEM(id++,"similar polygon text height"));
	AddPropItem("dim_map.fSimTriScale",PROPLIST_ITEM(id++,"similar polygon proportion"));
	//�����ָ߱�ע
	AddPropItem("Font",PROPLIST_ITEM(id++,"Others text high"));
	AddPropItem("dim_map.fLsGuigeTextSize",PROPLIST_ITEM(id++,"bolt spec","dimension the font size of bolt specifications"));	
	AddPropItem("dim_map.fJgOddmentTextSize",PROPLIST_ITEM(id++,"angle ends","dimension the font size of angle ends"));
	AddPropItem("dim_map.fPlateThickTextSize",PROPLIST_ITEM(id++,"plate thickness"));
	AddPropItem("dim_map.fPartGuigeTextSize",PROPLIST_ITEM(id++,"parts specifications and length"));
	AddPropItem("dim_map.fOtherTextSize",PROPLIST_ITEM(id++,"Others Chinese"));
	//ͼ��
	AddPropItem("LayerTable.layerHashList",PROPLIST_ITEM(id++,"layer","","Add a new layer"));
	AddPropItem("Layer.colorIndex",PROPLIST_ITEM(id++,"color"));
	AddPropItem("Layer.lineTypeName",PROPLIST_ITEM(id++,"lineType","","CONTINUOUS|HIDDEN|DASHDOT2X|DIVIDE|ZIGZAG"));
	AddPropItem("Layer.lineWeight",PROPLIST_ITEM(id++,"lineWeight","","0.00mm|0.05mm|0.09mm|0.13mm|0.15mm|0.18mm|0.20mm|0.25mm|0.30mm|0.35mm|0.40mm|0.50mm|0.53mm|0.60mm|0.70mm|0.80mm|0.90mm|1.00mm|1.06mm|1.20mm|1.40mm|1.58mm|2.00mm|2.11mm"));
	//ר��ͼ��
	AddPropItem("LayerTable",PROPLIST_ITEM(id++,"special layer"));
	long iSpecialLayerId=id;
	AddPropItem("LayerTable.VisibleProfileLayer",PROPLIST_ITEM(id++,"visible outline"));
	AddPropItem("LayerTable.UnvisibleProfileLayer",PROPLIST_ITEM(id++,"hidden line"));
	AddPropItem("LayerTable.BriefLineLayer",PROPLIST_ITEM(id++,"stress material single"));
	AddPropItem("LayerTable.AuxLineLayer",PROPLIST_ITEM(id++,"sub-material single"));
	AddPropItem("LayerTable.BoltSymbolLayer",PROPLIST_ITEM(id++,"bolt"));
	AddPropItem("LayerTable.SimPolyLayer",PROPLIST_ITEM(id++,"similar polygon"));
	AddPropItem("LayerTable.AxisLineLayer",PROPLIST_ITEM(id++,"center line "));
	AddPropItem("LayerTable.BendLineLayer",PROPLIST_ITEM(id++,"bending line"));
	AddPropItem("LayerTable.BreakLineLayer",PROPLIST_ITEM(id++,"disconnect the line"));
	AddPropItem("LayerTable.BoltLineLayer",PROPLIST_ITEM(id++,"bolt line"));
	AddPropItem("LayerTable.DimSizeLayer",PROPLIST_ITEM(id++,"dimension"));
	AddPropItem("LayerTable.DimTextLayer",PROPLIST_ITEM(id++,"ext layer"));
	AddPropItem("LayerTable.CommonLayer",PROPLIST_ITEM(id++,"rest"));
	AddPropItem("LayerTable.SonJgAxisLineLayer",PROPLIST_ITEM(id++,"the child angle center line"));
	AddPropItem("LayerTable.AssistHuoQuLineLayer",PROPLIST_ITEM(id++,"auxiliary bending line"));
	AddPropItem("LayerTable.DamagedSymbolLine",PROPLIST_ITEM(id++,"Plate Profile Symbol"));
#ifdef  __DRAWING_CONTEXT_
	m_xSpecialLayerIdArr.Empty();
	for(int i=0;i<16;i++)
		m_xSpecialLayerIdArr.append(iSpecialLayerId+i);
	MIN_SYSSETTING_PROPID=id;
#endif
}
IMPLEMENT_PROP_FUNC(CSystemSettingDlg)
int CSystemSettingDlg::MIN_SYSSETTING_PROPID=1;
void CSystemSettingDlg::InitPropHashtable()
{
	int id = 1;
	const DWORD HASHTABLESIZE = 500;
	const DWORD STATUSHASHTABLESIZE = 50;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	InitEnglishPropHashtable();
#else
	//����
	AddPropItem("General",PROPLIST_ITEM(id++,"����"));
	AddPropItem("general_set.LineMapScale",PROPLIST_ITEM(id++,"����ͼ����  1:","����ͼ���Ʊ���"));
	AddPropItem("general_set.ForceLineScale",PROPLIST_ITEM(id++,"˾��ͼ����  1:","˾��ͼ���Ʊ���"));
	AddPropItem("general_set.StruMapScale",PROPLIST_ITEM(id++,"�ṹͼ����  1:","�ṹͼ���Ʊ���"));
	CXhChar100 strHelp("Բ��:����˨ֱ��������ӦԲ��;ͼ��\\����ͼ��:����˨ֱ��������˨ͼ��,���߿������ű���,δ�ҵ���Ӧͼ��ʱ�����Բ�ס�");
	AddPropItem("general_set.StruMapScale.iBoltSymbolStyle",PROPLIST_ITEM(id++,"�ṹͼ��˨�״���ʽ",strHelp,"0.Բ��|1.ͼ��|2.����ͼ��|3.����ֱ��ͼ��"));
	AddPropItem("general_set.StruMapScale.fBoltSymbolFactor",PROPLIST_ITEM(id++,"��������","����ͼ����������/����ֱ��ͼ���ı�������"));
	AddPropItem("general_set.PartMapScale",PROPLIST_ITEM(id++,"����ͼ����  1:"));
	AddPropItem("general_set.PartMapScale.iBoltSymbolStyle",PROPLIST_ITEM(id++,"����ͼ��˨�״���ʽ",strHelp,"0.Բ��|1.ͼ��|2.����ͼ��|3.����ֱ��ͼ��"));
	AddPropItem("general_set.PartMapScale.fBoltSymbolFactor",PROPLIST_ITEM(id++,"��������","����ͼ����������/����ֱ��ͼ���ı�������"));
	AddPropItem("general_set.WeldMapScale",PROPLIST_ITEM(id++,"�麸ͼ����  1:"));
	AddPropItem("general_set.WeldMapScale.iBoltSymbolStyle",PROPLIST_ITEM(id++,"�麸ͼ��˨�״���ʽ",strHelp,"0.Բ��|1.ͼ��|2.����ͼ��|3.����ֱ��ͼ��"));
	AddPropItem("general_set.WeldMapScale.fBoltSymbolFactor",PROPLIST_ITEM(id++,"��������","����ͼ����������/����ֱ��ͼ���ı�������"));
	AddPropItem("general_set.iMatCharPosType",PROPLIST_ITEM(id++,"�����ַ�λ��","������ŵĲ����ַ�λ��","0.����Ҫ�����ַ�|1.�������ǰ|2.������ź�"));
	AddPropItem("general_set.cMatCharSeparator",PROPLIST_ITEM(id++,"�����ַ��ָ���","","��|�ո�"));
	AddPropItem("general_set.fHoleAugmentInNC",PROPLIST_ITEM(id++,"�׾�����ֵ","�׾�����ֵ(����NCͼ)"));
	AddPropItem("general_set.fWeldLineSymbolHigh",PROPLIST_ITEM(id++,"�����߸߶�"));
	AddPropItem("general_set.fWeldLineSymbolSpace",PROPLIST_ITEM(id++,"�����߼�϶"));
	AddPropItem("general_set.iDimPrecision",PROPLIST_ITEM(id++,"�ߴ羫ȷ��","","1.0mm|0.5mm|0.1mm"));
	strHelp.Copy("������ϸ��ͬ��ʽ�п�ͬ,��˨��ϸ���湹����ϸ���Ƚ��е���,��֤������һ�¡�");
	AddPropItem("general_set.iPartBOMStyle",PROPLIST_ITEM(id++,"������ϸ����ʽ",strHelp,"0.��׼��ʽ|1.�ӿ���ʽ"));
	strHelp.Copy("��ʽһ:���ʴӴ�С,�Ǹֹ��Ӵ�С,�������С������ʽ��:���ʴ�С����,��������С����");
	AddPropItem("general_set.iSumMatBOMStyle",PROPLIST_ITEM(id++,"���ϻ��ܱ���ʽ",strHelp,"0.��ʽһ|1.��ʽ��"));
	strHelp.Copy("��ע������ϸ��ʹ�ù����������򻯱�עʱ���ñ������ƹ��������м򻯹��ĸ�����������Ϊ26");
	AddPropItem("nBriefGuigeN",PROPLIST_ITEM(id++,"�򻯹����",strHelp));
	AddPropItem("bDrawMainPoleJointLine",PROPLIST_ITEM(id++,"����ͼ�ֶα�־��","�Ƿ��ڻ��Ƶ���ͼ��ʱ����Ʒֶα�־��","��|��"));
	AddPropItem("nJointLineOffset",PROPLIST_ITEM(id++,"�ֶα�־��ƫ��ֵ"));
	AddPropItem("nJointLineLen",PROPLIST_ITEM(id++,"�ֶα�־�߳���"));
	//���տ�·�������з�ʽ
	AddPropItem("ProcessCartAndLayout",PROPLIST_ITEM(id++,"�������տ��������з�ʽ"));
#ifdef __DRAWING_CONTEXT_
	if(sys.general_set.bCardLayoutBaseRow)
		strHelp.Copy("ÿ���������");	
	else
		strHelp.Copy("ÿ���������");
#endif
	AddPropItem("general_set.nPartRowsN",PROPLIST_ITEM(id++,strHelp));
	AddPropItem("general_set.nRowHigh",PROPLIST_ITEM(id++,"����ͼ�и�"));
	AddPropItem("general_set.nColumnWidth",PROPLIST_ITEM(id++,"����ͼ�п�"));
	strHelp.Copy("����Ϊ��׼ʱ���������У�����Ϊ��׼ʱ����������");
	AddPropItem("general_set.bCardLayoutBaseRow",PROPLIST_ITEM(id++,"��׼���з�ʽ",strHelp,"0.����Ϊ��׼|1.����Ϊ��׼"));
	AddPropItem("general_set.bHasFrame",PROPLIST_ITEM(id++,"װ����","�Ƿ��ڿ�Ƭ���ܻ�װ����","��|��"));
	AddPropItem("general_set.bProcessCardTbl",PROPLIST_ITEM(id++,"���տ�ͼ��","","��|��"));
	AddPropItem("general_set.m_sJgCard",PROPLIST_ITEM(id++,"�Ǹֹ��տ�","�Ǹֹ��տ�·��"));
	AddPropItem("general_set.m_sSlotCard",PROPLIST_ITEM(id++,"�۸ֹ��տ�","�۸ֹ��տ�·��"));
	AddPropItem("general_set.m_sTubeCard",PROPLIST_ITEM(id++,"�ֹܹ��տ�","�ֹܹ��տ�·��"));
	AddPropItem("general_set.m_sPlankCard",PROPLIST_ITEM(id++,"�ְ幤�տ�","�ְ幤�տ�·��"));
	AddPropItem("general_set.m_sWeldPartGroupCard",PROPLIST_ITEM(id++,"�麸�����տ�","�麸�����տ�·��"));
	AddPropItem("general_set.m_sProcessPartStatCard",PROPLIST_ITEM(id++,"���ռ�ͳ�ƿ�","���⹤��ͳ�ƿ�"));
	AddPropItem("MapArxPath",PROPLIST_ITEM(id++,"��ͼģ��·��"));
	AddPropItem("general_set.m_sTMAMapArxPath",PROPLIST_ITEM(id++,"TMA��ͼģ��·��"));
	AddPropItem("general_set.m_sLMAMapArxPath",PROPLIST_ITEM(id++,"LMA��ͼģ��·��"));
	AddPropItem("general_set.m_sLDSMapArxPath",PROPLIST_ITEM(id++,"LDS��ͼģ��·��"));
	AddPropItem("general_set.m_sTSAMapArxPath",PROPLIST_ITEM(id++,"TSA��ͼģ��·��"));
	//���ֻ��ƽ���������
	AddPropItem("digitalHandver",PROPLIST_ITEM(id++,"���ֻ���·�ƽ�"));
	AddPropItem("general_set.lCrossarmCr",PROPLIST_ITEM(id++,"��ᵣ��ɫ"));
	AddPropItem("general_set.rCrossarmCr",PROPLIST_ITEM(id++,"�Һᵣ��ɫ"));
	//��ͼ
	AddPropItem("LineMap",PROPLIST_ITEM(id++,"��ͼ"));
	AddPropItem("LineMap.Font",PROPLIST_ITEM(id++,"��������"));
	//˾��ͼ
	AddPropItem("ForceLineMap",PROPLIST_ITEM(id++,"˾��ͼ"));
	AddPropItem("ForceLineMap.Font",PROPLIST_ITEM(id++,"��������"));
	//�ṹͼ
	AddPropItem("StruMap",PROPLIST_ITEM(id++,"�ṹͼ"));
	AddPropItem("stru_map.Font",PROPLIST_ITEM(id++,"��������"));
	AddPropItem("stru_map.bExportByEnglish",PROPLIST_ITEM(id++,"�������","�ṹͼ�������","0.����|1.Ӣ��"));
	AddPropItem("stru_map.fMinWingWide",PROPLIST_ITEM(id++,"�Ǹ�֫����С����ֵ"));
	AddPropItem("stru_map.fMinPartThick",PROPLIST_ITEM(id++,"���������С����ֵ"));
	AddPropItem("stru_map.bDimSpecialLsSpace",PROPLIST_ITEM(id++,"��ע������˨���","�ṹͼ���Ƿ��ע������˨���","��|��"));
	AddPropItem("stru_map.bDimSpecialJgZhun",PROPLIST_ITEM(id++,"��ע�Ǹ������ľ�","","��|��"));
	AddPropItem("stru_map.bDimStruJgLen",PROPLIST_ITEM(id++,"��ע�Ǹֳ���","�ṹͼ���Ƿ��ע�Ǹֳ���","��|��"));
	AddPropItem("stru_map.bDimPartGuiGe",PROPLIST_ITEM(id++,"��ע�������","�ṹͼ���Ƿ��ע�������","��|��"));
	AddPropItem("stru_map.bDrawCutAngleAndWingInfo",PROPLIST_ITEM(id++,"���ƽǸ���֫�н���Ϣ","�ṹͼ���Ƿ����ֽǸ���֫�н���Ϣ","��|��"));
	AddPropItem("stru_map.bOnlyDrawHorizontalPlate",PROPLIST_ITEM(id++,"������ˮƽ��ʾ�ĸְ�","�ǣ������Ƶ�ǰ��ͼ��ˮƽ��ʾ�ĸְ壻�񣺻��Ƶ�ǰ��ͼ�е����иְ塣","��|��"));
	AddPropItem("stru_map.bUsePolyLineDrawEdge",PROPLIST_ITEM(id++,"����߻���������","�������ƽṹͼ�нǸֺ͸ְ�����������ͣ���ֱͨ�߻��߶����","��|��"));
	AddPropItem("stru_map.fPolyLineWidth",PROPLIST_ITEM(id++,"����߿��","����߿��"));
	AddPropItem("stru_map.PoleAxisLineOutSpreadLen",PROPLIST_ITEM(id++,"�˼�����������","�˼�������������"));
	AddPropItem("stru_map.fPoleAxisLineStartOutSpreadLen",PROPLIST_ITEM(id++,"ʼ��","�˼�����ʼ����������"));
	AddPropItem("stru_map.fPoleAxisLineEndOutSpreadLen",PROPLIST_ITEM(id++,"�ն�","�˼������ն���������"));
	AddPropItem("stru_map.bReadAdditionalBuffer",PROPLIST_ITEM(id++,"��ȡ���ӻ���","���ļ�ʱ�Ƿ���Ҫ��ȡ���ӻ���,�������ܻ�ͼ�ű�����ͼ�����ļ���","��|��"));
	//ͼ������
	AddPropItem("m_pCurMapFrm.style",PROPLIST_ITEM(id++,"ͼ����","ͼ����"));
	AddPropItem("m_pCurMapFrm.nLength",PROPLIST_ITEM(id++,"ͼ�򳤶�","ͼ�򳤶ȡ�"));
	AddPropItem("m_pCurMapFrm.nWidth",PROPLIST_ITEM(id++,"ͼ����","ͼ���ȡ�"));
	AddPropItem("m_pCurMapFrm.nLeftMargin",PROPLIST_ITEM(id++,"����϶","���ҳ�߼�϶ֵ��"));
	AddPropItem("m_pCurMapFrm.nOtherMargin",PROPLIST_ITEM(id++,"�������϶","�ϡ��¡��Ҳ�ҳ�߼�϶ֵ��"));
	AddPropItem("m_pCurMapFrm.nBottomRightFixLen",PROPLIST_ITEM(id++,"���²�̶�����","���²����ڷ���ͼֽ��Ϣ�ľ��γ��ȡ�"));
	AddPropItem("m_pCurMapFrm.nBottomRightFixWidth",PROPLIST_ITEM(id++,"���²�̶��߶�","���²����ڷ���ͼֽ��Ϣ�ľ��θ߶�,���ɱ��ʱ�ɸ��ݸø߶��Զ��ж��Ƿ���Ҫ��ҳ��ʾ��"));
	//�麸ͼ
	AddPropItem("PartGroupMap",PROPLIST_ITEM(id++,"�麸ͼ"));
	AddPropItem("part_group_map.Font",PROPLIST_ITEM(id++,"��������"));
	AddPropItem("part_group_map.fMinFontSize",PROPLIST_ITEM(id++,"������С�߶�","�����麸ͼʱ�����尴�������ŵ����뱣֤�������С�߶�."));
	AddPropItem("part_group_map.fMinDrawThick",PROPLIST_ITEM(id++,"�����С����ֵ","�����С����ֵ"));
	AddPropItem("part_group_map.nKmJgGIncrement",PROPLIST_ITEM(id++,"��ģ�Ǹ��ľ�����ֵ","��ģ�Ǹ��ľ����ֵ"));
	AddPropItem("part_group_map.fPartGroupDistThreshold",PROPLIST_ITEM(id++,"������ֵ","�麸ͼ����������ֵ"));
	AddPropItem("part_group_map.fPartGroupDistZoomCoef",PROPLIST_ITEM(id++,"����ϵ��","�麸ͼ����ϵ��"));
	AddPropItem("part_group_map.bDrawWeldLineSymbol",PROPLIST_ITEM(id++,"���ƺ����߱��","�麸ͼ�л��ƺ����߱��","��|��"));
	AddPropItem("part_group_map.iPlateLocateDrawStyle",PROPLIST_ITEM(id++,"���Ƹְ�Ի��","�麸ͼ�л��Ƹְ�Ի��","0.������|1.������|2.������"));
	AddPropItem("part_group_map.iLocateSymboleHeight",PROPLIST_ITEM(id++,"���Ƹְ�Ի��߶�","�麸ͼ�л��Ƹְ�Ի��ĸ߶�"));
	AddPropItem("part_group_map.bAutoDimPartNo",PROPLIST_ITEM(id++,"�Զ���ע�������","�Զ���ע�������","��|��"));
	AddPropItem("part_group_map.bAutoMatchView",PROPLIST_ITEM(id++,"�Զ����ֺ�����ͼ","ϵͳ�Զ����麸����������ͼ��ϵ�һ�����տ�������ʾ","��|��"));
	AddPropItem("part_group_map.bModulatePartGroupLen",PROPLIST_ITEM(id++,"�Զ������麸ͼ����","�Զ������麸ͼ����","��|��"));
	AddPropItem("part_group_map.bDimAbsoluteDist",PROPLIST_ITEM(id++,"��λ����Գߴ�","�Զ���ע��λ����Գߴ�","��|��"));
	AddPropItem("part_group_map.bDimAfterWeldCheckLine",PROPLIST_ITEM(id++,"������ߴ�","��ע������ߴ�","��|��"));
	AddPropItem("part_group_map.bDimBeforeWeldLocatSpace",PROPLIST_ITEM(id++,"��ǰװ�䶨λ�ߴ�","��ע��ǰװ�䶨λ�ߴ�","��|��"));
	AddPropItem("part_group_map.bDimPlatePoleRayAndAngle",PROPLIST_ITEM(id++,"���߲������߼��н�","��ע�ְ����Ӳ������߼����׼��֮��ļн�","��|��"));
	AddPropItem("part_group_map.bSmartFilterRedundantPart",PROPLIST_ITEM(id++,"�Զ�ɸѡ��ͼ��ʾ����","��ͼ���Զ�ɸѡ���뵱ǰ��ͼ�޹صĶ��๹����������ʾ��","��|��"));
	AddPropItem("part_group_map.bAngularDimension",PROPLIST_ITEM(id++,"��ע�Ƕ���","�Զ���ע�Ƕ���","��|��"));
	AddPropItem("part_group_map.bCreatePartGroupBOM",PROPLIST_ITEM(id++,"�����麸����ϸ��","�����麸����ϸ��","��|��"));
	AddPropItem("welding.textsize.bZoomWithScale",PROPLIST_ITEM(id++,"�����涯����","�������ͼ�����涯����","��|��"));
	AddPropItem("welding.textsize.wiScale",PROPLIST_ITEM(id++,"��׼���ű��� 1��","��ǰ�趨����߶�����Ӧ�Ļ�׼��ͼ����=ģ�ͳߴ�/ͼֽ�ߴ�"));
	//������ϸ��
	AddPropItem("part_group_map.part_table.bZoomInProcessCard",PROPLIST_ITEM(id++,"���տ��ڽ��б�������","���տ��ڵ��麸����ϸ���Ƿ���ͼ���еȱ�������","��|��"));
	AddPropItem("part_group_map.part_table.bDispPartGroupNo",PROPLIST_ITEM(id++,"�麸�����","�麸����ϸ������ʾ�麸�����","��|��"));
	AddPropItem("part_group_map.part_table.bDispPartGroupNum",PROPLIST_ITEM(id++,"�麸������","�麸����ϸ������ʾ�麸������","��|��"));
	AddPropItem("part_group_map.part_table.bDispPartNo",PROPLIST_ITEM(id++,"�������","�麸����ϸ������ʾ�������","��|��"));
	AddPropItem("part_group_map.part_table.bDispPartNum",PROPLIST_ITEM(id++,"��������","�麸����ϸ������ʾͬһ��Ź�������","��|��"));
	AddPropItem("part_group_map.part_table.bDispPartGuiGe",PROPLIST_ITEM(id++,"�������","�麸����ϸ������ʾ�������","��|��"));
	AddPropItem("part_group_map.part_table.bDispPartLen",PROPLIST_ITEM(id++,"��������","�麸����ϸ������ʾ��������","��|��"));
	AddPropItem("part_group_map.part_table.bDispPartWeight",PROPLIST_ITEM(id++,"�麸������","�麸����ϸ����ÿ����������������ͬ��Ź���С���������Լ��麸��������","��|��"));
	AddPropItem("part_group_map.part_table.bDispNotes",PROPLIST_ITEM(id++,"��ע","�麸����ϸ������ʾ������ע��Ϣ","��|��"));
	AddPropItem("part_group_map.part_table.bDispInternal",PROPLIST_ITEM(id++,"���켶������","���켶������","��|��"));
	AddPropItem("part_group_map.part_table.bDispAppearance",PROPLIST_ITEM(id++,"���켶�����","���켶�����","��|��"));
	//����ͼ
	AddPropItem("PartMap",PROPLIST_ITEM(id++,"����ͼ"));
	AddPropItem("part_map.iExportDeformedProfile",PROPLIST_ITEM(id++,"�����������ЧӦ","����ͼ�Ƿ����������ЧӦ","0.������|1.����|2.����ͼ����|3.���տ�����"));
	AddPropItem("part_map.bDimDeformedProfileText",PROPLIST_ITEM(id++,"��ע������������","���ǻ�������ʱ�Ƿ��ע��������ʱ�Ƿ��ע�������","��|��"));
	AddPropItem("part_map.bDimPlaneAngle",PROPLIST_ITEM(id++,"��ע������������н�","�Ƿ��ע������������н�","��|��"));
	AddPropItem("part_map.iDimAngleType",PROPLIST_ITEM(id++,"�����Ƕ�����","�����Ƕȱ�ע����,�ò������������ƻ����ǸֵĽǶȱ�ע���͡�","0.���|1.�۽�|2.�Զ�"));
	AddPropItem("part_map.fMinHuoquAngle",PROPLIST_ITEM(id++,"�����Ƕ���ֵ","С�ڴ˻����Ƕȵ����乹������ע������Ϣ"));
	AddPropItem("part_map.bDimHuoquType",PROPLIST_ITEM(id++,"��ע������������","�Ƿ��ע������������ ����,�������ְ���T,�����Ƕ�A��: T<=2��A<90��5<T<8��A<45��9<T<10��A<20��T>10��A<10ʱΪ����,�������Ϊ������","��|��"));
	AddPropItem("part_map.bUseZhengFanQun",PROPLIST_ITEM(id++,"��������ע��������","ʹ��������������ע������,����ʹ��������������","��|��"));
	AddPropItem("part_map.iPartMapStyle",PROPLIST_ITEM(id++,"���ձ�Ƿ���","��ʽһ��'��'�����Ҫ�˹��գ���ʽ����'V'�����Ҫ�˹���","0.��ʽһ|1.��ʽ��"));
	//�Ǹֹ���ͼ����
	AddPropItem("AnglePartMap",PROPLIST_ITEM(id++,"�Ǹֹ���ͼ"));
	AddPropItem("AnglePartMap.Font",PROPLIST_ITEM(id++,"�Ǹ���ͼ��������"));
	AddPropItem("part_map.angle.bOneCardMultiPart",PROPLIST_ITEM(id++,"һ�����","һ�����","��|��"));
	AddPropItem("part_map.angle.iJgZoomSchema",PROPLIST_ITEM(id++,"�Ǹ����ŷ���","2.����ͬ�����ż�3.����ֱ����ž�Ϊ�����տ���ͼ�����Զ��������ű���,����ֱ�����ʱ�������������ͼ����","0.������1:1����|1.ʹ�ù���ͼ����|2.����ͬ������|3.����ֱ�����"));//0.1:1���� 1.ʹ�ù���ͼ���� 2.����ͬ������ 3.����ֱ�����
	AddPropItem("part_map.angle.ciJgLenDrawMode",PROPLIST_ITEM(id++,"�������ȴ�����","�Ǹֳ��ȳ�����ͼ������ģʽ","0.��������|1.�м伷ѹ"));
	AddPropItem("part_map.angle.bMaxExtendAngleLength",PROPLIST_ITEM(id++,"����޶�������Ƴ���","�Ƿ��ڽǸֹ��տ���ͼ��������޶�����Ǹֻ��Ƴ��ȡ�","0.��|1.��"));
	AddPropItem("part_map.angle.iJgGDimStyle",PROPLIST_ITEM(id++,"�Ǹ��ľ��עλ��","�Ǹ��ľ��עλ��","0.ʼ��|1.�м�|2.�Զ��ж�"));
	AddPropItem("part_map.angle.nMaxBoltNumStartDimG",PROPLIST_ITEM(id++,"�����˨��","ʼ�˼��б�ע׼��֧�ֵ������˨����,��������˨����׼���Զ���ע�ڽǸ��м�"));
	AddPropItem("part_map.angle.iLsSpaceDimStyle",PROPLIST_ITEM(id++,"��˨����ע��ʽ","��˨����ע��ʽ","0.��X�᷽��|1.��Y�᷽��|2.�Զ��ж�|3.����ע���|4.����ߴ���"));
	AddPropItem("part_map.angle.nMaxBoltNumAlongX",PROPLIST_ITEM(id++,"�����˨��","��X�᷽���ע֧�ֵ������˨����,��������˨�����Զ�ʹ����Y�᷽���ע��˨���"));
	AddPropItem("part_map.angle.bDimCutAngle",PROPLIST_ITEM(id++,"��ע�нǳߴ�","��ע�нǳߴ�","��|��"));
	AddPropItem("part_map.angle.bDimCutAngleMap",PROPLIST_ITEM(id++,"��ʾ�н�ʾ��ͼ","��ʾ�н�ʾ��ͼ","��|��"));
	AddPropItem("part_map.angle.bDimPushFlatMap",PROPLIST_ITEM(id++,"��עѹ��","��עѹ��","��|��"));
	AddPropItem("part_map.angle.bDimKaiHe",PROPLIST_ITEM(id++,"��ע���Ͻ�","�Ƿ��ڲ�ͼ�����ע���ͽǶ�����","��|��"));
	AddPropItem("part_map.angle.bDimKaiheSumLen",PROPLIST_ITEM(id++,"��ע���������ܳ�","��ע���������ܳ���","��|��"));
	AddPropItem("part_map.angle.bDimKaiheAngle",PROPLIST_ITEM(id++,"��ע���ϽǶ���","��ע���ϽǶ�����","��|��"));
	AddPropItem("part_map.angle.bDimKaiheSegLen",PROPLIST_ITEM(id++,"��ע��������ֶγ�","��ע��������ֶγ���","��|��"));
	AddPropItem("part_map.angle.bDimKaiheScopeMap",PROPLIST_ITEM(id++,"��ע���������ʶ��","��ע���������ʶ����","��|��"));
	AddPropItem("part_map.angle.bJgUseSimpleLsMap",PROPLIST_ITEM(id++,"����˨ͼ��","�Ǹ�ʹ�ü���˨ͼ��","��|��"));
	AddPropItem("part_map.angle.bDimLsAbsoluteDist",PROPLIST_ITEM(id++,"��ע��˨���Գߴ�","��ע��˨���Գߴ�","��|��"));
	AddPropItem("part_map.angle.bMergeLsAbsoluteDist",PROPLIST_ITEM(id++,"�ϲ����ڵȾ���˨���Գߴ�","�ϲ����ڵȾ���˨���Գߴ�","��|��"));
	AddPropItem("part_map.angle.bDimRibPlatePartNo",PROPLIST_ITEM(id++,"��ע�Ǹ��߰���","�Ǹֹ��տ����Զ���ע�Ǹ��߰���","��|��"));
	AddPropItem("part_map.angle.bDimRibPlateMaterial",PROPLIST_ITEM(id++,"��ע�Ǹ��߰����","","��|��"));
	AddPropItem("part_map.angle.bDimRibPlateSetUpPos",PROPLIST_ITEM(id++,"��ע�Ǹ��߰�װ��λ��","�Ǹֹ��տ��б�ע�Ǹ��߰�װ��λ��","��|��"));
	strHelp.Copy("B:��ͷ�ߴ� L:֫�߳ߴ� C:��߳ߴ�\r\n��ʽһ:�н�=>CT LXB,��֫=>BC LXB,����֫=>CF L;\r\n��ʽ��:�н�=>BXL ��֫=>CXL �н�=>BXC(�д��=�н�+��֫)");
	AddPropItem("part_map.angle.iCutAngleDimType",PROPLIST_ITEM(id++,"�нǱ�ע��ʽ",strHelp,"0.��ʽһ|1.��ʽ��"));
	AddPropItem("part_map.angle.fKaiHeJiaoThreshold",PROPLIST_ITEM(id++,"���ϽǱ�ע��ֵ(��)","�Ǹ���֫�н���90���ƫ��ֵ���ڸ���ֵʱ��Ϊ��Ҫ���п��ϽǱ�ע��"));
	AddPropItem("part_map.angle.fKaiHeJiaoMinDeita",PROPLIST_ITEM(id++,"���Ͻ���С��ȫ��϶","�Ǹ�֫���Ͻ�ʱ���ӹ�����Ե�뿪�Ͻ���ֹ������С���롣"));
	//�ְ幹��ͼ����
	AddPropItem("PlateMap",PROPLIST_ITEM(id++,"�ְ幹��ͼ"));
	AddPropItem("PlateMap.Font",PROPLIST_ITEM(id++,"����ͼ��������"));
	AddPropItem("part_map.plate.bDimArcEdgeR",PROPLIST_ITEM(id++,"��עԲ���߰뾶","�Ƿ��עԲ�������߰뾶","��|��"));
	AddPropItem("part_map.plate.bDimArcAngle",PROPLIST_ITEM(id++,"��עԲ���߽Ƕ�","�Ƿ��עԲ���߽Ƕ�","��|��"));
	AddPropItem("part_map.plate.bDimPlateRayLs",PROPLIST_ITEM(id++,"��ע��������˨","�Ƿ��ע�������Լ���˨���","��|��"));
	AddPropItem("part_map.plate.bDimPlateBoltAngle",PROPLIST_ITEM(id++,"��ע��˨���߽Ƕ�","�Ƿ��ע����˨�Ƕ�","��|��"));
	AddPropItem("part_map.plate.bDimBoltEdgeSpace",PROPLIST_ITEM(id++,"��ע������˨�߾�","�Ƿ��ע��˨����ߵľ���","��|��"));
	AddPropItem("part_map.plate.bDimDiffRayLsSpace",PROPLIST_ITEM(id++,"��ע����������˨","�Ƿ��ע����������˨�ľ���","��|��"));
	AddPropItem("part_map.plate.bDimLsToHuoquLine",PROPLIST_ITEM(id++,"��ע��˨��������","�Ƿ��ע��˨�������߾���","��|��"));
	AddPropItem("part_map.plate.bDimLsToWeldEdge",PROPLIST_ITEM(id++,"��ע��˨�����ӱ�","�Ƿ��ע��˨�����ӱ߾���","��|��"));
	AddPropItem("part_map.plate.bDimPlateEdgeLen",PROPLIST_ITEM(id++,"��ע�ְ�߳�","��ע�ְ�߳�","��|��"));
	AddPropItem("part_map.plate.bDrawInnerEdge",PROPLIST_ITEM(id++,"������������","������������","��|��"));
	AddPropItem("part_map.plate.fInnerEdgeOffset",PROPLIST_ITEM(id++,"������ƫ�ƾ�","������ƫ�ƾ�"));
	AddPropItem("part_map.plate.bAdjustPlateUcs",PROPLIST_ITEM(id++,"�������ֱ�ע��ˮƽ����","�������������ϵ��ʹ�����ˮƽ������ƣ���֤���ֱ�ע��ˮƽ����","��|��"));
	AddPropItem("part_map.plate.bDimCoord",PROPLIST_ITEM(id++,"��ע����ֵ","�Ƿ��ע����ֵ","��|��"));
	AddPropItem("part_map.plate.bPlateUseSimpleLsMap",PROPLIST_ITEM(id++,"����˨ͼ��","�ְ��Ƿ�ʹ�ü���˨ͼ��","��|��"));
	strHelp.Copy("���ݸְ峤�������еĽ�Сֵ�����������ע����߶ȣ����Զ������ָ�ʱʹ��ϵͳ�����еĹ�������ָߡ�");
	AddPropItem("part_map.plate.bZoomDimTextSizeByWidth",PROPLIST_ITEM(id++,"�Զ������ע�ָ�",strHelp,"��|��"));
	AddPropItem("part_map.plate.fTextSizeZoomCoef",PROPLIST_ITEM(id++,"�ָ�����ϵ��","�ָ�=�ָ�����ϵ�����ְ峤�������еĽ�Сֵ,����߶Ⱦ�ȷ��Ϊ0.5mm��"));
	AddPropItem("part_map.plate.bDrawSlopeCutLine",PROPLIST_ITEM(id++,"���ƺ��촦���¿���","�ǣ���Ҫ���ƺ��촦���¿��ߣ��񣺲���Ҫ���ƺ��촦���¿��ߡ�","��|��"));
	AddPropItem("part_map.plate.bDrawDatumLine",PROPLIST_ITEM(id++,"���ƻ�׼��(�ֹ�����)","�ֹ��Ͻڵ��һ����˨����Ӧ���������˻�׼��","��|��"));
	AddPropItem("part_map.plate.iZhiWanLineType",PROPLIST_ITEM(id++,"��������ߴ���ʽ","��������߲�����ƫ�ƣ��ڲ������߿���ƫ��һ�����","0.���|1.�ڲ�"));
	AddPropItem("part_map.plate.fInnerOffThreshold",PROPLIST_ITEM(id++,"������ƫ����ֵ","�ڲ���������ƫ�ư��Ļ�����������ƫ����ֵ"));
	AddPropItem("part_map.plate.iCutArcsType",PROPLIST_ITEM(id++,"�������ǵ��л���ʽ","�������ǵ��л���ʽ","0.����|1.�н�|2.�л�"));
	AddPropItem("part_map.plate.bMarkHuoquPos",PROPLIST_ITEM(id++,"��ǻ���λ��","","��|��"));
	AddPropItem("part_map.plate.bDrawHuoQuPrjMap",PROPLIST_ITEM(id++,"���ƻ���ʾ��ͼ","","��|��"));
	AddPropItem("part_map.plate.bDrawHuoQuMarkLine",PROPLIST_ITEM(id++,"���ƻ���������","","��|��"));
	AddPropItem("part_map.plate.bDisplaypPartInfoTbl",PROPLIST_ITEM(id++,"��ʾ��Ϣ��","","��|��"));
	AddPropItem("part_map.plate.nPartInfoTblColumnWidth",PROPLIST_ITEM(id++,"����п�","ϵͳ������ʾ�����Զ��������п�,����ֵ������ǰֵʱȡ��ǰֵ��Ϊ�п�"));
	AddPropItem("part_map.plate.bUseDimStyleConfigFile",PROPLIST_ITEM(id++,"������ϸ����"));
	//�ֹܹ���ͼ
	AddPropItem("part_map.tube",PROPLIST_ITEM(id++,"�ֹܹ���ͼ"));
	AddPropItem("part_map.tube.fSlotWidthProcessError",PROPLIST_ITEM(id++,"���ۿ�ȼӹ����ֵ"));
	AddPropItem("part_map.tube.fSlotLenProcessError",PROPLIST_ITEM(id++,"���۳��ȼӹ����ֵ"));
	AddPropItem("part_map.tube.fReelTubeThresholdD",PROPLIST_ITEM(id++,"�ְ���ֱ����ֵ"));
	//��ע--�������
	AddPropItem("PartNoDim",PROPLIST_ITEM(id++,"�������"));
	strHelp.Copy("ѡ���Զ��ж�ʱ����ų����Զ��жϱ߿�����,��ѡΪԲȦ,��Ź���ʱʹ����Բ���Ρ��ṹͼ�и˼���ſ��ΪԲȦ");
	AddPropItem("dim_map.PartNo.iPartNoFrameStyle",PROPLIST_ITEM(id++,"��ͨ������ſ�����",strHelp,"0.ԲȦ|1.��Բ����|2.��ͨ����|3.�Զ��ж�"));
	AddPropItem("dim_map.PartNo.iFlPartNoFrameStyle",PROPLIST_ITEM(id++,"������ſ�����",strHelp,"0.ԲȦ|1.��Բ����|2.��ͨ����|3.�Զ��ж�"));
	AddPropItem("dim_map.PartNo.iStdInsertPlatePartNoFrameStyle",PROPLIST_ITEM(id++,"��׼����ſ�����",strHelp,"0.ԲȦ|1.��Բ����|2.��ͨ����|3.�Զ��ж�"));
	AddPropItem("dim_map.PartNo.fPartNoCirD",PROPLIST_ITEM(id++,"��ſ򳤶�","��ſ򳤶ȣ�������ֵΪ0ʱϵͳ������ָ��Զ������ſ򳤶�"));
	AddPropItem("dim_map.PartNo.fMainPoleNo",PROPLIST_ITEM(id++,"���ı��"));
	AddPropItem("dim_map.PartNo.fMainPoleNoOuterCirD",PROPLIST_ITEM(id++,"��Բֱ��"));
	AddPropItem("dim_map.PartNo.fMainPoleNoInnerCirD",PROPLIST_ITEM(id++,"��Բֱ��"));
	AddPropItem("dim_map.PartNo.iMirMsg",PROPLIST_ITEM(id++,"�ԳƷ�ʽ","��ע���ı��ʱ�ĶԳƷ�ʽ","0.X��Գ�|1.Y��Գ�|2.�޶Գ�"));
	AddPropItem("dim_map.PartNo.fPartNoTextSize",PROPLIST_ITEM(id++,"��������ָ�"));
	AddPropItem("dim_map.PartNo.fPartNoMargin",PROPLIST_ITEM(id++,"�ֿ��϶","������ſ�֮��ļ�϶���ñ������Ծ��α�ſ�������"));
	AddPropItem("dim_map.PartNo.bDrawRefLine",PROPLIST_ITEM(id++,"����������","��ע�������ʱ�Ƿ���Ҫ����������","��|��"));
	AddPropItem("dim_map.fSegITextSize",PROPLIST_ITEM(id++,"�κ��ָ�"));
	AddPropItem("dim_map.fSegICirD",PROPLIST_ITEM(id++,"�κſ򳤶�","�κſ򳤶ȣ�������ֵΪ0ʱϵͳ������ָ��Զ�����κſ򳤶�"));
	//����߶ȱ�ע��
	AddPropItem("PartNo",PROPLIST_ITEM(id++,"��������ָ�","����������ָ�"));
	AddPropItem("PartNoMargin",PROPLIST_ITEM(id++,"�ֿ��϶��","����������ſ�֮��ļ�϶ֵ"));
	AddPropItem("DimText",PROPLIST_ITEM(id++,"���ȱ�ע�ָ�","���ȳߴ��ע�ı���"));
	AddPropItem("DimAngleText",PROPLIST_ITEM(id++,"�Ƕȱ�ע�ָ�","�Ƕȳߴ��ע�ı���"));
	AddPropItem("HeaderText",PROPLIST_ITEM(id++,"����ͷ�ָ�","����ͷ���ָ�"));
	AddPropItem("CommonText",PROPLIST_ITEM(id++,"�������ָ�","��ͨ�����⺺�ָ�"));
	AddPropItem("NumericText",PROPLIST_ITEM(id++,"��������ָ�","������������ָ�"));
	AddPropItem("CutawayViewSymbolText",PROPLIST_ITEM(id++,"����ͼ���ָ�","����ͼ������߶�"));
	AddPropItem("CutawayViewSymbolText1",PROPLIST_ITEM(id++,"����ͼ������","����ͼ������"));
	AddPropItem("SimPolyText",PROPLIST_ITEM(id++,"�������ָ�","�����α�ע�����ָ�"));
	AddPropItem("dim_map.fSimTriScale",PROPLIST_ITEM(id++,"�����α���","�����α���"));
	AddPropItem("LsGuigeText",PROPLIST_ITEM(id++,"��˨����ָ�","��˨����ָ�"));
	AddPropItem("JgOddmentText",PROPLIST_ITEM(id++,"�Ǹָ�ͷ�ָ�","�Ǹ�����ͷ����߶�"));
	AddPropItem("PlateThickText",PROPLIST_ITEM(id++,"����ע�ָ�","�ְ��ȱ�ע����߶�"));
	AddPropItem("PartGuigeText",PROPLIST_ITEM(id++,"��������ָ�","��������볤���ָ�"));
	AddPropItem("OtherText",PROPLIST_ITEM(id++,"�����ָ�"));
	//�ߴ��ע
	AddPropItem("setMapSize",PROPLIST_ITEM(id++,"��ע��ʽ"));
	AddPropItem("dim_map.sArrowType",PROPLIST_ITEM(id++,"��ͷ����","","ʵ�ıպ�|���ıպ�|�պ�|�������|��б|30�Ƚ�|ֱ��"));
	AddPropItem("dim_map.fDimTextSize",PROPLIST_ITEM(id++,"�����ָ�"));
	AddPropItem("dim_map.fDimGap",PROPLIST_ITEM(id++,"��ע��϶","�������ע��֮��ļ�϶ֵ,Ĭ����С��϶ֵΪ�ָߵ�1/4"));
	AddPropItem("dim_map.fDimAngleTextSize",PROPLIST_ITEM(id++,"�Ƕ��ָ�"));
	AddPropItem("dim_map.fDimArrowSize",PROPLIST_ITEM(id++,"��ͷ��"));
	//���ϱ��ע
	AddPropItem("setMaterMap",PROPLIST_ITEM(id++,"���ϱ��ע"));
	AddPropItem("dim_map.fHeaderTextSize",PROPLIST_ITEM(id++,"��ͷ�ָ�"));
	AddPropItem("dim_map.fCommonTextSize",PROPLIST_ITEM(id++,"�����ָ�"));
	AddPropItem("dim_map.fNumericTextSize",PROPLIST_ITEM(id++,"�����ָ�"));
	AddPropItem("dim_map.sFontName",PROPLIST_ITEM(id++,"��������","��������,�ɽ�Ҫʹ�õ������ļ�*.shx���Ƶ�sysͼ�����Լ�CAD����Ŀ¼��,�����ڴ�ѡ��ʹ�ø������ļ�"));
	//����ͼ����ע
	AddPropItem("CutawayViewSymbol",PROPLIST_ITEM(id++,"����ͼ��"));
	AddPropItem("dim_map.fCutawayViewSymbolTextSize",PROPLIST_ITEM(id++,"�ָ�","����ͼ������߶�"));
	AddPropItem("dim_map.fCutawayViewSymbolLen",PROPLIST_ITEM(id++,"����","����ͼ������"));
	AddPropItem("dim_map.fCutawayViewPolyLineWidth",PROPLIST_ITEM(id++,"����߿��"));
	//�����α�ע
	AddPropItem("SimPolyLayer",PROPLIST_ITEM(id++,"������"));
	AddPropItem("dim_map.fSimPolyTextSize",PROPLIST_ITEM(id++,"�������ָ�"));
	AddPropItem("dim_map.fSimTriScale",PROPLIST_ITEM(id++,"�����α���"));
	//�����ָ߱�ע
	AddPropItem("Font",PROPLIST_ITEM(id++,"�����ָ�"));
	AddPropItem("dim_map.fLsGuigeTextSize",PROPLIST_ITEM(id++,"��˨���","��ע��˨���������С"));	
	AddPropItem("dim_map.fJgOddmentTextSize",PROPLIST_ITEM(id++,"�Ǹ�����ͷ","��ע�Ǹ�����ͷ�������С"));
	AddPropItem("dim_map.fPlateThickTextSize",PROPLIST_ITEM(id++,"�ְ���"));
	AddPropItem("dim_map.fPartGuigeTextSize",PROPLIST_ITEM(id++,"��������볤��"));
	AddPropItem("dim_map.fOtherTextSize",PROPLIST_ITEM(id++,"���຺��"));
	//ͼ��
	AddPropItem("LayerTable.layerHashList",PROPLIST_ITEM(id++,"ͼ��","","�����ͼ��"));
	AddPropItem("Layer.colorIndex",PROPLIST_ITEM(id++,"��ɫ"));
	AddPropItem("Layer.lineTypeName",PROPLIST_ITEM(id++,"����","����","CONTINUOUS|HIDDEN|DASHDOT2X|DIVIDE|ZIGZAG"));
	AddPropItem("Layer.lineWeight",PROPLIST_ITEM(id++,"�߿�","","0.00mm|0.05mm|0.09mm|0.13mm|0.15mm|0.18mm|0.20mm|0.25mm|0.30mm|0.35mm|0.40mm|0.50mm|0.53mm|0.60mm|0.70mm|0.80mm|0.90mm|1.00mm|1.06mm|1.20mm|1.40mm|1.58mm|2.00mm|2.11mm"));
	//ר��ͼ��
	AddPropItem("LayerTable",PROPLIST_ITEM(id++,"ר��ͼ��"));
	long iSpecialLayerId=id;
	AddPropItem("LayerTable.VisibleProfileLayer",PROPLIST_ITEM(id++,"�ɼ�������"));
	AddPropItem("LayerTable.UnvisibleProfileLayer",PROPLIST_ITEM(id++,"������"));
	AddPropItem("LayerTable.BriefLineLayer",PROPLIST_ITEM(id++,"�����ĵ���"));
	AddPropItem("LayerTable.AuxLineLayer",PROPLIST_ITEM(id++,"���ĵ���"));
	AddPropItem("LayerTable.BoltSymbolLayer",PROPLIST_ITEM(id++,"��˨"));
	AddPropItem("LayerTable.SimPolyLayer",PROPLIST_ITEM(id++,"������"));
	AddPropItem("LayerTable.AxisLineLayer",PROPLIST_ITEM(id++,"������"));
	AddPropItem("LayerTable.BendLineLayer",PROPLIST_ITEM(id++,"������"));
	AddPropItem("LayerTable.BreakLineLayer",PROPLIST_ITEM(id++,"�Ͽ�����"));
	AddPropItem("LayerTable.BoltLineLayer",PROPLIST_ITEM(id++,"��˨��"));
	AddPropItem("LayerTable.DimSizeLayer",PROPLIST_ITEM(id++,"�ߴ��ע"));
	AddPropItem("LayerTable.DimTextLayer",PROPLIST_ITEM(id++,"���ֱ�ע"));
	AddPropItem("LayerTable.CommonLayer",PROPLIST_ITEM(id++,"����"));
	AddPropItem("LayerTable.SonJgAxisLineLayer",PROPLIST_ITEM(id++,"�ӽǸ�����"));
	AddPropItem("LayerTable.AssistHuoQuLineLayer",PROPLIST_ITEM(id++,"����������"));
	AddPropItem("LayerTable.DamagedSymbolLine",PROPLIST_ITEM(id++,"�����������"));
#ifdef  __DRAWING_CONTEXT_
	m_xSpecialLayerIdArr.Empty();
	for(int i=0;i<16;i++)
		m_xSpecialLayerIdArr.append(iSpecialLayerId+i);
	MIN_SYSSETTING_PROPID=id;
#endif
#endif
}
#ifndef __DRAWING_CONTEXT_ 
int CSystemSettingDlg::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/,CPropTreeItem *pItem/*=NULL*/)
{
	return 0;
}
#else
int CSystemSettingDlg::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/,CPropTreeItem *pItem/*=NULL*/)
{
	BOOL bContinueFind=FALSE;	
	long handle=0;
	double dLineWeitht=0;
	CXhChar100 sText;
	if(m_pCurMapFrm!=NULL)
	{
		if(GetPropID("m_pCurMapFrm.style")==id)
			sText.Printf("%s",(char*)m_pCurMapFrm->sMapFrameName);
		else if(GetPropID("m_pCurMapFrm.nLength")==id)
		{
			sText.Printf("%d",m_pCurMapFrm->nLength);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_pCurMapFrm.nWidth")==id)
		{
			sText.Printf("%d",m_pCurMapFrm->nWidth);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_pCurMapFrm.nLeftMargin")==id)
		{
			sText.Printf("%d",m_pCurMapFrm->nLeftMargin);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_pCurMapFrm.nOtherMargin")==id)
		{
			sText.Printf("%d",m_pCurMapFrm->nTopMargin);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_pCurMapFrm.nBottomRightFixLen")==id)
		{
			sText.Printf("%d",m_pCurMapFrm->nBottomRightFixLen);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("m_pCurMapFrm.nBottomRightFixWidth")==id)
		{
			sText.Printf("%d",m_pCurMapFrm->nBottomRightFixWidth);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("m_pCurMapFrm.style")==id)
		sText.Copy("");
	if(GetPropID("general_set.LineMapScale")==id)
	{
		sText.Printf("%g",sys.general_set.LineMapScale);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("general_set.ForceLineScale")==id)
	{
		sText.Printf("%g",sys.general_set.ForceLineScale);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("general_set.StruMapScale")==id)
	{
		sText.Printf("%g",sys.general_set.StruMapScale.fRealToDraw);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("general_set.StruMapScale.iBoltSymbolStyle")==id)
	{
		if(sys.general_set.StruMapScale.iBoltSymbolStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.round hole");
		else if(sys.general_set.StruMapScale.iBoltSymbolStyle==1)
			sText.Copy("1.symbol");
		else if(sys.general_set.StruMapScale.iBoltSymbolStyle==2)
			sText.Copy("2.scale symbol");
		else if(sys.general_set.StruMapScale.iBoltSymbolStyle==3)
			sText.Copy("3.scale diameter symbol");
#else
			sText.Copy("0.Բ��");
		else if(sys.general_set.StruMapScale.iBoltSymbolStyle==1)
			sText.Copy("1.ͼ��");
		else if(sys.general_set.StruMapScale.iBoltSymbolStyle==2)
			sText.Copy("2.����ͼ��");
		else if(sys.general_set.StruMapScale.iBoltSymbolStyle==3)
			sText.Copy("3.����ֱ��ͼ��");
#endif
	}
	else if(GetPropID("general_set.StruMapScale.fBoltSymbolFactor")==id)
	{
		if(sys.general_set.StruMapScale.iBoltSymbolStyle==2)
			sText.Printf("%g",sys.general_set.StruMapScale.fBoltSymbolFactor);
		else if(sys.general_set.StruMapScale.iBoltSymbolStyle==3)
			sText.Printf("%g",sys.general_set.StruMapScale.fBoltDiaFactor);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("general_set.PartMapScale")==id)
	{
		sText.Printf("%g",sys.general_set.PartMapScale.fRealToDraw);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("general_set.PartMapScale.iBoltSymbolStyle")==id)
	{
		if(sys.general_set.PartMapScale.iBoltSymbolStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.round hole");
		else if(sys.general_set.PartMapScale.iBoltSymbolStyle==1)
			sText.Copy("1.symbol");
		else if(sys.general_set.PartMapScale.iBoltSymbolStyle==2)
			sText.Copy("2.scale symbol");
		else if(sys.general_set.PartMapScale.iBoltSymbolStyle==3)
			sText.Copy("3.scale diameter symbol");
#else
			sText.Copy("0.Բ��");
		else if(sys.general_set.PartMapScale.iBoltSymbolStyle==1)
			sText.Copy("1.ͼ��");
		else if(sys.general_set.PartMapScale.iBoltSymbolStyle==2)
			sText.Copy("2.����ͼ��");
		else if(sys.general_set.PartMapScale.iBoltSymbolStyle==3)
			sText.Copy("3.����ֱ��ͼ��");
#endif
	}
	else if(GetPropID("general_set.PartMapScale.fBoltSymbolFactor")==id)
	{
		if(sys.general_set.PartMapScale.iBoltSymbolStyle==2)
			sText.Printf("%g",sys.general_set.PartMapScale.fBoltSymbolFactor);
		else if(sys.general_set.PartMapScale.iBoltSymbolStyle==3)
			sText.Printf("%g",sys.general_set.PartMapScale.fBoltDiaFactor);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("general_set.WeldMapScale")==id)
		sText.Printf("%g",sys.general_set.WeldMapScale.fRealToDraw);
	else if(GetPropID("general_set.WeldMapScale.iBoltSymbolStyle")==id)
	{
		if(sys.general_set.WeldMapScale.iBoltSymbolStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.round hole");
		else if(sys.general_set.WeldMapScale.iBoltSymbolStyle==1)
			sText.Copy("1.symbol");
		else if(sys.general_set.WeldMapScale.iBoltSymbolStyle==2)
			sText.Copy("2.scale symbol");
		else if(sys.general_set.WeldMapScale.iBoltSymbolStyle==3)
			sText.Copy("3.scale diameter symbol");
#else
			sText.Copy("0.Բ��");
		else if(sys.general_set.WeldMapScale.iBoltSymbolStyle==1)
			sText.Copy("1.ͼ��");
		else if(sys.general_set.WeldMapScale.iBoltSymbolStyle==2)
			sText.Copy("2.����ͼ��");
		else if(sys.general_set.WeldMapScale.iBoltSymbolStyle==3)
			sText.Copy("3.����ֱ��ͼ��");
#endif
	}
	else if(GetPropID("general_set.WeldMapScale.fBoltSymbolFactor")==id)
	{
		if(sys.general_set.WeldMapScale.iBoltSymbolStyle==2)
			sText.Printf("%g",sys.general_set.WeldMapScale.fBoltSymbolFactor);
		else if(sys.general_set.WeldMapScale.iBoltSymbolStyle==3)
			sText.Printf("%g",sys.general_set.WeldMapScale.fBoltDiaFactor);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("general_set.iMatCharPosType")==id)
	{
		sys.GetPublicSysParaFromReg("Settings","MatCharPosType");
		if(sys.general_set.iMatCharPosType==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","0.not needed");
		else if(sys.general_set.iMatCharPosType==1)
			sText.Printf("%s","1.before part number");
		else if(sys.general_set.iMatCharPosType==2)
			sText.Printf("%s","2.after part number");
#else
			sText.Printf("%s","0.����Ҫ�����ַ�");
		else if(sys.general_set.iMatCharPosType==1)
			sText.Printf("%s","1.�������ǰ");
		else if(sys.general_set.iMatCharPosType==2)
			sText.Printf("%s","2.������ź�");
#endif
	}
	else if(GetPropID("general_set.cMatCharSeparator")==id)
	{
		sys.GetPublicSysParaFromReg("Settings","MatSeparator");
#ifdef AFX_TARG_ENU_ENGLISH
		if(sys.general_set.cMatCharSeparator==0)
			sText.Copy("No");
		else if(sys.general_set.cMatCharSeparator==' ')
			sText.Copy("Spacing");
		else 
			sText.Printf("%c",sys.general_set.cMatCharSeparator);
#else 
		if(sys.general_set.cMatCharSeparator==0)
			sText.Copy("��");
		else if(sys.general_set.cMatCharSeparator==' ')
			sText.Copy("�ո�");
		else 
			sText.Printf("%c",sys.general_set.cMatCharSeparator);
#endif
	}
	else if(GetPropID("general_set.iPartBOMStyle")==id)
	{
		if(sys.general_set.iPartBOMStyle==1)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","1.Broaden Style");
		else //if(sys.general_set.iPartBOMStyle==0)
			sText.Printf("%s","0.Standard Style");
#else
			sText.Printf("%s","1.�ӿ���ʽ");
		else //if(sys.general_set.iPartBOMStyle==0)
			sText.Printf("%s","0.��׼��ʽ");
#endif
	}
	else if(GetPropID("general_set.iSumMatBOMStyle")==id)
	{
		if(sys.general_set.iSumMatBOMStyle==1)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","1.Style 2");
		else //if(sys.general_set.iSumMatBOMStyle==0)
			sText.Printf("%s","0.Style 1");
#else
			sText.Printf("%s","1.��ʽ��");
		else //if(sys.general_set.iSumMatBOMStyle==0)
			sText.Printf("%s","0.��ʽһ");
#endif
	}
	else if(GetPropID("general_set.fHoleAugmentInNC")==id)
	{
		sText.Printf("%.2f",sys.general_set.fHoleAugmentInNC);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("general_set.fWeldLineSymbolHigh")==id)
	{
		sText.Printf("%.2f",sys.general_set.fWeldLineSymbolHigh);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("general_set.fWeldLineSymbolSpace")==id)
	{
		sText.Printf("%.2f",sys.general_set.fWeldLineSymbolSpace);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("general_set.iDimPrecision")==id)
	{
		if(sys.general_set.iDimPrecision==0)
			sText.Printf("%s","1.0mm");
		else if(sys.general_set.iDimPrecision==1)
			sText.Printf("%s","0.5mm");
		else if(sys.general_set.iDimPrecision==2)
			sText.Printf("%s","0.1mm");
	}
	else if(GetPropID("nBriefGuigeN")==id)
		sText.Printf("%d",sys.nBriefGuigeN);
	else if(GetPropID("bDrawMainPoleJointLine")==id)
	{
		if(sys.bDrawMainPoleJointLine)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("Yes");
		else	
			sText.Copy("No");
#else
			sText.Copy("��");
		else	
			sText.Copy("��");
#endif
	}	
	else if(GetPropID("nJointLineOffset")==id)
		sText.Printf("%d",sys.nJointLineOffset);
	else if(GetPropID("nJointLineLen")==id)
		sText.Printf("%d",sys.nJointLineLen);
	//���տ�·���������з�ʽ
	else if(GetPropID("general_set.bCardLayoutBaseRow")==id)
	{
		if(sys.general_set.bCardLayoutBaseRow)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","0.by the reference of row");
		else
			sText.Printf("%s","1.by the reference of column");
#else
			sText.Printf("%s","0.����Ϊ��׼");
		else
			sText.Printf("%s","1.����Ϊ��׼");
#endif
	}
	else if(GetPropID("general_set.nPartRowsN")==id)
		sText.Printf("%d",sys.general_set.nPartRowsN);
	else if(GetPropID("general_set.nRowHigh")==id)
		sText.Printf("%d",sys.general_set.nRowHigh);
	else if(GetPropID("general_set.nColumnWidth")==id)
		sText.Printf("%d",sys.general_set.nColumnWidth);
	else if(GetPropID("general_set.bHasFrame")==id)
	{
		if(sys.general_set.bHasFrame)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("general_set.bProcessCardTbl")==id)
	{
		if(sys.general_set.bProcessCardTbl)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("general_set.m_sJgCard")==id)
		sText.Printf("%s",&sys.general_set.m_sJgCard);
	else if(GetPropID("general_set.m_sSlotCard")==id)
		sText.Printf("%s",&sys.general_set.m_sSlotCard);
	else if(GetPropID("general_set.m_sTubeCard")==id)
		sText.Printf("%s",&sys.general_set.m_sTubeCard);
	else if(GetPropID("general_set.m_sPlankCard")==id)
		sText.Printf("%s",&sys.general_set.m_sPlankCard);
	else if(GetPropID("general_set.m_sWeldPartGroupCard")==id)
		sText.Printf("%s",&sys.general_set.m_sWeldPartGroupCard);
	else if(GetPropID("general_set.m_sProcessPartStatCard")==id)
		sText.Printf("%s",&sys.general_set.m_sProcessPartStatCard);
	else if(GetPropID("general_set.m_sTMAMapArxPath")==id)
		sText.Printf("%s",&sys.general_set.m_sTMAMapArxPath);
	else if(GetPropID("general_set.m_sLMAMapArxPath")==id)
		sText.Printf("%s",&sys.general_set.m_sLMAMapArxPath);
	else if(GetPropID("general_set.m_sLDSMapArxPath")==id)
		sText.Printf("%s",&sys.general_set.m_sLDSMapArxPath);
	else if(GetPropID("general_set.m_sTSAMapArxPath")==id)
		sText.Printf("%s",&sys.general_set.m_sTSAMapArxPath);
	//
	else if(GetPropID("general_set.lCrossarmCr")==id)
		sText.Printf("RGB%X",sys.general_set.lCrossarmCr);
	else if(GetPropID("general_set.rCrossarmCr")==id)
		sText.Printf("RGB%X",sys.general_set.rCrossarmCr);
	//�ṹͼ
	else if(GetPropID("stru_map.bExportByEnglish")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		sText.Copy("1.Ӣ��");
#else
		if(sys.stru_map.bExportByEnglish)
			sText.Copy("1.Ӣ��");
		else
			sText.Copy("0.����");
#endif
	}
	else if(GetPropID("stru_map.fMinWingWide")==id)
	{
		sText.Printf("%.1f",sys.stru_map.fMinWingWide);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("stru_map.fMinPartThick")==id)
	{
		sText.Printf("%.1f",sys.stru_map.fMinPartThick);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("stru_map.fHoleAugmentInNC")==id)
	{
		sText.Printf("%.1f",sys.general_set.fHoleAugmentInNC);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("stru_map.bDimSpecialLsSpace")==id)
	{
		if(sys.stru_map.bDimSpecialLsSpace)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("stru_map.bDimSpecialJgZhun")==id)
	{
		if(sys.stru_map.bDimSpecialJgZhun)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("stru_map.bDimStruJgLen")==id)
	{
		if(sys.stru_map.bDimStruJgLen)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("stru_map.bDimPartGuiGe")==id)
	{
		if(sys.stru_map.bDimPartGuiGe)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("stru_map.bReadAdditionalBuffer")==id)
	{
		if(sys.stru_map.bReadAdditionalBuffer)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("stru_map.bDrawCutAngleAndWingInfo")==id)
	{
		if(sys.stru_map.bDrawCutAngleAndWingInfo)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("stru_map.bOnlyDrawHorizontalPlate")==id)
	{
		if(sys.stru_map.bOnlyDrawHorizontalPlate)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("stru_map.bUsePolyLineDrawEdge")==id)
	{
		if(sys.stru_map.bUsePolyLineDrawEdge)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.part_table.bZoomInProcessCard")==id)
	{
		if(sys.part_group_map.part_table.bZoomInProcessCard)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("stru_map.fPolyLineWidth")==id)
	{
		sText.Printf("%.2f",sys.stru_map.fPolyLineWidth);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("stru_map.fPoleAxisLineStartOutSpreadLen")==id)
	{
		sText.Printf("%.2f",sys.stru_map.fPoleAxisLineStartOutSpreadLen);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("stru_map.fPoleAxisLineEndOutSpreadLen")==id)
	{
		sText.Printf("%.2f",sys.stru_map.fPoleAxisLineEndOutSpreadLen);
		SimplifiedNumString(sText);
	}
	//�ṹͼ�ָ�����
	else if(GetPropID("dim_map.fLsGuigeTextSize")==id)
	{
		sText.Printf("%.2f",sys.fonts.TextSize("LsGuigeText"));
		SimplifiedNumString(sText);
	}
	else if(GetPropID("dim_map.fJgOddmentTextSize")==id)
	{
		sText.Printf("%.2f",sys.fonts.TextSize("JgOddmentText"));
		SimplifiedNumString(sText);
	}
	else if(GetPropID("dim_map.fSimPolyTextSize")==id)
	{
		sText.Printf("%.2f",sys.fonts.TextSize("SimPolyText"));
		SimplifiedNumString(sText);
	}
	else if(GetPropID("dim_map.fPlateThickTextSize")==id)
	{
		sText.Printf("%.2f",sys.fonts.TextSize("PlateThickText"));
		SimplifiedNumString(sText);
	}
	else if(GetPropID("dim_map.fOtherTextSize")==id)
	{
		sText.Printf("%.2f",sys.fonts.TextSize("OtherText"));
		SimplifiedNumString(sText);
	}
	else if(GetPropID("dim_map.fPartGuigeTextSize")==id)
	{
		sText.Printf("%.2f",sys.fonts.TextSize("PartGuigeText"));
		SimplifiedNumString(sText);
	}
	else if(GetPropID("dim_map.fSimPolyTextSize")==id)
	{
		sText.Printf("%.2f",sys.fonts.TextSize("SimPolyText"));
		SimplifiedNumString(sText);
	}
	else if(GetPropID("dim_map.fSimTriScale")==id)
	{
		sText.Printf("%.2f",sys.dim_map.fSimTriScale);
		SimplifiedNumString(sText);
	}
	//�麸ͼ
	else if(GetPropID("part_group_map.fMinFontSize")==id)
	{
		sText.Printf("%.2f",sys.part_group_map.fMinFontSize);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("part_group_map.fMinDrawThick")==id)
	{
		sText.Printf("%.2f",sys.part_group_map.fMinDrawThick);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("part_group_map.nKmJgGIncrement")==id)
		sText.Printf("%d",sys.part_group_map.nKmJgGIncrement);
	else if(GetPropID("part_group_map.bDrawWeldLineSymbol")==id)
	{
		if(sys.part_group_map.bDrawWeldLineSymbol)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.iPlateLocateDrawStyle")==id)
	{
		if(sys.part_group_map.iPlateLocateDrawStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Don't draw");
		else if(sys.part_group_map.iPlateLocateDrawStyle==1)
			sText.Copy("1.Three vertical bar");
		else
			sText.Copy("2.The pennant");
#else
			sText.Copy("0.������");
		else if(sys.part_group_map.iPlateLocateDrawStyle==1)
			sText.Copy("1.������");
		else
			sText.Copy("2.������");
#endif
	}
	else if(GetPropID("part_group_map.iLocateSymboleHeight")==id)
	{
		sText.Printf("%d",sys.part_group_map.iLocateSymboleHeight);
	}
	else if(GetPropID("part_group_map.bDimEndOuterPlaneSpace")==id)
	{
		if(sys.part_group_map.bDimEndOuterPlaneSpace)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.bAutoDimPartNo")==id)
	{
		if(sys.part_group_map.bAutoDimPartNo)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.bAutoMatchView")==id)
	{
		if(sys.part_group_map.bAutoMatchView)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.bModulatePartGroupLen")==id)
	{
		if(sys.part_group_map.bModulatePartGroupLen)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.fPartGroupDistThreshold")==id)
	{
		sText.Printf("%.2f",sys.part_group_map.fPartGroupDistThreshold);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("part_group_map.fPartGroupDistZoomCoef")==id)
	{
		sText.Printf("%.2f",sys.part_group_map.fPartGroupDistZoomCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("part_group_map.bAngularDimension")==id)
	{
		if(sys.part_group_map.bAngularDimension)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.bDimBeforeWeldLocatSpace")==id)
	{
		if(sys.part_group_map.bDimBeforeWeldLocatSpace)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.bDimAfterWeldCheckLine")==id)
	{
		if(sys.part_group_map.bDimAfterWeldCheckLine)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.bDimAbsoluteDist")==id)
	{
		if(sys.part_group_map.bDimAbsoluteDist)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.bDimPlatePoleRayAndAngle")==id)
	{
		if(sys.part_group_map.bDimPlatePoleRayAndAngle)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.bSmartFilterRedundantPart")==id)
	{
		if(sys.part_group_map.bSmartFilterRedundantPart)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.bCreatePartGroupBOM")==id)
	{
		if(sys.part_group_map.bCreatePartGroupBOM)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("welding.textsize.bZoomWithScale")==id)
	{
		if(sys.welding.bZoomAdaptiveWithScale)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("welding.textsize.wiScale")==id)
		sText.Printf("%d",sys.welding.wiScaleModel2Draw);
	//�麸����ϸ��
	else if(GetPropID("part_group_map.part_table.bDispPartGroupNo")==id)
	{
		if(sys.part_group_map.part_table.bDispPartGroupNo)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.part_table.bDispPartGroupNum")==id)
	{
		if(sys.part_group_map.part_table.bDispPartGroupNum)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.part_table.bDispPartNo")==id)
	{
		if(sys.part_group_map.part_table.bDispPartNo)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.part_table.bDispPartNum")==id)
	{
		if(sys.part_group_map.part_table.bDispPartNum)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.part_table.bDispPartGuiGe")==id)
	{
		if(sys.part_group_map.part_table.bDispPartGuiGe)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.part_table.bDispPartLen")==id)
	{
		if(sys.part_group_map.part_table.bDispPartLen)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.part_table.bDispPartWeight")==id)
	{
		if(sys.part_group_map.part_table.bDispPartWeight)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.part_table.bDispNotes")==id)
	{
		if(sys.part_group_map.part_table.bDispNotes)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.part_table.bDispInternal")==id)
	{
		if(sys.part_group_map.part_table.bDispInternal)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else if(GetPropID("part_group_map.part_table.bDispAppearance")==id)
	{
		if(sys.part_group_map.part_table.bDispAppearance)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","Yes");
		else
			sText.Printf("%s","No");
#else
			sText.Printf("%s","��");
		else
			sText.Printf("%s","��");
#endif
	}
	else
		bContinueFind=TRUE;
	//ͼֽ��ע
	if(bContinueFind)
	{	//if else��ι����ڴ˴��
		bContinueFind=FALSE;
		//����ͼ
		if(GetPropID("part_map.iExportDeformedProfile")==id)
		{
			if(sys.part_map.iExportDeformedProfile==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","0.Do not onsider all");
			else if(sys.part_map.iExportDeformedProfile==1)
				sText.Printf("%s","1.Consider all");
			else if(sys.part_map.iExportDeformedProfile==2)
				sText.Printf("%s","2.Consider the details");
			else if(sys.part_map.iExportDeformedProfile==3)
				sText.Printf("%s","3.Consider component diagram");
#else
				sText.Printf("%s","0.������");
			else if(sys.part_map.iExportDeformedProfile==1)
				sText.Printf("%s","1.����");
			else if(sys.part_map.iExportDeformedProfile==2)
				sText.Printf("%s","2.����ͼ����");
			else if(sys.part_map.iExportDeformedProfile==3)
				sText.Printf("%s","3.���տ�����");
#endif
		}
		else if(GetPropID("part_map.bDimDeformedProfileText")==id)
		{
			if(sys.part_map.bDimDeformedProfileText)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.fMinHuoquAngle")==id)
		{
			sText.Printf("%f",sys.part_map.fMinHuoquAngle);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("part_map.bDimPlaneAngle")==id)
		{
			if(sys.part_map.bDimPlaneAngle)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.iDimAngleType")==id)
		{
			if(sys.part_map.iDimAngleType==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","0.Acute Angle");
			else if(sys.part_map.iDimAngleType==1)
				sText.Printf("%s","1.Obtuse Angle");
			else 
				sText.Printf("%s","2.Automatically");
#else
				sText.Printf("%s","0.���");
			else if(sys.part_map.iDimAngleType==1)
				sText.Printf("%s","1.�۽�");
			else 
				sText.Printf("%s","2.�Զ�");
#endif
		}
		else if(GetPropID("part_map.bDimHuoquType")==id)
		{
			if(sys.part_map.bDimHuoquType)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.bUseZhengFanQun")==id)
		{
			if(sys.part_map.bUseZhengFanQun)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.iPartMapStyle")==id)
		{
			if(sys.part_map.iPartMapStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","0.Style 1");
			else
				sText.Printf("%s","1.Style 2");
#else
				sText.Printf("%s","0.��ʽһ");
			else
				sText.Printf("%s","1.��ʽ��");
#endif
		}
		//�Ǹֹ���ͼ
		else if(GetPropID("part_map.angle.fKaiHeJiaoThreshold")==id)
		{	//��ע����б�������ݸ��¿��ϽǶ�����ֵ wht 11-06-13
			sys.GetPublicSysParaFromReg("Settings","KaiHeJiaoThreshold");
			sText.Printf("%.2f",sys.part_map.angle.fKaiHeJiaoThreshold);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("part_map.angle.fKaiHeJiaoMinDeita")==id)
		{	//��ע����б�������ݸ��¿��ϽǶ�����ֵ wht 11-06-13
			sys.GetPublicSysParaFromReg("Settings","KaiHeJiaoMinDeita");
			sText.Printf("%f",sys.part_map.angle.fKaiHeJiaoMinDeita);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("part_map.angle.iJgZoomSchema")==id)
		{
			if(sys.part_map.angle.iJgZoomSchema==1)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("1.Use part map scale");
			else if(sys.part_map.angle.iJgZoomSchema==2)
				sText.Printf("2.Zoom width and length to the same scale");
			else if(sys.part_map.angle.iJgZoomSchema==3)
				sText.Printf("3.Zoom length and width respectively");
			else 
				sText.Printf("0.No zoom 1:1 drawing");
#else
				sText.Printf("1.ʹ�ù���ͼ����");
			else if(sys.part_map.angle.iJgZoomSchema==2)
				sText.Printf("2.����ͬ������");
			else if(sys.part_map.angle.iJgZoomSchema==3)
				sText.Printf("3.����ֱ�����");
			else 
				sText.Printf("0.������1:1����");
#endif
		}
		else if(GetPropID("part_map.angle.ciJgLenDrawMode")==id)
		{
			if(sys.part_map.angle.ciJgLenDrawMode==1)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("1.Extrusion To Middle");
			else 
				sText.Printf("0.Extend To Right");
#else
				sText.Printf("1.�м伷ѹ");
			else 
				sText.Printf("0.��������");
#endif
		}
		else if(GetPropID("part_map.angle.bMaxExtendAngleLength")==id)
		{
			if(!sys.part_map.angle.bMaxExtendAngleLength)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("0.No");
			else
				sText.Printf("1.Yes");
#else
				sText.Printf("0.��");
			else
				sText.Printf("1.��");
#endif
		}
		else if(GetPropID("part_map.angle.bModulateLongJg")==id)
		{
			if(sys.part_map.angle.bModulateLongJg)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.angle.fLsDistThreshold")==id)
		{
			sText.Printf("%.2f",sys.part_map.angle.fLsDistThreshold);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("part_map.angle.fLsDistZoomCoef")==id)
		{
			sText.Printf("%.2f",sys.part_map.angle.fLsDistZoomCoef);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("part_map.angle.bOneCardMultiPart")==id)
		{
			if(sys.part_map.angle.bOneCardMultiPart)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.angle.bJgUseSimpleLsMap")==id)
		{
			if(sys.part_map.angle.bJgUseSimpleLsMap)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.angle.bDimLsAbsoluteDist")==id)
		{
			if(sys.part_map.angle.bDimLsAbsoluteDist)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.angle.bMergeLsAbsoluteDist")==id)
		{
			if(sys.part_map.angle.bMergeLsAbsoluteDist)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else
				sText.Copy("No");
#else
				sText.Copy("��");
			else
				sText.Copy("��");
#endif
		}
		else if(GetPropID("part_map.angle.bDimRibPlatePartNo")==id)
		{
			if(sys.part_map.angle.bDimRibPlatePartNo)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.angle.bDimRibPlateMaterial")==id)
		{
			if(sys.part_map.angle.bDimRibPlateMaterial)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.angle.bDimRibPlateSetUpPos")==id)
		{
			if(sys.part_map.angle.bDimRibPlateSetUpPos)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.angle.iCutAngleDimType")==id)
		{
			if(sys.part_map.angle.iCutAngleDimType==1)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","1.Style 2");				
			else
				sText.Printf("%s","0.Style 1");
#else			
				sText.Printf("%s","1.��ʽ��");
			else
				sText.Printf("%s","0.��ʽһ");
#endif
		}
		else if(GetPropID("part_map.angle.iJgGDimStyle")==id)
		{
			if(sys.part_map.angle.iJgGDimStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","0.Starting");
			else if(sys.part_map.angle.iJgGDimStyle==1)
				sText.Printf("%s","1.Middle");
			else 
				sText.Printf("%s","2.Automatically judge");
#else
				sText.Printf("%s","0.ʼ��");
			else if(sys.part_map.angle.iJgGDimStyle==1)
				sText.Printf("%s","1.�м�");
			else 
				sText.Printf("%s","2.�Զ��ж�");
#endif
		}
		else if(GetPropID("part_map.angle.nMaxBoltNumStartDimG")==id)
		{
			sText.Printf("%d",sys.part_map.angle.nMaxBoltNumStartDimG);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("part_map.angle.nMaxBoltNumAlongX")==id)
		{
			sText.Printf("%d",sys.part_map.angle.nMaxBoltNumAlongX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("part_map.angle.iLsSpaceDimStyle")==id)
		{
			if(sys.part_map.angle.iLsSpaceDimStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","0.X axis direction");
			else if(sys.part_map.angle.iLsSpaceDimStyle==1)
				sText.Printf("%s","1.Y axis direction");
			else if(sys.part_map.angle.iLsSpaceDimStyle==3)
				sText.Printf("%s","3.Not dimension");
			else if(sys.part_map.angle.iLsSpaceDimStyle==4)
				sText.Printf("%s","4.Only dimension size line");
			else
				sText.Printf("%s","2.Automatic judgment");
#else
				sText.Printf("%s","0.��X�᷽��");
			else if(sys.part_map.angle.iLsSpaceDimStyle==1)
				sText.Printf("%s","1.��Y�᷽��");
			else if(sys.part_map.angle.iLsSpaceDimStyle==3)
				sText.Printf("%s","3.����ע���");
			else if(sys.part_map.angle.iLsSpaceDimStyle==4)
				sText.Printf("%s","4.����ߴ���");
			else
				sText.Printf("%s","2.�Զ��ж�");
#endif
		}
		else if(GetPropID("part_map.angle.bDimPushFlatMap")==id)
		{
			if(sys.part_map.angle.bDimPushFlatMap)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.angle.bDimCutAngle")==id)
		{
			if(sys.part_map.angle.bDimCutAngle)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.angle.bDimKaiHe")==id)
		{
			if(sys.part_map.angle.bDimKaiHe)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.angle.bDimKaiheSumLen")==id)
		{
			if(sys.part_map.angle.bDimKaiheSumLen)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.angle.bDimKaiheAngle")==id)
		{
			if(sys.part_map.angle.bDimKaiheAngle)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.angle.bDimKaiheSegLen")==id)
		{
			if(sys.part_map.angle.bDimKaiheSegLen)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.angle.bDimKaiheScopeMap")==id)
		{
			if(sys.part_map.angle.bDimKaiheScopeMap)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.angle.bDimKaiheAngleMap")==id)
		{
			if(sys.part_map.angle.bDimKaiheAngleMap)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.angle.bDimCutAngleMap")==id)
		{
			if(sys.part_map.angle.bDimCutAngleMap)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		//�ְ幹��ͼ
		else if(GetPropID("part_map.plate.bDimPlateEdgeLen")==id)
		{
			if(sys.part_map.plate.bDimPlateEdgeLen)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.plate.bDrawInnerEdge")==id)
		{
			if(sys.part_map.plate.bDrawInnerEdge)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if (GetPropID("part_map.plate.fInnerEdgeOffset")==id)
			sText.Printf("%.1f",sys.part_map.plate.fInnerEdgeOffset);
		else if(GetPropID("part_map.plate.bDimCoord")==id)
		{
			if(sys.part_map.plate.bDimCoord)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.plate.bAdjustPlateUcs")==id)
		{
			if(sys.part_map.plate.bAdjustPlateUcs)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.plate.bDimArcEdgeR")==id)
		{
			if(sys.part_map.plate.bDimArcEdgeR)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.plate.bDimArcAngle")==id)
		{
			if(sys.part_map.plate.bDimArcAngle)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.plate.bDimPlateRayLs")==id)
		{
			if(sys.part_map.plate.bDimPlateRayLs)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.plate.bDimPlateBoltAngle")==id)
		{
			if(sys.part_map.plate.bDimPlateBoltAngle)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.plate.bDimBoltEdgeSpace")==id)
		{
			if(sys.part_map.plate.bDimBoltEdgeSpace)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.plate.bDimDiffRayLsSpace")==id)
		{
			if(sys.part_map.plate.bDimDiffRayLsSpace)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.plate.bDimLsToHuoquLine")==id)
		{
			if(sys.part_map.plate.bDimLsToHuoquLine)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.plate.bDimLsToWeldEdge")==id)
		{
			if(sys.part_map.plate.bDimLsToWeldEdge)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.plate.bPlateUseSimpleLsMap")==id)
		{
			if(sys.part_map.plate.bPlateUseSimpleLsMap)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.plate.bZoomDimTextSizeByWidth")==id)
		{
			if(sys.part_map.plate.bZoomDimTextSizeByWidth)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("part_map.plate.fTextSizeZoomCoef")==id)
		{
			sText.Printf("%.2f",sys.part_map.plate.fTextSizeZoomCoef);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("part_map.plate.bDrawSlopeCutLine")==id)
		{
			if(sys.part_map.plate.bDrawSlopeCutLine)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else
				sText.Printf("No");
#else
				sText.Printf("��");
			else
				sText.Printf("��");
#endif
		}
		else if(GetPropID("part_map.plate.bDrawDatumLine")==id)
		{
			if(sys.part_map.plate.bDrawDatumLine)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else
				sText.Printf("No");
#else
				sText.Printf("��");
			else
				sText.Printf("��");
#endif
		}
		else if(GetPropID("part_map.plate.iZhiWanLineType")==id)
		{
			if(sys.part_map.plate.iZhiWanLineType==0)
				sText.Copy("0.���");
			else if(sys.part_map.plate.iZhiWanLineType==1)
				sText.Copy("1.�ڲ�");
		}
		else if(GetPropID("part_map.plate.fInnerOffThreshold")==id)
		{
			sText.Printf("%f",sys.part_map.plate.fInnerOffThreshold);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("part_map.tube.fSlotLenProcessError")==id)
		{
			sText.Printf("%.2f",sys.part_map.tube.fSlotLenProcessError);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("part_map.tube.fSlotWidthProcessError")==id)
		{
			sText.Printf("%.2f",sys.part_map.tube.fSlotWidthProcessError);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("part_map.tube.fReelTubeThresholdD")==id)
		{
			sText.Printf("%.2f",sys.part_map.tube.fReelTubeThresholdD);
			SimplifiedNumString(sText);
		}
		else
			bContinueFind=TRUE;
	}
	//ͼֽ��ע
	if(bContinueFind)
	{	//if else��ι����ڴ˴��
		if(GetPropID("part_map.plate.iCutArcsType")==id)
		{
			if(sys.part_map.plate.iCutArcsType==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","0.No cutting");
			else if(sys.part_map.plate.iCutArcsType==1)
				sText.Printf("%s","1.Cutting angle");
			else 
				sText.Printf("%s","2.Cutting arc");
#else
				sText.Printf("%s","0.����");
			else if(sys.part_map.plate.iCutArcsType==1)
				sText.Printf("%s","1.�н�");
			else 
				sText.Printf("%s","2.�л�");
#endif
		}
		else if(GetPropID("part_map.plate.bMarkHuoquPos")==id)
		{
			if (sys.part_map.plate.bMarkHuoquPos)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else
				sText.Printf("No");
#else
				sText.Printf("��");
			else
				sText.Printf("��");
#endif
		}
		else if(GetPropID("part_map.plate.bDrawHuoQuPrjMap")==id)
		{
			if (sys.part_map.plate.bDrawHuoQuPrjMap)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else
				sText.Printf("No");
#else
				sText.Printf("��");
			else
				sText.Printf("��");
#endif
		}
		else if(GetPropID("part_map.plate.bDrawHuoQuMarkLine")==id)
		{
			if (sys.part_map.plate.bDrawHuoQuMarkLine)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else
				sText.Printf("No");
#else
				sText.Printf("��");
			else
				sText.Printf("��");
#endif
		}
		else if (GetPropID("part_map.plate.bDisplaypPartInfoTbl")==id)
		{
			if (sys.part_map.plate.bDisplaypPartInfoTbl)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else
				sText.Printf("No");
#else
				sText.Printf("��");
			else
				sText.Printf("��");
#endif
		}
		else if (GetPropID("part_map.plate.nPartInfoTblColumnWidth")==id)
			sText.Printf("%d",sys.part_map.plate.nPartInfoTblColumnWidth);
		//����
		else if(GetPropID("dim_map.sFontName")==id)
			sText.Printf("%s",&sys.dim_map.sFontName);
		else if(GetPropID("dim_map.fSegITextSize")==id)
		{
			sText.Printf("%.2f",sys.dim_map.fSegITextSize);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.fSegICirD")==id)
		{
			sText.Printf("%.2f",sys.dim_map.fSegICirD);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.PartNo.iPartNoFrameStyle")==id)
		{
			if(sys.dim_map.PartNo.iPartNoFrameStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","0.Circle");
			else if(sys.dim_map.PartNo.iPartNoFrameStyle==1)
				sText.Printf("%s","1.Waist round rectangle");
			else if(sys.dim_map.PartNo.iPartNoFrameStyle==2)
				sText.Printf("%s","2.Ordinary rectangular");
			else 
				sText.Printf("%s","3.Automatic judgment");
#else
				sText.Printf("%s","0.ԲȦ");
			else if(sys.dim_map.PartNo.iPartNoFrameStyle==1)
				sText.Printf("%s","1.��Բ����");
			else if(sys.dim_map.PartNo.iPartNoFrameStyle==2)
				sText.Printf("%s","2.��ͨ����");
			else 
				sText.Printf("%s","3.�Զ��ж�");
#endif
		}
		else if(GetPropID("dim_map.PartNo.iFlPartNoFrameStyle")==id)
		{
			if(sys.dim_map.PartNo.iFlPartNoFrameStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","0.Circle");
			else if(sys.dim_map.PartNo.iFlPartNoFrameStyle==1)
				sText.Printf("%s","1.Waist round rectangle");
			else if(sys.dim_map.PartNo.iFlPartNoFrameStyle==2)
				sText.Printf("%s","2.Ordinary rectangular");
			else 
				sText.Printf("%s","3.Automatic judgment");
#else
				sText.Printf("%s","0.ԲȦ");
			else if(sys.dim_map.PartNo.iFlPartNoFrameStyle==1)
				sText.Printf("%s","1.��Բ����");
			else if(sys.dim_map.PartNo.iFlPartNoFrameStyle==2)
				sText.Printf("%s","2.��ͨ����");
			else 
				sText.Printf("%s","3.�Զ��ж�");
#endif
		}
		else if(GetPropID("dim_map.PartNo.iStdInsertPlatePartNoFrameStyle")==id)
		{
			if(sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","0.Circle");
			else if(sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle==1)
				sText.Printf("%s","1.Waist round rectangle");
			else if(sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle==2)
				sText.Printf("%s","2.Ordinary rectangular");
			else 
				sText.Printf("%s","3.Automatic judgment");
#else
				sText.Printf("%s","0.ԲȦ");
			else if(sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle==1)
				sText.Printf("%s","1.��Բ����");
			else if(sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle==2)
				sText.Printf("%s","2.��ͨ����");
			else 
				sText.Printf("%s","3.�Զ��ж�");
#endif
		}
		else if(GetPropID("dim_map.PartNo.fPartNoTextSize")==id)
		{
			sText.Printf("%.2f",sys.fonts.TextSize("PartNo"));
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.PartNo.fPartNoCirD")==id)
		{
			sText.Printf("%.2f",sys.dim_map.PartNo.fPartNoCirD);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.PartNo.fMainPoleNoOuterCirD")==id)
		{
			sText.Printf("%.2f",sys.dim_map.PartNo.fMainPoleNoOuterCirD);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.PartNo.fMainPoleNoInnerCirD")==id)
		{
			sText.Printf("%.2f",sys.dim_map.PartNo.fMainPoleNoInnerCirD);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.PartNo.iMirMsg")==id)
		{
			if(sys.dim_map.PartNo.iMirMsg==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("0.X Axis Mirror");
			else if(sys.dim_map.PartNo.iMirMsg==1)
				sText.Copy("1.Y Axis Mirror");
			else 
				sText.Copy("2.No Mirror");
#else
				sText.Copy("0.X��Գ�");
			else if(sys.dim_map.PartNo.iMirMsg==1)
				sText.Copy("1.Y��Գ�");
			else 
				sText.Copy("2.�޶Գ�");
#endif
		}
		else if(GetPropID("dim_map.PartNo.fPartNoMargin")==id)
		{
			sText.Printf("%.2f",sys.fonts.TextSize("PartNoMargin"));
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.PartNo.bDrawRefLine")==id)
		{
			if(sys.dim_map.PartNo.bDrawRefLine)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("%s","Yes");
			else
				sText.Printf("%s","No");
#else
				sText.Printf("%s","��");
			else
				sText.Printf("%s","��");
#endif
		}
		else if(GetPropID("dim_map.fDimTextSize")==id)
		{
			sText.Printf("%.2f",sys.fonts.TextSize("DimText"));
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.fDimGap")==id)
		{
			sText.Printf("%.2f",sys.dim_map.fDimGap);	//�������ע�߼�϶ֵ
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.sArrowType")==id)
		{	//ʵ�ıպ�("")|���ıպ�("_CLOSEDBLANK")|�պ�("_CLOSED")|�������("_ARCHTICK")|
			//��б("_OBLIQUE")|30�Ƚ�("_OPEN30")|ֱ��("_OPEN90")";
			CString sArrowType=sys.dim_map.sArrowType;
			if(sArrowType.CompareNoCase("_CLOSEDBLANK")==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Hollow closed");
			else if(sArrowType.CompareNoCase("_CLOSED")==0)
				sText.Copy("Closed");
			else if(sArrowType.CompareNoCase("_ARCHTICK")==0)
				sText.Copy("Architectural symbol");
			else if(sArrowType.CompareNoCase("_OBLIQUE")==0)
				sText.Copy("Tilt");
			else if(sArrowType.CompareNoCase("_OPEN30")==0)
				sText.Copy("30 degree");
			else if(sArrowType.CompareNoCase("_OPEN90")==0)
				sText.Copy("Right angle");
			else //if(sArrowType.CompareNoCase("")==0) 
				sText.Copy("Solid closed");
#else
				sText.Copy("���ıպ�");
			else if(sArrowType.CompareNoCase("_CLOSED")==0)
				sText.Copy("�պ�");
			else if(sArrowType.CompareNoCase("_ARCHTICK")==0)
				sText.Copy("�������");
			else if(sArrowType.CompareNoCase("_OBLIQUE")==0)
				sText.Copy("��б");
			else if(sArrowType.CompareNoCase("_OPEN30")==0)
				sText.Copy("30�Ƚ�");
			else if(sArrowType.CompareNoCase("_OPEN90")==0)
				sText.Copy("ֱ��");
			else //if(sArrowType.CompareNoCase("")==0) 
				sText.Copy("ʵ�ıպ�");
#endif
		}
		else if(GetPropID("dim_map.fDimAngleTextSize")==id)
		{
			sText.Printf("%.2f",sys.fonts.TextSize("DimAngleText"));
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.fDimArrowSize")==id)
		{
			sText.Printf("%.2f",sys.dim_map.fDimArrowSize);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.fHeaderTextSize")==id)
		{
			sText.Printf("%.2f",sys.fonts.TextSize("HeaderText"));
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.fCommonTextSize")==id)
		{
			sText.Printf("%.2f",sys.fonts.TextSize("CommonText"));
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.fNumericTextSize")==id)
		{
			sText.Printf("%.2f",sys.fonts.TextSize("NumericText"));
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.fCutawayViewSymbolTextSize")==id)
		{
			sText.Printf("%.2f",sys.fonts.TextSize("CutawayViewSymbolText"));
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.fCutawayViewSymbolLen")==id)
		{
			sText.Printf("%.2f",sys.fonts.TextWtoH("CutawayViewSymbolText"));
			SimplifiedNumString(sText);
		}
		else if(GetPropID("dim_map.fCutawayViewPolyLineWidth")==id)
		{
			sText.Printf("%.2f",sys.dim_map.fCutawayViewPolyLineWidth);
			SimplifiedNumString(sText);
		}
		//������ɫ
		else if(GetPropID("dim_map.crIndex.crOutLine")==id)
			sText.Printf("%d",sys.dim_map.crIndex.crOutLine);
		else if(GetPropID("dim_map.crIndex.crDashLine")==id)
			sText.Printf("%d",sys.dim_map.crIndex.crDashLine);
		else if(GetPropID("dim_map.crIndex.crSingleLine")==id)
			sText.Printf("%d",sys.dim_map.crIndex.crSingleLine);
		else if(GetPropID("dim_map.crIndex.crLs")==id)
			sText.Printf("%d",sys.dim_map.crIndex.crLs);
		else if(GetPropID("dim_map.crIndex.crAxisLine")==id)
			sText.Printf("%d",sys.dim_map.crIndex.crAxisLine);
		else if(GetPropID("dim_map.crIndex.crDimSimTri")==id)
			sText.Printf("%d",sys.dim_map.crIndex.crDimSimTri);
		else if(GetPropID("dim_map.crIndex.crHuoQuLine")==id)
			sText.Printf("%d",sys.dim_map.crIndex.crHuoQuLine);
		else if(GetPropID("dim_map.crIndex.crDragLine")==id)
			sText.Printf("%d",sys.dim_map.crIndex.crDragLine);
		//ר��ͼ��
		else if(GetPropID("LayerTable.VisibleProfileLayer")==id)
			sText.Printf("%s",(char*)LayerTable::VisibleProfileLayer.layerName);
		//
		else if(GetPropID("LayerTable.UnvisibleProfileLayer")==id)
			sText.Printf("%s",(char*)LayerTable::UnvisibleProfileLayer.layerName);
		//
		else if(GetPropID("LayerTable.AuxLineLayer")==id)
			sText.Printf("%s",(char*)LayerTable::AuxLineLayer.layerName);
		//
		else if(GetPropID("LayerTable.AxisLineLayer")==id)
			sText.Printf("%s",(char*)LayerTable::AxisLineLayer.layerName);
		//
		else if(GetPropID("LayerTable.BendLineLayer")==id)
			sText.Printf("%s",(char*)LayerTable::BendLineLayer.layerName);
		//
		else if(GetPropID("LayerTable.BoltSymbolLayer")==id)
			sText.Printf("%s",(char*)LayerTable::BoltSymbolLayer.layerName);
		//
		else if(GetPropID("LayerTable.BreakLineLayer")==id)
			sText.Printf("%s",(char*)LayerTable::BreakLineLayer.layerName);
		//
		else if(GetPropID("LayerTable.CommonLayer")==id)
			sText.Printf("%s",(char*)LayerTable::CommonLayer.layerName);
		//
		else if(GetPropID("LayerTable.BriefLineLayer")==id)
			sText.Printf("%s",(char*)LayerTable::BriefLineLayer.layerName);
		//
		else if(GetPropID("LayerTable.SimPolyLayer")==id)
			sText.Printf("%s",(char*)LayerTable::SimPolyLayer.layerName);
		//
		else if(GetPropID("LayerTable.BoltLineLayer")==id)
			sText.Printf("%s",(char*)LayerTable::BoltLineLayer.layerName);
		//�ߴ��ע
		else if(GetPropID("LayerTable.DimSizeLayer")==id)
			sText.Printf("%s",(char*)LayerTable::DimSizeLayer.layerName);
		//���ֱ�ע
		else if(GetPropID("LayerTable.DimTextLayer")==id)
			sText.Printf("%s",(char*)LayerTable::DimTextLayer.layerName);
		//����������
		else if(GetPropID("LayerTable.AssistHuoQuLineLayer")==id)
			sText.Printf("%s",(char*)LayerTable::AssistHuoQuLineLayer.layerName);
		//�ӽǸ�������
		else if(GetPropID("LayerTable.SonJgAxisLineLayer")==id)
			sText.Printf("%s",(char*)LayerTable::SonJgAxisLineLayer.layerName);
		//�ְ����������ʶ��
		else if(GetPropID("LayerTable.DamagedSymbolLine")==id)
			sText.Printf("%s",(char*)LayerTable::DamagedSymbolLine.layerName);
		//
		else if(GetPropID("Layer.lineTypeName")==id)
		{
			if(pItem&&pItem->m_pParent)
			{
				TMADRAWING_LAYER *pLayer = (TMADRAWING_LAYER*)pItem->m_pParent->m_idProp;
				if(pLayer)
					sText.Printf("%s",(char*)pLayer->lineTypeName);
			}
		}
		else if(GetPropID("Layer.colorIndex")==id)
		{
			if(pItem&&pItem->m_pParent)
			{
				TMADRAWING_LAYER *pLayer = (TMADRAWING_LAYER*)pItem->m_pParent->m_idProp;
				if(pLayer)
					sText.Printf("RGB%X",GetColorFromIndex(pLayer->colorIndex));
			}
		}
		else if(GetPropID("Layer.lineWeight")==id)
		{
			if(pItem&&pItem->m_pParent)
			{
				TMADRAWING_LAYER *pLayer = (TMADRAWING_LAYER*)pItem->m_pParent->m_idProp;
				if(pLayer)
				{
					double fLineW=((int)pLayer->lineWeight)*0.01;
					sText.Printf("%.2fmm",fLineW);
				}
			}
		}
		else 
		{	//ͼ����idΪ��Ӧͼ���ָ��
			bool bPush = LayerTable::layerHashList.push_stack();
			TMADRAWING_LAYER *pLayer;
			for(pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if((long)pLayer==id)	
				{
					sText.Printf("%s",(char*)pLayer->layerName);
					bContinueFind=FALSE;
					break;
				}
			}
			if(bPush)
				LayerTable::layerHashList.pop_stack();
		}
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
BOOL CSystemSettingDlg::IsUseLayer(TMADRAWING_LAYER *pLayer)
{
	for(int i=0;i<m_xSpecialLayerIdArr.GetSize();i++)
	{
		CXhChar100 valuStr;
		if(GetPropValueStr(m_xSpecialLayerIdArr[i],valuStr)>0 && stricmp(valuStr,pLayer->layerName)==0)
			return TRUE;
	}
	return FALSE;
}
void CSystemSettingDlg::UpdateSpecialLayerItem()
{
	for(int i=0;i<m_xSpecialLayerIdArr.GetSize();i++)
	{
		CXhChar100 sText;
		if(GetPropValueStr(m_xSpecialLayerIdArr[i],sText)<=0)
			continue;
		m_propList.SetItemPropValue(m_xSpecialLayerIdArr[i],CString(sText));
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_xSpecialLayerIdArr[i],NULL);
		if(pItem)
			pItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	}
}
void CSystemSettingDlg::InsertOrUpdateLayerItem(TMADRAWING_LAYER *pLayer,CPropTreeItem *pParentItem)
{
	CXhChar100 valueStr;
	CItemInfo* lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
	lpInfo->m_buttonType = BDT_FILEPATH;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_sButtonName = "delete layer";
	lpInfo->m_strPropName = "layer name";
	lpInfo->m_strPropHelp = "layer name";
#else
	lpInfo->m_sButtonName = "ɾ��ͼ��";
	lpInfo->m_strPropName = "ͼ����";
	lpInfo->m_strPropHelp = "ͼ����";
#endif
	CPropTreeItem* pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,TRUE);
	pPropItem->m_idProp = (long)pLayer;	//
	if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "color";
	lpInfo->m_strPropHelp = "color";
#else
	lpInfo->m_strPropName = "��ɫ";
	lpInfo->m_strPropHelp = "��ɫ";
#endif
	CPropTreeItem* pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,TRUE);
	pSonPropItem->m_idProp = CSystemSettingDlg::GetPropID("Layer.colorIndex");
	if(GetPropValueStr(pSonPropItem->m_idProp,valueStr,valueStr.GetLengthMax(),pSonPropItem)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonPropItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbType = CDT_LIST;
	lpInfo->m_cmbItems = "CONTINUOUS|HIDDEN|DASHDOT2X|DIVIDE";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Line Type";
	lpInfo->m_strPropHelp = "Line type";
#else
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "����";
#endif
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,TRUE);
	pSonPropItem->m_idProp = CSystemSettingDlg::GetPropID("Layer.lineTypeName");
	if(GetPropValueStr(pSonPropItem->m_idProp,valueStr,valueStr.GetLengthMax(),pSonPropItem)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonPropItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbType = CDT_LIST;
	lpInfo->m_cmbItems = "0.00mm|0.05mm|0.09mm|0.13mm|0.15mm|0.18mm|0.20mm|0.25mm|0.30mm|0.35mm|0.40mm|0.50mm|0.53mm|0.60mm|0.70mm|0.80mm|0.90mm|1.00mm|1.06mm|1.20mm|1.40mm|1.58mm|2.00mm|2.11mm";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "lineWeight";
	lpInfo->m_strPropHelp = "lineWeight";
#else
	lpInfo->m_strPropName = "�߿�";
	lpInfo->m_strPropHelp = "�߿�";
#endif
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1,TRUE);
	pSonPropItem->m_idProp = CSystemSettingDlg::GetPropID("Layer.lineWeight");
	if(GetPropValueStr(pSonPropItem->m_idProp,valueStr,valueStr.GetLengthMax(),pSonPropItem)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonPropItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
}
CPropTreeItem* CSystemSettingDlg::InsertFontPropItem(char *propKeyStr,CPropTreeItem *pPropItem,int iMapType,int size0_wtoh1/*=0*/)
{
	double fDatumValue=0,fValue=0;
	if(size0_wtoh1==0)
	{	//�ָ�
		fValue=sys.fonts.TextSize(propKeyStr,iMapType);
		fDatumValue=sys.fonts.TextSize(propKeyStr);
	}
	else if(size0_wtoh1==1)
	{	//�����߱�
		fDatumValue=sys.fonts.TextWtoH(propKeyStr);
		fValue=sys.fonts.TextWtoH(propKeyStr,iMapType);
	}
	CXhChar50 sValue;
	if(fValue>0)
		sValue.Printf("%.2f",fValue);
	else
		sValue.Printf("%.2f",fDatumValue);
	SimplifiedNumString(sValue);
	KEY2WORD propId(iMapType,CTextItemProp::GetPropID(propKeyStr)+size0_wtoh1*1000);
	//����������
	CXhChar50 keyStr(propKeyStr);
	if(size0_wtoh1==1)
		keyStr.Append("1");
	long keyId=CSystemSettingDlg::GetPropID(keyStr);
	PROPLIST_ITEM *pItem=CSystemSettingDlg::GetPropItem(keyId);
	if(pItem==NULL)
		return NULL;
	CItemInfo* lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName.Format("%s",(char*)pItem->name);
	lpInfo->m_strPropHelp.Format("%s",(char*)pItem->propHelpStr);
	lpInfo->m_strPropValue=sValue;
	CPropTreeItem* pSonPropItem=m_propList.InsertItem(pPropItem,lpInfo,-1,TRUE);
	pSonPropItem->m_idProp=propId.idComplex;
	pSonPropItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
	return pSonPropItem;
}
void CSystemSettingDlg::DisplaySystemSetting()
{
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pSonPropItem=NULL,*pRootItem=m_propList.GetRootItem();
	const int GROUP_GENERAL=1,GROUP_LINE_MAP=2,GROUP_STRU_MAP=3,GROUP_PART_MAP=4,GROUP_PARTGROUP_MAP=5,GROUP_DIM_FONT=6,GROUP_LAYER=7;
	//���ûص�����
	m_propList.CleanTree();
	m_propList.SetModifyValueFunc(ModifySystemSettingValue);
	m_propList.SetButtonClickFunc(ButtonClickSystemSetting);
	m_propList.SetPropHelpPromptFunc(FireSystemSettingHelpStr);
	//����
	CPropertyListOper<CSystemSettingDlg> oper(&m_propList,this);
	pGroupItem=oper.InsertPropItem(pRootItem,"General");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL); 
	//���û�ͼ����
	oper.InsertEditPropItem(pGroupItem,"general_set.LineMapScale");		//���߱���
	oper.InsertEditPropItem(pGroupItem,"general_set.ForceLineScale");	//˾��ͼ����
	oper.InsertEditPropItem(pGroupItem,"general_set.StruMapScale");		//�ṹͼ����
	oper.InsertEditPropItem(pGroupItem,"general_set.PartMapScale");		//����ͼ����
	oper.InsertEditPropItem(pGroupItem,"general_set.WeldMapScale");		//�麸ͼ����
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"general_set.StruMapScale.iBoltSymbolStyle");	//�ṹͼ��˨ͼ��
	if(sys.general_set.StruMapScale.iBoltSymbolStyle>1&&sys.general_set.StruMapScale.iBoltSymbolStyle<4)
		oper.InsertEditPropItem(pPropItem,"general_set.StruMapScale.fBoltSymbolFactor");			//��������
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"general_set.PartMapScale.iBoltSymbolStyle");	//����ͼ��˨ͼ��
	if(sys.general_set.PartMapScale.iBoltSymbolStyle>1&&sys.general_set.PartMapScale.iBoltSymbolStyle<4)
		oper.InsertEditPropItem(pPropItem,"general_set.PartMapScale.fBoltSymbolFactor");			//��������
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"general_set.WeldMapScale.iBoltSymbolStyle");	//�躸ͼ��˨ͼ��
	if(sys.general_set.WeldMapScale.iBoltSymbolStyle>1&&sys.general_set.WeldMapScale.iBoltSymbolStyle<4)
		oper.InsertEditPropItem(pPropItem,"general_set.WeldMapScale.fBoltSymbolFactor");			//��������
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"general_set.iMatCharPosType");	//�����ַ�λ��
	if(sys.general_set.iMatCharPosType>0)	//�����ַ������
		oper.InsertCmbEditPropItem(pPropItem,"general_set.cMatCharSeparator");
	oper.InsertEditPropItem(pGroupItem,"general_set.fHoleAugmentInNC");		//�׾�����ֵ
	oper.InsertEditPropItem(pGroupItem,"general_set.fWeldLineSymbolSpace");	//�����߼�϶
	oper.InsertEditPropItem(pGroupItem,"general_set.fWeldLineSymbolHigh");	//�����߸߶�
	oper.InsertCmbListPropItem(pGroupItem,"general_set.iDimPrecision");	//�ߴ羫ȷ��
#ifdef __PART_DESIGN_INC_//defined(__TSA_FILE_)||defined(__LDS_FILE_)
	oper.InsertEditPropItem(pGroupItem,"nBriefGuigeN");		//�򻯹����
#endif
	oper.InsertCmbListPropItem(pGroupItem,"general_set.iPartBOMStyle");		//������ϸ����ʽ wht 10-11-11
	oper.InsertCmbListPropItem(pGroupItem,"general_set.iSumMatBOMStyle");	//���ϻ��ܱ���ʽ wht 11-05-06
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"bDrawMainPoleJointLine");	//���߷ֶα�־��
	if(sys.bDrawMainPoleJointLine)
	{	
		oper.InsertEditPropItem(pPropItem,"nJointLineLen");		//�ֶα�־�߳���
		oper.InsertEditPropItem(pPropItem,"nJointLineOffset");	//�ֶα�־��ƫ��ֵ
	}
	//���տ�·�������з�ʽ
	pGroupItem=oper.InsertPropItem(pRootItem,"ProcessCartAndLayout");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	oper.InsertCmbListPropItem(pGroupItem,"general_set.bCardLayoutBaseRow");	//���տ���׼���з�ʽ
	oper.InsertEditPropItem(pGroupItem,"general_set.nPartRowsN");	//ÿ���������
	oper.InsertEditPropItem(pGroupItem,"general_set.nRowHigh");		//����ͼ�и�
	oper.InsertEditPropItem(pGroupItem,"general_set.nColumnWidth");	//����ͼ�п�
	oper.InsertCmbListPropItem(pGroupItem,"general_set.bHasFrame");	//װ����
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"general_set.bProcessCardTbl");	//���տ�ͼ��
	pPropItem->m_bHideChildren=FALSE;
	if(sys.general_set.bProcessCardTbl)
	{
		oper.InsertFilePathPropItem(pPropItem,"general_set.m_sJgCard");		//�Ǹֹ��տ�
		oper.InsertFilePathPropItem(pPropItem,"general_set.m_sSlotCard");	//�۸ֹ��տ�
#ifdef __COMPLEX_PART_INC_//defined(__LMA_FILE_)||defined(__LDS_FILE_)
		oper.InsertFilePathPropItem(pPropItem,"general_set.m_sTubeCard");	//�ֹܹ��տ�
#endif
		oper.InsertFilePathPropItem(pPropItem,"general_set.m_sPlankCard");	//�ְ幤�տ�
		oper.InsertFilePathPropItem(pPropItem,"general_set.m_sWeldPartGroupCard");	//�麸�����տ�
		oper.InsertFilePathPropItem(pPropItem,"general_set.m_sProcessPartStatCard");//���ռ�ͳ�ƿ�
	}
	//��ͼģ��·��
	pGroupItem=oper.InsertPropItem(pRootItem,"MapArxPath");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	oper.InsertFilePathPropItem(pGroupItem,"general_set.m_sTMAMapArxPath");	//TMA��ͼģ��·��
	oper.InsertFilePathPropItem(pGroupItem,"general_set.m_sLMAMapArxPath");	//LMA��ͼģ��·��
	oper.InsertFilePathPropItem(pGroupItem,"general_set.m_sLDSMapArxPath");	//LDS��ͼģ��·��
	oper.InsertFilePathPropItem(pGroupItem,"general_set.m_sTSAMapArxPath");	//TSA��ͼģ��·��
	if( GetLicVersion()>=1000003&&(VerifyValidFunction(LICFUNC::FUNC_IDENTITY_DWGSOLID_EXPORT)||ValidateLocalizeFeature(FEATURE::EXPORT_3DSOLID_DWGMODEL)!=0))
	{	//���ֻ��ƽ�,���úᵣ��ɫ
		pGroupItem=oper.InsertPropItem(pRootItem,"digitalHandver","�ᵣ��ɫ����");
		pGroupItem->m_bHideChildren=FALSE;
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		oper.InsertCmbColorPropItem(pGroupItem,"general_set.lCrossarmCr");	//��ᵣ��ɫ
		oper.InsertCmbColorPropItem(pGroupItem,"general_set.rCrossarmCr");	//�Һᵣ��ɫ
	}
	//��ͼ
	pGroupItem=oper.InsertPropItem(pRootItem,"LineMap");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_LINE_MAP);
	pPropItem=oper.InsertEditPropItem(pGroupItem,"LineMap.Font");
	pPropItem->SetReadOnly();
	InsertFontPropItem("PartNo",pPropItem,CTextItemProp::SUMMARY_DRAWING);
	InsertFontPropItem("PartNoMargin",pPropItem,CTextItemProp::SUMMARY_DRAWING);
	InsertFontPropItem("DimText",pPropItem,CTextItemProp::SUMMARY_DRAWING);
	InsertFontPropItem("DimAngleText",pPropItem,CTextItemProp::SUMMARY_DRAWING);
	InsertFontPropItem("HeaderText",pPropItem,CTextItemProp::SUMMARY_DRAWING);
	InsertFontPropItem("CommonText",pPropItem,CTextItemProp::SUMMARY_DRAWING);
	InsertFontPropItem("NumericText",pPropItem,CTextItemProp::SUMMARY_DRAWING);
	InsertFontPropItem("CutawayViewSymbolText",pPropItem,CTextItemProp::SUMMARY_DRAWING);
	InsertFontPropItem("CutawayViewSymbolText",pPropItem,CTextItemProp::SUMMARY_DRAWING,1);
	InsertFontPropItem("SimPolyText",pPropItem,CTextItemProp::SUMMARY_DRAWING);
	InsertFontPropItem("SimPolyText",pPropItem,CTextItemProp::SUMMARY_DRAWING,1);
	InsertFontPropItem("LsGuigeText",pPropItem,CTextItemProp::SUMMARY_DRAWING);
	InsertFontPropItem("JgOddmentText",pPropItem,CTextItemProp::SUMMARY_DRAWING);
	InsertFontPropItem("PlateThickText",pPropItem,CTextItemProp::SUMMARY_DRAWING);
	InsertFontPropItem("PartGuigeText",pPropItem,CTextItemProp::SUMMARY_DRAWING);
	InsertFontPropItem("OtherText",pPropItem,CTextItemProp::SUMMARY_DRAWING);
	//˾��ͼ
	pGroupItem=oper.InsertPropItem(pRootItem,"ForceLineMap");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_LINE_MAP);
	pPropItem=oper.InsertEditPropItem(pGroupItem,"ForceLineMap.Font");
	pPropItem->SetReadOnly();
	InsertFontPropItem("PartNo",pPropItem,CTextItemProp::CHIEF_DRAWING);
	InsertFontPropItem("PartNoMargin",pPropItem,CTextItemProp::CHIEF_DRAWING);
	InsertFontPropItem("DimText",pPropItem,CTextItemProp::CHIEF_DRAWING);
	InsertFontPropItem("DimAngleText",pPropItem,CTextItemProp::CHIEF_DRAWING);
	InsertFontPropItem("HeaderText",pPropItem,CTextItemProp::CHIEF_DRAWING);
	InsertFontPropItem("CommonText",pPropItem,CTextItemProp::CHIEF_DRAWING);
	InsertFontPropItem("NumericText",pPropItem,CTextItemProp::CHIEF_DRAWING);
	InsertFontPropItem("CutawayViewSymbolText",pPropItem,CTextItemProp::CHIEF_DRAWING);
	InsertFontPropItem("CutawayViewSymbolText",pPropItem,CTextItemProp::CHIEF_DRAWING,1);
	InsertFontPropItem("SimPolyText",pPropItem,CTextItemProp::CHIEF_DRAWING);
	InsertFontPropItem("SimPolyText",pPropItem,CTextItemProp::CHIEF_DRAWING,1);
	InsertFontPropItem("LsGuigeText",pPropItem,CTextItemProp::CHIEF_DRAWING);
	InsertFontPropItem("JgOddmentText",pPropItem,CTextItemProp::CHIEF_DRAWING);
	InsertFontPropItem("PlateThickText",pPropItem,CTextItemProp::CHIEF_DRAWING);
	InsertFontPropItem("PartGuigeText",pPropItem,CTextItemProp::CHIEF_DRAWING);
	InsertFontPropItem("OtherText",pPropItem,CTextItemProp::CHIEF_DRAWING);
	//�ṹͼ
	pGroupItem=oper.InsertPropItem(pRootItem,"StruMap");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_STRU_MAP);
	oper.InsertCmbListPropItem(pGroupItem,"stru_map.bExportByEnglish");		//�������
	oper.InsertEditPropItem(pGroupItem,"stru_map.fMinWingWide");			//�Ǹ�֫����С����ֵ
	oper.InsertEditPropItem(pGroupItem,"stru_map.fMinPartThick");			//���������С����ֵ
	oper.InsertCmbListPropItem(pGroupItem,"stru_map.bDimSpecialLsSpace");	//��ע������˨���
	oper.InsertCmbListPropItem(pGroupItem,"stru_map.bDimSpecialJgZhun");	//��ע�Ǹ������ľ�
	oper.InsertCmbListPropItem(pGroupItem,"stru_map.bDimStruJgLen");		//��ע�Ǹֳ���
	// ��ע������� �ݲ����ø����ã�Ĭ�ϱ�ע������� wht 11-03-29
	//oper.InsertCmbListPropItem(pGroupItem,"stru_map.bDimPartGuiGe");		//��ע�������
	oper.InsertCmbListPropItem(pGroupItem,"stru_map.bDrawCutAngleAndWingInfo");	//���ƽǸ���֫�н���Ϣ
	oper.InsertCmbListPropItem(pGroupItem,"stru_map.bOnlyDrawHorizontalPlate");	//������ˮƽ��ʾ�ĸְ�
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"stru_map.bUsePolyLineDrawEdge");	//����߻���������
	if(sys.stru_map.bUsePolyLineDrawEdge)
		oper.InsertEditPropItem(pPropItem,"stru_map.fPolyLineWidth");	//����߿������
	pPropItem=oper.InsertEditPropItem(pGroupItem,"stru_map.PoleAxisLineOutSpreadLen");	//�˼��������쳤��
	oper.InsertEditPropItem(pPropItem,"stru_map.fPoleAxisLineStartOutSpreadLen");	//�˼�����ʼ�����쳤��
	oper.InsertEditPropItem(pPropItem,"stru_map.fPoleAxisLineEndOutSpreadLen");	//�˼������ն����쳤��
#ifdef __SMART_DRAW_INC_
	oper.InsertCmbListPropItem(pGroupItem,"stru_map.bReadAdditionalBuffer");	//��ȡ���ӻ���
#endif
	//ͼ��
	pGroupItem=oper.InsertCmbListPropItem(pRootItem,"m_pCurMapFrm.style");
	pGroupItem->m_lpNodeInfo->m_cmbItems=MakeMapFrameSizeSetString()+"�½�...";
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_STRU_MAP);
	if(m_pCurMapFrm!=NULL)
	{	
		pPropItem=oper.InsertEditPropItem(pGroupItem,"m_pCurMapFrm.nLength");	//ͼ�򳤶�
		pPropItem->SetReadOnly(m_pCurMapFrm->style<=9);	//Ĭ��ͼ��ߴ粻���޸� wht 16-01-28
		pPropItem=oper.InsertEditPropItem(pGroupItem,"m_pCurMapFrm.nWidth");	//ͼ����
		pPropItem->SetReadOnly(m_pCurMapFrm->style<=9);
		pPropItem=oper.InsertEditPropItem(pGroupItem,"m_pCurMapFrm.nLeftMargin");	//����϶
		pPropItem->SetReadOnly(m_pCurMapFrm->style<=9);
		pPropItem=oper.InsertEditPropItem(pGroupItem,"m_pCurMapFrm.nOtherMargin");	//�������϶
		pPropItem->SetReadOnly(m_pCurMapFrm->style<=9);
		pPropItem=oper.InsertEditPropItem(pGroupItem,"m_pCurMapFrm.nBottomRightFixLen");	//���²�̶�����
		pPropItem->SetReadOnly(m_pCurMapFrm->style<=9);
		pPropItem=oper.InsertEditPropItem(pGroupItem,"m_pCurMapFrm.nBottomRightFixWidth");	//���²�̶��߶�
		pPropItem->SetReadOnly(m_pCurMapFrm->style<=9);
	}
	//�ṹͼ��ע����
	pGroupItem=oper.InsertEditPropItem(pRootItem,"stru_map.Font");
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_STRU_MAP);
	pGroupItem->SetReadOnly();
	InsertFontPropItem("PartNo",pGroupItem,CTextItemProp::SEGMENT_DRAWING);
	InsertFontPropItem("PartNoMargin",pGroupItem,CTextItemProp::SEGMENT_DRAWING);
	InsertFontPropItem("DimText",pGroupItem,CTextItemProp::SEGMENT_DRAWING);
	InsertFontPropItem("DimAngleText",pGroupItem,CTextItemProp::SEGMENT_DRAWING);
	InsertFontPropItem("HeaderText",pGroupItem,CTextItemProp::SEGMENT_DRAWING);
	InsertFontPropItem("CommonText",pGroupItem,CTextItemProp::SEGMENT_DRAWING);
	InsertFontPropItem("NumericText",pGroupItem,CTextItemProp::SEGMENT_DRAWING);
	InsertFontPropItem("CutawayViewSymbolText",pGroupItem,CTextItemProp::SEGMENT_DRAWING);
	InsertFontPropItem("CutawayViewSymbolText",pGroupItem,CTextItemProp::SEGMENT_DRAWING,1);
	InsertFontPropItem("SimPolyText",pGroupItem,CTextItemProp::SEGMENT_DRAWING);
	oper.InsertEditPropItem(pGroupItem,"dim_map.fSimTriScale");	//�˼�����ʼ�����쳤��
	//InsertFontPropItem("SimPolyText",pGroupItem,CTextItemProp::SEGMENT_DRAWING,1);
	InsertFontPropItem("LsGuigeText",pGroupItem,CTextItemProp::SEGMENT_DRAWING);
	InsertFontPropItem("JgOddmentText",pGroupItem,CTextItemProp::SEGMENT_DRAWING);
	InsertFontPropItem("PlateThickText",pGroupItem,CTextItemProp::SEGMENT_DRAWING);
	InsertFontPropItem("PartGuigeText",pGroupItem,CTextItemProp::SEGMENT_DRAWING);
	InsertFontPropItem("OtherText",pGroupItem,CTextItemProp::SEGMENT_DRAWING);
	//�麸��
	pGroupItem=oper.InsertPropItem(pRootItem,"PartGroupMap");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_PARTGROUP_MAP);
	oper.InsertEditPropItem(pGroupItem,"part_group_map.fMinFontSize");		//������С�߶�
	oper.InsertEditPropItem(pGroupItem,"part_group_map.fMinDrawThick");		//�����С����ֵ
	oper.InsertEditPropItem(pGroupItem,"part_group_map.nKmJgGIncrement");	//��ģ�Ǹ��ľ�����ֵ
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_group_map.iPlateLocateDrawStyle");	//���Ƹְ�Ի��
	if(sys.part_group_map.iPlateLocateDrawStyle>0)
		oper.InsertEditPropItem(pPropItem,"part_group_map.iLocateSymboleHeight");	//���Ƹְ�Ի��߶�
	oper.InsertCmbListPropItem(pGroupItem,"part_group_map.bDrawWeldLineSymbol");	//���ƺ����߱��
	oper.InsertCmbListPropItem(pGroupItem,"part_group_map.bAutoDimPartNo");		//�Զ���ע�������
	oper.InsertCmbListPropItem(pGroupItem,"part_group_map.bAutoMatchView");		//�Զ����ֺ�����ͼ
	oper.InsertCmbListPropItem(pGroupItem,"part_group_map.bAngularDimension");	//��ע�Ƕ���
	oper.InsertCmbListPropItem(pGroupItem,"part_group_map.bDimBeforeWeldLocatSpace");	//��ǰװ�䶨λ�ߴ�
	oper.InsertCmbListPropItem(pGroupItem,"part_group_map.bDimAfterWeldCheckLine");		//������ߴ�
	oper.InsertCmbListPropItem(pGroupItem,"part_group_map.bDimAbsoluteDist");			//��λ����Գߴ�
	oper.InsertCmbListPropItem(pGroupItem,"part_group_map.bDimPlatePoleRayAndAngle");	//���߲������߼��н�
	oper.InsertCmbListPropItem(pGroupItem,"part_group_map.bSmartFilterRedundantPart");	//�Զ�ɸѡ��ͼ��ʾ����
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_group_map.bModulatePartGroupLen");	//�Զ������麸ͼ����
	pPropItem->m_bHideChildren=FALSE;
	if(sys.part_group_map.bModulatePartGroupLen)
	{
		oper.InsertEditPropItem(pPropItem,"part_group_map.fPartGroupDistThreshold");	//������ֵ
		oper.InsertEditPropItem(pPropItem,"part_group_map.fPartGroupDistZoomCoef");		//����ϵ��
	}
	//�麸����ϸ��
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_group_map.bCreatePartGroupBOM");	//�����麸����ϸ��
	pPropItem->m_bHideChildren=FALSE;
	if(sys.part_group_map.bCreatePartGroupBOM)
	{	
		oper.InsertCmbListPropItem(pPropItem,"part_group_map.part_table.bZoomInProcessCard");	//���տ��ڽ��б�������
		oper.InsertCmbListPropItem(pPropItem,"part_group_map.part_table.bDispPartGroupNo");		//�麸�����
		oper.InsertCmbListPropItem(pPropItem,"part_group_map.part_table.bDispPartGroupNum");	//�麸������
		oper.InsertCmbListPropItem(pPropItem,"part_group_map.part_table.bDispPartNo");			//�������
		oper.InsertCmbListPropItem(pPropItem,"part_group_map.part_table.bDispPartNum");			//��������
		oper.InsertCmbListPropItem(pPropItem,"part_group_map.part_table.bDispPartGuiGe");		//�������
		oper.InsertCmbListPropItem(pPropItem,"part_group_map.part_table.bDispPartLen");			//��������
		oper.InsertCmbListPropItem(pPropItem,"part_group_map.part_table.bDispPartWeight");		//�麸������
		oper.InsertCmbListPropItem(pPropItem,"part_group_map.part_table.bDispNotes");			//��ע
		oper.InsertCmbListPropItem(pPropItem,"part_group_map.part_table.bDispInternal");		//���켶������
		oper.InsertCmbListPropItem(pPropItem,"part_group_map.part_table.bDispAppearance");		//���켶�����
	}
	//�麸ͼ��ע����
	pPropItem=oper.InsertEditPropItem(pGroupItem,"part_group_map.Font");
	pPropItem->m_bHideChildren=FALSE;
	pPropItem->SetReadOnly();
	pSonPropItem=oper.InsertCmbListPropItem(pPropItem,"welding.textsize.bZoomWithScale");
	if(sys.welding.bZoomAdaptiveWithScale)
		oper.InsertEditPropItem(pSonPropItem,"welding.textsize.wiScale");
	InsertFontPropItem("PartNo",pPropItem,CTextItemProp::WELD_DRAWING);
	InsertFontPropItem("PartNoMargin",pPropItem,CTextItemProp::WELD_DRAWING);
	InsertFontPropItem("DimText",pPropItem,CTextItemProp::WELD_DRAWING);
	InsertFontPropItem("DimAngleText",pPropItem,CTextItemProp::WELD_DRAWING);
	InsertFontPropItem("HeaderText",pPropItem,CTextItemProp::WELD_DRAWING);
	InsertFontPropItem("CommonText",pPropItem,CTextItemProp::WELD_DRAWING);
	InsertFontPropItem("NumericText",pPropItem,CTextItemProp::WELD_DRAWING);
	InsertFontPropItem("CutawayViewSymbolText",pPropItem,CTextItemProp::WELD_DRAWING);
	InsertFontPropItem("CutawayViewSymbolText",pPropItem,CTextItemProp::WELD_DRAWING,1);
	InsertFontPropItem("SimPolyText",pPropItem,CTextItemProp::WELD_DRAWING);
	InsertFontPropItem("SimPolyText",pPropItem,CTextItemProp::WELD_DRAWING,1);
	InsertFontPropItem("LsGuigeText",pPropItem,CTextItemProp::WELD_DRAWING);
	InsertFontPropItem("JgOddmentText",pPropItem,CTextItemProp::WELD_DRAWING);
	InsertFontPropItem("PlateThickText",pPropItem,CTextItemProp::WELD_DRAWING);
	InsertFontPropItem("PartGuigeText",pPropItem,CTextItemProp::WELD_DRAWING);
	InsertFontPropItem("OtherText",pPropItem,CTextItemProp::WELD_DRAWING);
	//����ͼ
	pGroupItem=oper.InsertPropItem(pRootItem,"PartMap");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_PART_MAP);
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_map.iExportDeformedProfile");	//�����������ЧӦ
	if(sys.part_map.iExportDeformedProfile!=0)
		oper.InsertCmbListPropItem(pPropItem,"part_map.bDimDeformedProfileText");		//�Ƿ��ע�������
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_map.bDimPlaneAngle");		//��ע������������н�
	if(sys.part_map.bDimPlaneAngle)
		oper.InsertCmbListPropItem(pPropItem,"part_map.iDimAngleType");	//�����Ƕȱ�ע����
	oper.InsertEditPropItem(pGroupItem,"part_map.fMinHuoquAngle");		//��С�����Ƕ���ֵ
	oper.InsertCmbListPropItem(pGroupItem,"part_map.bDimHuoquType");	//��ע������������
	oper.InsertCmbListPropItem(pGroupItem,"part_map.bUseZhengFanQun");	//��������ע��������
	oper.InsertCmbListPropItem(pGroupItem,"part_map.iPartMapStyle");	//���ձ�Ƿ���
	//�Ǹֹ���ͼ
	pGroupItem=oper.InsertPropItem(pRootItem,"AnglePartMap");
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_PART_MAP);
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_map.angle.iJgZoomSchema");			//�Ǹ����ŷ��� wht 11-05-07
	if(sys.part_map.angle.iJgZoomSchema==3)
		oper.InsertCmbListPropItem(pPropItem,"part_map.angle.ciJgLenDrawMode");
	oper.InsertCmbListPropItem(pGroupItem,"part_map.angle.bMaxExtendAngleLength");	//����޶�������Ƴ���
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_map.angle.iJgGDimStyle");	//�Ǹ��ľ��עλ��
	if(sys.part_map.angle.iJgGDimStyle==2)
		oper.InsertEditPropItem(pPropItem,"part_map.angle.nMaxBoltNumStartDimG");	//�Զ��ж�-�����˨��
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_map.angle.iLsSpaceDimStyle");	//��˨����ע��ʽ
	if(sys.part_map.angle.iLsSpaceDimStyle==2)
		oper.InsertEditPropItem(pPropItem,"part_map.angle.nMaxBoltNumAlongX");	//��X�᷽���ע֧�ֵ������˨����
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_map.angle.bDimLsAbsoluteDist");	//��ע��˨���Գߴ�
	if(sys.part_map.angle.bDimLsAbsoluteDist)
		oper.InsertCmbListPropItem(pPropItem,"part_map.angle.bMergeLsAbsoluteDist");	//�ϲ����ڵȾ���˨���Գߴ�
	oper.InsertCmbListPropItem(pGroupItem,"part_map.angle.bDimPushFlatMap");	//��עѹ��
	oper.InsertCmbListPropItem(pGroupItem,"part_map.angle.bDimCutAngle");		//��ע�нǳߴ�
	oper.InsertCmbListPropItem(pGroupItem,"part_map.angle.bDimCutAngleMap");	//��ʾ�н�ʾ��ͼ
	oper.InsertCmbListPropItem(pGroupItem,"part_map.angle.bOneCardMultiPart");	//һ�����
	oper.InsertCmbListPropItem(pGroupItem,"part_map.angle.bJgUseSimpleLsMap");	//����˨ͼ��
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_map.angle.bDimRibPlatePartNo");	//��ע�Ǹ��߰���
	if(sys.part_map.angle.bDimRibPlatePartNo)
		oper.InsertCmbListPropItem(pPropItem,"part_map.angle.bDimRibPlateMaterial");	//��ע�Ǹ��߰����
	oper.InsertCmbListPropItem(pGroupItem,"part_map.angle.bDimRibPlateSetUpPos");	//��ע�Ǹ��߰�װ��λ��
	oper.InsertCmbListPropItem(pGroupItem,"part_map.angle.iCutAngleDimType");	//�Ǹ��нǱ�ע��ʽ wht 10-11-02
	oper.InsertEditPropItem(pGroupItem,"part_map.angle.fKaiHeJiaoThreshold");	//���ϽǱ�ע��ֵ wht 11-05-06
	oper.InsertEditPropItem(pGroupItem,"part_map.angle.fKaiHeJiaoMinDeita");	//���Ͻ���С��ȫ��϶ wjh 15-02-06
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_map.angle.bDimKaiHe");	//��ע���Ͻ�
	if(sys.part_map.angle.bDimKaiHe)
	{	
		oper.InsertCmbListPropItem(pPropItem,"part_map.angle.bDimKaiheAngle");	//��ע���ϽǶ���
		oper.InsertCmbListPropItem(pPropItem,"part_map.angle.bDimKaiheSumLen");	//���Ͻ��ܳ�
		oper.InsertCmbListPropItem(pPropItem,"part_map.angle.bDimKaiheSegLen");	//��ע���������ܳ�
		oper.InsertCmbListPropItem(pPropItem,"part_map.angle.bDimKaiheScopeMap");//��ע���������ʶ��
	}
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"AnglePartMap.Font");
	//InsertFontPropItem("HeaderText",pPropItem,CTextItemProp::ANGLE_DRAWING);
	CPropTreeItem* pItem=InsertFontPropItem("CommonText",pPropItem,CTextItemProp::ANGLE_DRAWING);
	pItem->m_lpNodeInfo->m_strPropName="�������ָ�";
	pItem=InsertFontPropItem("NumericText",pPropItem,CTextItemProp::ANGLE_DRAWING);
	pItem->m_lpNodeInfo->m_strPropName="��������ָ�";
	InsertFontPropItem("PartNo",pPropItem,CTextItemProp::ANGLE_DRAWING);
	InsertFontPropItem("DimText",pPropItem,CTextItemProp::ANGLE_DRAWING);
	InsertFontPropItem("DimAngleText",pPropItem,CTextItemProp::ANGLE_DRAWING);
	InsertFontPropItem("LsGuigeText",pPropItem,CTextItemProp::ANGLE_DRAWING);
	InsertFontPropItem("PartGuigeText",pPropItem,CTextItemProp::ANGLE_DRAWING);
	InsertFontPropItem("OtherText",pPropItem,CTextItemProp::ANGLE_DRAWING);
	//�ְ幹��ͼ
	pGroupItem=oper.InsertPropItem(pRootItem,"PlateMap");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_PART_MAP);
	oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.bDimPlateEdgeLen");	//��ע�ְ�߳�
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.bDrawInnerEdge");	//������������
	if(sys.part_map.plate.bDrawInnerEdge)
		oper.InsertEditPropItem(pPropItem,"part_map.plate.fInnerEdgeOffset");	//������ƫ�ƾ�
	oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.bDimCoord");	//��ע����ֵ
	oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.bAdjustPlateUcs");	//�������ֱ�ע��ˮƽ����
	oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.bDimArcEdgeR");
	oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.bDimArcAngle");
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.bDimPlateRayLs");	//��������˨���
	if(sys.part_map.plate.bDimPlateRayLs)
	{
		oper.InsertCmbListPropItem(pPropItem,"part_map.plate.bDimPlateBoltAngle");	//����˨�Ƕ�
		oper.InsertCmbListPropItem(pPropItem,"part_map.plate.bDimBoltEdgeSpace");	//����˨�߾�
		oper.InsertCmbListPropItem(pPropItem,"part_map.plate.bDimDiffRayLsSpace");	//����˨���
		oper.InsertCmbListPropItem(pPropItem,"part_map.plate.bDimLsToHuoquLine");	//����˨��������
		oper.InsertCmbListPropItem(pPropItem,"part_map.plate.bDimLsToWeldEdge");	//����˨�����ӱ�
	}
	oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.bPlateUseSimpleLsMap");	//����˨ͼ��
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.bZoomDimTextSizeByWidth");//�Զ������ע�ָ�
	if(sys.part_map.plate.bZoomDimTextSizeByWidth)
		oper.InsertEditPropItem(pPropItem,"part_map.plate.fTextSizeZoomCoef");	//�ָ�����ϵ��
	oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.bDrawSlopeCutLine");	//���ƺ��촦���¿���
#ifdef __COMPLEX_PART_INC_
	oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.bDrawDatumLine");		//���Ƹְ��׼��
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.iZhiWanLineType");	//���������
	if(sys.part_map.plate.iZhiWanLineType==1)
		oper.InsertEditPropItem(pPropItem,"part_map.plate.fInnerOffThreshold");
#endif
	oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.iCutArcsType");		//�������ǵ��л���ʽ
	oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.bMarkHuoquPos");		//��ǻ���λ��
	oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.bDrawHuoQuPrjMap");	//���ƻ���ʾ��ͼ
	oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.bDrawHuoQuMarkLine");	//���ƻ���������
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"part_map.plate.bDisplaypPartInfoTbl");	//��ʾ������Ϣ��
	if(sys.part_map.plate.bDisplaypPartInfoTbl)
		oper.InsertEditPropItem(pPropItem,"part_map.plate.nPartInfoTblColumnWidth");//������Ϣ���п�
	oper.InsertButtonPropItem(pGroupItem,"part_map.plate.bUseDimStyleConfigFile");	//������ϸ����
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"PlateMap.Font");
	InsertFontPropItem("PartNo",pPropItem,CTextItemProp::PLATE_DRAWING);
	InsertFontPropItem("DimText",pPropItem,CTextItemProp::PLATE_DRAWING);
	InsertFontPropItem("DimAngleText",pPropItem,CTextItemProp::PLATE_DRAWING);
	/*���¸ְ�����δ���ã��ݲ���ʾ
	InsertFontPropItem("LsGuigeText",pPropItem,CTextItemProp::PLATE_DRAWING);
	InsertFontPropItem("PlateThickText",pPropItem,CTextItemProp::PLATE_DRAWING);
	InsertFontPropItem("PartGuigeText",pPropItem,CTextItemProp::PLATE_DRAWING);
	InsertFontPropItem("OtherText",pPropItem,CTextItemProp::PLATE_DRAWING);
	*/
#ifdef __COMPLEX_PART_INC_//defined(__LMA_FILE_)||defined(__LDS_FILE_)
	//�ֹܹ���ͼ
	pGroupItem=oper.InsertPropItem(pRootItem,"part_map.tube");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_PART_MAP);
	oper.InsertEditPropItem(pGroupItem,"part_map.tube.fSlotWidthProcessError");	//���ۿ�����ֵ
	oper.InsertEditPropItem(pGroupItem,"part_map.tube.fSlotLenProcessError");	//���۳������ֵ
	oper.InsertEditPropItem(pGroupItem,"part_map.tube.fReelTubeThresholdD");	//���۳������ֵ
#endif
	//ͼֽ��ע
	//�������
	pGroupItem=oper.InsertPropItem(pRootItem,"PartNoDim");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DIM_FONT); 
	oper.InsertCmbListPropItem(pGroupItem,"dim_map.PartNo.iPartNoFrameStyle");	//��ͨ��ſ�����
	oper.InsertCmbListPropItem(pGroupItem,"dim_map.PartNo.iStdInsertPlatePartNoFrameStyle");	//��׼����ſ�����
	oper.InsertCmbListPropItem(pGroupItem,"dim_map.PartNo.iFlPartNoFrameStyle");//������ſ�����		
	oper.InsertEditPropItem(pGroupItem,"dim_map.PartNo.fPartNoCirD");	//��ſ򳤶�
	oper.InsertEditPropItem(pGroupItem,"dim_map.fSegITextSize");	//�κ��ָ�
	oper.InsertEditPropItem(pGroupItem,"dim_map.fSegICirD");	//�κſ򳤶�
	pPropItem=oper.InsertEditPropItem(pGroupItem,"dim_map.PartNo.fMainPoleNo");	//���ı��
	pPropItem->m_bHideChildren=FALSE;
	oper.InsertEditPropItem(pPropItem,"dim_map.PartNo.fMainPoleNoOuterCirD");	//��Բֱ��
	oper.InsertEditPropItem(pPropItem,"dim_map.PartNo.fMainPoleNoInnerCirD");	//��Բֱ��
	oper.InsertCmbListPropItem(pGroupItem,"dim_map.PartNo.bDrawRefLine");		//����������
	oper.InsertCmbListPropItem(pGroupItem,"dim_map.PartNo.iMirMsg");	//�ԳƷ�ʽ	
	//�ߴ��ע
	pGroupItem=oper.InsertPropItem(pRootItem,"setMapSize");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DIM_FONT);
	oper.InsertCmbListPropItem(pGroupItem,"dim_map.sArrowType");	//��ͷ����
	oper.InsertEditPropItem(pGroupItem,"dim_map.fDimGap");	//��ע��϶
	oper.InsertEditPropItem(pGroupItem,"dim_map.fDimArrowSize");	//��ͷ��
	//���ϱ��ע
	pGroupItem=oper.InsertPropItem(pRootItem,"setMaterMap");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DIM_FONT);
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"dim_map.sFontName");	//��������
	pPropItem->m_lpNodeInfo->m_cmbItems.Format("%s",(char*)MakeFontNameStr());
	//����ͼ����ע
	pGroupItem=oper.InsertPropItem(pRootItem,"CutawayViewSymbol");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DIM_FONT);
	oper.InsertEditPropItem(pGroupItem,"dim_map.fCutawayViewPolyLineWidth");	//����߿��
	oper.InsertEditPropItem(pGroupItem,"dim_map.fCutawayViewSymbolLen");	//����
	//ͼ��
	pGroupItem=oper.InsertGrayButtonPropItem(pRootItem,"LayerTable.layerHashList");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_LAYER);
	for(TMADRAWING_LAYER *pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
		InsertOrUpdateLayerItem(pLayer,pGroupItem);
	//ר��ͼ��
	pGroupItem=oper.InsertPropItem(pRootItem,"LayerTable");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_LAYER);
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.VisibleProfileLayer");	//�ɼ�����
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.UnvisibleProfileLayer");	//������
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.BriefLineLayer");	//�����ĵ���
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.AuxLineLayer");	//���ĵ���
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.BoltSymbolLayer");	//��˨
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.SimPolyLayer");	//������
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.AxisLineLayer");	//������
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.SonJgAxisLineLayer");	//�ӽǸ�����
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.BendLineLayer");	//������
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.AssistHuoQuLineLayer");	//����������
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.BreakLineLayer");	//�Ͽ�����
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.BoltLineLayer");	//��˨��
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.DimSizeLayer");	//�ߴ��ע
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.DimTextLayer");	//���ֱ�ע
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.DamagedSymbolLine");//�����������
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"LayerTable.CommonLayer");	//����
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeLayerSetString();
	//
	m_propList.Redraw();
}
//////////////////////////////////////////////////////////////////////////
//�ص���������ʱ���ܺ�����ʵ��
BOOL GetBitMap(const char* bmp_file,CBitmap& xBitMap)
{
	xBitMap.DeleteObject();
	FILE *fp = fopen(bmp_file,"rt");
	if(fp==NULL)
		return FALSE;
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	fread(&fileHeader,1,sizeof(BITMAPFILEHEADER),fp);
	fread(&infoHeader,1,sizeof(BITMAPINFOHEADER),fp);
	int i,nPalette = infoHeader.biClrUsed;
	if(nPalette==0&&infoHeader.biBitCount<=8)
		nPalette=GetSingleWord(infoHeader.biBitCount+1);
	DYN_ARRAY<RGBQUAD> palette(nPalette);
	for(i=0;i<nPalette;i++)
	{
		fread(&palette[i].rgbBlue,1,1,fp);
		fread(&palette[i].rgbGreen,1,1,fp);
		fread(&palette[i].rgbRed,1,1,fp);
		fread(&palette[i].rgbReserved,1,1,fp);
	}
	BITMAP image;
	image.bmType=0;
	image.bmPlanes=1;
	image.bmWidth=infoHeader.biWidth;
	image.bmBitsPixel=infoHeader.biBitCount;
	image.bmHeight=infoHeader.biHeight;
	int widthBits = infoHeader.biWidth*infoHeader.biBitCount;
	image.bmWidthBytes=((widthBits+31)/32)*4;
	BYTE_ARRAY fileBytes(infoHeader.biHeight*image.bmWidthBytes);
	image.bmBits = fileBytes;
	fread(image.bmBits,1,infoHeader.biHeight*image.bmWidthBytes,fp);
	fclose(fp);
	//
	BYTE byteConstArr[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	int bm32WidthBytes=infoHeader.biWidth*4;
	BYTE_ARRAY bit32bytes(infoHeader.biHeight*bm32WidthBytes);
	for(int rowi=0;rowi<infoHeader.biHeight;rowi++)
	{
		int row=infoHeader.biHeight-rowi-1;
		for(int i=0;i<infoHeader.biWidth;i++)
		{
			int colorindex=0;
			RGBQUAD crPixelColor;
			crPixelColor.rgbReserved=0;
			if(infoHeader.biBitCount==1)
			{	//��ɫͼ
				int ibyte=row*image.bmWidthBytes+i/8;
				if(ibyte&byteConstArr[i%8])
					crPixelColor.rgbBlue=crPixelColor.rgbGreen=crPixelColor.rgbRed=255;
			}
			else if(infoHeader.biBitCount==4)
			{	//16ɫͼ
				int ibyte=row*image.bmWidthBytes+i/2;
				if(i%2==0)
					colorindex=(fileBytes[ibyte]&0xf0)>>4;
				else
					colorindex=fileBytes[ibyte]&0x0f;
				crPixelColor=palette[colorindex];
			}
			else if(infoHeader.biBitCount==8)
			{	//256ɫͼ
				int ibyte=row*image.bmWidthBytes+i;
				colorindex=fileBytes[ibyte];
			}
			else if(infoHeader.biBitCount==24||infoHeader.biBitCount==32)
			{	//���24λ��32λ
				BYTE ciPixelBytes=infoHeader.biBitCount/8;
				int ibyte=row*image.bmWidthBytes+i*ciPixelBytes;
				memcpy(&crPixelColor,&fileBytes[ibyte],ciPixelBytes);
			}
			memcpy(&bit32bytes[rowi*bm32WidthBytes+i*4],&crPixelColor,4);
		}
	}

	image.bmBitsPixel=32;
	image.bmBits=bit32bytes;
	image.bmWidthBytes=bm32WidthBytes;
	if(!xBitMap.CreateBitmapIndirect(&image))
		return FALSE;
	return TRUE;
}
CString MakeMapFrameSizeSetString()
{
	CString mapFrameStr;
	for(MAP_FRAME_SIZE* pMapFrame=sys.m_hashMapFrameList.GetFirst();pMapFrame;pMapFrame=sys.m_hashMapFrameList.GetNext())
	{
		mapFrameStr+=pMapFrame->sMapFrameName;
		mapFrameStr+='|';
	}
	return mapFrameStr;
}
CString MakeLayerSetString()
{
	CString layerSetStr;
	char text[MAX_TEMP_CHAR_100+1]="";
	int nNodeNum=LayerTable::layerHashList.GetNodeNum();
	for(TMADRAWING_LAYER *pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
	{
		sprintf(text,"%s",(char*)pLayer->layerName);
		layerSetStr+=text;
		layerSetStr+='|';
	}
	return layerSetStr;
}
CXhChar500 MakeFontNameStr()
{
	//����ͼ�����������ļ�
	char sys_path[MAX_PATH];
	GetSysPath(sys_path);
	strcat(sys_path,"*.shx");
	CFileFind fileFind;
	CXhChar500 sCmbItems="����|����_GB2312|����|gbhzfs.shx|hztxt.shx|hzfs.shx|gvfs.shx";
	BOOL bFind=fileFind.FindFile(sys_path);
	while(bFind)
	{
		CString sFontName=fileFind.GetFileName();
		bFind = fileFind.FindNextFile();
		if(sFontName.CompareNoCase("gbhzfs.shx")==0
			||sFontName.CompareNoCase("hztxt.shx")==0
			||sFontName.CompareNoCase("hzfs.shx")==0
			||sFontName.CompareNoCase("gvfs.shx")==0
			||sFontName.GetLength()<=0)
			continue;
		sCmbItems.Append("|");
		sCmbItems.Append(sFontName.GetBuffer(1));
	}
	return sCmbItems;
}
//���������趨�� 0.��׼���� 1.��ͼ 2.˾��ͼ 3.�ṹͼ 4.����ͼ 5.�麸ͼ
void CreatNewPropItem(CPropertyList *pPropList,CPropTreeItem* pItem,char *sProp,int txt_size0_wtoh1=0)	
{
	if(sProp==NULL||strlen(sProp)<=0)
		return;
	CStringArray strArr;
	strArr.SetSize(5);
#ifdef AFX_TARG_ENU_ENGLISH
	strArr[0] = "general map";
	strArr[1] = "force line map";
	strArr[2] = "structure map";
	strArr[3] = "part map";
	strArr[4] = "weld map";
#else
	strArr[0] = "��ͼ";
	strArr[1] = "˾��ͼ";
	strArr[2] = "�ṹͼ";
	strArr[3] = "����ͼ";
	strArr[4] = "�麸ͼ";
#endif
	CSpecialSettingDlg dlg;
	dlg.txt_size0_wtoh1=txt_size0_wtoh1;
	strcpy(dlg.m_sProp,sProp);
	if(dlg.DoModal()!=IDOK)
		return;
	char sStr[50]="";
	double fDatumValue = 0;
	if(txt_size0_wtoh1==0)		//�ָ�
		fDatumValue = sys.fonts.TextSize(sProp);
	else if(txt_size0_wtoh1==1)	//�����߱�
		fDatumValue = sys.fonts.TextWtoH(sProp);
	for(int j=0;j<5;j++)
	{
		long idSonProp=0;
		CSystemSettingDlg::GetPropStr(pItem->m_idProp,sStr);	//��ȡ�������ַ���
		sprintf(sStr,"%s%d",sStr,j+1);							//�õ��������ַ���
		idSonProp = CSystemSettingDlg::GetPropID(sStr);			//������ID
		CPropTreeItem* pSonPropItem=pPropList->FindItemByPropId(idSonProp,NULL);
		if(!dlg.m_arrIsCanUse[j]			//�����������
		||fabs(dlg.m_arrValue[j])<EPS	//������ֵΪ0
			||fabs(dlg.m_arrValue[j]-fDatumValue)<EPS) //������ֵ�븸����ֵ���
		{
			if(pSonPropItem)
			{	//ɾ��������
				pPropList->DeleteItemByPropId(pSonPropItem->m_idProp);
				if(txt_size0_wtoh1==0)		//����߶�
					sys.fonts.SetTextSize(sProp,fDatumValue,j+1);
				else if(txt_size0_wtoh1==1)	//�����߱�
					sys.fonts.SetTextWtoH(sProp,fDatumValue,j+1);
			}
			continue;	
		}
		if(pSonPropItem==NULL)
		{	//���������
			CItemInfo* lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = strArr[j]+pItem->m_lpNodeInfo->m_strPropName;
			lpInfo->m_strPropHelp = strArr[j]+pItem->m_lpNodeInfo->m_strPropHelp;
			lpInfo->m_strPropValue.Format("%.2f",dlg.m_arrValue[j]);
			pSonPropItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
			CSystemSettingDlg::GetPropStr(pItem->m_idProp,sStr);	//��ȡ�������ַ���
			sprintf(sStr,"%s%d",sStr,j+1);	//�õ��������ַ���
			pSonPropItem->m_idProp = CSystemSettingDlg::GetPropID(sStr);
			pSonPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
		}
		else//��������������ֵ
			pPropList->SetItemPropValue(idSonProp,"%.2f",dlg.m_arrValue[j]);
		//����������ֵ
		if(txt_size0_wtoh1==0)		//����߶�
			sys.fonts.SetTextSize(sProp,dlg.m_arrValue[j],j+1);
		else if(txt_size0_wtoh1==1)	//�����߱�
			sys.fonts.SetTextWtoH(sProp,dlg.m_arrValue[j],j+1);
	}
}
void CreatNewLayerPropItem(CPropertyList *pPropList,CPropTreeItem* pItem)	
{
	CAddNewLayerDlg dlg;
	if(dlg.DoModal()!=IDOK)
		return;
	//��ӵ���ϣ����
	CreateNewLayer(dlg.m_xLayer.layerName,dlg.m_xLayer.lineTypeName,dlg.m_xLayer.lineWeight,
		dlg.m_xLayer.colorIndex,dlg.m_xLayer.layerId,dlg.m_xLayer.lineTypeId);
	TMADRAWING_LAYER* pNewLayer=LayerTable::layerHashList.SetValue(dlg.m_xLayer.layerName,dlg.m_xLayer);
	CSystemSettingDlg* pSysSetDlg=(CSystemSettingDlg*)pPropList->GetParent();
	if(pSysSetDlg)
	{	//�����ͼ��󣬸���������������ר��ͼ�����Ƽ���������ͼ���б�
		pSysSetDlg->InsertOrUpdateLayerItem(pNewLayer,pItem);
		pSysSetDlg->UpdateSpecialLayerItem();
	}
}

BOOL DeletePropItem2(CPropertyList *pPropList,CPropTreeItem* pItem,TMADRAWING_LAYER *pLayer)
{
	//�ж���ר��ͼ�����Ƿ�ռ�õ�
	CSystemSettingDlg* pSysSetDlg=(CSystemSettingDlg*)pPropList->GetParent();
	if(pSysSetDlg==NULL)
		return FALSE;
	if(pSysSetDlg->IsUseLayer(pLayer))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("This layer is used by special layer, unable to delete!");
#else
		AfxMessageBox("��ͼ�����ڱ�ר��ͼ��ʹ��,�޷�ɾ��!");
#endif
		return TRUE;
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(IDOK==MessageBox(NULL,"Are you sure to delete the selected layer? ", "prompt",IDOK))
#else
		if(IDOK==MessageBox(NULL,"ȷ��Ҫɾ��ѡ��ͼ����","��ʾ",IDOK))
#endif
		{
			LayerTable::layerHashList.DeleteNode(pLayer->layerName);
			pPropList->DeleteItemByPropId(pItem->m_idProp);
			//ɾ��ͼ��󣬸���ר��ͼ�����Ƽ���������ͼ���б�
			CSystemSettingDlg* pSysSetDlg=(CSystemSettingDlg*)pPropList->GetParent();
			if(pSysSetDlg)
				pSysSetDlg->UpdateSpecialLayerItem();
			return FALSE;
		}
		else 
			return TRUE;
	}
}
//�ص�����
BOOL FireSystemSettingHelpStr(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CSystemSettingDlg* pDlg=(CSystemSettingDlg*)pPropList->GetParent();
	if(pDlg==NULL)
		return FALSE;
	if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.iJgZoomSchema"))
	{
		//DRAWING_PARA draw_para(DRAWING_PARA::PART_MAP,DRAWING_PARA::DRAW_LINE_ANGLE);
		//pDlg->SetDrawingPara(draw_para);
	}
	//else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.PartNo.fPartNoTextSize"))
	//{
	//	pDlg->SetPromptPicture(IDB_BMP_BODY_QUAD);
	//}
	else
		pDlg->SetOtherOper();
	return TRUE;
}
BOOL ButtonClickSystemSetting(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	CXhChar16 sExt("dwg");	//��չ��
	CXhChar100 Filter;
	CXhChar500 SelectedFile;
	if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sJgCard"))
#ifdef AFX_TARG_ENU_ENGLISH
    		Filter.Printf("%s","Angle process card(*.dwg)|*.dwg||");
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sSlotCard"))
		Filter.Printf("%s","U-steel process card(*.dwg)|*.dwg||");
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sTubeCard"))
		Filter.Printf("%s","Tube process card(*.dwg)|*.dwg||");
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sPlankCard"))
		Filter.Printf("%s","Plate process card(*.dwg)|*.dwg||");
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sWeldPartGroupCard"))
		Filter.Printf("%s","Group Weld parts process card(*.dwg)|*.dwg||");
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sProcessPartStatCard"))
		Filter.Printf("%s","Process statistical card(*.dwg)|*.dwg||");
#else
		Filter.Printf("%s","�Ǹֹ��տ�(*.dwg)|*.dwg||");
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sSlotCard"))
		Filter.Printf("%s","�۸ֹ��տ�(*.dwg)|*.dwg||");
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sTubeCard"))
		Filter.Printf("%s","�ֹܹ��տ�(*.dwg)|*.dwg||");
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sPlankCard"))
		Filter.Printf("%s","�ְ幤�տ�(*.dwg)|*.dwg||");
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sWeldPartGroupCard"))
		Filter.Printf("%s","�麸�����տ�(*.dwg)|*.dwg||");
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sProcessPartStatCard"))
		Filter.Printf("%s","���ռ�ͳ�ƿ�(*.dwg)|*.dwg||");
#endif
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bUseDimStyleConfigFile"))
	{	//���иְ���ͼ��ϸ����
		CPlankPartParaExportDlg dlg;
		dlg.DoModal();
	}
	//ͼ��
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("LayerTable.layerHashList"))
		CreatNewLayerPropItem(pPropList,pItem);//����µ�ͼ��
	else 
	{
		TMADRAWING_LAYER *pLayer;
		for(pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
		{
			if((long)pLayer==pItem->m_idProp)
				return DeletePropItem2(pPropList,pItem,pLayer);
		}
	
		sExt.Copy("arx");
		if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sTMAMapArxPath"))
#ifdef AFX_TARG_ENU_ENGLISH
    			Filter.Printf("%s","TMA map module(*.arx)|*.arx||");
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sLMAMapArxPath"))
			Filter.Printf("%s","LMA map module(*.arx)|*.arx||");
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sLDSMapArxPath"))
			Filter.Printf("%s","LDS map module(*.arx)|*.arx||");
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sTSAMapArxPath"))
			Filter.Printf("%s","TSA map module(*.arx)|*.arx||");
#else
			Filter.Printf("%s","TMA��ͼģ��(*.arx)|*.arx||");
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sLMAMapArxPath"))
			Filter.Printf("%s","LMA��ͼģ��(*.arx)|*.arx||");
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sLDSMapArxPath"))
			Filter.Printf("%s","LDS��ͼģ��(*.arx)|*.arx||");
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sTSAMapArxPath"))
			Filter.Printf("%s","TSA��ͼģ��(*.arx)|*.arx||");
#endif
	}
	if(Filter.GetLength()>0)
	{
		CFileDialog FileDlg(TRUE,sExt,NULL,NULL,Filter);
		CString currPath = pItem->m_lpNodeInfo->m_strPropValue;
		FileDlg.m_ofn.lpstrTitle = "Select file";
		if (currPath.GetLength() > 0)
			FileDlg.m_ofn.lpstrInitialDir = currPath.Left(
			currPath.GetLength() - currPath.ReverseFind('\\'));
		if(IDOK!=FileDlg.DoModal())
			return FALSE;
		SelectedFile.Printf("%s",FileDlg.GetPathName());
		
		if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sJgCard"))
			sprintf(sys.general_set.m_sJgCard,"%s",&SelectedFile);
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sSlotCard"))
			sprintf(sys.general_set.m_sSlotCard,"%s",&SelectedFile);
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sTubeCard"))
			sprintf(sys.general_set.m_sTubeCard,"%s",&SelectedFile);
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sPlankCard"))
			sprintf(sys.general_set.m_sPlankCard,"%s",&SelectedFile);
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sWeldPartGroupCard"))
			sprintf(sys.general_set.m_sWeldPartGroupCard,"%s",&SelectedFile);
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sProcessPartStatCard"))
			sprintf(sys.general_set.m_sProcessPartStatCard,"%s",&SelectedFile);
		//��ͼģ��·�� wht 10-11-16
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sTMAMapArxPath"))
			sprintf(sys.general_set.m_sTMAMapArxPath,"%s",&SelectedFile);
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sLMAMapArxPath"))
			sprintf(sys.general_set.m_sLMAMapArxPath,"%s",&SelectedFile);
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sLDSMapArxPath"))
			sprintf(sys.general_set.m_sLDSMapArxPath,"%s",&SelectedFile);
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sTSAMapArxPath"))
			sprintf(sys.general_set.m_sTSAMapArxPath,"%s",&SelectedFile);
		pItem->m_lpNodeInfo->m_strPropValue.Format("%s",&SelectedFile);
	}
	return TRUE;
}
//�޸�ָ����Ĳ���ֵ
//�ṹͼ
static BOOL ModifyStruMapValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CSystemSettingDlg* pDlg=(CSystemSettingDlg*)pPropList->GetParent();
	if(pDlg==NULL)
		return FALSE;
	CPropertyListOper<CSystemSettingDlg> oper(pPropList,pDlg);
	if(pItem->m_idProp==CSystemSettingDlg::GetPropID("stru_map.fMinWingWide"))
		sys.stru_map.fMinWingWide=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("stru_map.fMinPartThick"))
		sys.stru_map.fMinPartThick=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("stru_map.bExportByEnglish"))
		sys.stru_map.bExportByEnglish=valueStr[0]-'0';
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("stru_map.bDimSpecialLsSpace"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.stru_map.bDimSpecialLsSpace=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.stru_map.bDimSpecialLsSpace=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.stru_map.bDimSpecialLsSpace=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("stru_map.bDimSpecialJgZhun"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.stru_map.bDimSpecialJgZhun=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.stru_map.bDimSpecialJgZhun=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.stru_map.bDimSpecialJgZhun=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("stru_map.bDimStruJgLen"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.stru_map.bDimStruJgLen=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.stru_map.bDimStruJgLen=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.stru_map.bDimStruJgLen=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("stru_map.bDimPartGuiGe"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.stru_map.bDimPartGuiGe=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.stru_map.bDimPartGuiGe=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.stru_map.bDimPartGuiGe=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("stru_map.bReadAdditionalBuffer"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.stru_map.bReadAdditionalBuffer=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.stru_map.bReadAdditionalBuffer=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.stru_map.bReadAdditionalBuffer=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("stru_map.bDrawCutAngleAndWingInfo"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
    		if(valueStr.CompareNoCase("Yes")==0)
			sys.stru_map.bDrawCutAngleAndWingInfo=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.stru_map.bDrawCutAngleAndWingInfo=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.stru_map.bDrawCutAngleAndWingInfo=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("stru_map.bOnlyDrawHorizontalPlate"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.stru_map.bOnlyDrawHorizontalPlate=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.stru_map.bOnlyDrawHorizontalPlate=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.stru_map.bOnlyDrawHorizontalPlate=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("stru_map.bUsePolyLineDrawEdge"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.stru_map.bUsePolyLineDrawEdge=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.stru_map.bUsePolyLineDrawEdge=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.stru_map.bUsePolyLineDrawEdge=FALSE;
		pPropList->DeleteItemByPropId(CSystemSettingDlg::GetPropID("stru_map.fPolyLineWidth"));
		if(sys.stru_map.bUsePolyLineDrawEdge)
			oper.InsertEditPropItem(pItem,"stru_map.fPolyLineWidth","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("m_pCurMapFrm.style"))
	{
		BOOL bRetCode=TRUE;
		CXhChar100 sText;
#ifdef AFX_TARG_ENU_ENGLISH
		if (valueStr.CompareNoCase("new ...")==0)
#else
		if (valueStr.CompareNoCase("�½�...")==0)
#endif
		{
			CNewMapFrameDlg dlg;
			if(dlg.DoModal()==IDOK)
			{
				CSystemSettingDlg::m_pCurMapFrm=&dlg.m_xMapFrm;
				if (CSystemSettingDlg::m_pCurMapFrm!=NULL)
					sys.m_hashMapFrameList.SetValue(CSystemSettingDlg::m_pCurMapFrm->sMapFrameName,*(CSystemSettingDlg::m_pCurMapFrm));
			}
			if(pDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pItem->m_lpNodeInfo->m_cmbItems=MakeMapFrameSizeSetString()+"new ..";
#else
				pItem->m_lpNodeInfo->m_cmbItems=MakeMapFrameSizeSetString()+"�½�...";
#endif
				pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
			}
			bRetCode=FALSE;	//�½�ͼ���Ӧ����FALSE,�������������ݲ�����ȷ��ʾΪ�½���ͼ��
		}
		else
		{
			CXhChar100 sKey;
			sKey.Printf("%s",valueStr);
			CSystemSettingDlg::m_pCurMapFrm=sys.m_hashMapFrameList.GetValue(sKey);
		}
		if (CSystemSettingDlg::m_pCurMapFrm!=NULL)
		{
			long idProp=CSystemSettingDlg::GetPropID("m_pCurMapFrm.nLength");
			if(pDlg->GetPropValueStr(idProp,sText)>0)
			{
				pPropList->SetItemPropValue(idProp,CString(sText));
				pPropList->SetItemReadOnly(idProp,CSystemSettingDlg::m_pCurMapFrm->style<=9);
			}
			idProp=CSystemSettingDlg::GetPropID("m_pCurMapFrm.nWidth");
			if(pDlg->GetPropValueStr(idProp,sText)>0)
			{
				pPropList->SetItemPropValue(idProp,CString(sText));
				pPropList->SetItemReadOnly(idProp,CSystemSettingDlg::m_pCurMapFrm->style<=9);
			}
			idProp=CSystemSettingDlg::GetPropID("m_pCurMapFrm.nLeftMargin");
			if(pDlg->GetPropValueStr(idProp,sText)>0)
			{
				pPropList->SetItemPropValue(idProp,CString(sText));
				pPropList->SetItemReadOnly(idProp,CSystemSettingDlg::m_pCurMapFrm->style<=9);
			}
			idProp=CSystemSettingDlg::GetPropID("m_pCurMapFrm.nOtherMargin");
			if(pDlg->GetPropValueStr(idProp,sText)>0)
			{
				pPropList->SetItemPropValue(idProp,CString(sText));
				pPropList->SetItemReadOnly(idProp,CSystemSettingDlg::m_pCurMapFrm->style<=9);
			}
			idProp=CSystemSettingDlg::GetPropID("m_pCurMapFrm.nBottomRightFixLen");
			if(pDlg->GetPropValueStr(idProp,sText)>0)
			{
				pPropList->SetItemPropValue(idProp,CString(sText));
				pPropList->SetItemReadOnly(idProp,CSystemSettingDlg::m_pCurMapFrm->style<=9);
			}
			idProp=CSystemSettingDlg::GetPropID("m_pCurMapFrm.nBottomRightFixWidth");
			if(pDlg->GetPropValueStr(idProp,sText)>0)
			{
				pPropList->SetItemPropValue(idProp,CString(sText));
				pPropList->SetItemReadOnly(idProp,CSystemSettingDlg::m_pCurMapFrm->style<=9);
			}
		}
		return bRetCode;
	}
	else if(CSystemSettingDlg::GetPropID("m_pCurMapFrm.nLength")==pItem->m_idProp)
	{
		CSystemSettingDlg::m_pCurMapFrm->nLength=atoi(valueStr);
		sys.m_hashMapFrameList.SetValue(CSystemSettingDlg::m_pCurMapFrm->sMapFrameName,*(CSystemSettingDlg::m_pCurMapFrm));
	}
	else if(CSystemSettingDlg::GetPropID("m_pCurMapFrm.nWidth")==pItem->m_idProp)
	{
		CSystemSettingDlg::m_pCurMapFrm->nWidth=atoi(valueStr);
		sys.m_hashMapFrameList.SetValue(CSystemSettingDlg::m_pCurMapFrm->sMapFrameName,*(CSystemSettingDlg::m_pCurMapFrm));
	}
	else if(CSystemSettingDlg::GetPropID("m_pCurMapFrm.nLeftMargin")==pItem->m_idProp)
	{
		CSystemSettingDlg::m_pCurMapFrm->nLeftMargin=atoi(valueStr);
		sys.m_hashMapFrameList.SetValue(CSystemSettingDlg::m_pCurMapFrm->sMapFrameName,*(CSystemSettingDlg::m_pCurMapFrm));
	}
	else if(CSystemSettingDlg::GetPropID("m_pCurMapFrm.nOtherMargin")==pItem->m_idProp)
	{
		CSystemSettingDlg::m_pCurMapFrm->nRightMargin=CSystemSettingDlg::m_pCurMapFrm->nTopMargin
			=CSystemSettingDlg::m_pCurMapFrm->nBottomMargin=atoi(valueStr);
		sys.m_hashMapFrameList.SetValue(CSystemSettingDlg::m_pCurMapFrm->sMapFrameName,*(CSystemSettingDlg::m_pCurMapFrm));
	}
	else if(CSystemSettingDlg::GetPropID("m_pCurMapFrm.nBottomRightFixLen")==pItem->m_idProp)
	{
		CSystemSettingDlg::m_pCurMapFrm->nBottomRightFixLen=atoi(valueStr);
		sys.m_hashMapFrameList.SetValue(CSystemSettingDlg::m_pCurMapFrm->sMapFrameName,*(CSystemSettingDlg::m_pCurMapFrm));
	}
	else if(CSystemSettingDlg::GetPropID("m_pCurMapFrm.nBottomRightFixWidth")==pItem->m_idProp)
	{
		CSystemSettingDlg::m_pCurMapFrm->nBottomRightFixWidth=atoi(valueStr);
		sys.m_hashMapFrameList.SetValue(CSystemSettingDlg::m_pCurMapFrm->sMapFrameName,*(CSystemSettingDlg::m_pCurMapFrm));
	}
	//�ṹͼ�ָ�����
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("stru_map.fPolyLineWidth"))
		sys.stru_map.fPolyLineWidth=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("stru_map.fPoleAxisLineStartOutSpreadLen"))
		sys.stru_map.fPoleAxisLineStartOutSpreadLen=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("stru_map.fPoleAxisLineEndOutSpreadLen"))
		sys.stru_map.fPoleAxisLineEndOutSpreadLen=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fLsGuigeTextSize"))
		sys.fonts.SetTextSize("LsGuigeText",atof(valueStr));
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fJgOddmentTextSize"))
		sys.fonts.SetTextSize("JgOddmentText",atof(valueStr));
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fSimPolyTextSize"))
		sys.fonts.SetTextSize("SimPolyText",atof(valueStr));
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fPlateThickTextSize"))
		sys.fonts.SetTextSize("PlateThickText",atof(valueStr));
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fOtherTextSize"))
		sys.fonts.SetTextSize("OtherText",atof(valueStr));
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fPartGuigeTextSize"))
		sys.fonts.SetTextSize("PartGuigeText",atof(valueStr));
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fSimPolyTextSize"))
		sys.fonts.SetTextSize("SimPolyText",atof(valueStr));
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fSimTriScale"))
		sys.dim_map.fSimTriScale=atof(valueStr);
	else
		return FALSE;
	return TRUE;
}
//�麸ͼ
static BOOL ModifyPartGroupMapValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CSystemSettingDlg* pDlg=(CSystemSettingDlg*)pPropList->GetParent();
	if(pDlg==NULL)
		return FALSE;
	CPropertyListOper<CSystemSettingDlg> oper(pPropList,pDlg);
	if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.fMinFontSize"))
		sys.part_group_map.fMinFontSize=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.fMinDrawThick"))
		sys.part_group_map.fMinDrawThick=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.nKmJgGIncrement"))
		sys.part_group_map.nKmJgGIncrement=atoi(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.bDrawWeldLineSymbol"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.bDrawWeldLineSymbol=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.bDrawWeldLineSymbol=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.bDrawWeldLineSymbol=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.iPlateLocateDrawStyle"))
	{
		sys.part_group_map.iPlateLocateDrawStyle=valueStr[0]-'0';
		pPropList->DeleteAllSonItems(pItem);
		if(sys.part_group_map.iPlateLocateDrawStyle>0)
			oper.InsertEditPropItem(pItem,"part_group_map.iLocateSymboleHeight","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.iLocateSymboleHeight"))
	{
		sys.part_group_map.iLocateSymboleHeight=atoi(valueStr);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.bAutoDimPartNo"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.bAutoDimPartNo=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.bAutoDimPartNo=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.bAutoDimPartNo=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.bAutoMatchView"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.bAutoMatchView=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.bAutoMatchView=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.bAutoMatchView=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.bModulatePartGroupLen"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.bModulatePartGroupLen=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.bModulatePartGroupLen=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.bModulatePartGroupLen=FALSE;
		pPropList->DeleteAllSonItems(pItem);
		if(sys.part_group_map.bModulatePartGroupLen)
		{
			oper.InsertEditPropItem(pItem,"part_group_map.fPartGroupDistThreshold","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"part_group_map.fPartGroupDistZoomCoef","","",-1,TRUE);
		}
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.bAngularDimension"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.bAngularDimension=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.bAngularDimension=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.bAngularDimension=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.bDimBeforeWeldLocatSpace"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.bDimBeforeWeldLocatSpace=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.bDimBeforeWeldLocatSpace=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.bDimBeforeWeldLocatSpace=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.bDimAbsoluteDist"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.bDimAbsoluteDist=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.bDimAbsoluteDist=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.bDimAbsoluteDist=FALSE;
	}else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.bDimAfterWeldCheckLine"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.bDimAfterWeldCheckLine=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.bDimAfterWeldCheckLine=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.bDimAfterWeldCheckLine=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.bSmartFilterRedundantPart"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.bSmartFilterRedundantPart=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.bSmartFilterRedundantPart=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.bSmartFilterRedundantPart=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.bDimPlatePoleRayAndAngle"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.bDimPlatePoleRayAndAngle=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.bDimPlatePoleRayAndAngle=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.bDimPlatePoleRayAndAngle=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.fPartGroupDistThreshold"))
		sys.part_group_map.fPartGroupDistThreshold=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.fPartGroupDistZoomCoef"))
		sys.part_group_map.fPartGroupDistZoomCoef=atof(valueStr);
			//�麸����ϸ��
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.bCreatePartGroupBOM"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.bCreatePartGroupBOM=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.bCreatePartGroupBOM=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.bCreatePartGroupBOM=FALSE;
		pPropList->DeleteAllSonItems(pItem);
		if(sys.part_group_map.bCreatePartGroupBOM)
		{	//������ϸ��
			oper.InsertCmbListPropItem(pItem,"part_group_map.part_table.bZoomInProcessCard","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_group_map.part_table.bDispPartGroupNo","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_group_map.part_table.bDispPartGroupNum","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_group_map.part_table.bDispPartNo","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_group_map.part_table.bDispPartNum","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_group_map.part_table.bDispPartGuiGe","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_group_map.part_table.bDispPartLen","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_group_map.part_table.bDispPartWeight","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_group_map.part_table.bDispNotes","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_group_map.part_table.bDispInternal","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_group_map.part_table.bDispAppearance","","","",-1,TRUE);
		}
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.part_table.bZoomInProcessCard"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.part_table.bZoomInProcessCard=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.part_table.bZoomInProcessCard=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.part_table.bZoomInProcessCard=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.part_table.bDispPartGroupNo"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.part_table.bDispPartGroupNo=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.part_table.bDispPartGroupNo=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.part_table.bDispPartGroupNo=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.part_table.bDispPartGroupNum"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.part_table.bDispPartGroupNum=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.part_table.bDispPartGroupNum=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.part_table.bDispPartGroupNum=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.part_table.bDispPartNo"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.part_table.bDispPartNo=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.part_table.bDispPartNo=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.part_table.bDispPartNo=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.part_table.bDispPartNum"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.part_table.bDispPartNum=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.part_table.bDispPartNum=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.part_table.bDispPartNum=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.part_table.bDispPartGuiGe"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.part_table.bDispPartGuiGe=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.part_table.bDispPartGuiGe=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.part_table.bDispPartGuiGe=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.part_table.bDispPartLen"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.part_table.bDispPartLen=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.part_table.bDispPartLen=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.part_table.bDispPartLen=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.part_table.bDispPartWeight"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.part_table.bDispPartWeight=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.part_table.bDispPartWeight=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.part_table.bDispPartWeight=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.part_table.bDispNotes"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.part_table.bDispNotes=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.part_table.bDispNotes=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.part_table.bDispNotes=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.part_table.bDispInternal"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.part_table.bDispInternal=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.part_table.bDispInternal=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.part_table.bDispInternal=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_group_map.part_table.bDispAppearance"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_group_map.part_table.bDispAppearance=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_group_map.part_table.bDispAppearance=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_group_map.part_table.bDispAppearance=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("welding.textsize.bZoomWithScale"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.welding.bZoomAdaptiveWithScale=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.welding.bZoomAdaptiveWithScale=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.welding.bZoomAdaptiveWithScale=FALSE;
		//
		pPropList->DeleteAllSonItems(pItem);
		if(sys.welding.bZoomAdaptiveWithScale)
			oper.InsertEditPropItem(pItem,"welding.textsize.wiScale","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("welding.textsize.wiScale"))
		sys.welding.wiScaleModel2Draw=atoi(valueStr);
	else
		return FALSE;
	return TRUE;
}
//����ͼ
static BOOL ModifyPartMapValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CSystemSettingDlg* pDlg=(CSystemSettingDlg*)pPropList->GetParent();
	if(pDlg==NULL)
		return FALSE;
	CPropertyListOper<CSystemSettingDlg> oper(pPropList,pDlg);
	if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.iExportDeformedProfile"))
	{
		sys.part_map.iExportDeformedProfile=valueStr[0]-'0';
		pPropList->DeleteItemByPropId(CSystemSettingDlg::GetPropID("part_map.bDimDeformedProfileText"));
		if(sys.part_map.iExportDeformedProfile!=0)
			oper.InsertCmbListPropItem(pItem,"part_map.bDimDeformedProfileText","","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.bDimDeformedProfileText"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.bDimDeformedProfileText=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.bDimDeformedProfileText=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.bDimDeformedProfileText=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.fMinHuoquAngle"))
		sys.part_map.fMinHuoquAngle=atof(valueStr);	//С�ڴ˻����Ƕȵ����乹������ע������Ϣ
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.bDimPlaneAngle"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.bDimPlaneAngle=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.bDimPlaneAngle=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.bDimPlaneAngle=FALSE;
		pPropList->DeleteItemByPropId(CSystemSettingDlg::GetPropID("part_map.iDimAngleType"));
		if(sys.part_map.bDimPlaneAngle)
			oper.InsertCmbListPropItem(pItem,"part_map.iDimAngleType","","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.iDimAngleType"))
		sys.part_map.iDimAngleType=valueStr[0]-'0';
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.bDimHuoquType"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.bDimHuoquType=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.bDimHuoquType=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.bDimHuoquType=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.bUseZhengFanQun"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.bUseZhengFanQun=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.bUseZhengFanQun=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.bUseZhengFanQun=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.iPartMapStyle"))
		sys.part_map.iPartMapStyle=valueStr[0]-'0';
	//�Ǹֹ���ͼ
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.iJgZoomSchema"))
	{	
		sys.part_map.angle.iJgZoomSchema=valueStr[0]-'0';
		//
		pPropList->DeleteAllSonItems(pItem);
		if(sys.part_map.angle.iJgZoomSchema==3)
			oper.InsertCmbListPropItem(pItem,"part_map.angle.ciJgLenDrawMode","","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.ciJgLenDrawMode"))
		sys.part_map.angle.ciJgLenDrawMode=valueStr[0]-'0';
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bMaxExtendAngleLength"))
	{
		if(valueStr[0]=='0')
			sys.part_map.angle.bMaxExtendAngleLength=FALSE;
		else
			sys.part_map.angle.bMaxExtendAngleLength=TRUE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.fKaiHeJiaoThreshold"))
	{	
		sys.part_map.angle.fKaiHeJiaoThreshold=atof(valueStr);
		CLDSLineAngle::KAIHEJIAO_ANGLE_THRESHOLD=sys.part_map.angle.fKaiHeJiaoThreshold;	//Ĭ�Ͽ��ϽǱ�ע��ֵΪ2��
		//����ע������� wht 11-06-13
		sys.WritePublicSysParaToReg("Settings","KaiHeJiaoThreshold");
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.fKaiHeJiaoMinDeita"))
	{	
		sys.part_map.angle.fKaiHeJiaoMinDeita=atof(valueStr);
		//����ע������� wht 11-06-13
		sys.WritePublicSysParaToReg("Settings","KaiHeJiaoMinDeita");
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bJgUseSimpleLsMap"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.angle.bJgUseSimpleLsMap=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.angle.bJgUseSimpleLsMap=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.angle.bJgUseSimpleLsMap=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bDimLsAbsoluteDist"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.angle.bDimLsAbsoluteDist=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.angle.bDimLsAbsoluteDist=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.angle.bDimLsAbsoluteDist=FALSE;
		//Ӧ��ǰɾ����������������ѡ<��>���ظ�������� wht 16-01-28
		pPropList->DeleteItemByPropId(CSystemSettingDlg::GetPropID("part_map.angle.bMergeLsAbsoluteDist"));
		if(sys.part_map.angle.bDimLsAbsoluteDist)
			oper.InsertCmbListPropItem(pItem,"part_map.angle.bMergeLsAbsoluteDist","","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bMergeLsAbsoluteDist"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.angle.bMergeLsAbsoluteDist=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.angle.bMergeLsAbsoluteDist=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.angle.bMergeLsAbsoluteDist=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bDimRibPlatePartNo"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.angle.bDimRibPlatePartNo=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.angle.bDimRibPlatePartNo=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.angle.bDimRibPlatePartNo=FALSE;
		//
		pPropList->DeleteAllSonItems(pItem);
		if(sys.part_map.angle.bDimRibPlatePartNo)
			oper.InsertCmbListPropItem(pItem,"part_map.angle.bDimRibPlateMaterial","","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bDimRibPlateMaterial"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.angle.bDimRibPlateMaterial=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.angle.bDimRibPlateMaterial=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.angle.bDimRibPlateMaterial=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bDimRibPlateSetUpPos"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.angle.bDimRibPlateSetUpPos=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.angle.bDimRibPlateSetUpPos=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.angle.bDimRibPlateSetUpPos=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bOneCardMultiPart"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.angle.bOneCardMultiPart=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.angle.bOneCardMultiPart=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.angle.bOneCardMultiPart=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.iJgGDimStyle"))
	{
		sys.part_map.angle.iJgGDimStyle=valueStr[0]-'0';
		pPropList->DeleteItemByPropId(CSystemSettingDlg::GetPropID("part_map.angle.nMaxBoltNumStartDimG"));
		if(sys.part_map.angle.iJgGDimStyle==2)
			oper.InsertEditPropItem(pItem,"part_map.angle.nMaxBoltNumStartDimG","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.nMaxBoltNumStartDimG"))
		sys.part_map.angle.nMaxBoltNumStartDimG=atoi(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.iLsSpaceDimStyle"))
	{
		sys.part_map.angle.iLsSpaceDimStyle=valueStr[0]-'0';
		pPropList->DeleteItemByPropId(CSystemSettingDlg::GetPropID("part_map.angle.nMaxBoltNumAlongX"));
		if(sys.part_map.angle.iLsSpaceDimStyle==2)
			oper.InsertEditPropItem(pItem,"part_map.angle.nMaxBoltNumAlongX","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.nMaxBoltNumAlongX"))
		sys.part_map.angle.nMaxBoltNumAlongX=atoi(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bDimPushFlatMap"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.angle.bDimPushFlatMap=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.angle.bDimPushFlatMap=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.angle.bDimPushFlatMap=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bDimCutAngle"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.angle.bDimCutAngle=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.angle.bDimCutAngle=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.angle.bDimCutAngle=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bDimKaiHe"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.angle.bDimKaiHe=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.angle.bDimKaiHe=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.angle.bDimKaiHe=FALSE;
		//���¿��ϽǱ�ע�ӿ���
		sys.part_map.angle.bDimKaiheAngle=sys.part_map.angle.bDimKaiHe;
		sys.part_map.angle.bDimKaiheSumLen=sys.part_map.angle.bDimKaiHe;
		sys.part_map.angle.bDimKaiheSegLen=sys.part_map.angle.bDimKaiHe;
		sys.part_map.angle.bDimKaiheScopeMap=sys.part_map.angle.bDimKaiHe;
		//
		pPropList->DeleteAllSonItems(pItem);
		if(sys.part_map.angle.bDimKaiHe)
		{	//��ע���ϽǶ���
			oper.InsertCmbListPropItem(pItem,"part_map.angle.bDimKaiheAngle","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_map.angle.bDimKaiheSumLen","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_map.angle.bDimKaiheSegLen","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_map.angle.bDimKaiheScopeMap","","","",-1,TRUE);
		}
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bDimKaiheSumLen"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.angle.bDimKaiheSumLen=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.angle.bDimKaiheSumLen=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.angle.bDimKaiheSumLen=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bDimKaiheAngle"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.angle.bDimKaiheAngle=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.angle.bDimKaiheAngle=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.angle.bDimKaiheAngle=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bDimKaiheSegLen"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.angle.bDimKaiheSegLen=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.angle.bDimKaiheSegLen=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.angle.bDimKaiheSegLen=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bDimKaiheScopeMap"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.angle.bDimKaiheScopeMap=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.angle.bDimKaiheScopeMap=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.angle.bDimKaiheScopeMap=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.bDimCutAngleMap"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.angle.bDimCutAngleMap=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.angle.bDimCutAngleMap=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.angle.bDimCutAngleMap=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.angle.iCutAngleDimType"))
		sys.part_map.angle.iCutAngleDimType=valueStr[0]-'0';	//�Ǹ��нǱ�ע��ʽ
			//�ְ幹��ͼ
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDimPlateEdgeLen"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.plate.bDimPlateEdgeLen=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bDimPlateEdgeLen=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDimPlateEdgeLen=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDrawInnerEdge"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.plate.bDrawInnerEdge=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bDrawInnerEdge=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDrawInnerEdge=FALSE;
		pPropList->DeleteItemByPropId(CSystemSettingDlg::GetPropID("part_map.plate.fInnerEdgeOffset"));
		if(sys.part_map.plate.bDrawInnerEdge)
			oper.InsertEditPropItem(pItem,"part_map.plate.fInnerEdgeOffset","","",-1,TRUE);
	}
	else if (pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.fInnerEdgeOffset"))
		sys.part_map.plate.fInnerEdgeOffset=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bAdjustPlateUcs"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.plate.bAdjustPlateUcs=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bAdjustPlateUcs=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bAdjustPlateUcs=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDimCoord"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.plate.bDimCoord=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bDimCoord=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDimCoord=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDimArcEdgeR"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.plate.bDimArcEdgeR=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bDimArcEdgeR=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDimArcEdgeR=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDimArcAngle"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.plate.bDimArcAngle=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bDimArcAngle=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDimArcAngle=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDimPlateRayLs"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
   		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.plate.bDimPlateRayLs=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bDimPlateRayLs=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDimPlateRayLs=FALSE;
		//
		pPropList->DeleteAllSonItems(pItem);
		if(sys.part_map.plate.bDimPlateRayLs)
		{
			oper.InsertCmbListPropItem(pItem,"part_map.plate.bDimPlateBoltAngle","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_map.plate.bDimBoltEdgeSpace","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_map.plate.bDimDiffRayLsSpace","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_map.plate.bDimLsToHuoquLine","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"part_map.plate.bDimLsToWeldEdge","","","",-1,TRUE);
		}
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDimPlateBoltAngle"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.plate.bDimPlateBoltAngle=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bDimPlateBoltAngle=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDimPlateBoltAngle=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDimBoltEdgeSpace"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.plate.bDimBoltEdgeSpace=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bDimBoltEdgeSpace=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDimBoltEdgeSpace=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDimDiffRayLsSpace"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.plate.bDimDiffRayLsSpace=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bDimDiffRayLsSpace=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDimDiffRayLsSpace=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDimLsToHuoquLine"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.plate.bDimLsToHuoquLine=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bDimLsToHuoquLine=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDimLsToHuoquLine=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDimLsToWeldEdge"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.plate.bDimLsToWeldEdge=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bDimLsToWeldEdge=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDimLsToWeldEdge=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bPlateUseSimpleLsMap"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.plate.bPlateUseSimpleLsMap=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bPlateUseSimpleLsMap=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bPlateUseSimpleLsMap=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bZoomDimTextSizeByWidth"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
   		if(valueStr.CompareNoCase("Yes")==0)
			sys.part_map.plate.bZoomDimTextSizeByWidth=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bZoomDimTextSizeByWidth=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bZoomDimTextSizeByWidth=FALSE;
		pPropList->DeleteItemByPropId(CSystemSettingDlg::GetPropID("part_map.plate.fTextSizeZoomCoef"));
		if(sys.part_map.plate.bZoomDimTextSizeByWidth)
			oper.InsertEditPropItem(pItem,"part_map.plate.fTextSizeZoomCoef","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.fTextSizeZoomCoef"))
		sys.part_map.plate.fTextSizeZoomCoef=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDrawSlopeCutLine"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDrawSlopeCutLine=TRUE;
		else
			sys.part_map.plate.bDrawSlopeCutLine=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDrawDatumLine"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDrawDatumLine=TRUE;
		else
			sys.part_map.plate.bDrawDatumLine=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.iZhiWanLineType"))
	{
		sys.part_map.plate.iZhiWanLineType=valueStr[0]-'0';
		//
		pPropList->DeleteAllSonItems(pItem);
		if(sys.part_map.plate.iZhiWanLineType==1)
			oper.InsertEditPropItem(pItem,"part_map.plate.fInnerOffThreshold","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.fInnerOffThreshold"))
		sys.part_map.plate.fInnerOffThreshold=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.iCutArcsType"))
		sys.part_map.plate.iCutArcsType=valueStr[0]-'0';
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bMarkHuoquPos"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bMarkHuoquPos = TRUE;
		else //if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bMarkHuoquPos=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDrawHuoQuPrjMap"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDrawHuoQuPrjMap = TRUE;
		else //if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bDrawHuoQuPrjMap=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDrawHuoQuMarkLine"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDrawHuoQuMarkLine=TRUE;
		else //if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bDrawHuoQuMarkLine=FALSE;
	}
	else if (pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.bDisplaypPartInfoTbl"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			sys.part_map.plate.bDisplaypPartInfoTbl = TRUE;
		else //if(valueStr.CompareNoCase("��")==0)
			sys.part_map.plate.bDisplaypPartInfoTbl=FALSE;
		CPropertyListOper<CSystemSettingDlg> oper(pPropList,(CSystemSettingDlg*)pPropList->GetParent());
		pPropList->DeleteAllSonItems(pItem);
		if(sys.part_map.plate.bDisplaypPartInfoTbl)
			oper.InsertEditPropItem(pItem,"part_map.plate.nPartInfoTblColumnWidth","","",-1,TRUE);//������Ϣ���п�
	}
	else if (pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.plate.nPartInfoTblColumnWidth"))
		sscanf(valueStr,"%d",&sys.part_map.plate.nPartInfoTblColumnWidth);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.tube.fSlotLenProcessError"))
		sys.part_map.tube.fSlotLenProcessError=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.tube.fSlotWidthProcessError"))
		sys.part_map.tube.fSlotWidthProcessError=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("part_map.tube.fReelTubeThresholdD"))
		sys.part_map.tube.fReelTubeThresholdD=atof(valueStr);
	else
		return FALSE;
	return TRUE;
}

static BOOL ModifySystemSettingValue(CPropertyList	*pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CSystemSettingDlg* pDlg=(CSystemSettingDlg*)pPropList->GetParent();
	if(pDlg==NULL)
		return FALSE;
	CPropertyListOper<CSystemSettingDlg> oper(pPropList,pDlg);
	BOOL bContinueFind=FALSE;	//��������
	CItemInfo* lpInfo=NULL;
	CPropTreeItem *pSonPropItem=NULL;
	char* sLineTypeName="";
	//��������
		//��ͼ����
	if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.LineMapScale"))
		sys.general_set.LineMapScale=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.ForceLineScale"))
		sys.general_set.ForceLineScale=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.StruMapScale"))
		sys.general_set.StruMapScale.fRealToDraw=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.StruMapScale.iBoltSymbolStyle"))
	{
		sys.general_set.StruMapScale.iBoltSymbolStyle=valueStr[0]-'0';
		pPropList->DeleteAllSonItems(pItem);
		if(sys.general_set.StruMapScale.iBoltSymbolStyle>1)
			oper.InsertEditPropItem(pItem,"general_set.StruMapScale.fBoltSymbolFactor","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.StruMapScale.fBoltSymbolFactor"))
	{
		if(sys.general_set.StruMapScale.iBoltSymbolStyle==2)		//�ṹͼ����ͼ��
			sys.general_set.StruMapScale.fBoltSymbolFactor=atof(valueStr);
		else if(sys.general_set.StruMapScale.iBoltSymbolStyle==3)	//�ṹͼ����ֱ��ͼ��
			sys.general_set.StruMapScale.fBoltDiaFactor=atof(valueStr);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.PartMapScale"))
		sys.general_set.PartMapScale.fRealToDraw=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.PartMapScale.iBoltSymbolStyle"))
	{
		sys.general_set.PartMapScale.iBoltSymbolStyle=valueStr[0]-'0';
		pPropList->DeleteAllSonItems(pItem);
		if(sys.general_set.PartMapScale.iBoltSymbolStyle>1)
			oper.InsertEditPropItem(pItem,"general_set.PartMapScale.fBoltSymbolFactor","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.PartMapScale.fBoltSymbolFactor"))
	{
		if(sys.general_set.PartMapScale.iBoltSymbolStyle==2)
			sys.general_set.PartMapScale.fBoltSymbolFactor=atof(valueStr);
		else if(sys.general_set.PartMapScale.iBoltSymbolStyle==3)
			sys.general_set.PartMapScale.fBoltDiaFactor=atof(valueStr);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.WeldMapScale"))
		sys.general_set.WeldMapScale.fRealToDraw=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.WeldMapScale.iBoltSymbolStyle"))
	{
		sys.general_set.WeldMapScale.iBoltSymbolStyle=valueStr[0]-'0';
		pPropList->DeleteAllSonItems(pItem);
		if(sys.general_set.WeldMapScale.iBoltSymbolStyle>1)
			oper.InsertEditPropItem(pItem,"general_set.WeldMapScale.fBoltSymbolFactor","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.WeldMapScale.fBoltSymbolFactor"))
	{
		if(sys.general_set.WeldMapScale.iBoltSymbolStyle==2)
			sys.general_set.WeldMapScale.fBoltSymbolFactor=atof(valueStr);
		else if(sys.general_set.WeldMapScale.iBoltSymbolStyle==3)
			sys.general_set.WeldMapScale.fBoltDiaFactor=atof(valueStr);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.iMatCharPosType"))
	{	
		sys.general_set.iMatCharPosType=valueStr[0]-'0';
		//����ע������� wht 11-06-13
		sys.WritePublicSysParaToReg("Settings","MatCharPosType");
		pPropList->DeleteAllSonItems(pItem);
		if(sys.general_set.iMatCharPosType>0)
			oper.InsertCmbEditPropItem(pItem,"general_set.cMatCharSeparator","","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.cMatCharSeparator"))
	{	//������ŵĲ����ַ��ָ���
		if(valueStr.CompareNoCase("��")==0)
			sys.general_set.cMatCharSeparator=0;
		else if(valueStr.CompareNoCase("�ո�")==0)
			sys.general_set.cMatCharSeparator=' ';
		else
			sys.general_set.cMatCharSeparator=valueStr[0];
		sys.WritePublicSysParaToReg("Settings","MatSeparator");
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.fHoleAugmentInNC"))
		sys.general_set.fHoleAugmentInNC=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.fWeldLineSymbolHigh"))
		sys.general_set.fWeldLineSymbolHigh=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.fWeldLineSymbolSpace"))
		sys.general_set.fWeldLineSymbolSpace=atof(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.iDimPrecision"))
	{
		if(valueStr.CompareNoCase("0.5mm")==0)
			sys.general_set.iDimPrecision=1;
		else if(valueStr.CompareNoCase("0.1mm")==0)
			sys.general_set.iDimPrecision=2;
		else //if(valueStr.CompareNoCase("1.0mm")==0)
			sys.general_set.iDimPrecision=0;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.iPartBOMStyle"))
		sys.general_set.iPartBOMStyle=valueStr[0]-'0';
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.iSumMatBOMStyle"))
		sys.general_set.iSumMatBOMStyle=valueStr[0]-'0';
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("nBriefGuigeN"))
		sys.nBriefGuigeN=min(atoi(valueStr),26);	//�򻯹����
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("bDrawMainPoleJointLine"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.bDrawMainPoleJointLine=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.bDrawMainPoleJointLine=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.bDrawMainPoleJointLine=FALSE;
		//
		pPropList->DeleteAllSonItems(pItem);
		if(sys.bDrawMainPoleJointLine)
		{	
			oper.InsertEditPropItem(pItem,"nJointLineLen","","",-1,TRUE);	//�ֶα�־�߳���
			oper.InsertEditPropItem(pItem,"nJointLineOffset","","",-1,TRUE);//�ֶα�־�߳���
		}
	}	
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("nJointLineOffset"))
		sys.nJointLineOffset=atoi(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("nJointLineLen"))
		sys.nJointLineLen=atoi(valueStr);
		//���տ�·�������÷�ʽ
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.bCardLayoutBaseRow"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("0.by the reference of row")==0)
			sys.general_set.bCardLayoutBaseRow=TRUE;
		else if(valueStr.CompareNoCase("1.by the reference of column")==0)
#else
		if(valueStr.CompareNoCase("0.����Ϊ��׼")==0)
			sys.general_set.bCardLayoutBaseRow=TRUE;
		else if(valueStr.CompareNoCase("1.����Ϊ��׼")==0)
#endif
			sys.general_set.bCardLayoutBaseRow=FALSE;
		//
		long idProp=CSystemSettingDlg::GetPropID("general_set.nPartRowsN");//,NULL);
		if(sys.general_set.bCardLayoutBaseRow)
#ifdef AFX_TARG_ENU_ENGLISH
			pPropList->SetItemPropName(idProp,CString("max columns in a row"));
		else
			pPropList->SetItemPropName(idProp,CString("max rows in a column"));
#else
			pPropList->SetItemPropName(idProp,CString("ÿ���������"));
		else
			pPropList->SetItemPropName(idProp,CString("ÿ���������"));
#endif
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.nPartRowsN"))
		sys.general_set.nPartRowsN=atoi(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.nRowHigh"))
		sys.general_set.nRowHigh=atoi(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.nColumnWidth"))
		sys.general_set.nColumnWidth=atoi(valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.bHasFrame"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
   		if(valueStr.CompareNoCase("Yes")==0)
			sys.general_set.bHasFrame=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.general_set.bHasFrame=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.general_set.bHasFrame=FALSE;
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.bProcessCardTbl"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			sys.general_set.bProcessCardTbl=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
			sys.general_set.bProcessCardTbl=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			sys.general_set.bProcessCardTbl=FALSE;
		//
		pPropList->DeleteAllSonItems(pItem);
		if(sys.general_set.bProcessCardTbl)
		{	
			oper.InsertFilePathPropItem(pItem,"general_set.m_sJgCard","","",-1,TRUE);
			oper.InsertFilePathPropItem(pItem,"general_set.m_sSlotCard","","",-1,TRUE);	
#ifdef __COMPLEX_PART_INC_//defined(__LMA_FILE_)||defined(__LDS_FILE_)
			oper.InsertFilePathPropItem(pItem,"general_set.m_sTubeCard","","",-1,TRUE);	
#endif
			oper.InsertFilePathPropItem(pItem,"general_set.m_sPlankCard","","",-1,TRUE);
			oper.InsertFilePathPropItem(pItem,"general_set.m_sWeldPartGroupCard","","",-1,TRUE);
			oper.InsertFilePathPropItem(pItem,"general_set.m_sProcessPartStatCard","","",-1,TRUE);
		}
	}
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sJgCard"))
		_snprintf(sys.general_set.m_sJgCard,MAX_PATH-1,"%s",valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sSlotCard"))
		_snprintf(sys.general_set.m_sSlotCard,MAX_PATH-1,"%s",valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sTubeCard"))
		_snprintf(sys.general_set.m_sTubeCard,MAX_PATH-1,"%s",valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sPlankCard"))
		_snprintf(sys.general_set.m_sPlankCard,MAX_PATH-1,"%s",valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sWeldPartGroupCard"))
		_snprintf(sys.general_set.m_sWeldPartGroupCard,MAX_PATH-1,"%s",valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sProcessPartStatCard"))
		_snprintf(sys.general_set.m_sProcessPartStatCard,MAX_PATH-1,"%s",valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sTMAMapArxPath"))
		_snprintf(sys.general_set.m_sTMAMapArxPath,MAX_PATH-1,"%s",valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sLMAMapArxPath"))
		_snprintf(sys.general_set.m_sLMAMapArxPath,MAX_PATH-1,"%s",valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sLDSMapArxPath"))
		_snprintf(sys.general_set.m_sLDSMapArxPath,MAX_PATH-1,"%s",valueStr);
	else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.m_sTSAMapArxPath"))
		_snprintf(sys.general_set.m_sTSAMapArxPath,MAX_PATH-1,"%s",valueStr);
	else if (pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.lCrossarmCr"))
	{
		COLORREF crCurSel=0;
		char tem_str[100]="";
		sprintf(tem_str,"%s",valueStr);
		memmove(tem_str, tem_str+3, 97);	//����RGB
		sscanf(tem_str,"%X",&crCurSel);
		int index=GetNearestACI(crCurSel);
		sys.general_set.lCrossarmCr=GetColorFromIndex(index);
	} 
	else if (pItem->m_idProp==CSystemSettingDlg::GetPropID("general_set.rCrossarmCr"))
	{
		COLORREF crCurSel=0;
		char tem_str[100]="";
		sprintf(tem_str,"%s",valueStr);
		memmove(tem_str, tem_str+3, 97);	//����RGB
		sscanf(tem_str,"%X",&crCurSel);
		int index=GetNearestACI(crCurSel);
		sys.general_set.rCrossarmCr=GetColorFromIndex(index);
	}
	else if(!ModifyPartGroupMapValue(pPropList,pItem,valueStr)&&!ModifyStruMapValue(pPropList,pItem,valueStr)
		&&!ModifyPartMapValue(pPropList,pItem,valueStr))
	{	//ͼֽ��ע
		BOOL bChange = FALSE;
		COLORREF crCurSel=0;
		char tem_str[100]="";
		sprintf(tem_str,"%s",valueStr);
		memmove(tem_str, tem_str+3, 97);	//����RGB
		sscanf(tem_str,"%X",&crCurSel);
		//��������
		if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.sFontName"))
		{
			valueStr.Remove('\r');	//�Ƴ����������ж���Ŀո��лس��Ʊ��
			valueStr.Remove('\n');
			valueStr.Remove(' ');
			valueStr.Remove('\t');
			sprintf(sys.dim_map.sFontName,"%s",valueStr);
		}
		//
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fSegITextSize"))
			sys.dim_map.fSegITextSize=atof(valueStr);
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fSegICirD"))
			sys.dim_map.fSegICirD=atof(valueStr);
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.PartNo.iPartNoFrameStyle"))
		{
			int iStyle=valueStr[0]-'0';
			if(iStyle!=0&&iStyle!=1&&iStyle!=2)
				sys.dim_map.PartNo.iPartNoFrameStyle=-1;
			else
				sys.dim_map.PartNo.iPartNoFrameStyle=iStyle;
		}
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.PartNo.iFlPartNoFrameStyle"))
		{
			int iStyle=valueStr[0]-'0';
			if(iStyle!=0&&iStyle!=1&&iStyle!=2)
				sys.dim_map.PartNo.iFlPartNoFrameStyle=-1;
			else
				sys.dim_map.PartNo.iFlPartNoFrameStyle=iStyle;
		}
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.PartNo.iStdInsertPlatePartNoFrameStyle"))
		{
			int iStyle=valueStr[0]-'0';
			if(iStyle!=0&&iStyle!=1&&iStyle!=2)
				sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle=-1;
			else
				sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle=iStyle;
		}
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.PartNo.fPartNoTextSize"))
			sys.fonts.SetTextSize("PartNo",atof(valueStr));
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.PartNo.fPartNoCirD"))
			sys.dim_map.PartNo.fPartNoCirD=atof(valueStr);
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.PartNo.fMainPoleNoOuterCirD"))
			sys.dim_map.PartNo.fMainPoleNoOuterCirD=atof(valueStr);
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.PartNo.fMainPoleNoInnerCirD"))
			sys.dim_map.PartNo.fMainPoleNoInnerCirD=atof(valueStr);
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.PartNo.iMirMsg"))
			sys.dim_map.PartNo.iMirMsg=valueStr[0]-'0';	//�ԳƷ�ʽ
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.PartNo.fPartNoMargin"))
			sys.fonts.SetTextSize("PartNoMargin",atof(valueStr));
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.PartNo.bDrawRefLine"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("��")==0)
#endif
				sys.dim_map.PartNo.bDrawRefLine=TRUE;	//����������
			else
				sys.dim_map.PartNo.bDrawRefLine=FALSE;
		}
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fDimTextSize"))
		{
			sys.fonts.SetTextSize("DimText",atof(valueStr));			
			sys.dim_map.fDimGap=0.25*sys.dim_map.fDimTextSize;
			CString sDimGap;
			sDimGap.Format("%.2f",sys.dim_map.fDimGap);
			pPropList->SetItemPropValue(CSystemSettingDlg::GetPropID("dim_map.fDimGap"),sDimGap);

		}
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fDimGap"))
		{
			sys.dim_map.fDimGap=atof(valueStr);	//�������ע�߼�϶ֵ
			DimStyleTable::dimStyle = GetDimStyle("standard",sys.dim_map.fDimTextSize);
		}
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fCutawayViewPolyLineWidth"))
			sys.dim_map.fCutawayViewPolyLineWidth=atof(valueStr);
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fCutawayViewSymbolLen"))
			sys.fonts.SetTextWtoH("CutawayViewSymbolText",atof(valueStr));
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fCutawayViewSymbolTextSize"))
		{
			sys.fonts.SetTextSize("CutawayViewSymbolText",atof(valueStr));
			sys.fonts.SetTextWtoH("CutawayViewSymbolText",2*atof(valueStr));
			CXhChar100 sLen("%.2f",sys.fonts.TextWtoH("CutawayViewSymbolText"));
			pPropList->SetItemPropValue(CSystemSettingDlg::GetPropID("dim_map.fCutawayViewSymbolLen"),sLen);
		}
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.sArrowType"))
		{	//ʵ�ıպ�("")|���ıպ�("_CLOSEDBLANK")|�պ�("_CLOSED")|�������("_ARCHTICK")|
			//��б("_OBLIQUE")|30�Ƚ�("_OPEN30")|ֱ��("_OPEN90")";
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Hollow closed")==0)
				strcpy(sys.dim_map.sArrowType,"_CLOSEDBLANK");
			else if(valueStr.CompareNoCase("Closed")==0)
				strcpy(sys.dim_map.sArrowType,"_CLOSED");
			else if(valueStr.CompareNoCase("Architectural symbol")==0)
				strcpy(sys.dim_map.sArrowType,"_ARCHTICK");
			else if(valueStr.CompareNoCase("Tilt")==0)
				strcpy(sys.dim_map.sArrowType,"_OBLIQUE");
			else if(valueStr.CompareNoCase("30 degree")==0)
				strcpy(sys.dim_map.sArrowType,"_OPEN30");
			else if(valueStr.CompareNoCase("Right angle")==0)
#else
			if(valueStr.CompareNoCase("���ıպ�")==0)
				strcpy(sys.dim_map.sArrowType,"_CLOSEDBLANK");
			else if(valueStr.CompareNoCase("�պ�")==0)
				strcpy(sys.dim_map.sArrowType,"_CLOSED");
			else if(valueStr.CompareNoCase("�������")==0)
				strcpy(sys.dim_map.sArrowType,"_ARCHTICK");
			else if(valueStr.CompareNoCase("��б")==0)
				strcpy(sys.dim_map.sArrowType,"_OBLIQUE");
			else if(valueStr.CompareNoCase("30�Ƚ�")==0)
				strcpy(sys.dim_map.sArrowType,"_OPEN30");
			else if(valueStr.CompareNoCase("ֱ��")==0)
#endif
				strcpy(sys.dim_map.sArrowType,"_OPEN90");
			else //if(valueStr.CompareNoCase("ʵ�ıպ�")==0)
				strcpy(sys.dim_map.sArrowType,"");
			DimStyleTable::dimStyle = GetDimStyle("standard",sys.dim_map.fDimTextSize);
			DimStyleTable::dimStyle2 = GetDimStyle2("dimStyle2",sys.dim_map.fDimTextSize);
			DimStyleTable::dimStyle3 = GetDimStyle("dimStyle3",sys.dim_map.fDimTextSize,FALSE);
			DimStyleTable::dimStyle4 = GetDimStyle("dimStyle4",sys.dim_map.fDimTextSize,FALSE,FALSE);
		}
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fDimAngleTextSize"))
			sys.fonts.SetTextSize("DimAngleText",atof(valueStr));
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fDimArrowSize"))
			sys.dim_map.fDimArrowSize=atof(valueStr);
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fHeaderTextSize"))
			sys.fonts.SetTextSize("HeaderText",atof(valueStr));
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fCommonTextSize"))
			sys.fonts.SetTextSize("CommonText",atof(valueStr));
		else if(pItem->m_idProp==CSystemSettingDlg::GetPropID("dim_map.fNumericTextSize"))
			sys.fonts.SetTextSize("NumericText",atof(valueStr));
		else
			bContinueFind=TRUE;
		//ͼֽ��ע
		if(bContinueFind)
		{	//if else��ι����ڴ˴��
			bContinueFind=FALSE;
		//ͼ��
		if(CSystemSettingDlg::GetPropID("LayerTable.VisibleProfileLayer")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::VisibleProfileLayer.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::VisibleProfileLayer,LayerTable::VisibleProfileLayer.lineTypeId);
					break;
				}
			}
		}
		//
		else if(CSystemSettingDlg::GetPropID("LayerTable.UnvisibleProfileLayer")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::UnvisibleProfileLayer.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::UnvisibleProfileLayer,LayerTable::UnvisibleProfileLayer.lineTypeId);
					break;
				}
			}
		}
		//
		else if(CSystemSettingDlg::GetPropID("LayerTable.AuxLineLayer")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::AuxLineLayer.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::AuxLineLayer,LayerTable::AuxLineLayer.lineTypeId);
					break;
				}
			}
		}
		//
		else if(CSystemSettingDlg::GetPropID("LayerTable.AxisLineLayer")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::AxisLineLayer.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::AxisLineLayer,LayerTable::AxisLineLayer.lineTypeId);
					break;
				}
			}
		}
		//
		else if(CSystemSettingDlg::GetPropID("LayerTable.SonJgAxisLineLayer")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::SonJgAxisLineLayer.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::SonJgAxisLineLayer,LayerTable::SonJgAxisLineLayer.lineTypeId);
					break;
				}
			}
		}
		//
		else if(CSystemSettingDlg::GetPropID("LayerTable.BendLineLayer")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::BendLineLayer.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::BendLineLayer,LayerTable::BendLineLayer.lineTypeId);
					break;
				}
			}
		}
		//
		else if(CSystemSettingDlg::GetPropID("LayerTable.AssistHuoQuLineLayer")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::AssistHuoQuLineLayer.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::AssistHuoQuLineLayer,LayerTable::AssistHuoQuLineLayer.lineTypeId);
					break;
				}
			}
		}
		//
		else if(CSystemSettingDlg::GetPropID("LayerTable.DamagedSymbolLine")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::DamagedSymbolLine.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::DamagedSymbolLine,LayerTable::DamagedSymbolLine.lineTypeId);
					break;
				}
			}
		}
		//
		else if(CSystemSettingDlg::GetPropID("LayerTable.BoltSymbolLayer")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::BoltSymbolLayer.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::BoltSymbolLayer,LayerTable::BoltSymbolLayer.lineTypeId);
					break;
				}
			}
		}
		//
		else if(CSystemSettingDlg::GetPropID("LayerTable.BreakLineLayer")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::BreakLineLayer.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::BreakLineLayer,LayerTable::BreakLineLayer.lineTypeId);
					break;
				}
			}
		}
		//
		else if(CSystemSettingDlg::GetPropID("LayerTable.CommonLayer")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::CommonLayer.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::CommonLayer,LayerTable::CommonLayer.lineTypeId);
					break;
				}
			}
		}
		//
		else if(CSystemSettingDlg::GetPropID("LayerTable.BriefLineLayer")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::BriefLineLayer.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::BriefLineLayer,LayerTable::BriefLineLayer.lineTypeId);
				    break;
				}
			}
		}
		//
		else if(CSystemSettingDlg::GetPropID("LayerTable.BoltLineLayer")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::BoltLineLayer.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::BoltLineLayer,LayerTable::BoltLineLayer.lineTypeId);
					break;
				}
			}
		}
		//
		else if(CSystemSettingDlg::GetPropID("LayerTable.SimPolyLayer")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::SimPolyLayer.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::SimPolyLayer,LayerTable::SimPolyLayer.lineTypeId);
				    break;
				}
			}
		}
		//�ߴ��ע
		else if(CSystemSettingDlg::GetPropID("LayerTable.DimSizeLayer")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::DimSizeLayer.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::DimSizeLayer,LayerTable::DimSizeLayer.lineTypeId);
					break;
				}
			}
		}
		//���ֱ�ע
		else if(CSystemSettingDlg::GetPropID("LayerTable.DimTextLayer")==pItem->m_idProp)
		{
			for(TMADRAWING_LAYER* pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if(stricmp(pLayer->layerName,valueStr)==0)
				{
					LayerTable::DimTextLayer.CopyProperty(pLayer);
					ModifySpecialLayer(LayerTable::DimTextLayer,LayerTable::DimTextLayer.lineTypeId);
					break;
				}
			}
		}
		//
		else if(CSystemSettingDlg::GetPropID("Layer.lineTypeName")==pItem->m_idProp)
		{
			if(pItem->m_pParent)
			{
				TMADRAWING_LAYER *pLayer = (TMADRAWING_LAYER*)pItem->m_pParent->m_idProp;
				if(pLayer)
				{
					pLayer->lineTypeName.Printf("%s",valueStr);
					ModifySpecialLayer(*pLayer,pLayer->lineTypeId);
				}
			}
		}
		else if(CSystemSettingDlg::GetPropID("Layer.lineWeight")==pItem->m_idProp)
		{
			if(pItem->m_pParent)
			{
				TMADRAWING_LAYER* pLayer=(TMADRAWING_LAYER*)pItem->m_pParent->m_idProp;
				if(pLayer)
				{
					CXhChar16 sValue("%s",valueStr);
					sValue.Replace("mm","");
					int lineW=(int)(atof(valueStr)*100);
					pLayer->lineWeight=(AcDb::LineWeight)lineW;
					ModifySpecialLayer(*pLayer,pLayer->lineTypeId);
				}
			}
		}
		else if(CSystemSettingDlg::GetPropID("Layer.colorIndex")==pItem->m_idProp)
		{
			if(pItem->m_pParent)
			{
				TMADRAWING_LAYER *pLayer = (TMADRAWING_LAYER*)pItem->m_pParent->m_idProp;
				if(pLayer)
				{
					pLayer->colorIndex=GetNearestACI(crCurSel);
					ModifySpecialLayer(*pLayer,pLayer->lineTypeId);
				}
			}
			
		}
		else 
		{
			BOOL bUpdateLayerName=FALSE;
			for(TMADRAWING_LAYER *pLayer=LayerTable::layerHashList.GetFirst();pLayer;pLayer=LayerTable::layerHashList.GetNext())
			{
				if((long)pLayer==pItem->m_idProp)
				{
					pLayer->layerName.Printf("%s",valueStr);
					ModifySpecialLayer(*pLayer,pLayer->lineTypeId);
					bUpdateLayerName=TRUE;
					break;
				}
			}
			if(bUpdateLayerName)
				pDlg->UpdateSpecialLayerItem();
			else
			{	//�����ض���������
				CSystemSettingDlg::KEY2WORD key(pItem->m_idProp);
				if(key.idProp==CTextItemProp::GetPropID("PartNo"))							//��������ָ�
					sys.fonts.SetTextSize("PartNo",atof(valueStr),(BYTE)key.idGroup);
				else if(key.idProp==CTextItemProp::GetPropID("PartNoMargin"))				//�ֿ��϶
					sys.fonts.SetTextSize("PartNoMargin",atof(valueStr),(BYTE)key.idGroup);
				else if(key.idProp==CTextItemProp::GetPropID("DimText"))					//�����ָ�
					sys.fonts.SetTextSize("DimText",atof(valueStr),(BYTE)key.idGroup);
				else if(key.idProp==CTextItemProp::GetPropID("DimAngleText"))				//�Ƕ��ָ�
					sys.fonts.SetTextSize("DimAngleText",atof(valueStr),(BYTE)key.idGroup);
				else if(key.idProp==CTextItemProp::GetPropID("HeaderText"))					//��ͷ�ָ�
					sys.fonts.SetTextSize("HeaderText",atof(valueStr),(BYTE)key.idGroup);
				else if(key.idProp==CTextItemProp::GetPropID("CommonText"))					//�����ָ�
					sys.fonts.SetTextSize("CommonText",atof(valueStr),(BYTE)key.idGroup);
				else if(key.idProp==CTextItemProp::GetPropID("NumericText"))				//�����ָ�
					sys.fonts.SetTextSize("NumericText",atof(valueStr),(BYTE)key.idGroup);
				else if(key.idProp==CTextItemProp::GetPropID("CutawayViewSymbolText"))		//�ָ�
					sys.fonts.SetTextSize("CutawayViewSymbolText",atof(valueStr),(BYTE)key.idGroup);
				else if(key.idProp==CTextItemProp::GetPropID("CutawayViewSymbolText")+1000)	//����					
					sys.fonts.SetTextWtoH("CutawayViewSymbolText",atof(valueStr),(BYTE)key.idGroup);
				else if(key.idProp==CTextItemProp::GetPropID("SimPolyText"))				//������
					sys.fonts.SetTextSize("SimPolyText",atof(valueStr),(BYTE)key.idGroup);			
				else if(key.idProp==CTextItemProp::GetPropID("LsGuigeText"))				//��˨���
					sys.fonts.SetTextSize("LsGuigeText",atof(valueStr),(BYTE)key.idGroup);		
				else if(key.idProp==CTextItemProp::GetPropID("JgOddmentText"))				//�Ǹ�����ͷ
					sys.fonts.SetTextSize("JgOddmentText",atof(valueStr),(BYTE)key.idGroup);
				else if(key.idProp==CTextItemProp::GetPropID("PlateThickText"))				//�ְ���
					sys.fonts.SetTextSize("PlateThickText",atof(valueStr),(BYTE)key.idGroup);
				else if(key.idProp==CTextItemProp::GetPropID("PartGuigeText"))				//�������					
					sys.fonts.SetTextSize("PartGuigeText",atof(valueStr),(BYTE)key.idGroup);
				else if(key.idProp==CTextItemProp::GetPropID("OtherText"))					//���຺��
					sys.fonts.SetTextSize("OtherText",atof(valueStr),(BYTE)key.idGroup);
				else //δ�ҵ���Ӧ���޸����ModifyStruMapValue��ModifyPartGroupMapValue��ModifyPartMapValue��ָ������FALSE
					return FALSE;
			}
		}
		}
		InitDrawingEnv();	//��ʼ����ͼ���� wht 10-09-17
	}
	return TRUE;
}
#endif
