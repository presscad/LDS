//<LOCALE_TRANSLATE BY hxr /> 2015-04-17
// lds_co_part.cpp: implementation of the CJoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdio.h"
#include "Tower.h"
#include "CreateFace.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef __COMMON_PART_INC_
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CFoot Class
//////////////////////////////////////////////////////////////////////
// Foot.cpp: implementation of the CFoot class.
//
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFoot::CFoot()
{
	attach_jg_handle[0]=attach_jg_handle[1]=0;		//所依附基础角钢的句柄
	xie_jg_handle[0]=xie_jg_handle[1]=xie_jg_handle[2]=xie_jg_handle[3]=0;		//所基础角钢上所连接的两根斜材的句柄
	base_node_handle=0;
	A=B=C=0;				//靴板前侧台阶高/靴板顶边距角钢外皮的边距/基础角钢底端最低点距底板间距
	lj_by_weld=FALSE;		//靴板角钢连接类型：TRUE:对焊 FALSE:螺栓连接(默认值必须为False,否则与CFootDesignDlg::m_iLjStyle默认值不一致)
	joint_in_zhun = TRUE;
	plank_list = new ATOM_LIST<THANDLE>;
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
}

CFoot::~CFoot()
{
	delete plank_list;
}

BOOL CFoot::CreateRefAxis()	//根据靴板结构得到相对坐标系
{
	CLDSLineAngle *pJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(attach_jg_handle[0],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	CLDSLineAngle *pJg2=(CLDSLineAngle*)BelongModel()->FromPartHandle(attach_jg_handle[1],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	CLDSNode *pNode = BelongModel()->FromNodeHandle(base_node_handle);
	if(pJg==NULL||pNode==NULL)
		return FALSE;
	//计算底脚板相对坐标系原点
	JGZJ JgZhunJuX,JgZhunJuY;
	double wing_wide,wing_thick;
	wing_wide = pJg->GetWidth();
	wing_thick = pJg->GetThick();
	getjgzj(JgZhunJuX,wing_wide);
	JgZhunJuY=JgZhunJuX;
	if(pJg->m_bEnableTeG)
	{
		JgZhunJuX=pJg->xWingXZhunJu;
		JgZhunJuY=pJg->xWingYZhunJu;
	}
	f3dPoint JgBer;
	BOOL bHorizOffset=FALSE;
	if(pJg->pStart->handle==base_node_handle)
	{
		ucs.origin = pJg->pStart->Position(true);
		JgBer=pJg->Start();
		if(pJg->desStartPos.IsFaceOffset())//.datum_to_ber_style==1)
			bHorizOffset=TRUE;
	}
	else
	{
		ucs.origin = pJg->pEnd->Position(true);
		JgBer=pJg->End();
		if(pJg->desEndPos.IsFaceOffset())//des_end_pos.datum_to_ber_style==1)
			bHorizOffset=TRUE;
	}
	if(pNode->Position(true).x>=0&&pNode->Position(true).y>=0)
	{	//第一象限
		ucs.axis_x.Set(1,0,0);
		ucs.axis_y.Set(0,-1,0);
		ucs.axis_z.Set(0,0,-1);
	}
	else if(pNode->Position(true).x>=0&&pNode->Position(true).y<=0)
	{	//第三象限
		ucs.axis_x.Set(0,-1,0);
		ucs.axis_y.Set(-1,0,0);
		ucs.axis_z.Set(0,0,-1);
	}
	else if(pNode->Position(true).x<=0&&pNode->Position(true).y<=0)
	{	//第四象限
		ucs.axis_x.Set(-1,0,0);
		ucs.axis_y.Set(0,1,0);
		ucs.axis_z.Set(0,0,-1);
	}
	else
	{	//第二象限
		ucs.axis_x.Set(0,1,0);
		ucs.axis_y.Set(1,0,0);
		ucs.axis_z.Set(0,0,-1);
	}
	if(!(pJg->GetClassTypeId()==CLS_GROUPLINEANGLE||(pJg2&&pJg2->GetClassTypeId()==CLS_GROUPLINEANGLE)))	//不是组合角钢时原点在半角钢心距处
	{
		f3dPoint vertex1,vertex2,jg_vec;
		jg_vec = pJg->End()-pJg->Start();
		if(bHorizOffset)
		{
			f3dLine line;
			line.startPt=pJg->Start()+pJg->GetWingVecX()*JgZhunJuX.g;
			line.endPt=pJg->End()+pJg->GetWingVecX()*JgZhunJuX.g;
			Int3dlf(vertex1,line,ucs.origin,f3dPoint(0,0,1));
			line.startPt=pJg->Start()+pJg->GetWingVecY()*JgZhunJuY.g;
			line.endPt=pJg->End()+pJg->GetWingVecY()*JgZhunJuY.g;
			Int3dlf(vertex2,line,ucs.origin,f3dPoint(0,0,1));
		}
		else
		{
			vertex1=JgBer+pJg->GetWingVecX()*JgZhunJuX.g;
			vertex2=JgBer+pJg->GetWingVecY()*JgZhunJuY.g;
		}
		ucs.origin=0.5*(vertex1+vertex2);
		double minz=min(vertex1.z,vertex2.z);
		if(fabs(ucs.origin.z-minz)>EPS)	//不是方塔，两肢心线端点不水平，默认以较高者为准
			Int3dlf(ucs.origin,ucs.origin,jg_vec,f3dPoint(0,0,minz),f3dPoint(0,0,1));
	}
	return TRUE;
}

void CFoot::Create3dFoot()
{
	for(CLDSPlate *pPlank=GetFirstPlank();pPlank!=NULL;pPlank=GetNextPlank())
		pPlank->Create3dSolidModel(TRUE);
}
CLDSPlate* CFoot::GetFirstPlank()
{
	THANDLE *ph=plank_list->GetFirst();
	if(ph)
	{
		CLDSPlate *pPlank=(CLDSPlate*)BelongModel()->FromPartHandle(*ph,CLS_PLATE);
		if(pPlank)
			return pPlank;
		else
		{
			plank_list->DeleteCursor();
			return GetFirstPlank();
		}
	}
	else
		return NULL;
}
CLDSPlate* CFoot::GetNextPlank()
{
	THANDLE *ph=plank_list->GetNext();
	if(ph)
	{
		CLDSPlate *pPlank=(CLDSPlate*)BelongModel()->FromPartHandle(*ph,CLS_PLATE);
		if(pPlank)
			return pPlank;
		else
		{
			plank_list->DeleteCursor();
			return GetNextPlank();
		}
	}
	else
		return NULL;
}
CLDSPlate* CFoot::AppendPlank()
{
	CLDSPlate* pPlank=(CLDSPlate*)BelongModel()->AppendPart(CLS_PLATE);
	THANDLE *ph=plank_list->append();
	*ph=pPlank->handle;
	pPlank->designInfo.m_hBaseNode = base_node_handle;
	pPlank->m_bDisplayHole = true;
	pPlank->cMaterial = CSteelMatLibrary::Q345BriefMark();//'H';	//底脚板上板缺省按Q345材质
	return pPlank;
}

void CFoot::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
		return;
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	BelongModel()->EvolveCfgword(cfgword,version,doc_type);
	buffer.ReadDword(&dwPermission);
	buffer.ReadWord(&A);	//靴板前侧台阶高
	buffer.ReadWord(&B);	//靴板顶边距角钢外皮的边距
	buffer.ReadWord(&C);	//基础角钢底端最低点距底板间距
	buffer.ReadInteger(&attach_jg_handle[0]);
	buffer.ReadInteger(&attach_jg_handle[1]);
	buffer.ReadInteger(&xie_jg_handle[0]);
	buffer.ReadInteger(&xie_jg_handle[1]);
	buffer.ReadInteger(&xie_jg_handle[2]);
	buffer.ReadInteger(&xie_jg_handle[3]);
	buffer.ReadInteger(&base_node_handle);
	buffer.ReadInteger(&joint_in_zhun);//斜材交于主材准线,否则交于外皮
	buffer.ReadInteger(&lj_by_weld);	//靴板角钢连接类型：TRUE:对焊 FALSE:螺栓连接
	buffer.ReadPoint(ucs.axis_x);	//靴板相对坐标X轴
	buffer.ReadPoint(ucs.axis_y);	//靴板相对坐标Y轴
	buffer.ReadPoint(ucs.axis_z);	//靴板相对坐标Z轴
	buffer.ReadPoint(ucs.origin);	//靴板相对坐标原点
	long i,n,h;
	buffer.ReadInteger(&n);
	plank_list->Empty();
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		plank_list->append(h);
	}
}
void CFoot::ToBuffer(CBuffer &buffer)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdToBuffer(buffer))
		return;
	buffer.Write(cfgword.flag.word,24);
	buffer.WriteDword(dwPermission);
	buffer.WriteWord(A);	//靴板前侧台阶高
	buffer.WriteWord(B);	//靴板顶边距角钢外皮的边距
	buffer.WriteWord(C);	//基础角钢底端最低点距底板间距
	buffer.WriteInteger(attach_jg_handle[0]);
	buffer.WriteInteger(attach_jg_handle[1]);
	buffer.WriteInteger(xie_jg_handle[0]);
	buffer.WriteInteger(xie_jg_handle[1]);
	buffer.WriteInteger(xie_jg_handle[2]);
	buffer.WriteInteger(xie_jg_handle[3]);
	buffer.WriteInteger(base_node_handle);
	buffer.WriteInteger(joint_in_zhun);//斜材交于主材准线,否则交于外皮
	buffer.WriteInteger(lj_by_weld);	//靴板角钢连接类型：TRUE:对焊 FALSE:螺栓连接
	buffer.WriteDouble(ucs.axis_x.x);	//靴板相对坐标X轴
	buffer.WriteDouble(ucs.axis_x.y);	//靴板相对坐标X轴
	buffer.WriteDouble(ucs.axis_x.z);	//靴板相对坐标X轴
	buffer.WriteDouble(ucs.axis_y.x);	//靴板相对坐标Y轴
	buffer.WriteDouble(ucs.axis_y.y);	//靴板相对坐标Y轴
	buffer.WriteDouble(ucs.axis_y.z);	//靴板相对坐标Y轴
	buffer.WriteDouble(ucs.axis_z.x);	//靴板相对坐标Z轴
	buffer.WriteDouble(ucs.axis_z.y);	//靴板相对坐标Z轴
	buffer.WriteDouble(ucs.axis_z.z);	//靴板相对坐标Z轴
	buffer.WriteDouble(ucs.origin.x);	//靴板相对坐标原点
	buffer.WriteDouble(ucs.origin.y);	//靴板相对坐标原点
	buffer.WriteDouble(ucs.origin.z);	//靴板相对坐标原点
	buffer.WriteInteger(plank_list->GetNodeNum());
	for(THANDLE *ph=plank_list->GetFirst();ph;ph=plank_list->GetNext())
		buffer.WriteInteger(*ph);
}
void CFoot::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<靴板 handle=\"0X%X\">\n",handle);
	fprintf(fp,"<配材号 cfgword.word=\"%d,%d,%d,%d,%d\"/>\n",cfgword.flag.word[0],cfgword.flag.word[1],cfgword.flag.word[2],cfgword.flag.word[3],cfgword.flag.word[4]);
	fprintf(fp,"<权限字 dwPermission=\"%d\"/>\n",dwPermission);
	fprintf(fp,"<靴板前侧台阶高 A=\"%d\"/>\n",A);
	fprintf(fp,"<靴板顶边距角钢外皮的边距 B=\"%d\"/>\n",B);
	fprintf(fp,"<基础角钢底端最低点距底板间距 C=\"%d\"/>\n",C);
	fprintf(fp,"<所依附基础角钢的句柄 attach_jg_handle=\"(0X%X,0X%X)\"/>\n",attach_jg_handle[0],attach_jg_handle[1]);
	fprintf(fp,"<所基础角钢上所连接的两根斜材的句柄 xie_jg_handle=\"(0X%X,0X%X,0X%X,0X%X)\"/>\n",xie_jg_handle[0],xie_jg_handle[1],xie_jg_handle[2],xie_jg_handle[3]);
	fprintf(fp,"<附属节点句柄 base_node_handle=\"0X%X\"/>\n",base_node_handle);
	if (joint_in_zhun)
		fprintf(fp,"<斜材交于主材准线否则交于外皮 joint_in_zhun=\"true\"/>\n");
	else
		fprintf(fp,"<斜材交于主材准线否则交于外皮 joint_in_zhun=\"false\"/>\n");
	if (lj_by_weld)
		fprintf(fp,"<靴板角钢连接类型 lj_by_weld=\"true:对焊\"/>\n");
	else
		fprintf(fp,"<靴板角钢连接类型 lj_by_weld=\"false:螺栓连接\"/>\n");
	fprintf(fp,"<相对坐标系>\n");
	sprintf(sText,"%f",ucs.axis_x.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.axis_x.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.axis_x.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<X轴 ucs.axis_x=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",ucs.axis_y.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.axis_y.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.axis_y.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<Y轴 ucs.axis_y=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",ucs.axis_z.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.axis_z.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.axis_z.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<Z轴 ucs.axis_z=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",ucs.origin.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.origin.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.origin.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<原点 ucs.origin=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	fprintf(fp,"</相对坐标系>\n");
	fprintf(fp,"<板列表>\n");
	fprintf(fp,"<板数 plank_list_Num=\"%d\"/>\n",plank_list->GetNodeNum());
	for(THANDLE *ph=plank_list->GetFirst();ph;ph=plank_list->GetNext())
		fprintf(fp,"<板句柄 handle=\"0X%X\"/>\n",*ph);
	fprintf(fp,"</板列表>\n");
	fprintf(fp,"</靴板>\n");
}
ATOM_LIST<BOLTHOLE_INFO> CLDSPolyPart::boltListX;
ATOM_LIST<BOLTHOLE_INFO> CLDSPolyPart::boltListY;
CLDSPolyPart::CLDSPolyPart()
{
	m_nClassTypeId=CLS_POLYPART;
	//strcpy(m_sClassName,"CLDSPolyPart");
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
}
CLDSPolyPart::~CLDSPolyPart()
{
	if(pSolidBody)
	{
		delete pSolidBody;
		pSolidBody=NULL;
	}
}
double CLDSPolyPart::GetWidth()
{
	if(segset.GetNodeNum()>0)
		return segset.GetFirst()->GetWidth();
	else
		return 0;
}
double CLDSPolyPart::GetThick()
{
	if(segset.GetNodeNum()>0)
		return segset.GetFirst()->GetThick();
	else
		return 0;
}
double CLDSPolyPart::GetLength()
{
	double sumlength=0;
	for(CLDSLinePart *pLinePart=segset.GetFirst();pLinePart;pLinePart=segset.GetNext())
		sumlength+=pLinePart->GetLength();
	return sumlength;
}
double CLDSPolyPart::GetWeight(BOOL bSolidLength/*=TRUE*/)
{
	double piece_weight=0;
	for(CLDSLinePart* pLinePart=segset.GetFirst();pLinePart;pLinePart=segset.GetNext())
		piece_weight+=pLinePart->GetWeight(bSolidLength);
	return piece_weight;
}
int CLDSPolyPart::GetSpecification(char *spec,BOOL bIncMat/*=TRUE*/)
{
	CLDSLinePart *pFirstLinePart=segset.GetFirst();
	return pFirstLinePart->GetSpecification(spec,bIncMat);
}
bool CLDSPolyPart::IsEqual(CLDSPolyPart *pPolyPart,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
						   int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{
	if(segset.GetNodeNum()!=pPolyPart->segset.GetNodeNum())
	{
		if(error_info!=NULL)
		{
			char ss[100];
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(ss,"{%s} sub-seg different",(char*)sPartNo);
#else
			sprintf(ss,"{%s}子构件数量不一致",(char*)sPartNo);
#endif
			strcpy(error_info,ss);
		}
		return false;
	}
	CLDSLinePart *pFirstLinePart=pPolyPart->segset.GetFirst();
	if(pFirstLinePart->GetClassTypeId()==CLS_LINEANGLE&&segset.GetNodeNum()==2)
		return IsEqualPolyAngle(pPolyPart,epsilon,error_info,pErrorInfo,pChkStyleFlag);
	CLDSLinePart *pSeg,*pMirSeg;
	for(pSeg=segset.GetFirst(),pMirSeg=pPolyPart->segset.GetFirst();
		pSeg&&pMirSeg;pSeg=segset.GetNext(),pMirSeg=pPolyPart->segset.GetNext())
	{
		if(!pSeg->IsEqual(pMirSeg,true,epsilon,error_info,pErrorInfo,pChkStyleFlag))
		{
			if(error_info!=NULL)
			{
				char ss[100];
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(ss,"middle sub-seg 0X%X and 0X%X",pSeg->handle,pMirSeg->handle);
#else
				sprintf(ss,"中子段0X%X与0X%X",pSeg->handle,pMirSeg->handle);
#endif
				strcat(ss,error_info);
				strcpy(error_info,ss);
			}
			return false;
		}
	}
	if(pSeg||pMirSeg)
	{
		if(error_info!=NULL)
		{
			char ss[100];
			if(pSeg)
				sprintf(ss,"0X%X",pSeg->handle);
			else
				strcpy(ss,"0x0");
			strcat(error_info,ss);
			if(pMirSeg)
				sprintf(ss,":0X%X",pMirSeg->handle);
			else
				strcpy(ss,":0x0");
			strcat(error_info,ss);
		}
		return false;
	}
	return true;
}
extern void CopyLineAngle(CLDSLineAngle *pDestLineAngle,CLDSLineAngle *pSrcLineAngle,BOOL bReverseStartEnd);
static double GetHuoquAngle(CLDSLineAngle* pPreJg,CLDSLineAngle* pCurJg)
{
	if(pPreJg==NULL || pCurJg==NULL)
		return 0;
	UCS_STRU ucs;
	pPreJg->getUCS(ucs,pPreJg->huoqu_base_wing_x0_y1_both2);
	f3dPoint prev_wing_norm_x=pPreJg->vxWingNorm;
	f3dPoint prev_wing_norm_y=pPreJg->vyWingNorm;
	f3dPoint vec_end=pCurJg->End()-pCurJg->Start();
	normalize(vec_end);
	vector_trans(vec_end,ucs,FALSE);
	double huoqu_angle=0,angle=0,min_huoqu_angle=1;
	if(pPreJg->huoqu_base_wing_x0_y1_both2==0||pPreJg->huoqu_base_wing_x0_y1_both2==2)
	{
		double angle=0;
		f3dPoint wing_norm_x=pCurJg->vxWingNorm;
		if(vec_end.y>0)	//内曲
			angle= ACOS(prev_wing_norm_x*wing_norm_x);
		else
			angle=-ACOS(prev_wing_norm_x*wing_norm_x);
		angle*=DEGTORAD_COEF;
		if(pPreJg->huoqu_base_wing_x0_y1_both2==0)
			huoqu_angle=angle;
		if(pPreJg->huoqu_base_wing_x0_y1_both2==2 && fabs(angle)>min_huoqu_angle)
		{
			pPreJg->huoqu_base_wing_x0_y1_both2=0;
			huoqu_angle=angle;
		}
	}
	if(pPreJg->huoqu_base_wing_x0_y1_both2==1||pPreJg->huoqu_base_wing_x0_y1_both2==2)
	{
		f3dPoint wing_norm_y=pCurJg->vyWingNorm;
		if(vec_end.x>0)	//内曲
			angle= ACOS(prev_wing_norm_y*wing_norm_y);
		else
			angle=-ACOS(prev_wing_norm_y*wing_norm_y);
		angle*=DEGTORAD_COEF;
		if(pPreJg->huoqu_base_wing_x0_y1_both2==1)
			huoqu_angle=angle;
		if(pPreJg->huoqu_base_wing_x0_y1_both2==2 && fabs(angle)>min_huoqu_angle)
		{
			pPreJg->huoqu_base_wing_x0_y1_both2=1;
			huoqu_angle=angle;
		}
	}
	return huoqu_angle;
}
bool CLDSPolyPart::IsEqualPolyAngle(CLDSPolyPart *pPolyJg,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
									int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{
	//检查构件基本信息
	if(pChkStyleFlag==NULL||*pChkStyleFlag&CHKPARTNO_RODINFO)
	{	
		bool bRet=true;
		if(fabs(GetLength()-pPolyJg->GetLength())>epsilon)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}length is different,lack%.1f",(char*)sPartNo,fabs(GetLength()-pPolyJg->GetLength()));
#else
				sprintf(error_info,"{%s}长度不同,差%.1f",(char*)sPartNo,fabs(GetLength()-pPolyJg->GetLength()));
#endif
			bRet=false;
		}
		else if(GetWidth()!=pPolyJg->GetWidth()!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s} wide is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}肢宽不同",(char*)sPartNo);
#endif
				bRet=false;
		}
		else if(GetThick()!=pPolyJg->GetThick()!=0)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}thickness is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}肢厚不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(cMaterial!=pPolyJg->cMaterial)
		{
			if(error_info)
#ifdef	AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}material is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}材质不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		if(!bRet)
		{
			if(pErrorInfo!=NULL)
				*pErrorInfo=CHKPARTNO_RODINFO;
			return false;
		}
	}
	if(pChkStyleFlag==NULL||*pChkStyleFlag&CHKPARTNO_RODBOLTINFO||*pChkStyleFlag&CHKPARTNO_ANGLECUT)
	{
		//检查分段角钢工艺信息,提取生成首尾衔接的临时角钢段数组
		CLDSLineAngle pre_jg,mir_pre_jg,cur_jg,mir_cur_jg;
		CLDSLineAngle* pFirJg=(CLDSLineAngle*)segset.GetFirst();
		CLDSLineAngle* pSecJg=(CLDSLineAngle*)segset.GetTail();
		if(pFirJg->pStart!=NULL&&pFirJg->pStart==pSecJg->pStart)
		{
			CopyLineAngle(&pre_jg,pFirJg,TRUE);
			CopyLineAngle(&cur_jg,pSecJg,FALSE);
		}
		else if(pFirJg->pStart!=NULL&&pFirJg->pStart==pSecJg->pEnd)
		{
			CopyLineAngle(&pre_jg,pFirJg,TRUE);
			CopyLineAngle(&cur_jg,pSecJg,TRUE);
		}
		else if(pFirJg->pEnd!=NULL&&pFirJg->pEnd==pSecJg->pStart)
		{
			CopyLineAngle(&pre_jg,pFirJg,FALSE);
			CopyLineAngle(&cur_jg,pSecJg,FALSE);
		}
		else if(pFirJg->pEnd!=NULL&&pFirJg->pEnd==pSecJg->pEnd)
		{
			CopyLineAngle(&pre_jg,pFirJg,FALSE);
			CopyLineAngle(&cur_jg,pSecJg,TRUE);
		}
		CLDSLineAngle* pMirFirJg=(CLDSLineAngle*)pPolyJg->segset.GetFirst();
		CLDSLineAngle* pMirSecJg=(CLDSLineAngle*)pPolyJg->segset.GetTail();
		if(pMirFirJg->pStart!=NULL&&pMirFirJg->pStart==pMirSecJg->pStart)
		{
			CopyLineAngle(&mir_pre_jg,pMirFirJg,TRUE);
			CopyLineAngle(&mir_cur_jg,pMirSecJg,FALSE);
		}
		else if(pMirFirJg->pStart!=NULL&&pMirFirJg->pStart==pMirSecJg->pEnd)
		{
			CopyLineAngle(&mir_pre_jg,pMirFirJg,TRUE);
			CopyLineAngle(&mir_cur_jg,pMirSecJg,TRUE);
		}
		else if(pMirFirJg->pEnd!=NULL&&pMirFirJg->pEnd==pMirSecJg->pStart)
		{
			CopyLineAngle(&mir_pre_jg,pMirFirJg,FALSE);
			CopyLineAngle(&mir_cur_jg,pMirSecJg,FALSE);
		}
		else if(pMirFirJg->pEnd!=NULL&&pMirFirJg->pEnd==pMirSecJg->pEnd)
		{
			CopyLineAngle(&mir_pre_jg,pMirFirJg,FALSE);
			CopyLineAngle(&mir_cur_jg,pMirSecJg,TRUE);
		}
		//比较制弯角钢的火曲角度	
		double huoqu_angle1=GetHuoquAngle(&pre_jg,&cur_jg);
		double huoqu_angle2=GetHuoquAngle(&mir_pre_jg,&mir_cur_jg);
		if(fabs(huoqu_angle1-huoqu_angle2)>EPS2)
		{
			if(error_info)
#ifdef	AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s} huoqu angle different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}火曲角度不一致",(char*)sPartNo);
#endif	
			if(pErrorInfo!=NULL)
				*pErrorInfo=CHKPARTNO_RODINFO;
			return false;
		}
		//进行正序比较（1-1，2-2）
		if(!IsEqualSegAngle(&pre_jg,&mir_pre_jg,FALSE,epsilon,NULL,NULL,NULL)||
			!IsEqualSegAngle(&cur_jg,&mir_cur_jg,FALSE,epsilon,NULL,NULL,NULL))
			goto REVERSE_ORDER;
		if(pre_jg.huoqu_base_wing_x0_y1_both2!=mir_pre_jg.huoqu_base_wing_x0_y1_both2)
		{
			if(error_info)
#ifdef	AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}huoqu base wing different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}火曲肢不一致",(char*)sPartNo);
#endif
			if(pErrorInfo!=NULL)
				*pErrorInfo=CHKPARTNO_RODINFO;
			return false;
		}
		return true;
		//进行反序比较（1-2，2-1）
REVERSE_ORDER:
		CXhChar100 errorStr;
		if(!IsEqualSegAngle(&pre_jg,&mir_cur_jg,TRUE,epsilon,errorStr,pErrorInfo,pChkStyleFlag)||
			!IsEqualSegAngle(&cur_jg,&mir_pre_jg,TRUE,epsilon,errorStr,pErrorInfo,pChkStyleFlag))
		{
			if(error_info)
				sprintf(error_info,errorStr);
			return false;
		}
		if(pre_jg.huoqu_base_wing_x0_y1_both2+mir_pre_jg.huoqu_base_wing_x0_y1_both2!=1&&
			pre_jg.huoqu_base_wing_x0_y1_both2+mir_pre_jg.huoqu_base_wing_x0_y1_both2!=4)
		{
			if(error_info)
#ifdef	AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}huoqu base wing different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}火曲肢不一致",(char*)sPartNo);
#endif
			if(pErrorInfo!=NULL)
				*pErrorInfo=CHKPARTNO_RODINFO;
			return false;
		}
	}
	return true;
}
bool CLDSPolyPart::IsEqualSegAngle(CLDSLineAngle *pSegJg,CLDSLineAngle *pMirSegJg,BOOL bReverse/*=FALSE*/,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
								int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{
	if(pChkStyleFlag==NULL||*pChkStyleFlag&CHKPARTNO_RODBOLTINFO)
	{	//检查螺栓信息是否相同
		if(pSegJg->GetLsRefList()->GetNodeNum()!=pMirSegJg->GetLsRefList()->GetNodeNum())
		{
			if(error_info)
#ifdef	AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}bolt number is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}螺栓数量不同",(char*)sPartNo);
#endif
			if(pErrorInfo!=NULL)
				*pErrorInfo=CHKPARTNO_RODBOLTINFO;
			return false;
		}
	}
	//
	CXhChar100 errorStr;
	UCS_STRU ucs,tem_ucs,vice_ucs,tem_vice_ucs;
	if(bReverse==FALSE)
	{	//正比
		pSegJg->getUCS(ucs,2);
		pMirSegJg->getUCS(vice_ucs,2);
		tem_ucs = ucs;
		tem_vice_ucs = vice_ucs;
		Sub_Pnt(tem_ucs.origin,ucs.origin,ucs.axis_z*pSegJg->startOdd());
		Sub_Pnt(tem_vice_ucs.origin,vice_ucs.origin,vice_ucs.axis_z*pMirSegJg->startOdd());
		if(pChkStyleFlag==NULL||*pChkStyleFlag&CHKPARTNO_RODBOLTINFO)
		{
			if(!IsLsRefListEqual(pSegJg->GetLsRefList(),tem_ucs,pMirSegJg->GetLsRefList(),tem_vice_ucs,epsilon,errorStr,NULL))
			{
				if(error_info)
#ifdef	AFX_TARG_ENU_ENGLISH
					sprintf(error_info,"{%s}proportional:%s",(char*)sPartNo,(char*)errorStr);
#else
					sprintf(error_info,"{%s}正比:%s",(char*)sPartNo,(char*)errorStr);
#endif
				if(pErrorInfo!=NULL)
					*pErrorInfo=CHKPARTNO_RODBOLTINFO;
				return false;
			}
		}
		if(pChkStyleFlag==NULL||*pChkStyleFlag&CHKPARTNO_ANGLECUT)
		{	//检查压扁切角信息是否相同
			for(int i=0;i<2;i++)
			{
				if(pSegJg->cut_wing[i]!=pMirSegJg->cut_wing[i])
				{	
					if(error_info)
#ifdef	AFX_TARG_ENU_ENGLISH
						sprintf(error_info,"{%s}cut leg data is different",(char*)sPartNo); 
#else
						sprintf(error_info,"{%s}切肢数据不同",(char*)sPartNo);
#endif
					if(pErrorInfo)
						*pErrorInfo=CHKPARTNO_ANGLECUT;
					return false;
				}
				if(pSegJg->cut_wing[i]>0)
				{
					for(int j=0;j<3;j++)
					{
						if(fabs(pSegJg->cut_wing_para[i][j]-pMirSegJg->cut_wing_para[i][j])>epsilon)
						{	
							if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
								sprintf(error_info,"{%s}cut leg data is different",(char*)sPartNo);
#else
								sprintf(error_info,"{%s}切肢数据不同",(char*)sPartNo);
#endif
							if(pErrorInfo)
								*pErrorInfo=CHKPARTNO_ANGLECUT;
							return false;
						}
					}
				}
				for(int j=0;j<4;j++)
				{
					if(fabs(pSegJg->cut_angle[j][i]-pMirSegJg->cut_angle[j][i])>epsilon)
					{	
						if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
							sprintf(error_info,"{%s}cut leg data is different",(char*)sPartNo);
#else
							sprintf(error_info,"{%s}切角数据不同",(char*)sPartNo);
#endif
						if(pErrorInfo)
							*pErrorInfo=CHKPARTNO_ANGLECUT;
						return false;
					}
				}
			}
		}
	}
	else
	{	//反比
		pSegJg->getUCS(ucs,2);
		pMirSegJg->getUCS(vice_ucs,2);
		tem_ucs = ucs;
		tem_vice_ucs = vice_ucs;
		Sub_Pnt(tem_ucs.origin,ucs.origin,ucs.axis_z*pSegJg->startOdd());
		Sub_Pnt(tem_vice_ucs.origin,vice_ucs.origin,vice_ucs.axis_z*pMirSegJg->startOdd());
		//反转pSegJg坐标系
		ucs.origin = pSegJg->End();
		f3dPoint vec=ucs.axis_x;
		ucs.axis_x = ucs.axis_y;
		ucs.axis_y = vec;
		ucs.axis_z = -1.0*ucs.axis_z;
		tem_ucs = ucs;
		Sub_Pnt(tem_ucs.origin,ucs.origin,ucs.axis_z*pSegJg->endOdd());
		//
		if(pChkStyleFlag==NULL||*pChkStyleFlag&CHKPARTNO_RODBOLTINFO)
		{	//反比检查螺栓信息是否相同
			if(!IsLsRefListEqual(pSegJg->GetLsRefList(),tem_ucs,pMirSegJg->GetLsRefList(),tem_vice_ucs,epsilon,errorStr,NULL))
			{
				if(error_info)
#ifdef	AFX_TARG_ENU_ENGLISH
					sprintf(error_info,"{%s}inversely proportional:%s",(char*)sPartNo,(char*)errorStr);
#else
					sprintf(error_info,"{%s}反比:%s",(char*)sPartNo,(char*)errorStr);
#endif
				if(pErrorInfo)
					*pErrorInfo=CHKPARTNO_RODBOLTINFO;
				return false;
			}
		}
		if(pChkStyleFlag==NULL||*pChkStyleFlag&CHKPARTNO_ANGLECUT)
		{	//检查切角信息是否相同
			for(int i=0;i<2;i++)
			{
				if(pSegJg->cut_wing[i]+pMirSegJg->cut_wing[1-i]!=0 && pSegJg->cut_wing[i]+pMirSegJg->cut_wing[1-i]!=3)
				{	
					if(error_info)
#ifdef	AFX_TARG_ENU_ENGLISH
						sprintf(error_info,"{%s}cut leg data is different",(char*)sPartNo);
#else
						sprintf(error_info,"{%s}切肢数据不同",(char*)sPartNo);
#endif				
					if(pErrorInfo)
						*pErrorInfo=CHKPARTNO_ANGLECUT;
					return false;
				}
				else if(pSegJg->cut_wing[i]+pMirSegJg->cut_wing[1-i]==3)
				{	
					for(int j=0;j<3;j++)
					{
						if(fabs(pSegJg->cut_wing_para[i][j]-pMirSegJg->cut_wing_para[1-i][j])>2)
						{
							if(error_info)
#ifdef	AFX_TARG_ENU_ENGLISH
								sprintf(error_info,"{%s}cut leg data is different",(char*)sPartNo);
#else
								sprintf(error_info,"{%s}切肢数据不同",(char*)sPartNo);
#endif
							if(pErrorInfo)
								*pErrorInfo=CHKPARTNO_ANGLECUT;
							return false;
						}
					}
				}
				for(int j=0;j<4;j++)
				{
					if(fabs(pSegJg->cut_angle[j][i]-pMirSegJg->cut_angle[3-j][i])>2)
					{
						if(error_info)
#ifdef	AFX_TARG_ENU_ENGLISH
							sprintf(error_info,"{%s}cut leg data is different",(char*)sPartNo);
#else
							sprintf(error_info,"{%s}切角数据不同",(char*)sPartNo);
#endif
						if(pErrorInfo)
							*pErrorInfo=CHKPARTNO_ANGLECUT;
						return false;
					}
				}
			}
		}
	}
	return true;
}
static double CompareBoltHoleList(ARRAY_LIST<BOLTHOLE_INFO> &datumBoltList,ARRAY_LIST<BOLTHOLE_INFO> &viceBoltList,double fAngleLen=0)
{
	if(datumBoltList.GetSize()!=viceBoltList.GetSize())
		return 1.7976931348623158e+308;	//返回最大浮点数
	double fError=0;
	int nCount=datumBoltList.GetSize();
	BOLTHOLE_INFO *pBolt1=NULL,*pBolt2=NULL;
	for(int i=0;i<nCount;i++)
	{
		pBolt1=datumBoltList.GetByIndex(i);
		GEPOINT pos1(pBolt1->posX,pBolt1->posY),pos2;
		if(fAngleLen>0)
		{
			pBolt2=viceBoltList.GetByIndex(nCount-1-i);
			pos2.Set(fAngleLen-pBolt2->posX,pBolt2->posY);
		}
		else
		{
			pBolt2=viceBoltList.GetByIndex(i);
			pos2.Set(pBolt2->posX,pBolt2->posY);
		}
		fError+=DISTANCE(pos1,pos2);
	}
	if(fError>0&&nCount>0)
		fError/=nCount;		//误差均分至单个螺栓孔上
	return fError;
}
static int CompareBoltHolePos(const BOLTHOLE_INFO &item1,const BOLTHOLE_INFO &item2)
{
	if(item1.centre.x>item2.centre.x)
		return 1;
	else if(item1.centre.x<item2.centre.x)
		return -1;
	else if(item1.centre.y>item2.centre.y)
		return 1;
	else if(item1.centre.y<item2.centre.y)
		return -1;
	else 
		return 0;
}
#include "SortFunc.h"
bool CLDSPolyPart::CompareLabelPart(CLDSPart *pPeerPart,MIRMSG* pMirmsg/*=NULL*/,BOOL bCmpls/*=TRUE*/,
										PEER_PART *pPeerInfo/*=NULL*/,double epsilon/*=1.0*/)
{
	if(!pPeerPart->IsPolyPart() || GetClassTypeId()!=pPeerPart->GetClassTypeId())
		return false;
	if(pPeerPart->m_hBlock!=m_hBlock)
		return false;	//从属部件不一致,返回false
	CLDSPolyPart* pPeerPolyPart=(CLDSPolyPart*)pPeerPart;
	if(pPeerInfo)
	{
		pPeerInfo->hPeerPart=pPeerPolyPart->handle;
		pPeerInfo->csBase=GetUnifiedACS();
		pPeerInfo->csPeer=pPeerPart->GetUnifiedACS();
	}
	if(handle==pPeerPolyPart->handle)
		return true;
	//比较基本信息
	if(GetLength()!=pPeerPolyPart->GetLength())
		return false;

	UCS_STRU datumUcs,temUcs1,temUcs2;
	datumUcs=GetUCS(true);
	temUcs1=pPeerPolyPart->GetUCS(true);
	temUcs2=pPeerPolyPart->GetUCS(true,true);
	//比较制弯方向
	typedef LINEPARTSET* RODSET_PTR;
	LINEPARTSET reverseRodSet;
	fPtList segVecList[3];
	UCS_STRU ucs_arr[3]={datumUcs,temUcs1,temUcs2};
	RODSET_PTR rodSetPtr[3]={&segset,&pPeerPolyPart->segset,&reverseRodSet};
	for(CLDSLinePart *pRod=pPeerPolyPart->segset.GetTail();pRod;pRod=pPeerPolyPart->segset.GetPrev())
		reverseRodSet.append(pRod);
	LINEPARTSET *pRodSet=NULL;
	for(int i=0;i<3;i++)
	{
		for(CLDSLinePart *pRod=rodSetPtr[i]->GetFirst();pRod;pRod=rodSetPtr[i]->GetNext())
		{
			f3dPoint len_vec=pRod->End()-pRod->Start();
			normalize(len_vec);
			vector_trans(len_vec,ucs_arr[i],FALSE);
			if(len_vec.z<0)
				len_vec*=-1.0;
			segVecList[i].append(len_vec);
		}
	}
	BOOL vecCmpRetArr[2]={FALSE,FALSE};	
	for(int i=0;i<2;i++)
	{
		int j=0;
		for(j=0;j<segVecList[0].GetNodeNum();j++)
		{
			if(!segVecList[0][j].IsEqual(segVecList[i+1][j],eps))
				break;
		}
		vecCmpRetArr[i]=(j==segVecList[0].GetNodeNum());
	}
	//比较螺栓
	//[0][0]:基准角钢X肢螺栓 [0][1]:基准角钢Y肢螺栓
	//[1][0]:当前角钢X肢螺栓 [0][1]:当前角钢Y肢螺栓
	ARRAY_LIST<BOLTHOLE_INFO> boltHoleList[2][2];	
	for(int i=0;i<2;i++)
	{
		BOLTHOLE_INFO *pBoltHole;
		CLDSPolyPart *pPolyPart=(i==0)?this:pPeerPolyPart;
		CLDSPolyPart::AnaylyzeAngleBolts(pPolyPart);	//统计螺栓时已考虑始端正负头
		for(pBoltHole=pPolyPart->boltListX.GetFirst();pBoltHole;pBoltHole=pPolyPart->boltListX.GetNext())
			boltHoleList[i][0].append(*pBoltHole);
		for(pBoltHole=pPolyPart->boltListY.GetFirst();pBoltHole;pBoltHole=pPolyPart->boltListY.GetNext())
			boltHoleList[i][1].append(*pBoltHole);
		CQuickSort<BOLTHOLE_INFO>::QuickSort(boltHoleList[i][0].m_pData,boltHoleList[i][0].GetSize(),CompareBoltHolePos);
		CQuickSort<BOLTHOLE_INFO>::QuickSort(boltHoleList[i][1].m_pData,boltHoleList[i][1].GetSize(),CompareBoltHolePos);
	}
	int nDatumLsXNum=boltHoleList[0][0].GetSize(),nDatumLsYNum=boltHoleList[0][1].GetSize();
	int nCurLsXNum=boltHoleList[1][0].GetSize(),nCurLsYNum=boltHoleList[1][1].GetSize();
	if((nDatumLsXNum+nDatumLsYNum)!=(nCurLsXNum+nCurLsYNum))
		return false;
	double HOLE_POS_CMP_EPS=1;
	double fAngleLen=pPeerPolyPart->GetLength();
	bool bRet=false;
	if( vecCmpRetArr[0]&&
		CompareBoltHoleList(boltHoleList[0][0],boltHoleList[1][0])<HOLE_POS_CMP_EPS&&
		CompareBoltHoleList(boltHoleList[0][1],boltHoleList[1][1])<HOLE_POS_CMP_EPS)
	{
		if(pPeerInfo)
		{
			pPeerInfo->csPeer=temUcs1;
			pPeerInfo->hashBoltsByBase.Empty();
			for(int i=0;i<2;i++)
			{
				for(int ii=0;ii<boltHoleList[0][i].GetSize();ii++)
				{
					PEER_BOLT *pPeerBolt=pPeerInfo->hashBoltsByBase.Add(boltHoleList[0][i].GetByIndex(ii)->BoltHandle());
					pPeerBolt->hPeerBolt=boltHoleList[1][i].GetByIndex(ii)->BoltHandle();
				}
			}
		}
		bRet=true;
	}
	if(!bRet && vecCmpRetArr[1] &&
		CompareBoltHoleList(boltHoleList[0][0],boltHoleList[1][1],fAngleLen)<HOLE_POS_CMP_EPS&&
		CompareBoltHoleList(boltHoleList[0][1],boltHoleList[1][0],fAngleLen)<HOLE_POS_CMP_EPS)
	{
		if(pPeerInfo)
		{
			pPeerInfo->csPeer=temUcs2;
			pPeerInfo->hashBoltsByBase.Empty();
			for(int i=0;i<2;i++)
			{
				for(int ii=0;ii<boltHoleList[0][i].GetSize();ii++)
				{
					PEER_BOLT *pPeerBolt=pPeerInfo->hashBoltsByBase.Add(boltHoleList[0][i].GetByIndex(ii)->BoltHandle());
					pPeerBolt->hPeerBolt=boltHoleList[1][1-i].GetByIndex(boltHoleList[0][i].GetSize()-ii-1)->BoltHandle();
				}
			}
		}
		bRet=true;
	}
	return bRet;	
}

void CLDSPolyPart::CopyProperty(CLDSDbObject* pObj)
{
	if(pObj->GetClassTypeId()!=CLS_POLYPART)
		return;
	CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pObj;
	iSeg=pPolyPart->iSeg;
	strcpy(layer(),pPolyPart->layer());
	m_uStatMatCoef=pPolyPart->m_uStatMatCoef;
	sPartNo.Copy(pPolyPart->sPartNo);
	cfgword=pPolyPart->cfgword;
	dwPermission=pPolyPart->dwPermission;
	cMaterial=pPolyPart->cMaterial;
	//
	segset.Empty();
	CLDSLinePart *pLinePart=NULL;
	for(pLinePart=pPolyPart->segset.GetFirst();pLinePart;pLinePart=pPolyPart->segset.GetNext())
		segset.append(pLinePart);
}
//根据构件上螺栓孔的先后顺序生螺栓句柄->孔索引标识(以1为基数)映射，返回螺栓孔数量 wjh-2016.1.05
long CLDSPolyPart::DispatchHoleIndexId(CHashList<long>* hashHoleIndexIdByBoltHandle)
{
	AnaylyzeAngleBolts(this);
	if(boltListX.GetNodeNum()==0&&boltListY.GetNodeNum()==0)
		return 0;
	long holeId=1;
	BOLTHOLE_INFO* pBoltHole;
	for(pBoltHole=boltListX.GetFirst();pBoltHole;pBoltHole=boltListX.GetNext(),holeId++)
		hashHoleIndexIdByBoltHandle->SetValue(pBoltHole->BoltHandle(),holeId);
	for(pBoltHole=boltListY.GetFirst();pBoltHole;pBoltHole=boltListY.GetNext(),holeId++)
		hashHoleIndexIdByBoltHandle->SetValue(pBoltHole->BoltHandle(),holeId);
	return hashHoleIndexIdByBoltHandle->GetNodeNum();
}

BOOL CLDSPolyPart::AnaylyzeAngleBolts(CLDSPart *pPolyAnglePart)
{
	if (pPolyAnglePart->GetClassTypeId() != CLS_POLYPART)
		return FALSE;	//类型不匹配
	CLDSPolyPart *pPolyAngle = (CLDSPolyPart*)pPolyAnglePart;
	for (CLDSPart *pSonPart = pPolyAngle->segset.GetFirst(); pSonPart; pSonPart = pPolyAngle->segset.GetNext())
	{
		if (pSonPart->GetClassTypeId() != CLS_LINEANGLE)
			return FALSE;	//类型不匹配
	}
	//生成首尾衔接的临时角钢段数组
	DYN_ARRAY<CLDSLineAngle> segArr(pPolyAngle->segset.GetNodeNum());
	//segArr.SetSize(pPolyAngle->segset.GetNodeNum());
	CLDSLineAngle *pFirstSeg =(CLDSLineAngle*)pPolyAngle->segset.GetFirst();
	CLDSLineAngle *pSecndSeg =(CLDSLineAngle*)pPolyAngle->segset.GetNext();
	CLDSLineAngle* pSeg = NULL, *pPrevSeg = NULL;
	int ii = 0;
	for(pSeg=(CLDSLineAngle*)pPolyAngle->segset.GetFirst();pSeg;pSeg=(CLDSLineAngle*)pPolyAngle->segset.GetNext(), ii++)
	{
		BOOL bPrevSegNeedReverseStartEnd = FALSE, bCurSegNeedReverseStartEnd = FALSE;
		if(pPrevSeg == NULL)	
		{	//首段火曲子角钢
			if ((pSeg->pStart == pSecndSeg->pStart&&DISTANCE(pSeg->Start(), pSecndSeg->Start()) < 2.0) ||
				(pSeg->pStart == pSecndSeg->pEnd&&DISTANCE(pSeg->Start(), pSecndSeg->End()) < 2.0))
				bCurSegNeedReverseStartEnd = TRUE;
		}
		else
		{
			if (pPrevSeg->pStart == pSeg->pStart&&DISTANCE(pPrevSeg->Start(), pSeg->Start()) < 2.0)
			{
				bPrevSegNeedReverseStartEnd = TRUE;
				bCurSegNeedReverseStartEnd = FALSE;
			}
			else if ((pPrevSeg->pStart == pSeg->pEnd&&DISTANCE(pPrevSeg->Start(), pSeg->End()) < 2.0))
			{
				bPrevSegNeedReverseStartEnd = TRUE;
				bCurSegNeedReverseStartEnd = TRUE;
			}
			else if (pPrevSeg->pEnd == pSeg->pStart&&DISTANCE(pPrevSeg->End(), pSeg->Start()) < 2.0)
			{
				bPrevSegNeedReverseStartEnd = FALSE;
				bCurSegNeedReverseStartEnd = FALSE;
			}
			else if (pPrevSeg->pEnd == pSeg->pEnd&&DISTANCE(pPrevSeg->End(), pSeg->End()) < 2.0)
			{
				bPrevSegNeedReverseStartEnd = FALSE;
				bCurSegNeedReverseStartEnd = TRUE;
			}
		}
		CopyLineAngle(&segArr[ii], pSeg, bCurSegNeedReverseStartEnd);
		//修正制弯点前一角钢段正负头 用于计算火曲变形处长度缩短值
		if (pPrevSeg)
		{
			//if (bPrevSegNeedReverseStartEnd)	//前一角钢段始端连接当前制弯点
			//	segArr[ii-1].SetStartOdd(0);
			//else	//前一角钢段终端连接当前制弯点
				segArr[ii-1].SetEndOdd(0);	//segArr[0->ii]中角钢已调整好始末端顺序 wjh-2019.6.10
			//if (bCurSegNeedReverseStartEnd)	//后一角钢段终端连接当前制弯点
			//	segArr[ii].SetEndOdd(0);
			//else	//后一角钢段始端连接当前制弯点
				segArr[ii].SetStartOdd(0);	//segArr[0->ii]中角钢已调整好始末端顺序 wjh-2019.6.10
		}
		pPrevSeg = pSeg;
	}
	//统计角钢上螺栓坐标
	CLDSPolyPart::boltListX.Empty();
	CLDSPolyPart::boltListY.Empty();
	double realStartPosX = 0;
	for (ii = 0; ii < (int)segArr.Size(); ii++)
	{
		CLDSLineAngle *pLineAngle = &segArr[ii];
		if (ii == 0)
			realStartPosX = pLineAngle->startOdd();
		for (CLsRef *pLsRef = pLineAngle->GetFirstLsRef(); pLsRef; pLsRef = pLineAngle->GetNextLsRef())
		{
			int liWingX0Y1 = 0;
			GEPOINT xLocation;
			if (pLineAngle->GetBoltLocalPos(pLsRef->pBolt, xLocation, false, &liWingX0Y1) == 0)
			{	//GetBoltLocalPos计算的螺栓孔位更准确 wjh-2019.4.17
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "0x%X角钢{%s}上的螺栓0x%X相对方位异常，未生成在NC数据中", pLineAngle->handle, pLineAngle->Label, pLsRef->pBolt->handle);
				continue;
			}
			double justify = (liWingX0Y1 == 0) ? pLsRef->pBolt->ucs.axis_z*pLineAngle->vxWingNorm : pLsRef->pBolt->ucs.axis_z*pLineAngle->vyWingNorm;
			if (fabs(justify) < 0.999)
				logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "0x%X角钢{%s}上的螺栓0x%X不垂直，可能导致孔位不准", pLineAngle->handle, pLineAngle->Label, pLsRef->pBolt->handle);
			f3dCircle cir;
			cir.radius = (*pLsRef)->get_d() / 2.0;
			cir.centre = xLocation;	
			cir.feature = (int)pLsRef;
			cir.centre.Set(cir.centre.x + realStartPosX, fabs(xLocation.y), 0);
			if (liWingX0Y1 == 0)	//螺栓在角钢X肢上
				CLDSPolyPart::boltListX.append(BOLTHOLE_INFO(cir.centre, pLsRef->waistLen, pLsRef->pBolt));
			else
				CLDSPolyPart::boltListY.append(BOLTHOLE_INFO(cir.centre, pLsRef->waistLen, pLsRef->pBolt));
		}
		realStartPosX += DISTANCE(pLineAngle->Start(), pLineAngle->End());
	}
	return TRUE;
}

BOOL CLDSPolyPart::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	if(pSolidBody!=NULL&& !IsModified())
		return TRUE;
	else
	{
		if(pSolidBody == NULL)
			pSolidBody = new CSolidBody;
		else
			pSolidBody->Empty();
	}
	SetModified(FALSE);
	CHashSet<CLDSPolyJoint*> hashJointByPrevRodHandle;
	for(CLDSPolyJoint *pPolyJoint=(CLDSPolyJoint*)BelongModel()->EnumPartFirst(CLS_POLYJOINT);
		pPolyJoint;pPolyJoint=(CLDSPolyJoint*)BelongModel()->EnumPartNext(CLS_POLYJOINT))
	{
		if(pPolyJoint->pPrevLinePart==NULL||pPolyJoint->pNextLinePart==NULL)
			continue;
		hashJointByPrevRodHandle.SetValue(pPolyJoint->pPrevLinePart->handle,pPolyJoint);
	}
	for(CLDSLinePart *pRod=segset.GetFirst();pRod;pRod=segset.GetNext())
	{
		pRod->Create3dSolidModel(bDisplayHole);
		pSolidBody->MergeBodyBuffer(pRod->pSolidBody->BufferPtr(),pRod->pSolidBody->BufferLength());
		CLDSPolyJoint *pJoint=hashJointByPrevRodHandle.GetValue(pRod->handle);
		if(pJoint)
		{
			pJoint->Create3dSolidModel(bDisplayHole);
			pSolidBody->MergeBodyBuffer(pJoint->pSolidBody->BufferPtr(),pJoint->pSolidBody->BufferLength());
		}
	}
	return TRUE;
}
UCS_STRU CLDSPolyPart::GetUCS(bool bIncOddment/*=false*/,bool bReverse/*=false*/)
{
	LoadDefaultUCS(&ucs);
	CLDSLinePart *pRod=segset.GetFirst();
	if(bReverse)
		pRod=segset.GetTail();
	if(pRod)
	{
		if(pRod->IsAngle())
		{
			((CLDSLineAngle*)pRod)->getUCS(pRod->ucs);
			ucs=pRod->ucs;
		}
		else if(pRod->GetClassTypeId()==CLS_LINEFLAT)
			ucs=((CLDSLineFlat*)pRod)->BuildUCS();
		else if(pRod->GetClassTypeId()==CLS_LINESLOT)
			ucs=((CLDSLineSlot*)pRod)->BuildUCS();
		else if(pRod->GetClassTypeId()==CLS_LINETUBE)
			ucs=((CLDSLineTube*)pRod)->BuildUCS();
		else
			ucs=pRod->ucs;
		if(pRod->bHuoQuEnd)
		{
			if(bIncOddment)
				ucs.origin-=pRod->ucs.axis_z*pRod->startOdd();
		}
		else if(pRod->bHuoQuStart)
		{
			double len = DISTANCE(pRod->Start(),pRod->End());
			ucs.axis_z = -ucs.axis_z;
			f3dPoint tm_axis = ucs.axis_x;
			ucs.axis_x = ucs.axis_y;
			ucs.axis_y = tm_axis;
			ucs.origin = pRod->End();
			if(bIncOddment)
				ucs.origin-=ucs.axis_z*pRod->endOdd();
		}
	}
	return ucs;
}
double CLDSPolyPart::startOdd(bool bToFL/*=false*/)
{
	CLDSLinePart *pRod=segset.GetFirst();
	if(pRod&&pRod->bHuoQuEnd)
		return pRod->startOdd(bToFL);
	else if(pRod&&pRod->bHuoQuStart)
		return pRod->endOdd(bToFL);
	return 0;
}
double CLDSPolyPart::endOdd(bool bToFL/*=false*/)
{
	CLDSLinePart *pRod=segset.GetTail();
	if(pRod&&pRod->bHuoQuEnd)
		return pRod->startOdd(bToFL);
	else if(pRod&&pRod->bHuoQuStart)
		return pRod->endOdd(bToFL);
	return 0;
}
UINT CLDSPolyJoint::crPolyJoint=RGB(255,128,128);
CLDSPolyJoint::CLDSPolyJoint()
{
	m_nClassTypeId=CLS_POLYJOINT;
	//strcpy(m_sClassName,"CLDSPolyJoint");
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
	pPrevLinePart=pNextLinePart=NULL;
	rBending=0;
	pSolidBody=NULL;
}
CLDSPolyJoint::~CLDSPolyJoint()
{
	if(pSolidBody)
	{
		delete pSolidBody;
		pSolidBody=NULL;
	}
}
BOOL CLDSPolyJoint::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	if(pPrevLinePart==NULL||pNextLinePart==NULL)
		return FALSE;
	if(pPrevLinePart->GetClassTypeId()!=pNextLinePart->GetClassTypeId())
		return FALSE;
	if(pSolidBody!=NULL&& !IsModified())
		return TRUE;
	else
	{
		if(pSolidBody == NULL)
			pSolidBody = new CSolidBody;
		else
			pSolidBody->Empty();
	}
	SetModified(FALSE);
	fBody body;
	fBody* pBody=&body;
	BOOL bPrevStartLj,bNextStartLj;
	if(pPrevLinePart->pStart==pNextLinePart->pStart&&pPrevLinePart->pStart!=NULL)
	{
		bPrevStartLj=TRUE;
		bNextStartLj=TRUE;
	}
	else if(pPrevLinePart->pStart==pNextLinePart->pEnd&&pPrevLinePart->pStart!=NULL)
	{
		bPrevStartLj=TRUE;
		bNextStartLj=FALSE;
	}
	else if(pPrevLinePart->pEnd==pNextLinePart->pStart&&pPrevLinePart->pEnd!=NULL)
	{
		bPrevStartLj=FALSE;
		bNextStartLj=TRUE;
	}
	else if(pPrevLinePart->pEnd==pNextLinePart->pEnd&&pPrevLinePart->pEnd!=NULL)
	{
		bPrevStartLj=FALSE;
		bNextStartLj=FALSE;
	}
	else
		return FALSE;
	if(pPrevLinePart->GetClassTypeId()==CLS_LINEANGLE)
	{
		f3dPoint vertex_arr[12];
		CLDSLineAngle *pPrevAngle = (CLDSLineAngle*)pPrevLinePart;
		CLDSLineAngle *pNextAngle = (CLDSLineAngle*)pNextLinePart;
		//建立制弯点变形面装配坐标系
		ucs.axis_z = pPrevAngle->End()-pPrevAngle->Start();
		normalize(ucs.axis_z);
		ucs.axis_x=pPrevAngle->GetWingVecX();
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		ucs.origin=pPrevAngle->Start()-ucs.axis_z*pPrevAngle->startOdd();
		double wing_wide = pPrevAngle->GetWidth();
		double wing_thick = pPrevAngle->GetThick();
		f3dPoint wing_x_vec,wing_y_vec,wing_x_norm,wing_y_norm;
		//前一根角钢始端顶点位置
		if(bPrevStartLj)
		{
			wing_x_vec=pPrevAngle->GetWingVecY();
			wing_y_vec=pPrevAngle->GetWingVecX();
			wing_x_norm=pPrevAngle->get_norm_y_wing();
			wing_y_norm=pPrevAngle->get_norm_x_wing();
		}
		else
		{
			wing_x_vec=pPrevAngle->GetWingVecX();
			wing_y_vec=pPrevAngle->GetWingVecY();
			wing_x_norm=pPrevAngle->get_norm_x_wing();
			wing_y_norm=pPrevAngle->get_norm_y_wing();
		}
		f3dPoint bi_vec=wing_x_vec+wing_y_vec;
		normalize(bi_vec);
		double cosa=bi_vec*wing_x_vec;
		double sina=sqrt(1-cosa*cosa);
		if(fabs(cosa)<eps||fabs(cosa)>EPS_COS)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The direction of angle's two leg coincide,data error";
#else
			throw "角钢两肢方向重合,数据错误";
#endif
		vertex_arr[0]=ucs.origin;
		vertex_arr[1]=vertex_arr[0]+wing_x_vec*wing_wide;
		vertex_arr[2]=vertex_arr[1]-wing_x_norm*wing_thick;
		vertex_arr[3]=vertex_arr[0]+bi_vec*(wing_thick/sina);
		vertex_arr[5]=vertex_arr[0]+wing_y_vec*wing_wide;
		vertex_arr[4]=vertex_arr[5]-wing_y_norm*wing_thick;
		//下一根角钢始端顶点位置
		if(bNextStartLj)
		{
			wing_x_vec=pNextAngle->GetWingVecX();
			wing_y_vec=pNextAngle->GetWingVecY();
			wing_x_norm=pNextAngle->get_norm_x_wing();
			wing_y_norm=pNextAngle->get_norm_y_wing();
		}
		else
		{
			wing_x_vec=pNextAngle->GetWingVecY();
			wing_y_vec=pNextAngle->GetWingVecX();
			wing_x_norm=pNextAngle->get_norm_y_wing();
			wing_y_norm=pNextAngle->get_norm_x_wing();
		}
		bi_vec=wing_x_vec+wing_y_vec;
		normalize(bi_vec);
		cosa=bi_vec*wing_x_vec;
		sina=sqrt(1-cosa*cosa);
		if(fabs(cosa)<eps||fabs(cosa)>EPS_COS)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The direction of angle's two leg coincide,data error";
#else
			throw "角钢两肢方向重合,数据错误";
#endif
		f3dPoint next_axis_z=pNextAngle->End()-pNextAngle->Start();
		normalize(next_axis_z);
		vertex_arr[6]=pNextAngle->Start()-pNextAngle->startOdd()*next_axis_z;
		vertex_arr[7]=vertex_arr[6]+wing_x_vec*wing_wide;
		vertex_arr[8]=vertex_arr[7]-wing_x_norm*wing_thick;
		vertex_arr[9]=vertex_arr[6]+bi_vec*(wing_thick/sina);
		vertex_arr[11]=vertex_arr[6]+wing_y_vec*wing_wide;
		vertex_arr[10]=vertex_arr[11]-wing_y_norm*wing_thick;
		
		int i;
		if(!bPrevStartLj)
		{
			for(i=0;i<6;i++)
				vertex_arr[i]+=ucs.axis_z*pPrevAngle->GetLength();
		}
		if(!bNextStartLj)
		{
			for(i=6;i<12;i++)
				vertex_arr[i]+=next_axis_z*pNextAngle->GetLength();
		}
		for(i=0;i<12;i++)
			pBody->vertex.append(vertex_arr[i]);
		BOOL bSameBer=FALSE;
		if(vertex_arr[0]==vertex_arr[6])
			bSameBer=TRUE;
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);

		for(i=0;i<6;i++)
		{
			f3dPolyFace *pFace = pBody->faceList.append();
			pFace->material=CLDSPolyJoint::crPolyJoint;
			if(!bSameBer||i>0)
			{
				createFace.NewOutterEdgeLine(pFace,i+6,i);
				createFace.NewOutterEdgeLine(pFace,(i+5)%6+6);
				createFace.NewOutterEdgeLine(pFace,i);
			}
			if(!bSameBer||i!=1)
			{
				pFace = pBody->faceList.append();
				pFace->material=CLDSPolyJoint::crPolyJoint;
				createFace.NewOutterEdgeLine(pFace,(i+5)%6+6,i);
				createFace.NewOutterEdgeLine(pFace,(i+5)%6);
				createFace.NewOutterEdgeLine(pFace,i);
			}
		}
	}
	else if(pPrevLinePart->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube *pPrevTube = (CLDSLineTube*)pPrevLinePart;
		CLDSLineTube *pNextTube = (CLDSLineTube*)pNextLinePart;
		f3dPoint prev_vec=pPrevTube->End()-pPrevTube->Start();
		f3dPoint next_vec=pNextTube->End()-pNextTube->Start();
		if(!bPrevStartLj)
			prev_vec=pPrevTube->Start()-pPrevTube->End();
		if(!bNextStartLj)
			next_vec=pNextTube->Start()-pNextTube->End();
		//建立制弯点变形面装配坐标系
		if(bPrevStartLj)
			ucs.origin=pPrevTube->Start();
		else
			ucs.origin=pPrevTube->End();
		ucs.axis_z = prev_vec^next_vec;
		normalize(ucs.axis_z);
		ucs.axis_x=next_vec;
		ucs.axis_y=ucs.axis_z^next_vec;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		double sector_angle=Pi-cal_angle_of_2vec(prev_vec,next_vec);
		double radius=pPrevTube->GetDiameter()*0.5;
		int uSectionSlices=CalArcResolution(radius,Pi,scale_of_user2scr,sample_len,smoothness);
		int uBendingSlices=CalArcResolution(radius,sector_angle,scale_of_user2scr,sample_len);
		uSectionSlices=max(uSectionSlices,4);	//圆截面方向的分片数
		uSectionSlices=min(uSectionSlices,24);
		uBendingSlices=max(uBendingSlices,2);	//弯曲弧方向的分片数
		uBendingSlices=min(uBendingSlices,12);
		double section_slice_angle=Pi/uSectionSlices;
		double bending_slice_angle=sector_angle/uBendingSlices;
		int i,j;
		f3dPoint vertice,vertice2;
		vertice=ucs.origin+ucs.axis_z*radius;
		pBody->vertex.append(vertice);	//上顶面极点
		for(i=1;i<uSectionSlices;i++)
		{
			vertice.Set(0,radius*sin(section_slice_angle*i),radius*cos(section_slice_angle*i));
			vertice2=vertice;
			coord_trans(vertice2,ucs,TRUE);
			pBody->vertex.append(vertice2);
			for(j=1;j<=uBendingSlices;j++)
			{
				vertice2=vertice;
				RotateVectorAroundVector(vertice2,bending_slice_angle*j,f3dPoint(0,0,1));
				pBody->vertex.append(vertice2);
			}
		}
		vertice=ucs.origin-ucs.axis_z*radius;
		pBody->vertex.append(vertice);	//下底面极点
		
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);

		int num=pBody->vertex.GetNodeNum();
		int base_index=1;
		f3dPolyFace *pFace=NULL;
		COLORREF crCutFaceOrPolyJoint=GetColor(CLS_POLYJOINT,-1,' '); //切割面
		for(j=0;j<uBendingSlices;j++)
		{
			pFace=pBody->faceList.append();
			pFace->material=crCutFaceOrPolyJoint;
			createFace.NewOutterEdgeLine(pFace,i+base_index,0);
			createFace.NewOutterEdgeLine(pFace,i+1+base_index);
			createFace.NewOutterEdgeLine(pFace,0);
		}
		for(i=1;i<uSectionSlices;i++)
		{
			base_index+=uBendingSlices+1;
			for(j=0;j<uBendingSlices;j++)
			{
				pFace=pBody->faceList.append();
				pFace->material=crCutFaceOrPolyJoint;
				createFace.NewOutterEdgeLine(pFace,j+1+base_index,j+base_index);
				createFace.NewOutterEdgeLine(pFace,j+base_index-uBendingSlices);
				createFace.NewOutterEdgeLine(pFace,j+base_index);

				pFace=pBody->faceList.append();
				pFace->material=crCutFaceOrPolyJoint;
				createFace.NewOutterEdgeLine(pFace,j+base_index-uBendingSlices-1,j+base_index-uBendingSlices);
				createFace.NewOutterEdgeLine(pFace,j+base_index);
				createFace.NewOutterEdgeLine(pFace,j+base_index-uBendingSlices);
			}
		}
		base_index+=uBendingSlices+1;
		for(j=0;j<uBendingSlices;j++)
		{
			pFace=pBody->faceList.append();
			pFace->material=crCutFaceOrPolyJoint;
			createFace.NewOutterEdgeLine(pFace,i+1+base_index,num-1);
			createFace.NewOutterEdgeLine(pFace,i+base_index);
			createFace.NewOutterEdgeLine(pFace,num-1);
		}
	}
	pSolidBody->ConvertFrom(pBody);
	return TRUE;
}
void CLDSPolyJoint::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_tma1_lma2_tsa3_lds4_default0/*=0*/)
{
	if(doc_tma1_lma2_tsa3_lds4_default0==0)
		doc_tma1_lma2_tsa3_lds4_default0=PRODUCT_TYPE;
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_tma1_lma2_tsa3_lds4_default0))
		return;
	long h;
	pPrevLinePart=pPrevLinePart=NULL;
	buffer.ReadDword(&dwPermission);
	buffer.ReadInteger(&h);
	CLDSPart *pPart=BelongModel()->FromPartHandle(h);
	if(pPart&&pPart->IsLinePart())
		pPrevLinePart=(CLDSLinePart*)pPart;

	buffer.ReadInteger(&h);
	pPart=BelongModel()->FromPartHandle(h);
	if(pPart&&pPart->IsLinePart())
		pNextLinePart=(CLDSLinePart*)pPart;
	buffer.ReadDouble(&rBending);
}
void CLDSPolyJoint::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
		return;
	buffer.WriteDword(dwPermission);
	if(pPrevLinePart)
		buffer.WriteInteger(pPrevLinePart->handle);
	else
		buffer.WriteInteger(0);
	if(pNextLinePart)
		buffer.WriteInteger(pNextLinePart->handle);
	else
		buffer.WriteInteger(0);
	buffer.WriteDouble(rBending);
}

void CLDSPolyJoint::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="";
	fprintf(fp,"<杆件变形点 handle=\"0X%X\">\n",handle);
	fprintf(fp,"<权限字 dwPermission=\"%d\"/>\n",dwPermission);
	if(pPrevLinePart)
		fprintf(fp,"<上一杆件句柄 handle=\"0X%X\"/>\n",pPrevLinePart->handle);
	else
		fprintf(fp,"<上一杆件句柄 handle=\"0X0\"/>\n");
	if(pNextLinePart)
		fprintf(fp,"<下一杆件句柄 handle=\"0X%X\"/>\n",pNextLinePart->handle);
	else
		fprintf(fp,"<下一杆件句柄 handle=\"0X0\"/>\n");
	sprintf(sText,"%f",rBending);
	SimplifiedNumString(sText);
	fprintf(fp,"<制弯时的制弯点内圆弧半径 rBending=\"%s\"/>\n",sText);
	fprintf(fp,"<杆件变形点>\n");
}

int CLDSPolyJoint::GetPropValueStr(long id, char* valueStr)
{
	char sText[500]="";
	if(GetPropID("handle")==id)
		sprintf(sText,"0X%X",handle);
	else if(GetPropID("layer")==id)
		strcpy(sText,layer());
	else if(GetPropID("cMaterial")==id)	
	{
		char matStr[20]="";
		QuerySteelMatMark(cMaterial,matStr);
		sprintf(sText,"%s",matStr);
	}
	else if(GetPropID("m_cQualityLevel")==id)
		sprintf(sText,"%C",m_cQualityLevel);
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("sPartNo")==id)
		strcpy(sText,GetPartNo());
	else if(GetPropID("cfgword")==id)
		//return MakeWordToString(cfgword,valueStr);
	{
		if(layer(0)=='L')	//接腿构件
			return cfgword.GetLegScopeStr(valueStr,50,true);
		else
			return cfgword.GetBodyScopeStr(valueStr);
	}
	else if(GetPropID("m_uStatMatCoef")==id)
		sprintf(sText,"%d",m_uStatMatCoef);
	else if(GetPropID("m_hPartWeldParent")==id)
		sprintf(sText,"0X%X",m_hPartWeldParent);
	else if(GetPropID("m_bDisplayHole")==id)
	{
		if(m_bDisplayHole)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_bVirtualPart")==id)
	{
		if(m_bVirtualPart)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");	
#else
			strcpy(sText,"是");
		else
			strcpy(sText,"否");	
#endif
	}
	else if(GetPropID("pPrevLinePart")==id)
	{
		if(pPrevLinePart)
			sprintf(sText,"0X%X",pPrevLinePart->handle);
		else
			sprintf(sText,"0X%X",0);
	}
	else if(GetPropID("pNextLinePart")==id)
	{
		if(pNextLinePart)
			sprintf(sText,"0X%X",pNextLinePart->handle);
		else
			sprintf(sText,"0X%X",0);
	}
	else if(GetPropID("rBending")==id)
	{
		if(pPrevLinePart&&pPrevLinePart->GetClassTypeId()==CLS_LINEANGLE)
			rBending=((CLDSLineAngle*)pPrevLinePart)->huoqu_r;
		sprintf(sText,"%f",rBending);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("huoqu_base_wing_x0_y1_both2")==id)
	{
		if(pPrevLinePart&&pPrevLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{
			int huoqu_base_wing_x0_y1_both2=0;
			huoqu_base_wing_x0_y1_both2=((CLDSLineAngle*)pPrevLinePart)->huoqu_base_wing_x0_y1_both2;
			if(huoqu_base_wing_x0_y1_both2==0)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"0.X Leg");
			else if(huoqu_base_wing_x0_y1_both2==1)
				strcpy(sText,"1.Y Leg");	
			else 
				strcpy(sText,"2.Double Leg");
#else
				strcpy(sText,"0.X肢");
			else if(huoqu_base_wing_x0_y1_both2==1)
				strcpy(sText,"1.Y肢");	
			else 
				strcpy(sText,"2.双肢");
#endif
		}
	}
	else if(GetPropID("m_bUserSpecColor")==id)
	{
		if(m_bUserSpecColor)
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sText,"Yes");
		else
			sprintf(sText,"No");
#else
			sprintf(sText,"是");
		else
			sprintf(sText,"否");
#endif
	}
	else if(GetPropID("crMaterial")==id)
		sprintf(sText,"RGB%X",crMaterial);
	else if(GetPropID("sNotes")==id)
		strcpy(sText,sNotes);
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
long CLDSPolyJoint::GetPropID(char* propStr)
{
	PROPLIST_ITEM* pItem = propHashtable.GetValue(propStr);
	if(pItem==NULL)
		pItem =CLDSPolyJoint::propHashtable.GetValue(propStr);
	if(pItem)
		return pItem->id;
	else
	{
		/*CString prompt="没找到名为\"";
		prompt+=propStr;
		prompt+="\"的属性ID！";
		AfxMessageBox(prompt);*/
		return 0;
	}
}
int CLDSPolyJoint::GetPropStr(long id, char *propStr)
{
	if(GetPropID("basicInfo")==0)
	{
		if(propStr)
			strcpy(propStr,"basicInfo");
		return strlen("basicInfo");
	}
	else if(GetPropID("handle")==id)
	{
		if(propStr)
			strcpy(propStr,"handle");
		return strlen("handle");
	}
	else if(GetPropID("layer")==id)
	{
		if(propStr)
			strcpy(propStr,"layer");
		return strlen("layer");
	}
	else if(GetPropID("cMaterial")==id)
	{
		if(propStr)
			strcpy(propStr,"cMaterial");
		return strlen("cMaterial");
	}
	else if(GetPropID("m_cQualityLevel")==id)
	{
		if(propStr)
			strcpy(propStr,"m_cQualityLevel");
		return strlen("m_cQualityLevel");
	}
	else if(GetPropID("iSeg")==id)
	{
		if(propStr)
			strcpy(propStr,"iSeg");
		return strlen("iSeg");
	}
	else if(GetPropID("sPartNo")==id)
	{
		if(propStr)
			strcpy(propStr,"sPartNo");
		return strlen("sPartNo");
	}
	else if(GetPropID("cfgword")==id)
	{
		if(propStr)
			strcpy(propStr,"cfgword");
		return strlen("cfgword");
	}
	else if(GetPropID("m_uStatMatCoef")==id)
	{
		if(propStr)
			strcpy(propStr,"m_uStatMatCoef");
		return strlen("m_uStatMatCoef");
	}
	else if(GetPropID("m_hPartWeldParent")==id)
	{
		if(propStr)
			strcpy(propStr,"m_hPartWeldParent");
		return strlen("m_hPartWeldParent");
	}
	else if(GetPropID("m_bDisplayHole")==id)
	{
		if(propStr)
			strcpy(propStr,"m_bDisplayHole");
		return strlen("m_bDisplayHole");
	}
	else if(GetPropID("m_bVirtualPart")==id)
	{
		if(propStr)
			strcpy(propStr,"m_bVirtualPart");
		return strlen("m_bVirtualPart");
	}
	else if(GetPropID("pPrevLinePart")==id)
	{
		if(propStr)
			strcpy(propStr,"pPrevLinePart");
		return strlen("pPrevLinePart");
	}
	else if(GetPropID("pNextLinePart")==id)
	{
		if(propStr)
			strcpy(propStr,"pNextLinePart");
		return strlen("pNextLinePart");
	}
	else if(GetPropID("rBending")==id)
	{
		if(propStr)
			strcpy(propStr,"rBending");
		return strlen("rBending");
	}
	else if(GetPropID("huoqu_base_wing_x0_y1_both2")==id)
	{
		if(propStr)
			strcpy(propStr,"huoqu_base_wing_x0_y1_both2");
		return strlen("huoqu_base_wing_x0_y1_both2");
	}
	else if(GetPropID("sNotes")==id)
	{
		if(propStr)
			strcpy(propStr,"sNotes");
		return strlen("sNotes");
	}
	else
	{
		if(propStr)
			strcpy(propStr,"");
		return 0;
	}
}
BOOL CLDSPolyJoint::IsPropEqual(HASHOBJSET &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	selectObjs.push_stack();
	char valueStr[500]="";
	CLDSPolyJoint *pPart=NULL, *pFirstPart = (CLDSPolyJoint*)selectObjs.GetFirst();
	pFirstPart->GetPropValueStr(idProp,valueStr);
	for(pPart=(CLDSPolyJoint*)selectObjs.GetNext();pPart;pPart=(CLDSPolyJoint*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pPart->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CLDSPolyJoint::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CLDSPolyJoint::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}

BOOL CLDSPolyJoint::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSPolyJoint::propStatusHashtable.SetValueAt(id,dwStatus);
}

CHuoQuSeg::CHuoQuSeg()
{
	handle=0;
	huoqu_r = 0;
	pStart=pEnd=NULL;
}

void CHuoQuSeg::getUCS(UCS_STRU& ucs)
{
    //--------VVVVVVVVVV----计算角钢相对坐标系的三个坐标轴方向  Wjh--00.05.15-------
	//由于三个坐标轴必须为右手螺旋正交，故用户给定的肢法线方向有可能
	//不合理，此时各坐标轴中以Z轴方向的级别为最高（不可变），其次为X轴、Y轴
	//Z轴代表角钢的延伸方向，X、Y轴分别代表两肢的伸展方向
    ucs.origin = start_xyz;
	//计算角钢Z轴方向
    Sub_Pnt( ucs.axis_z, end_xyz, start_xyz);
	ucs.axis_x = ucs.axis_z^norm_x_wing;
	ucs.axis_y = ucs.axis_z^ucs.axis_x;

	normalize(ucs.axis_x);
	normalize(ucs.axis_y);
	normalize(ucs.axis_z);
}

/*double CLDSPolyAngle::GetLength()
{
	double length=0.0;
	CLDSLineAngle *pSeg=NULL,*pPrevSeg=NULL;
	BOOL bFirstSeg,bSameOrder,bNextSameOrder;	//是否与火曲角钢的方向一致
	bFirstSeg=TRUE;
	for(pSeg=seg_arr.GetFirst();pSeg;pSeg=seg_arr.GetNext())
	{
		if(pPrevSeg==NULL)
		{
			pPrevSeg=pSeg;
			continue;
		}
		if(pPrevSeg->pStart==pSeg->pStart&&DISTANCE(pPrevSeg->Start(),pSeg->Start())<2.0)
		{
			bSameOrder = FALSE;
			bNextSameOrder = TRUE;
		}
		else if(pPrevSeg->pStart==pSeg->pEnd&&DISTANCE(pPrevSeg->Start(),pSeg->End())<2.0)
		{
			bSameOrder = FALSE;
			bNextSameOrder = FALSE;
		}
		else if(pPrevSeg->pEnd==pSeg->pStart&&DISTANCE(pPrevSeg->End(),pSeg->Start())<2.0)
		{
			bSameOrder = TRUE;
			bNextSameOrder = TRUE;
		}
		else
		{
			bSameOrder = TRUE;
			bNextSameOrder = FALSE;
		}
		pPrevSeg->getUCS(ucs);
		if(!bSameOrder)
		{	//相对坐标系反转到终点
			double len = DISTANCE(pPrevSeg->Start(),pPrevSeg->End());
			ucs.origin += ucs.axis_z*len;
			ucs.axis_z = -ucs.axis_z;
			f3dPoint tm_axis = ucs.axis_x;
			ucs.axis_x = ucs.axis_y;
			ucs.axis_y = tm_axis;
			if(bFirstSeg)
				length = pPrevSeg->endOdd();
		}
		else if(bFirstSeg)
			length = pPrevSeg->startOdd();

		
		//计算火曲变形处长度缩短值
		f3dPoint vec_start,vec_end,vec_mid;
		vec_start = pPrevSeg->Start()-pPrevSeg->End();
		vec_end   = pSeg->End()-pSeg->Start();
		if(bSameOrder!=bNextSameOrder)
			vec_end*=-1.0;
		normalize(vec_start);
		normalize(vec_end);
		vec_mid = (vec_start+vec_end)/2.0;
		double angle = cal_angle_of_2vec(vec_start,vec_end)/2.0;
		double sub_len = ((Pi/2-angle)*pPrevSeg->huoqu_r-tan(Pi/2.0-angle)*pPrevSeg->huoqu_r)*2;
		length += DISTANCE(pPrevSeg->Start(),pPrevSeg->End())+sub_len;
		pPrevSeg=pSeg;
		bFirstSeg= FALSE;
	}
	//处理最后一段子角钢上的螺栓
	pPrevSeg->getUCS(ucs);
	length += DISTANCE(pPrevSeg->Start(),pPrevSeg->End());
	if(!bNextSameOrder)
		length+=pPrevSeg->startOdd();
	else
		length+=pPrevSeg->endOdd();
	
	return length;
}

void CLDSPolyAngle::ExplodeHuoQu()
{
	for(CLDSLineAngle *pSeg=seg_arr.GetFirst();pSeg;pSeg=seg_arr.GetNext())
	{
		pSeg->UnlockStartPos();
		pSeg->UnlockEndPos();
		if(pSeg->bHuoQuStart)
		{
			pSeg->bHuoQuStart = FALSE;
			pSeg->SetStart(pSeg->pre_huoqu_start_xyz);
		}
		if(pSeg->bHuoQuEnd)
		{
			pSeg->bHuoQuEnd = FALSE;
			pSeg->SetEnd(pSeg->pre_huoqu_end_xyz);
		}
		pSeg->huoqu_start1_end2=0;
		pSeg->huoqu_handle=0;
		pSeg->sub_type = TA_JG_COMMON;
	}
}*/

CLDSGroupLineAngle::CLDSGroupLineAngle()
{
	size_idClassType=m_nClassTypeId=CLS_GROUPLINEANGLE;
	size_cSubClassType='D';
	//strcpy(m_sClassName,"组合角钢");	//输出警示信息时最好是中文 wjh-2019.6.28
	m_bUpdateSonAngleOddment=TRUE;
	style.dwStyle=0;
	jg_space=12;
	son_jg_h[0]=son_jg_h[1]=son_jg_h[2]=son_jg_h[3]=0;
	desStartPos.wing_x_offset.gStyle=4;
	desStartPos.wing_x_offset.offsetDist=0;
	desStartPos.wing_y_offset.gStyle=4;
	desStartPos.wing_y_offset.offsetDist=0;
	desEndPos.wing_x_offset.gStyle=4;
	desEndPos.wing_x_offset.offsetDist=0;
	desEndPos.wing_y_offset.gStyle=4;
	desEndPos.wing_y_offset.offsetDist=0;
}
CLDSGroupLineAngle::~CLDSGroupLineAngle()
{
}
IMPLEMENT_GET(CLDSGroupLineAngle,group_style)
{
	CLDSGroupLineAngle* pMotherObj=(CLDSGroupLineAngle*)MotherObject();
	if(pMotherObj==this)
	{
		if(size_cSubClassType=='D')
			return 0;	//背靠背双拼
		else if(size_cSubClassType=='T')
			return 1;	//T型
		else if(size_cSubClassType=='X')
			return 2;	//十字交叉四拼
		else
			return 0;
	}
	else
		return pMotherObj->group_style;
}
IMPLEMENT_SET(CLDSGroupLineAngle,group_style)
{
	if (value>=0)
	{
		CLDSGroupLineAngle* pMotherObj=(CLDSGroupLineAngle*)MotherObject();
		if(value==0)
			size_cSubClassType='D';
		else if(value==1)
			size_cSubClassType='T';
		else if(value==2)
			size_cSubClassType='X';
		style.wSectStyle=value;
		if(pMotherObj!=this)
			pMotherObj->group_style=value;
	}
}
IMPLEMENT_GET(CLDSLinePart,size_cSubClassType)
{
	CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
	if (pMotherObj==this)
	{
		if (this->m_nClassTypeId==CLS_LINEANGLE)
			return 'L';	//size.cSubClassType与m_nClassTypeId有冲突的情况进行防错处理 wjh-2019.12.31
		return size.cSubClassType;
	}
	else
		return pMotherObj->size_cSubClassType;
}
IMPLEMENT_SET(CLDSLinePart,size_cSubClassType)
{
	CLDSLinePart* pMotherObj=(CLDSLinePart*)MotherObject();
	if(GetClassTypeId()==CLS_GROUPLINEANGLE&&value!='D'&&value!='T'&&value!='X')
		return ;//size.cSubClassType;	//输入子类型不合法
	if ( value == 0&&
		(m_nClassTypeId==CLS_LINEANGLE||size.idClassType==CLS_LINEANGLE||size.idClassType==CLS_GROUPLINEANGLE))
		size.cSubClassType = 'L';
	else
		size.cSubClassType=value;
	if(pMotherObj!=this)
		pMotherObj->size_cSubClassType=value;
}
IMPLEMENT_GET(CLDSGroupLineAngle,ciMasterIndex){return style.ciMasterIndex;}
IMPLEMENT_SET(CLDSGroupLineAngle,ciMasterIndex){style.ciMasterIndex=value;}
IMPLEMENT_GET(CLDSGroupLineAngle,ciMasterDirectIndex){return style.ciMasterDirectIndex;}
IMPLEMENT_SET(CLDSGroupLineAngle,ciMasterDirectIndex){style.ciMasterDirectIndex=value;}

IMPLEMENT_GET(CLDSGroupLineAngle,m_hDatumSonAngle)
{
	if(style.ciMasterIndex<4)
		return son_jg_h[style.ciMasterIndex];
	else
		return son_jg_h[0];
}
IMPLEMENT_GET(CLDSGroupLineAngle,m_pDatumSonAngle)
{
	if(BelongModel())
		return (CLDSLineAngle*)BelongModel()->FromPartHandle(m_hDatumSonAngle,CLS_LINEANGLE);
	else
		return NULL;
}
bool CLDSGroupLineAngle::IsBelongTo(int iClassTypeId)const
{
	if(iClassTypeId==m_nClassTypeId||iClassTypeId==CLS_LINEANGLE||iClassTypeId==CLS_LINEPART||
		iClassTypeId==CLS_PART||iClassTypeId==CLS_DBOBJECT)
		return true;
	else
		return false;
}
void CLDSGroupLineAngle::UpdateSonJgOddment()
{
	CLDSLineAngle *pJg[4]={NULL};
	if(!m_bUpdateSonAngleOddment)
		return;
	for(int i=0;i<4;i++)
	{
		pJg[i]=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[i],CLS_LINEANGLE);
		if(pJg[i])
		{
			if(!pJg[i]->IsStartPosLocked())
				pJg[i]->SetStartOdd(startOdd());
			if(!pJg[i]->IsEndPosLocked())
				pJg[i]->SetEndOdd(endOdd());
		}
	}
}
void CLDSGroupLineAngle::CalStartOddment(char ciAutoCalMode/*=0*/,int liInitOddment/*=0*/)
{
	CLDSLineAngle *pJg[4]={NULL};
	CMinDouble minOddment;
	for(int i=0;i<4;i++)
	{
		pJg[i]=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[i],CLS_LINEANGLE);
		bool blMandatoryCalOddment=ciAutoCalMode!=0||(m_bUpdateSonAngleOddment&&desStartOdd.m_iOddCalStyle==0);
		if (pJg[i])
		{
			CStackVariant<int>  stack0(&pJg[i]->desStartOdd.m_iOddCalStyle,desStartOdd.m_iOddCalStyle);
			CStackVariant<long> stack1(&pJg[i]->desStartOdd.m_hRefPart1,desStartOdd.m_hRefPart1);
			CStackVariant<long> stack2(&pJg[i]->desStartOdd.m_hRefPart2,desStartOdd.m_hRefPart2);
			CStackVariant<double> stack3(&pJg[i]->desStartOdd.m_fCollideDist,desStartOdd.m_fCollideDist);
			if (!m_bUpdateSonAngleOddment)
				stack0.PopStack();
			pJg[i]->CalStartOddment(blMandatoryCalOddment);
			if(blMandatoryCalOddment)
				minOddment.Update(pJg[i]->dfStartOdd);
		}
	}
	if (minOddment.IsInited())
	{
		this->start_oddment=minOddment.number;
		for(int i=0;m_bUpdateSonAngleOddment&&i<4;i++)
		{
			if(pJg[i])
			{
				if(!pJg[i]->IsStartPosLocked())
					pJg[i]->SetStartOdd(start_oddment);
			}
		}
	}
}
void CLDSGroupLineAngle::CalEndOddment(char ciAutoCalMode/*=0*/,int liInitOddment/*=0*/)
{
	//if(this->m_bUpdateSonAngleOddment)
	CLDSLineAngle *pJg[4]={NULL};
	CMinDouble minOddment;
	for(int i=0;i<4;i++)
	{
		pJg[i]=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[i],CLS_LINEANGLE);
		bool blMandatoryCalOddment=ciAutoCalMode!=0||(m_bUpdateSonAngleOddment&&desEndOdd.m_iOddCalStyle==0);
		if (pJg[i])
		{
			CStackVariant<int>  stack0(&pJg[i]->desEndOdd.m_iOddCalStyle,desEndOdd.m_iOddCalStyle);
			CStackVariant<long> stack1(&pJg[i]->desEndOdd.m_hRefPart1,desEndOdd.m_hRefPart1);
			CStackVariant<long> stack2(&pJg[i]->desEndOdd.m_hRefPart2,desEndOdd.m_hRefPart2);
			CStackVariant<double> stack3(&pJg[i]->desEndOdd.m_fCollideDist,desEndOdd.m_fCollideDist);
			if (!m_bUpdateSonAngleOddment)
				stack0.PopStack();
			pJg[i]->CalEndOddment(blMandatoryCalOddment);
			if (blMandatoryCalOddment)
				minOddment.Update(pJg[i]->dfEndOdd);
		}
	}
	if (minOddment.IsInited())
	{
		this->end_oddment=minOddment.number;
		for(int i=0;m_bUpdateSonAngleOddment&&i<4;i++)
		{
			if(pJg[i])
			{
				if(!pJg[i]->IsEndPosLocked())
					pJg[i]->SetEndOdd(endOdd());
			}
		}
	}
}
void CLDSGroupLineAngle::UpdateSonJgPos()
{
	CLDSLineAngle *pJg[4]={NULL};
	double offset_space=0.5*jg_space;
	f3dPoint pos,pick,wing_vec_x,wing_vec_y;
	wing_vec_x=GetWingVecX();
	wing_vec_y=GetWingVecY();
	switch(group_style)
	{
	case 0:	//对角组合
		pJg[0]=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[0],CLS_LINEANGLE);
		pJg[1]=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[1],CLS_LINEANGLE);
		pos=OffsetBy2Vec(Start(),wing_vec_x,wing_vec_y,offset_space,offset_space);
		if(pJg[0])
		{
			pJg[0]->SetStart(pos);
			pJg[0]->desStartPos.jgber_cal_style=1;	//指定坐标
			pos=OffsetBy2Vec(End(),wing_vec_x,wing_vec_y,offset_space,offset_space);
			pJg[0]->SetEnd(pos);
			pJg[0]->desEndPos.jgber_cal_style=1;	//指定坐标
		}

		pos=OffsetBy2Vec(Start(),wing_vec_x,wing_vec_y,-offset_space,-offset_space);
		if(pJg[1])
		{
			pJg[1]->SetStart(pos);
			pJg[1]->desStartPos.jgber_cal_style=1;	//指定坐标
			pos=OffsetBy2Vec(End(),wing_vec_x,wing_vec_y,-offset_space,-offset_space);
			pJg[1]->SetEnd(pos);
			pJg[1]->desEndPos.jgber_cal_style=1;	//指定坐标
		}
		break;
	case 1:	//T型组合
		pJg[0]=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[0],CLS_LINEANGLE);
		pJg[1]=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[1],CLS_LINEANGLE);
		if(style.ciMasterIndex==0)
		{
			pos=Start()+wing_vec_y*offset_space;
			if(pJg[0])
			{
				pJg[0]->SetStart(pos);
				pJg[0]->desStartPos.jgber_cal_style=1;	//指定坐标
				pos=End()+wing_vec_y*offset_space;
				pJg[0]->SetEnd(pos);
				pJg[0]->desEndPos.jgber_cal_style=1;	//指定坐标
			}

			pos=Start()-wing_vec_y*offset_space;
			if(pJg[1])
			{
				pJg[1]->SetStart(pos);
				pJg[1]->desStartPos.jgber_cal_style=1;	//指定坐标
				pos=End()-wing_vec_y*offset_space;
				pJg[1]->SetEnd(pos);
				pJg[1]->desEndPos.jgber_cal_style=1;	//指定坐标
			}
		}
		else
		{
			pos=Start()+wing_vec_x*offset_space;
			if(pJg[1])
			{
				pJg[1]->SetStart(pos);
				pJg[1]->desStartPos.jgber_cal_style=1;	//指定坐标
				pos=End()+wing_vec_x*offset_space;
				pJg[1]->SetEnd(pos);
				pJg[1]->desEndPos.jgber_cal_style=1;	//指定坐标
			}

			pos=Start()-wing_vec_x*offset_space;
			if(pJg[0])
			{
				pJg[0]->SetStart(pos);
				pJg[0]->desStartPos.jgber_cal_style=1;	//指定坐标
				pos=End()-wing_vec_x*offset_space;
				pJg[0]->SetEnd(pos);
				pJg[0]->desEndPos.jgber_cal_style=1;	//指定坐标
			}
		}
		break;
	case 2:	//十字交叉组合
		pJg[0]=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[0],CLS_LINEANGLE);
		pJg[1]=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[1],CLS_LINEANGLE);
		pJg[2]=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[2],CLS_LINEANGLE);
		pJg[3]=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[3],CLS_LINEANGLE);
		pos=OffsetBy2Vec(Start(),wing_vec_x,wing_vec_y,offset_space,offset_space);
		if(pJg[0])
		{
			pJg[0]->SetStart(pos);
			pJg[0]->desStartPos.jgber_cal_style=1;	//指定坐标
			pos=OffsetBy2Vec(End(),wing_vec_x,wing_vec_y,offset_space,offset_space);
			pJg[0]->SetEnd(pos);
			pJg[0]->desEndPos.jgber_cal_style=1;	//指定坐标
		}

		pos=OffsetBy2Vec(Start(),wing_vec_x,wing_vec_y,offset_space,-offset_space);
		if(pJg[1])
		{
			pJg[1]->SetStart(pos);
			pJg[1]->desStartPos.jgber_cal_style=1;	//指定坐标
			pos=OffsetBy2Vec(End(),wing_vec_x,wing_vec_y,offset_space,-offset_space);
			pJg[1]->SetEnd(pos);
			pJg[1]->desEndPos.jgber_cal_style=1;	//指定坐标
		}

		pos=OffsetBy2Vec(Start(),wing_vec_x,wing_vec_y,-offset_space,-offset_space);
		if(pJg[2])
		{
			pJg[2]->SetStart(pos);
			pJg[2]->desStartPos.jgber_cal_style=1;	//指定坐标
			pos=OffsetBy2Vec(End(),wing_vec_x,wing_vec_y,-offset_space,-offset_space);
			pJg[2]->SetEnd(pos);
			pJg[2]->desEndPos.jgber_cal_style=1;	//指定坐标
		}

		pos=OffsetBy2Vec(Start(),wing_vec_x,wing_vec_y,-offset_space,offset_space);
		if(pJg[3])
		{
			pJg[3]->SetStart(pos);
			pJg[3]->desStartPos.jgber_cal_style=1;	//指定坐标
			pos=OffsetBy2Vec(End(),wing_vec_x,wing_vec_y,-offset_space,offset_space);
			pJg[3]->SetEnd(pos);
			pJg[3]->desEndPos.jgber_cal_style=1;	//指定坐标
		}
		break;
	default:
		break;
	}
}

void CLDSGroupLineAngle::UpdateSonJgPerm()
{
	for(int i=0;i<4;i++)
	{
		CLDSLineAngle *pJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[i],CLS_LINEANGLE);
		if(pJg)
		{
			pJg->dwStartPermission=dwStartPermission;
			pJg->dwPermission=dwPermission;
			pJg->dwEndPermission=dwEndPermission;
		}
	}
}

void CLDSGroupLineAngle::UpdateSonJgWingNorm(BYTE cbSonAnglesFlag/*=0xff*/)
{
	CLDSLineAngle *pJg[4]={NULL};
	double kaihe_angle=GetRadWingAngle();
	if(fabs(kaihe_angle-Pi)<EPS||fabs(kaihe_angle)<EPS)
	{
		des_norm_y.bByOtherWing=true;
		this->cal_y_wing_norm();
	}
	if(style.ciMasterDirectIndex!=0)
	{
		int siSerialIndex=style.ciMasterDirectIndex-1;
		CLDSLineAngle* pDatumSonAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[siSerialIndex],CLS_LINEANGLE);
		GEPOINT vxSonAngleWingNorm=pDatumSonAngle->vxWingNorm;
		GEPOINT vySonAngleWingNorm=pDatumSonAngle->vyWingNorm;
		if (group_style==0)
		{
			if (style.ciMasterIndex+1==style.ciMasterDirectIndex)
			{
				norm_x_wing=vxSonAngleWingNorm;
				norm_y_wing=vySonAngleWingNorm;
			}
			else //if(style.ciMasterIndex+1!=style.ciMasterDirectIndex)
			{
				norm_x_wing=-vySonAngleWingNorm;
				norm_y_wing=-vxSonAngleWingNorm;
			}
		}
		else if (group_style==1)
		{
			if (style.ciMasterIndex+1==style.ciMasterDirectIndex)
			{
				norm_x_wing=vxSonAngleWingNorm;
				norm_y_wing=vySonAngleWingNorm;
			}
			else if (style.ciMasterIndex==0)	//style.ciMasterDirectIndex==2
			{
				norm_x_wing=-vySonAngleWingNorm;
				norm_y_wing= vxSonAngleWingNorm;
			}
			else //if(style.ciMasterIndex==1)	//style.ciMasterDirectIndex==1
			{
				norm_x_wing= vySonAngleWingNorm;
				norm_y_wing=-vxSonAngleWingNorm;
			}
		}
		//else if (group_style==2)
		//{
		//TODO:需求不足，尚未处理 wjh-2019.11.2
		//}
		else
			return;
	}
	BYTEFLAG flag(cbSonAnglesFlag);
	double offset_space=0.5*jg_space;
	switch(group_style)
	{
	case 0:	//对角组合
		pJg[0]=!flag.GetBitState(0)?NULL:(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[0],CLS_LINEANGLE);
		pJg[1]=!flag.GetBitState(1)?NULL:(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[1],CLS_LINEANGLE);
		if(pJg[0]&&style.ciMasterDirectIndex!=1)
		{
			double jdxx=pJg[0]->vxWingNorm*vxWingNorm;
			double jdyy=pJg[0]->vyWingNorm*vyWingNorm;
			double jdxy=pJg[0]->vxWingNorm*vyWingNorm;
			double jdyx=pJg[0]->vyWingNorm*vxWingNorm;
			if (fabs(jdxx)<0.91&&fabs(jdyx)>0.91)	//cos(25°)=0.91
			{	//颠倒两肢朝向
				DESIGN_JGWING_NORM xTempObj=pJg[0]->des_norm_x;
				pJg[0]->des_norm_x=pJg[0]->des_norm_y;
				pJg[0]->des_norm_y=xTempObj;
				if (jdyx<0)
					pJg[0]->des_norm_x.ReverseDirection();
				if (jdxy<0)
					pJg[0]->des_norm_y.ReverseDirection();
				pJg[0]->SetBriefWingNormX(vxWingNorm);
				pJg[0]->SetBriefWingNormY(vyWingNorm);
			}
			else if (jdxx<=-0.91&&jdyy<=-0.91)
			{	//两肢朝向反向
				pJg[0]->des_norm_x.ReverseDirection();
				pJg[0]->des_norm_y.ReverseDirection();
				pJg[0]->SetBriefWingNormX(vxWingNorm);
				pJg[0]->SetBriefWingNormY(vyWingNorm);
			}
			if(pJg[0]->des_norm_x.IsSpecNormActual())//!pJg[0]->des_norm_x.bByOtherWing&&pJg[0]->des_norm_x.hViceJg<=0x20)
				pJg[0]->SetBriefWingNormX(vxWingNorm);
			else
				pJg[0]->cal_x_wing_norm(true);
			if(pJg[0]->des_norm_y.IsSpecNormActual())//!pJg[0]->des_norm_y.bByOtherWing&&pJg[0]->des_norm_y.hViceJg<=0x20)
				pJg[0]->SetBriefWingNormY(vyWingNorm);
			else
				pJg[0]->cal_y_wing_norm(true);
		}
		if(pJg[1]&&style.ciMasterDirectIndex!=2)
		{
			GEPOINT vxTmpWingNorm=-vxWingNorm;
			GEPOINT vyTmpWingNorm=-vyWingNorm;
			double jdxx=pJg[1]->vxWingNorm*vxTmpWingNorm;
			double jdyy=pJg[1]->vyWingNorm*vyTmpWingNorm;
			double jdxy=pJg[1]->vxWingNorm*vyTmpWingNorm;
			double jdyx=pJg[1]->vyWingNorm*vxTmpWingNorm;
			if (fabs(jdxx)<0.91&&fabs(jdyx)>0.91)	//cos(25°)=0.91
			{	//颠倒两肢朝向
				DESIGN_JGWING_NORM xTempObj=pJg[1]->des_norm_x;
				pJg[1]->des_norm_x=pJg[1]->des_norm_y;
				pJg[1]->des_norm_y=xTempObj;
				if (jdyx<0)
					pJg[1]->des_norm_x.ReverseDirection();
				if (jdxy<0)
					pJg[1]->des_norm_y.ReverseDirection();
				pJg[1]->SetBriefWingNormX(vxTmpWingNorm);
				pJg[1]->SetBriefWingNormY(vyTmpWingNorm);
			}
			else if (jdxx<=-0.91&&jdyy<=-0.91)
			{	//两肢朝向反向
				pJg[1]->des_norm_x.ReverseDirection();
				pJg[1]->des_norm_y.ReverseDirection();
				pJg[1]->SetBriefWingNormX(vxTmpWingNorm);
				pJg[1]->SetBriefWingNormY(vyTmpWingNorm);
			}
			if(pJg[1]->des_norm_x.IsSpecNormActual())//!pJg[1]->des_norm_x.bByOtherWing&&pJg[1]->des_norm_x.hViceJg<=0x20)
				pJg[1]->SetBriefWingNormX(vxTmpWingNorm);
			else
				pJg[1]->cal_x_wing_norm(true);
			if(pJg[1]->des_norm_y.IsSpecNormActual())//!pJg[1]->des_norm_y.bByOtherWing&&pJg[1]->des_norm_y.hViceJg<=0x20)
				pJg[1]->SetBriefWingNormY(vyTmpWingNorm);
			else
				pJg[1]->cal_y_wing_norm(true);
		}
		break;
	case 1:	//T型组合
		pJg[0]=!flag.GetBitState(0)?NULL:(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[0],CLS_LINEANGLE);
		pJg[1]=!flag.GetBitState(1)?NULL:(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[1],CLS_LINEANGLE);
		if(style.ciMasterIndex==0)
		{
			if(pJg[0]&&style.ciMasterDirectIndex!=1)
			{
				double jdxx=pJg[0]->vxWingNorm*vxWingNorm;
				double jdyy=pJg[0]->vyWingNorm*vyWingNorm;
				double jdxy=pJg[0]->vxWingNorm*vyWingNorm;
				double jdyx=pJg[0]->vyWingNorm*vxWingNorm;
				if (fabs(jdxx)<0.91&&fabs(jdyx)>0.91)	//cos(25°)=0.91
				{	//颠倒两肢朝向
					DESIGN_JGWING_NORM xTempObj=pJg[0]->des_norm_x;
					pJg[0]->des_norm_x=pJg[0]->des_norm_y;
					pJg[0]->des_norm_y=xTempObj;
					if (jdyx<0)
						pJg[0]->des_norm_x.ReverseDirection();
					if (jdxy<0)
						pJg[0]->des_norm_y.ReverseDirection();
					pJg[0]->SetBriefWingNormX(vxWingNorm);
					pJg[0]->SetBriefWingNormY(vyWingNorm);
				}
				else if (jdxx<=-0.91&&jdyy<=-0.91)
				{	//两肢朝向反向
					pJg[0]->des_norm_x.ReverseDirection();
					pJg[0]->des_norm_y.ReverseDirection();
					pJg[0]->SetBriefWingNormX(vxWingNorm);
					pJg[0]->SetBriefWingNormY(vyWingNorm);
				}
				if(pJg[0]->des_norm_x.IsSpecNormActual())//!pJg[0]->des_norm_x.bByOtherWing&&pJg[0]->des_norm_x.hViceJg<=0x20)
					pJg[0]->SetBriefWingNormX(vxWingNorm);
				else
					pJg[0]->cal_x_wing_norm(true);
				if(pJg[0]->des_norm_y.IsSpecNormActual())//!pJg[0]->des_norm_y.bByOtherWing&&pJg[0]->des_norm_y.hViceJg<=0x20)
					pJg[0]->SetBriefWingNormY(vyWingNorm);
				else
					pJg[0]->cal_y_wing_norm(true);
			}
			if(pJg[1]&&style.ciMasterDirectIndex!=2)
			{
				GEPOINT vyTmpWingNorm=-vxWingNorm;
				GEPOINT vxTmpWingNorm= vyWingNorm;
				double jdxx=pJg[1]->vxWingNorm*vxTmpWingNorm;
				double jdyy=pJg[1]->vyWingNorm*vyTmpWingNorm;
				double jdxy=pJg[1]->vxWingNorm*vyTmpWingNorm;
				double jdyx=pJg[1]->vyWingNorm*vxTmpWingNorm;
				if (fabs(jdxx)<0.91&&fabs(jdyx)>0.91)	//cos(25°)=0.91
				{	//颠倒两肢朝向
					DESIGN_JGWING_NORM xTempObj=pJg[1]->des_norm_x;
					pJg[1]->des_norm_x=pJg[1]->des_norm_y;
					pJg[1]->des_norm_y=xTempObj;
					if (jdyx<0)
						pJg[1]->des_norm_x.ReverseDirection();
					if (jdxy<0)
						pJg[1]->des_norm_y.ReverseDirection();
				}
				else if (jdxx<=-0.91&&jdyy<=-0.91)
				{	//两肢朝向反向
					pJg[1]->des_norm_x.ReverseDirection();
					pJg[1]->des_norm_y.ReverseDirection();
				}
				pJg[1]->SetBriefWingNormX(vxTmpWingNorm);
				pJg[1]->SetBriefWingNormY(vyTmpWingNorm);
				if(!pJg[1]->des_norm_x.IsSpecNormActual())//!pJg[1]->des_norm_x.bByOtherWing&&pJg[1]->des_norm_x.hViceJg<=0x20)
					pJg[1]->cal_x_wing_norm(true);
				if(!pJg[1]->des_norm_y.IsSpecNormActual())//!pJg[1]->des_norm_y.bByOtherWing&&pJg[1]->des_norm_y.hViceJg<=0x20)
					pJg[1]->cal_y_wing_norm(true);
			}
		}
		else
		{
			if(pJg[0]&&style.ciMasterDirectIndex!=1)
			{
				GEPOINT vyTmpWingNorm= vxWingNorm;
				GEPOINT vxTmpWingNorm=-vyWingNorm;
				double jdxx=pJg[0]->vxWingNorm*vxTmpWingNorm;
				double jdyy=pJg[0]->vyWingNorm*vyTmpWingNorm;
				double jdxy=pJg[0]->vxWingNorm*vyTmpWingNorm;
				double jdyx=pJg[0]->vyWingNorm*vxTmpWingNorm;
				if (fabs(jdxx)<0.91&&fabs(jdyx)>0.91)	//cos(25°)=0.91
				{	//颠倒两肢朝向
					DESIGN_JGWING_NORM xTempObj=pJg[0]->des_norm_x;
					pJg[0]->des_norm_x=pJg[0]->des_norm_y;
					pJg[0]->des_norm_y=xTempObj;
					if (jdyx<0)
						pJg[0]->des_norm_x.ReverseDirection();
					if (jdxy<0)
						pJg[0]->des_norm_y.ReverseDirection();
				}
				else if (jdxx<=-0.91&&jdyy<=-0.91)
				{	//两肢朝向反向
					pJg[0]->des_norm_x.ReverseDirection();
					pJg[0]->des_norm_y.ReverseDirection();
				}
				pJg[0]->SetBriefWingNormX(vxTmpWingNorm);
				pJg[0]->SetBriefWingNormY(vyTmpWingNorm);
				if(!pJg[0]->des_norm_x.IsSpecNormActual())//!pJg[0]->des_norm_x.bByOtherWing&&pJg[0]->des_norm_x.hViceJg<=0x20)
					pJg[0]->cal_x_wing_norm(true);
				if(!pJg[0]->des_norm_y.IsSpecNormActual())//!pJg[0]->des_norm_y.bByOtherWing&&pJg[0]->des_norm_y.hViceJg<=0x20)
					pJg[0]->cal_y_wing_norm(true);
			}
			if(pJg[1]&&style.ciMasterDirectIndex!=2)
			{
				double jdxx=pJg[1]->vxWingNorm*vxWingNorm;
				double jdyy=pJg[1]->vyWingNorm*vyWingNorm;
				double jdxy=pJg[1]->vxWingNorm*vyWingNorm;
				double jdyx=pJg[1]->vyWingNorm*vxWingNorm;
				if (fabs(jdxx)<0.91&&fabs(jdyx)>0.91)	//cos(25°)=0.91
				{	//颠倒两肢朝向
					DESIGN_JGWING_NORM xTempObj=pJg[1]->des_norm_x;
					pJg[1]->des_norm_x=pJg[1]->des_norm_y;
					pJg[1]->des_norm_y=xTempObj;
					if (jdyx<0)
						pJg[1]->des_norm_x.ReverseDirection();
					if (jdxy<0)
						pJg[1]->des_norm_y.ReverseDirection();
				}
				else if (jdxx<=-0.91&&jdyy<=-0.91)
				{	//两肢朝向反向
					pJg[1]->des_norm_x.ReverseDirection();
					pJg[1]->des_norm_y.ReverseDirection();
				}
				pJg[1]->SetBriefWingNormX(vxWingNorm);
				pJg[1]->SetBriefWingNormY(vyWingNorm);
				if(!pJg[1]->des_norm_x.IsSpecNormActual())//!pJg[1]->des_norm_x.bByOtherWing&&pJg[1]->des_norm_x.hViceJg<=0x20)
					pJg[1]->cal_x_wing_norm(true);
				if(!pJg[1]->des_norm_y.IsSpecNormActual())//!pJg[1]->des_norm_y.bByOtherWing&&pJg[1]->des_norm_y.hViceJg<=0x20)
					pJg[1]->cal_y_wing_norm(true);
			}
		}
		break;
	case 2:	//十字交叉组合
		pJg[0]=!flag.GetBitState(0)?NULL:(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[0],CLS_LINEANGLE);
		pJg[1]=!flag.GetBitState(1)?NULL:(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[1],CLS_LINEANGLE);
		pJg[2]=!flag.GetBitState(2)?NULL:(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[2],CLS_LINEANGLE);
		pJg[3]=!flag.GetBitState(3)?NULL:(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[3],CLS_LINEANGLE);
		if(pJg[0]&&style.ciMasterDirectIndex!=1)
		{
			if(pJg[0]->des_norm_x.IsSpecNormActual())//!pJg[0]->des_norm_x.bByOtherWing&&pJg[0]->des_norm_x.hViceJg<=0x20)
				pJg[0]->SetBriefWingNormX(vxWingNorm);
			if(pJg[0]->des_norm_y.IsSpecNormActual())//!pJg[0]->des_norm_y.bByOtherWing&&pJg[0]->des_norm_y.hViceJg<=0x20)
				pJg[0]->SetBriefWingNormY(vyWingNorm);
		}
		if(pJg[1]&&style.ciMasterDirectIndex!=2)
		{
			if(pJg[1]->des_norm_x.IsSpecNormActual())//!pJg[1]->des_norm_x.bByOtherWing&&pJg[1]->des_norm_x.hViceJg<=0x20)
				pJg[1]->SetBriefWingNormX(vyWingNorm);
			if(pJg[1]->des_norm_y.IsSpecNormActual())//!pJg[1]->des_norm_y.bByOtherWing&&pJg[1]->des_norm_y.hViceJg<=0x20)
				pJg[1]->SetBriefWingNormY(-vxWingNorm);
		}
		if(pJg[2]&&style.ciMasterDirectIndex!=3)
		{
			if(pJg[2]->des_norm_x.IsSpecNormActual())//!pJg[2]->des_norm_x.bByOtherWing&&pJg[2]->des_norm_x.hViceJg<=0x20)
				pJg[2]->SetBriefWingNormX(-vxWingNorm);
			if(pJg[2]->des_norm_y.IsSpecNormActual())//!pJg[2]->des_norm_y.bByOtherWing&&pJg[2]->des_norm_y.hViceJg<=0x20)
				pJg[2]->SetBriefWingNormY(-vyWingNorm);
		}
		if(pJg[3]&&style.ciMasterDirectIndex!=4)
		{
			if(pJg[3]->des_norm_x.IsSpecNormActual())//!pJg[3]->des_norm_x.bByOtherWing&&pJg[3]->des_norm_x.hViceJg<=0x20)
				pJg[3]->SetBriefWingNormX(-vyWingNorm);
			if(pJg[3]->des_norm_y.IsSpecNormActual())//!pJg[3]->des_norm_y.bByOtherWing&&pJg[3]->des_norm_y.hViceJg<=0x20)
				pJg[3]->SetBriefWingNormY(vxWingNorm);
		}
		break;
	default:
		break;
	}
}

void CLDSGroupLineAngle::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	CLDSLineAngle::FromBuffer(buffer,version,doc_type);
	pLsRefList->Empty();
	buffer.ReadInteger(&style.dwStyle);
	if(doc_type==1||doc_type==2||doc_type==5)
	{
		group_style=style.wSectStyle;
		size.idClassType=CLS_GROUPLINEANGLE;
	}
	buffer.ReadDouble(&_jg_space);	//组合角钢间的间隙值
	buffer.ReadInteger(&m_bUpdateSonAngleOddment);
	buffer.ReadInteger(&son_jg_h[0]);
	buffer.ReadInteger(&son_jg_h[1]);
	buffer.ReadInteger(&son_jg_h[2]);
	buffer.ReadInteger(&son_jg_h[3]);
}
void CLDSGroupLineAngle::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	pLsRefList->Empty();
	CGroupAngleLifeObj m_xGroupAngleLife(this,version,doc_type);
	CLDSLineAngle::ToBuffer(buffer,version,doc_type);
	//使用group_style（相当于用size.cSubClassType)而不是_group_style 
	//避免出现_group_style与size冲突情况，尤其是T型组合角钢 wjh-2015.5.18
	style.wSectStyle=wSectStyle;	//此行代码用于从size.cSubClassType更新属性至style.dwStyle中，否则在打断杆件时，style.dwStyle部分组合角钢属性很可能未继承 wjh-2016.12.23
	if( (doc_type==PRODUCT_TMA&&version>0&&version<5010301)||	//TMA V5.1.3.1
		(doc_type==PRODUCT_LMA&&version>0&&version<2000601)||	//LMA V2.0.6.1
		(doc_type==PRODUCT_TDA&&version>0&&version<1030601)||	//TDA V1.3.6.1
		(doc_type==PRODUCT_LDS&&version>0&&version<1030601))	//LDS V1.3.6.1
		buffer.WriteInteger(wSectStyle);
	else
		buffer.WriteInteger(style.dwStyle);
	buffer.WriteDouble(jg_space);	//组合角钢间的间隙值
	buffer.WriteInteger(m_bUpdateSonAngleOddment);
	buffer.WriteInteger(son_jg_h[0]);
	buffer.WriteInteger(son_jg_h[1]);
	buffer.WriteInteger(son_jg_h[2]);
	buffer.WriteInteger(son_jg_h[3]);
}
void CLDSGroupLineAngle::ToNormInfoBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.SeekToBegin();
	buffer.WriteDouble(norm_x_wing.x);	
	buffer.WriteDouble(norm_x_wing.y);	
	buffer.WriteDouble(norm_x_wing.z);	
	buffer.WriteDouble(norm_y_wing.x);	
	buffer.WriteDouble(norm_y_wing.y);	
	buffer.WriteDouble(norm_y_wing.z);
	buffer.Write(&desStartPos.wing_x_offset,sizeof(WING_OFFSET_STYLE));
	buffer.Write(&desStartPos.wing_y_offset,sizeof(WING_OFFSET_STYLE));
	buffer.Write(&desEndPos.wing_x_offset,sizeof(WING_OFFSET_STYLE));
	buffer.Write(&desEndPos.wing_y_offset,sizeof(WING_OFFSET_STYLE));
	//
	buffer.WriteInteger(des_norm_x.bByOtherWing);
	buffer.WriteInteger(des_norm_x.bSpecific);
	buffer.WriteDouble(des_norm_x.near_norm.x);	
	buffer.WriteDouble(des_norm_x.near_norm.y);	 
	buffer.WriteDouble(des_norm_x.near_norm.z);	 
	des_norm_x.spec_norm.ToBuffer(buffer,version,doc_type);
	buffer.WriteInteger(des_norm_x.hViceJg);
	//
	buffer.WriteInteger(des_norm_y.bByOtherWing);
	buffer.WriteInteger(des_norm_y.bSpecific);
	buffer.WriteDouble(des_norm_y.near_norm.x);	 
	buffer.WriteDouble(des_norm_y.near_norm.y);	 
	buffer.WriteDouble(des_norm_y.near_norm.z);	 
	des_norm_y.spec_norm.ToBuffer(buffer,version,doc_type);
	buffer.WriteInteger(des_norm_y.hViceJg);
}
void CLDSGroupLineAngle::FromNormInfoBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.SeekToBegin();
	buffer.ReadDouble(&norm_x_wing.x);
	buffer.ReadDouble(&norm_x_wing.y);
	buffer.ReadDouble(&norm_x_wing.z);
	buffer.ReadDouble(&norm_y_wing.x);
	buffer.ReadDouble(&norm_y_wing.y);
	buffer.ReadDouble(&norm_y_wing.z);
	buffer.Read(&desStartPos.wing_x_offset,sizeof(WING_OFFSET_STYLE));
	buffer.Read(&desStartPos.wing_y_offset,sizeof(WING_OFFSET_STYLE));
	buffer.Read(&desEndPos.wing_x_offset,sizeof(WING_OFFSET_STYLE));
	buffer.Read(&desEndPos.wing_y_offset,sizeof(WING_OFFSET_STYLE));
	//
	buffer.ReadInteger(&des_norm_x.bByOtherWing);
	buffer.ReadInteger(&des_norm_x.bSpecific);
	buffer.ReadDouble(&des_norm_x.near_norm.x);
	buffer.ReadDouble(&des_norm_x.near_norm.y); 
	buffer.ReadDouble(&des_norm_x.near_norm.z); 
	des_norm_x.spec_norm.FromBuffer(buffer,version,doc_type);
	if(des_norm_x.spec_norm.norm_style==0)
		des_norm_x.spec_norm.vector=norm_x_wing;
	buffer.ReadInteger(&des_norm_x.hViceJg);
	//
	buffer.ReadInteger(&des_norm_y.bByOtherWing);
	buffer.ReadInteger(&des_norm_y.bSpecific);
	buffer.ReadDouble(&des_norm_y.near_norm.x);
	buffer.ReadDouble(&des_norm_y.near_norm.y); 
	buffer.ReadDouble(&des_norm_y.near_norm.z); 
	des_norm_y.spec_norm.FromBuffer(buffer,version,doc_type);
	if(des_norm_y.spec_norm.norm_style==0)
		des_norm_y.spec_norm.vector=norm_y_wing;
	buffer.ReadInteger(&des_norm_y.hViceJg);
}
void CLDSGroupLineAngle::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	pLsRefList->Empty();
	char sText[500]="";
	fprintf(fp,"<组合角钢 handle=\"0X%X\">\n",handle);
	CLDSLineAngle::ToXmlFile(fp,schema);
	if (style.wSectStyle==0)
		fprintf(fp,"<实体状态时的火曲角钢组合型式 _group_style=\"0.对角\"/>\n");
	else if (style.wSectStyle==1)
		fprintf(fp,"<实体状态时的火曲角钢组合型式 _group_style=\"1.T型\"/>\n");
	else if (style.wSectStyle==2)
		fprintf(fp,"<实体状态时的火曲角钢组合型式 _group_style=\"2.十字交叉型\"/>\n");
	sprintf(sText,"%f",jg_space);
	SimplifiedNumString(sText);
	fprintf(fp,"<组合角钢间的间隙值 jg_space=\"%s\"/>\n",sText);
	if (m_bUpdateSonAngleOddment)
		fprintf(fp,"<更新子角钢正负头 m_bUpdateSonAngleOddment=\"true\"/>\n");
	else
		fprintf(fp,"<更新子角钢正负头 m_bUpdateSonAngleOddment=\"false\"/>\n");
	fprintf(fp,"<子角钢句柄 xie_jg_handle=\"(0X%X,0X%X,0X%X,0X%X)\"/>\n",son_jg_h[0],son_jg_h[1],son_jg_h[2],son_jg_h[3]);	
	fprintf(fp,"</组合角钢>\n");
}
void CLDSGroupLineAngle::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/* =0 */,long doc_type/* =0 */)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if( version!=0&&
		(doc_type==1&&version<4020005)||	//TMA(V4.2.0.5)
		(doc_type==2&&version<1020005)||	//LMA(V1.2.0.5)
		(doc_type==3&&version<2000005)||	//TSA(V2.0.0.5)
		(doc_type==4&&version<1020005)||	//LDS(V1.2.0.5)
		(doc_type==5&&version<1020005))		//TDA(V1.2.0.5)
		return;

	buffer.ReadDword(&dwPermission);
	buffer.ReadDword(&dwStartPermission);
	buffer.ReadDword(&dwEndPermission);
	buffer.ReadString(layer(),4);
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	buffer.ReadInteger(&m_uStatMatCoef);
	buffer.ReadPoint(norm_x_wing);
	buffer.ReadPoint(norm_y_wing);
	buffer.ReadDouble(&nominal_length);
	buffer.ReadInteger(&kaihe_base_wing_x0_y1);
	buffer.ReadInteger(&m_bEnableTeG);
	buffer.ReadInteger(&xWingXZhunJu.g);
	buffer.ReadInteger(&xWingXZhunJu.g1);
	buffer.ReadInteger(&xWingXZhunJu.g2);
	buffer.ReadInteger(&xWingXZhunJu.g3);
	buffer.ReadInteger(&xWingYZhunJu.g);
	buffer.ReadInteger(&xWingYZhunJu.g1);
	buffer.ReadInteger(&xWingYZhunJu.g2);
	buffer.ReadInteger(&xWingYZhunJu.g3);
	buffer.ReadInteger(&m_bReplPart);
	buffer.ReadInteger(&m_bUpdateSonAngleOddment);
	buffer.ReadByte(&org_material);
	buffer.ReadDouble(&org_wing_wide);
	buffer.ReadDouble(&org_wing_thick);

	buffer.ReadInteger(&son_jg_h[0]);
	buffer.ReadInteger(&son_jg_h[1]);
	buffer.ReadInteger(&son_jg_h[2]);
	buffer.ReadInteger(&son_jg_h[3]);	
	buffer.ReadString(sNotes,51);
	long h=0;
	buffer.ReadInteger(&h);
	pStart=m_pModel->FromNodeHandle(h);	// 起点句柄
	buffer.ReadDouble(&cut_wing_para[0][0]);
	buffer.ReadDouble(&cut_wing_para[0][1]);
	buffer.ReadDouble(&cut_wing_para[0][2]);
	//进行切肢的主肢,0表示无切肢,1表示X肢,2表示Y肢
	buffer.ReadInteger(&cut_wing[0]);
	buffer.ReadDouble(&cut_angle[0][0]);
	buffer.ReadDouble(&cut_angle[0][1]);
	buffer.ReadDouble(&cut_angle[1][0]);
	buffer.ReadDouble(&cut_angle[1][1]);
	f3dPoint pt;
	buffer.ReadPoint(pt);
	SetStart(pt);
	double ff;
	buffer.ReadDouble(&ff);
	SetStartOdd(ff);
	buffer.ReadPoint(pre_huoqu_start_xyz);
	desStartPos.FromBuffer(buffer);
	buffer.ReadInteger(&connectStart.d);   	//(非水平角钢的上端或水平角钢的)起始端螺栓信息
	buffer.Read(&connectStart.uiLegacyN,4);
	buffer.Read(connectStart.grade,5);
	buffer.ReadInteger(&connectStart.rows);
	buffer.ReadInteger(&h);
	pEnd=m_pModel->FromNodeHandle(h);
	buffer.ReadDouble(&cut_wing_para[1][0]);
	buffer.ReadDouble(&cut_wing_para[1][1]);
	buffer.ReadDouble(&cut_wing_para[1][2]);
	buffer.ReadInteger(&cut_wing[1]);
	buffer.ReadDouble(&cut_angle[2][0]);
	buffer.ReadDouble(&cut_angle[2][1]);
	buffer.ReadDouble(&cut_angle[3][0]);
	buffer.ReadDouble(&cut_angle[3][1]);
	buffer.ReadPoint(pt);
	SetEnd(pt);
	buffer.ReadDouble(&ff);   // 角钢始终端正负头
	SetEndOdd(ff);
	buffer.ReadPoint(pre_huoqu_end_xyz);
	desEndPos.FromBuffer(buffer);
	buffer.ReadInteger(&connectEnd.d);   	//(非水平角钢的上端或水平角钢的)终始端螺栓信息
	buffer.Read(&connectEnd.uiLegacyN,4);
	buffer.Read(connectEnd.grade,5);
	buffer.ReadInteger(&connectEnd.rows);
}
void CLDSGroupLineAngle::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteDword(dwPermission);
	buffer.WriteDword(dwStartPermission);
	buffer.WriteDword(dwEndPermission);
	buffer.WriteString(layer());
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	buffer.WriteInteger(m_uStatMatCoef);
	buffer.WritePoint(get_norm_x_wing());
	buffer.WritePoint(get_norm_y_wing());
	buffer.WriteDouble(nominal_length);
	buffer.WriteInteger(kaihe_base_wing_x0_y1);
	buffer.WriteInteger(m_bEnableTeG);
	buffer.WriteInteger(xWingXZhunJu.g);
	buffer.WriteInteger(xWingXZhunJu.g1);
	buffer.WriteInteger(xWingXZhunJu.g2);
	buffer.WriteInteger(xWingXZhunJu.g3);
	buffer.WriteInteger(xWingYZhunJu.g);
	buffer.WriteInteger(xWingYZhunJu.g1);
	buffer.WriteInteger(xWingYZhunJu.g2);
	buffer.WriteInteger(xWingYZhunJu.g3);
	buffer.WriteInteger(m_bReplPart);
	buffer.WriteInteger(m_bUpdateSonAngleOddment);
	buffer.WriteByte((BYTE)org_material);
	buffer.WriteDouble(org_wing_wide);
	buffer.WriteDouble(org_wing_thick);

	buffer.WriteInteger(son_jg_h[0]);
	buffer.WriteInteger(son_jg_h[1]);
	buffer.WriteInteger(son_jg_h[2]);
	buffer.WriteInteger(son_jg_h[3]);	
	buffer.WriteString(sNotes);
	if(pStart)	
		buffer.WriteInteger(pStart->handle);
	else
		buffer.WriteInteger(0);
	buffer.WriteInteger(bHuoQuStart);
	buffer.WriteDouble(cut_wing_para[0][0]);
	buffer.WriteDouble(cut_wing_para[0][1]);
	buffer.WriteDouble(cut_wing_para[0][2]);
	//进行切肢的主肢,0表示无切肢,1表示X肢,2表示Y肢
	buffer.WriteInteger(cut_wing[0]);
	buffer.WriteDouble(cut_angle[0][0]);
	buffer.WriteDouble(cut_angle[0][1]);
	buffer.WriteDouble(cut_angle[1][0]);
	buffer.WriteDouble(cut_angle[1][1]);
	buffer.WritePoint(Start());  	// 实际楞线起点
	buffer.WriteDouble(startOdd());
	buffer.WritePoint(pre_huoqu_start_xyz);
	desStartPos.ToBuffer(buffer,version,doc_type);
	buffer.WriteInteger(connectStart.d);   	//(非水平角钢的上端或水平角钢的)起始端螺栓信息
	buffer.Write(&connectStart.uiLegacyN,4);
	buffer.Write(connectStart.grade,5);
	buffer.WriteInteger(connectStart.rows);
	if(pEnd)
		buffer.WriteInteger(pEnd->handle);
	else
		buffer.WriteInteger(0);
	buffer.WriteDouble(cut_wing_para[1][0]);
	buffer.WriteDouble(cut_wing_para[1][1]);
	buffer.WriteDouble(cut_wing_para[1][2]);
	buffer.WriteInteger(cut_wing[1]);
	buffer.WriteDouble(cut_angle[2][0]);
	buffer.WriteDouble(cut_angle[2][1]);
	buffer.WriteDouble(cut_angle[3][0]);
	buffer.WriteDouble(cut_angle[3][1]);
	buffer.WritePoint(End());  	// 实际楞线终点
	buffer.WriteDouble(endOdd());   // 角钢始终端正负头.
	buffer.WritePoint(pre_huoqu_end_xyz);
	desEndPos.ToBuffer(buffer,version,doc_type);
	buffer.WriteInteger(connectEnd.d);   	//(非水平角钢的上端或水平角钢的)终始端螺栓信息
	buffer.Write(&connectEnd.uiLegacyN,4);
	buffer.Write(connectEnd.grade,5);
	buffer.WriteInteger(connectEnd.rows);
}
//自协同中心通过管道读取数据
void CLDSGroupLineAngle::FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
{
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer))
	{
		IndependentShadowPropFromBuffer(buffer);
		return;
	}
	long h;
	buffer.ReadDword(&dwPermission);
	buffer.ReadDword(&dwStartPermission);
	buffer.ReadDword(&dwEndPermission);
	if(dwPermission&dwUpdateWord)
	{
		buffer.ReadString(layer(),4);
		buffer.Read(cfgword.flag.word,24);
		buffer.ReadInteger(&m_uStatMatCoef);
		buffer.ReadPoint(norm_x_wing);
		buffer.ReadPoint(norm_y_wing);
		buffer.ReadBooleanThin(&_bVirtualPart);			//	隐含角钢
		buffer.ReadDouble(&nominal_length);
		buffer.ReadInteger(&kaihe_base_wing_x0_y1);
		buffer.ReadInteger(&huoqu_base_wing_x0_y1_both2);
		buffer.ReadInteger(&huoqu_start1_end2);
		//角钢特殊心线数据
		buffer.ReadInteger(&m_bEnableTeG);
		buffer.ReadInteger(&xWingXZhunJu.g);
		buffer.ReadInteger(&xWingXZhunJu.g1);
		buffer.ReadInteger(&xWingXZhunJu.g2);
		buffer.ReadInteger(&xWingXZhunJu.g3);
		buffer.ReadInteger(&xWingYZhunJu.g);
		buffer.ReadInteger(&xWingYZhunJu.g1);
		buffer.ReadInteger(&xWingYZhunJu.g2);
		buffer.ReadInteger(&xWingYZhunJu.g3);
//#if defined(__LDS_FILE_)
		buffer.ReadInteger(&size.idClassType);
		buffer.ReadByte(&size.cSubClassType);
//#endif
		double ff;
		buffer.ReadDouble(&ff);
		set_wing_wide(ff);
		buffer.ReadDouble(&ff);
		set_wing_thick(ff);
		buffer.ReadByte(&_material);
		buffer.ReadInteger(&m_bReplPart);
		buffer.ReadInteger(&m_bUpdateSonAngleOddment);
		buffer.ReadByte(&org_material);
		buffer.ReadDouble(&org_wing_wide);
		buffer.ReadDouble(&org_wing_thick);

		buffer.ReadInteger(&_iSeg.iSeg);
		buffer.ReadString(layer(),4);					// 本角钢所在图层.
		buffer.ReadInteger(&son_jg_h[0]);
		buffer.ReadInteger(&son_jg_h[1]);
		buffer.ReadInteger(&son_jg_h[2]);
		buffer.ReadInteger(&son_jg_h[3]);
		buffer.ReadBooleanThin(&m_bDisplayHole);		//及显示螺栓孔
		//设计信息
		buffer.ReadInteger(&des_norm_x.bByOtherWing);
		buffer.ReadInteger(&des_norm_x.bSpecific);
		buffer.ReadPoint(des_norm_x.near_norm);
		des_norm_x.spec_norm.FromBuffer(buffer);
		buffer.ReadInteger(&des_norm_x.hViceJg);
		buffer.ReadInteger(&des_norm_y.bByOtherWing);
		buffer.ReadInteger(&des_norm_y.bSpecific);
		buffer.ReadPoint(des_norm_y.near_norm);
		des_norm_y.spec_norm.FromBuffer(buffer);
		buffer.ReadInteger(&des_norm_y.hViceJg);
		buffer.ReadDouble(&huoqu_r);
		buffer.ReadInteger(&huoqu_handle);
		buffer.ReadString(sNotes,51);
		//组合角钢专有数据
		buffer.ReadInteger(&style.dwStyle);	//火曲角钢组合型式0:对角;1:T型;2:十字交叉型
		group_style=style.wSectStyle;
		buffer.ReadDouble(&_jg_space);	//子角钢之间间隙
	}
	if(dwStartPermission&dwUpdateWord)
	{
		//角钢始端基本信息
		buffer.ReadInteger(&h);
		pStart=m_pModel->FromNodeHandle(h);	// 起点句柄
		buffer.ReadInteger(&bHuoQuStart);
		// --------- 1.2. 切角 -----------
		/*cut_wing_para[0]表示始端切肢cut_wing_para[1]表示终端切肢
		...[][0]表示X轴外皮切取长度
		...[][1]表示Y轴外皮切取长度
		...[][2]表示Z轴外皮切取长度
		*/
		buffer.ReadDouble(&cut_wing_para[0][0]);
		buffer.ReadDouble(&cut_wing_para[0][1]);
		buffer.ReadDouble(&cut_wing_para[0][2]);
		//进行切肢的主肢,0表示无切肢,1表示X肢,2表示Y肢
		buffer.ReadInteger(&cut_wing[0]);
		// --------- 1.3. 切肢 -----------
		/*
		cut_angle[0]表示起点X肢
		cut_angle[1]表示起点Y肢
		cut_angle[2]表示终点X肢
		cut_angle[3]表示终点Y肢
		...[][0]表示平行楞线方向切取量  ...[][1]表示垂直楞线方向切取量
		*/
		buffer.ReadDouble(&cut_angle[0][0]);
		buffer.ReadDouble(&cut_angle[0][1]);
		buffer.ReadDouble(&cut_angle[1][0]);
		buffer.ReadDouble(&cut_angle[1][1]);
		f3dPoint pt;
		buffer.ReadPoint(pt);
		UnlockStartPos();
		SetStart(pt);  	// 实际楞线起点
		double ff;
		buffer.ReadDouble(&ff);
		SetStartOdd(ff);
		BOOL lock;
		buffer.ReadInteger(&lock);
		if(lock)
			LockStartPos();
		else
			UnlockStartPos();
		buffer.ReadPoint(pre_huoqu_start_xyz);
		desStartPos.FromBuffer(buffer);
		/*buffer.ReadInteger(&des_start_pos.jgber_cal_style);
		buffer.ReadInteger(&des_start_pos.datum_pos_style);
		buffer.ReadInteger(&des_start_pos.datum_to_ber_style);
		des_start_pos.datum_point.FromBuffer(buffer);
		buffer.ReadWord(&des_start_pos.huoqu_pos_auto_0_x_wing1_y_wing2);
		buffer.ReadInteger(&des_start_pos.prj_type);
		buffer.ReadDouble(&des_start_pos.datum_offset_dist);
		buffer.ReadDouble(&des_start_pos.offset_X_dist);
		buffer.ReadInteger(&des_start_pos.offset_X_dist_style);
		buffer.ReadDouble(&des_start_pos.offset_Y_dist);
		buffer.ReadInteger(&des_start_pos.offset_Y_dist_style);
		buffer.ReadInteger(&des_start_pos.datum_jg_h);
		buffer.ReadInteger(&des_start_pos.start_jg_h);
		buffer.ReadInteger(&des_start_pos.end_jg_h);
		buffer.ReadInteger(&des_start_pos.iStartJgBerStyle);
		buffer.ReadInteger(&des_start_pos.iEndJgBerStyle);
		buffer.ReadInteger(&des_start_pos.huoqu_jg_h);			//火曲基准角钢句柄
		buffer.ReadPoint(des_start_pos.face_offset_norm);//平推面法线
		buffer.ReadDouble(&des_start_pos.huoqu_dist);			//火曲线间隙
		buffer.ReadDouble(&des_start_pos.offset_zhun);			//偏移准线的偏移量
		buffer.ReadInteger(&des_start_pos.bByOtherEndOffset);	//根据另一端进行平推计算
		buffer.ReadDouble(&des_start_pos.fStartJgLengOffsetX);
		buffer.ReadDouble(&des_start_pos.fStartJgLengOffsetY);
		buffer.ReadDouble(&des_start_pos.fEndJgLengOffsetX);
		buffer.ReadDouble(&des_start_pos.fEndJgLengOffsetY);*/
		buffer.ReadInteger(&connectStart.d);   	//(非水平角钢的上端或水平角钢的)起始端螺栓信息
		buffer.Read(&connectStart.uiLegacyN,4);
		buffer.Read(connectStart.grade,5);   	//(非水平角钢的上端或水平角钢的)起始端螺栓信息
		buffer.ReadInteger(&connectStart.rows);
	}
	//角钢终端基本信息
	if(dwEndPermission&dwUpdateWord)
	{
		buffer.ReadInteger(&h);
		pEnd=m_pModel->FromNodeHandle(h);	// 终点句柄
		buffer.ReadInteger(&bHuoQuEnd);
		// --------- 1.2. 切角 -----------
		/*cut_wing_para[0]表示始端切肢cut_wing_para[1]表示终端切肢
		...[][0]表示X轴外皮切取长度
		...[][1]表示Y轴外皮切取长度
		...[][2]表示Z轴外皮切取长度
		*/
		buffer.ReadDouble(&cut_wing_para[1][0]);
		buffer.ReadDouble(&cut_wing_para[1][1]);
		buffer.ReadDouble(&cut_wing_para[1][2]);
		//进行切肢的主肢,0表示无切肢,1表示X肢,2表示Y肢
		buffer.ReadInteger(&cut_wing[1]);
		// --------- 1.3. 切肢 -----------
		/*
		cut_angle[0]表示起点X肢
		cut_angle[1]表示起点Y肢
		cut_angle[2]表示终点X肢
		cut_angle[3]表示终点Y肢
		...[][0]表示平行楞线方向切取量  ...[][1]表示垂直楞线方向切取量
		*/
		buffer.ReadDouble(&cut_angle[2][0]);
		buffer.ReadDouble(&cut_angle[2][1]);
		buffer.ReadDouble(&cut_angle[3][0]);
		buffer.ReadDouble(&cut_angle[3][1]);
		f3dPoint pt;
		buffer.ReadPoint(pt);
		UnlockEndPos();
		SetEnd(pt);  	// 实际楞线终点
		double ff;
		buffer.ReadDouble(&ff);
		SetEndOdd(ff);   // 角钢始终端正负头.
		BOOL lock;
		buffer.ReadInteger(&lock);
		if(lock)
			LockEndPos();
		else
			UnlockEndPos();
		buffer.ReadPoint(pre_huoqu_end_xyz);
		desEndPos.FromBuffer(buffer);
		/*buffer.ReadInteger(&des_end_pos.jgber_cal_style);
		buffer.ReadInteger(&des_end_pos.datum_pos_style);
		buffer.ReadInteger(&des_end_pos.datum_to_ber_style);
		des_end_pos.datum_point.FromBuffer(buffer);
		buffer.ReadWord(&des_end_pos.huoqu_pos_auto_0_x_wing1_y_wing2);
		buffer.ReadInteger(&des_end_pos.prj_type);
		buffer.ReadDouble(&des_end_pos.datum_offset_dist);
		buffer.ReadDouble(&des_end_pos.offset_X_dist);
		buffer.ReadInteger(&des_end_pos.offset_X_dist_style);
		buffer.ReadDouble(&des_end_pos.offset_Y_dist);
		buffer.ReadInteger(&des_end_pos.offset_Y_dist_style);
		buffer.ReadInteger(&des_end_pos.datum_jg_h);
		buffer.ReadInteger(&des_end_pos.start_jg_h);
		buffer.ReadInteger(&des_end_pos.end_jg_h);
		buffer.ReadInteger(&des_end_pos.iStartJgBerStyle);
		buffer.ReadInteger(&des_end_pos.iEndJgBerStyle);
		buffer.ReadInteger(&des_end_pos.huoqu_jg_h);				//火曲基准角钢句柄
		buffer.ReadPoint(des_end_pos.face_offset_norm);	//平推面法线方向
		buffer.ReadDouble(&des_end_pos.huoqu_dist);				//火曲线间隙
		buffer.ReadDouble(&des_end_pos.offset_zhun);			//偏移准线的偏移量
		buffer.ReadInteger(&des_end_pos.bByOtherEndOffset);	//根据另一端进行平推计算
		buffer.ReadDouble(&des_end_pos.fStartJgLengOffsetX);
		buffer.ReadDouble(&des_end_pos.fStartJgLengOffsetY);
		buffer.ReadDouble(&des_end_pos.fEndJgLengOffsetX);
		buffer.ReadDouble(&des_end_pos.fEndJgLengOffsetY);*/
		buffer.ReadInteger(&connectEnd.d);   	//(非水平角钢的上端或水平角钢的)终始端螺栓信息
		buffer.Read(&connectEnd.uiLegacyN,4);
		buffer.Read(connectEnd.grade,5);   	//(非水平角钢的上端或水平角钢的)起始端螺栓信息
		buffer.ReadInteger(&connectEnd.rows);
	}
	SetModified();
}
//通过管道向协同中心写入数据
void CLDSGroupLineAngle::ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdToBuffer(buffer))
	{
		IndependentShadowPropToBuffer(buffer);
		return;
	}
	buffer.WriteDword(dwPermission);
	buffer.WriteDword(dwStartPermission);
	buffer.WriteDword(dwEndPermission);
	if(dwPermission&dwUpdateWord)
	{	//角钢基本信息
		buffer.WriteString(layer());
		buffer.Write(cfgword.flag.word,24);
		buffer.WriteInteger(m_uStatMatCoef);
		buffer.WritePoint(get_norm_x_wing());
		buffer.WritePoint(get_norm_y_wing());
		buffer.WriteBooleanThin(m_bVirtualPart);			//	隐含角钢
		buffer.WriteDouble(nominal_length);
		buffer.WriteInteger(kaihe_base_wing_x0_y1);
		buffer.WriteInteger(huoqu_base_wing_x0_y1_both2);
		buffer.WriteInteger(huoqu_start1_end2);
		//角钢特殊心线数据
		buffer.WriteInteger(m_bEnableTeG);
		buffer.WriteInteger(xWingXZhunJu.g);
		buffer.WriteInteger(xWingXZhunJu.g1);
		buffer.WriteInteger(xWingXZhunJu.g2);
		buffer.WriteInteger(xWingXZhunJu.g3);
		buffer.WriteInteger(xWingYZhunJu.g);
		buffer.WriteInteger(xWingYZhunJu.g1);
		buffer.WriteInteger(xWingYZhunJu.g2);
		buffer.WriteInteger(xWingYZhunJu.g3);
//#ifdef __LDS_FILE_
		buffer.WriteInteger(size_idClassType);
		buffer.WriteByte(size_cSubClassType);
//#endif
		
		buffer.WriteDouble(GetWidth());
		buffer.WriteDouble(GetThick());
		buffer.WriteByte((BYTE)cMaterial);
		buffer.WriteInteger(m_bReplPart);
		buffer.WriteInteger(m_bUpdateSonAngleOddment);
		buffer.WriteByte((BYTE)org_material);
		buffer.WriteDouble(org_wing_wide);
		buffer.WriteDouble(org_wing_thick);
		
		buffer.WriteInteger(iSeg);
		buffer.WriteString(layer());					// 本角钢所在图层.
		buffer.WriteInteger(son_jg_h[0]);
		buffer.WriteInteger(son_jg_h[1]);
		buffer.WriteInteger(son_jg_h[2]);
		buffer.WriteInteger(son_jg_h[3]);
		buffer.WriteBooleanThin(m_bDisplayHole);		//及显示螺栓孔
		//设计信息
		buffer.WriteInteger(des_norm_x.bByOtherWing);
		buffer.WriteInteger(des_norm_x.bSpecific);
		buffer.WritePoint(des_norm_x.near_norm);
		des_norm_x.spec_norm.ToBuffer(buffer);
		buffer.WriteInteger(des_norm_x.hViceJg);
		buffer.WriteInteger(des_norm_y.bByOtherWing);
		buffer.WriteInteger(des_norm_y.bSpecific);
		buffer.WritePoint(des_norm_y.near_norm);
		des_norm_y.spec_norm.ToBuffer(buffer);
		buffer.WriteInteger(des_norm_y.hViceJg);
		buffer.WriteDouble(huoqu_r);
		buffer.WriteInteger(huoqu_handle);
		buffer.WriteString(sNotes);
		//组合角钢专有数据
		//此行代码用于从size.cSubClassType更新属性至style.dwStyle中，否则在打断杆件时，style.dwStyle部分组合角钢属性很可能未继承 wjh-2016.12.23
		style.wSectStyle=wSectStyle;
		buffer.WriteInteger(style.dwStyle);	//火曲角钢组合型式0:对角;1:T型;2:十字交叉型
		buffer.WriteDouble(jg_space);	//子角钢之间间隙
	}
	if(dwStartPermission&dwUpdateWord)
	{	//角钢始端基本信息
		if(pStart)	// 起点句柄
			buffer.WriteInteger(pStart->handle);
		else
			buffer.WriteInteger(0);
		buffer.WriteInteger(bHuoQuStart);
		// --------- 1.2. 切角 -----------
		/*cut_wing_para[0]表示始端切肢cut_wing_para[1]表示终端切肢
		...[][0]表示X轴外皮切取长度
		...[][1]表示Y轴外皮切取长度
		...[][2]表示Z轴外皮切取长度
		*/
		buffer.WriteDouble(cut_wing_para[0][0]);
		buffer.WriteDouble(cut_wing_para[0][1]);
		buffer.WriteDouble(cut_wing_para[0][2]);
		//进行切肢的主肢,0表示无切肢,1表示X肢,2表示Y肢
		buffer.WriteInteger(cut_wing[0]);
		// --------- 1.3. 切肢 -----------
		/*
		cut_angle[0]表示起点X肢
		cut_angle[1]表示起点Y肢
		cut_angle[2]表示终点X肢
		cut_angle[3]表示终点Y肢
		...[][0]表示平行楞线方向切取量  ...[][1]表示垂直楞线方向切取量
		*/
		buffer.WriteDouble(cut_angle[0][0]);
		buffer.WriteDouble(cut_angle[0][1]);
		buffer.WriteDouble(cut_angle[1][0]);
		buffer.WriteDouble(cut_angle[1][1]);
		buffer.WritePoint(Start());  	// 实际楞线起点
		buffer.WriteDouble(startOdd());
		buffer.WriteInteger(IsStartPosLocked());
		buffer.WritePoint(pre_huoqu_start_xyz);
		desStartPos.ToBuffer(buffer);
		/*buffer.WriteInteger(des_start_pos.jgber_cal_style);
		buffer.WriteInteger(des_start_pos.datum_pos_style);
		buffer.WriteInteger(des_start_pos.datum_to_ber_style);
		des_start_pos.datum_point.ToBuffer(buffer);
		buffer.WriteWord(des_start_pos.huoqu_pos_auto_0_x_wing1_y_wing2);
		buffer.WriteInteger(des_start_pos.prj_type);
		buffer.WriteDouble(des_start_pos.datum_offset_dist);
		buffer.WriteDouble(des_start_pos.offset_X_dist);
		buffer.WriteInteger(des_start_pos.offset_X_dist_style);
		buffer.WriteDouble(des_start_pos.offset_Y_dist);
		buffer.WriteInteger(des_start_pos.offset_Y_dist_style);
		buffer.WriteInteger(des_start_pos.datum_jg_h);
		buffer.WriteInteger(des_start_pos.start_jg_h);
		buffer.WriteInteger(des_start_pos.end_jg_h);
		buffer.WriteInteger(des_start_pos.iStartJgBerStyle);
		buffer.WriteInteger(des_start_pos.iEndJgBerStyle);
		buffer.WriteInteger(des_start_pos.huoqu_jg_h);			//火曲基准角钢句柄
		buffer.WritePoint(des_start_pos.face_offset_norm);//平推面法线
		buffer.WriteDouble(des_start_pos.huoqu_dist);			//火曲线间隙
		buffer.WriteDouble(des_start_pos.offset_zhun);			//偏移准线的偏移量
		buffer.WriteInteger(des_start_pos.bByOtherEndOffset);	//根据另一端进行平推计算
		buffer.WriteDouble(des_start_pos.fStartJgLengOffsetX);
		buffer.WriteDouble(des_start_pos.fStartJgLengOffsetY);
		buffer.WriteDouble(des_start_pos.fEndJgLengOffsetX);
		buffer.WriteDouble(des_start_pos.fEndJgLengOffsetY);*/
		buffer.WriteInteger(connectStart.d);   	//(非水平角钢的上端或水平角钢的)起始端螺栓信息
		buffer.Write(&connectStart.uiLegacyN,4);
		buffer.Write(connectStart.grade,5);
		buffer.WriteInteger(connectStart.rows);
	}
	if(dwEndPermission&dwUpdateWord)
	{	//角钢终端基本信息
		if(pEnd)	// 终点句柄
			buffer.WriteInteger(pEnd->handle);
		else
			buffer.WriteInteger(0);
		buffer.WriteInteger(bHuoQuEnd);
		// --------- 1.2. 切角 -----------
		/*cut_wing_para[0]表示始端切肢cut_wing_para[1]表示终端切肢
		...[][0]表示X轴外皮切取长度
		...[][1]表示Y轴外皮切取长度
		...[][2]表示Z轴外皮切取长度
		*/
		buffer.WriteDouble(cut_wing_para[1][0]);
		buffer.WriteDouble(cut_wing_para[1][1]);
		buffer.WriteDouble(cut_wing_para[1][2]);
		//进行切肢的主肢,0表示无切肢,1表示X肢,2表示Y肢
		buffer.WriteInteger(cut_wing[1]);
		// --------- 1.3. 切肢 -----------
		/*
		cut_angle[0]表示起点X肢
		cut_angle[1]表示起点Y肢
		cut_angle[2]表示终点X肢
		cut_angle[3]表示终点Y肢
		...[][0]表示平行楞线方向切取量  ...[][1]表示垂直楞线方向切取量
		*/
		buffer.WriteDouble(cut_angle[2][0]);
		buffer.WriteDouble(cut_angle[2][1]);
		buffer.WriteDouble(cut_angle[3][0]);
		buffer.WriteDouble(cut_angle[3][1]);
		buffer.WritePoint(End());  	// 实际楞线终点
		buffer.WriteDouble(endOdd());   // 角钢始终端正负头.
		buffer.WriteInteger(IsEndPosLocked());
		buffer.WritePoint(pre_huoqu_end_xyz);
		desEndPos.ToBuffer(buffer);
		/*buffer.WriteInteger(des_end_pos.jgber_cal_style);
		buffer.WriteInteger(des_end_pos.datum_pos_style);
		buffer.WriteInteger(des_end_pos.datum_to_ber_style);
		des_end_pos.datum_point.ToBuffer(buffer);
		buffer.WriteWord(des_end_pos.huoqu_pos_auto_0_x_wing1_y_wing2);
		buffer.WriteInteger(des_end_pos.prj_type);
		buffer.WriteDouble(des_end_pos.datum_offset_dist);
		buffer.WriteDouble(des_end_pos.offset_X_dist);
		buffer.WriteInteger(des_end_pos.offset_X_dist_style);
		buffer.WriteDouble(des_end_pos.offset_Y_dist);
		buffer.WriteInteger(des_end_pos.offset_Y_dist_style);
		buffer.WriteInteger(des_end_pos.datum_jg_h);
		buffer.WriteInteger(des_end_pos.start_jg_h);
		buffer.WriteInteger(des_end_pos.end_jg_h);
		buffer.WriteInteger(des_end_pos.iStartJgBerStyle);
		buffer.WriteInteger(des_end_pos.iEndJgBerStyle);
		buffer.WriteInteger(des_end_pos.huoqu_jg_h);				//火曲基准角钢句柄
		buffer.WritePoint(des_end_pos.face_offset_norm);	//平推面法线方向
		buffer.WriteDouble(des_end_pos.huoqu_dist);				//火曲线间隙
		buffer.WriteDouble(des_end_pos.offset_zhun);			//偏移准线的偏移量
		buffer.WriteInteger(des_end_pos.bByOtherEndOffset);	//根据另一端进行平推计算
		buffer.WriteDouble(des_end_pos.fStartJgLengOffsetX);
		buffer.WriteDouble(des_end_pos.fStartJgLengOffsetY);
		buffer.WriteDouble(des_end_pos.fEndJgLengOffsetX);
		buffer.WriteDouble(des_end_pos.fEndJgLengOffsetY);*/
		buffer.WriteInteger(connectEnd.d);   	//(非水平角钢的上端或水平角钢的)终始端螺栓信息
		buffer.Write(&connectEnd.uiLegacyN,4);
		buffer.Write(connectEnd.grade,5);
		buffer.WriteInteger(connectEnd.rows);
	}
}

void CLDSGroupLineAngle::UpdateSonJgBasicInfo()
{
	for(int i=0;i<4;i++)
	{
		CLDSLineAngle *pJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[i],CLS_LINEANGLE);
		if(pJg)
		{
			pJg->cMaterial=cMaterial;
			pJg->cfgword=cfgword;
			pJg->m_bDisplayHole=m_bDisplayHole;
			pJg->m_bEnableTeG=m_bEnableTeG;
			pJg->m_bReplPart=m_bReplPart;
			pJg->org_material=org_material;
			pJg->org_wing_thick=org_wing_thick;
			pJg->org_wing_wide=org_wing_wide;
			pJg->pStart=pStart;
			pJg->pEnd=pEnd;
			pJg->connectStart=connectStart;
			pJg->connectEnd=connectEnd;
			/* 以下代码有两个错误，应在螺栓计算数->连接数的集中转换中统一处理： 
			 * 1）group_style==3应该为==2表示四拼角钢；
			 * 2）不能在此处简单的将螺栓计算数除以子角钢数 wjh-2019.1.20
			if(group_style==3)
			{
				pJg->connectStart.N=max(2,connectStart.N/4);
				pJg->connectEnd.N=max(2,connectEnd.N/4);
			}
			else
			{
				pJg->connectStart.N=max(2,connectStart.N/2);
				pJg->connectEnd.N=max(2,connectEnd.N/2);
			}*/
			pJg->set_wing_wide(size_wide);
			pJg->set_wing_thick(size_thick);
			pJg->SetLayer(layer());
			pJg->iSeg=iSeg;
			if (m_bEnableTeG)
			{	//启用特殊准据
				if(this->wSectStyle==1&&ciMasterIndex!=i)
				{	//T型组合角钢的非特征子角钢与组合角钢两肢需要调换 wjh-2019.8.15
					pJg->xWingXZhunJu.g  = xWingYZhunJu.g;
					pJg->xWingXZhunJu.g1 = xWingYZhunJu.g1;
					pJg->xWingXZhunJu.g2 = xWingYZhunJu.g2;
					pJg->xWingXZhunJu.g3 = xWingYZhunJu.g3;
					pJg->xWingYZhunJu.g  = xWingXZhunJu.g;
					pJg->xWingYZhunJu.g1 = xWingXZhunJu.g1;
					pJg->xWingYZhunJu.g2 = xWingXZhunJu.g2;
					pJg->xWingYZhunJu.g3 = xWingXZhunJu.g3;
				}
				else
				{
					pJg->xWingXZhunJu.g = xWingXZhunJu.g;
					pJg->xWingXZhunJu.g1 = xWingXZhunJu.g1;
					pJg->xWingXZhunJu.g2 = xWingXZhunJu.g2;
					pJg->xWingXZhunJu.g3 = xWingXZhunJu.g3;
					pJg->xWingYZhunJu.g = xWingYZhunJu.g;
					pJg->xWingYZhunJu.g1 = xWingYZhunJu.g1;
					pJg->xWingYZhunJu.g2 = xWingYZhunJu.g2;
					pJg->xWingYZhunJu.g3 = xWingYZhunJu.g3;
				}
			}
#if defined(__LDS_)||defined(__TSA_)
			pJg->m_fWeightRaiseCoef=m_fWeightRaiseCoef;
#endif
		}
		else
			break;
	}
}

void CLDSGroupLineAngle::CalPosition(bool bIgnoreRepeatFlag/*=FALSE*/)
{
	CLDSLineAngle::CalPosition(bIgnoreRepeatFlag);
	UpdateSonJgPos();
}

bool CLDSGroupLineAngle::SetDirectMasterAngle()
{	//设定组合角钢自身为组合角钢及子角钢的肢法线计算基准角钢 wjh-2016.9.19
	ciMasterDirectIndex=0;
	return true;
}
void CLDSGroupLineAngle::cal_x_wing_norm()
{
	if(style.ciMasterDirectIndex==0)
		CLDSLineAngle::cal_x_wing_norm();
	else
	{
		CLDSLineAngle* pSonAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[style.ciMasterDirectIndex-1],CLS_LINEANGLE);
		if(pSonAngle&&style.ciMasterIndex==style.ciMasterDirectIndex-1)
			pSonAngle->cal_x_wing_norm();
		else
		{
			pSonAngle->cal_x_wing_norm();
			pSonAngle->cal_y_wing_norm();
		}
	}
}

void CLDSGroupLineAngle::cal_y_wing_norm()
{
	if(style.ciMasterDirectIndex==0)
		CLDSLineAngle::cal_y_wing_norm();
	else
	{
		CLDSLineAngle* pSonAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[style.ciMasterDirectIndex-1],CLS_LINEANGLE);
		if(pSonAngle&&style.ciMasterIndex==style.ciMasterDirectIndex-1)
			pSonAngle->cal_y_wing_norm();
		else
		{
			pSonAngle->cal_x_wing_norm();
			pSonAngle->cal_y_wing_norm();
		}
	}
}
static void UpdateSonAngleBasicInfo(CLDSLineAngle* pGroupAngle,CLDSLineAngle* pSonAngle)
{	//拷贝基本信息，否则可能因在Parts.InstantiateCursorLinePart()中调用可能因未初始化导致不能及时显示的错误 wjh-2019.10.15
	pSonAngle->cMaterial=pGroupAngle->cMaterial;
	pSonAngle->cfgword=pGroupAngle->cfgword;
	pSonAngle->m_bDisplayHole=pGroupAngle->m_bDisplayHole;
	pSonAngle->set_wing_wide(pGroupAngle->size_wide);
	pSonAngle->set_wing_thick(pGroupAngle->size_thick);
	pSonAngle->SetLayer(pGroupAngle->szLayer);
	pSonAngle->iSeg=pGroupAngle->iSeg;
	pSonAngle->m_bVirtualPart=false;
}
int CLDSGroupLineAngle::AppendSonJg()
{
	CLDSLineAngle *pJg;
	int nHits=0;
	//组合角钢初始化时都会调用AppendSonJg函数,因此在此处进行实体子角钢布材型式赋值 wjh-2013-2-19
	style.wSectStyle=group_style;
	if(son_jg_h[0]<0x20)
	{
		pJg=(CLDSLineAngle*)BelongModel()->AppendPart(CLS_LINEANGLE);
		pJg->group_father_jg_h=handle;
		pJg->pStart = pStart;
		pJg->pEnd = pEnd;
		pJg->SetStart(Start());
		pJg->SetEnd(End());
		//拷贝基本信息，否则可能因在Parts.InstantiateCursorLinePart()中调用可能因未初始化导致不能及时显示的错误 wjh-2019.10.15
		UpdateSonAngleBasicInfo(this,pJg);
		son_jg_h[0]=pJg->handle;
		nHits++;
	}
	if(son_jg_h[1]<0x20)
	{
		pJg=(CLDSLineAngle*)BelongModel()->AppendPart(CLS_LINEANGLE);
		pJg->group_father_jg_h=handle;
		pJg->pStart = pStart;
		pJg->pEnd = pEnd;
		pJg->SetStart(Start());
		pJg->SetEnd(End());
		//拷贝基本信息，否则可能因在Parts.InstantiateCursorLinePart()中调用可能因未初始化导致不能及时显示的错误 wjh-2019.10.15
		UpdateSonAngleBasicInfo(this,pJg);
		son_jg_h[1]=pJg->handle;
		nHits++;
	}
	/* 相关功能逻辑已移至UpdateSonAngleBasicInfo()中
	for(int i=0;i<2;i++)
	{
		pJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[i],CLS_LINEANGLE);
		pJg->m_bVirtualPart=false;
	}*/
	if(group_style==2)	//十字交叉型组合
	{
		if(son_jg_h[2]<0x20)
		{
			pJg=(CLDSLineAngle*)BelongModel()->AppendPart(CLS_LINEANGLE);
			pJg->group_father_jg_h=handle;
			pJg->pStart = pStart;
			pJg->pEnd = pEnd;
			pJg->SetStart(Start());
			pJg->SetEnd(End());
			//拷贝基本信息，否则可能因在Parts.InstantiateCursorLinePart()中调用可能因未初始化导致不能及时显示的错误 wjh-2019.10.15
			UpdateSonAngleBasicInfo(this,pJg);
			son_jg_h[2]=pJg->handle;
			nHits++;
		}
		if(son_jg_h[3]<0x20)
		{
			pJg=(CLDSLineAngle*)BelongModel()->AppendPart(CLS_LINEANGLE);
			pJg->group_father_jg_h=handle;
			pJg->pStart = pStart;
			pJg->pEnd = pEnd;
			pJg->SetStart(Start());
			pJg->SetEnd(End());
			//拷贝基本信息，否则可能因在Parts.InstantiateCursorLinePart()中调用可能因未初始化导致不能及时显示的错误 wjh-2019.10.15
			UpdateSonAngleBasicInfo(this,pJg);
			son_jg_h[3]=pJg->handle;
			nHits++;
		}
		/* 相关功能逻辑已移至UpdateSonAngleBasicInfo()中
		for(int i=2;i<4;i++)
		{
			pJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[i],CLS_LINEANGLE);
			pJg->m_bVirtualPart=false;
		}*/
	}
	else
	{
		for(int i=2;i<4;i++)
		{
			if(son_jg_h[i]>0x20)
			{
				pJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[i],CLS_LINEANGLE);
				if(pJg)
					pJg->m_bVirtualPart=true;
				pJg->group_father_jg_h=handle;
			}
		}
	}
	this->UpdateSonJgWingNorm();
	return nHits;
}

bool CLDSGroupLineAngle::ShiftMasterIndex(BYTE masterIndex/*=0*/)
{
	if(ciMasterIndex==masterIndex)
		return false;
	if(masterIndex==0)
	{	//将基准角钢由2号转换为1号,调整肢法线设计信息:X->Y,Y->-X
		DESIGN_JGWING_NORM des_wing_norm=des_norm_y;
		des_norm_y=des_norm_x;
		des_norm_x=des_wing_norm;
		des_norm_x.near_norm*=-1;
		des_norm_x.spec_norm.ReverseDirection();
		GEPOINT wing_norm=norm_y_wing;
		norm_y_wing=norm_x_wing;
		norm_x_wing=wing_norm;
		norm_x_wing*=-1;

		WING_OFFSET_STYLE start_wing_offset=desStartPos.wing_y_offset;
		desStartPos.wing_y_offset=desStartPos.wing_x_offset;
		desStartPos.wing_x_offset=start_wing_offset;
		if(desStartPos.wing_x_offset.operStyle==4)
			desStartPos.wing_x_offset.offsetDist*=-1;
		WING_OFFSET_STYLE end_wing_offset=desEndPos.wing_y_offset;
		desEndPos.wing_y_offset=desEndPos.wing_x_offset;
		desEndPos.wing_x_offset=end_wing_offset;
		if(desEndPos.wing_x_offset.operStyle==4)
			desEndPos.wing_x_offset.offsetDist*=-1;
	}
	else
	{
		//将基准角钢由1号转换为2号，调整肢法线设计信息:Y->X,X->-Y
		DESIGN_JGWING_NORM des_wing_norm=des_norm_x;
		des_norm_x=des_norm_y;
		des_norm_y=des_wing_norm;
		des_norm_y.near_norm*=-1;
		des_norm_y.spec_norm.ReverseDirection();
		GEPOINT wing_norm=norm_x_wing;
		norm_x_wing=norm_y_wing;
		norm_y_wing=wing_norm;
		norm_y_wing*=-1;

		WING_OFFSET_STYLE start_wing_offset=desStartPos.wing_x_offset;
		desStartPos.wing_x_offset=desStartPos.wing_y_offset;
		desStartPos.wing_y_offset=start_wing_offset;
		if(desStartPos.wing_y_offset.operStyle==4)
			desStartPos.wing_y_offset.offsetDist*=-1;
		WING_OFFSET_STYLE end_wing_offset=desEndPos.wing_x_offset;
		desEndPos.wing_x_offset=desEndPos.wing_y_offset;
		desEndPos.wing_y_offset=end_wing_offset;
		if(desEndPos.wing_y_offset.operStyle==4)
			desEndPos.wing_y_offset.offsetDist*=-1;
	}
	return true;
}
PROPLIST_ITEM* CLDSGroupLineAngle::UpdatePropItem(char* propKey,char* propName/*=NULL*/)		//得到属性字符串
{
	PROPLIST_ITEM *pItem=propHashtable.GetValue(propKey);
	if(pItem!=NULL&&propName!=NULL)
		pItem->name.Copy(propName);
	else
		pItem=CLDSLineAngle::UpdatePropItem(propKey,propName);
	return pItem;
}
int CLDSGroupLineAngle::GetPropStr(long id,char* propStr)
{
	char sText[100]="";
	if(GetPropID("group_style")==id)
		strcpy(sText,"group_style");
	else if(GetPropID("jg_space")==id)
		strcpy(sText,"jg_space");
	else if(GetPropID("m_bUpdateSonAngleOddment")==id)
		strcpy(sText,"m_bUpdateSonAngleOddment");
	else 
		return CLDSLineAngle::GetPropStr(id,propStr);
	if(propStr)
		strcpy(propStr,sText);
	return strlen(sText);
}

long CLDSGroupLineAngle::GetPropID(char* propStr)
{
	PROPLIST_ITEM* pItem=propHashtable.GetValue(propStr);
	if(pItem==NULL)
		pItem=CLDSLineAngle::propHashtable.GetValue(propStr);
	if(pItem)
		return pItem->id;
	else
		return 0;
}
BOOL CLDSGroupLineAngle::IsPropEqual(HASHOBJSET &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSGroupLineAngle *pGroupLineAngle=NULL, *pFirstGroupLineAngle = (CLDSGroupLineAngle*)selectObjs.GetFirst();
	pFirstGroupLineAngle->GetPropValueStr(idProp,valueStr);
	for(pGroupLineAngle=(CLDSGroupLineAngle*)selectObjs.GetNext();pGroupLineAngle;pGroupLineAngle=(CLDSGroupLineAngle*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pGroupLineAngle->GetPropValueStr(idProp,valueStr2);
		if(_stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}
int CLDSGroupLineAngle::GetPropStatus(long id)
{
	DWORD dwStatus;
	if(CLDSGroupLineAngle::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}
BOOL CLDSGroupLineAngle::SetPropStatus(long id,BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSGroupLineAngle::propStatusHashtable.SetValueAt(id,dwStatus);
}

BOOL CLDSGroupLineAngle::GetObjPropReadOnlyDefaultState(long id)
{
	return GetObjPropReadOnlyDefaultState(id,ShadowMode());
}

BOOL CLDSGroupLineAngle::GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode)
{
	if (shadow_mode==CLDSObject::SHADOWMODE_COMMON)
	{
		if(GetPropID("layer")==id)
			return FALSE;
		else if(GetPropID("cMaterial")==id)
			return FALSE;
		else if(GetPropID("m_cQualityLevel")==id)
			return FALSE;
		else if(GetPropID("cfgword")==id)
			return FALSE;
		else if(GetPropID("m_uStatMatCoef")==id)
			return FALSE;
		else if(GetPropID("m_hPartWeldParent")==id)
			return FALSE;
		else if(GetPropID("relativeObjs")==id)
			return FALSE;
		else if(GetPropID("sNotes")==id)
			return FALSE;
		else if(GetPropID("m_bEnableTeG")==id)
			return TRUE;
		else if(GetPropID("teGTable")==id)
			return TRUE;
		else if(GetPropID("m_hArcLift")==id)
			return TRUE;
		else
			return TRUE;
	}
	else if(shadow_mode==CLDSObject::SHADOWMODE_BLOCK)
	{
		if(GetPropID("layer")==id)
			return FALSE;
		//else if(GetPropID("m_hPartWeldParent")==id)
		//	return FALSE;
		else
			return TRUE;
	}
	else
	{
		if(GetPropID("handle")==id)
			return TRUE;
		else if(GetPropID("length")==id)
			return TRUE;
		else if(GetPropID("group_style")==id)
			return FALSE;
		else if(GetPropID("decWingAngle")==id)
			return TRUE;
		else if(GetPropID("m_bBendPart")==id)
			return TRUE;
		else
			return FALSE;
	}
}

int CLDSGroupLineAngle::GetPropValueStr(long id,char* valueStr)
{
	char sText[100]="";
	if(GetPropID("group_style")==id)
	{
		if(group_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Diagonal Group");
		else if(group_style==1)
			strcpy(sText,"1.T Group");
		else
			strcpy(sText,"2.Cross Combine");
#else
			strcpy(sText,"0.对角组合");
		else if(group_style==1)
			strcpy(sText,"1.T型组合");
		else
			strcpy(sText,"2.十字组合");
#endif
	}
	else if(GetPropID("jg_space")==id)
	{
		sprintf(sText,"%f",jg_space);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bUpdateSonAngleOddment")==id)
	{
		if(m_bUpdateSonAngleOddment)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("ciMasterIndex")==id)
	{
		if(this->wSectStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,ciMasterIndex==0?"Diagonal-I":"Diagonal-II");
		else if(this->wSectStyle==1)
			strcpy(sText,ciMasterIndex==0?"T-X":"T-Y");
#else
			strcpy(sText,ciMasterIndex==0?"对角I型":"对角II型");
		else if(this->wSectStyle==1)
			strcpy(sText,ciMasterIndex==0?"T-X型":"T-Y型");
#endif
	}
	else if(GetPropID("ciMasterDirectIndex")==id)
	{
		if(ciMasterDirectIndex==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Virtual angle");
		else if(ciMasterDirectIndex==1)
			strcpy(sText,"1# angle");
		else if(ciMasterDirectIndex==2)
			strcpy(sText,"2# angle");
#else
			strcpy(sText,"虚拟角钢");
		else if(ciMasterDirectIndex==1)
			strcpy(sText,"1号角钢");
		else if(ciMasterDirectIndex==2)
			strcpy(sText,"2号角钢");
#endif
	}
	else
		return CLDSLineAngle::GetPropValueStr(this,id,valueStr);
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}

#ifdef dklkasf
CLsRef* CLDSGroupLineAngle::AppendStartLsRef(CLsRef ls_ref,BOOL bNearLsCheck/*=TRUE*/)
{
	ls_ref.start1_end2_mid0=1;
	return AppendLsRef(ls_ref,bNearLsCheck);
}
CLsRef* CLDSGroupLineAngle::AppendMidLsRef(CLsRef ls_ref,BOOL bNearLsCheck/*=TRUE*/)
{
	ls_ref.start1_end2_mid0=0;
	return AppendLsRef(ls_ref,bNearLsCheck);
}
CLsRef* CLDSGroupLineAngle::AppendEndLsRef(CLsRef ls_ref,BOOL bNearLsCheck/*=TRUE*/)
{
	ls_ref.start1_end2_mid0=2;
	return AppendLsRef(ls_ref,bNearLsCheck);
}
CLsRef* CLDSGroupLineAngle::AppendLsRef(CLsRef ls_ref,BOOL bNearLsCheck/*=TRUE*/)
{
	f3dPoint norm,pos;
	//为组合角钢添加螺栓引用
	if(bNearLsCheck)	//检查附近螺栓
	{
		CLsRef *pLsRef=pLsRefList->FindLsByPos(ls_ref->ucs.origin,ls_ref->get_norm(),ls_ref->get_d());
		if(pLsRef&&(*pLsRef)->get_d()==ls_ref->get_d())
			return pLsRef;
	}
	pLsRefList->append(ls_ref);

	norm=ls_ref->get_norm();
	pos=ls_ref->ucs.origin;
	getUCS(ucs);
	coord_trans(pos,ucs,FALSE);
	CLDSLineAngle *pSonJg[4];
	int wing_x0_y1,quad;
	if(fabs(norm*get_norm_x_wing())>fabs(norm*get_norm_y_wing()))
		wing_x0_y1=0;
	else
		wing_x0_y1=1;
	if(pos.x>0&&pos.y>0)
		quad=0;
	else if(pos.x>0&&pos.y<0)
		quad=1;
	else if(pos.x<0&&pos.y<0)
		quad=2;
	else if(pos.x<0&&pos.y>0)
		quad=3;
	else				//位置不合适，无法插入螺栓
		return NULL;
	for(int i=0;i<4;i++)
		pSonJg[i]=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[i],CLS_LINEANGLE);
	switch(group_style)
	{
	case 0:		//对角型
		if(wing_x0_y1==0)	//X肢上的螺栓
		{
			if(pos.x>0)
			{
				if(ls_ref.start1_end2_mid0==0)
					return pSonJg[0]->AppendMidLsRef(ls_ref,bNearLsCheck);
				else if(ls_ref.start1_end2_mid0==1)
					return pSonJg[0]->AppendStartLsRef(ls_ref,bNearLsCheck);
				else //if(ls_ref.start1_end2_mid0==2)
					return pSonJg[0]->AppendEndLsRef(ls_ref,bNearLsCheck);
			}
			else
			{
				if(ls_ref.start1_end2_mid0==0)
					return pSonJg[1]->AppendMidLsRef(ls_ref,bNearLsCheck);
				else if(ls_ref.start1_end2_mid0==1)
					return pSonJg[1]->AppendStartLsRef(ls_ref,bNearLsCheck);
				else //if(ls_ref.start1_end2_mid0==2)
					return pSonJg[1]->AppendEndLsRef(ls_ref,bNearLsCheck);
			}
		}
		else				//Y肢上的螺栓
		{
			if(pos.y>0)
			{
				if(ls_ref.start1_end2_mid0==0)
					return pSonJg[0]->AppendMidLsRef(ls_ref,bNearLsCheck);
				else if(ls_ref.start1_end2_mid0==1)
					return pSonJg[0]->AppendStartLsRef(ls_ref,bNearLsCheck);
				else //if(ls_ref.start1_end2_mid0==2)
					return pSonJg[0]->AppendEndLsRef(ls_ref,bNearLsCheck);
			}
			else
			{
				if(ls_ref.start1_end2_mid0==0)
					return pSonJg[1]->AppendMidLsRef(ls_ref,bNearLsCheck);
				else if(ls_ref.start1_end2_mid0==1)
					return pSonJg[1]->AppendStartLsRef(ls_ref,bNearLsCheck);
				else //if(ls_ref.start1_end2_mid0==2)
					return pSonJg[1]->AppendEndLsRef(ls_ref,bNearLsCheck);
			}
		}
		break;
	case 1:		//T字型
		if(wing_x0_y1==0)	//X肢上的螺栓
		{
			if(ls_ref.start1_end2_mid0==0)
			{
				pSonJg[0]->AppendMidLsRef(ls_ref,bNearLsCheck);
				return pSonJg[1]->AppendMidLsRef(ls_ref,bNearLsCheck);
			}
			else if(ls_ref.start1_end2_mid0==1)
			{
				pSonJg[0]->AppendStartLsRef(ls_ref,bNearLsCheck);
				return pSonJg[1]->AppendStartLsRef(ls_ref,bNearLsCheck);
			}
			else //if(ls_ref.start1_end2_mid0==2)
			{
				pSonJg[0]->AppendEndLsRef(ls_ref,bNearLsCheck);
				return pSonJg[1]->AppendEndLsRef(ls_ref,bNearLsCheck);
			}
		}
		else				//Y肢上的螺栓
		{
			if(pos.y>0)
			{
				if(ls_ref.start1_end2_mid0==0)
					return pSonJg[0]->AppendMidLsRef(ls_ref,bNearLsCheck);
				else if(ls_ref.start1_end2_mid0==1)
					return pSonJg[0]->AppendStartLsRef(ls_ref,bNearLsCheck);
				else //if(ls_ref.start1_end2_mid0==2)
					return pSonJg[0]->AppendEndLsRef(ls_ref,bNearLsCheck);
			}
			else
			{
				if(ls_ref.start1_end2_mid0==0)
					return pSonJg[1]->AppendMidLsRef(ls_ref,bNearLsCheck);
				else if(ls_ref.start1_end2_mid0==1)
					return pSonJg[1]->AppendStartLsRef(ls_ref,bNearLsCheck);
				else //if(ls_ref.start1_end2_mid0==2)
					return pSonJg[1]->AppendEndLsRef(ls_ref,bNearLsCheck);
			}
		}
		break;
	case 2:		//十字型
		if(wing_x0_y1==0)	//X肢上的螺栓
		{
			if(pos.x>0)
			{
				if(ls_ref.start1_end2_mid0==0)
				{
					pSonJg[0]->AppendMidLsRef(ls_ref,bNearLsCheck);
					return pSonJg[1]->AppendMidLsRef(ls_ref,bNearLsCheck);
				}
				else if(ls_ref.start1_end2_mid0==1)
				{
					pSonJg[0]->AppendStartLsRef(ls_ref,bNearLsCheck);
					return pSonJg[1]->AppendStartLsRef(ls_ref,bNearLsCheck);
				}
				else //if(ls_ref.start1_end2_mid0==2)
				{
					pSonJg[0]->AppendEndLsRef(ls_ref,bNearLsCheck);
					return pSonJg[1]->AppendEndLsRef(ls_ref,bNearLsCheck);
				}
			}
			else
			{
				if(ls_ref.start1_end2_mid0==0)
				{
					pSonJg[2]->AppendMidLsRef(ls_ref,bNearLsCheck);
					return pSonJg[3]->AppendMidLsRef(ls_ref,bNearLsCheck);
				}
				else if(ls_ref.start1_end2_mid0==1)
				{
					pSonJg[2]->AppendStartLsRef(ls_ref,bNearLsCheck);
					return pSonJg[3]->AppendStartLsRef(ls_ref,bNearLsCheck);
				}
				else //if(ls_ref.start1_end2_mid0==2)
				{
					pSonJg[2]->AppendEndLsRef(ls_ref,bNearLsCheck);
					return pSonJg[3]->AppendEndLsRef(ls_ref,bNearLsCheck);
				}
			}
		}
		else				//Y肢上的螺栓
		{
			if(pos.y>0)
			{
				if(ls_ref.start1_end2_mid0==0)
				{
					pSonJg[0]->AppendMidLsRef(ls_ref,bNearLsCheck);
					return pSonJg[3]->AppendMidLsRef(ls_ref,bNearLsCheck);
				}
				else if(ls_ref.start1_end2_mid0==1)
				{
					pSonJg[0]->AppendStartLsRef(ls_ref,bNearLsCheck);
					return pSonJg[3]->AppendStartLsRef(ls_ref,bNearLsCheck);
				}
				else //if(ls_ref.start1_end2_mid0==2)
				{
					pSonJg[0]->AppendEndLsRef(ls_ref,bNearLsCheck);
					return pSonJg[3]->AppendEndLsRef(ls_ref,bNearLsCheck);
				}
			}
			else
			{
				if(ls_ref.start1_end2_mid0==0)
				{
					pSonJg[1]->AppendMidLsRef(ls_ref,bNearLsCheck);
					return pSonJg[2]->AppendMidLsRef(ls_ref,bNearLsCheck);
				}
				else if(ls_ref.start1_end2_mid0==1)
				{
					pSonJg[1]->AppendStartLsRef(ls_ref,bNearLsCheck);
					return pSonJg[2]->AppendStartLsRef(ls_ref,bNearLsCheck);
				}
				else //if(ls_ref.start1_end2_mid0==2)
				{
					pSonJg[1]->AppendEndLsRef(ls_ref,bNearLsCheck);
					return pSonJg[2]->AppendEndLsRef(ls_ref,bNearLsCheck);
				}
			}
		}
		break;
	default:
		break;
	}
	return NULL;
}
BOOL CLDSGroupLineAngle::DeleteLsRef(THANDLE ls_handle)
{
	CLDSLineAngle *pSonJg[4];
	int nHits=0;
	for(int i=0;i<4;i++)
	{
		pSonJg[i]=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[i],CLS_LINEANGLE);
		if(pSonJg[i])
		{
			if(pSonJg[i]->DeleteLsRef(ls_handle))
				nHits++;
		}
	}
	return nHits;
}
int CLDSGroupLineAngle::GetLsCount(short d/*=0*/)
{
	CLDSLineAngle *pSonJg[4];
	int nHits=0;
	for(int i=0;i<4;i++)
	{
		pSonJg[i]=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[i],CLS_LINEANGLE);
		if(pSonJg[i])
			nHits+=pSonJg[i]->GetLsCount(d);
	}
	return nHits;
}
void CLDSGroupLineAngle::EmptyLsRef()
{
	CLDSLineAngle *pSonJg[4];
	int nHits=0;
	for(int i=0;i<4;i++)
	{
		pSonJg[i]=(CLDSLineAngle*)BelongModel()->FromPartHandle(son_jg_h[i],CLS_LINEANGLE);
		if(pSonJg[i])
			pSonJg[i]->EmptyLsRef();
	}
}
#endif
//组合接头
CJoint::CJoint()
{
	m_nClassTypeId = CLS_JOINT;
	//strcpy(m_sClassName,"CJoint");
	part_list	= new ATOM_LIST<PART_ITEM>;
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
	blIntelliCalOddment = true;
	blInheritExistBoltsInMainAngle = blInheritExistBoltsInCoupleAngle = false;
}
CJoint::~CJoint()
{
	if(part_list)
		delete part_list;
}
void CJoint::EmptyAllSubPart()
{
	PART_ITEM *pI;
	for(pI=part_list->GetFirst();pI;pI=part_list->GetNext())
	{
		BelongModel()->DeletePart(pI->h);
		/* 以下代码好像有点烦　wjh-2012.8.11
		if(SuperiorTower()->Parts.DeleteNode(pI->h))
			continue;
		else
			SuperiorTower()->Parts.DeleteNode(pI->h);
		*/
	}
	part_list->Empty();
}
/*CLDSPolyAngle* CJoint::AppendHuoQuWrapJg(CLDSPolyAngle *pHuoQuJg,char cId)
{
	if(pHuoQuJg)
	{
		PART_ITEM item;
		item.h=pHuoQuJg->handle;
		item.cID=cId;
		//if(pHuoQuJg->GetClassTypeId()==CLS_POLYANGLE)
			part_list->append(item);
		else
			return NULL;
	}
	return NULL;
}
CLDSPolyAngle* CJoint::AppendHuoQuWrapJg(char cId)
{
	CLDSPolyAngle *pHuoQuJg=(CLDSPolyAngle*)SuperiorTower()->Parts.append(CLS_POLYANGLE);
	PART_ITEM item;
	item.h=pHuoQuJg->handle;
	item.cID=cId;
	part_list->append(item);
	return pHuoQuJg;
}*/
CLDSLineAngle* CJoint::AppendWrapJg(CLDSLineAngle *pJg,char cId)
{
	if(pJg)
	{
		PART_ITEM item;
		item.h=pJg->handle;
		item.cID=cId;
		part_list->append(item);
	}
	else
		return NULL;
	return pJg;
}
CLDSLineAngle* CJoint::AppendWrapJg(char cId)
{
	CLDSLineAngle *pJg=(CLDSLineAngle*)BelongModel()->AppendPart(CLS_LINEANGLE);
	PART_ITEM item;
	item.h=pJg->handle;
	item.cID=cId;
	part_list->append(item);
	pJg->iSeg=iSeg;
	strcpy(pJg->layer(),layer());
	return pJg;
}
BOOL CJoint::AppendWrapJg(long h,char cId)
{
	if(h>0x20)
	{
		PART_ITEM item;
		item.h=h;
		item.cID=cId;
		part_list->append(item);
		return TRUE;
	}
	else
		return FALSE;
}
CLDSPlate* CJoint::AppendWrapPlank(char cId)
{
	CLDSPlate *pPlank=(CLDSPlate*)BelongModel()->AppendPart(CLS_PLATE);
	PART_ITEM item;
	item.h=pPlank->handle;
	item.cID=cId;
	part_list->append(item);
	pPlank->iSeg=iSeg;
	strcpy(pPlank->layer(),layer());
	return pPlank;
}
BOOL CJoint::AppendWrapPlank(long h,char cId)
{
	if(h>0x20)
	{
		PART_ITEM item;
		item.h=h;
		item.cID=cId;
		part_list->append(item);
		return TRUE;
	}
	else
		return FALSE;
}
CLDSPlate* CJoint::AppendWrapPlank(CLDSPlate *pPlank,char cId)
{
	if(pPlank)
	{
		PART_ITEM item;
		item.h=pPlank->handle;
		item.cID=cId;
		part_list->append(item);
		return pPlank;
	}
	else
		return NULL;
}
CLDSPart *CJoint::GetFirstPart()
{
	PART_ITEM *pI=part_list->GetFirst();
	if(pI)
	{
		CLDSPart *pPart=BelongModel()->FromPartHandle(pI->h);
		if(pPart)
			return pPart;
		else
		{
			part_list->DeleteCursor();
			return GetFirstPart();
		}
	}
	else
		return NULL;
}
CLDSPart *CJoint::GetNextPart()
{
	PART_ITEM *pI=part_list->GetNext();
	if(pI)
	{
		CLDSPart *pPart=BelongModel()->FromPartHandle(pI->h);
		if(pPart)
			return pPart;
		else
		{
			part_list->DeleteCursor();
			return GetNextPart();
		}
	}
	else
		return NULL;
}
PART_ITEM *CJoint::FromPartId(char cId)
{
	for(PART_ITEM *pI=part_list->GetFirst();pI;pI=part_list->GetNext())
		if(toupper(pI->cID)==toupper(cId))
			return pI;
	return NULL;
}

void CJoint::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
		return;
	buffer.ReadDword(&dwPermission);
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	BelongModel()->EvolveCfgword(cfgword,version,doc_type);
	buffer.ReadInteger(&base_jg_handle_arr[0]);
	buffer.ReadInteger(&base_jg_handle_arr[1]);
	buffer.ReadInteger(&base_node_handle);
	buffer.ReadInteger(&joint_style);
	buffer.ReadDouble(&length);
	buffer.ReadPoint(ucs.axis_x);
	buffer.ReadPoint(ucs.axis_y);
	buffer.ReadPoint(ucs.axis_z);
	buffer.ReadPoint(ucs.origin);
	long i,n;
	EmptyAllSubPartH();
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		PART_ITEM *pI=part_list->append();
		buffer.ReadInteger(&pI->h);
		buffer.ReadByte(&pI->cID);
	}
}
void CJoint::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdToBuffer(buffer))
		return;
	buffer.WriteDword(dwPermission);
	CfgwordToBuffer(cfgword,buffer);
	buffer.WriteInteger(base_jg_handle_arr[0]);
	buffer.WriteInteger(base_jg_handle_arr[1]);
	buffer.WriteInteger(base_node_handle);
	buffer.WriteInteger(joint_style);
	buffer.WriteDouble(length);
	buffer.WritePoint(ucs.axis_x);
	buffer.WritePoint(ucs.axis_y);
	buffer.WritePoint(ucs.axis_z);
	buffer.WritePoint(ucs.origin);
	buffer.WriteInteger(part_list->GetNodeNum());
	PART_ITEM *pI;
	for(pI=part_list->GetFirst();pI;pI=part_list->GetNext())
	{
		buffer.WriteInteger(pI->h);
		buffer.WriteByte(pI->cID);
	}
}
void CJoint::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<接头 handle=\"0X%X\">\n",handle);
	fprintf(fp,"<权限字 dwPermission=\"%d\"/>\n",dwPermission);
	fprintf(fp,"<配材号 cfgword.word=\"%d,%d,%d,%d,%d\"/>\n",cfgword.flag.word[0],cfgword.flag.word[1],cfgword.flag.word[2],cfgword.flag.word[3],cfgword.flag.word[4]);
	fprintf(fp,"<基础角钢的句柄数组 base_jg_handle_arr=\"(0X%X,0X%X)\"/>\n",base_jg_handle_arr[0],base_jg_handle_arr[1]);
	fprintf(fp,"<附属节点句柄 base_node_handle=\"0X%X\"/>\n",base_node_handle);
	if (joint_style==0)
		fprintf(fp,"<接头方式 joint_style=\"0.外包铁\"/>\n");
	else if (joint_style==1)
		fprintf(fp,"<接头方式 joint_style=\"1.内外包铁\"/>\n");
	else if (joint_style==2)
		fprintf(fp,"<接头方式 joint_style=\"2.内包铁外包钢板\"/>\n");
	else if (joint_style==3)
		fprintf(fp,"<接头方式 joint_style=\"3.外包铁内包钢板\"/>\n");
	else if (joint_style==4)
		fprintf(fp,"<接头方式 joint_style=\"4.内包铁X肢钢板截断\"/>\n");
	else if (joint_style==5)
		fprintf(fp,"<接头方式 joint_style=\"5.内包铁Y肢钢板截断\"/>\n");
	else if (joint_style==6)
		fprintf(fp,"<接头方式 joint_style=\"6.四包铁X肢钢板截断\"/>\n");
	else if (joint_style==7)
		fprintf(fp,"<接头方式 joint_style=\"7.四包铁Y肢钢板截断\"/>\n");
	else if (joint_style==8)
		fprintf(fp,"<接头方式 joint_style=\"8.十字接头X肢钢板截断\"/>\n");
	else if (joint_style==9)
		fprintf(fp,"<接头方式 joint_style=\"9.十字接头Y肢钢板截断\"/>\n");
	sprintf(sText,"%f",length);
	SimplifiedNumString(sText);
	fprintf(fp,"<接头长度 length=\"%s\"/>\n",sText);
	fprintf(fp,"<相对坐标系>\n");
	sprintf(sText,"%f",ucs.axis_x.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.axis_x.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.axis_x.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<X轴 ucs.axis_x=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",ucs.axis_y.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.axis_y.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.axis_y.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<Y轴 ucs.axis_y=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",ucs.axis_z.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.axis_z.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.axis_z.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<Z轴 ucs.axis_z=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",ucs.origin.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.origin.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.origin.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<原点 ucs.origin=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	fprintf(fp,"</相对坐标系>\n");
	fprintf(fp,"<子构件列表>\n");
	fprintf(fp,"<子构件数 part_list_Num=\"%d\"/>\n",part_list->GetNodeNum());
	PART_ITEM *pI;
	for(pI=part_list->GetFirst();pI;pI=part_list->GetNext())
	{
		fprintf(fp,"<子构件");
		fprintf(fp," 句柄handle=\"0X%X\"",pI->h);
		fprintf(fp," 编号cID=\"%c\"",pI->cID);
		fprintf(fp,"/>\n");
	}
	fprintf(fp,"</子构件列表>\n");
	fprintf(fp,"</接头>\n");
}
void CWeldPart::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_tma1_lma2_tsa3_lds4_default0/*=0*/)
{
	buffer.ReadInteger(&handle);
	buffer.ReadInteger(&iSeg.iSeg);
	buffer.ReadString(sPartNo);
	buffer.ReadString(sName);
	int i,n;
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		PART_ITEM *pItem=SonPart.append();
		buffer.ReadInteger(&pItem->hSonPart);
		buffer.ReadInteger(&pItem->num);
	}
}
void CWeldPart::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(handle);
	buffer.WriteInteger(iSeg);
	buffer.WriteString(sPartNo);
	buffer.WriteString(sName);
	buffer.WriteInteger(SonPart.GetNodeNum());
	for(PART_ITEM *pItem=SonPart.GetFirst();pItem;pItem=SonPart.GetNext())
	{
		buffer.WriteInteger(pItem->hSonPart);
		buffer.WriteInteger(pItem->num);
	}
}

void CWeldPart::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="";
	fprintf(fp,"<焊接组合件 handle=\"0X%X\">\n",handle);
	fprintf(fp,"<段号 iSeg=\"%d\"/>\n",iSeg);
	fprintf(fp,"<构件编号 sPartNo=\"%s\"/>\n",sPartNo);
	fprintf(fp,"<名字 sName=\"%s\"/>\n",sName);
	fprintf(fp,"<子构件列表>\n");
	fprintf(fp,"<子构件数 SonPart_Num=\"%d\"/>\n",SonPart.GetNodeNum());
	if (SonPart.GetNodeNum()>0)
	{
		for(PART_ITEM *pItem=SonPart.GetFirst();pItem;pItem=SonPart.GetNext())
		{
			fprintf(fp,"<子构件");
			fprintf(fp," 句柄hSonPart=\"0X%X\"",pItem->hSonPart);
			fprintf(fp," 编号num=\"%d\"",pItem->num);
			fprintf(fp,"/>\n");
		}
	}
	fprintf(fp,"</子构件列表>\n");
	fprintf(fp,"</焊接组合件>\n");
}

char* CLDSGroupLineAngle::GetBriefPartTypeMark()
{
	if(size_idClassType==CLS_GROUPLINEANGLE)
	{
		if(group_style==0)
			return "DL";
		else if(group_style==1)
			return "TL";
		else if(group_style==2)
			return "XL";
		else
			return "?L";
	}
	else
		return "L";
}
BOOL CLDSGroupLineAngle::Create3dSolidModel(BOOL bDisplayHole,double scale_of_user2scr,double sample_len,int smoothness/*=36*/)
{
	if(pSolidBody==NULL)
		pSolidBody = new CSolidBody;
	else
	{
		pSolidBody->Empty();
		SetModified(FALSE);
		pSolidBody->m_bModified=FALSE;
	}
	return TRUE;
}
int GetProcessAngleExchangeBuffer(CLDSPart *part,CBuffer &buffer,bool bSaveSubSerial,long version);
int CLDSPolyPart::WriteProcessBuffer(CBuffer &buffer,bool bSaveSubSerial/*=false*/,long version/*=0*/)
{	
	return GetProcessAngleExchangeBuffer(this,buffer,bSaveSubSerial,version);
}
#ifdef __LDS_CONTEXT_
bool ProcessPolyPartKaiHeJiao(CLDSPolyPart *pPolyPart,ATOM_LIST<KAI_HE_JIAO_TEMP> &kaiHeJiaoList,double* fWeldLineLen/*=NULL*/);
bool CLDSPolyPart::CloneToBomPart(BOMPART* pBomPart)
{
	CLDSLinePart *pFirstRod=segset.GetFirst();
	if(pFirstRod==NULL||!pFirstRod->IsAngle()||pBomPart->cPartType!=BOMPART::ANGLE)
		return false;
	CLDSLineAngle *pFirstAngle=(CLDSLineAngle*)segset.GetFirst(),*pTailAngle=(CLDSLineAngle*)segset.GetTail();
	CLDSPart::CloneToBomPart(pBomPart);	//克隆基本属性
	//克隆角钢特有属性
	PART_ANGLE* pBomAngle=(PART_ANGLE*)pBomPart;
	pBomAngle->wingWideY=pFirstAngle->GetWidthWingY();
	pBomAngle->wing_angle=(float)pFirstAngle->GetDecWingAngle();
	pBomAngle->bCutRoot=pFirstAngle->m_bCutRoot;
	pBomAngle->bCutBer=pFirstAngle->m_bCutBer;
	pBomAngle->nPushFlat=0;	//制弯件不需要压扁
	if(strlen(sNotes)>0)
		strcpy(pBomAngle->sNotes,pFirstAngle->sNotes);
	//始端切角切肢信息
	if(pFirstAngle->bHuoQuEnd)
	{	//首角钢始端为火曲角钢始端
		pBomAngle->cut_wing[0] = pFirstAngle->cut_wing[0];
		pBomAngle->cut_wing_para[0][0] = ftoi(pFirstAngle->cut_wing_para[0][0]);
		pBomAngle->cut_wing_para[0][1] = ftoi(pFirstAngle->cut_wing_para[0][1]);
		pBomAngle->cut_wing_para[0][2] = ftoi(pFirstAngle->cut_wing_para[0][2]);
		pBomAngle->cut_angle[0][0] = ftoi(pFirstAngle->cut_angle[0][0]);
		pBomAngle->cut_angle[1][0] = ftoi(pFirstAngle->cut_angle[1][0]);
		pBomAngle->cut_angle[0][1] = ftoi(pFirstAngle->cut_angle[0][1]);
		pBomAngle->cut_angle[1][1] = ftoi(pFirstAngle->cut_angle[1][1]);
	}
	else if(pFirstAngle->bHuoQuStart)
	{	//首角钢终端为火曲角钢始端，取终端切角参数，XY肢对调
		if(pFirstAngle->cut_wing[1]==1)
			pBomAngle->cut_wing[0] = 2;
		else if(pFirstAngle->cut_wing[1]==2)
			pBomAngle->cut_wing[0] = 1;
		else 
			pBomAngle->cut_wing[0] = 0;
		pBomAngle->cut_wing_para[0][0] = ftoi(pFirstAngle->cut_wing_para[1][0]);
		pBomAngle->cut_wing_para[0][1] = ftoi(pFirstAngle->cut_wing_para[1][1]);
		pBomAngle->cut_wing_para[0][2] = ftoi(pFirstAngle->cut_wing_para[1][2]);
		pBomAngle->cut_angle[0][0] = ftoi(pFirstAngle->cut_angle[3][0]);
		pBomAngle->cut_angle[1][0] = ftoi(pFirstAngle->cut_angle[2][0]);
		pBomAngle->cut_angle[0][1] = ftoi(pFirstAngle->cut_angle[3][1]);
		pBomAngle->cut_angle[1][1] = ftoi(pFirstAngle->cut_angle[2][1]);
	}
	//终端切角切肢信息
	if(pTailAngle->bHuoQuStart)
	{	//末角钢终端为火曲角钢终端
		pBomAngle->cut_wing[1] = pFirstAngle->cut_wing[1];
		pBomAngle->cut_wing_para[1][0] = ftoi(pFirstAngle->cut_wing_para[1][0]);
		pBomAngle->cut_wing_para[1][1] = ftoi(pFirstAngle->cut_wing_para[1][1]);
		pBomAngle->cut_wing_para[1][2] = ftoi(pFirstAngle->cut_wing_para[1][2]);
		pBomAngle->cut_angle[2][0] = ftoi(pFirstAngle->cut_angle[2][0]);
		pBomAngle->cut_angle[3][0] = ftoi(pFirstAngle->cut_angle[3][0]);
		pBomAngle->cut_angle[2][1] = ftoi(pFirstAngle->cut_angle[2][1]);
		pBomAngle->cut_angle[3][1] = ftoi(pFirstAngle->cut_angle[3][1]);
	}
	else //if(pTailAngle->bHuoQuEnd)
	{	//末角钢始端为火曲角钢终端，取始端切角参数，XY肢对调
		if(pFirstAngle->cut_wing[0]==1)
			pBomAngle->cut_wing[1] = 2;
		else if(pFirstAngle->cut_wing[0]==2)
			pBomAngle->cut_wing[1] = 1;
		else 
			pBomAngle->cut_wing[1] = 0;
		pBomAngle->cut_wing_para[1][0] = ftoi(pFirstAngle->cut_wing_para[0][0]);
		pBomAngle->cut_wing_para[1][1] = ftoi(pFirstAngle->cut_wing_para[0][1]);
		pBomAngle->cut_wing_para[1][2] = ftoi(pFirstAngle->cut_wing_para[0][2]);
		pBomAngle->cut_angle[2][0] = ftoi(pFirstAngle->cut_angle[1][0]);
		pBomAngle->cut_angle[3][0] = ftoi(pFirstAngle->cut_angle[0][0]);
		pBomAngle->cut_angle[2][1] = ftoi(pFirstAngle->cut_angle[1][1]);
		pBomAngle->cut_angle[3][1] = ftoi(pFirstAngle->cut_angle[0][1]);
	}
	//螺栓
	CLDSPolyPart::AnaylyzeAngleBolts(this);
	pBomAngle->nM16Ls=pBomAngle->nM20Ls=pBomAngle->nM24Ls=pBomAngle->nMXLs=0;
	pBomAngle->nMSumLs = (short)(boltListX.GetNodeNum()+boltListY.GetNodeNum());
	pBomAngle->m_arrBoltRecs.SetSize(pBomAngle->nMSumLs);
	int i=0;
	for(BOLTHOLE_INFO *pBoltHole=boltListX.GetFirst();pBoltHole;pBoltHole=boltListX.GetNext(),i++)
	{
		pBomAngle->m_arrBoltRecs[i].d = (short)(pBoltHole->bolt_d);
		pBomAngle->m_arrBoltRecs[i].hole_d_increment = (float)(pBoltHole->hole_d-pBoltHole->bolt_d);
		pBomAngle->m_arrBoltRecs[i].x = (float)(pBoltHole->centre.x);
		pBomAngle->m_arrBoltRecs[i].y = -(float)(fabs(pBoltHole->centre.y));	//X肢
		if(16==pBomAngle->m_arrBoltRecs[i].d)
			pBomAngle->nM16Ls++;
		else if(20==pBomAngle->m_arrBoltRecs[i].d)
			pBomAngle->nM20Ls++;
		else if(24==pBomAngle->m_arrBoltRecs[i].d)
			pBomAngle->nM24Ls++;
		else
			pBomAngle->nMXLs++;
	}
	for(BOLTHOLE_INFO *pBoltHole=boltListY.GetFirst();pBoltHole;pBoltHole=boltListY.GetNext(),i++)
	{
		pBomAngle->m_arrBoltRecs[i].d = (short)(pBoltHole->bolt_d);
		pBomAngle->m_arrBoltRecs[i].hole_d_increment = (float)(pBoltHole->hole_d-pBoltHole->bolt_d);
		pBomAngle->m_arrBoltRecs[i].x = (float)(pBoltHole->centre.x);
		pBomAngle->m_arrBoltRecs[i].y = (float)(fabs(pBoltHole->centre.y));	//Y肢
		if(16==pBomAngle->m_arrBoltRecs[i].d)
			pBomAngle->nM16Ls++;
		else if(20==pBomAngle->m_arrBoltRecs[i].d)
			pBomAngle->nM20Ls++;
		else if(24==pBomAngle->m_arrBoltRecs[i].d)
			pBomAngle->nM24Ls++;
		else
			pBomAngle->nMXLs++;
	}
	//开合角
	ATOM_LIST<KAI_HE_JIAO_TEMP> listKaiHeJiao;
	ProcessPolyPartKaiHeJiao(this,listKaiHeJiao,&pBomAngle->fWeldLineLen);
	pBomAngle->bWeldPart=(pBomAngle->fWeldLineLen>0);
	pBomAngle->listKaihe.DeleteList();
	for(KAI_HE_JIAO_TEMP* pKaiHe=listKaiHeJiao.GetFirst();pKaiHe;pKaiHe=listKaiHeJiao.GetNext())
	{
		KAI_HE_JIAO kaihe;
		kaihe.decWingAngle=pKaiHe->decWingAngle;
		kaihe.position=pKaiHe->Position();
		kaihe.startLength=pKaiHe->StartLength();
		kaihe.endLength=pKaiHe->EndLength();
		pBomAngle->listKaihe.AttachNode(kaihe);
	}
	//制弯角钢子角钢
	int sign=0;
	f3dPoint datum_vec;
	UCS_STRU ucs=GetUCS(true);
	pBomAngle->listPolyAngle.DeleteList();
	for(CLDSLineAngle *pJg=(CLDSLineAngle*)segset.GetFirst();pJg;pJg=(CLDSLineAngle*)segset.GetNext())
	{
		f3dPoint norm_x=pJg->get_norm_x_wing();
		f3dPoint norm_y=pJg->get_norm_y_wing();
		f3dPoint len_vec ;
		Sub_Pnt(len_vec,pJg->End(),pJg->Start());
		if(datum_vec.IsZero())
		{
			if(pJg->bHuoQuStart)
				len_vec*=-1;
			datum_vec=len_vec;
		}
		else if(len_vec*datum_vec<0)
			len_vec*=-1;
		normalize(len_vec);
		len_vec*=pJg->GetLength();			//单位化长度方向后乘以杆件长度(考虑正负头) wht 15-09-24
		vector_trans(norm_x,ucs,FALSE);
		vector_trans(norm_y,ucs,FALSE);
		vector_trans(len_vec,ucs,FALSE);

		LIST_NODE<PART_ANGLE::POLY_ANGLE_INFO> *pNode=pBomAngle->listPolyAngle.AttachNode();
		pNode->data.nIndex=sign++;
		pNode->data.len_vec.Set(len_vec.x,len_vec.y,len_vec.z);	//延伸方向,包含长度信息 wht 15-05-05
		pNode->data.norm_x_wing.Set(norm_x.x,norm_x.y,norm_x.z);//X肢展开方向
		pNode->data.norm_y_wing.Set(norm_y.x,norm_y.y,norm_y.z);//Y肢展开方向
	}
	return true;
}
#endif
//////////////////////////////////////////////////////////////////////////
//CDatumSonAngleLifeObj
CGroupAngleLifeObj::CGroupAngleLifeObj()
{
	m_pSrcAngle=0;
	m_bRestore=false;
	m_iVersion=0;
	m_iDocType=0;
	memset(_pPresetMemPool, 0, 360);
	m_xBuffer.AttachMemory(_pPresetMemPool, 360);
	m_xBuffer.ClearContents();
}
CGroupAngleLifeObj::CGroupAngleLifeObj(CLDSGroupLineAngle* pAngle,long version/*=0*/,long doc_type/*=0*/)
{
	memset(_pPresetMemPool, 0, 360);
	m_xBuffer.AttachMemory(_pPresetMemPool, 360);
	m_xBuffer.SetBlockSize(10000);
	m_xBuffer.ClearContents();
	m_bRestore=false;
	m_pSrcAngle = pAngle;
	m_iVersion=version;
	m_iDocType=doc_type;
	//进行备份
	pAngle->ToNormInfoBuffer(m_xBuffer);
	if( (doc_type==1&&version>0&&version<5010301)||	//TMA V5.1.3.1
		(doc_type==2&&version>0&&version<2000601)||	//LMA V2.0.6.1
		(doc_type==4&&version>0&&version<1030601)||	//LDS V1.3.6.1
		(doc_type==5&&version>0&&version<1030601))	//TDA V1.3.6.1
	{	//旧版本中T型组合角钢默认的基准子角钢为son_jg_h[0],新版本中T型组合角钢基准子角钢发生变化时，保存低版本时需调整肢法线设计信息
		if(pAngle->group_style==1&&pAngle->m_hDatumSonAngle!=pAngle->son_jg_h[0])
			m_bRestore=pAngle->ShiftMasterIndex(0);
	}
}
CGroupAngleLifeObj::~CGroupAngleLifeObj()
{
	if(m_bRestore&&m_pSrcAngle)
		m_pSrcAngle->FromNormInfoBuffer(m_xBuffer,m_iVersion,m_iDocType);	
}
#endif