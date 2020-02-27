#include"stdafx.h"
#include"LogFile.h"
#include"f_ent.h"
#include"f_alg_fun.h"
#include"DxfFile.h"
#include"functiontest.h"

//椭圆上求切函数的测试
void TestArcTangent()
{
	//椭圆的创建
	f3dArcLine arcLine;
	f3dPoint centerPt(0,0,0),workNorm(0,0,1),columnNorm(0,1,1),arcStartPt(50,5,0),arcEndPt(0,80,0);
	double radius=50.0;
	arcLine.CreateEllipse(centerPt,arcStartPt,arcEndPt,columnNorm,workNorm,radius);

	//椭圆的绘制
	CDxfFile  dxfFile;
	dxfFile.OpenFile("D:\\测试\\切线测试.dxf");
	double draw_angle=Pi/50;
	f3dPoint startPt,endPt;
	startPt=arcLine.PositionInAngle(0);
	for(int i=1;i<101;i++)
	{
		double ang=draw_angle*i;
		endPt=arcLine.PositionInAngle(ang);
		dxfFile.NewLine(startPt,endPt);
		startPt=endPt;
	}
	//取不同的角度绘制两条线（切线和从圆心引出的线）
	double angle[8]={0,50,90,110,180,190,270,290};
	for(int j=0;j<8;j++)
	{
		angle[j]=angle[j]*RADTODEG_COEF;	
		//得到切线向量
		GEPOINT receVector=arcLine.TangentVecInAngle(angle[j]);
		normalize(receVector);
		//得到切点
		f3dPoint tangenPt=arcLine.PositionInAngle(angle[j]);	
		dxfFile.NewLine(centerPt,tangenPt);
		f3dPoint tangstartPt=tangenPt+30*receVector;
		f3dPoint tangendPt=tangenPt-30*receVector;
		dxfFile.NewLine(tangstartPt,tangendPt);
	}
	//利用logFile文件提示
	logerr.Log("切线测试中，如果两线相切，那么测试正确！");
	logerr.ShowToScreen();
	dxfFile.CloseFile();
}

//圆与圆相交功能函数的测试
struct Info
{
	f3dPoint arc_center1;
	f3dPoint arc_center2;
	double radius1;
	double radius2;
	Info()
	{
		arc_center1.Set(0,0,0);
		arc_center2.Set(0,0,0);
		radius1=0;
		radius2=0;
	}
}CIRCLEINFO[5];
void TestInt2dcc( )
{
	//两个圆包含关系
	CIRCLEINFO[0].arc_center2.Set(0,0,0),CIRCLEINFO[0].arc_center1.Set(0,0,0);
	CIRCLEINFO[0].radius1=50.0,CIRCLEINFO[0].radius2=30.0;
	//两个圆相离关系
	CIRCLEINFO[1].arc_center2.Set(0,0,0),CIRCLEINFO[1].arc_center1.Set(0,90,0);
	CIRCLEINFO[1].radius1=50.0,CIRCLEINFO[1].radius2=30.0;
	//两圆内切关系
	CIRCLEINFO[2].arc_center2.Set(0,0,0),CIRCLEINFO[2].arc_center1.Set(20,0,0);
	CIRCLEINFO[2].radius1=50.0,CIRCLEINFO[2].radius2=30;
	//两圆外切关系
	CIRCLEINFO[3].arc_center2.Set(0,0,0),CIRCLEINFO[3].arc_center1.Set(0,70,0);
	CIRCLEINFO[3].radius1=50.0,CIRCLEINFO[3].radius2=20;
	//两圆相交关系
	CIRCLEINFO[4].arc_center2.Set(0,0,0),CIRCLEINFO[4].arc_center1.Set(0,60,0);
	CIRCLEINFO[4].radius1=50.0,CIRCLEINFO[4].radius2=50.0;
	CDxfFile   dxfFile;
	dxfFile.OpenFile("D:\\测试\\圆与圆交点个数测试.dxf");	
	int nReceive[6];
	//两圆的绘制
	f3dPoint inters2d1Pt,inters2d2Pt;	
	for(int j=0;j<5;j++)
	{
		nReceive[j]=Int2dcc( inters2d1Pt, inters2d2Pt,CIRCLEINFO[j].arc_center1, CIRCLEINFO[j].radius1, CIRCLEINFO[j].arc_center2,CIRCLEINFO[j].radius2);
		if(1<j+1&&j+1<6)
		{
			CIRCLEINFO[j].arc_center1.x+=100*j;
			CIRCLEINFO[j].arc_center2.x+=100*j;
		}
		if(j+1>5&&j+1<11)
		{
			CIRCLEINFO[j].arc_center2.y+=200;
			CIRCLEINFO[j].arc_center1.y+=200;
		}
		if(j+1>6&&j+1<11)
		{
			CIRCLEINFO[j].arc_center1.x+=100*(j-5);
			CIRCLEINFO[j].arc_center2.x+=100*(j-5);
		}
		if(j+1>10&&j+1<16)
		{
			CIRCLEINFO[j].arc_center1.y+=400;
			CIRCLEINFO[j].arc_center2.y+=400;
		}
		if(j+1>11&&j+1<16)
		{
			CIRCLEINFO[j].arc_center1.x+=100*(j-10);
			CIRCLEINFO[j].arc_center2.x+=100*(j-10);
		}
		dxfFile.NewCircle(CIRCLEINFO[j].arc_center1,CIRCLEINFO[j].radius1);
		dxfFile.NewCircle(CIRCLEINFO[j].arc_center2,CIRCLEINFO[j].radius2);
	}
	//log文件提示
	logerr.Log("两个圆包含关系交点的个数为：%d",nReceive[0]);
	logerr.Log("两个圆相离关系交点的个数为：%d",nReceive[1]);
	logerr.Log("两圆内切关系交点的个数为：%d",nReceive[2]);
	logerr.Log("两圆外切关系交点的个数为：%d",nReceive[3]);
	logerr.Log("两圆相交关系交点的个数为：%d",nReceive[4]);
	dxfFile.CloseFile();
	logerr.ShowToScreen();
}

//直线和椭圆求交功能函数的测试
struct Pt
{
	f3dPoint lineStart;
	f3dPoint lineEnd;
	double arc_angle;
	Pt()
	{
		lineStart.Set(0,0,0);
		lineEnd.Set(0,0,0);
		arc_angle=0;
	}
}LINE_ANGLE[10];
void TestInt2dle( )
{
	//有0个交点
	LINE_ANGLE[0].lineStart.Set(0,0,0);
	LINE_ANGLE[0].lineEnd.Set(0,50,0);
	LINE_ANGLE[0].arc_angle=0;
	//有1个交点
	LINE_ANGLE[1].lineStart.Set(0,0,0);
	LINE_ANGLE[1].lineEnd.Set(50,20,0);
	LINE_ANGLE[1].arc_angle=50;
	//有1个交点
	LINE_ANGLE[2].lineStart.Set(0,0,0);
	LINE_ANGLE[2].lineEnd.Set(0,90,0);
	LINE_ANGLE[2].arc_angle=90;
	//有2个交点
	LINE_ANGLE[3].lineStart.Set(40,50,0);
	LINE_ANGLE[3].lineEnd.Set(-40,50,0);
	LINE_ANGLE[3].arc_angle=160;
	//有1个交点
	LINE_ANGLE[4].lineStart.Set(-50,-70,0);
	LINE_ANGLE[4].lineEnd.Set(50,70,0);
	LINE_ANGLE[4].arc_angle=180;
	//有1个交点
	LINE_ANGLE[5].lineStart.Set(0,0,0);
	LINE_ANGLE[5].lineEnd.Set(10,70,0);
	LINE_ANGLE[5].arc_angle=200;
	//有2个交点
	LINE_ANGLE[6].lineStart.Set(-30,80,0);
	LINE_ANGLE[6].lineEnd.Set(-30,-80,0);
	LINE_ANGLE[6].arc_angle=270;
	//有2个交点
	LINE_ANGLE[7].lineStart.Set(-40,60,0);
	LINE_ANGLE[7].lineEnd.Set(30,-90,0);
	LINE_ANGLE[7].arc_angle=300;
	//有2个交点
	LINE_ANGLE[8].lineStart.Set(-45,50,0);
	LINE_ANGLE[8].lineEnd.Set(70,30,0);
	LINE_ANGLE[8].arc_angle=360;
	//有0个交点
	LINE_ANGLE[9].lineStart.Set(0,0,0);
	LINE_ANGLE[9].lineEnd.Set(10,10,0);
	LINE_ANGLE[9].arc_angle=360;
	//椭圆的创建
	f3dArcLine arcLine;
	double radius=40,arc_angle=50;
	f3dPoint center(0,0,0),workNorm(0,0,1),columnNorm(0,1,1),arcStartPt(50,0,0),arcEndPt(-50,0,0),inters2d1Pt,inters2d2Pt,inters2d3Pt,inters2d4Pt;
	GEPOINT long_axis(3.4637098282877424e-015,68.284271247461902,11.715728752538091);	
	arcLine.CreateEllipse(center,arcStartPt,arcEndPt,columnNorm,workNorm,radius);
	//直线的绘制
	CDxfFile  dxfFile;
	dxfFile.OpenFile("D:\\测试\\直线与椭圆交点个数测试.dxf");
	int nReceive[10];            //用数组接收交点的个数
	int nPtNum[10]={0,1,1,2,1,1,2,2,2,0};
	for(int j=0;j<10;j++)
	{
		nReceive[j]=Int2dle(inters2d1Pt,inters2d2Pt,LINE_ANGLE[j].lineStart,LINE_ANGLE[j].lineEnd,center,arcStartPt,radius,LINE_ANGLE[j].arc_angle,long_axis);
		if(1<j+1&&j+1<6)
		{
			LINE_ANGLE[j].lineStart.x+=200*j;
			LINE_ANGLE[j].lineEnd.x+=200*j;
		}
		if(j+1>5&&j+1<11)
		{
			LINE_ANGLE[j].lineStart.y+=200;
			LINE_ANGLE[j].lineEnd.y+=200;
		}
		if(j+1>6&&j+1<11)
		{
			LINE_ANGLE[j].lineStart.x+=200*(j-5);
			LINE_ANGLE[j].lineEnd.x+=200*(j-5);
		}
		if(j+1>10&&j+1<16)
		{
			LINE_ANGLE[j].lineStart.y+=400;
			LINE_ANGLE[j].lineEnd.y+=400;
		}
		if(j+1>11&&j+1<16)
		{
			LINE_ANGLE[j].lineStart.x+=400*(j-10);
			LINE_ANGLE[j].lineEnd.x+=400*(j-10);
		}
		dxfFile.NewLine(LINE_ANGLE[j].lineStart,LINE_ANGLE[j].lineEnd);
		//log文件提示
		if(j==0)
			logerr.Log("图形的顺序是从左往右，从下往上：");
		if(nPtNum[j]!=nReceive[j])
			logerr.Log("测试错误！\t 弧度为%f的应为个数和实际测试个数分别为 ：%d\t%d",LINE_ANGLE[j].arc_angle,nPtNum[j],nReceive[j]);
		else
			logerr.Log("测试正确！\t 弧度为%f的应为个数和实际测试个数分别为 ：%d\t%d",LINE_ANGLE[j].arc_angle,nPtNum[j],nReceive[j]);
	}
	//椭圆弧段的绘制
	for(int j=0;j<10;j++)
	{
		double draw_angle=LINE_ANGLE[j].arc_angle/100*RADTODEG_COEF;
		f3dPoint startPt,endPt;
		startPt=arcLine.PositionInAngle(0);
		//将弧线分成100段，利用直线绘制成图 
		for(int i=1;i<101;i++)
		{
			double ang=draw_angle*i;
			endPt=arcLine.PositionInAngle(ang);
			if(1<j+1&&j+1<6)
			{
				if(i==1)
					startPt.x+=200*j;
				endPt.x+=200*j;
			}
			if(j+1>5&&j+1<11)
			{
				if(i==1)
					startPt.y+=200;
				endPt.y+=200;
			}
			if(j+1>6&&j+1<11)
			{
				if(i==1)
					startPt.x+=200*(j-5);
				endPt.x+=200*(j-5);
			}
			if(j+1>10&&j+1<16)
			{
				if(i==1)
			 		startPt.y+=400;
			 	endPt.y+=400;
			}
			if(j+1>11&&j+1<16)
			{
				if(i==1)
			 		startPt.x+=400*(j-10);
			 	endPt.x+=400*(j-10);
			}
			dxfFile.NewLine(startPt,endPt);
			startPt=endPt;
		}

	}
	logerr.ShowToScreen();
	dxfFile.CloseFile();
}
//椭圆和圆求交功能函数测试
static double CalEllipseY(double max_edge,double min_edge,double& x,int index)
{
	double fY;
	double fA=SQR(x)/SQR(max_edge);
	double fB=(1-fA);
	fY=min_edge*sqrt(fB);
	int iSign=1;
	if(index==3 || index==4)
		iSign=-1;
	if(index==2 || index==3)
		x*=-1;
	return iSign*fY;
}
struct Infor
{
	f3dPoint arc_center;
	double radius;
	Infor()
	{
		arc_center.Set(0,0,0);
		radius=0;
	}
}CIRCLE_INFO[10];

void TestInt2dce( )
{
	//椭圆与圆包含关系
	CIRCLE_INFO[0].arc_center.Set(0,0,0);
	CIRCLE_INFO[0].radius=30;
	//椭圆与圆相离关系
	CIRCLE_INFO[1].arc_center.Set(0,80,0);
	CIRCLE_INFO[1].radius=20.0;
	//圆包含椭圆内切（2点）关系
	CIRCLE_INFO[2].arc_center.Set(0,0,0);
	CIRCLE_INFO[2].radius=100;
	//圆包含椭圆内切（1点）关系
	CIRCLE_INFO[3].arc_center.Set(30,0,0);
	CIRCLE_INFO[3].radius=130;
	//椭圆包含圆内切（2点）关系
	CIRCLE_INFO[4].arc_center.Set(0,0,0);
	CIRCLE_INFO[4].radius=50;
	//椭圆包含圆内切（1点）关系
	CIRCLE_INFO[5].arc_center.Set(0,20,0);
	CIRCLE_INFO[5].radius=80;
	//椭圆与圆相交（2点）关系
	CIRCLE_INFO[6].arc_center.Set(0,30,0);
	CIRCLE_INFO[6].radius=40.0;
	//椭圆与圆相交（3点）关系
	CIRCLE_INFO[7].arc_center.Set(0,20,0);
	CIRCLE_INFO[7].radius=70;
	//椭圆与圆相交（4点）关系
	CIRCLE_INFO[8].arc_center.Set(0,0,0);
	CIRCLE_INFO[8].radius=70;
	//椭圆与圆外切关系
	CIRCLE_INFO[9].arc_center.Set(100,0,0);
	CIRCLE_INFO[9].radius=50;


	f3dPoint inters2d1Pt,inters2d2Pt,inters2d3Pt,inters2d4Pt;
	f3dPoint center(0,0,0),long_axis(100,0,0);
	double short_R=50,long_R=100;

	//圆的绘制
	CDxfFile  dxfFile;
	dxfFile.OpenFile("D:\\测试\\圆与椭圆交点个数测试.dxf");
	int nReceive[10]; //将交点的个数存到数组中
	for(int j=0;j<10;j++)
	{
		nReceive[j]=Int2dce(inters2d1Pt,inters2d2Pt,inters2d3Pt,inters2d4Pt,CIRCLE_INFO[j].arc_center,CIRCLE_INFO[j].radius,center,short_R,long_axis);
		if(1<j+1&&j+1<6)
		{
			CIRCLE_INFO[j].arc_center.x+=300*j;
		}
		if(j+1>5&&j+1<11)
		{
			CIRCLE_INFO[j].arc_center.y+=300;
		}
		if(j+1>6&&j+1<11)
		{
			CIRCLE_INFO[j].arc_center.x+=300*(j-5);
		}
		if(j+1>10&&j+1<16)
		{
			CIRCLE_INFO[j].arc_center.y+=600;
		}
		if(j+1>11&&j+1<16)
		{
			CIRCLE_INFO[j].arc_center.x+=300*(j-10);
		}
		dxfFile.NewCircle(CIRCLE_INFO[j].arc_center,CIRCLE_INFO[j].radius);
	}

	//椭圆的绘制
	int nSlices=50;                     //绘制50份
	double fDist=long_R/50;
	double x=0,y=0;
	f3dPoint startPt,endPt;
	for(int j=0;j<10;j++)
	{
		startPt.Set(long_R,0,0);             //开始绘制点
		for(int index=1;index<5;index++)
		{
			for(int i=1;i<=nSlices;i++)
			{
				if(index==1 || index==3)
					x=long_R-i*fDist;
				else if(index==2 || index==4)
					x=i*fDist;
				//
				y=CalEllipseY(long_R,short_R,x,index);
				endPt.Set(x,y,0);
				if(1<j+1&&j+1<6)
				{
					if(i==1&&index==1)
					{
						startPt.x+=300*j;
					}
					endPt.x+=300*j;
				}
				if(j+1>5&&j+1<11)
				{
					if(i==1&&index==1)
						startPt.y+=300;
					endPt.y+=300;
				}
				if(j+1>6&&j+1<11)
				{
					if(i==1&&index==1)
						startPt.x+=300*(j-5);
					endPt.x+=300*(j-5);
				}
				if(j+1>10&&j+1<16)
				{
					if(i==1&&index==1)
				 		startPt.y+=600;
				 	endPt.y+=600;
				}
				if(j+1>11&&j+1<16)
				{
					if(i==1&&index==1)
				 		startPt.x+=300*(j-10);
				 	endPt.x+=300*(j-10);
				}
				dxfFile.NewLine(startPt,endPt);
				startPt=endPt;
			}

		}
	}
	//log文件提示
	logerr.Log("椭圆与圆包含关系:\t\t%d\n椭圆与圆相离关系:\t\t%d\n圆包含椭圆内切（2点）关系:\t%d",nReceive[0],nReceive[1],nReceive[2]);
	logerr.Log("圆包含椭圆内切（1点）关系:\t%d\n椭圆包含圆内切（2点）关系:\t%d\n椭圆包含圆内切（1点）关系:\t%d",nReceive[3],nReceive[4],nReceive[5]);
	logerr.Log("椭圆与圆相交（2点）关系:\t%d \n椭圆与圆相交（3点）关系:\t%d\n椭圆与圆相交（4点）关系:\t%d",nReceive[6],nReceive[7],nReceive[8]);
	logerr.Log("椭圆与圆外切关系:\t\t%d",nReceive[9]);
	logerr.ShowToScreen();
	dxfFile.CloseFile();
}
	
	

	////椭圆的绘制
	//for(int j=0;j<10;j++)
	//{
	//	//将弧线分成100段，利用直线绘制成图 
	//	double draw_angle=Pi/50;
	//	f3dPoint startPt,endPt;
	//	startPt=arcLine.PositionInAngle(0);
	//	for(int i=1;i<101;i++)
	//	{
	//		double ang=draw_angle*i;
	//		endPt=arcLine.PositionInAngle(ang);
	//		if(1<j+1&&j+1<6)
	//		{
	//			if(i==1)
	//				startPt.x+=200*j;
	//			endPt.x+=200*j;
	//		}
	//		if(j+1>5&&j+1<11)
	//		{
	//			if(i==1)
	//				startPt.y+=200;
	//			endPt.y+=200;
	//		}
	//		if(j+1>6&&j+1<11)
	//		{
	//			if(i==1)
	//				startPt.x+=200*(j-5);
	//			endPt.x+=200*(j-5);
	//		}
	//		if(j+1>10&&j+1<16)
	//		{
	//			if(i==1)
	//		 		startPt.y+=400;
	//		 	endPt.y+=400;
	//		}
	//		if(j+1>11&&j+1<16)
	//		{
	//			if(i==1)
	//		 		startPt.x+=400*(j-10);
	//		 	endPt.x+=400*(j-10);
	//		}
	//		dxfFile.NewLine(startPt,endPt);
	//		startPt=endPt;
	//	}		
	//}
	
	//dxfFile.CloseFile();

	//
//
//}

