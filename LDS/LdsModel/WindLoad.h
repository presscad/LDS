#ifndef __WIND_LOAD_FUNC_H_
#define __WIND_LOAD_FUNC_H_
//���ݸ߶Ⱥ͵���ֲڶ�(�ؿ�)���ұ�׼��ѹ�߶ȱ仯ϵ��mu_z
//high����������ƽ��߶�(m), cLandEnv�������ֲڶ����
double Find_MuZ(double high,char cLandEnv);
//����������ϵ����yita_arr��yita_arr[i][j]:fai=(i+1)/10,b/h=j*2+2(j==0,b/h=1)
double Find_YiTa(double fai,double b_to_h);
//�������ܵ���ϵ���ͷ���������ܵ��������ʱ������ϵ��mu_s
//wind_type==0�������1(����),wind_type==1�������2(б��)����˫���Ǹ�,wind_type==3�������η���
double Find_MuS(double fai,int wind_type);
//���ݷ�λ�Ǽ���ѹ���㷽ʽ����X���Y����ص�X/Y��������ط���ϵ��
void GetWindLoadAllotCoef(double *Xcoef_inWx,double *Xcoef_inWy,double *Ycoef_inWx,double *Ycoef_inWy,
						  long azimuth,long iStyle);
extern double PERIOD_COEF;/*=0.013;*/
//��ѯ��������ϵ����
double QuerySteelEpsilon(double w0T1_2);
//��ѯ����Ӱ��ϵ����
double QueryVAffectCoef(double H,char cLandscape='B');
double QueryReviseVCoef(double BH_to_B0);
double QueryVibrationType1Coef(double ref_high,double BH_to_B0);
//����ָ������(m)����������ط���ϵ��beta_z
double CalBetaZInSpecHigh(double W0,double total_high,double high_pos,double BH_to_B0,char cLandscape='B');
//��������(m)���Ҹ�������ص���ϵ��beta_z��������ϵ����
double Find_BetaZ(double ta_high,int guy_0_other1=1);
#endif