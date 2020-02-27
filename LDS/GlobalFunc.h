#ifndef __GLOBAL_FUNC_H_
#define __GLOBAL_FUNC_H_

#include "Tower.h"
#include "lslist.h"
#include "env_def.h"
#include "ParseAdaptNo.h"
#include "CopyPasteOper.h"

int StatTrueBitsCount(DWORD flag);	//统计flag中值为1的位数
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL CalLineAngleWingNorm(CLDSLineAngle* pCurJg,CLDSLinePart *pByLinePart,f3dPoint view_norm,BOOL bUpdateOtherWing=TRUE,BOOL bSpecStyle=TRUE,BOOL bPromptError=TRUE);
int CalJgPosStyle(CLDSNode *pStart,CLDSNode *pEnd,const char *layer,int lstyle=0);
void GetWorkNorm(CLDSLineAngle *pMainJg, CLDSLineAngle* pViceJg, f3dPoint *norm);
//判断pJg是否有一肢的法线方向与norm大致相同
BOOL IsNormSameToPlank(CLDSLineAngle *pJg, f3dPoint norm);
BOOL IsInsideJg(CLDSLineAngle *pJg, f3dPoint norm, BYTE *x_wing0_y_wing1=NULL);
BOOL IsInsideJg(CLDSLineAngle *pJg, f3dPoint norm, int *x_wing0_y_wing1=NULL);
//计算角钢上指定节点处开合角起始与终止位置(角钢相对坐标值)Z坐标分量
BOOL CalKaiHeStartEndPos(CLDSLineAngle *pJg,CLDSNode *pNode,f3dPoint &startPos,f3dPoint &endPos,UCS_STRU &angleUcs,BOOL bIntelligent=FALSE,BOOL bUcsIncStartOdd=FALSE);
BOOL IsBerHuoQuJg(CLDSLineAngle *pHuoQuBaseJg, f3dPoint ray_jg_vec,int huoqu_x_wing0_y_wing1);
void CalAngleLayoutStyle(CLDSLineAngle *pRayAngle, CLDSLinePart *pDatumPart,int *ray_wing_x0_y1,int *datum_wing_x0_y1);
//计算十字插板立板法线
BOOL CalVerticalPlateNorm(CLDSPlate *pPlate,CLDSPart *pRefPart);
#endif

void MakeDirectory(char *path);
void DeleteDirectory(const char*  dirPath);
void SimplifiedNumString(CString &num_str);
char* SearchChar(char* srcStr,char ch,bool reverseOrder=false);
char* SearchChar(char* srcStr,const char* delimiter,bool reverseOrder=false);
void ANSIToUnicode(const char* src_str, wchar_t* des_str);
void UnicodeToANSI(const wchar_t* src_str, char* des_str);
void UTF8ToUnicode(const char* src_str, wchar_t* des_str);
void UnicodeToUTF8(const wchar_t* src_str, char* des_str);
void ANSIToUTF8(const char* src_str, char* des_str);
void UTF8ToANSI(const char* src_str, char* des_str);
int SelLsByWingWide(double wing_wide);
char restore_JG_guige(const char* guige, double &wing_wide, double &wing_thick);
char restore_JG_guige(const char* guige, double &wing_wide, double &wing_height, double &wing_thick);
char CalPartPos(CLDSLinePart* pPart);
int  GetMirLStyle(int style, int nMirMsg);
void CalMirAtomLayer(const char* sOrgLayer,char* sNewLayer, MIRMSG mirmsg);
//以下函数单独提取至TowerCad\CopyPasteOper.h中 wht 14-08-21
/*
void WritePointToClip(f3dPoint point);
BOOL ReadClipPoint(f3dPoint &point);
void ClearClipboard();
void WriteUCSToClip(UCS_STRU &ucs,FILE *fp=NULL);
BOOL ReadClipUCS(UCS_STRU &ucs,FILE *fp=NULL);
void WritePlaneToClip(f3dPoint &base_pos,f3dPoint &work_norm,FILE *fp=NULL);
BOOL ReadClipPlane(f3dPoint &base_pos,f3dPoint &work_norm,FILE *fp=NULL);
*/
void swap_ta_atom(CLDSPart *&pAtom1,CLDSPart*&pAtom2);
//ParsePartNo移至TowerCad\SegI.h中 wht 14-08-21
//bool ParsePartNo(char* sPartNo,SEGI* pSegKey,char* sSerialNo);

//BOOL ParsePartNo(char *sPartNo,int &iSegNo,char* sSerialNo);
//int RetrieveSegI(char *sPartNo);	功能与ParsePartNo相同 暂注释掉
int RetrievePartSerialNo(char *sPartNo);
BOOL PartNoToString(int iPartNo, char *sPartNo);
//根据编号定位构件
CLDSPart* FindPartByPartNo(char * sPartNo,int *pHits);
void GetNodeSetSizeScope(NODESET &nodeset,SCOPE_STRU &scope);
void SortGuiGe(CStringArray &str_arr,BOOL bTwoPara,int *order=NULL);
//判断dest_val是否为四个参数中的绝对值最大的一个(包括与其它值相等)
BOOL IsMaxInAll(double dest_val,double other1,double other2,double other3);

//以下函数单独提取至TowerCad\ParseAdaptNo.h中 wht 14-08-19
/*bool IsAdaptNoEnd();
int GetNoArr(char *limit_str,int no_arr[],int maxNoN);
long FindAdaptNo(char *limit_str,char *delimiter1=NULL,char *delimiter2=NULL);
SEGI FindAdaptNoForSegI(char *limit_str,char *delimiter1=NULL,char *delimiter2=NULL);
struct COEF_NO{
	int no;
	double coef;
	COEF_NO(){coef=no=0;}
	COEF_NO(int no_,double coef_){no=no_;coef=coef_;}
};
// <summary>
// 将一个代表含系数的编号序列的字符串解析为逐个含系数编号
// limit_str ：编号序列字符串 </param>
// delimiter1：编号间分隔符，一般为','</param>
// delimiter2：编号范围延续符，一般为'-'</param>
// delimiter3：编号的关联系数分隔，一般为'*'</param>
COEF_NO* FindAdaptCoefNo(char *limit_str,char *delimiter1=NULL,char *delimiter2=NULL,char *delimiter3=NULL);
*/
double GetPlasticDistortion(double thick,double neutro_t_coef,double bend_angle,double internal_radius,int base_inernal0_outter1);
#if defined(__LDS_)||defined(__TSA_)
CString MakeWeatherRegionSetString();
CString MakeWireTypeSetString();
CString MakeWireSetString(char *wireType);
void MakeWireTypeSetString(CComboBox *pCMB);
void MakeWireSetString(char *wireType,CComboBox *pCMB);
#endif
CString MakeArcLiftSetString();
CString MakeAngleSetString(char cType='L',bool bAddType=false);
CString MakeTubeSetString(BOOL bOnlyTubeD=FALSE);
CString MakeFlatSetString();
CString MakeSlotSetString();
CString MakeFittingSetString(BYTE ciFittingType);
CString MakeHShapeSetString(BYTE bHShapeType=0);
void ParseHShapeSpec(const char* sSpec,int iHShateType,SIZESPEC& size);
CString MakeBoltFamilySetString();
CString MakeBoltSetString(long hLsFamily);
CString MakeMaterialSetString();
CString MakeMaterialMarkSetString();
BOOL CheckNodeI(const CFGWORD* pCfgwordBody=NULL,const CFGWORD* pCfgwordLeg=NULL);//, short int *MAX_NODE_I=NULL);
//根据始终端节点得到杆件有效的对称信息
MIRMSG GetPoleValidMirMsg(CLDSNode *pStartNode,CLDSNode *pEndNode,int *xy1_yz2_xz3=0);
//根据起始点坐标得到杆件有效的对称信息
MIRMSG GetPoleValidMirMsg(f3dPoint startPos,f3dPoint endPos,int *xy1_yz2_xz3=0);
//根据设定区域初始化显示集合,pSameLifeRefObj表示选择与其具有共同生存空间的参照对象,NULL时默认为当前呼高
BOOL InitDisplaySetByPolyRgn(POLYGON &selRgn,CLDSDbObject* pSameLifeRefObj=NULL,NODESET* pRsltNodeSet=NULL,PARTSET* pRsltPartSet=NULL,bool rapidmode=false);
//根据杆件及对称节点初始化显示集合
BOOL InitDisplaySetByMirPoint(CLDSLinePart* pLinePart,CLDSNode* pMirStart,CLDSNode* pMirEnd);
//读取剪切板中的数据
typedef struct tagTABLE_DATA
{
	int nMaxRow;
	int nMaxCol;
	CStringArray strArr;
	tagTABLE_DATA(){nMaxRow=nMaxCol=0;}
	CString GetData(int nRow,int nCol)
	{
		if(nRow<0||nCol<0||nRow>nMaxRow-1||nCol>nMaxCol-1)
			return _T("");
		int nCurIndex=nRow*nMaxCol+nCol;
		if(nCurIndex<strArr.GetSize())
			return strArr[nRow*nMaxCol+nCol];
		else
			return _T("");
	}
}TABLE_DATA;
BOOL ReadStrFromClipboard(TABLE_DATA &table_data);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
CLDSLineTube *FindTubeInLine(CLDSLineTube *pLineTube,BOOL bStartEnd);
#endif
int SelBoltDByWingWide(double wing_wide,char* grade=NULL);
//通过段号字符串得到段号哈希表
//DWORD GetSegNoHashTblBySegStr(CString sSegStr,CHashList<SEGI> &segNoHashTbl);//移至TowerCad\ParseAdaptNo.h中 wht 14-09-20
//通过模型号字符串得到模型号哈希表
void GetModuleNoHashTblByModuleStr(CString sModuleScope,CHashTable<long> &moduleNoHashTbl);
//以下函数，添加链表元素时需要分配内存，如在外部释放动态库中分配的内存可能出错,故在此处实现 wht 12-05-17
int FindFlGParamByTubeD(double d,double couple_d,ATOM_LIST<FL_PARAM2*> &flList);
int FindFlGParamByTubeD(double d,ATOM_LIST<FL_PARAM2*> &flList);
int FindFlRParamByTubeD(double d,double couple_d,ATOM_LIST<FL_PARAM2*> &flList);
int FindFlRParamByTubeD(double d,ATOM_LIST<FL_PARAM2*> &flList);
void GetPlateSetConnectedInAngle(CLDSLineAngle *pAngle,PARTSET &plateSet,BYTE wing_x0_y1_all2,CLDSNode* pNode);
//根据角钢端头螺栓信息，智能初始化螺栓布置参数
bool SmartInitRayJgLsParas(CLDSNode* pNode,CLDSLineAngle *pRayJg,CLDSLineAngle* pBaseJg,ATOM_LIST<CDesignLsPara>* pLsParaList);
bool SmartInitBaseJgLsParas(CLDSNode* pNode,CLDSLineAngle *pBaseJg,BYTE work_wing,int bolt_num,ATOM_LIST<CDesignLsPara>* pLsParaList);
//根据角钢进行钢板制弯
bool BendPlateByLineAngle(CLDSPlate* pDatumPlate,CLDSLineAngle *pBendBaseJg,double fHuoquAngle,BYTE ciBend_Ber0_Wing1,double fBendSpace=10);
//////////////////////////////////////////////////////////////////////////
#endif