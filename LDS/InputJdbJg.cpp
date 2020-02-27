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
	// 2.选择连接板第一面的各射线杆件
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select all rods which the node plate connect:","");
#else 
	pCmdLine->FillCmdLine("DesignNodePlate 请选择该节点板基本面所连接的所有杆件:","");
#endif
	g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_LINETUBE)|
		GetSingleWord(SELECTINDEX_LINEFLAT)|GetSingleWord(SELECTINDEX_LINESLOT));
	((CMainFrame*)AfxGetMainWnd())->GetLDSView()->OpenWndSel();
	if(pCmdLine->GetStrFromCmdLine(cmdStr))
	{
		//根据句柄字符串添加构件
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
				{	//选中任一根组合角钢的子角钢，都等效替换为选中组合角钢　wjh-2016.11.4
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
		pCmdLine->FillCmdLine("DesignNodePlate 请选择该节点板第二面所连接的所有射线杆件:","");
#endif
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			//根据句柄字符串添加构件
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
				//如果第二面杆件存在钢管
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
		pCmdLine->FillCmdLine("DesignNodePlate 请选择该节点板第三面所连接的所有射线杆件:","");
#endif
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			//根据句柄字符串添加构件
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
		throw "第一面至少应有一根杆件才有必要进行节点板设计";
#endif
	}
	if(face2_jgset.GetNodeNum()<1 && m_pPlank->face_N>1 && face2_tubeset.GetNodeNum()<1)
	{
		g_pSolidDraw->SetEntSnapStatus(m_pBaseNode->handle, FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		throw "Second plane should have at least one rod that the multiple panel design is necessary";
#else 
		throw "第二面至少应有一根杆件才有必要进行多面板设计";
#endif
	}
	if(partset.GetNodeNum()<1&&m_pPlank->face_N>2)
	{
		g_pSolidDraw->SetEntSnapStatus(m_pBaseNode->handle, FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		throw "Third plane should have at least one rod that the 3-plane design is necessary";
#else 
		throw "第三面至少应有一根杆件才有必要进行三面板设计";
#endif
	}
	
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(m_pBaseNode->handle);
	//基准杆件不是钢管且第二面存在射线杆件
	if(m_pBasePart->GetClassTypeId()!=CLS_LINETUBE&&face2_jgset.GetNodeNum()+face2_tubeset.GetNodeNum()>0) 
	{
		pSelLinePart=partset.GetFirst();
		if(pSelLinePart&&pSelLinePart->GetClassTypeId()==CLS_LINEANGLE
			&&partset.GetNodeNum()==1&&pSelLinePart->handle==m_pBasePart->handle)
		{	//基准面有且仅有一根基准角钢时无需用户输入火曲基准杆件 wht 10-07-08
			design_huoqu[0].pBaseJg = (CLDSLineAngle*)pSelLinePart;
			g_pSolidDraw->SetEntSnapStatus(pSelLinePart->handle,TRUE);
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select the 1st bending rod of node plate(It should be on first plane):","");
#else 
			pCmdLine->FillCmdLine("DesignNodePlate 请选择该节点板第一根火曲线基准杆件(应在第一面上):","");
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
				{	//点击鼠标左键默认装配定位基准角钢
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
		pCmdLine->FillCmdLine("DesignNodePlate 请选择该节点板第二根火曲线基准杆件(应在第一面上):","");
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
	//基准杆件为组合角钢时也需要选择端连接射线杆件(预设射线杆件位置时用) wht 11-01-17
	if(m_pBasePart->GetClassTypeId()==CLS_LINEANGLE||m_pBasePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr="Please select end connect ray rod（At most two）!";
#else 
		cmdStr="DesignNodePlate 请选择第一根端连接射线杆件，无端连接回车键跳过:";
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
					pCmdLine->FinishCmdLine(CXhChar16("角钢0x%X",pSelLinePart->handle));
					cmdStr="DesignNodePlate 请选择第二根端连接射线杆件，无端连接回车键跳过:";
					pCmdLine->FillCmdLine(cmdStr,"");
				}
			}
			else	//无选中端连接角钢，直接跳出
			{
				pCmdLine->FinishCmdLine("跳过");
				break;
			}
		}while(end_lj_jgset.GetNodeNum()<2);
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidSet->SetDisplayType(DISP_SOLID);//切换到实体显示模式
		g_pSolidDraw->Draw();
	}
	//此处为何使用基准节点的父杆件作为基准杆件，如果在此处更新，前边选择基准杆件的步骤即为冗余操作。 wht 10-07-13
	/** 我也觉得这段代码有点冗余，可能历史久远之前担心前面没赋值基准杆件所以遗留下来的 wjh-2016.9.4
	if(m_pBaseNode->m_cPosCalType!=4)
	{	//非交叉节点
		m_pBasePart=(CLDSLinePart*)console.FromPartHandle(m_pBaseNode->hFatherPart,CLS_LINEPART);
		if(m_pBasePart.IsHasPtr())
			m_pPlank->designInfo.m_hBasePart = m_pBasePart->handle;
		else
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Can't find legal node's parent node!";
#else 
			throw "未找到合法的节点父杆件!";
#endif
	}*/
	m_bRayEndFaceOffset=FALSE;
	for(CLDSLinePart *pLinePart=partset.GetFirst();pLinePart;pLinePart=partset.GetNext())
	{
		if(!pLinePart->IsAngle())
			continue;
		if(pLinePart->handle==m_pBasePart->handle)
			continue;	//基准杆件
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
	pCmdLine->FillCmdLine("命令:","");
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
		pCmdLine->FillCmdLine("DesignNodePlate 选择该节点板第一根火曲线基准杆件(应在第一面上且缺省为三面板设计基准):","");
#endif
		for(;;)
		{
			if(g_pSolidSnap->SnapLine(line_3d,TRUE)<=0)
				continue;
			else if(0)//用于处理所选杆件不在第一面上时的情况
			{
				//...未完成WJH--00.09.17
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
		pCmdLine->FillCmdLine("DesignNodePlate 请选择该节点板第二根火曲线基准杆件(应在第一面上):","");
#endif
		for(;;)
		{
			if(g_pSolidSnap->SnapLine(line_3d,TRUE)<=0)
				continue;
			else if(0)//用于处理所选杆件不在第一面上时的情况
			{
				//...未完成WJH--00.09.17
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
	pCmdLine->FillCmdLine("命令:","");
#endif
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(m_pPlank->designInfo.m_hBaseNode,TRUE);
}
//斜材端点单螺栓设计条件判断
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


	//条件1:两射线杆件节点面上的肢法线方向相反
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
	if(bInsideJg1==bInsideJg2)	//肢法线方向相同不能进行斜补材端点单螺栓设计
		return FALSE;
	//条件2:斜材杆件端点只有一个螺栓
	int start0_end1_else2=LjPosInPart(pRayJg1);
	if(start0_end1_else2==0&&pRayJg1->connectStart.wnConnBoltN!=1)	//始端连接
		return FALSE;
	else if(start0_end1_else2==1&&pRayJg1->connectEnd.wnConnBoltN!=1)//终端连接
		return FALSE;
	else if(start0_end1_else2==2)
		return FALSE;
	double len_offset1=0,len_offset2=0;
	if(start0_end1_else2==0)
		len_offset1=pRayJg1->desStartPos.len_offset_dist;
	else
		len_offset1=pRayJg1->desStartPos.len_offset_dist;

	start0_end1_else2=LjPosInPart(pRayJg2);
	if(start0_end1_else2==0&&pRayJg2->connectStart.wnConnBoltN!=1)	//始端连接
		return FALSE;
	else if(start0_end1_else2==1&&pRayJg2->connectEnd.wnConnBoltN!=1)//终端连接
		return FALSE;
	else if(start0_end1_else2==2)
		return FALSE;
	if(start0_end1_else2==0)
		len_offset2=pRayJg2->desStartPos.len_offset_dist;
	else
		len_offset2=pRayJg2->desStartPos.len_offset_dist;
	//沿基准杆件长度方向偏移量相同才可设计单螺栓，否则有可能是偏移开的两个螺栓连接
	return len_offset1-len_offset2<EPS;
}
#endif