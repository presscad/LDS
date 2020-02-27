//<LOCALE_TRANSLATE BY hxr /> 2015-1-10
// DefinedView.cpp: implementation of the CDefinedView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tower.h"

/*#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLDSPart *CViewPart::GetPart()
{
	if(pPart==NULL&&m_pModel)
		pPart=m_pModel->FromPartHandle(handle);
	return pPart;
}

//��õ�ǰ���Ʊ�־�ַ����Լ�����ֵ
void CViewPart::GetCurDrawFlagStr(char *sCurFlag,int &iDrawIndex)
{
	CLDSPart *pPart=GetPart();
	if(pPart==NULL)
	{
		if(sCurFlag)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sCurFlag,"Unknown");
#else 
			strcpy(sCurFlag,"δ֪");
#endif
		iDrawIndex=0;
		return;
	}
	char sText[50]="";
	if(pPart->IsLinePart())
	{	
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			if(cFlag=='X')
			{
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"X Leg");
#else 
				strcpy(sText,"X֫");
#endif
				iDrawIndex=0;
			}
			else if(cFlag=='Y')
			{
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"Y Leg");
#else 
				strcpy(sText,"Y֫");
#endif
				iDrawIndex=1;
			}
			else if(cFlag=='Z')
			{
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"Z Section");
#else 
				strcpy(sText,"Z����");
#endif
				iDrawIndex=2;
			}
			else if(cFlag=='S')
			{
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"X Leg Spread Draw");
#else 
				strcpy(sText,"X֫չ������");
#endif
				iDrawIndex=3;
			}
			else if(cFlag=='Q')
			{
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"Y Leg Spread Draw");
#else 
				strcpy(sText,"Y֫չ������");
#endif
				iDrawIndex=4;
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"Judge Automatic");
#else 
				strcpy(sText,"�Զ��ж�");
#endif
				iDrawIndex=5;
			}
		}
		else
		{
			if(cFlag=='X')
			{
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"X Front");
#else 
				strcpy(sText,"X����");
#endif
				iDrawIndex=0;
			}
			else if(cFlag=='Y')
			{
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"Y Front");
#else 
				strcpy(sText,"Y����");
#endif
				iDrawIndex=1;
			}
			else if(cFlag=='Z')
			{
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"Z Section");
#else 
				strcpy(sText,"Z����");
#endif
				iDrawIndex=2;
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"Judge Automatic");
#else 
				strcpy(sText,"�Զ��ж�");
#endif
				iDrawIndex=3;
			}
		}
	}
	else if(pPart->GetClassTypeId()==CLS_PLATE)
	{
		if(cFlag=='X')
		{
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"X Section");
#else 
			strcpy(sText,"X����");
#endif
			iDrawIndex=0;
		}
		else if(cFlag=='Y')
		{
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Y Section");
#else 
			strcpy(sText,"Y����");
#endif
			iDrawIndex=1;
		}
		else if(cFlag=='Z')
		{
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Z Front");
#else 
			strcpy(sText,"Z����");
#endif
			iDrawIndex=2;
		}
		else if(cFlag=='S')
		{
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Spread Draw");
#else 
			strcpy(sText,"չ������");
#endif
			iDrawIndex=3;
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Judge Automatic");
#else 
			strcpy(sText,"�Զ��ж�");
#endif
			iDrawIndex=4;
		}
	}
	else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		if(cFlag=='X')
		{
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"X Section");
#else 
			strcpy(sText,"X����");
#endif
			iDrawIndex=0;
		}
		else if(cFlag=='Y')
		{
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Y Section");
#else 
			strcpy(sText,"Y����");
#endif
			iDrawIndex=1;
		}
		else if(cFlag=='Z')
		{
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Z Front");
#else 
			strcpy(sText,"Z����");
#endif
			iDrawIndex=2;
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Judge Automatic");
#else 
			strcpy(sText,"�Զ��ж�");
#endif
			iDrawIndex=3;
		}
	}
	else 
	{
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(sText,"Judge Automatic");
#else 
		strcpy(sText,"�Զ��ж�");
#endif
		iDrawIndex=0;
	}
	if(sCurFlag)
		strcpy(sCurFlag,sText);
}

//�����������ͼ��־��������ַ����Լ���־�ַ���("XYZSQ")
int CViewPart::GetDrawFlagCmbStrList(CString &sCmbList,char *sDrawFlag)
{
	char sTemp[10]="";
	CLDSPart *pPart=GetPart();
	if(pPart)
	{	
		if(pPart->IsLinePart())
		{	
			if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				strcpy(sTemp,"XYZSQ0");
#ifdef AFX_TARG_ENU_ENGLISH
				sCmbList="X Leg|Y Leg|Z Section|X Leg Spread Draw|Y Leg Spread Draw|Judge Automatic";
#else 
				sCmbList="X֫|Y֫|Z����|X֫չ������|Y֫չ������|�Զ��ж�";
#endif
			}
			else
			{
				strcpy(sTemp,"XYZ0");
#ifdef AFX_TARG_ENU_ENGLISH
				sCmbList="X Front|Y Front|Z Section|Judge Automatic";
#else 
				sCmbList="X����|Y����|Z����|�Զ��ж�";
#endif
			}
		}
		else if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			strcpy(sTemp,"XYZS0");
#ifdef AFX_TARG_ENU_ENGLISH
			sCmbList="X Section|Y Section|Z Front|Spread Draw|Judge Automatic";
#else 
			sCmbList="X����|Y����|Z����|չ������|�Զ��ж�";
#endif
		}
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			strcpy(sTemp,"XYZ0");
#ifdef AFX_TARG_ENU_ENGLISH
			sCmbList="X Section|Y Section|Z Front|Judge Automatic";
#else 
			sCmbList="X����|Y����|Z����|�Զ��ж�";
#endif
		}
		else 
		{
			strcpy(sTemp,"0");
#ifdef AFX_TARG_ENU_ENGLISH
			sCmbList="Judge Automatic";
#else 
			sCmbList="�Զ��ж�";
#endif
		}
		if(sDrawFlag)
			strcpy(sDrawFlag,sTemp);
		return strlen(sTemp);
	}
	else 
#ifdef AFX_TARG_ENU_ENGLISH
		sCmbList="Unknown";
#else 
		sCmbList="δ֪";
#endif
	return 0;
}

//���ݹ�������չ�����ȡ��ת���߼���ת�Ƕ� wht 11-08-08
/*CDrawingAssistant *CViewPart::GetRotateAxisAndAngle(CTower *pTower,f3dPoint draw_norm,
													f3dLine &rotate_axis,double &rotate_angle)
{
	if(pDisplayView==NULL||iResidePlaneNo<=0)
		return NULL;
	if(pAssist==NULL)
	{
		if(pDisplayView->m_iViewType==0)
			pAssist=pDisplayView->GetDrawingAssistByHandle(handle);
		else
			pAssist=pDisplayView->GetDrawingAssistByNo(iResidePlaneNo);
	}
	if(pAssist==NULL)
		return NULL;
	if(pAssist->m_nStyle==0)
		pAssist->GetRotateAxisAndAngle(draw_norm,rotate_axis,rotate_angle,pTower);
	else 
		pAssist=NULL;
	return pAssist;
}*/

//////////////////////////////////////////////////////////////////////////
//
CLayerFilter::CLayerFilter()
{	
	strcpy(FirstCharSet, "?");
	strcpy(SecondCharSet,"?");
	strcpy(ThirdCharSet, "?");
}

void CLayerFilter::SetFilter(char *firstCh,char *secondCh,char *thirdCh)
{	
	strcpy(FirstCharSet, firstCh);
	strcpy(SecondCharSet, secondCh);
	strcpy(ThirdCharSet, thirdCh);
}

BOOL CLayerFilter::IsValidLayer(char *layer)
{
	if(!IsMatch(*layer, FirstCharSet))
		return FALSE;//TRACE("�ַ�����ƥ��\n");
	if(!IsMatch(*(layer+1), SecondCharSet))
		return FALSE;//TRACE("�ַ�����ƥ��\n");
	if(!IsMatch(*(layer+2), ThirdCharSet))
		return FALSE;//TRACE("�ַ�����ƥ��\n");
	return TRUE;
}
CDisplayView::CDisplayView()
{
	int iNo=0;			//��ͼ���
	m_iSeg=-1;
	m_iViewType=m_idSubViewType=0;
	m_nClassTypeId=CLS_DISPLAYVIEW;
	strcpy(defaultLayer,"SPQ");
	//strcpy(m_sClassName,"CDisplayView");
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(name,"View");
#else 
	strcpy(name,"��ͼ");
#endif
	strcpy(folderName,"");
	LoadDefaultUCS(&ucs);
}
CDisplayView::ITEM_TAG_INFO::ITEM_TAG_INFO()
{
	pCurPart=NULL;
}
//��PART_ITEM�Ѿ��йҽӶ���ʱִ��TagInfo(...), ���򴴽��¹ҽ����ݶ���󷵻�
CDisplayView::ITEM_TAG_INFO* CDisplayView::LoadTagInfo(CViewPart* pItem)
{
	if(pItem->pTagInfo!=NULL)
		return (ITEM_TAG_INFO*)pItem->pTagInfo;
	ITEM_TAG_INFO* pTagInfo=listTagInfo.append();
	pItem->pTagInfo=pTagInfo;
	pTagInfo->AttachObject(pItem);
	return pTagInfo;
}
CDisplayView::ITEM_TAG_INFO* TagInfo(CViewPart* pItem)
{
	return (CDisplayView::ITEM_TAG_INFO*)pItem->pTagInfo;
}
CLDSPart* TagPart(CViewPart* pItem)
{
	if(pItem->pTagInfo!=NULL)
		return ((CDisplayView::ITEM_TAG_INFO*)pItem->pTagInfo)->pCurPart;
	return NULL;
}
void CDisplayView::InitHashTable()
{
	if(m_iViewType==0)
		return;
	partsetTable.Empty();
	partsetTable.CreateHashTable(partset.GetNodeNum());
	for(CViewPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		partsetTable.SetValueAt(pPart->handle,pPart);
	assistHashTable.Empty();
	assistHashTable.CreateHashTable(listAssist.GetNodeNum());
	for(CDrawingAssistant *pAssist=listAssist.GetFirst();pAssist;pAssist=listAssist.GetNext())
		assistHashTable.SetValueAt(pAssist->iNo,pAssist);
}
BOOL CDisplayView::IsValidObject(CLDSDbObject *pObj)
{
	if(m_iViewType==0)
		return filter.IsValidLayer(pObj->layer());
	CViewPart *pPart=NULL;
	if(partsetTable.GetHashTableSize()>0&&partsetTable.GetValueAt(pObj->handle,pPart))
		return TRUE;
	else
	{
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(pPart->handle==pObj->handle)
				return TRUE;
		}
		return FALSE;
	}
}
CViewPart *CDisplayView::FindViewPart(long h)
{
	if(m_iViewType!=0&&m_iViewType!=1)
		return NULL;
	return partset.GetValue(h);
	//CViewPart *pPart=NULL;
	//if(partsetTable.GetHashTableSize()>0&&partsetTable.GetValueAt(h,pPart))
	//	return pPart;
	//else
	//{
	//	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	//	{
	//		if(pPart->handle==h)
	//			return pPart;
	//	}
	//	return NULL;
	//}
}
CDrawingAssistant *CDisplayView::GetDrawingAssistByNo(int iNo,SEGI iSeg/* =SEGI */)
{
	if(m_iViewType!=0&&m_iViewType!=1)
		return NULL;
	if(m_iViewType==0&&iSeg.iSeg<=0)
		iSeg=m_iSeg;	//��δ����Ϸ��κ�,�����ͼ����ͼ����ʱ�κ�
	CDrawingAssistant *pAssist=NULL;
	if(assistHashTable.GetHashTableSize()>0&&assistHashTable.GetValueAt(iNo,pAssist))
		return pAssist;
	else
	{
		for(pAssist=listAssist.GetFirst();pAssist;pAssist=listAssist.GetNext())
		{
			if(m_iViewType==0&&pAssist->m_iSeg.iSeg!=iSeg.iSeg)
				continue;	//ͼ����ͼ�и�ͼԪ���ǵ�ǰ�εĸ���ͼԪ
			if(pAssist->iNo==iNo)
				return pAssist;
		}
		return NULL;
	}
}

CDrawingAssistant *CDisplayView::GetDrawingAssistByHandle(long h,SEGI iSeg/* =SEGI */)
{
	if(m_iViewType!=0&&m_iViewType!=1)
		return NULL;
	CViewPart *pViewPart=FindViewPart(h);
	if(iSeg.iSeg==0)
	{
		CLDSPart *pPart=this->m_pModel->FromPartHandle(h);
		if(pPart)
			iSeg=pPart->iSeg;
	}
	if(pViewPart==NULL)
		return NULL;
	CDrawingAssistant *pAssist=NULL;
	if(assistHashTable.GetHashTableSize()>0&&assistHashTable.GetValueAt(pViewPart->iResidePlaneNo,pAssist))
		return pAssist;
	else
	{
		for(pAssist=listAssist.GetFirst();pAssist;pAssist=listAssist.GetNext())
		{
			if(m_iViewType==0&&pAssist->m_iSeg.iSeg!=iSeg.iSeg)
				continue;	//ͼ����ͼ�и�ͼԪ���ǵ�ǰ�εĸ���ͼԪ
			if(pAssist->iNo==pViewPart->iResidePlaneNo)
				return pAssist;
		}
		return NULL;
	}
}

int CDisplayView::GetSpreadFaceCount(int iSeg/*=0*/)
{
	int nCount=0;
	CDrawingAssistant *pAssist=NULL;
	for(pAssist=listAssist.GetFirst();pAssist;pAssist=listAssist.GetNext())
	{
		if(m_iViewType==0&&iSeg>0&&iSeg!=pAssist->m_iSeg.iSeg)
			continue;	//����ͼ����ͼ��ǰ�εĻ�ͼ����ͼԪ
		if(pAssist->m_nStyle==0)
			nCount++;
	}
	return nCount;
}
bool CDisplayView::IsValidSubViewType(int idSubView)
{
	return false;
}
bool CDisplayView::IntelliUpdate(int idSubView)
{
	m_idSubViewType=idSubView;
	if(idSubView==SUBVIEW_HSECT)
	{
	}
	else if(idSubView==SUBVIEW_VFACE)
	{
	}
	else
		m_idSubViewType=0;
	return false;
}

//��������ѡ��Ҫ��ʾ�ļ���
BOOL CDisplayView::SelectDispSet(NODESET& DispNodeSet,PARTSET& DispPartSet,BLOCKREFSET& DispBlockRefSet,FOUNDATIONSET& DispFoundationSet)
{
	DispNodeSet.Empty();
	DispPartSet.Empty();
	DispBlockRefSet.Empty();
	DispFoundationSet.Empty();
	CLDSNode* pNode;
	CLDSPart *pPart;
	if(m_iViewType==0)
	{	//ͼ����ͼ
		BOOL pushed=m_pModel->PushPartStack(); //���������б�ǰӦ��ѹջ wht 12-03-05
		for(pPart=BelongModel()->EnumPartFirst();pPart;pPart=BelongModel()->EnumPartNext())
		{
			pPart->is_visible=TRUE;
			if(filter.IsValidLayer(pPart->layer()))
				DispPartSet.append(pPart);
		}
		m_pModel->PopPartStack(pushed);
		DispNodeSet.Empty();
		for(pNode=BelongModel()->EnumNodeFirst();pNode;pNode=BelongModel()->EnumNodeNext())
		{
			if(filter.IsValidLayer(pNode->layer()))
				DispNodeSet.append(pNode);
			//SuperiorTower()->SelectNodeSet(&SuperiorTower()->DispNodeSet,"�ڵ�","�ַ�1",filter.FirstCharSet,"�ַ�2",
			//filter.SecondCharSet,"�ַ�3",filter.ThirdCharSet,"����");
		}
		if(BelongModel()->IsTowerModel())
		{
			CTower *pTower=(CTower*)BelongModel();
			for(CBlockReference *pBlockRef=pTower->BlockRef.GetFirst();pBlockRef;pBlockRef=pTower->BlockRef.GetNext())
				DispBlockRefSet.append(pBlockRef);
			for(CLDSFoundation *pFoundation=pTower->Foundations.GetFirst();pFoundation;pFoundation=pTower->Foundations.GetNext())
				DispFoundationSet.append(pFoundation);
		}
	}
	else if(m_iViewType==1||m_iViewType==2)
	{	//1.������ͼ 2.��Ϲ���������ͼ��������ͼ������
		//CHashTable<CLDSNode*>hashNodes(SuperiorTower()->Node.GetNodeNum());
		//for(pNode=BelongModel()->EnumNodeFirst();pNode;pNode=BelongModel()->EnumNodeNext())
		//	hashNodes.SetValueAt(pNode->handle,pNode);
		for(CViewPart *pViewPart=partset.GetFirst(); pViewPart; pViewPart=partset.GetNext())
		{
			if(pViewPart->idClassType==CLS_NODE)//&&hashNodes.GetValueAt(pViewPart->handle,pNode))
			{
				pNode=m_pModel->FromNodeHandle(pViewPart->handle);
				if(pNode)
					DispNodeSet.append(pNode);
			}
			else if(pViewPart->idClassType!=CLS_NODE)
			{
				pPart = BelongModel()->FromPartHandle(pViewPart->handle);
				if(pPart)
					DispPartSet.append(pPart);
			}
		}
	}
	for(pNode=DispNodeSet.GetFirst();pNode;pNode=DispNodeSet.GetNext())
		pNode->is_visible=TRUE;
	for(pPart=DispPartSet.GetFirst();pPart;pPart=DispPartSet.GetNext())
		pPart->is_visible=TRUE;
	for(CLDSFoundation *pFoundation=DispFoundationSet.GetFirst();pFoundation;pFoundation=DispFoundationSet.GetNext())
		pFoundation->is_visible=TRUE;
	return TRUE;
}

void CDisplayView::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//����Ӱ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
		return;
	buffer.WriteInteger(dwPermission);
	//buffer.WriteInteger(iNo);
	buffer.WriteInteger(m_iViewType);
	if(version==0||
		(doc_type==1&&version>=5000010)||
		(doc_type==2&&version>=2000005)||
		(doc_type==4&&version>=1030005)||
		(doc_type==5&&version>=1030005))
		buffer.WriteInteger(m_idSubViewType);
	buffer.WriteString(name);
	buffer.WriteString(folderName);
	buffer.WriteString(defaultLayer);
	buffer.WriteString(filter.FirstCharSet);
	buffer.WriteString(filter.SecondCharSet);
	buffer.WriteString(filter.ThirdCharSet);
	if( version==0||
		(doc_type==1&&version>=4020200)||
		(doc_type==2&&version>=1020200)||
		(doc_type==3&&version>=2000200)||
		(doc_type==4&&version>=1020200)||
		(doc_type==5&&version>=1020200))
		des_cs.ToBuffer(buffer,version,doc_type);
	buffer.WritePoint(ucs.origin);
	buffer.WritePoint(ucs.axis_x);
	buffer.WritePoint(ucs.axis_y);
	buffer.WritePoint(ucs.axis_z);
	BUFFERPOP stack(&buffer,listAssist.GetNodeNum());
	buffer.WriteInteger(listAssist.GetNodeNum());
	for(CDrawingAssistant *pAssis=listAssist.GetFirst();pAssis;pAssis=listAssist.GetNext())
	{
		pAssis->ToBuffer(buffer,version,doc_type);
		stack.Increment();
	}
	/*{
		buffer.WriteInteger(pAssis->m_nStyle);
		buffer.WriteInteger(pAssis->iNo);
		buffer.WriteString(pAssis->name);
		if(pAssis->m_nStyle>0)
			buffer.WriteInteger(pAssis->iResidePlaneNo);
		pAssis->m_xLineStart.ToBuffer(buffer);
		if(pAssis->m_nStyle==0)
			pAssis->m_xFaceNorm.ToBuffer(buffer);
		else
			pAssis->m_xLineEnd.ToBuffer(buffer);
	}*/
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of draw assistant is wrong!");
#else
		CLDSObject::Log2File()->Log("��ͼ������¼�����ִ���!");
#endif
	stack.Initialize(&buffer,partset.GetNodeNum());
	buffer.WriteInteger(partset.GetNodeNum());
	for(CViewPart *pViewPart=partset.GetFirst();pViewPart;pViewPart=partset.GetNext())
	{
		buffer.WriteInteger(pViewPart->handle);
		buffer.WriteInteger(pViewPart->idClassType);
		buffer.WriteByte(pViewPart->cFlag);
		buffer.WriteInteger(pViewPart->iResidePlaneNo);
		//if(pViewPart->idClassType==CLS_LINEANGLE&&(pViewPart->cFlag=='S'||pViewPart->cFlag=='Q'))
		//	buffer.WriteInteger(pViewPart->nSpreadLineOffsetDist);	//չ����ƫ���� wht 10-12-10
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of view part is wrong!");
#else
		CLDSObject::Log2File()->Log("��ͼ������¼�����ִ���!");
#endif

}
void CDisplayView::FromCdfBuffer(CBuffer &buffer,long version/*=0*/)
{
	//��ȡӰ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer))
		return;
	buffer.ReadInteger(&dwPermission);
	buffer.ReadInteger(&m_iViewType);
	buffer.ReadInteger(&m_idSubViewType);
	buffer.ReadString(name);
	buffer.ReadString(folderName);
	buffer.ReadString(defaultLayer);
	buffer.ReadString(filter.FirstCharSet);
	buffer.ReadString(filter.SecondCharSet);
	buffer.ReadString(filter.ThirdCharSet);
	des_cs.FromBuffer(buffer,0,PRODUCT_TDA);//version,doc_type);
	buffer.ReadPoint(ucs.origin);
	buffer.ReadPoint(ucs.axis_x);
	buffer.ReadPoint(ucs.axis_y);
	buffer.ReadPoint(ucs.axis_z);
	int i,n=0;
	buffer.ReadInteger(&n);
	listAssist.Empty();
	for(i=0;i<n;i++)
	{
		CDrawingAssistant *pAssis=listAssist.append();
		pAssis->FromBuffer(buffer,0,PRODUCT_TDA);//version,doc_type);
	}
	n=0;
	buffer.ReadInteger(&n);
	partset.Empty();
	for(i=0;i<n;i++)
	{
		long h=0;
		buffer.ReadInteger(&h);
		CViewPart *pViewPart=AppendViewPart(h);
		buffer.ReadInteger(&pViewPart->idClassType);
		buffer.ReadByte(&pViewPart->cFlag);
		buffer.ReadInteger(&pViewPart->iResidePlaneNo);
	}
}
void CDisplayView::ToCdfBuffer(CBuffer &buffer,long version/*=0*/)
{
	//����Ӱ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdToBuffer(buffer))
		return;
	buffer.WriteInteger(dwPermission);
	buffer.WriteInteger(m_iViewType);
	buffer.WriteInteger(m_idSubViewType);
	buffer.WriteString(name);
	buffer.WriteString(folderName);
	buffer.WriteString(defaultLayer);
	buffer.WriteString(filter.FirstCharSet);
	buffer.WriteString(filter.SecondCharSet);
	buffer.WriteString(filter.ThirdCharSet);
	des_cs.ToBuffer(buffer);//,version,doc_type);
	buffer.WritePoint(ucs.origin);
	buffer.WritePoint(ucs.axis_x);
	buffer.WritePoint(ucs.axis_y);
	buffer.WritePoint(ucs.axis_z);
	BUFFERPOP stack(&buffer,listAssist.GetNodeNum());
	buffer.WriteInteger(listAssist.GetNodeNum());
	for(CDrawingAssistant *pAssis=listAssist.GetFirst();pAssis;pAssis=listAssist.GetNext())
	{
		pAssis->ToBuffer(buffer);//,version,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of draw assistant is wrong!");
#else
		CLDSObject::Log2File()->Log("��ͼ������¼�����ִ���!");
#endif
	stack.Initialize(&buffer,partset.GetNodeNum());
	buffer.WriteInteger(partset.GetNodeNum());
	for(CViewPart *pViewPart=partset.GetFirst();pViewPart;pViewPart=partset.GetNext())
	{
		buffer.WriteInteger(pViewPart->handle);
		buffer.WriteInteger(pViewPart->idClassType);
		buffer.WriteByte(pViewPart->cFlag);
		buffer.WriteInteger(pViewPart->iResidePlaneNo);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of view part is wrong!");
#else
		CLDSObject::Log2File()->Log("��ͼ������¼�����ִ���!");
#endif

}

void CDisplayView::ToXmlFile(FILE* fp,DWORD schema)
{
	fprintf(fp,"<��ͼ��Ϣ dwPermission=\"%d\" ViewType=\"%d\" name=\"%s\">\n",dwPermission,m_iViewType,name);
	fprintf(fp,"<������ͼ�ķ����� folderName=\"%s\"/>\n",folderName);
	fprintf(fp,"<Ĭ����ͼ�� defaultLayer=\"%s\"/>\n",defaultLayer);
	fprintf(fp,"<ͼ�������Ϣ FirstCharSet=\"%s\" SecondCharSet=\"%s\" ThirdCharSet=\"%s\"/>\n",filter.FirstCharSet,filter.SecondCharSet,filter.ThirdCharSet);
	fprintf(fp,"<ԭ������ origin=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.origin.x,ucs.origin.y,ucs.origin.z);
	fprintf(fp,"<X������ axis_x=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.axis_x.x,ucs.axis_x.y,ucs.axis_x.z);
	fprintf(fp,"<Y������ axis_y=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.axis_y.x,ucs.axis_y.y,ucs.axis_y.z);
	fprintf(fp,"<Z������ axis_z=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.axis_z.x,ucs.axis_z.y,ucs.axis_z.z);
	fprintf(fp,"<��ͼ�����б�  num=\"%d\"/>\n",listAssist.GetNodeNum());
	for(CDrawingAssistant *pAssis=listAssist.GetFirst();pAssis;pAssis=listAssist.GetNext())
		pAssis->ToXmlFile(fp,schema);
	fprintf(fp,"<�ؼ����� partNum=\"%d\"/>\n",partset.GetNodeNum());
	for(CViewPart *pViewPart=partset.GetFirst();pViewPart;pViewPart=partset.GetNext())
	{
		fprintf(fp,"<��ʾ�ؼ���Ϣ handle=\"0x%x\" idClassType=\"%d\" Flag=\"%d\" ResidePlaneNo=\"%d\"/>\n",pViewPart->handle,pViewPart->idClassType,
			pViewPart->cFlag,pViewPart->iResidePlaneNo);
	}
	fprintf(fp,"</��ͼ��Ϣ>\n");
}
void CDisplayView::FromBuffer(CBuffer &buffer, long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	//��ȡӰ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
		return;
	buffer.ReadInteger(&dwPermission);
	buffer.ReadInteger(&m_iViewType);
	if(version==0||
		(doc_type==1&&version>=5000010)||
		(doc_type==2&&version>=2000005)||
		(doc_type==4&&version>=1030005)||
		(doc_type==5&&version>=1030005))
		buffer.ReadInteger(&m_idSubViewType);
	buffer.ReadString(name);
#ifdef AFX_TARG_ENU_ENGLISH
	if(stricmp(name,"͸��ͼ")==0)
		strcpy(name,"Perspective View");
	else if(stricmp(name,"ǰ��ͼ")==0)
		strcpy(name,"Front View");
	else if(stricmp(name,"����ͼ")==0)
		strcpy(name,"Back View");
	else if(stricmp(name,"����ͼ")==0)
		strcpy(name,"Left View");
	else if(stricmp(name,"����ͼ")==0)
		strcpy(name,"Right View");
	else if(stricmp(name,"����ͼ")==0)
		strcpy(name,"Downward View");
#endif
	buffer.ReadString(folderName);
	buffer.ReadString(defaultLayer);
	buffer.ReadString(filter.FirstCharSet);
	buffer.ReadString(filter.SecondCharSet);
	buffer.ReadString(filter.ThirdCharSet);
	if( version==0||
		(doc_type==1&&version>=4020200)||
		(doc_type==2&&version>=1020200)||
		(doc_type==3&&version>=2000200)||
		(doc_type==4&&version>=1020200)||
		(doc_type==5&&version>=1020200))
		des_cs.FromBuffer(buffer,version,doc_type);
	buffer.ReadPoint(ucs.origin);
	buffer.ReadPoint(ucs.axis_x);
	buffer.ReadPoint(ucs.axis_y);
	buffer.ReadPoint(ucs.axis_z);
	int i,n=0;
	if(	version==0||doc_type==5||
	   (doc_type==1&&version>=4000107)||(doc_type==2&&version>=1000107)||
	   (doc_type==3&&version>=1070307)||(doc_type==4&&version>=1000307))
	{
		buffer.ReadInteger(&n);
		listAssist.Empty();
		for(i=0;i<n;i++)
		{
			CDrawingAssistant *pAssis=listAssist.append();
			pAssis->FromBuffer(buffer,version,doc_type);
			/*buffer.ReadInteger(&pAssis->m_nStyle);
			buffer.ReadInteger(&pAssis->iNo);
			buffer.ReadString(pAssis->name);
			if(pAssis->m_nStyle>0)
				buffer.ReadInteger(&pAssis->iResidePlaneNo);
			pAssis->m_xLineStart.FromBuffer(buffer);
			if(pAssis->m_nStyle==0)
				pAssis->m_xFaceNorm.FromBuffer(buffer);
			else
				pAssis->m_xLineEnd.FromBuffer(buffer);*/
		}
	}
	n=0;
	buffer.ReadInteger(&n);
	partset.Empty();
	for(i=0;i<n;i++)
	{
		long h=0;
		buffer.ReadInteger(&h);
		CViewPart *pViewPart=AppendViewPart(h);
		buffer.ReadInteger(&pViewPart->idClassType);
		buffer.ReadByte(&pViewPart->cFlag);
		if(	version==0||doc_type==5||
		   (doc_type==1&&version>=4000107)||(doc_type==2&&version>=1000107)||
		   (doc_type==3&&version>=1070307)||(doc_type==4&&version>=1000307))
			buffer.ReadInteger(&pViewPart->iResidePlaneNo);
		else if((doc_type==1&&version>=4000032)||(doc_type==2&&version>=1000087)||
				(doc_type==3&&version>=1070206)||(doc_type==4&&version>=1000206))
		{
			int nSpreadLineOffsetDist;
			if(pViewPart->idClassType==CLS_LINEANGLE&&(pViewPart->cFlag=='S'||pViewPart->cFlag=='Q'))
				buffer.ReadInteger(&nSpreadLineOffsetDist);	//չ����ƫ���� wht 10-12-10
		}

	}
}

//�õ�������ͼ���ڵĶκ� wht 10-12-28
SEGI CDisplayView::GetSegI()
{
	if(m_iSeg.iSeg>=0)
		return m_iSeg;
	CHashList<long> segNoList;	//�κ�Ϊkey,valueΪ�öκŹ�����
	CViewPart *pViewPart=NULL;
	for(pViewPart=partset.GetFirst();pViewPart;pViewPart=partset.GetNext())
	{
		if(pViewPart->idClassType==CLS_BOLT)
			continue;	//��ͳ����˨�κ�
		CLDSPart *pPart=pViewPart->GetPart();
		if(pPart)
		{
			if(pPart<=0)
				continue;
			long *pNum=segNoList.GetValue(pPart->iSeg.iSeg);
			if(pNum==NULL)
				segNoList.SetValue(pPart->iSeg.iSeg,1);
			else
				segNoList.SetValue(pPart->iSeg.iSeg,*pNum+1);
		}
	}
	//ȡ�������϶�Ķκ�Ϊ��ǰ������ͼ�Ķκ� wht 11-07-09
	long nPartNum=0,*pNum=NULL;
	for(pNum=segNoList.GetFirst();pNum;pNum=segNoList.GetNext())
	{
		if(*pNum>nPartNum)
		{
			nPartNum=*pNum;
			m_iSeg=SEGI(segNoList.GetCursorKey());
		}
	}
	return m_iSeg;
}
void CDisplayView::BackupTo(CDisplayView& backup)
{
	backup.m_iViewType=m_iViewType;	//0:��ʾͼ����ͼ 1:��ʾ������ͼ 2:��Ϲ���������ͼ��������ͼ������ֻ������ʾ
	strcpy(backup.defaultLayer,defaultLayer);
	strcpy(backup.folderName,folderName);	//������ʾ������ͼʱ������ʾ������κ���
	strcpy(backup.name,name);			//��ͼ����
	backup.ucs=ucs;
	backup.filter=filter;			//Ҫ��ʾ�ļ���
	backup.partset.Empty();
	for(CViewPart* pViewPart=partset.GetFirst();pViewPart;pViewPart=partset.GetNext())
		backup.AppendViewPart(*pViewPart);	//��ʾ����
	backup.listAssist.Empty();
	for(CDrawingAssistant* pAssist=listAssist.GetFirst();pAssist;pAssist=listAssist.GetNext())
		backup.listAssist.append(*pAssist);	//��ͼ�����б�
}
void CDisplayView::RestoreFrom(CDisplayView& backup)
{
	m_iViewType=backup.m_iViewType;	//0:��ʾͼ����ͼ 1:��ʾ������ͼ 2:��Ϲ���������ͼ��������ͼ������ֻ������ʾ
	strcpy(defaultLayer,backup.defaultLayer);
	strcpy(folderName,backup.folderName);	//������ʾ������ͼʱ������ʾ������κ���
	strcpy(name,backup.name);			//��ͼ����
	ucs=backup.ucs;
	filter=backup.filter;			//Ҫ��ʾ�ļ���
	partset.Empty();
	for(CViewPart* pViewPart=backup.partset.GetFirst();pViewPart;pViewPart=backup.partset.GetNext())
		AppendViewPart(*pViewPart);	//��ʾ����
	listAssist.Empty();
	for(CDrawingAssistant* pAssist=backup.listAssist.GetFirst();pAssist;pAssist=backup.listAssist.GetNext())
		listAssist.append(*pAssist);	//��ͼ�����б�
}

CViewPart *CDisplayView::AppendViewPart(long h)
{
	CViewPart *pViewPart=partset.Add(h);
	pViewPart->SetBelongModel(m_pModel);
	return pViewPart;
}
CViewPart *CDisplayView::AppendViewPart(const CViewPart &viewPart)
{
	CViewPart *pViewPart=partset.Add(viewPart.handle);
	pViewPart->CopyFrom(viewPart);
	pViewPart->SetBelongModel(m_pModel);
	return pViewPart;
}

//////////////////////////////////////////////////////////////////////////
//չ����
IMPLEMENT_GET(CDrawingAssistant,m_nStyle)
{
	return this->_wStyle;
}
IMPLEMENT_SET(CDrawingAssistant,m_nStyle)
{
	this->_wStyle=(WORD)value;
}
IMPLEMENT_GET(CDrawingAssistant,wStyleFlag)
{
	return _wFlag;
}
IMPLEMENT_SET(CDrawingAssistant,wStyleFlag)
{
	this->_wFlag=value;;
}
IMPLEMENT_GET(CDrawingAssistant,m_fRotRadAng)
{
	return m_fRotateAngle;
}
IMPLEMENT_SET(CDrawingAssistant,m_fRotRadAng)
{
	if(m_nStyle==3)
		m_fRotateAngle=value;
}
IMPLEMENT_SET(CDrawingAssistant,bSpreadOverRidge)	//չ�����Ŀ�귽λ1.�����ϲ�2.�����²�3.����ǰ��4.���ܱ���
{
	this->_biSpreadOverRidge=value;
}
IMPLEMENT_GET(CDrawingAssistant,bSpreadOverRidge)	//չ�����Ŀ�귽λ1.�����ϲ�2.�����²�3.����ǰ��4.���ܱ���
{
	return this->_biSpreadOverRidge;
}
IMPLEMENT_SET(CDrawingAssistant,ciSpreadDirection)	//չ�����Ŀ�귽λ1.�����ϲ�2.�����²�3.����ǰ��4.���ܱ���
{
	this->_ciSpreadDirection=value;
}
IMPLEMENT_GET(CDrawingAssistant,ciSpreadDirection)	//չ�����Ŀ�귽λ1.�����ϲ�2.�����²�3.����ǰ��4.���ܱ���
{
	return this->_ciSpreadDirection;
}
IMPLEMENT_SET(CDrawingAssistant,iResidePlaneNo)		//0.��ʾ��ͼ����,����>0ֵ��ʾ����ͼ����Ӧ��չ����;;-1��ʾչ������������,-2��ʾչ����������,-3��ʾչ��������ǰ��,-4��ʾչ�������ܱ��� wjh-2017.11.25
{
	this->_wiResidePlane=(WORD)value;
}
IMPLEMENT_GET(CDrawingAssistant,iResidePlaneNo)		//0.��ʾ��ͼ����,����>0ֵ��ʾ����ͼ����Ӧ��չ����;;-1��ʾչ������������,-2��ʾչ����������,-3��ʾչ��������ǰ��,-4��ʾչ�������ܱ��� wjh-2017.11.25
{
	return this->_wiResidePlane;
}
CDrawingAssistant::CDrawingAssistant()
{
	iNo=0;
	strcpy(name,"");
	_uiType=this->uiSpreadPlane=0;
	m_xDrawNorm.Set();
	m_fRotateAngle=0;
	m_xRotateAxisLine.startPt.Set();
	m_xRotateAxisLine.endPt.Set();
}

void CDrawingAssistant::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	buffer.ReadInteger(&this->_uiType);
	buffer.ReadInteger(&iNo);
	buffer.ReadString(name);
	if(  version==0||doc_type==5||
		(doc_type==1&&version>=4000108)||(doc_type==2&&version>=1000108)||
		(doc_type==3&&version>=1070308)||(doc_type==4&&version>=1000308))
		buffer.ReadInteger(&m_iSeg.iSeg);
	if(m_nStyle>0||version==0||(doc_type==2&&version>=2000800))
		buffer.ReadInteger(&uiSpreadPlane);
	if(m_nStyle==3&&(version==0||(doc_type==2&&version>=2000800)))
		buffer.ReadDouble(&m_fRotateAngle);
	m_xLineStart.FromBuffer(buffer,version,doc_type);
	if(m_nStyle==0||m_nStyle==3)
		m_xFaceNorm.FromBuffer(buffer,version,doc_type);
	if(m_nStyle==1||m_nStyle==2||(m_nStyle==0&&(wStyleFlag&CDrawingAssistant::ENABLE_LINE_END)>0))
		m_xLineEnd.FromBuffer(buffer,version,doc_type);
}

void CDrawingAssistant::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteInteger(this->_uiType);
	buffer.WriteInteger(iNo);
	buffer.WriteString(name);
	buffer.WriteInteger(m_iSeg);
	if(m_nStyle>0||version==0||(doc_type==2&&version>=2000800))
		buffer.WriteInteger(uiSpreadPlane);
	if(m_nStyle==3&&(version==0||(doc_type==2&&version>=2000800)))
		buffer.WriteDouble(m_fRotateAngle);
	m_xLineStart.ToBuffer(buffer,version,doc_type);
	if(m_nStyle==0||m_nStyle==3)
		m_xFaceNorm.ToBuffer(buffer,version,doc_type);
	if(m_nStyle==1||m_nStyle==2||(m_nStyle==0&&(wStyleFlag&CDrawingAssistant::ENABLE_LINE_END)>0))
		m_xLineEnd.ToBuffer(buffer,version,doc_type);
}

void CDrawingAssistant::ToXmlFile(FILE* fp,DWORD schema)
{
	fprintf(fp,"<��ͼ������Ϣ Style=\"%d\" iNo=\"%d\" name=\"%s\" iSeg=\"%d\">\n",m_nStyle,iNo,name,m_iSeg);
	if(m_nStyle>0)
		fprintf(fp,"<������ iResidePlaneNo=\"%d\"/>\n",iResidePlaneNo);
	m_xLineStart.ToXmlFile(fp,schema);
	if(m_nStyle==0)
		m_xFaceNorm.ToXmlFile(fp,schema);
	else 
		m_xLineEnd.ToXmlFile(fp,schema);
	fprintf(fp,"</��ͼ������Ϣ>\n");
}
void CDrawingAssistant::GetRotateAxisAndAngle(f3dPoint draw_norm,
											  f3dLine &rotate_axis,double &rotate_angle,IModel *pTower/*=NULL*/)
{
	rotate_angle=0;
	rotate_axis.startPt.Set();
	rotate_axis.endPt.Set();
	if(m_nStyle!=0||draw_norm.IsZero()||iNo<=0)
		return;
	if(!draw_norm.IsZero()&&draw_norm==m_xDrawNorm)
	{	//������ǰһ�μ���ʱ�ķ���һ��,ֱ�ӷ����ϴμ�����
		rotate_angle=m_fRotateAngle;
		rotate_axis.startPt=m_xRotateAxisLine.startPt;
		rotate_axis.endPt=m_xRotateAxisLine.endPt;
		return;
	}
	m_xFaceNorm.UpdateVector(pTower);
	m_xLineStart.UpdatePos(pTower);
	f3dPoint spread_face_norm=m_xFaceNorm.vector;
	f3dPoint spread_face_pick=m_xLineStart.Position();
	if(spread_face_norm.IsZero())
		return;
	//�������뷨�߼�����ת�ἰ��ת�Ƕ�
	m_xDrawNorm=draw_norm;
	f3dPoint axis_line_vec=spread_face_norm^m_xDrawNorm;
	normalize(axis_line_vec);
	m_fRotateAngle=cal_angle_of_2vec(draw_norm,spread_face_norm);
	if(this->ciSpreadDirection==1||this->ciSpreadDirection==2)
	{
		if(this->bSpreadOverRidge&&m_fRotateAngle<1.5707)	//Pi/2
			m_fRotateAngle+=Pi;
		else if(!this->bSpreadOverRidge&&m_fRotateAngle>1.5707)
			m_fRotateAngle-=Pi;
	}
	else if(ciSpreadDirection==0&&m_fRotateAngle>1.5707963)	//Pi/2=3.1415926/2=1.5707963
		m_fRotateAngle-=Pi;	//��֤Ĭ����С�Ƕ���תչ��
	rotate_angle=m_fRotateAngle;
	rotate_axis.startPt=m_xRotateAxisLine.startPt=spread_face_pick;
	rotate_axis.endPt=m_xRotateAxisLine.endPt=spread_face_pick+axis_line_vec*1000;
}
