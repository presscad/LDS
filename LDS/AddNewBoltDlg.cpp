// AddNewBoltDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "AddNewBoltDlg.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "SortFunc.h"
#include "XhCharString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef __ANGLE_PART_INC_

/////////////////////////////////////////////////////////////////////////////
// CAddNewBoltDlg dialog


CAddNewBoltDlg::CAddNewBoltDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddNewBoltDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddNewBoltDlg)
	m_fBoltPosX = 320.0;
	m_fBoltPosY = -40.0;
	m_fBoltPosZ = 0.0;
	m_fBoltR = 100;
	m_fStartAngle = 0.0;
	m_fHorzSpace = 60.0;
	m_fVertSpace = 60.0;
	m_fScopeAngle = 0.0;
	m_fFlWidth = 0.0;
	m_fTubeR = 0.0;
	m_nItemsArrayNum = 4;
	m_sBoltSpec = _T("20");
	m_iRectArray = 0;	//0-矩形阵列  非0-环形阵列
	m_iBoltNorm = 0;
	m_sSegNo = _T("");
	m_iRowNum = 2;
	m_iColumnNum = 2;
	m_bDoubleRow = FALSE;		//选择布置双排螺栓时按输入的螺栓位置自动布置为双排螺栓
	m_fSpecSpaceD = 0.0;
	m_bStaggerLayout = TRUE;	//水平面与竖直面交错布置
	m_bMirLayoutBolt = TRUE;	//对称布置特殊间距两侧螺栓
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("Specification");
#else 
	m_arrHeaderLabel.Add("规格");
#endif
	m_arrHeaderLabel.Add("X");
	m_arrHeaderLabel.Add("Y");
	m_arrHeaderLabel.Add("Z");
	listViewComparer.SetMaxColumnCount(4);
	m_pWorkPart = NULL;
	m_pDatumTube = NULL;
}


void CAddNewBoltDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddNewBoltDlg)
	DDX_Control(pDX, IDC_CMB_BOLT_SPEC, m_cmbBoltSpec);
	DDX_Text(pDX, IDC_E_BOLT_POS_X, m_fBoltPosX);
	DDX_Text(pDX, IDC_E_BOLT_POS_Y, m_fBoltPosY);
	DDX_Text(pDX, IDC_E_BOLT_POS_Z, m_fBoltPosZ);
	DDX_Text(pDX, IDC_E_BOLT_R, m_fBoltR);
	DDX_Text(pDX, IDC_E_START_ANGLE, m_fStartAngle);
	DDX_Text(pDX, IDC_E_HORZ_SPACE, m_fHorzSpace);
	DDX_Text(pDX, IDC_E_VERT_SPACE, m_fVertSpace);
	DDX_Text(pDX, IDC_E_SCOPE_ANGLE, m_fScopeAngle);
	DDX_Text(pDX, IDC_E_FL_WIDTH, m_fFlWidth);
	DDX_Text(pDX, IDC_E_TUBE_R, m_fTubeR);
	DDX_Text(pDX, IDC_E_ITEMS_ARRAY_NUM, m_nItemsArrayNum);
	DDX_CBString(pDX, IDC_CMB_BOLT_SPEC, m_sBoltSpec);
	DDX_Radio(pDX, IDC_R_RECT_ARRAY, m_iRectArray);
	DDX_Radio(pDX, IDC_RDO_BOLT_NORM, m_iBoltNorm);
	DDX_CBString(pDX, IDC_E_SEG_NO, m_sSegNo);
	DDX_Text(pDX, IDC_E_ROW_NUM, m_iRowNum);
	DDX_Text(pDX, IDC_E_COLUMN_NUM, m_iColumnNum);
	DDX_Check(pDX, IDC_CHK_DOUBLE_ROW, m_bDoubleRow);
	DDX_Text(pDX, IDC_E_SPEC_SPACE_D, m_fSpecSpaceD);
	DDX_Check(pDX, IDC_CHK_STAGGER_LAYOUT, m_bStaggerLayout);
	DDX_Check(pDX, IDC_CHK_MIR_LAYOUT_BOLT, m_bMirLayoutBolt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddNewBoltDlg, CDialog)
	//{{AFX_MSG_MAP(CAddNewBoltDlg)
	ON_BN_CLICKED(IDC_BN_BOLT_MODIFY, OnBnBoltModify)
	ON_BN_CLICKED(IDC_BN_ADD_BOLT, OnBnAddBolt)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_BOLT, OnDblclkListBolt)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_BOLT, OnKeydownListBolt)
	ON_BN_CLICKED(IDC_BTN_LAYOUT_BOLT, OnBtnLayoutBolt)
	ON_EN_CHANGE(IDC_E_ITEMS_ARRAY_NUM, OnChangeEItemsArrayNum)
	ON_EN_CHANGE(IDC_E_SCOPE_ANGLE, OnChangeEScopeAngle)
	ON_EN_CHANGE(IDC_E_ROW_NUM, OnChangeRowNum)
	ON_EN_CHANGE(IDC_E_COLUMN_NUM, OnChangeColumnNum)
	ON_BN_CLICKED(IDC_R_RECT_ARRAY, OnRectArray)
	ON_BN_CLICKED(IDC_R_ROUND_ARRAY, OnRoundArray)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_BOLT, OnColumnclickListBolt)
	ON_EN_CHANGE(IDC_E_VERT_SPACE, OnChangeVertSpace)
	ON_BN_CLICKED(IDC_CHK_DOUBLE_ROW, OnDoubleRow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddNewBoltDlg message handlers

BOOL CAddNewBoltDlg::OnInitDialog() 
{
	if(m_pWorkPart==NULL)
		return FALSE;

	UCS_STRU tubeUcs;
	CLDSPart *pPart = NULL;
	CLDSPlate *pPlate = NULL;
	CLDSParamPlate *pParamPlate = NULL;
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_BOLT);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	if(m_pWorkPart->m_hPartWeldParent)
	{
		pPart = console.FromPartHandle(m_pWorkPart->m_hPartWeldParent);
		if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
			m_pDatumTube = (CLDSLineTube*)pPart;
	}
	//计算环形阵列时的基准坐标系
	if(m_pDatumTube)
	{
		m_fTubeR=m_pDatumTube->GetDiameter()*0.5;
		m_pDatumTube->BuildUCS();
		tubeUcs=m_pDatumTube->ucs;
	}

	m_sSegNo = m_pWorkPart->iSeg.ToString();
	m_pListCtrl->DeleteAllItems();
	CLsRefList *pLsRefList=NULL;
	if(m_pWorkPart->GetClassTypeId()==CLS_PLATE)
	{	
		//判断是否符合环形阵列的基本条件
		pPlate = (CLDSPlate*)m_pWorkPart;
		if(pPlate&&pPlate->face_N==1&&m_pDatumTube)
		{	
			double angle = cal_angle_of_2vec(pPlate->ucs.axis_z,m_pDatumTube->ucs.axis_z);
			if(angle<(80*RADTODEG_COEF) || angle>(100*RADTODEG_COEF))
			{
				GetDlgItem(IDC_R_RECT_ARRAY)->EnableWindow(TRUE);
				GetDlgItem(IDC_R_ROUND_ARRAY)->EnableWindow(TRUE);
			}
			else
			{
				GetDlgItem(IDC_R_RECT_ARRAY)->EnableWindow(FALSE);
				GetDlgItem(IDC_R_ROUND_ARRAY)->EnableWindow(FALSE);
			}
		}
		m_iRectArray = ((CLDSPlate*)m_pWorkPart)->m_iLsArrayStyle;	//螺栓阵列方式
		pLsRefList=pPlate->GetLsRefList();
	}
	else if(m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		pParamPlate = (CLDSParamPlate*)m_pWorkPart;
		GetDlgItem(IDC_R_RECT_ARRAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_R_ROUND_ARRAY)->EnableWindow(FALSE);
		m_iRectArray = 0;	//螺栓布置方式
		if(pParamPlate->m_iParamType==TYPE_FL)
		{
			m_iRectArray = 1;
			double W=0,R=0;
			pParamPlate->GetDesignItemValue('A', &m_fScopeAngle);
			pParamPlate->GetDesignItemValue('R', &R);
			pParamPlate->GetDesignItemValue('W', &W);
			m_fFlWidth=0.5*W-R;//法兰宽度
			m_nItemsArrayNum = 4;
			m_fStartAngle = 0;
			m_fBoltR = m_fTubeR+m_fFlWidth*0.5;
		}
		else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
		{	//初始化环向板，环形布置螺栓时的参数 wht 11-05-16
			m_iRectArray = 1;
			double Ri=0,Ro=0,D=0;
			pParamPlate->GetDesignItemValue('R',&Ro);
			pParamPlate->GetDesignItemValue('D',&D);
			if(Ro>0)	//异型法兰或其余跨越环向板中指定了外圆弧半径绝对值
				D=Ro-m_fTubeR;
			else if(D>0)
				Ro=m_fTubeR+D;
			m_fFlWidth=D;
			m_nItemsArrayNum = 4;
			m_fStartAngle = 0;
			m_fBoltR = m_fTubeR+m_fFlWidth*0.5;
		}
		else if(pParamPlate->m_iParamType==TYPE_UEND || pParamPlate->m_iParamType==TYPE_ROLLEND)
		{
			double W,M;
			pParamPlate->GetDesignItemValue('W',&W);
			pParamPlate->GetDesignItemValue('M',&M);
			if(pParamPlate->m_iParamType==TYPE_UEND)
				m_fBoltPosZ = -pParamPlate->thick-0.5*W;
			else if(pParamPlate->m_iParamType==TYPE_ROLLEND)
				m_fBoltPosZ = -0.5*pParamPlate->thick;
			m_fScopeAngle=0;
			m_fHorzSpace = 80.0;//螺栓间距1
			m_fVertSpace = 80.0;//螺栓间距2
			m_fBoltPosX = M + 60;
			if(m_nItemsArrayNum==2)
				m_fBoltPosY = 0;
			else
				m_fBoltPosY = -0.5*m_fVertSpace;
		}
		else if(pParamPlate->m_iParamType==TYPE_XEND)
		{
			if(m_bStaggerLayout)
				m_iRowNum=1;
			double S,N,W,A,M,R;
			pParamPlate->GetDesignItemValue('N',&N);
			pParamPlate->GetDesignItemValue('S',&S);
			pParamPlate->GetDesignItemValue('W',&W);
			pParamPlate->GetDesignItemValue('R',&R);
			pParamPlate->GetDesignItemValue('A',&A);
			pParamPlate->GetDesignItemValue('M',&M);
			m_fBoltPosX = 30;
			m_fBoltPosY = N-30;
			m_fBoltPosZ = -(int)(0.5*pParamPlate->GetThick());
			m_fHorzSpace = 60.0;//列距
			m_fVertSpace = N-60;//行距
			m_fSpecSpaceD = 60+30;
			m_iRowNum = 1;
			m_iColumnNum = 4;
		}
		pLsRefList=pParamPlate->GetLsRefList();
	}
	RefreshDlgCtrlState();	//更新控件	
	RefreshListViewHeader();
	if(pLsRefList)
	{	//将板上已经存在的螺栓加入螺栓引用列表中
		for(CLsRef *pLsRef = pLsRefList->GetFirst();pLsRef;pLsRef = pLsRefList->GetNext())
			AddLsRefToList(pLsRef);
	}
	UpdateData(FALSE);
	return CDialog::OnInitDialog();
}

int CAddNewBoltDlg::AppendRecord(int iItem, CStringArray &str_arr)
{
	//insert items
	LV_ITEM lvi;
	if(iItem==-1)
		iItem=m_pListCtrl->GetItemCount();

	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = str_arr[0].GetBuffer(1);//_gszItem[i][0];
	iItem = m_pListCtrl->InsertItem(&lvi);
	//set item text for additional columns
	int nCount = m_arrHeaderLabel.GetSize();
	nCount = __min(nCount,str_arr.GetSize());
	for(int j = 1; j<nCount; j++)
	{
		str_arr[j].TrimLeft();
		str_arr[j].TrimRight();
		m_pListCtrl->SetItemText(iItem,j,str_arr[j]);
	}
	m_pListCtrl->RedrawItems(iItem,iItem);
	return iItem;
}

void CAddNewBoltDlg::OnBnAddBolt() 
{
	CString ss, sBoltSpec;
	CStringArray str_arr;
	UpdateData();
	m_cmbBoltSpec.GetWindowText(sBoltSpec);
	ss.Format("M%s",sBoltSpec);
	str_arr.Add(ss);
	if(m_iRectArray==0)	//矩形阵列
	{
		GetDlgItem(IDC_E_BOLT_POS_X)->GetWindowText(ss);
		str_arr.Add(ss);
		GetDlgItem(IDC_E_BOLT_POS_Y)->GetWindowText(ss);
		str_arr.Add(ss);
		GetDlgItem(IDC_E_BOLT_POS_Z)->GetWindowText(ss);
		str_arr.Add(ss);
	}
	else	//环形阵列
	{
		GetDlgItem(IDC_E_START_ANGLE)->GetWindowText(ss);
		str_arr.Add(ss);
		GetDlgItem(IDC_E_BOLT_R)->GetWindowText(ss);
		str_arr.Add(ss);
	}
	AppendRecord(-1,str_arr);
}

void CAddNewBoltDlg::OnBtnLayoutBolt() 
{
	m_pListCtrl=(CListCtrl*)GetDlgItem(IDC_LIST_BOLT);
	UpdateData();
	int i,nBoltNum;
	CString sBoltSpec, ss;
	CStringArray str_arr;
	m_cmbBoltSpec.GetWindowText(ss);
	sBoltSpec.Format("M%s",ss);
	nBoltNum=m_nItemsArrayNum;
	double posBolt[100][4];
	if(m_iRectArray)	//环形阵列
	{
		double init_angle = m_fStartAngle;
		double piece_angle = m_fScopeAngle/m_nItemsArrayNum;
		str_arr.SetSize(3);
		for(i=0;i<nBoltNum;i++)
		{
			str_arr[0] = sBoltSpec;						//规格
			str_arr[1].Format("%f",m_fBoltR);			//螺栓所在圆的半径
			str_arr[2].Format("%f",init_angle+piece_angle*i);
			SimplifiedNumString(str_arr[2]);			//方位角
			AppendRecord(-1,str_arr);
		}
	}
	else if(m_iRectArray==0)	//矩形阵列
	{
		//根据输入的第一个螺栓的位置,阵列数,以及矩阵(行X列),间距布置螺栓
		CLDSParamPlate *pParamPlate=NULL;
		if(m_pWorkPart&&m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
			pParamPlate=(CLDSParamPlate*)m_pWorkPart;
		//十字插板上布置螺栓时可能需要布置双排螺栓		wht 09-06-06
		//双排螺栓分两种方式：第一种就是默认的每个小包板上螺栓布置相同；
		//					  第二种就是水平面包板上的螺栓与竖直面上的螺栓呈交错布置
		if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
		{	//在包板坐标系下布置十字插板螺栓
			double H=0,N=0,A=0,M=0,R=0;
			pParamPlate->GetDesignItemValue('H',&H);
			pParamPlate->GetDesignItemValue('N',&N);
			pParamPlate->GetDesignItemValue('A',&A);
			pParamPlate->GetDesignItemValue('M',&M);
			pParamPlate->GetDesignItemValue('R',&R);
			double fBoltPosX=m_fBoltPosX+A+M;
			double fBoltPosY=m_fBoltPosY+R;
			double fBoltPosZ=m_fBoltPosZ;
			//double Y=0.5*H-(N-(0.5*H-m_fBoltPosY));
			//布置十字插板螺栓时用户输入的为相对包板的坐标此处需要将相对包板的坐标转换到十字插板坐标系下
			//水平面与竖直面交错布置时的螺栓Y坐标与H无关 wht 10-01-22
			double Y=R+(N-m_fBoltPosY);	//交错布置时的螺栓Y坐标
			double offset_dist=0;
			if(m_bDoubleRow)
			{	//双排布置螺栓
				double fSecRowBoltY=0,fSecY=0;
				//根据首个螺栓的位置自动判断第二行螺栓的Y坐标
				if((m_fBoltPosY+m_fVertSpace)>N)
					fSecRowBoltY=fBoltPosY-m_fVertSpace;
				else
					fSecRowBoltY=fBoltPosY+m_fVertSpace;
				fSecY=R+(N-(fSecRowBoltY-R));	//交错布置时的螺栓Y坐标
				for(i=0;i<nBoltNum;i++)
				{	//从第nBoltNum/2个螺栓开始考虑特殊间距造成的偏差
					double pos_y=fBoltPosY;
					if(i%2>0)
						pos_y=fSecRowBoltY;
					if(i>(nBoltNum/2-1))
					{	//在包板但不在基板上的螺栓考虑由于特殊间距造成的偏差
						offset_dist=m_fSpecSpaceD-m_fHorzSpace;
						if(m_bMirLayoutBolt)
						{	//对称布置特殊间距两侧螺栓	wht 10-03-09
							pos_y=fSecRowBoltY;
							if(i%2>0)
								pos_y=fBoltPosY;
						}
					}
					posBolt[4*i][0] = fBoltPosX+m_fHorzSpace*i+offset_dist;
					posBolt[4*i][1] = pos_y;
					posBolt[4*i][2] = fBoltPosZ;
					posBolt[4*i][3] = 1;//X-Y平面: Y正方向一侧的螺栓
					posBolt[4*i+1][0] = fBoltPosX+m_fHorzSpace*i+offset_dist;
					posBolt[4*i+1][1] = -pos_y;
					posBolt[4*i+1][2] = fBoltPosZ;
					posBolt[4*i+1][3] = 2;//X-Y平面: Y负方向一侧的螺栓
					if(m_bStaggerLayout)
					{
						pos_y=Y;
						if(i%2>0)
							pos_y=fSecY;	
						if(i>(nBoltNum/2-1)&&m_bMirLayoutBolt)
						{	//对称布置特殊间距两侧螺栓 wht 10-03-09
							pos_y=fSecY;
							if(i%2>0)
								pos_y=Y;
						}
						posBolt[4*i+2][0] = fBoltPosX+m_fHorzSpace*i+offset_dist;
						posBolt[4*i+2][1] = fBoltPosZ;
						posBolt[4*i+2][2] = pos_y;
						posBolt[4*i+2][3] = 3;//X-Z平面: Z正方向一侧的螺栓
						posBolt[4*i+3][0] = fBoltPosX+m_fHorzSpace*i+offset_dist;
						posBolt[4*i+3][1] = fBoltPosZ;
						posBolt[4*i+3][2] = -pos_y;
						posBolt[4*i+3][3] = 4;//X-Z平面: Z负方向一侧的螺栓
					}
					else
					{
						posBolt[4*i+2][0] = fBoltPosX+m_fHorzSpace*i+offset_dist;
						posBolt[4*i+2][1] = fBoltPosZ;
						posBolt[4*i+2][2] = pos_y;
						posBolt[4*i+2][3] = 3;//X-Z平面: Z正方向一侧的螺栓
						posBolt[4*i+3][0] = fBoltPosX+m_fHorzSpace*i+offset_dist;
						posBolt[4*i+3][1] = fBoltPosZ;
						posBolt[4*i+3][2] = -pos_y;
						posBolt[4*i+3][3] = 4;//X-Z平面: Z负方向一侧的螺栓
					}
				}
			}
			else 
			{	//使用特殊间距布置单排螺栓
				for(i=0;i<nBoltNum;i++)
				{	//从第nBoltNum/2个螺栓开始考虑特殊间距造成的偏差
					if(i>(nBoltNum/2-1))
						offset_dist=m_fSpecSpaceD-m_fHorzSpace;
					posBolt[4*i][0] = fBoltPosX+m_fHorzSpace*i+offset_dist;
					posBolt[4*i][1] = fBoltPosY;
					posBolt[4*i][2] = fBoltPosZ;
					posBolt[4*i][3] = 1;//X-Y平面: Y正方向一侧的螺栓
					posBolt[4*i+1][0] = fBoltPosX+m_fHorzSpace*i+offset_dist;
					posBolt[4*i+1][1] = -fBoltPosY;
					posBolt[4*i+1][2] = fBoltPosZ;
					posBolt[4*i+1][3] = 2;//X-Y平面: Y负方向一侧的螺栓
					posBolt[4*i+2][0] = fBoltPosX+m_fHorzSpace*i+offset_dist;
					posBolt[4*i+2][1] = fBoltPosZ;
					posBolt[4*i+2][2] = fBoltPosY;
					posBolt[4*i+2][3] = 3;//X-Z平面: Z正方向一侧的螺栓
					posBolt[4*i+3][0] = fBoltPosX+m_fHorzSpace*i+offset_dist;
					posBolt[4*i+3][1] = fBoltPosZ;
					posBolt[4*i+3][2] = -fBoltPosY;
					posBolt[4*i+3][3] = 4;//X-Z平面: Z负方向一侧的螺栓
				}
			}
			nBoltNum*=4;
		}
		else
		{
			for(i=0;i<nBoltNum;i++)
			{
				posBolt[i][0] = m_fBoltPosX+m_fHorzSpace*(i/m_iRowNum);
				posBolt[i][1] = m_fBoltPosY+m_fVertSpace*(i%m_iRowNum);
				posBolt[i][2] = m_fBoltPosZ;
				posBolt[i][3] = 0;
			}
			if(nBoltNum%m_iRowNum)	//最后一个螺栓位于中线上
				posBolt[nBoltNum-1][1] += 0.5*m_fVertSpace;
		}
		str_arr.SetSize(4);
		for(i=0;i<nBoltNum;i++)
		{
			str_arr[0] = sBoltSpec;						//规格
			str_arr[1].Format("%f",posBolt[i][0]);
			SimplifiedNumString(str_arr[1]);			//X
			str_arr[2].Format("%f",posBolt[i][1]);
			SimplifiedNumString(str_arr[2]);			//Y
			str_arr[3].Format("%f",posBolt[i][2]);
			SimplifiedNumString(str_arr[3]);			//Z	
			int iItem=AppendRecord(-1,str_arr);
			m_pListCtrl->SetItemData(iItem,(DWORD)posBolt[i][3]);//螺栓位置标志
		}
	}
}

void CAddNewBoltDlg::DeleteItem(int nCurSel) 
{
	long h=(long)m_pListCtrl->GetItemData(nCurSel);
	if(h>0x20)
	{
		if(m_pWorkPart->GetClassTypeId()==CLS_PLATE)
			((CLDSPlate*)m_pWorkPart)->DeleteLsRef(h);
		else if(m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
			((CLDSParamPlate*)m_pWorkPart)->DeleteLsRef(h);
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(h);
		g_pSolidOper->KeyDown(VK_DELETE);	//删除构件
	}
	m_pListCtrl->DeleteItem(nCurSel);
}

void CAddNewBoltDlg::OnKeydownListBolt(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	m_pListCtrl=(CListCtrl*)GetDlgItem(IDC_LIST_BOLT);
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_DELETE)
		{
			DeleteItem(iCurSel);
			pos = m_pListCtrl->GetFirstSelectedItemPosition();
		}
		else
			break;
	}
	*pResult = 0;
}

void CAddNewBoltDlg::OnBnBoltModify() 
{
	m_pListCtrl=(CListCtrl*)GetDlgItem(IDC_LIST_BOLT);
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		UpdateData();
		CString sBoltSpec,dd;
		m_cmbBoltSpec.GetWindowText(dd);
		sBoltSpec.Format("M%s",dd);
		m_pListCtrl->SetItemText(iCurSel,0,sBoltSpec);
		CXhChar100 ss;//[MAX_TEMP_CHAR_100+1]="";
		if(m_iRectArray==0)	//矩形阵列
		{
			ss.Printf("%.2f",m_fBoltPosX);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,1,ss);
			ss.Printf("%.2f",m_fBoltPosY);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,2,ss);
			ss.Printf("%.2f",m_fBoltPosZ);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,3,ss);
		}
		else	//环形阵列
		{
			ss.Printf("%.2f",m_fBoltR);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,1,ss);
			ss.Printf("%.2f",m_fStartAngle);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,2,ss);
		}
	}
}

void CAddNewBoltDlg::OnDblclkListBolt(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_pListCtrl=(CListCtrl*)GetDlgItem(IDC_LIST_BOLT);
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		char sText[51];
		m_pListCtrl->GetItemText(iCurSel,0,sText,50);
		m_sBoltSpec.Format("%s",sText+1);
		if(m_iRectArray==0)	//矩形阵列
		{
			m_pListCtrl->GetItemText(iCurSel,1,sText,50);
			m_fBoltPosX=atof(sText);
			m_pListCtrl->GetItemText(iCurSel,2,sText,50);
			m_fBoltPosY=atof(sText);
			m_pListCtrl->GetItemText(iCurSel,3,sText,50);
			m_fBoltPosZ=atof(sText);
		}
		else //环形阵列
		{
			m_pListCtrl->GetItemText(iCurSel,1,sText,50);
			m_fBoltR=atof(sText);
			m_pListCtrl->GetItemText(iCurSel,2,sText,50);
			m_fStartAngle=atof(sText);
		}
		UpdateData(FALSE);
	}
	*pResult = 0;
}

void CAddNewBoltDlg::CreateBolt(CLDSPart *pPart/*=NULL*/)
{	
	if(pPart==NULL)
		pPart=m_pWorkPart;
	if(pPart==NULL)
		return;
	f3dPoint pos;
	CLDSPlate *pPlate = NULL;
	CLDSParamPlate *pParamPlate = NULL;
	DWORD dwRayNo = 0;
	if(pPart&&pPart->GetClassTypeId()==CLS_PLATE)
	{
		pPlate = (CLDSPlate*)pPart;
		pPlate->m_iLsArrayStyle = m_iRectArray;	//保存钢板上的螺栓阵列方式
	}
	else if(pPart&&pPart->GetClassTypeId()==CLS_PARAMPLATE) 
		pParamPlate = (CLDSParamPlate*)pPart;
	if(m_iRectArray!=0)	//环向布置螺栓
	{
		if(m_iBoltNorm==0)
		{	//以钢管焊道线为基准
			if(pPlate)
				pPlate->m_bAttachPartByWeldRoad = TRUE;
			else if(pParamPlate)
				pParamPlate->SetDesignItemValue('J',1,"环向布置肘板或者螺栓时:1表示以焊道线为基准，0表示以基准边为基准。");
		}
		else //if(m_iBoltNorm==1)
		{	//以钢管基准边为基准
			if(pPlate)
				pPlate->m_bAttachPartByWeldRoad = FALSE;
			else if(pParamPlate)
				pParamPlate->SetDesignItemValue('J',0,"环向布置肘板或者螺栓时:1表示以焊道线为基准，0表示以基准边为基准。");
		}
	}
	//螺栓数据定义区
	int nCount=m_pListCtrl->GetItemCount();
	ATOM_LIST<RAYNO_RECORD>rayNoList;
	int iInitRayNo=1;
	char sText[101];
	for(int i=0;i<nCount;i++)
	{
		long h=(long)m_pListCtrl->GetItemData(i);
		if(h>0x20)	//原有螺栓,跳过
			continue;
		m_pListCtrl->GetItemText(i,0,sText,100);
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->set_d(atoi(sText+1));
		pBolt->iSeg = SEGI(m_sSegNo.GetBuffer());
		pBolt->SetLayer(pPart->layer());	//调整螺栓的图层名
		pBolt->cfgword=pPart->cfgword;		//调整螺栓配材号与基准构件配材号一致
		pBolt->des_base_pos.hPart = pPart->handle;
		pBolt->des_base_pos.cLocationStyle=TRUE; 
		pBolt->des_work_norm.norm_style=3;	//构件法线
		pBolt->des_work_norm.hVicePart=pPart->handle;
		pBolt->des_work_norm.direction=m_iBoltNorm;
		if(m_iRectArray==0)		//矩形阵列
		{
			m_pListCtrl->GetItemText(i,1,sText,100);
			pos.x=atof(sText);
			m_pListCtrl->GetItemText(i,2,sText,100);
			pos.y=atof(sText);
			m_pListCtrl->GetItemText(i,3,sText,100);
			pos.z=atof(sText);
			pBolt->des_base_pos.len_offset_dist=pos.x;
			double L=0;
			if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
			{	//十字插板	
				pParamPlate->GetDesignItemValue('L',&L);
				pBolt->des_base_pos.wing_offset_dist=pos.y;
				if(h==1)
				{	//X-Y平面: Y正方向一侧的螺栓
					if(L>pos.x)//Y正方向一侧,连接插板与包板的螺栓
						pBolt->feature=11;
					else
						pBolt->feature=12;	//Y正方向一侧,连接包板与其他连板的螺栓
					pBolt->des_base_pos.norm_offset.AddThick((LONG)pos.z);
				}
				else if(h==2)
				{	//X-Y平面: Y负方向一侧的螺栓
					if(L>pos.x)//Y负方向一侧,连接插板与包板的螺栓
						pBolt->feature=21;
					else
						pBolt->feature=22;	//Y负方向一侧,连接包板与其他连板的螺栓
					pBolt->des_base_pos.norm_offset.AddThick((LONG)pos.z);
				}
				else if(h==3)	
				{	//X-Z平面: Z正方向一侧
					if(L>pos.x)
						pBolt->feature=31;	//Z正方向一侧，连接立板与其他连板的螺栓
					else
						pBolt->feature=32;	//Z正方向一侧，连接包板与其他连板的螺栓
					pBolt->des_work_norm.norm_style=4;
					pBolt->des_work_norm.nearVector.Set(0,1,0);
					pBolt->des_base_pos.wing_offset_dist = pos.z;
					pBolt->ucs.axis_z=pParamPlate->ucs.axis_y;
					pBolt->des_base_pos.norm_offset.AddThick((LONG)pos.y);
				}
				else if(h==4)	
				{	//X-Z平面: Z负方向一侧
					if(L>pos.x)
						pBolt->feature=41;	//Z负方向一侧，连接立板与其他连板的螺栓
					else
						pBolt->feature=42;	//Z负方向一侧，连接包板与其他连板的螺栓
					pBolt->des_work_norm.norm_style=4;
					pBolt->des_work_norm.nearVector.Set(0,1,0);
					pBolt->des_base_pos.wing_offset_dist = pos.z;
					pBolt->ucs.axis_z=pParamPlate->ucs.axis_y;
					pBolt->des_base_pos.norm_offset.AddThick((LONG)pos.y);
				}
				if(pBolt->feature==11||pBolt->feature==21)	//螺栓在十字插板范围内修改螺栓通厚
					pBolt->AddL0Thick(pPart->handle,TRUE);
			}
			else
			{
				pBolt->AddL0Thick(pPart->handle,TRUE);
				pBolt->des_base_pos.wing_offset_dist = pos.y;
				pBolt->des_base_pos.norm_offset.AddThick((LONG)pos.z);
			}
			pBolt->des_work_norm.direction=m_iBoltNorm;	//0-朝外 1-朝内
			//coord_trans(pos,pPart->ucs,TRUE);
			RAYNO_RECORD *pRayNo;
			for(pRayNo=rayNoList.GetFirst();pRayNo;pRayNo=rayNoList.GetNext())
			{
				if(pRayNo->yCoord==pos.y)
				{	
					pBolt->dwRayNo=pRayNo->dwRayNo;
					break;
				}
			}
			if(pRayNo==NULL)
			{
				pRayNo=rayNoList.append();
				pRayNo->dwRayNo=GetSingleWord(iInitRayNo);
				pRayNo->yCoord=pos.y;
				pBolt->dwRayNo=pRayNo->dwRayNo;
				pRayNo->hPart=pPart->handle;
				iInitRayNo++;
			}
		}
		else	//环形阵列
		{
			if(!m_pDatumTube)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox("The part has no weld parent part that can't circular arrange bolts!");
#else 
				MessageBox("该构件无焊接父构件,不能环形阵列布置螺栓!");
#endif
				return;
			}
			pBolt->AddL0Thick(pPart->handle,TRUE);//螺栓通厚
			m_pListCtrl->GetItemText(i,2,sText,100);	//方位角
			double angle = atof(sText)*RADTODEG_COEF;	
			pBolt->des_base_pos.offset_angle = angle;	//螺栓相对于基准钢管的偏移角度
			m_pListCtrl->GetItemText(i,1,sText,100);	//螺栓距基准构件中心的距离
			pBolt->des_base_pos.R=atof(sText);			//该变量小于0表示螺栓为非环向布置
			pBolt->des_work_norm.direction=0;			//环向布置的螺栓默认朝外
			//调整环向肋板法向偏移量 wht 11-05-16
			if(m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE
				&&((CLDSParamPlate*)m_pWorkPart)->m_iParamType==TYPE_CIRRIBPLATE)
				pBolt->des_base_pos.norm_offset.AddThick(-ftol(0.5*m_pWorkPart->GetThick()));
		}
		CLsRef *pLsRef=NULL;
		if(pPlate)
			pLsRef=pPlate->AppendLsRef(pBolt->GetLsRef());
		else if(pParamPlate)
			pLsRef=pParamPlate->AppendLsRef(pBolt->GetLsRef());
		if(pLsRef)
			pLsRef->start1_end2_mid0=(BYTE)pBolt->feature;	//标记螺栓所在位置
		pBolt->SetModified();
		pBolt->CalGuigeAuto();
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	//设置螺栓引用射线号
	if(m_iRectArray==0&&pParamPlate)	//在参数化板上矩形阵列布置螺栓
		pParamPlate->GetLsRefList()->ResetBoltRefRayNo(pParamPlate);
	else if(m_iRectArray==0&&pPlate)	//在钢板上矩形阵列布置螺栓
		pPlate->GetLsRefList()->ResetBoltRefRayNo(pPlate);	
	/*
	//生成十字插板后，单独在插板上布置螺栓可能运行到此
	if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
	{	//布置完螺栓后即刻引用螺栓到包板上
		double X=0,W=0;
		for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
		{
			CLDSParamPlate *pPlate=(CLDSParamPlate*)console.FromPartHandle(pItem->h,CLS_PARAMPLATE);
			if(pPlate==NULL)
				continue;
			if(pPlate->m_iParamType!=TYPE_XEND)
				continue;	//非十字插板
			if(!pPlate->GetDesignItemValue('X',&X))
				continue;	//非十字插板包板
			RefLsToPart(pParamPlate->GetLsRefList(),pPlate);
		}
		pParamPlate->GetDesignItemValue('W',&W);
		for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
		{	//调整螺栓法向偏距
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			pBolt->des_base_pos.norm_offset.AddThick(-(long)W);
			pBolt->correct_pos();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
	}
	*/
}

void CAddNewBoltDlg::RefreshListViewHeader()
{
	LV_COLUMN lvc;
	int i, NUM_COLUMNS, nColumnCount;
	CHeaderCtrl *pHeader = m_pListCtrl->GetHeaderCtrl();
	nColumnCount = pHeader->GetItemCount();

	// Delete all of the columns.
	for (i=0;i < nColumnCount;i++)
	{
	   m_pListCtrl->DeleteColumn(0);
	}
	if(m_arrHeaderLabel.GetSize()>0)
		NUM_COLUMNS = m_arrHeaderLabel.GetSize();
	else
		NUM_COLUMNS = 0;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(i = 0; i<NUM_COLUMNS; i++)
	{
		lvc.iSubItem = i;
		lvc.pszText = m_arrHeaderLabel[i].GetBuffer(1);//_gszColumnLabel[i];
		if(i<1)
			lvc.cx = 80;
		else
			lvc.cx = 90;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}
//按螺栓位置Y坐标升序排列
int compareBoltPosY(const CLDSBolt &bolt1,const CLDSBolt &bolt2)
{
	if(bolt1.ucs.origin.y>bolt1.ucs.origin.y)
		return 1;
	else if(bolt1.ucs.origin.y<bolt1.ucs.origin.y)
		return -1;
	else 
		return 0;
}

void CAddNewBoltDlg::OnOK() 
{
	CreateBolt();
	CDialog::OnOK();
}

void CAddNewBoltDlg::OnChangeEItemsArrayNum() 
{
	UpdateData();
	m_fStartAngle = (m_fScopeAngle/m_nItemsArrayNum)*0.5;
	m_iColumnNum = (int)(m_nItemsArrayNum/m_iRowNum);
	UpdateData(FALSE);
}

void CAddNewBoltDlg::OnChangeEScopeAngle() 
{
	UpdateData();
	m_fStartAngle = 0;
	UpdateData(FALSE);
}

void CAddNewBoltDlg::RefreshDlgCtrlState()
{
	CLDSParamPlate *pParamPlate = NULL;
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
	m_CurPlateBmp.Detach();
#ifdef AFX_TARG_ENU_ENGLISH
	GetDlgItem(IDC_S_GROUP)->SetWindowText("Bolt Orientation");
	GetDlgItem(IDC_RDO_BOLT_NORM)->SetWindowText("Toward Outside");
	GetDlgItem(IDC_RDO_BOLT_NORM2)->SetWindowText("Toward Inside");
#else 
	GetDlgItem(IDC_S_GROUP)->SetWindowText("螺栓朝向");
	GetDlgItem(IDC_RDO_BOLT_NORM)->SetWindowText("朝外");
	GetDlgItem(IDC_RDO_BOLT_NORM2)->SetWindowText("朝里");
#endif
	if(m_pWorkPart&&m_pWorkPart->GetClassTypeId()==CLS_PLATE)
	{
		if(m_iRectArray)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_FL);
		else//矩形阵列
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_NONE1);
	}
	else if(m_pWorkPart&&m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		pParamPlate=(CLDSParamPlate*)m_pWorkPart;
		if(pParamPlate->m_iParamType==TYPE_FL)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_FL);
		else if(pParamPlate->m_iParamType==TYPE_UEND)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_UEND_BOLT);
		else if(pParamPlate->m_iParamType==TYPE_ROLLEND)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_ROLLEND_BOLT);
		else if(pParamPlate->m_iParamType==TYPE_XEND)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_XEND_BOLT);
		else
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_NONE);
	}

	GetDlgItem(IDC_LAYOUT_STYLE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_R_RECT_ARRAY)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_R_ROUND_ARRAY)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_S_RECT_LAYOUT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_E_COLUMN_NUM)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_E_ROW_NUM)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_S_X)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_E_SCOPE_ANGLE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_E_SPEC_SPACE_D)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CHK_DOUBLE_ROW)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CHK_STAGGER_LAYOUT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CHK_MIR_LAYOUT_BOLT)->ShowWindow(SW_HIDE);
#ifdef AFX_TARG_ENU_ENGLISH
	GetDlgItem(IDC_S_ANGLE)->SetWindowText("Fill Angle:");
	GetDlgItem(IDC_S_NAME_PROMPT)->SetWindowText("Tube Eexternal Radius:");
#else 
	GetDlgItem(IDC_S_ANGLE)->SetWindowText("填充角度:");
	GetDlgItem(IDC_S_NAME_PROMPT)->SetWindowText("钢管外半径:");
#endif
	GetDlgItem(IDC_E_VERT_SPACE)->EnableWindow(TRUE);
	if(m_iRectArray==0)
	{	//矩形阵列
		m_arrHeaderLabel.RemoveAll();
#ifdef AFX_TARG_ENU_ENGLISH          
		m_arrHeaderLabel.Add("Specification");
#else 
		m_arrHeaderLabel.Add("规格");
#endif
		m_arrHeaderLabel.Add("X");
		m_arrHeaderLabel.Add("Y");
		m_arrHeaderLabel.Add("Z");
		GetDlgItem(IDC_E_SCOPE_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_START_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BOLT_R)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BOLT_POS_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BOLT_POS_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BOLT_POS_Z)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_HORZ_SPACE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_VERT_SPACE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_ROW_NUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_COLUMN_NUM)->EnableWindow(TRUE);
		if(m_pWorkPart&&m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
		{	//十字插板上布置螺栓时可能需要布置双排螺栓		wht 09-06-06
			//双排螺栓分两种方式：第一种就是默认的每个小包板上螺栓布置相同；
			//					  第二种就是水平面包板上的螺栓与竖直面上的螺栓呈交错布置
			CLDSParamPlate *pXEndPlate=(CLDSParamPlate*)m_pWorkPart;
			if(pXEndPlate->m_iParamType==TYPE_XEND)
			{
				pXEndPlate->GetDesignItemValue('N',&m_fTubeR);
				GetDlgItem(IDC_LAYOUT_STYLE)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_R_RECT_ARRAY)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_R_ROUND_ARRAY)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_S_RECT_LAYOUT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_E_COLUMN_NUM)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_E_ROW_NUM)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_S_X)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_E_SCOPE_ANGLE)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_E_SPEC_SPACE_D)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_CHK_DOUBLE_ROW)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_CHK_STAGGER_LAYOUT)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_CHK_MIR_LAYOUT_BOLT)->ShowWindow(SW_SHOW);
#ifdef AFX_TARG_ENU_ENGLISH
				GetDlgItem(IDC_S_ANGLE)->SetWindowText("Special Distance D:");
				GetDlgItem(IDC_S_NAME_PROMPT)->SetWindowText("Wrap Plate Width N:");
#else 
				GetDlgItem(IDC_S_ANGLE)->SetWindowText("特殊间距D:");
				GetDlgItem(IDC_S_NAME_PROMPT)->SetWindowText("包板宽度N:");
#endif
				GetDlgItem(IDC_E_VERT_SPACE)->EnableWindow(m_bDoubleRow);
				GetDlgItem(IDC_CHK_MIR_LAYOUT_BOLT)->EnableWindow(m_bDoubleRow);
			}
		}
	}
	else
	{	//环形阵列
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_GROUP)->SetWindowText("Datum Line");
		GetDlgItem(IDC_RDO_BOLT_NORM)->SetWindowText("Weld Road Line");
		GetDlgItem(IDC_RDO_BOLT_NORM2)->SetWindowText("Datum Edge");
#else 
		GetDlgItem(IDC_S_GROUP)->SetWindowText("基准线");
		GetDlgItem(IDC_RDO_BOLT_NORM)->SetWindowText("焊道线");
		GetDlgItem(IDC_RDO_BOLT_NORM2)->SetWindowText("基准边");
#endif
		if(m_pDatumTube&&!m_pDatumTube->m_bHasWeldRoad)
			GetDlgItem(IDC_RDO_BOLT_NORM)->EnableWindow(FALSE);
		long J=0;
		if(m_pWorkPart->GetClassTypeId()==CLS_PLATE)
			J=((CLDSPlate*)m_pWorkPart)->m_bAttachPartByWeldRoad;
		else if(m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			if(m_pDatumTube&&!((CLDSParamPlate*)m_pWorkPart)->GetDesignItemValue('J',&J))
				J=m_pDatumTube->feature;	//读取钢管中以前变量m_bAttachPartByWeldRoad的值 现在该变量已去掉
		}
		if(m_pDatumTube&&m_pDatumTube->m_bHasWeldRoad&&J!=0)
			m_iBoltNorm = 0;	//以焊道线为基准
		else 
			m_iBoltNorm = 1;	//以钢管基准线为基准
		m_fScopeAngle = 360;
		m_arrHeaderLabel.RemoveAll();
#ifdef AFX_TARG_ENU_ENGLISH
		m_arrHeaderLabel.Add("Specification");
		m_arrHeaderLabel.Add("R'");
		m_arrHeaderLabel.Add("Directional Angle α");
#else 
		m_arrHeaderLabel.Add("规格");
		m_arrHeaderLabel.Add("R'");
		m_arrHeaderLabel.Add("方位角α");
#endif
		GetDlgItem(IDC_E_SCOPE_ANGLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_START_ANGLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BOLT_R)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_BOLT_POS_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BOLT_POS_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BOLT_POS_Z)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_HORZ_SPACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_VERT_SPACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_ROW_NUM)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_COLUMN_NUM)->EnableWindow(FALSE);
	}
	pPic->SetBitmap(m_CurPlateBmp);
}

BOOL CAddNewBoltDlg::AddLsRefToList(CLsRef *pLsRef)
{
	UCS_STRU tubeUcs;
	if(m_pDatumTube)
	{
		m_fTubeR=m_pDatumTube->GetDiameter()*0.5;
		m_pDatumTube->BuildUCS();
		tubeUcs=m_pDatumTube->ucs;
	}
	CString ss;
	CStringArray str_arr;
	ss.Format("M%d",pLsRef->GetLsPtr()->get_d());
	str_arr.Add(ss);	//螺栓直径
	if(m_iRectArray==0)	//矩形阵列
	{
		f3dPoint origin=pLsRef->GetLsPtr()->ucs.origin;
		coord_trans(origin,m_pWorkPart->ucs,FALSE);	//矩形阵列以板的坐标系为参考坐标系
		ss.Format("%f", origin.x);
		SimplifiedNumString(ss);
		str_arr.Add(ss);	//螺栓X坐标
		ss.Format("%f", origin.y);
		SimplifiedNumString(ss);
		str_arr.Add(ss);	//螺栓Y坐标
		ss.Format("%f", origin.z);
		SimplifiedNumString(ss);
		str_arr.Add(ss);	//螺栓Z坐标
		int i=AppendRecord(-1,str_arr);
		m_pListCtrl->SetItemData(i,pLsRef->GetLsPtr()->handle);
	}
	else	//环形阵列
	{
		if(!m_pDatumTube)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox("The part has no weld parent part!");
#else 
			MessageBox("该构件无焊接父构件!");
#endif
			return FALSE;
		}
		f3dPoint origin=pLsRef->GetLsPtr()->ucs.origin;
		coord_trans(origin,m_pWorkPart->ucs,FALSE);
		double dist = sqrt(SQR(origin.x)+SQR(origin.y));
		ss.Format("%f", dist);
		SimplifiedNumString(ss);
		str_arr.Add(ss);	//螺栓位置到原点的距离
		coord_trans(origin,m_pWorkPart->ucs,TRUE);
		f3dLine line(m_pWorkPart->ucs.origin,origin);
		f3dPoint inters1, inters2,line_vec,tempPt;
		line_vec = line.endPt - line.startPt;
		normalize(line_vec);
		int n=Int3dlc(&inters1,&inters2,line.startPt,line_vec,
				m_pDatumTube->Start(),m_pDatumTube->End(),m_pDatumTube->GetDiameter()/2,m_pDatumTube->GetDiameter()/2);
		if(n==1)
			tempPt = inters1;
		else if(n==2)
		{
			if(DISTANCE(inters1,origin)<DISTANCE(inters2,origin))
				tempPt = inters1;
			else 
				tempPt = inters2;
		}
		coord_trans(tempPt,m_pDatumTube->ucs,FALSE);
		double fPosAngle=Cal2dLineAng(0,0,tempPt.x,tempPt.y);
		/*
		coord_trans(origin,m_pDatumTube->ucs,FALSE);
		double fPosAngle=Cal2dLineAng(0,0,origin.x,origin.y);*/
		long J=0;
		if(m_pWorkPart->GetClassTypeId()==CLS_PLATE)
			J=((CLDSPlate*)m_pWorkPart)->m_bAttachPartByWeldRoad;
		else if(m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			if(!((CLDSParamPlate*)m_pWorkPart)->GetDesignItemValue('J',&J))
				J=m_pDatumTube->feature;	//读取钢管中以前变量m_bAttachPartByWeldRoad的值 现在该变量已去掉
		}
		if(m_pDatumTube->m_bHasWeldRoad&&J!=0)
		{
			int n=(int)(m_pDatumTube->CalWeldRoadAngle()/(2*Pi));
			fPosAngle-=(m_pDatumTube->CalWeldRoadAngle()-2*n*Pi);
		}
		fPosAngle*=DEGTORAD_COEF;
		if(fPosAngle<0)
			fPosAngle+=360;
		ss.Format("%f",fPosAngle);
		SimplifiedNumString(ss);
		str_arr.Add(ss);	//方位角
		int i=AppendRecord(-1,str_arr);
		m_pListCtrl->SetItemData(i,pLsRef->GetLsPtr()->handle);
	}
	UpdateData(FALSE);
	return TRUE;
}

void CAddNewBoltDlg::OnRectArray() 
{	//切换螺栓布置方式时 有问题，如何同时显示矩阵布置和环向布置的螺栓
	m_iRectArray = 0;
	RefreshDlgCtrlState();
	RefreshListViewHeader();
}

void CAddNewBoltDlg::OnRoundArray() 
{
	m_iRectArray = 1;
	RefreshDlgCtrlState();
	RefreshListViewHeader();
}

void CAddNewBoltDlg::OnColumnclickListBolt(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	listViewComparer.m_pListCtrl = m_pListCtrl;
	listViewComparer.m_iColumn = pNMListView->iSubItem;
	listViewComparer.sortOrderAscending[pNMListView->iSubItem]=!listViewComparer.sortOrderAscending[pNMListView->iSubItem];
	m_pListCtrl->SortItems(CompareItemProc,(DWORD)&listViewComparer);
	*pResult = 0;
}

void CAddNewBoltDlg::OnChangeColumnNum()
{
	UpdateData();
	m_nItemsArrayNum=m_iRowNum*m_iColumnNum;
	UpdateData(FALSE);
}

void CAddNewBoltDlg::OnChangeRowNum()
{
	UpdateData();
	m_nItemsArrayNum=m_iRowNum*m_iColumnNum;
	UpdateData(FALSE);
}

void CAddNewBoltDlg::OnChangeVertSpace()
{
	UpdateData();
	if(m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE
		&&((CLDSParamPlate*)m_pWorkPart)->m_iParamType==TYPE_XEND)
	{
		double N=0;
		((CLDSParamPlate*)m_pWorkPart)->GetDesignItemValue('N',&N);
		m_fBoltPosY=N-m_fBoltPosX;
	}
	else
		m_fBoltPosY=-m_fVertSpace*0.5;
	UpdateData(FALSE);
}

void CAddNewBoltDlg::OnDoubleRow() 
{
	UpdateData();
	//选择了双排布置则不启用行数列数
	GetDlgItem(IDC_E_VERT_SPACE)->EnableWindow(m_bDoubleRow);
	GetDlgItem(IDC_CHK_MIR_LAYOUT_BOLT)->EnableWindow(m_bDoubleRow);
	if(m_bDoubleRow)
		m_iRowNum=1;
	UpdateData(FALSE);
}
#endif