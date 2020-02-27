//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "lds_part.h"
#include "env_def.h"
#include "MirTaAtom.h"
#include "UserDefOper.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "LogFile.h"
#include "UI.h"
#include "MirOperObject.h"

/////////////////////////////////////////////////////////

//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
void AddMirObjToSrcObjRelaObjList(CLDSDbObject *pSrcObj,CLDSDbObject *pMirObj,MIRMSG msg)
{
	if(pSrcObj==NULL||pMirObj==NULL)
		return;
	BOOL bInList=FALSE;
	RELATIVE_OBJECT *pItem=NULL;
	for(pItem=pSrcObj->relativeObjs.GetFirst();pItem;pItem=pSrcObj->relativeObjs.GetNext())
	{	//�����ظ���ӹ�������
		if(pItem->hObj==pMirObj->handle)
		{
			bInList=TRUE;
			break;
		}
	}
	if(!bInList)
	{
		pItem=pSrcObj->relativeObjs.Add(pMirObj->handle);
		pItem->mirInfo=msg;
		if(pSrcObj->IsNode())
			((CLDSNode*)pSrcObj)->SetModified();
		else if(pSrcObj->IsPart())
			((CLDSPart*)pSrcObj)->SetModified();
	}
}
//����(��׼������������)�б��еĶ���˴˹���
void SetRelativeObj(CLDSDbObject *pObj)
{
	if(pObj==NULL)
		return;
	//���б��е�ÿ������������Ϊ������
	//����������Ϊ������Ĺ���������ӵ���������������б���
	RELATIVE_OBJECT *pTempItem=NULL,*pRelaObjItem=NULL,*pItem=NULL;
	for(pTempItem=pObj->relativeObjs.GetFirst();pTempItem;pTempItem=pObj->relativeObjs.GetNext())
	{	
		if(pTempItem->hObj==pObj->handle)	//�ų�����Գƣ�����ᵼ����ѭ�� wjh-2018.6.8
			pObj->relativeObjs.DeleteCursor();
		CLDSDbObject *pRelativeObj=(CLDSDbObject*)Ta.FromHandle(pTempItem->hObj,TRUE);
		if(pRelativeObj==NULL)
			continue;
		//�Ƚ���׼������ӵ�ǰ�����Ĺ��������б���
		BOOL bInList=FALSE;
		for(RELATIVE_OBJECT *pOthItem=pRelativeObj->relativeObjs.GetFirst();pOthItem;pOthItem=pRelativeObj->relativeObjs.GetNext())
		{	//�����ظ���ӹ�������
			if(pOthItem->hObj==pObj->handle)
			{
				bInList=TRUE;
				break;
			}
		}
		if(!bInList)	
		{
			pRelaObjItem=pRelativeObj->relativeObjs.Add(pObj->handle);
			pRelativeObj->SetModified(TRUE,FALSE);
			pRelaObjItem->mirInfo=pTempItem->mirInfo;
			//�����ԳƷ�ʽ
			if(pTempItem->mirInfo.axis_flag==8) //��ת�Գ�
			{
				pRelaObjItem->mirInfo.axis_flag=8;
				pRelaObjItem->mirInfo.rotate_angle=-pTempItem->mirInfo.rotate_angle;
				//pRelaObjItem->mirInfo.axis_flag=4|8;
			}
			else if(pTempItem->mirInfo.axis_flag==12) //Z��|��ת�Գ�
				pRelaObjItem->mirInfo.axis_flag=8;
		}
		pObj->relativeObjs.push_stack();
		for(pItem=pObj->relativeObjs.GetFirst();pItem;pItem=pObj->relativeObjs.GetNext())
		{	
			if(pItem->hObj==pRelativeObj->handle)
				continue;	//�Լ����Լ����ù���
			bInList=FALSE;
			for(RELATIVE_OBJECT *pOthItem=pRelativeObj->relativeObjs.GetFirst();pOthItem;pOthItem=pRelativeObj->relativeObjs.GetNext())
			{	//�����ظ���ӹ�������
				if(pOthItem->hObj==pItem->hObj)
				{
					bInList=TRUE;
					break;
				}
			}
			if(bInList)	
				continue;	//�б����Ѿ����ڸö���
			if(pTempItem->mirInfo.axis_flag&0x10 || pItem->mirInfo.axis_flag&0x10)
				continue;	//TODO:45�ȵȾ���Գƻ�δʵ�֣����ڸ�ֵ�ᵼ�´���Գ���Ϣ wjh-2015.9.16
			RELATIVE_OBJECT *pRelaObjItem=pRelativeObj->relativeObjs.Add(pItem->hObj);
			pRelativeObj->SetModified(TRUE,FALSE);
			pRelaObjItem->mirInfo=pItem->mirInfo;
			//�����ԳƷ�ʽ ��ʱ��������Ӧ���и��򵥵ķ�����
			if(pTempItem->mirInfo.axis_flag==1)
			{	//��ǰ�����������׼�����ԳƷ�ʽΪX��Գ�
				//��ԭ����Y��Գƺ�Z��Գƻ���
				if(pItem->mirInfo.axis_flag&1)		//X��Գ�
					pRelaObjItem->mirInfo.axis_flag=pItem->mirInfo.axis_flag^1;
				else if(pItem->mirInfo.axis_flag&2)	//����Y��Գƣ�Y��ΪZ
					pRelaObjItem->mirInfo.axis_flag=(pItem->mirInfo.axis_flag^2)|4;
				else if(pItem->mirInfo.axis_flag&4)	//����Z��Գƣ�Z��ΪY
					pRelaObjItem->mirInfo.axis_flag=(pItem->mirInfo.axis_flag^4)|2;
				else if(pItem->mirInfo.axis_flag==8)	//��ת�Գ�ת��ΪX��|��ת�Գ�
					pRelaObjItem->mirInfo.axis_flag=1|8;
			}
			else if(pTempItem->mirInfo.axis_flag==2)
			{	//��ǰ�����������׼�����ԳƷ�ʽΪY��Գ�
				//��ԭ����X��Գƺ�Z��Գƻ���
				if(pItem->mirInfo.axis_flag&2)		//Y��Գ�
					pRelaObjItem->mirInfo.axis_flag=pItem->mirInfo.axis_flag^2;
				else if(pItem->mirInfo.axis_flag&1)	//����X��Գƣ�X��ΪZ
					pRelaObjItem->mirInfo.axis_flag=(pItem->mirInfo.axis_flag^1)|4;
				else if(pItem->mirInfo.axis_flag&4)	//����Z��Գƣ�Z��ΪX
					pRelaObjItem->mirInfo.axis_flag=(pItem->mirInfo.axis_flag^4)|1;
				else if(pItem->mirInfo.axis_flag==8)	//��ת�Գ�ת��ΪY��|��ת�Գ�
					pRelaObjItem->mirInfo.axis_flag=2|8;
			}
			else if(pTempItem->mirInfo.axis_flag==4)
			{	//��ǰ�����������׼�����ԳƷ�ʽΪZ��Գ�
				//��ԭ����X��Գƺ�Y��Գƻ���
				if(pItem->mirInfo.axis_flag&4)		//Z��Գ�
					pRelaObjItem->mirInfo.axis_flag=pItem->mirInfo.axis_flag^4;
				else if(pItem->mirInfo.axis_flag&1)	//����X��Գƣ�X��ΪY
					pRelaObjItem->mirInfo.axis_flag=(pItem->mirInfo.axis_flag^1)|2;
				else if(pItem->mirInfo.axis_flag&2)	//����Y��Գƣ�Y��ΪX
					pRelaObjItem->mirInfo.axis_flag=(pItem->mirInfo.axis_flag^2)|1;
				else if(pItem->mirInfo.axis_flag==8)	//��ת�Գ�ת��ΪZ��|��ת�Գ�
					pRelaObjItem->mirInfo.axis_flag=4|8;
			}
			else if(pTempItem->mirInfo.axis_flag==12)
			{	//��ǰ�����������׼�����ԳƷ�ʽΪ��Z��|��ת�Գ�
				if(pItem->mirInfo.axis_flag==1)	//X��Գ�
					pRelaObjItem->mirInfo.axis_flag=2|8;
				else if(pItem->mirInfo.axis_flag==2)//Y��Գ�
					pRelaObjItem->mirInfo.axis_flag=1|8;
				if(pItem->mirInfo.axis_flag==9)	//X��|��ת�Գ�
					pRelaObjItem->mirInfo.axis_flag=1;
				else if(pItem->mirInfo.axis_flag==10)//Y��|��ת�Գ�
					pRelaObjItem->mirInfo.axis_flag=2;
				else if(pItem->mirInfo.axis_flag==4)//Z��Գ�
					pRelaObjItem->mirInfo.axis_flag=4|8;
				else if(pItem->mirInfo.axis_flag==8)//��ת�Գ�
					pRelaObjItem->mirInfo.axis_flag=4;
			}
			else if(pTempItem->mirInfo.axis_flag==8)
			{	//��ǰ�����������׼�����ԳƷ�ʽΪ����ת�Գ�
				if(pItem->mirInfo.axis_flag==1)	//X��Գ�
					pRelaObjItem->mirInfo.axis_flag=1|8;
				else if(pItem->mirInfo.axis_flag==2)//Y��Գ�
					pRelaObjItem->mirInfo.axis_flag=2|8;
				if(pItem->mirInfo.axis_flag==9)	//X��|��ת�Գ�
					pRelaObjItem->mirInfo.axis_flag=2;
				else if(pItem->mirInfo.axis_flag==10)//Y��|��ת�Գ�
					pRelaObjItem->mirInfo.axis_flag=1;
				else if(pItem->mirInfo.axis_flag==12)//Z��|��ת�Գ�
					pRelaObjItem->mirInfo.axis_flag=4;
				else if(pItem->mirInfo.axis_flag==4)//Z��Գ�
					pRelaObjItem->mirInfo.axis_flag=8;
				else if(pItem->mirInfo.axis_flag==8)
				{
					double angle1=0,angle2=0;
					pRelaObjItem->mirInfo.axis_flag=8;
					//��֤��ת�Ƕ���0~360֮��
					angle1=pTempItem->mirInfo.rotate_angle;
					if(angle1<0)
						angle1=360-(fabs(angle1)-(fabs(angle1)/360)*360);
					angle2=pItem->mirInfo.rotate_angle;
					if(angle2<0)
						angle2=360-(fabs(angle2)-(fabs(angle1)/360)*360);
					pRelaObjItem->mirInfo.rotate_angle=(short)ftoi(angle2-angle1);
				}
			}
			else if(pTempItem->mirInfo.axis_flag==9)
			{	//��ǰ�����������׼�����ԳƷ�ʽΪ��X��|��ת�Գ�
				if(pItem->mirInfo.axis_flag==1)	
					pRelaObjItem->mirInfo.axis_flag=4|8;
				else if(pItem->mirInfo.axis_flag==12)
					pRelaObjItem->mirInfo.axis_flag=1;
				else if(pItem->mirInfo.axis_flag==4)
					pRelaObjItem->mirInfo.axis_flag=2|8;
				else if(pItem->mirInfo.axis_flag==10)
					pRelaObjItem->mirInfo.axis_flag=4;
				else if(pItem->mirInfo.axis_flag==2)
					pRelaObjItem->mirInfo.axis_flag=8;
				else if(pItem->mirInfo.axis_flag==8)
					pRelaObjItem->mirInfo.axis_flag=2;
			}
			else if(pTempItem->mirInfo.axis_flag==10)
			{	//��ǰ�����������׼�����ԳƷ�ʽΪ��Y��|��ת�Գ�
				if(pItem->mirInfo.axis_flag==1)	
					pRelaObjItem->mirInfo.axis_flag=8;
				else if(pItem->mirInfo.axis_flag==8)
					pRelaObjItem->mirInfo.axis_flag=1;
				else if(pItem->mirInfo.axis_flag==2)
					pRelaObjItem->mirInfo.axis_flag=4|8;
				else if(pItem->mirInfo.axis_flag==12)
					pRelaObjItem->mirInfo.axis_flag=2;
				else if(pItem->mirInfo.axis_flag==4)
					pRelaObjItem->mirInfo.axis_flag=1|8;
				else if(pItem->mirInfo.axis_flag==9)
					pRelaObjItem->mirInfo.axis_flag=4;
			}
		}
		pObj->relativeObjs.pop_stack();
	}
}

//���ݶԳƹ�ϵת���ڵ���������
void ConvertAttachHandle(CLDSNode* pOriginNode, CLDSNode* pDestNode, MIRMSG mirmsg)
{
	CLDSNode *pTemNode1, *pTemNode2;
	CSuperSmartPtr<CLDSPart> pTemPart1, pTemPart2;
	f3dLine line;
	f3dPoint offset_vec;
	f3dPoint pickPos=pOriginNode->Position();
	if(pDestNode->m_cPosCalType==5)	//�����ȷֵ�
	{
		CLDSNode *pStart=Ta.Node.FromHandle(pDestNode->arrRelationNode[0]);
		CLDSNode *pEnd=Ta.Node.FromHandle(pDestNode->arrRelationNode[1]);
		if(pStart&&pEnd)
		{
			line.startPt=pStart->Position(false);
			line.endPt=pEnd->Position(false);
		}
	}
	else if(pDestNode->m_cPosCalType==7)	//ƫ�Ƶ�
	{
		CLDSNode *pStart=Ta.Node.FromHandle(pDestNode->arrRelationNode[1]);
		CLDSNode *pEnd=Ta.Node.FromHandle(pDestNode->arrRelationNode[2]);
		if(pStart&&pEnd)
			offset_vec=pEnd->Position(false)-pStart->Position(false);
	}
	if(mirmsg.axis_flag&1||mirmsg.axis_flag&2||mirmsg.axis_flag&4||mirmsg.axis_flag&8||mirmsg.axis_flag&0x10)
	{
		pTemNode1 = Ta.Node.FromHandle(pOriginNode->arrRelationNode[0]);
		if(pTemNode1!=NULL)//�������ڵ���Ϊ0,��ʾ�������ڵ�
		{
			pTemNode2 = pTemNode1->GetMirNode(mirmsg);//->mir_y_node_handle);
			if(pTemNode2!=NULL)//�������ڵ�����Ӧ�ĶԳƽڵ�ʱ����Ӧ�ĶԳƽڵ�
				pDestNode->arrRelationNode[0] = pTemNode2->handle;
			else//���ڵ�ĶԳƽڵ����Լ����������ѹ鲢����һ���� WJH-2004/02/06
				pDestNode->arrRelationNode[0] = 0;//pOriginNode->arrRelationNode[0];
		}
		
		pTemNode1 = Ta.Node.FromHandle(pOriginNode->arrRelationNode[1]);
		if(pTemNode1!=NULL)//�������ڵ���Ϊ0,��ʾ�������ڵ�
		{
			pTemNode2 = pTemNode1->GetMirNode(mirmsg);//->mir_y_node_handle);
			if(pTemNode2!=NULL)//�������ڵ�����Ӧ�ĶԳƽڵ�ʱ����Ӧ�ĶԳƽڵ�
				pDestNode->arrRelationNode[1] = pTemNode2->handle;
			else//���ڵ�ĶԳƽڵ����Լ����������ѹ鲢����һ���� WJH-2004/02/06
				pDestNode->arrRelationNode[1] = 0;//pOriginNode->arrRelationNode[1];
		}
		
		pTemNode1 = Ta.Node.FromHandle(pOriginNode->arrRelationNode[2]);
		if(pTemNode1!=NULL)//�������ڵ���Ϊ0,��ʾ�������ڵ�
		{
			pTemNode2 = pTemNode1->GetMirNode(mirmsg);//->mir_y_node_handle);
			if(pTemNode2!=NULL)//�������ڵ�����Ӧ�ĶԳƽڵ�ʱ����Ӧ�ĶԳƽڵ�
				pDestNode->arrRelationNode[2] = pTemNode2->handle;
			else//���ڵ�ĶԳƽڵ����Լ����������ѹ鲢����һ���� WJH-2004/02/06
				pDestNode->arrRelationNode[2] = 0;//pOriginNode->arrRelationNode[1];
		}
		
		pTemNode1 = Ta.Node.FromHandle(pOriginNode->arrRelationNode[3]);
		if(pTemNode1!=NULL)//�������ڵ���Ϊ0,��ʾ�������ڵ�
		{
			pTemNode2 = pTemNode1->GetMirNode(mirmsg);//->mir_y_node_handle);
			if(pTemNode2!=NULL)//�������ڵ�����Ӧ�ĶԳƽڵ�ʱ����Ӧ�ĶԳƽڵ�
				pDestNode->arrRelationNode[3] = pTemNode2->handle;
			else//���ڵ�ĶԳƽڵ����Լ����������ѹ鲢����һ���� WJH-2004/02/06
				pDestNode->arrRelationNode[3] = 0;//pOriginNode->arrRelationNode[1];
		}
		
		if(pOriginNode->m_cPosCalType==4)
		{
			pTemPart1 = Ta.Parts.FromHandle(pOriginNode->arrRelationPole[0]);
			if(pTemPart1.IsHasPtr()&&pTemPart1->IsLinePart())//�������Ǹ־��Ϊ0,��ʾ�������Ǹ�,�˳�
			{
				pTemPart2 = pTemPart1.LinePartPointer()->GetMirPart(mirmsg,pickPos);//(CLDSLineAngle*)Ta.FromJgHandle(pTemJg1->hMirPartY);
				if(pTemPart2.IsHasPtr())
					pDestNode->arrRelationPole[0] = pTemPart2->handle;
				//pTemJg2 = pTemJg1;
			}
			pTemPart1 = Ta.Parts.FromHandle(pOriginNode->arrRelationPole[1]);
			if(pTemPart1.IsHasPtr()&&pTemPart1->IsLinePart())//�������Ǹ־��Ϊ0,��ʾ�������Ǹ�,�˳�
			{
				pTemPart2 = pTemPart1.LinePartPointer()->GetMirPart(mirmsg,pickPos);//Ta.FromJgHandle(pTemJg1->hMirPartY);
				if(pTemPart2.IsHasPtr())
					pDestNode->arrRelationPole[1] = pTemPart2->handle;
				//pTemJg2=pTemJg1;
			}
		}
		else if(pDestNode->m_cPosCalType==5)	//�����ȷֵ�
		{
			CLDSNode *pStart=Ta.Node.FromHandle(pDestNode->arrRelationNode[0]);
			CLDSNode *pEnd=Ta.Node.FromHandle(pDestNode->arrRelationNode[1]);
			pDestNode->attach_scale=pOriginNode->attach_scale;
			if(pStart&&pEnd&&line.endPt.IsEqual(pStart->Position(false))&&line.startPt.IsEqual(pEnd->Position(false)))
				pDestNode->attach_scale=1-pDestNode->attach_scale;	//�Գƺ������ڵ�λ�õ���
		}
		else if(pDestNode->m_cPosCalType==7)	//ƫ�Ƶ�
		{
			CLDSNode *pStart=Ta.Node.FromHandle(pDestNode->arrRelationNode[1]);
			CLDSNode *pEnd=Ta.Node.FromHandle(pDestNode->arrRelationNode[2]);
			pDestNode->attach_offset=pOriginNode->attach_offset;
			f3dPoint vec;
			if(pStart&&pEnd)
				vec=pEnd->Position(false)-pStart->Position(false);
			if(vec*offset_vec<0)	//�Գƺ������ڵ������ƫ�Ʒ�����
				pDestNode->attach_offset*=-1.0;
		}
		else if(pDestNode->m_cPosCalType>=8&&pDestNode->m_cPosCalType<=10)	//��׼����ָ�����������
		{
			pDestNode->hRelationPlane=pOriginNode->hRelationPlane;
			pOriginNode->SyncMirPropTo(pDestNode,mirmsg,"pos");
		}
	}
}
static BOOL IsPartMirValid(CLDSLinePart *pLinePart, MIRMSG mirmsg)
{
	f3dPoint mir_point;
	f3dPoint lineStart=pLinePart->Start();
	f3dPoint lineEnd=pLinePart->End();
	if(pLinePart->pStart)
		lineStart=pLinePart->pStart->Position(false);
	if(pLinePart->pEnd)
		lineEnd=pLinePart->pEnd->Position(false);
	if(mirmsg.axis_flag&2)		//Y��Գ�
	{
		mir_point = GetMirPos(lineStart,mirmsg.GetSubMirItem(2));
		if(DISTANCE(lineEnd,mir_point)<EPS2)
			return FALSE;
	}
	if(mirmsg.axis_flag&1)		//X��Գ�
	{
		mir_point = GetMirPos(lineStart,mirmsg.GetSubMirItem(1));
		if(DISTANCE(lineEnd,mir_point)<EPS2)
			return FALSE;
	}
	if(mirmsg.axis_flag&4)		//Z��Գ�
	{
		mir_point = GetMirPos(lineStart,mirmsg.GetSubMirItem(3));
		if(DISTANCE(lineEnd,mir_point)<EPS2)
			return FALSE;
	}
	return TRUE;
}
static BOOL IsNodeMirValid(CLDSNode *pNode, MIRMSG mirmsg)
{
	f3dPoint mir_point;
	CLDSNode *pAttachNode;
	MIRMSG msg=mirmsg;
	int flag_arr[3]={1,2,4};
	for(int i=0;i<3;i++)
	{
		if(mirmsg.axis_flag&flag_arr[i])
		{
			msg = mirmsg;
			if(i!=0&&msg.axis_flag&1)
				msg.axis_flag-=1;
			if(i!=1&&msg.axis_flag&2)
				msg.axis_flag-=2;
			if(i!=2&&msg.axis_flag&4)
				msg.axis_flag-=4;
			for(int j=0;j<4;j++)
			{
				if(pNode->arrRelationNode[j]>0x20)
				{
					pAttachNode=Ta.Node.FromHandle(pNode->arrRelationNode[j]);
					if(pAttachNode&&pAttachNode->GetMirNode(msg)==NULL)
						return FALSE;
				}
			}
		}
	}
	return TRUE;
}

CLDSNode* CreateSingleMirNode(CLDSNode *pSrcNode, MIRMSG msg,BOOL bIncAttachInfo=TRUE,char *sErrorInfo=NULL)
{
	f3dPoint norminal_vec,offset_vec;	//���ڼ�¼ԭƫ�Ƶ������Ǹֵ���ƫ�Ʒ���
	f3dPoint mir_offset_vec;//���ڼ�¼ԭ�Գ�ƫ�Ƶ������Ǹֵ���ƫ�Ʒ���
	CLDSNode *pMirNode=NULL, *pAttachNode=NULL, *pMirAttachNode=NULL;
	CLDSPart *pPart;
	CLDSLinePart *pMirPart=NULL,*pAttachPart=NULL,*pMirAttachPart=NULL;
	CLDSLinePart* pFatherPart = NULL;

	if(bIncAttachInfo&&!IsNodeMirValid(pSrcNode,msg))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "fail to pass symmetry legality checking, parts can not be symmetrical";
#else
		throw "δͨ���ԳƺϷ��Լ��,���ܶԳƹ���";
#endif

	if(pSrcNode->hFatherPart>=0x20)
		pFatherPart = (CLDSLinePart*)Ta.Parts.FromHandle(pSrcNode->hFatherPart);
	if(pSrcNode->m_cPosCalType==7)	//ƫ�ƽڵ��Խڵ㸸����Ϊ��������
		pPart=pFatherPart;
	else
		pPart=Ta.Parts.FromHandle(pSrcNode->arrRelationPole[0]);
	f3dPoint pickPos=pSrcNode->Position();
	if(pPart&&pPart->IsLinePart())
	{
		pAttachPart = (CLDSLinePart*)pPart;
		pMirAttachPart = (CLDSLinePart*)pAttachPart->GetMirPart(msg,pickPos);
	}
	pMirNode=pSrcNode->GetMirNode(msg);
	if(pMirNode==NULL)
	{
		f3dPoint mirpos=GetMirPos(pSrcNode->Position(false),msg);
		if(mirpos==pSrcNode->Position(false))
#ifdef AFX_TARG_ENU_ENGLISH
			throw "original node is too near to axis of symmetry, can be symmetrical with the axis";
#else
			throw "ԭʼ�ڵ����Գ���̫��,���ܽ�����Ӧ��Գ�";
#endif
		pMirNode = console.AppendNode();
		pMirNode->SetPosition(mirpos);
		pMirNode->CopyProperty(pSrcNode);//�������ڵ�(����)����
		if(msg.axis_flag==8&&ftoi(msg.rotate_angle/90)%2!=0&&pSrcNode->m_cPosCalType==1)
			pMirNode->m_cPosCalType=2;	//��ת90�ȶԳ�ʱX��Y��������
		else if(msg.axis_flag==8&&ftoi(msg.rotate_angle/90)%2!=0&&pSrcNode->m_cPosCalType==2)
			pMirNode->m_cPosCalType=1;	//��ת90�ȶԳ�ʱX��Y��������
		NewNode(pMirNode);
	}
	CalMirAtomLayer(pSrcNode->layer(),pMirNode->layer(),msg);
	if(!bIncAttachInfo)
		return pMirNode;
	if(pFatherPart)
		pMirPart = (CLDSLinePart*)pFatherPart->GetMirPart(msg,pickPos);
	else
		pMirPart=NULL;
	if(pMirPart)
		pMirNode->hFatherPart = pMirPart->handle;
	if(bIncAttachInfo)
		ConvertAttachHandle(pSrcNode, pMirNode, msg);//���ݶԳƹ�ϵת���ڵ���������

	pMirNode->SetArcLift(pSrcNode->ArcLiftHandle());
	if(pSrcNode->ArcLiftHandle()>0x20)
		pMirNode->Set();
	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
	AddMirObjToSrcObjRelaObjList(pSrcNode,pMirNode,msg);
	return pMirNode;
	/*
	����ƫ�Ƶ��ƫ�Ʒ������Ի�׼�˼�ʼ���շ���Ϊ��׼�������Ե�һ����׼�㵽
	�ڶ�����׼��Ϊ��׼��������Ҫ�ж�ƫ�Ʒ����Ƿ��򣬹����´���ֹͣʹ�� wjh-2009-11-11
	if(pMirNode->m_cPosCalType==7)	//ƫ�Ƶ�
	{
	if(pAttachPart==NULL||pMirAttachPart==NULL)
	throw "δͨ���ԳƺϷ��Լ��,���ܶԳƹ���";
	offset_vec=pAttachPart->End()-pAttachPart->Start();
	offset_vec=GetMirVector(offset_vec,msg);
	mir_offset_vec=pMirAttachPart->End()-pMirAttachPart->Start();
	if(offset_vec*mir_offset_vec<0)	//�Գƺ�ƫ�Ʒ���һ��
	pMirNode->attach_offset*=-1.0;
	}*/
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
CLDSBolt *CreateSingleMirBolt(CLDSBolt* pBolt, MIRMSG msg, BOOL bIncAttachInfo=TRUE, THANDLE* relaPartIdArr=NULL,UINT nRelaPartCount=0);
void MirLsRef(CSuperSmartPtr<CLDSPart> pPart,CSuperSmartPtr<CLDSPart> pMirPart,MIRMSG msg)
{
	if(pPart==NULL||pMirPart==NULL)
		return;
	if(pPart->GetClassTypeId()!=pMirPart->GetClassTypeId())
		return;
	CLsRefList *pLsRefList=NULL;
	if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		pLsRefList=pPart.ParamPlatePointer()->GetLsRefList();
	else if(pPart->GetClassTypeId()==CLS_PLATE)
		pLsRefList=pPart.PlatePointer()->GetLsRefList();
	if(pLsRefList==NULL)
		return;
	for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
	{
		CLDSBolt *pMirBolt=(CLDSBolt*)pLsRef->GetLsPtr()->GetMirPart(msg);
		if(pMirBolt)
		{
			pMirBolt->ReplaceReferenceHandle(CHandleReplacer(pPart->handle,pMirPart->handle));
			pMirBolt->SetModified();
			pMirBolt->Create3dSolidModel();
			if(!pMirBolt->m_bVirtualPart||(pMirBolt->m_bVirtualPart&&g_sysPara.display.bDispVirtualPart))
				g_pSolidDraw->NewSolidPart(pMirBolt->GetSolidPartObject());
		}
		else
			pMirBolt=CreateSingleMirBolt(pLsRef->GetLsPtr(),msg);
		if(pMirPart->GetClassTypeId()==CLS_PLATE)
			pMirPart.PlatePointer()->AppendLsRef(pMirBolt->GetLsRef());
		else if(pMirPart->GetClassTypeId()==CLS_PARAMPLATE)
			pMirPart.ParamPlatePointer()->AppendLsRef(pMirBolt->GetLsRef());
	}
}

void MirThickPara(CDesignThickPara &src,CDesignThickPara &mir,MIRMSG msg)
{
	strcpy(mir.key_str,"");
	for(THICK_ITEM item=src.GetFirstItem();item.val!=0;item=src.GetNextItem())
	{
		if(item.IsHandle())
		{
			int sign=1;
			if(item.val<0)
				sign=-1;
			item.val*=sign;
			CLDSPart *pPart=Ta.Parts.FromHandle(item.val);
			if(pPart==NULL)
				continue;
			f3dPoint pickPos;
			CLDSPart *pMirPart=pPart->GetMirPart(msg,pickPos);
			if(pMirPart)
				mir.AddThick(sign*pMirPart->handle,TRUE);
			else
				mir.AddThick(item.val,item.IsHandle());
		}
		else	//ֱ����Ӻ��
			mir.AddThick(item.val);
	}
}
void MirThickPara(CParaThick &src,CParaThick &mir,MIRMSG msg)
{
	mir.BuildFromStr("");
	for(THICK_ITEM item=src.GetFirstItem();item.val!=0;item=src.GetNextItem())
	{
		if(item.IsHandle())
		{
			int sign=1;
			if(item.val<0)
				sign=-1;
			item.val*=sign;
			CLDSPart *pPart=Ta.Parts.FromHandle(item.val);
			if(pPart==NULL)
				continue;
			f3dPoint pickPos;
			CLDSPart *pMirPart=pPart->GetMirPart(msg,pickPos);
			if(pMirPart)
				mir.AddThick(sign*pMirPart->handle,TRUE);
			else
				mir.AddThick(item.val,item.IsHandle());
		}
		else	//ֱ����Ӻ��
			mir.AddThick(item.val);
	}
}
#endif

void MirDatumPoint(IModel *pTower,CLDSPoint &srcPoint,CLDSPoint &mirPoint,MIRMSG msg)
{
	mirPoint.datum_pos_style=srcPoint.datum_pos_style;
	strcpy(mirPoint.description,srcPoint.description);
	mirPoint.datum_pos=srcPoint.datum_pos;
	mirPoint.SetPosition(GetMirPos(srcPoint.Position(),msg));
	f3dPoint line_vec,mir_line_vec,line_vec_tm;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if(mirPoint.datum_pos_style==1||mirPoint.datum_pos_style==7)	//�Ǹ����
	{
		mirPoint.des_para.RODEND=srcPoint.des_para.RODEND;
		MirPartHandle(pTower,&mirPoint.des_para.RODEND.hRod,srcPoint.des_para.RODEND.hRod,msg);
		CLDSLineAngle *pAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(srcPoint.des_para.RODEND.hRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pAngle)
		{
			f3dPoint mirnorm;
			if(srcPoint.des_para.RODEND.offset_wing==0)
				mirnorm=GetMirVector(pAngle->get_norm_x_wing(),msg);
			else
				mirnorm=GetMirVector(pAngle->get_norm_y_wing(),msg);
			CLDSLineAngle *pMirAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(mirPoint.des_para.RODEND.hRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pMirAngle)
			{
				if(fabs(pMirAngle->get_norm_x_wing()*mirnorm)>fabs(pMirAngle->get_norm_y_wing()*mirnorm))
					mirPoint.des_para.RODEND.offset_wing=0;
				else
					mirPoint.des_para.RODEND.offset_wing=1;
				line_vec=pAngle->End()-pAngle->Start();
				mir_line_vec=pMirAngle->End()-pMirAngle->Start();
				line_vec_tm=GetMirVector(line_vec,msg);
				if(line_vec_tm*mir_line_vec>0)
					mirPoint.des_para.RODEND.direction=srcPoint.des_para.RODEND.direction;
				else
					mirPoint.des_para.RODEND.direction=1-srcPoint.des_para.RODEND.direction;
			}
		}
		mirPoint.des_para.RODEND.bIncOddEffect=srcPoint.des_para.RODEND.bIncOddEffect;
		mirPoint.des_para.RODEND.len_offset_dist=srcPoint.des_para.RODEND.len_offset_dist;
		mirPoint.des_para.RODEND.wing_offset_style=srcPoint.des_para.RODEND.wing_offset_style;
		mirPoint.des_para.RODEND.wing_offset_dist=srcPoint.des_para.RODEND.wing_offset_dist;
		if(pAngle&&srcPoint.des_para.RODEND.wing_offset_style==4)
		{	//�Գ�ʱ����ʶ�����ƫ������(g,g1,g2,g3) wjh-2015.3.7
			JGZJ defaultZJ;
			if(srcPoint.des_para.RODEND.offset_wing==0)	//X֫Ϊ��ǰ֫
				defaultZJ=pAngle->GetZhunJu('X');
			else
				defaultZJ=pAngle->GetZhunJu('Y');
			srcPoint.des_para.RODEND.wing_offset_style=defaultZJ.MatchStyleG(srcPoint.des_para.RODEND.wing_offset_dist,false);
			/** δ����g1=0��wing_offset_dist=0�������ʱ����г�1,�ں������ĽǸֹ��ʱ�������⡡wjh-2018.1.4
			if(srcPoint.des_para.RODEND.wing_offset_dist==defaultZJ.g)
				srcPoint.des_para.RODEND.wing_offset_style=0;
			else if(srcPoint.des_para.RODEND.wing_offset_dist==defaultZJ.g1)
				srcPoint.des_para.RODEND.wing_offset_style=1;
			else if(srcPoint.des_para.RODEND.wing_offset_dist==defaultZJ.g2)
				srcPoint.des_para.RODEND.wing_offset_style=2;
			else if(srcPoint.des_para.RODEND.wing_offset_dist==defaultZJ.g3)
				srcPoint.des_para.RODEND.wing_offset_style=3;
			*/
		}
		CDesignThickPara dest;
		MirThickPara(CDesignThickPara(srcPoint.des_para.RODEND.norm_offset),dest,msg);
		strcpy(mirPoint.des_para.RODEND.norm_offset,dest.key_str);
	}
	else if(mirPoint.datum_pos_style==2)	//�ڵ���Ǹ�����ͶӰ��ƽ��
	{
		mirPoint.des_para.RODNODE=srcPoint.des_para.RODNODE;
		MirPartHandle(pTower,&mirPoint.des_para.RODNODE.hRod,srcPoint.des_para.RODNODE.hRod,msg);
		MirPartHandle(pTower,&mirPoint.des_para.RODNODE.hNode,srcPoint.des_para.RODNODE.hNode,msg);
		CLDSLineAngle *pAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(srcPoint.des_para.RODNODE.hRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pAngle)
		{
			f3dPoint mirnorm;
			if(srcPoint.des_para.RODNODE.offset_wing==0)
				mirnorm=GetMirVector(pAngle->get_norm_x_wing(),msg);
			else
				mirnorm=GetMirVector(pAngle->get_norm_y_wing(),msg);
			CLDSLineAngle *pMirAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(mirPoint.des_para.RODNODE.hRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pMirAngle)
			{
				if(fabs(pMirAngle->get_norm_x_wing()*mirnorm)>fabs(pMirAngle->get_norm_y_wing()*mirnorm))
					mirPoint.des_para.RODNODE.offset_wing=0;
				else
					mirPoint.des_para.RODNODE.offset_wing=1;
				line_vec=pAngle->End()-pAngle->Start();
				mir_line_vec=pMirAngle->End()-pMirAngle->Start();
				line_vec_tm=GetMirVector(line_vec,msg);
				if(line_vec_tm*mir_line_vec>0)
					mirPoint.des_para.RODNODE.direction=srcPoint.des_para.RODNODE.direction;
				else
					mirPoint.des_para.RODNODE.direction=1-srcPoint.des_para.RODNODE.direction;
			}
		}
		mirPoint.des_para.RODNODE.bFaceOffset=srcPoint.des_para.RODNODE.bFaceOffset;
		f3dPoint face_norm_offset=GetMirVector(srcPoint.des_para.RODNODE.vPlaneNormal,msg);
		mirPoint.des_para.RODNODE.vPlaneNormal=face_norm_offset;
		mirPoint.des_para.RODNODE.len_offset_dist=srcPoint.des_para.RODNODE.len_offset_dist;
		mirPoint.des_para.RODNODE.wing_offset_style=srcPoint.des_para.RODNODE.wing_offset_style;
		mirPoint.des_para.RODNODE.wing_offset_dist=srcPoint.des_para.RODNODE.wing_offset_dist;
		if(srcPoint.des_para.RODEND.wing_offset_style==4)
		{	//�Գ�ʱ����ʶ�����ƫ������(g,g1,g2,g3) wjh-2015.3.7
			JGZJ defaultZJ;
			if(srcPoint.des_para.RODNODE.offset_wing==0)	//X֫Ϊ��ǰ֫
				defaultZJ=pAngle->GetZhunJu('X');
			else
				defaultZJ=pAngle->GetZhunJu('Y');
			srcPoint.des_para.RODNODE.wing_offset_style=defaultZJ.MatchStyleG(srcPoint.des_para.RODNODE.wing_offset_dist,false);
			/** δ����g1=0��wing_offset_dist=0�������ʱ����г�1,�ں������ĽǸֹ��ʱ�������⡡wjh-2018.1.4
			if(srcPoint.des_para.RODNODE.wing_offset_dist==defaultZJ.g)
				srcPoint.des_para.RODNODE.wing_offset_style=0;
			else if(srcPoint.des_para.RODNODE.wing_offset_dist==defaultZJ.g1)
				srcPoint.des_para.RODNODE.wing_offset_style=1;
			else if(srcPoint.des_para.RODNODE.wing_offset_dist==defaultZJ.g2)
				srcPoint.des_para.RODNODE.wing_offset_style=2;
			else if(srcPoint.des_para.RODNODE.wing_offset_dist==defaultZJ.g3)
				srcPoint.des_para.RODNODE.wing_offset_style=3;
			*/
		}
		CDesignThickPara dest;
		MirThickPara(CDesignThickPara(srcPoint.des_para.RODNODE.norm_offset),dest,msg);
		strcpy(mirPoint.des_para.RODNODE.norm_offset,dest.key_str);
	}
	else if(mirPoint.datum_pos_style==3)	//���߽���
	{
		mirPoint.des_para.AXIS_INTERS=srcPoint.des_para.AXIS_INTERS;
		MirPartHandle(pTower,&mirPoint.des_para.AXIS_INTERS.hDatum1,srcPoint.des_para.AXIS_INTERS.hDatum1,msg);
		MirPartHandle(pTower,&mirPoint.des_para.AXIS_INTERS.hDatum2,srcPoint.des_para.AXIS_INTERS.hDatum2,msg);
		f3dPoint nearVector;
		nearVector.x=srcPoint.des_para.AXIS_INTERS.near_vector[0];
		nearVector.y=srcPoint.des_para.AXIS_INTERS.near_vector[1];
		nearVector.z=srcPoint.des_para.AXIS_INTERS.near_vector[2];
		nearVector=GetMirVector(nearVector,msg);
		mirPoint.des_para.AXIS_INTERS.near_vector[0]=nearVector.x;
		mirPoint.des_para.AXIS_INTERS.near_vector[1]=nearVector.y;
		mirPoint.des_para.AXIS_INTERS.near_vector[2]=nearVector.z;
	}
	else if(mirPoint.datum_pos_style==6)	//��׼����ָ����������ڵ�
	{
		if(msg.axis_flag==0x01||msg.axis_flag==0x02||msg.axis_flag==0x04)
		{
			mirPoint.datum_pos_style=srcPoint.datum_pos_style;
			mirPoint.des_para.DATUMPLANE.hDatumPlane=srcPoint.des_para.DATUMPLANE.hDatumPlane;
			mirPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=srcPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2;
			//�Գƻ�׼�涨λ�Ķ�λ������Ϣ
			if(msg.axis_flag==0x01)	//X��Գ�
			{
				if(srcPoint.des_para.DATUMPLANE.cMirTransType==0)
					mirPoint.des_para.DATUMPLANE.cMirTransType='X';
				else if(srcPoint.des_para.DATUMPLANE.cMirTransType=='X')
					mirPoint.des_para.DATUMPLANE.cMirTransType=0;
				else if(srcPoint.des_para.DATUMPLANE.cMirTransType=='Y')
					mirPoint.des_para.DATUMPLANE.cMirTransType='Z';
				else if(srcPoint.des_para.DATUMPLANE.cMirTransType=='Z')
					mirPoint.des_para.DATUMPLANE.cMirTransType='Y';
			}
			else if(msg.axis_flag==0x02)	//Y��Գ�
			{
				if(srcPoint.des_para.DATUMPLANE.cMirTransType==0)
					mirPoint.des_para.DATUMPLANE.cMirTransType='Y';
				else if(srcPoint.des_para.DATUMPLANE.cMirTransType=='Y')
					mirPoint.des_para.DATUMPLANE.cMirTransType=0;
				else if(srcPoint.des_para.DATUMPLANE.cMirTransType=='X')
					mirPoint.des_para.DATUMPLANE.cMirTransType='Z';
				else if(srcPoint.des_para.DATUMPLANE.cMirTransType=='Z')
					mirPoint.des_para.DATUMPLANE.cMirTransType='X';
			}
			else if(msg.axis_flag==0x04)	//Z��Գ�
			{
				if(srcPoint.des_para.DATUMPLANE.cMirTransType==0)
					mirPoint.des_para.DATUMPLANE.cMirTransType='Z';
				else if(srcPoint.des_para.DATUMPLANE.cMirTransType=='Z')
					mirPoint.des_para.DATUMPLANE.cMirTransType=0;
				else if(srcPoint.des_para.DATUMPLANE.cMirTransType=='X')
					mirPoint.des_para.DATUMPLANE.cMirTransType='Y';
				else if(srcPoint.des_para.DATUMPLANE.cMirTransType=='Y')
					mirPoint.des_para.DATUMPLANE.cMirTransType='X';
			}
		}
	}
	else if(mirPoint.datum_pos_style==8)	//�ڵ������ͬλ
	{
		MirPartHandle(pTower,&mirPoint.des_para.hNode,srcPoint.des_para.hNode,msg);
	}
	else if(mirPoint.datum_pos_style==9)	//��׼�����ϵ��������
	{	
		mirPoint.datum_pos=srcPoint.datum_pos;
		MirPartHandle(pTower,&mirPoint.des_para.hPart,srcPoint.des_para.hPart,msg);
		if(pTower)
		{
			CLDSPart *pDatumPart=pTower->FromPartHandle(srcPoint.des_para.hPart);
			CLDSPart *pMirDatumPart=pTower->FromPartHandle(mirPoint.des_para.hPart);
			if(pDatumPart==NULL||pMirDatumPart==NULL)
				return ;
			f3dPoint datum_pos=srcPoint.datum_pos;
			coord_trans(datum_pos,pDatumPart->ucs,TRUE);
			mirPoint.datum_pos=GetMirPos(datum_pos,msg);
			coord_trans(mirPoint.datum_pos,pMirDatumPart->ucs,FALSE);
		}
	}
	else if(mirPoint.datum_pos_style==10)	//���ڵ��ı����ȷֵ� wht 12-03-19
	{	
		MirPartHandle(pTower,&mirPoint.des_para.SCALE_NODE.hLinePart,srcPoint.des_para.SCALE_NODE.hLinePart,msg);
		MirPartHandle(pTower,&mirPoint.des_para.SCALE_NODE.hStartNode,srcPoint.des_para.SCALE_NODE.hStartNode,msg);
		MirPartHandle(pTower,&mirPoint.des_para.SCALE_NODE.hEndNode,srcPoint.des_para.SCALE_NODE.hEndNode,msg);
		mirPoint.des_para.SCALE_NODE.start_offset_dist=srcPoint.des_para.SCALE_NODE.start_offset_dist;
		mirPoint.des_para.SCALE_NODE.end_offset_dist=srcPoint.des_para.SCALE_NODE.end_offset_dist;
		mirPoint.des_para.SCALE_NODE.offset_dist=srcPoint.des_para.SCALE_NODE.offset_dist;
		mirPoint.des_para.SCALE_NODE.offset_scale=srcPoint.des_para.SCALE_NODE.offset_scale;
	}
	else if(mirPoint.datum_pos_style==11)
	{	//�������߶�λ��
		MirPartHandle(pTower,&mirPoint.des_para.COLUMN_INTERS.hLineTube,srcPoint.des_para.COLUMN_INTERS.hLineTube,msg);
			//�Ե͵���ָ��λΪ1��ʾ��Ӧʸ�����Ϊ�����������
			//��1λ:axis_vec;
			//��2λ:sect_center;
			//��3λ:sect_norm;
		mirPoint.des_para.COLUMN_INTERS.cPointAndVectorFlag=srcPoint.des_para.COLUMN_INTERS.cPointAndVectorFlag;
		mirPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec=msg.MirVector(srcPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec);
		mirPoint.des_para.COLUMN_INTERS.SectCenter.sect_center=msg.MirPoint(srcPoint.des_para.COLUMN_INTERS.SectCenter.sect_center);
		mirPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm=msg.MirVector(srcPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm);
		mirPoint.des_para.COLUMN_INTERS.SectCenterParam.len_offset=srcPoint.des_para.COLUMN_INTERS.SectCenterParam.len_offset;
		MirPartHandle(pTower,&mirPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode,abs(srcPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode),msg);
		if(srcPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode<0)
			mirPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode*=-1;
		mirPoint.des_para.COLUMN_INTERS.ray_vec_style=srcPoint.des_para.COLUMN_INTERS.ray_vec_style;
		mirPoint.des_para.COLUMN_INTERS.RayVector.ray_vec=msg.MirVector(srcPoint.des_para.COLUMN_INTERS.RayVector.ray_vec);
		double angle=srcPoint.des_para.COLUMN_INTERS.ray_angle;
		CLDSLineTube* pTube=(CLDSLineTube*)pTower->FromPartHandle(mirPoint.des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE);
		if((mirPoint.des_para.COLUMN_INTERS.ray_vec_style==2||mirPoint.des_para.COLUMN_INTERS.ray_vec_style==3)&&pTube)
		{	//��ָ���ֹ���ʼ�߻򺸵��߿�ʼ��������ray_angleȷ�����߷���
			pTube->BuildUCS();
			f3dPoint mir_ray_vec=mirPoint.des_para.COLUMN_INTERS.RayVector.ray_vec;
			//angle=CalRotateAngle(pTube->ucs.axis_z,pTube->ucs.axis_x,mir_ray_vec);
			//ʹ��CalRotateAngle����Ƕȴ����²۸ֶԳƴ��� wht 18-12-29
			vector_trans(mir_ray_vec,pTube->ucs,FALSE);
			angle=Cal2dLineAng(0,0,mir_ray_vec.x,mir_ray_vec.y);
			double weldRoadAngle=0;
			if(mirPoint.des_para.COLUMN_INTERS.ray_vec_style==3)	//�ֹܺ�����Ϊ������ʼ��)
				weldRoadAngle=pTube->CalWeldRoadAngle();
			angle-=weldRoadAngle;
			if(angle<0)
				angle+=2*Pi;
			angle*=DEGTORAD_COEF;
		}
		mirPoint.des_para.COLUMN_INTERS.ray_angle=angle;
		mirPoint.des_para.COLUMN_INTERS.column_r=srcPoint.des_para.COLUMN_INTERS.column_r;
		mirPoint.des_para.COLUMN_INTERS.ray_para_offset=srcPoint.des_para.COLUMN_INTERS.ray_para_offset;
		mirPoint.des_para.COLUMN_INTERS.ray_offset_r=srcPoint.des_para.COLUMN_INTERS.ray_offset_r;
		mirPoint.des_para.COLUMN_INTERS.bSpecRo=srcPoint.des_para.COLUMN_INTERS.bSpecRo;
	}
#endif
}

void MirDatumVector(IModel *pTower,CLDSVector &srcVector,CLDSVector &mirVector,MIRMSG msg,f3dPoint *pPickPos=NULL)
{
	srcVector.UpdateVector(pTower);		//srcVectorδ����ǰ���жԳƣ����ܵ��µ�ǰ֫�жϴ��� wht 16-09-09
	mirVector.direction=srcVector.direction;
	mirVector.vector=GetMirVector(srcVector.vector,msg);
	mirVector.norm_style=srcVector.norm_style;
	MirPartHandle(pTower,&mirVector.hVicePart,srcVector.hVicePart,msg);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	//δ�ҵ��ԳƸ˼�ʱ�������øֹ�һ�˶Գ�,���ҶԳƹ��� wht 11-05-24
	CLDSLineTube *pViceTube=(CLDSLineTube*)pTower->FromPartHandle(srcVector.hVicePart,CLS_LINETUBE);
	MirPartHandle(pTower,&mirVector.hVicePart,srcVector.hVicePart,msg,pPickPos);
#endif
	if(mirVector.norm_style==2)
	{	//�˼������淨��
		MirPartHandle(pTower,&mirVector.hCrossPart,srcVector.hCrossPart,msg,pPickPos);
		mirVector.nearVector=GetMirVector(srcVector.nearVector,msg);
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(mirVector.norm_style==1)
	{	//�Ǹ�֫����
		CLDSLineAngle *pMirAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(mirVector.hVicePart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pMirAngle)
		{
			double ddx=pMirAngle->get_norm_x_wing()*mirVector.vector;
			double ddy=pMirAngle->get_norm_y_wing()*mirVector.vector;
			if(fabs(ddx)>fabs(ddy))
			{
				if(ddx>0)
					mirVector.direction=0;
				else
					mirVector.direction=1;
				mirVector.norm_wing=0;
			}
			else
			{
				if(ddy>0)
					mirVector.direction=0;
				else
					mirVector.direction=1;
				mirVector.norm_wing=1;
			}
		}
	}
	else if(mirVector.norm_style==4)
	{	//��׼�����ϵ���Է���
		mirVector.nearVector=srcVector.nearVector;
		if(pTower)
		{
			CLDSPart *pDatumPart=pTower->FromPartHandle(srcVector.hVicePart);
			CLDSPart *pMirDatumPart=pTower->FromPartHandle(mirVector.hVicePart);
			if(pDatumPart==NULL||pMirDatumPart==NULL)
				return ;
			f3dPoint datum_vec=srcVector.nearVector;
			vector_trans(datum_vec,pDatumPart->ucs,TRUE);
			mirVector.nearVector=GetMirVector(datum_vec,msg);
			vector_trans(mirVector.nearVector,pMirDatumPart->ucs,FALSE);
		}
	}
	else if(mirVector.norm_style==5)
	{	//��׼�˼��ϵ��������� wht 11-05-17
		CLDSLineAngle *pMirAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(mirVector.hVicePart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pMirAngle)
		{
			double ddx=pMirAngle->get_norm_x_wing()*mirVector.vector;
			double ddy=pMirAngle->get_norm_y_wing()*mirVector.vector;
			if(fabs(ddx)>fabs(ddy))
			{
				if(ddx>0)
					mirVector.direction=2;	//�Ǹ�X֫������
				else
					mirVector.direction=3;	//�Ǹ�X֫������
				mirVector.norm_wing=0;
			}
			else
			{
				if(ddy>0)
					mirVector.direction=4;	//�Ǹ�Y֫������
				else
					mirVector.direction=5;	//�Ǹ�Y֫������
				mirVector.norm_wing=1;
			}
		}
	}
#endif
}

static BOOL MirNode(CLDSNode *pSrcNode, MIRMSG mirmsg,BOOL bIncAttachInfo=TRUE,char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//�Ƿ�ɹ��Գ��������й���
	RELATIVE_OBJECT *pRelaObj=NULL;
	if(bIncAttachInfo&&!IsNodeMirValid(pSrcNode,mirmsg))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "fail to pass symmetry legality checking, parts can not be symmetrical";
#else
		throw "δͨ���ԳƺϷ��Լ��,���ܶԳƹ���";
#endif

	int i,flagArr[4]={1,2,4,16};
	CLDSNode* srcnode_arr[4]={NULL};
	NODESET newnodeset;
	srcnode_arr[0]=pSrcNode;
	for(i=0;i<4;i++)
	{
		if((mirmsg.axis_flag&flagArr[i])==0)
			continue;
		MIRMSG msg=mirmsg.GetSubMirItem(i+1);
		srcnode_arr[i+1]=CreateSingleMirNode(pSrcNode,msg,bIncAttachInfo);
		if(srcnode_arr[i+1]==NULL)
			return FALSE;
		else
			newnodeset.append(srcnode_arr[i+1]);
	}
	int array_num=mirmsg.array_num;
	if((mirmsg.axis_flag&8)==0)	//����ת�Գ�
		array_num=0;
	else
		mirmsg.axis_flag=8;
	srcnode_arr[0]=pSrcNode;
	short init_angle=mirmsg.rotate_angle;	
	for(i=0;i<4;i++)
	{
		if(srcnode_arr[i]==NULL)
			continue;
		pSrcNode = srcnode_arr[i];
		for(int j=0;j<array_num;j++)
		{	//���ж����ת�Գ�ʱ��׼����Ϊͬһ��������ֻ��Ҫ������ת�Ƕȼ���
			if(pSrcNode==NULL)
				continue;
			//������ת�Ƕ�
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSNode *pMirNode=CreateSingleMirNode(pSrcNode,mirmsg,bIncAttachInfo);
			if(pMirNode)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcnode_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcnode_arr[0]->relativeObjs.GetNext())
				{	//�����ظ���ӹ�������
					if(pRelaObj->hObj==pMirNode->handle)
					{
						bInList=TRUE;
						break;
					}
				}
				if(bInList)
					continue;
				pRelaObj=srcnode_arr[0]->relativeObjs.Add(pMirNode->handle);
				srcnode_arr[0]->SetModified(TRUE,FALSE);
				pRelaObj->mirInfo.rotate_angle=init_angle*(j+1);
				pRelaObj->mirInfo.axis_flag=mirmsg.axis_flag;
				if(i>0)
					pRelaObj->mirInfo.axis_flag+=flagArr[i-1];
				newnodeset.append(pMirNode);
			}
			else
				bSucceed=FALSE;
		}
	}
	//���öԳ����ɵĶ���˴˹���
	SetRelativeObj(srcnode_arr[0]);
#if defined(__TSA_)||defined(__LDS_)
	if(bIncAttachInfo)
	{
		for(CLDSNode* pNode=newnodeset.GetFirst();pNode;pNode=newnodeset.GetNext())
		{
			if(pNode->m_cPosCalType==4)	//����ڵ�Գ�ʱ��Ҫ���ǽ���ϸ�Ȳ��ո˼�Ҳ���и���
			{
				CSuperSmartPtr<CLDSPart> pPole1=Ta.Parts.FromHandle(pNode->arrRelationPole[0]);
				CSuperSmartPtr<CLDSPart> pPole2=Ta.Parts.FromHandle(pNode->arrRelationPole[1]);
				if(pPole1.IsHasPtr()&&pPole2.IsHasPtr()&&pPole1->IsLinePart()&&pPole2->IsLinePart())
				{
					pPole1.LinePartPointer()->CalLenCoef.hRefPole=pPole2->handle;
					pPole2.LinePartPointer()->CalLenCoef.hRefPole=pPole1->handle;
				}
			}
		}
	}
#endif
	return bSucceed;
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
CLDSBolt *CreateSingleMirBolt(CLDSBolt* pBolt, MIRMSG msg, BOOL bIncAttachInfo/*=TRUE*/, THANDLE* relaPartIdArr/*=NULL*/,UINT nRelaPartCount/*=0*/)
{
	CLDSBolt *pMirBolt=NULL;
	CMirOperObject::MIRROR* pMirror=CMirOperObject::AddOrGetMirror(pBolt->handle,pBolt);
	CMirOperObject::MIRROR::MIROBJ* pMirObj=pMirror->GetMirObj(msg);
	pMirBolt=(CLDSBolt*)(pMirObj!=NULL?pMirObj->pMirObj:pBolt->GetMirPart(msg));
	if(pMirBolt)	//�Գ���˨�Ѿ�����
	{
		if (pMirObj==NULL)
		{	//�ԳƲ���ǰ�Ѵ��ڶԳ���˨�������������˨�����Ϣ�������й������� wjh-2020.1.14
			CMirOperObject::MIRROR::MIROBJ mirobj;
			mirobj.ciMirFlag=msg.axis_flag;
			mirobj.siRotDegAng=msg.rotate_angle;
			mirobj.dwStateFlag=CMirOperObject::MIRBOLT_ALL;
			mirobj.pMirObj=pMirBolt;
			pMirObj=pMirror->xarrMirObjs.Append(mirobj);
		}
		if(pBolt->IsFootNail()&&!pMirBolt->IsFootNail()//Դ��˨Ϊ�Ŷ����Գ���˨�ǽŶ�
			&&pBolt->get_d()==pMirBolt->get_d())		//����˨ֱ�����
		{	//ԭ��˨Ϊ�Ŷ����Գ���˨�ǽŶ�ʱ�����Գ���˨Ϊ�Ŷ� wht 11-08-09
			pMirBolt->PushInitialStdBoltL();
			pMirBolt->set_L(pBolt->get_L());
			pMirBolt->m_hFamily = CLsLibrary::GetFootNailFamily();
			pMirBolt->m_dwFlag.SetFlagDword(CLDSBolt::FOOT_NAIL,TRUE);
			pMirBolt->m_cFuncType=pBolt->m_cFuncType;	//�������������
			pMirBolt->CalGuigeAuto();
			pMirBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pMirBolt->GetSolidPartObject());
			g_pSolidDraw->Draw();
			return pMirBolt;
		}
	}
	else
	{
		pMirBolt = (CLDSBolt*)console.AppendPart(CLS_BOLT);
		pMirBolt->CopyProperty(pBolt,pBolt->liInitL>0);	//������˨���Ǵ��׽Ŷ����Գ���˨Ҳ�������Ǵ��׽Ŷ�
		pMirBolt->cfgword=pBolt->cfgword;
		pMirBolt->ucs.origin=GetMirPos(pBolt->ucs.origin,msg);
		pMirBolt->ucs.axis_z=GetMirVector(pBolt->ucs.axis_z,msg);
		AddMirObjToSrcObjRelaObjList(pBolt,pMirBolt,msg);	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
		
		if (pMirObj==NULL)
		{	//�¶Գ���˨����
			CMirOperObject::MIRROR::MIROBJ mirobj;
			mirobj.ciMirFlag=msg.axis_flag;
			mirobj.siRotDegAng=msg.rotate_angle;
			mirobj.dwStateFlag=0;
			mirobj.pMirObj=pMirBolt;
			pMirObj=pMirror->xarrMirObjs.Append(mirobj);
		}
	}
	CalMirAtomLayer(pBolt->layer(),pMirBolt->layer(),msg);
	if(!bIncAttachInfo)
		return pMirBolt;
	CParaThick designL0;
	MirThickPara(*pBolt->GetL0ThickPara(),designL0,msg);
	pMirBolt->SetL0ThickPara(designL0);//key_str);
	//���������Ϣ�Գ�
	pMirBolt->des_work_norm=pBolt->des_work_norm;
	MirPartHandle(pBolt->BelongModel(),&pMirBolt->des_work_norm.hVicePart,pBolt->des_work_norm.hVicePart,msg);
	MirPartHandle(pBolt->BelongModel(),&pMirBolt->des_work_norm.hCrossPart,pBolt->des_work_norm.hCrossPart,msg);
	//���´�����ʱע�͵� ��δ�ҵ���˨������ƻ�׼����ʱ
	//��Ӧʹ�öԳ���˨�ķ�����Ʋ�����������˨���߲���ȷ wht 10-03-17
	//if(pMirBolt->des_work_norm.hVicePart==0)
	//	pMirBolt->des_work_norm.hVicePart=pBolt->des_work_norm.hVicePart;
	//if(pMirBolt->des_work_norm.hCrossPart==0)
	//	pMirBolt->des_work_norm.hCrossPart=pBolt->des_work_norm.hCrossPart;
	pMirBolt->des_work_norm.nearVector=GetMirVector(pBolt->des_work_norm.nearVector,msg);
	if (pBolt->des_work_norm.norm_style==4)	//��׼�����ϵ���Է���
	{
		pMirBolt->des_work_norm.nearVector=pBolt->des_work_norm.nearVector;
		if(pMirBolt->des_work_norm.hVicePart>0x20)
			pMirObj->dwStateFlag|=CMirOperObject::MIRBOLT_NORM_PARAM;
	}
	else if(pBolt->des_work_norm.norm_style==1)	//�Ǹ�֫����
	{
		CLDSLineAngle *pMirAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirBolt->des_work_norm.hVicePart);
		if(pMirAngle)
		{	//���ڻ�׼�Ǹ� wht 10-10-19
			pMirObj->dwStateFlag|=CMirOperObject::MIRBOLT_NORM_PARAM;
			double ddx=pMirAngle->get_norm_x_wing()*pMirBolt->ucs.axis_z;
			double ddy=pMirAngle->get_norm_y_wing()*pMirBolt->ucs.axis_z;
			if(fabs(ddx)>fabs(ddy))
			{
				if(ddx>0)
					pMirBolt->des_work_norm.direction=0;
				else
					pMirBolt->des_work_norm.direction=1;
				pMirBolt->des_work_norm.norm_wing=0;
			}
			else
			{
				if(ddy>0)
					pMirBolt->des_work_norm.direction=0;
				else
					pMirBolt->des_work_norm.direction=1;
				pMirBolt->des_work_norm.norm_wing=1;
			}
			strcpy(pMirBolt->des_base_pos.norm_offset.key_str,"");
			if(pMirBolt->des_work_norm.direction==0)	//����
				pMirBolt->des_base_pos.norm_offset.AddThick(-pMirBolt->des_base_pos.hPart,TRUE);
		}
	}
	//����λ�������Ϣ�Գ�
	pMirBolt->des_base_pos=pBolt->des_base_pos;
	pMirObj->dwStateFlag|=CMirOperObject::MIRBOLT_POS_PARAM;
	if (pBolt->des_base_pos.datumPoint.MirToPoint(pBolt->BelongModel(),pMirBolt->des_base_pos.datumPoint,msg)<0)
	{
		logerr.Log("0x%X��˨�Ķ�λ����Գƴ��ڴ���",pBolt->handle);
		pMirObj->RemoveState(CMirOperObject::MIRBOLT_POS_PARAM);
	}
	MirPartHandle(pBolt->BelongModel(),&pMirBolt->des_base_pos.hPart,pBolt->des_base_pos.hPart,msg);
	//���´�����ʱע�͵� ��δ�ҵ���˨λ����ƻ�׼����ʱ
	//��Ӧʹ�öԳ���˨��λ����Ʋ�����������˨λ�û᲻��ȷ  wht 10-03-17
	//if(pMirBolt->des_base_pos.hPart==0)
	//	pMirBolt->des_base_pos.hPart=pBolt->des_base_pos.hPart;
	CSuperSmartPtr<CLDSPart> pDatumPart=Ta.Parts.FromHandle(pBolt->des_base_pos.hPart);
	CSuperSmartPtr<CLDSPart> pMirDatumPart=Ta.Parts.FromHandle(pMirBolt->des_base_pos.hPart);
	if(pDatumPart.IsHasPtr()&&pMirDatumPart.IsHasPtr())
	{
		if(pDatumPart->IsLinePart())
		{	//��Ҫָ�Ǹֹ���
			f3dPoint line_vec=pDatumPart.LinePartPointer()->End()-pDatumPart.LinePartPointer()->Start();
			f3dPoint mir_line_vec=GetMirVector(line_vec,msg);
			line_vec=pMirDatumPart.LinePartPointer()->End()-pMirDatumPart.LinePartPointer()->Start();
			if(line_vec*mir_line_vec<0)
				pMirBolt->des_base_pos.direction=1-pMirBolt->des_base_pos.direction;
		}
		else if(pDatumPart->IsPlate())
		{	//��Ҫָ�ְ幹��
			f3dPoint mir_axis_x=GetMirVector(pDatumPart->ucs.axis_x,msg);
			f3dPoint mir_axis_y=GetMirVector(pDatumPart->ucs.axis_y,msg);
			//f3dPoint mir_axis_z=GetMirVector(pDatumPart->ucs.axis_z,msg);//�ݲ�������ᷴ�����
			if(mir_axis_x*pMirDatumPart->ucs.axis_x<0)	//�Գƺ�ģ����ѽ����˷���
				pMirBolt->des_base_pos.len_offset_dist*=-1.0;
			if(mir_axis_y*pMirDatumPart->ucs.axis_y<0)	//�Գƺ�ģ����ѽ����˷���
				pMirBolt->des_base_pos.wing_offset_dist*=-1.0;
		}
	}
	else
		pMirObj->RemoveState(CMirOperObject::MIRBOLT_POS_PARAM);
	if(pMirDatumPart.IsHasPtr()&&pMirDatumPart->GetClassTypeId()==CLS_LINEANGLE)
	{	//����˨֫������Ʋ���Ϊָ������ʱ�������ж���˨�ڽǸ��ϵĵ�ǰ����֫
		if( fabs(pMirDatumPart.LineAnglePointer()->get_norm_x_wing()*pMirBolt->get_norm())>
			fabs(pMirDatumPart.LineAnglePointer()->get_norm_y_wing()*pMirBolt->get_norm()))
			pMirBolt->des_base_pos.offset_wing=0;
		else
			pMirBolt->des_base_pos.offset_wing=1;
	}
	else
		pMirBolt->des_base_pos.offset_wing=(BYTE)pMirBolt->des_work_norm.norm_wing;
	//��������ͬ�����У��������ظ�����ע�͵� wjh-2015.10.8
	//pBolt->des_base_pos.datumPoint.MirToPoint(pBolt->BelongModel(),pMirBolt->des_base_pos.datumPoint,msg);
	MirThickPara(pBolt->des_base_pos.norm_offset,pMirBolt->des_base_pos.norm_offset,msg);
	//pMirBolt->correct_worknorm();
	//pMirBolt->correct_pos();
	if (UI::blEnableIntermediateUpdateUI)
	{
		pMirBolt->Create3dSolidModel();
		if (!pMirBolt->m_bVirtualPart || (pMirBolt->m_bVirtualPart&&g_sysPara.display.bDispVirtualPart))
			g_pSolidDraw->NewSolidPart(pMirBolt->GetSolidPartObject());
	}
	CLDSPart* pPart;
	CHashSet<CLDSPartPtr> hashRelaParts;
	//������绷�������Ĵ������Ĺ������󼯺ϡ�wjh-2015.3.7
	for(UINT i=0;relaPartIdArr!=NULL&&i<nRelaPartCount;i++)
	{
		pPart=pBolt->BelongModel()->FromPartHandle(relaPartIdArr[i]);
		if(pPart)
			hashRelaParts.SetValue(pPart->handle,pPart);
	}
	//����ͨ���ַ�����ȡ���ܵĹ������󼯺�
	if(hashRelaParts.GetValue(pBolt->des_base_pos.hPart)==NULL)
	{
		pPart=pBolt->BelongModel()->FromPartHandle(pBolt->des_base_pos.hPart);
		if(pPart)
			hashRelaParts.SetValue(pPart->handle,pPart);
	}
	CParaThick* pThick=pBolt->GetL0ThickPara();
	for(THICK_ITEM item=pThick->GetFirstItem();!item.IsFinalKey();item=pThick->GetNextItem())
	{
		if(item.cItemType!=1)	//���Ǿ��
			continue;
		pPart=pBolt->BelongModel()->FromPartHandle(item.val);
		if(pPart)
			hashRelaParts.SetValue(pPart->handle,pPart);
	}
	//�Գ���˨����  �����ҵ���˨������Ĺ���,Ȼ����˨���뵽��Ӧ�Ĺ���������
	//if(pMirBolt->IsFootNail())
	for(pPart=hashRelaParts.GetFirst();pPart!=NULL;pPart=hashRelaParts.GetNext())
	{	
		if(pPart->GetClassTypeId()==CLS_LINEANGLE&&pPart->GetLsRefList()->FromHandle(pBolt->handle))
		{
			CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pPart;
			CLDSLineAngle* pMirAngle=(CLDSLineAngle*)pLineAngle->GetMirPart(msg,pBolt->ucs.origin);
			if(pMirAngle==NULL)
				continue;
			//Ĭ����˨��ӵ��Ǹ��м�λ����Ȼ��һ���ԣ���������ܻᵼ�¶˵�ĵ���˨���ʧ�� wjh-2015.8.13
			pMirAngle->AppendMidLsRef(pMirBolt->GetLsRef());
			CLDSGroupLineAngle *pGroupJg=NULL;
			if(pMirAngle->group_father_jg_h>0x20)
				pGroupJg=(CLDSGroupLineAngle*)Ta.Parts.FromHandle(pMirAngle->group_father_jg_h,CLS_GROUPLINEANGLE);
			if(pGroupJg&&pGroupJg->group_style==2)
			{
				for(int i=0;i<4;i++)
				{
					if(pGroupJg->son_jg_h[i]==pMirAngle->handle)
						continue;
					CLDSLineAngle *pSonJg=(CLDSLineAngle*)Ta.Parts.FromHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
					if(pSonJg==NULL)
						continue;
					if(pSonJg->IsLsInWing(pMirBolt))
					{
						pSonJg->AppendMidLsRef(pMirBolt->GetLsRef());
						break;
					}
				}
			}
		}
		else if(pPart->FindLsByHandle(pBolt->handle)!=NULL)
		{
			CLDSPart *pMirPart=pPart->GetMirPart(msg);
			if(pMirPart!=NULL)
				pMirPart->GetLsRefList()->append(pMirBolt->GetLsRef());
		}
	}
	/*
	for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->IsLinePart())
		{
			if(((CLDSLinePart*)pPart)->FindLsByHandle(pBolt->handle))
			{
				CLDSPart *pMirPart=pPart->GetMirPart(msg);
				if(pMirPart&&pPart->GetClassTypeId()==pMirPart->GetClassTypeId())
					((CLDSLinePart*)pMirPart)->AppendMidLsRef(pBolt->GetLsRef());
			}
		}
		else if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			if(((CLDSPlate*)pPart)->FindLsByHandle(pBolt->handle))
			{
				CLDSPart *pMirPlate=pPart->GetMirPart(msg);
				if(pMirPlate&&pPart->GetClassTypeId()==pMirPlate->GetClassTypeId())
					((CLDSPlate*)pMirPlate)->AppendLsRef(pBolt->GetLsRef());
			}
		}
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			if(((CLDSParamPlate*)pPart)->FindLsByHandle(pBolt->handle))
			{
				CLDSPart *pMirPlate=pPart->GetMirPart(msg);
				if(pMirPlate&&pPart->GetClassTypeId()==pMirPlate->GetClassTypeId())
					((CLDSParamPlate*)pMirPlate)->AppendLsRef(pBolt->GetLsRef());
			}
		}
	}
	*/
	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
	AddMirObjToSrcObjRelaObjList(pBolt,pMirBolt,msg);
	return pMirBolt;
}
BOOL MirBolt(CLDSBolt* pSrcBolt, MIRMSG mirmsg, BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//�Ƿ�ɹ��Գ��������й���
	RELATIVE_OBJECT *pRelaObj=NULL;
	int flagArr[4]={1,2,4,16};
	CLDSBolt* srcbolt_arr[4]={NULL};
	srcbolt_arr[0]=pSrcBolt;
	for(int i=0;i<4;i++)
	{
		if((mirmsg.axis_flag&flagArr[i])==0)
			continue;
		MIRMSG msg=mirmsg.GetSubMirItem(i+1);
		srcbolt_arr[i+1]=CreateSingleMirBolt(pSrcBolt,msg,bIncAttachInfo);
	}
	int array_num=mirmsg.array_num;
	if((mirmsg.axis_flag&8)==0)	//����ת�Գ�
		array_num=0;
	else
		mirmsg.axis_flag=8;
	short init_angle=mirmsg.rotate_angle;
	for(i=0;i<4;i++)
	{
		if(srcbolt_arr[i]==NULL)
			continue;
		pSrcBolt = srcbolt_arr[i];
		for(int j=0;j<array_num;j++)
		{	//���ж����ת�Գ�ʱ��׼����Ϊͬһ��������ֻ��Ҫ������ת�Ƕȼ���
			if(pSrcBolt==NULL)
				continue;
			//������ת�Ƕ�
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSBolt *pMirBolt=CreateSingleMirBolt(pSrcBolt,mirmsg,bIncAttachInfo);
			if(pMirBolt)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcbolt_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcbolt_arr[0]->relativeObjs.GetNext())
				{	//�����ظ���ӹ�������
					if(pRelaObj->hObj==pMirBolt->handle)
					{
						bInList=TRUE;
						break;
					}
				}
				if(bInList)
					continue;
				pRelaObj=srcbolt_arr[0]->relativeObjs.Add(pMirBolt->handle);
				srcbolt_arr[0]->SetModified(TRUE,FALSE);
				pRelaObj->mirInfo.rotate_angle=init_angle*(j+1);
				pRelaObj->mirInfo.axis_flag=mirmsg.axis_flag;
				if(i>0)
					pRelaObj->mirInfo.axis_flag+=flagArr[i-1];
			}
			else
				bSucceed=FALSE;
		}
	}
	//���öԳ����ɵĶ���˴˹���
	SetRelativeObj(srcbolt_arr[0]);
	//�����Ƚ���һ����Գ��ٽ���һ����ת�Գƶ����ɵ���˨��
	//ֻ�ֱܷ��¼�����ζԳ�
	return bSucceed;
}
#endif
static CLDSLinePart *CreateSingleMirLinePart(CLDSLinePart *pSrcLinePart, MIRMSG msg,BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	CLDSLinePart *pMirLinePart=NULL;
	pMirLinePart=(CLDSLinePart*)pSrcLinePart->GetMirPart(msg);
	if(pMirLinePart==NULL)	//�ԳƸ˼�δ����
	{
		pMirLinePart=(CLDSLinePart*)console.AppendPart(pSrcLinePart->GetClassTypeId());
		pMirLinePart->SetSizeSpec(pSrcLinePart->GetSizeSpec());
		pMirLinePart->cMaterial=pSrcLinePart->cMaterial;
		if(pSrcLinePart->pStart)
			pMirLinePart->pStart = pSrcLinePart->pStart->GetMirNode(msg);
		if(pSrcLinePart->pEnd)
			pMirLinePart->pEnd = pSrcLinePart->pEnd->GetMirNode(msg);
		if(pMirLinePart->pStart==NULL&&pSrcLinePart->pStart)
			pMirLinePart->pStart = pSrcLinePart->pStart;
		if(pMirLinePart->pEnd==NULL&&pSrcLinePart->pEnd)
			pMirLinePart->pEnd = pSrcLinePart->pEnd;
		if(pMirLinePart->pStart&&pSrcLinePart->pStart->hFatherPart<0X20)
			pMirLinePart->pStart->hFatherPart=pMirLinePart->handle;
		if(pMirLinePart->pEnd&&pMirLinePart->pEnd->hFatherPart<0X20)
			pMirLinePart->pEnd->hFatherPart=pMirLinePart->handle;
		pMirLinePart->SetStart(GetMirPos(pSrcLinePart->Start(),msg));
		pMirLinePart->SetEnd(GetMirPos(pSrcLinePart->End(),msg));
		pMirLinePart->CopyProperty(pSrcLinePart);//�������ڵ�(����)����
		AddMirObjToSrcObjRelaObjList(pSrcLinePart,pMirLinePart,msg);	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
	}
	CalMirAtomLayer(pSrcLinePart->layer(),pMirLinePart->layer(),msg);
	if(!bIncAttachInfo)
		return pMirLinePart;
	//------vvvvvvvvv-------��˨���--------------------
	pMirLinePart->connectStart.d = pSrcLinePart->connectStart.d;
	pMirLinePart->connectStart.rows = pSrcLinePart->connectStart.rows;//ȱʡ������˨
	pMirLinePart->connectStart.uiLegacyN = pSrcLinePart->connectStart.uiLegacyN;
	pMirLinePart->connectEnd.d = pSrcLinePart->connectEnd.d;
	pMirLinePart->connectEnd.rows = pSrcLinePart->connectEnd.rows;//ȱʡ������˨
	pMirLinePart->connectEnd.uiLegacyN = pSrcLinePart->connectEnd.uiLegacyN;
	//------^^^^^^^^^-------��˨���--------------------
	NewLinePart(pMirLinePart);
	g_pSolidDraw->Draw();
	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
	AddMirObjToSrcObjRelaObjList(pSrcLinePart,pMirLinePart,msg);
	return pMirLinePart;
}
static BOOL MirLinePart(CLDSLinePart* pSrcLinePart, MIRMSG mirmsg,BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//�Ƿ�ɹ��Գ��������й���
	RELATIVE_OBJECT *pRelaObj=NULL;
	if(!IsPartMirValid(pSrcLinePart,mirmsg))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "fail to pass symmetry legality checking, parts can not be symmetrical";
#else
		throw "δͨ���ԳƺϷ��Լ��,���ܶԳƹ���";
#endif
	int flagArr[3]={1,2,4};
	CLDSLinePart* srcpole_arr[4]={NULL};
	srcpole_arr[0]=pSrcLinePart;
	for(int i=0;i<3;i++)
	{
		if((mirmsg.axis_flag&flagArr[i])==0)
			continue;
		MIRMSG msg=mirmsg.GetSubMirItem(i+1);
		srcpole_arr[i+1]=CreateSingleMirLinePart(pSrcLinePart,msg,bIncAttachInfo);
		if(srcpole_arr[i+1]==NULL)
			return FALSE;
#if defined(__LDS_)||defined(__TSA_)
		else
		{
			if(bIncAttachInfo)
			{
				if(pSrcLinePart->CalLenCoef.hHorizPole>0x20)
					MirPartHandle(&Ta,&srcpole_arr[i+1]->CalLenCoef.hHorizPole,pSrcLinePart->CalLenCoef.hHorizPole,mirmsg);
				if(pSrcLinePart->CalLenCoef.hRefPole>0x20)
					MirPartHandle(&Ta,&srcpole_arr[i+1]->CalLenCoef.hRefPole,pSrcLinePart->CalLenCoef.hRefPole,mirmsg);
			}
		}
#endif
	}
	int array_num=mirmsg.array_num;
	if((mirmsg.axis_flag&8)==0)	//����ת�Գ�
		array_num=0;
	else
		mirmsg.axis_flag=8;
	short init_angle=mirmsg.rotate_angle;
	for(i=0;i<4;i++)
	{
		if(srcpole_arr[i]==NULL)
			continue;
		pSrcLinePart = srcpole_arr[i];
		for(int j=0;j<array_num;j++)
		{	//���ж����ת�Գ�ʱ��׼����Ϊͬһ��������ֻ��Ҫ������ת�Ƕȼ���
			if(pSrcLinePart==NULL)
				continue;
			//������ת�Ƕ�
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSLinePart *pMirLinePart=CreateSingleMirLinePart(pSrcLinePart,mirmsg,bIncAttachInfo);
			if(pMirLinePart)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcpole_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcpole_arr[0]->relativeObjs.GetNext())
				{	//�����ظ���ӹ�������
					if(pRelaObj->hObj==pMirLinePart->handle)
					{
						bInList=TRUE;
						break;
					}
				}
				if(bInList)
					continue;
				pRelaObj=srcpole_arr[0]->relativeObjs.Add(pMirLinePart->handle);
				srcpole_arr[0]->SetModified(TRUE,FALSE);
				pRelaObj->mirInfo.rotate_angle=init_angle*(j+1);
				pRelaObj->mirInfo.axis_flag=mirmsg.axis_flag;
				if(i>0)
					pRelaObj->mirInfo.axis_flag+=flagArr[i-1];
#if defined(__TSA_)||defined(__LDS_)
				if(bIncAttachInfo)
				{
					if(pSrcLinePart->CalLenCoef.hHorizPole>0x20)
						MirPartHandle(&Ta,&pMirLinePart->CalLenCoef.hHorizPole,pSrcLinePart->CalLenCoef.hHorizPole,mirmsg);
					if(pSrcLinePart->CalLenCoef.hRefPole>0x20)
						MirPartHandle(&Ta,&pMirLinePart->CalLenCoef.hRefPole,pSrcLinePart->CalLenCoef.hRefPole,mirmsg);
				}
#endif
			}
			else
				bSucceed=FALSE;
		}
	}
	//���öԳ����ɵĶ���˴˹���
	SetRelativeObj(srcpole_arr[0]);
	return bSucceed;
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
void MirLinePartOddDesignInfo(CLDSLinePart* pSrcLinePart,CLDSLinePart* pMirLinePart,MIRMSG msg,bool bSrcStart,bool bMirStart)
{
	DESIGNODDMENT *pMirOddDesign=NULL,*pSrcOddDesign=NULL;
	if(pSrcLinePart==NULL||pMirLinePart==NULL)
		return;
	BOOL bUpdateSonAngleOddment=FALSE;
	if(pMirLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		bUpdateSonAngleOddment=((CLDSGroupLineAngle*)pMirLinePart)->m_bUpdateSonAngleOddment;
		((CLDSGroupLineAngle*)pMirLinePart)->m_bUpdateSonAngleOddment=FALSE;
	}
	if(bMirStart&&bSrcStart)	//�ԳƸ˼����Լ�����
		pMirLinePart->SetStartOdd(pSrcLinePart->startOdd());	
	else if(!bMirStart&&bSrcStart)
		pMirLinePart->SetEndOdd(pSrcLinePart->startOdd());
	else if(bMirStart&&!bSrcStart)
		pMirLinePart->SetStartOdd(pSrcLinePart->endOdd());
	else if(!bMirStart&&!bSrcStart)
		pMirLinePart->SetEndOdd(pSrcLinePart->endOdd());
	if(bSrcStart)
		pSrcOddDesign=&pSrcLinePart->desStartOdd;
	else
		pSrcOddDesign=&pSrcLinePart->desEndOdd;
	if(bMirStart)
		pMirOddDesign=&pMirLinePart->desStartOdd;
	else
		pMirOddDesign=&pMirLinePart->desEndOdd;
	pMirOddDesign->m_iOddCalStyle=pSrcOddDesign->m_iOddCalStyle;
	pMirOddDesign->m_fCollideDist=pSrcOddDesign->m_fCollideDist;
	MirPartHandle(&Ta,&pMirOddDesign->m_hRefPart1,pSrcOddDesign->m_hRefPart1,msg);
	MirPartHandle(&Ta,&pMirOddDesign->m_hRefPart2,pSrcOddDesign->m_hRefPart2,msg);
	if(bMirStart)
		pMirLinePart->CalStartOddment();
	else
		pMirLinePart->CalEndOddment();
	if(pSrcLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE&&pMirLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		CLDSGroupLineAngle* pSrcGroupAngle=(CLDSGroupLineAngle*)pSrcLinePart;
		CLDSGroupLineAngle* pMirGroupAngle=(CLDSGroupLineAngle*)pMirLinePart;
		for(int i=0;i<4;i++)
		{
			CLDSLineAngle* pSonAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pSrcGroupAngle->son_jg_h[i],CLS_LINEANGLE);
			if(pSonAngle==NULL)
				break;
			f3dPoint mirwingx_norm=msg.MirVector(pSonAngle->get_norm_x_wing());
			f3dPoint mirwingy_norm=msg.MirVector(pSonAngle->get_norm_y_wing());
			for(int j=0;j<4;j++)
			{
				CLDSLineAngle* pSonMirAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirGroupAngle->son_jg_h[j],CLS_LINEANGLE);
				if(pSonMirAngle==NULL)
					break;
				f3dPoint wingx_norm=pSonMirAngle->get_norm_x_wing();
				f3dPoint wingy_norm=pSonMirAngle->get_norm_y_wing();
				if(((mirwingx_norm*wingx_norm>EPS_COS2) && (mirwingy_norm*wingy_norm>EPS_COS2))||
				   ((mirwingx_norm*wingy_norm>EPS_COS2) && (mirwingy_norm*wingx_norm>EPS_COS2)))
				{
					MirLinePartOddDesignInfo(pSonAngle,pSonMirAngle,msg,bSrcStart,bMirStart);
					break;
				}
			}
		}
		pMirGroupAngle->m_bUpdateSonAngleOddment=bUpdateSonAngleOddment;
	}
	pMirLinePart->SetModified();
}
void MirLineAnglePosDesignInfo(CLDSLineAngle* pSrcAngle,CLDSLineAngle* pMirAngle,MIRMSG msg,bool bSrcStart,bool bMirStart,bool bCompatibleWing)
{
	CMultiOffsetPos *pSrcPosDesign,*pMirPosDesign;
	if((bMirStart&&pMirAngle->IsStartPosLocked())||(!bMirStart&&pMirAngle->IsEndPosLocked()))
		return;	//ʼ/�ն�λ������ʱ���ܵ���ʼ/�ն�λ��
	if(bSrcStart)
		pSrcPosDesign=&pSrcAngle->desStartPos;
	else
		pSrcPosDesign=&pSrcAngle->desEndPos;
	if(bMirStart)
		pMirPosDesign=&pMirAngle->desStartPos;
	else
		pMirPosDesign=&pMirAngle->desEndPos;
	//���ȸ���֫ƫ�Ʒ�ʽ�Լ�֫����ƫ�������ԳƽǸ֡�Ȼ���ٸ��ݲ�ͬ��������Ӧ����
	pMirPosDesign->wing_x_offset.offsetDist=pSrcPosDesign->wing_x_offset.offsetDist;
	pMirPosDesign->wing_y_offset.offsetDist=pSrcPosDesign->wing_y_offset.offsetDist;
	pMirPosDesign->wing_x_offset.gStyle=pSrcPosDesign->wing_x_offset.gStyle;
	pMirPosDesign->wing_y_offset.gStyle=pSrcPosDesign->wing_y_offset.gStyle;
	//pMirPosDesign->jgber_cal_style=pSrcPosDesign->jgber_cal_style;
	if(pMirPosDesign->jgber_cal_style==1)
	{
		f3dPoint src_pos,mir_pos;
		if(bSrcStart)
			src_pos=pSrcAngle->Start();
		else
			src_pos=pSrcAngle->End();
		mir_pos=GetMirPos(src_pos,msg);
		if(bMirStart)
			pMirAngle->SetStart(mir_pos);
		else
			pMirAngle->SetEnd(mir_pos);
	}
	else //if(pMirPosDesign->jgber_cal_style==0||pMirPosDesign->jgber_cal_style==2)
	{
		pMirPosDesign->datum_offset_dist=pSrcPosDesign->datum_offset_dist;
		MirPartHandle(&Ta,&pMirPosDesign->datum_jg_h,pSrcPosDesign->datum_jg_h,msg);
		if( pSrcPosDesign->spatialOperationStyle==0||
			pSrcPosDesign->spatialOperationStyle==1||pSrcPosDesign->spatialOperationStyle==4||
			pSrcPosDesign->spatialOperationStyle==7||pSrcPosDesign->spatialOperationStyle==10)
		{
			pMirPosDesign->spatialOperationStyle=pSrcPosDesign->spatialOperationStyle;
			//ֱ��ָ����������	�ԳƱ��´�˫������Ļ��㶨λ��ʽ wht 09-04-23
			if(pSrcPosDesign->spatialOperationStyle==0)
				MirDatumPoint(&Ta,pSrcPosDesign->datumPoint,pMirPosDesign->datumPoint,msg);
			if (pSrcPosDesign->spatialOperationStyle==7 || pSrcPosDesign->spatialOperationStyle==10)
			{
				MirPartHandle(&Ta, &pMirPosDesign->huoqu_jg_h, pSrcPosDesign->huoqu_jg_h, msg);
				pMirPosDesign->huoqu_dist = pSrcPosDesign->huoqu_dist;
				pMirPosDesign->offset_zhun = pSrcPosDesign->offset_zhun;
			}
		}
		else
		{
			//�Գ����Ӳ��ڴ�ӻ�׼���ϵĻ��㶨λ��Ϣ
			long datum_jg_h=pSrcPosDesign->datum_jg_h;
			if(datum_jg_h<0x20)
			{
				if(bSrcStart&&pSrcAngle->pStart)
					datum_jg_h=pSrcAngle->pStart->hFatherPart;
				else if(!bSrcStart&&pSrcAngle->pEnd)
					datum_jg_h=pSrcAngle->pEnd->hFatherPart;
			}
			long mir_datum_jg_h=pMirPosDesign->datum_jg_h;
			if(mir_datum_jg_h<0x20)
			{
				if(bMirStart&&pMirAngle->pStart)
					mir_datum_jg_h=pMirAngle->pStart->hFatherPart;
				else if(!bMirStart&&pMirAngle->pEnd)
					mir_datum_jg_h=pMirAngle->pEnd->hFatherPart;
			}
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(datum_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			CLDSLineAngle *pMirDatumLineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(mir_datum_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			BOOL bDatumCompatibleWing=TRUE;
			if(pDatumLineAngle&&pMirDatumLineAngle)
			{
				f3dPoint mir_vec=GetMirVector(pDatumLineAngle->get_norm_x_wing(),msg);
				if(fabs(mir_vec*pMirDatumLineAngle->get_norm_y_wing())>EPS_COS2)
					bDatumCompatibleWing=FALSE;
			}
			if(pSrcPosDesign->spatialOperationStyle==2||pSrcPosDesign->spatialOperationStyle==5||
				pSrcPosDesign->spatialOperationStyle==8||pSrcPosDesign->spatialOperationStyle==11||
				pSrcPosDesign->spatialOperationStyle==13)
			{
				if(bDatumCompatibleWing)
					pMirPosDesign->spatialOperationStyle=pSrcPosDesign->spatialOperationStyle;
				else
					pMirPosDesign->spatialOperationStyle=pSrcPosDesign->spatialOperationStyle+1;
			}
			else if(pSrcPosDesign->spatialOperationStyle==3||pSrcPosDesign->spatialOperationStyle==6||
				pSrcPosDesign->spatialOperationStyle==9||pSrcPosDesign->spatialOperationStyle==12||
				pSrcPosDesign->spatialOperationStyle==14)
			{
				if(bDatumCompatibleWing)
					pMirPosDesign->spatialOperationStyle=pSrcPosDesign->spatialOperationStyle;
				else
					pMirPosDesign->spatialOperationStyle=pSrcPosDesign->spatialOperationStyle-1;
			}
			MirPartHandle(&Ta,&pMirPosDesign->huoqu_jg_h,pSrcPosDesign->huoqu_jg_h,msg);
			pMirPosDesign->huoqu_dist=pSrcPosDesign->huoqu_dist;
			pMirPosDesign->offset_zhun=pSrcPosDesign->offset_zhun;
			//pMirPosDesign->bByOtherEndOffset=pSrcPosDesign->bByOtherEndOffset;
		}
		if(bCompatibleWing)
		{
			pMirPosDesign->wing_x_offset.gStyle=pSrcPosDesign->wing_x_offset.gStyle;
			pMirPosDesign->wing_x_offset.offsetDist=pSrcPosDesign->wing_x_offset.offsetDist;
			pMirPosDesign->wing_y_offset.gStyle=pSrcPosDesign->wing_y_offset.gStyle;
			pMirPosDesign->wing_y_offset.offsetDist=pSrcPosDesign->wing_y_offset.offsetDist;
		}
		else
		{
			pMirPosDesign->wing_x_offset.gStyle=pSrcPosDesign->wing_y_offset.gStyle;
			pMirPosDesign->wing_x_offset.offsetDist=pSrcPosDesign->wing_y_offset.offsetDist;
			pMirPosDesign->wing_y_offset.gStyle=pSrcPosDesign->wing_x_offset.gStyle;
			pMirPosDesign->wing_y_offset.offsetDist=pSrcPosDesign->wing_x_offset.offsetDist;
		}
		//���㵽���Ķ�λ��ʽ�ԳƽǸ�һ������ֵͬ
		//pMirPosDesign->datum_to_ber_style=pSrcPosDesign->datum_to_ber_style;
		pMirPosDesign->face_offset_norm=GetMirVector(pSrcPosDesign->face_offset_norm,msg);
	}
}
static void MirAngleBoltRefList(CLDSLineAngle *pSrcAngle, CLDSLineAngle *pMirAngle,MIRMSG msg,bool bCanNewBolt=false)
{
	if(pSrcAngle->GetClassTypeId()==CLS_GROUPLINEANGLE&&pMirAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)pSrcAngle;
		CLDSGroupLineAngle* pMirGroupAngle=(CLDSGroupLineAngle*)pMirAngle;
		for(int i=0;i<4;i++)
		{
			CLDSLineAngle *pSonAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pGroupAngle->son_jg_h[i]);
			CLDSLineAngle *pMirSonAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirGroupAngle->son_jg_h[i]);
			if(pSonAngle&&pMirSonAngle)
				MirAngleBoltRefList(pSonAngle,pMirSonAngle,msg,bCanNewBolt);
			if(pGroupAngle->group_style<2&&i==1)
				break;	//��ʮ����ϽǸ�
		}
		return;
	}
	for(CLsRef *pLsRef=pSrcAngle->GetFirstLsRef();pLsRef;pLsRef=pSrcAngle->GetNextLsRef())
	{
		CLDSBolt *pMirBolt=(CLDSBolt*)pLsRef->GetLsPtr()->GetMirPart(msg);
		if(pMirBolt)
		{
			pMirAngle->AppendMidLsRef(pMirBolt->GetLsRef());
			pMirBolt->ReplaceReferenceHandle(CHandleReplacer(pSrcAngle->handle,pMirAngle->handle));
			pMirBolt->SetModified();
			pMirBolt->Create3dSolidModel();
			if(!pMirBolt->m_bVirtualPart||(pMirBolt->m_bVirtualPart&&g_sysPara.display.bDispVirtualPart))
				g_pSolidDraw->NewSolidPart(pMirBolt->GetSolidPartObject());
		}
		else if(bCanNewBolt)
		{
			pMirBolt=CreateSingleMirBolt(pLsRef->GetLsPtr(),msg);
			pMirAngle->AppendMidLsRef(pMirBolt->GetLsRef());
		}
	}
}
CLDSLineAngle *CreateSingleMirAngle(CLDSLineAngle *pSrcAngle, MIRMSG msg,BOOL bIncAttachInfo=TRUE,char *sErrorInfo=NULL)
{
	CLDSLineAngle *pMirAngle=NULL;
	pMirAngle=(CLDSLineAngle*)pSrcAngle->GetMirPart(msg);
	if(pMirAngle==NULL)	//�ԳƽǸ�δ�ҵ�
	{
		pMirAngle=(CLDSLineAngle*)console.AppendPart(pSrcAngle->GetClassTypeId());
		pMirAngle->set_wing_wide(pSrcAngle->size_wide);
		pMirAngle->set_wing_thick(pSrcAngle->size_thick);
		if(pSrcAngle->IsUnequalAngle())	//���ȱ߽Ǹ�
			pMirAngle->set_y_wing_width(pSrcAngle->size_height);
		if(pSrcAngle->pStart)
			pMirAngle->pStart = pSrcAngle->pStart->GetMirNode(msg);
		if(pSrcAngle->pEnd)
			pMirAngle->pEnd = pSrcAngle->pEnd->GetMirNode(msg);
		if(pMirAngle->pStart==NULL&&pSrcAngle->pStart)
			pMirAngle->pStart = pSrcAngle->pStart;
		if(pMirAngle->pEnd==NULL&&pSrcAngle->pEnd)
			pMirAngle->pEnd = pSrcAngle->pEnd;
		if(pMirAngle->pStart&&pMirAngle->pStart->hFatherPart<0X20)
			pMirAngle->pStart->hFatherPart=pMirAngle->handle;
		if(pMirAngle->pEnd&&pMirAngle->pEnd->hFatherPart<0X20)
			pMirAngle->pEnd->hFatherPart=pMirAngle->handle;
		pMirAngle->CopyProperty(pSrcAngle);//�������ڵ�(����)����
		pMirAngle->bHuoQuStart=pSrcAngle->bHuoQuStart;
		pMirAngle->bHuoQuEnd=pSrcAngle->bHuoQuEnd;
		if(pSrcAngle->m_bEnableTeG)
		{	//����׼�� wht 10-06-06
			pMirAngle->m_bEnableTeG=TRUE;
			pMirAngle->xWingXZhunJu=pSrcAngle->xWingXZhunJu;
			pMirAngle->xWingYZhunJu=pSrcAngle->xWingYZhunJu;
		}
		pMirAngle->SetStart(GetMirPos(pSrcAngle->Start(),msg));
		pMirAngle->SetEnd(GetMirPos(pSrcAngle->End(),msg));
		if(pSrcAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			((CLDSGroupLineAngle*)pMirAngle)->size_cSubClassType=((CLDSGroupLineAngle*)pSrcAngle)->size_cSubClassType;
			((CLDSGroupLineAngle*)pMirAngle)->jg_space=((CLDSGroupLineAngle*)pSrcAngle)->jg_space;
		}
		//���ڽǸֶԳƺ�����������֫���ߵĶԳ���������˴�ʱ�ԳƽǸ�ʵ��ʱӦ����֫���ߵĶԳ� wjh-2015.2.1
		f3dPoint wing_x_norm=pSrcAngle->get_norm_x_wing();
		f3dPoint wing_y_norm=pSrcAngle->get_norm_y_wing();
		if(msg.axis_flag==0x01||msg.axis_flag==0x02||msg.axis_flag==0x10)
		{	//X��Y��Գƻ���Գ�ʱĬ����Ҫ��תX��Y֫
			//����ʵ���п�����Ĭ�ϱ���ԭ��������������Գ�Ŀ��λ���޽Ǹ�ʱ��X/Y����Գ�ʱҲӦ����X��Y֫ wjh-2018.1.28
			if(pMirAngle->MirLayStyle==CLDSLineAngle::MIRXorY_SAMEDIRECT||pMirAngle->MirLayStyle==CLDSLineAngle::MIRXorY_UNCHAHNGE)
			{	//����ͬ����Ҫ��תX��Y֫
				wing_x_norm=GetMirVector(pSrcAngle->get_norm_y_wing(),msg);
				wing_y_norm=GetMirVector(pSrcAngle->get_norm_x_wing(),msg);
			}
			else //if(pMirAngle->MirLayStyle==CLDSLineAngle::MIRXorY_INVERT||MirLayStyle==CLDSLineAngle::MIRXorY_UNCHAHNGE)
			{	//���ⷴ�����ԭ��������������Ҫ��תX��Y֫
				wing_x_norm=GetMirVector(pSrcAngle->get_norm_x_wing(),msg);
				wing_y_norm=GetMirVector(pSrcAngle->get_norm_y_wing(),msg);
			}
			if(pSrcAngle->GetClassTypeId()==CLS_GROUPLINEANGLE&&((CLDSGroupLineAngle*)pSrcAngle)->size_cSubClassType=='T')
				((CLDSGroupLineAngle*)pMirAngle)->ciMasterIndex=1-((CLDSGroupLineAngle*)pSrcAngle)->ciMasterIndex;
		}
		else
		{
			wing_x_norm=GetMirVector(pSrcAngle->get_norm_x_wing(),msg);
			wing_y_norm=GetMirVector(pSrcAngle->get_norm_y_wing(),msg);
		}
		pMirAngle->set_norm_x_wing(wing_x_norm);
		pMirAngle->set_norm_y_wing(wing_y_norm);
		if(pSrcAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			((CLDSGroupLineAngle*)pMirAngle)->AppendSonJg();
			((CLDSGroupLineAngle*)pMirAngle)->UpdateSonJgBasicInfo();
		}
		AddMirObjToSrcObjRelaObjList(pSrcAngle,pMirAngle,msg);	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
	}
	CalMirAtomLayer(pSrcAngle->layer(),pMirAngle->layer(),msg);
	if(!bIncAttachInfo)
		return pMirAngle;
	pSrcAngle->SetModified();
	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
	AddMirObjToSrcObjRelaObjList(pSrcAngle,pMirAngle,msg);
	if(pMirAngle->handle<pSrcAngle->feature)
		return pMirAngle;	//ԭ�й������Գ����¸�����Ϣ,�������ƻ�ԭ�����ݵ��½Ǹַ��߻�λ�ô��� wjh 2011.8.12
	//------vvvvvvvvv-------��˨���--------------------
	pMirAngle->connectStart = pSrcAngle->connectStart;
	pMirAngle->connectEnd = pSrcAngle->connectEnd;
	//pMirAngle->connectStart.d = pSrcAngle->connectStart.d;
	//pMirAngle->connectStart.rows = pSrcAngle->connectStart.rows;//ȱʡ������˨
	//pMirAngle->connectStart.N = pSrcAngle->connectStart.N;
	//pMirAngle->connectEnd.d = pSrcAngle->connectEnd.d;
	//pMirAngle->connectEnd.rows = pSrcAngle->connectEnd.rows;//ȱʡ������˨
	//pMirAngle->connectEnd.N = pSrcAngle->connectEnd.N;
	//------^^^^^^^^^-------��˨���--------------------
	CSuperSmartPtr<CLDSPart> pRefPart,pMirRefPart;
	f3dPoint mirPickPos;	//�Գ�֫���߼�����ո˼�ʱ��ʰȡ�㣬��ֵ�Խ��Ӱ�첻�󣬵�����������������ֶԳƸ˼�wjh-2014.5.4
	if(pSrcAngle->pStart)
		mirPickPos=pSrcAngle->pStart->Position();
	//X֫����
	pRefPart=Ta.Parts.FromHandle(pSrcAngle->des_norm_x.hViceJg);
	if(pRefPart.IsHasPtr()&&pRefPart->IsLinePart())
	{
		pMirRefPart=pRefPart.LinePartPointer()->GetMirPart(msg,mirPickPos);
		if(pMirRefPart.IsHasPtr())
			pMirAngle->des_norm_x.hViceJg=pMirRefPart->handle;
	}
	if(!pSrcAngle->IsSyncMirPropWith(pMirAngle,"wingNormInfo") || !pSrcAngle->SyncMirPropTo(pMirAngle,msg,"wingNormInfo"))
	{	//����ͨ�����¶Գƶ����ܺ�������֫���߷���Գ� wjh-2015.6.26
		pMirAngle->des_norm_x.bByOtherWing=pSrcAngle->des_norm_x.bByOtherWing;
		pMirAngle->des_norm_x.bSpecific=pSrcAngle->des_norm_x.bSpecific;
		pMirAngle->des_norm_x.near_norm=GetMirVector(pSrcAngle->des_norm_x.near_norm,msg);
		pMirAngle->des_norm_x.spec_norm.norm_style=pSrcAngle->des_norm_x.spec_norm.norm_style;
		pMirAngle->des_norm_x.spec_norm.norm_wing=pSrcAngle->des_norm_x.spec_norm.norm_wing;
		pMirAngle->des_norm_x.spec_norm.direction=pSrcAngle->des_norm_x.spec_norm.direction;
		pMirAngle->des_norm_x.spec_norm.nearVector=GetMirVector(pSrcAngle->des_norm_x.spec_norm.nearVector,msg);
		if(pSrcAngle->des_norm_x.spec_norm.vector.IsZero())
			pSrcAngle->des_norm_x.spec_norm.UpdateVector(pSrcAngle->BelongModel());
		pMirAngle->des_norm_x.spec_norm.vector=GetMirVector(pSrcAngle->des_norm_x.spec_norm.vector,msg);
		pRefPart=Ta.Parts.FromHandle(pSrcAngle->des_norm_x.spec_norm.hVicePart);
		if(pRefPart.IsHasPtr())
		{
			if(pMirAngle->des_norm_x.spec_norm.norm_style==3	//������������
				||pMirAngle->des_norm_x.spec_norm.norm_style==4)//��׼�����ϵ���Է���
				pMirRefPart=pRefPart->GetMirPart(msg);
			else if(pRefPart->IsLinePart())
				pMirRefPart=pRefPart.LinePartPointer()->GetMirPart(msg,mirPickPos);
			if(pMirRefPart.IsHasPtr())
			{
				pMirAngle->des_norm_x.spec_norm.hVicePart=pMirRefPart->handle;
				if(pMirRefPart->GetClassTypeId()==CLS_LINEANGLE
					&&pMirAngle->des_norm_x.spec_norm.norm_style==1)	
				{	//ָ���Ǹ�֫���� �����Ǹ�֫���߼������ wht 10-09-30
					double ddx=pMirRefPart.LineAnglePointer()->get_norm_x_wing()*pMirAngle->get_norm_x_wing();
					double ddy=pMirRefPart.LineAnglePointer()->get_norm_y_wing()*pMirAngle->get_norm_x_wing();
					if(fabs(ddx)>fabs(ddy))
					{
						if(ddx>0)
							pMirAngle->des_norm_x.spec_norm.direction=0;
						else
							pMirAngle->des_norm_x.spec_norm.direction=1;
						pMirAngle->des_norm_x.spec_norm.norm_wing=0;
					}
					else
					{
						if(ddy>0)
							pMirAngle->des_norm_x.spec_norm.direction=0;
						else
							pMirAngle->des_norm_x.spec_norm.direction=1;
						pMirAngle->des_norm_x.spec_norm.norm_wing=1;
					}
				}
			}
		}
		pRefPart=Ta.Parts.FromHandle(pSrcAngle->des_norm_x.spec_norm.hCrossPart);
		if(pRefPart.IsHasPtr()&&pRefPart->IsLinePart())
		{
			pMirRefPart=pRefPart.LinePartPointer()->GetMirPart(msg,mirPickPos);
			if(pMirRefPart.IsHasPtr())
				pMirAngle->des_norm_x.spec_norm.hCrossPart=pMirRefPart->handle;
		}
		//Y֫����
		pRefPart=Ta.Parts.FromHandle(pSrcAngle->des_norm_y.hViceJg);
		if(pRefPart.IsHasPtr()&&pRefPart->IsLinePart())
		{
			pMirRefPart=pRefPart.LinePartPointer()->GetMirPart(msg,mirPickPos);
			if(pMirRefPart.IsHasPtr())
				pMirAngle->des_norm_y.hViceJg=pMirRefPart->handle;
		}
		pMirAngle->des_norm_y.bByOtherWing=pSrcAngle->des_norm_y.bByOtherWing;
		pMirAngle->des_norm_y.bSpecific=pSrcAngle->des_norm_y.bSpecific;
		pMirAngle->des_norm_y.near_norm=GetMirVector(pSrcAngle->des_norm_y.near_norm,msg);
		pMirAngle->des_norm_y.spec_norm.norm_style=pSrcAngle->des_norm_y.spec_norm.norm_style;
		pMirAngle->des_norm_y.spec_norm.norm_wing=pSrcAngle->des_norm_y.spec_norm.norm_wing;
		pMirAngle->des_norm_y.spec_norm.direction=pSrcAngle->des_norm_y.spec_norm.direction;
		pMirAngle->des_norm_y.spec_norm.nearVector=GetMirVector(pSrcAngle->des_norm_y.spec_norm.nearVector,msg);
		if(pSrcAngle->des_norm_y.spec_norm.vector.IsZero())
			pSrcAngle->des_norm_y.spec_norm.UpdateVector(pSrcAngle->BelongModel());
		pMirAngle->des_norm_y.spec_norm.vector=GetMirVector(pSrcAngle->des_norm_y.spec_norm.vector,msg);
		pRefPart=Ta.Parts.FromHandle(pSrcAngle->des_norm_y.spec_norm.hVicePart);
		if(pRefPart.IsHasPtr())
		{
			if(pMirAngle->des_norm_y.spec_norm.norm_style==3	//������������
				||pMirAngle->des_norm_y.spec_norm.norm_style==4)//��׼�����ϵ���Է���
				pMirRefPart=pRefPart->GetMirPart(msg);
			else if(pRefPart->IsLinePart())
				pMirRefPart=pRefPart.LinePartPointer()->GetMirPart(msg,mirPickPos);
			if(pMirRefPart.IsHasPtr())
			{
				pMirAngle->des_norm_y.spec_norm.hVicePart=pMirRefPart->handle;
				if(pMirRefPart->GetClassTypeId()==CLS_LINEANGLE
					&&pMirAngle->des_norm_y.spec_norm.norm_style==1)
				{	//ָ���Ǹ�֫���� �����Ǹ�֫���߼������ wht 10-09-30	
					double ddx=pMirRefPart.LineAnglePointer()->get_norm_x_wing()*pMirAngle->get_norm_y_wing();
					double ddy=pMirRefPart.LineAnglePointer()->get_norm_y_wing()*pMirAngle->get_norm_y_wing();
					if(fabs(ddx)>fabs(ddy))
					{
						if(ddx>0)
							pMirAngle->des_norm_y.spec_norm.direction=0;
						else
							pMirAngle->des_norm_y.spec_norm.direction=1;
						pMirAngle->des_norm_y.spec_norm.norm_wing=0;
					}
					else
					{
						if(ddy>0)
							pMirAngle->des_norm_y.spec_norm.direction=0;
						else
							pMirAngle->des_norm_y.spec_norm.direction=1;
						pMirAngle->des_norm_y.spec_norm.norm_wing=1;
					}
				}
			}
		}
		pRefPart=Ta.Parts.FromHandle(pSrcAngle->des_norm_y.spec_norm.hCrossPart);
		if(pRefPart.IsHasPtr()&&pRefPart->IsLinePart())
		{
			pMirRefPart=pRefPart.LinePartPointer()->GetMirPart(msg,mirPickPos);
			if(pMirRefPart.IsHasPtr())
				pMirAngle->des_norm_y.spec_norm.hCrossPart=pMirRefPart->handle;
		}
		//��������б�ļ�����б�ĵķ�����Ʋ�������ҵϰ�߽�������
		//if(msg.axis_flag&0x01||msg.axis_flag&0x02)
		if(msg.axis_flag==0x01||msg.axis_flag==0x02||msg.axis_flag==0x10)
		{
			//if((pMirAngle->layer(1)=='X'&&pMirAngle->Layer(0)!='L')||(pMirAngle->layer(0)=='S'&&pMirAngle->layer(1)=='F'))
			if(pSrcAngle->MirLayStyle==CLDSLineAngle::MIRXorY_INVERT)
			{	//�������Գ�
				if(!pMirAngle->des_norm_x.bByOtherWing)	//���ո˼������֫Ϊȷ���������Ļ�׼֫
					pMirAngle->des_norm_x.near_norm*=-1;
				if(!pMirAngle->des_norm_y.bByOtherWing)
					pMirAngle->des_norm_y.near_norm*=-1;
			}
			else
			{	//ͬ�������Գ�
				DESIGN_JGWING_NORM des_wing_norm=pMirAngle->des_norm_x;
				pMirAngle->des_norm_x=pMirAngle->des_norm_y;
				pMirAngle->des_norm_y=des_wing_norm;
			}
		}
	}

	pMirAngle->ClearFlag();
	pMirAngle->cal_x_wing_norm();
	pMirAngle->cal_y_wing_norm();
	//������֫������ٸ�������ͷ�����������֫��������нǲ�����
	MirLinePartOddDesignInfo(pSrcAngle,pMirAngle,msg,true,true);
	MirLinePartOddDesignInfo(pSrcAngle,pMirAngle,msg,false,false);
	if(pMirAngle!=pSrcAngle)
	{
		//ʼ�����λ�����
		pMirAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2=pSrcAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2;
			//�����㷽ʽ
		pMirAngle->desStartPos.jgber_cal_style=pSrcAngle->desStartPos.jgber_cal_style;
		pMirAngle->desStartPos.face_offset_norm=GetMirVector(pSrcAngle->desStartPos.face_offset_norm,msg);
		//pMirAngle->desStartPos.bByOtherEndOffset=pSrcAngle->desStartPos.bByOtherEndOffset;
		if(pSrcAngle->desStartPos.spatialOperationStyle==16||pSrcAngle->desStartPos.spatialOperationStyle==17)
		{
			MirDatumPoint(&Ta,pSrcAngle->desStartPos.datumLineStart,pMirAngle->desStartPos.datumLineStart,msg);
			MirDatumPoint(&Ta,pSrcAngle->desStartPos.datumLineEnd,pMirAngle->desStartPos.datumLineEnd,msg);
			/*MirPartHandle(pSrcAngle->BelongModel(),&pMirAngle->desStartPos.start_jg_h,pSrcAngle->desStartPos.start_jg_h,msg);
			MirPartHandle(pSrcAngle->BelongModel(),&pMirAngle->desStartPos.end_jg_h,pSrcAngle->desStartPos.end_jg_h,msg);
			pMirAngle->desStartPos.iStartJgBerStyle=pSrcAngle->desStartPos.iStartJgBerStyle;
			pMirAngle->desStartPos.iEndJgBerStyle=pSrcAngle->desStartPos.iEndJgBerStyle;
			pMirAngle->desStartPos.fStartJgLengOffsetX=pSrcAngle->desStartPos.fStartJgLengOffsetX;
			pMirAngle->desStartPos.fStartJgLengOffsetY=pSrcAngle->desStartPos.fStartJgLengOffsetY;
			pMirAngle->desStartPos.fEndJgLengOffsetX=pSrcAngle->desStartPos.fEndJgLengOffsetX;
			pMirAngle->desStartPos.fEndJgLengOffsetY=pSrcAngle->desStartPos.fEndJgLengOffsetY;*/
		}
			//������㷽ʽ
		CSuperSmartPtr<CLDSLinePart> pDatumPart;
		MirPartHandle(pSrcAngle->BelongModel(),&pMirAngle->desStartPos.datum_jg_h,pSrcAngle->desStartPos.datum_jg_h,msg);
		if(pMirAngle->desStartPos.datum_jg_h>0x20)
			pDatumPart=(CLDSLinePart*)Ta.Parts.FromHandle(pMirAngle->desStartPos.datum_jg_h,CLS_LINEPART);
		else if(pMirAngle->pStart)
			pDatumPart=(CLDSLinePart*)Ta.Parts.FromHandle(pMirAngle->pStart->hFatherPart,CLS_LINEPART);
		int wing_x0_y1,datum_wing_x0_y1;
		pMirAngle->desStartPos.spatialOperationStyle=pSrcAngle->desStartPos.spatialOperationStyle;
		datum_wing_x0_y1=2;
		if(pMirAngle->IsLappedAnchorStart())
		{
			char cDatumDependWing=pMirAngle->GetLappedWorkWing(TRUE,NULL);
			if(cDatumDependWing=='X')
				datum_wing_x0_y1=0;
			else if(cDatumDependWing=='Y')
				datum_wing_x0_y1=1;
		}
		if(datum_wing_x0_y1==0)
		{	//���ڻ�׼�Ǹֵ�X֫��
			if(pMirAngle->desStartPos.spatialOperationStyle==3||
				pMirAngle->desStartPos.spatialOperationStyle==6||
				pMirAngle->desStartPos.spatialOperationStyle==9||
				pMirAngle->desStartPos.spatialOperationStyle==12||
				pMirAngle->desStartPos.spatialOperationStyle==14)
				pMirAngle->desStartPos.spatialOperationStyle-=1;
		}
		else if(datum_wing_x0_y1==1)
		{	//���ڻ�׼�Ǹֵ�Y֫��
			if(pMirAngle->desStartPos.spatialOperationStyle==2||
				pMirAngle->desStartPos.spatialOperationStyle==5||
				pMirAngle->desStartPos.spatialOperationStyle==8||
				pMirAngle->desStartPos.spatialOperationStyle==11||
				pMirAngle->desStartPos.spatialOperationStyle==13)
				pMirAngle->desStartPos.spatialOperationStyle+=1;
		}

		MirPartHandle(pSrcAngle->BelongModel(),&pMirAngle->desStartPos.huoqu_jg_h,pSrcAngle->desStartPos.huoqu_jg_h,msg);
		pMirAngle->desStartPos.huoqu_dist=pSrcAngle->desStartPos.huoqu_dist;
		pMirAngle->desStartPos.offset_zhun=pSrcAngle->desStartPos.offset_zhun;
		pMirAngle->desStartPos.datum_offset_dist=pSrcAngle->desStartPos.datum_offset_dist;
		MirDatumPoint(pSrcAngle->BelongModel(),pSrcAngle->desStartPos.datumPoint,pMirAngle->desStartPos.datumPoint,msg);
		wing_x0_y1=pMirAngle->GetStartWorkWing();	//��������MirDatumPoint֮��,��ΪdesStartPos.datumPointӰ��GetStartWorkWing wjh-2016.4.25
			//�Ի���������ƫ�Ʒ�ʽ
		//pMirAngle->desStartPos.datum_to_ber_style=pSrcAngle->desStartPos.datum_to_ber_style;
		if( (msg.axis_flag==0x01||msg.axis_flag==0x02||msg.axis_flag==0x10)&&
			(pSrcAngle->MirLayStyle!=CLDSLineAngle::MIRXorY_INVERT))
		{	//X/Y�Գ�������ͬ��,�������֫ƫ�Ʋ���
			pMirAngle->desStartPos.wing_x_offset=pSrcAngle->desStartPos.wing_y_offset;
			pMirAngle->desStartPos.wing_y_offset=pSrcAngle->desStartPos.wing_x_offset;
		}
		else
		{
			pMirAngle->desStartPos.wing_x_offset.gStyle=pSrcAngle->desStartPos.wing_x_offset.gStyle;
			pMirAngle->desStartPos.wing_x_offset.offsetDist=pSrcAngle->desStartPos.wing_x_offset.offsetDist;
			pMirAngle->desStartPos.wing_y_offset.gStyle=pSrcAngle->desStartPos.wing_y_offset.gStyle;
			pMirAngle->desStartPos.wing_y_offset.offsetDist=pSrcAngle->desStartPos.wing_y_offset.offsetDist;
		}
		if(pDatumPart.IsHasPtr()&&!pDatumPart.IsEqualPtr(pMirAngle)&&//����֫�ڷ���ʽ��������
			!(pMirAngle->desStartPos.wing_x_offset.gStyle==4&&pMirAngle->desStartPos.wing_x_offset.offsetDist==0&&
			 pMirAngle->desStartPos.wing_y_offset.gStyle==4&&pMirAngle->desStartPos.wing_y_offset.offsetDist==0))
		{	//ԭ�������Զ���ƫ����ʱ,��ƫ����Ϊ��ʱ,������ wjh-2011.8.28
			if(wing_x0_y1==0)
			{	//����X֫Ϊ���֫
				pMirAngle->desStartPos.wing_x_offset.gStyle=4;
				pMirAngle->desStartPos.wing_x_offset.offsetDist=0;
				pMirAngle->desStartPos.wing_y_offset.gStyle=0;
			}
			else if(wing_x0_y1==1)
			{	//����Y֫Ϊ���֫
				pMirAngle->desStartPos.wing_x_offset.gStyle=0;
				pMirAngle->desStartPos.wing_y_offset.gStyle=4;
				pMirAngle->desStartPos.wing_y_offset.offsetDist=0;
			}
		}
		//�ն����λ�����
		pMirAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2=pSrcAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2;
			//�����㷽ʽ
		pMirAngle->desEndPos.jgber_cal_style=pSrcAngle->desEndPos.jgber_cal_style;
		pMirAngle->desEndPos.face_offset_norm=GetMirVector(pSrcAngle->desEndPos.face_offset_norm,msg);
		//pMirAngle->desEndPos.bByOtherEndOffset=pSrcAngle->desEndPos.bByOtherEndOffset;
		if(pSrcAngle->desEndPos.spatialOperationStyle==16||pSrcAngle->desEndPos.spatialOperationStyle==17)
		{
			MirDatumPoint(&Ta,pSrcAngle->desEndPos.datumLineStart,pMirAngle->desEndPos.datumLineStart,msg);
			MirDatumPoint(&Ta,pSrcAngle->desEndPos.datumLineEnd,pMirAngle->desEndPos.datumLineEnd,msg);
			/*MirPartHandle(pSrcAngle->BelongModel(),&pMirAngle->desEndPos.start_jg_h,pSrcAngle->desEndPos.start_jg_h,msg);
			MirPartHandle(pSrcAngle->BelongModel(),&pMirAngle->desEndPos.end_jg_h,pSrcAngle->desEndPos.end_jg_h,msg);
			pMirAngle->desEndPos.iStartJgBerStyle=pSrcAngle->desEndPos.iStartJgBerStyle;
			pMirAngle->desEndPos.iEndJgBerStyle=pSrcAngle->desEndPos.iEndJgBerStyle;
			pMirAngle->desEndPos.fStartJgLengOffsetX=pSrcAngle->desEndPos.fStartJgLengOffsetX;
			pMirAngle->desEndPos.fStartJgLengOffsetY=pSrcAngle->desEndPos.fStartJgLengOffsetY;
			pMirAngle->desEndPos.fEndJgLengOffsetX=pSrcAngle->desEndPos.fEndJgLengOffsetX;
			pMirAngle->desEndPos.fEndJgLengOffsetY=pSrcAngle->desEndPos.fEndJgLengOffsetY;*/
		}
			//������㷽ʽ
		MirPartHandle(pSrcAngle->BelongModel(),&pMirAngle->desEndPos.datum_jg_h,pSrcAngle->desEndPos.datum_jg_h,msg);
		if(pMirAngle->desEndPos.datum_jg_h>0x20)
			pDatumPart=(CLDSLinePart*)Ta.Parts.FromHandle(pMirAngle->desEndPos.datum_jg_h);
		else if(pMirAngle->pEnd)
			pDatumPart=(CLDSLinePart*)Ta.Parts.FromHandle(pMirAngle->pEnd->hFatherPart);
		else
			pDatumPart=NULL;
		pMirAngle->desEndPos.spatialOperationStyle=pSrcAngle->desEndPos.spatialOperationStyle;
		datum_wing_x0_y1=2;
		if(pMirAngle->IsLappedAnchorEnd())
		{
			char cDatumDependWing=pMirAngle->GetLappedWorkWing(FALSE,NULL);
			if(cDatumDependWing=='X')
				datum_wing_x0_y1=0;
			else if(cDatumDependWing=='Y')
				datum_wing_x0_y1=1;
		}
		if(datum_wing_x0_y1==0)
		{	//���ڻ�׼�Ǹֵ�X֫��
			if(pMirAngle->desEndPos.spatialOperationStyle==3||
				pMirAngle->desEndPos.spatialOperationStyle==6||
				pMirAngle->desEndPos.spatialOperationStyle==9||
				pMirAngle->desEndPos.spatialOperationStyle==12||
				pMirAngle->desEndPos.spatialOperationStyle==14)
				pMirAngle->desEndPos.spatialOperationStyle-=1;
		}
		else if(datum_wing_x0_y1==1)
		{	//���ڻ�׼�Ǹֵ�Y֫��
			if(pMirAngle->desEndPos.spatialOperationStyle==2||
				pMirAngle->desEndPos.spatialOperationStyle==5||
				pMirAngle->desEndPos.spatialOperationStyle==8||
				pMirAngle->desEndPos.spatialOperationStyle==11||
				pMirAngle->desEndPos.spatialOperationStyle==13)
				pMirAngle->desEndPos.spatialOperationStyle+=1;
		}

		MirPartHandle(pSrcAngle->BelongModel(),&pMirAngle->desEndPos.huoqu_jg_h,pSrcAngle->desEndPos.huoqu_jg_h,msg);
		pMirAngle->desEndPos.huoqu_dist=pSrcAngle->desEndPos.huoqu_dist;
		pMirAngle->desEndPos.offset_zhun=pSrcAngle->desEndPos.offset_zhun;
		pMirAngle->desEndPos.datum_offset_dist=pSrcAngle->desEndPos.datum_offset_dist;
		MirDatumPoint(pSrcAngle->BelongModel(),pSrcAngle->desEndPos.datumPoint,pMirAngle->desEndPos.datumPoint,msg);
		wing_x0_y1=pMirAngle->GetEndWorkWing();	//��������MirDatumPoint֮��,��ΪdesEndPos.datumPointӰ��GetEndWorkWing wjh-2016.4.25
			//�Ի���������ƫ�Ʒ�ʽ
		//pMirAngle->desEndPos.datum_to_ber_style=pSrcAngle->desEndPos.datum_to_ber_style;
		if( (msg.axis_flag==0x01||msg.axis_flag==0x02||msg.axis_flag==0x10)&&
			(pSrcAngle->MirLayStyle!=CLDSLineAngle::MIRXorY_INVERT))
		{	//X/Y�Գ�������ͬ��,�������֫ƫ�Ʋ���
			pMirAngle->desEndPos.wing_x_offset=pSrcAngle->desEndPos.wing_y_offset;
			pMirAngle->desEndPos.wing_y_offset=pSrcAngle->desEndPos.wing_x_offset;
		}
		else
		{
			pMirAngle->desEndPos.wing_x_offset.gStyle=pSrcAngle->desEndPos.wing_x_offset.gStyle;
			pMirAngle->desEndPos.wing_x_offset.offsetDist=pSrcAngle->desEndPos.wing_x_offset.offsetDist;
			pMirAngle->desEndPos.wing_y_offset.gStyle=pSrcAngle->desEndPos.wing_y_offset.gStyle;
			pMirAngle->desEndPos.wing_y_offset.offsetDist=pSrcAngle->desEndPos.wing_y_offset.offsetDist;
		}
		if(pDatumPart.IsHasPtr()&&!pDatumPart.IsEqualPtr(pMirAngle)&&//����֫�ڷ���ʽ��������
			!(pMirAngle->desEndPos.wing_x_offset.gStyle==4&&pMirAngle->desEndPos.wing_x_offset.offsetDist==0&&
			 pMirAngle->desEndPos.wing_y_offset.gStyle==4&&pMirAngle->desEndPos.wing_y_offset.offsetDist==0))
		{	//ԭ�������Զ���ƫ����ʱ,��ƫ����Ϊ��ʱ,������ wjh-2011.8.28
			if(wing_x0_y1==0)
			{	//����X֫Ϊ���֫
				pMirAngle->desEndPos.wing_x_offset.gStyle=4;
				pMirAngle->desEndPos.wing_x_offset.offsetDist=0;
				pMirAngle->desEndPos.wing_y_offset.gStyle=0;
			}
			else if(wing_x0_y1==1)
			{	//����Y֫Ϊ���֫
				pMirAngle->desEndPos.wing_x_offset.gStyle=0;
				pMirAngle->desEndPos.wing_y_offset.gStyle=4;
				pMirAngle->desEndPos.wing_y_offset.offsetDist=0;
			}
		}
	}
	pSrcAngle->SyncMirPropTo(pMirAngle,msg,"Position");	//֮�ϴ����ڶԳ���֫�������ʱ����Ϣ�ԳƲ����� wjh-2016.4.27
	NewLinePart(pMirAngle);
	if(pSrcAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)pSrcAngle;
		CLDSGroupLineAngle* pMirGroupAngle=(CLDSGroupLineAngle*)pMirAngle;
		pMirGroupAngle->UpdateSonJgOddment();
		pMirGroupAngle->UpdateSonJgWingNorm();
		pMirGroupAngle->UpdateSonJgPos();
		for(int i=0;i<4;i++)
		{
			CLDSLineAngle *pSonAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pSrcAngle->son_jg_h[i],CLS_LINEANGLE);
			if(pSonAngle)
			{
				CLDSLineAngle * pMirSonAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirGroupAngle->son_jg_h[i],CLS_LINEANGLE);
				pMirSonAngle->SetPartNo(pSonAngle->GetPartNo());	//��������޷�����ϽǸ��жԳƹ���,��˱������⴦��
			}
			if(pGroupAngle->group_style<2&&i==1)
				break;	//��ʮ����ϽǸ�
		}
	}
	
	bool bCanNewBolt=false;
	if(pMirAngle->pStart==NULL&&pMirAngle->pEnd==NULL)
		bCanNewBolt=true;	//Ŀǰ��֧�ֶ̽Ǹ�����Ҫ�Գ�������˨����ͨ�Ǹ�ֻ�Գ�������˨����
	MirAngleBoltRefList(pSrcAngle,pMirAngle,msg,bCanNewBolt);
	g_pSolidDraw->Draw();
	return pMirAngle;
}

static BOOL MirAngle(CLDSLineAngle* pSrcLineAngle, MIRMSG mirmsg,BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//�Ƿ�ɹ��Գ��������й���
	RELATIVE_OBJECT *pRelaObj=NULL;
	if(!IsPartMirValid(pSrcLineAngle,mirmsg))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "fail to pass symmetry legality checking, parts can not be symmetrical";
#else
		throw "δͨ���ԳƺϷ��Լ��,���ܶԳƹ���";
#endif
	int flagArr[4]={1,2,4,16};
	CLDSLineAngle* srcangle_arr[4]={NULL};
	srcangle_arr[0]=pSrcLineAngle;
	for(int i=0;i<4;i++)
	{
		if((mirmsg.axis_flag&flagArr[i])==0)
			continue;
		MIRMSG msg=mirmsg.GetSubMirItem(i+1);
		srcangle_arr[i+1]=CreateSingleMirAngle(pSrcLineAngle,msg,bIncAttachInfo);
		if(srcangle_arr[i+1]==NULL)
			return FALSE;
#if defined(__TSA_)||defined(__LDS_)
		else
		{
			if(bIncAttachInfo)
			{
				if(pSrcLineAngle->CalLenCoef.hHorizPole>0x20)
					MirPartHandle(&Ta,&srcangle_arr[i+1]->CalLenCoef.hHorizPole,pSrcLineAngle->CalLenCoef.hHorizPole,mirmsg);
				if(pSrcLineAngle->CalLenCoef.hRefPole>0x20)
					MirPartHandle(&Ta,&srcangle_arr[i+1]->CalLenCoef.hRefPole,pSrcLineAngle->CalLenCoef.hRefPole,mirmsg);
			}
		}
#endif
	}
	int array_num=mirmsg.array_num;
	if((mirmsg.axis_flag&8)==0)	//����ת�Գ�
		array_num=0;
	else
		mirmsg.axis_flag=8;
	short init_angle=mirmsg.rotate_angle;
	for(i=0;i<4;i++)
	{
		if(srcangle_arr[i]==NULL)
			continue;
		pSrcLineAngle = srcangle_arr[i];
		for(int j=0;j<array_num;j++)
		{	//���ж����ת�Գ�ʱ��׼����Ϊͬһ��������ֻ��Ҫ������ת�Ƕȼ���
			if(pSrcLineAngle==NULL)
				continue;
			//������ת�Ƕ�
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSLineAngle *pMirAngle=CreateSingleMirAngle(pSrcLineAngle,mirmsg,bIncAttachInfo);
			if(pMirAngle)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcangle_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcangle_arr[0]->relativeObjs.GetNext())
				{	//�����ظ���ӹ�������
					if(pRelaObj->hObj==pMirAngle->handle)
					{
						bInList=TRUE;
						break;
					}
				}
				if(bInList)
					continue;
				pRelaObj=srcangle_arr[0]->relativeObjs.Add(pMirAngle->handle);
				srcangle_arr[0]->SetModified(TRUE,FALSE);
				pRelaObj->mirInfo.rotate_angle=init_angle*(j+1);
				pRelaObj->mirInfo.axis_flag=mirmsg.axis_flag;
				if(i>0)
					pRelaObj->mirInfo.axis_flag+=flagArr[i-1];
#if defined(__TSA_)||defined(__LDS_)
				if(bIncAttachInfo)
				{
					if(pSrcLineAngle->CalLenCoef.hHorizPole>0x20)
						MirPartHandle(&Ta,&pMirAngle->CalLenCoef.hHorizPole,pSrcLineAngle->CalLenCoef.hHorizPole,mirmsg);
					if(pSrcLineAngle->CalLenCoef.hRefPole>0x20)
						MirPartHandle(&Ta,&pMirAngle->CalLenCoef.hRefPole,pSrcLineAngle->CalLenCoef.hRefPole,mirmsg);
				}
#endif
			}
			else
				bSucceed=FALSE;
		}
	}
	//���öԳ����ɵĶ���˴˹���
	SetRelativeObj(srcangle_arr[0]);
	return bSucceed;
}
CLDSLineTube *CreateSingleMirTube(CLDSLineTube *pSrcLineTube, MIRMSG msg,BOOL bIncAttachInfo=TRUE,char *sErrorInfo=NULL)
{
	CLDSLineTube *pMirTube=NULL;
	pMirTube=(CLDSLineTube*)pSrcLineTube->GetMirPart(msg);
	if(pMirTube==NULL)
	{
		pMirTube = (CLDSLineTube*)console.AppendPart(CLS_LINETUBE);
		if(pSrcLineTube->pStart)
			pMirTube->pStart=pSrcLineTube->pStart->GetMirNode(msg);
		if(pSrcLineTube->pEnd)
			pMirTube->pEnd=pSrcLineTube->pEnd->GetMirNode(msg);
		if(pMirTube->pStart==NULL&&pSrcLineTube->pStart)
			pMirTube->pStart=pSrcLineTube->pStart;
		if(pMirTube->pEnd==NULL&&pSrcLineTube->pEnd)
			pMirTube->pEnd=pSrcLineTube->pEnd;
		if(pMirTube->pStart&&pMirTube->pStart->hFatherPart<0X20)
			pMirTube->pStart->hFatherPart=pMirTube->handle;
		if(pMirTube->pEnd&&pMirTube->pEnd->hFatherPart<0X20)
			pMirTube->pEnd->hFatherPart=pMirTube->handle;
		pMirTube->CopyProperty(pSrcLineTube);//�������ڵ�(����)����
		GEPOINT vMirWorkPlaneNormal=msg.MirVector(pSrcLineTube->WorkPlaneNorm());
		pMirTube->SetWorkPlaneNorm(vMirWorkPlaneNormal);
		AddMirObjToSrcObjRelaObjList(pSrcLineTube,pMirTube,msg);	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
	}
	//ͼ��
	CalMirAtomLayer(pSrcLineTube->layer(),pMirTube->layer(),msg);
	//ʼ�ն�λ��
	pMirTube->SetStart(GetMirPos(pSrcLineTube->Start(),msg));
	pMirTube->SetEnd(GetMirPos(pSrcLineTube->End(),msg));
	if(!bIncAttachInfo)
		return pMirTube;
	//�����淨��
	pMirTube->m_iNormCalStyle=pSrcLineTube->m_iNormCalStyle;
	pMirTube->nearWorkPlaneNorm=GetMirVector(pSrcLineTube->nearWorkPlaneNorm,msg);
	if(pMirTube->m_iNormCalStyle==0)	//ֱ��ָ�������淨�߷���
		pMirTube->SetWorkPlaneNorm(GetMirVector(pSrcLineTube->WorkPlaneNorm(),msg));
	else if(pSrcLineTube->nearWorkPlaneNorm.IsZero()&&pSrcLineTube->hWorkNormRefPart==0) 
		pMirTube->nearWorkPlaneNorm=GetMirVector(pSrcLineTube->WorkPlaneNorm(),msg);
	MirPartHandle(pSrcLineTube->BelongModel(),&pMirTube->hWorkNormRefPart,pSrcLineTube->hWorkNormRefPart,msg);
	//���Ӹ�����
	long hMirPartWeldParent=pMirTube->m_hPartWeldParent;
	MirPartHandle(pSrcLineTube->BelongModel(),&hMirPartWeldParent,pSrcLineTube->m_hPartWeldParent,msg);
	pMirTube->m_hPartWeldParent=hMirPartWeldParent;
	if(pMirTube!=pSrcLineTube)
	{	//ʼ�˶�λ��ʽ
		if(pMirTube->pStart&&pMirTube->pStart->hFatherPart!=pMirTube->handle)	
		{	
			pMirTube->desStartPos.endPosType=1;//�������
			pMirTube->desStartPos.hDatumPart=pMirTube->pStart->hFatherPart;
			pMirTube->desStartPos.datum_pos_style=pSrcLineTube->desStartPos.datum_pos_style;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirTube->desStartPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pMirTube->pEnd->Position(false)-pMirTube->pStart->Position(false))^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
				{	//X֫Ϊ��׼֫
					if(pMirTube->desStartPos.datum_pos_style==2||pMirTube->desStartPos.datum_pos_style==5)
						pMirTube->desStartPos.datum_pos_style-=1;
				}
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
				{	//Y֫Ϊ��׼֫
					if(pMirTube->desStartPos.datum_pos_style==1||pMirTube->desStartPos.datum_pos_style==4)
					pMirTube->desStartPos.datum_pos_style+=1;
				}
			}
		}
		if(pMirTube->desStartPos.endPosType==0) //�ڵ㼴����
			pMirTube->desStartPos.fNormOffset=pSrcLineTube->desStartPos.fNormOffset;
		//�ն˶�λ��ʽ
		if(pMirTube->pEnd&&pMirTube->pEnd->hFatherPart!=pMirTube->handle)	
		{	
			pMirTube->desEndPos.endPosType=1;//�������
			pMirTube->desEndPos.hDatumPart=pMirTube->pEnd->hFatherPart;
			pMirTube->desEndPos.datum_pos_style=pSrcLineTube->desEndPos.datum_pos_style;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirTube->desEndPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pMirTube->pEnd->Position(false)-pMirTube->pStart->Position(false))^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
				{	//X֫Ϊ��׼֫
					if(pMirTube->desEndPos.datum_pos_style==2||pMirTube->desEndPos.datum_pos_style==5)
						pMirTube->desEndPos.datum_pos_style-=1;
				}
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
				{	//Y֫Ϊ��׼֫
					if(pMirTube->desEndPos.datum_pos_style==1||pMirTube->desEndPos.datum_pos_style==4)
						pMirTube->desEndPos.datum_pos_style+=1;
				}
			}
		}
		if(pMirTube->desEndPos.endPosType==0) //�ڵ㼴����
			pMirTube->desEndPos.fNormOffset=pSrcLineTube->desEndPos.fNormOffset;
	}
	if(pSrcLineTube->m_cFuncType==CLDSLineTube::FUNC_WIREBUSH)
	{	//�׹ܶԳ�ʱ������ҶԳƵĶ�λ��˨�������ְ�  wxc-2017.3.20
		MirPartHandle(pSrcLineTube->BelongModel(),&pMirTube->bush.hBushRelyBolt,pSrcLineTube->bush.hBushRelyBolt,msg);
		MirPartHandle(pSrcLineTube->BelongModel(),&pMirTube->bush.hBushResidePlate,pSrcLineTube->bush.hBushResidePlate,msg);
	}
	pMirTube->CalPosition();
	NewLinePart(pMirTube);
	g_pSolidDraw->Draw();
	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
	AddMirObjToSrcObjRelaObjList(pSrcLineTube,pMirTube,msg);
	return pMirTube;
}
BOOL MirTube(CLDSLineTube *pSrcLineTube, MIRMSG mirmsg, BOOL bIncAttachInfo=TRUE,  char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//�Ƿ�ɹ��Գ��������й���
	RELATIVE_OBJECT *pRelaObj=NULL;
	if(!IsPartMirValid(pSrcLineTube,mirmsg))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "fail to pass symmetry legality checking, parts can not be symmetrical";
#else
		throw "δͨ���ԳƺϷ��Լ��,���ܶԳƹ���";
#endif
	int flagArr[3]={1,2,4};
	CLDSLineTube* srctube_arr[4]={NULL};
	srctube_arr[0]=pSrcLineTube;
	for(int i=0;i<3;i++)
	{
		if((mirmsg.axis_flag&flagArr[i])==0)
			continue;
		MIRMSG msg=mirmsg.GetSubMirItem(i+1);
		srctube_arr[i+1]=CreateSingleMirTube(pSrcLineTube,msg,bIncAttachInfo,sErrorInfo);
		if(srctube_arr[i+1]==NULL)
			return FALSE;
#if defined(__TSA_)||defined(__LDS_)
		else
		{
			if(bIncAttachInfo)
			{
				if(pSrcLineTube->CalLenCoef.hHorizPole>0x20)
					MirPartHandle(&Ta,&srctube_arr[i+1]->CalLenCoef.hHorizPole,pSrcLineTube->CalLenCoef.hHorizPole,mirmsg);
				if(pSrcLineTube->CalLenCoef.hRefPole>0x20)
					MirPartHandle(&Ta,&srctube_arr[i+1]->CalLenCoef.hRefPole,pSrcLineTube->CalLenCoef.hRefPole,mirmsg);
			}
		}
#endif
	}
	int array_num=mirmsg.array_num;
	if((mirmsg.axis_flag&8)==0)	//����ת�Գ�
		array_num=0;
	else
		mirmsg.axis_flag=8;
	short init_angle=mirmsg.rotate_angle;
	for(i=0;i<4;i++)
	{
		if(srctube_arr[i]==NULL)
			continue;
		pSrcLineTube = srctube_arr[i];
		for(int j=0;j<array_num;j++)
		{	//���ж����ת�Գ�ʱ��׼����Ϊͬһ��������ֻ��Ҫ������ת�Ƕȼ���
			if(pSrcLineTube==NULL)
				continue;
			//������ת�Ƕ�
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSLineTube *pMirTube=CreateSingleMirTube(pSrcLineTube,mirmsg);
			if(pMirTube)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srctube_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srctube_arr[0]->relativeObjs.GetNext())
				{	//�����ظ���ӹ�������
					if(pRelaObj->hObj==pMirTube->handle)
					{
						bInList=TRUE;
						break;
					}
				}
				if(bInList)
					continue;
				pRelaObj=srctube_arr[0]->relativeObjs.Add(pMirTube->handle);
				srctube_arr[0]->SetModified(TRUE,FALSE);
				pRelaObj->mirInfo.rotate_angle=init_angle*(j+1);
				pRelaObj->mirInfo.axis_flag=mirmsg.axis_flag;
				if(i>0)
					pRelaObj->mirInfo.axis_flag+=flagArr[i-1];
#if defined(__TSA_)||defined(__LDS_)
				if(bIncAttachInfo)
				{
					if(pSrcLineTube->CalLenCoef.hHorizPole>0x20)
						MirPartHandle(&Ta,&pMirTube->CalLenCoef.hHorizPole,pSrcLineTube->CalLenCoef.hHorizPole,mirmsg);
					if(pSrcLineTube->CalLenCoef.hRefPole>0x20)
						MirPartHandle(&Ta,&pMirTube->CalLenCoef.hRefPole,pSrcLineTube->CalLenCoef.hRefPole,mirmsg);
				}
#endif
			}
			else
				bSucceed=FALSE;
		}
	}
	//���öԳ����ɵĶ���˴˹���
	SetRelativeObj(srctube_arr[0]);
	return bSucceed;
}

CLDSLineFlat *CreateSingleMirFlat(CLDSLineFlat *pSrcLineFlat, MIRMSG msg, BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	CLDSLineFlat *pMirFlat=NULL;
	pMirFlat=(CLDSLineFlat*)pSrcLineFlat->GetMirPart(msg);
	if(pMirFlat==NULL)	
	{
		pMirFlat = (CLDSLineFlat*)console.AppendPart(CLS_LINEFLAT);
		if(pSrcLineFlat->pStart)
			pMirFlat->pStart=pSrcLineFlat->pStart->GetMirNode(msg);
		if(pSrcLineFlat->pEnd)
			pMirFlat->pEnd=pSrcLineFlat->pEnd->GetMirNode(msg);
		if(pMirFlat->pStart==NULL&&pSrcLineFlat->pStart)
			pMirFlat->pStart=pSrcLineFlat->pStart;
		if(pMirFlat->pEnd==NULL&&pSrcLineFlat->pEnd)
			pMirFlat->pEnd=pSrcLineFlat->pEnd;
		if(pMirFlat->pStart&&pMirFlat->pStart->hFatherPart<0X20)
			pMirFlat->pStart->hFatherPart=pMirFlat->handle;
		if(pMirFlat->pEnd&&pMirFlat->pEnd->hFatherPart<0X20)
			pMirFlat->pEnd->hFatherPart=pMirFlat->handle;
		pMirFlat->CopyProperty(pSrcLineFlat);//�������ڵ�(����)����
		AddMirObjToSrcObjRelaObjList(pSrcLineFlat,pMirFlat,msg);	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
	}
	//ͼ��
	CalMirAtomLayer(pSrcLineFlat->layer(),pMirFlat->layer(),msg);
	if(!bIncAttachInfo)
		return pMirFlat;
	//ʼ�ն�λ��
	pMirFlat->SetStart(GetMirPos(pSrcLineFlat->Start(),msg));
	pMirFlat->SetEnd(GetMirPos(pSrcLineFlat->End(),msg));
	//�����淨��
	pMirFlat->m_iNormCalStyle=pSrcLineFlat->m_iNormCalStyle;
	pMirFlat->nearWorkPlaneNorm=GetMirVector(pSrcLineFlat->nearWorkPlaneNorm,msg);
	if(pMirFlat->m_iNormCalStyle==0)	//ֱ��ָ�������淨�߷���
		pMirFlat->SetWorkPlaneNorm(GetMirVector(pSrcLineFlat->WorkPlaneNorm(),msg));
	else if(pSrcLineFlat->nearWorkPlaneNorm.IsZero()&&pSrcLineFlat->hWorkNormRefPart==0) 
		pMirFlat->nearWorkPlaneNorm=GetMirVector(pSrcLineFlat->WorkPlaneNorm(),msg);
	MirPartHandle(pSrcLineFlat->BelongModel(),&pMirFlat->hWorkNormRefPart,pSrcLineFlat->hWorkNormRefPart,msg);
	pMirFlat->CalWorkPlaneNorm();
	//���Ӹ�����
	long hMirPartWeldParent=pMirFlat->m_hPartWeldParent;
	MirPartHandle(pSrcLineFlat->BelongModel(),&hMirPartWeldParent,pSrcLineFlat->m_hPartWeldParent,msg);
	pMirFlat->m_hPartWeldParent=hMirPartWeldParent;
	if(pMirFlat!=pSrcLineFlat)
	{	//ʼ�˶�λ��ʽ
		if(pMirFlat->pStart&&pMirFlat->pStart->hFatherPart!=pMirFlat->handle)	
		{	
			pMirFlat->desStartPos.endPosType=1;//�������
			pMirFlat->desStartPos.hDatumPart=pMirFlat->pStart->hFatherPart;
			pMirFlat->desStartPos.datum_pos_style=pSrcLineFlat->desStartPos.datum_pos_style;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirFlat->desStartPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pMirFlat->pEnd->Position(false)-pMirFlat->pStart->Position(false))^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
				{	//X֫Ϊ��׼֫
					if(pMirFlat->desStartPos.datum_pos_style==2||pMirFlat->desStartPos.datum_pos_style==5)
						pMirFlat->desStartPos.datum_pos_style-=1;
				}
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
				{	//Y֫Ϊ��׼֫
					if(pMirFlat->desStartPos.datum_pos_style==1||pMirFlat->desStartPos.datum_pos_style==4)
						pMirFlat->desStartPos.datum_pos_style+=1;
				}
			}
		}
		if(pMirFlat->desStartPos.endPosType==0) //�ڵ㼴����
			pMirFlat->desStartPos.fNormOffset=pSrcLineFlat->desStartPos.fNormOffset;
		//�ն˶�λ��ʽ
		if(pMirFlat->pEnd&&pMirFlat->pEnd->hFatherPart!=pMirFlat->handle)	
		{	
			pMirFlat->desEndPos.endPosType=1;//�������
			pMirFlat->desEndPos.hDatumPart=pMirFlat->pEnd->hFatherPart;
			pMirFlat->desEndPos.datum_pos_style=pSrcLineFlat->desEndPos.datum_pos_style;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirFlat->desEndPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pMirFlat->pEnd->Position(false)-pMirFlat->pStart->Position(false))^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
				{	//X֫Ϊ��׼֫
					if(pMirFlat->desEndPos.datum_pos_style==2||pMirFlat->desEndPos.datum_pos_style==5)
						pMirFlat->desEndPos.datum_pos_style-=1;
				}
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
				{	//Y֫Ϊ��׼֫
					if(pMirFlat->desEndPos.datum_pos_style==1||pMirFlat->desEndPos.datum_pos_style==4)
						pMirFlat->desEndPos.datum_pos_style+=1;
				}
			}
		}
		if(pMirFlat->desEndPos.endPosType==0) //�ڵ㼴����
			pMirFlat->desEndPos.fNormOffset=pSrcLineFlat->desEndPos.fNormOffset;
	}
	NewLinePart(pMirFlat);
	if(pMirFlat->pStart==NULL&&pMirFlat->pEnd==NULL)
	{	//�Գ����ɻ�����۸��ϵ���˨
		for(CLsRef *pLsRef=pSrcLineFlat->GetFirstLsRef();pLsRef;pLsRef=pSrcLineFlat->GetNextLsRef())
		{
			CLDSBolt *pMirBolt=(CLDSBolt*)pLsRef->GetLsPtr()->GetMirPart(msg);
			if(pMirBolt)
			{
				pMirFlat->AppendMidLsRef(pMirBolt->GetLsRef());
				pMirBolt->ReplaceReferenceHandle(CHandleReplacer(pSrcLineFlat->handle,pMirFlat->handle));
				pMirBolt->SetModified();
				pMirBolt->Create3dSolidModel();
				if(!pMirBolt->m_bVirtualPart||(pMirBolt->m_bVirtualPart&&g_sysPara.display.bDispVirtualPart))
					g_pSolidDraw->NewSolidPart(pMirBolt->GetSolidPartObject());
			}
			else
			{
				pMirBolt=CreateSingleMirBolt(pLsRef->GetLsPtr(),msg);
				pMirFlat->AppendMidLsRef(pMirBolt->GetLsRef());
			}
			pMirBolt->correct_worknorm();
			pMirBolt->correct_pos();
			pMirBolt->CalGuigeAuto();
			pMirBolt->SetModified();
			pMirBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pMirBolt->GetSolidPartObject());
		}
	}
	g_pSolidDraw->Draw();
	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
	AddMirObjToSrcObjRelaObjList(pSrcLineFlat,pMirFlat,msg);
	return pMirFlat;
}
BOOL MirFlat(CLDSLineFlat *pSrcLineFlat, MIRMSG mirmsg, BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//�Ƿ�ɹ��Գ��������й���
	RELATIVE_OBJECT *pRelaObj=NULL;
	if(!IsPartMirValid(pSrcLineFlat,mirmsg))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "fail to pass symmetry legality checking, parts can not be symmetrical";
#else
		throw "δͨ���ԳƺϷ��Լ��,���ܶԳƹ���";
#endif
	int flagArr[3]={1,2,4};
	CLDSLineFlat* srcFlat_arr[4]={NULL};
	srcFlat_arr[0]=pSrcLineFlat;
	for(int i=0;i<3;i++)
	{
		if((mirmsg.axis_flag&flagArr[i])==0)
			continue;
		MIRMSG msg=mirmsg.GetSubMirItem(i+1);
		srcFlat_arr[i+1]=CreateSingleMirFlat(pSrcLineFlat,msg,bIncAttachInfo);
		if(srcFlat_arr[i+1]==NULL)
			return FALSE;
	}
	int array_num=mirmsg.array_num;
	if((mirmsg.axis_flag&8)==0)	//����ת�Գ�
		array_num=0;
	else
		mirmsg.axis_flag=8;
	short init_angle=mirmsg.rotate_angle;
	for(i=0;i<4;i++)
	{
		if(srcFlat_arr[i]==NULL)
			continue;
		pSrcLineFlat = srcFlat_arr[i];
		for(int j=0;j<array_num;j++)
		{	//���ж����ת�Գ�ʱ��׼����Ϊͬһ��������ֻ��Ҫ������ת�Ƕȼ���
			if(pSrcLineFlat==NULL)
				continue;
			//������ת�Ƕ�
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSLineFlat *pMirFlat=CreateSingleMirFlat(pSrcLineFlat,mirmsg,bIncAttachInfo);
			if(pMirFlat)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcFlat_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcFlat_arr[0]->relativeObjs.GetNext())
				{	//�����ظ���ӹ�������
					if(pRelaObj->hObj==pMirFlat->handle)
					{
						bInList=TRUE;
						break;
					}
				}
				if(bInList)
					continue;
				pRelaObj=srcFlat_arr[0]->relativeObjs.Add(pMirFlat->handle);
				srcFlat_arr[0]->SetModified(TRUE,FALSE);
				pRelaObj->mirInfo.rotate_angle=init_angle*(j+1);
				pRelaObj->mirInfo.axis_flag=mirmsg.axis_flag;
				if(i>0)
					pRelaObj->mirInfo.axis_flag+=flagArr[i-1];
			}
			else
				bSucceed=FALSE;
		}
	}
	//���öԳ����ɵĶ���˴˹���
	SetRelativeObj(srcFlat_arr[0]);
	return bSucceed;
}

CLDSLineSlot *CreateSingleMirSlot(CLDSLineSlot *pSrcLineSlot, MIRMSG msg, BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	CLDSLineSlot *pMirSlot=NULL;
	pMirSlot=(CLDSLineSlot*)pSrcLineSlot->GetMirPart(msg);
	if(pMirSlot==NULL)	
	{	
		pMirSlot = (CLDSLineSlot*)console.AppendPart(CLS_LINESLOT);
		if(pSrcLineSlot->pStart)
			pMirSlot->pStart=pSrcLineSlot->pStart->GetMirNode(msg);
		if(pSrcLineSlot->pEnd)
			pMirSlot->pEnd=pSrcLineSlot->pEnd->GetMirNode(msg);
		if(pMirSlot->pStart==NULL&&pSrcLineSlot->pStart)
			pMirSlot->pStart=pSrcLineSlot->pStart;
		if(pMirSlot->pEnd==NULL&&pSrcLineSlot->pEnd)
			pMirSlot->pEnd=pSrcLineSlot->pEnd;
		if(pMirSlot->pStart&&pMirSlot->pStart->hFatherPart<0X20)
			pMirSlot->pStart->hFatherPart=pMirSlot->handle;
		if(pMirSlot->pEnd&&pMirSlot->pEnd->hFatherPart<0X20)
			pMirSlot->pEnd->hFatherPart=pMirSlot->handle;
		pMirSlot->CopyProperty(pSrcLineSlot);//�������ڵ�(����)����
		AddMirObjToSrcObjRelaObjList(pSrcLineSlot,pMirSlot,msg);	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
		//ʼ�ն�λ��
		pMirSlot->SetStart(GetMirPos(pSrcLineSlot->Start(),msg));
		pMirSlot->SetEnd(GetMirPos(pSrcLineSlot->End(),msg));
	}
	//ͼ��
	CalMirAtomLayer(pSrcLineSlot->layer(),pMirSlot->layer(),msg);
	if(!bIncAttachInfo)
		return pMirSlot;
	//
	if(pSrcLineSlot->hAttachTube>0x20)
	{
		MirPartHandle(pMirSlot->BelongModel(),&pMirSlot->hAttachTube,pSrcLineSlot->hAttachTube,msg);
		//pSrcLineSlot->desDatumPoint.MirToPoint(pSrcLineSlot->BelongModel(),pMirSlot->desDatumPoint,msg);
		MirDatumPoint(pSrcLineSlot->BelongModel(),pSrcLineSlot->desDatumPoint,pMirSlot->desDatumPoint,msg);
	}
	pMirSlot->fLen=pSrcLineSlot->fLen;
	//�����淨��
	pMirSlot->m_iNormCalStyle=pSrcLineSlot->m_iNormCalStyle;
	pMirSlot->nearWorkPlaneNorm=GetMirVector(pSrcLineSlot->nearWorkPlaneNorm,msg);
	if(pMirSlot->m_iNormCalStyle==0)	//ֱ��ָ�������淨�߷���
		pMirSlot->SetWorkPlaneNorm(GetMirVector(pSrcLineSlot->WorkPlaneNorm(),msg));
	else if(pSrcLineSlot->nearWorkPlaneNorm.IsZero()&&pSrcLineSlot->hWorkNormRefPart==0) 
		pMirSlot->nearWorkPlaneNorm=GetMirVector(pSrcLineSlot->WorkPlaneNorm(),msg);
	MirPartHandle(pSrcLineSlot->BelongModel(),&pMirSlot->hWorkNormRefPart,pSrcLineSlot->hWorkNormRefPart,msg);
	pMirSlot->CalWorkPlaneNorm();
	//���Ӹ�����
	long hMirPartWeldParent=pMirSlot->m_hPartWeldParent;
	MirPartHandle(pSrcLineSlot->BelongModel(),&hMirPartWeldParent,pSrcLineSlot->m_hPartWeldParent,msg);
	pMirSlot->m_hPartWeldParent=hMirPartWeldParent;
	if(pMirSlot!=pSrcLineSlot)
	{	//ʼ�˶�λ��ʽ
		if(pMirSlot->pStart&&pMirSlot->pStart->hFatherPart!=pMirSlot->handle)	
		{	
			pMirSlot->desStartPos.endPosType=1;//�������
			pMirSlot->desStartPos.hDatumPart=pMirSlot->pStart->hFatherPart;
			pMirSlot->desStartPos.datum_pos_style=pSrcLineSlot->desStartPos.datum_pos_style;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirSlot->desStartPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pMirSlot->pEnd->Position(false)-pMirSlot->pStart->Position(false))^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
				{	//X֫Ϊ��׼֫
					if(pMirSlot->desStartPos.datum_pos_style==2||pMirSlot->desStartPos.datum_pos_style==5)
						pMirSlot->desStartPos.datum_pos_style-=1;
				}
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
				{	//Y֫Ϊ��׼֫
					if(pMirSlot->desStartPos.datum_pos_style==1||pMirSlot->desStartPos.datum_pos_style==4)
						pMirSlot->desStartPos.datum_pos_style+=1;
				}
			}
		}
		if(pMirSlot->desStartPos.endPosType==0) //�ڵ㼴����
			pMirSlot->desStartPos.fNormOffset=pSrcLineSlot->desStartPos.fNormOffset;
		//�ն˶�λ��ʽ
		if(pMirSlot->pEnd&&pMirSlot->pEnd->hFatherPart!=pMirSlot->handle)	
		{	
			pMirSlot->desEndPos.endPosType=1;//�������
			pMirSlot->desEndPos.hDatumPart=pMirSlot->pEnd->hFatherPart;
			pMirSlot->desEndPos.datum_pos_style=pSrcLineSlot->desEndPos.datum_pos_style;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirSlot->desEndPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pMirSlot->pEnd->Position(false)-pMirSlot->pStart->Position(false))^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
				{	//X֫Ϊ��׼֫
					if(pMirSlot->desEndPos.datum_pos_style==2||pMirSlot->desEndPos.datum_pos_style==5)
						pMirSlot->desEndPos.datum_pos_style-=1;
				}
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
				{	//Y֫Ϊ��׼֫
					if(pMirSlot->desEndPos.datum_pos_style==1||pMirSlot->desEndPos.datum_pos_style==4)
						pMirSlot->desEndPos.datum_pos_style+=1;
				}
			}
		}
		if(pMirSlot->desEndPos.endPosType==0) //�ڵ㼴����
			pMirSlot->desEndPos.fNormOffset=pSrcLineSlot->desEndPos.fNormOffset;
	}
	NewLinePart(pMirSlot);
	if(pMirSlot->pStart==NULL&&pMirSlot->pEnd==NULL)
	{	//�Գ����ɻ�����۸��ϵ���˨
		for(CLsRef *pLsRef=pSrcLineSlot->GetFirstLsRef();pLsRef;pLsRef=pSrcLineSlot->GetNextLsRef())
		{
			CLDSBolt *pMirBolt=(CLDSBolt*)pLsRef->GetLsPtr()->GetMirPart(msg);
			if(pMirBolt)
			{
				pMirSlot->AppendMidLsRef(pMirBolt->GetLsRef());
				pMirBolt->ReplaceReferenceHandle(CHandleReplacer(pSrcLineSlot->handle,pMirSlot->handle));
				pMirBolt->SetModified();
				pMirBolt->Create3dSolidModel();
				if(!pMirBolt->m_bVirtualPart||(pMirBolt->m_bVirtualPart&&g_sysPara.display.bDispVirtualPart))
					g_pSolidDraw->NewSolidPart(pMirBolt->GetSolidPartObject());
			}
			else
			{
				pMirBolt=CreateSingleMirBolt(pLsRef->GetLsPtr(),msg);
				pMirSlot->AppendMidLsRef(pMirBolt->GetLsRef());
			}
			pMirBolt->correct_worknorm();
			pMirBolt->correct_pos();
			pMirBolt->CalGuigeAuto();
			pMirBolt->SetModified();
			pMirBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pMirBolt->GetSolidPartObject());
		}
	}
	g_pSolidDraw->Draw();
	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
	AddMirObjToSrcObjRelaObjList(pSrcLineSlot,pMirSlot,msg);
	return pMirSlot;
}

BOOL MirSlot(CLDSLineSlot *pSrcLineSlot, MIRMSG mirmsg, BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//�Ƿ�ɹ��Գ��������й���
	RELATIVE_OBJECT *pRelaObj=NULL;
	if(!IsPartMirValid(pSrcLineSlot,mirmsg))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "fail to pass symmetry legality checking, parts can not be symmetrical";
#else
		throw "δͨ���ԳƺϷ��Լ��,���ܶԳƹ���";
#endif
	int flagArr[3]={1,2,4};
	CLDSLineSlot* srcSlot_arr[4]={NULL};
	srcSlot_arr[0]=pSrcLineSlot;
	for(int i=0;i<3;i++)
	{
		if((mirmsg.axis_flag&flagArr[i])==0)
			continue;
		MIRMSG msg=mirmsg.GetSubMirItem(i+1);
		srcSlot_arr[i+1]=CreateSingleMirSlot(pSrcLineSlot,msg,bIncAttachInfo);
		if(srcSlot_arr[i+1]==NULL)
			return FALSE;
	}
	int array_num=mirmsg.array_num;
	if((mirmsg.axis_flag&8)==0)	//����ת�Գ�
		array_num=0;
	else
		mirmsg.axis_flag=8;
	short init_angle=mirmsg.rotate_angle;
	for(i=0;i<4;i++)
	{
		if(srcSlot_arr[i]==NULL)
			continue;
		pSrcLineSlot = srcSlot_arr[i];
		for(int j=0;j<array_num;j++)
		{	//���ж����ת�Գ�ʱ��׼����Ϊͬһ��������ֻ��Ҫ������ת�Ƕȼ���
			if(pSrcLineSlot==NULL)
				continue;
			//������ת�Ƕ�
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSLineSlot *pMirSlot=CreateSingleMirSlot(pSrcLineSlot,mirmsg,bIncAttachInfo);
			if(pMirSlot)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcSlot_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcSlot_arr[0]->relativeObjs.GetNext())
				{	//�����ظ���ӹ�������
					if(pRelaObj->hObj==pMirSlot->handle)
					{
						bInList=TRUE;
						break;
					}
				}
				if(bInList)
					continue;
				pRelaObj=srcSlot_arr[0]->relativeObjs.Add(pMirSlot->handle);
				srcSlot_arr[0]->SetModified(TRUE,FALSE);
				pRelaObj->mirInfo.rotate_angle=init_angle*(j+1);
				pRelaObj->mirInfo.axis_flag=mirmsg.axis_flag;
				if(i>0)
					pRelaObj->mirInfo.axis_flag+=flagArr[i-1];
			}
			else
				bSucceed=FALSE;
		}
	}
	//���öԳ����ɵĶ���˴˹���
	SetRelativeObj(srcSlot_arr[0]);
	return bSucceed;
}

bool InitAnglePointByDatumPoint(CLDSPoint &datumPoint,CLDSLineAngle::ANGLEPOINT &anglePoint)
{
	if(datumPoint.datum_pos_style==1)
	{
		anglePoint.offset_wing=(BYTE)datumPoint.des_para.RODEND.offset_wing;
		anglePoint.wing_offset_style=datumPoint.des_para.RODEND.wing_offset_style;
		anglePoint.wing_offset_dist=datumPoint.des_para.RODEND.wing_offset_dist;
		anglePoint.len_offset_dist=datumPoint.des_para.RODEND.len_offset_dist;
		StrCopy(anglePoint.norm_offset,datumPoint.des_para.RODEND.norm_offset,51);
		anglePoint.direction=(BYTE)datumPoint.des_para.RODEND.direction;
	}
	else if(datumPoint.datum_pos_style==2)
	{
		anglePoint.offset_wing=(BYTE)datumPoint.des_para.RODNODE.offset_wing;
		anglePoint.wing_offset_style=datumPoint.des_para.RODNODE.wing_offset_style;
		anglePoint.wing_offset_dist=datumPoint.des_para.RODNODE.wing_offset_dist;
		anglePoint.len_offset_dist=datumPoint.des_para.RODNODE.len_offset_dist;
		StrCopy(anglePoint.norm_offset,datumPoint.des_para.RODNODE.norm_offset,51);
		anglePoint.direction=(BYTE)datumPoint.des_para.RODNODE.direction;
		anglePoint.hResideNode=datumPoint.des_para.RODNODE.hNode;
	}
	else
		return false;
	return true;
}

bool UpdateDatumPointByAnglePoint(CLDSPoint &datumPoint,CLDSLineAngle::ANGLEPOINT &anglePoint)
{
	if(datumPoint.datum_pos_style==1)
	{
		datumPoint.des_para.RODEND.offset_wing=anglePoint.offset_wing;
		datumPoint.des_para.RODEND.wing_offset_style=anglePoint.wing_offset_style;
		datumPoint.des_para.RODEND.wing_offset_dist=anglePoint.wing_offset_dist;
		datumPoint.des_para.RODEND.len_offset_dist=anglePoint.len_offset_dist;
		StrCopy(datumPoint.des_para.RODEND.norm_offset,anglePoint.norm_offset,51);
		datumPoint.des_para.RODEND.direction=anglePoint.direction;
	}
	else if(datumPoint.datum_pos_style==2)
	{
		datumPoint.des_para.RODNODE.offset_wing=anglePoint.offset_wing;
		datumPoint.des_para.RODNODE.wing_offset_style=anglePoint.wing_offset_style;
		datumPoint.des_para.RODNODE.wing_offset_dist=anglePoint.wing_offset_dist;
		datumPoint.des_para.RODNODE.len_offset_dist=anglePoint.len_offset_dist;
		StrCopy(datumPoint.des_para.RODNODE.norm_offset,anglePoint.norm_offset,51);
		datumPoint.des_para.RODNODE.direction=anglePoint.direction;
	}
	else 
		return false;
	return true;
}

CLDSPlate* CreateSingleMirPlate(CLDSPlate* pSrcPlate, MIRMSG msg, BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	CLDSPlate *pMirPlate=NULL;
	CDesignLjPartPara *pLjPart;
	pMirPlate=(CLDSPlate*)pSrcPlate->GetMirPart(msg);
	try{
		if(pMirPlate==NULL)
		{
			pMirPlate = (CLDSPlate*)console.AppendPart(CLS_PLATE);
			AddMirObjToSrcObjRelaObjList(pSrcPlate,pMirPlate,msg);	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
		}
		pMirPlate->CopyProperty(pSrcPlate);
		pMirPlate->ucs.origin=GetMirPos(pSrcPlate->ucs.origin,msg);
		pMirPlate->ucs.axis_x=GetMirVector(pSrcPlate->ucs.axis_x,msg);
		pMirPlate->ucs.axis_y=GetMirVector(pSrcPlate->ucs.axis_y,msg);
		pMirPlate->ucs.axis_z=GetMirVector(pSrcPlate->ucs.axis_z,msg);
		pMirPlate->m_tInnerColumnNorm=GetMirVector(pSrcPlate->m_tInnerColumnNorm,msg);
		if((msg.axis_flag&4)==0&&(msg.axis_flag&8)==0&&pSrcPlate->jdb_style!=5)
			pMirPlate->ucs.axis_x*=-1.0;	//����Z��Գƺ���ת�Գƻ򸽼Ӱ�(jdb_style==5��Ҫָʮ�ֲ�������)
		CalMirAtomLayer(pSrcPlate->layer(),pMirPlate->layer(),msg);
		//��ǰ��Ϊ�˷�ֹһ��װ�䷽λ��ͬ�ְ��ڳ���δ��������Ϣ�Գƺ󣬽��ж��θ�����Ϣ�Գ�ʱ���ҶԳƸְ���� wjh-2015.9.21
		pMirPlate->vertex_list.Empty();
		PROFILE_VERTEX *pPrevVertex=pSrcPlate->vertex_list.GetTail();
		for(PROFILE_VERTEX *pVertex=pSrcPlate->vertex_list.GetFirst();pVertex;pVertex=pSrcPlate->vertex_list.GetNext())
		{	//��� Ϊ����ڻ�׼����λ�ò����İ�ֱ�ӰѶ����б��ƹ�ȥ����
			if(msg.axis_flag&4||msg.axis_flag&8||pSrcPlate->jdb_style==5)	//Z��Գƻ���ת�Գ�
			{
				PROFILE_VERTEX vertex=*pVertex;
				if(vertex.type==3)	//��Բ��
				{
					vertex.column_norm=GetMirVector(vertex.column_norm,msg);
					vertex.center=GetMirPos(vertex.center,msg);
				}
				PROFILE_VERTEX* pTmVertex=pMirPlate->vertex_list.append(vertex);
				MirPartHandle(pSrcPlate->BelongModel(),&pTmVertex->m_hRod,vertex.m_hRod,msg);
			}
			else
			{
				PROFILE_VERTEX vertex=*pPrevVertex;
				vertex.vertex.Set(-pVertex->vertex.x,pVertex->vertex.y,pVertex->vertex.z);
				vertex.vertex.feature=pVertex->vertex.feature;
				//���ڣ��ᡢ����Գ�ʱԲ��ʰȡ������ҲӦ������Ӧ�任
				if(vertex.type==2)	//ָ��Բ���뾶��ʰȡ��
					vertex.center.x*=-1;
				else if(vertex.type==3)	//��Բ��
				{
					vertex.column_norm=GetMirVector(pPrevVertex->column_norm,msg);
					vertex.center=GetMirPos(pPrevVertex->center,msg);
				}
				vertex.m_cPosDesignType=pVertex->m_cPosDesignType;
				vertex.m_hRod=pVertex->m_hRod;
				PROFILE_VERTEX* pTmVertex=pMirPlate->vertex_list.insert(vertex,0);
				MirPartHandle(pSrcPlate->BelongModel(),&pTmVertex->m_hRod,vertex.m_hRod,msg);
				pPrevVertex=pVertex;
				//����λ�������˼��м����������ļ�������
				if(vertex.m_cPosDesignType>0&&vertex.m_cPosDesignType<5)
				{	
					pLjPart=pSrcPlate->GetLjRodPara(vertex.m_hRod);
					if(pLjPart==NULL || pLjPart->start0_end1!=2)
						continue;
					pLjPart->pCurLinePart=(CLDSLinePart*)Ta.Parts.FromHandle(pLjPart->hPart,CLS_LINEPART);
					CLDSLinePart* pTmLinePart=(CLDSLinePart*)Ta.Parts.FromHandle(pTmVertex->m_hRod,CLS_LINEPART);
					if(pTmLinePart==NULL || pLjPart->pCurLinePart==NULL)
						continue;
					f3dPoint line_vec=pLjPart->pCurLinePart->End()-pLjPart->pCurLinePart->Start();
					f3dPoint mir_line_vec=GetMirVector(line_vec,msg);
					f3dPoint tm_line_vec=pTmLinePart->End()-pTmLinePart->Start();
					if(mir_line_vec*tm_line_vec<0)	//�˼������෴
					{
						if(pTmVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER)
							pTmVertex->m_cPosDesignType=PROFILE_VERTEX::DT_BER2;
						else if(pTmVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER2)
							pTmVertex->m_cPosDesignType=PROFILE_VERTEX::DT_BER;
						else if(pTmVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING)
							pTmVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WING2;
						else if(pTmVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING2)
							pTmVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WING;

					}
				}
			}
		}
		if(!bIncAttachInfo)
			return pMirPlate;
		long hMirPartWeldParent=pMirPlate->m_hPartWeldParent;
		MirPartHandle(pSrcPlate->BelongModel(),&hMirPartWeldParent,pSrcPlate->m_hPartWeldParent,msg);
		pMirPlate->m_hPartWeldParent=hMirPartWeldParent;
		for(int j=0;j<pSrcPlate->face_N-1;j++)
		{
			pMirPlate->HuoQuLine[j]=pSrcPlate->HuoQuLine[j];
			pMirPlate->huoqufacenorm[j]=GetMirVector(pSrcPlate->huoqufacenorm[j],msg);
			pMirPlate->top_point=pSrcPlate->top_point;
		}
		/*for(f3dPoint *pPt=pSrcPlate->designInfo.additional_vertex.GetFirst();pPt;pPt=pSrcPlate->designInfo.additional_vertex.GetNext())
		{
		if(msg.axis_flag&4||msg.axis_flag&8)	//Z��Գƻ���ת�Գ�
		pMirPlate->designInfo.additional_vertex.append(*pPt);
		else
		pMirPlate->designInfo.additional_vertex.append(-pPt->x,pPt->y,pPt->z);
		}*/
		if((msg.axis_flag&4)==0&&(msg.axis_flag&8)==0&&pSrcPlate->jdb_style!=5)	//����Z��Գƺ���ת�Գƻ򸽼Ӱ�
		{
			//������������ʼ�� wjh-2015.6.30
			//pMirPlate->ucs.axis_x*=-1.0;
			pMirPlate->HuoQuLine[0].startPt.x*=-1.0;
			pMirPlate->HuoQuLine[0].endPt.x*=-1.0;
			pMirPlate->HuoQuLine[1].startPt.x*=-1.0;
			pMirPlate->HuoQuLine[1].endPt.x*=-1.0;
			if(pSrcPlate->IsPeakPointPlate())//pMirPlate->jdb_style==0&&pMirPlate->face_N==3)
				pMirPlate->top_point.x*=-1.0;
		}
		//���Ӱ���������Ϣ�Գ�
		pMirPlate->designInfo.m_bWeldToDatumJg=pSrcPlate->designInfo.m_bWeldToDatumJg;	//�ڵ�庸���ڻ�׼�Ǹ��� wht 10-06-07
		pMirPlate->designInfo.iFaceType=pSrcPlate->designInfo.iFaceType;
		pMirPlate->designInfo.iProfileStyle0123=pSrcPlate->designInfo.iProfileStyle0123;
		pMirPlate->designInfo.m_bEnableFlexibleDesign=pSrcPlate->designInfo.m_bEnableFlexibleDesign;
		pMirPlate->designInfo.iWeldLineStyle=pSrcPlate->designInfo.iWeldLineStyle;
		pMirPlate->designInfo.m_bSelfCorrectBendline=pSrcPlate->designInfo.m_bSelfCorrectBendline;
		MirPartHandle(pSrcPlate->BelongModel(),&pMirPlate->designInfo.m_hBaseNode,pSrcPlate->designInfo.m_hBaseNode,msg);
		if(pSrcPlate->designInfo.m_hBaseNode>0x20&&pMirPlate->designInfo.m_hBaseNode<0x20)
		{	//�ְ���ڻ�׼�ڵ㵫δ�ҵ��Ϸ��ĶԳƽڵ�,�޷���ɼ����Գ����� wht 10-10-19
			CLDSNode *pBaseNode=pSrcPlate->BelongModel()->FromNodeHandle(pSrcPlate->designInfo.m_hBaseNode);
			if(pBaseNode)
			{	//�˴�������ϢӦ���ݵ��ⲿ����ӡ�����±���
				if(sErrorInfo)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sErrorInfo,"0X%X basic point symmetry failed!",pBaseNode->handle);
#else
					sprintf(sErrorInfo,"0X%X����Գ�ʧ��!",pBaseNode->handle);
#endif
				return NULL;
			}
		}
		MirPartHandle(pSrcPlate->BelongModel(),&pMirPlate->designInfo.m_hBasePart,pSrcPlate->designInfo.m_hBasePart,msg,&pSrcPlate->ucs.origin);
		if(pSrcPlate->designInfo.m_hBasePart>0x20&&pMirPlate->designInfo.m_hBasePart<0x20)
		{	//�ְ���ڻ�׼�˼���δ�ҵ��Ϸ��ĶԳƸ˼�,�޷���ɼ����Գ����� wht 10-10-19
			CLDSPart *pBaseLinePart=pSrcPlate->BelongModel()->FromPartHandle(pSrcPlate->designInfo.m_hBasePart,CLS_LINEPART);
			if(pBaseLinePart)
			{
				if(sErrorInfo)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sErrorInfo,"0X%X datum rod symmetry failed!",pBaseLinePart->handle);
#else
					sprintf(sErrorInfo,"0X%X��׼�˼��Գ�ʧ��!",pBaseLinePart->handle);
#endif
				return NULL;
			}
		}
		pMirPlate->designInfo.weld_width=pSrcPlate->designInfo.weld_width;
		pMirPlate->designInfo.weld_start_high=pSrcPlate->designInfo.weld_start_high;
		pMirPlate->designInfo.weld_end_high=pSrcPlate->designInfo.weld_end_high;
		//���㼰�������������Ϣ�Գ�
		MirDatumPoint(pSrcPlate->BelongModel(),pSrcPlate->designInfo.origin,pMirPlate->designInfo.origin,msg);
		if(pSrcPlate->designInfo.norm.vector.IsZero())	//���·�����Ʋ��� wht 11-01-11
			pSrcPlate->designInfo.norm.UpdateVector(&Ta);
		CLDSNode *pBaseNode=pSrcPlate->GetBaseNode();
		f3dPoint *pPickPos=NULL;
		if(pBaseNode)
			pPickPos=&pBaseNode->Position(false);
		MirDatumVector(pSrcPlate->BelongModel(),pSrcPlate->designInfo.norm,pMirPlate->designInfo.norm,msg,pPickPos);
		BOOL bSameDatumRod=FALSE; //�ԳƸְ����ͬһ�˼���λ
		if(pSrcPlate->designInfo.origin.m_hDatumRod==pMirPlate->designInfo.origin.m_hDatumRod&&pSrcPlate->designInfo.origin.m_hDatumRod!=0)
			bSameDatumRod=TRUE;
		//
		bool bResideGroupAngle=false;
		SmartPartPtr pDatumPart=Ta.Parts.FromHandle(pSrcPlate->designInfo.origin.m_hDatumRod);
		CLDSLineAngle *pDatumAngle=(pDatumPart.IsHasPtr()&&pDatumPart->IsAngle())?pDatumPart.pAngle:NULL;
		CLDSLineTube *pDatumTube=(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())?pDatumPart.pTube:NULL;
		BOOL bSameDatumAngle=bSameDatumRod&&(pDatumAngle!=NULL);
		BOOL bSameDatumTube =bSameDatumRod&&(pDatumTube!=NULL);
		if(bSameDatumRod&&!bSameDatumAngle&&!bSameDatumTube)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Diagnoal mirror is unsupported, as the dependent rod for plate is not angle type.");
#else
			AfxMessageBox("�ݲ�֧�ְָ�Ļ�׼�˼��ǽǸ���ĽǶԳ�");
#endif
			return NULL;
		}
		CLDSLineAngle *pMirDatumAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirPlate->designInfo.origin.m_hDatumRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if( !bSameDatumRod&&pDatumAngle&&pMirDatumAngle&&
			(pDatumAngle->GetClassTypeId()==CLS_GROUPLINEANGLE||pDatumAngle->group_father_jg_h>0x20))
		{	//��׼����Ϊ��ϽǸ��Ҳ�������Գ�ʱ��Ҫ���¸ְ��׼����Ʋ��� wht-17.06.05
			bResideGroupAngle=true;
			CLDSLineAngle::ANGLEPOINT fromAnglePoint,toAnglePoint;
			if( InitAnglePointByDatumPoint(pSrcPlate->designInfo.origin,fromAnglePoint))
			{
				if(pSrcPlate->designInfo.origin.datum_pos_style==1)
				{
					fromAnglePoint.hResideNode=pSrcPlate->designInfo.origin.des_para.RODEND.direction==0?pDatumAngle->pStart->handle:pDatumAngle->pEnd->handle;
					toAnglePoint.hResideNode=pMirPlate->designInfo.origin.des_para.RODEND.direction==0?pMirDatumAngle->pStart->handle:pMirDatumAngle->pEnd->handle;
				}
				if(pDatumAngle->GetMirAnglePoint(fromAnglePoint,toAnglePoint,pMirDatumAngle,msg))
					UpdateDatumPointByAnglePoint(pMirPlate->designInfo.origin,toAnglePoint);
			}
		}
		BOOL bDatumJgStyleDiff=FALSE; //��׼�Ǹ���������ʽ�Ƿ�һ�� wht 11-01-17
		if(pMirPlate->designInfo.norm.norm_style==1&&pSrcPlate->designInfo.norm.norm_style==1
			&&pMirPlate->designInfo.norm.direction!=pSrcPlate->designInfo.norm.direction&&!bSameDatumRod
			&&!bResideGroupAngle)	//TODO:T����ϽǸֶ���Ӧ�����Ǹִ�����δ���� wht-17.06.05
		{	//�ְ��׼�Ǹ�һ��Ϊ����һ��Ϊ����(��Ҫ����������б���ϸ������Ӱ�ĶԳ�)
			pMirPlate->designInfo.norm.direction=pSrcPlate->designInfo.norm.direction;
			pMirPlate->designInfo.norm.UpdateVector(&Ta);
			pMirPlate->DesignSetupUcs();	//������Ƹְ�װ������ϵ
			bDatumJgStyleDiff=TRUE; 
		}
		else
		{
			pMirPlate->ucs.axis_z=pMirPlate->ucs.axis_x^pMirPlate->ucs.axis_y;
			pMirPlate->designInfo.norm.UpdateVector(&Ta);
			if(pMirPlate->designInfo.norm.vector*pMirPlate->ucs.axis_z<0)	//Ӧ���������淭ת
			{	//ֻ����jdb_style==5��ʮ�ֲ������Ż����
				if(pMirPlate->designInfo.norm.norm_style==0)
					pMirPlate->designInfo.norm.vector=pMirPlate->designInfo.norm.nearVector=pMirPlate->ucs.axis_z;
				else if(pMirPlate->designInfo.norm.norm_style==1)
					pMirPlate->designInfo.norm.direction*=-1;
				else
					pMirPlate->designInfo.norm.nearVector*=-1.0;
				pMirPlate->m_fNormOffset=-pMirPlate->m_fNormOffset-pMirPlate->GetThick();
			}
			else if(bSameDatumAngle)
			{	//ͬ��׼�Ǹֵĸְ�Գ� wjh-2017.9.27
				bool srcOrgNormalSameDirect=true,mirOrgNormalSameDirect=true;
				GEPOINT vOrgWorkNorm,vMirOrgWorkNorm;
				int offset_wing=-1;
				if(pSrcPlate->designInfo.origin.datum_pos_style==1||pSrcPlate->designInfo.origin.datum_pos_style==7)
					offset_wing=pSrcPlate->designInfo.origin.des_para.RODEND.offset_wing;
				else if(pSrcPlate->designInfo.origin.datum_pos_style==2)
					offset_wing=pSrcPlate->designInfo.origin.des_para.RODNODE.offset_wing;
				if(pDatumAngle&&offset_wing==0)
					vOrgWorkNorm=pDatumAngle->vxWingNorm;
				else if(pDatumAngle&&offset_wing==1)
					vOrgWorkNorm=pDatumAngle->vyWingNorm;
				if(vOrgWorkNorm*pSrcPlate->ucs.axis_z<0)
					srcOrgNormalSameDirect=false;
				offset_wing=-1;
				if(pMirPlate->designInfo.origin.datum_pos_style==1||pMirPlate->designInfo.origin.datum_pos_style==7)
					offset_wing=pMirPlate->designInfo.origin.des_para.RODEND.offset_wing;
				else if(pMirPlate->designInfo.origin.datum_pos_style==2)
					offset_wing=pMirPlate->designInfo.origin.des_para.RODNODE.offset_wing;
				if(pDatumAngle&&offset_wing==0)
					vMirOrgWorkNorm=pDatumAngle->vxWingNorm;
				else if(pDatumAngle&&offset_wing==1)
					vMirOrgWorkNorm=pDatumAngle->vyWingNorm;
				if(vMirOrgWorkNorm*pMirPlate->ucs.axis_z<0)
					mirOrgNormalSameDirect=false;
				if(srcOrgNormalSameDirect!=mirOrgNormalSameDirect&&!vOrgWorkNorm.IsZero()&&!vMirOrgWorkNorm.IsZero())
				{	//��λ��׼�Ǹ���ͬ,��һ��巨�߳��Ǹ����,һ��巨�߳��Ǹ��ڲ�(�������ͶԽ���ϽǸֽǶԳ�)
					CParaThick paraThick(pMirPlate->designInfo.origin.des_para.RODNODE.norm_offset);
					int index=0;
					for(THICK_ITEM item=paraThick.GetFirstItem();item.val!=0;item=paraThick.GetNextItem(),index++)
					{
						item.val*=-1;	//����
						paraThick.SetItem(index,item);
					}
					StrCopy(pMirPlate->designInfo.origin.des_para.RODNODE.norm_offset,(char*)paraThick.ToThickStr(),51);
					//sprintf(pMirPlate->designInfo.origin.des_para.RODNODE.norm_offset,"%f",-1.0*paraThick.Thick(&Ta));
				}
				//����Ӧ��II����ϽǸ�����ǶԳ�ʱ����ĶԳ�ͳһ��ANGLEPOINT�Գ��У�Ŀǰ������ʱ���������������ʱ���ְ����ζԳƴ���wjh-2017.6.6
				CLDSLineAngle::ANGLEPOINT fromAnglePoint,toAnglePoint;
				if( InitAnglePointByDatumPoint(pSrcPlate->designInfo.origin,fromAnglePoint))
				{
					if(pSrcPlate->designInfo.origin.datum_pos_style==1)
					{
						fromAnglePoint.hResideNode=pSrcPlate->designInfo.origin.des_para.RODEND.direction==0?pDatumAngle->pStart->handle:pDatumAngle->pEnd->handle;
						toAnglePoint.hResideNode=pMirPlate->designInfo.origin.des_para.RODEND.direction==0?pMirDatumAngle->pStart->handle:pMirDatumAngle->pEnd->handle;
					}
					if(pDatumAngle->GetMirAnglePoint(fromAnglePoint,toAnglePoint,pDatumAngle,msg))
					{
						if(pSrcPlate->designInfo.origin.datum_pos_style==1)
						{
							pMirPlate->designInfo.origin.des_para.RODEND.wing_offset_style=toAnglePoint.wing_offset_style;
							pMirPlate->designInfo.origin.des_para.RODEND.wing_offset_dist=toAnglePoint.wing_offset_dist;
						}
						else if(pSrcPlate->designInfo.origin.datum_pos_style==2)
						{
							pMirPlate->designInfo.origin.des_para.RODNODE.wing_offset_style=toAnglePoint.wing_offset_style;
							pMirPlate->designInfo.origin.des_para.RODNODE.wing_offset_dist=toAnglePoint.wing_offset_dist;
						}
						//UpdateDatumPointByAnglePoint(pMirPlate->designInfo.origin,toAnglePoint);
					}
				}
			}
			//else if(bSameDatumTube)
			//	ͬ��׼�ֹ�ʱ�ݲ����ĸְ嶨λ��׼��
		}
		pMirPlate->designInfo.norm.UpdateVector(&Ta);
		if(pSrcPlate->face_N>1)
		{
			MirDatumPoint(pSrcPlate->BelongModel(),pSrcPlate->designInfo.huoquline_start[0],pMirPlate->designInfo.huoquline_start[0],msg);
			MirDatumPoint(pSrcPlate->BelongModel(),pSrcPlate->designInfo.huoquline_end[0],pMirPlate->designInfo.huoquline_end[0],msg);
			MirDatumVector(pSrcPlate->BelongModel(),pSrcPlate->designInfo.huoqufacenorm[0],pMirPlate->designInfo.huoqufacenorm[0],msg);
			//TODO:ñ�ӶԳ��Ƿ������⣿
			//��׼�Ǹ���������ʽ��һ��ʱ���ԳƸְ�Z����(��׼�ְ�Z��)�ԳƷ����෴,�����淽��ҲӦ�÷���
			//CLDSPlate::SyncPlateProfile()������Ҳ����ش���,����Ӧͳһ���� wht 16-09-19
			pMirPlate->designInfo.huoqufacenorm[0].UpdateVector(&Ta);
			double dd1=pSrcPlate->huoqufacenorm[0]*pSrcPlate->ucs.axis_z;
			double dd2=pMirPlate->designInfo.huoqufacenorm[0].vector*pMirPlate->designInfo.norm.vector;
			if(dd1*dd2<0)
				pMirPlate->designInfo.huoqufacenorm[0].ReverseDirection();
		}
		if(pSrcPlate->face_N>2)
		{
			MirDatumPoint(pSrcPlate->BelongModel(),pSrcPlate->designInfo.huoquline_start[1],pMirPlate->designInfo.huoquline_start[1],msg);
			MirDatumPoint(pSrcPlate->BelongModel(),pSrcPlate->designInfo.huoquline_end[1],pMirPlate->designInfo.huoquline_end[1],msg);
			MirDatumVector(pSrcPlate->BelongModel(),pSrcPlate->designInfo.huoqufacenorm[1],pMirPlate->designInfo.huoqufacenorm[1],msg);
			//��׼�Ǹ���������ʽ��һ��ʱ���ԳƸְ�Z����(��׼�ְ�Z��)�ԳƷ����෴,�����淽��ҲӦ�÷���
			//CLDSPlate::SyncPlateProfile()������Ҳ����ش���,����Ӧͳһ���� wht 16-09-19
			pMirPlate->designInfo.huoqufacenorm[1].UpdateVector(&Ta);
			double dd1=pSrcPlate->huoqufacenorm[1]*pSrcPlate->ucs.axis_z;
			double dd2=pMirPlate->designInfo.huoqufacenorm[1].vector*pMirPlate->designInfo.norm.vector;
			if(dd1*dd2<0)
				pMirPlate->designInfo.huoqufacenorm[1].ReverseDirection();
		}
		//���ݻ��������Ϣ���¼�����λ��
		pMirPlate->designInfo.origin.UpdatePos(&Ta);
		//�޻�׼�������Ϣʱ�����°��λ��
		if(pMirPlate->designInfo.origin.datum_pos_style>0)
			pMirPlate->ucs.origin=pMirPlate->designInfo.origin.Position();
		else
			pMirPlate->designInfo.origin.datum_pos=pMirPlate->ucs.origin;
		//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
		AddMirObjToSrcObjRelaObjList(pSrcPlate,pMirPlate,msg);
		//������˨�Գ�	//�����еĸ˼�Ҫ������˨��������ͷ����Ӧ�Ƚ���˨�Գ�����
		CHashList<long> ljPartStart0End1HashList;
		ARRAY_LIST<THANDLE> listRelaParts(0,pSrcPlate->designInfo.partList.GetNodeNum()+1);
		for(pLjPart=pSrcPlate->designInfo.partList.GetFirst();pLjPart;pLjPart=pSrcPlate->designInfo.partList.GetNext())
		{
			ljPartStart0End1HashList.SetValue(pLjPart->hPart,pLjPart->start0_end1);
			listRelaParts.append(pLjPart->hPart);
		}
		//���ƶԳƸְ���˨����ʱ��Ӧ���ǻ�׼�˼�(˫�������Գ�ʱ��˨δ���뵽��׼�Ǹ���) wht 12-05-21
		if(ljPartStart0End1HashList.GetValue(pSrcPlate->designInfo.m_hBasePart)==NULL)
			ljPartStart0End1HashList.SetValue(pSrcPlate->designInfo.m_hBasePart,2);
		CLsRef *pLsRef;
		for(pLsRef=pSrcPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcPlate->GetNextLsRef())
		{
			CLDSBolt *pMirBolt=(CLDSBolt*)pLsRef->GetLsPtr()->GetMirPart(msg);
			CLsRef *pNewLsRef=NULL;
			if(pMirBolt)
			{
				pNewLsRef=pMirPlate->AppendLsRef(pMirBolt->GetLsRef());
				pMirBolt->ReplaceReferenceHandle(CHandleReplacer(pSrcPlate->handle,pMirPlate->handle));
				//�����˨ͨ��������޵�ǰ�ְ���ʱ����Ӹְ�����ͨ����� wht 11-01-11
				pMirBolt->AddL0Thick(pMirPlate->handle,TRUE,TRUE);
				pMirBolt->CalGuigeAuto();	//ͨ��仯��,���¼�����˨���
				pMirBolt->SetModified();
				if ( UI::blEnableIntermediateUpdateUI&&(
					!pMirBolt->m_bVirtualPart || (pMirBolt->m_bVirtualPart&&g_sysPara.display.bDispVirtualPart)))
				{
					pMirBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pMirBolt->GetSolidPartObject());
				}
			}
			else
			{
				pMirBolt=CreateSingleMirBolt(pLsRef->GetLsPtr(),msg,TRUE,listRelaParts.m_pData,listRelaParts.GetSize());
				pNewLsRef=pMirPlate->AppendLsRef(pMirBolt->GetLsRef());
			}
			if(pNewLsRef!=NULL)
				pNewLsRef->feature=0;	//��ոְ���˨����feature����,���������ӹ�����˨����ʱʹ�� wht 11-01-17
			pNewLsRef->cFlag=pLsRef->cFlag;

			for(long *pStart0End1=ljPartStart0End1HashList.GetFirst();pStart0End1;pStart0End1=ljPartStart0End1HashList.GetNext())
			{
				CSuperSmartPtr<CLDSPart>pSrcPart=Ta.Parts.FromHandle(ljPartStart0End1HashList.GetCursorKey());
				if(pSrcPart.IsHasPtr()&&pSrcPart->GetClassTypeId()==CLS_LINEANGLE)
				{
					CLDSLineAngle *pMirAngle=NULL;
					//����һ�˶ԳƲ��ҶԳƸ˼�ʱ������ 
					/*
					f3dPoint len_vec=pSrcPart.LineAnglePointer()->End()-pSrcPart.LineAnglePointer()->Start();
					normalize(len_vec);
					if(fabs(len_vec*f3dPoint(1,0,0))>EPS_COS2||	//�����ĶԳƸ˼�ʱ��������ֻ��һ�˶Գ�
						msg.axis_flag==8||msg.axis_flag==4)		//��ת�Գƻ�Z��Գ�
						pMirAngle=(CLDSLineAngle*)pSrcPart->GetMirPart(msg);
					else
						pMirAngle=(CLDSLineAngle*)pSrcPart->GetMirPart(msg,pLjPart->start0_end1);
					*/
					f3dPoint pickPos;
					if(pSrcPart.LinePartPointer()->pStart&&pSrcPart.LinePartPointer()->pStart->handle==pSrcPlate->designInfo.m_hBaseNode)
						pickPos=pSrcPart.LinePartPointer()->pStart->Position(false);
					else if(pSrcPart.LinePartPointer()->pEnd&&pSrcPart.LinePartPointer()->pEnd->handle==pSrcPlate->designInfo.m_hBaseNode)
						pickPos=pSrcPart.LinePartPointer()->pEnd->Position(false);
					pMirAngle=(CLDSLineAngle*)pSrcPart->GetMirPart(msg,pickPos);
					if(pMirAngle==NULL)
						continue;
					if(pSrcPart.LineAnglePointer()->FindLsByHandle(pLsRef->GetLsPtr()->handle)!=NULL)
					{
						int start0_end1=*pStart0End1;
						if(pSrcPart.IsEqualPtr(pMirAngle))
						{
							if(pMirAngle->pStart!=NULL&&pMirAngle->pStart->handle==pMirPlate->designInfo.m_hBaseNode)
								start0_end1=0;//1-pLjPart->start0_end1;
							else if(pMirAngle->pEnd!=NULL&&pMirAngle->pEnd->handle==pMirPlate->designInfo.m_hBaseNode)
								start0_end1=1;
							else
								start0_end1=2;
						}
						if(start0_end1==0)
							pMirAngle->AppendStartLsRef(pMirBolt->GetLsRef());
						else if(start0_end1==1)
							pMirAngle->AppendEndLsRef(pMirBolt->GetLsRef());
						else
							pMirAngle->AppendMidLsRef(pMirBolt->GetLsRef());
					}
				}
				else if(pSrcPart.IsHasPtr()&&pSrcPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
				{	//��ϽǸ� �����˨����ʱ���Զ��ж���˨�Ƿ�λ���ӽǸ��� wht 11-01-13
					CLDSLineAngle *pMirAngle=NULL;
					pMirAngle=(CLDSLineAngle*)pSrcPart->GetMirPart(msg);
					if(pMirAngle==NULL)
						continue;
					int start0_end1=*pStart0End1;
					if(pSrcPart.IsEqualPtr(pMirAngle))
					{
						if(pMirAngle->pStart!=NULL&&pMirAngle->pStart->handle==pMirPlate->designInfo.m_hBaseNode)
							start0_end1=0;//1-pLjPart->start0_end1;
						else if(pMirAngle->pEnd!=NULL&&pMirAngle->pEnd->handle==pMirPlate->designInfo.m_hBaseNode)
							start0_end1=1;
						else
							start0_end1=2;
					}
					if(start0_end1==0)	 
						pMirAngle->AppendStartLsRef(pMirBolt->GetLsRef());
					else if(start0_end1==1)
						pMirAngle->AppendEndLsRef(pMirBolt->GetLsRef());
					else
						pMirAngle->AppendMidLsRef(pMirBolt->GetLsRef());
				}
			}
		}
		//��׼�ڵ�Ϊ��׼�˼��˵�ʱ�Գƻ�׼�˼�����ͷ��Ϣ
		long hBaseNo=pSrcPlate->designInfo.m_hBaseNode;
		long hMirBaseNo=pMirPlate->designInfo.m_hBaseNode;
		CLDSLinePart *pBaseLinePart=(CLDSLinePart*)Ta.Parts.FromHandle(pSrcPlate->designInfo.m_hBasePart,CLS_LINEPART);
		CLDSLinePart *pMirBaseLinePart=(CLDSLinePart*)Ta.Parts.FromHandle(pMirPlate->designInfo.m_hBasePart,CLS_LINEPART);
		if(pBaseLinePart&&pMirBaseLinePart&&pBaseLinePart->pStart&&
			pBaseLinePart->pEnd&&pMirBaseLinePart->pStart&&pMirBaseLinePart->pEnd&&
			(pBaseLinePart->pStart->handle==hBaseNo||pBaseLinePart->pEnd->handle==hBaseNo)&&
			(pMirBaseLinePart->pStart->handle==hMirBaseNo||pMirBaseLinePart->pEnd->handle==hMirBaseNo))
		{
			int start0_end1=0,mir_start0_end1=0;
			if(pBaseLinePart->pEnd->handle==hBaseNo)
				start0_end1=1;
			if(pMirBaseLinePart->pEnd->handle==hMirBaseNo)
				mir_start0_end1=1;
			MirLinePartOddDesignInfo(pBaseLinePart,pMirBaseLinePart,msg,start0_end1==0,mir_start0_end1==0);
			pMirBaseLinePart->SetModified();
			if (UI::blEnableIntermediateUpdateUI)
			{
				pMirBaseLinePart->Create3dSolidModel(g_sysPara.bDisplayAllHole, g_pSolidOper->GetScaleUserToScreen(), g_pSolidSet->GetArcSamplingLength());
				g_pSolidDraw->NewSolidPart(pMirBaseLinePart->GetSolidPartObject());
			}
		}
		//��ǰԤ�Գ�����ͷֵ���Է�������߽Ǹ�ʱ�򲿷����ڽǸָ�ͷδ�ԳƵ����нǼ������� wjh-2017.6.26
		CHashSet<CDesignLjPartPara*> hashMirLjRodParas;
		CHashList<bool> hashParentGroupAngles;	//��֤��ϽǸֶԳ�Ψһ�Թ�ϣ���ְ�����Ӹ˼�Ϊ�ӽǸ�ʱ����Ҫ�����ӽǸ��ҵ���ϽǸ֣� wjh-2019.9.18
		for(pLjPart=pSrcPlate->designInfo.partList.GetFirst();pLjPart;pLjPart=pSrcPlate->designInfo.partList.GetNext())
		{
			pLjPart->pCurLinePart=(CLDSLinePart*)Ta.Parts.FromHandle(pLjPart->hPart,CLS_LINEPART);
			if(pLjPart->pCurLinePart==NULL)
				continue;
			long hMirLjPart=pLjPart->hPart;
			if(pLjPart->pCurLinePart->GetClassTypeId()==CLS_LINETUBE)	//�ֹ�����һ�˶Գ�
			{
				f3dPoint pickMirPos,*pPickPos=NULL;
				if(pLjPart->start0_end1==0||pLjPart->start0_end1==1)
				{
					pickMirPos=pLjPart->pCurLinePart->GetMirPickPos(pLjPart->start0_end1);
					pPickPos=&pickMirPos;
				}
				MirPartHandle(pSrcPlate->BelongModel(),&hMirLjPart,pLjPart->hPart,msg,pPickPos);
			}
			else
				MirPartHandle(pSrcPlate->BelongModel(),&hMirLjPart,pLjPart->hPart,msg);//,pLjPart->start0_end1
			/*����pLjPart->hPart���pMirPlate�Ĺ����������������ڵ���ModifyKeyId�������º����¼ӵĹ���������֮ǰ�Ĺ��������ظ�,��ʧ����������Ϣ��
				��ֱ�ӻ�ȡpLjPart->hPart�ĶԳƾ��,Ȼ�������pMirPlate�Ĺ������� wxc-2017.5.15*/
			CLDSLinePart* pMirLinePart=(CLDSLinePart*)Ta.Parts.FromHandle(hMirLjPart);
			if(pMirLinePart==NULL)
				continue;
			CDesignLjPartPara *pMirLjPart=pMirPlate->designInfo.partList.SetValue(hMirLjPart,*pLjPart);
			pMirLjPart->pCurLinePart=pMirLinePart;	//
			pMirLjPart->m_bAdjustLjEndPos=pLjPart->m_bAdjustLjEndPos;	//�Գ�ʱ�Ƿ�����˼����Ӷ˰ڷ�λ��
			pMirLjPart->m_nClassTypeId=pMirLjPart->pCurLinePart->GetClassTypeId();
			pMirLjPart->iFaceNo=pLjPart->iFaceNo;
			pMirLjPart->ber_space=pLjPart->ber_space;
			pMirLjPart->wing_space=pLjPart->wing_space;
			pMirLjPart->end_space=pLjPart->end_space;
			f3dPoint line_vec=pLjPart->pCurLinePart->End()-pLjPart->pCurLinePart->Start();
			f3dPoint mir_line_vec=pMirLjPart->pCurLinePart->End()-pMirLjPart->pCurLinePart->Start();
			if(pLjPart->start0_end1==0)
				line_vec*=-1.0;
			f3dPoint mir_vec_tm=GetMirVector(line_vec,msg);
			if(pLjPart->start0_end1>1)		//�м�����
				pMirLjPart->start0_end1=pLjPart->start0_end1;
			else if(mir_vec_tm*mir_line_vec>0)
				pMirLjPart->start0_end1=1;	//�ն�����
			else
				pMirLjPart->start0_end1=0;	//ʼ������
			if(pLjPart->start0_end1==0&&pMirLjPart->start0_end1==0)
				pMirLjPart->pCurLinePart->SetStartOdd(pLjPart->pCurLinePart->startOdd());
			else if(pLjPart->start0_end1==0&&pMirLjPart->start0_end1==1)
				pMirLjPart->pCurLinePart->SetEndOdd(pLjPart->pCurLinePart->startOdd());
			else if(pLjPart->start0_end1==1&&pMirLjPart->start0_end1==0)
				pMirLjPart->pCurLinePart->SetStartOdd(pLjPart->pCurLinePart->endOdd());
			else if(pLjPart->start0_end1==1&&pMirLjPart->start0_end1==1)
				pMirLjPart->pCurLinePart->SetEndOdd(pLjPart->pCurLinePart->endOdd());
			hashMirLjRodParas.SetValue(pLjPart->hPart,pMirLjPart);
		}
		//���ӹ��������Ϣ�Գ�
		for(pLjPart=pSrcPlate->designInfo.partList.GetFirst();pLjPart;pLjPart=pSrcPlate->designInfo.partList.GetNext())
		{
			CDesignLjPartPara *pMirLjPart=hashMirLjRodParas.GetValue(pLjPart->hPart);
			if(pMirLjPart==NULL)
			{
				console.DeletePart(pMirPlate->handle);
				if (UI::blDisableIntermediateMessageBox)
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.Log("Fail to meet symmetric requirements, can not finish symmetry!");
				else
					AfxMessageBox("Fail to meet symmetric requirements, can not finish symmetry!");
#else
					logerr.Log("��ʧ�˼�0x%X@�ְ�0x%X�Ĺ����Գ����Ӹ˼����޷���ɶԳ�!",pLjPart->hPart,pSrcPlate->handle);
				else
					AfxMessageBox(CXhChar200("��ʧ�˼�0x%X@�ְ�0x%X�Ĺ����Գ����Ӹ˼����޷���ɶԳ�!",pLjPart->hPart,pSrcPlate->handle));
#endif
				return NULL;
			}
			CLDSLinePart* pMirLinePart=pMirLjPart->pCurLinePart;
			MirLinePartOddDesignInfo(pLjPart->pCurLinePart,pMirLjPart->pCurLinePart,msg,
					pLjPart->start0_end1==0,pMirLjPart->start0_end1==0);
			long hParentGroupAngle=0;
			if (pLjPart->pCurLinePart->IsAngle()&&((CLDSLineAngle*)pLjPart->pCurLinePart)->group_father_jg_h>0x20)
				hParentGroupAngle=((CLDSLineAngle*)pLjPart->pCurLinePart)->group_father_jg_h;
			long hSafeBaseRod=pSrcPlate->m_hBaseRod;
			CLDSLineAngle * pBaseAngle=(CLDSLineAngle*)Ta.FromPartHandle(hSafeBaseRod,CLS_LINEANGLE);
			if (pBaseAngle!=NULL&&pBaseAngle->GetClassTypeId()==CLS_LINEANGLE&&pBaseAngle->hGroupAngle>0x20)
				hSafeBaseRod=pBaseAngle->hGroupAngle;
			if( hParentGroupAngle!=hSafeBaseRod&&hParentGroupAngle>0x20&&
				hashParentGroupAngles.GetValue(hParentGroupAngle)==NULL)
			{
				hashParentGroupAngles.SetValue(hParentGroupAngle,true);
				CLDSGroupLineAngle* pParentGroupAngle=(CLDSGroupLineAngle*)Ta.FromRodHandle(hParentGroupAngle);
				CLDSGroupLineAngle* pMirGroupAngle=pParentGroupAngle!=NULL?(CLDSGroupLineAngle*)pParentGroupAngle->GetMirRod(msg):NULL;
				if (pParentGroupAngle&&pMirGroupAngle)
				{
					MirLinePartOddDesignInfo(pParentGroupAngle,pMirGroupAngle,msg,
						pLjPart->start0_end1==0,pMirLjPart->start0_end1==0);
					MirLineAnglePosDesignInfo(pParentGroupAngle,pMirGroupAngle,msg,
					pLjPart->start0_end1==0,pMirLjPart->start0_end1==0,
					pMirLjPart->angle.cur_wing_x0_y1==pLjPart->angle.cur_wing_x0_y1);
				}
			}
			if(pMirLjPart->hPart==pMirPlate->designInfo.m_hBasePart)
			{	//��׼�˼�ֻ������ǰ����֫��λ�ò������Գ�	wjh-2012.3.19
				if(pLjPart->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
					pMirPlate->IsInsideAngle((CLDSLineAngle*)pMirLjPart->pCurLinePart,&pMirLjPart->angle.cur_wing_x0_y1);
				continue;
			}
			if(pMirLjPart->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{
				CLDSLineAngle *pSrcLineAngle=(CLDSLineAngle*)pLjPart->pCurLinePart;
				CLDSLineAngle *pMirLineAngle=(CLDSLineAngle*)pMirLjPart->pCurLinePart;
				f3dPoint verify_norm,wing_norm;
				if(pMirPlate->designInfo.norm.norm_style==1)	//�Ǹ�֫����
				{
					CLDSLineAngle* pDatumAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirPlate->designInfo.norm.hVicePart);
					if(pDatumAngle&&pMirPlate->designInfo.norm.norm_wing==0)	//X֫����
						wing_norm=pDatumAngle->get_norm_x_wing();
					else if(pDatumAngle&&pMirPlate->designInfo.norm.norm_wing==1)	//Y֫����
						wing_norm=pDatumAngle->get_norm_y_wing();
				}
				if(pMirLjPart->iFaceNo==1)
					verify_norm=pMirPlate->ucs.axis_z;
				else if(pMirLjPart->iFaceNo==2)
					verify_norm=pMirPlate->huoqufacenorm[0];
				else //if
					verify_norm=pMirPlate->huoqufacenorm[1];
				//�������жϱ�׼Ӧ���ǻ�׼�ǸֵĻ�׼֫����,�����Ǹְ巨�ߣ������ڶԳ�������˨����ʱ
				//���������жϴ���,�����������߽Ǹֶ�ͷƫ�Ƽ��нǴ��� wjh-2012.2.19
				if(wing_norm*pMirPlate->ucs.axis_z<0)
					verify_norm*=-1.0;
				double ddx=pMirLineAngle->get_norm_x_wing()*verify_norm;
				double ddy=pMirLineAngle->get_norm_y_wing()*verify_norm;
				pMirLjPart->angle.bEndLjJg=pLjPart->angle.bEndLjJg;
				pMirLjPart->angle.bTwoEdge=pLjPart->angle.bTwoEdge;
				if(fabs(ddx)>fabs(ddy))
					pMirLjPart->angle.cur_wing_x0_y1=0;
				else
					pMirLjPart->angle.cur_wing_x0_y1=1;
				
				//��ʼ����������ն���������� ����Ҫ����ʼ�ն�λ��
				if((pLjPart->start0_end1!=0&&pLjPart->start0_end1!=1)
					||!pLjPart->m_bAdjustLjEndPos)	//�ر�ָ���Ĳ���Ҫ����λ�õĸ˼� wht 10-02-26
					//||(pLjPart->angle.bTwoEdge&&pMirPlate->jdb_style!=6))	//���Ӱ��������Ĺ��ð帱��׼�Ǹֿ�ͨ��<��������>�����ж� wht 15-09-14
					continue;
				//����ͨ��pLjPart->angle.bTwoEdge�����ж��Ƿ���Ҫ�������߽Ǹ�λ��
				CLDSLineAngle* pRayAngle=(CLDSLineAngle*)pMirLjPart->pCurLinePart;
				bool blNeedUpdateTipPos=false;
				if (pMirLjPart->start0_end1==0&&pRayAngle->hStartDatumRod==pMirPlate->designInfo.m_hBasePart&&pRayAngle->IsLappedAnchorStart())
					blNeedUpdateTipPos=true;
				else if(pMirLjPart->start0_end1==1&&pRayAngle->hEndDatumRod==pMirPlate->designInfo.m_hBasePart&&pRayAngle->IsLappedAnchorEnd())
					blNeedUpdateTipPos=true;
				if (!blNeedUpdateTipPos)
					continue;	//ͨ����ǰ���Ӷ��Ƿ����ڻ�׼�˼�����Ϊ���¸˼�λ�õ��ж���������Ƚϸ�Ϊ�Ͻ� wjh-2019.12.27
				
				MirLineAnglePosDesignInfo((CLDSLineAngle*)pLjPart->pCurLinePart,
					(CLDSLineAngle*)pMirLjPart->pCurLinePart,msg,
					pLjPart->start0_end1==0,pMirLjPart->start0_end1==0,
					pMirLjPart->angle.cur_wing_x0_y1==pLjPart->angle.cur_wing_x0_y1);
				CMultiOffsetPos *pPosDesign;
				if((pMirLjPart->start0_end1==0&&pMirLjPart->pCurLinePart->IsStartPosLocked())||
					(pMirLjPart->start0_end1==1&&pMirLjPart->pCurLinePart->IsEndPosLocked()))
					continue;	//ʼ/�ն�λ���������ܵ���
				//������ԳƽǸֵĶ�λ��ʽ�����¼�������ͷ(���ǹ̶�����ͷ��Ӱ��)
				pMirLineAngle->CalPosition();
				if(pMirLjPart->start0_end1==0)
				{
					pPosDesign=&((CLDSLineAngle*)pMirLjPart->pCurLinePart)->desStartPos;
					pMirLineAngle->CalStartOddment();
				}
				else
				{
					pPosDesign=&((CLDSLineAngle*)pMirLjPart->pCurLinePart)->desEndPos;
					pMirLineAngle->CalEndOddment();
				}
				CLDSPart *pDatumPart=NULL;
				//���㶨λ��ʽΪָ����������ʱ����Ҫ����֫����ƫ����(���´�˫������߽Ǹ�)
				//if(pPosDesign->jgber_cal_style==0&&pPosDesign->datum_pos_style>0&&pPosDesign->datum_pos_style<=15)
				if(pPosDesign->jgber_cal_style!=1&&(pPosDesign->origin_pos_style==1||
					(pPosDesign->spatialOperationStyle>=0&&pPosDesign->spatialOperationStyle<15)))
				{
					long datum_jg_h=pPosDesign->datum_jg_h;
					if(datum_jg_h<0x20)
					{
						if(pMirLjPart->start0_end1==0)
							datum_jg_h=pMirLjPart->pCurLinePart->pStart->hFatherPart;
						else
							datum_jg_h=pMirLjPart->pCurLinePart->pEnd->hFatherPart;
					}
					pDatumPart=Ta.Parts.FromHandle(datum_jg_h);
					if(pDatumPart==NULL)
						continue;
					CLDSLinePart *pBaseLinePart=(CLDSLinePart*)Ta.Parts.FromHandle(pMirPlate->designInfo.m_hBasePart);
					if(pBaseLinePart&&pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
					{	//��׼�˼�Ϊ�ֹ�
						if(pDatumPart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
						{	//�������ǵ�ǰ����֫֫����ƫ���� wht 11-01-18
							if(pMirLjPart->angle.cur_wing_x0_y1==0)
							{
								pPosDesign->wing_x_offset.gStyle=4;
								pPosDesign->wing_x_offset.offsetDist=-0.5*pMirPlate->GetThick();
							}
							else
							{
								pPosDesign->wing_y_offset.gStyle=4;
								pPosDesign->wing_y_offset.offsetDist=-0.5*pMirPlate->GetThick();
							}
						}
					}
					//��׼�˼�Ϊ�Ǹ� ���㶨λ��ʽΪ������ͶӰʱ ���õ���֫����ƫ����(���´�����) wht 09-05-17
					//��׼�Ǹ�Ϊ��ϽǸ�ʱ���߲Ŷ�λ��ʽΪ��������ͶӰ���ʴ˴���pPosDesign->datum_pos_style!=1������ wht 11-01-17
					else if(toupper(pMirLjPart->pCurLinePart->layer(1))!='Z')	//������(�ų����´�����) wht 11-01-17
						//(pPosDesign->datum_pos_style!=1)
					{
						//�ж�{�ڵ����Ǹְڷ�λ��ʱ}�Ƿ��ǽǸ�֫�������:
						//1.�Ǹֵ�ǰ������ͷ�Ƿ���ڸ�����ײ���������ͷ
						//2.�����ϽǸֵ��ӽǸ����Ƿ�����뵱ǰ�Ǹ���/������ʽһ�µ��ӽǸ�
						//3.�ӽǸֵķǵ�ǰ֫���߷����뵱ǰ�Ǹ�(ָ����ϽǸֵ�)���췽�������ͬ
						if(pDatumPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
						{	
							//1.���㵱ǰ�Ǹֵ���ײ����ͷ���뵱ǰ����ͷ���бȽ�
							double old_oddment=0;	//��¼�Ǹֵ�����ͷ
							DESIGNODDMENT desOddment;
							BOOL bExceedCollideOddment=FALSE;//�Ǹ�����ͷ�Ƿ���ڸ�����ײ�������������ͷ
							if(pMirLjPart->start0_end1==0)
							{	//ʼ������
								desOddment=pMirLineAngle->desStartOdd;
								old_oddment=pMirLineAngle->startOdd();
								//������ײ�����һ������ͷֵ
								pMirLineAngle->desStartOdd.m_iOddCalStyle=0;
								pMirLineAngle->desStartOdd.m_hRefPart1=pDatumPart->handle;
								pMirLineAngle->CalStartOddment();
								if(fabs(pMirLineAngle->startOdd())>fabs(old_oddment))
									bExceedCollideOddment=TRUE;
								//�ָ�ԭ��������ͷֵ�Լ�����ͷ���㷽ʽ
								pMirLineAngle->SetStartOdd(old_oddment);
								pMirLineAngle->desStartOdd=desOddment;
							}
							else if(pMirLjPart->start0_end1==1)
							{	//�ն�����
								desOddment=pMirLineAngle->desEndOdd;
								old_oddment=pMirLineAngle->endOdd();
								//������ײ�����һ������ͷֵ
								pMirLineAngle->desEndOdd.m_iOddCalStyle=0;
								pMirLineAngle->desEndOdd.m_hRefPart1=pDatumPart->handle;
								pMirLineAngle->CalEndOddment();
								if(fabs(pMirLineAngle->endOdd())>fabs(old_oddment))
									bExceedCollideOddment=TRUE;
								//�ָ�ԭ��������ͷֵ�Լ�����ͷ���㷽ʽ
								pMirLineAngle->SetEndOdd(old_oddment);
								pMirLineAngle->desEndOdd=desOddment;
							}
							
							BOOL bCalcWingThick=FALSE;	//�Ƿ���֫��
							if(bExceedCollideOddment)
							{	//��ǰ�Ǹ�����ͷ���ڸ�����ײ�������������ͷʱ����Ҫ���������ж�
								f3dPoint len_vec;
								//���㵱ǰ�Ǹֵ����췽��
								if(pMirLjPart->start0_end1==0)
									len_vec=pMirLineAngle->Start()-pMirLineAngle->End();
								else if(pMirLjPart->start0_end1==1)
									len_vec=pMirLineAngle->End()-pMirLineAngle->Start();
								normalize(len_vec);
								CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pDatumPart;
								for(int j=0;j<4;j++)
								{
									CLDSLineAngle *pSonAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pGroupAngle->son_jg_h[j],CLS_LINEANGLE);
									if(pSonAngle==NULL)
										continue;
									if(pSonAngle->m_bVirtualPart)
										continue;	//���ӽǸ�Ϊ�鹹�������
									double ddx1=pSonAngle->get_norm_x_wing()*verify_norm;
									double ddy1=pSonAngle->get_norm_y_wing()*verify_norm;
									if(fabs(ddx1)>fabs(ddy1))
									{	//��ϽǸ��ӽǸֵ�X֫Ϊ��ǰ֫
										//3.�ж��ӽǸַǵ�ǰ֫���߷����Ƿ��뵱ǰ�Ǹ����췽��һ��
										if(pSonAngle->get_norm_y_wing()*len_vec<0)
											continue;
										//2.�ж��ӽǸ���/������ʽ�Ƿ��뵱ǰ�Ǹ�һ��
										if(pMirLjPart->angle.cur_wing_x0_y1==0)
										{
											if(pMirLineAngle->get_norm_x_wing()*pSonAngle->get_norm_x_wing()>0)
												bCalcWingThick=TRUE;
										}
										else if(pMirLjPart->angle.cur_wing_x0_y1==1)
										{
											if(pMirLineAngle->get_norm_y_wing()*pSonAngle->get_norm_x_wing()>0)
												bCalcWingThick=TRUE;
										}
									}
									else //if(fabs(ddx1)<fabs(ddy1))
									{	//��ϽǸ��ӽǸֵ�Y֫Ϊ��ǰ֫ 
										//3.�ж��ӽǸַǵ�ǰ֫���߷����Ƿ��뵱ǰ�Ǹ����췽��һ��
										if(pSonAngle->get_norm_x_wing()*len_vec<0)
											continue;
										//2.�ж��ӽǸ���/������ʽ�Ƿ��뵱ǰ�Ǹ�һ��
										if(pMirLjPart->angle.cur_wing_x0_y1==0)
										{
											if(pMirLineAngle->get_norm_x_wing()*pSonAngle->get_norm_y_wing()>0)
												bCalcWingThick=TRUE;	
										}
										else if(pMirLjPart->angle.cur_wing_x0_y1==1&&ddy1*ddy<0)
										{
											if(pMirLineAngle->get_norm_y_wing()*pSonAngle->get_norm_y_wing()>0)
												bCalcWingThick=TRUE;	
										}
									}
									BOOL hasShareBolt=FALSE;
									for(CLsRef *pLsRef=pMirPlate->GetFirstLsRef();pLsRef;pLsRef=pMirPlate->GetNextLsRef())
									{
										if( pMirLineAngle->FindLsByHandle((*pLsRef)->handle)==NULL ||
											pSonAngle->FindLsByHandle((*pLsRef)->handle)==NULL)
											continue;
										else
										{
											hasShareBolt=TRUE;
											break;
										}
									}
									bCalcWingThick&=hasShareBolt;
									if(bCalcWingThick)
										break;
								}
							}
							//�����Ǹְڷ�λ��
							if(pMirLjPart->angle.cur_wing_x0_y1==0)
							{
								pPosDesign->wing_x_offset.gStyle=4;
								if(bCalcWingThick)
									pPosDesign->wing_x_offset.offsetDist=-0.5*pMirPlate->GetThick()-pDatumPart->GetThick();
								else 
									pPosDesign->wing_x_offset.offsetDist=-0.5*pMirPlate->GetThick();
							}
							else //if(pMirLjPart->angle.cur_wing_x0_y1==1)
							{
								pPosDesign->wing_y_offset.gStyle=4;
								if(bCalcWingThick)
									pPosDesign->wing_y_offset.offsetDist=-0.5*pMirPlate->GetThick()-pDatumPart->GetThick();
								else 
									pPosDesign->wing_y_offset.offsetDist=-0.5*pMirPlate->GetThick();
							}
						}
						else  if(pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
						{
							//if(pMirPlate->jdb_style==6&&pPosDesign->datum_pos_style!=15)
							//	continue;	//���ð�ʱֻ�����ָ���������ĽǸֵ�λ�� wht 09-06-16
							if(pMirLjPart->angle.cur_wing_x0_y1==0)
							{
								if(pMirPlate->jdb_style==6)	//���ð�
									pPosDesign->wing_y_offset.gStyle=0;
								pPosDesign->wing_x_offset.gStyle=4;
								if(ddx>0)	//����
								{
									if((pMirLjPart->angle.bEndLjJg||pMirPlate->GetThick()==0
										||pSrcPlate->designInfo.m_bWeldToDatumJg)&&pDatumPart) //�ڵ�庸���ڻ�׼�Ǹ��� wht 10-06-07
									{	
										if(((CLDSLineAngle*)pDatumPart)->group_father_jg_h>0X20) //��׼�Ǹ�Ϊ��ϽǸ��ӽǸ� wht 11-01-17
											pPosDesign->wing_x_offset.offsetDist=-(0.5*pMirPlate->GetThick()+pDatumPart->GetThick());
										else //��׼�Ǹ�Ϊ��ͨ���Ǹ�
											pPosDesign->wing_x_offset.offsetDist=-pDatumPart->GetThick();
									}
									else
										pPosDesign->wing_x_offset.offsetDist=0;
								}
								else
								{
									if(pSrcPlate->designInfo.m_bWeldToDatumJg)	//�ڵ�庸���ڻ�׼�Ǹ��� wht 10-06-07
										pPosDesign->wing_x_offset.offsetDist=0;
									else
										pPosDesign->wing_x_offset.offsetDist=-pMirPlate->GetThick();
								}
							}
							else
							{
								if(pMirPlate->jdb_style==6)	//���ð�
									pPosDesign->wing_x_offset.gStyle=0;
								pPosDesign->wing_y_offset.gStyle=4;
								if(ddy>0)	//����
								{
									if((pMirLjPart->angle.bEndLjJg||pMirPlate->GetThick()==0
										||pSrcPlate->designInfo.m_bWeldToDatumJg)&&pDatumPart)//�ڵ�庸���ڻ�׼�Ǹ��� wht 10-06-07
									{
										if(((CLDSLineAngle*)pDatumPart)->group_father_jg_h>0X20) //��׼�Ǹ�Ϊ��ϽǸ��ӽǸ� wht 11-01-17
											pPosDesign->wing_y_offset.offsetDist=-(0.5*pMirPlate->GetThick()+pDatumPart->GetThick());
										else //��׼�Ǹ�Ϊ��ͨ���Ǹ�
											pPosDesign->wing_y_offset.offsetDist=-pDatumPart->GetThick();
									}
									else
										pPosDesign->wing_y_offset.offsetDist=0;
								}
								else
								{
									if(pSrcPlate->designInfo.m_bWeldToDatumJg)	//�ڵ�庸���ڻ�׼�Ǹ��� wht 10-06-07
										pPosDesign->wing_y_offset.offsetDist=0;
									else
										pPosDesign->wing_y_offset.offsetDist=-pMirPlate->GetThick();
								}
							}
						}
						if(pMirLjPart->angle.bEndLjJg)
						{	//�������������߽Ǹ�����˨�ķ���ƫ�����Լ�ͨ�� wht 11-01-17
							pMirLineAngle->ClearFlag();
							pMirLineAngle->CalPosition();	
							for(pLsRef=pMirPlate->GetFirstLsRef();pLsRef;pLsRef=pMirPlate->GetNextLsRef())
							{
								CLDSBolt *pBolt=pLsRef->GetLsPtr();
								if(!pMirLineAngle->FindLsByHandle(pBolt->handle))
									continue;	//�����ڵ�ǰ���߸˼�
								CLDSLineAngle *pDatumJg=(CLDSLineAngle*)pDatumPart;
								if(pDatumPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
								{	//��ϽǸ�
									pDatumJg=(CLDSLineAngle*)Ta.Parts.FromHandle(((CLDSGroupLineAngle*)pDatumJg)->m_hDatumSonAngle,CLS_LINEANGLE);
									if(pDatumJg==NULL)
										pDatumJg=(CLDSLineAngle*)pDatumPart;
								}
								if(pDatumJg->FindLsByHandle(pBolt->handle))
								{	//1.�����ӹ�����˨
									if(pLsRef->feature==0)
									{	//��һ����Ƹ���˨����
										pBolt->EmptyL0DesignPara();		//ͨ��
										pBolt->AddL0Thick(pDatumJg->handle,TRUE,TRUE);
										pBolt->AddL0Thick(pMirPlate->handle,TRUE,TRUE);
										pBolt->des_base_pos.norm_offset.EmptyThick(); //����ƫ����
										pLsRef->feature=1;	//����˨���ö�Ӧ����˨��������չ�
									}
									pBolt->AddL0Thick(pMirLineAngle->handle,TRUE,TRUE);
									if(pBolt->des_base_pos.hPart==pDatumJg->handle)	//1.1��λ�Ǹ�Ϊ��׼�Ǹ�
										pBolt->des_base_pos.norm_offset.AddThick(-pDatumJg->handle,TRUE,TRUE);
									if(pBolt->des_base_pos.hPart==pMirLineAngle->handle)
									{	//1.2��λ�Ǹ�Ϊ��ǰ�Ǹ�
										if((pMirLjPart->angle.cur_wing_x0_y1==0&&ddx>0)
											||(pMirLjPart->angle.cur_wing_x0_y1==1&&ddy>0))	//����
										{
											if(pBolt->des_work_norm.direction==1)
											{	//����
												pBolt->des_base_pos.norm_offset.AddThick(-pDatumJg->handle,TRUE,TRUE);
												pBolt->des_base_pos.norm_offset.AddThick(-pMirPlate->handle,TRUE,TRUE);
											}
											else //���� 
												pBolt->des_base_pos.norm_offset.AddThick(-pMirLineAngle->handle,TRUE,TRUE);
										}
										else 
										{	//����
											if(pBolt->des_work_norm.direction==0)//����
												pBolt->des_base_pos.norm_offset.AddThick(-pMirLineAngle->handle,TRUE,TRUE);
											else
											{	//����
												pBolt->des_base_pos.norm_offset.AddThick(-pDatumJg->handle,TRUE,TRUE);
												pBolt->des_base_pos.norm_offset.AddThick(-pMirPlate->handle,TRUE,TRUE);
											}
										}
									}
									//1.3��λ�Ǹ�Ϊ����һ��(����)���߽Ǹ�
									else if((pMirLjPart->angle.cur_wing_x0_y1==0&&ddx>0)
										||(pMirLjPart->angle.cur_wing_x0_y1==1&&ddy>0))	//����
									{
										if(pBolt->des_work_norm.direction==0)	//����
											pBolt->des_base_pos.norm_offset.AddThick(-pMirLineAngle->handle,TRUE);
									}
									else //����
									{
										if(pBolt->des_work_norm.direction==1)	//����
											pBolt->des_base_pos.norm_offset.AddThick(-pMirLineAngle->handle,TRUE);
									}

									pBolt->correct_pos();
									pBolt->CalGuigeAuto();
									pBolt->Create3dSolidModel();
									g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
									continue; //����
								}
								if(pBolt->des_base_pos.hPart!=pMirLineAngle->handle)//��λ�Ǹ�Ϊ��ǰ�Ǹ�
									continue;
								//2.�����ӷǹ�����˨ 
								pBolt->EmptyL0DesignPara();		//ͨ��
								pBolt->des_base_pos.norm_offset.EmptyThick(); //����ƫ����
								pBolt->AddL0Thick(pMirLineAngle->handle,TRUE,TRUE);
								pBolt->AddL0Thick(pMirPlate->handle,TRUE,TRUE);
								PARTSET actset;
								actset.append(pMirLineAngle);
								actset.append(pMirPlate);
								pBolt->des_base_pos.norm_offset.key_str=CLDSBolt::EmendZOffsetStr(pBolt,actset);
								if(pBolt->DianQuan.N>0&&pBolt->DianQuan.thick>0&&!pBolt->IsNeedGasket(actset))
								{	//�ж���˨�Ƿ���Ҫ��Ȧ
									pBolt->DianQuan.N=0;
									pBolt->DianQuan.thick=0;
								}
								pBolt->correct_pos();
								pBolt->CalGuigeAuto();
								pBolt->Create3dSolidModel();
								g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
							}
						}
					}
				}
				pMirLineAngle->ClearFlag();
				pMirLineAngle->CalPosition();
				//����Ǹ�λ�ú����¼����н���֫��Ϣ
				if(pDatumPart&&pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
					pMirLineAngle->CalCutAngleInfo((CLDSLineAngle*)pDatumPart,NULL,(pMirLjPart->start0_end1==0),NULL);
				else if(pDatumPart&&pDatumPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
				{
					CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pDatumPart;
					for(int j=0;j<4;j++)
					{
						CLDSLineAngle *pSonAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pGroupAngle->son_jg_h[j],CLS_LINEANGLE);
						if(pSonAngle==NULL)
							continue;
						if(pSonAngle->m_bVirtualPart)
							continue;	//���ӽǸ�Ϊ�鹹�������
						DESIGNODDMENT designStartOdd=pMirLineAngle->desStartOdd;
						DESIGNODDMENT designEndOdd=pMirLineAngle->desEndOdd;
						pMirLineAngle->CalCutAngleInfo(pSonAngle,NULL,(pMirLjPart->start0_end1==0),NULL);
						pMirLineAngle->desStartOdd=designStartOdd;
						pMirLineAngle->desEndOdd=designEndOdd;
					}	
				}
				pMirLineAngle->SetModified();
				if (UI::blEnableIntermediateUpdateUI)
					pMirLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				//�˴���Ҫ���Գ����ɵİ����ظ˼���ʾ�����������û����ܻ���Ϊδ������
				//Щ�˼���ѡʱ����ѡ�񣬵������������жϲ�׼ȷ�����ֲ����׷���ԭ�����
				//g_pSolidDraw->NewSolidPart(pMirLineAngle->GetSolidPartObject());
			}
			else if(pMirLjPart->pCurLinePart&&pMirLjPart->pCurLinePart->GetClassTypeId()==CLS_LINETUBE)
			{
				CLDSLineTube *pSrcLineTube=(CLDSLineTube*)pLjPart->pCurLinePart;
				CLDSLineTube *pMirLineTube=(CLDSLineTube*)pMirLjPart->pCurLinePart;
				CTubeEndPosPara *pPosDesign=NULL;
				if((pMirLjPart->start0_end1==0&&pMirLjPart->pCurLinePart->IsStartPosLocked())||
					(pMirLjPart->start0_end1==1&&pMirLjPart->pCurLinePart->IsEndPosLocked()))
					continue;	//ʼ/�ն�λ���������ܵ���
				//��ʼ����������ն���������� ����Ҫ����ʼ�ն�λ��
				if(pLjPart->start0_end1!=0&&pLjPart->start0_end1!=1)
					continue;
				TUBEJOINT *pMirTubeJoint=NULL;
				//�����ԳƸֹܵ�λ��
				if(pMirLjPart->start0_end1==0)
				{
					pPosDesign=&pMirLineTube->desStartPos;
					pMirTubeJoint=&pMirLineTube->m_tJointStart;
				}
				else
				{
					pPosDesign=&pMirLineTube->desEndPos;
					pMirTubeJoint=&pMirLineTube->m_tJointEnd;
				}
				if(pPosDesign&&(pPosDesign->endPosType==0||pPosDesign->endPosType==1))
				{
					if(pLjPart->start0_end1==0)
					{
						pPosDesign->fNormOffset=pSrcLineTube->desStartPos.fNormOffset;
						pPosDesign->fEccentricDist=pSrcLineTube->desStartPos.fEccentricDist;
					}
					else
					{
						pPosDesign->fNormOffset=pSrcLineTube->desEndPos.fNormOffset;
						pPosDesign->fEccentricDist=pSrcLineTube->desEndPos.fEccentricDist;
					}
				}
				pMirLineTube->CalPosition();
				pMirLineTube->SetModified();
				pMirLineTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				//�Գ����ɸֹ��ϵĲ��
				TUBEJOINT *pSrcTubeJoint=NULL;
				if(pLjPart->start0_end1==0)
					pSrcTubeJoint=&pSrcLineTube->m_tJointStart;
				else 
					pSrcTubeJoint=&pSrcLineTube->m_tJointEnd;
				if(pSrcTubeJoint&&pSrcTubeJoint->hLinkObj>0)	
				{	//Դ�ֹܶ˴��ڲ��
					CLDSParamPlate *pSrcParamPlate=NULL, *pMirParamPlate=NULL;
					pSrcParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pSrcTubeJoint->hLinkObj,CLS_PARAMPLATE);
					if(pMirTubeJoint&&pMirTubeJoint->hLinkObj>0)	//�ԳƸֹ���Ӧ�˴��ڲ��
						pMirParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pMirTubeJoint->hLinkObj,CLS_PARAMPLATE);
					if(pSrcParamPlate&&pMirParamPlate&&pSrcParamPlate->m_iParamType==pMirParamPlate->m_iParamType
						&&(pSrcParamPlate->m_iParamType==TYPE_XEND||pSrcParamPlate->m_iParamType==TYPE_UEND||
						pSrcParamPlate->m_iParamType==TYPE_ROLLEND))
					{	//����Ӧ����˨���뵽�����
						CLsRef *pLsRef=NULL;
						for(pLsRef=pSrcParamPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcParamPlate->GetNextLsRef())
						{
							CLDSBolt *pMirBolt=(CLDSBolt*)(*pLsRef)->GetMirPart(msg);
							if(pMirBolt)
								pMirParamPlate->AppendLsRef(pMirBolt->GetLsRef());
						}
					}
					else if(pSrcParamPlate&&(pSrcParamPlate->m_iParamType==TYPE_XEND||pSrcParamPlate->m_iParamType==TYPE_UEND||
						pSrcParamPlate->m_iParamType==TYPE_ROLLEND))
						MirTaAtom(pSrcParamPlate,msg);
				}
			}
			else if(pMirLjPart->pCurLinePart&&pMirLjPart->pCurLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
				;
			else if(pMirLjPart->pCurLinePart&&pMirLjPart->pCurLinePart->GetClassTypeId()==CLS_SPHERE)
				;
			else
			{
				//console.DispPartSet.DeleteNode(pMirPlate->handle);	//������console.DeletePart��
				console.DeletePart(pMirPlate->handle);
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("fail to meet symmetric rod, can not finish symmetry!");
#else
				AfxMessageBox(CXhChar200("δ�ҵ��˼�0x%X���ʵ�ƥ��ԳƸ˼����޷���ɶԳ�!",pLjPart->hPart));
#endif
				return NULL;
			}
		}
		//��������ͷ �˴�Ӧ�ü���ԳƸְ����߸˼�������ͷ wht 10-01-27
		for(pLjPart=pMirPlate->designInfo.partList.GetFirst();pLjPart;pLjPart=pMirPlate->designInfo.partList.GetNext())
		{
			CSuperSmartPtr<CLDSPart>pSrcPart=Ta.Parts.FromHandle(pLjPart->hPart);
			if(pSrcPart.IsHasPtr()&&pSrcPart->IsLinePart())
			{
				if(pLjPart->start0_end1==0&&pSrcPart.LinePartPointer()->desStartOdd.m_iOddCalStyle==1)
					pSrcPart.LinePartPointer()->CalStartOddment();
				else if(pLjPart->start0_end1==1&&pSrcPart.LinePartPointer()->desEndOdd.m_iOddCalStyle==1)
					pSrcPart.LinePartPointer()->CalEndOddment();
			}
		}
		if(pMirPlate->jdb_style==5)	//������׼����λ�ò����İ�
			pMirPlate->DesignSetupUcs();
		//pMirPlate->DesignPlate();  //�˴�������������Ʒ����޸Ĺ��İ�����α��治�� 
		if( pBaseLinePart&&pBaseLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE&&
			pMirBaseLinePart&&pMirBaseLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//��׼�Ǹ�Ϊ��ϽǸ���1�ŽǸ��޶Գƹ�ϵʱ,��Ҫ������Ƹְ�(��:T����ϽǸֺᵣ���¸���ְ�ǰ��Գ�) wht 16-08-09
			long mir_handle=0;
			MirPartHandle(pSrcPlate->BelongModel(),&mir_handle,((CLDSGroupLineAngle*)pBaseLinePart)->m_hDatumSonAngle,msg,&pSrcPlate->ucs.origin);
			if(!bDatumJgStyleDiff&&mir_handle!=((CLDSGroupLineAngle*)pMirBaseLinePart)->m_hDatumSonAngle)
				bDatumJgStyleDiff=TRUE;	
		}
		if(bDatumJgStyleDiff) //��׼�Ǹ���������ʽ��һ��Ӧ������Ƹְ����� wht 11-01-17
			pMirPlate->DesignPlate();
		if(UI::blEnableIntermediateUpdateUI)
		{
			pMirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			if(pMirPlate->vertex_list.GetNodeNum()>2)
				g_pSolidDraw->NewSolidPart(pMirPlate->GetSolidPartObject());
			g_pSolidDraw->Draw();
		}
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
	RELATIVE_OBJECT* pRelaObj=pSrcPlate->relativeObjs.Add(pMirPlate->handle);
	pRelaObj->mirInfo.rotate_angle=msg.rotate_angle;
	pRelaObj->mirInfo.axis_flag=msg.axis_flag;
	return pMirPlate;
}

BOOL MirPlate(CLDSPlate* pSrcPlate, MIRMSG mirmsg, BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//�Ƿ�ɹ��Գ��������й���
	RELATIVE_OBJECT *pRelaObj=NULL;
	int i,flagArr[4]={1,2,4,16};
	CLDSPlate* srcplate_arr[4]={NULL};
	srcplate_arr[0]=pSrcPlate;
	//mirmsg.axis_flag=0x10;
	//mirmsg.mir_norm.Set(-1,-1);
	//return CreateSingleMirPlate(pSrcPlate,mirmsg,sErrorInfo)!=NULL;
	bool clockwiseArr[4]={!pSrcPlate->bClockwiseCalProfile,!pSrcPlate->bClockwiseCalProfile,
						   pSrcPlate->bClockwiseCalProfile,!pSrcPlate->bClockwiseCalProfile};
	for(i=0;i<4;i++)
	{
		if((mirmsg.axis_flag&flagArr[i])==0)
			continue;
		MIRMSG msg=mirmsg.GetSubMirItem(i+1);
		srcplate_arr[i+1]=CreateSingleMirPlate(pSrcPlate,msg,bIncAttachInfo,sErrorInfo);
		if(srcplate_arr[i+1]==NULL)
			return FALSE;
		srcplate_arr[i+1]->bClockwiseCalProfile=clockwiseArr[i];
	}
	int array_num=mirmsg.array_num;
	if((mirmsg.axis_flag&8)==0)	//����ת�Գ�
		array_num=0;
	else
		mirmsg.axis_flag=8;
	short init_angle=mirmsg.rotate_angle;
	for(i=0;i<4;i++)
	{
		if(srcplate_arr[i]==NULL)
			continue;
		pSrcPlate = srcplate_arr[i];
		for(int j=0;j<array_num;j++)
		{	//���ж����ת�Գ�ʱ��׼����Ϊͬһ��������ֻ��Ҫ������ת�Ƕȼ���
			if(pSrcPlate==NULL)
				continue;
			//������ת�Ƕ�
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSPlate *pMirPlate=CreateSingleMirPlate(pSrcPlate,mirmsg,bIncAttachInfo,sErrorInfo);
			if(pMirPlate)
			{	
				pMirPlate->bClockwiseCalProfile=pSrcPlate->bClockwiseCalProfile;
				if(pMirPlate->GetThick()<=0)
				{
					//console.DispPartSet.DeleteNode(pMirPlate->handle);
					console.DeletePart(pMirPlate->handle);
				}
				BOOL bInList=FALSE;
				for(pRelaObj=srcplate_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcplate_arr[0]->relativeObjs.GetNext())
				{	//�����ظ���ӹ�������
					if(pRelaObj->hObj==pMirPlate->handle)
					{
						bInList=TRUE;
						break;
					}
				}
				if(bInList)
					continue;
				pRelaObj=srcplate_arr[0]->relativeObjs.Add(pMirPlate->handle);
				pRelaObj->mirInfo.rotate_angle=init_angle*(j+1);
				pRelaObj->mirInfo.axis_flag=mirmsg.axis_flag;
				if(i>0)
					pRelaObj->mirInfo.axis_flag+=flagArr[i-1];
			}
			else
				bSucceed=FALSE;
		}
	}
	for(i=0;i<3;i++)
	{
		if(srcplate_arr[i+1]&&srcplate_arr[i+1]->GetThick()<=0)
		{
			//console.DispPartSet.DeleteNode(srcplate_arr[i+1]->handle);
			console.DeletePart(srcplate_arr[i+1]->handle);
		}
	}
	//���öԳ����ɵĶ���˴˹���
	if(srcplate_arr[0]&&srcplate_arr[0]->GetThick()>0)
		SetRelativeObj(srcplate_arr[0]);
	return bSucceed;
}
CLDSParamPlate* CreateSingleMirParamPlate(CLDSParamPlate* pSrcParamPlate, MIRMSG msg, BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	CLDSParamPlate *pMirPlate=NULL;
	CMirOperObject::MIRROR* pMirror=CMirOperObject::AddOrGetMirror(pSrcParamPlate->handle,pSrcParamPlate);
	CMirOperObject::MIRROR::MIROBJ* pMirObj=pMirror->GetMirObj(msg);
	pMirPlate=(CLDSParamPlate*)(pMirObj!=NULL?pMirObj->pMirObj:pSrcParamPlate->GetMirPart(msg));
	if(pMirPlate==NULL)
	{
		pMirPlate = (CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pMirPlate->CopyProperty(pSrcParamPlate);
		pMirPlate->ucs.origin=GetMirPos(pSrcParamPlate->ucs.origin,msg);
		pMirPlate->ucs.axis_x=GetMirVector(pSrcParamPlate->ucs.axis_x,msg);
		pMirPlate->ucs.axis_y=GetMirVector(pSrcParamPlate->ucs.axis_y,msg);
		//if((msg.axis_flag&4)==0&&(msg.axis_flag&8)==0)
		//	pMirPlate->ucs.axis_x*=-1.0;	//������������ͨ�ְ岻ͬ�����������н�����Y�ᷭת wjh-2015.9.21
		pMirPlate->ucs.axis_z=GetMirVector(pSrcParamPlate->ucs.axis_z,msg);
		AddMirObjToSrcObjRelaObjList(pSrcParamPlate,pMirPlate,msg);	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
		if (pMirObj==NULL)
		{	//�¶ԳƲ����������
			CMirOperObject::MIRROR::MIROBJ mirobj;
			mirobj.ciMirFlag=msg.axis_flag;
			mirobj.siRotDegAng=msg.rotate_angle;
			mirobj.dwStateFlag=0;
			mirobj.pMirObj=pMirPlate;
			pMirObj=pMirror->xarrMirObjs.Append(mirobj);
		}
		//��ǰ��Ϊ�˷�ֹһ��װ�䷽λ��ͬ�ְ��ڳ���δ��������Ϣ�Գƺ󣬽��ж��θ�����Ϣ�Գ�ʱ���ҶԳƸְ���� wjh-2015.9.21
		PROFILE_VERTEX *pPrevVertex=pSrcParamPlate->vertex_list.GetTail();
		for(PROFILE_VERTEX *pVertex=pSrcParamPlate->vertex_list.GetFirst();pVertex;pVertex=pSrcParamPlate->vertex_list.GetNext())
		{	//��� Ϊ����ڻ�׼����λ�ò����İ�ֱ�ӰѶ����б��ƹ�ȥ����
			//if(msg.axis_flag&4||msg.axis_flag&8)	//Z��Գƻ���ת�Գ�
			//{	�˴��Գƶ�����ȫ��Ϊ���µڶ��ν��к���Ϣ�Գ�ʱ���ܹ���ȷ�ҵ��Գư壬������Ե�����ѥ����Ҳ���ȷ wjh-2015.9.21
				PROFILE_VERTEX vertex=*pVertex;
				if(vertex.type==3)	//��Բ��
				{
					vertex.column_norm=GetMirVector(vertex.column_norm,msg);
					vertex.center=GetMirPos(vertex.center,msg);
				}
				PROFILE_VERTEX* pTmVertex=pMirPlate->vertex_list.append(vertex);
			/*}
			else
			{
				PROFILE_VERTEX vertex=*pPrevVertex;
				vertex.vertex.Set(-pVertex->vertex.x,pVertex->vertex.y,pVertex->vertex.z);
				vertex.vertex.feature=pVertex->vertex.feature;
				//���ڣ��ᡢ����Գ�ʱԲ��ʰȡ������ҲӦ������Ӧ�任
				if(vertex.type==2)	//ָ��Բ���뾶��ʰȡ��
					vertex.center.x*=-1;
				else if(vertex.type==3)	//��Բ��
				{
					vertex.column_norm=GetMirVector(pPrevVertex->column_norm,msg);
					vertex.center=GetMirPos(pPrevVertex->center,msg);
				}
				vertex.m_cPosDesignType=pVertex->m_cPosDesignType;
				vertex.m_hRod=pVertex->m_hRod;
				PROFILE_VERTEX* pTmVertex=pMirPlate->vertex_list.insert(vertex,0);
				pPrevVertex=pVertex;
			}*/
		}
	}
	else
	{
		if (pMirObj==NULL)
		{	//�ԳƲ���ǰ�Ѵ��ڶԳ���˨�������������˨�����Ϣ�������й������� wjh-2020.1.14
			CMirOperObject::MIRROR::MIROBJ mirobj;
			mirobj.ciMirFlag=msg.axis_flag;
			mirobj.siRotDegAng=msg.rotate_angle;
			mirobj.dwStateFlag=CMirOperObject::MIRBOLT_ALL;
			mirobj.pMirObj=pMirPlate;
			pMirObj=pMirror->xarrMirObjs.Append(mirobj);
		}
		if (msg.axis_flag&8)
		{	//��ת�Գ�ʱ����֤�Գư����Ʋ�����ԭ��(pSrcParamPlate)��Ʋ���һ��
			pMirPlate->CopyProperty(pSrcParamPlate);
		}
	}
	//��һ�ζԳ�Ӧ�Գ�ͼ����Ϣ������Ĭ��ͼ����ΪSPQ��֮ǰ���ڳ��Ȳ�������GetMirPart()������
	//����UnifiedCfgword()����ĺŽ���������Ϊ255,�ڶ��ζԳ�ͼ����ȷ����ĺŽ���Ϊ2:A-H wht 15-07-02
	CalMirAtomLayer(pSrcParamPlate->layer(),pMirPlate->layer(),msg);
	if(!bIncAttachInfo)
		return pMirPlate;
	CLDSLineTube* pTube=(CLDSLineTube*)Ta.Parts.FromHandle(pSrcParamPlate->m_hPartWeldParent,CLS_LINETUBE);
	f3dPoint pickPos,*pPickPoint=NULL;	//�ж���ʼ�����ӻ����ն�����
	CLDSNode *pBaseNode=pSrcParamPlate->GetBaseNode();
	if(pBaseNode)
	{
		pickPos=pBaseNode->Position(false);
		pPickPoint=&pickPos;
	}
	long hMirPartWeldParent=pMirPlate->m_hPartWeldParent;
	MirPartHandle(pSrcParamPlate->BelongModel(),&hMirPartWeldParent,pSrcParamPlate->m_hPartWeldParent,msg,pPickPoint);
	pMirPlate->m_hPartWeldParent=hMirPartWeldParent;
	if(pSrcParamPlate->m_iParamType==TYPE_ELBOWPLATE)
	{
		long K,L;
		if(pSrcParamPlate->GetDesignItemValue('K',&K))
		{
			CLDSLineTube *pDatumTube=NULL,*pMirDatumTube=NULL;
			CSuperSmartPtr<CLDSPart> pBottomPlate=NULL,pMirBottomPlate;
			pDatumTube=(CLDSLineTube*)Ta.Parts.FromHandle(abs(K),CLS_LINETUBE);
			if(pSrcParamPlate->GetDesignItemValue('L',&L))//��û�׼�װ�
				pBottomPlate=Ta.Parts.FromHandle(L,CLS_PLATE,CLS_PARAMPLATE);
			if(pDatumTube&&pBottomPlate.IsHasPtr())
			{
				pMirDatumTube = (CLDSLineTube*)pDatumTube->GetMirPart(msg);
				pMirBottomPlate = pBottomPlate->GetMirPart(msg);
				if(pMirDatumTube&&pMirBottomPlate.IsHasPtr())
				{
					if(pMirBottomPlate->ucs.axis_z*pMirDatumTube->ucs.axis_z>0)
#ifdef AFX_TARG_ENU_ENGLISH
						pMirPlate->SetDesignItemValue('K',-pMirDatumTube->handle,"datum tube that rib depend on",PARAM_POS,1);
					else
						pMirPlate->SetDesignItemValue('K',pMirDatumTube->handle,"datum tube that rib depend on",PARAM_POS,1);
					pMirPlate->SetDesignItemValue('L',pMirBottomPlate->handle,"datum bottom plate",PARAM_POS,1);
#else
						pMirPlate->SetDesignItemValue('K',-pMirDatumTube->handle,"����������Ļ�׼�ֹ�",PARAM_POS,1);
					else
						pMirPlate->SetDesignItemValue('K',pMirDatumTube->handle,"����������Ļ�׼�ֹ�",PARAM_POS,1);
					pMirPlate->SetDesignItemValue('L',pMirBottomPlate->handle,"��׼�װ�",PARAM_POS,1);
#endif
					//�������Ķ�λ����� wxc-2015.3.8
					f3dPoint axis_mir_x=GetMirVector(pSrcParamPlate->ucs.axis_x,msg);	//�Գ�����X�᷽��
					project_vector(axis_mir_x,pMirDatumTube->ucs.axis_z);
					normalize(axis_mir_x);
					//����Գ�����ڶԳƻ�׼�ֹ��ϵĶ�λ�Ƕ�
					long J=0;
					double angle=CalRotateAngle(pMirDatumTube->ucs.axis_z,pMirDatumTube->ucs.axis_x,axis_mir_x);
					if(pMirBottomPlate->GetClassTypeId()==CLS_PLATE)
						J=pMirBottomPlate.PlatePointer()->m_bAttachPartByWeldRoad=pBottomPlate.PlatePointer()->m_bAttachPartByWeldRoad;
					else if(pMirBottomPlate->GetClassTypeId()==CLS_PARAMPLATE)
					{
						if(pBottomPlate.ParamPlatePointer()->GetDesignItemValue('J',&J))
							pMirBottomPlate.ParamPlatePointer()->SetDesignItemValue('J',J,NULL,PARAM_POS,1);
						else
							J=pMirDatumTube->feature=pBottomPlate->feature;
					}
					double weldRoadAngle=0;
					if(pMirDatumTube->m_bHasWeldRoad&&J!=0)
						weldRoadAngle=pMirDatumTube->CalWeldRoadAngle();
					angle-=weldRoadAngle;
					if(angle<0)
						angle+=2*Pi;
					angle*=DEGTORAD_COEF;
#ifdef AFX_TARG_ENU_ENGLISH
					pMirPlate->SetDesignItemValue('Y',angle,"rib plate positioning direction angle",PARAM_POS);
#else
					pMirPlate->SetDesignItemValue('Y',angle,"��嶨λ�����",PARAM_POS);
#endif
				}
				else
				{
					if(pMirDatumTube==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
						logerr.Log("0X%X tube's symmetrical tube not found!",abs(K));
					if(!pMirBottomPlate.IsHasPtr())
						logerr.Log("0X%X tube's symmetrical tube not found!",L);
#else
						logerr.Log("0X%X�ֹܵĶԳƸֹ�δ�ҵ�!",abs(K));
					if(!pMirBottomPlate.IsHasPtr())
						logerr.Log("0X%X�ְ�ĶԳƸְ�δ�ҵ�!",L);
#endif
					//console.DispPartSet.DeleteNode(pMirPlate->handle);
					console.DeletePart(pMirPlate->handle);
					return NULL;
				}
				//��������ڷ����̵���������������	
				if(pMirBottomPlate->GetClassTypeId()==CLS_PARAMPLATE)
					pMirBottomPlate.ParamPlatePointer()->AppendAttachPart(pMirPlate);
			}
		}
		else
		{
			f3dPoint columnnorm;
			pSrcParamPlate->GetDesignItemValue('x', &columnnorm.x);
			pSrcParamPlate->GetDesignItemValue('y', &columnnorm.y);
			pSrcParamPlate->GetDesignItemValue('z', &columnnorm.z);
			columnnorm=GetMirVector(columnnorm,msg);
#ifdef AFX_TARG_ENU_ENGLISH
			pMirPlate->SetDesignItemValue('x',columnnorm.x,"X component in tube direction");
			pMirPlate->SetDesignItemValue('y',columnnorm.y,"Y component in tube direction");
			pMirPlate->SetDesignItemValue('z',columnnorm.z,"Z component in tube direction");
#else
			pMirPlate->SetDesignItemValue('x',columnnorm.x,"�ֹܷ���X����");
			pMirPlate->SetDesignItemValue('y',columnnorm.y,"�ֹܷ���Y����");
			pMirPlate->SetDesignItemValue('z',columnnorm.z,"�ֹܷ���Z����");
#endif
		}
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
	{
		long C,E,G,F,c,e,g,f;
		pSrcParamPlate->GetDesignItemValue('E',&E);
		pSrcParamPlate->GetDesignItemValue('F',&F);
		pSrcParamPlate->GetDesignItemValue('G',&G);
		pSrcParamPlate->GetDesignItemValue('C',&C);
		MirPartHandle(pSrcParamPlate->BelongModel(),&e,E,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&f,F,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&g,G,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&c,C,msg);
		if(c==0||e==0||f==0)//||g==0
		{
			console.DispPartSet.DeleteNode(pMirPlate->handle);
			console.DeletePart(pMirPlate->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("fail to meet symmetric requirements, can not finish symmetry!");
#else
			CXhChar200 error;
			if(c==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",C);
			else if(e==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",E);
			else if(f==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",F);
			else if(g==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",G);
			AfxMessageBox(error);
#endif
			return NULL;
		}
		pMirPlate->SetDesignItemValue('C',c,"",PARAM_POS,1);	//��λ��׼�ֹ�
		pMirPlate->SetDesignItemValue('E',e,"",PARAM_POS,1);	//��λ��׼�ڵ�
		//��һ���׼�ְ�
		CLDSGeneralPlate::DESIGN_PARAM para_item1;
		pSrcParamPlate->GetDesignItemValue('F',&para_item1);
		para_item1.value.hPart=f;
		if(para_item1.cValueType!=3)
			pMirPlate->SetDesignItemValue('F',f,"",PARAM_POS,1);
		else //if(para_item1.cValueType==3)
			pMirPlate->SetDesignItem('F',para_item1,"",PARAM_POS);
		//�ڶ����׼�ְ�
		CLDSGeneralPlate::DESIGN_PARAM para_item2;
		pSrcParamPlate->GetDesignItemValue('G',&para_item2);
		para_item2.value.hPart=g;
		if(para_item2.cValueType!=3)
			pMirPlate->SetDesignItemValue('G',g,"",PARAM_POS,1);	
		else //if(para_item2.cValueType==3)
			pMirPlate->SetDesignItem('G',para_item2,"",PARAM_POS);
		//���ǰ�
		for(int i=0;i<4;i++)
		{
			CXhChar16 sKey("P%d",i);
			CLDSGeneralPlate::DESIGN_PARAM para_item;
			if(!pSrcParamPlate->GetDesignItemValue(KEY2C(sKey),&para_item))
				continue;
			long H=para_item.value.hPart,h=0;
			MirPartHandle(pSrcParamPlate->BelongModel(),&h,H,msg);
			if(h==0||h==f||h==g)
				continue;	//
			para_item.value.hPart=h;
			pMirPlate->SetDesignItem(KEY2C(sKey),para_item,"",PARAM_POS);
		}
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
	{
		long E=0,F=0,P=0,C=0,e=0,f=0,p=0,c=0;
		pSrcParamPlate->GetDesignItemValue('P',&P);	//'A'��Ϊ'P' wht 11-05-17
		pSrcParamPlate->GetDesignItemValue('E',&E);	//'B'��Ϊ'E'
		pSrcParamPlate->GetDesignItemValue('F',&F);	//'C'��Ϊ'F'
		pSrcParamPlate->GetDesignItemValue('C',&C);	//��׼�ֹ�
		MirPartHandle(pSrcParamPlate->BelongModel(),&p,P,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&e,E,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&f,F,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&c,C,msg);
		if(c==0||p==0||(E>0x20&&e==0))//||(F>0x20&&f==0)
		{
			console.DispPartSet.DeleteNode(pMirPlate->handle);
			console.DeletePart(pMirPlate->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("fail to meet symmetric requirements, can not finish symmtry!");
#else
			CXhChar200 error;
			if(c==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",C);
			else if(e==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",E);
			else if(f==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",F);
			else if(p==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",P);
			AfxMessageBox(error);
#endif
			return NULL;
		}
		//��һ���׼�ְ�
		CLDSGeneralPlate::DESIGN_PARAM para_item1;
		pSrcParamPlate->GetDesignItemValue('E',&para_item1);
		para_item1.value.hPart=e;
		if(para_item1.cValueType!=3)
			pMirPlate->SetDesignItemValue('E',e,"",PARAM_POS,1);
		else //if(para_item1.cValueType==3)
			pMirPlate->SetDesignItem('E',para_item1,"",PARAM_POS);
		//�ڶ����׼�ְ�
		CLDSGeneralPlate::DESIGN_PARAM para_item2;
		pSrcParamPlate->GetDesignItemValue('F',&para_item2);
		para_item2.value.hPart=f;
		if(para_item2.cValueType!=3)
			pMirPlate->SetDesignItemValue('F',f,"",PARAM_POS,1);	
		else //if(para_item2.cValueType==3)
			pMirPlate->SetDesignItem('F',para_item2,"",PARAM_POS);
		//
		pMirPlate->SetDesignItemValue('C',c,"",PARAM_POS,1);	//��λ��׼�ֹ�
		pMirPlate->SetDesignItemValue('P',p,"",PARAM_POS,1);	//��λ��׼�ڵ�
		//�߰����ӷ���(�����߰壬��ͨ���������η���) wht 11-05-17
		pMirPlate->ConnDirectList.Empty();
		CONNECT_DIRECTION *pVec=NULL;
		for(pVec=pSrcParamPlate->ConnDirectList.GetFirst();pVec;pVec=pSrcParamPlate->ConnDirectList.GetNext())
		{
			CONNECT_DIRECTION *pMirVec=pMirPlate->ConnDirectList.SetValue(pVec->serial,*pVec);
			MirDatumVector(&Ta,pVec->direct,pMirVec->direct,msg);
		}
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
	{
		long C=0,P=0,E=0,F=0,c=0,p=0,e=0,f=0;
		pSrcParamPlate->GetDesignItemValue('C',&C);
		pSrcParamPlate->GetDesignItemValue('P',&P);
		pSrcParamPlate->GetDesignItemValue('E',&E);
		pSrcParamPlate->GetDesignItemValue('F',&F);
		MirPartHandle(pSrcParamPlate->BelongModel(),&c,C,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&p,P,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&e,E,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&f,F,msg);
		if(c==0||p==0||e==0)
		{
			console.DeletePart(pMirPlate->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("fail to meet symmetric requirements, can not finish symmetry!");
#else
			CXhChar200 error;
			if(c==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",C);
			else if(e==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",E);
			else if(f==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",F);
			else if(p==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",P);
			AfxMessageBox(error);
#endif
			return NULL;
		}
		//��һ���׼�ְ�
		CLDSGeneralPlate::DESIGN_PARAM para_item1;
		pSrcParamPlate->GetDesignItemValue('E',&para_item1);
		para_item1.value.hPart=e;
		if(para_item1.cValueType!=3)
			pMirPlate->SetDesignItemValue('E',e,"",PARAM_POS,1);
		else //if(para_item1.cValueType==3)
			pMirPlate->SetDesignItem('E',para_item1,"",PARAM_POS);
		//�ڶ����׼�ְ�
		CLDSGeneralPlate::DESIGN_PARAM para_item2;
		pSrcParamPlate->GetDesignItemValue('F',&para_item2);
		para_item2.value.hPart=f;
		if(para_item2.cValueType!=3)
			pMirPlate->SetDesignItemValue('F',f,"",PARAM_POS,1);	
		else //if(para_item2.cValueType==3)
			pMirPlate->SetDesignItem('F',para_item2,"",PARAM_POS);
		//
		pMirPlate->SetDesignItemValue('C',c,"",PARAM_POS,1);	//��λ��׼�ֹ�
		pMirPlate->SetDesignItemValue('P',p,"",PARAM_POS,1);	//��λ��׼�ڵ�
		//��֪�������ε����Ǻ���˼�ˣ����ڸо�����Ҫ����W,N,��Y����������ʮ�ֲ���п�Խ����Z��Գƴ��� wjh 2011-11-10
		/*CLDSPlate *pDatumPlate=(CLDSPlate*)Ta.Parts.FromHandle(E,CLS_PLATE);
		CLDSPlate *pMirDatumPlate=(CLDSPlate*)Ta.Parts.FromHandle(e,CLS_PLATE);
		if(pDatumPlate&&pMirDatumPlate)
		{
			if(pDatumPlate->ucs.axis_z*pMirDatumPlate->ucs.axis_z<0&&(msg.axis_flag==0x01||msg.axis_flag==0x02))
			{	//X/Y��Գ�ʱ��Z�᷽���෴ʱW��Nֵ��Ҫ����
				double W=0,N=0;
				pSrcParamPlate->GetDesignItemValue('W',&W);
				pSrcParamPlate->GetDesignItemValue('N',&N);
				pMirPlate->SetDesignItemValue('W',N,"ȷ�������εĲ���W",PARAM_SHAPE);
				pMirPlate->SetDesignItemValue('N',W,"ȷ�������εĲ���N",PARAM_SHAPE);
				//��������� wht 11-05-17 
				double Y=0;
				pMirPlate->GetDesignItemValue('Y',&Y);
				if(Y>=0)
					Y=-1;
				else
					Y=1;
				pMirPlate->SetDesignItemValue('Y',Y,"��Բ�ͻ�����λ��(>0��׼�����;=0�Զ�;<0��׼���ڲ�)",PARAM_SHAPE);
			}
		}*/
		//��Խ��������ӷ��� wht 11-05-17
		pMirPlate->ConnDirectList.Empty();
		CONNECT_DIRECTION *pVec=NULL;
		for(pVec=pSrcParamPlate->ConnDirectList.GetFirst();pVec;pVec=pSrcParamPlate->ConnDirectList.GetNext())
		{
			CONNECT_DIRECTION *pMirVec=pMirPlate->ConnDirectList.SetValue(pVec->serial,*pVec);
			MirDatumVector(&Ta,pVec->direct,pMirVec->direct,msg);
		}
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
	{	//�Ǹ��߰�
		long A,B,a,b;
		pSrcParamPlate->GetDesignItemValue('A',&A);
		pSrcParamPlate->GetDesignItemValue('B',&B);
		MirPartHandle(pSrcParamPlate->BelongModel(),&a,A,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&b,B,msg);
		if(a==0||b==0)
		{
			//console.DispPartSet.DeleteNode(pMirPlate->handle);
			console.DeletePart(pMirPlate->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("fail to meet symmetric requirements, can not finish symmetry!");
			return NULL;
		}
		pMirPlate->SetDesignItemValue('A',a,"datum angle",PARAM_POS,1);
		pMirPlate->SetDesignItemValue('B',b,"datum node",PARAM_POS,1);
#else
			CXhChar200 error;
			if(a==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",A);
			else if(b==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",B);
			AfxMessageBox(error);
			return NULL;
		}
		pMirPlate->SetDesignItemValue('A',a,"��׼�Ǹ�",PARAM_POS,1);
		pMirPlate->SetDesignItemValue('B',b,"��׼�ڵ�",PARAM_POS,1);
#endif
	}
	else if (pSrcParamPlate->m_iParamType == TYPE_PEDALPLATE)
	{
		long A=0, a=0;
		pSrcParamPlate->GetDesignItemValue('A', &A);
		MirPartHandle(pSrcParamPlate->BelongModel(), &a, A, msg);
		if (a == 0)
		{
			console.DeletePart(pMirPlate->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("fail to meet symmetric requirements, can not finish symmetry!");
#else
			AfxMessageBox(CXhChar200("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!", A));
#endif
			return NULL;
		}
		pMirPlate->SetDesignItemValue('A', a, "", PARAM_POS, 1);
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
	{	//��ˮ��
		long C,B,c,b;
		pSrcParamPlate->GetDesignItemValue('C',&C);
		pSrcParamPlate->GetDesignItemValue('B',&B);
		MirPartHandle(pSrcParamPlate->BelongModel(),&c,C,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&b,B,msg);
		if(c==0||b==0)
		{
			//console.DispPartSet.DeleteNode(pMirPlate->handle);
			console.DeletePart(pMirPlate->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("fail to meet symmetric requirements, can not finish symmetry!");
			return NULL;
		}
		pMirPlate->SetDesignItemValue('C',c,"datum tube",PARAM_POS,1);
		pMirPlate->SetDesignItemValue('B',b,"datum node",PARAM_POS,1);
#else
			CXhChar200 error;
			if(c==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",C);
			else if(b==0)
				error.Printf("δ�ҵ�����0x%X���ʵ�ƥ��Գƹ������޷���ɶԳ�!",B);
			return NULL;
		}
		pMirPlate->SetDesignItemValue('C',c,"��׼�ֹ�",PARAM_POS,1);
		pMirPlate->SetDesignItemValue('B',b,"��׼�ڵ�",PARAM_POS,1);
#endif
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_UEND||pSrcParamPlate->m_iParamType==TYPE_ROLLEND)
	{	//���ݶԳƵõ���Z�᷽����������ת�Ƕ�
		CLDSLineTube *pMirLineTube=(CLDSLineTube*)Ta.FromPartHandle(pMirPlate->m_hPartWeldParent,CLS_LINETUBE);
		if(pMirLineTube==NULL)
		{
			AfxMessageBox("û���ҵ��ԳƲ��ĺ��Ӹֹܣ��Գ�ʧ��!");
			return FALSE;
		}
		pMirLineTube->BuildUCS();
		f3dPoint cal_axis_z=pMirLineTube->WorkPlaneNorm();
		f3dPoint mir_axis_z=GetMirVector(pSrcParamPlate->ucs.axis_z,msg);
		double rad_ang=0,deg_angle=0;
		if(pMirPlate->m_bStdPart)
			pMirPlate->GetDesignItemValue('X',&deg_angle);
		else
			pMirPlate->GetDesignItemValue('D',&deg_angle);
		rad_ang=deg_angle*RADTODEG_COEF;
		if(rad_ang!=0)
			RotateVectorAroundVector(cal_axis_z,rad_ang,pMirLineTube->ucs.axis_z);
		//
		if(mir_axis_z*cal_axis_z<0)
		{
			deg_angle+=180;
			if(deg_angle>=360)
				deg_angle-=360;
			if(pMirPlate->m_bStdPart)
				pMirPlate->SetDesignItemValue('X',deg_angle,"",PARAM_POS);
			else
				pMirPlate->SetDesignItemValue('D',deg_angle,"",PARAM_POS);
		}
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_RIBPLATE)
		pSrcParamPlate->SyncMirPropTo(pMirPlate,msg,"params");
	if(pSrcParamPlate->m_iParamType==TYPE_ENDPLATE||pSrcParamPlate->m_iParamType==TYPE_VERTPLATE)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pSrcParamPlate->m_hPartWeldParent,CLS_PARAMPLATE);
		if(!pParamPlate)
		{
			//console.DispPartSet.DeleteNode(pMirPlate->handle);
			console.DeletePart(pMirPlate->handle);
			return NULL;
		}
		CLDSParamPlate *pMirParamPlate=(CLDSParamPlate*)pParamPlate->GetMirPart(msg);
		if(pMirParamPlate)
			pMirPlate->m_hPartWeldParent=pMirParamPlate->handle;
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_BASEPLATE)
	{
		long hBaseNode,A,hBaseAngle,L;
		pSrcParamPlate->GetDesignItemValue('A',&hBaseNode);
		pSrcParamPlate->GetDesignItemValue('L',&hBaseAngle);
		MirPartHandle(pSrcParamPlate->BelongModel(),&A,hBaseNode,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&L,hBaseAngle,msg);
		if(A>0x20)
			pMirPlate->SetDesignItemValue('A',A,NULL,PARAM_POS,1);
		if(L>0x20)
			pMirPlate->SetDesignItemValue('L',L,NULL,PARAM_POS,1);
		//�ԳƸְ�����ԭ�������Ϣ
		pMirPlate->design.inited=pSrcParamPlate->design.inited;
		MirDatumPoint(&Ta,pSrcParamPlate->design.origin,pMirPlate->design.origin,msg);
		//�����������ĵ�����ͷ
		CLDSLineAngle*	pSrcAngle=NULL,*pMirAngle=NULL;
		pSrcAngle=(CLDSLineAngle*)pSrcParamPlate->BelongModel()->FromPartHandle(hBaseAngle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		pMirAngle=(CLDSLineAngle*)pSrcParamPlate->BelongModel()->FromPartHandle(L,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pSrcAngle && pMirAngle)
		{
			CXhChar50 sKeyStr;
			if(pSrcAngle->pStart->handle==hBaseNode)
				sKeyStr.Copy("oddStart");
			else
				sKeyStr.Copy("oddEnd");	
			int k;
			typedef CLDSLineAngle* CLDSLineAnglePtr;
			CLDSLineAnglePtr ppSonAngles[4]={NULL},ppMirSonAngles[4]={NULL},ppTemporyMirSonAngles[4]={NULL};
			if(pSrcAngle->GetClassTypeId()!=CLS_GROUPLINEANGLE)
			{
				ppSonAngles[0]=pSrcAngle;
				ppMirSonAngles[0]=pMirAngle;
			}
			else
			{
				for(k=0;k<4;k++)
					ppSonAngles[k]=(CLDSLineAngle*)Ta.Parts.FromHandle(((CLDSGroupLineAngle*)pSrcAngle)->son_jg_h[k],CLS_LINEANGLE);
				for(k=0;k<4;k++)
					ppTemporyMirSonAngles[k]=(CLDSLineAngle*)Ta.Parts.FromHandle(((CLDSGroupLineAngle*)pMirAngle)->son_jg_h[k],CLS_LINEANGLE);
				for(k=0;k<4;k++)
				{
					if(ppSonAngles[k]==NULL)
						break;
					GEPOINT vxMirWingNorm=msg.MirVector(ppSonAngles[k]->vxWingNorm);
					GEPOINT vyMirWingNorm=msg.MirVector(ppSonAngles[k]->vyWingNorm);
					for(int k2=0;k2<4;k2++)
					{
						if(ppTemporyMirSonAngles[k2]==NULL)
							break;
						if((vxMirWingNorm*ppTemporyMirSonAngles[k2]->vxWingNorm>EPS_COS2&&vyMirWingNorm*ppTemporyMirSonAngles[k2]->vyWingNorm>EPS_COS2)||
						   (vyMirWingNorm*ppTemporyMirSonAngles[k2]->vxWingNorm>EPS_COS2&&vxMirWingNorm*ppTemporyMirSonAngles[k2]->vyWingNorm>EPS_COS2))
						{
						   ppMirSonAngles[k]=ppTemporyMirSonAngles[k2];
						   break;
						}
					}
				}
			}
			for(k=0;k<4;k++)
			{
				pSrcAngle=ppSonAngles[k];
				pMirAngle=ppMirSonAngles[k];
				if(pSrcAngle==NULL||pMirAngle==NULL)
					continue;
				PROPLIST_TYPE* pListProps=pSrcAngle->GetSyncPropList();
				PROPLIST_ITEM* pItem=pListProps->GetPropItem(sKeyStr);
				MEMORY_BLOCK mpcl_copy1=pSrcAngle->mpcl;
				MEMORY_BLOCK mpcl_copy2=pMirAngle->mpcl;
				pSrcAngle->mpcl.SetBitState((WORD)(pItem->id-1),true);
				pMirAngle->mpcl.SetBitState((WORD)(pItem->id-1),true);
				pSrcAngle->SyncMirPropTo(pMirAngle,msg,sKeyStr);
				pSrcAngle->mpcl=mpcl_copy1;
				pMirAngle->mpcl=mpcl_copy2;
				if(pSrcAngle->m_hPartWeldParent==pSrcParamPlate->handle)
				{
					pMirAngle->m_hPartWeldParent=pMirPlate->handle;
					pMirAngle->m_bWeldPart=pSrcAngle->m_bWeldPart;
				}
				pSrcAngle->SyncMirPropTo(pMirAngle,msg,"cutAngleWing");
			}
		}
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_SHOEPLATE)
	{
		//��ȡĸ���ϵ������Ǹּ��ϣ����¶ԳƽǸֵĹ���������Ϣ(����ͷ���н�)
		ATOM_LIST<THANDLE> angleHandleList;
		long hBaseNode=0,hBasePlate=0,hBaseAngle=0,hConnectShoePlate=0,hRayAngleH=0,hRayAngleX=0;	//������ʼֵ��������������д�����Чֵ wjh-2015.9.21
		if(pSrcParamPlate->GetDesignItemValue('D',&hBaseAngle) && hBaseAngle>0X20)
			angleHandleList.append(hBaseAngle);
		if(pSrcParamPlate->GetDesignItemValue(KEY2C("LH"),&hRayAngleH) && hRayAngleH>0X20)
			angleHandleList.append(hRayAngleH);
		if(pSrcParamPlate->GetDesignItemValue(KEY2C("LX"),&hRayAngleX) && hRayAngleX>0X20)
			angleHandleList.append(hRayAngleX);
		pSrcParamPlate->GetDesignItemValue('C',&hBasePlate);
		CLDSGeneralPlate* pBasePlate=(CLDSGeneralPlate*)pSrcParamPlate->BelongModel()->FromPartHandle(hBasePlate);
		hBaseNode=pBasePlate->m_hBaseNode;
		if(msg.axis_flag==0x01||msg.axis_flag==0x02)
		{	//ѥ���е�'W'������ʾѥ���ڵװ�����������ᣬ�ڽ���X/Y�Գ�ʱ��ӦX/Y���� wjh-2015.9.21
			CLDSParamPlate::DESIGN_PARAM param;
			if(pSrcParamPlate->GetDesignItemValue(KEY2C("W"),&param) && (param.ToLong()==0||param.ToLong()==1))
				pMirPlate->SetDesignItemValue(KEY2C("W"),1-param.ToLong(),NULL,param.cParamType,param.cValueType);
		}
		/*{	//X�ᡢY��Գ�ѥ��ʱ����Ҫ�����ԳƷ�ʽΪ��ת�Գ� wht 15-04-17
			if(pBasePlate->ucs.axis_x.IsEqual(f3dPoint(1))||pBasePlate->ucs.axis_x.IsEqual(f3dPoint(-1)))
			{	//Դѥ����1��4����,X��Գ�Ӧת��Ϊ˳ʱ����ת90�ȣ�Y��Գ�Ӧת��Ϊ��ʱ����ת90
				if(msg.axis_flag==0x01)
					msg.rotate_angle=-90;
				else
					msg.rotate_angle=90;
			}
			else //if(pBasePlate->ucs.axis_x.IsEqual(f3dPoint(0,1,0))||pBasePlate->ucs.axis_x.IsEqual(f3dPoint(0,-1,0)))
			{	//Դѥ����2��3����,X��Գ�Ӧת��Ϊ��ʱ����ת90�ȣ�Y��Գ�Ӧת��Ϊ˳ʱ����ת90
				if(msg.axis_flag==0x01)
					msg.rotate_angle=90;
				else
					msg.rotate_angle=-90;
			}
			msg.axis_flag=0x08;
			msg.array_num=1;
		}*/
		CLDSLineAngle *pLineAngle=NULL,*pDestAngle=NULL;
		for(THANDLE* pHandle=angleHandleList.GetFirst();pHandle;pHandle=angleHandleList.GetNext())
		{
			long hDestAngle=0;
			pLineAngle=(CLDSLineAngle*)pSrcParamPlate->BelongModel()->FromPartHandle(*pHandle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			MirPartHandle(pSrcParamPlate->BelongModel(),&hDestAngle,*pHandle,msg);
			pDestAngle=(CLDSLineAngle*)pSrcParamPlate->BelongModel()->FromPartHandle(hDestAngle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pLineAngle==NULL || pDestAngle==NULL)
				continue;
			CXhChar50 sKeyStr;
			if(pLineAngle->pStart->handle==hBaseNode)
				sKeyStr.Copy("oddStart");
			else
				sKeyStr.Copy("oddEnd");	
			PROPLIST_TYPE* pListProps=pLineAngle->GetSyncPropList();
			PROPLIST_ITEM* pItem=pListProps->GetPropItem(sKeyStr);
			MEMORY_BLOCK mpcl_copy1=pLineAngle->mpcl;
			MEMORY_BLOCK mpcl_copy2=pDestAngle->mpcl;
			pLineAngle->mpcl.SetBitState((WORD)(pItem->id-1),true);
			pDestAngle->mpcl.SetBitState((WORD)(pItem->id-1),true);
			pLineAngle->SyncMirPropTo(pDestAngle,msg,sKeyStr);
			pLineAngle->mpcl=mpcl_copy1;
			pDestAngle->mpcl=mpcl_copy2;
		}
		//���¶Գ�ѥ�������������Ϣ
		long C,D,G,LH,LX;
		pSrcParamPlate->GetDesignItemValue('G',&hConnectShoePlate);
		MirPartHandle(pSrcParamPlate->BelongModel(),&C,hBasePlate,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&D,hBaseAngle,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&G,hConnectShoePlate,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&LH,hRayAngleH,msg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&LX,hRayAngleX,msg);
		pMirPlate->SetDesignItemValue('C',C,NULL,PARAM_POS,1);
		pMirPlate->SetDesignItemValue('D',D,NULL,PARAM_POS,1);
		if(G>0x20)
			pMirPlate->SetDesignItemValue('G',G,NULL,PARAM_POS,1);
		if(LH>0x20)
			pMirPlate->SetDesignItemValue(KEY2C("LH"),LH,NULL,PARAM_POS,1);
		if(LX>0x20)
			pMirPlate->SetDesignItemValue(KEY2C("LX"),LX,NULL,PARAM_POS,1);
	}
	if((msg.axis_flag&4)==0&&(msg.axis_flag&8)==0)	//����Z��Գƺ���ת�Գ�
		pMirPlate->ucs.axis_y*=-1.0;
	//
	if(pTube&&pTube->GetClassTypeId()==CLS_LINETUBE)
	{
		//�ڲ���Z��Գƻ���ת�ԳƵĸ˼�ʱ������һ�˶Գƣ����Ҳ��������
		/*if(msg.axis_flag!=4&&msg.axis_flag!=8&&cosa<EPS_COS2)
			pMirPart=pPart->GetMirPart(msg,scope_start0_end1_elseall);
		else
			pMirPart=pPart->GetMirPart(msg);*/
		double* pPickCoord=NULL;
		if(pPickPoint)
			pPickCoord=(double*)(*pPickPoint);
		CLDSLineTube *pMirTube=(CLDSLineTube*)pTube->GetMirPart(msg,pPickCoord);//����һ�˶ԳƲ��Ҹ˼�
		//�������Գ���Ч�Լ���ʮ�д��������Գ��㷨�ĸı��Ѳ�����Ҫ,�۲�һ��ʱ���ȥ�� wjh-2011.8.9
		CLDSNode *pMirStartNode=pTube->pStart->GetMirNode(msg);
		CLDSNode *pMirEndNode=pTube->pEnd->GetMirNode(msg);
		if(pMirTube==NULL||(pickPos==pTube->pStart->Position(false)&&pMirStartNode==NULL)||
			(pickPos==pTube->pEnd->Position(false)&&pMirEndNode==NULL)||(pPickPoint==NULL&&(pMirStartNode==NULL||pMirEndNode==NULL)))
		{	//Դ�˼����Ϊʼ����������������ʼ�˽ڵ��Ӧ�Ľڵ�	wht 10-01-15
			//Դ�˼����Ϊ�ն˶����������������ն˽ڵ��Ӧ�Ľڵ�
			//console.DispPartSet.DeleteNode(pMirPlate->handle);
			console.DeletePart(pMirPlate->handle);
			return NULL;
		}
		if(pMirTube==pTube)	//�ԳƸ˼�Ϊ����
		{
			if(pickPos==pTube->pStart->Position(false))
			{	//Դ�˼�Ϊʼ������
				pMirTube->m_tJointEnd=pTube->m_tJointStart;
				if(pSrcParamPlate->handle==pTube->m_tJointStart.hLinkObj)
					pMirTube->m_tJointEnd.hLinkObj=pMirPlate->handle;
				pMirTube->SetEndOdd(pTube->startOdd());
				pMirTube->desEndPos.fNormOffset=pTube->desStartPos.fNormOffset;	//�ֹܻ�׼�淨��ƫ����
			}
			else if(pickPos==pTube->pEnd->Position(false))
			{	//Դ�˼�Ϊ�ն�����
				pMirTube->m_tJointStart=pTube->m_tJointEnd;
				if(pSrcParamPlate->handle==pTube->m_tJointEnd.hLinkObj)
					pMirTube->m_tJointStart.hLinkObj=pMirPlate->handle;
				pMirTube->SetStartOdd(pTube->endOdd());
				pMirTube->desStartPos.fNormOffset=pTube->desEndPos.fNormOffset;	//�ֹܻ�׼�淨��ƫ����
			}
		}
		else
		{
			if(pickPos==pTube->pStart->Position(false)&&pMirStartNode&&pMirStartNode==pMirTube->pStart)
			{	//�ԳƸ˼���˶�ӦԴ�˼����
				pMirTube->m_tJointStart=pTube->m_tJointStart;
				if(pSrcParamPlate->handle==pTube->m_tJointStart.hLinkObj)
					pMirTube->m_tJointStart.hLinkObj=pMirPlate->handle;
				pMirTube->SetStartOdd(pTube->startOdd());
				pMirTube->desStartPos.fNormOffset=pTube->desStartPos.fNormOffset;	//�ֹܻ�׼�淨��ƫ����
			}
			else if(pickPos==pTube->pEnd->Position(false)&&pMirEndNode&&pMirEndNode==pMirTube->pEnd)
			{	//�ԳƸ˼��ն˶�ӦԴ�˼��ն�
				pMirTube->m_tJointEnd=pTube->m_tJointEnd;
				if(pSrcParamPlate->handle==pTube->m_tJointEnd.hLinkObj)
					pMirTube->m_tJointEnd.hLinkObj=pMirPlate->handle;
				pMirTube->SetEndOdd(pTube->endOdd());
				pMirTube->desEndPos.fNormOffset=pTube->desEndPos.fNormOffset;	//�ֹܻ�׼�淨��ƫ����
			}
			else if(pickPos==pTube->pStart->Position(false)&&pMirStartNode&&pMirStartNode==pMirTube->pEnd)
			{	//�ԳƸ˼���˶�ӦԴ�˼��ն�
				pMirTube->m_tJointEnd=pTube->m_tJointStart;
				if(pSrcParamPlate->handle==pTube->m_tJointStart.hLinkObj)
					pMirTube->m_tJointEnd.hLinkObj=pMirPlate->handle;
				pMirTube->SetEndOdd(pTube->startOdd());
				pMirTube->desEndPos.fNormOffset=pTube->desStartPos.fNormOffset;	//�ֹܻ�׼�淨��ƫ����
			}
			else if(pickPos==pTube->pEnd->Position(false)&&pMirEndNode&&pMirEndNode==pMirTube->pStart)
			{	//�ԳƸ˼��ն˶�ӦԴ�˼����
				pMirTube->m_tJointStart=pTube->m_tJointEnd;
				if(pSrcParamPlate->handle==pTube->m_tJointEnd.hLinkObj)
					pMirTube->m_tJointStart.hLinkObj=pMirPlate->handle;
				pMirTube->SetStartOdd(pTube->endOdd());
				pMirTube->desStartPos.fNormOffset=pTube->desEndPos.fNormOffset;	//�ֹܻ�׼�淨��ƫ����
			}
		}
		pMirTube->CalPosition();
		if(pMirTube->m_tJointStart.hLinkObj==pMirPlate->handle)
			pMirTube->DesignStartJointUCS(pMirPlate);
		else if(pMirTube->m_tJointEnd.hLinkObj==pMirPlate->handle)
			pMirTube->DesignEndJointUCS(pMirPlate);
		//
		pTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		pMirTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pTube->GetSolidPartObject());
		//g_pSolidDraw->NewSolidPart(pMirTube->GetSolidPartObject());
	}
	//
	pMirPlate->DesignPlate(false);	//���������˨����Ϊ��˨һ������Ӱ�쵽��˨�ĶԳ�
	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
	AddMirObjToSrcObjRelaObjList(pSrcParamPlate,pMirPlate,msg);
	//
	if(pSrcParamPlate->m_iParamType==TYPE_XEND)
	{	//�Գ�ʮ�ֲ��ʱ Ӧͬʱ�Գ�������ʮ�ֲ�嶨λ������ 
		//����Գ����ɲ���ϵ���˨ʱ�޷��ҵ���λ���� wht 10-03-18
		CLDSPlate *pMirVertPlate[2]={NULL,NULL},*pSrcVertPlate[2]={NULL,NULL};
		pSrcParamPlate->GetVertPlateInXEndPlate(&pSrcVertPlate[0],&pSrcVertPlate[1]);
		//���ȶԳ�ʮ�ֲ�������,������ʮ�ֲ�����Ϊ��λ��׼����;
		//���ŶԳ�ʮ�ֲ��İ���,������ʮ�ֲ������������Ϊ��λ��׼����;
		//���Գ�����ʮ�ֲ����˨,ˮƽ��˨�Ի���Ϊ��׼����,��ֱ��˨������Ϊ��׼������wht 10-08-03
		for(int i=0;i<2;i++)
		{
			if(pSrcVertPlate[i]==NULL)
				continue;
			pMirVertPlate[i]=CreateSingleMirPlate(pSrcVertPlate[i],msg);
			//Ŀǰ���ִ˴�ֵͬ�Ὣԭ��ֵ����������ע�͵�(��ԭ��Ϊ���¿ں�Ĳ��ʱ����Ϊ����˼�ȷ��) wjh-2014.8.23
			//pMirVertPlate[i]->designInfo.origin.des_para.hPart=pMirPlate->handle;	
			//pMirVertPlate[i]->designInfo.norm.hVicePart=pMirPlate->handle;
			pMirVertPlate[i]->DesignSetupUcs();
			pMirPlate->AppendAttachPart(pMirVertPlate[i]);	//��ӵ����ӹ���������
		}
		ATTACHPART_ITEM *pItem=NULL;
		for (pItem=pSrcParamPlate->GetFirstAttachPart();pItem;pItem=pSrcParamPlate->GetNextAttachPart())
		{
			CLDSPart *pPart=Ta.Parts.FromHandle(pItem->h);
			if(pPart==NULL)
				continue;
			if(pPart->GetClassTypeId()==CLS_PLATE)
			{	//����
				CLDSPlate *pAttachPlate=(CLDSPlate*)pPart;
				if(pAttachPlate==pSrcVertPlate[1]||pAttachPlate==pSrcVertPlate[0])
					continue;
				if(pAttachPlate->designInfo.origin.datum_pos_style==9	//ָ�������ϵ��������
					||pAttachPlate->designInfo.norm.norm_style==4)		//ָ�������ϵ���Է���
				{	
					CLDSPlate *pMirAttachPlate=CreateSingleMirPlate(pAttachPlate,msg);
					pMirAttachPlate->designInfo.origin.ReplaceReferenceHandle(CHandleReplacer(pSrcParamPlate->handle,pMirPlate->handle),NULL);	
					pMirAttachPlate->designInfo.norm.ReplaceReferenceHandle(CHandleReplacer(pSrcParamPlate->handle,pMirPlate->handle));
					for(int i=0;i<2;i++)
					{
						if(pSrcVertPlate[i]==NULL||pMirVertPlate[i]==NULL)
							continue;
						pMirAttachPlate->designInfo.origin.ReplaceReferenceHandle(CHandleReplacer(pSrcVertPlate[i]->handle,pMirVertPlate[i]->handle),NULL);	
						pMirAttachPlate->designInfo.norm.ReplaceReferenceHandle(CHandleReplacer(pSrcVertPlate[i]->handle,pMirVertPlate[i]->handle));
					}
					pMirAttachPlate->DesignSetupUcs();
					pMirPlate->AppendAttachPart(pMirAttachPlate);	//��ӵ����ӹ���������
				}
			}
			else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			{	//����
				CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPart;
				if(pLineAngle->desStartPos.datumPoint.datum_pos_style==9
					&&pLineAngle->desEndPos.datumPoint.datum_pos_style==9
					&&pLineAngle->des_norm_x.spec_norm.norm_style==4
					&&pLineAngle->des_norm_y.spec_norm.norm_style==4)
				{
					CLDSLineAngle *pMirLineAngle=CreateSingleMirAngle(pLineAngle,msg);
					pMirLineAngle->desStartPos.datumPoint.ReplaceReferenceHandle(CHandleReplacer(pSrcParamPlate->handle,pMirPlate->handle),NULL);
					pMirLineAngle->desEndPos.datumPoint.ReplaceReferenceHandle(CHandleReplacer(pSrcParamPlate->handle,pMirPlate->handle),NULL);
					pMirLineAngle->des_norm_x.spec_norm.ReplaceReferenceHandle(CHandleReplacer(pSrcParamPlate->handle,pMirPlate->handle));
					pMirLineAngle->des_norm_y.spec_norm.ReplaceReferenceHandle(CHandleReplacer(pSrcParamPlate->handle,pMirPlate->handle));
					for(int i=0;i<2;i++)
					{
						if(pSrcVertPlate[i]==NULL||pMirVertPlate[i]==NULL)
							continue;
						pMirLineAngle->desStartPos.datumPoint.ReplaceReferenceHandle(CHandleReplacer(pSrcVertPlate[i]->handle,pMirVertPlate[i]->handle),NULL);
						pMirLineAngle->desEndPos.datumPoint.ReplaceReferenceHandle(CHandleReplacer(pSrcVertPlate[i]->handle,pMirVertPlate[i]->handle),NULL);
						pMirLineAngle->des_norm_x.spec_norm.ReplaceReferenceHandle(CHandleReplacer(pSrcVertPlate[i]->handle,pMirVertPlate[i]->handle));
						pMirLineAngle->des_norm_y.spec_norm.ReplaceReferenceHandle(CHandleReplacer(pSrcVertPlate[i]->handle,pMirVertPlate[i]->handle));
					}
					//XY��Գư���ʱ������ wht 10-08-03
					if(msg.axis_flag&0x01||msg.axis_flag&0x02)
					{	//
						/*f3dPoint start_pos=pMirLineAngle->Start();
						f3dPoint end_pos=pMirLineAngle->End();
						coord_trans(start_pos,pMirPlate->ucs,FALSE);
						coord_trans(end_pos,pMirPlate->ucs,FALSE);
						pMirLineAngle->desStartPos.datumPoint.des_para.hPart=pMirPlate->handle;
						pMirLineAngle->desEndPos.datumPoint.des_para.hPart=pMirPlate->handle;
						pMirLineAngle->desStartPos.datumPoint.datum_pos=start_pos;
						pMirLineAngle->desEndPos.datumPoint.datum_pos=end_pos;
						f3dPoint norm_x=pMirLineAngle->get_norm_x_wing();
						f3dPoint norm_y=pMirLineAngle->get_norm_y_wing();
						if(start_pos.z>0&&pMirVertPlate[0])	//�ϲ�Ǹ�
						{
							if(fabs(pMirPlate->ucs.axis_z*norm_x)>EPS)
							{
								pMirLineAngle->des_norm_x.spec_norm.hVicePart=pMirPlate->handle;
								vector_trans(norm_x,pMirPlate->ucs,FALSE);
								pMirLineAngle->des_norm_x.spec_norm.nearVector=norm_x;
								pMirLineAngle->des_norm_y.spec_norm.hVicePart=pMirVertPlate[0]->handle;
								vector_trans(norm_y,pMirVertPlate[0]->ucs,FALSE);
								pMirLineAngle->des_norm_y.spec_norm.nearVector=norm_y;
							}
							else 
							{
								pMirLineAngle->des_norm_y.spec_norm.hVicePart=pMirPlate->handle;
								vector_trans(norm_y,pMirPlate->ucs,FALSE);
								pMirLineAngle->des_norm_y.spec_norm.nearVector=norm_y;
								pMirLineAngle->des_norm_x.spec_norm.hVicePart=pMirVertPlate[0]->handle;
								vector_trans(norm_x,pMirVertPlate[0]->ucs,FALSE);
								pMirLineAngle->des_norm_x.spec_norm.nearVector=norm_x;
							}
						}
						else if(pMirVertPlate[1])
						{
							if(fabs(pMirPlate->ucs.axis_z*norm_x)>EPS)
							{
								pMirLineAngle->des_norm_x.spec_norm.hVicePart=pMirPlate->handle;
								vector_trans(norm_x,pMirPlate->ucs,FALSE);
								pMirLineAngle->des_norm_x.spec_norm.nearVector=norm_x;
								pMirLineAngle->des_norm_y.spec_norm.hVicePart=pMirVertPlate[1]->handle;
								vector_trans(norm_y,pMirVertPlate[1]->ucs,FALSE);
								pMirLineAngle->des_norm_y.spec_norm.nearVector=norm_y;
							}
							else 
							{
								pMirLineAngle->des_norm_y.spec_norm.hVicePart=pMirPlate->handle;
								vector_trans(norm_y,pMirPlate->ucs,FALSE);
								pMirLineAngle->des_norm_y.spec_norm.nearVector=norm_y;
								pMirLineAngle->des_norm_x.spec_norm.hVicePart=pMirVertPlate[1]->handle;
								vector_trans(norm_x,pMirVertPlate[1]->ucs,FALSE);
								pMirLineAngle->des_norm_x.spec_norm.nearVector=norm_x;
							}
						}*/
						//else	//ָ��֫���߷���
							pMirLineAngle->des_norm_x.spec_norm.norm_style=
							pMirLineAngle->des_norm_y.spec_norm.norm_style=0;
						
					}
					pMirLineAngle->ClearFlag();
					pMirLineAngle->CalPosition();
					pMirLineAngle->cal_x_wing_norm();
					pMirLineAngle->cal_y_wing_norm();
					pMirLineAngle->SetModified();
					pMirLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
					g_pSolidDraw->NewSolidPart(pMirLineAngle->GetSolidPartObject());
					pMirPlate->AppendAttachPart(pMirLineAngle);	//��ӵ����ӹ���������
				}
			}
		}
	}
	//������˨�Գ�
	CLsRef *pLsRef;
	for(pLsRef=pSrcParamPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcParamPlate->GetNextLsRef())
	{
		CLDSBolt *pMirBolt=(CLDSBolt*)pLsRef->GetLsPtr()->GetMirPart(msg);
		if(pMirBolt)
		{
			pMirPlate->AppendLsRef(pMirBolt->GetLsRef());
			pMirBolt->ReplaceReferenceHandle(CHandleReplacer(pSrcParamPlate->handle,pMirPlate->handle));
			if(pMirPlate->m_iParamType==TYPE_ROLLEND||pMirPlate->m_iParamType==TYPE_UEND
				||pMirPlate->m_iParamType==TYPE_XEND)
			{	//�����ڶԳƸְ�ʱ���ɵ���˨�Ķ�λ��׼�����Լ����߻�׼���� wht 10-09-27
				//Դ��˨��λ�����Լ����߻�׼����ΪԴ���ʱִ�����²���
				if(pMirBolt->des_base_pos.hPart==0&&(*pLsRef)->des_base_pos.hPart==pSrcParamPlate->handle)
					pMirBolt->des_base_pos.hPart=pMirPlate->handle;
				if(pMirBolt->des_work_norm.hVicePart==0&&(*pLsRef)->des_work_norm.hVicePart==pSrcParamPlate->handle)
					pMirBolt->des_work_norm.hVicePart=pMirPlate->handle;
				/*if (pLsRef->pBolt->des_base_pos.hPart==pSrcParamPlate->handle)
				{	//���ڶԳ����Ӱ�ʱ�����ȶԳ���˨����ʱ��廹δ�Գƿ��ܵ��¾ֲ���λ����˨λ�ò���������Ҫ�ڴ�ʱ���� wjh-2020.1.14
					GEPOINT mir_axis_x=GetMirVector(pSrcParamPlate->ucs.axis_x,msg);
					GEPOINT mir_axis_y=GetMirVector(pSrcParamPlate->ucs.axis_y,msg);
					if (mir_axis_x*pMirPlate->ucs.axis_x<0)	//�Գƺ�ģ����ѽ����˷���
						pMirBolt->des_base_pos.len_offset_dist*=-1.0;
					if (mir_axis_y*pMirPlate->ucs.axis_y<0)	//�Գƺ�ģ����ѽ����˷���
						pMirBolt->des_base_pos.wing_offset_dist*=-1.0;
				}*/
			}
			pMirBolt->SetModified();
			pMirBolt->Create3dSolidModel();
			if(!pMirBolt->m_bVirtualPart||(pMirBolt->m_bVirtualPart&&g_sysPara.display.bDispVirtualPart))
				g_pSolidDraw->NewSolidPart(pMirBolt->GetSolidPartObject());
		}
		else
		{
			pMirBolt=CreateSingleMirBolt(pLsRef->GetLsPtr(),msg);
			pMirPlate->AppendLsRef(pMirBolt->GetLsRef());
		}
	}
	//�˴���Ӧ�öԲ����˨��λ����Ϊ������˨��δ������Ʋ�����
	//��λ����˨λ�ô���,�����޷�����λ���ҵ��Գ���˨ wht 16-01-30
	/*for(pLsRef=pMirPlate->GetFirstLsRef();pLsRef;pLsRef=pMirPlate->GetNextLsRef())
	{	//Ӧ��������˨�Գ����ɺ��ٵ�λ,ʮ�ֲ���ϵ���˨��������ȫ��λ�öԳƵ�
		//(�ֹܻ�׼�߲�ͬʱ������˨��λ����ȫ�Գ�) wht 11-01-23
		if(pMirPlate->m_iParamType==TYPE_XEND)
		{	//��λʮ�ֲ���ϵ���˨ wht 10-08-03
			CLDSBolt *pMirBolt=pLsRef->GetLsPtr();
			pMirBolt->correct_worknorm();
			pMirBolt->correct_pos();
			pMirBolt->CalGuigeAuto();
			pMirBolt->SetModified();
			pMirBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pMirBolt->GetSolidPartObject());
		}
	}*/
	//��˨������0����ͨ���ԳƼ����Z�᷽�������õ���Z���෴ wht 11-05-17
	if(pMirPlate->m_iParamType==TYPE_CIRRIBPLATE||pMirPlate->m_iParamType==TYPE_TUBERIBPLATE)
	{	
		double S=0,T=0;
		pMirPlate->GetDesignItemValue('S',&S);
		pMirPlate->GetDesignItemValue('T',&T);
		//���ݶԳƵõ���Z�᷽��
		f3dPoint axis_z=GetMirVector(pSrcParamPlate->ucs.axis_z,msg);
		BOOL bReDesign=FALSE;
		if(pMirPlate->ucs.axis_z*axis_z<0&&(fabs(S)>EPS||fabs(T)>EPS))
		{	//�����ԳƸְ�ķ���ƫ����,Ȼ���������
#ifdef AFX_TARG_ENU_ENGLISH
			pMirPlate->SetDesignItemValue('S',-1.0*S,"offset distance in datum tube's extending direction.",PARAM_POS);
			pMirPlate->SetDesignItemValue('T',-1.0*T,"offset distance in circular plate's normal direction.",PARAM_POS);
#else
			pMirPlate->SetDesignItemValue('S',-1.0*S,"�ػ�׼�ֹ����췽���ƫ�ƾ��롣",PARAM_POS);
			pMirPlate->SetDesignItemValue('T',-1.0*T,"�ػ���巨�߷����ƫ�ƾ��롣",PARAM_POS);
#endif
			bReDesign=TRUE;
		}
		else if(pMirPlate->GetLsCount()>0)
			bReDesign=TRUE;	//��˨������0,��Ӱ��ְ����ι���Ҫ�������
		if(bReDesign)	
			pMirPlate->DesignPlate(false);
	}
	/*else if(pMirPlate->m_iParamType==TYPE_RIBPLATE)
	{
		//���ݶԳƵõ���Y�ἰZ�᷽��
		f3dPoint axis_x=GetMirVector(pSrcParamPlate->ucs.axis_x,msg);
		f3dPoint axis_z=GetMirVector(pSrcParamPlate->ucs.axis_z,msg);
		f3dPoint huoqu_norm=GetMirVector(pSrcParamPlate->HuoquFaceNorm(),msg);
		double X=0,Z=0,dx=0,dz=0;
		pMirPlate->GetDesignItemValue('X',&X);	//X����ƫ����
		pMirPlate->GetDesignItemValue('Z',&Z);	//Z����ƫ����
		pMirPlate->GetDesignItemValue(KEY2C("dx"),&dx);	//������ĩ���Զ�λ����ķ���ƫ����
		pMirPlate->GetDesignItemValue(KEY2C("dz"),&dz);	//������ĩ���Զ�λ����ķ���ƫ����
		BOOL bReDesign=FALSE;
		if(pMirPlate->ucs.axis_x*axis_x<0&&fabs(X)>0)
		{
			pMirPlate->SetDesignItemValue('X',-1.0*X,NULL,PARAM_POS,0);
			pMirPlate->SetDesignItemValue(KEY2C("dx"),-1.0*dx,NULL,PARAM_POS,0);
			bReDesign=TRUE;
		}
		if(pMirPlate->ucs.axis_z*axis_z<0&&fabs(Z)>0)
		{
			pMirPlate->SetDesignItemValue('Z',-1.0*Z,NULL,PARAM_POS,0);
			bReDesign=TRUE;
		}
		if(pMirPlate->HuoquFaceNorm()*huoqu_norm<0&&fabs(dz)>0)
		{
			pMirPlate->SetDesignItemValue(KEY2C("dz"),-1.0*dz,NULL,PARAM_POS,0);
			bReDesign=TRUE;
		}
		if(bReDesign)	
			pMirPlate->DesignPlate(false);
	}*/
	else //if(pMirPlate->m_iParamType==TYPE_SHOEPLATE)
	{	//ѥ����Ҫ������˨�����������
		pMirPlate->DesignPlate(false);
	}
	pMirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pMirPlate->GetSolidPartObject());

	RELATIVE_OBJECT* pRelaObj=pSrcParamPlate->relativeObjs.Add(pMirPlate->handle);
	pSrcParamPlate->SetModified(TRUE,FALSE);
	pRelaObj->mirInfo.rotate_angle=msg.rotate_angle;
	pRelaObj->mirInfo.axis_flag=msg.axis_flag;
	return pMirPlate;
}

BOOL MirParamPlate(CLDSParamPlate* pSrcParamPlate, MIRMSG mirmsg, BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//�Ƿ�ɹ��Գ��������й���
	RELATIVE_OBJECT *pRelaObj=NULL;
	int i,flagArr[4]={1,2,4,16};
	CLDSParamPlate* srcplate_arr[4]={NULL};
	srcplate_arr[0]=pSrcParamPlate;
	for(i=0;i<4;i++)
	{
		if((mirmsg.axis_flag&flagArr[i])==0)
			continue;
		MIRMSG msg=mirmsg.GetSubMirItem(i+1);
		srcplate_arr[i+1]=CreateSingleMirParamPlate(pSrcParamPlate,msg,bIncAttachInfo);
		if(srcplate_arr[i+1]==NULL)
			return FALSE;
	}
	int array_num=mirmsg.array_num;
	if((mirmsg.axis_flag&8)==0)	//����ת�Գ�
		array_num=0;
	else
		mirmsg.axis_flag=8;
	short init_angle=mirmsg.rotate_angle;
	for(i=0;i<4;i++)
	{
		if(srcplate_arr[i]==NULL)
			continue;
		pSrcParamPlate = srcplate_arr[i];
		for(int j=0;j<array_num;j++)
		{	//���ж����ת�Գ�ʱ��׼����Ϊͬһ��������ֻ��Ҫ������ת�Ƕȼ���
			if(pSrcParamPlate==NULL)
				continue;
			//������ת�Ƕ�
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSParamPlate *pMirPlate=CreateSingleMirParamPlate(pSrcParamPlate,mirmsg,bIncAttachInfo);
			if(pMirPlate)
			{
				if(pMirPlate->GetThick()<=0)
				{
					//console.DispPartSet.DeleteNode(pMirPlate->handle);
					console.DeletePart(pMirPlate->handle);
				}
				BOOL bInList=FALSE;
				for(pRelaObj=srcplate_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcplate_arr[0]->relativeObjs.GetNext())
				{	//�����ظ���ӹ�������
					if(pRelaObj->hObj==pMirPlate->handle)
					{
						bInList=TRUE;
						break;
					}
				}
				if(bInList)
					continue;
				pRelaObj=srcplate_arr[0]->relativeObjs.Add(pMirPlate->handle);
				pRelaObj->mirInfo.rotate_angle=init_angle*(j+1);
				pRelaObj->mirInfo.axis_flag=mirmsg.axis_flag;
				if(i>0)
					pRelaObj->mirInfo.axis_flag+=flagArr[i-1];
			}
			else 
				bSucceed=FALSE;
		}
	}
	for(i=0;i<3;i++)
	{
		if(srcplate_arr[i+1]&&srcplate_arr[i+1]->GetThick()<=0)
		{
			//console.DispPartSet.DeleteNode(srcplate_arr[i+1]->handle);
			console.DeletePart(srcplate_arr[i+1]->handle);
		}
	}
	//���öԳ����ɵĶ���˴˹���
	SetRelativeObj(srcplate_arr[0]);
	return bSucceed;
}

CLDSSphere* CreateSingleMirSphere(CLDSSphere* pSrcSphere, MIRMSG msg, BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	CLDSSphere *pMirSphere=NULL;
	pMirSphere=(CLDSSphere*)pSrcSphere->GetMirPart(msg);
	Ta.BeginUndoListen();
	if(pMirSphere==NULL)
	{
		pMirSphere = (CLDSSphere*)console.AppendPart(CLS_SPHERE);
		pMirSphere->CopyProperty(pSrcSphere);
		AddMirObjToSrcObjRelaObjList(pSrcSphere,pMirSphere,msg);	//��ӶԳ����ɵĶ���Դ����Ĺ��������б�
	}
	CalMirAtomLayer(pSrcSphere->layer(),pMirSphere->layer(),msg);
	if(!bIncAttachInfo)
	{
		Ta.EndUndoListen();
		return pSrcSphere;
	}
	MirPartHandle(pSrcSphere->BelongModel(),&pMirSphere->hDatumNode,pSrcSphere->hDatumNode,msg);
	CLDSNode *pDatumNode=Ta.Node.FromHandle(pMirSphere->hDatumNode);
	if(pDatumNode)
		pMirSphere->ucs.origin=pDatumNode->Position(true);
	else
		pMirSphere->ucs.origin=GetMirPos(pSrcSphere->ucs.origin,msg);
	pMirSphere->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pMirSphere->GetSolidPartObject());
	g_pSolidDraw->Draw();
	Ta.EndUndoListen();
	return pMirSphere;
}

BOOL MirSphere(CLDSSphere* pSrcSphere, MIRMSG mirmsg, BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//�Ƿ�ɹ��Գ��������й���
	RELATIVE_OBJECT *pRelaObj=NULL;
	int flagArr[3]={1,2,4};
	CLDSSphere* srcsphere_arr[4]={NULL};
	srcsphere_arr[0]=pSrcSphere;
	for(int i=0;i<3;i++)
	{
		if((mirmsg.axis_flag&flagArr[i])==0)
			continue;
		MIRMSG msg=mirmsg.GetSubMirItem(i+1);
		srcsphere_arr[i+1]=CreateSingleMirSphere(pSrcSphere,msg,bIncAttachInfo);
		if(srcsphere_arr[i+1]==NULL)
			return FALSE;
	}
	int array_num=mirmsg.array_num;
	if((mirmsg.axis_flag&8)==0)	//����ת�Գ�
		array_num=0;
	else
		mirmsg.axis_flag=8;
	short init_angle=mirmsg.rotate_angle;
	for(i=0;i<4;i++)
	{
		if(srcsphere_arr[i]==NULL)
			continue;
		pSrcSphere = srcsphere_arr[i];
		for(int j=0;j<array_num;j++)
		{	//���ж����ת�Գ�ʱ��׼����Ϊͬһ��������ֻ��Ҫ������ת�Ƕȼ���
			if(pSrcSphere==NULL)
				continue;
			//������ת�Ƕ�
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSSphere *pMirSphere=CreateSingleMirSphere(pSrcSphere,mirmsg,bIncAttachInfo);
			if(pMirSphere)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcsphere_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcsphere_arr[0]->relativeObjs.GetNext())
				{	//�����ظ���ӹ�������
					if(pRelaObj->hObj==pMirSphere->handle)
					{
						bInList=TRUE;
						break;
					}
				}
				if(bInList)
					continue;
				pRelaObj=srcsphere_arr[0]->relativeObjs.Add(pMirSphere->handle);
				pRelaObj->mirInfo.rotate_angle=init_angle*(j+1);
				pRelaObj->mirInfo.axis_flag=mirmsg.axis_flag;
				if(i>0)
					pRelaObj->mirInfo.axis_flag+=flagArr[i-1];
			}
			else
				bSucceed=FALSE;
		}
	}
	//���öԳ����ɵĶ���˴˹���
	SetRelativeObj(srcsphere_arr[0]);
	return bSucceed;
}

CLDSPolyJoint* CreateSingleMirPolyJoint(CLDSPolyJoint* pSrcPolyJoint, MIRMSG msg, char *sErrorInfo=NULL)
{
	CLDSPolyJoint *pMirPolyJoint=NULL;
	pMirPolyJoint=(CLDSPolyJoint*)pSrcPolyJoint->GetMirPart(msg);
	if(pMirPolyJoint)	//�ԳƸ˼����ε��Ѿ�����
		return pMirPolyJoint;
	if(pSrcPolyJoint->pPrevLinePart==NULL||pSrcPolyJoint->pNextLinePart==NULL)
		return NULL;
	long huoqu_base_wing=0;
	if(pSrcPolyJoint->pPrevLinePart->IsAngle())
		huoqu_base_wing=((CLDSLineAngle*)pSrcPolyJoint->pPrevLinePart)->huoqu_base_wing_x0_y1_both2;
	long hNextLinePart=0,hPrevLinePart=0;
	MirPartHandle(pSrcPolyJoint->BelongModel(),&hNextLinePart,pSrcPolyJoint->pNextLinePart->handle,msg);
	MirPartHandle(pSrcPolyJoint->BelongModel(),&hPrevLinePart,pSrcPolyJoint->pPrevLinePart->handle,msg);
	CLDSLinePart *pPrevLinePart=(CLDSLinePart*)Ta.Parts.FromHandle(hPrevLinePart,CLS_LINEPART);
	CLDSLinePart *pNextLinePart=(CLDSLinePart*)Ta.Parts.FromHandle(hNextLinePart,CLS_LINEPART);
	if(pPrevLinePart&&pNextLinePart)
	{
		Ta.BeginUndoListen();
		long huoqu_handle=Ta.GetNewHandle();
		pPrevLinePart->huoqu_handle=huoqu_handle;
		pNextLinePart->huoqu_handle=huoqu_handle;
		pMirPolyJoint=(CLDSPolyJoint*)console.AppendPart(CLS_POLYJOINT);
		pMirPolyJoint->cMaterial=pPrevLinePart->cMaterial;
		pMirPolyJoint->iSeg=pPrevLinePart->iSeg;
		pMirPolyJoint->SetPartNo(pPrevLinePart->GetPartNo());
		CalMirAtomLayer(pSrcPolyJoint->layer(),pMirPolyJoint->layer(),msg);
		pMirPolyJoint->rBending=pSrcPolyJoint->rBending;
		pMirPolyJoint->pNextLinePart=pNextLinePart;
		pMirPolyJoint->pPrevLinePart=pPrevLinePart;
		if(pPrevLinePart->IsAngle())	//��¼������׼֫
			((CLDSLineAngle*)pMirPolyJoint->pPrevLinePart)->huoqu_base_wing_x0_y1_both2=huoqu_base_wing;
		pMirPolyJoint->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pMirPolyJoint->GetSolidPartObject());
		g_pSolidDraw->Draw();
		Ta.EndUndoListen();
		return pMirPolyJoint;
	}
	return NULL;
}
BOOL MirPolyJoint(CLDSPolyJoint* pSrcPolyJoint, MIRMSG mirmsg, char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//�Ƿ�ɹ��Գ��������й���
	RELATIVE_OBJECT *pRelaObj=NULL;
	int flagArr[3]={1,2,4};
	CLDSPolyJoint* srcPolyJoint_arr[4]={NULL};
	srcPolyJoint_arr[0]=pSrcPolyJoint;
	for(int i=0;i<3;i++)
	{
		if((mirmsg.axis_flag&flagArr[i])==0)
			continue;
		MIRMSG msg=mirmsg.GetSubMirItem(i+1);
		srcPolyJoint_arr[i+1]=CreateSingleMirPolyJoint(pSrcPolyJoint,msg);
		if(srcPolyJoint_arr[i+1]==NULL)
			return FALSE;
	}
	int array_num=mirmsg.array_num;
	if((mirmsg.axis_flag&8)==0)	//����ת�Գ�
		array_num=0;
	else
		mirmsg.axis_flag=8;
	short init_angle=mirmsg.rotate_angle;
	for(i=0;i<4;i++)
	{
		if(srcPolyJoint_arr[i]==NULL)
			continue;
		pSrcPolyJoint = srcPolyJoint_arr[i];
		for(int j=0;j<array_num;j++)
		{	//���ж����ת�Գ�ʱ��׼����Ϊͬһ��������ֻ��Ҫ������ת�Ƕȼ���
			if(pSrcPolyJoint==NULL)
				continue;
			//������ת�Ƕ�
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSPolyJoint *pMirPolyJoint=CreateSingleMirPolyJoint(pSrcPolyJoint,mirmsg);
			if(pMirPolyJoint)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcPolyJoint_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcPolyJoint_arr[0]->relativeObjs.GetNext())
				{	//�����ظ���ӹ�������
					if(pRelaObj->hObj==pMirPolyJoint->handle)
					{
						bInList=TRUE;
						break;
					}
				}
				if(bInList)
					continue;
				pRelaObj=srcPolyJoint_arr[0]->relativeObjs.Add(pMirPolyJoint->handle);
				pRelaObj->mirInfo.rotate_angle=init_angle*(j+1);
				pRelaObj->mirInfo.axis_flag=mirmsg.axis_flag;
				if(i>0)
					pRelaObj->mirInfo.axis_flag+=flagArr[i-1];
			}
			else
				bSucceed=FALSE;
		}
	}
	return bSucceed;
}
CLDSFitting* CreateSingleMirFitting(CLDSFitting* pSrcFitting, MIRMSG msg, char *sErrorInfo = NULL)
{
	CLDSFitting *pMirFitting = NULL;
	pMirFitting = (CLDSFitting*)pSrcFitting->GetMirPart(msg);
	if (pMirFitting)
		return pMirFitting;
	long hBolt1 = 0, hBolt2 = 0, hPart1 = 0, hPart2 = 0;
	MirPartHandle(pSrcFitting->BelongModel(), &hBolt1, pSrcFitting->holes[0].hBolt, msg);
	MirPartHandle(pSrcFitting->BelongModel(), &hPart1, pSrcFitting->holes[0].hResidePart, msg);
	MirPartHandle(pSrcFitting->BelongModel(), &hBolt2, pSrcFitting->holes[1].hBolt, msg);
	MirPartHandle(pSrcFitting->BelongModel(), &hPart2, pSrcFitting->holes[1].hResidePart, msg);
	CLDSBolt* pBolt1 = (CLDSBolt*)pSrcFitting->BelongModel()->FromPartHandle(hBolt1, CLS_BOLT);
	CLDSBolt* pBolt2 = (CLDSBolt*)pSrcFitting->BelongModel()->FromPartHandle(hBolt2, CLS_BOLT);
	CLDSPart* pResidePart1 = pSrcFitting->BelongModel()->FromPartHandle(hPart1);
	CLDSPart* pResidePart2 = pSrcFitting->BelongModel()->FromPartHandle(hPart2);
	if (pBolt1 && pBolt2 && pResidePart1 && pResidePart2)
	{
		//���ݽ������λ�ã�ȷ�����ҹҿ�
		f3dPoint xBasePt1, xBasePt2;
		pResidePart1->GetBoltIntersPos(pBolt1, xBasePt1, NULL, CLDSPart::DATUM_PLANE);
		pResidePart2->GetBoltIntersPos(pBolt2, xBasePt2, NULL, CLDSPart::DATUM_PLANE);
		BYTE ciQuad = 0;
		if (xBasePt1.x >= 0 && xBasePt1.y >= 0)
			ciQuad = 1;
		else if (xBasePt1.x <= 0 && xBasePt1.y >= 0)
			ciQuad = 2;
		else if (xBasePt1.x >= 0 && xBasePt1.y <= 0)
			ciQuad = 3;
		else if (xBasePt1.x <= 0 && xBasePt1.y <= 0)
			ciQuad = 4;
		CLDSPart *pResidePartL = NULL, *pResidePartR = NULL;
		f3dPoint xBaseVec = (xBasePt2 - xBasePt1).normalized();
		if (fabs(xBaseVec.x) > fabs(xBaseVec.y))
		{	//ˮƽ������
			if (ciQuad == 1 || ciQuad == 2)
			{	//ǰ��
				pResidePartL = (xBasePt1.x < xBasePt2.x) ? pResidePart1 : pResidePart2;
				pResidePartR = (xBasePt1.x < xBasePt2.x) ? pResidePart2 : pResidePart1;
			}
			else
			{	//���
				pResidePartL = (xBasePt1.x > xBasePt2.x) ? pResidePart1 : pResidePart2;
				pResidePartR = (xBasePt1.x > xBasePt2.x) ? pResidePart2 : pResidePart1;
			}
		}
		else
		{	//��ֱ������
			if (ciQuad == 1 || ciQuad == 3)
			{	//�Ҳ�
				pResidePartL = (xBasePt1.y > xBasePt2.y) ? pResidePart1 : pResidePart2;
				pResidePartR = (xBasePt1.y > xBasePt2.y) ? pResidePart2 : pResidePart1;
			}
			else
			{	//���
				pResidePartL = (xBasePt1.y < xBasePt2.y) ? pResidePart1 : pResidePart2;
				pResidePartR = (xBasePt1.y < xBasePt2.y) ? pResidePart2 : pResidePart1;
			}
		}
		CLDSBolt* pBoltL = (pResidePartL == pResidePart1) ? pBolt1 : pBolt2;
		CLDSBolt* pBoltR = (pResidePartL == pResidePart1) ? pBolt2 : pBolt1;
		Ta.BeginUndoListen();
		CLDSFitting *pMirFitting = (CLDSFitting*)console.AppendPart(CLS_FITTINGS);
		long handle = pMirFitting->handle;
		pMirFitting->CopyFrom(pSrcFitting);
		pMirFitting->handle = handle;
		pMirFitting->holes[0].hBolt = pBoltL->handle;
		pMirFitting->holes[0].hResidePart = pResidePartL->handle;
		pMirFitting->holes[1].hBolt = pBoltR->handle;
		pMirFitting->holes[1].hResidePart = pResidePartR->handle;
		pMirFitting->DesignFitting();
		pMirFitting->Create3dSolidModel(g_sysPara.bDisplayAllHole, g_pSolidOper->GetScaleUserToScreen(), g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pMirFitting->GetSolidPartObject());
		g_pSolidDraw->Draw();
		Ta.EndUndoListen();
		return pMirFitting;
	}
	return NULL;
}
BOOL MirFitting(CLDSFitting* pSrcFitting, MIRMSG mirmsg, char *sErrorInfo = NULL)
{
	BOOL bSucceed = TRUE;	//�Ƿ�ɹ��Գ��������й���
	RELATIVE_OBJECT *pRelaObj = NULL;
	int flagArr[3] = { 1,2,4 };
	CLDSFitting* srcFitting_arr[4] = { NULL };
	srcFitting_arr[0] = pSrcFitting;
	for (int i = 0; i < 3; i++)
	{
		if ((mirmsg.axis_flag&flagArr[i]) == 0)
			continue;
		MIRMSG msg = mirmsg.GetSubMirItem(i + 1);
		srcFitting_arr[i + 1] = CreateSingleMirFitting(pSrcFitting, msg);
		if (srcFitting_arr[i + 1] == NULL)
			return FALSE;
	}
	int array_num = mirmsg.array_num;
	if ((mirmsg.axis_flag & 8) == 0)	//����ת�Գ�
		array_num = 0;
	else
		mirmsg.axis_flag = 8;
	short init_angle = mirmsg.rotate_angle;
	for (i = 0; i < 4; i++)
	{
		if (srcFitting_arr[i] == NULL)
			continue;
		pSrcFitting = srcFitting_arr[i];
		for (int j = 0; j < array_num; j++)
		{	//���ж����ת�Գ�ʱ��׼����Ϊͬһ��������ֻ��Ҫ������ת�Ƕȼ���
			if (pSrcFitting == NULL)
				continue;
			//������ת�Ƕ�
			mirmsg.rotate_angle = init_angle * (j + 1);
			CLDSFitting *pMirFitting = CreateSingleMirFitting(pSrcFitting, mirmsg);
			if (pMirFitting)
			{
				BOOL bInList = FALSE;
				for (pRelaObj = srcFitting_arr[0]->relativeObjs.GetFirst(); pRelaObj; pRelaObj = srcFitting_arr[0]->relativeObjs.GetNext())
				{	//�����ظ���ӹ�������
					if (pRelaObj->hObj == pMirFitting->handle)
					{
						bInList = TRUE;
						break;
					}
				}
				if (bInList)
					continue;
				pRelaObj = srcFitting_arr[0]->relativeObjs.Add(pMirFitting->handle);
				pRelaObj->mirInfo.rotate_angle = init_angle * (j + 1);
				pRelaObj->mirInfo.axis_flag = mirmsg.axis_flag;
				if (i > 0)
					pRelaObj->mirInfo.axis_flag += flagArr[i - 1];
			}
			else
				bSucceed = FALSE;
		}
	}
	return bSucceed;
}
#endif

BOOL MirTaAtom(CLDSDbObject* pSrcObj, MIRMSG mirmsg,BOOL bIncAttachInfo/*=TRUE*/,char *sErrorInfo/*=NULL*/)
{
	try{
		if(pSrcObj->GetClassTypeId()==CLS_NODE)
		{
			if(!MirNode((CLDSNode*)pSrcObj,mirmsg,bIncAttachInfo,sErrorInfo))
				return FALSE;
		}
		else if(pSrcObj->GetClassTypeId()==CLS_LINEPART)
		{
			if(!MirLinePart((CLDSLinePart*)pSrcObj,mirmsg,bIncAttachInfo,sErrorInfo))
				return FALSE;
		}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(pSrcObj->GetClassTypeId()==CLS_LINEANGLE||pSrcObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			if(!MirAngle((CLDSLineAngle*)pSrcObj,mirmsg,bIncAttachInfo,sErrorInfo))
				return FALSE;
		}
		else if(pSrcObj->GetClassTypeId()==CLS_LINETUBE)
		{
			if(!MirTube((CLDSLineTube*)pSrcObj,mirmsg,bIncAttachInfo,sErrorInfo))
				return FALSE;
		}
		else if(pSrcObj->GetClassTypeId()==CLS_LINESLOT)
		{
			if(!MirSlot((CLDSLineSlot*)pSrcObj,mirmsg,bIncAttachInfo,sErrorInfo))
				return FALSE;
		}
		else if(pSrcObj->GetClassTypeId()==CLS_LINEFLAT)
		{
			if(!MirFlat((CLDSLineFlat*)pSrcObj,mirmsg,bIncAttachInfo,sErrorInfo))
				return FALSE;
		}
		else if(pSrcObj->GetClassTypeId()==CLS_PLATE)
		{
			if(!MirPlate((CLDSPlate*)pSrcObj,mirmsg,bIncAttachInfo,sErrorInfo))
				return FALSE;
		}
		else if(pSrcObj->GetClassTypeId()==CLS_PARAMPLATE)
		{
			if(!MirParamPlate((CLDSParamPlate*)pSrcObj,mirmsg,bIncAttachInfo,sErrorInfo))
				return FALSE;
		}
		else if(pSrcObj->GetClassTypeId()==CLS_BOLT)
		{
			if(!MirBolt((CLDSBolt*)pSrcObj,mirmsg,bIncAttachInfo,sErrorInfo))
				return FALSE;
		}
		else if(pSrcObj->GetClassTypeId()==CLS_SPHERE)
		{
			if(!MirSphere((CLDSSphere*)pSrcObj,mirmsg,bIncAttachInfo,sErrorInfo))
				return FALSE;
		}
		else if(pSrcObj->GetClassTypeId()==CLS_POLYJOINT)
		{	//�Գ����ɸ˼����ε�
			if(!MirPolyJoint((CLDSPolyJoint*)pSrcObj,mirmsg,sErrorInfo))
				return FALSE;
		}
		else if (pSrcObj->GetClassTypeId() == CLS_FITTINGS)
		{
			if (!MirFitting((CLDSFitting*)pSrcObj, mirmsg, sErrorInfo))
				return FALSE;
		}
#endif
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	return TRUE;
}

//�ô�������ֲ��LineTube.cpp�е�CLDSLineTube::SyncMirTransectFaceTo������
//BOOL CreateSingleMirTubeTransectFace(CLDSLineTube *pSrcTube,MIRMSG msg,BOOL bStart,BOOL bEnd, char *sErrorInfo=NULL)
