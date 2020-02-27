//<LOCALE_TRANSLATE Confirm by hxr/>
// SelFacePanelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "BitmapGraph.h"
#include "SelFacePanelDlg.h"
#include "StdFacePanel.h"
#include "FacePanelDetailDlg.h"
#include "GlobalFunc.h"
#include "comdef.h"
#include "Expression.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelFacePanelDlg dialog

static BOOL ModifyItemValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CSelFacePanelDlg *pSelPanelDlg=(CSelFacePanelDlg*)pPropList->GetParent();
	if(pItem->m_idProp==1)	//�ڼ�����
	{
		pSelPanelDlg->m_nInternodeNum=atoi(valueStr);
	}
	else if(pItem->m_idProp==2)	//ͼ�㷽λ�ַ�
		pSelPanelDlg->m_cThirdLayer=valueStr[0];
	else if(pItem->m_idProp==3)	//�ؼ�������
	{
		pSelPanelDlg->m_nKeyNodeNum=atoi(valueStr);
		pSelPanelDlg->ChangeKeyNodeNum();
	}
	else if(pItem->m_idProp==4)	//�ؼ���Գ�
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("No Mirror")==0)
			pSelPanelDlg->m_iKeyNodeMir=0;
		else if(valueStr.CompareNoCase("X Axis")==0)
			pSelPanelDlg->m_iKeyNodeMir=1;
		else if(valueStr.CompareNoCase("Y Axis")==0)
			pSelPanelDlg->m_iKeyNodeMir=2;
		else if(valueStr.CompareNoCase("XYZ Axis")==0)
#else
		if(valueStr.CompareNoCase("�޶Գ�")==0)
			pSelPanelDlg->m_iKeyNodeMir=0;
		else if(valueStr.CompareNoCase("X��")==0)
			pSelPanelDlg->m_iKeyNodeMir=1;
		else if(valueStr.CompareNoCase("Y��")==0)
			pSelPanelDlg->m_iKeyNodeMir=2;
		else if(valueStr.CompareNoCase("XYZ��")==0)
#endif
			pSelPanelDlg->m_iKeyNodeMir=3;
		else
			return FALSE;
	}
	else if(pItem->m_idProp==5)	//ģ����Գ�
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("No Mirror")==0)
			pSelPanelDlg->m_iStdFaceMir=0;
		else if(valueStr.CompareNoCase("X Axis")==0)
			pSelPanelDlg->m_iStdFaceMir=1;
		else if(valueStr.CompareNoCase("Y Axis")==0)
			pSelPanelDlg->m_iStdFaceMir=2;
		else if(valueStr.CompareNoCase("Z Axis")==0)
			pSelPanelDlg->m_iStdFaceMir=3;
		else if(valueStr.CompareNoCase("Mirror")==0)
			pSelPanelDlg->m_iStdFaceMir=4;
		else if(valueStr.CompareNoCase("Z Axis Mirror")==0)
#else
		if(valueStr.CompareNoCase("�޶Գ�")==0)
			pSelPanelDlg->m_iStdFaceMir=0;
		else if(valueStr.CompareNoCase("X��")==0)
			pSelPanelDlg->m_iStdFaceMir=1;
		else if(valueStr.CompareNoCase("Y��")==0)
			pSelPanelDlg->m_iStdFaceMir=2;
		else if(valueStr.CompareNoCase("Z��")==0)
			pSelPanelDlg->m_iStdFaceMir=3;
		else if(valueStr.CompareNoCase("����")==0)
			pSelPanelDlg->m_iStdFaceMir=4;
		else if(valueStr.CompareNoCase("Z��_����")==0)
			pSelPanelDlg->m_iStdFaceMir=5;
		else if(valueStr.CompareNoCase("�ǶԳ�")==0)
#endif
			pSelPanelDlg->m_iStdFaceMir=6;
		else
			return FALSE;
	}
	else if(pItem->m_idProp==pSelPanelDlg->GetPropID("acs.axis_x"))	//ģ����Գ�
	{
		int iVecType=valueStr[0]-'0';
		if(iVecType>0&&iVecType<=4)
		{
			pSelPanelDlg->m_vAcsAxisX=CStdVector::GetVector(iVecType);	//ģ����Գ�
			if(pSelPanelDlg->m_iKeyNodeMir==1&&iVecType>2)
				pSelPanelDlg->m_iKeyNodeMir=2;
			else if(pSelPanelDlg->m_iKeyNodeMir==2&&iVecType<=2)
				pSelPanelDlg->m_iKeyNodeMir=1;
			int idProp=pSelPanelDlg->GetPropID("keynodeMirStyle");
			CXhChar16 sText;
			pSelPanelDlg->GetPropValueStr(idProp,sText);
			pSelPanelDlg->m_propList.SetItemPropValue(idProp,sText);
		}
	}
	else if(pItem->m_idProp==pSelPanelDlg->GetPropID("acs.axis_x.x"))
		pSelPanelDlg->m_vAcsAxisX.x=atof(valueStr);
	else if(pItem->m_idProp==pSelPanelDlg->GetPropID("acs.axis_x.y"))
		pSelPanelDlg->m_vAcsAxisX.y=atof(valueStr);
	else if(pItem->m_idProp!=0)
	{
		DESIGN_PARAM_ITEM *pPara;
		memcpy(&pPara,&pItem->m_idProp,4);
		pPara->value.fValue=atof(valueStr);
	}
	return TRUE;
}
int CSelFacePanelDlg::m_iSelItemIndex=0;
CSelFacePanelDlg::CSelFacePanelDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CSelFacePanelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelFacePanelDlg)
	m_nKeyNodeNum = 2;
	m_iKeyNodeMir = 0;
	m_iStdFaceMir = 0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sKeyNodeOrder = _T("order selection for key points:");
#else
	m_sKeyNodeOrder = _T("�ؼ���ѡ��˳��:");
#endif
	//}}AFX_DATA_INIT
	m_cThirdLayer='Q';
	m_iTemplateGroup=1;
	m_nInternodeNum=2;
	m_uIconWidth=m_uIconHeight=m_dwIconWidth=m_dwIconHeight=60;
	m_bFirstPopup=TRUE;
	rgn_ucs.axis_x.Set(1,0,0);
	rgn_ucs.axis_y.Set(0,-1,0);
	rgn_ucs.axis_z.Set(0,0,-1);
	m_vAcsAxisX.Set(1,0,0);
	//m_pDraw=NULL;
}

void CSelFacePanelDlg::SetCurrWorkView(CDisplayView *pView)
{
	if(pView==NULL)
		return;
	//�жϵ�ǰ��������ͼ����
	VIEW_DIRECT m_eViewFlag = RANDOM_VIEW;	//��ʾ�ǵ�����ͼ������ԭ����
	if(((strchr(pView->filter.ThirdCharSet,'Q')||strchr(pView->filter.ThirdCharSet,'q'))&&
		(strchr(pView->filter.ThirdCharSet,'H')||strchr(pView->filter.ThirdCharSet,'h'))&&
		(strchr(pView->filter.ThirdCharSet,'Z')||strchr(pView->filter.ThirdCharSet,'z'))&&
		(strchr(pView->filter.ThirdCharSet,'Y')||strchr(pView->filter.ThirdCharSet,'y')))||
		strchr(pView->filter.ThirdCharSet,'?'))
		m_eViewFlag = PERSPECTIVE;
	else
	{
		if(strchr(pView->filter.ThirdCharSet,'Q') || strchr(pView->filter.ThirdCharSet,'q'))
			m_eViewFlag = FRONT_VIEW;
		else if(strchr(pView->filter.ThirdCharSet,'H') || strchr(pView->filter.ThirdCharSet,'h'))
			m_eViewFlag = BACK_VIEW;
		else if(strchr(pView->filter.ThirdCharSet,'Z') || strchr(pView->filter.ThirdCharSet,'z'))
			m_eViewFlag = LEFT_VIEW;
		else if(strchr(pView->filter.ThirdCharSet,'Y') || strchr(pView->filter.ThirdCharSet,'y'))
			m_eViewFlag = RIGHT_VIEW;
	}
	m_cThirdLayer=pView->defaultLayer[2];
	if(m_eViewFlag == FRONT_VIEW||m_eViewFlag == BACK_VIEW)
	{	//ǰ����ͼ
		if(m_iTemplateGroup==0||m_iTemplateGroup==2)	
			return;	//0��ʾ��ͷģ��&2���������
		else if(m_iTemplateGroup==1||m_iTemplateGroup==4)
		{	//1��ʾ����
			m_iKeyNodeMir=2;		//0.�޶Գƣ�1.X��Գƣ�2.Y��Գƣ�3.XYZ�Գ�
			if(m_iStdFaceMir!=5)	//0.�޶Գƣ�1.X��Գƣ�2.Y��Գƣ�3.Z��Գƣ�4.����5.Z�Գ�&����
				m_iStdFaceMir=3;
		}
		else if(m_iTemplateGroup==3)	//3:��ڼ���
		{
			if(m_iStdFaceMir==1||m_iStdFaceMir==2)	//0.�޶Գƣ�1.X��Գƣ�2.Y��Գƣ�3.Z��Գƣ�4.����5.Z�Գ�&����
				m_iStdFaceMir=2;
		}
		else if(m_iTemplateGroup==5)	//3:��ڼ���
		{	//"V�ͱ�׼��";
			m_cThirdLayer='G';
		}
		else if(m_iTemplateGroup==6)	//6.����������
		{	//"����������";
			//m_iStdFaceMir	//�ǶԳƻ�����/ǰ��/�Ķ�/����Գ�
		}
	}
	else if(m_eViewFlag == FRONT_VIEW||m_eViewFlag == BACK_VIEW)
	{	//���Ҳ���ͼ
		if(m_iTemplateGroup==0||m_iTemplateGroup==2)	
			return;	//0��ʾ��ͷģ��&2���������
		else if(m_iTemplateGroup==1||m_iTemplateGroup==4)
		{	//1��ʾ����
			m_iKeyNodeMir=1;		//0.�޶Գƣ�1.X��Գƣ�2.Y��Գƣ�3.XYZ�Գ�
			if(m_iStdFaceMir!=5)	//0.�޶Գƣ�1.X��Գƣ�2.Y��Գƣ�3.Z��Գƣ�4.����5.Z�Գ�&����
				m_iStdFaceMir=3;
		}
		else if(m_iTemplateGroup==3)	//3:��ڼ���
		{
			if(m_iStdFaceMir==1||m_iStdFaceMir==2)	//0.�޶Գƣ�1.X��Գƣ�2.Y��Գƣ�3.Z��Գƣ�4.����5.Z�Գ�&����
				m_iStdFaceMir=2;
		}
		else if(m_iTemplateGroup==5)	//3:��ڼ���
		{	//"V�ͱ�׼��";
			m_cThirdLayer='G';
		}
		else if(m_iTemplateGroup==6)	//6.����������
		{	//"����������";
			//m_iStdFaceMir	//�ǶԳƻ�����/ǰ��/�Ķ�/����Գ�
		}
	}
}

void CSelFacePanelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelFacePanelDlg)
	DDX_Control(pDX, IDC_TAB_GROUP, m_ctrlPropGroup);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelFacePanelDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CSelFacePanelDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CTRL_STD_FACE, OnItemchangedListCtrlStdFace)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_GROUP, OnSelchangeTabGroup)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelFacePanelDlg message handlers
/*void DrawPanel(CDrawEngine *pDraw)
{
	pDraw->ClearGraphicDatabase();
	pDraw->NewPoint(25,25,3);
	pDraw->NewLine(f3dPoint(0,0,0),f3dPoint(400,400,0));
}*/
BOOL CSelFacePanelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	InitPropHashtable();
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL_STD_FACE);
	m_pListCtrl->SetExtendedStyle( LVS_ICON);// | LVS_EX_REGIONAL );
	m_pListCtrl->SetBkColor(RGB(255,255,255));
	m_pListCtrl->SetIconSpacing(m_uIconWidth+5,m_uIconWidth+25);
	CPaintDC dc(this);
	m_uIconWidth=m_dwIconWidth;
	m_uIconHeight=m_dwIconHeight;
	if(m_bFirstPopup)
	{
		bitmap.InitDC(dc.GetSafeHdc(),m_uIconWidth,m_uIconHeight);
		imageList.Create(m_uIconWidth, m_uIconHeight, ILC_COLOR32,   2, 4);
	}
	else
	{
		bitmap.ReInitDC(dc.GetSafeHdc(),m_uIconWidth,m_uIconHeight);
		imageList.DeleteImageList();
		imageList.Create(m_uIconWidth, m_uIconHeight, ILC_COLOR32,   2, 4);
	}
	m_pListCtrl->SetImageList(&imageList,LVSIL_NORMAL);
	//���ɻ���
	if(solid_pen.GetSafeHandle()==0)
		solid_pen.CreatePen(PS_SOLID,2,RGB(0,0,200));
	if(wide_solid_pen.GetSafeHandle()==0)
		wide_solid_pen.CreatePen(PS_SOLID,1,RGB(255,0,0));
	if(green_solid_pen.GetSafeHandle()==0)
		green_solid_pen.CreatePen(PS_SOLID,2,RGB(0,255,0));
	if(green_dash_pen.GetSafeHandle()==0)
		green_dash_pen.CreatePen(PS_DASH,1,RGB(0,255,0));
	if(red_solid_pen.GetSafeHandle()==0)
		red_solid_pen.CreatePen(PS_SOLID,1,RGB(255,0,0));
	if(dash_pen.GetSafeHandle()==0)
		dash_pen.CreatePen(PS_DASH,1,RGB(0,0,0));
	if(dash_dot_pen.GetSafeHandle()==0)
		dash_dot_pen.CreatePen(PS_DASHDOT,1,RGB(255,0,0));

	UpdateFaceSet();
	m_bFirstPopup=FALSE;
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.m_iPropGroup=m_iTemplateGroup;
	RefreshTabCtrl(m_iTemplateGroup);
	DisplayPropertyList();
	GetWindowRect(&m_rectInitWndRgn);
	CWnd *pWnd = GetDlgItem(IDC_S_MAP);
	pWnd->GetWindowRect(&m_rectInitDrawRgn);	//��ͼ��Ч��������������
	//
	if(m_iSelItemIndex>=0&&m_iSelItemIndex<m_pListCtrl->GetItemCount())
	{	//������һ��ѡ����
		m_pListCtrl->SetSelectionMark(m_iSelItemIndex);
		m_pListCtrl->SetItemState(m_iSelItemIndex,LVIS_SELECTED|LVNI_FOCUSED,LVIS_SELECTED|LVNI_FOCUSED);
	}
	MoveWndToLocation();
	//��ʼ����ͼ����
	/*m_pDraw = CDrawEngine::CreateEngine();
	RECT rectClient;
	rectClient.top=m_rectInitDrawRgn.top;
	rectClient.bottom=m_rectInitDrawRgn.bottom;
	rectClient.left=m_rectInitDrawRgn.left;
	rectClient.right=m_rectInitDrawRgn.right;
	ScreenToClient(&rectClient);
	m_pDraw->Init(GetSafeHwnd(),&rectClient);
	CPoint scr_org(rectClient.left,rectClient.top);
	m_pDraw->SetViewportOrg(scr_org.x,scr_org.y);
	UCS_STRU cs;
	LoadDefaultUCS(&cs);
	//cs.axis_x.Set(1,1);
	//cs.axis_y.Set(-1,1);
	scr_org.y=rectClient.bottom;
	//scr_org.x+=10;
	//scr_org.y-=20;
	m_pDraw->FixView(scr_org,cs,3);
	m_pDraw->SetBkColor(RGB(155,155,255));
	DrawPanel(m_pDraw);*/
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CSelFacePanelDlg::GetPropValueStr(int idProp, char *valueStr)
{
	char sText[100]="";
	if(idProp==3)	//�ؼ�������
	{
		sprintf(sText,"%d",m_nKeyNodeNum);
	}
	else if(idProp==4)	//�ؼ���Գ�
	{
		if(m_iKeyNodeMir==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"No Mirror");
		else if(m_iKeyNodeMir==1)
			strcpy(sText,"X Axis");
		else if(m_iKeyNodeMir==2)
			strcpy(sText,"Y Axis");
		else if(m_iKeyNodeMir==3)
			strcpy(sText,"XYZ Axis");
#else
			strcpy(sText,"�޶Գ�");
		else if(m_iKeyNodeMir==1)
			strcpy(sText,"X��");
		else if(m_iKeyNodeMir==2)
			strcpy(sText,"Y��");
		else if(m_iKeyNodeMir==3)
			strcpy(sText,"XYZ��");
#endif
		else
			return 0;
	}
	else if(idProp==5)	//ģ����Գ�
	{
		if(m_iStdFaceMir==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"No Mirror");
		else if(m_iStdFaceMir==1)
			strcpy(sText,"X Axis");
		else if(m_iStdFaceMir==2)
			strcpy(sText,"Y Axis");
		else if(m_iStdFaceMir==3)
			strcpy(sText,"Z Axis");
		else if(m_iStdFaceMir==4)
			strcpy(sText,"Mirror");
		else if(m_iStdFaceMir==5)
			strcpy(sText,"Z Axis Mirror");
		else if(m_iStdFaceMir==6)
			strcpy(sText,"Corner Mirror");
#else
			strcpy(sText,"�޶Գ�");
		else if(m_iStdFaceMir==1)
			strcpy(sText,"X��");
		else if(m_iStdFaceMir==2)
			strcpy(sText,"Y��");
		else if(m_iStdFaceMir==3)
			strcpy(sText,"Z��");
		else if(m_iStdFaceMir==4)
			strcpy(sText,"����");
		else if(m_iStdFaceMir==5)
			strcpy(sText,"Z��_����");
		else if(m_iStdFaceMir==6)
			strcpy(sText,"�ǶԳ�");
#endif
		else
			return 0;
	}
	else if(idProp==GetPropID("acs.axis_x"))	//ģ����Գ�
		strcpy(sText,CStdVector::GetVecTypeString(m_vAcsAxisX));
	else if(idProp==GetPropID("acs.axis_x.x"))
		sprintf(sText,"%g",this->m_vAcsAxisX.x);
	else if(idProp==GetPropID("acs.axis_x.y"))
		sprintf(sText,"%g",this->m_vAcsAxisX.y);
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}

static bool IsHasAlphabet(char* str)
{
	int i,str_len=strlen(str);
	for(i=0;i<str_len;i++)
	{
		if((str[i]<'0'||str[i]>'9')&&str[i]!='.'&&str[i]!='-'&&str[i]!=' ')
			return true;
	}
	return false;
}

void CSelFacePanelDlg::AddHeadTemplate(CParaTemplate &headTemplate)
{
	CBitmap bmap;
	CPaintDC dc(this);
	CPen bluePen,redPen,greenPen;
	CBrush redBrush,greenBrush;
	//CHeadParameter *pPara;
	EXPRESSION_VAR *pVar;
	CExpression expression;
	expression.varList.Empty();
	UCS_STRU rot_ucs;	//ͷ����׼ģ����ڻ�ͼ������ǰ��ת�����任����ϵ
	UCS_STRU object_ucs;//����ʾ����ϵ������ͼԪ��������ϵ
	object_ucs.axis_x.Set(1,0,0);
	object_ucs.axis_y.Set(0,0,1);
	object_ucs.axis_z.Set(0,-1,0);
	
	redPen.CreatePen(PS_SOLID,1,RGB(255,0,0));
	bluePen.CreatePen(PS_SOLID,1,RGB(0,0,255));
	greenPen.CreatePen(PS_SOLID,1,RGB(0,255,0));
	redBrush.CreateSolidBrush(RGB(255,0,0));
	greenBrush.CreateSolidBrush(RGB(0,255,0));
	bmap.CreateCompatibleBitmap(&dc,m_uIconWidth,m_uIconHeight);
	
	//�ѱ��ʽת��Ϊ��ֵ
	for(DESIGN_PARAM_ITEM *pPara=headTemplate.listParams.GetFirst();pPara;pPara=headTemplate.listParams.GetNext())
		expression.varList.Append(ConvertParamItemToExprVar(pPara));
	/*for(pPara=headTemplate.para_list.GetFirst();pPara;pPara=headTemplate.para_list.GetNext())
	{
		if(strlen(pPara->startposx_expression)>0)
			pPara->m_dimStart.x=expression.SolveExpression(pPara->startposx_expression);
		if(strlen(pPara->startposy_expression)>0)
			pPara->m_dimStart.y=expression.SolveExpression(pPara->startposy_expression);
		if(strlen(pPara->startposz_expression)>0)
			pPara->m_dimStart.z=expression.SolveExpression(pPara->startposz_expression);
		if(strlen(pPara->endposx_expression)>0)
			pPara->m_dimEnd.x=expression.SolveExpression(pPara->endposx_expression);
		if(strlen(pPara->endposy_expression)>0)
			pPara->m_dimEnd.y=expression.SolveExpression(pPara->endposy_expression);
		if(strlen(pPara->endposz_expression)>0)
			pPara->m_dimEnd.z=expression.SolveExpression(pPara->endposz_expression);
	}*/
	for(CParaNode* pNode=headTemplate.listParaNodes.GetFirst();pNode;pNode=headTemplate.listParaNodes.GetNext())
	{
		if(IsHasAlphabet(pNode->posx_expression))
			pNode->pos.x=expression.SolveExpression(pNode->posx_expression);
		pVar=expression.varList.Append();
		pVar->fValue=pNode->pos.x;
		sprintf(pVar->variableStr,"%s_X",pNode->keyLabel.Label());
		if(IsHasAlphabet(pNode->posy_expression))
			pNode->pos.y=expression.SolveExpression(pNode->posy_expression);
		pVar=expression.varList.Append();
		pVar->fValue=pNode->pos.y;
		sprintf(pVar->variableStr,"%s_Y",pNode->keyLabel.Label());
		if(IsHasAlphabet(pNode->posz_expression))
			pNode->pos.z=expression.SolveExpression(pNode->posz_expression);
		pVar=expression.varList.Append();
		pVar->fValue=pNode->pos.z;
		sprintf(pVar->variableStr,"%s_Z",pNode->keyLabel.Label());
	}

	bitmap.Clear();
	CPen *pOldPen=bitmap.GetDC()->SelectObject(&redPen);
	CBrush *pOldBrush=bitmap.GetDC()->SelectObject(&redBrush);
	bitmap.GetDC()->SetBkMode(TRANSPARENT);
	SCOPE_STRU scope;
	for(pNode=headTemplate.listParaNodes.GetFirst();pNode;pNode=headTemplate.listParaNodes.GetNext())
	{
		f3dPoint pos=pNode->pos;
		coord_trans(pos,object_ucs,TRUE);
		//coord_trans(pos,headTemplate.rot_ucs,FALSE);
		scope.VerifyVertex(pos);
	}
	//0.7��Ҫ������ͼƬ�ܹ�ȫ������ͼ�β���һ����ԣ�ռ�
	double scale=0.8*min(m_uIconWidth/scope.wide(),m_uIconHeight/scope.high());
	rot_ucs=headTemplate.rot_ucs;
	rot_ucs.origin.Set((scope.fMinX+0.5*scope.wide()-0.5*m_uIconWidth/scale),(scope.fMinY+0.5*scope.high()+0.5*m_uIconHeight/scale));
	rot_ucs.axis_y*=-1;

	f3dPoint pos;
	f3dLine line;
	f2dLine line2d;
	bitmap.GetDC()->SelectObject(&greenPen);//bluePen);
	for(CParaRod* pPole=headTemplate.listParaRods.GetFirst();pPole;pPole=headTemplate.listParaRods.GetNext())
	{ 
		line.startPt=headTemplate.listParaNodes[pPole->keyStartNode].pos;
		line.endPt=headTemplate.listParaNodes[pPole->keyEndNode].pos;
		//�ɽ�ͼԪ����ת������ʾ����ϵ��
		coord_trans(line.startPt,object_ucs,TRUE);
		coord_trans(line.endPt,object_ucs,TRUE);
		//��ͼԪ��ת��ָ���ķ�λ
		coord_trans(line.startPt,rot_ucs,FALSE);
		coord_trans(line.endPt,rot_ucs,FALSE);
		line.startPt *= scale;
		line.endPt   *= scale;
		
		line.startPt.y+=1;	//��1��Ϊ���ڼ�ͼ��ѡ��ʱ��Ӱ��ʾʱ��ȫ�������ε�
		line.endPt.y+=1;	//��1��Ϊ���ڼ�ͼ��ѡ��ʱ��Ӱ��ʾʱ��ȫ�������ε�
		bitmap.GetDC()->MoveTo(ftoi(line.startPt.x),ftoi(line.startPt.y));
		bitmap.GetDC()->LineTo(ftoi(line.endPt.x),ftoi(line.endPt.y));
	}
	BITMAP bm;
	bitmap.GetBitmap(&bm);
	DWORD dwCount=m_uIconWidth*m_uIconHeight*4;
	BYTE *data =new BYTE[dwCount];
	bitmap.GetBitmapBits(dwCount,data);
	bmap.SetBitmapBits(dwCount,data);
	int face_i=imageList.Add(&bmap,RGB(0,0,0));
	bitmap.GetDC()->SelectObject(pOldPen);	//ѡ��ԭ����
	bitmap.GetDC()->SelectObject(pOldBrush);//ѡ��ԭ��ˢ
	m_pListCtrl->InsertItem(m_pListCtrl->GetItemCount(),headTemplate.sLabel,face_i);
	delete []data;
}
void CSelFacePanelDlg::AddFacePanel(CFacePanel &panel)
{
	CBitmap bmap;
	CPaintDC dc(this);
	CPen pen;
	DWORD dwCount=m_uIconWidth*m_uIconHeight*4;
	BYTE *data =new BYTE[dwCount];
	pen.CreatePen(PS_SOLID,1,RGB(0,255,0));
	bmap.CreateCompatibleBitmap(&dc,m_uIconWidth,m_uIconHeight);
	if(m_iTemplateGroup>0)
		panel.m_iFaceType=m_iTemplateGroup;
	SCOPE_STRU scope;
	for(CFacePanelNode *pNode=panel.node_list.GetFirst();pNode;pNode=panel.node_list.GetNext())
	{
		if(panel.m_iFaceType!=2&&pNode->m_bInternodeRepeat)
			continue;
		scope.VerifyVertex(f3dPoint(pNode->pos.x,pNode->pos.y));
	}
	CPen *pOldPen=bitmap.GetDC()->SelectObject(&pen);
	bitmap.Clear();
	double scale=0.9*min(m_uIconWidth/scope.wide(),m_uIconHeight/scope.high());
	UCS_2dSTRU ucs;
	ucs.origin.Set((scope.fMinX+0.5*scope.wide()-0.5*m_dwIconWidth/scale),(scope.fMinY+0.5*scope.high()+0.5*m_dwIconHeight/scale));
	ucs.axis_x.Set(1,0);
	ucs.axis_y.Set(0,-1);
	CExpression expression;
	EXPRESSION_VAR *pVar=expression.varList.Append();
	strcpy(pVar->variableStr,"N");
	if(panel.m_iFaceType!=2)
		pVar->fValue=panel.m_nInternodeNum;
	else
		pVar->fValue=1;
	pVar=expression.varList.Append();
	strcpy(pVar->variableStr,"i");
	int i,repeatNum=1;
	if(panel.m_iFaceType!=2)
		repeatNum=panel.m_nInternodeNum-panel.m_nRepeatLessNum;
	for(i=0;i<repeatNum;i++)
	{
		for(CFacePanelPole *pPole=panel.pole_list.GetFirst();pPole;pPole=panel.pole_list.GetNext())
		{
			if(i>0&&!pPole->m_bInternodeRepeat)
				continue;
			CFacePanelNode *pStart=&panel.node_list[pPole->start_i];
			CFacePanelNode *pEnd=&panel.node_list[pPole->end_i];
			if(pStart->m_bInternodeRepeat)
			{
				expression.VarAt(1)->fValue=i;	//expression.varList[1];
				double scale=expression.SolveExpression(pStart->scale_expression);
				CFacePanelNode *pScaleStart=&panel.node_list[pStart->attach_node_index[0]];
				CFacePanelNode *pScaleEnd=&panel.node_list[pStart->attach_node_index[1]];
				pStart->pos=pScaleStart->pos+(pScaleEnd->pos-pScaleStart->pos)*scale;
			}
			f2dPoint start_pos=pStart->pos;
			if(pEnd->m_bInternodeRepeat)
			{
				expression.VarAt(1)->fValue=i;	//expression.varList[1].fValue=i;
				double scale=expression.SolveExpression(pEnd->scale_expression);
				CFacePanelNode *pScaleStart=&panel.node_list[pEnd->attach_node_index[0]];
				CFacePanelNode *pScaleEnd=&panel.node_list[pEnd->attach_node_index[1]];
				pEnd->pos=pScaleStart->pos+(pScaleEnd->pos-pScaleStart->pos)*scale;
			}
			f2dPoint end_pos=pEnd->pos;
			coord_trans(start_pos,ucs,FALSE);
			coord_trans(end_pos,ucs,FALSE);
			start_pos.x*=scale;
			start_pos.y*=scale;
			end_pos.x*=scale;
			end_pos.y*=scale;
			start_pos.y+=1;	//��1��Ϊ���ڼ�ͼ��ѡ��ʱ��Ӱ��ʾʱ��ȫ�������ε�
			end_pos.y+=1;	//��1��Ϊ���ڼ�ͼ��ѡ��ʱ��Ӱ��ʾʱ��ȫ�������ε�
			bitmap.GetDC()->MoveTo(ftoi(start_pos.x),ftoi(start_pos.y));
			bitmap.GetDC()->LineTo(ftoi(end_pos.x),ftoi(end_pos.y));
		}
	}
	BITMAP bm;
	bitmap.GetBitmap(&bm);
	bitmap.GetBitmapBits(dwCount,data);
	bmap.SetBitmapBits(dwCount,data);
	int face_i=imageList.Add(&bmap,RGB(0,0,0));
	bitmap.GetDC()->SelectObject(pOldPen);	//ѡ��ԭ����
	m_pListCtrl->InsertItem(m_pListCtrl->GetItemCount(),panel.sLabel,face_i);
	delete []data;
}
void CSelFacePanelDlg::UpdateFaceSet()
{
	IStorage *pRootStg;
	DWORD grfMode = STGM_READ | STGM_SHARE_EXCLUSIVE;
	char file_path[MAX_PATH];
	GetSysPath(file_path);
	strcat(file_path,"facepanel.pns");
	HRESULT hr = ::StgOpenStorage(_bstr_t(file_path), NULL,grfMode,NULL, 0, &pRootStg);
	if(pRootStg==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("standard plane file can not open by default!");
#else
		AfxMessageBox("�򲻿�Ĭ�ϱ�׼���ļ���");
#endif
		return;
	}
	IStream *pStream;
	IStorage *pStg;
	CString Part;
	imageList.DeleteImageList();
	m_uIconWidth=m_dwIconWidth;
	m_uIconHeight=m_dwIconHeight;
	m_cThirdLayer='Q';
	if(m_iTemplateGroup==0)		//0��ʾ��ͷģ��
#ifdef AFX_TARG_ENU_ENGLISH
		Part="standard tower head segment";
	else if(m_iTemplateGroup==1)	//1stands for tower body
		Part="standard plane for tower body";
	else if(m_iTemplateGroup==2)	//2:stands for Horizontal surface insulation;
		Part="Horizontal insulation standard plane";
	else if(m_iTemplateGroup==3)	//3:multi-nodes surface
		Part="multi-nodes surface";
	else if(m_iTemplateGroup==4)	//3:multi-nodes surface
		Part="V standard plane";
	else if(m_iTemplateGroup==5)	//3:multi-nodes surface
		Part="V standard plane";
#else
		Part="��׼��ͷ��";
	else if(m_iTemplateGroup==1)	//1��ʾ����
		Part="�����׼��";
	else if(m_iTemplateGroup==2)	//2:��ʾ�����;
	{
		Part="�����׼��";
		m_cThirdLayer='G';
	}
	else if(m_iTemplateGroup==3)	//3:��ڼ���
		Part="��ڼ���";
	else if(m_iTemplateGroup==4)	//3:��ڼ���
		Part="K�ͱ�׼��";
	else if(m_iTemplateGroup==5)	//3:��ڼ���
	{
		Part="V�ͱ�׼��";
		m_cThirdLayer='G';
	}
	else if(m_iTemplateGroup==6)	//6.����������
		Part="����������";
#endif
	CString sThirdLayer=m_cThirdLayer;
	m_propList.SetItemPropValue(2,sThirdLayer);
	pRootStg->OpenStorage(_bstr_t(Part),NULL,grfMode,NULL,0,&pStg);
	if(pStg==NULL)
		pRootStg->CreateStorage(_bstr_t(Part),grfMode|STGM_CREATE,0,0,&pStg);
	imageList.Create(m_uIconWidth, m_uIconHeight, ILC_COLOR32,2,4);

	m_pListCtrl->SetImageList(&imageList,LVSIL_NORMAL);
	m_pListCtrl->DeleteAllItems();	//�����ʾ�б�
	IEnumSTATSTG *pFaceEnum;
	STATSTG face_statstg;
	pStg->EnumElements(0,NULL,0,&pFaceEnum);
	while(pFaceEnum->Next(1, &face_statstg, NULL) == S_OK)
	{
		if(face_statstg.type==STGTY_STREAM)
		{
			CString face_title=(char*)_bstr_t(face_statstg.pwcsName);
			HRESULT hr=pStg->OpenStream(face_statstg.pwcsName,NULL,grfMode,0,&pStream);
			if(m_iTemplateGroup==0)
			{
				CParaTemplate headTemplate;
				headTemplate.FromStream(pStream);
				sprintf(headTemplate.sLabel,"%s",face_title);
				AddHeadTemplate(headTemplate);
			}
			else
			{
				CFacePanel panel;
				sprintf(panel.sLabel,"%s",face_title);
				panel.FromStream(pStream);
				AddFacePanel(panel);
			}
			pStream->Release();
		}
	}
	pFaceEnum->Release();
	pStg->Release();
	pRootStg->Release();
	m_pListCtrl->Arrange(LVA_ALIGNLEFT);
}
BOOL CSelFacePanelDlg::ReadCursorHeadTemplate(CParaTemplate& headTemplate,BOOL bPromptError)
{
	if(m_pListCtrl->GetSelectedCount()<=0)
	{
		if(bPromptError)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("No standard plane selected��");
#else
			AfxMessageBox("û��ѡ����׼�棡");
#endif
		return FALSE;
	}
	POSITION pos=m_pListCtrl->GetFirstSelectedItemPosition();
	int iSel=m_iSelItemIndex=m_pListCtrl->GetNextSelectedItem(pos);
	m_pListCtrl->GetItemText(iSel,0,headTemplate.sLabel,24);
	//..
	IStorage *pRootStg;
	DWORD grfMode = STGM_READ | STGM_SHARE_EXCLUSIVE;
	char file_path[MAX_PATH];
	GetSysPath(file_path);
	strcat(file_path,"facepanel.pns");
	HRESULT hr = ::StgOpenStorage(_bstr_t(file_path), NULL,grfMode,NULL, 0, &pRootStg);
	if(pRootStg==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("standard plane file can not open by default��");
#else
		AfxMessageBox("�򲻿�Ĭ�ϱ�׼���ļ���");
#endif
		return FALSE;
	}
	IStream *pStream;
	IStorage *pStg;
	CString Part;
#ifdef AFX_TARG_ENU_ENGLISH
	Part="standard tower head segment";
#else
	Part="��׼��ͷ��";
#endif
	pRootStg->OpenStorage(_bstr_t(Part),NULL,grfMode,NULL,0,&pStg);
	if(pStg==NULL)
		pRootStg->CreateStorage(_bstr_t(Part),grfMode|STGM_CREATE,0,0,&pStg);

	pStg->OpenStream(_bstr_t(headTemplate.sLabel),NULL,grfMode,0,&pStream);
	if(pStream)
	{
		headTemplate.FromStream(pStream);
		pStream->Release();
	}
	pStg->Release();
	pRootStg->Release();
	//..
	m_nKeyNodeNum=headTemplate.KeyNodeNum();//.m_nKeyNode;
	if(m_nKeyNodeNum==1)
		m_iKeyNodeMir=3;
	ChangeKeyNodeNum();
	return TRUE;
}
BOOL CSelFacePanelDlg::ReadCursorFacePanel(CFacePanel& panel,BOOL bPromptError)
{
	if(m_pListCtrl->GetSelectedCount()<=0)
	{
		if(bPromptError)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("No standard plane selected��");
#else
			AfxMessageBox("û��ѡ����׼�棡");
#endif
		return FALSE;
	}
	POSITION pos=m_pListCtrl->GetFirstSelectedItemPosition();
	int iSel=m_iSelItemIndex=m_pListCtrl->GetNextSelectedItem(pos);
	m_pListCtrl->GetItemText(iSel,0,panel.sLabel,24);
	//..
	IStorage *pRootStg;
	DWORD grfMode = STGM_READ | STGM_SHARE_EXCLUSIVE;
	char file_path[MAX_PATH];
	GetSysPath(file_path);
	strcat(file_path,"facepanel.pns");
	HRESULT hr = ::StgOpenStorage(_bstr_t(file_path), NULL,grfMode,NULL, 0, &pRootStg);
	if(pRootStg==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("standard plane file can not open by default��");
#else
		AfxMessageBox("�򲻿�Ĭ�ϱ�׼���ļ���");
#endif
		return FALSE;
	}
	IStream *pStream;
	IStorage *pStg;
	CString Part;
	if(m_iTemplateGroup==0)		//0��ʾ��ͷģ��
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Part="standard tower head segment";
#else
		Part="��׼��ͷ��";
#endif
		pRootStg->OpenStorage(_bstr_t(Part),NULL,grfMode,NULL,0,&pStg);
		if(pStg==NULL)
			pRootStg->CreateStorage(_bstr_t(Part),grfMode|STGM_CREATE,0,0,&pStg);
	}
	if(m_iTemplateGroup==1)		//1��ʾ����
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Part="tower body standard plane";
#else
		Part="�����׼��";
#endif
		pRootStg->OpenStorage(_bstr_t(Part),NULL,grfMode,NULL,0,&pStg);
		if(pStg==NULL)
			pRootStg->CreateStorage(_bstr_t(Part),grfMode|STGM_CREATE,0,0,&pStg);
	}
	else if(m_iTemplateGroup==2)	//2:��ʾ�����;
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Part="Horizontal insulation standard plane";
#else
		Part="�����׼��";
#endif
		pRootStg->OpenStorage(_bstr_t(Part),NULL,grfMode,NULL,0,&pStg);
		if(pStg==NULL)
			pRootStg->CreateStorage(_bstr_t(Part),grfMode|STGM_CREATE,0,0,&pStg);
	}
	else if(m_iTemplateGroup==3)	//3:�Ȳ�
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Part="multi-nodes surface";
#else
		Part="��ڼ���";
#endif
		pRootStg->OpenStorage(_bstr_t(Part),NULL,grfMode,NULL,0,&pStg);
		if(pStg==NULL)
			pRootStg->CreateStorage(_bstr_t(Part),grfMode|STGM_CREATE,0,0,&pStg);
	}
	else if(m_iTemplateGroup==4)	//4:K�ͱ�׼��
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Part="K standard plane";
#else
		Part="K�ͱ�׼��";
#endif
		pRootStg->OpenStorage(_bstr_t(Part),NULL,grfMode,NULL,0,&pStg);
		if(pStg==NULL)
			pRootStg->CreateStorage(_bstr_t(Part),grfMode|STGM_CREATE,0,0,&pStg);
	}
	else if(m_iTemplateGroup==5)	//5:V�ͱ�׼��
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Part="V standard plane";
#else
		Part="V�ͱ�׼��";
#endif
		pRootStg->OpenStorage(_bstr_t(Part),NULL,grfMode,NULL,0,&pStg);
		if(pStg==NULL)
			pRootStg->CreateStorage(_bstr_t(Part),grfMode|STGM_CREATE,0,0,&pStg);
	}
	else if(m_iTemplateGroup==6)	//����������
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Part="Leg front plane";
#else
		Part="����������";
#endif
		pRootStg->OpenStorage(_bstr_t(Part),NULL,grfMode,NULL,0,&pStg);
		if(pStg==NULL)
			pRootStg->CreateStorage(_bstr_t(Part),grfMode|STGM_CREATE,0,0,&pStg);
	}

	pStg->OpenStream(_bstr_t(panel.sLabel),NULL,grfMode,0,&pStream);
	if(pStream)
	{
		panel.FromStream(pStream);
		pStream->Release();
	}
	pStg->Release();
	pRootStg->Release();
	//..
	m_nKeyNodeNum=panel.m_nKeyNode;
	if(m_nKeyNodeNum==1)
		m_iKeyNodeMir=3;
	UpdateData(FALSE);
	ChangeKeyNodeNum();
	return TRUE;
}

void CSelFacePanelDlg::OnOK() 
{
	int nBackCount=m_nKeyNodeNum;
	int nCount=m_nKeyNodeNum;
	/*if(m_iTemplateGroup==0&&!ReadCursorHeadTemplate(m_xHeadTemplate,TRUE))
		return;
	else if(m_iTemplateGroup!=0&&!ReadCursorFacePanel(m_xFacePanel,TRUE))
		return;*/
	if(m_iKeyNodeMir==1||m_iKeyNodeMir==2)
		nCount*=2;
	else if(m_iKeyNodeMir==3)
		nCount*=4;
	if(nCount<3)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("oly the number of key points is at least 3 can standard plan' layout location be determined. the number of key points is set unreasonably, please reset��");
#else
		AfxMessageBox("�ؼ�����������Ϊ3����ȷ����׼��İڷ�λ�ã��ؼ���������ԳƷ�ʽ���ò��������������ã�");
#endif
		return;
	}
	if(m_iTemplateGroup>0)
		m_xFacePanel.m_nInternodeNum=m_nInternodeNum;
	CDialog::OnOK();
	m_nKeyNodeNum=nBackCount;
}

void CSelFacePanelDlg::ChangeKeyNodeNum() 
{
#ifdef AFX_TARG_ENU_ENGLISH
	m_sKeyNodeOrder="order selection for key points:\n";
#else
	m_sKeyNodeOrder="�ؼ���ѡ��˳��:\n";
#endif
	CString ss;
	if(m_iTemplateGroup==0)
	{
		UINT j;
		CParaNode* pParaNode;
		for(pParaNode=m_xHeadTemplate.listParaNodes.GetFirst(),j=0;j<(UINT)m_nKeyNodeNum;
			pParaNode=m_xHeadTemplate.listParaNodes.GetNext(),j++)
		{
			if(j>=m_xHeadTemplate.listParaNodes.GetNodeNum())
				return;
			ss=pParaNode->keyLabel.Label();
			m_sKeyNodeOrder+="   "+ss+";\n";
		}
	}
	else
	{
		for(int i=0;i<m_nKeyNodeNum;i++)
		{
			if(i>=m_xFacePanel.node_list.GetNodeNum())
				return;
			ss=m_xFacePanel.node_list[i].sTitle;
			m_sKeyNodeOrder+="   "+ss+";\n";
		}
	}
	GetDlgItem(IDC_S_KEY_NODE_ORDER)->SetWindowText(m_sKeyNodeOrder);
}
void CSelFacePanelDlg::UpdateFaceDetailGraph()
{
	try{
		SCOPE_STRU scope;
		CRect rect;
		if(m_iTemplateGroup==0)
		{	//��׼��ͷģ���
			if(m_xHeadTemplate.image.bmBits!=NULL)
			{
				//������ʵĴ��ڴ�С
				int width=max(m_xHeadTemplate.image.bmWidth,m_rectInitDrawRgn.Width());
				int height=max(m_xHeadTemplate.image.bmHeight,m_rectInitDrawRgn.Height());
				CWnd *pWnd = GetDlgItem(IDC_S_MAP);
				pWnd->GetWindowRect(&rect);
				ScreenToClient(&rect);
				pWnd->MoveWindow(rect.left,rect.top,width,height);
				GetWindowRect(&rect);
				MoveWindow(rect.left,rect.top,m_rectInitWndRgn.Width()+width-m_rectInitDrawRgn.Width(),
					m_rectInitWndRgn.Height()+height-m_rectInitDrawRgn.Height());
			}
			else
			{
				//��ʼ������ϵ
				UCS_STRU object_ucs;//����ʾ����ϵ������ͼԪ��������ϵ
				object_ucs.axis_x.Set(1,0,0);
				object_ucs.axis_y.Set(0,0,1);
				object_ucs.axis_z.Set(0,-1,0);
				UCS_STRU rot_ucs=m_xHeadTemplate.rot_ucs;	//ͷ����׼ģ����ڻ�ͼ������ǰ��ת�����任����ϵ

				EXPRESSION_VAR *pVar;
				CExpression expression;
				expression.varList.Empty();
				DESIGN_PARAM_ITEM *pPara;
				for(pPara=m_xHeadTemplate.listParams.GetFirst();pPara;pPara=m_xHeadTemplate.listParams.GetNext())
					expression.varList.Append(ConvertParamItemToExprVar(pPara));
				/*for(pPara=m_xHeadTemplate.para_list.GetFirst();pPara;pPara=m_xHeadTemplate.para_list.GetNext())
				{
					if(strlen(pPara->startposx_expression)>0)
						pPara->m_dimStart.x=expression.SolveExpression(pPara->startposx_expression);
					if(strlen(pPara->startposy_expression)>0)
						pPara->m_dimStart.y=expression.SolveExpression(pPara->startposy_expression);
					if(strlen(pPara->startposz_expression)>0)
						pPara->m_dimStart.z=expression.SolveExpression(pPara->startposz_expression);
					if(strlen(pPara->endposx_expression)>0)
						pPara->m_dimEnd.x=expression.SolveExpression(pPara->endposx_expression);
					if(strlen(pPara->endposy_expression)>0)
						pPara->m_dimEnd.y=expression.SolveExpression(pPara->endposy_expression);
					if(strlen(pPara->endposz_expression)>0)
						pPara->m_dimEnd.z=expression.SolveExpression(pPara->endposz_expression);
					f3dPoint pos=pPara->m_dimStart;
					coord_trans(pos,object_ucs,TRUE);
					coord_trans(pos,rot_ucs,FALSE);
					scope.VerifyVertex(pos);
					pos=pPara->m_dimEnd;
					coord_trans(pos,object_ucs,TRUE);
					coord_trans(pos,rot_ucs,FALSE);
					scope.VerifyVertex(pos);
				}*/
				for(CParaNode* pNode=m_xHeadTemplate.listParaNodes.GetFirst();pNode;pNode=m_xHeadTemplate.listParaNodes.GetNext())
				{
					if(IsHasAlphabet(pNode->posx_expression))
						pNode->pos.x=expression.SolveExpression(pNode->posx_expression);
					pVar=expression.varList.Append();
					pVar->fValue=pNode->pos.x;
					sprintf(pVar->variableStr,"%s_X",pNode->keyLabel.Label());
					if(IsHasAlphabet(pNode->posy_expression))
						pNode->pos.y=expression.SolveExpression(pNode->posy_expression);
					pVar=expression.varList.Append();
					pVar->fValue=pNode->pos.y;
					sprintf(pVar->variableStr,"%s_Y",pNode->keyLabel.Label());
					if(IsHasAlphabet(pNode->posz_expression))
						pNode->pos.z=expression.SolveExpression(pNode->posz_expression);
					pVar=expression.varList.Append();
					pVar->fValue=pNode->pos.z;
					sprintf(pVar->variableStr,"%s_Z",pNode->keyLabel.Label());
					f3dPoint pos=pNode->pos;
					coord_trans(pos,object_ucs,TRUE);
					coord_trans(pos,rot_ucs,FALSE);
					scope.VerifyVertex(pos);
				}
				//��ԭ��ʼ���ڴ�С
				GetWindowRect(&rect);
				if(rect.Width()!=m_rectInitWndRgn.Width()||rect.Height()!=m_rectInitWndRgn.Height())
					MoveWindow(rect.left,rect.top,m_rectInitWndRgn.Width(),m_rectInitWndRgn.Height());
				CWnd *pWnd = GetDlgItem(IDC_S_MAP);
				pWnd->GetWindowRect(&rect);
				ScreenToClient(&rect);
				if(rect.Width()!=m_rectInitDrawRgn.Width()||rect.Height()!=m_rectInitDrawRgn.Height())
					pWnd->MoveWindow(rect.left,rect.top,m_rectInitDrawRgn.Width(),m_rectInitDrawRgn.Height());
				//����ͼ������ϵ����ƽ����
				double scale_x=(m_rectInitDrawRgn.Width()-60)/scope.wide();
				double scale_y=(m_rectInitDrawRgn.Height()-60)/scope.depth();
				m_fScaleScreenToUser=min(scale_x,scale_y);
				scope.centre(&rgn_ucs.origin.x,&rgn_ucs.origin.y,&rgn_ucs.origin.z);
				rgn_ucs.origin*=m_fScaleScreenToUser;
				rgn_ucs.origin.x+=-m_rectInitDrawRgn.Width()/2+20-rect.left;	//�����Ҳ��б�ע�ַ�������
				rgn_ucs.origin.y+=m_rectInitDrawRgn.Height()/2+rect.top;
			}
		}
		else//if(m_iTemplateGroup==1)
		{	//��׼�����
			for(CFacePanelNode *pNode=m_xFacePanel.node_list.GetFirst();pNode;
								pNode=m_xFacePanel.node_list.GetNext())
			{
				//if(m_xFacePanel.m_iFaceType!=2&&pNode->m_bInternodeRepeat)
				if(!m_xFacePanel.IsRepeateType()&&pNode->m_bInternodeRepeat)
					continue;
				scope.VerifyVertex(f3dPoint(pNode->pos.x,pNode->pos.y));
			}
			//��ԭ��ʼ���ڴ�С
			GetWindowRect(&rect);
			if(rect.Width()!=m_rectInitWndRgn.Width()||rect.Height()!=m_rectInitWndRgn.Height())
				MoveWindow(rect.left,rect.top,m_rectInitWndRgn.Width(),m_rectInitWndRgn.Height());
			CWnd *pWnd = GetDlgItem(IDC_S_MAP);
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);
			if(rect.Width()!=m_rectInitDrawRgn.Width()||rect.Height()!=m_rectInitDrawRgn.Height())
				pWnd->MoveWindow(rect.left,rect.top,m_rectInitDrawRgn.Width(),m_rectInitDrawRgn.Height());
			double scale_x=(m_rectInitDrawRgn.Width()-60)/scope.wide();
			double scale_y=(m_rectInitDrawRgn.Height()-60)/scope.high();
			m_fScaleScreenToUser=min(scale_x,scale_y);
			//ȷ����ͼ����ԶԻ���ͻ������������ϵ
			scope.centre(&rgn_ucs.origin.x,&rgn_ucs.origin.y,&rgn_ucs.origin.z);
			rgn_ucs.origin*=m_fScaleScreenToUser;
			rgn_ucs.origin.x+=-m_rectInitDrawRgn.Width()/2+20-rect.left;	//�����Ҳ��б�ע�ַ�������
			rgn_ucs.origin.y+=m_rectInitDrawRgn.Height()/2+rect.top;
		}
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
}

void CSelFacePanelDlg::OnItemchangedListCtrlStdFace(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(m_pListCtrl->GetSelectedCount()>0)
	{
		if(m_iTemplateGroup==0)
		{
			ReadCursorHeadTemplate(m_xHeadTemplate,TRUE);
			int nKeyNodeNum=m_xHeadTemplate.KeyNodeNum();
			if(nKeyNodeNum==1)
				m_iKeyNodeMir=3;	//XYZ�Գ�
			else if(nKeyNodeNum==2)
			{
				CParaNode* p1stNode=m_xHeadTemplate.listParaNodes.GetFirst();
				CParaNode* p2ndNode=m_xHeadTemplate.listParaNodes.GetNext();
				if (p1stNode&&p2ndNode)
				{	//����ʶ�����ؼ���ʱ�Ĺؼ���Գ����ͣ����޴˴����û����׺����趨��ֵ����Ī������ wjh-2019.11.6
					if(fabs(p1stNode->pos.x+p2ndNode->pos.x)<EPS)
						m_iKeyNodeMir=2;
					else
						m_iKeyNodeMir=1;
				}
				else if(m_iKeyNodeMir==0||m_iKeyNodeMir==3)
					m_iKeyNodeMir=1;	//X��Գ�
			}
			else
				m_iKeyNodeMir=0;
		}
		else
		{
			ReadCursorFacePanel(m_xFacePanel,TRUE);
			m_xFacePanel.m_iFaceType=m_iTemplateGroup;
			if(m_xFacePanel.m_nKeyNode==1)
				m_iKeyNodeMir=3;	//XYZ�Գ�
			else if(m_xFacePanel.m_nKeyNode==2)
			{
				if(m_iKeyNodeMir==0||m_iKeyNodeMir==3)
					m_iKeyNodeMir=2;	//Y��Գ�
			}
			else
				m_iKeyNodeMir=0;
		}
		DisplayPropertyList();
		UpdateFaceDetailGraph();
		Invalidate();
	}

	*pResult = 0;
}

void CSelFacePanelDlg::OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_iTemplateGroup = m_ctrlPropGroup.GetCurSel();
	m_propList.m_iPropGroup=m_iTemplateGroup;
	CPropTreeItem* pItem=m_propList.FindItemByPropId(GetPropID("templateMirStyle"),NULL);
	if (pItem)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pItem->m_lpNodeInfo->m_cmbItems = "No Mirror|X Axis|Y Axis|Z Axis|Mirror|Z Axis Mirror";
#else
		pItem->m_lpNodeInfo->m_cmbItems = "�޶Գ�|X��|Y��|Z��|����|Z��_����";
#endif
		if (this->m_iTemplateGroup == 6)
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_cmbItems += "|Corner Mirror";
#else
			pItem->m_lpNodeInfo->m_cmbItems += "|�ǶԳ�";
#endif
	}
	m_propList.Redraw();
	CPaintDC dc(this);
	m_uIconWidth=m_dwIconWidth;
	m_uIconHeight=m_dwIconHeight;
	bitmap.ReInitDC(dc.GetSafeHdc(),m_uIconWidth,m_uIconHeight);
	UpdateFaceSet();
	*pResult = 0;
}

void CSelFacePanelDlg::RefreshTabCtrl(int iCurSel)
{
	m_ctrlPropGroup.DeleteAllItems();
#ifdef AFX_TARG_ENU_ENGLISH
	m_ctrlPropGroup.InsertItem(0,"head");
	m_ctrlPropGroup.InsertItem(1,"body");
	m_ctrlPropGroup.InsertItem(2,"isolated plane");
	m_ctrlPropGroup.InsertItem(3,"nodes plane");
	m_ctrlPropGroup.InsertItem(4,"K plane");
	m_ctrlPropGroup.InsertItem(5,"V plane");
	m_ctrlPropGroup.InsertItem(6,"Leg front plane");
#else
	m_ctrlPropGroup.InsertItem(0,"ͷ��");
	m_ctrlPropGroup.InsertItem(1,"����");
	m_ctrlPropGroup.InsertItem(2,"������");
	m_ctrlPropGroup.InsertItem(3,"�ڼ���");
	m_ctrlPropGroup.InsertItem(4,"K����");
	m_ctrlPropGroup.InsertItem(5,"V����");
	m_ctrlPropGroup.InsertItem(6,"����������");
#endif
	m_ctrlPropGroup.SetCurSel(m_iTemplateGroup);
}

PROPLIST_ITEM* CSelFacePanelDlg::AddPropItem(const char* propKey,PROPLIST_ITEM item)//������������º���
{
	PROPLIST_ITEM* pItem=propHashtable.SetValue(propKey,item);
	pItem->propKeyStr.Copy(propKey);
	return pItem;
}
void CSelFacePanelDlg::InitPropHashtable()
{
	int id=1;
	propHashtable.SetHashTableGrowSize(10);
#ifdef AFX_TARG_ENU_ENGLISH
	AddPropItem("keynode_n",PROPLIST_ITEM(3,"key points quantity"));
	AddPropItem("keynodeMirStyle",PROPLIST_ITEM(4,"key points symmetry"));
	AddPropItem("templateMirStyle",PROPLIST_ITEM(5,"template plane symmetry"));
	AddPropItem("acs.axis_x",PROPLIST_ITEM(6,"ACS-axis X"));
	AddPropItem("acs.axis_x.x",PROPLIST_ITEM(7,"Coord.X"));
	AddPropItem("acs.axis_x.y",PROPLIST_ITEM(8,"Coord.Y"));
#else
	AddPropItem("keynode_n",PROPLIST_ITEM(3,"�ؼ�������"));
	AddPropItem("keynodeMirStyle",PROPLIST_ITEM(4,"�ؼ���Գ�"));
	AddPropItem("templateMirStyle",PROPLIST_ITEM(5,"ģ����Գ�"));
	AddPropItem("acs.axis_x",PROPLIST_ITEM(6,"ģ��X������","ģ������ϵX����ģ�������µķ�������"));
	AddPropItem("acs.axis_x.x",PROPLIST_ITEM(7,"X����"));
	AddPropItem("acs.axis_x.y",PROPLIST_ITEM(8,"Y����"));
#endif
}
long CSelFacePanelDlg::GetPropID(char* propStr)	//�õ�����ID
{
	PROPLIST_ITEM* pItem=propHashtable.GetValue(propStr);
	if(pItem)
		return pItem->id;
	else
		return 0;
}
void CSelFacePanelDlg::DisplayPropertyList()
{
	m_propList.CleanTree();
	//�����������ص�����
	m_propList.SetModifyValueFunc(ModifyItemValue);
	char sText[100]="";
	CPropTreeItem *pItem,*pGroupItem,*pRoot=m_propList.GetRootItem();
	CItemInfo* lpInfo;
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "template parameter";
	lpInfo->m_strPropHelp = "template parameter";
#else
	lpInfo->m_strPropName = "ģ�����";
	lpInfo->m_strPropHelp = "ģ�����";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp=0;
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=0XFF;
	if(m_iTemplateGroup==0)
	{
		m_propList.m_pObj=&m_xHeadTemplate;
		for(DESIGN_PARAM_ITEM *pPara=m_xHeadTemplate.listParams.GetFirst();pPara;pPara=m_xHeadTemplate.listParams.GetNext())
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = (char*)pPara->sLabel;
			lpInfo->m_strPropHelp = (char*)pPara->sNotes;
			pItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			memcpy(&pItem->m_idProp,&pPara,4);
			lpInfo->m_strPropValue.Format("%lf",pPara->value.fValue);
			SimplifiedNumString(lpInfo->m_strPropValue);
			pItem->m_dwPropGroup=1;
		}
	}
	else //if(m_iTemplateGroup==1)	//�����׼��
	{
		m_propList.m_pObj=&m_xFacePanel;
		if(m_xFacePanel.m_iFaceType==1||m_xFacePanel.IsRepeateType())//m_xFacePanel.m_iFaceType==3)
		{	//�����׼�������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "N";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "number of nodes";
#else
			lpInfo->m_strPropHelp = "�ڼ���";
#endif
			pItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pItem->m_idProp=1;
			lpInfo->m_strPropValue.Format("%d",m_nInternodeNum);
			pItem->m_dwPropGroup=0XFF;
		}
		//ͼ����������λ��Ϣ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "layer location characters";
		lpInfo->m_strPropHelp = "part's location:front,back,left,right,position 1��position 2��position 3��position 4��optional position,etc.";
#else
		lpInfo->m_strPropName = "ͼ�㷽λ�ַ�";
		lpInfo->m_strPropHelp = "�������ڷ�λ��ǰ�桢���桢���桢���桢1��λ��2��λ��3��λ��4��λ������λ�ȡ�";
#endif
		lpInfo->m_cmbItems="Q|H|Z|Y|1|2|3|4|G";
		pItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pItem->m_idProp=2;
		lpInfo->m_strPropValue.Format("%C",m_cThirdLayer);
		pItem->m_dwPropGroup=0XFF;
	}
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Assembly Info";
	lpInfo->m_strPropHelp = "assembly information";
#else
	lpInfo->m_strPropName = "װ����Ϣ";
	lpInfo->m_strPropHelp = "װ����Ϣ";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp=0;
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=0XFF;

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbItems="1.X+|2.X-|3.Y+|4.Y-";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "ACS-axis X";
	lpInfo->m_strPropHelp = "The assemble direction of template axis x.";
#else
	lpInfo->m_strPropName = "ģ��X������";
	lpInfo->m_strPropHelp = "ͷ��ģ��ֲ�����ϵX��������ģ������ϵ�е�װ�䷽��";
#endif
	pItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pItem->m_idProp=GetPropID("acs.axis_x");	//6
	if(GetPropValueStr(pItem->m_idProp,sText)>0)
		lpInfo->m_strPropValue=sText;
	pItem->m_dwPropGroup=1;
	int iVecType=CStdVector::GetVectorType(m_vAcsAxisX);
	if(iVecType==0||iVecType>4)
	{
		CPropTreeItem* pLeafItem;
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
	#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Coord.X";
	#else
		lpInfo->m_strPropName = "X����";
	#endif
		pLeafItem = m_propList.InsertItem(pItem,lpInfo, -1);
		pLeafItem->m_idProp=GetPropID("acs.axis_x.x");	//7
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue=sText;
		pLeafItem->m_dwPropGroup=1;
		pLeafItem->SetReadOnly(iVecType>0&&iVecType<=4);

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
	#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Coord.Y";
	#else
		lpInfo->m_strPropName = "Y����";
	#endif
		pLeafItem = m_propList.InsertItem(pItem,lpInfo, -1);
		pLeafItem->m_idProp=GetPropID("acs.axis_x.y");	//8
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue=sText;
		pLeafItem->m_dwPropGroup=1;
		pLeafItem->SetReadOnly(iVecType>0&&iVecType<=4);
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "key points quantity";
	lpInfo->m_strPropHelp = "key points quantity";
#else
	lpInfo->m_strPropName = "�ؼ�������";
	lpInfo->m_strPropHelp = "�ؼ�������";
#endif
	pItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pItem->m_idProp=GetPropID("keynode_n");	//3;
	if(GetPropValueStr(pItem->m_idProp,sText)>0)
		lpInfo->m_strPropValue=sText;
	pItem->SetReadOnly(m_iTemplateGroup==3||m_iTemplateGroup==5);	//��ڼ�����V�ͱ�׼��Ĺؼ���������������
	pItem->m_dwPropGroup=0XFF;
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems= "No Mirror|X Axis|Y Axis|XYZ Axis";	//|Zaxis
	lpInfo->m_strPropName = "key points symmetry";
	lpInfo->m_strPropHelp = "key points symmetry";
#else
	lpInfo->m_cmbItems= "�޶Գ�|X��|Y��|XYZ��";	//|Z��
	lpInfo->m_strPropName = "�ؼ���Գ�";
	lpInfo->m_strPropHelp = "�ؼ���Գ�";
#endif
	pItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pItem->m_idProp=GetPropID("keynodeMirStyle");//4;
	if(GetPropValueStr(pItem->m_idProp,sText)>0)
		lpInfo->m_strPropValue=sText;
	pItem->m_dwPropGroup=0X17;
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems= "No Mirror|X Axis|Y Axis|Z Axis|Mirror|Z Axis Mirror";
#else
	lpInfo->m_cmbItems= "�޶Գ�|X��|Y��|Z��|����|Z��_����";
#endif
	if(this->m_iTemplateGroup==6)
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems+="|Corner Mirror";
#else
		lpInfo->m_cmbItems+="|�ǶԳ�";
#endif
// 	if(m_iTemplateGroup==0)
// 		lpInfo->m_cmbItems= "�޶Գ�|X��|Y��";
// 	else
// 		lpInfo->m_cmbItems= "�޶Գ�|Z��|����|Z��_����";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "template plane symmetry";
	lpInfo->m_strPropHelp = "template plane symmetry";
#else
	lpInfo->m_strPropName = "ģ����Գ�";
	lpInfo->m_strPropHelp = "ģ����Գ�";
#endif
	pItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pItem->m_idProp=GetPropID("templateMirStyle");//5;
	if(GetPropValueStr(pItem->m_idProp,sText)>0)
		lpInfo->m_strPropValue=sText;
	pItem->m_dwPropGroup=0XFE;

	m_propList.Redraw();
}

//ת������������ʾ�߽�ܶ�Ķ�ά��Ϊ��Ч��(����WINDOWS��ʾ������)
static void Valid2dLine(f2dLine &line,CRect rect)
{
	double angle;
	angle = Cal2dLineAng(line);
	if(line.startPt.x<rect.left)
	{
		line.startPt.y -= (line.startPt.x-rect.left)*tan(angle);
		line.startPt.x  = rect.left;
	}
	else if(line.startPt.x>rect.right)
	{
		line.startPt.y -= (line.startPt.x-rect.right)*tan(angle);
		line.startPt.x  = rect.right;
	}
	if(line.startPt.y<rect.top)
	{
		line.startPt.x-= (line.startPt.y-rect.top)*tan(Pi/2-angle);
		line.startPt.y = rect.top;
	}
	else if(line.startPt.y>rect.bottom)
	{
		line.startPt.x -= (line.startPt.y-rect.bottom)*tan(Pi/2-angle);
		line.startPt.y  = rect.bottom;
	}
	if(line.endPt.x<rect.left)
	{
		line.endPt.y -= (line.endPt.x-rect.left)*tan(angle);
		line.endPt.x  = rect.left;
	}
	else if(line.endPt.x>rect.right)
	{
		line.endPt.y -= (line.endPt.x-rect.right)*tan(angle);
		line.endPt.x  = rect.right;
	}
	if(line.endPt.y<rect.top)
	{
		line.endPt.x-= (line.endPt.y-rect.top)*tan(Pi/2-angle);
		line.endPt.y = rect.top;
	}
	else if(line.endPt.y>rect.bottom)
	{
		line.endPt.x -= (line.endPt.y-rect.bottom)*tan(Pi/2-angle);
		line.endPt.y = rect.bottom;
	}
}
static BOOL IsInvalidLine(f2dLine line_2d, CRect rect)
{
	if(line_2d.startPt.x<rect.left&&line_2d.endPt.x<rect.left)
		return TRUE;
	else if(line_2d.startPt.y<rect.top&&line_2d.endPt.y<rect.top)
		return TRUE;
	else if(line_2d.startPt.x>rect.right&&line_2d.endPt.x>rect.right)
		return TRUE;
	else if(line_2d.startPt.y>rect.bottom&&line_2d.endPt.y>rect.bottom)
		return TRUE;
	return FALSE;
}

void CSelFacePanelDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	//m_pDraw->Draw(&dc);
	CRect rectDrawRgn;		//��ǰ��ͼ���ĵ�ǰ��Ч��ͼ�����Σ��Ի���ͻ������꣩
	CWnd *pWnd = GetDlgItem(IDC_S_MAP);
	pWnd->GetWindowRect(&rectDrawRgn);
	ScreenToClient(&rectDrawRgn);
	if(m_iTemplateGroup==0)
	{	//��׼��ͷģ���
		//��ʼ���Ҳ���ͷʾ��ͼ��ͼ��
		if(m_xHeadTemplate.image.bmBits!=NULL)
		{
			BITMAPINFO bmpInfo;
			memset(&bmpInfo,0,sizeof(BITMAPINFO));
			bmpInfo.bmiHeader.biBitCount=m_xHeadTemplate.image.bmBitsPixel;
			bmpInfo.bmiHeader.biHeight = m_xHeadTemplate.image.bmHeight;
			bmpInfo.bmiHeader.biWidth = m_xHeadTemplate.image.bmWidth;
			bmpInfo.bmiHeader.biPlanes=1;
			bmpInfo.bmiHeader.biSize=40;
			bmpInfo.bmiHeader.biSizeImage=m_xHeadTemplate.image.bmWidthBytes*m_xHeadTemplate.image.bmHeight;
			::StretchDIBits(dc.GetSafeHdc(),rectDrawRgn.left,rectDrawRgn.top,m_xHeadTemplate.image.bmWidth,m_xHeadTemplate.image.bmHeight,
				0,0,m_xHeadTemplate.image.bmWidth,m_xHeadTemplate.image.bmHeight,m_xHeadTemplate.image.bmBits,
				&bmpInfo,DIB_RGB_COLORS,SRCCOPY);
		}
		else
		{
			UCS_STRU object_ucs;//����ʾ����ϵ������ͼԪ��������ϵ
			object_ucs.axis_x.Set(1,0,0);
			object_ucs.axis_y.Set(0,0,1);
			object_ucs.axis_z.Set(0,-1,0);
			UCS_STRU rot_ucs=m_xHeadTemplate.rot_ucs;	//ͷ����׼ģ����ڻ�ͼ������ǰ��ת�����任����ϵ
			dc.SelectStockObject(BLACK_BRUSH);
			dc.Rectangle(rectDrawRgn);
			CBrush redBrush,greenBrush;
			redBrush.CreateSolidBrush(RGB(255,0,0));
			greenBrush.CreateSolidBrush(RGB(0,255,0));
			dc.SetBkMode(TRANSPARENT);
			CPen *pOldPen=dc.SelectObject(&red_solid_pen);
			CBrush *pOldBrush=dc.SelectObject(&redBrush);
			f3dPoint pos;
			f3dLine line;
			f2dLine line2d;
			/*dc.SetTextColor(RGB(255,0,0));
			for(CHeadParameter *pPara=m_xHeadTemplate.para_list.GetFirst();pPara;pPara=m_xHeadTemplate.para_list.GetNext())
			{
				line.startPt=pPara->m_dimStart;
				line.endPt=pPara->m_dimEnd;
				//�ɽ�ͼԪ����ת������ʾ����ϵ��
				coord_trans(line.startPt,object_ucs,TRUE);
				coord_trans(line.endPt,object_ucs,TRUE);
				//��ͼԪ��ת��ָ���ķ�λ
				coord_trans(line.startPt,rot_ucs,FALSE);
				coord_trans(line.endPt,rot_ucs,FALSE);
				line.startPt *= m_fScaleScreenToUser;
				line.endPt   *= m_fScaleScreenToUser;
				//line.startPt+=wnd_draw_offset;
				//line.endPt+=wnd_draw_offset;
				coord_trans(line.startPt,rgn_ucs,FALSE);
				coord_trans(line.endPt,rgn_ucs,FALSE);
				line2d.startPt.Set(line.startPt.x,line.startPt.y);
				line2d.endPt.Set(line.endPt.x,line.endPt.y);
				if(IsInvalidLine(line2d,rectDrawRgn))
					continue;
				Valid2dLine(line2d,rectDrawRgn);
				dc.MoveTo((int)line2d.startPt.x,(int)line2d.startPt.y);
				dc.LineTo((int)line2d.endPt.x,(int)line2d.endPt.y);
				if(pPara->m_bStartDimPoint)
					dc.Ellipse(ftoi(line2d.startPt.x-3),ftoi(line2d.startPt.y-3),ftoi(line2d.startPt.x+3),ftoi(line2d.startPt.y+3));
				if(pPara->m_bEndDimPoint)
					dc.Ellipse(ftoi(line2d.endPt.x-3),ftoi(line2d.endPt.y-3),ftoi(line2d.endPt.x+3),ftoi(line2d.endPt.y+3));
				
				f3dPoint center=(line.startPt+line.endPt)/2;
				CPoint posTxt(ftoi(center.x),ftoi(center.y));
				dc.TextOut(posTxt.x,posTxt.y,pPara->var.variableStr,strlen(pPara->var.variableStr));
			}*/
			dc.SelectObject(&greenBrush);
			dc.SelectObject(&solid_pen);
			dc.SetTextColor(RGB(0,255,0));
			for(CParaRod *pPole=m_xHeadTemplate.listParaRods.GetFirst();pPole;pPole=m_xHeadTemplate.listParaRods.GetNext())
			{
				CParaNode *pStartNode=m_xHeadTemplate.listParaNodes.GetValue(pPole->keyStartNode);
				CParaNode *pEndNode=m_xHeadTemplate.listParaNodes.GetValue(pPole->keyEndNode);
				line.startPt=pStartNode->pos;
				line.endPt=pEndNode->pos;
				//�ɽ�ͼԪ����ת������ʾ����ϵ��
				coord_trans(line.startPt,object_ucs,TRUE);
				coord_trans(line.endPt,object_ucs,TRUE);
				//��ͼԪ��ת��ָ���ķ�λ
				coord_trans(line.startPt,rot_ucs,FALSE);
				coord_trans(line.endPt,rot_ucs,FALSE);
				line.startPt *= m_fScaleScreenToUser;
				line.endPt   *= m_fScaleScreenToUser;
				//line.startPt+=wnd_draw_offset;
				//line.endPt+=wnd_draw_offset;
				coord_trans(line.startPt,rgn_ucs,FALSE);
				coord_trans(line.endPt,rgn_ucs,FALSE);
				line2d.startPt.Set(line.startPt.x,line.startPt.y);
				line2d.endPt.Set(line.endPt.x,line.endPt.y);
				if(IsInvalidLine(line2d,rectDrawRgn))
					continue;
				Valid2dLine(line2d,rectDrawRgn);
				
				if(line.is_highlight)
				{
					pOldPen=dc.SelectObject(&green_dash_pen);
					dc.MoveTo((int)line2d.startPt.x,(int)line2d.startPt.y);
					dc.LineTo((int)line2d.endPt.x,(int)line2d.endPt.y);
					dc.SelectObject(pOldPen);
				}
				else
				{
					if(line.pen.style==PS_DASH)
						dc.SelectObject(&dash_pen);
					else// if(line.pen.style==PS_SOLID)
						dc.SelectObject(&solid_pen);
					dc.MoveTo((int)line2d.startPt.x,(int)line2d.startPt.y);
					dc.LineTo((int)line2d.endPt.x,(int)line2d.endPt.y);
				}
			}
			dc.SelectObject(&green_solid_pen);
			for(CParaNode *pNode=m_xHeadTemplate.listParaNodes.GetFirst();pNode;pNode=m_xHeadTemplate.listParaNodes.GetNext())
			{
				pos=pNode->pos;
				//�ɽ�ͼԪ����ת������ʾ����ϵ��
				coord_trans(pos,object_ucs,TRUE);
				//��ͼԪ��ת��ָ���ķ�λ
				coord_trans(pos,rot_ucs,FALSE);
				pos *= m_fScaleScreenToUser;
				//pos+=wnd_draw_offset;
				coord_trans(pos,rgn_ucs,FALSE);
				CPoint point((int)pos.x,(int)pos.y);
				if(!rectDrawRgn.PtInRect(point))
					continue;
				dc.Ellipse(point.x-3,point.y-3,point.x+3,point.y+3);
				dc.TextOut(point.x+3,point.y+3,pNode->keyLabel.Label(),strlen(pNode->keyLabel.Label()));
			}
			dc.SelectObject(pOldPen);
			dc.SelectObject(pOldBrush);
		}
	}
	else
	{	//��׼�����
		dc.SelectStockObject(BLACK_BRUSH); // TODO: Add your message handler code here
		dc.Rectangle(rectDrawRgn);
		dc.SelectStockObject(HOLLOW_BRUSH);
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(RGB(255,0,0));
		CPen pen;
		pen.CreatePen(PS_SOLID,1,RGB(0,255,0));
		CPen *pOldPen=dc.SelectObject(&pen);
		for(CFacePanelNode *pNode=m_xFacePanel.node_list.GetFirst();pNode;
		pNode=m_xFacePanel.node_list.GetNext())
		{
			if(m_xFacePanel.m_iFaceType!=2&&pNode->m_bInternodeRepeat)
				continue;
			f3dPoint pos(pNode->pos.x*m_fScaleScreenToUser,pNode->pos.y*m_fScaleScreenToUser);
			coord_trans(pos,rgn_ucs,FALSE);
			f3dPoint start(pos.x-6,pos.y);
			f3dPoint end(pos.x+6,pos.y);
			dc.MoveTo(ftoi(start.x),ftoi(start.y));
			dc.LineTo(ftoi(end.x),ftoi(end.y));
			start.Set(pos.x,pos.y+6);
			end.Set(pos.x,pos.y-6);
			dc.MoveTo(ftoi(start.x),ftoi(start.y));
			dc.LineTo(ftoi(end.x),ftoi(end.y));
			start.Set(pos.x-4,pos.y-4);
			end.Set(pos.x+4,pos.y+4);
			dc.MoveTo(ftoi(start.x),ftoi(start.y));
			dc.LineTo(ftoi(end.x),ftoi(end.y));
			start.Set(pos.x-4,pos.y+4);
			end.Set(pos.x+4,pos.y-4);
			dc.MoveTo(ftoi(start.x),ftoi(start.y));
			dc.LineTo(ftoi(end.x),ftoi(end.y));
			dc.TextOut((int)pos.x,(int)pos.y,pNode->sTitle,strlen(pNode->sTitle));
		}
		CExpression expression;
		EXPRESSION_VAR *pVar=expression.varList.Append();
		strcpy(pVar->variableStr,"N");
		if(m_xFacePanel.m_iFaceType!=2)
			pVar->fValue=m_xFacePanel.m_nInternodeNum;
		else
			pVar->fValue=1;
		pVar=expression.varList.Append();
		strcpy(pVar->variableStr,"i");
		int i,repeatNum=1;
		if(m_xFacePanel.m_iFaceType!=2)
			repeatNum=m_xFacePanel.m_nInternodeNum-m_xFacePanel.m_nRepeatLessNum;
		for(i=0;i<repeatNum;i++)
		{
			for(CFacePanelPole *pPole=m_xFacePanel.pole_list.GetFirst();pPole;
			pPole=m_xFacePanel.pole_list.GetNext())
			{
				if(i>0&&!pPole->m_bInternodeRepeat)
					continue;
				CFacePanelNode *pStart=&m_xFacePanel.node_list[pPole->start_i];
				CFacePanelNode *pEnd=&m_xFacePanel.node_list[pPole->end_i];
				if(pStart->m_bInternodeRepeat)
				{
					expression.VarAt(1)->fValue=i;//expression.varList[1].fValue=i;
					double scale=expression.SolveExpression(pStart->scale_expression);
					CFacePanelNode *pScaleStart=&m_xFacePanel.node_list[pStart->attach_node_index[0]];
					CFacePanelNode *pScaleEnd=&m_xFacePanel.node_list[pStart->attach_node_index[1]];
					pStart->pos=pScaleStart->pos+(pScaleEnd->pos-pScaleStart->pos)*scale;
				}
				f3dPoint start_pos=pStart->pos;
				if(pEnd->m_bInternodeRepeat)
				{
					expression.VarAt(1)->fValue=i;//expression.varList[1].fValue=i;
					double scale=expression.SolveExpression(pEnd->scale_expression);
					CFacePanelNode *pScaleStart=&m_xFacePanel.node_list[pEnd->attach_node_index[0]];
					CFacePanelNode *pScaleEnd=&m_xFacePanel.node_list[pEnd->attach_node_index[1]];
					pEnd->pos=pScaleStart->pos+(pScaleEnd->pos-pScaleStart->pos)*scale;
				}
				f3dPoint end_pos=pEnd->pos;
				start_pos*=m_fScaleScreenToUser;
				end_pos  *=m_fScaleScreenToUser;
				coord_trans(start_pos,rgn_ucs,FALSE);
				coord_trans(end_pos,rgn_ucs,FALSE);
				dc.MoveTo(ftoi(start_pos.x),ftoi(start_pos.y));
				dc.LineTo(ftoi(end_pos.x),ftoi(end_pos.y));
			}
		}
		dc.SelectObject(pOldPen);
	}
}

void CSelFacePanelDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	/*if(m_pDraw)
	{
		CDrawEngine::Destroy(m_pDraw->GetEngineHandle());
		m_pDraw=NULL;
	}*/
}
