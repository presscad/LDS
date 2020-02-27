#ifndef __DATA_BASE_H_
#define __DATA_BASE_H_
#include "PartLib.h"
//�Ǹֽ�ͷ��ز���������,��ʱ���ڴ˴�,�Ժ�����PartLib.dll�� wht 11-01-25
/*
typedef struct tagJG_JOINT_PARA
{
	double fBaseJgWidth;		//��׼�Ǹֿ��
	double fBaseJgThick;		//��׼�Ǹֺ��
	double fInnerJgWidth;		//�ڰ��Ǹֿ��
	double fInnerJgThick;		//�ڰ��Ǹֺ��
	double fOuterPlateWidth;	//����ְ���
	double fOuterPlateThick;	//����ְ���
	double fOuterJgWidth;		//����Ǹֿ��
	double fOuterJgThick;		//����Ǹֺ��
}JG_JOINT_PARA;
extern int g_nJgJointRecordNum;
extern JG_JOINT_PARA jg_joint_table[];	//�Ǹֽ�ͷ��Ӧ��
extern JG_JOINT_PARA *FindJgJointPara(double main_w,double main_t);
extern BOOL FindInnerAngleGuiGe(double main_w,double main_t,double &inner_w,double &inner_t);
extern BOOL FindOuterAngleGuiGe(double main_w,double main_t,double &outer_w,double &outer_t);
extern BOOL FindOuterPlateGuiGe(double main_w,double main_t,double &plate_w,double &plate_t);
*/
//
extern double Q235A_STABLE_COEF[251];
extern double Q235B_STABLE_COEF[251];
extern double Q345A_STABLE_COEF[251];
extern double Q345B_STABLE_COEF[251];
extern double Q390A_STABLE_COEF[251];
extern double Q390B_STABLE_COEF[251];
extern double Q420A_STABLE_COEF[251];
extern double Q420B_STABLE_COEF[251];
extern double Q460A_STABLE_COEF[251];
extern double Q460B_STABLE_COEF[251];
BOOL JgZhunJuSerialize(CString sFileName, BOOL bWrite,BOOL bThrowError=TRUE);
void JgGuiGeSerialize(CString sFileName, BOOL bWrite);
void TubeGuiGeSerialize(CString sFileName, BOOL bWrite);
void SlotGuiGeSerialize(CString sFileName, BOOL bWrite);
void FlatGuiGeSerialize(CString sFileName, BOOL bWrite);
void JgJointParaSerialize(CString sFileName, BOOL bWrite);
void AnchorBoltParaSerialize(CString sFileName, BOOL bWrite);
void LsGuiGeSerialize(CLsFamily* pLsFamily,CString sFileName, BOOL bWrite);
void BoltFamilySerialize(CLsFamily* pLsFamily,CString sFileName, BOOL bWrite);
void LsSubPartSerialize(CLsSubPartFamily* pFamily,CString sFileName, BOOL bWrite);
void LsSpaceSerialize(CString sFileName, BOOL bWrite);
void CInsertPlateSerialize(CString sFileName, BOOL bWrite);
void UInsertPlateSerialize(CString sFileName, BOOL bWrite);
void XInsertPlateSerialize(CString sFileName, BOOL bWrite);
void FlPSerialize(CString sFileName, BOOL bWrite);
void FlDSerialize(CString sFileName, BOOL bWrite);
void FlGSerialize(CString sFileName, BOOL bWrite);
void FlRSerialize(CString sFileName, BOOL bWrite);
void FlCoupleLevelSerialize(CString sFileName, BOOL bWrite);
#ifdef __H_SHAPE_STEEL_
void HShapeGuiGeSerialize(CString sFileName,int iPartSubType, BOOL bWrite);
#endif
void EBFittingSerialize(const char* sFileName,BOOL bWrite);
void GDFittingSerialize(const char* sFileName,BOOL bWrite);
#endif
