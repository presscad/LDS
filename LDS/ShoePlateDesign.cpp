#include "stdafx.h"
#include "LDSView.h"
#include "ShoePlateDesign.h"
#include "MainFrm.h"
#include "CmdLineDlg.h"
#include "Tower.h"
#include "SnapTypeVerify.h"
#include "SortFunc.h"

#ifdef __ANGLE_PART_INC_
//////////////////////////////////////////////////////////////////////////
//
void SHOEPLATE::Init()
{
	m_cProfileSide=0;		//贯穿底板
	m_cLinkType=0;			
	m_cAngleWorkWing='X';		//
	m_nNormOffset=0;
	m_nThick=10;
	A=30;
	B=30;
	m_fMargin=0;
	m_fAlongVecLen=300;
}
//////////////////////////////////////////////////////////////////////////
//
static int comparefun(const WORKWINGLS& item1,const WORKWINGLS& item2)
{
	return compare_double(item1.fDist,item2.fDist);
}
//根据连接角钢更新靴板螺栓链表
void UpdateShoePlateLsRefList(CLDSParamPlate* pShoePlate,CLDSLineAngle* pJg,f3dPoint work_norm)
{
	if(pShoePlate==NULL)
		return;
	long hBasePlate=0;
	pShoePlate->GetDesignItemValue('C',&hBasePlate);
	CLDSGeneralPlate* pBasePlate=(CLDSGeneralPlate*)console.FromPartHandle(hBasePlate,CLS_PARAMPLATE);
	if(pBasePlate==NULL)
		return;
	f3dPoint pt=pBasePlate->ucs.origin,along_vec;
	if(DISTANCE(pt,pJg->Start())>DISTANCE(pt,pJg->End()))	//终端设计
		along_vec=pJg->Start()-pJg->End();
	else	//始端设计
		along_vec=pJg->End()-pJg->Start();
	normalize(along_vec);
	ARRAY_LIST<WORKWINGLS> workLsArr;
	for(CLsRef* pLsRef=pJg->GetFirstLsRef();pLsRef;pLsRef=pJg->GetNextLsRef())
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		if(fabs(work_norm*pBolt->ucs.axis_z)<EPS_COS2)
			continue;
		if(pBolt->IsFootNail())
			continue;
		f3dPoint len_vec=pBolt->ucs.origin-pt;
		double len_dist=len_vec*along_vec;
		WORKWINGLS* pWorkLs=workLsArr.append();
		pWorkLs->pls=pBolt;
		pWorkLs->fDist=len_dist;
	}
	if(workLsArr.GetSize()==0)
		return;
	CQuickSort<WORKWINGLS>::QuickSort(workLsArr.m_pData,workLsArr.GetSize(),comparefun);
	CMaxDouble max_base_len(workLsArr[0].fDist);
	for(WORKWINGLS* pWorkLs=workLsArr.GetFirst();pWorkLs;pWorkLs=workLsArr.GetNext())
	{
		if(pWorkLs->fDist<max_base_len.number+62)
		{
			max_base_len.Update(pWorkLs->fDist,pWorkLs->pls);
			pShoePlate->AppendLsRef(pWorkLs->pls->GetLsRef(),FALSE);
		}
	}
}
//靴板设计
CLDSParamPlate* CreateOrUpdateShoePlate(SHOEPLATE& shoe_plate,CLDSParamPlate* pShoePlate=NULL)
{
	if(pShoePlate==NULL)
	{
		pShoePlate =(CLDSParamPlate*)Ta.Parts.append(CLS_PARAMPLATE);
		pShoePlate->m_iParamType=TYPE_SHOEPLATE;	//靴板
		pShoePlate->m_bDisplayHole = true;
		pShoePlate->m_cFuncType=CLDSPart::FUNC_FOOTPLATE;
	}
	pShoePlate->thick=shoe_plate.m_nThick;
	//外形参数
	pShoePlate->SetDesignItemValue('A',shoe_plate.A,NULL,PARAM_SHAPE);			//台阶高
	pShoePlate->SetDesignItemValue('B',shoe_plate.B,NULL,PARAM_SHAPE);			//顶部前侧余量
	pShoePlate->SetDesignItemValue(KEY2C("IA"),shoe_plate.A,NULL,PARAM_SHAPE);			//台阶高
	pShoePlate->SetDesignItemValue(KEY2C("IB"),shoe_plate.B,NULL,PARAM_SHAPE);			//顶部前侧余量
	pShoePlate->SetDesignItemValue('M',shoe_plate.m_fMargin,NULL,PARAM_SHAPE);	//靴板延伸至底板边缘时的间隙值
	pShoePlate->SetDesignItemValue(KEY2C("AL"),shoe_plate.m_fAlongVecLen,NULL,PARAM_SHAPE);//靴板非连接侧角钢长度方向延伸量，默认根据角钢螺栓计算
	pShoePlate->SetDesignItemValue('F',shoe_plate.m_cProfileSide);		//外形类别 0.贯通 1.肢翼侧 2.楞线侧
	if(shoe_plate.m_cProfileSide>0)
		pShoePlate->SetDesignItemValue('L',shoe_plate.m_fCutLen,NULL,PARAM_SHAPE);	//切角数据
	//装配参数
	pShoePlate->SetDesignItemValue('C',shoe_plate.m_hBasePlate,NULL,0,DESIGN_PARAM_ITEM::TYPE_HANDLE);	//塔脚底板句柄
	pShoePlate->SetDesignItemValue('D',shoe_plate.m_hBaseJg,NULL,0,DESIGN_PARAM_ITEM::TYPE_HANDLE);		//依赖角钢
	pShoePlate->SetDesignItemValue(KEY2C("LX"),shoe_plate.m_hXieJg,NULL,0,DESIGN_PARAM_ITEM::TYPE_HANDLE);	//连接射线角钢
	//TODO:
	//if(shoe_plate.m_cAngleWorkWing=='X')
	//	pShoePlate->SetDesignItemValue('W',0,NULL,0);
	//else if(shoe_plate.m_cAngleWorkWing=='Y')
	//	pShoePlate->SetDesignItemValue('W',1,NULL,0);
	pShoePlate->SetDesignItemValue('W',shoe_plate.m_cAngleWorkWing,NULL,0);		//靴板在底板的延伸方向
	pShoePlate->SetDesignItemValue(KEY2C("CS"),shoe_plate.m_cLinkType,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);	//与基准角钢连接方式
	if(shoe_plate.m_cLinkType==1)	//对焊连接时需要考虑法向便宜量
		pShoePlate->SetDesignItemValue(KEY2C("NL"),shoe_plate.m_nNormOffset,NULL,PARAM_POS);		//法向偏移量
	if(shoe_plate.m_cProfileSide!=0)	//楞线测需要连接靴板
		pShoePlate->SetDesignItemValue('G',shoe_plate.m_hAssitPlate,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_HANDLE);
	if(pShoePlate->DesignPlate())
	{
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		pShoePlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pShoePlate->GetSolidPartObject());
		g_pSolidDraw->AddCS(pShoePlate->handle,pShoePlate->ucs);
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pShoePlate->handle);
		g_pSolidDraw->Draw();
	}
	return pShoePlate;
}
//设计靴板的外形参数
CLDSParamPlate* DesignShoePlateProfilePara(CCmdLineDlg *pCmdLine,SHOEPLATE& shoe_plate,CLDSParamPlate* pShoePlate)
{
	if(pShoePlate==NULL)
		return NULL;
	CString cmdStr,cmdValue;
	long* id_arr=NULL;
	CLDSParamPlate* pAssistPlate=(CLDSParamPlate*)Ta.FromPartHandle(shoe_plate.m_hAssitPlate);
	//输入根高和顶部余量参数
	if(shoe_plate.m_cProfileSide==0||(pAssistPlate==NULL&&shoe_plate.m_cProfileSide==2))
	{
		//输入靴板在依赖底座的根高
		int nLen=shoe_plate.A;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("Please input base-plate's A parameter <%d>:",nLen);
#else
		cmdStr.Format("ShoePlate 请输入靴板在依赖底板的根高A <%d>:",nLen);
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		while(1)
		{
			if(!pCmdLine->GetStrFromCmdLine(cmdValue))
			{
				pCmdLine->CancelCmdLine();
				return NULL;
			}
			if(cmdValue.GetLength()>0)
				nLen=atoi(cmdValue);
			break;
		}
		shoe_plate.A=nLen;
		pShoePlate=CreateOrUpdateShoePlate(shoe_plate,pShoePlate);
		//输入靴板顶部到依赖角钢楞边的距离
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("Input shoe-plate's distance to base LineAngle <%d>:",shoe_plate.B);
#else 
		cmdStr.Format("ShoePlate 输入靴板顶部宽度余量B <%d>:",shoe_plate.B);
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		while(1)
		{
			if(!pCmdLine->GetStrFromCmdLine(cmdValue))
			{
				pCmdLine->CancelCmdLine();
				return NULL;
			}
			if(cmdValue.GetLength()>0)
				shoe_plate.B=atoi(cmdValue);
			break;
		}
	}
	else if(pAssistPlate)
	{
		long A=0,B=0;
		pAssistPlate->GetDesignItemValue('A',&A);
		pAssistPlate->GetDesignItemValue('B',&B);
		shoe_plate.A=(short)A;
		shoe_plate.B=(short)B;
	}
	pShoePlate=CreateOrUpdateShoePlate(shoe_plate,pShoePlate);
	//当靴板上螺栓为空时，需手动指定连接螺栓或输入靴板在角钢方向的延伸量
	if(shoe_plate.m_cProfileSide==2)	//楞线测靴板没有螺栓
		pShoePlate->EmptyLsRef();
	if(pShoePlate->GetLsCount()<=0)
	{	
		int nLen=200;
		if(shoe_plate.m_cLinkType==1 ||(shoe_plate.m_cLinkType==0&&shoe_plate.m_cProfileSide==2))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("Input shoe-plate's distance on line_ange vec <%d>:",nLen);
#else 
			cmdStr.Format("ShoePlate 指定沿角钢方向的最小包容长度 <%d>:",nLen);
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("Input shoe-plate's distance on line_ange vec <%d>:",nLen);
#else 
			cmdStr.Format("ShoePlate 选择靴板与依赖主材角钢间的连接螺栓，或指定沿角钢方向的最小包容长度 <%d>:",nLen);
#endif
		}
		pCmdLine->FillCmdLine(cmdStr,"");
		while(1)
		{
			if(!pCmdLine->GetStrFromCmdLine(cmdValue))
			{
				pCmdLine->CancelCmdLine();
				return NULL;
			}
			if(cmdValue.GetLength()>0)
				nLen=atoi(cmdValue);
			break;
		}
		if(shoe_plate.m_cLinkType==0&&shoe_plate.m_cProfileSide!=2)
		{
			long nSelects=0;
			if((nSelects=g_pSolidSnap->GetLastSelectEnts(id_arr))>0)
			{
				pShoePlate->EmptyLsRef();
				for(int i=0;i<nSelects;i++)
				{
					CLDSBolt* pBolt=(CLDSBolt*)Ta.Parts.FromHandle(id_arr[i],CLS_BOLT);
					if(pBolt)
						pShoePlate->AppendLsRef(pBolt->GetLsRef(),FALSE);
				}
			}
		}
		shoe_plate.m_fAlongVecLen=nLen;
		pShoePlate=CreateOrUpdateShoePlate(shoe_plate,pShoePlate);
	}
	//输入靴板底部到底座边缘间距和厚度
	if(pAssistPlate)
	{
		pAssistPlate->GetDesignItemValue('M',&shoe_plate.m_fMargin);
		shoe_plate.m_nThick=(short)pAssistPlate->Thick;
	}
	else
	{
		//输入靴板底部到底板边缘间距
		int nLen=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("Input shoe-plate's distance to base palte <%d>:",nLen);
#else 
		cmdStr.Format("ShoePlate 输入靴板底边到底座边缘的间距M <%d>:",nLen);
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		while(1)
		{
			if(!pCmdLine->GetStrFromCmdLine(cmdValue))
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			if(cmdValue.GetLength()>0)
				nLen=atoi(cmdValue);
			break;
		}
		shoe_plate.m_fMargin=nLen;
		//输入靴板厚度
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("Please input shoe-plate's thick <%d>:",shoe_plate.m_nThick);
#else 
		cmdStr.Format("ShoePlate 请输入靴板厚度 <%d>:",shoe_plate.m_nThick);
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		while(1)
		{
			if(!pCmdLine->GetStrFromCmdLine(cmdValue))
			{
				pCmdLine->CancelCmdLine();
				return NULL;
			}
			if(cmdValue.GetLength()>0)
				shoe_plate.m_nThick=atoi(cmdValue);
			if(shoe_plate.m_nThick<=0)
				pCmdLine->FillCmdLine(cmdStr,"");
			else
				break;
		}
	}
	return CreateOrUpdateShoePlate(shoe_plate,pShoePlate);
}
//设计螺栓连接的靴板
CLDSParamPlate* DesignBoltConnShoePlate(CCmdLineDlg *pCmdLine,SHOEPLATE& shoe_plate)
{
	CLDSGeneralPlate* pBasePlate=(CLDSGeneralPlate*)Ta.FromPartHandle(shoe_plate.m_hBasePlate,CLS_PLATE,CLS_PARAMPLATE);
	CLDSLineAngle* pBaseAngle=(CLDSLineAngle*)Ta.FromPartHandle(shoe_plate.m_hBaseJg,CLS_LINEANGLE);
	if(pBaseAngle==NULL || pBasePlate==NULL)
		return FALSE;
	CString cmdStr,cmdValue;
	long* id_arr=NULL;
	CLDSParamPlate* pShoePlate=NULL;
	pShoePlate=CreateOrUpdateShoePlate(shoe_plate,pShoePlate);
	if(pShoePlate!=NULL)
	{
		pShoePlate->cMaterial=pBaseAngle->cMaterial;
		pShoePlate->iSeg=pBaseAngle->iSeg;
	}
	//根据靴板与角钢贴合肢，自动选择螺栓
	f3dPoint work_norm;
	if(shoe_plate.m_cAngleWorkWing=='X')
		work_norm=pBaseAngle->vxWingNorm;
	else
		work_norm=pBaseAngle->vyWingNorm;
	UpdateShoePlateLsRefList(pShoePlate,pBaseAngle,work_norm);
	pShoePlate=CreateOrUpdateShoePlate(shoe_plate,pShoePlate);
	//请确定靴板的外形类别
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("ShoePlate Please select shoe-plate's profile side<0.base side|1.wing side|1.ber side> [0]: ");
#else
	cmdStr.Format("ShoePlate 请确定靴板在依赖角钢的位置<0.贯穿底板|1.肢翼单侧|2.楞线单侧> [0]: ");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdValue,CCmdLineDlg::KEYWORD_AS_RETURN,"0|1|2"))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		if(cmdValue.GetLength()==0)
		{
			shoe_plate.m_cProfileSide=0;
			break;
		}
		else if(atoi(cmdValue)>=0&&atoi(cmdValue)<3)
		{
			shoe_plate.m_cProfileSide=atoi(cmdValue);
			break;
		}
		else
			pCmdLine->FillCmdLine(cmdStr,"");
	}
	if(shoe_plate.m_cProfileSide>0)	
	{	//选择设计靴板的辅助靴板
		CSnapTypeVerify verifier;
		DWORD dwhObj=0,dwExportFlag=0;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("Please select Assist Shoe Plate:");
#else
		if(shoe_plate.m_cProfileSide==1)
			cmdStr.Format("ShoePlate 请选择肢翼单侧靴板的辅助靴板:");
		else
			cmdStr.Format("ShoePlate 请选择楞线单侧靴板的辅助靴板:");
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		CSuperSmartPtr<CLDSPart> pAssistPlate;
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,
			GetSingleWord(SELECTINDEX_PARAMPLATE)|GetSingleWord(SELECTINDEX_PLATE));
		while(true)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			{
				pCmdLine->CancelCmdLine();
				return NULL;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;	//dwhObj不一定是选中对象的句柄标识，需要转换
			pAssistPlate=Ta.Parts.FromHandle(dwhObj);
			if(pAssistPlate.IsHasPtr()&&pAssistPlate->GetClassTypeId()==CLS_PARAMPLATE&&
				pAssistPlate.ParamPlatePointer()->m_iParamType==TYPE_SHOEPLATE)
				break;	//已选中依赖杆件或回车确认不需人依赖杆件
		}
		g_pSolidDraw->SetEntSnapStatus(pAssistPlate->handle);
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pAssistPlate->handle));
		shoe_plate.m_hAssitPlate=pAssistPlate->handle;
		pShoePlate=CreateOrUpdateShoePlate(shoe_plate,pShoePlate);
	}
	if(shoe_plate.m_cProfileSide!=2)
	{	//选择射线角钢
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("BasePlate select and confirm the xie angle:");
#else
		cmdStr.Format("BasePlate 请选择靴板连接的射线角钢:");
#endif	
		pCmdLine->FillCmdLine(cmdStr,"");
		CLDSLineAngle *pXieAngle=NULL;
		CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART);
		verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prLine);
		long* id_arr=NULL,retcode=0;
		DWORD dwhObj=0,dwExportFlag=0;
		while(true)
		{
			if((retcode=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			{
				pCmdLine->CancelCmdLine();
				return NULL;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;	//dwhObj不一定是选中对象的句柄标识，需要转换
			if(obj.provider==OBJPROVIDER::SOLIDSPACE)
				dwhObj=obj.hObj;
			if(dwhObj==0&&g_pSolidSnap->GetLastSelectEnts(id_arr)>0)
				dwhObj=id_arr[0];
			else if(dwhObj==0&&cmdValue.GetLength()>0)
				dwhObj=HexStrToLong(cmdValue);
			pXieAngle=(CLDSLineAngle*)Ta.FromRodHandle(dwhObj);
			if(pXieAngle || obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
				break;
		}
		if(pXieAngle)
		{
			g_pSolidDraw->SetEntSnapStatus(pXieAngle->handle);
			pCmdLine->FinishCmdLine(CXhChar16("0x%X",pXieAngle->handle));
			shoe_plate.m_hXieJg=pXieAngle->handle;
			UpdateShoePlateLsRefList(pShoePlate,pXieAngle,work_norm);
			pShoePlate=CreateOrUpdateShoePlate(shoe_plate,pShoePlate);
		}
		else
			pCmdLine->FinishCmdLine((CXhChar16("0x0")));
	}
	return CreateOrUpdateShoePlate(shoe_plate,pShoePlate);
	//return DesignShoePlateProfilePara(pCmdLine,shoe_plate,pShoePlate);
}
//设计对焊连接的靴板
CLDSParamPlate* DesignButtWeldShoePlate(CCmdLineDlg *pCmdLine,SHOEPLATE& shoe_plate)
{
	CString cmdStr,cmdValue;
	long* id_arr=NULL;
	CLDSParamPlate* pShoePlate=CreateOrUpdateShoePlate(shoe_plate);
	shoe_plate.m_cProfileSide=1;
	//请确定靴板的外形类别
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("ShoePlate Please select shoe-plate's profile side<1.wing side|1.ber side> [1]: ");
#else
	cmdStr.Format("ShoePlate 请确定靴板在依赖角钢的位置<1.肢翼单侧|2.楞线单侧> [1]: ");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdValue,CCmdLineDlg::KEYWORD_AS_RETURN,"1|2"))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		if(cmdValue.GetLength()==0)
		{
			shoe_plate.m_cProfileSide=1;
			break;
		}
		else if(atoi(cmdValue)>=1&&atoi(cmdValue)<3)
		{
			shoe_plate.m_cProfileSide=atoi(cmdValue);
			break;
		}
		else
			pCmdLine->FillCmdLine(cmdStr,"");
	}
	pCmdLine->FinishCmdLine(CXhChar16("%d",shoe_plate.m_cProfileSide));
	return CreateOrUpdateShoePlate(shoe_plate,pShoePlate);
	//return DesignShoePlateProfilePara(pCmdLine,shoe_plate,pShoePlate);
}
//设计靴板操作
int CLDSView::DesignShoePlate()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->Draw();
	CString cmdStr,cmdValue;
	SHOEPLATE shoe_plate;
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	CSnapTypeVerify verifier;
	DWORD dwhObj=0,dwExportFlag=0;
	CSysPromptObject prompt;
	CXhChar200 imagefile("%simages\\prompt_images\\shoe_plate.bmp",theApp.execute_path);
	prompt.SetPromptPicture(imagefile);
	//选择靴板的依赖底板
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Please select dependent base plate:");
#else
	cmdStr.Format("ShoePlate 请选择靴板的焊接底座板:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	CSuperSmartPtr<CLDSPart> pBasePlate;
	verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,
		GetSingleWord(SELECTINDEX_PARAMPLATE)|GetSingleWord(SELECTINDEX_PLATE));
	while(true)
	{
		if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		dwhObj=obj.hRelaObj;	//dwhObj不一定是选中对象的句柄标识，需要转换
		pBasePlate=Ta.Parts.FromHandle(dwhObj);
		if(pBasePlate.IsHasPtr())	//必须选择依赖底座板
			break;
	}
	g_pSolidDraw->SetEntSnapStatus(pBasePlate->handle);
	shoe_plate.m_hBasePlate=pBasePlate->handle;
	//选择基本依赖杆件
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("select and confirm the leg angle connected with base plate:");
#else
	cmdStr.Format("ShoePlate 请选择靴板的连接角钢:");
#endif	
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pBasePlate->handle));
	pCmdLine->FillCmdLine(cmdStr,"");
	CLDSLineAngle* pBaseAngle;
	verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE));
	verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prLine);
	long* id_arr=NULL;
	while(true)
	{
		if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		dwhObj=obj.hRelaObj;	//dwhObj不一定是选中对象的句柄标识，需要转换
		if(obj.provider==OBJPROVIDER::SOLIDSPACE)
			dwhObj=obj.hObj;
		if(dwhObj==0&&g_pSolidSnap->GetLastSelectEnts(id_arr)>0)
			dwhObj=id_arr[0];
		else if(dwhObj==0&&cmdValue.GetLength()>0)
			dwhObj=HexStrToLong(cmdValue);
		pBaseAngle=(CLDSLineAngle*)Ta.FromPartHandle(dwhObj,CLS_LINEANGLE);
		if(pBaseAngle)	//必须选择依赖角钢
			break;	
	}
	g_pSolidDraw->SetEntSnapStatus(pBaseAngle->handle);
	shoe_plate.m_hBaseJg=pBaseAngle->handle;
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pBaseAngle->handle));
	//计算角钢正负头
	f3dPoint origin=pBasePlate->ucs.origin;
	if(DISTANCE(pBaseAngle->Start(),origin)>DISTANCE(pBaseAngle->End(),origin))
	{
		if(pBaseAngle->endOdd()==0)	
		{
			pBaseAngle->desEndOdd.m_iOddCalStyle=0;	//碰撞干涉
			pBaseAngle->desEndOdd.m_hRefPart1=pBasePlate->handle;
			pBaseAngle->CalEndOddment();
		}
	}
	else
	{
		if(pBaseAngle->startOdd()==0)
		{
			pBaseAngle->desStartOdd.m_iOddCalStyle=0;	//碰撞干涉
			pBaseAngle->desStartOdd.m_hRefPart1=pBasePlate->handle;
			pBaseAngle->CalStartOddment();
		}
	}
	//根据依赖角钢自动计算靴板默认厚度值
	shoe_plate.m_nThick=(short)pBaseAngle->Thick;
	shoe_plate.m_nThick+=2+ftoi((short)shoe_plate.m_nThick)%2;
	shoe_plate.m_nThick=max((short)shoe_plate.m_nThick,6);	//最小板厚必须保证6mm
	//输入靴板厚度
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Please input shoe-plate's thick <%d>:",shoe_plate.m_nThick);
#else 
	cmdStr.Format("ShoePlate 请输入靴板厚度 <%d>:",shoe_plate.m_nThick);
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdValue))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		if(cmdValue.GetLength()>0)
			shoe_plate.m_nThick=atoi(cmdValue);
		if(shoe_plate.m_nThick<=0)
			pCmdLine->FillCmdLine(cmdStr,"");
		else
			break;
	}
	//靴板在依赖底板的延伸方向
	double justifyx=fabs(pBaseAngle->vxWingNorm*pBasePlate->ucs.axis_z);
	double justifyy=fabs(pBaseAngle->vyWingNorm*pBasePlate->ucs.axis_z);
	if(justifyx<justifyy+EPS)	//X肢与底板更垂直
		shoe_plate.m_cAngleWorkWing='X';
	else
		shoe_plate.m_cAngleWorkWing='Y';
	//靴板在依赖底板的延伸方向
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("ShoePlate Please select shoe-plate's extend direcion[X|Y] <X>: ");
#else
	cmdStr.Format("ShoePlate 请确定靴板与依赖角钢的连接肢[X(红肢)|Y(绿肢)] <X>: ");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdValue,CCmdLineDlg::KEYWORD_AS_RETURN,"X|Y"))
		{
			pCmdLine->CancelCmdLine();
			return NULL;
		}
		if(cmdValue.GetLength()==0)
			break;
		else if(toupper(cmdValue[0])=='X')
		{
			shoe_plate.m_cAngleWorkWing='X';
			break;
		}
		else if(toupper(cmdValue[0])=='Y')
		{
			shoe_plate.m_cAngleWorkWing='Y';
			break;
		}
		else
			pCmdLine->FillCmdLine(cmdStr,"");
	}
	//确定靴板与基准角钢的连接方式
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Please select base-plate's connect style[Bolt Connect(B)|Weld Connect(W)] <B>:");
#else
	cmdStr.Format("ShoePlate 请确定靴板与依赖角钢的连接方式[螺栓连接(B)|对焊连接(W)] <B>:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdValue,CCmdLineDlg::KEYWORD_AS_RETURN,"B|W"))
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		if(cmdValue.GetLength()<=0 || toupper(cmdValue[0])=='B')
		{
			shoe_plate.m_cLinkType=0;
			break;
		}
		else if(toupper(cmdValue[0])=='W')
		{
			shoe_plate.m_cLinkType=1;
			break;
		}
		else
			pCmdLine->FillCmdLine(cmdStr,"");
		break;
	}
	CUndoOperObject undo(&Ta,true);
	CLDSParamPlate* pShoePlate=NULL;
	if(shoe_plate.m_cLinkType==0)	//螺栓连接
		pShoePlate=DesignBoltConnShoePlate(pCmdLine,shoe_plate);
	else							//对焊连接
		pShoePlate=DesignButtWeldShoePlate(pCmdLine,shoe_plate);
	if(pShoePlate)
	{
		pCmdLine->FinishCmdLine();
		g_pSolidDraw->ReleaseSnapStatus();
		for(CLsRef* pLsRef=pShoePlate->GetFirstLsRef();pLsRef;pLsRef=pShoePlate->GetNextLsRef())
		{
			CLDSBolt* pBolt=pLsRef->GetLsPtr();
			pBolt->AddL0Thick(pShoePlate->handle,TRUE);
			pBolt->CalGuigeAuto();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
		g_pSolidDraw->Draw();
		return 1;
	}
	else
		return 0;
}
#endif