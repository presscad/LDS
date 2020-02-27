#include "stdafx.h"
#include "stdio.h"
#include "math.h"
#include "Tower.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//���ݸ߶Ⱥ͵���ֲڶ�(�ؿ�)���ұ�׼��ѹ�߶ȱ仯ϵ��mu_z
//high����������ƽ��߶�(m), cLandEnv�������ֲڶ����
#ifdef __PART_DESIGN_INC_
#ifdef __LDS_CONTEXT_
#include "../PreProcessor.h"
double Find_MuZ(double high,char cLandEnv)
{
	//��ѹ�߶ȱ仯ϵ����ֲ��Codesģ����
	return CPreProcessor::ActiveLoadCode->Find_MuZ(high,cLandEnv);
}
#else	//�����ڻ�ͼģ��Լ�����ļ���ȡʱ���� wjh-2019.9.12
//Ӧ��Ϊ�����涨�еķ�ѹ�߶ȱ仯ϵ����
//��׼��ѹ�߶ȱ仯ϵ��mu_z��ժ�ԡ��͵���·�����涨��DL/T 5154-2002��
//[...][0]:�߶ȣ�[...][i](i>0):��Ӧ����ֲڶ����ķ�ѹ�߶ȱ仯ϵ��
//'A'��ָ��������ͺ���\����\������ɳĮ����;
//'B'��ָ��Ұ\���\����\�����Լ����ݱȽ�ϡ�������ͳ��н���;
//'C'��ָ���ܼ�����Ⱥ�ĳ�������
double mu_z_0KV_arr[19][5]=	//���ԡ������ṹ���ع淶��GB50009-2001
{
	  5,1.17,1.00,0.74,0.62,
	 10,1.38,1.00,0.74,0.62,
	 15,1.52,1.14,0.74,0.62,
	 20,1.63,1.25,0.84,0.62,
	 30,1.80,1.42,1.00,0.62,
	 40,1.92,1.56,1.13,0.73,
	 50,2.03,1.67,1.25,0.84,
	 60,2.12,1.77,1.35,0.93,
	 70,2.20,1.86,1.45,1.02,
	 80,2.27,1.95,1.54,1.11,
	 90,2.34,2.02,1.62,1.19,
	100,2.40,2.09,1.70,1.27,
	150,2.64,2.38,2.03,1.61,
	200,2.83,2.61,2.30,1.92,
	250,2.99,2.80,2.54,2.19,
	300,3.12,2.97,2.75,2.45,
	350,3.12,3.12,2.94,2.68,
	400,3.12,3.12,3.12,2.91,
	450,3.12,3.12,3.12,3.12,
};
double Find_MuZ(double high,char cLandEnv)
{
	cLandEnv=toupper(cLandEnv);
	if(cLandEnv<'A'||cLandEnv>'D')
	{
		logerr.Log("��ǰ����ķ���ص���ֲڶ��������,ϵͳĬ��Ϊ'B'��,��ѹ�߶ȱ仯ϵ����ȡ��������");
		return 0;					//�������·��
	}

	//if(voltage_grade==0)	//���ս����ṹ���ع淶��
	if(high<=mu_z_0KV_arr[0][0])
		return mu_z_0KV_arr[0][cLandEnv-'A'+1];
	else
	{
		for(int i=0;i<19;i++)
		{
			if(high==mu_z_0KV_arr[i+1][0])		//ǡ�ò鵽��������Ӧ���ߵ�mu_zֵ
				return mu_z_0KV_arr[i+1][cLandEnv-'A'+1];
			else if(high<mu_z_0KV_arr[i+1][0])	//��ֵ����mu_z
			{
				return mu_z_0KV_arr[i][cLandEnv-'A'+1]+(mu_z_0KV_arr[i+1][cLandEnv-'A'+1]-mu_z_0KV_arr[i][cLandEnv-'A'+1])*
					(high-mu_z_0KV_arr[i][0])/(mu_z_0KV_arr[i+1][0]-mu_z_0KV_arr[i][0]);
			}
		}
		return mu_z_0KV_arr[16][cLandEnv-'A'+1];	//�߶ȳ�����mu_z_arr[18][0] (400) ��
	}
}
#endif
//-----------------------VVV--�������ϵ����z--VVV-----------------------------
//���� w0*T1^2��ѯ�ֽṹ����������ϵ����
double QuerySteelEpsilon(double w0T1_2)
{
	if(w0T1_2<0.01)
		return 1.47;
	else if(w0T1_2<0.02)
		return 1.37+10*w0T1_2;
	else if(w0T1_2<0.04)
		return 1.45+6*w0T1_2;
	else if(w0T1_2<0.06)
		return 1.53+4*w0T1_2;
	else if(w0T1_2<0.08)
		return 1.59+3*w0T1_2;
	else if(w0T1_2<0.10)
		return 1.63+2.5*w0T1_2;
	else if(w0T1_2<0.20)
		return 1.72+1.6*w0T1_2;
	else if(w0T1_2<0.40)
		return 1.84+w0T1_2;
	else if(w0T1_2<0.60)
		return 2.0+0.6*w0T1_2;
	else if(w0T1_2<0.80)
		return 2.06+0.5*w0T1_2;
	else if(w0T1_2<1.00)
		return 2.18+0.35*w0T1_2;
	else if(w0T1_2<2.00)
		return 2.26+0.27*w0T1_2;
	else if(w0T1_2<4.00)
		return 2.51+0.145*w0T1_2;
	else if(w0T1_2<6.00)
		return 2.71+0.095*w0T1_2;
	else if(w0T1_2<8.00)
		return 2.86+0.07*w0T1_2;
	else if(w0T1_2<10.00)
		return 2.94+0.06*w0T1_2;
	else if(w0T1_2<20.00)
		return 3.17+0.037*w0T1_2;
	else if(w0T1_2<30.00)
		return 3.45+0.023*w0T1_2;
	else
		return 4.14;
}
//��ѯ����Ӱ��ϵ����
double QueryVAffectCoef(double H,char cLandscape/*='B'*/)
{
	if(H<10)
	{
		if(cLandscape=='A')
			return 0.78;
		else if(cLandscape=='B')
			return 0.72;
		else if(cLandscape=='C')
			return 0.64;
		else
			return 0.53;
	}
	else if(H<20)
	{
		if(cLandscape=='A')
			return 0.73+0.005*H;
		else if(cLandscape=='B')
			return 0.65+0.007*H;
		else if(cLandscape=='C')
			return 0.55+0.009*H;
		else
			return 0.41+0.012*H;
	}
	else if(H<30)
	{
		if(cLandscape=='A')
			return 0.77+0.003*H;
		else if(cLandscape=='B')
			return 0.71+0.004*H;
		else if(cLandscape=='C')
			return 0.63+0.005*H;
		else
			return 0.51+0.014*H;
	}
	else if(H<40)
	{
		if(cLandscape=='A')
			return 0.83+0.001*H;
		else if(cLandscape=='B')
			return 0.77+0.002*H;
		else if(cLandscape=='C')
			return 0.66+0.004*H;
		else
			return 0.57+0.005*H;
	}
	else if(H<50)
	{
		if(cLandscape=='A')
			return 0.83+0.001*H;
		else if(cLandscape=='B')
			return 0.77+0.002*H;
		else if(cLandscape=='C')
			return 0.70+0.003*H;
		else
			return 0.61+0.004*H;
	}
	else if(H<60)
	{
		if(cLandscape=='A')
			return 0.83+0.001*H;
		else if(cLandscape=='B')
			return 0.82+0.001*H;
		else if(cLandscape=='C')
			return 0.75+0.002*H;
		else
			return 0.66+0.003*H;
	}
	else if(H<70)
	{
		if(cLandscape=='A')
			return 0.89;
		else if(cLandscape=='B')
			return 0.82+0.001*H;
		else if(cLandscape=='C')
			return 0.81+0.001*H;
		else
			return 0.66+0.003*H;
	}
	else if(H<80)
	{
		if(cLandscape=='A')
			return 0.89;
		else if(cLandscape=='B')
			return 0.89;
		else if(cLandscape=='C')
			return 0.74+0.002*H;
		else
			return 0.73+0.002*H;
	}
	else if(H<90)
	{
		if(cLandscape=='A')
			return 0.89;
		else if(cLandscape=='B')
			return 0.81+0.001*H;
		else if(cLandscape=='C')
			return 0.82+0.001*H;
		else
			return 0.73+0.002*H;
	}
	else if(H<100)
	{
		if(cLandscape=='A')
			return 0.89;
		else if(cLandscape=='B')
			return 0.90;
		else if(cLandscape=='C')
			return 0.91;
		else
			return 0.82+0.001*H;
	}
	else if(H<150)
	{
		if(cLandscape=='A')
			return 0.93-0.0004*H;
		else if(cLandscape=='B')
			return 0.92-0.0002*H;
		else if(cLandscape=='C')
			return 0.87+0.0004*H;
		else
			return 0.82+0.001*H;
	}
	else if(H<200)
	{
		if(cLandscape=='A')
			return 0.96-0.0006*H;
		else if(cLandscape=='B')
			return 0.92+0.0002*H;
		else if(cLandscape=='C')
			return 0.93;
		else
			return 0.88+0.0006*H;
	}
	else if(H<250)
	{
		if(cLandscape=='A')
			return 0.92-0.0004*H;
		else if(cLandscape=='B')
			return 0.96-0.0004*H;
		else if(cLandscape=='C')
			return 0.89-0.0002*H;
		else
			return 0.96+0.0002*H;
	}
	else if(H<300)
	{
		if(cLandscape=='A')
			return 0.97-0.0006*H;
		else if(cLandscape=='B')
			return 0.96-0.0004*H;
		else if(cLandscape=='C')
			return 0.97+0.0002*H;
		else
			return 1.01;
	}
	else if(H<350)
	{
		if(cLandscape=='A')
			return 0.79;
		else if(cLandscape=='B')
			return 0.90-0.0002*H;
		else if(cLandscape=='C')
			return 0.97-0.0002*H;
		else
			return 1.01;
	}
	else if(H<400)
	{
		if(cLandscape=='A')
			return 0.79;
		else if(cLandscape=='B')
			return 0.83;
		else if(cLandscape=='C')
			return 0.97-0.0002*H;
		else
			return 1.08-0.0002*H;
	}
	else if(H<450)
	{
		if(cLandscape=='A')
			return 0.79;
		else if(cLandscape=='B')
			return 0.83;
		else if(cLandscape=='C')
			return 0.73+0.0004*H;
		else
			return 1.00;
	}
	else
	{
		if(cLandscape=='A')
			return 0.79;
		else if(cLandscape=='B')
			return 0.83;
		else if(cLandscape=='C')
			return 0.91;
		else
			return 1.00;
	}
}
double QueryReviseVCoef(double BH_to_B0)
{
	if(BH_to_B0>=1)
		return 1.0;
	else if(BH_to_B0>0.9)
		return 1.10;
	else if(BH_to_B0>0.8)
		return 1.20;
	else if(BH_to_B0>0.7)
		return 1.32;
	else if(BH_to_B0>0.6)
		return 1.50;
	else if(BH_to_B0>0.5)
		return 1.75;
	else if(BH_to_B0>0.4)
		return 2.08;
	else if(BH_to_B0>0.3)
		return 2.53;
	else if(BH_to_B0>0.2)
		return 3.30;
	else if(BH_to_B0>0.1)
		return 5.60;
	else
		return 5.60;
}
double vibration_type_coef[10][6]={
	0.1,0.02,0.02,0.01,0.01,0.01,
	0.2,0.06,0.06,0.05,0.04,0.03,
	0.3,0.14,0.12,0.11,0.09,0.07,
	0.4,0.23,0.21,0.19,0.16,0.13,
	0.5,0.34,0.32,0.29,0.26,0.21,
	0.6,0.46,0.44,0.41,0.37,0.31,
	0.7,0.59,0.57,0.55,0.51,0.45,
	0.8,0.79,0.71,0.69,0.66,0.61,
	0.9,0.86,0.86,0.85,0.83,0.80,
	1.0,1.00,1.00,1.00,1.00,1.00,
};
double QueryVibrationType1Coef(double ref_high,double BH_to_B0)
{
	for(int i=0;i<10;i++)
	{
		if(ref_high<vibration_type_coef[i][0])
		{
			if(i==0)
			{
				if(BH_to_B0<0.6)
					return 0.01;
				else if(BH_to_B0<0.8)
					return -0.02+0.05*BH_to_B0;
				else
					return 0.02;
			}
			else
			{
				double fac1,fac2;
				if(BH_to_B0<0.2)
					return vibration_type_coef[i-1][5]+10*(vibration_type_coef[i][5]-vibration_type_coef[i-1][5])*(ref_high-0.1*i);
				else if(BH_to_B0<0.4)
				{
					fac1= vibration_type_coef[i-1][5]+5*(vibration_type_coef[i-1][4]-vibration_type_coef[i-1][5])*(BH_to_B0-0.2);
					fac2= vibration_type_coef[i][5]+5*(vibration_type_coef[i][4]-vibration_type_coef[i][5])*(BH_to_B0-0.2);
					return fac1+(fac2-fac1)*(10*ref_high-i);
				}
				else if(BH_to_B0<0.6)
				{
					fac1= vibration_type_coef[i-1][4]+5*(vibration_type_coef[i-1][3]-vibration_type_coef[i-1][4])*(BH_to_B0-0.4);
					fac2= vibration_type_coef[i][4]+5*(vibration_type_coef[i][3]-vibration_type_coef[i][4])*(BH_to_B0-0.4);
					return fac1+(fac2-fac1)*(10*ref_high-i);
				}
				else if(BH_to_B0<0.8)
				{
					fac1= vibration_type_coef[i-1][3]+5*(vibration_type_coef[i-1][2]-vibration_type_coef[i-1][3])*(BH_to_B0-0.6);
					fac2= vibration_type_coef[i][3]+5*(vibration_type_coef[i][2]-vibration_type_coef[i][3])*(BH_to_B0-0.6);
					return fac1+(fac2-fac1)*(10*ref_high-i);
				}
				else if(BH_to_B0<1.0)
				{
					fac1= vibration_type_coef[i-1][2]+5*(vibration_type_coef[i-1][1]-vibration_type_coef[i-1][2])*(BH_to_B0-0.8);
					fac2= vibration_type_coef[i][2]+5*(vibration_type_coef[i][1]-vibration_type_coef[i][2])*(BH_to_B0-0.8);
					return fac1+(fac2-fac1)*(10*ref_high-i);
				}
				else
					return vibration_type_coef[i-1][1]+(vibration_type_coef[i][1]-vibration_type_coef[i-1][1])*(10*ref_high-i);
			}
		}
	}
	return 1.0;
}
/*  ���ݡ������ṹ���ع淶��GB 50009��2001��¼E��E.1.1���ʽṹһ������½��Ƶ�
	������������T1=(0.007��0.013)H�����иֽṹ��ȡ��ֵ���ֽ�������ṹ��ȡ��ֵ��
*/
double PERIOD_COEF=0.013;
double CalBetaZInSpecHigh(double W0,double total_high,double high_pos,double BH_to_B0,char cLandscape/*='B'*/)
{
	double T1=PERIOD_COEF*total_high;	//������������������
	double epsilon=QuerySteelEpsilon(W0*T1*T1);
	double V=QueryVAffectCoef(total_high,cLandscape);
	double v_r=QueryReviseVCoef(BH_to_B0);
	double fai_z=QueryVibrationType1Coef(high_pos/total_high,BH_to_B0);
	double mu_z=Find_MuZ(high_pos,cLandscape);
	return 1.0+epsilon*V*v_r*fai_z/mu_z;
}
//-----------------------VVV--����ص���ϵ�����ѯ--VVV-----------------------------
//����ݡ������ṹ���ع淶��GB50009-2001���Ƹ���60m�����
//��׼beta_zΪ����ص���ϵ����ժ��DL/T5092-1999���ܿ��͵���·��Ƽ�����̡���
//[...][0]:���ߣ�[...][1]:����ص���ϵ��(�������߸���);[...][2]:����ص���ϵ��(��������)
double beta_z_arr[5][3]={	20,1.0,1.0 ,
							30,1.4,1.25,
							40,1.6,1.35,
							50,1.7,1.5 ,
							60,1.8,1.6  };
//�������߲��Ҹ�������ص���ϵ��beta_z(������ϵ��)
double Find_BetaZ(double ta_high,int guy_0_other1/*=1*/)
{
	if(guy_0_other1!=0&&guy_0_other1!=1)
		return 1.0;
	if(ta_high<=beta_z_arr[0][0])
		return beta_z_arr[0][guy_0_other1+1];
	else
	{
		for(int i=0;i<4;i++)
		{
			if(ta_high==beta_z_arr[i+1][0])		//ǡ�ò鵽��������Ӧ���ߵ�beta_zֵ
				return beta_z_arr[i+1][guy_0_other1+1];
			else if(ta_high<beta_z_arr[i+1][0])	//��ֵ����beta_z
			{
				return beta_z_arr[i][guy_0_other1+1]+(beta_z_arr[i+1][guy_0_other1+1]-beta_z_arr[i][guy_0_other1+1])*
					(ta_high-beta_z_arr[i][0])/(beta_z_arr[i+1][0]-beta_z_arr[i][0]);
			}
		}
		return beta_z_arr[4][guy_0_other1+1];	//���߳�����beta_z_arr[5][0] (60) ��
	}
}
//-----------------------VVV--����������ϵ�����ѯ--VVV-------------------------
//����������ϵ����yita_arr��yita_arr[i][j]:fai=(i+1)/10,b/h=j*2+2(j==0,b/h=1)
//��ժ�ԡ������ṹ���ع淶��GB50009-2001 &���͵���·�����涨��DL/T 5154-2002��
//					b/h<=1 b/h=2 b/h=4 b/h=6 
double yita_arr[6][4]={	1.00,1.00,1.00,1.00,	//�ա�0.1
						0.85,0.90,0.93,0.97,	//��= 0.2
						0.66,0.75,0.80,0.85,	//��= 0.3
						0.50,0.60,0.67,0.73,	//��= 0.4
						0.33,0.45,0.53,0.62,	//��= 0.5
						0.15,0.30,0.40,0.50,};	//��= 0.6
//����������ϵ����yita_arr��yita_arr[i][j]:fai=(i+1)/10,b/h=j*2+2(j==0,b/h=1)
double Find_YiTa(double fai,double b_to_h)
{
	double fai_coef,bh_coef,yita1,yita2;
	if(fai<=0.1)
		return 1.0;
	for(int i=0;i<6;i++)
	{
		if(fai<=(i+1)/10.0)
			fai_coef = 10*(fai-i/10.0);
		else if(i==5)	//fai>0.6
			fai_coef=1.0;
		else
			continue;
		if(b_to_h<=1.0)
		{
			if(i==0)
				return yita_arr[i][0];
			else
				return yita_arr[i-1][0]+(yita_arr[i][0]-yita_arr[i-1][0])*fai_coef;
		}
		else if(b_to_h<=2.0)
		{
			bh_coef = b_to_h-1.0;
			if(i==0)
			{
				yita1 = yita_arr[i][0]+(yita_arr[i][1]-yita_arr[i][0])*bh_coef;
				yita2 = yita1;//yita_arr[i+1][0]+(yita_arr[i+1][1]-yita_arr[i+1][0])*bh_coef;
			}
			else
			{
				yita1 = yita_arr[i-1][0]+(yita_arr[i-1][1]-yita_arr[i-1][0])*bh_coef;
				yita2 = yita_arr[i][0]+(yita_arr[i][1]-yita_arr[i][0])*bh_coef;
			}
			return yita1+(yita2-yita1)*fai_coef;
		}
		else if(b_to_h<=4.0)
		{
			bh_coef = (b_to_h-2.0)/2.0;
			if(i==0)
			{
				yita1 = yita_arr[i][1]+(yita_arr[i][2]-yita_arr[i][1])*bh_coef;
				return yita1;
			}
			else
			{
				yita1 = yita_arr[i-1][1]+(yita_arr[i-1][2]-yita_arr[i-1][1])*bh_coef;
				yita2 = yita_arr[i][1]+(yita_arr[i][2]-yita_arr[i][1])*bh_coef;
				return yita1+(yita2-yita1)*fai_coef;
			}
		}
		else if(b_to_h<=6.0)
		{
			bh_coef = (b_to_h-4.0)/2.0;
			if(i==0)
			{
				return yita1 = yita_arr[i][2]+(yita_arr[i][3]-yita_arr[i][2])*bh_coef;
			}
			else
			{
				yita1 = yita_arr[i-1][2]+(yita_arr[i-1][3]-yita_arr[i-1][2])*bh_coef;
				yita2 = yita_arr[i][2]+(yita_arr[i][3]-yita_arr[i][2])*bh_coef;
				return yita1+(yita2-yita1)*fai_coef;
			}
		}
		else// if(b_to_h>6.0)
		{
			if(i==0)
				return yita_arr[i][3];
			else
				return yita_arr[i-1][3]+(yita_arr[i][3]-yita_arr[i-1][3])*fai_coef;
		}
	}
	return 0;
}
//-----------------------VVV--�����������ʱ�����������ѯ--VVV-------------------------
//�Ǹ������������ʱ��������mu_s��ժ�ԡ������ṹ���ع淶��GB50009-2001��
//[i][...]:fai=(i+1)/10; 
//[...][j]:j==0�������1,j==1(2)�������2����˫���Ǹ�,j==3���������η���
double mu_s_arr[5][4]={	2.6,2.9,3.1,2.4,	//�ա�0.1
						2.4,2.7,2.9,2.2,	//��= 0.2
						2.2,2.4,2.7,2.0,	//��= 0.3
						2.0,2.2,2.4,1.8,	//��= 0.4
						1.9,1.9,2.0,1.6,};	//��= 0.5;
//�������ܵ���ϵ���ͷ���������ܵ��������ʱ������ϵ��mu_s
//wind_type==0�������1(����),wind_type==1�������2(б��)����˫���Ǹ�,wind_type==3�������η���
double Find_MuS(double fai,int wind_type)
{
	if(wind_type<0||wind_type>3)
		return 0;
	if(fai<=0.1)
		return mu_s_arr[0][wind_type];
	else
	{
		for(int i=0;i<4;i++)
		{
			if(fai==i/10.0+0.1)		//ǡ�ò鵽��������Ӧ����ϵ��faiֵ
				return mu_s_arr[i+1][wind_type];
			else if(fai<(i+1)/10.0)	//��ֵ����mu_s
			{
				return mu_s_arr[i][wind_type]+(mu_s_arr[i+1][wind_type]-mu_s_arr[i][wind_type])*
					(wind_type-i/10.0+0.1)*10;
			}
		}
		return mu_s_arr[4][wind_type];	//����ϵ��������0.5
	}
}
#endif