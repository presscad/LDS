#ifndef __POST_PROCESS_HEADER_H_
#define __POST_PROCESS_HEADER_H_
#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
void CreateAnalyzeFile(char *file_name);
void CreateFreqsAnalRptTxtFileByCN(char *file_name,DYNAMIC_ANAL_NODE* node_arr,VIBRAMODE_PROP modes_arr[],int modes_num);
void CreateDetailRptTxtFileByCN(char *file_name,CFGWORD cfgwordBody,CFGWORD cfgwordLeg,int rpt_type);
void CreateDetailRptFileFromReportByCN(char *file_name,int iDocType=0);
void CreateDetailRptFileFromReportByEN(char *file_name,int iDocType=0);
#endif
#endif