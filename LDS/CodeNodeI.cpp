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
//��һ����ZֵС�򷵻ظ�ֵ,������ȷ���0,���򷵻���ֵ
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
//ԭ��1.��������2.���м䵽����,3.�����Һ�ǰ��
//������ǰ�����򷵻�TRUE���򷵻�FALSE
static BOOL SortTaAtomUpDown(CLDSNode*&pNode1,CLDSNode*&pNode2)
{
	if(pNode1->feature!=pNode2->feature)
	{	//�������ȷ���Ҫ�ڵ�ı�ű��ŵ������ڵ㼰�˽ڵ���棬�Ա���TTA�ȳ���Խ�
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
	char ciRecodeMode;	//0.ȫ�����±���;1.�̳�ģʽ
	bool bInheritPracticalNodeI;//�̳�ʵԪ�ڵ�ı��
	bool bInheritDummyNodeI;	//�̳���Ԫ�ڵ�ı��
	bool bCanUseVacancyI;		//True:������
	UINT minDummyNodeI;			//��С��Ԫ���ýڵ���
	UINT maxPracticalNodeI;		//ԭ�����ʵԪ�ڵ���
	UINT maxDummyNodeI;			//ԭ�������Ԫ�ڵ���
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
//ʶ����ȡ���ܴ����ڵ㼰�˽ڵ㼯��
void IdentifyForceOrEndNodes()
{
	CLDSNode* pNode;
	NODESET allnodeset;
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->force_type==FORCE_NODE)
			pNode->feature=1;	//�ֶ��趨�Ĵ����ڵ�
		else //if(pNode->force_type==AUTO_NODE||pNode->force_type==SWING_NODE)
			pNode->feature=0;	//������Ϊ�Ǵ����ڵ㣨�������Ķ˵㣩
		allnodeset.append(pNode);
	}
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->pStart) 
			pRod->pStart->feature=1;
		if(pRod->pEnd) 
			pRod->pEnd->feature=1;
	}
	CPreProcessor::SmartJustifyNodeForceType(&Ta,allnodeset);//�м��������ѹ�������ص㼴Ϊ�����ڵ�
}
void CMainFrame::OnCodeNodeI() 
{
	int i,j,n,m_nMaxCode=0;
	CLDSNode *pNode;
	BeginWaitCursor();
/*		�ڵ��Ź���
	1.�ڵ���˳��:�������ϡ�������,�����浽����,������
	2.�ڵ���������,���˳��ֿպŶԳƽڵ�Ӧʮλ������ʮλ���ϱ����ͬ��
	4.�ڵ��Ÿ�λ��ʾ�ڵ�λ��1��ʾ���ҶԳ�2��ʾǰ��Գ�3��ʾ�ԽǶԳ�
*/
	n = Ta.Node.GetNodeNum();
	if(n<=0)		//������������б��
		return;
	//���ӽڵ��ŵ�ģʽ:0.���±���;1.�̳�ģʽ(����ԭ�����нڵ��ţ�����ɷ�Ϊ�����պͲ������գ����������λ��Ԫ�ڵ�)wjh -2017.2.6
	static CNodeIDlg schemadlg;
	if(schemadlg.DoModal()!=IDOK)
		return;
	//������Ԫ�ڵ���С��ţ�Ĭ��0,����ʵԪ�ڵ㣨�������ڵ��Ԫ�˽ڵ㣩
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
	//��Ž�����
	CLDSNode** ta_node_arr = new CLDSNode*[n];
	try
	{
#ifdef AFX_TARG_ENU_ENGLISH
		PutMessage(this,"Library is establishing label code of tower's nodes!");
#else 
		PutMessage(this,"���ڽ���ȫ���Ľڵ��Ż��ܿ�!");
#endif
		i = n = 0;
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			ta_node_arr[i] = pNode;
			n++;
			i++;
		}


		BOOL bSorted = FALSE;	//ð�������е������־
		char sPrompt[MAX_TEMP_CHAR_100+1]="";
#ifdef AFX_TARG_ENU_ENGLISH
		_snprintf(sPrompt,MAX_TEMP_CHAR_100,"It is sorting by the order from up to down");
#else 
		_snprintf(sPrompt,MAX_TEMP_CHAR_100,"���ڰ��������µĹ����������");
#endif
		PutMessage(this,sPrompt);
		for(i=0;i<n-1;i++)   //ð������
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
			if(pNode->point_i>0)				//�Ƿ��ѱ��
			{
				serial_no=max(serial_no,1+pNode->point_i/10);
				continue;
			}
			if(pNode->feature==0&&serial_no*10<codemode.minDummyNodeI)
				serial_no=codemode.minDummyNodeI/10;	//��֤��Ԫ�ڵ��Ų�С���趨��Сֵ
			if(pNode->feature==0&&!codemode.bCanUseVacancyI&&serial_no*10<=codemode.maxDummyNodeI)
				serial_no=1+codemode.maxDummyNodeI/10;	//��֤��Ԫ�ڵ��Ų�С���趨��Сֵ
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
	PutMessage(this,"���ڽ��нڵ㸸�˼����");
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
			error_type=1;		//�ڵ㸸�˼����Ϸ�(�Ҳ������Ǹ˼�)
		else
		{
			pPart->UnifiedCfgword();
			if(pNode==pPart.LinePartPointer()->pStart||pNode==pPart.LinePartPointer()->pEnd)
			{	
				if(!pNode->cfgword.And(pPart->cfgword))
					error_type=-1;		//��ĺ������޷�����
				else
					error_type=-2;		//�˽ڵ��д���һ������Ƿ��и������˼����ڵ�λ�ڸ˼��ڲ�ʱΪ���ϸ��˼���
			}
			else if(!pNode->cfgword.IsEqual(pPart->cfgword))
				error_type=2;	//�ڵ����丸�˼���ĺ��벻һ��
			else
			{
				CPtInLineCheck lineCheck(pPart.LinePartPointer()->pStart->Position(false),pPart.LinePartPointer()->pEnd->Position(false));
				double justify=lineCheck.CheckPoint(pNode->Position(false));
				if(justify<-EPS||justify>1+EPS)
					error_type=3;	//�ڵ㲻�ڸ��˼���
			}
		}
		CXhChar50 cfgStr,rodCfgStr;
		if(pNode->Layer(0)=='L')	//���ȹ���
			pNode->cfgword.GetLegScopeStr(cfgStr,50,true);
		else
			pNode->cfgword.GetBodyScopeStr(cfgStr);
		if(pPart.IsHasPtr()&&pPart->Layer(0)=='L')	//���ȹ���
			pPart->cfgword.GetLegScopeStr(rodCfgStr,50,true);
		else if(pPart.IsHasPtr())
			pPart->cfgword.GetBodyScopeStr(rodCfgStr);
		if(error_type!=0)	//�ڵ㸸�˼��д�����Ҫ����
		{
			if(error_type!=-2)	//����ȷ���ڵ㸸�˼��Ƿ�����д���һ�����
				hits++;
			if(error_type==-1)
#ifdef AFX_TARG_ENU_ENGLISH
				logger.Log("0X%X node doesn't match  configure word with parent rod 0X%X��Please update it ��\n",pNode->handle,pNode->hFatherPart);
#else 
				logger.Log("0X%X�ڵ�{%s}���丸�˼�0X%X��ĺ�{%s}��һ�£����ֶ�������\n",pNode->handle,(char*)cfgStr,pNode->hFatherPart,(char*)rodCfgStr);
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
							logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X node is endpoint of parent rod 0X%X��but it is interior point of other rod��it is unreasonable !So parent rod is automatically changed to A0X%X��\n",pNode->handle,pNode->hFatherPart,pRod->handle);
#else 
							logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X�ڵ�Ϊ���˼�0X%X�˵㣬����������˼��ڵ㣬���������Զ����ڵ㸸�˼�����Ϊ0X%X��\n",pNode->handle,pNode->hFatherPart,pRod->handle);
#endif
							hits++;
						}
						break;
					}
				}
				Ta.Parts.pop_stack(pushed);
				if(error_type>0)
				{
					if(pRod!=NULL)	//�������ø��˼�
					{
						correct_hits++;
#ifdef AFX_TARG_ENU_ENGLISH
						logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"The node's parent rod is automatically changed to 0X%X��\n",pRod->handle);
#else 
						logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"���Զ����ڵ㸸�˼�����Ϊ0X%X��\n",pRod->handle);
#endif
					}
					else
					{
#ifdef AFX_TARG_ENU_ENGLISH
						if(error_type==-1)
							logger.Log("0X%X node doesn't match  configure word with parent rod 0X%X��Please update it ��\n",pNode->handle,pNode->hFatherPart);
						else if(error_type==1)
							logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "0X%X node's parent rod 0X%X is not legal(Can't find it or not rod)��Automatically correct is failed,please modify manually��",pNode->handle,pNode->hFatherPart);
						else if(error_type==2)
							logger.Log("0X%X node doesn't match configure word with parent rod 0X%X��Automatically correct is failed,please modify manually��",pNode->handle,pNode->hFatherPart);
						else if(error_type==3)
							logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "0X%X node isn't in the area of parent rod 0X%X,parent rod isn't legal��Automatically correct is failed,please modify manually��",pNode->handle,pNode->hFatherPart);
#else 
						if(error_type==-1)
							logger.Log("0X%X�ڵ�{%s}���丸�˼�0X%X��ĺ�{%s}��һ�£����ֶ�������\n",pNode->handle,(char*)cfgStr,pNode->hFatherPart,(char*)rodCfgStr);
						else if(error_type==1)
							logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X�ڵ�ĸ��˼�0X%X���Ϸ�(�Ҳ������Ǹ˼�)���Զ�����ʧ�ܣ����ֶ�������",pNode->handle,pNode->hFatherPart);
						else if(error_type==2)
							logger.Log("0X%X�ڵ�{%s}���丸�˼�0X%X��ĺ�{%s}��һ�¡��Զ�����ʧ�ܣ����ֶ�������",pNode->handle,(char*)cfgStr,pNode->hFatherPart,(char*)rodCfgStr);
						else if(error_type==3)
							logger.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "0X%X�ڵ㲻���丸�˼�0X%X�����ڣ����ǺϷ��ĸ��˼����Զ�����ʧ�ܣ����ֶ�������",pNode->handle,pNode->hFatherPart);
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
		MessageBox("The node has been numbered,but its parent rod has some errors.Advice to correct them��");
	else
		MessageBox("Congratulate on your success for numbering nodes��");
#else 
	if(hits>correct_hits)
		MessageBox("�ѽ��нڵ��ţ����ڵ㸸���д�������������");
	else
		MessageBox("ף����ڵ��ųɹ���");
#endif
	GetLDSView()->RedrawAll();
}
/*//��AutoCad R14�н��л�ͼ
void CTowerView::OnDrawTaInAcad() 
{
	m_sAcadPath = AfxGetApp()->GetProfileString("ACAD","ACAD_PATH","");
	if(m_sAcadPath.IsEmpty())//�����ⲻ������
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
		AfxMessageBox("��ͼ�������ļ�����ʧ��,����������ͼģ��!");
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