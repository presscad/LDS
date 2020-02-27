//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "MainFrm.h"
#include "Tower.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "dlg.h"
#include "PromptDlg.h"
#include "DesignJdb.h"
#include "SnapTypeVerify.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

BOOL CDesignJdb::InputJdbLinePart()
{
	CString cmdStr;
	f3dLine line_3d;
	char sText[MAX_TEMP_CHAR_100+1];
	CSuperSmartPtr<CLDSPart>pPart;
	CLDSLinePart* pSelLinePart;
	CHashTable<CLDSLinePart*>partsetTable;
	partsetTable.CreateHashTable(100);
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	m_pBaseNode = console.FromNodeHandle(m_pPlank->designInfo.m_hBaseNode);
	m_pBasePart = console.FromPartHandle(m_pPlank->designInfo.m_hBasePart);
	if(m_pBasePart.IsHasPtr()&&!m_pBasePart->IsLinePart())
	{
		_delegateAngle.Fabricate(m_pBasePart,m_pBaseNode->Position());
		m_pBasePart=&_delegateAngle;
	}
	if(m_pBaseRod.IsHasPtr())
	{
		partset.append(m_pBaseRod);
		partsetTable.SetValueAt(m_pBaseRod->handle,m_pBaseRod);
	}
	// 2.ѡ�����Ӱ��һ��ĸ����߸˼�
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select all rods which the node plate connect:","");
#else 
	pCmdLine->FillCmdLine("DesignNodePlate ��ѡ��ýڵ������������ӵ����и˼�:","");
#endif
	g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_LINETUBE)|
		GetSingleWord(SELECTINDEX_LINEFLAT)|GetSingleWord(SELECTINDEX_LINESLOT));
	((CMainFrame*)AfxGetMainWnd())->GetLDSView()->OpenWndSel();
	if(pCmdLine->GetStrFromCmdLine(cmdStr))
	{
		//���ݾ���ַ�����ӹ���
		long *id_arr1=NULL,arr1_len=0;;
		if(cmdStr.GetLength()>0)
		{
			id_arr1=new long[cmdStr.GetLength()];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%s",cmdStr);
			for(char* key=strtok(sText,",");key;key=strtok(NULL,","))
			{
				long h;
				sscanf(key,"%X",&h);
				pPart=console.FromPartHandle(h);
				id_arr1[arr1_len]=h;
				arr1_len++;
			}
		}
		long *id_arr;
		int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
		for(int i=0;i<arr1_len+n;i++)
		{
			if(i<arr1_len)
				pPart=console.FromPartHandle(id_arr1[i]);
			else
				pPart=console.FromPartHandle(id_arr[i-arr1_len]);
			if(pPart.IsHasPtr()&&pPart->IsLinePart()&&
				pPart.LinePartPointer()->pStart!=NULL&&pPart.LinePartPointer()->pEnd!=NULL)
			{
				CLDSGroupLineAngle* pGroupAngle=0;
				if(pPart->IsAngle()&&pPart.LineAnglePointer()->group_father_jg_h>0x20)
					pGroupAngle=(CLDSGroupLineAngle*)Ta.Parts.FromHandle(pPart.LineAnglePointer()->group_father_jg_h,CLS_GROUPLINEANGLE);
				if(!partsetTable.GetValueAt(pPart->handle,pSelLinePart)&&(pGroupAngle==NULL||!partsetTable.GetValueAt(pGroupAngle->handle,pSelLinePart)))
				{	//ѡ����һ����ϽǸֵ��ӽǸ֣�����Ч�滻Ϊѡ����ϽǸ֡�wjh-2016.11.4
					if(pGroupAngle!=NULL)
						pPart=pGroupAngle;
					partsetTable.SetValueAt(pPart->handle,pPart.LinePartPointer());
					partset.append(pPart.LinePartPointer());
				}
			}
		}
		if(id_arr1)
			delete []id_arr1;
		g_pSolidDraw->ReleaseSnapStatus();
	}
	else
	{
		pCmdLine->CancelCmdLine();
		return FALSE;
	}
	if(m_pPlank->face_N>1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select all ray rods which the node plate's second plane connect:","");
#else 
		pCmdLine->FillCmdLine("DesignNodePlate ��ѡ��ýڵ��ڶ��������ӵ��������߸˼�:","");
#endif
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			//���ݾ���ַ�����ӹ���
			long *id_arr1=NULL,arr1_len=0;;
			if(cmdStr.GetLength()>0)
			{
				id_arr1=new long[cmdStr.GetLength()];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%s",cmdStr);
				for(char* key=strtok(sText,",");key;key=strtok(NULL,","))
				{
					long h;
					sscanf(key,"%X",&h);
					pPart=console.FromPartHandle(h);
					id_arr1[arr1_len]=h;
					arr1_len++;
				}
			}
			long *id_arr;
			int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
			for(int i=0;i<arr1_len+n;i++)
			{
				if(i<arr1_len)
					pPart=console.FromPartHandle(id_arr1[i]);
				else
					pPart=console.FromPartHandle(id_arr[i-arr1_len]);
				if(pPart.IsHasPtr()&&(pPart->GetClassTypeId()==CLS_LINEANGLE||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)&&
					pPart.LinePartPointer()->pStart!=NULL&&pPart.LinePartPointer()->pEnd!=NULL)
				{
					long hGroupFatherAngle=0;
					if(pPart->IsAngle())
						hGroupFatherAngle=pPart.LineAnglePointer()->group_father_jg_h;
					if(!partsetTable.GetValueAt(pPart->handle,pSelLinePart)&&(hGroupFatherAngle==0||!partsetTable.GetValueAt(hGroupFatherAngle,pSelLinePart)))
					{		
						partsetTable.SetValueAt(pPart->handle,pPart.LinePartPointer());
						face2_jgset.append(pPart.LineAnglePointer());
					}
				}
				//����ڶ���˼����ڸֹ�
				else if(pPart.IsHasPtr()&&pPart->GetClassTypeId()==CLS_LINETUBE)
				{
					if(!partsetTable.GetValueAt(pPart->handle,pSelLinePart))
					{		
						partsetTable.SetValueAt(pPart->handle,pPart.LineTubePointer());
						face2_tubeset.append(pPart.LineTubePointer());
					}
				}
			}
			if(id_arr1)
				delete []id_arr1;
			g_pSolidDraw->ReleaseSnapStatus();
		}
		else
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
	}
	if(m_pPlank->face_N>2)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select all ray rods which the node plate's third plane connect:","");
#else 
		pCmdLine->FillCmdLine("DesignNodePlate ��ѡ��ýڵ������������ӵ��������߸˼�:","");
#endif
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			//���ݾ���ַ�����ӹ���
			long *id_arr1=NULL,arr1_len=0;;
			if(cmdStr.GetLength()>0)
			{
				id_arr1=new long[cmdStr.GetLength()];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%s",cmdStr);
				for(char* key=strtok(sText,",");key;key=strtok(NULL,","))
				{
					long h;
					sscanf(key,"%X",&h);
					pPart=console.FromPartHandle(h);
					id_arr1[arr1_len]=h;
					arr1_len++;
				}
			}
			long *id_arr;
			int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
			for(int i=0;i<arr1_len+n;i++)
			{
				if(i<arr1_len)
					pPart=console.FromPartHandle(id_arr1[i]);
				else
					pPart=console.FromPartHandle(id_arr[i-arr1_len]);
				if(pPart.IsHasPtr()&&pPart->GetClassTypeId()==CLS_LINEANGLE&&
					pPart.LinePartPointer()->pStart!=NULL&&pPart.LinePartPointer()->pEnd!=NULL)
				{
					long hGroupFatherAngle=0;
					if(pPart->IsAngle())
						hGroupFatherAngle=pPart.LineAnglePointer()->group_father_jg_h;
					if(!partsetTable.GetValueAt(pPart->handle,pSelLinePart)&&(hGroupFatherAngle==0||!partsetTable.GetValueAt(hGroupFatherAngle,pSelLinePart)))
					{		
						partsetTable.SetValueAt(pPart->handle,pPart.LinePartPointer());
						face3_jgset.append(pPart.LineAnglePointer());
					}
				}
			}
			if(id_arr1)
				delete []id_arr1;
			g_pSolidDraw->ReleaseSnapStatus();
		}
		else
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
	}
	((CMainFrame*)AfxGetMainWnd())->GetLDSView()->OperOther();
	g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	if((partset.GetNodeNum()<1&&m_pPlank->face_N==1)||partset.GetNodeNum()<1)
	{
		g_pSolidDraw->SetEntSnapStatus(m_pBaseNode->handle, FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		throw "First plane should have at least one rod that the node plane design is necessary";
#else 
		throw "��һ������Ӧ��һ���˼����б�Ҫ���нڵ�����";
#endif
	}
	if(face2_jgset.GetNodeNum()<1 && m_pPlank->face_N>1 && face2_tubeset.GetNodeNum()<1)
	{
		g_pSolidDraw->SetEntSnapStatus(m_pBaseNode->handle, FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		throw "Second plane should have at least one rod that the multiple panel design is necessary";
#else 
		throw "�ڶ�������Ӧ��һ���˼����б�Ҫ���ж�������";
#endif
	}
	if(partset.GetNodeNum()<1&&m_pPlank->face_N>2)
	{
		g_pSolidDraw->SetEntSnapStatus(m_pBaseNode->handle, FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		throw "Third plane should have at least one rod that the 3-plane design is necessary";
#else 
		throw "����������Ӧ��һ���˼����б�Ҫ������������";
#endif
	}
	
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(m_pBaseNode->handle);
	//��׼�˼����Ǹֹ��ҵڶ���������߸˼�
	if(m_pBasePart->GetClassTypeId()!=CLS_LINETUBE&&face2_jgset.GetNodeNum()+face2_tubeset.GetNodeNum()>0) 
	{
		pSelLinePart=partset.GetFirst();
		if(pSelLinePart&&pSelLinePart->GetClassTypeId()==CLS_LINEANGLE
			&&partset.GetNodeNum()==1&&pSelLinePart->handle==m_pBasePart->handle)
		{	//��׼�����ҽ���һ����׼�Ǹ�ʱ�����û����������׼�˼� wht 10-07-08
			design_huoqu[0].pBaseJg = (CLDSLineAngle*)pSelLinePart;
			g_pSolidDraw->SetEntSnapStatus(pSelLinePart->handle,TRUE);
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select the 1st bending rod of node plate(It should be on first plane):","");
#else 
			pCmdLine->FillCmdLine("DesignNodePlate ��ѡ��ýڵ���һ�������߻�׼�˼�(Ӧ�ڵ�һ����):","");
#endif
			CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART);
			verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prLine);
			DWORD dwhObj=0,dwExportFlag=0;
			for(;;)
			{
				if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
				{
					pCmdLine->CancelCmdLine();
					return FALSE;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				dwhObj=obj.hRelaObj;
				if(dwhObj==0&&obj.ciTriggerType==SELOBJ::TRIGGER_LBUTTON && m_pBasePart->IsAngle())
				{	//���������Ĭ��װ�䶨λ��׼�Ǹ�
					design_huoqu[0].pBaseJg=m_pBasePart.pAngle;
					break;
				}
				if(dwhObj==0&&obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
				{
					long* id_arr=NULL;
					if(g_pSolidSnap->GetLastSelectEnts(id_arr)>0)
						dwhObj=id_arr[0];
				}
				pSelLinePart=(CLDSLinePart*)console.FromPartHandle(dwhObj);
				if(pSelLinePart&&pSelLinePart->IsAngle())
				{
					design_huoqu[0].pBaseJg = (CLDSLineAngle*)pSelLinePart;
					break;
				}	
			}
			if(design_huoqu[0].pBaseJg)
			{
				pCmdLine->FinishCmdLine(CXhChar16("0x%X",design_huoqu[0].pBaseJg->handle));
				g_pSolidDraw->SetEntSnapStatus(design_huoqu[0].pBaseJg->handle,TRUE);
			}
		}
	}
	if(face3_jgset.GetNodeNum()>0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select the 2nd bending rod of node plate(It should be on first plane):","");
#else 
		pCmdLine->FillCmdLine("DesignNodePlate ��ѡ��ýڵ��ڶ��������߻�׼�˼�(Ӧ�ڵ�һ����):","");
#endif
		CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART);
		verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prLine);
		DWORD dwhObj=0,dwExportFlag=0;
		for(;;)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;
			if(dwhObj==0&&obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
			{
				long* id_arr=NULL;
				if(g_pSolidSnap->GetLastSelectEnts(id_arr)>0)
					dwhObj=id_arr[0];
			}
			pSelLinePart=(CLDSLinePart*)console.FromPartHandle(dwhObj);
			if(pSelLinePart&&pSelLinePart->IsAngle())
			{
				design_huoqu[1].pBaseJg = (CLDSLineAngle*)pSelLinePart;
				break;
			}
		}
		if(design_huoqu[1].pBaseJg)
		{
			pCmdLine->FinishCmdLine(CXhChar16("0x%X",design_huoqu[1].pBaseJg->handle));
			g_pSolidDraw->SetEntSnapStatus(design_huoqu[1].pBaseJg->handle,TRUE);
		}
	}
	//��׼�˼�Ϊ��ϽǸ�ʱҲ��Ҫѡ����������߸˼�(Ԥ�����߸˼�λ��ʱ��) wht 11-01-17
	if(m_pBasePart->GetClassTypeId()==CLS_LINEANGLE||m_pBasePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr="Please select end connect ray rod��At most two��!";
#else 
		cmdStr="DesignNodePlate ��ѡ���һ�����������߸˼����޶����ӻس�������:";
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		DWORD dwhObj=0,dwExportFlag=0;
		CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART);
		verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prLine);
		do
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;
			if(dwhObj>0x20)
			{
				pSelLinePart=(CLDSLinePart*)console.FromPartHandle(dwhObj);
				if(pSelLinePart && pSelLinePart->IsAngle())
				{
					g_pSolidDraw->ReleaseSnapStatus();
					g_pSolidDraw->SetEntSnapStatus(dwhObj,TRUE);
					end_lj_jgset.append((CLDSLineAngle*)pSelLinePart);
					pCmdLine->FinishCmdLine(CXhChar16("�Ǹ�0x%X",pSelLinePart->handle));
					cmdStr="DesignNodePlate ��ѡ��ڶ������������߸˼����޶����ӻس�������:";
					pCmdLine->FillCmdLine(cmdStr,"");
				}
			}
			else	//��ѡ�ж����ӽǸ֣�ֱ������
			{
				pCmdLine->FinishCmdLine("����");
				break;
			}
		}while(end_lj_jgset.GetNodeNum()<2);
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidSet->SetDisplayType(DISP_SOLID);//�л���ʵ����ʾģʽ
		g_pSolidDraw->Draw();
	}
	//�˴�Ϊ��ʹ�û�׼�ڵ�ĸ��˼���Ϊ��׼�˼�������ڴ˴����£�ǰ��ѡ���׼�˼��Ĳ��輴Ϊ��������� wht 10-07-13
	/** ��Ҳ������δ����е����࣬������ʷ��Զ֮ǰ����ǰ��û��ֵ��׼�˼��������������� wjh-2016.9.4
	if(m_pBaseNode->m_cPosCalType!=4)
	{	//�ǽ���ڵ�
		m_pBasePart=(CLDSLinePart*)console.FromPartHandle(m_pBaseNode->hFatherPart,CLS_LINEPART);
		if(m_pBasePart.IsHasPtr())
			m_pPlank->designInfo.m_hBasePart = m_pBasePart->handle;
		else
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Can't find legal node's parent node!";
#else 
			throw "δ�ҵ��Ϸ��Ľڵ㸸�˼�!";
#endif
	}*/
	m_bRayEndFaceOffset=FALSE;
	for(CLDSLinePart *pLinePart=partset.GetFirst();pLinePart;pLinePart=partset.GetNext())
	{
		if(!pLinePart->IsAngle())
			continue;
		if(pLinePart->handle==m_pBasePart->handle)
			continue;	//��׼�˼�
		if(pLinePart->pStart==m_pBaseNode && ((CLDSLineAngle*)pLinePart)->desStartPos.IsFaceOffset())
		{
			m_bRayEndFaceOffset=TRUE;
			break;
		}
		else if(pLinePart->pEnd==m_pBaseNode && ((CLDSLineAngle*)pLinePart)->desEndPos.IsFaceOffset())
		{
			m_bRayEndFaceOffset=TRUE;
			break;
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("����:","");
#endif
	return TRUE;
}

void CDesignJdb::SetFeatureJg()
{
	f3dLine line_3d;
	CLDSLineAngle *pSelLinePart;
	CLDSNode *pBaseNode = console.FromNodeHandle(m_pPlank->designInfo.m_hBaseNode);
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(face2_jgset.GetNodeNum()>0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select the 1st bending rod of node plate(It should be on first plane and the default is three panel design)!","");
#else 
		pCmdLine->FillCmdLine("DesignNodePlate ѡ��ýڵ���һ�������߻�׼�˼�(Ӧ�ڵ�һ������ȱʡΪ�������ƻ�׼):","");
#endif
		for(;;)
		{
			if(g_pSolidSnap->SnapLine(line_3d,TRUE)<=0)
				continue;
			else if(0)//���ڴ�����ѡ�˼����ڵ�һ����ʱ�����
			{
				//...δ���WJH--00.09.17
			}
			else
			{
				pSelLinePart = (CLDSLineAngle*)console.FromPartHandle(line_3d.ID,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				design_huoqu[0].pBaseJg = pSelLinePart;
				g_pSolidDraw->SetEntSnapStatus(line_3d.ID,TRUE);
				break;
			}
		}
		if(m_pPlank->face_N==3)
			m_pPlank->designInfo.m_hBasePart  = pSelLinePart->handle;
	}
	if(face3_jgset.GetNodeNum()>0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select the 2nd bending rod of node plate(It should be on first plane)!","");
#else 
		pCmdLine->FillCmdLine("DesignNodePlate ��ѡ��ýڵ��ڶ��������߻�׼�˼�(Ӧ�ڵ�һ����):","");
#endif
		for(;;)
		{
			if(g_pSolidSnap->SnapLine(line_3d,TRUE)<=0)
				continue;
			else if(0)//���ڴ�����ѡ�˼����ڵ�һ����ʱ�����
			{
				//...δ���WJH--00.09.17
			}
			else
			{
				pSelLinePart = (CLDSLineAngle*)console.FromPartHandle(line_3d.ID,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				design_huoqu[1].pBaseJg = pSelLinePart;
				break;
			}
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("����:","");
#endif
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(m_pPlank->designInfo.m_hBaseNode,TRUE);
}
//б�Ķ˵㵥��˨��������ж�
BOOL CDesignJdb::IsOneLsDesignValid()
{
	f3dPoint vec1,vec2,norm;
	BOOL bInsideJg1,bInsideJg2;	
	CLDSLineAngle *pRayJg1=NULL,*pRayJg2=NULL;
	if(m_pPlank->face_N!=1)
		return FALSE;
	if(partset.GetNodeNum()!=3)
		return FALSE;
	for(CLDSLinePart *pLinePart=partset.GetFirst();pLinePart!=NULL;pLinePart=partset.GetNext())
	{
		if(pLinePart->GetClassTypeId()!=CLS_LINEANGLE)
			return FALSE;
	}


	//����1:�����߸˼��ڵ����ϵ�֫���߷����෴
	if(partset[0]!=m_pBasePart)
		pRayJg1=(CLDSLineAngle*)partset[0];
	if(partset[1]!=m_pBasePart)
	{
		if(pRayJg1==NULL)
			pRayJg1=(CLDSLineAngle*)partset[1];
		else
			pRayJg2=(CLDSLineAngle*)partset[1];
	}
	if(partset[2]!=m_pBasePart)
	{
		if(pRayJg1==NULL)
			return FALSE;
		else if(pRayJg2)
			return FALSE;
		else
			pRayJg2=(CLDSLineAngle*)partset[2];
	}
	bInsideJg1 = IsInsideJg(pRayJg1,(BYTE*)NULL);
	bInsideJg2 = IsInsideJg(pRayJg2,(BYTE*)NULL);
	if(bInsideJg1==bInsideJg2)	//֫���߷�����ͬ���ܽ���б���Ķ˵㵥��˨���
		return FALSE;
	//����2:б�ĸ˼��˵�ֻ��һ����˨
	int start0_end1_else2=LjPosInPart(pRayJg1);
	if(start0_end1_else2==0&&pRayJg1->connectStart.wnConnBoltN!=1)	//ʼ������
		return FALSE;
	else if(start0_end1_else2==1&&pRayJg1->connectEnd.wnConnBoltN!=1)//�ն�����
		return FALSE;
	else if(start0_end1_else2==2)
		return FALSE;
	double len_offset1=0,len_offset2=0;
	if(start0_end1_else2==0)
		len_offset1=pRayJg1->desStartPos.len_offset_dist;
	else
		len_offset1=pRayJg1->desStartPos.len_offset_dist;

	start0_end1_else2=LjPosInPart(pRayJg2);
	if(start0_end1_else2==0&&pRayJg2->connectStart.wnConnBoltN!=1)	//ʼ������
		return FALSE;
	else if(start0_end1_else2==1&&pRayJg2->connectEnd.wnConnBoltN!=1)//�ն�����
		return FALSE;
	else if(start0_end1_else2==2)
		return FALSE;
	if(start0_end1_else2==0)
		len_offset2=pRayJg2->desStartPos.len_offset_dist;
	else
		len_offset2=pRayJg2->desStartPos.len_offset_dist;
	//�ػ�׼�˼����ȷ���ƫ������ͬ�ſ���Ƶ���˨�������п�����ƫ�ƿ���������˨����
	return len_offset1-len_offset2<EPS;
}
#endif