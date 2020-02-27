#include "stdafx.h"
#include <math.h>
#include "f_ent.h"
#include "f_alg_fun.h"
#include "HashTable.h"
#include "TrunkSegment.h"
#include "LogFile.h"


//�¶Ȧ�x(��y)/��z
double PLAN_SECTION::set_dfSlopeGradient(double dfGradient)
{
	if(_dfSlopeGradient>=0)
	{
		_dfSlopeGradient=dfGradient;
		_dfZp2Zh=sqrt(1+dfSlopeGradient*dfSlopeGradient);
		if(_wiSegHeight>0&&!IsLegSegment())
		{
			double dfHalfDetaX=dfSlopeGradient*_wiSegHeight;
			double _dfBtmWidth=this->xTopPlane.dfWidth+dfHalfDetaX*2;
		}
	}
	return _dfSlopeGradient;
}
long PLAN_SECTION::set_ziBtmPlane(long ziPlane)
{	//��������ƽ��Z����
	return this->xBtmPlane.ziPlane=ziPlane;
}
double PLAN_SECTION::set_dfBtmWidth(double dfWidth)
{	//������Ƹ���
	return this->xBtmPlane.dfWidth=dfWidth;
}
TRUNK_SEGMENT::TRUNK_SEGMENT(double dfTopTrunkWidth/*=4500*/,double dfGradientCoef/*=0.11*/)
{
	_ciLegSerial=0;	//����
	idPanelType=0;
	xTopPlane.ziPlane=0;
	//ԭʼ������Ʋ���
	_wiSegHeight=4000;
	_dfSlopeGradient=dfGradientCoef;
	xTopPlane.dfWidth=dfTopTrunkWidth;
	//Zp��ʾ����ߣ�Zh��ʾ��ά�ռ��
	_dfZp2Zh=1.00603180864;//sqrt(1+dfSlopeGradient*dfSlopeGradient);
	xBtmPlane.dfWidth=5380;//_dfTopWidth+2*dfSlopeGradient*_wiSegHeight;
	cfgword.Clear();
}
//����ڼ䴹��
WORD TRUNK_SEGMENT::set_wiSegHeight(WORD wiHeight)
{
	if(wiHeight==_wiSegHeight||wiHeight==0)
		return wiHeight;
	_wiSegHeight=wiHeight;
	if(_dfSlopeGradient>=0&&_wiSegHeight>0)
	{
		double dfHalfDetaX=dfSlopeGradient*_wiSegHeight;
		this->xBtmPlane.dfWidth=this->xTopPlane.dfWidth+dfHalfDetaX*2;
		this->xBtmPlane.ziPlane=this->xTopPlane.ziPlane+_wiSegHeight;
	}
	return _wiSegHeight;
}
double TRUNK_SEGMENT::set_dfTopWidth(double dfWidth)
{
	if(dfWidth==xTopPlane.dfWidth||dfWidth==0)
		return xTopPlane.dfWidth;
	xTopPlane.dfWidth=dfWidth;
	if(_dfSlopeGradient>=0&&_wiSegHeight>0)
	{
		double dfHalfDetaX=dfSlopeGradient*_wiSegHeight;
		double _dfBtmWidth=this->xTopPlane.dfWidth+dfHalfDetaX*2;
	}
	return xTopPlane.dfWidth;
}
//����ˮƽ�н�
double TRUNK_SEGMENT::get_dfBevelGradDegAng()
{
	return 0;
}

double TRUNK_SEGMENT::CalBevelGradDegAngle()		//���ҹ�ͨ����ˮƽ�н�
{
	//Zp��ʾ����ߣ�Zh��ʾ��ά�ռ��
	double dfZp2Zh=_dfZp2Zh;//sqrt(1+dfSlopeGradient*dfSlopeGradient);
	double dfSlopeHeight=wiSegHeight*dfZp2Zh;
	double dfHalfDetaX=(xBtmPlane.dfWidth-xTopPlane.dfWidth)*0.5;
	double dfDegAngle=atan(dfSlopeHeight/(xTopPlane.dfWidth+dfHalfDetaX))*DEGTORAD_COEF;
	return dfDegAngle;
}
double TRUNK_SEGMENT::CalUpHalfBevelGradDegAngle()
{	//�ϲ���ͨ����K�ֲ��ģ�����ˮƽ�н�
	//Zp��ʾ����ߣ�Zh��ʾ��ά�ռ��
	double dfZp2Zh=_dfZp2Zh;//sqrt(1+dfSlopeGradient*dfSlopeGradient);
	double dfSlopeHeight=wiSegHeight*dfZp2Zh;
	double dfDegAngle=atan((dfSlopeHeight+dfSlopeHeight)/this->dfBtmWidth)*DEGTORAD_COEF;
	return dfDegAngle;
}
double TRUNK_SEGMENT::CalDownHalfBevelGradDegAngle()
{	//�²���ͨ����K�ֲ��ģ�����ˮƽ�н�
	//Zp��ʾ����ߣ�Zh��ʾ��ά�ռ��
	double dfZp2Zh=_dfZp2Zh;//sqrt(1+dfSlopeGradient*dfSlopeGradient);
	double dfSlopeHeight=wiSegHeight*dfZp2Zh;
	double dfDegAngle=atan((dfSlopeHeight+dfSlopeHeight)/dfTopWidth)*DEGTORAD_COEF;
	return dfDegAngle;
}
bool TRUNK_SEGMENT::CalWholeSectHeight(double* pfMinHeight,double* pfMaxHeight,double dfMinBevelDegAngle/*=25*/,double dfMaxBevelDegAngle/*=40*/)
{
	//���ҹ�ͨ����ˮƽ�н�
	//Zp��ʾ����ߣ�Zh��ʾ��ά�ռ��
	double dfZh2Zp=1.0/_dfZp2Zh;//sqrt(1+dfSlopeGradient*dfSlopeGradient);
	double dfMinTanBevelAngle=tan(dfMinBevelDegAngle*RADTODEG_COEF);
	double dfMaxTanBevelAngle=tan(dfMaxBevelDegAngle*RADTODEG_COEF);
	//  (dfTopWidth+dfHalfDetaX)/dfSlopeHeight<=1/dfMinTanBevelAngle;
	//->dfTopWidth/dfSlopeHeight+dfSlopeGradient/dfZp2Zh<=1/dfMinTanBevelAngle;
	//->dfSlopeHeight>=dfTopWidth/(1/dfMinTanBevelAngle-dfSlopeGradient/dfZp2Zh)
	double dfMinHeight=dfZh2Zp*xTopPlane.dfWidth/(1/dfMinTanBevelAngle-dfSlopeGradient*dfZh2Zp);
	double dfMaxHeight=dfZh2Zp*xTopPlane.dfWidth/(1/dfMaxTanBevelAngle-dfSlopeGradient*dfZh2Zp);
	if(pfMinHeight)
		*pfMinHeight=dfMinHeight;
	if(pfMaxHeight)
		*pfMaxHeight=dfMaxHeight;
	return true;
}
bool TRUNK_SEGMENT::CalUpHalfSectHeight(double* pfMinHeight,double* pfMaxHeight,double dfMinBevelDegAngle/*=25*/,double dfMaxBevelDegAngle/*=40*/)
{
	//Zp��ʾ����ߣ�Zh��ʾ��ά�ռ��
	double dfZh2Zp=1.0/_dfZp2Zh;//sqrt(1+dfSlopeGradient*dfSlopeGradient);
	double dfMinTanBevelAngle=tan(dfMinBevelDegAngle*RADTODEG_COEF);
	double dfMaxTanBevelAngle=tan(dfMaxBevelDegAngle*RADTODEG_COEF);
	//  (0.5*dfTopWidth+dfHalfDetaX)/dfSlopeHeight<=1/dfMinTanBevelAngle;
	//-> 0.5*dfTopWidth/dfSlopeHeight+dfSlopeGradient/dfZp2Zh<=1/dfMinTanBevelAngle;
	//->dfSlopeHeight>=0.5*dfTopWidth/(1/dfMinTanBevelAngle-dfSlopeGradient/dfZp2Zh)
	double dfMinHeight=dfZh2Zp*0.5*xTopPlane.dfWidth/(1/dfMinTanBevelAngle-dfSlopeGradient*dfZh2Zp);
	double dfMaxHeight=dfZh2Zp*0.5*xTopPlane.dfWidth/(1/dfMaxTanBevelAngle-dfSlopeGradient*dfZh2Zp);
	if(pfMinHeight)
		*pfMinHeight=dfMinHeight;
	if(pfMaxHeight)
		*pfMaxHeight=dfMaxHeight;
	return true;
}
bool TRUNK_SEGMENT::CalDownHalfSectHeight(double* pfMinHeight,double* pfMaxHeight,double dfMinBevelDegAngle/*=25*/,double dfMaxBevelDegAngle/*=40*/)
{
	//Zp��ʾ����ߣ�Zh��ʾ��ά�ռ��
	double dfZh2Zp=1.0/_dfZp2Zh;//sqrt(1+dfSlopeGradient*dfSlopeGradient);
	double dfMinTanBevelAngle=tan(dfMinBevelDegAngle*RADTODEG_COEF);
	double dfMaxTanBevelAngle=tan(dfMaxBevelDegAngle*RADTODEG_COEF);
	//  dfSlopeHeight/(0.5*dfTopWidth)>=dfMinTanBevelAngle;
	//->dfSlopeHeight>=0.5*dfTopWidth*dfMinTanBevelAngle
	double dfMinHeight=dfZh2Zp*0.5*xTopPlane.dfWidth*dfMinTanBevelAngle;
	double dfMaxHeight=dfZh2Zp*0.5*xTopPlane.dfWidth*dfMaxTanBevelAngle;
	if(pfMinHeight)
		*pfMinHeight=dfMinHeight;
	if(pfMaxHeight)
		*pfMaxHeight=dfMaxHeight;
	return true;
}
TRUNK_SUBLEG::TRUNK_SUBLEG()
{
	this->idPanelType=SECT_SUBLEG;
	this->_ciLegSerial=1;	//>=1��ʾ���ȷֶνڼ�
}
HEIGHTGROUP::HEIGHTGROUP()
{
	hRelaHeightModule=0;
	_ciBodySerial=1;
	dfSlopeGradient=0.11;
	ziCurrPlane=0;
	xSublegInfo.xFront.pLeftNode=xSublegInfo.xFront.pRightNode=NULL;
	xSublegInfo.xRight=xSublegInfo.xBack=xSublegInfo.xLeft=xSublegInfo.xFront;
}
BYTE HEIGHTGROUP::set_ciBodySerial(BYTE ciBodyNo)
{
	if(ciBodyNo>0&&ciBodyNo<=24)
		_ciBodySerial=ciBodyNo;	//1 based serial
	return _ciBodySerial;
}
WORD HEIGHTGROUP::CalTopPlane(WORD wnLegCount,WORD wiLevelDiff,HEIGHTGROUP::LEGINFO* pxLegInfo)
{
	//Zp��ʾ����ߣ�Zh��ʾ��ά�ռ��
	double dfZp2Zh=sqrt(1+dfSlopeGradient*dfSlopeGradient);
	double dfZh2Zp=1.0/dfZp2Zh;
	double dfMaxZp2WxIsDegA20=tan(70*RADTODEG_COEF);	//ʵ��LVб�������ļнǴ���20�㣬����ctan(��)<=ctan(20��)=tan_deg70
	double dfMinZp2WxIsDegB30=tan(30*RADTODEG_COEF);	//ʵ��LVб��ˮƽ�нǴ���30�㣬��tan(��)>=tan(30��)
	double halfwidth=xBtmPlane.dfWidth*0.5;
	xSublegInfo.wiMaxLegHeight=(WORD)(halfwidth*dfMaxZp2WxIsDegA20*dfZh2Zp);
	xSublegInfo.wiMaxLegHeight-=xSublegInfo.wiMaxLegHeight%500;	//��ȸ�һ��Ϊ0.5m������
	xSublegInfo.ziMinTopPlane=xBtmPlane.ziPlane-xSublegInfo.wiMaxLegHeight;
	double dfShortestLegCoordZ=xBtmPlane.ziPlane-wiLevelDiff*(wnLegCount-1);
	double dfShortestLegBtmWidth=xBtmPlane.dfWidth-2*dfSlopeGradient*(wiLevelDiff*(wnLegCount-1));
	xSublegInfo.ziMaxTopPlane=(int)(dfShortestLegCoordZ-(dfShortestLegBtmWidth*0.5)*dfMinZp2WxIsDegB30);
	xSublegInfo.ziMaxTopPlane-=xSublegInfo.ziMaxTopPlane%500;
	if(xSublegInfo.ziMaxTopPlane<xSublegInfo.ziMinTopPlane)
	{
		wnLegCount-=(WORD)(xSublegInfo.ziMinTopPlane-xSublegInfo.ziMaxTopPlane)/wiLevelDiff;
		xSublegInfo.ziMaxTopPlane=xSublegInfo.ziMinTopPlane;
	}
	wnLegCount=min(MAX_SUBLEG_COUNT,wnLegCount);
	xSublegInfo.wiLevelDiff=wiLevelDiff;
	xSublegInfo.wnLegCount=wnLegCount;
	if(pxLegInfo)
		*pxLegInfo=xSublegInfo;
	return wnLegCount;
}
double HEIGHTGROUP::CalPlaneWidth(double zfPlane)
{
	return xBtmPlane.dfWidth+(zfPlane-xBtmPlane.ziPlane)*dfSlopeGradient*2;
}
CTrunkBodyPlanner::EXIST_SECTION::EXIST_SECTION()
{
	idPanelType=TRUNK_SEGMENT::SECT_WHOLE;
	memset(harrRod,0,16);//[4];
	hTopNode=hBtmNode=0;
	ziTopPlane=ziBtmPlane=0;
	dfTopWidth=dfBtmWidth=0;
}
TRUNK_NODE* TRUNK_NODE::Append()
{
	if(this->linknext==NULL)
	{
		linknext=new TRUNK_NODE();
		return linknext;
	}
	else
		return linknext->Append();
}
TRUNK_NODE::TRUNK_NODE()
{
	ziPlane=0;
	memset(harrQuadNodes,0,16);
	linknext=NULL;
}
TRUNK_NODE::~TRUNK_NODE()
{
	DeleteChilds();
}
bool TRUNK_NODE::DeleteChilds()
{
	if(linknext==NULL)
		return false;
	linknext->DeleteChilds();
	delete linknext;	//��ĩβ�����ӽڵ�
	linknext=NULL;
	return true;
}
CTrunkBodyPlanner::CTrunkBodyPlanner()
{
	ciLegSchema=CFGLEG::MULTILEG_MAX08;
	wnHeightGroup=3;		//�滮�ĺ��߷�����
	wnMaxLegCount=1;
	wiLegDiffLevel=1000;	//�滮�ĺ����ڼ��ȼ���
	ziTopPlane=25850;		//������Zֵ
	ziHeight0Plane=8000;	//���߼�����ʼ��׼ƽ���Z����
	dfSlopeGradient=0.11;	//�¶Ȧ�x(��y)/��z
	uiTopWidth=4700;
	uiMinHeight=30000;
	uiMaxHeight=48000;
	memset(xSlope.harrTopNodes,0,16);
	memset(xSlope.harrBtmNodes,0,16);
}
double CTrunkBodyPlanner::CalPlaneWidth(double zfPlane)
{
	return uiTopWidth+(zfPlane-ziTopPlane)*dfSlopeGradient*2;
}
bool CTrunkBodyPlanner::Planning(IXhList<HEIGHTGROUP>* pListPlanHeights,bool bTopHalfSection/*=false*/)
{
	UINT uiVarHeight=uiMaxHeight-uiMinHeight;	//ָ
	UINT i,j,uiHeightLevel=0;
	if(wnHeightGroup>1)
	{
		if(uiVarHeight%(wnHeightGroup-1)>0)
			logerr.Log("�滮���߷���������߹滮��Χ��һ��");
		uiHeightLevel=uiVarHeight/(wnHeightGroup-1);
	}
	CHashListEx<HEIGHTGROUP> hashPlanHeights;
	if(pListPlanHeights==NULL)
		pListPlanHeights=&hashPlanHeights;
	//1.������ߺ�����ȵ�������Ƹ���
	double dfBtmWidth=this->uiTopWidth+(this->ziHeight0Plane+this->uiMaxHeight-this->ziTopPlane)*this->dfSlopeGradient*2;
	//2.���ɺ��߷��鼰������Ϣ
	HEIGHTGROUP* pHeight;
	PRESET_ARRAY32<HEIGHTGROUP*> heightset;	//�ӵ͵�������
	for(i=this->wnHeightGroup;i>0;i--)
	{
		pHeight=pListPlanHeights->Append(i);
		pHeight->ciBodySerial=i;
		heightset.Set(i-1,pHeight);
		int index=this->wnHeightGroup-i;
		pHeight->uiNamedHeight=this->uiMaxHeight-uiHeightLevel*index;//����
		pHeight->ziCurrPlane=this->ziTopPlane;
		pHeight->xBtmPlane.ziPlane=this->ziHeight0Plane+pHeight->uiNamedHeight;
		pHeight->xBtmPlane.dfWidth=dfBtmWidth-uiHeightLevel*index*this->dfSlopeGradient*2;
	}
	//3.���ɸ����ߵļ��Ȼ�����Ϣ
	wnMaxLegCount=uiHeightLevel*2/this->wiLegDiffLevel;
	for(pHeight=pListPlanHeights->MoveFirst();pHeight!=NULL;pHeight=pListPlanHeights->MoveNext())
	{
		WORD wnLegCount=pHeight->CalTopPlane(wnMaxLegCount,this->wiLegDiffLevel);
		if(wnLegCount>wnMaxLegCount)
			wnMaxLegCount=wnLegCount;
		//pHeight->xarrSubLegs.ZeroPresetMemory();
		pHeight->xarrSubLegs.ReSize(wnLegCount);
		for(j=0;j<wnLegCount;j++)
		{
			pHeight->xarrSubLegs[j].ciLegSerial=j+1;
			//pHeight->xarrSubLegs[j].xTopPlane=pHeight->xLegTopPlane;
			pHeight->xarrSubLegs[j].dfSlopeGradient=this->dfSlopeGradient;
			pHeight->xarrSubLegs[j].wiLegHeight=pHeight->xSublegInfo.wiMaxLegHeight-j*this->wiLegDiffLevel;
			pHeight->xarrSubLegs[j].ziBtmPlane=pHeight->xBtmPlane.ziPlane-j*this->wiLegDiffLevel;
			pHeight->xarrSubLegs[j].dfBtmWidth=pHeight->CalPlaneWidth(pHeight->xarrSubLegs[j].ziBtmPlane);
		}
	}
	ciLegSchema=wnMaxLegCount<=8?CFGLEG::MULTILEG_MAX08:CFGLEG::MULTILEG_MAX16;
	CLegSchemaLife schemalife(ciLegSchema);
	//4.����ȫ�����ߵĹ�������Body
	TRUNK_PLANE xCurrPlane;
	TRUNK_SEGMENT* pSegment;
	xCurrPlane.ziPlane=this->ziTopPlane;
	xCurrPlane.dfWidth=this->uiTopWidth;
	long ziLowestPlane=this->ziHeight0Plane+this->uiMaxHeight;
	bool bFirstSection=true;
	while(xCurrPlane.ziPlane<ziLowestPlane)
	{
		TRUNK_SEGMENT body(xCurrPlane.dfWidth,this->dfSlopeGradient);
		double dfMinHeight=4500,dfMaxHeight=4500;
		bool successed=true;
		if(bFirstSection&&bTopHalfSection)
		{
			successed=body.CalUpHalfSectHeight(&dfMinHeight,&dfMaxHeight);
			body.idPanelType=TRUNK_SEGMENT::SECT_HALF_UP;
		}
		else
			successed=body.CalWholeSectHeight(&dfMinHeight,&dfMaxHeight);
		bFirstSection=false;
		long uiPreferHeight=ftoi((dfMinHeight+dfMaxHeight)*0.5);
		long uiCurrHeight=ftoi(dfMaxHeight);
		uiPreferHeight-=uiPreferHeight%100;	//�滮�׶νڼ�߶ȵ�100mm���Ⱦ͹��ˡ�
		uiCurrHeight-=uiCurrHeight%100;
		for(j=0;j<heightset.Count;j++)
		{
			pHeight=heightset[j];
			if(xCurrPlane.ziPlane+uiCurrHeight<pHeight->xSublegInfo.ziMaxTopPlane)
			{
				body.cfgword.AddBodyLegs(pHeight->ciBodySerial);
				continue;
			}
			else if(xCurrPlane.ziPlane+dfMinHeight<pHeight->xSublegInfo.ziMaxTopPlane)
			{
				uiCurrHeight=pHeight->xSublegInfo.ziMaxTopPlane-xCurrPlane.ziPlane;
				body.cfgword.AddBodyLegs(pHeight->ciBodySerial);
				continue;
			}
		}
		if(body.cfgword.IsNull())
			break;
		pSegment=this->hashBodySegments.Append(body);
		pSegment->xTopPlane.ziPlane=xCurrPlane.ziPlane;
		pSegment->wiSegHeight=(WORD)min(uiPreferHeight,uiCurrHeight);
		xCurrPlane.dfWidth=pSegment->dfBtmWidth;
		xCurrPlane.ziPlane=xCurrPlane.ziPlane+pSegment->wiSegHeight;
	}
	//5.������߷����ר������ڼ�, ȷ���������λ��
	for(j=0;j<heightset.Count;j++)
	{	//��������߷��鵱ǰ�ѹ滮��ˮƽ��λ��
		for(pSegment=this->hashBodySegments.GetFirst();pSegment;pSegment=this->hashBodySegments.GetNext())
		{
			if(pSegment->cfgword.IsHasBodyNo(heightset[j]->ciBodySerial))
				heightset[j]->ziCurrPlane=max(pSegment->ziBtmPlane,heightset[j]->ziCurrPlane);
		}
	}
	for(j=0;j<heightset.Count;j++)
	{
		pHeight=heightset[j];
		if(pHeight->xSublegInfo.ziMinTopPlane-pHeight->ziCurrPlane<=0)
		{	//�Ѳ���Ҫֱ�Ӳ�����ڼ䣬ֱ�ӽӸ�����
			pHeight->xLegTopPlane.ziPlane=pHeight->ziCurrPlane;
			pHeight->xLegTopPlane.dfWidth=pHeight->CalPlaneWidth(pHeight->ziCurrPlane);
			for(UINT k=0;k<pHeight->xarrSubLegs.Count;k++)
				pHeight->xarrSubLegs[k].xTopPlane=pHeight->xLegTopPlane;
		}
		else
		{
			double dfCurrWidth=pHeight->CalPlaneWidth(pHeight->ziCurrPlane);
			TRUNK_SEGMENT halfseg(dfCurrWidth,this->dfSlopeGradient);
			double dfMinHeight=4500,dfMaxHeight=4500;
			bool successed=halfseg.CalDownHalfSectHeight(&dfMinHeight,&dfMaxHeight);
			long uiPreferHeight=ftoi((dfMinHeight+dfMaxHeight)*0.5);
			uiPreferHeight-=uiPreferHeight%100;
			if(pHeight->ziCurrPlane+uiPreferHeight<=pHeight->xSublegInfo.ziMaxTopPlane)
				uiPreferHeight=max(uiPreferHeight,pHeight->xSublegInfo.ziMinTopPlane-pHeight->ziCurrPlane);
			else
				uiPreferHeight=pHeight->xSublegInfo.ziMaxTopPlane-pHeight->ziCurrPlane;
			halfseg.cfgword.SetBodyLegs(pHeight->ciBodySerial);
			pSegment=this->hashBodySegments.Append(halfseg,0);
			pSegment->xTopPlane.ziPlane=pHeight->ziCurrPlane;
			pSegment->wiSegHeight=(WORD)uiPreferHeight;
			pSegment->idPanelType=TRUNK_SEGMENT::SECT_HALF_DOWN;
			pHeight->xLegTopPlane.ziPlane=pSegment->ziBtmPlane;
			pHeight->xLegTopPlane.dfWidth=pSegment->dfBtmWidth;//CalPlaneWidth(pHeight->xLegTopPlane.ziPlane);
			for(UINT k=0;k<pHeight->xarrSubLegs.Count;k++)
				pHeight->xarrSubLegs[k].xTopPlane=pHeight->xLegTopPlane;
		}
	}
	return true;
}

#include "Tower.h"
#include "env_def.h"
#include "ParseAdaptNo.h"
#include "DxfFile.h"
static CFGWORD StrToCfgWord(const char* bodystr)
{
	CFGWORD cfgword;
	long serial=FindAdaptNo(bodystr,",","-");
	cfgword.AddBodyLegs(serial);
	while((serial=FindAdaptNo(NULL))>0)
		cfgword.AddBodyLegs(serial);
	return cfgword;
}
bool CTrunkBodyPlanner::InitTrunkInfo(UINT idTowerModelTmpl/*=0*/)
{
	ziTopPlane=25850;	//��ʼ���������¶�����Ķ���λ��
	ziHeight0Plane=8000;
	//��ʼ�����и����ߵ��������ƽ����Ϣ
	hashExistHeights.Empty();
	EXIST_HEIGHT* pHeight=hashExistHeights.Add(30000);	//30m
	pHeight->ciBodySerial=1;
	pHeight->hTransitNode=0x8c;
	pHeight->hPostLegRod =0x606;
	pHeight->hLowestBaseNode=0xa8;
	pHeight=hashExistHeights.Add(33000);	//33m
	pHeight->ciBodySerial=2;
	pHeight->hTransitNode	=0xb0;
	pHeight->hLowestBaseNode=0xd0;
	pHeight->hPostLegRod	=0x6ce;
	//������ýṹ
	pHeight->pants.hCornerNode=0xb4;
	pHeight->pants.hFrontInnerNode=0xd4;
	pHeight->pants.hSideInnerNode =0xd8;
	pHeight=hashExistHeights.Add(36000);	//36m
	pHeight->ciBodySerial=3;
	pHeight->hTransitNode	=0xe4;
	pHeight->hPostLegRod	=0x77e;
	pHeight->hLowestBaseNode=0x100;
	pHeight=hashExistHeights.Add(39000);	//39m
	pHeight->ciBodySerial=4;
	pHeight->hTransitNode	=0x104;
	pHeight->hPostLegRod	=0x83a;
	pHeight->hLowestBaseNode=0x124;
	//������ýṹ
	pHeight->pants.hCornerNode=0x108;
	pHeight->pants.hFrontInnerNode=0x128;
	pHeight->pants.hSideInnerNode =0x12c;
	pHeight=hashExistHeights.Add(42000);	//42m
	pHeight->ciBodySerial=5;
	pHeight->hTransitNode	=0x134;
	pHeight->hPostLegRod	=0x8f6;
	pHeight->hLowestBaseNode=0x158;
	pHeight=hashExistHeights.Add(45000);	//45m
	pHeight->ciBodySerial=6;
	pHeight->hTransitNode	=0x15c;
	pHeight->hPostLegRod	=0x9ca;
	pHeight->hLowestBaseNode=0x184;
	//������ýṹ
	pHeight->pants.hCornerNode=0x160;
	pHeight->pants.hFrontInnerNode=0x188;
	pHeight->pants.hSideInnerNode =0x18c;
	pHeight=hashExistHeights.Add(48000);	//48m
	pHeight->ciBodySerial=7;
	pHeight->hTransitNode	=0x190;
	pHeight->hPostLegRod	=0xa36;
	pHeight->hLowestBaseNode=0x1b8;
	//������ýṹ
	pHeight->pants.hCornerNode=0x194;
	pHeight->pants.hFrontInnerNode=0x1bc;
	pHeight->pants.hSideInnerNode =0x1c0;
	//����ڼ�
	EXIST_SECTION* pSection=listExistSections.Append();
	pSection->cfgword=StrToCfgWord("4-7");
	pSection->ziTopPlane=25850;
	pSection->hTopNode=0x5e;
	pSection->hBtmNode=0x7c;
	pSection->ziBtmPlane=27950;
	pSection->harrRod[0]=0x4e2;
	pSection->harrRod[1]=0x4e3;
	pSection->harrRod[2]=0x4e4;
	pSection->harrRod[3]=0x4e5;

	pSection=listExistSections.Append();
	pSection->cfgword=StrToCfgWord("4-7");
	pSection->hTopNode=0x7c;
	pSection->hBtmNode=0x80;
	pSection->hQuadRod=0x4e6;
	pSection=listExistSections.Append();
	pSection->cfgword=StrToCfgWord("6-7");
	pSection->hTopNode=0x80;
	pSection->hBtmNode=0x84;
	pSection->hQuadRod=0x4fa;
	pSection=listExistSections.Append();
	pSection->cfgword.SetBodyLegs(7);//=StrToCfgWord("7");
	pSection->hTopNode=0x84;
	pSection->hBtmNode=0x190;
	pSection->hQuadRod=0x506;
	//��ʼ����������ͶӰ��׼��
	xSlope.hTopNode=0x5e;
	xSlope.hBtmNode=0x62;
	return true;
}
bool CTrunkBodyPlanner::AutoRetrieveTrunkInfo(CTower* pTower)
{
	CLDSNode* pTopNode,*pBtmNode,*pMirNode;
	pTopNode=pTower->FromNodeHandle(xSlope.harrTopNodes[0]);
	if(xSlope.harrTopNodes[1]==0&&(pMirNode=pTopNode->GetMirNode(MIRMSG(2)))!=NULL)
		xSlope.harrTopNodes[1]=pMirNode->handle;
	if(xSlope.harrTopNodes[2]==0&&(pMirNode=pTopNode->GetMirNode(MIRMSG(1)))!=NULL)
		xSlope.harrTopNodes[2]=pMirNode->handle;
	if(xSlope.harrTopNodes[3]==0&&(pMirNode=pTopNode->GetMirNode(MIRMSG(3)))!=NULL)
		xSlope.harrTopNodes[3]=pMirNode->handle;
	pBtmNode=pTower->FromNodeHandle(xSlope.harrBtmNodes[0]);
	if(xSlope.harrBtmNodes[1]==0&&(pMirNode=pBtmNode->GetMirNode(MIRMSG(2)))!=NULL)
		xSlope.harrBtmNodes[1]=pMirNode->handle;
	if(xSlope.harrBtmNodes[2]==0&&(pMirNode=pBtmNode->GetMirNode(MIRMSG(1)))!=NULL)
		xSlope.harrBtmNodes[2]=pMirNode->handle;
	if(xSlope.harrBtmNodes[3]==0&&(pMirNode=pBtmNode->GetMirNode(MIRMSG(3)))!=NULL)
		xSlope.harrBtmNodes[3]=pMirNode->handle;

	for(EXIST_SECTION* pSection=listExistSections.EnumObjectFirst();pSection;pSection=listExistSections.EnumObjectNext())
	{
		CLDSLinePart* pRod=pTower->FromRodHandle(pSection->harrRod[0]);
		CLDSLinePart* pMirRod=NULL;
		pTopNode=pTower->FromNodeHandle(pSection->hTopNode);
		pBtmNode=pTower->FromNodeHandle(pSection->hBtmNode);
		pSection->ziTopPlane=ftoi(pTopNode->xOriginalPos.z);// 27950;
		pSection->dfTopWidth=pTopNode->xOriginalPos.x*2;
		pSection->ziBtmPlane=ftoi(pBtmNode->xOriginalPos.z);// 32200;
		pSection->dfBtmWidth=pBtmNode->xOriginalPos.x*2;
		if(pSection->harrRod[1]==0&&(pMirRod=pRod->GetMirRod(MIRMSG(2)))!=NULL)
			pSection->harrRod[1]=pMirRod->handle;
		if(pSection->harrRod[2]==0&&(pMirRod=pRod->GetMirRod(MIRMSG(1)))!=NULL)
			pSection->harrRod[2]=pMirRod->handle;
		if(pSection->harrRod[3]==0&&(pMirRod=pRod->GetMirRod(MIRMSG(3)))!=NULL)
			pSection->harrRod[3]=pMirRod->handle;
	}
	for(EXIST_HEIGHT* pExistHeight=hashExistHeights.GetFirst();pExistHeight;pExistHeight=hashExistHeights.GetNext())
	{
		for(CLDSModule* pModule=pTower->Module.GetFirst();pModule;pModule=pTower->Module.GetNext())
		{
			if(pModule->idBodyNo==pExistHeight->ciBodySerial)
				pExistHeight->hRelaHeightModule=pModule->handle;
		}
	}
	return true;
}
void CTrunkBodyPlanner::AppendTransitHoriSectionToModel(HEIGHTGROUP* pHeight,CTower* pTower)
{
	CLDSNodePtr pxarrCornerNodes[4]={NULL};
	typedef CLDSLineAngle* CLDSLineAnglePtr;
	CLDSLineAnglePtr pxarrQuadAngles[4]={NULL};
	int j,signx[4]={1,-1,1,-1},signy[4]={1,1,-1,-1};
	//������Ľڵ㼰�Ǹ�
	CXhChar16 szLayer("LP1");
	GEPOINT xCorner(pHeight->xLegTopPlane.dfWidth*0.5,pHeight->xLegTopPlane.dfWidth*0.5,pHeight->xLegTopPlane.ziPlane);
	TRUNK_NODE* pHeadTrunkNode=this->hashTrunkNodes.GetValue(pHeight->xLegTopPlane.ziPlane);
	TRUNK_NODE* pTrunkNode=pHeadTrunkNode;
	while(pTrunkNode!=NULL){
		if(!pTrunkNode->cfgword.IsHasBodyNo(pHeight->ciBodySerial))
			pTrunkNode=pTrunkNode->linknext;
		else
			break;
	};
	if(pTrunkNode==NULL)
	{
		for(j=0;j<4;j++)
		{
			pxarrCornerNodes[j]=pTower->Node.append();
			szLayer.Printf("SP%d",j+1);
			pxarrCornerNodes[j]->SetLayer(szLayer);
			pxarrCornerNodes[j]->cfgword.Clear();
			pxarrCornerNodes[j]->cfgword.SetBodyLegs(pHeight->ciBodySerial);
			pxarrCornerNodes[j]->m_cPosCalType=CLDSNode::COORD_Z_SPECIFIED;
			pxarrCornerNodes[j]->SetPosition(xCorner.x*signx[j],xCorner.y*signy[j],xCorner.z);
			pxarrCornerNodes[j]->arrRelationNode[0]=this->xSlope.harrTopNodes[j];
			pxarrCornerNodes[j]->arrRelationNode[1]=this->xSlope.harrBtmNodes[j];
		}
	}
	else
	{
		for(j=0;j<4;j++)
			pxarrCornerNodes[j]=pTower->FromNodeHandle(pTrunkNode->harrQuadNodes[j]);
	}
	//���������ɺ����
	CLDSLineAngle* pHoriFrontAngle=(CLDSLineAngle*)pTower->AppendPart(CLS_LINEANGLE);
	pHoriFrontAngle->pStart=pxarrCornerNodes[1];
	pHoriFrontAngle->pEnd  =pxarrCornerNodes[0];
	pHoriFrontAngle->SetLayer("SHQ");
	pHoriFrontAngle->cfgword.Clear();
	pHoriFrontAngle->cfgword.SetBodyLegs(pHeight->ciBodySerial);
	pHoriFrontAngle->SetStart(pHoriFrontAngle->pStart->xOriginalPos);
	pHoriFrontAngle->SetEnd  (pHoriFrontAngle->pEnd  ->xOriginalPos);
	
	CLDSLineAngle* pHoriBackAngle=(CLDSLineAngle*)pTower->AppendPart(CLS_LINEANGLE);
	pHoriBackAngle->pStart=pxarrCornerNodes[3];
	pHoriBackAngle->pEnd  =pxarrCornerNodes[2];
	pHoriBackAngle->SetLayer("SHH");
	pHoriBackAngle->cfgword.Clear();
	pHoriBackAngle->cfgword.SetBodyLegs(pHeight->ciBodySerial);
	pHoriBackAngle->SetStart(pHoriBackAngle->pStart->xOriginalPos);
	pHoriBackAngle->SetEnd  (pHoriBackAngle->pEnd  ->xOriginalPos);
	
	CLDSLineAngle* pHoriRightAngle=(CLDSLineAngle*)pTower->AppendPart(CLS_LINEANGLE);
	pHoriRightAngle->pStart=pxarrCornerNodes[0];
	pHoriRightAngle->pEnd  =pxarrCornerNodes[2];
	pHoriRightAngle->SetLayer("SHY");
	pHoriRightAngle->cfgword.Clear();
	pHoriRightAngle->cfgword.SetBodyLegs(pHeight->ciBodySerial);
	pHoriRightAngle->SetStart(pHoriRightAngle->pStart->xOriginalPos);
	pHoriRightAngle->SetEnd  (pHoriRightAngle->pEnd  ->xOriginalPos);

	CLDSLineAngle* pHoriLeftAngle=(CLDSLineAngle*)pTower->AppendPart(CLS_LINEANGLE);
	pHoriLeftAngle->pStart=pxarrCornerNodes[1];
	pHoriLeftAngle->pEnd  =pxarrCornerNodes[3];
	pHoriLeftAngle->SetLayer("SHZ");
	pHoriLeftAngle->cfgword.Clear();
	pHoriLeftAngle->cfgword.SetBodyLegs(pHeight->ciBodySerial);
	pHoriLeftAngle->SetStart(pHoriLeftAngle->pStart->xOriginalPos);
	pHoriLeftAngle->SetEnd  (pHoriLeftAngle->pEnd  ->xOriginalPos);
	//��Ӻ�����м��
	CLDSNode* pFrontMidNode=pTower->AppendNode();
	pFrontMidNode->m_cPosCalType=CLDSNode::COORD_SCALE;
	pFrontMidNode->attach_scale=0.5;
	pFrontMidNode->hFatherPart=pHoriFrontAngle->handle;
	pFrontMidNode->cfgword.Clear();
	pFrontMidNode->cfgword.SetBodyLegs(pHeight->ciBodySerial);
	pFrontMidNode->arrRelationNode[0]=pxarrCornerNodes[0]->handle;
	pFrontMidNode->arrRelationNode[1]=pxarrCornerNodes[1]->handle;
	pFrontMidNode->SetPosition(0,pxarrCornerNodes[0]->xOriginalPos.y,pxarrCornerNodes[0]->xOriginalPos.z);
	CLDSNode* pRightMidNode=pTower->AppendNode();
	pRightMidNode->m_cPosCalType=CLDSNode::COORD_SCALE;
	pRightMidNode->attach_scale=0.5;
	pRightMidNode->hFatherPart=pHoriRightAngle->handle;
	pRightMidNode->cfgword.Clear();
	pRightMidNode->cfgword.SetBodyLegs(pHeight->ciBodySerial);
	pRightMidNode->arrRelationNode[0]=pxarrCornerNodes[0]->handle;
	pRightMidNode->arrRelationNode[1]=pxarrCornerNodes[2]->handle;
	pRightMidNode->SetPosition(pxarrCornerNodes[0]->xOriginalPos.x,0,pxarrCornerNodes[0]->xOriginalPos.z);
	CLDSNode* pBackMidNode=pTower->AppendNode();
	pBackMidNode->m_cPosCalType=CLDSNode::COORD_SCALE;
	pBackMidNode->attach_scale=0.5;
	pBackMidNode->hFatherPart=pHoriBackAngle->handle;
	pBackMidNode->cfgword.Clear();
	pBackMidNode->cfgword.SetBodyLegs(pHeight->ciBodySerial);
	pBackMidNode->arrRelationNode[0]=pxarrCornerNodes[3]->handle;
	pBackMidNode->arrRelationNode[1]=pxarrCornerNodes[2]->handle;
	pBackMidNode->SetPosition(0,-pxarrCornerNodes[0]->xOriginalPos.y,pxarrCornerNodes[0]->xOriginalPos.z);
	CLDSNode* pLeftMidNode=pTower->AppendNode();
	pLeftMidNode->m_cPosCalType=CLDSNode::COORD_SCALE;
	pLeftMidNode->attach_scale=0.5;
	pLeftMidNode->hFatherPart=pHoriRightAngle->handle;
	pLeftMidNode->cfgword.Clear();
	pLeftMidNode->cfgword.SetBodyLegs(pHeight->ciBodySerial);
	pLeftMidNode->arrRelationNode[0]=pxarrCornerNodes[1]->handle;
	pLeftMidNode->arrRelationNode[1]=pxarrCornerNodes[3]->handle;
	pLeftMidNode->SetPosition(-pxarrCornerNodes[0]->xOriginalPos.x,0,pxarrCornerNodes[0]->xOriginalPos.z);
	//���������ɸ������м�ڵ�
	pHeight->xSublegInfo.xFront.pNode=pFrontMidNode;
	pHeight->xSublegInfo.xRight.pNode=pRightMidNode;
	pHeight->xSublegInfo.xBack.pNode =pBackMidNode;
	pHeight->xSublegInfo.xLeft.pNode =pLeftMidNode;
}
void CTrunkBodyPlanner::AppendSectionToModel(PLAN_SECTION* pSection,HEIGHTGROUP* pBelongHeight,CTower* pTower)
{
	CLDSNodePtr pxarrTopNodes[4]={NULL},pxarrBtmNodes[4]={NULL};
	typedef CLDSLineAngle* CLDSLineAnglePtr;
	CLDSLineAnglePtr pxarrQuadAngles[4]={NULL};
	int j,signx[4]={1,-1,1,-1},signy[4]={1,1,-1,-1};
	//������Ľڵ㼰�Ǹ�
	CXhChar16 szLayer("LP1");
	GEPOINT xCorner(pSection->dfBtmWidth*0.5,pSection->dfBtmWidth*0.5,pSection->ziBtmPlane);
	for(j=0;j<4;j++)
	{
		pxarrBtmNodes[j]=pTower->Node.append();
		if(pSection->idPanelType==TRUNK_SEGMENT::SECT_SUBLEG)
			szLayer.Printf("LP%d",j+1);
		else
			szLayer.Printf("SP%d",j+1);
		pxarrBtmNodes[j]->SetLayer(szLayer);
		pxarrBtmNodes[j]->cfgword=pSection->cfgword;
		pxarrBtmNodes[j]->m_cPosCalType=CLDSNode::COORD_Z_SPECIFIED;
		pxarrBtmNodes[j]->SetPosition(xCorner.x*signx[j],xCorner.y*signy[j],xCorner.z);
		pxarrBtmNodes[j]->arrRelationNode[0]=this->xSlope.harrTopNodes[j];
		pxarrBtmNodes[j]->arrRelationNode[1]=this->xSlope.harrBtmNodes[j];
	}
	TRUNK_NODE* pHeadTrunkNode=this->hashTrunkNodes.GetValue(pSection->xTopPlane.ziPlane);
	TRUNK_NODE* pTrunkNode=pHeadTrunkNode;
	while(pTrunkNode!=NULL){
		if(!pTrunkNode->cfgword.And(pSection->cfgword))
			pTrunkNode=pTrunkNode->linknext;
		else
			break;
	};
	if(pTrunkNode==NULL)
	{
		GEPOINT xCorner(pSection->xTopPlane.dfWidth*0.5,pSection->xTopPlane.dfWidth*0.5,pSection->xTopPlane.ziPlane);
		for(j=0;j<4;j++)
		{
			pxarrTopNodes[j]=pTower->Node.append();
			szLayer.Printf("SP%d",j+1);
			pxarrTopNodes[j]->SetLayer(szLayer);
			pxarrTopNodes[j]->cfgword=pSection->cfgword;
			pxarrTopNodes[j]->UnifiedCfgword();
			pxarrTopNodes[j]->m_cPosCalType=CLDSNode::COORD_Z_SPECIFIED;
			pxarrTopNodes[j]->SetPosition(xCorner.x*signx[j],xCorner.y*signy[j],xCorner.z);
			pxarrTopNodes[j]->arrRelationNode[0]=this->xSlope.harrTopNodes[j];
			pxarrTopNodes[j]->arrRelationNode[1]=this->xSlope.harrBtmNodes[j];
		}
	}
	else
	{
		for(j=0;j<4;j++)
			pxarrTopNodes[j]=pTower->FromNodeHandle(pTrunkNode->harrQuadNodes[j]);
	}
	for(j=0;j<4;j++)
	{
		pxarrQuadAngles[j]=(CLDSLineAngle*)pTower->AppendPart(CLS_LINEANGLE);
		pxarrQuadAngles[j]->pStart=pxarrTopNodes[j];
		pxarrQuadAngles[j]->pEnd  =pxarrBtmNodes[j];
		if(pSection->idPanelType==TRUNK_SEGMENT::SECT_SUBLEG)
			szLayer.Printf("LZ%d",j+1);	//����
		else
			szLayer.Printf("SZ%d",j+1);	//����
		pxarrQuadAngles[j]->SetLayer(szLayer);
		pxarrQuadAngles[j]->cfgword=pSection->cfgword;
		if(pSection->idPanelType!=TRUNK_SEGMENT::SECT_SUBLEG)
			pxarrQuadAngles[j]->UnifiedCfgword();
		pxarrQuadAngles[j]->SetStart(pxarrTopNodes[j]->xOriginalPos);
		pxarrQuadAngles[j]->SetEnd  (pxarrBtmNodes[j]->xOriginalPos);
		//���ýǸ�֫����pxarrQuadAngles[j]->des_norm_x.hViceJg=
		//���ýǸֶ˵�λ��
	}
	if(pSection->idPanelType==TRUNK_SEGMENT::SECT_SUBLEG)
	{
		//��ӽ��Ȱ�����б��
		CLDSNodePtr parrTopNodes[4]={
			pBelongHeight->xSublegInfo.xFront.pNode,pBelongHeight->xSublegInfo.xRight.pNode,
			pBelongHeight->xSublegInfo.xBack.pNode ,pBelongHeight->xSublegInfo.xLeft.pNode};
		BYTE iarrBtmNodeIndex[8]={1,0,0,2,2,3,3,1};
		for(j=0;j<4;j++)
		{
			CLDSLineAngle* pPieLegAngle=(CLDSLineAngle*)pTower->AppendPart(CLS_LINEANGLE);
			pPieLegAngle->pStart=parrTopNodes[j];
			pPieLegAngle->pEnd=pxarrBtmNodes[iarrBtmNodeIndex[j*2]];
			pPieLegAngle->cfgword=pPieLegAngle->pEnd->cfgword;
			pPieLegAngle->SetStart(pPieLegAngle->pStart->xOriginalPos);
			pPieLegAngle->SetEnd(pPieLegAngle->pEnd->xOriginalPos);
			pPieLegAngle->ciLegQuad=iarrBtmNodeIndex[j*2]+1;
			pPieLegAngle->SetLayer("LX1");
			pPieLegAngle->layer(2)='0'+pPieLegAngle->ciLegQuad;
			CLDSLineAngle* pNaiLegAngle=(CLDSLineAngle*)pTower->AppendPart(CLS_LINEANGLE);
			pNaiLegAngle->pStart=parrTopNodes[j];
			pNaiLegAngle->pEnd=pxarrBtmNodes[iarrBtmNodeIndex[j*2+1]];
			pNaiLegAngle->cfgword=pNaiLegAngle->pEnd->cfgword;
			pNaiLegAngle->SetStart(pNaiLegAngle->pStart->xOriginalPos);
			pNaiLegAngle->SetEnd(pNaiLegAngle->pEnd->xOriginalPos);
			pNaiLegAngle->ciLegQuad=iarrBtmNodeIndex[j*2+1]+1;
			pNaiLegAngle->SetLayer("LX1");
			pNaiLegAngle->layer(2)='0'+pNaiLegAngle->ciLegQuad;
		}
	}
	//else if(pSection->idPanelType==TRUNK_SEGMENT::SECT_HALF_UP)
	//{
	//}
	else if(pSection->idPanelType==TRUNK_SEGMENT::SECT_HALF_DOWN)
	{
	}
	else //if(pSection->idPanelType==TRUNK_SEGMENT::SECT_WHOLE)
	{
	}
}
bool CTrunkBodyPlanner::UpdateTowerTrunk(CTower* pTower,CHashListEx<HEIGHTGROUP>* pHashPlanHeights)
{
	AutoRetrieveTrunkInfo(pTower);
	//1.��Ҫ��滮����
	bool bTopHalfSection=false;
	CHashListEx<HEIGHTGROUP> hashPlanHeights;
	if(pHashPlanHeights==NULL)
		pHashPlanHeights=&hashPlanHeights;
	CLogErrorLife life(&logerr);
	if(!Planning(pHashPlanHeights,bTopHalfSection))
		return false;
	CLDSNode* pNode;
	CLDSLinePart* pRod;
	EXIST_HEIGHT*  pExistHeight=hashExistHeights.GetFirst();
	EXIST_SECTION* pExistSect  =this->listExistSections.EnumObjectFirst();
	//ƥ����ߺ�������ĺ�
	HEIGHTGROUP* pHeight;
	EXIST_SECTION* pSection=NULL;
	BYTE ciMinIdleBodyNo=1;
	long hExcludeTransitNode=0;	//��ǰ��ߺ�����������ɺ����Ӧ�ų���ֻ�����ĸ������ϵĽǽڵ�
	for(pExistHeight=hashExistHeights.GetFirst();pExistHeight;pExistHeight=hashExistHeights.GetNext())
	{
		if(pExistHeight->ciBodySerial+1>ciMinIdleBodyNo)
		{
			hExcludeTransitNode=pExistHeight->hTransitNode;
			ciMinIdleBodyNo=pExistHeight->ciBodySerial+1;
		}
	}
	BYTE idHightestBody=ciMinIdleBodyNo-1;
	CXhSimpleList<TRUNK_SEGMENT> listInstSegments;
	for(pHeight=pHashPlanHeights->GetTail();pHeight;pHeight=pHashPlanHeights->GetPrev())
	{
		if((pExistHeight=hashExistHeights.GetValue(pHeight->uiNamedHeight))!=NULL)
		{
			pHeight->ciBodySerial=pExistHeight->ciBodySerial;
			pHeight->hRelaHeightModule=pExistHeight->hRelaHeightModule;
			//��Ʋ���ɺ��߽���
			continue;	//�Ѵ��ڸú���
		}
		//�����Ӻ���
		CLDSModule* pHeightModule=Ta.Module.append();
		pHeight->ciBodySerial=ciMinIdleBodyNo;
		CXhString(pHeightModule->description,51).Printf("%dm",pHeight->uiNamedHeight/1000);
		//TODO:����ciMinIdleBodyNo�Ƿ��غ�
		pHeightModule->iNo=ciMinIdleBodyNo;
		pHeightModule->SetBodyNo(ciMinIdleBodyNo);
		pHeight->hRelaHeightModule=pHeightModule->handle;
		CFGWORD legs;
		for(WORD iLeg=1;iLeg<=pHeight->xarrSubLegs.Count;iLeg++)
			legs.AddSpecNoWord(iLeg);
		pHeightModule->m_dwLegCfgWord.Clear();
		pHeightModule->m_dwLegCfgWord.SetBodyLegs(pHeight->ciBodySerial,legs.flag.word[0]);
		ciMinIdleBodyNo++;

		//ɸѡ�������ƺ��ߵ�����ڼ��п��ò���
		for(pSection=this->listExistSections.EnumObjectFirst();pSection;pSection=this->listExistSections.EnumObjectNext())
		{
			if(!pSection->cfgword.IsHasBodyNo(idHightestBody))
				continue;
			if(pSection->ziBtmPlane<pHeight->xSublegInfo.ziMinTopPlane)
			{
				pHeight->ziCurrPlane=pSection->ziBtmPlane;
				continue;
			}
			if(pSection->ziBtmPlane>=pHeight->xSublegInfo.ziMinTopPlane&&pSection->ziBtmPlane<=pHeight->xSublegInfo.ziMaxTopPlane)
			{
				pHeight->ziCurrPlane=pSection->ziBtmPlane;
				break;
			}
			else //if(pSection->ziBtmPlane>pHeight->xSublegInfo.ziMaxTopPlane)
			{	//�ټӵ�ǰ�ڼ��һ������˹���������ʱֻ���������ٽ������������һ�ڼ�λ��������
				//���ȵ���������
				if(pSection->ziBtmPlane-pHeight->xSublegInfo.ziMaxTopPlane<(int)(pHeight->xSublegInfo.wiLevelDiff*(pHeight->xarrSubLegs.Count-1)))
				{
					int wiReduceLegCount=(pSection->ziBtmPlane-pHeight->xSublegInfo.ziMaxTopPlane)/pHeight->xSublegInfo.wiLevelDiff;
					if((pSection->ziBtmPlane-pHeight->xSublegInfo.ziMaxTopPlane)%pHeight->xSublegInfo.wiLevelDiff>0)
						wiReduceLegCount+=1;
					pHeight->xSublegInfo.ziMaxTopPlane+=wiReduceLegCount*pHeight->xSublegInfo.wiLevelDiff;
					pHeight->xarrSubLegs.ReSize(pHeight->xarrSubLegs.Count-wiReduceLegCount);
					pHeight->xLegTopPlane.ziPlane=pHeight->ziCurrPlane=pSection->ziBtmPlane;
					pHeight->xLegTopPlane.dfWidth=pHeight->CalPlaneWidth(pSection->ziBtmPlane);
				}
				else	//����������ʧ��ʱ����ֱ�ӵ���ԭ�нڼ�
					break;
			}
		}
	}
	//�����м��ȱ�ڼ�
	//4.����ȫ�����ߵĹ�������Body
	//ɸѡ�������ƺ��ߵ�����ڼ��п��ò���
	TRUNK_PLANE xCurrPlane;
	TRUNK_SEGMENT* pSegment;
	CXhPtrSet<EXIST_SECTION> listModifiedExistSects;
	for(pSection=this->listExistSections.EnumObjectFirst();pSection;pSection=this->listExistSections.EnumObjectNext())
	{
		if(!pSection->cfgword.IsHasBodyNo(idHightestBody)||
			pSection->idPanelType!=TRUNK_SEGMENT::SECT_WHOLE)
			continue;
		if(xCurrPlane.dfWidth<EPS)
		{
			xCurrPlane.ziPlane=pSection->ziBtmPlane;
			xCurrPlane.dfWidth=pSection->dfBtmWidth;//CalPlaneWidth(xCurrPlane.ziPlane);
		}
		else if(xCurrPlane.ziPlane<pSection->ziBtmPlane)
		{
			xCurrPlane.ziPlane=pSection->ziBtmPlane;
			xCurrPlane.dfWidth=pSection->dfBtmWidth;
		}
		bool modified=false;
		for(pHeight=pHashPlanHeights->GetFirst();pHeight;pHeight=pHashPlanHeights->GetNext())
		{
			if(pHeight->ciBodySerial>idHightestBody&&pSection->ziBtmPlane<pHeight->xSublegInfo.ziMaxTopPlane)
			{
				pSection->cfgword.AddBodyLegs(pHeight->ciBodySerial);
				pHeight->ziCurrPlane=max(pHeight->ziCurrPlane,pSection->ziBtmPlane);
				modified=true;
				continue;
			}
		}
		if(modified)
			listModifiedExistSects.append(pSection);
	}
	while(true)
	{
		TRUNK_SEGMENT body(xCurrPlane.dfWidth,this->dfSlopeGradient);
		double dfMinHeight=4500,dfMaxHeight=4500;
		bool successed=body.CalWholeSectHeight(&dfMinHeight,&dfMaxHeight);
		long uiPreferHeight=ftoi((dfMinHeight+dfMaxHeight)*0.5);
		long uiCurrHeight=ftoi(dfMaxHeight);
		uiPreferHeight-=uiPreferHeight%100;	//�滮�׶νڼ�߶ȵ�100mm���Ⱦ͹��ˡ�
		uiCurrHeight-=uiCurrHeight%100;
		for(pHeight=pHashPlanHeights->GetFirst();pHeight;pHeight=pHashPlanHeights->GetNext())
		{
			if(pHeight->ciBodySerial<=idHightestBody)
				continue;	//��ʱ��������hRelaHeightModule����Ҳ�Ѹ�ֵ
			if(xCurrPlane.ziPlane+uiCurrHeight<pHeight->xSublegInfo.ziMaxTopPlane)
			{	//��ǰ�滮����������ڼ������ȫ�ķŵ���ǰ������
				body.cfgword.AddBodyLegs(pHeight->ciBodySerial);
				continue;
			}
			else if(xCurrPlane.ziPlane+dfMinHeight<pHeight->xSublegInfo.ziMaxTopPlane)
			{
				uiCurrHeight=pHeight->xSublegInfo.ziMaxTopPlane-xCurrPlane.ziPlane;
				body.cfgword.AddBodyLegs(pHeight->ciBodySerial);
				continue;
			}
		}
		if(body.cfgword.IsNull())
			break;
		pSegment=listInstSegments.Append(body);
		pSegment->xTopPlane.ziPlane=xCurrPlane.ziPlane;
		pSegment->wiSegHeight=(WORD)min(uiPreferHeight,uiCurrHeight);
		xCurrPlane.dfWidth=pSegment->dfBtmWidth;
		xCurrPlane.ziPlane=xCurrPlane.ziPlane+pSegment->wiSegHeight;
		for(pHeight=pHashPlanHeights->GetFirst();pHeight;pHeight=pHashPlanHeights->GetNext())
		{
			if(pHeight->ciBodySerial<=idHightestBody||!pSegment->cfgword.IsHasBodyNo(pHeight->ciBodySerial))
				continue;	//��ʱ��������hRelaHeightModule����Ҳ�Ѹ�ֵ
			pHeight->ziCurrPlane=pSegment->ziBtmPlane;
		}
	}
	//�������
	for(pHeight=pHashPlanHeights->GetFirst();pHeight;pHeight=pHashPlanHeights->GetNext())
	{
		if(pHeight->ciBodySerial<=idHightestBody)
			continue;
		if(pHeight->ziCurrPlane>=pHeight->xSublegInfo.ziMinTopPlane)
		{	//�Ѳ���Ҫֱ�Ӳ�����ڼ䣬ֱ�ӽӸ�����
			pHeight->xLegTopPlane.ziPlane=pHeight->ziCurrPlane;
			pHeight->xLegTopPlane.dfWidth=pHeight->CalPlaneWidth(pHeight->ziCurrPlane);
			for(UINT k=0;k<pHeight->xarrSubLegs.Count;k++)
				pHeight->xarrSubLegs[k].xTopPlane=pHeight->xLegTopPlane;
		}
		else //if(pHeight->ziCurrPlane<pHeight->xSublegInfo.ziMinTopPlane)
		{	//��Ҫ���Ӱ��ͨ�ڼ�
			double dfCurrWidth=pHeight->CalPlaneWidth(pHeight->ziCurrPlane);
			TRUNK_SEGMENT halfseg(dfCurrWidth,this->dfSlopeGradient);
			double dfMinHeight=4500,dfMaxHeight=4500;
			bool successed=halfseg.CalDownHalfSectHeight(&dfMinHeight,&dfMaxHeight);
			long uiPreferHeight=ftoi((dfMinHeight+dfMaxHeight)*0.5);
			uiPreferHeight-=uiPreferHeight%100;
			if(pHeight->ziCurrPlane+uiPreferHeight<=pHeight->xSublegInfo.ziMaxTopPlane)
				uiPreferHeight=max(uiPreferHeight,pHeight->xSublegInfo.ziMinTopPlane-pHeight->ziCurrPlane);
			else
				uiPreferHeight=pHeight->xSublegInfo.ziMaxTopPlane-pHeight->ziCurrPlane;
			halfseg.cfgword.SetBodyLegs(pHeight->ciBodySerial);
			pSegment=listInstSegments.Append(halfseg,0);
			pSegment->xTopPlane.ziPlane=pHeight->ziCurrPlane;
			pSegment->wiSegHeight=(WORD)uiPreferHeight;
			pSegment->idPanelType=TRUNK_SEGMENT::SECT_HALF_DOWN;
			pHeight->xLegTopPlane.ziPlane=pSegment->ziBtmPlane;
			pHeight->xLegTopPlane.dfWidth=pSegment->dfBtmWidth;//CalPlaneWidth(pHeight->xLegTopPlane.ziPlane);
			for(UINT k=0;k<pHeight->xarrSubLegs.Count;k++)
				pHeight->xarrSubLegs[k].xTopPlane=pHeight->xLegTopPlane;
		}
	}
	//�����ִ�ڼ������ĺ�
	CLDSNode* pTransitCornerNode=pTower->FromNodeHandle(hExcludeTransitNode);
	for(pSection=listModifiedExistSects.GetFirst();pSection;pSection=listModifiedExistSects.GetNext())
	{
		TAPER_SCOPE taper;
		f3dPoint xTopPoint(pSection->dfTopWidth*0.5,pSection->dfTopWidth*0.5,pSection->ziTopPlane);
		f3dPoint xBtmPoint(pSection->dfBtmWidth*0.5,pSection->dfBtmWidth*0.5,pSection->ziBtmPlane);
		taper.VerifyVertex(xTopPoint);
		taper.VerifyVertex(xBtmPoint);
		taper.VerifyVertex(-xTopPoint.x,-xTopPoint.y,xTopPoint.z);
		taper.VerifyVertex(-xBtmPoint.x,-xBtmPoint.y,xBtmPoint.z);
		bool bRemovedHoriPanel=false;
		if(pTransitCornerNode&&fabs(pSection->ziBtmPlane-pTransitCornerNode->xOriginalPos.z)<EPS)
			bRemovedHoriPanel=true;

		for(pNode=pTower->EnumNodeFirst();pNode;pNode=pTower->EnumNodeNext())
		{
			if(!pSection->cfgword.And(pNode->cfgword))
				continue;	//��������ǰ�ڼ�
			if(bRemovedHoriPanel&&fabs(pNode->xOriginalPos.z-pSection->ziBtmPlane)<EPS)
			{	//ֻ���������ǽڵ㣬��������
				GEPOINT xUnifyPoint(fabs(pNode->xOriginalPos.x),fabs(pNode->xOriginalPos.y),pNode->xOriginalPos.z);
				if(!xUnifyPoint.IsEqual(xBtmPoint))
					continue;
			}
			if(taper.IsIncludePoint(pNode->xOriginalPos))
				pNode->cfgword.AddSpecWord(pSection->cfgword);
		}
		for(pRod=pTower->EnumRodFirst();pRod;pRod=pTower->EnumRodNext())
		{
			if(!pSection->cfgword.And(pRod->cfgword))
				continue;	//��������ǰ�ڼ�
			if(pRod->pStart==NULL||pRod->pEnd==NULL)
				continue;
			if( bRemovedHoriPanel&&
				fabs(pRod->pStart->xOriginalPos.z-pSection->ziBtmPlane)<EPS&&
				fabs(pRod->pEnd->xOriginalPos.z-pSection->ziBtmPlane)<EPS)
				continue;	//֮ǰ����������ɸ����ڵĺ��Ӧ����
			if( !taper.IsIncludePoint(pRod->pStart->xOriginalPos,0.5)||
				!taper.IsIncludePoint(pRod->pEnd->xOriginalPos,0.5))
				continue;
			pRod->cfgword.AddSpecWord(pSection->cfgword);
		}
	}
	//��ȡ��������滮���ݣ����º�������
	hashTrunkNodes.Empty();
	for(pSegment=listInstSegments.EnumObjectFirst();pSegment;pSegment=listInstSegments.EnumObjectNext())
		AppendSectionToModel(pSegment,NULL,pTower);
	for(pHeight=pHashPlanHeights->GetFirst();pHeight;pHeight=pHashPlanHeights->GetNext())
	{
		if(pHeight->ciBodySerial<=idHightestBody)
			continue;
		this->AppendTransitHoriSectionToModel(pHeight,pTower);
		for(UINT i=0;i<pHeight->xarrSubLegs.Count;i++)
		{
			CFGLEG cfgleg(pHeight->ciBodySerial,i+1);
			pHeight->xarrSubLegs[i].cfgword=cfgleg.ToCfgWord();
			AppendSectionToModel(&pHeight->xarrSubLegs[i],pHeight,pTower);
		}
	}
	//������ͷ��ȫ�ֹ��ò��ֵ���ĺ�
	for(pNode=pTower->EnumNodeFirst();pNode;pNode=pTower->EnumNodeNext())
	{
		if(pNode->IsLegObj())
			continue;
		bool bGlobalShareObject=true;
		for(pExistHeight=hashExistHeights.GetFirst();bGlobalShareObject&&pExistHeight;pExistHeight=hashExistHeights.GetNext())
		{
			if(!pNode->cfgword.IsHasBodyNo(pExistHeight->ciBodySerial))
				bGlobalShareObject=false;
		}
		if(bGlobalShareObject)
		{
			for(pHeight=pHashPlanHeights->GetFirst();pHeight;pHeight=pHashPlanHeights->GetNext())
				pNode->cfgword.AddBodyLegs(pHeight->ciBodySerial);
		}
	}
	for(CLDSPart* pPart=pTower->EnumPartFirst();pPart;pPart=pTower->EnumPartNext())
	{
		if(pPart->IsLegObj())
			continue;
		bool bGlobalShareObject=true;
		for(pExistHeight=hashExistHeights.GetFirst();bGlobalShareObject&&pExistHeight;pExistHeight=hashExistHeights.GetNext())
		{
			if(!pPart->cfgword.IsHasBodyNo(pExistHeight->ciBodySerial))
				bGlobalShareObject=false;
		}
		if(bGlobalShareObject)
		{
			for(pHeight=pHashPlanHeights->GetFirst();pHeight;pHeight=pHashPlanHeights->GetNext())
				pPart->cfgword.AddBodyLegs(pHeight->ciBodySerial);
		}
	}
	return true;
}
static void DrawSegment(GEPOINT origin,PLAN_SECTION* pSect,CDxfFile* pDxfFile)
{
	GEPOINT xTopPoint(origin.x+pSect->xTopPlane.dfWidth*0.5,-(origin.z+pSect->xTopPlane.ziPlane));
	GEPOINT xBtmPoint(origin.x+pSect->dfBtmWidth*0.5,-(origin.z+pSect->ziBtmPlane));
	if( pSect->idPanelType==PLAN_SECTION::SECT_HALF_UP||
		pSect->idPanelType==PLAN_SECTION::SECT_SUBLEG)
	{
		pDxfFile->NewLine(xTopPoint,xBtmPoint);
		pDxfFile->NewLine(f3dPoint(xTopPoint.x-pSect->xTopPlane.dfWidth,xTopPoint.y),f3dPoint(xBtmPoint.x-pSect->dfBtmWidth,xBtmPoint.y));
		//�����
		pDxfFile->NewLine(f3dPoint(xTopPoint.x-pSect->xTopPlane.dfWidth,xTopPoint.y),f3dPoint(xTopPoint.x,xTopPoint.y));
		//����Ʋ��
		pDxfFile->NewLine(f3dPoint(xTopPoint.x-pSect->xTopPlane.dfWidth/2,xTopPoint.y),f3dPoint(xBtmPoint.x-pSect->dfBtmWidth,xBtmPoint.y));
		//�������
		pDxfFile->NewLine(f3dPoint(xTopPoint.x-pSect->xTopPlane.dfWidth/2,xTopPoint.y),f3dPoint(xBtmPoint.x,xBtmPoint.y));
	}
	else if(pSect->idPanelType==PLAN_SECTION::SECT_HALF_DOWN)
	{
		pDxfFile->NewLine(xTopPoint,xBtmPoint);
		pDxfFile->NewLine(f3dPoint(xTopPoint.x-pSect->xTopPlane.dfWidth,xTopPoint.y),f3dPoint(xBtmPoint.x-pSect->dfBtmWidth,xBtmPoint.y));
		//�����
		pDxfFile->NewLine(f3dPoint(xBtmPoint.x-pSect->dfBtmWidth,xBtmPoint.y),f3dPoint(xBtmPoint.x,xBtmPoint.y));
		//����Ʋ��
		pDxfFile->NewLine(f3dPoint(xBtmPoint.x-pSect->dfBtmWidth/2,xBtmPoint.y),f3dPoint(xTopPoint.x-pSect->xTopPlane.dfWidth,xTopPoint.y));
		//�������
		pDxfFile->NewLine(f3dPoint(xBtmPoint.x-pSect->dfBtmWidth/2,xBtmPoint.y),f3dPoint(xTopPoint.x,xTopPoint.y));
	}
	else //if(pSect->idPanelType==PLAN_SECTION::SECT_WHOLE)
	{
		pDxfFile->NewLine(xTopPoint,xBtmPoint);
		pDxfFile->NewLine(f3dPoint(xTopPoint.x-pSect->xTopPlane.dfWidth,xTopPoint.y),f3dPoint(xBtmPoint.x-pSect->dfBtmWidth,xBtmPoint.y));
		//X��Ʋ��
		pDxfFile->NewLine(f3dPoint(xTopPoint.x,xTopPoint.y),f3dPoint(xBtmPoint.x-pSect->dfBtmWidth,xBtmPoint.y));
		//X�����
		pDxfFile->NewLine(f3dPoint(xTopPoint.x-pSect->xTopPlane.dfWidth,xTopPoint.y),xBtmPoint);
	}
}
#include "LDS.h"
#include "MainFrm.h"
bool Test_TrunkPlanning(CTower* pTower=NULL)
{
	if(pTower==NULL)
		pTower=&Ta;
	CUndoOperObject undo(pTower,true);
	CTrunkBodyPlanner trunk;
	//��䵱ǰ���߹滮��Ϣ
	trunk.uiMinHeight=30000;	//***
	trunk.uiMaxHeight=54000;	//***
	trunk.wnHeightGroup=9;		//***
	trunk.wiLegDiffLevel=1000;	//***
	trunk.wnMaxLegCount=9;
	trunk.InitTrunkInfo();
	CHashListEx<HEIGHTGROUP> hashPlanHeights;
	CLogErrorLife life(&logerr);
	if(!trunk.UpdateTowerTrunk(pTower,&hashPlanHeights))
		return false;
	CMainFrame* pMainFrm=(CMainFrame*)theApp.GetMainWnd();
	pMainFrm->GetTowerTreePage()->RefreshTreeCtrl();
	HEIGHTGROUP* pHeight=hashPlanHeights.GetFirst();
	long liMaxWidth=ftoi(pHeight->xBtmPlane.dfWidth);
	liMaxWidth+=1000-liMaxWidth%500;

	CDxfFile dxffile;
	dxffile.extmin.Set(-liMaxWidth);
	dxffile.extmax.x=hashPlanHeights.Count*liMaxWidth;
	dxffile.extmax.y=trunk.ziHeight0Plane+pHeight->uiNamedHeight;
	dxffile.OpenFile("d:/trunk.dxf");
	PRESET_ARRAY64<bool> xarrSegStates;
	xarrSegStates.ZeroPresetMemory();
	GEPOINT origin;
	int iOffsetIndex=0;
	for(pHeight=hashPlanHeights.GetFirst();pHeight;pHeight=hashPlanHeights.GetNext(),iOffsetIndex++)
	{
		origin.x+=liMaxWidth;
		for(TRUNK_SEGMENT* pSegment=trunk.hashBodySegments.GetFirst();pSegment;pSegment=trunk.hashBodySegments.GetNext())
		{
			bool* pbState=xarrSegStates.GetAt(pSegment->Id);
			if(pbState!=NULL&&*pbState)
				continue;
			if(pSegment->cfgword.IsHasBodyNo(pHeight->ciBodySerial))
			{
				DrawSegment(origin,pSegment,&dxffile);
				xarrSegStates.Set(pSegment->Id,true);
			}
		}
		//������������߼���
		for(UINT j=0;j<pHeight->xarrSubLegs.Count;j++)
			DrawSegment(origin,pHeight->xarrSubLegs.GetAt(j),&dxffile);
	}
	dxffile.CloseFile();
	return true;

	/*HEIGHTGROUP height;
	TRUNK_SEGMENT body;
	double dfMinHeight,dfMaxHeight;
	body.dfTopWidth=4700;
	body.dfSlopeGradient=height.dfSlopeGradient=0.11;
	body.CalUpHalfSectHeight(&dfMinHeight,&dfMaxHeight,34.8,40);
	body.dfTopWidth=5085;
	body.CalWholeSectHeight(&dfMinHeight,&dfMaxHeight,25,40);
	//
	height.xBtmPlane.dfWidth=11333;
	height.xBtmPlane.ziPlane=56000;
	HEIGHTGROUP::LEGINFO xLegInfo48m,xLegInfo45m,xLegInfo42m;
	WORD wnLegCount=height.CalTopPlane(9,1000,&xLegInfo48m);
	height.xBtmPlane.dfWidth=10673;
	height.xBtmPlane.ziPlane=53000;
	wnLegCount=height.CalTopPlane(9,1000,&xLegInfo45m);
	height.xBtmPlane.dfWidth=10013;
	height.xBtmPlane.ziPlane=50000;
	wnLegCount=height.CalTopPlane(9,1000,&xLegInfo42m);
	return wnLegCount>0;*/
}
