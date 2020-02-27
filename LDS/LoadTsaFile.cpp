#include "stdafx.h"
//#include "LDS.h"
#include "Buffer.h"
#include "env_def.h"
#include "WindLoad.h"
#include "HashTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TRUSS_POLE_USERDEF	0x00000000// 0.自定义杆件
#define TRUSS_POLE_JG		0x00000001// 1.等肢角钢
#define TRUSS_POLE_JG_T_COM	0x00000011// 3.等肢角钢T型组合
#define TRUSS_POLE_JG_D_COM	0x00000101// 5.等肢角钢对角组合
#define TRUSS_POLE_JG_X_COM	0x00001001// 9.等肢角钢十字组合
#define TRUSS_POLE_TUBE		0x00010000// 16.钢管
#define TRUSS_POLE_YUANGANG	0x00010010// 18.圆钢
#define TRUSS_POLE_SLOT		0x00100000// 32.槽钢
#define TRUSS_POLE_FLAT		0x01000000// 64.扁钢
#define TRUSS_POLE_BAR		0x01100000// 96.钢筋
#define TRUSS_POLE_GUYS		0x10000000// 128.柔索

void PointRW(CArchive &ar, f3dPoint &pnt)
{
	if(ar.IsStoring())
	{
		ar<<pnt.x;
		ar<<pnt.y;
		ar<<pnt.z;
	}
	else
	{
		ar>>pnt.x;
		ar>>pnt.y;
		ar>>pnt.z;
	}
}
void PointRW(CArchive &ar, double* pnt)
{
	if(ar.IsStoring())
	{
		ar<<pnt[0];
		ar<<pnt[1];
		ar<<pnt[2];
	}
	else
	{
		ar>>pnt[0];
		ar>>pnt[1];
		ar>>pnt[2];
	}
}

typedef ATOM_LIST<CNodeLoad> CNodeLoadList;
BOOL LoadTsaFile(CArchive &ar)
{
	BYTE byte=0;
	WORD w=0;
	DWORD dw=0;
    long i,n,h,lh;
	double ff=0.0;
	f3dPoint point;
	//生成默认视图
	CDisplayView *pDispView=console.AppendView();
	strcpy(pDispView->name,"透视图");
	pDispView->m_iViewType=0;
	strcpy(pDispView->defaultLayer,"SPQ");
	strcpy(pDispView->filter.FirstCharSet,"?");
	strcpy(pDispView->filter.SecondCharSet,"?");
	strcpy(pDispView->filter.ThirdCharSet,"?");
	pDispView->ucs.axis_x.Set(1,0,0);
	pDispView->ucs.axis_y.Set(0,0,-1);
	pDispView->ucs.axis_z.Set(0,1,0);

	pDispView=console.AppendView();
	console.SetActiveView(pDispView->handle);
	strcpy(pDispView->name,"前面图");
	pDispView->m_iViewType=0;
	strcpy(pDispView->defaultLayer,"SPQ");
	strcpy(pDispView->filter.FirstCharSet,"TSL");
	strcpy(pDispView->filter.SecondCharSet,"ZXHFP");
	strcpy(pDispView->filter.ThirdCharSet,"Q12");
	pDispView->ucs.axis_x.Set(1,0,0);
	pDispView->ucs.axis_y.Set(0,0,-1);
	pDispView->ucs.axis_z.Set(0,1,0);
	
	pDispView=console.AppendView();
	strcpy(pDispView->name,"右面图");
	pDispView->m_iViewType=0;
	strcpy(pDispView->defaultLayer,"SPY");
	strcpy(pDispView->filter.FirstCharSet,"TSL");
	strcpy(pDispView->filter.SecondCharSet,"ZXHFP");
	strcpy(pDispView->filter.ThirdCharSet,"Y13");
	pDispView->ucs.axis_x.Set(0,-1,0);
	pDispView->ucs.axis_y.Set(0,0,-1);
	pDispView->ucs.axis_z.Set(1,0,0);

	pDispView=console.AppendView();
	strcpy(pDispView->name,"后面图");
	pDispView->m_iViewType=0;
	strcpy(pDispView->defaultLayer,"SPH");
	strcpy(pDispView->filter.FirstCharSet,"TSL");
	strcpy(pDispView->filter.SecondCharSet,"ZXHFP");
	strcpy(pDispView->filter.ThirdCharSet,"H34");
	pDispView->ucs.axis_x.Set(-1,0,0);
	pDispView->ucs.axis_y.Set(0,0,-1);
	pDispView->ucs.axis_z.Set(0,-1,0);

	pDispView=console.AppendView();
	strcpy(pDispView->name,"左面图");
	pDispView->m_iViewType=0;
	strcpy(pDispView->defaultLayer,"SPZ");
	strcpy(pDispView->filter.FirstCharSet,"TSL");
	strcpy(pDispView->filter.SecondCharSet,"ZXHFP");
	strcpy(pDispView->filter.ThirdCharSet,"Z24");
	pDispView->ucs.axis_x.Set(0,1,0);
	pDispView->ucs.axis_y.Set(0,0,-1);
	pDispView->ucs.axis_z.Set(-1,0,0);
	
	ar >>Ta.m_sPrjCode;
	CString prjName,sDesignPerson;
	ar >>prjName;
	_snprintf(Ta.m_sPrjName,50,"%s",prjName);
	ar >>sDesignPerson;
	_snprintf(Ta.m_sOperator,MAX_TEMP_CHAR_50,"%s",sDesignPerson);
	ar >>Ta.user_pipeline_no;
	ar >>Ta.m_hActiveModule;//当前活动塔模型
	ar >>Ta.m_hActiveStatus;//当前活动工况
	if(compareVersion(Ta.version,"1.00.00.43")>=0)
	{
		for(i=0;i<4;i++)
			ar >>Ta.m_arrActiveQuadLegNo[i];//第一~四象限激活接腿
	}
	if(compareVersion(Ta.version,"1.2.0.0")>=0)
		ar.Read(Ta.cfgword.flag.word,20);
	else
		ar>>Ta.cfgword.flag.word[0];		//所有构件的总配材号
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	if(compareVersion(Ta.version,"1.00.00.40")>=0)
	{
		ar >>Ta.m_cLandscape;		//塔位地形
		ar >>Ta.volt_grade;		//电压等级
	}
	if(compareVersion(Ta.version,"1.00.00.41")>=0)
		ar>>Ta.m_siSafetyGrade;
	if(compareVersion(Ta.version,"1.00.2.4")>=0)
		ar>>Ta.m_fIceDensity;	//杆塔裹冰重度kg/m3
#else
	if(compareVersion(Ta.version,"1.00.00.40")>=0)
	{
		ar >>byte;//Ta.m_cLandscape;		//塔位地形
		ar >>dw;//Ta.volt_grade;		//电压等级
	}
	if(compareVersion(Ta.version,"1.00.00.41")>=0)
		ar>>w;//Ta.m_siSafetyGrade;
	if(compareVersion(Ta.version,"1.00.2.4")>=0)
		ar>>ff;//Ta.m_fIceDensity;	//杆塔裹冰重度kg/m3
#endif
	short maxCode;	//原最大编号
	ar>>maxCode;//Ta.m_nMaxCode;
	if(compareVersion(Ta.version,"1.00.00.51")>=0)
	{	//角钢准距
		ar>>w;	zhunju_N=w;
		for(i=0;i<zhunju_N;i++)
		{
			ar>>w;	zhunju_table[i].wide=w;	//肢宽
			ar>>w;	zhunju_table[i].g=w;	//标准准距g
			ar>>w;	zhunju_table[i].g1=w;	//准距g1
			ar>>w;	zhunju_table[i].g2=w;	//准距g2
			ar>>w;	zhunju_table[i].g3=w;	//准距g3
		}
	}
	if(compareVersion(Ta.version,"1.3.0.1")>=0)
	{	//视图集合
		Ta.View.Empty();
		char version[20];
		_snprintf(version,19,"%s",Ta.version);
		ar>>n;
		for(i=0;i<n;i++)
		{
			CDisplayView* pDispView=Ta.AppendDisplayView();
			ar>>pDispView->handle;
			ar>>lh;
			char* mem_buffer = new char[lh];
			ar.Read(mem_buffer,lh);
			CBuffer buffer(lh);
			buffer.Write(mem_buffer,lh);
			delete []mem_buffer;
			buffer.SeekToBegin();
			buffer.ReadInteger(&lh);//iNo);
			buffer.ReadInteger(&pDispView->m_iViewType);
			buffer.ReadString(pDispView->name);
			buffer.ReadString(pDispView->defaultLayer);
			buffer.ReadString(pDispView->filter.FirstCharSet);
			buffer.ReadString(pDispView->filter.SecondCharSet);
			buffer.ReadString(pDispView->filter.ThirdCharSet);
			buffer.ReadPoint(pDispView->ucs.origin);
			buffer.ReadPoint(pDispView->ucs.axis_x);
			buffer.ReadPoint(pDispView->ucs.axis_y);
			buffer.ReadPoint(pDispView->ucs.axis_z);
			int n=0;
			buffer.ReadInteger(&n);
			pDispView->EmptyPartSet();
			for(int i=0;i<n;i++)
			{
				long h=0;
				buffer.ReadInteger(&h);
				CViewPart *pViewPart=pDispView->AppendViewPart(h);
				if(compareVersion(version,"1.0.0.25")<0)
					buffer.ReadDword(&dw);
				else if(compareVersion(version,"1.0.0.32")>=0)
					buffer.ReadInteger(&pViewPart->idClassType);
			}
		}
		ar>>lh;	//句柄值
		console.SetActiveView(lh);
	}

	CLDSNode *pNode;
	CLDSLinePart *pLinePart;
	ar>>n;
	for(i=0;i<n;i++)
	{
		pNode=console.AppendNode();
		RELATIVE_OBJECT *pMirObj;
		WORD w;
		ar>>pNode->handle;					//节点句柄
		if(compareVersion(Ta.version,"1.2.0.0")>=0)
			ar.Read(pNode->cfgword.flag.word,20);
		else
			ar>>pNode->cfgword.flag.word[0];
		if(compareVersion(Ta.version,"1.00.00.47")>=0)
		{
			ar>>w;
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
			if(w==FORCE_NODE)
				pNode->force_type=FORCE_NODE;
			else if(w==SWING_NODE)
				pNode->force_type=SWING_NODE;
			else
				pNode->force_type=AUTO_NODE;
#endif
		}
		ar.Read(pNode->layer(),4);
		f3dPoint pos;
		PointRW(ar,pos);		// 节点处“准线”的坐标
		pNode->SetPosition(pos);
		if(compareVersion(Ta.version,"1.00.00.46")>=0)
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
			PointRW(ar,pNode->PermanentLoad);	//此节点上的永久荷载
#else
			PointRW(ar,point);//pNode->PermanentLoad);	//此节点上的永久荷载
#endif
		long hPart;
		ar>>hPart;
		pNode->hFatherPart=hPart;		//节点所从属角钢句柄
		if(compareVersion(Ta.version,"1.4.0.1")<0)
		{
			long hObj=0;
			ar>>hObj;	//Y轴对称节点句柄
			pMirObj=pNode->relativeObjs.Add(hObj);
			pMirObj->mirInfo.axis_flag=2;
			ar>>hObj;	//X轴对称节点句柄
			pMirObj=pNode->relativeObjs.Add(hObj);
			pMirObj->mirInfo.axis_flag=1;
			ar>>hObj;	//Z轴对称节点句柄
			pMirObj=pNode->relativeObjs.Add(hObj);
			pMirObj->mirInfo.axis_flag=4;
			ar>>lh;//father_node_handle;
			ar>>w;//mir_msg;		// 是否有对称信息产生
		}
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
		ar>>pNode->point_i;
#else
		ar>>dw;//pNode->point_i;
#endif
		double attach_scale;
		ar>>attach_scale;
		pNode->attach_scale=attach_scale;// 比例点所点比例(自角钢起始点相对全角钢长)
		short attach_type;
		ar>>attach_type;
		if(attach_type==8)
			pNode->m_cPosCalViceType='Z';
		if(attach_type>=8)
			attach_type-=1;
		pNode->m_cPosCalType=(BYTE)attach_type;
		double attach_offset;
		ar>>attach_offset;		
		pNode->attach_offset=attach_offset;	// 偏移距离
		if(compareVersion(Ta.version,"1.5.0.1")>=0)
			ar.Read(pNode->arrRelationNode,4*sizeof(THANDLE));	// 根据两节点求比例等分点
		else
			ar.Read(pNode->arrRelationNode,2*sizeof(THANDLE));	// 根据两节点求比例等分点
		ar.Read(pNode->arrRelationPole,2*sizeof(THANDLE));	// 根据两角钢求交叉交点
		if(compareVersion(Ta.version,"1.00.00.34")>=0)
		{
			int j,hModule;
			ar>>lh;
			for(j=0;j<lh;j++)
			{
				ar>>hModule;//rest.m_hWorkModule;
				ar>>w;
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
				pNode->SetRestrict(hModule,w);
#endif
			}
			pNode->feature=0;
			if(compareVersion(Ta.version,"1.00.00.35")>=0)
			{
				ar>>lh;
				if(lh>0)
				{
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
					CNodeLoadList* pNodeLoadList = new CNodeLoadList;
					memcpy(&pNode->feature,&pNodeLoadList,4);	//临时赋指针给feature临时变量
					for(j=0;j<lh;j++)
					{
						double Cqi=0;
						CNodeLoad *pLoad=pNodeLoadList ->append();
						ar>>pLoad->m_hWorkStatus;//pLoad->m_hWorkModule;
						ar>>pLoad->m_hWorkStatus;
						ar>>Cqi;//pLoad->
						PointRW(ar,pLoad->wireFxy);
					}
#else
					for(j=0;j<lh;j++)
					{
						ar>>lh;
						ar>>lh;
						ar>>ff;
						PointRW(ar,point);
					}
#endif
				}
			}
		}
		if(compareVersion(Ta.version,"1.4.0.2")>=0)
		{
			ar>>lh;
			for(int j=0;j<lh;j++)
			{
				long hObj=0;
				ar>>hObj;
				pMirObj=pNode->relativeObjs.Add(hObj);
				if(compareVersion(Ta.version,"1.6.0.3")>=0)
				{
					ar>>pMirObj->mirInfo.axis_flag;
					PointRW(ar,(double*)pMirObj->mirInfo.origin);
					if((pMirObj->mirInfo.axis_flag&8)>0)
					{
						ar>>pMirObj->mirInfo.rotate_angle;
						ar>>pMirObj->mirInfo.array_num;
					}
				}
				else
				{
					ar>>h;pMirObj->mirInfo.axis_flag=(BYTE)h;
					PointRW(ar,(double*)pMirObj->mirInfo.origin);
					if((pMirObj->mirInfo.axis_flag&8)>0)
					{
						double ff;
						ar>>ff;pMirObj->mirInfo.rotate_angle=(short)ftoi(ff);
						ar>>h;pMirObj->mirInfo.array_num=(BYTE)h;
					}
				}
			}
		}
	}
	ar>>n;
	for(i=0;i<n;i++)
	{
		pLinePart=(CLDSLinePart*)console.AppendPart(CLS_LINEPART);
		RELATIVE_OBJECT *pMirObj;
		ar>>pLinePart->handle;
		if(compareVersion(Ta.version,"1.2.0.0")>=0)
			ar.Read(pLinePart->cfgword.flag.word,20);
		else
			ar>>pLinePart->cfgword.flag.word[0];
		ar.Read(pLinePart->layer(),4);
		pLinePart->layer(3)='\0';
		if(pLinePart->layer(1)=='B')
			pLinePart->layer(1)='F';
		ar>>h;pLinePart->pStart=Ta.Node.FromHandle(h);
		ar>>h;pLinePart->pEnd=Ta.Node.FromHandle(h);
		if(compareVersion(Ta.version,"1.00.3.0")>=0)
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
			ar>>pLinePart->m_iElemType;
#else
			ar>>w;
#endif
		if(compareVersion(Ta.version,"1.00.00.48")>=0)
		{
			ar>>w;
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
			if(w==0)
				pLinePart->m_bNeedSelMat=FALSE;
			else
				pLinePart->m_bNeedSelMat=TRUE;
#endif
			if(compareVersion(Ta.version,"1.0.2.0")>=0)
			{
				ar>>w;
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
				if(w==0)
					pLinePart->m_bNeedSelSize=FALSE;
				else
					pLinePart->m_bNeedSelSize=TRUE;
#endif
			}
		}
		if(compareVersion(Ta.version,"1.00.00.31")>=0)
		{
			ar>>w;
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
			if(w==0)
				pLinePart->start_joint_type=JOINT_HINGE;
			else if(w==1)
				pLinePart->start_joint_type=JOINT_RIGID;
			else
				pLinePart->start_joint_type=JOINT_AUTOJUSTIFY;
#endif
			ar>>w;
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
			if(w==0)
				pLinePart->end_joint_type=JOINT_HINGE;
			else if(w==1)
				pLinePart->end_joint_type=JOINT_RIGID;
			else
				pLinePart->end_joint_type=JOINT_AUTOJUSTIFY;
#endif
			if(compareVersion(Ta.version,"1.00.00.33")>=0)
			{
				ar>>w;
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
				if(w==0)
					pLinePart->start_force_type=CENTRIC_FORCE;
				else
					pLinePart->start_force_type=ECCENTRIC_FORCE;
#endif
				ar>>w;
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
				if(w==0)
					pLinePart->end_force_type=CENTRIC_FORCE;
				else
					pLinePart->end_force_type=ECCENTRIC_FORCE;
#endif
			}
			if(compareVersion(Ta.version,"1.00.00.32")>=0)
			{
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
				ar>>pLinePart->CalLenCoef.hRefPole;
				if(compareVersion(Ta.version,"1.00.00.49")>=0)
					ar>>pLinePart->CalLenCoef.hHorizPole;
				ar>>pLinePart->CalLenCoef.minR.coef;
				if(compareVersion(Ta.version,"1.00.02.07")>=0)
					ar>>pLinePart->CalLenCoef.paraR.coef;
				pLinePart->CalLenCoef.minR.hStartNode=pLinePart->CalLenCoef.minR.hEndNode=0;
				pLinePart->CalLenCoef.paraR.hStartNode=pLinePart->CalLenCoef.paraR.hEndNode=0;
				ar>>pLinePart->CalLenCoef.iTypeNo;
				if(pLinePart->CalLenCoef.iTypeNo<5)
					pLinePart->CalLenCoef.iTypeNo=0;
				else if(pLinePart->CalLenCoef.iTypeNo<9)
					pLinePart->CalLenCoef.iTypeNo-=4;
				else if(pLinePart->CalLenCoef.iTypeNo==9||pLinePart->CalLenCoef.iTypeNo==10)
					pLinePart->CalLenCoef.iTypeNo=5;	//将指定计算长度统一归为平行轴最小轴同时指定一种类型
				ar>>pLinePart->CalLenCoef.iSelRFlag;
#else
				ar>>h;
				if(compareVersion(Ta.version,"1.00.00.49")>=0)
					ar>>h;
				ar>>ff;//pLinePart->CalLenCoef.spec_callenMinR;
				if(compareVersion(Ta.version,"1.00.02.07")>=0)
					ar>>ff;//pLinePart->CalLenCoef.spec_callenParaR;
				ar>>dw;//pLinePart->CalLenCoef.iTypeNo;
				ar>>dw;//pLinePart->CalLenCoef.iSelRFlag;
#endif
			}
		}
		ar>>dw;	
		if(dw==TRUSS_POLE_JG)
			pLinePart->size_idClassType=CLS_LINEANGLE;
		else if(dw==TRUSS_POLE_JG_T_COM)
		{
			pLinePart->size_idClassType=CLS_GROUPLINEANGLE;
			pLinePart->size_cSubClassType='T';
		}
		else if(dw==TRUSS_POLE_JG_D_COM)
		{
			pLinePart->size_idClassType=CLS_GROUPLINEANGLE;
			pLinePart->size_cSubClassType='D';
		}
		else if(dw==TRUSS_POLE_JG_X_COM)
		{
			pLinePart->size_idClassType=CLS_GROUPLINEANGLE;
			pLinePart->size_cSubClassType='X';
		}
		else if(dw==TRUSS_POLE_TUBE)
			pLinePart->size_idClassType=CLS_LINETUBE;
		else if(dw==TRUSS_POLE_SLOT)
			pLinePart->size_idClassType=CLS_LINESLOT;
		else if(dw==TRUSS_POLE_FLAT)
			pLinePart->size_idClassType=CLS_LINEFLAT;
		else if(dw==TRUSS_POLE_GUYS)
			pLinePart->size_idClassType=0;//TRUSS_POLE_GUYS;
		else
			pLinePart->size_idClassType=0;//TRUSS_POLE_USERDEF;
		if(compareVersion(Ta.version,"1.00.00.33")<0)
			ar>>lh;	//此值已无用
		SIZESPEC size=pLinePart->GetSizeSpec();
		ar>>size.wide;
		ar>>size.thick;
		char material;
		ar>>material;	pLinePart->cMaterial=material;
		ar>>w;
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
		pLinePart->m_uStatMatNo=w;			//统材组号
#endif
		if(compareVersion(Ta.version,"1.4.0.2")<0)
		{
			ar>>w;	//mir_msg=w;		//对称信息
			long hObj=0;
			ar>>hObj;	//Y轴对称节点句柄
			pMirObj=pLinePart->relativeObjs.Add(hObj);
			pMirObj->mirInfo.axis_flag=2;
			ar>>hObj;	//X轴对称节点句柄
			pMirObj=pLinePart->relativeObjs.Add(hObj);
			pMirObj->mirInfo.axis_flag=1;
			ar>>hObj;	//Z轴对称节点句柄
			pMirObj=pLinePart->relativeObjs.Add(hObj);
			pMirObj->mirInfo.axis_flag=4;
			ar>>h;//father_pole_handle;	// 轴对称的杆件
		}
		if(compareVersion(Ta.version,"1.00.00.36")>=0)
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
			ar>>pLinePart->m_fWeightRaiseCoef;	// 重力放大系数，考虑联板、螺栓等对重力的放大作用
#else
			ar>>ff;//pLinePart->m_fWeightRaiseCoef;	// 重力放大系数，考虑联板、螺栓等对重力的放大作用
#endif
		if(compareVersion(Ta.version,"1.00.00.33")>=0)
		{
				//-----补助材计算数据-------
			ar>>w;
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
			pLinePart->sel_mat_method=AUX_POLE_SEL_METHOD(w);	//选材方式
			ar>>pLinePart->bear_load_coef;				//承担所支撑杆件内力的比例
			// --------- 3. 其它数据: 螺栓信息和杆件类型 -----------
			if(compareVersion(Ta.version,"1.00.00.49")>=0)
			{
				ar>>pLinePart->hole_out;
				if(compareVersion(Ta.version,"1.2.0.1")>=0)
				{	ar>>w; pLinePart->m_bAutoCalHoleOutCoef=(BOOL)w;	}
			}
#else
			ar>>ff;//pLinePart->bear_load_coef;				//承担所支撑杆件内力的比例
			// --------- 3. 其它数据: 螺栓信息和杆件类型 -----------
			if(compareVersion(Ta.version,"1.00.00.49")>=0)
			{
				ar>>ff;//pLinePart->hole_out;
				if(compareVersion(Ta.version,"1.2.0.1")>=0)
					ar>>w; //pLinePart->m_bAutoCalHoleOutCoef=(BOOL)w;
			}
#endif
			else
			{
				short MaxLsRows;
				if(compareVersion(Ta.version,"1.00.00.45")>=0)
					ar>>MaxLsRows;
			}
			ar>>pLinePart->connectStart.d;   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
			ar.Read(pLinePart->connectStart.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
			long count=0;
			ar>>count;   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
			pLinePart->connectStart.wnSummBoltN=(BYTE)count;
			ar>>pLinePart->connectEnd.d;   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
			ar.Read(pLinePart->connectEnd.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
			ar>>count;   	//(非水平杆件的上端或水平杆件的终止端螺栓信息
			pLinePart->connectEnd.wnSummBoltN=(BYTE)count;
		}
		if(compareVersion(Ta.version,"1.00.00.49")>=0)
		{
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
			ar>>pLinePart->userdef.E;
			ar>>pLinePart->userdef.area;
			ar>>pLinePart->userdef.ft;
			ar>>pLinePart->userdef.fc;
			ar>>pLinePart->userdef.unit_weight;
			if(compareVersion(Ta.version,"1.00.00.50")>=0&&compareVersion(Ta.version,"1.00.02.00")<0)
				ar>>pLinePart->userdef.pre_stress;	//以前的格式现在仅是为占位
			if(compareVersion(Ta.version,"1.00.00.52")>=0)
				ar>>pLinePart->userdef.pre_stress;
#else
			ar>>ff;//pLinePart->userdef.E;
			ar>>ff;//pLinePart->userdef.area;
			ar>>ff;//pLinePart->userdef.ft;
			ar>>ff;//pLinePart->userdef.fc;
			ar>>ff;//pLinePart->userdef.unit_weight;
			if(compareVersion(Ta.version,"1.00.00.50")>=0&&compareVersion(Ta.version,"1.00.02.00")<0)
				ar>>ff;//pLinePart->userdef.pre_stress;	//以前的格式现在仅是为占位
			if(compareVersion(Ta.version,"1.00.00.52")>=0)
				ar>>ff;//pLinePart->userdef.pre_stress;
#endif
		}
		if(compareVersion(Ta.version,"1.4.0.2")>=0)
		{
			ar>>lh;
			for(int j=0;j<lh;j++)
			{
				long hObj=0;
				ar>>hObj;
				pMirObj=pLinePart->relativeObjs.Add(hObj);
				if(compareVersion(Ta.version,"1.6.0.3")>=0)
				{
					ar>>pMirObj->mirInfo.axis_flag;
					PointRW(ar,(double*)pMirObj->mirInfo.origin);
					if((pMirObj->mirInfo.axis_flag&8)>0)
					{
						ar>>pMirObj->mirInfo.rotate_angle;
						ar>>pMirObj->mirInfo.array_num;
					}
				}
				else
				{
					ar>>h;pMirObj->mirInfo.axis_flag=(BYTE)h;
					PointRW(ar,(double*)pMirObj->mirInfo.origin);
					if((pMirObj->mirInfo.axis_flag&8)>0)
					{
						double ff;
						ar>>ff;pMirObj->mirInfo.rotate_angle=(short)ftoi(ff);
						ar>>h;pMirObj->mirInfo.array_num=(BYTE)h;
					}
				}
			}
		}
	}
	ar>>n;
	Ta.Module.Empty();
	CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(MODEL_GROUP);
	pNoGroup->EmptyNo();
	for(i=0;i<n;i++)
	{
		CLDSModule *pModule=Ta.Module.append();
		ar>>pModule->handle;
		ar>>pModule->iNo;
		pNoGroup->SetNoState(pModule->iNo);
		ar.Read(pModule->description,50);
		if(compareVersion(Ta.version,"1.2.0.0")>=0)
		{
			//下次升级版本号时去掉for循环 因为m_arrQuadLegCfgWord[4]改为了m_dwLegCfgWord wht 10-05-25
			for(int j=0;j<4;j++)
				ar.Read(pModule->m_dwLegCfgWord.flag.word,20);
			ar>>w;pModule->SetBodyNo(w);
		}
		else
		{
			//ar>>pModule->cfgword.flag.word[0];
			//pModule->SetBodyNo((WORD)pModule->cfgword.flag.word[0]);
			ar>>dw; pModule->SetBodyNo((WORD)dw);
			//下次升级版本号时去掉for循环 因为m_arrQuadLegCfgWord[4]改为了m_dwLegCfgWord wht 10-05-25
			for(int j=0;j<4;j++)
				ar>>pModule->m_dwLegCfgWord.flag.word[0];
		}
		if(compareVersion(Ta.version,"1.00.00.44")>=0)
		{
			for(int j=0;j<4;j++)	//本模型当前第一~四象限的当前接腿号
				ar>>pModule->m_arrActiveQuadLegNo[j];
		}
		if(compareVersion(Ta.version,"1.00.02.03")>=0)
			ar>>pModule->land_high;				//地形高(m)，用以计算杆塔风压高度变化系数
	}
	ar>>n;
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	Ta.WorkStatus.Empty();
	pNoGroup=Ta.NoManager.FromGroupID(WORKSTATUS_GROUP);
	pNoGroup->EmptyNo();
	CHashTable<CWorkStatus*>hashWorkStatus;
	hashWorkStatus.CreateHashTable(n);
	for(i=0;i<n;i++)
	{
		CWorkStatus *pStatus=Ta.WorkStatus.append();
		ar>>pStatus->handle;
		ar>>pStatus->iNo;
		hashWorkStatus.SetValueAt(pStatus->handle,pStatus);
		pNoGroup->SetNoState(pStatus->iNo);
		if(compareVersion(Ta.version,"1.00.02.06")>=0)
			ar.Read(pStatus->description,100);
		else
			ar.Read(pStatus->description,50);
		double kesai,gamaQ;
		ar>>kesai;
		pStatus->SetKesai(kesai);
		ar>>gamaQ;
		pStatus->SetGamaQ(gamaQ);
		if(compareVersion(Ta.version,"1.00.01.00")<=0)
			pStatus->SetGamaQ(1.4);
		if(compareVersion(Ta.version,"1.00.02.2")>=0)
			ar>>pStatus->ice_thick;
		if(compareVersion(Ta.version,"1.00.00.41")<0)
			ar>>Ta.m_siSafetyGrade;
		if(compareVersion(Ta.version,"1.00.00.36")<0)
		{
			double dd;
			long n;
			ar>>dd;
			ar>>n;
		}
		else
		{
			CFGWORD cfgword;
			if(compareVersion(Ta.version,"1.2.0.0")>=0)
				ar.Read(cfgword.flag.word,20);
			else
				ar>>cfgword.flag.word[0];
			ar>>dw;//dwQuad1LegCfgNo;
			ar>>dw;//dwQuad2LegCfgNo;
			ar>>dw;//dwQuad3LegCfgNo;
			ar>>dw;//dwQuad4LegCfgNo;
			if(compareVersion(Ta.version,"1.6.0.1")>=0)
				ar>>pStatus->WindLoad.velocity;
			else
			{
				ar>>lh;
				pStatus->WindLoad.velocity=lh;
			}
			if(Ta.volt_grade==330)		//330kV由原来的15m基准高风速改为10m高基准风速
				pStatus->WindLoad.velocity/=sqrt(1.14);
			else if(Ta.volt_grade==500)	//500kV由原来的15m基准高风速改为10m高基准风速
				pStatus->WindLoad.velocity/=sqrt(1.25);
			ar>>pStatus->WindLoad.azimuth;
			if(compareVersion(Ta.version,"1.00.02.2")>=0)
			{
				double case_coef;	//原指覆冰计算时的杆塔风荷载调整系数，现已转到系统参数
				ar>>case_coef;
			}
		}
		if(compareVersion(Ta.version,"1.00.01.00")>=0)
		{
			ar>>lh;
			for(int j=0;j<lh;j++)
			{
				CWorkStatus::WINDLOAD_REFPARAM refparam;
				ar>>h;
				if(compareVersion(Ta.version,"1.6.0.4")>=0)
					ar>>refparam.shieldCoef;
				pStatus->AttachWindLoad.SetValue(h,refparam);
			}
		}
		if(compareVersion(Ta.version,"1.00.00.40")>=0)
			ar.Read(pStatus->limit_str,100);
	}
	if(compareVersion(Ta.version,"1.00.01.00")>=0)
	{
		CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(WORKSTATUS_ATTACH_WINDLOAD);
		pNoGroup->EmptyNo();
		ar>>n;
		for(i=0;i<n;i++)
		{
			CAttachWindLoad *pWindLoad=Ta.AttachWindLoad.append();
			ar>>pWindLoad->handle;
			ar>>pWindLoad->iNo;
			ar.Read(pWindLoad->name,50);
			ar>>pWindLoad->m_fAttachWeight;
			ar>>pWindLoad->m_fProfileCoef;
			if(compareVersion(Ta.version,"1.0.2.5")>=0)
				ar>>pWindLoad->m_fBetaZ;
			ar>>pWindLoad->m_fShieldArea;
			ar>>pWindLoad->m_fShieldCoef;
			ar>>lh;
			for(long j=0;j<lh;j++)
			{
				ar>>h;
				pWindLoad->GetNodeList()->append(h);
			}
			if(compareVersion(Ta.version,"1.6.0.1")<=0)
			{
				for(long *pI=pWindLoad->GetNodeList()->GetFirst();pI;pI=pWindLoad->GetNodeList()->GetNext())
				{
					for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
					{
						if(pNode->point_i==*pI)
						{
							*pI=pNode->handle;
							break;
						}
					}
				}
			}
			pNoGroup->SetNoState(pWindLoad->iNo);
		}
	}
	if(compareVersion(Ta.version,"1.00.00.40")>=0)
	{
		ar>>n;
		Ta.WindLoadSeg.Empty();
		pNoGroup=Ta.NoManager.FromGroupID(WINDSEG_GROUP);
		pNoGroup->EmptyNo();
		double beta_z=1.0;
		if(compareVersion(Ta.version,"1.0.2.1")<0)
		{
			SCOPE_STRU scope;
			Ta.GetTaSizeScope(scope);
			beta_z=Find_BetaZ((scope.fMaxZ-scope.fMinZ)/1000,1);
		}
		for(i=0;i<n;i++)
		{
			CWindSegment *pWindSeg=Ta.WindLoadSeg.append();
			if(compareVersion(Ta.version,"1.0.2.1")>=0)
				ar>>pWindSeg->BetaZ;
			else
				pWindSeg->BetaZ=beta_z;
			ar>>pWindSeg->CqiFront;
			ar>>pWindSeg->CqiSide;
			if(compareVersion(Ta.version,"1.00.00.42")>=0)
				ar>>pWindSeg->K;
			ar>>pWindSeg->iNo;
			pNoGroup->SetNoState(pWindSeg->iNo);
			long iType;
			ar>>iType;
			pWindSeg->cType=(BYTE)(iType%100);
			if(pWindSeg->cType==1)	//0.八节点矩形塔身段
				pWindSeg->cType=0;
			else if(pWindSeg->cType<8)
				pWindSeg->cType=1;	//1.自定义多节点轮廓
			else
				pWindSeg->cType=2;	//2.六节点三角塔身段
			pWindSeg->cCalStyle=(BYTE)(iType/1000);	//0.塔身方式计算1.X方向水平横担2.Y方向水平横担
			pWindSeg->cMirType=(BYTE)((iType%1000)/100);	//对称信息0.无对称1.Y轴对称2.X轴对称
			for(int j=0;j<4;j++)
			{
				ar>>pWindSeg->nodeArr[j];
				pWindSeg->nodePtrArr[j]=Ta.Node.FromHandle(pWindSeg->nodeArr[j]);
			}
		}
	}
#endif
	if(compareVersion(Ta.version,"1.5.0.1")<=0)
	{
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(pNode->m_cPosCalType==4)	//交叉点
			{
				CSuperSmartPtr<CLDSPart> pRefPole1=Ta.Parts.FromHandle(pNode->arrRelationPole[0]);
				CSuperSmartPtr<CLDSPart> pRefPole2=Ta.Parts.FromHandle(pNode->arrRelationPole[1]);
				if(pRefPole1.IsHasPtr()&&pRefPole1->IsLinePart())
				{
					pNode->arrRelationNode[0]=pRefPole1.LinePartPointer()->pStart->handle;
					pNode->arrRelationNode[1]=pRefPole1.LinePartPointer()->pEnd->handle;
				}
				if(pRefPole2.IsHasPtr()&&pRefPole2->IsLinePart())
				{
					pNode->arrRelationNode[2]=pRefPole2.LinePartPointer()->pStart->handle;
					pNode->arrRelationNode[3]=pRefPole2.LinePartPointer()->pEnd->handle;
				}
			}
		}
	}
	Ta.GetMinAvailableHandle();
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		CNodeLoadList *pNodeLoadList=(CNodeLoadList*)pNode->feature;
		if(pNodeLoadList)
		{
			for(CNodeLoad *pNodeLoad=pNodeLoadList->GetFirst();pNodeLoad;pNodeLoad=pNodeLoadList->GetNext())
			{
				if(pNodeLoad->wireFxy.IsZero()&&pNodeLoad->permanentFz==0)
					continue;
				CWorkStatus *pStatus=NULL;
				hashWorkStatus.GetValueAt(pNodeLoad->m_hWorkStatus,pStatus);
				if(pStatus)
				{
					CExternalNodeLoad externalLoad;
					externalLoad.Fxyz=pNodeLoad->wireFxy/pStatus->GamaQ();
					externalLoad.permanentFz=pNodeLoad->permanentFz;
					pStatus->hashNodeLoad.SetValue(pNode->handle,externalLoad);
				}
			}
			if(pNodeLoadList)
				delete pNodeLoadList;
		}
	}
#endif
	Ta.Parts.RebuildHashtable();	//必须重建哈希表，否则由于添加Part后，重新修改赋值Part.handle,会导致哈希表乱
	int nVersion=FromStringVersion(Ta.version);
	Ta.InitEvolveCfgwordMap(nVersion,3);
	Ta.EvolveCfgword(Ta.cfgword,nVersion,3);
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		Ta.EvolveCfgword(pNode->cfgword,nVersion,3);
	for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		Ta.EvolveCfgword(pPart->cfgword,nVersion,3);
		SmartPartPtr pSmartPart=pPart;
		if(pSmartPart->IsLinePart()&&pSmartPart.pRod->pStart!=NULL&&pSmartPart.pRod->pEnd!=NULL)
			pSmartPart.pRod->IntelliUpdateConnBoltsBySummBolts();
	}
	return TRUE;
}
