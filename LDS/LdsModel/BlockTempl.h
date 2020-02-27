#pragma once
#include "Block.h"

struct HUMANPOST_LABELS{
	struct NODE_LABELS{
		KEY4C PTF,PTL,PTB,PTMU,PTMD;	//顶点前、左、后节点,顶柱上顶点、下顶点
		KEY4C PBF,PBL,PBB;	//底部前、左、后节点
		KEY4C PMF,PML,PMB;	//中间横隔前、左、后节点
	}nodes;
	struct ROD_LABELS{
		KEY4C RPF,RPB,RPL;	//前、后、侧撑柱
		KEY4C RH0,RHF,RHB;	//前后连杆、前、后侧横连杆
		KEY4C RT;	//梁柱衔接横杆
		KEY4C RT0;	//顶柱
	}rods;
	struct PLATE_LABELS{
		KEY4C BT1;		//端撑顶板
		KEY4C BEL,BER;	//端撑两端板
		KEY4C BSW,BSL,BSR;//端撑插板
		KEY4C BT2;		//立柱顶板
		KEY4C BLY;		//立柱插板
		KEY4C BMF,BMR,BML;//端撑连接板
	}plates;
};
struct TRIBEAM_LABELS{
	struct NODE_LABELS{
		KEY4C CLF,CLB,CRF,CRB;	//弯口（梁柱衔接口）节点
		KEY4C SLF,SLB,SRF,SRB;	//直口节点
		KEY4C NLF,NLB,NRF,NRB;	//用于预拱的节点
		KEY4C SMF,SMB;			//底部直梁中间节点
		KEY4C TL,TM,TR;			//顶梁节点
		KEY4C clf,clb,crf,crb,slf,slb,srf,srb,tl,tr;//重复节点
	}nodes;
	struct ROD_LABELS{
		KEY4C RTL,RTR;	//左(右)曲梁左(右)底部过渡传力横杆
		KEY4C LRBB,RRBB;		//脊背梁 Rod Beam Back
		KEY4C RBLF,RBLB;//左弯曲梁前/后侧杆件 Rod Beam Left    Front(Back)
		KEY4C RBRF,RBRB;//右弯曲梁前/后侧杆件 Rod Beam Right   Front(Back)
		KEY4C LBMF,LBMB,RBMF,RBMB;//中直梁前/后侧杆件 Rod Beam Middle  Front(Back)
		KEY4C RTLF,RTLB;//左侧顶梁前/后拉杆
		KEY4C RTRF,RTRB;//右侧顶梁前/后拉杆
	}rods;
	struct PLATE_LABELS{
		KEY4C BTLF,BTLB,BTRF,BTRB;	//曲梁顶板
		KEY4C BPLF,BPLB,BPRF,BPRB;	//曲梁基板
		KEY4C RPLF,RPLB,RPRF,RPRB;	//肋板
	}plates;
};
struct SQUAREBEAM_LABELS{
	struct NODE_LABELS{
		KEY4C PLFD,PLFU;	//方梁左前下/上节点
		KEY4C PLBD,PLBU;	//方梁左后下/上节点
		KEY4C PRFD,PRFU;	//方梁右前下/上节点
		KEY4C PRBD,PRBU;	//方梁右后下/上节点
		KEY4C pfd,pfu,pbd,pbu;	//重复节点
	}nodes;
	struct ROD_LABELS{
		KEY4C RFD,RFU;	//方梁前侧底部/顶部杆件
		KEY4C RBD,RBU;	//方梁后侧底部/顶部杆件
		KEY4C RLF,RLB;	//左侧方梁前侧/后侧拉杆
		KEY4C RRF,RRB;	//右侧方梁前侧/后侧拉杆
		KEY4C RLU,RRU;	//方梁顶部左侧/右侧杆件
		KEY4C RLD,RRD;	//方梁底部左侧/右侧过度传力横杆
	}rods;
};
struct CROSSARM_LABELS{
	struct NODE_LABELS
	{
		KEY4C KN1,KN2,KN3,KN4;
		KEY4C KNF,KNB;
		KEY4C kfd,kfu,kbd,kbu;
	}nodes;
	struct ROD_LABELS
	{
		KEY4C RFD,RFU;
		KEY4C RBD,RBU;
		KEY4C RW;
		KEY4C rld,rlu,rlf,rlb;
	}rods;
};
class CSubstationTemplate : public CParaBlockModelTemplate
{
	static const BYTE STROKE_SHUFLAG;//=0x01;
	static const BYTE STROKE_PIEFLAG;//=0x02;
	static const BYTE STROKE_NAFLAG ;//=0x04;
	BYTE MirLayoutStyle(int ciLayoutStyle);
protected:
	CMapList<CLDSLinePartPtr> hashmapRods;	//用于防止生成重复杆件
	CLDSNode* InsertNewScaleNode(CParaBlockModel* pBlockModel,double fScale,CLDSNode* pStart,CLDSNode* pEnd);
	CLDSLinePart* InsertNewPole(CParaBlockModel* pBloclModel,CLDSNode* pStart,CLDSNode* pEnd,int poleType,
									int layStyel,f3dPoint norm,long hRefRod=0);
	void CreateSegmentParts(CParaBlockModel* pBlockModel,CXhChar16 sLayer,KEY4C keyFStart,KEY4C keyFEnd,
							KEY4C keyBStart,KEY4C keyBEnd,KEY4C* keyTStart=NULL,KEY4C* keyTEnd=NULL,
							BYTE cbStrokeFlag=0xff,bool rightSide=false);
public:
	CXhChar16 m_sLeftFrontLayout,m_sRightFrontLayout;
	CHashPtrList<ARRAY_LIST<CXhChar16> >hashSectionLayouts;
	void RebuildModel(CParaBlockModel* pBlockModel);	//创建变电构架的几何模型
};
