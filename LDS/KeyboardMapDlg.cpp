// KeyboardMapDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "MainFrm.h"
#include "KeyboardMapDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKeyboardMapDlg dialog

CKeyboardMapDlg::CKeyboardMapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyboardMapDlg::IDD, pParent)
{
#ifdef AFX_TARG_ENU_ENGLISH
	m_listKeyboardMap.AddColumnHeader("Command");
	m_listKeyboardMap.AddColumnHeader("Command Line");
	m_listKeyboardMap.AddColumnHeader("Shortcut Key");
	m_listKeyboardMap.AddColumnHeader("Description");
	m_bEnglishEnv = TRUE;
#else 
	m_listKeyboardMap.AddColumnHeader("命令");
	m_listKeyboardMap.AddColumnHeader("命令行");
	m_listKeyboardMap.AddColumnHeader("快捷键");
	m_listKeyboardMap.AddColumnHeader("描述");
#endif
}


void CKeyboardMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyboardMapDlg)
	DDX_Control(pDX, IDC_LIST_KEYBOARD_SHORTCUTS, m_listKeyboardMap);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKeyboardMapDlg, CDialog)
	//{{AFX_MSG_MAP(CKeyboardMapDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyboardMapDlg message handlers
//
void CKeyboardMapDlg::InitCmdMapItems()
{
	//初始化快捷键命令
#ifdef AFX_TARG_ENU_ENGLISH
	m_xCmdMapHash.SetValue("NewTower", SYS_CMD_ITEM("New", "Ctrl+N", "N", "New File"));
	m_xCmdMapHash.SetValue("CmdOpen", SYS_CMD_ITEM("Open", "Ctrl+O", "", "Open File"));
	m_xCmdMapHash.SetValue("CmdSave", SYS_CMD_ITEM("Save", "Ctrl+S", "", "Save File"));
	m_xCmdMapHash.SetValue("CmdCopy", SYS_CMD_ITEM("Copy Plate", "Ctrl+C", "", "Copy Plate"));
	m_xCmdMapHash.SetValue("Find", SYS_CMD_ITEM("Query Part", "Ctrl+F", "F", "Query Part"));
	m_xCmdMapHash.SetValue("CmdHole", SYS_CMD_ITEM("Show Or Hide All Holes", "F3", "", "Show Or Hide All Holes"));
	m_xCmdMapHash.SetValue("CmdShift", SYS_CMD_ITEM("Switch Display Modes", "F6", "", "Switch Display Modes"));
	m_xCmdMapHash.SetValue("CmdDaoWei", SYS_CMD_ITEM("In Place", "F7", "", "Whole Tower In Place"));
	m_xCmdMapHash.SetValue("CmdRefresh", SYS_CMD_ITEM("Refresh", "F8", "", "Refresh Screen"));
	m_xCmdMapHash.SetValue("CmdStartDraw", SYS_CMD_ITEM("Start Draw Program", "F10", "", "Start Drawing Program By Specify Path"));
	m_xCmdMapHash.SetValue("Delete", SYS_CMD_ITEM("Delete Part", "Delete", "Delete", "Delete Specify Part"));
	
#else
	m_xCmdMapHash.SetValue("NewTower", SYS_CMD_ITEM("新建", "Ctrl+N", "N", "新建文件"));
	m_xCmdMapHash.SetValue("CmdOpen", SYS_CMD_ITEM("打开", "Ctrl+O", "", "打开文件"));
	m_xCmdMapHash.SetValue("CmdSave", SYS_CMD_ITEM("保存", "Ctrl+S", "", "保存文件"));
	m_xCmdMapHash.SetValue("CmdCopy", SYS_CMD_ITEM("复制钢板", "Ctrl+C", "", "复制钢板"));
	m_xCmdMapHash.SetValue("Find", SYS_CMD_ITEM("查找构件", "Ctrl+F", "F", "查找构件"));
	m_xCmdMapHash.SetValue("CmdHole", SYS_CMD_ITEM("显示或隐藏所有孔", "F3", "", "显示或隐藏所有孔"));
	m_xCmdMapHash.SetValue("CmdShift", SYS_CMD_ITEM("切换显示模式", "F6", "", "切换显示模式"));
	m_xCmdMapHash.SetValue("CmdDaoWei", SYS_CMD_ITEM("到位", "F7", "", "全塔到位"));
	m_xCmdMapHash.SetValue("CmdRefresh", SYS_CMD_ITEM("刷新", "F8", "", "刷新屏幕"));
	m_xCmdMapHash.SetValue("CmdStartDraw", SYS_CMD_ITEM("启动绘图程序", "F10", "", "按指定路径启动绘图程序"));
	m_xCmdMapHash.SetValue("Delete", SYS_CMD_ITEM("删除构件", "Delete", "Delete", "删除指定构件"));
#endif
	//初始化命令行命令
	SYS_CMD_ITEM* pCmdItem = NULL;
	CLDSView *pLDSView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	for (CLDSView::SYS_CMD* pCMD = pLDSView->listSysCmds.GetFirst(); pCMD; pCMD = pLDSView->listSysCmds.GetNext())
	{
		if(m_xCmdMapHash.GetValue(pCMD->cmdStr))
			continue;	//已存在命令
		pCmdItem = m_xCmdMapHash.Add(pCMD->cmdStr);
		pCmdItem->m_sCmdName.Copy(pCMD->descript);
		pCmdItem->m_sDescript.Copy(pCMD->descript);
		if (strlen(pCMD->briefCmdStr) > 0)
			pCmdItem->m_sCmdStr.Copy(pCMD->briefCmdStr);
		else
			pCmdItem->m_sCmdStr.Copy(pCMD->cmdStr);
	}
}

BOOL CKeyboardMapDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	long col_wide_arr[4]={180,70,70,200};
	m_listKeyboardMap.InitListCtrl(col_wide_arr);
	//
	InitCmdMapItems();
	CStringArray str_arr;
	str_arr.SetSize(4);
	for (SYS_CMD_ITEM* pCmdItem = m_xCmdMapHash.GetFirst(); pCmdItem; pCmdItem = m_xCmdMapHash.GetNext())
	{
		str_arr[0].Format("%s", (char*)pCmdItem->m_sCmdName);
		str_arr[1].Format("%s", (char*)pCmdItem->m_sCmdStr);
		str_arr[2].Format("%s", (char*)pCmdItem->m_sShortCut);
		str_arr[3].Format("%s", (char*)pCmdItem->m_sDescript);
		m_listKeyboardMap.InsertItemRecord(-1, str_arr);
	}
	return TRUE;
}
