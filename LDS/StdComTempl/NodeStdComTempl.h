#pragma once
//���ߣ�02 //ʵ�壺03
//���ӵ�һ�����飺
//1.�ᵣ���ӵ�
//2.����K��
//3.�������ӵ�
//4.����K��
//5.����ڵ�
//6.�������ӵ�
//7.����V�����ӵ�
//8.���������ӵ�
//���ӵ��������(����λ):����ĸASCII���16����+Index
//B  42//D 44//F 46//G 47//H  48//J 4A//T 54//Z 5A

//����
//�ᵣ�ڵ�  CORSS_ARM
static const DWORD STDTEMPL_CONNPOINT_HD1			= 0x02010001;//ˮƽ�ᵣ�����֧���ڲ����ӵ㣨��׼ˮƽ�ķǶϣ�
static const DWORD STDTEMPL_CONNPOINT_HD1A			= 0x02010011;//ˮƽ�ᵣ�����֧���ڲ����ӵ㣨��׼ˮƽ�ĶϿ���
static const DWORD STDTEMPL_CONNPOINT_HD2			= 0x02010002;//ˮƽ�ᵣ�����֧��������ӵ�(��׼ˮƽ��)
static const DWORD STDTEMPL_CONNPOINT_HD3			= 0x02010003;//�Ʊ��������������ᵣ�����ӵ�(��׼ˮƽ��)
static const DWORD STDTEMPL_CONNPOINT_HD3_G5		= 0x02010004;//�Ʊ��������������ᵣ�����ӵ�(��׼ˮƽ�˼��Ƕ�)
static const DWORD STDTEMPL_CONNPOINT_HD4			= 0x02010005;//èͷ�������ڹյ㴦
static const DWORD STDTEMPL_CONNPOINT_HD5			= 0x02010006;//èͷ��������յ㴦(��)
static const DWORD STDTEMPL_CONNPOINT_HD6			= 0x02010007;//èͷ��������յ㴦(��)
static const DWORD STDTEMPL_CONNPOINT_HD7			= 0x02010008;//������ˮƽ�ᵣ�²����ӵ�
static const DWORD STDTEMPL_CONNPOINT_HD8			= 0x02010009;//������ˮƽ�ᵣ�ϲ����ӵ�
static const DWORD STDTEMPL_CONNPOINT_HD9			= 0x0201000a;//��ͷ��ˮƽ�ᵣ���ӵ�
static const DWORD STDTEMPL_CONNPOINT_HD10			= 0x0201000b;//�ᵣ����(hd_10)
static const DWORD STDTEMPL_CONNPOINT_XH_HD3A		= 0x0201000c;//���ۺᵣ�����ӵ�4~7��
static const DWORD STDTEMPL_CONNPOINT_XH_HD12		= 0x0201000d;//���ۺᵣ�����ӵ�6��
static const DWORD STDTEMPL_CONNPOINT_XH_HD12_X		= 0x0201000e;//���ۺᵣ�����ӵ�6��
static const DWORD STDTEMPL_CONNPOINT_XH_HD13		= 0x0201000f;//����֧�ܺᵣ�����ӵ�
static const DWORD STDTEMPL_CONNPOINT_XH_HD14		= 0x02010010;//����֧�ܺᵣ�����ӵ�
//static const DWORD STDTEMPL_CONNPOINT_HD1A		= 0x02010011;//ˮƽ�ᵣ�����֧���ڲ����ӵ㣨��׼ˮƽ�ĶϿ���
//����
static const DWORD STDTEMPL_CONNPOINT_XH_DM1		=0x02090001;//���ߵ������ӵ�
static const DWORD STDTEMPL_CONNPOINT_XH_DM2		=0x02090002;//���ߵ������ӵ�
static const DWORD STDTEMPL_CONNPOINT_XH_DM3		=0x02090003;//���ߵ������ӵ�
//����K�ڵ�  CRANK ARM		  
static const DWORD STDTEMPL_CONNPOINT_ZK14_K1		=0x02020001; //��������Ǹ�(����Ǹ�)����
static const DWORD STDTEMPL_CONNPOINT_ZK14_K2		=0x02020002; //�������۲��ضϲ��øְ������滥��
static const DWORD STDTEMPL_CONNPOINT_ZK14_K3		=0x02020003;//�Ʊ�èͷ��������K�ڵ㴦����
static const DWORD STDTEMPL_CONNPOINT_ZK14_K1_SIDE	=0x02020004;//�Ʊ���������K�ڵ���棨���棩
static const DWORD STDTEMPL_CONNPOINT_ZK14_K3_SIDE	=0x02020005;//�Ʊ���������K�ڵ���棨˫����£�
//���½ڵ�  SLOPE
static const DWORD STDTEMPL_CONNPOINT_BP1			=0x02030001;//���Ǹ�������´����ӵ�
static const DWORD STDTEMPL_CONNPOINT_BP2			=0x02030002;//���Ǹ�èͷ�Ʊ���ͷ���´����ӵ�
static const DWORD STDTEMPL_CONNPOINT_BP3			=0x02030003;//��ϽǸ�������´����ӵ�
static const DWORD STDTEMPL_CONNPOINT_BP4			=0x02030004;//��ϽǸ�èͷ�Ʊ���ͷ���´����ӵ�
//��������K�ڵ� TOWER_BODY
static const DWORD STDTEMPL_CONNPOINT_SZK			=0x02040001;//zk 1,2,3,4,11,12,13,21,22,23,24;//
static const DWORD STDTEMPL_CONNPOINT_ZK_K			=0x02040002;//zk 1,2,3,4,11,12,13,21,22,23,24;//
static const DWORD STDTEMPL_CONNPOINT_HK1			=0x02040003;//HK1 ����K��
static const DWORD STDTEMPL_CONNPOINT_HK2			=0x02040004;//HK2 ����K��
//����ڵ�  CROSS
static const DWORD STDTEMPL_CONNPOINT_JC1			=0x02050001;//���Ǹ����������֧�ż�
static const DWORD STDTEMPL_CONNPOINT_JC1_K1		=0x02050002;//���Ǹ����������֧�ż�
static const DWORD STDTEMPL_CONNPOINT_JC1_K2		=0x02050003;//���Ǹ����������֧�ż�
static const DWORD STDTEMPL_CONNPOINT_JC2			=0x02050004;//��ϽǸ����������֧�ż�
static const DWORD STDTEMPL_CONNPOINT_JC3			=0x02050005;//���������֧�ż�
//���߽ڵ�
static const DWORD STDTEMPL_CONNPOINT_XH_GX1		=0x02060001;//���ߴ����ƽڵ�(ID:12)
static const DWORD STDTEMPL_CONNPOINT_XH_GX2		=0x02060002;//���ߴ����ƽڵ�(ID:13)
static const DWORD STDTEMPL_CONNPOINT_XH_GX3		=0x02060007;//���ߴ����ƽڵ�
static const DWORD STDTEMPL_CONNPOINT_XH_GX4		=0x02060008;//���ߴ����ƽڵ�
static const DWORD STDTEMPL_CONNPOINT_XH_GX5		=0x02060009;//���ߴ����ƽڵ�
static const DWORD STDTEMPL_CONNPOINT_XH_DX1		=0x02060003;//����֮�ܶ˽ڵ�(ID:14)
static const DWORD STDTEMPL_CONNPOINT_XH_DX2		=0x02060004;//����֮�ܶ˽ڵ�(ID:14)
static const DWORD STDTEMPL_CONNPOINT_XH_DX3		=0x02060005;//����֮���ڲ��Ҹ������ཻ�ڵ�(ID:14)
static const DWORD STDTEMPL_CONNPOINT_XH_DX3A		=0x02060006;//����֮���ڲ��Ҹ�(���߽ض�)�����ཻ�ڵ�
//static const DWORD STDTEMPL_CONNPOINT_XH_GX3		=0x02060007;//���ߴ����ƽڵ�
//static const DWORD STDTEMPL_CONNPOINT_XH_GX4		=0x02060008;//���ߴ����ƽڵ�
//static const DWORD STDTEMPL_CONNPOINT_XH_GX5		=0x02060009;//���ߴ����ƽڵ�
//����V�ڵ�
static const DWORD STDTEMPL_CONNPOINT_TV			=0x02070001;//TV
//�����Ľڵ�   AUXILIARY
static const DWORD STDTEMPL_CONNPOINT_FK1			=0x02080001;//˫�˼����������ӽڵ�
static const DWORD STDTEMPL_CONNPOINT_FK2			=0x02080002;//���˼����������ӽڵ�
static const DWORD STDTEMPL_CONNPOINT_FK3			=0x02080003;//�ĸ˼����������ӽڵ�
static const DWORD STDTEMPL_CONNPOINT_FK4			=0x02080004;//���ȸ��������ӽڵ�

//S  ʮ����//T T�ͽǸ�//K ����//N  ֫����//NW ֫��һ��һ��//G �˼�����//B ��˨����
//ʵ���
//�ᵣ�ڵ�
static const DWORD STDTEMPL_NODEPLATE_HD1			=0x03014801;//ˮƽ�ᵣ�����֧���ڲ����ӵ�(˫���ˮƽ��˲��Ͽ�)
static const DWORD STDTEMPL_NODEPLATE_HD1A			=0x03014818;//ˮƽ�ᵣ�����֧���ڲ����ӵ�(ˮƽ��˶Ͽ�)
static const DWORD STDTEMPL_NODEPLATE_HD2			=0x03014802;//ˮƽ�ᵣ�����֧��������ӵ�(˫���)
static const DWORD STDTEMPL_NODEPLATE_HD2_2			=0x03014803;//ˮƽ�ᵣ�����֧��������ӵ�(�����)
static const DWORD STDTEMPL_NODEPLATE_HD3			=0x03014804;//�Ʊ��������������ᵣ�����ӵ�(˫���)
static const DWORD STDTEMPL_NODEPLATE_HD3_2			=0x03014805;//�Ʊ��������������ᵣ�����ӵ�(�����)
static const DWORD STDTEMPL_NODEPLATE_HD3_G5		=0x03014806;//�Ʊ���������ᵣ�����ӵ�ˮƽ�˼��Ƕ�(˫���)
static const DWORD STDTEMPL_NODEPLATE_HD4			=0x03014807;//èͷ�������ڹյ㴦
static const DWORD STDTEMPL_NODEPLATE_HD4_2			=0x03014817;//èͷ�������ڹյ㴦
static const DWORD STDTEMPL_NODEPLATE_HD5			=0x03014808;//èͷ��������յ㴦(��)
static const DWORD STDTEMPL_NODEPLATE_HD6			=0x03014809;//èͷ��������յ㴦(��)
static const DWORD STDTEMPL_NODEPLATE_HD7			=0x0301480a;//������ˮƽ�ᵣ�²����ӵ�
static const DWORD STDTEMPL_NODEPLATE_HD8			=0x0301480b;//������ˮƽ�ᵣ�ϲ����ӵ�
static const DWORD STDTEMPL_NODEPLATE_HD9			=0x0301480c;//��ͷ��ˮƽ�ᵣ���ӵ�
static const DWORD STDTEMPL_NODEPLATE_HD10			=0x0301480d;//�ᵣ����(hd_10)
static const DWORD STDTEMPL_NODEPLATE_XH_HD3A		=0x0301480e;//���ۺᵣ�����ӵ�4��
static const DWORD STDTEMPL_NODEPLATE_XH_HD12		=0x0301480f;//���ۺᵣ�����ӵ�6��
static const DWORD STDTEMPL_NODEPLATE_XH_HD12_X		=0x03014810;//���ۺᵣ�����ӵ�6��
static const DWORD STDTEMPL_NODEPLATE_XH_HD13		=0x03014811;//����֧�ܺᵣ�����ӵ�
static const DWORD STDTEMPL_NODEPLATE_XH_HD14		=0x03014812;//����֧�ܺᵣ�����ӵ�
static const DWORD STDTEMPL_NODEPLATE_HD8_YS		=0x03014813;//�ᵣ����(hd_7)
static const DWORD STDTEMPL_NODEPLATE_HD7_T			=0x03014814;//�ᵣ��ͷ(T�Ǹ�)
static const DWORD STDTEMPL_NODEPLATE_HD9_T			=0x03014815;//�ᵣ����(T�Ǹ�)
static const DWORD STDTEMPL_NODEPLATE_HD10_T		=0x03014816;//�ᵣ����(T�Ǹ�)
//static const DWORD STDTEMPL_NODEPLATE_HD4_2		=0x03014817;//èͷ�������ڹյ㴦
//static const DWORD STDTEMPL_NODEPLATE_HD1A		=0x03014818;//ˮƽ�ᵣ�����֧���ڲ����ӵ�(ˮƽ��˶Ͽ�)
//����
static const DWORD STDTEMPL_NODEPLATE_XH_DM1		=0x03090001;//���ߵ������ӵ�
static const DWORD STDTEMPL_NODEPLATE_XH_DM1A		=0x03090004;//���ߵ������ӵ�
static const DWORD STDTEMPL_NODEPLATE_XH_DM2		=0x03090002;//���߸˼��������ӵ�
static const DWORD STDTEMPL_NODEPLATE_XH_DM3		=0x03090003;//���߸˼��������ӵ�
//����K�ڵ�  CRANK ARM		  
static const DWORD STDTEMPL_NODEPLATE_ZK14_K1		=0x03025A01; //��������Ǹ�(����Ǹ�)����
static const DWORD STDTEMPL_NODEPLATE_ZK14_K2		=0x03025A02; //�������۲��ضϲ��øְ������滥��
static const DWORD STDTEMPL_NODEPLATE_ZK14_K3		=0x03025A03;//�Ʊ�èͷ��������K�ڵ㴦����
static const DWORD STDTEMPL_NODEPLATE_ZK14_K1_SIDE	=0x03025A04;//�Ʊ���������K�ڵ���棨���棩
static const DWORD STDTEMPL_NODEPLATE_ZK14_K3_SIDE	=0x03025A05;//�Ʊ���������K�ڵ���棨˫����£�
//���½ڵ�
static const DWORD STDTEMPL_NODEPLATE_BP1			=0x03034201;//���Ǹ�������´����ӵ�
static const DWORD STDTEMPL_NODEPLATE_BP2			=0x03034202;//���Ǹ�èͷ�Ʊ���ͷ���´����ӵ�
static const DWORD STDTEMPL_NODEPLATE_BP3			=0x03034203;//��ϽǸ�������´����ӵ�
static const DWORD STDTEMPL_NODEPLATE_BP4			=0x03034203;//��ϽǸ�èͷ�Ʊ���ͷ���´����ӵ�
//��������K�ڵ�
//����  Z  5A
static const DWORD STDTEMPL_NODEPLATE_SZK1			=0x03045A01;//��б�ľ�Ϊ���Ǹ�
static const DWORD STDTEMPL_NODEPLATE_SZK2			=0x03045A02;//��б�ľ�Ϊ���Ǹ�
static const DWORD STDTEMPL_NODEPLATE_SZK5			=0x03045A03;//��б�ľ�Ϊ���Ǹ�(ID:11)	
static const DWORD STDTEMPL_NODEPLATE_SZK6			=0x03045A04;//��б�ľ�Ϊ���Ǹ�
static const DWORD STDTEMPL_NODEPLATE_ZK1_N1		=0x03045A05;//��б�ľ�Ϊ���Ǹ�
static const DWORD STDTEMPL_NODEPLATE_ZK1_N2		=0x03045A06;//��б�ľ�Ϊ���Ǹ�
static const DWORD STDTEMPL_NODEPLATE_ZK1_N3		=0x03045A07;//��б�ľ�Ϊ���Ǹ�
static const DWORD STDTEMPL_NODEPLATE_ZK2_S1		=0x03045A08;//������ϽǸ֣�б�ĵ��Ǹ�
static const DWORD STDTEMPL_NODEPLATE_ZK2_S2		=0x03045A09;//������ϽǸ֣�б�ĵ��Ǹ�
static const DWORD STDTEMPL_NODEPLATE_ZK2_S3		=0x03045A0A;//������ϽǸ֣�б�ĵ��Ǹ�
static const DWORD STDTEMPL_NODEPLATE_ZK2_T			=0x03045A0B;//������ϽǸ֣�б�ĵ��Ǹ�
static const DWORD STDTEMPL_NODEPLATE_ZK3_S			=0x03045A0C;//��б�ľ�Ϊ��ϽǸ�
static const DWORD STDTEMPL_NODEPLATE_ZK3_T			=0x03045A0D;//��б�ľ�Ϊ��ϽǸ�
static const DWORD STDTEMPL_NODEPLATE_ZK4			=0x03045A0E;//��б�ľ�Ϊ�ֹ�
//����  H  48
static const DWORD STDTEMPL_NODEPLATE_HK1			=0x03044801;//����ˮƽ��İ���֧�Ű�(�����ߣ�
static const DWORD STDTEMPL_NODEPLATE_HK2			=0x03044802;//����ˮƽ������ֿ۳Ű壨�����ߣ�
static const DWORD STDTEMPL_NODEPLATE_HK3			=0x03044803;//�����ǹ������ȸ��棬T����ϽǸ֡�hk3
//����ڵ�
static const DWORD STDTEMPL_NODEPLATE_JC1_K1		=0x03054A01;//������㣺���Ǹ֣���֧�ż�
static const DWORD STDTEMPL_NODEPLATE_JC1_K2		=0x03054A02;//������㣺���Ǹ֣���֧�ż�
static const DWORD STDTEMPL_NODEPLATE_JC2			=0x03054A03;//������㣺���Ǹ֣���֧�ż�	jc 06,07
static const DWORD STDTEMPL_NODEPLATE_JC3			=0x03054A04;//������㣺T����ϽǸ�		jc 08
//����V�ڵ�
static const DWORD STDTEMPL_NODEPLATE_TV1_G2		=0x03075401;//˫�˼����Ǹ�
static const DWORD STDTEMPL_NODEPLATE_TV1_G3		=0x03075402;//���˼����Ǹ�
static const DWORD STDTEMPL_NODEPLATE_TV1_G4		=0x03075403;//�ĸ˼����Ǹ�
static const DWORD STDTEMPL_NODEPLATE_TV2_B1		=0x03075404;//����˨T�ͽǸ�
static const DWORD STDTEMPL_NODEPLATE_TV2_B2		=0x03075405;//˫��˨T�ͽǸ�
static const DWORD STDTEMPL_NODEPLATE_TV2_B3		=0x03075406;//����˨T�ͽǸ�
//���߽ڵ�
//����  G  47
static const DWORD STDTEMPL_NODEPLATE_XH_GX1		=0x03064701;//���ߴ����ƽڵ�(ID:12)
static const DWORD STDTEMPL_NODEPLATE_XH_GX2		=0x03064702;//���ߴ����ƽڵ�(ID:13)
static const DWORD STDTEMPL_NODEPLATE_XH_GX3		=0x03064703;//���ߴ����ƽڵ�
static const DWORD STDTEMPL_NODEPLATE_XH_GX4		=0x03064704;//���ߴ����ƽڵ�
static const DWORD STDTEMPL_NODEPLATE_XH_GX5		=0x03064705;//���ߴ����ƽڵ�
//����  D  44
static const DWORD STDTEMPL_NODEPLATE_XH_DX1		=0x03064401;//���߼ܶ��Ͻڵ�(ID:14)
static const DWORD STDTEMPL_NODEPLATE_XH_DX1A		=0x03064402;//���߼ܶ��Ͻڵ�(ID:14)
static const DWORD STDTEMPL_NODEPLATE_XH_DX2		=0x03064403;//���߼ܶ��½ڵ�(ID:14)
static const DWORD STDTEMPL_NODEPLATE_XH_DX3		=0x03064404;//���߼ܶ��Ͻڵ�(ID:14)
static const DWORD STDTEMPL_NODEPLATE_XH_DX3A		=0x03064405;//���߼ܶ��Ͻڵ�(ID:14)
//�����Ľڵ�  
static const DWORD STDTEMPL_NODEPLATE_FK1			=0x03084601;//˫�˼����������ӽڵ�
static const DWORD STDTEMPL_NODEPLATE_FK2			=0x03084602;//���˼����������ӽڵ�
static const DWORD STDTEMPL_NODEPLATE_FK3			=0x03084603;//�ĸ˼����������ӽڵ�
static const DWORD STDTEMPL_NODEPLATE_FK4			=0x03084604;//���ȸ��������ӽڵ�






