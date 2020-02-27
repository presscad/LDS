//<LOCALE_TRANSLATE/>
//DrawMapScript.cpp ��ͼ�ű�

#include "stdafx.h"
#include "stdarx.h"
#include "MenuFunc.h"
#include "DrawMapScriptDlg.h"
#include "Drawing.h"
#include "DrawTaMap.h"
#include "SelCurFaceDlg.h"
#include "SysPara.h"
#include "InputDrawingUcsDlg.h"
#include "DrawUnit.h"
#include "SortFunc.h"
#include "DragEntSet.h"
#include "AdsTools.h"
#include "DimPlankLs.h"
#include "GlobalFunc.h"
#include "SelectDrawingDlg.h"
#include "Drawing.h"
#include "ScrTools.h"
#include "ImportScriptOptionDlg.h"

#ifdef __SMART_DRAW_INC_
//
BOOL FireRunCommand(CDialog *pDlg)
{
	CDrawMapScriptDlg *pDrawDlg=(CDrawMapScriptDlg*)pDlg;
	const int nCmdDlgWidth=100;	//�������
	if(pDrawDlg->GetOperType()==0)
	{	//ִ������ϵ㴦ֹͣ
		if(pDrawDlg->m_pComponent)
		{
			if(pDrawDlg->m_pComponent->m_bFirstCreate)
			{	//��һ�����ɣ���Ҫ�Զ�������ͼװ��λ��
				pDrawDlg->m_pComponent->CalInitAssemblyPosition();
				pDrawDlg->m_pComponent->m_bFirstCreate=FALSE;
			}
			//ʹ�����λ�����¼�������ϵԭ��λ��
			//pDrawDlg->m_pComponent->CalAssemblyPosByRelativePos();	
		}
		for(int j=0;j<5;j++)
		{
			DRAGSET.ClearEntSet();
			int nItemCount=pDrawDlg->m_listCmdSuperCtrl.GetRootCount();
			for(int i=0;i<nItemCount;i++)
			{
				CSuperGridCtrl::CTreeItem *pItem=pDrawDlg->m_listCmdSuperCtrl.GetRootItem(i);
				if(pItem==NULL)
					continue;
				DWORD dw=GetSingleWord(j+1);
				if(pItem->m_dwPropGroup!=GetSingleWord(1+j))
					continue;	//�����ڵ�ǰ����
				CDrawingCommand *pCmd=(CDrawingCommand*)pItem->m_idProp;
				if(pCmd==NULL)
					continue;
				if(pCmd&&pCmd->m_stateCmd&CMD_STATE_NEXTPOS)	//ȥ�������� ��һ�� ��־
					pCmd->m_stateCmd^=CMD_STATE_NEXTPOS;
				if(!pCmd->IsNeedExecute())
					continue;	//�������Ҫִ��
				if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY&&pCmd->m_paraSimPoly.nSimPolyCmdType==1)
					continue;	//������������ֶ����������
				BOOL bDelete=FALSE;
				if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY&&pCmd->m_paraSimPoly.nSimPolyCmdType==0)
					bDelete=TRUE;	//��������������������α�ע����ע��ɺ�������ɾ��
				pCmd->Run();
				if(bDelete)
				{	//ɾ������������ע�����ε�����
					ATOM_LIST<CDrawingCommand> *pCmdList=NULL;
					if(pCmd->m_pDrawingCom)
						pCmdList=&pCmd->m_pDrawingCom->cmdList;
					if(pCmdList)
					{
						CDrawingCommand *pTempCmd=NULL;
						for(pTempCmd=pCmdList->GetFirst();pTempCmd;pTempCmd=pCmdList->GetNext())
						{
							if(pTempCmd!=pCmd)
								continue;
							pCmdList->DeleteCursor();
							break;
						}
					}
				}
				pCmd->SetCmdFlag(TRUE);
				//���ƹ���ʱ���Ŵ�����ʾ����ʵ�壬��עʱ����Ҫ����acedSSGet��������ʧ��
				if(j==0)
				{	//���Ŵ�����ʾ����ʵ��
					ads_point L_T,R_B;
					DRAGSET.GetDragEntSetScope(L_T,R_B);
					R_B[X]+=nCmdDlgWidth;	//�������ʾ���Ҳ࣬�˴��������½�X����
				#ifdef _ARX_2007
					acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#else
					acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#endif
				}
				//if(j==1)
				//	acedCommand(RTSTR,"REGEN",RTNONE);
				//��������Ϊ����״̬
				if((i+1)<nItemCount)
				{
					CDrawingCommand *pNextCmd=NULL;
					CSuperGridCtrl::CTreeItem *pNextItem=pDrawDlg->m_listCmdSuperCtrl.GetRootItem(i+1);
					if(pNextItem)	
						pNextCmd=(CDrawingCommand*)pNextItem->m_idProp;
					if(pNextCmd)
					{
						pNextCmd->m_stateCmd|=CMD_STATE_NEXTPOS;
						if(pNextCmd->m_stateCmd&CMD_STATE_BREAKPOINT)
							return TRUE;
					}
				}
			}
			if(pDrawDlg->m_pComponent&&(pDrawDlg->m_pComponent->m_eClassType==DRAW_COM_LINEVIEW
				||pDrawDlg->m_pComponent->m_eClassType==DRAW_COM_STRUVIEW)&&j==0)
			{
				pDrawDlg->m_pComponent->SpecifyAssemblyPosition();
			#ifdef _ARX_2007
				ads_command(RTSTR,L"RE",RTNONE);
				acedCommand(RTSTR,L"zoom",RTSTR,L"a",RTNONE);
			#else
				ads_command(RTSTR,"RE",RTNONE);
				acedCommand(RTSTR,"zoom",RTSTR,"a",RTNONE);
			#endif
			}
		}
	}
	else if(pDrawDlg->GetOperType()==1)
	{	//����ִ��
		CSuperGridCtrl::CTreeItem *pItem=NULL,*pNextItem=NULL;
		pItem=pDrawDlg->GetFirstExecutableItem();	//��õ�һ����ִ����
		CDrawingCommand *pCmd=NULL,*pNextCmd=NULL;
		if(pItem)
		{
			pCmd=(CDrawingCommand*)pItem->m_idProp;
			if(pCmd)
			{
				if(pCmd->Run()==0)
					return FALSE;
				pCmd->SetCmdFlag(TRUE);
				//���Ŵ���ͻ����ʾ����ӵ�ʵ��
				ads_point L_T,R_B;
				DRAGSET.GetDragEntSetScope(L_T,R_B);
				R_B[X]+=nCmdDlgWidth;	//�������ʾ���Ҳ࣬�˴��������½�X����
			#ifdef _ARX_2007
				acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
			#else
				acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
			#endif
				if(pCmd->m_stateCmd&CMD_STATE_NEXTPOS)
				{
					pCmd->m_stateCmd^=CMD_STATE_NEXTPOS;
					//SuperGridCtrl ����ͼ���ǵ�����ͼ�겻��������ʾ �˴��������´��� wht 10-11-30
					if(pCmd->m_stateCmd<2)
						pItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd);	//����������״̬
					else 
						pItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd+1);	//����������״̬
				}
			}
			pNextItem=pDrawDlg->m_listCmdSuperCtrl.GetRootItem(pItem->GetIndex()+1);
			if(pNextItem)
				pNextCmd=(CDrawingCommand*)pNextItem->m_idProp;
			if(pNextCmd&&!(pNextCmd->m_stateCmd&CMD_STATE_NEXTPOS))
			{
				pNextCmd->m_stateCmd|=CMD_STATE_NEXTPOS;
				//SuperGridCtrl ����ͼ���ǵ�����ͼ�겻��������ʾ �˴��������´��� wht 10-11-30
				if(pNextCmd->m_stateCmd<2)
					pNextItem->m_lpNodeInfo->SetImage(pNextCmd->m_stateCmd);	//����������״̬
				else 
					pNextItem->m_lpNodeInfo->SetImage(pNextCmd->m_stateCmd+1);	//����������״̬
				pDrawDlg->m_listCmdSuperCtrl.SelectItem(pNextItem);
				if(pNextItem->m_dwPropGroup!=GetSingleWord(pDrawDlg->m_iOldGroup+1))
				{	//�����в����ڵ�ǰ�����л����¸�����
					int i=0;
					for(i=1;i<6;i++)
					{
						if(pItem->m_dwPropGroup==GetSingleWord(i))
							break;
					}
					if(i>0)
					{
						pDrawDlg->m_tabCmdType.SetCurSel(i);
						pDrawDlg->RefreshCommandList(i);
					}
				}
			}
		}
		return TRUE;
	}
	else if(pDrawDlg->GetOperType()==2)
	{	//����һ��
		CSuperGridCtrl::CTreeItem *pItem=NULL,*pNextItem=NULL;
		pItem=pDrawDlg->GetLastExecutedItem();	//������һ����ִ�е�������
		CDrawingCommand *pCmd=NULL,*pNextCmd=NULL;
		if(pItem)
		{
			pCmd=(CDrawingCommand*)pItem->m_idProp;
			if(pCmd)
			{
				pCmd->EmptyEntList();	//ɾ���ɸ��������ɵ�ʵ��
				//������ʾ����ʵ��
				ads_point L_T,R_B;
				DRAGSET.GetDragEntSetScope(L_T,R_B);
			#ifdef _ARX_2007
				acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
			#else
				acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
			#endif
				//�����ϸ�����״̬Ϊδִ��
				pCmd->SetCmdFlag(FALSE);
				if(!(pCmd->m_stateCmd&CMD_STATE_NEXTPOS))
				{
					pCmd->m_stateCmd|=CMD_STATE_NEXTPOS;
					//SuperGridCtrl ����ͼ���ǵ�����ͼ�겻��������ʾ �˴��������´��� wht 10-11-30
					if(pCmd->m_stateCmd<2)
						pItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd);	//����������״̬
					else 
						pItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd+1);	//����������״̬
				}
				pDrawDlg->m_listCmdSuperCtrl.SelectItem(pItem);
			}
			pNextItem=pDrawDlg->m_listCmdSuperCtrl.GetRootItem(pItem->GetIndex()+1);
			if(pNextItem)
				pNextCmd=(CDrawingCommand*)pNextItem->m_idProp;
			if(pNextCmd&&(pNextCmd->m_stateCmd&CMD_STATE_NEXTPOS))
			{
				pNextCmd->m_stateCmd^=CMD_STATE_NEXTPOS;
				//SuperGridCtrl ����ͼ���ǵ�����ͼ�겻��������ʾ �˴��������´��� wht 10-11-30
				if(pNextCmd->m_stateCmd<2)
					pNextItem->m_lpNodeInfo->SetImage(pNextCmd->m_stateCmd);	//����������״̬
				else 
					pNextItem->m_lpNodeInfo->SetImage(pNextCmd->m_stateCmd+1);	//����������״̬
			}
		}
		return TRUE;
	}
	else if(pDrawDlg->GetOperType()==5)
	{	//���������
		
	}
	return TRUE;
}

void InitDrawingList();
CSelectDrawingDlg drawingDlg;
void SmartDraw()
{
	CAcModuleResourceOverride resOverride;
	if(drawingDlg.DoModal()!=IDOK)
		return;
	if(drawingDlg.m_pDrawing==NULL)
		return;
	//0.��׼���� 1.��ͼ 2.˾��ͼ 3.�ṹͼ 4.����ͼ 5.�麸ͼ
	if(drawingDlg.m_pDrawing->m_nSegI.iSeg>0)
		sys.fonts.InitSpecialProp(3);
	else if(drawingDlg.m_pDrawing->m_nSegI.iSeg==-1)
		sys.fonts.InitSpecialProp(2);
	else if(drawingDlg.m_pDrawing->m_nSegI.iSeg==-2)
		sys.fonts.InitSpecialProp(1);
	else
		sys.fonts.InitSpecialProp(0);

	//��¼�˼�ʼ�ն�������
	double fOldStartOutSpreadLen=sys.stru_map.fPoleAxisLineStartOutSpreadLen;
	double fOldEndOutSpreadLen=sys.stru_map.fPoleAxisLineEndOutSpreadLen;
	//����ͼ��
	drawingDlg.m_pDrawing->InsertMapFrm();
	//
	DRAGSET.ClearEntSet();
	CDrawMapScriptDlg scriptDlg;
	scriptDlg.SetCallBackFunc(FireRunCommand);
	dimpartspec.EmptyPartSpecDim();	//��չ�������ע�б� wht 11-06-19
	if(drawingDlg.m_pDrawing->m_nSegI.iSeg==-1)	//˾��ͼ
		drawingDlg.m_pDrawing->UpdateSimpleSpecList();
	if(drawingDlg.m_pComponent)
	{	//��������ѡ����ͼ
		if(drawingDlg.m_pComponent->m_bNeedAdjust)
		{
			scriptDlg.m_pDrawing=drawingDlg.m_pDrawing;
			scriptDlg.m_pComponent=drawingDlg.m_pComponent;
			drawingDlg.m_pComponent->InitCmdFeature(-1);	//��ʼ������feature=-1,�����Ի��򱣳�˳��ʱ�õ� wht 11-07-23
			if(scriptDlg.DoModal()!=IDOK)
			{	//�ָ��˼�ʼ�ն�������
				sys.stru_map.fPoleAxisLineStartOutSpreadLen=fOldStartOutSpreadLen;
				sys.stru_map.fPoleAxisLineEndOutSpreadLen=fOldEndOutSpreadLen;
				return;
			}
			drawingDlg.m_pComponent->UpdateDrawingRectArray();
			drawingDlg.m_pComponent->InsertDrawingName();	//��ע��ͼ�����Լ��ϽӶκ�
		#ifdef _ARX_2007
			acedCommand(RTSTR,L"PDMODE",RTSTR,L"0",RTNONE);//��ʾ�� .
		#else
			acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);//��ʾ�� .
		#endif
		}
		else 
			drawingDlg.m_pComponent->Create();
	}
	else
	{	//����һ��ͼֽ
		CDrawingComponent *pComponent=NULL;
		for(pComponent=drawingDlg.m_pDrawing->GetFirstDrawingComponent();
			pComponent;pComponent=drawingDlg.m_pDrawing->GetNextDrawingComponent())
		{
			if(pComponent->m_bNeedAdjust)
			{
				scriptDlg.m_pDrawing=drawingDlg.m_pDrawing;
				scriptDlg.m_pComponent=pComponent;
				if(scriptDlg.DoModal()!=IDOK)
					break;
				pComponent->UpdateDrawingRectArray();
				pComponent->InsertDrawingName();	//��ע��ͼ�����Լ��ϽӶκ�
			#ifdef _ARX_2007
				acedCommand(RTSTR,L"PDMODE",RTSTR,L"0",RTNONE);//��ʾ�� .
			#else
				acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);//��ʾ�� .
			#endif
			}
			else 
				pComponent->Create();
		}
		drawingDlg.m_pDrawing->m_bAutoLayoutComponent=FALSE;
		drawingDlg.m_pDrawing->DimSectionViewSymbol();	//һ��ͼֽ������ɺ��ע����ͼλ��
	}
	//�ָ��˼�ʼ�ն�������
	sys.stru_map.fPoleAxisLineStartOutSpreadLen=fOldStartOutSpreadLen;
	sys.stru_map.fPoleAxisLineEndOutSpreadLen=fOldEndOutSpreadLen;
}

//���汾�ŵı������ȡ����һ�������� wht 11-08-09
BOOL SerializeDrawScriptHeader(CBuffer &buffer,BOOL bStore,long *pVersion=NULL)
{
	const long curVersion=1000024;	//�汾��
	if(bStore)
	{
		buffer.WriteString("DrawingList");
		// v1000003 ���� CDrawingBOM::m_nBriefGuigeN wht 11-06-15
		// v1000005 ���� ������ϸ��ע ��׼λ��ƫ���� wht 11-07-25
		// v1000006 bDimGuige��bDimJgLength��λbOnlyDimPartNo wht 11-07-28
		// v1000007 ������˨��Ȧ��ע��ʽ wht 11-08-02
		// v1000008 ɾ�����ƹ����������� wht 11-08-08
		// v1000009 �޸ı�ע�Ǹ�׼�ݲ��� wht 11-08-09
		// v1000010 ����Ǹ�׼�ݹ�����˨ wht 11-08-10
		// v1000011 ����Ǹ�׼���Ƿ����׼���ע wjh 11-08-31
		// v1000012 ����Ǹ�֫�������ʱ����չ��ʽ wjh 11-08-31
		// v1000013 ����ͷ��עλ���ɾ�������ת��Ϊ�˼���ͷ����λ�õ����ƫ����
		// v1000014 ����ƽ����˨ֱ�߻��ƹ��� wjh 11-9-25
		// v1000015 ��������˨�׾���ע����ѡ��� wjh 11-10-09
		// v1000016 CDrawingCommand�����ӹ�������˳������� wht 12-05-25
		// v1000017 CDrawingBOM�б���򻯹��� hxr 13-03-02
		// v1000018 CDrawingBOM�б���pMark->cMark wht 13-04-12
		// v1000019 ����ṹͼ���Ʊ��� �Ƿ��ע����ͷ���Ƿ��ע������ŵ�ѡ��� wht 13-04-19
		// v1000020 ����CLsSpaceDimUnit��nLsSpaceNum��bMergeDimLsSpace����
		//			��CDrawingComponent::m_bDimName��CDrawingComponent::m_bDimScale���� hxr 13-04-23
		// v1000021 ����MAP_FRAME_SIZE������ͼ�����Ʊ���sMapFrameName hxr 13-05-03
		// v1000022 CStruView����Ӽ��ű�ע�ԳƷ�ʽ wht 17-08-08
		// v1000023 CLsSpaceDimUnit�����dim_angle��bCalDimAngle wht 17-08-17
		// v1000024 ��ע����������������bDimRefLine����������,CPartDrawing�����ӹ���������� wjh 17-08-27
		buffer.WriteInteger(curVersion);
	}
	else 
	{
		char sBufferFlag[50]="";
		buffer.ReadString(sBufferFlag);
		if(stricmp(sBufferFlag,"DrawingList")!=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("the format of drawing list is wrong!");
#else
			AfxMessageBox("ͼֽ�б��ʽ����ȷ!");
#endif
			return FALSE;
		}
		long version=0;	//��ʼ(��С)�汾��Ϊ:1000001
		buffer.ReadInteger(&version);
		if(version<1000001)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("The version of the drawing information does not match !");
#else
			AfxMessageBox("��ͼ��Ϣ�汾�Ų�ƥ��!");
#endif
			return FALSE;
		}
		if(version>curVersion)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("The version of the drawing information is higher than the version of software!");
#else
			AfxMessageBox("��ͼ��Ϣ�汾�Ÿ�������汾��!");
#endif
			return FALSE;
		}
		if(pVersion)
			*pVersion=version;
	}
	return TRUE;
}

void SaveScriptToBuffer()
{
	CAttachBuffer* pAttachBuffer=Ta.AttachBuffer.GetValue(CTower::SMART_DRAW);
	if(pAttachBuffer==NULL)
		return;
	CBuffer buffer;
#ifdef AFX_TARG_ENU_ENGLISH
	PutMessage(NULL,"Saving the drawing information......");
#else
	PutMessage(NULL,"���ڱ����ͼ��Ϣ......");
#endif
	buffer.ClearBuffer();
	//ͼֽ�б�
	SerializeDrawScriptHeader(buffer,TRUE);
	//����ṹͼ���ƿ���ѡ�� wht 13-04-19
	buffer.WriteInteger(CStruView::m_bDimPartSpec);
	buffer.WriteInteger(CStruView::m_bDimPoleOddment);
	buffer.WriteInteger(CStruView::m_bDimFootNailSpace);
	buffer.WriteInteger(CStruView::m_bDimFillPlateSpace);
	buffer.WriteInteger(CStruView::m_bDimLsSpace);
	buffer.WriteInteger(CStruView::m_bDimJgZhunJu);
	buffer.WriteInteger(CStruView::m_bDimBoltInfo);
	buffer.WriteInteger(CStruView::m_bDimBoltPad);
	buffer.WriteInteger(CStruView::m_bDimSimPoly);
	//����ͼֽ��Ϣ
	int nDrawing=drawingList.GetNodeNum();
	buffer.WriteInteger(nDrawing);
	CDrawing *pDrawing=NULL;
	for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
		pDrawing->ToBuffer(buffer);
	pAttachBuffer->ClearBuffer();
	pAttachBuffer->Write(buffer.GetBufferPtr(),buffer.GetLength());
	PutMessage(NULL,NULL);
}

void LoadScriptFromBuffer()
{
	drawingList.Empty();
	CAttachBuffer* pAttachBuffer=Ta.AttachBuffer.GetValue(CTower::SMART_DRAW);
	if(pAttachBuffer==NULL||pAttachBuffer->GetLength()<=0)
		return;
	//��������װ��ΪCBuffer�࣬��������Ļ������ÿ��ܻ���� wjh-2012.12.23
	CBuffer buffer(pAttachBuffer->GetBufferPtr(),pAttachBuffer->GetLength());
	//if(Ta.ParseDrawingScriptBuffer==NULL)
		buffer.SeekToBegin();
	//Ta.DisplayProcess=NULL;
	//ͼֽ�б�
	long buffer_len=buffer.GetLength();
	long version=0;
	if(!SerializeDrawScriptHeader(buffer,FALSE,&version))
		return;
	if(version>=1000019)
	{	//��ȡ�ṹͼ���ƿ���ѡ�� wht 13-04-19
		buffer.ReadInteger(&CStruView::m_bDimPartSpec);
		buffer.ReadInteger(&CStruView::m_bDimPoleOddment);
		buffer.ReadInteger(&CStruView::m_bDimFootNailSpace);
		buffer.ReadInteger(&CStruView::m_bDimFillPlateSpace);
		buffer.ReadInteger(&CStruView::m_bDimLsSpace);
		buffer.ReadInteger(&CStruView::m_bDimJgZhunJu);
		buffer.ReadInteger(&CStruView::m_bDimBoltInfo);
		buffer.ReadInteger(&CStruView::m_bDimBoltPad);
		buffer.ReadInteger(&CStruView::m_bDimSimPoly);
	}
	//��ȡͼֽ��Ϣ
	int i=0,nDrawingCount=0;
	buffer.ReadInteger(&nDrawingCount);
	for(i=0;i<nDrawingCount;i++)
	{
		if(Ta.DisplayProcess)
#ifdef AFX_TARG_ENU_ENGLISH
			Ta.DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),"the reading progress of drawing script");
#else
			Ta.DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),"��ͼ�ű���ȡ����");
#endif
		CDrawing *pDrawing=drawingList.Add(0);
		pDrawing->FromBuffer(buffer,version);
	}
	if(Ta.DisplayProcess)
#ifdef AFX_TARG_ENU_ENGLISH
		Ta.DisplayProcess(100,"the reading progress of drawing script");
#else
		Ta.DisplayProcess(100,"��ͼ�ű���ȡ����");
#endif
}

#endif