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
		m_bObjectStart=true;	//��Ч������UNDO����
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
		m_bObjectStart=true;	//��Ч������UNDO����
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
// ��ʼ����Undo����
void CTower::BeginUndoListen(void)
{
	if(m_bGroupUndoStart)
		return;	//�����ó���Undo���ܣ��м�����е�BeginUndoListen��EndUndoListen��Ч
	CBuffer buffer;	//Ĭ��1k�����ռ䣬����̫����Ϊÿ��������Ҫ�ͷźͷ����ڴ�
	UndoBuffer.ClearBuffer();
	buffer.SetBlockSize(1000000);	//��Լ1M�����ռ�
	UndoBuffer.SetBlockSize(1000000);	// ��Լ1M�����ռ�
#ifdef __LDS_
	int nn=Point.GetNodeNum()+Line.GetNodeNum()+Plane.GetNodeNum()+Node.GetNodeNum()+Parts.GetNodeNum();//+WindLoadSeg.GetNodeNum();
#else
	int nn=Point.GetNodeNum()+Line.GetNodeNum()+Plane.GetNodeNum()+Node.GetNodeNum()+Parts.GetNodeNum();
#endif
	UndoBuffer.WriteInteger(nn);	// �����������
	for(CLDSPoint *pPoint=Point.GetFirst();pPoint;pPoint=Point.GetNext())
	{
		buffer.ClearContents();
		pPoint->ToBuffer(buffer);
		UndoBuffer.WriteInteger(pPoint->handle);
		UndoBuffer.WriteInteger(buffer.GetLength());
		UndoBuffer.Write(buffer.GetBufferPtr(),buffer.GetLength());
		pPoint->ResetUndoModifiedFlag();	// ��������޸ı��
	}
	for(CLDSLine *pLine=Line.GetFirst();pLine;pLine=Line.GetNext())
	{
		buffer.ClearContents();
		pLine->ToBuffer(buffer);
		UndoBuffer.WriteInteger(pLine->handle);
		UndoBuffer.WriteInteger(buffer.GetLength());
		UndoBuffer.Write(buffer.GetBufferPtr(),buffer.GetLength());
		pLine->ResetUndoModifiedFlag();	// ��������޸ı��
	}
	for(CLDSPlane *pPlane=Plane.GetFirst();pPlane;pPlane=Plane.GetNext())
	{
		buffer.ClearContents();
		pPlane->ToBuffer(buffer);
		UndoBuffer.WriteInteger(pPlane->handle);
		UndoBuffer.WriteInteger(buffer.GetLength());
		UndoBuffer.Write(buffer.GetBufferPtr(),buffer.GetLength());
		pPlane->ResetUndoModifiedFlag();	// ��������޸ı��
	}
	for(CLDSNode *pNode=Node.GetFirst();pNode;pNode=Node.GetNext())
	{
		buffer.ClearContents();
		pNode->ToBuffer(buffer);
		UndoBuffer.WriteInteger(pNode->handle);
		UndoBuffer.WriteInteger(buffer.GetLength());
		UndoBuffer.Write(buffer.GetBufferPtr(),buffer.GetLength());
		pNode->ResetUndoModifiedFlag();	// ��������޸ı��
	}
	for(CLDSPart *pPart=Parts.GetFirst();pPart;pPart=Parts.GetNext())
	{
		buffer.ClearContents();
		pPart->ToBuffer(buffer);
		UndoBuffer.WriteInteger(pPart->handle);
		UndoBuffer.WriteInteger(buffer.GetLength());
		UndoBuffer.Write(buffer.GetBufferPtr(),buffer.GetLength());
		pPart->ResetUndoModifiedFlag();	// �������Undo�޸ı��
	}
/*#ifdef __LDS_
	for(CWindSegment *pWindSeg=WindLoadSeg.GetFirst();pWindSeg;pWindSeg=WindLoadSeg.GetNext())
	{
		buffer.ClearBuffer();
		pWindSeg->ToBuffer(buffer);
		UndoBuffer.WriteInteger(pWindSeg->handle);
		UndoBuffer.WriteInteger(buffer.GetLength());
		UndoBuffer.Write(buffer.GetBufferPtr(),buffer.GetLength());
		pWindSeg->ResetUndoModifiedFlag();	// �������Undo�޸ı��
	}
	WindLoadSeg.Clean();
#endif*/
	Point.Clean();
	Line.Clean();
	Plane.Clean();
	Node.Clean();	// �����ǰɾ���Ľڵ�
	Parts.Clean();	// �����ǰɾ���Ĺ���
}
// ���ص�ǰUndo��¼�Ĳ�������
int CTower::CurrentUndoType()
{
	return UndoBufferStack.UndoRecordType();
}
// �Ƿ����µ�Undo��Ϣ��Ҫ�洢
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
// ��ʼ����Undo����
bool CTower::EndUndoListen(int iOperType/*=0*/)
{
	if(m_bGroupUndoStart)
		return false;	//�����ó���Undo���ܣ��м�����е�BeginUndoListen��EndUndoListen��Ч
	if(!IsHasUndoInfo())
		return false;
	CBuffer buffer(2000000);	//�趨Ϊ1M���ң����ⷴ�������ڴ�
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

	//���޸�ǰ�ĸ���������ϣ���Ա���ѯ
	UndoBuffer.SeekToBegin();
	UndoBuffer.ReadInteger(&nOrgObjNum);
	hashObjBuffer.CreateHashTable(nOrgObjNum+8);	// +8Ϊ�˷�ֹ������Ԫ���������
	for(i=0;i<nOrgObjNum;i++)
	{
		UndoBuffer.ReadInteger(&h);
		hashObjBuffer.SetValueAt(h,UndoBuffer.GetCursorPosition()-4);
		UndoBuffer.ReadInteger(&bufferLen);
		UndoBuffer.Offset(bufferLen);	//�����λ����һ����洢λ��
	}
	//�洢�˴β�����¼����ϸ����
	buffer.LogPosition();
	buffer.WriteInteger(nPointCounts);
	CBuffer tempBuffer(1000000);	//����1M���棬����Ƶ�������ڴ�
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
// ִ��Undo����
void CTower::UndoTower(void)
{
	//��ȡUndo������¼�����ڴ�
	long nBufferSize = UndoBufferStack.UndoRecordLength();
	if(nBufferSize<=0)
		return;	//��ֹ����Undo/Redo����,����������������ʱ�����Undo/Redo���´��� wjh-2011.8.9
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
	//Ԥ������Ҫ����ӵĶ��������ڻ���֮��Ĺ���ƥ��
	buffer.SeekToBegin();
	buffer.ReadInteger(&nPointCounts);
	for(i=0;i<nPointCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		//��λUndo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ&&nBufferSize>0)
		{
			pPoint=Point.append(FALSE);
			pPoint->handle=h;
		}
		//��λRedo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	buffer.ReadInteger(&nLineCounts);
	for(i=0;i<nLineCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		//��λUndo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ&&nBufferSize>0)
		{
			pLine=Line.append(FALSE);
			pLine->handle=h;
		}
		//��λRedo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	buffer.ReadInteger(&nPlaneCounts);
	for(i=0;i<nPlaneCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		//��λUndo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ&&nBufferSize>0)
		{
			pPlane=Plane.append(FALSE);
			pPlane->handle=h;
		}
		//��λRedo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	buffer.ReadInteger(&nNodeCounts);
	for(i=0;i<nNodeCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		//��λUndo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ&&nBufferSize>0)
		{
			pNode=Node.append(FALSE);
			pNode->handle=h;
			//DispNodeSet.append(pNode);
		}
		//��λRedo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	buffer.ReadInteger(&nPartCounts);
	for(i=0;i<nPartCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadInteger(&idClsType);
		buffer.ReadDword(&dwObjState);
		//��λUndo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ&&nBufferSize>0)
		{
			pPart=Parts.append(idClsType,FALSE);
			pPart->handle=h;
			//CLDSObject::_console->AddToDispSet(pPart);//DispPartSet.append
		}
		//��λRedo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	ReBuildObjsHashTable();
	//����ʵ�����ˢ������
	CBuffer tempBuffer(1000000);	//����1M���棬����Ƶ�������ڴ�
	buffer.SeekToBegin();
	buffer.ReadInteger(&nPointCounts);
	for(i=0;i<nPointCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		buffer.ReadInteger(&nBufferSize);
		
		if(dwObjState==UNDO_INSERT_OBJ)
		{
			buffer.Offset(nBufferSize);	//��λUndo����
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
		//��λRedo����
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
			buffer.Offset(nBufferSize);	//��λUndo����
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
		//��λRedo����
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
			buffer.Offset(nBufferSize);	//��λUndo����
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
		//��λRedo����
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
			buffer.Offset(nBufferSize);	//��λUndo����
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
			//��ʱh�Ϸ�����pNode��ɾ��Ϊ�գ��������� wjh-2012.2.19
			pNode->FromBuffer(tempBuffer);
		}
		//��λRedo����
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
			buffer.Offset(nBufferSize);	//��λUndo����
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
		//��λRedo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
	}
	CLDSObject::_console->GetDispPartSet().Clean();
}
// ִ��Redo����
void CTower::RedoTower(void)
{
	//��ȡRedo������¼�����ڴ�
	long nBufferSize=UndoBufferStack.RedoRecordLength(); 
	if(nBufferSize<=0)
		return;	//��ֹ����Undo/Redo����,����������������ʱ�����Undo/Redo���´��� wjh-2011.8.9
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
	//Ԥ������Ҫ����ӵĶ��������ڻ���֮��Ĺ���ƥ��
	buffer.SeekToBegin();
	buffer.ReadInteger(&nPointCounts);
	for(i=0;i<nPointCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		//��λUndo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		//��λRedo����
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
		//��λUndo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		//��λRedo����
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
		//��λUndo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		//��λRedo����
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
		//��λUndo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		//��λRedo����
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
		//��λUndo����
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);
		//��λRedo����
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
	//����ʵ�����ˢ������
	buffer.SeekToBegin();
	buffer.ReadInteger(&nPointCounts);
	for(i=0;i<nPointCounts;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwObjState);
		buffer.ReadInteger(&nBufferSize);
		buffer.Offset(nBufferSize);	//��λUndo����
		
		buffer.ReadInteger(&nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ)
		{
			Point.DeleteNode(h);
			buffer.Offset(nBufferSize);	//��λRedo����
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
		buffer.Offset(nBufferSize);	//��λUndo����
		
		buffer.ReadInteger(&nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ)
		{
			Line.DeleteNode(h);
			buffer.Offset(nBufferSize);	//��λRedo����
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
		buffer.Offset(nBufferSize);	//��λUndo����
		
		buffer.ReadInteger(&nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ)
		{
			Plane.DeleteNode(h);
			buffer.Offset(nBufferSize);	//��λRedo����
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
		buffer.Offset(nBufferSize);	//��λUndo����
		
		buffer.ReadInteger(&nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ)
		{
			CLDSObject::_console->GetDispNodeSet().DeleteNode(h);
			Node.DeleteNode(h);
			buffer.Offset(nBufferSize);	//��λRedo����
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
		buffer.Offset(nBufferSize);	//��λUndo����
		
		buffer.ReadInteger(&nBufferSize);
		if(dwObjState==UNDO_DELETE_OBJ)
		{
			CLDSObject::_console->GetDispPartSet().DeleteNode(h);//DispPartSet.DeleteNode(h);
			Parts.DeleteNode(h);
			buffer.Offset(nBufferSize);	//��λRedo����
		}
		else if((dwObjState==UNDO_INSERT_OBJ||dwObjState==UNDO_MODIFY_OBJ)&&nBufferSize>0)
		{
			pPart=Parts.FromHandle(h,idClsType);
			pPart->FromBuffer(buffer);
		}
	}
	CLDSObject::_console->GetDispPartSet().Clean();
}

// ����ִ��Undo����
BOOL CTower::CanUndoOper(void)
{
	if(UndoBufferStack.UndoRecordLength()>=0)
		return TRUE;
	else
		return FALSE;
}
// ����ִ��Redo����
BOOL CTower::CanRedoOper(void)
{
	if(UndoBufferStack.RedoRecordLength()>=0)
		return TRUE;
	else
		return FALSE;
}
