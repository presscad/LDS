#pragma once
#include "ShankDefCls.h"
#include "NewFileDlg.h"
#include "NewTowerDlg.h"
//
class CNewTowerDataOper
{
	BYTE m_ciMultiLegSchema;	//多接腿模式：1.8腿|2.16腿|3.24腿
	int m_nPoly;				//塔边数：3|4|5|6|8
	CShankDefine m_xShankDef;	//
	CFrameDefine m_xFrameDef;	//
public:
	static BOOL m_bSwitchMode;		//是否切换模式
	//
	BYTE m_ciRunUiMode;			//启用模式：0.经典模式|1.标准模式
	static const BYTE OLD_UI	=0;		//旧版界面操作(经典版)
	static const BYTE NEW_UI	=1;		//新版界面操作(标准版)
protected:
	void NewTowerModel();
	void TransShankDefToFrameDef();
	void TransFrameDefToShankDef();
public:
	CNewTowerDataOper(BYTE ciRunUiMode);
	//
	BOOL RunNewTowerUi();	//
};