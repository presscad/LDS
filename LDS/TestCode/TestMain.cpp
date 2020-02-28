#include "stdafx.h"
#include "..\LDS.h"
//#include "..\LDSDoc.h"
#include "..\LDSView.h"
//#include "env_def.h"
#include "..\MainFrm.h"
#include "..\CmdLineDlg.h"
#include "..\preprocessor.h"
#include "Tower.h"
#include "LmaDlg.h"
#include "LogFile.h"
#include "Expression.h"
#include "TempFile.h"
#include "HiberarchyId.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef __ALFA_TEST_VERSION_

void TestV4_1()
{
#ifdef __ANGLE_PART_INC_
	FILE* fp=fopen("d:\\test.txt","wt");
	long i=0,hits=0,hCurrentObj=0;
	try{
	/*
	long i,ll,hits=0,hCurrentObj=0;
	CProcBarDlg *pProcDlg = NULL;
	try
	{
		DisplayProcess(0);
		i=0;
		ll = Ta.Parts.GetNodeNum();
		CLDSPart *pPart;
		CLDSLineAngle *pLineAngle;
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			i++;
			if(pPart->GetClassTypeId()!=CLS_LINEANGLE&&pPart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
				continue;
			CLdsPartListStack stackParts(&Ta.Parts);
			pLineAngle=(CLDSLineAngle*)pPart;
			hCurrentObj=pLineAngle->handle;
			pLineAngle->ClearFlag();
			pLineAngle->Test_CalPosition();
			DisplayProcess(i*50/ll);
		}
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			i++;
			if(pPart->GetClassTypeId()!=CLS_LINEANGLE&&pPart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
				continue;
			CLdsPartListStack stackParts(&Ta.Parts);
			pLineAngle=(CLDSLineAngle*)pPart;
			hCurrentObj=pLineAngle->handle;
			//1.测试楞点坐标计算
			f3dPoint start_ber=pLineAngle->Start();
			f3dPoint end_ber=pLineAngle->End();
			pLineAngle->ClearFlag();
			pLineAngle->CalPosition();
			f3dPoint offset_start=start_ber-pLineAngle->Start();
			f3dPoint offset_end=end_ber-pLineAngle->End();
			if(offset_start.mod()>0.1)
			{
				fprintf(fp,"0X%X角钢始端Δx=%f;Δy=%f;Δz=%f\n",pLineAngle->handle,offset_start.x,offset_start.y,offset_start.z);
				hits++;
			}
			if(offset_end.mod()>0.1)
			{
				fprintf(fp,"0X%X角钢终端Δx=%f;Δy=%f;Δz=%f\n",pLineAngle->handle,offset_end.x,offset_end.y,offset_end.z);
				hits++;
			}
			//2.测试自动获取工作肢函数
			int work_wing_start=pLineAngle->GetStartWorkWing();
			int work_wing_end=pLineAngle->GetEndWorkWing();
			int work_wing_start_pre=pLineAngle->Test_GetStartWorkWing();
			int work_wing_end_pre=pLineAngle->Test_GetEndWorkWing();
			if(work_wing_start!=work_wing_start_pre)
			{
				fprintf(fp,"0X%X角钢始端工作肢函数不一致start_wing=%d<->start_wing_pre=%d\n",pLineAngle->handle,work_wing_start,work_wing_start_pre);
				hits++;
			}
			if(work_wing_end!=work_wing_end_pre)
			{
				fprintf(fp,"0X%X角钢始端工作肢函数不一致end_wing=%d<->end_wing_pre=%d\n",pLineAngle->handle,work_wing_end,work_wing_end_pre);
				hits++;
			}
			//3.测试固定正负头函数
			double odd=pLineAngle->GetStartExtraOdd(0);
			double odd_pre=pLineAngle->Test_GetStartExtraOdd(0);
			if(fabs(odd-odd_pre)>0.1)
			{
				fprintf(fp,"0X%X角钢始端X肢固定正负头计算函数不一致start_odd_x=%.1f<->start_odd_x_pre=%.1f\n",pLineAngle->handle,odd,odd_pre);
				hits++;
			}
			odd=pLineAngle->GetStartExtraOdd(1);
			odd_pre=pLineAngle->Test_GetStartExtraOdd(1);
			if(fabs(odd-odd_pre)>0.1)
			{
				fprintf(fp,"0X%X角钢始端Y肢固定正负头计算函数不一致start_odd_y=%.1f<->start_odd_y_pre=%.1f\n",pLineAngle->handle,odd,odd_pre);
				hits++;
			}
			odd=pLineAngle->GetStartExtraOdd(2);
			odd_pre=pLineAngle->Test_GetStartExtraOdd(2);
			if(fabs(odd-odd_pre)>0.1)
			{
				fprintf(fp,"0X%X角钢始端楞线固定正负头计算函数不一致end_odd_ber=%.1f<->end_odd_ber_pre=%.1f\n",pLineAngle->handle,odd,odd_pre);
				hits++;
			}
			odd=pLineAngle->GetEndExtraOdd(0);
			odd_pre=pLineAngle->Test_GetEndExtraOdd(0);
			if(fabs(odd-odd_pre)>0.1)
			{
				fprintf(fp,"0X%X角钢终端X肢固定正负头计算函数不一致end_odd_x=%.1f<->end_odd_x_pre=%.1f\n",pLineAngle->handle,odd,odd_pre);
				hits++;
			}
			odd=pLineAngle->GetEndExtraOdd(1);
			odd_pre=pLineAngle->Test_GetEndExtraOdd(1);
			if(fabs(odd-odd_pre)>0.1)
			{
				fprintf(fp,"0X%X角钢终端Y肢固定正负头计算函数不一致end_odd_y=%.1f<->end_odd_y_pre=%.1f\n",pLineAngle->handle,odd,odd_pre);
				hits++;
			}
			odd=pLineAngle->GetEndExtraOdd(2);
			odd_pre=pLineAngle->Test_GetEndExtraOdd(2);
			if(fabs(odd-odd_pre)>0.1)
			{
				fprintf(fp,"0X%X角钢终端楞线固定正负头计算函数不一致end_odd_ber=%.1f<->end_odd_ber_pre=%.1f\n",pLineAngle->handle,odd,odd_pre);
				hits++;
			}
			//4.测试固定正负头函数
			//Test_CopyProperty(CLDSDbObject* pObj)
			//5.测试角钢端头基点位置获取函数
			f3dPoint start=pLineAngle->GetStartDatumPos();
			f3dPoint end=pLineAngle->GetEndDatumPos();
			f3dPoint start_pre=pLineAngle->Test_GetStartDatumPos();
			f3dPoint end_pre=pLineAngle->Test_GetEndDatumPos();
			offset_start=start-start_pre;
			offset_end=end-end_pre;
			if(offset_start.mod()>0.1)
			{
				fprintf(fp,"0X%X角钢始端基点Δx=%f;Δy=%f;Δz=%f\n",pLineAngle->handle,offset_start.x,offset_start.y,offset_start.z);
				hits++;
			}
			if(offset_end.mod()>0.1)
			{
				fprintf(fp,"0X%X角钢终端基点Δx=%f;Δy=%f;Δz=%f\n",pLineAngle->handle,offset_end.x,offset_end.y,offset_end.z);
				hits++;
			}
			//6.测试角钢端头火曲原基点位置获取函数
			start=pLineAngle->GetStartDatumPosEx();
			end=pLineAngle->GetEndDatumPosEx();
			start_pre=pLineAngle->Test_GetStartDatumPosEx();
			end_pre=pLineAngle->Test_GetEndDatumPosEx();
			offset_start=start-start_pre;
			offset_end=end-end_pre;
			if(offset_start.mod()>0.1)
			{
				fprintf(fp,"0X%X角钢始端火曲原基点Δx=%f;Δy=%f;Δz=%f\n",pLineAngle->handle,offset_start.x,offset_start.y,offset_start.z);
				hits++;
			}
			if(offset_end.mod()>0.1)
			{
				fprintf(fp,"0X%X角钢终端火曲原基点Δx=%f;Δy=%f;Δz=%f\n",pLineAngle->handle,offset_end.x,offset_end.y,offset_end.z);
				hits++;
			}
			//7.测试自动获取X肢端头偏移量函数
			double offset_wing_start=pLineAngle->GetStartOffsetByWingX();
			double offset_wing_end=pLineAngle->GetEndOffsetByWingX();
			double offset_wing_start_pre=pLineAngle->Test_GetStartOffsetByWingX();
			double offset_wing_end_pre=pLineAngle->Test_GetEndOffsetByWingX();
			if(work_wing_start!=work_wing_start_pre)
			{
				fprintf(fp,"0X%X角钢始端X肢函数不一致start_wing=%d<->start_wing_pre=%d\n",pLineAngle->handle,offset_wing_start,offset_wing_start_pre);
				hits++;
			}
			if(work_wing_end!=work_wing_end_pre)
			{
				fprintf(fp,"0X%X角钢终端X肢函数不一致end_wing=%d<->end_wing_pre=%d\n",pLineAngle->handle,offset_wing_end,offset_wing_end_pre);
				hits++;
			}
			//8.测试自动获取X肢端头偏移量函数
			offset_wing_start=pLineAngle->GetStartOffsetByWingY();
			offset_wing_end=pLineAngle->GetEndOffsetByWingY();
			offset_wing_start_pre=pLineAngle->Test_GetStartOffsetByWingY();
			offset_wing_end_pre=pLineAngle->Test_GetEndOffsetByWingY();
			if(work_wing_start!=work_wing_start_pre)
			{
				fprintf(fp,"0X%X角钢始端X肢函数不一致start_wing=%d<->start_wing_pre=%d\n",pLineAngle->handle,offset_wing_start,offset_wing_start_pre);
				hits++;
			}
			if(work_wing_end!=work_wing_end_pre)
			{
				fprintf(fp,"0X%X角钢终端X肢函数不一致end_wing=%d<->end_wing_pre=%d\n",pLineAngle->handle,offset_wing_end,offset_wing_end_pre);
				hits++;
			}
			DisplayProcess(50+i*50/ll);
		}
	}
	//以下异常处理有问题，没有处理中间的压栈后未出站问题，但问题不大用户不可能异常到位2048次以上。2003/10/18 WJH
	catch(char *sError)
	{
		DisplayProcess(100);
		char sText[200]="";
		sprintf(sText,"%s,当前构件：0X%X",sError,hCurrentObj);
		AfxMessageBox(sText);
	}
	DisplayProcess(100);
	*/
/*
#ifdef __PART_DESIGN_INC_
	//测试前处理代码
	CLDSNode *pNode;
	CLDSModule* pModule;
	CFGWORD cfgwordBody,cfgwordLeg;
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		cfgwordBody.flag.bytes[pModule->GetBodyNo()-1]=0xff;
		cfgwordLeg.AddSpecWord(pModule->m_dwLegCfgWord);
	}
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		pNode->WindLoad.Set();
		pNode->AutoCalLoad.Set();
		pNode->TestWindLoad.Set();
		pNode->TestAutoCalLoad.Set();
	}
	CPreProcessor test_processor;
	CAnalyzeProcessor test_analysis;
	preprocessor.PrepareTower(&Ta,cfgwordBody,cfgwordLeg,true);
	test_processor.Test_PrepareTower(&Ta,cfgwordBody,cfgwordLeg,true);
	hits+=preprocessor.Test_EqualResult(test_processor,fp);
	preprocessor.UpdateInfoAtElemSizeChange();
	test_processor.Test_UpdateInfoAtElemSizeChange();
	hits+=preprocessor.Test_EqualResult(test_processor,fp);
	preprocessor.SetAnalysisProcessor(&analysis);
	test_processor.SetAnalysisProcessor(&test_analysis);

	ARRAY_LIST<WORD>legArr;
	ARRAY_LIST<CFGWORD>arrCalModule;
	int iCalUnit=0;
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		CFGWORD legword;	//暂用配材号代替存储各象位接腿号
		legArr.SetSize(0,10);
		arrCalModule.SetSize(0,10);
		//进行接腿轮换
		for(i=0;i<192;i++)
		{
			if(!pModule->m_dwLegCfgWord.IsHasNo(i+1))
				continue;
			legArr.append((WORD)i+1);
		}
		//进行高低腿轮换计算
		for(i=0;i<legArr.GetSize();i++)
		{
			legword.flag.word[0]=legArr[i];
			for(int j=0;j<legArr.GetSize();j++)
			{
				legword.flag.word[1]=legArr[j];
				for(int k=0;k<legArr.GetSize();k++)
				{
					legword.flag.word[2]=legArr[k];
					for(int m=0;m<legArr.GetSize();m++)
					{
						legword.flag.word[3]=legArr[m];
						arrCalModule.append(legword);
					}
				}
			}
		}
		for(i=0;i<arrCalModule.GetSize();i++)
		{
			WORD legQuadBak[4];
			memcpy(legQuadBak,pModule->m_arrActiveQuadLegNo,8);
			for(int iLeg=0;iLeg<4;iLeg++)
				pModule->m_arrActiveQuadLegNo[iLeg]=(WORD)arrCalModule[i].flag.word[iLeg];
			preprocessor.RetrieveFemAnalInfo(pModule);
			test_processor.RetrieveFemAnalInfo(pModule);
			//初次调用或计算模型中接腿发生变化时调用更新相关计算信息
			preprocessor.UpdateInfoAtLegChange(pModule);
			test_processor.UpdateInfoAtLegChange(pModule);
			for(CWorkStatus *pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
			{
				BOOL bPushStatus=Ta.WorkStatus.push_stack();
				Ta.m_hActiveStatus=pStatus->handle;
				//清空风压数据，重新计算
				for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
				{
					pNode->WindLoad.Set();
					pNode->AutoCalLoad.Set();
				}
				test_processor.Test_UpdateInfoAtStatusChange(pModule,pStatus,false);
				for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
				{
					pNode->TestWindLoad=pNode->WindLoad;
					pNode->TestAutoCalLoad=pNode->AutoCalLoad;
					pNode->WindLoad.Set();
					pNode->AutoCalLoad.Set();
				}
				//初次调用或工况发生变化时调用更新相关计算信息,fLoadStepCoef:线条荷载的加载系数，如0.7、1.0、1.1
				preprocessor.UpdateInfoAtStatusChange(pModule,pStatus,false);
				test_processor.Test_UpdateInfoAtStatusChange(pModule,pStatus,false);
				hits+=preprocessor.Test_EqualResult(test_processor,fp);
			}
			CXhChar100 prompt("已完成%5d#%2d号呼高%C%C-%C-%C腿的力学分析\n",iCalUnit,pModule->iNo,
				(arrCalModule[i].flag.word[0]-1)%8+'A',(arrCalModule[i].flag.word[1]-1)%8+'A',
				(arrCalModule[i].flag.word[2]-1)%8+'A',(arrCalModule[i].flag.word[3]-1)%8+'A');
			DisplayPrompt(prompt);
		}
	}
#endif
*/
	//计算风压段的承受风压荷载
	//void Test_CalWindAndAttachLoad(CTower *pTower,CLDSModule *pModule,CWorkStatus *pStatus,int iLoadExecuteCode,
	//	bool bForFoundation=false,double magnifyCoef=1.0,WINDSEGEXPORT_LIST* pExportList=NULL) ;*/
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	fclose(fp);
	if(hits>0)
		WinExec("notepad.exe d:\\test.txt",SW_SHOW);
	DisplayPrompt("");
#endif
}
int Test_ComparePartNoString(FILE* fp);
int Test_XhCharStrToKey(FILE* fp);
void TestV4_2()
{
	FILE* fp=fopen("d:\\test.txt","wt");
	int hits=Test_XhCharStrToKey(fp);
	fclose(fp);
	if(hits>0)
		WinExec("notepad.exe d:\\test.txt",SW_SHOW);
}
#ifdef __PART_DESIGN_INC_
#include "..\PostProcessReport.h"
//bool TestFemAndMemberCheckFile(char* filepath,CLogFile& rpt,CBuffer* pHeaderCopyBuf=NULL);
//void WriteFemAndMemberCheckTestInfo(CBuffer& buffer,char cOperType);
void TestFem()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(pCmdLine->m_bGettingStrFromCmdLine)
		pCmdLine->m_bForceExitCommand=TRUE;
	pCmdLine->FillCmdLine("杆塔设计验算测试文件<T.导入验算|E.导出验算测试文件|Y.导出优选测试文件>[T]:","");
	CString cmdStr="T";
	if(!pCmdLine->GetStrFromCmdLine(cmdStr))
	{
		pCmdLine->CancelCmdLine();
		return;
	}
	//pCmdLine->FillCmdLine("命令:","");
	CXhChar100 filepath;
	char cOperType='T';
	if(cmdStr.IsEmpty()||cmdStr.CompareNoCase("T")==0)
	{	//导入测试
		CFileDialog dlg(TRUE,"",filepath,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"杆塔设计验算测试文件(*.test)|*.test||");
		if(dlg.DoModal()!=IDOK)
			return;
		filepath.Printf("%s",dlg.GetPathName());
		CString name=dlg.GetFileName();
		CString title=dlg.GetFileTitle();
		theApp.GetLDSDoc()->SetPathName(dlg.GetPathName());
		theApp.GetLDSDoc()->SetTitle(title);
	}
	else
	{	//导出生成测试文件
		CLDSDoc *pDoc=theApp.GetLDSDoc();
		CString path=pDoc->GetPathName();
		path=path.Left(path.GetLength()-4);
		CFileDialog dlg(FALSE,"",path,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"杆塔设计验算测试文件(*.test)|*.test||");
		if(dlg.DoModal()!=IDOK)
			return;
		if(cmdStr.CompareNoCase("E")==0)
			cOperType='E';
		else
			cOperType='Y';
		filepath.Printf("%s",dlg.GetPathName());
	}
	if(cOperType=='E'||cOperType=='Y')
	{	//生成设计验算('E')或优化选材('Y')测试文件
		CBuffer buffer(10000000);
		//写入原始数据文件
		CString fvStr;
		theApp.GetFileVersion(fvStr);
		CXhChar16 file_version("%s",fvStr);
		buffer.WriteString("LDS_FEM_MEMCHK_TEST");
		buffer.WriteString(file_version);	//文件版本
		Ta.ToBuffer(buffer,DogSerialNo(),file_version);
		//进行设计验算或优化选材
		if(cOperType=='E')
			((CMainFrame*)theApp.m_pMainWnd)->ModelCheck();
		else
			((CMainFrame*)theApp.m_pMainWnd)->ModelOptimize();
		//写入测试内容及验证结果信息
		//WriteFemAndMemberCheckTestInfo(buffer,cOperType);
		FILE* data_fp = fopen(filepath,"wb");//cOperType=='E');
		fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,data_fp);
		fclose(data_fp);
	}
	else
	{
		CLogFile rpt("D:\\FemTestReport.txt");
		CBuffer buffer;
		//TestFemAndMemberCheckFile(filepath,rpt,&buffer);
		theApp.GetLDSDoc()->UpdateAllViews(NULL);
		//rpt.ShowToScreen();
		if(rpt.IsHasContents())	//未成功时提示是否重建测试文件副本
		{
			pCmdLine->FillCmdLine("是否重建杆塔设计验算测试文件副本<Y.是|N.否>[Y]:","");
			CString cmdStr="Y";
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
			{
				pCmdLine->CancelCmdLine();
				return;
			}
			if(cmdStr.GetLength()==0||cmdStr.CompareNoCase("Y")==0)
			{
				/*CLDSDoc *pDoc=theApp.GetLDSDoc();
				CString path=pDoc->GetPathName();
				path=path.Left(path.GetLength()-4);*/
				CFileDialog dlg(FALSE,"",filepath,
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					"杆塔设计验算测试文件(*.test)|*.test||");
				if(dlg.DoModal()!=IDOK)
					return;
				filepath.Printf("%s",dlg.GetPathName());
				FILE* data_fp = fopen(filepath,"wb");//cOperType=='E');
				postreport.ToBuffer(buffer);
				fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,data_fp);
				fclose(data_fp);
			}
		}
	}
}
#endif
void TestCfgLeg()
{
	CXhChar50 bodyStr;
	CXhChar16 legStr;
	CFGLEG legword=6;
	legword.GetBodyScopeStr(bodyStr);
	legword.GetLegScopeStr(legStr);
	if(!bodyStr.Equal(""))
		TRACE("6 bodyStr=%s\n",(char*)bodyStr);
	if(!legStr.Equal("B,C"))
		TRACE("6 legStr=%s\n",(char*)legStr);
	legword=8;
	legword.GetBodyScopeStr(bodyStr);
	legword.GetLegScopeStr(legStr);
	if(!bodyStr.Equal(""))
		TRACE("8 bodyStr=%s\n",(char*)bodyStr);
	if(!legStr.Equal("D"))
		TRACE("8 legStr=%s\n",(char*)legStr);
	legword=9;
	legword.GetBodyScopeStr(bodyStr);
	legword.GetLegScopeStr(legStr);
	if(!bodyStr.Equal(""))
		TRACE("9 bodyStr=%s\n",(char*)bodyStr);
	if(!legStr.Equal("A,D"))
		TRACE("9 legStr=%s\n",(char*)legStr);
	legword=0x01002f;
	legword.GetBodyScopeStr(bodyStr);
	legword.GetLegScopeStr(legStr);
	if(!bodyStr.Equal("9"))
		TRACE("0x01002f bodyStr=%s\n",(char*)bodyStr);
	if(!legStr.Equal("A-D,F"))
		TRACE("0x01000f legStr=%s\n",(char*)legStr);
	if(!legword.IsHasNo(9))
		TRACE("0x01000f IsHasBodyNo error\n");
	if(!legword.IsHasNo(9,2,true))
		TRACE("error\n");
	CFGLEG leg1=0x01000f;
	if(leg1.BodyFlag()!=0x0100)
		TRACE("0x01000f BodyFlag error\n");
	if(leg1.LegFlag()!=0x0f)
		TRACE("0x01000f BodyFlag error\n");
	CFGLEG leg;
	leg.SetBodyFlag(0x0100);
	leg.SetLegFlag(0x0f);
	if(leg!=leg1)
		TRACE("Set error\n");
}
#ifdef __PART_DESIGN_INC_
//from TdfFile.cpp
CFGWORD RetrieveCfgWordFromStr(char* cfg_str,bool bLegObj=false);
void Test_RetrieveCfgWordFromStr()
{
	CFGWORD cfgword;
	cfgword=RetrieveCfgWordFromStr("A-C",true);
	if(cfgword.ToCfgLeg().BodyFlag()==1&&cfgword.ToCfgLeg().LegFlag()!=7)
		TRACE("'A-C'接腿解析错误\n");
	cfgword=RetrieveCfgWordFromStr("A",true);
	if(!cfgword.IsEqual(CFGWORD(1,1)))
		TRACE("'A'接腿解析错误\n");
	cfgword=RetrieveCfgWordFromStr("A,D",true);
	if(cfgword.ToCfgLeg().BodyFlag()==1&&cfgword.ToCfgLeg().LegFlag()!=9)
		TRACE("'A-D'接腿解析错误\n");
	cfgword=RetrieveCfgWordFromStr("",true);
	if(!cfgword.IsNull())
		TRACE("'空'接腿解析错误\n");
	cfgword=RetrieveCfgWordFromStr("2B",true);
	if(!cfgword.IsEqual(CFGWORD(2,2)))
		TRACE("'2B'接腿解析错误\n");
	cfgword=RetrieveCfgWordFromStr("12A",true);
	if(!cfgword.IsEqual(CFGWORD(12,1)))
		TRACE("'12A'接腿解析错误\n");
	cfgword=RetrieveCfgWordFromStr("12:A-C",true);
	if(cfgword.ToCfgLeg().BodyFlag()==0x0800&&cfgword.ToCfgLeg().LegFlag()!=7)
		TRACE("'12:A-C'接腿解析错误\n");
}
#endif
//#include "matrix.h"
/*void CompareMat()
{
	EQUATION_MATRIX gauss,sub;
	FILE* fp=fopen("D:\\stiff_gauss.mat","rb");
	gauss.FromFile(fp);
	fclose(fp);
	fp=fopen("D:\\stiff_sub.mat","rb");
	sub.FromFile(fp);
	fclose(fp);
	CMatrix gaussmat(gauss.m_nStiffMatRowN,gauss.m_nStiffMatColN);
	memcpy(gaussmat.mat,gauss.mat,gauss.m_nStiffMatRowN*gauss.m_nStiffMatColN*8);
	CMatrix submat(sub.m_nStiffMatRowN,sub.m_nStiffMatColN);
	memcpy(submat.mat,sub.mat,sub.m_nStiffMatRowN*sub.m_nStiffMatColN*8);
	bool exi=false;
	CLogFile rpt("D:\\matReport.txt");
	//while(!exi)
	{
		int row=30;
		int col=50;
		int wide=100;
		for(int i=row;i<row+wide;i++)
		{
			for(int j=col;j<col+wide;j++)
			{
				if(gauss.freedom_label[i]!=sub.freedom_label[i])
					rpt.Log("%d行：guass{%d}-sub{%d}\n",i,gauss.freedom_label[i],sub.freedom_label[i]);
				if(gauss.freedom_label[j]!=sub.freedom_label[j])
					rpt.Log("%d列：guass{%d}-sub{%d}\n",j,gauss.freedom_label[j],sub.freedom_label[j]);
				if(fabs(gaussmat[i][j]-submat[i][j])>1)
					rpt.Log("%d-%d,差:%f\n",gauss.freedom_label[i],gauss.freedom_label[j],gaussmat[i][j]-submat[i][j]);
			}
		}
	}
	WinExec("notepad.exe D:\\matReport.txt",SW_SHOW);
}*/
void Test_EquidistantCornerOffset();
void Test_CutPlateBySector();
void ExportCutPlateInfo();
BOOL Test_CutPlateByPlate(FILE* fp);
void TestV4_3()
{
	FILE* fp=fopen("d:\\test.txt","wt");
	BOOL bRet=Test_CutPlateByPlate(fp);
	fclose(fp);
	if(!bRet)	
		WinExec("notepad.exe d:\\test.txt",SW_SHOW);
}
void Test_ColumnIntersPoint()
{
	logerr.ClearContents();
	Ta.Parts.Empty();
	Ta.Node.Empty();
	CLDSLineTube* pTube=(CLDSLineTube*)Ta.Parts.append(CLS_LINETUBE);
	SIZESPEC size;
	size.wide=160;
	size.thick=8;
	size.idClassType=CLS_LINETUBE;
	pTube->SetSizeSpec(size);
	pTube->SetStart(f3dPoint());
	pTube->SetEnd(f3dPoint(0,0,1000));
	pTube->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pTube->GetSolidPartObject());

	CLDSPoint point;
	point.datum_pos_style=11;	//柱面定位点
	point.des_para.COLUMN_INTERS.hLineTube=pTube->handle;
	//1.钢管柱面射线点
	point.des_para.COLUMN_INTERS.column_r=50;
	point.des_para.COLUMN_INTERS.AxisVector.axis_vec[0]=/*point.des_para.COLUMN_INTERS.axis_vec[2]=*/1.0;
	point.des_para.COLUMN_INTERS.SetSectCenter(500,0,500);
	//point.des_para.COLUMN_INTERS.sect_norm[0]=point.des_para.COLUMN_INTERS.sect_norm[2]=1.0;
	point.des_para.COLUMN_INTERS.ray_vec_style=2;
	point.des_para.COLUMN_INTERS.SetRayVector(1,0,0);//point.des_para.COLUMN_INTERS.ray_vec[1]=1;
	point.des_para.COLUMN_INTERS.bSpecRo=TRUE;
	point.des_para.COLUMN_INTERS.ray_offset_r=140;
	//point.des_para.COLUMN_INTERS.ray_para_offset=-30;
	point.UpdatePos(&Ta);
	CLDSNode* pNode;
	pNode=Ta.Node.append();
	pNode->SetPosition(point.des_para.COLUMN_INTERS.SectCenter.sect_center[0],
		point.des_para.COLUMN_INTERS.SectCenter.sect_center[1],point.des_para.COLUMN_INTERS.SectCenter.sect_center[2]);
	NewNode(pNode);

	pNode=Ta.Node.append();
	pNode->SetPosition(point.Position());
	NewNode(pNode);
	g_pSolidDraw->Draw();
	if(logerr.IsHasContents())
		logerr.ShowToScreen();
}
CXhChar100 PointToString(const f3dPoint &point,bool b3dPt/*=TRUE*/);
void CompareMultiOffsetPos(CLDSLineAngle *pLineAngle,bool bStart,CMultiOffsetPos *pSrcPos,CMultiOffsetPos *pDestPos)
{
	CString str;
	if(bStart)
		str.Format("0X%X角钢始端",pLineAngle->handle);
	else
		str.Format("0X%X角钢终端",pLineAngle->handle);
	if(pSrcPos->spatialOperationStyle  != pDestPos->spatialOperationStyle)
		logerr.Log("%s,spatialOperationStyle : %d->%d",str,pSrcPos->spatialOperationStyle,pDestPos->spatialOperationStyle);
	if(!pSrcPos->face_offset_norm.IsEqual(pDestPos->face_offset_norm.x,pDestPos->face_offset_norm.y,pDestPos->face_offset_norm.z))
	{
		CXhChar100 old_pnt_str=PointToString(pSrcPos->face_offset_norm,true);
		CXhChar100 new_pnt_str=PointToString(pDestPos->face_offset_norm,true);
		logerr.Log("%s,face_norm_offset改变%s->%s",str,(char*)old_pnt_str,(char*)new_pnt_str);
	}
	if(pSrcPos->cDatumAngleWing!=pDestPos->cDatumAngleWing)
		logerr.Log("%s,cDatumAngleWing : %c->%c",str,pSrcPos->cDatumAngleWing,pDestPos->cDatumAngleWing);
	if(pSrcPos->huoqu_pos_auto_0_x_wing1_y_wing2!=pDestPos->huoqu_pos_auto_0_x_wing1_y_wing2)
		logerr.Log("%s,huoqu_pos_auto_0_x_wing1_y_wing2 : %d->%d",str,pSrcPos->huoqu_pos_auto_0_x_wing1_y_wing2,pDestPos->huoqu_pos_auto_0_x_wing1_y_wing2);
	if(pSrcPos->cFaceOffsetDatumLine!=pDestPos->cFaceOffsetDatumLine)
	{
		char char1=pSrcPos->cFaceOffsetDatumLine==0?'0':pSrcPos->cFaceOffsetDatumLine;
		char char2=pDestPos->cFaceOffsetDatumLine==0?'0':pDestPos->cFaceOffsetDatumLine;
		logerr.Log("%s,cFaceOffsetDatumLine : %C->%C",str,char1,char2);
	}
	if(pSrcPos->coordCtrlType!=pDestPos->coordCtrlType)
		logerr.Log("%s,coordCtrlType : %C->%C",str,pSrcPos->coordCtrlType,pDestPos->coordCtrlType);
	if(pSrcPos->cMainCoordCtrlWing!=pDestPos->cMainCoordCtrlWing)
		logerr.Log("%s,cMainCoordCtrlWing : %C->%C",str,pSrcPos->cMainCoordCtrlWing,pDestPos->cMainCoordCtrlWing);
	if(pSrcPos->cWorkWingStyle!=pDestPos->cWorkWingStyle)
		logerr.Log("%s,cWorkWingStyle : %d->%d",str,pSrcPos->cWorkWingStyle,pDestPos->cWorkWingStyle);
	if(pSrcPos->origin_pos_style!=pDestPos->origin_pos_style)
		logerr.Log("%s,origin_pos_style ：%d -> %d",str,pSrcPos->origin_pos_style,pDestPos->origin_pos_style);
	if(pSrcPos->datumWingOffset.gStyle  != pDestPos->datumWingOffset.gStyle)
		logerr.Log("%s,datumWingOffset.gStyle : %d->%d",str,pSrcPos->datumWingOffset.gStyle,pDestPos->datumWingOffset.gStyle);
	if(pSrcPos->datumWingOffset.operStyle  != pDestPos->datumWingOffset.operStyle)
		logerr.Log("%s,datumWingOffset.operStyle : %d->%d",str,pSrcPos->datumWingOffset.operStyle,pDestPos->datumWingOffset.operStyle);
	if(pSrcPos->datumWingOffset.offsetDist  != pDestPos->datumWingOffset.offsetDist)
		logerr.Log("%s,datumWingOffset.offsetDist : %d->%d",str,pSrcPos->datumWingOffset.offsetDist,pDestPos->datumWingOffset.offsetDist);

	if(pSrcPos->ctrlWing.gStyle  != pDestPos->ctrlWing.gStyle)
		logerr.Log("%s,ctrlWing.gStyle : %d->%d",str,pSrcPos->ctrlWing.gStyle,pDestPos->ctrlWing.gStyle);
	if(pSrcPos->ctrlWing.operStyle  != pDestPos->ctrlWing.operStyle)
		logerr.Log("%s,ctrlWing.operStyle : %d->%d",str,pSrcPos->ctrlWing.operStyle,pDestPos->ctrlWing.operStyle);
	if(pSrcPos->ctrlWing.offsetDist  != pDestPos->ctrlWing.offsetDist)
		logerr.Log("%s,ctrlWing.offsetDist : %d->%d",str,pSrcPos->ctrlWing.offsetDist,pDestPos->ctrlWing.offsetDist);

	if(pSrcPos->otherWing.gStyle  != pDestPos->otherWing.gStyle)
		logerr.Log("%s,otherWing.gStyle : %d->%d",str,pSrcPos->otherWing.gStyle,pDestPos->otherWing.gStyle);
	if(pSrcPos->otherWing.operStyle  != pDestPos->otherWing.operStyle)
		logerr.Log("%s,otherWing.operStyle : %d->%d",str,pSrcPos->otherWing.operStyle,pDestPos->otherWing.operStyle);
	if(pSrcPos->otherWing.offsetDist  != pDestPos->otherWing.offsetDist)
		logerr.Log("%s,otherWing.offsetDist : %d->%d",str,pSrcPos->otherWing.offsetDist,pDestPos->otherWing.offsetDist);

	if(pSrcPos->wing_x_offset.gStyle  != pDestPos->wing_x_offset.gStyle)
		logerr.Log("%s,wing_x_offset.gStyle : %d->%d",str,pSrcPos->wing_x_offset.gStyle,pDestPos->wing_x_offset.gStyle);
	if(pSrcPos->wing_x_offset.operStyle  != pDestPos->wing_x_offset.operStyle)
		logerr.Log("%s,wing_x_offset.operStyle : %d->%d",str,pSrcPos->wing_x_offset.operStyle,pDestPos->wing_x_offset.operStyle);
	if(pSrcPos->wing_x_offset.offsetDist  != pDestPos->wing_x_offset.offsetDist)
		logerr.Log("%s,wing_x_offset.offsetDist : %d->%d",str,pSrcPos->wing_x_offset.offsetDist,pDestPos->wing_x_offset.offsetDist);

	if(pSrcPos->wing_y_offset.gStyle  != pDestPos->wing_y_offset.gStyle)
		logerr.Log("%s,wing_y_offset.gStyle : %d->%d",str,pSrcPos->wing_y_offset.gStyle,pDestPos->wing_y_offset.gStyle);
	if(pSrcPos->wing_y_offset.operStyle  != pDestPos->wing_y_offset.operStyle)
		logerr.Log("%s,wing_y_offset.operStyle : %d->%d",str,pSrcPos->wing_y_offset.operStyle,pDestPos->wing_y_offset.operStyle);
	if(pSrcPos->wing_y_offset.offsetDist  != pDestPos->wing_y_offset.offsetDist)
		logerr.Log("%s,wing_y_offset.offsetDist : %d->%d",str,pSrcPos->wing_y_offset.offsetDist,pDestPos->wing_y_offset.offsetDist);
}
void Test_SimpleAnglePos()
{
	CLogErrorLife life;
	long hb=0;
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		CLDSLineAngle* pAngle=pRod->IsAngle()?(CLDSLineAngle*)pRod:NULL;
		if(pAngle==NULL)
			continue;
		if(pRod->handle==0x8e||pRod->handle==hb)
			int b=3;
		CMultiOffsetPos desStartPos= pAngle->desStartPos;
		CMultiOffsetPos desEndPos  = pAngle->desEndPos;
		if(pAngle->desStartPos.jgber_cal_style==0)
		{
			pAngle->desStartPos.jgber_cal_style=2;
			if(!pAngle->IntelliUpdateSimplePosParam(TRUE))
				logerr.Log("0X%X角钢始端简单定位智能提取位置参数失败!\n",pAngle->handle);
			CompareMultiOffsetPos(pAngle,TRUE,&desStartPos,&pAngle->desStartPos);
			pAngle->desStartPos=desStartPos;
		}
		if(pAngle->desEndPos.jgber_cal_style==0)
		{
			pAngle->desEndPos.jgber_cal_style=2;
			if(!pAngle->IntelliUpdateSimplePosParam(FALSE))
				logerr.Log("0X%X角钢始端简单定位智能提取位置参数失败!\n",pAngle->handle);
			CompareMultiOffsetPos(pAngle,FALSE,&desEndPos,&pAngle->desEndPos);
			pAngle->desEndPos=desEndPos;
		}
	}
}
BOOL gbLegacyMode=FALSE;
BOOL gbLegacyAngleCtrlWingModeForTMAV5_1_2=FALSE;
struct RODPOSINFO{
	double length;
	GEPOINT start,end;
};
void RoutingDataCheck();
#include "SocketObject.h"
#include "../LocalFeatureDef.h"
#include "TowerType.h"
CHashStrList<PART> g_xPartHashList;
void ImportBomData()
{
	CFileDialog file_dlg(TRUE,"bom","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"统材信息(*.bom)|*.bom|所有文件(*.*)|*.*||");
	if(file_dlg.DoModal()!=IDOK)
		return;
	FILE *fp = fopen(file_dlg.GetPathName(),"rt");
	if(fp==NULL)
		return;
	g_xPartHashList.Empty();
	char line_text[200]="",sText[200]="";
	char key_word[100]="",type_word[100]="";
	CTowerType mat_tbl;
	bool body=false,part=false,bolt=false;
	while(!feof(fp))
	{
		fgets(line_text,200,fp);
		strcpy(sText,line_text);
		CString sLine=sText;
		sLine.Replace('\t',' ');
		sprintf(line_text,"%s",sLine);
		char *skey=strtok((char*)sText,"' ',\t");
		if(_stricmp(skey,"SECTION")==0)
		{
			sscanf(line_text,"%s%s",key_word,type_word);
			if(_stricmp(type_word,"BODY")==0)
			{
				body=true;part=false;bolt=false;
				continue;
			}
			else if(_stricmp(type_word,"PART")==0)
			{
				part=true;body=false;bolt=false;
				continue;
			}	
			else if(_stricmp(type_word,"BOLT")==0)
			{
				bolt=true;body=false;part=false;
				continue;
			}
		}
		if(body || bolt)
		{	//暂不获取呼高信息和螺栓信息
			continue;
		}
		else if(part)
		{	//获取构件信息
			int nPartNum=0;
			long iSeg=0;
			double length=0,singWeight=0,sumWeight=0;
			char sPartNo[16]="",sMaterial[20]="",sSpec[20]="",sNote[50]="";
			PART manuPart;
			sscanf(line_text,"%d%s%s%s%lf%d%lf%lf%s",&iSeg,sPartNo,sMaterial,sSpec,&length,&nPartNum,&singWeight,&sumWeight,sNote);	
			//
			PART* pPart=g_xPartHashList.Add(sPartNo);
			pPart->iSeg=iSeg;
			pPart->cMaterial=QuerySteelBriefMatMark(sMaterial);
			pPart->SetPartNo(sPartNo);
			pPart->sSpec.Copy(sSpec);
			strcpy(pPart->sNotes,sNote);
			pPart->AddPart(nPartNum);
			pPart->fPieceMinWeight=singWeight;
			CString str(sSpec);
			char mark;
			if(str.Find("Φ")>=0||str.Find("φ")>=0)	//不能使用spec[0]=='Φ'
			{	
				pPart->part_type=4;	//钢管
			}
			else if(sSpec[0] == 'L')
			{
				pPart->part_type=1;	//角钢
				sscanf(sSpec,"%c%lf%c%lf",&mark,&pPart->wide,&mark,&pPart->thick);
			}
			else if(sSpec[0] == '-')
			{
				pPart->part_type=3;	//钢板
				sscanf(sSpec,"%c%lf",&mark,&pPart->thick);
			}
			else if(sSpec[0]=='[')
				pPart->part_type=6;	//槽钢[
		}
	}
	fclose(fp);
}
//修复腿部V面大斜材为T型组合角钢时的摆放方式
void TestLegVGroupJG()
{
	int i=0,n=Ta.Node.GetNodeNum();
	for(CLDSNode* pNode=Ta.EnumNodeFirst();pNode;pNode=Ta.EnumNodeNext(),i++)
	{
		//腿部基准节点
		if(pNode->IsBodyObj())			
			continue;	//节点位于塔腿
		CLDSLinePart* pFatherPart=(CLDSLinePart*)Ta.FromPartHandle(pNode->hFatherPart);
		if(pFatherPart==NULL||pFatherPart->IsBodyObj() || pFatherPart->Layer(1)!='Z')
			continue;	//节点父杆件属于腿部主材
		//获取与节点连接的两V斜材
		LINEPARTSET linePartSet;
		for(CLDSLinePart* pLinePart=Ta.EnumRodFirst();pLinePart;pLinePart=Ta.EnumRodNext())
		{
			if(pLinePart->IsBodyObj() || pLinePart->Layer(1)!='X')
				continue;	//杆件属于腿部斜材
			if(pLinePart->pStart!=pNode && pLinePart->pEnd!=pNode)
				continue;	//斜材与节点相连
			if(pLinePart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
				continue;	//斜材组合角钢
			linePartSet.append(pLinePart);
		}
		if(linePartSet.GetNodeNum()!=2)
			continue;
		f3dPoint vec;
		UCS_STRU ucs;
		CLDSGroupLineAngle *pLeftAngle=NULL,*pRightAngle=NULL;
		for(CLDSLinePart* pRod=linePartSet.GetFirst();pRod;pRod=linePartSet.GetNext())
		{
			BOOL bStart=(pRod->pStart==pNode)?TRUE:FALSE;
			if(bStart)
				vec=(pRod->End()-pRod->Start()).normalized();
			else
				vec=(pRod->Start()-pRod->End()).normalized();
			if(ucs.axis_y.IsZero())
			{
				ucs.axis_y=vec;
				pLeftAngle=(CLDSGroupLineAngle*)pRod;
			}
			else
			{
				ucs.axis_z=ucs.axis_y^vec;
				if(ucs.axis_z.z<0)
				{
					pRightAngle=(CLDSGroupLineAngle*)pRod;
					ucs.axis_z*=-1;
				}
				else
				{
					pRightAngle=pLeftAngle;
					pLeftAngle=(CLDSGroupLineAngle*)pRod;
				}
				ucs.axis_y+=vec;
			}
		}
		CLDSLineAngle* pSonJg1=NULL,*pSonJg2=NULL;
		//左侧工作肢为Y肢，Y肢外贴
		pLeftAngle->group_style=1;
		pLeftAngle->size_cSubClassType='T';
		pLeftAngle->ciMasterIndex=0;
		pSonJg1=(CLDSLineAngle*)Ta.FromPartHandle(pLeftAngle->son_jg_h[0],CLS_LINEANGLE);
		pSonJg1->des_norm_x.bByOtherWing=pSonJg1->des_norm_y.bByOtherWing=0;
		pSonJg1->des_norm_x.hViceJg=pSonJg1->des_norm_y.hViceJg=0;
		pSonJg2=(CLDSLineAngle*)Ta.FromPartHandle(pLeftAngle->son_jg_h[1],CLS_LINEANGLE);
		pSonJg2->des_norm_x.bByOtherWing=pSonJg2->des_norm_y.bByOtherWing=0;
		pSonJg2->des_norm_x.hViceJg=pSonJg2->des_norm_y.hViceJg=0;
		pLeftAngle->cal_x_wing_norm();
		pLeftAngle->cal_y_wing_norm();
		pLeftAngle->CalPosition();
		pLeftAngle->UpdateSonJgBasicInfo();
		pLeftAngle->UpdateSonJgOddment();
		pLeftAngle->UpdateSonJgWingNorm();
		pLeftAngle->UpdateSonJgPos();
		//右侧工作肢为X肢，X肢外贴
		pRightAngle->group_style=1;
		pRightAngle->size_cSubClassType='T';
		pRightAngle->ciMasterIndex=1;
		pSonJg1=(CLDSLineAngle*)Ta.FromPartHandle(pRightAngle->son_jg_h[0],CLS_LINEANGLE);
		pSonJg1->des_norm_x.bByOtherWing=pSonJg1->des_norm_y.bByOtherWing=0;
		pSonJg1->des_norm_x.hViceJg=pSonJg1->des_norm_y.hViceJg=0;
		pSonJg2=(CLDSLineAngle*)Ta.FromPartHandle(pRightAngle->son_jg_h[1],CLS_LINEANGLE);
		pSonJg2->des_norm_x.bByOtherWing=pSonJg2->des_norm_y.bByOtherWing=0;
		pSonJg2->des_norm_x.hViceJg=pSonJg2->des_norm_y.hViceJg=0;
		pRightAngle->cal_x_wing_norm();
		pRightAngle->cal_y_wing_norm();
		pRightAngle->CalPosition();
		pRightAngle->UpdateSonJgBasicInfo();
		pRightAngle->UpdateSonJgOddment();
		pRightAngle->UpdateSonJgWingNorm();
		pRightAngle->UpdateSonJgPos();
	}
}
//导出LDS中腿部偏移节点的设计参数
#include "GlobalFunc.h"
void Test_ExportOffNodePara()
{
	CXhChar200 data_file=theApp.GetLDSDoc()->GetPathName();
	char* pszExtSpliter=SearchChar(data_file,'.',true);
	*pszExtSpliter=0;
	data_file.Append(".txt");
	FILE* fp=fopen(data_file,"wt");
	if(fp==NULL)
		return;
	for(CLDSNode* pNode=Ta.EnumNodeFirst();pNode;pNode=Ta.EnumNodeNext())
	{
		if(!pNode->IsLegObj())
			continue;
		CLDSNode* pNode1=Ta.FromNodeHandle(pNode->arrRelationNode[0]);
		CLDSNode* pNode2=Ta.FromNodeHandle(pNode->arrRelationNode[1]);
		if(pNode->m_cPosCalType==5 && pNode1 && pNode2)
			fprintf(fp,"%d,%d,%d,%f\n",pNode->pointI,pNode1->pointI,pNode2->pointI,pNode->attach_scale);
	}
	fclose(fp);
}
//导入偏移节点设计参数，修复当前模型的相关节点
struct OFF_NODE_PARA
{
	int m_iPoint;
	int m_iRelPoint1;
	int m_iRelPoint2;
	double m_fScale;
	OFF_NODE_PARA()
	{
		m_iPoint=0;
		m_iRelPoint1=0;
		m_iRelPoint2=0;
		m_fScale=0;
	}
};
void Test_ImportOffNodePara()
{
	CXhChar200 data_file=theApp.GetLDSDoc()->GetPathName();
	char* pszExtSpliter=SearchChar(data_file,'.',true);
	*pszExtSpliter=0;
	data_file.Append(".txt");
	FILE* fp=fopen(data_file,"rt");
	if(fp==NULL)
		return;
	ATOM_LIST<OFF_NODE_PARA> offNodeParaList;
	char sLine[MAX_PATH]="";
	while(!feof(fp))
	{
		if(fgets(sLine,MAX_PATH,fp)==NULL)
			break;
		if(strlen(sLine)<=0)
			continue;
		OFF_NODE_PARA* pNodePara=offNodeParaList.append();
		CXhChar100 line_txt(sLine);
		line_txt.Replace(',',' ');
		sscanf(line_txt,"%d%d%d%lf",&pNodePara->m_iPoint,&pNodePara->m_iRelPoint1,&pNodePara->m_iRelPoint2,&pNodePara->m_fScale);
	}
	fclose(fp);
	//修复当前模型的相关节点
	CHashSet<CLDSNode*> hashNodeByPointI;
	for(CLDSNode* pNode=Ta.EnumNodeFirst();pNode;pNode=Ta.EnumNodeNext())
	{
		if(pNode->pointI>0)
			hashNodeByPointI.SetValue(pNode->pointI,pNode);
	}
	CLDSNode* pNode=NULL,*pRelNode1=NULL,*pRelNode2=NULL;
	for(OFF_NODE_PARA* pOffNodePara=offNodeParaList.GetFirst();pOffNodePara;pOffNodePara=offNodeParaList.GetNext())
	{
		CLDSNode* pNode=hashNodeByPointI.GetValue(pOffNodePara->m_iPoint);
		CLDSNode* pRelNode1=hashNodeByPointI.GetValue(pOffNodePara->m_iRelPoint1);
		CLDSNode* pRelNode2=hashNodeByPointI.GetValue(pOffNodePara->m_iRelPoint2);
		if(pNode && pNode->m_cPosCalType==5 && pRelNode1 && pRelNode2)
		{
			pNode->arrRelationNode[0]=pRelNode1->handle;
			pNode->arrRelationNode[1]=pRelNode2->handle;
			pNode->attach_scale=pOffNodePara->m_fScale;
		}
	}
	//
}
void Test111()
{
	CLDSLineAngle* pJg=(CLDSLineAngle*)Ta.FromPartHandle(0X2389,CLS_LINEANGLE);
	if(pJg)
	{
		pJg->dfFilletROfSX=pJg->dfFilletROfSY=30;
		pJg->dfFilletROfEX=pJg->dfFilletROfEY=40;
		pJg->Create3dSolidModel();
	}
}
#include "LDSDoc.h"
#include "LDS.h"
#ifdef __GGG_
void TestDesignJG4()
{
	Ta.Empty();
	Ta.InitTower();
	console.InitTowerModel();
	console.DispNodeSet.Empty();
	console.DispPartSet.Empty();
	//添加呼高
	CLDSModule *pModule=NULL;
	Ta.Module.Empty();
	Ta.m_hActiveModule=0;
	CFGLEG::SetSchema(CFGLEG::MULTILEG_MAX08);
	CObjNoGroup *pGroup=Ta.NoManager.FromGroupID(MODEL_GROUP);
	pGroup->EmptyNo();
	//15m呼高
	pModule=Ta.Module.append();
	pModule->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pModule->iNo);
	strcpy(pModule->description,"15m");
	pModule->SetBodyNo(1);
	pModule->m_dwLegCfgWord.Clear();
	pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),0x000001);
	Ta.m_hActiveModule=pModule->handle;
	//18m呼高
	pModule=Ta.Module.append();
	pModule->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pModule->iNo);
	strcpy(pModule->description,"18m");
	pModule->SetBodyNo(2);
	pModule->m_dwLegCfgWord.Clear();
	pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),0x000001);
	//21m呼高
	pModule=Ta.Module.append();
	pModule->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pModule->iNo);
	strcpy(pModule->description,"21m");
	pModule->SetBodyNo(3);
	pModule->m_dwLegCfgWord.Clear();
	pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),0x000001);
	//24m呼高
	pModule=Ta.Module.append();
	pModule->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pModule->iNo);
	strcpy(pModule->description,"24m");
	pModule->SetBodyNo(4);
	pModule->m_dwLegCfgWord.Clear();
	pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),0x000001);
	//添加塔身节点和椎管
	CLDSConeTube *pConeTube=NULL;
	CFGWORD cfgword;
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		cfgword.AddBodyLegs(pModule->GetBodyNo());
	CLDSNode* pBodyNode1=console.AppendNode();
	pBodyNode1->SetPosition(0,0,0);
	pBodyNode1->layer(0)='S';
	pBodyNode1->cfgword=cfgword;
	CLDSNode* pBodyNode2=console.AppendNode();
	pBodyNode2->SetPosition(0,0,9000);
	pBodyNode2->layer(0)='S';
	pBodyNode2->cfgword=cfgword;
	CLDSNode* pBodyNode3=console.AppendNode();
	pBodyNode3->SetPosition(0,0,18000);
	pBodyNode3->layer(0)='S';
	pBodyNode3->cfgword=cfgword;
	//
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='S';
	pConeTube->pStart=pBodyNode1;
	pConeTube->pEnd=pBodyNode2;
	pConeTube->faceN=12;
	pConeTube->size_thick=8;
	pConeTube->size_wide=560*0.5;
	pConeTube->size_wideUpSide=785*0.5;
	pConeTube->cMaterial='H';	//Q345
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='S';
	pConeTube->pStart=pBodyNode2;
	pConeTube->pEnd=pBodyNode3;
	pConeTube->faceN=12;
	pConeTube->size_thick=12;
	pConeTube->size_wide=785*0.5;
	pConeTube->size_wideUpSide=1010*0.5;
	pConeTube->cMaterial='H';	//Q345
	pConeTube->cfgword=cfgword;
	//添加横担节点和椎管
	CLDSNode* pArmNodeL1=console.AppendNode();
	pArmNodeL1->SetPosition(-2680,0,0);
	pArmNodeL1->layer(0)='T';
	pArmNodeL1->cfgword=cfgword;
	CLDSNode* pArmNodeR1=console.AppendNode();
	pArmNodeR1->SetPosition(2680,0,0);
	pArmNodeR1->layer(0)='T';
	pArmNodeR1->cfgword=cfgword;
	CLDSNode* pSegNode2=console.AppendNode();
	pSegNode2->SetPosition(0,0,4000);
	pSegNode2->layer(0)='S';
	pSegNode2->cfgword=cfgword;
	CLDSNode* pArmNodeR2=console.AppendNode();
	pArmNodeR2->SetPosition(3530,0,4000);
	pArmNodeR2->layer(0)='T';
	pArmNodeR2->cfgword=cfgword;
	CLDSNode* pSegNode3=console.AppendNode();
	pSegNode3->SetPosition(0,0,8000);
	pSegNode3->layer(0)='S';
	pSegNode3->cfgword=cfgword;
	CLDSNode* pArmNodeL3=console.AppendNode();
	pArmNodeL3->SetPosition(-4080,0,8000);
	pArmNodeL3->layer(0)='T';
	pArmNodeL3->cfgword=cfgword;
	CLDSNode* pArmNodeR3=console.AppendNode();
	pArmNodeR3->SetPosition(4080,0,8000);
	pArmNodeR3->layer(0)='T';
	pArmNodeR3->cfgword=cfgword;
	//
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='T';
	pConeTube->pStart=pBodyNode1;
	pConeTube->pEnd=pArmNodeL1;
	pConeTube->faceN=12;
	pConeTube->size_thick=8;
	pConeTube->size_wide=150;
	pConeTube->size_wideUpSide=50;
	pConeTube->cMaterial='H';	//Q345
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='T';
	pConeTube->pStart=pBodyNode1;
	pConeTube->pEnd=pArmNodeR1;
	pConeTube->faceN=12;
	pConeTube->size_thick=8;
	pConeTube->size_wide=150;
	pConeTube->size_wideUpSide=50;
	pConeTube->cMaterial='H';	//Q345
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='T';
	pConeTube->pStart=pSegNode2;
	pConeTube->pEnd=pArmNodeR2;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=180;
	pConeTube->size_wideUpSide=150*0.5;
	pConeTube->cMaterial='H';	//Q345
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='T';
	pConeTube->pStart=pSegNode3;
	pConeTube->pEnd=pArmNodeL3;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=180;
	pConeTube->size_wideUpSide=150*0.5;
	pConeTube->cMaterial='H';	//Q345
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='T';
	pConeTube->pStart=pSegNode3;
	pConeTube->pEnd=pArmNodeR3;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=180;
	pConeTube->size_wideUpSide=150*0.5;
	pConeTube->cMaterial='H';	//Q345
	pConeTube->cfgword=cfgword;
	//添加15米呼高节点和杆件
	CLDSNode* pLegNode1=console.AppendNode();
	pLegNode1->SetPosition(0,0,23100);
	pLegNode1->layer(0)='L';
	pLegNode1->cfgword.Clear();
	pLegNode1->cfgword.AddBodyLegs(1,0x000001);
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='L';
	pConeTube->pStart=pBodyNode3;
	pConeTube->pEnd=pLegNode1;
	pConeTube->faceN=12;
	pConeTube->size_thick=14;
	pConeTube->size_wide=1010*0.5;
	pConeTube->size_wideUpSide=1138*0.5;
	pConeTube->cMaterial='H';	//Q345
	pConeTube->cfgword.Clear();
	pConeTube->cfgword.AddBodyLegs(1,0x000001);
	//添加18米呼高节点和杆件
	CLDSNode* pLegNode2=console.AppendNode();
	pLegNode2->SetPosition(0,0,26100);
	pLegNode2->layer(0)='L';
	pLegNode2->cfgword.Clear();
	pLegNode2->cfgword.AddBodyLegs(2,0x000001);
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='L';
	pConeTube->pStart=pBodyNode3;
	pConeTube->pEnd=pLegNode2;
	pConeTube->faceN=12;
	pConeTube->size_thick=14;
	pConeTube->size_wide=1010*0.5;
	pConeTube->size_wideUpSide=1213*0.5;
	pConeTube->cMaterial='H';	//Q345
	pConeTube->cfgword.Clear();
	pConeTube->cfgword.AddBodyLegs(2,0x000001);
	//添加21米呼高节点和杆件
	CLDSNode* pLegNode3=console.AppendNode();
	pLegNode3->SetPosition(0,0,29100);
	pLegNode3->layer(0)='L';
	pLegNode3->cfgword.Clear();
	pLegNode3->cfgword.AddBodyLegs(3,0x000001);
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='L';
	pConeTube->pStart=pBodyNode3;
	pConeTube->pEnd=pLegNode3;
	pConeTube->faceN=12;
	pConeTube->size_thick=14;
	pConeTube->size_wide=1010*0.5;
	pConeTube->size_wideUpSide=1288*0.5;
	pConeTube->cMaterial='H';	//Q345
	pConeTube->cfgword.Clear();
	pConeTube->cfgword.AddBodyLegs(3,0x000001);
	//添加24米呼高节点和杆件
	CLDSNode* pLegNode4=console.AppendNode();
	pLegNode4->SetPosition(0,0,27000);
	pLegNode4->layer(0)='S';
	pLegNode4->cfgword.Clear();
	pLegNode4->cfgword.AddBodyLegs(4);
	CLDSNode* pLegNode5=console.AppendNode();
	pLegNode5->SetPosition(0,0,32100);
	pLegNode5->layer(0)='L';
	pLegNode5->cfgword.Clear();
	pLegNode5->cfgword.AddBodyLegs(4,0x000001);
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='S';
	pConeTube->pStart=pBodyNode3;
	pConeTube->pEnd=pLegNode4;
	pConeTube->faceN=12;
	pConeTube->size_thick=14;
	pConeTube->size_wide=1010*0.5;
	pConeTube->size_wideUpSide=1235*0.5;
	pConeTube->cMaterial='H';	//Q345
	pConeTube->cfgword.Clear();
	pConeTube->cfgword.AddBodyLegs(4);
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='L';
	pConeTube->pStart=pLegNode4;
	pConeTube->pEnd=pLegNode5;
	pConeTube->faceN=12;
	pConeTube->size_thick=14;
	pConeTube->size_wide=1235*0.5;
	pConeTube->size_wideUpSide=1363*0.5;
	pConeTube->cMaterial='H';	//Q345
	pConeTube->cfgword.Clear();
	pConeTube->cfgword.AddBodyLegs(4,0x000001);
	//
	for(CLDSNode* pNode=console.EnumNodeFirst();pNode;pNode=console.EnumNodeNext())
		NewNode(pNode);
	for(CLDSLinePart* pRod=console.EnumRodFirst();pRod;pRod=console.EnumRodNext())
		NewLinePart(pRod);
	//更新视图
	theApp.GetLDSDoc()->UpdateAllViews(NULL);

}
void TestDesignGSP90()
{
	Ta.Empty();
	Ta.InitTower();
	console.InitTowerModel();
	console.DispNodeSet.Empty();
	console.DispPartSet.Empty();
	//添加呼高
	CFGLEG::SetSchema(CFGLEG::MULTILEG_MAX08);
	CObjNoGroup *pGroup=Ta.NoManager.FromGroupID(MODEL_GROUP);
	pGroup->EmptyNo();
	Ta.Module.Empty();
	Ta.m_hActiveModule=0;
	CLDSModule *pModule=NULL;
	//9m呼高
	pModule=Ta.Module.append();
	pModule->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pModule->iNo);
	strcpy(pModule->description,"9m");
	pModule->SetBodyNo(1);
	pModule->m_dwLegCfgWord.Clear();
	pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),0x000001);
	Ta.m_hActiveModule=pModule->handle;
	//10m呼高
	pModule=Ta.Module.append();
	pModule->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pModule->iNo);
	strcpy(pModule->description,"10m");
	pModule->SetBodyNo(2);
	pModule->m_dwLegCfgWord.Clear();
	pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),0x000001);
	//12m呼高
	pModule=Ta.Module.append();
	pModule->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pModule->iNo);
	strcpy(pModule->description,"12m");
	pModule->SetBodyNo(3);
	pModule->m_dwLegCfgWord.Clear();
	pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),0x000001);
	//21m呼高
	pModule=Ta.Module.append();
	pModule->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pModule->iNo);
	strcpy(pModule->description,"21m");
	pModule->SetBodyNo(4);
	pModule->m_dwLegCfgWord.Clear();
	pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),0x000001);
	//添加身部节点+椎管
	CFGWORD cfgword;
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		cfgword.AddBodyLegs(pModule->GetBodyNo());
	CLDSNode* pBodyNodeS=console.AppendNode();
	pBodyNodeS->SetPosition(0,0,0);
	pBodyNodeS->layer(0)='S';
	pBodyNodeS->cfgword=cfgword;
	CLDSNode* pBodyNodeE=console.AppendNode();
	pBodyNodeE->SetPosition(0,0,4000);
	pBodyNodeE->layer(0)='S';
	pBodyNodeE->cfgword=cfgword;
	CLDSConeTube *pConeTube=NULL;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='S';
	pConeTube->pStart=pBodyNodeS;
	pConeTube->pEnd=pBodyNodeE;
	pConeTube->faceN=12;
	pConeTube->size_thick=8;
	pConeTube->size_wide=280;
	pConeTube->size_wideUpSide=320;
	pConeTube->cfgword=cfgword;
	//添加腿部
	CLDSNode* pLegNode1=console.AppendNode();
	pLegNode1->SetPosition(0,0,13000);
	pLegNode1->layer(0)='L';
	pLegNode1->cfgword.Clear();
	pLegNode1->cfgword.AddBodyLegs(1,0x000001);
	CLDSNode* pLegNode2=console.AppendNode();
	pLegNode2->SetPosition(0,0,14000);
	pLegNode2->layer(0)='L';
	pLegNode2->cfgword.Clear();
	pLegNode2->cfgword.AddBodyLegs(2,0x000001);
	CLDSNode* pLegNode3=console.AppendNode();
	pLegNode3->SetPosition(0,0,16000);
	pLegNode3->layer(0)='L';
	pLegNode3->cfgword.Clear();
	pLegNode3->cfgword.AddBodyLegs(3,0x000001);
	CLDSNode* pLegNode4=console.AppendNode();
	pLegNode4->SetPosition(0,0,25000);
	pLegNode4->layer(0)='L';
	pLegNode4->cfgword.Clear();
	pLegNode4->cfgword.AddBodyLegs(4,0x000001);
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='L';
	pConeTube->pStart=pBodyNodeE;
	pConeTube->pEnd=pLegNode1;
	pConeTube->faceN=12;
	pConeTube->size_thick=8;
	pConeTube->size_wide=320;
	pConeTube->size_wideUpSide=410;
	pConeTube->cfgword.Clear();
	pConeTube->cfgword.AddBodyLegs(1,0x000001);
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='L';
	pConeTube->pStart=pBodyNodeE;
	pConeTube->pEnd=pLegNode2;
	pConeTube->faceN=12;
	pConeTube->size_thick=8;
	pConeTube->size_wide=320;
	pConeTube->size_wideUpSide=420;
	pConeTube->cfgword.Clear();
	pConeTube->cfgword.AddBodyLegs(2,0x000001);
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='L';
	pConeTube->pStart=pBodyNodeE;
	pConeTube->pEnd=pLegNode3;
	pConeTube->faceN=12;
	pConeTube->size_thick=8;
	pConeTube->size_wide=320;
	pConeTube->size_wideUpSide=440;
	pConeTube->cfgword.Clear();
	pConeTube->cfgword.AddBodyLegs(3,0x000001);
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='L';
	pConeTube->pStart=pBodyNodeE;
	pConeTube->pEnd=pLegNode4;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=320;
	pConeTube->size_wideUpSide=530;
	pConeTube->cfgword.Clear();
	pConeTube->cfgword.AddBodyLegs(4,0x000001);
	//添加横担
	CLDSNode* pArmNodeL1=console.AppendNode();
	pArmNodeL1->SetPosition(-2280,0,0);
	pArmNodeL1->layer(0)='T';
	pArmNodeL1->cfgword=cfgword;
	CLDSNode* pArmNodeR1=console.AppendNode();
	pArmNodeR1->SetPosition(2280,0,0);
	pArmNodeR1->layer(0)='T';
	pArmNodeR1->cfgword=cfgword;
	CLDSNode* pArmNodeL2=console.AppendNode();
	pArmNodeL2->SetPosition(-3850,0,4000);
	pArmNodeL2->layer(0)='T';
	pArmNodeL2->cfgword=cfgword;
	CLDSNode* pArmNodeR2=console.AppendNode();
	pArmNodeR2->SetPosition(3850,0,4000);
	pArmNodeR2->layer(0)='T';
	pArmNodeR2->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pBodyNodeS;
	pConeTube->pEnd=pArmNodeL1;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pBodyNodeS;
	pConeTube->pEnd=pArmNodeR1;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pBodyNodeE;
	pConeTube->pEnd=pArmNodeL2;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pBodyNodeE;
	pConeTube->pEnd=pArmNodeR2;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	//
	for(CLDSNode* pNode=console.EnumNodeFirst();pNode;pNode=console.EnumNodeNext())
		NewNode(pNode);
	for(CLDSLinePart* pRod=console.EnumRodFirst();pRod;pRod=console.EnumRodNext())
		NewLinePart(pRod);
	//更新视图
	theApp.GetLDSDoc()->UpdateAllViews(NULL);
}
void TestDesignSGDJ90()
{
	Ta.Empty();
	Ta.InitTower();
	console.InitTowerModel();
	console.DispNodeSet.Empty();
	console.DispPartSet.Empty();
	//添加呼高
	CFGLEG::SetSchema(CFGLEG::MULTILEG_MAX08);
	CObjNoGroup *pGroup=Ta.NoManager.FromGroupID(MODEL_GROUP);
	pGroup->EmptyNo();
	Ta.Module.Empty();
	Ta.m_hActiveModule=0;
	CLDSModule *pModule=NULL;
	//15m呼高
	pModule=Ta.Module.append();
	pModule->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pModule->iNo);
	strcpy(pModule->description,"15m");
	pModule->SetBodyNo(1);
	pModule->m_dwLegCfgWord.Clear();
	pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),0x000001);
	Ta.m_hActiveModule=pModule->handle;
	//18m呼高
	pModule=Ta.Module.append();
	pModule->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pModule->iNo);
	strcpy(pModule->description,"18m");
	pModule->SetBodyNo(2);
	pModule->m_dwLegCfgWord.Clear();
	pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),0x000001);
	//21m呼高
	pModule=Ta.Module.append();
	pModule->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pModule->iNo);
	strcpy(pModule->description,"21m");
	pModule->SetBodyNo(3);
	pModule->m_dwLegCfgWord.Clear();
	pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),0x000001);
	//24m呼高
	pModule=Ta.Module.append();
	pModule->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pModule->iNo);
	strcpy(pModule->description,"24m");
	pModule->SetBodyNo(4);
	pModule->m_dwLegCfgWord.Clear();
	pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),0x000001);
	//添加身部节点+椎管
	CFGWORD cfgword;
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		cfgword.AddBodyLegs(pModule->GetBodyNo());
	CLDSNode* pBodyNodeS=console.AppendNode();
	pBodyNodeS->SetPosition(0,0,0);
	pBodyNodeS->layer(0)='S';
	pBodyNodeS->cfgword=cfgword;
	CLDSNode* pBodyNodeE=console.AppendNode();
	pBodyNodeE->SetPosition(0,0,13000);
	pBodyNodeE->layer(0)='S';
	pBodyNodeE->cfgword=cfgword;
	CLDSConeTube *pConeTube=NULL;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='S';
	pConeTube->pStart=pBodyNodeS;
	pConeTube->pEnd=pBodyNodeE;
	pConeTube->faceN=12;
	pConeTube->size_thick=8;
	pConeTube->size_wide=280;
	pConeTube->size_wideUpSide=380;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	//添加腿部
	CLDSNode* pLegNode1=console.AppendNode();
	pLegNode1->SetPosition(0,0,27000);
	pLegNode1->layer(0)='L';
	pLegNode1->cfgword.Clear();
	pLegNode1->cfgword.AddBodyLegs(1,0x000001);
	CLDSNode* pLegNode2=console.AppendNode();
	pLegNode2->SetPosition(0,0,30000);
	pLegNode2->layer(0)='L';
	pLegNode2->cfgword.Clear();
	pLegNode2->cfgword.AddBodyLegs(2,0x000001);
	CLDSNode* pLegNode3=console.AppendNode();
	pLegNode3->SetPosition(0,0,33000);
	pLegNode3->layer(0)='L';
	pLegNode3->cfgword.Clear();
	pLegNode3->cfgword.AddBodyLegs(3,0x000001);
	CLDSNode* pLegNode4=console.AppendNode();
	pLegNode4->SetPosition(0,0,36000);
	pLegNode4->layer(0)='L';
	pLegNode4->cfgword.Clear();
	pLegNode4->cfgword.AddBodyLegs(4,0x000001);
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='L';
	pConeTube->pStart=pBodyNodeE;
	pConeTube->pEnd=pLegNode1;
	pConeTube->faceN=12;
	pConeTube->size_thick=8;
	pConeTube->size_wide=380;
	pConeTube->size_wideUpSide=488;
	pConeTube->cfgword.Clear();
	pConeTube->cfgword.AddBodyLegs(1,0x000001);
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='L';
	pConeTube->pStart=pBodyNodeE;
	pConeTube->pEnd=pLegNode2;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=380;
	pConeTube->size_wideUpSide=510;
	pConeTube->cfgword.Clear();
	pConeTube->cfgword.AddBodyLegs(2,0x000001);
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='L';
	pConeTube->pStart=pBodyNodeE;
	pConeTube->pEnd=pLegNode3;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=380;
	pConeTube->size_wideUpSide=534;
	pConeTube->cfgword.Clear();
	pConeTube->cfgword.AddBodyLegs(3,0x000001);
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='L';
	pConeTube->pStart=pBodyNodeE;
	pConeTube->pEnd=pLegNode4;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=380;
	pConeTube->size_wideUpSide=556;
	pConeTube->cfgword.Clear();
	pConeTube->cfgword.AddBodyLegs(4,0x000001);
	//添加横担
	CLDSNode* pArmNodeL1=console.AppendNode();
	pArmNodeL1->SetPosition(-3280,0,0);
	pArmNodeL1->layer(0)='T';
	pArmNodeL1->cfgword=cfgword;
	CLDSNode* pArmNodeR1=console.AppendNode();
	pArmNodeR1->SetPosition(3280,0,0);
	pArmNodeR1->layer(0)='T';
	pArmNodeR1->cfgword=cfgword;
	CLDSNode* pSegNode2=console.AppendNode();
	pSegNode2->SetPosition(0,0,4000);
	pSegNode2->layer(0)='S';
	pSegNode2->cfgword=cfgword;
	CLDSNode* pArmNodeL2=console.AppendNode();
	pArmNodeL2->SetPosition(-3820,0,4000);
	pArmNodeL2->layer(0)='T';
	pArmNodeL2->cfgword=cfgword;
	CLDSNode* pArmNodeR2=console.AppendNode();
	pArmNodeR2->SetPosition(3820,0,4000);
	pArmNodeR2->layer(0)='T';
	pArmNodeR2->cfgword=cfgword;
	CLDSNode* pSegNode3=console.AppendNode();
	pSegNode3->SetPosition(0,0,8000);
	pSegNode3->layer(0)='S';
	pSegNode3->cfgword=cfgword;
	CLDSNode* pArmNodeL3=console.AppendNode();
	pArmNodeL3->SetPosition(-4360,0,8000);
	pArmNodeL3->layer(0)='T';
	pArmNodeL3->cfgword=cfgword;
	CLDSNode* pArmNodeR3=console.AppendNode();
	pArmNodeR3->SetPosition(4360,0,8000);
	pArmNodeR3->layer(0)='T';
	pArmNodeR3->cfgword=cfgword;
	CLDSNode* pSegNode4=console.AppendNode();
	pSegNode4->SetPosition(0,0,12000);
	pSegNode4->layer(0)='S';
	pSegNode4->cfgword=cfgword;
	CLDSNode* pArmNodeL4=console.AppendNode();
	pArmNodeL4->SetPosition(-3900,0,12000);
	pArmNodeL4->layer(0)='T';
	pArmNodeL4->cfgword=cfgword;
	CLDSNode* pArmNodeR4=console.AppendNode();
	pArmNodeR4->SetPosition(3900,0,12000);
	pArmNodeR4->layer(0)='T';
	pArmNodeR4->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pBodyNodeS;
	pConeTube->pEnd=pArmNodeL1;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pBodyNodeS;
	pConeTube->pEnd=pArmNodeR1;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pSegNode2;
	pConeTube->pEnd=pArmNodeL2;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pSegNode2;
	pConeTube->pEnd=pArmNodeR2;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pSegNode3;
	pConeTube->pEnd=pArmNodeL3;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pSegNode3;
	pConeTube->pEnd=pArmNodeR3;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pSegNode4;
	pConeTube->pEnd=pArmNodeL4;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pSegNode4;
	pConeTube->pEnd=pArmNodeR4;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	//
	for(CLDSNode* pNode=console.EnumNodeFirst();pNode;pNode=console.EnumNodeNext())
		NewNode(pNode);
	for(CLDSLinePart* pRod=console.EnumRodFirst();pRod;pRod=console.EnumRodNext())
		NewLinePart(pRod);
	//更新视图
	theApp.GetLDSDoc()->UpdateAllViews(NULL);
}
void TestDesignGJ90()
{
	Ta.Empty();
	Ta.InitTower();
	console.InitTowerModel();
	console.DispNodeSet.Empty();
	console.DispPartSet.Empty();
	//添加呼高
	CFGLEG::SetSchema(CFGLEG::MULTILEG_MAX08);
	CObjNoGroup *pGroup=Ta.NoManager.FromGroupID(MODEL_GROUP);
	pGroup->EmptyNo();
	Ta.Module.Empty();
	Ta.m_hActiveModule=0;
	CLDSModule *pModule=NULL;
	//9m呼高
	pModule=Ta.Module.append();
	pModule->iNo=pGroup->EnumIdleNo();
	pGroup->SetNoState(pModule->iNo);
	strcpy(pModule->description,"21m");
	pModule->SetBodyNo(1);
	pModule->m_dwLegCfgWord.Clear();
	pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),0x000001);
	Ta.m_hActiveModule=pModule->handle;
	//添加身部节点+椎管
	CFGWORD cfgword;
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		cfgword.AddBodyLegs(pModule->GetBodyNo());
	CLDSNode* pBodyNodeS=console.AppendNode();
	pBodyNodeS->SetPosition(0,0,0);
	pBodyNodeS->layer(0)='S';
	pBodyNodeS->cfgword=cfgword;
	CLDSNode* pBodyNodeE=console.AppendNode();
	pBodyNodeE->SetPosition(0,0,8000);
	pBodyNodeE->layer(0)='S';
	pBodyNodeE->cfgword=cfgword;
	CLDSConeTube *pConeTube=NULL;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='S';
	pConeTube->pStart=pBodyNodeS;
	pConeTube->pEnd=pBodyNodeE;
	pConeTube->faceN=12;
	pConeTube->size_thick=8;
	pConeTube->size_wide=280;
	pConeTube->size_wideUpSide=360;
	pConeTube->cfgword=cfgword;
	//添加腿部
	CLDSNode* pLegNode1=console.AppendNode();
	pLegNode1->SetPosition(0,0,28500);
	pLegNode1->layer(0)='L';
	pLegNode1->cfgword.Clear();
	pLegNode1->cfgword.AddBodyLegs(1,0x000001);
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->layer(0)='L';
	pConeTube->pStart=pBodyNodeE;
	pConeTube->pEnd=pLegNode1;
	pConeTube->faceN=12;
	pConeTube->size_thick=8;
	pConeTube->size_wide=360;
	pConeTube->size_wideUpSide=565;
	pConeTube->cMaterial='H';
	pConeTube->cfgword.Clear();
	pConeTube->cfgword.AddBodyLegs(1,0x000001);
	//添加横担
	CLDSNode* pArmNodeL1=console.AppendNode();
	pArmNodeL1->SetPosition(-2280,0,0);
	pArmNodeL1->layer(0)='T';
	pArmNodeL1->cfgword=cfgword;
	CLDSNode* pArmNodeR1=console.AppendNode();
	pArmNodeR1->SetPosition(2280,0,0);
	pArmNodeR1->layer(0)='T';
	pArmNodeR1->cfgword=cfgword;
	CLDSNode* pSegNode2=console.AppendNode();
	pSegNode2->SetPosition(0,0,2000);
	pSegNode2->layer(0)='S';
	pSegNode2->cfgword=cfgword;
	CLDSNode* pArmNodeR2=console.AppendNode();
	pArmNodeR2->SetPosition(2850,0,2000);
	pArmNodeR2->layer(0)='T';
	pArmNodeR2->cfgword=cfgword;
	CLDSNode* pSegNode3=console.AppendNode();
	pSegNode3->SetPosition(0,0,7500);
	pSegNode3->layer(0)='S';
	pSegNode3->cfgword=cfgword;
	CLDSNode* pArmNodeL3=console.AppendNode();
	pArmNodeL3->SetPosition(-3855,0,7500);
	pArmNodeL3->layer(0)='T';
	pArmNodeL3->cfgword=cfgword;
	CLDSNode* pArmNodeR3=console.AppendNode();
	pArmNodeR3->SetPosition(3855,0,7500);
	pArmNodeR3->layer(0)='T';
	pArmNodeR3->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pBodyNodeS;
	pConeTube->pEnd=pArmNodeL1;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pBodyNodeS;
	pConeTube->pEnd=pArmNodeR1;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pSegNode2;
	pConeTube->pEnd=pArmNodeR2;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pSegNode3;
	pConeTube->pEnd=pArmNodeL3;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	pConeTube=(CLDSConeTube*)console.AppendPart(CLS_CONETUBE);
	pConeTube->SetLayer("TP1");
	pConeTube->pStart=pSegNode3;
	pConeTube->pEnd=pArmNodeR3;
	pConeTube->faceN=12;
	pConeTube->size_thick=10;
	pConeTube->size_wide=360*0.5;
	pConeTube->size_wideUpSide=160*0.5;
	pConeTube->cMaterial='H';
	pConeTube->cfgword=cfgword;
	//
	for(CLDSNode* pNode=console.EnumNodeFirst();pNode;pNode=console.EnumNodeNext())
		NewNode(pNode);
	for(CLDSLinePart* pRod=console.EnumRodFirst();pRod;pRod=console.EnumRodNext())
		NewLinePart(pRod);
	//更新视图
	theApp.GetLDSDoc()->UpdateAllViews(NULL);
}
#endif
#ifdef __LDS_
#include "DesignMultiLeg.h"
extern DWORD _LocalPlateHashFunc(const char *key, DWORD nHashSize);
extern bool  _LocalPlateKeyMatchFunc(const char* key, const char* szPartialMatchWithKey);
extern void ExecuteEmendBoltL0Para(DWORD dwParam = 0);
void TestMultiLegConnPoint()
{
	//CUndoOperObject undo(&Ta, true);
	//CStackVariant<char> stackprop0(&CLDSBolt::BOLTL0_CALMODE, CLDSBolt::L0CAL_BY_SUMM_THICK);
	//CIntelliModelDesigner intelli;
	//intelli.RetrieveModelLegacyConnections(&Ta);
	//intelli.m_bEnablePromptUI = false;
	//intelli.hashPlates.SetHashFunc(_LocalPlateHashFunc);
	//intelli.hashPlates.SetKeyMatchFunc(_LocalPlateKeyMatchFunc);
	//CTowerMultiLegDesigner designer;
	//designer.DesignTrunkPostShiftJoints(&Ta, &intelli);
	////
	//for (CLDSBolt* pBolt = (CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT); pBolt; pBolt = (CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
	//	pBolt->CalGuigeAuto();
}
#endif
int CLDSView::Test()
{
	//TestDesignGSP90();
	//TestDesignGJ90();
	//TestDesignSGDJ90();
	return 0;
	//
	POLYGON rgn;
	GEPOINT pick(19404.018936412751,9567.1892202950512,0.00000000000000000);
	f3dPoint vertarr[4];
	vertarr[0]=GEPOINT(25.000000000000071,25.000000000001510,0.00000000000000000);
	vertarr[1]=GEPOINT(-24.999999999999929,25.000000000001510,0.00000000000000000);
	vertarr[2]=GEPOINT(-24.999999999999929,-24.999999999998490,0.00000000000000000);
	vertarr[3]=GEPOINT(25.000000000000071,-24.999999999998490,0.00000000000000000);
	rgn.CreatePolygonRgn(vertarr,4);
	int retcode=rgn.PtInRgn2(pick);

	CLDSLinePart* pRod=Ta.FromRodHandle(0x44e);
	GEPOINT start=pRod->Start();
	long findH=0x19e;
	CLDSLineAngle* pTestAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(findH);
	CLDSLinePart::ENDBOLTS endbolts;
	pTestAngle->GetEffectiveEndBolts(&endbolts,false);
	for(CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pAngle;pAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
	{
		if(pAngle->pStart==NULL||pAngle->pEnd==NULL)
			continue;
		char layer[4]={0};
		StrCopy(layer,pAngle->layer(),4);
		if(layer[0]=='S'&&layer[1]=='X')
		{
			if( (pAngle->pStart->Position('X')*pAngle->pEnd->Position('X')<0)||
				(pAngle->pStart->Position('Y')*pAngle->pEnd->Position('Y')<0))
				pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_INVERT;	//塔身交叉斜材
			else if(fabs(pAngle->pStart->Position('X',false))<EPS||fabs(pAngle->pStart->Position('Y',false))<EPS||
				fabs(pAngle->pEnd->Position('X',false))<EPS||fabs(pAngle->pEnd->Position('Y',false))<EPS)
				pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
		}
		else if(layer[0]=='L'&&layer[1]=='X')
			pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;	//腿部斜材
		else if(layer[0]=='T'&&(layer[1]=='X'||layer[1]=='F'))
		{
			if( (pAngle->pStart->Position('X')*pAngle->pEnd->Position('X')<0)||
				(pAngle->pStart->Position('Y')*pAngle->pEnd->Position('Y')<0))
				pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_INVERT;	//头部交叉斜材
			else
				pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;	//头部斜材
		}
		else if(layer[1]=='Z')	//主材一律为X/Y对称里外同朝向
			pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
		else if(layer[0]=='S'&&layer[1]=='F')
			pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_INVERT;	//塔身辅材
		else if(layer[0]=='L'&&layer[1]=='F')
			pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;	//腿部辅材

	}
	if(!ValidateLocalizeFeature(FEATURE::BATCH_INTELLI_PRINT_PROCESSCARD))
		return 0;
	char ss10[10];
	CXhString ss(ss10,10);
	int lenmax=ss.LengthMax;
	ss.Copy("13432503444444444444444444340569");
	double f=1.3432503444444;
	CXhString ss2(ss10,10,"%.1f#12",f);
	ss2.AppendFormat("%d%Cd",67,'e');
	int len=ss2.Length;
	/*int len=ss.Length;*/
	//Test_SimpleAnglePos();
	/*CLogErrorLife life;
	CLDSPart* pPart;
	CHashList<RODPOSINFO> hashAngles;
	gbLegacyMode=TRUE;
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->IsAngle())
		{
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pPart;
			RODPOSINFO* pLine=hashAngles.Add(pAngle->handle);
			pAngle->CalPosition(true);
			pLine->length=pAngle->GetLength();
			pLine->start=pAngle->Start();
			pLine->end=pAngle->End();
		}
	}
	gbLegacyMode=FALSE;
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->handle==0xa7)
			int b=3;
		if(pPart->IsAngle())
		{
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pPart;
			pAngle->CalPosition(true);
			GEPOINT lineStart=pAngle->Start();
			GEPOINT lineEnd=pAngle->End();
			RODPOSINFO *pLine=hashAngles.GetValue(pPart->handle);
			double differenceStart=DISTANCE(lineStart,pLine->start);
			double differenceEnd  =DISTANCE(lineEnd,pLine->end);
			double diffLength=pAngle->GetLength()-pLine->length;
			if(pLine&&differenceStart>0.5)
				logerr.Log("0X%X角钢始端楞点位置计算发生变化difference=%.1f",pPart->handle,differenceStart);
			if(pLine&&differenceEnd>0.5)
				logerr.Log("0X%X角钢终端楞点位置计算发生变化difference=%.1f",pPart->handle,differenceEnd);
			if(pLine&&fabs(diffLength)>0.5)
				logerr.Log("0X%X角钢长度计算发生变化difference=%.1f",pPart->handle,diffLength);
		}
		//else if(pPart->GetClassTypeId()==CLS_BOLT)
		//{
		//	CLDSBolt* pBolt=(CLDSBolt*)pPart;
		//	GEPOINT pos=pBolt->ucs.origin;
		//	pBolt->correct_pos();
		//	double difference=DISTANCE(pos,pBolt->ucs.origin);
		//	if(difference>0.5)
		//		logerr.Log("0x%X螺栓位置计算发生变化difference=%.1f",pBolt->handle,difference);
		//}
	}*/
	/*for(CLDSBolt* pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
	{
		if(pBolt->handle==0xf6a)
			int b=3;
		if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)
		{
			gbLegacyIntersMode=TRUE;
			pBolt->des_base_pos.datumPoint.UpdatePos(&Ta);
			GEPOINT oldpos=pBolt->des_base_pos.datumPoint.Position();
			gbLegacyIntersMode=FALSE;
			pBolt->des_base_pos.datumPoint.UpdatePos(&Ta);
			GEPOINT newpos=pBolt->des_base_pos.datumPoint.Position();
			double dist=DISTANCE(oldpos,newpos);
			if(dist>0.1)
				logerr.Log("0x%x螺栓基点（心线交叉）计算出现版本偏差%f!",pBolt->handle,dist);
		}
	}
	gbLegacyIntersMode=FALSE;*/
	//Test_CutPlateBySector();
	//return 0;
	//Test_EquidistantCornerOffset();
	//CompareMat();
	//Test_RetrieveCfgWordFromStr();
	//TestV4_2();
	//Test_ColumnIntersPoint();

	//TestFem();
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	return 0;
}
#ifdef __PART_DESIGN_INC_
void Test_CalKlamdaCore();
//中国规范
double CalStableCoef1(double lamda,char sectionType,char cMaterial);
//美国导则
double CalStableCoef2(double lamda,double E,double fy);
void Test_CompStableCoef()
{
	double coef1,coef2;
	logerr.ClearContents();
	for(double  lamda=1;lamda<250;lamda+=0.5)
	{
		//Q235
		//DL/T 5154-2002
		coef1=CalStableCoef1(lamda,'A',CSteelMatLibrary::Q235BriefMark());
		coef2=IMemberDesignCode::CODE_DLT_2002()->CompStableCoef('A',lamda,CSteelMatLibrary::Q235Material(),10);
        if(fabs(coef1-coef2)>0.001)
			logerr.Log("fai=%.1f @A-DL/T-2002 error=%.3f\n",lamda,coef1-coef2);
		coef1=CalStableCoef1(lamda,'B',CSteelMatLibrary::Q235BriefMark());
		coef2=IMemberDesignCode::CODE_DLT_2002()->CompStableCoef('B',lamda,CSteelMatLibrary::Q235Material(),10);
        if(fabs(coef1-coef2)>0.001)
			logerr.Log("fai=%.1f @B-DL/T-2002 error=%.3f\n",lamda,coef1-coef2);
		//DL/T 5154-2012
		coef1=CalStableCoef1(lamda,'A',CSteelMatLibrary::Q235BriefMark());
		coef2=IMemberDesignCode::CODE_DLT_2012()->CompStableCoef('A',lamda,CSteelMatLibrary::Q235Material(),10);
        if(fabs(coef1-coef2)>0.001)
			logerr.Log("fai=%.1f @A-DL/T-201(0)2 error=%.3f\n",lamda,coef1-coef2);
		coef1=CalStableCoef1(lamda,'B',CSteelMatLibrary::Q235BriefMark());
		coef2=IMemberDesignCode::CODE_DLT_2012()->CompStableCoef('B',lamda,CSteelMatLibrary::Q235Material(),10);
        if(fabs(coef1-coef2)>0.001)
			logerr.Log("fai=%.1f @B-DL/T-201(0)2 error=%.3f\n",lamda,coef1-coef2);
		//ASCE 10-97
		coef1=CalStableCoef2(lamda,CSteelMatLibrary::Q235Material()->E,CSteelMatLibrary::Q235Material()->fy(10));
		coef2=IMemberDesignCode::CODE_ASCE10_97()->CompStableCoef('A',lamda,CSteelMatLibrary::Q235Material(),10);
        if(fabs(coef1-coef2)>0.001)
			logerr.Log("fai=%.1f @A-ASCE-10_97 error=%.3f\n",lamda,coef1-coef2);
		coef2=IMemberDesignCode::CODE_ASCE10_97()->CompStableCoef('B',lamda,CSteelMatLibrary::Q235Material(),10);
        if(fabs(coef1-coef2)>0.001)
			logerr.Log("fai=%.1f @B-ASCE-10_97 error=%.3f\n",lamda,coef1-coef2);
		//Q345
		//DL/T 5154-2002
		coef1=CalStableCoef1(lamda,'A',CSteelMatLibrary::Q345BriefMark());
		coef2=IMemberDesignCode::CODE_DLT_2002()->CompStableCoef('A',lamda,CSteelMatLibrary::Q345Material(),10);
        if(fabs(coef1-coef2)>0.001)
			logerr.Log("Q345fai=%.1f @A-DL/T-2002 error=%.3f\n",lamda,coef1-coef2);
		coef1=CalStableCoef1(lamda,'B',CSteelMatLibrary::Q345BriefMark());
		coef2=IMemberDesignCode::CODE_DLT_2002()->CompStableCoef('B',lamda,CSteelMatLibrary::Q345Material(),10);
        if(fabs(coef1-coef2)>0.001)
			logerr.Log("Q345fai=%.1f @B-DL/T-2002 error=%.3f\n",lamda,coef1-coef2);
		//DL/T 5154-2012
		coef1=CalStableCoef1(lamda,'A',CSteelMatLibrary::Q345BriefMark());
		coef2=IMemberDesignCode::CODE_DLT_2012()->CompStableCoef('A',lamda,CSteelMatLibrary::Q345Material(),10);
        if(fabs(coef1-coef2)>0.001)
			logerr.Log("Q345fai=%.1f @A-DL/T-201(0)2 error=%.3f\n",lamda,coef1-coef2);
		coef1=CalStableCoef1(lamda,'B',CSteelMatLibrary::Q345BriefMark());
		coef2=IMemberDesignCode::CODE_DLT_2012()->CompStableCoef('B',lamda,CSteelMatLibrary::Q345Material(),10);
        if(fabs(coef1-coef2)>0.001)
			logerr.Log("Q345fai=%.1f @B-DL/T-201(0)2 error=%.3f\n",lamda,coef1-coef2);
		//ASCE 10-97
		coef1=CalStableCoef2(lamda,CSteelMatLibrary::Q345Material()->E,CSteelMatLibrary::Q345Material()->fy(10));
		coef2=IMemberDesignCode::CODE_ASCE10_97()->CompStableCoef('A',lamda,CSteelMatLibrary::Q345Material(),10);
        if(fabs(coef1-coef2)>0.001)
			logerr.Log("Q345fai=%.1f @A-ASCE-10_97 error=%.3f\n",lamda,coef1-coef2);
		coef2=IMemberDesignCode::CODE_ASCE10_97()->CompStableCoef('B',lamda,CSteelMatLibrary::Q345Material(),10);
        if(fabs(coef1-coef2)>0.001)
			logerr.Log("Q345fai=%.1f @B-ASCE-10_97 error=%.3f\n",lamda,coef1-coef2);
	}
	logerr.ShowToScreen();
}
#endif
void modifyDoublePtr(double* pPointer)
{
	pPointer[0]=5;
	pPointer[1]=6;
	pPointer[2]=7;
}
void TestPtPtr(f3dPoint* pPt)
{
	modifyDoublePtr(*pPt);
}
#include "TimerCount.h"
#include "GlobalFunc.h"
extern CTimerCount timer;
COLORREF CalEfficColor(double efficCoef);
bool CalRodForceByBanlance(GEPOINT inputV,double* v1,double* v2);
void ExportPropListMapFile();	//from LDS.cpp
struct PARTDRAW{
	BYTE flag;
	CLDSPart* pPart;
	GEPOINT drawPlaneNorm;
	static const char ANGLE_X_WING = 'X';
	//Angle()
};
#include "DxfFile.h"
void TestMirRodAlgorithm()
{
	CLDSNode rodStart,rodEnd,mirRodStart,mirRodEnd;
	CLDSLinePart rod,mirRod;
	rod.pStart=&rodStart;
	rod.pEnd=&rodEnd;
	mirRod.pStart=&mirRodStart;
	mirRod.pEnd=&mirRodEnd;
	rodStart.SetPosition(100,0,-500);
	rodEnd.SetPosition(100,0, 500);
	mirRodStart.SetPosition(-100,0,-500);
	mirRodEnd.SetPosition(-100,0, 500);
	int mirrodstart_arr[]={-850,-850,-850,-850,-850,-850,-850,-500,-500,-500,-500,-400,-400,-400,-400,   0,  0,300,300,600};
	int mirrodend_arr[]  ={-550,-500,-300,   0, 400, 500, 950,-300,   0, 400, 500,-300,   0, 400, 500, 400,500,500,800,800};
	int pick_coordzarr[] ={-850,-550,-500,-400,-300,0,300,400,500,600,800};
	int vertex_n=sizeof(mirrodstart_arr)/4;	//19
	int pick_n=sizeof(pick_coordzarr)/4;	//11
	CDxfFile dxf;
	dxf.OpenFile("d:\\testMirRod.dxf");
	f3dPoint offset;
	for(int iStart=0;iStart<vertex_n;iStart++)
	{
		mirRodStart.SetPositionZ(mirrodstart_arr[iStart]);
		for(int iEnd=0;iEnd<vertex_n;iEnd++)
		{
			mirRodEnd.SetPositionZ(mirrodend_arr[iEnd]);
			for(int iPick=0;iPick<pick_n;iPick++)
			{
				f3dPoint pick(100,0,pick_coordzarr[iPick]);
				BYTE rslt=rod.MirSimilarGradeWith(&mirRod,MIRMSG(2),pick);
				offset.Set(400*iEnd,iStart*18000+iPick*1500);
				dxf.NewLine(f3dPoint(rod.pStart->Position('X',false),rod.pStart->Position('Z',false))+offset,f3dPoint(rod.pEnd->Position('X',false),rod.pEnd->Position('Z',false))+offset);
				dxf.NewLine(f3dPoint(mirRodStart.Position('X',false),mirRodStart.Position('Z',false))+offset,f3dPoint(mirRodEnd.Position('X',false),mirRodEnd.Position('Z',false))+offset);
				dxf.NewCircle(f3dPoint(pick.x,pick.z)+offset,20);
				if(rslt<=1)
					dxf.NewCircle(offset,50);
				else if(rslt==2)
					dxf.NewArc(offset.x,offset.y,50,0,180);
				else if(rslt==3)
					dxf.NewArc(offset.x,offset.y,50,180,360);
				else//无对称
					dxf.NewLine(offset+f3dPoint(0,-50,0),offset+f3dPoint(0,50,0));
			}
		}
	}
	dxf.CloseFile();
}

struct TESTPROP{
private:
	GEPOINT c;
public:
	GEPOINT& getC(){return c;}
	void setC(GEPOINT& value){
		c=value+1;
	}
	__declspec( property(put=setC,get=getC)) GEPOINT& C;
};
union POINTSTYLE{
	WORD wPointMode;
	struct{
		BYTE cbPointSize;
		BYTE ciPointStyle;
	};
};
//#include "FootDesignTemplDlg.h"
#include "Expression.h"
#include "SquareTree.h"
void Test_ParsePartNo();
//time_t NetTime();
void TestPortal()
{
	TESTPROP pp;
	pp.C.x=1;
	pp.C.y=2;
	GEPOINT t=pp.C;
	//time_t t=NetTime();
	//CTime now=t;
	//int day=now.GetDay();
	//int hour=now.GetHour();
	//int minnute=now.GetMinute();
	//int zone=_timezone;
	//now=tnow;
	//CString nowstr=now.FormatGmt("%A, %B %d, %Y %H:%M:%S");
	//logerr.EnableTimestamp();
	//logerr.Log("test timestamp");
	//GUID guid;
	//HRESULT hslt=CoCreateGuid(&guid);
	//CComBSTR bstrGuid(guid);
	//CString strGuid = bstrGuid;
	//logerr.Log(strGuid);
	//CXhChar50 bstr;
	//bstr.Printf("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",guid.Data1,guid.Data2 ,guid.Data3 ,
	//	guid.Data4[0],guid.Data4[1],guid.Data4[2],guid.Data4[3],guid.Data4[4],guid.Data4[5],guid.Data4[6],guid.Data4[7]);
	//logerr.Log(bstr);
	//logerr.ShowToScreen();
	//HKEY hKey=NULL,hSoftwareKey=NULL;
	//RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Xerofox\\TMA",0,KEY_WRITE,&hKey);
	//RegSetValueEx(hKey,"",NULL,REG_BINARY,(BYTE*)&guid,16);
	//theApp.WriteProfileBinary("","",(LPBYTE)&guid,16);
	//DUALKEY dual;
	//dual.lowId=3;
	//dual=234;
	//Test_ParsePartNo();
	/*POLYGONEX region;
	f3dPoint vertexes[4];
	vertexes[0].Set(22754.529384383328,1696.6985939097797,0.00000000000000000);
	vertexes[1].Set(22598.529384383328,1696.6985939163094,0.00000000000000000);
	vertexes[2].Set(22598.529384383313,925.69859392042690,0.00000000000000000);
	vertexes[3].Set(22754.529384383310,925.69859391390185,0.00000000000000000);
	region.CreatePolygonRgn(vertexes,4);
	GEPOINT	vr[4];
	vr[0].Set(22754.029384383317,1696.1985939097901);
	vr[1].Set(22599.029384383317,1696.1985939162989);
	vr[2].Set(22599.029384383324,926.19859392041644);
	vr[3].Set(22754.029384383321,926.19859391391230);
	int rslt[4];
	rslt[0]=region.LineInRgn(vr[0],vr[1]);
	rslt[1]=region.LineInRgn(vr[1],vr[2]);
	rslt[2]=region.LineInRgn(vr[2],vr[3]);
	rslt[3]=region.LineInRgn(vr[3],vr[0]);*/


	POINTSTYLE style;
	style.cbPointSize=4;
	union TEST64{
		__int64 id64;
		struct{DWORD d1,d2;};
	};
	//dual=DUALKEY(344,32,true);
	TEST64 tt;
	tt.d1=tt.d2=0xffffffff;
	/*timer.Start();
	for(int i=0;i<10000000;i++)
		DWORD m1=tt.d1%3017;
	timer.Relay(1);
	for(int i=0;i<10000000;i++)
		DWORD m1=tt.d1+3017;
	timer.Relay(2);
	for(int i=0;i<10000000;i++)
		DWORD m1=tt.id64%3017;
	timer.Relay(3);
	for(int i=0;i<10000000;i++)
		DWORD m1=tt.d1/3017;
	timer.Relay(4);
	timer.End();
	DWORD ticks1=timer.hashTicks[1];
	DWORD ticks2=timer.hashTicks[2];
	DWORD ticks3=timer.hashTicks[3];
	DWORD ticks4=timer.hashTicks[4];
	DWORD ticks=0;*/
	/*CExpression expr;
	EXPRESSION_VAR* pVar,*pIndexVar;
	pVar=expr.varList.append();
	strcpy(pVar->variableStr,"A");/\/\
	pVar->fValue=3.5;
	pIndexVar=expr.varList.append();
	strcpy(pIndexVar->variableStr,"i");
	pIndexVar->fValue=0;
	pVar=expr.varList.append();
	strcpy(pVar->variableStr,"N");
	pVar->fValue=2;
	pVar=expr.varList.append();
	strcpy(pVar->variableStr,"DX_ARR");
	double dxarr[9]={2.5,3.4,5.6,3.1,8.0,9.2,10.3,93.1,7.5};
	pVar->arrValues=dxarr;
	pVar->uiUpperBound=8;
	try{
		CLogErrorLife life(&logerr);
	for(int i=0;i<=4;i++)
	{
		pIndexVar->fValue=i;
		double val=expr.SolveExpression("A+DX_ARR[i*N+1]+N");
		logerr.Log("3.5+DX_ARR[%d*2+1]+2=%f",i,val);
	}
	}catch( char* error)
	{
		AfxMessageBox(error);
	}*/
	/*int i;
	CMatrix mat(18,18);
	mat.Resize(12,12);
	for(i=0;i<12;i++)
	{
		for(int j=0;j<12;j++)
			mat(i,j)=i*100+j;
	}
	FILE* fp=fopen("d:\\original.txt","wt");
	CXhChar500 linetxt; 
	for(i=0;i<12;i++)
	{
		linetxt.Empty();
		for(int j=0;j<12;j++)
			linetxt.Append(CXhChar16("%4.0f ",mat(i,j)));
		fprintf(fp,"%s\n",(char*)linetxt);
	}
	fclose(fp);
	fp=fopen("d:\\add.txt","wt");
	mat.Resize(18,18);
	for(i=0;i<18;i++)
	{
		linetxt.Empty();
		for(int j=0;j<18;j++)
			linetxt.Append(CXhChar16("%4.0f ",mat(i,j)));
		fprintf(fp,"%s\n",(char*)linetxt);
	}
	fclose(fp);
	fp=fopen("d:\\deduct.txt","wt");
	for(i=0;i<18;i++)
	{
		for(int j=0;j<18;j++)
			mat(i,j)=i*100+j;
	}
	mat.Resize(6,6);
	for(i=0;i<6;i++)
	{
		linetxt.Empty();
		for(int j=0;j<6;j++)
			linetxt.Append(CXhChar16("%4.0f ",mat(i,j)));
		fprintf(fp,"%s\n",(char*)linetxt);
	}
	fclose(fp);*/
	/*FILE* fp=fopen("d:\\1.elm","rt");
	int i;
	CMatrix mat(12,12);
	for(i=0;i<12;i++)
	{
		CXhChar200 lineTxt;
		fgets(lineTxt,200,fp);
		int j=0;
		char* key=strtok(lineTxt,",");
		for(j=0;key!=NULL;j++)
		{
			mat.row_header[i][j]=atof(key);
			key=strtok(NULL,",");
		}
		//lineTxt.Replace(',',' ');
		//sscanf(lineTxt,"%lf%lf%lf%lf%lf%lf",&mat.row_header[i][0],&mat.row_header[i][1],&mat.row_header[i][2],&mat.row_header[i][3],&mat.row_header[i][4],&mat.row_header[i][5]);
	}
	fclose(fp);
	CMatrix reduct=mat;
	reduct.Reduction(4,2);
	fp=fopen("d:\\1_reduct.txt","wt");
	for(i=0;i<12;i++)
		fprintf(fp,"%s\n",(char*)CXhChar200("%lf,%lf,%lf,%lf,%lf,%lf",reduct.row_header[i][0],reduct.row_header[i][1],reduct.row_header[i][2],reduct.row_header[i][3],reduct.row_header[i][4],reduct.row_header[i][5]));
	fclose(fp);
	reduct=mat;
	reduct.Reduction(5,1);
	fp=fopen("d:\\2_reduct.txt","wt");
	for(i=0;i<12;i++)
		fprintf(fp,"%s\n",(char*)CXhChar200("%lf,%lf,%lf,%lf,%lf,%lf",reduct.row_header[i][0],reduct.row_header[i][1],reduct.row_header[i][2],reduct.row_header[i][3],reduct.row_header[i][4],reduct.row_header[i][5]));
	fclose(fp);
	reduct=mat;
	reduct.Reduction(10,2);
	fp=fopen("d:\\3_reduct.txt","wt");
	for(i=0;i<12;i++)
		fprintf(fp,"%s\n",(char*)CXhChar200("%lf,%lf,%lf,%lf,%lf,%lf",reduct.row_header[i][0],reduct.row_header[i][1],reduct.row_header[i][2],reduct.row_header[i][3],reduct.row_header[i][4],reduct.row_header[i][5]));
	fclose(fp);
	reduct=mat;
	reduct.Reduction(11,1);
	fp=fopen("d:\\4_reduct.txt","wt");
	for(i=0;i<12;i++)
		fprintf(fp,"%s\n",(char*)CXhChar200("%lf,%lf,%lf,%lf,%lf,%lf",reduct.row_header[i][0],reduct.row_header[i][1],reduct.row_header[i][2],reduct.row_header[i][3],reduct.row_header[i][4],reduct.row_header[i][5]));
	fclose(fp);

	CMatrix mat(10,10);
	int i,j;
	for(i=0;i<10;i++)
		for(j=0;j<10;j++)
			mat(i,j)=i*10+j;
	mat.ToResidueMat(1,0,0,3);
	for(i=0;i<mat.GetRowsNum();i++)
	{
		for(j=0;j<mat.GetColumnsNum();j++)
			TRACE("%f ",mat(i,j));
		TRACE("\n");
	}*/
	//int size=sizeof(FOOTTEMPL_FLAG);
	//size=sizeof(ANGLE_SPACE_FLAG);
	MIRMSG mir;
	mir.axis_flag=8;
	mir.mir_norm.Set(1,0,0);
	f3dPoint pos=mir.MirPoint(GEPOINT(100,100,0));
	TestCfgLeg();
	char sPartNo[6]="FL9A";//"A235";
	WORD uiSegSerial=0;
	char prefix[2]="",suffix[2]="",sSerialNo[3]="",sSerialSuffix[2]="";
	SEGI segkey;
	ParsePartNo(sPartNo,&segkey,sSerialNo);
	CLogErrorLife loglife;
	f3dLine line1,line2;
	f3dPoint inters;
	line1.startPt.Set(0,0,500);
	line1.endPt.Set(1000,-1000,0);
	line2.startPt.Set(-900,-1000);
	line2.endPt.Set(1100,1000);
	int ret=Int3dDifferPlaneLL(line1.startPt,line1.endPt,line2.startPt,line2.endPt,inters,'Z');
	/*HIBERARCHY hiber;
	int seed_arr[17]={0x00,0x01,0x08,0x10,0x80,0x0100,0x0800,0x1000,0x8000,0x010000,0x080000,0x100000,0x800000,0x01000000,0x08000000,0x10000000,0x32000000};
	for(int up1=0;up1<17;up1++)
	{
		for(int up2=0;up2<17;up2++)
		{
			for(int down1=0;down1<17;down1++)
			{
				for(int down2=0;down2<17;down2++)
				{
					if(!hiber.SetHiberarchy(seed_arr[up1],seed_arr[up2],seed_arr[down1],seed_arr[down2]))
					{
						logerr.Log("0x%x-%x-O-%x-%x超出预定层级深度",seed_arr[up2],seed_arr[up1],seed_arr[down1],seed_arr[down2]);
						continue;
					}
					if(hiber.HiberUpId(1)!=seed_arr[up1])
						logerr.Log("0x%x-%x-O-%x-%x的+1级设定有误%d",seed_arr[up2],seed_arr[up1],seed_arr[down1],seed_arr[down2],seed_arr[up1]);
					if(hiber.HiberUpId(2)!=seed_arr[up2])
						logerr.Log("0x%x-%x-O-%x-%x的+2级设定有误%d",seed_arr[up2],seed_arr[up1],seed_arr[down1],seed_arr[down2],seed_arr[up2]);
					if(hiber.HiberDownId(1)!=seed_arr[down1])
						logerr.Log("0x%x-%x-O-%x-%x的-1级设定有误%d",seed_arr[up2],seed_arr[up1],seed_arr[down1],seed_arr[down2],seed_arr[down1]);
					if(hiber.HiberDownId(2)!=seed_arr[down2])
						logerr.Log("0x%x-%x-O-%x-%x的-2级设定有误%d",seed_arr[up2],seed_arr[up1],seed_arr[down1],seed_arr[down2],seed_arr[down2]);
					HIBERARCHY bkhiber=hiber;
					_int64 bkint64,i64;
					memcpy(&bkint64,&bkhiber,8);

					hiber.ModifyUpId(1,seed_arr[up1]);
					memcpy(&i64,&hiber,8);
					if(bkint64!=i64)
						logerr.Log("0x%x-%x-O-%x-%x的+1级设定有误",seed_arr[up2],seed_arr[up1],seed_arr[down1],seed_arr[down2]);

					hiber.ModifyUpId(2,seed_arr[up2]);
					memcpy(&i64,&hiber,8);
					if(bkint64!=i64)
						logerr.Log("0x%x-%x-O-%x-%x的+2级设定有误",seed_arr[up2],seed_arr[up1],seed_arr[down1],seed_arr[down2]);

					hiber.ModifyDownId(1,seed_arr[down1]);
					memcpy(&i64,&hiber,8);
					if(bkint64!=i64)
						logerr.Log("0x%x-%x-O-%x-%x的-1级设定有误",seed_arr[up2],seed_arr[up1],seed_arr[down1],seed_arr[down2]);

					hiber.ModifyDownId(2,seed_arr[down2]);
					memcpy(&i64,&hiber,8);
					if(bkint64!=i64)
						logerr.Log("0x%x-%x-O-%x-%x的-2级设定有误",seed_arr[up2],seed_arr[up1],seed_arr[down1],seed_arr[down2]);
				}
			}
		}
	}
	/*SUBID plexid5;
	SUBID16 plexid16;
	for(int group=0;group<18;group++)
	{
		for(int id=0;id<50;id++)
		{
			CXhChar16 idstr;
			if(group<16)
			{
				plexid5.Set(group,id);
				if(plexid5.Group()!=group||plexid5.Id()!=id)
					logerr.Log("SUBID %d-%d复合Id有误!",group,id);
				if(group>0)
					idstr.Printf("%d-%d",group,id);
				else
					idstr.Printf("%d",id);
				if(!idstr.Equal(plexid5.ToString()))
					logerr.Log("SUBID %d-%d复合Id生成字符串有误{%s}!",group,id,(char*)plexid5.ToString());
				plexid5.FromString(idstr);
				if(plexid5.Group()!=group||plexid5.Id()!=id)
					logerr.Log("SUBID %d-%d读取字符串有误!",group,id);
			}
			plexid16.Set(group,id);
			if(plexid16.Group()!=group||plexid16.Id()!=id)
				logerr.Log("SUBID16 %d-%d复合Id有误!",group,id);
			if(group>0)
				idstr.Printf("%d-%d",group,id);
			else
				idstr.Printf("%d",id);
			if(!idstr.Equal(plexid16.ToString()))
				logerr.Log("SUBID16 %d-%d复合Id生成字符串有误{%s}!",group,id,(char*)plexid16.ToString());
			plexid16.FromString(idstr);
			if(plexid16.Group()!=group||plexid16.Id()!=id)
				logerr.Log("SUBID16 %d-%d读取字符串有误!",group,id);
		}
	}
	/*GEPOINT inputV(12,34,0),v1(10,10),v2(567,432);
	RotateVectorAroundVector(inputV,0.5,GEPOINT(1,1,0));
	RotateVectorAroundVector(v1,0.5,GEPOINT(1,1,0));
	RotateVectorAroundVector(v2,0.5,GEPOINT(1,1,0));
	CalRodForceByBanlance(inputV,v1,v2);
	logerr.ClearContents();
	logerr.Log("sum(x)=%f",inputV.x+v1.x+v2.x);
	logerr.Log("sum(y)=%f",inputV.y+v1.y+v2.y);*/
	/*double effic=0.0;
	logerr.ClearContents();
	while(effic<1.2)
	{
		COLORREF cr1=CalEfficColor(effic);
		COLORREF cr2=g_sysPara.CalEfficColor(effic);
		BYTE r1=GetRValue(cr1);
		BYTE r2=GetRValue(cr2);
		BYTE g1=GetGValue(cr1);
		BYTE g2=GetGValue(cr2);
		BYTE b1=GetBValue(cr1);
		BYTE b2=GetBValue(cr2);
		if(cr1==cr2)
			logerr.Log("effic=%.2f,RGB(%d,%d,%d)",effic,r1,g1,b1);
		else
			logerr.Log("effic=%.2f,RGB1(%d,%d,%d)!=RGB2(%d,%d,%d)",effic,r1,g1,b1,r2,g2,b2);
		effic+=0.05;
	}
	logerr.ShowToScreen();
	effic=1.0;
	//CTempFileBuffer file("d:\\1.odf");
	//file.ClearContents();
	/*CTempFileBuffer tmpfile("d:\\lds.exe");
	FILE* fpr=fopen("D:\\鉴定资料准备\\测试用例\\新版测试文件\\SSJ2-C-W_node.tmp","rb");
	DYN_ARRAY<char>block(1024);
	timer.Start();
	while(!feof(fpr))
	{
		size_t size=fread(block,1,1024,fpr);
		tmpfile.Write(block,size);
	}
	timer.End();
	DWORD ticks1=timer.GetEndTicks()-timer.GetStartTicks();
	CTempFileBuffer tmpfile2("d:\\lds.ex");
	block.Resize(0x100000);
	fseek(fpr,0,SEEK_SET);
	timer.Start();
	while(!feof(fpr))
	{
		size_t size=fread(block,1,0x100000,fpr);
		tmpfile.Write(block,size);
	}
	timer.End();
	DWORD ticks2=timer.GetEndTicks()-timer.GetStartTicks();
	fclose(fpr);
	/*f3dLine line0,line1;
	line0.endPt.Set(1000,0,0);
	line1.startPt.Set(-100,100,0);
	line1.endPt.Set(0,0);
	f3dPoint inters;
	Int3dpl(line0,line1,inters);
	double coef=g_sysPara.IceLoadCoefInterpByThick(5);
	coef=g_sysPara.IceLoadCoefInterpByThick(10);
	coef=g_sysPara.IceLoadCoefInterpByThick(15);
	coef=g_sysPara.IceLoadCoefInterpByThick(20);
	coef=g_sysPara.IceLoadCoefInterpByThick(25);
	coef=g_sysPara.IceLoadCoefInterpByThick(30);
	coef=g_sysPara.IceLoadCoefInterpByThick(40);
	coef=g_sysPara.IceLoadCoefInterpByThick(50);
	/*f3dPoint pt(1,2,3);
	CXhChar100 ss=PointToString(pt);
	TestPtPtr(&pt);
	/*
	logerr.ClearContents();
	SUBID id;
	for(BYTE i=0;i<16;i++)
	{
		id=SUBID(0x12897,i);
		logerr.Log("0x%X=%d:0x%X",id,id.Group(),id.Id());
		id=SUBID(0x0fffffff,i);
		logerr.Log("0x%X=%d:0x%X",id,id.Group(),id.Id());
		id=SUBID(0x07ffffff,i);
		logerr.Log("0x%X=%d:0x%X",id,id.Group(),id.Id());
		id=SUBID(0x03ffffff,i);
		logerr.Log("0x%X=%d:0x%X",id,id.Group(),id.Id());
	}
	logerr.ShowToScreen();
	/*CODES code(1);
	code.details.family=1;
	IMemberDesignCode* pCode=IMemberDesignCode::FromCodeSerial(0);
	code=pCode->CodeSerial();
	pCode=IMemberDesignCode::FromCodeSerial(1);
	code=pCode->CodeSerial();
	pCode=IMemberDesignCode::FromCodeSerial(2);
	code=pCode->CodeSerial();
	pCode=IMemberDesignCode::FromCodeSerial(3);
	code=pCode->CodeSerial();
	if(pCode->CodeSerial()!=3)
		TRACE("error");
	CPreProcessor::ActiveDesignCode=IMemberDesignCode::CODE_DLT_2002();
	Test_CalKlamdaCore();
	CPreProcessor::ActiveDesignCode=IMemberDesignCode::CODE_DLT_2012();
	Test_CalKlamdaCore();
	CPreProcessor::ActiveDesignCode=IMemberDesignCode::CODE_ASCE10_97();
	Test_CalKlamdaCore();
	CPreProcessor::ActiveDesignCode=IMemberDesignCode::GB50135_2006();
	Test_CalKlamdaCore();
	Test_CompStableCoef();*/
}

/*void CompareMultiOffsetPos(CLDSLineAngle *pLineAngle,bool bStart,CMultiOffsetPos *pSrcPos,CMultiOffsetPos *pDestPos)
{
	logerr.ClearContents();
	CString str;
	if(bStart)
		str = "起始端";
	else
		str = "终止端";
	if(pSrcPos->datum_jg_h==pDestPos->datum_jg_h)
		logerr.Log("0X%X角钢%s pSrcPos->datum_jg_h : %X -> %X",pLineAngle->handle,str,pSrcPos->datum_jg_h,pDestPos->datum_jg_h);
	if(pSrcPos->origin_pos_style==pDestPos->origin_pos_style)
		logerr.Log("0X%X角钢%s,pSrcPos->origin_pos_style ：%d -> %d",pLineAngle->handle,str,pSrcPos->origin_pos_style,pDestPos->origin_pos_style);
	if(pSrcPos->cDatumAngleWing==pDestPos->cDatumAngleWing)
		logerr.Log("0X%X角钢%s,pSrcPos->cDatumAngleWing : %c->%c",pLineAngle->handle,str,pSrcPos->cDatumAngleWing,pDestPos->cDatumAngleWing);
	if(pSrcPos->datumWingOffset.gStyle  == pDestPos->datumWingOffset.gStyle)
		logerr.Log("0X%X角钢%s,pSrcPos->datumWingOffset.gStyle : %d->%d",pLineAngle->handle,str,pSrcPos->datumWingOffset.gStyle,pDestPos->datumWingOffset.gStyle);
	if(pSrcPos->datumWingOffset.operStyle  == pDestPos->datumWingOffset.operStyle)
		logerr.Log("0X%X角钢%s,pSrcPos->datumWingOffset.operStyle : %d->%d",pLineAngle->handle,str,pSrcPos->datumWingOffset.operStyle,pDestPos->datumWingOffset.operStyle);
	if(pSrcPos->datumWingOffset.offsetDist  == pDestPos->datumWingOffset.offsetDist)
		logerr.Log("0X%X角钢%s,pSrcPos->datumWingOffset.offsetDist : %d->%d",pLineAngle->handle,str,pSrcPos->datumWingOffset.offsetDist,pDestPos->datumWingOffset.offsetDist);

	if(pSrcPos->spatialOperationStyle  == pDestPos->spatialOperationStyle)
		logerr.Log("0X%X角钢%s,pSrcPos->spatialOperationStyle : %d->%d",pLineAngle->handle,str,pSrcPos->spatialOperationStyle,pDestPos->spatialOperationStyle);
}*/
#endif
#include "ExcelOper.h"
void AddBoltSumTblSheet(LPDISPATCH pWorkSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.SetName("螺栓汇总表");
	//初始标题列
	CTowerType  mattbl;
	mattbl.RunSummarize(&Ta,true);
	CHashStrList<int>	hashSegList,hashStrList;
	CHashStrList<CXhChar16> hashGradeList;
	int nSeg=mattbl.segIList.GetNodeNum()*2+2;
	CVariant2dArray map(100,nSeg);
	int iRow=0,iCol=0;
	map.SetValueAt(iRow,0,COleVariant("名称"));
	map.SetValueAt(iRow,1,COleVariant("级别"));
	map.SetValueAt(iRow,2,COleVariant("规格"));
	int i=0;
	CXhChar16 segStr;
	for(i=0;i<mattbl.segIList.GetNodeNum();i++)
	{
		iCol=i+3;
		segStr=mattbl.segIList[i].ToString();
		map.SetValueAt(iRow,iCol,COleVariant(segStr));
		hashSegList.SetValue(mattbl.segIList[i].ToString(),iCol);
	}
	for(int j=0;j<mattbl.segIList.GetNodeNum();j++)
	{
		iCol=i+j+3;
		segStr=mattbl.segIList[j].ToString();
		map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("%sA",(char*)segStr)));
		hashSegList.SetValue(CXhChar16("%sA",(char*)segStr),iCol);
	}
	//添加行---普通螺栓
	iRow=1;iCol=2;
	int iStartRow=iRow,iStartRow2=iRow;
	map.SetValueAt(iRow,0,COleVariant("普通螺栓"));
	map.SetValueAt(iRow,1,COleVariant("6.8"));
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X40")));
	hashStrList.SetValue(CXhChar16("1M16X40"),iRow);
	hashGradeList.SetValue(CXhChar16("M16X40"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X50")));
	hashStrList.SetValue(CXhChar16("1M16X50"),iRow);
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X60")));
	hashStrList.SetValue(CXhChar16("1M16X60"),iRow);
	hashGradeList.SetValue(CXhChar16("M16X60"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X70")));
	hashStrList.SetValue(CXhChar16("1M16X70"),iRow);
	hashGradeList.SetValue(CXhChar16("M16X70"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X45")));
	hashStrList.SetValue(CXhChar16("1M20X45"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X45"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X55")));
	hashStrList.SetValue(CXhChar16("1M20X55"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X55"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X65")));
	hashStrList.SetValue(CXhChar16("1M20X65"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X65"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X75")));
	hashStrList.SetValue(CXhChar16("1M20X75"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X75"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X85")));
	hashStrList.SetValue(CXhChar16("1M20X85"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X85"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X95")));
	hashStrList.SetValue(CXhChar16("1M20X95"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X95"),CXhChar16("6.8"));
	CExcelOper::MergeRowRange(excel_sheet,CXhChar16("B%d",iStartRow2+1),CXhChar16("B%d",iRow+1));
	iRow++;
	map.SetValueAt(iRow,1,COleVariant("8.8"));
	iStartRow2=iRow;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X55")));
	hashStrList.SetValue(CXhChar16("1M24X55"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X55"),CXhChar16("8.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X65")));
	hashStrList.SetValue(CXhChar16("1M24X65"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X65"),CXhChar16("8.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X75")));
	hashStrList.SetValue(CXhChar16("1M24X75"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X75"),CXhChar16("8.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X85")));
	hashStrList.SetValue(CXhChar16("1M24X85"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X85"),CXhChar16("8.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X95")));
	hashStrList.SetValue(CXhChar16("1M24X95"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X95"),CXhChar16("8.8"));
	CExcelOper::MergeRowRange(excel_sheet,CXhChar16("A%d",iStartRow+1),CXhChar16("A%d",iRow+1));
	CExcelOper::MergeRowRange(excel_sheet,CXhChar16("B%d",iStartRow2+1),CXhChar16("B%d",iRow+1));
	//双帽螺栓
	iRow++;
	iStartRow=iStartRow2=iRow;
	map.SetValueAt(iRow,0,COleVariant("双帽螺栓"));
	map.SetValueAt(iRow,1,COleVariant("6.8"));
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X50")));
	hashStrList.SetValue(CXhChar16("2M16X50"),iRow);
	hashGradeList.SetValue(CXhChar16("M16X50"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X60")));
	hashStrList.SetValue(CXhChar16("2M16X60"),iRow);
	hashGradeList.SetValue(CXhChar16("M16X60"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X70")));
	hashStrList.SetValue(CXhChar16("2M16X70"),iRow);
	hashGradeList.SetValue(CXhChar16("M16X70"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X80")));
	hashStrList.SetValue(CXhChar16("2M16X80"),iRow);
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X90")));
	hashStrList.SetValue(CXhChar16("2M16X90"),iRow);
	hashGradeList.SetValue(CXhChar16("M16X90"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X60")));
	hashStrList.SetValue(CXhChar16("2M20X60"),iRow);
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X70")));
	hashStrList.SetValue(CXhChar16("2M20X70"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X70"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X80")));
	hashStrList.SetValue(CXhChar16("2M20X80"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X80"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X90")));
	hashStrList.SetValue(CXhChar16("2M20X90"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X90"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X100")));
	hashStrList.SetValue(CXhChar16("2M20X100"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X100"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X70")));
	hashStrList.SetValue(CXhChar16("2M24X70"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X70"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X80")));
	hashStrList.SetValue(CXhChar16("2M24X80"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X80"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X90")));
	hashStrList.SetValue(CXhChar16("2M24X90"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X90"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X100")));
	hashStrList.SetValue(CXhChar16("2M24X100"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X100"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X110")));
	hashStrList.SetValue(CXhChar16("2M24X110"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X110"),CXhChar16("6.8"));
	CExcelOper::MergeRowRange(excel_sheet,CXhChar16("A%d",iStartRow+1),CXhChar16("A%d",iRow+1));
	CExcelOper::MergeRowRange(excel_sheet,CXhChar16("B%d",iStartRow2+1),CXhChar16("B%d",iRow+1));
	//防盗螺栓
	iRow++;
	iStartRow=iStartRow2=iRow;
	map.SetValueAt(iRow,0,COleVariant("防盗螺栓"));
	map.SetValueAt(iRow,1,COleVariant("6.8"));
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X40")));
	hashStrList.SetValue(CXhChar16("3M16X40"),iRow);
	hashGradeList.SetValue(CXhChar16("M16X40"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X50")));
	hashStrList.SetValue(CXhChar16("3M16X50"),iRow);
	hashGradeList.SetValue(CXhChar16("M16X50"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X60")));
	hashStrList.SetValue(CXhChar16("3M16X60"),iRow);
	hashGradeList.SetValue(CXhChar16("M16X60"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X70")));
	hashStrList.SetValue(CXhChar16("3M16X70"),iRow);
	hashGradeList.SetValue(CXhChar16("M16X70"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X45")));
	hashStrList.SetValue(CXhChar16("3M20X45"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X45"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X55")));
	hashStrList.SetValue(CXhChar16("3M20X55"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X55"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X65")));
	hashStrList.SetValue(CXhChar16("3M20X65"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X65"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X75")));
	hashStrList.SetValue(CXhChar16("3M20X75"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X75"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X85")));
	hashStrList.SetValue(CXhChar16("3M20X85"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X85"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X95")));
	hashStrList.SetValue(CXhChar16("3M20X95"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X95"),CXhChar16("6.8"));
	CExcelOper::MergeRowRange(excel_sheet,CXhChar16("B%d",iStartRow2+1),CXhChar16("B%d",iRow+1));
	iRow++;
	map.SetValueAt(iRow,1,COleVariant("8.8"));
	iStartRow2=iRow;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X55")));
	hashStrList.SetValue(CXhChar16("3M24X55"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X55"),CXhChar16("8.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X65")));
	hashStrList.SetValue(CXhChar16("3M24X65"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X65"),CXhChar16("8.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X75")));
	hashStrList.SetValue(CXhChar16("3M24X75"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X75"),CXhChar16("8.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X85")));
	hashStrList.SetValue(CXhChar16("3M24X85"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X85"),CXhChar16("8.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X95")));
	hashStrList.SetValue(CXhChar16("3M24X95"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X95"),CXhChar16("8.8"));
	CExcelOper::MergeRowRange(excel_sheet,CXhChar16("A%d",iStartRow+1),CXhChar16("A%d",iRow+1));
	CExcelOper::MergeRowRange(excel_sheet,CXhChar16("B%d",iStartRow2+1),CXhChar16("B%d",iRow+1));
	//脚钉
	iRow++;
	iStartRow=iStartRow2=iRow;
	map.SetValueAt(iRow,0,COleVariant("普通脚钉"));
	map.SetValueAt(iRow,1,COleVariant("6.8"));
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X180")));
	hashStrList.SetValue(CXhChar16("4M16X180"),iRow);
	hashGradeList.SetValue(CXhChar16("M16X180"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X190")));
	hashStrList.SetValue(CXhChar16("4M16X190"),iRow);
	hashGradeList.SetValue(CXhChar16("M16X190"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X200")));
	hashStrList.SetValue(CXhChar16("4M20X200"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X200"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X210")));
	hashStrList.SetValue(CXhChar16("4M20X210"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X210"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X220")));
	hashStrList.SetValue(CXhChar16("4M20X220"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X220"),CXhChar16("6.8"));
	CExcelOper::MergeRowRange(excel_sheet,CXhChar16("B%d",iStartRow2+1),CXhChar16("B%d",iRow+1));
	iRow++;
	map.SetValueAt(iRow,1,COleVariant("8.8"));
	iStartRow2=iRow;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X240")));
	hashStrList.SetValue(CXhChar16("4M24X240"),iRow);
	hashGradeList.SetValue(CXhChar16("M24X200"),CXhChar16("8.8"));
	CExcelOper::MergeRowRange(excel_sheet,CXhChar16("A%d",iStartRow+1),CXhChar16("A%d",iRow+1));
	CExcelOper::MergeRowRange(excel_sheet,CXhChar16("B%d",iStartRow2+1),CXhChar16("B%d",iRow+1));
	//防盗脚钉
	iRow++;
	iStartRow=iStartRow2=iRow;
	map.SetValueAt(iRow,0,COleVariant("防盗脚钉"));
	map.SetValueAt(iRow,1,COleVariant("6.8"));
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X180")));
	hashStrList.SetValue(CXhChar16("5M16X180"),iRow);
	hashGradeList.SetValue(CXhChar16("M16X180"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M16X190")));
	hashStrList.SetValue(CXhChar16("5M16X190"),iRow);
	hashGradeList.SetValue(CXhChar16("M16X190"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X200")));
	hashStrList.SetValue(CXhChar16("5M20X200"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X200"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X210")));
	hashStrList.SetValue(CXhChar16("5M20X210"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X210"),CXhChar16("6.8"));
	iRow++;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M20X220")));
	hashStrList.SetValue(CXhChar16("5M20X220"),iRow);
	hashGradeList.SetValue(CXhChar16("M20X220"),CXhChar16("6.8"));
	CExcelOper::MergeRowRange(excel_sheet,CXhChar16("B%d",iStartRow2+1),CXhChar16("B%d",iRow+1));
	iRow++;
	map.SetValueAt(iRow,1,COleVariant("8.8"));
	iStartRow2=iRow;
	map.SetValueAt(iRow,iCol,COleVariant(CXhChar16("M24X240")));
	hashStrList.SetValue(CXhChar16("5M24X240"),iRow);
	hashGradeList.SetValue(CXhChar16("MM24X240"),CXhChar16("8.8"));
	CExcelOper::MergeRowRange(excel_sheet,CXhChar16("A%d",iStartRow+1),CXhChar16("A%d",iRow+1));
	CExcelOper::MergeRowRange(excel_sheet,CXhChar16("B%d",iStartRow2+1),CXhChar16("B%d",iRow+1));
	//
	int iR=0,iC=0;
	for(CLsRecord* pLsRecord=mattbl.ls_recordset.GetFirst();pLsRecord;pLsRecord=mattbl.ls_recordset.GetNext())
	{
		if(!pLsRecord->bFootNail&&pLsRecord->GetPartNum()>pLsRecord->nTwoCap)
		{	//普通标准螺栓N=(总数-双帽数),其中非防盗数NT=N-pLsRecord->nBurglarproof
			//根据pLsRecord->iSeg输出到当前规格行的相应段号列(N在iSeg段,NT在iSeg#NT列
			int nCout1=pLsRecord->GetPartNum()-pLsRecord->nTwoCap;	//包含防盗的螺栓数
			int nCout2=nCout1-pLsRecord->nBurglarproof;				//不包含防盗的螺栓数
			int* pValue=hashStrList.GetValue(CXhChar16("1M%dX%d",pLsRecord->d,pLsRecord->L));
			CXhChar16* pGuige=hashGradeList.GetValue(CXhChar16("M%dX%d",pLsRecord->d,pLsRecord->L));
			if(pValue && pGuige->Equal(pLsRecord->grade))
			{
				segStr=pLsRecord->iSeg.ToString();
				iR=*pValue;
				iC=*(hashSegList.GetValue(segStr));
				map.SetValueAt(iR,iC,COleVariant(CXhChar16("%d",nCout1)));
				iC=*(hashSegList.GetValue(CXhChar16("%sA",(char*)segStr)));
				map.SetValueAt(iR,iC,COleVariant(CXhChar16("%d",nCout2)));
			}
			else
			{
				if(pValue==NULL)
					logerr.Log("EXCEL中缺少普通螺栓规格%s",(char*)CXhChar16("M%dX%d",pLsRecord->d,pLsRecord->L));
				else
					logerr.Log("普通螺栓{%s}级别不正确%s",(char*)CXhChar16("M%dX%d",pLsRecord->d,pLsRecord->L),pLsRecord->grade);
			}
		}
		if(!pLsRecord->bFootNail&&pLsRecord->nTwoCap>0)
		{	//普通双帽螺栓
			int* pValue=hashStrList.GetValue(CXhChar16("2M%dX%d",pLsRecord->d,pLsRecord->L));
			CXhChar16* pGuige=hashGradeList.GetValue(CXhChar16("M%dX%d",pLsRecord->d,pLsRecord->L));
			if(pValue && pGuige->Equal(pLsRecord->grade))
			{
				segStr=pLsRecord->iSeg.ToString();
				iR=*pValue;
				iC=*(hashSegList.GetValue(segStr));
				map.SetValueAt(iR,iC,COleVariant(CXhChar16("%d",pLsRecord->nTwoCap)));
			}
			else
			{
				if(pValue==NULL)
					logerr.Log("EXCEL中缺少双帽螺栓规格%s",(char*)CXhChar16("M%dX%d",pLsRecord->d,pLsRecord->L));
				else
					logerr.Log("双帽螺栓{%s}级别不正确%s",(char*)CXhChar16("M%dX%d",pLsRecord->d,pLsRecord->L),pLsRecord->grade);
			}
		}
		if(!pLsRecord->bFootNail&&pLsRecord->nBurglarproof>0)
		{	//防盗螺栓
			int* pValue=hashStrList.GetValue(CXhChar16("3M%dX%d",pLsRecord->d,pLsRecord->L));
			CXhChar16* pGuige=hashGradeList.GetValue(CXhChar16("M%dX%d",pLsRecord->d,pLsRecord->L));
			if(pValue && pGuige->Equal(pLsRecord->grade))
			{
				segStr=pLsRecord->iSeg.ToString();
				iR=*pValue;
				iC=*(hashSegList.GetValue(segStr));
				map.SetValueAt(iR,iC,COleVariant(CXhChar16("%d",pLsRecord->nBurglarproof)));
			}
			else
			{
				if(pValue==NULL)
					logerr.Log("EXCEL中缺少防盗螺栓规格%s",(char*)CXhChar16("M%dX%d",pLsRecord->d,pLsRecord->L));
				else
					logerr.Log("防盗螺栓{%s}级别不正确%s",(char*)CXhChar16("M%dX%d",pLsRecord->d,pLsRecord->L),pLsRecord->grade);
			}
		}
		if(pLsRecord->bFootNail&&pLsRecord->GetPartNum()>0)
		{	//普通脚钉总数N,其中非防盗数NT=N-pLsRecord->nBurglarproof
			int nCout1=pLsRecord->GetPartNum();
			int nCout2=nCout1-pLsRecord->nBurglarproof;				//普通的脚钉数
			int* pValue=hashStrList.GetValue(CXhChar16("4M%dX%d",pLsRecord->d,pLsRecord->L));
			CXhChar16* pGuige=hashGradeList.GetValue(CXhChar16("M%dX%d",pLsRecord->d,pLsRecord->L));
			if(pValue && pGuige->Equal(pLsRecord->grade))
			{
				segStr=pLsRecord->iSeg.ToString();
				iR=*pValue;
				iC=*(hashSegList.GetValue(segStr));
				map.SetValueAt(iR,iC,COleVariant(CXhChar16("%d",nCout1)));
				iC=*(hashSegList.GetValue(CXhChar16("%sA",(char*)segStr)));
				map.SetValueAt(iR,iC,COleVariant(CXhChar16("%d",nCout1)));
			}
			else
			{
				if(pValue==NULL)
					logerr.Log("EXCEL中缺少普通脚钉规格%s",(char*)CXhChar16("M%dX%d",pLsRecord->d,pLsRecord->L));
				else
					logerr.Log("普通脚钉{%s}级别不正确%s",(char*)CXhChar16("M%dX%d",pLsRecord->d,pLsRecord->L),pLsRecord->grade);
			}
		}
		if(pLsRecord->bFootNail&&pLsRecord->nBurglarproof>0)
		{	//防盗脚钉总数pLsRecord->nBurglarproof
			int* pValue=hashStrList.GetValue(CXhChar16("5M%dX%d",pLsRecord->d,pLsRecord->L));
			CXhChar16* pGuige=hashGradeList.GetValue(CXhChar16("M%dX%d",pLsRecord->d,pLsRecord->L));
			if(pValue && pGuige->Equal(pLsRecord->grade))
			{
				segStr=pLsRecord->iSeg.ToString();
				iR=*pValue;
				iC=*(hashSegList.GetValue(segStr));
				map.SetValueAt(iR,iC,COleVariant(CXhChar16("%d",pLsRecord->nBurglarproof)));
			}
			else
			{
				if(pValue==NULL)
					logerr.Log("EXCEL中缺少防盗脚钉规格%s",(char*)CXhChar16("M%dX%d",pLsRecord->d,pLsRecord->L));
				else
					logerr.Log("防盗脚钉{%s}级别不正确%s",(char*)CXhChar16("M%dX%d",pLsRecord->d,pLsRecord->L),pLsRecord->grade);
			}
		}
	}
	CXhChar16 cell_end("%C%d",'A'+nSeg-1,iRow+1);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);	//填充表格内容
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));//水平左对齐
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));	//设置边框
	excel_sheet.ReleaseDispatch();
}
void Test_BoltSubTbl()
{
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet;
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	AddBoltSumTblSheet(pWorksheet);
	excel_sheets.ReleaseDispatch();
}
int CLDSView::InternalTest()
{
	Test_ImportOffNodePara();
	return 0;
	if(!PRODUCT_FUNC::IsHasInternalTest())
		return -1;
	CLogErrorLife logErrLife;
	Test_BoltSubTbl();
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pCmdLine->FinishCmdLine();
	return 0;
}
