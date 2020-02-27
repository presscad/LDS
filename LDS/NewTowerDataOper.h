#pragma once
#include "ShankDefCls.h"
#include "NewFileDlg.h"
#include "NewTowerDlg.h"
//
class CNewTowerDataOper
{
	BYTE m_ciMultiLegSchema;	//�����ģʽ��1.8��|2.16��|3.24��
	int m_nPoly;				//��������3|4|5|6|8
	CShankDefine m_xShankDef;	//
	CFrameDefine m_xFrameDef;	//
public:
	static BOOL m_bSwitchMode;		//�Ƿ��л�ģʽ
	//
	BYTE m_ciRunUiMode;			//����ģʽ��0.����ģʽ|1.��׼ģʽ
	static const BYTE OLD_UI	=0;		//�ɰ�������(�����)
	static const BYTE NEW_UI	=1;		//�°�������(��׼��)
protected:
	void NewTowerModel();
	void TransShankDefToFrameDef();
	void TransFrameDefToShankDef();
public:
	CNewTowerDataOper(BYTE ciRunUiMode);
	//
	BOOL RunNewTowerUi();	//
};