//<LOCALE_TRANSLATE/>
// SelectDrawingViewDlg.cpp : implementation file
//
#include "stdafx.h"
#include "stdarx.h"
#include "SelectDrawingDlg.h"
#include "Drawing.h"
#include "image.h"
#include "NewDrawingViewDlg.h"
#include "AddViewDlg.h"
#include "MenuFunc.h"
#include "DragEntSet.h"
#include "DrawMapScriptDlg.h"
#include "AdsTools.h"
#include "InputDrawingUcsDlg.h"
#include "ImportScriptOptionDlg.h"
#include "GlobalFunc.h"
#include "LogFile.h"
#include "PropertyListOper.h"
#include "InputPartNoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __SMART_DRAW_INC_
/////////////////////////////////////////////////////////////////////////////
// CSelectDrawingDlg dialog

CSelectDrawingDlg *g_pSelectDrawingDlg=NULL;
CSelectDrawingDlg::CSelectDrawingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectDrawingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectDrawingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pComponent=NULL;
	m_pDrawing = NULL;
	m_pItemInfo=NULL;
	m_bCanKeepFocus=TRUE;
	m_bModeless=FALSE;	//非模态
	m_bRestartDlg=FALSE;
}


void CSelectDrawingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectDrawingDlg)
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Control(pDX, IDC_TREE_DRAWING, m_treeDrawing);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectDrawingDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectDrawingDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DRAWING, OnSelchangedTreeDrawing)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_DRAWING, OnRclickTreeDrawing)
	ON_COMMAND(ID_IMPORT_SCRIPT,OnImportScript)
	ON_COMMAND(ID_EXPORT_SCRIPT,OnExportScript)
	ON_COMMAND(ID_NEW_ITEM,OnNewItem)
	ON_COMMAND(ID_DELETE_ITEM,OnDeleteItem)
	ON_COMMAND(ID_ERASE_RELA_CAD_ENT, OnEraseRelaCadEnt)
	ON_COMMAND(ID_MOVE_TOP,OnMoveTop)
	ON_COMMAND(ID_MOVE_UP,OnMoveUp)
	ON_COMMAND(ID_MOVE_DOWN,OnMoveDown)
	ON_COMMAND(ID_MOVE_BOTTOM,OnMoveBottom)
	ON_COMMAND(ID_ADD_ITEM,OnAddItem)
	ON_COMMAND(ID_ADD_PART_ITEM,OnAddPartItem)
	ON_COMMAND(ID_UPDATE_ITEM,OnUpdateItem)
	ON_COMMAND(ID_REBULID_ITEM,OnRebuildItem)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_DRAWING, OnDblclkTreeDrawing)
	ON_BN_CLICKED(IDC_BTN_SAVE_SCRIPT, OnSaveScript)
	ON_NOTIFY(NM_CLICK, IDC_TREE_DRAWING, OnClickTreeDrawing)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_DRAWING, OnKeydownTreeDrawing)
	ON_WM_LBUTTONDOWN()
	ON_WM_CAPTURECHANGED()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTN_DISPLAY_CMD_DLG, OnDisplayCmdDlg)
	ON_BN_CLICKED(ID_BTN_CREATE_DRAWING, OnCreateDrawing)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectDrawingDlg message handlers
CString MakeMapFrameSizeSetString();
static BOOL ModifyItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CSelectDrawingDlg::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
static CString MakePartHandleSetString(const char* sPartNo)
{
	CString handleStr;
	for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(stricmp(pPart->GetPartNo(),sPartNo)==0)
		{
			handleStr.Append(CXhChar50("0X%X",pPart->handle));
			handleStr.Append("|");
		}
	}
	return handleStr;
}
IMPLEMENT_PROP_FUNC(CSelectDrawingDlg)
const DWORD HASHTABLESIZE = 500;
const DWORD STATUSHASHTABLESIZE = 50;
void CSelectDrawingDlg::InitEnglishPropHashtable()
{
	int id = 1;
	//常规
	AddPropItem("drawing",PROPLIST_ITEM(id++,"Drawing properties"));
	AddPropItem("drawing.m_nSegI",PROPLIST_ITEM(id++,""));
	AddPropItem("drawing.m_xMapFrm.style",PROPLIST_ITEM(id++,"drawing frame spec","",MakeMapFrameSizeSetString()));
	AddPropItem("drawing.m_xMapFrm.nLength",PROPLIST_ITEM(id++,"drawing frame length"));
	AddPropItem("drawing.m_xMapFrm.nWidth",PROPLIST_ITEM(id++,"drawing frame width"));
	AddPropItem("drawing.m_xMapFrm.nLeftMargin",PROPLIST_ITEM(id++,"left side space","clearance value on the left margin."));
	AddPropItem("drawing.m_xMapFrm.nOtherMargin",PROPLIST_ITEM(id++,"Other sides space","clearance value on the top margin, bottom margin,and right margin."));
	AddPropItem("drawing.m_xMapFrm.nBottomRightFixLen",PROPLIST_ITEM(id++,"fixed length on lower right side","rectangular length on the lower right side for placing drawings information."));
	AddPropItem("drawing.m_xMapFrm.nBottomRightFixWidth",PROPLIST_ITEM(id++,"fixed height on lower right side"));
	AddPropItem("drawing.m_sDrawingName",PROPLIST_ITEM(id++,"drawing name"));
	AddPropItem("drawing.m_sDrawingNo",PROPLIST_ITEM(id++,"drawing No"));
	AddPropItem("drawing.m_bSpecifyMapFrmPos",PROPLIST_ITEM(id++,"Specify drawing frame's location","","Yes|No"));
	AddPropItem("drawing.m_hActiveModule",PROPLIST_ITEM(id++,"module activated currently"));
	AddPropItem("drawing.m_bAutoLayoutComponent",PROPLIST_ITEM(id++,"layout automatically","automatically finding proper location according to the size of view.","Yes|No"));
	AddPropItem("drawing.m_fComponentSpace",PROPLIST_ITEM(id++,"View space","clearance between View and drawing frame, between two adjacent views."));
	//
	AddPropItem("component",PROPLIST_ITEM(id++,"View Properties"));
	AddPropItem("component.m_fScale",PROPLIST_ITEM(id++,"drawing scale 1:","drawing scale."));
	AddPropItem("component.m_bDimScale",PROPLIST_ITEM(id++,"dimension draw ratio","","Yes|No"));
	AddPropItem("component.m_fNamePosSpace",PROPLIST_ITEM(id++,"Name dimension position space"));
	AddPropItem("component.m_sDrawingLayer",PROPLIST_ITEM(id++,"layer scope","Scope of view layer"));
	AddPropItem("component.m_hModule",PROPLIST_ITEM(id++,"module group","module group relative to the current view."));
	AddPropItem("component.m_bSpecifyComPos",PROPLIST_ITEM(id++,"view picking position","","Yes|No"));
	AddPropItem("component.m_bNeedAdjust",PROPLIST_ITEM(id++,""));
	AddPropItem("component.m_eClassType",PROPLIST_ITEM(id++,"Type"));
	AddPropItem("component.m_sName",PROPLIST_ITEM(id++,"name","View name, which is used to dimension the view on the drawing if needed."));
	AddPropItem("component.m_bDimName",PROPLIST_ITEM(id++,"dimension view name","","Yes|No"));
	AddPropItem("component.m_sSectionViewCode",PROPLIST_ITEM(id++,"cutaway view code","Display code when drawing cutaway view symbol , delete,create and move the cutaway wide symbole through the back button."));
	AddPropItem("component.SectionView.startPt",PROPLIST_ITEM(id++,"starting of cutaway view symbol"));
	AddPropItem("component.SectionView.startPt.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("component.SectionView.startPt.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("component.SectionView.endPt",PROPLIST_ITEM(id++,"ending of cutaway view symbol"));
	AddPropItem("component.SectionView.endPt.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("component.SectionView.endPt.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("component.SectionView.bUpturned",PROPLIST_ITEM(id++,"direction of cutaway view symbol","","Upwards|Downwards"));
	AddPropItem("component.SectionView.hAttachViewKey",PROPLIST_ITEM(id++,"cutaway view symbol dependent view"));
	AddPropItem("component.m_nAnchorType",PROPLIST_ITEM(id++,"Drag and drop datum point type","","0.Top left corner|1.Top right corner|2.Lower left quarter|3.Lower right corner|"));
	AddPropItem("component.m_ptLocation",PROPLIST_ITEM(id++,"location of positioning point"));
	AddPropItem("component.m_ptLocation.x",PROPLIST_ITEM(id++,"offset in X direction"));
	AddPropItem("component.m_ptLocation.y",PROPLIST_ITEM(id++,"offset in Y direction"));
	AddPropItem("component.m_ptLocation.z",PROPLIST_ITEM(id++,"offset in Z direction"));
	AddPropItem("component.dcs",PROPLIST_ITEM(id++,"Mapping coordinates"));
	//
	AddPropItem("drawingBOM.m_bAutoNewTable",PROPLIST_ITEM(id++,"paging automatically","","Yes|No"));
	AddPropItem("drawingBOM.m_bHasTableHead",PROPLIST_ITEM(id++,"headers on paging table","","Yes|No"));
	//简化规格数	
	AddPropItem("drawingBOM.nBriefGuigeN",PROPLIST_ITEM(id++,"Simplify spec number"));
	//力学分析模式下单线图属性
	AddPropItem("lineView.m_bDimNodeNo",PROPLIST_ITEM(id++,"dimension node No.","","Yes|No"));
	AddPropItem("lineView.m_bDimPoleGuige",PROPLIST_ITEM(id++,"dimension rod's specification","","Yes|No"));
	//
	AddPropItem("LineView::m_bUsePolyLineSumMap",PROPLIST_ITEM(id++,"line style","line style when drawing general layout.","Ordinary straight line|Polygon line"));
	AddPropItem("LineView::m_bUsePolyLineChiefMap",PROPLIST_ITEM(id++,"line style","line style when drawing force line map.","Ordinary straight line|Polygon line"));
	//结构图公共变量
	AddPropItem("StruView",PROPLIST_ITEM(id++,"structure map properties"));
	AddPropItem("StruView::m_bDimPartSpec",PROPLIST_ITEM(id++,"dimension parts' details","","Yes|No"));
	AddPropItem("StruView::m_bDimPoleOddment",PROPLIST_ITEM(id++,"dimension rod's oddment","","Yes|No"));
	AddPropItem("StruView::m_bDimFootNailSpace",PROPLIST_ITEM(id++,"dimension foot nail space","","Yes|No"));
	AddPropItem("StruView::m_bDimFillPlateSpace",PROPLIST_ITEM(id++,"dimension filler plate's space","","Yes|No"));
	AddPropItem("StruView::m_bDimLsSpace",PROPLIST_ITEM(id++,"dimension bolts' special space","","Yes|No"));
	AddPropItem("StruView::m_bDimJgZhunJu",PROPLIST_ITEM(id++,"dimension angle's special G","","Yes|No"));
	AddPropItem("StruView::m_bDimBoltInfo",PROPLIST_ITEM(id++,"dimension bolts' info","","Yes|No"));
	AddPropItem("StruView::m_bDimBoltPad",PROPLIST_ITEM(id++,"dimension bolt's washer","","Yes|No"));
	AddPropItem("StruView::m_bDimSimPoly",PROPLIST_ITEM(id++,"dimension similar polygon","","Yes|No"));
	AddPropItem("struView.partNo",PROPLIST_ITEM(id++,"dim partNo context"));
	AddPropItem("struView.partNo.m_iMirMsg",PROPLIST_ITEM(id++,"mirror type","mirror type of parts number dimension","0.X Axis Mirror|1.Y Axis Mirror|2.No Mirror|"));
	AddPropItem("struView.partNo.m_bDrawRefLine",PROPLIST_ITEM(id++,"draw indication line","whether to draw indication line while dimension parts details","Yes|No"));
	//零件图
	AddPropItem("PartView.sPartNo",PROPLIST_ITEM(id++,"PartNo","part no"));
	AddPropItem("PartView.hPart",PROPLIST_ITEM(id++,"Part Handle","part handle"));
}
void CSelectDrawingDlg::InitPropHashtable()
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	InitEnglishPropHashtable();
#else
	//常规
	AddPropItem("drawing",PROPLIST_ITEM(id++,"图纸属性"));
	AddPropItem("drawing.m_nSegI",PROPLIST_ITEM(id++,""));
	AddPropItem("drawing.m_xMapFrm.style",PROPLIST_ITEM(id++,"图框规格","",MakeMapFrameSizeSetString()));
	AddPropItem("drawing.m_xMapFrm.nLength",PROPLIST_ITEM(id++,"图框长度"));
	AddPropItem("drawing.m_xMapFrm.nWidth",PROPLIST_ITEM(id++,"图框宽度"));
	AddPropItem("drawing.m_xMapFrm.nLeftMargin",PROPLIST_ITEM(id++,"左侧间隙","左侧页边间隙值。"));
	AddPropItem("drawing.m_xMapFrm.nOtherMargin",PROPLIST_ITEM(id++,"其它侧间隙","上、下、右侧页边间隙值。"));
	AddPropItem("drawing.m_xMapFrm.nBottomRightFixLen",PROPLIST_ITEM(id++,"右下侧固定长度","右下侧用于放置图纸信息的矩形长度。"));
	AddPropItem("drawing.m_xMapFrm.nBottomRightFixWidth",PROPLIST_ITEM(id++,"右下侧固定高度","右下侧用于放置图纸信息的矩形高度,生成表格时可根据该高度自动判断是否需要分页显示。"));
	AddPropItem("drawing.m_sDrawingName",PROPLIST_ITEM(id++,"图纸名称"));
	AddPropItem("drawing.m_sDrawingNo",PROPLIST_ITEM(id++,"图纸编号"));
	AddPropItem("drawing.m_bSpecifyMapFrmPos",PROPLIST_ITEM(id++,"指定图框位置","指定图框位置","是|否"));
	AddPropItem("drawing.m_hActiveModule",PROPLIST_ITEM(id++,"当前激活呼高"));
	AddPropItem("drawing.m_bAutoLayoutComponent",PROPLIST_ITEM(id++,"自动布局","根据视图大小自动查找合适的摆放位置。","是|否"));
	AddPropItem("drawing.m_fComponentSpace",PROPLIST_ITEM(id++,"视图间隙","视图与图框之间以及相邻两视图间的间隙值。"));
	//
	AddPropItem("component",PROPLIST_ITEM(id++,"视图属性"));
	AddPropItem("component.m_fScale",PROPLIST_ITEM(id++,"绘图比例 1:","绘图比例。"));
	AddPropItem("component.m_bDimScale",PROPLIST_ITEM(id++,"标注绘制比例","","是|否"));
	AddPropItem("component.m_fNamePosSpace",PROPLIST_ITEM(id++,"名称标注位置间隙"));
	AddPropItem("component.m_sDrawingLayer",PROPLIST_ITEM(id++,"图层范围","视图图层范围"));
	AddPropItem("component.m_hModule",PROPLIST_ITEM(id++,"呼高分组","当前视图关联的呼高分组。"));
	AddPropItem("component.m_bSpecifyComPos",PROPLIST_ITEM(id++,"拾取视图位置","生成视图时通过拖放视图在图纸中拾取视图摆放位置，选择否时可输入视图相对于图框左上角的相对位置。","是|否"));
	AddPropItem("component.m_bNeedAdjust",PROPLIST_ITEM(id++,""));
	AddPropItem("component.m_eClassType",PROPLIST_ITEM(id++,"类型"));
	AddPropItem("component.m_sName",PROPLIST_ITEM(id++,"视图名称","视图名称，需要在图纸中标注视图名称时使用该名称进行标注。"));
	AddPropItem("component.m_bDimName",PROPLIST_ITEM(id++,"标注视图名称","","是|否"));
	AddPropItem("component.m_sSectionViewCode",PROPLIST_ITEM(id++,"剖视图代号","绘制剖视图符时显示该代号，可通过后侧按钮删除、生成或移动剖视图符号。"));
	AddPropItem("component.SectionView.startPt",PROPLIST_ITEM(id++,"剖视图符起点","可点击按钮拖放选择起点位置，起点位置变动后系统自动更新剖视图朝向。"));
	AddPropItem("component.SectionView.startPt.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("component.SectionView.startPt.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("component.SectionView.endPt",PROPLIST_ITEM(id++,"剖视图符终点","可点击按钮拖放选择终点位置，起点位置变动后系统自动更新剖视图朝向。"));
	AddPropItem("component.SectionView.endPt.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("component.SectionView.endPt.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("component.SectionView.bUpturned",PROPLIST_ITEM(id++,"剖视图符朝向","剖视图符始端->终端逆时针旋转90°方向为朝上，顺时针旋转90°方向为朝下。","朝上|朝下"));
	AddPropItem("component.SectionView.hAttachViewKey",PROPLIST_ITEM(id++,"剖视图符从属视图","剖视图符从属视图,即移动该视图时剖视图符号随之移动。"));
	AddPropItem("component.m_nAnchorType",PROPLIST_ITEM(id++,"拖放基准点类型","视图生成后，进行拖放时拖放基准点的类型。","0.左上角|1.右上角|2.左下角|3.右下角|"));
	AddPropItem("component.m_ptLocation",PROPLIST_ITEM(id++,"定位点相对位置"));
	AddPropItem("component.m_ptLocation.x",PROPLIST_ITEM(id++,"X方向偏移值"));
	AddPropItem("component.m_ptLocation.y",PROPLIST_ITEM(id++,"Y方向偏移值"));
	AddPropItem("component.m_ptLocation.z",PROPLIST_ITEM(id++,"Z方向偏移值"));
	AddPropItem("component.dcs",PROPLIST_ITEM(id++,"绘图坐标系"));
	//
	AddPropItem("drawingBOM.m_bAutoNewTable",PROPLIST_ITEM(id++,"自动分页","表格行数过多时系统可根据边框高度以及边框右下角矩形高度自动实现分页绘制。","是|否"));
	AddPropItem("drawingBOM.m_bHasTableHead",PROPLIST_ITEM(id++,"分页表格带表头","表格分页后新表格是否绘制表头。","是|否"));
	//简化规格数	
	AddPropItem("drawingBOM.nBriefGuigeN",PROPLIST_ITEM(id++,"简化规格数","简化规格数，使用构件规格表来简化标注时，该变量控制构件规格表中简化规格的个数。最大个数为26"));
	//力学分析模式下单线图属性
	AddPropItem("lineView.m_bDimNodeNo",PROPLIST_ITEM(id++,"标注节点编号","","是|否"));
	AddPropItem("lineView.m_bDimPoleGuige",PROPLIST_ITEM(id++,"标注杆件规格","","是|否"));
	//
	AddPropItem("LineView::m_bUsePolyLineSumMap",PROPLIST_ITEM(id++,"线型","绘制总图时的线型。","普通直线|多段线"));
	AddPropItem("LineView::m_bUsePolyLineChiefMap",PROPLIST_ITEM(id++,"线型","绘制司令图时的线型。","普通直线|多段线"));
	//结构图公共变量
	AddPropItem("StruView",PROPLIST_ITEM(id++,"结构图属性"));
	AddPropItem("StruView::m_bDimPartSpec",PROPLIST_ITEM(id++,"标注构件明细","","是|否"));
	AddPropItem("StruView::m_bDimPoleOddment",PROPLIST_ITEM(id++,"标注杆件正负头","","是|否"));
	AddPropItem("StruView::m_bDimFootNailSpace",PROPLIST_ITEM(id++,"标注脚钉间距","","是|否"));
	AddPropItem("StruView::m_bDimFillPlateSpace",PROPLIST_ITEM(id++,"标注填板间距","","是|否"));
	AddPropItem("StruView::m_bDimLsSpace",PROPLIST_ITEM(id++,"标注螺栓特殊间距","","是|否"));
	AddPropItem("StruView::m_bDimJgZhunJu",PROPLIST_ITEM(id++,"标注角钢特殊准据","","是|否"));
	AddPropItem("StruView::m_bDimBoltInfo",PROPLIST_ITEM(id++,"标注螺栓信息","","是|否"));
	AddPropItem("StruView::m_bDimBoltPad",PROPLIST_ITEM(id++,"标注螺栓垫圈","","是|否"));
	AddPropItem("StruView::m_bDimSimPoly",PROPLIST_ITEM(id++,"标注相似形","","是|否"));
	AddPropItem("struView.partNo",PROPLIST_ITEM(id++,"标注构件明细"));
	AddPropItem("struView.partNo.m_iMirMsg",PROPLIST_ITEM(id++,"对称方式","构件编号标注时的对称方式。","0.X轴对称|1.Y轴对称|2.无对称|"));
	AddPropItem("struView.partNo.m_bDrawRefLine",PROPLIST_ITEM(id++,"绘制引出线","标注构件明细时是否绘制引出线。","是|否"));
	//零件图
	AddPropItem("PartView.sPartNo",PROPLIST_ITEM(id++,"构件编号","零件的构件编号"));
	AddPropItem("PartView.hPart",PROPLIST_ITEM(id++,"构件句柄","零件的构件句柄"));
#endif
}

//From Drawing.cpp
void InitDrawingList();			//初始化图纸列表
void UpdateStruDrawingList();	//更新结构图对应的图纸列表
BOOL CSelectDrawingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	/*CFont *pFont=GetFont();
	LOGFONT logFont;
	pFont->GetLogFont(&logFont);
	logFont.lfHeight*=2;
	logFont.lfWidth*=2;
	logFont.lfWeight*=2;
	m_xFont.CreateFontIndirect(&logFont);
	GetDlgItem(IDC_BTN_DISPLAY_CMD_DLG)->SetFont(&m_xFont);*/
	//
	m_Images.Create(IDB_IL_PROJECT, 16, 1, RGB(0,255,0));
	m_treeDrawing.SetImageList(&m_Images,TVSIL_NORMAL);	
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	if(m_pDrawing==NULL||m_pItemInfo==NULL)
	{	
		//初始化属性ID,初始化属性hastable应放在RefreshDrawingTree()之前，	
		//m_treeDrawing.SelectItem()会触发OnSelChange进而调用DisplayDrawingProperty() wht 11-06-17
		InitPropHashtable();	
		//初始化图纸列表
		InitDrawingList();
		//默认选中总图
		drawingList.GetFirst();
		m_pDrawing=drawingList.GetNext();
		RefreshDrawingTree(m_pDrawing,NULL);
		//显示系统设置
		DisplayDrawingProperty();
	}
	else 
	{	//刷新界面
		if(drawingList.GetNodeNum()>0)
		{	//防止关闭文件后再次运行执行绘图命令时出错 wht 11-07-27
			if(m_pDrawing)
				m_pDrawing->feature=1;		//设置标志位，初始化时不需要缩放窗口
			if(m_pComponent)
				m_pComponent->feature=1;	//设置标志位，初始化时不需要缩放窗口
			RefreshDrawingTree(m_pDrawing,m_pComponent);
			m_propList.Redraw();
		}
		else 
			RefreshDrawingTree(NULL,NULL);
	}
	SetTimer(1,300,NULL);//Set Timer
	//移动窗口到右上角
	CSize sizeMaxScr;
	sizeMaxScr.cx = GetSystemMetrics(SM_CXSCREEN);
	sizeMaxScr.cy = GetSystemMetrics(SM_CYSCREEN);
	CRect rect;
	GetWindowRect(rect);
	rect.left = 0;//sizeMaxScr.cx-rect.Width();
	rect.right=rect.Width();//sizeMaxScr.cx;
	rect.bottom = rect.Height();
	rect.top  =0;
	MoveWindow(rect,TRUE);
	return TRUE; 
}

void CSelectDrawingDlg::OnSelchangedTreeDrawing(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hSelectedItem = m_treeDrawing.GetSelectedItem();
	m_pItemInfo=(TREEITEM_INFO*)m_treeDrawing.GetItemData(hSelectedItem);
	m_pDrawing=NULL;
	m_pComponent=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	CString sCreateButtonName="creat";	//Adjust name of creat button according to the currently selected item wht 11-07-15
#else
	CString sCreateButtonName="生成";	//根据当前选中项调整生成按钮名称 wht 11-07-15
#endif
	if(m_pItemInfo&&m_pItemInfo->dwRefData&&m_pItemInfo->itemType==1)
	{
		m_pDrawing=(CDrawing*)m_pItemInfo->dwRefData;
		if(m_pDrawing->feature==1)
			m_pDrawing->feature=0;
		else 
		{	//选中一张图纸后自动显示图纸所有内容
			f2dRect rect;
			rect.topLeft.Set(m_pDrawing->dcs.origin.x,m_pDrawing->dcs.origin.y+m_pDrawing->m_xMapFrm.nWidth);
			rect.bottomRight.Set(m_pDrawing->dcs.origin.x+m_pDrawing->m_xMapFrm.nLength,m_pDrawing->dcs.origin.y);
			ZoomAcadView(rect,50);
		}
		if(m_pDrawing->m_nSegI.iSeg>=0)
#ifdef AFX_TARG_ENU_ENGLISH
			sCreateButtonName.Format("create 【%s structure map】",(char*)m_pDrawing->m_sDrawingName);
		else 
			sCreateButtonName.Format("create 【%s】",(char*)m_pDrawing->m_sDrawingName);
#else
			sCreateButtonName.Format("生成【%s结构图】",(char*)m_pDrawing->m_sDrawingName);
		else 
			sCreateButtonName.Format("生成【%s】",(char*)m_pDrawing->m_sDrawingName);
#endif
	}
	if(m_pItemInfo&&m_pItemInfo->dwRefData&&m_pItemInfo->itemType==2)
	{
		m_pComponent=(CDrawingComponent*)m_pItemInfo->dwRefData;
		m_pDrawing=m_pComponent->ParentDrawing();
		if(m_pComponent->feature==1)
		{
			if(m_pDrawing)
				m_pDrawing->feature=0;
			m_pComponent->feature=0;
		}
		else 
		{	//选中一个组件后自动将所选组件开窗放大
			f2dRect rect=m_pComponent->GetDrawRegion(TRUE);
			ZoomAcadView(rect,50);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		sCreateButtonName.Format("create 【%s】",(char*)m_pComponent->m_sName);
#else
		sCreateButtonName.Format("生成【%s】",(char*)m_pComponent->m_sName);
#endif
	}
	SetDlgItemText(ID_BTN_CREATE_DRAWING,sCreateButtonName);	//设置生成按钮名称
	DisplayDrawingProperty();
	*pResult = 0;
}

void CSelectDrawingDlg::OnRclickTreeDrawing(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TVHITTESTINFO HitTestInfo;
	GetCursorPos(&HitTestInfo.pt);
	m_treeDrawing.ScreenToClient(&HitTestInfo.pt);
	if(m_treeDrawing.HitTest(&HitTestInfo)==NULL)
		return;
	m_treeDrawing.Select(HitTestInfo.hItem,TVGN_CARET);
	m_treeDrawing.Expand(HitTestInfo.hItem,TVE_EXPAND);
	CMenu popMenu;
	popMenu.LoadMenu(IDR_MENU_SCRIPT);
	CMenu *pMenu=popMenu.GetSubMenu(3);
	while(pMenu->GetMenuItemCount()>0)
		pMenu->DeleteMenu(0,MF_BYPOSITION); 
	if(!m_pItemInfo)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"new");
		//pMenu->AppendMenu(MF_STRING,ID_EXPORT_SCRIPT,"export script");
		//pMenu->AppendMenu(MF_STRING,ID_IMPORT_SCRIPT,"Import Script ");
		pMenu->AppendMenu(MF_STRING,ID_UPDATE_ITEM,"Update list of charts");
		pMenu->AppendMenu(MF_SEPARATOR);
		pMenu->AppendMenu(MF_STRING,ID_REBULID_ITEM,"Reset list of charts");
#else
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"新建");
		//pMenu->AppendMenu(MF_STRING,ID_EXPORT_SCRIPT,"导出脚本");
		//pMenu->AppendMenu(MF_STRING,ID_IMPORT_SCRIPT,"导入脚本");
		pMenu->AppendMenu(MF_STRING,ID_UPDATE_ITEM,"更新图纸列表");
		pMenu->AppendMenu(MF_SEPARATOR);
		pMenu->AppendMenu(MF_STRING,ID_REBULID_ITEM,"重置图纸列表");
#endif
	}
	else if(m_pItemInfo->itemType==1)
	{	//图纸
		//pMenu->AppendMenu(MF_STRING,ID_MOVE_TOP,"置顶");
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_MOVE_UP,"shift up");
		pMenu->AppendMenu(MF_STRING,ID_MOVE_DOWN,"shift down");
#else
		pMenu->AppendMenu(MF_STRING,ID_MOVE_UP,"上移");
		pMenu->AppendMenu(MF_STRING,ID_MOVE_DOWN,"下移");
#endif
		//pMenu->AppendMenu(MF_STRING,ID_MOVE_BOTTOM,"置尾");
		pMenu->AppendMenu(MF_SEPARATOR);
		if(m_pDrawing)
		{	//当前选中项为可用图纸,选择【结构图】项时不需要 wht 11-07-15
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_ADD_ITEM,"add");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"delete");
			pMenu->AppendMenu(MF_STRING,ID_ERASE_RELA_CAD_ENT,"delete CAD entity");
			pMenu->AppendMenu(MF_STRING,ID_ADD_PART_ITEM,"add part view");
			pMenu->AppendMenu(MF_STRING,ID_EXPORT_SCRIPT,"export script");
			pMenu->AppendMenu(MF_STRING,ID_IMPORT_SCRIPT,"Import Script");
		}
		pMenu->AppendMenu(MF_STRING,ID_UPDATE_ITEM,"Update Drawings");
		pMenu->AppendMenu(MF_SEPARATOR);
		pMenu->AppendMenu(MF_STRING,ID_REBULID_ITEM,"Recreate");
#else
			pMenu->AppendMenu(MF_STRING,ID_ADD_ITEM,"添加");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除");
			pMenu->AppendMenu(MF_STRING,ID_ADD_PART_ITEM,"添加零件图");
			pMenu->AppendMenu(MF_STRING,ID_ERASE_RELA_CAD_ENT,"删除CAD实体");
			pMenu->AppendMenu(MF_STRING,ID_EXPORT_SCRIPT,"导出脚本");
			pMenu->AppendMenu(MF_STRING,ID_IMPORT_SCRIPT,"导入脚本");
		}
		pMenu->AppendMenu(MF_STRING,ID_UPDATE_ITEM,"更新图纸");
		pMenu->AppendMenu(MF_SEPARATOR);
		pMenu->AppendMenu(MF_STRING,ID_REBULID_ITEM,"重新生成");
#endif
	}
	else
	{
		if(m_pItemInfo->itemType==2||m_pItemInfo->itemType==3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_MOVE_TOP,"Top");
			pMenu->AppendMenu(MF_STRING,ID_MOVE_UP,"Shift up");
			pMenu->AppendMenu(MF_STRING,ID_MOVE_DOWN,"Shift down");
			pMenu->AppendMenu(MF_STRING,ID_MOVE_BOTTOM,"Bottom");
#else
			pMenu->AppendMenu(MF_STRING,ID_MOVE_TOP,"置顶");
			pMenu->AppendMenu(MF_STRING,ID_MOVE_UP,"上移");
			pMenu->AppendMenu(MF_STRING,ID_MOVE_DOWN,"下移");
			pMenu->AppendMenu(MF_STRING,ID_MOVE_BOTTOM,"置尾");
#endif
			pMenu->AppendMenu(MF_SEPARATOR);
		}
		if(m_pItemInfo->itemType==2||m_pItemInfo->itemType==3)	
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_UPDATE_ITEM,"update");
			pMenu->AppendMenu(MF_SEPARATOR);
			pMenu->AppendMenu(MF_STRING,ID_REBULID_ITEM,"recreat");	//recreat specified  view command list wht 11-06-16
		}
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"delete");
		pMenu->AppendMenu(MF_STRING,ID_ERASE_RELA_CAD_ENT,"delete CAD entity");
#else
			pMenu->AppendMenu(MF_STRING,ID_UPDATE_ITEM,"更新");
			pMenu->AppendMenu(MF_SEPARATOR);
			pMenu->AppendMenu(MF_STRING,ID_REBULID_ITEM,"重新生成");	//重新生成指定视图命令列表 wht 11-06-16
			pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"重新生成螺栓标注命令");
		}
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除");
		pMenu->AppendMenu(MF_STRING,ID_ERASE_RELA_CAD_ENT,"删除CAD实体");
#endif
	}
	CPoint menu_pos;
	GetCursorPos(&menu_pos);
	popMenu.GetSubMenu(3)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,this);
	*pResult = 0;
}

void CSelectDrawingDlg::DeleteItemInfo(TREEITEM_INFO *pItemInfo)
{
	if (pItemInfo==NULL)
		return;
	if(pItemInfo->itemType==1)
	{
		CDrawing* pDrawing=(CDrawing*)pItemInfo->dwRefData;
		if (pDrawing==NULL)
			return;
		for(CDrawing* pDrawingInlist = drawingList.GetFirst();pDrawingInlist;
			pDrawingInlist = drawingList.GetNext())
		{
			if(pDrawingInlist==pDrawing)
			{
				DeleteItemInfo(pDrawing);	//删除pDrawing及其子项关联的itemInfo wht 11-07-15
				drawingList.DeleteCursor();
				break;
			}
		}
	}
	else if(pItemInfo->itemType==2)
	{
		CDrawingComponent* pComponent=(CDrawingComponent*)pItemInfo->dwRefData;
		if (pComponent==NULL)
			return;
		CDrawing* pDrawing=pComponent->ParentDrawing();
		if(pDrawing==NULL)
			return;
		CDrawingComponent* pCom=NULL;
		for(pCom=pDrawing->GetFirstDrawingComponent();pCom;pCom=pDrawing->GetNextDrawingComponent())
		{
			if (pCom==pComponent)
			{
				DeleteItemInfo(pCom);	//删除pComponent关联的itemInfo
				pDrawing->listComponents.DeleteCursor();
				break;
			}
		}
	}
}
void CSelectDrawingDlg::OnDeleteItem()
{
	if(m_pItemInfo==NULL)
		return;
	if (m_pItemInfo->itemType==1||m_pItemInfo->itemType==2)
		DeleteItemInfo(m_pItemInfo);
	else if (m_pItemInfo->itemType==3)
	{
		HTREEITEM hChildItem=m_treeDrawing.GetChildItem(m_treeDrawing.GetSelectedItem());
		while(hChildItem)
		{
			HTREEITEM hCurItem=hChildItem;
			TREEITEM_INFO *pChildItemInfo=(TREEITEM_INFO*)m_treeDrawing.GetItemData(hCurItem);
			DeleteItemInfo(pChildItemInfo);
			hChildItem=m_treeDrawing.GetNextItem(hChildItem,TVGN_NEXT);
			m_treeDrawing.DeleteItem(hCurItem);
		}
	}
	m_treeDrawing.DeleteItem(m_treeDrawing.GetSelectedItem());
}
//删除当前项关联的CAD实体。
static void EraseReLaCadEnt(TREEITEM_INFO *pItemInfo)
{
	if (pItemInfo==NULL)
		return;
	if(pItemInfo->itemType==1)
	{
		CDrawing* pDrawing=(CDrawing*)pItemInfo->dwRefData;
		if (pDrawing==NULL)
			return;
		pDrawing->EraseRelaCadEnt();
	}
	else if(pItemInfo->itemType==2)
	{
		CDrawingComponent* pComponent=(CDrawingComponent*)pItemInfo->dwRefData;
		if (pComponent==NULL)
			return;
		pComponent->EraseRelaCadEnt();
	}
}

void CSelectDrawingDlg::OnEraseRelaCadEnt()
{
	if(m_pItemInfo==NULL)
		return;
	if (m_pItemInfo->itemType==1||m_pItemInfo->itemType==2)
		EraseReLaCadEnt(m_pItemInfo);
	else if (m_pItemInfo->itemType==3)
	{
		HTREEITEM hChildItem=m_treeDrawing.GetChildItem(m_treeDrawing.GetSelectedItem());
		while(hChildItem)
		{
			HTREEITEM hCurItem=hChildItem;
			TREEITEM_INFO *pChildItemInfo=(TREEITEM_INFO*)m_treeDrawing.GetItemData(hCurItem);
			EraseReLaCadEnt(pChildItemInfo);
			hChildItem=m_treeDrawing.GetNextItem(hChildItem,TVGN_NEXT);
		}
	}
}

void CSelectDrawingDlg::OnNewItem()
{
	if(m_pItemInfo&&(m_pItemInfo->itemType==2||m_pItemInfo->itemType==3))
	{
		CDrawingComponent *pComponent=(CDrawingComponent*)m_pItemInfo->dwRefData;
		if(pComponent==NULL||pComponent->m_eClassType!=DRAW_COM_STRUVIEW)
			return;
		CStruView *pStruView=(CStruView*)pComponent;
		pStruView->ReDimBoltInfo();
		return;
	}
	CNewDrawingViewDlg dlg;
	dlg.DoModal();
	RefreshDrawingTree();
}

void CSelectDrawingDlg::OnAddItem()
{
	HTREEITEM hItem = m_treeDrawing.GetSelectedItem();
	if(m_pItemInfo==NULL||m_pItemInfo->itemType!=1)
		return;
	CDrawing *pDrawing = (CDrawing *)m_pItemInfo->dwRefData;
	CAddViewDlg dlg;
	dlg.m_pDrawing = pDrawing;
	if(dlg.DoModal()==IDOK)
		RefreshDrawingTree(pDrawing);
}

void CSelectDrawingDlg::OnAddPartItem()
{
	HTREEITEM hItem = m_treeDrawing.GetSelectedItem();
	if(m_pItemInfo==NULL||m_pItemInfo->itemType!=1)
		return;
	CDrawing *pDrawing = (CDrawing*)m_pItemInfo->dwRefData;
	if(pDrawing==NULL)
		return;
	//根据用户输入件号，提取构件
	CInputPartNoDlg dlg;
	if(dlg.DoModal()!=IDOK)
		return;
	CLDSPart* pPart=(CLDSPart*)Ta.FromPartNo(dlg.m_sPartNo);
	if(pPart==NULL || pDrawing->listComponents.GetValue(pPart->handle))
		return;;	//已存在此构件视图
	int nStyle=0;
	CSuperSmartPtr<CLDSPart> pSmartPart=pPart;
	if(pSmartPart->GetClassTypeId()==CLS_LINEANGLE&&
		(pSmartPart.LineAnglePointer()->pStart==NULL||pSmartPart.LineAnglePointer()->pEnd==NULL))
		nStyle=1;	//短角钢
	else if(pSmartPart->IsPlate()&&pSmartPart.pBasicPlate->wiType==CLDSGeneralPlate::WASHER_PLATE)
		nStyle=2;	//2.螺栓垫板
	else if(pSmartPart->IsPlate()&&pSmartPart.pBasicPlate->wiType==CLDSGeneralPlate::WRAPPER_PLATE)
		nStyle=3;	//3.接头包钢板
	else 
		return;
	//添加构件图
	CPartDrawing *pPartDrawing=(CPartDrawing*)pDrawing->listComponents.Add(pPart->handle,DRAW_COM_PARTVIEW);
	if(nStyle==1||nStyle==3)
		pPartDrawing->m_fScale=0.1;	//短角钢及接头包钢板绘图比例默认为1::10
	else if(nStyle==2)
		pPartDrawing->m_fScale=0.2;	//螺栓垫板默认绘图比例为1:5
	pPartDrawing->m_hPart=pPart->handle;
	pPartDrawing->sPartNo.Copy(pPart->GetPartNo());
	pPartDrawing->m_pPart=pPart;
	char sGuige[100]="";
	pPart->GetSpecification(sGuige,TRUE);
	pPartDrawing->m_sName.Printf("%s %s",(char*)pPart->GetPartNo(),sGuige);
	pPartDrawing->m_bNeedAdjust=FALSE;
	pPartDrawing->CreateCmdList();
	//更新树节点
	RefreshDrawingTree(pDrawing);
}
//0.置顶 1.上移 2.下移 3.置尾
void CSelectDrawingDlg::MoveItem(int iType)
{
	HTREEITEM hCurItem=m_treeDrawing.GetSelectedItem();
	if(hCurItem==NULL)
		return;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_treeDrawing.GetItemData(hCurItem);
	if(pItemInfo==NULL)
		return;
	bool bMove=false;
	CDrawing *pDrawing=NULL;
	CDrawingComponent *pComponent=NULL;
	if(pItemInfo->itemType==1)
	{
		pDrawing=(CDrawing*)pItemInfo->dwRefData;
		if(pDrawing==NULL)
			return;
		bMove=drawingList.Move(pDrawing,iType);
	}
	else if(pItemInfo->itemType==2)
	{
		pComponent=(CDrawingComponent*)pItemInfo->dwRefData;
		if(pComponent==NULL)
			return;
		bMove=m_pDrawing->listComponents.Move(pComponent,iType);
	}
	if(bMove)
		RefreshDrawingTree(m_pDrawing,pComponent);
}
void CSelectDrawingDlg::OnMoveTop()
{
	MoveItem(0);
}
void CSelectDrawingDlg::OnMoveUp()
{
	MoveItem(1);
}
void CSelectDrawingDlg::OnMoveDown()
{
	MoveItem(2);
}
void CSelectDrawingDlg::OnMoveBottom()
{
	MoveItem(3);
}

//更新选中项
void CSelectDrawingDlg::UpdateItem()
{	//
	HTREEITEM hDrawingItem=m_treeDrawing.GetSelectedItem();
	if(hDrawingItem==NULL)
		return;
	if(m_pItemInfo==NULL)
		m_pItemInfo=(TREEITEM_INFO*)m_treeDrawing.GetItemData(hDrawingItem);
	if(m_pItemInfo==NULL)
		return;
	if(m_pItemInfo->itemType==1)
	{	//图纸
		CString sItemName=m_treeDrawing.GetItemText(hDrawingItem);
		CDrawing *pDrawing=(CDrawing*)m_pItemInfo->dwRefData;
		if(pDrawing)
		{
			//1.删除当前图元关联的CAD实体数据
			pDrawing->EraseRelaCadEnt();
			//2.更新命令列表
			pDrawing->UpdateCmdList();
			//3.刷新界面显示
			HTREEITEM hChildItem=m_treeDrawing.GetChildItem(hDrawingItem);
			while(hChildItem)
			{
				HTREEITEM hCurItem=hChildItem;
				TREEITEM_INFO *pItem=NULL;
				TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_treeDrawing.GetItemData(hCurItem);
				for(pItem=itemInfoList.GetFirst();pItem;pItem=itemInfoList.GetNext())
				{
					if(pItem==pItemInfo)
					{
						itemInfoList.DeleteCursor(TRUE);
						break;
					}
				}
				hChildItem=m_treeDrawing.GetNextItem(hChildItem,TVGN_NEXT);
				m_treeDrawing.DeleteItem(hCurItem);
			}
			//
			CDrawingComponent *pComponent=NULL;
			for(pComponent=pDrawing->GetFirstDrawingComponent();pComponent;pComponent=pDrawing->GetNextDrawingComponent())
			{
				HTREEITEM hItem=m_treeDrawing.InsertItem(pComponent->m_sName,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hDrawingItem);
				TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(2,(DWORD)pComponent));
				m_treeDrawing.SetItemData(hItem,(DWORD)pItemInfo);
			}
			//4.重新生成
			pDrawing->CreateComponents();
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(sItemName.CompareNoCase("structure map")==0)
#else
		else if(sItemName.CompareNoCase("结构图")==0)
#endif
		{	//结构图目录项,点击重新生成时重新生成所有结构图 wht 11-07-15
			HTREEITEM hStruItem=hDrawingItem;
			//删除所有分段结构图相关的itemInfo
			TREEITEM_INFO *pItem=NULL;
			for(pItem=itemInfoList.GetFirst();pItem;pItem=itemInfoList.GetNext())
			{
				if(pItem->itemType==1)
				{	//删除分段结构图图纸对应的itemInfo
					pDrawing=(CDrawing*)pItem->dwRefData;
					if(pDrawing&&pDrawing->m_nSegI.iSeg>=0)
						itemInfoList.DeleteCursor(TRUE);
				}
				else if(pItem->itemType==2)
				{	//删除结构图中各个图元对应的ItemInfo
					CDrawingComponent *pCom=(CDrawingComponent*)pItem->dwRefData;
					if(pCom&&pCom->ParentDrawing()&&pCom->ParentDrawing()->m_nSegI.iSeg>=0)
						itemInfoList.DeleteCursor(TRUE);
				}
			}
			//删除图纸列表树种所有结构图对应的节点
			HTREEITEM hChildItem=m_treeDrawing.GetChildItem(hStruItem);
			while(hChildItem)
			{
				HTREEITEM hCurItem=hChildItem;
				hChildItem=m_treeDrawing.GetNextItem(hChildItem,TVGN_NEXT);
				m_treeDrawing.DeleteItem(hCurItem);
			}
			//更新分段结构图图纸列表
			UpdateStruDrawingList();
			//刷新界面
			for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
			{
				if(pDrawing->m_nSegI.iSeg<0)
					continue;
				//在图纸列表中添加对应项
				HTREEITEM hSegItem=m_treeDrawing.InsertItem(pDrawing->m_sDrawingName,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hStruItem);
				TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(1,(DWORD)pDrawing));
				m_treeDrawing.SetItemData(hSegItem,(DWORD)pItemInfo);
				m_treeDrawing.Expand(hSegItem,TVE_COLLAPSE);
				CDrawingComponent *pComponent=NULL;
				for(pComponent=pDrawing->GetFirstDrawingComponent();pComponent;pComponent=pDrawing->GetNextDrawingComponent())
				{
					HTREEITEM hItem=m_treeDrawing.InsertItem(pComponent->m_sName,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hSegItem);
					pItemInfo=itemInfoList.append(TREEITEM_INFO(2,(DWORD)pComponent));
					m_treeDrawing.SetItemData(hItem,(DWORD)pItemInfo);
				}
			}
			m_treeDrawing.Expand(hStruItem,TVE_EXPAND);
		}
	}
	else if(m_pItemInfo->itemType==2)
	{	//更新该视图相关命令 
		CDrawingComponent *pComponent=(CDrawingComponent*)m_pItemInfo->dwRefData;
		if(pComponent)	//更新命令列表
		{
			//1.删除当前图元关联的CAD实体数据
			pComponent->EraseRelaCadEnt();
			//2.更新命令列表
			pComponent->UpdateCmdList(TRUE);
			//3.重新生成当前图元
			pComponent->ReCreate();
		}
	}
}

void CSelectDrawingDlg::RebulidItem()
{	//
	HTREEITEM hDrawingItem=m_treeDrawing.GetSelectedItem();
	if(hDrawingItem==NULL)
		return;
	if(m_pItemInfo==NULL)
		m_pItemInfo=(TREEITEM_INFO*)m_treeDrawing.GetItemData(hDrawingItem);
	if(m_pItemInfo==NULL)
		return;
	CLogErrorLife life;
	if(m_pItemInfo->itemType==1)
	{	//图纸
		CString sItemName=m_treeDrawing.GetItemText(hDrawingItem);
		CDrawing *pDrawing=(CDrawing*)m_pItemInfo->dwRefData;
		if(pDrawing)
		{
			pDrawing->InitDrawing(pDrawing->m_nSegI);
			//刷新界面显示
			HTREEITEM hChildItem=m_treeDrawing.GetChildItem(hDrawingItem);
			while(hChildItem)
			{
				HTREEITEM hCurItem=hChildItem;
				TREEITEM_INFO *pItem=NULL;
				TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_treeDrawing.GetItemData(hCurItem);
				for(pItem=itemInfoList.GetFirst();pItem;pItem=itemInfoList.GetNext())
				{
					if(pItem==pItemInfo)
					{
						itemInfoList.DeleteCursor(TRUE);
						break;
					}
				}
				hChildItem=m_treeDrawing.GetNextItem(hChildItem,TVGN_NEXT);
				m_treeDrawing.DeleteItem(hCurItem);
			}
			//
			CDrawingComponent *pComponent=NULL;
			for(pComponent=pDrawing->GetFirstDrawingComponent();pComponent;pComponent=pDrawing->GetNextDrawingComponent())
			{
				HTREEITEM hItem=m_treeDrawing.InsertItem(pComponent->m_sName,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hDrawingItem);
				TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(2,(DWORD)pComponent));
				m_treeDrawing.SetItemData(hItem,(DWORD)pItemInfo);
			}
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(sItemName.CompareNoCase("structure map")==0)
#else
		else if(sItemName.CompareNoCase("结构图")==0)
#endif
		{	//结构图目录项,点击重新生成时重新生成所有结构图 wht 11-07-15
			HTREEITEM hStruItem=hDrawingItem;
			//删除所有分段结构图相关的itemInfo
			TREEITEM_INFO *pItem=NULL;
			for(pItem=itemInfoList.GetFirst();pItem;pItem=itemInfoList.GetNext())
			{
				if(pItem->itemType==1)
				{	//删除分段结构图图纸对应的itemInfo
					pDrawing=(CDrawing*)pItem->dwRefData;
					if(pDrawing&&pDrawing->m_nSegI>=0)
						itemInfoList.DeleteCursor(TRUE);
				}
				else if(pItem->itemType==2)
				{	//删除结构图中各个图元对应的ItemInfo
					CDrawingComponent *pCom=(CDrawingComponent*)pItem->dwRefData;
					if(pCom&&pCom->ParentDrawing()&&pCom->ParentDrawing()->m_nSegI.iSeg>=0)
						itemInfoList.DeleteCursor(TRUE);
				}
			}
			//删除原有分段结构图
			for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
			{
				if(pDrawing->m_nSegI.iSeg>=0)
					drawingList.DeleteCursor();
			}
			//删除图纸列表树种所有结构图对应的节点
			HTREEITEM hChildItem=m_treeDrawing.GetChildItem(hStruItem);
			while(hChildItem)
			{
				HTREEITEM hCurItem=hChildItem;
				hChildItem=m_treeDrawing.GetNextItem(hChildItem,TVGN_NEXT);
				m_treeDrawing.DeleteItem(hCurItem);
			}
			//重新初始化分段结构图
			ATOM_LIST<SEGI> segNoList;
			Ta.GetSegNoList(segNoList);
			for(SEGI *pSegI=segNoList.GetFirst();pSegI;pSegI=segNoList.GetNext())
			{	
				pDrawing=drawingList.Add(0);
				pDrawing->InitDrawing(*pSegI);
#ifdef AFX_TARG_ENU_ENGLISH
				pDrawing->m_sDrawingName.Printf("%s segment",(char*)pSegI->ToString());
#else
				pDrawing->m_sDrawingName.Printf("%s段",(char*)pSegI->ToString());
#endif
				//在图纸列表中添加对应项
				HTREEITEM hSegItem=m_treeDrawing.InsertItem(pDrawing->m_sDrawingName,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hStruItem);
				TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(1,(DWORD)pDrawing));
				m_treeDrawing.SetItemData(hSegItem,(DWORD)pItemInfo);
				m_treeDrawing.Expand(hSegItem,TVE_COLLAPSE);
				CDrawingComponent *pComponent=NULL;
				for(pComponent=pDrawing->GetFirstDrawingComponent();pComponent;pComponent=pDrawing->GetNextDrawingComponent())
				{
					HTREEITEM hItem=m_treeDrawing.InsertItem(pComponent->m_sName,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hSegItem);
					pItemInfo=itemInfoList.append(TREEITEM_INFO(2,(DWORD)pComponent));
					m_treeDrawing.SetItemData(hItem,(DWORD)pItemInfo);
				}
			}
			m_treeDrawing.Expand(hStruItem,TVE_EXPAND);
		}
	}
	else if(m_pItemInfo->itemType==2)
	{	//重新生成该视图相关命令 wht 11-06-16
		CDrawingComponent *pComponent=(CDrawingComponent*)m_pItemInfo->dwRefData;
		if(pComponent)	//生成命令列表
			pComponent->CreateCmdList(pComponent->m_eClassType==DRAW_COM_STRUVIEW);//FALSE);
	}
}

void CSelectDrawingDlg::OnUpdateItem()
{
	//AfxGetApp()->DoWaitCursor(1);	//设置鼠标为等待状态  wht 12-03-23
	if(!m_pItemInfo)	
		UpdateDrawingList(FALSE);//更新所有图纸内容
	else 
		UpdateItem();			//更新选中项图纸或图元内容
	//AfxGetApp()->DoWaitCursor(-1);	//结束等待状态
}

void CSelectDrawingDlg::OnRebuildItem()
{
	//AfxGetApp()->DoWaitCursor(1);	//设置鼠标为等待状态  wht 12-03-23
	if(!m_pItemInfo)	
		UpdateDrawingList(TRUE);	//重置图纸列表
	else
		RebulidItem();				//重新生成选中项
	//AfxGetApp()->DoWaitCursor(-1);	//结束等待状态
}

void CSelectDrawingDlg::RefreshDrawingTree(CDrawing *pCurDrawing/*=NULL*/,
										   CDrawingComponent *pCurDrawingCom/*=NULL*/)
{
	itemInfoList.Empty();
	m_treeDrawing.DeleteAllItems();
	m_pItemInfo=NULL;	//初始化选中项
	m_pDrawing=NULL;
	m_pComponent=NULL;
	CDrawing *pDrawing=NULL;
	CDrawingComponent *pComponent=NULL;
	HTREEITEM hDrawing=NULL,hAnalysisItem=NULL,hSumItem=NULL,hStruItem=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	hDrawing=m_treeDrawing.InsertItem("drawing",PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET);
#else
	hDrawing=m_treeDrawing.InsertItem("图纸",PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET);
#endif
	m_treeDrawing.SetItemData(hDrawing,0);
	m_treeDrawing.Expand(hDrawing,TVE_EXPAND);
	//
	HTREEITEM hSelectedItem=NULL;
	for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
	{
		if(pDrawing->m_nSegI.iSeg==-1)
		{	//司令图
			hAnalysisItem=m_treeDrawing.InsertItem(pDrawing->m_sDrawingName,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hDrawing);
			TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(1,(DWORD)pDrawing));
			m_treeDrawing.SetItemData(hAnalysisItem,(DWORD)pItemInfo);
			if(pDrawing==pCurDrawing)
			{
				if(pCurDrawingCom)
					m_treeDrawing.Expand(hAnalysisItem,TVE_EXPAND);
				else 
					hSelectedItem=hAnalysisItem;
			}
			else 
				m_treeDrawing.Expand(hAnalysisItem,TVE_COLLAPSE);
			CStringKeyHashList<HTREEITEM> itemHashList;
			for(pComponent=pDrawing->GetFirstDrawingComponent();pComponent;pComponent=pDrawing->GetNextDrawingComponent())
			{
				HTREEITEM hGroupItem=hAnalysisItem;
				if(pComponent->m_pAttachView&&pComponent->m_pAttachView->m_iViewType==1)
				{	//定制视图对应的图元
					CXhChar16 sFolderNam("%s",pComponent->m_pAttachView->folderName);
					if(sFolderNam.GetLength()==0)
#ifdef AFX_TARG_ENU_ENGLISH
						sFolderNam.Copy("Custom spread view");
#else
						sFolderNam.Copy("定制展开视图");
#endif
					HTREEITEM *pFolderItem=itemHashList.GetValue(sFolderNam);
					if(pFolderItem==NULL)
					{	//添加定制视图所在分组对应项 wht 12-06-06
						HTREEITEM hFolderItem=m_treeDrawing.InsertItem(sFolderNam,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hAnalysisItem);
						TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(3,NULL));//类型改为3方便删除
						m_treeDrawing.SetItemData(hFolderItem,(DWORD)pItemInfo);
						itemHashList.SetValue(sFolderNam,hFolderItem);
						hGroupItem=hFolderItem;
					}
					else 
						hGroupItem=*pFolderItem;
				}
				CXhChar50 itemTitle=pComponent->m_sName;
				if(itemTitle.GetLength()==0)
					itemTitle.Copy(pComponent->m_pAttachView->name);
				HTREEITEM hItem=m_treeDrawing.InsertItem(itemTitle,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hGroupItem);
				TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(2,(DWORD)pComponent));
				m_treeDrawing.SetItemData(hItem,(DWORD)pItemInfo);
				if(pCurDrawingCom==pComponent)
					hSelectedItem=hItem;
			}
		}
		else if(pDrawing->m_nSegI.iSeg==-2)
		{
			hSumItem=m_treeDrawing.InsertItem(pDrawing->m_sDrawingName,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hDrawing);
			TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(1,(DWORD)pDrawing));
			m_treeDrawing.SetItemData(hSumItem,(DWORD)pItemInfo);
			if(pDrawing==pCurDrawing)
			{
				if(pCurDrawingCom)
					m_treeDrawing.Expand(hSumItem,TVE_EXPAND);
				else 
					hSelectedItem=hSumItem;
			}
			else 
				m_treeDrawing.Expand(hSumItem,TVE_COLLAPSE);
			for(pComponent=pDrawing->GetFirstDrawingComponent();pComponent;pComponent=pDrawing->GetNextDrawingComponent())
			{
				HTREEITEM hItem=m_treeDrawing.InsertItem(pComponent->m_sName,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hSumItem);
				TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(2,(DWORD)pComponent));
				m_treeDrawing.SetItemData(hItem,(DWORD)pItemInfo);
				if(pCurDrawingCom==pComponent)
					hSelectedItem=hItem;
			}
		}
		else
		{
			if(hStruItem==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				hStruItem=m_treeDrawing.InsertItem("structure map",PRJ_IMG_STATUS,PRJ_IMG_STATUS,hDrawing);
#else
				hStruItem=m_treeDrawing.InsertItem("结构图",PRJ_IMG_STATUS,PRJ_IMG_STATUS,hDrawing);
#endif
			TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(1,0));	//类型为1,数据为0 结构图目录项 wht 11-06-21
			m_treeDrawing.SetItemData(hStruItem,(DWORD)pItemInfo);
			//分段结构图
			HTREEITEM hSegItem=m_treeDrawing.InsertItem(pDrawing->m_sDrawingName,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hStruItem);
			pItemInfo=itemInfoList.append(TREEITEM_INFO(1,(DWORD)pDrawing));
			m_treeDrawing.SetItemData(hSegItem,(DWORD)pItemInfo);
			if(pDrawing==pCurDrawing)
			{
				if(pCurDrawingCom)
					m_treeDrawing.Expand(hSegItem,TVE_EXPAND);
				else 
					hSelectedItem=hSegItem;
			}
			else 
				m_treeDrawing.Expand(hSegItem,TVE_COLLAPSE);
			for(pComponent=pDrawing->GetFirstDrawingComponent();pComponent;pComponent=pDrawing->GetNextDrawingComponent())
			{
				HTREEITEM hItem=m_treeDrawing.InsertItem(pComponent->m_sName,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hSegItem);
				TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(2,(DWORD)pComponent));
				m_treeDrawing.SetItemData(hItem,(DWORD)pItemInfo);
				if(pCurDrawingCom==pComponent)
					hSelectedItem=hItem;
			}
		}
	}
	if(hStruItem==NULL)
	{	//结构图都被删除时也应保留结构图目录项 wht 11-07-15
#ifdef AFX_TARG_ENU_ENGLISH
		hStruItem=m_treeDrawing.InsertItem("structure map",PRJ_IMG_STATUS,PRJ_IMG_STATUS,hDrawing);
#else
		hStruItem=m_treeDrawing.InsertItem("结构图",PRJ_IMG_STATUS,PRJ_IMG_STATUS,hDrawing);
#endif
		TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(1,0));	//类型为1,数据为0 结构图目录项 wht 11-06-21
		m_treeDrawing.SetItemData(hStruItem,(DWORD)pItemInfo);
	}
	m_treeDrawing.SetRedraw();
	if(hSelectedItem)	//SelectItem会触发OnSelChanged事件故需要在最后设置选中项
	{
		HTREEITEM hParentItem=m_treeDrawing.GetParentItem(hSelectedItem);
		if(hParentItem)
		{	
			m_treeDrawing.Expand(hParentItem,TVE_EXPAND);
			m_treeDrawing.SelectItem(hSelectedItem);
			m_treeDrawing.Expand(hSelectedItem,TVE_EXPAND);
		}
	}
	else
		m_treeDrawing.Expand(hDrawing,TVE_EXPAND);
	//if(pDrawing==NULL&&pCurDrawingCom==NULL&&hStruItem)
	//	m_treeDrawing.Expand(hStruItem,TVE_EXPAND);	//
}

static BOOL FireSelChangeItem(CPropertyList *pPropList,CPropTreeItem *pItem)
{
	CSelectDrawingDlg *pDrawingDlg=(CSelectDrawingDlg*)pPropList->GetParent();
	if(pDrawingDlg==NULL)
		return FALSE;
	if(pDrawingDlg->m_pComponent&&pDrawingDlg->m_pComponent->m_eClassType==DRAW_COM_STRUVIEW)
	{
		CStruView *pStruView=(CStruView*)pDrawingDlg->m_pComponent;
		if(CSelectDrawingDlg::GetPropID("component.m_sSectionViewCode")==pItem->m_idProp
			||CSelectDrawingDlg::GetPropID("component.SectionView.startPt")==pItem->m_idProp
			||CSelectDrawingDlg::GetPropID("component.SectionView.startPt.x")==pItem->m_idProp
			||CSelectDrawingDlg::GetPropID("component.SectionView.startPt.y")==pItem->m_idProp
			||CSelectDrawingDlg::GetPropID("component.SectionView.endPt")==pItem->m_idProp
			||CSelectDrawingDlg::GetPropID("component.SectionView.endPt.x")==pItem->m_idProp
			||CSelectDrawingDlg::GetPropID("component.SectionView.endPt.y")==pItem->m_idProp
			||CSelectDrawingDlg::GetPropID("component.SectionView.bUpturned")==pItem->m_idProp
			||CSelectDrawingDlg::GetPropID("component.SectionView.hAttachViewKey")==pItem->m_idProp)
		{	//缩放至对应的剖视图
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol==NULL)
				return FALSE;
			SCOPE_STRU scope=pViewSymbol->GetEntScope(2);
			ZoomAcadView(scope,10);
		}
		else if(CSelectDrawingDlg::GetPropID("component.m_fNamePosSpace")==pItem->m_idProp
			||CSelectDrawingDlg::GetPropID("component.m_sName")==pItem->m_idProp)
		{
			SCOPE_STRU scope;
			ads_name ent_set;
			pStruView->GetAttachEntSet(ent_set,1,TRUE,&scope);
			ZoomAcadView(scope,100);
		}
	}
	return TRUE;
}

//From TsaFunc.cpp
void PreparePartSpecList(ATOM_LIST<SPEC_MARK> &specList);//预先准备简化的规格列表(司令图标注杆件规格时使用)
static BOOL ModifyDrawingValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CSelectDrawingDlg *pDrawingDlg=(CSelectDrawingDlg*)pPropList->GetParent();
	if(pDrawingDlg==NULL)
		return FALSE;
	CPropertyListOper<CSelectDrawingDlg> oper(pPropList,pDrawingDlg);
	CXhChar100 sText;
	CDrawing *pDrawing=NULL;
	CDrawingComponent *pDrawingComponent=NULL;
	if(pDrawingDlg->m_pItemInfo&&pDrawingDlg->m_pItemInfo->itemType==1)
		pDrawing=(CDrawing*)pDrawingDlg->m_pItemInfo->dwRefData;
	else if(pDrawingDlg->m_pItemInfo&&pDrawingDlg->m_pItemInfo->itemType==2)
		pDrawingComponent=(CDrawingComponent*)pDrawingDlg->m_pItemInfo->dwRefData;
	else 
		return 0;
	if(pDrawing)
	{
		if(CSelectDrawingDlg::GetPropID("drawing.m_nSegI")==pItem->m_idProp)
			pDrawing->m_nSegI=SEGI(valueStr.GetBuffer());
		else if(CSelectDrawingDlg::GetPropID("drawing.m_xMapFrm.style")==pItem->m_idProp)
		{
			sText.Printf("%s",valueStr);
			pDrawing->m_xMapFrm=*sys.m_hashMapFrameList.GetValue(sText);
			long idProp=CSelectDrawingDlg::GetPropID("drawing.m_xMapFrm.nLength");
			pPropList->SetItemReadOnly(idProp);
			if(pDrawingDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,CString(sText));
			idProp=CSelectDrawingDlg::GetPropID("drawing.m_xMapFrm.nWidth");
			pPropList->SetItemReadOnly(idProp);
			if(pDrawingDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,CString(sText));
			idProp=CSelectDrawingDlg::GetPropID("drawing.m_xMapFrm.nLeftMargin");
			pPropList->SetItemReadOnly(idProp);
			if(pDrawingDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,CString(sText));
			idProp=CSelectDrawingDlg::GetPropID("drawing.m_xMapFrm.nOtherMargin");
			pPropList->SetItemReadOnly(idProp);
			if(pDrawingDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,CString(sText));
			idProp=CSelectDrawingDlg::GetPropID("drawing.m_xMapFrm.nBottomRightFixLen");
			if(pDrawingDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,CString(sText));
			idProp=CSelectDrawingDlg::GetPropID("drawing.m_xMapFrm.nBottomRightFixWidth");
			if(pDrawingDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,CString(sText));
			//重新初始化表格放置位置
			CDrawingComponent *pComponent=NULL;
			for(pComponent=pDrawing->GetFirstDrawingComponent();pComponent;pComponent=pDrawing->GetNextDrawingComponent())
			{
				if(pComponent->m_eClassType==DRAW_COM_BOM)
					((CDrawingBOM*)pComponent)->UpdateTableInitPos();
			}
		}
		else if(CSelectDrawingDlg::GetPropID("drawing.m_xMapFrm.nLength")==pItem->m_idProp)
			pDrawing->m_xMapFrm.nLength=atoi(valueStr);
		else if(CSelectDrawingDlg::GetPropID("drawing.m_xMapFrm.nWidth")==pItem->m_idProp)
			pDrawing->m_xMapFrm.nWidth=atoi(valueStr);
		else if(CSelectDrawingDlg::GetPropID("drawing.m_xMapFrm.nLeftMargin")==pItem->m_idProp)
			pDrawing->m_xMapFrm.nLeftMargin=atoi(valueStr);
		else if(CSelectDrawingDlg::GetPropID("drawing.m_xMapFrm.nOtherMargin")==pItem->m_idProp)
		{
			pDrawing->m_xMapFrm.nRightMargin=pDrawing->m_xMapFrm.nTopMargin
				=pDrawing->m_xMapFrm.nBottomMargin=atoi(valueStr);
		}
		else if(CSelectDrawingDlg::GetPropID("drawing.m_xMapFrm.nBottomRightFixLen")==pItem->m_idProp)
			pDrawing->m_xMapFrm.nBottomRightFixLen=atoi(valueStr);
		else if(CSelectDrawingDlg::GetPropID("drawing.m_xMapFrm.nBottomRightFixWidth")==pItem->m_idProp)
		{
			pDrawing->m_xMapFrm.nBottomRightFixWidth=atoi(valueStr);
			CDrawingBOM *pSegPartBOM=NULL;
			CDrawingComponent *pCom=NULL;
			for(pCom=pDrawing->GetFirstDrawingComponent();pCom;pCom=pDrawing->GetNextDrawingComponent())
			{
				if(pCom->m_eClassType!=DRAW_COM_BOM)
					continue;
				CDrawingBOM *pDrawingBOM=(CDrawingBOM*)pCom;
				if(pDrawingBOM->m_eBOMType!=BOM_SEGPART)
					continue;
				pSegPartBOM=pDrawingBOM;
				break;
			}
			if(pSegPartBOM)
				pSegPartBOM->CreateCmdList();
		}
		else if(CSelectDrawingDlg::GetPropID("drawing.m_sDrawingName")==pItem->m_idProp)
		{
			pDrawing->m_sDrawingName.Copy(valueStr);
			HTREEITEM hCurItem=pDrawingDlg->m_treeDrawing.GetSelectedItem();
			if(hCurItem)	//同时更新左侧树中对应项名称 wht 11-06-16
				pDrawingDlg->m_treeDrawing.SetItemText(hCurItem,pDrawing->m_sDrawingName);
		}
		else if(CSelectDrawingDlg::GetPropID("drawing.m_sDrawingNo")==pItem->m_idProp)
			pDrawing->m_sDrawingNo.Copy(valueStr);
		else if(CSelectDrawingDlg::GetPropID("drawing.m_bSpecifyMapFrmPos")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				pDrawing->m_bSpecifyMapFrmPos=TRUE;
			else
				pDrawing->m_bSpecifyMapFrmPos=FALSE;
		}
		else if(CSelectDrawingDlg::GetPropID("drawing.m_fComponentSpace")==pItem->m_idProp)
			pDrawing->m_fComponentSpace=atof(valueStr);
		else if(CSelectDrawingDlg::GetPropID("drawing.m_bAutoLayoutComponent")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				pDrawing->m_bAutoLayoutComponent=TRUE;
			else
				pDrawing->m_bAutoLayoutComponent=FALSE;
		}
		else if(CSelectDrawingDlg::GetPropID("drawing.m_hActiveModule")==pItem->m_idProp)
		{
			CLDSModule *pModule=NULL;
			long hOldActiveModule=pDrawing->m_hActiveModule;
			for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
			{
				if(valueStr.CompareNoCase(pModule->description)==0)
				{
					pDrawing->m_hActiveModule=pModule->handle;
					break;
				}
			}
			if(hOldActiveModule!=pDrawing->m_hActiveModule)
			{	//修改当前呼高后重新生成指定图纸 wht 11-06-16
				pDrawingDlg->RebulidItem();
				//某些时候需要多张总图，为保证多张总图激活呼高一致
				//修改当前图纸激活呼高时应同时修改塔当前激活呼高 wht 11-07-14
				Ta.m_hActiveModule=pDrawing->m_hActiveModule;	
				/*CDrawingComponent *pComponent=NULL,*pOldActiveCom=NULL,*pNewActiveCom=NULL,*pSideCom=NULL;
				//改变图纸当前激活呼高后所有图元均需要指定位置生成
				pDrawing->m_bSpecifyMapFrmPos=TRUE;
				for(pComponent=pDrawing->GetFirstDrawingComponent();pComponent;pComponent=pDrawing->GetNextDrawingComponent())
				{
					pComponent->m_bSpecifyComPos=TRUE;
					if(pComponent->m_hModule==pDrawing->m_hActiveModule)
					{	//
						pNewActiveCom=pComponent;
						pComponent->m_hModule=pDrawing->m_hActiveModule;
						pComponent->m_pModule=Ta.Module.FromHandle(pComponent->m_hModule);
						if(pComponent->m_pModule)
							pComponent->m_sName.Printf("%s主体图",pComponent->m_pModule->description);
						pComponent->CreateCmdList();
					}
					else if(pComponent->m_hModule==hOldActiveModule)
					{	//
						BOOL bSideView=FALSE;
						if(pComponent->m_pAttachView&&stricmp(pComponent->m_pAttachView->defaultLayer,"SPY")==0)
							bSideView=TRUE;
						if(bSideView)
						{	//旧侧面图应修改为当前主体图对应的侧面图 wht 11-06-16
							pSideCom=pComponent;
							pComponent->m_hModule=pDrawing->m_hActiveModule;
							pComponent->m_pModule=Ta.Module.FromHandle(pComponent->m_hModule);
							if(pComponent->m_pModule)
								pComponent->m_sName.Printf("%s侧面图",pComponent->m_pModule->description);
							pComponent->CreateCmdList();
						}
						else
						{	//旧主体图
							pOldActiveCom=pComponent;
							pComponent->m_hModule=hOldActiveModule;
							pComponent->m_pModule=Ta.Module.FromHandle(pComponent->m_hModule);
							if(pComponent->m_pModule)
								pComponent->m_sName.Printf("%s呼高图",pComponent->m_pModule->description);
							pComponent->CreateCmdList();
						}
					}
				}
				//刷新左侧树节点
				HTREEITEM hDrawingItem=pDrawingDlg->m_treeDrawing.GetSelectedItem();
				HTREEITEM hChildItem=pDrawingDlg->m_treeDrawing.GetChildItem(hDrawingItem);
				while(hChildItem&&pOldActiveCom&&pNewActiveCom)
				{
					TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pDrawingDlg->m_treeDrawing.GetItemData(hChildItem);
					CDrawingComponent *pCom=NULL;
					if(pItemInfo&&pItemInfo->itemType==2)
						pCom=(CDrawingComponent*)pItemInfo->dwRefData;
					if(pCom&&(pCom==pOldActiveCom||pCom==pNewActiveCom||pCom==pSideCom))
						pDrawingDlg->m_treeDrawing.SetItemText(hChildItem,pCom->m_sName);
					hChildItem=pDrawingDlg->m_treeDrawing.GetNextItem(hChildItem,TVGN_NEXT);
				}*/
			}
		}
		else if(CSelectDrawingDlg::GetPropID("LineView::m_bUsePolyLineChiefMap")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Polygon line")==0)
#else
			if(valueStr.CompareNoCase("多段线")==0)
#endif
				CLineView::m_bUsePolyLineChiefMap=TRUE;
			else
				CLineView::m_bUsePolyLineChiefMap=FALSE;
		}
		else if(CSelectDrawingDlg::GetPropID("LineView::m_bUsePolyLineSumMap")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Polygon line")==0)
#else
			if(valueStr.CompareNoCase("多段线")==0)
#endif
				CLineView::m_bUsePolyLineSumMap=TRUE;
			else
				CLineView::m_bUsePolyLineSumMap=FALSE;
		}
	}
	else if(pDrawingComponent)
	{
		if(CSelectDrawingDlg::GetPropID("component.m_bSpecifyComPos")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				pDrawingComponent->m_bSpecifyComPos=TRUE;
			else 
				pDrawingComponent->m_bSpecifyComPos=FALSE;
			pPropList->DeleteItemByPropId(CSelectDrawingDlg::GetPropID("component.m_ptLocation"));
			pPropList->DeleteItemByPropId(CSelectDrawingDlg::GetPropID("component.m_ptLocation.x"));
			pPropList->DeleteItemByPropId(CSelectDrawingDlg::GetPropID("component.m_ptLocation.y"));
			CPropTreeItem *pFindItem=pPropList->FindItemByPropId(CSelectDrawingDlg::GetPropID("component"),NULL);
			if(pFindItem&&!pDrawingComponent->m_bSpecifyComPos)
			{	//
#ifdef AFX_TARG_ENU_ENGLISH
				char sAnchorType[50]="location";
				if(pDrawingComponent->m_nAnchorType==0)
					strcpy(sAnchorType,"Top left corner");
				else if(pDrawingComponent->m_nAnchorType==1)
					strcpy(sAnchorType,"Top right corner");
				else if(pDrawingComponent->m_nAnchorType==2)
					strcpy(sAnchorType,"Lower left quarter");
				else if(pDrawingComponent->m_nAnchorType==3)
					strcpy(sAnchorType,"Lower right corner");
#else
				char sAnchorType[50]="位置";
				if(pDrawingComponent->m_nAnchorType==0)
					strcpy(sAnchorType,"左上角");
				else if(pDrawingComponent->m_nAnchorType==1)
					strcpy(sAnchorType,"右上角");
				else if(pDrawingComponent->m_nAnchorType==2)
					strcpy(sAnchorType,"左下角");
				else if(pDrawingComponent->m_nAnchorType==3)
					strcpy(sAnchorType,"右下角");
#endif
				//
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "relative position of location point";
				lpInfo->m_strPropHelp.Format("offset position of view %s relative to the top left corner of drawing frame.",sAnchorType);
#else
				lpInfo->m_strPropName = "定位点相对位置";
				lpInfo->m_strPropHelp.Format("视图%s相对于图框左上角的偏移位置。",sAnchorType);
#endif
				CPropTreeItem *pPropItem = pPropList->InsertItem(pFindItem,lpInfo,pItem->m_iIndex,TRUE);
				pPropItem->m_idProp = CSelectDrawingDlg::GetPropID("component.m_ptLocation");
				pPropItem->m_bHideChildren=TRUE;
				if(pDrawingDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				//
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "offset in X direction";
				lpInfo->m_strPropHelp.Format("offset value of view %s relative to the top left corner of drawing frame in X direction.",sAnchorType);
#else
				lpInfo->m_strPropName = "X方向偏移值";
				lpInfo->m_strPropHelp.Format("视图%s相对于图框左上角X方向的偏移值。",sAnchorType);
#endif
				CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,TRUE);
				pSonPropItem->m_idProp = CSelectDrawingDlg::GetPropID("component.m_ptLocation.x");
				if(pDrawingDlg->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				//
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "offset in Y direction";
				lpInfo->m_strPropHelp.Format("offset value of view %s relative to the top left corner of drawing frame in Y direction.",sAnchorType);
#else
				lpInfo->m_strPropName = "Y方向偏移值";
				lpInfo->m_strPropHelp.Format("视图%s相对于图框左上角Y方向的偏移值。",sAnchorType);
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,1,TRUE);
				pSonPropItem->m_idProp = CSelectDrawingDlg::GetPropID("component.m_ptLocation.y");
				if(pDrawingDlg->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else if(CSelectDrawingDlg::GetPropID("component.m_eClassType")==pItem->m_idProp)
			pDrawingComponent->m_eClassType=(DRAW_COMPONENT_TYPE)atoi(valueStr);
		else if(CSelectDrawingDlg::GetPropID("component.m_sDrawingLayer")==pItem->m_idProp)
			pDrawingComponent->m_sDrawingLayer.Copy(valueStr);
		else if(CSelectDrawingDlg::GetPropID("component.m_sName")==pItem->m_idProp)
		{
			pDrawingComponent->ModifyComponentName(valueStr.GetBuffer(1));
			HTREEITEM hCurItem=pDrawingDlg->m_treeDrawing.GetSelectedItem();
			if(hCurItem)	//同时更新左侧树中对应项名称 wht 11-06-16
				pDrawingDlg->m_treeDrawing.SetItemText(hCurItem,pDrawingComponent->m_sName);
			//更新生成按钮名称
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("create 【%s】",(char*)pDrawingComponent->m_sName);
#else
			sText.Printf("生成【%s】",(char*)pDrawingComponent->m_sName);
#endif
			pDrawingDlg->SetDlgItemText(IDOK,sText);
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&CSelectDrawingDlg::GetPropID("component.m_sSectionViewCode")==pItem->m_idProp)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol)
			{
				f2dRect rect=pViewSymbol->GetDrawRegion();
				ZoomAcadView(rect,100);	//移动窗口至到剖视图位置
			}
			if(valueStr.CompareNoCase(pDrawingComponent->m_sSectionViewCode)==0)
				return FALSE;	//内容未发生变化不需要执行以下操作
			pDrawingComponent->m_sSectionViewCode.Copy(valueStr);
			if(pViewSymbol)
				pViewSymbol->ModifySymbolName(pDrawingComponent->m_sSectionViewCode);
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&CSelectDrawingDlg::GetPropID("component.SectionView.startPt.x")==pItem->m_idProp)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol==NULL)
				return FALSE;
			pViewSymbol->startPt.x=atof(valueStr);
			//删除原来的实体重新生成
			pViewSymbol->EraseRelativeAcadEntity();
			pViewSymbol->Draw();
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&CSelectDrawingDlg::GetPropID("component.SectionView.startPt.y")==pItem->m_idProp)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol==NULL)
				return FALSE;
			pViewSymbol->startPt.y=atof(valueStr);
			//删除原来的实体重新生成
			pViewSymbol->EraseRelativeAcadEntity();
			pViewSymbol->Draw();
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&CSelectDrawingDlg::GetPropID("component.SectionView.endPt.x")==pItem->m_idProp)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol==NULL)
				return FALSE;
			pViewSymbol->endPt.x=atof(valueStr);
			//删除原来的实体重新生成
			pViewSymbol->EraseRelativeAcadEntity();
			pViewSymbol->Draw();
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&CSelectDrawingDlg::GetPropID("component.SectionView.endPt.y")==pItem->m_idProp)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol==NULL)
				return FALSE;
			pViewSymbol->endPt.y=atof(valueStr);
			//删除原来的实体重新生成
			pViewSymbol->EraseRelativeAcadEntity();
			pViewSymbol->Draw();
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&CSelectDrawingDlg::GetPropID("component.SectionView.bUpturned")==pItem->m_idProp)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol==NULL)
				return FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Upwards")==0)
#else
			if(valueStr.CompareNoCase("朝上")==0)
#endif
				pViewSymbol->bUpturned=TRUE;
			else 
				pViewSymbol->bUpturned=FALSE;
			//删除原来的实体重新生成
			pViewSymbol->EraseRelativeAcadEntity();
			pViewSymbol->Draw();
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&CSelectDrawingDlg::GetPropID("component.SectionView.hAttachViewKey")==pItem->m_idProp)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CDrawing *pDrawing=pStruView->ParentDrawing();
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol==NULL||pDrawing==NULL)
				return FALSE;
			long key=0;
			int i=0,iViewIndex=valueStr[0]-'0';
			CDrawingComponent *pCom=NULL;
			for(pCom=pDrawing->GetFirstDrawingComponent();pCom;pCom=pDrawing->GetNextDrawingComponent())
			{
				if(i==iViewIndex)
				{
					key=pDrawing->listComponents.GetCursorKey();
					break;
				}
				i++;
			}
			if(pCom)
				pViewSymbol->hAttachViewKey=key;
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&(CSelectDrawingDlg::GetPropID("component.SectionView.startPt")==pItem->m_idProp
			||CSelectDrawingDlg::GetPropID("component.SectionView.endPt")==pItem->m_idProp))
		{
			;
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
				&&CSelectDrawingDlg::GetPropID("struView.partNo.m_bDrawRefLine")==pItem->m_idProp)//0:X轴对称；1:Y轴对称;其余不对称处理
		{
			CDrawingCommand *pCmd=NULL;
			for(pCmd=pDrawingComponent->cmdList.GetFirst();pCmd;pCmd=pDrawingComponent->cmdList.GetNext())
			{
				if(pCmd->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				BOOL bOldDrawRefLine=pCmd->m_paraPartSpec.bDrawRefLine;
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
#else
				if(valueStr.CompareNoCase("是")==0)
#endif
					pCmd->m_paraPartSpec.bDrawRefLine=TRUE;
				else 
					pCmd->m_paraPartSpec.bDrawRefLine=FALSE;
				if(bOldDrawRefLine!=pCmd->m_paraPartSpec.bDrawRefLine)	//修改对称方式后重新标注该构件
					pCmd->Rerun();	//删除命令生成的原有实体,重新生成
			}
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
				&&CSelectDrawingDlg::GetPropID("struView.partNo.m_iMirMsg")==pItem->m_idProp)//0:X轴对称；1:Y轴对称;其余不对称处理
		{
			CDrawingCommand *pCmd=NULL;
			for(pCmd=pDrawingComponent->cmdList.GetFirst();pCmd;pCmd=pDrawingComponent->cmdList.GetNext())
			{
				if(pCmd->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				if(pCmd->m_paraPartSpec.pObj==NULL)
					pCmd->m_paraPartSpec.pObj=(CLDSDbObject*)Ta.FromHandle(pCmd->m_paraPartSpec.hObj,CLS_DBOBJECT);
				if(pCmd->m_paraPartSpec.pObj==NULL||!pCmd->m_paraPartSpec.pObj->IsPart())
					continue;
				int iOldMirMsg=pCmd->m_paraPartSpec.iMirMsg;
				pCmd->m_paraPartSpec.iMirMsg=valueStr[0]-'0';
				if(iOldMirMsg!=pCmd->m_paraPartSpec.iMirMsg)	//修改对称方式后重新标注该构件
					pCmd->Rerun();	//删除命令生成的原有实体,重新生成
			}
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_PARTVIEW
			&&CSelectDrawingDlg::GetPropID("PartView.sPartNo")==pItem->m_idProp)
		{
			CPartDrawing *pPartView=(CPartDrawing*)pDrawingComponent;
			pPartView->sPartNo.Copy(valueStr);
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_PARTVIEW
			&&CSelectDrawingDlg::GetPropID("PartView.hPart")==pItem->m_idProp)
		{	
			CPartDrawing *pPartView=(CPartDrawing*)pDrawingComponent;
			sscanf(valueStr,"%X",&pPartView->m_hPart);
		}
		else if(CSelectDrawingDlg::GetPropID("component.m_fScale")==pItem->m_idProp)
		{
			pDrawingComponent->m_fScale=1.0/atof(valueStr);
			if(pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW&&
				pDrawingComponent->ParentDrawing()&&pDrawingComponent->ParentDrawing()->m_nSegI.iSeg>=0)
			{	//结构图中单线图修改修改名称时更新视图名称 wht 11-07-20
#ifdef AFX_TARG_ENU_ENGLISH
				pDrawingComponent->m_sName.Printf("single line map 1:%d",atoi(valueStr));
#else
				pDrawingComponent->m_sName.Printf("单线图 1:%d",atoi(valueStr));
#endif
				pPropList->SetItemPropValue(CSelectDrawingDlg::GetPropID("component.m_sName"),pDrawingComponent->m_sName);
				//同时更新左侧树中对应项名称
				HTREEITEM hCurItem=pDrawingDlg->m_treeDrawing.GetSelectedItem();
				if(hCurItem)
					pDrawingDlg->m_treeDrawing.SetItemText(hCurItem,pDrawingComponent->m_sName);
				//更新生成按钮名称
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("create 【%s】",(char*)pDrawingComponent->m_sName);
#else
				sText.Printf("生成【%s】",(char*)pDrawingComponent->m_sName);
#endif
				pDrawingDlg->SetDlgItemText(IDOK,sText);
			}
		}
		else if(CSelectDrawingDlg::GetPropID("component.m_fNamePosSpace")==pItem->m_idProp)
		{
			pDrawingComponent->m_fNamePosSpace=atof(valueStr);
			pDrawingComponent->InsertDrawingName();	//修改视图名称位置间隙后重新生成 wht 11-07-31
		}
		else if(CSelectDrawingDlg::GetPropID("component.m_nAnchorType")==pItem->m_idProp)
		{
			pDrawingComponent->m_nAnchorType=(TBL_LOCATE_STYLE)(valueStr[0]-'0');
#ifdef AFX_TARG_ENU_ENGLISH
			char sAnchorType[50]="location";
			if(pDrawingComponent->m_nAnchorType==0)
				strcpy(sAnchorType,"Top left corner");
			else if(pDrawingComponent->m_nAnchorType==1)
				strcpy(sAnchorType,"Top right corner");
			else if(pDrawingComponent->m_nAnchorType==2)
				strcpy(sAnchorType,"Lower left quarter");
			else if(pDrawingComponent->m_nAnchorType==3)
				strcpy(sAnchorType,"Lower right corner");
			pPropList->SetItemPropHelpStr(CSelectDrawingDlg::GetPropID("component.m_ptLocation"),
				"offset position of view %s relative to the top left corner of drawing frame.",sAnchorType);
			pPropList->SetItemPropHelpStr(CSelectDrawingDlg::GetPropID("component.m_ptLocation.x"),
				"offset value of view %s relative to the top left corner of drawing frame in X direction.",sAnchorType);
			pPropList->SetItemPropHelpStr(CSelectDrawingDlg::GetPropID("component.m_ptLocation.y"),
				"offset value of view %s relative to the top left corner of drawing frame in Y direction.",sAnchorType);
#else
			char sAnchorType[50]="位置";
			if(pDrawingComponent->m_nAnchorType==0)
				strcpy(sAnchorType,"左上角");
			else if(pDrawingComponent->m_nAnchorType==1)
				strcpy(sAnchorType,"右上角");
			else if(pDrawingComponent->m_nAnchorType==2)
				strcpy(sAnchorType,"左下角");
			else if(pDrawingComponent->m_nAnchorType==3)
				strcpy(sAnchorType,"右下角");
			pPropList->SetItemPropHelpStr(CSelectDrawingDlg::GetPropID("component.m_ptLocation"),
				"视图%s相对于图框左上角的偏移位置。",sAnchorType);
			pPropList->SetItemPropHelpStr(CSelectDrawingDlg::GetPropID("component.m_ptLocation.x"),
				"视图%s相对于图框左上角X方向的偏移值。",sAnchorType);
			pPropList->SetItemPropHelpStr(CSelectDrawingDlg::GetPropID("component.m_ptLocation.y"),
				"视图%s相对于图框左上角X方向的偏移值。",sAnchorType);
#endif
			if(pDrawingComponent->IsNeedSpecifyComPos())
				pDrawingComponent->CalInitAssemblyPosition();
		}
		else if(CSelectDrawingDlg::GetPropID("component.m_ptLocation.x")==pItem->m_idProp)
		{
			f3dPoint ptOldLocation=pDrawingComponent->GetAnchorPointRelativePos();
			double fLocationX=atof(valueStr);
			double fOffsetX=fLocationX-ptOldLocation.x;
			if(fabs(fOffsetX)>EPS)
			{
				pDrawingComponent->ModifyAnchorPointRelativePos(&fOffsetX,NULL);
				pDrawingComponent->ReCreate();	//重新生成
			}
		}
		else if(CSelectDrawingDlg::GetPropID("component.m_ptLocation.y")==pItem->m_idProp)
		{
			f3dPoint ptOldLocation=pDrawingComponent->GetAnchorPointRelativePos();
			double fLocationY=atof(valueStr);
			double fOffsetY=fLocationY-ptOldLocation.y;
			if(fabs(fOffsetY)>EPS)
			{
				pDrawingComponent->ModifyAnchorPointRelativePos(NULL,&fOffsetY);
				pDrawingComponent->ReCreate();	//重新生成
			}
		}
		else if(CSelectDrawingDlg::GetPropID("component.m_bNeedAdjust")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				pDrawingComponent->m_bNeedAdjust=TRUE;
			else 
				pDrawingComponent->m_bNeedAdjust=FALSE;
		}
		else if(CSelectDrawingDlg::GetPropID("lineView.m_bDimNodeNo")==pItem->m_idProp
			&&pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW)
		{
			CLineView *pLineView=(CLineView*)pDrawingComponent;
			if(pLineView->m_bAnalysisMode)
			{	//力学模式
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
#else
				if(valueStr.CompareNoCase("是")==0)
#endif
					pLineView->m_bDimNodeNo=TRUE;
				else 
					pLineView->m_bDimNodeNo=FALSE;
			}
		}
		else if(CSelectDrawingDlg::GetPropID("lineView.m_bDimPoleGuige")==pItem->m_idProp
			&&pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW)
		{
			CLineView *pLineView=(CLineView*)pDrawingComponent;
			if(pLineView->m_bAnalysisMode)
			{	//力学模式
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
#else
				if(valueStr.CompareNoCase("是")==0)
#endif
					pLineView->m_bDimPoleGuige=TRUE;
				else 
					pLineView->m_bDimPoleGuige=FALSE;
			}
		}
		else if(CSelectDrawingDlg::GetPropID("drawingBOM.m_bAutoNewTable")==pItem->m_idProp
			&&pDrawingComponent->m_eClassType==DRAW_COM_BOM)
		{
			CDrawingBOM *pDrawingBOM=(CDrawingBOM*)pDrawingComponent;
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				pDrawingBOM->m_bAutoNewTable=TRUE;
			else 
				pDrawingBOM->m_bAutoNewTable=FALSE;
		}
		else if(CSelectDrawingDlg::GetPropID("drawingBOM.m_bHasTableHead")==pItem->m_idProp
			&&pDrawingComponent->m_eClassType==DRAW_COM_BOM)
		{
			CDrawingBOM *pDrawingBOM=(CDrawingBOM*)pDrawingComponent;
			BOOL bHasTableHead=pDrawingBOM->m_bHasTableHead;
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				pDrawingBOM->m_bHasTableHead=TRUE;
			else 
				pDrawingBOM->m_bHasTableHead=FALSE;
			if(bHasTableHead!=pDrawingBOM->m_bHasTableHead
				&&pDrawingBOM->xFormatTbl2.GetRowsCount()>0
				&&pDrawingBOM->m_eBOMType==BOM_SEGPART)
			{	//调整分页后的表格
				if(bHasTableHead)
				{	//删除表头即前三行
					pDrawingBOM->xFormatTbl2.DelRow(0);
					pDrawingBOM->xFormatTbl2.DelRow(0);
					pDrawingBOM->xFormatTbl2.DelRow(0);
				}
				else 
				{	//插入表头即xFromatTbl1的前三行
					for(int i=0;i<3;i++)
					{
						double row_height;
						pDrawingBOM->xFormatTbl1.GetRowHigh(i,&row_height);
						pDrawingBOM->xFormatTbl2.InsertRow(i,row_height);
						int nColNum=pDrawingBOM->xFormatTbl1.GetColumnsCount();
						for(int j=0;j<nColNum;j++)
						{
							CGrid *pGrid = pDrawingBOM->xFormatTbl1.GetGridAt(i,j);
							pDrawingBOM->xFormatTbl2.GetGridAt(i,j)->Copy(*pGrid);
						}
					}
				}
			}
		}
		else if(CSelectDrawingDlg::GetPropID("component.m_bDimScale")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				pDrawingComponent->m_bDimScale=TRUE;
			else 
				pDrawingComponent->m_bDimScale=FALSE;
		}
		else if(CSelectDrawingDlg::GetPropID("component.m_bDimName")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				pDrawingComponent->m_bDimName=TRUE;
			else 
				pDrawingComponent->m_bDimName=FALSE;
		}
	}
	else if(pDrawingDlg->m_pItemInfo&&pDrawingDlg->m_pItemInfo->itemType==1)
	{	//结构图公共变量
		if(CSelectDrawingDlg::GetPropID("StruView::m_bDimPartSpec")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				CStruView::m_bDimPartSpec=TRUE;
			else
				CStruView::m_bDimPartSpec=FALSE;
		}
		else if(CSelectDrawingDlg::GetPropID("StruView::m_bDimPoleOddment")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				CStruView::m_bDimPoleOddment=TRUE;
			else
				CStruView::m_bDimPoleOddment=FALSE;
		}
		else if(CSelectDrawingDlg::GetPropID("StruView::m_bDimFootNailSpace")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				CStruView::m_bDimFootNailSpace=TRUE;
			else
				CStruView::m_bDimFootNailSpace=FALSE;
		}
		else if(CSelectDrawingDlg::GetPropID("StruView::m_bDimFillPlateSpace")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				CStruView::m_bDimFillPlateSpace=TRUE;
			else
				CStruView::m_bDimFillPlateSpace=FALSE;
		}
		else if(CSelectDrawingDlg::GetPropID("StruView::m_bDimLsSpace")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				CStruView::m_bDimLsSpace=TRUE;
			else
				CStruView::m_bDimLsSpace=FALSE;
		}
		else if(CSelectDrawingDlg::GetPropID("StruView::m_bDimJgZhunJu")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				CStruView::m_bDimJgZhunJu=TRUE;
			else
				CStruView::m_bDimJgZhunJu=FALSE;
		}
		else if(CSelectDrawingDlg::GetPropID("StruView::m_bDimBoltInfo")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				CStruView::m_bDimBoltInfo=TRUE;
			else
				CStruView::m_bDimBoltInfo=FALSE;
		}
		else if(CSelectDrawingDlg::GetPropID("StruView::m_bDimBoltPad")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				CStruView::m_bDimBoltPad=TRUE;
			else
				CStruView::m_bDimBoltPad=FALSE;
		}
		else if(CSelectDrawingDlg::GetPropID("StruView::m_bDimSimPoly")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				CStruView::m_bDimSimPoly=TRUE;
			else
				CStruView::m_bDimSimPoly=FALSE;
		}
	}
	return TRUE;
}

static BOOL ButtonClickDrawing(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	CSelectDrawingDlg *pDrawingDlg=(CSelectDrawingDlg*)pPropList->GetParent();
	if(pDrawingDlg==NULL)
		return FALSE;
	if(pDrawingDlg->m_pItemInfo==NULL)
		return FALSE;
	CDrawing *pCurDrawing=NULL;
	CDrawingComponent *pCurComponent=NULL;
	if(pDrawingDlg->m_pItemInfo->itemType==1)
		pCurDrawing=(CDrawing*)pDrawingDlg->m_pItemInfo->dwRefData;
	if(pDrawingDlg->m_pItemInfo->itemType==2)
		pCurComponent=(CDrawingComponent*)pDrawingDlg->m_pItemInfo->dwRefData;
	else
		return FALSE;
	if(pCurComponent==NULL&&pCurDrawing==NULL)
		return FALSE;
	CAcModuleResourceOverride resOverride;
	CXhChar100 sText;
	if(pCurComponent)
	{
		if(CSelectDrawingDlg::GetPropID("component.m_ptLocation")==pItem->m_idProp)
		{
			pDrawingDlg->HideWnd(TRUE);	//隐藏对话框
			//选中当前视图中的所有构件
			DRAGSET.ClearEntSet();
			pCurComponent->GetEntSet(DRAGSET.drag_ent_set);
			//启动拖放
			f3dPoint basepnt;
			f2dRect rect=pCurComponent->GetDrawRegion();
			if(pCurComponent->m_nAnchorType==TOP_LEFT)
				basepnt.Set(rect.topLeft.x,rect.topLeft.y,0);
			else if(pCurComponent->m_nAnchorType==TOP_RIGHT)
				basepnt.Set(rect.bottomRight.x,rect.topLeft.y,0);
			else if(pCurComponent->m_nAnchorType==BOTTOM_LEFT)
				basepnt.Set(rect.topLeft.x,rect.bottomRight.y,0);
			else //if(pCurComponent->m_nAnchorType==BOTTOM_RIGHT)
				basepnt.Set(rect.bottomRight.x,rect.bottomRight.y,0);
			f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar100 sPrompt("Please enter [%s] the insertion point\n",&pCurComponent->m_sName);
#else
			CXhChar100 sPrompt("请输入[%s]插入点\n",&pCurComponent->m_sName);
#endif
			int nRetCode=DragEntSet(basepnt,sPrompt);
			if(nRetCode==RTNORM)
			{	//视图位置变动后才需要更新坐标系位置
				double fOffsetX=basepnt.x-old_pos.x;
				double fOffsetY=basepnt.y-old_pos.y;
				pCurComponent->ModifyAnchorPointRelativePos(&fOffsetX,&fOffsetY);
				//更新界面
				CWnd* pMainWnd=AfxGetMainWnd();
				if(pMainWnd)	//存在pMainWnd=NULL的情况
					pMainWnd->SetFocus();
				actrTransactionManager->flushGraphics();
				acedUpdateDisplay();
			}
			long idProp=CSelectDrawingDlg::GetPropID("component.m_ptLocation.x");
			if(pDrawingDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,"%s",&sText);
			idProp=CSelectDrawingDlg::GetPropID("component.m_ptLocation.y");
			if(pDrawingDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,"%s",&sText);
		}
		else if(CSelectDrawingDlg::GetPropID("drawingBOM.nBriefGuigeN")==pItem->m_idProp
			&&pCurComponent->m_eClassType==DRAW_COM_BOM)
		{
			CDrawingBOM *pDrawingBOM=(CDrawingBOM*)pCurComponent;
			if(pDrawingBOM->m_eBOMType==BOM_GUIGE)
			{
				//修改简化规格数后重新生成简化的规格列表数据
				PreparePartSpecList(pDrawingBOM->SpecList);
				pDrawingBOM->m_nBriefGuigeN=sys.nBriefGuigeN;		//简化规格数
				if(pDrawingDlg->GetPropValueStr(CSelectDrawingDlg::GetPropID("drawingBOM.nBriefGuigeN"),sText)>0)
					pPropList->SetItemPropValue(CSelectDrawingDlg::GetPropID("drawingBOM.nBriefGuigeN"),"%s",&sText);
			}
		}
		else if(CSelectDrawingDlg::GetPropID("component.m_sName")==pItem->m_idProp)
		{
			pDrawingDlg->HideWnd(TRUE);	//隐藏对话框
			DRAGSET.ClearEntSet();
			//选中视图名称
			pCurComponent->GetAttachEntSet(DRAGSET.drag_ent_set,1);
			//第一次生成视图需要指定插入点
			f2dRect rect=pCurComponent->GetDrawRegion(TRUE,TRUE);
			f3dPoint basepnt((rect.topLeft.x+rect.bottomRight.x)*0.5,rect.bottomRight.y-pCurComponent->m_fNamePosSpace);
			f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar100 sPrompt("Please input [%s] insertion point\n",&pCurComponent->m_sName);
#else
			CXhChar100 sPrompt("请输入[%s]插入点\n",&pCurComponent->m_sName);
#endif
			int nRetCode=DragEntSet(basepnt,sPrompt);
			if(nRetCode==RTNORM)
			{	//视图位置变动后才需要更新坐标系位置
				double fOffsetX=basepnt.x-old_pos.x;
				double fOffsetY=basepnt.y-old_pos.y;
				pCurComponent->m_fNamePosSpace-=fOffsetY;
				//更新界面
				CWnd* pMainWnd=AfxGetMainWnd();
				if(pMainWnd)	//存在pMainWnd=NULL的情况
					pMainWnd->SetFocus();
				actrTransactionManager->flushGraphics();
				acedUpdateDisplay();
			}
			long idProp=CSelectDrawingDlg::GetPropID("component.m_fNamePosSpace");
			if(pDrawingDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,"%s",&sText);
		}
		else if(pCurComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&CSelectDrawingDlg::GetPropID("component.m_sSectionViewCode")==pItem->m_idProp)
		{
			DRAGSET.ClearEntSet();
			CStruView *pStruView=(CStruView*)pCurComponent;
			CDrawing *pDrawing=pStruView->ParentDrawing();
			if(pDrawing==NULL)
				return FALSE;
			CSectionViewSymbol *pViewSymbol=NULL;
			pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol==NULL)
			{	//添加剖视图符
				CStruView *pMainView=NULL;
				CDrawingComponent *pComponent=NULL;
				for(pComponent=pDrawing->GetFirstDrawingComponent();pComponent;
					pComponent=pDrawing->GetNextDrawingComponent())
				{
					if(pComponent->m_eClassType==DRAW_COM_STRUVIEW&&pComponent->m_pAttachView
						&&pComponent->m_pAttachView->m_iViewType==0)
					{
						pMainView=(CStruView*)pComponent;
						break;
					}
				}
				if(pMainView==NULL)
				{	//尝试在同段号的图纸中查找主视图
					CDrawing *pMainDrawing=NULL;
					for(pMainDrawing=drawingList.GetFirst();pMainDrawing;pMainDrawing=drawingList.GetNext())
					{
						if(pMainDrawing->m_nSegI.iSeg!=pDrawing->m_nSegI.iSeg)
							continue;
						for(pComponent=pMainDrawing->GetFirstDrawingComponent();pComponent;
						pComponent=pMainDrawing->GetNextDrawingComponent())
						{
							if(pComponent->m_eClassType==DRAW_COM_STRUVIEW&&pComponent->m_pAttachView
								&&pComponent->m_pAttachView->m_iViewType==0)
							{
								pMainView=(CStruView*)pComponent;
								break;
							}
						}
					}
				}
				
				pDrawingDlg->HideWnd(TRUE);	//隐藏对话框
				if(pMainView)	
				{	//自动缩放至主视图所在区域
					f2dRect rect=pMainView->GetDrawRegion(TRUE);
					ZoomAcadView(rect,50);
				}
				CSectionViewSymbol::AddSectionViewSymbol(pMainView,pStruView);
			}
			else 
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(AfxMessageBox("Need to delete cutaway view symbol (Y), or move cutaway view symbol's position (N)?",MB_YESNO)==IDYES)
#else
				if(AfxMessageBox("需要删除剖视图符(Y),还是移动剖视图符位置(N)?",MB_YESNO)==IDYES)
#endif
				{	//删除剖视图符
					pViewSymbol->EraseRelativeAcadEntity();
					CSectionViewSymbol *pSymbol=NULL;
					for(pSymbol=pDrawing->sectionViewSymbols.GetFirst();pSymbol;
						pSymbol=pDrawing->sectionViewSymbols.GetNext())
					{
						if(pSymbol!=pViewSymbol)
							continue;
						pPropList->DeleteItemByPropId(CSelectDrawingDlg::GetPropID("component.SectionView.startPt"));
						pPropList->DeleteItemByPropId(CSelectDrawingDlg::GetPropID("component.SectionView.startPt.x"));
						pPropList->DeleteItemByPropId(CSelectDrawingDlg::GetPropID("component.SectionView.startPt.y"));
						pPropList->DeleteItemByPropId(CSelectDrawingDlg::GetPropID("component.SectionView.endPt"));
						pPropList->DeleteItemByPropId(CSelectDrawingDlg::GetPropID("component.SectionView.endPt.x"));
						pPropList->DeleteItemByPropId(CSelectDrawingDlg::GetPropID("component.SectionView.endPt.y"));
						pPropList->DeleteItemByPropId(CSelectDrawingDlg::GetPropID("component.SectionView.bUpturned"));
						pPropList->DeleteItemByPropId(CSelectDrawingDlg::GetPropID("component.SectionView.hAttachViewKey"));
						pDrawing->sectionViewSymbols.DeleteCursor(TRUE);
						break;
					}
				}
				else
				{
					pDrawingDlg->HideWnd(TRUE);	//隐藏对话框
					//选择剖视图相关实体集
					pViewSymbol->GetEntSet(DRAGSET.drag_ent_set);
					f3dPoint basepnt(pViewSymbol->startPt.x,pViewSymbol->startPt.y);
					f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
					CXhChar100 sPrompt("please input [%s-%s] starting position\n",(char*)pViewSymbol->viewName,(char*)pViewSymbol->viewName);
#else
					CXhChar100 sPrompt("请输入[%s-%s]起点位置\n",(char*)pViewSymbol->viewName,(char*)pViewSymbol->viewName);
#endif
					int nRetCode=DragEntSet(basepnt,sPrompt);
					if(nRetCode==RTNORM)
					{	//更新剖视图符位置
						double fOffsetX=basepnt.x-old_pos.x;
						double fOffsetY=basepnt.y-old_pos.y;
						pViewSymbol->startPt.x+=fOffsetX;
						pViewSymbol->startPt.y+=fOffsetY;
						pViewSymbol->endPt.x+=fOffsetX;
						pViewSymbol->endPt.y+=fOffsetY;
						//更新界面
						actrTransactionManager->flushGraphics();
						acedUpdateDisplay();
					}
				}
			}
		}
		else if(pCurComponent->m_eClassType==DRAW_COM_STRUVIEW&&
				CSelectDrawingDlg::GetPropID("component.SectionView.startPt")==pItem->m_idProp)
		{
			pDrawingDlg->HideWnd(TRUE);	//隐藏对话框
			DRAGSET.ClearEntSet();
			CStruView *pStruView=(CStruView*)pCurComponent;
			CSectionViewSymbol *pViewSymbol=NULL;
			pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol==NULL)
				return FALSE;
			//选择剖视图相关实体集
			pViewSymbol->GetEntSet(DRAGSET.drag_ent_set,0);
			//缩放至剖视图符始端位置
			SCOPE_STRU scope=pViewSymbol->GetEntScope(0);
			ZoomAcadView(scope,10);
			//
			f3dPoint basepnt(pViewSymbol->startPt.x,pViewSymbol->startPt.y);
			f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar100 sPrompt("please input [%s-%s] starting position\n",&pViewSymbol->viewName,&pViewSymbol->viewName);
#else
			CXhChar100 sPrompt("请输入[%s-%s]起点位置\n",&pViewSymbol->viewName,&pViewSymbol->viewName);
#endif
			int nRetCode=DragEntSet(basepnt,sPrompt);
			if(nRetCode==RTNORM)
			{	//视图位置变动后才需要更新坐标系位置
				double fOffsetX=basepnt.x-old_pos.x;
				double fOffsetY=basepnt.y-old_pos.y;
				pViewSymbol->startPt.x+=fOffsetX;
				pViewSymbol->startPt.y+=fOffsetY;
				//调整方向点位置重新生成剖视图符
				pViewSymbol->EraseRelativeAcadEntity();
				pViewSymbol->Draw();
				//更新界面
				actrTransactionManager->flushGraphics();
				acedUpdateDisplay();
			}
		}
		else if(pCurComponent->m_eClassType==DRAW_COM_STRUVIEW&&
			CSelectDrawingDlg::GetPropID("component.SectionView.endPt")==pItem->m_idProp)
		{
			pDrawingDlg->HideWnd(TRUE);	//隐藏对话框
			DRAGSET.ClearEntSet();
			CStruView *pStruView=(CStruView*)pCurComponent;
			CSectionViewSymbol *pViewSymbol=NULL;
			pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol==NULL)
				return FALSE;
			//选择剖视图相关实体集
			pViewSymbol->GetEntSet(DRAGSET.drag_ent_set,1);
			//缩放至剖视图符始端位置
			SCOPE_STRU scope=pViewSymbol->GetEntScope(1);
			ZoomAcadView(scope,10);
			f3dPoint basepnt(pViewSymbol->endPt.x,pViewSymbol->endPt.y);
			f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar100 sPrompt("please input[%s-%s]starting position\n",&pViewSymbol->viewName,&pViewSymbol->viewName);
#else
			CXhChar100 sPrompt("请输入[%s-%s]起点位置\n",&pViewSymbol->viewName,&pViewSymbol->viewName);
#endif
			int nRetCode=DragEntSet(basepnt,sPrompt);
			if(nRetCode==RTNORM)
			{	//视图位置变动后才需要更新坐标系位置
				double fOffsetX=basepnt.x-old_pos.x;
				double fOffsetY=basepnt.y-old_pos.y;
				pViewSymbol->endPt.x+=fOffsetX;
				pViewSymbol->endPt.y+=fOffsetY;
				//调整方向点位置重新生成剖视图符
				pViewSymbol->EraseRelativeAcadEntity();
				pViewSymbol->Draw();
				//更新界面
				actrTransactionManager->flushGraphics();
				acedUpdateDisplay();
			}
		}
		else if(CSelectDrawingDlg::GetPropID("component.dcs")==pItem->m_idProp)
		{
			UCS_STRU dcs=pCurComponent->GetDCS();
			CInputDrawingUcsDlg dlg;
			dlg.m_fAxisX_X = dcs.axis_x.x;
			dlg.m_fAxisX_Y = dcs.axis_x.y;
			dlg.m_fAxisX_Z = dcs.axis_x.z;
			dlg.m_fAxisZ_X = dcs.axis_z.x;
			dlg.m_fAxisZ_Y = dcs.axis_z.y;
			dlg.m_fAxisZ_Z = dcs.axis_z.z;
			dlg.m_fOriginPosX = dcs.origin.x;
			dlg.m_fOriginPosY = dcs.origin.y;
			dlg.m_fOriginPosZ = dcs.origin.z;
			if(dlg.DoModal()==IDOK)
			{
				dcs.origin.Set(dlg.m_fOriginPosX,dlg.m_fOriginPosY,dlg.m_fOriginPosZ);
				dcs.axis_x.Set(dlg.m_fAxisX_X,dlg.m_fAxisX_Y,dlg.m_fAxisX_Z);
				dcs.axis_z.Set(dlg.m_fAxisZ_X,dlg.m_fAxisZ_Y,dlg.m_fAxisZ_Z);
				dcs.axis_y=dcs.axis_z^dcs.axis_x;
				dcs.axis_x=dcs.axis_y^dcs.axis_z;
				normalize(dcs.axis_x);
				normalize(dcs.axis_y);
				normalize(dcs.axis_z);
				pCurComponent->SetDCS(dcs);
				//结构图对应的绘制坐标系发生变化后应及时更新构件绘制方式 wht 11-07-31
				if(pCurComponent->m_eClassType==DRAW_COM_STRUVIEW)	
					((CStruView*)pCurComponent)->UpdatePartDrawTypeAndSpecialDim();
			}
		}
	}
	return TRUE;
}

void CSelectDrawingDlg::DisplayDrawingProperty()
{
	//设置回调函数
	m_propList.CleanTree();
	m_propList.m_nObjClassTypeId = 0;
	m_propList.SetModifyValueFunc(ModifyDrawingValue);
	m_propList.SetButtonClickFunc(ButtonClickDrawing);
	m_propList.SetModifyStatusFunc(ModifyItemStatus);
	m_propList.SetPropHelpPromptFunc(FireSelChangeItem);
	//常规
	CPropertyListOper<CSelectDrawingDlg> oper(&m_propList,this);
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pSonPropItem=NULL,*pRootItem=m_propList.GetRootItem();
	//设置绘图比例
	CDrawing *pDrawing=NULL;
	CDrawingComponent *pDrawingComponent=NULL;
	if(m_pItemInfo&&m_pItemInfo->itemType==1)
		pDrawing=(CDrawing*)m_pItemInfo->dwRefData;
	else if(m_pItemInfo&&m_pItemInfo->itemType==2)
		pDrawingComponent=(CDrawingComponent*)m_pItemInfo->dwRefData;
	else
		oper.InsertPropItem(pRootItem,"drawing");	//图纸属性
	if(pDrawing)
	{
		pGroupItem=oper.InsertPropItem(pRootItem,"drawing");
		if(CSelectDrawingDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CSelectDrawingDlg::GetPropStatus(pGroupItem->m_idProp);
		//图纸名称
		oper.InsertEditPropItem(pGroupItem,"drawing.m_sDrawingName");
		//图纸编号
		oper.InsertEditPropItem(pGroupItem,"drawing.m_sDrawingNo");
		if(pDrawing->m_nSegI.iSeg==-1||pDrawing->m_nSegI.iSeg==-2)
		{	//当前激活呼高分组
			pPropItem=oper.InsertCmbListPropItem(pGroupItem,"drawing.m_hActiveModule");
			pPropItem->m_lpNodeInfo->m_cmbItems="";
			CLDSModule *pModule=NULL;
			for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
			{
				pPropItem->m_lpNodeInfo->m_cmbItems+=pModule->description;
				pPropItem->m_lpNodeInfo->m_cmbItems+="|";
			}
		}
		//图框类型
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"drawing.m_xMapFrm.style");
		//图框长度
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"drawing.m_xMapFrm.nLength");
		pSonPropItem->SetReadOnly();
		//图框宽度
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"drawing.m_xMapFrm.nWidth");
		pSonPropItem->SetReadOnly();
		//左侧间隙
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"drawing.m_xMapFrm.nLeftMargin");
		pSonPropItem->SetReadOnly();
		//其它侧间隙
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"drawing.m_xMapFrm.nOtherMargin");
		pSonPropItem->SetReadOnly();
		//右下侧固定长度
		oper.InsertEditPropItem(pPropItem,"drawing.m_xMapFrm.nBottomRightFixLen");
		//右下侧固定高度
		oper.InsertEditPropItem(pPropItem,"drawing.m_xMapFrm.nBottomRightFixWidth");
		//指定图框位置
		oper.InsertCmbListPropItem(pGroupItem,"drawing.m_bSpecifyMapFrmPos");
		//自动布局
		oper.InsertCmbListPropItem(pGroupItem,"drawing.m_bAutoLayoutComponent");
		//视图间隙
		oper.InsertEditPropItem(pGroupItem,"drawing.m_fComponentSpace");
		//线型
		if(pDrawing->m_nSegI.iSeg==-1)
			oper.InsertCmbListPropItem(pGroupItem,"LineView::m_bUsePolyLineChiefMap");
		if(pDrawing->m_nSegI.iSeg==-2)
			oper.InsertCmbListPropItem(pGroupItem,"LineView::m_bUsePolyLineSumMap");
	}
	else if(pDrawingComponent)
	{
		pGroupItem=oper.InsertPropItem(pRootItem,"component");
		if(CSelectDrawingDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CSelectDrawingDlg::GetPropStatus(pGroupItem->m_idProp);
		//类型
		pPropItem=oper.InsertEditPropItem(pGroupItem,"component.m_eClassType");
		pPropItem->SetReadOnly();
		if(pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW
			||pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW)
		{	
			if(pDrawingComponent->ParentDrawing()->m_nSegI.iSeg<0
				&&pDrawingComponent->m_eClassType!=DRAW_COM_STRUVIEW)
			{	//关联呼高分组
				pPropItem=oper.InsertEditPropItem(pGroupItem,"component.m_hModule");
				pPropItem->SetReadOnly();
			}
			if(pDrawingComponent->m_pAttachView&&pDrawingComponent->m_pAttachView->m_iViewType==0)
			{	//图层范围
				pPropItem=oper.InsertEditPropItem(pGroupItem,"component.m_sDrawingLayer");
				pPropItem->SetReadOnly();	//图层范围暂时设置为只读属性
			}
		}
		//绘图比例
		if(pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW
			||pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			||pDrawingComponent->m_eClassType==DRAW_COM_PARTVIEW)
			oper.InsertEditPropItem(pGroupItem,"component.m_fScale");
			
		//名称
		oper.InsertBtnEditPropItem(pGroupItem,"component.m_sName");
		if(pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW
			||pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW)
		{	//名称标注位置间隙
			pPropItem=oper.InsertEditPropItem(pGroupItem,"component.m_fNamePosSpace");
#ifdef AFX_TARG_ENU_ENGLISH
			if(pDrawingComponent->m_eClassType!=DRAW_COM_STRUVIEW)
				pPropItem->m_lpNodeInfo->m_strPropHelp.Format("clearance value between view name labelling position and the bottom of View, clearance value between segement No. on moduel and the top of view.");
			else 
				pPropItem->m_lpNodeInfo->m_strPropHelp.Format("View name at the bottom of the dimension position with the view of the clearance value.");
#else
			if(pDrawingComponent->m_eClassType!=DRAW_COM_STRUVIEW)
				pPropItem->m_lpNodeInfo->m_strPropHelp.Format("视图名称标注位置与视图底部的间隙值,呼高图上接段号标注位置与视图顶部的间隙值。");
			else 
				pPropItem->m_lpNodeInfo->m_strPropHelp.Format("视图名称标注位置与视图底部的间隙值。");
#endif
		}
		if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW&&pDrawingComponent->m_pAttachView
			&&pDrawingComponent->m_pAttachView->m_iViewType==1)
		{	//剖视图代号
			pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"component.m_sSectionViewCode");
			if(CSelectDrawingDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CSelectDrawingDlg::GetPropStatus(pPropItem->m_idProp);
			else
				pPropItem->m_bHideChildren = TRUE;
			//
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol)
			{	
				//剖视图符从属视图 调整视图顺序后可能导致关联视图错误,下拉框各项应支持关联数据。
				pSonPropItem=oper.InsertCmbListPropItem(pPropItem,"component.SectionView.hAttachViewKey");
				pSonPropItem->m_lpNodeInfo->m_cmbItems=pStruView->ParentDrawing()->MakeComponentSetStr();
				//方向
				oper.InsertCmbListPropItem(pPropItem,"component.SectionView.bUpturned");
				//起点
				pSonPropItem=oper.InsertButtonPropItem(pPropItem,"component.SectionView.startPt");
				if(CSelectDrawingDlg::GetPropStatus(pSonPropItem->m_idProp)!=-1)
					pSonPropItem->m_bHideChildren = CSelectDrawingDlg::GetPropStatus(pSonPropItem->m_idProp);
				else 
					pSonPropItem->m_bHideChildren=TRUE;
				//
				oper.InsertEditPropItem(pSonPropItem,"component.SectionView.startPt.x");
				//
				oper.InsertEditPropItem(pSonPropItem,"component.SectionView.startPt.y");
				//终点
				pSonPropItem=oper.InsertButtonPropItem(pPropItem,"component.SectionView.endPt");
				if(CSelectDrawingDlg::GetPropStatus(pSonPropItem->m_idProp)!=-1)
					pSonPropItem->m_bHideChildren = CSelectDrawingDlg::GetPropStatus(pSonPropItem->m_idProp);
				else 
					pSonPropItem->m_bHideChildren=TRUE;
				//
				oper.InsertEditPropItem(pSonPropItem,"component.SectionView.endPt.x");
				//
				oper.InsertEditPropItem(pSonPropItem,"component.SectionView.endPt.y");
			}
		}
		//图框类型
		oper.InsertCmbListPropItem(pGroupItem,"component.m_nAnchorType");
		//指定视图位置
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"component.m_bSpecifyComPos");
		pPropItem->SetReadOnly(pDrawingComponent->ParentDrawing()->m_bAutoLayoutComponent);
		if(!pDrawingComponent->m_bSpecifyComPos)
		{	//
#ifdef AFX_TARG_ENU_ENGLISH
			char sAnchorType[50]="location";
			if(pDrawingComponent->m_nAnchorType==0)
				strcpy(sAnchorType,"Top left corner");
			else if(pDrawingComponent->m_nAnchorType==1)
				strcpy(sAnchorType,"Top right corner");
			else if(pDrawingComponent->m_nAnchorType==2)
				strcpy(sAnchorType,"Lower left quarter");
			else if(pDrawingComponent->m_nAnchorType==3)
				strcpy(sAnchorType,"Lower right corner");
#else
			char sAnchorType[50]="位置";
			if(pDrawingComponent->m_nAnchorType==0)
				strcpy(sAnchorType,"左上角");
			else if(pDrawingComponent->m_nAnchorType==1)
				strcpy(sAnchorType,"右上角");
			else if(pDrawingComponent->m_nAnchorType==2)
				strcpy(sAnchorType,"左下角");
			else if(pDrawingComponent->m_nAnchorType==3)
				strcpy(sAnchorType,"右下角");
#endif
			//
			pPropItem=oper.InsertButtonPropItem(pGroupItem,"component.m_ptLocation");
#ifdef AFX_TARG_ENU_ENGLISH
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("view%s offset position relative to the top left corner of drawing frame.",sAnchorType);
#else
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("视图%s相对于图框左上角的偏移位置。",sAnchorType);
#endif
			pPropItem->m_bHideChildren=TRUE;
			pPropItem->SetReadOnly(pDrawingComponent->ParentDrawing()->m_bAutoLayoutComponent);
			//
			pSonPropItem=oper.InsertEditPropItem(pPropItem,"component.m_ptLocation.x");
#ifdef AFX_TARG_ENU_ENGLISH
			pSonPropItem->m_lpNodeInfo->m_strPropHelp.Format("view%s offset position in X direction relative to the top left corner of drawing frame.",sAnchorType);
#else
			pSonPropItem->m_lpNodeInfo->m_strPropHelp.Format("视图%s相对于图框左上角X方向的偏移值。",sAnchorType);
#endif
			pSonPropItem->SetReadOnly(pDrawingComponent->ParentDrawing()->m_bAutoLayoutComponent);
			//
			pSonPropItem=oper.InsertEditPropItem(pPropItem,"component.m_ptLocation.y");
#ifdef AFX_TARG_ENU_ENGLISH
			pSonPropItem->m_lpNodeInfo->m_strPropHelp.Format("view%s offset position in Y direction relative to the top left corner of drawing frame.",sAnchorType);
#else
			pSonPropItem->m_lpNodeInfo->m_strPropHelp.Format("视图%s相对于图框左上角Y方向的偏移值。",sAnchorType);
#endif
			pSonPropItem->SetReadOnly(pDrawingComponent->ParentDrawing()->m_bAutoLayoutComponent);
		}
		//
		if(pDrawingComponent->m_eClassType!=DRAW_COM_BOM)
		{	//绘图坐标系
			oper.InsertButtonPropItem(pGroupItem,"component.dcs");
		}
		if(pDrawingComponent->m_eClassType==DRAW_COM_BOM
			&&((CDrawingBOM*)pDrawingComponent)->m_eBOMType==BOM_SEGPART)
		{	//分段构件明细表支持自动分页功能
			oper.InsertCmbListPropItem(pGroupItem,"drawingBOM.m_bAutoNewTable");
			//
			CDrawingBOM *pDrawingBOM=(CDrawingBOM*)pDrawingComponent;
			if(pDrawingBOM->m_bAutoNewTable)
				oper.InsertCmbListPropItem(pGroupItem,"drawingBOM.m_bHasTableHead");
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_BOM
			&&((CDrawingBOM*)pDrawingComponent)->m_eBOMType==BOM_GUIGE)
		{	//简化规格表---简化规格数 
			oper.InsertButtonPropItem(pGroupItem,"drawingBOM.nBriefGuigeN");
		}
		if(pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW
			&&((CLineView*)pDrawingComponent)->m_bAnalysisMode)
		{	//标注节点编号
			oper.InsertCmbListPropItem(pGroupItem,"lineView.m_bDimNodeNo");
			//
			oper.InsertCmbListPropItem(pGroupItem,"lineView.m_bDimPoleGuige");
		}
		if(pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW
			||pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			||pDrawingComponent->m_eClassType==DRAW_COM_PARTVIEW)
		{
			//标注绘制比例
			oper.InsertCmbListPropItem(pGroupItem,"component.m_bDimScale");
		}
		//标注视图名称
		oper.InsertCmbListPropItem(pGroupItem,"component.m_bDimName");
		//
		if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW)
		{
			pPropItem=oper.InsertEditPropItem(pGroupItem,"struView.partNo");
			pPropItem->m_bHideChildren = FALSE;
			//构件明细对称方式 0:X轴对称；1:Y轴对称;其余不对称处理
			oper.InsertCmbListPropItem(pPropItem,"struView.partNo.m_iMirMsg");
			oper.InsertCmbListPropItem(pPropItem,"struView.partNo.m_bDrawRefLine");
		}
		if(pDrawingComponent->m_eClassType==DRAW_COM_PARTVIEW)
		{	//零件图
			oper.InsertEditPropItem(pGroupItem,"PartView.sPartNo");
			pPropItem=oper.InsertCmbEditPropItem(pGroupItem,"PartView.hPart");
			pPropItem->m_lpNodeInfo->m_cmbItems=MakePartHandleSetString(((CPartDrawing*)pDrawingComponent)->sPartNo);
		}
	}
	else if(m_pItemInfo&&m_pItemInfo->itemType==1)
	{	//结构图目录项 显示结构图公共属性
		pGroupItem=oper.InsertPropItem(pRootItem,"StruView");
		if(CSelectDrawingDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CSelectDrawingDlg::GetPropStatus(pGroupItem->m_idProp);
		//
		oper.InsertCmbListPropItem(pGroupItem,"StruView::m_bDimLsSpace");
		//
		oper.InsertCmbListPropItem(pGroupItem,"StruView::m_bDimJgZhunJu");
		//
		oper.InsertCmbListPropItem(pGroupItem,"StruView::m_bDimFootNailSpace");
		//
		oper.InsertCmbListPropItem(pGroupItem,"StruView::m_bDimFillPlateSpace");
		//
		oper.InsertCmbListPropItem(pGroupItem,"StruView::m_bDimPartSpec");
		//
		oper.InsertCmbListPropItem(pGroupItem,"StruView::m_bDimPoleOddment");
		//
		oper.InsertCmbListPropItem(pGroupItem,"StruView::m_bDimSimPoly");
		//
		oper.InsertCmbListPropItem(pGroupItem,"StruView::m_bDimBoltInfo");
		//
		oper.InsertCmbListPropItem(pGroupItem,"StruView::m_bDimBoltPad");
	}
	m_propList.Redraw();
}
int CSelectDrawingDlg::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{	
	CXhChar100 sText;
	CDrawing *pDrawing=NULL;
	CDrawingComponent *pDrawingComponent=NULL;
	if(m_pItemInfo&&m_pItemInfo->itemType==1)
		pDrawing=(CDrawing*)m_pItemInfo->dwRefData;
	else if(m_pItemInfo&&m_pItemInfo->itemType==2)
		pDrawingComponent=(CDrawingComponent*)m_pItemInfo->dwRefData;
	else 
		return 0;
	if(pDrawing)
	{
		if(GetPropID("drawing.m_nSegI")==id)
			sText.Printf("%s",(char*)pDrawing->m_nSegI.ToString());
		else if(GetPropID("drawing.m_xMapFrm.style")==id)
		{
			sText.Printf("%s",(char*)pDrawing->m_xMapFrm.sMapFrameName);
		}
		else if(GetPropID("drawing.m_xMapFrm.nLength")==id)
		{
			sText.Printf("%d",pDrawing->m_xMapFrm.nLength);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.m_xMapFrm.nWidth")==id)
		{
			sText.Printf("%d",pDrawing->m_xMapFrm.nWidth);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.m_xMapFrm.nLeftMargin")==id)
		{
			sText.Printf("%d",pDrawing->m_xMapFrm.nLeftMargin);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.m_xMapFrm.nOtherMargin")==id)
		{
			sText.Printf("%d",pDrawing->m_xMapFrm.nTopMargin);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.m_xMapFrm.nBottomRightFixLen")==id)
		{
			sText.Printf("%d",pDrawing->m_xMapFrm.nBottomRightFixLen);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.m_xMapFrm.nBottomRightFixWidth")==id)
		{
			sText.Printf("%d",pDrawing->m_xMapFrm.nBottomRightFixWidth);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.m_sDrawingName")==id)
			sText.Printf("%s",(char*)pDrawing->m_sDrawingName);
		else if(GetPropID("drawing.m_sDrawingNo")==id)
			sText.Printf("%s",(char*)pDrawing->m_sDrawingNo);
		else if(GetPropID("drawing.m_bSpecifyMapFrmPos")==id)
		{
			if(pDrawing->m_bSpecifyMapFrmPos)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else 
				sText.Copy("No");
#else
				sText.Copy("是");
			else 
				sText.Copy("否");
#endif
		}
		else if(GetPropID("drawing.m_bAutoLayoutComponent")==id)
		{
			if(pDrawing->m_bAutoLayoutComponent)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else 
				sText.Copy("No");
#else
				sText.Copy("是");
			else 
				sText.Copy("否");
#endif
		}
		else if(GetPropID("drawing.m_fComponentSpace")==id)
		{
			sText.Printf("%f",pDrawing->m_fComponentSpace);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.m_hActiveModule")==id)
		{
			CLDSModule *pModule=NULL;
			for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
			{
				if(pModule->handle==pDrawing->m_hActiveModule)
				{
					sText.Printf("%s",pModule->description);
					break;
				}
			}
		}
		else if(GetPropID("drawing.dcs.origin.x")==id)
		{
			sText.Printf("%f",pDrawing->dcs.origin.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.dcs.origin.y")==id)
		{
			sText.Printf("%f",pDrawing->dcs.origin.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.dcs.origin.z")==id)
		{
			sText.Printf("%f",pDrawing->dcs.origin.z);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.dcs.axis_x.x")==id)
		{
			sText.Printf("%f",pDrawing->dcs.axis_x.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.dcs.axis_x.y")==id)
		{
			sText.Printf("%f",pDrawing->dcs.axis_x.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.dcs.axis_x.z")==id)
		{
			sText.Printf("%f",pDrawing->dcs.axis_x.z);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.dcs.axis_y.x")==id)
		{
			sText.Printf("%f",pDrawing->dcs.axis_y.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.dcs.axis_y.y")==id)
		{
			sText.Printf("%f",pDrawing->dcs.axis_y.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.dcs.axis_y.z")==id)
		{
			sText.Printf("%f",pDrawing->dcs.axis_y.z);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.dcs.axis_z.x")==id)
		{
			sText.Printf("%f",pDrawing->dcs.axis_z.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.dcs.axis_z.y")==id)
		{
			sText.Printf("%f",pDrawing->dcs.axis_z.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("drawing.dcs.axis_z.z")==id)
		{
			sText.Printf("%f",pDrawing->dcs.axis_y.z);
			SimplifiedNumString(sText);
		}
		//多段线绘制司令图
		else if(GetPropID("LineView::m_bUsePolyLineChiefMap")==id)
		{
			if(CLineView::m_bUsePolyLineChiefMap)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Polygon line");
			else 
				sText.Copy("Ordinary straight line");
#else
				sText.Copy("多段线");
			else 
				sText.Copy("普通直线");
#endif
		}
		//多段线绘制总图
		else if(GetPropID("LineView::m_bUsePolyLineSumMap")==id)
		{
			if(CLineView::m_bUsePolyLineSumMap)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Polygon line");
			else 
				sText.Copy("Ordinary straight line");
#else
				sText.Copy("多段线");
			else 
				sText.Copy("普通直线");
#endif
		}
	}
	else if(pDrawingComponent)
	{
		if(GetPropID("component.m_eClassType")==id)
		{
			if(pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW)
			{
				if(((CLineView*)pDrawingComponent)->m_bAnalysisMode)
#ifdef AFX_TARG_ENU_ENGLISH
					sText.Copy("Force line map");
				else 
					sText.Copy("Single line map");
			}
			else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW)
				sText.Copy("Structure map");
			else if(pDrawingComponent->m_eClassType==DRAW_COM_STDVIEW)
				sText.Copy("Standard map");
			else if(pDrawingComponent->m_eClassType==DRAW_COM_PARTVIEW)
				sText.Copy("Part map");
			else if(pDrawingComponent->m_eClassType==DRAW_COM_BOM)
				sText.Copy("List of materials");
			else
				sText.Copy("Others");
#else
					sText.Copy("司令图");
				else 
					sText.Copy("单线图");
			}
			else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW)
				sText.Copy("结构图");
			else if(pDrawingComponent->m_eClassType==DRAW_COM_STDVIEW)
				sText.Copy("标准图");
			else if(pDrawingComponent->m_eClassType==DRAW_COM_PARTVIEW)
				sText.Copy("零件图");
			else if(pDrawingComponent->m_eClassType==DRAW_COM_BOM)
				sText.Copy("物料清单");
			else
				sText.Copy("其他");
#endif
		}
		else if(GetPropID("component.m_hModule")==id)
		{	//视图关联呼高
			if(pDrawingComponent->m_pModule==NULL)
				pDrawingComponent->m_pModule=Ta.Module.FromHandle(pDrawingComponent->m_hModule);
			if(pDrawingComponent->m_pModule)
				sText.Copy(pDrawingComponent->m_pModule->description);
		}
		else if(GetPropID("component.m_bNeedAdjust")==id)
		{
			if(pDrawingComponent->m_bNeedAdjust)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else
				sText.Copy("No");
#else
				sText.Copy("是");
			else
				sText.Copy("否");
#endif
		}
		else if(GetPropID("component.m_bSpecifyComPos")==id)
		{
			if(pDrawingComponent->m_bSpecifyComPos)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else
				sText.Copy("No");
#else
				sText.Copy("是");
			else
				sText.Copy("否");
#endif
		}
		else if(GetPropID("component.m_sDrawingLayer")==id)
			sText.Printf("%s",(char*)pDrawingComponent->m_sDrawingLayer);
		else if(GetPropID("component.rect")==id)
		{
			f2dRect rect=pDrawingComponent->GetComponentRect();
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%.f×%.f",rect.Width(),rect.Height());
#else
			sText.Printf("%.f×%.f",rect.Width(),rect.Height());
#endif
		}
		else if(GetPropID("component.m_sName")==id)
			sText.Printf("%s",(char*)pDrawingComponent->m_sName);
		else if(GetPropID("component.m_sSectionViewCode")==id)
			sText.Printf("%s",(char*)pDrawingComponent->m_sSectionViewCode);
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&GetPropID("component.SectionView.hAttachViewKey")==id)
		{
			CDrawingComponent *pCom=NULL;
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CDrawing *pDrawing=pStruView->ParentDrawing();
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol)
			{
				long index=0;
				for(pCom=pDrawing->GetFirstDrawingComponent();pCom;pCom=pDrawing->GetNextDrawingComponent())
				{
					long key=pDrawing->listComponents.GetCursorKey();
					if(key==pViewSymbol->hAttachViewKey)
						break;
					index++;
				}
				if(pCom)
					sText.Printf("%d.%s",index,&pCom->m_sName);
			}
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&GetPropID("component.SectionView.bUpturned")==id)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol&&pViewSymbol->bUpturned)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Upwards");
			else 
				sText.Copy("Downwards");
#else
				sText.Copy("朝上");
			else 
				sText.Copy("朝下");
#endif
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&GetPropID("component.SectionView.startPt.x")==id)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol)
				sText.Printf("%lf",pViewSymbol->startPt.x);
			SimplifiedNumString(sText);
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&GetPropID("component.SectionView.startPt.y")==id)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol)
				sText.Printf("%lf",pViewSymbol->startPt.y);
			SimplifiedNumString(sText);
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&GetPropID("component.SectionView.endPt.x")==id)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol)
				sText.Printf("%lf",pViewSymbol->endPt.x);
			SimplifiedNumString(sText);
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&GetPropID("component.SectionView.endPt.y")==id)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol)
				sText.Printf("%lf",pViewSymbol->endPt.y);
			SimplifiedNumString(sText);
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&GetPropID("struView.partNo.m_iMirMsg")==id)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			if(pStruView->m_iMirMsg==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("0.X Axis Mirror");
			else if(pStruView->m_iMirMsg==1)
				sText.Printf("1.Y Axis Mirror");
			else 
				sText.Printf("2.No Mirror");
#else
				sText.Printf("0.X轴对称");
			else if(pStruView->m_iMirMsg==1)
				sText.Printf("1.Y轴对称");
			else 
				sText.Printf("2.无对称");
#endif
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&GetPropID("struView.partNo.m_bDrawRefLine")==id)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			if(!pStruView->m_bDrawRefLine)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else 
				sText.Printf("No");
#else
				sText.Printf("是");
			else 
				sText.Printf("否");
#endif
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_PARTVIEW&&GetPropID("PartView.sPartNo")==id)
			sText.Copy(((CPartDrawing*)pDrawingComponent)->sPartNo);
		else if(pDrawingComponent->m_eClassType==DRAW_COM_PARTVIEW&&GetPropID("PartView.hPart")==id)
			sText.Printf("0X%X",((CPartDrawing*)pDrawingComponent)->m_hPart);
		/*else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&GetPropID("component.SectionView.startPt")==id)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol)
				sText.Printf("%.1f,%.1f",pViewSymbol->startPt.x,pViewSymbol->startPt.y);
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			&&GetPropID("component.SectionView.endPt")==id)
		{
			CStruView *pStruView=(CStruView*)pDrawingComponent;
			CSectionViewSymbol *pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol)
				sText.Printf("%.1f,%.1f",pViewSymbol->endPt.x,pViewSymbol->endPt.y);
		}*/
		else if(GetPropID("component.m_fScale")==id)
		{
			sText.Printf("%.0f",1.0/pDrawingComponent->m_fScale);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("component.m_fNamePosSpace")==id)
		{
			sText.Printf("%f",pDrawingComponent->m_fNamePosSpace);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("component.m_nAnchorType")==id)
		{
			if(pDrawingComponent->m_nAnchorType==TOP_LEFT)			//左上角
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("0.Top left corner");
			else if(pDrawingComponent->m_nAnchorType==TOP_RIGHT)	//top right corner
				sText.Printf("1.Top right corner");
			else if(pDrawingComponent->m_nAnchorType==BOTTOM_LEFT)	//lower left corner
				sText.Printf("2.Lower left quarter");
			else //if(pDrawingComponent->m_nAnchorType==BOTTOM_RIGHT)//lower right corner
				sText.Printf("3.Lower right corner");
#else
				sText.Printf("0.左上角");
			else if(pDrawingComponent->m_nAnchorType==TOP_RIGHT)	//右上角
				sText.Printf("1.右上角");
			else if(pDrawingComponent->m_nAnchorType==BOTTOM_LEFT)	//左下角
				sText.Printf("2.左下角");
			else //if(pDrawingComponent->m_nAnchorType==BOTTOM_RIGHT)//右下角
				sText.Printf("3.右下角");
#endif
		}
		else if(GetPropID("component.m_ptLocation.x")==id)
		{
			sText.Printf("%f",pDrawingComponent->GetAnchorPointRelativePos().x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("component.m_ptLocation.y")==id)
		{
			sText.Printf("%f",pDrawingComponent->GetAnchorPointRelativePos().y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("component.m_ptLocation.z")==id)
		{
			sText.Printf("%f",pDrawingComponent->GetAnchorPointRelativePos().z);
			SimplifiedNumString(sText);
		}
		/*else if(GetPropID("component.dcs.origin.x")==id)
		{
			sText.Printf("%f",pDrawingComponent->dcs.origin.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("component.dcs.origin.y")==id)
		{
			sText.Printf("%f",pDrawingComponent->dcs.origin.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("component.dcs.origin.z")==id)
		{
			sText.Printf("%f",pDrawingComponent->dcs.origin.z);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("component.dcs.axis_x.x")==id)
		{
			sText.Printf("%f",pDrawingComponent->dcs.axis_x.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("component.dcs.axis_x.y")==id)
		{
			sText.Printf("%f",pDrawingComponent->dcs.axis_x.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("component.dcs.axis_x.z")==id)
		{
			sText.Printf("%f",pDrawingComponent->dcs.axis_x.z);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("component.dcs.axis_y.x")==id)
		{
			sText.Printf("%f",pDrawingComponent->dcs.axis_y.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("component.dcs.axis_y.y")==id)
		{
			sText.Printf("%f",pDrawingComponent->dcs.axis_y.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("component.dcs.axis_y.z")==id)
		{
			sText.Printf("%f",pDrawingComponent->dcs.axis_y.z);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("component.dcs.axis_z.x")==id)
		{
			sText.Printf("%f",pDrawingComponent->dcs.axis_z.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("component.dcs.axis_z.y")==id)
		{
			sText.Printf("%f",pDrawingComponent->dcs.axis_z.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("component.dcs.axis_z.z")==id)
		{
			sText.Printf("%f",pDrawingComponent->dcs.axis_y.z);
			SimplifiedNumString(sText);
		}*/
		else if(GetPropID("drawingBOM.nBriefGuigeN")==id
			&&pDrawingComponent->m_eClassType==DRAW_COM_BOM)
		{
			CDrawingBOM *pDrawingBOM=(CDrawingBOM*)pDrawingComponent;
			pDrawingBOM->m_nBriefGuigeN=sys.nBriefGuigeN=min(pDrawingBOM->SpecList.GetNodeNum(),26);
			sText.Printf("%d",pDrawingBOM->m_nBriefGuigeN);
		}
		else if(GetPropID("lineView.m_bDimNodeNo")==id
			&&pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW)
		{
			CLineView *pLineView=(CLineView*)pDrawingComponent;
			if(pLineView->m_bAnalysisMode)
			{
				if(pLineView->m_bDimNodeNo)
#ifdef AFX_TARG_ENU_ENGLISH
					sText.Copy("Yes");
				else
					sText.Copy("No");
#else
					sText.Copy("是");
				else
					sText.Copy("否");
#endif
			}
		}
		else if(GetPropID("lineView.m_bDimPoleGuige")==id
			&&pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW)
		{
			CLineView *pLineView=(CLineView*)pDrawingComponent;
			if(pLineView->m_bAnalysisMode)
			{
				if(pLineView->m_bDimPoleGuige)
#ifdef AFX_TARG_ENU_ENGLISH
					sText.Copy("Yes");
				else
					sText.Copy("No");
#else
					sText.Copy("是");
				else
					sText.Copy("否");
#endif
			}
		}
		else if(GetPropID("drawingBOM.m_bAutoNewTable")==id
			&&pDrawingComponent->m_eClassType==DRAW_COM_BOM)
		{
			CDrawingBOM *pDrawingBOM=(CDrawingBOM*)pDrawingComponent;
			if(pDrawingBOM->m_bAutoNewTable)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else
				sText.Copy("No");
#else
				sText.Copy("是");
			else
				sText.Copy("否");
#endif
		}
		else if(GetPropID("drawingBOM.m_bHasTableHead")==id
			&&pDrawingComponent->m_eClassType==DRAW_COM_BOM)
		{
			CDrawingBOM *pDrawingBOM=(CDrawingBOM*)pDrawingComponent;
			if(pDrawingBOM->m_bHasTableHead)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else
				sText.Copy("No");
#else
				sText.Copy("是");
			else
				sText.Copy("否");
#endif
		}
		else if(GetPropID("component.m_bDimScale")==id)
		{
			if(pDrawingComponent->m_bDimScale)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else
				sText.Copy("No");
#else
				sText.Copy("是");
			else
				sText.Copy("否");
#endif
		}
		else if(GetPropID("component.m_bDimName")==id)
		{
			if(pDrawingComponent->m_bDimName)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else
				sText.Copy("No");
#else
				sText.Copy("是");
			else
				sText.Copy("否");
#endif
		}
	}
	//结构图公共变量
	else if(GetPropID("StruView::m_bDimPartSpec")==id)
	{
		if(CStruView::m_bDimPartSpec)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("Yes");
		else 
			sText.Copy("No");
#else
			sText.Copy("是");
		else 
			sText.Copy("否");
#endif
	}
	else if(GetPropID("StruView::m_bDimPoleOddment")==id)
	{
		if(CStruView::m_bDimPoleOddment)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("Yes");
		else 
			sText.Copy("No");
#else
			sText.Copy("是");
		else 
			sText.Copy("否");
#endif
	}
	else if(GetPropID("StruView::m_bDimFootNailSpace")==id)
	{
		if(CStruView::m_bDimFootNailSpace)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("Yes");
		else 
			sText.Copy("No");
#else
			sText.Copy("是");
		else 
			sText.Copy("否");
#endif
	}
	else if(GetPropID("StruView::m_bDimFillPlateSpace")==id)
	{
		if(CStruView::m_bDimFillPlateSpace)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("Yes");
		else 
			sText.Copy("No");
#else
			sText.Copy("是");
		else 
			sText.Copy("否");
#endif
	}
	else if(GetPropID("StruView::m_bDimLsSpace")==id)
	{
		if(CStruView::m_bDimLsSpace)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("Yes");
		else 
			sText.Copy("No");
#else
			sText.Copy("是");
		else 
			sText.Copy("否");
#endif
	}
	else if(GetPropID("StruView::m_bDimJgZhunJu")==id)
	{
		if(CStruView::m_bDimJgZhunJu)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("Yes");
		else 
			sText.Copy("No");
#else
			sText.Copy("是");
		else 
			sText.Copy("否");
#endif
	}
	else if(GetPropID("StruView::m_bDimBoltInfo")==id)
	{
		if(CStruView::m_bDimBoltInfo)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("Yes");
		else 
			sText.Copy("No");
#else
			sText.Copy("是");
		else 
			sText.Copy("否");
#endif
	}
	else if(GetPropID("StruView::m_bDimBoltPad")==id)
	{
		if(CStruView::m_bDimBoltPad)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("Yes");
		else 
			sText.Copy("No");
#else
			sText.Copy("是");
		else 
			sText.Copy("否");
#endif
	}
	else if(GetPropID("StruView::m_bDimSimPoly")==id)
	{
		if(CStruView::m_bDimSimPoly)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("Yes");
		else 
			sText.Copy("No");
#else
			sText.Copy("是");
		else 
			sText.Copy("否");
#endif
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}

void CSelectDrawingDlg::OnDblclkTreeDrawing(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/*HTREEITEM hItem=m_treeDrawing.GetSelectedItem();	
	//调整当前项的所有兄弟节点处理合拢状态
	HTREEITEM hParentItem=m_treeDrawing.GetParentItem(hItem);
	if(hParentItem==NULL)
	{
		HTREEITEM hSiblingItem = m_treeDrawing.GetRootItem();
		while(hSiblingItem)
		{
			if(hSiblingItem!=hItem)
				m_treeDrawing.Expand(hSiblingItem,TVE_COLLAPSE);
			hSiblingItem=m_treeDrawing.GetNextSiblingItem(hSiblingItem);
		}
	}
	else
	{
		HTREEITEM hSonItem=m_treeDrawing.GetChildItem(hParentItem);
		while(hSonItem)
		{
			if(hSonItem!=hItem)
				m_treeDrawing.Expand(hSonItem,TVE_COLLAPSE);
			hSonItem=m_treeDrawing.GetNextSiblingItem(hSonItem);
		}
	}*/
	*pResult = 0;
}

void CSelectDrawingDlg::UpdateDrawingList(BOOL bReset)
{
	SEGI nSegI;
	long hComKey=0;
	CXhChar50 sDrawingName;
	if(m_pDrawing)
	{	//记录当前图纸的段号及名称,再次打开时选中该图纸 wht 11-06-17
		nSegI=m_pDrawing->m_nSegI;
		sDrawingName.Copy(m_pDrawing->m_sDrawingName);
		if(m_pComponent)
			hComKey=m_pDrawing->listComponents.GetKey(m_pComponent);
	}
	if(bReset)
	{	//重置图纸列表
		drawingList.Empty();	//清空图纸链表
		InitDrawingList();		//重新初始化图纸链表
	}
	else 
	{	//更新图纸列表
		CDrawing *pDrawing=NULL;
		for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
			pDrawing->UpdateCmdList();
	}
	if(sDrawingName.GetLength()>0)
	{	//根据图纸名称及段号选中保存文件之前的图纸
		CDrawing *pDrawing=NULL;
		for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
		{
			if(stricmp(pDrawing->m_sDrawingName,sDrawingName)==0&&pDrawing->m_nSegI.iSeg==nSegI.iSeg)
			{
				m_pDrawing=pDrawing;
				break;
			}
		}
	}
	if(m_pDrawing)
		m_pComponent=(CDrawingComponent*)m_pDrawing->listComponents.GetValue(hComKey);
	RefreshDrawingTree(m_pDrawing,m_pComponent);	//刷新左侧图纸列表树
}

//From DrawMapScript.cpp
void SaveScriptToBuffer();
//From DataFile.cpp
void SaveTma();
void CSelectDrawingDlg::OnSaveScript() 
{
	SEGI nSegI;
	long hComKey=0;
	CXhChar50 sDrawingName;
	if(m_pDrawing)
	{	//记录当前图纸的段号及名称,再次打开时选中该图纸 wht 11-06-17
		nSegI=m_pDrawing->m_nSegI;
		sDrawingName.Copy(m_pDrawing->m_sDrawingName);
		if(m_pComponent)
			hComKey=m_pDrawing->listComponents.GetKey(m_pComponent);
	}
	Ta.AttachBuffer[CTower::SMART_DRAW].ClearBuffer();	//清空脚本缓存
	//保存绘图脚本到Ta.m_bufferDrawingScript
	//设定增长额度为5M左右,以防频繁分配释放内存(易导致内存碎片并增加机时) wjh-2011.8.6
	Ta.AttachBuffer[CTower::SMART_DRAW].SetBlockSize(5000000);
	SaveScriptToBuffer();
	SaveTma();	//保存文件
	if(sDrawingName.GetLength()>0)
	{	//根据图纸名称及段号选中保存文件之前的图纸
		CDrawing *pDrawing=NULL;
		for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
		{
			if(stricmp(pDrawing->m_sDrawingName,sDrawingName)==0&&pDrawing->m_nSegI.iSeg==nSegI.iSeg)
			{
				m_pDrawing=pDrawing;
				break;
			}
		}
	}
	if(m_pDrawing)
		m_pComponent=(CDrawingComponent*)m_pDrawing->listComponents.GetValue(hComKey);
	//RefreshDrawingTree(m_pDrawing,m_pComponent);	//刷新左侧图纸列表树
}

void CSelectDrawingDlg::OnClickTreeDrawing(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TVHITTESTINFO HitTestInfo;
	GetCursorPos(&HitTestInfo.pt);
	m_treeDrawing.ScreenToClient(&HitTestInfo.pt);
	if(m_treeDrawing.HitTest(&HitTestInfo)==NULL)
		return;
	m_treeDrawing.Select(HitTestInfo.hItem,TVGN_CARET);       
	if(HitTestInfo.flags&TVHT_ONITEMSTATEICON)	//点击在CheckBox上       
	{       
		BOOL bCheck=m_treeDrawing.GetCheck(HitTestInfo.hItem);
		HTREEITEM hChildItem=m_treeDrawing.GetChildItem(HitTestInfo.hItem);
		while(hChildItem)
		{
			m_treeDrawing.SetCheck(hChildItem,!bCheck);
			hChildItem=m_treeDrawing.GetNextItem(hChildItem,TVGN_NEXT);
		}
	}  
	//if(HitTestInfo.flags&TVHT_ONITEMBUTTON)//点击在+号上
	*pResult = 0;
}

void CSelectDrawingDlg::OnKeydownTreeDrawing(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	if(pTVKeyDown->wVKey==VK_DELETE)
		OnDeleteItem();
	*pResult = 0;
}

BOOL CSelectDrawingDlg::Create()
{
	HWND hWnd = adsw_acadMainWnd();
	if(GetSafeHwnd()==0)
		CDialog::Create(CSelectDrawingDlg::IDD,CWnd::FromHandle(hWnd));
	OnInitDialog();
	m_bModeless=TRUE;	//非模态
	return TRUE;
}

void CSelectDrawingDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	if(m_bModeless)
		delete this;	//删除自己所占资源
	else 
		ReleaseCapture();
	if(m_bRestartDlg)
	{	//PostNcDestroy()何时调用，如何触发? 多次DoModal后控件的DrawItem函数无法响应? wht 11-03-16
		m_bRestartDlg=FALSE;
		DoModal();	
	}
}

LRESULT CSelectDrawingDlg::OnAcadKeepFocus(WPARAM, LPARAM)
{	//保证非模态对话框可以获得焦点
	return m_bCanKeepFocus;
}

//From DrawMapScript.cpp
BOOL FireRunCommand(CDialog *pDlg);
void CSelectDrawingDlg::OnOK() 
{
	
}
void CSelectDrawingDlg::OnCreateDrawing() 
{
	if(!m_bModeless)
	{
		KillTimer(1);
		if(GetCapture())
			ReleaseCapture();
		CDialog::OnOK();
	}
}

void CSelectDrawingDlg::HideWnd(BOOL bRestart)
{
	KillTimer(1);
	if(GetCapture())
		ReleaseCapture();
	m_bRestartDlg=bRestart;
	CDialog::OnCancel();
}

void CSelectDrawingDlg::OnCancel() 
{
	if(m_bModeless)
		ShowWindow(SW_HIDE);	//隐藏对话框
	else 
		HideWnd(FALSE);
}

#ifdef _WIN64
void CSelectDrawingDlg::OnTimer(UINT_PTR nIDEvent) 
#else
void CSelectDrawingDlg::OnTimer(UINT nIDEvent) 
#endif
{
	POINT pt;
	GetCursorPos(&pt);
	RECT rect;
	GetWindowRect(&rect);
	if (!PtInRect(&rect,pt)&&!GetCapture())
	{
		SetCapture();
		KillTimer(1);
		SetTimer(1,300,NULL);//reset timer
	}		
	CDialog::OnTimer(nIDEvent);
}

void CSelectDrawingDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetClientRect(&rect);
	if(!rect.PtInRect(point))	
		HideWnd(FALSE);	//鼠标不在当前窗口内
	CDialog::OnLButtonDown(nFlags, point);
}

void CSelectDrawingDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetClientRect(&rect);
	if(!rect.PtInRect(point)) 
		HideWnd(FALSE);	//鼠标不在当前窗口内
	CDialog::OnRButtonDown(nFlags, point);
}

void CSelectDrawingDlg::OnMButtonDown(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetClientRect(&rect);
	if(!rect.PtInRect(point)) 
		HideWnd(FALSE);	//鼠标不在当前窗口内
	CDialog::OnMButtonDown(nFlags, point);
}
void CSelectDrawingDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetClientRect(&rect);
	CWnd *pWnd=GetCapture();
	BOOL bInRect=rect.PtInRect(point);
	if(bInRect&&pWnd==this)
		ReleaseCapture();	//鼠标在对话框范围内并且已执行过SetCapture()
	else if(!bInRect&&(pWnd==NULL||pWnd!=this))		
		SetCapture();		//鼠标不在对话框范围内且未执行过SetCapture()
	CDialog::OnMouseMove(nFlags, point);
}

void CSelectDrawingDlg::OnCaptureChanged(CWnd *pWnd) 
{
	CDialog::OnCaptureChanged(pWnd);
}

void CSelectDrawingDlg::OnDisplayCmdDlg() 
{
	if(m_pDrawing==NULL||m_pComponent==NULL)
		return;
	HideWnd(TRUE);	//隐藏窗口
	CAcModuleResourceOverride resOverride;
	//记录杆件始终端延伸量
	double fOldStartOutSpreadLen=sys.stru_map.fPoleAxisLineStartOutSpreadLen;
	double fOldEndOutSpreadLen=sys.stru_map.fPoleAxisLineEndOutSpreadLen;
	DRAGSET.ClearEntSet();
	CDrawMapScriptDlg scriptDlg;
	scriptDlg.m_bCallByDrawingDlg=TRUE;
	scriptDlg.SetCallBackFunc(FireRunCommand);
	scriptDlg.m_pDrawing=m_pDrawing;
	scriptDlg.m_pComponent=m_pComponent;
	m_pComponent->InitCmdFeature(-1);	//初始化命令feature=-1,重启对话框保持顺序时用到 wht 11-07-23
	if(scriptDlg.DoModal()==IDOK)
	{
		m_pComponent->UpdateDrawingRectArray();
		m_pComponent->InsertDrawingName();	//标注视图名称以及上接段号
	}
	//恢复杆件始终端延伸量
	sys.stru_map.fPoleAxisLineStartOutSpreadLen=fOldStartOutSpreadLen;
	sys.stru_map.fPoleAxisLineEndOutSpreadLen=fOldEndOutSpreadLen;
}
void CSelectDrawingDlg::DeleteItemInfo(CDrawing *pDrawing)
{	//删除pDrawing及其子项关联的itemInfo
	TREEITEM_INFO *pItem=NULL;
	for(pItem=itemInfoList.GetFirst();pItem;pItem=itemInfoList.GetNext())
	{
		if(pItem->itemType==1)
		{	//删除图纸对应的itemInfo
			CDrawing *pCurDrawing=(CDrawing*)pItem->dwRefData;
			if(pCurDrawing&&pDrawing==pCurDrawing)
				itemInfoList.DeleteCursor(TRUE);
		}
		else if(pItem->itemType==2)
		{	//删除图纸中各个图元对应的ItemInfo
			CDrawingComponent *pCom=(CDrawingComponent*)pItem->dwRefData;
			if(pCom&&pCom->ParentDrawing()&&pCom->ParentDrawing()==pDrawing)
				itemInfoList.DeleteCursor(TRUE);
		}
	}
}
void CSelectDrawingDlg::DeleteItemInfo(CDrawingComponent *pComponent)
{	//删除pComponent关联的itemInfo
	TREEITEM_INFO *pItem=NULL;
	for(pItem=itemInfoList.GetFirst();pItem;pItem=itemInfoList.GetNext())
	{
		if(pItem->itemType==2)
		{	
			CDrawingComponent *pCom=(CDrawingComponent*)pItem->dwRefData;
			if(pCom&&pCom==pComponent)
				itemInfoList.DeleteCursor(TRUE);
		}
	}
}

//将版本号的保存与读取放在一个函数中 wht 11-08-09
BOOL SerializeDrawScriptHeader(CBuffer &buffer,BOOL bStore,long *pVersion=NULL);
void CSelectDrawingDlg::OnImportScript()
{
	HideWnd(TRUE);
	CAcModuleResourceOverride resOverride;
	CFileDialog dlg(TRUE,"dms","DrawingMapScript",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"script of Drawing(*.dms)|*.dms|All files (*.*)|*.*||");
#else
		"绘图脚本(*.dms)|*.dms|所有文件(*.*)|*.*||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	FILE *fp = fopen(dlg.GetPathName(),"rb");
	if(fp==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("specified file cannot be opened.");
#else
		AfxMessageBox("打不开指定文件！");
#endif
		return;
	}
	//
	CBuffer buffer;
	long file_len;
	fread(&file_len,sizeof(long),1,fp);
	BYTE *byte_arr=new BYTE[file_len];
	fread(byte_arr,file_len,1,fp);
	buffer.Write(byte_arr,file_len);
	buffer.SeekToBegin();
	delete []byte_arr;
	fclose(fp);
	
	//图纸列表
	long buffer_len=buffer.GetLength();
	long version=0;	//初始(最小)版本号为:1000001
	if(!SerializeDrawScriptHeader(buffer,FALSE,&version))
		return;
	//
	HTREEITEM hDrawingItem=m_treeDrawing.GetSelectedItem();
	if(hDrawingItem==NULL)
		return;
	if(m_pItemInfo==NULL)
		m_pItemInfo=(TREEITEM_INFO*)m_treeDrawing.GetItemData(hDrawingItem);
	/*if(m_pItemInfo==NULL)
	{	//初始化图纸feature=0,用来处理同一段号存在多张图纸的情况
		CDrawing *pDrawing=NULL;
		for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
			pDrawing->feature=0;
		int i=0,nDrawingCount=0;
		buffer.ReadInteger(&nDrawingCount);
		for(i=0;i<nDrawingCount;i++)
		{
			if(Ta.DisplayProcess)
				Ta.DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),"绘图脚本读取进度");
			CDrawing drawing;
			drawing.FromBuffer(buffer,version);
			//遍历当前图纸列表，查看是否存在同段号图纸
			CDrawing *pDestDrawing=NULL;
			for(pDestDrawing=drawingList.GetFirst();pDestDrawing;pDestDrawing=drawingList.GetNext())
			{
				if(drawing.m_nSegI==pDestDrawing->m_nSegI&&pDestDrawing->feature==0)
					break;	
			}
			if(pDestDrawing)	//存在,直接复制属性
				pDestDrawing->CopyProperty(&drawing);
			else				//不存在,新建一张图纸
			{
				if(drawing.m_nSegI>=0)
				{	//查找第一个大于当前图纸段号的图纸,将新加图纸插入至前一个位置
					for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
					{
						if(pDrawing->m_nSegI>drawing.m_nSegI)
							break;
					}
				}
				pDestDrawing=drawingList.insert(0);
				pDestDrawing->CopyProperty(&drawing);
			}
			pDestDrawing->feature=1;	//表示已处理过
		}
	}
	else*/
	if(m_pItemInfo&&m_pItemInfo->itemType==1) 
	{	//图纸
		CDrawing *pDrawing=(CDrawing*)m_pItemInfo->dwRefData;
		if(pDrawing)
		{	
			pDrawing->FromBuffer(buffer,version);
			RefreshDrawingTree(m_pDrawing,NULL);
		}
	}
	if(Ta.DisplayProcess)
#ifdef AFX_TARG_ENU_ENGLISH
		Ta.DisplayProcess(100,"Drawing script reading progress");
#else
		Ta.DisplayProcess(100,"绘图脚本读取进度");
#endif
}

void CSelectDrawingDlg::OnExportScript()
{
	CAcModuleResourceOverride resOverride;
	HTREEITEM hDrawingItem=m_treeDrawing.GetSelectedItem();
	if(hDrawingItem==NULL)
		return;
	if(m_pItemInfo==NULL)
		m_pItemInfo=(TREEITEM_INFO*)m_treeDrawing.GetItemData(hDrawingItem);
	//导出指定的绘图脚本
	CBuffer buffer(1000000);	
	buffer.ClearBuffer();
	SerializeDrawScriptHeader(buffer,TRUE);
	/*if(m_pItemInfo==NULL)
	{
		CImportScriptOptionDlg importDlg;
		if(importDlg.DoModal()!=IDOK)
			return;
		CHashTable<int> segTable;
		segTable.CreateHashTable(200);
		char seg_str[200]="";
		sprintf(seg_str,"%s",importDlg.m_sSegStr);
		if(importDlg.m_bStruMap)
		{
			for(int iSeg=FindAdaptNo(seg_str,",","-");!IsAdaptNoEnd();iSeg=FindAdaptNo(NULL,",","-"))
				segTable.SetValueAt(iSeg,iSeg);
		}
		//图纸列表
		buffer.LogPosition();
		long pos_start=buffer.GetCursorPosition();
		buffer.WriteInteger(drawingList.GetNodeNum());
		//
		int nDrawingCount=0;
		CDrawing *pDrawing=NULL;
		for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
		{
			int nSegI=-1;
			if(importDlg.m_bStruMap&&pDrawing->m_nSegI>=0)
			{
				if(stricmp(seg_str,"*")==0||segTable.GetValueAt(pDrawing->m_nSegI,nSegI))
					nSegI=pDrawing->m_nSegI;
			}
			if((importDlg.m_bAnalysisMap&&pDrawing->m_nSegI==-1)//司令图
				||(importDlg.m_bSumMap&&pDrawing->m_nSegI==-2)	//总图
				||(nSegI>=0&&nSegI==pDrawing->m_nSegI))			//结构图
			{
				pDrawing->ToBuffer(buffer);
				nDrawingCount++;
			}
		}
		//重写图纸个数
		long pos=buffer.GetCursorPosition();
		buffer.SeekPosition(pos_start);
		buffer.WriteInteger(nDrawingCount);
		buffer.SeekPosition(pos);
	}
	else*/
	CString sScriptName="DrawMapScript";
	if(m_pItemInfo&&m_pItemInfo->itemType==1) 
	{	//图纸
		CString sItemName=m_treeDrawing.GetItemText(hDrawingItem);
		sScriptName.Format("%s-%s",g_sFileName,sItemName);
		CDrawing *pDrawing=(CDrawing*)m_pItemInfo->dwRefData;
		if(pDrawing)	//导出选中图纸
			pDrawing->ToBuffer(buffer);
	}
	else 
		return;
	//
	HideWnd(TRUE);	
	CFileDialog dlg(FALSE,"dms",sScriptName,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Drawing script(*.dms)|*.dms|All files(*.*)|*.*||");
#else
		"绘图脚本(*.dms)|*.dms|所有文件(*.*)|*.*||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	FILE *fp = fopen(dlg.GetPathName(),"wb");
	if(fp==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Drawing script reading progress!");
#else
		AfxMessageBox("打不开指定文件！");
#endif
		return;
	}
	long file_len=buffer.GetLength();
	fwrite(&file_len,sizeof(long),1,fp);
	fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);
}
#endif

