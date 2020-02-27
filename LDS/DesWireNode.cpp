#include "stdafx.h"
#include "LDS.h"
#include "MainFrm.h"
#include "LDSDoc.h"
#include "DesWireNodeDlg.h"
#include "InputWirePointDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(__LDS_)||defined(__TSA_)

void CMainFrame::OnDesWireNode()
{
	CDesWireNodeDlg dlg;
	//CInputWirePointDlg dlg;
	if(dlg.DoModal()!=IDOK)
		return;
	//重新刷新界面
	GetLDSView()->RedrawAll();
}
#endif