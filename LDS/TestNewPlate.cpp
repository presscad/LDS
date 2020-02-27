#include "stdafx.h"
#include "..\LDS.h"
#include "..\env_def.h"
#include "..\Query.h"
#include "..\GlobalFunc.h"
#include "..\XInsertPlateIntersTubeDlg.h"
//#include "DesignJdb.h"
//#include "MirTaAtom.h"
#include "Tower.h"
#include "..\XhCharString.h"
#include "SortFunc.h"
#include "LogFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __ALFA_TEST_VERSION_

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
//布置钢管插板上的螺栓和附加板
extern CXInsertPlateIntersTubeDlg inters_tube_dlg;
BOOL Test_LayoutInsertPlateBoltAndAttachPlate(CLDSParamPlate *pParamPlate,char* sPartNoP,
										 char* sPartNoX,BOOL bUseLineAngle,char *sJgGuige)
{
	//if(nBoltNum%2)	//保证螺栓个数为偶数
	//	nBoltNum+=1;
	double A=0,N=0,M=0,R=0,L=0,H=0,S=0,W=0;
	if(pParamPlate->m_bStdPart)
	{	
		CLDSStdPartModel stdPartModel;
		if(!pParamPlate->GetStdPartModel(&stdPartModel))
			return FALSE;
		//生成标准构件上的螺栓
		stdPartModel.CreateBoltInPlate(pParamPlate);
		//绘制标准构件上的螺栓
		for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
			g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
		A=stdPartModel.param.insert_plate_param.A;
		N=stdPartModel.param.insert_plate_param.N;
		M=stdPartModel.param.insert_plate_param.M;
		R=stdPartModel.param.insert_plate_param.R;
		L=stdPartModel.param.insert_plate_param.L;
		H=stdPartModel.param.insert_plate_param.H;
		S=stdPartModel.param.insert_plate_param.S;
		W=stdPartModel.param.insert_plate_param.W;
	}
	else
	{
		pParamPlate->GetDesignItemValue('A',&A);
		pParamPlate->GetDesignItemValue('N',&N);
		pParamPlate->GetDesignItemValue('M',&M);
		pParamPlate->GetDesignItemValue('R',&R);
		pParamPlate->GetDesignItemValue('L',&L);
		pParamPlate->GetDesignItemValue('H',&H);
		pParamPlate->GetDesignItemValue('S',&S);
		pParamPlate->GetDesignItemValue('W',&W);
		//布置插板上的螺栓
		logto.Log("error");
		inters_tube_dlg.CreateBolt(pParamPlate);
	}
	
	double x=A+M;
	double y=R;
	double z=0.5*pParamPlate->GetThick();
	CLDSPlate *pUpVertPlate=NULL,*pDownVertPlate=NULL; //Z正方向立板  Z负方向立板
	if(pParamPlate->m_iParamType==TYPE_XEND)
	{	//生成十字板上的附加板并且加入到附加构件列表中
		CString sPartNo;
		//十字插板需要布置立板
		//十字插板立板法线设计基准构件为基板			//wht 10-03-06
		//X-Y平面上的包板及螺栓法线设计基准构件为基板
		//X-Z平面上的包板及螺栓法线设计基准构件为对应的立板,立板法线修改后包板及螺栓法线能及时更新
		for(int i=0;i<2;i++)
		{
			CLDSPlate *pVertPlate=(CLDSPlate*)Ta.Parts.append(CLS_PLATE);
			pVertPlate->iSeg=pParamPlate->iSeg;
			pVertPlate->cfgword=pParamPlate->cfgword;
			pVertPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
			pVertPlate->cMaterial = pParamPlate->cMaterial;
			pVertPlate->layer(2)=pParamPlate->layer(2); //调整包板的图层名
			pVertPlate->m_hPartWeldParent=pParamPlate->handle;//根据父构件的参数设计包板
			pVertPlate->set_thick(ftoi(pParamPlate->GetThick()));
			pVertPlate->SetPartNo(sPartNoP);
			pVertPlate->jdb_style=5;	//基准构件上相对位置不变的附加板
			//.................................................
			//此处有问题尽量不去使用jdb_style 稍后修改 
			//.................................................
			pVertPlate->designInfo.m_hBasePart=pParamPlate->handle;
			pVertPlate->designInfo.norm.norm_style=4;	//基准构件上的相对方向
			pVertPlate->designInfo.norm.hVicePart=pParamPlate->handle;
			pVertPlate->designInfo.origin.datum_pos_style=9;//基准构件上的相对坐标
			pVertPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
			pVertPlate->m_fNormOffset=-z;
			if(i==0)
			{	//Z正方向的立板
				pUpVertPlate=pVertPlate;	//记录Z正方向立板
				pVertPlate->designInfo.origin.datum_pos.Set(0,0,z);
				pVertPlate->designInfo.norm.nearVector.Set(0,-1,0);
				//引用相应的螺栓到立板上
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{
					if(pLsRef->start1_end2_mid0==31)//引入螺栓到Z正方向立板上
					{
						pVertPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pVertPlate->handle,TRUE);
					}
				}
			}
			else if(i==1)
			{	//Z负方向的立板
				pDownVertPlate=pVertPlate;	//记录Z负方向立板
				pVertPlate->designInfo.origin.datum_pos.Set(0,0,-z);
				pVertPlate->designInfo.norm.nearVector.Set(0,1,0);
				//引用相应的螺栓到立板上
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{
					if(pLsRef->start1_end2_mid0==41)//引入螺栓到Z负方向立板上
					{
						pVertPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pVertPlate->handle,TRUE);
					}
				}
			}
			pVertPlate->DesignSetupUcs();
			pVertPlate->vertex_list.append(PROFILE_VERTEX(0,0,0));
			pVertPlate->vertex_list.append(PROFILE_VERTEX(L,0,0));
			pVertPlate->vertex_list.append(PROFILE_VERTEX(L,0.5*H-z,0));
			pVertPlate->vertex_list.append(PROFILE_VERTEX(0,0.5*H-z,0));
			pVertPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pVertPlate->GetSolidPartObject());
			//将立板添加到附加构件列表中
			pParamPlate->AppendAttachPart(pVertPlate);
		}
		if(bUseLineAngle)
		{	//使用角钢代替包板
			double fAngleY=0.5*pParamPlate->GetThick();	//短角钢定位Y坐标
			double wing_wide=N+R-0.5*pParamPlate->GetThick();
			double wing_thick=W;
			if(sJgGuige)	//根据指定角钢规格生成角钢 wht 11-06-01
				restore_JG_guige(sJgGuige,wing_wide,wing_thick);
			for(i=0;i<4;i++)
			{	//生成四根短角钢代替八块包板
				CLDSLineAngle *pShortJg=(CLDSLineAngle*)Ta.Parts.append(CLS_LINEANGLE);
				pShortJg->iSeg=pParamPlate->iSeg;
				pShortJg->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
				pShortJg->cMaterial = pParamPlate->cMaterial;
				pShortJg->layer(2)=pParamPlate->layer(2); //调整包板的图层名
				//角钢为螺栓连接，不应该有焊接父构件	wht 10-08-02
				//pShortJg->m_hPartWeldParent=pParamPlate->handle;//根据父构件的参数设计包板
				pShortJg->set_wing_thick(wing_thick);	//肢厚
				pShortJg->set_wing_wide(wing_wide);		//肢宽
				sprintf(pShortJg->GetPartNo(),"%s",sPartNoX);
				//位置参数
				pShortJg->pOldDesStartPos->datum_pos_style=15;	//直接指定基点
				pShortJg->pOldDesStartPos->datum_point.datum_pos_style=9;	//指定构件上的相对坐标
				pShortJg->pOldDesStartPos->datum_point.des_para.hPart=pParamPlate->handle;
				pShortJg->pOldDesEndPos->datum_pos_style=15;	//直接指定基点
				pShortJg->pOldDesEndPos->datum_point.datum_pos_style=9;	//指定构件上的相对坐标
				pShortJg->pOldDesEndPos->datum_point.des_para.hPart=pParamPlate->handle;
				//法线参数
				pShortJg->des_norm_x.spec_norm.norm_style=4;//指定构件上的相对方向
				pShortJg->des_norm_x.bByOtherWing=FALSE;
				pShortJg->des_norm_x.bSpecific=TRUE;
				pShortJg->des_norm_y.spec_norm.norm_style=4;//指定构件上的相对方向
				pShortJg->des_norm_y.bByOtherWing=FALSE;
				pShortJg->des_norm_y.bSpecific=TRUE;
				if(i==0)
				{	//X-Y平面: Y负Z上
					pShortJg->pOldDesStartPos->datum_point.datum_pos.Set(x,-fAngleY,z);
					pShortJg->pOldDesEndPos->datum_point.datum_pos.Set(x+S,-fAngleY,z);
					//X肢法线方向随Z正方向侧立板 Y肢法线方向随基板
					pShortJg->des_norm_x.spec_norm.hVicePart=pUpVertPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
					pShortJg->des_norm_y.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,-1);
					//引入螺栓
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y负方向一侧的螺栓 X-Z平面:Z正方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22
							||pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pShortJg->AppendMidLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
						}
					}
				}
				else if(i==1)
				{	//X-Y平面: Y正Z上
					pShortJg->pOldDesStartPos->datum_point.datum_pos.Set(x,fAngleY,z);
					pShortJg->pOldDesEndPos->datum_point.datum_pos.Set(x+S,fAngleY,z);
					//Y肢法线方向随Z负方向侧立板 X肢法线方向随基板
					pShortJg->des_norm_y.spec_norm.hVicePart=pUpVertPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
					pShortJg->des_norm_x.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
					//引入螺栓
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y正方向一侧的螺栓 X-Z平面:Z正方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12
							||pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pShortJg->AppendMidLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
						}
					}
				}
				else if(i==2)
				{	//X-Y平面: Y负Z下
					pShortJg->pOldDesStartPos->datum_point.datum_pos.Set(x,-fAngleY,-z);
					pShortJg->pOldDesEndPos->datum_point.datum_pos.Set(x+S,-fAngleY,-z);
					//X肢法线方向随Z负方向侧立板 Y肢法线方向随基板
					pShortJg->des_norm_y.spec_norm.hVicePart=pDownVertPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
					pShortJg->des_norm_x.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,1);
					//引入螺栓
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y负方向一侧的螺栓 X-Z平面:Z负方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22
							||pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
						{
							pShortJg->AppendMidLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
						}
					}
				} 
				else if(i==3)
				{	//X-Y平面: Y正Z下
					pShortJg->pOldDesStartPos->datum_point.datum_pos.Set(x,fAngleY,-z);
					pShortJg->pOldDesEndPos->datum_point.datum_pos.Set(x+S,fAngleY,-z);
					//Y肢法线方向随Z正方向侧立板 X肢法线方向随基板
					pShortJg->des_norm_x.spec_norm.hVicePart=pDownVertPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
					pShortJg->des_norm_y.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
					//引入螺栓
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y正方向一侧的螺栓 X-Z平面:Z负方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12
							||pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
						{
							pShortJg->AppendMidLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
						}
					}
				}
				pShortJg->ClearFlag();
				pShortJg->CalPosition();
				pShortJg->cal_x_wing_norm();
				pShortJg->cal_y_wing_norm();
				pShortJg->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pShortJg->GetSolidPartObject());
				//将包板添加到附加构件列表中
				pParamPlate->AppendAttachPart(pShortJg);
			}
		}
		else
		{	//包板为普通钢板
			for(i=0;i<8;i++)
			{
				CLDSPlate *pPlate=(CLDSPlate*)Ta.Parts.append(CLS_PLATE);
				pPlate->iSeg=pParamPlate->iSeg;
				pPlate->cfgword=pParamPlate->cfgword;
				pPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
				pPlate->cMaterial = pParamPlate->cMaterial;
				pPlate->layer(2)=pParamPlate->layer(2); //调整包板的图层名
				//包板为螺栓连接，不应该有焊接父构件	wht 10-08-02
				//pPlate->m_hPartWeldParent=pParamPlate->handle;//根据父构件的参数设计包板
				pPlate->set_thick(ftoi(W));
				strcpy(pPlate->GetPartNo(),sPartNoX);
				pPlate->jdb_style=5;	//基准构件上相对位置不变的附加板
				pPlate->designInfo.m_hBasePart=pParamPlate->handle;
				pPlate->designInfo.norm.norm_style=4;	//基准构件上的相对方向
				pPlate->designInfo.origin.datum_pos_style=9;//基准构件上的相对坐标
				if(i==0)
				{	//X-Y平面: Y负Z上
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,-1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,-y,z);
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y负方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==1)
				{	//X-Y平面: Y负Z下
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,-1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,-y,-z);
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y负方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==2)
				{	//X-Y平面: Y正Z上
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,y,z);
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y正方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==3)
				{	//X-Y平面: Y正Z下
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,y,-z);
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Y平面:Y正方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==4)
				{	//X-Z平面: Y下Z正
					pPlate->designInfo.norm.hVicePart=pUpVertPlate->handle;			//上侧立板
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pUpVertPlate->handle;	//上侧立板
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,z);				//y=R z=半个板厚
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Z平面:Z正方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==5)
				{	//X-Z平面: Y上Z正
					pPlate->designInfo.norm.hVicePart=pUpVertPlate->handle;			//上侧立板
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pUpVertPlate->handle;	//上侧立板
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,-z);				//y=R z=半个板厚
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Z平面:Z正方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==6)
				{	//X-Z平面: Y下Z负
					pPlate->designInfo.norm.hVicePart=pDownVertPlate->handle;		//下侧立板
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pDownVertPlate->handle;//下侧立板
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,-z);				//y=R z=半个板厚
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Z平面:Z负方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==7)
				{	//X-Z平面: Y上Z负
					pPlate->designInfo.norm.hVicePart=pDownVertPlate->handle;		//下侧立板
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pDownVertPlate->handle;//下侧立板
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,z);				//y=R z=半个板厚
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Z平面:Z负方向一侧的螺栓
						if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				pPlate->DesignSetupUcs();
				pPlate->vertex_list.append(PROFILE_VERTEX(0,0,0));
				pPlate->vertex_list.append(PROFILE_VERTEX(S,0,0));
				pPlate->vertex_list.append(PROFILE_VERTEX(S,N,0));
				pPlate->vertex_list.append(PROFILE_VERTEX(0,N,0));
				pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				//将包板添加到附加构件列表中
				pParamPlate->AppendAttachPart(pPlate);
			}
		}
	}
	for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
	{	
		if(pParamPlate->m_iParamType==TYPE_XEND) 
		{
			//将螺栓引入立板/包板后调整螺栓法向偏移量
			(*pLsRef)->des_base_pos.norm_offset.AddThick(-(long)W);	
			//调整十字插板螺栓法线设计参数以及位置设计参数	wht 10-03-06
			if(pUpVertPlate&&pDownVertPlate)
			{	//X-Z平面:Z正方向一侧的螺栓
				if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
				{
					(*pLsRef)->des_work_norm.norm_style=4;	//基准构件上的相对方向
					(*pLsRef)->des_work_norm.nearVector.Set(0,0,1);
					(*pLsRef)->des_work_norm.hVicePart=pUpVertPlate->handle;
					//调整位置参数
					(*pLsRef)->des_base_pos.hPart=pUpVertPlate->handle;
					double wing_offset_dist=fabs((*pLsRef)->des_base_pos.wing_offset_dist);
					double len_offset_dist=fabs((*pLsRef)->des_base_pos.len_offset_dist);
					(*pLsRef)->des_base_pos.wing_offset_dist=wing_offset_dist-z; // z=半个板厚
					(*pLsRef)->des_base_pos.len_offset_dist=len_offset_dist;
				}
				//X-Z平面:Z负方向一侧的螺栓
				else if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
				{
					(*pLsRef)->des_work_norm.norm_style=4;	//基准构件上的相对方向
					(*pLsRef)->des_work_norm.nearVector.Set(0,0,1);
					(*pLsRef)->des_work_norm.hVicePart=pDownVertPlate->handle;
					//调整位置参数
					(*pLsRef)->des_base_pos.hPart=pDownVertPlate->handle;
					double wing_offset_dist=fabs((*pLsRef)->des_base_pos.wing_offset_dist);
					double len_offset_dist=fabs((*pLsRef)->des_base_pos.len_offset_dist);
					(*pLsRef)->des_base_pos.wing_offset_dist=wing_offset_dist-z; // z=半个板厚
					(*pLsRef)->des_base_pos.len_offset_dist=len_offset_dist;
				}
			}
		}
		(*pLsRef)->correct_pos();
		(*pLsRef)->CalGuigeAuto();
		(*pLsRef)->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
	}
	return TRUE;
}

#endif
#endif