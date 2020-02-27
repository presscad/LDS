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

//添加对称生成的对象到源对象的关联对象列表
void AddMirObjToSrcObjRelaObjList(CLDSDbObject *pSrcObj,CLDSDbObject *pMirObj,MIRMSG msg)
{
	if(pSrcObj==NULL||pMirObj==NULL)
		return;
	BOOL bInList=FALSE;
	RELATIVE_OBJECT *pItem=NULL;
	for(pItem=pSrcObj->relativeObjs.GetFirst();pItem;pItem=pSrcObj->relativeObjs.GetNext())
	{	//避免重复添加关联对象
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
//设置(基准构件关联对象)列表中的对象彼此关联
void SetRelativeObj(CLDSDbObject *pObj)
{
	if(pObj==NULL)
		return;
	//将列表中的每个对象依次做为主对象
	//其他对象做为主对象的关联对象添加到主对象关联构件列表中
	RELATIVE_OBJECT *pTempItem=NULL,*pRelaObjItem=NULL,*pItem=NULL;
	for(pTempItem=pObj->relativeObjs.GetFirst();pTempItem;pTempItem=pObj->relativeObjs.GetNext())
	{	
		if(pTempItem->hObj==pObj->handle)	//排除自身对称，否则会导致死循环 wjh-2018.6.8
			pObj->relativeObjs.DeleteCursor();
		CLDSDbObject *pRelativeObj=(CLDSDbObject*)Ta.FromHandle(pTempItem->hObj,TRUE);
		if(pRelativeObj==NULL)
			continue;
		//先将基准构件添加当前构件的关联对象列表中
		BOOL bInList=FALSE;
		for(RELATIVE_OBJECT *pOthItem=pRelativeObj->relativeObjs.GetFirst();pOthItem;pOthItem=pRelativeObj->relativeObjs.GetNext())
		{	//避免重复添加关联对象
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
			//调整对称方式
			if(pTempItem->mirInfo.axis_flag==8) //旋转对称
			{
				pRelaObjItem->mirInfo.axis_flag=8;
				pRelaObjItem->mirInfo.rotate_angle=-pTempItem->mirInfo.rotate_angle;
				//pRelaObjItem->mirInfo.axis_flag=4|8;
			}
			else if(pTempItem->mirInfo.axis_flag==12) //Z轴|旋转对称
				pRelaObjItem->mirInfo.axis_flag=8;
		}
		pObj->relativeObjs.push_stack();
		for(pItem=pObj->relativeObjs.GetFirst();pItem;pItem=pObj->relativeObjs.GetNext())
		{	
			if(pItem->hObj==pRelativeObj->handle)
				continue;	//自己和自己不用关联
			bInList=FALSE;
			for(RELATIVE_OBJECT *pOthItem=pRelativeObj->relativeObjs.GetFirst();pOthItem;pOthItem=pRelativeObj->relativeObjs.GetNext())
			{	//避免重复添加关联对象
				if(pOthItem->hObj==pItem->hObj)
				{
					bInList=TRUE;
					break;
				}
			}
			if(bInList)	
				continue;	//列表中已经存在该对象
			if(pTempItem->mirInfo.axis_flag&0x10 || pItem->mirInfo.axis_flag&0x10)
				continue;	//TODO:45度等镜面对称还未实现，现在赋值会导致错误对称信息 wjh-2015.9.16
			RELATIVE_OBJECT *pRelaObjItem=pRelativeObj->relativeObjs.Add(pItem->hObj);
			pRelativeObj->SetModified(TRUE,FALSE);
			pRelaObjItem->mirInfo=pItem->mirInfo;
			//调整对称方式 暂时这样处理，应该有更简单的方法。
			if(pTempItem->mirInfo.axis_flag==1)
			{	//当前操作构件与基准构件对称方式为X轴对称
				//则原来的Y轴对称和Z轴对称互换
				if(pItem->mirInfo.axis_flag&1)		//X轴对称
					pRelaObjItem->mirInfo.axis_flag=pItem->mirInfo.axis_flag^1;
				else if(pItem->mirInfo.axis_flag&2)	//包括Y轴对称，Y换为Z
					pRelaObjItem->mirInfo.axis_flag=(pItem->mirInfo.axis_flag^2)|4;
				else if(pItem->mirInfo.axis_flag&4)	//包括Z轴对称，Z换为Y
					pRelaObjItem->mirInfo.axis_flag=(pItem->mirInfo.axis_flag^4)|2;
				else if(pItem->mirInfo.axis_flag==8)	//旋转对称转化为X轴|旋转对称
					pRelaObjItem->mirInfo.axis_flag=1|8;
			}
			else if(pTempItem->mirInfo.axis_flag==2)
			{	//当前操作构件与基准构件对称方式为Y轴对称
				//则原来的X轴对称和Z轴对称互换
				if(pItem->mirInfo.axis_flag&2)		//Y轴对称
					pRelaObjItem->mirInfo.axis_flag=pItem->mirInfo.axis_flag^2;
				else if(pItem->mirInfo.axis_flag&1)	//包括X轴对称，X换为Z
					pRelaObjItem->mirInfo.axis_flag=(pItem->mirInfo.axis_flag^1)|4;
				else if(pItem->mirInfo.axis_flag&4)	//包括Z轴对称，Z换为X
					pRelaObjItem->mirInfo.axis_flag=(pItem->mirInfo.axis_flag^4)|1;
				else if(pItem->mirInfo.axis_flag==8)	//旋转对称转化为Y轴|旋转对称
					pRelaObjItem->mirInfo.axis_flag=2|8;
			}
			else if(pTempItem->mirInfo.axis_flag==4)
			{	//当前操作构件与基准构件对称方式为Z轴对称
				//则原来的X轴对称和Y轴对称互换
				if(pItem->mirInfo.axis_flag&4)		//Z轴对称
					pRelaObjItem->mirInfo.axis_flag=pItem->mirInfo.axis_flag^4;
				else if(pItem->mirInfo.axis_flag&1)	//包括X轴对称，X换为Y
					pRelaObjItem->mirInfo.axis_flag=(pItem->mirInfo.axis_flag^1)|2;
				else if(pItem->mirInfo.axis_flag&2)	//包括Y轴对称，Y换为X
					pRelaObjItem->mirInfo.axis_flag=(pItem->mirInfo.axis_flag^2)|1;
				else if(pItem->mirInfo.axis_flag==8)	//旋转对称转化为Z轴|旋转对称
					pRelaObjItem->mirInfo.axis_flag=4|8;
			}
			else if(pTempItem->mirInfo.axis_flag==12)
			{	//当前操作构件与基准构件对称方式为：Z轴|旋转对称
				if(pItem->mirInfo.axis_flag==1)	//X轴对称
					pRelaObjItem->mirInfo.axis_flag=2|8;
				else if(pItem->mirInfo.axis_flag==2)//Y轴对称
					pRelaObjItem->mirInfo.axis_flag=1|8;
				if(pItem->mirInfo.axis_flag==9)	//X轴|旋转对称
					pRelaObjItem->mirInfo.axis_flag=1;
				else if(pItem->mirInfo.axis_flag==10)//Y轴|旋转对称
					pRelaObjItem->mirInfo.axis_flag=2;
				else if(pItem->mirInfo.axis_flag==4)//Z轴对称
					pRelaObjItem->mirInfo.axis_flag=4|8;
				else if(pItem->mirInfo.axis_flag==8)//旋转对称
					pRelaObjItem->mirInfo.axis_flag=4;
			}
			else if(pTempItem->mirInfo.axis_flag==8)
			{	//当前操作构件与基准构件对称方式为：旋转对称
				if(pItem->mirInfo.axis_flag==1)	//X轴对称
					pRelaObjItem->mirInfo.axis_flag=1|8;
				else if(pItem->mirInfo.axis_flag==2)//Y轴对称
					pRelaObjItem->mirInfo.axis_flag=2|8;
				if(pItem->mirInfo.axis_flag==9)	//X轴|旋转对称
					pRelaObjItem->mirInfo.axis_flag=2;
				else if(pItem->mirInfo.axis_flag==10)//Y轴|旋转对称
					pRelaObjItem->mirInfo.axis_flag=1;
				else if(pItem->mirInfo.axis_flag==12)//Z轴|旋转对称
					pRelaObjItem->mirInfo.axis_flag=4;
				else if(pItem->mirInfo.axis_flag==4)//Z轴对称
					pRelaObjItem->mirInfo.axis_flag=8;
				else if(pItem->mirInfo.axis_flag==8)
				{
					double angle1=0,angle2=0;
					pRelaObjItem->mirInfo.axis_flag=8;
					//保证旋转角度在0~360之间
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
			{	//当前操作构件与基准构件对称方式为：X轴|旋转对称
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
			{	//当前操作构件与基准构件对称方式为：Y轴|旋转对称
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

//根据对称关系转换节点的依附句柄
void ConvertAttachHandle(CLDSNode* pOriginNode, CLDSNode* pDestNode, MIRMSG mirmsg)
{
	CLDSNode *pTemNode1, *pTemNode2;
	CSuperSmartPtr<CLDSPart> pTemPart1, pTemPart2;
	f3dLine line;
	f3dPoint offset_vec;
	f3dPoint pickPos=pOriginNode->Position();
	if(pDestNode->m_cPosCalType==5)	//比例等分点
	{
		CLDSNode *pStart=Ta.Node.FromHandle(pDestNode->arrRelationNode[0]);
		CLDSNode *pEnd=Ta.Node.FromHandle(pDestNode->arrRelationNode[1]);
		if(pStart&&pEnd)
		{
			line.startPt=pStart->Position(false);
			line.endPt=pEnd->Position(false);
		}
	}
	else if(pDestNode->m_cPosCalType==7)	//偏移点
	{
		CLDSNode *pStart=Ta.Node.FromHandle(pDestNode->arrRelationNode[1]);
		CLDSNode *pEnd=Ta.Node.FromHandle(pDestNode->arrRelationNode[2]);
		if(pStart&&pEnd)
			offset_vec=pEnd->Position(false)-pStart->Position(false);
	}
	if(mirmsg.axis_flag&1||mirmsg.axis_flag&2||mirmsg.axis_flag&4||mirmsg.axis_flag&8||mirmsg.axis_flag&0x10)
	{
		pTemNode1 = Ta.Node.FromHandle(pOriginNode->arrRelationNode[0]);
		if(pTemNode1!=NULL)//父依附节点句柄为0,表示无依附节点
		{
			pTemNode2 = pTemNode1->GetMirNode(mirmsg);//->mir_y_node_handle);
			if(pTemNode2!=NULL)//父依附节点有相应的对称节点时填相应的对称节点
				pDestNode->arrRelationNode[0] = pTemNode2->handle;
			else//父节点的对称节点是自己本身的情况已归并到上一行中 WJH-2004/02/06
				pDestNode->arrRelationNode[0] = 0;//pOriginNode->arrRelationNode[0];
		}
		
		pTemNode1 = Ta.Node.FromHandle(pOriginNode->arrRelationNode[1]);
		if(pTemNode1!=NULL)//父依附节点句柄为0,表示无依附节点
		{
			pTemNode2 = pTemNode1->GetMirNode(mirmsg);//->mir_y_node_handle);
			if(pTemNode2!=NULL)//父依附节点有相应的对称节点时填相应的对称节点
				pDestNode->arrRelationNode[1] = pTemNode2->handle;
			else//父节点的对称节点是自己本身的情况已归并到上一行中 WJH-2004/02/06
				pDestNode->arrRelationNode[1] = 0;//pOriginNode->arrRelationNode[1];
		}
		
		pTemNode1 = Ta.Node.FromHandle(pOriginNode->arrRelationNode[2]);
		if(pTemNode1!=NULL)//父依附节点句柄为0,表示无依附节点
		{
			pTemNode2 = pTemNode1->GetMirNode(mirmsg);//->mir_y_node_handle);
			if(pTemNode2!=NULL)//父依附节点有相应的对称节点时填相应的对称节点
				pDestNode->arrRelationNode[2] = pTemNode2->handle;
			else//父节点的对称节点是自己本身的情况已归并到上一行中 WJH-2004/02/06
				pDestNode->arrRelationNode[2] = 0;//pOriginNode->arrRelationNode[1];
		}
		
		pTemNode1 = Ta.Node.FromHandle(pOriginNode->arrRelationNode[3]);
		if(pTemNode1!=NULL)//父依附节点句柄为0,表示无依附节点
		{
			pTemNode2 = pTemNode1->GetMirNode(mirmsg);//->mir_y_node_handle);
			if(pTemNode2!=NULL)//父依附节点有相应的对称节点时填相应的对称节点
				pDestNode->arrRelationNode[3] = pTemNode2->handle;
			else//父节点的对称节点是自己本身的情况已归并到上一行中 WJH-2004/02/06
				pDestNode->arrRelationNode[3] = 0;//pOriginNode->arrRelationNode[1];
		}
		
		if(pOriginNode->m_cPosCalType==4)
		{
			pTemPart1 = Ta.Parts.FromHandle(pOriginNode->arrRelationPole[0]);
			if(pTemPart1.IsHasPtr()&&pTemPart1->IsLinePart())//父依附角钢句柄为0,表示无依附角钢,退出
			{
				pTemPart2 = pTemPart1.LinePartPointer()->GetMirPart(mirmsg,pickPos);//(CLDSLineAngle*)Ta.FromJgHandle(pTemJg1->hMirPartY);
				if(pTemPart2.IsHasPtr())
					pDestNode->arrRelationPole[0] = pTemPart2->handle;
				//pTemJg2 = pTemJg1;
			}
			pTemPart1 = Ta.Parts.FromHandle(pOriginNode->arrRelationPole[1]);
			if(pTemPart1.IsHasPtr()&&pTemPart1->IsLinePart())//父依附角钢句柄为0,表示无依附角钢,退出
			{
				pTemPart2 = pTemPart1.LinePartPointer()->GetMirPart(mirmsg,pickPos);//Ta.FromJgHandle(pTemJg1->hMirPartY);
				if(pTemPart2.IsHasPtr())
					pDestNode->arrRelationPole[1] = pTemPart2->handle;
				//pTemJg2=pTemJg1;
			}
		}
		else if(pDestNode->m_cPosCalType==5)	//比例等分点
		{
			CLDSNode *pStart=Ta.Node.FromHandle(pDestNode->arrRelationNode[0]);
			CLDSNode *pEnd=Ta.Node.FromHandle(pDestNode->arrRelationNode[1]);
			pDestNode->attach_scale=pOriginNode->attach_scale;
			if(pStart&&pEnd&&line.endPt.IsEqual(pStart->Position(false))&&line.startPt.IsEqual(pEnd->Position(false)))
				pDestNode->attach_scale=1-pDestNode->attach_scale;	//对称后依附节点位置调换
		}
		else if(pDestNode->m_cPosCalType==7)	//偏移点
		{
			CLDSNode *pStart=Ta.Node.FromHandle(pDestNode->arrRelationNode[1]);
			CLDSNode *pEnd=Ta.Node.FromHandle(pDestNode->arrRelationNode[2]);
			pDestNode->attach_offset=pOriginNode->attach_offset;
			f3dPoint vec;
			if(pStart&&pEnd)
				vec=pEnd->Position(false)-pStart->Position(false);
			if(vec*offset_vec<0)	//对称后依附节点算出的偏移方向反向
				pDestNode->attach_offset*=-1.0;
		}
		else if(pDestNode->m_cPosCalType>=8&&pDestNode->m_cPosCalType<=10)	//基准面上指定坐标分量点
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
	if(mirmsg.axis_flag&2)		//Y轴对称
	{
		mir_point = GetMirPos(lineStart,mirmsg.GetSubMirItem(2));
		if(DISTANCE(lineEnd,mir_point)<EPS2)
			return FALSE;
	}
	if(mirmsg.axis_flag&1)		//X轴对称
	{
		mir_point = GetMirPos(lineStart,mirmsg.GetSubMirItem(1));
		if(DISTANCE(lineEnd,mir_point)<EPS2)
			return FALSE;
	}
	if(mirmsg.axis_flag&4)		//Z轴对称
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
	f3dPoint norminal_vec,offset_vec;	//用于记录原偏移点依附角钢的正偏移方向
	f3dPoint mir_offset_vec;//用于记录原对称偏移点依附角钢的正偏移方向
	CLDSNode *pMirNode=NULL, *pAttachNode=NULL, *pMirAttachNode=NULL;
	CLDSPart *pPart;
	CLDSLinePart *pMirPart=NULL,*pAttachPart=NULL,*pMirAttachPart=NULL;
	CLDSLinePart* pFatherPart = NULL;

	if(bIncAttachInfo&&!IsNodeMirValid(pSrcNode,msg))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "fail to pass symmetry legality checking, parts can not be symmetrical";
#else
		throw "未通过对称合法性检测,不能对称构件";
#endif

	if(pSrcNode->hFatherPart>=0x20)
		pFatherPart = (CLDSLinePart*)Ta.Parts.FromHandle(pSrcNode->hFatherPart);
	if(pSrcNode->m_cPosCalType==7)	//偏移节点以节点父构件为依附构件
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
			throw "原始节点距离对称轴太近,不能进行相应轴对称";
#endif
		pMirNode = console.AppendNode();
		pMirNode->SetPosition(mirpos);
		pMirNode->CopyProperty(pSrcNode);//拷贝父节点(部分)属性
		if(msg.axis_flag==8&&ftoi(msg.rotate_angle/90)%2!=0&&pSrcNode->m_cPosCalType==1)
			pMirNode->m_cPosCalType=2;	//旋转90度对称时X和Y分量调换
		else if(msg.axis_flag==8&&ftoi(msg.rotate_angle/90)%2!=0&&pSrcNode->m_cPosCalType==2)
			pMirNode->m_cPosCalType=1;	//旋转90度对称时X和Y分量调换
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
		ConvertAttachHandle(pSrcNode, pMirNode, msg);//根据对称关系转换节点的依附句柄

	pMirNode->SetArcLift(pSrcNode->ArcLiftHandle());
	if(pSrcNode->ArcLiftHandle()>0x20)
		pMirNode->Set();
	//添加对称生成的对象到源对象的关联对象列表
	AddMirObjToSrcObjRelaObjList(pSrcNode,pMirNode,msg);
	return pMirNode;
	/*
	由于偏移点的偏移方向不再以基准杆件始至终方向为基准，而是以第一个基准点到
	第二个基准点为基准，不再需要判断偏移方向是否反向，故以下代码停止使用 wjh-2009-11-11
	if(pMirNode->m_cPosCalType==7)	//偏移点
	{
	if(pAttachPart==NULL||pMirAttachPart==NULL)
	throw "未通过对称合法性检测,不能对称构件";
	offset_vec=pAttachPart->End()-pAttachPart->Start();
	offset_vec=GetMirVector(offset_vec,msg);
	mir_offset_vec=pMirAttachPart->End()-pMirAttachPart->Start();
	if(offset_vec*mir_offset_vec<0)	//对称后偏移方向不一致
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
		else	//直接添加厚度
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
		else	//直接添加厚度
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
	if(mirPoint.datum_pos_style==1||mirPoint.datum_pos_style==7)	//角钢楞点
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
		{	//对称时智能识别典型偏移类型(g,g1,g2,g3) wjh-2015.3.7
			JGZJ defaultZJ;
			if(srcPoint.des_para.RODEND.offset_wing==0)	//X肢为当前肢
				defaultZJ=pAngle->GetZhunJu('X');
			else
				defaultZJ=pAngle->GetZhunJu('Y');
			srcPoint.des_para.RODEND.wing_offset_style=defaultZJ.MatchStyleG(srcPoint.des_para.RODEND.wing_offset_dist,false);
			/** 未考虑g1=0且wing_offset_dist=0情况，此时会错判成1,在后续更改角钢规格时带来问题　wjh-2018.1.4
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
	else if(mirPoint.datum_pos_style==2)	//节点向角钢心线投影或平推
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
		{	//对称时智能识别典型偏移类型(g,g1,g2,g3) wjh-2015.3.7
			JGZJ defaultZJ;
			if(srcPoint.des_para.RODNODE.offset_wing==0)	//X肢为当前肢
				defaultZJ=pAngle->GetZhunJu('X');
			else
				defaultZJ=pAngle->GetZhunJu('Y');
			srcPoint.des_para.RODNODE.wing_offset_style=defaultZJ.MatchStyleG(srcPoint.des_para.RODNODE.wing_offset_dist,false);
			/** 未考虑g1=0且wing_offset_dist=0情况，此时会错判成1,在后续更改角钢规格时带来问题　wjh-2018.1.4
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
	else if(mirPoint.datum_pos_style==3)	//心线交点
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
	else if(mirPoint.datum_pos_style==6)	//基准面上指定坐标分量节点
	{
		if(msg.axis_flag==0x01||msg.axis_flag==0x02||msg.axis_flag==0x04)
		{
			mirPoint.datum_pos_style=srcPoint.datum_pos_style;
			mirPoint.des_para.DATUMPLANE.hDatumPlane=srcPoint.des_para.DATUMPLANE.hDatumPlane;
			mirPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=srcPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2;
			//对称基准面定位的定位参数信息
			if(msg.axis_flag==0x01)	//X轴对称
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
			else if(msg.axis_flag==0x02)	//Y轴对称
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
			else if(msg.axis_flag==0x04)	//Z轴对称
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
	else if(mirPoint.datum_pos_style==8)	//节点与基点同位
	{
		MirPartHandle(pTower,&mirPoint.des_para.hNode,srcPoint.des_para.hNode,msg);
	}
	else if(mirPoint.datum_pos_style==9)	//基准构件上的相对坐标
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
	else if(mirPoint.datum_pos_style==10)	//两节点间的比例等分点 wht 12-03-19
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
	{	//柱面射线定位点
		MirPartHandle(pTower,&mirPoint.des_para.COLUMN_INTERS.hLineTube,srcPoint.des_para.COLUMN_INTERS.hLineTube,msg);
			//自低到高指定位为1表示对应矢量或点为参数化点或方向。
			//第1位:axis_vec;
			//第2位:sect_center;
			//第3位:sect_norm;
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
		{	//从指定钢管起始边或焊道线开始，并根据ray_angle确定射线方向
			pTube->BuildUCS();
			f3dPoint mir_ray_vec=mirPoint.des_para.COLUMN_INTERS.RayVector.ray_vec;
			//angle=CalRotateAngle(pTube->ucs.axis_z,pTube->ucs.axis_x,mir_ray_vec);
			//使用CalRotateAngle计算角度错误导致槽钢对称错误 wht 18-12-29
			vector_trans(mir_ray_vec,pTube->ucs,FALSE);
			angle=Cal2dLineAng(0,0,mir_ray_vec.x,mir_ray_vec.y);
			double weldRoadAngle=0;
			if(mirPoint.des_para.COLUMN_INTERS.ray_vec_style==3)	//钢管焊道线为柱面起始边)
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
	srcVector.UpdateVector(pTower);		//srcVector未计算前进行对称，可能导致当前肢判断错误 wht 16-09-09
	mirVector.direction=srcVector.direction;
	mirVector.vector=GetMirVector(srcVector.vector,msg);
	mirVector.norm_style=srcVector.norm_style;
	MirPartHandle(pTower,&mirVector.hVicePart,srcVector.hVicePart,msg);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	//未找到对称杆件时尝试启用钢管一端对称,查找对称构件 wht 11-05-24
	CLDSLineTube *pViceTube=(CLDSLineTube*)pTower->FromPartHandle(srcVector.hVicePart,CLS_LINETUBE);
	MirPartHandle(pTower,&mirVector.hVicePart,srcVector.hVicePart,msg,pPickPos);
#endif
	if(mirVector.norm_style==2)
	{	//杆件交叉面法线
		MirPartHandle(pTower,&mirVector.hCrossPart,srcVector.hCrossPart,msg,pPickPos);
		mirVector.nearVector=GetMirVector(srcVector.nearVector,msg);
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(mirVector.norm_style==1)
	{	//角钢肢法线
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
	{	//基准构件上的相对方向
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
	{	//基准杆件上的特征方向 wht 11-05-17
		CLDSLineAngle *pMirAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(mirVector.hVicePart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pMirAngle)
		{
			double ddx=pMirAngle->get_norm_x_wing()*mirVector.vector;
			double ddy=pMirAngle->get_norm_y_wing()*mirVector.vector;
			if(fabs(ddx)>fabs(ddy))
			{
				if(ddx>0)
					mirVector.direction=2;	//角钢X肢正方向
				else
					mirVector.direction=3;	//角钢X肢负方向
				mirVector.norm_wing=0;
			}
			else
			{
				if(ddy>0)
					mirVector.direction=4;	//角钢Y肢正方向
				else
					mirVector.direction=5;	//角钢Y肢负方向
				mirVector.norm_wing=1;
			}
		}
	}
#endif
}

static BOOL MirNode(CLDSNode *pSrcNode, MIRMSG mirmsg,BOOL bIncAttachInfo=TRUE,char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//是否成功对称生成所有构件
	RELATIVE_OBJECT *pRelaObj=NULL;
	if(bIncAttachInfo&&!IsNodeMirValid(pSrcNode,mirmsg))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "fail to pass symmetry legality checking, parts can not be symmetrical";
#else
		throw "未通过对称合法性检测,不能对称构件";
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
	if((mirmsg.axis_flag&8)==0)	//无旋转对称
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
		{	//进行多次旋转对称时基准构件为同一个构件，只需要调整旋转角度即可
			if(pSrcNode==NULL)
				continue;
			//调整旋转角度
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSNode *pMirNode=CreateSingleMirNode(pSrcNode,mirmsg,bIncAttachInfo);
			if(pMirNode)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcnode_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcnode_arr[0]->relativeObjs.GetNext())
				{	//避免重复添加关联对象
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
	//设置对称生成的对象彼此关联
	SetRelativeObj(srcnode_arr[0]);
#if defined(__TSA_)||defined(__LDS_)
	if(bIncAttachInfo)
	{
		for(CLDSNode* pNode=newnodeset.GetFirst();pNode;pNode=newnodeset.GetNext())
		{
			if(pNode->m_cPosCalType==4)	//交叉节点对称时需要考虑将长细比参照杆件也进行更新
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
	if(pMirBolt)	//对称螺栓已经生成
	{
		if (pMirObj==NULL)
		{	//对称操作前已存在对称螺栓对象，无须更新螺栓相关信息，仅进行关联即可 wjh-2020.1.14
			CMirOperObject::MIRROR::MIROBJ mirobj;
			mirobj.ciMirFlag=msg.axis_flag;
			mirobj.siRotDegAng=msg.rotate_angle;
			mirobj.dwStateFlag=CMirOperObject::MIRBOLT_ALL;
			mirobj.pMirObj=pMirBolt;
			pMirObj=pMirror->xarrMirObjs.Append(mirobj);
		}
		if(pBolt->IsFootNail()&&!pMirBolt->IsFootNail()//源螺栓为脚钉，对称螺栓非脚钉
			&&pBolt->get_d()==pMirBolt->get_d())		//两螺栓直径相等
		{	//原螺栓为脚钉，对称螺栓非脚钉时调整对称螺栓为脚钉 wht 11-08-09
			pMirBolt->PushInitialStdBoltL();
			pMirBolt->set_L(pBolt->get_L());
			pMirBolt->m_hFamily = CLsLibrary::GetFootNailFamily();
			pMirBolt->m_dwFlag.SetFlagDword(CLDSBolt::FOOT_NAIL,TRUE);
			pMirBolt->m_cFuncType=pBolt->m_cFuncType;	//拷贝特殊孔类型
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
		pMirBolt->CopyProperty(pBolt,pBolt->liInitL>0);	//新增螺栓若非代孔脚钉，对称螺栓也不可能是代孔脚钉
		pMirBolt->cfgword=pBolt->cfgword;
		pMirBolt->ucs.origin=GetMirPos(pBolt->ucs.origin,msg);
		pMirBolt->ucs.axis_z=GetMirVector(pBolt->ucs.axis_z,msg);
		AddMirObjToSrcObjRelaObjList(pBolt,pMirBolt,msg);	//添加对称生成的对象到源对象的关联对象列表
		
		if (pMirObj==NULL)
		{	//新对称螺栓对象
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
	//法线设计信息对称
	pMirBolt->des_work_norm=pBolt->des_work_norm;
	MirPartHandle(pBolt->BelongModel(),&pMirBolt->des_work_norm.hVicePart,pBolt->des_work_norm.hVicePart,msg);
	MirPartHandle(pBolt->BelongModel(),&pMirBolt->des_work_norm.hCrossPart,pBolt->des_work_norm.hCrossPart,msg);
	//以下代码暂时注释掉 在未找到螺栓法线设计基准构件时
	//不应使用对称螺栓的法线设计参数，否则螺栓法线不正确 wht 10-03-17
	//if(pMirBolt->des_work_norm.hVicePart==0)
	//	pMirBolt->des_work_norm.hVicePart=pBolt->des_work_norm.hVicePart;
	//if(pMirBolt->des_work_norm.hCrossPart==0)
	//	pMirBolt->des_work_norm.hCrossPart=pBolt->des_work_norm.hCrossPart;
	pMirBolt->des_work_norm.nearVector=GetMirVector(pBolt->des_work_norm.nearVector,msg);
	if (pBolt->des_work_norm.norm_style==4)	//基准构件上的相对方向
	{
		pMirBolt->des_work_norm.nearVector=pBolt->des_work_norm.nearVector;
		if(pMirBolt->des_work_norm.hVicePart>0x20)
			pMirObj->dwStateFlag|=CMirOperObject::MIRBOLT_NORM_PARAM;
	}
	else if(pBolt->des_work_norm.norm_style==1)	//角钢肢法线
	{
		CLDSLineAngle *pMirAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirBolt->des_work_norm.hVicePart);
		if(pMirAngle)
		{	//存在基准角钢 wht 10-10-19
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
			if(pMirBolt->des_work_norm.direction==0)	//朝外
				pMirBolt->des_base_pos.norm_offset.AddThick(-pMirBolt->des_base_pos.hPart,TRUE);
		}
	}
	//基点位置设计信息对称
	pMirBolt->des_base_pos=pBolt->des_base_pos;
	pMirObj->dwStateFlag|=CMirOperObject::MIRBOLT_POS_PARAM;
	if (pBolt->des_base_pos.datumPoint.MirToPoint(pBolt->BelongModel(),pMirBolt->des_base_pos.datumPoint,msg)<0)
	{
		logerr.Log("0x%X螺栓的定位基点对称存在错误",pBolt->handle);
		pMirObj->RemoveState(CMirOperObject::MIRBOLT_POS_PARAM);
	}
	MirPartHandle(pBolt->BelongModel(),&pMirBolt->des_base_pos.hPart,pBolt->des_base_pos.hPart,msg);
	//以下代码暂时注释掉 在未找到螺栓位置设计基准构件时
	//不应使用对称螺栓的位置设计参数，否则螺栓位置会不正确  wht 10-03-17
	//if(pMirBolt->des_base_pos.hPart==0)
	//	pMirBolt->des_base_pos.hPart=pBolt->des_base_pos.hPart;
	CSuperSmartPtr<CLDSPart> pDatumPart=Ta.Parts.FromHandle(pBolt->des_base_pos.hPart);
	CSuperSmartPtr<CLDSPart> pMirDatumPart=Ta.Parts.FromHandle(pMirBolt->des_base_pos.hPart);
	if(pDatumPart.IsHasPtr()&&pMirDatumPart.IsHasPtr())
	{
		if(pDatumPart->IsLinePart())
		{	//主要指角钢构件
			f3dPoint line_vec=pDatumPart.LinePartPointer()->End()-pDatumPart.LinePartPointer()->Start();
			f3dPoint mir_line_vec=GetMirVector(line_vec,msg);
			line_vec=pMirDatumPart.LinePartPointer()->End()-pMirDatumPart.LinePartPointer()->Start();
			if(line_vec*mir_line_vec<0)
				pMirBolt->des_base_pos.direction=1-pMirBolt->des_base_pos.direction;
		}
		else if(pDatumPart->IsPlate())
		{	//主要指钢板构件
			f3dPoint mir_axis_x=GetMirVector(pDatumPart->ucs.axis_x,msg);
			f3dPoint mir_axis_y=GetMirVector(pDatumPart->ucs.axis_y,msg);
			//f3dPoint mir_axis_z=GetMirVector(pDatumPart->ucs.axis_z,msg);//暂不处理Ｚ轴反向情况
			if(mir_axis_x*pMirDatumPart->ucs.axis_x<0)	//对称后的Ｘ轴已进行了反向
				pMirBolt->des_base_pos.len_offset_dist*=-1.0;
			if(mir_axis_y*pMirDatumPart->ucs.axis_y<0)	//对称后的Ｙ轴已进行了反向
				pMirBolt->des_base_pos.wing_offset_dist*=-1.0;
		}
	}
	else
		pMirObj->RemoveState(CMirOperObject::MIRBOLT_POS_PARAM);
	if(pMirDatumPart.IsHasPtr()&&pMirDatumPart->GetClassTypeId()==CLS_LINEANGLE)
	{	//在螺栓肢法线设计参数为指定法线时，智能判断螺栓在角钢上的当前工作肢
		if( fabs(pMirDatumPart.LineAnglePointer()->get_norm_x_wing()*pMirBolt->get_norm())>
			fabs(pMirDatumPart.LineAnglePointer()->get_norm_y_wing()*pMirBolt->get_norm()))
			pMirBolt->des_base_pos.offset_wing=0;
		else
			pMirBolt->des_base_pos.offset_wing=1;
	}
	else
		pMirBolt->des_base_pos.offset_wing=(BYTE)pMirBolt->des_work_norm.norm_wing;
	//上面有相同代码行，此行因重复而被注释掉 wjh-2015.10.8
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
	//增加外界环境上下文传过来的关联对象集合　wjh-2015.3.7
	for(UINT i=0;relaPartIdArr!=NULL&&i<nRelaPartCount;i++)
	{
		pPart=pBolt->BelongModel()->FromPartHandle(relaPartIdArr[i]);
		if(pPart)
			hashRelaParts.SetValue(pPart->handle,pPart);
	}
	//根据通厚字符串提取可能的关联对象集合
	if(hashRelaParts.GetValue(pBolt->des_base_pos.hPart)==NULL)
	{
		pPart=pBolt->BelongModel()->FromPartHandle(pBolt->des_base_pos.hPart);
		if(pPart)
			hashRelaParts.SetValue(pPart->handle,pPart);
	}
	CParaThick* pThick=pBolt->GetL0ThickPara();
	for(THICK_ITEM item=pThick->GetFirstItem();!item.IsFinalKey();item=pThick->GetNextItem())
	{
		if(item.cItemType!=1)	//不是句柄
			continue;
		pPart=pBolt->BelongModel()->FromPartHandle(item.val);
		if(pPart)
			hashRelaParts.SetValue(pPart->handle,pPart);
	}
	//对称螺栓引用  首先找到螺栓相关联的构件,然后将螺栓引入到对应的关联构件上
	//if(pMirBolt->IsFootNail())
	for(pPart=hashRelaParts.GetFirst();pPart!=NULL;pPart=hashRelaParts.GetNext())
	{	
		if(pPart->GetClassTypeId()==CLS_LINEANGLE&&pPart->GetLsRefList()->FromHandle(pBolt->handle))
		{
			CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pPart;
			CLDSLineAngle* pMirAngle=(CLDSLineAngle*)pLineAngle->GetMirPart(msg,pBolt->ucs.origin);
			if(pMirAngle==NULL)
				continue;
			//默认螺栓添加到角钢中间位（虽然不一定对），否则可能会导致端点的单螺栓设计失败 wjh-2015.8.13
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
	//添加对称生成的对象到源对象的关联对象列表
	AddMirObjToSrcObjRelaObjList(pBolt,pMirBolt,msg);
	return pMirBolt;
}
BOOL MirBolt(CLDSBolt* pSrcBolt, MIRMSG mirmsg, BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//是否成功对称生成所有构件
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
	if((mirmsg.axis_flag&8)==0)	//无旋转对称
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
		{	//进行多次旋转对称时基准构件为同一个构件，只需要调整旋转角度即可
			if(pSrcBolt==NULL)
				continue;
			//调整旋转角度
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSBolt *pMirBolt=CreateSingleMirBolt(pSrcBolt,mirmsg,bIncAttachInfo);
			if(pMirBolt)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcbolt_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcbolt_arr[0]->relativeObjs.GetNext())
				{	//避免重复添加关联对象
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
	//设置对称生成的对象彼此关联
	SetRelativeObj(srcbolt_arr[0]);
	//对于先进行一次轴对称再进行一次旋转对称而生成的螺栓，
	//只能分别记录下两次对称
	return bSucceed;
}
#endif
static CLDSLinePart *CreateSingleMirLinePart(CLDSLinePart *pSrcLinePart, MIRMSG msg,BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	CLDSLinePart *pMirLinePart=NULL;
	pMirLinePart=(CLDSLinePart*)pSrcLinePart->GetMirPart(msg);
	if(pMirLinePart==NULL)	//对称杆件未发现
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
		pMirLinePart->CopyProperty(pSrcLinePart);//拷贝父节点(部分)属性
		AddMirObjToSrcObjRelaObjList(pSrcLinePart,pMirLinePart,msg);	//添加对称生成的对象到源对象的关联对象列表
	}
	CalMirAtomLayer(pSrcLinePart->layer(),pMirLinePart->layer(),msg);
	if(!bIncAttachInfo)
		return pMirLinePart;
	//------vvvvvvvvv-------螺栓规格--------------------
	pMirLinePart->connectStart.d = pSrcLinePart->connectStart.d;
	pMirLinePart->connectStart.rows = pSrcLinePart->connectStart.rows;//缺省单排螺栓
	pMirLinePart->connectStart.uiLegacyN = pSrcLinePart->connectStart.uiLegacyN;
	pMirLinePart->connectEnd.d = pSrcLinePart->connectEnd.d;
	pMirLinePart->connectEnd.rows = pSrcLinePart->connectEnd.rows;//缺省单排螺栓
	pMirLinePart->connectEnd.uiLegacyN = pSrcLinePart->connectEnd.uiLegacyN;
	//------^^^^^^^^^-------螺栓规格--------------------
	NewLinePart(pMirLinePart);
	g_pSolidDraw->Draw();
	//添加对称生成的对象到源对象的关联对象列表
	AddMirObjToSrcObjRelaObjList(pSrcLinePart,pMirLinePart,msg);
	return pMirLinePart;
}
static BOOL MirLinePart(CLDSLinePart* pSrcLinePart, MIRMSG mirmsg,BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//是否成功对称生成所有构件
	RELATIVE_OBJECT *pRelaObj=NULL;
	if(!IsPartMirValid(pSrcLinePart,mirmsg))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "fail to pass symmetry legality checking, parts can not be symmetrical";
#else
		throw "未通过对称合法性检测,不能对称构件";
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
	if((mirmsg.axis_flag&8)==0)	//无旋转对称
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
		{	//进行多次旋转对称时基准构件为同一个构件，只需要调整旋转角度即可
			if(pSrcLinePart==NULL)
				continue;
			//调整旋转角度
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSLinePart *pMirLinePart=CreateSingleMirLinePart(pSrcLinePart,mirmsg,bIncAttachInfo);
			if(pMirLinePart)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcpole_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcpole_arr[0]->relativeObjs.GetNext())
				{	//避免重复添加关联对象
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
	//设置对称生成的对象彼此关联
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
	if(bMirStart&&bSrcStart)	//对称杆件是自己本身
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
		return;	//始/终端位置锁定时不能调整始/终端位置
	if(bSrcStart)
		pSrcPosDesign=&pSrcAngle->desStartPos;
	else
		pSrcPosDesign=&pSrcAngle->desEndPos;
	if(bMirStart)
		pMirPosDesign=&pMirAngle->desStartPos;
	else
		pMirPosDesign=&pMirAngle->desEndPos;
	//首先复制肢偏移方式以及肢法向偏移量到对称角钢。然后再根据不同类型做相应调整
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
			//直接指定基点坐标	对称变坡处双面板主材基点定位方式 wht 09-04-23
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
			//对称连接材在搭接基准材上的基点定位信息
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
		//基点到楞点的定位方式对称角钢一般是相同值
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
				break;	//非十字组合角钢
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
	if(pMirAngle==NULL)	//对称角钢未找到
	{
		pMirAngle=(CLDSLineAngle*)console.AppendPart(pSrcAngle->GetClassTypeId());
		pMirAngle->set_wing_wide(pSrcAngle->size_wide);
		pMirAngle->set_wing_thick(pSrcAngle->size_thick);
		if(pSrcAngle->IsUnequalAngle())	//不等边角钢
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
		pMirAngle->CopyProperty(pSrcAngle);//拷贝父节点(部分)属性
		pMirAngle->bHuoQuStart=pSrcAngle->bHuoQuStart;
		pMirAngle->bHuoQuEnd=pSrcAngle->bHuoQuEnd;
		if(pSrcAngle->m_bEnableTeG)
		{	//特殊准距 wht 10-06-06
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
		//由于角钢对称函数已增加了肢法线的对称条件，因此此时对称角钢实体时应考虑肢法线的对称 wjh-2015.2.1
		f3dPoint wing_x_norm=pSrcAngle->get_norm_x_wing();
		f3dPoint wing_y_norm=pSrcAngle->get_norm_y_wing();
		if(msg.axis_flag==0x01||msg.axis_flag==0x02||msg.axis_flag==0x10)
		{	//X、Y轴对称或面对称时默认需要调转X与Y肢
			//工程实践中看对于默认保持原朝向的情况，如果对称目标位置无角钢时，X/Y及面对称时也应调换X与Y肢 wjh-2018.1.28
			if(pMirAngle->MirLayStyle==CLDSLineAngle::MIRXorY_SAMEDIRECT||pMirAngle->MirLayStyle==CLDSLineAngle::MIRXorY_UNCHAHNGE)
			{	//里外同向，需要调转X与Y肢
				wing_x_norm=GetMirVector(pSrcAngle->get_norm_y_wing(),msg);
				wing_y_norm=GetMirVector(pSrcAngle->get_norm_x_wing(),msg);
			}
			else //if(pMirAngle->MirLayStyle==CLDSLineAngle::MIRXorY_INVERT||MirLayStyle==CLDSLineAngle::MIRXorY_UNCHAHNGE)
			{	//里外反向或保留原有里外铁，不需要调转X与Y肢
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
		AddMirObjToSrcObjRelaObjList(pSrcAngle,pMirAngle,msg);	//添加对称生成的对象到源对象的关联对象列表
	}
	CalMirAtomLayer(pSrcAngle->layer(),pMirAngle->layer(),msg);
	if(!bIncAttachInfo)
		return pMirAngle;
	pSrcAngle->SetModified();
	//添加对称生成的对象到源对象的关联对象列表
	AddMirObjToSrcObjRelaObjList(pSrcAngle,pMirAngle,msg);
	if(pMirAngle->handle<pSrcAngle->feature)
		return pMirAngle;	//原有构件不对称以下附加信息,否则极易破坏原有数据导致角钢法线或位置错误 wjh 2011.8.12
	//------vvvvvvvvv-------螺栓规格--------------------
	pMirAngle->connectStart = pSrcAngle->connectStart;
	pMirAngle->connectEnd = pSrcAngle->connectEnd;
	//pMirAngle->connectStart.d = pSrcAngle->connectStart.d;
	//pMirAngle->connectStart.rows = pSrcAngle->connectStart.rows;//缺省单排螺栓
	//pMirAngle->connectStart.N = pSrcAngle->connectStart.N;
	//pMirAngle->connectEnd.d = pSrcAngle->connectEnd.d;
	//pMirAngle->connectEnd.rows = pSrcAngle->connectEnd.rows;//缺省单排螺栓
	//pMirAngle->connectEnd.N = pSrcAngle->connectEnd.N;
	//------^^^^^^^^^-------螺栓规格--------------------
	CSuperSmartPtr<CLDSPart> pRefPart,pMirRefPart;
	f3dPoint mirPickPos;	//对称肢法线计算参照杆件时的拾取点，其值对结果影响不大，但必须输入否则不允许部分对称杆件wjh-2014.5.4
	if(pSrcAngle->pStart)
		mirPickPos=pSrcAngle->pStart->Position();
	//X肢法线
	pRefPart=Ta.Parts.FromHandle(pSrcAngle->des_norm_x.hViceJg);
	if(pRefPart.IsHasPtr()&&pRefPart->IsLinePart())
	{
		pMirRefPart=pRefPart.LinePartPointer()->GetMirPart(msg,mirPickPos);
		if(pMirRefPart.IsHasPtr())
			pMirAngle->des_norm_x.hViceJg=pMirRefPart->handle;
	}
	if(!pSrcAngle->IsSyncMirPropWith(pMirAngle,"wingNormInfo") || !pSrcAngle->SyncMirPropTo(pMirAngle,msg,"wingNormInfo"))
	{	//优先通过更新对称对象功能函数进行肢法线方向对称 wjh-2015.6.26
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
			if(pMirAngle->des_norm_x.spec_norm.norm_style==3	//构件工作法线
				||pMirAngle->des_norm_x.spec_norm.norm_style==4)//基准构件上的相对方向
				pMirRefPart=pRefPart->GetMirPart(msg);
			else if(pRefPart->IsLinePart())
				pMirRefPart=pRefPart.LinePartPointer()->GetMirPart(msg,mirPickPos);
			if(pMirRefPart.IsHasPtr())
			{
				pMirAngle->des_norm_x.spec_norm.hVicePart=pMirRefPart->handle;
				if(pMirRefPart->GetClassTypeId()==CLS_LINEANGLE
					&&pMirAngle->des_norm_x.spec_norm.norm_style==1)	
				{	//指定角钢肢法线 纠正角钢肢法线计算参数 wht 10-09-30
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
		//Y肢法线
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
			if(pMirAngle->des_norm_y.spec_norm.norm_style==3	//构件工作法线
				||pMirAngle->des_norm_y.spec_norm.norm_style==4)//基准构件上的相对方向
				pMirRefPart=pRefPart->GetMirPart(msg);
			else if(pRefPart->IsLinePart())
				pMirRefPart=pRefPart.LinePartPointer()->GetMirPart(msg,mirPickPos);
			if(pMirRefPart.IsHasPtr())
			{
				pMirAngle->des_norm_y.spec_norm.hVicePart=pMirRefPart->handle;
				if(pMirRefPart->GetClassTypeId()==CLS_LINEANGLE
					&&pMirAngle->des_norm_y.spec_norm.norm_style==1)
				{	//指定角钢肢法线 纠正角钢肢法线计算参数 wht 10-09-30	
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
		//对于塔身斜材及其他斜材的法线设计参数按行业习惯进行修正
		//if(msg.axis_flag&0x01||msg.axis_flag&0x02)
		if(msg.axis_flag==0x01||msg.axis_flag==0x02||msg.axis_flag==0x10)
		{
			//if((pMirAngle->layer(1)=='X'&&pMirAngle->Layer(0)!='L')||(pMirAngle->layer(0)=='S'&&pMirAngle->layer(1)=='F'))
			if(pSrcAngle->MirLayStyle==CLDSLineAngle::MIRXorY_INVERT)
			{	//里外铁对称
				if(!pMirAngle->des_norm_x.bByOtherWing)	//参照杆件计算的肢为确定里外铁的基准肢
					pMirAngle->des_norm_x.near_norm*=-1;
				if(!pMirAngle->des_norm_y.bByOtherWing)
					pMirAngle->des_norm_y.near_norm*=-1;
			}
			else
			{	//同里外铁对称
				DESIGN_JGWING_NORM des_wing_norm=pMirAngle->des_norm_x;
				pMirAngle->des_norm_x=pMirAngle->des_norm_y;
				pMirAngle->des_norm_y=des_wing_norm;
			}
		}
	}

	pMirAngle->ClearFlag();
	pMirAngle->cal_x_wing_norm();
	pMirAngle->cal_y_wing_norm();
	//调整完肢法向后，再更新正负头参数（会根据肢法向进行切角操作）
	MirLinePartOddDesignInfo(pSrcAngle,pMirAngle,msg,true,true);
	MirLinePartOddDesignInfo(pSrcAngle,pMirAngle,msg,false,false);
	if(pMirAngle!=pSrcAngle)
	{
		//始端楞点位置设计
		pMirAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2=pSrcAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2;
			//楞点计算方式
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
			//基点计算方式
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
		{	//塔在基准角钢的X肢上
			if(pMirAngle->desStartPos.spatialOperationStyle==3||
				pMirAngle->desStartPos.spatialOperationStyle==6||
				pMirAngle->desStartPos.spatialOperationStyle==9||
				pMirAngle->desStartPos.spatialOperationStyle==12||
				pMirAngle->desStartPos.spatialOperationStyle==14)
				pMirAngle->desStartPos.spatialOperationStyle-=1;
		}
		else if(datum_wing_x0_y1==1)
		{	//搭在基准角钢的Y肢上
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
		wing_x0_y1=pMirAngle->GetStartWorkWing();	//必须置于MirDatumPoint之后,因为desStartPos.datumPoint影响GetStartWorkWing wjh-2016.4.25
			//自基点至楞点的偏移方式
		//pMirAngle->desStartPos.datum_to_ber_style=pSrcAngle->desStartPos.datum_to_ber_style;
		if( (msg.axis_flag==0x01||msg.axis_flag==0x02||msg.axis_flag==0x10)&&
			(pSrcAngle->MirLayStyle!=CLDSLineAngle::MIRXorY_INVERT))
		{	//X/Y对称里外铁同向,需调换两肢偏移参数
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
		if(pDatumPart.IsHasPtr()&&!pDatumPart.IsEqualPtr(pMirAngle)&&//根据肢摆方方式进行修正
			!(pMirAngle->desStartPos.wing_x_offset.gStyle==4&&pMirAngle->desStartPos.wing_x_offset.offsetDist==0&&
			 pMirAngle->desStartPos.wing_y_offset.gStyle==4&&pMirAngle->desStartPos.wing_y_offset.offsetDist==0))
		{	//原本都是自定义偏移量时,如偏移量为零时,不修正 wjh-2011.8.28
			if(wing_x0_y1==0)
			{	//自身X肢为搭接肢
				pMirAngle->desStartPos.wing_x_offset.gStyle=4;
				pMirAngle->desStartPos.wing_x_offset.offsetDist=0;
				pMirAngle->desStartPos.wing_y_offset.gStyle=0;
			}
			else if(wing_x0_y1==1)
			{	//自身Y肢为搭接肢
				pMirAngle->desStartPos.wing_x_offset.gStyle=0;
				pMirAngle->desStartPos.wing_y_offset.gStyle=4;
				pMirAngle->desStartPos.wing_y_offset.offsetDist=0;
			}
		}
		//终端楞点位置设计
		pMirAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2=pSrcAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2;
			//楞点计算方式
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
			//基点计算方式
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
		{	//塔在基准角钢的X肢上
			if(pMirAngle->desEndPos.spatialOperationStyle==3||
				pMirAngle->desEndPos.spatialOperationStyle==6||
				pMirAngle->desEndPos.spatialOperationStyle==9||
				pMirAngle->desEndPos.spatialOperationStyle==12||
				pMirAngle->desEndPos.spatialOperationStyle==14)
				pMirAngle->desEndPos.spatialOperationStyle-=1;
		}
		else if(datum_wing_x0_y1==1)
		{	//搭在基准角钢的Y肢上
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
		wing_x0_y1=pMirAngle->GetEndWorkWing();	//必须置于MirDatumPoint之后,因为desEndPos.datumPoint影响GetEndWorkWing wjh-2016.4.25
			//自基点至楞点的偏移方式
		//pMirAngle->desEndPos.datum_to_ber_style=pSrcAngle->desEndPos.datum_to_ber_style;
		if( (msg.axis_flag==0x01||msg.axis_flag==0x02||msg.axis_flag==0x10)&&
			(pSrcAngle->MirLayStyle!=CLDSLineAngle::MIRXorY_INVERT))
		{	//X/Y对称里外铁同向,需调换两肢偏移参数
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
		if(pDatumPart.IsHasPtr()&&!pDatumPart.IsEqualPtr(pMirAngle)&&//根据肢摆方方式进行修正
			!(pMirAngle->desEndPos.wing_x_offset.gStyle==4&&pMirAngle->desEndPos.wing_x_offset.offsetDist==0&&
			 pMirAngle->desEndPos.wing_y_offset.gStyle==4&&pMirAngle->desEndPos.wing_y_offset.offsetDist==0))
		{	//原本都是自定义偏移量时,如偏移量为零时,不修正 wjh-2011.8.28
			if(wing_x0_y1==0)
			{	//自身X肢为搭接肢
				pMirAngle->desEndPos.wing_x_offset.gStyle=4;
				pMirAngle->desEndPos.wing_x_offset.offsetDist=0;
				pMirAngle->desEndPos.wing_y_offset.gStyle=0;
			}
			else if(wing_x0_y1==1)
			{	//自身Y肢为搭接肢
				pMirAngle->desEndPos.wing_x_offset.gStyle=0;
				pMirAngle->desEndPos.wing_y_offset.gStyle=4;
				pMirAngle->desEndPos.wing_y_offset.offsetDist=0;
			}
		}
	}
	pSrcAngle->SyncMirPropTo(pMirAngle,msg,"Position");	//之上代码在对称两肢坐标控制时，信息对称不完整 wjh-2016.4.27
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
				pMirSonAngle->SetPartNo(pSonAngle->GetPartNo());	//构件编号无法从组合角钢中对称过来,因此必须特殊处理
			}
			if(pGroupAngle->group_style<2&&i==1)
				break;	//非十字组合角钢
		}
	}
	
	bool bCanNewBolt=false;
	if(pMirAngle->pStart==NULL&&pMirAngle->pEnd==NULL)
		bCanNewBolt=true;	//目前仅支持短角钢视需要对称生成螺栓，普通角钢只对称已有螺栓引用
	MirAngleBoltRefList(pSrcAngle,pMirAngle,msg,bCanNewBolt);
	g_pSolidDraw->Draw();
	return pMirAngle;
}

static BOOL MirAngle(CLDSLineAngle* pSrcLineAngle, MIRMSG mirmsg,BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//是否成功对称生成所有构件
	RELATIVE_OBJECT *pRelaObj=NULL;
	if(!IsPartMirValid(pSrcLineAngle,mirmsg))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "fail to pass symmetry legality checking, parts can not be symmetrical";
#else
		throw "未通过对称合法性检测,不能对称构件";
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
	if((mirmsg.axis_flag&8)==0)	//无旋转对称
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
		{	//进行多次旋转对称时基准构件为同一个构件，只需要调整旋转角度即可
			if(pSrcLineAngle==NULL)
				continue;
			//调整旋转角度
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSLineAngle *pMirAngle=CreateSingleMirAngle(pSrcLineAngle,mirmsg,bIncAttachInfo);
			if(pMirAngle)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcangle_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcangle_arr[0]->relativeObjs.GetNext())
				{	//避免重复添加关联对象
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
	//设置对称生成的对象彼此关联
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
		pMirTube->CopyProperty(pSrcLineTube);//拷贝父节点(部分)属性
		GEPOINT vMirWorkPlaneNormal=msg.MirVector(pSrcLineTube->WorkPlaneNorm());
		pMirTube->SetWorkPlaneNorm(vMirWorkPlaneNormal);
		AddMirObjToSrcObjRelaObjList(pSrcLineTube,pMirTube,msg);	//添加对称生成的对象到源对象的关联对象列表
	}
	//图层
	CalMirAtomLayer(pSrcLineTube->layer(),pMirTube->layer(),msg);
	//始终端位置
	pMirTube->SetStart(GetMirPos(pSrcLineTube->Start(),msg));
	pMirTube->SetEnd(GetMirPos(pSrcLineTube->End(),msg));
	if(!bIncAttachInfo)
		return pMirTube;
	//工作面法线
	pMirTube->m_iNormCalStyle=pSrcLineTube->m_iNormCalStyle;
	pMirTube->nearWorkPlaneNorm=GetMirVector(pSrcLineTube->nearWorkPlaneNorm,msg);
	if(pMirTube->m_iNormCalStyle==0)	//直接指定工作面法线方向
		pMirTube->SetWorkPlaneNorm(GetMirVector(pSrcLineTube->WorkPlaneNorm(),msg));
	else if(pSrcLineTube->nearWorkPlaneNorm.IsZero()&&pSrcLineTube->hWorkNormRefPart==0) 
		pMirTube->nearWorkPlaneNorm=GetMirVector(pSrcLineTube->WorkPlaneNorm(),msg);
	MirPartHandle(pSrcLineTube->BelongModel(),&pMirTube->hWorkNormRefPart,pSrcLineTube->hWorkNormRefPart,msg);
	//焊接父构件
	long hMirPartWeldParent=pMirTube->m_hPartWeldParent;
	MirPartHandle(pSrcLineTube->BelongModel(),&hMirPartWeldParent,pSrcLineTube->m_hPartWeldParent,msg);
	pMirTube->m_hPartWeldParent=hMirPartWeldParent;
	if(pMirTube!=pSrcLineTube)
	{	//始端定位方式
		if(pMirTube->pStart&&pMirTube->pStart->hFatherPart!=pMirTube->handle)	
		{	
			pMirTube->desStartPos.endPosType=1;//搭接连接
			pMirTube->desStartPos.hDatumPart=pMirTube->pStart->hFatherPart;
			pMirTube->desStartPos.datum_pos_style=pSrcLineTube->desStartPos.datum_pos_style;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirTube->desStartPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pMirTube->pEnd->Position(false)-pMirTube->pStart->Position(false))^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
				{	//X肢为基准肢
					if(pMirTube->desStartPos.datum_pos_style==2||pMirTube->desStartPos.datum_pos_style==5)
						pMirTube->desStartPos.datum_pos_style-=1;
				}
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
				{	//Y肢为基准肢
					if(pMirTube->desStartPos.datum_pos_style==1||pMirTube->desStartPos.datum_pos_style==4)
					pMirTube->desStartPos.datum_pos_style+=1;
				}
			}
		}
		if(pMirTube->desStartPos.endPosType==0) //节点即基点
			pMirTube->desStartPos.fNormOffset=pSrcLineTube->desStartPos.fNormOffset;
		//终端定位方式
		if(pMirTube->pEnd&&pMirTube->pEnd->hFatherPart!=pMirTube->handle)	
		{	
			pMirTube->desEndPos.endPosType=1;//搭接连接
			pMirTube->desEndPos.hDatumPart=pMirTube->pEnd->hFatherPart;
			pMirTube->desEndPos.datum_pos_style=pSrcLineTube->desEndPos.datum_pos_style;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirTube->desEndPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pMirTube->pEnd->Position(false)-pMirTube->pStart->Position(false))^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
				{	//X肢为基准肢
					if(pMirTube->desEndPos.datum_pos_style==2||pMirTube->desEndPos.datum_pos_style==5)
						pMirTube->desEndPos.datum_pos_style-=1;
				}
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
				{	//Y肢为基准肢
					if(pMirTube->desEndPos.datum_pos_style==1||pMirTube->desEndPos.datum_pos_style==4)
						pMirTube->desEndPos.datum_pos_style+=1;
				}
			}
		}
		if(pMirTube->desEndPos.endPosType==0) //节点即基点
			pMirTube->desEndPos.fNormOffset=pSrcLineTube->desEndPos.fNormOffset;
	}
	if(pSrcLineTube->m_cFuncType==CLDSLineTube::FUNC_WIREBUSH)
	{	//套管对称时，需查找对称的定位螺栓和依赖钢板  wxc-2017.3.20
		MirPartHandle(pSrcLineTube->BelongModel(),&pMirTube->bush.hBushRelyBolt,pSrcLineTube->bush.hBushRelyBolt,msg);
		MirPartHandle(pSrcLineTube->BelongModel(),&pMirTube->bush.hBushResidePlate,pSrcLineTube->bush.hBushResidePlate,msg);
	}
	pMirTube->CalPosition();
	NewLinePart(pMirTube);
	g_pSolidDraw->Draw();
	//添加对称生成的对象到源对象的关联对象列表
	AddMirObjToSrcObjRelaObjList(pSrcLineTube,pMirTube,msg);
	return pMirTube;
}
BOOL MirTube(CLDSLineTube *pSrcLineTube, MIRMSG mirmsg, BOOL bIncAttachInfo=TRUE,  char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//是否成功对称生成所有构件
	RELATIVE_OBJECT *pRelaObj=NULL;
	if(!IsPartMirValid(pSrcLineTube,mirmsg))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "fail to pass symmetry legality checking, parts can not be symmetrical";
#else
		throw "未通过对称合法性检测,不能对称构件";
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
	if((mirmsg.axis_flag&8)==0)	//无旋转对称
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
		{	//进行多次旋转对称时基准构件为同一个构件，只需要调整旋转角度即可
			if(pSrcLineTube==NULL)
				continue;
			//调整旋转角度
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSLineTube *pMirTube=CreateSingleMirTube(pSrcLineTube,mirmsg);
			if(pMirTube)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srctube_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srctube_arr[0]->relativeObjs.GetNext())
				{	//避免重复添加关联对象
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
	//设置对称生成的对象彼此关联
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
		pMirFlat->CopyProperty(pSrcLineFlat);//拷贝父节点(部分)属性
		AddMirObjToSrcObjRelaObjList(pSrcLineFlat,pMirFlat,msg);	//添加对称生成的对象到源对象的关联对象列表
	}
	//图层
	CalMirAtomLayer(pSrcLineFlat->layer(),pMirFlat->layer(),msg);
	if(!bIncAttachInfo)
		return pMirFlat;
	//始终端位置
	pMirFlat->SetStart(GetMirPos(pSrcLineFlat->Start(),msg));
	pMirFlat->SetEnd(GetMirPos(pSrcLineFlat->End(),msg));
	//工作面法线
	pMirFlat->m_iNormCalStyle=pSrcLineFlat->m_iNormCalStyle;
	pMirFlat->nearWorkPlaneNorm=GetMirVector(pSrcLineFlat->nearWorkPlaneNorm,msg);
	if(pMirFlat->m_iNormCalStyle==0)	//直接指定工作面法线方向
		pMirFlat->SetWorkPlaneNorm(GetMirVector(pSrcLineFlat->WorkPlaneNorm(),msg));
	else if(pSrcLineFlat->nearWorkPlaneNorm.IsZero()&&pSrcLineFlat->hWorkNormRefPart==0) 
		pMirFlat->nearWorkPlaneNorm=GetMirVector(pSrcLineFlat->WorkPlaneNorm(),msg);
	MirPartHandle(pSrcLineFlat->BelongModel(),&pMirFlat->hWorkNormRefPart,pSrcLineFlat->hWorkNormRefPart,msg);
	pMirFlat->CalWorkPlaneNorm();
	//焊接父构件
	long hMirPartWeldParent=pMirFlat->m_hPartWeldParent;
	MirPartHandle(pSrcLineFlat->BelongModel(),&hMirPartWeldParent,pSrcLineFlat->m_hPartWeldParent,msg);
	pMirFlat->m_hPartWeldParent=hMirPartWeldParent;
	if(pMirFlat!=pSrcLineFlat)
	{	//始端定位方式
		if(pMirFlat->pStart&&pMirFlat->pStart->hFatherPart!=pMirFlat->handle)	
		{	
			pMirFlat->desStartPos.endPosType=1;//搭接连接
			pMirFlat->desStartPos.hDatumPart=pMirFlat->pStart->hFatherPart;
			pMirFlat->desStartPos.datum_pos_style=pSrcLineFlat->desStartPos.datum_pos_style;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirFlat->desStartPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pMirFlat->pEnd->Position(false)-pMirFlat->pStart->Position(false))^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
				{	//X肢为基准肢
					if(pMirFlat->desStartPos.datum_pos_style==2||pMirFlat->desStartPos.datum_pos_style==5)
						pMirFlat->desStartPos.datum_pos_style-=1;
				}
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
				{	//Y肢为基准肢
					if(pMirFlat->desStartPos.datum_pos_style==1||pMirFlat->desStartPos.datum_pos_style==4)
						pMirFlat->desStartPos.datum_pos_style+=1;
				}
			}
		}
		if(pMirFlat->desStartPos.endPosType==0) //节点即基点
			pMirFlat->desStartPos.fNormOffset=pSrcLineFlat->desStartPos.fNormOffset;
		//终端定位方式
		if(pMirFlat->pEnd&&pMirFlat->pEnd->hFatherPart!=pMirFlat->handle)	
		{	
			pMirFlat->desEndPos.endPosType=1;//搭接连接
			pMirFlat->desEndPos.hDatumPart=pMirFlat->pEnd->hFatherPart;
			pMirFlat->desEndPos.datum_pos_style=pSrcLineFlat->desEndPos.datum_pos_style;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirFlat->desEndPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pMirFlat->pEnd->Position(false)-pMirFlat->pStart->Position(false))^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
				{	//X肢为基准肢
					if(pMirFlat->desEndPos.datum_pos_style==2||pMirFlat->desEndPos.datum_pos_style==5)
						pMirFlat->desEndPos.datum_pos_style-=1;
				}
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
				{	//Y肢为基准肢
					if(pMirFlat->desEndPos.datum_pos_style==1||pMirFlat->desEndPos.datum_pos_style==4)
						pMirFlat->desEndPos.datum_pos_style+=1;
				}
			}
		}
		if(pMirFlat->desEndPos.endPosType==0) //节点即基点
			pMirFlat->desEndPos.fNormOffset=pSrcLineFlat->desEndPos.fNormOffset;
	}
	NewLinePart(pMirFlat);
	if(pMirFlat->pStart==NULL&&pMirFlat->pEnd==NULL)
	{	//对称生成或引入槽钢上的螺栓
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
	//添加对称生成的对象到源对象的关联对象列表
	AddMirObjToSrcObjRelaObjList(pSrcLineFlat,pMirFlat,msg);
	return pMirFlat;
}
BOOL MirFlat(CLDSLineFlat *pSrcLineFlat, MIRMSG mirmsg, BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//是否成功对称生成所有构件
	RELATIVE_OBJECT *pRelaObj=NULL;
	if(!IsPartMirValid(pSrcLineFlat,mirmsg))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "fail to pass symmetry legality checking, parts can not be symmetrical";
#else
		throw "未通过对称合法性检测,不能对称构件";
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
	if((mirmsg.axis_flag&8)==0)	//无旋转对称
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
		{	//进行多次旋转对称时基准构件为同一个构件，只需要调整旋转角度即可
			if(pSrcLineFlat==NULL)
				continue;
			//调整旋转角度
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSLineFlat *pMirFlat=CreateSingleMirFlat(pSrcLineFlat,mirmsg,bIncAttachInfo);
			if(pMirFlat)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcFlat_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcFlat_arr[0]->relativeObjs.GetNext())
				{	//避免重复添加关联对象
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
	//设置对称生成的对象彼此关联
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
		pMirSlot->CopyProperty(pSrcLineSlot);//拷贝父节点(部分)属性
		AddMirObjToSrcObjRelaObjList(pSrcLineSlot,pMirSlot,msg);	//添加对称生成的对象到源对象的关联对象列表
		//始终端位置
		pMirSlot->SetStart(GetMirPos(pSrcLineSlot->Start(),msg));
		pMirSlot->SetEnd(GetMirPos(pSrcLineSlot->End(),msg));
	}
	//图层
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
	//工作面法线
	pMirSlot->m_iNormCalStyle=pSrcLineSlot->m_iNormCalStyle;
	pMirSlot->nearWorkPlaneNorm=GetMirVector(pSrcLineSlot->nearWorkPlaneNorm,msg);
	if(pMirSlot->m_iNormCalStyle==0)	//直接指定工作面法线方向
		pMirSlot->SetWorkPlaneNorm(GetMirVector(pSrcLineSlot->WorkPlaneNorm(),msg));
	else if(pSrcLineSlot->nearWorkPlaneNorm.IsZero()&&pSrcLineSlot->hWorkNormRefPart==0) 
		pMirSlot->nearWorkPlaneNorm=GetMirVector(pSrcLineSlot->WorkPlaneNorm(),msg);
	MirPartHandle(pSrcLineSlot->BelongModel(),&pMirSlot->hWorkNormRefPart,pSrcLineSlot->hWorkNormRefPart,msg);
	pMirSlot->CalWorkPlaneNorm();
	//焊接父构件
	long hMirPartWeldParent=pMirSlot->m_hPartWeldParent;
	MirPartHandle(pSrcLineSlot->BelongModel(),&hMirPartWeldParent,pSrcLineSlot->m_hPartWeldParent,msg);
	pMirSlot->m_hPartWeldParent=hMirPartWeldParent;
	if(pMirSlot!=pSrcLineSlot)
	{	//始端定位方式
		if(pMirSlot->pStart&&pMirSlot->pStart->hFatherPart!=pMirSlot->handle)	
		{	
			pMirSlot->desStartPos.endPosType=1;//搭接连接
			pMirSlot->desStartPos.hDatumPart=pMirSlot->pStart->hFatherPart;
			pMirSlot->desStartPos.datum_pos_style=pSrcLineSlot->desStartPos.datum_pos_style;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirSlot->desStartPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pMirSlot->pEnd->Position(false)-pMirSlot->pStart->Position(false))^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
				{	//X肢为基准肢
					if(pMirSlot->desStartPos.datum_pos_style==2||pMirSlot->desStartPos.datum_pos_style==5)
						pMirSlot->desStartPos.datum_pos_style-=1;
				}
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
				{	//Y肢为基准肢
					if(pMirSlot->desStartPos.datum_pos_style==1||pMirSlot->desStartPos.datum_pos_style==4)
						pMirSlot->desStartPos.datum_pos_style+=1;
				}
			}
		}
		if(pMirSlot->desStartPos.endPosType==0) //节点即基点
			pMirSlot->desStartPos.fNormOffset=pSrcLineSlot->desStartPos.fNormOffset;
		//终端定位方式
		if(pMirSlot->pEnd&&pMirSlot->pEnd->hFatherPart!=pMirSlot->handle)	
		{	
			pMirSlot->desEndPos.endPosType=1;//搭接连接
			pMirSlot->desEndPos.hDatumPart=pMirSlot->pEnd->hFatherPart;
			pMirSlot->desEndPos.datum_pos_style=pSrcLineSlot->desEndPos.datum_pos_style;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirSlot->desEndPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pMirSlot->pEnd->Position(false)-pMirSlot->pStart->Position(false))^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
				{	//X肢为基准肢
					if(pMirSlot->desEndPos.datum_pos_style==2||pMirSlot->desEndPos.datum_pos_style==5)
						pMirSlot->desEndPos.datum_pos_style-=1;
				}
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
				{	//Y肢为基准肢
					if(pMirSlot->desEndPos.datum_pos_style==1||pMirSlot->desEndPos.datum_pos_style==4)
						pMirSlot->desEndPos.datum_pos_style+=1;
				}
			}
		}
		if(pMirSlot->desEndPos.endPosType==0) //节点即基点
			pMirSlot->desEndPos.fNormOffset=pSrcLineSlot->desEndPos.fNormOffset;
	}
	NewLinePart(pMirSlot);
	if(pMirSlot->pStart==NULL&&pMirSlot->pEnd==NULL)
	{	//对称生成或引入槽钢上的螺栓
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
	//添加对称生成的对象到源对象的关联对象列表
	AddMirObjToSrcObjRelaObjList(pSrcLineSlot,pMirSlot,msg);
	return pMirSlot;
}

BOOL MirSlot(CLDSLineSlot *pSrcLineSlot, MIRMSG mirmsg, BOOL bIncAttachInfo=TRUE, char *sErrorInfo=NULL)
{
	BOOL bSucceed=TRUE;	//是否成功对称生成所有构件
	RELATIVE_OBJECT *pRelaObj=NULL;
	if(!IsPartMirValid(pSrcLineSlot,mirmsg))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "fail to pass symmetry legality checking, parts can not be symmetrical";
#else
		throw "未通过对称合法性检测,不能对称构件";
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
	if((mirmsg.axis_flag&8)==0)	//无旋转对称
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
		{	//进行多次旋转对称时基准构件为同一个构件，只需要调整旋转角度即可
			if(pSrcLineSlot==NULL)
				continue;
			//调整旋转角度
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSLineSlot *pMirSlot=CreateSingleMirSlot(pSrcLineSlot,mirmsg,bIncAttachInfo);
			if(pMirSlot)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcSlot_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcSlot_arr[0]->relativeObjs.GetNext())
				{	//避免重复添加关联对象
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
	//设置对称生成的对象彼此关联
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
			AddMirObjToSrcObjRelaObjList(pSrcPlate,pMirPlate,msg);	//添加对称生成的对象到源对象的关联对象列表
		}
		pMirPlate->CopyProperty(pSrcPlate);
		pMirPlate->ucs.origin=GetMirPos(pSrcPlate->ucs.origin,msg);
		pMirPlate->ucs.axis_x=GetMirVector(pSrcPlate->ucs.axis_x,msg);
		pMirPlate->ucs.axis_y=GetMirVector(pSrcPlate->ucs.axis_y,msg);
		pMirPlate->ucs.axis_z=GetMirVector(pSrcPlate->ucs.axis_z,msg);
		pMirPlate->m_tInnerColumnNorm=GetMirVector(pSrcPlate->m_tInnerColumnNorm,msg);
		if((msg.axis_flag&4)==0&&(msg.axis_flag&8)==0&&pSrcPlate->jdb_style!=5)
			pMirPlate->ucs.axis_x*=-1.0;	//不是Z轴对称和旋转对称或附加板(jdb_style==5主要指十字插板的立板)
		CalMirAtomLayer(pSrcPlate->layer(),pMirPlate->layer(),msg);
		//提前是为了防止一组装配方位相同钢板在初次未含附加信息对称后，进行二次附件信息对称时查找对称钢板错误 wjh-2015.9.21
		pMirPlate->vertex_list.Empty();
		PROFILE_VERTEX *pPrevVertex=pSrcPlate->vertex_list.GetTail();
		for(PROFILE_VERTEX *pVertex=pSrcPlate->vertex_list.GetFirst();pVertex;pVertex=pSrcPlate->vertex_list.GetNext())
		{	//如果 为相对于基准构件位置不动的板直接把顶点列表复制过去即可
			if(msg.axis_flag&4||msg.axis_flag&8||pSrcPlate->jdb_style==5)	//Z轴对称或旋转对称
			{
				PROFILE_VERTEX vertex=*pVertex;
				if(vertex.type==3)	//椭圆弧
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
				//由于Ｘ轴、Ｙ轴对称时圆心拾取点坐标也应进行相应变换
				if(vertex.type==2)	//指定圆弧半径及拾取点
					vertex.center.x*=-1;
				else if(vertex.type==3)	//椭圆弧
				{
					vertex.column_norm=GetMirVector(pPrevVertex->column_norm,msg);
					vertex.center=GetMirPos(pPrevVertex->center,msg);
				}
				vertex.m_cPosDesignType=pVertex->m_cPosDesignType;
				vertex.m_hRod=pVertex->m_hRod;
				PROFILE_VERTEX* pTmVertex=pMirPlate->vertex_list.insert(vertex,0);
				MirPartHandle(pSrcPlate->BelongModel(),&pTmVertex->m_hRod,vertex.m_hRod,msg);
				pPrevVertex=pVertex;
				//更新位于依赖杆件中间的轮廓顶点的计算类型
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
					if(mir_line_vec*tm_line_vec<0)	//杆件朝向相反
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
		if(msg.axis_flag&4||msg.axis_flag&8)	//Z轴对称或旋转对称
		pMirPlate->designInfo.additional_vertex.append(*pPt);
		else
		pMirPlate->designInfo.additional_vertex.append(-pPt->x,pPt->y,pPt->z);
		}*/
		if((msg.axis_flag&4)==0&&(msg.axis_flag&8)==0&&pSrcPlate->jdb_style!=5)	//不是Z轴对称和旋转对称或附加板
		{
			//已移至函数开始处 wjh-2015.6.30
			//pMirPlate->ucs.axis_x*=-1.0;
			pMirPlate->HuoQuLine[0].startPt.x*=-1.0;
			pMirPlate->HuoQuLine[0].endPt.x*=-1.0;
			pMirPlate->HuoQuLine[1].startPt.x*=-1.0;
			pMirPlate->HuoQuLine[1].endPt.x*=-1.0;
			if(pSrcPlate->IsPeakPointPlate())//pMirPlate->jdb_style==0&&pMirPlate->face_N==3)
				pMirPlate->top_point.x*=-1.0;
		}
		//连接板基本设计信息对称
		pMirPlate->designInfo.m_bWeldToDatumJg=pSrcPlate->designInfo.m_bWeldToDatumJg;	//节点板焊接于基准角钢上 wht 10-06-07
		pMirPlate->designInfo.iFaceType=pSrcPlate->designInfo.iFaceType;
		pMirPlate->designInfo.iProfileStyle0123=pSrcPlate->designInfo.iProfileStyle0123;
		pMirPlate->designInfo.m_bEnableFlexibleDesign=pSrcPlate->designInfo.m_bEnableFlexibleDesign;
		pMirPlate->designInfo.iWeldLineStyle=pSrcPlate->designInfo.iWeldLineStyle;
		pMirPlate->designInfo.m_bSelfCorrectBendline=pSrcPlate->designInfo.m_bSelfCorrectBendline;
		MirPartHandle(pSrcPlate->BelongModel(),&pMirPlate->designInfo.m_hBaseNode,pSrcPlate->designInfo.m_hBaseNode,msg);
		if(pSrcPlate->designInfo.m_hBaseNode>0x20&&pMirPlate->designInfo.m_hBaseNode<0x20)
		{	//钢板存在基准节点但未找到合法的对称节点,无法完成继续对称生成 wht 10-10-19
			CLDSNode *pBaseNode=pSrcPlate->BelongModel()->FromNodeHandle(pSrcPlate->designInfo.m_hBaseNode);
			if(pBaseNode)
			{	//此处错误信息应传递到外部，打印到记事本中
				if(sErrorInfo)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sErrorInfo,"0X%X basic point symmetry failed!",pBaseNode->handle);
#else
					sprintf(sErrorInfo,"0X%X基点对称失败!",pBaseNode->handle);
#endif
				return NULL;
			}
		}
		MirPartHandle(pSrcPlate->BelongModel(),&pMirPlate->designInfo.m_hBasePart,pSrcPlate->designInfo.m_hBasePart,msg,&pSrcPlate->ucs.origin);
		if(pSrcPlate->designInfo.m_hBasePart>0x20&&pMirPlate->designInfo.m_hBasePart<0x20)
		{	//钢板存在基准杆件但未找到合法的对称杆件,无法完成继续对称生成 wht 10-10-19
			CLDSPart *pBaseLinePart=pSrcPlate->BelongModel()->FromPartHandle(pSrcPlate->designInfo.m_hBasePart,CLS_LINEPART);
			if(pBaseLinePart)
			{
				if(sErrorInfo)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sErrorInfo,"0X%X datum rod symmetry failed!",pBaseLinePart->handle);
#else
					sprintf(sErrorInfo,"0X%X基准杆件对称失败!",pBaseLinePart->handle);
#endif
				return NULL;
			}
		}
		pMirPlate->designInfo.weld_width=pSrcPlate->designInfo.weld_width;
		pMirPlate->designInfo.weld_start_high=pSrcPlate->designInfo.weld_start_high;
		pMirPlate->designInfo.weld_end_high=pSrcPlate->designInfo.weld_end_high;
		//基点及工作法线设计信息对称
		MirDatumPoint(pSrcPlate->BelongModel(),pSrcPlate->designInfo.origin,pMirPlate->designInfo.origin,msg);
		if(pSrcPlate->designInfo.norm.vector.IsZero())	//更新法线设计参数 wht 11-01-11
			pSrcPlate->designInfo.norm.UpdateVector(&Ta);
		CLDSNode *pBaseNode=pSrcPlate->GetBaseNode();
		f3dPoint *pPickPos=NULL;
		if(pBaseNode)
			pPickPos=&pBaseNode->Position(false);
		MirDatumVector(pSrcPlate->BelongModel(),pSrcPlate->designInfo.norm,pMirPlate->designInfo.norm,msg,pPickPos);
		BOOL bSameDatumRod=FALSE; //对称钢板基于同一杆件定位
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
			AfxMessageBox("暂不支持钢板的基准杆件非角钢类的角对称");
#endif
			return NULL;
		}
		CLDSLineAngle *pMirDatumAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirPlate->designInfo.origin.m_hDatumRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if( !bSameDatumRod&&pDatumAngle&&pMirDatumAngle&&
			(pDatumAngle->GetClassTypeId()==CLS_GROUPLINEANGLE||pDatumAngle->group_father_jg_h>0x20))
		{	//基准构件为组合角钢且不是自身对称时需要更新钢板基准点设计参数 wht-17.06.05
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
		BOOL bDatumJgStyleDiff=FALSE; //基准角钢里外铁形式是否一致 wht 11-01-17
		if(pMirPlate->designInfo.norm.norm_style==1&&pSrcPlate->designInfo.norm.norm_style==1
			&&pMirPlate->designInfo.norm.direction!=pSrcPlate->designInfo.norm.direction&&!bSameDatumRod
			&&!bResideGroupAngle)	//TODO:T型组合角钢顶面应按单角钢处理，暂未处理 wht-17.06.05
		{	//钢板基准角钢一个为里铁一个为外铁(主要用于塔身交叉斜材上辅材连接板的对称)
			pMirPlate->designInfo.norm.direction=pSrcPlate->designInfo.norm.direction;
			pMirPlate->designInfo.norm.UpdateVector(&Ta);
			pMirPlate->DesignSetupUcs();	//重新设计钢板装配坐标系
			bDatumJgStyleDiff=TRUE; 
		}
		else
		{
			pMirPlate->ucs.axis_z=pMirPlate->ucs.axis_x^pMirPlate->ucs.axis_y;
			pMirPlate->designInfo.norm.UpdateVector(&Ta);
			if(pMirPlate->designInfo.norm.vector*pMirPlate->ucs.axis_z<0)	//应进行里外面翻转
			{	//只有在jdb_style==5的十字插板立板才会如此
				if(pMirPlate->designInfo.norm.norm_style==0)
					pMirPlate->designInfo.norm.vector=pMirPlate->designInfo.norm.nearVector=pMirPlate->ucs.axis_z;
				else if(pMirPlate->designInfo.norm.norm_style==1)
					pMirPlate->designInfo.norm.direction*=-1;
				else
					pMirPlate->designInfo.norm.nearVector*=-1.0;
				pMirPlate->m_fNormOffset=-pMirPlate->m_fNormOffset-pMirPlate->GetThick();
			}
			else if(bSameDatumAngle)
			{	//同基准角钢的钢板对称 wjh-2017.9.27
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
				{	//定位基准角钢相同,但一块板法线朝角钢外侧,一块板法线朝角钢内侧(常见Ⅱ型对角组合角钢角对称)
					CParaThick paraThick(pMirPlate->designInfo.origin.des_para.RODNODE.norm_offset);
					int index=0;
					for(THICK_ITEM item=paraThick.GetFirstItem();item.val!=0;item=paraThick.GetNextItem(),index++)
					{
						item.val*=-1;	//反号
						paraThick.SetItem(index,item);
					}
					StrCopy(pMirPlate->designInfo.origin.des_para.RODNODE.norm_offset,(char*)paraThick.ToThickStr(),51);
					//sprintf(pMirPlate->designInfo.origin.des_para.RODNODE.norm_offset,"%f",-1.0*paraThick.Thick(&Ta));
				}
				//将来应将II型组合角钢自身角对称时基点的对称统一到ANGLEPOINT对称中，目前暂用临时代码解决无柔性设计时，钢板外形对称错误　wjh-2017.6.6
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
			//	同基准钢管时暂不更改钢板定位基准点
		}
		pMirPlate->designInfo.norm.UpdateVector(&Ta);
		if(pSrcPlate->face_N>1)
		{
			MirDatumPoint(pSrcPlate->BelongModel(),pSrcPlate->designInfo.huoquline_start[0],pMirPlate->designInfo.huoquline_start[0],msg);
			MirDatumPoint(pSrcPlate->BelongModel(),pSrcPlate->designInfo.huoquline_end[0],pMirPlate->designInfo.huoquline_end[0],msg);
			MirDatumVector(pSrcPlate->BelongModel(),pSrcPlate->designInfo.huoqufacenorm[0],pMirPlate->designInfo.huoqufacenorm[0],msg);
			//TODO:帽子对称是否有问题？
			//基准角钢里外铁样式不一致时，对称钢板Z轴与(基准钢板Z轴)对称方向相反,制弯面方向也应该反向
			//CLDSPlate::SyncPlateProfile()函数中也有相关代码,将来应统一处理 wht 16-09-19
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
			//基准角钢里外铁样式不一致时，对称钢板Z轴与(基准钢板Z轴)对称方向相反,制弯面方向也应该反向
			//CLDSPlate::SyncPlateProfile()函数中也有相关代码,将来应统一处理 wht 16-09-19
			pMirPlate->designInfo.huoqufacenorm[1].UpdateVector(&Ta);
			double dd1=pSrcPlate->huoqufacenorm[1]*pSrcPlate->ucs.axis_z;
			double dd2=pMirPlate->designInfo.huoqufacenorm[1].vector*pMirPlate->designInfo.norm.vector;
			if(dd1*dd2<0)
				pMirPlate->designInfo.huoqufacenorm[1].ReverseDirection();
		}
		//根据基点设计信息重新计算板的位置
		pMirPlate->designInfo.origin.UpdatePos(&Ta);
		//无基准点设计信息时不更新板的位置
		if(pMirPlate->designInfo.origin.datum_pos_style>0)
			pMirPlate->ucs.origin=pMirPlate->designInfo.origin.Position();
		else
			pMirPlate->designInfo.origin.datum_pos=pMirPlate->ucs.origin;
		//添加对称生成的对象到源对象的关联对象列表
		AddMirObjToSrcObjRelaObjList(pSrcPlate,pMirPlate,msg);
		//连板螺栓对称	//可能有的杆件要根据螺栓计算正负头所以应先将螺栓对称生成
		CHashList<long> ljPartStart0End1HashList;
		ARRAY_LIST<THANDLE> listRelaParts(0,pSrcPlate->designInfo.partList.GetNodeNum()+1);
		for(pLjPart=pSrcPlate->designInfo.partList.GetFirst();pLjPart;pLjPart=pSrcPlate->designInfo.partList.GetNext())
		{
			ljPartStart0End1HashList.SetValue(pLjPart->hPart,pLjPart->start0_end1);
			listRelaParts.append(pLjPart->hPart);
		}
		//完善对称钢板螺栓引用时，应考虑基准杆件(双主材填板对称时螺栓未引入到基准角钢上) wht 12-05-21
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
				//如果螺栓通厚参数中无当前钢板句柄时则添加钢板句柄到通厚参数 wht 11-01-11
				pMirBolt->AddL0Thick(pMirPlate->handle,TRUE,TRUE);
				pMirBolt->CalGuigeAuto();	//通厚变化后,重新计算螺栓规格
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
				pNewLsRef->feature=0;	//清空钢板螺栓引用feature属性,调整端连接共用螺栓参数时使用 wht 11-01-17
			pNewLsRef->cFlag=pLsRef->cFlag;

			for(long *pStart0End1=ljPartStart0End1HashList.GetFirst();pStart0End1;pStart0End1=ljPartStart0End1HashList.GetNext())
			{
				CSuperSmartPtr<CLDSPart>pSrcPart=Ta.Parts.FromHandle(ljPartStart0End1HashList.GetCursorKey());
				if(pSrcPart.IsHasPtr()&&pSrcPart->GetClassTypeId()==CLS_LINEANGLE)
				{
					CLDSLineAngle *pMirAngle=NULL;
					//启用一端对称查找对称杆件时有问题 
					/*
					f3dPoint len_vec=pSrcPart.LineAnglePointer()->End()-pSrcPart.LineAnglePointer()->Start();
					normalize(len_vec);
					if(fabs(len_vec*f3dPoint(1,0,0))>EPS_COS2||	//在求横材对称杆件时不能启用只有一端对称
						msg.axis_flag==8||msg.axis_flag==4)		//旋转对称或Z轴对称
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
				{	//组合角钢 添加螺栓引用时可自动判断螺栓是否位于子角钢上 wht 11-01-13
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
		//基准节点为基准杆件端点时对称基准杆件正负头信息
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
		//提前预对称正负头值，以防多根射线角钢时因部分相邻角钢负头未对称导致切角计算有误 wjh-2017.6.26
		CHashSet<CDesignLjPartPara*> hashMirLjRodParas;
		CHashList<bool> hashParentGroupAngles;	//保证组合角钢对称唯一性哈希表（钢板的连接杆件为子角钢时，需要根据子角钢找到组合角钢） wjh-2019.9.18
		for(pLjPart=pSrcPlate->designInfo.partList.GetFirst();pLjPart;pLjPart=pSrcPlate->designInfo.partList.GetNext())
		{
			pLjPart->pCurLinePart=(CLDSLinePart*)Ta.Parts.FromHandle(pLjPart->hPart,CLS_LINEPART);
			if(pLjPart->pCurLinePart==NULL)
				continue;
			long hMirLjPart=pLjPart->hPart;
			if(pLjPart->pCurLinePart->GetClassTypeId()==CLS_LINETUBE)	//钢管启用一端对称
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
			/*根据pLjPart->hPart填充pMirPlate的关联构件，可能由于调用ModifyKeyId函数导致后面新加的关联构件与之前的关联构件重复,丢失关联构件信息，
				故直接获取pLjPart->hPart的对称句柄,然后再填充pMirPlate的关联构件 wxc-2017.5.15*/
			CLDSLinePart* pMirLinePart=(CLDSLinePart*)Ta.Parts.FromHandle(hMirLjPart);
			if(pMirLinePart==NULL)
				continue;
			CDesignLjPartPara *pMirLjPart=pMirPlate->designInfo.partList.SetValue(hMirLjPart,*pLjPart);
			pMirLjPart->pCurLinePart=pMirLinePart;	//
			pMirLjPart->m_bAdjustLjEndPos=pLjPart->m_bAdjustLjEndPos;	//对称时是否调整杆件连接端摆放位置
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
			if(pLjPart->start0_end1>1)		//中间连接
				pMirLjPart->start0_end1=pLjPart->start0_end1;
			else if(mir_vec_tm*mir_line_vec>0)
				pMirLjPart->start0_end1=1;	//终端连接
			else
				pMirLjPart->start0_end1=0;	//始端连接
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
		//连接构件设计信息对称
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
					logerr.Log("丢失杆件0x%X@钢板0x%X的关联对称连接杆件，无法完成对称!",pLjPart->hPart,pSrcPlate->handle);
				else
					AfxMessageBox(CXhChar200("丢失杆件0x%X@钢板0x%X的关联对称连接杆件，无法完成对称!",pLjPart->hPart,pSrcPlate->handle));
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
			{	//基准杆件只修正当前连接肢，位置参数不对称	wjh-2012.3.19
				if(pLjPart->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
					pMirPlate->IsInsideAngle((CLDSLineAngle*)pMirLjPart->pCurLinePart,&pMirLjPart->angle.cur_wing_x0_y1);
				continue;
			}
			if(pMirLjPart->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{
				CLDSLineAngle *pSrcLineAngle=(CLDSLineAngle*)pLjPart->pCurLinePart;
				CLDSLineAngle *pMirLineAngle=(CLDSLineAngle*)pMirLjPart->pCurLinePart;
				f3dPoint verify_norm,wing_norm;
				if(pMirPlate->designInfo.norm.norm_style==1)	//角钢肢法线
				{
					CLDSLineAngle* pDatumAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pMirPlate->designInfo.norm.hVicePart);
					if(pDatumAngle&&pMirPlate->designInfo.norm.norm_wing==0)	//X肢法线
						wing_norm=pDatumAngle->get_norm_x_wing();
					else if(pDatumAngle&&pMirPlate->designInfo.norm.norm_wing==1)	//Y肢法线
						wing_norm=pDatumAngle->get_norm_y_wing();
				}
				if(pMirLjPart->iFaceNo==1)
					verify_norm=pMirPlate->ucs.axis_z;
				else if(pMirLjPart->iFaceNo==2)
					verify_norm=pMirPlate->huoqufacenorm[0];
				else //if
					verify_norm=pMirPlate->huoqufacenorm[1];
				//里外铁判断标准应该是基准角钢的基准肢法线,而不是钢板法线，否则在对称塔身单螺栓连接时
				//里外铁会判断错误,进而导致射线角钢端头偏移及切角错误 wjh-2012.2.19
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
				
				//非始端连接亦非终端连接则继续 不需要调整始终端位置
				if((pLjPart->start0_end1!=0&&pLjPart->start0_end1!=1)
					||!pLjPart->m_bAdjustLjEndPos)	//特别指定的不需要调整位置的杆件 wht 10-02-26
					//||(pLjPart->angle.bTwoEdge&&pMirPlate->jdb_style!=6))	//连接板命令做的共用板副基准角钢可通过<两端轮廓>属性判断 wht 15-09-14
					continue;
				//不能通过pLjPart->angle.bTwoEdge属性判断是否需要更新射线角钢位置
				CLDSLineAngle* pRayAngle=(CLDSLineAngle*)pMirLjPart->pCurLinePart;
				bool blNeedUpdateTipPos=false;
				if (pMirLjPart->start0_end1==0&&pRayAngle->hStartDatumRod==pMirPlate->designInfo.m_hBasePart&&pRayAngle->IsLappedAnchorStart())
					blNeedUpdateTipPos=true;
				else if(pMirLjPart->start0_end1==1&&pRayAngle->hEndDatumRod==pMirPlate->designInfo.m_hBasePart&&pRayAngle->IsLappedAnchorEnd())
					blNeedUpdateTipPos=true;
				if (!blNeedUpdateTipPos)
					continue;	//通过当前连接端是否搭接在基准杆件上作为更新杆件位置的判断条件，相比较更为严谨 wjh-2019.12.27
				
				MirLineAnglePosDesignInfo((CLDSLineAngle*)pLjPart->pCurLinePart,
					(CLDSLineAngle*)pMirLjPart->pCurLinePart,msg,
					pLjPart->start0_end1==0,pMirLjPart->start0_end1==0,
					pMirLjPart->angle.cur_wing_x0_y1==pLjPart->angle.cur_wing_x0_y1);
				CMultiOffsetPos *pPosDesign;
				if((pMirLjPart->start0_end1==0&&pMirLjPart->pCurLinePart->IsStartPosLocked())||
					(pMirLjPart->start0_end1==1&&pMirLjPart->pCurLinePart->IsEndPosLocked()))
					continue;	//始/终端位置锁定后不能调整
				//调整完对称角钢的定位方式后，重新计算正负头(考虑固定正负头的影响)
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
				//基点定位方式为指定基点坐标时，需要调整肢法向偏移量(变坡处双面板射线角钢)
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
					{	//基准杆件为钢管
						if(pDatumPart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
						{	//不调整非当前工作肢肢法向偏移量 wht 11-01-18
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
					//基准杆件为角钢 基点定位方式为向楞线投影时 不用调整肢法向偏移量(变坡处主材) wht 09-05-17
					//基准角钢为组合角钢时射线才定位方式为向楞轴线投影，故此处用pPosDesign->datum_pos_style!=1不合适 wht 11-01-17
					else if(toupper(pMirLjPart->pCurLinePart->layer(1))!='Z')	//非主材(排除变坡处主材) wht 11-01-17
						//(pPosDesign->datum_pos_style!=1)
					{
						//判断{在调整角钢摆放位置时}是否考虑角钢肢厚的依据:
						//1.角钢当前的正负头是否大于根据碰撞计算的正负头
						//2.检查组合角钢的子角钢中是否存在与当前角钢里/外铁形式一致的子角钢
						//3.子角钢的非当前肢法线方向与当前角钢(指向组合角钢的)延伸方向大致相同
						if(pDatumPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
						{	
							//1.计算当前角钢的碰撞正负头，与当前正负头进行比较
							double old_oddment=0;	//记录角钢的正负头
							DESIGNODDMENT desOddment;
							BOOL bExceedCollideOddment=FALSE;//角钢正负头是否大于根据碰撞计算出来的正负头
							if(pMirLjPart->start0_end1==0)
							{	//始端连接
								desOddment=pMirLineAngle->desStartOdd;
								old_oddment=pMirLineAngle->startOdd();
								//根据碰撞计算出一个正负头值
								pMirLineAngle->desStartOdd.m_iOddCalStyle=0;
								pMirLineAngle->desStartOdd.m_hRefPart1=pDatumPart->handle;
								pMirLineAngle->CalStartOddment();
								if(fabs(pMirLineAngle->startOdd())>fabs(old_oddment))
									bExceedCollideOddment=TRUE;
								//恢复原来的正负头值以及正负头计算方式
								pMirLineAngle->SetStartOdd(old_oddment);
								pMirLineAngle->desStartOdd=desOddment;
							}
							else if(pMirLjPart->start0_end1==1)
							{	//终端连接
								desOddment=pMirLineAngle->desEndOdd;
								old_oddment=pMirLineAngle->endOdd();
								//根据碰撞计算出一个正负头值
								pMirLineAngle->desEndOdd.m_iOddCalStyle=0;
								pMirLineAngle->desEndOdd.m_hRefPart1=pDatumPart->handle;
								pMirLineAngle->CalEndOddment();
								if(fabs(pMirLineAngle->endOdd())>fabs(old_oddment))
									bExceedCollideOddment=TRUE;
								//恢复原来的正负头值以及正负头计算方式
								pMirLineAngle->SetEndOdd(old_oddment);
								pMirLineAngle->desEndOdd=desOddment;
							}
							
							BOOL bCalcWingThick=FALSE;	//是否考虑肢厚
							if(bExceedCollideOddment)
							{	//当前角钢正负头大于根据碰撞计算出来的正负头时才需要进行以下判断
								f3dPoint len_vec;
								//计算当前角钢的延伸方向
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
										continue;	//若子角钢为虚构件则继续
									double ddx1=pSonAngle->get_norm_x_wing()*verify_norm;
									double ddy1=pSonAngle->get_norm_y_wing()*verify_norm;
									if(fabs(ddx1)>fabs(ddy1))
									{	//组合角钢子角钢的X肢为当前肢
										//3.判断子角钢非当前肢法线方向是否与当前角钢延伸方向一致
										if(pSonAngle->get_norm_y_wing()*len_vec<0)
											continue;
										//2.判断子角钢里/外铁形式是否与当前角钢一致
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
									{	//组合角钢子角钢的Y肢为当前肢 
										//3.判断子角钢非当前肢法线方向是否与当前角钢延伸方向一致
										if(pSonAngle->get_norm_x_wing()*len_vec<0)
											continue;
										//2.判断子角钢里/外铁形式是否与当前角钢一致
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
							//调整角钢摆放位置
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
							//	continue;	//共用板时只需调整指定坐标基点的角钢的位置 wht 09-06-16
							if(pMirLjPart->angle.cur_wing_x0_y1==0)
							{
								if(pMirPlate->jdb_style==6)	//共用板
									pPosDesign->wing_y_offset.gStyle=0;
								pPosDesign->wing_x_offset.gStyle=4;
								if(ddx>0)	//里铁
								{
									if((pMirLjPart->angle.bEndLjJg||pMirPlate->GetThick()==0
										||pSrcPlate->designInfo.m_bWeldToDatumJg)&&pDatumPart) //节点板焊接于基准角钢上 wht 10-06-07
									{	
										if(((CLDSLineAngle*)pDatumPart)->group_father_jg_h>0X20) //基准角钢为组合角钢子角钢 wht 11-01-17
											pPosDesign->wing_x_offset.offsetDist=-(0.5*pMirPlate->GetThick()+pDatumPart->GetThick());
										else //基准角钢为普通单角钢
											pPosDesign->wing_x_offset.offsetDist=-pDatumPart->GetThick();
									}
									else
										pPosDesign->wing_x_offset.offsetDist=0;
								}
								else
								{
									if(pSrcPlate->designInfo.m_bWeldToDatumJg)	//节点板焊接于基准角钢上 wht 10-06-07
										pPosDesign->wing_x_offset.offsetDist=0;
									else
										pPosDesign->wing_x_offset.offsetDist=-pMirPlate->GetThick();
								}
							}
							else
							{
								if(pMirPlate->jdb_style==6)	//共用板
									pPosDesign->wing_x_offset.gStyle=0;
								pPosDesign->wing_y_offset.gStyle=4;
								if(ddy>0)	//里铁
								{
									if((pMirLjPart->angle.bEndLjJg||pMirPlate->GetThick()==0
										||pSrcPlate->designInfo.m_bWeldToDatumJg)&&pDatumPart)//节点板焊接于基准角钢上 wht 10-06-07
									{
										if(((CLDSLineAngle*)pDatumPart)->group_father_jg_h>0X20) //基准角钢为组合角钢子角钢 wht 11-01-17
											pPosDesign->wing_y_offset.offsetDist=-(0.5*pMirPlate->GetThick()+pDatumPart->GetThick());
										else //基准角钢为普通单角钢
											pPosDesign->wing_y_offset.offsetDist=-pDatumPart->GetThick();
									}
									else
										pPosDesign->wing_y_offset.offsetDist=0;
								}
								else
								{
									if(pSrcPlate->designInfo.m_bWeldToDatumJg)	//节点板焊接于基准角钢上 wht 10-06-07
										pPosDesign->wing_y_offset.offsetDist=0;
									else
										pPosDesign->wing_y_offset.offsetDist=-pMirPlate->GetThick();
								}
							}
						}
						if(pMirLjPart->angle.bEndLjJg)
						{	//调整端连接射线角钢上螺栓的法向偏移量以及通厚 wht 11-01-17
							pMirLineAngle->ClearFlag();
							pMirLineAngle->CalPosition();	
							for(pLsRef=pMirPlate->GetFirstLsRef();pLsRef;pLsRef=pMirPlate->GetNextLsRef())
							{
								CLDSBolt *pBolt=pLsRef->GetLsPtr();
								if(!pMirLineAngle->FindLsByHandle(pBolt->handle))
									continue;	//不属于当前射线杆件
								CLDSLineAngle *pDatumJg=(CLDSLineAngle*)pDatumPart;
								if(pDatumPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
								{	//组合角钢
									pDatumJg=(CLDSLineAngle*)Ta.Parts.FromHandle(((CLDSGroupLineAngle*)pDatumJg)->m_hDatumSonAngle,CLS_LINEANGLE);
									if(pDatumJg==NULL)
										pDatumJg=(CLDSLineAngle*)pDatumPart;
								}
								if(pDatumJg->FindLsByHandle(pBolt->handle))
								{	//1.端连接共用螺栓
									if(pLsRef->feature==0)
									{	//第一次设计该螺栓参数
										pBolt->EmptyL0DesignPara();		//通厚
										pBolt->AddL0Thick(pDatumJg->handle,TRUE,TRUE);
										pBolt->AddL0Thick(pMirPlate->handle,TRUE,TRUE);
										pBolt->des_base_pos.norm_offset.EmptyThick(); //法向偏移量
										pLsRef->feature=1;	//该螺栓引用对应的螺栓参数已清空过
									}
									pBolt->AddL0Thick(pMirLineAngle->handle,TRUE,TRUE);
									if(pBolt->des_base_pos.hPart==pDatumJg->handle)	//1.1定位角钢为基准角钢
										pBolt->des_base_pos.norm_offset.AddThick(-pDatumJg->handle,TRUE,TRUE);
									if(pBolt->des_base_pos.hPart==pMirLineAngle->handle)
									{	//1.2定位角钢为当前角钢
										if((pMirLjPart->angle.cur_wing_x0_y1==0&&ddx>0)
											||(pMirLjPart->angle.cur_wing_x0_y1==1&&ddy>0))	//里铁
										{
											if(pBolt->des_work_norm.direction==1)
											{	//朝内
												pBolt->des_base_pos.norm_offset.AddThick(-pDatumJg->handle,TRUE,TRUE);
												pBolt->des_base_pos.norm_offset.AddThick(-pMirPlate->handle,TRUE,TRUE);
											}
											else //朝外 
												pBolt->des_base_pos.norm_offset.AddThick(-pMirLineAngle->handle,TRUE,TRUE);
										}
										else 
										{	//外铁
											if(pBolt->des_work_norm.direction==0)//朝外
												pBolt->des_base_pos.norm_offset.AddThick(-pMirLineAngle->handle,TRUE,TRUE);
											else
											{	//朝内
												pBolt->des_base_pos.norm_offset.AddThick(-pDatumJg->handle,TRUE,TRUE);
												pBolt->des_base_pos.norm_offset.AddThick(-pMirPlate->handle,TRUE,TRUE);
											}
										}
									}
									//1.3定位角钢为另外一根(外铁)射线角钢
									else if((pMirLjPart->angle.cur_wing_x0_y1==0&&ddx>0)
										||(pMirLjPart->angle.cur_wing_x0_y1==1&&ddy>0))	//里铁
									{
										if(pBolt->des_work_norm.direction==0)	//朝外
											pBolt->des_base_pos.norm_offset.AddThick(-pMirLineAngle->handle,TRUE);
									}
									else //外铁
									{
										if(pBolt->des_work_norm.direction==1)	//朝内
											pBolt->des_base_pos.norm_offset.AddThick(-pMirLineAngle->handle,TRUE);
									}

									pBolt->correct_pos();
									pBolt->CalGuigeAuto();
									pBolt->Create3dSolidModel();
									g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
									continue; //继续
								}
								if(pBolt->des_base_pos.hPart!=pMirLineAngle->handle)//定位角钢为当前角钢
									continue;
								//2.端连接非共用螺栓 
								pBolt->EmptyL0DesignPara();		//通厚
								pBolt->des_base_pos.norm_offset.EmptyThick(); //法向偏移量
								pBolt->AddL0Thick(pMirLineAngle->handle,TRUE,TRUE);
								pBolt->AddL0Thick(pMirPlate->handle,TRUE,TRUE);
								PARTSET actset;
								actset.append(pMirLineAngle);
								actset.append(pMirPlate);
								pBolt->des_base_pos.norm_offset.key_str=CLDSBolt::EmendZOffsetStr(pBolt,actset);
								if(pBolt->DianQuan.N>0&&pBolt->DianQuan.thick>0&&!pBolt->IsNeedGasket(actset))
								{	//判断螺栓是否需要垫圈
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
				//计算角钢位置后重新计算切角切肢信息
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
							continue;	//若子角钢为虚构件则继续
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
				//此处不要将对称生成的板的相关杆件显示出来，否则用户可能会因为未看到这
				//些杆件框选时错误选择，导致其他操作判断不准确，而又不容易发现原因何在
				//g_pSolidDraw->NewSolidPart(pMirLineAngle->GetSolidPartObject());
			}
			else if(pMirLjPart->pCurLinePart&&pMirLjPart->pCurLinePart->GetClassTypeId()==CLS_LINETUBE)
			{
				CLDSLineTube *pSrcLineTube=(CLDSLineTube*)pLjPart->pCurLinePart;
				CLDSLineTube *pMirLineTube=(CLDSLineTube*)pMirLjPart->pCurLinePart;
				CTubeEndPosPara *pPosDesign=NULL;
				if((pMirLjPart->start0_end1==0&&pMirLjPart->pCurLinePart->IsStartPosLocked())||
					(pMirLjPart->start0_end1==1&&pMirLjPart->pCurLinePart->IsEndPosLocked()))
					continue;	//始/终端位置锁定后不能调整
				//非始端连接亦非终端连接则继续 不需要调整始终端位置
				if(pLjPart->start0_end1!=0&&pLjPart->start0_end1!=1)
					continue;
				TUBEJOINT *pMirTubeJoint=NULL;
				//调整对称钢管的位置
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
				//对称生成钢管上的插板
				TUBEJOINT *pSrcTubeJoint=NULL;
				if(pLjPart->start0_end1==0)
					pSrcTubeJoint=&pSrcLineTube->m_tJointStart;
				else 
					pSrcTubeJoint=&pSrcLineTube->m_tJointEnd;
				if(pSrcTubeJoint&&pSrcTubeJoint->hLinkObj>0)	
				{	//源钢管端存在插板
					CLDSParamPlate *pSrcParamPlate=NULL, *pMirParamPlate=NULL;
					pSrcParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pSrcTubeJoint->hLinkObj,CLS_PARAMPLATE);
					if(pMirTubeJoint&&pMirTubeJoint->hLinkObj>0)	//对称钢管相应端存在插板
						pMirParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pMirTubeJoint->hLinkObj,CLS_PARAMPLATE);
					if(pSrcParamPlate&&pMirParamPlate&&pSrcParamPlate->m_iParamType==pMirParamPlate->m_iParamType
						&&(pSrcParamPlate->m_iParamType==TYPE_XEND||pSrcParamPlate->m_iParamType==TYPE_UEND||
						pSrcParamPlate->m_iParamType==TYPE_ROLLEND))
					{	//将对应的螺栓引入到插板上
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
				//console.DispPartSet.DeleteNode(pMirPlate->handle);	//已移至console.DeletePart中
				console.DeletePart(pMirPlate->handle);
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("fail to meet symmetric rod, can not finish symmetry!");
#else
				AfxMessageBox(CXhChar200("未找到杆件0x%X合适的匹配对称杆件，无法完成对称!",pLjPart->hPart));
#endif
				return NULL;
			}
		}
		//计算正负头 此处应该计算对称钢板射线杆件的正负头 wht 10-01-27
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
		if(pMirPlate->jdb_style==5)	//相对与基准构件位置不动的板
			pMirPlate->DesignSetupUcs();
		//pMirPlate->DesignPlate();  //此处不可以重新设计否则修改过的板的外形保存不上 
		if( pBaseLinePart&&pBaseLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE&&
			pMirBaseLinePart&&pMirBaseLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//基准角钢为组合角钢且1号角钢无对称关系时,需要重新设计钢板(如:T型组合角钢横担上下隔面钢板前后对称) wht 16-08-09
			long mir_handle=0;
			MirPartHandle(pSrcPlate->BelongModel(),&mir_handle,((CLDSGroupLineAngle*)pBaseLinePart)->m_hDatumSonAngle,msg,&pSrcPlate->ucs.origin);
			if(!bDatumJgStyleDiff&&mir_handle!=((CLDSGroupLineAngle*)pMirBaseLinePart)->m_hDatumSonAngle)
				bDatumJgStyleDiff=TRUE;	
		}
		if(bDatumJgStyleDiff) //基准角钢里外铁形式不一致应重新设计钢板外形 wht 11-01-17
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
	BOOL bSucceed=TRUE;	//是否成功对称生成所有构件
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
	if((mirmsg.axis_flag&8)==0)	//无旋转对称
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
		{	//进行多次旋转对称时基准构件为同一个构件，只需要调整旋转角度即可
			if(pSrcPlate==NULL)
				continue;
			//调整旋转角度
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
				{	//避免重复添加关联对象
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
	//设置对称生成的对象彼此关联
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
		//	pMirPlate->ucs.axis_x*=-1.0;	//参数化板与普通钢板不同，后续代码中进行了Y轴翻转 wjh-2015.9.21
		pMirPlate->ucs.axis_z=GetMirVector(pSrcParamPlate->ucs.axis_z,msg);
		AddMirObjToSrcObjRelaObjList(pSrcParamPlate,pMirPlate,msg);	//添加对称生成的对象到源对象的关联对象列表
		if (pMirObj==NULL)
		{	//新对称参数化板对象
			CMirOperObject::MIRROR::MIROBJ mirobj;
			mirobj.ciMirFlag=msg.axis_flag;
			mirobj.siRotDegAng=msg.rotate_angle;
			mirobj.dwStateFlag=0;
			mirobj.pMirObj=pMirPlate;
			pMirObj=pMirror->xarrMirObjs.Append(mirobj);
		}
		//提前是为了防止一组装配方位相同钢板在初次未含附加信息对称后，进行二次附件信息对称时查找对称钢板错误 wjh-2015.9.21
		PROFILE_VERTEX *pPrevVertex=pSrcParamPlate->vertex_list.GetTail();
		for(PROFILE_VERTEX *pVertex=pSrcParamPlate->vertex_list.GetFirst();pVertex;pVertex=pSrcParamPlate->vertex_list.GetNext())
		{	//如果 为相对于基准构件位置不动的板直接把顶点列表复制过去即可
			//if(msg.axis_flag&4||msg.axis_flag&8)	//Z轴对称或旋转对称
			//{	此处对称顶点完全是为了下第二次进行含信息对称时，能够正确找到对称板，否则配对的塔脚靴板查找不正确 wjh-2015.9.21
				PROFILE_VERTEX vertex=*pVertex;
				if(vertex.type==3)	//椭圆弧
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
				//由于Ｘ轴、Ｙ轴对称时圆心拾取点坐标也应进行相应变换
				if(vertex.type==2)	//指定圆弧半径及拾取点
					vertex.center.x*=-1;
				else if(vertex.type==3)	//椭圆弧
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
		{	//对称操作前已存在对称螺栓对象，无须更新螺栓相关信息，仅进行关联即可 wjh-2020.1.14
			CMirOperObject::MIRROR::MIROBJ mirobj;
			mirobj.ciMirFlag=msg.axis_flag;
			mirobj.siRotDegAng=msg.rotate_angle;
			mirobj.dwStateFlag=CMirOperObject::MIRBOLT_ALL;
			mirobj.pMirObj=pMirPlate;
			pMirObj=pMirror->xarrMirObjs.Append(mirobj);
		}
		if (msg.axis_flag&8)
		{	//旋转对称时，保证对称板的设计参数与原板(pSrcParamPlate)设计参数一致
			pMirPlate->CopyProperty(pSrcParamPlate);
		}
	}
	//第一次对称应对称图层信息，由于默认图层名为SPQ，之前对于称腿部构件在GetMirPart()函数中
	//调用UnifiedCfgword()后配材号将错误修正为255,第二次对称图层正确后配材号将变为2:A-H wht 15-07-02
	CalMirAtomLayer(pSrcParamPlate->layer(),pMirPlate->layer(),msg);
	if(!bIncAttachInfo)
		return pMirPlate;
	CLDSLineTube* pTube=(CLDSLineTube*)Ta.Parts.FromHandle(pSrcParamPlate->m_hPartWeldParent,CLS_LINETUBE);
	f3dPoint pickPos,*pPickPoint=NULL;	//判断是始端连接还是终端连接
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
			if(pSrcParamPlate->GetDesignItemValue('L',&L))//获得基准底板
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
						pMirPlate->SetDesignItemValue('K',-pMirDatumTube->handle,"肘板所依靠的基准钢管",PARAM_POS,1);
					else
						pMirPlate->SetDesignItemValue('K',pMirDatumTube->handle,"肘板所依靠的基准钢管",PARAM_POS,1);
					pMirPlate->SetDesignItemValue('L',pMirBottomPlate->handle,"基准底板",PARAM_POS,1);
#endif
					//更新肘板的定位方向角 wxc-2015.3.8
					f3dPoint axis_mir_x=GetMirVector(pSrcParamPlate->ucs.axis_x,msg);	//对称肘板的X轴方向
					project_vector(axis_mir_x,pMirDatumTube->ucs.axis_z);
					normalize(axis_mir_x);
					//计算对称肘板在对称基准钢管上的定位角度
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
					pMirPlate->SetDesignItemValue('Y',angle,"肘板定位方向角",PARAM_POS);
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
						logerr.Log("0X%X钢管的对称钢管未找到!",abs(K));
					if(!pMirBottomPlate.IsHasPtr())
						logerr.Log("0X%X钢板的对称钢板未找到!",L);
#endif
					//console.DispPartSet.DeleteNode(pMirPlate->handle);
					console.DeletePart(pMirPlate->handle);
					return NULL;
				}
				//将肘板存放在法兰盘的依附构件链表里	
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
			pMirPlate->SetDesignItemValue('x',columnnorm.x,"钢管方向X分量");
			pMirPlate->SetDesignItemValue('y',columnnorm.y,"钢管方向Y分量");
			pMirPlate->SetDesignItemValue('z',columnnorm.z,"钢管方向Z分量");
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
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",C);
			else if(e==0)
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",E);
			else if(f==0)
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",F);
			else if(g==0)
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",G);
			AfxMessageBox(error);
#endif
			return NULL;
		}
		pMirPlate->SetDesignItemValue('C',c,"",PARAM_POS,1);	//定位基准钢管
		pMirPlate->SetDesignItemValue('E',e,"",PARAM_POS,1);	//定位基准节点
		//第一块基准钢板
		CLDSGeneralPlate::DESIGN_PARAM para_item1;
		pSrcParamPlate->GetDesignItemValue('F',&para_item1);
		para_item1.value.hPart=f;
		if(para_item1.cValueType!=3)
			pMirPlate->SetDesignItemValue('F',f,"",PARAM_POS,1);
		else //if(para_item1.cValueType==3)
			pMirPlate->SetDesignItem('F',para_item1,"",PARAM_POS);
		//第二块基准钢板
		CLDSGeneralPlate::DESIGN_PARAM para_item2;
		pSrcParamPlate->GetDesignItemValue('G',&para_item2);
		para_item2.value.hPart=g;
		if(para_item2.cValueType!=3)
			pMirPlate->SetDesignItemValue('G',g,"",PARAM_POS,1);	
		else //if(para_item2.cValueType==3)
			pMirPlate->SetDesignItem('G',para_item2,"",PARAM_POS);
		//覆盖板
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
		pSrcParamPlate->GetDesignItemValue('P',&P);	//'A'改为'P' wht 11-05-17
		pSrcParamPlate->GetDesignItemValue('E',&E);	//'B'改为'E'
		pSrcParamPlate->GetDesignItemValue('F',&F);	//'C'改为'F'
		pSrcParamPlate->GetDesignItemValue('C',&C);	//基准钢管
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
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",C);
			else if(e==0)
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",E);
			else if(f==0)
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",F);
			else if(p==0)
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",P);
			AfxMessageBox(error);
#endif
			return NULL;
		}
		//第一块基准钢板
		CLDSGeneralPlate::DESIGN_PARAM para_item1;
		pSrcParamPlate->GetDesignItemValue('E',&para_item1);
		para_item1.value.hPart=e;
		if(para_item1.cValueType!=3)
			pMirPlate->SetDesignItemValue('E',e,"",PARAM_POS,1);
		else //if(para_item1.cValueType==3)
			pMirPlate->SetDesignItem('E',para_item1,"",PARAM_POS);
		//第二块基准钢板
		CLDSGeneralPlate::DESIGN_PARAM para_item2;
		pSrcParamPlate->GetDesignItemValue('F',&para_item2);
		para_item2.value.hPart=f;
		if(para_item2.cValueType!=3)
			pMirPlate->SetDesignItemValue('F',f,"",PARAM_POS,1);	
		else //if(para_item2.cValueType==3)
			pMirPlate->SetDesignItem('F',para_item2,"",PARAM_POS);
		//
		pMirPlate->SetDesignItemValue('C',c,"",PARAM_POS,1);	//定位基准钢管
		pMirPlate->SetDesignItemValue('P',p,"",PARAM_POS,1);	//定位基准节点
		//肋板连接方向(环向肋板，普通环向板和异形法兰) wht 11-05-17
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
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",C);
			else if(e==0)
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",E);
			else if(f==0)
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",F);
			else if(p==0)
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",P);
			AfxMessageBox(error);
#endif
			return NULL;
		}
		//第一块基准钢板
		CLDSGeneralPlate::DESIGN_PARAM para_item1;
		pSrcParamPlate->GetDesignItemValue('E',&para_item1);
		para_item1.value.hPart=e;
		if(para_item1.cValueType!=3)
			pMirPlate->SetDesignItemValue('E',e,"",PARAM_POS,1);
		else //if(para_item1.cValueType==3)
			pMirPlate->SetDesignItem('E',para_item1,"",PARAM_POS);
		//第二块基准钢板
		CLDSGeneralPlate::DESIGN_PARAM para_item2;
		pSrcParamPlate->GetDesignItemValue('F',&para_item2);
		para_item2.value.hPart=f;
		if(para_item2.cValueType!=3)
			pMirPlate->SetDesignItemValue('F',f,"",PARAM_POS,1);	
		else //if(para_item2.cValueType==3)
			pMirPlate->SetDesignItem('F',para_item2,"",PARAM_POS);
		//
		pMirPlate->SetDesignItemValue('C',c,"",PARAM_POS,1);	//定位基准钢管
		pMirPlate->SetDesignItemValue('P',p,"",PARAM_POS,1);	//定位基准节点
		//不知道王海涛当初是何意思了，现在感觉不需要调整W,N,及Y参数，否则十字插板中跨越连板Z轴对称错误 wjh 2011-11-10
		/*CLDSPlate *pDatumPlate=(CLDSPlate*)Ta.Parts.FromHandle(E,CLS_PLATE);
		CLDSPlate *pMirDatumPlate=(CLDSPlate*)Ta.Parts.FromHandle(e,CLS_PLATE);
		if(pDatumPlate&&pMirDatumPlate)
		{
			if(pDatumPlate->ucs.axis_z*pMirDatumPlate->ucs.axis_z<0&&(msg.axis_flag==0x01||msg.axis_flag==0x02))
			{	//X/Y轴对称时，Z轴方向相反时W，N值需要互换
				double W=0,N=0;
				pSrcParamPlate->GetDesignItemValue('W',&W);
				pSrcParamPlate->GetDesignItemValue('N',&N);
				pMirPlate->SetDesignItemValue('W',N,"确定板外形的参数W",PARAM_SHAPE);
				pMirPlate->SetDesignItemValue('N',W,"确定板外形的参数N",PARAM_SHAPE);
				//调整内外侧 wht 11-05-17 
				double Y=0;
				pMirPlate->GetDesignItemValue('Y',&Y);
				if(Y>=0)
					Y=-1;
				else
					Y=1;
				pMirPlate->SetDesignItemValue('Y',Y,"半圆型环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)",PARAM_SHAPE);
			}
		}*/
		//跨越连板的连接方向 wht 11-05-17
		pMirPlate->ConnDirectList.Empty();
		CONNECT_DIRECTION *pVec=NULL;
		for(pVec=pSrcParamPlate->ConnDirectList.GetFirst();pVec;pVec=pSrcParamPlate->ConnDirectList.GetNext())
		{
			CONNECT_DIRECTION *pMirVec=pMirPlate->ConnDirectList.SetValue(pVec->serial,*pVec);
			MirDatumVector(&Ta,pVec->direct,pMirVec->direct,msg);
		}
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
	{	//角钢肋板
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
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",A);
			else if(b==0)
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",B);
			AfxMessageBox(error);
			return NULL;
		}
		pMirPlate->SetDesignItemValue('A',a,"基准角钢",PARAM_POS,1);
		pMirPlate->SetDesignItemValue('B',b,"基准节点",PARAM_POS,1);
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
			AfxMessageBox(CXhChar200("未找到构件0x%X合适的匹配对称构件，无法完成对称!", A));
#endif
			return NULL;
		}
		pMirPlate->SetDesignItemValue('A', a, "", PARAM_POS, 1);
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
	{	//遮水板
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
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",C);
			else if(b==0)
				error.Printf("未找到构件0x%X合适的匹配对称构件，无法完成对称!",B);
			return NULL;
		}
		pMirPlate->SetDesignItemValue('C',c,"基准钢管",PARAM_POS,1);
		pMirPlate->SetDesignItemValue('B',b,"基准节点",PARAM_POS,1);
#endif
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_UEND||pSrcParamPlate->m_iParamType==TYPE_ROLLEND)
	{	//根据对称得到的Z轴方向调整插板旋转角度
		CLDSLineTube *pMirLineTube=(CLDSLineTube*)Ta.FromPartHandle(pMirPlate->m_hPartWeldParent,CLS_LINETUBE);
		if(pMirLineTube==NULL)
		{
			AfxMessageBox("没有找到对称插板的焊接钢管，对称失败!");
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
		//对称钢板坐标原点设计信息
		pMirPlate->design.inited=pSrcParamPlate->design.inited;
		MirDatumPoint(&Ta,pSrcParamPlate->design.origin,pMirPlate->design.origin,msg);
		//更新塔脚主材的正负头
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
		//获取母板上的依赖角钢集合，更新对称角钢的关联属性信息(正负头、切角)
		ATOM_LIST<THANDLE> angleHandleList;
		long hBaseNode=0,hBasePlate=0,hBaseAngle=0,hConnectShoePlate=0,hRayAngleH=0,hRayAngleX=0;	//不赋初始值，后续代码会误判存在有效值 wjh-2015.9.21
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
		{	//靴板中的'W'参数表示靴板在底板的沿向坐标轴，在进行X/Y对称时，应X/Y调换 wjh-2015.9.21
			CLDSParamPlate::DESIGN_PARAM param;
			if(pSrcParamPlate->GetDesignItemValue(KEY2C("W"),&param) && (param.ToLong()==0||param.ToLong()==1))
				pMirPlate->SetDesignItemValue(KEY2C("W"),1-param.ToLong(),NULL,param.cParamType,param.cValueType);
		}
		/*{	//X轴、Y轴对称靴板时，需要调整对称方式为旋转对称 wht 15-04-17
			if(pBasePlate->ucs.axis_x.IsEqual(f3dPoint(1))||pBasePlate->ucs.axis_x.IsEqual(f3dPoint(-1)))
			{	//源靴板在1、4象限,X轴对称应转换为顺时针旋转90度，Y轴对称应转换为逆时针旋转90
				if(msg.axis_flag==0x01)
					msg.rotate_angle=-90;
				else
					msg.rotate_angle=90;
			}
			else //if(pBasePlate->ucs.axis_x.IsEqual(f3dPoint(0,1,0))||pBasePlate->ucs.axis_x.IsEqual(f3dPoint(0,-1,0)))
			{	//源靴板在2、3象限,X轴对称应转换为逆时针旋转90度，Y轴对称应转换为顺时针旋转90
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
		//更新对称靴板的依赖构件信息
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
	if((msg.axis_flag&4)==0&&(msg.axis_flag&8)==0)	//不是Z轴对称和旋转对称
		pMirPlate->ucs.axis_y*=-1.0;
	//
	if(pTube&&pTube->GetClassTypeId()==CLS_LINETUBE)
	{
		//在查找Z轴对称或旋转对称的杆件时不启用一端对称，横材也不能启用
		/*if(msg.axis_flag!=4&&msg.axis_flag!=8&&cosa<EPS_COS2)
			pMirPart=pPart->GetMirPart(msg,scope_start0_end1_elseall);
		else
			pMirPart=pPart->GetMirPart(msg);*/
		double* pPickCoord=NULL;
		if(pPickPoint)
			pPickCoord=(double*)(*pPickPoint);
		CLDSLineTube *pMirTube=(CLDSLineTube*)pTube->GetMirPart(msg,pPickCoord);//启用一端对称查找杆件
		//下面关与对称有效性检查的十行代码可能随对称算法的改变已不再需要,观察一段时间后去掉 wjh-2011.8.9
		CLDSNode *pMirStartNode=pTube->pStart->GetMirNode(msg);
		CLDSNode *pMirEndNode=pTube->pEnd->GetMirNode(msg);
		if(pMirTube==NULL||(pickPos==pTube->pStart->Position(false)&&pMirStartNode==NULL)||
			(pickPos==pTube->pEnd->Position(false)&&pMirEndNode==NULL)||(pPickPoint==NULL&&(pMirStartNode==NULL||pMirEndNode==NULL)))
		{	//源杆件如果为始端连接则必须存在与始端节点对应的节点	wht 10-01-15
			//源杆件如果为终端端连接则必须存在与终端节点对应的节点
			//console.DispPartSet.DeleteNode(pMirPlate->handle);
			console.DeletePart(pMirPlate->handle);
			return NULL;
		}
		if(pMirTube==pTube)	//对称杆件为本身
		{
			if(pickPos==pTube->pStart->Position(false))
			{	//源杆件为始端连接
				pMirTube->m_tJointEnd=pTube->m_tJointStart;
				if(pSrcParamPlate->handle==pTube->m_tJointStart.hLinkObj)
					pMirTube->m_tJointEnd.hLinkObj=pMirPlate->handle;
				pMirTube->SetEndOdd(pTube->startOdd());
				pMirTube->desEndPos.fNormOffset=pTube->desStartPos.fNormOffset;	//钢管基准面法向偏移量
			}
			else if(pickPos==pTube->pEnd->Position(false))
			{	//源杆件为终端连接
				pMirTube->m_tJointStart=pTube->m_tJointEnd;
				if(pSrcParamPlate->handle==pTube->m_tJointEnd.hLinkObj)
					pMirTube->m_tJointStart.hLinkObj=pMirPlate->handle;
				pMirTube->SetStartOdd(pTube->endOdd());
				pMirTube->desStartPos.fNormOffset=pTube->desEndPos.fNormOffset;	//钢管基准面法向偏移量
			}
		}
		else
		{
			if(pickPos==pTube->pStart->Position(false)&&pMirStartNode&&pMirStartNode==pMirTube->pStart)
			{	//对称杆件起端对应源杆件起端
				pMirTube->m_tJointStart=pTube->m_tJointStart;
				if(pSrcParamPlate->handle==pTube->m_tJointStart.hLinkObj)
					pMirTube->m_tJointStart.hLinkObj=pMirPlate->handle;
				pMirTube->SetStartOdd(pTube->startOdd());
				pMirTube->desStartPos.fNormOffset=pTube->desStartPos.fNormOffset;	//钢管基准面法向偏移量
			}
			else if(pickPos==pTube->pEnd->Position(false)&&pMirEndNode&&pMirEndNode==pMirTube->pEnd)
			{	//对称杆件终端对应源杆件终端
				pMirTube->m_tJointEnd=pTube->m_tJointEnd;
				if(pSrcParamPlate->handle==pTube->m_tJointEnd.hLinkObj)
					pMirTube->m_tJointEnd.hLinkObj=pMirPlate->handle;
				pMirTube->SetEndOdd(pTube->endOdd());
				pMirTube->desEndPos.fNormOffset=pTube->desEndPos.fNormOffset;	//钢管基准面法向偏移量
			}
			else if(pickPos==pTube->pStart->Position(false)&&pMirStartNode&&pMirStartNode==pMirTube->pEnd)
			{	//对称杆件起端对应源杆件终端
				pMirTube->m_tJointEnd=pTube->m_tJointStart;
				if(pSrcParamPlate->handle==pTube->m_tJointStart.hLinkObj)
					pMirTube->m_tJointEnd.hLinkObj=pMirPlate->handle;
				pMirTube->SetEndOdd(pTube->startOdd());
				pMirTube->desEndPos.fNormOffset=pTube->desStartPos.fNormOffset;	//钢管基准面法向偏移量
			}
			else if(pickPos==pTube->pEnd->Position(false)&&pMirEndNode&&pMirEndNode==pMirTube->pStart)
			{	//对称杆件终端对应源杆件起端
				pMirTube->m_tJointStart=pTube->m_tJointEnd;
				if(pSrcParamPlate->handle==pTube->m_tJointEnd.hLinkObj)
					pMirTube->m_tJointStart.hLinkObj=pMirPlate->handle;
				pMirTube->SetStartOdd(pTube->endOdd());
				pMirTube->desStartPos.fNormOffset=pTube->desEndPos.fNormOffset;	//钢管基准面法向偏移量
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
	pMirPlate->DesignPlate(false);	//不能设计螺栓，因为螺栓一动可能影响到螺栓的对称
	//添加对称生成的对象到源对象的关联对象列表
	AddMirObjToSrcObjRelaObjList(pSrcParamPlate,pMirPlate,msg);
	//
	if(pSrcParamPlate->m_iParamType==TYPE_XEND)
	{	//对称十字插板时 应同时对称生成以十字插板定位的立板 
		//否则对称生成插板上的螺栓时无法找到定位构件 wht 10-03-18
		CLDSPlate *pMirVertPlate[2]={NULL,NULL},*pSrcVertPlate[2]={NULL,NULL};
		pSrcParamPlate->GetVertPlateInXEndPlate(&pSrcVertPlate[0],&pSrcVertPlate[1]);
		//首先对称十字插板的立板,立板以十字插板基板为定位基准构件;
		//接着对称十字插板的包板,包板以十字插板基板或者立板为定位基准构件;
		//最后对称生成十字插板螺栓,水平螺栓以基板为基准构件,竖直螺栓以立板为基准构件。wht 10-08-03
		for(int i=0;i<2;i++)
		{
			if(pSrcVertPlate[i]==NULL)
				continue;
			pMirVertPlate[i]=CreateSingleMirPlate(pSrcVertPlate[i],msg);
			//目前发现此处同值会将原有值错误充掉，故注释掉(如原板为变坡口横材插板时法线为交叉杆件确定) wjh-2014.8.23
			//pMirVertPlate[i]->designInfo.origin.des_para.hPart=pMirPlate->handle;	
			//pMirVertPlate[i]->designInfo.norm.hVicePart=pMirPlate->handle;
			pMirVertPlate[i]->DesignSetupUcs();
			pMirPlate->AppendAttachPart(pMirVertPlate[i]);	//添加到附加构件链表中
		}
		ATTACHPART_ITEM *pItem=NULL;
		for (pItem=pSrcParamPlate->GetFirstAttachPart();pItem;pItem=pSrcParamPlate->GetNextAttachPart())
		{
			CLDSPart *pPart=Ta.Parts.FromHandle(pItem->h);
			if(pPart==NULL)
				continue;
			if(pPart->GetClassTypeId()==CLS_PLATE)
			{	//包板
				CLDSPlate *pAttachPlate=(CLDSPlate*)pPart;
				if(pAttachPlate==pSrcVertPlate[1]||pAttachPlate==pSrcVertPlate[0])
					continue;
				if(pAttachPlate->designInfo.origin.datum_pos_style==9	//指定构件上的相对坐标
					||pAttachPlate->designInfo.norm.norm_style==4)		//指定构件上的相对方向
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
					pMirPlate->AppendAttachPart(pMirAttachPlate);	//添加到附加构件链表中
				}
			}
			else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			{	//包钢
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
					//XY轴对称包钢时有问题 wht 10-08-03
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
						if(start_pos.z>0&&pMirVertPlate[0])	//上侧角钢
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
						//else	//指定肢法线方向
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
					pMirPlate->AppendAttachPart(pMirLineAngle);	//添加到附加构件链表中
				}
			}
		}
	}
	//连板螺栓对称
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
			{	//纠正在对称钢板时生成的螺栓的定位基准构件以及法线基准构件 wht 10-09-27
				//源螺栓定位构件以及法线基准构件为源插板时执行以下操作
				if(pMirBolt->des_base_pos.hPart==0&&(*pLsRef)->des_base_pos.hPart==pSrcParamPlate->handle)
					pMirBolt->des_base_pos.hPart=pMirPlate->handle;
				if(pMirBolt->des_work_norm.hVicePart==0&&(*pLsRef)->des_work_norm.hVicePart==pSrcParamPlate->handle)
					pMirBolt->des_work_norm.hVicePart=pMirPlate->handle;
				/*if (pLsRef->pBolt->des_base_pos.hPart==pSrcParamPlate->handle)
				{	//由于对称连接板时可能先对称螺栓，此时插板还未对称可能导致局部定位的螺栓位置参数错误，需要在此时更新 wjh-2020.1.14
					GEPOINT mir_axis_x=GetMirVector(pSrcParamPlate->ucs.axis_x,msg);
					GEPOINT mir_axis_y=GetMirVector(pSrcParamPlate->ucs.axis_y,msg);
					if (mir_axis_x*pMirPlate->ucs.axis_x<0)	//对称后的Ｘ轴已进行了反向
						pMirBolt->des_base_pos.len_offset_dist*=-1.0;
					if (mir_axis_y*pMirPlate->ucs.axis_y<0)	//对称后的Ｙ轴已进行了反向
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
	//此处不应该对插板螺栓到位，因为可能螺栓还未完善设计参数，
	//到位后螺栓位置错误,导致无法根据位置找到对称螺栓 wht 16-01-30
	/*for(pLsRef=pMirPlate->GetFirstLsRef();pLsRef;pLsRef=pMirPlate->GetNextLsRef())
	{	//应在所有螺栓对称生成后再到位,十字插板上的螺栓并不是完全按位置对称的
		//(钢管基准边不同时关联螺栓不位置完全对称) wht 11-01-23
		if(pMirPlate->m_iParamType==TYPE_XEND)
		{	//到位十字插板上的螺栓 wht 10-08-03
			CLDSBolt *pMirBolt=pLsRef->GetLsPtr();
			pMirBolt->correct_worknorm();
			pMirBolt->correct_pos();
			pMirBolt->CalGuigeAuto();
			pMirBolt->SetModified();
			pMirBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pMirBolt->GetSolidPartObject());
		}
	}*/
	//螺栓数大于0或者通过对称计算的Z轴方向与计算得到的Z轴相反 wht 11-05-17
	if(pMirPlate->m_iParamType==TYPE_CIRRIBPLATE||pMirPlate->m_iParamType==TYPE_TUBERIBPLATE)
	{	
		double S=0,T=0;
		pMirPlate->GetDesignItemValue('S',&S);
		pMirPlate->GetDesignItemValue('T',&T);
		//根据对称得到的Z轴方向
		f3dPoint axis_z=GetMirVector(pSrcParamPlate->ucs.axis_z,msg);
		BOOL bReDesign=FALSE;
		if(pMirPlate->ucs.axis_z*axis_z<0&&(fabs(S)>EPS||fabs(T)>EPS))
		{	//调整对称钢板的法向偏移量,然后重新设计
#ifdef AFX_TARG_ENU_ENGLISH
			pMirPlate->SetDesignItemValue('S',-1.0*S,"offset distance in datum tube's extending direction.",PARAM_POS);
			pMirPlate->SetDesignItemValue('T',-1.0*T,"offset distance in circular plate's normal direction.",PARAM_POS);
#else
			pMirPlate->SetDesignItemValue('S',-1.0*S,"沿基准钢管延伸方向的偏移距离。",PARAM_POS);
			pMirPlate->SetDesignItemValue('T',-1.0*T,"沿环向板法线方向的偏移距离。",PARAM_POS);
#endif
			bReDesign=TRUE;
		}
		else if(pMirPlate->GetLsCount()>0)
			bReDesign=TRUE;	//螺栓数大于0,会影响钢板外形故需要重新设计
		if(bReDesign)	
			pMirPlate->DesignPlate(false);
	}
	/*else if(pMirPlate->m_iParamType==TYPE_RIBPLATE)
	{
		//根据对称得到的Y轴及Z轴方向
		f3dPoint axis_x=GetMirVector(pSrcParamPlate->ucs.axis_x,msg);
		f3dPoint axis_z=GetMirVector(pSrcParamPlate->ucs.axis_z,msg);
		f3dPoint huoqu_norm=GetMirVector(pSrcParamPlate->HuoquFaceNorm(),msg);
		double X=0,Z=0,dx=0,dz=0;
		pMirPlate->GetDesignItemValue('X',&X);	//X方向偏移量
		pMirPlate->GetDesignItemValue('Z',&Z);	//Z方向偏移量
		pMirPlate->GetDesignItemValue(KEY2C("dx"),&dx);	//制弯面末段自定位基点的法向偏移量
		pMirPlate->GetDesignItemValue(KEY2C("dz"),&dz);	//制弯面末段自定位基点的法向偏移量
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
	{	//靴板需要根据螺栓重新设计外形
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
	BOOL bSucceed=TRUE;	//是否成功对称生成所有构件
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
	if((mirmsg.axis_flag&8)==0)	//无旋转对称
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
		{	//进行多次旋转对称时基准构件为同一个构件，只需要调整旋转角度即可
			if(pSrcParamPlate==NULL)
				continue;
			//调整旋转角度
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
				{	//避免重复添加关联对象
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
	//设置对称生成的对象彼此关联
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
		AddMirObjToSrcObjRelaObjList(pSrcSphere,pMirSphere,msg);	//添加对称生成的对象到源对象的关联对象列表
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
	BOOL bSucceed=TRUE;	//是否成功对称生成所有构件
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
	if((mirmsg.axis_flag&8)==0)	//无旋转对称
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
		{	//进行多次旋转对称时基准构件为同一个构件，只需要调整旋转角度即可
			if(pSrcSphere==NULL)
				continue;
			//调整旋转角度
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSSphere *pMirSphere=CreateSingleMirSphere(pSrcSphere,mirmsg,bIncAttachInfo);
			if(pMirSphere)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcsphere_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcsphere_arr[0]->relativeObjs.GetNext())
				{	//避免重复添加关联对象
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
	//设置对称生成的对象彼此关联
	SetRelativeObj(srcsphere_arr[0]);
	return bSucceed;
}

CLDSPolyJoint* CreateSingleMirPolyJoint(CLDSPolyJoint* pSrcPolyJoint, MIRMSG msg, char *sErrorInfo=NULL)
{
	CLDSPolyJoint *pMirPolyJoint=NULL;
	pMirPolyJoint=(CLDSPolyJoint*)pSrcPolyJoint->GetMirPart(msg);
	if(pMirPolyJoint)	//对称杆件变形点已经生成
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
		if(pPrevLinePart->IsAngle())	//记录火曲基准肢
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
	BOOL bSucceed=TRUE;	//是否成功对称生成所有构件
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
	if((mirmsg.axis_flag&8)==0)	//无旋转对称
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
		{	//进行多次旋转对称时基准构件为同一个构件，只需要调整旋转角度即可
			if(pSrcPolyJoint==NULL)
				continue;
			//调整旋转角度
			mirmsg.rotate_angle=init_angle*(j+1);
			CLDSPolyJoint *pMirPolyJoint=CreateSingleMirPolyJoint(pSrcPolyJoint,mirmsg);
			if(pMirPolyJoint)
			{
				BOOL bInList=FALSE;
				for(pRelaObj=srcPolyJoint_arr[0]->relativeObjs.GetFirst();pRelaObj;pRelaObj=srcPolyJoint_arr[0]->relativeObjs.GetNext())
				{	//避免重复添加关联对象
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
		//根据金具所在位置，确认左右挂孔
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
		{	//水平方向金具
			if (ciQuad == 1 || ciQuad == 2)
			{	//前侧
				pResidePartL = (xBasePt1.x < xBasePt2.x) ? pResidePart1 : pResidePart2;
				pResidePartR = (xBasePt1.x < xBasePt2.x) ? pResidePart2 : pResidePart1;
			}
			else
			{	//后侧
				pResidePartL = (xBasePt1.x > xBasePt2.x) ? pResidePart1 : pResidePart2;
				pResidePartR = (xBasePt1.x > xBasePt2.x) ? pResidePart2 : pResidePart1;
			}
		}
		else
		{	//垂直方向金具
			if (ciQuad == 1 || ciQuad == 3)
			{	//右侧
				pResidePartL = (xBasePt1.y > xBasePt2.y) ? pResidePart1 : pResidePart2;
				pResidePartR = (xBasePt1.y > xBasePt2.y) ? pResidePart2 : pResidePart1;
			}
			else
			{	//左侧
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
	BOOL bSucceed = TRUE;	//是否成功对称生成所有构件
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
	if ((mirmsg.axis_flag & 8) == 0)	//无旋转对称
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
		{	//进行多次旋转对称时基准构件为同一个构件，只需要调整旋转角度即可
			if (pSrcFitting == NULL)
				continue;
			//调整旋转角度
			mirmsg.rotate_angle = init_angle * (j + 1);
			CLDSFitting *pMirFitting = CreateSingleMirFitting(pSrcFitting, mirmsg);
			if (pMirFitting)
			{
				BOOL bInList = FALSE;
				for (pRelaObj = srcFitting_arr[0]->relativeObjs.GetFirst(); pRelaObj; pRelaObj = srcFitting_arr[0]->relativeObjs.GetNext())
				{	//避免重复添加关联对象
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
		{	//对称生成杆件变形点
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

//该代码已移植到LineTube.cpp中的CLDSLineTube::SyncMirTransectFaceTo函数中
//BOOL CreateSingleMirTubeTransectFace(CLDSLineTube *pSrcTube,MIRMSG msg,BOOL bStart,BOOL bEnd, char *sErrorInfo=NULL)
