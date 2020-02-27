// Bolt.cpp: implementation of the C3dLs class.
//wbt translation
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tower.h"
#include "BoltHoleInfo.h"
#include "CreateFace.h"
#include "../StdPartSolid/StdMetaSolid.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

IMPLEMENT_GET(DIANQUAN_STRU,AutoMatchThick)
{	//重新设计螺栓时是否自动设计垫圈厚度与数量
	if(_wholethick&0x8000)
		return false;
	else	//为保证与之前垫圈信息兼容，最高位为0时表示自动设计垫圈，为1时表示不重新设计厚度 wjh-2016.12.22
		return true;
}
IMPLEMENT_SET(DIANQUAN_STRU,AutoMatchThick)
{	//重新设计螺栓时是否自动设计垫圈厚度与数量
	if(value)
		_wholethick&=0x7fff;
	else
		_wholethick|=0x8000;
}
short DIANQUAN_STRU::get_thick()
{	//垫圈厚度获取与设置,thick<256;
	return _wholethick&0x7fff;
}
short DIANQUAN_STRU::set_thick(short _thickvalue)
{	//垫圈厚度获取与设置,thick<256;
	_wholethick&=0x8000;
	_wholethick|=(0x7fff&_thickvalue);
	return _wholethick&0x7fff;
}
LSPOS_STYLE TranslateLsPosStyleFromI(int i)
{
	if(i==LS_POS_AXIS_INTERS)
		return LS_POS_AXIS_INTERS;
	else if(i==LS_POS_JG_BER)
		return LS_POS_JG_BER;
	else if(i==LS_POS_NODE_PRJ)
		return LS_POS_NODE_PRJ;
	else if(i==LS_POS_LS)
		return LS_POS_LS;
	else if(i==LS_POS_NODE_FACEOFFSET)
		return LS_POS_NODE_FACEOFFSET;
	else if(i==LS_POS_UCS)
		return LS_POS_UCS;
	else //if(i==LS_POS_NO_PARA)
		return LS_POS_NO_PARA;
}
bool CLDSBolt::get_blCalL0IncGapByInters()
{
	return m_bCalL0IncGapByInters;
}
bool CLDSBolt::set_blCalL0IncGapByInters(bool blVal)
{
	return m_bCalL0IncGapByInters = blVal;
}
long CLDSBolt::get_m_hFamily()
{
	CLDSBolt* pMotherObj = (CLDSBolt*)MotherObject();
	if (pMotherObj == this)
	{
		if (_hFamily > 0)
			return _hFamily;
		else
			return CLsLibrary::DefaultFamily()->GetFamilyHandle();
	}
	else
		return pMotherObj->m_hFamily;
}
long CLDSBolt::set_m_hFamily(long value)
{
	if (value >= 0)
	{
		CLDSBolt* pMotherObj = (CLDSBolt*)MotherObject();
		_hFamily = (short)value;
		CalGuigeAuto();
		if (pMotherObj != this)
			pMotherObj->m_hFamily = value;
	}
	return _hFamily;
}
double CLDSBolt::get_hole_d_increment()const { return m_sfHoleDIncrement; }
double CLDSBolt::set_hole_d_increment(double value) { return m_sfHoleDIncrement = (float)value; }
//根据铁塔制图规范中的螺栓表计算双帽螺栓的有效长度 wht 11-07-29
//long CLDSBolt::TwoCapM16IncreL=10;//M16螺栓因增加螺帽导致有效长增加值
//long CLDSBolt::TwoCapM20IncreL=15;//M20螺栓因增加螺帽导致有效长增加值
//long CLDSBolt::TwoCapM24IncreL=20;//M24螺栓因增加螺帽导致有效长增加值
const DWORD CLDSBolt::TWO_CAP   =0X00000001;	//脚钉螺栓标识位
const DWORD CLDSBolt::ANTI_THEFT=0X00000002;	//防盗螺栓标识位
const DWORD CLDSBolt::ANTI_LOOSE=0X00000004;	//含防松垫圈标识位
const DWORD CLDSBolt::FOOT_NAIL =0X00000008;	//脚钉螺栓标识位
const DWORD CLDSBolt::REVERSE_NORM =0X00000010;	//螺栓朝向标识位
bool CLDSBolt::SyncMirProp(const char* propKeyStr/*=NULL*/,BYTE ciMirAll0X1Y2Z3/*=0*/,DWORD dwPropFilterFlag/*=0xffffffff*/)//propKeyStr表示同步对称全部属性
{
	return false;
}
bool CLDSBolt::SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)//propKeyStr表示同步对称全部位置对称无关的属性
{
	/*
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pDestObj;
	if(propKeyStr==NULL||strcmp(propKeyStr,"synchronize_scope")==0)
	{
		MEMORY_BLOCK cl=mpcl;
		if(cl.IsZero())
			cl=MPCL;
		PROPLIST_ITEM* pItem=propMirSync.GetPropItem("synchronize_scope");
		if(pItem!=NULL && cl.GetBitState((WORD)pItem->id-1))
			pDestObj->mpcl=mpcl;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"specification")==0)&&IsSyncMirPropWith(pDestObj,"specification"))
	{	//规格
		pAngle->size_wide=size_wide;
		pAngle->size_thick=size_thick;
		pAngle->size_idClassType=size_idClassType;
		pAngle->size_cSubClassType=size_cSubClassType;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"iSeg")==0)&&IsSyncMirPropWith(pDestObj,"iSeg"))
	{	//分段号
		pAngle->iSeg=iSeg;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cfgword")==0)&&IsSyncMirPropWith(pDestObj,"cfgword"))
	{	//配材号
		pAngle->cfgword=cfgword;
	}
	*/
	pDestObj->SetModified();
	return true;
}
bool CLDSBolt::SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)
{
	if(pDestObj==NULL)
		return false;
	CLDSBolt* pBolt=(CLDSBolt*)pDestObj;
	if(propKeyStr==NULL||strcmp(propKeyStr,"synchronize_scope")==0)
	{
		MEMORY_BLOCK cl=mpcl;
		if(cl.IsZero())
			cl=MPCL;
		PROPLIST_ITEM* pItem=propMirSync.GetPropItem("synchronize_scope");
		if(pItem!=NULL && cl.GetBitState((WORD)pItem->id-1))
			pDestObj->mpcl=mpcl;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"layer")==0)&&IsSyncMirPropWith(pDestObj,"layer"))
	{	//图层名同步对称
		MirObjLayer(layer(),pBolt->layer(),mirmsg);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"specification")==0)&&IsSyncMirPropWith(pDestObj,"specification"))
	{	//规格
		StrCopy(pBolt->grade,Grade(),5);
		pBolt->m_cFuncType=m_cFuncType;
		if((!IsFootNail()&&!pBolt->IsFootNail())||(IsFootNail()&&pBolt->IsFootNail()))
		{	//根据刘伟要求,螺栓为非脚钉时,才同步螺栓规格系列和特殊标识位  wxc-2017.4.11
			pBolt->set_d(d);
			pBolt->set_L(L);
			pBolt->m_hFamily=m_hFamily;
			pBolt->m_dwFlag=m_dwFlag;
		}
		pBolt->m_bVirtualPart=m_bVirtualPart;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"iSeg")==0)&&IsSyncMirPropWith(pDestObj,"iSeg"))
	{	//分段号
		pBolt->iSeg=iSeg;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cfgword")==0)&&IsSyncMirPropWith(pDestObj,"cfgword"))
	{	//配材号
		pBolt->cfgword=cfgword;
	}

	//将肢法线同步对称更新置于位置对称更新之前，是为了适应更新时调整肢法线朝向后应随动调整肢法向偏移量 wjh-2015.6.24
	if((propKeyStr==NULL||strcmp(propKeyStr,"workNormInfo")==0)&&IsSyncMirPropWith(pDestObj,"workNormInfo"))
	{
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"Position")==0)&&IsSyncMirPropWith(pDestObj,"Position"))
	{

		pBolt->des_base_pos.m_biWingOffsetStyle=des_base_pos.m_biWingOffsetStyle;
		if(pBolt->des_base_pos.cLocationStyle!=0)
		{
			pBolt->ucs.origin=mirmsg.MirPoint(ucs.origin);
			CSuperSmartPtr<CLDSPart> pDatumPart=BelongModel()->FromPartHandle(pBolt->des_base_pos.hPart);
			if(!pDatumPart.IsNULL())
			{
				if(pBolt->des_base_pos.R<=0)
				{	//非环向布置螺栓
					GEPOINT location=pBolt->ucs.origin;
					coord_trans(location,pDatumPart->ucs,FALSE,TRUE);
					pBolt->des_base_pos.len_offset_dist  = location.x;
					pBolt->des_base_pos.wing_offset_dist = location.y;
				}
				else 
				{
					//TODO:需要完善环向定位情况
				}
			}
		}
		else
		{
			pBolt->des_base_pos.len_offset_dist=des_base_pos.len_offset_dist;
			pBolt->des_base_pos.wing_offset_dist=des_base_pos.wing_offset_dist;
			if(pBolt->des_base_pos.datumPoint.datum_pos_style==1)
			{
				pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=
					des_base_pos.datumPoint.des_para.RODEND.wing_offset_style;
				pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=
					des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist;
			}
			else if(pBolt->des_base_pos.datumPoint.datum_pos_style==2)
			{
				pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style=
					des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style;
				pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist=
					des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist;
				pBolt->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset=
					des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset;
				//同步时由于对称螺栓的法向偏移不一定相同，故不更新法线方向 wjh-2016.6.25
				//pBolt->des_base_pos.datumPoint.des_para.RODNODE.face_norm_offset[0]=
				//	des_base_pos.datumPoint.des_para.RODNODE.face_norm_offset[0];
				//pBolt->des_base_pos.datumPoint.des_para.RODNODE.face_norm_offset[1]=
				//	des_base_pos.datumPoint.des_para.RODNODE.face_norm_offset[1];
				//pBolt->des_base_pos.datumPoint.des_para.RODNODE.face_norm_offset[2]=
				//	des_base_pos.datumPoint.des_para.RODNODE.face_norm_offset[2];
			}
			else if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)
			{
				pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=
					des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1;
				pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=
					des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2;
				pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1=
					des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1;
				pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2=
					des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2;
				MirPartHandle(BelongModel(),&pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1,
					des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1,mirmsg,&ucs.origin);
				MirPartHandle(BelongModel(),&pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2,
					des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2,mirmsg,&ucs.origin);
			}
		}
		pBolt->correct_pos();
	}
	return true;
}
IMPLEMENT_GET(CLDSBolt,m_bCanSimpleLocateMode)
{
	//TODO:判断还不够严格 wjh-2017.12.15
	if(m_bDisableSimpleLocateMode)
		return false;
	if(des_base_pos.datumPoint.datum_pos_style==1)
		return true;	//角钢楞点定位方式
	else if(des_base_pos.datumPoint.datum_pos_style==2)
		return true;	//杆件节点定位方式
	else if(des_base_pos.datumPoint.datum_pos_style==3)
		return true;	//心线交点定位方式
	else
		return false;
}
bool CLDSBolt::m_bEnableActiveHeightFilterOnL0InternalCal = false;
char CLDSBolt::BOLTL0_CALMODE = CLDSBolt::L0CAL_BY_BOLTSELF;
//默认为L0CAL_INC_GAP_THICK时有可能因为旧代码建模函数内部在确定螺栓位置前先算通厚导致通厚异常
//因此必须在特定场景下，进行如下设定才行： wjh-2019.7.23
//1)BOLTL0_CALMODE= CLDSBolt::L0CAL_BY_SUMM_THICK
//2)BOLTL0_PREFER_MODE=CLDSBolt::L0CAL_INC_GAP_THICK
//... 建模过程代码，过程结束确保螺栓已经正位
//3)BOLTL0_CALMODE= CLDSBolt::L0CAL_BY_BOLTSELF
char CLDSBolt::BOLTL0_PREFER_MODE = CLDSBolt::L0CAL_BY_SUMM_THICK;
CLDSBolt::CLDSBolt(IModel *pModel/*=NULL*/)
{
	//m_dwRef = 1;
	_hFamily =0;
	d = 20;
	m_sfHoleDIncrement = 1.5;
	_L = 60;
	_siInitStdL = 0;
	_L0 = 0;
	m_uStatMatCoef = 1;
	m_bNeedUpdateL0=false;
	dwRayNo = 0;	//缺省不在任何射线上
	grade[4]='\0';	//将末尾字符设为0，以便将来读取"10.9"级别时不出错
	strcpy(grade,"");//默认为空此时方可在调用CalGuigeAuto时自动识别为规格库中螺栓级别 wjh-2013.9.4
	strcpy(statSegStr,"");
	is_data_modified = TRUE;
	pSolidBody = NULL;
	memset(&DianQuan,0,sizeof(DIANQUAN_STRU));
	memset(&DianBan,0,sizeof(DIANBAN_STRU));
	des_base_pos.direction=des_base_pos.offset_wing=0;
	des_base_pos.hPart=0;
	des_base_pos.len_offset_dist=des_base_pos.wing_offset_dist=0;
	des_base_pos.m_biWingOffsetStyle=4;	//默认为自定义,否则可能会与原代码冲突重复计算横向偏移量 wjh-2016.4.14
	memset(&des_work_norm,0,sizeof(CLDSVector));
	m_nClassTypeId=CLS_BOLT;
	//strcpy(m_sClassName,"CLDSBolt");
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
	crMaterial=GetColor(CLS_BOLT,0,' ');
	m_bDisableSimpleLocateMode=false;
#ifdef __DRAWING_CONTEXT_
	memset(relaRayArr,0,sizeof(RELA_RAYPART)*4);//关联构件句柄
#endif
	SetBelongModel(pModel);
	m_bAggregated=false;
	this->m_bCalL0IncGapByInters= (CLDSBolt::BOLTL0_PREFER_MODE!=CLDSBolt::L0CAL_BY_SUMM_THICK);
}
bool CLDSBolt::IsAggregated()
{
	if(m_iShadowObjType!=SHADOW_EMBEDDED&&m_iShadowObjType!=SHADOW_EXTERBLK)
		return false;	//不是部件装配影射螺栓
	else
		return m_bAggregated;
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)&&defined(__DRAWING_CONTEXT_)
BOOL CLDSBolt::SetRelaPartRayNo(long hPart,DWORD dw,const double* rayVec)
{
	if(hPart==des_base_pos.hPart)
	{
		baseRayVec.Set(rayVec[0],rayVec[1],rayVec[2]);
		dwRayNo=dw;	//避免角钢中人为设定或历史遗留的射线号扰乱结果 wjh 2011.8.11
		return TRUE;
	}
	for(int i=0;i<4;i++)
	{
		if(relaRayArr[i].hRelaPart==0||relaRayArr[i].hRelaPart==hPart)
		{
			relaRayArr[i].hRelaPart=hPart;
			relaRayArr[i].dwRayNo=dw;
			relaRayArr[i].rayVec.Set(rayVec[0],rayVec[1],rayVec[2]);
			return TRUE;
		}
	}
	return FALSE;
}
#endif

CLDSBolt::~CLDSBolt()
{
	if(pSolidBody)
	{
		delete pSolidBody;
		pSolidBody=NULL;
	}
}
//TODO:暂时还未决定到底使用CLDSPart还是更详细的构件分类 wjh-2017.11.22
static BYTE _localUpdateMovePartsFlag=0x02;	//0x01:清除关联构件记录缓存;0x02:临时阻止记录角钢位置变动关联构件
static ARRAY_LIST<CLDSBolt*> _localMoveBoltArray;
static ARRAY_LIST<CLDSGeneralPlate*> _localMovePlateArray;
BYTE CLDSBolt::GetMovePartsUpdateFlag(){return _localUpdateMovePartsFlag;}
BYTE CLDSBolt::SetMovePartsUpdateFlag(BYTE cbFlag/*=0x01*/){return _localUpdateMovePartsFlag=cbFlag;}
void CLDSBolt::ClearUpdateMovePartsBuff()
{
	if(IsLaboratoryFuncEnabled()&&(_localUpdateMovePartsFlag&0x03)==CLEAR_BUFF_OF_RELA_PARTS)
	{	//清空角钢移位关联构件记录缓存
		_localMoveBoltArray.SetSize(0,50);
		_localMovePlateArray.SetSize(0,5);
		_localUpdateMovePartsFlag-=CLEAR_BUFF_OF_RELA_PARTS;
	}
}
//遍历第一颗因最近一次CLDSLineAngle::CalPosition影响到的位移螺栓
CLDSPart* CLDSBolt::EnumFirstRelaMovePart()
{
	if(!IsLaboratoryFuncEnabled())
		return NULL;
	CLDSBolt** ppBolt=_localMoveBoltArray.GetFirst();
	return ppBolt!=NULL?*ppBolt:NULL;
}
//遍历下一颗因最近一次CLDSLineAngle::CalPosition影响到的位移螺栓
CLDSPart* CLDSBolt::EnumNextRelaMovePart()
{
	if(!IsLaboratoryFuncEnabled())
		return NULL;
	CLDSBolt** ppBolt=_localMoveBoltArray.GetNext();
	return ppBolt!=NULL?*ppBolt:NULL;
}
IMPLEMENT_SET(CLDSBolt,m_cFuncType)
{
	if(value==CLDSBolt::FUNC_FOOTNAIL||value==CLDSBolt::FUNC_FOOTNAIL2)
	{
		if(get_oldL()>EPS)
			_cFuncType=CLDSBolt::FUNC_FOOTNAIL2;
		else
			_cFuncType=CLDSBolt::FUNC_FOOTNAIL;
	}
	else
		_cFuncType=value;
}
BYTE CLDSBolt::FuncType()
{
	if (_bVirtualPart&&_cFuncType<2)
		return 2;	//默认旧版文件中的虚拟螺栓为挂线孔
	else if (!_bVirtualPart&&_cFuncType>=2)
	{	//修正不合理的螺栓孔功用类型
		if (IsFootNail())
		{
			if (get_oldL()>EPS)
				_cFuncType=CLDSBolt::FUNC_FOOTNAIL2;
			else
				_cFuncType=CLDSBolt::FUNC_FOOTNAIL;
		}
		else	//连接螺栓
			return 0;
	}
	//人为自动设置
	if(_cFuncType>0)
		return _cFuncType;
	//智能判断
	if(IsFootNail())
	{
		if(get_oldL()>EPS)
			_cFuncType=CLDSBolt::FUNC_FOOTNAIL2;
		else
			_cFuncType=CLDSBolt::FUNC_FOOTNAIL;
	}
	return _cFuncType;
}
BYTE CLDSBolt::FilterFuncType()
{	//相比FuncType()，过滤代孔脚钉为脚钉
	if(FuncType()==CLDSBolt::FUNC_FOOTNAIL2)
		return FUNC_FOOTNAIL;
	else
		return m_cFuncType;
}
bool CLDSBolt::IsRelaUsed()
{
	CSuperSmartPtr<CLDSPart>pPart;
	for(pPart=BelongModel()->EnumPartFirst();pPart.IsHasPtr();pPart=BelongModel()->EnumPartNext())
	{
		if(pPart->IsBlockRefShadowObject())
			continue;	//跳过装配部件引用炸开后的影射构件，只计塔主体构件中的螺栓引用
		if(pPart->IsLinePart()&&pPart.LinePartPointer()->FindLsByHandle(handle))
			return true;
		else if(pPart->GetClassTypeId()==CLS_PLATE&&pPart.PlatePointer()->FindLsByHandle(handle)!=NULL)
			return true;
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&pPart.ParamPlatePointer()->FindLsByHandle(handle)!=NULL)
			return true;
		else if(pPart->IsArcPart()&&pPart.ArcPartPointer()->FindLsByHandle(handle)!=NULL)
			return true;
	}
	return false;
}
bool CLDSBolt::IsBelongTo(int iClassTypeId)const
{
	if(iClassTypeId==m_nClassTypeId||iClassTypeId==CLS_PART)
		return true;
	else
		return false;
}
BOOL CLDSBolt::IsStatSeg(SEGI seg)
{
	if(seg.iSeg==iSeg.iSeg)
		return TRUE;
	else if(strlen(statSegStr)>0)
	{
		char *separator=",";
		char segStrTemp[100];
		strcpy(segStrTemp,statSegStr);
		char *key=strtok(segStrTemp,separator);
		while(key!=NULL)
		{
			int i,key_len=strlen(key);
			for(i=0;i<key_len;i++)
			{
				if(key[i]=='-')
				{
					key[i]='\0';
					break;
				}
			}
			SEGI min_seg=SEGI(key);
			SEGI max_seg=min_seg;
			if(i<key_len)
				max_seg=SEGI(&key[i+1]);
			if(seg.iSeg>=min_seg.iSeg&&seg.iSeg<=max_seg.iSeg)
				return TRUE;
			key=strtok(NULL,separator);
		}
	}
	return FALSE;
}
int CLDSBolt::GetStatSegArr(SEGI seg_arr[],int maxSegN)
{
	SEGI segArr[100]={SEGI()};
	char segStrTemp[100];
	if(maxSegN>100)	//再多无实际工程意义
		maxSegN=100;
	int segN=1;
	segArr[0]=iSeg;
	strcpy(segStrTemp,statSegStr);
	SEGI iNo;
	if(strlen(statSegStr)>0)
	{
		char *separator=",";
		char *key=strtok(segStrTemp,separator);
		while(key!=NULL)
		{
			int key_len=strlen(key);
			for(int i=0;i<key_len;i++)
			{
				if(key[i]=='-')
				{
					key[i]='\0';
					break;
				}
			}
			SEGI min_seg=SEGI(key);
			SEGI max_seg=min_seg;
			if(i<key_len)
				max_seg=SEGI(&key[i+1]);
			for(iNo.iSeg=min_seg.iSeg;iNo.iSeg<=max_seg.iSeg;iNo.iSeg++)
			{
				int j;
				for(j=0;j<segN;j++)
				{
					if(segArr[j].iSeg==iNo.iSeg)
						break;
					else if(iNo.iSeg<segArr[j].iSeg)	//&&segN<100
					{
						memmove(&segArr[j+1],&segArr[j],sizeof(SEGI)*(segN-j));
						segArr[j]=iNo;
						segN++;	//新增一个段号
						break;
					}
				}
				if(j==segN)
				{
					segArr[j]=iNo;
					segN++;
				}
				if(segN==100)
					break;
			}
			key=strtok(NULL,separator);
		}
		int copy_n=min(segN,maxSegN);
		memcpy(seg_arr,segArr,sizeof(SEGI)*copy_n);
	}
	else if(maxSegN>0)
		seg_arr[0]=iSeg;
	return segN;
}

int CLDSBolt::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	char sText[100]="";
#ifdef __LDS_CONTEXT_
	QUEUENODE_HOLE *pRelaHole = hashBoltHoles.GetValue(handle);
#endif
	if(GetPropID("basicInfo")==0)
		strcpy(sText,"");
	else if(GetPropID("handle")==id)
		sprintf(sText,"0X%X",handle);
	else if(GetPropID("layer")==id)
		strcpy(sText,layer());
	else if(CLDSBolt::GetPropID("m_hFamily")==id)
	{
		CLsFamily *pFamily=CLsLibrary::FromHandle(m_hFamily);
		if(pFamily==NULL)
			pFamily=CLsLibrary::DefaultFamily();
		if(pFamily)
			strcpy(sText,pFamily->name);
	}
	else if(GetPropID("grade")==id)
		sprintf(sText,"%s",Grade());
	else if(GetPropID("specification")==id)
		sprintf(sText,"%sM%dX%d", Grade(),get_d(),_L); 
	else if(GetPropID("L0")==id)
		sprintf(sText,"%d",_L0); 
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("statSegStr")==id)
		strcpy(sText,statSegStr);
	else if(GetPropID("dwRayNo")==id)
		return MakeWordToString(dwRayNo,valueStr);
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
	else if(GetPropID("hole_d_increment")==id)
	{
		sprintf(sText,"%f",hole_d_increment);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("gasketInfo")==id)
	{
		if(DianQuan.thick>0&&DianQuan.N>0)
			sprintf(sText,"-%dX%d",DianQuan.thick,DianQuan.N);
		else
			strcpy(sText,""); 
	}
	else if(GetPropID("gasketThick")==id)
		sprintf(sText,"%d",DianQuan.thick);
	else if(GetPropID("gasketN")==id)
		sprintf(sText,"%d",DianQuan.N);
	else if(GetPropID("gasketOffset")==id)
	{
		sprintf(sText,"%f",DianQuan.offset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bVirtualPart")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bVirtualPart)
			strcpy(sText,"Yes");	//隐含构件
		else
			strcpy(sText,"No");
#else 
		if(m_bVirtualPart)
			strcpy(sText,"是");	//隐含构件
		else
			strcpy(sText,"否");
#endif
	}
	/*else if(GetPropID("m_bFootNail")==id)
	{
		if(m_bFootNail)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
	}
	else if(GetPropID("m_bTwoCapLs")==id)
	{
		if(m_bTwoCapLs)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
	}*/
	else if(GetPropID("m_bAntiLooseWasher")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(this->IsAntiLoose())//m_dwFlag.IsHasFlag(ANTI_LOOSE))
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(this->IsAntiLoose())//m_dwFlag.IsHasFlag(ANTI_LOOSE))
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_bAntiTheft")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(IsAntiTheft())
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else	//IsAntiTheft()包括了m_dwFlag含防盗属性及选用防盗螺栓系列两种情况 wjh-2019.3.28
		if(IsAntiTheft())//m_dwFlag.IsHasFlag(ANTI_THEFT))
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_bUserSpecColor")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bUserSpecColor)
			sprintf(sText,"Yes");
		else
			sprintf(sText,"No");
#else 
		if(m_bUserSpecColor)
			sprintf(sText,"是");
		else
			sprintf(sText,"否");
#endif
	}
	else if(GetPropID("crMaterial")==id)
		sprintf(sText,"RGB%X",crMaterial);
	else if(GetPropID("sNotes")==id)
		strcpy(sText,sNotes);
	else if (GetPropID("m_bCalL0IncGapByInters") == id)
	{
		if (m_bCalL0IncGapByInters)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "1.Include Part Gap"; 
		else
			valueStr = "0.Sum Part Thick";
#else 
			strcpy(sText, "1.计入缺失垫板间隙");
		else
			strcpy(sText, "0.累加通厚");
#endif
	}
	else if(GetPropID("assemblePos")==id)
		strcpy(sText,"");
	else if(GetPropID("des_base_pos")==id)
	{
		if(m_bCanSimpleLocateMode)
			strcpy(sText,"简单模式");
		else
			strcpy(sText,"高级模式");
	}
	else if(GetPropID("des_base_pos.datum_pos_style")==id)
	{
		if(des_base_pos.datumPoint.datum_pos_style==1)
			strcpy(sText,"1.角钢楞点定位");
		else if(des_base_pos.datumPoint.datum_pos_style==2)
			strcpy(sText,"2.角钢节点定位");
		else if(des_base_pos.datumPoint.datum_pos_style==3)
			strcpy(sText,"3.角钢心线交点");
		else
			strcpy(sText,"其它");
	}
	else if(GetPropID("des_base_pos.fWingOffDist1")==id)
	{
		CLDSLineAngle* pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(des_base_pos.hPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pAngle==NULL)
		{
			Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"螺栓0X%X的定位角钢0X%X不存在!",handle,des_base_pos.hPart);
			return FALSE;
		}
		JGZJ jgzj=pAngle->GetZhunJu(des_base_pos.offset_wing==0?'X':'Y');
		short siStep1WingOffsetStyle=4,siStep2WingOffsetStyle=des_base_pos.m_biWingOffsetStyle;
		double fStep1WingOffsetDist=0,fStep2WingOffsetDist=des_base_pos.wing_offset_dist;
		if(des_base_pos.datumPoint.datum_pos_style==2)
		{
			CLDSNode* pNode=BelongModel()->FromNodeHandle(des_base_pos.datumPoint.m_hNode);
			if(pNode!=NULL)
				jgzj.g=ftoi(des_base_pos.offset_wing==0?pAngle->GetNodeWingXG(pNode):pAngle->GetNodeWingYG(pNode));
		}
		double fDist=fStep2WingOffsetDist=jgzj.ByStyleG(des_base_pos.m_biWingOffsetStyle,des_base_pos.wing_offset_dist);;
		if(des_base_pos.datumPoint.datum_pos_style==1)		//角钢楞点
		{
			siStep1WingOffsetStyle=des_base_pos.datumPoint.des_para.RODEND.wing_offset_style;
			fStep1WingOffsetDist=jgzj.ByStyleG(des_base_pos.datumPoint.des_para.RODEND.wing_offset_style,des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist);
			fDist+=fStep1WingOffsetDist;
		}
		else if(des_base_pos.datumPoint.datum_pos_style==2)	//角钢节点
		{
			siStep1WingOffsetStyle=des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style;
			fStep1WingOffsetDist=jgzj.ByStyleG(des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style,des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist);
			fDist+=fStep1WingOffsetDist;
		}
		else if(des_base_pos.datumPoint.datum_pos_style==3)	//心线交点
		{	
			if(des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1==des_base_pos.hPart)
			{
				siStep1WingOffsetStyle=des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1;
				fStep1WingOffsetDist=jgzj.ByStyleG(des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1,des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1);
				fDist=fStep1WingOffsetDist;
			}
			else
			{
				siStep1WingOffsetStyle=des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2;
				fStep1WingOffsetDist=jgzj.ByStyleG(des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2,des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2);
				fDist=fStep1WingOffsetDist;
			}
		}
		//
		int iStyleG=jgzj.MatchStyleG(fDist);
		if(siStep1WingOffsetStyle!=4&&fStep2WingOffsetDist==0)
			iStyleG=siStep1WingOffsetStyle;
		else if(siStep2WingOffsetStyle!=4&&fStep1WingOffsetDist==0)
			iStyleG=siStep2WingOffsetStyle;
		if(iStyleG==0)
			sprintf(sText,"g=%f",fDist);
		else if(iStyleG<4)
			sprintf(sText,"g%d=%f",iStyleG,fDist);
		else //if(iStyleG==4)
			sprintf(sText,"%f",fDist);
		SimplifiedNumString(sText);	
	}
	else if(GetPropID("des_base_pos.fWingOffDist3")==id)
	{
		CLDSLineAngle* pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(des_base_pos.hPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pAngle==NULL)
		{
			Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "螺栓0X%X的定位角钢0X%X不存在!", handle, des_base_pos.hPart);
			return FALSE;
		}
		JGZJ jgzj=pAngle->GetZhunJu(des_base_pos.offset_wing==0?'X':'Y');
		if(des_base_pos.datumPoint.datum_pos_style==2)
		{
			CLDSNode* pNode=BelongModel()->FromNodeHandle(des_base_pos.datumPoint.m_hNode);
			if(pNode!=NULL)
				jgzj.g=ftoi(des_base_pos.offset_wing==0?pAngle->GetNodeWingXG(pNode):pAngle->GetNodeWingYG(pNode));
		}
		double fDist=0;
		if(des_base_pos.datumPoint.datum_pos_style==2&&!des_base_pos.datumPoint.des_para.RODNODE.vPlaneNormal.IsZero())	//角钢节点
			fDist=jgzj.ByStyleG(des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style,des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist);
		//
		int iStyleG=jgzj.MatchStyleG(fDist);
		if(des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style!=4)
			iStyleG=des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style;
		if(iStyleG==0)
			sprintf(sText,"g=%f",fDist);
		else if(iStyleG<4)
			sprintf(sText,"g%d=%f",iStyleG,fDist);
		else //if(iStyleG==4)
			sprintf(sText,"%f",fDist);
		SimplifiedNumString(sText);	
	}
	else if(GetPropID("des_base_pos.hCrossPart")==id&&des_base_pos.datumPoint.datum_pos_style==3)
	{
		if(des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1==des_base_pos.hPart)
			sprintf(sText,"0X%X",des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2);
		else
			sprintf(sText,"0X%X",des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1);
	}
	else if(GetPropID("des_base_pos.fWingOffDist2")==id&&des_base_pos.datumPoint.datum_pos_style==3)
	{
		CLDSLineAngle* pCrossJg;
		long hCrossJg=des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
		double fDist=des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1;
		int iStyleG=des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1;
		if(des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1==des_base_pos.hPart)
		{	//交叉基准点中第一根参照角钢为基准角钢，第二根为交叉参照角钢
			hCrossJg=des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
			if((pCrossJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(hCrossJg,CLS_LINEANGLE))==NULL)
			{
				Log2File()->Log("螺栓0X%X的定位交叉角钢0X%X不存在!",handle,hCrossJg);
				return FALSE;
			}
			JGZJ jgzj=pCrossJg->GetZhunJu();
			fDist=jgzj.ByStyleG(des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2,des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2);
			if(des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2==4)
				iStyleG=jgzj.MatchStyleG(fDist);
			else
				iStyleG=des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2;
		}
		else
		{
			if((pCrossJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(hCrossJg,CLS_LINEANGLE))==NULL)
			{
				Log2File()->Log("螺栓0X%X的定位交叉角钢0X%X不存在!",handle,hCrossJg);
				return FALSE;
			}
			JGZJ jgzj=pCrossJg->GetZhunJu();
			fDist=jgzj.ByStyleG(des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1,des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1);
			if(iStyleG==4)
				iStyleG=jgzj.MatchStyleG(fDist);
		}
		//
		if(iStyleG==0)
			sprintf(sText,"g=%f",fDist);
		else if(iStyleG<4)
			sprintf(sText,"g%d=%f",iStyleG,fDist);
		else //if(iStyleG==4)
			sprintf(sText,"%f",fDist);
		SimplifiedNumString(sText);	
	}
	else if(GetPropID("des_base_pos.iDirection")==id)
	{
		if(des_base_pos.direction==0)
		{
			if(des_base_pos.datumPoint.datum_pos_style==1)	//角钢楞点
				strcpy(sText,"始端");
			else
				strcpy(sText,"始->终");
		}
		else
		{
			if(des_base_pos.datumPoint.datum_pos_style==1)	//角钢楞点
				strcpy(sText,"终端");
			else
				strcpy(sText,"终->始");
		}
	}
	else if(GetPropID("des_base_pos.fLenOffDist")==id)
	{
		double fDist=des_base_pos.len_offset_dist;
		if(des_base_pos.datumPoint.datum_pos_style==1)		//角钢楞点
		{	
			if(des_base_pos.datumPoint.des_para.RODEND.direction==des_base_pos.direction)
				fDist+=des_base_pos.datumPoint.des_para.RODEND.len_offset_dist;
			else
				fDist-=des_base_pos.datumPoint.des_para.RODEND.len_offset_dist;
		}
		else if(des_base_pos.datumPoint.datum_pos_style==2)	//角钢节点
		{	
			if(des_base_pos.datumPoint.des_para.RODNODE.direction==des_base_pos.direction)
				fDist+=des_base_pos.datumPoint.des_para.RODNODE.len_offset_dist;
			else
				fDist-=des_base_pos.datumPoint.des_para.RODNODE.len_offset_dist;
		}
		sprintf(sText,"%f",fDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_base_pos.datumPoint")==id)
	{
		if(m_dwFlag.IsHasFlag(CLDSBolt::REVERSE_NORM))
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"-L0");
#else 
			strcpy(sText,"-通厚");
#endif
		else
			strcpy(sText,"");
	}
	else if(GetPropID("des_base_pos.datumPoint.x")==id)
	{
		des_base_pos.datumPoint.UpdatePos(m_pModel);
		sprintf(sText,"%f",des_base_pos.datumPoint.Position().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_base_pos.datumPoint.y")==id)
	{
		des_base_pos.datumPoint.UpdatePos(m_pModel);
		sprintf(sText,"%f",des_base_pos.datumPoint.Position().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_base_pos.datumPoint.z")==id)
	{
		des_base_pos.datumPoint.UpdatePos(m_pModel);
		sprintf(sText,"%f",des_base_pos.datumPoint.Position().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_base_pos.hPart")==id||GetPropID("des_base_pos.hBasePart")==id)
		sprintf(sText,"0X%X",des_base_pos.hPart);
	else if(GetPropID("des_base_pos.hBaseNode")==id)
		sprintf(sText,"0X%X",des_base_pos.datumPoint.des_para.RODNODE.hNode);
	else if(GetPropID("des_base_pos.m_biWingOffsetStyle")==id)
	{
		if(des_base_pos.m_biWingOffsetStyle==0)
			strcpy(sText,"g");
		else if(des_base_pos.m_biWingOffsetStyle==1)
			strcpy(sText,"g1");
		else if(des_base_pos.m_biWingOffsetStyle==2)
			strcpy(sText,"g2");
		else if(des_base_pos.m_biWingOffsetStyle==3)
			strcpy(sText,"g3");
		else if(des_base_pos.m_biWingOffsetStyle==4)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User-Defined");
#else
			strcpy(sText,"自定义");
#endif
	}
	else if(GetPropID("des_base_pos.wing_offset_dist")==id)
	{
		sprintf(sText,"%f",des_base_pos.wing_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_base_pos.len_offset_dist")==id)
	{
		sprintf(sText,"%f",des_base_pos.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RotationStartEdge")==id)
	{
		long iStyle0Datum1WeldEdge=0;
		CSuperSmartPtr<CLDSPart> pPlate=BelongModel()->FromPartHandle(des_base_pos.hPart);
		if(pPlate.IsHasPtr()&&pPlate->GetClassTypeId()==CLS_PARAMPLATE && pPlate.ParamPlatePointer()->m_iParamType==TYPE_FL)
			pPlate.ParamPlatePointer()->GetDesignItemValue('J',&iStyle0Datum1WeldEdge);
		else if(pPlate.IsHasPtr()&&pPlate->GetClassTypeId()==CLS_PLATE)
			iStyle0Datum1WeldEdge=pPlate.PlatePointer()->m_bAttachPartByWeldRoad;
		if(iStyle0Datum1WeldEdge!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"1.Weld Edge");
		else
			strcpy(sText,"0.Datum Edge");
#else
			strcpy(sText,"1.焊道线");
		else
			strcpy(sText,"0.基准边");
#endif
	}
	else if(GetPropID("des_base_pos.offset_angle")==id)
	{
		sprintf(sText,"%f",des_base_pos.offset_angle*DEGTORAD_COEF);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_base_pos.R")==id)
	{
		sprintf(sText,"%f",des_base_pos.R);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_base_pos.offset_wing")==id||GetPropID("des_base_pos.iWorkWing")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_base_pos.offset_wing==0)
			strcpy(sText,"X Leg");
		else if(des_base_pos.offset_wing==1)
			strcpy(sText,"Y Leg");
#else 
		if(des_base_pos.offset_wing==0)
			strcpy(sText,"X肢");
		else if(des_base_pos.offset_wing==1)
			strcpy(sText,"Y肢");
#endif
	}
	else if(GetPropID("des_base_pos.cResidePlane")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_base_pos.reside_plane==0)
			strcpy(sText,"Datum Plane");
		else if(des_base_pos.reside_plane==1)
			strcpy(sText,"Bending plane"); 
		else if(des_base_pos.reside_plane==2)
			strcpy(sText,"2nd bending plane"); 
#else 
		if(des_base_pos.reside_plane==0)
			strcpy(sText,"基准平面");
		else if(des_base_pos.reside_plane==1)
			strcpy(sText,"火曲面"); 
		else if(des_base_pos.reside_plane==2)
			strcpy(sText,"第二火曲面"); 
#endif
	}
	else if(GetPropID("des_base_pos.direction")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_base_pos.direction==0)
			strcpy(sText,"Start->End");
		else if(des_base_pos.direction==1)
			strcpy(sText,"End->Start");
#else 
		if(des_base_pos.direction==0)
			strcpy(sText,"始->终");
		else if(des_base_pos.direction==1)
			strcpy(sText,"终->始");
#endif
	}
	else if(GetPropID("des_base_pos.norm_offset")==id)
		sprintf(sText,"%d",des_base_pos.norm_offset.Thick(BelongModel()));
	else if(GetPropID("ucs.origin.x")==id)
	{
		sprintf(sText,"%f",ucs.origin.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.y")==id)
	{
		sprintf(sText,"%f",ucs.origin.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.z")==id)
	{
		sprintf(sText,"%f",ucs.origin.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_work_norm.norm_style")==id)
		strcpy(sText,des_work_norm.GetNormStyleDes());
	else if(GetPropID("des_work_norm.hVicePart")==id)
		sprintf(sText,"0X%X",des_work_norm.hVicePart);
	else if(GetPropID("des_work_norm.norm_wing")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_work_norm.norm_wing==0)
			strcpy(sText,"X leg normal");
		else if(des_work_norm.norm_wing==1)
			strcpy(sText,"Y leg normal"); 
#else 
		if(des_work_norm.norm_wing==0)
			strcpy(sText,"X肢法线");
		else if(des_work_norm.norm_wing==1)
			strcpy(sText,"Y肢法线"); 
#endif
	}
	else if(GetPropID("des_work_norm.reside_plane")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_work_norm.reside_plane==0)
			strcpy(sText,"Datum Plane");
		else if(des_work_norm.reside_plane==1)
			strcpy(sText,"Bending plane"); 
		else if(des_work_norm.reside_plane==2)
			strcpy(sText,"2nd bending plane"); 
#else 
		if(des_work_norm.reside_plane==0)
			strcpy(sText,"基准平面");
		else if(des_work_norm.reside_plane==1)
			strcpy(sText,"火曲面"); 
		else if(des_work_norm.reside_plane==2)
			strcpy(sText,"第二火曲面"); 
#endif
	}
	else if(GetPropID("des_work_norm.direction")==id)
		sprintf(sText,"%s",des_work_norm.GetDirectionDes());
	else if(GetPropID("des_work_norm.hCrossPart")==id)
		sprintf(sText,"0X%X",des_work_norm.hCrossPart);
	else if(GetPropID("des_work_norm.nearVector.x")==id)
	{
		sprintf(sText,"%f",des_work_norm.nearVector.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_work_norm.nearVector.y")==id)
	{
		sprintf(sText,"%f",des_work_norm.nearVector.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_work_norm.nearVector.z")==id)
	{
		sprintf(sText,"%f",des_work_norm.nearVector.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_work_norm.rot_ang")==id)
	{
		sprintf(sText,"%f",des_work_norm.rot_ang);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("work_norm.x")==id)
	{
		sprintf(sText,"%f",get_norm().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("work_norm.y")==id)
	{
		sprintf(sText,"%f",get_norm().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("work_norm.z")==id)
	{
		sprintf(sText,"%f",get_norm().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("relativeObjs")==id)
	{
		for(RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
		{
			if(strlen(sText)==0)
				_snprintf(sText,99,"0X%X",pRelaObj->hObj);
			else
				_snprintf(sText,99,"%s,0X%X",sText,pRelaObj->hObj);
		}
	}
	else if(GetPropID("shadowInfo")==0)
		strcpy(sText,"");
	else if(GetPropID("m_hMotherObj")==id)
	{
		CLDSBolt *pMotherBolt=(CLDSBolt*)MotherObject(false);
		if(pMotherBolt!=this)
			sprintf(sText,"0X%X",pMotherBolt->handle);
		else 
			strcpy(sText,"0X0");
	}
	else if(GetPropID("m_hBlockRef")==id)
	{
		CBlockParameter blockPara;
		if (GetBlockParameter(blockPara))
			sprintf(sText,"0X%X",blockPara.m_pBlockRef->handle);
		else 
			strcpy(sText,"0X0");
	}
	else if(GetPropID("m_hBlock")==id)
		strcpy(sText,GetBlockIdName());
#ifdef __LDS_CONTEXT_
	else if(pRelaHole&&pRelaHole->xQueueNode.GetPropValueStr(id,sText)>0)
	{
		if(valueStr)
			StrCopy(valueStr,sText,nMaxStrBufLen-1);
		return strlen(sText);
	}
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	if(strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("0X%X Bolt's property##d string value out of length",handle,id);
#else
	if(strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("螺栓0X%X属性#%d字符串超出长度限制",handle,id);
#endif
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen-1);
	return strlen(sText);
}
BOOL CLDSBolt::IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSBolt *pBolt=NULL, *pFirstBolt = (CLDSBolt*)selectObjs.GetFirst();
	pFirstBolt->GetPropValueStr(idProp,valueStr);
	for(pBolt=(CLDSBolt*)selectObjs.GetNext();pBolt;pBolt=(CLDSBolt*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pBolt->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

BOOL CLDSBolt::GetObjPropReadOnlyDefaultState(long id)
{
	return GetObjPropReadOnlyDefaultState(id,ShadowMode());
}
BOOL CLDSBolt::GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode)
{
	if (shadow_mode==CLDSObject::SHADOWMODE_COMMON)
	{
		if(GetPropID("layer")==id)
			return FALSE;
		else if(GetPropID("L0")==id)
			return FALSE;
		else if(GetPropID("statSegStr")==id)
			return FALSE;
		else if(GetPropID("cfgword")==id)
			return FALSE;
		else if(GetPropID("dwRayNo")==id)
			return FALSE;
		else if(GetPropID("m_uStatMatCoef")==id)
			return FALSE;
		else if(GetPropID("gasketInfo")==id)
			return FALSE;
		else if(GetPropID("gasketThick")==id)
			return FALSE;
		else if(GetPropID("gasketN")==id)
			return FALSE;
		else if(GetPropID("gasketOffset")==id)
			return FALSE;
		else if(GetPropID("relativeObjs")==id)
			return FALSE;
		else if(GetPropID("sNotes")==id)
			return FALSE;
		else
			return TRUE;
	}
	else if(shadow_mode==CLDSObject::SHADOWMODE_BLOCK)
	{
		if(GetPropID("layer")==id)
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if(GetPropID("handle")==id)
			return TRUE;
		else
			return FALSE;
	}
}

CLsRef CLDSBolt::GetLsRef()
{
	CLsRef ls_ref;
	ls_ref = this;
	return ls_ref;
}
int CLDSBolt::GetSpecification(char *spec,BOOL bIncMat/*=TRUE*/)
{
	CLDSBolt* pMotherBolt=(CLDSBolt*)MotherPart();
	if(pMotherBolt!=this)
		return pMotherBolt->GetSpecification(spec,bIncMat);
	char steelmark[20]="";
	char tmp_spec[200];
	if(bIncMat)
		_snprintf(tmp_spec,199,"%sM%dX%d",Grade(),d,_L);
	else
		_snprintf(tmp_spec,199,"M%dX%d",d,_L);
	//_L已为整数，如果再执行SimplifiedNumString会导致6.8M16x40->6.8M16x4 wjh-2019.9.15
	//SimplifiedNumString(tmp_spec);
	if(spec)
		strcpy(spec,tmp_spec);
	return strlen(tmp_spec);
}
f3dPoint CLDSBolt::set_norm(const double* norm)
{
	GEPOINT normal(norm);
	if(normalize(normal))
	{
		ucs.axis_z = normal;
		is_data_modified = TRUE;
	}
	SetModified();
	return ucs.axis_z;
}

f3dPoint CLDSBolt::get_norm() const
{	return ucs.axis_z;	}

/*void CLDSBolt::set_pos(f3dPoint ls_pos)
{
	SetModified();
	ucs.origin	= ls_pos;
}*/
//螺栓是否以构件上相对坐标为基准定位
BOOL CLDSBolt::IsLocalCoordLocation(CLDSPart* pDatumPart/*=NULL*/) const
{
	CSuperSmartPtr<CLDSPart> pDatumPlate=pDatumPart;
	if(pDatumPart==NULL)
		pDatumPlate=BelongModel()->FromPartHandle(des_base_pos.hPart);
	else if(pDatumPart->handle!=des_base_pos.hPart)
		return FALSE;
	if(pDatumPlate.IsNULL()||!pDatumPlate->IsPlate())
		return FALSE;
	if(des_base_pos.R<=0&&des_base_pos.cLocationStyle>0)
		return TRUE;//非环向布置螺栓
	return FALSE;
}
BOOL CLDSBolt::IsCanLocalCoordLocation(CLDSPart* pDatumPart/*=NULL*/) const
{
	CSuperSmartPtr<CLDSPart> pDatumPlate=pDatumPart;
	if(pDatumPart==NULL)
		pDatumPlate=BelongModel()->FromPartHandle(des_base_pos.hPart);
	else if(pDatumPart->handle!=des_base_pos.hPart)
		return FALSE;
	if(pDatumPlate.IsNULL()||!pDatumPlate->IsPlate())
		return FALSE;
	if(des_base_pos.R<=0)
		return TRUE;//非环向布置螺栓
	return FALSE;
}
BOOL CLDSBolt::correct_pos()
{
	if(IsShadowObject())
	{
		ShadowDesign();
	}
	else
	{
		CSuperSmartPtr<CLDSPart> pDatumPart=BelongModel()->FromPartHandle(des_base_pos.hPart);
		if(pDatumPart.IsNULL())
		{
			if(des_base_pos.datumPoint.datum_pos_style>0)
			{
				des_base_pos.datumPoint.UpdatePos(BelongModel());
				ucs.origin=des_base_pos.datumPoint.Position();
				SetModified();
				return TRUE;
			}
			return FALSE;
		}
		f3dPoint worknorm=ucs.axis_z;
		if(m_dwFlag.IsHasFlag(CLDSBolt::REVERSE_NORM))
			worknorm*=-1.0;

		if(des_base_pos.datumPoint.datum_pos_style>0)
			des_base_pos.datumPoint.UpdatePos(BelongModel());
		double z_level=0;
		if(pDatumPart->IsPlate())
			z_level = pDatumPart.GeneralPlatePointer()->GetNormOffset();
		CLDSLineAngle agentAngle;
		if(pDatumPart->IsArcPart())
			pDatumPart=agentAngle.Fabricate(pDatumPart,des_base_pos.datumPoint.Position());
		if(pDatumPart->IsLinePart())
		{	//以杆件为定位基准构件
			f3dPoint wing_vec=pDatumPart->ucs.axis_x,len_vec;
			if(des_base_pos.direction==0)
				len_vec=pDatumPart.LinePartPointer()->End()-pDatumPart.LinePartPointer()->Start();
			else //if(des_base_pos.direction==1)
				len_vec=pDatumPart.LinePartPointer()->Start()-pDatumPart.LinePartPointer()->End();
			normalize(len_vec);
			if(pDatumPart->IsAngle())
			{	//角钢为基准杆件
				if(des_base_pos.offset_wing==0)
					wing_vec=pDatumPart.LineAnglePointer()->GetWingVecX();
				else //if(des_base_pos.offset_wing==1)
					wing_vec=pDatumPart.LineAnglePointer()->GetWingVecY();
			}
			else if(pDatumPart->GetClassTypeId()==CLS_LINEFLAT)
				wing_vec=pDatumPart->ucs.axis_y;
			else if (pDatumPart->GetClassTypeId() == CLS_LINESLOT)
			{
				if (des_base_pos.offset_wing == 0)
					wing_vec = pDatumPart->ucs.axis_x;
				else //if (des_base_pos.offset_wing > 0)		//X+肢|X-肢
					wing_vec = pDatumPart->ucs.axis_y;
			}
			f3dPoint base_pos;
			if(des_base_pos.cLocationStyle)
			{	//基点坐标为相对坐标,此时需要转换为绝对坐标
				if(des_base_pos.direction==0)
					base_pos=pDatumPart.LinePartPointer()->Start()-len_vec*pDatumPart.LinePartPointer()->startOdd();
				else
					base_pos=pDatumPart.LinePartPointer()->End()-len_vec*pDatumPart.LinePartPointer()->endOdd();
				base_pos+=len_vec*des_base_pos.datumPoint.xRsltPosition.x+wing_vec*des_base_pos.datumPoint.xRsltPosition.y;
				//此处之前好像有笔误 wjh-2019.8.26
				base_pos+=worknorm*des_base_pos.datumPoint.xRsltPosition.z;
			}
			else
				base_pos=des_base_pos.datumPoint.Position();
			if(pDatumPart->IsAngle()&&(des_base_pos.datumPoint.datum_pos_style==1||des_base_pos.datumPoint.datum_pos_style==2))
			{
				JGZJ jgzj=des_base_pos.offset_wing==0?pDatumPart.LineAnglePointer()->GetZhunJu('X'):pDatumPart.LineAnglePointer()->GetZhunJu('Y');
				if(des_base_pos.datumPoint.datum_pos_style==2&&pDatumPart->handle==des_base_pos.datumPoint.m_hDatumRod)
				{	//之前未处理节点特殊准距情况,极易导致角钢端头基点位置与螺栓不一致,问题很隐含 wjh-2017.11.23
					CLDSNode* pDatumNode=BelongModel()->FromNodeHandle(des_base_pos.datumPoint.m_hNode);
					jgzj.g=ftoi(des_base_pos.offset_wing==0?pDatumPart.pAngle->GetNodeWingXG(pDatumNode):pDatumPart.pAngle->GetNodeWingYG(pDatumNode));
				}
				if(des_base_pos.m_biWingOffsetStyle==0)
					des_base_pos.wing_offset_dist=jgzj.g;
				else if(des_base_pos.m_biWingOffsetStyle==1)
					des_base_pos.wing_offset_dist=jgzj.g1;
				else if(des_base_pos.m_biWingOffsetStyle==2)
					des_base_pos.wing_offset_dist=jgzj.g2;
				else if(des_base_pos.m_biWingOffsetStyle==3)
					des_base_pos.wing_offset_dist=jgzj.g3;
			}
			ucs.origin=base_pos+wing_vec*des_base_pos.wing_offset_dist+len_vec*des_base_pos.len_offset_dist;
			ucs.origin+=worknorm*des_base_pos.norm_offset.Thick(BelongModel());
		}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else
		{	//以除角钢外的其他构件为基准构件布置螺栓
			if(des_base_pos.R<=0)
			{	//非环向布置螺栓
				if(des_base_pos.cLocationStyle)
				{	//基点坐标为相对坐标,此时需要转换为绝对坐标
					//ucs.origin=des_base_pos.datumPoint.Position(); //多余代码，注释掉 wjh-2016.3.25
					ucs.origin.x = des_base_pos.len_offset_dist;
					ucs.origin.y = des_base_pos.wing_offset_dist;
					ucs.origin.z = 0;//des_base_pos.norm_offset.Thick();
					bool bSameNormalToDatumPart=worknorm*pDatumPart->ucs.axis_z>0;
					if(pDatumPart->GetClassTypeId()==CLS_PARAMPLATE&&
						pDatumPart.ParamPlatePointer()->m_iParamType==TYPE_XEND&&
						fabs(worknorm*pDatumPart->ucs.axis_y)>EPS_COS2)
					{	//十字插板上与主板垂直的板上的螺栓
						ucs.origin.y = 0;
						ucs.origin.z = des_base_pos.wing_offset_dist;
					}
					if( pDatumPart->GetClassTypeId()==CLS_PLATE&&	//位于火曲面上的螺栓
						des_base_pos.reside_plane>0&&des_base_pos.reside_plane<pDatumPart.PlatePointer()->face_N)
						ucs.origin=pDatumPart.PlatePointer()->RestoreLsPos(ucs.origin,des_base_pos.reside_plane);
					else
					{
						if(fabs(z_level)>1)
							ucs.origin.z=bSameNormalToDatumPart?z_level:-z_level;
						coord_trans(ucs.origin,pDatumPart->ucs,TRUE);
					}
				}
				else
				{
					ucs.origin=des_base_pos.datumPoint.Position();
					ucs.origin+=pDatumPart->ucs.axis_x*des_base_pos.len_offset_dist;
					if(pDatumPart->GetClassTypeId()!=CLS_PARAMPLATE||pDatumPart.ParamPlatePointer()->m_iParamType!=TYPE_XEND)
						ucs.origin+=pDatumPart->ucs.axis_y*des_base_pos.wing_offset_dist;
					else if(pDatumPart->GetClassTypeId()==CLS_PARAMPLATE&&
						pDatumPart.ParamPlatePointer()->m_iParamType==TYPE_XEND&&
						fabs(worknorm*pDatumPart->ucs.axis_y)>EPS_COS2) //十字插板上与主板垂直的板上的螺栓
						ucs.origin+=pDatumPart->ucs.axis_y*des_base_pos.wing_offset_dist;
					else
						ucs.origin+=pDatumPart->ucs.axis_z*des_base_pos.wing_offset_dist;
					//ucs.origin+=pDatumPart->ucs.axis_z*des_base_pos.norm_offset.Thick();
				}
				ucs.origin+=worknorm*des_base_pos.norm_offset.Thick(BelongModel());
			}
			else if(pDatumPart->IsArcPart())
			{	//环形构件螺栓  wht 09-12-15
				f3dPoint ray_vec=pDatumPart->ucs.axis_x;	//螺栓所在直线初始方向
				//计算环形构件坐标系时已考虑了start_rot_angle,此处不用再考虑 wht 15-03-19
				double offset_angle=0;//pDatumPart.ArcPartPointer()->start_rot_angle;
				offset_angle+=des_base_pos.offset_angle;
				RotateVectorAroundVector(ray_vec,offset_angle,pDatumPart->ucs.axis_z);
				ucs.origin=pDatumPart->ucs.origin+pDatumPart->ucs.axis_z*(des_base_pos.wing_offset_dist+pDatumPart.ArcPartPointer()->offset_v)
					+ray_vec*(des_base_pos.R+pDatumPart.ArcPartPointer()->offset_h+des_base_pos.norm_offset.Thick(BelongModel()));				
			}
			else 
			{	//环向布置螺栓
				CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(pDatumPart->m_hPartWeldParent,CLS_LINETUBE);
				if(!pDatumTube||pDatumTube->GetClassTypeId()!=CLS_LINETUBE)
					return FALSE;
				//angle为环向布置螺栓的基准边偏移角度，该角度相对于钢管的基准边来计算。 wht 10-10-14
				double angle=0;//des_base_pos.offset_angle;
				if(pDatumPart->GetClassTypeId()==CLS_PLATE)
				{
					if(pDatumTube->m_bHasWeldRoad&&pDatumPart.PlatePointer()->m_bAttachPartByWeldRoad)//以焊道线为基准布置螺栓
						angle += pDatumTube->CalWeldRoadAngle();
					//在修改布置螺栓的基准后应该重新设基准构件的坐标系
					pDatumPart.PlatePointer()->DesignSetupUcs();
				}
				else if(pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
				{
					long J=0;
					pDatumPart.ParamPlatePointer()->GetDesignItemValue('J',&J);
					//为统一肘板、环向肋板及环向螺栓转角定位, 经王金河 王海涛 王晓川三人确认
					//以后统一不再考虑法兰X轴的初始偏转角 wjh-2015.8.05
					//double D=0;
					//if(pDatumPart.ParamPlatePointer()->m_bStdPart)
					//	pDatumPart.ParamPlatePointer()->GetDesignItemValue('X',&D);
					//else
					//	pDatumPart.ParamPlatePointer()->GetDesignItemValue('D',&D);
					//D=D*RADTODEG_COEF;
					//螺栓偏移角度是在法兰坐标系下的相对偏移角度	wht 10-01-29
					//angle+=D;
					if(J!=0)
						angle += pDatumTube->CalWeldRoadAngle();
					//在修改布置螺栓的基准后应该重新设基准构件的坐标系(布置肘板时不用更新坐标系)
					if(pDatumTube->m_tJointStart.hLinkObj==pDatumPart->handle)
						pDatumTube->DesignStartJointUCS(pDatumPart.ParamPlatePointer());
					else if(pDatumTube->m_tJointEnd.hLinkObj==pDatumPart->handle)
						pDatumTube->DesignEndJointUCS(pDatumPart.ParamPlatePointer());
				}
				f3dPoint axis_x(cos(angle),sin(angle),0);	//螺栓所在直线的初始方向
				normalize(axis_x);
				f3dPoint axis_z(-axis_x.y,axis_x.x);
				normalize(axis_z);
				pDatumTube->BuildUCS();
				//此处应使用基准钢板的坐标系而不是基准钢管的坐标系，	//wht 10-09-29
				//基准钢板与钢管垂直时使用两坐标系效果相同故以前未发现该问题
				//此处偏移角度是相对于基准钢管X轴的偏移角度，故进行坐标系转换时应使用钢管坐标系 wht 10-10-14
				vector_trans(axis_z,pDatumTube->ucs,TRUE);	//转换到绝对坐标系下
				vector_trans(axis_x,pDatumTube->ucs,TRUE);	//将螺栓所在直线的初始方向转换到绝对坐标系下
				f3dPoint inters_vec=axis_z^pDatumPart->ucs.axis_z;	//螺栓所在直线的最终方向
				normalize(inters_vec);
				normalize(axis_x);
				double cosa=axis_x*inters_vec;					//计算初始方向及最终方向间的余弦值
				if(cosa<0)//修正最终方向的正负
					axis_x=-inters_vec;
				else
					axis_x=inters_vec;	
				//计算螺栓位置
				//-------------------------
				//优化环向布置螺栓位置计算方法 wht 10-01-14
				//旧:先计算出螺栓偏移角度，根据角度计算出螺栓所在直线的
				//   方向(计算方向时坐标转换有问题),然后用钢管与钢板求交得到基准点
				//新:螺栓偏移角度基准边方向应在基准钢管坐标系进行计算，但是螺栓偏移角度应该在钢板坐标系下计算
				//   先计算出螺栓基准边的方向然后绕钢板Z轴旋转得到螺栓位置
				//-------------------------
				f3dPoint pos;
				f3dPoint face_norm=pDatumPart->ucs.axis_z;
				f3dPoint face_pick=pDatumPart->ucs.origin+pDatumPart->ucs.axis_z*z_level;
				//钢管与与钢板XY面的交点
				//Int3dlf(&center_pos,f3dLine(pDatumTube->Start(),pDatumTube->End()),face_pick,face_norm);
				pos=face_pick+axis_x*des_base_pos.R;
				if(fabs(des_base_pos.offset_angle)>0)
					rotate_point_around_axis(pos,des_base_pos.offset_angle,face_pick,face_pick+face_norm*1000);
				ucs.origin=pos;
				ucs.origin+=ucs.axis_z*des_base_pos.norm_offset.Thick(BelongModel());
			}
		}
#endif
		if(m_dwFlag.IsHasFlag(CLDSBolt::REVERSE_NORM))
		{	//螺栓朝向变反需修正螺栓位置，首先还原到原来的位置，然后沿法线偏移一个通厚
			//ucs.origin-=ucs.axis_z*(2*des_base_pos.norm_offset.Thick(BelongModel()));
			ucs.origin-=ucs.axis_z*get_L0();
			if(DianQuan.offset>0&&DianQuan.N>0)	//存在垫圈时，需要调整垫圈位置
				DianQuan.offset=get_L0()-fabs(DianQuan.offset)-DianQuan.thick*DianQuan.N;
		}
	}
	SetModified();
	return TRUE;
}
void CLDSBolt::correct_worknorm()
{
	if(IsShadowObject())
	{
		ShadowDesign();
	}
	else
	{
		// TODO: 这里的10是哪来的,如何存储?wjh2012-8-5
		if(feature==10)	//十字插板上与主板垂直的板上的螺栓
			des_work_norm.norm_wing=10;
		if(des_work_norm.UpdateVector(BelongModel()))
		{
			ucs.axis_z=des_work_norm.vector;
			SetModified();
		}
		//基准构件为环向构件时在此根据螺栓位置计算法线方向	wht 09-12-15
		CLDSArcPart *pArcPart=(CLDSArcPart*)BelongModel()->FromPartHandle(des_work_norm.hVicePart,CLS_ARCPART);
		if(pArcPart)
		{	
			correct_pos();	//首先更新螺栓位置
			GEPOINT axis_z=ucs.origin-f3dPoint(pArcPart->center);
			project_vector(axis_z,pArcPart->worknorm);
			normalize(axis_z);
			ucs.axis_z=(ucs.axis_z*axis_z<0)?-axis_z:axis_z;
				
		}
	}
}
bool CLDSBolt::ShadowDesign()
{
	if(!IsShadowObject())
		return false;
	CLDSBolt *pMotherBolt=(CLDSBolt*)MotherObject(false);
	//基本信息的映射
	d=pMotherBolt->get_d();
	SetGrade(pMotherBolt->Grade());
	strncpy(statSegStr,pMotherBolt->statSegStr,23);
	hole_d_increment=pMotherBolt->hole_d_increment;
	m_dwFlag=pMotherBolt->m_dwFlag;
	//cfgword=pMotherBolt->cfgword;

	iSeg = pMotherBolt->iSeg;
	m_hFamily=pMotherBolt->m_hFamily;
	DianQuan = pMotherBolt->DianQuan;
	m_bVirtualPart=pMotherBolt->m_bVirtualPart;
	if(m_iShadowObjType==SHADOW_ISOLATED)
	{
		CParaCS *pAcs=m_pModel->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs);
		if(pAcs==NULL)
			return false;
		pAcs->SetModifiedState();
		if(pAcs->GetDefStyle()==CParaCS::CS_PARAM)
			pAcs->UpdateACS(BelongModel());
		else
			pAcs->UpdateACS(pMotherBolt);
		ucs =UCS_STRU(pAcs->GetACS());
		ucs.origin+=ucs.axis_z*des_base_pos.norm_offset.Thick(BelongModel());
		//L=pMotherBolt->get_L();
		//L0 = get_L0(TRUE);	//孤立影射螺栓不一定与母螺栓一致
		CalGuigeAuto(TRUE);
		return true;
	}
	else if(m_iShadowObjType==SHADOW_EMBEDDED||m_iShadowObjType==SHADOW_EXTERBLK)
	{
		CBlockReference *pBlockRef=NULL;
		if(m_pModel->IsTowerModel())
			pBlockRef=((CTower*)m_pModel)->BlockRef.FromHandle(shadow.BLOCK_OBJ.m_hBlockRef);
		if(pBlockRef==NULL)
			return false;
		ucs=pMotherBolt->ucs;
		CBlockParameter blockPara;
		if(!GetBlockParameter(blockPara))
			return false;
		UCS_STRU cs=blockPara.m_pBlock->TransToACS(blockPara.m_pBlockRef->GetACS());
		coord_trans(ucs.origin,cs,TRUE);
		vector_trans(ucs.axis_x,cs,TRUE);
		vector_trans(ucs.axis_y,cs,TRUE);
		vector_trans(ucs.axis_z,cs,TRUE);
		L =pMotherBolt->get_L();
		_L0 =pMotherBolt->get_L0();	//孤立影射螺栓不一定与母螺栓一致
	}
	else if(m_iShadowObjType==SHADOW_ISOLATED_RELA_LOCATION)
	{
		CLDSPart *pRelationPart=((CTower*)m_pModel)->FromPartHandle(shadow.COMMON_OBJ.m_hAcs);
		CLDSPart *pRelaMotherPart=NULL;
		CLsRefList *pLsRefList=NULL;
		if(pRelationPart!=NULL)
		{
			pRelaMotherPart=pRelationPart->MotherPart();
			if(pRelaMotherPart)
			{
				pLsRefList=pRelaMotherPart->GetLsRefList();
				pMotherBolt=(CLDSBolt*)MotherObject();
			}
		}
		if(pLsRefList&&pLsRefList->FromHandle(pMotherBolt->handle))
		{
			ucs=pMotherBolt->ucs;
			coord_trans(ucs.origin,pRelaMotherPart->ucs,FALSE);
			coord_trans(ucs.origin,pRelationPart->ucs,TRUE);
			vector_trans(ucs.axis_x,pRelaMotherPart->ucs,FALSE);
			vector_trans(ucs.axis_x,pRelationPart->ucs,TRUE);
			vector_trans(ucs.axis_y,pRelaMotherPart->ucs,FALSE);
			vector_trans(ucs.axis_y,pRelationPart->ucs,TRUE);
			vector_trans(ucs.axis_z,pRelaMotherPart->ucs,FALSE);
			vector_trans(ucs.axis_z,pRelationPart->ucs,TRUE);
			CalGuigeAuto(TRUE);
			if(_L0==0)
			{
				_L0 =pMotherBolt->get_L0();
				L =pMotherBolt->get_L();
			}
		}
	}
	else
		return false;
	SetModified();
	return true;
}

int CLDSBolt::set_d(int ls_d)
{
	if(ls_d>0)
	{
		_d = ls_d;
		is_data_modified = TRUE;
	}
	CLDSBolt *pBolt=(CLDSBolt*)MotherPart();
	if(pBolt!=this)
		pBolt->set_d(_d);
	SetModified();
	return _d;
}

int CLDSBolt::get_d()
{	
	CLDSBolt *pBolt=(CLDSBolt*)MotherPart();
	if(pBolt==this)
		return _d;
	else
		return pBolt->get_d();
}

double CLDSBolt::set_L(double ls_L)
{	
	if(ls_L>eps)
	{
		_L = (short)ls_L;
		is_data_modified = TRUE;
	}
	SetModified();
	return _L;
}

double CLDSBolt::get_L()
{	return _L;}

double CLDSBolt::add_L(double addL)	//通厚增加值
{
	_L+=ftoi(addL);
	SetModified();
	return _L;
}
double CLDSBolt::get_dfLe()
{
	if (!IsFootNail())
		return _L;
	double D, H = 0, H2 = 0;
	FindLsD_H(d, &D, &H, &H2);
	CLsFamily *pLsFamily = CLsLibrary::FromHandle(m_hFamily);
	if (pLsFamily == NULL)
		pLsFamily = CLsLibrary::DefaultFamily();
	LS_XING_HAO *pLsXingHao = pLsFamily->FindLsGuiGeByValidLen(d, _L);
	return (pLsXingHao == NULL) ? L : L0 + H2 + pLsXingHao->no_thread_len;
}

long CLDSBolt::set_oldL(long ls_L)
{
	return _siInitStdL = (short)ls_L;
}

long CLDSBolt::get_oldL() const
{	return _siInitStdL;	}
bool CLDSBolt::restore_oldL()
{
	if (_siInitStdL<=0||!IsFootNail())
		return false;
	_hFamily=1;	//将来应存储旧的螺栓系列号，并在此恢复为旧系列号 wjh 2011.11.25
	_L = _siInitStdL;		//L赋值应在m_hFamily之后，因为m_hFamily赋值时会重新计算螺栓规格导致无法用L判断是否应删除脚钉螺栓 wht 13-04-02
	SetModified(TRUE);	//is_data_modified = TRUE;不包含Undo操作及文档修改标识
	return true;
}
bool CLDSBolt::PushInitialStdBoltL()
{	//由普通螺栓升级为代孔脚钉时，存档当前初始标准螺栓有效长度 wjh-2019.8.21
	if (_siInitStdL==0&&!IsFootNail())
		return (_siInitStdL = _L)>0;
	else
		return false;
}
bool CLDSBolt::PopInitialStdBoltL()
{
	return restore_oldL();
}

CParaThick *CLDSBolt::GetL0ThickPara()
{
	return &designL0;
}

//清空原有螺栓通厚参数,直接设置螺栓通厚
/*double CLDSBolt::set_L0(double ls_L0)
{
	designL0.EmptyThick();
	L0=ls_L0;
	return L0;
}*/

void CLDSBolt::EmptyL0DesignPara()	//清空原有螺栓通厚参数
{
	m_bNeedUpdateL0=true;
	designL0.EmptyThick();
}

//在原有通厚基础上，增加螺栓通厚
void CLDSBolt::AddL0Thick(long val,BOOL bHandle/*=FALSE*/,BOOL bSamePartFilter/*=FALSE**/)
{
	m_bNeedUpdateL0= true;
	designL0.AddThick(val,bHandle,bSamePartFilter);
}

void CLDSBolt::GetL0KeyStr(char *key_str, UINT nMaxStrBuffLen/* = 200*/)//得到通厚字符串
{
	CLDSBolt* pMotherBolt = (CLDSBolt*)MotherPart();
	if (pMotherBolt != this)
		return pMotherBolt->GetL0KeyStr(key_str);
	if (key_str)
		StrCopy(key_str, designL0.ToThickStr(), nMaxStrBuffLen);
}
void CLDSBolt::SetL0KeyStr(const char *key_str)//设置通厚字符串
{
	m_bNeedUpdateL0= true;
	designL0.BuildFromStr(key_str);
	SetModified(TRUE,FALSE);
}
void CLDSBolt::SetL0ThickPara(CParaThick thick)//设置通厚字符串
{
	designL0=thick;
}
struct THICKPART
{
	double zBottom;
	double thick;
	CLDSPart* pPart;
	THICKPART(CLDSPart* _pPart=NULL,double _zBtm=0,double _thick=0)
	{
		pPart=_pPart;
		thick=_thick;
		zBottom=_zBtm;
	}
};
int _LocalCompareThickPart(const THICKPART& thickitem1,const THICKPART& thickitem2)
{
	if(thickitem1.zBottom+thickitem1.thick/2>thickitem2.zBottom+thickitem2.thick/2)
		return 1;
	else if(thickitem1.zBottom+thickitem1.thick/2<thickitem2.zBottom+thickitem2.thick/2)
		return -1;
	else
		return 0;
}
short CLDSBolt::CalL0()
{
	long thick=0;
	CLDSModule* pModule=NULL;
	if(m_pModel&&m_pModel->IsTowerModel())
		pModule=((CTower*)m_pModel)->GetActiveModule();
	int i=0;
	THICK_ITEM item;
	CLDSPart* pPriorThickPart=NULL,*pPart=NULL;
	bool hasPriorPartThick=false;	//是否具有不受呼高限制的构件通厚项
	THICKPART _arrThickParts[24];
	CLDSPartPtr _arrRelaThickPartObjs[24] = { NULL };
	CFGWORD xCurrCfgWord;
	ARRAY_LIST<THICKPART> xarrThickParts(_arrThickParts, 24);
	ARRAY_LIST<CLDSPartPtr> xarrRelaThickPartObjs(_arrRelaThickPartObjs, 24);
	//1.提取关联构件对象，并检查是否存在优先构件
	for(item=designL0.GetFirstItem();!item.IsFinalKey();item=designL0.GetNextItem(),i++)
	{
		xarrRelaThickPartObjs.append(NULL);
		if (item.cItemType == THICK_ITEM::PART_THROUGH&& (pPart = m_pModel->FromPartHandle(abs(item.val))) != NULL)
			xarrRelaThickPartObjs[i] = pPart;
		else if(item.cItemType==THICK_ITEM::PART_THROUGHnT&&(pPart=m_pModel->FromPartHandle(abs(item.val)))!=NULL)
		{
			xarrRelaThickPartObjs[i] = pPart;
			if (!hasPriorPartThick)
			{
				hasPriorPartThick = true;
				xCurrCfgWord = pPart->cfgword;
				pPriorThickPart = pPart;
				continue;
			}
			CFGWORD cfgword = xCurrCfgWord & pPart->cfgword;
			if (!cfgword.IsEqual(xCurrCfgWord) && cfgword.IsEqual(pPart->cfgword))
			{	//优先构件配材号范围缩小时，以优先构件中最小配材号范围作为通厚配材号计算基准 wjh-2019.7.12
				xCurrCfgWord = pPart->cfgword;
				pPriorThickPart = pPart;
				continue;
			}
		}
	}
	//2.锁定一组有效共存构件的共存配材号xCurrCfgWord
	for (i = 0; i < xarrRelaThickPartObjs.Count; i++)
	{
		if (xarrRelaThickPartObjs[i] != NULL)
			pPart = xarrRelaThickPartObjs[i];
		else
			continue;
		if (xCurrCfgWord.IsNull())
			xCurrCfgWord = pPart->UnifiedCfgword();
		else if (xCurrCfgWord.And(pPart->UnifiedCfgword()))
			xCurrCfgWord &= pPart->UnifiedCfgword();
		else //与统计的基准穿越构件不共存
			continue;
	}
	bool blNowCalL0IncGapByInters = false;
	if( BOLTL0_CALMODE==CLDSBolt::L0CAL_INC_GAP_THICK||
		(this->m_bCalL0IncGapByInters&&BOLTL0_CALMODE==CLDSBolt::L0CAL_BY_BOLTSELF))
		blNowCalL0IncGapByInters =true;
	bool blEnableHeightFilter = hasPriorPartThick && m_bEnableActiveHeightFilterOnL0InternalCal;
	double zBottom=0,zTop=0;	//贯穿构件底面（靠近螺栓头一侧）和顶面（螺母侧）轴向位置
	GEPOINT vWorkNormal=ucs.axis_z;
	//3.常规统计有效穿过构件的集合及通厚累加值
	for(item=designL0.GetFirstItem(),i=0;!item.IsFinalKey();item=designL0.GetNextItem(),i++)
	{
		//THICK_ITEM item(data+i);
		int sign=1;
		if(item.val<0)
			sign=-1;
		if(m_pModel!=NULL && item.IsHandle())	//句柄
		{
			long t=0;
			if ((pPart=xarrRelaThickPartObjs[i]) == NULL)
				continue;
			//3.1统计筛选当前的有效通厚计算穿过构件
			if (hasPriorPartThick&&pPriorThickPart)
			{ //存在优先通厚统计构件
				if(item.cItemType != THICK_ITEM::PART_THROUGHnT && !pPriorThickPart->ModuleTogetherWith(pPart))
					continue;	//跳过与优先统计构件不共存构件
			}
			//通过调用m_bEnableActiveHeightFilterOnL0InternalCal，可以允许在某些特定场景（快速自动建模时）关掉呼高筛选 wjh-2019.6.14
			else if (blEnableHeightFilter&&pModule)
			{	//启用当前呼高过滤
				if (!pModule->IsSonPart(pPart))// && item.cItemType == THICK_ITEM::PART_THROUGH)
					continue;	//非当前呼高的有效构件
			}
			else if(!xCurrCfgWord.And(pPart->UnifiedCfgword()))
				continue;
			//3.2统计有效穿越构件的穿过中心点清单
			if(blNowCalL0IncGapByInters)
			{
				GEPOINT location;
				THICKPART thickpart(pPart, 0, pPart->GetThick());
				int wingx0_y1 = 0;
				if (pPart->GetBoltIntersPos(this, location, &wingx0_y1, CLDSPart::BOLTBTM_PLANE))
				{	//统计螺栓与构件的穿过点及相应厚度
					thickpart.zBottom = (location - GEPOINT(ucs.origin))*ucs.axis_z;
					if (thickpart.zBottom > 200 || thickpart.zBottom < -50)
						Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X螺栓通厚计算错误，可能原因是螺栓位置{z=%.0f}错误", handle,thickpart.zBottom);
					xarrThickParts.append(thickpart);
				}
			}
			//3.3按传统方式累加厚度值
#ifdef __COMMON_PART_INC_
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pParamPlate = ((CLDSParamPlate*)pPart); 
				if(pParamPlate->m_iParamType==TYPE_UEND)
					t = ftol(pParamPlate->GetThick())*2;
				else 
					t = ftol(pParamPlate->GetThick());	
			}
			else if(pPart->IsArcPart()||pPart->IsLinePart()||pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE)
#endif
				t=ftol(pPart->GetThick());
			thick+=t*sign;
		}
		else	//指定厚度值
			thick+=item.val;
	}
	_L0 =(short)thick;
	//4.计算螺栓通厚时考虑螺栓垫圈厚度 wht 11-01-10
	if(DianQuan.N>0&&DianQuan.thick)
		_L0 +=DianQuan.N*DianQuan.thick;
	for(item=designL0.GetFirstItem();!item.IsFinalKey();item=designL0.GetNextItem())
	{
		if((!hasPriorPartThick&&item.cItemType==THICK_ITEM::PART_THROUGH)||
			item.cItemType==THICK_ITEM::PART_THROUGHnT)
		{
			BOOL pushed=BelongModel()->PushPartStack();
			CLDSLineAngle* pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(item.val,CLS_LINEANGLE);
			BelongModel()->PopPartStack(pushed);
			if(pAngle==NULL)
				continue;
			GEPOINT localpos;
			if( pAngle->start_push_flat_x1_y2==1||pAngle->start_push_flat_x1_y2==2||
				pAngle->end_push_flat_x1_y2==1||pAngle->end_push_flat_x1_y2==2||
				pAngle->mid_push_flat_x1_y2==1||pAngle->mid_push_flat_x1_y2==2)
			{
				if(!pAngle->GetBoltLocalPos(this,localpos,true))
					continue;
			}
			else
				continue;
			double length=pAngle->GetLength();
			double dfOldL0=_L0;
			if( localpos.x<pAngle->start_push_flat_length&&((
				pAngle->start_push_flat_x1_y2==2&&localpos.y<0)||(pAngle->start_push_flat_x1_y2==1&&localpos.y>0)))
				_L0 += (short)pAngle->Thick;	//螺栓穿孔处正好有压扁,计算双倍肢厚
			else if(localpos.x>length-pAngle->end_push_flat_length&&((
				pAngle->end_push_flat_x1_y2==2&&localpos.y<0)||(pAngle->end_push_flat_x1_y2==1&&localpos.y>0)))
				_L0 += (short)pAngle->Thick;	//螺栓穿孔处正好有压扁,计算双倍肢厚
			else if(pAngle->mid_push_flat_x1_y2==1||pAngle->mid_push_flat_x1_y2==2)
			{
				CLDSNode *pMidPushFlatNode=BelongModel()->FromNodeHandle(pAngle->mid_push_flat_node_handle);
				if(pMidPushFlatNode==NULL)
					continue;
				GEPOINT mid_push_flat_pos=pAngle->GetDatumPos(pMidPushFlatNode);
				double len_starttomid = pAngle->startOdd()+DISTANCE(pAngle->Start(),mid_push_flat_pos);
				if(localpos.x>len_starttomid-pAngle->mid_push_flat_start_offset&&localpos.x<len_starttomid+pAngle->mid_push_flat_end_offset&&((
				pAngle->mid_push_flat_x1_y2==2&&localpos.y<0)||(pAngle->mid_push_flat_x1_y2==1&&localpos.y>0)))
					_L0 +=(short)pAngle->Thick;	//螺栓穿孔处正好有压扁,计算双倍肢厚
			}
			if(dfOldL0>_L0)
			{
				for(i=0;i<xarrThickParts.GetSize();i++)
				{
					if(xarrThickParts[i].pPart!=pAngle)
						continue;
					xarrThickParts[i].thick+=dfOldL0-_L0;
				}
			}
		}
	}
	//5.根据穿过中心点的z向范围值计算通厚值（可避免中间因缺少垫板引起的通厚计算偏差)
	if (blNowCalL0IncGapByInters)
	{
		CBubbleSort<THICKPART>::BubSort(xarrThickParts.m_pData, xarrThickParts.GetSize(), _LocalCompareThickPart);
		double dfSumGap = 0;
		double dfSummPartThick = xarrThickParts.GetSize() > 0 ? xarrThickParts[0].thick : 0;
		for (i = 1; i < xarrThickParts.GetSize(); i++)
		{
			double gap = xarrThickParts[i].zBottom - (xarrThickParts[i - 1].zBottom + xarrThickParts[i - 1].thick);
			if (gap >= 2)	//可能是缺少垫板，应增加预留垫板间隙
				dfSumGap += ftoi(gap);
			dfSummPartThick += xarrThickParts[i].thick;
		}
		_L0 = max(_L0, ftoi(dfSumGap + dfSummPartThick));
		if (xarrThickParts.GetSize() > 0 && fabs(xarrThickParts[0].zBottom) > 2)
			ucs.origin += xarrThickParts[0].zBottom*ucs.axis_z;	//修正螺栓穿插位置
	}
	m_bNeedUpdateL0=false;
	return _L0;
}
short CLDSBolt::get_L0(BOOL bNeedUpdateL0/*=FALSE*/)
{	
	if(m_bNeedUpdateL0||bNeedUpdateL0)
	{
		return CalL0();
		bool hasPriorPartThick=false;
		//通过调用m_bEnableActiveHeightFilterOnL0InternalCal，可以允许在某些特定场景（快速自动建模时）关掉呼高筛选 wjh-2019.6.14
		_L0 =(short)designL0.Thick(BelongModel(), m_bEnableActiveHeightFilterOnL0InternalCal,&hasPriorPartThick);
		//计算螺栓通厚时考虑螺栓垫圈厚度 wht 11-01-10
		if(DianQuan.N>0&&DianQuan.thick)
			_L0 +=DianQuan.N*DianQuan.thick;
		THICK_ITEM item;
		for(item=designL0.GetFirstItem();!item.IsFinalKey();item=designL0.GetNextItem())
		{
			if((!hasPriorPartThick&&item.cItemType==THICK_ITEM::PART_THROUGH)||
				item.cItemType==THICK_ITEM::PART_THROUGHnT)
			{
				BOOL pushed=BelongModel()->PushPartStack();
				CLDSLineAngle* pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(item.val,CLS_LINEANGLE);
				BelongModel()->PopPartStack(pushed);
				if(pAngle==NULL)
					continue;
				GEPOINT localpos;
				if( pAngle->start_push_flat_x1_y2==1||pAngle->start_push_flat_x1_y2==2||
					pAngle->end_push_flat_x1_y2==1||pAngle->end_push_flat_x1_y2==2||
					pAngle->mid_push_flat_x1_y2==1||pAngle->mid_push_flat_x1_y2==2)
				{
					if(!pAngle->GetBoltLocalPos(this,localpos,true))
						continue;
				}
				else
					continue;
				double length=pAngle->GetLength();
				if( localpos.x<pAngle->start_push_flat_length&&((
					pAngle->start_push_flat_x1_y2==2&&localpos.y<0)||(pAngle->start_push_flat_x1_y2==1&&localpos.y>0)))
					_L0 +=(short)pAngle->Thick;	//螺栓穿孔处正好有压扁,计算双倍肢厚
				else if(localpos.x>length-pAngle->end_push_flat_length&&((
					pAngle->end_push_flat_x1_y2==2&&localpos.y<0)||(pAngle->end_push_flat_x1_y2==1&&localpos.y>0)))
					_L0 +=(short)pAngle->Thick;	//螺栓穿孔处正好有压扁,计算双倍肢厚
				else if(pAngle->mid_push_flat_x1_y2==1||pAngle->mid_push_flat_x1_y2==2)
				{
					CLDSNode *pMidPushFlatNode=BelongModel()->FromNodeHandle(pAngle->mid_push_flat_node_handle);
					if(pMidPushFlatNode==NULL)
						continue;
					GEPOINT mid_push_flat_pos=pAngle->GetDatumPos(pMidPushFlatNode);
					double len_starttomid = pAngle->startOdd()+DISTANCE(pAngle->Start(),mid_push_flat_pos);
					if(localpos.x>len_starttomid-pAngle->mid_push_flat_start_offset&&localpos.x<len_starttomid+pAngle->mid_push_flat_end_offset&&((
					pAngle->mid_push_flat_x1_y2==2&&localpos.y<0)||(pAngle->mid_push_flat_x1_y2==1&&localpos.y>0)))
						_L0 +=(short)pAngle->Thick;	//螺栓穿孔处正好有压扁,计算双倍肢厚
				}
			}
		}
		m_bNeedUpdateL0=false;
	}
	return _L0;
}
#include "SortFunc.h"
int CompareThickItemByZOffset(const THICK_ITEMTAG& item1,const THICK_ITEMTAG& item2)
{
	if(item1.zOffset>item2.zOffset)
		return 1;
	else if(item1.zOffset<item2.zOffset)
		return -1;
	else
		return 0;
}
CXhChar50 CLDSBolt::EmendZOffsetStr(CLDSBolt* pBolt,PARTSET& partset)
{
	//修正Ｚ向偏移位置
	pBolt->correct_pos();
	long legacy_zoffset=pBolt->des_base_pos.norm_offset.Thick(pBolt->BelongModel());
	f3dPoint datumPosition=pBolt->ucs.origin;
	f3dPoint worknorm=pBolt->ucs.axis_z;
	if(pBolt->m_dwFlag.IsHasFlag(CLDSBolt::REVERSE_NORM))
	{
		worknorm*=-1.0;
		datumPosition-=pBolt->get_L0()*worknorm;
	}
	datumPosition-=legacy_zoffset*worknorm;
	ARRAY_LIST<THICK_ITEMTAG> items;
	items.SetSize(0,partset.GetNodeNum());
	for(CLDSPart* pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		f3dPoint inters;
		if(pPart->GetBoltIntersPos(pBolt,inters,NULL,CLDSPart::BOLTBTM_PLANE))
		{
			THICK_ITEMTAG* pItem=items.append();
			pItem->val=pPart->handle;
			pItem->pPart=pPart;
			pItem->through=ftoi(pPart->GetThick());
			f3dPoint zoffset_vec=inters-datumPosition;
			pItem->zOffset=zoffset_vec*worknorm;
		}
	}
	CQuickSort<THICK_ITEMTAG>::QuickSort(items.m_pData,items.GetSize(),CompareThickItemByZOffset);
	CXhChar50 zstr;
	for(int i=items.GetSize()-1;i>=0;i--)
	{
		double tolerance=items[i].through/2;
		if(items[i].zOffset>-1||items[i].zOffset+tolerance>0)
			continue;	//位于基点之上的穿孔厚度项
		if(i==0)
			zstr.Append(CXhChar16("-0X%X,",items[i].val));
		else
		{
			int gap=ftoi(items[i-1].zOffset-items[i].zOffset-items[i].through);
			if(gap>2)
				zstr.Append(CXhChar16("-%d,",gap));
			zstr.Append(CXhChar16("-0X%X,",items[i].val));
		}
	}
	if(zstr.GetLength()>0)
		zstr[zstr.GetLength()-1]=0;
	return zstr;
}

UCS_STRU CLDSBolt::GetUCS()
{
    //--------VVVVVVVVVV----计算角钢相对坐标系的三个坐标轴方向  Wjh--00.05.15-------
    if(normalize(ucs.axis_z)==FALSE)
	{
		ucs.axis_z.Set(0,1,0);
#ifdef AFX_TARG_ENU_ENGLISH
        Log2File()->Log("The work direction of bolt 0X%X is null.",handle);
#else 
		Log2File()->Log("0X%X螺栓方向为空值",handle);
#endif
	}
    //--------VVVVVVVV--------产生一不与ucs.axis_z下平行的随机向量vec
	if(ucs.axis_x.IsZero()||fabs(ucs.axis_x*ucs.axis_z)>EPS_COS)
		ucs.axis_x = inters_vec(ucs.axis_z);
	ucs.axis_x -= (ucs.axis_x*ucs.axis_z)*ucs.axis_z;
	normalize(ucs.axis_x);
    ucs.axis_y = ucs.axis_z^ucs.axis_x;
	return ucs;
}
CLDSPart *CLDSBolt::GetMirPart(MIRMSG mirmsg,const double *pPickPos/*=NULL*/,BOOL bCheckProfile/*=TRUE*/)
{
	/* 由于在对称钢板时对称的螺栓很难保证（尤其是标准插板反面时）严格的对称关系，
	故在获取对称螺栓时暂不从关联构件列表中查找。 wjh 2011.11.28
	for(RELATIVE_OBJECT *pItem=relativeObjs.GetFirst();pItem;pItem=relativeObjs.GetNext())
	{	//先在关联构件列表中查找
		CLDSBolt *pBolt=NULL;
		if(pItem->mirInfo.IsSameMirMsg(mirmsg))
		{
			pBolt=(CLDSBolt*)BelongModel()->FromPartHandle(pItem->hObj,CLS_BOLT);
			if(pBolt)
				return pBolt;
			else
				break;
		}
	}
	*/
	f3dPoint mir_origin=GetMirPos(ucs.origin,mirmsg);
	f3dPoint mir_axis_z=GetMirVector(ucs.axis_z,mirmsg);
	int cls_typeid=GetClassTypeId();
	BOOL pushed=m_pModel->PushPartStack(); //遍历构件列表前应先压栈 wht 12-03-05
	cfgword=UnifiedCfgword();
	BOOL bTubeFootNail=IsFootNail();
	CLDSLineSlot *pLineSlot=bTubeFootNail?(CLDSLineSlot*)BelongModel()->FromPartHandle(des_base_pos.hPart,CLS_LINESLOT):NULL;
	if(pLineSlot&&pLineSlot->IsBaseSlot())
		bTubeFootNail=TRUE;	//钢管上的脚钉 wht 18-12-29
	else
		bTubeFootNail=FALSE;
	for(CLDSPart *pMirPart=BelongModel()->EnumPartFirst(cls_typeid);pMirPart;pMirPart=BelongModel()->EnumPartNext(cls_typeid))
	{
		CLDSBolt *pMirBolt=(CLDSBolt*)pMirPart;
		//判断是否为对称螺栓时如果要求有效长度必须相同,容易生成重叠螺栓 wht 11-01-11
		if(pMirBolt->get_d()!=d)//||pMirBolt->get_L()!=L)
			continue;
		//配材号不完全相同的螺栓，即使位置完全相同，也不认为是对称螺栓，
		//以防多接腿空间相同，而处在不同呼高中螺栓冲突 wjh-2011.10.8
		pMirBolt->UnifiedCfgword();
		if(!ModuleTogetherWith(pMirBolt))
			continue;
		f3dPoint origin_vec=pMirBolt->ucs.origin-mir_origin;
		double len = origin_vec.mod();
		double dd=mir_axis_z*pMirPart->ucs.axis_z;
		if(len<L&&fabs(dd)>EPS_COS2)
		{
			if(bTubeFootNail&&dd<0)
				continue;	//钢管脚钉，对称时要求朝向一致 wht 18-12-29
			f3dPoint point =mir_origin;
			project_point(point,pMirBolt->ucs.origin,pMirPart->ucs.axis_z);
			double fDistance =DISTANCE(point,pMirBolt->ucs.origin);
			if(fabs(fDistance)<3)
			{
				m_pModel->PopPartStack(pushed);
				return pMirPart;
			}
		}
	}
	m_pModel->PopPartStack(pushed);
	return NULL;
}

void CLDSBolt::CopyProperty(CLDSBolt *pLs,bool blPushLAsInitStdBoltL/*=true*/)
{
	iSeg = pLs->iSeg;
	StrCopy(statSegStr,pLs->statSegStr,24);
	sPartNo=pLs->sPartNo;
	d = pLs->get_d();
	DianQuan = pLs->DianQuan;
	SetGrade(pLs->Grade());
	hole_d_increment = pLs->hole_d_increment;
	dwRayNo = pLs->dwRayNo;
	if( blPushLAsInitStdBoltL && _hFamily!=pLs->m_hFamily&&pLs->IsFootNail())
		_siInitStdL = (short)pLs->liInitL;
	m_hFamily=pLs->m_hFamily;
	m_cFuncType=pLs->m_cFuncType;	//拷贝特殊孔类型
	//m_bFootNail = pLs->m_bFootNail;
	//m_nLsCap = pLs->m_nLsCap;	//m_bTwoCapLs
	m_dwFlag=pLs->m_dwFlag;
	m_bVirtualPart=pLs->m_bVirtualPart;
	_L = (short)pLs->get_L();
	_L0 = (short)pLs->L0;
	designL0=pLs->designL0;
	ucs.axis_z = pLs->ucs.axis_z;
	ucs.origin = pLs->ucs.origin;
	des_base_pos.norm_offset=pLs->des_base_pos.norm_offset;
}
void CLDSBolt::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	DWORD dw;
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if( version==0||
		(doc_type==1&&version>=4020005)||	//TMA(V4.2.0.5)
		(doc_type==2&&version>=1020005)||	//LMA(V1.2.0.5)
		(doc_type==3&&version>=2000005)||	//TSA(V2.0.0.5)
		(doc_type==4&&version>=1020005)||	//LDS(V1.2.0.5)
		(doc_type==5&&version>=1020005))	//TDA(V1.2.0.5)
	{
		buffer.ReadDword(&dwPermission);
		buffer.ReadString(layer_tag,4);
		CfgwordFromBuffer(cfgword,buffer,version,doc_type);
		BelongModel()->EvolveCfgword(cfgword,version,doc_type);
		buffer.ReadString(sNotes,51);
		buffer.ReadInteger(&m_uStatMatCoef);
		buffer.ReadString(statSegStr,23);
		buffer.ReadInteger(&dw);
		m_dwFlag=dw;
		//buffer>>d;
		hole_d_increment=(float)buffer.ReadDouble();
		buffer.ReadDword(&dwRayNo);
		//buffer>>m_nDisplayWasher;
		_L=(short)buffer.ReadDouble();
		_siInitStdL=(short)buffer.ReadDouble();
		_L0=(short)buffer.ReadDouble();
		buffer.ReadPoint(ucs.origin);
		buffer.ReadPoint(ucs.axis_x);
		buffer.ReadPoint(ucs.axis_z);
		bool hasDianQuan;
		buffer.ReadBooleanThin(&hasDianQuan);
		if(hasDianQuan)
		{
			buffer.ReadWord(&DianQuan._wholethick);
			buffer.ReadWord(&DianQuan.N);
			buffer.ReadDouble(&DianQuan.offset);
		}
		//buffer.ReadString(designL0.key_str,50);
		designL0.FromBuffer(buffer,version,doc_type);
		des_base_pos.FromBuffer(buffer,version,doc_type);
		des_work_norm.FromBuffer(buffer,version,doc_type);
		int n=0;
		buffer.ReadInteger(&n);
		for(int j=0;j<n;j++)
		{
			RELATIVE_OBJECT *pMirObj=relativeObjs.Add(buffer.ReadInteger());
			buffer.ReadByte(&pMirObj->mirInfo.axis_flag);
			buffer.ReadPoint(pMirObj->mirInfo.origin);
			if((pMirObj->mirInfo.axis_flag&0x08)>0)
			{
				buffer.ReadWord(&pMirObj->mirInfo.rotate_angle);
				buffer.ReadByte(&pMirObj->mirInfo.array_num);
			}
			if(pMirObj->mirInfo.axis_flag&0x10)	//镜像
				buffer.ReadPoint(pMirObj->mirInfo.mir_norm);
		}
	}
}
void CLDSBolt::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteDword(dwPermission);
	buffer.WriteString(layer_tag);
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	buffer.WriteString(sNotes);
	buffer.WriteInteger(m_uStatMatCoef);
	buffer.WriteString(statSegStr);
	buffer.WriteInteger(m_dwFlag.FlagWord());
	//buffer<<d;
	buffer.WriteDouble(hole_d_increment);
	buffer.WriteDword(dwRayNo);
	//buffer<<m_nDisplayWasher;
	buffer.WriteDouble(L);
	buffer.WriteDouble(_siInitStdL);
	buffer.WriteDouble(L0);
	buffer.WritePoint(ucs.origin);
	buffer.WritePoint(ucs.axis_x);
	buffer.WritePoint(ucs.axis_z);
	//螺栓垫的存储
	buffer.WriteBooleanThin(DianQuan.N>0);
	if(DianQuan.N>0)
	{
		if( version==0||
			(doc_type==1&&version>=5010500)||	//TMA(V5.1.5.0)
			(doc_type==2&&version>=2000800)||	//LMA(V2.0.8.0)
			(doc_type==4&&version>=1030700)||	//LDS(V1.2.0.5)
			(doc_type==5&&version>=1030700))	//TDA(V1.2.0.5)
			buffer.WriteWord(DianQuan._wholethick);
		else
			buffer.WriteWord((WORD)(DianQuan._wholethick&0x7fff));
		buffer.WriteWord(DianQuan.N);
		buffer.WriteDouble(DianQuan.offset);
	}
	//buffer.WriteString(designL0.key_str);
	designL0.ToBuffer(buffer,version,doc_type);
	des_base_pos.ToBuffer(buffer,version,doc_type);
	des_work_norm.ToBuffer(buffer,version,doc_type);
	//关联构件
	buffer.WriteInteger(relativeObjs.GetNodeNum());	
	for(RELATIVE_OBJECT *pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pMirObj->hObj);
		buffer.WriteByte(pMirObj->mirInfo.axis_flag);
		buffer.WritePoint(pMirObj->mirInfo.origin);
		if((pMirObj->mirInfo.axis_flag&8)>0)
		{
			buffer.WriteWord(pMirObj->mirInfo.rotate_angle);
			buffer.WriteByte(pMirObj->mirInfo.array_num);
		}
		if(pMirObj->mirInfo.axis_flag&0x10)	//镜像
			buffer.WritePoint(pMirObj->mirInfo.mir_norm);

	}
}
void CLDSBolt::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropFromBuffer(buffer,version,doc_type);
		return;
	}
	DWORD dw;
	bool bPrevDeflateVersion=false;	//文件瘦身之前的版本
	if( (doc_type==1&&version>0&&version<4000027)||	//TMA
		(doc_type==2&&version>0&&version<1000082)||	//LMA
		(doc_type==4&&version>0&&version<1000009))	//LDS
		bPrevDeflateVersion=true;
	buffer.ReadDword(&dwPermission);
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	BelongModel()->EvolveCfgword(cfgword,version,doc_type);
	if( version==0||
	   (version>=5000003&&doc_type==1)||(version>=2000003&&doc_type==2)||
	   (version>=1030003&&doc_type==4)||(version>=1030003&&doc_type==5))
	   mpcl.FromBuffer(&buffer,version,doc_type);
	if( version==0||doc_type==5||
		(doc_type==1&&version>=4000100)|| //TMA V4.0.1.0
		(doc_type==2&&version>=1000100)|| //LMA V1.0.1.0
		(doc_type==4&&version>=1000300))//LDS V1.0.3.0
		buffer.ReadInteger(&m_uStatMatCoef);	//螺栓统材系数 wht 11-01-10
	buffer.ReadString(layer(),4);
	buffer.ReadInteger(&_iSeg.iSeg);
	if( (doc_type==1&&version==4000109)|| //TMA V4.0.1.9
		(doc_type==2&&version==1000109)|| //LMA V1.0.1.9
		(doc_type==4&&version==1000309))  //LDS V1.0.3.9
	{	//为了避免增加版本号，该版临时这样判断处理
		int test_d=0;
		memcpy(&test_d,buffer.GetBufferPtr()+buffer.GetCursorPosition(),sizeof(int));
		if(test_d>=12&&test_d<100)
			buffer.ReadInteger(&test_d);
		else
		{
			buffer.ReadString(statSegStr,23);
			buffer.ReadInteger(&test_d);
		}
		_d = (short)test_d;
	}
	else if(version==0||doc_type==5||
		(doc_type==1&&version>4000109)|| //TMA V4.0.1.9
		(doc_type==2&&version>1000109)|| //LMA V1.0.1.9
		(doc_type==4&&version>1000309))  //LDS V1.0.3.9
	{
		buffer.ReadString(statSegStr,23);
		_d=(short)buffer.ReadInteger();
	}
	else
		_d=(short)buffer.ReadInteger();
	if(bPrevDeflateVersion)
	{
		double D,H;
		buffer>>D;
		buffer>>H;
	}
	m_sfHoleDIncrement =(float)buffer.ReadDouble();
	if(bPrevDeflateVersion)
	{
		long edge_space;	//以前加上是用于表示板上螺栓的边距，后发现无用节省空间去掉 wjh -2010-3-16
		buffer>>edge_space;
	}
	buffer>>dwRayNo;
	if(bPrevDeflateVersion)
	{
		buffer.ReadInteger(&dw);
		m_bVirtualPart=(dw!=0);
	}
	else
		buffer.ReadBooleanThin(&_bVirtualPart);
	if(version==0 || doc_type==5||
		(doc_type==1&&version>=4000111)||
		(doc_type==2&&version>=1000111)||
		(doc_type==4&&version>=1000311))
	{
		_hFamily=(short)buffer.ReadInteger();
		m_dwFlag=buffer.ReadInteger();
		if(  version>0&&(
			(doc_type==1&&version<5020201)||	//<TMA V5,2,2,1
			(doc_type==2&&version<2010201)||	//<LMA V2,1,2,1
			(doc_type==4&&version<1031001)||	//<LDS V1,3,10,1
			(doc_type==5&&version<1031001)))		//<TDA V1,3,10,1
			buffer.ReadInteger();//&m_nDisplayWasher);
	}
	else
	{
		BOOL justify;
		buffer.ReadInteger(&justify);//m_bFootNail;
		m_dwFlag.SetFlagDword(FOOT_NAIL,justify);
		buffer.ReadInteger(&justify);//m_bHaveSpringPad;
		m_dwFlag.SetFlagDword(ANTI_LOOSE,justify);
		buffer.ReadInteger(&justify);//m_bHaveSpringPad;
		m_dwFlag.SetFlagDword(TWO_CAP,justify);
		buffer.ReadInteger(&justify);//m_bBurglarproof;
		m_dwFlag.SetFlagDword(ANTI_THEFT,justify);
		if(d>24)	//法兰底脚螺栓系列
			m_hFamily=4;//属脚钉螺栓系列m_bFootNail
		else if(m_dwFlag.IsHasFlag(TWO_CAP))
			m_hFamily=2;//属双帽螺栓系列m_bTwoCapLs
		else if(m_dwFlag.IsHasFlag(FOOT_NAIL))
			m_hFamily=3;//属脚钉螺栓系列m_bFootNail
		else
			m_hFamily=1;//属标准螺栓系列
	}
	_L=(short)buffer.ReadDouble();
	_siInitStdL=(short)buffer.ReadDouble();
	_L0=(short)buffer.ReadDouble();
	if(bPrevDeflateVersion)
	{
		long m_dwRef;	//以前加上是用于表示板上螺栓的边距，后发现无用节省空间去掉 wjh -2010-3-16
		buffer>>m_dwRef;
	}
	buffer.ReadPoint(ucs.origin);
	buffer.ReadPoint(ucs.axis_x);
	buffer.ReadPoint(ucs.axis_z);
	if( version==0||doc_type==4||doc_type==5||
		(doc_type==1&&version>=4000021)||	//TMA >=V4.0.0.21
		(doc_type==2&&version>=1000076))	//LMA >=V1.0.0.76
	{	//用户指定构件颜色
		if(bPrevDeflateVersion)
		{
			buffer.ReadInteger(&dw);
			m_bUserSpecColor=(dw!=0);
		}
		else
			buffer.ReadBooleanThin(&m_bUserSpecColor);
		if(m_bUserSpecColor)
			buffer>>crMaterial;
	}
	//螺栓垫的存储
	if(bPrevDeflateVersion)
	{
		buffer>>DianQuan._wholethick;
		buffer>>DianQuan.N;
		buffer>>DianQuan.offset;
	}
	else
	{
		bool hasDianQuan;
		buffer.ReadBooleanThin(&hasDianQuan);
		if(hasDianQuan)
		{
			buffer>>DianQuan._wholethick;
			buffer>>DianQuan.N;
			buffer>>DianQuan.offset;
		}
	}
	buffer.Read(grade,4);
	if(	version==0&&doc_type!=3||
		(doc_type==1&&version>=5010800)||	//TMA V5.1.8.0
		(doc_type==2&&version>=2000900)||	//LMA V2.0.9.0
		(doc_type==4&&version>=1030900)||	//LDS V1.3.9.0
		(doc_type==5&&version>=1030900))	//TDA V1.3.9.0
		buffer.ReadBooleanThin(&m_bDisableSimpleLocateMode);
	if (version==0||(
		(doc_type==1&&version>=5020200)||	//TMA V5,2,2,0
		(doc_type==2&&version>=2010200)||	//LMA V2,1,2,0
		(doc_type==3&&version>=2010401)||	//TSA V2,1,4,1
		(doc_type==4&&version>=1031000)||	//LDS V1,3,10,0
		(doc_type==5&&version>=1031000)))	//TDA V1,3,10,0
		buffer.ReadBooleanThin(&this->m_bCalL0IncGapByInters);
	else if (doc_type==1||doc_type==2)	//旧格式文件读时，考虑到旧放样文件基本都以螺栓通厚累加原则计算
		m_bCalL0IncGapByInters=false;
	else
		m_bCalL0IncGapByInters = true;
	//螺栓通厚设计参数
	designL0.FromBuffer(buffer,version,doc_type);
	//buffer.ReadString(designL0.key_str,51);
	//m_bNeedUpdateL0=TRUE;	//隐掉，由于软件目前已提供通厚修订功能，现没有必要在新读取文件时就自动重新算通厚。wjh-2011.9.3
	//基点位置设计数据
	des_base_pos.FromBuffer(buffer,version,doc_type);
	//工作法线设计数据
	des_work_norm.FromBuffer(buffer,version,doc_type);
	buffer.ReadString(sNotes,51);
	//螺栓功用类型
	if(	version==0||
		(doc_type==1&&version>=5000000)||	//TMA V5.0.0.0
		(doc_type==2&&version>=5000000)||	//LMA V2.0.0.0
		(doc_type==3&&version>=2010000)||	//TSA V2.1.0.0
		(doc_type==4&&version>=1030000)||	//LDS V1.3.0.0
		(doc_type==5&&version>=1030000))	//TDA V1.3.0.0
	{
		buffer.ReadByte(&_cFuncType);
		if(m_cFuncType<2&&m_bVirtualPart)
			m_cFuncType=2;	//默认旧版文件中的虚拟螺栓为挂线孔
	}
	//关联构件列表
	int i,nn=0;
	buffer.ReadInteger(&nn);
	RELATIVE_OBJECT *pRelaObj=NULL;
	relativeObjs.Empty();
	for(i=0;i<nn;i++)
	{
		pRelaObj=relativeObjs.Add(buffer.ReadInteger());
		if(bPrevDeflateVersion)
		{
			buffer.ReadDword(&dw);
			pRelaObj->mirInfo.axis_flag=(BYTE)dw;
			buffer.ReadInteger(&dw);
			pRelaObj->mirInfo.array_num=(BYTE)dw;
			buffer.ReadPoint(pRelaObj->mirInfo.origin);
			double ff;
			buffer.ReadDouble(&ff);
			pRelaObj->mirInfo.rotate_angle=(short)ff;
		}
		else
		{
			buffer.ReadByte(&pRelaObj->mirInfo.axis_flag);
			bool bSpecOrigin;
			buffer.ReadBooleanThin(&bSpecOrigin);
			if(bSpecOrigin)
				buffer.ReadPoint(pRelaObj->mirInfo.origin);
			if(pRelaObj->mirInfo.axis_flag&0x08)
			{
				buffer.ReadByte(&pRelaObj->mirInfo.array_num);
				buffer.ReadWord(&pRelaObj->mirInfo.rotate_angle);
			}
		}
	}
	SetModified();
}
void CLDSBolt::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropToBuffer(buffer,version,doc_type);
		return;
	}
	buffer<<dwPermission;
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	if( version==0||
	   (version>=5000003&&doc_type==1)||(version>=2000003&&doc_type==2)||
	   (version>=1030003&&doc_type==4)||(version>=1030003&&doc_type==5))
	   mpcl.ToBuffer(&buffer,version,doc_type);
	buffer.WriteInteger(m_uStatMatCoef);	//保存螺栓统材系数 wht 11-01-10
	buffer.WriteString(layer());
	buffer.WriteInteger(iSeg);
	buffer.WriteString(statSegStr);//sNotes
	buffer.WriteInteger(_d);
	buffer.WriteDouble(m_sfHoleDIncrement);
	buffer<<dwRayNo;
	buffer.WriteBooleanThin(_bVirtualPart);
	buffer.WriteInteger(m_hFamily);
	buffer.WriteInteger(m_dwFlag.FlagWord());
	if(  version>0&&(
		(doc_type==1&&version<5020201)||	//<TMA V5,2,2,1
		(doc_type==2&&version<2010201)||	//<LMA V2,1,2,1
		(doc_type==4&&version<1031001)||	//<LDS V1,3,10,1
		(doc_type==5&&version<1031001)))		//<TDA V1,3,10,1
		buffer.WriteInteger(0);//&m_nDisplayWasher);
	buffer.WriteDouble(_L);
	buffer.WriteDouble(_siInitStdL);
	buffer.WriteDouble(_L0);
	buffer<<ucs.origin.x;
	buffer<<ucs.origin.y;
	buffer<<ucs.origin.z;
	buffer<<ucs.axis_x.x;
	buffer<<ucs.axis_x.y;
	buffer<<ucs.axis_x.z;
	buffer<<ucs.axis_z.x;
	buffer<<ucs.axis_z.y;
	buffer<<ucs.axis_z.z;
	buffer.WriteBooleanThin(m_bUserSpecColor);	//指定构件颜色	
	if(m_bUserSpecColor)
		buffer<<crMaterial;		//构件颜色
	//螺栓垫的存储
	buffer.WriteBooleanThin(DianQuan.N>0);
	if(DianQuan.N>0)
	{
		if( version==0||
			(doc_type==1&&version>=5010500)||	//TMA(V5.1.5.0)
			(doc_type==2&&version>=2000800)||	//LMA(V2.0.8.0)
			(doc_type==4&&version>=1030700)||	//LDS(V1.2.0.5)
			(doc_type==5&&version>=1030700))	//TDA(V1.2.0.5)
			buffer.WriteWord(DianQuan._wholethick);
		else
			buffer.WriteWord((WORD)(DianQuan._wholethick&0x7fff));
		buffer<<DianQuan.N;
		buffer<<DianQuan.offset;
	}
	buffer.Write(Grade(),4);
	if(	version==0&&doc_type!=3||
		(doc_type==1&&version>=5010800)||	//TMA V5.1.8.0
		(doc_type==2&&version>=2000900)||	//LMA V2.0.9.0
		(doc_type==4&&version>=1030900)||	//LDS V1.3.9.0
		(doc_type==5&&version>=1030900))	//TDA V1.3.9.0
		buffer.WriteBooleanThin(m_bDisableSimpleLocateMode);
	if (version==0||(
		(doc_type==1&&version>=5020200)||	//TMA V5,2,2,0
		(doc_type==2&&version>=2010200)||	//LMA V2,1,2,0
		(doc_type==3&&version>=2010401)||	//TSA V2,1,4,1
		(doc_type==4&&version>=1031000)||	//LDS V1,3,10,0
		(doc_type==5&&version>=1031000)))	//TDA V1,3,10,0
		buffer.WriteBooleanThin(m_bCalL0IncGapByInters);
	//螺栓通厚设计参数
	designL0.ToBuffer(buffer,version,doc_type);
	//buffer.WriteString(designL0.key_str);
	//基点位置设计数据
	des_base_pos.ToBuffer(buffer,version,doc_type);
	//工作法线设计数据
	des_work_norm.ToBuffer(buffer,version,doc_type);
	buffer.WriteString(sNotes);
	//螺栓功用类型
	if(	version==0||
		(doc_type==1&&version>=5000000)||	//TMA V5.0.0.0
		(doc_type==2&&version>=5000000)||	//LMA V2.0.0.0
		(doc_type==3&&version>=2010000)||	//TSA V2.1.0.0
		(doc_type==4&&version>=1030000)||	//LDS V1.3.0.0
		(doc_type==5&&version>=1030000))	//TDA V1.3.0.0
		buffer.WriteByte(_cFuncType);	//功用类型
	//关联构件列表
	RELATIVE_OBJECT *pRelaObj=NULL;
	BUFFERPOP stack(&buffer,relativeObjs.GetNodeNum());
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for(pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pRelaObj->hObj);
		buffer.WriteByte(pRelaObj->mirInfo.axis_flag);
		bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
		buffer.WriteBooleanThin(bSpecOrigin);
		if(bSpecOrigin)
			buffer.WritePoint(pRelaObj->mirInfo.origin);
		if(pRelaObj->mirInfo.axis_flag&0x08)
		{
			buffer.WriteByte(pRelaObj->mirInfo.array_num);
			buffer.WriteWord(pRelaObj->mirInfo.rotate_angle);
		}
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of relative object is wrong!");
#else
		CLDSObject::Log2File()->Log("关联构件记录数出现错误!");
#endif

}

void CLDSBolt::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<螺栓 handle=\"0X%X\">\n",handle);
	fprintf(fp,"<权限字 dwPermission=\"%d\"/>\n",dwPermission);
	fprintf(fp,"<配材号 cfgword.word=\"%d,%d,%d,%d,%d\"/>\n",cfgword.flag.word[0],cfgword.flag.word[1],cfgword.flag.word[2],cfgword.flag.word[3],cfgword.flag.word[4]);
	fprintf(fp,"<统材系数 m_uStatMatCoef=\"%d\"/>\n",m_uStatMatCoef);
	fprintf(fp,"<图层 layer=\"%s\"/>\n",layer());
	fprintf(fp,"<分段号 iSeg=\"%d\"/>\n",iSeg);
	fprintf(fp,"<螺栓归属段号字符串 statSegStr=\"%s\"/>\n",statSegStr);
	fprintf(fp,"<螺栓直径 d=\"%d\"/>\n",d);
	sprintf(sText,"%f",hole_d_increment);
	SimplifiedNumString(sText);
	fprintf(fp,"<螺栓孔直径增量 hole_d_increment=\"%s\"/>\n",sText);
	fprintf(fp,"<所在的射线索引号 dwRayNo=\"%d\"/>\n",dwRayNo);
	if (m_bVirtualPart)
		fprintf(fp,"<虚拟螺栓 m_bVirtualPart=\"true\"/>\n");
	else
		fprintf(fp,"<虚拟螺栓 m_bVirtualPart=\"false\"/>\n");
	fprintf(fp,"<从属螺栓规格系列 m_hFamily=\"%d\"/>\n",m_hFamily);
	fprintf(fp,"<标识符 m_dwFlag.FlagWord=\"%d\"/>\n",m_dwFlag.FlagWord());
	sprintf(sText,"%f",L);
	SimplifiedNumString(sText);
	fprintf(fp,"<螺栓有效长度 L=\"%s\"/>\n",sText);
	sprintf(sText,"%f",_siInitStdL);
	SimplifiedNumString(sText);
	fprintf(fp,"<恢复变为脚钉的螺栓有效长度 InitStdL=\"%s\"/>\n",sText);
	sprintf(sText,"%f",L0);
	SimplifiedNumString(sText);
	fprintf(fp,"<螺栓通过厚度 L0=\"%s\"/>\n",sText);
	fprintf(fp,"<相对坐标系>\n");
	sprintf(sText,"%f",ucs.origin.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.origin.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.origin.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<原点 ucs.origin=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",ucs.axis_x.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.axis_x.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.axis_x.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<X轴 ucs.axis_x=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",ucs.axis_z.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.axis_z.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.axis_z.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<Z轴 ucs.axis_z=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	fprintf(fp,"</相对坐标系>\n");
	if (m_bUserSpecColor)
	{
		fprintf(fp,"<用户指定构件颜色 m_bUserSpecColor=\"true\"/>\n");
		fprintf(fp,"<构件颜色 crMaterial=\"%d\"/>\n",crMaterial);
	}
	else
		fprintf(fp,"<用户指定构件颜色 m_bUserSpecColor=\"false\"/>\n");
	if (DianQuan.N>0)
	{
		fprintf(fp,"<垫圈厚 DianQuan.thick=\"%d\"/>\n",DianQuan.thick);
		sprintf(sText,"%f",DianQuan.offset);
		SimplifiedNumString(sText);
		fprintf(fp,"<从螺栓基点沿轴向的偏移量 DianQuan.offset=\"%s\"/>\n",sText);
		fprintf(fp,"<垫圈数量 DianQuan.N=\"%d\"/>\n",DianQuan.N);
	}
	fprintf(fp,"<螺栓级别 grade=\"%s\"/>\n",Grade());
	fprintf(fp,"<螺栓通厚设计参数 designL0.key_str=\"%s\"/>\n",(char*)designL0.ToThickStr());
	//基点位置设计数据
	des_base_pos.ToXmlFile(fp,schema);
	//工作法线设计数据
	des_work_norm.ToXmlFile(fp,schema);
	fprintf(fp,"<备注 sNotes=\"%s\"/>\n",sNotes);
	fprintf(fp,"<关联构件列表>\n");
	fprintf(fp,"<关联构件数 relativeObjs.GetNodeNum=\"%d\"/>\n",relativeObjs.GetNodeNum());
	RELATIVE_OBJECT *pRelaObj=NULL;
	for(pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		fprintf(fp,"<关联构件 hObj=\"OX%X\"",pRelaObj->hObj);
		fprintf(fp," mirInfo.axis_flag=\"%d\"",pRelaObj->mirInfo.axis_flag);
		bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
		if (bSpecOrigin)
		{
			fprintf(fp," bSpecOrigin=\"true\"");
			sprintf(sText,"%f",pRelaObj->mirInfo.origin.x);
			SimplifiedNumString(sText);
			sprintf(sText2,"%f",pRelaObj->mirInfo.origin.y);
			SimplifiedNumString(sText2);
			sprintf(sText3,"%f",pRelaObj->mirInfo.origin.z);
			SimplifiedNumString(sText3);
			fprintf(fp," mirInfo.origin=\"(%s,%s,%s)\"",sText,sText2,sText3);
		}
		else
			fprintf(fp," bSpecOrigin=\"false\"");
		if(pRelaObj->mirInfo.axis_flag&0x08)
		{
			fprintf(fp," mirInfo.array_num=\"%d\"",pRelaObj->mirInfo.array_num);
			fprintf(fp," mirInfo.rotate_angle=\"%d\"",pRelaObj->mirInfo.rotate_angle);
		}
		fprintf(fp,"/>\n");
	}
	fprintf(fp,"</关联构件列表>\n");
	fprintf(fp,"</螺栓>\n");
}
void CLDSBolt::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//已调用过)
		return;
	_console->AddToObjSetBuf(this);
	if(!bAddRelaObj)
		return;
	bAddRelaObj=bAddSubLevelRelaObj;
	CLDSPart* pPart=m_pModel->FromPartHandle(des_base_pos.hPart);
	if(pPart)
		pPart->AddToConsoleObjBuf(bAddRelaObj);
	des_base_pos.datumPoint.SetBelongModel(m_pModel);
	des_base_pos.datumPoint.AddToConsoleObjBuf(bAddRelaObj);
	des_work_norm.AddToConsoleObjBuf(bAddRelaObj);
}



bool CLDSBolt::IsFootNail()
{
	CLDSBolt* pMotherBolt=(CLDSBolt*)MotherPart();
	if(pMotherBolt!=this)
		return pMotherBolt->IsFootNail();
	return m_dwFlag.IsHasFlag(FOOT_NAIL)!=FALSE;
}
bool CLDSBolt::IsAntiTheft()
{
	CLDSBolt* pMotherBolt=(CLDSBolt*)MotherPart();
	if(pMotherBolt!=this)
		return pMotherBolt->IsAntiTheft();
	if(m_dwFlag.IsHasFlag(ANTI_THEFT))
		return true;
	CLsFamily *pLsFamily=CLsLibrary::FromHandle(m_hFamily);
	return pLsFamily!=NULL?pLsFamily->m_bBurglarproof:FALSE;
}
BITFLAG CLDSBolt::get_dwSafeFlag()
{
	BITFLAG dwflag=m_dwFlag;
	dwflag.SetFlagDword(ANTI_THEFT,IsAntiTheft());
	dwflag.SetFlagDword(ANTI_LOOSE,IsAntiLoose());
	return dwflag;
}
bool CLDSBolt::IsTwoCap()
{
	CLDSBolt* pMotherBolt=(CLDSBolt*)MotherPart();
	if(pMotherBolt!=this)
		return pMotherBolt->IsTwoCap();
	return m_dwFlag.IsHasFlag(TWO_CAP)!=FALSE;
}
bool CLDSBolt::IsAntiLoose()
{
	CLDSBolt* pMotherBolt=(CLDSBolt*)MotherPart();
	if(pMotherBolt!=this)
		return pMotherBolt->IsAntiLoose();
	if(m_dwFlag.IsHasFlag(ANTI_LOOSE))
		return true;
	CLsFamily *pLsFamily=CLsLibrary::FromHandle(m_hFamily);
	return pLsFamily!=NULL?pLsFamily->m_bAntiLoose:FALSE;
}
bool CLDSBolt::CalGuigeAuto(BOOL bCalShadowBoltGuiGe/*=FALSE*/,BOOL bIncGrade/*=FALSE*/)
{
	if(bCalShadowBoltGuiGe==FALSE&&IsShadowObject())
		return true;
	SetModified();
	LS_XING_HAO *pLsXingHao=NULL;
	CLsFamily *pLsFamily=CLsLibrary::FromHandle(m_hFamily);
	if(pLsFamily==NULL)
		pLsFamily=CLsLibrary::DefaultFamily();
	long hActualFamily=pLsFamily->GetFamilyHandle();
	CLsFamily *pActualFamily=pLsFamily;
	if(m_dwFlag.IsHasFlag(ANTI_LOOSE)&&pLsFamily->m_hMappingAntiLooseFamily>0)
		hActualFamily=pLsFamily->m_hMappingAntiLooseFamily;
	if(m_dwFlag.IsHasFlag(ANTI_THEFT)&&pLsFamily->m_hMappingAntiTheftFamily>0)
		hActualFamily=pLsFamily->m_hMappingAntiTheftFamily;
	if(hActualFamily==pLsFamily->GetFamilyHandle())
		pActualFamily=pLsFamily;
	else if((pActualFamily=CLsLibrary::FromHandle(hActualFamily))==NULL)
	{
		hActualFamily=pLsFamily->GetFamilyHandle();
		pActualFamily=pLsFamily;
	}
	if(library)
		pLsXingHao=library->FindLsGuiGe(d,get_L0(TRUE),hActualFamily);
	if(pLsXingHao==NULL)
		return false;
	L=pLsXingHao->valid_len;
	//不注释掉会在自动修正螺栓通厚时变更螺栓的级别，这对于部分同一塔型同一规格在接头处螺栓级别不同的情况
	//会导致出错 wjh-2012-8-8
	if(strcmp(grade,"")==0||bIncGrade)	//只在无螺栓级别设定时才自动更新螺栓级别 wjh-2013.9.4
		StrCopy(Grade(),CXhChar16("%.1f",pLsXingHao->grade),5);
	//m_nWasherThick=pLsXingHao->washer_thick;
	if(DianQuan.AutoMatchThick)
	{
		BYTE thicks[4]={0};
		int count=CLDSPart::library->GetWasherThicks(d,thicks);
		if(count==0)//DianQuan.thick<=0)
			DianQuan.thick=pLsXingHao->washer_thick;
		else
		{
			short nearthick=DianQuan.thick;
			for(int i=0;i<4;i++)
			{	//查找最接近的垫圈厚
				if(abs(thicks[i]-DianQuan.thick)<abs(thicks[i]-nearthick))
					nearthick=thicks[i];
				DianQuan.thick=nearthick;
			}
		}
	}
	m_dwFlag.SetFlagDword(TWO_CAP,pLsFamily->m_bDualCap);
	m_dwFlag.SetFlagDword(FOOT_NAIL,pLsFamily->m_bFootNail);
	//根据成都铁塔厂的调研情况,防盗螺栓可以作为一种特殊属性处理(任何一种螺栓都可以增加防盗属性),
	//故变更螺栓系列,不应变更其防盗属性,除非手动设定为防盗（防松）螺栓系列 wjh-2017.11.24
	//故综合各种情况，更改防盗/松螺栓规格系列时，不更改统计属性，但IsAntiTheft()返回值会有变化 wjh-2018.3.28
	//if(pLsFamily->m_bBurglarproof)
	//	m_dwFlag.SetFlagDword(ANTI_THEFT,TRUE);
	//else
	//	m_dwFlag.SetFlagDword(ANTI_THEFT,FALSE);
	//if(pLsFamily->m_bAntiLoose)
	//	m_dwFlag.SetFlagDword(ANTI_LOOSE,TRUE);
	SetModified();
	return true;
}
//判断螺栓是否需要垫圈
BOOL CLDSBolt::IsNeedGasket(PARTSET& partset)
{
	f3dPoint datumPos=ucs.origin;
	f3dPoint worknorm=ucs.axis_z;
	ARRAY_LIST<THICK_ITEMTAG> items;
	items.SetSize(0,partset.GetNodeNum());
	for(CLDSPart* pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		f3dPoint inters;
		if(pPart->GetBoltIntersPos(this,inters,NULL,CLDSPart::BOLTBTM_PLANE))
		{
			THICK_ITEMTAG* pItem=items.append();
			pItem->val=pPart->handle;
			pItem->pPart=pPart;
			pItem->through=ftoi(pPart->GetThick());
			f3dPoint zoffset_vec=inters-datumPos;
			pItem->zOffset=zoffset_vec*worknorm;
		}
	}
	CQuickSort<THICK_ITEMTAG>::QuickSort(items.m_pData,items.GetSize(),CompareThickItemByZOffset);
	double fMaxSpace=3;
	for(int i=0;i<items.GetSize()-1;i++)
	{
		double fFullDist=items[i+1].zOffset-items[i].zOffset;
		double fSpacing=fFullDist-items[i].through;
		if(fabs(fSpacing)>fMaxSpace)
			return TRUE;
	}
	return FALSE;
}
//计算垫圈偏移量：自螺栓算起,垫圈法线和螺栓法线一致
void CLDSBolt::CalGasketOffset(PARTSET& partset)
{
	if(DianQuan.N==0 || DianQuan.thick==0)
		return;
	f3dPoint datumPos=ucs.origin;
	f3dPoint worknorm=ucs.axis_z;
	ARRAY_LIST<THICK_ITEMTAG> items;
	items.SetSize(0,partset.GetNodeNum());
	for(CLDSPart* pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		f3dPoint inters;
		if(pPart->GetBoltIntersPos(this,inters,NULL,CLDSPart::BOLTBTM_PLANE))
		{
			THICK_ITEMTAG* pItem=items.append();
			pItem->val=pPart->handle;
			pItem->pPart=pPart;
			pItem->through=ftoi(pPart->GetThick());
			f3dPoint zoffset_vec=inters-datumPos;
			pItem->zOffset=zoffset_vec*worknorm;
		}
	}
	CQuickSort<THICK_ITEMTAG>::QuickSort(items.m_pData,items.GetSize(),CompareThickItemByZOffset);
	double fMaxSpace=3;
	for(int i=0;i<items.GetSize()-1;i++)
	{
		double fFullDist=items[i+1].zOffset-items[i].zOffset;
		double fSpacing=fFullDist-items[i].through;
		if(fabs(fSpacing)>fMaxSpace)
		{
			DianQuan.offset=items[i].zOffset+items[i].through;
			fMaxSpace=fabs(fSpacing);
		}
	}
}
int CLDSBolt::CorrectBoltZnL0Para()
{
	CLDSBolt* pBolt=this;
	return CorrectBoltZnL0Para(&pBolt,1);
}
struct CORRECT_BOLT_L{
	double m_fOldL,m_fOldL0;
	CORRECT_BOLT_L(double L=0,double L0=0){m_fOldL=L;m_fOldL0=L0;}
};
//自动修正螺栓Z向穿插位置及通厚
int CLDSBolt::CorrectBoltZnL0Para(CLDSBolt** ppBolts,UINT count/*=1*/)	//自动修正螺栓Z向穿插位置及通厚
{
	long hCurBolt=0;
	UINT i,nInvalidHits=0;
	if(count==0)
		return 0;
	try{
		IModel* pModel=(ppBolts[0])->BelongModel();
		CLdsPartListStack stack(pModel);
		CHashList<CORRECT_BOLT_L> hashCorrectBolts;
		for(i=0;i<count;i++)
		{
			CLDSBolt *pBolt=ppBolts[i];
			if(strlen(pBolt->statSegStr)==0)
			{	//需要修正
				pBolt->Unlock();	
				pBolt->EmptyL0DesignPara();
				pBolt->des_base_pos.norm_offset.EmptyThick();
				//记录需修正的螺栓
				hashCorrectBolts.SetValue(pBolt->handle,CORRECT_BOLT_L(pBolt->get_L(),pBolt->get_L0()));
			}
			else
				pBolt->Lock();	//多接腿轮换螺栓不参预修正
		}
		//修正螺栓通厚
		for(CLDSPart* pPart=pModel->EnumPartFirst();pPart;pPart=pModel->EnumPartNext())
		{
			CLsRefList *pLsRefList=pPart->GetLsRefList();
			if(pLsRefList)
			{
				for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
				{
					CLDSBolt* pBolt=pLsRef->GetLsPtr();
					if(hashCorrectBolts.GetValue(pBolt->handle))	//构件螺栓引用中有待修正的螺栓
						pBolt->AddL0Thick(pPart->handle,TRUE);
				}
			}
		}
		//修正螺栓Z向偏移
		for(i=0;i<count;i++)
		{
			CLDSBolt *pBolt=ppBolts[i];
			CORRECT_BOLT_L* pCorrect=hashCorrectBolts.GetValue(pBolt->handle);
			if(pCorrect==NULL)
				continue;	//虚拟螺栓仅为打孔用，无实际统计意义，因此也无修正必要wjh-2011.9.3
			PARTSET partset;
			for(THICK_ITEM item=pBolt->GetFirstL0ThickItem();!item.IsFinalKey();item=pBolt->GetNextL0ThickItem())
			{
				if(!item.IsHandle())
					continue;
				CLDSPart* pPart=pBolt->BelongModel()->FromPartHandle(item.val);
				if(pPart)
					partset.append(pPart);
			}
			pBolt->des_base_pos.norm_offset.key_str=CLDSBolt::EmendZOffsetStr(pBolt,partset);
			pBolt->correct_pos();
		}
		for(i=0;i<count;i++)
		{
			CLDSBolt* pBolt=ppBolts[i];
			CORRECT_BOLT_L* pCorrect=hashCorrectBolts.GetValue(pBolt->handle);
			if(pCorrect==NULL)
				continue;	//虚拟螺栓仅为打孔用，无实际统计意义，因此也无修正必要wjh-2011.9.3
			BOOL bRet=pBolt->CalGuigeAuto();
			double L=pCorrect->m_fOldL,L0=pCorrect->m_fOldL0;
			if(L0!=pBolt->get_L0()||L!=pBolt->get_L())
			{
				CXhChar50 boltname;
				if(pBolt->IsFootNail())
#ifdef AFX_TARG_ENU_ENGLISH
					boltname.Printf("%4d#0X%X climbing bolt",nInvalidHits+1,pBolt->handle);
				else
					boltname.Printf("%4d#0X%X bolt",nInvalidHits+1,pBolt->handle);
				if(L0!=pBolt->get_L0())
					pBolt->Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"%s L0 correction:%d->%d，",(char*)boltname,ftoi(L0),ftoi(pBolt->get_L0()));
				else
					pBolt->Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"%s L0:%d，",(char*)boltname,ftoi(L0));
#else
					boltname.Printf("%4d#0X%X脚钉",nInvalidHits+1,pBolt->handle);
				else
					boltname.Printf("%4d#0X%X螺栓",nInvalidHits+1,pBolt->handle);
				if(L0!=pBolt->get_L0())
					pBolt->Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"%s通厚修正:%d->%d，",(char*)boltname,ftoi(L0),ftoi(pBolt->L0));
				else
					pBolt->Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"%s通厚:%d，",(char*)boltname,ftoi(L0));
#endif
				if(bRet)
				{
					if(L!=pBolt->get_L())
#ifdef AFX_TARG_ENU_ENGLISH
						pBolt->Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"Specification has been corrected successfully:M%dX%d->M%dX%d!\n",pBolt->get_d(),ftoi(L),pBolt->get_d(),ftoi(pBolt->get_L()));
					else
						pBolt->Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"Specification:M%dX%d!\n",pBolt->get_d(),ftoi(L));
				}
				else
					pBolt->Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"Fail to correct the bolt specification,the suitable bolt specification for L0 is not found(keep the old specificationM%dX%d)!\n",pBolt->get_d(),ftoi(pBolt->get_L()));
#else
						pBolt->Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"规格成功修正:M%dX%d->M%dX%d!\n",pBolt->get_d(),ftoi(L),pBolt->get_d(),ftoi(pBolt->get_L()));
					else
						pBolt->Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"规格:M%dX%d!\n",pBolt->get_d(),ftoi(L));
				}
				else
					pBolt->Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"螺栓规格修正失败，没有找到符合通厚值的螺栓规格(保留原规格M%dX%d)!\n",pBolt->get_d(),ftoi(pBolt->get_L()));
#endif
				nInvalidHits++;
			}
		}
		return true;
	}
	catch(char* sError)
	{
		CXhChar50 error;
#ifdef AFX_TARG_ENU_ENGLISH
		error.Printf("%s error bolt handle is:0X%X",sError,hCurBolt);
#else
		error.Printf("%s出错螺栓句柄为:0X%X",sError,hCurBolt);
#endif
		return false;
	}
}
int CLDSBolt::CalPadPara(double dist)
{
	return CLDSBolt::CalPadPara(d,dist,&DianQuan);
}
//*pType:0.垫板 1.垫圈
int CLDSBolt::CalPadPara(int d,double dist,DIANQUAN_STRU* pDianQuanInfo/*=NULL*/)
{
	short nBoltPadThick=0;
	DIANQUAN_STRU DianQuan;
	memset(&DianQuan,0,sizeof(DIANQUAN_STRU));
	int nType=DIANQUAN_STRU::DIANQUAN;
	if(library)
	{	
		nBoltPadThick = library->GetBestMatchWasherThick(d, dist);
		if (fabs(dist) < EPS2)
			DianQuan.N = 0;
		else if(dist<(nBoltPadThick*3+1.2))
		{	//大于1个垫圈厚度小于3个垫圈厚度
			nType=DIANQUAN_STRU::DIANQUAN;
			DianQuan.thick = nBoltPadThick;
			DianQuan.N = (int)(dist/DianQuan.thick+0.5);
		}
		else
		{
			nType=DIANQUAN_STRU::DIANBAN;
			DianQuan.N = 0;
			DianQuan.thick = FindPlankThick(dist);
		}
	}
	/*if(d==12)
	{
		if(fabs(dist)<EPS2)
			DianQuan.N = 0;
		else if(dist<(nBoltPadThick*3+1.2))	//dist>nBoltPadThick&&
		{	//大于1个垫圈厚度小于3个垫圈厚度
			nType=DIANQUAN_STRU::DIANQUAN;
			DianQuan.thick = nBoltPadThick;
			DianQuan.N = (int)(dist/nBoltPadThick+0.5);
		}
		else
		{
			nType=DIANQUAN_STRU::DIANBAN;
			DianQuan.thick = FindPlankThick(dist);
		}
	}
	else if(d==16 || d==20 || d==24)
	{
		if(fabs(dist)<EPS2)
			DianQuan.N = 0;
		else if(dist<(nBoltPadThick*2+1.2))	//dist>nBoltPadThick&&
		{	//大于1个垫圈厚度小于2个垫圈厚度
			nType=DIANQUAN_STRU::DIANQUAN;
			DianQuan.thick = nBoltPadThick;
			DianQuan.N = (int)(dist/nBoltPadThick+0.5);
		}
		else
		{
			nType=DIANQUAN_STRU::DIANBAN;
			DianQuan.thick = FindPlankThick(dist);
			//DianQuan.N = 1;
		}
	}*/
	if(pDianQuanInfo&&nType==DIANQUAN_STRU::DIANQUAN)
		*pDianQuanInfo=DianQuan;
	return nType;
}

int CLDSBolt::ValidCheck()
{
	if(des_base_pos.hPart==0||BelongModel()->FromPartHandle(des_base_pos.hPart)==NULL)
		return -1;	//螺栓缺少定位基准构件
	else if(des_base_pos.datumPoint.datum_pos_style==1||des_base_pos.datumPoint.datum_pos_style==7)
	{	//角钢楞点或包铁楞点定位
		if(BelongModel()->FromPartHandle(des_base_pos.datumPoint.des_para.RODEND.hRod)==NULL)
			return -1;	//螺栓位置设计缺少基准构件
	}
	else if(des_base_pos.datumPoint.datum_pos_style==2)
	{	//角钢节点定位
		if(BelongModel()->FromPartHandle(des_base_pos.datumPoint.des_para.RODNODE.hRod)==NULL)
			return -1;	//螺栓位置设计缺少基准杆件
		if(BelongModel()->FromNodeHandle(des_base_pos.datumPoint.des_para.RODNODE.hNode)==NULL)
			return -2;	//螺栓位置设计缺少基准节点
	}
	else if(des_base_pos.datumPoint.datum_pos_style==3)
	{	//角钢心线交点
		if(BelongModel()->FromPartHandle(des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1)==NULL)
			return -1;	//螺栓位置设计缺少基准角钢
		if(BelongModel()->FromPartHandle(des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2)==NULL)
			return -1;	//螺栓位置设计缺少基准角钢
	}
	else if(des_base_pos.datumPoint.datum_pos_style==8)
	{
		if(BelongModel()->FromPartHandle(des_base_pos.datumPoint.des_para.hNode)==NULL)
			return -2;	//螺栓位置设计缺少基准节点
	}
	/*if(des_base_pos.datum_pos==LS_POS_AXIS_INTERS&&	//心线交点
		(BelongModel()->FromPartHandle(des_base_pos.hPart1,CLS_LINEANGLE)==NULL||BelongModel()->FromPartHandle(des_base_pos.hPart2,CLS_LINEANGLE)==NULL))
		return -1;	//螺栓位置设计缺少基准角钢
	else if(des_base_pos.datum_pos==LS_POS_JG_BER&&BelongModel()->FromPartHandle(des_base_pos.hPart1,CLS_LINEANGLE)==NULL)
		return -1;
	else if(des_base_pos.datum_pos==LS_POS_NODE_PRJ&&BelongModel()->FromPartHandle(des_base_pos.hPart1,CLS_LINEANGLE)==NULL)
		return -1;	//
	else if(des_base_pos.datum_pos==LS_POS_NODE_PRJ&&BelongModel()->FromNodeHandle(des_base_pos.hNode)==NULL)
		return -2;	//螺栓位置设计缺少基准节点
	else if((des_base_pos.datum_pos==LS_POS_LS)&&BelongModel()->FromPartHandle(des_base_pos.hLs,CLS_BOLT)==NULL)
		return -3;	//螺栓位置设计缺少基准螺栓
	else */
	if(des_work_norm.norm_style!=0&&BelongModel()->FromPartHandle(des_work_norm.hVicePart)==NULL)
		return -4;	//螺栓法线设计缺少基准构件
	else if(des_work_norm.norm_style==2&&BelongModel()->FromPartHandle(des_work_norm.hCrossPart)==NULL)
		return -4;	//螺栓法线设计缺少基准构件
	else
		return 0;
}

double CLDSBolt::GetWeight(BOOL bSolidLength/*=TRUE*/)
{
	CLDSBolt* pMotherBolt=(CLDSBolt*)MotherPart();
	if(pMotherBolt!=this)
		return pMotherBolt->GetWeight();
	if(library)
		return library->GetLsWeight(d,L,m_hFamily);
	else
		return 0;
}
void CLDSBolt::CreateBoltSolidBody(fBody* pBody,int d,double L,double D,double H)
{
	pBody->Empty();
	int i=0;
	double cosa_arr[6]={1.0,0.5,-0.5,-1.0,-0.5,0.5};
	double sina_arr[6]={0.0,0.8660254037844,0.8660254037844,0.0,-0.8660254037844,-0.8660254037844};
	double D_2=D/2.0;
	double bolt_r=d*0.5;

	f3dPoint vertex_arr[24];
	for(i=0;i<6;i++)
	{
		vertex_arr[i].Set(cosa_arr[i]*D_2,sina_arr[i]*D_2,-H);			//螺栓头顶面
		vertex_arr[6+i].Set(vertex_arr[i].x,vertex_arr[i].y,0);			//螺栓头贴合面
		vertex_arr[12+i].Set(cosa_arr[i]*bolt_r,sina_arr[i]*bolt_r,0);	//螺栓柱贴合面
		vertex_arr[18+i].Set(vertex_arr[12+i].x,vertex_arr[12+i].y,L);	//螺栓柱顶面
	}
	for(i=0;i<24;i++)
		pBody->vertex.append(vertex_arr[i]);
	COLORREF crLs=0;
	crLs=GetColor(CLS_BOLT,d,' ');//根据螺栓直径得到相应的颜色
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);

	//1.螺栓第一面生成(螺栓头顶面)
	f3dPolyFace* pFace = pBody->faceList.append();
	pFace->material = crLs;
	pFace->poly_norm.Set(0,0,-1);
	for(i=0;i<6;i++)
		createFace.NewOutterEdgeLine(pFace,(11-i-1)%6,5-i);
	
	//2.螺栓第二面生成(螺栓头贴合面)
	pFace = pBody->faceList.append();
	pFace->material = crLs;
	pFace->poly_norm.Set(0,0,1);
	for(i=0;i<6;i++)
		createFace.NewOutterEdgeLine(pFace,6+(i+1)%6,6+i);
	
	//3.螺栓柱第三面生成(螺栓柱顶面)
	pFace = pBody->faceList.append();
	pFace->material = crLs;
	pFace->poly_norm.Set(0,0,1);
	for(i=0;i<6;i++)
		createFace.NewOutterEdgeLine(pFace,18+(i+1)%6,18+i);
	
	//4.螺栓头及螺栓柱侧面的生成
	for(i=0;i<6;i++)
	{
		int ni=(i+1)%6;
		//螺栓头侧面
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		createFace.NewOutterEdgeLine(pFace,ni,i);
		createFace.NewOutterEdgeLine(pFace,ni+6,ni);
		createFace.NewOutterEdgeLine(pFace, i+6,ni+6);
		createFace.NewOutterEdgeLine(pFace, i,i+6);
		//螺栓柱侧面
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		createFace.NewOutterEdgeLine(pFace,ni+12,i+12);
		createFace.NewOutterEdgeLine(pFace,ni+18);
		createFace.NewOutterEdgeLine(pFace, i+18);
		createFace.NewOutterEdgeLine(pFace, i+12);
	}
}
void CLDSBolt::CreateBoltCapSolidBody(fBody* pBody,int d,double D,double H2)
{
	pBody->Empty();
	int i=0;
	double cosa_arr[6]={1.0,0.5,-0.5,-1.0,-0.5,0.5};
	double sina_arr[6]={0.0,0.8660254037844,0.8660254037844,0.0,-0.8660254037844,-0.8660254037844};
	double D_2=D/2.0;
	f3dPoint vertex_arr[12];
	for(i=0;i<6;i++)
	{
		vertex_arr[i].Set(cosa_arr[i]*D_2,sina_arr[i]*D_2,0);			//螺帽贴合面
		vertex_arr[6+i].Set(vertex_arr[i].x,vertex_arr[i].y,H2);	//螺帽顶面
	}
	for(i=0;i<12;i++)
		pBody->vertex.append(vertex_arr[i]);
	COLORREF crLs=0;
	crLs=GetColor(CLS_BOLT,d,' ');//根据螺栓直径得到相应的颜色
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);

	//1.螺帽第一面生成(螺帽贴合面)
	f3dPolyFace* pFace = pBody->faceList.append();
	pFace->material = crLs;
	pFace->poly_norm.Set(0,0,-1);
	for(i=0;i<6;i++)
		createFace.NewOutterEdgeLine(pFace,(11-i-1)%6,5-i);
	
	//2.螺栓第二面生成(螺帽顶面)
	pFace = pBody->faceList.append();
	pFace->material = crLs;
	pFace->poly_norm.Set(0,0,1);
	for(i=0;i<6;i++)
		createFace.NewOutterEdgeLine(pFace,6+(i+1)%6,6+i);
	
	//3.螺帽侧面的生成
	for(i=0;i<6;i++)
	{
		int ni=(i+1)%6;
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		createFace.NewOutterEdgeLine(pFace,ni,i);
		createFace.NewOutterEdgeLine(pFace,ni+6,ni);
		createFace.NewOutterEdgeLine(pFace, i+6,ni+6);
		createFace.NewOutterEdgeLine(pFace, i,i+6);
	}
}
typedef f3dPoint* f3dPointPtr;
BOOL CLDSBolt::CreateClimbNailSolidModel(fBody* pBody,double scale_of_user2scr,double sample_len,int smoothness/*=36*/)
{
  	double D;     	  		// 螺帽直径.
  	double H;				// 螺栓头高度.
	double H2;				// 螺帽高度.
	double dfTheftCapH=0;	// 防盗螺母高度
	double dfThinCapH=0;	// 薄螺母高度
	FindLsD_H(d,&D,&H,&H2);
	CLsFamily* pNailFamily=CLsLibrary::FromHandle(m_hFamily);
	if(!pNailFamily->m_bFootNail)
		return false;
	LS_XING_HAO* pBoltSize=pNailFamily->FindLsGuiGeByValidLen(this->d,this->L);
	int liNoneThreadLen=(pBoltSize!=NULL)?pBoltSize->no_thread_len:120;
	int liThreadLen=ftoi(this->L)-liNoneThreadLen;
	int i=0;
	for(i=0;i<8;i++)
	{
		double dfDe=0;
		if(pNailFamily->ls_sub_part[i].idSubPart==CLsSubPartFamily::TYPE_ANTITHEFT_NUT)	//防盗系列
			FindBoltCapD_H(d,&dfDe,&dfTheftCapH,CLsSubPartFamily::TYPE_ANTITHEFT_NUT);
		else if(pNailFamily->ls_sub_part[i].idSubPart==CLsSubPartFamily::TYPE_THIN_NUT)	//薄螺母系列
			FindBoltCapD_H(d,&dfDe,&dfThinCapH,CLsSubPartFamily::TYPE_THIN_NUT);
	}
	double cosa_arr[6]={1.0,0.5,-0.5,-1.0,-0.5,0.5};
	double sina_arr[6]={0.0,0.8660254037844,0.8660254037844,0.0,-0.8660254037844,-0.8660254037844};
	double D_2=D/2.0;
    f3dPoint vertex_arr[60];
	//    ___    ___
	//   _| |****| |_________  
	//  |///O→→→→→→→→|→脚钉正法线方向,
	double dfOffsetLen=liThreadLen-H2-this->L0;
	for(i=0;i<6;i++)
	{
		vertex_arr[i].Set(cosa_arr[i]*D_2,sina_arr[i]*D_2,-H2);
		vertex_arr[6+i].Set(vertex_arr[i].x,vertex_arr[i].y,0);
		vertex_arr[12+i].Set(cosa_arr[i]*d/2.0,sina_arr[i]*d/2.0,-dfOffsetLen);
		vertex_arr[18+i].Set(vertex_arr[12+i].x,vertex_arr[12+i].y,L-dfOffsetLen);
		vertex_arr[24+i].Set(vertex_arr[i].x,vertex_arr[i].y,L0);
		vertex_arr[30+i].Set(vertex_arr[i].x,vertex_arr[i].y,L0+H2);
		if(dfThinCapH>0)	//带薄螺母双帽
			vertex_arr[i].z=-H2-dfThinCapH;
		else if(IsTwoCap()&&dfTheftCapH<=0)		//普通双螺帽,添加dfTheftCapH<=0是为了防止误把防盗识别为双帽
			vertex_arr[i].z=-H2*2;
	}
	//垫圈实体
	if(/*m_bHaveSpringPad||*/DianQuan.N>0)
	{
		double R=15;
		double thick=DianQuan.thick*DianQuan.N;
		if(d==12)
			R=12;
		else if(d==16)
			R=15;
		else if(d==20)
			R=18.5;
		else if(d==24)
			R=22;
		for(i=0;i<12;i++)
		{
			vertex_arr[i+36].Set(R*cos(i*Pi/6),R*sin(i*Pi/6),DianQuan.offset);
			vertex_arr[i+48].Set(vertex_arr[i+36].x,vertex_arr[i+36].y,DianQuan.offset+thick);
		}
	}
	f3dPolyFace *pFace;
	f3dPoint *pVertice;
	for(i=0;i<42;i++)
		pVertice=pBody->vertex.append(vertex_arr[i]);
	if(/*m_bHaveSpringPad&&*/DianQuan.N>0)
	{
		for(int i=36;i<60;i++)
			pVertice=pBody->vertex.append(vertex_arr[i]);
	}
	COLORREF crLs=0;
	crLs=GetColor(CLS_BOLT,d,' ');//根据螺栓直径得到相应的颜色
	if(m_bUserSpecColor)	//用户指定构件颜色
		crLs=crMaterial;
	/*if(IsShadowPart())
	{
		BYTE* color=(BYTE*)&crLs;
		for(int i=0;i<3;i++)
		{	
			color[i]/=3;
			color[i]+=color[i];
		}
	}*/
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);

	//1.螺栓第一面生成(螺栓头底面)
	if(!IsAntiTheft()||dfTheftCapH<=0)
	{	//含防盗螺母时螺栓头底面与防盗螺母顶面重合且内嵌，不需生成 wjh-2019.4.28
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		pFace->poly_norm=-ucs.axis_z;
		for(i=0;i<6;i++)
			createFace.NewOutterEdgeLine(pFace,(11-i-1)%6,5-i);
	}
	
	//2.螺栓第二面生成(螺栓头顶面)
	pFace = pBody->faceList.append();
	pFace->material = crLs;
	pFace->poly_norm=ucs.axis_z;
	for(i=0;i<6;i++)
		createFace.NewOutterEdgeLine(pFace,6+(i+1)%6,6+i);
	
	//3.螺栓第三面生成
	pFace = pBody->faceList.append();
	pFace->material = crLs;
	pFace->poly_norm=ucs.axis_z;
	for(i=0;i<6;i++)
		createFace.NewOutterEdgeLine(pFace,18+(i+1)%6,18+i);
	
	//4.螺栓第四面生成		--螺帽底面
	pFace = pBody->faceList.append();
	pFace->material = crLs;
	pFace->poly_norm=-ucs.axis_z;
	createFace.NewOutterEdgeLine(pFace,28,29);
	createFace.NewOutterEdgeLine(pFace,27,28);
	createFace.NewOutterEdgeLine(pFace,26,27);
	createFace.NewOutterEdgeLine(pFace,25,26);
	createFace.NewOutterEdgeLine(pFace,24,25);
	createFace.NewOutterEdgeLine(pFace,29,24);
	//5.螺栓第五面生成		--螺帽顶面
	pFace = pBody->faceList.append();
	pFace->material = crLs;
	pFace->poly_norm=ucs.axis_z;
	createFace.NewOutterEdgeLine(pFace,30,31);
	createFace.NewOutterEdgeLine(pFace,31,32);
	createFace.NewOutterEdgeLine(pFace,32,33);
	createFace.NewOutterEdgeLine(pFace,33,34);
	createFace.NewOutterEdgeLine(pFace,34,35);
	createFace.NewOutterEdgeLine(pFace,35,30);
	//6.螺栓头及螺帽侧面的生成
	f3dPolyFace* pEndFace=pBody->faceList.append();
	pEndFace->material = crLs;
	for(i=0;i<6;i++)
	{
		int k=(i+1)%6;
		//螺栓头底面
		createFace.NewOutterEdgeLine(pEndFace,17-k,17-i);
		//螺栓头侧面
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		createFace.NewOutterEdgeLine(pFace,k,i);
		createFace.NewOutterEdgeLine(pFace,k+6,k);
		createFace.NewOutterEdgeLine(pFace,i+6,k+6);
		createFace.NewOutterEdgeLine(pFace,i,i+6);
		//螺帽侧面
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		createFace.NewOutterEdgeLine(pFace,k+24,i+24);
		createFace.NewOutterEdgeLine(pFace,k+30,k+24);
		createFace.NewOutterEdgeLine(pFace,i+30,k+30);
		createFace.NewOutterEdgeLine(pFace,i+24,i+30);
	}
	//7.螺栓柱侧面的生成
	for(i=12;i<18;i++)
	{
		int k=i+1;
		if(k==18)
			k=12;
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		createFace.NewOutterEdgeLine(pFace,k,i);
		createFace.NewOutterEdgeLine(pFace,k+6,k);
		createFace.NewOutterEdgeLine(pFace,i+6,k+6);
		createFace.NewOutterEdgeLine(pFace,i  ,i+6);
	}
	
	if(/*m_bHaveSpringPad&&*/DianQuan.N>0)
	{	//有垫圈时的垫圈实体
		COLORREF crPlate = GetColor(CLS_PLATE,-1,' ');
		//垫圈底面
		pFace = pBody->faceList.append();
		pFace->material = crPlate;
		createFace.NewOutterEdgeLine(pFace,36,37);
		int j=0;
		for(j=47;j>36;j--)
			createFace.NewOutterEdgeLine(pFace,j);
		//垫圈顶面
		pFace = pBody->faceList.append();
		pFace->material = crPlate;
		createFace.NewOutterEdgeLine(pFace,48,59);
		for(j=49;j<60;j++)
			createFace.NewOutterEdgeLine(pFace,j);
		//垫圈侧面
		for(j=0;j<12;j++)
		{
			pFace = pBody->faceList.append();
			pFace->material = crPlate;
			createFace.NewOutterEdgeLine(pFace,(j+1)%12+36,j+36);
			createFace.NewOutterEdgeLine(pFace,(j+1)%12+48);
			createFace.NewOutterEdgeLine(pFace,j+48);
			createFace.NewOutterEdgeLine(pFace,j+36);
		}
	}
	if(IsAntiTheft()&&dfTheftCapH>0)//含防盗螺母
	{
		GECS acs;
		acs.InitStdCS();
		acs.origin.z=-H2-dfTheftCapH;
		COLORREF crTheftColor=	GetColor(CLS_BOLT,-1,' ');//根据螺栓直径得到相应的颜色
		//0x06=NUT_INC_BTM|NUT_INC_SIDE;// = 0x06;
		BOOL pushed=pBody->faceList.PushStack();
		METASOLID::AppendHexPolygonNutSolid(pBody,D,dfTheftCapH,acs,crTheftColor,0x06);
		if(!pBody->faceList.PopStack(pushed))
			logerr.Log("脚钉0x%X实体面生成时出栈错误",handle);
		for(pFace=pBody->faceList.GetNext();pFace;pFace=pBody->faceList.GetNext())
		{
			if(!pFace->poly_norm.IsZero())
			{
				if(!vector_trans(pFace->poly_norm,ucs,TRUE,TRUE))
					logerr.Log("脚钉0x%X的装配坐标系有错误",handle);
			}
		}
	}
	return TRUE;
}
BOOL CLDSBolt::Create3dSolidModel(BOOL bDisplayHole,double scale_of_user2scr,double sample_len,int smoothness/*=36*/)
{
	if(CreateShadowPart3dSolidModel(bDisplayHole,scale_of_user2scr,sample_len,smoothness))
		return TRUE;

    f3dPoint vertex_arr[60];
	if(pSolidBody==NULL)
		pSolidBody = new CSolidBody;
	else if(!IsSolidModified())	//数据未修改
		return TRUE;
	else
		pSolidBody->Empty();
	is_solid_modified = FALSE;
	fBody body;
	fBody *pBody=&body;
	GetUCS();
	if(this->IsFootNail()&&CreateClimbNailSolidModel(pBody,scale_of_user2scr,sample_len,smoothness))
	{	//脚钉实体需要单独生成
		for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
		{
			if(coord_trans(*pVertice,ucs,TRUE,TRUE)!=TRUE)
				return FALSE;
		}
		pSolidBody->ConvertFrom(pBody);
		return true;
	}
  	double D;     	  		// 螺帽直径.
  	double H;				// 螺栓头高度.
	double H2;				// 螺帽高度.
	double dfTheftCapH=0;	// 防盗螺母高度
	double dfThinCapH=0;	// 薄螺母高度
	FindLsD_H(d,&D,&H,&H2);
	CLsFamily* pBoltFamily=CLsLibrary::FromHandle(m_hFamily);
	if(pBoltFamily==NULL)
		pBoltFamily=CLsLibrary::DefaultFamily();
	int i=0;
	for(i=0;pBoltFamily!=NULL&&i<8;i++)
	{
		double dfDe=0;
		if(pBoltFamily->ls_sub_part[i].idSubPart==CLsSubPartFamily::TYPE_ANTITHEFT_NUT)	//防盗系列
			FindBoltCapD_H(d,&dfDe,&dfTheftCapH,CLsSubPartFamily::TYPE_ANTITHEFT_NUT);
		else if(pBoltFamily->ls_sub_part[i].idSubPart==CLsSubPartFamily::TYPE_THIN_NUT)	//防盗系列
			FindBoltCapD_H(d,&dfDe,&dfThinCapH,CLsSubPartFamily::TYPE_THIN_NUT);
	}
	double cosa_arr[6]={1.0,0.5,-0.5,-1.0,-0.5,0.5};
	double sina_arr[6]={0.0,0.8660254037844,0.8660254037844,0.0,-0.8660254037844,-0.8660254037844};
	double D_2=D/2.0;
	for(i=0;i<6;i++)
	{
		vertex_arr[i].Set(cosa_arr[i]*D_2,sina_arr[i]*D_2,-H);
		vertex_arr[6+i].Set(vertex_arr[i].x,vertex_arr[i].y,0);
		vertex_arr[12+i].Set(cosa_arr[i]*d/2.0,sina_arr[i]*d/2.0,0);
		vertex_arr[18+i].Set(vertex_arr[12+i].x,vertex_arr[12+i].y,L);
		vertex_arr[24+i].Set(vertex_arr[i].x,vertex_arr[i].y,L0);
		if(dfTheftCapH>0)		//防盗螺母后续有单独的防盗帽实体添加代码
			vertex_arr[30+i].Set(vertex_arr[i].x,vertex_arr[i].y,L0+H2);
		else if(dfThinCapH>0)	//带薄螺母双帽
			vertex_arr[30+i].Set(vertex_arr[i].x,vertex_arr[i].y,L0+H2+dfThinCapH);
		else if(IsTwoCap())		//普通双螺帽
			vertex_arr[30+i].Set(vertex_arr[i].x,vertex_arr[i].y,L0+H2+H2);
		else
			vertex_arr[30+i].Set(vertex_arr[i].x,vertex_arr[i].y,L0+H2);
	}
	//垫圈实体
	if(/*m_bHaveSpringPad||*/DianQuan.N>0)
	{
		double R=15;
		double thick=DianQuan.thick*DianQuan.N;
		if(d==12)
			R=12;
		else if(d==16)
			R=15;
		else if(d==20)
			R=18.5;
		else if(d==24)
			R=22;
		for(i=0;i<12;i++)
		{
			vertex_arr[i+36].Set(R*cos(i*Pi/6),R*sin(i*Pi/6),DianQuan.offset);
			vertex_arr[i+48].Set(vertex_arr[i+36].x,vertex_arr[i+36].y,DianQuan.offset+thick);
		}
	}
	f3dPolyFace *pFace;
	f3dPoint *pVertice;
	for(i=0;i<36;i++)
		pVertice=pBody->vertex.append(vertex_arr[i]);
	if(/*m_bHaveSpringPad&&*/DianQuan.N>0)
	{
		for(int i=36;i<60;i++)
			pVertice=pBody->vertex.append(vertex_arr[i]);
	}
	COLORREF crLs=0;
	crLs=GetColor(CLS_BOLT,d,' ');//根据螺栓直径得到相应的颜色
	if(m_bUserSpecColor)	//用户指定构件颜色
		crLs=crMaterial;
	/*if(IsShadowPart())
	{
		BYTE* color=(BYTE*)&crLs;
		for(int i=0;i<3;i++)
		{	
			color[i]/=3;
			color[i]+=color[i];
		}
	}*/
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);

	//1.螺栓第一面生成
	pFace = pBody->faceList.append();
	pFace->material = crLs;
	pFace->poly_norm=-ucs.axis_z;
	for(i=0;i<6;i++)
		createFace.NewOutterEdgeLine(pFace,(11-i-1)%6,5-i);
	
	//2.螺栓第二面生成
	pFace = pBody->faceList.append();
	pFace->material = crLs;
	pFace->poly_norm=ucs.axis_z;
	for(i=0;i<6;i++)
		createFace.NewOutterEdgeLine(pFace,6+(i+1)%6,6+i);
	
	//3.螺栓第三面生成
	pFace = pBody->faceList.append();
	pFace->material = crLs;
	pFace->poly_norm=ucs.axis_z;
	for(i=0;i<6;i++)
		createFace.NewOutterEdgeLine(pFace,18+(i+1)%6,18+i);
	
	//4.螺栓第四面生成		--螺帽底面
	pFace = pBody->faceList.append();
	pFace->material = crLs;
	pFace->poly_norm=-ucs.axis_z;
	if (IsAntiTheft() && dfTheftCapH == 0)
	{	//防盗螺栓螺栓帽以三角形显示
		createFace.NewOutterEdgeLine(pFace, 27, 29);
		createFace.NewOutterEdgeLine(pFace, 25, 27);
		createFace.NewOutterEdgeLine(pFace, 29, 25);
	}
	else
	{
		createFace.NewOutterEdgeLine(pFace, 28, 29);
		createFace.NewOutterEdgeLine(pFace, 27, 28);
		createFace.NewOutterEdgeLine(pFace, 26, 27);
		createFace.NewOutterEdgeLine(pFace, 25, 26);
		createFace.NewOutterEdgeLine(pFace, 24, 25);
		createFace.NewOutterEdgeLine(pFace, 29, 24);
	}
	//5.螺栓第五面生成		--螺帽顶面
	if (dfTheftCapH <= 0)
	{	//含防盗螺母时普通螺母顶面与防盗螺母顶面重合且内嵌，不需生成 wjh-2019.4.28
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		pFace->poly_norm = ucs.axis_z;
		if (IsAntiTheft() && dfTheftCapH == 0)
		{	//防盗螺栓,但未特别指定防盗规格时，螺栓帽以三角形显示 wjh-2019.5.22
			createFace.NewOutterEdgeLine(pFace, 35, 31);
			createFace.NewOutterEdgeLine(pFace, 31, 33);
			createFace.NewOutterEdgeLine(pFace, 33, 35);
		}
		else
		{
			createFace.NewOutterEdgeLine(pFace, 30, 31);
			createFace.NewOutterEdgeLine(pFace, 31, 32);
			createFace.NewOutterEdgeLine(pFace, 32, 33);
			createFace.NewOutterEdgeLine(pFace, 33, 34);
			createFace.NewOutterEdgeLine(pFace, 34, 35);
			createFace.NewOutterEdgeLine(pFace, 35, 30);
		}
	}
	//6.螺栓头及螺帽侧面的生成
	for(i=0;i<6;i++)
	{
		int k=(i+1)%6;
		//螺栓头侧面
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		createFace.NewOutterEdgeLine(pFace,k,i);
		createFace.NewOutterEdgeLine(pFace,k+6,k);
		createFace.NewOutterEdgeLine(pFace,i+6,k+6);
		createFace.NewOutterEdgeLine(pFace,i,i+6);
		//螺帽侧面
		if (IsAntiTheft() && dfTheftCapH == 0)
		{	//防盗螺栓螺栓帽以三角形显示
			k = (i + 2) % 6;
			if (i % 2 == 0)	//偶数号顶点不显示
				continue;
		}
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		createFace.NewOutterEdgeLine(pFace,k+24,i+24);
		createFace.NewOutterEdgeLine(pFace,k+30,k+24);
		createFace.NewOutterEdgeLine(pFace,i+30,k+30);
		createFace.NewOutterEdgeLine(pFace,i+24,i+30);
	}
	//7.螺栓柱侧面的生成
	for(i=12;i<18;i++)
	{
		int k=i+1;
		if(k==18)
			k=12;
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		createFace.NewOutterEdgeLine(pFace,k,i);
		createFace.NewOutterEdgeLine(pFace,k+6,k);
		createFace.NewOutterEdgeLine(pFace,i+6,k+6);
		createFace.NewOutterEdgeLine(pFace,i  ,i+6);
	}
	
	if(/*m_bHaveSpringPad&&*/DianQuan.N>0)
	{	//有垫圈时的垫圈实体
		COLORREF crPlate = GetColor(CLS_PLATE,-1,' ');
		//垫圈底面
		pFace = pBody->faceList.append();
		pFace->material = crPlate;
		createFace.NewOutterEdgeLine(pFace,36,37);
		int j=0;
		for(j=47;j>36;j--)
			createFace.NewOutterEdgeLine(pFace,j);
		//垫圈顶面
		pFace = pBody->faceList.append();
		pFace->material = crPlate;
		createFace.NewOutterEdgeLine(pFace,48,59);
		for(j=49;j<60;j++)
			createFace.NewOutterEdgeLine(pFace,j);
		//垫圈侧面
		for(j=0;j<12;j++)
		{
			pFace = pBody->faceList.append();
			pFace->material = crPlate;
			createFace.NewOutterEdgeLine(pFace,(j+1)%12+36,j+36);
			createFace.NewOutterEdgeLine(pFace,(j+1)%12+48);
			createFace.NewOutterEdgeLine(pFace,j+48);
			createFace.NewOutterEdgeLine(pFace,j+36);
		}
	}
	if(IsAntiTheft()&&dfTheftCapH>0)//含防盗螺母
	{
		GECS acs;
		acs.InitStdCS();
		acs.origin.z=L0+H2;
		COLORREF crTheftColor=	GetColor(CLS_BOLT,-1,' ');//根据螺栓直径得到相应的颜色
		//0x05=NUT_INC_TOP|NUT_INC_SIDE;// = 0x04;
		BOOL facepushed=pBody->faceList.PushStack();
		METASOLID::AppendHexPolygonNutSolid(pBody,D,dfTheftCapH,acs,crTheftColor,0x05);
		if(!pBody->faceList.PopStack(facepushed))
			logerr.Log("螺栓0x%X实体面生成时出栈错误",handle);
		for(pFace=pBody->faceList.GetNext();pFace;pFace=pBody->faceList.GetNext())
		{
			if(!pFace->poly_norm.IsZero())
			{
				if(!vector_trans(pFace->poly_norm,ucs,TRUE,TRUE))
					logerr.Log("螺栓0x%X的装配坐标系有错误",handle);
			}
		}
	}
	for(pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
	{
		if(coord_trans(*pVertice,ucs,TRUE,TRUE)!=TRUE)
			return FALSE;
	}
	pSolidBody->ConvertFrom(pBody);
	return TRUE;
}

void CLDSBolt::CloneTo(CLDSBolt &bolt)
{
	CBuffer buffer;
	ToBuffer(buffer);
	buffer.SeekToBegin();
	bolt.FromBuffer(buffer);
}

bool CLDSBolt::ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType/*=0*/)
{
	bool replaced=replacer.VerifyAndReplace(&handle,true);
	if(des_base_pos.hPart==replacer.hOld)
	{
		replacer.SelectPerfectPartHandle(ucs.origin);
		if(replacer.IsReverseWingXAndWingY())
			des_base_pos.offset_wing=(des_base_pos.offset_wing==1)?0:1;
	}
	if(idClassType==0||idClassType!=CLS_NODE)
		replaced|=replacer.VerifyAndReplace(&des_base_pos.hPart);
	replaced|=des_base_pos.norm_offset.ReplaceReferenceHandle(replacer);
	replaced|=des_base_pos.datumPoint.ReplaceReferenceHandle(replacer,ucs.origin,idClassType);
	replaced|=des_work_norm.ReplaceReferenceHandle(replacer);
	replaced|=designL0.ReplaceReferenceHandle(replacer);
	for(RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
		replaced|=replacer.VerifyAndReplace(&pRelaObj->hObj);
	if(replaced)
		SetModified();
	return replaced;
}
long CLDSBolt::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	long nInitCount = pListHandlers->Count;
	pListHandlers->Append(HANDLE_ADDR(&handle));
	pListHandlers->Append(HANDLE_ADDR(&des_base_pos.hPart));
	des_base_pos.norm_offset.StatObjHandleReference(pListHandlers);
	des_base_pos.datumPoint.StatObjHandleReference(pListHandlers);
	des_work_norm.StatObjHandleReference(pListHandlers);
	designL0.StatObjHandleReference(pListHandlers);
	for (RELATIVE_OBJECT *pRelaObj = relativeObjs.GetFirst(); pRelaObj; pRelaObj = relativeObjs.GetNext())
		pListHandlers->Append(HANDLE_ADDR(&pRelaObj->hObj));
	return pListHandlers->Count-nInitCount;
}
#endif