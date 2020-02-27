#include "stdafx.h"
#include "stdio.h"
#include "OriginalData.h"
#include "env_def.h"
#include "TowerType.h"
#include "SortFunc.h"
#include "LDS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


////////////////////////////////////////////////////////
/*IMPLEMENT_GET(ANGLEOFFSET,ciOffsetWing)
{	//ƫ��֫,0.X֫;1.Y֫;2.���
	return (cbWingOffsetMode&0x0f);
}
IMPLEMENT_SET(ANGLEOFFSET,ciOffsetWing)
{
	cbWingOffsetMode=(cbWingOffsetMode&0xf0)+(value&0x0f);
}
IMPLEMENT_GET(ANGLEOFFSET,ciStyleOfG)
{	//��ƫ��׼������,0.g;1.g1;2.g2;3.g3;4.ָ��ciWingOffset
	return (cbWingOffsetMode&0xf0);
}
IMPLEMENT_SET(ANGLEOFFSET,ciStyleOfG)
{
	cbWingOffsetMode=cbWingOffsetMode&0x0f+(value<<4);
}*/
////////////////////////////////////////////////////////
IMPLEMENT_GET(CDataCheckModel::PART_DATA::BOLT_COUNT,nNailCount)
{
	DWORD n=(count&0x00ff);
	return (BYTE)n;
}
IMPLEMENT_SET(CDataCheckModel::PART_DATA::BOLT_COUNT,nNailCount)
{
	DWORD n=(value&0xff);
	count=count&0xff00+n;
}
IMPLEMENT_GET(CDataCheckModel::PART_DATA::BOLT_COUNT,nEarthCount)
{
	DWORD n=(count&0x0f00);
	return (BYTE)(n>>8);
}
IMPLEMENT_SET(CDataCheckModel::PART_DATA::BOLT_COUNT,nEarthCount)
{
	DWORD n=(value&0x0f);
	count=count&0xf0ff+(n<<8);
}
IMPLEMENT_GET(CDataCheckModel::PART_DATA::BOLT_COUNT,nAssemCount)
{
	DWORD n=(count&0xf000);
	return (BYTE)(n>>12);
}
IMPLEMENT_SET(CDataCheckModel::PART_DATA::BOLT_COUNT,nAssemCount)
{
	DWORD n=(value&0x0f);
	count=count&0xf0ff+(n<<12);
}
IMPLEMENT_GET(CDataCheckModel::PART_DATA::BOLT_COUNT,nSignMountCount)
{
	DWORD n=(count&0x0f0000);
	return (BYTE)(n>>16);
}
IMPLEMENT_SET(CDataCheckModel::PART_DATA::BOLT_COUNT,nSignMountCount)
{
	WORD n=(value&0x0f);
	count=count&0xf0ff+(n<<16);
}
IMPLEMENT_GET(CDataCheckModel::PART_DATA::BOLT_COUNT,nWireCount)
{
	DWORD n=(count&0xf00000);
	return (BYTE)(n>>20);
}
IMPLEMENT_SET(CDataCheckModel::PART_DATA::BOLT_COUNT,nWireCount)
{
	DWORD n=(value&0x0f);
	count=count&0xff0f+(n<<20);
}
IMPLEMENT_GET(CDataCheckModel::SIZE_DATA,idStr)
{
	CXhChar50 keyStr=CodingIdStr(ciSizeType,from,to);
	return keyStr;
}
IMPLEMENT_SET(CDataCheckModel::SIZE_DATA,ciSizeType)
{
	BYTE ciType=value&0x0f;
	_ciSizeType=(ciSizeType&0xf0)|ciType;
}
IMPLEMENT_GET(CDataCheckModel::SIZE_DATA,ciSizeType)
{
	return _ciSizeType&0x0f;
}
IMPLEMENT_SET(CDataCheckModel::SIZE_DATA,cbSizeDimFlag)	//0.����; 0x01:X����; 0x02:Y����; 0x04:Z����
{
	BYTE cbDimFlag=value<<4;
	_ciSizeType=(_ciSizeType&0x0f)|cbDimFlag;
}
IMPLEMENT_GET(CDataCheckModel::SIZE_DATA,cbSizeDimFlag)	//0.����; 0x01:X����; 0x02:Y����; 0x04:Z����
{
	BYTE cbDimFlag=_ciSizeType&0xf0;
	return cbDimFlag>>4;
}
CDataCheckModel::SIZE_DATA::SIZE_DATA(DWORD id/*=0*/)
{
	m_id=id;
	_ciSizeType=ciClassifyType=0;
	tolerance=0.5;
	memset(layer4,0,4);
}
CXhChar50 CDataCheckModel::SIZE_DATA::CodingIdStr(BYTE ciSizeType,const KEYPOINT& from,const KEYPOINT& to)
{	//�������:<���ͺ�:�����-�յ���>��<���ͺ�:���>
	CXhChar50 keystr("%d",ciSizeType);
	//0.�ؼ�������;1.�ؼ����ߴ�;2.��������;3.��������;4.�׾�;5.����׾�(����,����);6.�ؼ��㵽������С����;7.������������С����
	if(ciSizeType==0)
	{
		keystr.Append(CXhChar16("%d",to.ciPointType),'#');
		if(to.ciPointType==0)	//�Ǹֹؼ���
		{
			keystr.Append(CXhChar16(".%X",to.angle.hAngle));
			keystr.Append(CXhChar16("-%X",to.angle.hNode));
			keystr.Append(CXhChar16("%d%d%d!%d!%d",to.angle.offset.ciOffsetPlane,
				to.angle.offset.ciOffsetWing,to.angle.offset.ciStyleOfG,to.angle.offset.wiWingOffset,
				to.angle.offset.wiLenOffset));
		}
		else if(to.ciPointType==1)//1.�����ֲ������
		{
			keystr.Append(CXhChar16(".%X",to.coord.hPart));
			keystr.Append(CXhChar16("@%d-%d-%d",ftoi(to.coord.x),ftoi(to.coord.y),ftoi(to.coord.z)));
		}
		else //if(to.ciPointType==2)//2.��λ��
		{
			keystr.Append(CXhChar16(".%X",to.hole.hDatumPart));
			keystr.Append(CXhChar16("-%X",to.hole.hBolt));
		}
	}
	else if(ciSizeType==1)	//�ؼ���ߴ�
	{
		CXhChar50 fromstr=CodingIdStr(0,from,to),tostr=CodingIdStr(0,to,from);
		keystr.Printf("1#%s:%s",(char*)fromstr+2,(char*)tostr+2);
	}
	//else if(
	return keystr;
}
CDataCheckModel xDataCheckModel;
CDataCheckModel::CDataCheckModel()
{

}
CDataCheckModel::~CDataCheckModel()
{

}
void CDataCheckModel::FromBuffer(CBuffer& attach_buffer)
{
	int i,ver=0,nPartNum=0,nSizeItemNum=0;
	attach_buffer.ReadInteger(&ver);
	attach_buffer.ReadInteger(&nPartNum);
	m_xHashPartCheck.Empty();
	for(i=0;i<nPartNum;i++)
	{
		CXhChar16 sKey;
		attach_buffer.ReadString(sKey);
		PART_CHECK_ITEM* pPartItem=AddPartCheckItem(sKey);
		attach_buffer.ReadDword(&pPartItem->m_dwFlag);
		pPartItem->drawing.m_sPartNo=sKey;
		attach_buffer.ReadByte(&pPartItem->drawing.ciPartType);		//��������
		attach_buffer.ReadInteger(&pPartItem->drawing.iSeg.iSeg);	//�����ֶκ�
		attach_buffer.ReadByte(&pPartItem->drawing.cMaterial);		//����
		attach_buffer.ReadWord(&pPartItem->drawing.nCount);			//������
		attach_buffer.ReadDouble(&pPartItem->drawing.m_fThick);		//���
		attach_buffer.ReadDouble(&pPartItem->drawing.m_fWidth);		//���
		attach_buffer.ReadDouble(&pPartItem->drawing.m_fLength);		//����
		attach_buffer.ReadInteger(&pPartItem->drawing.nM16Bolt.count);	//M16
		attach_buffer.ReadInteger(&pPartItem->drawing.nM20Bolt.count);	//M20
		attach_buffer.ReadInteger(&pPartItem->drawing.nM24Bolt.count);	//M24
		attach_buffer.ReadInteger(&pPartItem->drawing.nElseBolt.count);	//����
		attach_buffer.ReadInteger(&pPartItem->drawing.nSpecialHole.count);	//����
		attach_buffer.ReadString(pPartItem->sNotes);
	}
	attach_buffer.ReadInteger(&nSizeItemNum);
	hashSizeItems.Empty();
	for(i=0;i<nSizeItemNum;i++)
	{
		DWORD id=0;
		attach_buffer.ReadDword(&id);
		SIZE_DATA* pSizeItem=hashSizeItems.Add(id);
		attach_buffer.ReadString(pSizeItem->m_sKey);
		attach_buffer.ReadByte(&pSizeItem->_ciSizeType);
		attach_buffer.ReadByte(&pSizeItem->ciClassifyType);
		attach_buffer.Read(&pSizeItem->dist,sizeof(SIZE_DATA::DISTANCE));
		attach_buffer.Read(&pSizeItem->from,sizeof(KEYPOINT));
		attach_buffer.Read(&pSizeItem->to,sizeof(KEYPOINT));
		//ͳ����ʾ��Ϣ
		attach_buffer.WriteInteger(pSizeItem->iSeg.iSeg);
		attach_buffer.Write(pSizeItem->layer4,4);
		attach_buffer.WriteFloat(pSizeItem->tolerance);	//��ʾ���
		//��ע��Ϣ
		attach_buffer.WriteByte(pSizeItem->dimension.ciDimensionType);
		attach_buffer.WriteWord(pSizeItem->dimension.siUpOffset);
	}
}
void CDataCheckModel::ToBuffer(CBuffer& attach_buffer)
{
	//����У������
	int nPartNum=GetPartCheckNum(TRUE);
	attach_buffer.WriteInteger(0);	//�汾��
	attach_buffer.WriteInteger(nPartNum);
	for(PART_CHECK_ITEM* pPartItem=EnumFirPartItem();pPartItem;pPartItem=EnumFirPartItem())
	{
		if(pPartItem->m_dwFlag<=0)
			continue;
		attach_buffer.WriteString(pPartItem->m_sKey);
		attach_buffer.WriteDword(pPartItem->m_dwFlag);
		pPartItem->drawing.m_sPartNo=pPartItem->m_sKey;
		attach_buffer.WriteByte(pPartItem->drawing.ciPartType);		//��������
		attach_buffer.WriteInteger(pPartItem->drawing.iSeg.iSeg);	//�����ֶκ�
		attach_buffer.WriteByte(pPartItem->drawing.cMaterial);		//����
		attach_buffer.WriteWord(pPartItem->drawing.nCount);			//������
		attach_buffer.WriteDouble(pPartItem->drawing.m_fThick);		//���
		attach_buffer.WriteDouble(pPartItem->drawing.m_fWidth);		//���
		attach_buffer.WriteDouble(pPartItem->drawing.m_fLength);		//����
		attach_buffer.WriteInteger(pPartItem->drawing.nM16Bolt);	//M16
		attach_buffer.WriteInteger(pPartItem->drawing.nM20Bolt);	//M20
		attach_buffer.WriteInteger(pPartItem->drawing.nM24Bolt);	//M24
		attach_buffer.WriteInteger(pPartItem->drawing.nElseBolt);	//����
		attach_buffer.WriteInteger(pPartItem->drawing.nSpecialHole);	//����
		attach_buffer.WriteString(pPartItem->sNotes);
	}
	attach_buffer.WriteInteger(hashSizeItems.GetNodeNum());
	for(SIZE_DATA* pSizeItem=EnumFirstCoordSizeItem();pSizeItem;pSizeItem=EnumNextCoordSizeItem())
	{
		attach_buffer.WriteDword(pSizeItem->Id);
		attach_buffer.WriteString(pSizeItem->idStr);
		attach_buffer.WriteByte(pSizeItem->_ciSizeType);
		attach_buffer.WriteByte(pSizeItem->ciClassifyType);
		attach_buffer.Write(&pSizeItem->dist,sizeof(SIZE_DATA::DISTANCE));
		attach_buffer.Write(&pSizeItem->from,sizeof(KEYPOINT));
		attach_buffer.Write(&pSizeItem->to,sizeof(KEYPOINT));
		//ͳ����ʾ��Ϣ
		attach_buffer.WriteInteger(pSizeItem->iSeg.iSeg);
		attach_buffer.Write(pSizeItem->layer4,4);
		attach_buffer.WriteFloat(pSizeItem->tolerance);	//��ʾ���
		//��ע��Ϣ
		attach_buffer.WriteByte(pSizeItem->dimension.ciDimensionType);
		attach_buffer.WriteWord(pSizeItem->dimension.siUpOffset);
	}
}
//���ݵ�ǰ���߻�ȡ����ڵ㼯��
void CDataCheckModel::GetOriginalObjectSet()
{
	CLDSModule* pModule=Ta.GetActiveModule();
	if(pModule==NULL)
		return;
	LINEPARTSET sum_partset;
	NODESET sum_nodeset;
	pModule->RefreshModule(sum_nodeset,sum_partset);
	//��ȡ������ͼ��ѡ��������ͼ�е������ڵ�
	CDisplayView *pFrontView=NULL;
	for(CDisplayView *pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
	{
		if(pView->m_iViewType==0&&stricmp(pView->defaultLayer,"SPQ")==0&&pView->filter.ThirdCharSet[0]!='?')
			pFrontView=pView;
		if(pFrontView)
			break;
	}
	for(CLDSNode *pNode=sum_nodeset.GetFirst();pNode;pNode=sum_nodeset.GetNext())
	{
		if(!pNode->cfgword.IsHasBodyNoOnly(pModule->GetBodyNo()))	//ѡ������ڸ�ģ�͵Ľڵ�
			continue;
		if(pFrontView->filter.IsValidLayer(pNode->layer()))
			nodeSet.append(pNode);
	}
	for(CLDSLinePart *pLinePart=sum_partset.GetFirst();pLinePart;pLinePart=sum_partset.GetNext())
	{
		if(!pLinePart->cfgword.IsHasBodyNoOnly(pModule->GetBodyNo()))	
			continue;
		if(IsMatch(pLinePart->layer(2),pFrontView->filter.ThirdCharSet))
			partSet.append(pLinePart);
	}
}
struct SLOP_NODE_COUPLE{CLDSNodePtr pStart,pEnd;};
void CDataCheckModel::ChooseFeatPt(NODESET &selNodeSet,int iType)
{
	if(nodeSet.GetNodeNum()<=0)
		return;
	CLDSNode* pNode,*pTemNode;
	CLDSNode *pLowestNode = nodeSet.GetFirst();
	CLDSNode *pHighestNode = nodeSet.GetFirst();
	if(iType==SIZE_SECTION_Z||iType==SIZE_GUAXIAN_SPACE_Z)
	{	//�ڼ䴹��
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{
			if(pNode->Position('Z',false)>pLowestNode->Position('Z',false))
				pLowestNode = pNode;
			if(iType==SIZE_SECTION_Z&&!pNode->IsUdfCoordZ())
				continue;	//�������û����������Ľڵ�
			else if(iType==SIZE_GUAXIAN_SPACE_Z&&(pNode->m_cPosCalType!=0||toupper(pNode->layer(2))!='Q'))
				continue;	//����Ϊ�ᵣ���ߵ㶼��ǰ����ƽڵ�
			for(pTemNode=selNodeSet.GetFirst();pTemNode;pTemNode=selNodeSet.GetNext())
			{
				if(pNode->Position('Z',false)==pTemNode->Position('Z',false))
				{
					if(pNode->Position('X',false)<pTemNode->Position('X',false))
						pTemNode=selNodeSet.SetCursorValue(pNode);
					break;
				}
			}
			if(pTemNode==NULL)
				pTemNode=selNodeSet.append(pNode);
		}
		if(iType==SIZE_GUAXIAN_SPACE_Z)
			selNodeSet.append(pLowestNode);
	}
	else if(iType==SIZE_SEG_SPACE_Z)
	{	//��ͷ������ֶμ��
		CHashSet<CLDSNodePtr> hashSegNodes;
		double min_shank_z=0;
		bool init_shank_z=false;
		CLDSNode *pShankTopNode=NULL;
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{	//���������߽ڵ�
			if(pNode->Position('Z',false)>pLowestNode->Position('Z',false))
				pLowestNode = pNode;
			if(pNode->Position('Z',false)<pHighestNode->Position('Z',false))
				pHighestNode = pNode;
			if(isdigit(pNode->layer(2))!=0&&toupper(pNode->layer(0))!='T')
			{
				if(pShankTopNode==NULL)
					pShankTopNode=pNode;
				else if(pNode->Position('Z',false)<pShankTopNode->Position('Z',false))
					pShankTopNode=pNode;
			}
		}
		for(CSuperSmartPtr<CLDSPart> pPart=partSet.GetFirst();pPart.IsHasPtr();pPart=partSet.GetNext())
		{
			if(pPart->layer(0)=='T' || !pPart->IsLinePart() || pPart.LinePartPointer()->pStart==NULL ||pPart.LinePartPointer()->pEnd==NULL)
				continue;
			pNode=NULL;
			if( isdigit(pPart.LinePartPointer()->pStart->layer(2))!=0&&toupper(pPart.LinePartPointer()->pStart->layer(0))!='T')
				pNode=pPart.LinePartPointer()->pStart;
			if((isdigit(pPart.LinePartPointer()->pEnd->layer(2))!=0&&toupper(pPart.LinePartPointer()->pEnd->layer(0))!='T')&&
				(pNode==NULL||(pPart.LinePartPointer()->pEnd&&pNode->Position('Z',false)<pPart.LinePartPointer()->pEnd->Position('Z',false))))
				pNode=pPart.LinePartPointer()->pEnd;
			if(pNode==NULL)
				continue;
			for(pTemNode=hashSegNodes.GetFirst();pTemNode;pTemNode=hashSegNodes.GetNext())
			{	
				if(pPart->iSeg.iSeg == hashSegNodes.GetCursorKey())
				{	//δ���ǽڵ�ֻ�������¶���֮һ��������»�ȡ�ڵ㲻�Ƿֽ�㡡
					if(pNode->Position('Z',false)>pTemNode->Position('Z',false)||
						(pNode->Position('Z',false)==pTemNode->Position('Z',false)&&pNode->Position('X',false)<pTemNode->Position('X',false)))
						pTemNode=hashSegNodes.SetValue(pPart->iSeg.iSeg,pNode);
					break;
				}
			}
			if(pTemNode==NULL)
			{
				if(!init_shank_z&&pNode->layer(0)!='T')
					min_shank_z=pNode->Position('Z',false);
				else if(init_shank_z&&pNode->layer(0)!='T'&&min_shank_z>pNode->Position('Z',false))
				{
					min_shank_z=pNode->Position('Z',false);
					init_shank_z=true;
				}
				pTemNode=hashSegNodes.SetValue(pPart->iSeg.iSeg,pNode);
			}
		}
		if(pShankTopNode==NULL)
			return;
		if(pShankTopNode->Position('Z',false)>pHighestNode->Position('Z',false))
			selNodeSet.append(pHighestNode);
		if(min_shank_z>pShankTopNode->Position('Z',false))
			selNodeSet.append(pShankTopNode);
		for(pTemNode=hashSegNodes.GetFirst();pTemNode;pTemNode=hashSegNodes.GetNext())
			selNodeSet.append(pTemNode);
	}
	else if(iType==SIZE_TOWER_HEIGHT)
	{	//����(��ͷ+����)
		double min_shank_z=0;
		CLDSNode *pShankTopNode=NULL;
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{	//���������߽ڵ�
			if(pNode->Position('Z',false)>pLowestNode->Position('Z',false))
				pLowestNode = pNode;
			if(pNode->Position('Z',false)<pHighestNode->Position('Z',false))
				pHighestNode = pNode;
			if(!isdigit(pNode->layer(2))||toupper(pNode->layer(0))!='S')
				continue;
			if(pShankTopNode==NULL)
			{
				min_shank_z=pNode->Position('Z',false);
				pShankTopNode=pNode;
			}
			else if(min_shank_z>pNode->Position('Z',false))
			{
				min_shank_z=pNode->Position('Z',false);
				pShankTopNode=pNode;
			}
		}
		if(min_shank_z>pHighestNode->Position('Z',false))
			selNodeSet.append(pHighestNode);
		selNodeSet.append(pShankTopNode);
		selNodeSet.append(pLowestNode);
	}
	else if(iType==SIZE_SHANK_SPACE_XY)
	{	//�����¶ο��ڳߴ�
		double max_shank_z=0;
		ATOM_LIST<SLOP_NODE_COUPLE> listSlopeOpen;
		SLOP_NODE_COUPLE dimBtmCouple, *pCouple;
		dimBtmCouple.pStart=dimBtmCouple.pEnd=NULL;
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{
			if(dimBtmCouple.pStart==NULL)
				dimBtmCouple.pStart=dimBtmCouple.pEnd=pNode;
			else if(dimBtmCouple.pStart->Position('Z',false)<pNode->Position('Z',false))
				dimBtmCouple.pStart=dimBtmCouple.pEnd=pNode;
			else if(dimBtmCouple.pStart->Position('Z',false)==pNode->Position('Z',false))
			{
				if(pNode->Position('X',false)<dimBtmCouple.pStart->Position('X',false))
					dimBtmCouple.pStart=pNode;
				if(pNode->Position('X',false)>dimBtmCouple.pEnd->Position('X',false))
					dimBtmCouple.pEnd=pNode;
			}
			if (pNode->m_cPosCalType!=0||!isdigit(pNode->layer(2))||toupper(pNode->layer(0))=='T')
				continue;	//������¿ڽڵ����Ϊ���ƽڵ㣬
			for(pCouple=listSlopeOpen.GetFirst();pCouple;pCouple=listSlopeOpen.GetNext())
			{	
				if(pNode->Position('Z',false)==pCouple->pStart->Position('Z',false))
				{
					if(pNode->Position('X',false)<pCouple->pStart->Position('X',false))
						pCouple->pStart=pNode;
					if(pNode->Position('X',false)>pCouple->pEnd->Position('X',false))
						pCouple->pEnd=pNode;
					break;
				}
			}
			if(pCouple==NULL)
			{
				if(listSlopeOpen.GetNodeNum()==0)
					max_shank_z=pNode->Position('Z',false);
				else if(max_shank_z<pNode->Position('Z',false))
					max_shank_z=pNode->Position('Z',false);
				pCouple=listSlopeOpen.append();
				pCouple->pStart=pCouple->pEnd=pNode;
			}
		}
		for(pCouple=listSlopeOpen.GetFirst();pCouple;pCouple=listSlopeOpen.GetNext())
		{
			if(pCouple->pStart!=pCouple->pEnd)
			{
				selNodeSet.append(pCouple->pStart);
				selNodeSet.append(pCouple->pEnd);
			}
		}
		if(max_shank_z<dimBtmCouple.pStart->Position('Z',false))
		{
			selNodeSet.append(dimBtmCouple.pStart);
			selNodeSet.append(dimBtmCouple.pEnd);
		}
	}
	else if(iType==SIZE_GUAXIAN_SPACE_XY)
	{	//��ͷ(�����ߵ�)ˮƽ����ע
		for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
		{
			if(pNode->Position('X',false)<EPS)
				continue;	//�������ϵĽڵ�
			if(toupper(pNode->layer(0))=='T'&&pNode->IsUdfCoordX())
				selNodeSet.append(pNode);	//��ͷ(�����ߵ�)ˮƽ����ע
		}
	}
}
CDataCheckModel::SIZE_DATA* CDataCheckModel::AppendCoordSizeItem(const char* idStr)
{
	SIZE_DATA* pStrKeyItem=idStr!=NULL?GetCoordSizeItem(idStr):NULL;
	if(pStrKeyItem!=NULL)
		return pStrKeyItem;
	SIZE_DATA* pItem=hashSizeItems.Add(0);
	if(idStr!=NULL&&idStr[0]!=0)
		hashKeyStrSizeItems.SetValue(idStr,pItem);
	return pItem;
}
void CDataCheckModel::ExtractCoordCheckItems()
{
	/*GetOriginalObjectSet();
	//��ȡ�ڵ�����У����

	//��ȡ�ڵ��ߴ�У����
	for(int i=0;i<6;i++)
	{
		NODESET nodeSetTemp;
		ChooseFeatPt(nodeSetTemp,i);
		if(nodeSetTemp.GetNodeNum()==0)
			continue;
		CLDSNode* pNode;
		DYN_ARRAY<CLDSNodePtr>arrDimNodes(nodeSetTemp.GetNodeNum());
		UINT j=0;
		for(pNode=nodeSetTemp.GetFirst();pNode;pNode=nodeSetTemp.GetNext(),j++)
			arrDimNodes[j]=pNode;
		if(nodeSetTemp.GetNodeNum()<=2&&i<3)
			continue;
		if(i==SIZE_SHANK_SPACE_XY)	//�¶ο��ڰ�nodeSetTemp�ɶ����ɳߴ��ע
		{	
			for(j=0;j+1<arrDimNodes.Size();j+=2)
			{
				COORD* pCoord=m_hashControlData.Add("");
				pCoord->ciCoordType=COORD_TWO_NODE_SIZE;
				pCoord->ciTNodeSizeType=i;
				pCoord->m_hNode1=arrDimNodes[j]->handle;
				pCoord->m_hNode2=arrDimNodes[j+1]->handle;
			}
		}
		else
		{	
			for(j=0;j<arrDimNodes.Size()-1;j++)
			{
				COORD* pCoord=m_hashControlData.Add("");
				pCoord->ciCoordType=COORD_TWO_NODE_SIZE;
				pCoord->ciTNodeSizeType=i;
				pCoord->m_hNode1=arrDimNodes[j]->handle;
				pCoord->m_hNode2=arrDimNodes[j+1]->handle;
			}
		}
	}
	//��ȡ���������ͻ�������
	*/
}
void CDataCheckModel::ExtractPartCheckItems()
{
	//У����Ŀ wjh-2017.1.16
	/*
		1.����PART_DATAУ����Ŀ 
		2.������Ƴߴ�У����Ŀ
		3.��������˨��׼�϶
		4.���⹤�տױ��漰�Գ���У��
		5.���߸˼����ӵ�Ԥ�������⽻�ӵ㱨��
		6.���������϶��ƫ��׼�౨��
		7.���߿״�С�������ס��ӵؿס�����ס����ƿױ����嵥���쳣���
		8.Ԥ���߶ȱ���(���ؼ����ƽڵ�ĵ��߼�ʵ��Ԥ���߶�)���쳣��ʾ
		9.���߰弰���߰屨�漰��Բ������
		10.��ߴ���
		11.�ְ�߾�(��ͨ�ְ�\���ְ�\���)�쳣��ʾ��Ϣ
		12.�нǾ�ʾ��Ϣ
		13.���ָ˼������仯����˨����δ�涯
		14.�Ǹ��������嵥(�ҳ��ص�˼��嵥)
		15.���߰�ת�Ƕ���������������漰�ҿ׶���
		16.��ϽǸֱ�ż���϶�Ŵ�ֵ
		17.�Ȳ���ֵ
		18.����㱨��
	*/
	//��ȡ����У����
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Extract Part CheckItem Propcess��");
#else 
	DisplayProcess(0,"��ȡ��������У������ȣ�");
#endif
	int i=0,n=0;
	CTowerType mat_tbl;
	mat_tbl.RunSummarize(&Ta);
	mat_tbl.SortPart(0);
	n=mat_tbl.part_recordset.GetNodeNum();
	for(PART* pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
	{
		i++;
	#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((100*i)/n,"Extract Part CheckItem Propcess��");
#else 
		DisplayProcess((100*i)/n,"��ȡ��������У������ȣ�");
#endif
		CXhChar16 sPartNo(pRec->GetPartNo());
#ifndef __ENABLE_XHSTRING_LENGTH_PROPERTY_
		if(sPartNo.Length()<=0)
#else
		if(sPartNo.Length<=0)
#endif
			continue;
		PART_CHECK_ITEM* pItem=AddPartCheckItem(sPartNo);
		strcpy(pItem->model.m_sPartNo,sPartNo);
		pItem->model.iSeg=pRec->iSeg;
		pItem->model.ciPartType=pRec->part_type;
		pItem->model.cMaterial=pRec->cMaterial;
		pItem->model.nCount=(WORD)pRec->GetPartNum();
		pItem->model.m_fThick=pRec->thick;
		pItem->model.m_fWidth=pRec->wide;
		pItem->model.m_fLength=pRec->length;
		pItem->model.nM16Bolt=pRec->nM16Ls;
		pItem->model.nM20Bolt=pRec->nM20Ls;
		pItem->model.nM24Bolt=pRec->nM24Ls;
		pItem->model.nElseBolt=pRec->nMXLs;
		pItem->SetDrawingDefaultData();
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Extract Part CheckItem Propcess��");
#else 
	DisplayProcess(100,"��ȡ��������У������ȣ�");
#endif
}
int CDataCheckModel::GetPartCheckNum(BOOL bConfirm/*=FALSE*/)
{
	int nSum=m_xHashPartCheck.GetNodeNum();
	int nConfirmItem=0;
	for(PART_CHECK_ITEM* pItem=EnumFirPartItem();pItem;pItem=EnumNextPartItem())
	{
		if(pItem->m_dwFlag>0)
			nConfirmItem++;
	}
	if(bConfirm==FALSE)
		return nSum-nConfirmItem;
	else
		return nConfirmItem; 
}
CDataCheckModel::PART_CHECK_ITEM* CDataCheckModel::AddPartCheckItem(const char* sPartNo)
{
	PART_CHECK_ITEM* pCheckItem=m_xHashPartCheck.GetValue(sPartNo);
	if(pCheckItem==NULL)
	{
		pCheckItem=m_xHashPartCheck.Add(sPartNo);
		strcpy(pCheckItem->m_sKey,sPartNo);
	}
	return pCheckItem;
}
//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_GET(CDataCheckModel::PART_CHECK_ITEM,StateMessage)
{
	CXhChar16 message;
	BITFLAG flag(m_dwFlag);
	if(m_dwFlag==0)
		return message="";	//δȷ���κι���У����Ϣ
	else
	{
		BYTE successed=0,failed=0,suspend=0;
		if(!flag.IsHasFlag(PART_DATA::PROP_MATERIAL))	//����
			suspend++;
		else
		{
			if(model.cMaterial==drawing.cMaterial)
				successed++;
			else
				failed++;
		}
		if(!flag.IsHasFlag(PART_DATA::PROP_SIZE))	//���
			suspend++;
		else
		{
			if((drawing.ciPartType==PART_DATA::TYPE_PLATE&&fabs(model.m_fThick-drawing.m_fThick)<EPS)||
				fabs(model.m_fWidth-drawing.m_fWidth)+fabs(model.m_fThick-drawing.m_fThick)<EPS)
				successed++;
			else
				failed++;
		}
		if(!flag.IsHasFlag(PART_DATA::PROP_LENGTH))	//����
			suspend++;
		else
		{
			if(drawing.ciPartType!=PART_DATA::TYPE_ANGLE||fabs(model.m_fLength-drawing.m_fLength)<EPS)
				successed++;
			else
				failed++;
		}
		if(!flag.IsHasFlag(PART_DATA::PROP_COUNT))	//������
			suspend++;
		else
		{
			if(model.nCount==drawing.nCount)
				successed++;
			else
				failed++;
		}
		if(!flag.IsHasFlag(PART_DATA::PROP_M16_NX))	//M16��˨�����ڽǸ�X֫�ϣ�
			suspend++;
		else
		{
			if((drawing.ciPartType==PART_DATA::TYPE_ANGLE&&model.nM16Bolt.countOfWingX==drawing.nM16Bolt.countOfWingX)||
				model.nM16Bolt.count==drawing.nM16Bolt.count)
				successed++;
			else
				failed++;
		}
		if(!flag.IsHasFlag(PART_DATA::PROP_M16_NY))	//M16��˨�����ڽǸ�Y֫�ϣ�
			suspend++;
		else
		{
			if(drawing.ciPartType!=PART_DATA::TYPE_ANGLE||model.nM16Bolt.countOfWingY==drawing.nM16Bolt.countOfWingY)
				successed++;
			else
				failed++;
		}
		if(!flag.IsHasFlag(PART_DATA::PROP_M20_NX))	//M20��˨�����ڽǸ�X֫�ϣ�
			suspend++;
		else
		{
			if((drawing.ciPartType==PART_DATA::TYPE_ANGLE&&model.nM20Bolt.countOfWingX==drawing.nM20Bolt.countOfWingX)||
				model.nM20Bolt.count==drawing.nM20Bolt.count)
				successed++;
			else
				failed++;
		}
		if(!flag.IsHasFlag(PART_DATA::PROP_M20_NY))	//M20��˨�����ڽǸ�Y֫�ϣ�
			suspend++;
		else
		{
			if(drawing.ciPartType!=PART_DATA::TYPE_ANGLE||model.nM20Bolt.countOfWingY==drawing.nM20Bolt.countOfWingY)
				successed++;
			else
				failed++;
		}
		if(!flag.IsHasFlag(PART_DATA::PROP_M24_NX))	//M24��˨�����ڽǸ�X֫�ϣ�
			suspend++;
		else
		{
			if((drawing.ciPartType==PART_DATA::TYPE_ANGLE&&model.nM24Bolt.countOfWingX==drawing.nM24Bolt.countOfWingX)||
				model.nM24Bolt.count==drawing.nM24Bolt.count)
				successed++;
			else
				failed++;
		}
		if(!flag.IsHasFlag(PART_DATA::PROP_M24_NY))	//M24��˨�����ڽǸ�Y֫�ϣ�
			suspend++;
		else
		{
			if(drawing.ciPartType!=PART_DATA::TYPE_ANGLE||model.nM24Bolt.countOfWingY==drawing.nM24Bolt.countOfWingY)
				successed++;
			else
				failed++;
		}
		if(!flag.IsHasFlag(PART_DATA::PROP_MX_NX))	//Mxx��˨�����ڽǸ�X֫�ϣ�
			suspend++;
		else
		{
			if((drawing.ciPartType==PART_DATA::TYPE_ANGLE&&model.nElseBolt.countOfWingX==drawing.nElseBolt.countOfWingX)||
				model.nElseBolt.count==drawing.nElseBolt.count)
				successed++;
			else
				failed++;
		}
		if(!flag.IsHasFlag(PART_DATA::PROP_MX_NY))	//Mxx��˨�����ڽǸ�Y֫�ϣ�
			suspend++;
		else
		{
			if(drawing.ciPartType!=PART_DATA::TYPE_ANGLE||model.nElseBolt.countOfWingY==drawing.nElseBolt.countOfWingY)
				successed++;
			else
				failed++;
		}
		if(!flag.IsHasFlag(PART_DATA::PROP_MS_N))	//�������
			suspend++;
		else
		{
			if( model.nSpecialHole.nNailCount==drawing.nSpecialHole.nNailCount&&
				model.nSpecialHole.nWireCount==drawing.nSpecialHole.nWireCount&&
				model.nSpecialHole.nAssemCount==drawing.nSpecialHole.nAssemCount&&
				model.nSpecialHole.nEarthCount==drawing.nSpecialHole.nEarthCount&&
				model.nSpecialHole.nSignMountCount==drawing.nSpecialHole.nSignMountCount)
				successed++;
			else
				failed++;
		}
		if(failed==0&&suspend==0)
			message="��";
		else
		{
			if(failed>0)
				message.Printf("��%d",failed);
			if(suspend>0)
			{
#ifndef __ENABLE_XHSTRING_LENGTH_PROPERTY_
				if(message.Length()>0)
#else
				if(message.Length>0)
#endif
					message.Append(' ');
				message.Append(CXhChar16("?%d",suspend));
			}
		}
	}
	return message;
}
void CDataCheckModel::PART_CHECK_ITEM::RefreshLoftPartInfo()
{
	CLDSPart* pPart=(CLDSPart*)Ta.FromPartNo(m_sKey);
	if(pPart==NULL)
		return;
	model.cMaterial=pPart->cMaterial;
	model.m_fThick=pPart->GetThick();
	model.m_fWidth=pPart->GetWidth();
}
void CDataCheckModel::PART_CHECK_ITEM::SetDrawingDefaultData()
{
	BITFLAG flag(m_dwFlag);
	drawing.m_sPartNo=model.m_sPartNo;
	if(!flag.IsHasFlag(PART_DATA::PROP_MATERIAL))	//����
		drawing.cMaterial=model.cMaterial;
	if(!flag.IsHasFlag(PART_DATA::PROP_SIZE))	//���
	{
		drawing.m_fWidth=model.m_fWidth;
		drawing.m_fThick=model.m_fThick;
	}
	if(!flag.IsHasFlag(PART_DATA::PROP_LENGTH))	//����
		drawing.m_fLength=model.m_fLength;
	if(!flag.IsHasFlag(PART_DATA::PROP_COUNT))	//������
		drawing.nCount=model.nCount;
	if(!flag.IsHasFlag(PART_DATA::PROP_M16_NX))	//M16��˨�����ڽǸ�X֫�ϣ�
		drawing.nM16Bolt.countOfWingX=model.nM16Bolt.countOfWingX;
	if(!flag.IsHasFlag(PART_DATA::PROP_M16_NY))	//M16��˨�����ڽǸ�Y֫�ϣ�
		drawing.nM16Bolt.countOfWingY=model.nM16Bolt.countOfWingY;
	if(!flag.IsHasFlag(PART_DATA::PROP_M20_NX))	//M20��˨�����ڽǸ�X֫�ϣ�
		drawing.nM20Bolt.countOfWingX=model.nM20Bolt.countOfWingX;
	if(!flag.IsHasFlag(PART_DATA::PROP_M20_NY))	//M20��˨�����ڽǸ�Y֫�ϣ�
		drawing.nM20Bolt.countOfWingY=model.nM20Bolt.countOfWingY;
	if(!flag.IsHasFlag(PART_DATA::PROP_M24_NX))	//M24��˨�����ڽǸ�X֫�ϣ�
		drawing.nM24Bolt.countOfWingX=model.nM24Bolt.countOfWingX;
	if(!flag.IsHasFlag(PART_DATA::PROP_M24_NY))	//M24��˨�����ڽǸ�Y֫�ϣ�
		drawing.nM24Bolt.countOfWingY=model.nM24Bolt.countOfWingY;
	if(!flag.IsHasFlag(PART_DATA::PROP_MX_NX))	//Mxx��˨�����ڽǸ�X֫�ϣ�
		drawing.nElseBolt.countOfWingX=model.nElseBolt.countOfWingX;
	if(!flag.IsHasFlag(PART_DATA::PROP_MX_NY))	//Mxx��˨�����ڽǸ�Y֫�ϣ�
		drawing.nElseBolt.countOfWingY=model.nElseBolt.countOfWingY;
	if(!flag.IsHasFlag(PART_DATA::PROP_MS_N))	//�������
		drawing.nSpecialHole=model.nSpecialHole;
}
CXhChar16 CDataCheckModel::PART_CHECK_ITEM::GetMatStr(BOOL bLoft/*=TRUE*/)
{
	CXhChar16 sMaterial;
	if(bLoft)
		QuerySteelMatMark(model.cMaterial,sMaterial);
	else
		QuerySteelMatMark(drawing.cMaterial,sMaterial);
	return sMaterial;
}
CXhChar16 CDataCheckModel::PART_CHECK_ITEM::GetGuige(BOOL bLoft/*=TRUE*/)
{
	PART_DATA* pData=bLoft ? &model : &drawing;
	if(model.ciPartType==PART_DATA::TYPE_PLATE)
		return CXhChar16("-%g",pData->m_fThick);
	else
		return CXhChar16("%gx%g",pData->m_fWidth,pData->m_fThick);
}
