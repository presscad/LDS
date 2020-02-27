//<LOCALE_TRANSLATE Confirm by hxr/>
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LDS.h"
#include "tower.h"
#include "stdafx.h"
#include "DesignJdb.h"
#include "env_def.h"
#include "DesignJdb.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "dlg.h"
#include "PromptDlg.h"
#include "ScrTools.h"
#include "DesCrossLjbDlg.h"
#include "GSMH.h"
#include "NetDogAPI.h"
#include "DesignLjParaDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

//����ĺ�������Node.cpp
extern BOOL IsMaxInAll(double dest_val,double other1,double other2,double other3);
void GetCrossNodePartSet(CLDSNode *pNode,f3dPoint cross_norm,LINEPARTSET &partset)
{
	int RayJgN=0;
	normalize(cross_norm);	//���淨�������жϵ�ǰ�����߽Ǹ�
	for(CLDSLineAngle *pJg=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pJg!=NULL;pJg=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
	{
		if(pJg->pStart==NULL||pJg->pEnd==NULL)
			continue;
		if( pNode->handle==pJg->pStart->handle||
			pNode->handle==pJg->pEnd->handle )
		{
			f3dPoint vec=pJg->pEnd->Position(false)-pJg->pStart->Position(false);
			normalize(vec);
			if(fabs(vec*cross_norm)>EPS)
				continue;	//�ǵ�ǰ�����߽Ǹ�
			if(RayJgN==4)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "more over angles selected, system does not support this type of connection design temporally";
#else
				throw "ѡ���˹���ĽǸ�,ϵͳ�ݲ�֧��������ʽ���������";
#endif
			partset.append(pJg);
			RayJgN++;
		}
	}
}
BOOL CDesignJdb::DesignCrossPlank(CLDSPlate* pPlank)
{
	if(pPlank==NULL)
		return FALSE;
	m_pPlank = pPlank;
	pPlank->designInfo.iProfileStyle0123=1;
	m_pBaseNode=console.FromNodeHandle(m_pPlank->designInfo.m_hBaseNode);
	m_pPlank->designInfo.partList.Empty();
	m_pPlank->designInfo.additional_vertex.Empty();
	double ddXX,ddXY,ddYX,ddYY;
	double base_wing_wide,other_wing_wide;
	f3dPoint vertex[8];
	CLDSNode *pSelNode;
	CLDSLineAngle *pRayJg[4]={NULL},*pAnotherJg=NULL;
	BOOL IsJgCollide=FALSE;
		
	CDesignJdb designJdb;
	f3dPoint mark_pos,wing_vec,main_norm;
	try
	{
		pSelNode = console.FromNodeHandle(m_pPlank->designInfo.m_hBaseNode);
	//һ���ڵ���Ч���ж�
		if(pSelNode==NULL)
		{
			g_pSolidSet->SetOperType(OPER_OTHER);
			return FALSE;
		}
		else
		{
			if(pSelNode->m_cPosCalType!=4)
			{
				g_pSolidSet->SetOperType(OPER_OTHER);
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("the selected are not leg rods intersection,invalid operation");
#else
				AfxMessageBox("ѡ�еĲ������Ľ����,������Ч");
#endif
				return FALSE;
			}
		}
		m_pPlank->face_N = 1;
		//������ʾ�����
		g_pSolidDraw->SetEntSnapStatus(pSelNode->handle,TRUE);
		m_pPlank->designInfo.m_hBaseNode = pSelNode->handle;
		m_pPlank->designInfo.m_hBasePart=pSelNode->arrRelationPole[0];
		m_pBasePart=(CLDSLinePart*)console.FromPartHandle(pSelNode->arrRelationPole[0],CLS_LINEANGLE);
		if(m_pBasePart.IsNULL())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("intersection angle lost!");
#else
			AfxMessageBox("����������Ǹ��Ѷ�ʧ!");
#endif
			return FALSE;
		}
		f3dPoint cross_vec1=m_pBasePart.LinePartPointer()->pStart->Position(true)-m_pBasePart.LinePartPointer()->pEnd->Position(true);
		pAnotherJg=(CLDSLineAngle*)console.FromPartHandle(pSelNode->arrRelationPole[1],CLS_LINEANGLE);
		if(pAnotherJg==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("intersection angle lost!");
#else
			AfxMessageBox("����������Ǹ��Ѷ�ʧ!");
#endif
			return FALSE;
		}
		if(m_pBasePart->handle==pAnotherJg->handle)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("two intersection angles are treated as one angle !");
#else
			AfxMessageBox("������������Ǹ�Ϊͬһ���Ǹ�!");
#endif
			return FALSE;
		}
		f3dPoint cross_vec2=pAnotherJg->pStart->Position(true)-pAnotherJg->pEnd->Position(true);
		f3dPoint cross_norm=cross_vec1^cross_vec2;
	//������ý���㴦�Ǹּ���
		GetCrossNodePartSet(pSelNode,cross_norm,partset);
		int i;
		for(i=0;i<4;i++)
		{
			if(i==0)
			{
               CLDSLinePart *pLinePart=partset.GetFirst();
			   if(pLinePart&&pLinePart->GetClassTypeId()==CLS_LINEANGLE)
					pRayJg[i]=(CLDSLineAngle*)pLinePart;
			}
			else
			{
				CLDSLinePart *pLinePart=partset.GetNext();
				if(pLinePart&&pLinePart->GetClassTypeId()==CLS_LINEANGLE)
					pRayJg[i]=(CLDSLineAngle*)pLinePart;
			}
			if(pRayJg[i]==NULL)
                break;
		}
		partset.append(m_pBasePart.LinePartPointer());
		partset.append(pAnotherJg);
	//��������ڴ˽��洦�Ĺ����淽�����õ��ֵ���ڹ���������һ�ԣ����ж��Ƿ����
		if(m_pBasePart.LineAnglePointer()->GetWidth()<pAnotherJg->GetWidth())
		{
			CLDSLineAngle *pTmJg=m_pBasePart.LineAnglePointer();
			m_pBasePart = pAnotherJg;
			pAnotherJg = pTmJg;
		}
		base_wing_wide = m_pBasePart.LineAnglePointer()->GetWidth();
		other_wing_wide = pAnotherJg->GetWidth();
		ddXX = m_pBasePart.LineAnglePointer()->get_norm_x_wing()*pAnotherJg->get_norm_x_wing();
		ddXY = m_pBasePart.LineAnglePointer()->get_norm_x_wing()*pAnotherJg->get_norm_y_wing();
		ddYX = m_pBasePart.LineAnglePointer()->get_norm_y_wing()*pAnotherJg->get_norm_x_wing();
		ddYY = m_pBasePart.LineAnglePointer()->get_norm_y_wing()*pAnotherJg->get_norm_y_wing();
		m_pPlank->designInfo.norm.norm_style=1;
		m_pPlank->designInfo.norm.hVicePart=m_pBasePart->handle;
		m_pPlank->designInfo.norm.direction=0;
		if(IsMaxInAll(ddXX,ddXY,ddYX,ddYY))
		{
			if(ddXX>0)	//������ĽǸ��໥����
				IsJgCollide=TRUE;
			m_pPlank->ucs.axis_z = m_pBasePart.LineAnglePointer()->get_norm_x_wing();
			m_pPlank->designInfo.norm.norm_wing=0;
		}
		else if(IsMaxInAll(ddXY,ddXX,ddYX,ddYY))
		{
			if(ddXY>0)	//������ĽǸ��໥����
				IsJgCollide=TRUE;
			m_pPlank->ucs.axis_z = m_pBasePart.LineAnglePointer()->get_norm_x_wing();
			m_pPlank->designInfo.norm.norm_wing=0;
		}
		else if(IsMaxInAll(ddYX,ddXY,ddXX,ddYY))
		{
			if(ddYX>0)	//������ĽǸ��໥����
				IsJgCollide=TRUE;
			m_pPlank->ucs.axis_z = m_pBasePart.LineAnglePointer()->get_norm_y_wing();
			m_pPlank->designInfo.norm.norm_wing=1;
		}
		else// if(IsMaxInAll(ddYY,ddXY,ddXX,ddYX))
		{
			if(ddYY>0)	//������ĽǸ��໥����
				IsJgCollide=TRUE;
			m_pPlank->ucs.axis_z = m_pBasePart.LineAnglePointer()->get_norm_y_wing();
			m_pPlank->designInfo.norm.norm_wing=1;
		}
		if(IsJgCollide)	//������ĽǸ��໥����
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("cross bar Angle interfere,use truncation order to truncate a cross bar, use planar plate method to design the cross board!");
#else
			AfxMessageBox("����ĽǸ��໥���棬�����ýض�����ض�һ������ĺ��õ������Ƶķ�����ƴ˽����!");
#endif
			return FALSE;
		}
		else if(partset.GetNodeNum()!=0)	//������޸���,�������������߽Ǹ�
		{
			double dd = DistOf3dll(m_pBasePart.LineAnglePointer()->Start(),m_pBasePart.LineAnglePointer()->End(),
				pAnotherJg->Start(),pAnotherJg->End());
			m_pPlank->Thick=ftol(dd);
			if(m_pPlank->GetThick()<2)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "plate thickness is too thin, wrong data,it is possible the ends of cross bar have not designed !";
#else
				throw "������,��������,�п����ǽ���ĵ�������δ���!";
#endif
			m_pPlank->designInfo.m_hBasePart = m_pBasePart->handle;
		}
		else// if(jgset.GetNodeNum()==0)	//�޸��潻���,�����������߽Ǹ�
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The intersection does not interfere or collide angle or radial angle, no need to design cross board!";
#else
			throw "��������û�и�����ײ�Ǹֻ����߲ĽǸ�,������ƽ����!";
#endif

	//�ġ�-------------�����������������ϵ--------------

		//���µ�λ���㽻����������
		pSelNode->GetIntersZhun(&m_pPlank->ucs.origin);
		f3dPoint perp;
		//�˴��޸�Ϊ�ڻ�׼�Ǹ������ϻ�ȡ���� wht 11-01-18
		SnapPerp(&perp,m_pBasePart.LineAnglePointer()->Start(),m_pBasePart.LineAnglePointer()->End(),m_pPlank->ucs.origin,NULL);
		//SnapPerp(&perp,pAnotherJg->Start(),pAnotherJg->End(),m_pPlank->ucs.origin,NULL);
		project_point(m_pPlank->ucs.origin,perp,m_pPlank->ucs.axis_z);
		//���������������ϵ
		m_pPlank->ucs.axis_y = m_pBasePart.LineAnglePointer()->End()-m_pBasePart.LineAnglePointer()->Start();
		m_pPlank->ucs.axis_x = m_pPlank->ucs.axis_y^m_pPlank->ucs.axis_z;
		m_pPlank->ucs.axis_y = m_pPlank->ucs.axis_z^m_pPlank->ucs.axis_x;
		normalize(m_pPlank->ucs.axis_x);
		normalize(m_pPlank->ucs.axis_y);
		normalize(m_pPlank->ucs.axis_z);
		m_pPlank->designInfo.origin.datum_pos_style=3;
		m_pPlank->designInfo.origin.des_para.AXIS_INTERS.hDatum1=m_pBasePart->handle;
		m_pPlank->designInfo.origin.des_para.AXIS_INTERS.hDatum2=pAnotherJg->handle;
		static CDesCrossLjbDlg cross_ljb_dlg;
		cross_ljb_dlg.m_nThick=(int)m_pPlank->GetThick();
		if(cross_ljb_dlg.DoModal()!=IDOK)
		{
			m_bTerminateByUser=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
			throw "drop out halfway!";	
#else
			throw "��;�˳�!";		
#endif
		}
		m_pPlank->Thick=cross_ljb_dlg.m_nThick;
		xDesLjbPara.nVertexDist=cross_ljb_dlg.m_nVertexDist;
		xDesLjbPara.iProfileCalType01=cross_ljb_dlg.m_iProfileType;
		m_pPlank->SetPartNo(cross_ljb_dlg.m_sPartNo.GetBuffer(0));
		m_pPlank->iSeg=SEGI(cross_ljb_dlg.m_sPartSegI.GetBuffer());
		m_pPlank->cMaterial=CSteelMatLibrary::RecordAt(cross_ljb_dlg.m_iMaterial).cBriefMark;

		//��ƻ�׼�����
		DesignLineAngle(1,m_pBasePart.LineAnglePointer(),pAnotherJg,TRUE);	//��Ƶ�һ�������
		//�������������˨�����轫��׼�����ϵ���˨���뵽�ǻ�׼���ϼ��� wht 11-01-18
		CLsRef *pLsRef=NULL;
		BOLTSET legacyBolts;
		for(pLsRef=m_pBasePart.LineAnglePointer()->GetFirstLsRef();pLsRef;pLsRef=m_pBasePart.LineAnglePointer()->GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(!pAnotherJg->IsLsInWing(pBolt))
				continue;
			legacyBolts.append(pBolt);
			if(!pAnotherJg->FindLsByHandle(pBolt->handle))
			{
				pAnotherJg->AppendMidLsRef(pBolt->GetLsRef());
				pBolt->AddL0Thick(pAnotherJg->handle,TRUE);	//��˨�����ͨ��
			}
		}
		//��Ʒǻ�׼�����
		DesignLineAngle(1,pAnotherJg,m_pBasePart.LineAnglePointer(),TRUE,NULL,&legacyBolts);	//��Ƶڶ��������
		int nRayPoleCount=0;
		for(i=0;i<partset.GetNodeNum()-2;i++)
		{
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pRayJg[i]->handle);
			if(pRayJg[i]->pStart==m_pBaseNode)
			{
				pRayJg[i]->desStartOdd.m_fCollideDist=g_sysPara.VertexDist;
				pRayJg[i]->desStartOdd.m_iOddCalStyle=0;
				pRayJg[i]->desStartOdd.m_hRefPart1=m_pBasePart->handle;
				pRayJg[i]->desStartOdd.m_hRefPart2=m_pBasePart->handle;
				pRayJg[i]->CalStartOddment();
			}
			else if(pRayJg[i]->pEnd==m_pBaseNode)
			{
				pRayJg[i]->desEndOdd.m_fCollideDist=g_sysPara.VertexDist;
				pRayJg[i]->desEndOdd.m_iOddCalStyle=0;
				pRayJg[i]->desEndOdd.m_hRefPart1=m_pBasePart->handle;
				pRayJg[i]->desEndOdd.m_hRefPart2=m_pBasePart->handle;
				pRayJg[i]->CalEndOddment();
			}
			DesignLineAngle(1,pRayJg[i],m_pBasePart.LineAnglePointer());	//������߽Ǹ�
		}
		m_pPlank->jdb_style = 0;
		m_pPlank->designInfo.iFaceType=1;
		pPlank->designInfo.m_bEnableFlexibleDesign=TRUE;	//���������������
		CDesignLjParaDlg lj_dlg;
		m_pPlank->designInfo.iProfileStyle0123=xDesLjbPara.iProfileCalType01+1; //���ζ��巽ʽ wht 11-01-18
		/*lj_dlg.m_pLjPara=&pPlank->designInfo;
		if(lj_dlg.DoModal()!=IDOK)
			throw "��;�˳�!";*/
		if(xDesLjbPara.iProfileCalType01==2&&partset.GetNodeNum()==2)	//�����ڲ������(��֧�������߽Ǹֵ������������ΰ� wht 11 04-27)
			m_pPlank->DesignRhombusPlate();
		else //��ͨ����
			m_pPlank->DesignPlate();
		//m_pPlank->designInfo.iProfileStyle0123=3;	//�Զ������� ����ΪĬ��Ϊ�������θ�����,����ͷ����ʱ���ܲ��ܳ��׸������� wjh-2013.12.20
	}
	catch(char *sError)
	{
		if(!m_bTerminateByUser)
			AfxMessageBox(sError);
		return FALSE;
	}
	return TRUE;
}
#endif