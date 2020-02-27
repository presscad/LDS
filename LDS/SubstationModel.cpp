#include "stdafx.h"
#include "SubstationModel.h"
#include "TimerCount.h"
#include "PostProcessReport.h"
#include "env_def.h"
#include "GlobalFunc.h"	//SearchChar()在此文件定义

#ifdef __PART_DESIGN_INC_
void FemResolverMonitor(int process,const char* prompt);
GEPOINT CalLocalCS_Z_AXIS(const double* axis_x_coords)
{
	GEPOINT axis_x(axis_x_coords);
	if(axis_x.IsZero())
		return GEPOINT(0,0,0);
	if(fabs(axis_x.x)+fabs(axis_x.z)<EPS&&fabs(axis_x.y)>EPS)
		return GEPOINT(0,0,1);	//局部坐标系Ｘ轴与整体坐标系Ｙ轴平行
	else
	{
		GEPOINT axis_z(-axis_x.z,0,axis_x.x);
		normalize(axis_z);
		return axis_z;
	}
}
////////////////////////////////////////////////////////////////////////////////////////
CFemModel::CFemModel(void)
{
	m_ciPercent=0;
}
CFemModel::~CFemModel(void)
{
}

void CFemModel::ImportSubstationExchangeFile(char* filename)
{
	FILE* fp=fopen(filename,"rt");
	if(fp==NULL)
		return;
	Ta.Empty();
	console.DispNodeSet.Empty();
	console.DispPartSet.Empty();
	Ta.InitTower();
	console.InitTowerModel();
	UCS_STRU ucs;
	ucs.axis_x.Set(1,0,0);
	ucs.axis_z.Set(0,0,-1);
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	long nNode,nElem,nMaterial,nSection,nRestrict,nFoundation,nLoadCase;
	int iNodeLimit,iElemLimit,iMatLimit,iSectionLimit,iResLimit,iFoundLimit,iLoadStart,iNodeLoadLimit,iElemLoadLimit;
	iNodeLimit=iElemLimit=iMatLimit=iSectionLimit=iResLimit=iFoundLimit=iLoadStart=iNodeLoadLimit=iElemLoadLimit=2;
	CXhChar100 lineText,projectName;
	int validLineIndex=0,iMaterialIndex=1;;
	FEMMODEL_LOAD* pLoad=NULL;
	while(!ferror(fp) && !feof(fp))
	{
		lineText.Empty();
		fgets(lineText,100,fp);
		if(lineText.GetLength()==0)
			continue;
		lineText.Replace("　"," ");	//将全角空格换成半角空格以便统一处理
		CXhChar100 copyLnText=lineText;
		char* splitter=" =\t\r\n";
		char* key=strtok(copyLnText,splitter);
		if(key==NULL||strlen(key)==0)
			continue;	//空行
		//提取有效信息
		if(validLineIndex==0)
			projectName=lineText;
		else if(strstr(lineText,"节点坐标")!=NULL)
			continue;
		else if(strstr(lineText,"关联信息")!=NULL)
			continue;
		else if(strstr(lineText,"材料信息")!=NULL)
			continue;
		else if(strstr(lineText,"截面信息")!=NULL)
			continue;
		else if(strstr(lineText,"支座信息")!=NULL)
			continue;
		else if(strstr(lineText,"约束信息")!=NULL)
			continue;
		else if(strstr(lineText,"荷载信息")!=NULL)
			continue;
		if(strchr(lineText,','))
			lineText.Replace(',',' ');
		if(validLineIndex==1)
		{
			sscanf(lineText,"%d%d%d%d%d%d%d",&nNode,&nElem,&nMaterial,&nSection,&nRestrict,&nFoundation,&nLoadCase);
			iNodeLimit=nNode+2;
			iElemLimit=iNodeLimit+nElem;
			iMatLimit=iElemLimit+nMaterial;
			iSectionLimit=iMatLimit+nSection;
			iResLimit=iSectionLimit+nRestrict;
			iLoadStart=iFoundLimit=iResLimit+nFoundation;
			//load_case.nLoadCase=nLoadCase;
			iNodeLoadLimit=iFoundLimit+1;
		}
		else if(validLineIndex>1 && validLineIndex<iNodeLimit)
		{	//文件给定坐标单位为m,需转化为mm，并调整坐标系
			FEMMODEL_NODE node;
			sscanf(lineText,"%d%lf%lf%lf",&node.pointI,&node.pos.x,&node.pos.y,&node.pos.z);
			node.pos*=1000;
			coord_trans(node.pos,ucs,FALSE,TRUE);
			FEMMODEL_NODE*pSubstationNode=hashSubstationNodes.SetValue(node.pointI,node);
			pSubstationNode->wRestrict=0;
			CLDSNode* pNode=Ta.Node.append();
			pNode->SetPosition(node.pos.x,node.pos.y,node.pos.z);
			pNode->point_i=node.pointI;
			pSubstationNode->pLdsNode=pNode;
			pSubstationNode->hNode=pNode->handle;
			NewNode(pNode);
		}
		else if(validLineIndex>=iNodeLimit && validLineIndex<iElemLimit)
		{
			FEMMODEL_ELEM elem;
			sscanf(lineText,"%d%d%d%d",&elem.elemI,&elem.startPointI,&elem.endPointI,&elem.sectionI);
			FEMMODEL_ELEM* pStationElem=hashSubstationElems.SetValue(elem.elemI,elem);
			CLDSLinePart* pRod=(CLDSLinePart*)Ta.Parts.append(CLS_LINEPART);
			pRod->size_idClassType=CLS_LINETUBE;
			pStationElem->hPart=pRod->handle;
			pStationElem->pLdsRod=pRod;
			FEMMODEL_NODE* pSubstationStartNode=hashSubstationNodes.GetValue(elem.startPointI);
			FEMMODEL_NODE* pSubstationEndNode  =hashSubstationNodes.GetValue(elem.endPointI);
			pRod->pStart=Ta.Node.FromHandle(pSubstationStartNode->hNode);
			pRod->pEnd  =Ta.Node.FromHandle(pSubstationEndNode->hNode);
			pRod->SetStart(pSubstationStartNode->pos);
			pRod->SetEnd(pSubstationEndNode->pos);
			pStationElem->cbJointStart=pStationElem->cbJointEnd=0;
			NewLinePart(pRod);
		}
		else if(validLineIndex>=iElemLimit && validLineIndex<iMatLimit)
		{	//材质信息
			CXhChar16 sValue1,sValue2,sValue3,sValue4;
			sscanf(lineText,"%s%s%s%s",(char*)sValue1,(char*)sValue2,(char*)sValue3,(char*)sValue4);
			FEMMODEL_MATERIAL material;
			material.E=atof(sValue1);
			material.shear=atof(sValue2);
			material.poson=atof(sValue3);
			material.density=atof(sValue4);
			hashSubstationMaterial.SetValue(iMaterialIndex,material);
			iMaterialIndex++;
		}
		else if(validLineIndex>=iMatLimit && validLineIndex<iSectionLimit)
		{	//截面信息
			FEMMODEL_SECTION section;
			CXhChar16 sValue1,sValue2,sValue3,sValue4;
			sscanf(lineText,"%d%s%s%s%s",&section.materialI,(char*)sValue1,(char*)sValue2,(char*)sValue3,(char*)sValue4);
			section.area=atof(sValue1);
			section.IX=atof(sValue2);
			section.IY=atof(sValue3);
			section.IZ=atof(sValue4);
			section.sectionI=validLineIndex-iMatLimit+1;
			hashSubstationSections.SetValue(section.sectionI,section);
		}
		else if(validLineIndex>=iSectionLimit && validLineIndex<iResLimit)
		{	//单元约束
			int elemI,xRestrict,yRestrict,zRestrict,xRotRestrict,yRotRestrict,zRotRestrict;
			CXhChar16 sKey;
			sscanf(lineText,"%d%s%d%d%d%d%d%d",&elemI,(char*)sKey,&xRestrict,&yRestrict,&zRestrict,&xRotRestrict,&yRotRestrict,&zRotRestrict);
			FEMMODEL_ELEM* pElem=hashSubstationElems.GetValue(elemI);
			if(strchr(sKey,'S')!=NULL)
			{
				pElem->cbJointStart=0;
				if(xRestrict!=0)
					pElem->cbJointStart|=0x01;
				if(yRestrict!=0)
					pElem->cbJointStart|=0x02;
				if(zRestrict!=0)
					pElem->cbJointStart|=0x04;
				if(xRotRestrict!=0)
					pElem->cbJointStart|=0x08;
				if(yRotRestrict!=0)
					pElem->cbJointStart|=0x10;
				if(zRotRestrict!=0)
					pElem->cbJointStart|=0x20;
			}
			else
			{
				pElem->cbJointEnd=0;
				if(xRestrict!=0)
					pElem->cbJointEnd|=0x01;
				if(yRestrict!=0)
					pElem->cbJointEnd|=0x02;
				if(zRestrict!=0)
					pElem->cbJointEnd|=0x04;
				if(xRotRestrict!=0)
					pElem->cbJointEnd|=0x08;
				if(yRotRestrict!=0)
					pElem->cbJointEnd|=0x10;
				if(zRotRestrict!=0)
					pElem->cbJointEnd|=0x20;
			}
		}
		else if(validLineIndex>=iResLimit && validLineIndex<iFoundLimit)
		{	//支座信息
			int pointI,xRestrict,yRestrict,zRestrict,xRotRestrict,yRotRestrict,zRotRestrict;
			sscanf(lineText,"%d%d%d%d%d%d%d",&pointI,&xRestrict,&yRestrict,&zRestrict,&xRotRestrict,&yRotRestrict,&zRotRestrict);
			WORD wFlag=0;
			if(xRestrict==1)
				wFlag|=X_RESTRICT;
			if(yRestrict==1)
				wFlag|=Y_RESTRICT;
			if(zRestrict==1)
				wFlag|=Z_RESTRICT;
			if(xRotRestrict==1)
				wFlag|=X_ROT_RESTRICT;
			if(yRotRestrict==1)
				wFlag|=Y_ROT_RESTRICT;
			if(zRotRestrict==1)
				wFlag|=Z_ROT_RESTRICT;
			//设置节点约束
			FEMMODEL_NODE* pSubstationNode=hashSubstationNodes.GetValue(pointI);
			pSubstationNode->wRestrict=wFlag;
		}
		else if(validLineIndex>=iFoundLimit)
		{	//荷载信息
			if(validLineIndex==iLoadStart)
			{
				pLoad=listCaseLoads.append();
				sscanf(lineText,"%lf%lf",&pLoad->fGamaG1,&pLoad->fGamaG2);
			}
			else if(validLineIndex==iLoadStart+1)//strchr(lineText,'.')==NULL)
			{	
				sscanf(lineText,"%d%d",&(pLoad->nNodeLoad),&(pLoad->nElemLoad));
				iNodeLoadLimit=validLineIndex+pLoad->nNodeLoad+1;
				iLoadStart=iElemLoadLimit=iNodeLoadLimit+pLoad->nElemLoad;
			}
			else if(validLineIndex<iNodeLoadLimit)
			{
				FEMMODEL_NODE_LOAD nodeLoad;
				CXhChar16 nodeInfo,sLoad;
				sscanf(lineText,"%s%lf",(char*)nodeInfo,&nodeLoad.fLoad);
				nodeInfo.Replace('.',' ');
				sscanf(nodeInfo,"%d%d",&nodeLoad.pointI,&nodeLoad.directionI);
				if(nodeLoad.directionI==2||nodeLoad.directionI==3)
					nodeLoad.fLoad=-nodeLoad.fLoad;
				pLoad->nodeLoadList.append(nodeLoad);
			}
			else if(validLineIndex>=iNodeLoadLimit && validLineIndex<iElemLoadLimit)
			{
				FEMMODEL_ELEM_LOAD elemLoad;
				sscanf(lineText,"%d%d%lf%lf%d",&elemLoad.elemI,&elemLoad.loadType,&elemLoad.fDist,&elemLoad.fLoad,&elemLoad.directionI);
				if(elemLoad.directionI==2||elemLoad.directionI==3)
					elemLoad.fLoad=-elemLoad.fLoad;
				pLoad->elemLoadList.append(elemLoad);
			}
		}
		validLineIndex++;
	}
	fclose(fp);
}
void CFemModel::InitializeLdsModel()
{
	FEMMODEL_NODE* pStationNode;
	FEMMODEL_ELEM* pStationElem;
	CHashSet<CLDSNode*> hashNodes;
	for(pStationNode=hashSubstationNodes.GetFirst();pStationNode;pStationNode=hashSubstationNodes.GetNext())
	{
		pStationNode->pLdsNode=Ta.Node.append();
		pStationNode->pLdsNode->SetLayer("SPQ");
		pStationNode->pLdsNode->point_i=pStationNode->pointI;
		pStationNode->pLdsNode->SetPosition(pStationNode->pos.x,-pStationNode->pos.y,-pStationNode->pos.z);
		pStationNode->pLdsNode->SetRestrict(pStationNode->wRestrict);
		hashNodes.SetValue(pStationNode->pointI,pStationNode->pLdsNode);
	}
	int i=0,j=0;
	//提取有限元计算模型中的单元集合
	for(pStationElem=hashSubstationElems.GetFirst();pStationElem;pStationElem=hashSubstationElems.GetNext(),i++)
	{
		CLDSLinePart* pRod=(CLDSLinePart*)Ta.AppendPart(CLS_LINEPART);
		pRod->size_idClassType=CLS_LINETUBE;
		pRod->SetLayer("SZQ");
		//pFemElem->elem_type=0;
		//pFemElem->T0=pFemElem->N=0;
		pRod->pStart=hashNodes.GetValue(pStationElem->startPointI);
		pRod->pEnd=hashNodes.GetValue(pStationElem->endPointI);
		pRod->start_joint_type=pRod->end_joint_type=JOINT_RIGID;
		if(pStationElem->cbJointStart==0x07)
			pRod->start_joint_type=JOINT_HINGE;
		else if(pStationElem->cbJointStart==0x3f)
			pRod->start_joint_type=JOINT_RIGID;
		//else if(pStationElem->cbJointStart==0)
		//	pRod->cbStartJoint=0x3f;
		//else //if(pStationElem->cbJointStart==0)
		//	pRod->cbStartJoint=pStationElem->cbJointStart;
		//else
		//	pFemElem->cbStartJoint=pStationElem->cbJointStart;
		if(pStationElem->cbJointEnd==0x07)
			pRod->end_joint_type=JOINT_HINGE;
		else if(pStationElem->cbJointEnd==0x3f)
			pRod->end_joint_type=JOINT_RIGID;
		//else if(pStationElem->cbJointEnd==0)
		//	pRod->cbEndJoint=0x3f;
		//else //if(pStationElem->cbJointEnd==0)
		//	pRod->cbEndJoint=pStationElem->cbJointEnd;
		//else
		//	pFemElem->cbEndJoint=pStationElem->cbJointEnd;
		if(pRod->start_joint_type==JOINT_RIGID||pRod->end_joint_type==JOINT_RIGID)
			pRod->m_iElemType=2;
		FEMMODEL_SECTION* pSect=hashSubstationSections.GetValue(pStationElem->sectionI);
		//模拟钢管规格尺寸
		pRod->size_thick=SQRT(pSect->area*1e6/125.6);	//125.6=40*Pi
		pRod->size_wide=pRod->size_thick*40;
		pStationElem->pLdsRod=pRod;
	}
}
bool CFemModel::AnalyzeFemModel(const char* filename/*=NULL*/)
{
	FEMMODEL_NODE* pStationNode;
	FEMMODEL_ELEM* pStationElem;
	CHashSet<FEM_NODE*> hashFemNodeSet;
	CHashSet<FEM_ELEMENT*> hashFemElemSet;
	FEM_NODE* pFemNode;
	FEM_ELEMENT* pFemElem;
	//提取有限元计算模型中的节点集合
	for(pStationNode=hashSubstationNodes.GetFirst();pStationNode;pStationNode=hashSubstationNodes.GetNext())
	{
		pFemNode=listFemNodes.append();
		pFemNode->point_i=pStationNode->pointI;
		pFemNode->dimension=3;
		pStationNode->index=listFemNodes.GetNodeNum()-1;
		pFemNode->pos=pStationNode->pos;
		pFemNode->wRestrict=pStationNode->wRestrict;
		hashFemNodeSet.SetValue(pFemNode->point_i,pFemNode);
	}
	int i=0,j=0;
	//提取有限元计算模型中的单元集合
	for(pStationElem=hashSubstationElems.GetFirst();pStationElem;pStationElem=hashSubstationElems.GetNext(),i++)
	{
		pFemElem=listFemElems.append();
		pFemElem->idElem=pStationElem->elemI;
		pFemElem->elem_type=0;
		pFemElem->T0=pFemElem->N=0;
		pFemElem->pStart=hashFemNodeSet.GetValue(pStationElem->startPointI);
		pFemElem->pEnd=hashFemNodeSet.GetValue(pStationElem->endPointI);
		pFemElem->length=DISTANCE(pFemElem->pStart->pos,pFemElem->pEnd->pos);
		pFemElem->lenStdVec=(pFemElem->pEnd->pos-pFemElem->pStart->pos)/pFemElem->length;
		hashFemElemSet.SetValue(pStationElem->elemI,pFemElem);
		FEMMODEL_NODE* pStartStationNode=hashSubstationNodes.GetValue(pStationElem->startPointI);
		FEMMODEL_NODE* pEndStationNode=hashSubstationNodes.GetValue(pStationElem->endPointI);
		pFemElem->start_i=pStartStationNode->index;
		pFemElem->end_i=pEndStationNode->index;
		pFemElem->start_joint_type=pFemElem->end_joint_type=JOINT_RIGID;
		if(pStationElem->cbJointStart==0x07)
			pFemElem->start_joint_type=JOINT_HINGE;
		else if(pStationElem->cbJointStart==0x3f)
			pFemElem->start_joint_type=JOINT_RIGID;
		else if(pStationElem->cbJointStart==0)
			pFemElem->cbStartJoint=0x3f;
		else //if(pStationElem->cbJointStart==0)
			pFemElem->cbStartJoint=pStationElem->cbJointStart;
		//else
		//	pFemElem->cbStartJoint=pStationElem->cbJointStart;
		if(pStationElem->cbJointEnd==0x07)
			pFemElem->end_joint_type=JOINT_HINGE;
		else if(pStationElem->cbJointEnd==0x3f)
			pFemElem->end_joint_type=JOINT_RIGID;
		else if(pStationElem->cbJointEnd==0)
			pFemElem->cbEndJoint=0x3f;
		else //if(pStationElem->cbJointEnd==0)
			pFemElem->cbEndJoint=pStationElem->cbJointEnd;
		//else
		//	pFemElem->cbEndJoint=pStationElem->cbJointEnd;
		FEMMODEL_SECTION* pSect=hashSubstationSections.GetValue(pStationElem->sectionI);
		if(pSect==NULL)
		{
			pFemElem->E=2.06e5;//pStationElem->E,MPa
			pFemElem->G=pFemElem->E/2.6;//pStationElem->E
			pFemElem->density=0;
		}
		else
		{
			pFemElem->A=pSect->area*1e6;//m2->mm2
			pFemElem->Iz=pSect->IY*1e12;	//不支持IY!=IZ情况,1e12#m4->mm4
			pFemElem->Ip=pSect->IX*1e12;
			FEMMODEL_MATERIAL* pMaterial=hashSubstationMaterial.GetValue(pSect->materialI);
			if(pMaterial)
			{
				pFemElem->E=pMaterial->E*0.001;	//TODO:单位,*0.001#kPa->MPa
				pFemElem->G=pMaterial->shear*0.001;
				pFemElem->density=pMaterial->density*1e-7;	//单位:kN/m3->kg/mm3
			}
		}
	}
	//提取有限元计算过程中的工况集合
	FEMMODEL_LOAD* pLoadCase;
	CWorkStatus* pLdsCase;
	Ta.WorkStatus.Empty();
	CObjNoGroup* pCaseGroup=Ta.NoManager.FromGroupID(WORKSTATUS_GROUP);
	pCaseGroup->EmptyNo();
	for(pLoadCase=listCaseLoads.GetFirst(),i=1;pLoadCase;pLoadCase=listCaseLoads.GetNext(),i++)
	{
		pCaseGroup->SetNoState(i);
		pLdsCase=Ta.WorkStatus.append();
		pLdsCase->m_bLoadIncFactor=true;
		pLdsCase->iNo=i;
		sprintf(pLdsCase->description,"%d",i);
		if(i==1)
			Ta.m_hActiveStatus=pLdsCase->handle;
	}
	//postreport.InitProcTempFile(CXhChar200("%s",filename));
	//postreport.Empty();
	CLDSModule* pModule=Ta.GetActiveModule();
	CFemResolver* pResolver=CFemResolver::CreateResolver();
	pResolver->SetProcessMonitor(FemResolverMonitor);
	pResolver->SetBuffSize(400*0x100000);	//400M缓存空间
	CTimerCount timer;
	timer.Start();
	try{
		bool bFirstResolve=true;
		int iCaseNo;
		for(pLoadCase=listCaseLoads.GetFirst(),pLdsCase=Ta.WorkStatus.GetFirst(),iCaseNo=1;pLoadCase;
			pLoadCase=listCaseLoads.GetNext(),pLdsCase=Ta.WorkStatus.GetNext(),iCaseNo++)
		{
			FEM_NODE* pPrevFemNode=NULL;
			for(pFemNode=listFemNodes.GetFirst();pFemNode;pFemNode=listFemNodes.GetNext())
			{
				pFemNode->Fxyz.Set();
				pFemNode->Moment.Set();
			}
			//清空单元内２，３，４断面上的的内力及内力矩
			for(pStationElem=hashSubstationElems.GetFirst();pStationElem;pStationElem=hashSubstationElems.GetNext())
			{
				pStationElem->QF.Set();
				pStationElem->QWF=0;
				pStationElem->F_sect_s.Set();
				pStationElem->M_sect_s.Set();
				pStationElem->F_sect_e.Set();
				pStationElem->M_sect_e.Set();
				for(j=0;j<3;j++)
				{
					pStationElem->sects[j].F_T_S.Set();
					pStationElem->sects[j].M_T_S.Set();
				}
			}
			//1.施加节点集中荷载
			for(FEMMODEL_NODE_LOAD* pNodeLoad=pLoadCase->nodeLoadList.GetFirst();pNodeLoad;pNodeLoad=pLoadCase->nodeLoadList.GetNext())
			{
				if(pPrevFemNode&&pNodeLoad->pointI==pPrevFemNode->point_i)
					pFemNode=pPrevFemNode;
				else
					pPrevFemNode=pFemNode=hashFemNodeSet.GetValue(pNodeLoad->pointI);
				switch(pNodeLoad->directionI)
				{
				case 1:
					pFemNode->Fxyz.x+=pNodeLoad->fLoad*1000;	//kN->N
					break;
				case 2:
					pFemNode->Fxyz.y+=pNodeLoad->fLoad*1000;
					break;
				case 3:
					pFemNode->Fxyz.z+=pNodeLoad->fLoad*1000;
					break;
				case 4:
					pFemNode->Moment.x+=pNodeLoad->fLoad*1e6;	//kN*m->N*mm
					break;
				case 5:
					pFemNode->Moment.y+=pNodeLoad->fLoad*1e6;
					break;
				case 6:
					pFemNode->Moment.z+=pNodeLoad->fLoad*1e6;
					break;
				default:
					break;
				}
			}
			FEM_ELEMENT* pPrevFemElem=NULL;
			FEMMODEL_ELEM* pPrevStationElem=NULL;
			//2.施加单元荷载
			for(FEMMODEL_ELEM_LOAD* pElemLoad=pLoadCase->elemLoadList.GetFirst();pElemLoad;pElemLoad=pLoadCase->elemLoadList.GetNext())
			{
				if(pPrevFemElem&&pElemLoad->elemI==pPrevStationElem->elemI)
				{
					pFemElem=pPrevFemElem;
					pStationElem=pPrevStationElem;
				}
				else
				{
					pPrevFemElem=pFemElem=hashFemElemSet.GetValue(pElemLoad->elemI);
					pPrevStationElem=pStationElem=hashSubstationElems.GetValue(pElemLoad->elemI);
				}
				GEPOINT armLength,armStartM;
				double scale;
				//对于释放部分转动约束情况，应依据《简明建筑结构设计手册》以Y和Z轴分别考虑杆件两端的转动约束类型 wjh-2015.12.09
				//BYTE ciJointStartAxisY=pFemElem->start_joint_type,ciJointStartAxisZ=pFemElem->start_joint_type;
				//BYTE ciJointEndAxisY=pFemElem->end_joint_type,ciJointEndAxisZ=pFemElem->end_joint_type;
				//if((pFemElem->cbStartJoint&0x10)==0)
				//	ciJointStartAxisY=JOINT_HINGE;
				//if((pFemElem->cbStartJoint&0x20)==0)
				//	ciJointStartAxisZ=JOINT_HINGE;
				//if((pFemElem->cbEndJoint&0x10)==0)
				//	ciJointEndAxisY=JOINT_HINGE;
				//if((pFemElem->cbEndJoint&0x20)==0)
				//	ciJointEndAxisZ=JOINT_HINGE;
				int jointStartType=pFemElem->start_joint_type;
				int jointEndType=pFemElem->end_joint_type;
				//if((pFemElem->cbStartJoint&0x10+pFemElem->cbStartJoint&0x20)==0)	//这种写法是危害极大的隐含错误
				if((pFemElem->cbStartJoint&0x10)+(pFemElem->cbStartJoint&0x20)==0)
					jointStartType=JOINT_HINGE;
				if((pFemElem->cbEndJoint&0x10)+(pFemElem->cbEndJoint&0x20)==0)
					jointEndType=JOINT_HINGE;
				if(pElemLoad->loadType==1)
				{	//均布力(暂不考虑均面力等效到梁端时形成的等效力偶）
					double M,ql=pElemLoad->fLoad*pFemElem->length;	//kN/m *mm = N
					if(pElemLoad->directionI==1)
					{
						pStationElem->QF.x+=pElemLoad->fLoad;
						if((jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)||
							(jointStartType==JOINT_HINGE&&jointEndType==JOINT_HINGE))
						{
							pFemElem->pStart->Fxyz.x+=ql/2;
							pFemElem->pEnd->Fxyz.x  +=ql/2;
							pStationElem->F_sect_s.x+=ql/2;
							pStationElem->F_sect_e.x+=ql/2;
						}
						//累加计算均布力在单元中间断面上的内力(矩)影响
						//等效两端Ｙ及Ｚ轴方向的力偶
						if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
						{	//两端固接
							M= pFemElem->lenStdVec.z*ql*pFemElem->length/12;	//N*mm,ql^2/12
							pFemElem->pStart->Moment.y+=M;
							pFemElem->pEnd  ->Moment.y-=M;
							pStationElem->M_sect_s.y+=M;
							pStationElem->M_sect_e.y-=M;
							M=-pFemElem->lenStdVec.y*ql*pFemElem->length/12;	//N*mm,ql^2/12
							pFemElem->pStart->Moment.z+=M;
							pFemElem->pEnd  ->Moment.z-=M;
							pStationElem->M_sect_s.z+=M;
							pStationElem->M_sect_e.z-=M;
						}
						else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
						{	//始端固接,终端铰接
							pStationElem->F_sect_s.x+=ql*0.625;
							pStationElem->F_sect_e.x+=ql*0.375;
							pFemElem->pStart->Fxyz.x+=ql*0.625;	//ql*5/8
							pFemElem->pEnd->Fxyz.x  +=ql*0.375;	//ql*3/8
							M= pFemElem->lenStdVec.z*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pStart->Moment.y+=M;
							pStationElem->M_sect_s.y+=M;
							M=-pFemElem->lenStdVec.y*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pStart->Moment.z+=M;
							pStationElem->M_sect_s.z+=M;
						}
						else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
						{	//始端固接,终端铰接
							pStationElem->F_sect_s.x-=ql*0.375;
							pStationElem->F_sect_e.x-=ql*0.625;
							pFemElem->pStart->Fxyz.x+=ql*0.375;	//ql*3/8
							pFemElem->pEnd->Fxyz.x  +=ql*0.625;	//ql*5/8
							M= pFemElem->lenStdVec.z*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pEnd->Moment.y-=M;
							pStationElem->M_sect_e.y-=M;
							M=-pFemElem->lenStdVec.y*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pEnd->Moment.z-=M;
							pStationElem->M_sect_e.z-=M;
						}
					}
					else if(pElemLoad->directionI==2)
					{
						pStationElem->QF.y+=pElemLoad->fLoad;
						if((jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)||
							(jointStartType==JOINT_HINGE&&jointEndType==JOINT_HINGE))
						{
							pFemElem->pStart->Fxyz.y+=ql/2;
							pFemElem->pEnd->Fxyz.y  +=ql/2;
							pStationElem->F_sect_s.y+=ql/2;
							pStationElem->F_sect_e.y+=ql/2;
						}
						//累加计算均布力在单元中间断面上的内力(矩)影响
						//等效两端X及Z轴方向的力偶
						if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
						{	//两端固接
							M=-pFemElem->lenStdVec.z*ql*pFemElem->length/12;	//N*mm,ql^2/12
							pFemElem->pStart->Moment.x+=M;
							pFemElem->pEnd  ->Moment.x-=M;
							pStationElem->M_sect_s.x+=M;
							pStationElem->M_sect_e.x-=M;
							M= pFemElem->lenStdVec.x*ql*pFemElem->length/12;	//N*mm,ql^2/12
							pFemElem->pStart->Moment.z+=M;
							pFemElem->pEnd  ->Moment.z-=M;
							pStationElem->M_sect_s.z+=M;
							pStationElem->M_sect_e.z-=M;
						}
						else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
						{	//始端固接,终端铰接
							pStationElem->F_sect_s.y+=ql*0.625;
							pStationElem->F_sect_e.y+=ql*0.375;
							pFemElem->pStart->Fxyz.y+=ql*0.625;	//ql*5/8
							pFemElem->pEnd  ->Fxyz.y+=ql*0.375;	//ql*3/8
							M=-pFemElem->lenStdVec.z*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pStart->Moment.x+=M;
							pStationElem->M_sect_s.x+=M;
							M= pFemElem->lenStdVec.x*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pStart->Moment.z+=M;
							pStationElem->M_sect_s.z+=M;
						}
						else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
						{	//始端铰接,终端固接
							pStationElem->F_sect_s.y+=ql*0.375;
							pStationElem->F_sect_e.y+=ql*0.625;
							pFemElem->pStart->Fxyz.y+=ql*0.375;	//ql*3/8
							pFemElem->pEnd  ->Fxyz.y+=ql*0.625;	//ql*5/8
							M=-pFemElem->lenStdVec.z*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pEnd->Moment.x-=M;
							pStationElem->M_sect_e.x-=M;
							M= pFemElem->lenStdVec.x*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pEnd->Moment.z-=M;
							pStationElem->M_sect_e.z-=M;
						}
					}
					else if(pElemLoad->directionI==3)
					{
						pStationElem->QF.z+=pElemLoad->fLoad;
						if((jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)||
							(jointStartType==JOINT_HINGE&&jointEndType==JOINT_HINGE))
						{
							pFemElem->pStart->Fxyz.z+=ql/2;
							pFemElem->pEnd  ->Fxyz.z+=ql/2;
							pStationElem->F_sect_s.z+=ql/2;
							pStationElem->F_sect_e.z+=ql/2;
						}
						//累加计算均布力在单元中间断面上的内力(矩)影响
						//等效两端X及Y轴方向的力偶
						if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
						{	//两端固接
							M= pFemElem->lenStdVec.y*ql*pFemElem->length/12;	//N*mm,ql^2/12
							pFemElem->pStart->Moment.x+=M;
							pFemElem->pEnd  ->Moment.x-=M;
							pStationElem->M_sect_s.x+=M;
							pStationElem->M_sect_e.x-=M;
							M=-pFemElem->lenStdVec.x*ql*pFemElem->length/12;	//N*mm,ql^2/12
							pFemElem->pStart->Moment.y+=M;
							pFemElem->pEnd  ->Moment.y-=M;
							pStationElem->M_sect_s.y+=M;
							pStationElem->M_sect_e.y-=M;
						}
						else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
						{	//始端固接,终端铰接
							pStationElem->F_sect_s.z+=ql*0.625;
							pStationElem->F_sect_e.z+=ql*0.375;
							pFemElem->pStart->Fxyz.z+=ql*0.625;	//ql*5/8
							pFemElem->pEnd  ->Fxyz.z+=ql*0.375;	//ql*3/8
							M= pFemElem->lenStdVec.y*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pStart->Moment.x+=M;
							pStationElem->M_sect_s.x+=M;
							M=-pFemElem->lenStdVec.x*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pStart->Moment.z+=M;
							pStationElem->M_sect_s.z+=M;
						}
						else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
						{	//始端铰接,终端固接
							pStationElem->F_sect_s.z+=ql*0.375;
							pStationElem->F_sect_e.z+=ql*0.625;
							pFemElem->pStart->Fxyz.z+=ql*0.375;	//ql*3/8
							pFemElem->pEnd  ->Fxyz.z+=ql*0.625;	//ql*5/8
							M= pFemElem->lenStdVec.y*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pEnd->Moment.x-=M;
							pStationElem->M_sect_e.x-=M;
							M=-pFemElem->lenStdVec.x*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pEnd->Moment.z-=M;
							pStationElem->M_sect_e.z-=M;
						}
					}
				}
				else if(pElemLoad->loadType==2)
				{	//集中力
					double armLength;
					double scaleFactor=1000*pElemLoad->fDist/pFemElem->length;
					double P=pElemLoad->fLoad*1000;	//1000:kN->N
					//double a=scaleFactor,b=1-scaleFactor;	//定义a,b纯粹是为了代码与建筑结构手册公式方便对照
					GEPOINT fs,fe,ms,me;	//始末端等效力及等效力矩
					if(pElemLoad->directionI==1)
					{	//X向集中力
						if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_HINGE)
						{
							pFemElem->pStart->Fxyz.x+=P*(1-scaleFactor);//pElemLoad->fLoad*1000*(1-scaleFactor)
							pFemElem->pEnd  ->Fxyz.x+=P*scaleFactor;
							pStationElem->F_sect_s.x+=P*(1-scaleFactor);
							pStationElem->F_sect_e.x+=P*scaleFactor;
						}
						else
						{
							GEPOINT Faxis=P*pFemElem->lenStdVec.x*pFemElem->lenStdVec;
							pFemElem->pStart->Fxyz+=Faxis*(1-scaleFactor);
							pFemElem->pEnd  ->Fxyz+=Faxis*scaleFactor;
							pStationElem->F_sect_s+=Faxis*(1-scaleFactor);
							pStationElem->F_sect_e+=Faxis*scaleFactor;
							GEPOINT Fsect=GEPOINT(P-Faxis.x,-Faxis.y,-Faxis.z);
							if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
							{
								fs=Fsect*(1-scaleFactor)*(1-scaleFactor)*(3*scaleFactor+(1-scaleFactor));	//
								fe=Fsect*scaleFactor*scaleFactor*(scaleFactor+3*(1-scaleFactor));			//终端等效力
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｙ及Ｚ轴方向的力偶
								GEPOINT Mvec=pFemElem->lenStdVec^Fsect;
								GEPOINT MS=Mvec*scaleFactor*(1-scaleFactor)*(1-scaleFactor)*pFemElem->length;
								GEPOINT ME=Mvec*scaleFactor*scaleFactor*(1-scaleFactor)*pFemElem->length;
								pFemElem->pStart->Moment+=MS;
								pFemElem->pEnd  ->Moment-=ME;
								pStationElem->M_sect_s+=MS;
								pStationElem->M_sect_e-=ME;
							}
							else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
							{
								fs=Fsect*0.5*(1-scaleFactor)*(3-(1-scaleFactor)*(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*0.5*scaleFactor*scaleFactor*(3-scaleFactor);
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｙ及Ｚ轴方向的力偶
								armStartM.y= P*pFemElem->lenStdVec.z*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(2-scaleFactor);	//N*m
								pFemElem->pStart->Moment.y+=armStartM.y;
								armStartM.z=-P*pFemElem->lenStdVec.y*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(2-scaleFactor);	//N*m
								pFemElem->pStart->Moment.z+=armStartM.z;
								pStationElem->M_sect_s.y+=armStartM.y;
								pStationElem->M_sect_s.z+=armStartM.z;
							}
							else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
							{
								fs=Fsect*0.5*(1-scaleFactor)*(1-scaleFactor)*(3-(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*0.5*scaleFactor*(3-scaleFactor*scaleFactor);
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｙ及Ｚ轴方向的力偶
								armStartM.y= P*pFemElem->lenStdVec.z*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(1+scaleFactor);	//N*m
								pFemElem->pEnd  ->Moment.y-=armStartM.y;
								armStartM.z=-P*pFemElem->lenStdVec.y*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(1+scaleFactor);	//N*m
								pFemElem->pEnd  ->Moment.z-=armStartM.z;
								pStationElem->M_sect_e.y-=armStartM.y;
								pStationElem->M_sect_e.z-=armStartM.z;
							}
							pStationElem->F_sect_s+=fs;
							pStationElem->F_sect_e+=fe;
						}
						//累加计算均布力在单元中间断面上的内力(矩)影响
						double prev_s=0;
						for(j=0,scale=0;j<3;j++)
						{
							scale+=0.25;
							double d=pElemLoad->fDist*1000,s=scale*pFemElem->length;
							if(d<s-EPS&&d>prev_s+EPS)
							{	//集中力在截面左侧
								pStationElem->sects[j].F_L_S.x+=P;	//N
								//等效两端Ｙ及Ｚ轴方向的力偶
								armLength=(d-s)*pFemElem->lenStdVec.z;
								pStationElem->sects[j].M_L_S.y-=P*armLength;	//N*mm
								armLength=(d-s)*pFemElem->lenStdVec.y;
								pStationElem->sects[j].M_L_S.z+=P*armLength;	//N*mm
							}
							else if(d>=s-EPS&&d<=s+EPS)	//集中力在截面上
								pStationElem->sects[j].F_T_S.x+=P;	//N
							prev_s=s;
						}
					}
					else if(pElemLoad->directionI==2)
					{	//Y向集中力
						if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_HINGE)
						{
							pFemElem->pStart->Fxyz.y+=P*(1-scaleFactor);//pElemLoad->fLoad*1000*(1-scaleFactor)
							pFemElem->pEnd  ->Fxyz.y+=P*scaleFactor;
							pStationElem->F_sect_s.y+=P*(1-scaleFactor);
							pStationElem->F_sect_e.y+=P*scaleFactor;
						}
						else
						{
							GEPOINT Faxis=P*pFemElem->lenStdVec.y*pFemElem->lenStdVec;
							pFemElem->pStart->Fxyz+=Faxis*(1-scaleFactor);
							pFemElem->pEnd  ->Fxyz+=Faxis*scaleFactor;
							pStationElem->F_sect_s+=Faxis*(1-scaleFactor);
							pStationElem->F_sect_e+=Faxis*scaleFactor;
							GEPOINT Fsect=GEPOINT(-Faxis.x,P-Faxis.y,-Faxis.z);
							if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
							{
								fs=Fsect*(1-scaleFactor)*(1-scaleFactor)*(3*scaleFactor+(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*scaleFactor*scaleFactor*(scaleFactor+3*(1-scaleFactor));
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｘ及Ｚ轴方向的力偶
								GEPOINT Mvec=pFemElem->lenStdVec^Fsect;
								GEPOINT MS=Mvec*scaleFactor*(1-scaleFactor)*(1-scaleFactor)*pFemElem->length;
								GEPOINT ME=Mvec*scaleFactor*scaleFactor*(1-scaleFactor)*pFemElem->length;
								pFemElem->pStart->Moment+=MS;
								pFemElem->pEnd  ->Moment-=ME;
								pStationElem->M_sect_s+=MS;
								pStationElem->M_sect_e-=ME;
							}
							else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
							{
								fs=Fsect*0.5*(1-scaleFactor)*(3-(1-scaleFactor)*(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*0.5*scaleFactor*scaleFactor*(3-scaleFactor);
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｘ及Ｚ轴方向的力偶
								armStartM.x=-P*pFemElem->lenStdVec.z*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(2-scaleFactor);	//N*m
								pFemElem->pStart->Moment.x+=armStartM.x;
								armStartM.z= P*pFemElem->lenStdVec.x*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(2-scaleFactor);	//N*m
								pFemElem->pStart->Moment.z+=armStartM.z;
								pStationElem->M_sect_s.x+=armStartM.x;
								pStationElem->M_sect_s.z+=armStartM.z;
							}
							else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
							{
								fs=Fsect*0.5*(1-scaleFactor)*(1-scaleFactor)*(3-(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*0.5*scaleFactor*(3-scaleFactor*scaleFactor);
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｘ及Ｚ轴方向的力偶
								armStartM.x=-P*pFemElem->lenStdVec.z*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(1+scaleFactor);	//N*m
								pFemElem->pEnd  ->Moment.x-=armStartM.x;
								armStartM.z= P*pFemElem->lenStdVec.x*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(1+scaleFactor);	//N*m
								pFemElem->pEnd  ->Moment.z-=armStartM.z;
								pStationElem->M_sect_e.x-=armStartM.x;
								pStationElem->M_sect_e.z-=armStartM.z;
							}
							pStationElem->F_sect_s+=fs;
							pStationElem->F_sect_e+=fe;
						}
						//累加计算均布力在单元中间断面上的内力(矩)影响
						double prev_s=0;
						for(j=0,scale=0;j<3;j++)
						{
							scale+=0.25;
							double d=pElemLoad->fDist*1000,s=scale*pFemElem->length;
							if(d<s-EPS&&d>prev_s+EPS)
							{	//集中力在截面左侧
								pStationElem->sects[j].F_L_S.y+=P;
								//等效两端X及Ｚ轴方向的力偶
								armLength=(d-s)*pFemElem->lenStdVec.z;
								pStationElem->sects[j].M_L_S.x+=P*armLength;	//N*mm
								armLength=(d-s)*pFemElem->lenStdVec.x;
								pStationElem->sects[j].M_L_S.z-=P*armLength;	//N*mm
							}
							else if(d>=s-EPS&&d<=s+EPS)	//集中力在截面上
								pStationElem->sects[j].F_T_S.y+=P;	//N
							prev_s=s;
						}
					}
					else if(pElemLoad->directionI==3)
					{	//Z向集中力
						if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_HINGE)
						{
							pFemElem->pStart->Fxyz.z+=P*(1-scaleFactor);//pElemLoad->fLoad*1000*(1-scaleFactor)
							pFemElem->pEnd  ->Fxyz.z+=P*scaleFactor;
							pStationElem->F_sect_s.z+=P*(1-scaleFactor);
							pStationElem->F_sect_e.z+=P*scaleFactor;
						}
						else
						{	
							GEPOINT Faxis=P*pFemElem->lenStdVec.z*pFemElem->lenStdVec;
							pFemElem->pStart->Fxyz+=Faxis*(1-scaleFactor);
							pFemElem->pEnd  ->Fxyz+=Faxis*scaleFactor;
							pStationElem->F_sect_s+=Faxis*(1-scaleFactor);
							pStationElem->F_sect_e+=Faxis*scaleFactor;
							GEPOINT Fsect=GEPOINT(-Faxis.x,-Faxis.y,P-Faxis.z);
							if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
							{
								fs=Fsect*(1-scaleFactor)*(1-scaleFactor)*(3*scaleFactor+(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*scaleFactor*scaleFactor*(scaleFactor+3*(1-scaleFactor));
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｙ及Ｘ轴方向的力偶
								GEPOINT Mvec=pFemElem->lenStdVec^Fsect;
								GEPOINT MS=Mvec*scaleFactor*(1-scaleFactor)*(1-scaleFactor)*pFemElem->length;
								GEPOINT ME=Mvec*scaleFactor*scaleFactor*(1-scaleFactor)*pFemElem->length;
								pFemElem->pStart->Moment+=MS;
								pFemElem->pEnd  ->Moment-=ME;
								pStationElem->M_sect_s+=MS;
								pStationElem->M_sect_e-=ME;
							}
							else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
							{
								fs=Fsect*0.5*(1-scaleFactor)*(3-(1-scaleFactor)*(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*0.5*scaleFactor*scaleFactor*(3-scaleFactor);
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｙ及Ｘ轴方向的力偶
								armStartM.y=-P*pFemElem->lenStdVec.x*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(2-scaleFactor);	//N*m
								pFemElem->pStart->Moment.y+=armStartM.y;
								armStartM.x= P*pFemElem->lenStdVec.y*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(2-scaleFactor);	//N*m
								pFemElem->pStart->Moment.x+=armStartM.x;
								pStationElem->M_sect_s.y+=armStartM.y;
								pStationElem->M_sect_s.x+=armStartM.x;
							}
							else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
							{
								fs=Fsect*0.5*(1-scaleFactor)*(1-scaleFactor)*(3-(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*0.5*scaleFactor*(3-scaleFactor*scaleFactor);
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｙ及Ｘ轴方向的力偶
								armStartM.y=-P*pFemElem->lenStdVec.x*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(1+scaleFactor);	//N*m
								pFemElem->pEnd  ->Moment.y-=armStartM.y;
								armStartM.x= P*pFemElem->lenStdVec.y*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(1+scaleFactor);	//N*m
								pFemElem->pEnd  ->Moment.x-=armStartM.x;
								pStationElem->M_sect_e.y-=armStartM.y;
								pStationElem->M_sect_e.x-=armStartM.x;
							}
							pStationElem->F_sect_s+=fs;
							pStationElem->F_sect_e+=fe;
						}
						//累加计算均布力在单元中间断面上的内力(矩)影响
						double prev_s=0;
						for(j=0,scale=0;j<3;j++)
						{
							scale+=0.25;
							double d=pElemLoad->fDist*1000,s=scale*pFemElem->length;
							if(d<s-EPS&&d>prev_s+EPS)
							{	//集中力在截面左侧（或截面上）
								pStationElem->sects[j].F_L_S.z+=P;
								//等效两端X及Y轴方向的力偶
								armLength=(d-s)*pFemElem->lenStdVec.y;
								pStationElem->sects[j].M_L_S.x-=P*armLength;	//N*mm
								armLength=(d-s)*pFemElem->lenStdVec.x;
								pStationElem->sects[j].M_L_S.y+=P*armLength;	//N*mm
							}
							else if(d>=s-EPS&&d<=s+EPS)	//集中力在截面上
								pStationElem->sects[j].F_T_S.z+=P;	//N
							prev_s=s;
						}
					}
				}
				else if(pElemLoad->loadType==3)
				{	//集中力偶
					double M=pElemLoad->fLoad*1000000;	//kN*m->N*mm
					double scaleFactor=1000*pElemLoad->fDist/pFemElem->length;
					GEPOINT Mv,M2Axis,M2Sect;	//集中力偶轴向扭矩投影分量及横截面弯矩投影分量,N*m
					if(pElemLoad->directionI==1)
					{	//X向集中力偶
						M2Axis=pFemElem->lenStdVec*(pFemElem->lenStdVec.x*M);
						M2Sect.Set(M-M2Axis.x,-M2Axis.y,-M2Axis.z);
						Mv.Set(M,0,0);
					}
					else if(pElemLoad->directionI==2)
					{	//Y向集中力偶
						M2Axis=pFemElem->lenStdVec*(pFemElem->lenStdVec.y*M);
						M2Sect.Set(-M2Axis.x,M-M2Axis.y,-M2Axis.z);
						Mv.Set(0,M,0);
					}
					else if(pElemLoad->directionI==3)
					{	//Z向集中力偶
						M2Axis=pFemElem->lenStdVec*(pFemElem->lenStdVec.z*M);
						M2Sect.Set(-M2Axis.x,-M2Axis.y,M-M2Axis.z);
						Mv.Set(0,0,M);
					}
					//等效横向集中力偶产生的端截面反作用力
					GEPOINT Rvec=(pFemElem->lenStdVec^M2Sect)/pFemElem->length;
					if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
					{
						Rvec*=6*scaleFactor*(1-scaleFactor);
						//等效两端截面的横向弯矩
						GEPOINT ms=M2Sect*(1-scaleFactor)*(2*scaleFactor-(1-scaleFactor));
						GEPOINT me=M2Sect*scaleFactor*(2*(1-scaleFactor)-scaleFactor);
						pFemElem->pStart->Moment+=ms;
						pFemElem->pEnd  ->Moment+=me;
						pStationElem->M_sect_s+=ms;
						pStationElem->M_sect_e+=me;
					}
					else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
					{
						Rvec*=1.5*(1-(1-scaleFactor)*(1-scaleFactor));
						//等效两端截面的横向弯矩
						GEPOINT ms=0.5*M2Sect*(1-3*(1-scaleFactor)*(1-scaleFactor));
						pFemElem->pStart->Moment+=ms;
						pStationElem->M_sect_s+=ms;
					}
					else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
					{
						Rvec*=1.5*(1-scaleFactor*scaleFactor);
						//等效两端截面的横向弯矩
						GEPOINT me=0.5*M2Sect*(1-3*scaleFactor*scaleFactor);
						pFemElem->pEnd  ->Moment+=me;
						pStationElem->M_sect_e+=me;
					}
					//else if(pFemElem->start_joint_type==JOINT_HINGE&&pFemElem->end_joint_type==JOINT_HINGE)
					//	Rvec*=1;
					pStationElem->F_sect_s+=Rvec;
					pStationElem->F_sect_e-=Rvec;
					pFemElem->pStart->Fxyz+=Rvec;
					pFemElem->pEnd  ->Fxyz-=Rvec;

					//等效轴向扭矩在端截面产生的反作用力矩
					bool bShaftStart=(pFemElem->cbStartJoint&0x08)>0;
					bool bShaftEnd  =(pFemElem->cbEndJoint&0x08)>0;
					if(bShaftStart&&bShaftEnd)
					{
						//等效集中力偶的扭矩投影分量（线性比例等效）
						GEPOINT ms=M2Axis*(1-scaleFactor);
						GEPOINT me=M2Axis*scaleFactor;
						pFemElem->pStart->Moment+=ms;
						pFemElem->pEnd  ->Moment+=me;
						pStationElem->M_sect_s+=ms;
						pStationElem->M_sect_e+=me;
					}
					else if(bShaftStart)
					{	//等效扭矩至始端固接点
						pFemElem->pStart->Moment+=M2Axis;
						pStationElem->M_sect_s+=M2Axis;
					}
					else if(bShaftEnd)
					{	//等效扭矩至终端固接点
						pFemElem->pEnd  ->Moment+=M2Axis;
						pStationElem->M_sect_e+=M2Axis;
					}
					//累加计算单元集中力偶在单元中间断面上的内力(矩)影响
					double prev_s=0;
					for(j=0,scale=0;j<3;j++)
					{
						scale+=0.25;
						double d=pElemLoad->fDist*1000,s=scale*pFemElem->length;
						if(d<s-EPS&&d>prev_s+EPS)
						{	//集中力偶在截面左侧（或截面上）
							pStationElem->sects[j].M_L_S+=Mv;
						}
						else if(d>=s-EPS&&d<=s+EPS)	//集中力在截面上
							pStationElem->sects[j].M_T_S+=Mv;	//N
						prev_s=s;
					}
				}
			}
			CHashList<GENERIC_FORCE> hashFemNodeWeight;
			CHashList<ELEMSECT_FORCE> hashElemSectForce;
			for(pFemNode=listFemNodes.GetFirst(),pStationNode=hashSubstationNodes.GetFirst();pFemNode;
				pFemNode=listFemNodes.GetNext(),pStationNode=hashSubstationNodes.GetNext())
			{
				if(pFemNode->Fxyz.IsZero())
					continue;
				if(pStationNode&&pStationNode->pLdsNode!=NULL)
				{
					CExternalNodeLoad* pExterNodeLoad=pLdsCase->hashNodeLoad.Add(pStationNode->pLdsNode->handle);
					pExterNodeLoad->designLoad=pFemNode->Fxyz;
				}
				if(fabs(pLoadCase->fGamaG1-pLoadCase->fGamaG2)>=0.1)
				{
					GENERIC_FORCE gf(pFemNode->Fxyz,pFemNode->Moment);
					hashFemNodeWeight.SetValue(pFemNode->point_i,gf);
				}
			}
			if(fabs(pLoadCase->fGamaG1-pLoadCase->fGamaG2)>=0.1)
			{
				for(pStationElem=hashSubstationElems.GetFirst();pStationElem;pStationElem=hashSubstationElems.GetNext())
				{
					ELEMSECT_FORCE sect;
					sect.Fs=GENERIC_FORCE(pStationElem->F_sect_s,pStationElem->M_sect_s);
					sect.Fe=GENERIC_FORCE(pStationElem->F_sect_e,pStationElem->M_sect_e);
					hashElemSectForce.SetValue(pStationElem->elemI,sect);
				}
			}
			//3.施加重力荷载
			double weightCoef=1.0;	//也可是1.2
			for(int iWeightStyle=0;iWeightStyle<2;iWeightStyle++)
			{
				if(iWeightStyle==0)
					weightCoef=pLoadCase->fGamaG1;
				else if(iWeightStyle==1)
				{
					weightCoef=pLoadCase->fGamaG2;
					if(fabs(pLoadCase->fGamaG1-pLoadCase->fGamaG2)<=0.1)
						continue;
				}
				if(iWeightStyle==1)
				{	//恢复无重力模式下的原外荷载
					GENERIC_FORCE* pGF=NULL;
					ELEMSECT_FORCE* pSectF=NULL;
					for(pFemNode=listFemNodes.GetFirst();pFemNode;pFemNode=listFemNodes.GetNext())
					{
						pFemNode->Fxyz.Set();
						if((pGF=hashFemNodeWeight.GetValue(pFemNode->point_i))!=NULL)
						{
							pFemNode->Fxyz=pGF->Fxyz;
							pFemNode->Moment=pGF->Moment;
						}
					}
					for(pStationElem=hashSubstationElems.GetFirst();pStationElem;pStationElem=hashSubstationElems.GetNext())
					{
						if((pSectF=hashElemSectForce.GetValue(pStationElem->elemI))!=NULL)
						{
							pStationElem->F_sect_s=pSectF->Fs.Fxyz;
							pStationElem->M_sect_s=pSectF->Fs.Moment;
							pStationElem->F_sect_e=pSectF->Fe.Fxyz;
							pStationElem->M_sect_e=pSectF->Fe.Moment;
						}
					}
				}
				for(pFemElem=listFemElems.GetFirst(),pStationElem=hashSubstationElems.GetFirst();pFemElem;
					pFemElem=listFemElems.GetNext(),pStationElem=hashSubstationElems.GetNext())
				{
					double M=0,ql=pFemElem->density*pFemElem->length*pFemElem->A*10*weightCoef;//N
					//pFemElem->pStart->Fxyz.z+=weightFz*0.5;
					//pFemElem->pEnd  ->Fxyz.z+=weightFz*0.5;
					pStationElem->QWF=pFemElem->density*pFemElem->A*10*weightCoef;
					int jointStartType=pFemElem->start_joint_type;
					int jointEndType=pFemElem->end_joint_type;
					if((pFemElem->cbStartJoint&0x10)+(pFemElem->cbStartJoint&0x20)==0)
						jointStartType=JOINT_HINGE;
					if((pFemElem->cbEndJoint&0x10)+(pFemElem->cbEndJoint&0x20)==0)
						jointEndType=JOINT_HINGE;
					if((jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)||
						(jointStartType==JOINT_HINGE&&jointEndType==JOINT_HINGE))
					{
						pFemElem->pStart->Fxyz.z+=ql/2;
						pFemElem->pEnd  ->Fxyz.z+=ql/2;
						pStationElem->F_sect_s.z+=ql/2;
						pStationElem->F_sect_e.z+=ql/2;
					}
					//累加计算均布力在单元中间断面上的内力(矩)影响
					//等效两端X及Y轴方向的力偶
					if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
					{	//两端固接
						M= pFemElem->lenStdVec.y*ql*pFemElem->length/12;	//N*mm,ql^2/12
						pFemElem->pStart->Moment.x+=M;
						pFemElem->pEnd  ->Moment.x-=M;
						pStationElem->M_sect_s.x+=M;
						pStationElem->M_sect_e.x-=M;
						M=-pFemElem->lenStdVec.x*ql*pFemElem->length/12;	//N*mm,ql^2/12
						pFemElem->pStart->Moment.y+=M;
						pFemElem->pEnd  ->Moment.y-=M;
						pStationElem->M_sect_s.y+=M;
						pStationElem->M_sect_e.y-=M;
					}
					else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
					{	//始端固接,终端铰接
						pStationElem->F_sect_s.z+=ql*0.625;
						pStationElem->F_sect_e.z+=ql*0.375;
						pFemElem->pStart->Fxyz.z+=ql*0.625;	//ql*5/8
						pFemElem->pEnd  ->Fxyz.z+=ql*0.375;	//ql*3/8
						M= pFemElem->lenStdVec.y*ql*pFemElem->length/8;	//N*mm,ql^2/8
						pFemElem->pStart->Moment.x+=M;
						pStationElem->M_sect_s.x+=M;
						M=-pFemElem->lenStdVec.x*ql*pFemElem->length/8;	//N*mm,ql^2/8
						pFemElem->pStart->Moment.z+=M;
						pStationElem->M_sect_s.z+=M;
					}
					else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
					{	//始端铰接,终端固接
						pStationElem->F_sect_s.z+=ql*0.375;
						pStationElem->F_sect_e.z+=ql*0.625;
						pFemElem->pStart->Fxyz.z+=ql*0.375;	//ql*3/8
						pFemElem->pEnd  ->Fxyz.z+=ql*0.625;	//ql*5/8
						M= pFemElem->lenStdVec.y*ql*pFemElem->length/8;	//N*mm,ql^2/8
						pFemElem->pEnd->Moment.x-=M;
						pStationElem->M_sect_e.x-=M;
						M=-pFemElem->lenStdVec.x*ql*pFemElem->length/8;	//N*mm,ql^2/8
						pFemElem->pEnd->Moment.z-=M;
						pStationElem->M_sect_e.z-=M;
					}
				}
				BOOL rslt;
				if(bFirstResolve)
				{
					//if(g_sysPara.AnalysisPara.m_iResolverStyle==0)
					//	rslt=pResolver->truss_resolve(&listFemNodes,&listFemElems,GAUSS_REDUCTION,false);
					//else
						rslt=pResolver->truss_resolve(&listFemNodes,&listFemElems,WAVE_FRONT_METHOD,false);
				}
				else
					rslt=pResolver->truss_equation_recur(&listFemNodes,&listFemElems);
				bFirstResolve=false;
				for(pFemElem=listFemElems.GetFirst();pFemElem;pFemElem=listFemElems.GetNext())
					pFemElem->pTag=NULL;
				if(filename!=NULL)
					ExportResultTxtFile(filename,iCaseNo,weightCoef);
				else
					ExportResultBuffer(pLoadCase,iWeightStyle);
			}
		}
		timer.End();
	}
	catch (char* error_info){	//防止中间抛出异常导致无法销毁pResolver空占内存　wjh-2015.10.21
		AfxMessageBox(error_info);
	}
	//postreport.hashNodeAnalBuffer.RebuildHashTable();
	//postreport.hashElemAnalBuffer.RebuildHashTable();
	CFemResolver::Destroy(pResolver->GetHandle());
	return true;
	//AfxMessageBox(CXhChar50("time elapse %.1fs!",(timer.GetEndTicks()-timer.GetStartTicks())*0.001));
}
void CFemModel::Empty()
{
	hashSubstationNodes.Empty();
	hashSubstationElems.Empty();
	hashSubstationMaterial.Empty();
	hashSubstationSections.Empty();
	listCaseLoads.Empty();
	listFemNodes.Empty();	//LDS有限元计算模型
	listFemElems.Empty();
}
void CFemModel::ModelFromBuffer(CBuffer& buffer)
{
	Empty();
	int i,n,id;
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&id);
		FEMMODEL_NODE* pStationNode=hashSubstationNodes.Add(id);
		pStationNode->pointI=id;
		buffer.ReadPoint(pStationNode->pos);
		buffer.ReadWord(&pStationNode->wRestrict);
	}
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&id);
		FEMMODEL_ELEM* pStationElem=hashSubstationElems.Add(id);
		pStationElem->elemI=id;
		buffer.ReadInteger(&pStationElem->startPointI);
		buffer.ReadInteger(&pStationElem->endPointI);
		buffer.ReadByte(&pStationElem->cbJointStart);	//始端约束信息
		buffer.ReadByte(&pStationElem->cbJointEnd);		//终端约束信息
		buffer.ReadInteger(&pStationElem->sectionI);	//截面标识号
	}
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&id);
		FEMMODEL_MATERIAL* pMaterial=hashSubstationMaterial.Add(id);
		buffer.ReadDouble(&pMaterial->E);
		buffer.ReadDouble(&pMaterial->shear);
		buffer.ReadDouble(&pMaterial->poson);
		buffer.ReadDouble(&pMaterial->density);
		//buffer.ReadDouble(&pMaterial->fy);	//屈服强度
		//buffer.ReadDouble(&pMaterial->ft);	//设计强度
	}
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&id);
		FEMMODEL_SECTION* pSection=hashSubstationSections.Add(id);
		pSection->sectionI=id;
		buffer.ReadDword(&pSection->materialI);
		buffer.ReadDouble(&pSection->area);
		buffer.ReadDouble(&pSection->IX);
		buffer.ReadDouble(&pSection->IY);
		buffer.ReadDouble(&pSection->IZ);
		//buffer.ReadByte(&cRodSectType);		//杆件规格类型，1.角钢;2.钢管
		//buffer.ReadDouble(&sizeWidth);		//肢宽、直径等
		//buffer.ReadDouble(&sizeThick);		//肢厚、壁厚等
		//buffer.ReadDouble(&sizeReserved);	//槽钢高度等
	}
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		FEMMODEL_LOAD* pLoad=listCaseLoads.append();
		buffer.ReadDouble(&pLoad->fGamaG1);
		buffer.ReadDouble(&pLoad->fGamaG2);
		int j,m;
		buffer.ReadInteger(&m);
		for(j=0;j<m;j++)
		{
			FEMMODEL_NODE_LOAD* pNodeLoad=pLoad->nodeLoadList.append();
			buffer.ReadInteger(&pNodeLoad->pointI);
			buffer.ReadByte(&pNodeLoad->directionI);//广义力方向
			buffer.ReadDouble(&pNodeLoad->fLoad);
		}
		buffer.ReadInteger(&m);
		for(j=0;j<m;j++)
		{
			FEMMODEL_ELEM_LOAD* pElemLoad=pLoad->elemLoadList.append();
			buffer.ReadInteger(&pElemLoad->elemI);
			buffer.ReadByte(&pElemLoad->loadType);	//广义力类型
			buffer.ReadByte(&pElemLoad->directionI);//广义力方向
			buffer.ReadDouble(&pElemLoad->fDist);	//广义力在杆件上的作用点纵向位置（始->终）
			buffer.ReadDouble(&pElemLoad->fLoad);
		}
	}
}
void CFemModel::ModelToBuffer(CBuffer& buffer)
{
	buffer.WriteInteger(hashSubstationNodes.GetNodeNum());
	for(FEMMODEL_NODE* pStationNode=hashSubstationNodes.GetFirst();pStationNode;pStationNode=hashSubstationNodes.GetNext())
	{
		buffer.WriteInteger(pStationNode->pointI);
		buffer.WritePoint(pStationNode->pos);
		buffer.WriteWord(pStationNode->wRestrict);
	}
	buffer.WriteInteger(hashSubstationElems.GetNodeNum());
	for(FEMMODEL_ELEM* pStationElem=hashSubstationElems.GetFirst();pStationElem;pStationElem=hashSubstationElems.GetNext())
	{
		buffer.WriteInteger(pStationElem->elemI);
		buffer.WriteInteger(pStationElem->startPointI);
		buffer.WriteInteger(pStationElem->endPointI);
		buffer.WriteByte(pStationElem->cbJointStart);	//始端约束信息
		buffer.WriteByte(pStationElem->cbJointEnd);		//终端约束信息
		buffer.WriteInteger(pStationElem->sectionI);	//截面标识号
	}
	buffer.WriteInteger(hashSubstationMaterial.GetNodeNum());
	for(FEMMODEL_MATERIAL* pMaterial=hashSubstationMaterial.GetFirst();pMaterial;pMaterial=hashSubstationMaterial.GetNext())
	{
		buffer.WriteDword(hashSubstationMaterial.GetCursorKey());
		buffer.WriteDouble(pMaterial->E);
		buffer.WriteDouble(pMaterial->shear);
		buffer.WriteDouble(pMaterial->poson);
		buffer.WriteDouble(pMaterial->density);
	}
	buffer.WriteInteger(hashSubstationSections.GetNodeNum());
	for(FEMMODEL_SECTION* pSection=hashSubstationSections.GetFirst();pSection;pSection=hashSubstationSections.GetNext())
	{
		buffer.WriteDword(pSection->sectionI);//hashSubstationSections.GetCursorKey());
		buffer.WriteDword(pSection->materialI);
		buffer.WriteDouble(pSection->area);
		buffer.WriteDouble(pSection->IX);
		buffer.WriteDouble(pSection->IY);
		buffer.WriteDouble(pSection->IZ);
	}
	buffer.WriteInteger(listCaseLoads.GetNodeNum());
	for(FEMMODEL_LOAD* pLoad=listCaseLoads.GetFirst();pLoad;pLoad=listCaseLoads.GetNext())
	{
		buffer.WriteDouble(pLoad->fGamaG1);
		buffer.WriteDouble(pLoad->fGamaG2);
		buffer.WriteInteger(pLoad->nodeLoadList.GetNodeNum());
		for(FEMMODEL_NODE_LOAD* pNodeLoad=pLoad->nodeLoadList.GetFirst();pNodeLoad;pNodeLoad=pLoad->nodeLoadList.GetNext())
		{
			buffer.WriteInteger(pNodeLoad->pointI);
			buffer.WriteByte(pNodeLoad->directionI);//广义力方向
			buffer.WriteDouble(pNodeLoad->fLoad);
		}
		buffer.WriteInteger(pLoad->elemLoadList.GetNodeNum());
		for(FEMMODEL_ELEM_LOAD* pElemLoad=pLoad->elemLoadList.GetFirst();pElemLoad;pElemLoad=pLoad->elemLoadList.GetNext())
		{
			buffer.WriteInteger(pElemLoad->elemI);
			buffer.WriteByte(pElemLoad->loadType);	//广义力类型
			buffer.WriteByte(pElemLoad->directionI);//广义力方向
			buffer.WriteDouble(pElemLoad->fDist);	//广义力在杆件上的作用点纵向位置（始->终）
			buffer.WriteDouble(pElemLoad->fLoad);
		}
	}
}
UINT CFemModel::TestRsltsBufLength()
{
	UINT size=4;
	for(FEMMODEL_LOAD* pCase=listCaseLoads.GetFirst();pCase;pCase=listCaseLoads.GetNext())
	{
		size+=8;
		for(FEMMODEL_LOAD::FEMNODE_RSLT* pNodeRslt=pCase->listNodeRslts.EnumObjectFirst();pNodeRslt;pNodeRslt=pCase->listNodeRslts.EnumObjectNext())
			size+=56;
		size+=4;
		for(FEMMODEL_LOAD::FEMELEM_RSLT* pElemRslt=pCase->listElemRslts.EnumObjectFirst();pElemRslt;pElemRslt=pCase->listElemRslts.EnumObjectNext())
			size+=80+384;
	}
	return size;
}
void CFemModel::RsltsToBuffer(CBuffer& buffer)
{
	buffer.WriteInteger(listCaseLoads.GetNodeNum());
	int idCase=1;
	for(FEMMODEL_LOAD* pCase=listCaseLoads.GetFirst();pCase;pCase=listCaseLoads.GetNext(),idCase++)
	{
		buffer.WriteInteger(idCase);
		buffer.PushPositionStack();
		buffer.WriteInteger(0);
		for(FEMMODEL_LOAD::FEMNODE_RSLT* pNodeRslt=pCase->listNodeRslts.EnumObjectFirst();pNodeRslt;pNodeRslt=pCase->listNodeRslts.EnumObjectNext())
		{
			buffer.WriteInteger(pNodeRslt->pointI);
			buffer.WriteInteger(pNodeRslt->idCase);	//iWeightCoefStyle
			buffer.WritePoint(pNodeRslt->vDeita);
			buffer.WritePoint(pNodeRslt->vRotTheta);
			buffer.IncrementLevelCount(1);
		}
		buffer.PopPositionStack();
		buffer.WriteInteger(buffer.LevelCount(1));
		buffer.RestoreNowPosition();

		buffer.PushPositionStack();
		buffer.WriteInteger(0);
		for(FEMMODEL_LOAD::FEMELEM_RSLT* pElemRslt=pCase->listElemRslts.EnumObjectFirst();pElemRslt;pElemRslt=pCase->listElemRslts.EnumObjectNext())
		{
			buffer.WriteInteger(pElemRslt->elemI);
			buffer.WriteInteger(pElemRslt->idCase);	//iWeightCoefStyle
			buffer.WritePoint(pElemRslt->lcs.axis_x);		//始->终为X轴正向的受力分析时的局部坐标系（在全局坐标系下描述）
			buffer.WritePoint(pElemRslt->lcs.axis_y);		//始->终为X轴正向的受力分析时的局部坐标系（在全局坐标系下描述）
			buffer.WritePoint(pElemRslt->lcs.axis_z);		//始->终为X轴正向的受力分析时的局部坐标系（在全局坐标系下描述）
			//GEPOINT vStartForce,vStartMoment;	//始端截面内力（局部坐标系）
			//GEPOINT vEndForce,vEndMoment;		//终端截面内力（局部坐标系）
			for(int j=0;j<4;j++)
			{
				buffer.WritePoint(pElemRslt->segments[j].vForceStart);
				buffer.WritePoint(pElemRslt->segments[j].vMomentStart);
				buffer.WritePoint(pElemRslt->segments[j].vForceEnd);
				buffer.WritePoint(pElemRslt->segments[j].vMomentEnd);
			}
			buffer.IncrementLevelCount(1);
		}
		buffer.PopPositionStack();
		buffer.WriteInteger(buffer.LevelCount(1));
		buffer.RestoreNowPosition();
	}
}
//装计算结果导出成内存数据块
void CFemModel::ExportResultBuffer(FEMMODEL_LOAD* pCase,int iWeightCoefStyle)
{
	FEMMODEL_NODE* pStationNode;
	FEMMODEL_ELEM* pStationElem;
	FEM_NODE* pFemNode;
	FEM_ELEMENT* pFemElem;

	for(pFemNode=listFemNodes.GetFirst(),pStationNode=hashSubstationNodes.GetFirst();pFemNode;
		pFemNode=listFemNodes.GetNext(),pStationNode=hashSubstationNodes.GetNext())
	{
		FEMMODEL_LOAD::FEMNODE_RSLT* pNodeRslt=pCase->listNodeRslts.AttachObject();
		pNodeRslt->pointI=pStationNode->pointI;
		pNodeRslt->idCase=iWeightCoefStyle;
		pNodeRslt->vDeita=pFemNode->offset;
		pNodeRslt->vRotTheta=pFemNode->rot_offset;
	}
	for(pFemElem=listFemElems.GetFirst(),pStationElem=hashSubstationElems.GetFirst();pFemElem;
		pFemElem=listFemElems.GetNext(),pStationElem=hashSubstationElems.GetNext())
	{
		FEMMODEL_LOAD::FEMELEM_RSLT* pElemRslt=pCase->listElemRslts.AttachObject();
		pElemRslt->idCase=iWeightCoefStyle;
		pElemRslt->elemI=pStationElem->elemI;
		GEPOINT SF,SM,EF,EM;	//节间始末端断面内力和内力矩
		GEPOINT QF=pStationElem->QF;
		QF.z+=pStationElem->QWF;	//将重力荷载折算成分布荷载处理
		GECS cs;
		cs.axis_x=pFemElem->pEnd->pos-pFemElem->pStart->pos;
		normalize(cs.axis_x);
		cs.axis_z=CalLocalCS_Z_AXIS(cs.axis_x);
		cs.axis_y=cs.axis_z^cs.axis_x;
		pElemRslt->lcs=cs;	//赋值有限元计算局部坐标系
		//1.整杆始末端断面内力信息输出
		SF=-(pFemElem->start_force-pStationElem->F_sect_s);
		SM=-(pFemElem->start_moment-pStationElem->M_sect_s);
		EF=pFemElem->end_force-pStationElem->F_sect_e;
		EM=pFemElem->end_moment-pStationElem->M_sect_e;
		GEPOINT sf=cs.TransVToCS(SF);
		GEPOINT ef=cs.TransVToCS(EF);
		GEPOINT sm=cs.TransVToCS(SM);
		GEPOINT em=cs.TransVToCS(EM);
		pElemRslt->segments[0].vForceStart=sf;
		pElemRslt->segments[0].vMomentStart=sm;
		pElemRslt->segments[3].vForceEnd=ef;
		pElemRslt->segments[3].vMomentEnd=em;
		//j+1号－j+2号断面内力信息输出
		if( !pStationElem->sects[0].F_T_S.IsZero()||!pStationElem->sects[0].F_L_S.IsZero()||!pStationElem->sects[0].M_T_S.IsZero()||!pStationElem->sects[0].M_L_S.IsZero()||
			!pStationElem->sects[1].F_T_S.IsZero()||!pStationElem->sects[1].F_L_S.IsZero()||!pStationElem->sects[1].M_T_S.IsZero()||!pStationElem->sects[1].M_L_S.IsZero()||
			!pStationElem->sects[2].F_T_S.IsZero()||!pStationElem->sects[2].F_L_S.IsZero()||!pStationElem->sects[2].M_T_S.IsZero()||!pStationElem->sects[2].M_L_S.IsZero()||
			!QF.IsZero())//pStationElem->cbJointStart>0x07||pStationElem->cbJointEnd>0x07||
		{
			for(int j=0;j<4;j++)
			{
				EF=SF;
				EM=SM;
				if(j<3)
				{
					EF+=pStationElem->sects[j].F_L_S;
					EM+=pStationElem->sects[j].M_L_S;
				}
				if(!QF.IsZero())
				{
					EF-=QF*0.25*pFemElem->length;
					GEPOINT MQ=0.03125*QF^pFemElem->lenStdVec;
					EM-=MQ*pFemElem->length*pFemElem->length;
				}
				EM+=SF^pFemElem->lenStdVec*(pFemElem->length*0.25);
				sf=cs.TransVToCS(SF);
				ef=cs.TransVToCS(EF);
				sm=cs.TransVToCS(SM);
				em=cs.TransVToCS(EM);
				pElemRslt->segments[j].vForceStart=sf;
				pElemRslt->segments[j].vMomentStart=sm;
				pElemRslt->segments[j].vForceEnd=ef;
				pElemRslt->segments[j].vMomentEnd=em;
				//fprintf(fp,"%d.  %d.  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f\n",pStationElem->elemI,j+2,
				//	-sf.x*0.001, sf.y*0.001, sf.z*0.001,-sm.x*1e-6 , sm.y*1e-6 , sm.z*1e-6,
				//	ef.x*0.001,-ef.y*0.001,-ef.z*0.001, em.x*1e-6 ,-em.y*1e-6 ,-em.z*1e-6);
				if(j==3)
					break;
				SF=EF-pStationElem->sects[j].F_T_S;
				SM=EM-pStationElem->sects[j].M_T_S;
			}
		}
	}
}
//将计算数据导出成文本文件
bool CFemModel::ExportResultTxtFile(const char* filename,int iCaseNo,double weightCoef)	//将计算数据导出成文本文件
{
	FEMMODEL_NODE* pStationNode;
	FEMMODEL_ELEM* pStationElem;
	FEM_NODE* pFemNode;
	FEM_ELEMENT* pFemElem;

	CXhChar200 fn(filename);
	char* separtor=SearchChar(fn,'.',true);
	*separtor=0;
	fn.Append(CXhChar50("_%d_weiyi#G%d.txt",iCaseNo,ftoi(weightCoef*10)));
	FILE* fp=fopen(fn,"wt");
	for(pFemNode=listFemNodes.GetFirst(),pStationNode=hashSubstationNodes.GetFirst();pFemNode;
		pFemNode=listFemNodes.GetNext(),pStationNode=hashSubstationNodes.GetNext())
	{
		fprintf(fp,"%d.  %f  %f  %f  %f  %f  %f\n",pStationNode->pointI,
			pFemNode->offset.x,-pFemNode->offset.y,-pFemNode->offset.z,
			pFemNode->rot_offset.x,-pFemNode->rot_offset.y,-pFemNode->rot_offset.z);
		/*
		CNodeAnalyzeInfo nodeAnalInfo;
		//初始化键值区
		nodeAnalInfo.m_hNode=pStationNode->pLdsNode->handle;
		nodeAnalInfo.m_iModuleNo=pModule->iNo;
		nodeAnalInfo.legword.dwQuadLeg=pModule->ActiveQuadLegDword();
		nodeAnalInfo.m_dwStatusNo=iCaseNo;
		//初始化数据区
		nodeAnalInfo.SetZero();
		nodeAnalInfo.offset=pFemNode->offset;
		nodeAnalInfo.rot_offset=pFemNode->rot_offset;
		nodeAnalInfo.external_force=pFemNode->Fxyz;
		nodeAnalInfo.internal_moment=-pFemNode->Moment;
		nodeAnalInfo.unbalance_force=pFemNode->unbalance_force;
		postreport.AppendToAnalyzeBuffer(nodeAnalInfo);
		*/
	}
	fclose(fp);
	fn.Copy(filename);
	separtor=SearchChar(fn,'.',true);
	*separtor=0;
	fn.Append(CXhChar50("_%d_li#G%d.txt",iCaseNo,ftoi(weightCoef*10)));
	fp=fopen(fn,"wt");
	for(pFemElem=listFemElems.GetFirst(),pStationElem=hashSubstationElems.GetFirst();pFemElem;
		pFemElem=listFemElems.GetNext(),pStationElem=hashSubstationElems.GetNext())
	{
		GEPOINT SF,SM,EF,EM;	//节间始末端断面内力和内力矩
		GEPOINT QF=pStationElem->QF;
		QF.z+=pStationElem->QWF;	//将重力荷载折算成分布荷载处理
		GECS cs;
		cs.axis_x=pFemElem->pEnd->pos-pFemElem->pStart->pos;
		normalize(cs.axis_x);
		cs.axis_z=CalLocalCS_Z_AXIS(cs.axis_x);
		cs.axis_y=cs.axis_z^cs.axis_x;
		//1.整杆始末端断面内力信息输出
		SF=-(pFemElem->start_force-pStationElem->F_sect_s);
		SM=-(pFemElem->start_moment-pStationElem->M_sect_s);
		EF=pFemElem->end_force-pStationElem->F_sect_e;
		EM=pFemElem->end_moment-pStationElem->M_sect_e;
		GEPOINT sf=cs.TransVToCS(SF);
		GEPOINT ef=cs.TransVToCS(EF);
		GEPOINT sm=cs.TransVToCS(SM);
		GEPOINT em=cs.TransVToCS(EM);
		fprintf(fp,"%d{%d-%d}.  1.  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f\n",pStationElem->elemI,pStationElem->startPointI,pStationElem->endPointI,
			-sf.x*0.001, sf.y*0.001, sf.z*0.001,-sm.x*1e-6 , sm.y*1e-6 , sm.z*1e-6,
			ef.x*0.001,-ef.y*0.001,-ef.z*0.001, em.x*1e-6 ,-em.y*1e-6 ,-em.z*1e-6);
		//fprintf(fp,"%d.  1.  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f\n",pStationElem->elemI,
		//	SF.x*0.001,-SF.y*0.001,-SF.z*0.001,SM.x*1e-6,-SM.y*1e-6,-SM.z*1e-6,
		//	EF.x*0.001,-EF.y*0.001,-EF.z*0.001,EM.x*1e-6,-EM.y*1e-6,-EM.z*1e-6);
		//j+1号－j+2号断面内力信息输出
		if( !pStationElem->sects[0].F_T_S.IsZero()||!pStationElem->sects[0].F_L_S.IsZero()||!pStationElem->sects[0].M_T_S.IsZero()||!pStationElem->sects[0].M_L_S.IsZero()||
			!pStationElem->sects[1].F_T_S.IsZero()||!pStationElem->sects[1].F_L_S.IsZero()||!pStationElem->sects[1].M_T_S.IsZero()||!pStationElem->sects[1].M_L_S.IsZero()||
			!pStationElem->sects[2].F_T_S.IsZero()||!pStationElem->sects[2].F_L_S.IsZero()||!pStationElem->sects[2].M_T_S.IsZero()||!pStationElem->sects[2].M_L_S.IsZero()||
			!QF.IsZero())//pStationElem->cbJointStart>0x07||pStationElem->cbJointEnd>0x07||

		{
			for(int j=0;j<4;j++)
			{
				EF=SF;
				EM=SM;
				if(j<3)
				{
					EF+=pStationElem->sects[j].F_L_S;
					EM+=pStationElem->sects[j].M_L_S;
				}
				if(!QF.IsZero())
				{
					EF-=QF*0.25*pFemElem->length;
					GEPOINT MQ=0.03125*QF^pFemElem->lenStdVec;
					EM-=MQ*pFemElem->length*pFemElem->length;
				}
				EM+=SF^pFemElem->lenStdVec*(pFemElem->length*0.25);
				sf=cs.TransVToCS(SF);
				ef=cs.TransVToCS(EF);
				sm=cs.TransVToCS(SM);
				em=cs.TransVToCS(EM);
				fprintf(fp,"%d.  %d.  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f\n",pStationElem->elemI,j+2,
					-sf.x*0.001, sf.y*0.001, sf.z*0.001,-sm.x*1e-6 , sm.y*1e-6 , sm.z*1e-6,
					ef.x*0.001,-ef.y*0.001,-ef.z*0.001, em.x*1e-6 ,-em.y*1e-6 ,-em.z*1e-6);
				if(j==3)
					break;
				SF=EF-pStationElem->sects[j].F_T_S;
				SM=EM-pStationElem->sects[j].M_T_S;
			}
		}
		/*
		ELEM_ANALYZE_INFO elemAnalInfo;
		//初始化键值区
		elemAnalInfo.idElem=pStationElem->sectionI;
		elemAnalInfo.m_iModuleNo=pModule->iNo;
		elemAnalInfo.legword.dwQuadLeg=pModule->ActiveQuadLegDword();
		elemAnalInfo.m_dwStatusNo=iCaseNo;
		//初始化数据区
		elemAnalInfo.iStartI=pFemElem->pStart->point_i;
		elemAnalInfo.iEndI=pFemElem->pEnd->point_i;
		elemAnalInfo.m_hParentRod=pStationElem->pLdsRod->handle;
		elemAnalInfo.pParentRod=pStationElem->pLdsRod;
		elemAnalInfo.N=pFemElem->N;
		elemAnalInfo.moment=pFemElem->start_moment;
		elemAnalInfo.weight=pFemElem->density*pFemElem->length*pFemElem->A*10*weightCoef;
		//elemAnalInfo.moment=pFemElem->end_moment;

		//elemAnalInfo.iStartI=pElem->pStart->point_i;
		//elemAnalInfo.iEndI=pElem->pEnd->point_i;
		//elemAnalInfo.m_hParentRod=pElem->pOrgPole->handle;
		//elemAnalInfo.weight=pElem->SelfWeight();
		postreport.AppendToAnalyzeBuffer(elemAnalInfo);
		*/
	}
	fclose(fp);
	return true;
}
/*{
	FILE* fp=fopen(filename,"rt");
	if(fp==NULL)
		return;
	Ta.Empty();
	console.DispNodeSet.Empty();
	console.DispPartSet.Empty();
	Ta.InitTower();
	console.InitTowerModel();
	UCS_STRU ucs;
	ucs.axis_x.Set(1,0,0);
	ucs.axis_z.Set(0,0,-1);
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	long nNode,nElem,nMaterial,nSection,nRestrict,nFoundation,nLoadCase;
	int iNodeLimit,iElemLimit,iMatLimit,iSectionLimit,iResLimit,iFoundLimit,iLoadStart,iNodeLoadLimit,iElemLoadLimit;
	iNodeLimit=iElemLimit=iMatLimit=iSectionLimit=iResLimit=iFoundLimit=iLoadStart=iNodeLoadLimit=iElemLoadLimit=2;
	CXhChar100 lineText,projectName;
	int validLineIndex=0,iMaterialIndex=1;;
	SUBSTATION_LOADCASE load_case;
	SUBSTATION_LOAD* pLoad=NULL;
	while(!ferror(fp) && !feof(fp))
	{
		lineText.Empty();
		fgets(lineText,100,fp);
		if(lineText.Length()==0)
			continue;
		lineText.Replace("　"," ");	//将全角空格换成半角空格以便统一处理
		CXhChar100 copyLnText=lineText;
		char* splitter=" =\t\r\n";
		char* key=strtok(copyLnText,splitter);
		if(key==NULL||strlen(key)==0)
			continue;	//空行
		//提取有效信息
		if(validLineIndex==0)
			projectName=lineText;
		else if(strstr(lineText,"节点坐标")!=NULL)
			continue;
		else if(strstr(lineText,"关联信息")!=NULL)
			continue;
		else if(strstr(lineText,"材料信息")!=NULL)
			continue;
		else if(strstr(lineText,"截面信息")!=NULL)
			continue;
		else if(strstr(lineText,"支座信息")!=NULL)
			continue;
		else if(strstr(lineText,"约束信息")!=NULL)
			continue;
		else if(strstr(lineText,"荷载信息")!=NULL)
			continue;
		if(strchr(lineText,','))
			lineText.Replace(',',' ');
		if(validLineIndex==1)
		{
			sscanf(lineText,"%d%d%d%d%d%d%d",&nNode,&nElem,&nMaterial,&nSection,&nRestrict,&nFoundation,&nLoadCase);
			iNodeLimit=nNode+2;
			iElemLimit=iNodeLimit+nElem;
			iMatLimit=iElemLimit+nMaterial;
			iSectionLimit=iMatLimit+nSection;
			iResLimit=iSectionLimit+nRestrict;
			iLoadStart=iFoundLimit=iResLimit+nFoundation;
			load_case.nLoadCase=nLoadCase;
			iNodeLoadLimit=iFoundLimit+1;
		}
		else if(validLineIndex>1 && validLineIndex<iNodeLimit)
		{	//文件给定坐标单位为m,需转化为mm，并调整坐标系
			SUBSTATION_NODE node;
			sscanf(lineText,"%d%lf%lf%lf",&node.pointI,&node.pos.x,&node.pos.y,&node.pos.z);
			node.pos*=1000;
			coord_trans(node.pos,ucs,FALSE,TRUE);
			SUBSTATION_NODE*pSubstationNode=hashSubstationNodes.SetValue(node.pointI,node);
			pSubstationNode->wRestrict=0;
			CLDSNode* pNode=Ta.Node.append();
			pNode->SetPosition(node.pos.x,node.pos.y,node.pos.z);
			pNode->point_i=node.pointI;
			pSubstationNode->pLdsNode=pNode;
			pSubstationNode->hNode=pNode->handle;
			NewNode(pNode);
		}
		else if(validLineIndex>=iNodeLimit && validLineIndex<iElemLimit)
		{
			SUBSTATION_ELEM elem;
			sscanf(lineText,"%d%d%d%d",&elem.elemI,&elem.startPointI,&elem.endPointI,&elem.sectionI);
			SUBSTATION_ELEM* pStationElem=hashSubstationElems.SetValue(elem.elemI,elem);
			CLDSLinePart* pRod=(CLDSLinePart*)Ta.Parts.append(CLS_LINEPART);
			pRod->size_idClassType=CLS_LINETUBE;
			pStationElem->hPart=pRod->handle;
			pStationElem->pLdsRod=pRod;
			SUBSTATION_NODE* pSubstationStartNode=hashSubstationNodes.GetValue(elem.startPointI);
			SUBSTATION_NODE* pSubstationEndNode  =hashSubstationNodes.GetValue(elem.endPointI);
			pRod->pStart=Ta.Node.FromHandle(pSubstationStartNode->hNode);
			pRod->pEnd  =Ta.Node.FromHandle(pSubstationEndNode->hNode);
			pRod->SetStart(pSubstationStartNode->pos);
			pRod->SetEnd(pSubstationEndNode->pos);
			pStationElem->cbJointStart=pStationElem->cbJointEnd=0;
			NewLinePart(pRod);
		}
		else if(validLineIndex>=iElemLimit && validLineIndex<iMatLimit)
		{	//材质信息
			CXhChar16 sValue1,sValue2,sValue3,sValue4;
			sscanf(lineText,"%s%s%s%s",(char*)sValue1,(char*)sValue2,(char*)sValue3,(char*)sValue4);
			SUBSTATION_MATERIAL material;
			material.E=atof(sValue1);
			material.shear=atof(sValue2);
			material.poson=atof(sValue3);
			material.density=atof(sValue4);
			hashSubstationMaterial.SetValue(iMaterialIndex,material);
			iMaterialIndex++;
		}
		else if(validLineIndex>=iMatLimit && validLineIndex<iSectionLimit)
		{	//截面信息
			SUBSTATION_SECTION section;
			CXhChar16 sValue1,sValue2,sValue3,sValue4;
			sscanf(lineText,"%d%s%s%s%s",&section.materialI,(char*)sValue1,(char*)sValue2,(char*)sValue3,(char*)sValue4);
			section.area=atof(sValue1);
			section.IX=atof(sValue2);
			section.IY=atof(sValue3);
			section.IZ=atof(sValue4);
			section.sectionI=validLineIndex-iMatLimit+1;
			hashSubstationSections.SetValue(section.sectionI,section);
		}
		else if(validLineIndex>=iSectionLimit && validLineIndex<iResLimit)
		{	//单元约束
			int elemI,xRestrict,yRestrict,zRestrict,xRotRestrict,yRotRestrict,zRotRestrict;
			CXhChar16 sKey;
			sscanf(lineText,"%d%s%d%d%d%d%d%d",&elemI,(char*)sKey,&xRestrict,&yRestrict,&zRestrict,&xRotRestrict,&yRotRestrict,&zRotRestrict);
			SUBSTATION_ELEM* pElem=hashSubstationElems.GetValue(elemI);
			if(strchr(sKey,'S')!=NULL)
			{
				pElem->cbJointStart=0;
				if(xRestrict!=0)
					pElem->cbJointStart|=0x01;
				if(yRestrict!=0)
					pElem->cbJointStart|=0x02;
				if(zRestrict!=0)
					pElem->cbJointStart|=0x04;
				if(xRotRestrict!=0)
					pElem->cbJointStart|=0x08;
				if(yRotRestrict!=0)
					pElem->cbJointStart|=0x10;
				if(zRotRestrict!=0)
					pElem->cbJointStart|=0x20;
			}
			else
			{
				pElem->cbJointEnd=0;
				if(xRestrict!=0)
					pElem->cbJointEnd|=0x01;
				if(yRestrict!=0)
					pElem->cbJointEnd|=0x02;
				if(zRestrict!=0)
					pElem->cbJointEnd|=0x04;
				if(xRotRestrict!=0)
					pElem->cbJointEnd|=0x08;
				if(yRotRestrict!=0)
					pElem->cbJointEnd|=0x10;
				if(zRotRestrict!=0)
					pElem->cbJointEnd|=0x20;
			}
		}
		else if(validLineIndex>=iResLimit && validLineIndex<iFoundLimit)
		{	//支座信息
			int pointI,xRestrict,yRestrict,zRestrict,xRotRestrict,yRotRestrict,zRotRestrict;
			sscanf(lineText,"%d%d%d%d%d%d%d",&pointI,&xRestrict,&yRestrict,&zRestrict,&xRotRestrict,&yRotRestrict,&zRotRestrict);
			WORD wFlag=0;
			if(xRestrict==1)
				wFlag|=X_RESTRICT;
			if(yRestrict==1)
				wFlag|=Y_RESTRICT;
			if(zRestrict==1)
				wFlag|=Z_RESTRICT;
			if(xRotRestrict==1)
				wFlag|=X_ROT_RESTRICT;
			if(yRotRestrict==1)
				wFlag|=Y_ROT_RESTRICT;
			if(zRotRestrict==1)
				wFlag|=Z_ROT_RESTRICT;
			//设置节点约束
			SUBSTATION_NODE* pSubstationNode=hashSubstationNodes.GetValue(pointI);
			pSubstationNode->wRestrict=wFlag;
		}
		else if(validLineIndex>=iFoundLimit)
		{	//荷载信息
			if(validLineIndex==iLoadStart)
			{
				pLoad=listCaseLoads.append();
				sscanf(lineText,"%lf%lf",&pLoad->fGamaG1,&pLoad->fGamaG2);
			}
			else if(validLineIndex==iLoadStart+1)//strchr(lineText,'.')==NULL)
			{	
				sscanf(lineText,"%d%d",&(pLoad->nNodeLoad),&(pLoad->nElemLoad));
				iNodeLoadLimit=validLineIndex+pLoad->nNodeLoad+1;
				iLoadStart=iElemLoadLimit=iNodeLoadLimit+pLoad->nElemLoad;
			}
			else if(validLineIndex<iNodeLoadLimit)
			{
				SUBSTATION_NODE_LOAD nodeLoad;
				CXhChar16 nodeInfo,sLoad;
				sscanf(lineText,"%s%lf",(char*)nodeInfo,&nodeLoad.fLoad);
				nodeInfo.Replace('.',' ');
				sscanf(nodeInfo,"%d%d",&nodeLoad.pointI,&nodeLoad.directionI);
				if(nodeLoad.directionI==2||nodeLoad.directionI==3)
					nodeLoad.fLoad=-nodeLoad.fLoad;
				pLoad->nodeLoadList.append(nodeLoad);
			}
			else if(validLineIndex>=iNodeLoadLimit && validLineIndex<iElemLoadLimit)
			{
				SUBSTATION_ELEM_LOAD elemLoad;
				sscanf(lineText,"%d%d%lf%lf%d",&elemLoad.elemI,&elemLoad.loadType,&elemLoad.fDist,&elemLoad.fLoad,&elemLoad.directionI);
				if(elemLoad.directionI==2||elemLoad.directionI==3)
					elemLoad.fLoad=-elemLoad.fLoad;
				pLoad->elemLoadList.append(elemLoad);
			}
		}
		validLineIndex++;
	}
	fclose(fp);
	SUBSTATION_NODE* pStationNode;
	SUBSTATION_ELEM* pStationElem;
	ATOM_LIST<FEM_NODE> listFemNodes;
	ATOM_LIST<FEM_ELEMENT> listFemElems;
	CHashSet<FEM_NODE*> hashFemNodeSet;
	CHashSet<FEM_ELEMENT*> hashFemElemSet;
	FEM_NODE* pFemNode;
	FEM_ELEMENT* pFemElem;
	//提取有限元计算模型中的节点集合
	for(pStationNode=hashSubstationNodes.GetFirst();pStationNode;pStationNode=hashSubstationNodes.GetNext())
	{
		pFemNode=listFemNodes.append();
		pFemNode->point_i=pStationNode->pointI;
		pFemNode->dimension=3;
		pStationNode->index=listFemNodes.GetNodeNum()-1;
		pFemNode->pos=pStationNode->pos;
		pFemNode->wRestrict=pStationNode->wRestrict;
		hashFemNodeSet.SetValue(pFemNode->point_i,pFemNode);
		pStationNode->pLdsNode->SetRestrict(pStationNode->wRestrict);
	}
	int i=0,j=0;
	//提取有限元计算模型中的单元集合
	for(pStationElem=hashSubstationElems.GetFirst();pStationElem;pStationElem=hashSubstationElems.GetNext(),i++)
	{
		pFemElem=listFemElems.append();
		pFemElem->idElem=pStationElem->elemI;
		pFemElem->elem_type=0;
		pFemElem->T0=pFemElem->N=0;
		pFemElem->pStart=hashFemNodeSet.GetValue(pStationElem->startPointI);
		pFemElem->pEnd=hashFemNodeSet.GetValue(pStationElem->endPointI);
		pFemElem->length=DISTANCE(pFemElem->pStart->pos,pFemElem->pEnd->pos);
		pFemElem->lenStdVec=(pFemElem->pEnd->pos-pFemElem->pStart->pos)/pFemElem->length;
		hashFemElemSet.SetValue(pStationElem->elemI,pFemElem);
		SUBSTATION_NODE* pStartStationNode=hashSubstationNodes.GetValue(pStationElem->startPointI);
		SUBSTATION_NODE* pEndStationNode=hashSubstationNodes.GetValue(pStationElem->endPointI);
		pFemElem->start_i=pStartStationNode->index;
		pFemElem->end_i=pEndStationNode->index;
		pFemElem->start_joint_type=pFemElem->end_joint_type=JOINT_RIGID;
		if(pStationElem->cbJointStart==0x07)
			pFemElem->start_joint_type=JOINT_HINGE;
		else if(pStationElem->cbJointStart==0x3f)
			pFemElem->start_joint_type=JOINT_RIGID;
		else if(pStationElem->cbJointStart==0)
			pFemElem->cbStartJoint=0x3f;
		else //if(pStationElem->cbJointStart==0)
			pFemElem->cbStartJoint=pStationElem->cbJointStart;
		//else
		//	pFemElem->cbStartJoint=pStationElem->cbJointStart;
		if(pStationElem->cbJointEnd==0x07)
			pFemElem->end_joint_type=JOINT_HINGE;
		else if(pStationElem->cbJointEnd==0x3f)
			pFemElem->end_joint_type=JOINT_RIGID;
		else if(pStationElem->cbJointEnd==0)
			pFemElem->cbEndJoint=0x3f;
		else //if(pStationElem->cbJointEnd==0)
			pFemElem->cbEndJoint=pStationElem->cbJointEnd;
		//else
		//	pFemElem->cbEndJoint=pStationElem->cbJointEnd;
		if(pFemElem->start_joint_type==JOINT_RIGID||pFemElem->end_joint_type==JOINT_RIGID)
			pStationElem->pLdsRod->m_iElemType=2;
		pStationElem->pLdsRod->start_joint_type=pFemElem->start_joint_type;
		pStationElem->pLdsRod->end_joint_type=pFemElem->end_joint_type;
		SUBSTATION_SECTION* pSect=hashSubstationSections.GetValue(pStationElem->sectionI);
		if(pSect==NULL)
		{
			pFemElem->E=2.06e5;//pStationElem->E,MPa
			pFemElem->G=pFemElem->E/2.6;//pStationElem->E
			pFemElem->density=0;
		}
		else
		{
			pFemElem->A=pSect->area*1e6;//m2->mm2
			pFemElem->Iz=pSect->IY*1e12;	//不支持IY!=IZ情况,1e12#m4->mm4
			pFemElem->Ip=pSect->IX*1e12;
			SUBSTATION_MATERIAL* pMaterial=hashSubstationMaterial.GetValue(pSect->materialI);
			if(pMaterial)
			{
				pFemElem->E=pMaterial->E*0.001;	//TODO:单位,*0.001#kPa->MPa
				pFemElem->G=pMaterial->shear*0.001;
				pFemElem->density=pMaterial->density*1e-7;	//单位:kN/m3->kg/mm3
			}
		}
		//模拟钢管规格尺寸
		pStationElem->pLdsRod->size_thick=SQRT(pFemElem->A/125.6);	//125.6=40*Pi
		pStationElem->pLdsRod->size_wide=pStationElem->pLdsRod->size_thick*40;
	}
	//提取有限元计算过程中的工况集合
	SUBSTATION_LOAD* pCase;
	CWorkStatus* pLdsCase;
	Ta.WorkStatus.Empty();
	CObjNoGroup* pCaseGroup=Ta.NoManager.FromGroupID(WORKSTATUS_GROUP);
	pCaseGroup->EmptyNo();
	for(pCase=listCaseLoads.GetFirst(),i=1;pCase;pCase=load_case.substationLoadList.GetNext(),i++)
	{
		pCaseGroup->SetNoState(i);
		pLdsCase=Ta.WorkStatus.append();
		pLdsCase->m_bLoadIncFactor=true;
		pLdsCase->iNo=i;
		sprintf(pLdsCase->description,"%d",i);
		if(i==1)
			Ta.m_hActiveStatus=pLdsCase->handle;
	}
	postreport.InitProcTempFile(CXhChar200("%s",filename));
	postreport.Empty();
	CLDSModule* pModule=Ta.GetActiveModule();
	CFemResolver* pResolver=CFemResolver::CreateResolver();
	pResolver->SetProcessMonitor(FemResolverMonitor);
	pResolver->SetBuffSize(400*0x100000);	//400M缓存空间
	CTimerCount timer;
	timer.Start();
	try{
		bool bFirstResolve=true;
		int iCaseNo;
		for(pCase=load_case.substationLoadList.GetFirst(),pLdsCase=Ta.WorkStatus.GetFirst(),iCaseNo=1;pCase;
			pCase=load_case.substationLoadList.GetNext(),pLdsCase=Ta.WorkStatus.GetNext(),iCaseNo++)
		{
			FEM_NODE* pPrevFemNode=NULL;
			for(pFemNode=listFemNodes.GetFirst();pFemNode;pFemNode=listFemNodes.GetNext())
			{
				pFemNode->Fxyz.Set();
				pFemNode->Moment.Set();
			}
			//清空单元内２，３，４断面上的的内力及内力矩
			for(pStationElem=hashSubstationElems.GetFirst();pStationElem;pStationElem=hashSubstationElems.GetNext())
			{
				pStationElem->QF.Set();
				pStationElem->QWF=0;
				pStationElem->F_sect_s.Set();
				pStationElem->M_sect_s.Set();
				pStationElem->F_sect_e.Set();
				pStationElem->M_sect_e.Set();
				for(j=0;j<3;j++)
				{
					pStationElem->sects[j].F_T_S.Set();
					pStationElem->sects[j].M_T_S.Set();
				}
			}
			//1.施加节点集中荷载
			for(SUBSTATION_NODE_LOAD* pNodeLoad=pCase->nodeLoadList.GetFirst();pNodeLoad;pNodeLoad=pCase->nodeLoadList.GetNext())
			{
				if(pPrevFemNode&&pNodeLoad->pointI==pPrevFemNode->point_i)
					pFemNode=pPrevFemNode;
				else
					pPrevFemNode=pFemNode=hashFemNodeSet.GetValue(pNodeLoad->pointI);
				switch(pNodeLoad->directionI)
				{
				case 1:
					pFemNode->Fxyz.x+=pNodeLoad->fLoad*1000;	//kN->N
					break;
				case 2:
					pFemNode->Fxyz.y+=pNodeLoad->fLoad*1000;
					break;
				case 3:
					pFemNode->Fxyz.z+=pNodeLoad->fLoad*1000;
					break;
				case 4:
					pFemNode->Moment.x+=pNodeLoad->fLoad*1e6;	//kN*m->N*mm
					break;
				case 5:
					pFemNode->Moment.y+=pNodeLoad->fLoad*1e6;
					break;
				case 6:
					pFemNode->Moment.z+=pNodeLoad->fLoad*1e6;
					break;
				default:
					break;
				}
			}
			FEM_ELEMENT* pPrevFemElem=NULL;
			SUBSTATION_ELEM* pPrevStationElem=NULL;
			//2.施加单元荷载
			for(SUBSTATION_ELEM_LOAD* pElemLoad=pCase->elemLoadList.GetFirst();pElemLoad;pElemLoad=pCase->elemLoadList.GetNext())
			{
				if(pPrevFemElem&&pElemLoad->elemI==pPrevStationElem->elemI)
				{
					pFemElem=pPrevFemElem;
					pStationElem=pPrevStationElem;
				}
				else
				{
					pPrevFemElem=pFemElem=hashFemElemSet.GetValue(pElemLoad->elemI);
					pPrevStationElem=pStationElem=hashSubstationElems.GetValue(pElemLoad->elemI);
				}
				GEPOINT armLength,armStartM;
				double scale;
				//对于释放部分转动约束情况，应依据《简明建筑结构设计手册》以Y和Z轴分别考虑杆件两端的转动约束类型 wjh-2015.12.09
				//BYTE ciJointStartAxisY=pFemElem->start_joint_type,ciJointStartAxisZ=pFemElem->start_joint_type;
				//BYTE ciJointEndAxisY=pFemElem->end_joint_type,ciJointEndAxisZ=pFemElem->end_joint_type;
				//if((pFemElem->cbStartJoint&0x10)==0)
				//	ciJointStartAxisY=JOINT_HINGE;
				//if((pFemElem->cbStartJoint&0x20)==0)
				//	ciJointStartAxisZ=JOINT_HINGE;
				//if((pFemElem->cbEndJoint&0x10)==0)
				//	ciJointEndAxisY=JOINT_HINGE;
				//if((pFemElem->cbEndJoint&0x20)==0)
				//	ciJointEndAxisZ=JOINT_HINGE;
				int jointStartType=pFemElem->start_joint_type;
				int jointEndType=pFemElem->end_joint_type;
				if((pFemElem->cbStartJoint&0x10)+(pFemElem->cbStartJoint&0x20)==0)
					jointStartType=JOINT_HINGE;
				if((pFemElem->cbEndJoint&0x10)+(pFemElem->cbEndJoint&0x20)==0)
					jointEndType=JOINT_HINGE;
				if(pElemLoad->loadType==1)
				{	//均布力(暂不考虑均面力等效到梁端时形成的等效力偶）
					double M,ql=pElemLoad->fLoad*pFemElem->length;	//kN/m *mm = N
					if(pElemLoad->directionI==1)
					{
						pStationElem->QF.x+=pElemLoad->fLoad;
						if((jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)||
							(jointStartType==JOINT_HINGE&&jointEndType==JOINT_HINGE))
						{
							pFemElem->pStart->Fxyz.x+=ql/2;
							pFemElem->pEnd->Fxyz.x  +=ql/2;
							pStationElem->F_sect_s.x+=ql/2;
							pStationElem->F_sect_e.x+=ql/2;
						}
						//累加计算均布力在单元中间断面上的内力(矩)影响
						//等效两端Ｙ及Ｚ轴方向的力偶
						if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
						{	//两端固接
							M= pFemElem->lenStdVec.z*ql*pFemElem->length/12;	//N*mm,ql^2/12
							pFemElem->pStart->Moment.y+=M;
							pFemElem->pEnd  ->Moment.y-=M;
							pStationElem->M_sect_s.y+=M;
							pStationElem->M_sect_e.y-=M;
							M=-pFemElem->lenStdVec.y*ql*pFemElem->length/12;	//N*mm,ql^2/12
							pFemElem->pStart->Moment.z+=M;
							pFemElem->pEnd  ->Moment.z-=M;
							pStationElem->M_sect_s.z+=M;
							pStationElem->M_sect_e.z-=M;
						}
						else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
						{	//始端固接,终端铰接
							pStationElem->F_sect_s.x+=ql*0.625;
							pStationElem->F_sect_e.x+=ql*0.375;
							pFemElem->pStart->Fxyz.x+=ql*0.625;	//ql*5/8
							pFemElem->pEnd->Fxyz.x  +=ql*0.375;	//ql*3/8
							M= pFemElem->lenStdVec.z*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pStart->Moment.y+=M;
							pStationElem->M_sect_s.y+=M;
							M=-pFemElem->lenStdVec.y*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pStart->Moment.z+=M;
							pStationElem->M_sect_s.z+=M;
						}
						else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
						{	//始端固接,终端铰接
							pStationElem->F_sect_s.x-=ql*0.375;
							pStationElem->F_sect_e.x-=ql*0.625;
							pFemElem->pStart->Fxyz.x+=ql*0.375;	//ql*3/8
							pFemElem->pEnd->Fxyz.x  +=ql*0.625;	//ql*5/8
							M= pFemElem->lenStdVec.z*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pEnd->Moment.y-=M;
							pStationElem->M_sect_e.y-=M;
							M=-pFemElem->lenStdVec.y*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pEnd->Moment.z-=M;
							pStationElem->M_sect_e.z-=M;
						}
					}
					else if(pElemLoad->directionI==2)
					{
						pStationElem->QF.y+=pElemLoad->fLoad;
						if((jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)||
							(jointStartType==JOINT_HINGE&&jointEndType==JOINT_HINGE))
						{
							pFemElem->pStart->Fxyz.y+=ql/2;
							pFemElem->pEnd->Fxyz.y  +=ql/2;
							pStationElem->F_sect_s.y+=ql/2;
							pStationElem->F_sect_e.y+=ql/2;
						}
						//累加计算均布力在单元中间断面上的内力(矩)影响
						//等效两端X及Z轴方向的力偶
						if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
						{	//两端固接
							M=-pFemElem->lenStdVec.z*ql*pFemElem->length/12;	//N*mm,ql^2/12
							pFemElem->pStart->Moment.x+=M;
							pFemElem->pEnd  ->Moment.x-=M;
							pStationElem->M_sect_s.x+=M;
							pStationElem->M_sect_e.x-=M;
							M= pFemElem->lenStdVec.x*ql*pFemElem->length/12;	//N*mm,ql^2/12
							pFemElem->pStart->Moment.z+=M;
							pFemElem->pEnd  ->Moment.z-=M;
							pStationElem->M_sect_s.z+=M;
							pStationElem->M_sect_e.z-=M;
						}
						else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
						{	//始端固接,终端铰接
							pStationElem->F_sect_s.y+=ql*0.625;
							pStationElem->F_sect_e.y+=ql*0.375;
							pFemElem->pStart->Fxyz.y+=ql*0.625;	//ql*5/8
							pFemElem->pEnd  ->Fxyz.y+=ql*0.375;	//ql*3/8
							M=-pFemElem->lenStdVec.z*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pStart->Moment.x+=M;
							pStationElem->M_sect_s.x+=M;
							M= pFemElem->lenStdVec.x*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pStart->Moment.z+=M;
							pStationElem->M_sect_s.z+=M;
						}
						else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
						{	//始端铰接,终端固接
							pStationElem->F_sect_s.y+=ql*0.375;
							pStationElem->F_sect_e.y+=ql*0.625;
							pFemElem->pStart->Fxyz.y+=ql*0.375;	//ql*3/8
							pFemElem->pEnd  ->Fxyz.y+=ql*0.625;	//ql*5/8
							M=-pFemElem->lenStdVec.z*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pEnd->Moment.x-=M;
							pStationElem->M_sect_e.x-=M;
							M= pFemElem->lenStdVec.x*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pEnd->Moment.z-=M;
							pStationElem->M_sect_e.z-=M;
						}
					}
					else if(pElemLoad->directionI==3)
					{
						pStationElem->QF.z+=pElemLoad->fLoad;
						if((jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)||
							(jointStartType==JOINT_HINGE&&jointEndType==JOINT_HINGE))
						{
							pFemElem->pStart->Fxyz.z+=ql/2;
							pFemElem->pEnd  ->Fxyz.z+=ql/2;
							pStationElem->F_sect_s.z+=ql/2;
							pStationElem->F_sect_e.z+=ql/2;
						}
						//累加计算均布力在单元中间断面上的内力(矩)影响
						//等效两端X及Y轴方向的力偶
						if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
						{	//两端固接
							M= pFemElem->lenStdVec.y*ql*pFemElem->length/12;	//N*mm,ql^2/12
							pFemElem->pStart->Moment.x+=M;
							pFemElem->pEnd  ->Moment.x-=M;
							pStationElem->M_sect_s.x+=M;
							pStationElem->M_sect_e.x-=M;
							M=-pFemElem->lenStdVec.x*ql*pFemElem->length/12;	//N*mm,ql^2/12
							pFemElem->pStart->Moment.y+=M;
							pFemElem->pEnd  ->Moment.y-=M;
							pStationElem->M_sect_s.y+=M;
							pStationElem->M_sect_e.y-=M;
						}
						else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
						{	//始端固接,终端铰接
							pStationElem->F_sect_s.z+=ql*0.625;
							pStationElem->F_sect_e.z+=ql*0.375;
							pFemElem->pStart->Fxyz.z+=ql*0.625;	//ql*5/8
							pFemElem->pEnd  ->Fxyz.z+=ql*0.375;	//ql*3/8
							M= pFemElem->lenStdVec.y*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pStart->Moment.x+=M;
							pStationElem->M_sect_s.x+=M;
							M=-pFemElem->lenStdVec.x*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pStart->Moment.z+=M;
							pStationElem->M_sect_s.z+=M;
						}
						else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
						{	//始端铰接,终端固接
							pStationElem->F_sect_s.z+=ql*0.375;
							pStationElem->F_sect_e.z+=ql*0.625;
							pFemElem->pStart->Fxyz.z+=ql*0.375;	//ql*3/8
							pFemElem->pEnd  ->Fxyz.z+=ql*0.625;	//ql*5/8
							M= pFemElem->lenStdVec.y*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pEnd->Moment.x-=M;
							pStationElem->M_sect_e.x-=M;
							M=-pFemElem->lenStdVec.x*ql*pFemElem->length/8;	//N*mm,ql^2/8
							pFemElem->pEnd->Moment.z-=M;
							pStationElem->M_sect_e.z-=M;
						}
					}
				}
				else if(pElemLoad->loadType==2)
				{	//集中力
					double armLength;
					double scaleFactor=1000*pElemLoad->fDist/pFemElem->length;
					double P=pElemLoad->fLoad*1000;	//1000:kN->N
					//double a=scaleFactor,b=1-scaleFactor;	//定义a,b纯粹是为了代码与建筑结构手册公式方便对照
					GEPOINT fs,fe,ms,me;	//始末端等效力及等效力矩
					if(pElemLoad->directionI==1)
					{	//X向集中力
						if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_HINGE)
						{
							pFemElem->pStart->Fxyz.x+=P*(1-scaleFactor);//pElemLoad->fLoad*1000*(1-scaleFactor)
							pFemElem->pEnd  ->Fxyz.x+=P*scaleFactor;
							pStationElem->F_sect_s.x+=P*(1-scaleFactor);
							pStationElem->F_sect_e.x+=P*scaleFactor;
						}
						else
						{
							GEPOINT Faxis=P*pFemElem->lenStdVec.x*pFemElem->lenStdVec;
							pFemElem->pStart->Fxyz+=Faxis*(1-scaleFactor);
							pFemElem->pEnd  ->Fxyz+=Faxis*scaleFactor;
							pStationElem->F_sect_s+=Faxis*(1-scaleFactor);
							pStationElem->F_sect_e+=Faxis*scaleFactor;
							GEPOINT Fsect=GEPOINT(P-Faxis.x,-Faxis.y,-Faxis.z);
							if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
							{
								fs=Fsect*(1-scaleFactor)*(1-scaleFactor)*(3*scaleFactor+(1-scaleFactor));	//
								fe=Fsect*scaleFactor*scaleFactor*(scaleFactor+3*(1-scaleFactor));			//终端等效力
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｙ及Ｚ轴方向的力偶
								GEPOINT Mvec=pFemElem->lenStdVec^Fsect;
								GEPOINT MS=Mvec*scaleFactor*(1-scaleFactor)*(1-scaleFactor)*pFemElem->length;
								GEPOINT ME=Mvec*scaleFactor*scaleFactor*(1-scaleFactor)*pFemElem->length;
								pFemElem->pStart->Moment+=MS;
								pFemElem->pEnd  ->Moment-=ME;
								pStationElem->M_sect_s+=MS;
								pStationElem->M_sect_e-=ME;
							}
							else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
							{
								fs=Fsect*0.5*(1-scaleFactor)*(3-(1-scaleFactor)*(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*0.5*scaleFactor*scaleFactor*(3-scaleFactor);
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｙ及Ｚ轴方向的力偶
								armStartM.y= P*pFemElem->lenStdVec.z*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(2-scaleFactor);	//N*m
								pFemElem->pStart->Moment.y+=armStartM.y;
								armStartM.z=-P*pFemElem->lenStdVec.y*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(2-scaleFactor);	//N*m
								pFemElem->pStart->Moment.z+=armStartM.z;
								pStationElem->M_sect_s.y+=armStartM.y;
								pStationElem->M_sect_s.z+=armStartM.z;
							}
							else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
							{
								fs=Fsect*0.5*(1-scaleFactor)*(1-scaleFactor)*(3-(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*0.5*scaleFactor*(3-scaleFactor*scaleFactor);
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｙ及Ｚ轴方向的力偶
								armStartM.y= P*pFemElem->lenStdVec.z*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(1+scaleFactor);	//N*m
								pFemElem->pEnd  ->Moment.y-=armStartM.y;
								armStartM.z=-P*pFemElem->lenStdVec.y*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(1+scaleFactor);	//N*m
								pFemElem->pEnd  ->Moment.z-=armStartM.z;
								pStationElem->M_sect_e.y-=armStartM.y;
								pStationElem->M_sect_e.z-=armStartM.z;
							}
							pStationElem->F_sect_s+=fs;
							pStationElem->F_sect_e+=fe;
						}
						//累加计算均布力在单元中间断面上的内力(矩)影响
						double prev_s=0;
						for(j=0,scale=0;j<3;j++)
						{
							scale+=0.25;
							double d=pElemLoad->fDist*1000,s=scale*pFemElem->length;
							if(d<s-EPS&&d>prev_s+EPS)
							{	//集中力在截面左侧
								pStationElem->sects[j].F_L_S.x+=P;	//N
								//等效两端Ｙ及Ｚ轴方向的力偶
								armLength=(d-s)*pFemElem->lenStdVec.z;
								pStationElem->sects[j].M_L_S.y-=P*armLength;	//N*mm
								armLength=(d-s)*pFemElem->lenStdVec.y;
								pStationElem->sects[j].M_L_S.z+=P*armLength;	//N*mm
							}
							else if(d>=s-EPS&&d<=s+EPS)	//集中力在截面上
								pStationElem->sects[j].F_T_S.x+=P;	//N
							prev_s=s;
						}
					}
					else if(pElemLoad->directionI==2)
					{	//Y向集中力
						if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_HINGE)
						{
							pFemElem->pStart->Fxyz.y+=P*(1-scaleFactor);//pElemLoad->fLoad*1000*(1-scaleFactor)
							pFemElem->pEnd  ->Fxyz.y+=P*scaleFactor;
							pStationElem->F_sect_s.y+=P*(1-scaleFactor);
							pStationElem->F_sect_e.y+=P*scaleFactor;
						}
						else
						{
							GEPOINT Faxis=P*pFemElem->lenStdVec.y*pFemElem->lenStdVec;
							pFemElem->pStart->Fxyz+=Faxis*(1-scaleFactor);
							pFemElem->pEnd  ->Fxyz+=Faxis*scaleFactor;
							pStationElem->F_sect_s+=Faxis*(1-scaleFactor);
							pStationElem->F_sect_e+=Faxis*scaleFactor;
							GEPOINT Fsect=GEPOINT(-Faxis.x,P-Faxis.y,-Faxis.z);
							if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
							{
								fs=Fsect*(1-scaleFactor)*(1-scaleFactor)*(3*scaleFactor+(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*scaleFactor*scaleFactor*(scaleFactor+3*(1-scaleFactor));
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｘ及Ｚ轴方向的力偶
								GEPOINT Mvec=pFemElem->lenStdVec^Fsect;
								GEPOINT MS=Mvec*scaleFactor*(1-scaleFactor)*(1-scaleFactor)*pFemElem->length;
								GEPOINT ME=Mvec*scaleFactor*scaleFactor*(1-scaleFactor)*pFemElem->length;
								pFemElem->pStart->Moment+=MS;
								pFemElem->pEnd  ->Moment-=ME;
								pStationElem->M_sect_s+=MS;
								pStationElem->M_sect_e-=ME;
							}
							else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
							{
								fs=Fsect*0.5*(1-scaleFactor)*(3-(1-scaleFactor)*(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*0.5*scaleFactor*scaleFactor*(3-scaleFactor);
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｘ及Ｚ轴方向的力偶
								armStartM.x=-P*pFemElem->lenStdVec.z*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(2-scaleFactor);	//N*m
								pFemElem->pStart->Moment.x+=armStartM.x;
								armStartM.z= P*pFemElem->lenStdVec.x*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(2-scaleFactor);	//N*m
								pFemElem->pStart->Moment.z+=armStartM.z;
								pStationElem->M_sect_s.x+=armStartM.x;
								pStationElem->M_sect_s.z+=armStartM.z;
							}
							else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
							{
								fs=Fsect*0.5*(1-scaleFactor)*(1-scaleFactor)*(3-(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*0.5*scaleFactor*(3-scaleFactor*scaleFactor);
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｘ及Ｚ轴方向的力偶
								armStartM.x=-P*pFemElem->lenStdVec.z*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(1+scaleFactor);	//N*m
								pFemElem->pEnd  ->Moment.x-=armStartM.x;
								armStartM.z= P*pFemElem->lenStdVec.x*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(1+scaleFactor);	//N*m
								pFemElem->pEnd  ->Moment.z-=armStartM.z;
								pStationElem->M_sect_e.x-=armStartM.x;
								pStationElem->M_sect_e.z-=armStartM.z;
							}
							pStationElem->F_sect_s+=fs;
							pStationElem->F_sect_e+=fe;
						}
						//累加计算均布力在单元中间断面上的内力(矩)影响
						double prev_s=0;
						for(j=0,scale=0;j<3;j++)
						{
							scale+=0.25;
							double d=pElemLoad->fDist*1000,s=scale*pFemElem->length;
							if(d<s-EPS&&d>prev_s+EPS)
							{	//集中力在截面左侧
								pStationElem->sects[j].F_L_S.y+=P;
								//等效两端X及Ｚ轴方向的力偶
								armLength=(d-s)*pFemElem->lenStdVec.z;
								pStationElem->sects[j].M_L_S.x+=P*armLength;	//N*mm
								armLength=(d-s)*pFemElem->lenStdVec.x;
								pStationElem->sects[j].M_L_S.z-=P*armLength;	//N*mm
							}
							else if(d>=s-EPS&&d<=s+EPS)	//集中力在截面上
								pStationElem->sects[j].F_T_S.y+=P;	//N
							prev_s=s;
						}
					}
					else if(pElemLoad->directionI==3)
					{	//Z向集中力
						if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_HINGE)
						{
							pFemElem->pStart->Fxyz.z+=P*(1-scaleFactor);//pElemLoad->fLoad*1000*(1-scaleFactor)
							pFemElem->pEnd  ->Fxyz.z+=P*scaleFactor;
							pStationElem->F_sect_s.z+=P*(1-scaleFactor);
							pStationElem->F_sect_e.z+=P*scaleFactor;
						}
						else
						{	
							GEPOINT Faxis=P*pFemElem->lenStdVec.z*pFemElem->lenStdVec;
							pFemElem->pStart->Fxyz+=Faxis*(1-scaleFactor);
							pFemElem->pEnd  ->Fxyz+=Faxis*scaleFactor;
							pStationElem->F_sect_s+=Faxis*(1-scaleFactor);
							pStationElem->F_sect_e+=Faxis*scaleFactor;
							GEPOINT Fsect=GEPOINT(-Faxis.x,-Faxis.y,P-Faxis.z);
							if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
							{
								fs=Fsect*(1-scaleFactor)*(1-scaleFactor)*(3*scaleFactor+(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*scaleFactor*scaleFactor*(scaleFactor+3*(1-scaleFactor));
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｙ及Ｘ轴方向的力偶
								GEPOINT Mvec=pFemElem->lenStdVec^Fsect;
								GEPOINT MS=Mvec*scaleFactor*(1-scaleFactor)*(1-scaleFactor)*pFemElem->length;
								GEPOINT ME=Mvec*scaleFactor*scaleFactor*(1-scaleFactor)*pFemElem->length;
								pFemElem->pStart->Moment+=MS;
								pFemElem->pEnd  ->Moment-=ME;
								pStationElem->M_sect_s+=MS;
								pStationElem->M_sect_e-=ME;
							}
							else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
							{
								fs=Fsect*0.5*(1-scaleFactor)*(3-(1-scaleFactor)*(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*0.5*scaleFactor*scaleFactor*(3-scaleFactor);
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｙ及Ｘ轴方向的力偶
								armStartM.y=-P*pFemElem->lenStdVec.x*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(2-scaleFactor);	//N*m
								pFemElem->pStart->Moment.y+=armStartM.y;
								armStartM.x= P*pFemElem->lenStdVec.y*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(2-scaleFactor);	//N*m
								pFemElem->pStart->Moment.x+=armStartM.x;
								pStationElem->M_sect_s.y+=armStartM.y;
								pStationElem->M_sect_s.x+=armStartM.x;
							}
							else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
							{
								fs=Fsect*0.5*(1-scaleFactor)*(1-scaleFactor)*(3-(1-scaleFactor));//pElemLoad->fLoad*1000*(1-scaleFactor)
								fe=Fsect*0.5*scaleFactor*(3-scaleFactor*scaleFactor);
								pFemElem->pStart->Fxyz+=fs;
								pFemElem->pEnd  ->Fxyz+=fe;
								//等效两端Ｙ及Ｘ轴方向的力偶
								armStartM.y=-P*pFemElem->lenStdVec.x*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(1+scaleFactor);	//N*m
								pFemElem->pEnd  ->Moment.y-=armStartM.y;
								armStartM.x= P*pFemElem->lenStdVec.y*pFemElem->length*0.5*(1-scaleFactor)*scaleFactor*(1+scaleFactor);	//N*m
								pFemElem->pEnd  ->Moment.x-=armStartM.x;
								pStationElem->M_sect_e.y-=armStartM.y;
								pStationElem->M_sect_e.x-=armStartM.x;
							}
							pStationElem->F_sect_s+=fs;
							pStationElem->F_sect_e+=fe;
						}
						//累加计算均布力在单元中间断面上的内力(矩)影响
						double prev_s=0;
						for(j=0,scale=0;j<3;j++)
						{
							scale+=0.25;
							double d=pElemLoad->fDist*1000,s=scale*pFemElem->length;
							if(d<s-EPS&&d>prev_s+EPS)
							{	//集中力在截面左侧（或截面上）
								pStationElem->sects[j].F_L_S.z+=P;
								//等效两端X及Y轴方向的力偶
								armLength=(d-s)*pFemElem->lenStdVec.y;
								pStationElem->sects[j].M_L_S.x-=P*armLength;	//N*mm
								armLength=(d-s)*pFemElem->lenStdVec.x;
								pStationElem->sects[j].M_L_S.y+=P*armLength;	//N*mm
							}
							else if(d>=s-EPS&&d<=s+EPS)	//集中力在截面上
								pStationElem->sects[j].F_T_S.z+=P;	//N
							prev_s=s;
						}
					}
				}
				else if(pElemLoad->loadType==3)
				{	//集中力偶
					double M=pElemLoad->fLoad*1000000;	//kN*m->N*mm
					double scaleFactor=1000*pElemLoad->fDist/pFemElem->length;
					GEPOINT Mv,M2Axis,M2Sect;	//集中力偶轴向扭矩投影分量及横截面弯矩投影分量,N*m
					if(pElemLoad->directionI==1)
					{	//X向集中力偶
						M2Axis=pFemElem->lenStdVec*(pFemElem->lenStdVec.x*M);
						M2Sect.Set(M-M2Axis.x,-M2Axis.y,-M2Axis.z);
						Mv.Set(M,0,0);
					}
					else if(pElemLoad->directionI==2)
					{	//Y向集中力偶
						M2Axis=pFemElem->lenStdVec*(pFemElem->lenStdVec.y*M);
						M2Sect.Set(-M2Axis.x,M-M2Axis.y,-M2Axis.z);
						Mv.Set(0,M,0);
					}
					else if(pElemLoad->directionI==3)
					{	//Z向集中力偶
						M2Axis=pFemElem->lenStdVec*(pFemElem->lenStdVec.z*M);
						M2Sect.Set(-M2Axis.x,-M2Axis.y,M-M2Axis.z);
						Mv.Set(0,0,M);
					}
					//等效横向集中力偶产生的端截面反作用力
					GEPOINT Rvec=(pFemElem->lenStdVec^M2Sect)/pFemElem->length;
					if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
					{
						Rvec*=6*scaleFactor*(1-scaleFactor);
						//等效两端截面的横向弯矩
						GEPOINT ms=M2Sect*(1-scaleFactor)*(2*scaleFactor-(1-scaleFactor));
						GEPOINT me=M2Sect*scaleFactor*(2*(1-scaleFactor)-scaleFactor);
						pFemElem->pStart->Moment+=ms;
						pFemElem->pEnd  ->Moment+=me;
						pStationElem->M_sect_s+=ms;
						pStationElem->M_sect_e+=me;
					}
					else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
					{
						Rvec*=1.5*(1-(1-scaleFactor)*(1-scaleFactor));
						//等效两端截面的横向弯矩
						GEPOINT ms=0.5*M2Sect*(1-3*(1-scaleFactor)*(1-scaleFactor));
						pFemElem->pStart->Moment+=ms;
						pStationElem->M_sect_s+=ms;
					}
					else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
					{
						Rvec*=1.5*(1-scaleFactor*scaleFactor);
						//等效两端截面的横向弯矩
						GEPOINT me=0.5*M2Sect*(1-3*scaleFactor*scaleFactor);
						pFemElem->pEnd  ->Moment+=me;
						pStationElem->M_sect_e+=me;
					}
					//else if(pFemElem->start_joint_type==JOINT_HINGE&&pFemElem->end_joint_type==JOINT_HINGE)
					//	Rvec*=1;
					pStationElem->F_sect_s+=Rvec;
					pStationElem->F_sect_e-=Rvec;
					pFemElem->pStart->Fxyz+=Rvec;
					pFemElem->pEnd  ->Fxyz-=Rvec;

					//等效轴向扭矩在端截面产生的反作用力矩
					bool bShaftStart=(pFemElem->cbStartJoint&0x08)>0;
					bool bShaftEnd  =(pFemElem->cbEndJoint&0x08)>0;
					if(bShaftStart&&bShaftEnd)
					{
						//等效集中力偶的扭矩投影分量（线性比例等效）
						GEPOINT ms=M2Axis*(1-scaleFactor);
						GEPOINT me=M2Axis*scaleFactor;
						pFemElem->pStart->Moment+=ms;
						pFemElem->pEnd  ->Moment+=me;
						pStationElem->M_sect_s+=ms;
						pStationElem->M_sect_e+=me;
					}
					else if(bShaftStart)
					{	//等效扭矩至始端固接点
						pFemElem->pStart->Moment+=M2Axis;
						pStationElem->M_sect_s+=M2Axis;
					}
					else if(bShaftEnd)
					{	//等效扭矩至终端固接点
						pFemElem->pEnd  ->Moment+=M2Axis;
						pStationElem->M_sect_e+=M2Axis;
					}
					//累加计算单元集中力偶在单元中间断面上的内力(矩)影响
					double prev_s=0;
					for(j=0,scale=0;j<3;j++)
					{
						scale+=0.25;
						double d=pElemLoad->fDist*1000,s=scale*pFemElem->length;
						if(d<s-EPS&&d>prev_s+EPS)
						{	//集中力偶在截面左侧（或截面上）
							pStationElem->sects[j].M_L_S+=Mv;
						}
						else if(d>=s-EPS&&d<=s+EPS)	//集中力在截面上
							pStationElem->sects[j].M_T_S+=Mv;	//N
						prev_s=s;
					}
				}
			}
			CHashList<GENERIC_FORCE> hashFemNodeWeight;
			CHashList<ELEMSECT_FORCE> hashElemSectForce;
			for(pFemNode=listFemNodes.GetFirst(),pStationNode=hashSubstationNodes.GetFirst();pFemNode;
				pFemNode=listFemNodes.GetNext(),pStationNode=hashSubstationNodes.GetNext())
			{
				if(pFemNode->Fxyz.IsZero())
					continue;
				CExternalNodeLoad* pExterNodeLoad=pLdsCase->hashNodeLoad.Add(pStationNode->pLdsNode->handle);
				pExterNodeLoad->designLoad=pFemNode->Fxyz;
				if(fabs(pLoad->fGamaG1-pLoad->fGamaG2)>=0.1)
				{
					GENERIC_FORCE gf(pFemNode->Fxyz,pFemNode->Moment);
					hashFemNodeWeight.SetValue(pFemNode->point_i,gf);
				}
			}
			if(fabs(pLoad->fGamaG1-pLoad->fGamaG2)>=0.1)
			{
				for(pStationElem=hashSubstationElems.GetFirst();pStationElem;pStationElem=hashSubstationElems.GetNext())
				{
					ELEMSECT_FORCE sect;
					sect.Fs=GENERIC_FORCE(pStationElem->F_sect_s,pStationElem->M_sect_s);
					sect.Fe=GENERIC_FORCE(pStationElem->F_sect_e,pStationElem->M_sect_e);
					hashElemSectForce.SetValue(pStationElem->elemI,sect);
				}
			}
			//3.施加重力荷载
			double weightCoef=1.0;	//也可是1.2
			for(int iWeightStyle=0;iWeightStyle<2;iWeightStyle++)
			{
				if(iWeightStyle==0)
					weightCoef=pLoad->fGamaG1;
				else if(iWeightStyle==1)
				{
					weightCoef=pLoad->fGamaG2;
					if(fabs(pLoad->fGamaG1-pLoad->fGamaG2)<=0.1)
						continue;
				}
				if(iWeightStyle==1)
				{	//恢复无重力模式下的原外荷载
					GENERIC_FORCE* pGF=NULL;
					ELEMSECT_FORCE* pSectF=NULL;
					for(pFemNode=listFemNodes.GetFirst();pFemNode;pFemNode=listFemNodes.GetNext())
					{
						pFemNode->Fxyz.Set();
						if((pGF=hashFemNodeWeight.GetValue(pFemNode->point_i))!=NULL)
						{
							pFemNode->Fxyz=pGF->Fxyz;
							pFemNode->Moment=pGF->Moment;
						}
					}
					for(pStationElem=hashSubstationElems.GetFirst();pStationElem;pStationElem=hashSubstationElems.GetNext())
					{
						if((pSectF=hashElemSectForce.GetValue(pStationElem->elemI))!=NULL)
						{
							pStationElem->F_sect_s=pSectF->Fs.Fxyz;
							pStationElem->M_sect_s=pSectF->Fs.Moment;
							pStationElem->F_sect_e=pSectF->Fe.Fxyz;
							pStationElem->M_sect_e=pSectF->Fe.Moment;
						}
					}
				}
				for(pFemElem=listFemElems.GetFirst(),pStationElem=hashSubstationElems.GetFirst();pFemElem;
					pFemElem=listFemElems.GetNext(),pStationElem=hashSubstationElems.GetNext())
				{
					double M=0,ql=pFemElem->density*pFemElem->length*pFemElem->A*10*weightCoef;//N
					//pFemElem->pStart->Fxyz.z+=weightFz*0.5;
					//pFemElem->pEnd  ->Fxyz.z+=weightFz*0.5;
					pStationElem->QWF=pFemElem->density*pFemElem->A*10*weightCoef;
					int jointStartType=pFemElem->start_joint_type;
					int jointEndType=pFemElem->end_joint_type;
					if((pFemElem->cbStartJoint&0x10)+(pFemElem->cbStartJoint&0x20)==0)
						jointStartType=JOINT_HINGE;
					if((pFemElem->cbEndJoint&0x10)+(pFemElem->cbEndJoint&0x20)==0)
						jointEndType=JOINT_HINGE;
					if((jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)||
						(jointStartType==JOINT_HINGE&&jointEndType==JOINT_HINGE))
					{
						pFemElem->pStart->Fxyz.z+=ql/2;
						pFemElem->pEnd  ->Fxyz.z+=ql/2;
						pStationElem->F_sect_s.z+=ql/2;
						pStationElem->F_sect_e.z+=ql/2;
					}
					//累加计算均布力在单元中间断面上的内力(矩)影响
					//等效两端X及Y轴方向的力偶
					if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_RIGID)
					{	//两端固接
						M= pFemElem->lenStdVec.y*ql*pFemElem->length/12;	//N*mm,ql^2/12
						pFemElem->pStart->Moment.x+=M;
						pFemElem->pEnd  ->Moment.x-=M;
						pStationElem->M_sect_s.x+=M;
						pStationElem->M_sect_e.x-=M;
						M=-pFemElem->lenStdVec.x*ql*pFemElem->length/12;	//N*mm,ql^2/12
						pFemElem->pStart->Moment.y+=M;
						pFemElem->pEnd  ->Moment.y-=M;
						pStationElem->M_sect_s.y+=M;
						pStationElem->M_sect_e.y-=M;
					}
					else if(jointStartType==JOINT_RIGID&&jointEndType==JOINT_HINGE)
					{	//始端固接,终端铰接
						pStationElem->F_sect_s.z+=ql*0.625;
						pStationElem->F_sect_e.z+=ql*0.375;
						pFemElem->pStart->Fxyz.z+=ql*0.625;	//ql*5/8
						pFemElem->pEnd  ->Fxyz.z+=ql*0.375;	//ql*3/8
						M= pFemElem->lenStdVec.y*ql*pFemElem->length/8;	//N*mm,ql^2/8
						pFemElem->pStart->Moment.x+=M;
						pStationElem->M_sect_s.x+=M;
						M=-pFemElem->lenStdVec.x*ql*pFemElem->length/8;	//N*mm,ql^2/8
						pFemElem->pStart->Moment.z+=M;
						pStationElem->M_sect_s.z+=M;
					}
					else if(jointStartType==JOINT_HINGE&&jointEndType==JOINT_RIGID)
					{	//始端铰接,终端固接
						pStationElem->F_sect_s.z+=ql*0.375;
						pStationElem->F_sect_e.z+=ql*0.625;
						pFemElem->pStart->Fxyz.z+=ql*0.375;	//ql*3/8
						pFemElem->pEnd  ->Fxyz.z+=ql*0.625;	//ql*5/8
						M= pFemElem->lenStdVec.y*ql*pFemElem->length/8;	//N*mm,ql^2/8
						pFemElem->pEnd->Moment.x-=M;
						pStationElem->M_sect_e.x-=M;
						M=-pFemElem->lenStdVec.x*ql*pFemElem->length/8;	//N*mm,ql^2/8
						pFemElem->pEnd->Moment.z-=M;
						pStationElem->M_sect_e.z-=M;
					}
				}
				BOOL rslt;
				if(bFirstResolve)
				{
					if(g_sysPara.AnalysisPara.m_iResolverStyle==0)
						rslt=pResolver->truss_resolve(&listFemNodes,&listFemElems,GAUSS_REDUCTION,false);
					else
						rslt=pResolver->truss_resolve(&listFemNodes,&listFemElems,WAVE_FRONT_METHOD,false);
				}
				else
					rslt=pResolver->truss_equation_recur(&listFemNodes,&listFemElems);
				bFirstResolve=false;
				for(pFemElem=listFemElems.GetFirst();pFemElem;pFemElem=listFemElems.GetNext())
					pFemElem->pTag=NULL;
				CXhChar200 fn(filename);
				char* separtor=SearchChar(fn,'.',true);
				*separtor=0;
				fn.Append(CXhChar50("_%d_weiyi#G%d.txt",iCaseNo,ftoi(weightCoef*10)));
				FILE* fp=fopen(fn,"wt");
				for(pFemNode=listFemNodes.GetFirst(),pStationNode=hashSubstationNodes.GetFirst();pFemNode;
					pFemNode=listFemNodes.GetNext(),pStationNode=hashSubstationNodes.GetNext())
				{
					fprintf(fp,"%d.  %f  %f  %f  %f  %f  %f\n",pStationNode->pointI,
						pFemNode->offset.x,-pFemNode->offset.y,-pFemNode->offset.z,
						pFemNode->rot_offset.x,-pFemNode->rot_offset.y,-pFemNode->rot_offset.z);
					CNodeAnalyzeInfo nodeAnalInfo;
					//初始化键值区
					nodeAnalInfo.m_hNode=pStationNode->pLdsNode->handle;
					nodeAnalInfo.m_iModuleNo=pModule->iNo;
					nodeAnalInfo.legword.dwQuadLeg=pModule->ActiveQuadLegDword();
					nodeAnalInfo.m_dwStatusNo=iCaseNo;
					//初始化数据区
					nodeAnalInfo.SetZero();
					nodeAnalInfo.offset=pFemNode->offset;
					nodeAnalInfo.rot_offset=pFemNode->rot_offset;
					nodeAnalInfo.external_force=pFemNode->Fxyz;
					nodeAnalInfo.internal_moment=-pFemNode->Moment;
					nodeAnalInfo.unbalance_force=pFemNode->unbalance_force;
					postreport.AppendToAnalyzeBuffer(nodeAnalInfo);
				}
				fclose(fp);
				fn.Copy(filename);
				separtor=SearchChar(fn,'.',true);
				*separtor=0;
				fn.Append(CXhChar50("_%d_li#G%d.txt",iCaseNo,ftoi(weightCoef*10)));
				fp=fopen(fn,"wt");
				for(pFemElem=listFemElems.GetFirst(),pStationElem=hashSubstationElems.GetFirst();pFemElem;
					pFemElem=listFemElems.GetNext(),pStationElem=hashSubstationElems.GetNext())
				{
					GEPOINT SF,SM,EF,EM;	//节间始末端断面内力和内力矩
					GEPOINT QF=pStationElem->QF;
					QF.z+=pStationElem->QWF;	//将重力荷载折算成分布荷载处理
					GECS cs;
					cs.axis_x=pFemElem->pEnd->pos-pFemElem->pStart->pos;
					normalize(cs.axis_x);
					cs.axis_z=CalLocalCS_Z_AXIS(cs.axis_x);
					cs.axis_y=cs.axis_z^cs.axis_x;
					//1.整杆始末端断面内力信息输出
					SF=-(pFemElem->start_force-pStationElem->F_sect_s);
					SM=-(pFemElem->start_moment-pStationElem->M_sect_s);
					EF=pFemElem->end_force-pStationElem->F_sect_e;
					EM=pFemElem->end_moment-pStationElem->M_sect_e;
					GEPOINT sf=cs.TransVToCS(SF);
					GEPOINT ef=cs.TransVToCS(EF);
					GEPOINT sm=cs.TransVToCS(SM);
					GEPOINT em=cs.TransVToCS(EM);
					fprintf(fp,"%d{%d-%d}.  1.  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f\n",pStationElem->elemI,pStationElem->startPointI,pStationElem->endPointI,
						-sf.x*0.001, sf.y*0.001, sf.z*0.001,-sm.x*1e-6 , sm.y*1e-6 , sm.z*1e-6,
						 ef.x*0.001,-ef.y*0.001,-ef.z*0.001, em.x*1e-6 ,-em.y*1e-6 ,-em.z*1e-6);
					//fprintf(fp,"%d.  1.  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f\n",pStationElem->elemI,
					//	SF.x*0.001,-SF.y*0.001,-SF.z*0.001,SM.x*1e-6,-SM.y*1e-6,-SM.z*1e-6,
					//	EF.x*0.001,-EF.y*0.001,-EF.z*0.001,EM.x*1e-6,-EM.y*1e-6,-EM.z*1e-6);
					//j+1号－j+2号断面内力信息输出
					if( !pStationElem->sects[0].F_T_S.IsZero()||!pStationElem->sects[0].F_L_S.IsZero()||!pStationElem->sects[0].M_T_S.IsZero()||!pStationElem->sects[0].M_L_S.IsZero()||
						!pStationElem->sects[1].F_T_S.IsZero()||!pStationElem->sects[1].F_L_S.IsZero()||!pStationElem->sects[1].M_T_S.IsZero()||!pStationElem->sects[1].M_L_S.IsZero()||
						!pStationElem->sects[2].F_T_S.IsZero()||!pStationElem->sects[2].F_L_S.IsZero()||!pStationElem->sects[2].M_T_S.IsZero()||!pStationElem->sects[2].M_L_S.IsZero()||
						!QF.IsZero())//pStationElem->cbJointStart>0x07||pStationElem->cbJointEnd>0x07||
						
					{
						for(j=0;j<4;j++)
						{
							EF=SF;
							EM=SM;
							if(j<3)
							{
								EF+=pStationElem->sects[j].F_L_S;
								EM+=pStationElem->sects[j].M_L_S;
							}
							if(!QF.IsZero())
							{
								EF-=QF*0.25*pFemElem->length;
								GEPOINT MQ=0.03125*QF^pFemElem->lenStdVec;
								EM-=MQ*pFemElem->length*pFemElem->length;
							}
							EM+=SF^pFemElem->lenStdVec*(pFemElem->length*0.25);
							sf=cs.TransVToCS(SF);
							ef=cs.TransVToCS(EF);
							sm=cs.TransVToCS(SM);
							em=cs.TransVToCS(EM);
							fprintf(fp,"%d.  %d.  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f\n",pStationElem->elemI,j+2,
								-sf.x*0.001, sf.y*0.001, sf.z*0.001,-sm.x*1e-6 , sm.y*1e-6 , sm.z*1e-6,
								 ef.x*0.001,-ef.y*0.001,-ef.z*0.001, em.x*1e-6 ,-em.y*1e-6 ,-em.z*1e-6);
							if(j==3)
								break;
							SF=EF-pStationElem->sects[j].F_T_S;
							SM=EM-pStationElem->sects[j].M_T_S;
						}
					}
					ELEM_ANALYZE_INFO elemAnalInfo;
					//初始化键值区
					elemAnalInfo.idElem=pStationElem->sectionI;
					elemAnalInfo.m_iModuleNo=pModule->iNo;
					elemAnalInfo.legword.dwQuadLeg=pModule->ActiveQuadLegDword();
					elemAnalInfo.m_dwStatusNo=iCaseNo;
					//初始化数据区
					elemAnalInfo.iStartI=pFemElem->pStart->point_i;
					elemAnalInfo.iEndI=pFemElem->pEnd->point_i;
					elemAnalInfo.m_hParentRod=pStationElem->pLdsRod->handle;
					elemAnalInfo.pParentRod=pStationElem->pLdsRod;
					elemAnalInfo.N=pFemElem->N;
					elemAnalInfo.moment=pFemElem->start_moment;
					elemAnalInfo.weight=pFemElem->density*pFemElem->length*pFemElem->A*10*weightCoef;
					//elemAnalInfo.moment=pFemElem->end_moment;

					//elemAnalInfo.iStartI=pElem->pStart->point_i;
					//elemAnalInfo.iEndI=pElem->pEnd->point_i;
					//elemAnalInfo.m_hParentRod=pElem->pOrgPole->handle;
					//elemAnalInfo.weight=pElem->SelfWeight();
					postreport.AppendToAnalyzeBuffer(elemAnalInfo);
				}
				fclose(fp);
			}
		}
		timer.End();
	}
	catch (char* error_info){	//防止中间抛出异常导致无法销毁pResolver空占内存　wjh-2015.10.21
		AfxMessageBox(error_info);
	}
	postreport.hashNodeAnalBuffer.RebuildHashTable();
	postreport.hashElemAnalBuffer.RebuildHashTable();
	CFemResolver::Destroy(pResolver->GetHandle());
	AfxMessageBox(CXhChar50("time elapse %.1fs!",(timer.GetEndTicks()-timer.GetStartTicks())*0.001));
}*/
#endif