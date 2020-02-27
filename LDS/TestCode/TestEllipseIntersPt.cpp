#include"stdafx.h"
#include"LogFile.h"
#include"f_ent.h"
#include"f_alg_fun.h"
#include"DxfFile.h"
#include"functiontest.h"

//��Բ�����к����Ĳ���
void TestArcTangent()
{
	//��Բ�Ĵ���
	f3dArcLine arcLine;
	f3dPoint centerPt(0,0,0),workNorm(0,0,1),columnNorm(0,1,1),arcStartPt(50,5,0),arcEndPt(0,80,0);
	double radius=50.0;
	arcLine.CreateEllipse(centerPt,arcStartPt,arcEndPt,columnNorm,workNorm,radius);

	//��Բ�Ļ���
	CDxfFile  dxfFile;
	dxfFile.OpenFile("D:\\����\\���߲���.dxf");
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
	//ȡ��ͬ�ĽǶȻ��������ߣ����ߺʹ�Բ���������ߣ�
	double angle[8]={0,50,90,110,180,190,270,290};
	for(int j=0;j<8;j++)
	{
		angle[j]=angle[j]*RADTODEG_COEF;	
		//�õ���������
		GEPOINT receVector=arcLine.TangentVecInAngle(angle[j]);
		normalize(receVector);
		//�õ��е�
		f3dPoint tangenPt=arcLine.PositionInAngle(angle[j]);	
		dxfFile.NewLine(centerPt,tangenPt);
		f3dPoint tangstartPt=tangenPt+30*receVector;
		f3dPoint tangendPt=tangenPt-30*receVector;
		dxfFile.NewLine(tangstartPt,tangendPt);
	}
	//����logFile�ļ���ʾ
	logerr.Log("���߲����У�����������У���ô������ȷ��");
	logerr.ShowToScreen();
	dxfFile.CloseFile();
}

//Բ��Բ�ཻ���ܺ����Ĳ���
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
	//����Բ������ϵ
	CIRCLEINFO[0].arc_center2.Set(0,0,0),CIRCLEINFO[0].arc_center1.Set(0,0,0);
	CIRCLEINFO[0].radius1=50.0,CIRCLEINFO[0].radius2=30.0;
	//����Բ�����ϵ
	CIRCLEINFO[1].arc_center2.Set(0,0,0),CIRCLEINFO[1].arc_center1.Set(0,90,0);
	CIRCLEINFO[1].radius1=50.0,CIRCLEINFO[1].radius2=30.0;
	//��Բ���й�ϵ
	CIRCLEINFO[2].arc_center2.Set(0,0,0),CIRCLEINFO[2].arc_center1.Set(20,0,0);
	CIRCLEINFO[2].radius1=50.0,CIRCLEINFO[2].radius2=30;
	//��Բ���й�ϵ
	CIRCLEINFO[3].arc_center2.Set(0,0,0),CIRCLEINFO[3].arc_center1.Set(0,70,0);
	CIRCLEINFO[3].radius1=50.0,CIRCLEINFO[3].radius2=20;
	//��Բ�ཻ��ϵ
	CIRCLEINFO[4].arc_center2.Set(0,0,0),CIRCLEINFO[4].arc_center1.Set(0,60,0);
	CIRCLEINFO[4].radius1=50.0,CIRCLEINFO[4].radius2=50.0;
	CDxfFile   dxfFile;
	dxfFile.OpenFile("D:\\����\\Բ��Բ�����������.dxf");	
	int nReceive[6];
	//��Բ�Ļ���
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
	//log�ļ���ʾ
	logerr.Log("����Բ������ϵ����ĸ���Ϊ��%d",nReceive[0]);
	logerr.Log("����Բ�����ϵ����ĸ���Ϊ��%d",nReceive[1]);
	logerr.Log("��Բ���й�ϵ����ĸ���Ϊ��%d",nReceive[2]);
	logerr.Log("��Բ���й�ϵ����ĸ���Ϊ��%d",nReceive[3]);
	logerr.Log("��Բ�ཻ��ϵ����ĸ���Ϊ��%d",nReceive[4]);
	dxfFile.CloseFile();
	logerr.ShowToScreen();
}

//ֱ�ߺ���Բ�󽻹��ܺ����Ĳ���
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
	//��0������
	LINE_ANGLE[0].lineStart.Set(0,0,0);
	LINE_ANGLE[0].lineEnd.Set(0,50,0);
	LINE_ANGLE[0].arc_angle=0;
	//��1������
	LINE_ANGLE[1].lineStart.Set(0,0,0);
	LINE_ANGLE[1].lineEnd.Set(50,20,0);
	LINE_ANGLE[1].arc_angle=50;
	//��1������
	LINE_ANGLE[2].lineStart.Set(0,0,0);
	LINE_ANGLE[2].lineEnd.Set(0,90,0);
	LINE_ANGLE[2].arc_angle=90;
	//��2������
	LINE_ANGLE[3].lineStart.Set(40,50,0);
	LINE_ANGLE[3].lineEnd.Set(-40,50,0);
	LINE_ANGLE[3].arc_angle=160;
	//��1������
	LINE_ANGLE[4].lineStart.Set(-50,-70,0);
	LINE_ANGLE[4].lineEnd.Set(50,70,0);
	LINE_ANGLE[4].arc_angle=180;
	//��1������
	LINE_ANGLE[5].lineStart.Set(0,0,0);
	LINE_ANGLE[5].lineEnd.Set(10,70,0);
	LINE_ANGLE[5].arc_angle=200;
	//��2������
	LINE_ANGLE[6].lineStart.Set(-30,80,0);
	LINE_ANGLE[6].lineEnd.Set(-30,-80,0);
	LINE_ANGLE[6].arc_angle=270;
	//��2������
	LINE_ANGLE[7].lineStart.Set(-40,60,0);
	LINE_ANGLE[7].lineEnd.Set(30,-90,0);
	LINE_ANGLE[7].arc_angle=300;
	//��2������
	LINE_ANGLE[8].lineStart.Set(-45,50,0);
	LINE_ANGLE[8].lineEnd.Set(70,30,0);
	LINE_ANGLE[8].arc_angle=360;
	//��0������
	LINE_ANGLE[9].lineStart.Set(0,0,0);
	LINE_ANGLE[9].lineEnd.Set(10,10,0);
	LINE_ANGLE[9].arc_angle=360;
	//��Բ�Ĵ���
	f3dArcLine arcLine;
	double radius=40,arc_angle=50;
	f3dPoint center(0,0,0),workNorm(0,0,1),columnNorm(0,1,1),arcStartPt(50,0,0),arcEndPt(-50,0,0),inters2d1Pt,inters2d2Pt,inters2d3Pt,inters2d4Pt;
	GEPOINT long_axis(3.4637098282877424e-015,68.284271247461902,11.715728752538091);	
	arcLine.CreateEllipse(center,arcStartPt,arcEndPt,columnNorm,workNorm,radius);
	//ֱ�ߵĻ���
	CDxfFile  dxfFile;
	dxfFile.OpenFile("D:\\����\\ֱ������Բ�����������.dxf");
	int nReceive[10];            //��������ս���ĸ���
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
		//log�ļ���ʾ
		if(j==0)
			logerr.Log("ͼ�ε�˳���Ǵ������ң��������ϣ�");
		if(nPtNum[j]!=nReceive[j])
			logerr.Log("���Դ���\t ����Ϊ%f��ӦΪ������ʵ�ʲ��Ը����ֱ�Ϊ ��%d\t%d",LINE_ANGLE[j].arc_angle,nPtNum[j],nReceive[j]);
		else
			logerr.Log("������ȷ��\t ����Ϊ%f��ӦΪ������ʵ�ʲ��Ը����ֱ�Ϊ ��%d\t%d",LINE_ANGLE[j].arc_angle,nPtNum[j],nReceive[j]);
	}
	//��Բ���εĻ���
	for(int j=0;j<10;j++)
	{
		double draw_angle=LINE_ANGLE[j].arc_angle/100*RADTODEG_COEF;
		f3dPoint startPt,endPt;
		startPt=arcLine.PositionInAngle(0);
		//�����߷ֳ�100�Σ�����ֱ�߻��Ƴ�ͼ 
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
//��Բ��Բ�󽻹��ܺ�������
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
	//��Բ��Բ������ϵ
	CIRCLE_INFO[0].arc_center.Set(0,0,0);
	CIRCLE_INFO[0].radius=30;
	//��Բ��Բ�����ϵ
	CIRCLE_INFO[1].arc_center.Set(0,80,0);
	CIRCLE_INFO[1].radius=20.0;
	//Բ������Բ���У�2�㣩��ϵ
	CIRCLE_INFO[2].arc_center.Set(0,0,0);
	CIRCLE_INFO[2].radius=100;
	//Բ������Բ���У�1�㣩��ϵ
	CIRCLE_INFO[3].arc_center.Set(30,0,0);
	CIRCLE_INFO[3].radius=130;
	//��Բ����Բ���У�2�㣩��ϵ
	CIRCLE_INFO[4].arc_center.Set(0,0,0);
	CIRCLE_INFO[4].radius=50;
	//��Բ����Բ���У�1�㣩��ϵ
	CIRCLE_INFO[5].arc_center.Set(0,20,0);
	CIRCLE_INFO[5].radius=80;
	//��Բ��Բ�ཻ��2�㣩��ϵ
	CIRCLE_INFO[6].arc_center.Set(0,30,0);
	CIRCLE_INFO[6].radius=40.0;
	//��Բ��Բ�ཻ��3�㣩��ϵ
	CIRCLE_INFO[7].arc_center.Set(0,20,0);
	CIRCLE_INFO[7].radius=70;
	//��Բ��Բ�ཻ��4�㣩��ϵ
	CIRCLE_INFO[8].arc_center.Set(0,0,0);
	CIRCLE_INFO[8].radius=70;
	//��Բ��Բ���й�ϵ
	CIRCLE_INFO[9].arc_center.Set(100,0,0);
	CIRCLE_INFO[9].radius=50;


	f3dPoint inters2d1Pt,inters2d2Pt,inters2d3Pt,inters2d4Pt;
	f3dPoint center(0,0,0),long_axis(100,0,0);
	double short_R=50,long_R=100;

	//Բ�Ļ���
	CDxfFile  dxfFile;
	dxfFile.OpenFile("D:\\����\\Բ����Բ�����������.dxf");
	int nReceive[10]; //������ĸ����浽������
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

	//��Բ�Ļ���
	int nSlices=50;                     //����50��
	double fDist=long_R/50;
	double x=0,y=0;
	f3dPoint startPt,endPt;
	for(int j=0;j<10;j++)
	{
		startPt.Set(long_R,0,0);             //��ʼ���Ƶ�
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
	//log�ļ���ʾ
	logerr.Log("��Բ��Բ������ϵ:\t\t%d\n��Բ��Բ�����ϵ:\t\t%d\nԲ������Բ���У�2�㣩��ϵ:\t%d",nReceive[0],nReceive[1],nReceive[2]);
	logerr.Log("Բ������Բ���У�1�㣩��ϵ:\t%d\n��Բ����Բ���У�2�㣩��ϵ:\t%d\n��Բ����Բ���У�1�㣩��ϵ:\t%d",nReceive[3],nReceive[4],nReceive[5]);
	logerr.Log("��Բ��Բ�ཻ��2�㣩��ϵ:\t%d \n��Բ��Բ�ཻ��3�㣩��ϵ:\t%d\n��Բ��Բ�ཻ��4�㣩��ϵ:\t%d",nReceive[6],nReceive[7],nReceive[8]);
	logerr.Log("��Բ��Բ���й�ϵ:\t\t%d",nReceive[9]);
	logerr.ShowToScreen();
	dxfFile.CloseFile();
}
	
	

	////��Բ�Ļ���
	//for(int j=0;j<10;j++)
	//{
	//	//�����߷ֳ�100�Σ�����ֱ�߻��Ƴ�ͼ 
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

