#include "stdafx.h"
#include "LDSView.h"
#include "BasePlateDesign.h"
#include "MainFrm.h"
#include "CmdLineDlg.h"
#include "Tower.h"
#include "SnapTypeVerify.h"


#ifdef __ANGLE_PART_INC_
//////////////////////////////////////////////////////////////////////////
//
void BASEPLATE::Init()
{
	m_cShape=0;
	m_cNormOffsetType=0;
	m_iVectZType=6;
	m_nThick=15;
	C=5;
	XA=XB=YA=YB=300;
}
//////////////////////////////////////////////////////////////////////////
//初稿原始代码移植自DesignFoot.cpp wjh-2016.8.20
f3dPoint CalBasePlatePosition(BASEPLATE& base_plate)
{
	f3dPoint origin;
	CLDSNode *pNode=console.FromNodeHandle(base_plate.m_hBaseNode);
	if(pNode==NULL)
		return origin;
	origin=pNode->Position(true);
	CLDSLinePart* pBaseRod=(CLDSLinePart*)console.FromPartHandle(base_plate.m_hBaseRod,CLS_LINEPART);
	if(pBaseRod==NULL)
		return origin;
	else if(pBaseRod->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube* pBaseTube=(CLDSLineTube*)pBaseRod;
	}
	else if(pBaseRod->IsAngle())
	{
		CLDSLineAngle* pBaseJg=(CLDSLineAngle*)pBaseRod;
		double offset_gx=0,offset_gy=0;
		if(pBaseJg->GetClassTypeId()!=CLS_GROUPLINEANGLE)
		{
			JGZJ JgZhunJuX=pBaseJg->GetZhunJu('X');
			JGZJ JgZhunJuY=pBaseJg->GetZhunJu('Y');
			offset_gx=JgZhunJuX.g*0.5;
			offset_gy=JgZhunJuY.g*0.5;
		}
		//确定底脚板根坐标系方向
		f3dPoint axisVecX,lenVec;
		if(pBaseJg->pStart==pNode)
		{
			lenVec=pBaseJg->End()-pBaseJg->Start();
			if(lenVec.z<0)	//基准角钢在底板上侧
				axisVecX=-pBaseJg->GetWingVecY();
			else
				axisVecX=-pBaseJg->GetWingVecX();
		}
		else if(pBaseJg->pEnd==pNode)
		{
			lenVec=pBaseJg->Start()-pBaseJg->End();
			if(lenVec.z<0)	//基准角钢在底板上侧
				axisVecX=-pBaseJg->GetWingVecX();
			else
				axisVecX=-pBaseJg->GetWingVecY();
		}
		if(fabs(axisVecX.x)>fabs(axisVecX.y)&&axisVecX.x>0)
		{	//1号位
			origin.x+=offset_gx;
			origin.y+=offset_gy;
		}
		else if(fabs(axisVecX.x)>fabs(axisVecX.y)&&axisVecX.x<0)
		{
			origin.x-=offset_gx;
			origin.y-=offset_gy;
		}
		else if(fabs(axisVecX.x)<fabs(axisVecX.y)&&axisVecX.y>0)
		{
			origin.x-=offset_gy;
			origin.y+=offset_gx;
		}
		else if(fabs(axisVecX.x)<fabs(axisVecX.y)&&axisVecX.y<0)
		{
			origin.x+=offset_gy;
			origin.y-=offset_gx;
		}
	}
	return origin;
}
void UpdateLinkAngleOddment(BASEPLATE& base_plate,CLDSLineAngle* pMainJg)
{
	CLDSNode* pBaseNode=Ta.FromNodeHandle(base_plate.m_hBaseNode);
	if(pBaseNode==NULL || pMainJg==NULL)
		return;
	f3dPoint origin=CalBasePlatePosition(base_plate);
	f3dPoint vertice,vertex_x,vertex_y,vertex_up,vertex_down,line_vec;
	double datumPlaneOrgZ=pBaseNode->Position().z;
	datumPlaneOrgZ-=base_plate.m_nNormOffset+base_plate.m_nThick;
	f3dPoint ber_pos;
	short C=max(0,base_plate.C);
	BOOL bStart=FALSE;
	if(pMainJg->pStart->handle==base_plate.m_hBaseNode)
	{
		line_vec=pMainJg->Start()-pMainJg->End();
		normalize(line_vec);
		vertex_x = pMainJg->Start()+pMainJg->GetWingVecX()*pMainJg->GetWidth();
		vertex_y = pMainJg->Start()+pMainJg->GetWingVecY()*pMainJg->GetWidth();
		ber_pos = pMainJg->Start();
		bStart=TRUE;
	}
	else
	{
		line_vec=pMainJg->End()-pMainJg->Start();
		normalize(line_vec);
		vertex_x = pMainJg->End()+pMainJg->GetWingVecX()*pMainJg->GetWidth();
		vertex_y = pMainJg->End()+pMainJg->GetWingVecY()*pMainJg->GetWidth();
		ber_pos = pMainJg->End();
		bStart=FALSE;
	}
	//根据不同的连接方式计算角钢正负头
	double odd=0;
	if(base_plate.m_bWeldDiZuo)
	{	//主材焊接到底座
		f3dPoint datumPlaneOrg(origin.x,origin.y,datumPlaneOrgZ);
		Int3dlf(vertice,ber_pos,line_vec,datumPlaneOrg,f3dPoint(0,0,-1));
		odd=DISTANCE(vertice,ber_pos);
		if((datumPlaneOrgZ-ber_pos.z)*line_vec.z<0)
			odd*=-1.0;
	}
	else
	{	//主材与底座有间隔
		if(pMainJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pMainJg;
			vertex_up=vertex_down=ber_pos;
			for(int i=0;i<4;i++)
			{
				CLDSLineAngle *pSonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
				if(pSonJg)
				{
					f3dPoint tem_pos;
					if(pMainJg->pStart->handle==base_plate.m_hBaseNode)
						tem_pos=pSonJg->Start();
					else
						tem_pos=pSonJg->End();
					vertice=tem_pos;
					if(vertice.z>vertex_down.z)
						vertex_down=vertice;
					if(vertice.z<vertex_up.z)
						vertex_up=vertice;
					vertice=tem_pos+pSonJg->GetWingVecX()*pSonJg->GetWidth();
					if(vertice.z>vertex_down.z)
						vertex_down=vertice;
					if(vertice.z<vertex_up.z)
						vertex_up=vertice;
					vertice=tem_pos+pSonJg->GetWingVecY()*pSonJg->GetWidth();
					if(vertice.z>vertex_down.z)
						vertex_down=vertice;
					if(vertice.z<vertex_up.z)
						vertex_up=vertice;
				}
			}
		}
		else
		{
			if(vertex_x.z>vertex_y.z)
			{	//X肢更低
				vertex_up=vertex_y;
				vertex_down=vertex_x;
			}
			else
			{	//Y肢更低
				vertex_up=vertex_x;
				vertex_down=vertex_y;
			}
		}
		odd= vertex_down.z-datumPlaneOrgZ+C;
		odd*=(-1)*(line_vec.mod()/line_vec.z);
	}
	if(bStart)
		pMainJg->SetStartOdd(odd);
	else
		pMainJg->SetEndOdd(odd);
}
//底板设计
CLDSParamPlate* CreateOrUpdateBasePlate(BASEPLATE& base_plate,CLDSParamPlate* pBasePlate=NULL)
{
	if(pBasePlate==NULL)
	{
		pBasePlate =(CLDSParamPlate*)Ta.Parts.append(CLS_PARAMPLATE);
		pBasePlate->m_iParamType=TYPE_BASEPLATE;	//塔脚底板
		pBasePlate->m_bDisplayHole = true;
		pBasePlate->m_cFuncType=CLDSPart::FUNC_FOOTPLATE;
	}
	pBasePlate->thick=base_plate.m_nThick;
	if(base_plate.m_cNormOffsetType==0)			//上平面
		base_plate.m_nNormOffset=-base_plate.m_nThick;
	else if(base_plate.m_cNormOffsetType==1)	//下平面
		base_plate.m_nNormOffset=0;
	else if(base_plate.m_cNormOffsetType==2)	//中间平面
		base_plate.m_nNormOffset=-(short)pBasePlate->thick/2;
	else //if(base_plate.m_cNormOffsetType==2)	//中间平面
		base_plate.m_nNormOffset=base_plate.m_nNormOffset;
	f3dPoint origin=CalBasePlatePosition(base_plate);
	pBasePlate->design.origin.SetPosition(origin);
	pBasePlate->ucs.origin=origin+pBasePlate->ucs.axis_z*base_plate.m_nNormOffset;
	//
	CLDSNode *pBaseNode=console.FromNodeHandle(base_plate.m_hBaseNode);
	CLDSLinePart* pBaseRod=(CLDSLinePart*)Ta.FromPartHandle(base_plate.m_hBaseRod);
	if(pBaseRod==NULL)
	{
		CLDSNode *pBaseNode=console.FromNodeHandle(base_plate.m_hBaseNode);
		pBasePlate->CopyModuleInstanceInfo(pBaseNode);
		pBasePlate->SetDesignItemValue('Z',base_plate.m_iVectZType,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_INTEGER);
	}
	else if(pBaseRod->GetClassTypeId()==CLS_LINETUBE)
		pBasePlate->CopyModuleInstanceInfo(pBaseRod);
	else if(pBaseRod->IsAngle())
	{	//调整角钢正负头
		pBasePlate->CopyModuleInstanceInfo(pBaseRod);
		CLDSLineAngle* pMainJg=(CLDSLineAngle*)pBaseRod;
		UpdateLinkAngleOddment(base_plate,pMainJg);
	}
	if(pBaseRod==NULL||pBaseRod->IsAngle())
		pBasePlate->ucs.axis_z=CStdVector::GetVector(base_plate.m_iVectZType);
	else //if(!pBaseRod->IsAngle())
	{
		if(pBaseNode==pBaseRod->pStart)
			pBasePlate->ucs.axis_z=pBaseRod->Start()-pBaseRod->End();
		else
			pBasePlate->ucs.axis_z=pBaseRod->End()-pBaseRod->Start();
		normalize(pBasePlate->ucs.axis_z);
	}
		
	pBasePlate->SetDesignItemValue('A',base_plate.m_hBaseNode,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);
	pBasePlate->SetDesignItemValue('L',base_plate.m_hBaseRod,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE);
	pBasePlate->SetDesignItemValue('N',base_plate.m_nNormOffset,NULL,PARAM_POS);	//法线偏移
	pBasePlate->SetDesignItemValue('S',base_plate.m_cShape,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER);		//底板外形
	if(base_plate.m_cShape==0)	//方形
	{
		pBasePlate->SetDesignItemValue(KEY2C("XA"),base_plate.XA,NULL,PARAM_SHAPE);
		pBasePlate->SetDesignItemValue(KEY2C("XB"),base_plate.XB,NULL,PARAM_SHAPE);
		pBasePlate->SetDesignItemValue(KEY2C("YA"),base_plate.YA,NULL,PARAM_SHAPE);
		pBasePlate->SetDesignItemValue(KEY2C("YB"),base_plate.YB,NULL,PARAM_SHAPE);
		for(int i=0;i<4;i++)
		{
			pBasePlate->SetDesignItemValue(KEY2C(CXhChar16("X%d",i)),base_plate.XC[i],NULL,PARAM_SHAPE);
			pBasePlate->SetDesignItemValue(KEY2C(CXhChar16("Y%d",i)),base_plate.YC[i],NULL,PARAM_SHAPE);
		}
	}
	else	//圆形
		pBasePlate->SetDesignItemValue(KEY2C("R"),base_plate.m_fR,NULL,PARAM_SHAPE);	//半径
	if(pBasePlate->DesignPlate())
	{
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		pBasePlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pBasePlate->GetSolidPartObject());
		g_pSolidDraw->AddCS(pBasePlate->handle,pBasePlate->ucs);
		g_pSolidDraw->SetEntSnapStatus(pBasePlate->handle);
		g_pSolidDraw->Draw();
	}
	return pBasePlate;
}
//设计底板
#include "AddDiJiaoLsDlg.h"
int CLDSView::DesignBasePlate()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->Draw();
	CString cmdStr,cmdValue;
	BASEPLATE base_plate;
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	CUndoOperObject undo(&Ta,true);
	CLDSParamPlate* pBasePlate=NULL;
	//选择底板的依赖节点
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Please select dependent nodes on base plate:");
#else
	cmdStr.Format("BasePlate 请选择底座板的依赖节点:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	CLDSNode *pBaseNode=NULL;
	while(1)
	{
		f3dPoint* point;
		int ret=g_pSolidSnap->SnapPoint(point);
		if(ret<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		else if(ret>0)
		{
			pBaseNode=console.FromNodeHandle(point->ID);
			if(pBaseNode)
				break;
		}
	}
	base_plate.m_hBaseNode=pBaseNode->handle;
	//选择基本依赖杆件
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("select and confirm the leg angle connected with base plate:");
#else
#ifdef __COMPLEX_PART_INC_
	cmdStr.Format("BasePlate 请选择底座板所连接的关联杆件:");
#else
	cmdStr.Format("BasePlate 请选择底座板所连接的关联角钢:");
#endif
#endif	
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pBaseNode->handle));
	pCmdLine->FillCmdLine(cmdStr,"");
	//2.选择确认底板所连接的主材角钢
	CSuperSmartPtr<CLDSLinePart> pRod,pBaseRod;
	CLDSLineAngle *pAngle=NULL,*pBaseAngle=NULL;
	CLDSPart* pPart=console.DispPartSet.FromHandle(pBaseNode->hFatherPart);
	if(pPart&&pPart->IsLinePart())
		pBaseRod=(CLDSLinePart*)pPart;
	else //if(pBaseRod.IsNULL())
	{
		for(pRod=Ta.EnumRodFirst();pRod.IsHasPtr();pRod=Ta.EnumRodNext())
		{
			if(pRod->m_bVirtualPart)
				continue;	//跳过虚拟辅助角钢
			if(pRod->IsAngle()&&pRod->GetClassTypeId()==CLS_LINEANGLE&&pRod.LineAnglePointer()->group_father_jg_h>0)
				continue;	//跳过组合角钢的子角钢
			if(pRod->pStart==pBaseNode || pRod->pEnd==pBaseNode)
			{
				if((pRod->layer(0)=='S'||pRod->layer(0)=='L')&&pRod->layer(1)=='Z')
				{
					pBaseRod=pRod;
					break;
				}
				else if(pBaseRod.IsNULL()||pRod->GetWidth()>pBaseRod->GetWidth())
					pBaseRod=pRod;
			}
		}
	}
	g_pSolidDraw->ReleaseSnapStatus();
	if(pBaseRod.IsHasPtr())
		g_pSolidDraw->SetEntSnapStatus(pBaseRod->handle);

	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART);
	verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prLine);
	long* id_arr=NULL,retcode=0;
	DWORD dwhObj=0,dwExportFlag=0;
	while(true)
	{
		if((retcode=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
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
		pBaseRod=Ta.FromRodHandle(dwhObj);
		base_plate.m_hBaseRod=dwhObj;
		if(pBaseRod.IsHasPtr() || obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
			break;	//已选中依赖杆件或回车确认不需人依赖杆件
	}
	if(pBaseRod.IsNULL())
	{	//基板没有依赖杆件，确定法线方向
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("please select base_plate norm<1.X+|2.X-|3.Y+|4.Y-|5.Z+|6.Z->[6]: ");
#else
		cmdStr.Format("BasePlate 请确定基板法线方向<1.X+|2.X-|3.Y+|4.Y-|5.Z+|6.Z->[6]: ");
#endif
		while(1)
		{
			pCmdLine->FillCmdLine(cmdStr,"");
			if(!pCmdLine->GetStrFromCmdLine(cmdValue,CCmdLineDlg::KEYWORD_AS_RETURN,"1|2|3|4|5|6"))
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			if(cmdValue.GetLength()==0)
				base_plate.m_iVectZType=6;
			else
				base_plate.m_iVectZType=atoi(cmdValue);
			if(base_plate.m_iVectZType>6 || base_plate.m_iVectZType<1)
				pCmdLine->FillCmdLine(cmdStr,"");
			else
				break;
		}
		pCmdLine->FinishCmdLine();
	}
	else
	{
		g_pSolidDraw->SetEntSnapStatus(pBaseRod->handle);
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pBaseRod->handle));
	}
	pBasePlate=CreateOrUpdateBasePlate(base_plate,pBasePlate);
	//节点在钢板的位置
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Please select node base-plate's shape type<0.square|1.circle>[0]:");
#else
	cmdStr.Format("BasePlate 请确定钢板与节点的相对位置<顶面(T)、底面(B)、中间面(M)或输入节点至底面的法向偏移值>[T]:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdValue,CCmdLineDlg::KEYWORD_AS_RETURN,"T|B|M"))
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		if(cmdValue.GetLength()==0 || toupper(cmdValue[0])=='T')
		{
			base_plate.m_cNormOffsetType=0;
			break;
		}
		else if(toupper(cmdValue[0])=='B')
		{
			base_plate.m_cNormOffsetType=1;
			break;
		}
		else if(toupper(cmdValue[0])=='M')
		{
			base_plate.m_cNormOffsetType=2;
			break;
		}
		if(atoi(cmdValue)==0&&cmdValue[0]!='0')
			pCmdLine->FillCmdLine(cmdStr,"");
		else
		{
			base_plate.m_cNormOffsetType=3;
			base_plate.m_nNormOffset=atoi(cmdValue);
			break;
		}
	}
	pBasePlate=CreateOrUpdateBasePlate(base_plate,pBasePlate);
	//选择底板外形类型
	char cShape='1';
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Please select base-plate's shape type<Square(1)|Rectangle(2)|Circle(3)>[1]:");
#else
	cmdStr.Format("BasePlate 请指定基板外形类型<方形(1)|矩形(2)|圆形(3)>[1]:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdValue,CCmdLineDlg::KEYWORD_AS_RETURN,"1|2|3"))
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		if(cmdValue.GetLength()>0)
			cShape=toupper(cmdValue[0]);
		if(cShape!='3')
			base_plate.m_cShape=0;
		else
			base_plate.m_cShape=1;
		if(base_plate.m_cShape>1 || base_plate.m_cShape<0)
			pCmdLine->FillCmdLine(cmdStr,"");
		else
			break;
	}
	//输入外形参数
	if(cShape=='1')
	{	//方形底板
		int nLen=700;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("Input base-plate's edge length[%d]:",nLen);
#else 
		cmdStr.Format("BasePlate 请指定方形钢板边长[%d]:",nLen);
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
			if(nLen<=0)
				pCmdLine->FillCmdLine(cmdStr,"");
			else
				break;
		}
		base_plate.XA=base_plate.XB=base_plate.YA=base_plate.YB=(int)(nLen*0.5);
		pBasePlate=CreateOrUpdateBasePlate(base_plate,pBasePlate);
	}
	else if(cShape=='2')
	{	//矩形
		int nLen=350,nWidth=350;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("Input base-plate's length(by axis X+)[%d]:",nLen);
#else 
		cmdStr.Format("BasePlate 请指定方形钢板X+轴向长[%d]:",nLen);
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
			if(nLen<=0)
				pCmdLine->FillCmdLine(cmdStr,"");
			else
				break;
		}
		base_plate.XA=(int)nLen;
		pBasePlate=CreateOrUpdateBasePlate(base_plate,pBasePlate);
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("Input base-plate's length(by axis X-)[%d]:",nLen);
#else 
		cmdStr.Format("BasePlate 请指定方形钢板X-轴向长[%d]:",nLen);
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
			if(nLen<=0)
				pCmdLine->FillCmdLine(cmdStr,"");
			else
				break;
		}
		base_plate.XB=(int)nLen;
		if(base_plate.XB!=base_plate.XA)
			pBasePlate=CreateOrUpdateBasePlate(base_plate,pBasePlate);
		nWidth=nLen;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("Input base-plate's width(by axis Y+)[%d]:",nWidth);
#else 
		cmdStr.Format("BasePlate 请指定方形钢板Y+轴向长[%d]:",nWidth);
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
				nWidth=atoi(cmdValue);
			if(nWidth<=0)
				pCmdLine->FillCmdLine(cmdStr,"");
			else
				break;
		}
		base_plate.YA=(int)nWidth;
		pBasePlate=CreateOrUpdateBasePlate(base_plate,pBasePlate);
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("Input base-plate's width(by axis Y-)[%d]:",nWidth);
#else 
		cmdStr.Format("BasePlate 请指定方形钢板Y-轴向长[%d]:",nWidth);
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
				nWidth=atoi(cmdValue);
			if(nWidth<=0)
				pCmdLine->FillCmdLine(cmdStr,"");
			else
				break;
		}
		base_plate.YB=(int)nWidth;
		if(base_plate.YB!=base_plate.YA)
			pBasePlate=CreateOrUpdateBasePlate(base_plate,pBasePlate);
	}
	else
	{	//圆形底板
		base_plate.m_nThick=min(ftoi(pBaseRod->GetThick()),6);
		if(base_plate.m_nThick%2>0)
			base_plate.m_nThick+=1;
		double radius=base_plate.m_fR=200;
		if(pBaseRod.IsHasPtr()&&pBaseRod->GetClassTypeId()==CLS_LINETUBE&&(
			(pBaseRod->pStart&&pBaseRod->pStart->handle==base_plate.m_hBaseNode)||
			(pBaseRod->pEnd&&pBaseRod->pEnd->handle==base_plate.m_hBaseNode)))
			base_plate.m_fR=radius=pBaseRod.pTube->GetDiameter()*0.5;	//基板为基准杆件端节点时，默认为钢管端头板
		pBasePlate=CreateOrUpdateBasePlate(base_plate,pBasePlate);
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("Please select base-plate's radius[%g]:",radius);
#else 
		cmdStr.Format("BasePlate 请指定圆形钢板半径[%g]:",radius);
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
				radius=atoi(cmdValue);
			if(radius<0)
				pCmdLine->FillCmdLine(cmdStr,"");
			else
				break;
		}
		if(base_plate.m_fR!=radius)
		{
			base_plate.m_fR=radius;
			pBasePlate=CreateOrUpdateBasePlate(base_plate,pBasePlate);
		}
	}
	//输入底板厚度
	if(base_plate.m_cShape==0)
		base_plate.m_nThick=QueryDiBanParaByWide(base_plate.XA*2,'T');
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Please input base-plate's thick[%d]:",base_plate.m_nThick);
#else 
	cmdStr.Format("BasePlate 请输入钢板厚度[%d]:",base_plate.m_nThick);
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
			base_plate.m_nThick=atoi(cmdValue);
		if(base_plate.m_nThick<=0)
			pCmdLine->FillCmdLine(cmdStr,"");
		else
			break;
	}
	g_pSolidDraw->ReleaseSnapStatus();
	pBasePlate=CreateOrUpdateBasePlate(base_plate,pBasePlate);
	//底板螺栓设计
	BOOL bDesignHole=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Please design base-plate's holes<Yes(Y)|No(N)>[N]:");
#else 
	cmdStr.Format("BasePlate 是否添加底座板螺栓<是(Y)|否(N)>[N]:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	if(!pCmdLine->GetStrFromCmdLine(cmdValue,CCmdLineDlg::KEYWORD_AS_RETURN,"Y|N"))
	{
		pCmdLine->CancelCmdLine();
		return 0;
	}
	bDesignHole=cmdValue.GetLength()==0 ? FALSE : toupper(cmdValue[0])=='Y';
	if(bDesignHole)
	{
		CAddDiJiaoLsDlg addHoledlg;
		addHoledlg.m_fHoleD=QueryDiBanParaByWide(base_plate.XA*2,'D');
		if(base_plate.m_cShape==1)
		{	//圆塔脚默认从22.5°布螺栓孔
			addHoledlg.m_bEnableRayAngleMode=true;
			addHoledlg.m_fY=22.5;
			addHoledlg.m_fX=QueryDiBanParaByWide(base_plate.XA*2,'S');
			addHoledlg.m_nMirN=8;
		}
		else
		{
			addHoledlg.m_bEnableRayAngleMode=false;
			addHoledlg.m_fY=addHoledlg.m_fX=QueryDiBanParaByWide(base_plate.XA*2,'S');
			addHoledlg.m_nMirN=4;
		}
		if(addHoledlg.DoModal()!=IDOK)
			return 0;
		double len,angle;
		if(addHoledlg.m_bEnableRayAngleMode)
		{
			len=fabs(addHoledlg.m_fX);
			angle=22.5*RADTODEG_COEF;
		}
		else
		{
			f3dPoint center(addHoledlg.m_fX,addHoledlg.m_fY,0);
			len=center.mod();
			angle=ACOS(addHoledlg.m_fX/len);
			if(addHoledlg.m_fY<0)
				angle*=-1.0;	//Y坐标为负 起始角也应该取负
		}
		for(int i=0;i<addHoledlg.m_nMirN;i++)
		{
			f3dPoint pt=GetPtInPolar(f3dPoint(0,0,0),angle+2*i*Pi/addHoledlg.m_nMirN,len);
			long d=atoi(addHoledlg.m_sLsGuiGe);
			long hole_d=ftoi(addHoledlg.m_fHoleD);
			CLDSBolt *pRealLs =(CLDSBolt*)Ta.Parts.append(CLS_BOLT);
			pRealLs->m_bVirtualPart=TRUE;
			pRealLs->m_cFuncType=CLDSBolt::FUNC_SETUPHOLE;	//装配孔
			pRealLs->set_norm(pBasePlate->ucs.axis_z);
			pRealLs->des_work_norm.norm_style=3;
			pRealLs->des_work_norm.hVicePart=pBasePlate->handle;
			pRealLs->des_work_norm.direction=0;
			pRealLs->set_d(hole_d);
			pRealLs->hole_d_increment = hole_d-d;
			pRealLs->AddL0Thick(pBasePlate->thick,FALSE);
			f3dPoint dijiaols_pos(pt.x,pt.y);
			coord_trans(dijiaols_pos,pBasePlate->ucs,TRUE);
			pRealLs->ucs.origin=dijiaols_pos;
			pRealLs->cfgword=pBasePlate->cfgword;	//调整螺栓配材号与基准构件配材号一致
			//填充螺栓位置设计参数
			pRealLs->des_base_pos.cLocationStyle=DESIGN_LS_POS::LOCAL_COORD;
			pRealLs->des_base_pos.wing_offset_dist=pt.y;
			pRealLs->des_base_pos.len_offset_dist=pt.x;
			pRealLs->des_base_pos.datumPoint.datum_pos_style=9;
			pRealLs->des_base_pos.datumPoint.des_para.hPart=pBasePlate->handle;
			pRealLs->des_base_pos.datumPoint.datum_pos.Set(pt.x,pt.y,0);
			pRealLs->des_base_pos.hPart=pBasePlate->handle;
			pBasePlate->AppendLsRef(pRealLs->GetLsRef());
		}
		pBasePlate->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBasePlate->GetSolidPartObject());
		g_pSolidDraw->AddCS(pBasePlate->handle,pBasePlate->ucs);
		g_pSolidDraw->Draw();
	}
	return 1;
}
#endif
