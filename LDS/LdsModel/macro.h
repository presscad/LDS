#ifndef __LDS_PLATFORM_MACRO_HEADER_
#define __LDS_PLATFORM_MACRO_HEADER_
//本文件用于管理由于LDS平台下派生出来的越来越多的产品而带来的复杂宏定义
#if defined(__LDS_)||defined(__TMA_)||defined(__TSA_)||defined(__LMA_)||defined(__STATION_)||defined(__TDA_)
	#ifndef __LDS_CONTEXT_
		#define __LDS_CONTEXT_
	#endif
#elif defined(__LDSGROUP_)||defined(__TMAGROUP_)||defined(__TSAGROUP_)||defined(__LMAGROUP_)||defined(__TDAGROUP_)
	#ifndef __LDS_GROUP_CONTEXT_
		#define __LDS_GROUP_CONTEXT_
	#endif
#elif defined(__MAP_LDS_)||defined(__MAP_TMA_)||defined(__MAP_TSA_)||defined(__MAP_LMA_)||defined(__MAP_TDA_)
	#ifndef __DRAWING_CONTEXT_
		#define __DRAWING_CONTEXT_
	#endif
#endif
#if defined(__LDS_)||defined(__LDSGROUP_)||defined(__MAP_LDS_)	//产品或模块名称宏定义
	#ifndef __LDS_FILE_
		#define __LDS_FILE_	//模型内容定义
	#endif
	#if !defined(__LDS_TSA_)&&!defined(__RAPID_ROUGH_3DMODEL_)
		#define __RAPID_ROUGH_3DMODEL_	//快速生成三维粗略模型           
	#endif
#elif defined(__TMA_)||defined(__TMAGROUP_)||defined(__MAP_TMA_)
	#ifndef __TMA_FILE_
		#define __TMA_FILE_
	#endif
#elif defined(__LMA_)||defined(__LMAGROUP_)||defined(__MAP_LMA_)||defined(__STATION_)
	#ifndef __LMA_FILE_
		#define __LMA_FILE_
	#endif
#elif defined(__TSA_)||defined(__TSAGROUP_)||defined(__MAP_TSA_)
	#ifndef __TSA_FILE_
		#define __TSA_FILE_
	#endif
#elif defined(__TDA_)||defined(__TDAGROUP_)//||defined(__MAP_TDA_)
	#ifndef __TDA_FILE_
		#define __TDA_FILE_
	#endif
#endif
//根据模型内容宏定义确定具体构件子项及功能子项宏定义
#if defined(__LDS_FILE_)||defined(__TSA_FILE_)
	#ifndef __FEM_ANALYZE_INC_
		#define __FEM_ANALYZE_INC_	//杆塔有限元力学分析计算功能
	#endif
#endif
#if defined(__LDS_FILE_)||defined(__TSA_FILE_)
	#ifndef __PART_DESIGN_INC_
		#define __PART_DESIGN_INC_	//杆塔构件设计功能（根据已知内力进行单元验算，优化选材）
	#endif
#endif
#if defined(__LDS_FILE_)||defined(__TMA_FILE_)||defined(__TDA_FILE_)||defined(__LMA_FILE_)
	#ifndef __ANGLE_PART_INC_
		#define __ANGLE_PART_INC_	//角钢构件定义
	#endif
#endif
#if defined(__LDS_FILE_)||defined(__TDA_FILE_)
	#define __ADVANCED_EDITION_
#endif
#if defined(__LDS_FILE_)||defined(__TDA_FILE_)||defined(__LMA_FILE_)
	#ifndef __COMPLEX_PART_INC_
		#define __COMPLEX_PART_INC_	//角钢外等各类复杂型钢构件定义
	#endif
#endif
#if defined(__ANGLE_PART_INC_)||defined(__COMPLEX_PART_INC_)
	#ifndef __COMMON_PART_INC_
		#define __COMMON_PART_INC_	//常规钢构件如钢板、螺栓等
	#endif
#endif
#if (defined(__LDS_FILE_)||defined(__TDA_FILE_))&&!defined(__LDS_TSA_)
	#ifndef __SMART_DRAW_INC_
		#define __SMART_DRAW_INC_
	#endif
#endif
#ifdef AFX_TARG_ENU
#define AFX_TARG_ENU_ENGLISH
#endif

#if defined(__TMA_FILE_)
#define PRODUCT_TYPE 1
#elif defined(__LMA_FILE_)
#define PRODUCT_TYPE 2
#elif defined(__TSA_FILE_)
#define PRODUCT_TYPE 3
#elif defined(__LDS_FILE_)
#define PRODUCT_TYPE 4
#else
#define PRODUCT_TYPE 5
#endif
#endif
