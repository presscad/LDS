#ifndef __WIND_LOAD_FUNC_H_
#define __WIND_LOAD_FUNC_H_
//根据高度和地面粗糙度(地况)查找标准风压高度变化系数mu_z
//high代表离地面或海平面高度(m), cLandEnv代表地面粗糙度类别
double Find_MuZ(double high,char cLandEnv);
//多榀桁架体型系数表yita_arr，yita_arr[i][j]:fai=(i+1)/10,b/h=j*2+2(j==0,b/h=1)
double Find_YiTa(double fai,double b_to_h);
//根据塔架挡风系数和风向查找塔架的整体计算时的体型系数mu_s
//wind_type==0代表风向1(正方),wind_type==1代表风向2(斜方)单（双）角钢,wind_type==3表三角形风向
double Find_MuS(double fai,int wind_type);
//根据方位角及风压计算方式计算X向和Y向荷载的X/Y两方向荷载分配系数
void GetWindLoadAllotCoef(double *Xcoef_inWx,double *Xcoef_inWy,double *Ycoef_inWx,double *Ycoef_inWy,
						  long azimuth,long iStyle);
extern double PERIOD_COEF;/*=0.013;*/
//查询脉动增大系数ξ
double QuerySteelEpsilon(double w0T1_2);
//查询脉动影响系数ν
double QueryVAffectCoef(double H,char cLandscape='B');
double QueryReviseVCoef(double BH_to_B0);
double QueryVibrationType1Coef(double ref_high,double BH_to_B0);
//计算指定塔高(m)处杆塔风荷载风振系数beta_z
double CalBetaZInSpecHigh(double W0,double total_high,double high_pos,double BH_to_B0,char cLandscape='B');
//根据塔高(m)查找杆塔风荷载调整系数beta_z（即风振系数）
double Find_BetaZ(double ta_high,int guy_0_other1=1);
#endif