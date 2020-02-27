#ifndef __ADS_TOOLS_H_
#define __ADS_TOOLS_H_
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
/* 定义测量数组元素个数的函数 — ELEMENTS */
#define ELEMENTS(array) (sizeof(array)/sizeof((array)[0]))
extern ads_matrix CTM;		      // Current Transformation Matrix 
extern ads_point Base; 		      /* Base point for xform */

extern void ads_mat_ident(ads_matrix matrix);
int dragsampler(ads_point pt,ads_matrix mat);
struct resbuf *entitem(struct resbuf *rchain, int gcode);
int DragEntSet( ads_point base,char *prompt);
#define C_point(d, s)   d[X] = s[X];  d[Y] = s[Y];  d[Z] = s[Z];
#define Cpoint(d, s)   d[X] = s.x;  d[Y] = s.y;  d[Z] = s.z;
/* Set point variable from three co-ordinates */
#define Spoint(pt, x, y, z)  pt[X] = (x);  pt[Y] = (y);  pt[Z] = (z)
int JustifyInters(f3dPoint rgn_vert[4],const struct resbuf *filter);
//return 0表示无碰撞
int SearchNoneIntersPos(f3dPoint *ent_pos,f3dPoint rgn_vert[4],
					  const struct resbuf *filter,
					  int *quad_order=NULL,double scope_radius=40,int quad0_axis1X2Y=0,f3dPoint *searchDirect=NULL);
void GreateSelectionSet(ads_name set,char *sLayer="*", BOOL bSelLine=TRUE, BOOL bSelPoint=TRUE);
void ProcessHideLine(AcDbBlockTableRecord *pBlockTableRecord,fPtList& vertex_list,char *layer);
//平移实体集
int TranslationEntSet(ads_point base,ads_point cur,ads_name set);
#endif