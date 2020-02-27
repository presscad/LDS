//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "MainFrm.h"
#include "LDSDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(__LDS_)||defined(__TSA_)
static void swap_ta_atom(CLDSNode *&pAtom1,CLDSNode*&pAtom2)
{
	CLDSNode *pAtom = pAtom1;
	pAtom1 = pAtom2;
	pAtom2 = pAtom;
}
//第一参数Z值小则返回负值,二者相等返回0,否则返回正值
static int CompareTaAtomUpDown(CLDSNode *pNode1,CLDSNode *pNode2)
{
	if(pNode1->Position(false).z>pNode2->Position(false).z)
		return 1;
	else if(pNode1->Position(false).z<pNode1->Position(false).z)
		return -1;
	else
		return 0;
	return TRUE;
}
//原则1.自上至下2.自中间到两边,3.先左右后前后
//若排序前已有序返回TRUE否则返回FALSE
static BOOL SortTaAtomUpDown(CLDSNode*&pNode1,CLDSNode*&pNode2)
{
	if(pNode1->feature!=pNode2->feature)
	{	//将交叉点等非主要节点的编号编排到传力节点及端节点后面，以便与TTA等程序对接
		if(pNode2->feature-pNode1->feature>0)
		{
			swap_ta_atom(pNode1,pNode2);
			return FALSE;
		}
		return TRUE;
	}
	if(pNode1->Position(false).z>pNode2->Position(false).z+EPS)
	{
		swap_ta_atom(pNode1,pNode2);
		return FALSE;
	}
	else if(pNode1->Position(false).z<pNode2->Position(false).z-EPS)
		return TRUE;
	else if(fabs(pNode1->Position(false).x)<fabs(pNode2->Position(false).x)-EPS)
	{
		swap_ta_atom(pNode1,pNode2);
		return FALSE;
	}
	else if(fabs(pNode1->Position(false).x)>fabs(pNode2->Position(false).x)+EPS)
		return TRUE;
	else if(fabs(pNode1->Position(false).y)<fabs(pNode2->Position(false).y)-EPS)
	{
		swap_ta_atom(pNode1,pNode2);
		return FALSE;
	}
	return TRUE;
}

struct CODENODEI_RULE{
	char ciRecodeMode;	//0.全部重新编排;1.继承模式
	bool bInheritPracticalNodeI;//继承实元节点的编号
	bool bInheritDummyNodeI;	//继承哑元节点的编号
	bool bCanUseVacancyI;		//True:允许插空
	UINT minDummyNodeI;			//最小哑元可用节点编号
	UINT maxPracticalNodeI;		//原有最大实元节点编号
	UINT maxDummyNodeI;			//原有最大哑元节点编号
};
static void ClearNodeI(CODENODEI_RULE& codemode)
{
	CLDSNode *pNode;
	for(pNode=Ta.Node.GetFirst();pNode!=NULL;pNode=Ta.Node.GetNext())
	{
		if(codemode.ciRecodeMode==0||(pNode->feature==0&&!codemode.bInheritDummyNodeI)||
			(pNode->feature==1&&!codemode.bInheritPracticalNodeI))
			pNode->point_i=0;
	}
}

//from Node.cpp
int MirPointI(int pointI,MIRMSG msg);
#include "PreProcessor.h"
#include "NodeIDlg.h"
//识别提取汇总传力节点及端节点集合
void IdentifyForceOrEndNodes()
{
	CLDSNode* pNode;
	NODESET allnodeset;
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->force_type==FORCE_NODE)
			pNode->feature=1;	//手动设定的传力节点
		else //if(pNode->force_type==AUTO_NODE||pNode->force_type==SWING_NODE)
			pNode->feature=0;	//其余置为非传力节点（非受力材端点）
		allnodeset.append(pNode);
	}
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->pStart) 
			pRod->pStart->feature=1;
		if(pRod->pEnd) 
			pRod->pEnd->feature=1;
	}
	CPreProcessor::SmartJustifyNodeForceType(&Ta,allnodeset);//有集中力或风压轮廓加载点即为传力节点
}
void CMainFrame::OnCodeNodeI() 
{
	int i,j,n,m_nMaxCode=0;
	CLDSNode *pNode;
	BeginWaitCursor();
/*		节点编号规则
	1.节点编号顺序:由下向上、由左到右,由正面到侧面,最后断面
	2.节点编号宜连续,不宜出现空号对称节点应十位（包括十位以上编号相同）
	4.节点编号个位表示节点位置1表示左右对称2表示前后对称3表示对角对称
*/
	n = Ta.Node.GetNodeNum();
	if(n<=0)		//空塔，不需进行编号
		return;
	//增加节点编号的模式:0.重新编排;1.继承模式(保留原来已有节点编号，具体可分为允许插空和不允许插空，重新向后移位哑元节点)wjh -2017.2.6
	static CNodeIDlg schemadlg;
	if(schemadlg.DoModal()!=IDOK)
		return;
	//增加哑元节点最小编号，默认0,紧接实元节点（即受力节点或单元端节点）
	CODENODEI_RULE codemode;
	codemode.ciRecodeMode=schemadlg.m_bRecodeAll?0:1;
	codemode.minDummyNodeI=schemadlg.m_iMinDummyNodeI;
	codemode.maxDummyNodeI=codemode.maxPracticalNodeI=0;
	codemode.bCanUseVacancyI=schemadlg.m_iPriorMiddleVacancy==0;
	codemode.bInheritPracticalNodeI=(schemadlg.m_bInheritPracticalNode==TRUE);
	codemode.bInheritDummyNodeI=(schemadlg.m_bInheritDummyNode==TRUE);
	IdentifyForceOrEndNodes();
	ClearNodeI(codemode);
	CHashList<CLDSNode*>hashNodeI;
	if(codemode.ciRecodeMode!=0)
	{
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(pNode->pointI==0)
				continue;
			hashNodeI.SetValue(pNode->pointI,pNode);
			if(pNode->feature==0)
				codemode.maxDummyNodeI=max(codemode.maxDummyNodeI,pNode->pointI);
			else
				codemode.maxPracticalNodeI=max(codemode.maxPracticalNodeI,pNode->pointI);
		}
	}
	//编号进度条
	CLDSNode** ta_node_arr = new CLDSNode*[n];
	try
	{
#ifdef AFX_TARG_ENU_ENGLISH
		PutMessage(this,"Library is establishing label code of tower's nodes!");
#else 
		PutMessage(this,"正在建立全塔的节点编号汇总库!");
#endif
		i = n = 0;
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			ta_node_arr[i] = pNode;
			n++;
			i++;
		}


		BOOL bSorted = FALSE;	//冒泡排序中的有序标志
		char sPrompt[MAX_TEMP_CHAR_100+1]="";
#ifdef AFX_TARG_ENU_ENGLISH
		_snprintf(sPrompt,MAX_TEMP_CHAR_100,"It is sorting by the order from up to down");
#else 
		_snprintf(sPrompt,MAX_TEMP_CHAR_100,"正在按由上向下的规则进行排序");
#endif
		PutMessage(this,sPrompt);
		for(i=0;i<n-1;i++)   //冒泡排序
		{
			bSorted = TRUE;
			for(j=0;j<n-i-1;j++)
			{
				BOOL b = SortTaAtomUpDown(ta_node_arr[j],ta_node_arr[j+1]);
				bSorted = bSorted&&b;
			}
			if(bSorted)
				break;
		}

		UINT serial_no = 1;
		if(codemode.bInheritPracticalNodeI&&!codemode.bCanUseVacancyI)
			serial_no=1+codemode.maxPracticalNodeI/10;
		for(j=0;j<n;j++)
		{
			pNode = ta_node_arr[j];
			if(pNode->point_i>0)				//是否已编号
			{
				serial_no=max(serial_no,1+pNode->point_i/10);
				continue;
			}
			if(pNode->feature==0&&serial_no*10<codemode.minDummyNodeI)
				serial_no=codemode.minDummyNodeI/10;	//保证哑元节点编号不小于设定最小值
			if(pNode->feature==0&&!codemode.bCanUseVacancyI&&serial_no*10<=codemode.maxDummyNodeI)
				serial_no=1+codemode.maxDummyNodeI/10;	//保证哑元节点编号不小于设定最小值
			pNode->point_i = serial_no*10+CalPointQuadPos(pNode->Position(false))-1;
			if(codemode.bCanUseVacancyI)
			{
				while(hashNodeI.GetValue(pNode->pointI)!=NULL)
				{
					pNode->point_i+=10;
					serial_no++;
				}
			}
			CLDSNode *pMirXNode = pNode->GetMirNode(MIRMSG(2));
			if(pMirXNode&&pMirXNode!=pNode)
				pMirXNode->point_i = MirPointI(pNode->point_i,MIRMSG(2));//serial_no*10+CalPointQuadPos(pMirNode->pos)-1;
			CLDSNode *pMirYNode = pNode->GetMirNode(MIRMSG(1));
			if(pMirYNode&&pMirYNode!=pNode)
				pMirYNode->point_i = MirPointI(pNode->point_i,MIRMSG(1));//serial_no*10+CalPointQuadPos(pMirNode->pos)-1;
			CLDSNode *pMirZNode = pNode->GetMirNode(MIRMSG(3));
			if(pMirZNode&&pMirZNode!=pMirXNode&&pMirZNode!=pMirYNode)
				pMirZNode->point_i = MirPointI(pNode->point_i,MIRMSG(3));//serial_no*10+CalPointQuadPos(pMirNode->pos)-1;
			serial_no++;
		}
	}
	catch(CString sError)
	{
		AfxMessageBox(sError);
		PutMessage(this,NULL);
		delete []ta_node_arr;
		return;
	}
	delete []ta_node_arr;
#ifdef AFX_TARG_ENU_ENGLISH
	PutMessage(this,"The node's parent rod is checking");
#else 
	PutMessage(this,"正在进行节点父杆件检查");
#endif

	int hits=0,correct_hits=0;
	CLogFile logger(CXhChar200("%soutput.txt",theApp.szExecPath));
	CLogErrorLife life(&logger);
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		pNode->UnifiedCfgword();
		CSuperSmartPtr<CLDSPart> pPart=Ta.Parts.FromHandle(pNode->hFatherPart);
		int error_type=0;
		if(pPart.IsNULL()||!pPart->IsLinePart())
			error_type=1;		//节点父杆件不合法(找不到或不是杆件)
		else
		{
			pPart->UnifiedCfgword();
			if(pNode==pPart.LinePartPointer()->pStart||pNode==pPart.LinePartPointer()->pEnd)
			{	
				if(!pNode->cfgword.And(pPart->cfgword))
					error_type=-1;		//配材号问题无法修正
				else
					error_type=-2;		//端节点有待进一步检查是否有更合理父杆件（节点位于杆件内侧时为更合父杆件）
			}
			else if(!pNode->cfgword.IsEqual(pPart->cfgword))
				error_type=2;	//节点与其父杆件配材号与不一致
			else
			{
				CPtInLineCheck lineCheck(pPart.LinePartPointer()->pStart->Position(false),pPart.LinePartPointer()->pEnd->Position(false));
				double justify=lineCheck.CheckPoint(pNode->Position(false));
				if(justify<-EPS||justify>1+EPS)
					error_type=3;	//节点不在父杆件内
			}
		}
		CXhChar50 cfgStr,rodCfgStr;
		if(pNode->Layer(0)=='L')	//接腿构件
			pNode->cfgword.GetLegScopeStr(cfgStr,50,true);
		else
			pNode->cfgword.GetBodyScopeStr(cfgStr);
		if(pPart.IsHasPtr()&&pPart->Layer(0)=='L')	//接腿构件
			pPart->cfgword.GetLegScopeStr(rodCfgStr,50,true);
		else if(pPart.IsHasPtr())
			pPart->cfgword.GetBodyScopeStr(rodCfgStr);
		if(error_type!=0)	//节点父杆件有错误需要修正
		{
			if(error_type!=-2)	//还不确定节点父杆件是否合理，有待进一步检查
				hits++;
			if(error_type==-1)
#ifdef AFX_TARG_ENU_ENGLISH
				logger.Log("0X%X node doesn't match  configure word with parent rod 0X%X！Please update it ！\n",pNode->handle,pNode->hFatherPart);
#else 
				logger.Log("0X%X节点{%s}与其父杆件0X%X配材号{%s}不一致！请手动修正！\n",pNode->handle,(char*)cfgStr,pNode->hFatherPart,(char*)rodCfgStr);
#endif
			if(error_type!=-1)
			{
				BOOL pushed=Ta.Parts.push_stack();
				for(CLDSLinePart *pRod=Ta.Parts.GetFirstLinePart();pRod;pRod=Ta.Parts.GetNextLinePart())
				{
					if(pRod->pStart==NULL||pRod->pEnd==NULL)
						continue;
					if(!pNode->ModuleTogetherWith(pRod))
						continue;
					CPtInLineCheck lineCheck(pRod->pStart->Position(false),pRod->pEnd->Position(false));
					double justify=lineCheck.CheckPoint(pNode->Position(false));
					if(justify>EPS&&justify<1-EPS)
					{
						correct_hits++;
						pNode->hFatherPart=pRod->handle;
						if(error_type==-2)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X node is endpoint of parent rod 0X%X，but it is interior point of other rod，it is unreasonable !So parent rod is automatically changed to A0X%X！\n",pNode->handle,pNode->hFatherPart,pRod->handle);
#else 
							logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X节点为父杆件0X%X端点，但又是其余杆件内点，不合理！已自动将节点父杆件修正为0X%X！\n",pNode->handle,pNode->hFatherPart,pRod->handle);
#endif
							hits++;
						}
						break;
					}
				}
				Ta.Parts.pop_stack(pushed);
				if(error_type>0)
				{
					if(pRod!=NULL)	//已修正好父杆件
					{
						correct_hits++;
#ifdef AFX_TARG_ENU_ENGLISH
						logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"The node's parent rod is automatically changed to 0X%X！\n",pRod->handle);
#else 
						logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"已自动将节点父杆件修正为0X%X！\n",pRod->handle);
#endif
					}
					else
					{
#ifdef AFX_TARG_ENU_ENGLISH
						if(error_type==-1)
							logger.Log("0X%X node doesn't match  configure word with parent rod 0X%X！Please update it ！\n",pNode->handle,pNode->hFatherPart);
						else if(error_type==1)
							logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "0X%X node's parent rod 0X%X is not legal(Can't find it or not rod)！Automatically correct is failed,please modify manually！",pNode->handle,pNode->hFatherPart);
						else if(error_type==2)
							logger.Log("0X%X node doesn't match configure word with parent rod 0X%X！Automatically correct is failed,please modify manually！",pNode->handle,pNode->hFatherPart);
						else if(error_type==3)
							logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "0X%X node isn't in the area of parent rod 0X%X,parent rod isn't legal！Automatically correct is failed,please modify manually！",pNode->handle,pNode->hFatherPart);
#else 
						if(error_type==-1)
							logger.Log("0X%X节点{%s}与其父杆件0X%X配材号{%s}不一致！请手动修正！\n",pNode->handle,(char*)cfgStr,pNode->hFatherPart,(char*)rodCfgStr);
						else if(error_type==1)
							logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X节点的父杆件0X%X不合法(找不到或不是杆件)。自动修正失败，请手动修正！",pNode->handle,pNode->hFatherPart);
						else if(error_type==2)
							logger.Log("0X%X节点{%s}与其父杆件0X%X配材号{%s}不一致。自动修正失败，请手动修正！",pNode->handle,(char*)cfgStr,pNode->hFatherPart,(char*)rodCfgStr);
						else if(error_type==3)
							logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "0X%X节点不在其父杆件0X%X区域内，不是合法的父杆件。自动修正失败，请手动修正！",pNode->handle,pNode->hFatherPart);
#endif
					}
				}
			}
		}
	}
	EndWaitCursor();
	PutMessage(this,NULL);
	m_pDoc->SetModifiedFlag();
#ifdef AFX_TARG_ENU_ENGLISH
	if(hits>correct_hits)
		MessageBox("The node has been numbered,but its parent rod has some errors.Advice to correct them！");
	else
		MessageBox("Congratulate on your success for numbering nodes！");
#else 
	if(hits>correct_hits)
		MessageBox("已进行节点编号，但节点父杆有错误建议检查修正！");
	else
		MessageBox("祝贺你节点编号成功！");
#endif
	GetLDSView()->RedrawAll();
}
/*//在AutoCad R14中进行绘图
void CTowerView::OnDrawTaInAcad() 
{
	m_sAcadPath = AfxGetApp()->GetProfileString("ACAD","ACAD_PATH","");
	if(m_sAcadPath.IsEmpty())//有问题不起作用
	{
		m_sAcadPath = AfxGetApp()->GetProfileString(
			"HEY_LOCAL_MACHINE\\Software\\Autodesk\\AutoCAD\\R14.0\\	\
			ACAD-2451974:76027970","AcadLocation","");
	}
	FILE* fp;
	CString file;
	file = m_sAcadPath.Left(m_sAcadPath.GetLength()-8);
	file+="Acad.rx";
	if((fp=fopen(file,"a+"))==NULL)
	{
		AfxMessageBox("绘图启动动文件创建失败,不能启动绘图模块!");
		return;
	}
	else
	{
		fprintf(fp,"%sDrawTower.arx\n",APP_PATH);
		fclose(fp);
	}
	WinExec(m_sAcadPath,SW_SHOW);
	::DeleteFile(file);
}*/
#endif