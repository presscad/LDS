#include "StdAfx.h"
#include "EmendBoltPosition.h"
#include "DxfFile.h"
#include "LogFile.h"
//////////////////////////////////////////////////////////////////////////
//AngleDgree
//////////////////////////////////////////////////////////////////////////
CEmendBoltPosition::CEmendBoltPosition(float fThick/*=0*/,float fDgree/*=0*/,float fWidth/*=0*/)
{
		m_fThick=fThick;
		m_fHalfDgree=fDgree/2;
		m_fWidth=fWidth;
		m_fEmendWidth=0;
}
//修改螺栓宽度
bool CEmendBoltPosition::EmendDistance()
{
	CLogErrorLife logErrLife;
	//检测宽度是否为0
	if(m_fWidth==0)
	{
		logerr.Log("输入的宽度不能为0!\n");
		return false;
	}
	//检测宽度与厚度是否正确
	if(abs(m_fWidth)<m_fThick)
	{
		logerr.Log("输入的宽度小于厚度!\n");
		return false;
	}
	//检测角度是否合理
	if(m_fHalfDgree>90)
	{
		logerr.Log("输入的角度过大!\n");
		return false;
	}
	//如果传来的角度为180或90度，则宽度不变
	if(m_fHalfDgree==90||m_fHalfDgree==45)
	{
		m_fEmendWidth=m_fWidth;
		return true;
	}
	//获取m_dgree的tan值
	double angle=m_fHalfDgree*RADTODEG_COEF;
	float tandgree=(float)tan(angle);
	//根据数学公式计算出宽度
	if(m_fWidth>0.000001)
		m_fEmendWidth=tandgree*m_fThick+abs(m_fWidth)-m_fThick;
	else
		m_fEmendWidth=-(tandgree*m_fThick+abs(m_fWidth)-m_fThick);
	return true;
}
//获取计算的厚度
float CEmendBoltPosition::GetEmendDistance()
{
	return m_fEmendWidth;
}
// 更改螺栓链表的posY
bool CEmendBoltPosition::EmendBoltPosY(ATOM_LIST<BOLT_INFO>& boltlist)
{
	if(boltlist.GetNodeNum()==0)
		return false;
	for(BOLT_INFO *pbolt=boltlist.GetFirst();pbolt!=NULL;pbolt=boltlist.GetNext())
	{
		m_fWidth=pbolt->posY;
		bool result=EmendDistance();
		if(result)
			pbolt->posY=m_fEmendWidth;
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
//TEST
//////////////////////////////////////////////////////////////////////////
void TestBolt(void)
{
	CFile file;
	int i=0,choice=0;
	try{
		file.Open("D:\\testWidth.txt",CFile::modeReadWrite|CFile::modeCreate);
		for(float fthick=1;fthick<20;++fthick)
			for(float fwidth=30;fwidth<200;++fwidth)
			{
				for(float fdgree=10;fdgree<180;++fdgree)
				{
					CEmendBoltPosition Bolt(fthick,fdgree,fwidth);
					Bolt.EmendDistance();
					CString emp;
					emp.Format("厚度:%lg,宽度:%lg,角度:%lg,修改后宽度为:%lg\r\n",fthick,fwidth,fdgree,Bolt.GetEmendDistance());
					file.Write(emp,emp.GetLength());
				}
			}
			file.Close();
			AfxMessageBox("成功写入到D:\testWidth.txt");
	}
	catch(CFileException* e)
	{
		CString str;
		str.Format("失败的原因是:%d",e->m_cause);
		AfxMessageBox(str);
		file.Abort();
		e->Delete();
	}
}
void TestBoltList(int dgree)
{
	CDxfFile dxf;
	CString path;
	path.Format("D:\\testbolt%d.dxf",dgree);
	BOLT_INFO* pnbolt=new BOLT_INFO[20];
	BOLT_INFO* pbolt=NULL;
	dxf.OpenFile(path);
	ATOM_LIST<BOLT_INFO> boltlist;
	CEmendBoltPosition angledgree(20,(float)dgree);
	for(int i=1;i<=20;++i)
	{
		pnbolt[i-1].posX=float(i*20);
		if(i%4==1)
			pnbolt[i-1].posY=25;
		else if(i%4==2)
			pnbolt[i-1].posY=-25;
		else if(i%4==3)
			pnbolt[i-1].posY=50;
		else
			pnbolt[i-1].posY=-50;	
		pnbolt[i-1].bolt_d=16;
		boltlist.append(pnbolt[i-1]);
	}
	f3dPoint pointbegin(0,0,0);
	f3dPoint pointend(500,0,0);
	dxf.NewLine(pointbegin,pointend);
	pointbegin=f3dPoint(500,-300,0);
	pointend=f3dPoint(500,300,0);
	dxf.NewLine(pointbegin,pointend);
	pbolt=boltlist.GetFirst();
	while(pbolt!=NULL)
	{
		pointbegin=f3dPoint(pbolt->posX,0,0);
		pointend=f3dPoint(pbolt->posX,pbolt->posY,0);
		dxf.NewLine(pointbegin,pointend);
		dxf.NewCircle(pointend,pbolt->bolt_d/2);
		pointbegin=f3dPoint(pbolt->posX,pbolt->posY,0);
		pointend=f3dPoint(500,pbolt->posY,0);
		dxf.NewLine(pointbegin,pointend);
		pbolt=boltlist.GetNext();
	}
	angledgree.EmendBoltPosY(boltlist);
	pointbegin=f3dPoint(500,0,0);
	pointend=f3dPoint(1000,0,0);
	dxf.NewLine(pointbegin,pointend);
	pbolt=boltlist.GetFirst();
	while(pbolt!=NULL)
	{
		pointbegin=f3dPoint(500+pbolt->posX,0,0);
		pointend=f3dPoint(500+pbolt->posX,pbolt->posY,0);
		dxf.NewLine(pointbegin,pointend);
		dxf.NewCircle(pointend,pbolt->bolt_d/2);
		pointbegin=f3dPoint(500+pbolt->posX,pbolt->posY,0);
		pointend=f3dPoint(500,pbolt->posY,0);
		dxf.NewLine(pointbegin,pointend);
		pbolt=boltlist.GetNext();
	}
	delete[] pnbolt;
	dxf.CloseFile();
	pnbolt=NULL;
	path="成功写入"+path;
	AfxMessageBox(path);
}
void TestBoltWidth(void)
{
	TestBoltList(88);
	TestBoltList(89);
	TestBoltList(90);
	TestBoltList(91);
	TestBoltList(92);
}