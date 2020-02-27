#include "stdafx.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "DrawTower.h"
#include "MenuFunc.h"
#include "Ta.h"
#include "GlobalVar.h"
#include "Query.h"
#include "MxbDlg.h"
//角钢明细表记录结构
typedef struct tagLsRecord
{
   int      type;         //1-螺栓；2-脚钉；3-垫圈
   char     grade[10];
   char     guige[20];
   char     symbol[10];
   int      number;
   double   weight;
   char     sNotes[30];
}LS_RECORD_STRU;
typedef struct tagPadRecord
{
	char  grade[10];		//Q235或A0F
	char  guige[10];			//垫圈厚度
	char  symbol[15];		//符号  厚度X数量
	double weight;			//重量
	double unit_wei;		//单件重
	int   number;			//数量
	char  sNotes[30];		//备注
}PAD_RECORD_STRU;
LS_RECORD_STRU *pRecordArr,*nailRecordArr;
PAD_RECORD_STRU *padRecordArr;
static PAD_RECORD_STRU pad_record[10];

static char TITLE[]="螺栓.脚钉.垫圈明细表";
static struct LS_HEADER {
	char sName[20];
    char sGrade[20];
	char sGuige[20];
	char sSymbol[20];
	char sNum[20];
	char sSubTotalWei[20];
	char sNotes[20];
	} header={"名称","级别","规格","符号","数量","重量(kg)","备注"};
static int slist[]={2,2,1,2,3,1,1};
static double fLineHeight[]={12,10,8};//依次为标题行、关键字行及数据行的高度
static double ItemSpace[]={10,12,23,15,10,20,30};//栏间距 WJH--00.10.03
static long nRecCount=0L,nRecCount1;//第一个表示总记录数，第二个表示当前段(列)记录数
static long nLsRecCount,nNailRecCount,nPadRecCount;

void BoltPadList(void);
int GetLsRecord();
void CreateLsTable(f3dPoint porg,int LocationPntStyle,int order=0);
void ads_mat_x_vec (ads_matrix mat, ads_point pin, ads_point pout);
void ads_mat_x_pt  (ads_matrix mat, ads_point pin, ads_point pout);
void ads_subvec    (ads_point ap, ads_point bp, ads_point dp);
void ads_mat_ident (ads_matrix matrix);
int	dragsampler (ads_point pt, ads_matrix mat);
static int nLineNum;
// 螺栓脚钉垫圈明细表
void BoltPadList(void)
{
	BoltPadTable(0);
	return;
	CMxbDlg dlg;
	long s1,s2;
	int i,k=0,nn,nColumnN;
	int LocationPntStyle,LayoutStyle;
	double ListWidth=0;
	AcGePoint3d LeftBottomPos,TopRightPos;
	f3dPoint porg,point,p0,p1,p2,p3;
	ads_point pp0,pp1,pp2,pp3;
	ads_name sset,ename;
	
	ads_mat_ident(CTM);
	// Select entities for DRAGGEN test 
	ads_prompt("\n请稍候......");
	//获得角钢(节点板)记录数
	nRecCount = GetLsRecord();
	ads_printf("\n表格共%ld行!",nRecCount);//记录数
	
	if (dlg.DoModal()!=IDOK)
		return;
	
	ads_command(RTSTR,"undo",RTSTR,"g",RTNONE);
	
	LeftBottomPos = acdbCurDwg()->extmin();//在WCS中图样左下界线的X,Y坐标
	TopRightPos = acdbCurDwg()->extmax();//在WCS中图样右上界线的X,Y坐标
	if(fabs(LeftBottomPos.x)>1e5)
		LeftBottomPos.x = LeftBottomPos.y = 0;
	if(fabs(TopRightPos.x)>1e5)
		TopRightPos.x = TopRightPos.y = 0;
	
	if(nRecCount>0)
	{
		nn=ELEMENTS(ItemSpace);
		for (i = 0; i < nn; i++)//计算明细表总宽
			ListWidth+=ItemSpace[i];
		
		LocationPntStyle=dlg.m_nLocatePos;//定位点方式
		LayoutStyle=dlg.m_nLayStyle;//排列方式
		nLineNum=dlg.m_nColumnLineN;	//行数
		
		if(LayoutStyle==0)//表格上下排成一列	Wjh--00.04.26
		{
			switch (LocationPntStyle)
			{
			case 0://左上角
				p0.Set(LeftBottomPos.x,TopRightPos.y,0.0);
				p1.Set(LeftBottomPos.x+ListWidth,TopRightPos.y,0.0);
				p2.Set(LeftBottomPos.x+ListWidth,TopRightPos.y-fLineHeight[0]-fLineHeight[1]-fLineHeight[2]*nRecCount,0.0);
				p3.Set(LeftBottomPos.x,TopRightPos.y-fLineHeight[0]-fLineHeight[1]-fLineHeight[2]*nRecCount,0.0);
				break;
			case 1://右上角
				p0.Set(TopRightPos.x,TopRightPos.y,0.0);
				p1.Set(TopRightPos.x-ListWidth,TopRightPos.y,0.0);
				p2.Set(TopRightPos.x-ListWidth,TopRightPos.y-fLineHeight[0]-fLineHeight[1]-fLineHeight[2]*nRecCount,0.0);
				p3.Set(TopRightPos.x,TopRightPos.y-fLineHeight[0]-fLineHeight[1]-fLineHeight[2]*nRecCount,0.0);
				break;
			case 2://左下角
				p0.Set(LeftBottomPos.x,LeftBottomPos.y,0.0);
				p1.Set(LeftBottomPos.x+ListWidth,LeftBottomPos.y,0.0);
				p2.Set(LeftBottomPos.x+ListWidth,LeftBottomPos.y+fLineHeight[0]+fLineHeight[1]+fLineHeight[2]*nRecCount,0.0);
				p3.Set(LeftBottomPos.x,LeftBottomPos.y+fLineHeight[0]+fLineHeight[1]+fLineHeight[2]*nRecCount,0.0);
				break;
			case 3://右下角
				p0.Set(TopRightPos.x,LeftBottomPos.y,0.0);
				p1.Set(TopRightPos.x-ListWidth,LeftBottomPos.y,0.0);
				p2.Set(TopRightPos.x-ListWidth,LeftBottomPos.y+fLineHeight[0]+fLineHeight[1]+fLineHeight[2]*nRecCount,0.0);
				p3.Set(TopRightPos.x,LeftBottomPos.y+fLineHeight[0]+fLineHeight[1]+fLineHeight[2]*nRecCount,0.0);
				break;
			}
		}
		else         //表格分成若干段
		{
			nColumnN=(int)ceil((double)nRecCount/nLineNum);
			nColumnN+=1;
			
			switch (LocationPntStyle)//表格0定位点	Wjh--00.04.26
			{
			case 0://左上角
				p0.Set(LeftBottomPos.x,TopRightPos.y,0.0);
				p1.Set(LeftBottomPos.x+ListWidth*nColumnN,TopRightPos.y,0.0);
				p2.Set(LeftBottomPos.x+ListWidth*nColumnN,TopRightPos.y-fLineHeight[0]-fLineHeight[1]-fLineHeight[2]*nLineNum,0.0);
				p3.Set(LeftBottomPos.x,TopRightPos.y-fLineHeight[0]-fLineHeight[1]-fLineHeight[2]*nLineNum,0.0);
				break;
			case 1://右上角
				p0.Set(TopRightPos.x,TopRightPos.y,0.0);
				p1.Set(TopRightPos.x-ListWidth*nColumnN,TopRightPos.y,0.0);
				p2.Set(TopRightPos.x-ListWidth*nColumnN,TopRightPos.y-fLineHeight[0]-fLineHeight[1]-fLineHeight[2]*nLineNum,0.0);
				p3.Set(TopRightPos.x,TopRightPos.y-fLineHeight[0]-fLineHeight[1]-fLineHeight[2]*nLineNum,0.0);
				break;
			case 2://左下角
				p0.Set(LeftBottomPos.x,LeftBottomPos.y,0.0);
				p1.Set(LeftBottomPos.x+ListWidth*nColumnN,LeftBottomPos.y,0.0);
				p2.Set(LeftBottomPos.x+ListWidth*nColumnN,LeftBottomPos.y+fLineHeight[0]+fLineHeight[1]+fLineHeight[2]*nLineNum,0.0);
				p3.Set(LeftBottomPos.x,LeftBottomPos.y+fLineHeight[0]+fLineHeight[1]+fLineHeight[2]*nLineNum,0.0);
				break;
			case 3://右下角
				p0.Set(TopRightPos.x,LeftBottomPos.y,0.0);
				p1.Set(TopRightPos.x-ListWidth*nColumnN,LeftBottomPos.y,0.0);
				p2.Set(TopRightPos.x-ListWidth*nColumnN,LeftBottomPos.y+fLineHeight[0]+fLineHeight[1]+fLineHeight[2]*nLineNum,0.0);
				p3.Set(TopRightPos.x,LeftBottomPos.y+fLineHeight[0]+fLineHeight[1]+fLineHeight[2]*nLineNum,0.0);
				break;
			}
		}
		
		Cpoint(pp0,p0);
		Cpoint(pp1,p1);
		Cpoint(pp2,p2);
		Cpoint(pp3,p3);
		ads_command(RTSTR,"PLINE",RT3DPOINT,pp0,RT3DPOINT,pp1,RT3DPOINT,pp2,RT3DPOINT,pp3,RTSTR,"C",0);
		ads_entlast(ename);//选中刚画出的多义线(矩形框)
		ads_redraw(ename,3);//3用于高亮显示刚画出的多义线(矩形框)
		
		ads_command(RTSTR,"zoom",RTSTR,"e",RTNONE);
		acedSSGet("L",NULL,NULL,NULL,sset);//选中刚生成的实体并将其组成为选择集
		
		Cpoint(Base,p0);//以上左上角点为表格插入点
		ads_point pt;//临时用
		if (ads_draggen(sset,"\n请点材料表位置,<Enter退出>: ",0,dragsampler,pt)==RTNORM)
		{	//上面函数中的的参数"0"(第三个参数)表示鼠标显示状态
			ads_xformss(sset, CTM);//CTM为生成的坐标平移矩阵
		}
		else//不进行拖拽修改
		{
			pt[X] = pp0[X];
			pt[Y] = pp0[Y];
			pt[Z] = pp0[Z];
			ads_redraw(ename,2);
		}
		ads_command(RTSTR,"erase",RTPICKS,sset,RTSTR,"",RTNONE);
		porg.x = pt[X];
		porg.y = pt[Y];
		porg.z = pt[Z];
		
		if(LayoutStyle==0)//单列表格输出方式	Wjh--00.10.04
		{
			nRecCount1=nRecCount;
			CreateLsTable(porg,LocationPntStyle);
		}
		else//分段表格输出方式	Wjh--00.04.26
		{
			nColumnN=(int)ceil((double)nRecCount/nLineNum);
			
			for (k = 0; k < nColumnN; k++)
			{
				if (k<(nColumnN-1))
				{
					s1=1+k*nLineNum;
					s2=(1+k)*nLineNum;
					nRecCount1=nLineNum;//当前列(段中的数据行数)
				}
				else//最后一列(段)表格
				{
					s1=s2+1;
					s2=nRecCount;
					nRecCount1=nRecCount-(nColumnN-1)*nLineNum;//当前列(段中的数据行数)
				}
				
				switch (LocationPntStyle)
				{
				case 0://左上角
					point.Set(porg.x+k*ListWidth,porg.y,porg.z);
					break;
				case 1://右上角
					point.Set(porg.x-k*ListWidth,porg.y,porg.z);
					break;
				case 2://左下角
					point.Set(porg.x+k*ListWidth,porg.y,porg.z);
					break;
				case 3://右下角
					point.Set(porg.x-k*ListWidth,porg.y,porg.z);
					break;
				}
				CreateLsTable(point,LocationPntStyle,k);
			}
		}
   }
   delete []pRecordArr;
   delete []nailRecordArr;
   delete []padRecordArr;
   ads_command(RTSTR,"undo",RTSTR,"e",RTNONE);//结束当前undo状态
}


/*
 程序功能 ---- 在图形屏幕上生成一个表格
 定义函数 ---- ztdata
 变量说明 ---- TITLE       标题头
               HEADER      栏名表 (NME1,NME2,NME3,...)
               DATAS       数据表 (COL1,COL2,COL3,...)
                           COL1=(RAW11,RAW12,RAW13,...)
                           COL2=(RAW21,RAW22,RAW23,...)
                           COL3=(RAW31,RAW32,RAW33,...)
               PORG        表格图形屏幕上的基准点
               LocationPntStyle        表格基准点
                                1.左上角
                                2.左下角
                                3.右上角
                                4.右下角
               SLIST       文本水平基准
                                1.左对齐
                                2.中对齐
                                3.右对齐
               HSPACE      每栏宽度 (WID1,WID2,WID3,...)
               HPARAMS     栏名参数 (TVSIZE,HVSIZE,VSIZE)
           |-----------------------------------------------| ------
           |                  TITLE(标题头)                | TVSIZE
           |-----------------------------------------------| ------
           |  NME1  |  NME2  |  NME3  | .................. | HVSIZE
           |-----------------------------------------------| ------
           |  RAW11 |  RAW21 |  RAW31 | .................. | VSIZE
           |-----------------------------------------------| ------
           |  RAW12 |  RAW22 |  RAW32 | .................. |
           |-----------------------------------------------|
           |  RAW13 |  RAW23 |  RAW33 | .................. |
           |-----------------------------------------------|
           | ............................................. |
           |-----------------------------------------------|
           |  WID1  |  WID2  |  WID3  | .................. |
 调用函数 ---- none
 其他说明 ---- none
*/

void CreateLsTable(f3dPoint porg,int LocationPntStyle,int order)
{
	long j,i=0;
	double htext,xspace=0,xxx=0;
	int nn,ttt;
	unsigned int k;
	double len=0.0;
	ads_point pdata,phead,ptil,pend,pu,pd,pu1,pd1,pr1,pl2,pr2,pl3,pr3;
	double fTitleHeight,fItemHeight,fDataHeight,ListWidth=0,xoff,yoff,xofft,yofft,yoffh;
	double thight,xhc,yhc;
	char sty[5],strs[50];
	char sheader[20],slraw[5];
	ads_name lll;
	double textsize = 1.5;
	
	//未使用 暂时注释  wht 10-01-09
	//AcDbObjectId TextStyleId;
	//GetTextStyleId(TextStyleId,1.5);
	//GetTextStyleId(TextStyleId,6.0);
	//GetTextStyleId(TextStyleId,3.0);
	
	fTitleHeight=fLineHeight[0];
	fItemHeight=fLineHeight[1];
	fDataHeight=fLineHeight[2];
	
	if (strcmp(TITLE,"")==0) fTitleHeight=0;
	if (strcmp(header.sName,"")==0) fItemHeight=0;
	
	nn=ELEMENTS(ItemSpace);//条目栏数目
	
	for (i=0;i<nn;i++)
		ListWidth=ListWidth+ItemSpace[i];
	
	ads_point pt;
	switch (LocationPntStyle)
	{
	case 0://左上角
		fDataHeight=-fDataHeight;
		fItemHeight=-fItemHeight; 
		fTitleHeight=-fTitleHeight;
		break;
	case 1://右上角
		fDataHeight=-fDataHeight;
		fItemHeight=-fItemHeight; 
		fTitleHeight=-fTitleHeight;
		porg.x -= ListWidth;
		break;
	case 2://左下角
		break;
	case 3://右下角
		porg.x -=ListWidth;
		break;
	default:
		break;
	}
	
	xofft=ListWidth/2;
	yoff=fDataHeight/2;
	yofft=fTitleHeight/2;
	yoffh=fItemHeight/2;
	
	// 建立标题头
	if (strcmp(TITLE,"")!=0)
	{
		Spoint(ptil,porg.x+xofft,porg.y+yofft,porg.z);
		ads_command(RTSTR,"text",RTSTR,"S",RTSTR,"TITLE_TEXT",
			RTSTR,"j",RTSTR,"mc",RTPOINT,ptil,RTSTR,"0",
			RTSTR,TITLE,RTNONE);
	}
	
	// 建立栏名
	if (strcmp(header.sName,"")!=0)
	{
		for (i = 0; i < nn; i++)
		{
			if (i>0)
				xspace=xspace+ItemSpace[i-1];
			xhc=xspace+ItemSpace[i]/2;
			
			switch (i)
			{
			case 0:
				strcpy(sheader,header.sName);
				break;
			case 1:
				strcpy(sheader,header.sGrade);
				break;
			case 2:
				strcpy(sheader,header.sGuige);
				break;
			case 3:
				strcpy(sheader,header.sSymbol);
				break;
			case 4:
				strcpy(sheader,header.sNum);
				break;
			case 5:
				strcpy(sheader,header.sSubTotalWei);
				break;
			case 6:
				strcpy(sheader,header.sNotes);
				break;
			default:
				break;
			}
			
			len=0.0;
			for (k=0;k<strlen(sheader);k++)
			{
				if (sheader[k]>0)
					len=len+1;
				else
					len=len+0.5;
			}
			htext=4.0;
			
			if (len*4.0>ItemSpace[i])
				htext=ItemSpace[i]/len;
			
			Spoint(phead,porg.x+xhc,porg.y+fTitleHeight+yoffh,0);
			ads_command(RTSTR,"text",RTSTR,"S",RTSTR,"HEADER_TEXT",
				RTSTR,"j",RTSTR,"mc",RTPOINT,phead,RTSTR,"0",
				RTSTR,sheader,RTNONE);
		}
	}
	
	// 写表格正文
	for (j=0;j<nLsRecCount;j++)
	{
		xspace=0;
		for (i = 0; i < nn; i++)
		{
			ttt=slist[i];
			
			switch (ttt)
			{
			case 1:
				strcpy(sty,"ml");
				xoff=1;
				break;
			case 2:
				strcpy(sty,"mc");
				xoff=ItemSpace[i]/2;
				break;
			case 3:
				strcpy(sty,"mr");
				xoff=ItemSpace[i]-1;
				break;
			}
			if (i>0)
				xspace=xspace+ItemSpace[i-1];
			xhc=xspace+xoff;
			yhc=fTitleHeight+fItemHeight+yoff+j*fDataHeight;
			Spoint(pdata,porg.x+xhc,porg.y+yhc,porg.z);
			
			switch (i)
			{
			case 0:
				if(pRecordArr[j+order*nLineNum].type==1)
					strcpy(strs,"螺栓");
				else if(pRecordArr[j+order*nLineNum].type==2)
					strcpy(strs,"垫圈");
				else
					strcpy(strs,"脚钉");
				break;
			case 1:
				strcpy(strs,pRecordArr[j+order*nLineNum].grade);
				break;
			case 2:
				strcpy(strs,pRecordArr[j+order*nLineNum].guige);
				break;
			case 3:
				strcpy(strs,pRecordArr[j+order*nLineNum].symbol);
				break;
			case 4:
				itoa(pRecordArr[j+order*nLineNum].number,strs,10);
				break;
			case 5:
				ads_rtos(pRecordArr[j+order*nLineNum].weight,2,2,strs);
				break;
			case 6:
				strcpy(strs,pRecordArr[j+order*nLineNum].sNotes);
				break;
			}
			
			
			if (i==3)    //螺栓符号-第四列
			{
				if (pRecordArr[j+order*nLineNum].type==1)//螺栓
				{
					char blkname[MAX_PATH];
					strcpy(blkname,sys_path);
					strcat(blkname,strs);
					double scale = 1.0/20;
					ads_command(RTSTR,"insert",RTSTR,blkname,RT3DPOINT,pdata,RTREAL,scale,RTREAL,scale,RTREAL,0.0,0);
				}
				else
				{
					//ads_command(RTSTR,"text",RTSTR,"s",RTSTR,"simplex",RTSTR,"j",RTSTR,sty,RTPOINT,pdata,RTREAL,textsize,RTSTR,"0",RTSTR,strs,RTNONE);
				}
			} 
			else
				ads_command(RTSTR,"text",RTSTR,"S",RTSTR,"Standard",
				RTSTR,"j",RTSTR,sty,RTPOINT,pdata,RTSTR,"0",
				RTSTR,strs,RTNONE);
		}
	}
	for (j=0;j<nNailRecCount;j++)
	{
		xspace=0;
		for (i = 0; i < nn; i++)
		{
			ttt=slist[i];
			
			switch (ttt)
			{
			case 1:
				strcpy(sty,"ml");
				xoff=1;
				break;
			case 2:
				strcpy(sty,"mc");
				xoff=ItemSpace[i]/2;
				break;
			case 3:
				strcpy(sty,"mr");
				xoff=ItemSpace[i]-1;
				break;
			}
			if (i>0)
				xspace=xspace+ItemSpace[i-1];
			xhc=xspace+xoff;
			yhc=fTitleHeight+fItemHeight+yoff+(j+nLsRecCount)*fDataHeight;
			Spoint(pdata,porg.x+xhc,porg.y+yhc,porg.z);
			
			switch (i)
			{
			case 0:
				strcpy(strs,"脚钉");
				break;
			case 1:
				strcpy(strs,nailRecordArr[j+order*nLineNum].grade);
				break;
			case 2:
				strcpy(strs,nailRecordArr[j+order*nLineNum].guige);
				break;
			case 3:
				strcpy(strs,nailRecordArr[j+order*nLineNum].symbol);
				break;
			case 4:
				itoa(nailRecordArr[j+order*nLineNum].number,strs,10);
				break;
			case 5:
				ads_rtos(nailRecordArr[j+order*nLineNum].weight,2,2,strs);
				break;
			case 6:
				strcpy(strs,nailRecordArr[j+order*nLineNum].sNotes);
				break;
			}
			
			
			if (i==3)    //符号-第四列
			{
				char blkname[MAX_PATH];
				strcpy(blkname,sys_path);
				strcat(blkname,"NAIL_FRONT");
				double scale = 1.0/20;
				ads_command(RTSTR,"insert",RTSTR,blkname,RT3DPOINT,pdata,RTREAL,scale,RTREAL,scale,RTREAL,0.0,0);
			} 
			else
				ads_command(RTSTR,"text",RTSTR,"S",RTSTR,"Standard",
				RTSTR,"j",RTSTR,sty,RTPOINT,pdata,RTSTR,"0",
				RTSTR,strs,RTNONE);
		}
	}
	for (j=0;j<nPadRecCount;j++)
	{
		xspace=0;
		for (i = 0; i < nn; i++)
		{
			ttt=slist[i];
			
			switch (ttt)
			{
			case 1:
				strcpy(sty,"ml");
				xoff=1;
				break;
			case 2:
				strcpy(sty,"mc");
				xoff=ItemSpace[i]/2;
				break;
			case 3:
				strcpy(sty,"mr");
				xoff=ItemSpace[i]-1;
				break;
			}
			if (i>0)
				xspace=xspace+ItemSpace[i-1];
			xhc=xspace+xoff;
			yhc=fTitleHeight+fItemHeight+yoff+(j+nLsRecCount+nPadRecCount)*fDataHeight;
			Spoint(pdata,porg.x+xhc,porg.y+yhc,porg.z);
			
			switch (i)
			{
			case 0:
				strcpy(strs,"垫圈");
				break;
			case 1:
				strcpy(strs,padRecordArr[j+order*nLineNum].grade);
				break;
			case 2:
				strcpy(strs,padRecordArr[j+order*nLineNum].guige);
				break;
			case 3:
				strcpy(strs,padRecordArr[j+order*nLineNum].symbol);
				break;
			case 4:
				itoa(padRecordArr[j+order*nLineNum].number,strs,10);
				break;
			case 5:
				ads_rtos(padRecordArr[j+order*nLineNum].weight,2,2,strs);
				break;
			case 6:
				strcpy(strs,padRecordArr[j+order*nLineNum].sNotes);
				break;
			}
			
			
			if (i==3)    //螺栓符号-第四列
			{
				char blkname[MAX_PATH];
				strcpy(blkname,sys_path);
				strcat(blkname,strs);
				double scale = 1.0/20;
				//ads_command(RTSTR,"insert",RTSTR,blkname,RT3DPOINT,pdata,RTREAL,scale,RTREAL,scale,RTREAL,0.0,0);
			} 
			else
				ads_command(RTSTR,"text",RTSTR,"S",RTSTR,"Standard",
				RTSTR,"j",RTSTR,sty,RTPOINT,pdata,RTSTR,"0",
				RTSTR,strs,RTNONE);
		}
	}
	
	// 生成表格线(竖线)
	thight=nRecCount1*fDataHeight+fTitleHeight+fItemHeight;
	Spoint(pend,porg.x,porg.y+thight,porg.z);
	Cpoint(pt,porg);//点类型转换
	ads_command(RTSTR,"line",RTPOINT,pt,RTPOINT,pend,RTSTR,"",RTNONE);
	for (i = 0; i < nn; i++)
	{
		xxx=xxx+ItemSpace[i];
		Spoint(pu,porg.x+xxx,porg.y+fTitleHeight,porg.z);
		Spoint(pd,porg.x+xxx,porg.y+thight,porg.z);
		ads_command(RTSTR,"line",RTPOINT,pu,RTPOINT,pd,RTSTR,"",RTNONE);
	}
	Spoint(pu1,porg.x+ListWidth,porg.y,porg.z);
	Spoint(pd1,porg.x+ListWidth,porg.y+thight,porg.z);
	ads_command(RTSTR,"line",RTPOINT,pu1,RTPOINT,pd1,RTSTR,"",RTNONE);
	
	// 生成表格线(横线)
	Spoint(pr1,porg.x+ListWidth,porg.y,porg.z);
	Spoint(pl2,porg.x,porg.y+fTitleHeight,porg.z);
	Spoint(pr2,porg.x+ListWidth,porg.y+fTitleHeight,porg.z);
	Spoint(pl3,porg.x,porg.y+fTitleHeight+fItemHeight,porg.z);
	Spoint(pr3,porg.x+ListWidth,porg.y+fTitleHeight+fItemHeight,porg.z);
	Cpoint(pt,porg);//点类型转换
	ads_command(RTSTR,"line",RTPOINT,pt,RTPOINT,pr1,RTSTR,"",RTNONE);
	ads_command(RTSTR,"line",RTPOINT,pl2,RTPOINT,pr2,RTSTR,"",RTNONE);
	ads_command(RTSTR,"line",RTPOINT,pl3,RTPOINT,pr3,RTSTR,"",RTNONE);
	ads_entlast(lll);
	ltoa(1+nRecCount1,slraw,10);
	ads_command(RTSTR,"array",RTENAME,lll,RTSTR,"",RTSTR,"r",RTSTR,slraw,RTSTR,"1",RTREAL,fDataHeight,RTNONE);
}
static void AppendPadRec(DIANQUAN_STRU pad)
{
	int k;
	char guige[10];
	if(pad.N<=0)
		return;

	for(k=0;k<nPadRecCount;k++)
	{
		sprintf(guige,"-%dX%.1f",pad.thick,pad.d);
		if(strcmp(guige,pad_record[k].guige)==0)	//已有型号的垫圈
		{
			pad_record[k].number += pad.N;
			pad_record[k].weight = 
				pad_record[k].number*pad_record[k].unit_wei;
			return;
		}
	}
	
	{	//新型号垫圈
		pad_record[nPadRecCount].number = pad.N;
		sprintf(pad_record[nPadRecCount].guige,"-%dX.1f",pad.thick,pad.d);
		sprintf(pad_record[nPadRecCount].grade,"Q235");
		if(pad.thick<=2)
		{
			pad_record[nPadRecCount].unit_wei = 0.004685;
			pad_record[nPadRecCount].weight = pad.N*0.004685;//立即数由规程获得
		}
		else if(pad.thick<=3)
		{
			if(pad.d<=17.5)
			{
				pad_record[nPadRecCount].unit_wei = 0.01065;
				pad_record[nPadRecCount].weight = 
				pad.N*0.01065;//立即数由规程获得
			}
			else
			{
				pad_record[nPadRecCount].unit_wei = 0.01637;
				pad_record[nPadRecCount].weight = 
				pad.N*0.01637;//立即数由规程获得
			}
		}
		else
		{
			pad_record[nPadRecCount].unit_wei = 0.03108;
			pad_record[nPadRecCount].weight = 
			pad.N*0.03108;//立即数由规程获得
		}
		nPadRecCount+=1;
	}
}
// 按顺序获取螺栓记录信息
int GetLsRecord()
{
	int j,k,ll_list,ll_ls;
	fTowerAtom *pAtom;
	CJg *pJg;
	CNode *pNode;
	CJdb *pJdb;
	CLsList *pLsList;
	CLs *pLs;
	nRecCount = nLsRecCount = nNailRecCount = nPadRecCount = 0;
	LS_RECORD_STRU ls_record[30],nail_record[10];
	//下面一段以后需要另处理暂时注掉 WJH--01.07.07
	/*ll_list = Ta.Ls.GetNodeNum();
	for(int i=0;i<ll_list;i++)
	{
		pLsList = &Ta.Ls[i];
		pAtom = Ta.FromHandle(pLsList->attach_part_handle);
		if(pAtom!=NULL&&pAtom->atom_type==AtomType::taJg)
		{
			pJg = (CJg*)pAtom;
			if(pJg->belong_seg_i!=g_nCurMapSegNo)
				continue;
		}
		else if(pAtom!=NULL&&pAtom->atom_type==AtomType::taNode)
		{
			pNode = (CNode*)pAtom;
			if(pNode->belong_seg_i!=g_nCurMapSegNo)
				continue;
		}
		else if(pAtom!=NULL&&pAtom->atom_type==AtomType::taJdb)
		{
			pJdb =  (CJdb*)pAtom;
			pNode = Ta.FromNodeHandle(pJdb->base_node_handle);
			if(pNode->belong_seg_i!=g_nCurMapSegNo)
				continue;
		}
		ll_ls = pLsList->GetNodeNum();
		for(j=0;j<ll_ls;j++)
		{
			pLs = &pLsList->GetByIndex(j);
			if(!pLs->original_or_copy)//副本
				continue;
			AppendPadRec(pLs->DianQuan);
			if(nRecCount==0)
			{
				nRecCount+=1;
				if(strlen(pLs->guige)>=10)	//纯粮粹防错用,以后应去掉 WJH-01.05.19
					FindLsGuiGe(pLs->guige,pLs->d,pLs->L0);
				if(!pLs->bolt_or_footnail)//螺栓(FALSE)脚钉(TRUE)
				{
					nLsRecCount++;
					strcpy(ls_record[0].guige, pLs->guige);
					ls_record[0].number = 1;
					strcpy(ls_record[0].symbol,ls_record[0].guige);
					ls_record[0].type = 1;//螺栓
					if(pLs->d<=16)
						sprintf(ls_record[0].grade,"%3.1f",4.8);
					else if(pLs->d<=20)
						sprintf(ls_record[0].grade,"%3.1f",6.8);
					else	//这里有问题,本部分仍有6.8级别的螺栓
						sprintf(ls_record[0].grade,"%3.1f",8.8);
					ls_record[0].weight = FindLsWei(pLs->guige)*ls_record[0].number;
					strcpy(ls_record[0].sNotes,"");
				}
				else	//脚钉
				{
					nNailRecCount++;
					strcpy(nail_record[0].guige, pLs->guige);
					nail_record[0].number = 1;
					strcpy(nail_record[0].symbol,nail_record[0].guige);
					nail_record[0].type = 2;//螺栓
					if(pLs->d<=16)
						sprintf(nail_record[0].grade,"%3.1f",4.8);
					else if(pLs->d<=20)
						sprintf(nail_record[0].grade,"%3.1f",6.8);
					else	//这里有问题,本部分仍有6.8级别的螺栓
						sprintf(nail_record[0].grade,"%3.1f",8.8);
					nail_record[0].weight = FindLsWei(pLs->guige)*nail_record[0].number;
					strcpy(nail_record[0].sNotes,"");
				}
			}
			else
			{
				if(!pLs->bolt_or_footnail)//螺栓
				{
					for(k=0;k<nLsRecCount;k++)
					{
						if(strcmp(pLs->guige,ls_record[k].guige)==0)
						{
							ls_record[k].weight=ls_record[k].weight*
								(ls_record[k].number+1)/ls_record[k].number;
							ls_record[k].number++;
							break;	//重新开始循环
						}
					}
				
					if(k==nLsRecCount)
					{	//新型号的螺栓
						strcpy(ls_record[nLsRecCount].guige, pLs->guige);
						ls_record[nLsRecCount].number = 1;
						strcpy(ls_record[nLsRecCount].symbol,ls_record[nLsRecCount].guige);
						ls_record[nLsRecCount].type = 1;//螺栓
						if(pLs->d<=16)
							sprintf(ls_record[k].grade,"%3.1f",4.8);
						else if(pLs->d<=20)
							sprintf(ls_record[k].grade,"%3.1f",6.8);
						else
							sprintf(ls_record[k].grade,"%3.1f",8.8);
						ls_record[nLsRecCount].weight =
							FindLsWei(pLs->guige)*ls_record[nLsRecCount].number;
						strcpy(ls_record[nLsRecCount].sNotes,"");
						nLsRecCount++;
						nRecCount++;
					}
				}
				else	//脚钉
				{
					for(k=0;k<nNailRecCount;k++)
					{
						if(strcmp(pLs->guige,nail_record[k].guige)==0)
						{
							nail_record[k].weight=nail_record[k].weight*
								(nail_record[k].number+1)/nail_record[k].number;
							nail_record[k].number++;
							break;	//重新开始循环
						}
					}
				
					if(k==nNailRecCount)
					{	//新型号的螺栓或脚钉
						strcpy(nail_record[nNailRecCount].guige, pLs->guige);
						nail_record[nNailRecCount].number = 1;
						strcpy(nail_record[nNailRecCount].symbol,nail_record[nNailRecCount].guige);
						nail_record[nNailRecCount].type = 2;//脚钉
						if(pLs->d<=16)
							sprintf(nail_record[k].grade,"%3.1f",4.8);
						else if(pLs->d<=20)
							sprintf(nail_record[k].grade,"%3.1f",6.8);
						else
							sprintf(nail_record[k].grade,"%3.1f",8.8);
						nail_record[nNailRecCount].weight =
							FindLsWei(pLs->guige)*nail_record[nNailRecCount].number;
						strcpy(nail_record[nNailRecCount].sNotes,"");
						nNailRecCount++;
						nRecCount++;
					}
				}
			}
		}
	}*/
	pRecordArr = new LS_RECORD_STRU[nLsRecCount];
	nailRecordArr = new LS_RECORD_STRU[nNailRecCount];
	padRecordArr = new PAD_RECORD_STRU[nPadRecCount];
	memcpy(pRecordArr,ls_record,nLsRecCount*sizeof(LS_RECORD_STRU));
	memcpy(nailRecordArr,nail_record,nNailRecCount*sizeof(LS_RECORD_STRU));
	memcpy(padRecordArr,pad_record,nPadRecCount*sizeof(PAD_RECORD_STRU));

	return nRecCount;
}
