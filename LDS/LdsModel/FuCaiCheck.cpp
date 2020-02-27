#include "stdafx.h"
#include "SortFunc.h"
#include "FuCaiCheck.h"

CTransFuCai::CTransFuCai(CLDSLineAngle* pJg)
{
	m_fDeitaByRodNormal=0;
	if((m_pCurrAngle=pJg)!=NULL)
	{
		if(InitTransInfo())
			InitBoltInfo();
	}
}
CTransFuCai::~CTransFuCai()
{

}
bool CTransFuCai::InitTransInfo()
{
	CLDSLinePart* pStartLinkRod=m_pCurrAngle->GetStartDatumPart();
	CLDSLinePart* pEndLinkRod=m_pCurrAngle->GetEndDatumPart();
	if(pStartLinkRod==NULL||pEndLinkRod==NULL)
		return false;
	GEPOINT vWorkPlaneNorm=CalFaceNorm(pStartLinkRod->Start(),pStartLinkRod->End(),pEndLinkRod->Start(),pEndLinkRod->End());
	if(fabs(vWorkPlaneNorm.x)<0.01&&fabs(vWorkPlaneNorm.y)>0.8)
		vWorkPlaneNorm.x=0;
	else if(fabs(vWorkPlaneNorm.y)<0.01&&fabs(vWorkPlaneNorm.x)>0.8)
		vWorkPlaneNorm.y=0;
	else
		return false;
	normalize(vWorkPlaneNorm);	//修正因各种原因带来的工作面法向X/Y轴偏差
	vRodVec=m_pCurrAngle->End()-m_pCurrAngle->Start();
	m_fNoneOddRodLength=vRodVec.mod();
	vStdRodVec=vRodVec/m_fNoneOddRodLength;
	vStdPrjRodVec=vStdRodVec-vWorkPlaneNorm*(vStdRodVec*vWorkPlaneNorm);
	normalize(vStdPrjRodVec);
	double deita=vRodVec*vWorkPlaneNorm;
	m_fSina=vStdRodVec*vWorkPlaneNorm;
	m_fCosa=vStdPrjRodVec*vStdRodVec;
	m_fDeitaByRodNormal=deita/m_fCosa;
	m_vRotateAxisVec=vWorkPlaneNorm^vStdRodVec;
	vRodNormal=vStdRodVec;
	RotateVectorAroundVector(vRodNormal,1.0,0.0,m_vRotateAxisVec);
	if(vRodNormal*vWorkPlaneNorm>0)
		vRodNormal*=-1.0;
	vPrjRodVec=vRodVec-vWorkPlaneNorm*deita;
	GEPOINT vRodSpreadVec=vRodVec+vRodNormal*m_fDeitaByRodNormal;
	m_fLenZoomCoef=vRodSpreadVec.mod()/m_fNoneOddRodLength;
	return true;
}
void CTransFuCai::TransPointToSpreadPlane(f3dPoint& pos)
{
	//1.计算直线与展开面夹角
	GEPOINT posVec=pos-m_pCurrAngle->Start();
	double m_fPosLength=posVec*vStdRodVec;
	GEPOINT posLenVec=vStdRodVec*m_fPosLength;
	GEPOINT posVertVec=posVec-posLenVec;
	RotateVectorAroundVector(posVertVec,m_fSina,m_fCosa,m_vRotateAxisVec);
	pos=m_pCurrAngle->Start()+vStdPrjRodVec*(m_fPosLength*m_fLenZoomCoef)+posVertVec;
}
double CTransFuCai::SpreadRodLength()
{
	return (m_pCurrAngle->startOdd()+m_pCurrAngle->endOdd()+m_fNoneOddRodLength)*m_fLenZoomCoef;
}
void CTransFuCai::InitBoltInfo()
{
	int wingx0_y1=0;
	GEPOINT vWingVecX=m_pCurrAngle->GetWingVecX();
	GEPOINT vWingVecY=m_pCurrAngle->GetWingVecY();
	GEPOINT interspos,localpos;
	GEPOINT vStartPos=m_pCurrAngle->Start();
	vStartPos-=vStdRodVec*m_pCurrAngle->startOdd();
	for(CLsRef *pLsRef=m_pCurrAngle->GetFirstLsRef();pLsRef;pLsRef=m_pCurrAngle->GetNextLsRef())
	{
		if(!m_pCurrAngle->GetBoltIntersPos(pLsRef->GetLsPtr(),interspos,&wingx0_y1))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Bolt(0x%X)'s position is error", pLsRef->pBolt->handle);
#else
			logerr.Log("螺栓0x%X位置错误",pLsRef->pBolt->handle);
#endif
			continue;
		}
		GEPOINT bolt_vec=interspos-vStartPos;
		localpos.x=(bolt_vec*vStdRodVec)*m_fLenZoomCoef;
		if(wingx0_y1==0)
			localpos.y=-bolt_vec*vWingVecX;
		else
			localpos.y=bolt_vec*vWingVecY;
		BOLT_INFO *pBoltInfo=m_xBoltInfoList.append();
		pBoltInfo->bolt_d=(short)(*pLsRef)->get_d();
		pBoltInfo->hole_d_increment=(float)(*pLsRef)->hole_d_increment;
		pBoltInfo->hiberId.masterId=pLsRef->GetLsPtr()->handle;
		//X坐标
		pBoltInfo->posX=(float)localpos.x;
		//Y坐标
		pBoltInfo->posY=(float)localpos.y;
	}
}
static int LocalCompareBoltInfo(const BOLT_INFO& boltinfo1,const BOLT_INFO& boltinfo2)
{
	if(boltinfo1.posX-boltinfo2.posX>EPS2)
		return 1;
	else if(boltinfo1.posX-boltinfo2.posX<-EPS2)
		return -1;
	else if(fabs(boltinfo1.posY)-fabs(boltinfo2.posY)>0)
		return 1;
	else if(fabs(boltinfo1.posY)-fabs(boltinfo2.posY)<0)
		return -1;
	else
		return 0;
}
int CTransFuCai::IsEqual(CTransFuCai *pFuCai,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
	int *pErrorType/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/,bool bSimpleCompareMode/*=false*/)
{
	if(pChkStyleFlag==NULL)
		return false;
	if(!IsInited()||!pFuCai->IsInited())	//按传统模式比对
		return -1;
	CXhChar200 errorDescript;
	m_fSpreadRodLength=SpreadRodLength();;
	double fPeerRodLength=pFuCai->SpreadRodLength();
	if(pChkStyleFlag==NULL || *pChkStyleFlag&CHKPARTNO_RODINFO)
	{	//构件基本信息
		bool bRet=true;
		if(m_fSpreadRodLength-fPeerRodLength>epsilon)
		{
			if(error_info)
				sprintf(error_info,"{%s}长度不同,差%.1f",(char*)sPartNo,fabs(m_fSpreadRodLength-fPeerRodLength));
			bRet=false;
		}
		else if(m_pCurrAngle->GetWidth()!=pFuCai->m_pCurrAngle->GetWidth()!=0)
		{
			if(error_info)
				sprintf(error_info,"{%s}肢宽不同",(char*)sPartNo);
			bRet=false;
		}
		else if(m_pCurrAngle->GetThick()!=pFuCai->m_pCurrAngle->GetThick()!=0)
		{
			if(error_info)
				sprintf(error_info,"{%s}肢厚不同",(char*)sPartNo);
			bRet=false;
		}
		else if(m_pCurrAngle->cMaterial!=pFuCai->m_pCurrAngle->cMaterial)
		{
			if(error_info)
				sprintf(error_info,"{%s}材质不同",(char*)sPartNo);
			bRet=false;
		}
		if(!bRet)
		{
			if(pErrorType!=NULL)
				*pErrorType=CHKPARTNO_RODINFO;
			return false;
		}
	}
	int i,j;
	bool reverseStartEnd=false;
	if(pChkStyleFlag==NULL || *pChkStyleFlag&CHKPARTNO_RODBOLTINFO)
	{	//检查螺栓信息是否相同
		BOLT_INFO* pBoltInfo,*pPeerBoltInfo;
		int count=m_xBoltInfoList.GetNodeNum(),count2=pFuCai->m_xBoltInfoList.GetNodeNum();
		ARRAY_LIST<BOLT_INFO> listBoltsWingX,listBoltsWingY;
		ARRAY_LIST<BOLT_INFO> listBoltsWingX2,listBoltsWingY2;
		for(pBoltInfo=m_xBoltInfoList.GetFirst();pBoltInfo;pBoltInfo=m_xBoltInfoList.GetNext())
		{
			if(pBoltInfo->posY<=0)	//X肢
				listBoltsWingX.append(*pBoltInfo);
			else
				listBoltsWingY.append(*pBoltInfo);
		}
		for(pBoltInfo=pFuCai->m_xBoltInfoList.GetFirst();pBoltInfo;pBoltInfo=pFuCai->m_xBoltInfoList.GetNext())
		{
			if(pBoltInfo->posY<=0)	//X肢
				listBoltsWingX2.append(*pBoltInfo);
			else
				listBoltsWingY2.append(*pBoltInfo);
		}
		CQuickSort<BOLT_INFO>::QuickSort(listBoltsWingX.m_pData,listBoltsWingX.GetSize(),LocalCompareBoltInfo);
		CQuickSort<BOLT_INFO>::QuickSort(listBoltsWingY.m_pData,listBoltsWingY.GetSize(),LocalCompareBoltInfo);
		CQuickSort<BOLT_INFO>::QuickSort(listBoltsWingX2.m_pData,listBoltsWingX2.GetSize(),LocalCompareBoltInfo);
		CQuickSort<BOLT_INFO>::QuickSort(listBoltsWingY2.m_pData,listBoltsWingY2.GetSize(),LocalCompareBoltInfo);
		bool boltmatched=false;
		if(bSimpleCompareMode)
		{	//简单对比模式，不进行具体螺栓的匹配性检查，只要发现不能一致就停止比对
			double epsilon_square=epsilon*epsilon;
			if(listBoltsWingX.GetSize()==listBoltsWingX2.GetSize() && listBoltsWingY.GetSize()==listBoltsWingY2.GetSize())
			{	//进行正向螺栓比对
				boltmatched=true;
				for(i=0;i<listBoltsWingX.GetSize();i++)
				{
					double deitax=listBoltsWingX[i].posX-listBoltsWingX2[i].posX;
					double deitay=listBoltsWingX[i].posY-listBoltsWingX2[i].posY;
					if(fabs(deitax)>epsilon)
						boltmatched=false;	//对应螺栓位置Ｘ值偏差太大
					if(boltmatched&&fabs(deitay)>epsilon)
						boltmatched=false;	//对应螺栓位置Ｙ值偏差太大
					if(boltmatched&&deitax*deitax+deitay*deitay>epsilon_square)
						boltmatched=false;	//对应螺栓位置偏差太大
					if(boltmatched&&listBoltsWingX[i].bolt_d!=listBoltsWingX2[i].bolt_d)
						boltmatched=false;	//螺栓直径不同
					if(boltmatched&&(listBoltsWingX[i].bolt_d+listBoltsWingX[i].hole_d_increment)!=(listBoltsWingX2[i].bolt_d+listBoltsWingX2[i].hole_d_increment))
						boltmatched=false;	//螺栓孔径不同
					if(!boltmatched)
						break;
				}
				for(i=0;boltmatched&&i<listBoltsWingY.GetSize();i++)
				{
					double deitax=listBoltsWingY[i].posX-listBoltsWingY2[i].posX;
					double deitay=listBoltsWingY[i].posY-listBoltsWingY2[i].posY;
					if(fabs(deitax)>epsilon)
						boltmatched=false;	//对应螺栓位置Ｘ值偏差太大
					if(boltmatched&&fabs(deitay)>epsilon)
						boltmatched=false;	//对应螺栓位置Ｙ值偏差太大
					if(boltmatched && deitax*deitax+deitay*deitay>epsilon_square)
						boltmatched=false;	//对应螺栓位置偏差太大
					if(boltmatched && listBoltsWingY[i].bolt_d!=listBoltsWingY2[i].bolt_d)
						boltmatched=false;	//螺栓直径不同
					if(boltmatched &&(listBoltsWingY[i].bolt_d+listBoltsWingY[i].hole_d_increment)!=(listBoltsWingY2[i].bolt_d+listBoltsWingY2[i].hole_d_increment))
						boltmatched=false;	//螺栓孔径不同
					if(!boltmatched)
						break;
				}
			}
			if(!boltmatched&&listBoltsWingX.GetSize()==listBoltsWingY2.GetSize() && listBoltsWingY.GetSize()==listBoltsWingX2.GetSize())
			{
				boltmatched=true;
				count=listBoltsWingX.GetSize();
				for(i=0;i<count;i++)
				{
					double deitax=listBoltsWingX[i].posX-(fPeerRodLength-listBoltsWingY2[count-1-i].posX);
					double deitay=listBoltsWingX[i].posY+listBoltsWingY2[count-1-i].posY;
					if(fabs(deitax)>epsilon)
						boltmatched=false;	//对应螺栓位置Ｘ值偏差太大
					if(boltmatched&&fabs(deitay)>epsilon)
						boltmatched=false;	//对应螺栓位置Ｙ值偏差太大
					if(boltmatched&&deitax*deitax+deitay*deitay>epsilon_square)
						boltmatched=false;	//对应螺栓位置偏差太大
					if(boltmatched&&listBoltsWingX[i].bolt_d!=listBoltsWingY2[count-1-i].bolt_d)
						boltmatched=false;	//螺栓直径不同
					if(boltmatched&&(listBoltsWingX[i].bolt_d+listBoltsWingX[i].hole_d_increment)!=(listBoltsWingY2[count-1-i].bolt_d+listBoltsWingY2[count-1-i].hole_d_increment))
						boltmatched=false;	//螺栓孔径不同
					if(!boltmatched)
						break;
				}
				count=listBoltsWingY.GetSize();
				for(i=0;boltmatched&&i<listBoltsWingY.GetSize();i++)
				{
					double deitax=listBoltsWingY[i].posX-(fPeerRodLength-listBoltsWingX2[count-1-i].posX);
					double deitay=listBoltsWingY[i].posY+listBoltsWingX2[count-1-i].posY;
					if(fabs(deitax)>epsilon)
						boltmatched=false;	//对应螺栓位置Ｘ值偏差太大
					if(boltmatched&&fabs(deitay)>epsilon)
						boltmatched=false;	//对应螺栓位置Ｙ值偏差太大
					if(boltmatched && deitax*deitax+deitay*deitay>epsilon_square)
						boltmatched=false;	//对应螺栓位置偏差太大
					if(boltmatched && listBoltsWingY[i].bolt_d!=listBoltsWingX2[count-1-i].bolt_d)
						boltmatched=false;	//螺栓直径不同
					if(boltmatched && (listBoltsWingY[i].bolt_d+listBoltsWingY[i].hole_d_increment)!=(listBoltsWingX2[count-1-i].bolt_d+listBoltsWingX2[count-1-i].hole_d_increment))
						boltmatched=false;	//螺栓孔径不同
					if(!boltmatched)
						break;
				}
				reverseStartEnd=boltmatched;
			}
			if(!boltmatched)
				return false;
		}
		else
		{	//进行完整对比模式，不管是否一致都要进行完整的最佳螺栓螺栓的匹配性检查
			CHashListEx<CLDSPart::PEER_BOLT> hashPeerBolts;			//从this对象角度看
			CHashListEx<CLDSPart::PEER_BOLT> hashPeerBolts2;		//从pFuCai对象角度看
			CHashListEx<CLDSPart::PEER_BOLT> hashReversePeerBolts;	//从this对象角度看，逆向比对
			CHashListEx<CLDSPart::PEER_BOLT> hashReversePeerBolts2;	//从pFuCai对象角度看，逆向比对
			CLDSPart::PEER_BOLT* pPeerBolt,*pPeerBolt2;
			double boltMatchIndex=0,reverseBoltMatchIndex=0;
			//进行正向螺栓比对
			count=listBoltsWingX.GetSize();
			count2=listBoltsWingX2.GetSize();
			for(i=0;i<count;i++)
			{
				pBoltInfo=&listBoltsWingX[i];
				CLDSPart::PEER_BOLT basebolt(pBoltInfo->hiberId.masterId);
				for(j=0;j<count2;j++)
				{
					pPeerBoltInfo=&listBoltsWingX2[j];
					double deitax=pBoltInfo->posX-pPeerBoltInfo->posX;
					double deitay=pBoltInfo->posY-pPeerBoltInfo->posY;
					double dist=SQRT(deitax*deitax+deitay*deitay);
					long hPeerBolt=pPeerBoltInfo->hiberId.masterId;
					pPeerBolt2=hashPeerBolts2.GetValue(hPeerBolt);
					if(dist>=50||(pPeerBolt2&&dist>=pPeerBolt2->difference))
						continue;	//当前待匹配螺栓已有更合适的螺栓匹配关联
					else if(basebolt.hPeerBolt==0)
					{
						if(pPeerBolt2)
							hashPeerBolts.DeleteNode(pPeerBolt2->hPeerBolt);
						basebolt.hPeerBolt=hPeerBolt;
						basebolt.difference=dist;
						if(basebolt.difference>epsilon)
							basebolt.ciState =1;
						if((pBoltInfo->bolt_d+pBoltInfo->hole_d_increment)!=(pPeerBoltInfo->bolt_d+pPeerBoltInfo->hole_d_increment))
							basebolt.ciState|=2;	//螺栓孔径不同
						hashPeerBolts2.Add(hPeerBolt)->hPeerBolt=basebolt.hBaseBolt;
					}
					else if(dist<basebolt.difference)
					{
						hashPeerBolts2.DeleteNode(basebolt.hPeerBolt);
						basebolt.hPeerBolt=hPeerBolt;
						hashPeerBolts2.Add(hPeerBolt)->hPeerBolt=basebolt.hBaseBolt;
						basebolt.difference=dist;
						basebolt.ciState=0;
						if(basebolt.difference>epsilon)
							basebolt.ciState =1;
						if((pBoltInfo->bolt_d+pBoltInfo->hole_d_increment)!=(pPeerBoltInfo->bolt_d+pPeerBoltInfo->hole_d_increment))
							basebolt.ciState|=2;	//螺栓孔径不同
					}
				}
				if(basebolt.hPeerBolt>0x20)
					hashPeerBolts.SetValue(basebolt.hBaseBolt,basebolt);
			}
			count =listBoltsWingY.GetSize();
			count2=listBoltsWingY2.GetSize();
			for(i=0;i<count;i++)
			{
				pBoltInfo=&listBoltsWingY[i];
				CLDSPart::PEER_BOLT basebolt(pBoltInfo->hiberId.masterId);
				for(j=0;j<count2;j++)
				{
					pPeerBoltInfo=&listBoltsWingY2[j];
					double deitax=listBoltsWingY[i].posX-listBoltsWingY2[j].posX;
					double deitay=listBoltsWingY[i].posY-listBoltsWingY2[j].posY;
					double dist=SQRT(deitax*deitax+deitay*deitay);
					long hPeerBolt=listBoltsWingY2[j].hiberId.masterId;
					pPeerBolt2=hashPeerBolts2.GetValue(hPeerBolt);
					if(dist>=50 || (pPeerBolt2&&dist>=pPeerBolt2->difference))
						continue;	//当前待匹配螺栓已有更合适的螺栓匹配关联
					else if(basebolt.hPeerBolt==0)
					{
						if(pPeerBolt2)
							hashPeerBolts.DeleteNode(pPeerBolt2->hPeerBolt);
						basebolt.hPeerBolt=hPeerBolt;
						basebolt.difference=dist;
						if(basebolt.difference>epsilon)
							basebolt.ciState=1;
						if((pBoltInfo->bolt_d+pBoltInfo->hole_d_increment)!=(pPeerBoltInfo->bolt_d+pPeerBoltInfo->hole_d_increment))
							basebolt.ciState|=2;	//螺栓孔径不同
						hashPeerBolts2.Add(basebolt.hPeerBolt)->hPeerBolt=basebolt.hBaseBolt;
					}
					else if(dist<basebolt.difference)
					{
						hashPeerBolts2.DeleteNode(basebolt.hPeerBolt);
						basebolt.hPeerBolt=hPeerBolt;
						hashPeerBolts2.Add(basebolt.hPeerBolt)->hPeerBolt=basebolt.hBaseBolt;
						basebolt.difference=dist;
						basebolt.ciState=0;
						if(basebolt.difference>epsilon)
							basebolt.ciState =1;
						if((pBoltInfo->bolt_d+pBoltInfo->hole_d_increment)!=(pPeerBoltInfo->bolt_d+pPeerBoltInfo->hole_d_increment))
							basebolt.ciState|=2;	//螺栓孔径不同
					}
				}
				if(basebolt.hPeerBolt>0x20)
					hashPeerBolts.SetValue(basebolt.hBaseBolt,basebolt);
			}
			if(hashPeerBolts.GetNodeNum()==m_xBoltInfoList.GetNodeNum()&&hashPeerBolts2.GetNodeNum()==pFuCai->m_xBoltInfoList.GetNodeNum())
				boltmatched=true;
			boltMatchIndex=0;
			for(pPeerBolt=hashPeerBolts.GetFirst();pPeerBolt;pPeerBolt=hashPeerBolts.GetNext())
			{
				if(pPeerBolt->ciState&0x01)
					boltMatchIndex+=pPeerBolt->difference;
				if(pPeerBolt->ciState!=0)
					boltmatched=false;
			}
			if(boltMatchIndex>epsilon)
			{	//进行反向比对
				count=listBoltsWingX.GetSize();
				count2=listBoltsWingY2.GetSize();
				for(i=0;i<count;i++)
				{
					pBoltInfo=&listBoltsWingX[i];
					CLDSPart::PEER_BOLT basebolt(pBoltInfo->hiberId.masterId);
					for(j=count2-1;j>=0;j--)
					{
						pPeerBoltInfo=&listBoltsWingY2[j];
						double deitax=pBoltInfo->posX-(fPeerRodLength-pPeerBoltInfo->posX);
						double deitay=pBoltInfo->posY+pPeerBoltInfo->posY;
						double dist=SQRT(deitax*deitax+deitay*deitay);
						long hPeerBolt=listBoltsWingY2[j].hiberId.masterId;
						CLDSPart::PEER_BOLT* pPeerBolt2=hashReversePeerBolts2.GetValue(hPeerBolt);
						if(dist>=50 || pPeerBolt2&&dist>=pPeerBolt2->difference)
							continue;	//当前待匹配螺栓已有更合适的螺栓匹配关联
						else if(pPeerBolt->hPeerBolt==0)
						{
							if(pPeerBolt2)
								hashReversePeerBolts.DeleteNode(pPeerBolt2->hPeerBolt);
							basebolt.hPeerBolt=hPeerBolt;
							basebolt.difference=dist;
							if(basebolt.difference>epsilon)
								basebolt.ciState=1;
							if((pBoltInfo->bolt_d+pBoltInfo->hole_d_increment)!=(pPeerBoltInfo->bolt_d+pPeerBoltInfo->hole_d_increment))
								basebolt.ciState|=2;	//螺栓孔径不同
							hashReversePeerBolts2.Add(basebolt.hPeerBolt)->hPeerBolt=basebolt.hBaseBolt;
						}
						else if(dist<pPeerBolt->difference)
						{
							hashReversePeerBolts2.DeleteNode(basebolt.hPeerBolt);
							basebolt.hPeerBolt=hPeerBolt;
							hashReversePeerBolts2.Add(hPeerBolt)->hPeerBolt=basebolt.hBaseBolt;
							basebolt.difference=dist;
							basebolt.ciState=0;
							if(basebolt.difference>epsilon)
								basebolt.ciState=1;
							if((pBoltInfo->bolt_d+pBoltInfo->hole_d_increment)!=(pPeerBoltInfo->bolt_d+pPeerBoltInfo->hole_d_increment))
								basebolt.ciState=2;	//螺栓孔径不同
						}
					}
					if(basebolt.hPeerBolt>0x20)
						hashReversePeerBolts.SetValue(basebolt.hBaseBolt,basebolt);
				}
				count=listBoltsWingY.GetSize();
				count2=listBoltsWingX2.GetSize();
				for(i=0;i<count;i++)
				{
					pBoltInfo=&listBoltsWingY[i];
					CLDSPart::PEER_BOLT basebolt(pBoltInfo->hiberId.masterId);
					for(j=count2-1;j>=0;j--)
					{
						pPeerBoltInfo=&listBoltsWingX2[j];
						double deitax=pBoltInfo->posX-(fPeerRodLength-pPeerBoltInfo->posX);
						double deitay=pBoltInfo->posY+pPeerBoltInfo->posY;
						double dist=SQRT(deitax*deitax+deitay*deitay);
						long hPeerBolt=pPeerBoltInfo->hiberId.masterId;
						CLDSPart::PEER_BOLT* pPeerBolt2=hashReversePeerBolts2.GetValue(hPeerBolt);
						if(dist>=50 || pPeerBolt2&&dist>=pPeerBolt2->difference)
							continue;	//当前待匹配螺栓已有更合适的螺栓匹配关联
						else if(pPeerBolt->hPeerBolt==0)
						{
							if(pPeerBolt2)
								hashReversePeerBolts.DeleteNode(pPeerBolt2->hPeerBolt);
							basebolt.hPeerBolt=hPeerBolt;
							basebolt.difference=dist;
							if(basebolt.difference>epsilon)
								basebolt.ciState=1;
							if((pBoltInfo->bolt_d+pBoltInfo->hole_d_increment)!=(pPeerBoltInfo->bolt_d+pPeerBoltInfo->hole_d_increment))
								basebolt.ciState|=2;	//螺栓孔径不同
							hashReversePeerBolts2.Add(basebolt.hPeerBolt)->hPeerBolt=basebolt.hBaseBolt;
						}
						else if(dist<pPeerBolt->difference)
						{
							hashReversePeerBolts2.DeleteNode(basebolt.hPeerBolt);
							basebolt.hPeerBolt=hPeerBolt;
							hashReversePeerBolts2.Add(hPeerBolt)->hPeerBolt=basebolt.hBaseBolt;
							basebolt.difference=dist;
							basebolt.ciState=0;
							if(basebolt.difference>epsilon)
								basebolt.ciState=1;
							if((pBoltInfo->bolt_d+pBoltInfo->hole_d_increment)!=(pPeerBoltInfo->bolt_d+pPeerBoltInfo->hole_d_increment))
								basebolt.ciState=2;	//螺栓孔径不同
						}
					}
					if(basebolt.hPeerBolt>0x20)
						hashReversePeerBolts.SetValue(basebolt.hBaseBolt,basebolt);
				}
				if(hashReversePeerBolts.GetNodeNum()==m_xBoltInfoList.GetNodeNum()&&hashReversePeerBolts2.GetNodeNum()==pFuCai->m_xBoltInfoList.GetNodeNum())
					boltmatched=true;
				reverseBoltMatchIndex=0;
				for(pPeerBolt=hashReversePeerBolts.GetFirst();pPeerBolt;pPeerBolt=hashReversePeerBolts.GetNext())
				{
					if(pPeerBolt->ciState&0x01)
						reverseBoltMatchIndex+=pPeerBolt->difference;
					if(pPeerBolt->ciState!=0)
						boltmatched=false;
				}
			}
			if(boltMatchIndex>epsilon&&boltMatchIndex>reverseBoltMatchIndex)
				reverseStartEnd=true;
			else
				reverseStartEnd=false;
		}
		if(!boltmatched)
		{
			if(error_info)
				sprintf(error_info,"{%s}角钢螺栓信息不同",(char*)m_pCurrAngle->GetPartNo());
			if(pErrorType!=NULL)
				*pErrorType=CHKPARTNO_RODBOLTINFO;
			return false;
		}
	}
	if(pChkStyleFlag==NULL||*pChkStyleFlag&CHKPARTNO_ANGLECUT)
	{	//检查压扁切角信息是否相同
		bool checkpassed=true;
		if(!reverseStartEnd)
		{	//正向对比特殊工艺（压扁及切角)信息
			if( m_pCurrAngle->start_push_flat_x1_y2!=pFuCai->m_pCurrAngle->start_push_flat_x1_y2||
				m_pCurrAngle->start_push_flat_x1_y2>0&&fabs(m_pCurrAngle->start_push_flat_length-pFuCai->m_pCurrAngle->start_push_flat_length)>EPS)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				errorDescript.Printf("{%s} start push flat data is different",(char*)sPartNo); 
#else
				errorDescript.Printf("{%s}始端压扁数据不同",(char*)sPartNo); 
#endif
				checkpassed=false;
			}
			if( m_pCurrAngle->end_push_flat_x1_y2!=pFuCai->m_pCurrAngle->end_push_flat_x1_y2||
				m_pCurrAngle->end_push_flat_x1_y2>0&&fabs(m_pCurrAngle->end_push_flat_length-pFuCai->m_pCurrAngle->end_push_flat_length)>EPS)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				errorDescript.Printf("{%s} end push flat data is different",(char*)sPartNo); 
#else
				errorDescript.Printf("{%s}终端压扁数据不同",(char*)sPartNo); 
#endif
				checkpassed=false;
			}
			if(!checkpassed)
			{
				if(error_info)
					strcpy(error_info,errorDescript);
				if(pErrorType!=NULL)
					*pErrorType=CHKPARTNO_ANGLECUT;
				return false;
			}
			for(i=0;i<2;i++)
			{
				if(m_pCurrAngle->cut_wing[i]!=pFuCai->m_pCurrAngle->cut_wing[i])
				{	//记录正比信息，便于准确输出 wht 11-05-26
#ifdef AFX_TARG_ENU_ENGLISH
					errorDescript.Printf("{%s} cut leg data is different",(char*)sPartNo); 
#else
					errorDescript.Printf("{%s}切肢数据不同",(char*)sPartNo); 
#endif
					checkpassed=false;
				}
				if(m_pCurrAngle->cut_wing[i]>0)
				{
					for(j=0;j<3;j++)
					{
						if(fabs(m_pCurrAngle->cut_wing_para[i][j]-pFuCai->m_pCurrAngle->cut_wing_para[i][j])>epsilon)
						{	//记录正比信息，便于准确输出 wht 11-05-26
#ifdef AFX_TARG_ENU_ENGLISH
							errorDescript.Printf("{%s} cut leg data is different",(char*)sPartNo);
#else
							errorDescript.Printf("{%s}切肢数据不同",(char*)sPartNo);
#endif
							checkpassed=false;
							break;
						}
					}
				}
				for(j=0;j<4;j++)
				{
					if(fabs(m_pCurrAngle->cut_angle[j][i]-pFuCai->m_pCurrAngle->cut_angle[j][i])>epsilon)
					{	//记录正比信息，便于准确输出 wht 11-05-26
#ifdef AFX_TARG_ENU_ENGLISH
						errorDescript.Printf("{%s} cut leg data is different",(char*)sPartNo);
#else
						errorDescript.Printf("{%s}切角数据不同",(char*)sPartNo);
#endif
						checkpassed=false;
						break;
					}
				}
			}
			if(!checkpassed)
			{
				if(error_info)
					strcpy(error_info,errorDescript);
				if(pErrorType!=NULL)
					*pErrorType=CHKPARTNO_ANGLECUT;
				return false;
			}
		}
		else
		{	//反向对比特殊工艺（压扁及切角)信息
			for(i=0;i<2;i++)
			{
				bool bRetCode=true;
				if(m_pCurrAngle->cut_wing[i]+pFuCai->m_pCurrAngle->cut_wing[1-i]!=0 && m_pCurrAngle->cut_wing[i]+pFuCai->m_pCurrAngle->cut_wing[1-i]!=3)
				{	//至少有一根端头有切肢，但X/Y肢不相符 wjh-2014.4.8
#ifdef AFX_TARG_ENU_ENGLISH
					errorDescript.Printf("{%s} cut leg data is different",(char*)sPartNo);
#else
					errorDescript.Printf("{%s}切肢数据不同",(char*)sPartNo);
#endif
					checkpassed=false;
				}
				else if(m_pCurrAngle->cut_wing[i]+pFuCai->m_pCurrAngle->cut_wing[1-i]==3)
				{	//端头都有切肢，且X/Y肢相符 wjh-2014.4.8
					for(j=0;j<3;j++)
					{
						if(fabs(m_pCurrAngle->cut_wing_para[i][j]-pFuCai->m_pCurrAngle->cut_wing_para[1-i][j])>2)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							errorDescript.Printf("{%s} cut leg data is different",(char*)sPartNo);
#else
							errorDescript.Printf("{%s}切肢数据不同",(char*)sPartNo);
#endif
							checkpassed=false;
							break;
						}
					}
				}
				//else if(cut_wing[i]+pJg->cut_wing[1-i]==0)	//两根角钢都无切肢，不需考虑切肢信息比对
				for(j=0;checkpassed&&j<4;j++)
				{
					if(fabs(m_pCurrAngle->cut_angle[j][i]-pFuCai->m_pCurrAngle->cut_angle[3-j][i])>2)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						errorDescript.Printf("{%s} cut leg data is different",(char*)sPartNo);
#else
						errorDescript.Printf("{%s}切角数据不同",(char*)sPartNo);
#endif
						checkpassed=false;
						break;
					}
				}
				if(!checkpassed)
				{
					if(error_info)
						strcpy(error_info,errorDescript);
					if(pErrorType!=NULL)
						*pErrorType=CHKPARTNO_ANGLECUT;
					return false;
				}
			}
			if( (m_pCurrAngle->start_push_flat_x1_y2>0||pFuCai->m_pCurrAngle->end_push_flat_x1_y2>0)&&
				(m_pCurrAngle->start_push_flat_x1_y2==pFuCai->m_pCurrAngle->end_push_flat_x1_y2||fabs(m_pCurrAngle->start_push_flat_length-pFuCai->m_pCurrAngle->end_push_flat_length)>EPS))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				errorDescript.Printf("{%s} push flat data is different",(char*)sPartNo);
#else
				errorDescript.Printf("{%s}压扁数据不同",(char*)sPartNo);
#endif
				checkpassed=false;
			}
			if( checkpassed&&
				(m_pCurrAngle->end_push_flat_x1_y2>0||pFuCai->m_pCurrAngle->start_push_flat_x1_y2>0)&&
				(m_pCurrAngle->end_push_flat_x1_y2==pFuCai->m_pCurrAngle->start_push_flat_x1_y2||fabs(m_pCurrAngle->end_push_flat_length-pFuCai->m_pCurrAngle->start_push_flat_length)>EPS))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				errorDescript.Printf("{%s} push flat data is different",(char*)sPartNo);
#else
				errorDescript.Printf("{%s}压扁数据不同",(char*)sPartNo);
#endif
				checkpassed=false;
			}
			if(!checkpassed)
			{
				if(error_info)
					strcpy(error_info,errorDescript);
				if(pErrorType!=NULL)
					*pErrorType=CHKPARTNO_ANGLECUT;
				return false;
			}
		}
	}
	return true;
}
