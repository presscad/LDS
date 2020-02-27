#include "stdafx.h"
#include "Tower.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CUndoOperObject::CUndoOperObject(CTower *pTower,bool bGroupUndo/*=false*/,
	int iOperType/*=0*/,bool bStartListenInstant/*=true*/)
{
	m_pTower=pTower;
	m_iOperType=iOperType;
	m_bGroupUndo=bGroupUndo;
	if(bStartListenInstant&&!pTower->GroupUndoStarted())
	{
		m_bObjectStart=true;	//有效启动该UNDO对像
		if(m_bGroupUndo)
			m_pTower->StartUndoGroup();
		else
			m_pTower->BeginUndoListen();
	}
	else
		m_bObjectStart=false;
}
CUndoOperObject::~CUndoOperObject()
{
	if(m_bObjectStart)
	{
		if(m_bGroupUndo)
			m_pTower->EndUndoGroup(m_iOperType);
		else
			m_pTower->EndUndoListen(m_iOperType);
	}
}
bool CUndoOperObject::StartUndoListen()
{
	if(!m_bObjectStart)
	{
		m_bObjectStart=true;	//有效启动该UNDO对像
		if(m_bGroupUndo)
			m_pTower->StartUndoGroup();
		else
			m_pTower->BeginUndoListen();
	}
	return m_bObjectStart;
}
void CTower::StartUndoGroup()
{
	BeginUndoListen();
	m_bGroupUndoStart=true;
}
bool CTower::EndUndoGroup(int iOperType/*=0*/)
{
	m_bGroupUndoStart=false;
	return EndUndoListen(iOperType);
}
// 开始监听Undo操作
void CTower::BeginUndoListen(void)
{
	if(m_bGroupUndoStart)
		return;	//已启用成组Undo功能，中间过程中的BeginUndoListen及EndUndoListen无效
	CBuffer buffer;	//默认1k递增空间，不能太大，因为每个对象都需要释放和分配内存
	UndoBuffer.ClearBuffer();
	buffer.SetBlockSize(1000000);	//大约1M递增空间
	UndoBuffer.SetBlockSize(1000000);	// 大约1M递增空间
#ifdef __LDS_
	int nn=Point.GetNodeNum()+Line.GetNodeNum()+Plane.GetNodeNum()+Node.GetNodeNum()+Parts.GetNodeNum();//+WindLoadSeg.GetNodeNum();
#else
	int nn=Point.GetNodeNum()+Line.GetNodeNum()+Plane.GetNodeNum()+Node.GetNodeNum()+Parts.GetNodeNum();
#endif
	UndoBuffer.WriteInteger(nn);	// 存入对象总数
	for(CLDSPoint *pPoint=Point.GetFirst();pPoint;pPoint=Point.GetNext())
	{
		buffer.ClearContents();
		pPoint->ToBuffer(buffer);
		UndoBuffer.WriteInteger(pPoint->handle);
		UndoBuffer.WriteInteger(buffer.GetLength());
		UndoBuffer.Write(buffer.GetBufferPtr(),buffer.GetLength());
		pPoint->ResetUndoModifiedFlag();	// 清除对象修改标记
	}
	for(CLDSLine *pLine=Line.GetFirst();pLine;pLine=Line.GetNext())
	{
		buffer.ClearContents();
		pLine->ToBuffer(buffer);
		UndoBuffer.WriteInteger(pLine->handle);
		UndoBuffer.WriteInteger(buffer.GetLength());
		UndoBuffer.Write(buffer.GetBufferPtr(),buffer.GetLength());
		pLine->ResetUndoModifiedFlag();	// 清除对象修改标记
	}
	for(CLDSPlane *pPlane=Plane.GetFirst();pPlane;pPlane=Plane.GetNext())
	{
		buffer.ClearContents();
		pPlane->ToBuffer(buffer);
		UndoBuffer.WriteInteger(pPlane->handle);
		UndoBuffer.WriteInteger(buffer.GetLength());
		UndoBuffer.Write(buffer.GetBufferPtr(),buffer.GetLength());
		pPlane->ResetUndoModifiedFlag();	// 清除对象修改标记
	}
	for(CLDSNode *pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		buffer.ClearContents();
		pNode->ToBuffer(buffer);
		UndoBuffer.WriteInteger(pNode->handle);
		UndoBuffer.WriteInteger(buffer.GetLength());
		UndoBuffer.Write(buffer.GetBufferPtr(),buffer.GetLength());
		pNode->ResetUndoModifiedFlag();	// 清除对象修改标记
	}
	for(CLDSPart *pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		buffer.ClearContents();
		pPart->ToBuffer(buffer);
		UndoBuffer.WriteInteger(pPart->handle);
		UndoBuffer.WriteInteger(buffer.GetLength());
		UndoBuffer.Write(buffer.GetBufferPtr(),buffer.GetLength());
		pPart->ResetUndoModifiedFlag();	// 清除对象Undo修改标记
	}
/*#ifdef __LDS_
	for(CWindSegment *pWindSeg=WindLoadSeg.GetFirst();pWindSeg;pWindSeg=WindLoadSeg.GetNext())
	{
		buffer.ClearBuffer();
		pWindSeg->ToBuffer(buffer);
		UndoBuffer.WriteInteger(pWindSeg->handle);
		UndoBuffer.WriteInteger(buffer.GetLength());
		UndoBuffer.Write(buffer.GetBufferPtr(),buffer.GetLength());
		pWindSeg->ResetUndoModifiedFlag();	// 清除对象Undo修改标记
	}
	WindLoadSeg.Clean();
#endif*/
	Point.Clean();
	Line.Clean();
	Plane.Clean();
	Node.Clean();	// 清空以前删除的节点
	Parts.Clean();	// 清空以前删除的构件
}
// 返回当前Undo记录的操作类型
int CTower::CurrentUndoType()
{
	return UndoBufferStack.UndoRecordType();
}
// 是否有新的Undo信息需要存储
BOOL CTower::IsHasUndoInfo()
{
	if(UndoBuffer.GetLength()==0)
		return FALSE;
	BOOL pushed=Point.push_stack();
	for(CLDSPoint *pPoint=Point.GetFirst(TRUE);pPoint;pPoint=Point.GetNext(TRUE))
	{
		if(Point.IsCursorDeleted()||pPoint->IsUndoModified())
		{
			if(pushed)
				Point.pop_stack();
			return TRUE;
		}
	}
	if(pushed)
		Point.pop_stack();
	pushed=Line.push_stack();
	for(CLDSLine *pLine=Line.GetFirst(TRUE);pLine;pLine=Line.GetNext(TRUE))
	{
		if(Line.IsCursorDeleted()||pLine->IsUndoModified())
		{
			if(pushed)
				Line.pop_stack();
			return TRUE;
		}
	}
	if(pushed)
		Line.pop_stack();
	pushed=Plane.push_stack();
	for(CLDSPlane *pPlane=Plane.GetFirst(TRUE);pPlane;pPlane=Plane.GetNext(TRUE))
	{
		if(Plane.IsCursorDeleted()||pPlane->IsUndoModified())
		{
			if(pushed)
				Plane.pop_stack();
			return TRUE;
		}
	}
	if(pushed)
		Plane.pop_stack();
	pushed=Node.push_stack();
	for(CLDSNode *pNode=Node.GetFirst(TRUE);pNode;pNode=Node.GetNext(TRUE))
	{
		if(Node.IsCursorDeleted()||pNode->IsUndoModified())
		{
			if(pushed)
				Node.pop_stack();
			return TRUE;
		}
	}
	if(pushed)
		Node.pop_stack();
	pushed=Parts.push_stack();
	for(CLDSPart *pPart=Parts.GetFirst(0,TRUE);pPart;pPart=Parts.GetNext(0,TRUE))
	{
		if(Parts.IsCursorDeleted()||pPart->IsUndoModified())
		{
			if(pushed)
				Parts.pop_stack();
			return TRUE;
		}
	}
	if(pushed)
		Parts.pop_stack();
	return FALSE;
}
// 开始监听Undo操作
bool CTower::EndUndoListen(int iOperType/*=0*/)
{
	if(m_bGroupUndoStart)
		return false;	//已启用成组Undo功能，中间过程中的BeginUndoListen及EndUndoListen无效
	if(!IsHasUndoInfo())
		return false;
	CBuffer buffer(2000000);	//设定为1M左右，以免反复申请内存
	CLDSPoint *pPoint=NULL;
	CLDSLine *pLine=NULL;
	CLDSPlane *pPlane=NULL;
	CLDSNode *pNode=NULL;
	CLDSPart *pPart=NULL;
/*#ifdef __LDS_
	CWindSegment *pSeg=NULL;
	long nWindSegCounts=0;
#endif*/
	CHashTable<long> hashObjBuffer;
	long i,iPos,h,bufferLen,nOrgObjNum;
	long nPointCounts=0,nLineCounts=0,nPlaneCounts=0,nNodeCounts=0,nPartCounts=0,dwObjState=0;

	//将修改前的各对象存入哈希表以备查询
	UndoBuffer.SeekToBegin();
	UndoBuffer.ReadInteger(&nOrgObjNum);
	hashObjBuffer.CreateHashTable(nOrgObjNum+8);	// +8为了防止出现零元素数的情况
	for(i=0;i<nOrgObjNum;i++)
	{
		UndoBuffer.ReadInteger(&h);
		hashObjBuffer.SetValueAt(h,UndoBuffer.GetCursorPosition()-4);
		UndoBuffer.ReadInteger(&bufferLen);
		UndoBuffer.Offset(bufferLen);	//向后移位到下一对象存储位置
	}
	//存储此次操作记录的详细数据
	buffer.LogPosition();
	buffer.WriteInteger(nPointCounts);
	CBuffer tempBuffer(1000000);	//设置1M缓存，以免频繁分配内存
	for(pPoint=Point.GetFirst(TRUE);pPoint;pPoint=Point.GetNext(TRUE))
	{
		if(Point.IsCursorDeleted())
		{
			nPointCounts++;
			buffer.WriteInteger(pPoint->handle);
			dwObjState=UNDO_DELETE_OBJ;
			buffer.WriteDword(dwObjState);
			if(hashObjBuffer.GetValueAt(pPoint->handle,iPos))
			{
				UndoBuffer.SeekPosition(iPos+4);
				UndoBuffer.ReadInteger(&bufferLen);
				buffer.WriteInteger(bufferLen);
				buffer.Write(UndoBuffer.GetBufferPtr()+iPos+8,bufferLen);
			}
			else
				buffer.WriteInteger(0);
			tempBuffer.ClearContents();
			pPoint->ToBuffer(tempBuffer);
			buffer.WriteInteger(tempBuffer.GetLength());
			buffer.Write(tempBuffer.GetBufferPtr(),tempBuffer.GetLength());
		}
		else if(pPoint->IsUndoModified())
		{
			nPointCounts++;
			buffer.WriteInteger(pPoint->handle);
			if(hashObjBuffer.GetValueAt(pPoint->handle,iPos))
				dwObjState=UNDO_MODIFY_OBJ;
			else
				dwObjState=UNDO_INSERT_OBJ;
			buffer.WriteDword(dwObjState);
			if(hashObjBuffer.GetValueAt(pPoint->handle,iPos))
			{
				UndoBuffer.SeekPosition(iPos+4);
				UndoBuffer.ReadInteger(&bufferLen);
				buffer.WriteInteger(bufferLen);
				buffer.Write(UndoBuffer.GetBufferPtr()+iPos+8,bufferLen);
			}
			else
				buffer.WriteInteger(0);
			tempBuffer.ClearContents();
			pPoint->ToBuffer(tempBuffer);
			buffer.WriteInteger(tempBuffer.GetLength());
			buffer.Write(tempBuffer.GetBufferPtr(),tempBuffer.GetLength());
		}
	}
	buffer.RecallPosition();
	buffer.WriteInteger(nPointCounts);
	buffer.RecallPosition();

	buffer.LogPosition();
	buffer.WriteInteger(nPointCounts);
	for(pLine=Line.GetFirst(TRUE);pLine;pLine=Line.GetNext(TRUE))
	{
		if(Line.IsCursorDeleted())
		{
			nLineCounts++;
			buffer.WriteInteger(pLine->handle);
			dwObjState=UNDO_DELETE_OBJ;
			buffer.WriteDword(dwObjState);
			if(hashObjBuffer.GetValueAt(pLine->handle,iPos))
			{
				UndoBuffer.SeekPosition(iPos+4);
				UndoBuffer.ReadInteger(&bufferLen);
				buffer.WriteInteger(bufferLen);
				buffer.Write(UndoBuffer.GetBufferPtr()+iPos+8,bufferLen);
			}
			else
				buffer.WriteInteger(0);
			tempBuffer.ClearContents();
			pLine->ToBuffer(tempBuffer);
			buffer.WriteInteger(tempBuffer.GetLength());
			buffer.Write(tempBuffer.GetBufferPtr(),tempBuffer.GetLength());
		}
		else if(pLine->IsUndoModified())
		{
			nLineCounts++;
			buffer.WriteInteger(pLine->handle);
			if(hashObjBuffer.GetValueAt(pLine->handle,iPos))
				dwObjState=UNDO_MODIFY_OBJ;
			else
				dwObjState=UNDO_INSERT_OBJ;
			buffer.WriteDword(dwObjState);
			if(hashObjBuffer.GetValueAt(pLine->handle,iPos))
			{
				UndoBuffer.SeekPosition(iPos+4);
				UndoBuffer.ReadInteger(&bufferLen);
				buffer.WriteInteger(bufferLen);
				buffer.Write(UndoBuffer.GetBufferPtr()+iPos+8,bufferLen);
			}
			else
				buffer.WriteInteger(0);
			tempBuffer.ClearContents();
			pLine->ToBuffer(tempBuffer);
			buffer.WriteInteger(tempBuffer.GetLength());
			buffer.Write(tempBuffer.GetBufferPtr(),tempBuffer.GetLength());
		}
	}
	buffer.RecallPosition();
	buffer.WriteInteger(nLineCounts);
	buffer.RecallPosition();

	buffer.LogPosition();
	buffer.WriteInteger(nPlaneCounts);
	for(pPlane=Plane.GetFirst(TRUE);pPlane;pPlane=Plane.GetNext(TRUE))
	{
		if(Plane.IsCursorDeleted())
		{
			nPlaneCounts++;
			buffer.WriteInteger(pPlane->handle);
			dwObjState=UNDO_DELETE_OBJ;
			buffer.WriteDword(dwObjState);
			if(hashObjBuffer.GetValueAt(pPlane->handle,iPos))
			{
				UndoBuffer.SeekPosition(iPos+4);
				UndoBuffer.ReadInteger(&bufferLen);
				buffer.WriteInteger(bufferLen);
				buffer.Write(UndoBuffer.GetBufferPtr()+iPos+8,bufferLen);
			}
			else
				buffer.WriteInteger(0);
			tempBuffer.ClearContents();
			pPlane->ToBuffer(tempBuffer);
			buffer.WriteInteger(tempBuffer.GetLength());
			buffer.Write(tempBuffer.GetBufferPtr(),tempBuffer.GetLength());
		}
		else if(pPlane->IsUndoModified())
		{
			nPlaneCounts++;
			buffer.WriteInteger(pPlane->handle);
			if(hashObjBuffer.GetValueAt(pPlane->handle,iPos))
				dwObjState=UNDO_MODIFY_OBJ;
			else
				dwObjState=UNDO_INSERT_OBJ;
			buffer.WriteDword(dwObjState);
			if(hashObjBuffer.GetValueAt(pPlane->handle,iPos))
			{
				UndoBuffer.SeekPosition(iPos+4);
				UndoBuffer.ReadInteger(&bufferLen);
				buffer.WriteInteger(bufferLen);
				buffer.Write(UndoBuffer.GetBufferPtr()+iPos+8,bufferLen);
			}
			else
				buffer.WriteInteger(0);
			tempBuffer.ClearContents();
			pPlane->ToBuffer(tempBuffer);
			buffer.WriteInteger(tempBuffer.GetLength());
			buffer.Write(tempBuffer.GetBufferPtr(),tempBuffer.GetLength());
		}
	}
	buffer.RecallPosition();
	buffer.WriteInteger(nPlaneCounts);
	buffer.RecallPosition();

	buffer.LogPosition();
	buffer.WriteInteger(nNodeCounts);
	for(pNode=Node.GetFirst(TRUE);pNode;pNode=Node.GetNext(TRUE))
	{
		if(Node.IsCursorDeleted())
		{
			nNodeCounts++;
			buffer.WriteInteger(pNode->handle);
			dwObjState=UNDO_DELETE_OBJ;
			buffer.WriteDword(dwObjState);
			if(hashObjBuffer.GetValueAt(pNode->handle,iPos))
			{
				UndoBuffer.SeekPosition(iPos+4);
				UndoBuffer.ReadInteger(&bufferLen);
				buffer.WriteInteger(bufferLen);
				buffer.Write(UndoBuffer.GetBufferPtr()+iPos+8,bufferLen);
			}
			else
				buffer.WriteInteger(0);
			tempBuffer.ClearContents();
			pNode->ToBuffer(tempBuffer);
			buffer.WriteInteger(tempBuffer.GetLength());
			buffer.Write(tempBuffer.GetBufferPtr(),tempBuffer.GetLength());
		}
		else if(pNode->IsUndoModified())
		{
			nNodeCounts++;
			buffer.WriteInteger(pNode->handle);
			if(hashObjBuffer.GetValueAt(pNode->handle,iPos))
				dwObjState=UNDO_MODIFY_OBJ;
			else
				dwObjState=UNDO_INSERT_OBJ;
			buffer.WriteDword(dwObjState);
			if(hashObjBuffer.GetValueAt(pNode->handle,iPos))
			{
				UndoBuffer.SeekPosition(iPos+4);
				UndoBuffer.ReadInteger(&bufferLen);
				buffer.WriteInteger(bufferLen);
				buffer.Write(UndoBuffer.GetBufferPtr()+iPos+8,bufferLen);
			}
			else
				buffer.WriteInteger(0);
			tempBuffer.ClearContents();
			pNode->ToBuffer(tempBuffer);
			buffer.WriteInteger(tempBuffer.GetLength());
			buffer.Write(tempBuffer.GetBufferPtr(),tempBuffer.GetLength());
		}
	}
	buffer.RecallPosition();
	buffer.WriteInteger(nNodeCounts);
	buffer.RecallPosition();

	buffer.LogPosition();
	buffer.WriteInteger(nPartCounts);
	for(pPart=Parts.GetFirst(0,TRUE);pPart;pPart=Parts.GetNext(0,TRUE))
	{
		if(Parts.IsCursorDeleted())
		{
			nPartCounts++;
			buffer.WriteInteger(pPart->handle);
			buffer.WriteInteger(pPart->GetClassTypeId());
			dwObjState=UNDO_DELETE_OBJ;
			buffer.WriteDword(dwObjState);
			if(hashObjBuffer.GetValueAt(pPart->handle,iPos))
			{
				UndoBuffer.SeekPosition(iPos+4);
				UndoBuffer.ReadInteger(&bufferLen);
				buffer.WriteInteger(bufferLen);
				buffer.Write(UndoBuffer.GetBufferPtr()+iPos+8,bufferLen);
			}
			else
				buffer.WriteInteger(0);
			tempBuffer.ClearContents();
			pPart->ToBuffer(tempBuffer);
			buffer.WriteInteger(tempBuffer.GetLength());
			buffer.Write(tempBuffer.GetBufferPtr(),tempBuffer.GetLength());
		}
		else if(pPart->IsUndoModified())
		{
			nPartCounts++;
			buffer.WriteInteger(pPart->handle);
			buffer.WriteInteger(pPart->GetClassTypeId());
			if(hashObjBuffer.GetValueAt(pPart->handle,iPos))
				dwObjState=UNDO_MODIFY_OBJ;
			else
				dwObjState=UNDO_INSERT_OBJ;
			buffer.WriteDword(dwObjState);
			if(hashObjBuffer.GetValueAt(pPart->handle,iPos))
			{
				UndoBuffer.SeekPosition(iPos+4);
				UndoBuffer.ReadInteger(&bufferLen);
				buffer.WriteInteger(bufferLen);
				buffer.Write(UndoBuffer.GetBufferPtr()+iPos+8,bufferLen);
			}
			else
				buffer.WriteInteger(0);
			tempBuffer.ClearContents();
			pPart->ToBuffer(tempBuffer);
			buffer.WriteInteger(tempBuffer.GetLength());
			buffer.Write(tempBuffer.GetBufferPtr(),tempBuffer.GetLength());
		}
	}
	buffer.RecallPosition();
	buffer.WriteInteger(nPartCounts);
	buffer.RecallPosition();
/*#ifdef __LDS_
	buffer.LogPosition();
	buffer.WriteInteger(nWindSegCounts);
	for(pWindSeg=WindLoadSeg.GetFirst(TRUE);pWindSeg;pWindSeg=WindLoadSeg.GetNext(TRUE))
	{
		if(WindLoadSeg.IsCursorDeleted())
		{
			nWindSegCounts++;
			buffer.WriteInteger(pWindSeg->handle);
			dwObjState=UNDO_DELETE_OBJ;
			buffer.WriteDword(dwObjState);
			if(hashObjBuffer.GetValueAt(pWindSeg->handle,iPos))
			{
				UndoBuffer.SeekPosition(iPos+4);
				UndoBuffer.ReadInteger(&bufferLen);
				buffer.WriteInteger(bufferLen);
				buffer.Write(UndoBuffer.GetBufferPtr()+iPos+8,bufferLen);
			}
			else
				buffer.WriteInteger(0);
			CBuffer tempBuffer;
			pWindSeg->ToBuffer(tempBuffer);
			buffer.WriteInteger(tempBuffer.GetLength());
			buffer.Write(tempBuffer.GetBufferPtr(),tempBuffer.GetLength());
		}
		else if(pWindSeg->IsUndoModified())
		{
			nWindSegCounts++;
			buffer.WriteInteger(pWindSeg->handle);
			if(hashObjBuffer.GetValueAt(pWindSeg->handle,iPos))
				dwObjState=UNDO_MODIFY_OBJ;
			else
				dwObjState=UNDO_INSERT_OBJ;
			buffer.WriteDword(dwObjState);
			if(hashObjBuffer.GetValueAt(pWindSeg->handle,iPos))
			{
				UndoBuffer.SeekPosition(iPos+4);
				UndoBuffer.ReadInteger(&bufferLen);
				buffer.WriteInteger(bufferLen);
				buffer.Write(UndoBuffer.GetBufferPtr()+iPos+8,bufferLen);
			}
			else
				buffer.WriteInteger(0);
			CBuffer tempBuffer;
			pWindSeg->ToBuffer(tempBuffer);
			buffer.WriteInteger(tempBuffer.GetLength());
			buffer.Write(tempBuffer.GetBufferPtr(),tempBuffer.GetLength());
		}
	}
	buffer.RecallPosition();
	buffer.WriteInteger(nWindSegCounts);
#endif*/
	if(nPointCounts+nLineCounts+nPlaneCounts+nNodeCounts+nPartCounts>0)
		UndoBufferStack.push(buffer.GetBufferPtr(),buffer.GetLength(),iOperType);
	Point.Clean();
	Line.Clean();
	Plane.Clean();
	Node.Clean();
	Parts.Clean();
	return true;
}
// 执行Undo操作
void CTower::UndoTower(void)
{
	//获取Undo操作记录数据内存
	long nBufferSize = UndoBufferStack.UndoRecordLength();
	if(nBufferSize<=0)
		return;	//防止在无Undo/Redo内容,但界面来不及更新时点击了Undo/Redo导致错误 wjh-2011.8.9
	char *pBuffer = new char[nBufferSize];
	UndoBufferStack.popUndo(pBuffer);
	CBuffer buffer(nBufferSize);
	buffer.Write(pBuffer,nBufferSize);
	delete []pBuffer;

	CLDSPoint *pPoint;
	CLDSLine *pLine;
	CLDSPlane *pPlane;
	CLDSNode *pNode;
	CLDSPart *pPart;
	long i=0,nPointCounts=0,nLineCounts=0,nPlaneCounts=0,nNodeCounts=0,nPartCounts=0;
	long h,idClsType;
	DWORD dwObjState=0;
	//预生成需要新添加的对象，以用于互相之间的关联匹配
	buffer.SeekToBegin();
	buffer.ReadInteger(&nPointCounts);
	for(i=0;i<nPointCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		//移位Undo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ&&nBufferSize>0)
		{
			pPoint=Point.append(FALSE);
			pPoint->handle=h;
		}
		//移位Redo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	buffer.ReadInteger(&nLineCounts);
	for(i=0;i<nLineCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		//移位Undo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ&&nBufferSize>0)
		{
			pLine=Line.append(FALSE);
			pLine->handle=h;
		}
		//移位Redo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	buffer.ReadInteger(&nPlaneCounts);
	for(i=0;i<nPlaneCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		//移位Undo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ&&nBufferSize>0)
		{
			pPlane=Plane.append(FALSE);
			pPlane->handle=h;
		}
		//移位Redo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	buffer.ReadInteger(&nNodeCounts);
	for(i=0;i<nNodeCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		//移位Undo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ&&nBufferSize>0)
		{
			pNode=Node.append(FALSE);
			pNode->handle=h;
			//DispNodeSet.append(pNode);
		}
		//移位Redo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	buffer.ReadInteger(&nPartCounts);
	for(i=0;i<nPartCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadInteger(&idClsType);
		buffer.ReadDword(&dwObjState);
		//移位Undo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ&&nBufferSize>0)
		{
			pPart=Parts.append(idClsType,FALSE);
			pPart->handle=h;
			//CLDSObject::_console->AddToDispSet(pPart);//DispPartSet.append
		}
		//移位Redo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	ReBuildObjsHashTable();
	//根据实际情况刷新数据
	CBuffer tempBuffer(1000000);	//设置1M缓存，以免频繁分配内存
	buffer.SeekToBegin();
	buffer.ReadInteger(&nPointCounts);
	for(i=0;i<nPointCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		buffer.ReadInteger(&nBufferSize);
		
		if(dwObjState==UNDO_INSERT_OBJ)
		{
			buffer.Offset(nBufferSize);	//移位Undo部分
			Point.DeleteNode(h);
		}
		else if((dwObjState==UNDO_DELETE_OBJ||dwObjState==UNDO_MODIFY_OBJ)&&nBufferSize>0)
		{
			char *data=new char[nBufferSize];
			buffer.Read(data,nBufferSize);
			tempBuffer.ClearContents();
			tempBuffer.Write(data,nBufferSize);
			delete []data;
			
			tempBuffer.SeekToBegin();
			pPoint=Point.FromHandle(h);
			pPoint->FromBuffer(tempBuffer,0);
		}
		//移位Redo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	buffer.ReadInteger(&nLineCounts);
	for(i=0;i<nLineCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		buffer.ReadInteger(&nBufferSize);
		
		if(dwObjState==UNDO_INSERT_OBJ)
		{
			buffer.Offset(nBufferSize);	//移位Undo部分
			Line.DeleteNode(h);
		}
		else if((dwObjState==UNDO_DELETE_OBJ||dwObjState==UNDO_MODIFY_OBJ)&&nBufferSize>0)
		{
			char *data=new char[nBufferSize];
			buffer.Read(data,nBufferSize);
			tempBuffer.ClearContents();
			tempBuffer.Write(data,nBufferSize);
			delete []data;
			
			tempBuffer.SeekToBegin();
			pLine=Line.FromHandle(h);
			pLine->FromBuffer(tempBuffer);
		}
		//移位Redo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	buffer.ReadInteger(&nPlaneCounts);
	for(i=0;i<nPlaneCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		buffer.ReadInteger(&nBufferSize);
		
		if(dwObjState==UNDO_INSERT_OBJ)
		{
			buffer.Offset(nBufferSize);	//移位Undo部分
			Plane.DeleteNode(h);
		}
		else if((dwObjState==UNDO_DELETE_OBJ||dwObjState==UNDO_MODIFY_OBJ)&&nBufferSize>0)
		{
			char *data=new char[nBufferSize];
			buffer.Read(data,nBufferSize);
			tempBuffer.ClearContents();
			tempBuffer.Write(data,nBufferSize);
			delete []data;
			
			tempBuffer.SeekToBegin();
			pPlane=Plane.FromHandle(h);
			pPlane->FromBuffer(tempBuffer);
		}
		//移位Redo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	buffer.ReadInteger(&nNodeCounts);
	for(i=0;i<nNodeCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		buffer.ReadInteger(&nBufferSize);
		
		if(dwObjState==UNDO_INSERT_OBJ)
		{
			buffer.Offset(nBufferSize);	//移位Undo部分
			CLDSObject::_console->GetDispNodeSet().DeleteNode(h);//DispNodeSet.DeleteNode(h);
			Node.DeleteNode(h);
		}
		else if((dwObjState==UNDO_DELETE_OBJ||dwObjState==UNDO_MODIFY_OBJ)&&nBufferSize>0)
		{
			char *data=new char[nBufferSize];
			buffer.Read(data,nBufferSize);
			tempBuffer.ClearContents();
			tempBuffer.Write(data,nBufferSize);
			delete []data;
			
			tempBuffer.SeekToBegin();
			pNode=Node.FromHandle(h);
			//有时h合法，但pNode被删除为空，导致死机 wjh-2012.2.19
			pNode->FromBuffer(tempBuffer);
		}
		//移位Redo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	buffer.ReadInteger(&nPartCounts);
	for(i=0;i<nPartCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadInteger(&idClsType);
		buffer.ReadDword(&dwObjState);
		buffer.ReadInteger(&nBufferSize);
		
		if(dwObjState==UNDO_INSERT_OBJ)
		{
			buffer.Offset(nBufferSize);	//移位Undo部分
			CLDSObject::_console->GetDispPartSet().DeleteNode(h);//DispPartSet.DeleteNode(h);
			Parts.DeleteNode(h);
		}
		else if((dwObjState==UNDO_DELETE_OBJ||dwObjState==UNDO_MODIFY_OBJ)&&nBufferSize>0)
		{
			char *data=new char[nBufferSize];
			buffer.Read(data,nBufferSize);
			tempBuffer.ClearContents();
			tempBuffer.Write(data,nBufferSize);
			delete []data;

			tempBuffer.SeekToBegin();
			pPart=Parts.FromHandle(h);
			pPart->FromBuffer(tempBuffer);
		}
		//移位Redo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	CLDSObject::_console->GetDispPartSet().Clean();
}
// 执行Redo操作
void CTower::RedoTower(void)
{
	//获取Redo操作记录数据内存
	long nBufferSize=UndoBufferStack.RedoRecordLength(); 
	if(nBufferSize<=0)
		return;	//防止在无Undo/Redo内容,但界面来不及更新时点击了Undo/Redo导致错误 wjh-2011.8.9
	char *pBuffer = new char[nBufferSize];
	UndoBufferStack.popRedo(pBuffer);
	CBuffer buffer(nBufferSize);
	buffer.Write(pBuffer,nBufferSize);
	delete []pBuffer;

	CLDSPoint *pPoint;
	CLDSLine *pLine;
	CLDSPlane *pPlane;
	CLDSNode *pNode;
	CLDSPart *pPart;
	long i=0,nPointCounts=0,nLineCounts=0,nPlaneCounts=0,nNodeCounts=0,nPartCounts=0;
	long h,idClsType;
	DWORD dwObjState=0;
	//预生成需要新添加的对象，以用于互相之间的关联匹配
	buffer.SeekToBegin();
	buffer.ReadInteger(&nPointCounts);
	for(i=0;i<nPointCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		//移位Undo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		//移位Redo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		if(dwObjState==UNDO_INSERT_OBJ&&nBufferSize>0)
		{
			pPoint=Point.append(FALSE);
			pPoint->handle=h;
		}
	}
	buffer.ReadInteger(&nLineCounts);
	for(i=0;i<nLineCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		//移位Undo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		//移位Redo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		if(dwObjState==UNDO_INSERT_OBJ&&nBufferSize>0)
		{
			pLine=Line.append(FALSE);
			pLine->handle=h;
		}
	}
	buffer.ReadInteger(&nPlaneCounts);
	for(i=0;i<nPlaneCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		//移位Undo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		//移位Redo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		if(dwObjState==UNDO_INSERT_OBJ&&nBufferSize>0)
		{
			pPlane=Plane.append(FALSE);
			pPlane->handle=h;
		}
	}
	buffer.ReadInteger(&nNodeCounts);
	for(i=0;i<nNodeCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		//移位Undo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		//移位Redo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		if(dwObjState==UNDO_INSERT_OBJ&&nBufferSize>0)
		{
			pNode=Node.append(FALSE);
			pNode->handle=h;
			//CLDSObject::_console->AddToDispSet(pNode);
		}
	}
	buffer.ReadInteger(&nPartCounts);
	for(i=0;i<nPartCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadInteger(&idClsType);
		buffer.ReadDword(&dwObjState);
		//移位Undo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		//移位Redo部分
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		if(dwObjState==UNDO_INSERT_OBJ&&nBufferSize>0)
		{
			pPart=Parts.append(idClsType,FALSE);
			pPart->handle=h;
			//CLDSObject::_console->AddToDispSet(pPart);
		}
	}
	ReBuildObjsHashTable();
	//根据实际情况刷新数据
	buffer.SeekToBegin();
	buffer.ReadInteger(&nPointCounts);
	for(i=0;i<nPointCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);	//移位Undo部分
		
		buffer.ReadInteger(&nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ)
		{
			Point.DeleteNode(h);
			buffer.Offset(nBufferSize);	//移位Redo部分
		}
		else if((dwObjState==UNDO_INSERT_OBJ||dwObjState==UNDO_MODIFY_OBJ)&&nBufferSize>0)
		{
			pPoint=Point.FromHandle(h);
			pPoint->FromBuffer(buffer,0);
		}
	}
	buffer.ReadInteger(&nLineCounts);
	for(i=0;i<nLineCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);	//移位Undo部分
		
		buffer.ReadInteger(&nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ)
		{
			Line.DeleteNode(h);
			buffer.Offset(nBufferSize);	//移位Redo部分
		}
		else if((dwObjState==UNDO_INSERT_OBJ||dwObjState==UNDO_MODIFY_OBJ)&&nBufferSize>0)
		{
			pLine=Line.FromHandle(h);
			pLine->FromBuffer(buffer);
		}
	}
	buffer.ReadInteger(&nPlaneCounts);
	for(i=0;i<nPlaneCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);	//移位Undo部分
		
		buffer.ReadInteger(&nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ)
		{
			Plane.DeleteNode(h);
			buffer.Offset(nBufferSize);	//移位Redo部分
		}
		else if((dwObjState==UNDO_INSERT_OBJ||dwObjState==UNDO_MODIFY_OBJ)&&nBufferSize>0)
		{
			pPlane=Plane.FromHandle(h);
			pPlane->FromBuffer(buffer);
		}
	}
	buffer.ReadInteger(&nNodeCounts);
	for(i=0;i<nNodeCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);	//移位Undo部分
		
		buffer.ReadInteger(&nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ)
		{
			CLDSObject::_console->GetDispNodeSet().DeleteNode(h);
			Node.DeleteNode(h);
			buffer.Offset(nBufferSize);	//移位Redo部分
		}
		else if((dwObjState==UNDO_INSERT_OBJ||dwObjState==UNDO_MODIFY_OBJ)&&nBufferSize>0)
		{
			pNode=Node.FromHandle(h);
			pNode->FromBuffer(buffer);
		}
	}
	buffer.ReadInteger(&nPartCounts);
	for(i=0;i<nPartCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadInteger(&idClsType);
		buffer.ReadDword(&dwObjState);
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);	//移位Undo部分
		
		buffer.ReadInteger(&nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ)
		{
			CLDSObject::_console->GetDispPartSet().DeleteNode(h);//DispPartSet.DeleteNode(h);
			Parts.DeleteNode(h);
			buffer.Offset(nBufferSize);	//移位Redo部分
		}
		else if((dwObjState==UNDO_INSERT_OBJ||dwObjState==UNDO_MODIFY_OBJ)&&nBufferSize>0)
		{
			pPart=Parts.FromHandle(h,idClsType);
			pPart->FromBuffer(buffer);
		}
	}
	CLDSObject::_console->GetDispPartSet().Clean();
}

// 可以执行Undo操作
BOOL CTower::CanUndoOper(void)
{
	if(UndoBufferStack.UndoRecordLength()>=0)
		return TRUE;
	else
		return FALSE;
}
// 可以执行Redo操作
BOOL CTower::CanRedoOper(void)
{
	if(UndoBufferStack.RedoRecordLength()>=0)
		return TRUE;
	else
		return FALSE;
}
