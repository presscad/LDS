//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "Tower.h"
#include "MirTaAtom.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "PromptDlg.h"
#include "dlg.h"
#include "UserDefOper.h"
#include "ScrTools.h"
#include "FootDesignDlg.h"
#include "Query.h"
#include "CalInt3dllDlg.h"
#include "3dLFDlg.h"
#include "PromptDlg.h"
#include "ShortJgDefDlg.h"
#include "CalPoleIntersPtDlg.h"
#include "SelJgPointKeyDlg.h"
#include "Sel2JgKeyPointDlg.h"
#include "Sel2BoltsDistDlg.h"
#include "LinePartExtendVecDlg.h"
#include "JointBaseJg.h"
#include "LmaDlg.h"
#include "MainFrm.h"
#include "CmdLineDlg.h"
#include "ToolKitDlg.h"
#include "DefineOffsetPosDlg.h"
#include "CorrectPlateParaDlg.h"
#include "SnapTypeVerify.h"
#include "btc.h"
#include "KeypointLifeObj.h"
#include "OriginalData.h"
#include "ClonePlate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////////////////////////////////

void CLDSView::OnToolKit()
{
	//CToolKitDlg dlg;
	//dlg.DoModal();
	CToolKitDlg *pToolKitDlg = new CToolKitDlg();
	pToolKitDlg->Create();
	pToolKitDlg->ShowModelessDlg();
}

void FindBoltRelatedParts(const CLDSBolt *pBolt,PARTSET &partSet);
void AppendAngleSectionFrameDrawing(CLDSLineAngle* pAngle,CLDSNode* pNode,int gWingX,int gWingY,BYTE ciIntersPlane=0,UCS_STRU *pObjectCS=NULL,BYTE cbHighlightFlag=0);
BOOL RetrieveParaPointByAngleHiberarchy(CLDSLineAngle* pAngle,HIBERARCHY hiberarchy,CLDSPoint* pParaPoint);
void CLDSView::OnMeasure() 
{
	Command("Dist");
	/*
	DISPLAY_TYPE type;
	g_pSolidSet->GetDisplayType(&type);
	if(type==DISP_LINE)
		DistMeasureOnDispLine();
	else
		DistMeasure();
	*/
}
int CLDSView::DistMeasureOnDispLine()
{
	CLDSDbObject* pObj1=NULL,*pObj2=NULL,*pObj3=NULL,*pObj4=NULL;
	CLDSNode* pNode=NULL;
	CLDSPart* pPart=NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr;
	//��һ���ؼ���
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Please select angle that the measured key points belong to or the corresponding node successively");
#else
	cmdStr.Format("DIST ��ѡ���������ʼ�ڵ㣬��ؼ������ڵĽǸ�:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	int state;
	CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_POINT);
	verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
	DWORD dwhObj,dwExportFlag;
	while(true)
	{
		if((state=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
		{
			pCmdLine->CancelCmdLine();
			return -1;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		dwhObj=obj.hRelaObj;
		if((pNode=Ta.Node.FromHandle(dwhObj))!=NULL)
			break;
		pPart=Ta.Parts.FromHandle(dwhObj);
		if(pPart && (pPart->GetClassTypeId()==CLS_LINEANGLE||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE))
			break;
	}
	g_pSolidDraw->SetEntSnapStatus(dwhObj);
	if(pNode)
	{
		pCmdLine->FinishCmdLine(CXhChar16("�ڵ�0x%X",pNode->handle));
		pObj1=pNode;
	}
	else if(pPart)
	{
		pCmdLine->FinishCmdLine(CXhChar16("�Ǹ�0x%X",pPart->handle));
		pObj1=pPart;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("DIST Select Key Node On Line Angle:");
#else
		cmdStr.Format("DIST ��ѡ��ǰ�Ǹ��Ϲؼ��ڵ�:");
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		verifier.ClearSnapFlag();
		verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
		while(true)
		{
			if((state=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			{
				pCmdLine->CancelCmdLine();
				return -1;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;
			if((pNode=Ta.Node.FromHandle(dwhObj))!=NULL)
				break;
		}
		g_pSolidDraw->SetEntSnapStatus(dwhObj);
		pObj2=pNode;
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pObj2->handle));
	}
	//�ڶ����ؼ���
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Please select angle that the measured key points belong to or the corresponding node successively");
#else
	cmdStr.Format("DIST ��ѡ���������ֹ�ڵ㣬��ؼ������ڵĽǸ�:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	verifier.ClearSnapFlag();
	verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
	verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
	while(true)
	{
		if((state=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
		{
			pCmdLine->CancelCmdLine();
			return -1;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		dwhObj=obj.hRelaObj;
		if((pNode=Ta.Node.FromHandle(dwhObj))!=NULL)
			break;
		pPart=Ta.Parts.FromHandle(dwhObj);
		if(pPart && (pPart->GetClassTypeId()==CLS_LINEANGLE||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE))
			break;
	}
	g_pSolidDraw->SetEntSnapStatus(dwhObj);
	if(pNode)
	{
		pCmdLine->FinishCmdLine(CXhChar16("�ڵ�0x%X",pNode->handle));
		pCmdLine->CancelCmdLine();
		if(pObj1->GetClassTypeId()==CLS_NODE)
		{
			pObj2=pNode;
			FinishMeasure(pObj1,pObj2);
		}
		else if(pObj1->GetClassTypeId()==CLS_LINEANGLE || pObj1->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			pObj3=pNode;
			FinishMeasure(pObj1,pObj2,pObj3);
		}
	}
	else if(pPart)
	{
		pCmdLine->FinishCmdLine(CXhChar16("�Ǹ�0x%X",pPart->handle));
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("DIST Select Key Node On Line Angle");
#else
		cmdStr.Format("DIST ��ѡ��ǰ�Ǹ��Ϲؼ��ڵ�:");
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		verifier.ClearSnapFlag();
		verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
		while(true)
		{
			if((state=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			{
				pCmdLine->CancelCmdLine();
				return -1;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;
			if((pNode=Ta.Node.FromHandle(dwhObj))!=NULL)
				break;
		}
		g_pSolidDraw->SetEntSnapStatus(dwhObj);
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pNode->handle));
		pCmdLine->CancelCmdLine();
		if(pObj1->GetClassTypeId()==CLS_NODE)
		{
			pObj2=pPart;
			pObj3=pNode;
			FinishMeasure(pObj1,pObj2,pObj3);
		}
		else if(pObj1->GetClassTypeId()==CLS_LINEANGLE || pObj1->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			pObj3=pPart;
			pObj4=pNode;
			FinishMeasure(pObj1,pObj2,pObj3,pObj4);
		}
	}
	//
	g_pSolidDraw->ReleaseSnapStatus();
	DistMeasureOnDispLine();
return 0;
}
#include "KeypointLifeObj.h"
double MeasureMinDistanceOfPointToAngle(const double* point,CLDSLineAngle* pAngle,double* vPoint2Angle,double filterMinDist=10000,BYTE cbValidWingFlag=0x03)
{
	f3dPoint perp;
	GEPOINT posStart(point),posEnd,lenStdVec;
	lenStdVec=pAngle->End()-pAngle->Start();
	double rodLength=lenStdVec.mod();
	lenStdVec/=rodLength;
	rodLength+=pAngle->startOdd()+pAngle->endOdd();
	if(SnapPerp(&perp,pAngle->Start(),pAngle->End(),posStart))
		posEnd=perp;
	GEPOINT vec=perp-pAngle->Start();
	double dist=vec*lenStdVec;
	if(dist<-pAngle->startOdd())
		posEnd=pAngle->Start()-f3dPoint(pAngle->startOdd()*lenStdVec);
	else if(dist+pAngle->startOdd()>rodLength)
		posEnd=pAngle->End()+f3dPoint(pAngle->endOdd()*lenStdVec);;
	vec=posEnd-posStart;
	dist=vec.mod();
	if(filterMinDist<dist-pAngle->size_wide)
		return filterMinDist;	//�������б�filterMinDist���̵ľ���
	GEPOINT vWingVecX=pAngle->GetWingVecX();
	GEPOINT vWingVecY=pAngle->GetWingVecY();
	GEPOINT vec2x,vec2y,minvec=vec;
	double justify_vec_n_x=-(vec*vWingVecX);
	double justify_vec_n_y=-(vec*vWingVecY);
	if(justify_vec_n_x>0)	//��X֫������������
	{
		vec2x=vec+vWingVecX*pAngle->size_wide;
		if(pAngle->vxWingNorm*vec>0)
		{
			GEPOINT v=vec2x-pAngle->vxWingNorm*pAngle->size_thick;
			if(v.mod2()<vec2x.mod2())
				vec2x=v;
		}
		if(minvec.mod2()>vec2x.mod2())
			minvec=vec2x;
		if(justify_vec_n_x>0&&justify_vec_n_x<pAngle->size_wide&&(cbValidWingFlag&0x01))
		{	//Ӧ���㵽X֫��ͶӰ�����ľ���
			GEPOINT vec2xi=vec+justify_vec_n_x*vWingVecX;
			if(justify_vec_n_y>0)	//X֫�ڲ����Ƥ�����������
				vec2xi-=pAngle->size_thick*pAngle->vxWingNorm;
			if(minvec.mod2()>vec2xi.mod2())
				minvec=vec2xi;
		}
	}
	if(justify_vec_n_y>0)	//��X֫������������
	{
		vec2y=vec+vWingVecY*pAngle->size_wide;
		if(pAngle->vyWingNorm*vec>0)
		{
			GEPOINT v=vec2y-pAngle->vyWingNorm*pAngle->size_thick;
			if(v.mod2()<vec2y.mod2())
				vec2y=v;
		}
		if(minvec.mod2()>vec2y.mod2())
			minvec=vec2y;
		if(justify_vec_n_y>0&&justify_vec_n_y<pAngle->size_wide&&(cbValidWingFlag&0x02))
		{	//Ӧ���㵽X֫��ͶӰ�����ľ���
			GEPOINT vec2yi=vec+justify_vec_n_y*vWingVecY;
			if(justify_vec_n_x>0)	//X֫�ڲ����Ƥ�����������
				vec2yi-=pAngle->size_thick*pAngle->vyWingNorm;
			if(minvec.mod2()>vec2yi.mod2())
				minvec=vec2yi;
		}
	}
	posEnd=posStart+minvec;
	dist=minvec.mod();
	if(filterMinDist>dist)
	{
		filterMinDist=dist;
		if(vPoint2Angle)
			memcpy(vPoint2Angle,(double*)minvec,24);
	}
	return filterMinDist;	//�������б�filterMinDist���̵ľ���
}
double MeasureMinDistanceBetweenParts(CLDSPart* pPart1,CLDSPart* pPart2,double* xMinDistPosStart,double* xMinDistPosEnd)
{
	SmartPartPtr pDatumPart=pPart1,pPart=pPart2;
	f3dPoint perp;
	f3dLine dist_line;
	double dist_min=1000000,dist=0;
	if(pDatumPart->GetClassTypeId()==CLS_LINETUBE||pPart->GetClassTypeId()==CLS_LINETUBE)
	{	//����һ������Ϊ�ֹ�
		if(pPart->GetClassTypeId()==CLS_LINETUBE&&pDatumPart->GetClassTypeId()!=CLS_LINETUBE)
		{	//��֤��׼����Ϊ�ֹ�
			CLDSPart* pTempPart=pDatumPart;
			pDatumPart=pPart;
			pPart=pTempPart;
		}
		f3dPoint start=pDatumPart.LineTubePointer()->Start();
		f3dPoint end=pDatumPart.LineTubePointer()->End();
		if(pPart->GetClassTypeId()==CLS_BOLT)
		{
			SnapPerp(&perp,start,end,pPart->ucs.origin,&dist_min);
			dist_line.startPt=pPart->ucs.origin;
			dist_line.endPt=perp;
		}
		else 
		{
			int vertex_n=pPart->pSolidBody->KeyPointNum();
			//for(f3dPoint *pPt=pPart->pBody->vertex.GetFirst();pPt;pPt=pPart->pBody->vertex.GetNext())
			for(int vi=0;vi<vertex_n;vi++)
			{
				f3dPoint vertex=pPart->pSolidBody->GetKeyPointAt(vi);
				SnapPerp(&perp,start,end,vertex,&dist);
				if(dist<dist_min)
				{
					dist_min=dist;
					dist_line.startPt=vertex;
					dist_line.endPt=perp;
				}
			}
		}
		double fTubeR=pDatumPart.LineTubePointer()->GetDiameter()*0.5;
		f3dPoint line_vec=(dist_line.startPt-dist_line.endPt).normalized();
		dist_line.endPt+=line_vec*fTubeR;
		dist_min-=fTubeR;
		if(xMinDistPosStart)
			memcpy(xMinDistPosStart,(double*)dist_line.startPt,24);
		if(xMinDistPosEnd)
			memcpy(xMinDistPosEnd,(double*)dist_line.endPt,24);
		return dist_min;
	}
	else if(pDatumPart->GetClassTypeId()==CLS_LINEANGLE||pPart->GetClassTypeId()==CLS_LINEANGLE)
	{	//����һ������Ϊ�Ǹ� ������һ��������Ϊ�ֹܺ���˨
		if(pPart->GetClassTypeId()==CLS_LINEANGLE&&pDatumPart->GetClassTypeId()!=CLS_LINEANGLE)
		{	//��֤��׼����Ϊ�Ǹ�
			CLDSPart* pTempPart=pDatumPart;
			pDatumPart=pPart;
			pPart=pTempPart;
		}
		double wing_wide=pDatumPart.LineAnglePointer()->GetWidth();
		f3dPoint start=pDatumPart.LineAnglePointer()->Start();
		f3dPoint end=pDatumPart.LineAnglePointer()->End();
		f3dPoint len_vec;
		Sub_Pnt(len_vec,end,start);
		normalize(len_vec);
		Sub_Pnt(start,start,len_vec*pDatumPart.LineAnglePointer()->startOdd());
		Add_Pnt(end,end,len_vec*pDatumPart.LineAnglePointer()->endOdd());
		CHashList<int>hashKeyPoints;
		int edge_n=pPart->pSolidBody->KeyEdgeLineNum();
		for(int ei=0;ei<edge_n;ei++)
		{
			CRawSolidEdge edge;
			if(pPart->pSolidBody->GetKeyEdgeLineAt(ei,edge))
			{
				hashKeyPoints.SetValue(edge.LineStartId,edge.LineStartId);
				hashKeyPoints.SetValue(edge.LineEndId,edge.LineEndId);
			}
		}
		//int vertex_n=pPart->pSolidBody->KeyEdgeLineNum();
		//for(int vi=0;vi<vertex_n;vi++)
		for(int* pVertexId=hashKeyPoints.GetFirst();pVertexId;pVertexId=hashKeyPoints.GetNext())
		{
			if(*pVertexId<=0)
				continue;
			f3dPoint vertex=pPart->pSolidBody->GetKeyPointAt(*pVertexId-1);//vi);
			if(pDatumPart->IsAngle())
			{	//��׼�˼�Ϊ�Ǹ�ʱ��������������
				BYTE cbValidWingFlag=0x03;
				if(pPart->IsAngle())
				{
					if(pDatumPart.IsEqualPtr(pPart.pAngle->GetStartDatumPart()))
					{
						if(pPart.pAngle->desStartPos.IsInDatumJgWingX())
							cbValidWingFlag-=0x01;
						else if(pPart.pAngle->desStartPos.IsInDatumJgWingY())
							cbValidWingFlag-=0x02;
					}
					else if(pDatumPart.IsEqualPtr(pPart.pAngle->GetEndDatumPart()))
					{
						if(pPart.pAngle->desEndPos.IsInDatumJgWingX())
							cbValidWingFlag-=0x01;
						else if(pPart.pAngle->desEndPos.IsInDatumJgWingY())
							cbValidWingFlag-=0x02;
					}
				}
				GEPOINT vec2Angle;
				double distance=dist_min;
				if((distance=MeasureMinDistanceOfPointToAngle(vertex,pDatumPart.pAngle,vec2Angle,dist_min,cbValidWingFlag))<dist_min)
				{
					dist_min=distance;
					dist_line.startPt=vertex;
					dist_line.endPt=vertex+vec2Angle;
				}
				continue;
			}
			f3dLine datum_line;
			for(int i=0;i<7;i++)
			{
				if(i==0)
				{
					datum_line.startPt=start;
					datum_line.endPt=end;
				}
				else if(i==1||i==2)
				{
					f3dPoint wing_vec=pDatumPart.LineAnglePointer()->GetWingVecX();
					if(i==2)
						wing_vec=pDatumPart.LineAnglePointer()->GetWingVecY();
					datum_line.startPt=start+wing_vec*wing_wide;
					datum_line.endPt=end+wing_vec*wing_wide;
				}
				else if(i==3||i==4)
				{
					f3dPoint wing_vec=pDatumPart.LineAnglePointer()->GetWingVecX();	
					if(i==4)
						wing_vec=pDatumPart.LineAnglePointer()->GetWingVecY();
					datum_line.startPt=start;
					datum_line.endPt=start+wing_vec*wing_wide;
				}
				else if(i==5||i==6)
				{
					f3dPoint wing_vec=pDatumPart.LineAnglePointer()->GetWingVecX();	
					if(i==6)
						wing_vec=pDatumPart.LineAnglePointer()->GetWingVecY();
					datum_line.startPt=end;
					datum_line.endPt=end+wing_vec*wing_wide;
				}
				if(pPart->GetClassTypeId()==CLS_BOLT)
					SnapPerp(&perp,datum_line,pPart->ucs.origin,&dist);
				else
					SnapPerp(&perp,datum_line,vertex,&dist);
				int nRetCode=datum_line.PtInLine(perp);
				if(dist<dist_min&&nRetCode>0)
				{
					dist_min=dist;
					dist_line.startPt=vertex;
					dist_line.endPt=perp;
				}
			}
			if(pPart->GetClassTypeId()==CLS_BOLT)
				break;	//��˨
		}
		//�ڲ��������Ǹ�֮�����̾���ʱ�û�����ѡ���˻�׼���� ������Ҫ�������¼���
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			double wing_wide=pPart.LineAnglePointer()->GetWidth();
			f3dPoint start=pPart.LineAnglePointer()->Start();
			f3dPoint end=pPart.LineAnglePointer()->End();
			Sub_Pnt(len_vec,end,start);
			normalize(len_vec);
			Sub_Pnt(start,start,len_vec*pPart.LineAnglePointer()->startOdd());
			Add_Pnt(end,end,len_vec*pPart.LineAnglePointer()->endOdd());
			hashKeyPoints.Empty();
			int edge_n=pDatumPart->pSolidBody->KeyEdgeLineNum();
			for(int ei=0;ei<edge_n;ei++)
			{
				CRawSolidEdge edge;
				if(pDatumPart->pSolidBody->GetKeyEdgeLineAt(ei,edge))
				{
					hashKeyPoints.SetValue(edge.LineStartId,edge.LineStartId);
					hashKeyPoints.SetValue(edge.LineEndId,edge.LineEndId);
				}
			}
			//int vertex_n=pDatumPart->pSolidBody->KeyEdgeLineNum();
			//for(int vi=0;vi<vertex_n;vi++)
			for(int* pVertexId=hashKeyPoints.GetFirst();pVertexId;pVertexId=hashKeyPoints.GetNext())
			{
				if(*pVertexId<=0)
					continue;
				f3dPoint vertex=pDatumPart->pSolidBody->GetKeyPointAt(*pVertexId-1);//vi);
				if(pPart->IsAngle())
				{	//��׼�˼�Ϊ�Ǹ�ʱ��������������
					BYTE cbValidWingFlag=0x03;
					if(pDatumPart->IsAngle())
					{
						if(pPart.IsEqualPtr(pDatumPart.pAngle->GetStartDatumPart()))
						{
							if(pDatumPart.pAngle->desStartPos.IsInDatumJgWingX())
								cbValidWingFlag-=0x01;
							else if(pDatumPart.pAngle->desStartPos.IsInDatumJgWingY())
								cbValidWingFlag-=0x02;
						}
						else if(pPart.IsEqualPtr(pDatumPart.pAngle->GetEndDatumPart()))
						{
							if(pDatumPart.pAngle->desEndPos.IsInDatumJgWingX())
								cbValidWingFlag-=0x01;
							else if(pDatumPart.pAngle->desEndPos.IsInDatumJgWingY())
								cbValidWingFlag-=0x02;
						}
					}
					GEPOINT vec2Angle;
					double distance=dist_min;
					if((distance=MeasureMinDistanceOfPointToAngle(vertex,pPart.pAngle,vec2Angle,dist_min,cbValidWingFlag))<dist_min)
					{
						dist_min=distance;
						dist_line.startPt=vertex;
						dist_line.endPt=vertex+vec2Angle;
					}
					continue;
				}
				f3dLine datum_line;
				for(int i=0;i<7;i++)
				{
					if(i==0)
					{	//����
						datum_line.startPt=start;
						datum_line.endPt=end;
					}
					else if(i==1||i==2)
					{	//
						f3dPoint wing_vec=pPart.LineAnglePointer()->GetWingVecX();	
						if(i==2)
							wing_vec=pPart.LineAnglePointer()->GetWingVecY();
						datum_line.startPt=start+wing_vec*wing_wide;
						datum_line.endPt=end+wing_vec*wing_wide;
					}
					else if(i==3||i==4)
					{
						f3dPoint wing_vec=pPart.LineAnglePointer()->GetWingVecX();	
						if(i==4)
							wing_vec=pPart.LineAnglePointer()->GetWingVecY();
						datum_line.startPt=start;
						datum_line.endPt=start+wing_vec*wing_wide;
					}
					else if(i==5||i==6)
					{
						f3dPoint wing_vec=pPart.LineAnglePointer()->GetWingVecX();	
						if(i==6)
							wing_vec=pPart.LineAnglePointer()->GetWingVecY();
						datum_line.startPt=end;
						datum_line.endPt=end+wing_vec*wing_wide;
					}
					SnapPerp(&perp,datum_line,vertex,&dist);
					int nRetCode=datum_line.PtInLine(perp);
					if(dist<dist_min&&nRetCode>0)
					{
						dist_min=dist;
						dist_line.startPt=vertex;
						dist_line.endPt=perp;
					}
				}
			}
		}
		if(xMinDistPosStart)
			memcpy(xMinDistPosStart,(double*)dist_line.startPt,24);
		if(xMinDistPosEnd)
			memcpy(xMinDistPosEnd,(double*)dist_line.endPt,24);
		return dist_min;
	}
	return 1000000;
}
double MeasureMinDistanceBetweenAngles(CLDSLineAngle* pAngle1,CLDSLineAngle* pAngle2,double* xMinDistPosStart,double* xMinDistPosEnd)
{
	double dist,mindist=pAngle1->GetLength();
	GEPOINT vertexArr[10],vertexEndArr[5];
	GEPOINT vWingNormX=pAngle1->vxWingNorm,vWingVecX=pAngle1->GetWingVecX();
	GEPOINT vWingNormY=pAngle1->vyWingNorm,vWingVecY=pAngle1->GetWingVecY();
	f3dPoint lenStdVec=pAngle1->End()-pAngle1->Start();
	normalize(lenStdVec);
	int i,start0end1;
	//����pAngle1���㵽pAngle2����̾���
	for(start0end1=0;start0end1<2;start0end1++)
	{
		GEPOINT xTipPos=pAngle1->Start()-lenStdVec*pAngle1->startOdd();
		if(start0end1==1)
			xTipPos=pAngle1->End()+lenStdVec*pAngle1->endOdd();
		int step=+start0end1*5;
		vertexArr[0+step] =xTipPos;
		vertexArr[1+step] =xTipPos+vWingVecX*pAngle1->size_wide;
		vertexArr[2+step] =vertexArr[1]-vWingNormX*pAngle1->size_thick;
		vertexArr[3+step] =xTipPos+vWingVecY*pAngle1->size_wide;
		vertexArr[4+step] =vertexArr[3]-vWingNormY*pAngle1->size_thick;
	}
	GEPOINT xMinDistVertex,vMinDistVec,vDistVec;
	for(i=0;i<10;i++)
	{
		BYTE cbValidWingFlag=0x03;
		if(i<5&&pAngle1->GetStartDatumPart()==pAngle2)
		{
			if(pAngle1->desStartPos.IsInDatumJgWingX())
				cbValidWingFlag-=0x01;
			else if(pAngle1->desStartPos.IsInDatumJgWingY())
				cbValidWingFlag-=0x02;
		}
		if(i>=5&&pAngle1->GetEndDatumPart()==pAngle2)
		{
			if(pAngle1->desEndPos.IsInDatumJgWingX())
				cbValidWingFlag-=0x01;
			else if(pAngle1->desEndPos.IsInDatumJgWingY())
				cbValidWingFlag-=0x02;
		}
		if((dist=MeasureMinDistanceOfPointToAngle(vertexArr[i],pAngle2,vDistVec,mindist,cbValidWingFlag))<mindist)
		{
			xMinDistVertex=vertexArr[i];
			mindist=dist;
			vMinDistVec=vDistVec;
		}
	}
	//����pAngle2���㵽pAngle1����̾���
	vWingNormX=pAngle2->vxWingNorm;
	vWingVecX =pAngle2->GetWingVecX();
	vWingNormY=pAngle2->vyWingNorm;
	vWingVecY =pAngle2->GetWingVecY();
	lenStdVec=pAngle2->End()-pAngle2->Start();
	normalize(lenStdVec);
	for(start0end1=0;start0end1<2;start0end1++)
	{
		GEPOINT xTipPos=start0end1==0?pAngle2->Start()-lenStdVec*pAngle2->startOdd():pAngle2->End()+lenStdVec*pAngle2->endOdd();
		int step=+start0end1*5;
		vertexArr[0+step] =xTipPos;
		vertexArr[1+step] =xTipPos+vWingVecX*pAngle2->size_wide;
		vertexArr[2+step] =vertexArr[1]-vWingNormX*pAngle2->size_thick;
		vertexArr[3+step] =xTipPos+vWingVecY*pAngle2->size_wide;
		vertexArr[4+step] =vertexArr[3]-vWingNormY*pAngle2->size_thick;
	}
	for(i=0;i<10;i++)
	{
		BYTE cbValidWingFlag=0x03;
		if(i<5&&pAngle2->GetStartDatumPart()==pAngle1)
		{
			if(pAngle2->desStartPos.IsInDatumJgWingX())
				cbValidWingFlag-=0x01;
			else if(pAngle2->desStartPos.IsInDatumJgWingY())
				cbValidWingFlag-=0x02;
		}
		if(i>=5&&pAngle2->GetEndDatumPart()==pAngle1)
		{
			if(pAngle2->desEndPos.IsInDatumJgWingX())
				cbValidWingFlag-=0x01;
			else if(pAngle2->desEndPos.IsInDatumJgWingY())
				cbValidWingFlag-=0x02;
		}
		if((dist=MeasureMinDistanceOfPointToAngle(vertexArr[i],pAngle1,vDistVec,mindist,cbValidWingFlag))<mindist)
		{
			xMinDistVertex=vertexArr[i];
			mindist=dist;
			vMinDistVec=vDistVec;
		}
	}
	if(xMinDistPosStart)
		memcpy(xMinDistPosStart,(double*)xMinDistVertex,24);
	if(xMinDistPosEnd)
	{
		xMinDistPosEnd[0]=xMinDistVertex.x+vMinDistVec.x;
		xMinDistPosEnd[1]=xMinDistVertex.y+vMinDistVec.y;
		xMinDistPosEnd[2]=xMinDistVertex.z+vMinDistVec.z;
	}
	return mindist;
}
//����ֵΪ�Ǹ��ڽڵ㴦��:0.��ֱͶӰ����;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��
int IntelliRecogWorkPlane(CLDSLineAngle* pAngle,CLDSNode* pNode,char ciWorkWingX0Y1)
{
	if(pNode->hFatherPart!=pAngle->handle)
		return 0;	//���ڽڵ㸸�˼����ǵ�ǰ�Ǹ�ʱ�ſ���ƽ��
	if(pNode->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED)
		return 2;	//2.Y-Zƽ��ƽ��
	else if(pNode->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED)
		return 3;	//3.X-Zƽ��ƽ��
	else if(pNode->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)
		return 1;	//1.X-Yƽ��ƽ��
	else if(pNode->m_cPosCalType==CLDSNode::COORD_OFFSET&&pNode->m_cPosCalViceType>='X')
	{
		if(pNode->m_cPosCalViceType=='X')
			return 2;	//2.Y-Zƽ��ƽ��
		else if(pNode->m_cPosCalViceType=='Y')
			return 3;	//3.X-Zƽ��ƽ��
		else if(pNode->m_cPosCalViceType=='Z')
			return 1;	//1.X-Yƽ��ƽ��
		else
			return 0;
	}
	else if((pAngle->m_cPosCtrlDatumLineType-'X')==ciWorkWingX0Y1)
		return pAngle->m_cPosCtrlType;
	else if(pAngle->pStart==pNode||pAngle->pEnd==pNode)
	{
		CMultiOffsetPos* pPosPara=pAngle->pStart==pNode ? &pAngle->desStartPos : &pAngle->desEndPos;
		if(pPosPara->IsFaceOffset())
		{
			if(fabs(pPosPara->face_offset_norm.x)+fabs(pPosPara->face_offset_norm.y)<EPS&&fabs(pPosPara->face_offset_norm.z)>EPS2)
				return 1;	//1.X-Yƽ��ƽ��
			if(fabs(pPosPara->face_offset_norm.y)+fabs(pPosPara->face_offset_norm.z)<EPS&&fabs(pPosPara->face_offset_norm.x)>EPS2)
				return 2;	//2.Y-Zƽ��ƽ��
			if(fabs(pPosPara->face_offset_norm.x)+fabs(pPosPara->face_offset_norm.z)<EPS&&fabs(pPosPara->face_offset_norm.y)>EPS2)
				return 3;	//3.X-Zƽ��ƽ��
		}
		else if(pPosPara->spatialOperationStyle==15)
		{
			GEPOINT lenStdVec=pAngle->End()-pAngle->Start();
			normalize(lenStdVec);
			//if((pPosPara->cMainCoordCtrlWing-'X')==ciWorkWingX0Y1)
			{	//��ǰ֫Ϊ����֫,0:X-Z����;1:Y-Z����;2.X-Y����(����spatialOperationStyle=15ʱ��Ч)
				if(pPosPara->coordCtrlType==0)		//0:X-Z����,ѡY-Zƽ�����X-Yƽ����
					return fabs(lenStdVec.z)<fabs(lenStdVec.x)?2:1;
				else if(pPosPara->coordCtrlType==1)	//1:Y-Z����,ѡX-Zƽ�����X-Yƽ����
					return fabs(lenStdVec.z)<fabs(lenStdVec.y)?3:1;
				else if(pPosPara->coordCtrlType==2)	//2:X-Y����,ѡX-Zƽ�����Y-Zƽ����
					return fabs(lenStdVec.x)<fabs(lenStdVec.y)?3:2;
			}
		}
		return 0;
	}
	else
		return 0;
}
static int _LocalDistMeasure(CLDSView* pView)
{
	//TODO:��ʱ����
	bool bCheckDimensionApppend=false;
	struct ANGLENODE_COUPLE{
		CLDSLineAngle* pAngle;
		CLDSNode* pNode;
		ANGLENODE_COUPLE(){pAngle=NULL;pNode=NULL;}
		bool IsValid(){return (pAngle!=NULL&&pNode!=NULL);}
	};
	{	//��Ӵ˻�����Ϊ���ƾֲ�����������������
	bool needRsltDisplay=true;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select angle that the measured key points belong to or the corresponding node successively","");
#else
	pCmdLine->FillCmdLine("DIST ��ѡ���������ʼ�ڵ㡢�����㣬�����������ڵĹ���:","");
#endif
	int state;
	GEPOINT posStart,posEnd;
	DWORD dwhObj,dwExportFlag;
	IDrawing* pDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_POINT);
	verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
	verifier.AddVerifyType(OBJPROVIDER::DRAWINGSPACE,IDbEntity::DbPoint);
	verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART|GetSingleWord(SELECTINDEX_BOLT)|
		GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE)|GetSingleWord(SELECTINDEX_HOLEWALL));
	CLDSNode* pStartNode=NULL,*pEndNode=NULL;
	IDbEntity* pStartPoint=NULL,*pEndPoint=NULL;
	SmartPartPtr pStartPart,pEndPart,pStartBoltResidePart,pEndBoltResidePart;
	CLDSBolt* pStartBolt=NULL,*pEndBolt=NULL;
	CPlateDisplayLifeObject lifePlates[2];
	CAngleDisplayLifeObject lifeAngles[2];
	CLDSPoint startPoint,endPoint;
	ANGLENODE_COUPLE startAngleNode,endAngleNode;	//'�Ǹ�-�ڵ�' vs '�Ǹ�-�ڵ�'ģʽ�������
	while(true)
	{
		if((state=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
		{
			pCmdLine->CancelCmdLine();
			return -1;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		dwhObj=obj.hRelaObj;
		if (obj.provider == OBJPROVIDER::LINESPACE)
		{
			if ((pStartNode = Ta.Node.FromHandle(dwhObj)) != NULL)
				break;
			if ((pStartPart = Ta.Parts.FromHandle(dwhObj)) != NULL)
				break;
		}
		else if(obj.provider==OBJPROVIDER::DRAWINGSPACE)
		{
			IDbEntity* pEnt=obj.idEnt>0?pDrawing->GetDbEntity(obj.idEnt):NULL;
			if(pEnt&&pEnt->GetDbEntType()==IDbEntity::DbPoint)//||pEnt->GetDbEntType()==IDbEntity::DbText)
			{
				posStart=((IDbPoint*)pEnt)->Position();
				posStart=g_pSolidOper->TransPFromGL(posStart);
				pStartPoint=pEnt;
				break;
			}
		}
		else if (obj.provider == OBJPROVIDER::SOLIDSPACE)
		{
			if (obj.hObj < 0 && obj.iObjType == CLS_HOLEWALL)
			{
				obj.TranslateHoleWall(g_pSolidDraw);
				pStartPart = Ta.Parts.FromHandle(obj.hObj, CLS_BOLT);
				pStartBoltResidePart=Ta.FromPartHandle(obj.hResidePart);
			}
			else
				pStartPart = Ta.Parts.FromHandle(dwhObj);
			if (pStartPart.IsHasPtr())
				break;
		}
	}
	g_pSolidDraw->SetEntSnapStatus(dwhObj);
	if(pStartNode)
	{
		pCmdLine->FinishCmdLine(CXhChar16("��ʼ�ڵ�0x%X",pStartNode->handle));
		posStart=pStartNode->Position();
	}
	else if(pStartPoint)
	{
		HIBERID hiberid=pStartPoint->GetHiberId();
		SmartPartPtr pMasterPart=Ta.Parts.FromHandle(hiberid.masterId);
		if( pMasterPart.IsHasPtr()&&pMasterPart->IsAngle()&&
			RetrieveParaPointByAngleHiberarchy(pMasterPart.pAngle,pStartPoint->GetHiberId(),&startPoint)&&
			startPoint.UpdatePos(&Ta))
			posStart=startPoint.Position();
		pCmdLine->FinishCmdLine(CXhChar16("������%d",pStartPoint->GetId()));
	}
	else if(pStartPart.IsNULL())
		logerr.Log("encounter an unexpected error");
	else if(pStartPart->GetClassTypeId()==CLS_BOLT)
	{
		pStartBolt=pStartPart.pBolt;
		posStart=pStartBolt->ucs.origin;
		pCmdLine->FinishCmdLine(CXhChar16("��ʼ��˨0x%X",pStartBolt->handle));
	}
	else
	{
		posStart=pStartPart->ucs.origin;
		if(pStartPart->IsAngle())
		{
			pCmdLine->FillCmdLine("DIST ָ����ǰ�Ǹֽ���ڵ㡢ʼ���������ѡ����ֹ�����ڵ���һ�����������������:","");
			lifeAngles[0].AttachDisplayState(pStartPart,0);
			CLDSView::RedrawAngleFrameDrawing(pStartPart.pAngle,0x03);
			CLDSNode* pSectionNode=NULL;
			verifier.ClearSnapFlag();
			//verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE));
			verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
			verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,SELECT_PART|GetSingleWord(SELECTINDEX_NODE));
			verifier.AddVerifyType(OBJPROVIDER::DRAWINGSPACE,IDbEntity::DbPoint);
			startAngleNode.pAngle=pStartPart.pAngle;
			if(g_sysPara.m_bLineDepthTest)
			{
				g_pSolidSet->DisableLineModelDepthTest();
				g_pSolidDraw->Draw();
			}
			while(true)
			{
				if((state=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				{
					if(g_sysPara.m_bLineDepthTest)
						g_pSolidSet->EnableLineModelDepthTest();
					pCmdLine->CancelCmdLine();
					return -1;
				}
				SELOBJ obj(dwhObj,dwExportFlag,pDrawing);
				dwhObj=obj.hRelaObj;
				pSectionNode=dwhObj>0?Ta.Node.FromHandle(dwhObj):NULL;
				if(pSectionNode!=NULL)
					break;
				IDbEntity* pEnt=(obj.provider==OBJPROVIDER::DRAWINGSPACE&&obj.idEnt>0)?pDrawing->GetDbEntity(obj.idEnt):NULL;
				if(pEnt&&pEnt->GetDbEntType()==IDbEntity::DbPoint)//||pEnt->GetDbEntType()==IDbEntity::DbText)
				{
					posStart=((IDbPoint*)pEnt)->Position();
					posStart=g_pSolidOper->TransPFromGL(posStart);
					HIBERID hiberid=pEnt->GetHiberId();
					if( pStartPart->IsAngle()&&
						RetrieveParaPointByAngleHiberarchy(pStartPart.pAngle,pEnt->GetHiberId(),&startPoint)&&
						startPoint.UpdatePos(&Ta))
						posStart=startPoint.Position();
					pStartPoint=pEnt;
					break;
				}
				else if(obj.provider==OBJPROVIDER::LINESPACE||obj.provider==OBJPROVIDER::SOLIDSPACE)
				{
					pEndPart=Ta.FromRodHandle(dwhObj);
					break;
				}
			}
			if(g_sysPara.m_bLineDepthTest)
				g_pSolidSet->EnableLineModelDepthTest();
			if(pSectionNode)
			{
				pCmdLine->FinishCmdLine(CXhChar16("�ڵ�(0x%X)����",pSectionNode->handle));
				g_pSolidDraw->SetEntSnapStatus(pStartPart->handle,false);
				int iPlaneType=IntelliRecogWorkPlane(pStartPart.pAngle,pSectionNode,0);
				//1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��
				char ciPlaneType=iPlaneType;
				if(iPlaneType==1)
					ciPlaneType='Z';
				else if(iPlaneType==2)
					ciPlaneType='X';
				else if(iPlaneType==3)
					ciPlaneType='Y';
				AppendAngleSectionFrameDrawing(pStartPart.pAngle,pSectionNode,0,0,ciPlaneType);
				startAngleNode.pNode=pSectionNode;
			}
		}
		else if(pStartPart->IsPlate())
			lifePlates[0].AttachDisplayState(pStartPart.GeneralPlatePointer(),2);
			//RedrawPlateProfileDrawing(pStartPart.GeneralPlatePointer());
		g_pSolidDraw->Draw();
		if((pStartPart->IsPlate()||pStartPoint==NULL)&&pEndPart.IsNULL())
		{
			verifier.ClearSnapFlag();
			verifier.AddVerifyType(OBJPROVIDER::DRAWINGSPACE,IDbEntity::DbPoint);
			if(pStartPart->IsAngle())
			{
				pCmdLine->FillCmdLine("DIST ָ����ǰ�����ϵ���������Ϊ��ʼ���ѡ����ֹ�����ڵ���һ���˼�:","");
				verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prLine);
				verifier.AddVerifyType(OBJPROVIDER::SOLIDSPACE,CLS_LINEANGLE);
			}
			else
			{
				verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,SELECT_PART);
				pCmdLine->FillCmdLine("DIST ָ����ǰ�����ϵ���������Ϊ��ʼ��, ��ѡ����һ�������������:","");
			}
			while(true)
			{
				if((state=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				{
					pCmdLine->CancelCmdLine();
					return -1;
				}
				SELOBJ obj(dwhObj,dwExportFlag,pDrawing);
				dwhObj=obj.hRelaObj;
				IDbEntity* pEnt=(obj.provider==OBJPROVIDER::DRAWINGSPACE&&obj.idEnt>0)?pDrawing->GetDbEntity(obj.idEnt):NULL;
				if(pEnt&&pEnt->GetDbEntType()==IDbEntity::DbPoint)//||pEnt->GetDbEntType()==IDbEntity::DbText)
				{
					posStart=((IDbPoint*)pEnt)->Position();
					posStart=g_pSolidOper->TransPFromGL(posStart);
					HIBERID hiberid=pEnt->GetHiberId();
					if( pStartPart.IsHasPtr()&&pStartPart->IsAngle()&&
						pStartPart->IsAngle()&&
						RetrieveParaPointByAngleHiberarchy(pStartPart.pAngle,pEnt->GetHiberId(),&startPoint)&&
						startPoint.UpdatePos(&Ta))
						posStart=startPoint.Position();
					pStartPoint=pEnt;
					startAngleNode.pNode=NULL;	//ѡ�е��ǵ�һ���Ǹ��ϵĹؼ��㣬������'�˼�-�ڵ�'ģʽ
					break;
				}
				else if(obj.provider==OBJPROVIDER::LINESPACE||obj.provider==OBJPROVIDER::SOLIDSPACE)
				{
					pEndPart=Ta.FromRodHandle(dwhObj);
					break;
				}
			}
			pCmdLine->FinishCmdLine();
		}
	}

	if(pEndPart.IsNULL())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select angle that the measured feature points belong to or the corresponding node successively","");
#else
		pCmdLine->FillCmdLine("DIST ��ѡ���������ֹ�ڵ㡢�����㣬�����������ڵĹ���:","");
#endif
		verifier.ClearSnapFlag();
		verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
		verifier.AddVerifyType(OBJPROVIDER::DRAWINGSPACE,IDbEntity::DbPoint);
		verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART|GetSingleWord(SELECTINDEX_BOLT)|
			GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE)|GetSingleWord(SELECTINDEX_HOLEWALL));
		while(true)
		{
			if((state=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			{
				pCmdLine->CancelCmdLine();
				return -1;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;
			if (obj.provider == OBJPROVIDER::LINESPACE)
			{
				if((pEndNode = Ta.Node.FromHandle(dwhObj)) != NULL)
					break;
				if ((pEndPart = Ta.Parts.FromHandle(dwhObj)) != NULL)
					break;
			}
			else if(obj.provider==OBJPROVIDER::DRAWINGSPACE)
			{
				IDbEntity* pEnt=obj.idEnt>0?pDrawing->GetDbEntity(obj.idEnt):NULL;
				if(pEnt&&pEnt->GetDbEntType()==IDbEntity::DbPoint)//||pEnt->GetDbEntType()==IDbEntity::DbText)
				{
					posEnd=((IDbPoint*)pEnt)->Position();
					posEnd=g_pSolidOper->TransPFromGL(posEnd);
					pEndPoint=pEnt;
					break;
				}
			}
			else if (obj.provider == OBJPROVIDER::SOLIDSPACE)
			{
				if (obj.hObj < 0 && obj.iObjType == CLS_HOLEWALL)
				{
					obj.TranslateHoleWall(g_pSolidDraw);
					pEndPart = Ta.Parts.FromHandle(obj.hObj, CLS_BOLT);
					pEndBoltResidePart=Ta.FromPartHandle(obj.hResidePart);
				}
				else
					pEndPart = Ta.Parts.FromHandle(dwhObj);
				if (pEndPart.IsHasPtr())
				{
					if (startAngleNode.IsValid() && pEndPart->IsAngle())
						endAngleNode.pAngle = pEndPart.pAngle;
					break;
				}
			}
		}
		g_pSolidDraw->SetEntSnapStatus(dwhObj);
		if(pEndNode)
		{
			posEnd=pEndNode->Position();
			pCmdLine->FinishCmdLine(CXhChar16("��ֹ�ڵ�0x%X",pEndNode->handle));
		}
		else if(pEndPoint)
		{
			HIBERID hiberid=pEndPoint->GetHiberId();
			SmartPartPtr pMasterPart=Ta.Parts.FromHandle(hiberid.masterId);
			if( pMasterPart.IsHasPtr()&&pMasterPart->IsAngle()&&
				RetrieveParaPointByAngleHiberarchy(pMasterPart.pAngle,pEndPoint->GetHiberId(),&endPoint)&&
				endPoint.UpdatePos(&Ta))
				posEnd=endPoint.Position();
			if(pEndPart.IsNULL())
				pEndPart=pMasterPart;
			pCmdLine->FinishCmdLine(CXhChar16("������%d",pEndPoint->GetId()));
		}
		else if(pEndPart.IsNULL())
			logerr.Log("encounter an unexpected error");
	}
	else
		g_pSolidDraw->SetEntSnapStatus(pEndPart->handle);
	bool selectMinDistPoint=false;
	if(pEndPart.IsHasPtr()&&pEndPart->GetClassTypeId()==CLS_BOLT)
	{
		pEndBolt=pEndPart.pBolt;
		posEnd=pEndBolt->ucs.origin;
		pCmdLine->FinishCmdLine(CXhChar16("��ֹ��˨0x%X",pEndBolt->handle));
	}
	else if(pEndPart.IsHasPtr()&&(pStartPoint!=NULL||pStartNode!=NULL||startAngleNode.IsValid()))
	{
		posEnd=pEndPart->ucs.origin;
		selectMinDistPoint=false;
		if(pEndPart->IsAngle())
		{
			endAngleNode.pAngle=pEndPart.pAngle;
			pCmdLine->FillCmdLine("DIST ָ����ǰ�Ǹֽ���ڵ��ֱ��ѡ��ʼĩ�������㣬�س����Զ�ѡ�������:","");
			lifeAngles[1].AttachDisplayState(pEndPart,0);
			CLDSView::RedrawAngleFrameDrawing(pEndPart.pAngle,0x03);
			CLDSNode* pSectionNode=NULL;
			verifier.ClearSnapFlag();
			verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
			verifier.AddVerifyType(OBJPROVIDER::DRAWINGSPACE,IDbEntity::DbPoint);
			//g_pSolidSet->SetSolidAndLineStatus
			if(g_sysPara.m_bLineDepthTest)
			{
				g_pSolidSet->DisableLineModelDepthTest();
				g_pSolidDraw->Draw();
			}
			while(true)
			{
				if((state=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				{
					if(g_sysPara.m_bLineDepthTest)
						g_pSolidSet->EnableLineModelDepthTest();
					pCmdLine->CancelCmdLine();
					return -1;
				}
				SELOBJ obj(dwhObj,dwExportFlag,pDrawing);
				dwhObj=obj.hRelaObj;
				pSectionNode=dwhObj>0?Ta.Node.FromHandle(dwhObj):NULL;
				if(pSectionNode!=NULL)
					break;
				if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
				{
					selectMinDistPoint=true;
					break;
				}
				IDbEntity* pEnt=obj.idEnt>0?pDrawing->GetDbEntity(obj.idEnt):NULL;
				if(pEnt&&pEnt->GetDbEntType()==IDbEntity::DbPoint)//||pEnt->GetDbEntType()==IDbEntity::DbText)
				{
					posEnd=((IDbPoint*)pEnt)->Position();
					posEnd=g_pSolidOper->TransPFromGL(posStart);
					HIBERID hiberid=pEnt->GetHiberId();
					if( pEndPart->IsAngle()&&
						RetrieveParaPointByAngleHiberarchy(pEndPart.pAngle,pEnt->GetHiberId(),&endPoint)&&
						endPoint.UpdatePos(&Ta))
						posEnd=endPoint.Position();
					pEndPoint=pEnt;
					break;
				}
			}
			if(g_sysPara.m_bLineDepthTest)
				g_pSolidSet->EnableLineModelDepthTest();
			if(pSectionNode)
			{
				endAngleNode.pNode=pSectionNode;
				g_pSolidDraw->SetEntSnapStatus(pStartPart->handle,false);
				int iPlaneType=IntelliRecogWorkPlane(pEndPart.pAngle,pSectionNode,0);
				//1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��
				char ciPlaneType=iPlaneType;
				if(iPlaneType==1)
					ciPlaneType='Z';
				else if(iPlaneType==2)
					ciPlaneType='X';
				else if(iPlaneType==3)
					ciPlaneType='Y';
				AppendAngleSectionFrameDrawing(pEndPart.pAngle,pSectionNode,0,0,ciPlaneType);
				pCmdLine->FinishCmdLine(CXhChar16("�ڵ�(0x%X)����",pSectionNode->handle));
			}

		}
		else if(pEndPart->IsPlate())
			lifePlates[1].AttachDisplayState(pEndPart.GeneralPlatePointer(),2);
			//RedrawPlateProfileDrawing(pEndPart.GeneralPlatePointer());
		g_pSolidDraw->Draw();
		if(!(startAngleNode.IsValid()&&endAngleNode.IsValid())&&!selectMinDistPoint&&
			(pEndPart->IsPlate()||pEndPoint==NULL))
		{
			pCmdLine->FillCmdLine("DIST ָ����ǰ�����ϵĹؼ��㣬�س����Զ�ѡ�������","");
			verifier.ClearSnapFlag();
			verifier.AddVerifyType(OBJPROVIDER::DRAWINGSPACE,IDbEntity::DbPoint);
			while(true)
			{
				if((state=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				{
					pCmdLine->CancelCmdLine();
					return -1;
				}
				SELOBJ obj(dwhObj,dwExportFlag,pDrawing);
				dwhObj=obj.hRelaObj;
				if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
				{
					selectMinDistPoint=true;
					break;
				}
				IDbEntity* pEnt=obj.idEnt>0?pDrawing->GetDbEntity(obj.idEnt):NULL;
				if(pEnt&&pEnt->GetDbEntType()==IDbEntity::DbPoint)//||pEnt->GetDbEntType()==IDbEntity::DbText)
				{
					posEnd=((IDbPoint*)pEnt)->Position();
					posEnd=g_pSolidOper->TransPFromGL(posEnd);
					HIBERID hiberid=pEnt->GetHiberId();
					if( pEndPart.IsHasPtr()&&pEndPart->IsAngle()&&
						RetrieveParaPointByAngleHiberarchy(pEndPart.pAngle,pEnt->GetHiberId(),&endPoint)&&
						endPoint.UpdatePos(&Ta))
						posEnd=endPoint.Position();
					pEndPoint=pEnt;
					break;
				}
			}
			pCmdLine->FinishCmdLine();
		}
	}
	CLDSModule* pActivateModule=Ta.GetActiveModule();
	double distance=0;
	bool measured=false;
	if((pStartNode&&pEndNode)||(pStartPoint&&pEndPoint))
		measured=true;
	else if(pStartBolt&&pEndBolt)
	{
		SmartPartPtr pDatumPart;
		if (pStartBoltResidePart.blValid)
			pDatumPart=pStartBoltResidePart;
		else
			pDatumPart=Ta.Parts.FromHandle(pStartBolt->des_base_pos.hPart);
		bool shareDatumPart=false;
		if(pDatumPart.IsNULL())
		{
			if (pEndBoltResidePart.blValid)
				pDatumPart=pEndBoltResidePart;
			else
				pDatumPart=Ta.Parts.FromHandle(pEndBolt->des_base_pos.hPart);
			if(pDatumPart->FindLsByHandle(pStartBolt->handle)!=NULL)
				shareDatumPart=true;
		}
		else if(pDatumPart->FindLsByHandle(pEndBolt->handle)!=NULL)
			shareDatumPart=true;
		else if(pStartBolt->des_base_pos.hPart==pEndBolt->des_base_pos.hPart)
			shareDatumPart=true;//ͬһ��׼����
		if(!shareDatumPart)
		{
			PARTSET partset;
			FindBoltRelatedParts(pStartBolt,partset);
			for(CLDSPart* pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			{
				if(pDatumPart.IsEqualPtr(pPart)||!pActivateModule->IsSonPart(pPart))
					continue;
				if(pPart->FindLsByHandle(pEndBolt->handle))
					break;
			}
			if(pPart)
			{
				pDatumPart=pPart;
				shareDatumPart=true;
			}
		}
		if(shareDatumPart&&pDatumPart.IsHasPtr())
		{
			pDatumPart->GetBoltIntersPos(pStartBolt,posStart);
			pDatumPart->GetBoltIntersPos(pEndBolt,posEnd);
			if(pDatumPart->IsAngle()&&theApp.m_bManuRevisionMode&&PRODUCT_FUNC::IsHasInternalTest())
			{
				CSel2BoltsDistDlg distboltdlg;
				distboltdlg.m_pDatumBolt=pStartBolt;
				distboltdlg.m_pDestBolt=pEndBolt;
				distboltdlg.m_pDatumAngle=pDatumPart.pAngle;
				distboltdlg.posStart=posStart;
				distboltdlg.posEnd=posEnd;
				if(distboltdlg.DoModal()!=IDOK)
					return 0;
				bCheckDimensionApppend=distboltdlg.m_bAddChkItemD||distboltdlg.m_bAddChkItemG||distboltdlg.m_bAddChkItemL;
			}
		}
		else if(fabs(pStartBolt->ucs.axis_z*pEndBolt->ucs.axis_z)>EPS_COS2)
		{
			f3dPoint inters=pEndBolt->ucs.origin;
			Int3dlf(inters,inters,pEndBolt->ucs.axis_z,pStartBolt->ucs.origin,pStartBolt->ucs.axis_z);
			double dd=(inters-pEndBolt->ucs.origin)*pEndBolt->ucs.axis_z;
			if(fabs(dd)<pEndBolt->get_L())
				posEnd=inters;
			else
				posEnd=pEndBolt->ucs.origin;
			posStart=pStartBolt->ucs.origin;
		}
		else
		{
			posStart=pStartBolt->ucs.origin;
			posEnd=pEndBolt->ucs.origin;
		}
		measured=true;
	}
	else if(startAngleNode.IsValid()&&endAngleNode.IsValid())
	{	//'�Ǹ�-�ڵ�' vs '�Ǹ�-�ڵ�'ģʽ�������
		GEPOINT vLenVec1=startAngleNode.pAngle->End()-startAngleNode.pAngle->Start();
		GEPOINT vLenVec2=endAngleNode.pAngle->End()-endAngleNode.pAngle->Start();
		GEPOINT vWorkNorm=CalFaceNorm(startAngleNode.pAngle->Start(),startAngleNode.pAngle->End(),endAngleNode.pAngle->Start(),endAngleNode.pAngle->End());
		char ciStartWorkWingX0Y1=fabs(startAngleNode.pAngle->vxWingNorm*vWorkNorm)>fabs(startAngleNode.pAngle->vyWingNorm*vWorkNorm)?0:1;
		char ciEndWorkWingX0Y1  =fabs(endAngleNode.pAngle->vxWingNorm*vWorkNorm)>fabs(endAngleNode.pAngle->vyWingNorm*vWorkNorm)?0:1;
		CSel2JgKeyPointDlg distdlg;
		distdlg.m_pFromAngle=startAngleNode.pAngle;
		distdlg.m_pFromAngleNode=startAngleNode.pNode;
		distdlg.m_pToAngle=endAngleNode.pAngle;
		distdlg.m_pToAngleNode=endAngleNode.pNode;
		distdlg.m_iFirstKeyPoint =ciStartWorkWingX0Y1+1;
		distdlg.m_iSecondKeyPoint=ciEndWorkWingX0Y1+1;
		distdlg.m_iFirstOffsetStyle =IntelliRecogWorkPlane(startAngleNode.pAngle,startAngleNode.pNode,ciStartWorkWingX0Y1);
		distdlg.m_iSecondOffsetStyle=IntelliRecogWorkPlane(endAngleNode.pAngle,endAngleNode.pNode,ciEndWorkWingX0Y1);
		if(distdlg.DoModal()!=IDOK)
			return 0;
		bCheckDimensionApppend=distdlg.m_bAddCheckDim;
		posStart=distdlg.GetStartPt();
		posEnd=distdlg.GetEndPt();
	}
	else if(pStartPoint!=NULL&&pEndPart.IsHasPtr()&&pEndPart->IsLinePart()||
			pEndPoint!=NULL&&pStartPart.IsHasPtr()&&pStartPart->IsLinePart())
	{
		//����ѡ��ʼ�˹ؼ�����û�����ˢ����Ļ����pStartPoint��Ч,�������������ʴ˴���������pStartPoint,
		//����Ϊֱ��ʹ��֮ǰ���ɵ�startPoint wjh-2016.9.23
		//IDbEntity* pPoint=pStartPoint!=NULL?pStartPoint:pEndPoint;
		//if(pStartPoint!=NULL)
		//	posStart=startPoint.Position();
		//else if(pEndPoint!=NULL)
		//	posStart=endPoint.Position();
		SmartRodPtr pRod=pStartPoint!=NULL?pEndPart.pRod:pStartPart.pRod;
		f3dPoint perp,vec;
		GEPOINT lenStdVec=pRod->End()-pRod->Start();
		double rodLength=lenStdVec.mod();
		if(rodLength>EPS)
			lenStdVec/=rodLength;
		if(pRod->GetClassTypeId()==CLS_LINETUBE)
		{	//�㵽�ֹ���̾���
			if(SnapPerp(&perp,pRod->Start(),pRod->End(),posStart))
				posEnd=perp;
			if((perp-pRod->Start())*lenStdVec<-pRod->startOdd())
				perp=pRod->Start();
			else if((perp-pRod->End())*lenStdVec>rodLength+pRod->endOdd())
				perp=pRod->End();
			vec=posStart-posEnd;
			normalize(vec);
			posEnd+=vec*(pRod.pTube->GetDiameter()*0.5);
		}
		else if(pRod->GetClassTypeId()==CLS_LINEANGLE)
		{
			double dist=MeasureMinDistanceOfPointToAngle(posStart,pRod.pAngle,vec,rodLength);
			posEnd=posStart+vec;
		}
	}
	else if((pStartBolt!=NULL&&pEndPoint!=NULL)||(pEndBolt!=NULL&&pStartPoint!=NULL))
	{
		CLDSBolt* pBolt=pStartBolt!=NULL?pStartBolt:pEndBolt;
		//IDbEntity* pPoint=pStartPoint!=NULL?pStartPoint:pEndPoint;
		if(pStartPoint!=NULL)
			posEnd=startPoint.Position();
		else if(pEndPoint!=NULL)
			posEnd=endPoint.Position();
		f3dPoint perp;
		posStart=pBolt->ucs.origin;
		if(SnapPerp(&perp,posStart,posStart+pBolt->ucs.axis_z,posEnd))
			posStart=perp;
		if( fabs(pStartBolt->ucs.axis_z*pEndBolt->ucs.axis_z)>EPS_COS2&&
			pStartBolt->des_base_pos.hPart==pEndBolt->des_base_pos.hPart)
		{	//����˨���Ӧ�ֺ����������ע 
			distance=DISTANCE(posStart,posEnd);
			if(!bCheckDimensionApppend)
			{
				f3dLine distline(posStart,posEnd);
				distline.pen.crColor=RGB(255,255,0);
				g_pSolidDraw->NewLine(distline);
			}
			//g_pSolidDraw->Draw();
			MessageBox(pView->GetSafeHwnd(),CXhChar16("d =%.2fmm",distance),"����",MB_OK);
			g_pSolidDraw->ReleaseSnapStatus();
			needRsltDisplay=false;
		}
	}
	else if(pStartBolt!=NULL&&pEndPart.IsHasPtr()&&pEndPart->IsLinePart()||
			pEndBolt!=NULL&&pStartPart.IsHasPtr()&&pStartPart->IsLinePart())
	{
		CLDSBolt* pBolt=pStartBolt!=NULL?pStartBolt:pEndBolt;
		SmartRodPtr pRod=pStartBolt!=NULL?pEndPart.pRod:pStartPart.pRod;
		f3dPoint perp,vec;
		GEPOINT lenStdVec=pRod->End()-pRod->Start();
		double rodLength=lenStdVec.mod();
		if(rodLength>EPS)
			lenStdVec/=rodLength;
		rodLength+=pRod->startOdd()+pRod->endOdd();
		posStart=pBolt->ucs.origin;
		if(pRod->IsAngle()&&pRod.pAngle->GetBoltIntersPos(pBolt,perp))
			posStart=perp;
		else if (fabs(pBolt->ucs.axis_z*lenStdVec) < EPS_COS&&
			Int3dDifferPlaneLL(pBolt->ucs.origin, pBolt->ucs.origin + pBolt->ucs.axis_z*pBolt->get_L(), pRod->Start(), pRod->End(), perp) > 0)
		{
			posStart = perp;	//��˨������˼���������
			GEPOINT vzLenVec = f3dPoint(posStart) - pBolt->ucs.origin;
			double zLenPos = vzLenVec * pBolt->ucs.axis_z;
			if (zLenPos > pBolt->L)
				posStart = pBolt->ucs.origin + pBolt->dfLe*pBolt->ucs.axis_z;
			else
			{
				double D, H = 0, H2 = 0;
				FindLsD_H(pBolt->d, &D, &H, &H2);
				double zReverseLen = !pBolt->IsFootNail() ? H2 : pBolt->L - pBolt->dfLe;
				if (zLenPos < -zReverseLen)	//��ʱ��d��Ϊ�ж�����
					posStart = pBolt->ucs.origin - zReverseLen * pBolt->ucs.axis_z;
			}
		}

		if(pRod->GetClassTypeId()==CLS_LINETUBE)
		{	//�㵽�ֹ���̾���
			if(SnapPerp(&perp,pRod->Start(),pRod->End(),posStart))
				posEnd=perp;
			if((perp-pRod->Start())*lenStdVec<-pRod->startOdd())
				posEnd=pRod->Start();
			else if((perp-pRod->Start())*lenStdVec>rodLength+pRod->endOdd())
				posEnd=pRod->End();
			vec=posStart-posEnd;
			normalize(vec);
			posEnd+=vec*(pRod.pTube->GetDiameter()*0.5);
		}
		else if(pRod->GetClassTypeId()==CLS_LINEANGLE)
		{
			double dist=MeasureMinDistanceOfPointToAngle(posStart,pRod.pAngle,vec,rodLength);
			posEnd=posStart+vec;
		}
	}
	else if(pStartPart.IsHasPtr()&&pEndPart.IsHasPtr()&&pStartPart->IsAngle()&&pEndPart->IsAngle())
	{
		distance=MeasureMinDistanceBetweenAngles(pStartPart.pAngle,pEndPart.pAngle,posStart,posEnd);
		if(pStartPart.pAngle->IsCutAngleOrWing()||pEndPart.pAngle->IsCutAngleOrWing())
		{	//�����н�ʱ����Ӧ����ʵ�嶥�㼶��С������
			GEPOINT posStart2,posEnd2;
			if(MeasureMinDistanceBetweenParts(pStartPart,pEndPart,posStart2,posEnd2)>distance)
			{
				posStart=posStart2;
				posEnd=posEnd2;
			}
		}
	}
	else if(pStartPart.IsHasPtr()&&pEndPart.IsHasPtr())//&&pStartPart->IsLinePart()&&pEndPart->IsLinePart())
	{	//��������������С����
		distance=MeasureMinDistanceBetweenParts(pStartPart,pEndPart,posStart,posEnd);
	}
	if(needRsltDisplay)
	{
		distance=DISTANCE(posStart,posEnd);
		if(!bCheckDimensionApppend)
		{
			f3dLine distline(posStart,posEnd);
			distline.pen.crColor=RGB(255,255,0);
			g_pSolidDraw->NewLine(distline);
		}
		//g_pSolidDraw->Draw();
		MessageBox(pView->GetSafeHwnd(),CXhChar16("d =%.2fmm",distance),"����",MB_OK);
		g_pSolidDraw->ReleaseSnapStatus();
	}
	}
	_LocalDistMeasure(pView);	//�ظ���������
	return 0;
}
int CLDSView::DistMeasure()
{
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	return _LocalDistMeasure(this);
}

void CLDSView::OnViewJgKeyPoint()
{
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return;
	JgKeyPoint();
	//Command("AngleKeyPoint");
}
int CLDSView::JgKeyPoint()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("AngleKeyPoint Please select one angle:","");
#else
	pCmdLine->FillCmdLine("AngleKeyPoint ��ѡ��ؼ������ڵĽǸ�:","");
#endif
	g_pSolidDraw->ReleaseSnapStatus();
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE));
	verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prLine);
	CLDSLineAngle* pSelAngle=NULL;
	DWORD dwhObj=0,dwExportFlag=0;
	while(1)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		dwhObj=obj.hRelaObj;
		if(dwhObj>0x20)
		{
			pSelAngle=(CLDSLineAngle*)console.FromPartHandle(dwhObj,CLS_LINEANGLE);
			if(pSelAngle)
				break;
		}
	}
	g_pSolidDraw->SetEntSnapStatus(pSelAngle->handle);
	pCmdLine->FinishCmdLine(CXhChar16("0X%X",pSelAngle->handle));
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("AngleKeyPoint Please select one node for key point:","");
#else
	pCmdLine->FillCmdLine("AngleKeyPoint ��ѡ��ؼ���������ڵĽڵ�:","");
#endif
	CLDSNode* pSelNode;
	verifier.ClearSnapFlag();
	verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
	verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prPoint);
	CDisplayNodeAtFrontLife showPoint;
	showPoint.DisplayNodeAtFront();
	while(1)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		dwhObj=obj.hRelaObj;
		if(dwhObj>0x20)
		{
			pSelNode=console.FromNodeHandle(dwhObj);
			if(pSelNode)
				break;
		}
	}
	g_pSolidDraw->SetEntSnapStatus(pSelNode->handle);
	pCmdLine->FinishCmdLine(CXhChar16("0X%X",pSelNode->handle));
	FinishViewKeyPoint(pSelAngle,pSelNode);
	g_pSolidDraw->ReleaseSnapStatus();
	JgKeyPoint();
	return TRUE;
}

void CLDSView::OnLinePartExtendVec() 
{
	m_nPrevCommandID=ID_LINEPART_EXTEND_VEC;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat rod extending direction";
#else
	m_sPrevCommandName="�ظ��˼����췽��";
#endif
	m_curTask = TASK_CAL_JG_EXTEND_VEC;
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_LINE);
	m_nObjectSnapedCounts=0;
	m_setSnapObjects.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
	g_pPromptMsg->SetMsg("select the angle that the extending direction will be measured");
#else
	g_pPromptMsg->SetMsg("ѡ��Ҫ�������췽��ĽǸ�");
#endif
}

void CLDSView::OnCalWorkFaceNorm() 
{
	m_nPrevCommandID=ID_CAL_WORK_FACE_NORM;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to calculate work plane normal line";
#else
	m_sPrevCommandName="�ظ����㹤���淨��";
#endif
	m_curTask = TASK_CAL_WORK_FACE_NORM;
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_LINE);
	m_nObjectSnapedCounts=0;
	m_setSnapObjects.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
	g_pPromptMsg->SetMsg("select two unparallel angle line on the work plane");
#else
	g_pPromptMsg->SetMsg("ѡ�������ϵ�������ƽ�еĽǸ���");
#endif
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
CSelJgPointKeyDlg _LocalAngleKeyPointDlg;
BOOL CLDSView::FinishViewKeyPoint(CLDSDbObject *pObj1, CLDSDbObject *pObj2)
{
	CXhChar50 idstr;
	CDataCheckModel::KEYPOINT point;
	_LocalAngleKeyPointDlg.m_pJg=(CLDSLineAngle*)pObj1;
	_LocalAngleKeyPointDlg.m_pNode=(CLDSNode*)pObj2;
	if(_LocalAngleKeyPointDlg.m_pJg->m_cPosCtrlDatumLineType=='X')
		_LocalAngleKeyPointDlg.m_iKeyPoint=1;
	else if(_LocalAngleKeyPointDlg.m_pJg->m_cPosCtrlDatumLineType=='Y')
		_LocalAngleKeyPointDlg.m_iKeyPoint=2;
	else
		_LocalAngleKeyPointDlg.m_iKeyPoint=0;
	if(_LocalAngleKeyPointDlg.m_pJg->m_cPosCtrlType<4)
		_LocalAngleKeyPointDlg.m_iOffsetStyle=_LocalAngleKeyPointDlg.m_pJg->m_cPosCtrlType;
	if(PRODUCT_FUNC::IsHasInternalTest()&&theApp.m_bManuRevisionMode)
	{
		point.ciPointType=0;
		point.angle.hAngle=pObj1->handle;
		point.angle.hNode=pObj2->handle;
		memset(&point.angle.offset,0,sizeof(ANGLEOFFSET));
		if(_LocalAngleKeyPointDlg.m_iKeyPoint==0)
			point.angle.offset.ciOffsetWing=2;
		else
			point.angle.offset.ciOffsetWing=_LocalAngleKeyPointDlg.m_iKeyPoint-1;
		point.angle.offset.ciOffsetPlane=_LocalAngleKeyPointDlg.m_iOffsetStyle;
		idstr=CDataCheckModel::SIZE_DATA::CodingIdStr(0,point,point);
		CDataCheckModel::SIZE_DATA* pExistItem=xDataCheckModel.GetCoordSizeItem(idstr);
		if(pExistItem)
		{
			_LocalAngleKeyPointDlg.m_bChkCoordX=(pExistItem->cbSizeDimFlag&0x01)>0;
			_LocalAngleKeyPointDlg.m_bChkCoordY=(pExistItem->cbSizeDimFlag&0x02)>0;
			_LocalAngleKeyPointDlg.m_bChkCoordZ=(pExistItem->cbSizeDimFlag&0x04)>0;
		}
		else
			_LocalAngleKeyPointDlg.m_bChkCoordX=_LocalAngleKeyPointDlg.m_bChkCoordY=_LocalAngleKeyPointDlg.m_bChkCoordZ=FALSE;
	}

	_LocalAngleKeyPointDlg.UpdateAndShowDialog();
	return TRUE;
}

//����������Ϣ����˨�˾�,��� wht 11-04-26
static LSSPACE_STRU SetConnectInfoAndLsSpace(CLDSLineAngle *pBaseJg,long base_node_handle)
{
	LSSPACE_STRU LsSpace;
	CJointBaseJg lsInfoDlg;
	//������˨��������˨��������˨���˾�
	if(pBaseJg->pStart&&pBaseJg->pStart->handle==base_node_handle)
	{
		//if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
		//	lsInfoDlg.m_nLsN = pBaseJg->connectStart.N/4;
		//else
		//	lsInfoDlg.m_nLsN = pBaseJg->connectStart.N/2;
		lsInfoDlg.m_nLsN=pBaseJg->wnStartConnBoltN;
		lsInfoDlg.m_nLsN=max(3,lsInfoDlg.m_nLsN);
		lsInfoDlg.m_nLsRows = pBaseJg->connectStart.rows-1;
		lsInfoDlg.m_sLsGuiGe.Format("%d",pBaseJg->connectStart.d);
		lsInfoDlg.m_sJgGuiGe.Format("L%.0fX%.0f",pBaseJg->GetWidth(),pBaseJg->GetThick());
		GetLsSpace(LsSpace,pBaseJg->connectStart.d);
		lsInfoDlg.m_nLsEndSpace=LsSpace.EndSpace;
		if(lsInfoDlg.m_nLsRows==0)
			lsInfoDlg.m_nLsSpace=LsSpace.SingleRowSpace;
		else
			lsInfoDlg.m_nLsSpace=LsSpace.doubleRowSpace;
		if(lsInfoDlg.DoModal()==IDOK)
		{
			//������˨�˾��Լ���˨���
			LsSpace.EndSpace=lsInfoDlg.m_nLsEndSpace;
			if(lsInfoDlg.m_nLsRows==0)
				LsSpace.SingleRowSpace=lsInfoDlg.m_nLsSpace;
			else 
				LsSpace.doubleRowSpace=lsInfoDlg.m_nLsSpace;
			//������˨��������˨����
			pBaseJg->connectStart.wnConnBoltN=lsInfoDlg.m_nLsN;
			pBaseJg->connectStart.rows=lsInfoDlg.m_nLsRows+1;
			sscanf(lsInfoDlg.m_sLsGuiGe,"%d",&pBaseJg->connectStart.d);
			pBaseJg->connectStart.LsSpace.EndSpace=LsSpace.EndSpace;
			pBaseJg->connectStart.LsSpace.SingleRowSpace=LsSpace.SingleRowSpace;
			pBaseJg->connectStart.LsSpace.DoubleRowSpace=LsSpace.doubleRowSpace;
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pBaseJg;
				for(int i=0;i<4;i++)
				{
					CLDSLineAngle *pCommBaseJg=NULL;
					if(pGroupAngle&&pGroupAngle->son_jg_h[i]>=0x20)
						pCommBaseJg=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
					if(pCommBaseJg==NULL)
						continue;
					pCommBaseJg->connectStart.wnConnBoltN=lsInfoDlg.m_nLsN;
					pCommBaseJg->connectStart.rows=lsInfoDlg.m_nLsRows+1;
					sscanf(lsInfoDlg.m_sLsGuiGe,"%d",&pCommBaseJg->connectStart.d);
					pCommBaseJg->connectStart.LsSpace.EndSpace=LsSpace.EndSpace;
					pCommBaseJg->connectStart.LsSpace.SingleRowSpace=LsSpace.SingleRowSpace;
					pCommBaseJg->connectStart.LsSpace.DoubleRowSpace=LsSpace.doubleRowSpace;
				}
			}
		}
	}
	else if(pBaseJg->pEnd&&pBaseJg->pEnd->handle==base_node_handle)
	{
		//if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
		//	lsInfoDlg.m_nLsN = pBaseJg->connectEnd.N/4;
		//else
		//	lsInfoDlg.m_nLsN = pBaseJg->connectEnd.N/2;
		lsInfoDlg.m_nLsN=pBaseJg->wnEndConnBoltN;
		lsInfoDlg.m_nLsN=max(3,lsInfoDlg.m_nLsN);
		lsInfoDlg.m_nLsRows = pBaseJg->connectEnd.rows-1;
		lsInfoDlg.m_sLsGuiGe.Format("%d",pBaseJg->connectEnd.d);
		lsInfoDlg.m_sJgGuiGe.Format("L%.0fX%.0f",pBaseJg->GetWidth(),pBaseJg->GetThick());
		GetLsSpace(LsSpace,pBaseJg->connectEnd.d);
		lsInfoDlg.m_nLsEndSpace=LsSpace.EndSpace;
		if(lsInfoDlg.m_nLsRows==0)
			lsInfoDlg.m_nLsSpace=LsSpace.SingleRowSpace;
		else
			lsInfoDlg.m_nLsSpace=LsSpace.doubleRowSpace;
		if(lsInfoDlg.DoModal()==IDOK)
		{	
			//������˨�˾��Լ���˨���
			LsSpace.EndSpace=lsInfoDlg.m_nLsEndSpace;
			if(lsInfoDlg.m_nLsRows==0)
				LsSpace.SingleRowSpace=lsInfoDlg.m_nLsSpace;
			else 
				LsSpace.doubleRowSpace=lsInfoDlg.m_nLsSpace;
			//������˨��������˨����
			pBaseJg->connectEnd.wnConnBoltN=lsInfoDlg.m_nLsN;
			pBaseJg->connectEnd.rows=lsInfoDlg.m_nLsRows+1;
			sscanf(lsInfoDlg.m_sLsGuiGe,"%d",&pBaseJg->connectEnd.d);
			pBaseJg->connectEnd.LsSpace.EndSpace=LsSpace.EndSpace;
			pBaseJg->connectEnd.LsSpace.SingleRowSpace=LsSpace.SingleRowSpace;
			pBaseJg->connectEnd.LsSpace.DoubleRowSpace=LsSpace.doubleRowSpace;
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pBaseJg;
				for(int i=0;i<4;i++)
				{
					CLDSLineAngle *pCommBaseJg=NULL;
					if(pGroupAngle&&pGroupAngle->son_jg_h[i]>=0x20)
						pCommBaseJg=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
					if(pCommBaseJg==NULL)
						continue;
					pCommBaseJg->connectEnd.wnConnBoltN=lsInfoDlg.m_nLsN;
					pCommBaseJg->connectEnd.rows=lsInfoDlg.m_nLsRows+1;
					sscanf(lsInfoDlg.m_sLsGuiGe,"%d",&pCommBaseJg->connectEnd.d);
					pCommBaseJg->connectEnd.LsSpace.EndSpace=LsSpace.EndSpace;
					pCommBaseJg->connectEnd.LsSpace.SingleRowSpace=LsSpace.SingleRowSpace;
					pCommBaseJg->connectEnd.LsSpace.DoubleRowSpace=LsSpace.doubleRowSpace;
				}
			}
		}
	}
	return LsSpace;
}
//�׽Ű����
BOOL CLDSView::FinishFootPlank(CLDSDbObject *pAtom1,CLDSDbObject *pAtom2) 
{
	CLDSLineAngle *pSelJg=NULL;
	CLDSNode* pBaseNode=NULL;
	double wing_wide, wing_thick;
	static CFootDesignDlg dlg;
	if(pAtom1->GetClassTypeId()==CLS_NODE)
		pBaseNode = (CLDSNode*)pAtom1;
	if(pAtom2->GetClassTypeId()==CLS_LINEANGLE||pAtom2->GetClassTypeId()==CLS_GROUPLINEANGLE)
		pSelJg = (CLDSLineAngle*)pAtom2;
	if(pBaseNode==NULL||pSelJg==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("select wrong tower parts,please select a node and an angle!");
#else
		AfxMessageBox("ѡ���˴����������,��ѡ��һ���ڵ㼰һ���Ǹ�!");
#endif
		return FALSE;
	}
	else if(pSelJg->pStart->handle!=pBaseNode->handle&&pSelJg->pEnd->handle!=pBaseNode->handle)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("selected main material angle and node donot match");
#else
		AfxMessageBox("��ѡ���ĽǸ���ڵ㲻ƥ��");
#endif
		return FALSE;
	}

	try
	{
		wing_wide = pSelJg->GetWidth();
		wing_thick = pSelJg->GetThick();
		CFoot foot;
		foot.SetBelongModel(console.GetActiveModel());
		foot.attach_jg_handle[0] = pSelJg->handle;
		foot.base_node_handle = pBaseNode->handle;
		dlg.m_pFoot = &foot;
		//������˨������Ϣ����˨�˾��� wht 11-04-26
		SetConnectInfoAndLsSpace(pSelJg,pBaseNode->handle);
		if(dlg.DoModal()!=IDOK)
		{
			g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("design interrupt!");
#else
			AfxMessageBox("����жϣ�");
#endif
			return FALSE;
		}
		else
		{
			g_pSolidDraw->ReleaseSnapStatus();
			m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
			CUndoOperObject undo(&Ta,true);
			if(!dlg.DesignFoot())
				return FALSE;
			theApp.GetWorkPathAuto();	//��ȡ��ǰ�ļ����ڵ�Ŀ¼
			if(!dlg.m_bSaveData)
			{	//��̤�Ű�������ݱ��浽Ĭ��·����
				CString sFileName;
				sFileName.Format("%sFootData.xml",theApp.work_path);
				dlg.XMLDataSerialize(sFileName,TRUE);
			}
			else
			{	//��浽�û�ָ���ļ���
#ifdef AFX_TARG_ENU_ENGLISH
				CFileDialog file_dlg(FALSE,theApp.work_path,"tower foot plate",
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"tower foot plate(*.xml)|*.xml||");
#else
				CFileDialog file_dlg(FALSE,theApp.work_path,"���Ű�",
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"���Ű�(*.xml)|*.xml||");
#endif
				if(file_dlg.DoModal()!=IDOK)
					return FALSE;
				CString sFileName = file_dlg.GetPathName();
				dlg.XMLDataSerialize(sFileName,TRUE);
			}
		}
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
		g_pSolidDraw->ReleaseSnapStatus();
		return FALSE;
	}
	return TRUE;
}
//�׽Ű����
BOOL CLDSView::FinishHoofPlank(CLDSDbObject *pAtom1,CLDSDbObject *pAtom2,CLDSDbObject *pAtom3) 
{
	CLDSLineAngle *pSelJg[2]={NULL};
	CLDSNode* pBaseNode=NULL;
	static CFootDesignDlg dlg;
	if(pAtom1->GetClassTypeId()==CLS_NODE)
		pBaseNode = (CLDSNode*)pAtom1;
	if((pAtom2->GetClassTypeId()==CLS_LINEANGLE||pAtom2->GetClassTypeId()==CLS_GROUPLINEANGLE))
		pSelJg[0] = (CLDSLineAngle*)pAtom2;
	if(pAtom3->GetClassTypeId()==CLS_LINEANGLE||pAtom3->GetClassTypeId()==CLS_GROUPLINEANGLE)
		pSelJg[1] = (CLDSLineAngle*)pAtom3;
	if(pBaseNode==NULL||pSelJg[0]==NULL||pSelJg[1]==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("select wrong tower parts,please select a node and two angle plates!");
#else
		AfxMessageBox("ѡ���˴����������,��ѡ��һ���ڵ㼰�����Ǹ�!");
#endif
		return FALSE;
	}
	else if(pSelJg[0]->pStart->handle!=pBaseNode->handle&&pSelJg[0]->pEnd->handle!=pBaseNode->handle)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("first selected main material angle and node don't match");
#else
		AfxMessageBox("��ѡ��һ�����ĽǸ���ڵ㲻ƥ��");
#endif
		return FALSE;
	}
	else if(pSelJg[1]->pStart->handle!=pBaseNode->handle&&pSelJg[1]->pEnd->handle!=pBaseNode->handle)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("second selected main material angle and node do not match");
#else
		AfxMessageBox("��ѡ�ڶ������ĽǸ���ڵ㲻ƥ��");
#endif
		return FALSE;
	}

	CFoot foot;
	foot.SetBelongModel(console.GetActiveModel());
	foot.attach_jg_handle[0] = pSelJg[0]->handle;
	foot.attach_jg_handle[1] = pSelJg[1]->handle;
	foot.base_node_handle = pBaseNode->handle;
	dlg.m_pFoot = &foot;
	//������˨������Ϣ����˨�˾��� wht 11-04-26
	SetConnectInfoAndLsSpace(pSelJg[0],pBaseNode->handle);
	SetConnectInfoAndLsSpace(pSelJg[1],pBaseNode->handle);
	if(dlg.DoModal()!=IDOK)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		return FALSE;
	}
	else
	{
		CUndoOperObject undo(&Ta,true);
		if(!dlg.DesignFoot())
			return FALSE;
		theApp.GetWorkPathAuto();	//��ȡ��ǰ�ļ����ڵ�Ŀ¼
		if(!dlg.m_bSaveData)
		{	//��̤�Ű�������ݱ��浽Ĭ��·����
			CString sFileName;
			sFileName.Format("%sFootData.xml",theApp.work_path);
			dlg.XMLDataSerialize(sFileName,TRUE);
		}
		else
		{	//��浽�û�ָ���ļ���
#ifdef AFX_TARG_ENU_ENGLISH
			CFileDialog file_dlg(FALSE,theApp.work_path,"tower foot plate",
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"tower foot plate(*.xml)|*.xml||");
#else
			CFileDialog file_dlg(FALSE,theApp.work_path,"���Ű�",
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"���Ű�(*.xml)|*.xml||");
#endif
			if(file_dlg.DoModal()!=IDOK)
				return FALSE;
			CString sFileName = file_dlg.GetPathName();
			dlg.XMLDataSerialize(sFileName,TRUE);
		}
		g_pSolidDraw->ReleaseSnapStatus();
		m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	}
	return TRUE;
}
#endif

BOOL CLDSView::FinishMeasure(CLDSDbObject* pObj1,CLDSDbObject *pObj2/*=NULL*/,CLDSDbObject* pObj3/*=NULL*/,CLDSDbObject *pObj4/*=NULL*/)
{
	CString str;
	f3dPoint start, end, vec;
	CLDSNode *pSelNode1=NULL, *pSelNode2=NULL;
	CLDSLineAngle *pSelJg1=NULL, *pSelJg2=NULL;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CSelJgPointKeyDlg KeyPointDlg;
	CSel2JgKeyPointDlg KeyPoint2Dlg;
#endif
	if( pObj1->GetClassTypeId()==CLS_NODE&&pObj2->GetClassTypeId()==CLS_NODE)
	{	//�����Ϊ�ڵ�
		pSelNode1 = (CLDSNode*)pObj1;
		pSelNode2 = (CLDSNode*)pObj2;
		start = pSelNode1->ActivePosition();
		end	  = pSelNode2->ActivePosition();
		vec=end-start;
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(pObj1->GetClassTypeId()==CLS_NODE&&
		(pObj2->GetClassTypeId()==CLS_LINEANGLE||pObj2->GetClassTypeId()==CLS_GROUPLINEANGLE)&&
		pObj3->GetClassTypeId()==CLS_NODE)
	{	//��һ��Ϊ�ڵ㣬�ڶ���Ϊ�Ǹֹؼ���
		KeyPointDlg.m_pJg=(CLDSLineAngle*)pObj2;
		KeyPointDlg.m_pNode=(CLDSNode*)pObj3;
		if(KeyPointDlg.DoModal()!=IDOK)
			return FALSE;
		pSelNode1=(CLDSNode*)pObj1;
		start=pSelNode1->ActivePosition();
		end.Set(KeyPointDlg.m_fPosX,KeyPointDlg.m_fPosY,KeyPointDlg.m_fPosZ);
		vec=end-start;
	}
	else if((pObj1->GetClassTypeId()==CLS_LINEANGLE||pObj1->GetClassTypeId()==CLS_GROUPLINEANGLE)&&
		pObj2->GetClassTypeId()==CLS_NODE&&pObj3->GetClassTypeId()==CLS_NODE)
	{	//��һ��Ϊ�Ǹֹؼ��㣬�ڶ���Ϊ�ڵ�
		KeyPointDlg.m_pJg=(CLDSLineAngle*)pObj1;
		KeyPointDlg.m_pNode=(CLDSNode*)pObj2;
		if(KeyPointDlg.DoModal()!=IDOK)
			return FALSE;
		pSelNode1=(CLDSNode*)pObj3;
		start=pSelNode1->ActivePosition();
		end.Set(KeyPointDlg.m_fPosX,KeyPointDlg.m_fPosY,KeyPointDlg.m_fPosZ);
		vec=end-start;
	}
	else if((pObj1->GetClassTypeId()==CLS_LINEANGLE||pObj1->GetClassTypeId()==CLS_GROUPLINEANGLE)&&
		pObj2->GetClassTypeId()==CLS_NODE && 
		(pObj3->GetClassTypeId()==CLS_LINEANGLE||pObj3->GetClassTypeId()==CLS_GROUPLINEANGLE)&&
		pObj4->GetClassTypeId()==CLS_NODE)
	{	//���Ǹֹؼ���
		pSelNode1=(CLDSNode*)pObj2;
		pSelNode2=(CLDSNode*)pObj4;
		pSelJg1=(CLDSLineAngle*)pObj1;
		pSelJg2=(CLDSLineAngle*)pObj3;
		f3dPoint norm;
		GetWorkNorm(pSelJg1,pSelJg2,&norm);
		IsInsideJg(pSelJg1,norm,&KeyPoint2Dlg.m_iFirstKeyPoint);
		KeyPoint2Dlg.m_iFirstKeyPoint++;
		IsInsideJg(pSelJg2,norm,&KeyPoint2Dlg.m_iSecondKeyPoint);
		KeyPoint2Dlg.m_iSecondKeyPoint++;
		if(KeyPoint2Dlg.DoModal()!=IDOK)
			return FALSE;

		if(KeyPoint2Dlg.m_iFirstOffsetStyle==0)	//ͶӰ
		{
			if(KeyPoint2Dlg.m_iFirstKeyPoint==0)
				start=pSelJg1->GetDatumPosBer(pSelNode1);
			else if(KeyPoint2Dlg.m_iFirstKeyPoint==1)
				start=pSelJg1->GetDatumPosWingX(pSelNode1);
			else
				start=pSelJg1->GetDatumPosWingY(pSelNode1);
		}
		else
		{
			if(KeyPoint2Dlg.m_iFirstKeyPoint==0)
				start=pSelJg1->GetDatumPosBerByFaceOffset(pSelNode1,f3dPoint(0,0,1));
			else if(KeyPoint2Dlg.m_iFirstKeyPoint==1)
				start=pSelJg1->GetDatumPosWingXByFaceOffset(pSelNode1,f3dPoint(0,0,1));
			else
				start=pSelJg1->GetDatumPosWingYByFaceOffset(pSelNode1,f3dPoint(0,0,1));
		}
		if(KeyPoint2Dlg.m_iSecondOffsetStyle==0)	//ͶӰ
		{
			if(KeyPoint2Dlg.m_iSecondKeyPoint==0)
				end=pSelJg2->GetDatumPosBer(pSelNode2);
			else if(KeyPoint2Dlg.m_iSecondKeyPoint==1)
				end=pSelJg2->GetDatumPosWingX(pSelNode2);
			else
				end=pSelJg2->GetDatumPosWingY(pSelNode2);
		}
		else
		{
			if(KeyPoint2Dlg.m_iSecondKeyPoint==0)
				end=pSelJg2->GetDatumPosBerByFaceOffset(pSelNode2,f3dPoint(0,0,1));
			else if(KeyPoint2Dlg.m_iSecondKeyPoint==1)
				end=pSelJg2->GetDatumPosWingXByFaceOffset(pSelNode2,f3dPoint(0,0,1));
			else
				end=pSelJg2->GetDatumPosWingYByFaceOffset(pSelNode2,f3dPoint(0,0,1));
		}
		vec=end-start;
	}
#endif
	str.Format("L = %6.2fmm", vec.mod());
#ifdef AFX_TARG_ENU_ENGLISH
	MessageBox(str,"measure results");
#else
	MessageBox(str,"�������");
#endif
	return TRUE;
}
BOOL CLDSView::FinishCalLinePartExtendVec(CLDSDbObject *pTowerAtom)
{
	if(pTowerAtom==NULL)
		return FALSE;
	else if(pTowerAtom->GetClassTypeId()==CLS_NODE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("select wrong tower parts!");
#else
		AfxMessageBox("ѡ���˴����������!");
#endif
		return FALSE;
	}
	else if(!((CLDSPart*)pTowerAtom)->IsLinePart())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("select wrong tower parts!");
#else
		AfxMessageBox("ѡ���˴����������!");
#endif
		return FALSE;
	}
	/*
	CLDSLinePart *pLinePart=(CLDSLinePart*)pTowerAtom;
	f3dPoint Vector;
	Sub_Pnt(Vector,pLinePart->End(),pLinePart->Start());
	normalize(Vector);
	CReportVectorDlg dlg;
	dlg.m_fVectorX = Vector.x;
	dlg.m_fVectorY = Vector.y;
	dlg.m_fVectorZ = Vector.z;
	dlg.DoModal();
	*/
	CLinePartExtendVecDlg extend_vec_dlg;
	extend_vec_dlg.m_pLinePart=(CLDSLinePart*)pTowerAtom;
	extend_vec_dlg.DoModal();
	g_pSolidDraw->ReleaseSnapStatus();
	return TRUE;
}
BOOL CLDSView::FinishCalWorkFaceNorm(
		CLDSDbObject *pTowerAtom1, CLDSDbObject *pTowerAtom2)
{
	f3dPoint vec1,vec2,norm;
	if(pTowerAtom1==NULL||pTowerAtom2==NULL)
		return FALSE;
	if( !pTowerAtom1->IsPart()||!pTowerAtom2->IsPart())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("select wrong tower parts!");
#else
		AfxMessageBox("ѡ���˴����������!");
#endif
		return FALSE;
	}
	CLDSPart *pPart1=(CLDSPart*)pTowerAtom1;
	CLDSPart *pPart2=(CLDSPart*)pTowerAtom2;
	if( !pPart1->IsLinePart()||!pPart2->IsLinePart())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("select wrong tower parts!");
#else
		AfxMessageBox("ѡ���˴����������!");
#endif
		return FALSE;
	}
	CLDSLinePart *pLinePart1 = (CLDSLinePart*)pTowerAtom1;
	CLDSLinePart *pLinePart2 = (CLDSLinePart*)pTowerAtom2;
	BOOL bBySingleLine=TRUE;
#ifdef __ANGLE_PART_INC_
	//�����淨�߼����ָ�Ϊ���������㣬��Ϊ��ע������������ʱ����׼�߼������õķ��������
#ifdef AFX_TARG_ENU_ENGLISH
	if(MessageBox("based on actual angle outer line(Y) or base on line map(N)?",NULL,MB_YESNO)==IDYES)
#else
	if(MessageBox("�ԽǸ�ʵ��������Ϊ��׼(Y)�����Ե���ͼΪ��׼(N)?",NULL,MB_YESNO)==IDYES)
#endif
		bBySingleLine=FALSE;
#endif
	if(!bBySingleLine||pLinePart1->pStart==NULL||pLinePart1->pEnd==NULL||pLinePart2->pStart==NULL||pLinePart2->pEnd==NULL)
	{
		bBySingleLine=FALSE;
		Sub_Pnt(vec1,pLinePart1->End(),pLinePart1->Start());
		Sub_Pnt(vec2,pLinePart2->End(),pLinePart2->Start());
	}
	else
	{
		Sub_Pnt(vec1,pLinePart1->pEnd->ActivePosition(),pLinePart1->pStart->ActivePosition());
		Sub_Pnt(vec2,pLinePart2->pEnd->ActivePosition(),pLinePart2->pStart->ActivePosition());
	}
	normalize(vec1);
	normalize(vec2);
	if(fabs(vec1*vec2)>=EPS_COS2)
	{
		if(bBySingleLine)
			vec2=pLinePart2->pStart->ActivePosition()-pLinePart1->pStart->ActivePosition();
		else
			vec2=pLinePart2->Start()-pLinePart1->Start();
		normalize(vec2);
	}
	norm = cross_prod(vec1,vec2);
	normalize(norm);
	CReportVectorDlg dlg;
	dlg.m_fVectorX = norm.x;
	dlg.m_fVectorY = norm.y;
	dlg.m_fVectorZ = norm.z;
	dlg.DoModal();
	g_pSolidDraw->ReleaseSnapStatus();
	return TRUE;
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL CLDSView::ExtendJg(CLDSLineAngle *pSelJg, BOOL bExtendStart) 
{
	CLDSNode *pNode;
	double ll,dd;
	f3dPoint pnt,vec;
	CLDSLineAngle *pJg;
	CExtendJgDlg dlg;

	Sub_Pnt(vec,pSelJg->pEnd->ActivePosition(), pSelJg->pStart->ActivePosition());
	normalize(vec);
	if(dlg.DoModal()!=IDOK)
		return FALSE;
	if(dlg.m_nExtendStyle==0)//����
	{
		dd = fabs(vec.z);
		if(dd<EPS2)
			return FALSE;
		ll = dlg.m_fExtendDist*vec.mod()/dd;
		if(bExtendStart)//��˵�
		{
			Sub_Pnt(pnt,pSelJg->pStart->ActivePosition(), ll*vec);
			pNode = console.AppendNode();
			pJg = (CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
			pNode->SetPosition(pnt);
			pNode->m_cPosCalType = 7;//5.�ؽǸ���ĳ�ڵ��ƫ�Ƶ�
			pNode->arrRelationNode[0] = pSelJg->pStart->handle;
			pNode->arrRelationPole[0] = pSelJg->handle;
			pNode->attach_offset = -dlg.m_fExtendDist;
			pNode->hFatherPart = pJg->handle;
			pNode->SetLayer(pSelJg->layer());
			pNode->layer(1) = 'P';
			pNode->m_cPosCalType = 7;	//8.�ؽǸ���ĳ�ڵ�Ĵ�ֱƫ�Ƶ�
			pNode->m_cPosCalViceType='Z';
			pNode->cfgword = Ta.GetDefaultCfgPartNo();

			pJg->pStart = pSelJg->pStart;
			pJg->pEnd = pNode;
			pJg->SetLayer(pSelJg->layer());
			pJg->CopyProperty(pSelJg);
			//------vvvvvvvvv-------��˨���--------------------
			pJg->connectStart.d = pSelJg->connectStart.d;
			pJg->connectStart.rows = pSelJg->connectStart.rows;//ȱʡ������˨
			pJg->connectStart.wnConnBoltN = pSelJg->connectStart.wnConnBoltN;
			pJg->connectEnd.d = pSelJg->connectEnd.d;
			pJg->connectEnd.rows = pSelJg->connectEnd.rows;//ȱʡ������˨
			pJg->connectEnd.wnConnBoltN = pSelJg->connectEnd.wnConnBoltN;
			//------^^^^^^^^^-------��˨���--------------------
			pJg->set_norm_x_wing( pSelJg->get_norm_x_wing());
			pJg->set_norm_y_wing( pSelJg->get_norm_y_wing());
		}
		else//�ն˵�
		{
			Add_Pnt(pnt,pSelJg->pEnd->ActivePosition(), ll*vec);
			pNode = console.AppendNode();
			pJg = (CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
			pNode->SetPosition(pnt);
			pNode->layer(1) = 'P';
			pNode->m_cPosCalType = 7;//5.�ؽǸ���ĳ�ڵ��ƫ�Ƶ�
			pNode->m_cPosCalViceType='Z';	//8.�ؽǸ���ĳ�ڵ�Ĵ�ֱƫ�Ƶ�
			pNode->arrRelationNode[0] = pSelJg->pEnd->handle;
			pNode->arrRelationPole[0] = pSelJg->handle;
			pNode->attach_offset = dlg.m_fExtendDist;
			pNode->hFatherPart = pJg->handle;
			pNode->SetLayer(pSelJg->layer());
			pNode->cfgword = Ta.GetDefaultCfgPartNo();

			pJg->pStart = pSelJg->pEnd;
			pJg->pEnd = pNode;
			pJg->SetLayer(pSelJg->layer());
			pJg->CopyProperty(pSelJg);
			//------vvvvvvvvv-------��˨���--------------------
			pJg->connectStart.d = pSelJg->connectStart.d;
			pJg->connectStart.rows = pSelJg->connectStart.rows;//ȱʡ������˨
			pJg->connectStart.wnConnBoltN = pSelJg->connectStart.wnConnBoltN;
			pJg->connectEnd.d  = pSelJg->connectEnd.d;
			pJg->connectEnd.rows = pSelJg->connectEnd.rows;//ȱʡ������˨
			pJg->connectEnd.wnConnBoltN = pSelJg->connectEnd.wnConnBoltN;
			//------^^^^^^^^^-------��˨���--------------------
			pJg->set_norm_x_wing( pSelJg->get_norm_x_wing());
			pJg->set_norm_y_wing( pSelJg->get_norm_y_wing());
		}
	}
	else//����
	{
		if(bExtendStart)//��˵�
		{
			Sub_Pnt(pnt,pSelJg->pStart->ActivePosition(), dlg.m_fExtendDist*vec);
			pNode = console.AppendNode();
			pJg = (CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
			pNode->SetPosition(pnt);
			pNode->m_cPosCalType= 7;	//7.�ؽǸ���ĳ�ڵ��ƫ�Ƶ�
			pNode->arrRelationNode[0] = pSelJg->pStart->handle;
			pNode->arrRelationPole[0] = pSelJg->handle;
			pNode->attach_offset = -dlg.m_fExtendDist;
			pNode->hFatherPart = pJg->handle;
			pNode->SetLayer(pSelJg->layer());
			pNode->layer(1) = 'P';
			pNode->cfgword =  Ta.GetDefaultCfgPartNo();

			pJg->pStart = pSelJg->pStart;
			pJg->pEnd = pNode;
			pJg->SetLayer(pSelJg->layer());
			pJg->CopyProperty(pSelJg);
			//------vvvvvvvvv-------��˨���--------------------
			pJg->connectStart.d = pSelJg->connectStart.d;
			pJg->connectStart.rows = pSelJg->connectStart.rows;//ȱʡ������˨
			pJg->connectStart.wnConnBoltN = pSelJg->connectStart.wnConnBoltN;
			pJg->connectEnd.d = pSelJg->connectEnd.d;
			pJg->connectEnd.rows = 
				pSelJg->connectEnd.rows;//ȱʡ������˨
			pJg->connectEnd.wnConnBoltN = pSelJg->connectEnd.wnConnBoltN;
			//------^^^^^^^^^-------��˨���--------------------
			pJg->set_norm_x_wing(pSelJg->get_norm_x_wing());
			pJg->set_norm_y_wing(pSelJg->get_norm_y_wing());
		}
		else//�ն˵�
		{
			Add_Pnt(pnt,pSelJg->pEnd->ActivePosition(), dlg.m_fExtendDist*vec);
			pNode = console.AppendNode();
			pJg = (CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
			pNode->SetPosition(pnt);
			pNode->layer(1) = 'P';
			pNode->m_cPosCalType = 7;	//7.�ؽǸ���ĳ�ڵ��ƫ�Ƶ�
			pNode->arrRelationNode[0] = pSelJg->pEnd->handle;
			pNode->arrRelationPole[0] = pSelJg->handle;
			pNode->attach_offset = dlg.m_fExtendDist;
			pNode->hFatherPart = pJg->handle;
			pNode->SetLayer(pSelJg->layer());
			pNode->cfgword =  Ta.GetDefaultCfgPartNo();

			pJg->pStart = pSelJg->pEnd;
			pJg->pEnd = pNode;
			pJg->SetLayer(pSelJg->layer());
			pJg->CopyProperty(pSelJg);
			//------vvvvvvvvv-------��˨���--------------------
			pJg->connectStart.d = pSelJg->connectStart.d;
			pJg->connectStart.rows = pSelJg->connectStart.rows;//ȱʡ������˨
			pJg->connectStart.wnConnBoltN = pSelJg->connectStart.wnConnBoltN;
			pJg->connectEnd.d = pSelJg->connectEnd.d;
			pJg->connectEnd.rows = pSelJg->connectEnd.rows;//ȱʡ������˨
			pJg->connectEnd.wnConnBoltN = pSelJg->connectEnd.wnConnBoltN;
			//------^^^^^^^^^-------��˨���--------------------
			pJg->set_norm_x_wing( pSelJg->get_norm_x_wing());
			pJg->set_norm_y_wing( pSelJg->get_norm_y_wing());
		}
	}
	m_pDoc->SetModifiedFlag();
	return TRUE;
}
void CLDSView::FinishCal2PoleIntersPt(CLDSDbObject *pObj1,CLDSDbObject *pObj2)
{
	if(pObj1==NULL||pObj2==NULL)
		return;
	if(pObj1->GetClassTypeId()!=pObj2->GetClassTypeId())
		return;	//���˼����Ͳ�һ��ʱ���ܼ���˼�����
	if(!pObj1->IsPart()||!pObj2->IsPart())
		return;
	CLDSPart *pPart1=(CLDSPart*)pObj1;
	CLDSPart *pPart2=(CLDSPart*)pObj2;
	if(!pPart1->IsLinePart()||!pPart2->IsLinePart())
		return;
	CCalPoleIntersPtDlg IntJgWingDlg;
	IntJgWingDlg.m_pBasePole=(CLDSLinePart*)pObj1;
	IntJgWingDlg.m_pOtherPole=(CLDSLinePart*)pObj2;
	IntJgWingDlg.DoModal();
}
void CreateOffsetLsSimplified(BOLTSET &boltSet,double lenoffset,double wingOffset,int normOffset)
{	//����ƫ����˨
	Ta.BeginUndoListen();
	PARTSET datumPartSet;
	CSuperSmartPtr<CLDSPart>pDatumPart;
	CHashList<CLDSPart*>hashDatumPart;
	for(CLDSBolt *pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
	{
		CLDSPlate *pDatumPlate=NULL;
		CLDSLineAngle *pDatumLineAngle = (CLDSLineAngle*)console.FromPartHandle(pBolt->des_base_pos.hPart,CLS_LINEANGLE);
		CLDSBolt* pNewBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT,TRUE);
		pBolt->CloneTo(*pNewBolt);
		pNewBolt->relativeObjs.Empty();	//����ƫ����˨����չ�����ϵ�������޸�����˨���ʱԴ��˨������֮�仯 wht 17-02-23
		pNewBolt->des_base_pos.len_offset_dist+=lenoffset;
		pNewBolt->des_base_pos.wing_offset_dist+=wingOffset;
		//��Ϊ������˨λ�õ�֫��ƫ��ʱ����Ҫ��֫��ƫ�Ʒ�ʽ����Ϊ4.�Զ��壬������֫��ƫ������������ wxc 17-04-20
		if(fabs(wingOffset)>0)
			pNewBolt->des_base_pos.m_biWingOffsetStyle=4;
		if(normOffset!=0)
			pNewBolt->des_base_pos.norm_offset.AddThick(normOffset);
		pNewBolt->CalGuigeAuto();
		pNewBolt->correct_worknorm();
		pNewBolt->correct_pos();
		pNewBolt->SetModified();
		pNewBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pNewBolt->GetSolidPartObject());
		for(pDatumPart=Ta.Parts.GetFirst();pDatumPart.IsHasPtr();pDatumPart=Ta.Parts.GetNext())
		{
			if(pDatumPart->GetClassTypeId()==CLS_PLATE&&pDatumPart.PlatePointer()->FindLsByHandle(pBolt->handle))
			{	//����˨���뵽��׼��˨���ڸְ岢������Ƹְ�����
				pDatumPart.PlatePointer()->AppendLsRef(pNewBolt->GetLsRef());
				if(!hashDatumPart.GetValue(pDatumPart->handle))
				{
					hashDatumPart.SetValue(pDatumPart->handle,pDatumPart);
					datumPartSet.append(pDatumPart);
				}
			}
			else if(pDatumPart->GetClassTypeId()==CLS_PARAMPLATE&&pDatumPart.ParamPlatePointer()->FindLsByHandle(pBolt->handle))
			{	//����˨���뵽��׼��˨���ڸְ岢������Ƹְ�����
				pDatumPart.ParamPlatePointer()->AppendLsRef(pNewBolt->GetLsRef());
				if(!hashDatumPart.GetValue(pDatumPart->handle))
				{
					hashDatumPart.SetValue(pDatumPart->handle,pDatumPart);
					datumPartSet.append(pDatumPart);
				}
			}
			else if(pDatumPart->GetClassTypeId()==CLS_LINEANGLE&&pDatumPart.LineAnglePointer()->FindLsByHandle(pBolt->handle))
			{	//����˨���뵽��׼�Ǹ�
				if(pBolt->des_base_pos.datumPoint.datum_pos_style==1)
				{	//�Ǹ���㶨λ
					if(pNewBolt->des_base_pos.direction==0)
						pDatumPart.LineAnglePointer()->AppendStartLsRef(pNewBolt->GetLsRef());
					else if(pNewBolt->des_base_pos.direction==1)
						pDatumPart.LineAnglePointer()->AppendEndLsRef(pNewBolt->GetLsRef());
				} 
				else //�˼��ϵĽڵ㶨λ��Ǹ����߽��㶨λ
					pDatumPart.LineAnglePointer()->AppendMidLsRef(pNewBolt->GetLsRef());
			}
		}
	}
	for(pDatumPart=datumPartSet.GetFirst();pDatumPart.IsHasPtr();pDatumPart=datumPartSet.GetNext())
	{
		if(pDatumPart->GetClassTypeId()==CLS_PLATE)
		{
			pDatumPart.PlatePointer()->DesignPlate();
			pDatumPart.PlatePointer()->SetModified();
			pDatumPart.PlatePointer()->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pDatumPart.PlatePointer()->GetSolidPartObject());
		}
		else if(pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			pDatumPart.ParamPlatePointer()->DesignPlate();
			pDatumPart.ParamPlatePointer()->SetModified();
			pDatumPart.ParamPlatePointer()->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pDatumPart.ParamPlatePointer()->GetSolidPartObject());
		}
	}
	Ta.EndUndoListen();
	g_pSolidDraw->Draw();
}
static double bolt_easy_lenoffset=0;
static double bolt_easy_lenoffset_prev=0;
#include "MoveBoltDlg.h"
CMoveBoltDlg dlg;
//From DisplayPlateProperty.cpp
void DrawParamArrowLine(IDrawing *pDrawing,GEPOINT base_pos,GEPOINT line_vec,
						GEPOINT work_norm,const char* param_name=NULL,double offset_dist=0,
						double line_len=150,long hPart=0,UCS_STRU *pObjectCS=NULL);
void CLDSView::CopyBoltSimplified(BOLTSET* pBoltSet/*=NULL*/)
{
	CUndoOperObject undo(&Ta,true);
	CLDSBolt *pBolt;
	BOLTSET boltSet;
	if(pBoltSet==NULL)
	{
		long *id_arr;
		int i,nSelect=g_pSolidSnap->GetLastSelectEnts(id_arr);
		for(i=0;i<nSelect;i++)
		{
			pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[i],CLS_BOLT);
			if(pBolt==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("System only supports duplication of offset bolt currently!");
#else
				AfxMessageBox("ϵͳĿǰ��֧�ָ���ƫ����˨����!");
#endif
				return;
			}
			boltSet.append(pBolt);
		}
	}
	else
	{
		for(pBolt=pBoltSet->GetFirst();pBolt;pBolt=pBoltSet->GetNext())
			boltSet.append(pBolt);
	}
	bolt_easy_lenoffset=bolt_easy_lenoffset_prev=0;	//ÿ�θ��ƶ�����
	//ճ����˨
	int nMotherBolts=boltSet.GetNodeNum();
	CLDSBolt *pFirstBolt=NULL;
	GEPOINT line_vec,work_norm;
	GEPOINT base_pos,vWingOffsetVec;
	GEPOINT vFrontDirection=g_pSolidOper->TransVFromGL(GEPOINT(0,0,1));
	for(pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
	{
		if(pFirstBolt==NULL)
		{
			base_pos=pBolt->ucs.origin;
			work_norm=pBolt->ucs.axis_z;
			if(vFrontDirection*pBolt->ucs.axis_z<0)
				vFrontDirection=-pBolt->ucs.axis_z;
			else
				vFrontDirection=pBolt->ucs.axis_z;
			base_pos+=vFrontDirection*pBolt->get_L();
			CLDSLinePart* pRod=Ta.FromRodHandle(pBolt->des_base_pos.hPart);
			if(pRod==NULL)
				pRod=Ta.FromRodHandle(pBolt->des_base_pos.datumPoint.des_para.hPart);
			if(pRod)
			{
				line_vec=(pBolt->des_base_pos.direction==0)?(pRod->End()-pRod->Start()):(pRod->Start()-pRod->End());
				normalize(line_vec);
				//f3dPoint pos;
				//SnapPerp(&pos,pRod->Start(),pRod->End(),base_pos);
				//base_pos=pos;
				//base_pos+=work_norm*pRod->GetWidth()*0.5;
				if(pRod->IsAngle())
				{
					CLDSLineAngle* pAngle=(CLDSLineAngle*)pRod;
					if(fabs(pAngle->vxWingNorm*pBolt->ucs.axis_z)>fabs(pAngle->vyWingNorm*pBolt->ucs.axis_z))
						vWingOffsetVec=pAngle->GetWingVecX();
					else
						vWingOffsetVec=pAngle->GetWingVecY();
				}
			}
			pFirstBolt=pBolt;
		}
		if(bolt_easy_lenoffset==0)	//��Ҫ���ݵ�һ����˨�����жϳ������췽��ƫ����
		{
			LSSPACE_STRU LsSpace;
			GetLsSpace(LsSpace,pBolt->get_d());
			CLDSPoint& datumPoint=pBolt->des_base_pos.datumPoint;
			CLDSLinePart* pRod=Ta.FromRodHandle(pBolt->des_base_pos.hPart);
			if(pRod==NULL)
				pRod=Ta.FromRodHandle(pBolt->des_base_pos.datumPoint.des_para.hPart);
			JGZJ jgzj;
			getjgzj(jgzj,pRod?pRod->GetWidth():0);
			if( jgzj.g1>0&&	//����֧��˫��׼��ʱ����Ĭ��˫����˨���
				(datumPoint.datum_pos_style==1&&datumPoint.des_para.RODEND.wing_offset_style>0&&datumPoint.des_para.RODEND.wing_offset_style<4)||
				(datumPoint.datum_pos_style==2&&datumPoint.des_para.RODNODE.wing_offset_style>0&&datumPoint.des_para.RODEND.wing_offset_style<4)||
				(datumPoint.datum_pos_style==3&&datumPoint.des_para.AXIS_INTERS.wing_offset_style1>0&&datumPoint.des_para.RODEND.wing_offset_style<4))
				bolt_easy_lenoffset=LsSpace.doubleRowSpace;
			else
				bolt_easy_lenoffset=LsSpace.SingleRowSpace;
			if(pBolt->des_base_pos.len_offset_dist<0)
				bolt_easy_lenoffset*=-1;	//����ƫ��
		}
	}
	//IDrawing *pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	bool sketchDisplayed=false;
	if(pFirstBolt&&!line_vec.IsZero())
	{
		btc::SKETCH_PLANE sketch(base_pos,line_vec,vFrontDirection);
		g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),sketch.pVertexArr,sketch.VertexCount,sketch.normal,"��L");
		if(!vWingOffsetVec.IsZero())
			g_pSolidDraw->NewWorkPlaneSketch(2,RGB(152,152,152),&base_pos,1,vWingOffsetVec,"��G",75);
		//g_pSolidDraw->NewWorkPlaneSketch(3,RGB(152,152,152),&base_pos,1,work_norm,"��N",75);
		//DrawParamArrowLine(pBuddyDrawing,base_pos,line_vec,work_norm,NULL,0,250,pFirstBolt?pFirstBolt->handle:0);
		sketchDisplayed=true;
		g_pSolidDraw->Draw();
	}
	while(true)
	{
		dlg.m_fLenOffsetDist=bolt_easy_lenoffset;
		if(dlg.DoModal()!=IDOK)
			break;
		bolt_easy_lenoffset=dlg.m_fLenOffsetDist;
		CreateOffsetLsSimplified(boltSet,bolt_easy_lenoffset,dlg.m_fWingOffsetDist,dlg.m_nNormOffset);
		double temp=bolt_easy_lenoffset;
		bolt_easy_lenoffset+=bolt_easy_lenoffset-bolt_easy_lenoffset_prev;
		bolt_easy_lenoffset_prev=temp;
	}
	if(sketchDisplayed)
	{
		g_pSolidDraw->DelWorkPlaneSketch(1);
		if(!vWingOffsetVec.IsZero())
			g_pSolidDraw->DelWorkPlaneSketch(2);
		//g_pSolidDraw->DelWorkPlaneSketch(2);
		g_pSolidDraw->Draw();
	}
}
void CLDSView::CopyNodes(NODESET& nodeset)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CUndoOperObject undo(&Ta);
	CLDSNode* pNode=NULL,*pFirstNode=nodeset.GetFirst();
	BYTE ciPosCalType=pFirstNode->m_cPosCalType;
	for(pNode=nodeset.GetNext();pNode;pNode=nodeset.GetNext())
	{
		if(pNode->m_cPosCalType==ciPosCalType)
			continue;
		ciPosCalType=0;
		break;
	}
	if(ciPosCalType==CLDSNode::COORD_OFFSET)
	{	//ƫ�Ƶ�
		CString cmdStr;
		char cOffsetDatumType='P';
		pCmdLine->FillCmdLine("Copy->Nodes �¸���ƫ�ƽڵ��ƫ�ƻ�׼Ϊ[��һ�ڵ�(P)��ԭʼƫ�ƻ�׼�ڵ�(O)] <P>:","");
		if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"P|O"))
			return;
		if(cmdStr.GetLength()>0)
			cOffsetDatumType=toupper(cmdStr[0]);
		if(cOffsetDatumType!='P'&&cOffsetDatumType!='O')
			cOffsetDatumType='P';
		double offset,prevOffset,originalOffset,step=100;
		offset=originalOffset=pFirstNode->attach_offset;	//��ʼƫ����
		if(offset<0)
			step*=-1;
		prevOffset=offset;
		while(true)
		{
			if(cOffsetDatumType=='P')
				step=offset;
			else
				offset=prevOffset+step;
			pCmdLine->FillCmdLine((char*)CXhChar100("Copy->Nodes ָ���½ڵ������һ�ڵ��ƫ�Ʋ���, ESC��ֹ���� <%g>:",step),"");
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				return;
			if(cmdStr.GetLength()>0)
				step=atof(cmdStr);
			if(cOffsetDatumType=='P')
				offset=step;
			else
				offset=prevOffset+step;
			NODESET currset;
			for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
			{
				CLDSNode* pNewNode=Ta.AppendNode();
				if(cOffsetDatumType=='P')
				{	//��ǰһ�ڵ�Ϊ��׼����ƫ��
					pNewNode->arrRelationNode[0]=pNode->handle;
					currset.append(pNewNode);
				}
				else //if(cOffsetDatumType=='O')
					pNewNode->arrRelationNode[0]=pNode->arrRelationNode[0];
				pNewNode->CopyProperty(pNode);
				pNewNode->SetPosition(pNode->Position(false));
				pNewNode->hFatherPart=pNode->hFatherPart;
				pNewNode->arrRelationNode[1]=pNode->arrRelationNode[1];
				pNewNode->arrRelationNode[2]=pNode->arrRelationNode[2];
				pNewNode->attach_offset=offset;
				pNewNode->CalPosition(true);
				NewNode(pNewNode);
			}
			if(cOffsetDatumType=='O')
			{
				step=offset-prevOffset;
				prevOffset=offset;
			}
			if(currset.GetNodeNum()>0)
			{
				nodeset.Empty();
				for(pNode=currset.GetFirst();pNode;pNode=currset.GetNext())
					nodeset.append(pNode);
			}
			g_pSolidDraw->Draw();
		}
	}
	else if(ciPosCalType>=CLDSNode::COORD_X_SPECIFIED&&ciPosCalType<=CLDSNode::COORD_Z_SPECIFIED)
	{	//ָ��X��������ڵ�
		CString cmdStr;
		double step=1000;
		while(true)
		{
			pCmdLine->FillCmdLine((char*)CXhChar100("Copy->Nodes ָ���½ڵ����ǰһ�ڵ����������ֵ, ESC��ֹ���� <%g>",step),"");
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				return;
			if(cmdStr.GetLength()>0)
				step=atof(cmdStr);
			NODESET currset;
			for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
			{
				CLDSNode* pNewNode=Ta.AppendNode();
				pNewNode->CopyProperty(pNode);
				pNewNode->hFatherPart=pNode->hFatherPart;
				pNewNode->arrRelationNode[0]=pNode->arrRelationNode[0];
				pNewNode->arrRelationNode[1]=pNode->arrRelationNode[1];
				pNewNode->SetPosition(pNode->Position(false));
				if(pFirstNode->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED)
				{
					double coord=pNode->Position('X',false)+step;	//��ʼƫ����
					pNewNode->SetPositionX(coord);
				}
				else if(pFirstNode->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED)
				{
					double coord=pNode->Position('Y',false)+step;	//��ʼƫ����
					pNewNode->SetPositionY(coord);
				}
				else if(pFirstNode->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)
				{
					double coord=pNode->Position('Z',false)+step;	//��ʼƫ����
					pNewNode->SetPositionZ(coord);
				}
				pNewNode->hFatherPart=pNode->hFatherPart;
				pNewNode->CalPosition(true);
				currset.append(pNewNode);
				NewNode(pNewNode);
			}
			nodeset.Empty();
			for(pNode=currset.GetFirst();pNode;pNode=currset.GetNext())
				nodeset.append(pNode);
			g_pSolidDraw->Draw();
		}
	}
	else
	{	//���������κ������ڵ���и���
		CString cmdStr;
		char cOffsetDatumType='P';
		GEPOINT step(100,0,0);
		while(true)
		{
			pCmdLine->FillCmdLine((char*)CXhChar100("Copy->Nodes ָ���½ڵ������һ�ڵ��ƫ�Ʋ���, ESC��ֹ���� <��x=%g,��y=%g,��z=%g>:",step.x,step.y,step.z),"");
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				return;
			CXhChar100 coordstr=cmdStr;
			char* coord=strtok(coordstr,",");
			step.x=(coord!=NULL)?atof(coord):step.x;
			coord=strtok(NULL,",");
			step.y=(coord!=NULL)?atof(coord):step.y;
			coord=strtok(NULL,",");
			step.z=(coord!=NULL)?atof(coord):step.z;
			NODESET currset;
			for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
			{
				CLDSNode* pNewNode=Ta.AppendNode();
				//��ǰһ�ڵ�Ϊ��׼����ƫ��
				pNewNode->CopyProperty(pNode);
				GEPOINT pos=pNode->Position(false)+step;
				pNode->m_cPosCalType=0;	//�������κ������ڵ�
				pNewNode->SetPosition(pos);
				currset.append(pNewNode);
				NewNode(pNewNode);
			}
			nodeset.Empty();
			for(pNode=currset.GetFirst();pNode;pNode=currset.GetNext())
				nodeset.append(pNode);
			g_pSolidDraw->Draw();
		}
	}
}
#ifdef __COMMON_PART_INC_
void ShadowCreate(ATOM_LIST<int> &idList,CParaCS::PARAMETRIC_INFO param,int iShadowCreateStyle,int nOffsetNum,long pRelationHandle);
#endif
static f3dPoint plate_offset_vector;
static f3dPoint plate_offset_vector_prev;
bool CLDSView::CopySingularPlate(CLDSPlate* pSrcPlate,CLDSNode* pNode)
{
	CModifiedPartsOperObject updatesolid;
	CClonePlate clonePlate;
	clonePlate.SetSuperiorTower(&Ta);
	clonePlate.AnalyzePlateInfo(pSrcPlate);
	//�л���ʵ����ʾģʽ,ѡ��ճ�����ɵĸְ�
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	CUndoOperObject undo(&Ta);
	int iMatchType=clonePlate.IntelligentMatchPlate(pNode,console.DispPartSet);
	if(iMatchType<=0)
		return false;
	CLDSPlate *pPlate=clonePlate.CreatePlate(console.DispPartSet);
	if(pPlate==NULL)
		return false;
	if(iMatchType==2||iMatchType==3)
	{				
		CCorrectPlateParaDlg correctParaDlg;
		correctParaDlg.m_nPlateThick=(int)pPlate->GetThick();
		correctParaDlg.m_sPlatePartNo=pPlate->GetPartNo();
		correctParaDlg.m_sPlateSegI=pPlate->iSeg.ToString();
		correctParaDlg.m_iProfileStyle=pPlate->designInfo.iProfileStyle0123;
		if(iMatchType==2)	//���ƥ��
			correctParaDlg.m_bGuiGeMatch=TRUE;
		int nOldThick=(int)pPlate->GetThick();	//��¼�������֮��ĺ��
		if(correctParaDlg.DoModal()==IDOK)
		{
			//���¸ְ������Ϣ
			pPlate->designInfo.iProfileStyle0123=correctParaDlg.m_iProfileStyle;
			pPlate->iSeg=SEGI(correctParaDlg.m_sPlateSegI.GetBuffer());
			pPlate->Thick=correctParaDlg.m_nPlateThick;
			pPlate->SetPartNo(correctParaDlg.m_sPlatePartNo.GetBuffer(0));
			//�޸ĸְ��Ⱥ�Ӧ���Զ��������߸˼�ƫ����
			//if(correctParaDlg.m_nPlateThick!=nOldThick)
				clonePlate.UpdateRayLinePartOffset(pPlate,g_sysPara.nAutoAdjustTubePos); 
			//�����ְ���Ʋ���
			if(correctParaDlg.m_bCorrectAngleProfilePara)
				clonePlate.CorrectPlateDesignPara(pPlate);
			//�Զ�������˨�����˨���
			if(correctParaDlg.m_bCorrectLsInfo)
				clonePlate.CorrectPlateLsInfo(pPlate);
		}
		else
		{
			console.DeletePart(pPlate->handle);
			return false;
		}
	}
	pPlate->DesignPlate();
	pPlate->SetModified();
	pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	CLsRef *pLsRef=NULL;
	for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{	//��λ��˨����ʾ
		(*pLsRef)->correct_worknorm();
		(*pLsRef)->correct_pos();	
		(*pLsRef)->CalGuigeAuto();
		(*pLsRef)->SetModified();
		(*pLsRef)->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
	}
	//��ʾ�ֹܲ��
	CDesignLjPartPara *pLjPartPara=NULL;
	for(pLjPartPara=pPlate->GetFirstLjPara();pLjPartPara;pLjPartPara=pPlate->GetNextLjPara())
	{	
		if(pLjPartPara->pCurLinePart==NULL)
			pLjPartPara->pCurLinePart=(CLDSLineTube*)console.FromPartHandle(pLjPartPara->hPart,CLS_LINEPART);
		if(pLjPartPara->pCurLinePart&&pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINETUBE)
		{
			CLDSLineTube *pCurLineTube=(CLDSLineTube*)pLjPartPara->pCurLinePart;
			pCurLineTube->ClearFlag();
			pCurLineTube->CalPosition();
			CLDSParamPlate *pParamPlate=NULL;
			if(pLjPartPara->start0_end1==0)
			{
				pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pLjPartPara->hPart,CLS_PARAMPLATE);
				pCurLineTube->DesignStartJoint();
			}
			else if(pLjPartPara->start0_end1==1)
			{
				pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pLjPartPara->hPart,CLS_PARAMPLATE);
				pCurLineTube->DesignEndJoint();
			}
			if(pParamPlate)
			{
				pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pParamPlate->GetSolidPartObject());
			}
			pCurLineTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pCurLineTube->GetSolidPartObject());
		}
	}
	g_pSolidDraw->Draw();
	return true;
}
#include "SnapTypeVerify.h"
void CLDSView::CopyOrdinaryParts(PARTSET& partset)
{
	CSuperSmartPtr<CLDSPart> pPart,pNewPart;
	CLDSPart *pFirstPart=partset.GetFirst();
	CUndoOperObject undo(&Ta,true);
	CString cmdStr;
	char cReuseMode='C';
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(pFirstPart->GetClassTypeId()==CLS_PLATE&&partset.GetNodeNum()==1)
	{
		pCmdLine->FillCmdLine("ָ��Ӧ�øְ����ģʽ��Ŀ��ڵ㣬��ָ������ģʽ[����(C)��Ӱ��(S)�����(M)] <C>:","");
		CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_POINT);
		DWORD dwhObj=0,dwExportFlag=0;
		PEEKCMDLINE cmd_detect("C|S|M",pCmdLine);
		int state,disableElseMode=false;;
		CDisplayNodeAtFrontLife displayNode;
		displayNode.DisplayNodeAtFront();
		while(true)
		{
			if((state=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier,PEEKCMDLINE::MatchKeyword,&cmd_detect))<0)
			{
				pCmdLine->CancelCmdLine();
				return ;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			if(!disableElseMode&&state==0&&obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
				break;
			if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
			{
				pCmdLine->PeekStrFromCmdLine(cmdStr);
				break;
			}
			dwhObj=obj.hRelaObj;
			CLDSNode* pNode=Ta.Node.FromHandle(obj.hRelaObj);
			if(pNode==NULL)
				continue;
			disableElseMode=true;
			bool bRetCode=CopySingularPlate((CLDSPlate*)pFirstPart,pNode);
			if(bRetCode)
				pCmdLine->FinishCmdLine(CXhChar16("�ڵ�0x%X",pNode->handle));
			else
				pCmdLine->FinishCmdLine(CXhChar16("�ڵ�0x%X���ְ����ʧ��",pNode->handle));
			pCmdLine->FillCmdLine("ָ��Ӧ�øְ����ģʽ��Ŀ��ڵ�:","");
		}
	}
	else
	{
		pCmdLine->FillCmdLine("ָ���������õ�ģʽ[����(C)��Ӱ��(S)] <C>:","");	//��ʱ������<���(M)>ѡ��
		if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"C|S|M"))
			return;
	}
	if(cmdStr.GetLength()>0)
		cReuseMode=toupper(cmdStr[0]);
	if(cReuseMode!='C'&&cReuseMode!='S'&&cReuseMode!='M')
		cReuseMode='C';
	plate_offset_vector.Set();
	plate_offset_vector_prev.Set();
	GEPOINT vOffsetStep;
	if(cReuseMode=='C')
	{
#ifdef __COMMON_PART_INC_
		int normal_offset=max(6,(int)pFirstPart->Thick);
		CLDSParamPlate* pParamPlate=NULL;
		if(pFirstPart->GetClassTypeId()==CLS_PARAMPLATE)
			pParamPlate=(CLDSParamPlate*)pFirstPart;
		if(pParamPlate!=NULL)
		{
			if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE||pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
			{	//�����߰�
				pCmdLine->FillCmdLine("���뻷����ػ�׼�ֹܸ��Ƶ�ƫ����:","");
			}
		}
		pCmdLine->FillCmdLine((char*)CXhChar100("ָ���������߷����ƫ����,���X/Y/Z���귽���ƫ��ֵ[�ֲ�����(A)��ȫ������(G)] <%d>:",normal_offset),"");
		if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"A|G"))
			return;
		char cOffsetMode=0;
		if(cmdStr.GetLength()>0&&toupper(cmdStr[0])=='A')
			cOffsetMode='A';	//���ֲ�����ƫ��
		else if(cmdStr.GetLength()>0&&toupper(cmdStr[0])=='G')
			cOffsetMode='G';	//��ȫ������ƫ��
		else if(cmdStr.GetLength()>0)
			normal_offset=atoi(cmdStr);
		if(cOffsetMode=='A'||cOffsetMode=='G')
		{
			pCmdLine->FillCmdLine("ָ��������X/Y/Z���������᷽���ƫ���� <0,0,0>:","");
			if(pCmdLine->GetStrFromCmdLine(cmdStr))
				return;
			CXhChar100 coordstr=cmdStr;
			char* coord=strtok(coordstr,",");
			vOffsetStep.x=(coord!=NULL)?atof(coord):vOffsetStep.x;
			coord=strtok(NULL,",");
			vOffsetStep.y=(coord!=NULL)?atof(coord):vOffsetStep.y;
			coord=strtok(NULL,",");
			vOffsetStep.z=(coord!=NULL)?atof(coord):vOffsetStep.z;
		}
		for(pPart=partset.GetFirst();pPart.IsHasPtr();pPart=partset.GetNext())
		{
			//if(!pPart->IsPlate())
			if(pPart->GetClassTypeId()!=CLS_PLATE)
				continue;
			if(cOffsetMode=='A')
				pNewPart=pPart.pPlate->CloneCopy(vOffsetStep,true);
			else if(cOffsetMode=='G')
				pNewPart=pPart.pPlate->CloneCopy(vOffsetStep,false);
			else
				pNewPart=pPart.pPlate->CloneCopy(normal_offset,NULL);
			if(pNewPart.IsNULL())
				continue;
			pNewPart->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pNewPart->GetSolidPartObject());
			//��ΰ˵���Ÿ��Ƶĸְ���ò�Ҫ����˨��������˨�ȵ������༭�����Ƴ���˨���򵥡�wjh-2016.12.16
			pNewPart->GetLsRefList()->Empty();
			/*���´�������ǰ�ɴ��뽫��Ӧ�û���ֲ�������ط�����ֱ��ȥ����wjh-2016.9.8
			CLDSPlate *pPlate=pPart.pPlate;
			if(!plate_offset_vector.IsZero())
			{
				f3dPoint temp_pos=plate_offset_vector;
				plate_offset_vector+=(plate_offset_vector-plate_offset_vector_prev);
				plate_offset_vector_prev=temp_pos;
			}
			else
				plate_offset_vector.Set(500,0,0);
			CParaCS::PARAMETRIC_INFO param;
			//ͨ���Ի����ʼ��������ƫ������ڵ�ľ�������
			CDefineOffsetPosDlg defOffsetPosDlg;
			defOffsetPosDlg.offset_vector=plate_offset_vector;
			defOffsetPosDlg.datum_pos=pPlate->ucs.origin;
			defOffsetPosDlg.m_bDirectDefPos=true;
			if(defOffsetPosDlg.DoModal()!=IDOK)
				return;
			plate_offset_vector=defOffsetPosDlg.offset_vector;
			param.m_xMirInfo.origin.x=LENGTH(defOffsetPosDlg.offset_vector);
			normalize(defOffsetPosDlg.offset_vector);
			param.desAxisX.vector=defOffsetPosDlg.offset_vector;
			param.m_cDefStyle=CParaCS::CS_TRANSLATE;
			ATOM_LIST<int> idList;
			idList.append(pPlate->handle);
			ShadowCreate(idList,param,1,1,0);
//#else
//#ifdef AFX_TARG_ENU_ENGLISH
//			pCmdLine->PromptStrInCmdLine("Please select the node to be pasted the position");
//#else
//			pCmdLine->PromptStrInCmdLine("��ѡ����Ҫճ��λ�õĽڵ�");
//#endif
			*/
		}
#endif
	}
	else if(cReuseMode=='S')
	{
		int state;
		char cShadowType='E';
		DWORD dwhObj,dwExportFlag;
		SmartRodPtr pDatumRod;
		pCmdLine->FillCmdLine("ѡ��һ���˼���Ϊ���Ƶ�ƫ�Ʒ��򣬻�[X��Գ�(X)/Y��Գ�(Y)/Z��Գ�(Z)/Z����ת(R)/����(E)]:","");
		PEEKCMDLINE cmd_detect("X|Y|Z|R|E",pCmdLine);
		CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_LINE);
		verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART);
		while(true)
		{
			if((state=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier,PEEKCMDLINE::MatchKeyword,&cmd_detect))<0)
			{
				pCmdLine->CancelCmdLine();
				return ;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
			{
				if(pCmdLine->PeekStrFromCmdLine(cmdStr))
					cShadowType=toupper(cmdStr[0]);
				break;
			}
			dwhObj=obj.hRelaObj;
			pDatumRod=dwhObj>0?Ta.FromRodHandle(dwhObj):NULL;
			if(pDatumRod.IsHasPtr())
				break;
		}
		if(pDatumRod.IsHasPtr())
		{	//ѡ����ƫ�Ʒ���Ļ�׼�˼�
			g_pSolidDraw->SetEntSnapStatus(pDatumRod->handle);
			GEPOINT vLenOffsetVec=pDatumRod->End()-pDatumRod->Start();
			Standarized(vLenOffsetVec);
			btc::SKETCH_PLANE sketch(pFirstPart->ucs.origin,vLenOffsetVec,NULL,300);
			g_pSolidDraw->NewWorkPlaneSketch(1,RGB(255,0,0),sketch.pVertexArr,sketch.VertexCount,sketch.normal,"");
			g_pSolidDraw->Draw();
			CParaCS::PARAMETRIC_INFO param;
			param.m_cDefStyle=CParaCS::CS_TRANSLATE;
			param.desAxisX.norm_style=5;
			param.desAxisX.hVicePart=pDatumRod->handle;
			param.desAxisX.direction=0;	//Ĭ��ʼ->��
			double steplength=100;
			while(true){
				pCmdLine->FillCmdLine((char*)CXhChar100("����Ӱ�临�ƶ����ƫ�Ʋ���, ��ESC��ֹ���� <%g>:",steplength),"");
				if(!pCmdLine->GetStrFromCmdLine(cmdStr))
					return;
				if(cmdStr.GetLength()>0)
					steplength=atof(cmdStr);
				param.m_xMirInfo.origin.x+=steplength;
				CLDSPart *pShadowPart=pFirstPart->NewCopy(true);
				CParaCS *pAssembleCS=pShadowPart->GetParaCS();
				pAssembleCS->SetParamInfo(param);
				console.DispPartSet.append(pShadowPart);
				pShadowPart->ShadowDesign();
				pShadowPart->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pShadowPart->GetSolidPartObject());
				g_pSolidDraw->Draw();
			}
		}
		else if(cShadowType>='X'&&cShadowType<='Z' || cShadowType=='R')
		{
			CLDSPart *pShadowPart;
			CParaCS::PARAMETRIC_INFO param;
			if(cShadowType=='R')
			{
				param.m_cDefStyle=CParaCS::CS_MIRTRANS;
				for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
					pShadowPart=pPart->NewCopy(true);
			}
			else
			{
				param.m_cDefStyle=CParaCS::CS_TRANSLATE;
				GEPOINT vLenOffsetVec(1,0,0);
				if(cShadowType=='Y')
					vLenOffsetVec.Set(0,1,0);
				else if(cShadowType=='Z')
					vLenOffsetVec.Set(0,0,1);
				btc::SKETCH_PLANE sketch(pFirstPart->ucs.origin,vLenOffsetVec,NULL,300);
				g_pSolidDraw->NewWorkPlaneSketch(1,RGB(255,0,0),sketch.pVertexArr,sketch.VertexCount,sketch.normal,"");
				g_pSolidDraw->Draw();
				param.desAxisX.norm_style=0;
				param.desAxisX.vector=vLenOffsetVec;
				double steplength=100;
				while(true){
					pCmdLine->FillCmdLine((char*)CXhChar100("����Ӱ�临�ƶ����ƫ�Ʋ���, ��ESC��ֹ���� <%g>:",steplength),"");
					if(!pCmdLine->GetStrFromCmdLine(cmdStr))
						return;
					if(cmdStr.GetLength()>0)
						steplength=atof(cmdStr);
					param.m_xMirInfo.origin.x+=steplength;
					for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
					{
						pShadowPart=pPart->NewCopy(true);
						CParaCS *pAssembleCS=pShadowPart->GetParaCS();
						pAssembleCS->SetParamInfo(param);
						console.DispPartSet.append(pShadowPart);
						pShadowPart->ShadowDesign();
						pShadowPart->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pShadowPart->GetSolidPartObject());
					}
				}
			}
			g_pSolidDraw->Draw();
		}
		else
			OnShadowCreate();
	}
	else if(cReuseMode=='M')
	{
		//pCmdLine->FillCmdLine("ָ��ѡ�й�����Ҫ���õ��ĺ��߽��Ⱥ�:","");
		//if(!pCmdLine->GetStrFromCmdLine(cmdStr))
		//	return;
		CCfgPartNoDlg cfgdlg;
		cfgdlg.cfgword = pFirstPart->cfgword; 
		if(pFirstPart->IsBodyObj())
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if(cfgdlg.DoModal()==IDOK)
		{
			for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
				pPart->cfgword = cfgdlg.cfgword;		//�ڴ˴����µ�һ���ڵ������Ϻ�
		}
	}
}
//���ɽڵ��ģ��
void CLDSView::OnExportConnectionTemplate()
{
	ExportConnecTempData();
}
int CLDSView::ExportConnectionTemplate()
{
	return ExportConnecTempData();
}
void CLDSView::OnCopyToClipboard()
{
	ExportConnecTempData(1);
}
int CLDSView::ExportConnecTempData(BYTE ciMode/*=0*/)
{
	if(!CLDSPart::IsLaboratoryFuncEnabled())//PRODUCT_FUNC::IsHasInternalTest())
		return 0;
	CCmdLockObject cmdLock(this);
	if(!cmdLock.LockSuccessed())
		return FALSE;
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//��׽�ڵ��
	CLDSPlate* pNodePlate=NULL;
	long *id_arr=NULL;
	int count=g_pSolidSnap->GetLastSelectEnts(id_arr);
	for(int i=0;i<count;i++)
	{
		if((pNodePlate=(CLDSPlate*)Ta.Parts.FromHandle(id_arr[i],CLS_PLATE))!=NULL)
			break;
	}
	if(pNodePlate==NULL)
	{
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("ConnectTemplate please choose node plate:","");
#else
		pCmdLine->FillCmdLine("ConnectTemplate ��ѡ��ڵ��:","");
#endif
		DWORD dwhObj=0,dwExport=0;
		CSnapTypeVerify verify(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE));
		while(1)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExport,&verify)<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExport);
			pNodePlate=(CLDSPlate*)console.FromPartHandle(obj.hRelaObj,CLS_PLATE);
			if(pNodePlate)
				break;
		}
		g_pSolidDraw->SetEntSnapStatus(pNodePlate->handle);
		pCmdLine->FinishCmdLine(CXhChar16("0X%X",pNodePlate->handle));
	}
	//��ȡ�ڵ��ģ�����Ʋ���
	CDeepClonePlate xClonePlate(&Ta);
	xClonePlate.ExtractPlateInfo(pNodePlate);
	if(ciMode==0)
	{	//�������ļ���
		CXhChar50 templfilename("temp.bt");
		if(strlen(pNodePlate->Label)>1)
			templfilename.Printf("%s.bt",pNodePlate->Label);
		CFileDialog file_dlg(FALSE,"bt",templfilename,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"�ڵ��ģ���ļ�(*.bt)|*.bt|�����ļ�(*.*)|*.*||");
		if(file_dlg.DoModal()==IDOK)
			xClonePlate.ToXmlSchemaFile(file_dlg.GetPathName());
	}
	else if(ciMode==1)
	{	//���������а���
		xClonePlate.WriteToClipboard();
	}
	//	
	g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("����:","");
#endif
	return TRUE;
}
//�����ڵ��
void CLDSView::OnConnectFromTemplate()
{
	ConnectFromTemplate();
}
int CLDSView::ConnectFromTemplate()
{
	if(!CLDSPart::IsLaboratoryFuncEnabled())//PRODUCT_FUNC::IsHasInternalTest())
		return 0;
	CCmdLockObject cmdLock(this);
	if(!cmdLock.LockSuccessed())
		return FALSE;
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//ѡ����ƽڵ�
	NODESET nodeSet;
	CLDSNode* pDesNode=NULL;
	g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("DesignConnect please choose the design node:","");
#else
	pCmdLine->FillCmdLine("DesignConnect ��ѡ��Ҫ��ƵĽڵ�:","");
#endif
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verify(OBJPROVIDER::DRAWINGSPACE,GetSingleWord(SELECTINDEX_NODE));
	verify.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
	{
		CDisplayNodeAtFrontLife displayNode;
		displayNode.DisplayNodeAtFront();
		while(1)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verify)<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			pDesNode=console.FromNodeHandle(obj.hRelaObj);
			if(pDesNode)
				break;
		}
	}
	pCmdLine->FinishCmdLine(CXhChar16("0X%X",pDesNode->handle));
	//��ƽڵ��(���ȴӼ��а��л�ȡģ������)
	CUndoOperObject undo(&Ta,true);
	CDeepClonePlate xClonePlate(&Ta);
	if(xClonePlate.ReadFromClipboard()==FALSE)
	{	//����ڵ��ģ���ļ�
		CString templfile;
		CFileDialog dlg(TRUE,"bt","template.bt",
#ifdef AFX_TARG_ENU_ENGLISH
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"tower design template file(*.ft)|*.ft|all files(*.*)|*.*||");
#else
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"�ڵ��ģ���ļ�(*.bt)|*.bt|�����ļ�(*.*)|*.*||");
#endif
		if(dlg.DoModal()!=IDOK)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		templfile=dlg.GetPathName();
		xClonePlate.FromXmlSchemaFile(templfile);
	}
	//��ƽڵ��
	int iMatchType=xClonePlate.IntelligentMatchPlate(pDesNode,console.DispPartSet);
	if(iMatchType<=0)
		return FALSE;
	CModifiedPartsOperObject updatesolid;
	CLDSPlate *pPlate=xClonePlate.CreatePlate(console.DispPartSet);
	if(pPlate==NULL)
		return FALSE;
	if(iMatchType==2||iMatchType==3)
	{				
		CCorrectPlateParaDlg correctParaDlg;
		correctParaDlg.m_nPlateThick=(int)pPlate->GetThick();
		correctParaDlg.m_sPlatePartNo=pPlate->GetPartNo();
		correctParaDlg.m_sPlateSegI=pPlate->iSeg.ToString();
		correctParaDlg.m_iProfileStyle=pPlate->designInfo.iProfileStyle0123;
		if(iMatchType==2)	//���ƥ��
			correctParaDlg.m_bGuiGeMatch=TRUE;
		if(correctParaDlg.DoModal()==IDOK)
		{
			//���¸ְ������Ϣ
			pPlate->designInfo.iProfileStyle0123=correctParaDlg.m_iProfileStyle;
			pPlate->iSeg=SEGI(correctParaDlg.m_sPlateSegI.GetBuffer());
			pPlate->Thick=correctParaDlg.m_nPlateThick;
			pPlate->SetPartNo(correctParaDlg.m_sPlatePartNo.GetBuffer(0));
			//�޸ĸְ��Ⱥ�Ӧ���Զ��������߸˼�ƫ����
			xClonePlate.UpdateRayLinePartOffset(pPlate,g_sysPara.nAutoAdjustTubePos); 
			if(correctParaDlg.m_bCorrectAngleProfilePara)	//�����ְ���Ʋ���
				xClonePlate.CorrectPlateDesignPara(pPlate);
			if(correctParaDlg.m_bCorrectLsInfo)	//�Զ�������˨�����˨���
				xClonePlate.CorrectPlateLsInfo(pPlate);
		}
	}
	pPlate->DesignPlate();
	pPlate->SetModified();
	pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{	//��λ��˨����ʾ
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		PARTSET partSet;
		FindBoltRelatedParts(pBolt,partSet);	
		pBolt->des_base_pos.norm_offset.key_str=CLDSBolt::EmendZOffsetStr(pBolt,partSet);//������˨����ƫ�Ʋ�
		pBolt->correct_worknorm();
		pBolt->correct_pos();	
		pBolt->CalGuigeAuto();
		pBolt->SetModified();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	//��ʾ�ֹܲ��
	for(CDesignLjPartPara *pLjPartPara=pPlate->GetFirstLjPara();pLjPartPara;pLjPartPara=pPlate->GetNextLjPara())
	{	
		pLjPartPara->pCurLinePart=(CLDSLineTube*)console.FromPartHandle(pLjPartPara->hPart,CLS_LINEPART);
		if(pLjPartPara->pCurLinePart==NULL)
			continue;
		if(pLjPartPara->pCurLinePart->GetClassTypeId()==CLS_LINETUBE)
		{
			CLDSLineTube *pCurLineTube=(CLDSLineTube*)pLjPartPara->pCurLinePart;
			pCurLineTube->ClearFlag();
			pCurLineTube->CalPosition();
			CLDSParamPlate *pParamPlate=NULL;
			if(pLjPartPara->start0_end1==0)
			{
				pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pLjPartPara->hPart,CLS_PARAMPLATE);
				pCurLineTube->DesignStartJoint();
			}
			else if(pLjPartPara->start0_end1==1)
			{
				pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pLjPartPara->hPart,CLS_PARAMPLATE);
				pCurLineTube->DesignEndJoint();
			}
			if(pParamPlate)
			{
				pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pParamPlate->GetSolidPartObject());
			}
			pCurLineTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pCurLineTube->GetSolidPartObject());
		}
	}
	g_pSolidDraw->Draw();
	//
	g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("����:","");
#endif
	return TRUE;
}
#endif
