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
	m_bModeless=FALSE;	//��ģ̬
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
	//����
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
	//�򻯹����	
	AddPropItem("drawingBOM.nBriefGuigeN",PROPLIST_ITEM(id++,"Simplify spec number"));
	//��ѧ����ģʽ�µ���ͼ����
	AddPropItem("lineView.m_bDimNodeNo",PROPLIST_ITEM(id++,"dimension node No.","","Yes|No"));
	AddPropItem("lineView.m_bDimPoleGuige",PROPLIST_ITEM(id++,"dimension rod's specification","","Yes|No"));
	//
	AddPropItem("LineView::m_bUsePolyLineSumMap",PROPLIST_ITEM(id++,"line style","line style when drawing general layout.","Ordinary straight line|Polygon line"));
	AddPropItem("LineView::m_bUsePolyLineChiefMap",PROPLIST_ITEM(id++,"line style","line style when drawing force line map.","Ordinary straight line|Polygon line"));
	//�ṹͼ��������
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
	//���ͼ
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
	//����
	AddPropItem("drawing",PROPLIST_ITEM(id++,"ͼֽ����"));
	AddPropItem("drawing.m_nSegI",PROPLIST_ITEM(id++,""));
	AddPropItem("drawing.m_xMapFrm.style",PROPLIST_ITEM(id++,"ͼ����","",MakeMapFrameSizeSetString()));
	AddPropItem("drawing.m_xMapFrm.nLength",PROPLIST_ITEM(id++,"ͼ�򳤶�"));
	AddPropItem("drawing.m_xMapFrm.nWidth",PROPLIST_ITEM(id++,"ͼ����"));
	AddPropItem("drawing.m_xMapFrm.nLeftMargin",PROPLIST_ITEM(id++,"����϶","���ҳ�߼�϶ֵ��"));
	AddPropItem("drawing.m_xMapFrm.nOtherMargin",PROPLIST_ITEM(id++,"�������϶","�ϡ��¡��Ҳ�ҳ�߼�϶ֵ��"));
	AddPropItem("drawing.m_xMapFrm.nBottomRightFixLen",PROPLIST_ITEM(id++,"���²�̶�����","���²����ڷ���ͼֽ��Ϣ�ľ��γ��ȡ�"));
	AddPropItem("drawing.m_xMapFrm.nBottomRightFixWidth",PROPLIST_ITEM(id++,"���²�̶��߶�","���²����ڷ���ͼֽ��Ϣ�ľ��θ߶�,���ɱ��ʱ�ɸ��ݸø߶��Զ��ж��Ƿ���Ҫ��ҳ��ʾ��"));
	AddPropItem("drawing.m_sDrawingName",PROPLIST_ITEM(id++,"ͼֽ����"));
	AddPropItem("drawing.m_sDrawingNo",PROPLIST_ITEM(id++,"ͼֽ���"));
	AddPropItem("drawing.m_bSpecifyMapFrmPos",PROPLIST_ITEM(id++,"ָ��ͼ��λ��","ָ��ͼ��λ��","��|��"));
	AddPropItem("drawing.m_hActiveModule",PROPLIST_ITEM(id++,"��ǰ�������"));
	AddPropItem("drawing.m_bAutoLayoutComponent",PROPLIST_ITEM(id++,"�Զ�����","������ͼ��С�Զ����Һ��ʵİڷ�λ�á�","��|��"));
	AddPropItem("drawing.m_fComponentSpace",PROPLIST_ITEM(id++,"��ͼ��϶","��ͼ��ͼ��֮���Լ���������ͼ��ļ�϶ֵ��"));
	//
	AddPropItem("component",PROPLIST_ITEM(id++,"��ͼ����"));
	AddPropItem("component.m_fScale",PROPLIST_ITEM(id++,"��ͼ���� 1:","��ͼ������"));
	AddPropItem("component.m_bDimScale",PROPLIST_ITEM(id++,"��ע���Ʊ���","","��|��"));
	AddPropItem("component.m_fNamePosSpace",PROPLIST_ITEM(id++,"���Ʊ�עλ�ü�϶"));
	AddPropItem("component.m_sDrawingLayer",PROPLIST_ITEM(id++,"ͼ�㷶Χ","��ͼͼ�㷶Χ"));
	AddPropItem("component.m_hModule",PROPLIST_ITEM(id++,"���߷���","��ǰ��ͼ�����ĺ��߷��顣"));
	AddPropItem("component.m_bSpecifyComPos",PROPLIST_ITEM(id++,"ʰȡ��ͼλ��","������ͼʱͨ���Ϸ���ͼ��ͼֽ��ʰȡ��ͼ�ڷ�λ�ã�ѡ���ʱ��������ͼ�����ͼ�����Ͻǵ����λ�á�","��|��"));
	AddPropItem("component.m_bNeedAdjust",PROPLIST_ITEM(id++,""));
	AddPropItem("component.m_eClassType",PROPLIST_ITEM(id++,"����"));
	AddPropItem("component.m_sName",PROPLIST_ITEM(id++,"��ͼ����","��ͼ���ƣ���Ҫ��ͼֽ�б�ע��ͼ����ʱʹ�ø����ƽ��б�ע��"));
	AddPropItem("component.m_bDimName",PROPLIST_ITEM(id++,"��ע��ͼ����","","��|��"));
	AddPropItem("component.m_sSectionViewCode",PROPLIST_ITEM(id++,"����ͼ����","��������ͼ��ʱ��ʾ�ô��ţ���ͨ����ఴťɾ�������ɻ��ƶ�����ͼ���š�"));
	AddPropItem("component.SectionView.startPt",PROPLIST_ITEM(id++,"����ͼ�����","�ɵ����ť�Ϸ�ѡ�����λ�ã����λ�ñ䶯��ϵͳ�Զ���������ͼ����"));
	AddPropItem("component.SectionView.startPt.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("component.SectionView.startPt.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("component.SectionView.endPt",PROPLIST_ITEM(id++,"����ͼ���յ�","�ɵ����ť�Ϸ�ѡ���յ�λ�ã����λ�ñ䶯��ϵͳ�Զ���������ͼ����"));
	AddPropItem("component.SectionView.endPt.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("component.SectionView.endPt.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("component.SectionView.bUpturned",PROPLIST_ITEM(id++,"����ͼ������","����ͼ��ʼ��->�ն���ʱ����ת90�㷽��Ϊ���ϣ�˳ʱ����ת90�㷽��Ϊ���¡�","����|����"));
	AddPropItem("component.SectionView.hAttachViewKey",PROPLIST_ITEM(id++,"����ͼ��������ͼ","����ͼ��������ͼ,���ƶ�����ͼʱ����ͼ������֮�ƶ���"));
	AddPropItem("component.m_nAnchorType",PROPLIST_ITEM(id++,"�ϷŻ�׼������","��ͼ���ɺ󣬽����Ϸ�ʱ�ϷŻ�׼������͡�","0.���Ͻ�|1.���Ͻ�|2.���½�|3.���½�|"));
	AddPropItem("component.m_ptLocation",PROPLIST_ITEM(id++,"��λ�����λ��"));
	AddPropItem("component.m_ptLocation.x",PROPLIST_ITEM(id++,"X����ƫ��ֵ"));
	AddPropItem("component.m_ptLocation.y",PROPLIST_ITEM(id++,"Y����ƫ��ֵ"));
	AddPropItem("component.m_ptLocation.z",PROPLIST_ITEM(id++,"Z����ƫ��ֵ"));
	AddPropItem("component.dcs",PROPLIST_ITEM(id++,"��ͼ����ϵ"));
	//
	AddPropItem("drawingBOM.m_bAutoNewTable",PROPLIST_ITEM(id++,"�Զ���ҳ","�����������ʱϵͳ�ɸ��ݱ߿�߶��Լ��߿����½Ǿ��θ߶��Զ�ʵ�ַ�ҳ���ơ�","��|��"));
	AddPropItem("drawingBOM.m_bHasTableHead",PROPLIST_ITEM(id++,"��ҳ������ͷ","����ҳ���±���Ƿ���Ʊ�ͷ��","��|��"));
	//�򻯹����	
	AddPropItem("drawingBOM.nBriefGuigeN",PROPLIST_ITEM(id++,"�򻯹����","�򻯹������ʹ�ù����������򻯱�עʱ���ñ������ƹ��������м򻯹��ĸ�����������Ϊ26"));
	//��ѧ����ģʽ�µ���ͼ����
	AddPropItem("lineView.m_bDimNodeNo",PROPLIST_ITEM(id++,"��ע�ڵ���","","��|��"));
	AddPropItem("lineView.m_bDimPoleGuige",PROPLIST_ITEM(id++,"��ע�˼����","","��|��"));
	//
	AddPropItem("LineView::m_bUsePolyLineSumMap",PROPLIST_ITEM(id++,"����","������ͼʱ�����͡�","��ֱͨ��|�����"));
	AddPropItem("LineView::m_bUsePolyLineChiefMap",PROPLIST_ITEM(id++,"����","����˾��ͼʱ�����͡�","��ֱͨ��|�����"));
	//�ṹͼ��������
	AddPropItem("StruView",PROPLIST_ITEM(id++,"�ṹͼ����"));
	AddPropItem("StruView::m_bDimPartSpec",PROPLIST_ITEM(id++,"��ע������ϸ","","��|��"));
	AddPropItem("StruView::m_bDimPoleOddment",PROPLIST_ITEM(id++,"��ע�˼�����ͷ","","��|��"));
	AddPropItem("StruView::m_bDimFootNailSpace",PROPLIST_ITEM(id++,"��ע�Ŷ����","","��|��"));
	AddPropItem("StruView::m_bDimFillPlateSpace",PROPLIST_ITEM(id++,"��ע�����","","��|��"));
	AddPropItem("StruView::m_bDimLsSpace",PROPLIST_ITEM(id++,"��ע��˨������","","��|��"));
	AddPropItem("StruView::m_bDimJgZhunJu",PROPLIST_ITEM(id++,"��ע�Ǹ�����׼��","","��|��"));
	AddPropItem("StruView::m_bDimBoltInfo",PROPLIST_ITEM(id++,"��ע��˨��Ϣ","","��|��"));
	AddPropItem("StruView::m_bDimBoltPad",PROPLIST_ITEM(id++,"��ע��˨��Ȧ","","��|��"));
	AddPropItem("StruView::m_bDimSimPoly",PROPLIST_ITEM(id++,"��ע������","","��|��"));
	AddPropItem("struView.partNo",PROPLIST_ITEM(id++,"��ע������ϸ"));
	AddPropItem("struView.partNo.m_iMirMsg",PROPLIST_ITEM(id++,"�ԳƷ�ʽ","������ű�עʱ�ĶԳƷ�ʽ��","0.X��Գ�|1.Y��Գ�|2.�޶Գ�|"));
	AddPropItem("struView.partNo.m_bDrawRefLine",PROPLIST_ITEM(id++,"����������","��ע������ϸʱ�Ƿ���������ߡ�","��|��"));
	//���ͼ
	AddPropItem("PartView.sPartNo",PROPLIST_ITEM(id++,"�������","����Ĺ������"));
	AddPropItem("PartView.hPart",PROPLIST_ITEM(id++,"�������","����Ĺ������"));
#endif
}

//From Drawing.cpp
void InitDrawingList();			//��ʼ��ͼֽ�б�
void UpdateStruDrawingList();	//���½ṹͼ��Ӧ��ͼֽ�б�
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
		//��ʼ������ID,��ʼ������hastableӦ����RefreshDrawingTree()֮ǰ��	
		//m_treeDrawing.SelectItem()�ᴥ��OnSelChange��������DisplayDrawingProperty() wht 11-06-17
		InitPropHashtable();	
		//��ʼ��ͼֽ�б�
		InitDrawingList();
		//Ĭ��ѡ����ͼ
		drawingList.GetFirst();
		m_pDrawing=drawingList.GetNext();
		RefreshDrawingTree(m_pDrawing,NULL);
		//��ʾϵͳ����
		DisplayDrawingProperty();
	}
	else 
	{	//ˢ�½���
		if(drawingList.GetNodeNum()>0)
		{	//��ֹ�ر��ļ����ٴ�����ִ�л�ͼ����ʱ���� wht 11-07-27
			if(m_pDrawing)
				m_pDrawing->feature=1;		//���ñ�־λ����ʼ��ʱ����Ҫ���Ŵ���
			if(m_pComponent)
				m_pComponent->feature=1;	//���ñ�־λ����ʼ��ʱ����Ҫ���Ŵ���
			RefreshDrawingTree(m_pDrawing,m_pComponent);
			m_propList.Redraw();
		}
		else 
			RefreshDrawingTree(NULL,NULL);
	}
	SetTimer(1,300,NULL);//Set Timer
	//�ƶ����ڵ����Ͻ�
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
	CString sCreateButtonName="����";	//���ݵ�ǰѡ����������ɰ�ť���� wht 11-07-15
#endif
	if(m_pItemInfo&&m_pItemInfo->dwRefData&&m_pItemInfo->itemType==1)
	{
		m_pDrawing=(CDrawing*)m_pItemInfo->dwRefData;
		if(m_pDrawing->feature==1)
			m_pDrawing->feature=0;
		else 
		{	//ѡ��һ��ͼֽ���Զ���ʾͼֽ��������
			f2dRect rect;
			rect.topLeft.Set(m_pDrawing->dcs.origin.x,m_pDrawing->dcs.origin.y+m_pDrawing->m_xMapFrm.nWidth);
			rect.bottomRight.Set(m_pDrawing->dcs.origin.x+m_pDrawing->m_xMapFrm.nLength,m_pDrawing->dcs.origin.y);
			ZoomAcadView(rect,50);
		}
		if(m_pDrawing->m_nSegI.iSeg>=0)
#ifdef AFX_TARG_ENU_ENGLISH
			sCreateButtonName.Format("create ��%s structure map��",(char*)m_pDrawing->m_sDrawingName);
		else 
			sCreateButtonName.Format("create ��%s��",(char*)m_pDrawing->m_sDrawingName);
#else
			sCreateButtonName.Format("���ɡ�%s�ṹͼ��",(char*)m_pDrawing->m_sDrawingName);
		else 
			sCreateButtonName.Format("���ɡ�%s��",(char*)m_pDrawing->m_sDrawingName);
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
		{	//ѡ��һ��������Զ�����ѡ��������Ŵ�
			f2dRect rect=m_pComponent->GetDrawRegion(TRUE);
			ZoomAcadView(rect,50);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		sCreateButtonName.Format("create ��%s��",(char*)m_pComponent->m_sName);
#else
		sCreateButtonName.Format("���ɡ�%s��",(char*)m_pComponent->m_sName);
#endif
	}
	SetDlgItemText(ID_BTN_CREATE_DRAWING,sCreateButtonName);	//�������ɰ�ť����
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
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"�½�");
		//pMenu->AppendMenu(MF_STRING,ID_EXPORT_SCRIPT,"�����ű�");
		//pMenu->AppendMenu(MF_STRING,ID_IMPORT_SCRIPT,"����ű�");
		pMenu->AppendMenu(MF_STRING,ID_UPDATE_ITEM,"����ͼֽ�б�");
		pMenu->AppendMenu(MF_SEPARATOR);
		pMenu->AppendMenu(MF_STRING,ID_REBULID_ITEM,"����ͼֽ�б�");
#endif
	}
	else if(m_pItemInfo->itemType==1)
	{	//ͼֽ
		//pMenu->AppendMenu(MF_STRING,ID_MOVE_TOP,"�ö�");
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_MOVE_UP,"shift up");
		pMenu->AppendMenu(MF_STRING,ID_MOVE_DOWN,"shift down");
#else
		pMenu->AppendMenu(MF_STRING,ID_MOVE_UP,"����");
		pMenu->AppendMenu(MF_STRING,ID_MOVE_DOWN,"����");
#endif
		//pMenu->AppendMenu(MF_STRING,ID_MOVE_BOTTOM,"��β");
		pMenu->AppendMenu(MF_SEPARATOR);
		if(m_pDrawing)
		{	//��ǰѡ����Ϊ����ͼֽ,ѡ�񡾽ṹͼ����ʱ����Ҫ wht 11-07-15
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
			pMenu->AppendMenu(MF_STRING,ID_ADD_ITEM,"���");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ��");
			pMenu->AppendMenu(MF_STRING,ID_ADD_PART_ITEM,"������ͼ");
			pMenu->AppendMenu(MF_STRING,ID_ERASE_RELA_CAD_ENT,"ɾ��CADʵ��");
			pMenu->AppendMenu(MF_STRING,ID_EXPORT_SCRIPT,"�����ű�");
			pMenu->AppendMenu(MF_STRING,ID_IMPORT_SCRIPT,"����ű�");
		}
		pMenu->AppendMenu(MF_STRING,ID_UPDATE_ITEM,"����ͼֽ");
		pMenu->AppendMenu(MF_SEPARATOR);
		pMenu->AppendMenu(MF_STRING,ID_REBULID_ITEM,"��������");
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
			pMenu->AppendMenu(MF_STRING,ID_MOVE_TOP,"�ö�");
			pMenu->AppendMenu(MF_STRING,ID_MOVE_UP,"����");
			pMenu->AppendMenu(MF_STRING,ID_MOVE_DOWN,"����");
			pMenu->AppendMenu(MF_STRING,ID_MOVE_BOTTOM,"��β");
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
			pMenu->AppendMenu(MF_STRING,ID_UPDATE_ITEM,"����");
			pMenu->AppendMenu(MF_SEPARATOR);
			pMenu->AppendMenu(MF_STRING,ID_REBULID_ITEM,"��������");	//��������ָ����ͼ�����б� wht 11-06-16
			pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"����������˨��ע����");
		}
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ��");
		pMenu->AppendMenu(MF_STRING,ID_ERASE_RELA_CAD_ENT,"ɾ��CADʵ��");
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
				DeleteItemInfo(pDrawing);	//ɾ��pDrawing�������������itemInfo wht 11-07-15
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
				DeleteItemInfo(pCom);	//ɾ��pComponent������itemInfo
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
//ɾ����ǰ�������CADʵ�塣
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
	//�����û�������ţ���ȡ����
	CInputPartNoDlg dlg;
	if(dlg.DoModal()!=IDOK)
		return;
	CLDSPart* pPart=(CLDSPart*)Ta.FromPartNo(dlg.m_sPartNo);
	if(pPart==NULL || pDrawing->listComponents.GetValue(pPart->handle))
		return;;	//�Ѵ��ڴ˹�����ͼ
	int nStyle=0;
	CSuperSmartPtr<CLDSPart> pSmartPart=pPart;
	if(pSmartPart->GetClassTypeId()==CLS_LINEANGLE&&
		(pSmartPart.LineAnglePointer()->pStart==NULL||pSmartPart.LineAnglePointer()->pEnd==NULL))
		nStyle=1;	//�̽Ǹ�
	else if(pSmartPart->IsPlate()&&pSmartPart.pBasicPlate->wiType==CLDSGeneralPlate::WASHER_PLATE)
		nStyle=2;	//2.��˨���
	else if(pSmartPart->IsPlate()&&pSmartPart.pBasicPlate->wiType==CLDSGeneralPlate::WRAPPER_PLATE)
		nStyle=3;	//3.��ͷ���ְ�
	else 
		return;
	//��ӹ���ͼ
	CPartDrawing *pPartDrawing=(CPartDrawing*)pDrawing->listComponents.Add(pPart->handle,DRAW_COM_PARTVIEW);
	if(nStyle==1||nStyle==3)
		pPartDrawing->m_fScale=0.1;	//�̽Ǹּ���ͷ���ְ��ͼ����Ĭ��Ϊ1::10
	else if(nStyle==2)
		pPartDrawing->m_fScale=0.2;	//��˨���Ĭ�ϻ�ͼ����Ϊ1:5
	pPartDrawing->m_hPart=pPart->handle;
	pPartDrawing->sPartNo.Copy(pPart->GetPartNo());
	pPartDrawing->m_pPart=pPart;
	char sGuige[100]="";
	pPart->GetSpecification(sGuige,TRUE);
	pPartDrawing->m_sName.Printf("%s %s",(char*)pPart->GetPartNo(),sGuige);
	pPartDrawing->m_bNeedAdjust=FALSE;
	pPartDrawing->CreateCmdList();
	//�������ڵ�
	RefreshDrawingTree(pDrawing);
}
//0.�ö� 1.���� 2.���� 3.��β
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

//����ѡ����
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
	{	//ͼֽ
		CString sItemName=m_treeDrawing.GetItemText(hDrawingItem);
		CDrawing *pDrawing=(CDrawing*)m_pItemInfo->dwRefData;
		if(pDrawing)
		{
			//1.ɾ����ǰͼԪ������CADʵ������
			pDrawing->EraseRelaCadEnt();
			//2.���������б�
			pDrawing->UpdateCmdList();
			//3.ˢ�½�����ʾ
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
			//4.��������
			pDrawing->CreateComponents();
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(sItemName.CompareNoCase("structure map")==0)
#else
		else if(sItemName.CompareNoCase("�ṹͼ")==0)
#endif
		{	//�ṹͼĿ¼��,�����������ʱ�����������нṹͼ wht 11-07-15
			HTREEITEM hStruItem=hDrawingItem;
			//ɾ�����зֶνṹͼ��ص�itemInfo
			TREEITEM_INFO *pItem=NULL;
			for(pItem=itemInfoList.GetFirst();pItem;pItem=itemInfoList.GetNext())
			{
				if(pItem->itemType==1)
				{	//ɾ���ֶνṹͼͼֽ��Ӧ��itemInfo
					pDrawing=(CDrawing*)pItem->dwRefData;
					if(pDrawing&&pDrawing->m_nSegI.iSeg>=0)
						itemInfoList.DeleteCursor(TRUE);
				}
				else if(pItem->itemType==2)
				{	//ɾ���ṹͼ�и���ͼԪ��Ӧ��ItemInfo
					CDrawingComponent *pCom=(CDrawingComponent*)pItem->dwRefData;
					if(pCom&&pCom->ParentDrawing()&&pCom->ParentDrawing()->m_nSegI.iSeg>=0)
						itemInfoList.DeleteCursor(TRUE);
				}
			}
			//ɾ��ͼֽ�б��������нṹͼ��Ӧ�Ľڵ�
			HTREEITEM hChildItem=m_treeDrawing.GetChildItem(hStruItem);
			while(hChildItem)
			{
				HTREEITEM hCurItem=hChildItem;
				hChildItem=m_treeDrawing.GetNextItem(hChildItem,TVGN_NEXT);
				m_treeDrawing.DeleteItem(hCurItem);
			}
			//���·ֶνṹͼͼֽ�б�
			UpdateStruDrawingList();
			//ˢ�½���
			for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
			{
				if(pDrawing->m_nSegI.iSeg<0)
					continue;
				//��ͼֽ�б�����Ӷ�Ӧ��
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
	{	//���¸���ͼ������� 
		CDrawingComponent *pComponent=(CDrawingComponent*)m_pItemInfo->dwRefData;
		if(pComponent)	//���������б�
		{
			//1.ɾ����ǰͼԪ������CADʵ������
			pComponent->EraseRelaCadEnt();
			//2.���������б�
			pComponent->UpdateCmdList(TRUE);
			//3.�������ɵ�ǰͼԪ
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
	{	//ͼֽ
		CString sItemName=m_treeDrawing.GetItemText(hDrawingItem);
		CDrawing *pDrawing=(CDrawing*)m_pItemInfo->dwRefData;
		if(pDrawing)
		{
			pDrawing->InitDrawing(pDrawing->m_nSegI);
			//ˢ�½�����ʾ
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
		else if(sItemName.CompareNoCase("�ṹͼ")==0)
#endif
		{	//�ṹͼĿ¼��,�����������ʱ�����������нṹͼ wht 11-07-15
			HTREEITEM hStruItem=hDrawingItem;
			//ɾ�����зֶνṹͼ��ص�itemInfo
			TREEITEM_INFO *pItem=NULL;
			for(pItem=itemInfoList.GetFirst();pItem;pItem=itemInfoList.GetNext())
			{
				if(pItem->itemType==1)
				{	//ɾ���ֶνṹͼͼֽ��Ӧ��itemInfo
					pDrawing=(CDrawing*)pItem->dwRefData;
					if(pDrawing&&pDrawing->m_nSegI>=0)
						itemInfoList.DeleteCursor(TRUE);
				}
				else if(pItem->itemType==2)
				{	//ɾ���ṹͼ�и���ͼԪ��Ӧ��ItemInfo
					CDrawingComponent *pCom=(CDrawingComponent*)pItem->dwRefData;
					if(pCom&&pCom->ParentDrawing()&&pCom->ParentDrawing()->m_nSegI.iSeg>=0)
						itemInfoList.DeleteCursor(TRUE);
				}
			}
			//ɾ��ԭ�зֶνṹͼ
			for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
			{
				if(pDrawing->m_nSegI.iSeg>=0)
					drawingList.DeleteCursor();
			}
			//ɾ��ͼֽ�б��������нṹͼ��Ӧ�Ľڵ�
			HTREEITEM hChildItem=m_treeDrawing.GetChildItem(hStruItem);
			while(hChildItem)
			{
				HTREEITEM hCurItem=hChildItem;
				hChildItem=m_treeDrawing.GetNextItem(hChildItem,TVGN_NEXT);
				m_treeDrawing.DeleteItem(hCurItem);
			}
			//���³�ʼ���ֶνṹͼ
			ATOM_LIST<SEGI> segNoList;
			Ta.GetSegNoList(segNoList);
			for(SEGI *pSegI=segNoList.GetFirst();pSegI;pSegI=segNoList.GetNext())
			{	
				pDrawing=drawingList.Add(0);
				pDrawing->InitDrawing(*pSegI);
#ifdef AFX_TARG_ENU_ENGLISH
				pDrawing->m_sDrawingName.Printf("%s segment",(char*)pSegI->ToString());
#else
				pDrawing->m_sDrawingName.Printf("%s��",(char*)pSegI->ToString());
#endif
				//��ͼֽ�б�����Ӷ�Ӧ��
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
	{	//�������ɸ���ͼ������� wht 11-06-16
		CDrawingComponent *pComponent=(CDrawingComponent*)m_pItemInfo->dwRefData;
		if(pComponent)	//���������б�
			pComponent->CreateCmdList(pComponent->m_eClassType==DRAW_COM_STRUVIEW);//FALSE);
	}
}

void CSelectDrawingDlg::OnUpdateItem()
{
	//AfxGetApp()->DoWaitCursor(1);	//�������Ϊ�ȴ�״̬  wht 12-03-23
	if(!m_pItemInfo)	
		UpdateDrawingList(FALSE);//��������ͼֽ����
	else 
		UpdateItem();			//����ѡ����ͼֽ��ͼԪ����
	//AfxGetApp()->DoWaitCursor(-1);	//�����ȴ�״̬
}

void CSelectDrawingDlg::OnRebuildItem()
{
	//AfxGetApp()->DoWaitCursor(1);	//�������Ϊ�ȴ�״̬  wht 12-03-23
	if(!m_pItemInfo)	
		UpdateDrawingList(TRUE);	//����ͼֽ�б�
	else
		RebulidItem();				//��������ѡ����
	//AfxGetApp()->DoWaitCursor(-1);	//�����ȴ�״̬
}

void CSelectDrawingDlg::RefreshDrawingTree(CDrawing *pCurDrawing/*=NULL*/,
										   CDrawingComponent *pCurDrawingCom/*=NULL*/)
{
	itemInfoList.Empty();
	m_treeDrawing.DeleteAllItems();
	m_pItemInfo=NULL;	//��ʼ��ѡ����
	m_pDrawing=NULL;
	m_pComponent=NULL;
	CDrawing *pDrawing=NULL;
	CDrawingComponent *pComponent=NULL;
	HTREEITEM hDrawing=NULL,hAnalysisItem=NULL,hSumItem=NULL,hStruItem=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	hDrawing=m_treeDrawing.InsertItem("drawing",PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET);
#else
	hDrawing=m_treeDrawing.InsertItem("ͼֽ",PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET);
#endif
	m_treeDrawing.SetItemData(hDrawing,0);
	m_treeDrawing.Expand(hDrawing,TVE_EXPAND);
	//
	HTREEITEM hSelectedItem=NULL;
	for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
	{
		if(pDrawing->m_nSegI.iSeg==-1)
		{	//˾��ͼ
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
				{	//������ͼ��Ӧ��ͼԪ
					CXhChar16 sFolderNam("%s",pComponent->m_pAttachView->folderName);
					if(sFolderNam.GetLength()==0)
#ifdef AFX_TARG_ENU_ENGLISH
						sFolderNam.Copy("Custom spread view");
#else
						sFolderNam.Copy("����չ����ͼ");
#endif
					HTREEITEM *pFolderItem=itemHashList.GetValue(sFolderNam);
					if(pFolderItem==NULL)
					{	//��Ӷ�����ͼ���ڷ����Ӧ�� wht 12-06-06
						HTREEITEM hFolderItem=m_treeDrawing.InsertItem(sFolderNam,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hAnalysisItem);
						TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(3,NULL));//���͸�Ϊ3����ɾ��
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
				hStruItem=m_treeDrawing.InsertItem("�ṹͼ",PRJ_IMG_STATUS,PRJ_IMG_STATUS,hDrawing);
#endif
			TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(1,0));	//����Ϊ1,����Ϊ0 �ṹͼĿ¼�� wht 11-06-21
			m_treeDrawing.SetItemData(hStruItem,(DWORD)pItemInfo);
			//�ֶνṹͼ
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
	{	//�ṹͼ����ɾ��ʱҲӦ�����ṹͼĿ¼�� wht 11-07-15
#ifdef AFX_TARG_ENU_ENGLISH
		hStruItem=m_treeDrawing.InsertItem("structure map",PRJ_IMG_STATUS,PRJ_IMG_STATUS,hDrawing);
#else
		hStruItem=m_treeDrawing.InsertItem("�ṹͼ",PRJ_IMG_STATUS,PRJ_IMG_STATUS,hDrawing);
#endif
		TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(1,0));	//����Ϊ1,����Ϊ0 �ṹͼĿ¼�� wht 11-06-21
		m_treeDrawing.SetItemData(hStruItem,(DWORD)pItemInfo);
	}
	m_treeDrawing.SetRedraw();
	if(hSelectedItem)	//SelectItem�ᴥ��OnSelChanged�¼�����Ҫ���������ѡ����
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
		{	//��������Ӧ������ͼ
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
void PreparePartSpecList(ATOM_LIST<SPEC_MARK> &specList);//Ԥ��׼���򻯵Ĺ���б�(˾��ͼ��ע�˼����ʱʹ��)
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
			//���³�ʼ��������λ��
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
			if(hCurItem)	//ͬʱ����������ж�Ӧ������ wht 11-06-16
				pDrawingDlg->m_treeDrawing.SetItemText(hCurItem,pDrawing->m_sDrawingName);
		}
		else if(CSelectDrawingDlg::GetPropID("drawing.m_sDrawingNo")==pItem->m_idProp)
			pDrawing->m_sDrawingNo.Copy(valueStr);
		else if(CSelectDrawingDlg::GetPropID("drawing.m_bSpecifyMapFrmPos")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
			{	//�޸ĵ�ǰ���ߺ���������ָ��ͼֽ wht 11-06-16
				pDrawingDlg->RebulidItem();
				//ĳЩʱ����Ҫ������ͼ��Ϊ��֤������ͼ�������һ��
				//�޸ĵ�ǰͼֽ�������ʱӦͬʱ�޸�����ǰ������� wht 11-07-14
				Ta.m_hActiveModule=pDrawing->m_hActiveModule;	
				/*CDrawingComponent *pComponent=NULL,*pOldActiveCom=NULL,*pNewActiveCom=NULL,*pSideCom=NULL;
				//�ı�ͼֽ��ǰ������ߺ�����ͼԪ����Ҫָ��λ������
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
							pComponent->m_sName.Printf("%s����ͼ",pComponent->m_pModule->description);
						pComponent->CreateCmdList();
					}
					else if(pComponent->m_hModule==hOldActiveModule)
					{	//
						BOOL bSideView=FALSE;
						if(pComponent->m_pAttachView&&stricmp(pComponent->m_pAttachView->defaultLayer,"SPY")==0)
							bSideView=TRUE;
						if(bSideView)
						{	//�ɲ���ͼӦ�޸�Ϊ��ǰ����ͼ��Ӧ�Ĳ���ͼ wht 11-06-16
							pSideCom=pComponent;
							pComponent->m_hModule=pDrawing->m_hActiveModule;
							pComponent->m_pModule=Ta.Module.FromHandle(pComponent->m_hModule);
							if(pComponent->m_pModule)
								pComponent->m_sName.Printf("%s����ͼ",pComponent->m_pModule->description);
							pComponent->CreateCmdList();
						}
						else
						{	//������ͼ
							pOldActiveCom=pComponent;
							pComponent->m_hModule=hOldActiveModule;
							pComponent->m_pModule=Ta.Module.FromHandle(pComponent->m_hModule);
							if(pComponent->m_pModule)
								pComponent->m_sName.Printf("%s����ͼ",pComponent->m_pModule->description);
							pComponent->CreateCmdList();
						}
					}
				}
				//ˢ��������ڵ�
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
			if(valueStr.CompareNoCase("�����")==0)
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
			if(valueStr.CompareNoCase("�����")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
				char sAnchorType[50]="λ��";
				if(pDrawingComponent->m_nAnchorType==0)
					strcpy(sAnchorType,"���Ͻ�");
				else if(pDrawingComponent->m_nAnchorType==1)
					strcpy(sAnchorType,"���Ͻ�");
				else if(pDrawingComponent->m_nAnchorType==2)
					strcpy(sAnchorType,"���½�");
				else if(pDrawingComponent->m_nAnchorType==3)
					strcpy(sAnchorType,"���½�");
#endif
				//
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "relative position of location point";
				lpInfo->m_strPropHelp.Format("offset position of view %s relative to the top left corner of drawing frame.",sAnchorType);
#else
				lpInfo->m_strPropName = "��λ�����λ��";
				lpInfo->m_strPropHelp.Format("��ͼ%s�����ͼ�����Ͻǵ�ƫ��λ�á�",sAnchorType);
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
				lpInfo->m_strPropName = "X����ƫ��ֵ";
				lpInfo->m_strPropHelp.Format("��ͼ%s�����ͼ�����Ͻ�X�����ƫ��ֵ��",sAnchorType);
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
				lpInfo->m_strPropName = "Y����ƫ��ֵ";
				lpInfo->m_strPropHelp.Format("��ͼ%s�����ͼ�����Ͻ�Y�����ƫ��ֵ��",sAnchorType);
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
			if(hCurItem)	//ͬʱ����������ж�Ӧ������ wht 11-06-16
				pDrawingDlg->m_treeDrawing.SetItemText(hCurItem,pDrawingComponent->m_sName);
			//�������ɰ�ť����
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("create ��%s��",(char*)pDrawingComponent->m_sName);
#else
			sText.Printf("���ɡ�%s��",(char*)pDrawingComponent->m_sName);
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
				ZoomAcadView(rect,100);	//�ƶ�������������ͼλ��
			}
			if(valueStr.CompareNoCase(pDrawingComponent->m_sSectionViewCode)==0)
				return FALSE;	//����δ�����仯����Ҫִ�����²���
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
			//ɾ��ԭ����ʵ����������
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
			//ɾ��ԭ����ʵ����������
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
			//ɾ��ԭ����ʵ����������
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
			//ɾ��ԭ����ʵ����������
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
			if(valueStr.CompareNoCase("����")==0)
#endif
				pViewSymbol->bUpturned=TRUE;
			else 
				pViewSymbol->bUpturned=FALSE;
			//ɾ��ԭ����ʵ����������
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
				&&CSelectDrawingDlg::GetPropID("struView.partNo.m_bDrawRefLine")==pItem->m_idProp)//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
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
				if(valueStr.CompareNoCase("��")==0)
#endif
					pCmd->m_paraPartSpec.bDrawRefLine=TRUE;
				else 
					pCmd->m_paraPartSpec.bDrawRefLine=FALSE;
				if(bOldDrawRefLine!=pCmd->m_paraPartSpec.bDrawRefLine)	//�޸ĶԳƷ�ʽ�����±�ע�ù���
					pCmd->Rerun();	//ɾ���������ɵ�ԭ��ʵ��,��������
			}
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
				&&CSelectDrawingDlg::GetPropID("struView.partNo.m_iMirMsg")==pItem->m_idProp)//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
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
				if(iOldMirMsg!=pCmd->m_paraPartSpec.iMirMsg)	//�޸ĶԳƷ�ʽ�����±�ע�ù���
					pCmd->Rerun();	//ɾ���������ɵ�ԭ��ʵ��,��������
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
			{	//�ṹͼ�е���ͼ�޸��޸�����ʱ������ͼ���� wht 11-07-20
#ifdef AFX_TARG_ENU_ENGLISH
				pDrawingComponent->m_sName.Printf("single line map 1:%d",atoi(valueStr));
#else
				pDrawingComponent->m_sName.Printf("����ͼ 1:%d",atoi(valueStr));
#endif
				pPropList->SetItemPropValue(CSelectDrawingDlg::GetPropID("component.m_sName"),pDrawingComponent->m_sName);
				//ͬʱ����������ж�Ӧ������
				HTREEITEM hCurItem=pDrawingDlg->m_treeDrawing.GetSelectedItem();
				if(hCurItem)
					pDrawingDlg->m_treeDrawing.SetItemText(hCurItem,pDrawingComponent->m_sName);
				//�������ɰ�ť����
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("create ��%s��",(char*)pDrawingComponent->m_sName);
#else
				sText.Printf("���ɡ�%s��",(char*)pDrawingComponent->m_sName);
#endif
				pDrawingDlg->SetDlgItemText(IDOK,sText);
			}
		}
		else if(CSelectDrawingDlg::GetPropID("component.m_fNamePosSpace")==pItem->m_idProp)
		{
			pDrawingComponent->m_fNamePosSpace=atof(valueStr);
			pDrawingComponent->InsertDrawingName();	//�޸���ͼ����λ�ü�϶���������� wht 11-07-31
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
			char sAnchorType[50]="λ��";
			if(pDrawingComponent->m_nAnchorType==0)
				strcpy(sAnchorType,"���Ͻ�");
			else if(pDrawingComponent->m_nAnchorType==1)
				strcpy(sAnchorType,"���Ͻ�");
			else if(pDrawingComponent->m_nAnchorType==2)
				strcpy(sAnchorType,"���½�");
			else if(pDrawingComponent->m_nAnchorType==3)
				strcpy(sAnchorType,"���½�");
			pPropList->SetItemPropHelpStr(CSelectDrawingDlg::GetPropID("component.m_ptLocation"),
				"��ͼ%s�����ͼ�����Ͻǵ�ƫ��λ�á�",sAnchorType);
			pPropList->SetItemPropHelpStr(CSelectDrawingDlg::GetPropID("component.m_ptLocation.x"),
				"��ͼ%s�����ͼ�����Ͻ�X�����ƫ��ֵ��",sAnchorType);
			pPropList->SetItemPropHelpStr(CSelectDrawingDlg::GetPropID("component.m_ptLocation.y"),
				"��ͼ%s�����ͼ�����Ͻ�X�����ƫ��ֵ��",sAnchorType);
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
				pDrawingComponent->ReCreate();	//��������
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
				pDrawingComponent->ReCreate();	//��������
			}
		}
		else if(CSelectDrawingDlg::GetPropID("component.m_bNeedAdjust")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("��")==0)
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
			{	//��ѧģʽ
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
#else
				if(valueStr.CompareNoCase("��")==0)
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
			{	//��ѧģʽ
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
#else
				if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
#endif
				pDrawingBOM->m_bHasTableHead=TRUE;
			else 
				pDrawingBOM->m_bHasTableHead=FALSE;
			if(bHasTableHead!=pDrawingBOM->m_bHasTableHead
				&&pDrawingBOM->xFormatTbl2.GetRowsCount()>0
				&&pDrawingBOM->m_eBOMType==BOM_SEGPART)
			{	//������ҳ��ı��
				if(bHasTableHead)
				{	//ɾ����ͷ��ǰ����
					pDrawingBOM->xFormatTbl2.DelRow(0);
					pDrawingBOM->xFormatTbl2.DelRow(0);
					pDrawingBOM->xFormatTbl2.DelRow(0);
				}
				else 
				{	//�����ͷ��xFromatTbl1��ǰ����
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
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
#endif
				pDrawingComponent->m_bDimName=TRUE;
			else 
				pDrawingComponent->m_bDimName=FALSE;
		}
	}
	else if(pDrawingDlg->m_pItemInfo&&pDrawingDlg->m_pItemInfo->itemType==1)
	{	//�ṹͼ��������
		if(CSelectDrawingDlg::GetPropID("StruView::m_bDimPartSpec")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
			pDrawingDlg->HideWnd(TRUE);	//���ضԻ���
			//ѡ�е�ǰ��ͼ�е����й���
			DRAGSET.ClearEntSet();
			pCurComponent->GetEntSet(DRAGSET.drag_ent_set);
			//�����Ϸ�
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
			CXhChar100 sPrompt("������[%s]�����\n",&pCurComponent->m_sName);
#endif
			int nRetCode=DragEntSet(basepnt,sPrompt);
			if(nRetCode==RTNORM)
			{	//��ͼλ�ñ䶯�����Ҫ��������ϵλ��
				double fOffsetX=basepnt.x-old_pos.x;
				double fOffsetY=basepnt.y-old_pos.y;
				pCurComponent->ModifyAnchorPointRelativePos(&fOffsetX,&fOffsetY);
				//���½���
				CWnd* pMainWnd=AfxGetMainWnd();
				if(pMainWnd)	//����pMainWnd=NULL�����
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
				//�޸ļ򻯹�������������ɼ򻯵Ĺ���б�����
				PreparePartSpecList(pDrawingBOM->SpecList);
				pDrawingBOM->m_nBriefGuigeN=sys.nBriefGuigeN;		//�򻯹����
				if(pDrawingDlg->GetPropValueStr(CSelectDrawingDlg::GetPropID("drawingBOM.nBriefGuigeN"),sText)>0)
					pPropList->SetItemPropValue(CSelectDrawingDlg::GetPropID("drawingBOM.nBriefGuigeN"),"%s",&sText);
			}
		}
		else if(CSelectDrawingDlg::GetPropID("component.m_sName")==pItem->m_idProp)
		{
			pDrawingDlg->HideWnd(TRUE);	//���ضԻ���
			DRAGSET.ClearEntSet();
			//ѡ����ͼ����
			pCurComponent->GetAttachEntSet(DRAGSET.drag_ent_set,1);
			//��һ��������ͼ��Ҫָ�������
			f2dRect rect=pCurComponent->GetDrawRegion(TRUE,TRUE);
			f3dPoint basepnt((rect.topLeft.x+rect.bottomRight.x)*0.5,rect.bottomRight.y-pCurComponent->m_fNamePosSpace);
			f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar100 sPrompt("Please input [%s] insertion point\n",&pCurComponent->m_sName);
#else
			CXhChar100 sPrompt("������[%s]�����\n",&pCurComponent->m_sName);
#endif
			int nRetCode=DragEntSet(basepnt,sPrompt);
			if(nRetCode==RTNORM)
			{	//��ͼλ�ñ䶯�����Ҫ��������ϵλ��
				double fOffsetX=basepnt.x-old_pos.x;
				double fOffsetY=basepnt.y-old_pos.y;
				pCurComponent->m_fNamePosSpace-=fOffsetY;
				//���½���
				CWnd* pMainWnd=AfxGetMainWnd();
				if(pMainWnd)	//����pMainWnd=NULL�����
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
			{	//�������ͼ��
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
				{	//������ͬ�κŵ�ͼֽ�в�������ͼ
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
				
				pDrawingDlg->HideWnd(TRUE);	//���ضԻ���
				if(pMainView)	
				{	//�Զ�����������ͼ��������
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
				if(AfxMessageBox("��Ҫɾ������ͼ��(Y),�����ƶ�����ͼ��λ��(N)?",MB_YESNO)==IDYES)
#endif
				{	//ɾ������ͼ��
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
					pDrawingDlg->HideWnd(TRUE);	//���ضԻ���
					//ѡ������ͼ���ʵ�弯
					pViewSymbol->GetEntSet(DRAGSET.drag_ent_set);
					f3dPoint basepnt(pViewSymbol->startPt.x,pViewSymbol->startPt.y);
					f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
					CXhChar100 sPrompt("please input [%s-%s] starting position\n",(char*)pViewSymbol->viewName,(char*)pViewSymbol->viewName);
#else
					CXhChar100 sPrompt("������[%s-%s]���λ��\n",(char*)pViewSymbol->viewName,(char*)pViewSymbol->viewName);
#endif
					int nRetCode=DragEntSet(basepnt,sPrompt);
					if(nRetCode==RTNORM)
					{	//��������ͼ��λ��
						double fOffsetX=basepnt.x-old_pos.x;
						double fOffsetY=basepnt.y-old_pos.y;
						pViewSymbol->startPt.x+=fOffsetX;
						pViewSymbol->startPt.y+=fOffsetY;
						pViewSymbol->endPt.x+=fOffsetX;
						pViewSymbol->endPt.y+=fOffsetY;
						//���½���
						actrTransactionManager->flushGraphics();
						acedUpdateDisplay();
					}
				}
			}
		}
		else if(pCurComponent->m_eClassType==DRAW_COM_STRUVIEW&&
				CSelectDrawingDlg::GetPropID("component.SectionView.startPt")==pItem->m_idProp)
		{
			pDrawingDlg->HideWnd(TRUE);	//���ضԻ���
			DRAGSET.ClearEntSet();
			CStruView *pStruView=(CStruView*)pCurComponent;
			CSectionViewSymbol *pViewSymbol=NULL;
			pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol==NULL)
				return FALSE;
			//ѡ������ͼ���ʵ�弯
			pViewSymbol->GetEntSet(DRAGSET.drag_ent_set,0);
			//����������ͼ��ʼ��λ��
			SCOPE_STRU scope=pViewSymbol->GetEntScope(0);
			ZoomAcadView(scope,10);
			//
			f3dPoint basepnt(pViewSymbol->startPt.x,pViewSymbol->startPt.y);
			f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar100 sPrompt("please input [%s-%s] starting position\n",&pViewSymbol->viewName,&pViewSymbol->viewName);
#else
			CXhChar100 sPrompt("������[%s-%s]���λ��\n",&pViewSymbol->viewName,&pViewSymbol->viewName);
#endif
			int nRetCode=DragEntSet(basepnt,sPrompt);
			if(nRetCode==RTNORM)
			{	//��ͼλ�ñ䶯�����Ҫ��������ϵλ��
				double fOffsetX=basepnt.x-old_pos.x;
				double fOffsetY=basepnt.y-old_pos.y;
				pViewSymbol->startPt.x+=fOffsetX;
				pViewSymbol->startPt.y+=fOffsetY;
				//���������λ��������������ͼ��
				pViewSymbol->EraseRelativeAcadEntity();
				pViewSymbol->Draw();
				//���½���
				actrTransactionManager->flushGraphics();
				acedUpdateDisplay();
			}
		}
		else if(pCurComponent->m_eClassType==DRAW_COM_STRUVIEW&&
			CSelectDrawingDlg::GetPropID("component.SectionView.endPt")==pItem->m_idProp)
		{
			pDrawingDlg->HideWnd(TRUE);	//���ضԻ���
			DRAGSET.ClearEntSet();
			CStruView *pStruView=(CStruView*)pCurComponent;
			CSectionViewSymbol *pViewSymbol=NULL;
			pViewSymbol=pStruView->GetSectionViewSymbol();
			if(pViewSymbol==NULL)
				return FALSE;
			//ѡ������ͼ���ʵ�弯
			pViewSymbol->GetEntSet(DRAGSET.drag_ent_set,1);
			//����������ͼ��ʼ��λ��
			SCOPE_STRU scope=pViewSymbol->GetEntScope(1);
			ZoomAcadView(scope,10);
			f3dPoint basepnt(pViewSymbol->endPt.x,pViewSymbol->endPt.y);
			f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar100 sPrompt("please input[%s-%s]starting position\n",&pViewSymbol->viewName,&pViewSymbol->viewName);
#else
			CXhChar100 sPrompt("������[%s-%s]���λ��\n",&pViewSymbol->viewName,&pViewSymbol->viewName);
#endif
			int nRetCode=DragEntSet(basepnt,sPrompt);
			if(nRetCode==RTNORM)
			{	//��ͼλ�ñ䶯�����Ҫ��������ϵλ��
				double fOffsetX=basepnt.x-old_pos.x;
				double fOffsetY=basepnt.y-old_pos.y;
				pViewSymbol->endPt.x+=fOffsetX;
				pViewSymbol->endPt.y+=fOffsetY;
				//���������λ��������������ͼ��
				pViewSymbol->EraseRelativeAcadEntity();
				pViewSymbol->Draw();
				//���½���
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
				//�ṹͼ��Ӧ�Ļ�������ϵ�����仯��Ӧ��ʱ���¹������Ʒ�ʽ wht 11-07-31
				if(pCurComponent->m_eClassType==DRAW_COM_STRUVIEW)	
					((CStruView*)pCurComponent)->UpdatePartDrawTypeAndSpecialDim();
			}
		}
	}
	return TRUE;
}

void CSelectDrawingDlg::DisplayDrawingProperty()
{
	//���ûص�����
	m_propList.CleanTree();
	m_propList.m_nObjClassTypeId = 0;
	m_propList.SetModifyValueFunc(ModifyDrawingValue);
	m_propList.SetButtonClickFunc(ButtonClickDrawing);
	m_propList.SetModifyStatusFunc(ModifyItemStatus);
	m_propList.SetPropHelpPromptFunc(FireSelChangeItem);
	//����
	CPropertyListOper<CSelectDrawingDlg> oper(&m_propList,this);
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pSonPropItem=NULL,*pRootItem=m_propList.GetRootItem();
	//���û�ͼ����
	CDrawing *pDrawing=NULL;
	CDrawingComponent *pDrawingComponent=NULL;
	if(m_pItemInfo&&m_pItemInfo->itemType==1)
		pDrawing=(CDrawing*)m_pItemInfo->dwRefData;
	else if(m_pItemInfo&&m_pItemInfo->itemType==2)
		pDrawingComponent=(CDrawingComponent*)m_pItemInfo->dwRefData;
	else
		oper.InsertPropItem(pRootItem,"drawing");	//ͼֽ����
	if(pDrawing)
	{
		pGroupItem=oper.InsertPropItem(pRootItem,"drawing");
		if(CSelectDrawingDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CSelectDrawingDlg::GetPropStatus(pGroupItem->m_idProp);
		//ͼֽ����
		oper.InsertEditPropItem(pGroupItem,"drawing.m_sDrawingName");
		//ͼֽ���
		oper.InsertEditPropItem(pGroupItem,"drawing.m_sDrawingNo");
		if(pDrawing->m_nSegI.iSeg==-1||pDrawing->m_nSegI.iSeg==-2)
		{	//��ǰ������߷���
			pPropItem=oper.InsertCmbListPropItem(pGroupItem,"drawing.m_hActiveModule");
			pPropItem->m_lpNodeInfo->m_cmbItems="";
			CLDSModule *pModule=NULL;
			for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
			{
				pPropItem->m_lpNodeInfo->m_cmbItems+=pModule->description;
				pPropItem->m_lpNodeInfo->m_cmbItems+="|";
			}
		}
		//ͼ������
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"drawing.m_xMapFrm.style");
		//ͼ�򳤶�
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"drawing.m_xMapFrm.nLength");
		pSonPropItem->SetReadOnly();
		//ͼ����
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"drawing.m_xMapFrm.nWidth");
		pSonPropItem->SetReadOnly();
		//����϶
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"drawing.m_xMapFrm.nLeftMargin");
		pSonPropItem->SetReadOnly();
		//�������϶
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"drawing.m_xMapFrm.nOtherMargin");
		pSonPropItem->SetReadOnly();
		//���²�̶�����
		oper.InsertEditPropItem(pPropItem,"drawing.m_xMapFrm.nBottomRightFixLen");
		//���²�̶��߶�
		oper.InsertEditPropItem(pPropItem,"drawing.m_xMapFrm.nBottomRightFixWidth");
		//ָ��ͼ��λ��
		oper.InsertCmbListPropItem(pGroupItem,"drawing.m_bSpecifyMapFrmPos");
		//�Զ�����
		oper.InsertCmbListPropItem(pGroupItem,"drawing.m_bAutoLayoutComponent");
		//��ͼ��϶
		oper.InsertEditPropItem(pGroupItem,"drawing.m_fComponentSpace");
		//����
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
		//����
		pPropItem=oper.InsertEditPropItem(pGroupItem,"component.m_eClassType");
		pPropItem->SetReadOnly();
		if(pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW
			||pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW)
		{	
			if(pDrawingComponent->ParentDrawing()->m_nSegI.iSeg<0
				&&pDrawingComponent->m_eClassType!=DRAW_COM_STRUVIEW)
			{	//�������߷���
				pPropItem=oper.InsertEditPropItem(pGroupItem,"component.m_hModule");
				pPropItem->SetReadOnly();
			}
			if(pDrawingComponent->m_pAttachView&&pDrawingComponent->m_pAttachView->m_iViewType==0)
			{	//ͼ�㷶Χ
				pPropItem=oper.InsertEditPropItem(pGroupItem,"component.m_sDrawingLayer");
				pPropItem->SetReadOnly();	//ͼ�㷶Χ��ʱ����Ϊֻ������
			}
		}
		//��ͼ����
		if(pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW
			||pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			||pDrawingComponent->m_eClassType==DRAW_COM_PARTVIEW)
			oper.InsertEditPropItem(pGroupItem,"component.m_fScale");
			
		//����
		oper.InsertBtnEditPropItem(pGroupItem,"component.m_sName");
		if(pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW
			||pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW)
		{	//���Ʊ�עλ�ü�϶
			pPropItem=oper.InsertEditPropItem(pGroupItem,"component.m_fNamePosSpace");
#ifdef AFX_TARG_ENU_ENGLISH
			if(pDrawingComponent->m_eClassType!=DRAW_COM_STRUVIEW)
				pPropItem->m_lpNodeInfo->m_strPropHelp.Format("clearance value between view name labelling position and the bottom of View, clearance value between segement No. on moduel and the top of view.");
			else 
				pPropItem->m_lpNodeInfo->m_strPropHelp.Format("View name at the bottom of the dimension position with the view of the clearance value.");
#else
			if(pDrawingComponent->m_eClassType!=DRAW_COM_STRUVIEW)
				pPropItem->m_lpNodeInfo->m_strPropHelp.Format("��ͼ���Ʊ�עλ������ͼ�ײ��ļ�϶ֵ,����ͼ�ϽӶκű�עλ������ͼ�����ļ�϶ֵ��");
			else 
				pPropItem->m_lpNodeInfo->m_strPropHelp.Format("��ͼ���Ʊ�עλ������ͼ�ײ��ļ�϶ֵ��");
#endif
		}
		if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW&&pDrawingComponent->m_pAttachView
			&&pDrawingComponent->m_pAttachView->m_iViewType==1)
		{	//����ͼ����
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
				//����ͼ��������ͼ ������ͼ˳�����ܵ��¹�����ͼ����,���������Ӧ֧�ֹ������ݡ�
				pSonPropItem=oper.InsertCmbListPropItem(pPropItem,"component.SectionView.hAttachViewKey");
				pSonPropItem->m_lpNodeInfo->m_cmbItems=pStruView->ParentDrawing()->MakeComponentSetStr();
				//����
				oper.InsertCmbListPropItem(pPropItem,"component.SectionView.bUpturned");
				//���
				pSonPropItem=oper.InsertButtonPropItem(pPropItem,"component.SectionView.startPt");
				if(CSelectDrawingDlg::GetPropStatus(pSonPropItem->m_idProp)!=-1)
					pSonPropItem->m_bHideChildren = CSelectDrawingDlg::GetPropStatus(pSonPropItem->m_idProp);
				else 
					pSonPropItem->m_bHideChildren=TRUE;
				//
				oper.InsertEditPropItem(pSonPropItem,"component.SectionView.startPt.x");
				//
				oper.InsertEditPropItem(pSonPropItem,"component.SectionView.startPt.y");
				//�յ�
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
		//ͼ������
		oper.InsertCmbListPropItem(pGroupItem,"component.m_nAnchorType");
		//ָ����ͼλ��
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
			char sAnchorType[50]="λ��";
			if(pDrawingComponent->m_nAnchorType==0)
				strcpy(sAnchorType,"���Ͻ�");
			else if(pDrawingComponent->m_nAnchorType==1)
				strcpy(sAnchorType,"���Ͻ�");
			else if(pDrawingComponent->m_nAnchorType==2)
				strcpy(sAnchorType,"���½�");
			else if(pDrawingComponent->m_nAnchorType==3)
				strcpy(sAnchorType,"���½�");
#endif
			//
			pPropItem=oper.InsertButtonPropItem(pGroupItem,"component.m_ptLocation");
#ifdef AFX_TARG_ENU_ENGLISH
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("view%s offset position relative to the top left corner of drawing frame.",sAnchorType);
#else
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("��ͼ%s�����ͼ�����Ͻǵ�ƫ��λ�á�",sAnchorType);
#endif
			pPropItem->m_bHideChildren=TRUE;
			pPropItem->SetReadOnly(pDrawingComponent->ParentDrawing()->m_bAutoLayoutComponent);
			//
			pSonPropItem=oper.InsertEditPropItem(pPropItem,"component.m_ptLocation.x");
#ifdef AFX_TARG_ENU_ENGLISH
			pSonPropItem->m_lpNodeInfo->m_strPropHelp.Format("view%s offset position in X direction relative to the top left corner of drawing frame.",sAnchorType);
#else
			pSonPropItem->m_lpNodeInfo->m_strPropHelp.Format("��ͼ%s�����ͼ�����Ͻ�X�����ƫ��ֵ��",sAnchorType);
#endif
			pSonPropItem->SetReadOnly(pDrawingComponent->ParentDrawing()->m_bAutoLayoutComponent);
			//
			pSonPropItem=oper.InsertEditPropItem(pPropItem,"component.m_ptLocation.y");
#ifdef AFX_TARG_ENU_ENGLISH
			pSonPropItem->m_lpNodeInfo->m_strPropHelp.Format("view%s offset position in Y direction relative to the top left corner of drawing frame.",sAnchorType);
#else
			pSonPropItem->m_lpNodeInfo->m_strPropHelp.Format("��ͼ%s�����ͼ�����Ͻ�Y�����ƫ��ֵ��",sAnchorType);
#endif
			pSonPropItem->SetReadOnly(pDrawingComponent->ParentDrawing()->m_bAutoLayoutComponent);
		}
		//
		if(pDrawingComponent->m_eClassType!=DRAW_COM_BOM)
		{	//��ͼ����ϵ
			oper.InsertButtonPropItem(pGroupItem,"component.dcs");
		}
		if(pDrawingComponent->m_eClassType==DRAW_COM_BOM
			&&((CDrawingBOM*)pDrawingComponent)->m_eBOMType==BOM_SEGPART)
		{	//�ֶι�����ϸ��֧���Զ���ҳ����
			oper.InsertCmbListPropItem(pGroupItem,"drawingBOM.m_bAutoNewTable");
			//
			CDrawingBOM *pDrawingBOM=(CDrawingBOM*)pDrawingComponent;
			if(pDrawingBOM->m_bAutoNewTable)
				oper.InsertCmbListPropItem(pGroupItem,"drawingBOM.m_bHasTableHead");
		}
		else if(pDrawingComponent->m_eClassType==DRAW_COM_BOM
			&&((CDrawingBOM*)pDrawingComponent)->m_eBOMType==BOM_GUIGE)
		{	//�򻯹���---�򻯹���� 
			oper.InsertButtonPropItem(pGroupItem,"drawingBOM.nBriefGuigeN");
		}
		if(pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW
			&&((CLineView*)pDrawingComponent)->m_bAnalysisMode)
		{	//��ע�ڵ���
			oper.InsertCmbListPropItem(pGroupItem,"lineView.m_bDimNodeNo");
			//
			oper.InsertCmbListPropItem(pGroupItem,"lineView.m_bDimPoleGuige");
		}
		if(pDrawingComponent->m_eClassType==DRAW_COM_LINEVIEW
			||pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW
			||pDrawingComponent->m_eClassType==DRAW_COM_PARTVIEW)
		{
			//��ע���Ʊ���
			oper.InsertCmbListPropItem(pGroupItem,"component.m_bDimScale");
		}
		//��ע��ͼ����
		oper.InsertCmbListPropItem(pGroupItem,"component.m_bDimName");
		//
		if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW)
		{
			pPropItem=oper.InsertEditPropItem(pGroupItem,"struView.partNo");
			pPropItem->m_bHideChildren = FALSE;
			//������ϸ�ԳƷ�ʽ 0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
			oper.InsertCmbListPropItem(pPropItem,"struView.partNo.m_iMirMsg");
			oper.InsertCmbListPropItem(pPropItem,"struView.partNo.m_bDrawRefLine");
		}
		if(pDrawingComponent->m_eClassType==DRAW_COM_PARTVIEW)
		{	//���ͼ
			oper.InsertEditPropItem(pGroupItem,"PartView.sPartNo");
			pPropItem=oper.InsertCmbEditPropItem(pGroupItem,"PartView.hPart");
			pPropItem->m_lpNodeInfo->m_cmbItems=MakePartHandleSetString(((CPartDrawing*)pDrawingComponent)->sPartNo);
		}
	}
	else if(m_pItemInfo&&m_pItemInfo->itemType==1)
	{	//�ṹͼĿ¼�� ��ʾ�ṹͼ��������
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
				sText.Copy("��");
			else 
				sText.Copy("��");
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
				sText.Copy("��");
			else 
				sText.Copy("��");
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
		//����߻���˾��ͼ
		else if(GetPropID("LineView::m_bUsePolyLineChiefMap")==id)
		{
			if(CLineView::m_bUsePolyLineChiefMap)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Polygon line");
			else 
				sText.Copy("Ordinary straight line");
#else
				sText.Copy("�����");
			else 
				sText.Copy("��ֱͨ��");
#endif
		}
		//����߻�����ͼ
		else if(GetPropID("LineView::m_bUsePolyLineSumMap")==id)
		{
			if(CLineView::m_bUsePolyLineSumMap)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Polygon line");
			else 
				sText.Copy("Ordinary straight line");
#else
				sText.Copy("�����");
			else 
				sText.Copy("��ֱͨ��");
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
					sText.Copy("˾��ͼ");
				else 
					sText.Copy("����ͼ");
			}
			else if(pDrawingComponent->m_eClassType==DRAW_COM_STRUVIEW)
				sText.Copy("�ṹͼ");
			else if(pDrawingComponent->m_eClassType==DRAW_COM_STDVIEW)
				sText.Copy("��׼ͼ");
			else if(pDrawingComponent->m_eClassType==DRAW_COM_PARTVIEW)
				sText.Copy("���ͼ");
			else if(pDrawingComponent->m_eClassType==DRAW_COM_BOM)
				sText.Copy("�����嵥");
			else
				sText.Copy("����");
#endif
		}
		else if(GetPropID("component.m_hModule")==id)
		{	//��ͼ��������
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
				sText.Copy("��");
			else
				sText.Copy("��");
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
				sText.Copy("��");
			else
				sText.Copy("��");
#endif
		}
		else if(GetPropID("component.m_sDrawingLayer")==id)
			sText.Printf("%s",(char*)pDrawingComponent->m_sDrawingLayer);
		else if(GetPropID("component.rect")==id)
		{
			f2dRect rect=pDrawingComponent->GetComponentRect();
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%.f��%.f",rect.Width(),rect.Height());
#else
			sText.Printf("%.f��%.f",rect.Width(),rect.Height());
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
				sText.Copy("����");
			else 
				sText.Copy("����");
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
				sText.Printf("0.X��Գ�");
			else if(pStruView->m_iMirMsg==1)
				sText.Printf("1.Y��Գ�");
			else 
				sText.Printf("2.�޶Գ�");
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
				sText.Printf("��");
			else 
				sText.Printf("��");
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
			if(pDrawingComponent->m_nAnchorType==TOP_LEFT)			//���Ͻ�
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("0.Top left corner");
			else if(pDrawingComponent->m_nAnchorType==TOP_RIGHT)	//top right corner
				sText.Printf("1.Top right corner");
			else if(pDrawingComponent->m_nAnchorType==BOTTOM_LEFT)	//lower left corner
				sText.Printf("2.Lower left quarter");
			else //if(pDrawingComponent->m_nAnchorType==BOTTOM_RIGHT)//lower right corner
				sText.Printf("3.Lower right corner");
#else
				sText.Printf("0.���Ͻ�");
			else if(pDrawingComponent->m_nAnchorType==TOP_RIGHT)	//���Ͻ�
				sText.Printf("1.���Ͻ�");
			else if(pDrawingComponent->m_nAnchorType==BOTTOM_LEFT)	//���½�
				sText.Printf("2.���½�");
			else //if(pDrawingComponent->m_nAnchorType==BOTTOM_RIGHT)//���½�
				sText.Printf("3.���½�");
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
					sText.Copy("��");
				else
					sText.Copy("��");
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
					sText.Copy("��");
				else
					sText.Copy("��");
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
				sText.Copy("��");
			else
				sText.Copy("��");
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
				sText.Copy("��");
			else
				sText.Copy("��");
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
				sText.Copy("��");
			else
				sText.Copy("��");
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
				sText.Copy("��");
			else
				sText.Copy("��");
#endif
		}
	}
	//�ṹͼ��������
	else if(GetPropID("StruView::m_bDimPartSpec")==id)
	{
		if(CStruView::m_bDimPartSpec)
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
	else if(GetPropID("StruView::m_bDimPoleOddment")==id)
	{
		if(CStruView::m_bDimPoleOddment)
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
	else if(GetPropID("StruView::m_bDimFootNailSpace")==id)
	{
		if(CStruView::m_bDimFootNailSpace)
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
	else if(GetPropID("StruView::m_bDimFillPlateSpace")==id)
	{
		if(CStruView::m_bDimFillPlateSpace)
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
	else if(GetPropID("StruView::m_bDimLsSpace")==id)
	{
		if(CStruView::m_bDimLsSpace)
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
	else if(GetPropID("StruView::m_bDimJgZhunJu")==id)
	{
		if(CStruView::m_bDimJgZhunJu)
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
	else if(GetPropID("StruView::m_bDimBoltInfo")==id)
	{
		if(CStruView::m_bDimBoltInfo)
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
	else if(GetPropID("StruView::m_bDimBoltPad")==id)
	{
		if(CStruView::m_bDimBoltPad)
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
	else if(GetPropID("StruView::m_bDimSimPoly")==id)
	{
		if(CStruView::m_bDimSimPoly)
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
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}

void CSelectDrawingDlg::OnDblclkTreeDrawing(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/*HTREEITEM hItem=m_treeDrawing.GetSelectedItem();	
	//������ǰ��������ֵܽڵ㴦���£״̬
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
	{	//��¼��ǰͼֽ�Ķκż�����,�ٴδ�ʱѡ�и�ͼֽ wht 11-06-17
		nSegI=m_pDrawing->m_nSegI;
		sDrawingName.Copy(m_pDrawing->m_sDrawingName);
		if(m_pComponent)
			hComKey=m_pDrawing->listComponents.GetKey(m_pComponent);
	}
	if(bReset)
	{	//����ͼֽ�б�
		drawingList.Empty();	//���ͼֽ����
		InitDrawingList();		//���³�ʼ��ͼֽ����
	}
	else 
	{	//����ͼֽ�б�
		CDrawing *pDrawing=NULL;
		for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
			pDrawing->UpdateCmdList();
	}
	if(sDrawingName.GetLength()>0)
	{	//����ͼֽ���Ƽ��κ�ѡ�б����ļ�֮ǰ��ͼֽ
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
	RefreshDrawingTree(m_pDrawing,m_pComponent);	//ˢ�����ͼֽ�б���
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
	{	//��¼��ǰͼֽ�Ķκż�����,�ٴδ�ʱѡ�и�ͼֽ wht 11-06-17
		nSegI=m_pDrawing->m_nSegI;
		sDrawingName.Copy(m_pDrawing->m_sDrawingName);
		if(m_pComponent)
			hComKey=m_pDrawing->listComponents.GetKey(m_pComponent);
	}
	Ta.AttachBuffer[CTower::SMART_DRAW].ClearBuffer();	//��սű�����
	//�����ͼ�ű���Ta.m_bufferDrawingScript
	//�趨�������Ϊ5M����,�Է�Ƶ�������ͷ��ڴ�(�׵����ڴ���Ƭ�����ӻ�ʱ) wjh-2011.8.6
	Ta.AttachBuffer[CTower::SMART_DRAW].SetBlockSize(5000000);
	SaveScriptToBuffer();
	SaveTma();	//�����ļ�
	if(sDrawingName.GetLength()>0)
	{	//����ͼֽ���Ƽ��κ�ѡ�б����ļ�֮ǰ��ͼֽ
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
	//RefreshDrawingTree(m_pDrawing,m_pComponent);	//ˢ�����ͼֽ�б���
}

void CSelectDrawingDlg::OnClickTreeDrawing(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TVHITTESTINFO HitTestInfo;
	GetCursorPos(&HitTestInfo.pt);
	m_treeDrawing.ScreenToClient(&HitTestInfo.pt);
	if(m_treeDrawing.HitTest(&HitTestInfo)==NULL)
		return;
	m_treeDrawing.Select(HitTestInfo.hItem,TVGN_CARET);       
	if(HitTestInfo.flags&TVHT_ONITEMSTATEICON)	//�����CheckBox��       
	{       
		BOOL bCheck=m_treeDrawing.GetCheck(HitTestInfo.hItem);
		HTREEITEM hChildItem=m_treeDrawing.GetChildItem(HitTestInfo.hItem);
		while(hChildItem)
		{
			m_treeDrawing.SetCheck(hChildItem,!bCheck);
			hChildItem=m_treeDrawing.GetNextItem(hChildItem,TVGN_NEXT);
		}
	}  
	//if(HitTestInfo.flags&TVHT_ONITEMBUTTON)//�����+����
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
	m_bModeless=TRUE;	//��ģ̬
	return TRUE;
}

void CSelectDrawingDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	if(m_bModeless)
		delete this;	//ɾ���Լ���ռ��Դ
	else 
		ReleaseCapture();
	if(m_bRestartDlg)
	{	//PostNcDestroy()��ʱ���ã���δ���? ���DoModal��ؼ���DrawItem�����޷���Ӧ? wht 11-03-16
		m_bRestartDlg=FALSE;
		DoModal();	
	}
}

LRESULT CSelectDrawingDlg::OnAcadKeepFocus(WPARAM, LPARAM)
{	//��֤��ģ̬�Ի�����Ի�ý���
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
		ShowWindow(SW_HIDE);	//���ضԻ���
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
		HideWnd(FALSE);	//��겻�ڵ�ǰ������
	CDialog::OnLButtonDown(nFlags, point);
}

void CSelectDrawingDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetClientRect(&rect);
	if(!rect.PtInRect(point)) 
		HideWnd(FALSE);	//��겻�ڵ�ǰ������
	CDialog::OnRButtonDown(nFlags, point);
}

void CSelectDrawingDlg::OnMButtonDown(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetClientRect(&rect);
	if(!rect.PtInRect(point)) 
		HideWnd(FALSE);	//��겻�ڵ�ǰ������
	CDialog::OnMButtonDown(nFlags, point);
}
void CSelectDrawingDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetClientRect(&rect);
	CWnd *pWnd=GetCapture();
	BOOL bInRect=rect.PtInRect(point);
	if(bInRect&&pWnd==this)
		ReleaseCapture();	//����ڶԻ���Χ�ڲ�����ִ�й�SetCapture()
	else if(!bInRect&&(pWnd==NULL||pWnd!=this))		
		SetCapture();		//��겻�ڶԻ���Χ����δִ�й�SetCapture()
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
	HideWnd(TRUE);	//���ش���
	CAcModuleResourceOverride resOverride;
	//��¼�˼�ʼ�ն�������
	double fOldStartOutSpreadLen=sys.stru_map.fPoleAxisLineStartOutSpreadLen;
	double fOldEndOutSpreadLen=sys.stru_map.fPoleAxisLineEndOutSpreadLen;
	DRAGSET.ClearEntSet();
	CDrawMapScriptDlg scriptDlg;
	scriptDlg.m_bCallByDrawingDlg=TRUE;
	scriptDlg.SetCallBackFunc(FireRunCommand);
	scriptDlg.m_pDrawing=m_pDrawing;
	scriptDlg.m_pComponent=m_pComponent;
	m_pComponent->InitCmdFeature(-1);	//��ʼ������feature=-1,�����Ի��򱣳�˳��ʱ�õ� wht 11-07-23
	if(scriptDlg.DoModal()==IDOK)
	{
		m_pComponent->UpdateDrawingRectArray();
		m_pComponent->InsertDrawingName();	//��ע��ͼ�����Լ��ϽӶκ�
	}
	//�ָ��˼�ʼ�ն�������
	sys.stru_map.fPoleAxisLineStartOutSpreadLen=fOldStartOutSpreadLen;
	sys.stru_map.fPoleAxisLineEndOutSpreadLen=fOldEndOutSpreadLen;
}
void CSelectDrawingDlg::DeleteItemInfo(CDrawing *pDrawing)
{	//ɾ��pDrawing�������������itemInfo
	TREEITEM_INFO *pItem=NULL;
	for(pItem=itemInfoList.GetFirst();pItem;pItem=itemInfoList.GetNext())
	{
		if(pItem->itemType==1)
		{	//ɾ��ͼֽ��Ӧ��itemInfo
			CDrawing *pCurDrawing=(CDrawing*)pItem->dwRefData;
			if(pCurDrawing&&pDrawing==pCurDrawing)
				itemInfoList.DeleteCursor(TRUE);
		}
		else if(pItem->itemType==2)
		{	//ɾ��ͼֽ�и���ͼԪ��Ӧ��ItemInfo
			CDrawingComponent *pCom=(CDrawingComponent*)pItem->dwRefData;
			if(pCom&&pCom->ParentDrawing()&&pCom->ParentDrawing()==pDrawing)
				itemInfoList.DeleteCursor(TRUE);
		}
	}
}
void CSelectDrawingDlg::DeleteItemInfo(CDrawingComponent *pComponent)
{	//ɾ��pComponent������itemInfo
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

//���汾�ŵı������ȡ����һ�������� wht 11-08-09
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
		"��ͼ�ű�(*.dms)|*.dms|�����ļ�(*.*)|*.*||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	FILE *fp = fopen(dlg.GetPathName(),"rb");
	if(fp==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("specified file cannot be opened.");
#else
		AfxMessageBox("�򲻿�ָ���ļ���");
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
	
	//ͼֽ�б�
	long buffer_len=buffer.GetLength();
	long version=0;	//��ʼ(��С)�汾��Ϊ:1000001
	if(!SerializeDrawScriptHeader(buffer,FALSE,&version))
		return;
	//
	HTREEITEM hDrawingItem=m_treeDrawing.GetSelectedItem();
	if(hDrawingItem==NULL)
		return;
	if(m_pItemInfo==NULL)
		m_pItemInfo=(TREEITEM_INFO*)m_treeDrawing.GetItemData(hDrawingItem);
	/*if(m_pItemInfo==NULL)
	{	//��ʼ��ͼֽfeature=0,��������ͬһ�κŴ��ڶ���ͼֽ�����
		CDrawing *pDrawing=NULL;
		for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
			pDrawing->feature=0;
		int i=0,nDrawingCount=0;
		buffer.ReadInteger(&nDrawingCount);
		for(i=0;i<nDrawingCount;i++)
		{
			if(Ta.DisplayProcess)
				Ta.DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),"��ͼ�ű���ȡ����");
			CDrawing drawing;
			drawing.FromBuffer(buffer,version);
			//������ǰͼֽ�б��鿴�Ƿ����ͬ�κ�ͼֽ
			CDrawing *pDestDrawing=NULL;
			for(pDestDrawing=drawingList.GetFirst();pDestDrawing;pDestDrawing=drawingList.GetNext())
			{
				if(drawing.m_nSegI==pDestDrawing->m_nSegI&&pDestDrawing->feature==0)
					break;	
			}
			if(pDestDrawing)	//����,ֱ�Ӹ�������
				pDestDrawing->CopyProperty(&drawing);
			else				//������,�½�һ��ͼֽ
			{
				if(drawing.m_nSegI>=0)
				{	//���ҵ�һ�����ڵ�ǰͼֽ�κŵ�ͼֽ,���¼�ͼֽ������ǰһ��λ��
					for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
					{
						if(pDrawing->m_nSegI>drawing.m_nSegI)
							break;
					}
				}
				pDestDrawing=drawingList.insert(0);
				pDestDrawing->CopyProperty(&drawing);
			}
			pDestDrawing->feature=1;	//��ʾ�Ѵ����
		}
	}
	else*/
	if(m_pItemInfo&&m_pItemInfo->itemType==1) 
	{	//ͼֽ
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
		Ta.DisplayProcess(100,"��ͼ�ű���ȡ����");
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
	//����ָ���Ļ�ͼ�ű�
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
		//ͼֽ�б�
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
			if((importDlg.m_bAnalysisMap&&pDrawing->m_nSegI==-1)//˾��ͼ
				||(importDlg.m_bSumMap&&pDrawing->m_nSegI==-2)	//��ͼ
				||(nSegI>=0&&nSegI==pDrawing->m_nSegI))			//�ṹͼ
			{
				pDrawing->ToBuffer(buffer);
				nDrawingCount++;
			}
		}
		//��дͼֽ����
		long pos=buffer.GetCursorPosition();
		buffer.SeekPosition(pos_start);
		buffer.WriteInteger(nDrawingCount);
		buffer.SeekPosition(pos);
	}
	else*/
	CString sScriptName="DrawMapScript";
	if(m_pItemInfo&&m_pItemInfo->itemType==1) 
	{	//ͼֽ
		CString sItemName=m_treeDrawing.GetItemText(hDrawingItem);
		sScriptName.Format("%s-%s",g_sFileName,sItemName);
		CDrawing *pDrawing=(CDrawing*)m_pItemInfo->dwRefData;
		if(pDrawing)	//����ѡ��ͼֽ
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
		"��ͼ�ű�(*.dms)|*.dms|�����ļ�(*.*)|*.*||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	FILE *fp = fopen(dlg.GetPathName(),"wb");
	if(fp==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Drawing script reading progress!");
#else
		AfxMessageBox("�򲻿�ָ���ļ���");
#endif
		return;
	}
	long file_len=buffer.GetLength();
	fwrite(&file_len,sizeof(long),1,fp);
	fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);
}
#endif

