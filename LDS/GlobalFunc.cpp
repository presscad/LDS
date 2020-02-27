//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
//#include "LDS.h"
#include "direct.h"
#include "io.h"
#include "Tower.h"
#include "XhCharString.h"
#include "GlobalFunc.h"
#include "WireLoadDatabase.h"
#include "env_def.h"
#include "./StdPartSolid/Fitting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CTower Ta;
int StatTrueBitsCount(DWORD flag)
{
	int count=0;
 	while(flag){
		count++;
		flag = flag&(flag-1);
	}
	return count;
}
void MakeDirectory(char *path)
{
	char bak_path[MAX_PATH],drive[MAX_PATH];
	strcpy(bak_path,path);
	char *dir = strtok(bak_path,"/\\");
	if(strlen(dir)==2&&dir[1]==':')
	{
		strcpy(drive,dir);
		strcat(drive,"\\");
		_chdir(drive);
		dir = strtok(NULL,"/\\");
	}
	while(dir)
	{
		_mkdir(dir);
		_chdir(dir);
		dir = strtok(NULL,"/\\");
	}
}
void DeleteDirectory(const char*  dirPath)
{
	if(access(dirPath, 0)==-1)
		return;	//不存在此目录
	char  path[MAX_PATH];
	strcpy(path,dirPath);
    strcat (path,"\\*");
	_finddata_t fb;   //查找相同属性文件的存储结构体
	long handle =_findfirst(path,&fb);
	if(handle != 0)
	{
		while(0 == _findnext(handle,&fb))
		{
			if(0 != strcmp(fb.name,".."))
			{	//windows下，常有个系统文件，名为“..”,对它不做处理
				memset(path,0,sizeof(path));
				strcpy(path,dirPath);
				strcat(path,"\\");
				strcat (path,fb.name);
				if(fb.attrib==16)
					DeleteDirectory(path);	//迭代删除子文件夹
				else
					remove(path);	//删除文件
			}	
		}
		_findclose(handle);
	}
	//移除文件夹
	int iRet=rmdir(dirPath);
}
void SimplifiedNumString(CString &num_str)
{
	int str_len = num_str.GetLength();
	int pointPos = -1;
	for(int i=str_len-1;i>=0;i--)
	{
		if(num_str[i]=='.')
		{
			pointPos=i;
			break;
		}
	}
	if(pointPos<0)
		return;
	if(pointPos>0&&!isdigit((BYTE)num_str[pointPos-1]))
		return; //小数点前一位为数字时才进行下一步的简化 wht 11-04-01
	int iPos=str_len-1;
	while(iPos>=pointPos)
	{
		if(num_str[iPos]=='0'||num_str[iPos]=='.')
			iPos--;
		else
			break;
	}
	num_str=num_str.Left(iPos+1);
	if(num_str.Compare("-0")==0)
		num_str="0";
}
char* SearchChar(char* srcStr,char ch,bool reverseOrder/*=false*/)
{
	if(!reverseOrder)
		return strchr(srcStr,ch);
	else
	{
		int len=strlen(srcStr);
		for(int i=len-1;i>=0;i--)
		{
			if(srcStr[i]==ch)
				return &srcStr[i];
		}
	}
	return NULL;
}
char* SearchChar(char* srcStr,const char* delimiter,bool reverseOrder/*=false*/)
{
	char* iter=NULL;
	const char* iter2=NULL;
	if(!reverseOrder)
	{
		for(iter=srcStr;*iter!=0;iter++)
		{
			for(iter2=delimiter;*iter2!=0;iter2++)
			{
				if(*iter==*iter2)
					return iter;
			}
		}
	}
	else
	{
		int len=strlen(srcStr);
		for(int i=len-1;i>=0;i--)
		{
			for(iter2=delimiter;*iter2!=0;iter2++)
			{
				if(srcStr[i]==*iter2)
					return &srcStr[i];
			}
		}
	}
	return NULL;
}
void ANSIToUnicode(const char* src_str, wchar_t* des_str)
{
	int nLen = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
	wchar_t* sResult = (wchar_t*)malloc((nLen + 1) * sizeof(wchar_t));
	memset(sResult, 0, sizeof(wchar_t)*(nLen + 1));
	MultiByteToWideChar(CP_ACP, 0, src_str, -1, (LPWSTR)sResult, nLen);
	//
	memset(des_str, 0, MAX_PATH);
	wcscpy(des_str, sResult);
	free(sResult);
}

void UnicodeToANSI(const wchar_t* src_str, char* des_str)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, src_str, -1, NULL, 0, NULL, NULL);
	char* sResult = (char*)malloc((nLen + 1) * sizeof(char));
	memset(sResult, 0, sizeof(char)*(nLen + 1));
	WideCharToMultiByte(CP_ACP, 0, src_str, -1, sResult, nLen, NULL, NULL);
	//
	memset(des_str, 0, MAX_PATH);
	strcpy(des_str, sResult);
	free(sResult);
}

void UTF8ToUnicode(const char* src_str, wchar_t* des_str)
{
	int nLen = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* sResult = (wchar_t*)malloc((nLen + 1) * sizeof(wchar_t));
	memset(sResult, 0, (nLen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, (LPWSTR)sResult, nLen);
	//
	memset(des_str, 0, MAX_PATH);
	wcscpy(des_str, sResult);
	free(sResult);
}

void UnicodeToUTF8(const wchar_t* src_str, char* des_str)
{
	int nLen = WideCharToMultiByte(CP_UTF8, 0, src_str, -1, NULL, 0, NULL, NULL);
	char* sResult = (char*)malloc((nLen + 1) * sizeof(char));
	memset(sResult, 0, sizeof(char)*(nLen + 1));
	WideCharToMultiByte(CP_UTF8, 0, src_str, -1, sResult, nLen, NULL, NULL);
	//
	memset(des_str, 0, MAX_PATH);
	strcpy(des_str, sResult);
	free(sResult);
}
void ANSIToUTF8(const char* src_str, char* des_str)
{
	wchar_t sWText[MAX_PATH];
	ANSIToUnicode(src_str, sWText);	//ansi多字符转宽字符
	UnicodeToUTF8(sWText, des_str);	//宽字符转utf8多字符
}
void UTF8ToANSI(const char* src_str, char* des_str)
{
	wchar_t sWText[MAX_PATH];
	UTF8ToUnicode(src_str, sWText);	//utf8多字符转宽字符
	UnicodeToANSI(sWText, des_str);	//宽字符转ansi多字符
}
void SortGuiGe(CStringArray &str_arr,BOOL bTwoPara,int *order/*=NULL*/)
{
	int i,j;
	int nGuiGe = str_arr.GetSize();
	CString pureguige1,pureguige2,temStr;
	if(order)
	{
		for(i=0;i<nGuiGe;i++)
			order[i] = i;
	}
	for(i=0;i<nGuiGe;i++)
	{
		BOOL bSorted = TRUE;
		double Para1[2],Para2[2];
		for(j=0;j<nGuiGe-i;j++)
		{
			pureguige1 = str_arr[j];
			pureguige2 = str_arr[(j+1)%nGuiGe];
			pureguige1.Remove('L');
			pureguige2.Remove('L');
			pureguige1.Remove('M');
			pureguige2.Remove('M');
			pureguige1.Remove('-');
			pureguige2.Remove('-');
			pureguige1.Remove('_');
			pureguige2.Remove('_');
			if(!bTwoPara)	//单参数
			{
				sscanf(pureguige1,"%lf",&Para1[0]);
				sscanf(pureguige2,"%lf",&Para1[1]);
				if(Para1[0]>Para1[1])
				{
					//值交换
					temStr = str_arr[j];
					str_arr[j] = str_arr[(j+1)%nGuiGe];
					str_arr[(j+1)%nGuiGe] = temStr;
					//序号交换
					if(order)
					{
						int temOrder = order[j];
						order[j] = order[(j+1)%nGuiGe];
						order[(j+1)%nGuiGe] = temOrder;
					}
					bSorted = FALSE;
				}
			}
			else
			{
				char ch;
				sscanf(pureguige1,"%lf%c%lf",&Para1[0],&ch,&Para2[0]);
				sscanf(pureguige2,"%lf%c%lf",&Para1[1],&ch,&Para2[1]);
				if(Para1[0]>Para1[1]||(Para1[0]==Para1[1]&&Para2[0]>Para2[1]))
				{
					temStr = str_arr[j];
					str_arr[j] = str_arr[(j+1)%nGuiGe];
					str_arr[(j+1)%nGuiGe] = temStr;
					bSorted = FALSE;
				}
			}
		}
		if(bSorted)	//已有序
			break;
	}
}

void GetNodeSetSizeScope(NODESET &nodeset,SCOPE_STRU &scope)
{
	CLDSNode *pNode;
	scope.fMinX = scope.fMinY = scope.fMinZ = 100000.0;
	scope.fMaxX = scope.fMaxY = scope.fMaxZ =-100000.0;
	for(pNode=nodeset.GetFirst();pNode!=NULL;pNode=nodeset.GetNext())
	{
		scope.fMaxX = __max(scope.fMaxX, pNode->Position(false).x);
		scope.fMaxY = __max(scope.fMaxY, pNode->Position(false).y);
		scope.fMaxZ = __max(scope.fMaxZ, pNode->Position(false).z);
		scope.fMinX = __min(scope.fMinX, pNode->Position(false).x);
		scope.fMinY = __min(scope.fMinY, pNode->Position(false).y);
		scope.fMinZ = __min(scope.fMinZ, pNode->Position(false).z);
	}
}

int SelLsByWingWide(double wing_wide)
{
	//if(wing_wide<50)	//这是王金合自己定的
	//	return 12;
	//else 
	if(wing_wide<63)
		return 16;
	else if(wing_wide<140)
		return 20;
	else
		return 24;
}

char restore_JG_guige(const char* guige, double &wing_wide, double &wing_thick)
{
	char mark,material;	//symbol='L' mark='X';
	const char* pszSizeSpec=strchr(guige,'L');
	if (pszSizeSpec!=NULL)
		guige=pszSizeSpec+1;
	sscanf(guige,"%lf%c%lf%c",&wing_wide,&mark,&wing_thick,&material);
	material  = (char)toupper(material);//角钢材质A3(S)或Q345(H)

	return material;
}
//不等肢角钢的规格分解
char restore_JG_guige(const char* guige, double &wing_wide, double &wing_height, double &wing_thick)
{
	char mark,material=0,mark2=0;	//symbol='L' mark='X';
	const char* pszSizeSpec=strchr(guige,'L');
	if (pszSizeSpec!=NULL)
		guige=pszSizeSpec+1;
	int type=0;
	int strLen = strlen(guige);
	for (int i=0;i<strLen;i++)
	{
		if(guige[i]=='X'||guige[i]=='x'||guige[i]=='*')
			type++;
	}
	if(type==1)
	{
		sscanf(guige,"%lf%c%lf%c",&wing_wide,&mark,&wing_thick,&material);
		wing_height=0;
	}
	else if(type==2)
		sscanf(guige,"%lf%c%lf%c%lf%c",&wing_wide,&mark,&wing_height,&mark2,&wing_thick,&material);
	material  = (char)toupper(material);//角钢材质A3(S)或Q345(H)

	return material;
}

int  GetMirLStyle(int style, int nMirMsg)
{
	switch(nMirMsg)
	{
	case 0:		//布置方式图左右对称
		return 5-style;
	case 1:		//布置方式图前后对称
		if(style>2)
			return 7-style;		//3--4对应
		else
			return 3-style;		//1--2对应
	case 2:		//布置方式图Z轴对称
		if(style%2>0)	//奇数1--3对应
			return 4-style;
		else			//偶数2--4对应
			return 6-style;
	default:
		return style;
	}
}

//此函数有问题
char CalPartPos(CLDSLinePart* pPart)
{
	f3dPoint start,end;
	start = pPart->pStart->Position(false);
	end	  = pPart->pEnd->Position(false);
	if(start.x==0&&start.y==0&&end.x==0&&end.y==0)
		return 'T';
	else if( pPart->layer(1)=='Z')//主材布于四象限
	{
		if(start.x>=0&&start.y>=0&&end.x>=0&&end.y>=0)
			return '1';
		else if(start.x<=0&&start.y>=0&&end.x<=0&&end.y>=0)
			return '2';
		else if(start.x>=0&&start.y<=0&&end.x>=0&&end.y<=0)
			return '3';
		else if(start.x<=0&&start.y<=0&&end.x<=0&&end.y<=0)
			return '4';
		else if(start.y>0&&end.y>0)
			return 'Q';
		else if(start.y<0&&end.y<0)
			return 'H';
		else if(start.x>0&&end.x>0)
			return 'Z';
		else if(start.x<0&&end.x<0)
			return 'Y';
		else
			return 'Q';
	}	//非主材(斜补材)
	else if(start.y>=0&&end.y>=0&&start.x*end.x<=0)
		return 'Q';
	else if(start.y<=0&&end.y<=0&&start.x*end.x<=0)
		return 'H';
	else if(start.x<=0&&end.x<=0&&start.y*end.y<=0)
		return 'Z';
	else if(start.x>=0&&end.x>=0&&start.y*end.y<=0)
		return 'Y';
	else if(start.x>=0&&start.y>=0&&end.x>=0&&end.y>=0)
		return '1';
	else if(start.x<=0&&start.y>=0&&end.x<=0&&end.y>=0)
		return '2';
	else if(start.x>=0&&start.y<=0&&end.x>=0&&end.y<=0)
		return '3';
	else if(start.x<=0&&start.y<=0&&end.x<=0&&end.y<=0)
		return '4';
	else
		return 'T';
}
void CalMirAtomLayer(const char* sOrgLayer,char* sNewLayer,MIRMSG mirmsg)
{
	if(sNewLayer!=sOrgLayer)
		strcpy(sNewLayer,sOrgLayer);

	if(mirmsg.axis_flag&1)
	{	//X轴对称
		if(sOrgLayer[2]=='Q')
			sNewLayer[2]='H';
		else if(sOrgLayer[2]=='H')
			sNewLayer[2]='Q';
		else if(sOrgLayer[2]=='1')
			sNewLayer[2]='3';
		else if(sOrgLayer[2]=='2')
			sNewLayer[2]='4';
		else if(sOrgLayer[2]=='3')
			sNewLayer[2]='1';
		else if(sOrgLayer[2]=='4')
			sNewLayer[2]='2';
	}
	else if(mirmsg.axis_flag&2)
	{	//Y轴对称
		if(sOrgLayer[2]=='Z')
			sNewLayer[2]='Y';
		else if(sOrgLayer[2]=='Y')
			sNewLayer[2]='Z';
		else if(sOrgLayer[2]=='1')
			sNewLayer[2]='2';
		else if(sOrgLayer[2]=='2')
			sNewLayer[2]='1';
		else if(sOrgLayer[2]=='3')
			sNewLayer[2]='4';
		else if(sOrgLayer[2]=='4')
			sNewLayer[2]='3';
	}
	else if(mirmsg.axis_flag&4)
	{	//Z轴对称
		if(sOrgLayer[2]=='Q')
			sNewLayer[2]='H';
		else if(sOrgLayer[2]=='H')
			sNewLayer[2]='Q';
		else if(sOrgLayer[2]=='Z')
			sNewLayer[2]='Y';
		else if(sOrgLayer[2]=='Y')
			sNewLayer[2]='Z';
		else if(sOrgLayer[2]=='1')
			sNewLayer[2]='4';
		else if(sOrgLayer[2]=='2')
			sNewLayer[2]='3';
		else if(sOrgLayer[2]=='3')
			sNewLayer[2]='2';
		else if(sOrgLayer[2]=='4')
			sNewLayer[2]='1';
	}
	/*else 有些时候轴对称与旋转对称同时起作用，此时必须复合处理图层名的对称问题 wjh-2017.12.21*/
	if(mirmsg.axis_flag&8)	//旋转对称
	{
		if(abs(mirmsg.rotate_angle)%90>0)
			return;	//非方塔四角对称不对称更新图层名 wjh-2015.6.09
		int iRot=((-mirmsg.rotate_angle/90)%4);
		if(iRot<0)
			iRot+=4;
		char mir_CHAR1[5]="QYHZ";
		char mir_CHAR2[5]="2134";
		if(sNewLayer[2]=='Q')
			sNewLayer[2]=mir_CHAR1[iRot];
		else if(sNewLayer[2]=='H')
			sNewLayer[2]=mir_CHAR1[(iRot+2)%4];
		else if(sNewLayer[2]=='Z')
			sNewLayer[2]=mir_CHAR1[(iRot+3)%4];
		else if(sNewLayer[2]=='Y')
			sNewLayer[2]=mir_CHAR1[(iRot+1)%4];
		else if(sNewLayer[2]=='1')
			sNewLayer[2]=mir_CHAR2[(iRot+1)%4];
		else if(sNewLayer[2]=='2')
			sNewLayer[2]=mir_CHAR2[iRot];
		else if(sNewLayer[2]=='3')
			sNewLayer[2]=mir_CHAR2[(iRot+2)%4];
		else if(sNewLayer[2]=='4')
			sNewLayer[2]=mir_CHAR2[(iRot+3)%4];
	}
	else if(mirmsg.axis_flag==0x10&&(mirmsg.mir_norm.IsEqual(1,-1,0)||mirmsg.mir_norm.IsEqual(-1,1,0)))
	{	//45°镜面对称
		if(sNewLayer[2]=='Q')
			sNewLayer[2]='Y';
		else if(sNewLayer[2]=='H')
			sNewLayer[2]='Z';
		else if(sNewLayer[2]=='Z')
			sNewLayer[2]='H';
		else if(sOrgLayer[2]=='Y')
			sNewLayer[2]='Q';
		else if(sNewLayer[2]=='1')
			sNewLayer[2]='1';
		else if(sNewLayer[2]=='2')
			sNewLayer[2]='3';
		else if(sNewLayer[2]=='3')
			sNewLayer[2]='2';
		else if(sNewLayer[2]=='4')
			sNewLayer[2]='4';
	}
	else if(mirmsg.axis_flag==0x10&&(mirmsg.mir_norm.IsEqual(1,1,0)||mirmsg.mir_norm.IsEqual(-1,-1,0)))
	{	//135°镜面对称
		if(sNewLayer[2]=='Q')
			sNewLayer[2]='Z';
		else if(sNewLayer[2]=='H')
			sNewLayer[2]='Y';
		else if(sNewLayer[2]=='Z')
			sNewLayer[2]='Q';
		else if(sNewLayer[2]=='Y')
			sNewLayer[2]='H';
		else if(sNewLayer[2]=='1')
			sNewLayer[2]='4';
		else if(sNewLayer[2]=='2')
			sNewLayer[2]='2';
		else if(sNewLayer[2]=='3')
			sNewLayer[2]='3';
		else if(sNewLayer[2]=='4')
			sNewLayer[2]='1';
	}
}
/*
BOOL ReadClipPoint(f3dPoint &point)
{
	FILE *fp;
	char filename[MAX_PATH];
	GetSystemDirectory(filename,MAX_PATH);
	strcat(filename,"\\TowerClip.tmp");
	if((fp=fopen(filename,"rt"))==NULL)
	{
		AfxMessageBox("无数据可粘贴!");
		return FALSE;
	}
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&point.x);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&point.y);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&point.z);
	fclose(fp);
	return TRUE;
}
void WritePointToClip(f3dPoint point)
{
	FILE *fp;
	char filename[MAX_PATH];
	GetSystemDirectory(filename,MAX_PATH);
	strcat(filename,"\\TowerClip.tmp");
	if((fp=fopen(filename,"wt"))==NULL)
	{
		AfxMessageBox("临时文件读写失败,粘贴失败!");
		return;
	}
	fprintf(fp,"%.14lf ",point.x);
	fprintf(fp,"%.14lf ",point.y);
	fprintf(fp,"%.14lf ",point.z);
	fclose(fp);
}
void ClearClipboard()
{
	FILE *fp;
	char filename[MAX_PATH];
	GetSystemDirectory(filename,MAX_PATH);
	strcat(filename,"\\TowerClip.tmp");
	if((fp=fopen(filename,"w+"))==NULL)
	{
		AfxMessageBox("临时文件清空失败!");
		return;
	}
	fclose(fp);
}
BOOL ReadClipUCS(UCS_STRU &ucs,FILE *fp)
{
	BOOL bExternalFP=TRUE;
	char filename[MAX_PATH];
	UCS_STRU tm_ucs;
	GetSystemDirectory(filename,MAX_PATH);
	strcat(filename,"\\TowerUcs.tmp");
	if(fp==NULL)
	{
		bExternalFP=FALSE;
		if((fp=fopen(filename,"rt"))==NULL)
		{
			AfxMessageBox("无数据可粘贴!");
			return FALSE;
		}
	}
	//Read ucs origin
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.origin.x);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.origin.y);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.origin.z);
	//Read axis X
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_x.x);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_x.y);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_x.z);
	//Read axis Y
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_y.x);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_y.y);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_y.z);
	//Read axis z
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_z.x);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_z.y);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_z.z);
	if(!bExternalFP)	//只有内部文件需要关闭此文件
		fclose(fp);
	ucs = tm_ucs;
	return TRUE;
}
void WriteUCSToClip(UCS_STRU &ucs,FILE *fp)
{
	BOOL bExternalFP=TRUE;
	char filename[MAX_PATH];
	GetSystemDirectory(filename,MAX_PATH);
	if(fp==NULL)
	{
		bExternalFP = FALSE;
		strcat(filename,"\\TowerUcs.tmp");
		if((fp=fopen(filename,"wt"))==NULL)
		{
			AfxMessageBox("临时文件读写失败,粘贴失败!");
			return;
		}
	}
	fprintf(fp,"%.14lf ",ucs.origin.x);
	fprintf(fp,"%.14lf ",ucs.origin.y);
	fprintf(fp,"%.14lf ",ucs.origin.z);
	fprintf(fp,"%.14lf ",ucs.axis_x.x);
	fprintf(fp,"%.14lf ",ucs.axis_x.y);
	fprintf(fp,"%.14lf ",ucs.axis_x.z);
	fprintf(fp,"%.14lf ",ucs.axis_y.x);
	fprintf(fp,"%.14lf ",ucs.axis_y.y);
	fprintf(fp,"%.14lf ",ucs.axis_y.z);
	fprintf(fp,"%.14lf ",ucs.axis_z.x);
	fprintf(fp,"%.14lf ",ucs.axis_z.y);
	fprintf(fp,"%.14lf \n",ucs.axis_z.z);
	if(!bExternalFP)	//只有内部文件需要关闭此文件
		fclose(fp);
}
BOOL ReadClipPlane(f3dPoint &base_pos,f3dPoint &work_norm,FILE *fp)
{
	BOOL bExternalFP=TRUE;
	char filename[MAX_PATH];
	UCS_STRU tm_ucs;
	GetSystemDirectory(filename,MAX_PATH);
	strcat(filename,"\\TowerPlane.tmp");
	if(fp==NULL)
	{
		bExternalFP=FALSE;
		if((fp=fopen(filename,"rt"))==NULL)
		{
			AfxMessageBox("无数据可粘贴!");
			return FALSE;
		}
	}
	//Read 基准点
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&base_pos.x);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&base_pos.y);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&base_pos.z);
	//Read 工作法线
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&work_norm.x);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&work_norm.y);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&work_norm.z);
	if(!bExternalFP)	//只有内部文件需要关闭此文件
		fclose(fp);
	return TRUE;
}
void WritePlaneToClip(f3dPoint &base_pos,f3dPoint &work_norm,FILE *fp)
{
	BOOL bExternalFP=TRUE;
	char filename[MAX_PATH];
	GetSystemDirectory(filename,MAX_PATH);
	if(fp==NULL)
	{
		bExternalFP = FALSE;
		strcat(filename,"\\TowerPlane.tmp");
		if((fp=fopen(filename,"wt"))==NULL)
		{
			AfxMessageBox("临时文件读写失败,粘贴失败!");
			return;
		}
	}
	fprintf(fp,"%.14lf ",base_pos.x);
	fprintf(fp,"%.14lf ",base_pos.y);
	fprintf(fp,"%.14lf ",base_pos.z);
	fprintf(fp,"%.14lf ",work_norm.x);
	fprintf(fp,"%.14lf ",work_norm.y);
	fprintf(fp,"%.14lf \n",work_norm.z);
	if(!bExternalFP)	//只有内部文件需要关闭此文件
		fclose(fp);
}*/
//判断dest_val是否为四个参数中的绝对值最大的一个(包括与其它值相等)
BOOL IsMaxInAll(double dest_val,double other1,double other2,double other3)
{
	if(fabs(dest_val)-fabs(other1)<-EPS)
		return FALSE;
	else if(fabs(dest_val)-fabs(other2)<-EPS)
		return FALSE;
	else if(fabs(dest_val)-fabs(other3)<-EPS)
		return FALSE;
	else
		return TRUE;
}
void swap_ta_atom(CLDSPart *&pAtom1,CLDSPart*&pAtom2)
{
	CLDSPart *pAtom = pAtom1;
	pAtom1 = pAtom2;
	pAtom2 = pAtom;
}
/*
bool ParsePartNo(char* sPartNo,SEGI* pSegI,char* sSerialNo)
{
	int i;
	int str_len = strlen(sPartNo);
	int separator = -1;
	pSegI->key.number=0;
	pSegI->key.prefix[0]=pSegI->key.prefix[1]=0;
	int iDigitStart=0;	//构件编号中首个数字的出现位置索引
	int iDigitFinal=0;
	if(sSerialNo)
		strcpy(sSerialNo,"");
	for(i=0;i<str_len;i++)
	{
		if(isdigit(sPartNo[i]))
			iDigitFinal=i;
		if(i==0&&!isdigit(sPartNo[i]))
		{
			pSegI->key.prefix[0]=sPartNo[0];
			iDigitStart=1;
		}
		else if(i==1&&!isdigit(sPartNo[i])&&sPartNo[i]!='-'&&sPartNo[i]!='_')
		{
			memcpy(pSegI->key.prefix,sPartNo,2);
			iDigitStart=2;
		}
		else if(separator<0&&(sPartNo[i]=='_' || sPartNo[i]=='-'))
			separator=i;
	}
	if(separator>0)
	{	//有段号流水号间分隔符
		char numStr[6]="";
		int len=min(6,separator-iDigitStart);
		if(len>0)
		{
			memcpy(numStr,&sPartNo[iDigitStart],len);
			for(i=0;i<len;i++)
			{
				if(numStr[i]!='.'&&isdigit(numStr[i])==0)	//有字母存在时直接转换为数字段号会失真，此时应从字母之后认为是流水号
				{
					separator=i+iDigitStart-1;
					break;
				}
			}
			pSegI->key.number=atoi(numStr);
		}
		else
			return false;
		if(sSerialNo)
		{
			if(separator+1<str_len)
			{
				memcpy(sSerialNo,&sPartNo[separator+1],str_len-separator);
				sSerialNo[str_len-separator]=0;
			}
			else
				return false;
		}
	}
	else
	{	//无段号流水号间分隔符，默认取后两位连续数字及后缀作为流水号
		char numStr[8]="";
		int len=min(8,iDigitFinal-1-iDigitStart);
		if(len>0)
		{
			memcpy(numStr,&sPartNo[iDigitStart],len);
			pSegI->key.number=atoi(numStr);
		}
		else if(len<0)	
		{	//针对FL7A这样的编号(流水号中数字少于2)提取
			if(sSerialNo)
			{
				memcpy(sSerialNo,&sPartNo[iDigitStart],str_len-iDigitStart);
				sSerialNo[str_len-iDigitStart]=0;
			}
			return true;
		}
		if(sSerialNo)
		{
			memcpy(sSerialNo,&sPartNo[iDigitFinal-1],str_len-iDigitFinal+1);
			sSerialNo[str_len-iDigitFinal+1]=0;
		}
	}
	return TRUE;
}*/

/*BOOL ParsePartNo(char* sPartNo,int &iSegNo,char *sSerialNo)
{
	int i;
	bool bFindSeparator = false;
	int iStart = -1, iEnd = 0;
	int iStrLen = strlen(sPartNo);
	char sSegNum[100]="";
	//第一个字符不是数字返回FALSE
	if(!isdigit(sPartNo[0]))
		return FALSE;
	for(i=0; i<iStrLen; i++)
	{
		if(isdigit(sPartNo[i]) || sPartNo[i]=='_' || sPartNo[i]=='-')
		{
			if(isdigit(sPartNo[i]) == 0)
				bFindSeparator = true;
			if(iStart == -1)
			{
				iStart = i;
				iEnd = i-1;
			}
			iEnd++;		//如果字符串均为数字在此处记录iEnd的值
		}
		else
		{
			if(iStart >= 0)
			{
				iEnd = i-1;
				break;
			}
		}
	}

	if(bFindSeparator)
	{
		for(i=iStart; i<=iEnd; i++)
		{
			if(sPartNo[i]=='-' || sPartNo[i]=='-')
			{
				sSegNum[i-iStart] = '\0';
				break;
			}
			else
				sSegNum[i-iStart] = sPartNo[i];
		}
		if(sSerialNo)
			strcpy(sSerialNo,&sPartNo[i+1]);
	}
	else
	{
		if((iEnd-iStart+1)>=3)
		{
			for(i=iStart; i<=iEnd-2; i++)
				sSegNum[i-iStart] = sPartNo[i];
			sSegNum[iEnd-iStart+1-2] = '\0';
			if(sSerialNo)
				strcpy(sSerialNo,&sPartNo[iEnd-1]);
		}
	}
	iSegNo = atoi(sSegNum);
	return TRUE;
}*/
BOOL PartNoToString(int iPartNo, char *sPartNo)
{
	if(iPartNo<100)
	{
		strcpy(sPartNo,"0");
		return FALSE;
	}
	if(iPartNo%1000>=100)
		_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%d-%d",iPartNo/1000,iPartNo%1000);
	else
		_snprintf(sPartNo,MAX_CHAR_GUIGE_16,"%d",(iPartNo/1000)*100+iPartNo%100);
	return TRUE;
}
//根据构件编号查找构件并返回
CLDSPart* FindPartByPartNo(char *sPartNo,int *pHits)
{
	CLDSPart *pPart,*pFindPart=NULL;
	int nHits=0;
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->m_bVirtualPart)
			continue;
		if(	stricmp(pPart->GetPartNo(),sPartNo)==0)
		{
			if(pHits==NULL)
				return pPart;
			else if(nHits==0)
			{
				pFindPart = pPart;
				nHits++;
			}
			else
				nHits++;
		}
	}
	if(pFindPart)
	{
		if(pHits)
			*pHits = nHits;
		return pFindPart;
	}
	return NULL;
}
//以下函数单独提取至TowerCad\ParseAdaptNo.cpp中 wht 14-08-19
/*bool IsAdaptNoEnd();
bool IsAdaptNoEndForSegI();
int GetNoArr(char *limit_str,int no_arr[],int maxNoN);
long FindAdaptNo(char *limit_str,char *delimiter1=NULL,char *delimiter2=NULL);
SEGI FindAdaptNoForSegI(char *limit_str,char *delimiter1=NULL,char *delimiter2=NULL);
COEF_NO* FindAdaptCoefNo(char *limit_str,char *delimiter1=NULL,char *delimiter2=NULL,char *delimiter3=NULL);*/

double GetPlasticDistortion(double thick,double neutro_t_coef,double bend_angle,double internal_radius,int base_internal0_outter1)
{
	double neutro_len = bend_angle*(internal_radius+thick*neutro_t_coef);
	double half_bend_angle = 0.5*bend_angle;
	if(base_internal0_outter1==0)
	{
		double internal_len=2*internal_radius*tan(half_bend_angle);
		return neutro_len-internal_len;
	}
	else
	{
		double outter_len=2*(internal_radius+thick)*tan(half_bend_angle);
		return neutro_len-outter_len;
	}
}

//功能与ParsePartNo相同,但不如PartPartNo暂注释掉	wht 10-04-08
/*
int RetrieveSegI(char *sPartNo)
{
	int i;
	bool bFindSeparator = false;
	int iStart = -1, iEnd = 0;
	int iStrLen = strlen(sPartNo);
	char sSegNum[100];

	for(i=0; i<iStrLen; i++)
	{
		if(isdigit(sPartNo[i]) || sPartNo[i]=='_' || sPartNo[i]=='-')
		{
			if(isdigit(sPartNo[i]) == 0)
				bFindSeparator = true;
			if(iStart == -1)
			{
				iStart = i;
				iEnd = i-1;
			}
			iEnd++;		//如果字符串均为数字在此处记录iEnd的值
		}
		else
		{
			if(iStart >= 0)
			{
				iEnd = i-1;
				break;
			}
		}
	}

	if(bFindSeparator)
	{
		for(i=iStart; i<=iEnd; i++)
		{
			if(sPartNo[i]=='-' || sPartNo[i]=='-')
				sSegNum[i-iStart] = '\0';
			else
				sSegNum[i-iStart] = sPartNo[i];
		}
	}
	else
	{
		if((iEnd-iStart+1)>=3)
		{
			for(i=iStart; i<=iEnd-2; i++)
				sSegNum[i-iStart] = sPartNo[i];
			sSegNum[iEnd-iStart+1-2] = '\0';
		}
	}
	return atoi(sSegNum);
}*/

int RetrievePartSerialNo(char *sPartNo)
{
	SEGI segI;
	CXhChar16 serialstr;
	if(ParsePartNo(sPartNo,&segI,serialstr))
		return atoi(serialstr);
	//后续代码太旧了，如果遇到类似552A这类编号就会识别错误　wjh-2017.8.8
	int i;
	bool bFindSeparator = false;
	bool bHasPrefix=false;
	int iStart = 0, iEnd = 0;
	int str_len = strlen(sPartNo);

	for(i=0; i<str_len; i++)
	{
		if(isdigit((BYTE)sPartNo[i]))
		{
			iStart=i;
			break;
		}
		else
			bHasPrefix=true;
	}
	for(i=0; i<str_len; i++)
	{
		if(sPartNo[i]=='-'||sPartNo[i]=='_')
		{
			iStart=i;
			bFindSeparator=true;
		}
	}
	if(bFindSeparator)
		iStart++;
	else if(!bHasPrefix)
		iStart=str_len-2;
	if(iStart>=str_len||iStart<0)
		return 0;
	else
		return atoi(&sPartNo[iStart]);
}

int AddNoStrNum(char *sSrc, int nLsNum, char *sDest, int nStrDestLen)
{
	int nStrLen = 0;		//目的字符串长度
	char sTemp[5];			//保存数字字符串的临时变量
	int nTempLen = 0;		//临时字符串的长度
	if(nStrDestLen>0)
		memset(sDest, 0, nStrDestLen);
	memset(sTemp, 0, 5);
	for(int i=0; i<(int)strlen(sSrc); i++)
	{
		if(isdigit((BYTE)sSrc[i]))//判断是否为数字，是数字则保存到临时变量中
		{
			sTemp[nTempLen] = sSrc[i];
			nTempLen++;
		}
		else					
		{	
			if(sTemp[0]!='\0')				//防止非数字开头的情况
			{
				itoa(atoi(sTemp)+nLsNum, sTemp, 10);
				if(nStrDestLen>0 && nStrLen<nStrDestLen)
				{//将经过计算后的数字写入目的字符串
					if(nStrDestLen<(nStrLen+(int)strlen(sTemp)))
						strncat(sDest, sTemp, nStrDestLen-nStrLen);
					else
						strcat(sDest, sTemp);
				}	
				nStrLen += strlen(sTemp);	//目的字符串的长度
			}
			if(nStrDestLen>0 && nStrLen<=(nStrDestLen-1))
				sDest[nStrLen] = sSrc[i];	//将非数字字符写入目的字符串
			nStrLen++;					
			memset(sTemp, 0, 5);			//将字符串临时变量以及其长度变量清空		
			nTempLen = 0;				
		}
	}
	//将临时变量最后保存的数字写入目的字符串
	if(sTemp[0]!='\0')		//防止非数字结尾的情况
	{
		itoa(atoi(sTemp)+nLsNum, sTemp, 10);
		if(nStrDestLen>0 && nStrLen<nStrDestLen)
		{
			if(nStrDestLen<(nStrLen+(int)strlen(sTemp)))
				strncat(sDest, sTemp, nStrDestLen-nStrLen);
			else
				strcat(sDest, sTemp);
		}
		nStrLen += strlen(sTemp);
	}
	return nStrLen;
}
#if defined(__TSA_)||defined(__LDS_)
void GetSysPath(char* App_Path,const char *src_path/*=NULL*/,char *file_name/*=NULL*/,char *extension/*=NULL*/);//获得系统运行路径，App_Path应在函数外开辟好内存
CString MakeWeatherRegionSetString()
{
	CWireLoadDatabase db;
	char file_path[MAX_PATH];
	GetSysPath(file_path,NULL,NULL,NULL);
	strcat(file_path,"wireload.d");
	if(!db.OpenDbFile(file_path))
		return "";
	CString regionStr;
	WEATHER_REGION wr;
	BOOL finded=db.QueryFirstWeatherRegion(&wr);
	while(finded)
	{
		regionStr+=wr.weatherCode;
		finded=db.QueryNextWeatherRegion(&wr);
		if(finded)
			regionStr+='|';
	}
	return regionStr;
}
CString MakeWireTypeSetString()
{
	CWireLoadDatabase db;
	char file_path[MAX_PATH];
	GetSysPath(file_path,NULL,NULL,NULL);
	strcat(file_path,"wireload.d");
	if(!db.OpenDbFile(file_path))
		return "";
	CString strSet;
	char wireType[50]="";
	BOOL finded=db.QueryFirstWireType(wireType);
	while(finded)
	{
		strSet+=wireType;
		finded=db.QueryNextWireType(wireType);
		if(finded)
			strSet+='|';
	}
	return strSet;
}
CString MakeWireSetString(char *wireType)
{
	CWireLoadDatabase db;
	char file_path[MAX_PATH];
	GetSysPath(file_path,NULL,NULL,NULL);
	strcat(file_path,"wireload.d");
	if(!db.OpenDbFile(file_path))
		return "";
	CString strSet;
	DIANXIAN_STRU wire;
	BOOL finded=db.QueryFirstWire(wireType,&wire);
	while(finded)
	{
		strSet+=wire.specWire;
		finded=db.QueryNextWire(&wire);
		if(finded)
			strSet+='|';
	}
	return strSet;
}
void MakeWireTypeSetString(CComboBox *pCMB)
{
	CWireLoadDatabase db;
	char file_path[MAX_PATH];
	GetSysPath(file_path,NULL,NULL,NULL);
	strcat(file_path,"wireload.d");
	if(!db.OpenDbFile(file_path))
		return;
	pCMB->ResetContent();
	char wireType[50]="";
	BOOL finded=db.QueryFirstWireType(wireType);
	while(finded)
	{
		pCMB->AddString(wireType);
		finded=db.QueryNextWireType(wireType);
	}
}
void MakeWireSetString(char *wireType,CComboBox *pCMB)
{
	CWireLoadDatabase db;
	char file_path[MAX_PATH];
	GetSysPath(file_path,NULL,NULL,NULL);
	strcat(file_path,"wireload.d");
	if(!db.OpenDbFile(file_path))
		return;
	pCMB->ResetContent();
	DIANXIAN_STRU wire;
	BOOL finded=db.QueryFirstWire(wireType,&wire);
	while(finded)
	{
		pCMB->AddString(wire.specWire);
		finded=db.QueryNextWire(&wire);
	}
}
#endif

CString MakeTubeSetString(BOOL bOnlyTubeD/*=FALSE*/)
{
	CString sizeStr;
	char text[MAX_TEMP_CHAR_100+1]="";
	CHashStrTable<long> tubeDiaHash;
	tubeDiaHash.CreateHashTable(tubeguige_N);
	for(int i=0;i<tubeguige_N;i++)
	{
		_snprintf(text,MAX_TEMP_CHAR_100,"%f",tubeguige_table[i].D);
		SimplifiedNumString(text);
		if(bOnlyTubeD)
		{	//避免添加重复直径
			if(tubeDiaHash.ContainsKey(text))
				continue;
			tubeDiaHash.SetValueAt(text,1);
			sizeStr+=text;
		}
		else
		{
			sizeStr+=text;
			_snprintf(text,MAX_TEMP_CHAR_100,"X%f",tubeguige_table[i].thick);
			SimplifiedNumString(text);
			sizeStr+=text;
		}
		if(i<tubeguige_N-1)
			sizeStr+='|';
	}
	return sizeStr;
}
CString MakeFlatSetString()
{
	CString sizeStr;
	char text[MAX_TEMP_CHAR_100+1]="";
	for(int i=0;i<bianguige_N;i++)
	{
		_snprintf(text,MAX_TEMP_CHAR_100,"%.f",bianguige_table[i].wide);
		SimplifiedNumString(text);
		sizeStr+=text;
		_snprintf(text,MAX_TEMP_CHAR_100,"X%.f",bianguige_table[i].thick);
		SimplifiedNumString(text);
		sizeStr+=text;
		if(i<bianguige_N-1)
			sizeStr+='|';
	}
	return sizeStr;
}
CString MakeArcLiftSetString()
{
#ifdef AFX_TARG_ENU_ENGLISH
	CString arcLiftStr="Without Lift Arc";
#else 
	CString arcLiftStr="无预拱";
#endif
	for(CLDSArcLift *pLift=Ta.ArcLift.GetFirst();pLift;pLift=Ta.ArcLift.GetNext())
	{
		arcLiftStr+="|";
		CString sLiftName;
		sLiftName.Format("%s(%d)",pLift->name,pLift->iNo);
		arcLiftStr+=sLiftName;
	}
	return arcLiftStr;
}
CString MakeAngleSetString(char cType/*='L'*/,bool bAddType/*=false*/)
{
	CString sizeStr;
	char text[MAX_TEMP_CHAR_100+1]="";
	for(int i=0;i<jgguige_N;i++)
	{
		if(jgguige_table[i].cType!=cType)
			continue;
		if(bAddType)
			_snprintf(text,MAX_TEMP_CHAR_100,"L%gx%g",jgguige_table[i].wing_wide,jgguige_table[i].wing_thick);
		else
			_snprintf(text,MAX_TEMP_CHAR_100,"%gx%g",jgguige_table[i].wing_wide,jgguige_table[i].wing_thick);
		sizeStr+=text;
		if(i<jgguige_N-1)
			sizeStr+='|';
	}
	return sizeStr;
}
CString MakeSlotSetString()
{
	CString sizeStr;
	char text[MAX_TEMP_CHAR_100+1]="";
	for(int i=0;i<slotguige_N;i++)
	{
		_snprintf(text,MAX_TEMP_CHAR_100,"%s",slotguige_table[i].guige);
		sizeStr+=text;
		if(i<slotguige_N-1)
			sizeStr+='|';
	}
	return sizeStr;
}
CString MakeFittingSetString(BYTE ciFittingType)
{
	CString sizeStr;
	if(ciFittingType==CLDSFitting::TYPE_EB)
	{
		IFittingLibrary<FITTING_EB>* pxarrEBLib=FittingLibrary<FITTING_EB>();
		for(int i=0;i<pxarrEBLib->Count;i++)
		{
			if(i>0)
				sizeStr+='|';
			sizeStr+=pxarrEBLib->pxSizeTbl[i].szSizeCode;
		}
	}
	else if(ciFittingType==CLDSFitting::TYPE_GD)
	{
		IFittingLibrary<FITTING_GD>* pxarrGDLib=FittingLibrary<FITTING_GD>();
		for(int i=0;i<pxarrGDLib->Count;i++)
		{
			if(i>0)
				sizeStr+='|';
			sizeStr+=pxarrGDLib->pxSizeTbl[i].szSizeCode;
		}
	}
#if defined(__LDS_CONTEXT_)||defined(__LDS_GROUP_CONTEXT_)
	else if(ciFittingType==CLDSFitting::TYPE_UR)
	{
		IFittingLibrary<FITTING_UR>* pxarrURLib=FittingLibrary<FITTING_UR>();
		for(int i=0;i<pxarrURLib->Count;i++)
		{
			if(i>0)
				sizeStr+='|';
			sizeStr+=pxarrURLib->pxSizeTbl[i].szSizeCode;
		}
	}
#endif
	return sizeStr;
}
#ifdef __H_SHAPE_STEEL_
CString MakeHShapeSetString(BYTE bHShapeType/*=0*/)
{
	CString sizeStr;
	char text[MAX_TEMP_CHAR_100+1]="";
	if(bHShapeType==0)
	{
		for(int i=0;i<hw_guige_N;i++)
		{
			_snprintf(text,MAX_TEMP_CHAR_100,"%gX%g",hw_guige_table[i].h,hw_guige_table[i].b);
			sizeStr+=text;
			if(i<hw_guige_N-1)
				sizeStr+='|';
		}
	}
	else if(bHShapeType==1)
	{
		for(int i=0;i<hm_guige_N;i++)
		{
			_snprintf(text,MAX_TEMP_CHAR_100,"%gX%g",hm_guige_table[i].h,hm_guige_table[i].b);
			sizeStr+=text;
			if(i<hm_guige_N-1)
				sizeStr+='|';
		}
	}
	else if(bHShapeType==2)
	{
		for(int i=0;i<hn_guige_N;i++)
		{
			_snprintf(text,MAX_TEMP_CHAR_100,"%gX%g",hn_guige_table[i].h,hn_guige_table[i].b);
			sizeStr+=text;
			if(i<hn_guige_N-1)
				sizeStr+='|';
		}
	}
	else if(bHShapeType==3)
	{
		for(int i=0;i<hp_guige_N;i++)
		{
			_snprintf(text,MAX_TEMP_CHAR_100,"%gX%g",hp_guige_table[i].h,hp_guige_table[i].b);
			sizeStr+=text;
			if(i<hp_guige_N-1)
				sizeStr+='|';
		}
	}
	return sizeStr;
}
void ParseHShapeSpec(const char* sSpec,int iHShateType,SIZESPEC& size)
{
	//1.把字符串进行拆分
	sscanf(sSpec,"%lfX%lf",&size.height,&size.wide);
	//2.遍历table,根据h，w进行比较，找到匹配的一条记录，
	if(iHShateType==0)
	{
		for(int i=0;i<hw_guige_N;++i)
		{
			if(hw_guige_table[i].h==size.height&&hw_guige_table[i].b==size.wide)
			{
				size.thick=hw_guige_table[i].t1;
				size.thick2=hw_guige_table[i].t2;
				return;
			}
		}
	}
	else if(iHShateType==1)
	{
		for(int i=0;i<hm_guige_N;++i)
		{
			if(hm_guige_table[i].h==size.height&&hm_guige_table[i].b==size.wide)
			{
				size.thick=hm_guige_table[i].t1;
				size.thick2=hm_guige_table[i].t2;
				return;
			}
		}
	}
	else if(iHShateType==2)
	{
		for(int i=0;i<hn_guige_N;++i)
		{
			if(hn_guige_table[i].h==size.height&&hn_guige_table[i].b==size.wide)
			{
				size.thick=hn_guige_table[i].t1;
				size.thick2=hn_guige_table[i].t2;
				return;
			}
		}
	}
	else if(iHShateType==3)
	{
		for(int i=0;i<hp_guige_N;++i)
		{
			if(hp_guige_table[i].h==size.height&&hp_guige_table[i].b==size.wide)
			{
				size.thick=hp_guige_table[i].t1;
				size.thick2=hp_guige_table[i].t2;
				return;
			}	
		}
	}
}
#endif
CString MakeBoltFamilySetString()
{
	CString sizeStr;
	for(CLsFamily* pFamily=CLsLibrary::GetFirstFamily();pFamily;pFamily=CLsLibrary::GetNextFamily())
	{
		if(sizeStr.GetLength()>0)
			sizeStr+='|';
		sizeStr+=pFamily->name;
	}
	return sizeStr;
}
CString MakeBoltSetString(long hLsFamily)
{
	CLsFamily *pLsFamily=CLsLibrary::FromHandle(hLsFamily);
	if(pLsFamily==NULL)
		pLsFamily=CLsLibrary::DefaultFamily();
	if(pLsFamily==NULL)
		return "";
	CString sizeStr;
	for(int i=0;i<pLsFamily->GetCount();i++)
	{
		sizeStr+=pLsFamily->RecordAt(i)->guige;
		if(i<pLsFamily->GetCount()-1)
			sizeStr+='|';
	}
	return sizeStr;
}
CString MakeMaterialSetString()
{
	CString matStr;
	for(int i=0;i<CSteelMatLibrary::GetCount();i++)
	{
		STEELMAT mat=CSteelMatLibrary::RecordAt(i);
		matStr+=mat.mark;
		matStr=matStr+"("+mat.cBriefMark+')';
		if(i<CSteelMatLibrary::GetCount()-1)
			matStr+='|';
	}
	return matStr;
}
CString MakeMaterialMarkSetString()
{
	CString matStr;
	for(int i=0;i<CSteelMatLibrary::GetCount();i++)
	{
		matStr+=CSteelMatLibrary::RecordAt(i).mark;
		if(i<CSteelMatLibrary::GetCount()-1)
			matStr+='|';
	}
	return matStr;
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
//计算角钢上指定节点处开合角起始与终止位置(角钢相对坐标值)
static BOOL CalKaiHeStartEndPos(CLDSLineAngle *pRayJg,const f3dLine &lineX,
								const f3dLine &lineY,const f3dLine &lineBer,
								f3dLine &rayLineWing,
								BOOL bInXWing,BOOL bStartJoint,const UCS_STRU &ucs,
								f3dPoint &startPos,f3dPoint &endPos)
{
	BOOL bNeedKaiHe=FALSE;
	f3dLine rayLineBer,line_wing;
	f3dPoint vec,inters;
	Sub_Pnt(vec,pRayJg->End(),pRayJg->Start());
	normalize(vec);
	if(bStartJoint)
	{
		Sub_Pnt(rayLineWing.startPt,rayLineWing.startPt,vec*pRayJg->startOdd());
		Sub_Pnt(rayLineBer.startPt,pRayJg->Start(),vec*pRayJg->startOdd());
		rayLineBer.endPt = pRayJg->End();
	}
	else
	{
		rayLineBer.startPt = pRayJg->Start();
		Add_Pnt(rayLineWing.endPt,rayLineWing.endPt,vec*pRayJg->endOdd());
		Add_Pnt(rayLineBer.endPt,pRayJg->End(),vec*pRayJg->endOdd());
	}
	//转换为基准角钢相对坐标系下的直线
	coord_trans(rayLineWing.startPt,ucs,FALSE);
	coord_trans(rayLineWing.endPt,ucs,FALSE);
	coord_trans(rayLineBer.startPt,ucs,FALSE);
	coord_trans(rayLineBer.endPt,ucs,FALSE);
	f3dPoint current_end=startPos;
	if(bStartJoint)
		current_end=rayLineWing.startPt;
	else
		current_end=rayLineWing.endPt;
	if(bInXWing)
	{
		rayLineWing.startPt.y = rayLineWing.endPt.y = 0;
		rayLineBer.startPt.y = rayLineBer.endPt.y = 0;
		line_wing = lineX;
		if(current_end.x>0&&current_end.x<lineX.startPt.x)
		{
			if(startPos.z>current_end.z)
				startPos.z=current_end.z;
			if(endPos.z<current_end.z)
				endPos.z=current_end.z;
		}
	}
	else
	{
		rayLineWing.startPt.x = rayLineWing.endPt.x = 0;
		rayLineBer.startPt.x = rayLineBer.endPt.x = 0;
		line_wing = lineY;
		if(current_end.y>0&&current_end.y<lineY.startPt.y)
		{
			if(startPos.z>current_end.z)
				startPos.z=current_end.z;
			if(endPos.z<current_end.z)
				endPos.z=current_end.z;
		}
	}

	if(Int3dll(rayLineWing,line_wing,inters)>0)
	{
		if(startPos.z>inters.z)
			startPos = inters;
		if(endPos.z<inters.z)
			endPos = inters;
		bNeedKaiHe=TRUE;
	}
	if(Int3dll(rayLineWing,lineBer,inters)>0)
	{
		if(startPos.z>inters.z)
			startPos = inters;
		if(endPos.z<inters.z)
			endPos = inters;
		bNeedKaiHe=TRUE;
	}
	if(Int3dll(rayLineBer,line_wing,inters)>0)
	{
		if(startPos.z>inters.z)
			startPos = inters;
		if(endPos.z<inters.z)
			endPos = inters;
		bNeedKaiHe=TRUE;
	}
	if(Int3dll(rayLineBer,lineBer,inters)>0)
	{
		if(startPos.z>inters.z)
			startPos = inters;
		if(endPos.z<inters.z)
			endPos = inters;
		bNeedKaiHe=TRUE;
	}
	return bNeedKaiHe;
}
static BOOL CalKaiHeStartEndPosByRayJg( CLDSLineAngle *pJg,CLDSNode *pNode, 
									    const f3dLine &lineX,const f3dLine &lineY,
									    const f3dLine &lineBer,f3dPoint &startPos,
										f3dPoint &endPos,UCS_STRU &angleUcs)
{
	CLDSLineAngle *pRayJg;
	LINEPARTSET jgset;
	f3dPoint start_pos,end_pos;
	f3dLine rayLineWing;
	double ddmx_rx,ddmx_ry,ddmy_rx,ddmy_ry;
	start_pos = startPos;
	end_pos = endPos;
	BOOL bNeedKaiHeByRayJg=FALSE;
	BOOL bKaiHeWingX=TRUE;
	int kaihe_base_wing=pJg->GetKaiHeWingX0Y1();
	if(kaihe_base_wing==-1)	//人工指定不需要开合角
		return FALSE;
	if(kaihe_base_wing==1)
		bKaiHeWingX=FALSE;
	double length=pJg->GetLength();
	for(CLDSPart *pPart=Ta.Parts.GetFirst(CLS_LINEANGLE);pPart;pPart=Ta.Parts.GetNext(CLS_LINEANGLE))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
		if((pLinePart->pStart==pNode&&!pLinePart->bHuoQuStart)||(pLinePart->pEnd==pNode&&!pLinePart->bHuoQuEnd))
			jgset.append(pLinePart);
	}
	for(pRayJg=(CLDSLineAngle*)jgset.GetFirst();pRayJg!=NULL;pRayJg=(CLDSLineAngle*)jgset.GetNext())
	{
		double wide,thick;
		f3dPoint vec;
		if(pRayJg==pJg)
			continue;
		Sub_Pnt(vec,pRayJg->End(),pRayJg->Start());
		normalize(vec);
		if(fabs(vec*angleUcs.axis_z)>EPS_COS2)
			continue;	//共线
		wide = pRayJg->GetWidth();
		thick = pRayJg->GetThick();
		ddmx_rx = dot_prod(pJg->get_norm_x_wing(),pRayJg->get_norm_x_wing());
		ddmx_ry = dot_prod(pJg->get_norm_x_wing(),pRayJg->get_norm_y_wing());
		ddmy_rx = dot_prod(pJg->get_norm_y_wing(),pRayJg->get_norm_x_wing());
		ddmy_ry = dot_prod(pJg->get_norm_y_wing(),pRayJg->get_norm_y_wing());
		BOOL bNeedKaiHe = FALSE;
		if(IsMaxInAll(ddmx_rx,ddmx_ry,ddmy_rx,ddmy_ry)&&fabs(ddmx_rx)>EPS_COS2&&bKaiHeWingX)
		{	//X肢开合角
			Sub_Pnt(rayLineWing.startPt,pRayJg->Start(),pRayJg->get_norm_y_wing()*wide);
			Sub_Pnt(rayLineWing.endPt,pRayJg->End(),pRayJg->get_norm_y_wing()*wide);
			if(pRayJg->pStart==pNode)
				bNeedKaiHe=CalKaiHeStartEndPos(pRayJg,lineX,lineY,lineBer,rayLineWing,TRUE,TRUE,angleUcs,start_pos,end_pos);
			else
				bNeedKaiHe=CalKaiHeStartEndPos(pRayJg,lineX,lineY,lineBer,rayLineWing,TRUE,FALSE,angleUcs,start_pos,end_pos);
		}
		else if(IsMaxInAll(ddmx_ry,ddmx_rx,ddmy_rx,ddmy_ry)&&fabs(ddmx_ry)>EPS_COS2&&bKaiHeWingX)
		{	//X肢开合角
			Sub_Pnt(rayLineWing.startPt,pRayJg->Start(),pRayJg->get_norm_x_wing()*wide);
			Sub_Pnt(rayLineWing.endPt,pRayJg->End(),pRayJg->get_norm_x_wing()*wide);
			if(pRayJg->pStart==pNode)
				bNeedKaiHe=CalKaiHeStartEndPos(pRayJg,lineX,lineY,lineBer,rayLineWing,TRUE,TRUE,angleUcs,start_pos,end_pos);
			else
				bNeedKaiHe=CalKaiHeStartEndPos(pRayJg,lineX,lineY,lineBer,rayLineWing,TRUE,FALSE,angleUcs,start_pos,end_pos);
		}
		else if(IsMaxInAll(ddmy_rx,ddmx_rx,ddmx_ry,ddmy_ry)&&fabs(ddmy_rx)>EPS_COS2&&!bKaiHeWingX)
		{	//Y肢开合角
			Sub_Pnt(rayLineWing.startPt,pRayJg->Start(),pRayJg->get_norm_y_wing()*wide);
			Sub_Pnt(rayLineWing.endPt,pRayJg->End(),pRayJg->get_norm_y_wing()*wide);
			if(pRayJg->pStart==pNode)
				bNeedKaiHe=CalKaiHeStartEndPos(pRayJg,lineX,lineY,lineBer,rayLineWing,FALSE,TRUE,angleUcs,start_pos,end_pos);
			else
				bNeedKaiHe=CalKaiHeStartEndPos(pRayJg,lineX,lineY,lineBer,rayLineWing,FALSE,FALSE,angleUcs,start_pos,end_pos);
		}
		else if(IsMaxInAll(ddmy_ry,ddmy_rx,ddmx_rx,ddmx_ry)&&fabs(ddmy_ry)>EPS_COS2&&!bKaiHeWingX)
		{	//Y肢开合角
			Sub_Pnt(rayLineWing.startPt,pRayJg->Start(),pRayJg->get_norm_x_wing()*wide);
			Sub_Pnt(rayLineWing.endPt,pRayJg->End(),pRayJg->get_norm_x_wing()*wide);
			if(pRayJg->pStart==pNode)
				bNeedKaiHe=CalKaiHeStartEndPos(pRayJg,lineX,lineY,lineBer,rayLineWing,FALSE,TRUE,angleUcs,start_pos,end_pos);
			else
				bNeedKaiHe=CalKaiHeStartEndPos(pRayJg,lineX,lineY,lineBer,rayLineWing,FALSE,FALSE,angleUcs,start_pos,end_pos);
		}
		else
			continue;
		//因为角钢是斜的所以应起止各应再扩大肢宽的0.8(经验常数)
		if (bNeedKaiHe)
		{
			startPos.z = __min(startPos.z, start_pos.z);
			endPos.z = __max(endPos.z, end_pos.z);
		}
		if (bNeedKaiHeByRayJg == FALSE)
			bNeedKaiHeByRayJg = bNeedKaiHe;
	}
	if(bNeedKaiHeByRayJg)
	{	//10mm的开合角角钢间隙
		startPos.z-=10;
		endPos.z+=10;
	}
	return bNeedKaiHeByRayJg;
}
//计算角钢上指定节点处开合角起始与终止位置(角钢相对坐标值)Z坐标分量
BOOL CalKaiHeStartEndPos(CLDSLineAngle *pJg,CLDSNode *pNode,f3dPoint &startPos,f3dPoint &endPos,UCS_STRU &angleUcs,BOOL bIntelligent/*=FALSE*/,BOOL bUcsIncStartOdd/*=FALSE*/)
{
	f3dPoint inters;
	CTmaPtrList<CLDSLineAngle,CLDSLineAngle*> jgset,jgsetX,jgsetY;
	f3dLine lineX,lineY,lineBer,rayLineWing,rayLineBer,edgeline;
	double len,wing_wide,wing_thick;
	BOOL bKaiHe=FALSE;
	BOOL bKaiHeWingX=TRUE;
	int kaihe_base_wing=pJg->GetKaiHeWingX0Y1();
	if(kaihe_base_wing==-1)	//人工指定不需要开合角
		return FALSE;
	if(kaihe_base_wing==1)
		bKaiHeWingX=FALSE;
	len = pJg->GetLength();
	wing_wide = pJg->GetWidth();
	wing_thick = pJg->GetThick();
	//基准角钢上的根棱线(角钢相对坐标系下)
	double TOLERANCE=10;
	if(bUcsIncStartOdd)
	{	//lineX/Y/Ber用于与钢板轮廓边求交时,钢板轮廓边的坐标是在angleUcs下的,必须统一,否则对于存在正负头或制弯角钢时会漏判 wjh-2017.8.27
		f3dPoint xAcualStart=pJg->Start()-angleUcs.axis_z*pJg->startOdd();
		f3dPoint xAcualEnd=pJg->End()+angleUcs.axis_z*pJg->endOdd();
		double xAcualStartZ=(xAcualStart-angleUcs.origin)*angleUcs.axis_z-TOLERANCE;
		double xAcualEndZ=(xAcualEnd-angleUcs.origin)*angleUcs.axis_z+TOLERANCE;
		lineBer.startPt.Set(0,0,xAcualStartZ);
		lineBer.endPt.Set(0,0,xAcualEndZ);
		lineX.startPt.Set(wing_wide,0,xAcualStartZ);
		lineX.endPt.Set(wing_wide,0,xAcualEndZ);
		lineY.startPt.Set(0,wing_wide,xAcualStartZ);
		lineY.endPt.Set(0,wing_wide,xAcualEndZ);
	}
	else
	{
		lineBer.startPt.Set(0,0, -pJg->startOdd()-TOLERANCE);
		lineBer.endPt.Set(0,0,len-pJg->startOdd()+TOLERANCE);
		lineX.startPt.Set(wing_wide,0, -pJg->startOdd()-TOLERANCE);
		lineX.endPt.Set(wing_wide,0,len-pJg->startOdd()+TOLERANCE);
		lineY.startPt.Set(0,wing_wide, -pJg->startOdd()-TOLERANCE);
		lineY.endPt.Set(0,wing_wide,len-pJg->startOdd()+TOLERANCE);
	}

	Ta.Parts.push_stack();
	PARTSET plateSet;
	BYTE wing_x0_y1_all2=2;
	if(!bIntelligent&&bKaiHeWingX)
		wing_x0_y1_all2=0;
	else if(!bIntelligent&&!bKaiHeWingX)
		wing_x0_y1_all2=1;
	GetPlateSetConnectedInAngle(pJg,plateSet,wing_x0_y1_all2,pNode);
	for(CLDSPlate *pPlate=(CLDSPlate*)plateSet.GetFirst();pPlate;pPlate=(CLDSPlate*)plateSet.GetNext())
	{
		int n=pPlate->vertex_list.GetNodeNum();
		if(n>2)	//有板
		{
			for(int j=0;j<n;j++)
			{
				edgeline.startPt=pPlate->GetRealVertex(pPlate->vertex_list[j].vertex);
				edgeline.endPt=pPlate->GetRealVertex(pPlate->vertex_list[(j+1)%n].vertex);
				coord_trans(edgeline.startPt,pPlate->ucs,TRUE);
				coord_trans(edgeline.endPt,pPlate->ucs,TRUE);
				coord_trans(edgeline.startPt,angleUcs,FALSE);
				coord_trans(edgeline.endPt,angleUcs,FALSE);
				if(bKaiHeWingX)
				{
					edgeline.startPt.y=edgeline.endPt.y=0;
					if(startPos.z>edgeline.startPt.z)
						startPos.z=edgeline.startPt.z;
					if(endPos.z<edgeline.startPt.z)
						endPos.z=edgeline.startPt.z;
					if(Int3dll(edgeline,lineX,inters)>0)
					{
						if(startPos.z>inters.z)
							startPos = inters;
						if(endPos.z<inters.z)
							endPos = inters;
						bKaiHe = TRUE;
					}
					if(Int3dll(edgeline,lineBer,inters)>0)
					{
						if(startPos.z>inters.z)
							startPos = inters;
						if(endPos.z<inters.z)
							endPos = inters;
						bKaiHe = TRUE;
					}
				}
				else
				{
					edgeline.startPt.x=edgeline.endPt.x=0;
					if(startPos.z>edgeline.startPt.z)
						startPos.z=edgeline.startPt.z;
					if(endPos.z<edgeline.startPt.z)
						endPos.z=edgeline.startPt.z;
					if(Int3dll(edgeline,lineY,inters)>0)
					{
						if(startPos.z>inters.z)
							startPos = inters;
						if(endPos.z<inters.z)
							endPos = inters;
						bKaiHe = TRUE;
					}
					if(Int3dll(edgeline,lineBer,inters)>0)
					{
						if(startPos.z>inters.z)
							startPos = inters;
						if(endPos.z<inters.z)
							endPos = inters;
						bKaiHe = TRUE;
					}
				}
			}
		}
	}
	Ta.Parts.pop_stack();
	//退化为角钢直接连接的板连接设计
	if(bKaiHe)
	{	//10mm的开合角角钢间隙
		startPos.z-=10;
		endPos.z+=10;
	}
	else
		bKaiHe=CalKaiHeStartEndPosByRayJg(pJg,pNode,lineX,lineY,lineBer,startPos,endPos,angleUcs);
	return bKaiHe;
}
//判断pJg是否有一肢的法线方向与norm大致方向相同（方向反向认为不同）
BOOL IsNormSameToPlank(CLDSLineAngle *pJg, f3dPoint norm)
{
	BOOL bNormSameToPlank;
	f3dPoint normXwing,normYwing;
	normXwing = pJg->get_norm_x_wing();
	normYwing = pJg->get_norm_y_wing();
	normalize(normXwing);
	normalize(normYwing);
	normalize(norm);
	if(fabs(dot_prod(normXwing,norm))>0.707)
	{
		if(dot_prod(normXwing,norm)>0)
			bNormSameToPlank = TRUE;
		else
			bNormSameToPlank = FALSE;
	}
	else if(fabs(dot_prod(normYwing,norm))>0.707)
	{
		if(dot_prod(normYwing,norm)>0)
			bNormSameToPlank = TRUE;
		else
			bNormSameToPlank = FALSE;
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		throw "The angle's leg direction and specified direction are not in conformity!";
#else 
		throw "角钢肢方向与指定方向不符,数据错误!";
#endif
	return bNormSameToPlank;
}
BOOL IsInsideJg(CLDSLineAngle *pJg, f3dPoint norm,int *x_wing0_y_wing1)
{
	double ddx,ddy;
	ddx = pJg->get_norm_x_wing()*norm;
	ddy = pJg->get_norm_y_wing()*norm;
	if(fabs(ddx)>fabs(ddy))
	{
		if(x_wing0_y_wing1)
			*x_wing0_y_wing1 = 0;
		if(ddx>0)	//里铁
			return TRUE;
		else
			return FALSE;
	}
	else 
	{
		if(x_wing0_y_wing1)
			*x_wing0_y_wing1 = 1;
		if(ddy>0)	//里铁
			return TRUE;
		else
			return FALSE;
	}
}
BOOL IsInsideJg(CLDSLineAngle *pJg, f3dPoint norm,BYTE *x_wing0_y_wing1)
{
	int iWingStyle=0;
	if(!IsInsideJg(pJg,norm,&iWingStyle))
		return FALSE;
	if(x_wing0_y_wing1)
		*x_wing0_y_wing1=(BYTE)iWingStyle;
	return TRUE;
}
//获得两角钢的共面工作法线（主角钢工作肢法线）
void GetWorkNorm(CLDSLineAngle *pMainJg, CLDSLineAngle* pViceJg, f3dPoint *norm)
{
	f3dPoint norm1x,norm1y,norm2x,norm2y,vec1,vec2;
	double dd1x_2x,dd1x_2y,dd1y_2x,dd1y_2y;
	if(norm==NULL)
		return;
	vec1=pMainJg->End()-pMainJg->Start();
	vec2=pViceJg->End()-pViceJg->Start();
	normalize(vec1);
	normalize(vec2);
	if(fabs(vec1*vec2)>EPS_COS2)	//两角钢接近共线，此时共面法线应两角钢求叉积
	{
		vec2=pViceJg->Start()-pMainJg->Start();
		normalize(vec2);
		f3dPoint face_norm=vec1^vec2;
		normalize(face_norm);
		if(fabs(face_norm*pMainJg->get_norm_x_wing())>fabs(face_norm*pMainJg->get_norm_y_wing()))
			*norm=pMainJg->get_norm_x_wing();
		else
			*norm=pMainJg->get_norm_y_wing();
	}
	else
	{
		norm1x = pMainJg->get_norm_x_wing();
		norm1y = pMainJg->get_norm_y_wing();
		norm2x = pViceJg->get_norm_x_wing();
		norm2y = pViceJg->get_norm_y_wing();
		normalize(norm1x);
		normalize(norm1y);
		normalize(norm2x);
		normalize(norm2y);
		dd1x_2x = fabs(dot_prod(norm1x,norm2x));
		dd1x_2y = fabs(dot_prod(norm1x,norm2y));
		dd1y_2x = fabs(dot_prod(norm1y,norm2x));
		dd1y_2y = fabs(dot_prod(norm1y,norm2y));
		//工程误差范围之外,则找最为共面的肢为工作面肢
		if((dd1x_2x>dd1y_2x&&dd1x_2x>dd1y_2y)
			||(dd1x_2y>dd1y_2x&&dd1x_2y>dd1y_2y))
			*norm = pMainJg->get_norm_x_wing();
		else
			*norm = pMainJg->get_norm_y_wing();
	}
}
void CalAngleLayoutStyle(CLDSLineAngle *pRayAngle, CLDSLinePart *pDatumPart,int *ray_wing_x0_y1,int *datum_wing_x0_y1)
{
	CSuperSmartPtr<CLDSLinePart>pDatumLinePart=pDatumPart;
	if(pDatumLinePart->IsAngle())
	{
		double ddxx=fabs(pRayAngle->get_norm_x_wing()*pDatumLinePart.LineAnglePointer()->get_norm_x_wing());
		double ddxy=fabs(pRayAngle->get_norm_x_wing()*pDatumLinePart.LineAnglePointer()->get_norm_y_wing());
		double ddyx=fabs(pRayAngle->get_norm_y_wing()*pDatumLinePart.LineAnglePointer()->get_norm_x_wing());
		double ddyy=fabs(pRayAngle->get_norm_y_wing()*pDatumLinePart.LineAnglePointer()->get_norm_y_wing());
		if(IsMaxInAll(ddxx,ddxy,ddyx,ddyy))
		{
			*ray_wing_x0_y1=0;
			*datum_wing_x0_y1=0;
		}
		else if(IsMaxInAll(ddxy,ddxx,ddyx,ddyy))
		{
			*ray_wing_x0_y1=0;
			*datum_wing_x0_y1=1;
		}
		else if(IsMaxInAll(ddyx,ddxx,ddxy,ddyy))
		{
			*ray_wing_x0_y1=1;
			*datum_wing_x0_y1=0;
		}
		else// if(IsMaxInAll(ddyy,ddxx,ddxy,ddyx))
		{
			*ray_wing_x0_y1=1;
			*datum_wing_x0_y1=1;
		}
	}
	else
	{
		f3dPoint datum_vec=pDatumLinePart->End()-pDatumLinePart->Start();
		f3dPoint ray_vec=pRayAngle->End()-pRayAngle->Start();
		f3dPoint worknorm=datum_vec^ray_vec;
		normalize(worknorm);
		double ddx=worknorm*pRayAngle->get_norm_x_wing();
		double ddy=worknorm*pRayAngle->get_norm_y_wing();
		if(fabs(ddx)>fabs(ddy))
			*ray_wing_x0_y1=0;
		else
			*ray_wing_x0_y1=1;
	}
}

//计算十字插板立板法线
BOOL CalVerticalPlateNorm(CLDSPlate *pPlate,CLDSPart *pRefPart)
{
	//焊接父构件为十字插板
	CLDSParamPlate *pFatherPlate=(CLDSParamPlate*)console.FromPartHandle(pPlate->m_hPartWeldParent,CLS_PARAMPLATE);
	if(pFatherPlate==NULL)
		return FALSE;
	if(pFatherPlate->m_iParamType!=TYPE_XEND)
		return FALSE;
	//定位构件为十字插板
	if(pPlate->designInfo.origin.des_para.hPart!=pFatherPlate->handle)
		return FALSE;
	if(pRefPart==NULL)
		return FALSE;
	if(pRefPart->IsLinePart())
	{
		CLDSLinePart *pRefLinePart=(CLDSLinePart*)pRefPart;
		CLDSLineTube *pDatumTube=(CLDSLineTube*)console.FromPartHandle(pFatherPlate->m_hPartWeldParent,CLS_LINETUBE);
		if(pDatumTube==NULL)
			return FALSE;
		f3dPoint datum_line_vec=pDatumTube->End()-pDatumTube->Start();
		normalize(datum_line_vec);
		f3dPoint line_vec=pRefLinePart->End()-pRefLinePart->Start();
		normalize(line_vec);
		if(fabs(datum_line_vec*line_vec)>EPS_COS2)
			return FALSE;
		pPlate->designInfo.norm.norm_style=2;	//杆件交叉面法线
		pPlate->designInfo.norm.hVicePart=pDatumTube->handle;
		pPlate->designInfo.norm.hCrossPart=pRefLinePart->handle;
		return TRUE;
	}
	else if(pRefPart->GetClassTypeId()==CLS_PLATE || pRefPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		pPlate->designInfo.norm.norm_style=4;	//基准构件上的相对方向
		pPlate->designInfo.norm.hVicePart=pRefPart->handle;
		pPlate->designInfo.norm.vector.Set(0,0,1);
		return TRUE;
	}
	return FALSE;
}
#endif
//根据始终端节点得到杆件有效的对称信息
MIRMSG GetPoleValidMirMsg(CLDSNode *pStartNode,CLDSNode *pEndNode,int *xy1_yz2_xz3)
{
	MIRMSG mirmsg;
	*xy1_yz2_xz3 = 0;//0不存在互斥对称 1表示XY对称互斥 2表示YZ对称互斥 3表示XZ对称互斥
	if(pStartNode&&pEndNode)
	{
		mirmsg.axis_flag=0;
		int i,flag_arr[3]={1,2,4};
		f3dPoint mir_point_start[3],mir_point_end[3];
		for(i=0;i<3;i++)
		{
			mir_point_start[i]=GetMirPos(pStartNode->Position(false),MIRMSG(i+1));
			mir_point_end[i]=GetMirPos(pEndNode->Position(false),MIRMSG(i+1));
			//首先应保证原杆件对称后不和本身重合
			if(!((DISTANCE(pEndNode->Position(false),mir_point_start[i])<EPS2&&DISTANCE(pStartNode->Position(false),mir_point_end[i])<EPS2)	
				||(DISTANCE(pStartNode->Position(false),mir_point_start[i])<EPS2&&DISTANCE(pEndNode->Position(false),mir_point_end[i])<EPS2)))
			{
				//如果杆件存在始终端节点，应查找是否有相应的对称节点
				if(pStartNode->GetMirNode(MIRMSG(i+1))&&pEndNode->GetMirNode(MIRMSG(i+1)))
					mirmsg.axis_flag |= flag_arr[i];	
			}
		}
		if((mirmsg.axis_flag&1)&&(mirmsg.axis_flag&2))
		{	//判断XY对称是否互斥
			if((DISTANCE(mir_point_start[0],mir_point_start[1])<EPS2 && DISTANCE(mir_point_end[0],mir_point_end[1])<EPS2)
				||(DISTANCE(mir_point_start[0],mir_point_end[1])<EPS2 && DISTANCE(mir_point_end[0],mir_point_start[1])<EPS2))
				*xy1_yz2_xz3 = 1;
		}
		if((mirmsg.axis_flag&1)&&(mirmsg.axis_flag&4))
		{	//判断XZ对称是否互斥
			if((DISTANCE(mir_point_start[0],mir_point_start[2])<EPS2 && DISTANCE(mir_point_end[0],mir_point_end[2])<EPS2)
				||(DISTANCE(mir_point_start[0],mir_point_end[2])<EPS2 && DISTANCE(mir_point_end[0],mir_point_start[2])<EPS2))
				*xy1_yz2_xz3 = 3;
		}
		if((mirmsg.axis_flag&2)&&(mirmsg.axis_flag&4))
		{	//判断YZ对称是否互斥
			if((DISTANCE(mir_point_start[1],mir_point_start[2])<EPS2 && DISTANCE(mir_point_end[1],mir_point_end[2])<EPS2)
				||(DISTANCE(mir_point_start[1],mir_point_end[2])<EPS2 && DISTANCE(mir_point_end[1],mir_point_start[2])<EPS2))
				*xy1_yz2_xz3 = 2;
		}
	}
	return mirmsg;
}

//根据起始端位置得到杆件有效的对称信息
MIRMSG GetPoleValidMirMsg(f3dPoint startPos,f3dPoint endPos,int *xy1_yz2_xz3)
{
	MIRMSG mirmsg;
	*xy1_yz2_xz3 = 0;//0不存在互斥对称 1表示XY对称互斥 2表示YZ对称互斥 3表示XZ对称互斥
	mirmsg.axis_flag=0;
	int i,flag_arr[3]={1,2,4};
	f3dPoint mir_point_start[3],mir_point_end[3];
	for(i=0;i<3;i++)
	{
		mir_point_start[i]=GetMirPos(startPos,MIRMSG(i+1));
		mir_point_end[i]=GetMirPos(endPos,MIRMSG(i+1));
		//首先应保证原杆件对称后不和本身重合
		if(!((DISTANCE(endPos,mir_point_start[i])<EPS2&&DISTANCE(startPos,mir_point_end[i])<EPS2)	
			||(DISTANCE(startPos,mir_point_start[i])<EPS2&&DISTANCE(endPos,mir_point_end[i])<EPS2)))
				mirmsg.axis_flag |= flag_arr[i];	
	}
	if((mirmsg.axis_flag&1)&&(mirmsg.axis_flag&2))
	{	//判断XY对称是否互斥
		if((DISTANCE(mir_point_start[0],mir_point_start[1])<EPS2 && DISTANCE(mir_point_end[0],mir_point_end[1])<EPS2)
			||(DISTANCE(mir_point_start[0],mir_point_end[1])<EPS2 && DISTANCE(mir_point_end[0],mir_point_start[1])<EPS2))
			*xy1_yz2_xz3 = 1;
	}
	if((mirmsg.axis_flag&1)&&(mirmsg.axis_flag&4))
	{	//判断XZ对称是否互斥
		if((DISTANCE(mir_point_start[0],mir_point_start[2])<EPS2 && DISTANCE(mir_point_end[0],mir_point_end[2])<EPS2)
			||(DISTANCE(mir_point_start[0],mir_point_end[2])<EPS2 && DISTANCE(mir_point_end[0],mir_point_start[2])<EPS2))
			*xy1_yz2_xz3 = 3;
	}
	if((mirmsg.axis_flag&2)&&(mirmsg.axis_flag&4))
	{	//判断YZ对称是否互斥
		if((DISTANCE(mir_point_start[1],mir_point_start[2])<EPS2 && DISTANCE(mir_point_end[1],mir_point_end[2])<EPS2)
			||(DISTANCE(mir_point_start[1],mir_point_end[2])<EPS2 && DISTANCE(mir_point_end[1],mir_point_start[2])<EPS2))
			*xy1_yz2_xz3 = 2;
	}
	return mirmsg;
}

//从剪切板中读取文本数据 单元格内容不能为空
BOOL ReadStrFromClipboard(TABLE_DATA &table_data)
{
	if(!IsClipboardFormatAvailable(CF_TEXT)) 
		return FALSE; 
	if(!OpenClipboard(NULL)) 
		return FALSE; 
	HGLOBAL hglb=GetClipboardData(CF_TEXT); 
	if(hglb != NULL) 
	{
		char *sTemp=(char*)GlobalLock(hglb); 
		if(sTemp==NULL) 
			return FALSE;
		CString sBuffer;
		sBuffer.Format("%s",sTemp);
		sBuffer.Replace('\r',' ');
		sBuffer.Replace('\t',' ');
		int nRowIndex=0;
		//按行拆分字符串
		while(nRowIndex<sBuffer.GetLength()&&nRowIndex!=-1)
		{
			int nColIndex=0;
			int nPrevRowIndex=nRowIndex;
			nRowIndex=sBuffer.Find("\n",nPrevRowIndex+1);
			if(nRowIndex>sBuffer.GetLength())
				break;
			CString sLineText=_T("");
			if(nPrevRowIndex==0)
				sLineText=sBuffer.Mid(0,nRowIndex);
			else
				sLineText=sBuffer.Mid(nPrevRowIndex+1,nRowIndex-nPrevRowIndex);
			if(sLineText.GetLength()<=0)
				continue;
			sLineText.Replace('\n',' ');
			int nCol=0;
			//按列拆分每行的字符串
			while(nColIndex<sLineText.GetLength()&&nColIndex!=-1)
			{
				int nPrevColIndex=nColIndex;
				nColIndex=sLineText.Find(' ',nPrevColIndex+1);
				if(nColIndex>sLineText.GetLength())
					break;
				CString sCellText=_T("");
				if(nPrevColIndex==0)
					sCellText=sLineText.Mid(0,nColIndex);
				else
					sCellText=sLineText.Mid(nPrevColIndex,nColIndex-nPrevColIndex);
				//将回车换行制表符等字符串替换为空格，移除字符串中多余的空格
				sCellText.Replace('\n',' ');
				sCellText.Replace('\t',' ');
				sCellText.Replace('\r',' ');
				sCellText.TrimLeft(' ');
				sCellText.TrimRight(' ');
				if(sCellText.GetLength()<=0)
					continue;
				//将单元格元素添加到字符串数组中
				table_data.strArr.Add(sCellText);
				//统计列数
				nCol++;
			}
			//统计列数
			if(nCol>table_data.nMaxCol)
				table_data.nMaxCol=nCol;
			//统计行数
			if(nCol>0)
				table_data.nMaxRow++;
		}
		GlobalUnlock(hglb); 
	}
	CloseClipboard();
	return TRUE;
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
CLDSLineTube *FindTubeInLine(CLDSLineTube *pLineTube,BOOL bStartEnd)
{
	CLDSNode *pNode=NULL;
	CLDSLineTube *pRelatedTube=NULL;
	if(pLineTube==NULL)
		return NULL;
	if(bStartEnd)
		pNode=pLineTube->pStart;
	else 
		pNode=pLineTube->pEnd;
	CSuperSmartPtr<CLDSPart>pPart;
	CLDSModule *pActiveModule=Ta.GetActiveModule();
	f3dPoint len_vec1=pLineTube->End()-pLineTube->Start();
	normalize(len_vec1);
	for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr(); pPart=Ta.Parts.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_LINETUBE||pPart->handle==pLineTube->handle) //排除当前钢管
			continue;
		if(!pLineTube->ModuleTogetherWith(pPart))
			continue;	//不是共存于同一模型中的构件
		if(pPart.LineTubePointer()->pStart!=pNode&&pPart.LineTubePointer()->pEnd!=pNode)
			continue;
		//if(pPart.LineTubePointer()->GetDiameter()!=pLineTube->GetDiameter())
		//	continue;	//保证共线钢管直径相同
		//两钢管是否共线
		f3dPoint len_vec2=pPart.LineTubePointer()->End()-pPart.LineTubePointer()->Start();
		normalize(len_vec2);
		if(fabs(len_vec1*len_vec2)>EPS_COS)
		{
			pRelatedTube = pPart.LineTubePointer();
			break;
		}
	}
	return pRelatedTube;
}
#endif
int SelBoltDByWingWide(double wing_wide,char* grade/*=NULL*/)
{
	int d=12;
	BOLT_SELCOND* pMaxBoltSeries=NULL;
	for(BOLT_SELCOND *pBoltSel=Ta.listSelBoltCond.GetFirst();pBoltSel;pBoltSel=Ta.listSelBoltCond.GetNext())
	{
		if(pMaxBoltSeries==NULL)
			pMaxBoltSeries=pBoltSel;
		else if(pBoltSel->upWidthLim>pMaxBoltSeries->upWidthLim)
			pMaxBoltSeries=pBoltSel;
		if(wing_wide<=pBoltSel->upWidthLim)
		{
			if(grade)
				StrCopy(grade,pBoltSel->grade,5);
			return pBoltSel->d;
		}
	}
	if(pBoltSel==NULL&&pMaxBoltSeries!=NULL)
	{
		logerr.Log("%.0f的角钢肢宽未指定合适的螺栓规格级别，系统默认为%sM%d系列螺栓",wing_wide,pMaxBoltSeries->grade,pMaxBoltSeries->d);
		if(grade)
			StrCopy(grade,pMaxBoltSeries->grade,5);
		return pMaxBoltSeries->d;
	}
	else
		return d;
}

//通过段号字符串得到段号哈希表
/*void GetSegNoHashTblBySegStr(CString sSegStr,CHashTable<long> &segNoHashTbl)
{
	char seg_str[200]="";
	_snprintf(seg_str,199,"%s",sSegStr);
	if(seg_str[0]=='*')
		segNoHashTbl.Empty();
	else
	{
		segNoHashTbl.CreateHashTable(160);
		for(long seg_i=FindAdaptNo(seg_str,",","-");!IsAdaptNoEnd();seg_i=FindAdaptNo(NULL,",","-"))
			segNoHashTbl.SetValueAt(seg_i,seg_i);
	}
}*/
//移至TowerCad\ParseAdaptNo.h中 wht 14-09-20
/*DWORD GetSegNoHashTblBySegStr(CString sSegStr,CHashList<SEGI> &segNoHashTbl)
{
char seg_str[200]="";
_snprintf(seg_str,199,"%s",sSegStr);
if(seg_str[0]=='*')
segNoHashTbl.Empty();
else
{
for(SEGI seg_i=FindAdaptNoForSegI(seg_str,",","-");!IsAdaptNoEndForSegI();seg_i=FindAdaptNoForSegI(NULL,",","-"))
segNoHashTbl.SetValue(seg_i.iSeg,seg_i);
}
return segNoHashTbl.GetNodeNum();
}*/
//通过模型号字符串得到模型号哈希表
void GetModuleNoHashTblByModuleStr(CString sModuleScope,CHashTable<long> &moduleNoHashTbl)
{
	char module_str[200]="";
	_snprintf(module_str,199,"%s",sModuleScope);
	if(module_str[0]=='*')
		moduleNoHashTbl.Empty();
	else
	{
		moduleNoHashTbl.CreateHashTable(160);
		for(long module_i=FindAdaptNo(module_str);!IsAdaptNoEnd();module_i=FindAdaptNo(NULL))
			moduleNoHashTbl.SetValueAt(module_i,module_i);
	}
}


//根据主管直径查找所有可用的刚性法兰
int FindFlGParamByTubeD(double d,ATOM_LIST<FL_PARAM2*> &flList)
{
	flList.Empty();
	for(int i=0;i<flg_param_N;i++)
	{
		if(fabs(flg_param_tbl[i].d-d)<EPS)
			flList.append(&flg_param_tbl[i]);
	}
	return flList.GetNodeNum();
}
//根据主管直径和对接钢管直径查找所有可用的刚性法兰
int FindFlGParamByTubeD(double d,double couple_d,ATOM_LIST<FL_PARAM2*> &flList)
{
	flList.Empty();
	for(int i=0;i<flg_param_N;i++)
	{
		if(fabs(flg_param_tbl[i].d-d)<EPS&&fabs(flg_param_tbl[i].couple_d-couple_d)<EPS)
			flList.append(&flg_param_tbl[i]);
	}
	return flList.GetNodeNum();
}
//根据主管直径查找所有可用的柔性法兰
int FindFlRParamByTubeD(double d,ATOM_LIST<FL_PARAM2*> &flList)
{	
	flList.Empty();
	for(int i=0;i<flr_param_N;i++)
	{
		if(fabs(flr_param_tbl[i].d-d)<EPS)
			flList.append(&flr_param_tbl[i]);
	}
	return flList.GetNodeNum();
}
//根据主管直径和对接钢管直径查找所有可用的柔性法兰
int FindFlRParamByTubeD(double d,double couple_d,ATOM_LIST<FL_PARAM2*> &flList)
{
	flList.Empty();
	for(int i=0;i<flr_param_N;i++)
	{
		if(fabs(flr_param_tbl[i].d-d)<EPS&&fabs(flr_param_tbl[i].couple_d-couple_d)<EPS)
			flList.append(&flr_param_tbl[i]);
	}
	return flList.GetNodeNum();
}
//根据设定区域初始化显示集合,pSameLifeRefObj表示选择与其具有共同生存空间的参照对象,NULL时默认为当前呼高
BOOL InitDisplaySetByPolyRgn(POLYGON &selRgn,CLDSDbObject* pSameLifeRefObj/*=NULL*/,
		NODESET* pRsltNodeSet/*=NULL*/,PARTSET* pRsltPartSet/*=NULL*/,bool rapidmode/*=false*/)
{
	CLDSModule *pActiveModule=NULL;
	if(pSameLifeRefObj!=NULL)
	{
		for(pActiveModule=Ta.Module.GetFirst();pActiveModule;pActiveModule=Ta.Module.GetNext())
		{
			if(pActiveModule->IsSonPart(pSameLifeRefObj,0,true))
				break;
		}
	}
	if(pActiveModule==NULL)
		pActiveModule=Ta.Module.FromHandle(Ta.m_hActiveModule);
	int nn = Ta.Node.GetNodeNum();
	if(pRsltNodeSet==NULL)
		pRsltNodeSet=&console.DispNodeSet;
	if(pRsltPartSet==NULL)
		pRsltPartSet=&console.DispPartSet;
	pRsltNodeSet->Empty();
	CHashList<bool>hashLiveNodes;
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())//int i=0;i<nn;i++)
	{
		if(pSameLifeRefObj)
		{
			if(!pSameLifeRefObj->ModuleTogetherWith(pNode))
				continue;
		}
		else if(pActiveModule&&!pActiveModule->IsSonPart(pNode))
			continue;	//非当前模型中的构件 wht 11-07-22
		if(selRgn.PtInRgn(pNode->xOriginalPos)==1)//点在多边形域面内（包括边界在内）
		{
			pRsltNodeSet->append(pNode);
			hashLiveNodes.SetValue(pNode->handle,true);
		}
	}
	pRsltPartSet->Empty();
	for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pSameLifeRefObj)
		{
			if(!pSameLifeRefObj->ModuleTogetherWith(pPart))
				continue;
		}
		else if(pActiveModule&&!pActiveModule->IsSonPart(pPart))
			continue;	//非当前模型中的构件 wht 11-07-22
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		if(!pPart->IsLinePart())
		{	//板
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{	//参数化板
				CLDSParamPlate *pParamPlate = (CLDSParamPlate*)pPart;
				if(pParamPlate->IsFL()||pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND)
					continue;	//在下边处理(根据钢管添加端连接构件)	
				long h=0;
				if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
					pParamPlate->GetDesignItemValue('E',&h);
				else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
					pParamPlate->GetDesignItemValue('P',&h);	//'A'改为'P' wht 11-05-20
				else if(pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
					pParamPlate->GetDesignItemValue('P',&h);
				CLDSNode *pBaseNode=console.FromNodeHandle(h);
				if(pBaseNode)
				{	//参数板基准节点不在多边形域面内(包括边界在内)则继续
					if(selRgn.PtInRgn(pBaseNode->ActivePosition())!=1)	
						continue;
				}
				else	
				{	//参数板原点不在多边形域面内(包括边界在内)
					if(selRgn.PtInRgn(pParamPlate->ucs.origin)!=1)
						continue;	
				}
				pRsltPartSet->append(pParamPlate);
			}
			else if(pPart->GetClassTypeId()==CLS_PLATE)
			{	//普通钢板
				CLDSPlate *pPlate = (CLDSPlate*)pPart;
				CLDSNode *pBaseNode=console.FromNodeHandle(pPlate->designInfo.m_hBaseNode);
				if(pBaseNode)
				{	//钢板基准节点不在多边形域面内(包括边界在内)则继续
					if(selRgn.PtInRgn(pBaseNode->ActivePosition())!=1)	
						continue;
				}
				else	
				{	//钢板原点不在多边形域面内(包括边界在内)则继续
					if(selRgn.PtInRgn(pPlate->ucs.origin)!=1)
						continue;
				}
				pRsltPartSet->append(pPlate);
				for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
					pRsltPartSet->append(pLsRef->GetLsPtr());
			}
			else	
			{	//其他构件	球体，环形构件等等
				if(selRgn.PtInRgn(pPart->ucs.origin)!=1)//构件原点不在多边形域面内(包括边界在内)则继续
					continue;
				pRsltPartSet->append(pPart);
			}
		}
		else
#endif
		{	//杆件
			CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
			CLDSNode* pStart = pLinePart->pStart;
			CLDSNode* pEnd   = pLinePart->pEnd;
			if(pStart==NULL||pEnd==NULL)
				continue;	//非正常杆件
			f3dLine line;
			line.startPt = pStart->xOriginalPos;
			line.endPt   = pEnd->xOriginalPos;
			if(rapidmode)
			{	//快速模式
				if(hashLiveNodes.GetValue(pStart->handle)==NULL)
					continue;
				else if(hashLiveNodes.GetValue(pEnd->handle)==NULL)
					continue;
			}
			else if(selRgn.LineInRgn(line.startPt,line.endPt)!=1) //线不在多边形域面内(包括边界在内)则继续
				continue;
			pRsltPartSet->append(pLinePart);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
			if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//添加端板及其附带构件到显示列表
				Ta.Parts.push_stack();
				CLDSLineTube *pLineTube = (CLDSLineTube*)pLinePart;
				CLDSParamPlate *pStartPlate = (CLDSParamPlate*)console.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
				if(pStartPlate)
				{	
					pRsltPartSet->append(pStartPlate);
					for(CLsRef *pLsRef=pStartPlate->GetFirstLsRef();pLsRef;pLsRef=pStartPlate->GetNextLsRef())
					{
						pRsltPartSet->append(pLsRef->GetLsPtr());
					}
					for(ATTACHPART_ITEM *pItem=pStartPlate->GetFirstAttachPart();pItem;pItem=pStartPlate->GetNextAttachPart())
					{
						CLDSPart *pTempPart = console.FromPartHandle(pItem->h);
						if(pTempPart)
							pRsltPartSet->append(pTempPart);
					}
				}
				CLDSParamPlate *pEndPlate = (CLDSParamPlate*)console.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
				if(pEndPlate)
				{
					pRsltPartSet->append(pEndPlate);
					for(CLsRef *pLsRef=pEndPlate->GetFirstLsRef();pLsRef;pLsRef=pEndPlate->GetNextLsRef())
						pRsltPartSet->append(pLsRef->GetLsPtr());
					for(ATTACHPART_ITEM *pItem=pEndPlate->GetFirstAttachPart();pItem;pItem=pEndPlate->GetNextAttachPart())
					{
						CLDSPart *pTempPart = console.FromPartHandle(pItem->h);
						if(pTempPart)
							pRsltPartSet->append(pTempPart);
					}
				}
				Ta.Parts.pop_stack();
			}
			else if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{	//添加角钢上的螺栓到显示列表
				CLDSLineAngle *pLineAngle = (CLDSLineAngle*)pLinePart;
				for(CLsRef *pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
					pRsltPartSet->append(pLsRef->GetLsPtr());
			}
#endif
		}
	}
	if(pRsltPartSet->GetNodeNum()<=0 || pRsltNodeSet->GetNodeNum()<=0)
		return FALSE;
	return TRUE;
}
//根据杆件及对称节点初始化展开视图的构件集合
BOOL InitDisplaySetByMirPoint(CLDSLinePart* pLinePart,CLDSNode* pMirStart,CLDSNode* pMirEnd)
{
	if(pLinePart==NULL || pMirStart==NULL )
		return FALSE;
	f3dPoint pPoints[4];
	pPoints[0] = pLinePart->pStart->ActivePosition();
	pPoints[1] = pLinePart->pEnd->ActivePosition();
	int nCounts=4;
	if(pMirEnd!=NULL)
	{
		pPoints[2] = pMirEnd->ActivePosition();
		if(pMirStart!=NULL) //起点不重合
			pPoints[3] = pMirStart->ActivePosition();
		else				//起点重合
			nCounts = 3;
	}
	else
	{
		pPoints[2] = pMirStart->ActivePosition();
		nCounts = 3;
	}
	fPolygonRgn selRgn;
	if(!selRgn.CreatePolygonRgn(pPoints, nCounts))
		return FALSE;
	CLDSModule *pActiveModule=Ta.Module.FromHandle(Ta.m_hActiveModule);
	int nn = Ta.Node.GetNodeNum();
	console.DispNodeSet.Empty();
	for(int i=0;i<nn;i++)
	{
		CLDSNode* pNode = &Ta.Node[i];
		if(pActiveModule&&!pActiveModule->IsSonPart(pNode))
			continue;	//非当前模型中的构件 wht 11-07-22
		if(selRgn.PtInRgn(pNode->ActivePosition())==1)//点在多边形域面内(包括边界在内)
			console.DispNodeSet.append(pNode);
	}
	f3dLine line;
	console.DispPartSet.Empty();
	for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pActiveModule&&!pActiveModule->IsSonPart(pPart))
			continue;	//非当前模型中的构件 wht 11-07-22
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		if(!pPart->IsLinePart())
		{	//板
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{	//参数化板
				CLDSParamPlate *pParamPlate = (CLDSParamPlate*)pPart;
				if(pParamPlate->IsFL()||pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND)
					continue;	//在下边处理(根据钢管添加端连接构件)
				CLDSNode *pBaseNode=pParamPlate->GetBaseNode();
				if(pBaseNode)
				{	//参数板基准节点不在多边形域面内(包括边界在内)则继续
					if(selRgn.PtInRgn(pBaseNode->ActivePosition())!=1)	
						continue;
				}
				else	
				{	//参数板原点不在多边形域面内(包括边界在内)
					if(selRgn.PtInRgn(pParamPlate->ucs.origin)!=1)
						continue;	
				}
				console.DispPartSet.append(pParamPlate);
			}
			else if(pPart->GetClassTypeId()==CLS_PLATE)
			{	//普通钢板
				CLDSPlate *pPlate = (CLDSPlate*)pPart;
				if(pPlate->jdb_style==4)	//螺栓垫板以基准杆件为判断基准
				{
					CLDSLinePart* pLinePart=(CLDSLinePart*)console.FromPartHandle(pPlate->designInfo.m_hBasePart,CLS_LINEPART);
					if(pLinePart==NULL)
						continue;
					if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
						continue;	//非正常杆件
					line.startPt = pLinePart->pStart->ActivePosition();
					line.endPt   = pLinePart->pEnd->ActivePosition();
					if(selRgn.LineInRgn(line)!=1) //线不在多边形域面内(包括边界在内)则继续
						continue;
				}
				else
				{
					CLDSNode *pBaseNode=console.FromNodeHandle(pPlate->designInfo.m_hBaseNode);
					if(pBaseNode)
					{	//钢板基准节点不在多边形域面内(包括边界在内)则继续
						if(selRgn.PtInRgn(pBaseNode->ActivePosition())!=1)	
							continue;
					}
					else	
					{	//钢板原点不在多边形域面内(包括边界在内)则继续
						if(selRgn.PtInRgn(pPlate->ucs.origin)!=1)
							continue;
					}
				}
				console.DispPartSet.append(pPlate);
				for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
					console.DispPartSet.append(pLsRef->GetLsPtr());
			}
			else	
			{	//其他构件	球体，环形构件等等
				if(selRgn.PtInRgn(pPart->ucs.origin)!=1)//构件原点不在多边形域面内(包括边界在内)则继续
					continue;
				console.DispPartSet.append(pPart);
			}
		}
		else
#endif
		{	//杆件
			pLinePart=(CLDSLinePart*)pPart;
			CLDSNode* pStart = pLinePart->pStart;
			CLDSNode* pEnd   = pLinePart->pEnd;
			if(pStart==NULL||pEnd==NULL)
				continue;	//非正常杆件
			line.startPt = pStart->ActivePosition();
			line.endPt   = pEnd->ActivePosition();
			if(selRgn.LineInRgn(line)!=1) //线不在多边形域面内(包括边界在内)则继续
				continue;
			console.DispPartSet.append(pLinePart);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
			if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//添加端板及其附带构件到显示列表
				CLDSLineTube *pLineTube = (CLDSLineTube*)pLinePart;
				Ta.Parts.push_stack();
				CSuperSmartPtr<CLDSPart> pStartPlate = console.FromPartHandle(pLineTube->m_tJointStart.hLinkObj);
				if(pStartPlate.IsHasPtr()&&pStartPlate->IsPlate())
				{	
					if(console.DispPartSet.FromHandle(pStartPlate->handle)==NULL)
						console.DispPartSet.append(pStartPlate);
					if(pStartPlate->GetClassTypeId()==CLS_PARAMPLATE)
					{
						for(CLsRef *pLsRef=pStartPlate.ParamPlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pStartPlate.ParamPlatePointer()->GetNextLsRef())
						{
							if(console.DispPartSet.FromHandle(pLsRef->GetLsPtr()->handle)==NULL)
								console.DispPartSet.append(pLsRef->GetLsPtr());
						}
						for(ATTACHPART_ITEM *pItem=pStartPlate.ParamPlatePointer()->GetFirstAttachPart();pItem;pItem=pStartPlate.ParamPlatePointer()->GetNextAttachPart())
						{
							CLDSPart *pTempPart = console.FromPartHandle(pItem->h);
							if(pTempPart&&console.DispPartSet.FromHandle(pItem->h)==NULL)
								console.DispPartSet.append(pTempPart);
						}
					}
					else if(pStartPlate->GetClassTypeId()==CLS_PLATE)
					{
						for(CLsRef *pLsRef=pStartPlate.PlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pStartPlate.ParamPlatePointer()->GetNextLsRef())
						{
							if(console.DispPartSet.FromHandle(pLsRef->GetLsPtr()->handle)==NULL)
								console.DispPartSet.append(pLsRef->GetLsPtr());
						}
					}
				}
				CSuperSmartPtr<CLDSPart> pEndPlate = console.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj);
				if(pEndPlate.IsHasPtr()&&pEndPlate->IsPlate())
				{
					if(console.DispPartSet.FromHandle(pEndPlate->handle)==NULL)
						console.DispPartSet.append(pEndPlate);
					if(pEndPlate->GetClassTypeId()==CLS_PARAMPLATE)
					{
						for(CLsRef *pLsRef=pEndPlate.ParamPlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pEndPlate.ParamPlatePointer()->GetNextLsRef())
						{
							if(console.DispPartSet.FromHandle(pLsRef->GetLsPtr()->handle)==NULL)
								console.DispPartSet.append(pLsRef->GetLsPtr());
						}
						for(ATTACHPART_ITEM *pItem=pEndPlate.ParamPlatePointer()->GetFirstAttachPart();pItem;pItem=pEndPlate.ParamPlatePointer()->GetNextAttachPart())
						{
							CLDSPart *pTempPart = console.FromPartHandle(pItem->h);
							if(pTempPart&&console.DispPartSet.FromHandle(pItem->h)==NULL)
								console.DispPartSet.append(pTempPart);
						}
					}
					else if(pEndPlate->GetClassTypeId()==CLS_PLATE)
					{
						for(CLsRef *pLsRef=pEndPlate.PlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pEndPlate.PlatePointer()->GetNextLsRef())
						{
							if(console.DispPartSet.FromHandle(pLsRef->GetLsPtr()->handle)==NULL)
								console.DispPartSet.append(pLsRef->GetLsPtr());
						}
					}
				}
				Ta.Parts.pop_stack();
			}
			else if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{	//添加角钢上的螺栓到显示列表
				CLDSLineAngle *pLineAngle = (CLDSLineAngle*)pLinePart;
				for(CLsRef *pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
				{
					if(console.DispPartSet.FromHandle(pLsRef->GetLsPtr()->handle)==NULL)
						console.DispPartSet.append(pLsRef->GetLsPtr());
				}
			}
#endif
		}
	}
	return TRUE;
}

//转换超出顶点显示边界很多的二维线为有效线(否则WINDOWS显示不正常)
static BOOL Valid2dLine(GEPOINT2D start,GEPOINT2D end,GEPOINT2D topleft,GEPOINT2D btmright)
{
	double deitax=end.x-start.x;
	double deitay=end.y-start.y;
	if(fabs(deitax)<=EPS)
	{	//竖线
		if(start.x<topleft.x||start.x>btmright.x)
			return FALSE;
		if(start.y<topleft.y&&end.y<topleft.y)
			return FALSE;
		if(start.y>btmright.y&&end.y>btmright.y)
			return FALSE;
		return TRUE;
	}
	else if(fabs(deitay)<=EPS)
	{	//水平横线
		if(start.y<topleft.y||start.y>btmright.y)
			return FALSE;
		if(start.x<topleft.x&&end.x<topleft.x)
			return FALSE;
		if(start.x>btmright.x&&end.x>btmright.x)
			return FALSE;
		return TRUE;
	}
	double tan_val =(end.y-start.y)/deitax;
	double ctan_val=(end.x-start.x)/deitay;
	if(start.x<topleft.x)
	{
		start.y -= (start.x-topleft.x)*tan_val;
		start.x  = topleft.x;
	}
	else if(start.x>btmright.x)
	{
		start.y -= (start.x-btmright.x)*tan_val;
		start.x  = btmright.x;
	}
	if(start.y<topleft.y)
	{
		start.x-= (start.y-topleft.y)*ctan_val;
		start.y = topleft.y;
	}
	else if(start.y>btmright.y)
	{
		start.x -= (start.y-btmright.y)*ctan_val;
		start.y  = btmright.y;
	}
	if(end.x<topleft.x)
	{
		end.y -= (end.x-topleft.x)*tan_val;
		end.x  = topleft.x;
	}
	else if(end.x>btmright.x)
	{
		end.y -= (end.x-btmright.x)*tan_val;
		end.x  = btmright.x;
	}
	if(end.y<topleft.y)
	{
		end.x-= (end.y-topleft.y)*ctan_val;
		end.y = topleft.y;
	}
	else if(end.y>btmright.y)
	{
		end.x -= (end.y-btmright.y)*ctan_val;
		end.y = btmright.y;
	}
	else
		return FALSE;
	return TRUE;
}
static BOOL IsOusideLine(GEPOINT2D start,GEPOINT2D end, GEPOINT2D topleft,GEPOINT2D btmright)
{
	if(topleft.y>btmright.y)	//规范为左上角Y值小
	{
		double ff=topleft.y;
		topleft.y=btmright.y;
		btmright.y=ff;
	}
	if(start.x<start.x&&end.x<topleft.x)
		return TRUE;
	else if(start.y<topleft.y&&end.y<topleft.y)
		return TRUE;
	else if(start.x>btmright.x&&end.x>btmright.x)
		return TRUE;
	else if(start.y>btmright.y&&end.y>btmright.y)
		return TRUE;
	return FALSE;
}
BOOL IsLineInRect(GEPOINT2D start,GEPOINT2D end, GEPOINT2D topleft,GEPOINT2D btmright)
{
 	if(IsOusideLine(start,end,topleft,btmright))//区域外直线
		return FALSE;
	if( start.x<topleft.x||start.y<topleft.y||start.x>btmright.x||start.y>btmright.y||
		end.x  <topleft.x||end.y  <topleft.y||end.x  >btmright.x||end.y  >btmright.y)
		return Valid2dLine(start,end,topleft,btmright);
	else
		return TRUE;
}
void GetPlateSetConnectedInAngle(CLDSLineAngle *pAngle,PARTSET &plateSet,BYTE wing_x0_y1_all2,CLDSNode* pNode)
{
	plateSet.Empty();
	POLYGON rgn;
	int i=0;
	f3dPoint pos;
	if(pNode)
	{
		pos=pNode->Position();
		if(pNode==pAngle->pStart)
			pos+=pAngle->ucs.axis_z*(-pAngle->startOdd()+60);
		else if(pNode==pAngle->pEnd)
			pos-=pAngle->ucs.axis_z*(pAngle->endOdd()+60);
		SnapPerp(&pos,pAngle->Start(),pAngle->End(),pos);
	}
	const double MARGIN_EPS = pAngle->GetWidth();
	GECS wingx_cs,wingy_cs;
	//X肢坐标系
	wingx_cs.axis_x=pAngle->End()-pAngle->Start();
	normalize(wingx_cs.axis_x);
	wingx_cs.origin=GEPOINT(pAngle->Start())-wingx_cs.axis_x*pAngle->startOdd();
	wingx_cs.axis_y=pAngle->GetWingVecX();
	wingx_cs.axis_z=pAngle->vxWingNorm;
	//Y肢坐标系
	wingy_cs=wingx_cs;
	wingy_cs.axis_y=pAngle->GetWingVecY();
	wingy_cs.axis_z=pAngle->vyWingNorm;
	GEPOINT2D topleft(0,0),btmright(pAngle->GetLength(),pAngle->GetWidth());
	for (CLDSPlate* pPlate=(CLDSPlate*)console.EnumPartFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)console.EnumPartNext(CLS_PLATE))
	{	
		int nFace=pPlate->GetFaceN();
		//1.过滤板的法线和角钢的法线不平行的
		f3dPoint vCurNorm;
		f3dPoint datum_pos=pPlate->ucs.origin;
		char cLappedWing='X';
		for(i=0;i<nFace;i++)
		{
			f3dPoint vPlateNorm=(i==0)?pPlate->ucs.axis_z:pPlate->GetHuoquFaceNorm(i-1);
			double ddx=vPlateNorm*pAngle->vxWingNorm;
			double ddy=vPlateNorm*pAngle->vyWingNorm;
			double justify=0;
			if((wing_x0_y1_all2==0||wing_x0_y1_all2==2)&&
				fabs(ddx)>EPS_COS2&&fabs(ddx)>fabs(justify))
			{
				vCurNorm=pAngle->vxWingNorm;
				cLappedWing='X';
				break;
			}
			if((wing_x0_y1_all2==1||wing_x0_y1_all2==2)&&
				fabs(ddy)>EPS_COS2&&fabs(ddy)>fabs(justify))
			{
				vCurNorm=pAngle->vyWingNorm;
				cLappedWing='Y';
				break;
			}
		}
		if(i>=nFace)
			continue;
		else if(i>0)
		{
			datum_pos=pPlate->GetHuoquLine(i-1).endPt;
			coord_trans(datum_pos,pPlate->ucs,TRUE);
		}
		//2.指定节点时首先根据钢板基准节点进行判断
		if(pNode&&pPlate->designInfo.m_hBaseNode==pNode->handle)
		{
			BOOL bIncAngle=FALSE;
			if(pPlate->designInfo.m_hBasePart==pAngle->handle)
				bIncAngle=TRUE;
			else
			{
				for(CDesignLjPartPara* pLjPart=pPlate->designInfo.partList.GetFirst();pLjPart;
					pLjPart=pPlate->designInfo.partList.GetNext())
				{
					if(pLjPart->hPart==pAngle->handle)
						bIncAngle=TRUE;
				}
			}
			if(bIncAngle)	//此连接板中没有连接当前角钢
			{
				plateSet.append(pPlate);
				continue;
			}
		}
		//3.未指定节点或根据指定节点判断无效时判断钢板是否与角钢相连
		if(fabs((datum_pos-pAngle->Start())*vCurNorm)>MARGIN_EPS)
			continue;
		//4.判断钢板与角钢是否有螺栓连接
		CLsRef *pLsRef=NULL;
		for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{
			if(pAngle->FindLsByHandle((*pLsRef)->handle)!=NULL)
				break;
		}
		if(pLsRef==NULL)
			continue;
		//5.判断角钢是否与钢板贴合
		GECS cs=(cLappedWing=='X')?wingx_cs:wingy_cs;
		GEPOINT prevVertex;
		bool findPrevVertex=false,bValidPlate=false;
		CMaxDouble max_x;
		CMinDouble min_x;
		for (PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			//此处应该是获取钢板上轮廓点的真实坐标，不应调用SpreadToACS()函数  wxc-2017.1.20
			GEPOINT vertice=pPlate->GetRealVertex(pVertex->vertex);	
			vertice.z+=pPlate->GetNormOffset();
			coord_trans(vertice,pPlate->ucs,TRUE,TRUE);
			vertice=cs.TransPToCS(vertice);
			min_x.Update(vertice.x);
			max_x.Update(vertice.x);
			if(fabs(vertice.z)>pAngle->GetThick()+pPlate->GetThick()*2)
				continue;
			if(!findPrevVertex)
			{
				prevVertex=vertice;
				findPrevVertex=true;
				continue;
			}
			else if(!bValidPlate&&IsLineInRect(GEPOINT2D(prevVertex.x,prevVertex.y),GEPOINT2D(vertice.x,vertice.y),topleft,btmright))
				bValidPlate=true;
			prevVertex=vertice;
		}
		if(!bValidPlate)
			continue;
		if(pNode)
		{
			f3dPoint node_pos=pos;
			node_pos=cs.TransPToCS(node_pos);
			if(node_pos.x<min_x.number||node_pos.x>max_x.number)
				continue;
		}
		plateSet.append(pPlate);
	}
}
bool SmartInitRayJgLsParas(CLDSNode* pNode,CLDSLineAngle *pRayJg,CLDSLineAngle* pBaseJg,ATOM_LIST<CDesignLsPara>* pLsParaList)
{
	if(pNode==NULL||pRayJg==NULL||pBaseJg==NULL)
		return false;
	if(pRayJg->pStart==NULL||pRayJg->pEnd==NULL)
		return false;
	if(pBaseJg&&pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		CLDSGroupLineAngle* pBaseGroupJg=(CLDSGroupLineAngle*)pBaseJg;
		pBaseJg=(CLDSLineAngle*)Ta.Parts.FromHandle(pBaseGroupJg->m_hDatumSonAngle,CLS_LINEANGLE);
	}
	int nHasEndLs=0;
	for(CLsRef* pLsRef=pBaseJg->GetFirstLsRef();pLsRef;pLsRef=pBaseJg->GetNextLsRef())
	{
		if(pRayJg->FindLsByHandle((*pLsRef)->handle))
			nHasEndLs++;
	}
	BOOL bHasEndLj=(nHasEndLs>0)?TRUE:FALSE;
	BOOL bDesStart=(pRayJg->pStart->handle==pNode->handle)?TRUE:FALSE;
	f3dPoint work_norm;
	GetWorkNorm(pBaseJg,pRayJg,&work_norm);
	int work_wing=0;
	BOOL bInsideXieJg=IsInsideJg(pRayJg,work_norm,&work_wing);
	CConnectInfo ls_group=bDesStart?pRayJg->connectStart:pRayJg->connectEnd;
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,ls_group.d);
	int bolt_num=ls_group.wnConnBoltN-nHasEndLs;
	int bolt_rows=(bolt_num>=5)?2:1;
	JGZJ jgzj=(work_wing==0)?pRayJg->GetZhunJu('X'):pRayJg->GetZhunJu('Y');
	//初始化角钢螺栓设计参数
	CDesignLsPara ls_stru;
	ls_stru.d=ls_group.d;
	//法向
	ls_stru.des_work_norm.norm_style=1;
	ls_stru.des_work_norm.hVicePart=pRayJg->handle;
	ls_stru.des_work_norm.direction=bInsideXieJg?0:1;
	ls_stru.des_work_norm.norm_wing=work_wing;
	//基点
	if(bHasEndLj)
	{	//有端连接螺栓存在时，肯定是单排螺栓
		f3dPoint base_wing_vec,ray_vec;
		if(fabs(pBaseJg->vxWingNorm*work_norm)>fabs(pBaseJg->vyWingNorm*work_norm))
			base_wing_vec=pBaseJg->GetWingVecX();
		else
			base_wing_vec=pBaseJg->GetWingVecY();
		if(bDesStart)
			ray_vec=(pRayJg->End()-pRayJg->Start()).normalized();
		else
			ray_vec=(pRayJg->Start()-pRayJg->End()).normalized();
		//
		bolt_rows=1;	
		ls_stru.des_base_pos.datumPoint.datum_pos_style=3;
		ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1=pRayJg->handle;
		ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2=pBaseJg->handle;
		ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=0;
		ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=4;
		if(base_wing_vec*ray_vec>0)
			ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2=LsSpace.EndSpace+ftoi(pBaseJg->GetWidth());
		else
			ls_stru.des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2=-LsSpace.EndSpace;
	}
	else
	{	//无端连接(角钢楞点为定位基点)
		ls_stru.des_base_pos.datumPoint.datum_pos_style=1;
		ls_stru.des_base_pos.datumPoint.des_para.RODEND.hRod=pRayJg->handle;
		ls_stru.des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
		ls_stru.des_base_pos.datumPoint.des_para.RODEND.direction=bDesStart?0:1;
		ls_stru.des_base_pos.datumPoint.des_para.RODEND.offset_wing=work_wing;
		ls_stru.des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=0;
		ls_stru.des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
	}
	//位置
	ls_stru.des_base_pos.direction=bDesStart?0:1;
	ls_stru.des_base_pos.hPart=pRayJg->handle;
	ls_stru.des_base_pos.offset_wing=work_wing;
	double len_offset=LsSpace.EndSpace;
	for(int i=0;i<bolt_num;i++)
	{	
		ls_stru.des_base_pos.len_offset_dist=len_offset;
		if(bolt_rows==1||jgzj.g1==0||jgzj.g2==0)
		{	//单排螺栓布置
			ls_stru.des_base_pos.wing_offset_dist=jgzj.g;
			len_offset+=LsSpace.SingleRowSpace;
		}
		else				//双排螺栓布置
		{
			if(i%2==0)	//偶数螺栓布置在楞线侧
				ls_stru.des_base_pos.wing_offset_dist=jgzj.g1;
			else		//奇数螺栓布置在肢翼侧
				ls_stru.des_base_pos.wing_offset_dist=jgzj.g2;
			len_offset+=LsSpace.doubleRowSpace*0.5;
		}
		pLsParaList->append(ls_stru);
	}
	return pLsParaList->GetNodeNum()>0;
}
bool SmartInitBaseJgLsParas(CLDSNode* pNode,CLDSLineAngle *pBaseJg,BYTE work_wing,int bolt_num,ATOM_LIST<CDesignLsPara>* pLsParaList)
{
	if(pNode==NULL||pBaseJg==NULL)
		return false;
	BOOL bDesStart=(pBaseJg->pStart->handle==pNode->handle)?TRUE:FALSE;
	int bolt_d=bDesStart?pBaseJg->connectStart.d:pBaseJg->connectEnd.d;
	int bolt_rows=(bolt_num>=5)?2:1;
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,bolt_d);
	JGZJ jgzj=(work_wing==0)?pBaseJg->GetZhunJu('X'):pBaseJg->GetZhunJu('Y');
	//
	CDesignLsPara ls_stru;
	ls_stru.d=bolt_d;
	ls_stru.des_work_norm.norm_style=1;
	ls_stru.des_work_norm.hVicePart=pBaseJg->handle;
	ls_stru.des_work_norm.norm_wing=work_wing;
	ls_stru.des_work_norm.direction=0;
	ls_stru.des_base_pos.datumPoint.datum_pos_style=2;	//杆件上节点为基准
	ls_stru.des_base_pos.datumPoint.des_para.RODNODE.hNode=pNode->handle;
	ls_stru.des_base_pos.datumPoint.des_para.RODNODE.hRod=pBaseJg->handle;
	ls_stru.des_base_pos.datumPoint.des_para.RODNODE.offset_wing=work_wing;
	ls_stru.des_base_pos.datumPoint.des_para.RODNODE.direction=0;
	ls_stru.des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style=0;
	//ls_stru.des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset=m_bRayEndFaceOffset;
	ls_stru.des_base_pos.hPart=pBaseJg->handle;
	ls_stru.des_base_pos.norm_offset.AddThick(-pBaseJg->handle,TRUE);
	ls_stru.des_base_pos.offset_wing=work_wing;
	double up_offset=0,down_offset=0;
	if(bolt_rows==1)
	{	//单排布置螺栓
		if(bolt_num%2==0)
			down_offset=up_offset=LsSpace.SingleRowSpace*0.5;
		else if(bolt_num%2==1)
			down_offset=LsSpace.SingleRowSpace;
		for(int i=0;i<bolt_num;i++)
		{
			ls_stru.des_base_pos.wing_offset_dist=jgzj.g;
			if(i%2==0)
			{	//偶数螺栓布在终端侧
				ls_stru.des_base_pos.direction=1;	//自终至始偏移
				ls_stru.des_base_pos.len_offset_dist=up_offset;
				up_offset+=LsSpace.SingleRowSpace;
			}
			else		
			{	//奇数螺栓布在终端侧
				ls_stru.des_base_pos.len_offset_dist=down_offset;
				ls_stru.des_base_pos.direction=0;	//自始至终偏移
				down_offset+=LsSpace.SingleRowSpace;
			}
		}
	}
	else
	{	//多排布置螺栓
		if(bolt_num%2==0)
			down_offset=up_offset=LsSpace.doubleRowSpace*0.25;
		else if(bolt_num%2==1)
			down_offset=LsSpace.doubleRowSpace*0.5;
		BYTE cUpBoltLine=1,cDownBoltLine=2;
		for(int i=0;i<bolt_num;i++)
		{
			if(i%2==0)
			{	//偶数螺栓布在始端侧
				if(cUpBoltLine==1)
					ls_stru.des_base_pos.wing_offset_dist=jgzj.g1;
				else
					ls_stru.des_base_pos.wing_offset_dist=jgzj.g2;
				cUpBoltLine=(cUpBoltLine+1)%2;
				//
				ls_stru.des_base_pos.direction=1;
				ls_stru.des_base_pos.len_offset_dist=up_offset;
				up_offset+=LsSpace.doubleRowSpace*0.5;
			}
			else
			{	//奇数螺栓布在终端侧
				if(cDownBoltLine==1)
					ls_stru.des_base_pos.wing_offset_dist=jgzj.g1;
				else
					ls_stru.des_base_pos.wing_offset_dist=jgzj.g2;
				cDownBoltLine=(cDownBoltLine+1)%2;
				//
				ls_stru.des_base_pos.direction=0;
				ls_stru.des_base_pos.len_offset_dist=down_offset;
				down_offset+=LsSpace.doubleRowSpace*0.5;
			}
		}
	}
	return pLsParaList->GetNodeNum()>0;
}
//根据角钢进行钢板制弯
bool BendPlateByLineAngle(CLDSPlate* pDatumPlate,CLDSLineAngle *pBendBaseJg,double fHuoquAngle,BYTE ciBend_Ber0_Wing1,double fBendSpace/*=10*/)
{
	if(pDatumPlate==NULL||pBendBaseJg==NULL)
		return false;
	double ddx=pDatumPlate->ucs.axis_z*pBendBaseJg->vxWingNorm;
	double ddy=pDatumPlate->ucs.axis_z*pBendBaseJg->vyWingNorm;
	f3dPoint offset_vec=(fabs(ddx)>fabs(ddy))?pBendBaseJg->GetWingVecX():pBendBaseJg->GetWingVecY();
	f3dPoint pick_vec=(fabs(ddx)>fabs(ddy))?pBendBaseJg->vxWingNorm:pBendBaseJg->vyWingNorm;
	if(ciBend_Ber0_Wing1==0)
		offset_vec*=-1;
	else
		pick_vec*=-1;
	f3dPoint huoquPtS(pBendBaseJg->Start()),huoquPtE(pBendBaseJg->End());
	if(ciBend_Ber0_Wing1==1)
	{	//肢翼侧
		huoquPtS+=offset_vec*pBendBaseJg->GetWidth();
		huoquPtE+=offset_vec*pBendBaseJg->GetWidth();
	}
	f3dPoint huoqu_norm=pDatumPlate->ucs.axis_z;
	f3dPoint huoquline_vec=(huoquPtE-huoquPtS);
	GEPOINT verify_norm=offset_vec^huoquline_vec;
	if(verify_norm*pDatumPlate->ucs.axis_z<0)
		huoquline_vec*=-1.0;
	if(!RotateVectorAroundVector(huoqu_norm,fHuoquAngle,huoquline_vec))
		return false;
	//初始化钢板火曲信息
	huoquPtS+=offset_vec*fBendSpace;
	huoquPtE+=offset_vec*fBendSpace;
	coord_trans(huoquPtS,pDatumPlate->ucs,FALSE);
	coord_trans(huoquPtE,pDatumPlate->ucs,FALSE);
	huoquPtS.z=huoquPtE.z=0;
	pDatumPlate->face_N++;
	pDatumPlate->HuoQuLine[pDatumPlate->face_N-2].startPt=huoquPtS;
	pDatumPlate->HuoQuLine[pDatumPlate->face_N-2].endPt=huoquPtE;
	pDatumPlate->huoqufacenorm[pDatumPlate->face_N-2]=huoqu_norm;
	if(pDatumPlate->face_N==3)
		pDatumPlate->jdb_style=1;
	pDatumPlate->designInfo.m_bEnableFlexibleDesign=FALSE;	//关闭柔性设计
	//计算制弯线在板轮廓上的连接制弯点
	f2dLine huoquline2d(huoquPtS,huoquPtE);
	PROFILE_VERTEX* pHuoquStartVertex=NULL,*pHuoquEndVertex=NULL;
	PROFILE_VERTEX *pVertex=NULL,*pPrevVertex=pDatumPlate->vertex_list.GetTail();
	for(pVertex=pDatumPlate->vertex_list.GetFirst();pVertex;pPrevVertex=pVertex,pVertex=pDatumPlate->vertex_list.GetNext())
	{
		int prev_min_face=0,cur_min_face=0;
		if(pPrevVertex->vertex.feature<10)
			prev_min_face=pPrevVertex->vertex.feature;
		else
			prev_min_face=min(pPrevVertex->vertex.feature%10,pPrevVertex->vertex.feature/10);
		if(prev_min_face>1)
			continue;	//不是基面上的轮廓边
		if(pVertex->vertex.feature<10)
			cur_min_face=pVertex->vertex.feature;
		else
			cur_min_face=min(pVertex->vertex.feature%10,pVertex->vertex.feature/10);
		if(cur_min_face>1)
			continue;	//不是基面上的轮廓边
		f3dPoint inters;
		f2dLine edge_line(pPrevVertex->vertex,pVertex->vertex);
		if(Int2dpl(huoquline2d,edge_line,inters.x,inters.y)<=0)
			continue;	//两直线没有有效交点
		double justify=0;	//交点有效性判断
		if(fabs(edge_line.endPt.x-edge_line.startPt.x)>eps)
			justify=(inters.x-edge_line.startPt.x)/(edge_line.endPt.x-edge_line.startPt.x);
		else
			justify=(inters.y-edge_line.startPt.y)/(edge_line.endPt.y-edge_line.startPt.y);
		PROFILE_VERTEX *pHuoquVertex=NULL;
		if(justify>=-eps&&justify<=eps)			//前一顶点为制弯点
			pHuoquVertex=pPrevVertex;
		else if(justify>eps&&justify<1-eps)		//交点为轮廓边的内点
			pHuoquVertex=pDatumPlate->vertex_list.insert(PROFILE_VERTEX(inters));
		else if(justify>=1-eps&&justify<=1+eps)	//当前顶点为制弯点
			pHuoquVertex=pVertex;
		if(pHuoquVertex)
		{
			pHuoquVertex->vertex.feature=10+pDatumPlate->face_N;
			if(pHuoquStartVertex==NULL)
				pHuoquStartVertex=pHuoquVertex;
			else if(pHuoquEndVertex==NULL)
			{
				pHuoquEndVertex=pHuoquVertex;
				break;	//已找到并生成制弯线的两个匹配轮廓顶点
			}
		}
	}
	if(pHuoquStartVertex==NULL || pHuoquEndVertex==NULL)
		return false;	//板外形有问题，无法找到与制弯线相匹配的轮廓顶点
	//更新轮廓点所在面号
	vector_trans(huoquline_vec,pDatumPlate->ucs,FALSE,TRUE);
	GEPOINT verify_huoquvec=pHuoquEndVertex->vertex-pHuoquStartVertex->vertex;
	if(huoquline_vec*verify_huoquvec<0)
	{
		PROFILE_VERTEX* pTempVertex=pHuoquEndVertex;
		pHuoquEndVertex=pHuoquStartVertex;
		pHuoquStartVertex=pTempVertex;
	}
	BOOL bStart=FALSE;
	pVertex=pDatumPlate->vertex_list.GetFirst();
	while(1)
	{
		if(!bStart && pVertex==pHuoquStartVertex)
			bStart=TRUE;	//火曲面开始点
		if(bStart)	//开始火曲面
		{
			if(pVertex->vertex.feature==1)
				pVertex->vertex.feature=pDatumPlate->face_N;
			if(pVertex==pHuoquEndVertex)
				break;		//终止火曲面
		}
		pVertex=pDatumPlate->vertex_list.GetNext();
		if(pVertex==NULL)
			pVertex=pDatumPlate->vertex_list.GetFirst();
	}
	//完成火曲钢板后，更新新生成的火曲面上螺栓的法线
	for(CLsRef *pLsRef=pDatumPlate->GetFirstLsRef();pLsRef;pLsRef=pDatumPlate->GetNextLsRef())
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		f3dPoint ls_pos=pBolt->ucs.origin;
		coord_trans(ls_pos,pDatumPlate->ucs,FALSE);
		ls_pos.z=0;	//Z坐标归零 否则不能正确判断螺栓是否位于火曲面上
		if(DistOf2dPtLine(pHuoquStartVertex->vertex,pHuoquEndVertex->vertex,ls_pos)<0)
		{	//螺栓在火曲线右侧,说明螺栓为火曲面上的螺栓,须更新螺栓法线方向
			if((*pLsRef)->des_base_pos.datumPoint.datum_pos_style==0)
			{	//位置由用户指定的螺栓须更新螺栓孔坐标
				pBolt->des_base_pos.cLocationStyle=true;
				pBolt->des_base_pos.reside_plane=(BYTE)pDatumPlate->face_N-1;
				pBolt->des_base_pos.hPart=pDatumPlate->handle;
				pBolt->des_base_pos.len_offset_dist=ls_pos.x;
				pBolt->des_base_pos.wing_offset_dist=ls_pos.y;
			}
			pBolt->set_norm(huoqu_norm);
			pBolt->des_work_norm.norm_style=3;	//构件工作法线
			pBolt->des_work_norm.hVicePart=pDatumPlate->handle;		//基准板
			pBolt->des_work_norm.reside_plane=(BYTE)pDatumPlate->face_N-1;//构件工作法线
			pBolt->des_work_norm.vector=huoqu_norm;
			pBolt->correct_worknorm();
			pBolt->correct_pos();
		}
	}
	return true;
}