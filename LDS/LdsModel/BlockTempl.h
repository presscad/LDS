#pragma once
#include "Block.h"

struct HUMANPOST_LABELS{
	struct NODE_LABELS{
		KEY4C PTF,PTL,PTB,PTMU,PTMD;	//����ǰ���󡢺�ڵ�,�����϶��㡢�¶���
		KEY4C PBF,PBL,PBB;	//�ײ�ǰ���󡢺�ڵ�
		KEY4C PMF,PML,PMB;	//�м���ǰ���󡢺�ڵ�
	}nodes;
	struct ROD_LABELS{
		KEY4C RPF,RPB,RPL;	//ǰ���󡢲����
		KEY4C RH0,RHF,RHB;	//ǰ�����ˡ�ǰ����������
		KEY4C RT;	//�����νӺ��
		KEY4C RT0;	//����
	}rods;
	struct PLATE_LABELS{
		KEY4C BT1;		//�˳Ŷ���
		KEY4C BEL,BER;	//�˳����˰�
		KEY4C BSW,BSL,BSR;//�˳Ų��
		KEY4C BT2;		//��������
		KEY4C BLY;		//�������
		KEY4C BMF,BMR,BML;//�˳����Ӱ�
	}plates;
};
struct TRIBEAM_LABELS{
	struct NODE_LABELS{
		KEY4C CLF,CLB,CRF,CRB;	//��ڣ������νӿڣ��ڵ�
		KEY4C SLF,SLB,SRF,SRB;	//ֱ�ڽڵ�
		KEY4C NLF,NLB,NRF,NRB;	//����Ԥ���Ľڵ�
		KEY4C SMF,SMB;			//�ײ�ֱ���м�ڵ�
		KEY4C TL,TM,TR;			//�����ڵ�
		KEY4C clf,clb,crf,crb,slf,slb,srf,srb,tl,tr;//�ظ��ڵ�
	}nodes;
	struct ROD_LABELS{
		KEY4C RTL,RTR;	//��(��)������(��)�ײ����ɴ������
		KEY4C LRBB,RRBB;		//������ Rod Beam Back
		KEY4C RBLF,RBLB;//��������ǰ/���˼� Rod Beam Left    Front(Back)
		KEY4C RBRF,RBRB;//��������ǰ/���˼� Rod Beam Right   Front(Back)
		KEY4C LBMF,LBMB,RBMF,RBMB;//��ֱ��ǰ/���˼� Rod Beam Middle  Front(Back)
		KEY4C RTLF,RTLB;//��ඥ��ǰ/������
		KEY4C RTRF,RTRB;//�Ҳඥ��ǰ/������
	}rods;
	struct PLATE_LABELS{
		KEY4C BTLF,BTLB,BTRF,BTRB;	//��������
		KEY4C BPLF,BPLB,BPRF,BPRB;	//��������
		KEY4C RPLF,RPLB,RPRF,RPRB;	//�߰�
	}plates;
};
struct SQUAREBEAM_LABELS{
	struct NODE_LABELS{
		KEY4C PLFD,PLFU;	//������ǰ��/�Ͻڵ�
		KEY4C PLBD,PLBU;	//���������/�Ͻڵ�
		KEY4C PRFD,PRFU;	//������ǰ��/�Ͻڵ�
		KEY4C PRBD,PRBU;	//�����Һ���/�Ͻڵ�
		KEY4C pfd,pfu,pbd,pbu;	//�ظ��ڵ�
	}nodes;
	struct ROD_LABELS{
		KEY4C RFD,RFU;	//����ǰ��ײ�/�����˼�
		KEY4C RBD,RBU;	//�������ײ�/�����˼�
		KEY4C RLF,RLB;	//��෽��ǰ��/�������
		KEY4C RRF,RRB;	//�Ҳ෽��ǰ��/�������
		KEY4C RLU,RRU;	//�����������/�Ҳ�˼�
		KEY4C RLD,RRD;	//�����ײ����/�Ҳ���ȴ������
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
	CMapList<CLDSLinePartPtr> hashmapRods;	//���ڷ�ֹ�����ظ��˼�
	CLDSNode* InsertNewScaleNode(CParaBlockModel* pBlockModel,double fScale,CLDSNode* pStart,CLDSNode* pEnd);
	CLDSLinePart* InsertNewPole(CParaBlockModel* pBloclModel,CLDSNode* pStart,CLDSNode* pEnd,int poleType,
									int layStyel,f3dPoint norm,long hRefRod=0);
	void CreateSegmentParts(CParaBlockModel* pBlockModel,CXhChar16 sLayer,KEY4C keyFStart,KEY4C keyFEnd,
							KEY4C keyBStart,KEY4C keyBEnd,KEY4C* keyTStart=NULL,KEY4C* keyTEnd=NULL,
							BYTE cbStrokeFlag=0xff,bool rightSide=false);
public:
	CXhChar16 m_sLeftFrontLayout,m_sRightFrontLayout;
	CHashPtrList<ARRAY_LIST<CXhChar16> >hashSectionLayouts;
	void RebuildModel(CParaBlockModel* pBlockModel);	//������繹�ܵļ���ģ��
};
