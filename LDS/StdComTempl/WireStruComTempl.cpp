#include "stdafx.h"
#include "Tower.h"
#include "NodeStdComTempl.h"
#include "WireStruComTempl.h"
#include "Markup.h"

#ifdef __LDS_
TOWER_PLANINFO::TOWER_PLANINFO()
{
	biCircuitDC=false;		//是否为直流 
	ciTensionType=0;		//杆塔类型:0.直线悬垂塔;1.耐张塔;2.终端塔;3.分岐塔;4.换位塔
	wiVoltGrade=750;		//电压等级kV
	wiCircuits=2;		//回路数
	wiSeaLevel=1000;	//海拔高度,m
	uiNamedHeight=0;		//呼高(mm),0表示根据塔型及电压等级自动选择呼高
	pSetupNode10=pSetupNode20=NULL;
	szTowerTypeCode="Gu";
}
bool ExportShankDefToXML(CMarkup& xml,CShankDefineObject *pParent)
{
	int i;
	CString sText;
	xml.IntoElem();
	if(pParent->cObjType=='K')
	{	//坡段数据
		xml.AddElem("K");
		sText.Format("%g",pParent->Slope()->m_fTopCoordZ);
		xml.AddAttrib("起始高",sText);
		sText.Format("%g",pParent->Slope()->m_fTopFrontWidth);
		xml.AddAttrib("正面开口",sText);
		sText.Format("%g",pParent->Slope()->m_fTopSideWidth);
		xml.AddAttrib("侧面开口",sText);
		if(pParent->Slope()->m_cMainRodType=='T')
			xml.AddAttrib("主材类型","T");
		else
			xml.AddAttrib("主材类型","L");
		for(i=0;i<pParent->Slope()->ModuleNum();i++)
		{	//导出子项 DATA_MODULE
			CShankModule *pModule=pParent->Slope()->ModuleAtIndex(i);
			ExportShankDefToXML(xml,pModule);
		}
		for(i=0;i<pParent->Slope()->SegmentNum();i++)
		{	//导出子项 DATA_SEG
			CShankSegment *pSeg=pParent->Slope()->SegmentAtIndex(i);
			ExportShankDefToXML(xml,pSeg);
		}
	}
	else if(pParent->cObjType=='D')
	{	//坡段内分段数据
		xml.AddElem("D");
		sText.Format("%g",pParent->Segment()->m_fHeight);
		xml.AddAttrib("垂高",sText);
		sText.Format("%s",(char*)pParent->Segment()->m_iSeg.ToString());
		xml.AddAttrib("段号",sText);
		if(pParent->Segment()->SectionNum()==0)
		{
			sText.Format("%d",pParent->Segment()->divide_n);
			xml.AddAttrib("等分数",sText);
		}
		if(strlen(pParent->Segment()->front_layout)>=1)
			xml.AddAttrib("正面布材",CString(pParent->Segment()->front_layout));
		if(strlen(pParent->Segment()->side_layout)>=1)
			xml.AddAttrib("侧面布材",CString(pParent->Segment()->side_layout));
		if(strlen(pParent->Segment()->hsect_layout)>=1)
			xml.AddAttrib("上横隔布材",CString(pParent->Segment()->hsect_layout));
		if(strlen(pParent->Segment()->v_layout)>=1)
			xml.AddAttrib("V面布材",CString(pParent->Segment()->v_layout));
		for(i=0;i<pParent->Segment()->JunctionSegmentNum();i++)
		{	//导出子项 DATA_SEG
			CShankSegment *pSeg=pParent->Segment()->JunctionSegmentAtIndex(i);
			ExportShankDefToXML(xml,pSeg);
		}
		for(i=0;i<pParent->Segment()->SectionNum();i++)
		{	//导出子项 DATA_SECTION
			CShankSection *pSection=pParent->Segment()->SectionAtIndex(i);
			ExportShankDefToXML(xml,pSection);
		}
		for(i=0;i<pParent->Segment()->ModuleNum();i++)
		{	//导出子项 DATA_MODULE
			CShankModule *pModule=pParent->Segment()->ModuleAtIndex(i);
			ExportShankDefToXML(xml,pModule);
		}
	}
	else if(pParent->cObjType=='B')
	{	//呼高分组数据
		xml.AddElem("B");
		xml.AddAttrib("名称",CString(pParent->Module()->name));
		for(i=0;i<pParent->Module()->SegmentNum();i++)
		{	//导出子项 DATA_SEG
			CShankSegment *pSegment=pParent->Module()->SegmentAtIndex(i);
			ExportShankDefToXML(xml,pSegment);
		}
		for(i=0;i<pParent->Module()->LegNum();i++)
		{	//导出子项 DATA_LEG
			CShankLeg *pLeg=pParent->Module()->LegAtIndex(i);
			ExportShankDefToXML(xml,pLeg);
		}
	}
	else if(pParent->cObjType=='L')
	{	//塔腿数据
		xml.AddElem("L");
		sText.Format("%g",pParent->Leg()->m_fHeight);
		xml.AddAttrib("垂高",sText);
		if(pParent->Leg()->m_fFrontWidth>0)
		{
			sText.Format("%g",pParent->Leg()->m_fFrontWidth);
#ifdef AFX_TARG_ENU_ENGLISH
			xml.AddAttrib("front open",sText);
#else
			xml.AddAttrib("正面开口",sText);
#endif
		}
		if(pParent->Leg()->m_fSideWidth>0)
		{
			sText.Format("%g",pParent->Leg()->m_fSideWidth);
#ifdef AFX_TARG_ENU_ENGLISH
			xml.AddAttrib("side open",sText);
#else
			xml.AddAttrib("侧面开口",sText);
#endif
		}
		sText.Format("%s",(char*)pParent->Leg()->m_iSeg.ToString());
		xml.AddAttrib("段号",sText);
		xml.AddAttrib("节间数",(char*)CXhChar16(pParent->Leg()->divide_n));
		if(strlen(pParent->Leg()->front_layout)>=1)
			xml.AddAttrib("正面布材",CString(pParent->Leg()->front_layout));
		if(strlen(pParent->Leg()->side_layout)>=1)
			xml.AddAttrib("侧面布材",CString(pParent->Leg()->side_layout));
		if(strlen(pParent->Leg()->hsect_layout)>=1)
			xml.AddAttrib("上横隔布材",CString(pParent->Leg()->hsect_layout));
		if(strlen(pParent->Leg()->v_layout)>=1)
			xml.AddAttrib("V面布材",CString(pParent->Leg()->v_layout));
		for(i=0;i<pParent->Leg()->SectionNum();i++)
		{	//导出子项 DATA_SECTION
			CShankSection *pSection=pParent->Leg()->SectionAtIndex(i);
			ExportShankDefToXML(xml,pSection);
		}
	}
	else if(pParent->cObjType=='J')
	{	//分段内节点数据
		xml.AddElem("J");
		/*CExpression expression;
		EXPRESSION_VAR *pVar=expression.varList.append();
		pVar->fValue=1;
		strcpy(pVar->variableStr,"D");
		double pos=expression.SolveExpression(pParent->Section()->btm_zstr);
		if(pos<=1)*/
		if(strlen(pParent->Section()->btm_zstr)>=1)
			xml.AddAttrib("垂高",CString(pParent->Section()->btm_zstr));
		if(strlen(pParent->Section()->front_layout)>=1)
			xml.AddAttrib("正面布材",CString(pParent->Section()->front_layout));
		if(strlen(pParent->Section()->side_layout)>=1)
			xml.AddAttrib("侧面布材",CString(pParent->Section()->side_layout));
		if(strlen(pParent->Section()->hsect_layout)>=1)
			xml.AddAttrib("上横隔布材",CString(pParent->Section()->hsect_layout));
		if(strlen(pParent->Section()->v_layout)>=1)
			xml.AddAttrib("V面布材",CString(pParent->Section()->v_layout));
		sText.Format("%d",pParent->Section()->divide_n);
		xml.AddAttrib("等分数",sText);
	}
	xml.OutOfElem();
	return true;
}
bool ExportShankDefToXML(CMarkup& xml,CShankDefine *pShankDefine)
{
	int i;
	CString sText;
	for(i=0;i<pShankDefine->SlopeNum();i++)
	{
		CShankSlope *pSlope=pShankDefine->SlopeAtIndex(i);
		ExportShankDefToXML(xml,pSlope);
	}
	return true;
}
#include "..\MirTaAtom.h"
bool CTowerPlanner::AppendWireStruArms(IModel* pModel)
{
	ARM_ASSMBLY* pArm;
	for(pArm=this->hashArmAssmblies.GetFirst();pArm;pArm=this->hashArmAssmblies.GetNext())
	{
		GEPOINT xPosNode10(pArm->xN10.xfSectCorner,pArm->xN10.yfSectCorner,pArm->xN10.ziPlane);
		GEPOINT xPosNode20(pArm->xN20.xfSectCorner,pArm->xN20.yfSectCorner,pArm->xN20.ziPlane);
		CLDSNode*pNode, *pSetupNode10=NULL,*pSetupNode20=NULL;
		for(pNode=pModel->EnumNodeFirst();pNode;pNode=pModel->EnumNodeNext())
		{
			if(pSetupNode10==NULL&&pNode->xOriginalPos.IsEqual(xPosNode10))
				pSetupNode10=pNode;
			if(pSetupNode20==NULL&&pNode->xOriginalPos.IsEqual(xPosNode20))
				pSetupNode20=pNode;
			if(pSetupNode10&&pSetupNode20)
				break;
		}
		CWireStruComTemplModel model(pModel);
		CArmHeadComTempl xWireStruTempl;
		IStdComTempl::TEMPL_SOURCE source(0,pArm->idWireStruTmpl);
		xWireStruTempl.InitComponentTempl(&source);

		if(pSetupNode10)
		{
			CStdTemplParaNode* pParaNode=xWireStruTempl.listParaNodes.Add(KEY4C(1));	//N10节点
			long hSetupNode=pSetupNode10!=NULL?pSetupNode10->handle:0;
			xWireStruTempl.SetMappingNode(1,hSetupNode);
			CLDSNode* pMirNodeX=pSetupNode10->GetMirNode(MIRMSG(1));
			CStdTemplParaNode* pMirParaNode=xWireStruTempl.listParaNodes.Add(TMPLOBJID(1,'X'));	//N10节点
			if(pMirNodeX!=NULL&&pMirParaNode!=NULL)
			{
				xWireStruTempl.SetMappingNode(TMPLOBJID(1,'X'),pMirNodeX->handle);
				pMirParaNode->pRelaNode=pMirNodeX;
			}
		}
		if(pSetupNode20)
		{
			CStdTemplParaNode* pParaNode=xWireStruTempl.listParaNodes.Add(KEY4C(2));	//N20节点
			long hSetupNode=pSetupNode20!=NULL?pSetupNode20->handle:0;
			xWireStruTempl.SetMappingNode(2,hSetupNode);
			CLDSNode* pMirNodeX=pSetupNode20->GetMirNode(MIRMSG(1));
			CStdTemplParaNode* pMirParaNode=xWireStruTempl.listParaNodes.Add(TMPLOBJID(2,'X'));	//N10节点
			if(pMirNodeX!=NULL&&pMirParaNode!=NULL)
			{
				xWireStruTempl.SetMappingNode(TMPLOBJID(2,'X'),pMirNodeX->handle);
				pMirParaNode->pRelaNode=pMirNodeX;
			}
		}

		if(pSetupNode10&&pSetupNode20)
			model.BuildModel(&xWireStruTempl);
		//刷新实体
		CLDSPart* pPart;
		for(pNode=model.m_xMapNodeSet.GetFirst();pNode;pNode=model.m_xMapNodeSet.GetNext())
			MirTaAtom(pNode,MIRMSG(2),FALSE);
		for(pPart=model.m_xMapPartSet.GetFirst();pPart;pPart=model.m_xMapPartSet.GetNext())
			MirTaAtom(pPart,MIRMSG(2),FALSE);
		for(pNode=model.m_xMapNodeSet.GetFirst();pNode;pNode=model.m_xMapNodeSet.GetNext())
			MirTaAtom(pNode,MIRMSG(2),TRUE);
		for(pPart=model.m_xMapPartSet.GetFirst();pPart;pPart=model.m_xMapPartSet.GetNext())
			MirTaAtom(pPart,MIRMSG(2),TRUE);
	}
	return true;
}
bool CTowerPlanner::Planning(TOWER_PLANINFO& planInfo,CTower* pTower)
{
	WORD wnMetalFittingLength=250;
	WORD wnInsulatorLength=0;
	WORD wnElectricSpace=1;
	if(planInfo.wiVoltGrade<=110)
	{
		wnInsulatorLength=146*7;
		planInfo.uiNamedHeight=30000;
	}
	else if(planInfo.wiVoltGrade<=220)
	{
		wnInsulatorLength=146*13;
		wnElectricSpace=1800;
		planInfo.uiNamedHeight=40000;
	}
	else if(planInfo.wiVoltGrade<=330)
	{
		wnInsulatorLength=146*17;
		wnElectricSpace=2200;
		planInfo.uiNamedHeight=50000;
	}
	else if(planInfo.wiVoltGrade<=500)
	{
		wnInsulatorLength=155*25;
		wnElectricSpace=3200;
		planInfo.uiNamedHeight=60000;
	}
	else //if(planInfo.wiVoltGrade<=750)
	{
		wnInsulatorLength=170*32;
		wnElectricSpace=4300;
		planInfo.uiNamedHeight=70000;
	}
	if(planInfo.ciTensionType>0)
	{
		if(planInfo.wiVoltGrade<=330)
			wnInsulatorLength+=146;	//110~330kV耐张绝缘子增加一片
		else if(planInfo.wiVoltGrade==500)
			wnInsulatorLength+=155*2;	//500kV耐张绝缘子增加两片
	}
	WORD wnAttachLength=wnMetalFittingLength+wnInsulatorLength+wnElectricSpace;
	UINT uiTotalHeight=planInfo.uiNamedHeight;
	UINT uiLayerHeightEarthToCircuit=wnAttachLength;
	UINT uiLayerHeightCircuitToCircuit=wnAttachLength;
	if(strstr(planInfo.szTowerTypeCode,"Gu")!=NULL)
	{
		if(planInfo.wiCircuits==1)
		{
			uiTotalHeight+=wnAttachLength*2;
		}
		else if(planInfo.wiCircuits==2)
		{
			uiTotalHeight+=wnAttachLength*3;
		}
		else if(planInfo.wiCircuits==4)
		{
			uiTotalHeight+=wnAttachLength*6;
		}
	}
	if(uiTotalHeight>40)
	{	//
		int add_pieces=ftoi((uiTotalHeight*0.001-40)*0.1);
		wnInsulatorLength=add_pieces*146;
	}
	double gfSlopeZ2X=25;	//Z2X,一般可取值20~30
	long ziCurrPlane=0;
	double gfXieRod=0.5;	//30°~50°，z2x=0.5~1.1918
	ARM_ASSMBLY* pArmAssm,*pTopArmAssm=NULL,*pUpperArmAssm=NULL;
	//if(planInfo.wiCircuits==2)
	{	//目前以下代仅对双回路鼓型塔有效
		pArmAssm=pTopArmAssm=this->hashArmAssmblies.Add(1);
		pArmAssm->ciDirection='X';
		pArmAssm->name="导地线复合横担";
		pArmAssm->idWireStruTmpl=1;	//HYJ-1
		pArmAssm->xN10.ziPlane=0;
		pArmAssm->xN10.xfSectCorner=pArmAssm->xN10.yfSectCorner=1000;
		double xfWidth=pArmAssm->xN10.xfSectCorner;
		int nMinSection=ftoi(uiLayerHeightCircuitToCircuit/(xfWidth*2.2));
		int nMaxSection=ftoi(uiLayerHeightCircuitToCircuit/xfWidth);
		int nSection=6;
		if(nMinSection>=6||nMaxSection<=6)
			nSection=(nMinSection+nMaxSection)/2;
		double zfSectHeight=(uiLayerHeightCircuitToCircuit/nSection);	//最下一个节间属于中层导线横担
		zfSectHeight=zfSectHeight-ftoi(zfSectHeight)%100+100;
		pArmAssm->xN20.ziPlane=pArmAssm->xN10.ziPlane+ftoi(zfSectHeight);
		pArmAssm->xN20.xfSectCorner=pArmAssm->xN20.yfSectCorner=pArmAssm->xN10.xfSectCorner+zfSectHeight/gfSlopeZ2X;
		//计算中层导线横担	
		uiLayerHeightCircuitToCircuit=ftoi(zfSectHeight*nSection);	//预估上中层导线横担层高
		ziCurrPlane=ftoi(zfSectHeight*(nSection-1));	//中层导线横担对应塔身上顶面Z坐标
		double xfTopSectCorner=pArmAssm->xN10.xfSectCorner+ziCurrPlane/gfSlopeZ2X;
		nMinSection=ftoi(uiLayerHeightCircuitToCircuit/(xfTopSectCorner*2.2));
		nMaxSection=ftoi(uiLayerHeightCircuitToCircuit/xfTopSectCorner);
		int nSection2=(nMinSection<5&&5<nMaxSection)?5:(nMinSection+nMaxSection)/2;
		double zfSectHeight2=uiLayerHeightCircuitToCircuit/nSection2;	//最下一个节间属于中层导线横担
		zfSectHeight2=zfSectHeight2-ftoi(zfSectHeight2)%100+100;
		ARM_ASSMBLY* pArmAssm2=this->hashArmAssmblies.Add(2);
		pArmAssm2->ciDirection='X';
		pArmAssm2->name="中导线横担";
		pArmAssm2->idWireStruTmpl=2;	//HX3
		pArmAssm2->xN10.ziPlane=pArmAssm->xN10.ziPlane+uiLayerHeightCircuitToCircuit;
		pArmAssm2->xN10.xfSectCorner=pArmAssm2->xN10.yfSectCorner=pArmAssm->xN10.xfSectCorner+pArmAssm2->xN10.ziPlane/gfSlopeZ2X;
		pArmAssm2->xN20.ziPlane=pArmAssm2->xN10.ziPlane+ftoi(zfSectHeight2);
		pArmAssm2->xN20.xfSectCorner=pArmAssm2->xN20.yfSectCorner=pArmAssm->xN10.xfSectCorner+pArmAssm2->xN20.ziPlane/gfSlopeZ2X;
		//计算下层导线横担
		uiLayerHeightCircuitToCircuit=ftoi(zfSectHeight2*nSection);	//预估上中层导线横担层高
		ziCurrPlane+=ftoi(zfSectHeight2*(nSection-1));	//中层导线横担对应塔身上顶面Z坐标
		xfTopSectCorner=pArmAssm->xN10.xfSectCorner+ziCurrPlane/gfSlopeZ2X;
		nMinSection=ftoi(uiLayerHeightCircuitToCircuit/(xfTopSectCorner*2.2));
		nMaxSection=ftoi(uiLayerHeightCircuitToCircuit/xfTopSectCorner);
		int nSection3=(nMinSection<5&&5<nMaxSection)?5:(nMinSection+nMaxSection)/2;
		double zfSectHeight3=uiLayerHeightCircuitToCircuit/nSection3;	//最下一个节间属于中层导线横担
		zfSectHeight3=zfSectHeight3-ftoi(zfSectHeight3)%100+100;
		ARM_ASSMBLY* pArmAssm3=this->hashArmAssmblies.Add(3);
		pArmAssm3->ciDirection='X';
		pArmAssm3->name="下导线横担";
		pArmAssm3->idWireStruTmpl=2;	//HX3
		pArmAssm3->xN10.ziPlane=pArmAssm2->xN10.ziPlane+uiLayerHeightCircuitToCircuit;
		pArmAssm3->xN10.xfSectCorner=pArmAssm3->xN10.yfSectCorner=pArmAssm->xN10.xfSectCorner+pArmAssm3->xN10.ziPlane/gfSlopeZ2X;
		pArmAssm3->xN20.ziPlane=pArmAssm3->xN10.ziPlane+ftoi(zfSectHeight3);
		pArmAssm3->xN20.xfSectCorner=pArmAssm3->xN20.yfSectCorner=pArmAssm->xN10.xfSectCorner+pArmAssm3->xN20.ziPlane/gfSlopeZ2X;
	}
	//创建杆塔塔身模板
	//1.创建上层导线横担对应塔身分段
	pUpperArmAssm=pTopArmAssm;
 	ARM_ASSMBLY* pBtmArmAssm=hashArmAssmblies.GetValue(2);
	CShankSlope *pUpperSlope=AppendSlope();
	pUpperSlope->m_fTopCoordZ=0;
	pUpperSlope->m_fTopFrontWidth=pTopArmAssm->xN10.xfSectCorner*2;
	pUpperSlope->m_fTopSideWidth =pTopArmAssm->xN10.yfSectCorner*2;
	CShankSegment* pSegment=pUpperSlope->AppendSegment();
	pSegment->m_fHeight=pBtmArmAssm->xN10.ziPlane-pTopArmAssm->xN10.ziPlane;
	CShankSection* pSection=pSegment->AppendSection();
	int niSectHeight=pTopArmAssm->xN20.ziPlane-pTopArmAssm->xN10.ziPlane;
	pSection->btm_zstr.Printf("%d",niSectHeight);
	StrCopy(pSection->front_layout,"10",8);
	memcpy(pSection->side_layout,pSection->front_layout,8);
	memcpy(pSection->hsect_layout,"PL1",4);

	pSection=pSegment->AppendSection();
	pSection->divide_n=ftoi(pSegment->m_fHeight/niSectHeight)-1;
	pSection->btm_zstr.Printf("%g",pSegment->m_fHeight-niSectHeight);
	memcpy(pSection->front_layout,"10",3);
	memcpy(pSection->side_layout,pSection->front_layout,8);
	memcpy(pSection->hsect_layout,"PL2",4);
	//2.创建中层导线横担对应塔身分段
	pUpperArmAssm=pBtmArmAssm;
	pBtmArmAssm=hashArmAssmblies.GetValue(3);
	pSegment=pUpperSlope->AppendSegment();
	pSegment->m_fHeight=pBtmArmAssm->xN10.ziPlane-pUpperArmAssm->xN10.ziPlane;
	pSection=pSegment->AppendSection();
	niSectHeight=pUpperArmAssm->xN20.ziPlane-pUpperArmAssm->xN10.ziPlane;
	pSection->btm_zstr.Printf("%d",niSectHeight);
	StrCopy(pSection->front_layout,"10",8);
	memcpy(pSection->side_layout,pSection->front_layout,8);
	memcpy(pSection->hsect_layout,"PL1",4);

	pSection=pSegment->AppendSection();
	pSection->divide_n=ftoi(pSegment->m_fHeight/niSectHeight)-1;
	pSection->btm_zstr.Printf("%g",pSegment->m_fHeight-niSectHeight);
	memcpy(pSection->front_layout,"10",3);
	memcpy(pSection->side_layout,pSection->front_layout,8);
	memcpy(pSection->hsect_layout,"PL2",4);
	//3.创建下层导线横担对应塔身分段
	pUpperArmAssm=pBtmArmAssm;
	pBtmArmAssm=hashArmAssmblies.GetValue(3);
	pSegment=pUpperSlope->AppendSegment();
	pSegment->m_fHeight=2*(pUpperArmAssm->xN20.ziPlane-pUpperArmAssm->xN10.ziPlane);
	pSection=pSegment->AppendSection();
	niSectHeight=pUpperArmAssm->xN20.ziPlane-pUpperArmAssm->xN10.ziPlane;
	pSection->btm_zstr.Printf("%d",niSectHeight);
	StrCopy(pSection->front_layout,"10",8);
	memcpy(pSection->side_layout,pSection->front_layout,8);
	memcpy(pSection->hsect_layout,"PL1",4);
	pSection=pSegment->AppendSection();
	pSection->divide_n=ftoi(pSegment->m_fHeight/niSectHeight)-1;
	pSection->btm_zstr.Printf("%g",pSegment->m_fHeight-niSectHeight);
	memcpy(pSection->front_layout,"10",3);
	memcpy(pSection->side_layout,pSection->front_layout,8);
	memcpy(pSection->hsect_layout,"PL2",4);
	
	//创建变坡以下的塔身坡段
	double gfSlopeZ2X2=max(10,gfSlopeZ2X-15);
	CShankSlope *pDownerSlope=AppendSlope();
	pDownerSlope->m_fTopCoordZ=pUpperArmAssm->xN10.ziPlane+pSegment->m_fHeight;
	pDownerSlope->m_fTopFrontWidth=(pTopArmAssm->xN10.xfSectCorner+pDownerSlope->m_fTopCoordZ/gfSlopeZ2X)*2;
	pDownerSlope->m_fTopSideWidth =pDownerSlope->m_fTopFrontWidth;
	long niLeastHeight=planInfo.uiNamedHeight-ftoi(pDownerSlope->m_fTopCoordZ-pUpperArmAssm->xN20.ziPlane);
	double xfTopSectWidth=pDownerSlope->m_fTopFrontWidth;
	double zfTopZ=ziCurrPlane=ftoi(pDownerSlope->m_fTopCoordZ);
	bool firstSegment=true;
	while(niLeastHeight>0)
	{
		double zfMinHeight =ftoi(xfTopSectWidth*1.0);
		double zfMidHeight =ftoi(xfTopSectWidth*1.6);
		double zfMaxHeight =ftoi(xfTopSectWidth*2.2);
		int odd=ftoi(zfMidHeight)%500;
		if(odd>0)
			zfMidHeight=zfMidHeight-odd+500;
		if(zfMidHeight>zfMaxHeight)
		{
			if((odd=ftoi(zfMaxHeight)%500)>0)
				zfMidHeight=zfMaxHeight-odd;
			else
				zfMidHeight=zfMaxHeight;
		}
		pSegment=pDownerSlope->AppendSegment();
		pSegment->divide_n=2;
		pSegment->m_fHeight=zfMidHeight*2;
		ziCurrPlane+=ftoi(pSegment->m_fHeight);
		niLeastHeight-=ftoi(pSegment->m_fHeight);
		double zfTopHalfWidth=pDownerSlope->m_fTopFrontWidth+2*(ziCurrPlane-pDownerSlope->m_fTopCoordZ)/gfSlopeZ2X2;
		//pSection=pSegment->AppendSection();
		pSegment->divide_n=2;//ftoi(pSegment->m_fHeight/niSectHeight)-1;
		memcpy(pSegment->front_layout,"12",3);
		memcpy(pSegment->side_layout,pSegment->front_layout,8);
		if(firstSegment)
			memcpy(pSection->hsect_layout,"PL7",4);
		firstSegment=false;

		if(niLeastHeight<1.1*pSegment->m_fHeight)
		{
			CShankModule* pModule=pSegment->AppendModule();
			CShankLeg* pLeg=pModule->AppendLeg();
			pModule->name="基准呼高";
			pLeg->m_fHeight=niLeastHeight;
			pLeg->m_fFrontWidth=pDownerSlope->m_fTopFrontWidth+2*(ziCurrPlane+niLeastHeight-pDownerSlope->m_fTopCoordZ)/gfSlopeZ2X2;
			pLeg->m_fSideWidth=pLeg->m_fFrontWidth;
			if(pLeg->m_fHeight<3)
			{
				memcpy(pLeg->front_layout,"L1",3);
				pLeg->divide_n=2;
			}
			else if(pLeg->m_fHeight<4)
			{
				memcpy(pLeg->front_layout,"L2",3);
				pLeg->divide_n=3;
			}
			else if(pLeg->m_fHeight<5)
			{
				memcpy(pLeg->front_layout,"L3",3);
				pLeg->divide_n=4;
			}
			else if(pLeg->m_fHeight<6)
			{
				memcpy(pLeg->front_layout,"L5",3);
				pLeg->divide_n=5;
			}
			else if(pLeg->m_fHeight<7)
			{
				memcpy(pLeg->front_layout,"L6",3);
				pLeg->divide_n=6;
			}
			else if(pLeg->m_fHeight<8)
			{
				memcpy(pLeg->front_layout,"L9",3);
				pLeg->divide_n=7;
			}
			else //if(pLeg->m_fHeight<9)
			{
				memcpy(pLeg->front_layout,"L9",3);
				pLeg->divide_n=8;
			}
			memcpy(pLeg->side_layout,pLeg->front_layout,3);
			memcpy(pLeg->hsect_layout,"PL19",5);
			break;
		}
	}

	//导出XML文件
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("铁塔");
	xml.AddAttrib("单位","毫米");
	xml.AddAttrib("接腿模式",1);
	ExportShankDefToXML(xml,this);
	//ExprotListChildItemToXML(xml,DATA_SLOPE,NULL);
	xml.Save("D:\\test.xml");
	///////////////////////////////////
	CShankDefineToTowerModel modeler;
	modeler.UpdateTowerModel(pTower, this);
	AppendWireStruArms(pTower);
	return false;
}
bool CWireStruComTemplModel::IntelliDesignTowerParams(TOWER_PLANINFO& planinfo)
{
	return false;
}
//////////////////////////////////////////////////////////////////////////
//
CWireStruComTemplModel::CWireStruComTemplModel(IModel* pModel) : CStdComModel(pModel)
{
}
CLDSNode* CWireStruComTemplModel::GetLdsNodeFromId(TMPLOBJID idNode)
{
	TMPLOBJID id=idNode;
	id.ciMirTransType=0;
	if(id==KEY4C("N10"))
		id=1;
	else if(id==KEY4C("N20"))
		id=2;
	id.ciMirTransType=idNode.ciMirTransType;
	return CStdComModel::GetLdsNodeFromId(id);
}
#endif
CArmHeadComTempl::CArmHeadComTempl(void)
{
	m_idWireStruTmplType=0;
}
CArmHeadComTempl::~CArmHeadComTempl(void)
{
}
const char* CArmHeadComTempl::GetName()
{
	return name;
}
bool CArmHeadComTempl::GetSketchImage(IMAGE_INFO* pSketchImage)
{
	if(pSketchImage)
	{
		pSketchImage->ciSrcType=1;
		StrCopy(pSketchImage->szImgFile,this->szSketchImgFile,51);
	}
	return true;
}
void CArmHeadComTempl::InitComponentTempl(TEMPL_SOURCE* pTmplSource/*=NULL*/)
{
	if(pTmplSource!=NULL&&pTmplSource->ciSourceType!=0)
		return ;	//暂时不支持由指定文件来确定模板
	CATLOG_ITEM* pPosiParams=hashCategoryItems.Add(3);
	pPosiParams->name.Copy("装配位置");
	CATLOG_ITEM* pLevel1Params=hashCategoryItems.Add(1);
	pLevel1Params->name.Copy("一级尺寸设计参数");
	CATLOG_ITEM* pLevel2Params=hashCategoryItems.Add(2);
	pLevel2Params->name.Copy("二级尺寸设计参数");
	if(pTmplSource==NULL||pTmplSource->idStdComTempl==1)
	{
		name="HX1";
		szSketchImgFile="D:\\HYJ-1.bmp";
		m_idWireStruTmplType=1;
		//1.标准组件设计参数定义区
		STDTEMPL_PARAM_ITEM* pParam=listParams.Add(KEY4C("X1"));
		//一类参数，杆塔规划时设计人员关心的直接尺寸参数
		pParam->sLabel.Copy("X1");
		pParam->sNotes.Copy("地线挂点X向到塔心轴线距离, mm");
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;//ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
		pParam->value.fValue=10400;
		pParam->uiCategoryId=pLevel1Params->Id;
		pParam=listParams.Add(KEY4C("X2"));
		pParam->sLabel.Copy("X2");
		pParam->sNotes.Copy("导线挂点X向到塔心轴线距离, mm");
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;//ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
		pParam->value.fValue=7700;
		pParam->uiCategoryId=pLevel1Params->Id;
		pParam=listParams.Add(KEY4C("Y1"));
		pParam->sLabel.Copy("Y1");
		pParam->sNotes.Copy("地线挂点Y向前后开口, mm");
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;//ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
		pParam->value.fValue=600;
		pParam->uiCategoryId=pLevel1Params->Id;
		pParam=listParams.Add(KEY4C("Y2"));
		pParam->sLabel.Copy("Y2");
		pParam->sNotes.Copy("导线挂点Y向前后开口, mm");
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
		pParam->value.fValue=600;
		pParam->uiCategoryId=pLevel1Params->Id;
		pParam=listParams.Add(KEY4C("Z1"));
		pParam->sLabel.Copy("Z1");
		pParam->sNotes.Copy("导地线挂点间Z向垂高距离, mm");
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
		pParam->value.fValue=2300;
		pParam->uiCategoryId=pLevel1Params->Id;
		pParam=listParams.Add(KEY4C("Z2"));
		pParam->sLabel.Copy("Z2");
		pParam->sNotes.Copy("导线挂点到塔身顶平面间Z向垂高距离(低于塔顶时为负值), mm");
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
		pParam->value.fValue=600;
		pParam->uiCategoryId=pLevel1Params->Id;
		//二类参数，杆塔规划时设计人员不直接关心，但对塔头或横担外形有直接控制作用的尺寸参数
		pParam=listParams.Add(KEY4C("X3"));
		pParam->sLabel.Copy("X3");
		pParam->sNotes.Copy("地线挂点到底部梁中间支撑点N60间X向投影距离, mm");
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;//ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
		pParam->value.fValue=800;
		pParam->uiCategoryId=pLevel2Params->Id;
		//pParam=listParams.Add(8);
		//pParam->sLabel.Copy("X4");
		//pParam->sNotes.Copy("导线挂点到挂架底部梁与曲臂连接点N80间X向投影距离, mm");
		//pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;//ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
		//pParam->value.fValue=600;
		pParam=listParams.Add(KEY4C("Z3"));
		pParam->sLabel.Copy("Z3");
		pParam->sNotes.Copy("地线小挂架高度, mm");
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;//ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
		pParam->value.fValue=600;
		pParam->uiCategoryId=pLevel2Params->Id;
		pParam=listParams.Add(KEY4C("Z4"));
		pParam->sLabel.Copy("Z4");
		pParam->sNotes.Copy("导线小挂架高度, mm");
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;//ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
		pParam->value.fValue=600;
		pParam->uiCategoryId=pLevel2Params->Id;
		//2.标准组件节点定义区
		CStdTemplParaNode* pParaNode;
		pParaNode=listParaNodes.Add(1);	//N10	塔身上侧装配节点
		pParaNode=listParaNodes.Add(2);	//N20	塔身下侧装配节点
		pParaNode=listParaNodes.Add(TMPLOBJID(1,'X'));	//N12	塔身上侧装配节点
		pParaNode=listParaNodes.Add(TMPLOBJID(2,'X'));	//N22	塔身下侧装配节点
		pParaNode=listParaNodes.Add(KEY4C("N30"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::INDEPENDENT;
		strcpy(pParaNode->posx_expression,"X1");
		strcpy(pParaNode->posy_expression,"Y1/2");
		strcpy(pParaNode->posz_expression,"NODE_Z(1)-Z1-Z2");	//NODE_Z(1)=N10_Z
		pParaNode=listParaNodes.Add(KEY4C("N32"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::INDEPENDENT;
		strcpy(pParaNode->posx_expression,"X1");
		strcpy(pParaNode->posy_expression,"-Y1/2");
		strcpy(pParaNode->posz_expression,"NODE_Z(1)-Z1-Z2");
		pParaNode=listParaNodes.Add(KEY4C("N40"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::INDEPENDENT;
		strcpy(pParaNode->posx_expression,"X2");
		strcpy(pParaNode->posy_expression,"Y2/2");
		strcpy(pParaNode->posz_expression,"NODE_Z(1)-Z2");
		pParaNode=listParaNodes.Add(KEY4C("N42"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::INDEPENDENT;
		strcpy(pParaNode->posx_expression,"X2");
		strcpy(pParaNode->posy_expression,"-Y2/2");
		strcpy(pParaNode->posz_expression,"NODE_Z(1)-Z2");
		//次要节点
		//N50
		pParaNode=listParaNodes.Add(KEY4C("N50"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::PLANE_XZ_SPECIFIED;
		strcpy(pParaNode->posx_expression,"X1-X3");
		//strcpy(pParaNode->posy_expression,"Y2/2");
		CXhString(pParaNode->posz_expression,50).Printf("NODE_Z(%d)-Z3",KEY4C("N30"));
		pParaNode->attachNodeArr[0]=KEY4C("N10");
		pParaNode->attachNodeArr[1]=KEY4C("N20");
		pParaNode->attachNodeArr[2]=KEY4C("N30");
		//N52
		pParaNode=listParaNodes.Add(KEY4C("N52"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::PLANE_XZ_SPECIFIED;
		pParaNode->ciMirTransType='X';
		strcpy(pParaNode->posx_expression,"X1-X3");
		//strcpy(pParaNode->posy_expression,"Y2/2");
		CXhString(pParaNode->posz_expression,50).Printf("NODE_Z(%d)-Z3",KEY4C("N30"));
		pParaNode->attachNodeArr[0]=KEY4C("N10");
		pParaNode->attachNodeArr[1]=KEY4C("N20");
		pParaNode->attachNodeArr[2]=KEY4C("N30");
		//N60
		pParaNode=listParaNodes.Add(KEY4C("N60"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::COORD_Z_SPECIFIED;
		CXhString(pParaNode->posz_expression,50).Printf("NODE_Z(%d)",KEY4C("N30"));
		pParaNode->attachNodeArr[0]=KEY4C("N50");
		pParaNode->attachNodeArr[1]=KEY4C("N10");
		//N62
		pParaNode=listParaNodes.Add(KEY4C("N62"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::COORD_Z_SPECIFIED;
		CXhString(pParaNode->posz_expression,50).Printf("NODE_Z(%d)",KEY4C("N30"));
		pParaNode->attachNodeArr[0]=KEY4C("N52");
		pParaNode->attachNodeArr[1]=KEY4C("N10");
		pParaNode->attachNodeArr[1].ciMirTransType='X';
		//N70
		pParaNode=listParaNodes.Add(KEY4C("N70"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::COORD_X_SPECIFIED;
		strcpy(pParaNode->posx_expression,"X1-X3");
		pParaNode->attachNodeArr[0]=KEY4C("N30");
		pParaNode->attachNodeArr[1]=KEY4C("N60");
		//N72
		pParaNode=listParaNodes.Add(KEY4C("N72"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::COORD_X_SPECIFIED;
		strcpy(pParaNode->posx_expression,"X1-X3");
		pParaNode->attachNodeArr[0]=KEY4C("N32");
		pParaNode->attachNodeArr[1]=KEY4C("N62");
		//N80
		pParaNode=listParaNodes.Add(KEY4C("N80"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::COORD_Z_SPECIFIED;
		strcpy(pParaNode->posz_expression,"NODE_Z(1)-Z2-Z4");
		pParaNode->attachNodeArr[0]=KEY4C("N70");
		pParaNode->attachNodeArr[1]=KEY4C("N20");
		//N82
		pParaNode=listParaNodes.Add(KEY4C("N82"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::COORD_Z_SPECIFIED;
		strcpy(pParaNode->posz_expression,"NODE_Z(1)-Z2-Z4");
		pParaNode->attachNodeArr[0]=KEY4C("N72");
		pParaNode->attachNodeArr[1]=KEY4C("N20");
		pParaNode->attachNodeArr[1].ciMirTransType='X';
		//N90
		pParaNode=listParaNodes.Add(KEY4C("N90"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::COORD_Z_SPECIFIED;
		strcpy(pParaNode->posz_expression,"NODE_Z(1)-Z2");
		pParaNode->attachNodeArr[0]=KEY4C("N70");
		pParaNode->attachNodeArr[1]=KEY4C("N20");
		//N92
		pParaNode=listParaNodes.Add(KEY4C("N92"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::COORD_Z_SPECIFIED;
		strcpy(pParaNode->posz_expression,"NODE_Z(1)-Z2");
		pParaNode->attachNodeArr[0]=KEY4C("N72");
		pParaNode->attachNodeArr[1]=KEY4C("N20");
		pParaNode->attachNodeArr[1].ciMirTransType='X';
		//3.标准组件杆件定义区
		CStdTemplParaRod* pParaRod;
		CStdTemplParaAngle* pParaAngle;
		const int CLS_LINEANGLE=13;
		pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(KEY4C("R1"),CLS_LINEANGLE);
		pParaAngle->keyStartNode=KEY4C("N50");
		pParaAngle->keyEndNode=KEY4C("N10");
		pParaRod=listParaRods.Add(TMPLOBJID(KEY4C("R1"),'X'),CLS_LINEANGLE);
		pParaRod->keyStartNode=KEY4C("N52");//TMPLOBJID(pParaAngle->keyStartNode,'X');
		pParaRod->keyEndNode  =TMPLOBJID(1,'X');//pParaAngle->keyEndNode,'X');

		pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(KEY4C("R2"),CLS_LINEANGLE);
		pParaAngle->keyStartNode=KEY4C("N70");
		pParaAngle->keyEndNode=KEY4C("N20");
		pParaRod=listParaRods.Add(TMPLOBJID(KEY4C("R2"),'X'),CLS_LINEANGLE);
		pParaRod->keyStartNode=KEY4C("N72");//TMPLOBJID(pParaAngle->keyStartNode,'X');
		pParaRod->keyEndNode  =TMPLOBJID(2,'X');//pParaAngle->keyEndNode,'X');

		pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(KEY4C("R3"),CLS_LINEANGLE);
		pParaAngle->keyStartNode=KEY4C("N60");
		pParaAngle->keyEndNode=KEY4C("N30");
		pParaRod=listParaRods.Add(TMPLOBJID(KEY4C("R3"),'X'),CLS_LINEANGLE);
		pParaRod->keyStartNode=KEY4C("N62");//TMPLOBJID(pParaAngle->keyStartNode,'X');
		pParaRod->keyEndNode  =KEY4C("N32");//TMPLOBJID(pParaAngle->keyEndNode,'X');

		pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(KEY4C("R4"),CLS_LINEANGLE);
		pParaAngle->keyStartNode=KEY4C("N50");
		pParaAngle->keyEndNode=KEY4C("N30");
		pParaRod=listParaRods.Add(TMPLOBJID(KEY4C("R4"),'X'),CLS_LINEANGLE);
		pParaRod->keyStartNode=KEY4C("N52");//TMPLOBJID(pParaAngle->keyStartNode,'X');
		pParaRod->keyEndNode  =KEY4C("N32");//TMPLOBJID(pParaAngle->keyEndNode,'X');

		pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(KEY4C("R5"),CLS_LINEANGLE);
		pParaAngle->keyStartNode=KEY4C("N90");
		pParaAngle->keyEndNode=KEY4C("N40");
		pParaRod=listParaRods.Add(TMPLOBJID(KEY4C("R5"),'X'),CLS_LINEANGLE);
		pParaRod->keyStartNode=KEY4C("N92");//TMPLOBJID(pParaAngle->keyStartNode,'X');
		pParaRod->keyEndNode  =KEY4C("N42");//TMPLOBJID(pParaAngle->keyEndNode,'X');

		pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(KEY4C("R6"),CLS_LINEANGLE);
		pParaAngle->keyStartNode=KEY4C("N80");
		pParaAngle->keyEndNode=KEY4C("N40");
		pParaRod=listParaRods.Add(TMPLOBJID(KEY4C("R6"),'X'),CLS_LINEANGLE);
		pParaRod->keyStartNode=KEY4C("N82");//TMPLOBJID(pParaAngle->keyStartNode,'X');
		pParaRod->keyEndNode  =KEY4C("N42");//TMPLOBJID(pParaAngle->keyEndNode,'X');

		//for(pParaRod=listParaRods.GetFirst();pParaRod;pParaRod=listParaRods.GetNext())

		CParaDatumPlane* pPlane=listParaPlanes.Add(1);
		pPlane->label="横担前工作面";
		pPlane->XFirPos.keyRefRod=KEY4C("R1");
		pPlane->XFirPos.bStart=false;
		pPlane->XSecPos.keyRefRod=KEY4C("R2");
		pPlane->XSecPos.bStart=false;
		pPlane->XThirPos.keyRefRod=KEY4C("R3");
		pPlane->XThirPos.bStart=false;
	}
	else if(pTmplSource->idStdComTempl==2)
	{
		name="HX3";
		szSketchImgFile="D:\\HX3.bmp";
		m_idWireStruTmplType=2;
		//1.标准组件设计参数定义区
		STDTEMPL_PARAM_ITEM* pParam=listParams.Add(KEY4C("X"));
		//一类参数，杆塔规划时设计人员关心的直接尺寸参数
		pParam->sLabel.Copy("X");
		pParam->sNotes.Copy("导线挂点X向到塔心轴线距离, mm");
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;//ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
		pParam->value.fValue=10400;
		pParam->uiCategoryId=pLevel1Params->Id;
		pParam=listParams.Add(KEY4C("Y"));
		pParam->sLabel.Copy("Y");
		pParam->sNotes.Copy("导线挂点Y向前后开口, mm");
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
		pParam->value.fValue=600;
		pParam->uiCategoryId=pLevel1Params->Id;
		//二类参数，杆塔规划时设计人员不直接关心，但对塔头或横担外形有直接控制作用的尺寸参数
		pParam=listParams.Add(KEY4C("Z1"));
		pParam->sLabel.Copy("Z1");
		pParam->sNotes.Copy("导线横担高Z向垂高, mm");
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
		pParam->value.fValue=4000;
		pParam->uiCategoryId=pLevel2Params->Id;
		pParam=listParams.Add(KEY4C("Z2"));
		pParam->sLabel.Copy("Z2");
		pParam->sNotes.Copy("导线横担挂点端面垂直高度, mm");
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
		pParam->value.fValue=600;
		pParam->uiCategoryId=pLevel2Params->Id;
		//2.标准组件节点定义区
		CStdTemplParaNode* pParaNode;
		pParaNode=listParaNodes.Add(1);	//N10	塔身上侧装配节点
		pParaNode=listParaNodes.Add(2);	//N20	塔身下侧装配节点
		pParaNode=listParaNodes.Add(TMPLOBJID(1,'X'));	//N12	塔身上侧装配节点
		pParaNode=listParaNodes.Add(TMPLOBJID(2,'X'));	//N22	塔身下侧装配节点
		pParaNode=listParaNodes.Add(KEY4C("N30"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::INDEPENDENT;
		strcpy(pParaNode->posx_expression,"X");
		strcpy(pParaNode->posy_expression,"Y/2");
		strcpy(pParaNode->posz_expression,"NODE_Z(2)");	//NODE_Z(1)=N10_Z
		pParaNode=listParaNodes.Add(KEY4C("N32"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::INDEPENDENT;
		strcpy(pParaNode->posx_expression,"X");
		strcpy(pParaNode->posy_expression,"-Y/2");
		strcpy(pParaNode->posz_expression,"NODE_Z(2)");
		//次要节点
		//N40
		pParaNode=listParaNodes.Add(KEY4C("N40"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::PLANE_XZ_SPECIFIED;
		strcpy(pParaNode->posx_expression,"X");
		//strcpy(pParaNode->posy_expression,"Y/2");
		CXhString(pParaNode->posz_expression,50).Printf("NODE_Z(%d)-Z2",KEY4C("N30"));
		pParaNode->attachNodeArr[0]=KEY4C("N10");
		pParaNode->attachNodeArr[1]=KEY4C("N20");
		pParaNode->attachNodeArr[2]=KEY4C("N30");
		//N42
		pParaNode=listParaNodes.Add(KEY4C("N42"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->ciPosCalType=CStdTemplParaNode::PLANE_XZ_SPECIFIED;
		pParaNode->ciMirTransType='X';
		strcpy(pParaNode->posx_expression,"X");
		//strcpy(pParaNode->posy_expression,"Y/2");
		CXhString(pParaNode->posz_expression,50).Printf("NODE_Z(%d)-Z2",KEY4C("N30"));
		pParaNode->attachNodeArr[0]=KEY4C("N10");
		pParaNode->attachNodeArr[1]=KEY4C("N20");
		pParaNode->attachNodeArr[2]=KEY4C("N30");
		//3.标准组件杆件定义区
		CStdTemplParaRod* pParaRod;
		CStdTemplParaAngle* pParaAngle;
		const int CLS_LINEANGLE=13;
		pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(KEY4C("R1"),CLS_LINEANGLE);
		pParaAngle->keyStartNode=KEY4C("N20");
		pParaAngle->keyEndNode=KEY4C("N30");
		pParaRod=listParaRods.Add(TMPLOBJID(KEY4C("R1"),'X'),CLS_LINEANGLE);
		pParaRod->keyStartNode=KEY4C("N22");//TMPLOBJID(pParaAngle->keyStartNode,'X');
		pParaRod->keyEndNode  =TMPLOBJID(1,'X');//pParaAngle->keyEndNode,'X');

		pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(KEY4C("R2"),CLS_LINEANGLE);
		pParaAngle->keyStartNode=KEY4C("N10");
		pParaAngle->keyEndNode=KEY4C("N40");
		pParaRod=listParaRods.Add(TMPLOBJID(KEY4C("R2"),'X'),CLS_LINEANGLE);
		pParaRod->keyStartNode=TMPLOBJID(1,'X');//TMPLOBJID(pParaAngle->keyStartNode,'X');
		pParaRod->keyEndNode  =KEY4C("N42");

		pParaAngle=(CStdTemplParaAngle*)listParaRods.Add(KEY4C("R3"),CLS_LINEANGLE);
		pParaAngle->keyStartNode=KEY4C("N40");
		pParaAngle->keyEndNode=KEY4C("N30");
		pParaRod=listParaRods.Add(TMPLOBJID(KEY4C("R3"),'X'),CLS_LINEANGLE);
		pParaRod->keyStartNode=KEY4C("N42");//TMPLOBJID(pParaAngle->keyStartNode,'X');
		pParaRod->keyEndNode  =KEY4C("N32");//TMPLOBJID(pParaAngle->keyEndNode,'X');
		pParaRod=listParaRods.Add(KEY4C("R4"),CLS_LINEANGLE);
		pParaRod->keyStartNode=KEY4C("N30");//TMPLOBJID(pParaAngle->keyStartNode,'X');
		pParaRod->keyEndNode  =KEY4C("N32");//TMPLOBJID(pParaAngle->keyEndNode,'X');
		pParaRod=listParaRods.Add(KEY4C("R5"),CLS_LINEANGLE);
		pParaRod->keyStartNode=KEY4C("N40");//TMPLOBJID(pParaAngle->keyStartNode,'X');
		pParaRod->keyEndNode  =KEY4C("N42");//TMPLOBJID(pParaAngle->keyEndNode,'X');

		CParaDatumPlane* pPlane=listParaPlanes.Add(1);
		pPlane->label="横担前工作面";
		pPlane->XFirPos.keyRefRod=KEY4C("R1");
		pPlane->XFirPos.bStart=true;
		pPlane->XThirPos.keyRefRod=KEY4C("R1");
		pPlane->XThirPos.bStart=false;
		pPlane->XSecPos.keyRefRod=KEY4C("R2");
		pPlane->XSecPos.bStart=false;
	}
	if(szSketchImgFile.GetLength()>0)
		this->sketchimg.LoadBmpFile(szSketchImgFile);
	return;
}
bool CArmHeadComTempl::IsMeetConditions()
{
	return true;
}
long CArmHeadComTempl::MappingLdsNodeH(long idTemplNode)
{
	if(idTemplNode==KEY4C("N10"))
		idTemplNode=1;
	else if(idTemplNode==KEY4C("N20"))
		idTemplNode=2;
	else if(idTemplNode==KEY4C("N30"))
		idTemplNode=3;
	else if(idTemplNode==KEY4C("N40"))
		idTemplNode=4;
	else if(idTemplNode==KEY4C("N50"))
		idTemplNode=5;
	long *phLdsNode=hashMatchNodeHid.GetValue(idTemplNode);
	if(phLdsNode)
		return *phLdsNode;
	else
		return 0;
}
long CArmHeadComTempl::SetMappingNode(long idTemplNode,long hLdsNode)
{
	if(idTemplNode==KEY4C("N10"))
		idTemplNode=1;
	else if(idTemplNode==KEY4C("N20"))
		idTemplNode=2;
	else if(idTemplNode==KEY4C("N30"))
		idTemplNode=3;
	else if(idTemplNode==KEY4C("N40"))
		idTemplNode=4;
	else if(idTemplNode==KEY4C("N50"))
		idTemplNode=5;
	return CStdComTempl::SetMappingNode(idTemplNode,hLdsNode);
}
bool CArmHeadComTempl::SetMappingNode(long idTemplNode,CLDSNode *pNode)
{
	if(idTemplNode==KEY4C("N10"))
		idTemplNode=1;
	else if(idTemplNode==KEY4C("N20"))
		idTemplNode=2;
	else if(idTemplNode==KEY4C("N30"))
		idTemplNode=3;
	else if(idTemplNode==KEY4C("N40"))
		idTemplNode=4;
	else if(idTemplNode==KEY4C("N50"))
		idTemplNode=5;
	return CStdComTempl::SetMappingNode(idTemplNode,pNode);
}
//////////////////////////////////////////////////////////////////////////
//
#ifdef __COMMON_PART_INC_
void MAP_RODnNODE::Set(long idTmplObj/*=0*/,CLDSDbObject* pMapLdsObj/*=NULL*/)
{
	ciCondGroup=0;
	if(idTmplObj==0||pMapLdsObj==NULL)
	{
		idNodeOrRod=ciNode0Rod1=0;
		pNode=NULL;
	}
	if(pMapLdsObj&&pMapLdsObj->IsNode())
	{
		ciNode0Rod1=0;
		idNodeOrRod=idTmplObj;
		pNode=(CLDSNode*)pMapLdsObj;
	}
	else if(pMapLdsObj&&pMapLdsObj->IsLinePart())
	{
		ciNode0Rod1=1;
		idNodeOrRod=idTmplObj;
		pRod=(CLDSLinePart*)pMapLdsObj;
	}
	else
	{
		idNodeOrRod=ciNode0Rod1=0;
		pNode=NULL;
	}
}
WIREPOINT_GROUP::WIREPOINT_GROUP()
{
	ciGroupSerial=0;					//挂点分组号
	ciResidePlane=0;
	ciFittingType=CLDSFitting::TYPE_EB;	//金具类型
	cnShaftD=36;			//金具耳轴直径,mm
	_cnHoleD=0;
	wnFittingWidth=80;		//挂接金具宽度
	_wnReservedWidth=0;
	wiInsulatorStyle=0;		//挂接串型，如单(I)串、双串(单挂)、双串双挂、V串
	strcpy(szFittingSizeCode,"EB-16/21-100");	//金具型号
	strcpy(szUringSizeCode,"U-16");
	cnWirePointCount=1;		//挂点(孔)数量
	ciLocateStyle=0;		//挂点定位类型0.从挂线角钢中点往两侧对称布置，依赖开档距离定位;1.挂点与附近横担角钢间的干涉间隙
	wiSpaceL=400;		//挂孔间开档距离,与wiClearance同
	sfMasterSwingAngle=sfSlaveUrSwingAngle=0.0f;
}

CStdComTemplWireStructure::CStdComTemplWireStructure()
{
	m_cnFixWireAngleConnBoltsN=4;
	m_cnWireGroupCount=1;
}
void CStdComTemplWireStructure::FromBuffer(BUFFER_IO* pIO,DWORD version/*=0*/)
{
	CStdComTempl::FromBuffer(pIO,version);
	pIO->ReadByte(&m_cnFixWireAngleConnBoltsN);	//固接挂线短角钢的连接螺栓数(因连接两个基座角钢，所以必须是偶数）
	pIO->ReadByte(&m_cnWireGroupCount);			//当前挂线架的挂线角钢上挂点分组数量
	for (int i=0;i<m_cnWireGroupCount;i++)
	{
		pIO->ReadByte(&xarrWireGroups[i].ciGroupSerial);			//挂点分组号
		pIO->ReadByte(&xarrWireGroups[i].ciFittingType);			//金具类型0.EB挂板|1.GD挂板|2.U形螺栓
		pIO->ReadByte(&xarrWireGroups[i].cnShaftD);				//金具耳轴直径,mm
		xarrWireGroups[i].cnHoleD=pIO->ReadByte();				//预留安装孔径,mm，一般cnHoleD=cnShaftD+2
		pIO->ReadWord(&xarrWireGroups[i].wnFittingWidth);		//挂接金具宽度
		xarrWireGroups[i].wnReservedWidth=pIO->ReadWord();		//金具预留宽度,mm，一般wnReservedWidth=wnFittingWidth+2
		pIO->ReadWord(&xarrWireGroups[i].wiInsulatorStyle);		//挂接串型，如单(I)串、双串(单挂)、双串双挂、V串
		pIO->ReadString(xarrWireGroups[i].szFittingSizeCode,17);	//金具型号
		pIO->ReadString(xarrWireGroups[i].szUringSizeCode,17);	//EB/GD金具下的挂接U形环型号
		pIO->ReadByte(&xarrWireGroups[i].cnWirePointCount);		//挂点(孔)数量
		pIO->ReadByte(&xarrWireGroups[i].ciResidePlane);			//0.当前工作平面（挂线角钢工作肢或钢板平面）;1.挂线角钢另一肢
		pIO->ReadByte(&xarrWireGroups[i].ciLocateStyle);			//挂点定位类型0.从挂线角钢中点往两侧对称布置，依赖开档距离定位;1.挂点与附近横担角钢间的干涉间隙
		pIO->ReadWord(&xarrWireGroups[i].wiSpaceL);//挂孔间开档距离;或wiClearance;	//干涉间隙
		pIO->ReadFloat(&xarrWireGroups[i].sfMasterSwingAngle);	//主金具摆动角，度
		pIO->ReadFloat(&xarrWireGroups[i].sfSlaveUrSwingAngle);	//挂接U形环摆动角，度
	}
	DWORD count;
	pIO->ReadDword(&count);
	xarrKeyRodNodes.ReSize(count);
	for (DWORD ii = 0; ii < count; ii++)
	{
		pIO->ReadByte(&xarrKeyRodNodes[ii].ciNode0Rod1);
		pIO->ReadByte(&xarrKeyRodNodes[ii].ciCondGroup);
		pIO->ReadInteger(&xarrKeyRodNodes[ii].idNodeOrRod);
	}
}
void CStdComTemplWireStructure::ToBuffer(BUFFER_IO* pIO,DWORD version/*=0*/)
{
	CStdComTempl::ToBuffer(pIO,version);
	pIO->WriteByte(m_cnFixWireAngleConnBoltsN);	//固接挂线短角钢的连接螺栓数(因连接两个基座角钢，所以必须是偶数）
	pIO->WriteByte(m_cnWireGroupCount);			//当前挂线架的挂线角钢上挂点分组数量
	for (int i=0;i<m_cnWireGroupCount;i++)
	{
		pIO->WriteByte(xarrWireGroups[i].ciGroupSerial);			//挂点分组号
		pIO->WriteByte(xarrWireGroups[i].ciFittingType);			//金具类型0.EB挂板|1.GD挂板|2.U形螺栓
		pIO->WriteByte(xarrWireGroups[i].cnShaftD);				//金具耳轴直径,mm
		pIO->WriteByte(xarrWireGroups[i].cnHoleD);				//预留安装孔径,mm，一般cnHoleD=cnShaftD+2
		pIO->WriteWord(xarrWireGroups[i].wnFittingWidth);		//挂接金具宽度
		pIO->WriteWord(xarrWireGroups[i].wnReservedWidth);		//金具预留宽度,mm，一般wnReservedWidth=wnFittingWidth+2
		pIO->WriteWord(xarrWireGroups[i].wiInsulatorStyle);		//挂接串型，如单(I)串、双串(单挂)、双串双挂、V串
		pIO->WriteString(xarrWireGroups[i].szFittingSizeCode,17);	//金具型号
		pIO->WriteString(xarrWireGroups[i].szUringSizeCode,17);	//EB/GD金具下的挂接U形环型号
		pIO->WriteByte(xarrWireGroups[i].cnWirePointCount);		//挂点(孔)数量
		pIO->WriteByte(xarrWireGroups[i].ciResidePlane);			//0.当前工作平面（挂线角钢工作肢或钢板平面）;1.挂线角钢另一肢
		pIO->WriteByte(xarrWireGroups[i].ciLocateStyle);			//挂点定位类型0.从挂线角钢中点往两侧对称布置，依赖开档距离定位;1.挂点与附近横担角钢间的干涉间隙
		pIO->WriteWord(xarrWireGroups[i].wiSpaceL);//挂孔间开档距离;或wiClearance;	//干涉间隙
		pIO->WriteFloat(xarrWireGroups[i].sfMasterSwingAngle);	//主金具摆动角，度
		pIO->WriteFloat(xarrWireGroups[i].sfSlaveUrSwingAngle);	//挂接U形环摆动角，度
	}
	//
	pIO->WriteDword(xarrKeyRodNodes.Count);
	for (UINT ii = 0; ii < xarrKeyRodNodes.Count; ii++)
	{
		pIO->WriteByte(xarrKeyRodNodes[ii].ciNode0Rod1);
		pIO->WriteByte(xarrKeyRodNodes[ii].ciCondGroup);
		pIO->WriteInteger(xarrKeyRodNodes[ii].idNodeOrRod);
	}
}
bool CStdComTemplWireStructure::CloneStdTmplFrom(IStdComTempl* pStdComTmpl)
{
	return CStdComTempl::CloneStdTmplFrom(pStdComTmpl);
}
void CStdComTemplWireStructure::InitComponentTempl(TEMPL_SOURCE* pTemplSource/*=NULL*/)
{
	TEMPL_SOURCE source(0,IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM1);
	if(pTemplSource==NULL)
		pTemplSource=&source;
	if (pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM1)
		InitComTempl_EB_DANGLE_EARTH_WIRE_ARM1();
	else if (pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM2)
		InitComTempl_EB_DANGLE_EARTH_WIRE_ARM2();
	else if (pTemplSource->idStdComTempl == IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM3)
		InitComTempl_EB_DANGLE_EARTH_WIRE_ARM3();
	else if (pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_EB_EARTH_WIRE_ARM)
		InitComTempl_EB_EARTH_WIRE_ARM();
	else if (pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_EB_EARTH_WIRE_PLATE)
		InitComTempl_EB_EARTH_WIRE_PLATE();
	else if (pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM1)
		InitComTempl_EB_DANGLE_WIRE_I_ARM1();
	else if (pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM2)
		InitComTempl_EB_DANGLE_WIRE_I_ARM2();
	else if (pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM3)
		InitComTempl_EB_DANGLE_WIRE_I_ARM3();
	else if (pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_EB_WIRE_V_ARM1)
		InitComTempl_EB_WIRE_V_ARM1();
	else if (pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_EB_WIRE_V_ARM2)
		InitComTempl_EB_WIRE_V_ARM2();
	else if (pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_EB_WIRE_V_ARM3)
		InitComTempl_EB_WIRE_V_ARM3();
	else if (pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_EB_WIRE_V_ARM4)
		InitComTempl_EB_WIRE_V_ARM4();
	else if(pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_EB_WIRE_V_ARM5)
		InitComTempl_EB_WIRE_V_ARM5();
	else if(pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_EB_WIRE_V_ARM6)
		InitComTempl_EB_WIRE_V_ARM6();
	else if(pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_EB)
		InitComTempl_META_SECT_BTM_1_EB();
	else if(pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_2_EB)
		InitComTempl_META_SECT_BTM_2_EB();
	else if(pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_META_SECT_FRT_1_GD)
		InitComTempl_META_SECT_FRT_1_GD();
	else if(pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_GD)
		InitComTempl_META_SECT_BTM_1_GD();
	/*else if(pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_V_OUTER_1_EB)
	{
		ClearTmplParams();
		name.Copy("直线塔V串边相单EB挂点");
		szSketchImgFile.Copy("CZC-EB1.bmp");
	}
	else if(pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_V_OUTER_2_EB)
	{
		ClearTmplParams();
		name.Copy("直线塔V串边相双EB挂点");
		szSketchImgFile.Copy("CZC-EB2.bmp");
	}*/
	else if(pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_NZ_FRT_2_GD)
		InitComTempl_NZ_FRT_2_GD();
	else if(pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_NZ_BTM_2_GD)
		InitComTempl_NZ_BTM_2_GD();
	else if(pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_NZ_BTM_3_GD)
		InitComTempl_NZ_BTM_3_GD();
	/*else if(pTemplSource->idStdComTempl==IStdComTempl::STDTMPL_STRU_NZ_FRT_3_GD)
	{	经与李美峰核实，实际工程中不存在正面叁个GD挂点的情况
		ClearTmplParams();
		name.Copy("耐张塔正面叁GD挂点");
		szSketchImgFile.Copy("CNZ-GD2.bmp");
	}*/
}
bool CStdComTemplWireStructure::ClearTmplParams()
{
	//TODO:以后应判断是否需要初始化参数默认值 wjh-2019.10.31
	this->hashMatchNodeHid.Empty();
	this->hashMatchRodHid.Empty();
	this->hashMatchPlates.Empty();
	for(CStdTemplParaNode* pParaNode=listParaNodes.GetFirst();pParaNode;pParaNode=listParaNodes.GetNext())
		pParaNode->pRelaNode=NULL;
	for(CStdTemplParaRod* pParaRod=listParaRods.GetFirst();pParaRod;pParaRod=listParaRods.GetNext())
		pParaRod->pRelaRod=NULL;
	return true;
}
bool CStdComTemplWireStructure::IsMeetConditions()
{	//是否满足当前挂点结构的基本设计条件 wjh-2019.11.24
	CStdTemplParaNode* pParaNode;
	CStdTemplParaRod* pParaRod;
	for(pParaNode=this->listParaNodes.GetFirst();pParaNode;pParaNode=listParaNodes.GetNext())
	{
		if(pParaNode->m_bNecessaryNode&&hashMatchNodeHid.GetValue(pParaNode->Id)==NULL)
			return false;
	}
	for(pParaRod=this->listParaRods.GetFirst();pParaRod;pParaRod=listParaRods.GetNext())
	{
		if(pParaRod->m_bNecessaryRod&&hashMatchRodHid.GetValue(pParaRod->Id)==NULL)
			return false;
	}
	return true;
}
extern CTower Ta;
bool CStdComTemplWireStructure::MeetRecogKeyObjs(MAP_RODnNODE* parrMapObjs/*=NULL*/,long count/*=1*/,
		MAP_RODnNODE* parrMeetMapObjs/*=NULL*/,long lnMaxMeetCount/*=4*/)
{	//判断当前已匹配数据对象（关键节点或杆件）是否满足挂线架的其余数据对象识别 wjh-2019.11.24
	PRESET_ARRAY4<MAP_RODnNODE> keyobjs;
	UINT i,j,cnMeetObjs=0,matchcount=0;
	for(int groupserial=0;groupserial<=2;groupserial++)
	{
		keyobjs.Clear();
		matchcount=0;
		for(j=0;j<xarrKeyRodNodes.Count;j++)
		{
			if(this->xarrKeyRodNodes[j].ciCondGroup!=groupserial)
				continue;
			else if(this->xarrKeyRodNodes[j].idNodeOrRod>0)
				keyobjs.Append(this->xarrKeyRodNodes[j]);
		}
		cnMeetObjs=0;
		for(i=0;i<(UINT)count;i++)
		{
			if(parrMapObjs[i].ciCondGroup!=groupserial)
				continue;
			else if(parrMapObjs[i].idNodeOrRod==0)
				break;
			for(j=0;j<keyobjs.Count;j++)
			{
				MAP_RODnNODE* pCurrKeyObj=keyobjs.GetAt(j);
				if(pCurrKeyObj->ciNode0Rod1==parrMapObjs[i].ciNode0Rod1&&pCurrKeyObj->idNodeOrRod==parrMapObjs[i].idNodeOrRod)
				{
					pCurrKeyObj->pNode=parrMapObjs[i].pNode;
					cnMeetObjs++;
					break;
				}
			}
			if(cnMeetObjs==keyobjs.Count)
				break;
		}
		if(cnMeetObjs==keyobjs.Count)
			break;
	}
	if(cnMeetObjs!=keyobjs.Count)
		return false;	//不满足该项条件
	if(parrMeetMapObjs)
	{
		for(i=0;i<keyobjs.Count&&i<(UINT)lnMaxMeetCount;i++)
			parrMeetMapObjs[i]=keyobjs.At(i);
	}
	return true;
}
bool CStdComTemplWireStructure::SetRecogKeyNodesAndRods(MAP_RODnNODE* parrMapObjs/*=NULL*/,long count/*=1*/)
{
	PRESET_ARRAY4<MAP_RODnNODE> keyobjs;
	if (!MeetRecogKeyObjs(parrMapObjs, count, keyobjs.pPresetObjs, 4))
		return false;	//不满足该项条件
	for (int i = 0; i < 4 && keyobjs[i].idNodeOrRod>0; i++)
		continue;
	keyobjs.SetSize(i);
	//根据关键节点或杆件查找关联节点和杆件
	CLDSLinePart* pRod=NULL;
	SmartRodPtr pSmartRod;
	if( this->m_idStdComTmpl==STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM1||
		this->m_idStdComTmpl==STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM2)
	{
		MAP_RODnNODE mapobj = keyobjs[0];
		if (mapobj.ciNode0Rod1 == MAP_RODnNODE::TYPE_NODE)
		{
			CStdComTempl::SetMappingNode(mapobj.idNodeOrRod, mapobj.pNode);
			//查找节点N20
			CLDSNode* pNode10 = mapobj.pNode, *pNode20 = NULL, *pNode30 = NULL, *pNode40 = NULL;
			if (pNode10->xOriginalPos.x > 0)	
			{	//右侧横担
				CLDSNode* pTipNode = pNode10;
				for (CLDSNode* pNode = Ta.Node.GetFirst(); pNode; pNode = Ta.Node.GetNext())
				{
					if (pNode->xOriginalPos.x > pTipNode->xOriginalPos.x&&
						pNode->xOriginalPos.y > 0 &&//fabs(pNode->xOriginalPos.y-pTipNode->xOriginalPos.y)<EPS&&
						fabs(pNode->xOriginalPos.z - pNode10->xOriginalPos.z) < EPS)
						pTipNode = pNode;
				}
				if (pTipNode != pNode10)
					pNode20 = pTipNode;
			}
			else if (pNode10->xOriginalPos.x < 0)
			{	//左侧横担
				CLDSNode* pTipNode = pNode10;
				for (CLDSNode* pNode = Ta.Node.GetFirst(); pNode; pNode = Ta.Node.GetNext())
				{
					if (pNode->xOriginalPos.x < pTipNode->xOriginalPos.x&&
						pNode->xOriginalPos.y > 0 &&fabs(pNode->xOriginalPos.z - pNode10->xOriginalPos.z) < EPS)
						pTipNode = pNode;
				}
				if (pTipNode != pNode10)
					pNode20 = pTipNode;
			}
			if (pNode20 == NULL)
				return false;
			CStdComTempl::SetMappingNode(2, pNode20);
			RecogMatchRods(2);
			//添加N20对称节点，用于识别T型组合角钢(4号杆件)
			CLDSNode* pMirNodeX = pNode20->GetMirNode(MIRMSG(1));
			if (pMirNodeX)
			{
				CStdTemplParaNode* pMirParaNode = listParaNodes.Add(TMPLOBJID(2, 'X'));
				CStdComTempl::SetMappingNode(TMPLOBJID(2, 'X'), pMirNodeX->handle);
				RecogMatchRods(4);
			}
			//查找地线支架顶部节点N30
			CLDSNode* pTopNode = pNode10;
			for (CLDSNode* pNode = Ta.Node.GetFirst(); pNode; pNode = Ta.Node.GetNext())
			{
				CLDSNode* pLeftNode = pNode10, *pRightNode = pNode20;
				if (pNode10->xOriginalPos.x < 0)	
				{	//左侧横担节点
					pLeftNode = pNode20;
					pRightNode = pNode10;
				}
				if (pNode->xOriginalPos.x > pLeftNode->xOriginalPos.x&&
					pNode->xOriginalPos.x < pRightNode->xOriginalPos.x && (
					(pNode->xOriginalPos.z == pTopNode->xOriginalPos.z&&pNode->xOriginalPos.y > pTopNode->xOriginalPos.y) ||
						pNode->xOriginalPos.z < pTopNode->xOriginalPos.z))
				{
					pTopNode = pNode;
				}
			}
			if (pTopNode == pNode10)
				return false;
			pNode30 = pTopNode;
			CStdComTempl::SetMappingNode(3, pNode30);
			RecogMatchRods(1);
			RecogMatchRods(5);
			//查找竖直拉杆节点N40
			CPtInLineCheck check(pNode10->xOriginalPos, pNode20->xOriginalPos);
			for (pNode = Ta.Node.GetFirst(); pNode; pNode = Ta.Node.GetNext())
			{
				bool bOnLine = false;
				double fraction = check.CheckPointEx(pNode->xOriginalPos, &bOnLine, 1);
				if (!bOnLine || fraction<0.2 || fraction>0.8)
					continue;
				pNode40 = pNode;
				break;
			}
			if (pNode40 == NULL)
				return false;
			CStdComTempl::SetMappingNode(4, pNode40);
			return RecogMatchRods(3)>0;
		}
	}
	else if (this->m_idStdComTmpl == STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM3)
	{
		MAP_RODnNODE mapobj = keyobjs[0];
		if (mapobj.ciNode0Rod1 == MAP_RODnNODE::TYPE_NODE)
		{
			CStdComTempl::SetMappingNode(mapobj.idNodeOrRod, mapobj.pNode);
			//查找横担杆件和拉杆
			CLDSNode* pKeyNode = mapobj.pNode;
			CMaxDouble maxcosa(0), maxupgradient;
			for (pRod = Ta.EnumRodFirst(); pRod; pRod = Ta.EnumRodNext())
			{
				pSmartRod = pRod;
				if (pRod->pStart == NULL || pRod->pEnd == NULL)
					continue;
				else if (pSmartRod->GetClassTypeId() == CLS_LINEANGLE && pSmartRod.pAngle->group_father_jg_h > 0)
					continue;
				if (pRod->pStart == pKeyNode || pRod->pEnd == pKeyNode)
				{
					GEPOINT lenvec = pRod->pEnd->xOriginalPos - pRod->pStart->xOriginalPos;
					if (pRod->pEnd == pKeyNode)
						lenvec *= -1.0;
					normalize(lenvec);
					maxcosa.Update(fabs(lenvec.x), pRod);
					maxupgradient.Update(fabs(lenvec.z), pRod);
				}
			}
			if (maxcosa.m_pRelaObj == NULL || maxupgradient.m_pRelaObj == NULL)
				return false;
			//水平横担杆件
			SmartRodPtr pHoriRod = (CLDSLinePart*)maxcosa.m_pRelaObj;
			if (pHoriRod->pStart == pKeyNode)
				CStdComTempl::SetMappingNode(2, pHoriRod->pEnd);
			else if (pHoriRod->pEnd == pKeyNode)
				CStdComTempl::SetMappingNode(2, pHoriRod->pStart);
			this->hashMatchRodHid.SetValue(1, pHoriRod->handle);
			//横担拉杆 
			SmartRodPtr pTensionRod = (CLDSLinePart*)maxupgradient.m_pRelaObj;
			if (pTensionRod->pStart == pKeyNode)
				CStdComTempl::SetMappingNode(3, pTensionRod->pEnd);
			else if (pTensionRod->pEnd == pKeyNode)
				CStdComTempl::SetMappingNode(3, pTensionRod->pStart);
			this->hashMatchRodHid.SetValue(2, pTensionRod->handle);
			//通过智能识别地线支架底梁杆件
			return RecogMatchRods(R_W_T) > 0;	//识别前后侧贯穿水平挂线角钢
		}
	}
	else if(this->m_idStdComTmpl==STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM1)
	{
		MAP_RODnNODE mapobj= keyobjs[0];
		if(mapobj.ciNode0Rod1==MAP_RODnNODE::TYPE_NODE)
		{
			CStdComTempl::SetMappingNode(mapobj.idNodeOrRod, mapobj.pNode);
			RecogMatchRods(4);
			//查找横担杆件和拉杆
			CLDSNode* pKeyNode = mapobj.pNode;
			CMaxDouble maxcosa(0),maxupgradient;
			for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
			{
				pSmartRod=pRod;
				if(pRod->pStart==NULL||pRod->pEnd==NULL)
					continue;
				else if(pSmartRod->GetClassTypeId()==CLS_LINEANGLE&&pSmartRod.pAngle->group_father_jg_h>0)
					continue;
				if(pRod->pStart==pKeyNode||pRod->pEnd==pKeyNode)
				{
					GEPOINT lenvec=pRod->pEnd->xOriginalPos-pRod->pStart->xOriginalPos;
					if(pRod->pEnd==pKeyNode)
						lenvec*=-1.0;
					normalize(lenvec);
					maxcosa.Update(fabs(lenvec.x),pRod);
					maxupgradient.Update(-lenvec.z,pRod);
				}
			}
			if (maxcosa.m_pRelaObj == NULL || maxupgradient.m_pRelaObj == NULL)
				return false;
			//找到水平横担杆件
			SmartRodPtr pHoriRod=(CLDSLinePart*)maxcosa.m_pRelaObj;
			if(pHoriRod->pStart==pKeyNode)
				CStdComTempl::SetMappingNode(2,pHoriRod->pEnd);
			else if(pHoriRod->pEnd==pKeyNode)
				CStdComTempl::SetMappingNode(2,pHoriRod->pStart);
			this->hashMatchRodHid.SetValue(1,pHoriRod->handle);
			//找到横担拉杆
			SmartRodPtr pTensionRod=(CLDSLinePart*)maxupgradient.m_pRelaObj;
			if(pTensionRod->pStart==pKeyNode)
				CStdComTempl::SetMappingNode(3,pTensionRod->pEnd);
			else if(pTensionRod->pEnd==pKeyNode)
				CStdComTempl::SetMappingNode(3,pTensionRod->pStart);
			this->hashMatchRodHid.SetValue(2,pTensionRod->handle);
		}
	}
	else if (this->m_idStdComTmpl == STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM2)
	{
		MAP_RODnNODE mapobj = keyobjs[0];
		if (mapobj.ciNode0Rod1 == MAP_RODnNODE::TYPE_NODE)
		{
			CStdComTempl::SetMappingNode(mapobj.idNodeOrRod, mapobj.pNode);
			//查找水平横担
			CLDSNode* pKeyNode = mapobj.pNode;
			CMaxDouble maxcosa(0);
			for (pRod = Ta.EnumRodFirst(); pRod; pRod = Ta.EnumRodNext())
			{
				pSmartRod = pRod;
				if (pSmartRod->pStart == NULL || pSmartRod->pEnd == NULL)
					continue;
				else if (pSmartRod->GetClassTypeId() == CLS_LINEANGLE && pSmartRod.pAngle->group_father_jg_h > 0)
					continue;
				if (pSmartRod->pStart == pKeyNode || pSmartRod->pEnd == pKeyNode)
				{
					GEPOINT lenvec = pSmartRod->pEnd->xOriginalPos - pSmartRod->pStart->xOriginalPos;
					if (pSmartRod->pEnd == pKeyNode)
						lenvec *= -1.0;
					normalize(lenvec);
					maxcosa.Update(fabs(lenvec.x), pRod);
				}
			}
			//找到水平横担杆件
			if (maxcosa.m_pRelaObj)			
			{
				SmartRodPtr pHoriRod = (CLDSLinePart*)maxcosa.m_pRelaObj;
				hashMatchRodHid.SetValue(1, pHoriRod->handle);
			}
		}
		mapobj = keyobjs[1];
		if (mapobj.ciNode0Rod1 == MAP_RODnNODE::TYPE_NODE)
		{
			CStdComTempl::SetMappingNode(mapobj.idNodeOrRod, mapobj.pNode);
			RecogMatchRods(3);
			//查找水平横担
			CLDSNode* pKeyNode = mapobj.pNode;
			CMaxDouble maxcosa(0);
			for (pRod = Ta.EnumRodFirst(); pRod; pRod = Ta.EnumRodNext())
			{
				pSmartRod = pRod;
				if (pSmartRod->pStart == NULL || pSmartRod->pEnd == NULL)
					continue;
				else if (pSmartRod->GetClassTypeId() == CLS_LINEANGLE && pSmartRod.pAngle->group_father_jg_h > 0)
					continue;
				if (pSmartRod->pStart == pKeyNode || pSmartRod->pEnd == pKeyNode)
				{
					GEPOINT lenvec = pSmartRod->pEnd->xOriginalPos - pSmartRod->pStart->xOriginalPos;
					if (pSmartRod->pEnd == pKeyNode)
						lenvec *= -1.0;
					normalize(lenvec);
					maxcosa.Update(fabs(lenvec.x), pRod);
				}
			}
			if (maxcosa.m_pRelaObj)			
			{
				SmartRodPtr pHoriRod = (CLDSLinePart*)maxcosa.m_pRelaObj;
				hashMatchRodHid.SetValue(2, pHoriRod->handle);
			}
		}
		RecogMatchRods(4);
		CStdTemplParaRod* pParaRod = listParaRods.GetValue(4);
		if (pParaRod == NULL)
			return false;
		CStdTemplParaNode* pParaNodeS = this->listParaNodes.GetValue(pParaRod->idStartNode);
		CStdTemplParaNode* pParaNodeE = this->listParaNodes.GetValue(pParaRod->idEndNode);
		if (pParaNodeS->pRelaNode && pParaNodeE->pRelaNode)
		{
			GEPOINT ptS = pParaNodeS->pRelaNode->xPreliftPos;
			GEPOINT ptE = pParaNodeE->pRelaNode->xPreliftPos;
			GEPOINT ptM = (ptS + ptE)*0.5;
			CLDSNode* pNode = Ta.MatchExterNode(ptM);
			CStdTemplParaNode* pParaNode = listParaNodes.GetValue(3);
			if (pNode && pParaNode)
			{
				
				pParaNode->pRelaNode = pNode;
				pParaNode->xPosition = pNode->xOriginalPos;
				hashMatchNodeHid.SetValue(3, pNode->handle);
				return RecogMatchRods(5)>0;
			}
		}
	}
	else if(m_idStdComTmpl==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_EB||
			m_idStdComTmpl==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_2_EB)
	{
		MAP_RODnNODE mapobj=keyobjs[0];
		if(mapobj.ciNode0Rod1==MAP_RODnNODE::TYPE_NODE)
		{
			CStdComTempl::SetMappingNode(mapobj.idNodeOrRod, mapobj.pNode);
			//添加对称节点
			CLDSNode* pMirNodeX = mapobj.pNode->GetMirNode(MIRMSG(1));
			if(pMirNodeX==NULL)
				return false;
			hashMatchNodeHid.SetValue(TMPLOBJID(mapobj.idNodeOrRod,'X'),pMirNodeX->handle);
			RecogMatchRods(1);
			//自动识别横担主材弦杆主材，用于修正挂线底座T型组合角钢朝向 wjh-2019.9.10
			CStdTemplParaRod* pArmPostParaRod=listParaRods.GetValue(2);
			pArmPostParaRod->pRelaRod=Ta.FromRodHandle(mapobj.pNode->hFatherPart);
			if (pArmPostParaRod->pRelaRod!=NULL)
				hashMatchRodHid.SetValue(pArmPostParaRod->Id,pArmPostParaRod->pRelaRod->handle);
		}
	}
	else if(m_idStdComTmpl==IStdComTempl::STDTMPL_STRU_META_SECT_FRT_1_GD||
			m_idStdComTmpl==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_GD)
	{
		MAP_RODnNODE mapobj=keyobjs[0];
		if(mapobj.ciNode0Rod1==MAP_RODnNODE::TYPE_NODE)
		{
			CStdComTempl::SetMappingNode(mapobj.idNodeOrRod, mapobj.pNode);
			//自动识别横担主材弦杆主材
			CLDSNode* pKeyNode = mapobj.pNode;
			CStdTemplParaRod* pParaRod = listParaRods.GetValue(1);
			CLDSLinePart* pDatumRod =Ta.FromRodHandle(pKeyNode->hFatherPart);
			if (pParaRod == NULL || pDatumRod == NULL)
				return false;
			hashMatchRodHid.SetValue(1, pDatumRod->handle);
			//查找连接到N10节点的射线杆件
			LINEPARTSET linkPartSet;
			for (pRod = Ta.EnumRodFirst(); pRod; pRod = Ta.EnumRodNext())
			{
				pSmartRod = pRod;
				if (pRod->pStart == NULL || pRod->pEnd == NULL)
					continue;
				else if (pSmartRod->GetClassTypeId() == CLS_LINEANGLE && pSmartRod.pAngle->group_father_jg_h > 0)
					continue;
				else if (pRod == pDatumRod)
					continue;
				if (pRod->pStart == pKeyNode || pRod->pEnd == pKeyNode)
					linkPartSet.append(pRod);
			}
			GEPOINT vStdPlaneNormal, vLine, vDatumLine;
			if (m_idStdComTmpl == IStdComTempl::STDTMPL_STRU_META_SECT_FRT_1_GD)
				vStdPlaneNormal.Set(0, 1, 0);
			else
				vStdPlaneNormal.Set(0, 0, 1);
			vDatumLine = pDatumRod->pEnd->xOriginalPos - pDatumRod->pStart->xOriginalPos;
			normalize(vDatumLine);
			for (pRod = linkPartSet.GetFirst(); pRod; pRod = linkPartSet.GetNext())
			{
				if (pRod->pStart == pKeyNode || pRod->pStart == pKeyNode)
					vLine = pRod->pEnd->xOriginalPos - pRod->pStart->xOriginalPos;
				else
					vLine = pRod->pStart->xOriginalPos - pRod->pEnd->xOriginalPos;
				GEPOINT vNormal = vLine ^ vDatumLine;
				normalize(vNormal);
				if (fabs(vNormal*vStdPlaneNormal) < 0.82)
					continue;	//
				if (hashMatchRodHid.GetValue(5) == NULL)
					hashMatchRodHid.SetValue(5, pRod->handle);
				else if(hashMatchRodHid.GetValue(6)==NULL)
					hashMatchRodHid.SetValue(6, pRod->handle);
				else
					hashMatchRodHid.SetValue(7, pRod->handle);
			}
		}
	}
	else if(m_idStdComTmpl==IStdComTempl::STDTMPL_STRU_NZ_FRT_2_GD)
	{
		CLDSLinePart* pDatumRod = NULL;
		for (int i = 0; i < 2; i++)
		{
			MAP_RODnNODE mapobj = keyobjs[i];
			if (mapobj.ciNode0Rod1 != MAP_RODnNODE::TYPE_NODE)
				return false;
			CStdTemplParaNode* pParaNode = this->listParaNodes.GetValue(mapobj.idNodeOrRod);
			if (pParaNode == NULL)
				return false;
			CStdComTempl::SetMappingNode(mapobj.idNodeOrRod, mapobj.pNode);
			//查找水平横担
			if (pDatumRod == NULL)
				pDatumRod = Ta.FromRodHandle(mapobj.pNode->hFatherPart);
			else if (pDatumRod->handle != mapobj.pNode->hFatherPart)
				return false;
			hashMatchRodHid.SetValue(1, pDatumRod->handle);
		}
		if (pDatumRod == NULL)
			return false;
		RecogMatchRods(6);
		RecogMatchRods(7);
		//查找N10连接竖直拉杆
		CLDSNode* pNode10 = keyobjs[0].pNode, *pNode30 = NULL, *pNode40 = NULL;
		CMaxDouble maxupgradient;
		for (pRod = Ta.EnumRodFirst(); pRod; pRod = Ta.EnumRodNext())
		{
			pSmartRod = pRod;
			if (pRod->pStart == NULL || pRod->pEnd == NULL)
				continue;
			else if (pSmartRod->GetClassTypeId() == CLS_LINEANGLE && pSmartRod.pAngle->group_father_jg_h > 0)
				continue;
			if (pRod->pStart == pNode10 || pRod->pEnd == pNode10)
			{
				GEPOINT lenvec = pRod->pEnd->xOriginalPos - pRod->pStart->xOriginalPos;
				if (pRod->pEnd == pNode10)
					lenvec *= -1.0;
				normalize(lenvec);
				maxupgradient.Update(-lenvec.z, pRod);
			}
		}
		if (maxupgradient.m_pRelaObj == NULL)
			return false;
		CLDSLinePart* pVertRod = (CLDSLinePart*)maxupgradient.m_pRelaObj;
		if (pVertRod->pStart == pNode10)
			pNode30 = pVertRod->pEnd;
		else if (pVertRod->pEnd == pNode10)
			pNode30 = pVertRod->pStart;
		CStdComTempl::SetMappingNode(3, pNode30);
		RecogMatchRods(2);
		RecogMatchRods(3);
		//查找N20连接的正面射线杆件
		CLDSNode* pNode20 = keyobjs[1].pNode;
		LINEPARTSET linkPartSet;
		for (pRod = Ta.EnumRodFirst(); pRod; pRod = Ta.EnumRodNext())
		{
			pSmartRod = pRod;
			if (pRod->pStart == NULL || pRod->pEnd == NULL)
				continue;
			if (pRod->GetClassTypeId() == CLS_LINEANGLE && pSmartRod.pAngle->group_father_jg_h > 0)
				continue;
			if (pRod->pStart == pNode20 || pRod->pEnd == pNode20)
				linkPartSet.append(pRod);
		}
		GEPOINT vStdPlaneNormal(0, 1, 0), vDatumLine, vRayLine;
		vDatumLine = pDatumRod->pEnd->xOriginalPos - pDatumRod->pStart->xOriginalPos;
		normalize(vDatumLine);
		for (pRod = linkPartSet.GetFirst(); pRod; pRod = linkPartSet.GetNext())
		{
			if (pRod->pStart == pNode20 || pRod->pStart == pNode20)
				vRayLine = pRod->pEnd->xOriginalPos - pRod->pStart->xOriginalPos;
			else
				vRayLine = pRod->pStart->xOriginalPos - pRod->pEnd->xOriginalPos;
			GEPOINT vNormal = vRayLine ^ vDatumLine;
			normalize(vNormal);
			if (fabs(vNormal*vStdPlaneNormal) < 0.82)
				continue;	//
			if(pRod->pStart==pNode30 || pRod->pEnd==pNode30)
				continue;	//3号杆件已注册
			if (hashMatchRodHid.GetValue(4) == NULL)
			{
				hashMatchRodHid.SetValue(4, pRod->handle);
				if (pRod->pStart == pNode20)
					pNode40 = pRod->pEnd;
				else if (pRod->pEnd == pNode20)
					pNode40 = pRod->pStart;
				CStdComTempl::SetMappingNode(4, pNode40);
			}			
		}
		RecogMatchRods(5);
	}
	else if (m_idStdComTmpl == IStdComTempl::STDTMPL_STRU_NZ_BTM_2_GD)
	{
		CLDSLinePart* pDatumRod = NULL;
		for (int i = 0; i < 2; i++)
		{
			MAP_RODnNODE mapobj = keyobjs[i];
			if (mapobj.ciNode0Rod1 != MAP_RODnNODE::TYPE_NODE)
				return false;
			CStdTemplParaNode* pParaNode = this->listParaNodes.GetValue(mapobj.idNodeOrRod);
			if (pParaNode == NULL)
				return false;
			CStdComTempl::SetMappingNode(mapobj.idNodeOrRod, mapobj.pNode);
			//查找水平横担
			if (pDatumRod == NULL)
				pDatumRod = Ta.FromRodHandle(mapobj.pNode->hFatherPart);
			else if (pDatumRod->handle != mapobj.pNode->hFatherPart)
				return false;
			hashMatchRodHid.SetValue(1, pDatumRod->handle);
		}
		//查找连接射线杆件
		CLDSNode* pKeyNode1 = keyobjs[0].pNode;
		CLDSNode* pKeyNode2 = keyobjs[1].pNode;
		LINEPARTSET linkPartSet;
		for (pRod = Ta.EnumRodFirst(); pRod; pRod = Ta.EnumRodNext())
		{
			pSmartRod = pRod;
			if (pRod->pStart == NULL || pRod->pEnd == NULL)
				continue;
			else if (pSmartRod->GetClassTypeId() == CLS_LINEANGLE && pSmartRod.pAngle->group_father_jg_h > 0)
				continue;
			else if(pRod==pDatumRod)
				continue;
			if (pRod->pStart == pKeyNode1 || pRod->pEnd == pKeyNode1)
				linkPartSet.append(pRod);
			else if (pRod->pStart == pKeyNode2 || pRod->pEnd == pKeyNode2)
				linkPartSet.append(pRod);
		}
		GEPOINT vStdPlaneNormal(0, 0, 1), vLine, vDatumLine;
		vDatumLine = pDatumRod->pEnd->xOriginalPos - pDatumRod->pStart->xOriginalPos;
		normalize(vDatumLine);
		for (pRod = linkPartSet.GetFirst(); pRod; pRod = linkPartSet.GetNext())
		{
			if (pRod->pStart == pKeyNode1|| pRod->pStart== pKeyNode2)
				vLine = pRod->pEnd->xOriginalPos - pRod->pStart->xOriginalPos;
			else
				vLine = pRod->pStart->xOriginalPos - pRod->pEnd->xOriginalPos;
			GEPOINT vNormal = vLine ^ vDatumLine;
			normalize(vNormal);
			if (fabs(vNormal*vStdPlaneNormal) < 0.82)
				continue;	//
			normalize(vLine);
			if (pRod->pStart == pKeyNode1 || pRod->pEnd == pKeyNode1)
			{
				if (fabs(vLine*vDatumLine) < EPS2)
					hashMatchRodHid.SetValue(2, pRod->handle);
				else
				{
					if (hashMatchRodHid.GetValue(4) == NULL)
						hashMatchRodHid.SetValue(4, pRod->handle);
					else
						hashMatchRodHid.SetValue(5, pRod->handle);
				}
			}
			else
			{
				if (fabs(vLine*vDatumLine) < EPS2)
					hashMatchRodHid.SetValue(3, pRod->handle);
				else
				{
					if (hashMatchRodHid.GetValue(6) == NULL)
						hashMatchRodHid.SetValue(6, pRod->handle);
					else
						hashMatchRodHid.SetValue(7, pRod->handle);
				}
			}
		}
	}
	else if (m_idStdComTmpl == IStdComTempl::STDTMPL_STRU_NZ_BTM_3_GD)
	{
		MAP_RODnNODE mapobj = keyobjs[0];
		if (mapobj.ciNode0Rod1 == MAP_RODnNODE::TYPE_NODE)
		{
			CLDSNode* pKeyNode = mapobj.pNode;
			CStdTemplParaNode* pParaNode = this->listParaNodes.GetValue(mapobj.idNodeOrRod);
			if (pParaNode == NULL)
				return false;
			pParaNode->pRelaNode = mapobj.pNode;
			pParaNode->xPosition = mapobj.pNode->xOriginalPos;
			hashMatchNodeHid.SetValue(mapobj.idNodeOrRod, mapobj.pNode->handle);
			//RecogMatchRods(3);
			CMaxDouble maxcosa(0), maxupgradient;
			for (pRod = Ta.EnumRodFirst(); pRod; pRod = Ta.EnumRodNext())
			{
				pSmartRod = pRod;
				if (pRod->pStart == NULL || pRod->pEnd == NULL)
					continue;
				else if (pSmartRod->GetClassTypeId() == CLS_LINEANGLE && pSmartRod.pAngle->group_father_jg_h > 0)
					continue;
				if (pRod->pStart == pKeyNode || pRod->pEnd == pKeyNode)
				{
					GEPOINT lenvec = pRod->pEnd->xOriginalPos - pRod->pStart->xOriginalPos;
					if (pRod->pEnd == pKeyNode)
						lenvec *= -1.0;
					normalize(lenvec);
					maxcosa.Update(fabs(lenvec.x), pRod);
					maxupgradient.Update(-lenvec.z, pRod);
				}
			}
			SmartRodPtr pHoriRod, pVertRod;
			if (maxcosa.m_pRelaObj)			//找到水平横担杆件
			{
				pHoriRod = (CLDSLinePart*)maxcosa.m_pRelaObj;
				this->hashMatchRodHid.SetValue(1, pHoriRod->handle);
			}
		}
		mapobj = keyobjs[1];
		if (mapobj.ciNode0Rod1 == MAP_RODnNODE::TYPE_NODE)
		{
			hashMatchNodeHid.SetValue(mapobj.idNodeOrRod, mapobj.pNode->handle);
			CStdTemplParaNode* pParaNode = this->listParaNodes.GetValue(mapobj.idNodeOrRod);
			if (pParaNode == NULL)
				return false;
			pParaNode->pRelaNode = mapobj.pNode;
			pParaNode->xPosition = mapobj.pNode->xOriginalPos;
		}
		mapobj = keyobjs[2];
		if (mapobj.ciNode0Rod1 == MAP_RODnNODE::TYPE_NODE)
		{
			hashMatchNodeHid.SetValue(mapobj.idNodeOrRod, mapobj.pNode->handle);
			CStdTemplParaNode* pParaNode = this->listParaNodes.GetValue(mapobj.idNodeOrRod);
			if (pParaNode == NULL)
				return false;
			pParaNode->pRelaNode = mapobj.pNode;
			pParaNode->xPosition = mapobj.pNode->xOriginalPos;
			return true;
		}
	}
	return true;
}
long CStdComTemplWireStructure::SetMappingNode(long idTemplNode,long hLdsNode)
{
	if(idTemplNode==KEY4C("N10"))
		idTemplNode=1;
	else if(idTemplNode==KEY4C("N20"))
		idTemplNode=2;
	else if(idTemplNode==KEY4C("N30"))
		idTemplNode=3;
	else if(idTemplNode==KEY4C("N40"))
		idTemplNode=4;
	CLDSNode* pKeyNode=Ta.FromNodeHandle(hLdsNode);
	CStdTemplParaNode* pParaNode=this->listParaNodes.GetValue(idTemplNode);
	if(pParaNode)
	{
		pParaNode->pRelaNode=pKeyNode;
		pParaNode->xPosition=pKeyNode->xOriginalPos;
	}
	CStdComTempl::SetMappingNode(idTemplNode,hLdsNode);
	return hLdsNode;
}
bool CStdComTemplWireStructure::SetMappingNode(long idTemplNode,CLDSNode *pKeyNode)
{
	if(idTemplNode==KEY4C("N10"))
		idTemplNode=1;
	else if(idTemplNode==KEY4C("N20"))
		idTemplNode=2;
	else if(idTemplNode==KEY4C("N30"))
		idTemplNode=3;
	else if(idTemplNode==KEY4C("N40"))
		idTemplNode=4;
	CStdTemplParaNode* pParaNode=this->listParaNodes.GetValue(idTemplNode);
	if(pParaNode)
	{
		pParaNode->pRelaNode=pKeyNode;
		pParaNode->xPosition=pKeyNode->xOriginalPos;
	}
	return CStdComTempl::SetMappingNode(idTemplNode,pKeyNode);
}
int CStdComTemplWireStructure::RecogMatchRods(UINT idRecogRod/*=0*/)
{
	if(m_pBelongModel==NULL)
	{
		m_pBelongModel=&Ta;
		logerr.Log("error");
	}
	//根据节点:1,2,3识别出三根关键杆件
	return CStdComTempl::RecogMatchRods(idRecogRod);
}
bool CStdComTemplWireStructure::TranslateTmplInstance()
{
	bool bRet = true;
	if (this->m_idStdComTmpl == IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM1)
		bRet = TransTemlInstance_EB_DANGLE_EARTH_WIRE_ARM1();
	else if(this->m_idStdComTmpl == IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM2)
		bRet = TransTemlInstance_EB_DANGLE_EARTH_WIRE_ARM2();
	else if(this->m_idStdComTmpl == IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM3)
		bRet = TransTemlInstance_EB_DANGLE_EARTH_WIRE_ARM3();
	else if (this->m_idStdComTmpl == IStdComTempl::STDTMPL_STRU_NZ_FRT_2_GD)
		bRet = TransTemlInstance_NZ_FRT_2_GD();
	else if (this->m_idStdComTmpl == IStdComTempl::STDTMPL_STRU_NZ_BTM_2_GD)
		bRet = TransTemlInstance_NZ_BTM_2_GD();
	else if (this->m_idStdComTmpl == IStdComTempl::STDTMPL_STRU_NZ_BTM_3_GD)
		bRet = TransTemlInstance_NZ_BTM_3_GD();
	else if (this->m_idStdComTmpl == IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_EB)
		bRet = TransTemlInstance_SECT_BTM_1_EB();
	else if (this->m_idStdComTmpl == IStdComTempl::STDTMPL_STRU_META_SECT_BTM_2_EB)
		bRet = TransTemlInstance_SECT_BTM_2_EB();
	else if (this->m_idStdComTmpl == IStdComTempl::STDTMPL_STRU_META_SECT_FRT_1_GD)
		bRet = TransTemlInstance_SECT_FRT_1_GD();
	else if (this->m_idStdComTmpl == IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_GD)
		bRet = TransTemlInstance_SECT_BTM_1_GD();
	else if(this->m_idStdComTmpl == IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM1)
		bRet = TransTemlInstance_EB_DANGLE_WIRE_I_ARM1();
	else if(this->m_idStdComTmpl == IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM2)
		bRet = TransTemlInstance_EB_DANGLE_WIRE_I_ARM2();
	return bRet;
}
#endif
