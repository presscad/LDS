#include "stdafx.h"
#include "LDS.h"
#include "console.h"
#include "LDSView.h"

bool CLDSView::FromViewBuffer(CBuffer& buffer,DISPLAY_ENV& enviornment,NODESET* pDisplayNodeSet,PARTSET* pDisplayPartSet)
{
	//char envpool[2048]={0};
	UINT envpoolsize=0;
	buffer.ReadInteger(&envpoolsize);
	if(buffer.Read(enviornment.envpool,envpoolsize)<envpoolsize)
		return false;
	buffer.ReadInteger(&enviornment.eViewType);
	buffer.ReadInteger(&enviornment.m_hView);
	buffer.ReadInteger(&enviornment.m_hHeightModule);
	buffer.ReadPoint(enviornment.object_ucs.origin);
	buffer.ReadPoint(enviornment.object_ucs.axis_x);
	buffer.ReadPoint(enviornment.object_ucs.axis_y);
	buffer.ReadPoint(enviornment.object_ucs.axis_z);
	//读取原视图中显示杆件与节点集合
	long hObj=0;
	int i,nNodeCount=0,nPartCount=0;
	buffer.ReadInteger(&nNodeCount);
	if(pDisplayNodeSet==NULL)
		buffer.Offset(nNodeCount*4);
	else
	{
		pDisplayNodeSet->Empty();
		for(i=0;i<nNodeCount;i++)
		{
			buffer.ReadInteger(&hObj);
			CLDSNode* pNode=Ta.Node.FromHandle(hObj);
			if(pNode)
				pDisplayNodeSet->append(pNode);
		}
	}
	buffer.ReadInteger(&nPartCount);
	if(pDisplayPartSet==NULL)
		buffer.Offset(nPartCount*4);
	else
	{
		pDisplayPartSet->Empty();
		for(i=0;i<nPartCount;i++)
		{
			buffer.ReadInteger(&hObj);
			CLDSPart* pPart=Ta.Parts.FromHandle(hObj);
			if(pPart)
				pDisplayPartSet->append(pPart);
		}
	}
	return true;
}
void CLDSView::CurrentViewToBuffer(CBuffer& buffer)
{
	char envpool[2048]={0};
	UINT envpoolsize=g_pSolidOper->GetCurrEnvionment(envpool);
	buffer.WriteInteger(envpoolsize);
	buffer.Write(envpool,envpoolsize);
	buffer.WriteInteger(m_eViewFlag);
	CDisplayView* pDisplayView=console.GetActiveView();
	if(pDisplayView&&pDisplayView->m_iViewType==0&&m_eViewFlag==RANDOM_VIEW)
		buffer.WriteInteger(0);
	else
		buffer.WriteInteger(pDisplayView->handle);
	buffer.WriteInteger(Ta.m_hActiveModule);
	UCS_STRU object_ucs;
	g_pSolidSet->GetObjectUcs(object_ucs);
	buffer.WritePoint(object_ucs.origin);
	buffer.WritePoint(object_ucs.axis_x);
	buffer.WritePoint(object_ucs.axis_y);
	buffer.WritePoint(object_ucs.axis_z);
	//写入当前显示杆件与节点集合
	buffer.WriteInteger(console.DispNodeSet.GetNodeNum());
	for(CLDSNode* pNode=console.DispNodeSet.GetFirst();pNode;pNode=console.DispNodeSet.GetNext())
		buffer.WriteInteger(pNode->handle);
	buffer.WriteInteger(console.DispPartSet.GetNodeNum());
	for(CLDSPart* pPart=console.DispPartSet.GetFirst();pPart;pPart=console.DispPartSet.GetNext())
		buffer.WriteInteger(pPart->handle);
}
// 是否有新的Undo信息需要存储
BOOL CLDSView::IsViewStateModified()
{
	long nBufSize=UndoViewBuffStack.LastRecordLength();
	if(nBufSize<=0)
		return 0xffff;
	CHAR_ARRAY bufpool(nBufSize);
	UndoViewBuffStack.LastRecordBuffer(bufpool);
	DISPLAY_ENV dispenv;
	NODESET nodeset;
	PARTSET partset;
	CDisplayView* pDisplayView=console.GetActiveView();
	if(pDisplayView==NULL)
		return TRUE;
	CLDSModule* pDisplayModule=console.GetActiveModule();
	if(pDisplayModule==NULL)
		return TRUE;
	FromViewBuffer(CBuffer(bufpool,nBufSize),dispenv,NULL,NULL);
	if(pDisplayView->m_iViewType==0&&m_eViewFlag==RANDOM_VIEW)
	{	//临时展开的视图,只要展开坐标系不同就应视为修改
		UCS_STRU objcs;
		g_pSolidSet->GetObjectUcs(objcs);
		if( !objcs.origin.IsEqual(dispenv.object_ucs.origin)||!objcs.axis_x.IsEqual(dispenv.object_ucs.axis_x)||
			!objcs.axis_y.IsEqual(dispenv.object_ucs.axis_y)||!objcs.axis_z.IsEqual(dispenv.object_ucs.axis_z))
			return TRUE;
	}
	else if(pDisplayView->handle!=dispenv.m_hView||pDisplayModule->handle!=dispenv.m_hHeightModule)
		return TRUE;
	/*
	BYTE cbModifyFlag=0x80;
	if(nodeset.GetNodeNum()!=console.DispNodeSet.GetNodeNum())
		return cbModifyFlag|=0x02;
	if(partset.GetNodeNum()!=console.DispPartSet.GetNodeNum())
		return cbModifyFlag|=0x4;
	*/
	//TODO:获取显示环境的差异

	return FALSE;//cbModifyFlag;
}
// 执行视图压栈操作
UINT CLDSView::PushView(void)
{
	CBuffer buffer(2048);
	CurrentViewToBuffer(buffer);
	if(IsViewStateModified())
		UndoViewBuffStack.push(buffer.GetBufferPtr(),buffer.GetLength(),1);
	else
	{
		UndoViewBuffStack.UpdateLastRecord(buffer.GetBufferPtr(),buffer.GetLength(),1);
		UndoViewBuffStack.TerminateLaterRedo();
	}
	return buffer.GetLength();
}
// 执行Undo操作
void CLDSView::UndoView(void)
{
	//获取Undo操作记录数据内存
	long nBufferSize=UndoViewBuffStack.UndoRecordLength();
	if(nBufferSize<=0)
		return;	//防止在无Undo/Redo内容,但界面来不及更新时点击了Undo/Redo导致错误 wjh-2011.8.9
	CBuffer buffer(2048);
	CurrentViewToBuffer(buffer);
	if(UndoViewBuffStack.CurrentRecordSerial()!=UndoViewBuffStack.LastRecordSerial())
	{	//自动将当前状态存储为新视图记录后再undo
		if(IsViewStateModified())
			UndoViewBuffStack.push(buffer.GetBufferPtr(),buffer.GetLength(),1);
		else
			UndoViewBuffStack.UpdateLastRecord(buffer.GetBufferPtr(),buffer.GetLength(),1);
		UndoViewBuffStack.popUndo(NULL);
	}
	else
		UndoViewBuffStack.UpdateLastRecord(buffer.GetBufferPtr(),buffer.GetLength(),1);
	nBufferSize=UndoViewBuffStack.UndoRecordLength();
	if(nBufferSize<=0)
		return;
	CHAR_ARRAY pool(nBufferSize);
	UndoViewBuffStack.popUndo(pool);
	buffer.AttachMemory(pool,nBufferSize);
	DISPLAY_ENV dispenv;
	FromViewBuffer(buffer,dispenv,&console.DispNodeSet,&console.DispPartSet);
	m_eViewFlag=VIEW_DIRECT(dispenv.eViewType);
	g_pSolidOper->SetCurrEnvionment(dispenv.envpool);
	if(dispenv.m_hView>0x20)
		console.SetActiveView(dispenv.m_hView);
	Ta.m_hActiveModule=dispenv.m_hHeightModule;
	g_pSolidDraw->BuildDisplayList(&OBJECTCS_CONTEXT(&UCS_STRU(dispenv.object_ucs)));
	g_pSolidDraw->Draw();
}
// 执行Redo操作
void CLDSView::RedoView(void)
{
	CBuffer buffer(2048);
	CurrentViewToBuffer(buffer);
	if(UndoViewBuffStack.CurrentRecordSerial()==UndoViewBuffStack.LastRecordSerial())
	{	//自动将当前状态更新存储到为原视图记录后再redo
		UndoViewBuffStack.UpdateLastRecord(buffer.GetBufferPtr(),buffer.GetLength(),1);
		UndoViewBuffStack.popRedo(NULL);
	}
	//获取Redo操作记录数据内存
	long nBufferSize=UndoViewBuffStack.RedoRecordLength(); 
	if(nBufferSize<=0)
		return;	//防止在无Undo/Redo内容,但界面来不及更新时点击了Undo/Redo导致错误 wjh-2011.8.9
	CHAR_ARRAY pool(nBufferSize);
	UndoViewBuffStack.popRedo(pool);
	buffer.AttachMemory(pool,nBufferSize);
	DISPLAY_ENV dispenv;
	FromViewBuffer(buffer,dispenv,&console.DispNodeSet,&console.DispPartSet);
	m_eViewFlag=VIEW_DIRECT(dispenv.eViewType);
	g_pSolidOper->SetCurrEnvionment(dispenv.envpool);
	if(dispenv.m_hView>0x20)
		console.SetActiveView(dispenv.m_hView);
	Ta.m_hActiveModule=dispenv.m_hHeightModule;
	g_pSolidDraw->BuildDisplayList(&OBJECTCS_CONTEXT(&UCS_STRU(dispenv.object_ucs)));
	g_pSolidDraw->Draw();
}
// 可以执行Undo操作
BOOL CLDSView::CanUndoView(void)
{
	if(UndoViewBuffStack.UndoRecordLength()>=0)
		return TRUE;
	else
		return FALSE;
}
// 可以执行Redo操作
BOOL CLDSView::CanRedoView(void)
{
	if(UndoViewBuffStack.RedoRecordLength()>=0)
		return TRUE;
	else
		return FALSE;
}
