#include "stdafx.h"
#include "Tower.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

void CTower::UpdateLinePartConnectInfo(int workloadPercent)
{
	CSuperSmartPtr<CLDSLinePart> pLinePart;
	int i=0,j=0;
	for(pLinePart=Parts.GetFirstLinePart();pLinePart.IsHasPtr();pLinePart=Parts.GetNextLinePart(),i++)
	{
		if(pLinePart->GetClassTypeId()!=CLS_LINEANGLE)
			return;
		BOOL bPush=Parts.push_stack();
		for(j=0;j<2;j++)
		{
			long hFatherPart;
			CConnectInfo *pConnectInfo;
			CMultiOffsetPos *pDesPosPara;
			if(j==0)
			{	//起始端
				if(pLinePart->pStart==NULL)
					continue;
				hFatherPart=pLinePart->pStart->hFatherPart;
				pConnectInfo=&pLinePart->connectStart;
				pDesPosPara=&pLinePart.LineAnglePointer()->desStartPos;
			}
			else
			{	//终止端
				if(pLinePart->pEnd==NULL)
					continue;
				hFatherPart=pLinePart->pEnd->hFatherPart;
				pConnectInfo=&pLinePart->connectEnd;
				pDesPosPara=&pLinePart.LineAnglePointer()->desEndPos;
			}
			if(pDesPosPara->datum_jg_h>0)	//指定搭接基准角钢
				hFatherPart=pDesPosPara->datum_jg_h;
			
			if(hFatherPart==pLinePart->handle)	//端节点父杆件就是当前杆件本身，则视为主要材双肢连接
				pConnectInfo->m_iConnectWing=0;
			else if(pDesPosPara->jgber_cal_style!=0||pDesPosPara->spatialOperationStyle>=15||//非搭接定位
				pDesPosPara->IsInDatumJgBer()&&toupper(pLinePart->layer(1))=='Z')	//主材搭在另一材楞线
				pConnectInfo->m_iConnectWing=0;
			else
			{
				f3dPoint datum_norm;
				CSuperSmartPtr<CLDSLinePart> pDatumLinePart=(CLDSLinePart*)Parts.FromHandle(hFatherPart);
				if(pDatumLinePart.IsNULL()||!pDatumLinePart->IsLinePart())
					continue;
				if(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE||pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
				{
					if(pDesPosPara->IsInDatumJgWingX())
						datum_norm=pDatumLinePart.LineAnglePointer()->get_norm_x_wing();
					else if(pDesPosPara->IsInDatumJgWingY())
						datum_norm=pDatumLinePart.LineAnglePointer()->get_norm_y_wing();
				}
				else
				{
					f3dPoint datum_vec,line_vec;
					datum_vec=pDatumLinePart->End()-pDatumLinePart->Start();
					line_vec=pLinePart->End()-pLinePart->Start();
					datum_norm=datum_vec^line_vec;
					normalize(datum_norm);
				}
				if( fabs(datum_norm*pLinePart.LineAnglePointer()->get_norm_x_wing()) >
					fabs(datum_norm*pLinePart.LineAnglePointer()->get_norm_y_wing()))
					pConnectInfo->m_iConnectWing=1;	//X肢连接
				else
					pConnectInfo->m_iConnectWing=2;	//Y肢连接
			}
		}
		if(bPush)
			Parts.pop_stack();
		if(DisplayProcess)
		{
			int percent=i*workloadPercent/Parts.GetNodeNum();
			DisplayProcess(percent,NULL);
		}
	}
}

void CTower::RetrieveConnectPointInfo()
{
	long i=0,index=0,updateConnectPercent=15;
	CLDSNode *pNode;
	CSuperSmartPtr<CLDSLinePart> pLinePart;
	CHashTable<long>partSetTbl;
	partSetTbl.CreateHashTable(Node.GetNodeNum());
	LINEPARTSET* linkPartSetArr=new LINEPARTSET[Node.GetNodeNum()];
	for(pNode=Node.GetFirst();pNode;pNode=Node.GetNext(),i++)
		partSetTbl.SetValueAt(pNode->handle,i);
	for(pLinePart=Parts.GetFirstLinePart();pLinePart.IsHasPtr();pLinePart=Parts.GetNextLinePart())
	{
		if(pLinePart->pStart!=NULL&&partSetTbl.GetValueAt(pLinePart->pStart->handle,index))
			linkPartSetArr[index].append(pLinePart);
		if(pLinePart->pEnd!=NULL&&partSetTbl.GetValueAt(pLinePart->pEnd->handle,index))
			linkPartSetArr[index].append(pLinePart);
	}
	UpdateLinePartConnectInfo(updateConnectPercent);
	i=0;
	for(pNode=Node.GetFirst();pNode;pNode=Node.GetNext(),i++)
	{
		pNode->GetConnectPointInfoList()->Empty();
		if(!partSetTbl.GetValueAt(pNode->handle,index))
			continue;
		CSuperSmartPtr<CLDSPart> pDatumPart=Parts.FromHandle(pNode->hFatherPart);
		if(pDatumPart.IsNULL()||!pDatumPart->IsLinePart()||pDatumPart->GetClassTypeId()!=CLS_LINEANGLE)
			continue;
		BOOL bPush=Node.push_stack();
		CConnectPointInfo *pConnectPoint=NULL; 
		f3dPoint datum_vec,line_vec;
		datum_vec=pDatumPart.LinePartPointer()->pEnd->Position(true)-pDatumPart.LinePartPointer()->pStart->Position(true);
		for(pLinePart=linkPartSetArr[index].GetFirst();pLinePart.IsHasPtr();pLinePart=linkPartSetArr[index].GetNext())
		{
			if(pDatumPart.IsEqualPtr(pLinePart)||pLinePart->GetClassTypeId()!=CLS_LINEANGLE)
				continue;
			CMultiOffsetPos *pDesPosPara;
			if(pLinePart->pStart==pNode)
				pDesPosPara=&pLinePart.LineAnglePointer()->desStartPos;
			else if(pLinePart->pEnd==pNode)
				pDesPosPara=&pLinePart.LineAnglePointer()->desEndPos;
			line_vec=pLinePart->pEnd->Position(true)-pLinePart->pStart->Position(true);
			normalize(line_vec);
			for(pConnectPoint=pNode->GetConnectPointInfoList()->GetFirst();pConnectPoint;pConnectPoint=pNode->GetConnectPointInfoList()->GetNext())
			{
				if(!pLinePart->cfgword.And(pConnectPoint->cfgword))
					continue;
				else
					pConnectPoint->cfgword&=pLinePart->UnifiedCfgword();
				if(fabs(line_vec*pConnectPoint->work_norm)<EPS2)
					pConnectPoint->linkPartSet.append(pLinePart);
				else
				{
					if(pConnectPoint->m_iConnectWing==0 && pDesPosPara->IsInDatumJgWingX())
						pConnectPoint->linkPartSet.append(pLinePart);
					else if(pConnectPoint->m_iConnectWing==1 && pDesPosPara->IsInDatumJgWingY())
						pConnectPoint->linkPartSet.append(pLinePart);
					else
						continue;
				}
				break;
			}
			if(pConnectPoint==NULL)
			{
				pConnectPoint=pNode->GetConnectPointInfoList()->append();
				pConnectPoint->cfgword=pDatumPart->UnifiedCfgword()&pLinePart->UnifiedCfgword();
				pConnectPoint->linkPartSet.append(pLinePart);
				pConnectPoint->m_hBaseNode=pNode->handle;
				pConnectPoint->m_hBasePart=pDatumPart->handle;
				pConnectPoint->work_norm=datum_vec^line_vec;
				normalize(pConnectPoint->work_norm);
				if( fabs(pConnectPoint->work_norm*pDatumPart.LineAnglePointer()->get_norm_x_wing()) >
					fabs(pConnectPoint->work_norm*pDatumPart.LineAnglePointer()->get_norm_y_wing()))
					pConnectPoint->m_iConnectWing=0;	//连接固接在基准角钢X肢上
				else
					pConnectPoint->m_iConnectWing=1;	//连接固接在基准角钢Y肢上
			}
		}
		if(bPush)
			Node.pop_stack();
		if(DisplayProcess)
		{
			int percent=updateConnectPercent+i*(100-updateConnectPercent)/Node.GetNodeNum();
			DisplayProcess(percent,NULL);
		}
	}
	DisplayProcess(100,NULL);
	delete []linkPartSetArr;
}
#endif
