#pragma once
#include "lds_part.h"
#ifdef __COMPLEX_PART_INC_
struct TUBE_WORKPLANE{
	CLDSLineTube *m_pMainTube;
	CLDSLineTube *m_pViceTube;
	CHashList<CLDSLineTube*> m_hashTubePtrList;
	f3dPoint m_xWorkNormPlane;
	f3dPoint m_xViceTubeVec;
	double m_fViceVecCosa;		//�����ֹ����߷��������ܷ�����
	TUBE_WORKPLANE()
	{
		m_pMainTube=NULL;
		m_pViceTube=NULL;
		m_fViceVecCosa=0;
	}
};

//���������ڵ��麸��ͼ
class CSpherePartGroup
{
	CLDSPartGroup* m_pPartGroup;	//��ڵ��麸��
	//ԭ��Ϊ��ڵ�λ��
	//Y��Ϊ��ת���᷽��(��׼�ֹ����������ڵ������)
	//Z��Ϊ�ɸ���֧�ܷ���vice_axis_vec��Y����ȷ��(��֤����֧�������ϲ�)
	//X����Y���Z�Ṳͬ���ȷ��
	GECS dcs;
	CParaCS::PARAMETRIC_INFO datumcs_parainfo;
	CLDSLineTube* m_pDatumTube;	//��ת�����׼�ֹ�
	CLDSLineTube* m_pViceTube;	//�������׼�ֹ�����ϵĶ�λ�渨���ֹ�,��Ƹ���
	CLDSSphere* m_pSphere;
public:
	CSpherePartGroup(CLDSPartGroup* pPartGroup);
	virtual ~CSpherePartGroup(void);
	BOOL AddSpherePartGroupViews(CLDSSphere *pSphere,PARTSET &partset);
private:
	//����ָ��������ת�����������˼���ʼ���������б�,������������
	TUBE_WORKPLANE *InitWorkPlaneList(CLDSLineTube *pMainTube,CLDSLineTube *pViceTube,
									 PARTSET &partset,ATOM_LIST<TUBE_WORKPLANE> &workPlaneList);
	//���ݹ����������ͼ
	CSectionView *AddSectionViewByWorkPlane(TUBE_WORKPLANE *pWorkPlane,PARTSET &partSet,CXhChar16 sViewName);
	//��Ӹֹܶ�������ͼ
	CSectionView *AddTubeEndSectionview(CSectionView *pMainView,CLDSLineTube *pLineTube,CXhChar16 sViewName);
};
#endif