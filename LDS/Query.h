#ifndef __QUERY_H_
#define __QUERY_H_
#include "Tower.h"
#include "database.h"

//与构件直径有关的裹冰厚度修正系数alfa1
double QueryIceReviseCoefAlfa1(double diameter);
//high_pos(m)高度位置,裹冰厚度的高度递增系数alfa2
double QueryIceReviseCoefAlfa2(double high_pos);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL restore_Ls_guige(const char* guige, CLDSBolt& ls);
BOOL restore_Ls_guige(const char* guige, double* grade,long* d, long* L );
#endif
//BOOL getjgzj(JGZJ& jgzj,double wing_wide);
void AddSteelMatRecord(CComboBox *pCMB);
void AddJgRecord(CComboBox *pCMB);
void AddTubeRecord(CComboBox *pCMB,BOOL bOnlyAddTubeD=FALSE);
void AddBianRecord(CComboBox *pCMB);
void AddSlotRecord(CComboBox* pCMB);
void AddLsRecord(CComboBox* pCMB,long hLsFamily=0,BOOL bOnlyDiameter=FALSE);
//int FindTubeIndex(double D,double thick);
//int FindBianIndex(double wide,double thick);
#endif