//<LOCALE_TRANSLATE/>
//DrawMapScript.cpp 绘图脚本

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
	const int nCmdDlgWidth=100;	//命令框宽度
	if(pDrawDlg->GetOperType()==0)
	{	//执行命令到断点处停止
		if(pDrawDlg->m_pComponent)
		{
			if(pDrawDlg->m_pComponent->m_bFirstCreate)
			{	//第一次生成，需要自动计算视图装配位置
				pDrawDlg->m_pComponent->CalInitAssemblyPosition();
				pDrawDlg->m_pComponent->m_bFirstCreate=FALSE;
			}
			//使用相对位置重新计算坐标系原点位置
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
					continue;	//不属于当前分组
				CDrawingCommand *pCmd=(CDrawingCommand*)pItem->m_idProp;
				if(pCmd==NULL)
					continue;
				if(pCmd&&pCmd->m_stateCmd&CMD_STATE_NEXTPOS)	//去除命令行 下一步 标志
					pCmd->m_stateCmd^=CMD_STATE_NEXTPOS;
				if(!pCmd->IsNeedExecute())
					continue;	//该命令不需要执行
				if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY&&pCmd->m_paraSimPoly.nSimPolyCmdType==1)
					continue;	//该命令仅用来手动添加相似形
				BOOL bDelete=FALSE;
				if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY&&pCmd->m_paraSimPoly.nSimPolyCmdType==0)
					bDelete=TRUE;	//该命令仅用来启用相似形标注，标注完成后该命令可删除
				pCmd->Run();
				if(bDelete)
				{	//删除用来启动标注相似形的命令
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
				//绘制构件时缩放窗口显示所有实体，标注时不需要否则acedSSGet函数调用失败
				if(j==0)
				{	//缩放窗口显示所有实体
					ads_point L_T,R_B;
					DRAGSET.GetDragEntSetScope(L_T,R_B);
					R_B[X]+=nCmdDlgWidth;	//命令框显示在右侧，此处调整右下角X坐标
				#ifdef _ARX_2007
					acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#else
					acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#endif
				}
				//if(j==1)
				//	acedCommand(RTSTR,"REGEN",RTNONE);
				//设置下行为运行状态
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
	{	//单步执行
		CSuperGridCtrl::CTreeItem *pItem=NULL,*pNextItem=NULL;
		pItem=pDrawDlg->GetFirstExecutableItem();	//获得第一个可执行项
		CDrawingCommand *pCmd=NULL,*pNextCmd=NULL;
		if(pItem)
		{
			pCmd=(CDrawingCommand*)pItem->m_idProp;
			if(pCmd)
			{
				if(pCmd->Run()==0)
					return FALSE;
				pCmd->SetCmdFlag(TRUE);
				//缩放窗口突出显示新添加的实体
				ads_point L_T,R_B;
				DRAGSET.GetDragEntSetScope(L_T,R_B);
				R_B[X]+=nCmdDlgWidth;	//命令框显示在右侧，此处调整右下角X坐标
			#ifdef _ARX_2007
				acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
			#else
				acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
			#endif
				if(pCmd->m_stateCmd&CMD_STATE_NEXTPOS)
				{
					pCmd->m_stateCmd^=CMD_STATE_NEXTPOS;
					//SuperGridCtrl 设置图标是第三个图标不能正常显示 此处暂做如下处理 wht 10-11-30
					if(pCmd->m_stateCmd<2)
						pItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd);	//设置命令行状态
					else 
						pItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd+1);	//设置命令行状态
				}
			}
			pNextItem=pDrawDlg->m_listCmdSuperCtrl.GetRootItem(pItem->GetIndex()+1);
			if(pNextItem)
				pNextCmd=(CDrawingCommand*)pNextItem->m_idProp;
			if(pNextCmd&&!(pNextCmd->m_stateCmd&CMD_STATE_NEXTPOS))
			{
				pNextCmd->m_stateCmd|=CMD_STATE_NEXTPOS;
				//SuperGridCtrl 设置图标是第三个图标不能正常显示 此处暂做如下处理 wht 10-11-30
				if(pNextCmd->m_stateCmd<2)
					pNextItem->m_lpNodeInfo->SetImage(pNextCmd->m_stateCmd);	//设置命令行状态
				else 
					pNextItem->m_lpNodeInfo->SetImage(pNextCmd->m_stateCmd+1);	//设置命令行状态
				pDrawDlg->m_listCmdSuperCtrl.SelectItem(pNextItem);
				if(pNextItem->m_dwPropGroup!=GetSingleWord(pDrawDlg->m_iOldGroup+1))
				{	//命令行不属于当前分组切换到下个分组
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
	{	//回退一步
		CSuperGridCtrl::CTreeItem *pItem=NULL,*pNextItem=NULL;
		pItem=pDrawDlg->GetLastExecutedItem();	//获得最后一个已执行的命令行
		CDrawingCommand *pCmd=NULL,*pNextCmd=NULL;
		if(pItem)
		{
			pCmd=(CDrawingCommand*)pItem->m_idProp;
			if(pCmd)
			{
				pCmd->EmptyEntList();	//删除由该命令生成的实体
				//缩放显示所有实体
				ads_point L_T,R_B;
				DRAGSET.GetDragEntSetScope(L_T,R_B);
			#ifdef _ARX_2007
				acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
			#else
				acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
			#endif
				//设置上个命令状态为未执行
				pCmd->SetCmdFlag(FALSE);
				if(!(pCmd->m_stateCmd&CMD_STATE_NEXTPOS))
				{
					pCmd->m_stateCmd|=CMD_STATE_NEXTPOS;
					//SuperGridCtrl 设置图标是第三个图标不能正常显示 此处暂做如下处理 wht 10-11-30
					if(pCmd->m_stateCmd<2)
						pItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd);	//设置命令行状态
					else 
						pItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd+1);	//设置命令行状态
				}
				pDrawDlg->m_listCmdSuperCtrl.SelectItem(pItem);
			}
			pNextItem=pDrawDlg->m_listCmdSuperCtrl.GetRootItem(pItem->GetIndex()+1);
			if(pNextItem)
				pNextCmd=(CDrawingCommand*)pNextItem->m_idProp;
			if(pNextCmd&&(pNextCmd->m_stateCmd&CMD_STATE_NEXTPOS))
			{
				pNextCmd->m_stateCmd^=CMD_STATE_NEXTPOS;
				//SuperGridCtrl 设置图标是第三个图标不能正常显示 此处暂做如下处理 wht 10-11-30
				if(pNextCmd->m_stateCmd<2)
					pNextItem->m_lpNodeInfo->SetImage(pNextCmd->m_stateCmd);	//设置命令行状态
				else 
					pNextItem->m_lpNodeInfo->SetImage(pNextCmd->m_stateCmd+1);	//设置命令行状态
			}
		}
		return TRUE;
	}
	else if(pDrawDlg->GetOperType()==5)
	{	//添加相似形
		
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
	//0.基准级别 1.总图 2.司令图 3.结构图 4.构件图 5.组焊图
	if(drawingDlg.m_pDrawing->m_nSegI.iSeg>0)
		sys.fonts.InitSpecialProp(3);
	else if(drawingDlg.m_pDrawing->m_nSegI.iSeg==-1)
		sys.fonts.InitSpecialProp(2);
	else if(drawingDlg.m_pDrawing->m_nSegI.iSeg==-2)
		sys.fonts.InitSpecialProp(1);
	else
		sys.fonts.InitSpecialProp(0);

	//记录杆件始终端延伸量
	double fOldStartOutSpreadLen=sys.stru_map.fPoleAxisLineStartOutSpreadLen;
	double fOldEndOutSpreadLen=sys.stru_map.fPoleAxisLineEndOutSpreadLen;
	//插入图框
	drawingDlg.m_pDrawing->InsertMapFrm();
	//
	DRAGSET.ClearEntSet();
	CDrawMapScriptDlg scriptDlg;
	scriptDlg.SetCallBackFunc(FireRunCommand);
	dimpartspec.EmptyPartSpecDim();	//清空构件规格标注列表 wht 11-06-19
	if(drawingDlg.m_pDrawing->m_nSegI.iSeg==-1)	//司令图
		drawingDlg.m_pDrawing->UpdateSimpleSpecList();
	if(drawingDlg.m_pComponent)
	{	//单独生成选中视图
		if(drawingDlg.m_pComponent->m_bNeedAdjust)
		{
			scriptDlg.m_pDrawing=drawingDlg.m_pDrawing;
			scriptDlg.m_pComponent=drawingDlg.m_pComponent;
			drawingDlg.m_pComponent->InitCmdFeature(-1);	//初始化命令feature=-1,重启对话框保持顺序时用到 wht 11-07-23
			if(scriptDlg.DoModal()!=IDOK)
			{	//恢复杆件始终端延伸量
				sys.stru_map.fPoleAxisLineStartOutSpreadLen=fOldStartOutSpreadLen;
				sys.stru_map.fPoleAxisLineEndOutSpreadLen=fOldEndOutSpreadLen;
				return;
			}
			drawingDlg.m_pComponent->UpdateDrawingRectArray();
			drawingDlg.m_pComponent->InsertDrawingName();	//标注视图名称以及上接段号
		#ifdef _ARX_2007
			acedCommand(RTSTR,L"PDMODE",RTSTR,L"0",RTNONE);//显示点 .
		#else
			acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);//显示点 .
		#endif
		}
		else 
			drawingDlg.m_pComponent->Create();
	}
	else
	{	//生成一张图纸
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
				pComponent->InsertDrawingName();	//标注视图名称以及上接段号
			#ifdef _ARX_2007
				acedCommand(RTSTR,L"PDMODE",RTSTR,L"0",RTNONE);//显示点 .
			#else
				acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);//显示点 .
			#endif
			}
			else 
				pComponent->Create();
		}
		drawingDlg.m_pDrawing->m_bAutoLayoutComponent=FALSE;
		drawingDlg.m_pDrawing->DimSectionViewSymbol();	//一张图纸绘制完成后标注剖视图位置
	}
	//恢复杆件始终端延伸量
	sys.stru_map.fPoleAxisLineStartOutSpreadLen=fOldStartOutSpreadLen;
	sys.stru_map.fPoleAxisLineEndOutSpreadLen=fOldEndOutSpreadLen;
}

//将版本号的保存与读取放在一个函数中 wht 11-08-09
BOOL SerializeDrawScriptHeader(CBuffer &buffer,BOOL bStore,long *pVersion=NULL)
{
	const long curVersion=1000024;	//版本号
	if(bStore)
	{
		buffer.WriteString("DrawingList");
		// v1000003 新增 CDrawingBOM::m_nBriefGuigeN wht 11-06-15
		// v1000005 新增 构件明细标注 基准位置偏移量 wht 11-07-25
		// v1000006 bDimGuige、bDimJgLength换位bOnlyDimPartNo wht 11-07-28
		// v1000007 新增螺栓垫圈标注方式 wht 11-08-02
		// v1000008 删除绘制构件多余属性 wht 11-08-08
		// v1000009 修改标注角钢准据参数 wht 11-08-09
		// v1000010 保存角钢准据关联螺栓 wht 11-08-10
		// v1000011 保存角钢准据是否进行准距标注 wjh 11-08-31
		// v1000012 保存角钢肢宽夸大绘制时的拓展方式 wjh 11-08-31
		// v1000013 正负头标注位置由绝对坐标转换为杆件端头基点位置的相对偏移量
		// v1000014 增加平放螺栓直线绘制功能 wjh 11-9-25
		// v1000015 增加虚螺栓孔径标注开关选项功能 wjh 11-10-09
		// v1000016 CDrawingCommand中增加构件绘制顺序号属性 wht 12-05-25
		// v1000017 CDrawingBOM中保存简化规格表 hxr 13-03-02
		// v1000018 CDrawingBOM中保存pMark->cMark wht 13-04-12
		// v1000019 保存结构图控制变量 是否标注正负头、是否标注构件编号等选项开关 wht 13-04-19
		// v1000020 新增CLsSpaceDimUnit中nLsSpaceNum、bMergeDimLsSpace属性
		//			及CDrawingComponent::m_bDimName、CDrawingComponent::m_bDimScale属性 hxr 13-04-23
		// v1000021 保存MAP_FRAME_SIZE中增加图框名称变量sMapFrameName hxr 13-05-03
		// v1000022 CStruView中添加件号标注对称方式 wht 17-08-08
		// v1000023 CLsSpaceDimUnit中添加dim_angle、bCalDimAngle wht 17-08-17
		// v1000024 标注相似形命令中增加bDimRefLine引出线属性,CPartDrawing中增加构件编号属性 wjh 17-08-27
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
			AfxMessageBox("图纸列表格式不正确!");
#endif
			return FALSE;
		}
		long version=0;	//初始(最小)版本号为:1000001
		buffer.ReadInteger(&version);
		if(version<1000001)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("The version of the drawing information does not match !");
#else
			AfxMessageBox("绘图信息版本号不匹配!");
#endif
			return FALSE;
		}
		if(version>curVersion)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("The version of the drawing information is higher than the version of software!");
#else
			AfxMessageBox("绘图信息版本号高于软件版本号!");
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
	PutMessage(NULL,"正在保存绘图信息......");
#endif
	buffer.ClearBuffer();
	//图纸列表
	SerializeDrawScriptHeader(buffer,TRUE);
	//保存结构图绘制开关选项 wht 13-04-19
	buffer.WriteInteger(CStruView::m_bDimPartSpec);
	buffer.WriteInteger(CStruView::m_bDimPoleOddment);
	buffer.WriteInteger(CStruView::m_bDimFootNailSpace);
	buffer.WriteInteger(CStruView::m_bDimFillPlateSpace);
	buffer.WriteInteger(CStruView::m_bDimLsSpace);
	buffer.WriteInteger(CStruView::m_bDimJgZhunJu);
	buffer.WriteInteger(CStruView::m_bDimBoltInfo);
	buffer.WriteInteger(CStruView::m_bDimBoltPad);
	buffer.WriteInteger(CStruView::m_bDimSimPoly);
	//保存图纸信息
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
	//必须重新装箱为CBuffer类，否则后续的缓存引用可能会出错 wjh-2012.12.23
	CBuffer buffer(pAttachBuffer->GetBufferPtr(),pAttachBuffer->GetLength());
	//if(Ta.ParseDrawingScriptBuffer==NULL)
		buffer.SeekToBegin();
	//Ta.DisplayProcess=NULL;
	//图纸列表
	long buffer_len=buffer.GetLength();
	long version=0;
	if(!SerializeDrawScriptHeader(buffer,FALSE,&version))
		return;
	if(version>=1000019)
	{	//读取结构图绘制开关选项 wht 13-04-19
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
	//读取图纸信息
	int i=0,nDrawingCount=0;
	buffer.ReadInteger(&nDrawingCount);
	for(i=0;i<nDrawingCount;i++)
	{
		if(Ta.DisplayProcess)
#ifdef AFX_TARG_ENU_ENGLISH
			Ta.DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),"the reading progress of drawing script");
#else
			Ta.DisplayProcess(ftoi(buffer.GetCursorPosition()/(buffer_len*0.01)),"绘图脚本读取进度");
#endif
		CDrawing *pDrawing=drawingList.Add(0);
		pDrawing->FromBuffer(buffer,version);
	}
	if(Ta.DisplayProcess)
#ifdef AFX_TARG_ENU_ENGLISH
		Ta.DisplayProcess(100,"the reading progress of drawing script");
#else
		Ta.DisplayProcess(100,"绘图脚本读取进度");
#endif
}

#endif